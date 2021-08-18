#include "includes.h"
#include "led.h"
#include "pwm.h"
#include "DMA.h"
 
extern u8 SendBuff[200];//发送缓冲区
extern u8 Rx_Buff[650];//接收缓冲区

struct V{
  //计量数据
  u16 UART1_ReceiveSize; //DMA接收数据的长度
  u16 recv_len; //发送数据实际长度
  u8  recv_times;//接收数据次数
  u8  interruput_times;//计数器中断次数 
  TIM_TypeDef * CNT_TIMx;//计数定时器
  TIM_TypeDef * PWM_TIMx;//PWM定时器

  //参数数据
  //u8 sd;//当前数据段数
  u16 pulse_remainder;//脉冲余数
  u8 pulse_offset[20];//每一段的起始位（偏移量）
  u32 pulse_num;//总脉冲段数，默认两段脉冲
  u32 output_port;//输出端子，默认为0
  u16 mode;//脉冲段模式
  u32 data[600];//脉冲段数据
  
};

extern struct V Volume;

void data_init(void)//数据初始化
{
  Volume.pulse_num=2;//两段脉冲
  
  Volume.CNT_TIMx=TIM9;
  Volume.PWM_TIMx=TIM10;//端子定时器指定
  Volume.pulse_offset[0]=1;
  Volume.pulse_offset[1]=6;
  //两段脉冲，Y0输出，速率100和10，个数为1000和100个，等待模式为脉冲发送完成模式和等待时间模式，使用时间寄存器T1，时间为500ms跳转默认下段一
  Volume.data[0] = 0x00000002;
  Volume.data[1] = 0x0000000A;
  Volume.data[2] = 0x00000064;
  Volume.data[3] = 0x00030001;
  Volume.data[4] = 0x00000000;
  Volume.data[5] = 0x00000000;
  Volume.data[6] = 0x00000002;
  Volume.data[7] = 0x00000014;
  Volume.data[8] = 0x00020000;
  Volume.data[9] = 0x00000BB8;
  Volume.data[10] = 0x00000001;
   
}

u32 My_Atoi(char *source) //字符串转整形,遇到第一个非数字停止
{
        
	int length = 0 ; //计算长度
         while(source[length] >= '0' && source[length] <= '9')
          length++;
	u32 temp_sum = 0;
	for(int tmp=0;tmp<length;tmp++) 
          temp_sum = (source[tmp]-'0')+ (temp_sum*10) ; //字符转数字并合并
	return temp_sum;
}

