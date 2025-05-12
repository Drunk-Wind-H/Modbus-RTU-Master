#include <stdio.h>
#include <stdint.h>
#include "main.h"
#include "usart.h"
#include "dma.h"

#define UART_BUFFER_SIZE 256
#define DMA_TX_BUFFER_SIZE 64  // DMA发送缓冲区大小

// UART实例定义 - 根据实际硬件配置修改
extern UART_HandleTypeDef huart4;  // 假设使用UART4，根据实际项目修改

// 循环缓冲区结构
typedef struct {
    uint8_t buffer[UART_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
    volatile uint16_t count;
} CircularBuffer;

// DMA传输状态
typedef enum {
    DMA_READY = 0,   // DMA就绪，可以发起新传输
    DMA_BUSY = 1     // DMA正在传输
} DMA_StatusTypeDef;

// 全局变量
static CircularBuffer txBuffer = {0};
static volatile DMA_StatusTypeDef dmaTxStatus = DMA_READY;
static uint8_t dmaTxBuffer[DMA_TX_BUFFER_SIZE];  // DMA传输缓冲区

// 向循环缓冲区添加数据
static uint8_t Buffer_Write(CircularBuffer *cb, uint8_t data)
{
    if(cb->count >= UART_BUFFER_SIZE)
        return 0;  // 缓冲区已满

    cb->buffer[cb->tail] = data;
    cb->tail = (cb->tail + 1) % UART_BUFFER_SIZE;
    cb->count++;
    return 1;
}

// 从循环缓冲区读取数据
static uint8_t Buffer_Read(CircularBuffer *cb, uint8_t *data)
{
    if(cb->count == 0)
        return 0;  // 缓冲区为空

    *data = cb->buffer[cb->head];
    cb->head = (cb->head + 1) % UART_BUFFER_SIZE;
    cb->count--;
    return 1;
}

// 启动DMA传输
static void StartDmaTransfer(void)
{
    uint16_t bytesToSend = 0;
    uint16_t i;

    // 如果DMA忙或缓冲区为空，不启动传输
    if (dmaTxStatus == DMA_BUSY || txBuffer.count == 0)
        return;

    // 确定要发送的字节数（不超过DMA缓冲区大小和循环缓冲区中的可用数据）
    bytesToSend = (txBuffer.count > DMA_TX_BUFFER_SIZE) ? DMA_TX_BUFFER_SIZE : txBuffer.count;

    // 从循环缓冲区复制数据到DMA发送缓冲区
    for (i = 0; i < bytesToSend; i++)
    {
        Buffer_Read(&txBuffer, &dmaTxBuffer[i]);
    }

    // 标记DMA为忙状态
    dmaTxStatus = DMA_BUSY;

    // 启动DMA传输
    HAL_UART_Transmit_DMA(&huart4, dmaTxBuffer, bytesToSend);
}

// DMA发送完成回调
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    // 检查是否是我们使用的UART实例
    if (huart == &huart4)
    {
        // DMA传输完成，将状态设置为就绪
        dmaTxStatus = DMA_READY;
        
        // 如果缓冲区中还有数据，启动下一次DMA传输
        if (txBuffer.count > 0)
        {
            StartDmaTransfer();
        }
    }
}

// UART错误回调
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    // 检查是否是我们使用的UART实例
    if (huart == &huart4)
    {
        // 重置DMA状态
        dmaTxStatus = DMA_READY;
        
        // 可以在这里添加UART错误处理代码
        // 例如: 尝试重新初始化UART或记录错误
    }
}

// 重定向printf到UART
int fputc(int ch, FILE *f)
{
    uint8_t status;
    uint32_t timeout;
    
    // 加入临界区保护
    __disable_irq();
    
    // 尝试将字符添加到发送缓冲区
    status = Buffer_Write(&txBuffer, (uint8_t)ch);
    
    // 退出临界区
    __enable_irq();
    
    if (!status)
    {
        // 如果缓冲区满，等待一些空间
        timeout = HAL_GetTick() + 100;  // 100ms超时
        
        do {
            // 退出超时检查
            if (HAL_GetTick() > timeout)
                return EOF;  // 超时返回错误
            
            // 重新尝试添加字符
            __disable_irq();
            status = Buffer_Write(&txBuffer, (uint8_t)ch);
            __enable_irq();
            
        } while (!status);
    }
    
    // 如果DMA就绪且有数据待发送，启动DMA传输
    if (dmaTxStatus == DMA_READY && txBuffer.count > 0)
    {
        StartDmaTransfer();
    }
    
    return ch;
}

// 初始化UART缓冲和DMA
void UART_Buffer_Init(void)
{
    // 重置缓冲区
    txBuffer.head = 0;
    txBuffer.tail = 0;
    txBuffer.count = 0;
    dmaTxStatus = DMA_READY;
    
    // 可以在这里添加其他初始化代码
    // 例如设置UART DMA模式
}
