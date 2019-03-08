#include "sys.h"
#include "led.h"
#include "delay.h"
#include "timer.h"
#include "FreeRTOS.h"
#include "task.h"


#define START_TASK_PRIO		1       //任务优先级
#define START_STK_SIZE 		128     //任务堆栈大小	
TaskHandle_t StartTask_Handler;     //任务句柄
void start_task(void *pvParameters);//任务函数

#define UART1_TASK_PRIO		2       //任务优先级
#define UART1_STK_SIZE 		256     //任务堆栈大小	
TaskHandle_t UART1Task_Handler;     //任务句柄
void uart1_task(void *pvParameters);//任务函数

#define UART2_TASK_PRIO		3       //任务优先级
#define UART2_STK_SIZE 		128     //任务堆栈大小	
TaskHandle_t UART2Task_Handler;     //任务句柄
void uart2_task(void *pvParameters);//任务函数

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 	 
	delay_init();	    				//延时函数初始化	  
	uart1_init(115200);					//初始化串口
	uart2_init(115200);
	LED_Init();		  					//初始化LED

	/*创建开始任务*/
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

/*开始任务任务函数*/
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
        LED1=0;
        for(int i=0; i<10; i++){
             USART_SendData(USART2,TX[i]);
             while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET);
        }
        LED1=1;
        vTaskDelay(500);
    }
}
