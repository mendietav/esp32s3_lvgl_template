#include "wifi_events.h"
#include <string.h>
#include "bsp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"

/* School network: open (no password), 2.4 GHz */
#define WIFI_SSID "Cooperadora Alumnos"

#define WIFI_MAX_RETRY 5

static const char *TAG = "wifi_events";
static int s_retry = 0;

/* Runs in the default event loop task, NOT in the LVGL context:
 * only log or enqueue commands here, never call lv_* functions. */
static void wifi_event_cb(void *arg, esp_event_base_t base,
                          int32_t id, void *data)
{
    if (base == WIFI_EVENT) {
        switch (id) {
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "STA_START -> connecting");
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
                esp_wifi_connect();
            } else {
                ESP_LOGE(TAG, "giving up after %d retries", WIFI_MAX_RETRY);
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

    /* Open network: accept any security level, no password */
    wifi_config_t cfg = {0};
    strncpy((char *)cfg.sta.ssid, WIFI_SSID, sizeof(cfg.sta.ssid) - 1);
    cfg.sta.threshold.authmode = WIFI_AUTH_OPEN;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &cfg));
    /* STA_START triggers esp_wifi_connect() inside bsp_wifi's own handler */
    ESP_ERROR_CHECK(esp_wifi_start());
}