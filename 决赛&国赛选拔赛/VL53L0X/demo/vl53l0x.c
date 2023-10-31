#include "vl53l0x.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK MiniV3 STM32������
//VL53L0X-���ܲ��� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/7/1
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

VL53L0X_Dev_t vl53l0x_dev0;//�豸I2C���ݲ���
VL53L0X_Dev_t vl53l0x_dev1;
VL53L0X_Dev_t vl53l0x_dev2;
VL53L0X_Dev_t vl53l0x_dev3;
u16 distancebuff[4];

VL53L0X_DeviceInfo_t vl53l0x_dev_info;//�豸ID�汾��Ϣ
uint8_t AjustOK=0;//У׼��־λ

//VL53L0X������ģʽ����
//0��Ĭ��;1:�߾���;2:������;3:����
mode_data Mode_data[]=
{
    {(FixPoint1616_t)(0.25*65536), 
	 (FixPoint1616_t)(18*65536),
	 33000,
	 14,
	 10},//Ĭ��
		
	{(FixPoint1616_t)(0.25*65536) ,
	 (FixPoint1616_t)(18*65536),
	 200000, 
	 14,
	 10},//�߾���
		
    {(FixPoint1616_t)(0.1*65536) ,
	 (FixPoint1616_t)(60*65536),
	 33000,
	 18,
	 14},//������
	
    {(FixPoint1616_t)(0.25*65536) ,
	 (FixPoint1616_t)(32*65536),
	 20000,
	 14,
	 10},//����
		
};

//API������Ϣ��ӡ
//Status�����鿴VL53L0X_Error�����Ķ���
void print_pal_error(VL53L0X_Error Status)
{
	
	char buf[VL53L0X_MAX_STRING_LENGTH];
	
	VL53L0X_GetPalErrorString(Status,buf);//����Status״̬��ȡ������Ϣ�ַ���
	
    printf("API Status: %i : %s\r\n",Status, buf);//��ӡ״̬�ʹ�����Ϣ
	
}

//ģʽ�ַ�����ʾ
//mode:0-Ĭ��;1-�߾���;2-������;3-����
void mode_string(u8 mode,char *buf)
{
	switch(mode)
	{
		case Default_Mode: strcpy(buf,"Default");        break;
		case HIGH_ACCURACY: strcpy(buf,"High Accuracy"); break;
		case LONG_RANGE: strcpy(buf,"Long Range");       break;
		case HIGH_SPEED: strcpy(buf,"High Speed");       break;
	}

}

