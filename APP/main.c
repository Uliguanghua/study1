#include "main.h"


//u16 Pulse_num=10;//������������65536��
//char temp_pulse_num[10];//�������


//char USART_ReceiveString[50];									//����PC�˷��͹������ַ�										//������Ϣ��־λ
//int Receive_sum = 0; //�����±�
//
	
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
        data_init();//������ʼ��
        Delay_TIM2_Configuration();//������ʱ����ʼ��
        MYDMASEND_Config((u32)&(USART1->DR),(u32)SendBuff,SEND_BUF_SIZE);//DMA���ͳ�ʼ��
        MYDMAREC_Config((u32)&(USART1->DR),(u32)Rx_Buff,RECEIVE_BUF_SIZE);//DMA���ճ�ʼ��
       
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
      Led_Status(flag.current_state);//����ģʽ����	
      u8 start[]={"������ָ���л�״̬��go_set�л�������״̬��go_start�л�������״̬\r\n"};
      strcpy((char *)SendBuff,(const char*)start);
      Volume.recv_len = strlen((const char *)SendBuff); 
      MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
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
    if(flag.rx_flag)                                 															//?����?��?��???��y?Y
    {
      flag.rx_flag = 0;  //������ձ�־λ
      Volume.recv_len = strlen((const char *)SendBuff); //����
      MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
       while(1)
	 {
            if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
            { 
              DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
              break; 
             }
         }
     if(0 == strcmp((char const *)SendBuff,"exit"))//�˳�����ģʽ   
     {
        Volume.recv_times=0;
        flag.current_state=0;
        flag.task_flag=0;
        Led_Status(flag.current_state);
        Led_Status(flag.current_state);//����ģʽ����	
        u8 start[]={"������ָ���л�״̬��go_set�л�������״̬��go_start�л�������״̬\r\n"};
        strcpy((char *)SendBuff,(const char*)start);
        Volume.recv_len = strlen((const char *)SendBuff); 
        MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
        while(1)
	 {
            if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
            { 
              DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
              break; 
             }
         }
     }
                                                          
     else if(0 == strcmp((char const *)SendBuff,"go_set") && 0==Volume.recv_times)//��������ģʽ
    {
      Volume.recv_times=1;//��һ�ν��յ���Ч����
      flag.current_state=1;
      flag.task_flag=1;
      memset(Volume.data,0,600);//��������״̬ʱ������ݴ洢����
      //OSTaskResume(5);//�ָ�����B
    }
    else if(0 == strcmp((char const *)SendBuff,"go_start") && 0==Volume.recv_times)//��������ģʽ
    {
       Volume.recv_times=1;//��һ�ν��յ���Ч����
       flag.current_state=2;
       flag.task_flag=2;
      // OSTaskResume(6);//�ָ�����C
    }
    else if(1==flag.current_state &&  0!=Volume.recv_times)
    {
      Volume.recv_times++;
      flag.task_flag=1;
     // OSTaskResume(5);//�ָ�����B
    }
    else if(2==flag.current_state &&  0!=Volume.recv_times)
    {
      Volume.recv_times++;
      flag.task_flag=2;
     // OSTaskResume(6);//�ָ�����C
    }else//������ʾ
    {
      strcpy((char *)SendBuff,(const char*)err_display);
      Volume.recv_len = strlen((const char *)SendBuff); 
      
      MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
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
    Led_Status(flag.current_state);//����ģʽ����
    delay_ms(20);
  //  OSTaskSuspend(OS_PRIO_SELF);//������������

  } 
       
	
}

