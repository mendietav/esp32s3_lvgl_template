#include "scr_meter.h"
#include "screens.h"
#include "ui.h"
#include "esp_log.h"
#include <stdio.h>

static const char *TAG = "scr_meter";

static float s_threshold = 50.0f;
static float s_last_temp = 0.0f;

/* ──────────── swipe ──────────── */

static void swipe_cb(lv_event_t *e)
{
    (void)e;
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    if (dir == LV_DIR_LEFT) {
        ui_screens_navigate(SCREEN_CHART);
    }
}

/* ──────────── helpers ──────────── */

static void update_status(float celsius)
{
    if (celsius >= s_threshold + 10.0f) {
        lv_label_set_text(ui_lblStatus, "CRITICO");
        lv_obj_set_style_text_color(ui_lblStatus, lv_color_hex(0xF44336), LV_PART_MAIN);
        lv_obj_set_style_arc_color(ui_arcTemp, lv_color_hex(0xF44336), LV_PART_INDICATOR);
    } else if (celsius >= s_threshold) {
        lv_label_set_text(ui_lblStatus, "ALERTA");
        lv_obj_set_style_text_color(ui_lblStatus, lv_color_hex(0xFFC107), LV_PART_MAIN);
        lv_obj_set_style_arc_color(ui_arcTemp, lv_color_hex(0xFFC107), LV_PART_INDICATOR);
    } else {
        lv_label_set_text(ui_lblStatus, "NORMAL");
        lv_obj_set_style_text_color(ui_lblStatus, lv_color_hex(0x4CAF50), LV_PART_MAIN);
        lv_obj_set_style_arc_color(ui_arcTemp, lv_color_hex(0x4CAF50), LV_PART_INDICATOR);
    }
}

/* ──────────── API pública ──────────── */

void scr_meter_update_temp(float celsius)
{
    s_last_temp = celsius;
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f C", celsius);
    lv_label_set_text(ui_lblTemp, buf);

    int val = (int)celsius;
    if (val < 0)   val = 0;
    if (val > 100) val = 100;
    lv_arc_set_value(ui_arcTemp, val);

    update_status(celsius);
    ESP_LOGI(TAG, "temp=%.1f umbral=%.1f", celsius, s_threshold);
}

void scr_meter_set_threshold(float threshold)
{
    s_threshold = threshold;
    update_status(s_last_temp);
}

float scr_meter_get_threshold(void)
{
    return s_threshold;
}

/* ──────────── ciclo de vida ──────────── */

void scr_meter_prepare(void)
{
    ESP_LOGD(TAG, "prepare");
    lv_obj_add_flag(ui_scrMeter, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui_scrMeter, swipe_cb, LV_EVENT_GESTURE, NULL);
}

void scr_meter_init(void)
{
    ESP_LOGD(TAG, "init");
}

void scr_meter_uninit(void)
{
    ESP_LOGD(TAG, "uninit");
}

void scr_meter_step(void)
{
}