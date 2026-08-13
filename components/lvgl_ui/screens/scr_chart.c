#include "scr_chart.h"
#include "screens.h"
#include "ui.h"
#include "esp_log.h"
#include <stdio.h>

static const char *TAG = "scr_chart";

#define CHART_POINTS 60

static lv_chart_series_t *s_series = NULL;
static float s_min = 999.0f;
static float s_max = -999.0f;

/* ──────────── API pública ──────────── */

void scr_chart_add_point(float celsius)
{
    if (!s_series) return;

    /* Actualizar min/max */
    if (celsius < s_min) s_min = celsius;
    if (celsius > s_max) s_max = celsius;

    /* Agregar punto al gráfico (valor * 10 para tener un decimal) */
    lv_chart_set_next_value(ui_chartTemp, s_series, (lv_coord_t)(celsius * 10));

    /* Actualizar labels */
    char buf[16];
    snprintf(buf, sizeof(buf), "Min: %.1f", s_min);
    lv_label_set_text(ui_lblMin, buf);
    snprintf(buf, sizeof(buf), "Max: %.1f", s_max);
    lv_label_set_text(ui_lblMax, buf);
}

static void swipe_cb(lv_event_t *e)
{
    (void)e;
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    if (dir == LV_DIR_LEFT) {
        ui_screens_navigate(SCREEN_HISTORY);
    } else if (dir == LV_DIR_RIGHT) {
        ui_screens_navigate(SCREEN_METER);
    }
}

/* ──────────── ciclo de vida ──────────── */

void scr_chart_prepare(void)
{
    ESP_LOGD(TAG, "prepare");

    /* Configurar el chart */
    lv_chart_set_type(ui_chartTemp, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(ui_chartTemp, CHART_POINTS);
    lv_chart_set_update_mode(ui_chartTemp, LV_CHART_UPDATE_MODE_SHIFT);

    /* Rango Y: 0 a 1000 (temperatura * 10) */
    lv_chart_set_range(ui_chartTemp, LV_CHART_AXIS_PRIMARY_Y, 0, 1000);

    /* Crear serie */
    s_series = lv_chart_add_series(ui_chartTemp, lv_color_hex(0x4FC3F7),
                                   LV_CHART_AXIS_PRIMARY_Y);
                                   lv_obj_add_flag(ui_scrChart, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui_scrChart, swipe_cb, LV_EVENT_GESTURE, NULL);
}

void scr_chart_init(void)
{
    ESP_LOGD(TAG, "init");
}

void scr_chart_uninit(void)
{
    ESP_LOGD(TAG, "uninit");
}

void scr_chart_step(void)
{
}