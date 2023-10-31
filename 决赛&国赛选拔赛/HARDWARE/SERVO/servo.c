#include "servo.h"
#include "delay.h"

//PB6,PB9 TIM4��CH1��CH4
//PA1  ��TIM2 CH2
void My_TIM4_Init(u16 arr,u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;
     
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM2,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
     
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;//��������
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_9|GPIO_Pin_1 | GPIO_Pin_7 |GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
    TIM_TimeBaseInitStruct.TIM_Period=arr;//�Զ���װ��ֵ
    TIM_TimeBaseInitStruct.TIM_Prescaler=psc;//Ԥ��Ƶϵ��
			
    TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OCNPolarity=TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	
//    TIM_OC3Init(TIM4,&TIM_OCInitStruct);//ͨ��1
    TIM_OC2Init(TIM4,&TIM_OCInitStruct);//ͨ��2
	TIM_OC1Init(TIM4,&TIM_OCInitStruct);//ͨ��3
    TIM_OC4Init(TIM4,&TIM_OCInitStruct);//ͨ��4
	
	TIM_OC2Init(TIM2,&TIM_OCInitStruct);//ͨ��4
     
    TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);        //ʹ��Ԥװ�ؼĴ���
    TIM_OC2PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
	
    TIM_Cmd(TIM4,ENABLE);        //ʹ��TIM4
	TIM_Cmd(TIM2,ENABLE);        //ʹ��TIM4
         
}

void Servo_SetAngle(float Angle)
{
	TIM_SetCompare2(TIM2, Angle / 180 * 2000 + 500);
}

void Dump(void)
{
	Servo_SetAngle(0);
	delay_ms(1000);
	delay_ms(500);
	Servo_SetAngle(90);

}

