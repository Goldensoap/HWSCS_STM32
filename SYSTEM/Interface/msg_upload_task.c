/*********************************************************************
 * 头文件
 */

#include "msg_upload_task.h"
/*********************************************************************
 * 全局变量
 */
TaskHandle_t Msg_Upload_Task_Handler;     	//任务句柄
/*********************************************************************
 * 本地变量
 */
static u8 UPLOAD_BUF[MSG_UPLOAD_LEN];
/*********************************************************************
 * 本地函数
 */

void msg_upload_task(void *pvParameters)
{
    BaseType_t result;
    while(1){
        for(int i=0;i<MSG_UPLOAD_LEN;i++)UPLOAD_BUF[i]=0;
        result=Receive_From_Queue( UPLOAD_BUF );
        if(result==pdTRUE){

            Upload_To_Host( UPLOAD_BUF );

        }
    }
}
