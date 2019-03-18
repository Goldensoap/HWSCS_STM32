/*********************************************************************
 * 头文件
 */

#include "cmd_upload_task.h"
/*********************************************************************
 * 全局变量
 */
TaskHandle_t CMD_Upload_Task_Handler;     	//任务句柄
QueueHandle_t CMD_Upload_Queue;             //信息上传队列句柄
/*********************************************************************
 * 本地变量
 */
static u8 UPLOAD_BUF[CMD_UPLOAD_LEN];
/*********************************************************************
 * 本地函数
 */

void cmd_upload_task(void *pvParameters)
{
    CMD_Upload_Queue=xQueueCreate(CMD_UPLOAD_Q_NUM,CMD_UPLOAD_LEN);
    BaseType_t err;
    while(1){
        for(int i=0;i<CMD_UPLOAD_LEN;i++){
            UPLOAD_BUF[i]=0;
        }
        err=xQueueReceive(CMD_Upload_Queue,UPLOAD_BUF,portMAX_DELAY);
        if(err==pdTRUE){
#if _DEBUG
            for(int i=0;i<CMD_UPLOAD_LEN;i++){
                USART_SendData(USART2,UPLOAD_BUF[i]);
                while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
            }
#endif
        }
    }
}
