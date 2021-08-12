#ifndef _PWM_H
#define _PWM_H
#include "sys.h"

extern  u8  current_state;

extern u32 PWM_CK_CNT;
extern u32 PWM_PRESCALER;
/* �궨�� --------------------------------------------------------------------*/
/* PWM��� */


#define PWM_TIM_PIN               GPIO_Pin_9
#define PWM_TIM_GPIO_PORT         GPIOF
#define PWM_TIM_SOURCE            GPIO_PinSource9
#define PWM_TIM_AF                GPIO_AF_TIM14
#define PWM_TIM_OCxInit           TIM_OC1Init              //�Ƚ����ͨ��(���������)
#define PWM_TIM_SetComparex       TIM_SetCompare1          //���ñȽ�ֵ(ռ�ձ�)






/* CNT��ʱ */
#define CNT_TIMx                  TIM12
#define CNT_TIM_CLK               RCC_APB1Periph_TIM12
#define CNT_TIM_GPIO_CLK          RCC_AHB1Periph_GPIOF
#define CNT_TIM_PIN               GPIO_Pin_9
#define CNT_TIM_GPIO_PORT         GPIOF
#define CNT_TIM_SOURCE            GPIO_PinSource9
#define CNT_TIM_AF                GPIO_AF_TIM12
#define CNT_TIM_IRQn              TIM8_BRK_TIM12_IRQn 
#define CNT_TIM_Priority          2
#define CNT_TIM_IRQHandler        TIM8_BRK_TIM12_IRQHandler           //�жϺ����ӿ�


/* �������� ------------------------------------------------------------------*/
void PWM_TIM_Configuration(void);
void TIM_Initializes(void);
void PWM_Output(uint32_t Frequency, uint32_t Dutycycle);
void ouput_pulse(uint32_t NumPulse);

//void TIM14_PWM_Init(u32 arr,u32 psc);
#endif
