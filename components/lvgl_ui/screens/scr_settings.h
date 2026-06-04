#ifndef __SCR_SETTINGS_H__
#define __SCR_SETTINGS_H__

#include "lvgl.h"

extern lv_obj_t *ui_scrSettings;

void scr_settings_prepare(void);
void scr_settings_init(void);
void scr_settings_uninit(void);
void scr_settings_step(void);

#endif