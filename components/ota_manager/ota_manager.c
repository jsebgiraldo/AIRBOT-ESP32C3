#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_app_format.h"
#include "esp_http_server.h"

#include "ota_manager.h"

static const char *TAG = "ota_manager";

#define OTA_URL_SIZE 256
#define FIRMWARE_VERSION "1.0.0"

static char ota_url_buffer[OTA_URL_SIZE];
static httpd_handle_t server = NULL;

const char* ota_get_version(void)
{
    return FIRMWARE_VERSION;
}

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        break;
    }
    return ESP_OK;
}

static void ota_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting OTA update from: %s", (char *)pvParameter);

    esp_http_client_config_t config = {
        .url = (char *)pvParameter,
        .event_handler = _http_event_handler,
        .keep_alive_enable = true,
    };

    esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };

    ESP_LOGI(TAG, "Attempting to download update from %s", config.url);
    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "OTA Succeed, Rebooting...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        esp_restart();
    } else {
        ESP_LOGE(TAG, "Firmware upgrade failed");
    }
    vTaskDelete(NULL);
}

esp_err_t ota_update_from_url(const char *url)
{
    if (url == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    strncpy(ota_url_buffer, url, OTA_URL_SIZE - 1);
    ota_url_buffer[OTA_URL_SIZE - 1] = '\0';

    xTaskCreate(&ota_task, "ota_task", 8192, ota_url_buffer, 5, NULL);
    return ESP_OK;
}

/* HTTP handler for OTA update */
static esp_err_t ota_post_handler(httpd_req_t *req)
{
    char buf[256];
    int ret, remaining = req->content_len;

    if (remaining > sizeof(buf) - 1) {
        httpd_resp_send_err(req, HTTPD_414_URI_TOO_LONG, "URL too long");
        return ESP_FAIL;
    }

    while (remaining > 0) {
        if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;
            }
            return ESP_FAIL;
        }
        remaining -= ret;
    }
    buf[req->content_len] = '\0';

    ESP_LOGI(TAG, "Received OTA URL: %s", buf);

    esp_err_t err = ota_update_from_url(buf);
    if (err == ESP_OK) {
        httpd_resp_sendstr(req, "OTA update started");
    } else {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to start OTA");
    }

    return err;
}

/* HTTP handler for firmware upload */
static esp_err_t ota_upload_handler(httpd_req_t *req)
{
    esp_ota_handle_t ota_handle;
    const esp_partition_t *update_partition = NULL;
    esp_err_t err;

    update_partition = esp_ota_get_next_update_partition(NULL);
    if (update_partition == NULL) {
        ESP_LOGE(TAG, "No OTA partition found");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "No OTA partition");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%"PRIx32,
             update_partition->subtype, update_partition->address);

    err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin failed (%s)", esp_err_to_name(err));
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "OTA begin failed");
        return ESP_FAIL;
    }

    char *buf = malloc(1024);
    if (buf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for OTA buffer");
        esp_ota_abort(ota_handle);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Memory allocation failed");
        return ESP_FAIL;
    }

    int received;
    int remaining = req->content_len;

    while (remaining > 0) {
        if ((received = httpd_req_recv(req, buf, MIN(remaining, 1024))) <= 0) {
            if (received == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;
            }
            free(buf);
            esp_ota_abort(ota_handle);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive data");
            return ESP_FAIL;
        }

        err = esp_ota_write(ota_handle, (const void *)buf, received);
        if (err != ESP_OK) {
            free(buf);
            esp_ota_abort(ota_handle);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "OTA write failed");
            return ESP_FAIL;
        }
        remaining -= received;
    }

    free(buf);

    err = esp_ota_end(ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_end failed (%s)", esp_err_to_name(err));
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "OTA end failed");
        return ESP_FAIL;
    }

    err = esp_ota_set_boot_partition(update_partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed (%s)", esp_err_to_name(err));
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Set boot partition failed");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "OTA update successful, preparing to restart...");
    httpd_resp_sendstr(req, "OTA update successful, rebooting...");

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    esp_restart();

    return ESP_OK;
}

/* HTTP handler for version info */
static esp_err_t version_handler(httpd_req_t *req)
{
    char version_info[128];
    snprintf(version_info, sizeof(version_info), 
             "{\"version\":\"%s\",\"idf_version\":\"%s\"}", 
             FIRMWARE_VERSION, esp_get_idf_version());
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, version_info);
    return ESP_OK;
}

esp_err_t ota_register_http_handler(httpd_handle_t srv)
{
    if (srv == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    httpd_uri_t ota_uri = {
        .uri       = "/ota",
        .method    = HTTP_POST,
        .handler   = ota_post_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t upload_uri = {
        .uri       = "/upload",
        .method    = HTTP_POST,
        .handler   = ota_upload_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t version_uri = {
        .uri       = "/version",
        .method    = HTTP_GET,
        .handler   = version_handler,
        .user_ctx  = NULL
    };

    httpd_register_uri_handler(srv, &ota_uri);
    httpd_register_uri_handler(srv, &upload_uri);
    httpd_register_uri_handler(srv, &version_uri);

    return ESP_OK;
}

httpd_handle_t ota_start_http_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 8080;
    config.max_uri_handlers = 10;

    ESP_LOGI(TAG, "Starting HTTP server on port: %d", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        ota_register_http_handler(server);
        return server;
    }

    ESP_LOGE(TAG, "Error starting HTTP server!");
    return NULL;
}

esp_err_t ota_manager_init(void)
{
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;

    ESP_LOGI(TAG, "OTA Manager initialized");
    ESP_LOGI(TAG, "Running partition type %d subtype %d (offset 0x%08"PRIx32")",
             running->type, running->subtype, running->address);
    ESP_LOGI(TAG, "Firmware version: %s", FIRMWARE_VERSION);

    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            ESP_LOGI(TAG, "An OTA update has been performed. Validating...");
            esp_ota_mark_app_valid_cancel_rollback();
        }
    }

    return ESP_OK;
}
