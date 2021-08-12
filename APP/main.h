/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"
#include "delay.h"
#include "led.h"
#include "sys.h"
#include "includes.h"
#include "usart.h"
//#include "exti.h"
#include "pwm.h"
#include "dma.h"


//variable
u8 Time_Register[100];
u8 Signal_Register[100];

u8 time=0;//时间
u8 signal=0;//信号
u8 ext_signal=0;//外部信号

u8  current_state=0;//0:空闲状态    1：设置脉冲数状态    2：运行状态
#define SEND_BUF_SIZE 200  //发送缓冲区长度
#define RECEIVE_BUF_SIZE 200 // 接收缓冲区长度
u8 time_register[100];
u8 signal_register[100];



u8 rx_flag = 0; //数据帧接收标志
u8 task_flag=0;//任务标志位，1表示任务B运行，2表示任务C运行

u8 SendBuff[SEND_BUF_SIZE];//发送缓冲区
u8 Rx_Buff[RECEIVE_BUF_SIZE];//接收缓冲区

u16 UART1_ReceiveSize;            //DMA接收数据的长度
u16 recv_len; //发送数据实际长度
u8 recv_times=0;//接收数据次数

//数据存放
u8 offset=0;//偏移段数
u8 sd=0;//当前数据个数
u8 pulse_offset[100];//每一段的起始位（偏移量）
u32 pulse_num = 2;//总脉冲段数，默认两段脉冲
u32 output_port=0;//输出端子，默认为0
u16 mode=0;//模式
u32 data[600];//脉冲段数据


u8 err_display[200]={"\r\n输入数据错误，请重新输入！"};



u32 PWM_CK_CNT =10000;              
u32 PWM_PRESCALER = (84000000/10000 - 1);  //预分频值



#define START_TASK_PRIO			10  ///开始任务的优先级
#define START_STK_SIZE			512


//任务任务堆栈
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void Start_Task(void *pdata);

//LEDA任务
//设置任务优先级
#define LED0_TASK_PRIO			4
//设置任务堆栈大小
#define LED0_STK_SIZE			512
//任务堆栈
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//任务函数
void Led_Task_A(void *pdata);

//LEDB任务
//设置任务优先级
#define LED1_TASK_PRIO			5
//设置任务堆栈大小
#define LED1_STK_SIZE			512
//任务堆栈
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//任务函数
void Led_Task_B(void *pdata);


//LEDC任务
//设置任务优先级
#define LED2_TASK_PRIO			6
//设置任务堆栈大小
#define LED2_STK_SIZE			512
//任务堆栈
OS_STK LED2_TASK_STK[LED2_STK_SIZE];
//任务函数
void Led_Task_C(void *pdata);

void TimingDelay_Decrement(void);
u32 My_Atoi(char *source); //字符串转整形
void My_Itoa (u16 num,char str[]);//整型转字符串
void Led_Status(u8 state);//LED状态

bool Output_Check(u8 *recv_data);//输出端子有效性检测
bool Number_Sum_Check(u8 *recv_data);//总脉冲段数有效性检测
bool Speed_Check(u8 *recv_data);//脉冲数率有效性检测
bool Number_Check(u8 *recv_data);//脉冲个数有效性检测
bool Mode_Check(u8 *recv_data);//脉冲模式有效性检测
bool Time_Register_Check(u8 *recv_data);//时间寄存器有效性检测
bool Signal_Register_Check(u8 *recv_data);//信号寄存器有效性检测
bool External_Signal_Check(u8 *recv_data);//外部信号端子有效性检测
bool Time_Check(u8 *recv_data);//时间有效性检测
bool Section_Num_Check(u8 *recv_data,u16 sum);//跳转脉冲段序号有效性检测
bool End_Check(u8 *recv_data);//结束指令有效性检测


#endif 