/***********************************************************************
@file   : SysStateControl.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
@note   : 
************************************************************************/
#include "SysStateControl.h"
#include "Data.h"
#include "AlarmCheck.h"
#include "config.h"
#include "SysCapacityControl.h"



/************************************************************************
@name  	: SysGetUnitWorkState
@brief 	: 
@param 	: None
@return	: test_mode/normal_mode
*************************************************************************/
uint8_t SysGetUnitWorkState(uint8_t unit_ch)
{
    uint8_t res = UNIT_MODE_NULL;

    if (atw.sys.table.unit[unit_ch].event.state.bit.unit_run_state == TRUE)
    {
        if (atw.sys.table.unit[unit_ch].event.unit_run_mode == 1)
        {
            res = UNIT_MODE_HEAT;
        }
        else if (atw.sys.table.unit[unit_ch].event.unit_run_mode == 2)
        {
            res = UNIT_MODE_COOL;
        }
        else if (atw.sys.table.unit[unit_ch].event.unit_run_mode == 2)
        {
            res = UNIT_MODE_PUMP;
        }
    }
    
    return res;
}

/************************************************************************
@name  	: SysSetUnitWorkMode
@brief 	: 
@param 	: state:ON/OFF
@return	: None
*************************************************************************/
void SysSetUnitWorkMode(uint8_t unit_ch,uint8_t mode)
{
    atw.sys.table.unit[unit_ch].cmd.unit_run_mode = mode;
} 
 

/************************************************************************
@name  	: SysSetUnitSwitchState
@brief 	: 
@param 	: state:ON/OFF
@return	: None
*************************************************************************/
void SysSetUnitSwitchState(uint8_t unit_ch,uint8_t state)
{
    atw.sys.table.unit[unit_ch].cmd.data1.bit.unit_run = state;
} 

/************************************************************************
@name  	: SysSetAllUnitSwitchState
@brief 	: 
@param 	: state:ON/OFF
@return	: None
*************************************************************************/
void SysSetAllUnitSwitchState(uint8_t state)
{
    uint8_t ch;

    for (ch = CHILLER_0; ch < CHILLER_CH_MAX; ch++)
    {
        atw.sys.table.unit[ch].cmd.data1.bit.unit_run = state;
    }
}


/************************************************************************
@name  	: GetSysUrgencyStopState
@brief 	: 
@param 	: None
@return	: None
@note   : 获取系统急停信号
*************************************************************************/
uint8_t GetSysUrgencyStopState(void)
{
    uint8_t res = FALSE;


    return res;
}


/************************************************************************
@name  	: GetSysUrgencySwitchState
@brief 	: 
@param 	: None
@return	: None
@note   : 获取系统的紧急联动关信号(CHILLER_0 DI5/紧急联动信号) @note开路使能,
*************************************************************************/
static uint8_t GetSysUrgencySwitchState(void)
{
    uint8_t res = OFF;

    if (atw.sys.table.unit[CHILLER_0].io.DIO.bit.urgency_off == FALSE)//CHILLER_0 DI输入
    {
        res = ON;
    }

    return res;
}

/************************************************************************
@name  	: SysCheckUnitOptExist
@brief 	: 
@param 	: None
@return	: None
@note   : 检测系统中是否有可运转的单元
*************************************************************************/
static uint8_t SysCheckUnitOptExist(void)
{
    uint8_t res = FALSE;
    uint8_t ch;

    for (ch = CHILLER_0; ch < CHILLER_CH_MAX; ch++)
    {
        if ((atw.sys.table.unit[ch].event.state.bit.unit_opt_state == TRUE)//存在可运转的单元
            && (atw.sys.local.unit[ch].link_state == TRUE))//通信连接正常
        {
            res = TRUE;
            break;
        }
    }

    return res;
}

/************************************************************************
@name  	: CheckSysOperateState
@brief 	: 
@param 	: None
@return	: None
@note   : 3.3 系统运转限制
            3.3.1.运转时间限制(系统故障)
            3.3.2.紧急情况关机联动限制
            3.3.3.不同类型单元混装(系统故障)
            3.3.4.系统故障限制
            3.3.5.系统中没有可运转的单元
*************************************************************************/
uint8_t CheckSysOperateState(void)
{
    uint8_t res = FALSE;

    if ((CheckkSysAlarmState() == FALSE)//无系统故障
        && (GetSysUrgencySwitchState() == OFF)//无紧急联动关信号
        && (SysCheckUnitOptExist() == TRUE))//有可运转的单元
    {
        res = TRUE;
    }

    return res;
}

