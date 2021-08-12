#include "main.h"


u16 Pulse_num=10;//������������65536��
char temp_pulse_num[10];//�������


//char USART_ReceiveString[50];									//����PC�˷��͹������ַ�										//������Ϣ��־λ
//int Receive_sum = 0; //�����±�
//u32  frequency= 10; //����Ƶ��
	
RCC_ClocksTypeDef get_rcc_clock;

int main(void)
{
        
        RCC_GetClocksFreq(&get_rcc_clock);//�鿴ϵͳʱ��
        
        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//ϵͳ�ж����ȼ�����
        
        SystemInit();          //ʱ�ӳ�ʼ��
	delay_init(168);     //��ʱ��ʼ��       
        LED_Init();          //LED��ʼ��
	OSInit();          //UCOS��ʼ��        
	uart_init(9600);	//���ڳ�ʼ��
        MYDMASEND_Config((u32)&(USART1->DR),(u32)SendBuff,SEND_BUF_SIZE);//DMA���ͳ�ʼ��
        MYDMAREC_Config((u32)&(USART1->DR),(u32)Rx_Buff,RECEIVE_BUF_SIZE);//DMA���ճ�ʼ��
        
        
   
      //  TIM_Initializes();//��ʱ����ʼ��
       
	OSTaskCreate(Start_Task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO); //������ʼ����
	OSStart(); //��ʼ����
}





//��ʼ����
void Start_Task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
	OSStatInit();  //����ͳ������
	
	OS_ENTER_CRITICAL();  //�����ٽ���(�ر��ж�)
	OSTaskCreate(Led_Task_A,(void*)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);//����LEDA����
        OSTaskCreate(Led_Task_B,(void*)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);//����LEDB����
         OSTaskCreate(Led_Task_C,(void*)0,(OS_STK*)&LED2_TASK_STK[LED2_STK_SIZE-1],LED2_TASK_PRIO);//����LEDC����
	OSTaskSuspend(START_TASK_PRIO);//����ʼ����
	OS_EXIT_CRITICAL();  //�˳��ٽ���(���ж�)
}

//����A---����״̬
void Led_Task_A(void *pdata)
{       
      Led_Status(current_state);//����ģʽ����	
      u8 start[]={"������ָ���л�״̬��go_set�л�������״̬��go_start�л�������״̬\r\n"};
      strcpy((char *)SendBuff,(const char*)start);
      recv_len = strlen((const char *)SendBuff); 
      MYDMA_Enable(DMA2_Stream7,recv_len); 
       while(1)
	 {
            if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
            { 
              DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
              break; 
             }
         }
  
  while(1)
  {
    if(rx_flag)                                 															//?����?��?��???��y?Y
    {
      rx_flag = 0;  //������ձ�־λ
      recv_len = strlen((const char *)SendBuff); //����
      MYDMA_Enable(DMA2_Stream7,recv_len); 
       while(1)
	 {
            if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
            { 
              DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
              break; 
             }
         }
          
                                                          
        
    if(0 == strcmp((char const *)SendBuff,"go_set") && 0==recv_times)//��������ģʽ
    {
      recv_times=1;//��һ�ν��յ���Ч����
      current_state=1;
      task_flag=1;
      //OSTaskResume(5);//�ָ�����B
    }
    else if(0 == strcmp((char const *)SendBuff,"go_start") && 0==recv_times)//��������ģʽ
    {
       recv_times=1;//��һ�ν��յ���Ч����
       current_state=2;
       task_flag=2;
      // OSTaskResume(6);//�ָ�����C
    }
    else if(1==current_state &&  0!=recv_times)
    {
      recv_times++;
      task_flag=1;
     // OSTaskResume(5);//�ָ�����B
    }
    else if(2==current_state &&  0!=recv_times)
    {
      recv_times++;
      task_flag=2;
     // OSTaskResume(6);//�ָ�����C
    }else//������ʾ
    {
      strcpy((char *)SendBuff,(const char*)err_display);
      recv_len = strlen((const char *)SendBuff); 
      
      MYDMA_Enable(DMA2_Stream7,recv_len); 
       while(1)
	 {
            if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
            { 
              DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
              break; 
            }
         }
    }
    
  }
    Led_Status(current_state);//����ģʽ����
    delay_ms(20);
  //  OSTaskSuspend(OS_PRIO_SELF);//������������

  } 
       
	
}

