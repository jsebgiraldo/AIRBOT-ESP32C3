idf_component_register(
    SRCS "main.c" "wifi_manager.c"
    INCLUDE_DIRS "."
    REQUIRES 
        nvs_flash
        esp_wifi
        esp_http_server
        esp_https_ota
        esp_app_format
        bootloader_support
        ota_manager
)
