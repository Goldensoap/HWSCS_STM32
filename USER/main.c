/*********************************************************************
 * 头文件
 */

/*系统公用*/
#include "sys_task_pub.h"
/*硬件接口任务*/
#include "hardware_init.h"

/*********************************************************************
 * 全局变量
 */
TaskHandle_t StartTask_Handler;
/*********************************************************************
 * 本地变量
 */

/*********************************************************************
 * 主函数
 */
int main(void)
{
    /*系统初始化*/
    hardware_init();

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
 * 本地函数
 */

/*启动任务*/
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区

    /*创建上位机信息解析任务*/
    xTaskCreate((TaskFunction_t )cmd_parse_task,
                (const char*    )"cmd_get_task",
                (uint16_t       )CMD_PARSE_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )CMD_PARSE_TASK_PRIO,
                (TaskHandle_t*  )&CMD_ParseTask_Handler);
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

    /*创建信息上传测试任务*/
    xTaskCreate((TaskFunction_t )msg_upload_task,
            (const char*    )"msg_upload_task",
            (uint16_t       )MSG_UPLOAD_STK_SIZE,
            (void*          )NULL,
            (UBaseType_t    )MSG_UPLOAD_TASK_PRIO,
            (TaskHandle_t*  )&Msg_Upload_Task_Handler);

    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}
