/***********************************************************************
@file   : IOProcess.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
@note   : refresh DI,DO,AI
@note   : 基板上与硬件相关数据向unit.table内写入
            包括DI,Dout,sensor
************************************************************************/
#include "IOProcess.h"
#include "safety_function.h"
#include "TimerCounter.h"
#include "Data.h"


#define TM_AD_REFRESH   ((uint16_t)1)  //1s

// IOParamType io;






/************************************************************************
@name  	: AdConvertToRealValue
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static int16_t AdConvertToRealValue(uint8_t ch)
{
	int16_t res;
	
	
	
	return res;
}

/************************************************************************
@name  	: UpdateDswState
@brief 	: 
@param 	: None
@return	: None
@note   : 
*************************************************************************/
static void UpdateDswState(void)
{
    //TODO get local
    atw.unit.table.init.dsw1.bit.unit_addr = atw.io.dsw.cell.unit_addr;
    atw.unit.table.init.dsw1.bit.capacity = atw.io.dsw.cell.capacity;
    atw.unit.table.init.dsw1.bit.unit_work_state = atw.io.dsw.cell.unit_work_state;
    atw.unit.table.init.dsw1.bit.power_on_reset = atw.io.dsw.cell.power_on_reset;
    atw.unit.table.init.dsw1.bit.refrigent_type = atw.io.dsw.cell.refrigent_type;
    atw.unit.table.init.dsw1.bit.pump_type = atw.io.dsw.cell.pump_type;
    atw.unit.table.init.dsw1.bit.unit_series = atw.io.dsw.cell.unit_series;
    atw.unit.table.init.dsw1.bit.temp_type = atw.io.dsw.cell.temp_type;
    atw.unit.table.init.dsw1.bit.tier_type = atw.io.dsw.cell.tier_type;

}

