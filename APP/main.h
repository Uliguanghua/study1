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
#include "exti.h"
#include "pwm.h"
#include "dma.h"
#include "stmflash.h"

#define FLASH_SAVE_ADDR  0X08040000	//FLASH�����ַ
#define SEND_BUF_SIZE 200  //���ͻ���������
#define RECEIVE_BUF_SIZE 650 // ���ջ���������

u8 SendBuff[SEND_BUF_SIZE];//���ͻ�����
u8 Rx_Buff[RECEIVE_BUF_SIZE];//���ջ�����

OS_EVENT* DATA_MBOX;//����


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
  u8  interruput_times;//�������жϴ��� 
  TIM_TypeDef * CNT_TIMx;//������ʱ��
  TIM_TypeDef * PWM_TIMx;//PWM��ʱ��
  u8 ext_signal;//�ⲿ�ź�
  //��������

  u16 pulse_remainder;//��������
  u8 pulse_offset[20];//ÿһ�ε���ʼλ��ƫ������
  u32 pulse_num;//�����������Ĭ����������
  u32 data[101];//��������� 
}Volume;



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
void Set_Task(void *pdata);

//��������
//�����������ȼ�
#define LED1_TASK_PRIO			5
//���������ջ��С
#define LED1_STK_SIZE			512
//�����ջ
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//������
void Run_Task(void *pdata);


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




/**************��������*******************/

u32 My_Atoi(char *source); //�ַ���ת����
void My_Itoa (u16 num,char str[]);//����ת�ַ���


void Led_Status(u8 state);//LED״̬


//������Ч�Լ��
bool Output_Check(u8 *recv_data);//���������Ч�Լ��
bool Number_Sum_Check(u8 *recv_data);//�����������Ч�Լ��
bool Speed_Check(u8 *recv_data);//����������Ч�Լ��
bool Number_Check(u8 *recv_data);//���������Ч�Լ��
bool Mode_Check(u8 *recv_data);//����ģʽ��Ч�Լ��
bool External_Signal_Check(u8 *recv_data);//�ⲿ�źŶ�����Ч�Լ��
bool Time_Check(u8 *recv_data);//ʱ����Ч�Լ��
bool Section_Num_Check(u8 *recv_data,u16 sum);//��ת����������Ч�Լ��
bool End_Check(u8 *recv_data);//����ָ����Ч�Լ��
u8 Data_Check(void);//����֡У��,���󷵻ش���ţ���ȷ����0

//��ʼ��
void data_init(void);//���ݳ�ʼ��
void Output_Place(u32 data);//����ָ����ʱ����ʼ��

//���ݱ���������
void Frequency_Select(u32 *PWM_CK_CNT,u16 *PWM_PRESCALER,TIM_TypeDef * PWM_TIMx,u32 frequency,u32 port);//Ƶ��ѡ��
void Print_Mode_Switch(u8 * send_data);//���ݴ�ӡ
void Data_Save(void);//���ݱ���
void Pluse_Number(u8 sd);////���ݸ��������жϴ���
void Err_Print(u8 err ,u8 *message);//�����ӡ
void Print_Data(void);//�ϴ����ݴ�ӡ
#endif 