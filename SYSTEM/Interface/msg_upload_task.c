/*********************************************************************
 * 头文件
 */

#include "msg_upload_task.h"
/*********************************************************************
 * 全局变量
 */
TaskHandle_t Msg_Upload_Task_Handler;     	//任务句柄
QueueHandle_t Msg_Upload_Queue;             //信息上传队列句柄
/*********************************************************************
 * 本地变量
 */
static u8 UPLOAD_BUF[MSG_UPLOAD_LEN];
/*********************************************************************
 * 本地函数
 */

void msg_upload_task(void *pvParameters)
{
    Msg_Upload_Queue=xQueueCreate(MSG_UPLOAD_Q_NUM,MSG_UPLOAD_LEN);
    BaseType_t err;
    while(1){
        for(int i=0;i<MSG_UPLOAD_LEN;i++){
            UPLOAD_BUF[i]=0;
        }
        err=xQueueReceive(Msg_Upload_Queue,UPLOAD_BUF,portMAX_DELAY);
        if(err==pdTRUE){
#if _DEBUG
            printf("%s\r\n",UPLOAD_BUF);
#endif
        }
    }
}
