#include "main.h"


RCC_ClocksTypeDef get_rcc_clock;
/************************������*******************************************/
int main(void)
{
        
        RCC_GetClocksFreq(&get_rcc_clock);//�鿴ϵͳʱ��
        
        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//ϵͳ�ж����ȼ�����
        
        
        SystemInit();          //ʱ�ӳ�ʼ��
	delay_init(168);     //��ʱ��ʼ��       
        LED_Init();          //LED��ʼ��
        EXIT_Init_All();    //��ʼ�������ж�
	OSInit();          //UCOS��ʼ��        
	uart_init(9600);	//���ڳ�ʼ��
     
        STMFLASH_Read(FLASH_SAVE_ADDR,Volume.data,101);//FLAH��
        Delay_TIM2_Configuration();//������ʱ����ʼ��
        MYDMASEND_Config((u32)&(USART1->DR),(u32)SendBuff,SEND_BUF_SIZE);//DMA���ͳ�ʼ��
        MYDMAREC_Config((u32)&(USART1->DR),(u32)Rx_Buff,RECEIVE_BUF_SIZE);//DMA���ճ�ʼ��
        
        DATA_MBOX=OSMboxCreate((void*)0);//������Ϣ����
       
	OSTaskCreate(Start_Task,
                     (void*)0,
                     (OS_STK*)&START_TASK_STK[START_STK_SIZE-1],
                     START_TASK_PRIO); //������ʼ����
	OSStart(); //��ʼ����
}

/*****************************����**************************************/
//��ʼ����
void Start_Task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
	OSStatInit();  //����ͳ������
        Led_Status(flag.current_state);//ģʽ���� 
        u8 start[]={"������ָ���л�״̬��go_set�л�������״̬��go_start�л�������״̬\r\n"}; 
        Print_Mode_Switch(start);//��ӡģʽ��Ϣ
	
	OS_ENTER_CRITICAL();  //�����ٽ���(�ر��ж�)
	//OSTaskCreate(Led_Task_A,(void*)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);//����LEDA����
        OSTaskCreate(Set_Task,(void*)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);//����LEDB����
        OSTaskCreate(Run_Task,(void*)0,(OS_STK*)&LED2_TASK_STK[LED2_STK_SIZE-1],LED2_TASK_PRIO);//����LEDC����
        OSTaskCreate(DATA_Task,(void*)0,(OS_STK*)&DATA_TASK_STK[DATA_STK_SIZE-1],DATA_TASK_PRIO);//�������ݴ�������
	OSTaskSuspend(START_TASK_PRIO);//����ʼ����
	OS_EXIT_CRITICAL();  //�˳��ٽ���(���ж�)
}


//��������
void Set_Task(void *pdata)
{
  u8 message[200];//����ģʽ�µ���Ϣ��ӡ
  memset(message,0,200);
  while(1)
  {
    if(flag.current_state==1 && flag.rx_flag == 1) 
    {
      if(0 == strcmp((char const *)Rx_Buff,"exit") )
      {
        memset(Rx_Buff,0,650);//��ս������ݻ�����
        flag.current_state=0;//�ص�����
        flag.rx_flag = 0;
        Led_Status(flag.current_state);//ģʽ���� 
        
      }else
      {
        u8 err= Data_Check();//���ݼ��
        if(0 == err )
        {          
          strcpy((char *)message,"�������óɹ�!");
          Print_Mode_Switch(message);
          memset(message,0,200);
          flag.current_state=0;//�ص�����
          flag.rx_flag = 0;
          Led_Status(flag.current_state);//ģʽ���� 
          
          //���ݱ���
          memset(Volume.data,0,101);//�������
          Data_Save();//���ݱ���
          STMFLASH_Write(FLASH_SAVE_ADDR,Volume.data,101);//FLASHд
          
          memset(Rx_Buff,0,650);//��ս������ݻ�����
        }else
        {
           memset(Rx_Buff,0,650);//��ս������ݻ�����
           Err_Print(err,message);//�����ӡ
                          
           flag.rx_flag = 0;
        }
      
      }
     }

    
    delay_ms(25);
  }

 }

