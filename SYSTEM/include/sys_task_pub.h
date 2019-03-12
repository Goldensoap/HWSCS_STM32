#ifndef SYS_TASK_PUB_H
#define SYS_TASK_PUB_H

/*********************************************************************
 * 编译选项
 */
#define _DEBUG	1//debug 条件编译 1开启debug，0关闭

/*********************************************************************
 * 公用头文件
 */

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
#define START_TASK_PRIO			1               //任务优先级
#define START_STK_SIZE 			128             //任务堆栈大小
extern TaskHandle_t StartTask_Handler;     	    //任务句柄
extern void start_task(void *pvParameters);	    //任务函数
/*上位机命令解析任务*/
#define CMD_PARSE_TASK_PRIO		10              //任务优先级
#define CMD_PARSE_STK_SIZE 		128             //任务堆栈大小
extern TaskHandle_t CMD_ParseTask_Handler;     	//任务句柄
extern void cmd_parse_task(void *pvParameters);	//任务函数
/*时间任务*/
#define RTC_TASK_PRIO			11              //任务优先级
#define RTC_STK_SIZE 			256             //任务堆栈大小	
extern TaskHandle_t RTCTask_Handler;     		//任务句柄
extern void rtc_task(void *pvParameters);		//任务函数
/*LCD人机交互任务*/
#define LCD_TASK_PRIO			0               //任务优先级
#define LCD_STK_SIZE 			128             //任务堆栈大小
extern TaskHandle_t LCDTask_Handler;     		//任务句柄
extern void lcd_task(void *pvParameters);		//任务函数
/*信息上传任务*/
#define MSG_UPLOAD_TASK_PRIO	9               //任务优先级
#define MSG_UPLOAD_STK_SIZE 	128             //任务堆栈大小
extern TaskHandle_t Msg_Upload_Task_Handler;    //任务句柄
extern void msg_upload_task(void *pvParameters);//任务函数
/*********************************************************************
 * 公用队列区
 */

/*队列定义*/
#define CMD_PARSE_Q_NUM	5		//上位机命令获取消息队列的数量
#define CMD_REC_LEN  	50      //队列单元长度 50Byte，也是最大接收字节数
extern QueueHandle_t CMD_Get_Queue;	//上位机命令获取消息队列句柄

#define TIME_STAMP_Q_NUM 1      // 时间邮箱，长度1
#define TIME_STAMP_LEN   4      //u32 4字节长度
extern QueueHandle_t Time_Stamp_Queue;

#define MSG_UPLOAD_Q_NUM 5      //信息和ACK上传队列
#define MSG_UPLOAD_LEN   50     //队列单元长度 50byte
extern QueueHandle_t Msg_Upload_Queue;
#endif
