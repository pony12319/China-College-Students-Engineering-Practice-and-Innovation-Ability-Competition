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


//#ifndef __HX711_H
//#define __HX711_H
// 
//#include "sys.h"
// 
//#define HX711_SCK PGout(5)// PG5
//#define HX711_DOUT PGin(6)// PG6
// 
// 
//extern void Init_HX711pin(void);
//extern u32 HX711_Read(void);
//extern void Get_Maopi(void);
//extern void Get_Weight(void);
// 
//extern u32 HX711_Buffer;
//extern u32 Weight_Maopi;
//extern s32 Weight_Shiwu;
//extern u8 Flag_Error;
// 
//#endif
// 