/************************************************************************
@name  	: SysSetRunModeUpdate
@brief 	: 
@param 	: None
@return	: None
@note   : 根据外部控制器的设置来决定运行模式
            1.组控器
            2.I/O端口
*************************************************************************/
void SysSetRunModeUpdate(void)//TODO
{
    if (atw.sys.grplink.grp_ctrl.state1.bit.run == TRUE)
    {
        if (atw.sys.grplink.grp_ctrl.state1.bit.cool == TRUE)
        {
            atw.sys.local.sys_set_mode = SYS_MODE_COOL;
        }
        else if (atw.sys.grplink.grp_ctrl.state1.bit.heat == TRUE)
        {
            atw.sys.local.sys_set_mode = SYS_MODE_HEAT;
        }
        else if (atw.sys.grplink.grp_ctrl.state1.bit.pump == TRUE)
        {
            atw.sys.local.sys_set_mode = SYS_MODE_PUMP;
        }
        else
        {
            atw.sys.local.sys_set_mode = SYS_MODE_NULL;
        }
    }
    else
    {
        atw.sys.local.sys_set_mode = SYS_MODE_NULL;
    }
}

/************************************************************************
@name  	: GetSysSetRunMode
@brief 	: 
@param 	: None
@return	: None
@note   : 
*************************************************************************/
uint8_t GetSysSetRunMode(void)
{
    return atw.sys.local.sys_set_mode;
}

/************************************************************************
@name  	: GetSysRealRunMode
@brief 	: 
@param 	: None
@return	: None
@note   : 
*************************************************************************/
uint8_t GetSysRealRunMode(void)
{    
    return atw.sys.local.sys_rt_mode;
}

/************************************************************************
@name  	: CheckSysModeChange
@brief 	: sys mode change check
@param 	: None
@return	: None
@note   : 
*************************************************************************/
uint8_t CheckSysModeChange(void)
{
    uint8_t res = FALSE;

    if ((atw.sys.local.sys_set_mode != SYS_MODE_NULL)
        && (atw.sys.local.sys_set_mode != atw.sys.local.sys_rt_mode))
    {
        res = TRUE;
    }
    return res;
}

/************************************************************************
@name  	: CheckSysTempMeetStandyMode
@brief 	: check water temperature whether the condition meet standby mode
@param 	: None
@return	: None
@note   : 
*************************************************************************/
uint8_t CheckSysTempMeetStandyMode(void)
{
    uint8_t res = FALSE;

    
    
    return res;
}

/************************************************************************
@name  	: CheckSysTempMeetCoolMode
@brief 	: check water temperature whether the condition meet cool mode
@param 	: None
@return	: None
@note   : 
*************************************************************************/
uint8_t CheckSysTempMeetCoolMode(void)
{
    uint8_t res = FALSE;

    
    
    return res;
}

/************************************************************************
@name  	: CheckSysTempMeetHeatMode
@brief 	: check water temperature whether the condition meet heat mode
@param 	: None
@return	: None
@note   : 
*************************************************************************/
uint8_t CheckSysTempMeetHeatMode(void)
{
    uint8_t res = FALSE;

    
    
    return res;
}

/************************************************************************
@name  	: SysStatusRunCool
@brief 	: 
@param 	: None
@return	: None
@note   : 
*************************************************************************/
void SysStatusRunCool(void)
{
    atw.sys.local.work_step = SYS_MODE_COOL;
    SysCapacityControl();
}

/************************************************************************
@name  	: SysStatusRunHeat
@brief 	: 
@param 	: None
@return	: None
@note   : 
*************************************************************************/
void SysStatusRunHeat(void)
{
    atw.sys.local.work_step = SYS_MODE_HEAT;
    SysCapacityControl();
}

/************************************************************************
@name  	: SysStatusRunAntifreeze
@brief 	: 
@param 	: None
@return	: None
@note   : 
*************************************************************************/
void SysStatusRunAntifreeze(void)
{
    atw.sys.local.work_step = SYS_MODE_ANTIFREEZE;
    SysCapacityControl();
}

/************************************************************************
@name  	: SysStatusRunPump
@brief 	: 
@param 	: None
@return	: None
@note   : 
*************************************************************************/
void SysStatusRunPump(void)
{
    atw.sys.local.work_step = SYS_MODE_HEAT;
    
    
}








