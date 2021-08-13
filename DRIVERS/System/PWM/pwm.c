#include "pwm.h"

/******************************************* PWM输出 ********************************************/
void PWM_TIM10_Configuration(void)
{
  GPIO_InitTypeDef        GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef       TIM_OCInitStructure;


  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//F6
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOF, &GPIO_InitStructure);


  GPIO_PinAFConfig(GPIOF, GPIO_PinSource6, GPIO_AF_TIM10);


  TIM_TimeBaseStructure.TIM_Prescaler = PWM_PRESCALER; //预分频值          
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数       
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;//ARR值                        
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //时钟分割           
  TIM_TimeBaseInit(TIM10, &TIM_TimeBaseStructure);


  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;   //设置模式，是输出比较还是PWM模式               
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //模式使能     
  TIM_OCInitStructure.TIM_Pulse = 0xFFFF;                            
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //配置有效电平的极性      
  TIM_OC1Init(TIM10, &TIM_OCInitStructure);
  
  TIM_SelectMasterSlaveMode( TIM10, TIM_MasterSlaveMode_Enable);		// 定时器主从模式使能
  TIM_SelectOutputTrigger( TIM10, TIM_TRGOSource_Update);			// 选择触发方式：使用更新事件作为触发输出

  TIM_Cmd(TIM10, DISABLE);
}

void PWM_TIM11_Configuration(void)
{
  GPIO_InitTypeDef        GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef       TIM_OCInitStructure;


  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//F7
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOF, &GPIO_InitStructure);


  GPIO_PinAFConfig(GPIOF, GPIO_PinSource7, GPIO_AF_TIM11);


  TIM_TimeBaseStructure.TIM_Prescaler = PWM_PRESCALER; //预分频值          
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数       
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;//ARR值                        
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //时钟分割           
  TIM_TimeBaseInit(TIM11, &TIM_TimeBaseStructure);


  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;   //设置模式，是输出比较还是PWM模式               
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //模式使能     
  TIM_OCInitStructure.TIM_Pulse = 0xFFFF;                            
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //配置有效电平的极性      
  TIM_OC1Init(TIM11, &TIM_OCInitStructure);
  
  TIM_SelectMasterSlaveMode( TIM11, TIM_MasterSlaveMode_Enable);		// 定时器主从模式使能
  TIM_SelectOutputTrigger( TIM11, TIM_TRGOSource_Update);			// 选择触发方式：使用更新事件作为触发输出

  TIM_Cmd(TIM11, DISABLE);
}
  
void PWM_TIM13_Configuration(void)
{
  GPIO_InitTypeDef        GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef       TIM_OCInitStructure;


  RCC_APB2PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//F8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOF, &GPIO_InitStructure);


  GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF_TIM13);


  TIM_TimeBaseStructure.TIM_Prescaler = PWM_PRESCALER; //预分频值          
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数       
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;//ARR值                        
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //时钟分割           
  TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);


  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;   //设置模式，是输出比较还是PWM模式               
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //模式使能     
  TIM_OCInitStructure.TIM_Pulse = 0xFFFF;                            
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //配置有效电平的极性      
  TIM_OC1Init(TIM13, &TIM_OCInitStructure);
  
  TIM_SelectMasterSlaveMode( TIM13, TIM_MasterSlaveMode_Enable);		// 定时器主从模式使能
  TIM_SelectOutputTrigger( TIM13, TIM_TRGOSource_Update);			// 选择触发方式：使用更新事件作为触发输出

  TIM_Cmd(TIM13, DISABLE);
}


void PWM_TIM14_Configuration(void)
{
  GPIO_InitTypeDef        GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef       TIM_OCInitStructure;


  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//F9
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOF, &GPIO_InitStructure);


  GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_TIM14);


  TIM_TimeBaseStructure.TIM_Prescaler = PWM_PRESCALER;           
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;        
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;                        
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;            
  TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);


  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                  
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;      
  TIM_OCInitStructure.TIM_Pulse = 0xFFFF;                            
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;          
  TIM_OC1Init(TIM14, &TIM_OCInitStructure);
  
  TIM_SelectMasterSlaveMode( TIM14, TIM_MasterSlaveMode_Enable);		// 定时器主从模式使能
  TIM_SelectOutputTrigger( TIM14, TIM_TRGOSource_Update);			// 选择触发方式：使用更新事件作为触发输出

  TIM_Cmd(TIM14, DISABLE);
}





