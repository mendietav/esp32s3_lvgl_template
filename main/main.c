#include "esp_lvgl_port.h"
#include "nvs_flash.h"

#include "bsp_i2c.h"
#include "bsp_display.h"
#include "bsp_touch.h"
#include "bsp_lvgl.h"

#include "lvgl_ui.h"
#include "wifi_events.h"

/** @brief App entry point */
void app_main(void) {
    // The WiFi driver stores calibration data in NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Initialize necessary peripherals to handle display and touch
    i2c_master_bus_handle_t i2c_bus = bsp_i2c_init();
    bsp_display_init();
    bsp_touch_init(i2c_bus);
    ESP_ERROR_CHECK(bsp_lvgl_init());
    bsp_display_brightness_init();
    bsp_display_set_brightness(100);
    // Always take LVGL mutex before doing anything with LVGL
    if (lvgl_port_lock(0)) {
        lvgl_ui_start();
        lvgl_port_unlock();
    }

    // After lvgl_ui_start(): the UI update queue already exists
    wifi_events_start();
}