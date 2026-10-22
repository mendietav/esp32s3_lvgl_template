#ifndef __SCREENS_UPDATE_H__
#define __SCREENS_UPDATE_H__

#include <stdint.h>

/**
 * Inter-task UI update queue.
 *
 * Other FreeRTOS tasks MUST NOT call LVGL functions directly. Instead they
 * call ui_update_cmd() to enqueue a command. The LVGL timer drains the queue
 * via ui_update_screens() each tick, already holding the LVGL lock.
 *
 * To add a new command:
 *   1. Add its value to ui_update_cmd_t below UI_UPDATE_MAX.
 *   2. Add a static handler function in screens_update.c.
 *   3. Register it in the s_handlers[] table.
 */

/* WiFi connection state sent to the UI */
typedef enum {
    UI_WIFI_CONNECTING,
    UI_WIFI_CONNECTED,
    UI_WIFI_FAILED,
} ui_wifi_state_t;

typedef enum {
    UI_UPDATE_WIFI_STATE,   /* data: (void *)(uintptr_t) ui_wifi_state_t */
    UI_UPDATE_RSSI,         /* data: (void *)(intptr_t) RSSI in dBm      */
    UI_UPDATE_MAX
} ui_update_cmd_t;

/** Initialize the update queue. Called once inside lvgl_ui_start(). */
void ui_update_init(void);

/**
 * Enqueue a UI update command from any task / ISR context.
 * data interpretation is command-specific; cast to/from (void*)(uintptr_t).
 */
void ui_update_cmd(ui_update_cmd_t cmd, void *data);

/** Drain the queue and execute pending handlers. Called from the LVGL tick. */
void ui_update_screens(void);

#endif