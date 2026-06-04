#include "scr_system.h"
#include "screens.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_idf_version.h"
#include "esp_chip_info.h"
#include <stdio.h>
#include "bsp_button.h"
#include "esp_system.h"

static const char *TAG = "scr_system";

lv_obj_t *ui_scrSystem = NULL;

static lv_obj_t *s_lbl_heap     = NULL;
static lv_obj_t *s_lbl_heap_min = NULL;
static lv_timer_t *s_timer      = NULL;

static uint32_t s_heap_min = UINT32_MAX;

/* ──────────── helpers ──────────── */

static void update_all(void)
{
    char buf[48];

    uint32_t heap = esp_get_free_heap_size();
    if (heap < s_heap_min) s_heap_min = heap;

    snprintf(buf, sizeof(buf), "Heap libre: %lu bytes", heap);
    lv_label_set_text(s_lbl_heap, buf);

    snprintf(buf, sizeof(buf), "Heap min: %lu bytes", s_heap_min);
    lv_label_set_text(s_lbl_heap_min, buf);
}

static void update_timer_cb(lv_timer_t *t)
{
    (void)t;
    update_all();
}

static void swipe_event_cb(lv_event_t *e)
{
    (void)e;
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    if (dir == LV_DIR_RIGHT) {
        ui_screens_navigate(SCREEN_HOME);
    } else if (dir == LV_DIR_LEFT) {
        ui_screens_navigate(SCREEN_SETTINGS);
    }
}

/* ──────────── ciclo de vida ──────────── */

void scr_system_prepare(void)
{
    ui_scrSystem = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_scrSystem, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_scrSystem, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui_scrSystem, swipe_event_cb, LV_EVENT_GESTURE, NULL);

    /* Título */
    lv_obj_t *lbl_title = lv_label_create(ui_scrSystem);
    lv_label_set_text(lbl_title, "SISTEMA");
    lv_obj_align(lbl_title, LV_ALIGN_TOP_MID, 0, 10);

    /* Versión IDF */
    lv_obj_t *lbl_idf = lv_label_create(ui_scrSystem);
    char buf[48];
    snprintf(buf, sizeof(buf), "IDF: %s", esp_get_idf_version());
    lv_label_set_text(lbl_idf, buf);
    lv_obj_align(lbl_idf, LV_ALIGN_CENTER, 0, -50);

    /* Chip info */
    esp_chip_info_t chip;
    esp_chip_info(&chip);
    lv_obj_t *lbl_chip = lv_label_create(ui_scrSystem);
    snprintf(buf, sizeof(buf), "Cores: %d  Rev: %d", chip.cores, chip.revision);
    lv_label_set_text(lbl_chip, buf);
    lv_obj_align(lbl_chip, LV_ALIGN_CENTER, 0, -20);

    /* Heap libre */
    s_lbl_heap = lv_label_create(ui_scrSystem);
    lv_label_set_text(s_lbl_heap, "Heap libre: --");
    lv_obj_align(s_lbl_heap, LV_ALIGN_CENTER, 0, 10);

    /* Heap mínimo */
    s_lbl_heap_min = lv_label_create(ui_scrSystem);
    lv_label_set_text(s_lbl_heap_min, "Heap min: --");
    lv_obj_align(s_lbl_heap_min, LV_ALIGN_CENTER, 0, 40);

    /* Navegación */
    lv_obj_t *lbl_nav = lv_label_create(ui_scrSystem);
    lv_label_set_text(lbl_nav, "< swipe para navegar >");
    lv_obj_align(lbl_nav, LV_ALIGN_BOTTOM_MID, 0, -10);

    ESP_LOGD(TAG, "prepare");
}

void scr_system_init(void)
{
    ESP_LOGD(TAG, "init");
    s_heap_min = esp_get_free_heap_size();
    update_all();
    s_timer = lv_timer_create(update_timer_cb, 1000, NULL);
}

void scr_system_uninit(void)
{
    ESP_LOGD(TAG, "uninit");
    if (s_timer) {
        lv_timer_del(s_timer);
        s_timer = NULL;
    }
}

void scr_system_step(void)
{
    if (bsp_button_was_pressed()) {
        ESP_LOGI(TAG, "button -> reset heap min");
        s_heap_min = esp_get_free_heap_size();
        update_all();
    }
}