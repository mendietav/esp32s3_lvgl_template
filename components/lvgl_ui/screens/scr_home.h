#ifndef __SCR_HOME_H__
#define __SCR_HOME_H__

#include "lvgl.h"

extern lv_obj_t *ui_scrHome;

void scr_home_prepare(void);
void scr_home_init(void);
void scr_home_uninit(void);
void scr_home_step(void);

#endif