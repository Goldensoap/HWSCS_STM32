#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

/*********************************************************************
 * 宏定义
 */

/*定义最大接收字节数 50*/
#define USART_REC_LEN  			50  	


/*********************************************************************
 * 函数声明
 */
void uart1_init(u32 bound);
void uart2_init(u32 bound);
#endif


