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

u8 time=0;//ʱ��
u8 signal=0;//�ź�
u8 ext_signal=0;//�ⲿ�ź�

u8  current_state=0;//0:����״̬    1������������״̬    2������״̬
#define SEND_BUF_SIZE 200  //���ͻ���������
#define RECEIVE_BUF_SIZE 200 // ���ջ���������
u8 time_register[100];
u8 signal_register[100];



u8 rx_flag = 0; //����֡���ձ�־
u8 task_flag=0;//�����־λ��1��ʾ����B���У�2��ʾ����C����

u8 SendBuff[SEND_BUF_SIZE];//���ͻ�����
u8 Rx_Buff[RECEIVE_BUF_SIZE];//���ջ�����

u16 UART1_ReceiveSize;            //DMA�������ݵĳ���
u16 recv_len; //��������ʵ�ʳ���
u8 recv_times=0;//�������ݴ���

//���ݴ��
u8 offset=0;//ƫ�ƶ���
u8 sd=0;//��ǰ���ݸ���
u8 pulse_offset[100];//ÿһ�ε���ʼλ��ƫ������
u32 pulse_num = 2;//�����������Ĭ����������
u32 output_port=0;//������ӣ�Ĭ��Ϊ0
u16 mode=0;//ģʽ
u32 data[600];//���������


u8 err_display[200]={"\r\n�������ݴ������������룡"};



u32 PWM_CK_CNT =10000;              
u32 PWM_PRESCALER = (84000000/10000 - 1);  //Ԥ��Ƶֵ



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

//LEDB����
//�����������ȼ�
#define LED1_TASK_PRIO			5
//���������ջ��С
#define LED1_STK_SIZE			512
//�����ջ
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//������
void Led_Task_B(void *pdata);


//LEDC����
//�����������ȼ�
#define LED2_TASK_PRIO			6
//���������ջ��С
#define LED2_STK_SIZE			512
//�����ջ
OS_STK LED2_TASK_STK[LED2_STK_SIZE];
//������
void Led_Task_C(void *pdata);

void TimingDelay_Decrement(void);
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


#endif 