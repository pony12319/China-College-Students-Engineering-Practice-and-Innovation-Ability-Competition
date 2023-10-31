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


//得到path路径下,目标文件的总个数
//path:路径		    
//返回值:总有效文件数
u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//临时目录
	FILINFO tfileinfo;	//临时文件信息	
	u8 *fn;	 			 			   			     
    res=f_opendir(&tdir,(const TCHAR*)path); 	//打开目录
  	tfileinfo.lfsize=_MAX_LFN*2+1;				//长文件名最大长度
	tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);//为长文件缓存区分配内存
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)//查询总的有效文件数
		{
	        res=f_readdir(&tdir,&tfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||tfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出		  
     		fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				rval++;//有效文件数增加1
			}	    
		}  
	} 
	return rval;
}

//播放gif视频
//注意：只支持gif格式播放，其他格式未测试
void show_video()
{
	u8 res;
 	DIR picdir;	 		//图片目录
	FILINFO picfileinfo;//文件信息
	u8 *fn;   			//长文件名
	u8 *pname;			//带路径的文件名
	u16 totpicnum; 		//图片文件总数
	u16 curindex;		//图片当前索引
	u8 key;				//键值
	u8 pause=0;			//暂停标记
	u8 t;
	u16 temp;
	u16 *picindextbl;	//图片索引表
	
	while(f_opendir(&picdir,"0:/PICTURE"))//打开图片文件夹
 	{	    
		Show_Str(30,170,240,16,"PICTURE文件夹错误!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	}  
	totpicnum=pic_get_tnum("0:/PICTURE"); //得到总有效文件数
  	while(totpicnum==NULL)//图片文件为0		
 	{	    
		Show_Str(30,170,240,16,"没有图片文件!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	}
  	picfileinfo.lfsize=_MAX_LFN*2+1;						//长文件名最大长度
	picfileinfo.lfname=mymalloc(SRAMIN,picfileinfo.lfsize);	//为长文件缓存区分配内存
 	pname=mymalloc(SRAMIN,picfileinfo.lfsize);				//为带路径的文件名分配内存
 	picindextbl=mymalloc(SRAMIN,2*totpicnum);				//申请2*totpicnum个字节的内存,用于存放图片索引
 	while(picfileinfo.lfname==NULL||pname==NULL||picindextbl==NULL)//内存分配出错
 	{	    
		Show_Str(30,170,240,16,"内存分配失败!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	
	//记录索引
    res=f_opendir(&picdir,"0:/PICTURE"); //打开目录
	if(res==FR_OK)
	{
		curindex=0;//当前索引为0
		while(1)//全部查询一遍
		{
			temp=picdir.index;								//记录当前index
	        res=f_readdir(&picdir,&picfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出		  
     		fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				picindextbl[curindex]=temp;//记录索引
				curindex++;
			}	    
		} 
	}   
	delay_ms(1500);
	piclib_init();										//初始化画图	   	   
	curindex=0;											//从0开始显示
   	res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//打开目录
	while(res==FR_OK)//打开成功
	{	
		
		dir_sdi(&picdir,picindextbl[curindex]);			//改变当前目录索引	   
        res=f_readdir(&picdir,&picfileinfo);       		//读取目录下的一个文件
        if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出
     	fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
		strcpy((char*)pname,"0:/PICTURE/");				//复制路径(目录)
		strcat((char*)pname,(const char*)fn);  			//将文件名接在后面
 		LCD_Clear(BLACK);
 		res = ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//显示图片  
		t=0;
		while(1) 
		{
			curindex++;		   	
			if(curindex>=totpicnum)curindex=0;//到末尾的时候,自动从头开始
			break;

		}					     
	} 											  
	myfree(SRAMIN,picfileinfo.lfname);	//释放内存			    
	myfree(SRAMIN,pname);				//释放内存			    
	myfree(SRAMIN,picindextbl);			//释放内存	

	
}
// 格式化并显示字符串
void DisplayFormattedString(int x, int y, int width, int height, const char *format, ...) 
{
    char buffer[256]; // 用于存储格式化后的字符串
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    Show_Str(x, y, width, height, buffer, 24, 0);
}

//展示是否满载
void IsFullLoad(u16 distance[])
{
	//distancebuff[3] -- 有害垃圾
	//distancebuff[1] -- 其他垃圾
	//distancebuff[2] -- 可回收垃圾
	//distancebuff[0] -- 厨余垃圾
	
			//有害垃圾满载检测
			//空载数据 250左右
			if (distancebuff[3]<200)
			{
				delay_ms(50);
				if (distancebuff[3]<200)
					Show_Str(550,0,200,24,"有害满载",24,0);
			}
			else
			{
				delay_ms(50);
				if (distancebuff[3]>200)
					Show_Str(550,0,200,24,"xxxxxxxx",24,0);
			}
				
			//可回收垃圾满载
			//空载数据 250左右
			if (distancebuff[2]<200)
			{
				delay_ms(50);
				if (distancebuff[2]<200)
					Show_Str(550,40,200,24,"可回收满载",24,0);
			}	
			else
			{
				delay_ms(50);
				if (distancebuff[2]>220)
					Show_Str(550,40,200,24,"xxxxxxxxxx",24,0);
			}
			
			//厨余垃圾满载
			//空载数据 250左右
			if (distancebuff[0]<200)
			{
				delay_ms(50);
				if (distancebuff[0]<220)
					Show_Str(550,80,200,24,"厨余满载",24,0);
			}	
			else
			{
				delay_ms(50);
				if (distancebuff[0]>220)
					Show_Str(550,80,200,24,"xxxxxxxx",24,0);
			}
			
			//其他垃圾满载
			//空载数据 250左右
			if (distancebuff[1]<220)
			{
				delay_ms(50);
				if (distancebuff[1]<220)
					Show_Str(550,120,200,24,"其他满载",24,0);
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
	 u8 order = 1;//垃圾顺序
	 u16 position = 30;//y坐标
	 u16 len;
	 char t;//暂存串口值
	 int i = 1490, j = 1655, k=1490, m;//i水平控制参数，j倾斜控制参数
	 
	 // 定义一个二维字符数组来存储垃圾名称
    const char garbages[4][20] = {
        "有害垃圾",
		"可回收垃圾", 
		"厨余垃圾",
		"其他垃圾"
    };
	TIM6_Init();//计时器初始化
	delay_init();	    	 //延时函数初始化	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为115200
	belt_init();
	start_belt();
	My_TIM4_Init(20000-1,72-1);//周期为20ms
	Compressor_Init();
	Switch_Init();//光电开关初始化
	TIM_SetCompare4(TIM4,1750);     //接1  初始105度
	delay_ms(1000);
	TIM_SetCompare1(TIM4,1490);     //接0水平舵机 初始90度
	Servo_SetAngle(90); //开合舵机初始化 90度

	 
	vl53l0x_init();				

	start_belt();
	LCD_Init();			   		//初始化LCD   
	W25QXX_Init();				//初始化W25Q128
 	my_mem_init(SRAMIN);		//初始化内部内存池
	exfuns_init();				//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
    POINT_COLOR=RED;      
	while(font_init()) 		//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
		delay_ms(200);				  
	}  
		
	Show_Str(30, 50, 200, 24, "垃圾分类实验", 24, 0);
	delay_ms(1000);
	
	show_video();
	printf("ok");
	
	//反复发送OK给js，但是使用过程中有点问题
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

 	
	LCD_Fill(0,0,800,480,WHITE);//清除显示
	Show_Str(0,0,500,30,"序号  垃圾种类    数量  状态",24,0);
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
					//有害垃圾
					case 'A':
						stop_belt();
						
						//水平旋转
						while(i <= 2480)
						{
							TIM_SetCompare1(TIM4,i);
							delay_ms(10);
							i += 11;
						}
						delay_ms(300);
						
						//倾倒动作
						while(j >= 1350)
						{
							TIM_SetCompare4(TIM4,j);
							delay_ms(25);
							j -= 11;
						}
						delay_ms(1000);                  //等待垃圾倒下
						
						//回正
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
						
						//显示信息
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
					
						//可回收垃圾
					case 'B':
						stop_belt();
					
						//水平选择
						while(i >= 500)
						{
							TIM_SetCompare1(TIM4,i);
							delay_ms(10);
							i -= 11;
						}
						delay_ms(300);
					
						//倾倒动作
						while(j >= 1480)
						{
							TIM_SetCompare4(TIM4,j);
							delay_ms(25);
							j -= 11;
						}
						delay_ms(1000);                  //等待垃圾倒下
						Compressor_Press();		//压缩垃圾	压缩机动了之后需要重新初始化串口、激光
						uart_init(9600);
						Dump();				
						vl53l0x_init();				//重新初始化激光测距，解决了一个奇怪的问题
						
						//控制开合
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
						
						
								
						//回正
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
						
						//显示信息
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

						//厨余垃圾
					case 'C':
						stop_belt();
											//水平转动
						while(i <= 1820)
						{
							TIM_SetCompare1(TIM4,i);
							delay_ms(10);
							i += 11;
						}
						delay_ms(6);
						
						//倾倒动作
						while(j >= 1350)
						{
							TIM_SetCompare4(TIM4,j);
							delay_ms(25);
							j -= 11;
						}
						delay_ms(1000);                  //等待垃圾倒下
						
						
						//回正
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
						
						//显示信息
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
						
						//其他垃圾
					case 'D':
						stop_belt();
					
						while(i >= 1160)
						{
							TIM_SetCompare1(TIM4,i);
							delay_ms(10);
							i -= 11;
						}
						delay_ms(300);
						
						//倾倒动作
						while(j >= 1370)
						{
							TIM_SetCompare4(TIM4,j);
							delay_ms(30);
							j -= 11;
						}
						delay_ms(1000);                  //等待垃圾倒下
						delay_ms(1000); 
						
						//回正
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
						
						//显示信息
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
			
			//10个垃圾后不再判断超时
			if (order == 10)
			{
				TIM_Cmd(TIM6,DISABLE);
				maxtime = 0;
			}
			
			//识别不了都按其他垃圾处理
			if (maxtime >= 9)
			{
				while(i >= 1160)
					{
						TIM_SetCompare1(TIM4,i);
						delay_ms(10);
						i -= 11;
					}
					delay_ms(300);
					
					//倾倒动作
					while(j >= 1370)
					{
						TIM_SetCompare4(TIM4,j);
						delay_ms(30);
						j -= 11;
					}
					delay_ms(1000);                  //等待垃圾倒下
					delay_ms(1000); 
					
					//回正
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
					
					//显示信息
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
			
			//满载检测
			vl53l0x_init();
			vl53l0x_getdistance();	
			//打印输出测试
//			printf("厨余0:%d\r\n",distancebuff[0]);
//			printf("其他1:%d\r\n",distancebuff[1]);
//			printf("可回收2:%d\r\n",distancebuff[2]);
//			printf("有害3:%d\r\n",distancebuff[3]);

			IsFullLoad(distancebuff);
	    }
}
















