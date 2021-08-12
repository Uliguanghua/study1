#include "pwm.h"

/******************************************* PWM输出 ********************************************/

void PWM_TIM_Configuration(void)
{
  GPIO_InitTypeDef        GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef       TIM_OCInitStructure;


  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

  GPIO_InitStructure.GPIO_Pin = PWM_TIM_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(PWM_TIM_GPIO_PORT, &GPIO_InitStructure);


  GPIO_PinAFConfig(PWM_TIM_GPIO_PORT, PWM_TIM_SOURCE, PWM_TIM_AF);


  TIM_TimeBaseStructure.TIM_Prescaler = PWM_PRESCALER;           
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;        
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;                        
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;            
  TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);


  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                  
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;      
  TIM_OCInitStructure.TIM_Pulse = 0xFFFF;                            
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;          
  PWM_TIM_OCxInit(TIM14, &TIM_OCInitStructure);
  
  TIM_SelectMasterSlaveMode( TIM14, TIM_MasterSlaveMode_Enable);		// 定时器主从模式使能
  TIM_SelectOutputTrigger( TIM14, TIM_TRGOSource_Update);			// 选择触发方式：使用更新事件作为触发输出

  TIM_Cmd(TIM14, DISABLE);
}

/************************************************
函数名称 ： CNT_TIM_Configuration
功    能 ： 计时定时器配置
*************************************************/
void CNT_TIM_Configuration(void)
{
  //GPIO_InitTypeDef        GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef        NVIC_InitStructure;

  /* 时钟配置 */
  RCC_APB1PeriphClockCmd(CNT_TIM_CLK, ENABLE);


  /* NVIC配置 */
  NVIC_InitStructure.NVIC_IRQChannel = CNT_TIM_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = CNT_TIM_Priority;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* 时基配置 */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;                           //预分频值
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;        //向上计数
  TIM_TimeBaseStructure.TIM_Period = 0x14;                         //定时周期
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;            //分频因子
  TIM_TimeBaseInit(CNT_TIMx, &TIM_TimeBaseStructure);
  
  TIM_SelectInputTrigger( CNT_TIMx, TIM_TS_ITR3);
  TIM_SelectSlaveMode( CNT_TIMx,TIM_SlaveMode_External1 );		// 等同 TIM12->SMCR |= 0x07


  TIM_ClearFlag(CNT_TIMx, TIM_FLAG_Update);
  TIM_ITConfig(CNT_TIMx, TIM_IT_Update, ENABLE);                     //使能"更新"中断

  TIM_Cmd(CNT_TIMx, ENABLE);
}

/************************************************
    Frequency --- 频率
    Dutycycle --- 占空比(12代表占空比为12%)
*************************************************/
void PWM_Output(uint32_t Frequency, uint32_t Dutycycle)
{
  uint32_t pwm_period;
  uint32_t pwm_pulse;

  /* 输出PWM */
  pwm_period = PWM_CK_CNT/Frequency - 1;                             //计算出计数周期(决定输出的频率)
  pwm_pulse  = (pwm_period + 1)*Dutycycle / 100;                     //计算出脉宽值(决定PWM占空比)


    if((0xFFFF < pwm_period) || (0xFFFF < pwm_pulse))                //16位定时器：period和pulse都不能超过0xFFFF
    {
      return;
    }

  TIM_Cmd(TIM14, DISABLE);                                        //失能TIM
  TIM_SetCounter(TIM14, 0);                                       //计数清零
  TIM_SetAutoreload(TIM14, pwm_period);                           //更改频率
  PWM_TIM_SetComparex(TIM14, pwm_pulse);                          //更改占空比
  TIM_Cmd(TIM14, ENABLE);                                         //使能TIM
  
}

void ouput_pulse(uint32_t NumPulse)
{
 TIM_Cmd(CNT_TIMx, DISABLE);
 TIM_SetCounter(CNT_TIMx, 0);
 TIM_SetAutoreload(CNT_TIMx, NumPulse-1);                           //设置中断更新数
 TIM_ClearFlag(CNT_TIMx, TIM_FLAG_Update);
 TIM_Cmd(CNT_TIMx, ENABLE);
}
 
/************************************************
 TIM定时器初始化
*************************************************/
void TIM_Initializes(void)
{
  PWM_TIM_Configuration();
  CNT_TIM_Configuration();
}


/**** Copyright (C)2018 strongerHuang. All Rights Reserved **** END OF FILE ****/


