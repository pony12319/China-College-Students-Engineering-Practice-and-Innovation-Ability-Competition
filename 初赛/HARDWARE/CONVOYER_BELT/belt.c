#include "belt.h"

void belt_init()
{
	GPIO_InitTypeDef GPIO_InitStr;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStr;
	TIM_OCInitTypeDef TIM_OCInitStr;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);//ʹ��GPIOG��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);//ʹ��GPIOA���˿ڸ���ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//ʹ�ܶ�ʱ��
	
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;//�������
	GPIO_InitStr.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOG, &GPIO_InitStr);
	
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_AF_PP;//��������
	GPIO_InitStr.GPIO_Pin = GPIO_Pin_6| GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStr);
	
	TIM_TimeBaseInitStr.TIM_ClockDivision  = TIM_CKD_DIV1;//����Ƶ
	TIM_TimeBaseInitStr.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInitStr.TIM_Period = 100-1;
	TIM_TimeBaseInitStr.TIM_Prescaler = 72-1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStr);
	
	TIM_OCStructInit(&TIM_OCInitStr);
	TIM_OCInitStr.TIM_OCMode = TIM_OCMode_PWM1;//PWMģʽһ
	TIM_OCInitStr.TIM_OCPolarity = TIM_OCPolarity_High;//���Ը�
	TIM_OCInitStr.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStr.TIM_Pulse = 15;
	
	TIM_OC1Init(TIM3, &TIM_OCInitStr);
	TIM_OC2Init(TIM3, &TIM_OCInitStr);
	
	//����PG3 PG4����Ϊ�ߵ�ƽ ˳ʱ��ת��
	GPIO_SetBits(GPIOG, GPIO_Pin_3);
	GPIO_ResetBits(GPIOG, GPIO_Pin_4);
	
	TIM_Cmd(TIM3,ENABLE);        //ʹ��TIM3
}

void stop_belt()
{
	TIM_SetCompare1(TIM3,0);
	TIM_SetCompare2(TIM3,0);
}
void start_belt()
{
	TIM_SetCompare2(TIM3,23);//���1
	
	TIM_SetCompare1(TIM3,30);//���2
}













