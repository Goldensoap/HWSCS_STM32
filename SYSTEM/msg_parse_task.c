/*********************************************************************
 * 头文件
 */

#include "msg_parse_task.h"
/*********************************************************************
 * 全局变量
 */
TaskHandle_t Msg_Parse_Task_Handler;     	//任务句柄
QueueHandle_t Msg_Parse_Queue;	//mesh消息队列句柄
sensor_t SensorMsg;
/*********************************************************************
 * 本地变量
 */
#define SENSORDATA  1
#define ROUTEDATA   2
#define ACK         3
/*********************************************************************
 * 本地函数
 */

void msg_parse_task(void *pvParameters)
{
    u8 msg[MSG_PARSE_LEN];
    BaseType_t err;

    Msg_Parse_Queue=xQueueCreate(MSG_PARSE_Q_NUM,MSG_PARSE_LEN);//创建队列
    while(1){
        for(int i=0;i<MSG_PARSE_LEN;i++)msg[i]=0;
        err= xQueueReceive(Msg_Parse_Queue,msg,portMAX_DELAY);
        if(err==pdTRUE){
            switch (msg[0])
            {
                case SENSORDATA:
                    SensorMsg.address=(msg[1]<<8)+msg[2];
                    SensorMsg.room=msg[3];
                    SensorMsg.sensorNum=msg[4];
                    SensorMsg.sensorType=msg[5];
                    SensorMsg.data=(msg[6]<<8)+msg[7];
                    xTaskNotifyGive(Sensor_Store_Task_Handler);
                    break;
                case ROUTEDATA:
                    break;
                case ACK:
				{
                    uint32_t label;
                    err=pdFAIL;
                    label = msg[1];
                    while(err==pdFAIL){
                        err=xTaskNotify(CMD_Upload_Task_Handler,label,eSetValueWithoutOverwrite);
                    }
                    break;
				}
                default:
                    break;
            }
        }
    }
}
