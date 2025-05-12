#include "app_manager.h"
#include "main.h"
#include "mb_include.h"   // 包含Modbus头文件
#include "mb_test.h"
#include <stdio.h>
#include <stdint.h>

/**
 * @brief  应用层初始化
 */
void App_Init(void)
{
    /* 初始化各个模块 */
    LED_Init();
    KEY_Init();
    Comm_Init(); // 初始化通信相关，如RS485方向控制引脚
    
    // 初始化UART缓冲区
    UART_Buffer_Init();

    ModbusTest_Init();

}

/**
 * @brief  应用层周期性任务
 */
void App_Task(void)
{
    /* LED任务 - 运行指示灯 */
    //LED_Task();
    
    // /* 按键任务 */
    // KEY_Task();
    
    // /* 通信任务 */
    // Comm_Task();
		mbh_poll();
}

/**
 * @brief  LED初始化
 */
void LED_Init(void)
{
    /* LED引脚已在gpio.c中初始化 */
    // 初始状态可以设置为灭
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);
}

/**
 * @brief  LED任务处理
 */
void LED_Task(void)
{
    // static uint32_t lastToggleTime = 0;
    // uint32_t currentTime = HAL_GetTick();
    
    // // PA6作为运行指示灯，500ms闪烁一次
    // if(currentTime - lastToggleTime >= 500)
    // {
    //     lastToggleTime = currentTime;
    //     HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
    // }
    // PA7可以用于其他状态指示
}

/**
 * @brief  按键初始化
 */
void KEY_Init(void)
{
    /* 按键引脚已在gpio.c中初始化 */
}

/**
 * @brief  按键任务处理
 */
void KEY_Task(void)
{
    /* 简单的按键扫描示例 */
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET)
    {
        // 按键1按下处理
    }
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET)
    {
        // 按键2按下处理
    }
}

/**
 * @brief  通信初始化
 */
void Comm_Init(void)
{

#ifdef USE_STM32_HAL
    
#endif
}

/**
 * @brief  通信任务处理
 */
void Comm_Task(void)
{
    /* 如果有其他通信逻辑（如LoRa, NB-IoT）可以在这里处理 */
}
