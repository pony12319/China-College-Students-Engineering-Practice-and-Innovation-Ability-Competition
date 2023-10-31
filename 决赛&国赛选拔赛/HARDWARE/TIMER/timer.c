#include "timer.h"

int maxtime = 0;
int time = 0;

//ʹ�ö�ʱ����ʱ���ܣ��޷�ʶ��ʱ���д���
//������ʱ��TIM6 
void TIM6_Init(void)
{
    //�����ṹ��
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    //�����ж���
	TIM6_NVIC_Config();
	//��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	//����psr��arr
	TIM_TimeBaseStructure.TIM_Prescaler = 49999;
	TIM_TimeBaseStructure.TIM_Period = 7199;
	//ʹ��TIM6�ĸ����ж�
	TIM_ITConfig(TIM6,  TIM_IT_Update, ENABLE);
    //�����־λ
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	//��ʼ��TIM6
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	//ʹ��TIM6
	TIM_Cmd(TIM6,ENABLE);
}

void TIM6_NVIC_Config()
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn ;	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
    NVIC_Init(&NVIC_InitStructure);
}



void TIM6_IRQHandler()
{
	//����������ж�
    //�����и�������ǣ�Ϊʲô�����ж��ˣ�����Ҫ�ж��ж��Ƿ�����
    //���� TIM���жϺ�EXTI��һ����TIM���ж��кܶ��࣬��ÿһ�����ʱ������뵽TIMx_IRQHandler
    //��������Ҫ�жϾ�������жϵ����ĸ��¼���
	if(TIM_GetITStatus(TIM6,TIM_IT_Update) != RESET)
	{
			maxtime += 1;

		//�����־λ
		TIM_ClearITPendingBit(TIM6,TIM_FLAG_Update);
	}
}










