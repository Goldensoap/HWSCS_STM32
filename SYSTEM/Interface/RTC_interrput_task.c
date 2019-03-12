/*********************************************************************
 * 头文件
 */
#include "RTC_interrput_task.h"

/*********************************************************************
 * 全局变量
 */

/*********************************************************************
 * 本地变量
 */

/*********************************************************************
 * 本地函数声明
 */

/*********************************************************************
 * 本地函数
 */

/*RTC时钟中断每秒触发一次*/
void RTC_IRQHandler(void)
{
	u32 timestamp;
	BaseType_t xHigherPriorityTaskWoken;

	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)//秒钟中断
	{
		timestamp=RTC_Get();//更新时间
		if(Time_Stamp_Queue!=NULL){
			xQueueOverwriteFromISR(Time_Stamp_Queue,&timestamp,&xHigherPriorityTaskWoken);
		}
 	}
	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//闹钟中断
	{
		RTC_ClearITPendingBit(RTC_IT_ALR);		//清闹钟中断	   
  	}
	RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);		//清闹钟中断
	RTC_WaitForLastTask();

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

u32 RTC_Get(void)
{
	u32 timecount;
    timecount=RTC_GetCounter();
	return timecount;
}

u8 RTC_Set_Timestamp(u32 timestamp)
{
	RTC_EnterConfigMode();// 允许配置
	RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
	PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问
	RTC_SetCounter(timestamp);	//设置RTC计数器的值
	RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
	RTC_ExitConfigMode(); //退出配置模式
	return 0;
}