/************************************************************************
@name  	: UpdateDIState
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void UpdateDIState(void)
{
    //TODO get local

    atw.unit.table.io.DIO.bit.unit_flow_switch = atw.io.din.cell.unit_waterflow_switch;
    atw.unit.table.io.DIO.bit.sys_flow_switch = atw.io.din.cell.sys_waterflow_switch;
    atw.unit.table.io.DIO.bit.remote_heat_cool_switch = atw.io.din.cell.remote_mode_change;
    atw.unit.table.io.DIO.bit.remote_di = atw.io.din.cell.remote_di;
    atw.unit.table.io.DIO.bit.urgency_off = atw.io.din.cell.urgency_off;
    atw.unit.table.io.DIO.bit.pump_over_fb = atw.io.din.cell.water_pump_over_load;
    atw.unit.table.io.DIO.bit.sec_pump_fb = atw.io.din.cell.secondary_pump_fb;
    atw.unit.table.io.DIO2.bit.unit_hot_water_flow_switch = atw.io.din.cell.unit_hot_waterflow_fan_prot_fb1;
    atw.unit.table.io.DIO2.bit.sys_hot_water_pump_switch = atw.io.din.cell.sys_hot_waterflow_fan_prot_fb2;
    atw.unit.table.io.DIO2.bit.sys_hot_water_flow_switch = atw.io.din.cell.heat_low_pressure_power_prot;
    atw.unit.table.io.DIO2.bit.heat_low_pres_switch = atw.io.din.cell.cool_low_pressure_switch;
    // atw.unit.table.io.DIO2.bit.power_protect = atw.io.din.cell.unit_hot_waterflow_fan_prot_fb1;
    // atw.unit.table.io.DIO2.bit.cool_low_pres_switch = atw.io.din.cell.unit_hot_waterflow_fan_prot_fb1;
  
}

/************************************************************************
@name  	: UpdateDOState
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void UpdateDOState(void)
{
    atw.io.dout.cell.four_way_v1 = atw.cycle[CYCLE_1].io.dout.bit.four_way;
    atw.io.dout.cell.four_way_v2 = atw.cycle[CYCLE_2].io.dout.bit.four_way;
    atw.io.dout.cell.water_heater = atw.unit.table.io.DIO.bit.water_heater;
    atw.io.dout.cell.water_pump = atw.unit.table.io.DIO.bit.water_pump;
    atw.io.dout.cell.comp_hc1 = atw.cycle[CYCLE_1].io.dout.bit.comp_heat_belt;
    atw.io.dout.cell.comp_hc2 = atw.cycle[CYCLE_2].io.dout.bit.comp_heat_belt;
    atw.io.dout.cell.chassis_heat_belt1 = atw.unit.table.io.DIO.bit.heat_belt1_out;
    atw.io.dout.cell.chassis_heat_belt2 = atw.unit.table.io.DIO.bit.heat_belt2_out;
    atw.io.dout.cell.evi_sv1 = atw.cycle[CYCLE_1].io.dout.bit.evi_valve;
    atw.io.dout.cell.oil_return_sv1 = atw.cycle[CYCLE_1].io.dout.bit.oil_valve;
    atw.io.dout.cell.evi_sv2 = atw.cycle[CYCLE_2].io.dout.bit.evi_valve;
    atw.io.dout.cell.oil_return_sv2 = atw.cycle[CYCLE_2].io.dout.bit.oil_valve;
    atw.io.dout.cell.water_valve = atw.unit.table.io.DIO.bit.water_valve;
    atw.io.dout.cell.board_heater = atw.unit.local.DIO.bit.board_heater;
    atw.io.dout.cell.hot_water_valve = atw.unit.table.io.DIO.bit.hot_water_valve;
    atw.io.dout.cell.BPHE_sv1 = atw.cycle[CYCLE_1].io.dout.bit.BPHE_valve;
    atw.io.dout.cell.BPHE_sv2 = atw.cycle[CYCLE_2].io.dout.bit.BPHE_valve;
    atw.io.dout.cell.coil_branch_sv1 = atw.cycle[CYCLE_1].io.dout.bit.coil_valve;
    atw.io.dout.cell.coil_branch_sv2 = atw.cycle[CYCLE_2].io.dout.bit.coil_valve;
    atw.io.dout.cell.defrost_sv1 = atw.cycle[CYCLE_1].io.dout.bit.defrost_valve;
    atw.io.dout.cell.defrost_sv2 = atw.cycle[CYCLE_2].io.dout.bit.defrost_valve;
    atw.io.dout.cell.hot_water_pump = atw.unit.table.io.DIO.bit.hot_water_valve;
    atw.io.dout.cell.antifreeze_heater = atw.unit.table.io.DIO.bit.antifreeze_heat;

    //TODO update to local
}

/************************************************************************
@name  	: UpdateSensorState
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void UpdateSensorState(void)
{
    uint8_t ch;

    if (TimerCheck(NO_AD_REFRESH) == TRUE)
    {
        TimerSet(NO_AD_REFRESH,TM_AD_REFRESH); //刷新时间//1s
        //get ad value
        for (ch = SENSOR_SHORT; ch < SENSOR_CH_MAX; ch++)
        {
            atw.io.sensor[ch].rt_value = AdConvertToRealValue(ch);
        }
//update to atw parameters  //TODO add limit handle
    //unit 
        atw.unit.table.io.Tam = atw.io.sensor[SENSOR_Tam].rt_value;
        atw.unit.table.io.Tw_in = atw.io.sensor[SENSOR_Tw_in].rt_value;
        atw.unit.table.io.Tw_out = atw.io.sensor[SENSOR_Tw_out].rt_value;
        atw.unit.table.io.Tw_sys_out = atw.io.sensor[SENSOR_Tw_sys_out].rt_value;

    //cycle1
        //压力相关
        atw.cycle[CYCLE_1].comp[COMP_1].local.Pd_last = atw.cycle[CYCLE_1].comp[COMP_1].local.Pd;//Pd  //TODO 压力处理时,为3s平均值
        atw.cycle[CYCLE_1].comp[COMP_1].local.Pd = atw.io.sensor[SENSOR_Pd1].rt_value;
        atw.cycle[CYCLE_1].comp[COMP_1].local.Ps_last = atw.cycle[CYCLE_1].comp[COMP_1].local.Ps;//Ps
        atw.cycle[CYCLE_1].comp[COMP_1].local.Ps = atw.io.sensor[SENSOR_Ps1].rt_value;

        atw.cycle[CYCLE_1].comp[COMP_1].local.Tc_last = atw.cycle[CYCLE_1].comp[COMP_1].local.Tc;//Tc
        atw.cycle[CYCLE_1].comp[COMP_1].local.Tc = 0;   //TODO 查表,表待定 //排气压力传感器Pd对应的饱和温度值表
        atw.cycle[CYCLE_1].comp[COMP_1].local.Tevp = 0; //TODO            //吸气压力传感器Ps对应的饱和温度值表

        //温度
        atw.cycle[CYCLE_1].comp[COMP_1].local.Td_last = atw.cycle[CYCLE_1].comp[COMP_1].local.Td;//Td
        atw.cycle[CYCLE_1].comp[COMP_1].local.Td = atw.io.sensor[SENSOR_Td1].rt_value;
        atw.cycle[CYCLE_1].comp[COMP_1].local.Ts_last = atw.cycle[CYCLE_1].comp[COMP_1].local.Ts;//Ts
        atw.cycle[CYCLE_1].comp[COMP_1].local.Ts = atw.io.sensor[SENSOR_Ts1].rt_value;
        atw.cycle[CYCLE_1].comp[COMP_1].local.SH_Td = atw.cycle[CYCLE_1].comp[COMP_1].local.Td - atw.cycle[CYCLE_1].comp[COMP_1].local.Tc;
        atw.cycle[CYCLE_1].comp[COMP_1].local.SH_TS_act_last = atw.cycle[CYCLE_1].comp[COMP_1].local.SH_TS_act;
        atw.cycle[CYCLE_1].comp[COMP_1].local.SH_TS_act = atw.cycle[CYCLE_1].comp[COMP_1].local.Ts - atw.cycle[CYCLE_1].comp[COMP_1].local.Tevp; 
        atw.cycle[CYCLE_1].comp[COMP_1].local.SH_TS_Target_last = atw.cycle[CYCLE_1].comp[COMP_1].local.SH_TS_Target;
        // atw.cycle[CYCLE_1].comp[COMP_1].local.SH_TS_Target =  
        atw.cycle[CYCLE_1].io.Tsub_in = atw.io.sensor[SENSOR_Tsub_in1].rt_value;
        atw.cycle[CYCLE_1].io.Tsub_out = atw.io.sensor[SENSOR_Tsub_out1].rt_value;
        atw.cycle[CYCLE_1].io.SH_sub_last = atw.cycle[CYCLE_1].io.SH_sub;
        atw.cycle[CYCLE_1].io.SH_sub = atw.cycle[CYCLE_1].io.Tsub_out - atw.cycle[CYCLE_1].io.Tsub_in;
        atw.cycle[CYCLE_1].io.Tcoil = atw.io.sensor[SENSOR_Tcoil1].rt_value;

    //cycle2
        //压力相关
        atw.cycle[CYCLE_2].comp[COMP_1].local.Pd_last = atw.cycle[CYCLE_2].comp[COMP_1].local.Pd;//Pd  //TODO 压力处理时,为3s平均值
        atw.cycle[CYCLE_2].comp[COMP_1].local.Pd = atw.io.sensor[SENSOR_Pd2].rt_value;
        atw.cycle[CYCLE_2].comp[COMP_1].local.Ps_last = atw.cycle[CYCLE_2].comp[COMP_1].local.Ps;//Ps
        atw.cycle[CYCLE_2].comp[COMP_1].local.Ps = atw.io.sensor[SENSOR_Ps2].rt_value;

        atw.cycle[CYCLE_2].comp[COMP_1].local.Tc_last = atw.cycle[CYCLE_2].comp[COMP_1].local.Tc;//Tc
        atw.cycle[CYCLE_2].comp[COMP_1].local.Tc = 0;   //TODO 查表,表待定 //排气压力传感器Pd对应的饱和温度值表
        atw.cycle[CYCLE_2].comp[COMP_1].local.Tevp = 0; //TODO            //吸气压力传感器Ps对应的饱和温度值表

        //温度
        atw.cycle[CYCLE_2].comp[COMP_1].local.Td_last = atw.cycle[CYCLE_2].comp[COMP_1].local.Td;//Td
        atw.cycle[CYCLE_2].comp[COMP_1].local.Td = atw.io.sensor[SENSOR_Td2].rt_value;
        atw.cycle[CYCLE_2].comp[COMP_1].local.Ts_last = atw.cycle[CYCLE_2].comp[COMP_1].local.Ts;//Ts
        atw.cycle[CYCLE_2].comp[COMP_1].local.Ts = atw.io.sensor[SENSOR_Ts2].rt_value;
        atw.cycle[CYCLE_2].comp[COMP_1].local.SH_Td = atw.cycle[CYCLE_2].comp[COMP_1].local.Td - atw.cycle[CYCLE_2].comp[COMP_1].local.Tc;
        atw.cycle[CYCLE_2].comp[COMP_1].local.SH_TS_act_last = atw.cycle[CYCLE_2].comp[COMP_1].local.SH_TS_act;
        atw.cycle[CYCLE_2].comp[COMP_1].local.SH_TS_act = atw.cycle[CYCLE_2].comp[COMP_1].local.Ts - atw.cycle[CYCLE_2].comp[COMP_1].local.Tevp; 
        atw.cycle[CYCLE_2].comp[COMP_1].local.SH_TS_Target_last = atw.cycle[CYCLE_2].comp[COMP_1].local.SH_TS_Target;
        // atw.cycle[CYCLE_2].comp[COMP_1].local.SH_TS_Target =  
        atw.cycle[CYCLE_2].io.Tsub_in = atw.io.sensor[SENSOR_Tsub_in2].rt_value;
        atw.cycle[CYCLE_2].io.Tsub_out = atw.io.sensor[SENSOR_Tsub_out2].rt_value;
        atw.cycle[CYCLE_2].io.SH_sub_last = atw.cycle[CYCLE_2].io.SH_sub;
        atw.cycle[CYCLE_2].io.SH_sub = atw.cycle[CYCLE_2].io.Tsub_out - atw.cycle[CYCLE_2].io.Tsub_in;
        atw.cycle[CYCLE_2].io.Tcoil = atw.io.sensor[SENSOR_Tcoil2].rt_value;

    }
}

/************************************************************************
@name  	: GetLocalAddress
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
uint8_t GetLocalAddress(void)
{
    uint8_t res = 0;

    res = atw.io.dsw.cell.unit_addr;
    // res = 1;
    return res;
}

/************************************************************************
@name  	: GetLocalGroupAddress
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
uint8_t GetLocalGroupAddress(void)
{
    return atw.io.dsw.cell.group_addr;
}

/************************************************************************
@name  	: DoSet
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void DoSet(uint8_t ch,uint8_t state)
{
    if (state == OFF)
    {
        atw.io.dout.data &= ~((uint32_t)(1<<ch));
    }
    else
    {
        atw.io.dout.data |= ((uint32_t)(1<<ch));
    }
}

/************************************************************************
@name  	: IOProcess
@brief 	: 
@param 	: None
@return	: None
@note   : 更新主板上的DSW,DI,DO,sensor值到相应unit table中,上电时需要先执行获取DSW的相关配置
*************************************************************************/
void IOProcess(uint8_t taskNo,uint8_t flag_init)
{
    SFSetRunTaskNo( taskNo );
    if (flag_init == FALSE)
    {
        UpdateDswState();//dsw的值只在上电时刷新
    }
    UpdateDIState();
    UpdateDOState();
    UpdateSensorState();

}
