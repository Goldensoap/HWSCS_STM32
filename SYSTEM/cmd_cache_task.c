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
struct  //指令缓存结构体
{
    /* data */
    uint32_t cmd;
    uint32_t timestamp;

}cmd_cache[5];

/*********************************************************************
 * 本地函数
 */

void cmd_cache_task(void *pvParameters)
{
    uint32_t NotifyValue=0;
    uint32_t TimeStamp;
    BaseType_t err;
    u8 ack[1];
    u8 cmd[4];
    
    for( int i=0; i < 5; i++ ){ //初始化缓存
        cmd_cache[i].cmd = 0;
        cmd_cache[i].timestamp = 0;
    }
    
    while(1){
		err=xTaskNotifyWait((uint32_t	)U32_MAX,
							(uint32_t	)U32_MAX,
							(uint32_t*	)&NotifyValue,
							(TickType_t )500);
        if(err==pdTRUE){ //接收到通知
            if(NotifyValue<256){ //mesh回传标签
                
                for( int i=0; i < 5; i++ ){ //删除对应缓存指令
                    if( NotifyValue==((cmd_cache[i].cmd)&0x000000ff )){
                        cmd_cache[i].cmd = 0;
                        cmd_cache[i].timestamp = 0;
                        break;
                    }
                }
                ack[0]=(uint8_t)NotifyValue;
                xQueueSend(Msg_Upload_Queue,ack,100);//发送ACK至信息上传队列
            }else{ //上位机指令，先对比缓存，若重复则更新时间戳

                cmd[0]=NotifyValue>>24;
                cmd[1]=(NotifyValue&0x00ff0000)>>16;
                cmd[2]=(NotifyValue&0x0000ff00)>>8;
                cmd[3]=NotifyValue&0x000000ff;
                err = xQueueSend(CMD_Upload_Queue,cmd,0); //发送至指令上传队列
                xQueuePeek(Time_Stamp_Queue,&TimeStamp,SKIP);
                for( int i=0; i<5; i++ ){ //命令缓存，未做溢出异常处理
                    if( (cmd_cache[i].cmd = NotifyValue) || (cmd_cache[i].timestamp == 0) ){
                        cmd_cache[i].cmd = NotifyValue;
                        cmd_cache[i].timestamp = TimeStamp;
                        break;
                    }
                }

                if(err == errQUEUE_FULL){
                    #if _DEBUG
                    printf("DEBUG:cmd_upload_que full\r\n");
                    #endif
                }
            }
        }else{ //超时未接收到通知，检查缓存
            xQueuePeek(Time_Stamp_Queue,&TimeStamp,SKIP);
            for(int i=0; i<5 ;i++){ //缓存处理
                if( cmd_cache[i].timestamp != 0 ){
                    if( (TimeStamp - cmd_cache[i].timestamp)>=3 ){ //超时3秒，清除，未作异常处理
                        cmd_cache[i].cmd = 0;
                        cmd_cache[i].timestamp = 0;
                    #if _DEBUG
                        printf("DEBUG:cmd ack time out...clean\r\n");
                    #endif
                    }else if( TimeStamp != cmd_cache[i].timestamp ){ //相差1至2秒，重发指令
                        cmd[0]=(cmd_cache[i].cmd)>>24;
                        cmd[1]=((cmd_cache[i].cmd)&0x00ff0000)>>16;
                        cmd[2]=((cmd_cache[i].cmd)&0x0000ff00)>>8;
                        cmd[3]=(cmd_cache[i].cmd)&0x000000ff;
                        err = xQueueSend(CMD_Upload_Queue,cmd,0); //发送至指令上传队列
                    #if _DEBUG
                        printf("DEBUG:cmd resent...%d\r\n",cmd_cache[i].cmd);
                    #endif
                    }
                }
            }
        }
    }
}
