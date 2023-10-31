#include "timer.h"

int maxtime = 0;
int time = 0;

//使用定时器计时功能，无法识别时进行处理
//基本定时器TIM6 
void TIM6_Init(void)
{
    //声明结构体
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    //配置中断组
	TIM6_NVIC_Config();
	//打开时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	//设置psr和arr
	TIM_TimeBaseStructure.TIM_Prescaler = 49999;
	TIM_TimeBaseStructure.TIM_Period = 7199;
	//使能TIM6的更新中断
	TIM_ITConfig(TIM6,  TIM_IT_Update, ENABLE);
    //清除标志位
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	//初始化TIM6
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	//使能TIM6
	TIM_Cmd(TIM6,ENABLE);
}

void TIM6_NVIC_Config()
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn ;	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
    NVIC_Init(&NVIC_InitStructure);
}



void TIM6_IRQHandler()
{
	//如果产生了中断
    //这里有个问题就是：为什么进到中断了，还需要判断中断是否发生。
    //答案是 TIM的中断和EXTI不一样，TIM的中断有很多类，而每一类产生时都会进入到TIMx_IRQHandler
    //所以我们要判断具体产生中断的是哪个事件。
	if(TIM_GetITStatus(TIM6,TIM_IT_Update) != RESET)
	{
			maxtime += 1;

		//清除标志位
		TIM_ClearITPendingBit(TIM6,TIM_FLAG_Update);
	}
}










