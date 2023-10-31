#ifndef __COMPRESSOR_H
#define __COMPRESSOR_H

#include "stm32f10x.h"
#include "delay.h"


void Compressor_Init(void);
void Compressor_Stop(void);
void Compressor_Forward(void);
void Compressor_Back(void);
void Compressor_Press(void);


#endif


