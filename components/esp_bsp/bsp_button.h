#ifndef __BSP_BUTTON_H__
#define __BSP_BUTTON_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void bsp_button_init(void);
bool bsp_button_was_pressed(void);

#ifdef __cplusplus
}
#endif

#endif