#include "scr_settings.h"
#include "screens.h"
#include "bsp_display.h"
#include "esp_log.h"
#include "bsp_button.h"

static const char *TAG = "scr_settings";

lv_obj_t *ui_scrSettings = NULL;

static lv_obj_t *s_lbl_brightness = NULL;
static lv_obj_t *s_slider         = NULL;

/* ──────────── callbacks ──────────── */

static void slider_event_cb(lv_event_t *e)
{
    (void)e;
    int val = lv_slider_get_value(s_slider);
    bsp_display_set_brightness((uint8_t)val);
    char buf[16];
    snprintf(buf, sizeof(buf), "Brillo: %d%%", val);
    lv_label_set_text(s_lbl_brightness, buf);
}

static void swipe_event_cb(lv_event_t *e)
{
    (void)e;
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    if (dir == LV_DIR_RIGHT) {
        ui_screens_navigate(SCREEN_SYSTEM);
    }
}

/* ──────────── ciclo de vida ──────────── */

void scr_settings_prepare(void)
{
    ui_scrSettings = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_scrSettings, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_scrSettings, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui_scrSettings, swipe_event_cb, LV_EVENT_GESTURE, NULL);

    /* Título */
    lv_obj_t *lbl_title = lv_label_create(ui_scrSettings);
    lv_label_set_text(lbl_title, "AJUSTES");
    lv_obj_align(lbl_title, LV_ALIGN_TOP_MID, 0, 10);

    /* Label brillo */
    s_lbl_brightness = lv_label_create(ui_scrSettings);
    lv_label_set_text(s_lbl_brightness, "Brillo: 100%");
    lv_obj_align(s_lbl_brightness, LV_ALIGN_CENTER, 0, -30);

    /* Slider de brillo */
    s_slider = lv_slider_create(ui_scrSettings);
    lv_obj_set_width(s_slider, 200);
    lv_slider_set_range(s_slider, 10, 100);
    lv_slider_set_value(s_slider, 100, LV_ANIM_OFF);
    lv_obj_align(s_slider, LV_ALIGN_CENTER, 0, 10);
    lv_obj_add_event_cb(s_slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    /* Navegación */
    lv_obj_t *lbl_nav = lv_label_create(ui_scrSettings);
    lv_label_set_text(lbl_nav, "< swipe para navegar >");
    lv_obj_align(lbl_nav, LV_ALIGN_BOTTOM_MID, 0, -10);

    ESP_LOGD(TAG, "prepare");
}

void scr_settings_init(void)
{
    ESP_LOGD(TAG, "init");
    /* Sincronizar slider con brillo actual */
    uint8_t current = bsp_display_get_brightness();
    lv_slider_set_value(s_slider, current, LV_ANIM_OFF);
    char buf[16];
    snprintf(buf, sizeof(buf), "Brillo: %d%%", current);
    lv_label_set_text(s_lbl_brightness, buf);
}

void scr_settings_uninit(void)
{
    ESP_LOGD(TAG, "uninit");
}

void scr_settings_step(void)
{
    if (bsp_button_was_pressed()) {
        ESP_LOGI(TAG, "button -> reset brightness");
        bsp_display_set_brightness(100);
        lv_slider_set_value(s_slider, 100, LV_ANIM_OFF);
        lv_label_set_text(s_lbl_brightness, "Brillo: 100%");
    }
}