/*********************************************************************
 * 头文件
 */

#include "cmd_parse_task.h"
/*********************************************************************
 * 全局变量
 */
TaskHandle_t CMD_ParseTask_Handler;     	//任务句柄
QueueHandle_t CMD_Parse_Queue;	//上位机命令获取消息队列句柄
/*********************************************************************
 * 本地变量
 */

/*********************************************************************
 * 本地函数
 */

/*上位机信息解析任务函数*/
void cmd_parse_task(void *pvParameters)
{
	u8 cmd[CMD_PARSE_LEN];
	BaseType_t err;
		/*创建队列*/
	CMD_Parse_Queue=xQueueCreate(CMD_PARSE_Q_NUM,CMD_PARSE_LEN);
  	while(1){
		u8 identity = 0;
		for(int i=0;i<CMD_PARSE_LEN;i++)cmd[i]=0;
		err = xQueueReceive(CMD_Parse_Queue,cmd,portMAX_DELAY);
		/*验证身份*/
		(err==pdTRUE)&&(cmd[0]==0)&&(cmd[1]==1)&&(cmd[2]==2)&&(cmd[3]==3)&&(cmd[4]==4)&&(cmd[5]==5)&&(cmd[6]==6)&&(identity = 1);

		if(identity){
			if(cmd[7]=='T'){
				u32 seccount = 0;
				seccount = (cmd[8]<<24)+(cmd[9]<<16)+(cmd[10]<<8)+cmd[11];
				xTaskNotify(RTCTask_Handler,seccount,eSetValueWithOverwrite);
			}
			if(cmd[0]=='C'){

			}
			if(cmd[0]=='R'){
				
			}
		}
  	}
}
