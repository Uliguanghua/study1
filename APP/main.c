#include "main.h"


//u16 Pulse_num=10;//脉冲个数，最大65536个
//char temp_pulse_num[10];//脉冲计数


//char USART_ReceiveString[50];									//接收PC端发送过来的字符										//接收消息标志位
//int Receive_sum = 0; //数组下标
//
	
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
        data_init();//参数初始化
        Delay_TIM2_Configuration();//计数定时器初始化
        MYDMASEND_Config((u32)&(USART1->DR),(u32)SendBuff,SEND_BUF_SIZE);//DMA发送初始化
        MYDMAREC_Config((u32)&(USART1->DR),(u32)Rx_Buff,RECEIVE_BUF_SIZE);//DMA接收初始化
        
        DATA_MBOX=OSMboxCreate((void*)0);//建立消息邮箱
       
	OSTaskCreate(Start_Task,
                     (void*)0,
                     (OS_STK*)&START_TASK_STK[START_STK_SIZE-1],
                     START_TASK_PRIO); //创建开始任务
	OSStart(); //开始任务
}

//开始任务
void Start_Task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
	OSStatInit();  //开启统计任务
        Led_Status(flag.current_state);//模式灯亮 
        u8 start[]={"请输入指令切换状态：go_set切换到设置状态，go_start切换到运行状态\r\n"}; 
        Print_Mode_Switch(start);//打印模式信息
	
	OS_ENTER_CRITICAL();  //进入临界区(关闭中断)
	//OSTaskCreate(Led_Task_A,(void*)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);//创建LEDA任务
        OSTaskCreate(Led_Task_B,(void*)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);//创建LEDB任务
        OSTaskCreate(Led_Task_C,(void*)0,(OS_STK*)&LED2_TASK_STK[LED2_STK_SIZE-1],LED2_TASK_PRIO);//创建LEDC任务
        OSTaskCreate(DATA_Task,(void*)0,(OS_STK*)&DATA_TASK_STK[DATA_STK_SIZE-1],DATA_TASK_PRIO);//创建数据处理任务
	OSTaskSuspend(START_TASK_PRIO);//挂起开始任务
	OS_EXIT_CRITICAL();  //退出临界区(开中断)
}

//任务A---空闲状态
//void Led_Task_A(void *pdata)
//{       
//     
//     
//      
//          
//     while(1)
//     {      
//          
//          if(flag.current_state == 0)                                 															//?óê?μ?ò???êy?Y
//          {
//            Led_Status(flag.current_state);//空闲模式灯亮
//        /*  Volume.recv_len = strlen((const char *)SendBuff); //回显
//            MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
//            while(1)
//             {
//                if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
//                { 
//                  DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
//                  break; 
//                 }
//             }
//         if(0 == strcmp((char const *)SendBuff,"exit"))//退出设置模式   
//         {
//            Volume.recv_times=0;
//            flag.current_state=0;
//            flag.task_flag=0;
//
//            Led_Status(flag.current_state);//空闲模式灯亮	
//            u8 mode[]={"\r\n请输入指令切换状态：go_set切换到设置状态，go_start切换到运行状态\r\n"};
//            Print_Mode_Switch(mode);//打印信息
//            
//         }
//                                                              
//         else if(0 == strcmp((char const *)SendBuff,"go_set") && 0==Volume.recv_times)//进入设置模式
//        {
//          Volume.recv_times=1;//第一次接收到有效数据
//          flag.current_state=1;
//          flag.task_flag=1;
//          memset(Volume.data,0,600);//进入设置状态时清空数据存储区域
//          //OSTaskResume(5);//恢复任务B
//        }
//        else if(0 == strcmp((char const *)SendBuff,"go_start") && 0==Volume.recv_times)//进入运行模式
//        {
//           Volume.recv_times=1;//第一次接收到有效数据
//           flag.current_state=2;
//           flag.task_flag=2;
//          // OSTaskResume(6);//恢复任务C
//        }
//        else if(1==flag.current_state &&  0!=Volume.recv_times)
//        {
//          Volume.recv_times++;
//          flag.task_flag=1;
//         // OSTaskResume(5);//恢复任务B
//        }
//        else if(2==flag.current_state &&  0!=Volume.recv_times)
//        {
//          Volume.recv_times++;
//          flag.task_flag=2;
//         // OSTaskResume(6);//恢复任务C
//        }else//错误提示
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
//        //OSTaskSuspend(OS_PRIO_SELF);//挂起自身任务
//      } 
//        
//	
//}

