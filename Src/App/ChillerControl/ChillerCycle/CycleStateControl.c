/***********************************************************************
@file   : CycleStateControl.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
@note   : cycle状态获取或设置
************************************************************************/
#include "CycleStateControl.h"
#include "data.h"
#include "config.h"
#include "AlarmCheck.h"
#include "CompControl.h"



/************************************************************************
@name  	: GetCycleSetRunMode
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
uint8_t GetCycleSetRunMode(uint8_t cycle)
{
    uint8_t res = CYCLE_MODE_NULL;

    res = atw.cycle[cycle].set_run_mode;
    return res;
}

/************************************************************************
@name  	: GetCycleRealRunMode
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
uint8_t GetCycleRealRunMode(uint8_t cycle)
{
    uint8_t res = CYCLE_MODE_NULL;

    res = atw.cycle[cycle].rt_run_mode;
    return res;
}

/************************************************************************
@name  	: CheckCycleOperateState
@brief 	: cycle的可操作状态更新
@param 	: None
@return	: None
@note   : cycle不可运行状态
            1.压机处于冷启动保护
            2.压机处于3mins屏蔽保护
            3.压机保护停机
            4.压机锁定状态
            5.单元运转准备时,20s检测风机实际转速与目标值相差10rps则报风机故障,cycle不可运转
*************************************************************************/
uint8_t CheckCycleOperateState(uint8_t cycle)
{
    uint8_t comp_step = atw.cycle[cycle].comp[COMP_1].local.work_step;

    if ((CheckCycleAlarmState(cycle) == FALSE)//cycle无故障
        && (GetCycleRealRunMode(cycle) == CYCLE_MODE_NULL))//cycle关机
    {
        if ((comp_step == COMP_3MINS_PROTECT) || (comp_step == COMP_RUN_COLD_START))//3mins保护或冷启动保护
        {
            atw.cycle[cycle].opt_state = FALSE;
        }
        else atw.cycle[cycle].opt_state = TRUE;
    }
    else atw.cycle[cycle].opt_state = FALSE;

    return atw.cycle[cycle].opt_state;
}

/************************************************************************
@name  	: CheckCycleCloseableState
@brief 	: cycle的可关机
@param 	: None
@return	: None
@note   : 6.3
            cycle可关机
                1.cycle已运转
                2.cycle中压缩机持续运转时间超9mins
                3.cycle运转状态为通常运转,回油运转或化霜准备
*************************************************************************/
uint8_t CheckCycleCloseableState(uint8_t cycle)
{
    uint8_t step = atw.cycle[cycle].work_step;

    if ((atw.cycle[cycle].comp[COMP_1].link.state.freq_output > 0)//已运转
        && (atw.cycle[cycle].comp[COMP_1].local.current_run_time > 9)//压机运转时间超9mins
        && ((step == CYCLE_STATUS_DEFROST_PREPARE) || (step == CYCLE_STATUS_RUN_HEAT) || (step == CYCLE_STATUS_RUN_COOL) || (step == CYCLE_STATUS_RUN_OIL_RETURN)))
    {
        atw.cycle[cycle].close_able_state = TRUE;
    }
    else atw.cycle[cycle].close_able_state = FALSE;

    return atw.cycle[cycle].close_able_state;
}

/************************************************************************
@name  	: GetCycleOperateState
@brief 	: 获取cycle的可操作状态
@param 	: None
@return	: None
@note   : cycle不可运行状态
            1.压机处于冷启动保护
            2.压机处于3mins屏蔽保护
            3.压机保护停机
            4.压机锁定状态
*************************************************************************/
uint8_t GetCycleOperateState(uint8_t cycle)
{
    return atw.cycle[cycle].opt_state;
}

















