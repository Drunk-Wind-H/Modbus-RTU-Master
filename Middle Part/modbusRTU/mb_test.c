/**
  ******************************************************************************
  * @file    mb_test.c
  * @author  Modbus RTU Master Test Module
  * @brief   Modbus RTU主站测试模块实现
  ******************************************************************************
  * @attention
  *
  * 该文件包含Modbus RTU主站测试模块的具体实现
  *
  ******************************************************************************
  */

/* 包含头文件 */
#include "mb_test.h"
#include "mb_include.h"
#include "usart.h"
#include "tim.h"
#include "main.h"
#include "mb_host.h"

/**
 * @brief  初始化Modbus测试模块
 * @param  无
 * @retval 无
 */
void ModbusTest_Init(void)
{
    // 初始化Modbus主站
    mbh_init(9600);  // 波特率9600
    printf("ModbusTest_Init success\r\n");
}

/**
 * @brief  Modbus测试任务，放入TIM7定时器中断中调用
 * @param  无
 * @retval 无
 */
void ModbusTest_Task(void)
{    
        // 检查主站状态是否空闲
        uint8_t state = mbh_getState();
        if (state == MBH_STATE_IDLE)
        {
            // 准备发送查询从站输入寄存器的命令
            uint8_t data[4];
            uint16_t regAddr = MODBUS_REG_ADDRESS;
            uint16_t regCount = MODBUS_REG_COUNT;
            
            // 高字节在前，低字节在后
            data[0] = (regAddr >> 8) & 0xFF;   // 寄存器地址高字节
            data[1] = regAddr & 0xFF;          // 寄存器地址低字节
            data[2] = (regCount >> 8) & 0xFF;  // 寄存器数量高字节
            data[3] = regCount & 0xFF;         // 寄存器数量低字节
                     
            // 预先构建完整的Modbus帧用于显示
            uint8_t frame[8]; // 地址(1) + 功能码(1) + 数据(4) + CRC(2)
            frame[0] = MODBUS_SLAVE_ADDR;
            frame[1] = MODBUS_FUNCTION_READ;
            memcpy(&frame[2], data, 4);
            
            // 计算CRC (这里只是用于显示，实际发送时mbh_send会计算CRC)
            uint16_t crc = mb_crc16(frame, 6);
            frame[6] = crc & 0xFF;
            frame[7] = (crc >> 8) & 0xFF;
            
            /*for debug state*/
           	// static uint8_t lastState = 0xFF;
            // if (lastState != mbHost.state)
            // {
            //     printf("ModbusTest_Task state changed: %d -> %d\r\n", lastState, mbHost.state);
            //     lastState = mbHost.state;
            // }
            
            // 发送Modbus命令
            if (mbh_send(MODBUS_SLAVE_ADDR, MODBUS_FUNCTION_READ, data, 4) != 0)
            {
               printf("ModbusTest_Task processing in mbh_send() failed\r\n");
            }
        }  
} 
