/**
  ******************************************************************************
  * @file    mb_test.h
  * @author  Modbus RTU Master Test Module
  * @brief   Modbus RTU主站测试模块头文件
  ******************************************************************************
  * @attention
  *
  * 该文件包含Modbus RTU主站测试模块的函数声明
  *
  ******************************************************************************
  */

#ifndef __MB_TEST_H
#define __MB_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含头文件 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mb_include.h"

/* 宏定义 */
#define MODBUS_SLAVE_ADDR      1       // 从站地址
#define MODBUS_FUNCTION_READ   0x04    // 功能码：读输入寄存器
#define MODBUS_REG_ADDRESS     0x0001  // 寄存器地址
#define MODBUS_REG_COUNT       1       // 读取寄存器数量
#define MODBUS_TEST_INTERVAL   2000    // 测试间隔时间(ms)

/* 函数声明 */
/**
 * @brief  初始化Modbus测试模块
 * @param  无
 * @retval 无
 */
void ModbusTest_Init(void);

/**
 * @brief  Modbus测试任务，放入主循环调用
 * @param  无
 * @retval 无
 */
void ModbusTest_Task(void);

/**
 * @brief  以十六进制格式打印数据
 * @param  prefix: 前缀说明
 * @param  data: 数据缓冲区
 * @param  len: 数据长度
 * @retval 无
 */
void ModbusTest_PrintHexFrame(const char* prefix, uint8_t* data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __MB_TEST_H */ 