//LED任务B---设置状态
void Led_Task_B(void *pdata)
{
  u8 message[200];//设置模式下的信息打印
  memset(message,0,200);
  while(1)
  {
    if(flag.current_state==1 && flag.rx_flag == 1) 
    {
      if(0 == strcmp((char const *)Rx_Buff,"exit") )
      {
        memset(Rx_Buff,0,650);//清空接收数据缓冲区
        flag.current_state=0;//回到空闲
        Led_Status(flag.current_state);//模式灯亮 
        flag.rx_flag = 0;
      }else
      {
        u8 err= Data_Check();
        if(0 == err )
        {
          
          strcpy((char *)message,"数据设置成功!");
          Print_Mode_Switch(message);
          memset(message,0,200);
          flag.current_state=0;//回到空闲
          flag.rx_flag = 0;
          Led_Status(flag.current_state);//模式灯亮 
          
          //数据保存
          memset(Volume.data,0,101);//数据清空
          Data_Save();
          
          memset(Rx_Buff,0,650);//清空接收数据缓冲区
        }else
        {
           memset(Rx_Buff,0,650);//清空接收数据缓冲区
           switch(err)//根据错误号打印信息
           {
           case 1:{ 
                    strcpy((char *)message,"脉冲速率错误!");
                    Print_Mode_Switch(message);
                    memset(message,0,200);
                   };break;
           case 2:{ 
                    strcpy((char *)message,"脉冲个数错误!");
                    Print_Mode_Switch(message);
                    memset(message,0,200);
                   };break; 
           case 3:{ 
                    strcpy((char *)message,"脉冲段模式错误!");
                    Print_Mode_Switch(message);
                    memset(message,0,200);
                   };break; 
           case 4:{ 
                    strcpy((char *)message,"等待时间错误!");
                    Print_Mode_Switch(message);
                    memset(message,0,200);
                   };break;
           case 5:{ 
                    strcpy((char *)message,"信号端子错误!");
                    Print_Mode_Switch(message);
                    memset(message,0,200);
                   };break;
           case 6:{ 
                    strcpy((char *)message,"跳转数错误!");
                    Print_Mode_Switch(message);
                    memset(message,0,200);
                   };break;
           case 7:{ 
                    strcpy((char *)message,"其他错误错误!");
                    Print_Mode_Switch(message);
                    memset(message,0,200);
                   };break;
           case 8:{ 
                    strcpy((char *)message,"数据头错误!");
                    Print_Mode_Switch(message);
                    memset(message,0,200);
                   };break;
           case 9:{ 
                    strcpy((char *)message,"数据长度错误!");
                    Print_Mode_Switch(message);
                    memset(message,0,200);
                   };break;
                  
           }
          flag.rx_flag = 0;
        }
      
      }
      }
   
   
   // OSTaskSuspend(OS_PRIO_SELF);//挂起自身任务
    
    delay_ms(20);
  }

 }

