#include "main.h"


u16 Pulse_num=10;//脉冲个数，最大65536个
char temp_pulse_num[10];//脉冲计数


//char USART_ReceiveString[50];									//接收PC端发送过来的字符										//接收消息标志位
//int Receive_sum = 0; //数组下标
//u32  frequency= 10; //脉冲频率
	
RCC_ClocksTypeDef get_rcc_clock;

int main(void)
{
        
        RCC_GetClocksFreq(&get_rcc_clock);//查看系统时钟
        
        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//系统中断优先级分组
        
        SystemInit();          //时钟初始化
	delay_init(168);     //延时初始化       
        LED_Init();          //LED初始化
	OSInit();          //UCOS初始化        
	uart_init(9600);	//串口初始化
        MYDMASEND_Config((u32)&(USART1->DR),(u32)SendBuff,SEND_BUF_SIZE);//DMA发送初始化
        MYDMAREC_Config((u32)&(USART1->DR),(u32)Rx_Buff,RECEIVE_BUF_SIZE);//DMA接收初始化
        
        
   
      //  TIM_Initializes();//定时器初始化
       
	OSTaskCreate(Start_Task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO); //创建开始任务
	OSStart(); //开始任务
}





//开始任务
void Start_Task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
	OSStatInit();  //开启统计任务
	
	OS_ENTER_CRITICAL();  //进入临界区(关闭中断)
	OSTaskCreate(Led_Task_A,(void*)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);//创建LEDA任务
        OSTaskCreate(Led_Task_B,(void*)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);//创建LEDB任务
         OSTaskCreate(Led_Task_C,(void*)0,(OS_STK*)&LED2_TASK_STK[LED2_STK_SIZE-1],LED2_TASK_PRIO);//创建LEDC任务
	OSTaskSuspend(START_TASK_PRIO);//挂起开始任务
	OS_EXIT_CRITICAL();  //退出临界区(开中断)
}

//任务A---空闲状态
void Led_Task_A(void *pdata)
{       
      Led_Status(current_state);//空闲模式灯亮	
      u8 start[]={"请输入指令切换状态：go_set切换到设置状态，go_start切换到运行状态\r\n"};
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
    if(rx_flag)                                 															//?óê?μ?ò???êy?Y
    {
      rx_flag = 0;  //清除接收标志位
      recv_len = strlen((const char *)SendBuff); //回显
      MYDMA_Enable(DMA2_Stream7,recv_len); 
       while(1)
	 {
            if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
            { 
              DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
              break; 
             }
         }
          
                                                          
        
    if(0 == strcmp((char const *)SendBuff,"go_set") && 0==recv_times)//进入设置模式
    {
      recv_times=1;//第一次接收到有效数据
      current_state=1;
      task_flag=1;
      //OSTaskResume(5);//恢复任务B
    }
    else if(0 == strcmp((char const *)SendBuff,"go_start") && 0==recv_times)//进入运行模式
    {
       recv_times=1;//第一次接收到有效数据
       current_state=2;
       task_flag=2;
      // OSTaskResume(6);//恢复任务C
    }
    else if(1==current_state &&  0!=recv_times)
    {
      recv_times++;
      task_flag=1;
     // OSTaskResume(5);//恢复任务B
    }
    else if(2==current_state &&  0!=recv_times)
    {
      recv_times++;
      task_flag=2;
     // OSTaskResume(6);//恢复任务C
    }else//错误提示
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
    Led_Status(current_state);//空闲模式灯亮
    delay_ms(20);
  //  OSTaskSuspend(OS_PRIO_SELF);//挂起自身任务

  } 
       
	
}

