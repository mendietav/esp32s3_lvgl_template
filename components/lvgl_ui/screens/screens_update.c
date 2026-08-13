#include "screens_update.h"
#include "scr_meter.h"
#include "scr_chart.h"
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

#define UPDATE_QUEUE_DEPTH 16

static const char *TAG = "screens_update";

typedef struct {
    ui_update_cmd_t cmd;
    void           *data;
} ui_cmd_msg_t;

typedef void (*handler_fn_t)(void *data);

static QueueHandle_t s_queue = NULL;

static void handle_temperatura(void *data)
{
    int32_t centesimas = (int32_t)(uintptr_t)data;
    float celsius = centesimas / 100.0f;
    scr_meter_update_temp(celsius);
    scr_chart_add_point(celsius);
}

static const handler_fn_t s_handlers[UI_UPDATE_MAX] = {
    [UI_UPDATE_TEMPERATURA] = handle_temperatura,
};

void ui_update_init(void)
{
    s_queue = xQueueCreate(UPDATE_QUEUE_DEPTH, sizeof(ui_cmd_msg_t));
    ESP_ERROR_CHECK(s_queue ? ESP_OK : ESP_FAIL);
}

void ui_update_cmd(ui_update_cmd_t cmd, void *data)
{
    if (!s_queue) return;
    ui_cmd_msg_t msg = { .cmd = cmd, .data = data };
    if (xQueueSend(s_queue, &msg, 0) != pdTRUE) {
        ESP_LOGW(TAG, "queue full, dropping cmd %d", (int)cmd);
    }
}

void ui_update_screens(void)
{
    if (!s_queue) return;
    ui_cmd_msg_t msg;
    while (xQueueReceive(s_queue, &msg, 0) == pdTRUE) {
        if (msg.cmd < UI_UPDATE_MAX && s_handlers[msg.cmd]) {
            s_handlers[msg.cmd](msg.data);
        }
    }
}