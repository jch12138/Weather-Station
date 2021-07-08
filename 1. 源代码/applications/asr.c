//#include "main.h"
#include "stm32l4xx_hal.h"
#include <string.h>
#include "stdio.h"
#include "rtthread.h"
#include "rtdevice.h"
#include "board.h"
#include "commom.h"

#define ASR_ADDR                    0x1E
#define ASR_ADD_WORD_ADDR           0x01   //词条添加地址
#define ASR_MODE_ADDR               0x02   //识别模式设置地址，值为0-2，0:循环识别模式 1:口令模式 ,2:按键模式，默认为循环检测
#define ASR_RGB_ADDR                0x03   //RGB灯设置地址,需要发两位，第一个直接为灯号1：蓝 2:红 3：绿 ,
                                           //第二个字节为亮度0-255，数值越大亮度越高
#define ASR_REC_GAIN                0x04   //识别灵敏度设置地址，灵敏度可设置为0x00-0x55，值越高越容易检测但是越容易误判，
                                           //建议设置值为0x40-0x55,默认值为0x40 
#define ASR_CLEAR_ADDR              0x05   //清除掉电缓存操作地址，录入信息前均要清除下缓存区信息
#define ASR_KEY_FLAG                0x06   //用于按键模式下，设置启动识别模式
#define ASR_VOICE_FLAG              0x07   //用于设置是否开启识别结果提示音
#define ASR_RESULT                  0x08   //识别结果存放地址
#define ASR_BUZZER                  0x09   //蜂鸣器控制写1开启，写0关闭
#define ASR_NUM_CLECK               0x0a   //录入词条数目校验

/* defined the KEY2 pin: PD8 */
#define KEY1_PIN    GET_PIN(D, 9)
/* defined the KEY2 pin: PD8 */
#define KEY2_PIN    GET_PIN(D, 8)

extern int city_id[50];
/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

I2C_HandleTypeDef hi2c1;
/**
* @brief I2C MSP Initialization
* This function configures the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hi2c->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }

}


/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10909CEC;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

void I2C_Write(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,uint16_t NumByteToWrite){
  uint8_t buff[100];
  buff[0] = RegisterAddr;
  memcpy(buff+1, pBuffer,NumByteToWrite);
  HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)DeviceAddr, buff, NumByteToWrite+1, 1000);
}


void I2C_Read(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr, uint16_t NumByteToRead){
  uint8_t Register[3]={RegisterAddr};
  HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)DeviceAddr, Register, 1, 1000);
  HAL_I2C_Master_Receive(&hi2c1, (uint16_t)DeviceAddr, pBuffer, NumByteToRead, 1000);
}

uint8_t rgb_set(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t buf[3];
    
    buf[0] = r;
    buf[1] = g;
    buf[2] = b; 
    //
    I2C_Write(buf,ASR_ADDR,ASR_RGB_ADDR,3 );
    
	return 0;
}

uint8_t asr_add_words(uint8_t id, char * words)
{
    uint8_t buf[80];

    buf[0] = id;
    memcpy(buf+1, words, strlen(words));

    printf("%s  %d\r\n",buf,strlen(words));

    I2C_Write(buf,ASR_ADDR,ASR_ADD_WORD_ADDR,strlen(words)+1);
	
		return 0;
}


uint8_t asr_get_result(void)
{
    uint8_t res;
		I2C_Read(&res,  ASR_ADDR ,ASR_RESULT,1);
    return res;
}

uint8_t asr_get_words_num(void)
{
    uint8_t res;
		I2C_Read(&res,  ASR_ADDR ,ASR_NUM_CLECK,1);
    return res;
}

void key_irq_entry(void *parameter)
{
    uint8_t data = 0x01;
    I2C_Write(&data,ASR_ADDR, ASR_KEY_FLAG,1);
}

uint8_t asr_init(void)
{   
    uint8_t data;

    MX_I2C1_Init();

    rt_thread_mdelay(100);

		data = 0x40;
    I2C_Write(&data,ASR_ADDR,ASR_REC_GAIN,1);
		rt_thread_mdelay(100);
	
		data = 0x01;
    I2C_Write(&data,ASR_ADDR,ASR_VOICE_FLAG,1);
		rt_thread_mdelay(100);
		
    rgb_set(255,255,255);
    rt_thread_mdelay(1000);
		rgb_set(0,0,0);
		
    rt_pin_mode(KEY2_PIN, PIN_MODE_INPUT);
    rt_pin_attach_irq(KEY2_PIN, PIN_IRQ_MODE_FALLING, key_irq_entry,RT_NULL);
    rt_pin_irq_enable(KEY2_PIN, PIN_IRQ_ENABLE);

		return 0;
}

void asr_thread_entry(void* parameter)
{
  rt_uint8_t res;

  while(net_state != RT_TRUE)
  {
      rt_thread_mdelay(500);
  }

  while (1)
  {
    res = asr_get_result();
    
    if(res != 255)
    {
      rt_kprintf("result = %d\n",res);
      if(res >= 10 && res <= 49)
      change_city(city_id[res]);
    }
    rt_thread_mdelay(2000);
  }
  
}

int asr_start(void)
{
		asr_init();

    rt_thread_t tid = rt_thread_create("asr",           /* 线程名称 */
                                      asr_thread_entry, /* 线程入口 */
                                      RT_NULL,          /* 入口参数 */
                                      2048,             /* 线程栈大小 */
                                      10,               /* 线程优先级 */
                                      10);              /* 线程时间片 */

    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
        rt_kprintf("asr thread started\n");
    }
}
INIT_APP_EXPORT(asr_start);

void key_thread_entry(void* parameter)
{
	rt_pin_mode(KEY1_PIN, PIN_MODE_INPUT);
	int id = 10;
	
	while(net_state != RT_TRUE)
  {
      rt_thread_mdelay(500);
  }
	
	while(1)
	{
		if (rt_pin_read(KEY1_PIN) == PIN_LOW)
		{
			rt_thread_mdelay(20);
			
			if (rt_pin_read(KEY1_PIN) == PIN_LOW)
			{
				change_city(city_id[id++]);
				if(id >49)id = 10;
			}
			
		}
	}
}

int key_start(void)
{

    rt_thread_t tid = rt_thread_create("key",           /* 线程名称 */
                                      key_thread_entry, /* 线程入口 */
                                      RT_NULL,          /* 入口参数 */
                                      2048,             /* 线程栈大小 */
                                      10,               /* 线程优先级 */
                                      10);              /* 线程时间片 */

    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
        rt_kprintf("key thread started\n");
    }
}
INIT_APP_EXPORT(key_start);