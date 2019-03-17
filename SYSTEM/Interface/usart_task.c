/*********************************************************************
 * 头文件
 */

#include "usart_task.h"

/*********************************************************************
 * 全局变量
 */

/*********************************************************************
 * 本地变量
 */

#define SOH 0x01
#define EOT 0x04
#define ESC 0x1B
#define Rev_SOH 0x01
#define Rev_EOT 0x02
#define Set_ESC 0x04
#define Clear_ESC 0xFB
#define Init_STA 0

/*接收缓冲,最大CMD_PARSE_LEN个字节*/
static u8 USART1_RX_BUF[CMD_PARSE_LEN];
static u8 USART2_RX_BUF[MSG_PARSE_LEN];
/*接收状态标记： 
 * bit4-8   冗余
 * bit3		接收到ESC（0x1B） 0 → 1
 * bit2，	接收到EOT（0x04） 0 → 1
 * bit1，   接收到SOH（0x01） 0 → 1
*/
static u8 USART1_RX_STA=Init_STA;
static u8 USART2_RX_STA=Init_STA;
/*计数*/
static u8 USART1_RX_CONTER=0;
static u8 USART2_RX_CONTER=0;

/*********************************************************************
 * 串口中断服务程序
 */
/*串口1中断服务程序*/
void USART1_IRQHandler(void)                	
{
	u8 ReceiveByte;
	BaseType_t xHigherPriorityTaskWoken;
	/*接收报文*/
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){  //接收中断
		ReceiveByte =USART_ReceiveData(USART1);	//读取接收到的数据
		if(		(ReceiveByte == SOH) && (USART1_RX_STA & Set_ESC)==0)USART1_RX_STA |= Rev_SOH; //接收到帧头
		else if((ReceiveByte == EOT) && (USART1_RX_STA & Set_ESC)==0)USART1_RX_STA |= Rev_EOT; //接收到帧尾
		else if((ReceiveByte == ESC) && (USART1_RX_STA & Set_ESC)==0)USART1_RX_STA |= Set_ESC;//接收到转义字符
		else if(USART1_RX_STA & Rev_SOH){ //接收报文
			USART1_RX_BUF[USART1_RX_CONTER]=ReceiveByte;
			USART1_RX_CONTER++;
			if(USART1_RX_CONTER>(CMD_PARSE_LEN-1)){//接收字符超过上限
				USART2_RX_STA=Init_STA; //清零状态
				USART1_RX_CONTER=0;
				for(int i=0; i<CMD_PARSE_LEN; i++)USART1_RX_BUF[i]=0;
			}
			USART1_RX_STA &= Clear_ESC;
		}
 	}
	/*CRC校验匹配和消息发送*/
	if((USART1_RX_STA& Rev_EOT)&&(CMD_Parse_Queue!=NULL)){ 
		uint32_t temp = 0;
		u8 count=0;
		CRC_ResetDR(); //初始化CRC 余数
		for(int i=(USART1_RX_CONTER-5);i>=0;i--){//输入报文内容计算
			temp += (USART1_RX_BUF[i]<<(count*8));
			if(count==3 || i==0){
				CRC_CalcCRC(temp);
				temp = 0;
				count = 0;
			}else count++;
		}
		for(int i=1;i<5;i++){
			temp += (USART1_RX_BUF[USART1_RX_CONTER-i]<<(count*8));
			count++;
		}
		CRC_CalcCRC(temp); //输入CRC余数
		temp = CRC_GetCRC();//取得计算结果

		if(temp == 0)xQueueSendFromISR(CMD_Parse_Queue,USART1_RX_BUF,&xHigherPriorityTaskWoken); //校验成功发送至队列
		USART1_RX_STA=Init_STA; //清空标志
		USART1_RX_CONTER=0;
		for(int i=0; i<CMD_PARSE_LEN; i++)USART1_RX_BUF[i]=0;//清空队列
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//进行上下文切换
	}
}

/*串口2中断服务程序*/
void USART2_IRQHandler(void)            
{
	u8 ReceiveByte;
	BaseType_t xHigherPriorityTaskWoken;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		ReceiveByte =USART_ReceiveData(USART2);	//读取接收到的数据
		if((USART2_RX_STA&0x8000)==0){//接收未完成
			if(USART2_RX_STA&0x4000){//接收到了0x0d
				if(ReceiveByte!=0x0a)USART2_RX_STA=0;//接收错误,重新开始
				else{
					USART2_RX_STA|=0x8000;	//接收完成了
				}
			}
			else{ //还没收到0X0D
				if(ReceiveByte==0x0d)USART2_RX_STA|=0x4000;
				else{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=ReceiveByte ;
					USART2_RX_STA++;
					if(USART2_RX_STA>(CMD_PARSE_LEN-1))USART2_RX_STA=0;//接收数据错误,重新开始接收	  
				}
			}
		}
  	}

	if((USART2_RX_STA&0x8000)&&(Msg_Parse_Queue!=NULL)){
		xQueueSendFromISR(Msg_Parse_Queue,USART2_RX_BUF,&xHigherPriorityTaskWoken);
		USART2_RX_STA=0;
		for(int i=0; i<MSG_PARSE_LEN; i++)USART2_RX_BUF[i]=0;
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}
