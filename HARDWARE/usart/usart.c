/*********************************************************************
 * 头文件
 */
/*系统配置*/
#include "sys.h"
#include "usart.h"
/*如果使用os,则包括下面的头文件即可.*/
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"
#include "queue.h"	  
#endif

/*********************************************************************
 * 全局变量
 */

extern QueueHandle_t CMD_Get_Queue;
/*********************************************************************
 * 本地变量
 */

/*接收缓冲,最大USART_REC_LEN个字节*/
static u8 USART1_RX_BUF[USART_REC_LEN];
static u8 USART2_RX_BUF[USART_REC_LEN];
/*接收状态标记：
 * bit15，	接收完成标志
 * bit14，	接收到0x0d
 * bit13~0，接收到的有效字节数目
*/
static u16 USART1_RX_STA=0;
static u16 USART2_RX_STA=0;

/*********************************************************************
 * 加入以下代码,支持printf函数,而不需要选择use MicroLIB  
*/

#pragma import(__use_no_semihosting)             
/*标准库需要的支持函数*/                 
struct __FILE 
{ 
	int handle;
}; 

FILE __stdout;       
/*定义_sys_exit()以避免使用半主机模式*/    
void _sys_exit(int x) 
{
	x = x;
}
/*重定义fputc函数 */
/*注意,读取USARTx->SR能避免莫名其妙的错误*/
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;
	return ch;
}

/*********************************************************************
 * 串口初始化配置
 */
void uart1_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟

	/*USART1_TX   GPIOA.9*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

	/*USART1_RX	  GPIOA.10初始化*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

	/*Usart1 NVIC 配置*/
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5 ;//抢占优先级5
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	/* USART1 工作参数配置:
        - BaudRate = 参数输入 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); 		//初始化串口1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//开启串口接受中断
	USART_ClearFlag(USART1,USART_FLAG_TC);			//清除串口1发送中断
	USART_Cmd(USART1, ENABLE);                   	//使能串口1 
}

void uart2_init(u32 bound)
{
	/*GPIO端口设置*/
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	//使能USART2，GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	/*USART2_TX   GPIOA.2*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2

	/*USART2_RX	  GPIOA.3初始化*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3 

	/*Usart2 NVIC 配置*/
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6 ;//抢占优先级6
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	/* USART2 工作参数配置:
        - BaudRate = 参数输入 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART2, &USART_InitStructure); //初始化串口2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_ClearFlag(USART2,USART_FLAG_TC);			//清除串口2发送中断
	USART_Cmd(USART2, ENABLE);                    //使能串口2
}

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
					if(USART1_RX_STA>(USART_REC_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收	  
				}
			}
		}
    }

	if((USART1_RX_STA&0x8000)&&(CMD_Get_Queue!=NULL)){
		xQueueSendFromISR(CMD_Get_Queue,USART1_RX_BUF,&xHigherPriorityTaskWoken);
		USART1_RX_STA=0;
		for(int i=0; i<USART_REC_LEN; i++)USART1_RX_BUF[i]=0;
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

/*串口2中断服务程序*/
void USART2_IRQHandler(void)            
{

}
