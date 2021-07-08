#ifndef COMMON_H
#define COMMON_H

#include "stdint.h"
#include "bmp.h"
/* 此文件用于包含一些公用的函数 */

/* 联网状态 */
extern rt_bool_t net_state;

/* 显示中文 */
void lcd_disp_str_en_ch(uint16_t usX, uint16_t usY, const uint8_t *pStr, uint16_t usColor_Background, uint16_t usColor_Foreground);
void lcd_disp_str_ch(uint16_t usX, uint16_t usY, const uint8_t *pStr, uint16_t usColor_Background, uint16_t usColor_Foreground);
void lcd_disp_str_en(uint16_t usX, uint16_t usY, const char *pStr, uint16_t usColor_Background, uint16_t usColor_Foreground);

/* 天气 */
#include "weather.h"
extern Weather wt[3];  /* 三日天气预报 */
extern Realtime rt;    /* 当前实时天气 */
void fetch_weather(void);
int change_city(int id);
/* UI */
void show_weather(void);

/* led */
void led_init(void);


#endif