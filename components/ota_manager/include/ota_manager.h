#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include "esp_err.h"
#include "esp_http_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize OTA manager and start HTTP server for OTA updates
 * @return ESP_OK on success
 */
esp_err_t ota_manager_init(void);

/**
 * @brief Perform OTA update from URL
 * @param url URL to firmware binary
 * @return ESP_OK on success
 */
esp_err_t ota_update_from_url(const char *url);

/**
 * @brief Get current firmware version
 * @return Version string
 */
const char* ota_get_version(void);

/**
 * @brief Register OTA update endpoint in HTTP server
 * @param server HTTP server handle
 * @return ESP_OK on success
 */
esp_err_t ota_register_http_handler(httpd_handle_t server);

/**
 * @brief Start HTTP server for OTA updates
 * @return HTTP server handle or NULL on error
 */
httpd_handle_t ota_start_http_server(void);

#ifdef __cplusplus
}
#endif

#endif // OTA_MANAGER_H
