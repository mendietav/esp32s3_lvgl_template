#ifndef __SCR_SYSTEM_H__
#define __SCR_SYSTEM_H__

#include "lvgl.h"

extern lv_obj_t *ui_scrSystem;

void scr_system_prepare(void);
void scr_system_init(void);
void scr_system_uninit(void);
void scr_system_step(void);

#endif