idf_component_register(
  SRCS "main.c" "wifi_events.c"
  INCLUDE_DIRS "."
  REQUIRES "nvs_flash" "esp_bsp" "lvgl_ui" "espressif__esp_lvgl_port"
           "esp_wifi" "esp_event" "esp_netif" "esp_timer"
)