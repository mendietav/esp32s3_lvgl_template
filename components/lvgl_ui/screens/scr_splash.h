#ifndef __SCR_SPLASH_H__
#define __SCR_SPLASH_H__

#include "lvgl.h"

extern lv_obj_t *ui_scrSplash;

void scr_splash_prepare(void);
void scr_splash_init(void);
void scr_splash_uninit(void);
void scr_splash_step(void);

#endif