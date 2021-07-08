/*
 * File      : httpclient.c
 *
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date             Author      Notes
 * 2018-07-20     flybreak     first version
 * 2018-09-05     flybreak     Upgrade API to webclient latest version
 */

#include <webclient.h>  /* 使用 HTTP 协�??与服务器通信需要包�?此头文件 */
#include <sys/socket.h> /* 使用BSD socket，需要包含socket.h头文�? */
#include <netdb.h>
#include <cJSON.h>
#include <finsh.h>
#include "drv_lcd.h"
#include "gbk2utf8.h"
#include "weather.h"

#include "commom.h"

#define GET_HEADER_BUFSZ        1024        //头部大小
#define GET_RESP_BUFSZ          4096        //响应缓冲区大小
#define GET_URL_LEN_MAX         256         //网址最大长度
#define GET_URI                 "http://api.k780.com/?app=weather.realtime&weaid=%d&ag=today,futureDay&appkey=47100&sign=be87b72e50497b2700d2bc963cb7c973&format=json" //获取天气�? API

Weather wt[3];  /* 三日天气预报 */
Realtime rt = {"","", 0, "", 0,0,0};    /* 当前实时天气 */
int area_id = 265;  /* 默认城市 */
/* 城市对应码表 */
int city_id[50] = {0,0,0,0,0,0,0,0,0,0,1,36,48,59,77,94,105,141,147,165,169,186,200,209,225,248,265,286,291,299,316,326,337,353,363,384,399,401,402,403,405,406,408,410,412,425,434,2628,2631,2743};

/* 打印天气信息 */
void print_weather_info(Weather wt)
{
    rt_kprintf("\n");
    rt_kprintf("day weather id is %d\n",wt.D_wtId);
    rt_kprintf("night weather id is %d\n",wt.N_wtId);
    rt_kprintf("temp is %d/%d\n",wt.Temp_L,wt.Temp_H);
    rt_kprintf("\n");
}

/* 打印实时天气信息 */
void print_realtime(Realtime rt)
{
    rt_kprintf("\n");
    rt_kprintf("city is %s %s\n",rt.area_1,rt.area_2);
    rt_kprintf("weather id is %d\n",rt.wtId);
    rt_kprintf("aqi is %d\n",rt.aqi);
    rt_kprintf("temp is %d\n",rt.temp);
    rt_kprintf("humi is %d\n",rt.humi);
    rt_kprintf("\n");
}

/* 天气数据解析 */
void weather_data_parse(rt_uint8_t *data)
{
    cJSON *root = RT_NULL, *object = RT_NULL, *item = RT_NULL, *sub_item = RT_NULL, *child_item = RT_NULL;

    root = cJSON_Parse((const char *)data);
    if (!root)
    {
        rt_kprintf("No memory for cJSON root!\n");
        return;
    }
    object = cJSON_GetObjectItem(root, "success");
    rt_kprintf("\nsuccess: %s \n", object->valuestring);

    if( strcmp("1", object->valuestring) != 0)
    {
        rt_kprintf("request failed\n");
        goto __exit;
    }
    
    object = cJSON_GetObjectItem(root, "result");

    rt_memset(rt.area_1,0,10);
    item = cJSON_GetObjectItem(object, "area_1");
    char * area_1 = RT_NULL;
    utf82gbk(&area_1,(void *)item->valuestring,rt_strlen(item->valuestring));
    rt_kprintf("area_1: %s\n", item->valuestring);
    
    rt_memcpy(rt.area_1,area_1,rt_strlen(area_1));
    
    rt_free(area_1);

    rt_memset(rt.area_2,0,10);
    item = cJSON_GetObjectItem(object, "area_2");
    char * area_2 = RT_NULL;
    utf82gbk(&area_2,(void *)item->valuestring,rt_strlen(item->valuestring));
    rt_kprintf("area_2: %s\n", item->valuestring);
    
    rt_memcpy(rt.area_2,area_2,rt_strlen(area_2));
    
    rt_free(area_2);


    item = cJSON_GetObjectItem(object, "realTime");

    sub_item = cJSON_GetObjectItem(item, "wtAqi");
    rt_kprintf("aqi:%s \n", sub_item->valuestring);
    rt.aqi = atoi(sub_item->valuestring);

    sub_item = cJSON_GetObjectItem(item, "wtId");
    rt_kprintf("wtId:%s \n", sub_item->valuestring);
    rt.wtId = atoi(sub_item->valuestring);

    rt_memset(rt.wtNm,0,10);
    sub_item = cJSON_GetObjectItem(item, "wtNm");
    char * wtNm = RT_NULL;
    utf82gbk(&wtNm,(void *)sub_item->valuestring,rt_strlen(sub_item->valuestring));
    rt_kprintf("wtNm:%s \n", sub_item->valuestring);
    rt_memcpy(rt.wtNm, wtNm, rt_strlen(wtNm));
    rt_free(wtNm);

    sub_item = cJSON_GetObjectItem(item, "wtTemp");
    rt_kprintf("wtTemp:%s \n", sub_item->valuestring);
    rt.temp = atoi(sub_item->valuestring);

    sub_item = cJSON_GetObjectItem(item, "wtHumi");
    rt_kprintf("wtHumi:%s \n", sub_item->valuestring);
    rt.humi = atoi(sub_item->valuestring);

    item = cJSON_GetObjectItem(object, "today");
    sub_item = cJSON_GetObjectItem(item, "wtId1");
    wt[0].D_wtId = atoi(sub_item->valuestring);
    sub_item = cJSON_GetObjectItem(item, "wtId2");
    wt[0].N_wtId = atoi(sub_item->valuestring);
    sub_item = cJSON_GetObjectItem(item, "wtTemp1");
    wt[0].Temp_H = atoi(sub_item->valuestring);
    sub_item = cJSON_GetObjectItem(item, "wtTemp2");
    wt[0].Temp_L = atoi(sub_item->valuestring);

    //print_weather_info(wt[0]);

    item = cJSON_GetObjectItem(object, "futureDay");
    for (int i=0;i<2;i++)
    {
        sub_item = cJSON_GetArrayItem(item,i);
        child_item = cJSON_GetObjectItem(sub_item, "wtId1");
        wt[i+1].D_wtId = atoi(child_item->valuestring);
        child_item = cJSON_GetObjectItem(sub_item, "wtId2");
        wt[i+1].N_wtId = atoi(child_item->valuestring);
        child_item = cJSON_GetObjectItem(sub_item, "wtTemp1");
        wt[i+1].Temp_H = atoi(child_item->valuestring);
        child_item = cJSON_GetObjectItem(sub_item, "wtTemp2");
        wt[i+1].Temp_L = atoi(child_item->valuestring);
    }
    
    for(int i=0;i<3;i++)
    {
        print_weather_info(wt[i]);
    }

__exit:

    if (root != RT_NULL)
        cJSON_Delete(root);

}

