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



#define SEND_BUF_SIZE 200  //���ͻ���������
#define RECEIVE_BUF_SIZE 650 // ���ջ���������

u8 SendBuff[SEND_BUF_SIZE];//���ͻ�����
u8 Rx_Buff[RECEIVE_BUF_SIZE];//���ջ�����

OS_EVENT* DATA_MBOX;//����

/*********************ģ��ֵ***************/
struct S{
  
  u8 time;//ʱ��
  u8 ext_signal;//�ⲿ�ź�

}simulation;

/*********************��־***************/
struct F{
  u8  current_state;//0:����״̬    1������������״̬    2������״̬
  u8 rx_flag; //����֡���ձ�־
  u8 task_flag;//�����־λ��1��ʾ����B���У�2��ʾ����C����
  u8 send_finish_flag;//����һ��������ɱ�־λ

}flag;


/*****************������****************/

struct V{
  //��������
  u16 UART1_ReceiveSize; //DMA�������ݵĳ���
  u16 recv_len; //��������ʵ�ʳ���
  u8  recv_times;//�������ݴ���
  u8  interruput_times;//�������жϴ��� 
  TIM_TypeDef * CNT_TIMx;//������ʱ��
  TIM_TypeDef * PWM_TIMx;//PWM��ʱ��

  //��������
  u8 sd;//��ǰ���ݶ���
  u16 pulse_remainder;//��������
  u8 pulse_offset[20];//ÿһ�ε���ʼλ��ƫ������
  u32 pulse_num;//�����������Ĭ����������
  u32 output_port;//������ӣ�Ĭ��Ϊ0
  u16 mode;//�����ģʽ
  u32 data[101];//���������
  u8 pulse_data[21][31];//��������ָ��
  
  
}Volume;


u8 err_display[200]={"\r\n�������ݴ������������룡"};



u32 PWM_CK_CNT =10000;    //��������    10k/s      
u16 PWM_PRESCALER = (84000000/10000 - 1);  //Ԥ��Ƶֵ





/**************�����ջ********************/

//��ʼ����
#define START_TASK_PRIO			10  ///��ʼ��������ȼ�
#define START_STK_SIZE			512
//���������ջ
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void Start_Task(void *pdata);

//LEDA����
//�����������ȼ�
#define LED0_TASK_PRIO			4
//���������ջ��С
#define LED0_STK_SIZE			512
//�����ջ
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//������
void Led_Task_A(void *pdata);

//��������
//�����������ȼ�
#define LED1_TASK_PRIO			5
//���������ջ��С
#define LED1_STK_SIZE			512
//�����ջ
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//������
void Led_Task_B(void *pdata);


//��������
//�����������ȼ�
#define LED2_TASK_PRIO			6
//���������ջ��С
#define LED2_STK_SIZE			512
//�����ջ
OS_STK LED2_TASK_STK[LED2_STK_SIZE];
//������
void Led_Task_C(void *pdata);

//���ݴ�������
//�����������ȼ�
#define DATA_TASK_PRIO			3
//���������ջ��С
#define DATA_STK_SIZE			512
//�����ջ
OS_STK DATA_TASK_STK[DATA_STK_SIZE];
//������
void DATA_Task(void *pdata);


void data_init()//���ݳ�ʼ��
{
  Volume.pulse_num=2;//��������
  
  Volume.CNT_TIMx=TIM9;
  Volume.PWM_TIMx=TIM10;//���Ӷ�ʱ��ָ��
  Volume.pulse_offset[0]=1;
  Volume.pulse_offset[1]=6;
  //�������壬Y0���������100��10������Ϊ1000��100�����ȴ�ģʽΪ���巢�����ģʽ�͵ȴ�ʱ��ģʽ��ʹ��ʱ��Ĵ���T1��ʱ��Ϊ500ms��תĬ���¶�һ
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

/**************��������*******************/

u32 My_Atoi(char *source); //�ַ���ת����
void My_Itoa (u16 num,char str[]);//����ת�ַ���
void Led_Status(u8 state);//LED״̬

bool Output_Check(u8 *recv_data);//���������Ч�Լ��
bool Number_Sum_Check(u8 *recv_data);//�����������Ч�Լ��
bool Speed_Check(u8 *recv_data);//����������Ч�Լ��
bool Number_Check(u8 *recv_data);//���������Ч�Լ��
bool Mode_Check(u8 *recv_data);//����ģʽ��Ч�Լ��
bool Time_Register_Check(u8 *recv_data);//ʱ��Ĵ�����Ч�Լ��
bool Signal_Register_Check(u8 *recv_data);//�źżĴ�����Ч�Լ��
bool External_Signal_Check(u8 *recv_data);//�ⲿ�źŶ�����Ч�Լ��
bool Time_Check(u8 *recv_data);//ʱ����Ч�Լ��
bool Section_Num_Check(u8 *recv_data,u16 sum);//��ת����������Ч�Լ��
bool End_Check(u8 *recv_data);//����ָ����Ч�Լ��
void Output_Place(u32 data);//����ָ����ʱ����ʼ��
void Frequency_Select(u32 *PWM_CK_CNT,u16 *PWM_PRESCALER,TIM_TypeDef * PWM_TIMx,u32 frequency,u32 port);//Ƶ��ѡ��
void Print_Mode_Switch(u8 * send_data);//���ݴ�ӡ

#endif 