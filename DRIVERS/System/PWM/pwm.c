#include "pwm.h"

/******************************************* PWM��� ********************************************/
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


  TIM_TimeBaseStructure.TIM_Prescaler = PWM_PRESCALER; //Ԥ��Ƶֵ          
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���       
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;//ARRֵ                        
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ʱ�ӷָ�           
  TIM_TimeBaseInit(TIM10, &TIM_TimeBaseStructure);


  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;   //����ģʽ��������Ƚϻ���PWMģʽ               
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //ģʽʹ��     
  TIM_OCInitStructure.TIM_Pulse = 0xFFFF;                            
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //������Ч��ƽ�ļ���      
  TIM_OC1Init(TIM10, &TIM_OCInitStructure);
  
  TIM_SelectMasterSlaveMode( TIM10, TIM_MasterSlaveMode_Enable);		// ��ʱ������ģʽʹ��
  TIM_SelectOutputTrigger( TIM10, TIM_TRGOSource_Update);			// ѡ�񴥷���ʽ��ʹ�ø����¼���Ϊ�������

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


  TIM_TimeBaseStructure.TIM_Prescaler = PWM_PRESCALER; //Ԥ��Ƶֵ          
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���       
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;//ARRֵ                        
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ʱ�ӷָ�           
  TIM_TimeBaseInit(TIM11, &TIM_TimeBaseStructure);


  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;   //����ģʽ��������Ƚϻ���PWMģʽ               
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //ģʽʹ��     
  TIM_OCInitStructure.TIM_Pulse = 0xFFFF;                            
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //������Ч��ƽ�ļ���      
  TIM_OC1Init(TIM11, &TIM_OCInitStructure);
  
  TIM_SelectMasterSlaveMode( TIM11, TIM_MasterSlaveMode_Enable);		// ��ʱ������ģʽʹ��
  TIM_SelectOutputTrigger( TIM11, TIM_TRGOSource_Update);			// ѡ�񴥷���ʽ��ʹ�ø����¼���Ϊ�������

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


  TIM_TimeBaseStructure.TIM_Prescaler = PWM_PRESCALER; //Ԥ��Ƶֵ          
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���       
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;//ARRֵ                        
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ʱ�ӷָ�           
  TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);


  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;   //����ģʽ��������Ƚϻ���PWMģʽ               
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //ģʽʹ��     
  TIM_OCInitStructure.TIM_Pulse = 0xFFFF;                            
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //������Ч��ƽ�ļ���      
  TIM_OC1Init(TIM13, &TIM_OCInitStructure);
  
  TIM_SelectMasterSlaveMode( TIM13, TIM_MasterSlaveMode_Enable);		// ��ʱ������ģʽʹ��
  TIM_SelectOutputTrigger( TIM13, TIM_TRGOSource_Update);			// ѡ�񴥷���ʽ��ʹ�ø����¼���Ϊ�������

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
  
  TIM_SelectMasterSlaveMode( TIM14, TIM_MasterSlaveMode_Enable);		// ��ʱ������ģʽʹ��
  TIM_SelectOutputTrigger( TIM14, TIM_TRGOSource_Update);			// ѡ�񴥷���ʽ��ʹ�ø����¼���Ϊ�������

  TIM_Cmd(TIM14, DISABLE);
}





/************************************************
 ��ʱ��ʱ������
*************************************************/
void CNT_TIM12_Configuration(uint16_t TIM_TS_ITRx)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef        NVIC_InitStructure;

  /* ʱ������ */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);


  /* NVIC���� */
  NVIC_InitStructure.NVIC_IRQChannel = TIM8_BRK_TIM12_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* ʱ������ */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;                           //Ԥ��Ƶֵ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;        //���ϼ���
  TIM_TimeBaseStructure.TIM_Period = 0x14;                         //��ʱ����
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;            //��Ƶ����
  TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);
  
  TIM_SelectInputTrigger( TIM12, TIM_TS_ITRx);
  TIM_SelectSlaveMode( TIM12,TIM_SlaveMode_External1);		// ��ͬ TIM12->SMCR |= 0x07,�ⲿʱ��ģʽ1


  TIM_ClearFlag(TIM12, TIM_FLAG_Update);
  TIM_ITConfig(TIM12, TIM_IT_Update, ENABLE);                     //ʹ��"����"�ж�

  TIM_Cmd(TIM12, ENABLE);
}

void CNT_TIM9_Configuration(uint16_t TIM_TS_ITRx)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef        NVIC_InitStructure;

  /* ʱ������ */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);


  /* NVIC���� */
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* ʱ������ */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;                           //Ԥ��Ƶֵ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;        //���ϼ���
  TIM_TimeBaseStructure.TIM_Period = 0x14;                         //��ʱ����
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;            //��Ƶ����
  TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);
  
  TIM_SelectInputTrigger(TIM9, TIM_TS_ITRx);
  TIM_SelectSlaveMode( TIM9,TIM_SlaveMode_External1 );		// ��ͬ TIM12->SMCR |= 0x07


  TIM_ClearFlag(TIM9, TIM_FLAG_Update);
  TIM_ITConfig(TIM9, TIM_IT_Update, ENABLE);                     //ʹ��"����"�ж�

  TIM_Cmd(TIM9, ENABLE);
}

/************************************************
    Frequency --- Ƶ��
*************************************************/
void PWM_Output(uint32_t Frequency, TIM_TypeDef * PWM_TIMx)//����Ƶ������
{
  uint32_t pwm_period;
  uint32_t pwm_pulse;

  /* ���PWM */
  pwm_period = PWM_CK_CNT/Frequency - 1;                               //�������������(���������Ƶ��)
  pwm_pulse  = (pwm_period + 1)*50 / 100;                              //���������ֵ(����PWMռ�ձ�)50%


    if((0xFFFF < pwm_period) || (0xFFFF < pwm_pulse))                //16λ��ʱ����period��pulse�����ܳ���0xFFFF
    {
      return;
    }

  TIM_Cmd(PWM_TIMx, DISABLE);                                        //ʧ��TIM
  TIM_SetCounter(PWM_TIMx, 0);                                       //��ʱ����������
  TIM_SetAutoreload(PWM_TIMx, pwm_period);                           //����Ƶ�ʣ�����ARR��ֵ
  TIM_SetCompare1(PWM_TIMx, pwm_pulse);                              //����ռ�ձ�
  TIM_Cmd(PWM_TIMx, ENABLE);                                         //ʹ��TIM
  
}

void Pulse_Output_Number(u16 NumPulse, TIM_TypeDef * CNT_TIMx)//�����������
{
 TIM_Cmd(CNT_TIMx, DISABLE);
 TIM_SetCounter(CNT_TIMx, 0);
 TIM_SetAutoreload(CNT_TIMx, NumPulse-1);                           //�����жϸ�����
 TIM_ClearFlag(CNT_TIMx, TIM_FLAG_Update);
 TIM_Cmd(CNT_TIMx, ENABLE);
}





