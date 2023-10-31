#include "belt.h"

void belt_init()
{
	GPIO_InitTypeDef GPIO_InitStr;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStr;
	TIM_OCInitTypeDef TIM_OCInitStr;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);//使能GPIOG的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);//使能GPIOA、端口复用时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//使能定时器
	
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStr.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOG, &GPIO_InitStr);
	
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽
	GPIO_InitStr.GPIO_Pin = GPIO_Pin_6| GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStr);
	
	TIM_TimeBaseInitStr.TIM_ClockDivision  = TIM_CKD_DIV1;//不分频
	TIM_TimeBaseInitStr.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStr.TIM_Period = 100-1;
	TIM_TimeBaseInitStr.TIM_Prescaler = 72-1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStr);
	
	TIM_OCStructInit(&TIM_OCInitStr);
	TIM_OCInitStr.TIM_OCMode = TIM_OCMode_PWM1;//PWM模式一
	TIM_OCInitStr.TIM_OCPolarity = TIM_OCPolarity_High;//极性高
	TIM_OCInitStr.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStr.TIM_Pulse = 15;
	
	TIM_OC1Init(TIM3, &TIM_OCInitStr);
	TIM_OC2Init(TIM3, &TIM_OCInitStr);
	
	//设置PG3 PG4引脚为高电平 顺时针转动
	GPIO_SetBits(GPIOG, GPIO_Pin_3);
	GPIO_ResetBits(GPIOG, GPIO_Pin_4);
	
	TIM_Cmd(TIM3,ENABLE);        //使能TIM3
}

void stop_belt()
{
	TIM_SetCompare1(TIM3,0);
	TIM_SetCompare2(TIM3,0);
}
void start_belt()
{
	TIM_SetCompare2(TIM3,23);//电机1
	
	TIM_SetCompare1(TIM3,30);//电机2
}













