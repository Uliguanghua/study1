#include "dma.h"																	   	  
#include "delay.h"


void MYDMASEND_Config(u32 par,u32 mar,u16 ndtr)
{ 
 
	DMA_InitTypeDef  DMA_InitStructure;

        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2ʱ��ʹ�� 
        DMA_DeInit(DMA2_Stream7);//��λ���мĴ���
	
	while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE);//�ȴ�DMA������ 
	
  /* ���� DMA Stream */
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //ͨ��ѡ��
  DMA_InitStructure.DMA_PeripheralBaseAddr = par;//DMA�����ַ
  DMA_InitStructure.DMA_Memory0BaseAddr = mar;//DMA �洢��0��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//�洢��������ģʽ
  DMA_InitStructure.DMA_BufferSize = ndtr;//���ݴ����� 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݳ���:8λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//�洢�����ݳ���:8λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// ʹ����ͨģʽ 
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//�е����ȼ�
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
  DMA_Init(DMA2_Stream7, &DMA_InitStructure);//��ʼ��DMA Stream

  USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);//�򿪴��ڵ�DMAģʽ����

} 


void MYDMAREC_Config(u32 par,u32 mar,u16 ndtr)
{
    DMA_InitTypeDef DMA_InitStructure;
    //tUART1_Rx.dwUART1RxLen = 0;
    /* 1.ʹ��DMA2ʱ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    /* 2.����ʹ��DMA�������� */
    DMA_DeInit(DMA2_Stream2); 

    DMA_InitStructure.DMA_Channel             = DMA_Channel_4;               /* ����DMAͨ�� */
    DMA_InitStructure.DMA_PeripheralBaseAddr  = par;   /* Դ */
    DMA_InitStructure.DMA_Memory0BaseAddr     = mar;             /* Ŀ�� */
    DMA_InitStructure.DMA_DIR                 = DMA_DIR_PeripheralToMemory;    /* ���� */
    DMA_InitStructure.DMA_BufferSize          = ndtr;                    /* ���� */                  
    DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;    /* �����ַ�Ƿ����� */
    DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;         /* �ڴ��ַ�Ƿ����� */
    DMA_InitStructure.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_Byte;      /* Ŀ�����ݴ��� */
    DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;      /* Դ���ݿ�� */
    DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;              /* ���δ���ģʽ/ѭ������ģʽ */
    DMA_InitStructure.DMA_Priority            = DMA_Priority_VeryHigh;        /* DMA���ȼ� */
    DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Disable;          /* FIFOģʽ/ֱ��ģʽ */
    DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_HalfFull;   /* FIFO��С */
    DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;       /* ���δ��� */
    DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single;

    /* 3. ����DMA */
    DMA_Init(DMA2_Stream2, &DMA_InitStructure);

    /* 4.���ڽ��ղ���ҪDMA�жϣ��ʲ�����DMA�ж� */

    /* 5.ʹ�ܴ��ڵ�DMA���� */
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);

    /* 6. ���ڽ��ղ���ҪDMA�жϣ��ʲ�������DMA�ж����ȼ� */

    /* 7.ʹ��DMA */ 
    DMA_Cmd(DMA2_Stream2,ENABLE);
}

//����һ��DMA����
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:���ݴ�����  
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
 
	DMA_Cmd(DMA_Streamx, DISABLE);                      //�ر�DMA���� 
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}	//ȷ��DMA���Ա�����  
		
	DMA_SetCurrDataCounter(DMA_Streamx,ndtr);          //���ݴ�����  
 
	DMA_Cmd(DMA_Streamx, ENABLE);                      //����DMA���� 
        
}	  

 

























