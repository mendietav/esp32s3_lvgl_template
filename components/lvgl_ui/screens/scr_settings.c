#include "scr_settings.h"
#include "screens.h"
#include "scr_meter.h"
#include "ui.h"
#include "esp_log.h"
#include <stdio.h>

static const char *TAG = "scr_settings";

/* ──────────── callbacks ──────────── */

static void slider_event_cb(lv_event_t *e)
{
    (void)e;
    int val = lv_slider_get_value(ui_sliderThreshold);
    scr_meter_set_threshold((float)val);
    char buf[20];
    snprintf(buf, sizeof(buf), "Umbral: %d C", val);
    lv_label_set_text(ui_lblThreshold, buf);
    ESP_LOGI(TAG, "threshold -> %d C", val);
}

static void swipe_event_cb(lv_event_t *e)
{
    (void)e;
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    if (dir == LV_DIR_RIGHT) {
        ui_screens_navigate(SCREEN_HISTORY);
    }
}

/* ──────────── ciclo de vida ──────────── */

void scr_settings_prepare(void)
{
    ESP_LOGD(TAG, "prepare");

    /* Configurar slider de umbral: rango 20-80°C */
    lv_slider_set_range(ui_sliderThreshold, 20, 80);
    lv_slider_set_value(ui_sliderThreshold, 50, LV_ANIM_OFF);
    lv_obj_add_event_cb(ui_sliderThreshold, slider_event_cb,
                        LV_EVENT_VALUE_CHANGED, NULL);

    /* Swipe para navegar */
    lv_obj_add_flag(ui_scrSettings, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui_scrSettings, swipe_event_cb,
                        LV_EVENT_GESTURE, NULL);
}

void scr_settings_init(void)
{
    ESP_LOGD(TAG, "init");
    float threshold = scr_meter_get_threshold();
    lv_slider_set_value(ui_sliderThreshold, (int)threshold, LV_ANIM_OFF);
    char buf[20];
    snprintf(buf, sizeof(buf), "Umbral: %d C", (int)threshold);
    lv_label_set_text(ui_lblThreshold, buf);
}

void scr_settings_uninit(void)
{
    ESP_LOGD(TAG, "uninit");
}

void scr_settings_step(void)
{
}