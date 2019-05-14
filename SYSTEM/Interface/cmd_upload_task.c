/*********************************************************************
 * 头文件
 */

#include "cmd_upload_task.h"
/*********************************************************************
 * 全局变量
 */
TaskHandle_t CMD_Upload_Task_Handler;     	//任务句柄
/*********************************************************************
 * 本地变量
 */
static u8 UPLOAD_BUF[CMD_UPLOAD_LEN];
/*********************************************************************
 * 本地函数
 */

void cmd_upload_task(void *pvParameters)
{
    BaseType_t err;
    while(1){
        for(int i=0;i<CMD_UPLOAD_LEN;i++){
            UPLOAD_BUF[i]=0;
        }
        err=xQueueReceive(CMD_Upload_Queue,UPLOAD_BUF,portMAX_DELAY);
        if(err==pdTRUE){
            for(int i=0;i<CMD_UPLOAD_LEN;i++){
                USART_SendData(USART2,UPLOAD_BUF[i]);
                while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
            }
        }
    }
}
