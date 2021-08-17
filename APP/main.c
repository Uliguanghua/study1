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
        
        DATA_MBOX=OSMboxCreate((void*)0);//������Ϣ����
       
	OSTaskCreate(Start_Task,
                     (void*)0,
                     (OS_STK*)&START_TASK_STK[START_STK_SIZE-1],
                     START_TASK_PRIO); //������ʼ����
	OSStart(); //��ʼ����
}

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
        OSTaskCreate(Led_Task_B,(void*)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);//����LEDB����
        OSTaskCreate(Led_Task_C,(void*)0,(OS_STK*)&LED2_TASK_STK[LED2_STK_SIZE-1],LED2_TASK_PRIO);//����LEDC����
        OSTaskCreate(DATA_Task,(void*)0,(OS_STK*)&DATA_TASK_STK[DATA_STK_SIZE-1],DATA_TASK_PRIO);//�������ݴ�������
	OSTaskSuspend(START_TASK_PRIO);//����ʼ����
	OS_EXIT_CRITICAL();  //�˳��ٽ���(���ж�)
}

//����A---����״̬
//void Led_Task_A(void *pdata)
//{       
//     
//     
//      
//          
//     while(1)
//     {      
//          
//          if(flag.current_state == 0)                                 															//?����?��?��???��y?Y
//          {
//            Led_Status(flag.current_state);//����ģʽ����
//        /*  Volume.recv_len = strlen((const char *)SendBuff); //����
//            MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
//            while(1)
//             {
//                if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
//                { 
//                  DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
//                  break; 
//                 }
//             }
//         if(0 == strcmp((char const *)SendBuff,"exit"))//�˳�����ģʽ   
//         {
//            Volume.recv_times=0;
//            flag.current_state=0;
//            flag.task_flag=0;
//
//            Led_Status(flag.current_state);//����ģʽ����	
//            u8 mode[]={"\r\n������ָ���л�״̬��go_set�л�������״̬��go_start�л�������״̬\r\n"};
//            Print_Mode_Switch(mode);//��ӡ��Ϣ
//            
//         }
//                                                              
//         else if(0 == strcmp((char const *)SendBuff,"go_set") && 0==Volume.recv_times)//��������ģʽ
//        {
//          Volume.recv_times=1;//��һ�ν��յ���Ч����
//          flag.current_state=1;
//          flag.task_flag=1;
//          memset(Volume.data,0,600);//��������״̬ʱ������ݴ洢����
//          //OSTaskResume(5);//�ָ�����B
//        }
//        else if(0 == strcmp((char const *)SendBuff,"go_start") && 0==Volume.recv_times)//��������ģʽ
//        {
//           Volume.recv_times=1;//��һ�ν��յ���Ч����
//           flag.current_state=2;
//           flag.task_flag=2;
//          // OSTaskResume(6);//�ָ�����C
//        }
//        else if(1==flag.current_state &&  0!=Volume.recv_times)
//        {
//          Volume.recv_times++;
//          flag.task_flag=1;
//         // OSTaskResume(5);//�ָ�����B
//        }
//        else if(2==flag.current_state &&  0!=Volume.recv_times)
//        {
//          Volume.recv_times++;
//          flag.task_flag=2;
//         // OSTaskResume(6);//�ָ�����C
//        }else//������ʾ
//        {
//          strcpy((char *)SendBuff,(const char*)err_display);
//          Volume.recv_len = strlen((const char *)SendBuff); 
//          
//          MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
//           while(1)
//            {
//                if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
//                { 
//                  DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
//                  break; 
//                }
//             }
//        }
//        */
//          
//
//           
//          
//          }
//        
//        delay_ms(20);
//        //OSTaskSuspend(OS_PRIO_SELF);//������������
//      } 
//        
//	
//}

