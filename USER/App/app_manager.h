#ifndef __APP_MANAGER_H__
#define __APP_MANAGER_H__

#include "main.h"

/* 应用层初始化和任务函数 */
void App_Init(void);
void App_Task(void);

/* LED相关函数 */
void LED_Init(void);
void LED_Task(void);

void KEY_Init(void);
void KEY_Task(void);
	
void Comm_Init(void);
void Comm_Task(void);
	
// UART缓冲区初始化函数声明
void UART_Buffer_Init(void);

#endif /* __APP_MANAGER_H__ */ 