//�������
void Run_Task(void *pdata)
{
  u8 sd = 0;//��ǰ�����
 
  OS_CPU_SR cpu_sr=0;
    while(1)
  {
    if(flag.current_state==2 && flag.rx_flag==2)
    {
      sd = 0;                                   //��ǰ����
      Led_Status(flag.current_state);           //����ģʽ����
      Output_Place(Volume.data[0]>>16);          //����ָ������ʱ��ѡ��
      Volume.pulse_num=Volume.data[0]&0x0000FFFF;//�ܶ���
      memset(Volume.send_pulse_num,0,20);//��ո����洢��
      
      while(sd+1 <= Volume.pulse_num)
      {
        OS_ENTER_CRITICAL();
        flag.send_finish_flag=0;        
        OS_EXIT_CRITICAL();
        
        switch(Volume.data[3+sd*5]>>16)//�ж�ģʽ
        {
        case 1:{
                    Pluse_Number(sd);
                    Frequency_Select(&PWM_CK_CNT,&PWM_PRESCALER,Volume.PWM_TIMx,Volume.data[1+sd*5],Volume.data[0]>>16);//����Ƶ���趨�Ĵ���ֵ
                    PWM_Output(Volume.data[1+sd*5],Volume.PWM_TIMx);//�������
                    
                    while(flag.send_finish_flag == 0 && flag.current_state==2);                                          //ѭ���ȴ���һ�����巢�����
                   
                    OS_ENTER_CRITICAL();
                    Volume.send_pulse_num[sd]=Volume.pulse_suspend*65535+Volume.pulse_remainder;
                    OS_EXIT_CRITICAL();
        };break;
        case 2:{
                    Pluse_Number(sd);
                    Frequency_Select(&PWM_CK_CNT,&PWM_PRESCALER,Volume.PWM_TIMx,Volume.data[1+sd*5],Volume.data[0]>>16);//����Ƶ���趨�Ĵ���ֵ
                    PWM_Output(Volume.data[1+sd*5],Volume.PWM_TIMx);//�������
                    while(flag.send_finish_flag == 0 && flag.current_state==2);                                        //ѭ���ȴ���һ�����巢�����                                 
                    Delay_ms(Volume.data[1+sd*5+3]);
                    Volume.send_pulse_num[sd]=Volume.pulse_suspend*65535+Volume.pulse_remainder;
                    
        };break;
        case 3:{
                    Pluse_Number(sd);
                    Frequency_Select(&PWM_CK_CNT,&PWM_PRESCALER,Volume.PWM_TIMx,Volume.data[1+sd*5],Volume.data[0]>>16);//����Ƶ���趨�Ĵ���ֵ
                    PWM_Output(Volume.data[1+sd*5],Volume.PWM_TIMx);//�������
                    flag.ext_signal=0;
                    
                    while(flag.send_finish_flag == 0 && flag.current_state==2);//ѭ���ȴ���һ�����巢�����
                    Volume.send_pulse_num[sd]=Volume.pulse_suspend*65535+Volume.pulse_remainder;
                    while(flag.ext_signal!=((Volume.data[1+sd*5+2]  &  0x0000FFFF)+1));//�ȴ��ź�
                    
                    
                    OS_ENTER_CRITICAL();  //�����ٽ���(�ر��ж�)
                    flag.ext_signal=0;
                    OS_EXIT_CRITICAL();  //�˳��ٽ���(���ж�)
        };break;
        case 4:{
                    Pluse_Number(sd);
                    Frequency_Select(&PWM_CK_CNT,&PWM_PRESCALER,Volume.PWM_TIMx,Volume.data[1+sd*5],Volume.data[0]>>16);//����Ƶ���趨�Ĵ���ֵ
                    PWM_Output(Volume.data[1+sd*5],Volume.PWM_TIMx);                                                   //�������
                 
                    Delay_ms(Volume.data[1+sd*5+3]);
                    if(Volume.CNT_TIMx->CNT == 0)
                      Volume.send_pulse_num[sd]=Volume.pulse_suspend*65535+Volume.pulse_remainder;
                    else
                      Volume.send_pulse_num[sd]=Volume.pulse_suspend*65535+Volume.CNT_TIMx->CNT;
                    TIM_Cmd(Volume.PWM_TIMx, DISABLE);                  
                    TIM_Cmd(Volume.CNT_TIMx, DISABLE);
        
        };break;
        
        case 5:{
          
                    Pluse_Number(sd);
                    Frequency_Select(&PWM_CK_CNT,&PWM_PRESCALER,Volume.PWM_TIMx,Volume.data[1+sd*5],Volume.data[0]>>16);//����Ƶ���趨�Ĵ���ֵ
                    PWM_Output(Volume.data[1+sd*5],Volume.PWM_TIMx);                                                    //�������
                    OS_ENTER_CRITICAL(); 
                    flag.ext_signal=0;                   
                    OS_EXIT_CRITICAL();
                    while(flag.ext_signal!=((Volume.data[1+sd*5+2] & 0x0000FFFF)+1));                                //�ȴ��ź�
                    
                    if(Volume.CNT_TIMx->CNT == 0)
                      Volume.send_pulse_num[sd]=Volume.pulse_suspend*65535+Volume.pulse_remainder;
                    else
                      Volume.send_pulse_num[sd]=Volume.pulse_suspend*65535+Volume.CNT_TIMx->CNT;
                    
                    OS_ENTER_CRITICAL(); 
                    flag.ext_signal=0;                   
                    OS_EXIT_CRITICAL();
                    TIM_Cmd(Volume.PWM_TIMx, DISABLE);                  
                    TIM_Cmd(Volume.CNT_TIMx, DISABLE);
                   
          };break;
          case 6:{
                    Pluse_Number(sd);
                    Frequency_Select(&PWM_CK_CNT,&PWM_PRESCALER,Volume.PWM_TIMx,Volume.data[1+sd*5],Volume.data[0]>>16);//����Ƶ���趨�Ĵ���ֵ
                    PWM_Output(Volume.data[1+sd*5],Volume.PWM_TIMx);                                                   //�������
                    
                    OS_ENTER_CRITICAL();  //�����ٽ���(�ر��ж�)
                    flag.ext_signal=0;
                    OS_EXIT_CRITICAL();  //�˳��ٽ���(���ж�)                   
                    //while(flag.send_finish_flag == 0);//ѭ���ȴ���һ�����巢�����
                    while(flag.ext_signal!=((Volume.data[1+sd*5+2] & 0x0000FFFF)+1))//�ȴ��ź�
                    {
                      if(flag.send_finish_flag != 0)
                        break;
                    } 
                    
                    if(Volume.CNT_TIMx->CNT == 0)
                      Volume.send_pulse_num[sd]=Volume.pulse_suspend*65535+Volume.pulse_remainder;
                    else
                      Volume.send_pulse_num[sd]=Volume.pulse_suspend*65535+Volume.CNT_TIMx->CNT;
                    
                    OS_ENTER_CRITICAL();  //�����ٽ���(�ر��ж�)
                    flag.ext_signal=0;
                    OS_EXIT_CRITICAL();  //�˳��ٽ���(���ж�)
                    TIM_Cmd(Volume.PWM_TIMx, DISABLE);                  
                    TIM_Cmd(Volume.CNT_TIMx, DISABLE);
                    
          };break;
        }
       
          
        if(Volume.data[1+sd*5+4] == 0)
          sd++;
        else
        {
          sd = Volume.data[1+sd*5+4]-1;
        }
        
        Pulse_Num_Print(sd);//�������������ӡ
        OS_ENTER_CRITICAL();
        flag.send_finish_flag=0;
        OS_EXIT_CRITICAL();
      }
      
      //��ձ�־λ
      sd = 0;
      flag.rx_flag=0;
      flag.current_state=0;
      Led_Status(flag.current_state);
    }
    
    delay_ms(20);
  }
}

