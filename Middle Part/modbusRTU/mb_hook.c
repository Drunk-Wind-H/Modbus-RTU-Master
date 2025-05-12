/**
  ******************************************************************************
  * @file    mb_hook.c
  * @author  Derrick Wang
  * @brief   modebus回调函数接口
  ******************************************************************************
  * @note	
  * 针对modbus的回调处理，请再该文件中添加		
  ******************************************************************************
  */

#include "mb_include.h"

void mbh_hook_rec01(uint8_t add,uint8_t *data,uint8_t datalen)
{

}
void mbh_hook_rec02(uint8_t add,uint8_t *data,uint8_t datalen)
{

}
void mbh_hook_rec03(uint8_t add,uint8_t *data,uint8_t datalen)
{

}
void mbh_hook_rec04(uint8_t add,uint8_t *data,uint8_t datalen)
{
  printf("processing in mbh_hook_rec04\r\n");
}
void mbh_hook_rec05(uint8_t add,uint8_t *data,uint8_t datalen)
{

}
void mbh_hook_rec06(uint8_t add,uint8_t *data,uint8_t datalen)
{

}
void mbh_hook_rec15(uint8_t add,uint8_t *data,uint8_t datalen)
{

}
void mbh_hook_rec16(uint8_t add,uint8_t *data,uint8_t datalen)
{

}


void mbh_hook_timesErr(uint8_t add,uint8_t cmd)
{
  printf("mbh_hook_timesErr\r\n");  
	mbHost.txCounter=0;
  mbHost.rxCounter=0;
  mbHost.state=MBH_STATE_IDLE;
  mb_port_uartEnable(1,0);  //enable tx,disable rx
}

