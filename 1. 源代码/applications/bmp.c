#include "rtthread.h"
#include "drv_lcd.h"
#include "bmp.h"
#include <dfs_posix.h>

/*==================================================================
                        用到的变量及宏定义
==================================================================*/
BMP_HEAD bmp; //图片文件头
BMP_POINT point;

/*==================================================================
* Function    : RGB888ToRGB565
* Description    : 把24位图变成16位图
* Input Para    : unsigned char r , unsigned char g , unsigned char b
* Output Para    : void
* Return Value: 颜色值
==================================================================*/
uint16_t RGB888ToRGB565(unsigned char r, unsigned char g, unsigned char b)
{
    return (uint32_t)(r & 0xF8) << 8 | (g & 0xFC) << 3 | (b & 0xF8) >> 3;
}

uint16_t RGB888ToBGR565(unsigned char r, unsigned char g, unsigned char b)
{
    return (uint16_t)(b & 0xF8) << 8 | (g & 0xFC) << 3 | (r & 0xF8) >> 3;
}
/*==================================================================
* Function    : DrawPixel
* Description    : 画点函数，在TFT的某一点上画一个颜色点
* Input Para    : unsigned int x , unsigned int y , int Color  ===》 x坐标，y坐标，颜色值
* Output Para    : void
* Return Value: void
==================================================================*/
void DrawPixel(unsigned int x, unsigned int y, int Color)
{
    lcd_draw_point_color(x, y, Color);
}

/*==================================================================
* Function    : Show_BMP
* Description    : 显示BMP格式的文件，分为图片和图标，图片尺寸是240*320是完全显示，图标只显示内容，其他白色的部分维持原来的颜色
* Input Para    : unsigned char type , unsigned int point_x , unsigned int point_y , char *path
         ===》   显示的类型：PCITURE图片，ICON图标 ，x轴起始点 ，y轴起始点 ，图片在SD卡里的路径
* Output Para    : void
* Return Value: void
==================================================================*/
void Show_BMP(unsigned int point_x, unsigned int point_y, char *path)
{
    int fd;
    
    unsigned char *buffer;

    unsigned long tx, ty, r_data, g_data, b_data;
    // 挂载设备
    fd = open(path, O_RDONLY); // 读取图标路径

    if (fd >= 0)
    {
        read(fd, (void *)&bmp, sizeof(bmp));

        if ((bmp.pic_head[0] == 'B') && (bmp.pic_head[1] == 'M')) // 判断是不是BMP文件
        {    
            lseek(fd,((bmp.pic_data_address_h << 16) | bmp.pic_data_address_l),SEEK_SET);
            lcd_address_set(point_x, point_y, point_x + bmp.pic_w_l - 1, point_y + bmp.pic_h_l - 1);
            
            buffer = rt_malloc(bmp.pic_w_l * 3);

            for (ty = 0; ty < bmp.pic_h_l; ty++)
            {
                read(fd, buffer, (bmp.pic_w_l) * 3);
                for (tx = 0; tx < bmp.pic_w_l; tx++)
                {
                    r_data = *(tx * 3 + 2 + buffer);
                    g_data = *(tx * 3 + 1 + buffer);
                    b_data = *(tx * 3 + 0 + buffer);
                    if (tx < 240)
                    {
                        point.x = tx;
                        point.y = bmp.pic_h_l - 1 - ty;
                        point.r = r_data;
                        point.g = g_data;
                        point.b = b_data;
                        uint16_t color = RGB888ToRGB565(point.r, point.g, point.b);
                        lcd_write_half_word(color);
                    }
                }
            }

            rt_free(buffer);
            
        }
        close(fd);

    } 
} 