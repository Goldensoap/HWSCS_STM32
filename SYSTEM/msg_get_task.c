/*********************************************************************
 * 头文件
 */

#include "msg_get_task.h"
/*********************************************************************
 * 全局变量
 */
TaskHandle_t MSG_Get_Task_Handler; 
/*********************************************************************
 * 本地变量
 */
#define U32_MAX 0xffffffff
#define SENSOR  1
#define ROUTE   2
/*********************************************************************
 * 本地函数
 */

void MSG_Get_task(void *pvParameters)
{
	uint32_t NotifyValue=0;
	BaseType_t err;
    u8 Msg[MSG_UPLOAD_LEN];
    for(int i =0;i<MSG_UPLOAD_LEN;i++)Msg[i]=0;

    while(1){
        /* code */
		err=xTaskNotifyWait((uint32_t	)U32_MAX,
							(uint32_t	)U32_MAX,
							(uint32_t*	)&NotifyValue,
							(TickType_t )portMAX_DELAY);
        if(err==pdTRUE){
            switch ((NotifyValue&0x00ff0000)>>16)
            {
                case SENSOR:
                    /* code */
                    Msg[0]=sensor_store.sensorType;
                    Msg[1]=sensor_store.data>>8;
                    Msg[2]=sensor_store.data&0x00FF;
                    Msg[3]=sensor_store.room;
                    Msg[4]=sensor_store.sensorNum;
                    Msg[5]=sensor_store.timestamp>>24;
                    Msg[6]=(sensor_store.timestamp&0x00ff0000)>>16;
                    Msg[7]=(sensor_store.timestamp&0x0000ff00)>>8;
                    Msg[8]=sensor_store.timestamp&0x000000ff;
                    err = xQueueSend(Msg_Upload_Queue,Msg,100); //发送至信息上传队列
                    if(err == pdPASS)for(int i =0;i<9;i++)Msg[i]=0;
                    break;
                case ROUTE:
                    break;
                default:
                    break;
            }
        }
    }
    
}