//LED����B---����״̬
void Led_Task_B(void *pdata)
{
  while(1)
  {
    if(flag.current_state==1 && flag.task_flag==1) 
    {
      Led_Status(flag.current_state);//����ģʽ����   
      if(1==Volume.recv_times)
      {
       u8 output_clue[]={"\r\n��ָ������������:\r\n"};
       strcpy((char *)SendBuff,(const char*)output_clue);
       Volume.recv_len = strlen((const char *)SendBuff); 
       MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
       while(1)
	 {
            if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
            { 
              DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
              break; 
             }
         }   
       
      } 
       else if(2==Volume.recv_times)
      {
        if(Output_Check(SendBuff))     //���������Ч�Լ��
         {   
           Volume.output_port=SendBuff[1]-'0';//���ӱ���
           switch(Volume.output_port)//���ݶ������ö�ʱ��
           {
             case 0:{Volume.CNT_TIMx = TIM9;
                     Volume.PWM_TIMx = TIM10;
             };break;
             case 1:{Volume.CNT_TIMx = TIM12;
                    Volume.PWM_TIMx = TIM13;
             };break;
             case 2:{Volume.CNT_TIMx = TIM9;
                    Volume.PWM_TIMx = TIM11;
             };break;
             case 3:{Volume.CNT_TIMx = TIM12;
                    Volume.PWM_TIMx = TIM14;
             };break;  
           }
           u8 output_clue[]={"\r\n�����������ܶ���:\r\n"};
           strcpy((char *)SendBuff,(const char*)output_clue);
           Volume.recv_len = strlen((const char *)SendBuff); 
           MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
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
           Volume.recv_times--;
           
         }

      }else if(3==Volume.recv_times)  
      {
         if(Number_Sum_Check(SendBuff)) //�����������Ч�Լ��
         {
           
           Volume.pulse_offset[0]=1;
           Volume.pulse_num=My_Atoi((char *)SendBuff);//�������������
           Volume.data[0]=(Volume.output_port<<16)+Volume.pulse_num;//������
           
           u8 speed_clue[]={"\r\n�������������ʣ�1~200000��\r\n"};
           strcpy((char *)SendBuff,(const char*)speed_clue);
           Volume.recv_len = strlen((const char *)SendBuff); 
           MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
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
           Volume.recv_times--;
         }
        
      }else if(4==Volume.recv_times)  
      {
         if(Speed_Check(SendBuff)) //����������Ч�Լ��
         {
           Volume.data[Volume.pulse_offset[Volume.sd]+0]=My_Atoi((char *)SendBuff);//�������ʱ���
           u8 speed_clue[]={"\r\n�������������(1 ~ 2147483647)\r\n"};
           strcpy((char *)SendBuff,(const char*)speed_clue);
           Volume.recv_len = strlen((const char *)SendBuff); 
           MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
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
           Volume.recv_times--;
         }
        
      }else if(5==Volume.recv_times)  
      {
         if(Number_Check(SendBuff)) //���������Ч�Լ��
         {
           Volume.data[Volume.pulse_offset[Volume.sd]+1]=My_Atoi((char *)SendBuff);//�����������
           u8 mode_clue[]={"\r\n�����õȴ�ģʽ�����巢�����finish��waitʱ��ģʽwait_time��wait�ź�ģʽwait_signal��ACTʱ��ģʽact_time��EXT�ź�ģʽext_signal��EXT�ź�/���巢�����ģʽ��ext_finish��\r\n"};
           strcpy((char *)SendBuff,(const char*)mode_clue);
           Volume.recv_len = strlen((const char *)SendBuff); 
           MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
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
           Volume.recv_times--;
         }
        
      }else if(6==Volume.recv_times)  
      {
         if(Mode_Check(SendBuff)) //����ģʽ��Ч�Լ��
         {
           if(0 == strcmp((char const *)SendBuff,"wait_time") || 0 == strcmp((char const *)SendBuff,"act_time"))
           {       
             if(0 == strcmp((char const *)SendBuff,"wait_time"))
               Volume.mode=0x0002;//waitʱ��ģʽ
             else
               Volume.mode=0x0004;//ACTʱ��ģʽ
              u8 mode_clue[]={"\r\n��������ʱ��ļĴ�����ţ�T1~T100��\r\n"};
              strcpy((char *)SendBuff,(const char*)mode_clue);
              Volume.recv_len = strlen((const char *)SendBuff); 
              MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
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
              Volume.mode=0x0003;//wait�ź�ģʽ
              u8 mode_clue[]={"\r\n�������źżĴ�����ţ�S1~S100��\r\n"};
              strcpy((char *)SendBuff,(const char*)mode_clue);
              Volume.recv_len = strlen((const char *)SendBuff); 
              MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
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
               Volume.mode=0x0006;//EXT�ź�/���巢�����ģʽ
             else
               Volume.mode=0x0005;//EXT�ź�ģʽ
              u8 mode_clue[]={"\r\n�������ⲿ�źŴ�������(w0��w1��w2��w3��w4)\r\n"};
              strcpy((char *)SendBuff,(const char*)mode_clue);
              Volume.recv_len = strlen((const char *)SendBuff); 
              MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
              while(1)
              {
                if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
                { 
                  DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
                  break; 
                }
              }
           }else{
             Volume.mode=0x0001;
             Volume.data[Volume.pulse_offset[Volume.sd]+2]=0x00010000;//���巢�����ģʽ
             Volume.data[Volume.pulse_offset[Volume.sd]+3] = 0;
             Volume.recv_times+=2;//�����������ݽ���
              u8 mode_clue[]={"\r\n������Ҫ��ת���Ķ���ţ�0��Ĭ����һ�Σ������ҪС�ڵ����ܶ�����Ҫ������ѭ����\r\n"};
              strcpy((char *)SendBuff,(const char*)mode_clue);
              Volume.recv_len = strlen((const char *)SendBuff); 
              MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
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
           Volume.recv_times--;
         } 
        
      }else if(7==Volume.recv_times) //�Ĵ�������
       {
         if(Volume.mode == 0x0002 || Volume.mode == 0x0004)//ʱ��Ĵ���
         {
           if(Time_Register_Check(SendBuff))
           {
             simulation.time=(u8)My_Atoi((char *)SendBuff);//ʱ��Ĵ�����Ŵ洢
             if(Volume.mode == 0x0002)//���ݱ���
              Volume.data[Volume.pulse_offset[Volume.sd]+2] = 0x00020000+simulation.time;
             else
              Volume.data[Volume.pulse_offset[Volume.sd]+2] = 0x00040000+simulation.time;
             u8 speed_clue[]={"\r\n�������趨�ĵȴ�ʱ�䣨1 ~ 65535ms��\r\n"};
             strcpy((char *)SendBuff,(const char*)speed_clue);
             Volume.recv_len = strlen((const char *)SendBuff); 
             MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
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
             Volume.recv_times--;
            }
           
         }else  if(Volume.mode == 0x0003)//�źżĴ���
         {
           if(Signal_Register_Check(SendBuff))
           {
             simulation.signal=(u8)My_Atoi((char *)SendBuff);//�źżĴ�����Ŵ洢
             Volume.data[Volume.pulse_offset[Volume.sd]+2] = 0x00030000+simulation.signal;//���ݱ���
             Volume.data[Volume.pulse_offset[Volume.sd]+3] = 0;
             
             u8 speed_clue[]={"\r\n������Ҫ��ת���Ķ���ţ�0��Ĭ����һ�Σ������ҪС�ڵ����ܶ�����Ҫ������ѭ����\r\n"};
             strcpy((char *)SendBuff,(const char*)speed_clue);
             Volume.recv_len = strlen((const char *)SendBuff); 
             MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
             while(1)
             {
              if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
              { 
                DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
                break; 
              }
             }
             Volume.recv_times++;
           }else
            {
             Volume.recv_times--;
            }
           
         }else  if(Volume.mode == 0x0005 || Volume.mode == 0x0006)//�ⲿ�źŶ���
         {
          if(External_Signal_Check(SendBuff))
          {
            simulation.ext_signal=(u8)My_Atoi((char *)SendBuff);//�ⲿ�źŶ�����Ŵ洢
            if(Volume.mode == 0x0005)//���ݱ���
              Volume.data[Volume.pulse_offset[Volume.sd]+2] = 0x00050000+simulation.ext_signal;
            else
              Volume.data[Volume.pulse_offset[Volume.sd]+2] = 0x00060000+simulation.ext_signal;
            Volume.data[Volume.pulse_offset[Volume.sd]+3] = 0;
            
            u8 speed_clue[]={"\r\n������Ҫ��ת���Ķ���ţ�0��Ĭ����һ�Σ������ҪС�ڵ����ܶ�����Ҫ������ѭ����\r\n"};
             strcpy((char *)SendBuff,(const char*)speed_clue);
             Volume.recv_len = strlen((const char *)SendBuff); 
             MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
             while(1)
             {
              if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
              { 
                DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
                break; 
              }
             }
             Volume.recv_times++;
          }else
          {
            Volume.recv_times--;
          }
         }
                    
      }else if(8==Volume.recv_times) //�ȴ�ʱ�� 
       {
         if(Time_Check(SendBuff))
         {
             
             Volume.data[Volume.pulse_offset[Volume.sd]+3] = My_Atoi((char*)SendBuff);//�ȴ�ʱ�䱣��
             u8 speed_clue[]={"\r\n������Ҫ��ת���Ķ���ţ�0��Ĭ����һ�Σ������ҪС�ڵ����ܶ�����Ҫ������ѭ����\r\n"};
             strcpy((char *)SendBuff,(const char*)speed_clue);
             Volume.recv_len = strlen((const char *)SendBuff); 
             MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
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
           Volume.recv_times--;
         }
      }else if(9==Volume.recv_times) //�������ת
       {
         if(Section_Num_Check(SendBuff,Volume.pulse_num))
         {
           Volume.data[Volume.pulse_offset[Volume.sd]+4]=My_Atoi((char *)SendBuff);
           Volume.sd++;
           
           if(Volume.sd == Volume.pulse_num)//����������������
           {          
             u8 speed_clue[]={"\r\n�����������\r\n������set_end��������\r\n"};
             strcpy((char *)SendBuff,(const char*)speed_clue);
             Volume.recv_len = strlen((const char *)SendBuff); 
             MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
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
                  Volume.recv_times=3;    
                  Volume.pulse_offset[Volume.sd]=Volume.pulse_offset[Volume.sd-1]+5;
                  
                  u8 speed_clue[]={"\r\n�������������ʣ�1~200000��\r\n"};
                  strcpy((char *)SendBuff,(const char*)speed_clue);
                  Volume.recv_len = strlen((const char *)SendBuff); 
                  MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
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
           Volume.recv_times--;
         }
      }else if(10==Volume.recv_times) //���ý���
       {
         if(End_Check(SendBuff))
         {
             Volume.recv_times=0;
             Volume.sd=0;
             flag.current_state=0;//����״̬
             Led_Status(flag.current_state);
             u8 speed_clue[]={"\r\n������ָ���л�״̬��go_set�л�������״̬��go_start�л�������״̬\r\n"};
             strcpy((char *)SendBuff,(const char*)speed_clue);
             Volume.recv_len = strlen((const char *)SendBuff); 
             MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
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
           Volume.recv_times--;
         }
      }
      flag.task_flag=0;
    }
   // OSTaskSuspend(OS_PRIO_SELF);//������������
    
    delay_ms(20);
  }

 }

//LED����C---����״̬
void Led_Task_C(void *pdata)
{
    
    while(1)
  {
    if(flag.current_state==2 && flag.task_flag==2)
    {
      Led_Status(flag.current_state);//����ģʽ����
      Output_Place(Volume.data[0]>>16);//����ָ��
      Volume.pulse_num=Volume.data[0]&0x0000FFFF;//�ܶ���
   
      while(Volume.sd+1 <= Volume.pulse_num)
      {
        
        switch(Volume.data[Volume.pulse_offset[Volume.sd]+2]>>16)//�ж�ģʽ
        {
        case 1:{
                  if(Volume.data[Volume.pulse_offset[Volume.sd]+1] > 65535)//�ж��������
                  {
                    Volume.interruput_times = Volume.data[Volume.pulse_offset[Volume.sd]+1] / 65535;
                    Volume.pulse_remainder = Volume.data[Volume.pulse_offset[Volume.sd]+1] % 65535;
                  }else
                  {
                    Volume.pulse_remainder = Volume.data[Volume.pulse_offset[Volume.sd]+1];
                  }
                
                
                  if(Volume.interruput_times > 0)
                  {
                    Pulse_Output_Number(65535,Volume.CNT_TIMx);
                  }     
                  else
                  {
                    Pulse_Output_Number(Volume.pulse_remainder,Volume.CNT_TIMx);
                  }
                    Frequency_Select(&PWM_CK_CNT,&PWM_PRESCALER,Volume.PWM_TIMx,Volume.data[Volume.pulse_offset[Volume.sd]],Volume.data[0]>>16);//����Ƶ���趨�Ĵ���ֵ
                    PWM_Output(Volume.data[Volume.pulse_offset[Volume.sd]],Volume.PWM_TIMx);//�������
                    while(flag.send_finish_flag == 0);//ѭ���ȴ���һ�����巢�����
                 
        };break;
        case 2:{
                  if(Volume.data[Volume.pulse_offset[Volume.sd]+1] > 65535)//�ж��������
                  {
                    Volume.interruput_times = Volume.data[Volume.pulse_offset[Volume.sd]+1] / 65535;
                    Volume.pulse_remainder = Volume.data[Volume.pulse_offset[Volume.sd]+1] % 65535;
                  }else
                  {
                    Volume.pulse_remainder = Volume.data[Volume.pulse_offset[Volume.sd]+1];
                  }
                
                
                  if(Volume.interruput_times > 0)
                  {
                    Pulse_Output_Number(65535,Volume.CNT_TIMx);
                  }     
                  else
                  {
                    Pulse_Output_Number(Volume.pulse_remainder,Volume.CNT_TIMx);
                  }
                    Frequency_Select(&PWM_CK_CNT,&PWM_PRESCALER,Volume.PWM_TIMx,Volume.data[Volume.pulse_offset[Volume.sd]],Volume.data[0]>>16);//����Ƶ���趨�Ĵ���ֵ
                    PWM_Output(Volume.data[Volume.pulse_offset[Volume.sd]],Volume.PWM_TIMx);//�������
                    while(flag.send_finish_flag == 0);//ѭ���ȴ���һ�����巢�����
                    
                    u32 temp_register=Volume.data[Volume.pulse_offset[Volume.sd]+2]&0x0000FFFF;
                    simulation.Time_Register[temp_register-1]=Volume.data[Volume.pulse_offset[Volume.sd]+3];
                    Delay_ms(simulation.Time_Register[temp_register-1]);
        };break;
        case 3:{
          
        };break;
        case 4:{};break;
        case 5:{};break;
        case 6:{};break;
        }
       
          
        if(Volume.data[Volume.pulse_offset[Volume.sd]+4] == 0)
          Volume.sd++;
        else
        {
          Volume.sd = Volume.data[Volume.pulse_offset[Volume.sd]+4]-1;
        }
        
        flag.send_finish_flag=0;
      }
      
      Volume.recv_times=0;
      flag.task_flag=0;
      flag.current_state=0;
    }
    //OSTaskSuspend(OS_PRIO_SELF);//������������
    
    
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
                
                Volume.UART1_ReceiveSize =RECEIVE_BUF_SIZE - DMA_GetCurrDataCounter(DMA2_Stream2);
                
                if(Volume.UART1_ReceiveSize !=0)//�������ݳ��Ȳ�Ϊ0
                {
                        flag.rx_flag=1;  //����֡���ձ�־λ��Ϊ1
                        
                      memset(SendBuff,0,SEND_BUF_SIZE);//��ս��ջ�����
                      for(temp=0;temp<Volume.UART1_ReceiveSize;temp++)
                      {
                      SendBuff[temp]= Rx_Buff[temp]; 
                      }
                      memset(Rx_Buff,0,Volume.UART1_ReceiveSize);//��ս��ջ�����
                      //OSTaskResume(4);
                }
                DMA_ClearFlag(DMA2_Stream2,DMA_FLAG_TCIF2 | DMA_FLAG_FEIF2 | DMA_FLAG_DMEIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_HTIF2);//���������ɱ�־
                DMA_SetCurrDataCounter(DMA2_Stream2, RECEIVE_BUF_SIZE);
                DMA_Cmd(DMA2_Stream2, ENABLE);     //��DMA
        }
}


void TIM8_BRK_TIM12_IRQHandler(void)//������12����¼�
{
 
  if(TIM_GetITStatus(Volume.CNT_TIMx, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(Volume.CNT_TIMx, TIM_IT_Update);
    if(Volume.interruput_times > 0)//�Ƿ���65535����
    {
      Volume.interruput_times--;
      if(Volume.interruput_times > 0)
       Pulse_Output_Number(65535,Volume.CNT_TIMx);//��������
      else
       Pulse_Output_Number(Volume.pulse_remainder,Volume.CNT_TIMx);
    }else{
      flag.send_finish_flag=1;
      TIM_Cmd(Volume.PWM_TIMx, DISABLE);                  
      TIM_Cmd(Volume.CNT_TIMx, DISABLE);
    }

  }
}

void TIM1_BRK_TIM9_IRQHandler(void)//������9����¼�
{
 
  if(TIM_GetITStatus(Volume.CNT_TIMx, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(Volume.CNT_TIMx, TIM_IT_Update);
    if(Volume.interruput_times > 0)//�Ƿ���65535����
    {
      Volume.interruput_times--;
      if(Volume.interruput_times > 0)
       Pulse_Output_Number(65535,Volume.CNT_TIMx);//��������
      else
       Pulse_Output_Number(Volume.pulse_remainder,Volume.CNT_TIMx);
    }else{
      flag.send_finish_flag=1;
      TIM_Cmd(Volume.PWM_TIMx, DISABLE);                  
      TIM_Cmd(Volume.CNT_TIMx, DISABLE);
    }

  }
}
