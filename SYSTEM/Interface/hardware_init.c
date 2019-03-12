/*********************************************************************
 * 头文件
 */

#include "hardware_init.h"

/*硬件头文件*/
#include "lcd.h"
#include "led.h"
#include "rtc.h"
#include "usart.h"
#include "sys.h"
#include "delay.h"
/*********************************************************************
 * 全局变量
 */

/*********************************************************************
 * 本地变量
 */

#define TIMESTAMP   1500000000

/*********************************************************************
 * 本地函数
 */

void hardware_init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 	 
	delay_init();	    				//延时函数初始化	  
	uart1_init(115200);					//初始化串口
	uart2_init(115200);
	LED_Init();		  					//初始化LED
    LCD_Init();                         //初始化LCD
	while(RTC_Init(TIMESTAMP))		//RTC初始化	，一定要初始化成功
	{ 
		LCD_ShowString(126,111,110,16,16,"RTC ERROR!   ");
		delay_ms(800);
		LCD_ShowString(126,111,110,16,16,"RTC Trying...");
	}
}
