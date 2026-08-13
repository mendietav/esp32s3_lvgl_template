#include "scr_history.h"
#include "screens.h"
#include "ui.h"
#include "esp_log.h"
#include <stdio.h>

static const char *TAG = "scr_history";

#define MAX_ALERTS 10

static lv_obj_t *s_rows[MAX_ALERTS] = {0};
static int       s_count            = 0;

/* ──────────── API pública ──────────── */

void scr_history_add_alert(float celsius, int uptime_s)
{
    int h = uptime_s / 3600;
    int m = (uptime_s % 3600) / 60;
    int s = uptime_s % 60;

    char buf[32];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d  %.1fC", h, m, s, celsius);

    if (s_count < MAX_ALERTS) {
        /* Agregar nueva fila */
        lv_obj_t *lbl = lv_label_create(ui_panelHistory);
        lv_label_set_text(lbl, buf);
        lv_obj_set_style_text_color(lbl, lv_color_hex(0xFFB347), LV_PART_MAIN);
        lv_obj_set_width(lbl, 150);
        s_rows[s_count] = lbl;
        s_count++;
    } else {
        /* Descartar la más antigua, mover las demás */
        lv_obj_del(s_rows[0]);
        for (int i = 0; i < MAX_ALERTS - 1; i++) {
            s_rows[i] = s_rows[i + 1];
        }
        lv_obj_t *lbl = lv_label_create(ui_panelHistory);
        lv_label_set_text(lbl, buf);
        lv_obj_set_style_text_color(lbl, lv_color_hex(0xFFB347), LV_PART_MAIN);
        lv_obj_set_width(lbl, 150);
        s_rows[MAX_ALERTS - 1] = lbl;
    }

    ESP_LOGI(TAG, "alert added: %s", buf);
}

static void swipe_cb(lv_event_t *e)
{
    (void)e;
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    if (dir == LV_DIR_LEFT) {
        ui_screens_navigate(SCREEN_SETTINGS);
    } else if (dir == LV_DIR_RIGHT) {
        ui_screens_navigate(SCREEN_CHART);
    }
}

/* ──────────── ciclo de vida ──────────── */

void scr_history_prepare(void)
{
    ESP_LOGD(TAG, "prepare");

    /* Configurar panel con layout vertical */
    lv_obj_set_flex_flow(ui_panelHistory, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(ui_panelHistory, 4, LV_PART_MAIN);
    lv_obj_add_flag(ui_scrHistory, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui_scrHistory, swipe_cb, LV_EVENT_GESTURE, NULL);
}

void scr_history_init(void)
{
    ESP_LOGD(TAG, "init");
}

void scr_history_uninit(void)
{
    ESP_LOGD(TAG, "uninit");
}

void scr_history_step(void)
{
}