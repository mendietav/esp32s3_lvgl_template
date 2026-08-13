#ifndef __SCR_HISTORY_H__
#define __SCR_HISTORY_H__

#include "lvgl.h"

void scr_history_prepare(void);
void scr_history_init(void);
void scr_history_uninit(void);
void scr_history_step(void);

void scr_history_add_alert(float celsius, int uptime_s);

#endif