/************************************************
 计时定时器配置
*************************************************/
void CNT_TIM12_Configuration(uint16_t TIM_TS_ITRx)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef        NVIC_InitStructure;

  /* 时钟配置 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);


  /* NVIC配置 */
  NVIC_InitStructure.NVIC_IRQChannel = TIM8_BRK_TIM12_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* 时基配置 */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;                           //预分频值
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;        //向上计数
  TIM_TimeBaseStructure.TIM_Period = 0x14;                         //定时周期
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;            //分频因子
  TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);
  
  TIM_SelectInputTrigger( TIM12, TIM_TS_ITRx);
  TIM_SelectSlaveMode( TIM12,TIM_SlaveMode_External1);		// 等同 TIM12->SMCR |= 0x07,外部时钟模式1


  TIM_ClearFlag(TIM12, TIM_FLAG_Update);
  TIM_ITConfig(TIM12, TIM_IT_Update, ENABLE);                     //使能"更新"中断

  TIM_Cmd(TIM12, ENABLE);
}

void CNT_TIM9_Configuration(uint16_t TIM_TS_ITRx)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef        NVIC_InitStructure;

  /* 时钟配置 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);


  /* NVIC配置 */
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* 时基配置 */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;                           //预分频值
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;        //向上计数
  TIM_TimeBaseStructure.TIM_Period = 0x14;                         //定时周期
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;            //分频因子
  TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);
  
  TIM_SelectInputTrigger(TIM9, TIM_TS_ITRx);
  TIM_SelectSlaveMode( TIM9,TIM_SlaveMode_External1 );		// 等同 TIM12->SMCR |= 0x07


  TIM_ClearFlag(TIM9, TIM_FLAG_Update);
  TIM_ITConfig(TIM9, TIM_IT_Update, ENABLE);                     //使能"更新"中断

  TIM_Cmd(TIM9, ENABLE);
}

/************************************************
    Frequency --- 频率
*************************************************/
void PWM_Output(uint32_t Frequency, TIM_TypeDef * PWM_TIMx)//脉冲频率设置
{
  uint32_t pwm_period;
  uint32_t pwm_pulse;

  /* 输出PWM */
  pwm_period = PWM_CK_CNT/Frequency - 1;                               //计算出计数周期(决定输出的频率)
  pwm_pulse  = (pwm_period + 1)*50 / 100;                              //计算出脉宽值(决定PWM占空比)50%


    if((0xFFFF < pwm_period) || (0xFFFF < pwm_pulse))                //16位定时器：period和pulse都不能超过0xFFFF
    {
      return;
    }

  TIM_Cmd(PWM_TIMx, DISABLE);                                        //失能TIM
  TIM_SetCounter(PWM_TIMx, 0);                                       //定时器计数清零
  TIM_SetAutoreload(PWM_TIMx, pwm_period);                           //更改频率，设置ARR的值
  TIM_SetCompare1(PWM_TIMx, pwm_pulse);                              //更改占空比
  TIM_Cmd(PWM_TIMx, ENABLE);                                         //使能TIM
  
}

void Pulse_Output_Number(u16 NumPulse, TIM_TypeDef * CNT_TIMx)//脉冲个数设置
{
 TIM_Cmd(CNT_TIMx, DISABLE);
 TIM_SetCounter(CNT_TIMx, 0);
 TIM_SetAutoreload(CNT_TIMx, NumPulse-1);                           //设置中断更新数
 TIM_ClearFlag(CNT_TIMx, TIM_FLAG_Update);
 TIM_Cmd(CNT_TIMx, ENABLE);
}





