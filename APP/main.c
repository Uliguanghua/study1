#include "main.h"

u16 pulse_num=10;//������������65536��
char temp_pulse_num[10];//�������


char USART_ReceiveString[50];									//����PC�˷��͹������ַ�										//������Ϣ��־λ
int Receive_sum = 0; //�����±�
u32  frequency= 10; //����Ƶ��
	
RCC_ClocksTypeDef get_rcc_clock;

int main(void)
{
 
        RCC_GetClocksFreq(&get_rcc_clock);//�鿴ϵͳʱ��
        
      //  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//ϵͳ�ж����ȼ�����
        
        SystemInit();          //ʱ�ӳ�ʼ��
	//delay_init(168);     //��ʱ��ʼ��       
        LED_Init();          //LED��ʼ��
	//OSInit();        //UCOS��ʼ��        
	//uart_init(9600);	
   
        LED0=0;//ϵͳ������LED0����������ģʽ
        LED1=0;
      //  TIM_Initializes();//��ʱ����ʼ��

	//OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO); //������ʼ����
	//OSStart(); //��ʼ����
}

/*

u32 simple_atoi(char *source) //�ַ���ת����
{
	int length = strlen(source); //���㳤��
	u32 temp_sum = 0;
	for(int tmp=0;tmp<length;tmp++) temp_sum += (source[tmp]-'0') * pow(10,length-1-tmp); //�ַ�ת���ֲ��ϲ�
	return temp_sum;
}

void myitoa (u16 num,char str[])//����ת�ַ���
{
        u8 wei=0;

        do{
            str[wei++]=num%10+'0';//ȡ��һ������
           }
        while ((num/=10)>0);//ɾ��������
        
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




//��ʼ����
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
	OSStatInit();  //����ͳ������
	
	OS_ENTER_CRITICAL();  //�����ٽ���(�ر��ж�)
	//OSTaskCreate(led_taskA,(void*)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);//����LEDA����
        OSTaskCreate(led_taskB,(void*)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);//����LEDB����
	OSTaskSuspend(START_TASK_PRIO);//����ʼ����
	OS_EXIT_CRITICAL();  //�˳��ٽ���(���ж�)
}

//LED����A
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
       // OSTaskSuspend(OS_PRIO_SELF);//������������
       
	
}

//LED����B
void led_taskB(void *pdata)
{
  

  u8 recv_len =0;
  u8 times=0;
  while(1)
  {
      if(USART_RX_STA&0x8000)
      {
        recv_len=USART_RX_STA&0x3fff;
       for(times=0;times<recv_len;times++)//����
	{
          USART_SendData(USART1, USART_RX_BUF[times]);         
          while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	}
             
         switch(current_state)
         {
          case 0:{           //����״̬
            
                 
                      LED0=0;
                      LED1=1;
                      LED2=1;
                  
      
               
                      if(strcmp(USART_RX_BUF,"set") == 0)//��������������״̬ʱY0����
                      {
                      LED0=1;
                      LED1=0;
                      LED2=1;  
                      current_state=1;
                      }
                 
                      if(strcmp(USART_RX_BUF,"start") == 0)//��������״̬ʱY1����
                      {
                      LED0=1;
                      LED1=1;
                      LED2=0;
                      
                      PWM_Output(frequency, 50); 
                     // ouput_pulse(pulse_num);//
                 
                      current_state=2;
                      }	 
               
                  };break;
                  
                  
           
                  
          case 1:{                    //����������״̬
                  pulse_num=simple_atoi(USART_RX_BUF);
                  ouput_pulse(pulse_num);
                  LED10=!LED10;
                  current_state=0;
              
                  };break;
                  
                  
                  
                  
          case 2:{                    //����״̬
              
             frequency= simple_atoi(USART_RX_BUF);
             if(frequency > 1000)
             {
               if(frequency > 100000)
               {
                PWM_CK_CNT =4000000 ;               
                PWM_PRESCALER = (84000000/4000000 - 1);  //Ԥ��Ƶֵ
                PWM_TIM_Configuration();
               }else
               {
                  PWM_CK_CNT =1000000 ;               
                PWM_PRESCALER = (84000000/1000000 - 1);  //Ԥ��Ƶֵ
                PWM_TIM_Configuration();
               }
               
             }else
             {             
                PWM_CK_CNT =10000 ;               
                PWM_PRESCALER = (84000000/10000 - 1);  //Ԥ��Ƶֵ
                PWM_TIM_Configuration();
             }
            
              
              PWM_Output(frequency, 50);
             
               //TIM14_PWM_Init(frequency,84-1);
    
            };break;
            
          }
         
         memset(USART_RX_BUF,0,50);//�������
         Receive_sum = 0; //���´��
         USART_RX_STA=0;
         
      }
         
    // interrupt_times=0;//����ж�λ
  }
}



void USART1_IRQHandler(void)                	//����1�жϷ������
{   										//������Ϣ��־λ     

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
   // temp_pulse_num[0]=pulse_num;//������
    myitoa(pulse_num,temp_pulse_num);
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
