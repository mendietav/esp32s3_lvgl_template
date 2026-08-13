#include "sensor_task.h"
#include "screens_update.h"
#include "driver/temperature_sensor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "sensor_task";

static void sensor_task(void *arg)
{
    (void)arg;

    /* Init sensor de temperatura */
    temperature_sensor_handle_t temp_handle = NULL;
    temperature_sensor_config_t ts_cfg = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 80);
    if (temperature_sensor_install(&ts_cfg, &temp_handle) == ESP_OK) {
        temperature_sensor_enable(temp_handle);
        ESP_LOGI(TAG, "temp sensor ok");
    } else {
        ESP_LOGE(TAG, "temp sensor failed");
        vTaskDelete(NULL);
        return;
    }

    while (1) {
        float celsius = 0.0f;
        if (temperature_sensor_get_celsius(temp_handle, &celsius) == ESP_OK) {
            /* Codificar como int32 (centesimas) para pasar por la cola */
            int32_t centesimas = (int32_t)(celsius * 100.0f);
            ui_update_cmd(UI_UPDATE_TEMPERATURA, (void *)(uintptr_t)centesimas);
            ESP_LOGD(TAG, "temp=%.1f", celsius);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void sensor_task_start(void)
{
    xTaskCreate(sensor_task, "sensor_task", 4096, NULL, 5, NULL);
    ESP_LOGI(TAG, "sensor task started");
}