void My_Itoa (u16 num,char str[])//整型转字符串
{
        u8 offset=0;

        do{
            str[offset++]=num%10+'0';//取下一个数字
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


void Led_Status(u8 state)//LED 状态
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
bool Output_Check(u8 *recv_data)//输出端子有效性检测
{
  u8 data_len=strlen((char *)recv_data);
  if(data_len > 2 || data_len ==0)//长度检查
    return false;
  
    if(recv_data[0]!='Y' || (recv_data[1]<'0' || recv_data[1]>'3'))
      return false;
    
  return true;
}
*/
bool Number_Sum_Check(u8 *recv_data)//总脉冲段数有效性检测
{
  u8 *p = recv_data;
  u8 data_len=0;
  while(*p >='0' && *p <= '9')
  {
    data_len++;
    p++;
  }
  p = recv_data;        
  if(data_len > 2 || data_len == 0)//长度检查
    return false;
  
  u32 data=My_Atoi((char *)p);
  if(data > 20 || data == 0)
    return false;
  Volume.pulse_num = data;
  return true;
}


bool Speed_Check(u8 *recv_data)//脉冲数率有效性检测
{
  u8 * p = recv_data;
  u8 data_len=strlen((char *)p);
  if(data_len > 6 || data_len == 0)//长度检查
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

bool Number_Check(u8 *recv_data)//脉冲个数有效性检测
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

bool Mode_Check(u8 *recv_data)//脉冲模式有效性检测
{
  if('F' == *recv_data || 'T' == *recv_data || 'S' == *recv_data || 'A' == *recv_data || 'E' == *recv_data || 
      'X' == *recv_data )
    if('\0' == *(recv_data+1))
    return true;
  return false; 
}

bool Time_Register_Check(u8 *recv_data)//时间寄存器有效性检测
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

bool Signal_Register_Check(u8 *recv_data)//信号寄存器有效性检测
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

bool External_Signal_Check(u8 *recv_data)//外部信号端子有效性检测
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

bool Time_Check(u8 *recv_data)//时间有效性检测
{
  u8 *p=recv_data;
  
  u8 data_len=strlen((char *)p);
  if(data_len > 5 || data_len==0)//长度检查
    return false;
  u8 temp=0;
  for(;temp<data_len;temp++)
  {
    if(*p > '9' || *p < '0')//数字检查
      return false;
    p++;
  }
  u32 data=My_Atoi((char *)recv_data);
  if(data > 65535|| data == 0)//数值检查
    return false;
  
   return true; 
}

bool Section_Num_Check(u8 *recv_data)//跳转脉冲段序号有效性检测
{
  u8 * p = recv_data;
  u8 data_len=0;
  while(*p <= '9' && *p >= '0')
  {
    p++;
    data_len++;
  }
    p = recv_data;//重置位置
    
  if(data_len > 2 || data_len == 0)//长度检查
    return false;
  
  u8 temp=0;
  for(;temp<data_len;temp++)
  {
    if(*p > '9' || *p < '0')//数字检查
      return false;
    p++;
  }
  
  u32 data=My_Atoi((char *)recv_data);
  if(data > Volume.pulse_num)//数值检查
    return false;
  
   return true; 
}

 


void Output_Place(u32 data)//端子指定定时器初始化
{

  switch(data)
  {
    case 0:{//Y0输出
              PWM_TIM10_Configuration();
              CNT_TIM9_Configuration(TIM_TS_ITR2); 
              
    }break;
    case 1:{//Y1输出
              PWM_TIM13_Configuration();
              CNT_TIM12_Configuration(TIM_TS_ITR2);
        
    }break;
    case 2:{//Y2输出
              PWM_TIM11_Configuration();
              CNT_TIM9_Configuration(TIM_TS_ITR3);
     
    }break;
    case 3:{//Y3输出
             PWM_TIM14_Configuration();
             CNT_TIM12_Configuration(TIM_TS_ITR3);
          
    }break;   
  
  }
  
          switch(data)//根据端子配置定时器
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
  
  
}

void Frequency_Select(u32 *PWM_CK_CNT,u16 *PWM_PRESCALER,TIM_TypeDef * PWM_TIMx,u32 frequency,u32 port)//根据脉冲频率设置寄存器值
{

   if(port == 0 || port == 2)
   {
      if(frequency > 1000)
     {
          if(frequency > 100000)
          {
              *PWM_CK_CNT =4000000 ;               
              *PWM_PRESCALER = (168000000/4000000 - 1);  //预分频值
               TIM_PrescalerConfig(PWM_TIMx, *PWM_PRESCALER, TIM_PSCReloadMode_Immediate);//立即更新预分频参数
              
           }else
          {
              *PWM_CK_CNT =1000000 ;               
              *PWM_PRESCALER = (168000000/1000000 - 1);  //预分频值
               TIM_PrescalerConfig(PWM_TIMx, *PWM_PRESCALER, TIM_PSCReloadMode_Immediate);//立即更新预分频参数
           }
                 
      }else
      {             
              *PWM_CK_CNT =10000 ;               
              *PWM_PRESCALER = (168000000/10000 - 1);  //预分频值
               TIM_PrescalerConfig(PWM_TIMx, *PWM_PRESCALER, TIM_PSCReloadMode_Immediate);//立即更新预分频参数
      }
      
   }else
   {
      if(frequency > 1000)
     {
          if(frequency > 100000)
          {
              *PWM_CK_CNT =4000000 ;               
              *PWM_PRESCALER = (84000000/4000000 - 1);  //预分频值
               TIM_PrescalerConfig(PWM_TIMx, *PWM_PRESCALER, TIM_PSCReloadMode_Immediate);//立即更新预分频参数
           }else
          {
              *PWM_CK_CNT =1000000 ;               
              *PWM_PRESCALER = (84000000/1000000 - 1);  //预分频值
               TIM_PrescalerConfig(PWM_TIMx, *PWM_PRESCALER, TIM_PSCReloadMode_Immediate);//立即更新预分频参数
           }
                 
      }else
      {             
              *PWM_CK_CNT =10000 ;               
              *PWM_PRESCALER = (84000000/10000 - 1);  //预分频值
               TIM_PrescalerConfig(PWM_TIMx, *PWM_PRESCALER, TIM_PSCReloadMode_Immediate);//立即更新预分频参数
      }
   
   }

}

void Print_Mode_Switch(u8 * send_data)//数据打印
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

bool Data_Head_Check(u8 *p)//数据头
{
  //数据头
  if(Rx_Buff[0] == 'H')
  {
    p++;
    if(Rx_Buff[1] == '-')
    {
      p++;
      if(Rx_Buff[2] == 'Y')
      {
        p++;
        if(Rx_Buff[3] >= '0' && Rx_Buff[3] < '4')
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

u8 Pulse_Data(u8 *p)//数据段错误判断，正确返回0，错误返回非0值(1:速率错误 2：个数错误 3：模式错误 4：时间错误 5：信号端子错误 6：跳转数错误 7：其他错误)
{
  if(!Speed_Check(p))//速率判断
    return 1;
  
  else
  {
    p=p+strlen((const char *)p)+1;
    //while(*p)
    if(!Number_Check(p))//个数判断
    return 2;
    
    else
    {
      p=p+strlen((const char *)p)+1;
      if(!Mode_Check(p))//模式判断
        return 3;
      else
      {
        switch(*p)
        {
          case 'F':{
            p=p+strlen((const char *)p)+1;
            if(!Section_Num_Check(p))//跳转数
              return 6;
            else 
            {
              while(*p <= '9' && *p >= '0')
                p++;
              if(*p == '\r' && *(p+1) == '\n')
              {
                p+=2;
                return Pulse_Data(p);//跑下一段
              }else
              {
                if(*p == 0)//判断结束
                  return 0;
                else
                  return 7;
              }               
               
            }
             
          };break;
          
          
        case 'T':{
            p=p+strlen((const char *)p)+1;
            if(!Time_Check(p))//时间判断
              return 4;
            else
            {
             p=p+strlen((const char *)p)+1 ;
              if(!Section_Num_Check(p))//跳转数
                return 6;
              else 
              {
                while(*p <= '9' && *p >= '0')
                  p++;
                if(*p == '\r' && *(p+1) == '\n')
                {
                  p+=2;
                  return Pulse_Data(p);//跑下一段
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
            if(!External_Signal_Check(p))//信号端子判断
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
                  return Pulse_Data(p);//跑下一段
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
                  return Pulse_Data(p);//跑下一段
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
            if(!External_Signal_Check(p))//信号端子判断
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
                  return Pulse_Data(p);//跑下一段
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
            if(!External_Signal_Check(p))//信号端子判断
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
                  return Pulse_Data(p);//跑下一段
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

u8 Data_Format_Check(u8 *p)//数据格式判断
{
  u16 temp = 0;
  if(!Data_Head_Check(p))//数据头判断
    return 8;
  
  
  while(*p != '\n')
    p++;
  p++;
  
  u16 data_len = strlen((const char *) p);//数据长度
  u8 * str=p;
  for(;temp<data_len;temp++)//数据分隔符替换为0
  {
    if(*str == '-')
      *str = 0;
    str++;
  }
  u8 err=Pulse_Data(p);
  if(err)//脉冲段判断
  {
    return err;
  }
     
  return 0;
}

bool Data_Length_Check(u8 *p)//数据长度判断
{
 p+=5;//移动到总段数位置
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

u8 Data_Check(void)//数据帧校验,错误返回错误号，正确返回0
{
  u16 len =strlen((char const*)Rx_Buff);//接收数据长度
  if(len < 15 || len > 607)//长度
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

void Data_Pulse_Save(u8 *p)//脉冲段数据保存
{
  u8 sd=0;
  Volume.pulse_offset[sd]=1;
  while(sd < Volume.pulse_num)
  {
    Volume.data[Volume.pulse_offset[sd]] = My_Atoi((char*)p);//脉冲速率
    p=p+strlen((const char *)p)+1;
    Volume.data[Volume.pulse_offset[sd]+1] = My_Atoi((char*)p);//脉冲个数
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
           Volume.data[Volume.pulse_offset[sd]+3] = My_Atoi((char*)p);//时间
          while(*p)
            p++;
          p++;
          Volume.data[Volume.pulse_offset[sd]+4] = My_Atoi((char*)p);      
        };break;
        case 'S':{
        p+=3;
           Volume.data[Volume.pulse_offset[sd]+2] = (3<<16)+My_Atoi((char*)p);
           Volume.data[Volume.pulse_offset[sd]+3] = 0;
          while(*p)
            p++;
          p++;
          Volume.data[Volume.pulse_offset[sd]+4] = My_Atoi((char*)p);    
        };break;
        case 'A':{
        p+=2;
           Volume.data[Volume.pulse_offset[sd]+2] = 4<<16;
           Volume.data[Volume.pulse_offset[sd]+3] = My_Atoi((char*)p);//时间
          while(*p)
            p++;
          p++;
          Volume.data[Volume.pulse_offset[sd]+4] = My_Atoi((char*)p);      
        };break;
        case 'E':{
        p+=3;
           Volume.data[Volume.pulse_offset[sd]+2] = (3<<16)+My_Atoi((char*)p);
           Volume.data[Volume.pulse_offset[sd]+3] = 0;
          while(*p)
            p++;
          p++;
          Volume.data[Volume.pulse_offset[sd]+4] = My_Atoi((char*)p);
        };break;
        case 'X':{
         p+=3;
           Volume.data[Volume.pulse_offset[sd]+2] = (3<<16)+My_Atoi((char*)p);
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


void Data_Save(void)//数据保存
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

void Err_Print(u8 err ,u8 *message)//错误打印
{
  
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
}

void Pluse_Number(u8 sd)//根据个数设置中断次数
{
                  if(Volume.data[Volume.pulse_offset[sd]+1] > 65535)//判断脉冲个数
                  {
                    Volume.interruput_times = Volume.data[Volume.pulse_offset[sd]+1] / 65535;
                    Volume.pulse_remainder = Volume.data[Volume.pulse_offset[sd]+1] % 65535;
                  }else
                  {
                    Volume.pulse_remainder = Volume.data[Volume.pulse_offset[sd]+1];
                  }
                               
                  if(Volume.interruput_times > 0)
                  {
                    Pulse_Output_Number(65535,Volume.CNT_TIMx);
                  }     
                  else
                  {
                    Pulse_Output_Number(Volume.pulse_remainder,Volume.CNT_TIMx);
                  }
}
