/*********************************************************************
 * 头文件
 */

#include "LCD_task.h"
#include "lcd.h"
#include "delay.h"
/*********************************************************************
 * 全局变量
 */
TaskHandle_t LCDTask_Handler;     		//任务句柄
/*********************************************************************
 * 本地变量
 */
//时间结构体
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	//公历日月年周
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;		 
}_calendar_obj;					 
static _calendar_obj calendar;	//日历结构体
//月份数据表
static const u8 table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表	  
//平年的月份日期表
static const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
//LCD刷屏时使用的颜色
static int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };

#define SKIP	0  //队列获取跳过阻塞
/*********************************************************************
 * 函数声明
 */
static u8 Is_Leap_Year(u16 year);
static u8 Time_Get(u32 timecount);
static u8 RTC_Get_Week(u16 year,u8 month,u8 day);

/*********************************************************************
 * 本地函数
 */

/*人机交互测试*/
void lcd_task(void *pvParameters)
{
	u8 t;
	u32 timestamp=0;
	/*基础说明*/
    POINT_COLOR = RED;                  //设置字体红色
	LCD_ShowString(30,10,200,16,16,"ATK STM32F103RCT6");	//CPU型号
	LCD_ShowString(30,30,200,16,16,"FreeRTOS core runing"); //操作系统型号
	LCD_ShowString(30,50,200,16,16,"Environmental Perceptor"); //设备名
	/*mesh 路由表+状态信息*/

	/*时间模块UI框架*/
	POINT_COLOR = BLACK;
	LCD_DrawRectangle(125,110,234,314); //画一个矩形	
	LCD_DrawLine(125,130,234,130);		//画线
	POINT_COLOR = BLUE;
	LCD_ShowString(126,111,110,16,16,"RTC TEST");	//框内填充字			 
	LCD_ShowString(126,131,200,16,16,"    -  -     ");	   //日期格式
	LCD_ShowString(126,182,200,16,16,"  :  :  ");// 时间格式
	while(1){
		/* 更新时间 */
		xQueuePeek(Time_Stamp_Queue,&timestamp,SKIP);
		Time_Get(timestamp);
		if(t!=calendar.sec)
		{
			t=calendar.sec;
			LCD_ShowNum(126,131,calendar.w_year,4,16);									  
			LCD_ShowNum(166,131,calendar.w_month,2,16);									  
			LCD_ShowNum(190,131,calendar.w_date,2,16);	 
			switch(calendar.week)
			{
				case 0:
					LCD_ShowString(126,149,200,16,16,"Sunday   ");
					break;
				case 1:
					LCD_ShowString(126,149,200,16,16,"Monday   ");
					break;
				case 2:
					LCD_ShowString(126,149,200,16,16,"Tuesday  ");
					break;
				case 3:
					LCD_ShowString(126,149,200,16,16,"Wednesday");
					break;
				case 4:
					LCD_ShowString(126,149,200,16,16,"Thursday ");
					break;
				case 5:
					LCD_ShowString(126,149,200,16,16,"Friday   ");
					break;
				case 6:
					LCD_ShowString(126,149,200,16,16,"Saturday ");
					break;  
			}
			LCD_ShowNum(126,182,calendar.hour,2,16);									  
			LCD_ShowNum(150,182,calendar.min,2,16);									  
			LCD_ShowNum(174,182,calendar.sec,2,16);
		}
		/*更新路由表*/
		delay_ms(10);
	}
}

/*判断是否是闰年函数
月份   1  2  3  4  5  6  7  8  9  10 11 12
闰年   31 29 31 30 31 30 31 31 30 31 30 31
非闰年 31 28 31 30 31 30 31 31 30 31 30 31
输入:年份
输出:该年份是不是闰年.1,是.0,不是*/
static u8 Is_Leap_Year(u16 year)
{			  
	if(year%4==0) //必须能被4整除
	{
		if(year%100==0) 
		{
			if(year%400==0)return 1;//如果以00结尾,还要能被400整除 	   
			else return 0;
		}else return 1;
	}else return 0;
}

//得到当前的时间
//返回值:0,成功;其他:错误代码.
static u8 Time_Get(u32 timecount)
{
	static u16 daycnt=0;
	timecount += 8*3600;//UTC 北京时间修正8h 
	u32 temp=0;
	u16 temp1=0;
 	temp=(timecount)/86400;   //得到天数(秒钟数对应的)
	if(daycnt!=temp)//超过一天了
	{	  
		daycnt=temp;
		temp1=1970;	//从1970年开始
		while(temp>=365)
		{				 
			if(Is_Leap_Year(temp1))//是闰年
			{
				if(temp>=366)temp-=366;//闰年的秒钟数
				else {temp1++;break;}  
			}
			else temp-=365;	  //平年 
			temp1++;  
		}   
		calendar.w_year=temp1;//得到年份
		temp1=0;
		while(temp>=28)//超过了一个月
		{
			if(Is_Leap_Year(calendar.w_year)&&temp1==1)//当年是不是闰年/2月份
			{
				if(temp>=29)temp-=29;//闰年的秒钟数
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//平年
				else break;
			}
			temp1++;  
		}
		calendar.w_month=temp1+1;	//得到月份
		calendar.w_date=temp+1;  	//得到日期 
	}
	temp=timecount%86400;     		//得到秒钟数   	   
	calendar.hour=temp/3600;     	//小时
	calendar.min=(temp%3600)/60; 	//分钟	
	calendar.sec=(temp%3600)%60; 	//秒钟
	calendar.week=RTC_Get_Week(calendar.w_year,calendar.w_month,calendar.w_date);//获取星期   
	return 0;
}
//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//输入参数：公历年月日 
//返回值：星期号																						 
static u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{
	u16 temp2;
	u8 yearH,yearL;

	yearH=year/100;	yearL=year%100; 
	// 如果为21世纪,年份数加100  
	if (yearH>19)yearL+=100;
	// 所过闰年数只算1900年之后的  
	temp2=yearL+yearL/4;
	temp2=temp2%7;
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}
