#include "InfrarePSwitch.h"
//PF7 
void Switch_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG, ENABLE);	 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_7);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_9);
}

//1 设定距离之外
//0 设定距离之内
int Switch_Check(void)
{
	return GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_7);
	
}

//1 有东西
//0 无东西
int IsAnything(void)
{
	return GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_9);
}


