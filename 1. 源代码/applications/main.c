#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

/* 配置 LED 灯引脚 */
#define LED_PIN              PIN_LED_R

int main(void)
{
    /* 设置 LED 引脚为输出模式 */
    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);
    
    while (1)
    {
        /* LED 灯亮 */
        rt_pin_write(LED_PIN, PIN_LOW);
        rt_thread_mdelay(500);

        /* LED 灯灭 */
        rt_pin_write(LED_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        
    }

    return 0;
}