/* 请求天气数据 */
void fetch_weather(void)
{
    rt_uint8_t *buffer = RT_NULL;
    int resp_status;
    struct webclient_session *session = RT_NULL;
    char *weather_url = RT_NULL;
    int content_length = -1, bytes_read = 0;
    int content_pos = 0;

    /* weather_url 分配空间 */
    weather_url = rt_calloc(1, GET_URL_LEN_MAX);
    if (weather_url == RT_NULL)
    {
        rt_kprintf("No memory for weather_url!\n");
        goto __exit;
    }
    /* 拼接 GET 网址 */
    rt_snprintf(weather_url, GET_URL_LEN_MAX, GET_URI, area_id);

    /* 创建会话并且设置响应的大�? */
    session = webclient_session_create(GET_HEADER_BUFSZ);
    if (session == RT_NULL)
    {
        rt_kprintf("No memory for get header!\n");
        goto __exit;
    }

    /* 发�? GET 请求使用默�?�的头部 */
    if ((resp_status = webclient_get(session, weather_url)) != 200)
    {
        rt_kprintf("webclient GET request failed, response(%d) error.\n", resp_status);
        goto __exit;
    }

    /* 分配用于存放接收数据的缓�? */
    buffer = rt_calloc(1, GET_RESP_BUFSZ);
    if (buffer == RT_NULL)
    {
        rt_kprintf("No memory for data receive buffer!\n");
        goto __exit;
    }

    content_length = webclient_content_length_get(session);
    if (content_length < 0)
    {
        /* 返回的数�?�?分块传输�?. */
        do
        {
            bytes_read = webclient_read(session, buffer, GET_RESP_BUFSZ);
            if (bytes_read <= 0)
            {
                break;
            }
        }while (1);
    }
    else
    {
        do
        {
            bytes_read = webclient_read(session, buffer,
                                        content_length - content_pos > GET_RESP_BUFSZ ?
                                        GET_RESP_BUFSZ : content_length - content_pos);
            if (bytes_read <= 0)
            {
                break;
            }
            content_pos += bytes_read;
        }while (content_pos < content_length);
    }

    /* 天气数据解析 */
    weather_data_parse(buffer);

__exit:
    /* 释放网址空间 */
    if (weather_url != RT_NULL)
        rt_free(weather_url);
    /* 关闭会话 */
    if (session != RT_NULL)
        webclient_close(session);
    /* 释放缓冲区空�? */
    if (buffer != RT_NULL)
        rt_free(buffer);
}

//MSH_CMD_EXPORT(weather, Get weather by webclient);

void weather_thread_entry(void * parameter)
{
    while(net_state != RT_TRUE)
    {
        rt_thread_mdelay(500);
    }

    while (1)
    {
        fetch_weather();
        /* 一分钟更新一次数据 */
        rt_thread_mdelay(60000);
    }
    
}

int weather_start(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("weather", weather_thread_entry, RT_NULL, 2048,RT_THREAD_PRIORITY_MAX - 2, 20);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
        rt_kprintf("weather thread started\n");
    }
    else
    {
        rt_kprintf("create weather thread err!\n");
    }
    return RT_EOK;
}
INIT_APP_EXPORT(weather_start);


int change_city(int id)
{

    area_id = id;
    fetch_weather();
    show_weather();

    return 0;
}
