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



//OS_EVENT* interrupt_MBOX;//邮箱
//u8 interrupt_times = 0;//0：无中断   1：有中断
u8  current_state=0;//0:空闲状态    1：设置脉冲数状态    2：运行状态


u32 PWM_CK_CNT =10000 ;               
u32 PWM_PRESCALER = (84000000/10000 - 1);  //预分频值




#define START_TASK_PRIO			10  ///开始任务的优先级
#define START_STK_SIZE			128

/*
//任务任务堆栈
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);

//LEDA任务
//设置任务优先级
#define LED0_TASK_PRIO			7
//设置任务堆栈大小
#define LED0_STK_SIZE			128
//任务堆栈
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//任务函数
void led_taskA(void *pdata);

//LEDB任务
//设置任务优先级
#define LED1_TASK_PRIO			6
//设置任务堆栈大小
#define LED1_STK_SIZE			128
//任务堆栈
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//任务函数
void led_taskB(void *pdata);

*/

void TimingDelay_Decrement(void);

#endif 