//LED任务B---设置状态
void Led_Task_B(void *pdata)
{
  while(1)
  {
    if(current_state==1 && task_flag==1) 
    {
      Led_Status(current_state);//设置模式灯亮   
      if(1==recv_times)
      {
       u8 output_clue[]={"\r\n请指定输出脉冲端子:\r\n"};
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
        if(Output_Check(SendBuff))     //输出端子有效性检测
         {   
           output_port=SendBuff[1]-'0';//端子保存
           u8 output_clue[]={"\r\n请输入脉冲总段数:\r\n"};
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
         if(Number_Sum_Check(SendBuff)) //总脉冲段数有效性检测
         {
           data[0]=(output_port<<16)+pulse_num;//首数据
           pulse_offset[0]=1;
           pulse_num=My_Atoi((char *)SendBuff);//总脉冲段数保存
           u8 speed_clue[]={"\r\n请输入脉冲数率（1~200000）\r\n"};
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
         if(Speed_Check(SendBuff)) //脉冲速率有效性检测
         {
           data[pulse_offset[sd]+0]=My_Atoi((char *)SendBuff);//脉冲速率保存
           u8 speed_clue[]={"\r\n请输入脉冲个数(1 ~ 2147483647)\r\n"};
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
         if(Number_Check(SendBuff)) //脉冲个数有效性检测
         {
           data[pulse_offset[sd]+1]=My_Atoi((char *)SendBuff);//脉冲个数保存
           u8 mode_clue[]={"\r\n请设置等待模式（脉冲发送完成finish、wait时间模式wait_time、wait信号模式wait_signal、ACT时间模式act_time、EXT信号模式ext_signal、EXT信号/脉冲发送完成模式、ext_finish）\r\n"};
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
         if(Mode_Check(SendBuff)) //脉冲模式有效性检测
         {
           if(0 == strcmp((char const *)SendBuff,"wait_time") || 0 == strcmp((char const *)SendBuff,"act_time"))
           {       
             if(0 == strcmp((char const *)SendBuff,"wait_time"))
               mode=0x0002;//wait时间模式
             else
               mode=0x0004;//ACT时间模式
              u8 mode_clue[]={"\r\n请输入存放时间的寄存器序号（T1~T100）\r\n"};
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
              mode=0x0003;//wait信号模式
              u8 mode_clue[]={"\r\n请输入信号寄存器序号（S1~S100）\r\n"};
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
               mode=0x0006;//EXT信号/脉冲发送完成模式
             else
               mode=0x0005;//EXT信号模式
              u8 mode_clue[]={"\r\n请输入外部信号触发端子(w0、w1、w2、w3、w4)\r\n"};
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
             data[pulse_offset[sd]+2]=0x00010000;//脉冲发送完成模式
             data[pulse_offset[sd]+3] = 0;
             recv_times+=2;//跳过两次数据接收
              u8 mode_clue[]={"\r\n请输入要跳转到的段序号（0是默认下一段，段序号要小于等于总段数且要避免死循环）\r\n"};
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
        
      }else if(7==recv_times) //寄存器类型
       {
         if(mode == 0x0002 || mode == 0x0004)//时间寄存器
         {
           if(Time_Register_Check(SendBuff))
           {
             SendBuff[0]=SendBuff[1];
             SendBuff[1]=SendBuff[2];
             SendBuff[2]=SendBuff[3];
             SendBuff[3]=SendBuff[4];
             time=(u8)My_Atoi((char *)SendBuff);//时间寄存器序号存储
             if(mode == 0x0002)//数据保存
              data[pulse_offset[sd]+2] = 0x00020000+time;
             else
              data[pulse_offset[sd]+2] = 0x00040000+time;
             u8 speed_clue[]={"\r\n请输入设定的等待时间（1 ~ 65535ms）\r\n"};
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
           
         }else  if(mode == 0x0003)//信号寄存器
         {
           if(Signal_Register_Check(SendBuff))
           {
             SendBuff[0]=SendBuff[1];
             SendBuff[1]=SendBuff[2];
             SendBuff[2]=SendBuff[3];
             SendBuff[3]=SendBuff[4];
             signal=(u8)My_Atoi((char *)SendBuff);//信号寄存器序号存储
             data[pulse_offset[sd]+2] = 0x00030000+signal;//数据保存
             data[pulse_offset[sd]+3] = 0;
             
             u8 speed_clue[]={"\r\n请输入要跳转到的段序号（0是默认下一段，段序号要小于等于总段数且要避免死循环）\r\n"};
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
           
         }else  if(mode == 0x0005 || mode == 0x0006)//外部信号端子
         {
          if(External_Signal_Check(SendBuff))
          {
            SendBuff[0]=SendBuff[1];
            SendBuff[1]=SendBuff[2];
            ext_signal=(u8)My_Atoi((char *)SendBuff);//外部信号端子序号存储
            if(mode == 0x0005)//数据保存
              data[pulse_offset[sd]+2] = 0x00050000+ext_signal;
            else
              data[pulse_offset[sd]+2] = 0x00060000+ext_signal;
            data[pulse_offset[sd]+3] = 0;
            
            u8 speed_clue[]={"\r\n请输入要跳转到的段序号（0是默认下一段，段序号要小于等于总段数且要避免死循环）\r\n"};
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
                    
      }else if(8==recv_times) //等待时间 
       {
         if(Time_Check(SendBuff))
         {
             
             data[pulse_offset[sd]+3] = My_Atoi((char*)SendBuff);//等待时间保存
             u8 speed_clue[]={"\r\n请输入要跳转到的段序号（0是默认下一段，段序号要小于等于总段数且要避免死循环）\r\n"};
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
      }else if(9==recv_times) //脉冲段跳转
       {
         if(Section_Num_Check(SendBuff,pulse_num))
         {
           data[pulse_offset[sd]+4]=My_Atoi((char *)SendBuff);
           sd++;
           
           if(sd == pulse_num)//脉冲段数据设置完成
           {          
             u8 speed_clue[]={"\r\n数据设置完成\r\n请输入set_end结束设置\r\n"};
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
                  
                  u8 speed_clue[]={"\r\n请输入脉冲数率（1~200000）\r\n"};
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
      }else if(10==recv_times) //设置结束
       {
         if(End_Check(SendBuff))
         {
             recv_times=0;
             current_state=0;//空闲状态
             Led_Status(current_state);
             u8 speed_clue[]={"\r\n请输入指令切换状态：go_set切换到设置状态，go_start切换到运行状态\r\n"};
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
   // OSTaskSuspend(OS_PRIO_SELF);//挂起自身任务
    task_flag=0;
    delay_ms(20);
  }

 }

//LED任务C---运行状态
void Led_Task_C(void *pdata)
{
    
    while(1)
  {
    if(current_state==2 && task_flag==2)
    {
      
      
      
     
      Led_Status(current_state);//运行模式灯亮
      
    }
    //OSTaskSuspend(OS_PRIO_SELF);//挂起自身任务
    task_flag=0;
    delay_ms(20);
  }
}


void USART1_IRQHandler(void)//串口空闲中断
{
        u16 temp;
        if(USART_GetITStatus(USART1,USART_IT_IDLE) != RESET)
        {
                DMA_Cmd(DMA2_Stream2, DISABLE); //关闭DMA,防止处理其间有数据

                temp = USART1->SR;
                temp = USART1->DR;//清除IDLE标志位
               
                //USART_ClearFlag(USART1,USART_IT_IDLE);
                
                UART1_ReceiveSize =RECEIVE_BUF_SIZE - DMA_GetCurrDataCounter(DMA2_Stream2);
                
                if(UART1_ReceiveSize !=0)//接收数据长度不为0
                {
                        rx_flag=1;  //数据帧接收标志位置为1
                        
                      memset(SendBuff,0,SEND_BUF_SIZE);//清空接收缓冲区
                      for(temp=0;temp<UART1_ReceiveSize;temp++)
                      {
                      SendBuff[temp]= Rx_Buff[temp]; 
                      }
                      memset(Rx_Buff,0,UART1_ReceiveSize);//清空接收缓冲区
                      //OSTaskResume(4);
                }
                DMA_ClearFlag(DMA2_Stream2,DMA_FLAG_TCIF2 | DMA_FLAG_FEIF2 | DMA_FLAG_DMEIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_HTIF2);//清除传输完成标志
                DMA_SetCurrDataCounter(DMA2_Stream2, RECEIVE_BUF_SIZE);
                DMA_Cmd(DMA2_Stream2, ENABLE);     //打开DMA
        }
}




/*
void CNT_TIM_IRQHandler(void)
{
  if(TIM_GetITStatus(CNT_TIMx, TIM_IT_Update) != RESET)
  {
   // temp_pulse_num[0]=pulse_num;//脉冲数
    My_Itoa(pulse_num,temp_pulse_num);
    TIM_ClearITPendingBit(CNT_TIMx, TIM_IT_Update);
    TIM_Cmd(PWM_TIMx, DISABLE);                  
    TIM_Cmd(CNT_TIMx, DISABLE);  
    USART_RX_STA|=0x8000;
    current_state=0;//回到空闲状态
    u16 times=0;
    u16 pulse_len=strlen(temp_pulse_num);
       for(times=0;times<pulse_len;times++)
	{
          USART_SendData(USART1, temp_pulse_num[times]); //反馈脉冲数        
          while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	}
    memset(temp_pulse_num,0,10);
  }
}
*/
