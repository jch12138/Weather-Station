#include "rtthread.h"
#include "drv_lcd.h"
#include "bmp.h"

#include "commom.h"

#define DBG_TAG "UI"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define LIGHT RT_TRUE
#define DARK RT_FALSE

#define PIC RT_TRUE
#define ICON RT_FALSE

rt_bool_t display_mode = LIGHT;

int fore_color[2] = {0xdedb,0x0000};
int back_color[2] = {0x2945,0xf7be};

int fore_color_curr = -1;
int back_color_curr = -1;

int ui_test(int argc, char **argv)
{
    lcd_clear(0xf7be);

    Show_BMP(11, 34, "/day/cloudy.bmp");

    lcd_disp_str_ch(90, 13, "ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½", 0xf7be, 0xc638);

    lcd_disp_str_ch(115, 100, "ï¿½ï¿½ï¿½ï¿½", 0x9628, 0xffff);
}
MSH_CMD_EXPORT(ui_test, ui test);

char * get_weather_name_by_id(uint8_t wtId)
{
    if (wtId == 1)
    {
        return "sunny";
    }
    else if (wtId == 2)
    {
        return "cloudy";
    }
    else if (wtId == 3)
    {
        return "overcast";
    }
    else if (wtId == 4 | wtId == 7 | wtId == 8 | wtId == 9 | wtId == 10 | wtId == 11 | wtId == 12 | wtId == 13 |
             wtId == 20 | wtId == 22 | wtId == 23 | wtId == 24 | wtId == 25 | wtId == 26 | wtId == 41 | wtId == 5 | wtId == 6)
    {
        return "rain";
    }
    else if (wtId == 19 | wtId == 21 | wtId == 30 | wtId == 31 | wtId == 32 | wtId == 33 | wtId == 34 | wtId == 35 | wtId == 36 | wtId == 37 | wtId == 38 | wtId == 39 | wtId == 40)
    {
        return "foggy";
    }
    else if (wtId == 14 | wtId == 15 | wtId == 16 | wtId == 17 | wtId == 18 | wtId == 27 | wtId == 28 | wtId == 29 | wtId == 42)
    {
        return "snow";
    }
    else
    {
        return "unknow";
    }
}

void get_weather_icon_path(char path[], uint8_t wtId, rt_bool_t size)
{
    char res_path[30] ;

    rt_memset(path, 0, rt_strlen(path));
    
    rt_sprintf(res_path, "/%s%s/%s.bmp", (display_mode ? "day" : "night"), (size ? "" : "_small"), get_weather_name_by_id(wtId));
    rt_strncpy(path, res_path, rt_strlen(res_path));
}


