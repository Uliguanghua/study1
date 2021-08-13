#include "includes.h"
#include "led.h"
#include "pwm.h"
 

u32 My_Atoi(char *source) //�ַ���ת����
{
	int length = strlen(source); //���㳤��
	u32 temp_sum = 0;
	for(int tmp=0;tmp<length;tmp++) 
          temp_sum = (source[tmp]-'0')+ (temp_sum*10) ; //�ַ�ת���ֲ��ϲ�
	return temp_sum;
}

void My_Itoa (u16 num,char str[])//����ת�ַ���
{
        u8 offset=0;

        do{
            str[offset++]=num%10+'0';//ȡ��һ������
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


void Led_Status(u8 state)//LED ״̬
{
  switch(state)
  {
  case 0:{STA_LED0=0;
          STA_LED1=1;
          STA_LED2=1;};break;
          
  case 1:{STA_LED0=1;
          STA_LED1=0;
          STA_LED2=1;};break;
          
  case 2:{STA_LED0=1;
          STA_LED1=1;
          STA_LED2=0;};break;
          
  default:{STA_LED0=1;
          STA_LED1=1;
          STA_LED2=1;};break;
  }
}

bool Output_Check(u8 *recv_data)//���������Ч�Լ��
{
  u8 data_len=strlen((char *)recv_data);
  if(data_len > 2 || data_len ==0)//���ȼ��
    return false;
  
    if(recv_data[0]!='Y' || (recv_data[1]<'0' || recv_data[1]>'3'))
      return false;
    
  return true;
}

bool Number_Sum_Check(u8 *recv_data)//�����������Ч�Լ��
{
  u8 data_len=strlen((char *)recv_data);
  if(data_len > 3)//���ȼ��
    return false;
  u8 temp;
  for(temp=0;temp<data_len;temp++)
  {
    if(recv_data[temp]>'9' || recv_data[temp]<'0')
      return false;
  }
  u32 data=My_Atoi((char *)recv_data);
  if(data > 100 || data == 0)
    return false;
  return true;
}


bool Speed_Check(u8 *recv_data)//����������Ч�Լ��
{
  u8 data_len=strlen((char *)recv_data);
  if(data_len > 6)//���ȼ��
    return false;
  u8 temp;
  for(temp=0;temp<data_len;temp++)
  {
    if(recv_data[temp]>'9' || recv_data[temp]<'0')
      return false;
  }
  u32 data=My_Atoi((char *)recv_data);
  if(data > 200000 || data == 0)
    return false;
  return true;
  
}

bool Number_Check(u8 *recv_data)//���������Ч�Լ��
{
  u8 temp;
  u8 data_len=strlen((char *)recv_data);
  if(data_len>10 || data_len==0)
    
  for(temp=0;temp<data_len;temp++)
  {
    if(recv_data[temp]>'9' || recv_data[temp]<'0' || recv_data[0]>'2')
      return false;
  }
  
  
  u32 data=My_Atoi((char *)recv_data);
  if(data > 2147483647 || data == 0)
    return false;
  return true;
  
}

bool Mode_Check(u8 *recv_data)//����ģʽ��Ч�Լ��
{
  
  if(0 == strcmp((char const *)recv_data,"wait_time") || 0 == strcmp((char const *)recv_data,"finish") || 0 == strcmp((char const *)recv_data,"wait_signal")
     || 0 == strcmp((char const *)recv_data,"act_time") || 0 == strcmp((char const *)recv_data,"ext_signal") || 0 == strcmp((char const *)recv_data,"ext_finish"))
    
    return true;
  return false;
  
}

bool Time_Register_Check(u8 *recv_data)//ʱ��Ĵ�����Ч�Լ��
{
  if(recv_data[0] != 'T')
    return false;
  u8 data_len=strlen((char *)recv_data);
  if(data_len > 4)
    return false;
  recv_data[0]=recv_data[1];
  recv_data[1]=recv_data[2];
  recv_data[2]=recv_data[3];
  recv_data[3]=recv_data[4];
   u32 data=My_Atoi((char *)recv_data);
   if(data > 100 || data == 0)
     return false;
   return true;
  
}

bool Signal_Register_Check(u8 *recv_data)//�źżĴ�����Ч�Լ��
{
  if(recv_data[0] != 'S')
    return false;
  u8 data_len=strlen((char *)recv_data);
  if(data_len > 4)
    return false;
  recv_data[0]=recv_data[1];
  recv_data[1]=recv_data[2];
  recv_data[2]=recv_data[3];
  recv_data[3]=recv_data[4];
   u32 data=My_Atoi((char *)recv_data);
   if(data > 100 || data == 0)
     return false;
   return true;
  
}

bool External_Signal_Check(u8 *recv_data)//�ⲿ�źŶ�����Ч�Լ��
{
  if(recv_data[0] != 'W')
    return false;
  u8 data_len=strlen((char *)recv_data);
  if(data_len > 2)
    return false;
  recv_data[0]=recv_data[1];
  recv_data[1]=recv_data[2];
   u32 data=My_Atoi((char *)recv_data);
   if(data > 4)
     return false;
   return true;
}

bool Time_Check(u8 *recv_data)//ʱ����Ч�Լ��
{
  u8 data_len=strlen((char *)recv_data);
  if(data_len > 5)//���ȼ��
    return false;
  u8 temp;
  for(temp=0;temp<data_len;temp++)
  {
    if(recv_data[temp]>'9' || recv_data[temp]<'0')//���ּ��
      return false;
  }
  u32 data=My_Atoi((char *)recv_data);
  if(data > 65535|| data == 0)//��ֵ���
    return false;
  
   return true; 
}

bool Section_Num_Check(u8 *recv_data,u16 sum)//��ת����������Ч�Լ��
{
  u8 data_len=strlen((char *)recv_data);
  if(data_len > 3 || data_len ==0)//���ȼ��
    return false;
  
  u8 temp;
  for(temp=0;temp<data_len;temp++)
  {
    if(recv_data[temp]>'9' || recv_data[temp]<'0')//���ּ��
      return false;
  }
  
  u32 data=My_Atoi((char *)recv_data);
  if(data > sum)//��ֵ���
    return false;
  
   return true; 

}

bool End_Check(u8 *recv_data)//����ָ����Ч�Լ��
{
 
    if(0 == strcmp((char const *)recv_data,"set_end"))

    return true;
  
   return false; 

}

void Output_Place(u32 data)//����ָ����ʱ����ʼ��
{

  switch(data)
  {
  case 0:{//Y0���
            PWM_TIM10_Configuration();
            CNT_TIM9_Configuration(TIM_TS_ITR2); 
            
  }break;
  case 1:{//Y1���
            PWM_TIM13_Configuration();
            CNT_TIM12_Configuration(TIM_TS_ITR2);
      
  }break;
  case 2:{//Y2���
            PWM_TIM11_Configuration();
            CNT_TIM9_Configuration(TIM_TS_ITR3);
   
  }break;
  case 3:{//Y3���
           PWM_TIM14_Configuration();
           CNT_TIM12_Configuration(TIM_TS_ITR3);
        
  }break;
  
  
  }
}




