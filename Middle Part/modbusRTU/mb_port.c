/**
  ******************************************************************************
  * @file    mb_port.c
  * @author  Derrick Wang
  * @brief   modebus移植接口
  ******************************************************************************
  * @note
  * 该文件为modbus移植接口的实现，根据不同的MCU平台进行移植
  ******************************************************************************
  */

/* 包含头文件 */
// #include <stdint.h>  /* 标准整数类型定义 */
// #include <stddef.h>  /* 标准定义 */
#include "mb_include.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* 宏定义 */
// RS485方向控制引脚定义(B3)
#define RS485_DIR_GPIO_PORT       GPIOB
#define RS485_DIR_GPIO_PIN        GPIO_PIN_3
// RS485方向控制模式
#define RS485_DIR_TX              1   // 发送模式
#define RS485_DIR_RX              0   // 接收模式

/**
 * @brief  设置RS485方向控制引脚状态
 * @param  dir: RS485_DIR_TX=发送模式, RS485_DIR_RX=接收模式
 * @retval 无
 */
void mb_port_rs485_dir(uint8_t dir)
{
    if (dir == RS485_DIR_TX)
    {
        // 设置为发送模式 - 引脚置高
        HAL_GPIO_WritePin(RS485_DIR_GPIO_PORT, RS485_DIR_GPIO_PIN, GPIO_PIN_SET);
    }
    else
    {
        // 设置为接收模式 - 引脚置低
        HAL_GPIO_WritePin(RS485_DIR_GPIO_PORT, RS485_DIR_GPIO_PIN, GPIO_PIN_RESET);
    }
}

void mb_port_uartInit(void)
{
	mb_port_rs485_dir(RS485_DIR_TX);
}

void mb_port_uartEnable(uint8_t txen,uint8_t rxen)
{
	// 根据参数控制RS485方向
	if(txen)
	{
		// 启用发送 - 设置RS485为发送模式
		mb_port_rs485_dir(RS485_DIR_TX);
		__HAL_UART_ENABLE_IT(&huart2,UART_IT_TC);
	}
	else
	{
		__HAL_UART_DISABLE_IT(&huart2,UART_IT_TC);
	}
	
	if(rxen)
	{
		// 启用接收 - 设置RS485为接收模式
		mb_port_rs485_dir(RS485_DIR_RX);
		__HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);
	}
	else
	{
		__HAL_UART_DISABLE_IT(&huart2,UART_IT_RXNE);
	}		
}
void mb_port_putchar(uint8_t ch)
{
	huart2.Instance->TDR = ch;  //直接操作寄存器比HAL封装的更高效，STM32H7中使用TDR代替DR
}

void mb_port_getchar(uint8_t *ch)
{
	*ch = (uint8_t)(huart2.Instance->RDR & (uint8_t)0x00FF);  //STM32H7中使用RDR代替DR	
}

void mb_port_timerInit(uint32_t baud)
{
	/*定时器部分初始化*/
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 12000;
	if( baud > 19200 )
	{
			htim3.Init.Period  = 35;       /* 1800us. */
	}
	else
	{
			/* The timer reload value for a character is given by:
			 *
			 * ChTimeValue = Ticks_per_1s / ( Baudrate / 11 )
			 *             = 11 * Ticks_per_1s / Baudrate
			 *             = 220000 / Baudrate
			 * The reload for t3.5 is 1.5 times this value and similary
			 * for t3.5.
			 */
			htim3.Init.Period = ( 7UL * 220000UL ) / ( 2UL * baud );
	}
	if (HAL_TIM_Base_Start_IT(&htim3) != HAL_OK)
	{
		// 启动失败，进行错误处理
		Error_Handler();
	}
	if (HAL_TIM_Base_Start_IT(&htim7) != HAL_OK)
	{
		// 启动失败，进行错误处理
		Error_Handler();
	}

}

void mb_port_timerEnable()
{
	__HAL_TIM_DISABLE(&htim3);
	__HAL_TIM_CLEAR_IT(&htim3,TIM_IT_UPDATE);                //清除中断位
	__HAL_TIM_ENABLE_IT(&htim3,TIM_IT_UPDATE);                //使能中断位
	__HAL_TIM_SET_COUNTER(&htim3,0);                           //设置定时器计数为0
	__HAL_TIM_ENABLE(&htim3);                                 //使能定时器
}

void mb_port_timerDisable()
{
	__HAL_TIM_DISABLE(&htim3);
	__HAL_TIM_SET_COUNTER(&htim3,0); 
	__HAL_TIM_DISABLE_IT(&htim3,TIM_IT_UPDATE);
	__HAL_TIM_CLEAR_IT(&htim3,TIM_IT_UPDATE);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

  if (htim->Instance == TIM7)
  {
	extern void ModbusTest_Task(void);
	ModbusTest_Task();
  }

  // 如果你使用了其他定时器，可以在这里添加更多的 if 语句进行区分
  // if (htim->Instance == TIM6)
  // {
  //     // 处理 TIM6 的定时任务
  // }
}