//����VL53L0X�豸I2C��ַ
//dev:�豸I2C�����ṹ��
//newaddr:�豸��I2C��ַ
VL53L0X_Error vl53l0x_Addr_set(VL53L0X_Dev_t *dev,uint8_t newaddr)
{
	uint16_t Id;
	uint8_t FinalAddress;
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	u8 sta=0x00;
	
	FinalAddress = newaddr;
	
	if(FinalAddress==dev->I2cDevAddr)//���豸I2C��ַ��ɵ�ַһ��,ֱ���˳�
		return VL53L0X_ERROR_NONE;
	//�ڽ��е�һ���Ĵ�������֮ǰ����I2C��׼ģʽ(400Khz)
	Status = VL53L0X_WrByte(dev,0x88,0x00);
	if(Status!=VL53L0X_ERROR_NONE) 
	{
		sta=0x01;//����I2C��׼ģʽ����
		goto set_error;
	}
	//����ʹ��Ĭ�ϵ�0x52��ַ��ȡһ���Ĵ���
	Status = VL53L0X_RdWord(dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
	if(Status!=VL53L0X_ERROR_NONE) 
	{
		sta=0x02;//��ȡ�Ĵ�������
		goto set_error;
	}
	if(Id == 0xEEAA)
	{
		//�����豸�µ�I2C��ַ
		Status = VL53L0X_SetDeviceAddress(dev,FinalAddress);
		if(Status!=VL53L0X_ERROR_NONE) 
		{
			sta=0x03;//����I2C��ַ����
			goto set_error;
		}
		//�޸Ĳ����ṹ���I2C��ַ
		dev->I2cDevAddr = FinalAddress;
		//����µ�I2C��ַ��д�Ƿ�����
		Status = VL53L0X_RdWord(dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
		if(Status!=VL53L0X_ERROR_NONE) 
		{
			sta=0x04;//��I2C��ַ��д����
			goto set_error;
		}	
	}
	set_error:
	if(Status!=VL53L0X_ERROR_NONE)
	{
		print_pal_error(Status);//��ӡ������Ϣ
	}
	if(sta!=0)
	  printf("sta:0x%x\r\n",sta);
	return Status;
}

//vl53l0x��λ����
//dev:�豸I2C�����ṹ��
void vl53l0x_reset(VL53L0X_Dev_t *dev)
{
	uint8_t addr;
	addr = dev->I2cDevAddr;//�����豸ԭI2C��ַ
    VL53L0X_Xshut0=0;//ʧ��VL53L0X
	delay_ms(30);
	VL53L0X_Xshut0=1;//ʹ��VL53L0X,�ô��������ڹ���(I2C��ַ��ָ�Ĭ��0X52)
	delay_ms(30);	
	dev->I2cDevAddr=0x52;
	vl53l0x_Addr_set(dev,addr);//����VL53L0X������ԭ���ϵ�ǰԭI2C��ַ
	VL53L0X_DataInit(dev);	
}

VL53L0X_Error vl53l0x_initX(VL53L0X_Dev_t *dev,u8 vl53l0_x_id)
{
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;		
	VL53L0X_Dev_t *pMyDevice = dev;	
	
	pMyDevice->I2cDevAddr = VL53L0X_Addr;//I2C��ַ(�ϵ�Ĭ��0x52)
	pMyDevice->comms_type = 1;           //I2Cͨ��ģʽ
	pMyDevice->comms_speed_khz = 400;    //I2Cͨ������
	
	switch(vl53l0_x_id)
	  {
		case 0:  
		   VL53L0X_Xshut0=1;
   	       delay_ms(20);
		   vl53l0x_Addr_set(pMyDevice,0x54);//���õ�һ��VL53L0X������I2C��ַ
		   //vl53l0x_dev0.I2cDevAddr=0x54;
		   break;
		case 1:  		     
	       VL53L0X_Xshut1=1;
   	       delay_ms(20);
		   vl53l0x_Addr_set(pMyDevice,0x56);//���õ�һ��VL53L0X������I2C��ַ
		   //vl53l0x_dev1.I2cDevAddr=0x56;
		   break;
		case 2:  
			VL53L0X_Xshut2=1;
   	        delay_ms(20);
		    vl53l0x_Addr_set(pMyDevice,0x58);
		    //vl53l0x_dev2.I2cDevAddr=0x58;
			break;
		case 3:  
			VL53L0X_Xshut3=1;
   	        delay_ms(20);
		    vl53l0x_Addr_set(pMyDevice,0x5a);
		    //vl53l0x_dev3.I2cDevAddr=0x5a;
			break;
	  }
    if(Status!=VL53L0X_ERROR_NONE) goto error;
	Status = VL53L0X_DataInit(pMyDevice);//�豸��ʼ��
	if(Status!=VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	Status = VL53L0X_GetDeviceInfo(pMyDevice,&vl53l0x_dev_info);//��ȡ�豸ID��Ϣ
    if(Status!=VL53L0X_ERROR_NONE) goto error;
		
	  AjustOK=0;
	
	error:
	if(Status!=VL53L0X_ERROR_NONE)
	{
		print_pal_error(Status);//��ӡ������Ϣ
		return Status;
	}
}

//��ʼ��vl53l0x
//dev:�豸I2C�����ṹ��
VL53L0X_Error vl53l0x_init()
{	
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;	
	VL53L0X_i2c_init();//��ʼ��IIC����
	
	Status=vl53l0x_initX(&vl53l0x_dev0,0);
	if(Status!=0) return Status;
	
	Status=vl53l0x_initX(&vl53l0x_dev1,1);
	if(Status!=0) return Status;
	
	Status=vl53l0x_initX(&vl53l0x_dev2,2);
	if(Status!=0) return Status;
	
	Status=vl53l0x_initX(&vl53l0x_dev3,3);
	if(Status!=0) return Status;
  
}

u8 mode=0;//VL53L0x����ģʽ����--2019/11/12
void all_vl53l0x_init()
{
	if(vl53l0x_init())     //vl53l0x��ʼ��
	 {
		printf("VL53L0X_Init Error!!!\r\n");
		delay_ms(200);
	 }
	 else
	 { 
		 printf("VL53L0X_Init OK\r\n");
		 delay_ms(200);
	 }

	if(vl53l0x_set_mode(&vl53l0x_dev0,mode)&&vl53l0x_set_mode(&vl53l0x_dev1,mode)
		  &&vl53l0x_set_mode(&vl53l0x_dev2,mode)&&vl53l0x_set_mode(&vl53l0x_dev3,mode))   //���ò���ģʽ
		{
			printf("Mode Set Error!!!\r\n");
		}
	else
		printf("Mode Set OK!!!\r\n");
	delay_ms(200);
}

void vl53l0x_getdistance()
{
	VL53L0X_Error Status=VL53L0X_ERROR_NONE;//VL53L0x����״̬
	static char buf[VL53L0X_MAX_STRING_LENGTH];//����ģʽ�ַ����ַ�������
	
	Status = vl53l0x_start_single_test(&vl53l0x_dev0,&vl53l0x_data,buf);//ִ��һ�β���
	if(Status==VL53L0X_ERROR_NONE)
		//printf("d0: %4imm%x\r\n",Distance_data,vl53l0x_dev0.I2cDevAddr);//��ӡ��������
	       distancebuff[0]=Distance_data;
	else
		delay_ms(1);
//		printf("Measurement d0 is Error!!!\r\n");	
	//delay_ms(10);
	
	Status = vl53l0x_start_single_test(&vl53l0x_dev1,&vl53l0x_data,buf);//ִ��һ�β���
	if(Status==VL53L0X_ERROR_NONE)
		   distancebuff[1]=Distance_data;
	else
		delay_ms(1);
//		printf("Measurement d1 is Error!!!\r\n");	
	//delay_ms(10);
	
	Status = vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data,buf);//ִ��һ�β���
	if(Status==VL53L0X_ERROR_NONE)
		distancebuff[2]=Distance_data;
	else
		delay_ms(1);
//		printf("Measurement d2 is Error!!!\r\n");	
	//delay_ms(10);
	
	Status = vl53l0x_start_single_test(&vl53l0x_dev3,&vl53l0x_data,buf);//ִ��һ�β���
	if(Status==VL53L0X_ERROR_NONE)
		distancebuff[3]=Distance_data;
	else
		delay_ms(1);
//		printf("Measurement d3 is Error!!!\r\n");	
	//delay_ms(1000);
}
/*
//VL53L0X�����Գ���
void vl53l0x_test(void)
{
	 while(vl53l0x_init(&vl53l0x_dev))//vl53l0x��ʼ��
	 {
		printf("VL53L0X_Init Error!!!\r\n");
		delay_ms(500);
	 }
	 printf("VL53L0X_Init OK\r\n");
	 
	 while(1)
	 {
		 //	vl53l0x_calibration_test(&vl53l0x_dev);   break;//У׼ģʽ
		 //vl53l0x_general_test(&vl53l0x_dev);       break;//��ͨ����ģʽ
		 //vl53l0x_interrupt_test(&vl53l0x_dev);     break;//�жϲ���ģʽ  
		 vl53l0x_general_test(&vl53l0x_dev);
		 delay_ms(500);
	 }
}
*/
//----------���º���ΪUSMART����------------//

//��ȡvl53l0x������ID��Ϣ
void vl53l0x_info(void)
{
	printf("\r\n-------vl53l0x�������豸��Ϣ-------\r\n\r\n");
	printf("  Name:%s\r\n",vl53l0x_dev_info.Name);
	printf("  Addr:0x%x\r\n",vl53l0x_dev0.I2cDevAddr);
	printf("  ProductId:%s\r\n",vl53l0x_dev_info.ProductId);
	printf("  RevisionMajor:0x%x\r\n",vl53l0x_dev_info.ProductRevisionMajor);
	printf("  RevisionMinor:0x%x\r\n",vl53l0x_dev_info.ProductRevisionMinor);
	printf("\r\n-----------------------------------\r\n");
}

//��ȡһ�β�����������
//modeģʽ���� 0:Ĭ��;1:�߾���;2:������;3:����
void One_measurement(u8 mode)
{
	vl53l0x_set_mode(&vl53l0x_dev0,mode);
	VL53L0X_PerformSingleRangingMeasurement(&vl53l0x_dev0,&vl53l0x_data);
	printf("\r\n d: %4d mm.\r\n",vl53l0x_data.RangeMilliMeter);
		
}
