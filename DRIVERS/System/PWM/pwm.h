#ifndef _PWM_H
#define _PWM_H
#include "sys.h"

extern  u8  current_state;

extern u32 PWM_CK_CNT;
extern u32 PWM_PRESCALER;
/* 宏定义 --------------------------------------------------------------------*/
/* PWM输出 */
#define PWM_TIMx                  TIM14
#define PWM_TIM_CLK               RCC_APB1Periph_TIM14      //源代码须对应APB1、APB2
#define PWM_TIM_GPIO_CLK          RCC_AHB1Periph_GPIOF
#define PWM_TIM_PIN               GPIO_Pin_9
#define PWM_TIM_GPIO_PORT         GPIOF
#define PWM_TIM_SOURCE            GPIO_PinSource9
#define PWM_TIM_AF                GPIO_AF_TIM14
#define PWM_TIM_OCxInit           TIM_OC1Init              //比较输出通道(与引脚相关)
#define PWM_TIM_SetComparex       TIM_SetCompare1          //设置比较值(占空比)

                                                           


#if 0
1.对于16位定时器, 10MHz定时器时钟
  50%占空比, 频率范围：153Hz ~ 5MHz (10MHz/0xFFFF≈152.5)
  0.1%精度占空比, 频率范围：200Hz - 10KHz

2.对于32位定时器, 10MHz定时器时钟
  50%占空比, 频率范围：2Hz ~ 5MHz (占空比0.1%精度 10M * 500 > 0xFFFFFFFF)
  0.1%精度占空比, 频率范围：2Hz - 10KHz
#endif

/* CNT计时 */
#define CNT_TIMx                  TIM12
#define CNT_TIM_CLK               RCC_APB1Periph_TIM12
#define CNT_TIM_GPIO_CLK          RCC_AHB1Periph_GPIOF
#define CNT_TIM_PIN               GPIO_Pin_9
#define CNT_TIM_GPIO_PORT         GPIOF
#define CNT_TIM_SOURCE            GPIO_PinSource9
#define CNT_TIM_AF                GPIO_AF_TIM12
#define CNT_TIM_IRQn              TIM8_BRK_TIM12_IRQn 
#define CNT_TIM_Priority          2
#define CNT_TIM_IRQHandler        TIM8_BRK_TIM12_IRQHandler           //中断函数接口


/* 函数申明 ------------------------------------------------------------------*/
void PWM_TIM_Configuration(void);
void TIM_Initializes(void);
void PWM_Output(uint32_t Frequency, uint32_t Dutycycle);
void ouput_pulse(uint32_t NumPulse);

//void TIM14_PWM_Init(u32 arr,u32 psc);
#endif
