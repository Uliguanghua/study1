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
      Led_Status(flag.current_state);//空闲模式灯亮
      u8 start[]={"请输入指令切换状态：go_set切换到设置状态，go_start切换到运行状态\r\n"}; 
      Print_Mode_Switch(start);//打印模式信息
      OSTaskCreate(DATA_Task,(void*)0,(OS_STK*)&DATA_TASK_STK[DATA_STK_SIZE-1],DATA_TASK_PRIO);//创建数据处理任务
      
     while(1)
     {
      if(flag.rx_flag)                                 															//?óê?μ?ò???êy?Y
      {
      flag.rx_flag = 0;  //清除接收标志位
    /*  Volume.recv_len = strlen((const char *)SendBuff); //回显
        MYDMA_Enable(DMA2_Stream7,Volume.recv_len); 
        while(1)
	 {
            if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)
            { 
              DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
              break; 
             }
         }
     if(0 == strcmp((char const *)SendBuff,"exit"))//退出设置模式   
     {
        Volume.recv_times=0;
        flag.current_state=0;
        flag.task_flag=0;

        Led_Status(flag.current_state);//空闲模式灯亮	
        u8 mode[]={"\r\n请输入指令切换状态：go_set切换到设置状态，go_start切换到运行状态\r\n"};
        Print_Mode_Switch(mode);//打印信息
        
     }
                                                          
     else if(0 == strcmp((char const *)SendBuff,"go_set") && 0==Volume.recv_times)//进入设置模式
    {
      Volume.recv_times=1;//第一次接收到有效数据
      flag.current_state=1;
      flag.task_flag=1;
      memset(Volume.data,0,600);//进入设置状态时清空数据存储区域
      //OSTaskResume(5);//恢复任务B
    }
    else if(0 == strcmp((char const *)SendBuff,"go_start") && 0==Volume.recv_times)//进入运行模式
    {
       Volume.recv_times=1;//第一次接收到有效数据
       flag.current_state=2;
       flag.task_flag=2;
      // OSTaskResume(6);//恢复任务C
    }
    else if(1==flag.current_state &&  0!=Volume.recv_times)
    {
      Volume.recv_times++;
      flag.task_flag=1;
     // OSTaskResume(5);//恢复任务B
    }
    else if(2==flag.current_state &&  0!=Volume.recv_times)
    {
      Volume.recv_times++;
      flag.task_flag=2;
     // OSTaskResume(6);//恢复任务C
    }else//错误提示
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
    */
  }
    Led_Status(flag.current_state);//空闲模式灯亮
    //delay_ms(20);
    OSTaskSuspend(OS_PRIO_SELF);//挂起自身任务

  } 
       
	
}

