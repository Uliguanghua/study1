#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"


void MYDMAREC_Config(u32 par,u32 mar,u16 ndtr);
void MYDMASEND_Config(u32 par,u32 mar,u16 ndtr);
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr);	//使能一次DMA传输		   
#endif






























