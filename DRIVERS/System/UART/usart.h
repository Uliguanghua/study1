#ifndef __USART_H
#define __USART_H	
#include "stm32f4xx_conf.h"
#include "sys.h" 



#define USART_REC_LEN  			200  
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
extern char  USART_RX_BUF[USART_REC_LEN]; 
extern u16 USART_RX_STA;         		  	

//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
#endif
