#include "vl53l0x_it.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK MiniV3 STM32������
//VL53L0X-�жϲ���ģʽ ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/7/1
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

//�����޾���ֵ ��λ:mm
#define Thresh_Low   800 //60  ʵ�ʾ������С�����޻��ߴ������޲Żᴥ���ж�
#define Thresh_High 1000 //150

//�ж�ģʽ�����ṹ��
typedef struct 
{
	const int VL53L0X_Mode;//ģʽ
	uint32_t ThreshLow;    //����ֵ
	uint32_t ThreshHigh;   //����ֵ
}AlrmMode_t; 

AlrmMode_t AlarmModes ={
	
     VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_OUT,// value < thresh_low OR value > thresh_high
	 Thresh_Low<<16,
	 Thresh_High<<16
};

//�ж����ó�ʼ��
static void exti_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);
	EXTI_InitStructure.EXTI_Line = EXTI_Line4; 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			    //ʹ�ܰ���WK_UP���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;		    //�����ȼ�3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				    //ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
	
}

//������־λ alarm_flag 
//1:�о���
//0����
u8 alarm_flag=0;

//�ⲿ�жϷ�����
void EXTI4_IRQHandler(void)
{
	alarm_flag=1;//��־
	EXTI_ClearITPendingBit(EXTI_Line4);  //���LINE4�ϵ��жϱ�־λ 
}

extern uint8_t AjustOK;
extern mode_data Mode_data[];

//vl53l0x�жϲ���ģʽ����
//dev:�豸I2C�����ṹ��
//mode: 0:Ĭ��;1:�߾���;2:������;3:����
void vl53l0x_interrupt_start(VL53L0X_Dev_t *dev,uint8_t mode)
{
	 uint8_t VhvSettings;
	 uint8_t PhaseCal;
	 uint32_t refSpadCount;
	 uint8_t isApertureSpads;
	 //VL53L0X_RangingMeasurementData_t RangingMeasurementData;
	 //static char buf[VL53L0X_MAX_STRING_LENGTH];//����ģʽ�ַ����ַ�������
	 VL53L0X_Error status=VL53L0X_ERROR_NONE;//����״̬
	
	
	 exti_init();//�жϳ�ʼ��
	 //mode_string(mode,buf);//��ʾ��ǰ���õ�ģʽ
   //sprintf((char*)buf,"Thresh Low:  %d mm ",Thresh_Low);	
	 //sprintf((char*)buf,"Thresh High: %d mm",Thresh_High);	
	
	 vl53l0x_reset(dev);//��λvl53l0x(Ƶ���л�����ģʽ���׵��²ɼ��������ݲ�׼���������һ����)
	 status = VL53L0X_StaticInit(dev);
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	
	 	status = VL53L0X_PerformRefCalibration(dev, &VhvSettings, &PhaseCal);//Ref�ο�У׼
		if(status!=VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
		status = VL53L0X_PerformRefSpadManagement(dev, &refSpadCount, &isApertureSpads);//ִ�вο�SPAD����
		if(status!=VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);		 
	 
	 status = VL53L0X_SetDeviceMode(dev,VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);//ʹ����������ģʽ
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 delay_ms(2);
	 status = VL53L0X_SetInterMeasurementPeriodMilliSeconds(dev,Mode_data[mode].timingBudget);//�����ڲ����ڲ���ʱ��
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 delay_ms(2);
	 status = VL53L0X_SetLimitCheckEnable(dev,VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,1);//ʹ��SIGMA��Χ���
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 delay_ms(2);
	 status = VL53L0X_SetLimitCheckEnable(dev,VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,1);//ʹ���ź����ʷ�Χ���
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 delay_ms(2);
	 status = VL53L0X_SetLimitCheckValue(dev,VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,Mode_data[mode].sigmaLimit);//�趨SIGMA��Χ
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 delay_ms(2); 
	 status = VL53L0X_SetLimitCheckValue(dev,VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,Mode_data[mode].signalLimit);//�趨�ź����ʷ�Χ��Χ
	 if(status!=VL53L0X_ERROR_NONE) goto error; 
	 delay_ms(2);
	 status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(dev,Mode_data[mode].timingBudget);//�趨��������ʱ��
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 delay_ms(2); 
	 status = VL53L0X_SetVcselPulsePeriod(dev, VL53L0X_VCSEL_PERIOD_PRE_RANGE, Mode_data[mode].preRangeVcselPeriod);//�趨VCSEL��������
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 delay_ms(2);
	 status = VL53L0X_SetVcselPulsePeriod(dev, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, Mode_data[mode].finalRangeVcselPeriod);//�趨VCSEL�������ڷ�Χ
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 delay_ms(2);
	 status = VL53L0X_StopMeasurement(dev);//ֹͣ����
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 delay_ms(2);
	 status = VL53L0X_SetInterruptThresholds(dev,VL53L0X_DEVICEMODE_CONTINUOUS_RANGING,AlarmModes.ThreshLow, AlarmModes.ThreshHigh);//�趨�����ж��ϡ�����ֵ
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 delay_ms(2);
	 status = VL53L0X_SetGpioConfig(dev,0,VL53L0X_DEVICEMODE_CONTINUOUS_RANGING,AlarmModes.VL53L0X_Mode,VL53L0X_INTERRUPTPOLARITY_LOW);//�趨�����ж�ģʽ �½���
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 delay_ms(2);
	 status = VL53L0X_ClearInterruptMask(dev,0);//���VL53L0X�жϱ�־λ
	 
	 error://������Ϣ
	 if(status!=VL53L0X_ERROR_NONE)
	 {
		print_pal_error(status);
		return ;
	 }

	 alarm_flag = 0;
	 VL53L0X_StartMeasurement(dev);//��������
	 
	 printf("Start Measurement!\r\n");
	 
}

/*
//vl53l0x�жϲ���ģʽ����
//dev:�豸I2C�����ṹ��
void vl53l0x_interrupt_test(VL53L0X_Dev_t *dev)
{
	u8 mode=0;
	
	vl53l0x_interrupt_start(dev,mode);
}
*/


