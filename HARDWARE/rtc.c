#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "rtc.h" 		    


static void RTC_NVIC_Config(void)
{	
    NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTC全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;	//先占优先级0位
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//先占优先级0位,从优先级0位
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//使能该通道中断
	NVIC_Init(&NVIC_InitStructure);		//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

//实时时钟配置
//初始化RTC时钟,同时检测时钟是否工作正常
//BKP->DR1用于保存是否第一次配置的设置
//返回0:正常
//其他:错误代码

u8 RTC_Init(u32 seccount)
{
	//检查是不是第一次配置时钟
	u8 temp=0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟   
	PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问  
	if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050){		//从指定的后备寄存器中读出数据:读出了与写入的指定数据不相乎

		BKP_DeInit();	//复位备份区域 	
		RCC_LSEConfig(RCC_LSE_ON);	//设置外部低速晶振(LSE),使用外设低速晶振
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET){	//检查指定的RCC标志位设置与否,等待低速晶振就绪
			temp++;
			delay_ms(10);
		}
		if(temp>=250)return 1;//初始化时钟失败,晶振有问题	    
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		RTC_WaitForSynchro();		//等待RTC寄存器同步
		RTC_ITConfig(RTC_IT_SEC, ENABLE);//使能RTC秒中断
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		RTC_EnterConfigMode();// 允许配置
		RTC_SetPrescaler(32767); //设置RTC预分频的值
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟  
		PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问
		RTC_SetCounter(seccount);	//设置RTC计数器的值
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成  	
		RTC_ExitConfigMode(); //退出配置模式
		BKP_WriteBackupRegister(BKP_DR1, 0X5050);	//向指定的后备寄存器中写入用户程序数据
	}else{	//系统继续计时
		RTC_WaitForSynchro();	//等待最近一次对RTC寄存器的写操作完成
		RTC_ITConfig(RTC_IT_SEC, ENABLE);	//使能RTC秒中断
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
	}
	RTC_NVIC_Config();//RCT中断分组设置
	return 0; //ok
}
