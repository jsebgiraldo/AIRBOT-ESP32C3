#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize WiFi in Station mode
 * @return ESP_OK on success
 */
esp_err_t wifi_init_sta(void);

/**
 * @brief Check if WiFi is connected
 * @return true if connected, false otherwise
 */
bool wifi_is_connected(void);

/**
 * @brief Get current IP address
 * @param ip_str Buffer to store IP address string (min 16 bytes)
 * @return ESP_OK on success
 */
esp_err_t wifi_get_ip(char *ip_str, size_t len);

#ifdef __cplusplus
}
#endif

#endif // WIFI_MANAGER_H
