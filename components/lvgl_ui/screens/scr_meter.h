#ifndef __SCR_METER_H__
#define __SCR_METER_H__

#include "lvgl.h"

void scr_meter_prepare(void);
void scr_meter_init(void);
void scr_meter_uninit(void);
void scr_meter_step(void);

void scr_meter_update_temp(float celsius);
void scr_meter_set_threshold(float threshold);
float scr_meter_get_threshold(void);

#endif