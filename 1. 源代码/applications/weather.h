#ifndef WEATHER_H
#define WEATHER_H

#include "rtthread.h"

typedef struct
{
    /* 城市名字 */
    char area_1[10];
    char area_2[10];

    /* 天气类型 */
    uint8_t wtId;

    /* 天气名称 */
    char wtNm[10];

    /* 空气质量 */
    int aqi;

    /* 实时温度 */
    int temp;

    /* 实时湿度 */
    uint8_t humi;

} Realtime;

typedef struct 
{
    /* 白天 天气类型 */
    uint8_t D_wtId;

    /* 夜晚 天气类型 */
    uint8_t N_wtId;

    /* 最低温度 */
    int Temp_L;

    /* 最高温度 */
    int Temp_H;


} Weather ;



#endif