/***********************************************************************
@file   : UnitStateControl.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
@note   : 
************************************************************************/
#include "UnitStateControl.h"
#include "Data.h"
#include "config.h"
#include "CycleStateControl.h"
#include "AlarmCheck.h"
#include "ChillerUnit.h"






/************************************************************************
@name  	: UnitSetRunModeUpdate
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void UnitSetRunModeUpdate(void)
{
    if (atw.unit.table.sys_cmd.data1.bit.unit_run == TRUE)
    {
        atw.unit.local.set_run_mode = atw.unit.table.sys_cmd.unit_run_mode;
    }
    else atw.unit.local.set_run_mode = UNIT_MODE_NULL;

}

/************************************************************************
@name  	: GetUnitSetRunMode
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
uint8_t GetUnitSetRunMode(void)
{
    return atw.unit.local.set_run_mode;
}



/************************************************************************
@name  	: GetUnitRealRunMode
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
uint8_t GetUnitRealRunMode(void)
{
    return atw.unit.local.rt_run_mode;
}

/************************************************************************
@name  	: UnitOperableStateUpdate
@brief 	: unit的可操作状态更新
@param 	: None
@return	: None
@note   : 1.存在可运转的cycle,unit即为可运转
          2.  4.1.模块单元运转限值
            4.1.1 环境温度限制
*************************************************************************/
void UnitOperableStateUpdate(void)
{
    uint8_t cycle;

//更新cycle的可运转数量,已运转数量
    atw.unit.table.event.cycle_cnt.bit.operate = 0; //可运转的数量
    atw.unit.table.event.cycle_cnt.bit.running = 0; //已运转的数量
    for (cycle = CYCLE_1;cycle < atw.unit.local.cycle_active_cnt;cycle++)
    {
        if (GetCycleOperateState(cycle) == TRUE)//cycle operate
        {
            atw.unit.table.event.cycle_cnt.bit.operate++;
        }
        
        if (atw.cycle[cycle].comp[COMP_1].link.state.freq_output > 0)//压机频率>0
        {
            atw.unit.table.event.cycle_cnt.bit.running++;
        }
    }
//单元可运转状态    
    if ((CheckUnitAlarmState() == FALSE) && (UnitUrgencyStopCheck() == FALSE))//无单元故障,且无急停信号
    {
        if ((atw.unit.table.event.cycle_cnt.bit.operate > 0)    //有可运转的cycle
            || (atw.unit.table.event.cycle_cnt.bit.running > 0))//或有正在运转的cycle
        {
            atw.unit.table.event.state.bit.unit_opt_state = TRUE;//unit可运转 //TODO
        }
        else atw.unit.table.event.state.bit.unit_opt_state = FALSE;//unit不可运转 //TODO
    }
    else atw.unit.table.event.state.bit.unit_opt_state = FALSE;

}

/************************************************************************
@name  	: UnitOperableStateCheck
@brief 	: 单元可运行状态判断
@param 	: None
@return	: None
*************************************************************************/
uint8_t UnitOperableStateCheck(void)
{
    return  atw.unit.table.event.state.bit.unit_opt_state;//unit可运转 //TODO
}

/************************************************************************
@name  	: UnitSetCycleRun
@brief 	: 单元开启cycle
@param 	: None
@return	: None
*************************************************************************/
void UnitSetCycleRun(uint8_t cycle,uint8_t cycle_mode)
{
    atw.cycle[cycle].set_run_mode = cycle_mode;
}

/************************************************************************
@name  	: UnitSetCycleFreq
@brief 	: 单元开启cycle
@param 	: None
@return	: None
*************************************************************************/
void UnitSetCycleFreq(uint8_t cycle,uint16_t freq)
{
    atw.cycle[cycle].set_freq = freq;
}

/************************************************************************
@name  	: UnitSetCycleClose
@brief 	: 单元关闭cycle
@param 	: None
@return	: None
*************************************************************************/
void UnitSetCycleClose(uint8_t cycle)
{
    atw.cycle[cycle].set_run_mode = CYCLE_MODE_NULL;
}



/************************************************************************
@name  	: UnitRunCool
@brief 	: 
@param 	: None
@return	: None
@note   : //TODO 参数确定 unit_freq_add
          
*************************************************************************/
void UnitRunCool(void)
{
    if (atw.unit.table.sys_cmd.unit_cycle_add > 0)//系统开启的cycle>0
    {
        atw.unit.local.rt_run_mode = UNIT_MODE_COOL;
        UnitSetCycleRun(CYCLE_1,CYCLE_MODE_COOL);
        UnitSetCycleFreq(CYCLE_1,atw.unit.table.sys_cmd.unit_freq_add);
    }
    
    
    
}

/************************************************************************
@name  	: UnitRunHeat
@brief 	: 
@param 	: None
@return	: None
@note   : 
*************************************************************************/
void UnitRunHeat(void)
{
    if (atw.unit.table.sys_cmd.unit_cycle_add > 0)//系统开启的cycle>0 //TODO
    {
        atw.unit.local.rt_run_mode = UNIT_MODE_HEAT;
        UnitSetCycleRun(CYCLE_1,CYCLE_MODE_COOL);
        UnitSetCycleFreq(CYCLE_1,atw.unit.table.sys_cmd.unit_freq_add);
    }
    
}

/************************************************************************
@name  	: UnitRunPump
@brief 	: 
@param 	: None
@return	: None
@note   : 
*************************************************************************/
void UnitRunPump(void)
{
    atw.unit.local.rt_run_mode = UNIT_MODE_PUMP;
    
}



