//LED����B---����״̬
void Led_Task_B(void *pdata)
{
  while(1)
  {
    if(current_state==1 && task_flag==1) 
    {
      Led_Status(current_state);//����ģʽ����   
      if(1==recv_times)
      {
       u8 output_clue[]={"\r\n��ָ������������:\r\n"};
       strcpy((char *)SendBuff,(const char*)output_clue);
       recv_len = strlen((const char *)SendBuff); 
       MYDMA_Enable(DMA2_Stream7,recv_len); 
       while(1)
	 {
            if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
            { 
              DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
              break; 
             }
         }   
       
      } 
       else if(2==recv_times)
      {
        if(Output_Check(SendBuff))     //���������Ч�Լ��
         {   
           output_port=SendBuff[1]-'0';//���ӱ���
           u8 output_clue[]={"\r\n�����������ܶ���:\r\n"};
           strcpy((char *)SendBuff,(const char*)output_clue);
           recv_len = strlen((const char *)SendBuff); 
           MYDMA_Enable(DMA2_Stream7,recv_len); 
           while(1)
	   {
            if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
            { 
              DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
              break; 
            }
           } 
        
         }else
         {
           recv_times--;
           
         }

      }else if(3==recv_times)  
      {
         if(Number_Sum_Check(SendBuff)) //�����������Ч�Լ��
         {
           data[0]=(output_port<<16)+pulse_num;//������
           pulse_offset[0]=1;
           pulse_num=My_Atoi((char *)SendBuff);//�������������
           u8 speed_clue[]={"\r\n�������������ʣ�1~200000��\r\n"};
           strcpy((char *)SendBuff,(const char*)speed_clue);
           recv_len = strlen((const char *)SendBuff); 
           MYDMA_Enable(DMA2_Stream7,recv_len); 
           while(1)
           {
            if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
            { 
              DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
              break; 
            }
           }
        
         }else
         {
           recv_times--;
         }
        
      }else if(4==recv_times)  
      {
         if(Speed_Check(SendBuff)) //����������Ч�Լ��
         {
           data[pulse_offset[sd]+0]=My_Atoi((char *)SendBuff);//�������ʱ���
           u8 speed_clue[]={"\r\n�������������(1 ~ 2147483647)\r\n"};
           strcpy((char *)SendBuff,(const char*)speed_clue);
           recv_len = strlen((const char *)SendBuff); 
           MYDMA_Enable(DMA2_Stream7,recv_len); 
           while(1)
           {
            if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
            { 
              DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
              break; 
            }
           }
        
         }else
         {
           recv_times--;
         }
        
      }else if(5==recv_times)  
      {
         if(Number_Check(SendBuff)) //���������Ч�Լ��
         {
           data[pulse_offset[sd]+1]=My_Atoi((char *)SendBuff);//�����������
           u8 mode_clue[]={"\r\n�����õȴ�ģʽ�����巢�����finish��waitʱ��ģʽwait_time��wait�ź�ģʽwait_signal��ACTʱ��ģʽact_time��EXT�ź�ģʽext_signal��EXT�ź�/���巢�����ģʽ��ext_finish��\r\n"};
           strcpy((char *)SendBuff,(const char*)mode_clue);
           recv_len = strlen((const char *)SendBuff); 
           MYDMA_Enable(DMA2_Stream7,recv_len); 
           while(1)
           {
            if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
            { 
              DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
              break; 
            }
           }
        
         }else
         {
           recv_times--;
         }
        
      }else if(6==recv_times)  
      {
         if(Mode_Check(SendBuff)) //����ģʽ��Ч�Լ��
         {
           if(0 == strcmp((char const *)SendBuff,"wait_time") || 0 == strcmp((char const *)SendBuff,"act_time"))
           {       
             if(0 == strcmp((char const *)SendBuff,"wait_time"))
               mode=0x0002;//waitʱ��ģʽ
             else
               mode=0x0004;//ACTʱ��ģʽ
              u8 mode_clue[]={"\r\n��������ʱ��ļĴ�����ţ�T1~T100��\r\n"};
              strcpy((char *)SendBuff,(const char*)mode_clue);
              recv_len = strlen((const char *)SendBuff); 
              MYDMA_Enable(DMA2_Stream7,recv_len); 
              while(1)
              {
                if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
                { 
                  DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
                  break; 
                }
              }
           } else if(0 == strcmp((char const *)SendBuff,"wait_signal") )
           {
              mode=0x0003;//wait�ź�ģʽ
              u8 mode_clue[]={"\r\n�������źżĴ�����ţ�S1~S100��\r\n"};
              strcpy((char *)SendBuff,(const char*)mode_clue);
              recv_len = strlen((const char *)SendBuff); 
              MYDMA_Enable(DMA2_Stream7,recv_len); 
              while(1)
              {
                if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
                { 
                  DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
                  break; 
                }
              }
           }else if(0 == strcmp((char const *)SendBuff,"ext_finish") || 0 == strcmp((char const *)SendBuff,"ext_signal"))
           {
             if(0 == strcmp((char const *)SendBuff,"ext_finish"))
               mode=0x0006;//EXT�ź�/���巢�����ģʽ
             else
               mode=0x0005;//EXT�ź�ģʽ
              u8 mode_clue[]={"\r\n�������ⲿ�źŴ�������(w0��w1��w2��w3��w4)\r\n"};
              strcpy((char *)SendBuff,(const char*)mode_clue);
              recv_len = strlen((const char *)SendBuff); 
              MYDMA_Enable(DMA2_Stream7,recv_len); 
              while(1)
              {
                if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
                { 
                  DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
                  break; 
                }
              }
           }else{
             mode=0x0001;
             data[pulse_offset[sd]+2]=0x00010000;//���巢�����ģʽ
             data[pulse_offset[sd]+3] = 0;
             recv_times+=2;//�����������ݽ���
              u8 mode_clue[]={"\r\n������Ҫ��ת���Ķ���ţ�0��Ĭ����һ�Σ������ҪС�ڵ����ܶ�����Ҫ������ѭ����\r\n"};
              strcpy((char *)SendBuff,(const char*)mode_clue);
              recv_len = strlen((const char *)SendBuff); 
              MYDMA_Enable(DMA2_Stream7,recv_len); 
              while(1)
              {
                if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
                { 
                  DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
                  break; 
                }
              }
           }
        
         }else
         {
           recv_times--;
         } 
        
      }else if(7==recv_times) //�Ĵ�������
       {
         if(mode == 0x0002 || mode == 0x0004)//ʱ��Ĵ���
         {
           if(Time_Register_Check(SendBuff))
           {
             SendBuff[0]=SendBuff[1];
             SendBuff[1]=SendBuff[2];
             SendBuff[2]=SendBuff[3];
             SendBuff[3]=SendBuff[4];
             time=(u8)My_Atoi((char *)SendBuff);//ʱ��Ĵ�����Ŵ洢
             if(mode == 0x0002)//���ݱ���
              data[pulse_offset[sd]+2] = 0x00020000+time;
             else
              data[pulse_offset[sd]+2] = 0x00040000+time;
             u8 speed_clue[]={"\r\n�������趨�ĵȴ�ʱ�䣨1 ~ 65535ms��\r\n"};
             strcpy((char *)SendBuff,(const char*)speed_clue);
             recv_len = strlen((const char *)SendBuff); 
             MYDMA_Enable(DMA2_Stream7,recv_len); 
             while(1)
             {
              if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
              { 
                DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
                break; 
              }
             }
           }else
            {
             recv_times--;
            }
           
         }else  if(mode == 0x0003)//�źżĴ���
         {
           if(Signal_Register_Check(SendBuff))
           {
             SendBuff[0]=SendBuff[1];
             SendBuff[1]=SendBuff[2];
             SendBuff[2]=SendBuff[3];
             SendBuff[3]=SendBuff[4];
             signal=(u8)My_Atoi((char *)SendBuff);//�źżĴ�����Ŵ洢
             data[pulse_offset[sd]+2] = 0x00030000+signal;//���ݱ���
             data[pulse_offset[sd]+3] = 0;
             
             u8 speed_clue[]={"\r\n������Ҫ��ת���Ķ���ţ�0��Ĭ����һ�Σ������ҪС�ڵ����ܶ�����Ҫ������ѭ����\r\n"};
             strcpy((char *)SendBuff,(const char*)speed_clue);
             recv_len = strlen((const char *)SendBuff); 
             MYDMA_Enable(DMA2_Stream7,recv_len); 
             while(1)
             {
              if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
              { 
                DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
                break; 
              }
             }
             recv_times++;
           }else
            {
             recv_times--;
            }
           
         }else  if(mode == 0x0005 || mode == 0x0006)//�ⲿ�źŶ���
         {
          if(External_Signal_Check(SendBuff))
          {
            SendBuff[0]=SendBuff[1];
            SendBuff[1]=SendBuff[2];
            ext_signal=(u8)My_Atoi((char *)SendBuff);//�ⲿ�źŶ�����Ŵ洢
            if(mode == 0x0005)//���ݱ���
              data[pulse_offset[sd]+2] = 0x00050000+ext_signal;
            else
              data[pulse_offset[sd]+2] = 0x00060000+ext_signal;
            data[pulse_offset[sd]+3] = 0;
            
            u8 speed_clue[]={"\r\n������Ҫ��ת���Ķ���ţ�0��Ĭ����һ�Σ������ҪС�ڵ����ܶ�����Ҫ������ѭ����\r\n"};
             strcpy((char *)SendBuff,(const char*)speed_clue);
             recv_len = strlen((const char *)SendBuff); 
             MYDMA_Enable(DMA2_Stream7,recv_len); 
             while(1)
             {
              if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
              { 
                DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
                break; 
              }
             }
             recv_times++;
          }else
          {
            recv_times--;
          }
         }
                    
      }else if(8==recv_times) //�ȴ�ʱ�� 
       {
         if(Time_Check(SendBuff))
         {
             
             data[pulse_offset[sd]+3] = My_Atoi((char*)SendBuff);//�ȴ�ʱ�䱣��
             u8 speed_clue[]={"\r\n������Ҫ��ת���Ķ���ţ�0��Ĭ����һ�Σ������ҪС�ڵ����ܶ�����Ҫ������ѭ����\r\n"};
             strcpy((char *)SendBuff,(const char*)speed_clue);
             recv_len = strlen((const char *)SendBuff); 
             MYDMA_Enable(DMA2_Stream7,recv_len); 
             while(1)
             {
              if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
              { 
                DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
                break; 
              }
             }
         }else
         {
           recv_times--;
         }
      }else if(9==recv_times) //�������ת
       {
         if(Section_Num_Check(SendBuff,pulse_num))
         {
           data[pulse_offset[sd]+4]=My_Atoi((char *)SendBuff);
           sd++;
           
           if(sd == pulse_num)//����������������
           {          
             u8 speed_clue[]={"\r\n�����������\r\n������set_end��������\r\n"};
             strcpy((char *)SendBuff,(const char*)speed_clue);
             recv_len = strlen((const char *)SendBuff); 
             MYDMA_Enable(DMA2_Stream7,recv_len); 
             while(1)
            {
             if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
             { 
               DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
               break; 
             }
            }
        
          }else
           {
                  recv_times=3;    
                  pulse_offset[sd]=pulse_offset[sd-1]+5;
                  
                  u8 speed_clue[]={"\r\n�������������ʣ�1~200000��\r\n"};
                  strcpy((char *)SendBuff,(const char*)speed_clue);
                  recv_len = strlen((const char *)SendBuff); 
                  MYDMA_Enable(DMA2_Stream7,recv_len); 
                  while(1)
                  {
                    if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
                    { 
                      DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
                      break; 
                    }
                  }
           
           }        
        
         }else
         {
           recv_times--;
         }
      }else if(10==recv_times) //���ý���
       {
         if(End_Check(SendBuff))
         {
             recv_times=0;
             current_state=0;//����״̬
             Led_Status(current_state);
             u8 speed_clue[]={"\r\n������ָ���л�״̬��go_set�л�������״̬��go_start�л�������״̬\r\n"};
             strcpy((char *)SendBuff,(const char*)speed_clue);
             recv_len = strlen((const char *)SendBuff); 
             MYDMA_Enable(DMA2_Stream7,recv_len); 
             while(1)
            {
             if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
             { 
               DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
               break; 
             }
            }
    
         }else
         {
           recv_times--;
         }
      }
    }
   // OSTaskSuspend(OS_PRIO_SELF);//������������
    task_flag=0;
    delay_ms(20);
  }

 }

