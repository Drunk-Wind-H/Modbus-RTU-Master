#include <stdio.h>
#include <stdint.h>
#include "main.h"
#include "usart.h"
#include "dma.h"

#define UART_BUFFER_SIZE 256
#define DMA_TX_BUFFER_SIZE 64  // DMA���ͻ�������С

// UARTʵ������ - ����ʵ��Ӳ�������޸�
extern UART_HandleTypeDef huart4;  // ����ʹ��UART4������ʵ����Ŀ�޸�

// ѭ���������ṹ
typedef struct {
    uint8_t buffer[UART_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
    volatile uint16_t count;
} CircularBuffer;

// DMA����״̬
typedef enum {
    DMA_READY = 0,   // DMA���������Է����´���
    DMA_BUSY = 1     // DMA���ڴ���
} DMA_StatusTypeDef;

// ȫ�ֱ���
static CircularBuffer txBuffer = {0};
static volatile DMA_StatusTypeDef dmaTxStatus = DMA_READY;
static uint8_t dmaTxBuffer[DMA_TX_BUFFER_SIZE];  // DMA���仺����

// ��ѭ���������������
static uint8_t Buffer_Write(CircularBuffer *cb, uint8_t data)
{
    if(cb->count >= UART_BUFFER_SIZE)
        return 0;  // ����������

    cb->buffer[cb->tail] = data;
    cb->tail = (cb->tail + 1) % UART_BUFFER_SIZE;
    cb->count++;
    return 1;
}

// ��ѭ����������ȡ����
static uint8_t Buffer_Read(CircularBuffer *cb, uint8_t *data)
{
    if(cb->count == 0)
        return 0;  // ������Ϊ��

    *data = cb->buffer[cb->head];
    cb->head = (cb->head + 1) % UART_BUFFER_SIZE;
    cb->count--;
    return 1;
}

// ����DMA����
static void StartDmaTransfer(void)
{
    uint16_t bytesToSend = 0;
    uint16_t i;

    // ���DMAæ�򻺳���Ϊ�գ�����������
    if (dmaTxStatus == DMA_BUSY || txBuffer.count == 0)
        return;

    // ȷ��Ҫ���͵��ֽ�����������DMA��������С��ѭ���������еĿ������ݣ�
    bytesToSend = (txBuffer.count > DMA_TX_BUFFER_SIZE) ? DMA_TX_BUFFER_SIZE : txBuffer.count;

    // ��ѭ���������������ݵ�DMA���ͻ�����
    for (i = 0; i < bytesToSend; i++)
    {
        Buffer_Read(&txBuffer, &dmaTxBuffer[i]);
    }

    // ���DMAΪæ״̬
    dmaTxStatus = DMA_BUSY;

    // ����DMA����
    HAL_UART_Transmit_DMA(&huart4, dmaTxBuffer, bytesToSend);
}

// DMA������ɻص�
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    // ����Ƿ�������ʹ�õ�UARTʵ��
    if (huart == &huart4)
    {
        // DMA������ɣ���״̬����Ϊ����
        dmaTxStatus = DMA_READY;
        
        // ����������л������ݣ�������һ��DMA����
        if (txBuffer.count > 0)
        {
            StartDmaTransfer();
        }
    }
}

// UART����ص�
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    // ����Ƿ�������ʹ�õ�UARTʵ��
    if (huart == &huart4)
    {
        // ����DMA״̬
        dmaTxStatus = DMA_READY;
        
        // �������������UART���������
        // ����: �������³�ʼ��UART���¼����
    }
}

// �ض���printf��UART
int fputc(int ch, FILE *f)
{
    uint8_t status;
    uint32_t timeout;
    
    // �����ٽ�������
    __disable_irq();
    
    // ���Խ��ַ���ӵ����ͻ�����
    status = Buffer_Write(&txBuffer, (uint8_t)ch);
    
    // �˳��ٽ���
    __enable_irq();
    
    if (!status)
    {
        // ��������������ȴ�һЩ�ռ�
        timeout = HAL_GetTick() + 100;  // 100ms��ʱ
        
        do {
            // �˳���ʱ���
            if (HAL_GetTick() > timeout)
                return EOF;  // ��ʱ���ش���
            
            // ���³�������ַ�
            __disable_irq();
            status = Buffer_Write(&txBuffer, (uint8_t)ch);
            __enable_irq();
            
        } while (!status);
    }
    
    // ���DMA�����������ݴ����ͣ�����DMA����
    if (dmaTxStatus == DMA_READY && txBuffer.count > 0)
    {
        StartDmaTransfer();
    }
    
    return ch;
}

// ��ʼ��UART�����DMA
void UART_Buffer_Init(void)
{
    // ���û�����
    txBuffer.head = 0;
    txBuffer.tail = 0;
    txBuffer.count = 0;
    dmaTxStatus = DMA_READY;
    
    // �������������������ʼ������
    // ��������UART DMAģʽ
}
