/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"
//#include "delay.h"
#include "led.h"
#include "sys.h"
//#include "includes.h"
//#include "usart.h"
//#include "exti.h"
//#include "pwm.h"
#include "math.h"


//variable



//OS_EVENT* interrupt_MBOX;//����
//u8 interrupt_times = 0;//0�����ж�   1�����ж�
u8  current_state=0;//0:����״̬    1������������״̬    2������״̬


u32 PWM_CK_CNT =10000 ;               
u32 PWM_PRESCALER = (84000000/10000 - 1);  //Ԥ��Ƶֵ




#define START_TASK_PRIO			10  ///��ʼ��������ȼ�
#define START_STK_SIZE			128

/*
//���������ջ
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);

//LEDA����
//�����������ȼ�
#define LED0_TASK_PRIO			7
//���������ջ��С
#define LED0_STK_SIZE			128
//�����ջ
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//������
void led_taskA(void *pdata);

//LEDB����
//�����������ȼ�
#define LED1_TASK_PRIO			6
//���������ջ��С
#define LED1_STK_SIZE			128
//�����ջ
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//������
void led_taskB(void *pdata);

*/

void TimingDelay_Decrement(void);

#endif 