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
#define LCD     3
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
				{
                    u8 roomNum = (NotifyValue&0x0000ff00)>>8;
                    u8 typeNum = NotifyValue&0x000000ff;
                    SpaceNum_t *room_p = DataSheet;
                    for( int i=0; i < ROOM_MAX; i++){
                        if( room_p->space_num == roomNum){ //命中房间
                            break;
                        }else if(room_p->next == NULL){ //房间未找到
#if _DEBUG
                            printf("DEBUG:room not found\r\n");
#endif
                            break;
                        }else room_p = room_p->next; //下一房间节点
                    }
                    SensorType_t *type_p = room_p->sensorType;
                    for( int j=0; j<TYPE_MAX; j++){
                        if( type_p->sensorType == typeNum ){ //命中类型
                            SensorLabel_t *device = type_p->sensorLable;
                            SensorData_t *data = NULL;
                            while(device != NULL){ //输出类型下所有设备的最新数据
                                data = device->sensorData;
                                Msg[0]=data->sensorType;    // 传感类型
                                Msg[1]=data->data>>8;       // 传感数据 高位
                                Msg[2]=data->data&0x00FF;   // 传感数据 低位
                                Msg[3]=data->room;          // 传感器房间号
                                Msg[4]=(data->sensorLabel&0x00ff0000)>>16;  //传感器编号 大端
                                Msg[5]=(data->sensorLabel&0x0000ff00)>>8;   //传感器编号
                                Msg[6]=data->sensorLabel&0x000000ff;        //传感器编号
                                Msg[7]=data->timestamp>>24;                 //时间戳，大端
                                Msg[8]=(data->timestamp&0x00ff0000)>>16;    //时间戳
                                Msg[9]=(data->timestamp&0x0000ff00)>>8;     //时间戳
                                Msg[10]=data->timestamp&0x000000ff;         //时间戳
                                err = xQueueSend(Msg_Upload_Queue,Msg,100); //发送至信息上传队列
                                device = device->next;
                            }
                            break;
                        }else if( type_p->next == NULL ){ //类型未找到
#if _DEBUG
                            printf("DEBUG:type not found\r\n");
#endif
                            break;
                        }else type_p = type_p->next; //下一类型节点
                    }
                    if(err == pdPASS)for(int i =0;i<11;i++)Msg[i]=0;
                    break;
				}
                case ROUTE:
                    break;
                case LCD:
                {
                    uint16_t data_count=0;
                    uint8_t room_count=0;
                    SpaceNum_t *room = DataSheet;
                    SensorType_t *type = NULL;
                    SensorLabel_t *device = NULL;
                    SensorData_t *data = NULL;
                    while(room != NULL){
                        room_count ++;
                        type = room->sensorType;
                        while(type != NULL){
                            device = type->sensorLable;
                            while(device != NULL){
                                data = device->sensorData;
                                data_count += data->count;
                                device = device->next;
                            }
                            type = type->next;
                        }
                        room = room->next;
                    }
                    LCD_display.totalroom = room_count;
                    LCD_display.totaldata = data_count;
					break;
                }
                default:
                    break;
            }
        }
    }
}
