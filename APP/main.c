#include "main.h"

u16 pulse_num=10;//脉冲个数，最大65536个
char temp_pulse_num[10];//脉冲计数


char USART_ReceiveString[50];									//接收PC端发送过来的字符										//接收消息标志位
int Receive_sum = 0; //数组下标
u32  frequency= 10; //脉冲频率
	
RCC_ClocksTypeDef get_rcc_clock;

int main(void)
{
 
        RCC_GetClocksFreq(&get_rcc_clock);//查看系统时钟
        
      //  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//系统中断优先级分组
        
        SystemInit();          //时钟初始化
	//delay_init(168);     //延时初始化       
        LED_Init();          //LED初始化
	//OSInit();        //UCOS初始化        
	//uart_init(9600);	
   
        LED0=0;//系统启动，LED0灯亮，空闲模式
        LED1=0;
      //  TIM_Initializes();//定时器初始化

	//OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO); //创建开始任务
	//OSStart(); //开始任务
}

/*

u32 simple_atoi(char *source) //字符串转整形
{
	int length = strlen(source); //计算长度
	u32 temp_sum = 0;
	for(int tmp=0;tmp<length;tmp++) temp_sum += (source[tmp]-'0') * pow(10,length-1-tmp); //字符转数字并合并
	return temp_sum;
}

void myitoa (u16 num,char str[])//整型转字符串
{
        u8 wei=0;

        do{
            str[wei++]=num%10+'0';//取下一个数字
           }
        while ((num/=10)>0);//删除该数字
        
        u8 str_len = strlen(str)-1;
        u8 start_wei=0;
        
        while(str_len>start_wei)
        {
          str[start_wei]^=str[str_len];
          str[str_len]^=str[start_wei];
          str[start_wei]^=str[str_len];
          str_len--;
          start_wei++;
        }         
}




//开始任务
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
	OSStatInit();  //开启统计任务
	
	OS_ENTER_CRITICAL();  //进入临界区(关闭中断)
	//OSTaskCreate(led_taskA,(void*)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);//创建LEDA任务
        OSTaskCreate(led_taskB,(void*)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);//创建LEDB任务
	OSTaskSuspend(START_TASK_PRIO);//挂起开始任务
	OS_EXIT_CRITICAL();  //退出临界区(开中断)
}

//LED任务A
void led_taskA(void *pdata)
{
  
          // TIM14_PWM_Init(10000,84-1);
          //TIM_SetCompare1(TIM14,0); 
         while(1) 
	{

         // LED_TOGGLE();  
         // TIM_SetCompare1(TIM14,0);                              
          delay_ms(10);    
        
         // PWM_Output(1000, 20, 1000);                    
               // delay_ms(10);
        }
       // OSTaskSuspend(OS_PRIO_SELF);//挂起自身任务
       
	
}

//LED任务B
void led_taskB(void *pdata)
{
  

  u8 recv_len =0;
  u8 times=0;
  while(1)
  {
      if(USART_RX_STA&0x8000)
      {
        recv_len=USART_RX_STA&0x3fff;
       for(times=0;times<recv_len;times++)//回显
	{
          USART_SendData(USART1, USART_RX_BUF[times]);         
          while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	}
             
         switch(current_state)
         {
          case 0:{           //空闲状态
            
                 
                      LED0=0;
                      LED1=1;
                      LED2=1;
                  
      
               
                      if(strcmp(USART_RX_BUF,"set") == 0)//进入设置脉冲数状态时Y0灯亮
                      {
                      LED0=1;
                      LED1=0;
                      LED2=1;  
                      current_state=1;
                      }
                 
                      if(strcmp(USART_RX_BUF,"start") == 0)//进入运行状态时Y1灯亮
                      {
                      LED0=1;
                      LED1=1;
                      LED2=0;
                      
                      PWM_Output(frequency, 50); 
                     // ouput_pulse(pulse_num);//
                 
                      current_state=2;
                      }	 
               
                  };break;
                  
                  
           
                  
          case 1:{                    //设置脉冲数状态
                  pulse_num=simple_atoi(USART_RX_BUF);
                  ouput_pulse(pulse_num);
                  LED10=!LED10;
                  current_state=0;
              
                  };break;
                  
                  
                  
                  
          case 2:{                    //运行状态
              
             frequency= simple_atoi(USART_RX_BUF);
             if(frequency > 1000)
             {
               if(frequency > 100000)
               {
                PWM_CK_CNT =4000000 ;               
                PWM_PRESCALER = (84000000/4000000 - 1);  //预分频值
                PWM_TIM_Configuration();
               }else
               {
                  PWM_CK_CNT =1000000 ;               
                PWM_PRESCALER = (84000000/1000000 - 1);  //预分频值
                PWM_TIM_Configuration();
               }
               
             }else
             {             
                PWM_CK_CNT =10000 ;               
                PWM_PRESCALER = (84000000/10000 - 1);  //预分频值
                PWM_TIM_Configuration();
             }
            
              
              PWM_Output(frequency, 50);
             
               //TIM14_PWM_Init(frequency,84-1);
    
            };break;
            
          }
         
         memset(USART_RX_BUF,0,50);//清空数组
         Receive_sum = 0; //重新存放
         USART_RX_STA=0;
         
      }
         
    // interrupt_times=0;//清除中断位
  }
}



void USART1_IRQHandler(void)                	//串口1中断服务程序
{   										//接收消息标志位     

        //OSIntExit();  						
	u8 Res;
	OSIntEnter();    
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	
		
		if((USART_RX_STA&0x8000)==0)
		{
			if(USART_RX_STA&0x4000)
			{
				if(Res!=0x0a)USART_RX_STA=0;
				else {
                                    USART_RX_STA|=0x8000;
                                  //  interrupt_times=1;
                                }
			}
			else    
			{	
                                  if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;  
				}		 
			}
		}   		 
  } 
  
        
	OSIntExit();  											 


} 


void CNT_TIM_IRQHandler(void)
{
  if(TIM_GetITStatus(CNT_TIMx, TIM_IT_Update) != RESET)
  {
   // temp_pulse_num[0]=pulse_num;//脉冲数
    myitoa(pulse_num,temp_pulse_num);
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
