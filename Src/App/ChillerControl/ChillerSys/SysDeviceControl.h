#ifndef __SYSDEVICECONTROL_H
#define __SYSDEVICECONTROL_H
#include "Common.h"









uint8_t SysGetWaterPumpType(void);
// void SysWaterPumpControl(uint8_t unit_ch,uint8_t state);
void SysAllWaterPumpControl(uint8_t state);
void SysWaterValveControl(uint8_t unit_ch,uint8_t state);
void SysAllWaterValveControl(uint8_t state);
void SysFloorHeatTwowayValveControl(uint8_t state);    //地板采暖二通阀
void SysFanCiolTwowayValveControl(uint8_t state);      //风机盘管二通阀


void SysWaterHeaterControl(void);//辅助水电加热器



#endif//__SYSDEVICECONTROL_H
