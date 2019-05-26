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
#define ALLTABLE    1
#define SENSOR      2
#define ROUTE       4
#define LCD         3

static void Send_Whole_Table( void );
static void Send_Spec_Room_And_Type( u8 roomNum, u8 typeNum );
static void LCD_Update( void );
/*********************************************************************
 * 本地函数
 */

void MSG_Get_task(void *pvParameters)
{
	uint32_t NotifyValue=0;
	BaseType_t err;

    while(1){
        /* code */
		err=xTaskNotifyWait((uint32_t	)U32_MAX,
							(uint32_t	)U32_MAX,
							(uint32_t*	)&NotifyValue,
							(TickType_t )portMAX_DELAY);
        if(err==pdTRUE){
            switch ((NotifyValue&0x00ff0000)>>16)
            {
                case ALLTABLE:
                    Send_Whole_Table();
                    break;
                case SENSOR:
				{
                    u8 roomNum = (NotifyValue&0x0000ff00)>>8;
                    u8 typeNum = NotifyValue&0x000000ff;
                    Send_Spec_Room_And_Type( roomNum, typeNum );
                    break;
				}
                case ROUTE:
                    break;
                case LCD:
                    LCD_Update();
					break;
                default:
                    break;
            }
        }
    }
}

static void Send_Whole_Table( void )
{
    BaseType_t err;
    char Msg[MSG_UPLOAD_LEN];
    SpaceNum_t *room = DataSheet;
    SensorType_t *type = NULL;
    SensorLabel_t *device = NULL;
    SensorData_t *data = NULL;
    while(room != NULL){
        type = room->sensorType;
        while(type != NULL){
            device = type->sensorLable;
            while(device != NULL){
                data = device->sensorData;

                sprintf(Msg,"{\"Type\":\"SENSOR\",\"Content\":{\"Space\":%u,\"Device\":\"%x\",\"Sensor\":%u,\"Type\":%u,\"Data\":%u,\"Time\":%u}}\r\n",\
                    room->space_num,\
                    (u16)((device->sensorLabel)&0x0000ffff),\
                    (u8)(((device->sensorLabel)&0x00ff0000)>>16),\
                    type->sensorType,\
                    (u16)(data->data),\
                    (u32)(data->timestamp));

                err = Msg_Upload_To_Host( Msg ); //发送至信息上传队列
                if(err != pdPASS){
                    
                };

                device = device->next;
            }
            type = type->next;
        }
        room = room->next;
    }
}

static void Send_Spec_Room_And_Type( u8 roomNum, u8 typeNum )
{
	BaseType_t err;
    char Msg[MSG_UPLOAD_LEN];

    SpaceNum_t *room_p = DataSheet;
    for( int i=0; i < ROOM_MAX; i++){
        if( room_p->space_num == roomNum){ //命中房间
            break;
        }else if(room_p->next == NULL){ //房间未找到

            #if _DEBUG
            printf("{\"Type\":\"DEBUG\",\"Content\":\"room not found\"}\r\n");
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

                sprintf(Msg,"{\"Type\":\"SENSOR\",\"Content\":{\"Space\":%u,\"Device\":\"%x\",\"Sensor\":%u,\"Type\":%u,\"Data\":%u,\"Time\":%u}}\r\n",\
                    roomNum,\
                    (u16)((device->sensorLabel)&0x0000ffff),\
                    (u8)(((device->sensorLabel)&0x00ff0000)>>16),\
                    typeNum,\
                    (u16)(data->data),\
                    (u32)(data->timestamp));

                err = Msg_Upload_To_Host( Msg ); //发送至信息上传队列
                device = device->next;
            }
            break;
        }else if( type_p->next == NULL ){ //类型未找到

            #if _DEBUG
            printf("{\"Type\":\"DEBUG\",\"Content\":\"type not found\"}\r\n");
            #endif

            break;
        }else type_p = type_p->next; //下一类型节点
    }

    if(err != pdPASS){

    };
}

static void LCD_Update( void )
{
    uint16_t data_count=0;
    uint8_t room_count=0;
    uint8_t sensor_count=0;
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
                sensor_count ++;
                device = device->next;
            }
            type = type->next;
        }
        room = room->next;
    }
    LCD_display.totalroom = room_count;
    LCD_display.totaldata = data_count;
    LCD_display.totalsensor = sensor_count;
}
