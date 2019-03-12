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

/*接收缓冲,最大CMD_PARSE_LEN个字节*/
static u8 USART1_RX_BUF[CMD_PARSE_LEN];
static u8 USART2_RX_BUF[MSG_PARSE_LEN];
/*接收状态标记：
 * bit15，	接收完成标志
 * bit14，	接收到0x0d
 * bit13~0，接收到的有效字节数目
*/
static u16 USART1_RX_STA=0;
static u16 USART2_RX_STA=0;

/*********************************************************************
 * 串口中断服务程序
 */
/*串口1中断服务程序*/
void USART1_IRQHandler(void)                	
{
	u8 ReceiveByte;
	BaseType_t xHigherPriorityTaskWoken;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		ReceiveByte =USART_ReceiveData(USART1);	//读取接收到的数据
		if((USART1_RX_STA&0x8000)==0){//接收未完成
			if(USART1_RX_STA&0x4000){//接收到了0x0d
				if(ReceiveByte!=0x0a)USART1_RX_STA=0;//接收错误,重新开始
				else{
					USART1_RX_STA|=0x8000;	//接收完成了
				}
			}
			else{ //还没收到0X0D
				if(ReceiveByte==0x0d)USART1_RX_STA|=0x4000;
				else{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=ReceiveByte ;
					USART1_RX_STA++;
					if(USART1_RX_STA>(CMD_PARSE_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收	  
				}
			}
		}
  }

	if((USART1_RX_STA&0x8000)&&(CMD_Parse_Queue!=NULL)){
		xQueueSendFromISR(CMD_Parse_Queue,USART1_RX_BUF,&xHigherPriorityTaskWoken);
		USART1_RX_STA=0;
		for(int i=0; i<CMD_PARSE_LEN; i++)USART1_RX_BUF[i]=0;
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
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
