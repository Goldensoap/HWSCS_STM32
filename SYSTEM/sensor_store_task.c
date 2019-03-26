/*********************************************************************
 * 头文件
 */

#include "sensor_store_task.h"
/*********************************************************************
 * 全局变量
 */
TaskHandle_t Sensor_Store_Task_Handler;
SensorStore sensor_store;
/*********************************************************************
 * 本地变量
 */
#define SKIP	0  //队列获取跳过阻塞
/*********************************************************************
 * 本地函数
 */

void sensor_store_task(void *pvParameters)
{
    u32 NotifyValue;
    u32 timestamp;
    while(1){
        /* code */
        NotifyValue = ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
        if(NotifyValue==1){
            xQueuePeek(Time_Stamp_Queue,&timestamp,SKIP);
            sensor_store.address=SensorMsg.address;
            sensor_store.room=SensorMsg.room;
            sensor_store.sensorNum=SensorMsg.sensorNum;
            sensor_store.sensorType=SensorMsg.sensorType;
            sensor_store.data=SensorMsg.data;
            sensor_store.timestamp=timestamp;
        }
        
    }
    
}
