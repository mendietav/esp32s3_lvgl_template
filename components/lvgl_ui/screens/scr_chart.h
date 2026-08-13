#ifndef __SCR_CHART_H__
#define __SCR_CHART_H__

#include "lvgl.h"

void scr_chart_prepare(void);
void scr_chart_init(void);
void scr_chart_uninit(void);
void scr_chart_step(void);

void scr_chart_add_point(float celsius);

#endif