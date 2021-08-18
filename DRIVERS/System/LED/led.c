#include "led.h"

	    
//LED IO初始化
void LED_Init(void)
{
 
        
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOH|RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOI,ENABLE); //使能GPIOF、B、E、H、G、I的时钟
      
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_8| GPIO_Pin_7|GPIO_Pin_9|GPIO_Pin_11;//F6、7、8、9、11
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  //上拉输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //高速GPIO
	GPIO_Init(GPIOF,&GPIO_InitStructure);
        GPIO_SetBits(GPIOF,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_11);
        
        
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;//G6、7
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  //上拉输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //高速GPIO
	GPIO_Init(GPIOG,&GPIO_InitStructure);
        GPIO_SetBits(GPIOG,GPIO_Pin_6|GPIO_Pin_7);
        
         GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;//H6、7、8、9
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  //上拉输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //高速GPIO
	GPIO_Init(GPIOH,&GPIO_InitStructure);
        GPIO_SetBits(GPIOH,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
        
        
         GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;//E4、5、6
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  //上拉输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //高速GPIO
	GPIO_Init(GPIOE,&GPIO_InitStructure);
        GPIO_SetBits(GPIOE,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);
        
        
        
        
        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//B4、5、6、7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
        
        
        
}