#include "bsp_button.h"
#include "iot_button.h"
#include "button_gpio.h"
#include "esp_log.h"
#include <stdbool.h>

static const char *TAG = "bsp_button";

#define BOOT_BUTTON_GPIO    0

static button_handle_t s_btn_handle = NULL;
static volatile bool   s_pressed    = false;

static void button_press_cb(void *arg, void *data)
{
    (void)arg;
    (void)data;
    s_pressed = true;
    ESP_LOGD(TAG, "BOOT pressed");
}

void bsp_button_init(void)
{
    button_config_t btn_cfg = {
        .short_press_time = 50,
        .long_press_time  = 1000,
    };
    button_gpio_config_t gpio_cfg = {
        .gpio_num     = BOOT_BUTTON_GPIO,
        .active_level = 0,
        .enable_power_save = false,
        .disable_pull      = false,
    };
    iot_button_new_gpio_device(&btn_cfg, &gpio_cfg, &s_btn_handle);
    iot_button_register_cb(s_btn_handle, BUTTON_SINGLE_CLICK, NULL, button_press_cb, NULL);
    ESP_LOGI(TAG, "BOOT button init ok (GPIO%d)", BOOT_BUTTON_GPIO);
}

bool bsp_button_was_pressed(void)
{
    if (s_pressed) {
        s_pressed = false;
        return true;
    }
    return false;
}