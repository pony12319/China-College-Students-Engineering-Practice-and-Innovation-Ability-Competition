/************************************************************************************
						
*************************************************************************************/
#include "HX711.h"
#include "delay.h"
#include "usart.h"
u32 HX711_Buffer;
u32 Weight_Maopi;
s32 Weight_Shiwu;
u8 Flag_Error = 0;

//У׼����
//��Ϊ��ͬ�Ĵ������������߲��Ǻ�һ�£���ˣ�ÿһ����������Ҫ�������������������ʹ����ֵ��׼ȷ��
//�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
//������Գ���������ƫСʱ����С����ֵ��
//��ֵ����ΪС��
#define GapValue 2535


void Init_HX711pin(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	 //ʹ��PG�˿�ʱ��

	//HX711_SCK PG5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOG, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOG
	
	//HX711_DOUT PG6
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
    GPIO_Init(GPIOG, &GPIO_InitStructure);  
	
	GPIO_SetBits(GPIOG,GPIO_Pin_5);					//��ʼ������Ϊ0
}



//****************************************************
//��ȡHX711
//****************************************************
u32 HX711_Read(void)	//����128
{
	unsigned long count; 
	unsigned char i; 
  	HX711_DOUT=1; 
	delay_us(1);
  	HX711_SCK=0; 
  	count=0; 
  	while(HX711_DOUT); 
  	for(i=0;i<24;i++)
	{ 
	  	HX711_SCK=1; 
	  	count=count<<1; 
		delay_us(1);
		HX711_SCK=0; 
	  	if(HX711_DOUT)
			count++; 
		delay_us(1);
	} 
 	HX711_SCK=1; 
    count=count^0x800000;//��25�������½�����ʱ��ת������
	delay_us(1);
	HX711_SCK=0;  
	return(count);
}

//****************************************************
//��ȡëƤ����
//****************************************************
void Get_Maopi(void)
{
	Weight_Maopi = HX711_Read();	
	//printf("Weight_Maopi = %d \n",Weight_Maopi);
} 

//****************************************************
//����
//****************************************************
double Get_Weight(void)
{
	double w;
	HX711_Buffer = HX711_Read();
	//printf("HX711_Buffer = %d \n",HX711_Buffer);
	if(HX711_Buffer >= Weight_Maopi)			
	{
		Weight_Shiwu = HX711_Buffer;
		Weight_Shiwu = Weight_Shiwu - Weight_Maopi;				//��ȡʵ���AD������ֵ��
	
		//Weight_Shiwu = (s32)((float)Weight_Shiwu/GapValue); 	//����ʵ���ʵ������
		w = ((float)Weight_Shiwu/GapValue);
																		//��Ϊ��ͬ�Ĵ������������߲�һ������ˣ�ÿһ����������Ҫ���������GapValue���������
																		//�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
																		//������Գ���������ƫСʱ����С����ֵ��
		
	}
   return w;
	
}


///************************************************************************************
//						
//*************************************************************************************/
//#include "HX711.h"
//#include "delay.h"
// 
//u32 HX711_Buffer;
//u32 Weight_Maopi;
//s32 Weight_Shiwu;
//u8 Flag_Error = 0;
// 
//float P=1;
//float P_;  //��Ӧ��ʽ�е�p'
//float X=0;
//float X_;  //X'
//float K=0;
//float Q=0.01;//����
//float R=0.2;  //R����ܴ󣬸�����Ԥ��ֵ����ô��������Ӧ�ͻ�ٶۣ���֮�෴
//float R=0.5;
//float distance=0;
//float distance1=0;
//float KLM(float Z)
//{
//  X_=X+0;
//  P_=P+Q;
//  K=P_/(P_+R);
//  X=X_+K*(Z-X_);
//  P=P_-K*P_;
//  return X;
//}
// 
// 
//У׼����
//��Ϊ��ͬ�Ĵ������������߲��Ǻ�һ�£���ˣ�ÿһ����������Ҫ�������������������ʹ����ֵ��׼ȷ��
//�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
//������Գ���������ƫСʱ����С����ֵ��
//��ֵ����ΪС��
//#define GapValue 106.5
// 
// 
//void Init_HX711pin(void)//��ʼ��
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	 //ʹ��PF�˿�ʱ��
// 
//	//HX711_SCK
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 // �˿�����
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
//	GPIO_Init(GPIOG, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB
//	
//	//HX711_DOUT
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
//    GPIO_Init(GPIOG, &GPIO_InitStructure);  
//	
//	GPIO_SetBits(GPIOG,GPIO_Pin_0);					//��ʼ������Ϊ0
//}
// 
// 
// 
//****************************************************
//��ȡHX711
//****************************************************
//u32 HX711_Read(void)	//����128
//{
//	unsigned long count; 
//	unsigned char i; 
//  	HX711_DOUT=1; 
//	delay_us(1);
//  	HX711_SCK=0; 
//  	count=0; 
//  	while(HX711_DOUT); 
//  	for(i=0;i<24;i++)
//	{ 
//	  	HX711_SCK=1; 
//	  	count=count<<1; 
//		delay_us(1);
//		HX711_SCK=0; 
//	  	if(HX711_DOUT)
//			count++; 
//		delay_us(1);
//	} 
// 	HX711_SCK=1; 
//    count=count^0x800000;//��25�������½�����ʱ��ת������
//	delay_us(1);
//	HX711_SCK=0;  
//	return(count);
//}
// 
//****************************************************
//��ȡëƤ����
//****************************************************
//void Get_Maopi(void)
//{
//	Weight_Maopi = HX711_Read();	
//} 
// 
//****************************************************
//����
//****************************************************
//void Get_Weight(void)
//{
//	HX711_Buffer = HX711_Read();
//	if(HX711_Buffer > Weight_Maopi)			
//	{
//		Weight_Shiwu = HX711_Buffer;
//		Weight_Shiwu = Weight_Shiwu - Weight_Maopi;				//��ȡʵ���AD������ֵ��
//	
//		Weight_Shiwu = (s32)((float)Weight_Shiwu/GapValue)-478; 	//����ʵ���ʵ������
//																		//��Ϊ��ͬ�Ĵ������������߲�һ������ˣ�ÿһ����������Ҫ���������GapValue���������
//																		//�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
//																		//������Գ���������ƫСʱ����С����ֵ��
//		Weight_Shiwu=KLM(Weight_Shiwu);
//	}
//	
//	//return Weight_Shiwu * 1.0;
//}


