/* USER CODE BEGIN Includes */
#include "mb.h"
#include "mbport.h"
#include "string.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
// Modbus主站相关变量
static uint8_t ucMBAddress = 1;  // 从站地址
static uint16_t usRegHoldingBuf[REG_HOLDING_NREGS];  // 保持寄存器缓冲区
static uint8_t ucErrorCount = 0;  // 错误计数器
/* USER CODE END PV */

/* USER CODE BEGIN PFP */
// Modbus主站回调函数
eMBErrorCode eMBMasterRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress, 
                                  USHORT usNRegs, eMBRegisterMode eMode)
{
    eMBErrorCode eStatus = MB_ENOERR;
    
    if ((usAddress >= REG_HOLDING_START) && 
        (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS))
    {
        switch (eMode)
        {
            case MB_REG_READ:
                memcpy(pucRegBuffer, &usRegHoldingBuf[usAddress - REG_HOLDING_START], 
                       usNRegs * 2);
                break;
                
            case MB_REG_WRITE:
                memcpy(&usRegHoldingBuf[usAddress - REG_HOLDING_START], pucRegBuffer, 
                       usNRegs * 2);
                break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    
    return eStatus;
}

// 错误处理函数
void vMBErrorCB(eMBErrorCode eStatus)
{
    switch (eStatus)
    {
        case MB_EPORTERR:
            // 端口错误
            ucErrorCount++;
            break;
            
        case MB_ENORES:
            // 资源不足
            ucErrorCount++;
            break;
            
        case MB_EIO:
            // IO错误
            ucErrorCount++;
            break;
            
        case MB_EILLSTATE:
            // 非法状态
            ucErrorCount++;
            break;
            
        default:
            break;
    }
    
    // 如果错误次数过多，可以采取相应措施
    if (ucErrorCount > 10)
    {
        // 可以在这里添加错误恢复机制
        ucErrorCount = 0;
    }
}
/* USER CODE END PFP */

/* USER CODE BEGIN 2 */
// Modbus主站初始化
eMBErrorCode eStatus;
eStatus = eMBInit(MB_RTU, 0x01, 0, 9600, MB_PAR_NONE);
if (eStatus != MB_ENOERR) {
    Error_Handler();
}

// 注册回调函数
eStatus = eMBMasterRegHoldingCB(usRegHoldingBuf, 0, REG_HOLDING_NREGS, MB_REG_READ);
if (eStatus != MB_ENOERR) {
    Error_Handler();
}

// 启动Modbus主站
eStatus = eMBEnable();
if (eStatus != MB_ENOERR) {
    Error_Handler();
}

// 初始化LED
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
/* USER CODE END 2 */

/* USER CODE BEGIN WHILE */
while (1)
{
    // Modbus主站轮询
    (void)eMBPoll();
    
    // 读取从站数据
    uint16_t usRegHoldingStart = 0;
    uint16_t usRegHoldingNRegs = 1;
    eStatus = eMBMasterReqReadHoldingRegister(ucMBAddress, usRegHoldingStart, 
                                             usRegHoldingNRegs, usRegHoldingBuf);
    if (eStatus == MB_ENOERR) {
        // 处理读取到的数据
        if (usRegHoldingBuf[0] == 1) {
            // 有人
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);  // 点亮LED
        } else {
            // 无人
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);  // 关闭LED
        }
    } else {
        // 读取失败，调用错误处理
        vMBErrorCB(eStatus);
    }
    
    HAL_Delay(1000);  // 延时1秒
    /* USER CODE END WHILE */
} 