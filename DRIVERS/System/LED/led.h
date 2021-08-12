#ifndef _LED_H
#define _LED_H
#include "sys.h"



//LED端口定义
#define LED0 PFout(6)
#define LED1 PFout(8)
#define LED2 PFout(7)
#define LED3 PFout(9)
#define LED_interrupt PEout(4)

#define STA_LED0 PGout(7)
#define STA_LED1 PGout(6)
#define STA_LED2 PHout(9)
#define LED13 PHout(8)
#define LED14 PHout(7)
#define LED15 PHout(6)
#define LED16 PFout(11)

#define LED5 PEout(6)
#define LED6 PEout(5)


void LED_Init(void);  //led初始化
#endif
