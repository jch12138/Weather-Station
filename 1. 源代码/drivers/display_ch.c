#include <rtthread.h>
#include <rtdevice.h>
#include <dfs_posix.h>
#include "drv_lcd.h"
#include "ascii.h"

#include "commom.h"

#define macWIDTH_EN_CHAR 8   //Ӣ���ַ�����
#define macHEIGHT_EN_CHAR 16 //Ӣ���ַ��߶�

#define macWIDTH_CH_CHAR 16  //�����ַ�����
#define macHEIGHT_CH_CHAR 16 //�����ַ��߶�

#define mac_DispWindow_X_Star 0 //  ��ʼ���? X ����
#define mac_DispWindow_Y_Star 0 //  ��ʼ���? Y ����

#define mac_DispWindow_COLUMN 240                   // ������
#define mac_DispWindow_PAGE 240                     //������
static const char *file_location = "/HZLIB.bin"; //�ֿ��ļ���·��
/**
 *  LCD ����ʾӢ���ַ�
 * usX : ���ض�ɨ��ķ������ַ������? X ����
 * usY : ���ض�ɨ��ķ������ַ������? Y ����
 * cChar : Ҫ��ʾ��Ӣ���ַ�
 * usColor_Background : ѡ��Ӣ���ַ��ı���ɫ
 * usColor_Foreground : ѡ��Ӣ���ַ���ǰ��ɫ
 * ����ֵ �� ��
 */
void lcd_disp_char_en(uint16_t usX, uint16_t usY, const char cChar, uint16_t usColor_Background, uint16_t usColor_Foreground)
{
    uint8_t ucTemp, ucRelativePosition, ucPage, ucColum;

    ucRelativePosition = cChar - ' ';

    lcd_open_window(usX, usY, macWIDTH_EN_CHAR, macHEIGHT_EN_CHAR);

    for (ucPage = 0; ucPage < macHEIGHT_EN_CHAR; ucPage++)
    {
        ucTemp = ucAscii_1608[ucRelativePosition][ucPage];

        for (ucColum = 0; ucColum < macWIDTH_EN_CHAR; ucColum++)
        {
            if (ucTemp & 0x01)
            {
                lcd_write_half_word(usColor_Foreground);
            }
            else
            {
                lcd_write_half_word(usColor_Background);
            }
            ucTemp >>= 1;

        } /*д��һ�� */
    }
    /*ȫ��д�� */
}

/**
 * �� LCD ����ʾӢ���ַ���
 * usX : ���ض�ɨ�跽�����ַ�������ʼ X ����
 * usY : ���ض�ɨ�跽�����ַ�������ʼ Y ����
 * pStr : Ҫ��ʾ��Ӣ���ַ������׵�ַ
 * usColor_Background : ѡ��Ӣ���ַ����ı���ɫ
 * usColor_Foreground : ѡ��Ӣ���ַ�����ǰ��ɫ
 * ����ֵ : ��
 */

void lcd_disp_str_en(uint16_t usX, uint16_t usY, const char *pStr, uint16_t usColor_Background, uint16_t usColor_Foreground)
{
    while (*pStr != '\0')
    {
        if ((usX - mac_DispWindow_X_Star + macWIDTH_EN_CHAR) > mac_DispWindow_COLUMN)
        {
            usX = mac_DispWindow_X_Star;
            usY += macHEIGHT_EN_CHAR;
        }

        if ((usY - mac_DispWindow_Y_Star + macHEIGHT_EN_CHAR) > mac_DispWindow_PAGE)
        {
            usX = mac_DispWindow_X_Star;
            usY = mac_DispWindow_Y_Star;
        }

        lcd_disp_char_en(usX, usY, *pStr, usColor_Background, usColor_Foreground);
        pStr++;
        usX += macWIDTH_EN_CHAR;
    }
}

/**
 * �� SD ���л�ȡ�����ֿ�
 * *pBuffer : ����ָ�� 
 * c : ��ȡ���ַ�
 */

int GetGBKCode_from_sd(uint8_t *pBuffer, uint16_t c)
{
    unsigned char Hight8bit, Low8bit;
    unsigned int pos;
    int fd, size, ret;
    int result = 0;

    Hight8bit = c >> 8;   /* ȡ��8λ���� */
    Low8bit = c & 0x00FF; /* ȡ��8λ���� */

    //rt_kprintf("%d,%d\r\n",Hight8bit,Low8bit);
    //rt_kprintf("%x,%x\r\n",Hight8bit,Low8bit);

    pos = ((Hight8bit - 0xa0 - 16) * 94 + Low8bit - 0xa0 - 1) * 2 * 16;

    /* ��ֻ��ģʽ�� �ֿ��ļ� */
    fd = open(file_location, O_RDONLY);
    if (fd >= 0)
    {
        //�ƶ�ָ��
        ret = lseek(fd, pos, 0);
        //rt_kprintf("lseek  = %d \n",ret);
        size = read(fd, pBuffer, 32);
        close(fd);
        if (size < 0)
        {
            rt_kprintf("read %s  file error!!!\n", file_location);
        }

        result = 0;
    }
    else
    {
        rt_kprintf("open %s file error!!!\n", file_location);
        result = -1;
    }
    return result;
}

/**
 * �� lcd ����ʾ�����ַ�
 * x : ���ض�ɨ�跽�����ַ�������ʼ X ����
 * y : ���ض�ɨ�跽�����ַ�������ʼ Y ����
 * usChar : Ҫ��ʾ���ַ�
 * usColor_Background : ѡ���ַ����ı���ɫ
 * usColor_Foreground : ѡ���ַ�����ǰ��ɫ
 * 
 */
