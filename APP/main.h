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



#define SEND_BUF_SIZE 200  //发送缓冲区长度
#define RECEIVE_BUF_SIZE 650 // 接收缓冲区长度

u8 SendBuff[SEND_BUF_SIZE];//发送缓冲区
u8 Rx_Buff[RECEIVE_BUF_SIZE];//接收缓冲区

OS_EVENT* DATA_MBOX;//邮箱

/*********************模拟值***************/
struct S{
  
  u8 time;//时间
  u8 ext_signal;//外部信号

}simulation;

/*********************标志***************/
struct F{
  u8  current_state;//0:空闲状态    1：设置脉冲数状态    2：运行状态
  u8 rx_flag; //数据帧接收标志
  u8 task_flag;//任务标志位，1表示任务B运行，2表示任务C运行
  u8 send_finish_flag;//发送一段脉冲完成标志位

}flag;


/*****************数据量****************/

struct V{
  //计量数据
  u16 UART1_ReceiveSize; //DMA接收数据的长度
  u16 recv_len; //发送数据实际长度
  u8  recv_times;//接收数据次数
  u8  interruput_times;//计数器中断次数 
  TIM_TypeDef * CNT_TIMx;//计数定时器
  TIM_TypeDef * PWM_TIMx;//PWM定时器

  //参数数据
  u8 sd;//当前数据段数
  u16 pulse_remainder;//脉冲余数
  u8 pulse_offset[20];//每一段的起始位（偏移量）
  u32 pulse_num;//总脉冲段数，默认两段脉冲
  u32 output_port;//输出端子，默认为0
  u16 mode;//脉冲段模式
  u32 data[101];//脉冲段数据
  u8 pulse_data[21][31];//脉冲数据指令
  
  
}Volume;






u32 PWM_CK_CNT =10000;    //计数周期    10k/s      
u16 PWM_PRESCALER = (84000000/10000 - 1);  //预分频值





/**************任务堆栈********************/

//开始任务
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

//设置任务
//设置任务优先级
#define LED1_TASK_PRIO			5
//设置任务堆栈大小
#define LED1_STK_SIZE			512
//任务堆栈
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//任务函数
void Led_Task_B(void *pdata);


//运行任务
//设置任务优先级
#define LED2_TASK_PRIO			6
//设置任务堆栈大小
#define LED2_STK_SIZE			512
//任务堆栈
OS_STK LED2_TASK_STK[LED2_STK_SIZE];
//任务函数
void Led_Task_C(void *pdata);

//数据处理任务
//设置任务优先级
#define DATA_TASK_PRIO			3
//设置任务堆栈大小
#define DATA_STK_SIZE			512
//任务堆栈
OS_STK DATA_TASK_STK[DATA_STK_SIZE];
//任务函数
void DATA_Task(void *pdata);


void data_init()//数据初始化
{
  Volume.pulse_num=2;//两段脉冲
  
  Volume.CNT_TIMx=TIM9;
  Volume.PWM_TIMx=TIM10;//端子定时器指定
  Volume.pulse_offset[0]=1;
  Volume.pulse_offset[1]=6;
  //两段脉冲，Y0输出，速率100和10，个数为1000和100个，等待模式为脉冲发送完成模式和等待时间模式，使用时间寄存器T1，时间为500ms跳转默认下段一
  Volume.data[0] = 0x00000002;
  Volume.data[1] = 0x0000000A;
  Volume.data[2] = 0x00000064;
  Volume.data[3] = 0x00010000;
  Volume.data[4] = 0x00000000;
  Volume.data[5] = 0x00000000;
  Volume.data[6] = 0x00000002;
  Volume.data[7] = 0x00000014;
  Volume.data[8] = 0x00020001;
  Volume.data[9] = 0x000001F4;
  Volume.data[10] = 0x00000001;
   
}

/**************函数声明*******************/

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
void Output_Place(u32 data);//端子指定定时器初始化
void Frequency_Select(u32 *PWM_CK_CNT,u16 *PWM_PRESCALER,TIM_TypeDef * PWM_TIMx,u32 frequency,u32 port);//频率选择
void Print_Mode_Switch(u8 * send_data);//数据打印
u8 Data_Check(void);//数据帧校验,错误返回错误号，正确返回0
void Data_Save(void);//数据保存
#endif 