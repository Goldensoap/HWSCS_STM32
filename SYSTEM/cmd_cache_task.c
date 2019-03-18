/*********************************************************************
 * 头文件
 */

#include "cmd_cache_task.h"
/*********************************************************************
 * 全局变量
 */
TaskHandle_t CMD_Cache_Task_Handler;     	//任务句柄
/*********************************************************************
 * 本地变量
 */
#define U32_MAX 0xffffffff
/*********************************************************************
 * 本地函数
 */

void cmd_cache_task(void *pvParameters)
{
    uint32_t NotifyValue=0;
    BaseType_t err;
    u8 ack[MSG_UPLOAD_LEN];
    for(int i=0;i<MSG_UPLOAD_LEN;i++)ack[i]=0;

    while(1){
		err=xTaskNotifyWait((uint32_t	)U32_MAX,
							(uint32_t	)U32_MAX,
							(uint32_t*	)&NotifyValue,
							(TickType_t )portMAX_DELAY);
        if(err==pdTRUE){
            u8 cmd[3];
            cmd[0]=NotifyValue>>24;
            cmd[1]=(NotifyValue&0x00ff0000)>>16;
            cmd[2]=(NotifyValue&0x0000ff00)>>8;
            err = xQueueSend(CMD_Upload_Queue,cmd,0); //发送至指令上传队列
            if(err == errQUEUE_FULL){
#if _DEBUG
            printf("cmd_upload_que,full\r\n");
#endif
            }
            ack[0]=(NotifyValue&0x000000ff);
            xQueueSend(Msg_Upload_Queue,ack,100);//发送ACK至信息上传队列
        }
    }
}
