#ifndef __WIFI_EVENTS_H__
#define __WIFI_EVENTS_H__

/**
 * Initialize the WiFi driver, register our event handler and start
 * connecting to the school network (open, no password).
 * Call after lvgl_ui_start() so the UI update queue already exists.
 */
void wifi_events_start(void);

#endif