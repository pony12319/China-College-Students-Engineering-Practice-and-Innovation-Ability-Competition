#ifndef __SERVO_H
#define __SERVO_H  
#include "stm32f10x.h"

void My_TIM4_Init(u16 arr,u16 psc);
void Dump(void);
void Servo_SetAngle(float Angle);
                 
#endif

