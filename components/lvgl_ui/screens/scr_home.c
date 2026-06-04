#include "scr_home.h"
#include "screens.h"
#include "bsp_battery.h"
#include "esp_timer.h"
#include "driver/temperature_sensor.h"
#include "esp_log.h"
#include <stdio.h>
#include "bsp_button.h"

static const char *TAG = "scr_home";

lv_obj_t *ui_scrHome = NULL;

static lv_obj_t *s_lbl_bat    = NULL;
static lv_obj_t *s_lbl_temp   = NULL;
static lv_obj_t *s_lbl_uptime = NULL;
static lv_obj_t *s_lbl_nav    = NULL;

static lv_timer_t                  *s_timer       = NULL;
static temperature_sensor_handle_t  s_temp_handle = NULL;

/* ──────────── helpers ──────────── */

static int voltage_to_pct(float v)
{
    int pct = (int)((v - 3.0f) / 1.2f * 100.0f);
    if (pct < 0)   pct = 0;
    if (pct > 100) pct = 100;
    return pct;
}

static void update_all(void)
{
    char buf[32];

    /* Batería */
    float voltage = 0.0f;
    uint16_t raw = 0;
    bsp_battery_get_voltage(&voltage, &raw);
    snprintf(buf, sizeof(buf), "Bateria: %d%%", voltage_to_pct(voltage));
    lv_label_set_text(s_lbl_bat, buf);

    /* Temperatura */
    if (s_temp_handle) {
        float celsius = 0.0f;
        if (temperature_sensor_get_celsius(s_temp_handle, &celsius) == ESP_OK) {
            snprintf(buf, sizeof(buf), "Temp: %.1f C", celsius);
            lv_label_set_text(s_lbl_temp, buf);
        }
    }

    /* Uptime */
    int64_t us  = esp_timer_get_time();
    int total_s = (int)(us / 1000000LL);
    int h = total_s / 3600;
    int m = (total_s % 3600) / 60;
    int s = total_s % 60;
    snprintf(buf, sizeof(buf), "Uptime: %02d:%02d:%02d", h, m, s);
    lv_label_set_text(s_lbl_uptime, buf);
}

static void update_timer_cb(lv_timer_t *t)
{
    (void)t;
    update_all();
}

static void swipe_event_cb(lv_event_t *e)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    if (dir == LV_DIR_LEFT) {
        ui_screens_navigate(SCREEN_SYSTEM);
    }
}

/* ──────────── ciclo de vida ──────────── */

void scr_home_prepare(void)
{
    /* Sensor de temperatura */
    temperature_sensor_config_t ts_cfg = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 80);
    if (temperature_sensor_install(&ts_cfg, &s_temp_handle) == ESP_OK) {
        temperature_sensor_enable(s_temp_handle);
    } else {
        s_temp_handle = NULL;
    }

    ui_scrHome = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_scrHome, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_scrHome, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui_scrHome, swipe_event_cb, LV_EVENT_GESTURE, NULL);

    /* Título */
    lv_obj_t *lbl_title = lv_label_create(ui_scrHome);
    lv_label_set_text(lbl_title, "HOME");
    lv_obj_align(lbl_title, LV_ALIGN_TOP_MID, 0, 10);

    /* Labels de datos */
    s_lbl_bat = lv_label_create(ui_scrHome);
    lv_label_set_text(s_lbl_bat, "Bateria: --%");
    lv_obj_align(s_lbl_bat, LV_ALIGN_CENTER, 0, -30);

    s_lbl_temp = lv_label_create(ui_scrHome);
    lv_label_set_text(s_lbl_temp, "Temp: --.- C");
    lv_obj_align(s_lbl_temp, LV_ALIGN_CENTER, 0, 0);

    s_lbl_uptime = lv_label_create(ui_scrHome);
    lv_label_set_text(s_lbl_uptime, "Uptime: 00:00:00");
    lv_obj_align(s_lbl_uptime, LV_ALIGN_CENTER, 0, 30);

    /* Navegación */
    s_lbl_nav = lv_label_create(ui_scrHome);
    lv_label_set_text(s_lbl_nav, "< swipe para navegar >");
    lv_obj_align(s_lbl_nav, LV_ALIGN_BOTTOM_MID, 0, -10);

    lv_scr_load(ui_scrHome);

    ESP_LOGD(TAG, "prepare");
}

void scr_home_init(void)
{
    ESP_LOGD(TAG, "init");
    update_all();
    s_timer = lv_timer_create(update_timer_cb, 1000, NULL);
}

void scr_home_uninit(void)
{
    ESP_LOGD(TAG, "uninit");
    if (s_timer) {
        lv_timer_del(s_timer);
        s_timer = NULL;
    }
}

void scr_home_step(void)
{
    if (bsp_button_was_pressed()) {
        ESP_LOGI(TAG, "button -> refresh");
        update_all();
    }
}