//LED����C---����״̬
void Led_Task_C(void *pdata)
{
    
    while(1)
  {
    if(current_state==2 && task_flag==2)
    {
      
      
      
     
      Led_Status(current_state);//����ģʽ����
      
    }
    //OSTaskSuspend(OS_PRIO_SELF);//������������
    task_flag=0;
    delay_ms(20);
  }
}


void USART1_IRQHandler(void)//���ڿ����ж�
{
        u16 temp;
        if(USART_GetITStatus(USART1,USART_IT_IDLE) != RESET)
        {
                DMA_Cmd(DMA2_Stream2, DISABLE); //�ر�DMA,��ֹ�������������

                temp = USART1->SR;
                temp = USART1->DR;//���IDLE��־λ
               
                //USART_ClearFlag(USART1,USART_IT_IDLE);
                
                UART1_ReceiveSize =RECEIVE_BUF_SIZE - DMA_GetCurrDataCounter(DMA2_Stream2);
                
                if(UART1_ReceiveSize !=0)//�������ݳ��Ȳ�Ϊ0
                {
                        rx_flag=1;  //����֡���ձ�־λ��Ϊ1
                        
                      memset(SendBuff,0,SEND_BUF_SIZE);//��ս��ջ�����
                      for(temp=0;temp<UART1_ReceiveSize;temp++)
                      {
                      SendBuff[temp]= Rx_Buff[temp]; 
                      }
                      memset(Rx_Buff,0,UART1_ReceiveSize);//��ս��ջ�����
                      //OSTaskResume(4);
                }
                DMA_ClearFlag(DMA2_Stream2,DMA_FLAG_TCIF2 | DMA_FLAG_FEIF2 | DMA_FLAG_DMEIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_HTIF2);//���������ɱ�־
                DMA_SetCurrDataCounter(DMA2_Stream2, RECEIVE_BUF_SIZE);
                DMA_Cmd(DMA2_Stream2, ENABLE);     //��DMA
        }
}




/*
void CNT_TIM_IRQHandler(void)
{
  if(TIM_GetITStatus(CNT_TIMx, TIM_IT_Update) != RESET)
  {
   // temp_pulse_num[0]=pulse_num;//������
    My_Itoa(pulse_num,temp_pulse_num);
    TIM_ClearITPendingBit(CNT_TIMx, TIM_IT_Update);
    TIM_Cmd(PWM_TIMx, DISABLE);                  
    TIM_Cmd(CNT_TIMx, DISABLE);  
    USART_RX_STA|=0x8000;
    current_state=0;//�ص�����״̬
    u16 times=0;
    u16 pulse_len=strlen(temp_pulse_num);
       for(times=0;times<pulse_len;times++)
	{
          USART_SendData(USART1, temp_pulse_num[times]); //����������        
          while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	}
    memset(temp_pulse_num,0,10);
  }
}
*/
