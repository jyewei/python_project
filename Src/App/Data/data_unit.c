/***********************************************************************
@file   : data_unit.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
@note   : unit与sys相关的参数获取或设置
************************************************************************/
#include "data_unit.h"
#include "IOProcess.h"
#include "Data.h"




/************************************************************************
@name  	: UnitGetParamFromConfig
@brief 	: unit根据配置获取参数
@param 	: None
@return	: None
@note   :1.框体种类 
         2.cycle有效数量
*************************************************************************/
void UnitGetParamFromConfig(void)
{
    uint8_t capacity = atw.unit.table.init.dsw1.bit.capacity;

//1.框体

//2.cycle有效    
    switch (capacity)
    {
        case CAPACITY_65KW:
        {
            atw.unit.local.cycle_active_cnt = 1;
            atw.cycle[CYCLE_1].enable = TRUE;
            atw.cycle[CYCLE_2].enable = FALSE;
            atw.cycle[CYCLE_3].enable = FALSE;
            atw.cycle[CYCLE_4].enable = FALSE;
        }
            break;
        case CAPACITY_130KW:
        {
            atw.unit.local.cycle_active_cnt = 2;
            atw.cycle[CYCLE_1].enable = TRUE;
            atw.cycle[CYCLE_2].enable = TRUE;
            atw.cycle[CYCLE_3].enable = FALSE;
            atw.cycle[CYCLE_4].enable = FALSE;
        }
            break;
        case CAPACITY_300KW:
        {
            atw.unit.local.cycle_active_cnt = 4;
            atw.cycle[CYCLE_1].enable = TRUE;
            atw.cycle[CYCLE_2].enable = TRUE;
            atw.cycle[CYCLE_3].enable = TRUE;
            atw.cycle[CYCLE_4].enable = TRUE;
        }
            break;
        default:
            break;
    }



    
}

/************************************************************************
@name  	: GetTableCyleEvent
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
CycleTableEventType GetTableCyleEvent(uint8_t cycle)
{
    CycleTableEventType cycle_event;

    cycle_event.alarm_code = 0;

    return cycle_event;
}

/************************************************************************
@name  	: GetTableCyleIO
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
CycleTableIOType GetTableCyleIO(uint8_t cycle)
{
    CycleTableIOType cycle_io;

    cycle_io.io1.bit.four_way = atw.cycle[cycle].io.dout.bit.four_way;
    cycle_io.io1.bit.comp_heat_belt = atw.cycle[cycle].io.dout.bit.comp_heat_belt;
    cycle_io.io1.bit.evi_valve = atw.cycle[cycle].io.dout.bit.evi_valve;
    cycle_io.io1.bit.oil_valve = atw.cycle[cycle].io.dout.bit.oil_valve;
    cycle_io.io1.bit.BPHE_valve = atw.cycle[cycle].io.dout.bit.BPHE_valve;
    cycle_io.io1.bit.coil_valve = atw.cycle[cycle].io.dout.bit.coil_valve;
    cycle_io.io1.bit.defrost_valve = atw.cycle[cycle].io.dout.bit.defrost_valve;
    cycle_io.io1.bit.fan_protect_fb = atw.cycle[cycle].io.dout.bit.fan_protect_fb;

    cycle_io.io2 = 0;
    cycle_io.io3 = 0;
    cycle_io.Ps = atw.cycle[cycle].comp[COMP_1].local.Ps;
    cycle_io.Pd = atw.cycle[cycle].comp[COMP_1].local.Pd;
    cycle_io.Ts = atw.cycle[cycle].comp[COMP_1].local.Ts;
    cycle_io.Td = atw.cycle[cycle].comp[COMP_1].local.Td;
    cycle_io.Tsub_in = atw.cycle[cycle].io.Tsub_in;
    cycle_io.Tsub_out = atw.cycle[cycle].io.Tsub_out;
    cycle_io.Tcoil = atw.cycle[cycle].io.Tcoil;
    cycle_io.exv1_opening = atw.cycle[cycle].expv[EXPV1].rt_exv_step; //TODO
    cycle_io.exv2_opening = atw.cycle[cycle].expv[EXPV2].rt_exv_step;
    cycle_io.exv3_opening = atw.cycle[cycle].expv[EXPV3].rt_exv_step;
    cycle_io.comp_current = atw.cycle[cycle].comp[COMP_1].link.state.current;
    cycle_io.comp_rt_freq = atw.cycle[cycle].comp[COMP_1].link.state.freq_output;
    cycle_io.comp_set_freq = atw.cycle[cycle].comp[COMP_1].local.set_freq;
    cycle_io.fan1_rt_freq = atw.cycle[cycle].fan[FAN_1].link.state.foc_freq_output;
    cycle_io.fan1_set_freq = atw.cycle[cycle].fan[FAN_1].link.ctrl.freq_set;
    cycle_io.fan2_rt_freq = atw.cycle[cycle].fan[FAN_2].link.state.foc_freq_output;
    cycle_io.fan2_set_freq = atw.cycle[cycle].fan[FAN_2].link.ctrl.freq_set;

    return cycle_io;
}

/************************************************************************
@name  	: GetTableCyleInitSum
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
CycleTableInitSumType GetTableCyleInitSum(uint8_t cycle)
{
    CycleTableInitSumType cycle_init_sum;

    cycle_init_sum.comp_romNO = 0;  //TODO
    cycle_init_sum.fan_romNO = 0;
    cycle_init_sum.comp_run_time_sum = atw.cycle[cycle].comp[COMP_1].local.total_run_time;
    cycle_init_sum.comp_power_sum = atw.cycle[cycle].comp[COMP_1].local.total_electric_power_use;
    cycle_init_sum.sys_run_time_sum = 0;//TODO

    return cycle_init_sum;
}





