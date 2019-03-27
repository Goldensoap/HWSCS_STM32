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
 * 公共变量
 */
#define ROOM_MAX 255  // 房间数量上限，用于查询溢出
#define TYPE_MAX 255  // 传感器类型数量上限，用于查询溢出
#define SENSOR_MAX 65535 //传感器数量上限
#define DATA_MAX 10 //单设备存储数据条目上限
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

/*时间任务*/
#define RTC_TASK_PRIO			26              //任务优先级
#define RTC_STK_SIZE 			128             //任务堆栈大小	
extern TaskHandle_t RTCTask_Handler;     		//任务句柄
extern void rtc_task(void *pvParameters);		//任务函数

/*上位机命令解析任务*/
#define CMD_PARSE_TASK_PRIO		25              //任务优先级
#define CMD_PARSE_STK_SIZE 		128             //任务堆栈大小
extern TaskHandle_t CMD_ParseTask_Handler;     	//任务句柄
extern void cmd_parse_task(void *pvParameters);	//任务函数
/*mesh指令缓存任务*/
#define CMD_CACHE_TASK_PRIO		24              //任务优先级
#define CMD_CACHE_STK_SIZE 		128             //任务堆栈大小
extern TaskHandle_t CMD_Cache_Task_Handler;     //任务句柄
extern void cmd_cache_task(void *pvParameters);	//任务函数
/*指令上传mesh任务*/
#define CMD_UPLOAD_TASK_PRIO	23              //任务优先级
#define CMD_UPLOAD_STK_SIZE 	128             //任务堆栈大小
extern TaskHandle_t CMD_Upload_Task_Handler;    //任务句柄
extern void cmd_upload_task(void *pvParameters);//任务函数


/*信息上传任务*/
#define MSG_UPLOAD_TASK_PRIO	15              //任务优先级
#define MSG_UPLOAD_STK_SIZE 	128             //任务堆栈大小
extern TaskHandle_t Msg_Upload_Task_Handler;    //任务句柄
extern void msg_upload_task(void *pvParameters);//任务函数
/*传感信息调取任务*/
#define MSG_GET_TASK_PRIO       14              //任务优先级
#define MSG_GET_STK_SIZE        128             //任务堆栈大小
extern TaskHandle_t MSG_Get_Task_Handler;       //任务句柄
extern void MSG_Get_task(void *pvParameters);   //任务函数
/*mesh信息解析任务*/
#define MSG_PARSE_TASK_PRIO	    14              //任务优先级
#define MSG_PARSE_STK_SIZE 	    128             //任务堆栈大小
extern TaskHandle_t Msg_Parse_Task_Handler;     //任务句柄
extern void msg_parse_task(void *pvParameters); //任务函数
/*传感信息储存任务*/
#define SENSOR_STORE_TASK_PRIO  13              //任务优先级
#define SENSOR_STORE_STK_SIZE   128             //任务堆栈大小
extern TaskHandle_t Sensor_Store_Task_Handler;     //任务句柄
extern void sensor_store_task(void *pvParameters); //任务函数
/*路由信息储存任务*/
#define ROUTE_STORE_TASK_PRIO   12              //任务优先级
#define ROUTE_STORE_STK_SIZE    128             //任务堆栈大小
extern TaskHandle_t Route_Store_Task_Handler;     //任务句柄
extern void route_store_task(void *pvParameters); //任务函数

/*LCD人机交互任务*/
#define LCD_TASK_PRIO			0               //任务优先级
#define LCD_STK_SIZE 			128             //任务堆栈大小
extern TaskHandle_t LCDTask_Handler;     		//任务句柄
extern void lcd_task(void *pvParameters);		//任务函数
/*********************************************************************
 * 公用队列区
 */

/*队列定义*/
#define CMD_PARSE_Q_NUM	    5		//上位机命令获取消息队列的数量
#define CMD_PARSE_LEN  	    30      //队列单元长度 30Byte，也是最大接收字节数
extern QueueHandle_t CMD_Parse_Queue;	//上位机命令获取消息队列句柄

#define CMD_UPLOAD_Q_NUM    5      //控制指令上传队列
#define CMD_UPLOAD_LEN      3     //队列单元长度 3byte
extern QueueHandle_t CMD_Upload_Queue;

#define MSG_PARSE_Q_NUM     5      //mesh信息缓存队列
#define MSG_PARSE_LEN       30     //队列单元长度 50byte
extern QueueHandle_t Msg_Parse_Queue;

#define MSG_UPLOAD_Q_NUM    5      //信息和ACK上传队列
#define MSG_UPLOAD_LEN      30     //队列单元长度 50byte
extern QueueHandle_t Msg_Upload_Queue;

#define TIME_STAMP_Q_NUM    1      // 时间邮箱，长度1
#define TIME_STAMP_LEN      4      //u32 4字节长度
extern QueueHandle_t Time_Stamp_Queue;

/*********************************************************************
 * 结构体类型区
 */
typedef struct sensor_msg_from_mesh
{

    u16 address;
    u8  room;
    u8  sensorNum;
    u8  sensorType;
    u16  data;

}sensor_t;

typedef struct devive_status_from_mesh
{
    u8 totalroom;
    u8  totalsensor;
    u16 totaldata;
}device_t;

typedef struct route_msg_from_mesh
{
    
}route_t;

typedef struct cmd_for_control_mesh
{
    
}cmd_t;

/* 下面四个是嵌套链表结构体*/
typedef struct SensorData
{
    u8  count;
    u8  room;
    u8  sensorType;
    u32 sensorLabel;
    u16 data;
    u32 timestamp;
    struct SensorData *next;

}SensorData_t;

typedef struct SensorLabel
{
    
    u32 sensorLabel;
    struct SensorData *sensorData;
    struct SensorLabel *next;

}SensorLabel_t;

typedef struct SensorType
{
    
    u8 sensorType;
    struct SensorLabel *sensorLable;
    struct SensorType *next;
    
}SensorType_t;

typedef struct SpaceNum
{
   
    u8 space_num;
    struct SensorType *sensorType;
    struct SpaceNum *next;

}SpaceNum_t;

extern sensor_t SensorMsg;
extern SpaceNum_t *DataSheet;
extern device_t LCD_display;
#endif
