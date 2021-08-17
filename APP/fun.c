#include "includes.h"
#include "led.h"
#include "pwm.h"
#include "DMA.h"
 
extern u8 SendBuff[200];//���ͻ�����
extern u8 Rx_Buff[650];//���ջ�����

struct V{
  //��������
  u16 UART1_ReceiveSize; //DMA�������ݵĳ���
  u16 recv_len; //��������ʵ�ʳ���
  u8  recv_times;//�������ݴ���
  u8  interruput_times;//�������жϴ��� 
  TIM_TypeDef * CNT_TIMx;//������ʱ��
  TIM_TypeDef * PWM_TIMx;//PWM��ʱ��

  //��������
  u8 sd;//��ǰ���ݶ���
  u16 pulse_remainder;//��������
  u8 pulse_offset[20];//ÿһ�ε���ʼλ��ƫ������
  u32 pulse_num;//�����������Ĭ����������
  u32 output_port;//������ӣ�Ĭ��Ϊ0
  u16 mode;//�����ģʽ
  u32 data[600];//���������
  
};

extern struct V Volume;


u32 My_Atoi(char *source) //�ַ���ת����,������һ��������ֹͣ
{
        
	int length = 0 ; //���㳤��
         while(source[length] >= '0' && source[length] <= '9')
          length++;
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
/*
bool Output_Check(u8 *recv_data)//���������Ч�Լ��
{
  u8 data_len=strlen((char *)recv_data);
  if(data_len > 2 || data_len ==0)//���ȼ��
    return false;
  
    if(recv_data[0]!='Y' || (recv_data[1]<'0' || recv_data[1]>'3'))
      return false;
    
  return true;
}
*/
bool Number_Sum_Check(u8 *recv_data)//�����������Ч�Լ��
{
  u8 *p = recv_data;
  u8 data_len=0;
  while(*p >='0' && *p <= '9')
  {
    data_len++;
    p++;
  }
  p = recv_data;        
  if(data_len > 2 || data_len == 0)//���ȼ��
    return false;
  
  u32 data=My_Atoi((char *)p);
  if(data > 20 || data == 0)
    return false;
  Volume.pulse_num = data;
  return true;
}


bool Speed_Check(u8 *recv_data)//����������Ч�Լ��
{
  u8 * p = recv_data;
  u8 data_len=strlen((char *)p);
  if(data_len > 6 || data_len == 0)//���ȼ��
    return false;
  u8 temp;
  for(temp=0;temp<data_len;temp++)
  {
    if(*p> '9' || *p <'0')
      return false;
    p++;
  }
  u32 data=My_Atoi((char *)recv_data);
  if(data > 200000 || data == 0)
    return false;
  return true;
}

bool Number_Check(u8 *recv_data)//���������Ч�Լ��
{
  u8 *p = recv_data;
  u8 temp=0;
  u8 data_len=strlen((char *)p);
  if(data_len>10 || data_len==0)
    return false;
  if(data_len ==10 && *p > '2')
    return false;
  
  for(;temp<data_len;temp++)
  {
    if(*p > '9' || *p < '0')
      return false;
    p++;
  }
  
  
  u32 data=My_Atoi((char *)recv_data);
  if(data > 2147483647 || data == 0)
    return false;
  return true;
  
}

bool Mode_Check(u8 *recv_data)//����ģʽ��Ч�Լ��
{
  if('F' == *recv_data || 'T' == *recv_data || 'S' == *recv_data || 'A' == *recv_data || 'E' == *recv_data || 
      'X' == *recv_data )
    if('\0' == *(recv_data+1))
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
  u8 *p=recv_data;
  if(*p != 'W')
    return false;
  u8 data_len=strlen((char *)recv_data);
  if(data_len > 2 || data_len == 0)
    return false;
    p++;
   u32 data=My_Atoi((char *)recv_data);
   if(data > 4)
     return false;
   if(data == 0 && *p==0)
      return false;
   return true;
}

bool Time_Check(u8 *recv_data)//ʱ����Ч�Լ��
{
  u8 *p=recv_data;
  
  u8 data_len=strlen((char *)p);
  if(data_len > 5 || data_len==0)//���ȼ��
    return false;
  u8 temp=0;
  for(;temp<data_len;temp++)
  {
    if(*p > '9' || *p < '0')//���ּ��
      return false;
    p++;
  }
  u32 data=My_Atoi((char *)recv_data);
  if(data > 65535|| data == 0)//��ֵ���
    return false;
  
   return true; 
}

bool Section_Num_Check(u8 *recv_data)//��ת����������Ч�Լ��
{
  u8 * p = recv_data;
  u8 data_len=0;
  while(*p <= '9' && *p >= '0')
  {
    p++;
    data_len++;
  }
    p = recv_data;//����λ��
    
  if(data_len > 2 || data_len == 0)//���ȼ��
    return false;
  
  u8 temp=0;
  for(;temp<data_len;temp++)
  {
    if(*p > '9' || *p < '0')//���ּ��
      return false;
    p++;
  }
  
  u32 data=My_Atoi((char *)recv_data);
  if(data > Volume.pulse_num)//��ֵ���
    return false;
  
   return true; 
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

void Frequency_Select(u32 *PWM_CK_CNT,u16 *PWM_PRESCALER,TIM_TypeDef * PWM_TIMx,u32 frequency,u32 port)//��������Ƶ�����üĴ���ֵ
{

   if(port == 0 || port == 2)
   {
      if(frequency > 1000)
     {
          if(frequency > 100000)
          {
              *PWM_CK_CNT =4000000 ;               
              *PWM_PRESCALER = (168000000/4000000 - 1);  //Ԥ��Ƶֵ
               TIM_PrescalerConfig(PWM_TIMx, *PWM_PRESCALER, TIM_PSCReloadMode_Immediate);//��������Ԥ��Ƶ����
              
           }else
          {
              *PWM_CK_CNT =1000000 ;               
              *PWM_PRESCALER = (168000000/1000000 - 1);  //Ԥ��Ƶֵ
               TIM_PrescalerConfig(PWM_TIMx, *PWM_PRESCALER, TIM_PSCReloadMode_Immediate);//��������Ԥ��Ƶ����
           }
                 
      }else
      {             
              *PWM_CK_CNT =10000 ;               
              *PWM_PRESCALER = (168000000/10000 - 1);  //Ԥ��Ƶֵ
               TIM_PrescalerConfig(PWM_TIMx, *PWM_PRESCALER, TIM_PSCReloadMode_Immediate);//��������Ԥ��Ƶ����
      }
      
   }else
   {
      if(frequency > 1000)
     {
          if(frequency > 100000)
          {
              *PWM_CK_CNT =4000000 ;               
              *PWM_PRESCALER = (84000000/4000000 - 1);  //Ԥ��Ƶֵ
               TIM_PrescalerConfig(PWM_TIMx, *PWM_PRESCALER, TIM_PSCReloadMode_Immediate);//��������Ԥ��Ƶ����
           }else
          {
              *PWM_CK_CNT =1000000 ;               
              *PWM_PRESCALER = (84000000/1000000 - 1);  //Ԥ��Ƶֵ
               TIM_PrescalerConfig(PWM_TIMx, *PWM_PRESCALER, TIM_PSCReloadMode_Immediate);//��������Ԥ��Ƶ����
           }
                 
      }else
      {             
              *PWM_CK_CNT =10000 ;               
              *PWM_PRESCALER = (84000000/10000 - 1);  //Ԥ��Ƶֵ
               TIM_PrescalerConfig(PWM_TIMx, *PWM_PRESCALER, TIM_PSCReloadMode_Immediate);//��������Ԥ��Ƶ����
      }
   
   }

}

void Print_Mode_Switch(u8 * send_data)//���ݴ�ӡ
{
      memset(SendBuff,0,200);
      strcpy((char *)SendBuff,(const char*)send_data);
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

bool Data_Head_Check(u8 *p)//����ͷ
{
  //����ͷ
  if(Rx_Buff[0] == 'H')
  {
    p++;
    if(Rx_Buff[1] == '-')
    {
      p++;
      if(Rx_Buff[2] == 'Y')
      {
        p++;
        if(Rx_Buff[3] > '0' && Rx_Buff[3] < '4')
        {
          p++;
          if(Rx_Buff[1] == '-')
          {
             p++;
             if(Number_Sum_Check(p))
             {
              
              p++;
                if(*p == '\r' && *(p+1) == '\n')
                {                
                  return true;
                }               
                else 
                  return false;
             }else
               return false;
               
          }else
            return false;
           
        }else
          return false;
          
      }else
        return false;
        
    }else
       return false;     
  }          
  else
    return false;
}

u8 Pulse_Data(u8 *p)//���ݶδ����жϣ���ȷ����0�����󷵻ط�0ֵ(1:���ʴ��� 2���������� 3��ģʽ���� 4��ʱ����� 5���źŶ��Ӵ��� 6����ת������ 7����������)
{
  if(!Speed_Check(p))//�����ж�
    return 1;
  
  else
  {
    p=p+strlen((const char *)p)+1;
    //while(*p)
    if(!Number_Check(p))//�����ж�
    return 2;
    
    else
    {
      p=p+strlen((const char *)p)+1;
      if(!Mode_Check(p))//ģʽ�ж�
        return 3;
      else
      {
        switch(*p)
        {
          case 'F':{
            p=p+strlen((const char *)p)+1;
            if(!Section_Num_Check(p))//��ת��
              return 6;
            else 
            {
              while(*p <= '9' && *p >= '0')
                p++;
              if(*p == '\r' && *(p+1) == '\n')
              {
                p+=2;
                return Pulse_Data(p);//����һ��
              }else
              {
                if(*p == 0)//�жϽ���
                  return 0;
                else
                  return 7;
              }               
               
            }
             
          };break;
          
          
        case 'T':{
            p=p+strlen((const char *)p)+1;
            if(!Time_Check(p))//ʱ���ж�
              return 4;
            else
            {
             p=p+strlen((const char *)p)+1 ;
              if(!Section_Num_Check(p))//��ת��
                return 6;
              else 
              {
                while(*p <= '9' && *p >= '0')
                  p++;
                if(*p == '\r' && *(p+1) == '\n')
                {
                  p+=2;
                  return Pulse_Data(p);//����һ��
                }else
                 {
                    if(*p == 0)
                      return 0;
                    else
                      return 7;
                  }     
                 
               }
            }
            
          };break;
          
          case 'S':{
            p=p+strlen((const char *)p)+1;
            if(!External_Signal_Check(p))//�źŶ����ж�
              return 5;
            else
            {
             p=p+strlen((const char *)p)+1 ;
              if(!Section_Num_Check(p))
                return 6;
              else 
              {
                while(*p <= '9' && *p >= '0')
                  p++;
                if(*p == '\r' && *(p+1) == '\n')
                {
                  p+=2;
                  return Pulse_Data(p);//����һ��
                }else
                 {
                    if(*p == 0)
                      return 0;
                    else
                      return 7;
                  }                     
              }
            }
          };break;
          
          case 'A':{
            p=p+strlen((const char *)p)+1;
            if(!Time_Check(p))
              return 4;
            else
            {
             p=p+strlen((const char *)p)+1 ;
              if(!Section_Num_Check(p))
                return 6;
              else 
              {
                while(*p <= '9' && *p >= '0')
                  p++;
                if(*p == '\r' && *(p+1) == '\n')
                {
                  p+=2;
                  return Pulse_Data(p);//����һ��
                }else
                 {
                    if(*p == 0)
                      return 0;
                    else
                      return 7;
                  }     
                 
               }
            }
          };break;
          
          case 'E':{
            p=p+strlen((const char *)p)+1;
            if(!External_Signal_Check(p))//�źŶ����ж�
              return 5;
            else
            {
             p=p+strlen((const char *)p)+1 ;
              if(!Section_Num_Check(p))
                return 6;
              else 
              {
                while(*p <= '9' && *p >= '0')
                  p++;
                if(*p == '\r' && *(p+1) == '\n')
                {
                  p+=2;
                  return Pulse_Data(p);//����һ��
                }else
                 {
                    if(*p == 0)
                      return 0;
                    else
                      return 7;
                  }                     
              }
            }
          };break;
          
          case 'X':{
            p=p+strlen((const char *)p)+1;
            if(!External_Signal_Check(p))//�źŶ����ж�
              return 5;
            else
            {
             p=p+strlen((const char *)p)+1 ;
              if(!Section_Num_Check(p))
                return 6;
              else 
              {
                while(*p <= '9' && *p >= '0')
                  p++;
                if(*p == '\r' && *(p+1) == '\n')
                {
                  p+=2;
                  return Pulse_Data(p);//����һ��
                }else
                 {
                    if(*p == 0)
                      return 0;
                    else
                      return 7;
                  }                     
              }
            }
          };break;
         
        }
        
        
      }
    }
  
  }
  
  return 0;
}

u8 Data_Format_Check(u8 *p)//���ݸ�ʽ�ж�
{
  u16 temp = 0;
  if(!Data_Head_Check(p))//����ͷ�ж�
    return 8;
  
  
  while(*p != '\n')
    p++;
  p++;
  
  u16 data_len = strlen((const char *) p);//���ݳ���
  u8 * str=p;
  for(;temp<data_len;temp++)//���ݷָ����滻Ϊ0
  {
    if(*str == '-')
      *str = 0;
    str++;
  }
  u8 err=Pulse_Data(p);
  if(err)//������ж�
  {
    return err;
  }
     
  return 0;
}

bool Data_Length_Check(u8 *p)//���ݳ����ж�
{
 p+=5;//�ƶ����ܶ���λ��
 u32 data=My_Atoi((char *)p);
 u32 temp=0;
 while(!(*p==0 && *(p+1)==0))
 {
    if(*p == '\n')
      temp++;
    p++;
 }
if(temp != data)
  return false;
  return true;
}

u8 Data_Check(void)//����֡У��,���󷵻ش���ţ���ȷ����0
{
  u16 len =strlen((char const*)Rx_Buff);//�������ݳ���
  if(len < 15 || len > 607)//����
    return 9;
  
  u8 *p=Rx_Buff; 
  u8 state=Data_Format_Check(p);
 if(state)
 {
    return state;
 }

 if(!Data_Length_Check(p))
 return 9;

  return 0;
}

void Data_Pulse_Save(u8 *p)//��������ݱ���
{
  u8 sd=0;
  Volume.pulse_offset[sd]=1;
  while(sd < Volume.pulse_num)
  {
    Volume.data[Volume.pulse_offset[sd]] = My_Atoi((char*)p);//��������
    p=p+strlen((const char *)p)+1;
    Volume.data[Volume.pulse_offset[sd]+1] = My_Atoi((char*)p);//�������
    p=p+strlen((const char *)p)+1;
    switch(*p)
    {
        case 'F':{
          p+=2;
           Volume.data[Volume.pulse_offset[sd]+2] = 1<<16;
           Volume.data[Volume.pulse_offset[sd]+3] = 0;
           Volume.data[Volume.pulse_offset[sd]+4] = My_Atoi((char*)p);
        };break;
        case 'T':{
        p+=2;
           Volume.data[Volume.pulse_offset[sd]+2] = 2<<16;
           Volume.data[Volume.pulse_offset[sd]+3] = My_Atoi((char*)p);//ʱ��
          while(*p)
            p++;
          p++;
          Volume.data[Volume.pulse_offset[sd]+4] = My_Atoi((char*)p);      
        };break;
        case 'S':{
        p+=3;
           Volume.data[Volume.pulse_offset[sd]+2] = 3<<16+My_Atoi((char*)p);
           Volume.data[Volume.pulse_offset[sd]+3] = 0;
          while(*p)
            p++;
          p++;
          Volume.data[Volume.pulse_offset[sd]+4] = My_Atoi((char*)p);    
        };break;
        case 'A':{
        p+=2;
           Volume.data[Volume.pulse_offset[sd]+2] = 4<<16;
           Volume.data[Volume.pulse_offset[sd]+3] = My_Atoi((char*)p);//ʱ��
          while(*p)
            p++;
          p++;
          Volume.data[Volume.pulse_offset[sd]+4] = My_Atoi((char*)p);      
        };break;
        case 'E':{
        p+=3;
           Volume.data[Volume.pulse_offset[sd]+2] = 3<<16+My_Atoi((char*)p);
           Volume.data[Volume.pulse_offset[sd]+3] = 0;
          while(*p)
            p++;
          p++;
          Volume.data[Volume.pulse_offset[sd]+4] = My_Atoi((char*)p);
        };break;
        case 'X':{
         p+=3;
           Volume.data[Volume.pulse_offset[sd]+2] = 3<<16+My_Atoi((char*)p);
           Volume.data[Volume.pulse_offset[sd]+3] = 0;
          while(*p)
            p++;
          p++;
          Volume.data[Volume.pulse_offset[sd]+4] = My_Atoi((char*)p);
        };break;
    
    }
    while(*p <= '9' && *p >= '0')
    p++;
    if(*p== '\r')
    {
      p+=2;
      sd++;
      Volume.pulse_offset[sd]=1+5*sd;
    }else
   sd++;
      
  }
 
}


void Data_Save(void)//���ݱ���
{
  u8 *p = Rx_Buff;
  p+=3;
  u32 duanzi = *p-'0';
  p+=2;
  u32 pulse_num = My_Atoi((char*)p);
  Volume.data[0] = (duanzi<<16)+pulse_num;
  while(*p != '\n')
    p++;
  p++;
  Data_Pulse_Save(p);
  
}