//LED任务C---运行状态
void Led_Task_C(void *pdata)
{
    while(1)
  {
    if(flag.current_state==2 && flag.task_flag==2)
    {
      Led_Status(flag.current_state);//运行模式灯亮
      Output_Place(Volume.data[0]>>16);//端子指定
      Volume.pulse_num=Volume.data[0]&0x0000FFFF;//总段数
   
      while(Volume.sd+1 <= Volume.pulse_num)
      {
        
        switch(Volume.data[Volume.pulse_offset[Volume.sd]+2]>>16)//判断模式
        {
        case 1:{
                  if(Volume.data[Volume.pulse_offset[Volume.sd]+1] > 65535)//判断脉冲个数
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
                    Frequency_Select(&PWM_CK_CNT,&PWM_PRESCALER,Volume.PWM_TIMx,Volume.data[Volume.pulse_offset[Volume.sd]],Volume.data[0]>>16);//根据频率设定寄存器值
                    PWM_Output(Volume.data[Volume.pulse_offset[Volume.sd]],Volume.PWM_TIMx);//脉冲输出
                    while(flag.send_finish_flag == 0);//循环等待到一段脉冲发送完成
                 
        };break;
        case 2:{
                  if(Volume.data[Volume.pulse_offset[Volume.sd]+1] > 65535)//判断脉冲个数
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
                    Frequency_Select(&PWM_CK_CNT,&PWM_PRESCALER,Volume.PWM_TIMx,Volume.data[Volume.pulse_offset[Volume.sd]],Volume.data[0]>>16);//根据频率设定寄存器值
                    PWM_Output(Volume.data[Volume.pulse_offset[Volume.sd]],Volume.PWM_TIMx);//脉冲输出
                    while(flag.send_finish_flag == 0);//循环等待到一段脉冲发送完成
                    
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
    //OSTaskSuspend(OS_PRIO_SELF);//挂起自身任务
    
    
    delay_ms(20);
  }
}

//数据处理任务
void DATA_Task(void *pdata)
{
  INT8U err_box;//邮箱错误码
  u8 *msg;//邮箱消息数据
  //result= *((u8 *)OSMboxPend(Str_Box,0,&err)); //请求消息邮箱
  
  
  while(1)
  {
    msg=(u8 *)OSMboxPend(DATA_MBOX,0,&err_box);//等待消息邮箱数据
    if(*msg == 0)
    {
        switch(flag.current_state)//判断当前状态
        {
        case 0:{//空闲
          if(0 == strcmp((char const *)Rx_Buff,"go_set"))//进入设置模式
          {
            memset(Rx_Buff,0,650);//清空接收数据缓冲区
            
            flag.current_state=1;
            Led_Status(flag.current_state);//模式灯亮      
            //flag.task_flag = 1;
            //OSMboxPost(DATA_MBOX,&flag.task_flag);//邮箱数据发送
            //OSTaskResume(5);//恢复任务B
          }
          else if(0 == strcmp((char const *)Rx_Buff,"go_start") && 0==Volume.recv_times)//进入运行模式
          {
            memset(Rx_Buff,0,650);//清空接收数据缓冲区
           Volume.recv_times=1;//第一次接收到有效数据
           flag.current_state=2;
           flag.task_flag=2;
          }
          else
          {
              memset(Rx_Buff,0,650);//清空接收数据缓冲区
              u8 err_display[]={"\r\n输入数据错误，请重新输入！"};
              Print_Mode_Switch(err_display);//打印信息         
          }
         };break;
         
          case 1:{//设置
            flag.rx_flag = 1;
         };break;
         
          case 2:{//运行
            flag.rx_flag = 2;
         };break;
         
       }
    }

    //DMA_Cmd(DMA2_Stream2, ENABLE);     //打开DMA,数据处理完成后接收数据通道开启
    delay_ms(30);
  }
  
  //OSTaskSuspend(OS_PRIO_SELF);//挂起自身任务
}


void USART1_IRQHandler(void)//串口空闲中断
{
        volatile u16 temp;
        if(USART_GetITStatus(USART1,USART_IT_IDLE) != RESET)
        {
                DMA_Cmd(DMA2_Stream2, DISABLE); //关闭DMA,防止处理其间有数据
                
                temp = USART1->SR;
                temp = USART1->DR;//清除IDLE标志位
               
                //USART_ClearFlag(USART1,USART_IT_IDLE);
                
                Volume.UART1_ReceiveSize =RECEIVE_BUF_SIZE - DMA_GetCurrDataCounter(DMA2_Stream2);
                
                if(Volume.UART1_ReceiveSize !=0)//接收数据长度不为0
                {
                      flag.rx_flag=0;  //数据帧接收标志位置为1
                      OSMboxPost(DATA_MBOX,&flag.rx_flag);
                                             
                }
                DMA_ClearFlag(DMA2_Stream2,DMA_FLAG_TCIF2 | DMA_FLAG_FEIF2 | DMA_FLAG_DMEIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_HTIF2);//清除传输完成标志
                DMA_SetCurrDataCounter(DMA2_Stream2, RECEIVE_BUF_SIZE);
                DMA_Cmd(DMA2_Stream2, ENABLE);     //打开DMA
        }
        
}


void TIM8_BRK_TIM12_IRQHandler(void)//计数器12溢出事件
{
 
  if(TIM_GetITStatus(Volume.CNT_TIMx, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(Volume.CNT_TIMx, TIM_IT_Update);
    if(Volume.interruput_times > 0)//是否是65535进入
    {
      Volume.interruput_times--;
      if(Volume.interruput_times > 0)
       Pulse_Output_Number(65535,Volume.CNT_TIMx);//个数设置
      else
       Pulse_Output_Number(Volume.pulse_remainder,Volume.CNT_TIMx);
    }else{
      flag.send_finish_flag=1;
      TIM_Cmd(Volume.PWM_TIMx, DISABLE);                  
      TIM_Cmd(Volume.CNT_TIMx, DISABLE);
    }

  }
}

void TIM1_BRK_TIM9_IRQHandler(void)//计数器9溢出事件
{
 
  if(TIM_GetITStatus(Volume.CNT_TIMx, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(Volume.CNT_TIMx, TIM_IT_Update);
    if(Volume.interruput_times > 0)//是否是65535进入
    {
      Volume.interruput_times--;
      if(Volume.interruput_times > 0)
       Pulse_Output_Number(65535,Volume.CNT_TIMx);//个数设置
      else
       Pulse_Output_Number(Volume.pulse_remainder,Volume.CNT_TIMx);
    }else{
      flag.send_finish_flag=1;
      TIM_Cmd(Volume.PWM_TIMx, DISABLE);                  
      TIM_Cmd(Volume.CNT_TIMx, DISABLE);
    }

  }
}