//LED����B---����״̬
void Led_Task_B(void *pdata)
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
        Led_Status(flag.current_state);//ģʽ���� 
        flag.rx_flag = 0;
      }else
      {
        u8 err= Data_Check();
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
          Data_Save();
          
          memset(Rx_Buff,0,650);//��ս������ݻ�����
        }else
        {
           memset(Rx_Buff,0,650);//��ս������ݻ�����
           switch(err)//���ݴ���Ŵ�ӡ��Ϣ
           {
           case 1:{ 
                    strcpy((char *)message,"�������ʴ���!");
                    Print_Mode_Switch(message);
                    memset(message,0,200);
                   };break;
           case 2:{ 
                    strcpy((char *)message,"�����������!");
                    Print_Mode_Switch(message);
                    memset(message,0,200);
                   };break; 
           case 3:{ 
                    strcpy((char *)message,"�����ģʽ����!");
                    Print_Mode_Switch(message);
                    memset(message,0,200);
                   };break; 
           case 4:{ 
                    strcpy((char *)message,"�ȴ�ʱ�����!");
                    Print_Mode_Switch(message);
                    memset(message,0,200);
                   };break;
           case 5:{ 
                    strcpy((char *)message,"�źŶ��Ӵ���!");
                    Print_Mode_Switch(message);
                    memset(message,0,200);
                   };break;
           case 6:{ 
                    strcpy((char *)message,"��ת������!");
                    Print_Mode_Switch(message);
                    memset(message,0,200);
                   };break;
           case 7:{ 
                    strcpy((char *)message,"�����������!");
                    Print_Mode_Switch(message);
                    memset(message,0,200);
                   };break;
           case 8:{ 
                    strcpy((char *)message,"����ͷ����!");
                    Print_Mode_Switch(message);
                    memset(message,0,200);
                   };break;
           case 9:{ 
                    strcpy((char *)message,"���ݳ��ȴ���!");
                    Print_Mode_Switch(message);
                    memset(message,0,200);
                   };break;
                  
           }
          flag.rx_flag = 0;
        }
      
      }
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
                    
                    Delay_ms(Volume.data[Volume.pulse_offset[Volume.sd]+3]);
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

//���ݴ�������
void DATA_Task(void *pdata)
{
  INT8U err_box;//���������
  u8 *msg;//������Ϣ����
  //result= *((u8 *)OSMboxPend(Str_Box,0,&err)); //������Ϣ����
  
  
  while(1)
  {
    msg=(u8 *)OSMboxPend(DATA_MBOX,0,&err_box);//�ȴ���Ϣ��������
    if(*msg == 0)
    {
        switch(flag.current_state)//�жϵ�ǰ״̬
        {
        case 0:{//����
          if(0 == strcmp((char const *)Rx_Buff,"go_set"))//��������ģʽ
          {
            memset(Rx_Buff,0,650);//��ս������ݻ�����
            
            flag.current_state=1;
            Led_Status(flag.current_state);//ģʽ����      
            //flag.task_flag = 1;
            //OSMboxPost(DATA_MBOX,&flag.task_flag);//�������ݷ���
            //OSTaskResume(5);//�ָ�����B
          }
          else if(0 == strcmp((char const *)Rx_Buff,"go_start") && 0==Volume.recv_times)//��������ģʽ
          {
            memset(Rx_Buff,0,650);//��ս������ݻ�����
           Volume.recv_times=1;//��һ�ν��յ���Ч����
           flag.current_state=2;
           flag.task_flag=2;
          }
          else
          {
              memset(Rx_Buff,0,650);//��ս������ݻ�����
              u8 err_display[]={"\r\n�������ݴ������������룡"};
              Print_Mode_Switch(err_display);//��ӡ��Ϣ         
          }
         };break;
         
          case 1:{//����
            flag.rx_flag = 1;
         };break;
         
          case 2:{//����
            flag.rx_flag = 2;
         };break;
         
       }
    }

    //DMA_Cmd(DMA2_Stream2, ENABLE);     //��DMA,���ݴ�����ɺ��������ͨ������
    delay_ms(30);
  }
  
  //OSTaskSuspend(OS_PRIO_SELF);//������������
}


void USART1_IRQHandler(void)//���ڿ����ж�
{
        volatile u16 temp;
        if(USART_GetITStatus(USART1,USART_IT_IDLE) != RESET)
        {
                DMA_Cmd(DMA2_Stream2, DISABLE); //�ر�DMA,��ֹ�������������
                
                temp = USART1->SR;
                temp = USART1->DR;//���IDLE��־λ
               
                //USART_ClearFlag(USART1,USART_IT_IDLE);
                
                Volume.UART1_ReceiveSize =RECEIVE_BUF_SIZE - DMA_GetCurrDataCounter(DMA2_Stream2);
                
                if(Volume.UART1_ReceiveSize !=0)//�������ݳ��Ȳ�Ϊ0
                {
                      flag.rx_flag=0;  //����֡���ձ�־λ��Ϊ1
                      OSMboxPost(DATA_MBOX,&flag.rx_flag);
                                             
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