//LED任务B---设置状态
void Led_Task_B(void *pdata)
{
  while(1)
  {
    if(flag.current_state==1 && flag.task_flag==1) 
    {
      Led_Status(flag.current_state);//设置模式灯亮   
      if(1==Volume.recv_times)
      {
       u8 output_clue[]={"\r\n请指定输出脉冲端子:\r\n"};
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
        if(Output_Check(SendBuff))     //输出端子有效性检测
         {   
           Volume.output_port=SendBuff[1]-'0';//端子保存
           switch(Volume.output_port)//根据端子配置定时器
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
           u8 output_clue[]={"\r\n请输入脉冲总段数:\r\n"};
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
         if(Number_Sum_Check(SendBuff)) //总脉冲段数有效性检测
         {
           
           Volume.pulse_offset[0]=1;
           Volume.pulse_num=My_Atoi((char *)SendBuff);//总脉冲段数保存
           Volume.data[0]=(Volume.output_port<<16)+Volume.pulse_num;//首数据
           
           u8 speed_clue[]={"\r\n请输入脉冲数率（1~200000）\r\n"};
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
         if(Speed_Check(SendBuff)) //脉冲速率有效性检测
         {
           Volume.data[Volume.pulse_offset[Volume.sd]+0]=My_Atoi((char *)SendBuff);//脉冲速率保存
           u8 speed_clue[]={"\r\n请输入脉冲个数(1 ~ 2147483647)\r\n"};
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
         if(Number_Check(SendBuff)) //脉冲个数有效性检测
         {
           Volume.data[Volume.pulse_offset[Volume.sd]+1]=My_Atoi((char *)SendBuff);//脉冲个数保存
           u8 mode_clue[]={"\r\n请设置等待模式（脉冲发送完成finish、wait时间模式wait_time、wait信号模式wait_signal、ACT时间模式act_time、EXT信号模式ext_signal、EXT信号/脉冲发送完成模式、ext_finish）\r\n"};
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
         if(Mode_Check(SendBuff)) //脉冲模式有效性检测
         {
           if(0 == strcmp((char const *)SendBuff,"wait_time") || 0 == strcmp((char const *)SendBuff,"act_time"))
           {       
             if(0 == strcmp((char const *)SendBuff,"wait_time"))
               Volume.mode=0x0002;//wait时间模式
             else
               Volume.mode=0x0004;//ACT时间模式
              u8 mode_clue[]={"\r\n请输入存放时间的寄存器序号（T1~T100）\r\n"};
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
              Volume.mode=0x0003;//wait信号模式
              u8 mode_clue[]={"\r\n请输入信号寄存器序号（S1~S100）\r\n"};
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
               Volume.mode=0x0006;//EXT信号/脉冲发送完成模式
             else
               Volume.mode=0x0005;//EXT信号模式
              u8 mode_clue[]={"\r\n请输入外部信号触发端子(w0、w1、w2、w3、w4)\r\n"};
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
             Volume.data[Volume.pulse_offset[Volume.sd]+2]=0x00010000;//脉冲发送完成模式
             Volume.data[Volume.pulse_offset[Volume.sd]+3] = 0;
             Volume.recv_times+=2;//跳过两次数据接收
              u8 mode_clue[]={"\r\n请输入要跳转到的段序号（0是默认下一段，段序号要小于等于总段数且要避免死循环）\r\n"};
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
        
      }else if(7==Volume.recv_times) //寄存器类型
       {
         if(Volume.mode == 0x0002 || Volume.mode == 0x0004)//时间寄存器
         {
           if(Time_Register_Check(SendBuff))
           {
             simulation.time=(u8)My_Atoi((char *)SendBuff);//时间寄存器序号存储
             if(Volume.mode == 0x0002)//数据保存
              Volume.data[Volume.pulse_offset[Volume.sd]+2] = 0x00020000+simulation.time;
             else
              Volume.data[Volume.pulse_offset[Volume.sd]+2] = 0x00040000+simulation.time;
             u8 speed_clue[]={"\r\n请输入设定的等待时间（1 ~ 65535ms）\r\n"};
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
           
         }else  if(Volume.mode == 0x0003)//信号寄存器
         {
           if(Signal_Register_Check(SendBuff))
           {
             simulation.ext_signal=(u8)My_Atoi((char *)SendBuff);//信号寄存器序号存储
             Volume.data[Volume.pulse_offset[Volume.sd]+2] = 0x00030000+simulation.ext_signal;//数据保存
             Volume.data[Volume.pulse_offset[Volume.sd]+3] = 0;
             
             u8 speed_clue[]={"\r\n请输入要跳转到的段序号（0是默认下一段，段序号要小于等于总段数且要避免死循环）\r\n"};
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
           
         }else  if(Volume.mode == 0x0005 || Volume.mode == 0x0006)//外部信号端子
         {
          if(External_Signal_Check(SendBuff))
          {
            simulation.ext_signal=(u8)My_Atoi((char *)SendBuff);//外部信号端子序号存储
            if(Volume.mode == 0x0005)//数据保存
              Volume.data[Volume.pulse_offset[Volume.sd]+2] = 0x00050000+simulation.ext_signal;
            else
              Volume.data[Volume.pulse_offset[Volume.sd]+2] = 0x00060000+simulation.ext_signal;
            Volume.data[Volume.pulse_offset[Volume.sd]+3] = 0;
            
            u8 speed_clue[]={"\r\n请输入要跳转到的段序号（0是默认下一段，段序号要小于等于总段数且要避免死循环）\r\n"};
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
                    
      }else if(8==Volume.recv_times) //等待时间 
       {
         if(Time_Check(SendBuff))
         {
             
             Volume.data[Volume.pulse_offset[Volume.sd]+3] = My_Atoi((char*)SendBuff);//等待时间保存
             u8 speed_clue[]={"\r\n请输入要跳转到的段序号（0是默认下一段，段序号要小于等于总段数且要避免死循环）\r\n"};
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
      }else if(9==Volume.recv_times) //脉冲段跳转
       {
         if(Section_Num_Check(SendBuff,Volume.pulse_num))
         {
           Volume.data[Volume.pulse_offset[Volume.sd]+4]=My_Atoi((char *)SendBuff);
           Volume.sd++;
           
           if(Volume.sd == Volume.pulse_num)//脉冲段数据设置完成
           {          
             u8 speed_clue[]={"\r\n数据设置完成\r\n请输入set_end结束设置\r\n"};
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
                  
                  u8 speed_clue[]={"\r\n请输入脉冲数率（1~200000）\r\n"};
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
      }else if(10==Volume.recv_times) //设置结束
       {
         if(End_Check(SendBuff))
         {
             Volume.recv_times=0;
             Volume.sd=0;
             flag.current_state=0;//空闲状态
             Led_Status(flag.current_state);
             u8 speed_clue[]={"\r\n请输入指令切换状态：go_set切换到设置状态，go_start切换到运行状态\r\n"};
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
  //result= *((u8 *)OSMboxPend(Str_Box,0,&err)); //请求消息邮箱
  if(1==flag.rx_flag)
  {
    switch(flag.current_state)//判断当前状态
    {
    case 0:{//空闲
      if(0 == strcmp((char const *)SendBuff,"go_set"))//进入设置模式
      {
        flag.current_state=1;
        Led_Status(flag.current_state);//模式灯亮
           
        memset(Rx_Buff,0,650);//清空接收数据缓冲区
        OSTaskResume(5);//恢复任务B
      }
      else if(0 == strcmp((char const *)SendBuff,"go_start") && 0==Volume.recv_times)//进入运行模式
      {
       Volume.recv_times=1;//第一次接收到有效数据
       flag.current_state=2;
       flag.task_flag=2;
      // OSTaskResume(6);//恢复任务C
      }
    };break;
    case 1:{//设置
    };break;
    case 2:{//运行
    };break;
    
    
    }
    
    flag.rx_flag = 0;
    DMA_Cmd(DMA2_Stream2, ENABLE);     //打开DMA,数据处理完成后接收数据通道开启
  }
  OSTaskSuspend(OS_PRIO_SELF);//挂起自身任务
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
                      flag.rx_flag=1;  //数据帧接收标志位置为1
                                             
                      OSTaskResume(3);//恢复数据处理任务
                }
                DMA_ClearFlag(DMA2_Stream2,DMA_FLAG_TCIF2 | DMA_FLAG_FEIF2 | DMA_FLAG_DMEIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_HTIF2);//清除传输完成标志
                DMA_SetCurrDataCounter(DMA2_Stream2, RECEIVE_BUF_SIZE);
                //DMA_Cmd(DMA2_Stream2, ENABLE);     //打开DMA
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
