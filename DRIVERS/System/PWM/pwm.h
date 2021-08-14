#ifndef _PWM_H
#define _PWM_H
#include "sys.h"

extern  u8  current_state;
extern u32 PWM_CK_CNT;
extern u16 PWM_PRESCALER;

/* º¯ÊýÉêÃ÷ ------------------------------------------------------------------*/
void PWM_TIM10_Configuration(void);
void PWM_TIM11_Configuration(void);
void PWM_TIM13_Configuration(void);
void PWM_TIM14_Configuration(void);
void Delay_TIM2_Configuration(void);

void CNT_TIM9_Configuration(uint16_t TIM_TS_ITRx);
void CNT_TIM12_Configuration(uint16_t TIM_TS_ITRx);

void PWM_Output(uint32_t Frequency, TIM_TypeDef * PWM_TIMx);
void Pulse_Output_Number(u16 NumPulse,TIM_TypeDef * CNT_TIMx);

//void TIM14_PWM_Init(u32 arr,u32 psc);
#endif
