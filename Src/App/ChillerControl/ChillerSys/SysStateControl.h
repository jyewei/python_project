#ifndef __SYSSTATECONTROL_H
#define __SYSSTATECONTROL_H
#include "Common.h"


//系统状态
typedef enum
{
    SYS_STATUS_INIT,           //上电
    SYS_STATUS_OFF,            //关机
    SYS_STATUS_PREPARE,        //运转准备
    SYS_STATUS_RUN,            //运转
    SYS_STATUS_STANDBY,        //待机

}SysStatusEnum;





uint8_t SysGetUnitWorkState(uint8_t unit_ch);
void SysSetUnitWorkMode(uint8_t unit_ch,uint8_t mode);
void SysSetUnitSwitchState(uint8_t unit_ch,uint8_t state);
void SysSetAllUnitSwitchState(uint8_t state);

uint8_t GetSysUrgencyStopState(void);
uint8_t CheckSysOperateState(void);
void SysSetRunModeUpdate(void);
uint8_t GetSysSetRunMode(void);
uint8_t GetSysRealRunMode(void);
uint8_t CheckSysModeChange(void);
uint8_t CheckSysTempMeetStandyMode(void);
uint8_t CheckSysTempMeetCoolMode(void);
uint8_t CheckSysTempMeetHeatMode(void);



void SysStatusRunCool(void);
void SysStatusRunHeat(void);
void SysStatusRunAntifreeze(void);
void SysStatusRunPump(void);


#endif//__SYSSTATECONTROL_H
