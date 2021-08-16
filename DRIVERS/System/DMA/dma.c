#include "dma.h"																	   	  
#include "delay.h"


void MYDMASEND_Config(u32 par,u32 mar,u16 ndtr)
{ 
 
	DMA_InitTypeDef  DMA_InitStructure;

        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2时钟使能 
        DMA_DeInit(DMA2_Stream7);//复位所有寄存器
	
	while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE);//等待DMA可配置 
	
  /* 配置 DMA Stream */
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
  DMA_InitStructure.DMA_PeripheralBaseAddr = par;//DMA外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr = mar;//DMA 存储器0地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//存储器到外设模式
  DMA_InitStructure.DMA_BufferSize = ndtr;//数据传输量 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//中等优先级
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
  DMA_Init(DMA2_Stream7, &DMA_InitStructure);//初始化DMA Stream

  USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);//打开串口的DMA模式接收

} 


void MYDMAREC_Config(u32 par,u32 mar,u16 ndtr)
{
    DMA_InitTypeDef DMA_InitStructure;
    //tUART1_Rx.dwUART1RxLen = 0;
    /* 1.使能DMA2时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    /* 2.配置使用DMA接收数据 */
    DMA_DeInit(DMA2_Stream2); 

    DMA_InitStructure.DMA_Channel             = DMA_Channel_4;               /* 配置DMA通道 */
    DMA_InitStructure.DMA_PeripheralBaseAddr  = par;   /* 源 */
    DMA_InitStructure.DMA_Memory0BaseAddr     = mar;             /* 目的 */
    DMA_InitStructure.DMA_DIR                 = DMA_DIR_PeripheralToMemory;    /* 方向 */
    DMA_InitStructure.DMA_BufferSize          = ndtr;                    /* 长度 */                  
    DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;    /* 外设地址是否自增 */
    DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;         /* 内存地址是否自增 */
    DMA_InitStructure.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_Byte;      /* 目的数据带宽 */
    DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;      /* 源数据宽度 */
    DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;              /* 单次传输模式/循环传输模式 */
    DMA_InitStructure.DMA_Priority            = DMA_Priority_VeryHigh;        /* DMA优先级 */
    DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Disable;          /* FIFO模式/直接模式 */
    DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_HalfFull;   /* FIFO大小 */
    DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;       /* 单次传输 */
    DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single;

    /* 3. 配置DMA */
    DMA_Init(DMA2_Stream2, &DMA_InitStructure);

    /* 4.由于接收不需要DMA中断，故不设置DMA中断 */

    /* 5.使能串口的DMA接收 */
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);

    /* 6. 由于接收不需要DMA中断，故不能配置DMA中断优先级 */

    /* 7.使能DMA */ 
   DMA_Cmd(DMA2_Stream2,ENABLE);
}

//开启一次DMA传输
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:数据传输量  
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
 
	DMA_Cmd(DMA_Streamx, DISABLE);                      //关闭DMA传输 
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}	//确保DMA可以被设置  
		
	DMA_SetCurrDataCounter(DMA_Streamx,ndtr);          //数据传输量  
 
	DMA_Cmd(DMA_Streamx, ENABLE);                      //开启DMA传输 
        
}	  

 

























