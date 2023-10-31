#include "compressor.h"
void Compressor_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStr;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);//使能GPIOF的时钟--->压缩机
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStr.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15; //PF15--->CW  PF14--->CCW
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStr);
	
	
	GPIO_ResetBits(GPIOF, GPIO_Pin_14);
	GPIO_ResetBits(GPIOF, GPIO_Pin_15);
	delay_ms(10);
	GPIO_SetBits(GPIOF, GPIO_Pin_14);

}

void Compressor_Stop(void)
{
	GPIO_ResetBits(GPIOF, GPIO_Pin_14);
	GPIO_ResetBits(GPIOF, GPIO_Pin_15);
	//GPIO_SetBits(GPIOG, GPIO_Pin_14);
	//GPIO_SetBits(GPIOG, GPIO_Pin_15);
	
}


void Compressor_Forward(void)
{
	GPIO_ResetBits(GPIOF, GPIO_Pin_14);
	GPIO_ResetBits(GPIOF, GPIO_Pin_15);
	delay_ms(10);
	GPIO_SetBits(GPIOF, GPIO_Pin_15);

}
void Compressor_Back(void)
{
	GPIO_ResetBits(GPIOF, GPIO_Pin_14);
	GPIO_ResetBits(GPIOF, GPIO_Pin_15);
	delay_ms(10);
	GPIO_SetBits(GPIOF, GPIO_Pin_14);
	
}

void Compressor_Press(void)
{
	int i;
	Compressor_Forward();
	for(i=0;i<8;i++)
	delay_ms(1000);
	Compressor_Back();
	for(i=0;i<8;i++)
	delay_ms(1000);
	Compressor_Stop();
	
}



