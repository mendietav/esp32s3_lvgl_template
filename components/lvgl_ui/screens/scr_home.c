#include "scr_home.h"
#include "screens.h"
#include "bsp_battery.h"
#include "esp_timer.h"
#include "driver/temperature_sensor.h"
#include "esp_log.h"
#include <stdio.h>

static const char *TAG = "scr_home";

lv_obj_t *ui_scrHome = NULL;

static lv_obj_t *s_arc_bat    = NULL;
static lv_obj_t *s_lbl_bat    = NULL;
static lv_obj_t *s_lbl_temp   = NULL;
static lv_obj_t *s_lbl_uptime = NULL;

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

static void set_arc_color(int pct)
{
    lv_color_t c;
    if      (pct > 50) c = lv_color_hex(0x4CAF50);
    else if (pct > 20) c = lv_color_hex(0xFFC107);
    else               c = lv_color_hex(0xF44336);
    lv_obj_set_style_arc_color(s_arc_bat, c, LV_PART_INDICATOR);
}

static void update_all(void)
{
    char buf[16];

    /* Batería */
    float voltage = 0.0f;
    uint16_t raw  = 0;
    bsp_battery_get_voltage(&voltage, &raw);
    int pct = voltage_to_pct(voltage);
    lv_arc_set_value(s_arc_bat, pct);
    set_arc_color(pct);
    snprintf(buf, sizeof(buf), "%d%%", pct);
    lv_label_set_text(s_lbl_bat, buf);

    /* Temperatura interna del chip */
    if (s_temp_handle) {
        float celsius = 0.0f;
        if (temperature_sensor_get_celsius(s_temp_handle, &celsius) == ESP_OK) {
            snprintf(buf, sizeof(buf), "%.1f\xC2\xB0""C", celsius);
            lv_label_set_text(s_lbl_temp, buf);
        }
    }

    /* Uptime HH:MM:SS */
    int64_t us  = esp_timer_get_time();
    int total_s = (int)(us / 1000000LL);
    int h = total_s / 3600;
    int m = (total_s % 3600) / 60;
    int s = total_s % 60;
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", h, m, s);
    lv_label_set_text(s_lbl_uptime, buf);
}

static void update_timer_cb(lv_timer_t *t)
{
    (void)t;
    update_all();
}

/* ──────────── ciclo de vida ──────────── */

void scr_home_prepare(void)
{
    /* Sensor de temperatura */
    temperature_sensor_config_t ts_cfg = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 80);
    if (temperature_sensor_install(&ts_cfg, &s_temp_handle) == ESP_OK) {
        temperature_sensor_enable(s_temp_handle);
        ESP_LOGI(TAG, "temp sensor ok");
    } else {
        ESP_LOGW(TAG, "temp sensor failed");
        s_temp_handle = NULL;
    }

    /* Pantalla base */
    ui_scrHome = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(ui_scrHome, lv_color_hex(0x0A0A14), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_scrHome, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(ui_scrHome, LV_OBJ_FLAG_SCROLLABLE);

    /* Widget 1: lv_arc — batería */
    s_arc_bat = lv_arc_create(ui_scrHome);
    lv_obj_set_size(s_arc_bat, 130, 130);
    lv_arc_set_range(s_arc_bat, 0, 100);
    lv_arc_set_value(s_arc_bat, 0);
    lv_arc_set_bg_angles(s_arc_bat, 135, 45);
    lv_arc_set_mode(s_arc_bat, LV_ARC_MODE_NORMAL);
    lv_obj_set_style_arc_color(s_arc_bat, lv_color_hex(0x1E1E30), LV_PART_MAIN);
    lv_obj_set_style_arc_color(s_arc_bat, lv_color_hex(0x4CAF50), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(s_arc_bat, 12, LV_PART_MAIN);
    lv_obj_set_style_arc_width(s_arc_bat, 12, LV_PART_INDICATOR);
    lv_obj_set_style_opa(s_arc_bat, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_align(s_arc_bat, LV_ALIGN_LEFT_MID, 6, 6);

    lv_obj_t *lbl_bat_title = lv_label_create(ui_scrHome);
    lv_label_set_text(lbl_bat_title, "BAT");
    lv_obj_set_style_text_color(lbl_bat_title, lv_color_hex(0x55507A), LV_PART_MAIN);
    lv_obj_align_to(lbl_bat_title, s_arc_bat, LV_ALIGN_CENTER, 0, -12);

    s_lbl_bat = lv_label_create(ui_scrHome);
    lv_label_set_text(s_lbl_bat, "--%");
    lv_obj_set_style_text_color(s_lbl_bat, lv_color_hex(0xE0D8FF), LV_PART_MAIN);
    lv_obj_align_to(s_lbl_bat, s_arc_bat, LV_ALIGN_CENTER, 0, 8);

    /* Widget 2: lv_label — temperatura */
    lv_obj_t *panel_temp = lv_obj_create(ui_scrHome);
    lv_obj_set_size(panel_temp, 145, 68);
    lv_obj_set_style_bg_color(panel_temp, lv_color_hex(0x12122A), LV_PART_MAIN);
    lv_obj_set_style_border_color(panel_temp, lv_color_hex(0x2A2A50), LV_PART_MAIN);
    lv_obj_set_style_border_width(panel_temp, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(panel_temp, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_all(panel_temp, 0, LV_PART_MAIN);
    lv_obj_clear_flag(panel_temp, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(panel_temp, LV_ALIGN_TOP_RIGHT, -6, 10);

    lv_obj_t *lbl_temp_title = lv_label_create(panel_temp);
    lv_label_set_text(lbl_temp_title, "TEMP CHIP");
    lv_obj_set_style_text_color(lbl_temp_title, lv_color_hex(0x55507A), LV_PART_MAIN);
    lv_obj_align(lbl_temp_title, LV_ALIGN_TOP_MID, 0, 8);

    s_lbl_temp = lv_label_create(panel_temp);
    lv_label_set_text(s_lbl_temp, "--.-\xC2\xB0""C");
    lv_obj_set_style_text_color(s_lbl_temp, lv_color_hex(0xFFB347), LV_PART_MAIN);
    lv_obj_align(s_lbl_temp, LV_ALIGN_CENTER, 0, 10);

    /* Widget 3: lv_label — uptime */
    lv_obj_t *panel_up = lv_obj_create(ui_scrHome);
    lv_obj_set_size(panel_up, 145, 68);
    lv_obj_set_style_bg_color(panel_up, lv_color_hex(0x12122A), LV_PART_MAIN);
    lv_obj_set_style_border_color(panel_up, lv_color_hex(0x2A2A50), LV_PART_MAIN);
    lv_obj_set_style_border_width(panel_up, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(panel_up, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_all(panel_up, 0, LV_PART_MAIN);
    lv_obj_clear_flag(panel_up, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(panel_up, LV_ALIGN_BOTTOM_RIGHT, -6, -10);

    lv_obj_t *lbl_up_title = lv_label_create(panel_up);
    lv_label_set_text(lbl_up_title, "UPTIME");
    lv_obj_set_style_text_color(lbl_up_title, lv_color_hex(0x55507A), LV_PART_MAIN);
    lv_obj_align(lbl_up_title, LV_ALIGN_TOP_MID, 0, 8);

    s_lbl_uptime = lv_label_create(panel_up);
    lv_label_set_text(s_lbl_uptime, "00:00:00");
    lv_obj_set_style_text_color(s_lbl_uptime, lv_color_hex(0x4FC3F7), LV_PART_MAIN);
    lv_obj_align(s_lbl_uptime, LV_ALIGN_CENTER, 0, 10);

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
}