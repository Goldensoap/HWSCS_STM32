/*********************************************************************
 * 头文件
 */

#include "sensor_store_task.h"
/*********************************************************************
 * 全局变量
 */
TaskHandle_t Sensor_Store_Task_Handler;
SpaceNum_t *DataSheet = NULL;
/*********************************************************************
 * 本地变量
 */

/*********************************************************************
 * 本地函数
 */

void sensor_store_task(void *pvParameters)
{
    u32 NotifyValue;
    /*初始化储存表头*/
    DataSheet = (SpaceNum_t  *)pvPortMalloc( sizeof(SpaceNum_t) );
    DataSheet->next = NULL;
    DataSheet->sensorType = NULL;
    DataSheet->space_num = 1;
    while(1){
        /* code */
        NotifyValue = ulTaskNotifyTake( pdTRUE,portMAX_DELAY );
        if( NotifyValue == 1 ){
            int i;
            SpaceNum_t *room = DataSheet;
            /*查询房间并选中*/
            for( i=0; i < ROOM_MAX; i++){
                if( room->space_num == SensorMsg.room ){ //命中房间
                    break;
                }else if( room->next == NULL ){ //房间未在表中，记录之
                    room->next = (SpaceNum_t  *)pvPortMalloc( sizeof(SpaceNum_t) );
                    room = room->next;
                    room->space_num = SensorMsg.room;
                    room->sensorType = NULL;
                    room->next = NULL;
                    break;
                }else{ //按链搜寻房间
                    room = room->next;
                }
            }
            if(room != NULL){ //房间未溢出
                /*查询类型并选中*/
                SensorType_t *type = NULL;
                if( room->sensorType == NULL ){ //传感类型首节点初始化
                    room->sensorType = (SensorType_t *)pvPortMalloc( sizeof(SensorType_t) );
                    room->sensorType->next = NULL;
                    room->sensorType->sensorType = SensorMsg.sensorType;
                    room->sensorType->sensorLable = NULL;
                }
                type = room->sensorType;

                for( i=0; i < TYPE_MAX; i++ ){
                    if( type->sensorType == SensorMsg.sensorType ){ // 命中类型
                        break;
                    }else if( type->next == NULL ){ //类型未在房间中，记录之
                        type->next = (SensorType_t *)pvPortMalloc( sizeof(SensorType_t) );
                        type = type->next;
                        type->sensorType = SensorMsg.sensorType;
                        type->sensorLable = NULL;
                        type->next = NULL;
                        break;
                    }else{  //按链搜寻类型
                        type = type->next;
                    }
                }
                if( type != NULL ){ //类型未溢出
                    /*查询设备并选中*/
                    uint32_t label;
                    label = SensorMsg.address + (SensorMsg.sensorNum<<16);
                    SensorLabel_t *labelp = NULL;
                    if( type->sensorLable == NULL ){ //设备首节点初始化
                        type->sensorLable = (SensorLabel_t *)pvPortMalloc( sizeof(SensorLabel_t) );
                        type->sensorLable->next = NULL;
                        type->sensorLable->sensorLabel = label;
                        type->sensorLable->sensorData = NULL;
                    }
                    labelp = type->sensorLable;

                    for( i=0; i < SENSOR_MAX; i++ ){
                        if( labelp->sensorLabel == label ){ //命中设备
                            break;
                        }else if( labelp->next == NULL ){ //设备未在房间中，记录之
                            labelp->next = (SensorLabel_t *)pvPortMalloc( sizeof(SensorLabel_t) );
                            labelp = labelp->next;
                            labelp->sensorLabel = label;
                            labelp->sensorData = NULL;
                            labelp->next = NULL;
                            break;
                        }else{  //按链表搜寻设备
                            labelp = labelp->next;
                        }
                    }
                    if( labelp != NULL ){ //设备未溢出
                        /*头插法插入数据*/
                        u32 timestamp;
                        xQueuePeek( Time_Stamp_Queue,&timestamp,SKIP );
                        if( labelp->sensorData == NULL ){  //头数据初始化
                            labelp->sensorData = (SensorData_t *)pvPortMalloc( sizeof(SensorData_t) );
                            labelp->sensorData->count = 1;
                            labelp->sensorData->data = SensorMsg.data;
                            labelp->sensorData->next = NULL;
                            labelp->sensorData->timestamp = timestamp;
                        }else if( labelp->sensorData->count < DATA_MAX ){ //头插法插入新数据
                            SensorData_t *data = (SensorData_t *)pvPortMalloc( sizeof(SensorData_t) );
                            data->next = labelp->sensorData;
                            data->count = (labelp->sensorData->count) + 1; //条目计数+1
                            labelp->sensorData = data;

                            data->data = SensorMsg.data;
                            data->timestamp = timestamp;
                        }else{  //更新头数据，并删除多余尾数据
                            SensorData_t *data = (SensorData_t *)pvPortMalloc( sizeof(SensorData_t) );
                            data->next = labelp->sensorData;
                            data->count = labelp->sensorData->count;
                            labelp->sensorData = data;

                            data->data = SensorMsg.data;
                            data->timestamp = timestamp;

                            SensorData_t *end = NULL; // 删除尾节点
                            end = data->next;
                            while(end->next==NULL){
                                end = end->next;
                            }
                            vPortFree(end);
                        }
                    }
#if _DEBUG
                    else printf("DEBUG:device overflow\r\n");
#endif
                }
#if _DEBUG
                else printf("DEBUG:type overflow\r\n");
#endif
            }
#if _DEBUG
            else printf("DEBUG:room overflow\r\n");
#endif
        }
    }
}
