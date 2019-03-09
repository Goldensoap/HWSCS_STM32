/*********************************************************************
 * 头文件
 */
/*C语言标准库*/

/*系统配置*/
#include "sys.h"
#include "delay.h"
#include "usart.h"
/*外设硬件*/
#include "led.h"
#include "lcd.h"
#include "rtc.h"
/*FreeRTOS*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/*********************************************************************
 * 任务定义区
 * 任务优先级 0-32
 * 0优先级最低为空闲任务，32最高
 */
/*启动任务*/
#define START_TASK_PRIO			1       //任务优先级
#define START_STK_SIZE 			128     //任务堆栈大小	
TaskHandle_t StartTask_Handler;     	//任务句柄
void start_task(void *pvParameters);	//任务函数

/*上位机信息解析任务测试*/
#define CMD_GET_TASK_PRIO		10      //任务优先级
#define CMD_GET_STK_SIZE 		128     //任务堆栈大小	
TaskHandle_t CMD_GETTask_Handler;     	//任务句柄
void cmd_get_task(void *pvParameters);	//任务函数

/*mesh网信息解析任务测试*/
#define MESH_GET_TASK_PRIO		9       //任务优先级
#define MESH_GET_STK_SIZE 		128     //任务堆栈大小
TaskHandle_t MESH_GETTask_Handler;     	//任务句柄
void mesh_get_task(void *pvParameters);	//任务函数

/*网络信息上传任务测试*/
#define MSG_UPLOAD_TASK_PRIO		9       //任务优先级
#define MSG_UPLOAD_STK_SIZE 		128     //任务堆栈大小	
TaskHandle_t MSG_UPLOADTask_Handler;     	//任务句柄
void msg_upload_task(void *pvParameters);	//任务函数

/*指令上传任务测试*/
#define CMD_UPLOAD_TASK_PRIO		9       //任务优先级
#define CMD_UPLOAD_STK_SIZE 		128     //任务堆栈大小	
TaskHandle_t CMD_UPLOADTask_Handler;     	//任务句柄
void cmd_upload_task(void *pvParameters);	//任务函数

/*人机交互测试*/
#define LCD_TASK_PRIO			0       //任务优先级
#define LCD_STK_SIZE 			128     //任务堆栈大小
TaskHandle_t LCDTask_Handler;     		//任务句柄
void lcd_task(void *pvParameters);		//任务函数

/*RTC时钟测试*/
#define RTC_TASK_PRIO			11      //任务优先级
#define RTC_STK_SIZE 			256     //任务堆栈大小	
TaskHandle_t RTCTask_Handler;     		//任务句柄
void rtc_task(void *pvParameters);		//任务函数

/*********************************************************************
 * 全局变量
 */
QueueHandle_t CMD_Get_Queue;	//上位机命令获取消息队列句柄

/*********************************************************************
 * 本地变量
 */
#define CMD_GET_Q_NUM	5		//上位机命令获取消息队列的数量
static int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };//LCD刷屏时使用的颜色

/*********************************************************************
 * 主函数
 */
int main(void)
{
	/*系统初始化*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 	 
	delay_init();	    				//延时函数初始化	  
	uart1_init(115200);					//初始化串口
	uart2_init(115200);
	LED_Init();		  					//初始化LED
    LCD_Init();                         //初始化LCD

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
	/*创建队列*/
	CMD_Get_Queue=xQueueCreate(CMD_GET_Q_NUM,USART_REC_LEN);

    /*创建上位机信息解析任务*/
    xTaskCreate((TaskFunction_t )cmd_get_task,     	
                (const char*    )"cmd_get_task",   	
                (uint16_t       )CMD_GET_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )CMD_GET_TASK_PRIO,	
                (TaskHandle_t*  )&CMD_GETTask_Handler);
    // /*创建mesh网信息解析任务*/
    // xTaskCreate((TaskFunction_t )mesh_get_task,     	
    //         (const char*    )"mesh_get_task",   	
    //         (uint16_t       )MESH_GET_STK_SIZE, 
    //         (void*          )NULL,				
    //         (UBaseType_t    )MESH_GET_TASK_PRIO,	
    //         (TaskHandle_t*  )&MESH_GETTask_Handler);
    /*创建RTC测试任务*/
    xTaskCreate((TaskFunction_t )rtc_task,     	
            (const char*    )"rtc_task",   	
            (uint16_t       )RTC_STK_SIZE, 
            (void*          )NULL,				
            (UBaseType_t    )RTC_TASK_PRIO,
            (TaskHandle_t*  )&RTCTask_Handler);

    /*创建LCD测试任务*/
    xTaskCreate((TaskFunction_t )lcd_task,     	
            (const char*    )"lcd_task",   	
            (uint16_t       )LCD_STK_SIZE, 
            (void*          )NULL,
            (UBaseType_t    )LCD_TASK_PRIO,
            (TaskHandle_t*  )&LCDTask_Handler);

    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

/*上位机信息解析任务函数*/
void cmd_get_task(void *pvParameters)
{
	u8 cmd[USART_REC_LEN];
	int i;
	BaseType_t err;
    while(1){
		for(i=0;i<USART_REC_LEN;i++)cmd[i]=0;
		err = xQueueReceive(CMD_Get_Queue,cmd,portMAX_DELAY);
		if(err==pdTRUE)printf("%s",&cmd[0]);
    }
}

/*mesh网信息解析任务函数*/
void mesh_get_task(void *pvParameters)
{

    while(1){

    }
}

/*网络信息上传任务函数*/
void msg_upload_task(void *pvParameters)
{
	while(1){
		/* code */
	}

}

/*指令上传任务函数*/
void cmd_upload_task(void *pvParameters)
{
	while(1){
		/* code */
	}
	
}

/*RTC测试任务*/
void rtc_task(void *pvParameters)
{

	while(RTC_Init())		//RTC初始化	，一定要初始化成功
	{ 
		LCD_ShowString(126,111,110,16,16,"RTC ERROR!   ");
		delay_ms(800);
		LCD_ShowString(126,111,110,16,16,"RTC Trying...");
	}
    vTaskDelete(RTCTask_Handler); //删除任务
	while(1)
	{

	}
}

/*人机交互测试*/
void lcd_task(void *pvParameters)
{
	u8 t;
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
