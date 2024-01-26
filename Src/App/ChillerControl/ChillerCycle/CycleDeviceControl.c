/***********************************************************************
@file   : CycleDeviceControl.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
@note   : 1.四通阀控制
          2.压缩机曲轴箱电加热带
          3.压缩机喷焓电磁阀
          4.回油电磁阀
          5.驱动风冷风扇
************************************************************************/
#include "CycleDeviceControl.h"
#include "TimerCounter.h"
#include "config.h"
#include "Data.h"




//===四通阀控制
/************************************************************************
@name  	: CycleFourWayValveControl
@brief 	: 
@param 	: None
@return	: None
@note   : 默认为关(制冷),开(制热)
*************************************************************************/
void CycleFourWayValveControl(uint8_t cycle,uint8_t state)
{
    atw.cycle[cycle].io.dout.bit.four_way = state;
}

/************************************************************************
@name  	: CycleGetFourWayValveState
@brief 	: 
@param 	: None
@return	: None
@note   : 获取四通阀的状态
*************************************************************************/
uint8_t CycleGetFourWayValveState(uint8_t cycle)
{
    uint8_t res = OFF;

    res = atw.cycle[cycle].io.dout.bit.four_way;
    return res;
}


//压缩机曲轴箱电加热带
/************************************************************************
@name  	: CycleCompCrankcaseHeatBeltControl
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void CycleCompCrankcaseHeatBeltControl(uint8_t cycle)
{
    uint16_t Fi_run = atw.cycle[cycle].comp[COMP_1].link.state.freq_output;//压缩机实际频率
    int16_t  tam_temp = atw.unit.table.io.Tam;
    int16_t  Td = atw.cycle[cycle].comp[COMP_1].local.Td;

    if (Fi_run == 0) //压缩机停止中
    {
        if (atw.cycle[cycle].comp[COMP_1].local.sns_err.bit.Td == FALSE)//无故障
        {
            if ((tam_temp < 240)
                || (atw.unit.local.sns_err.bit.Tam == TRUE))
            {
                if (Td < 500)
                {
                    atw.cycle[cycle].io.dout.bit.comp_heat_belt = ON;
                }
                else atw.cycle[cycle].io.dout.bit.comp_heat_belt = OFF;
            }
            else
            {
                atw.cycle[cycle].io.dout.bit.comp_heat_belt = OFF;
            }
        }
        else atw.cycle[cycle].io.dout.bit.comp_heat_belt = OFF;
    }
    else//运行中
    {
        atw.cycle[cycle].io.dout.bit.comp_heat_belt = OFF;
    }
}

//压缩机喷焓电磁阀
/************************************************************************
@name  	: CycleCompEviValveControl
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void CycleCompEviValveControl(uint8_t cycle)
{
    uint8_t step = atw.cycle[cycle].work_step;
    uint8_t evi_valve_state = OFF;
    uint16_t Fi_run = atw.cycle[cycle].comp[COMP_1].link.state.freq_output;//压缩机实际频率
    uint16_t comp_run_time = atw.cycle[cycle].comp[COMP_1].local.current_run_time;//压缩机当前运行时间
    int16_t Td = atw.cycle[cycle].comp[COMP_1].local.Td;
    int16_t Tc = atw.cycle[cycle].comp[COMP_1].local.Tc;
    uint16_t I_inv = atw.cycle[cycle].comp[COMP_1].link.state.current;//压机电流
    int16_t Td_last = atw.cycle[cycle].comp[COMP_1].local.Td_last;

    switch (step)
    {
        case CYCLE_STATUS_OFF:
        case CYCLE_STATUS_START1:
        case CYCLE_STATUS_START2:
        case CYCLE_STATUS_START_DEFROST_DONE:
        case CYCLE_STATUS_DIFFER_PRESSURE:
        case CYCLE_STATUS_DEFROST_RUN:      //关闭
        {
            evi_valve_state = OFF;
        }
            break;
        case CYCLE_STATUS_RUN_COOL:
        case CYCLE_STATUS_RUN_OIL_RETURN:
        case CYCLE_STATUS_DEFROST_PREPARE:
        case CYCLE_STATUS_RUN_HEAT:
        {
            if (atw.unit.table.init.dsw1.bit.unit_work_state == UNIT_STATE_TEST)//测试模式
            {
                if (atw.cycle[cycle].expv[EXPV3].rt_exv_step > 0)//exv3>0,则开启该阀
                {
                    evi_valve_state = ON;
                }
                else evi_valve_state = OFF;
            }
            else//非测试模式
            {
                if (atw.cycle[cycle].expv[EXPV3].rt_exv_step > 0)
                {
                    evi_valve_state = ON;
                }
                else evi_valve_state = OFF;

                if (evi_valve_state == OFF)
                {
                    if ((Fi_run >= 400)         //频率大于40Hz
                        && (comp_run_time > 5)  //压缩机运行时间超5mins
                        && ( ((Td >= Tc + 180) && (Td >= Td_last)) || (Td >= Tc + 200) )
                        && (I_inv <= 0))//TODO
                    {
                        evi_valve_state = ON;
                    }
                    else if ((Fi_run >= 300)
                            && (Td >= 980)
                            && (I_inv <= 0))//TODO
                    {
                        evi_valve_state = ON;
                    }
                }
            }
        }
            break;
        default:
            break;
    }
    atw.cycle[cycle].io.dout.bit.evi_valve = evi_valve_state;
}

//回油电磁阀
/************************************************************************
@name  	: OilReturnValveControl
@brief 	: 
@param 	: None
@return	: None
@note   : 进入cycle关机时,延时30s关闭,
            cycle启动1关闭
            其它为打开
*************************************************************************/
void CycleOilReturnValveControl(uint8_t cycle)
{
    uint8_t last_step = atw.cycle[cycle].last_step;
    uint8_t step = atw.cycle[cycle].work_step;

    if ((step == CYCLE_STATUS_OFF) && (last_step != CYCLE_STATUS_OFF))
    {
        TimerSet(NO_CYCLE1_OIL_VALVE + cycle,30);//进入cycle关机,延时30s后关闭
    }
    else if (step == CYCLE_STATUS_OFF)
    {
        if (TimerCheck(NO_CYCLE1_OIL_VALVE + cycle)== TRUE)
        {
            atw.cycle[cycle].io.dout.bit.oil_valve = OFF;//
        }
    }
    else if (step == CYCLE_STATUS_START1)
    {
        atw.cycle[cycle].io.dout.bit.oil_valve = OFF;//
    }
    else
    {
        atw.cycle[cycle].io.dout.bit.oil_valve = ON;//
    }
}