void show_weather(void)
{
    time_t now;     
    char time_str[20];
    char city[30];
    char weather_name[10];

    if(back_color_curr != back_color[display_mode])
    {
        back_color_curr = back_color[display_mode];
        fore_color_curr = fore_color[display_mode];
        lcd_set_color(back_color_curr,fore_color_curr);
        lcd_clear(back_color_curr);
    }
    
    lcd_draw_line(18, 130, 224, 130);

    lcd_disp_str_ch(31, 140, "½ñÌì",  back_color_curr, fore_color_curr);
    lcd_disp_str_ch(105, 140, "Ã÷Ìì", back_color_curr, fore_color_curr);
    lcd_disp_str_ch(175, 140, "ºóÌì", back_color_curr, fore_color_curr);

    rt_sprintf(city, "%s %s     ", rt.area_1, rt.area_2);
    lcd_disp_str_en_ch(10, 5, city, back_color_curr, fore_color_curr);

    rt_sprintf(weather_name, "%s     ", rt.wtNm);
    lcd_disp_str_en_ch(115, 40, weather_name, back_color_curr, fore_color_curr);

    now = time(RT_NULL);
    rt_strncpy(time_str, ctime(&now)+11,5);
    time_str[5] = '\0';
    lcd_show_string(180,5,16,time_str);

    char bmp_path[25];

    get_weather_icon_path(bmp_path,rt.wtId,PIC);
    Show_BMP(11,24,bmp_path);

    char aqi_state[15];

    if (rt.aqi <= 50)
    {
        rt_sprintf(aqi_state, "%3d  ÓÅÐã ", rt.aqi);
        lcd_disp_str_en_ch(115, 60, aqi_state, 0x9628, 0xffff);
    }
    else if(rt.aqi <=100)
    {
        rt_sprintf(aqi_state, "%3d  Á¼ºÃ ", rt.aqi);
        lcd_disp_str_en_ch(115, 60, aqi_state, 0xffe, 0xffff);
    }
    else
    {
        rt_sprintf(aqi_state, "%3d  ÎÛÈ¾ ", rt.aqi);
        lcd_disp_str_en_ch(115, 60, aqi_state, 0xf800, 0xffff);
    }


    char temp_state[10];
    rt_sprintf(temp_state, "ÎÂ¶È:  %d", rt.temp);
    lcd_disp_str_en_ch(115, 80, temp_state, back_color_curr, fore_color_curr);

    char humi_state[10];
    rt_sprintf(humi_state, "Êª¶È:  %d", rt.humi);
    lcd_disp_str_en_ch(115, 100, humi_state, back_color_curr, fore_color_curr);

    if(display_mode == LIGHT)
    {
        get_weather_icon_path(bmp_path,wt[0].D_wtId ,ICON);
        Show_BMP(20,160,bmp_path);

        get_weather_icon_path(bmp_path,wt[1].D_wtId ,ICON);
        Show_BMP(95,160,bmp_path);

        get_weather_icon_path(bmp_path,wt[2].D_wtId ,ICON);
        Show_BMP(168,160,bmp_path);

    }
    else
    {
        get_weather_icon_path(bmp_path,wt[0].N_wtId ,ICON);
        Show_BMP(20,160,bmp_path);

        get_weather_icon_path(bmp_path,wt[1].N_wtId ,ICON);
        Show_BMP(95,160,bmp_path);

        get_weather_icon_path(bmp_path,wt[2].N_wtId ,ICON);
        Show_BMP(168,160,bmp_path);
    }


    lcd_show_string(21, 209, 16, "%d /%d  ", wt[0].Temp_L, wt[0].Temp_H);
    lcd_show_string(98, 209, 16, "%d /%d  ", wt[1].Temp_L, wt[1].Temp_H);
    lcd_show_string(170, 209, 16, "%d /%d  ", wt[2].Temp_L, wt[2].Temp_H);
}

void ui_thread_entry(void *parameter)
{
    

    lcd_show_string(50,110,16,"system starting...");
    
    while (net_state != RT_TRUE)
    {
        rt_thread_mdelay(500);
    }



    while (1)
    {
        show_weather();

        rt_thread_mdelay(10000);
    }
}

void time_thread_entry(void *parameter)
{
    while (net_state != RT_TRUE)
    {
        rt_thread_mdelay(500);
    }

    time_t now;     
    char buf[5];
    rt_uint8_t hour;

    while (1)
    {
        now = time(RT_NULL);
        rt_strncpy(buf, ctime(&now)+11,2);
        hour = atoi(buf);
        //rt_kprintf("hour: %d\n",hour);
        if(display_mode == DARK && hour > 5)
        {
            display_mode = LIGHT;
        }
        if(display_mode == LIGHT && hour > 17)
        {
            display_mode = DARK;
        }
        rt_thread_mdelay(60000);
    }
    
}

int ui_start(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("UI", ui_thread_entry, RT_NULL, 2048, 2, 20);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
        rt_kprintf("ui thread started\n");
    }
    else
    {
        rt_kprintf("create ui thread err!\n");
    }

    tid = rt_thread_create("time", time_thread_entry, RT_NULL, 2048, 30, 20);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
        rt_kprintf("time thread started\n");
    }
    else
    {
        rt_kprintf("create time thread err!\n");
    }

    return RT_EOK;
}
INIT_APP_EXPORT(ui_start);

int change_display_mode(int argc, char *argv[])
{
    if(display_mode == DARK)
    {
        display_mode = LIGHT;
    }
    else
    {
        display_mode = DARK;
    }

    show_weather();

    return 0;
}
MSH_CMD_EXPORT(change_display_mode, change_display_mode);
