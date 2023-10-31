#ifndef __HX711_H
#define __HX711_H

#include "sys.h"

#define HX711_SCK  PGout(5)// PG5
#define HX711_DOUT PGin(6)// PG6


void Init_HX711pin(void);
u32 HX711_Read(void);
void Get_Maopi(void);
double Get_Weight(void);


#endif

