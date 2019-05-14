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

QueueHandle_t Msg_Upload_Queue;         //信息上传队列句柄
QueueHandle_t Msg_Parse_Queue;	        //mesh消息队列句柄
QueueHandle_t Time_Stamp_Queue;			//队列句柄
QueueHandle_t CMD_Parse_Queue;	        //上位机命令获取消息队列句柄
QueueHandle_t CMD_Upload_Queue;         //信息上传队列句柄
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

    Msg_Upload_Queue=xQueueCreate(MSG_UPLOAD_Q_NUM,MSG_UPLOAD_LEN); //创建信息上传队列
    Time_Stamp_Queue=xQueueCreate(TIME_STAMP_Q_NUM,TIME_STAMP_LEN); //创建时间邮箱
    Msg_Parse_Queue=xQueueCreate(MSG_PARSE_Q_NUM,MSG_PARSE_LEN);    //创建传感网络信息接收队列
    CMD_Parse_Queue=xQueueCreate(CMD_PARSE_Q_NUM,CMD_PARSE_LEN);    //创建上位机命令接收队列
    CMD_Upload_Queue=xQueueCreate(CMD_UPLOAD_Q_NUM,CMD_UPLOAD_LEN); //创建上位机命令上传队列

    /*创建上位机指令解析任务*/
    xTaskCreate((TaskFunction_t )cmd_parse_task,
                (const char*    )"cmd_parse_task",
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

    /*创建mesh信息解析任务*/
    xTaskCreate((TaskFunction_t )msg_parse_task,
            (const char*    )"msg_parse_task",
            (uint16_t       )MSG_PARSE_STK_SIZE,
            (void*          )NULL,
            (UBaseType_t    )MSG_PARSE_TASK_PRIO,
            (TaskHandle_t*  )&Msg_Parse_Task_Handler);

    /*创建信息调取任务*/
    xTaskCreate((TaskFunction_t )MSG_Get_task,
            (const char*    )"MSG_get_task",
            (uint16_t       )MSG_GET_STK_SIZE,
            (void*          )NULL,
            (UBaseType_t    )MSG_GET_TASK_PRIO,
            (TaskHandle_t*  )&MSG_Get_Task_Handler);

    /*创建传感信息储存任务*/
    xTaskCreate((TaskFunction_t )sensor_store_task,
            (const char*    )"MSG_get_task",
            (uint16_t       )SENSOR_STORE_STK_SIZE,
            (void*          )NULL,
            (UBaseType_t    )SENSOR_STORE_TASK_PRIO,
            (TaskHandle_t*  )&Sensor_Store_Task_Handler);

    /*创建指令上传任务*/
    xTaskCreate((TaskFunction_t )cmd_upload_task,
            (const char*    )"cmd_upload_task",
            (uint16_t       )CMD_UPLOAD_STK_SIZE,
            (void*          )NULL,
            (UBaseType_t    )CMD_UPLOAD_TASK_PRIO,
            (TaskHandle_t*  )&CMD_Upload_Task_Handler);

    /*创建mesh指令缓存任务*/
    xTaskCreate((TaskFunction_t )cmd_cache_task,
            (const char*    )"cmd_cache_task",
            (uint16_t       )CMD_CACHE_STK_SIZE,
            (void*          )NULL,
            (UBaseType_t    )CMD_CACHE_TASK_PRIO,
            (TaskHandle_t*  )&CMD_Cache_Task_Handler);

    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}
