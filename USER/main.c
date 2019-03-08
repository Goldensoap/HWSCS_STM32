/*********************************************************************
 * 头文件
 */
/*系统配置*/
#include "sys.h"
#include "delay.h"
/*外设硬件*/
#include "led.h"
#include "lcd.h"
#include "rtc.h"
#include "timer.h"
/*FreeRTOS*/
#include "FreeRTOS.h"
#include "task.h"

/*********************************************************************
 * 任务定义区
 */
/*启动任务*/
#define START_TASK_PRIO		1       //任务优先级
#define START_STK_SIZE 		128     //任务堆栈大小	
TaskHandle_t StartTask_Handler;     //任务句柄
void start_task(void *pvParameters);//任务函数
/*串口1测试*/
#define UART1_TASK_PRIO		10      //任务优先级
#define UART1_STK_SIZE 		128     //任务堆栈大小	
TaskHandle_t UART1Task_Handler;     //任务句柄
void uart1_task(void *pvParameters);//任务函数
/*串口2测试*/
#define UART2_TASK_PRIO		9       //任务优先级
#define UART2_STK_SIZE 		128     //任务堆栈大小	
TaskHandle_t UART2Task_Handler;     //任务句柄
void uart2_task(void *pvParameters);//任务函数
/*RTC时钟测试*/
#define RTC_TASK_PRIO		11       //任务优先级
#define RTC_STK_SIZE 		256     //任务堆栈大小	
TaskHandle_t RTCTask_Handler;     //任务句柄
void rtc_task(void *pvParameters);//任务函数

/*********************************************************************
 * 全局变量
 */
//LCD刷屏时使用的颜色
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };

/*********************************************************************
 * 主函数
 */
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 	 
	delay_init();	    				//延时函数初始化	  
	uart1_init(115200);					//初始化串口
	uart2_init(115200);
	LED_Init();		  					//初始化LED
    LCD_Init();                         //初始化LCD

    POINT_COLOR = RED;                  //设置字体红色
	LCD_ShowString(30,10,200,16,16,"ATK STM32F103RCT6");	
	LCD_ShowString(30,30,200,16,16,"FreeRTOS core runing");
	LCD_ShowString(30,50,200,16,16,"environment ");

	/*创建开始任务*/
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

/*********************************************************************
 * 任务函数
 */
/*启动任务*/
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    /*创建中断测试任务*/
    xTaskCreate((TaskFunction_t )uart1_task,     	
                (const char*    )"uart1_task",   	
                (uint16_t       )UART1_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )UART1_TASK_PRIO,	
                (TaskHandle_t*  )&UART1Task_Handler);   
    /*创建uart2测试任务*/
    xTaskCreate((TaskFunction_t )uart2_task,     	
            (const char*    )"uart2_task",   	
            (uint16_t       )UART2_STK_SIZE, 
            (void*          )NULL,				
            (UBaseType_t    )UART2_TASK_PRIO,	
            (TaskHandle_t*  )&UART2Task_Handler);
    /*创建RTC测试任务*/
    xTaskCreate((TaskFunction_t )rtc_task,     	
            (const char*    )"rtc_task",   	
            (uint16_t       )RTC_STK_SIZE, 
            (void*          )NULL,				
            (UBaseType_t    )RTC_TASK_PRIO,
            (TaskHandle_t*  )&RTCTask_Handler);

    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

/*中断测试任务函数*/
void uart1_task(void *pvParameters)
{

    while(1){
        printf("uart1 out put......\r\n");
        LED0=~LED0;
        vTaskDelay(1000);
    }
}

/*uart2测试任务函数*/
void uart2_task(void *pvParameters)
{
    u8 TX[10]="0123456789";
    while(1){
        for(int i=0; i<10; i++){
             USART_SendData(USART2,TX[i]);
             while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET);
        }
        LED1=~LED1;
        vTaskDelay(500);
    }
}

/*RTC测试任务*/
void rtc_task(void *pvParameters)
{
	u8 t;
	
	POINT_COLOR = BLACK;

	LCD_DrawRectangle(125,110,234,314); //画一个矩形	
	LCD_DrawLine(125,130,234,130);		//画线
	POINT_COLOR = BLUE;
	LCD_ShowString(126,111,110,16,16,"RTC TEST");
	while(RTC_Init())		//RTC初始化	，一定要初始化成功
	{ 
		LCD_ShowString(126,111,110,16,16,"RTC ERROR!   ");
		delay_ms(800);
		LCD_ShowString(126,111,110,16,16,"RTC Trying...");
	}
    /*显示时间*/
	POINT_COLOR=BLUE;//设置字体为蓝色					 
	LCD_ShowString(126,131,200,16,16,"    -  -     ");	   
	LCD_ShowString(126,182,200,16,16,"  :  :  ");
	while(1)
	{
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
		delay_ms(10);
	}
}
