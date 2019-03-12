/*********************************************************************
 * 头文件
 */
#include "RTC_task.h"
#include "RTC_interrput_task.h"
/*********************************************************************
 * 全局变量
 */
TaskHandle_t RTCTask_Handler;     		//任务句柄
QueueHandle_t Time_Stamp_Queue;			//队列句柄
/*********************************************************************
 * 本地变量
 */

#define U32_MAX 0xffffffff
/*********************************************************************
 * 本地函数
 */

/*RTC测试任务*/
void rtc_task(void *pvParameters)
{
	uint32_t NotifyValue=0;
	BaseType_t err;

	Time_Stamp_Queue=xQueueCreate(TIME_STAMP_Q_NUM,TIME_STAMP_LEN);
	while(1)
	{
		err=xTaskNotifyWait((uint32_t	)U32_MAX,
							(uint32_t	)U32_MAX,
							(uint32_t*	)&NotifyValue,
							(TickType_t )portMAX_DELAY);
		if(err==pdTRUE){
			if(NotifyValue==RTC_Get()){ //对比RTC时间戳
				NotifyValue=0;
#if _DEBUG
			    printf("TimeStamp same....");
#endif
			}else{ 				//若值不同则进行同步
				RTC_Set_Timestamp(NotifyValue);
#if _DEBUG
			    printf("TimeStamp syn success....");
			    printf("%d",NotifyValue);
#endif
				NotifyValue=0;
			}
		}
	}
}
