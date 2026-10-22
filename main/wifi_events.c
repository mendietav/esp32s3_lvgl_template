#include "wifi_events.h"
#include <string.h>
#include "bsp_wifi.h"
#include "screens_update.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_timer.h"
#include "esp_wifi.h"

/* School network: open (no password), 2.4 GHz */
#define WIFI_SSID "Cooperadora Alumnos"

#define WIFI_MAX_RETRY 5
#define RSSI_PERIOD_US (2 * 1000 * 1000)

static const char *TAG = "wifi_events";
static int s_retry = 0;

static void send_state(ui_wifi_state_t state)
{
    ui_update_cmd(UI_UPDATE_WIFI_STATE, (void *)(uintptr_t)state);
}

/* Runs in the esp_timer task: only enqueue, never call lv_* */
static void rssi_timer_cb(void *arg)
{
    wifi_ap_record_t ap;
    if (esp_wifi_sta_get_ap_info(&ap) == ESP_OK) {
        ui_update_cmd(UI_UPDATE_RSSI, (void *)(intptr_t)ap.rssi);
    }
}

/* Runs in the default event loop task, NOT in the LVGL context */
static void wifi_event_cb(void *arg, esp_event_base_t base,
                          int32_t id, void *data)
{
    if (base == WIFI_EVENT) {
        switch (id) {
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "STA_START -> connecting");
            send_state(UI_WIFI_CONNECTING);
            break;
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "STA_CONNECTED");
            break;
        case WIFI_EVENT_STA_DISCONNECTED: {
            wifi_event_sta_disconnected_t *ev = (wifi_event_sta_disconnected_t *)data;
            ESP_LOGW(TAG, "STA_DISCONNECTED reason=%d", ev->reason);
            if (s_retry < WIFI_MAX_RETRY) {
                s_retry++;
                ESP_LOGI(TAG, "retry %d/%d", s_retry, WIFI_MAX_RETRY);
                send_state(UI_WIFI_CONNECTING);
                esp_wifi_connect();
            } else {
                ESP_LOGE(TAG, "giving up after %d retries", WIFI_MAX_RETRY);
                send_state(UI_WIFI_FAILED);
            }
            break;
        }
        default:
            break;
        }
    } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *ev = (ip_event_got_ip_t *)data;
        s_retry = 0;
        ESP_LOGI(TAG, "GOT_IP " IPSTR, IP2STR(&ev->ip_info.ip));
        send_state(UI_WIFI_CONNECTED);
    }
}

void wifi_events_start(void)
{
    /* NULL credentials: initializes the driver but does NOT connect, so
     * we can register our handler before the first event fires. */
    bsp_wifi_init(NULL, NULL);

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_cb, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_cb, NULL, NULL));

    /* RSSI every 2 s (only sends while connected) */
    const esp_timer_create_args_t rssi_args = {
        .callback = rssi_timer_cb,
        .name = "rssi",
    };
    esp_timer_handle_t rssi_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&rssi_args, &rssi_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(rssi_timer, RSSI_PERIOD_US));

    /* Open network: accept any security level, no password */
    wifi_config_t cfg = {0};
    strncpy((char *)cfg.sta.ssid, WIFI_SSID, sizeof(cfg.sta.ssid) - 1);
    cfg.sta.threshold.authmode = WIFI_AUTH_OPEN;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &cfg));
    /* STA_START triggers esp_wifi_connect() inside bsp_wifi's own handler */
    ESP_ERROR_CHECK(esp_wifi_start());
}