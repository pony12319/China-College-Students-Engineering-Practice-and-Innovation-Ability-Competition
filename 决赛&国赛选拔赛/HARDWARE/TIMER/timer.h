#ifndef __TIMER_H
#define __TIMER_H	 
#include "sys.h"
	
extern int maxtime;
	
void TIM6_Init(void);
void TIM6_NVIC_Config();
	
#endif
