#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h" 		 	 
#include "lcd.h"  
#include "key.h"     
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"   
#include "text.h"
#include "piclib.h"	
#include "string.h"		
#include "math.h"
#include "vl53l0x.h"
#include "vl53l0x_gen.h"
#include "tpad.h"
#include <stdarg.h>
#include "stdio.h"
#include "servo.h"
#include "compressor.h"
#include "belt.h"
#include "InfrarePSwitch.h"
#include "timer.h"


//�õ�path·����,Ŀ���ļ����ܸ���
//path:·��		    
//����ֵ:����Ч�ļ���
u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//��ʱĿ¼
	FILINFO tfileinfo;	//��ʱ�ļ���Ϣ	
	u8 *fn;	 			 			   			     
    res=f_opendir(&tdir,(const TCHAR*)path); 	//��Ŀ¼
  	tfileinfo.lfsize=_MAX_LFN*2+1;				//���ļ�����󳤶�
	tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);//Ϊ���ļ������������ڴ�
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)//��ѯ�ܵ���Ч�ļ���
		{
	        res=f_readdir(&tdir,&tfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||tfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�		  
     		fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				rval++;//��Ч�ļ�������1
			}	    
		}  
	} 
	return rval;
}

//����gif��Ƶ
//ע�⣺ֻ֧��gif��ʽ���ţ�������ʽδ����
void show_video()
{
	u8 res;
 	DIR picdir;	 		//ͼƬĿ¼
	FILINFO picfileinfo;//�ļ���Ϣ
	u8 *fn;   			//���ļ���
	u8 *pname;			//��·�����ļ���
	u16 totpicnum; 		//ͼƬ�ļ�����
	u16 curindex;		//ͼƬ��ǰ����
	u8 key;				//��ֵ
	u8 pause=0;			//��ͣ���
	u8 t;
	u16 temp;
	u16 *picindextbl;	//ͼƬ������
	
	while(f_opendir(&picdir,"0:/PICTURE"))//��ͼƬ�ļ���
 	{	    
		Show_Str(30,170,240,16,"PICTURE�ļ��д���!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  
	totpicnum=pic_get_tnum("0:/PICTURE"); //�õ�����Ч�ļ���
  	while(totpicnum==NULL)//ͼƬ�ļ�Ϊ0		
 	{	    
		Show_Str(30,170,240,16,"û��ͼƬ�ļ�!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}
  	picfileinfo.lfsize=_MAX_LFN*2+1;						//���ļ�����󳤶�
	picfileinfo.lfname=mymalloc(SRAMIN,picfileinfo.lfsize);	//Ϊ���ļ������������ڴ�
 	pname=mymalloc(SRAMIN,picfileinfo.lfsize);				//Ϊ��·�����ļ��������ڴ�
 	picindextbl=mymalloc(SRAMIN,2*totpicnum);				//����2*totpicnum���ֽڵ��ڴ�,���ڴ��ͼƬ����
 	while(picfileinfo.lfname==NULL||pname==NULL||picindextbl==NULL)//�ڴ�������
 	{	    
		Show_Str(30,170,240,16,"�ڴ����ʧ��!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  	
	//��¼����
    res=f_opendir(&picdir,"0:/PICTURE"); //��Ŀ¼
	if(res==FR_OK)
	{
		curindex=0;//��ǰ����Ϊ0
		while(1)//ȫ����ѯһ��
		{
			temp=picdir.index;								//��¼��ǰindex
	        res=f_readdir(&picdir,&picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�		  
     		fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				picindextbl[curindex]=temp;//��¼����
				curindex++;
			}	    
		} 
	}   
	delay_ms(1500);
	piclib_init();										//��ʼ����ͼ	   	   
	curindex=0;											//��0��ʼ��ʾ
   	res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//��Ŀ¼
	while(res==FR_OK)//�򿪳ɹ�
	{	
		
		dir_sdi(&picdir,picindextbl[curindex]);			//�ı䵱ǰĿ¼����	   
        res=f_readdir(&picdir,&picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
        if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�
     	fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
		strcpy((char*)pname,"0:/PICTURE/");				//����·��(Ŀ¼)
		strcat((char*)pname,(const char*)fn);  			//���ļ������ں���
 		LCD_Clear(BLACK);
 		res = ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//��ʾͼƬ  
		t=0;
		while(1) 
		{
			curindex++;		   	
			if(curindex>=totpicnum)curindex=0;//��ĩβ��ʱ��,�Զ���ͷ��ʼ
			break;

		}					     
	} 											  
	myfree(SRAMIN,picfileinfo.lfname);	//�ͷ��ڴ�			    
	myfree(SRAMIN,pname);				//�ͷ��ڴ�			    
	myfree(SRAMIN,picindextbl);			//�ͷ��ڴ�	

	
}
// ��ʽ������ʾ�ַ���
void DisplayFormattedString(int x, int y, int width, int height, const char *format, ...) 
{
    char buffer[256]; // ���ڴ洢��ʽ������ַ���
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    Show_Str(x, y, width, height, buffer, 24, 0);
}

//չʾ�Ƿ�����
void IsFullLoad(u16 distance[])
{
	//distancebuff[3] -- �к�����
	//distancebuff[1] -- ��������
	//distancebuff[2] -- �ɻ�������
	//distancebuff[0] -- ��������
	
			//�к��������ؼ��
			//�������� 250����
			if (distancebuff[3]<200)
			{
				delay_ms(50);
				if (distancebuff[3]<200)
					Show_Str(550,0,200,24,"�к�����",24,0);
			}
			else
			{
				delay_ms(50);
				if (distancebuff[3]>200)
					Show_Str(550,0,200,24,"xxxxxxxx",24,0);
			}
				
			//�ɻ�����������
			//�������� 250����
			if (distancebuff[2]<200)
			{
				delay_ms(50);
				if (distancebuff[2]<200)
					Show_Str(550,40,200,24,"�ɻ�������",24,0);
			}	
			else
			{
				delay_ms(50);
				if (distancebuff[2]>220)
					Show_Str(550,40,200,24,"xxxxxxxxxx",24,0);
			}
			
			//������������
			//�������� 250����
			if (distancebuff[0]<200)
			{
				delay_ms(50);
				if (distancebuff[0]<220)
					Show_Str(550,80,200,24,"��������",24,0);
			}	
			else
			{
				delay_ms(50);
				if (distancebuff[0]>220)
					Show_Str(550,80,200,24,"xxxxxxxx",24,0);
			}
			
			//������������
			//�������� 250����
			if (distancebuff[1]<220)
			{
				delay_ms(50);
				if (distancebuff[1]<220)
					Show_Str(550,120,200,24,"��������",24,0);
			}	
			else
			{
				delay_ms(50);
				if (distancebuff[1]>220)
					Show_Str(550,120,200,24,"xxxxxxxx",24,0);
			}
	
	
}

 int main(void)
 {	 
	 u8 order = 1;//����˳��
	 u16 position = 30;//y����
	 u16 len;
	 char t;//�ݴ洮��ֵ
	 int i = 1490, j = 1655, k=1490, m;//iˮƽ���Ʋ�����j��б���Ʋ���
	 
	 // ����һ����ά�ַ��������洢��������
    const char garbages[4][20] = {
        "�к�����",
		"�ɻ�������", 
		"��������",
		"��������"
    };
	TIM6_Init();//��ʱ����ʼ��
	delay_init();	    	 //��ʱ������ʼ��	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ115200
	belt_init();
	start_belt();
	My_TIM4_Init(20000-1,72-1);//����Ϊ20ms
	Compressor_Init();
	Switch_Init();//��翪�س�ʼ��
	TIM_SetCompare4(TIM4,1750);     //��1  ��ʼ105��
	delay_ms(1000);
	TIM_SetCompare1(TIM4,1490);     //��0ˮƽ��� ��ʼ90��
	Servo_SetAngle(90); //���϶����ʼ�� 90��

	 
	vl53l0x_init();				

	start_belt();
	LCD_Init();			   		//��ʼ��LCD   
	W25QXX_Init();				//��ʼ��W25Q128
 	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH.
    POINT_COLOR=RED;      
	while(font_init()) 		//����ֿ�
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  
		
	Show_Str(30, 50, 200, 24, "��������ʵ��", 24, 0);
	delay_ms(1000);
	
	show_video();
	printf("ok");
	
	//��������OK��js������ʹ�ù������е�����
//	while(1)
//	{
//		printf("ok");
//		if(USART_RX_STA&0x8000)
//		{
//			len = USART_RX_STA&0x3fff;
//			if(USART_RX_BUF[0] == 's')
//			{
//				USART_RX_STA=0;
//				break;
//			}
//				
//		}
//		delay_ms(300);
//		
//	}

 	
	LCD_Fill(0,0,800,480,WHITE);//�����ʾ
	Show_Str(0,0,500,30,"���  ��������    ����  ״̬",24,0);
	maxtime = 0;
	TIM_Cmd(TIM6,DISABLE);
	delay_ms(10);
	TIM_Cmd(TIM6,ENABLE);

	
	while(1)
		{
				
			if (USART_RX_STA&0x8000)
			{
				len = USART_RX_STA&0x3fff;
				t = USART_RX_BUF[0];
				switch(t)
				{
					//�к�����
					case 'A':
						stop_belt();
						
						//ˮƽ��ת
						while(i <= 2480)
						{
							TIM_SetCompare1(TIM4,i);
							delay_ms(10);
							i += 11;
						}
						delay_ms(300);
						
						//�㵹����
						while(j >= 1350)
						{
							TIM_SetCompare4(TIM4,j);
							delay_ms(25);
							j -= 11;
						}
						delay_ms(1000);                  //�ȴ���������
						
						//����
						while(j <= 1750)
						{
								TIM_SetCompare4(TIM4,j);
								delay_ms(1);
								j += 11;
						}
						if(i <= 1490)
						{
								while(i <= 1490)
								{
									 TIM_SetCompare1(TIM4,i);
									 delay_ms(1);
									 i += 11;
								}
						}
						else if(i > 1501)
						{
								while(i >= 1490)
								{
									 TIM_SetCompare1(TIM4,i);
									 delay_ms(1);
									 i -= 11;
								}
						}
						
						//��ʾ��Ϣ
						DisplayFormattedString(0, position, 550, 30, "%d    %s     1      ok!", order, garbages[0]);
						order += 1;
						position += 30;
						if (position>=450)
							position = 30;
						USART_RX_STA=0;
						start_belt();
						printf("cc");
						maxtime = 0;
						uart_init(9600);
						break;
					
						//�ɻ�������
					case 'B':
						stop_belt();
					
						//ˮƽѡ��
						while(i >= 500)
						{
							TIM_SetCompare1(TIM4,i);
							delay_ms(10);
							i -= 11;
						}
						delay_ms(300);
					
						//�㵹����
						while(j >= 1480)
						{
							TIM_SetCompare4(TIM4,j);
							delay_ms(25);
							j -= 11;
						}
						delay_ms(1000);                  //�ȴ���������
						Compressor_Press();		//ѹ������	ѹ��������֮����Ҫ���³�ʼ�����ڡ�����
						uart_init(9600);
						Dump();				
						vl53l0x_init();				//���³�ʼ�������࣬�����һ����ֵ�����
						
						//���ƿ���
						while(k<1700)
						{
							TIM_SetCompare4(TIM3,i);
							delay_ms(5);
							k+=50;
						}
						while(k>1500)
						{
							TIM_SetCompare4(TIM3,i);
							delay_ms(5);
							k-=50;
						}
						
						
								
						//����
						while(j <= 1750)
						{
								TIM_SetCompare4(TIM4,j);
								delay_ms(1);
								j += 11;
						}
						if(i <= 1490)
						{
								while(i <= 1490)
								{
									 TIM_SetCompare1(TIM4,i);
									delay_ms(1);
									 i += 11;
								}
						}
						else if(i > 1501)
						{
								while(i >= 1490)
								{
									 TIM_SetCompare1(TIM4,i);
									 delay_ms(1);
									 i -= 11;
								}
						}
						
						//��ʾ��Ϣ
						DisplayFormattedString(0, position, 550, 30, "%d    %s   1      ok!", order, garbages[1]);
						order += 1;
						position += 30;
						if (position>=450)
							position = 30;
						USART_RX_STA=0;
						start_belt();
						printf("cc");
						maxtime = 0;
						break;

						//��������
					case 'C':
						stop_belt();
											//ˮƽת��
						while(i <= 1820)
						{
							TIM_SetCompare1(TIM4,i);
							delay_ms(10);
							i += 11;
						}
						delay_ms(6);
						
						//�㵹����
						while(j >= 1350)
						{
							TIM_SetCompare4(TIM4,j);
							delay_ms(25);
							j -= 11;
						}
						delay_ms(1000);                  //�ȴ���������
						
						
						//����
						while(j <= 1750)
						{
								TIM_SetCompare4(TIM4,j);
								delay_ms(1);
								j += 11;
						}
						if(i <= 1490)
						{
								while(i <= 1490)
								{
									 TIM_SetCompare1(TIM4,i);
									 delay_ms(1);
									 i += 11;
								}
						}
						else if(i > 1501)
						{
								while(i >= 1490)
								{
									 TIM_SetCompare1(TIM4,i);
									 delay_ms(1);
									 i -= 11;
								}
						}
						
						//��ʾ��Ϣ
						DisplayFormattedString(0, position, 550, 30, "%d    %s     1      ok!", order, garbages[2]);
						order += 1;
						position += 30;
						if (position>=450)
							position = 30;
						USART_RX_STA=0;
						start_belt();
						printf("cc");
						maxtime = 0;
						uart_init(9600);
						break;
						
						//��������
					case 'D':
						stop_belt();
					
						while(i >= 1160)
						{
							TIM_SetCompare1(TIM4,i);
							delay_ms(10);
							i -= 11;
						}
						delay_ms(300);
						
						//�㵹����
						while(j >= 1370)
						{
							TIM_SetCompare4(TIM4,j);
							delay_ms(30);
							j -= 11;
						}
						delay_ms(1000);                  //�ȴ���������
						delay_ms(1000); 
						
						//����
						while(j <= 1750)
						{
								TIM_SetCompare4(TIM4,j);
								delay_ms(1);
								j += 11;
						}
						if(i <= 1490)
						{
								while(i <= 1490)
								{
									 TIM_SetCompare1(TIM4,i);
									 delay_ms(1);
									 i += 11;
								}
						}
						else if(i > 1501)
						{
								while(i >= 1490)
								{
									 TIM_SetCompare1(TIM4,i);
									 delay_ms(1);
									 i -= 11;
								}
						}
						
						//��ʾ��Ϣ
						DisplayFormattedString(0, position, 550, 30, "%d    %s     1      ok!", order, garbages[3]);
						order += 1;
						position += 30;
						if (position>=450)
							position = 30;
						USART_RX_STA=0;
						start_belt();
						printf("cc");
						maxtime = 0;
						uart_init(9600);
						break;
						
				}
				
			}
			
			//10�����������жϳ�ʱ
			if (order == 10)
			{
				TIM_Cmd(TIM6,DISABLE);
				maxtime = 0;
			}
			
			//ʶ���˶���������������
			if (maxtime >= 9)
			{
				while(i >= 1160)
					{
						TIM_SetCompare1(TIM4,i);
						delay_ms(10);
						i -= 11;
					}
					delay_ms(300);
					
					//�㵹����
					while(j >= 1370)
					{
						TIM_SetCompare4(TIM4,j);
						delay_ms(30);
						j -= 11;
					}
					delay_ms(1000);                  //�ȴ���������
					delay_ms(1000); 
					
					//����
					while(j <= 1750)
					{
							TIM_SetCompare4(TIM4,j);
							delay_ms(1);
							j += 11;
					}
					if(i <= 1490)
					{
							while(i <= 1490)
							{
								 TIM_SetCompare1(TIM4,i);
								 delay_ms(1);
								 i += 11;
							}
					}
					else if(i > 1501)
					{
							while(i >= 1490)
							{
								 TIM_SetCompare1(TIM4,i);
								 delay_ms(1);
								 i -= 11;
							}
					}
					
					//��ʾ��Ϣ
					DisplayFormattedString(0, position, 550, 30, "%d    %s     1      ok!", order, garbages[3]);
					order += 1;
					position += 30;
					if (position>=450)
						position = 30;
					USART_RX_STA=0;
					start_belt();
					maxtime = 0;
					uart_init(9600);
			}
			
			//���ؼ��
			vl53l0x_init();
			vl53l0x_getdistance();	
			//��ӡ�������
//			printf("����0:%d\r\n",distancebuff[0]);
//			printf("����1:%d\r\n",distancebuff[1]);
//			printf("�ɻ���2:%d\r\n",distancebuff[2]);
//			printf("�к�3:%d\r\n",distancebuff[3]);

			IsFullLoad(distancebuff);
	    }
}
















