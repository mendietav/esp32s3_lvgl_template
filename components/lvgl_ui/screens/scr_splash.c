#include "scr_splash.h"
#include "screens.h"
#include "bsp_display.h"
#include "esp_log.h"

static const char *TAG = "scr_splash";

lv_obj_t *ui_scrSplash = NULL;

static lv_obj_t   *s_lbl_anim  = NULL;
static lv_timer_t *s_dot_timer = NULL;
static int         s_dot_state = 0;
static bool        s_can_touch = false;

/* ──────────── callbacks ──────────── */

static void backlight_anim_cb(void *var, int32_t value)
{
    (void)var;
    bsp_display_set_brightness((uint8_t)value);
}

static void backlight_ready_cb(lv_anim_t *a)
{
    (void)a;
    s_can_touch = true;
}

static void dot_timer_cb(lv_timer_t *t)
{
    (void)t;
    static const char * const frames[] = {
        "Iniciando",
        "Iniciando.",
        "Iniciando..",
        "Iniciando..."
    };
    s_dot_state = (s_dot_state + 1) % 4;
    lv_label_set_text(s_lbl_anim, frames[s_dot_state]);
}

static void on_splash_clicked(lv_event_t *e)
{
    (void)e;
    if (!s_can_touch) return;
    ESP_LOGI(TAG, "touch -> SCREEN_HOME");
    ui_screens_navigate(SCREEN_HOME);
}

/* ──────────── ciclo de vida ──────────── */

void scr_splash_prepare(void)
{
    bsp_display_set_brightness(0);

    ui_scrSplash = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(ui_scrSplash, lv_color_hex(0x0A0A14), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_scrSplash, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(ui_scrSplash, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_scrSplash, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui_scrSplash, on_splash_clicked, LV_EVENT_CLICKED, NULL);

    s_lbl_anim = lv_label_create(ui_scrSplash);
    lv_label_set_text(s_lbl_anim, "Iniciando");
    lv_obj_set_style_text_color(s_lbl_anim, lv_color_hex(0xCCBBFF), LV_PART_MAIN);
    lv_obj_align(s_lbl_anim, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *lbl_hint = lv_label_create(ui_scrSplash);
    lv_label_set_text(lbl_hint, "toca para continuar");
    lv_obj_set_style_text_color(lbl_hint, lv_color_hex(0x2A2A44), LV_PART_MAIN);
    lv_obj_align(lbl_hint, LV_ALIGN_BOTTOM_MID, 0, -6);

    lv_scr_load(ui_scrSplash);

    ESP_LOGD(TAG, "prepare");
}

void scr_splash_init(void)
{
    ESP_LOGD(TAG, "init");
    s_can_touch = false;
    s_dot_state = 0;
    lv_label_set_text(s_lbl_anim, "Iniciando");

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, backlight_anim_cb);
    lv_anim_set_var(&a, NULL);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_time(&a, 800);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_set_ready_cb(&a, backlight_ready_cb);
    lv_anim_start(&a);

    s_dot_timer = lv_timer_create(dot_timer_cb, 500, NULL);
}

void scr_splash_uninit(void)
{
    ESP_LOGD(TAG, "uninit");
    if (s_dot_timer) {
        lv_timer_del(s_dot_timer);
        s_dot_timer = NULL;
    }
}

void scr_splash_step(void)
{
}