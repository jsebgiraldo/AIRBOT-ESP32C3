#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "wifi_manager.h"
#include "ota_manager.h"

static const char *TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "Starting AIRBOT ESP32-C3 Firmware v%s", ota_get_version());

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize OTA manager
    ESP_ERROR_CHECK(ota_manager_init());

    // Initialize WiFi
    ESP_LOGI(TAG, "Connecting to WiFi...");
    ret = wifi_init_sta();
    
    if (ret == ESP_OK) {
        char ip_str[16];
        if (wifi_get_ip(ip_str, sizeof(ip_str)) == ESP_OK) {
            ESP_LOGI(TAG, "Connected! IP Address: %s", ip_str);
            ESP_LOGI(TAG, "OTA Update endpoints:");
            ESP_LOGI(TAG, "  - POST http://%s:8080/ota (with URL in body)", ip_str);
            ESP_LOGI(TAG, "  - POST http://%s:8080/upload (with firmware binary)", ip_str);
            ESP_LOGI(TAG, "  - GET  http://%s:8080/version", ip_str);
        }

        // Start HTTP server for OTA updates
        httpd_handle_t server = ota_start_http_server();
        if (server == NULL) {
            ESP_LOGE(TAG, "Failed to start HTTP server");
        } else {
            ESP_LOGI(TAG, "OTA HTTP server started successfully");
        }
    } else {
        ESP_LOGE(TAG, "Failed to connect to WiFi");
    }

    // Main application loop
    while (1) {
        // Your application code here
        ESP_LOGI(TAG, "AIRBOT running... (Firmware v%s)", ota_get_version());
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