//���ݴ�������
void DATA_Task(void *pdata)
{
  INT8U err_box;//���������
  u8 *msg;//������Ϣ���� 
  
/*********ָ���**********/
  while(1)
  {
    msg=(u8 *)OSMboxPend(DATA_MBOX,0,&err_box);//�ȴ���Ϣ��������
    if(*msg == 0)
    {
        switch(flag.current_state)//�жϵ�ǰ״̬
        {
            case 0:{                                              //����
                      if(0 == strcmp((char const *)Rx_Buff,"data"))//�鿴�ϴ�����
                      {
                        memset(Rx_Buff,0,650);//��ս������ݻ�����
                        Print_Data();           
                       }else if(0 == strcmp((char const *)Rx_Buff,"go_set"))//��������ģʽ
                        {
                          memset(Rx_Buff,0,650);//��ս������ݻ�����                       
                          flag.current_state=1;
                          Led_Status(flag.current_state);//ģʽ����      
             
                         }
                        else if(0 == strcmp((char const *)Rx_Buff,"go_start") /*&& 0==Volume.recv_times*/)//��������ģʽ
                        {
                          memset(Rx_Buff,0,650);//��ս������ݻ�����
                         //Volume.recv_times=1;//��һ�ν��յ���Ч����
                         flag.current_state=2;
                         flag.rx_flag=2;
                        }
                        else
                        {
                            memset(Rx_Buff,0,650);//��ս������ݻ�����
                            u8 err_display[]={"\r\n�������ݴ������������룡"};
                            Print_Mode_Switch(err_display);//��ӡ��Ϣ         
                        }
                    };break;
             
            case 1:{          //����
               
                      flag.rx_flag = 1;
                
                    };break;
             
            case 2:{//����
                        flag.rx_flag = 2;
                        if(0 == strcmp((char const *)Rx_Buff,"STOP"))//��ֹ���
                        {
                         
                         TIM_Cmd(Volume.PWM_TIMx, DISABLE);                  
                         TIM_Cmd(Volume.CNT_TIMx, DISABLE);
                         flag.rx_flag = 0;
                       
                         flag.current_state=0;
                         Volume.pulse_num=0;
                         
                         Led_Status(flag.current_state);//ģʽ���� 
                        }
                        memset(Rx_Buff,0,650);//��ս������ݻ�����
                    };break;
             
           }
    }

  }
  

}