void lcd_disp_char_ch(uint16_t x, uint16_t y, uint16_t usChar, uint16_t usColor_Background, uint16_t usColor_Foreground)
{
    uint8_t ucPage, ucColum;
    uint8_t ucBuffer[32];
    uint16_t us_Temp;

    lcd_open_window(x, y, macWIDTH_CH_CHAR, macHEIGHT_CH_CHAR);
    GetGBKCode_from_sd(ucBuffer, usChar); //ȡ��ģ����

    for (ucPage = 0; ucPage < macHEIGHT_CH_CHAR; ucPage++)
    {
        //ȡ�������ֽڵ����ݣ��� LCD �ϼ���һ�����ֵ�һ��
        us_Temp = ucBuffer[ucPage * 2];
        us_Temp = (us_Temp << 8);
        us_Temp |= ucBuffer[ucPage * 2 + 1];

        for (ucColum = 0; ucColum < macWIDTH_CH_CHAR; ucColum++)
        {
            if (us_Temp & (0x01 << 15)) //��λ��ǰ
            {
                lcd_write_half_word(usColor_Foreground);
            }
            else
            {
                lcd_write_half_word(usColor_Background);
            }
            us_Temp <<= 1;
        }
    }
}

/*
 * LCD ��ʾ�����ַ���
 * usX : ���ض�ɨ�跽�����ַ�������ʼ X ����
 * usY : ���ض�ɨ�跽�����ַ�������ʼ Y ����
 * pStr : Ҫ��ʾ���ַ����׵�ַ
 * usColor_Background : ѡ���ַ����ı���ɫ
 * usColor_Foreground : ѡ���ַ�����ǰ��ɫ
 */
void lcd_disp_str_ch(uint16_t usX, uint16_t usY, const uint8_t *pStr, uint16_t usColor_Background, uint16_t usColor_Foreground)
{
    rt_uint16_t usCh = 0;
    rt_uint8_t usCh_low = 0;
    rt_uint8_t usCh_high = 0;
    rt_uint16_t usCh_temp = 0xffff;

    while (*pStr != '\0')
    {
        if ((usX - mac_DispWindow_X_Star + macWIDTH_CH_CHAR) > mac_DispWindow_COLUMN)
        {
            usX = mac_DispWindow_X_Star;
            usY += macHEIGHT_CH_CHAR;
        }

        if ((usY - mac_DispWindow_Y_Star + macHEIGHT_CH_CHAR) > mac_DispWindow_PAGE)
        {
            usX = mac_DispWindow_X_Star;
            usY = mac_DispWindow_Y_Star;
        }
        usCh_low = *pStr;
        usCh_high = *(pStr + 1);
        usCh_temp = 0xffff & usCh_low;
        usCh_temp = usCh_temp << 8;
        usCh_temp = usCh_temp | usCh_high;

        lcd_disp_char_ch(usX, usY, usCh_temp, usColor_Background, usColor_Foreground);
        usX += macWIDTH_CH_CHAR;
        pStr += 2; //һ�����������ֽ�
    }
}

/**
 * ��ʾ��Ӣ���ַ���
 * usX : ���ض�ɨ�跽�����ַ�������ʼ X ����
 * usY : ���ض�ɨ�跽�����ַ�������ʼ Y ����
 * pStr : Ҫ��ʾ���ַ����׵�ַ
 * usColor_Background : ѡ���ַ����ı���ɫ
 * usColor_Foreground : ѡ���ַ�����ǰ��ɫ
 * 
 * 
*/
void lcd_disp_str_en_ch(uint16_t usX, uint16_t usY, const uint8_t *pStr, uint16_t usColor_Background, uint16_t usColor_Foreground)
{
    uint16_t usCh;
    rt_uint8_t usCh_low = 0;
    rt_uint8_t usCh_high = 0;
    rt_uint16_t usCh_temp = 0xffff;
    while (*pStr != '\0')
    {
        if (*pStr <= 126) //Ӣ���ַ�
        {
            if ((usX - mac_DispWindow_X_Star + macWIDTH_EN_CHAR) > mac_DispWindow_COLUMN)
            {
                usX = mac_DispWindow_X_Star;
                usY += macHEIGHT_EN_CHAR;
            }

            if ((usY - mac_DispWindow_Y_Star + macHEIGHT_EN_CHAR) > mac_DispWindow_PAGE)
            {
                usX = mac_DispWindow_X_Star;
                usY = mac_DispWindow_Y_Star;
            }

            lcd_disp_char_en(usX, usY, *pStr, usColor_Background, usColor_Foreground);

            usX += macWIDTH_EN_CHAR;

            pStr++;
        }
        else
        {
            if ((usX - mac_DispWindow_X_Star + macWIDTH_CH_CHAR) > mac_DispWindow_COLUMN)
            {
                usX = mac_DispWindow_X_Star;
                usY += macHEIGHT_CH_CHAR;
            }

            if ((usY - mac_DispWindow_Y_Star + macHEIGHT_CH_CHAR) > mac_DispWindow_PAGE)
            {
                usX = mac_DispWindow_X_Star;
                usY = mac_DispWindow_Y_Star;
            }

            usCh_low = *pStr;
            usCh_high = *(pStr + 1);
            usCh_temp = 0xffff & usCh_low;
            usCh_temp = usCh_temp << 8;
            usCh_temp = usCh_temp | usCh_high;

            lcd_disp_char_ch(usX, usY, usCh_temp, usColor_Background, usColor_Foreground);
            usX += macWIDTH_CH_CHAR;
            pStr += 2; //һ�����������ֽ�
        }
    }
}

