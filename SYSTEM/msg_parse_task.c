/*********************************************************************
 * 头文件
 */

#include "cmd_parse_task.h"
/*********************************************************************
 * 全局变量
 */
TaskHandle_t Msg_Parse_Task_Handler;     	//任务句柄
QueueHandle_t Msg_Parse_Queue;	//上位机命令获取消息队列句柄
/*********************************************************************
 * 本地变量
 */

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
            if(msg[0]=='R'){
#if _DEBUG
            printf("%s",msg);
#endif
            }
        }
    }
}