/************************�жϷ�����************************************/

/*���ڿ����жϷ�����*/
void USART1_IRQHandler(void)
{
        OSIntEnter();
        
        volatile u16 temp;
        if(USART_GetITStatus(USART1,USART_IT_IDLE) != RESET)
        {
                DMA_Cmd(DMA2_Stream2, DISABLE); //�ر�DMA,��ֹ�������������
                
                temp = USART1->SR;
                temp = USART1->DR;//���IDLE��־λ
               
                
                
                u8 UART1_ReceiveSize =RECEIVE_BUF_SIZE - DMA_GetCurrDataCounter(DMA2_Stream2);
                
                if(UART1_ReceiveSize !=0)//�������ݳ��Ȳ�Ϊ0
                {
                      flag.rx_flag=0;  //����֡���ձ�־λ��Ϊ1
                      OSMboxPost(DATA_MBOX,&flag.rx_flag);
                                             
                }
                DMA_ClearFlag(DMA2_Stream2,DMA_FLAG_TCIF2 | DMA_FLAG_FEIF2 | DMA_FLAG_DMEIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_HTIF2);//���������ɱ�־
                DMA_SetCurrDataCounter(DMA2_Stream2, RECEIVE_BUF_SIZE);
                DMA_Cmd(DMA2_Stream2, ENABLE);     //��DMA
        }
        OSIntExit();
}

/*��ʱ���жϷ�����*/
void TIM8_BRK_TIM12_IRQHandler(void)
{
  OSIntEnter();
  
  if(TIM_GetITStatus(Volume.CNT_TIMx, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(Volume.CNT_TIMx, TIM_IT_Update);
    if(Volume.interruput_times > 0)//�Ƿ���65535����
    {
      
      Volume.interruput_times--;
      if(Volume.interruput_times > 0)
      {
        Pulse_Output_Number(65535,Volume.CNT_TIMx);//��������
        Volume.pulse_suspend++;
      }
           
      else
       Pulse_Output_Number(Volume.pulse_remainder,Volume.CNT_TIMx);
    }else{
      flag.send_finish_flag=1;
      TIM_Cmd(Volume.PWM_TIMx, DISABLE);                  
      TIM_Cmd(Volume.CNT_TIMx, DISABLE);
    }

  }
  OSIntExit();
}

void TIM1_BRK_TIM9_IRQHandler(void)
{
  OSIntEnter();
  
  if(TIM_GetITStatus(Volume.CNT_TIMx, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(Volume.CNT_TIMx, TIM_IT_Update);
    if(Volume.interruput_times > 0)//�Ƿ���65535����
    {
      Volume.interruput_times--;
      if(Volume.interruput_times > 0)
      {
        Pulse_Output_Number(65535,Volume.CNT_TIMx);//��������
        Volume.pulse_suspend++;
      }
       
      else
       Pulse_Output_Number(Volume.pulse_remainder,Volume.CNT_TIMx);
    }else{
      flag.send_finish_flag=1;
      TIM_Cmd(Volume.PWM_TIMx, DISABLE);                  
      TIM_Cmd(Volume.CNT_TIMx, DISABLE);
    }

  }
  
  OSIntExit();
}

/*�ⲿ�жϷ�����*/
void EXTI4_IRQHandler(void)
{
       
        OSIntEnter();
        
        flag.ext_signal=1;

        EXTI->PR=1<<4;  //���LINE4�ϵ��жϱ�־λ
        OSIntExit();
}

void EXTI9_5_IRQHandler(void)
{

        OSIntEnter(); 
        if(EXTI_GetITStatus(EXTI_Line5) != RESET)
        {
          flag.ext_signal=2;   
          EXTI->PR=1<<5;  //���LINE�ϵ��жϱ�־λ
        }else if(EXTI_GetITStatus(EXTI_Line6) != RESET)
        {
          flag.ext_signal=3;   
          EXTI->PR=1<<6;  //���LINE�ϵ��жϱ�־λ
        }else if(EXTI_GetITStatus(EXTI_Line7) != RESET)
        {
          flag.ext_signal=4;   
          EXTI->PR=1<<7;  //���LINE�ϵ��жϱ�־λ
        }
        
        OSIntExit();

}
