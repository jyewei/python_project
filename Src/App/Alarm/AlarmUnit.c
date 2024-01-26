/***********************************************************************
@file   : AlarmUnit.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "AlarmUnit.h"
#include "config.h"
#include "Data.h"
#include "TimerCounter.h"
#include "UnitStateControl.h"



typedef struct
{
    uint8_t ac_power_in_check;      //1.电源检测
    uint8_t pump_run_wait;          //3.单元水泵故障,水泵启动20s后
    uint8_t pump_overload_check;    //水泵过载检测时间
    uint8_t sensor_tam_check;       //环境温度传感器故障
    uint8_t sensor_Tw_in_check;     //回水温度传感器故障
    uint8_t sensor_Tw_out_check;    //出水温度传感器故障
    uint8_t cool_temp_out_low_check;//制冷出水温度低
    uint8_t heat_temp_out_high_check;//制热出水温度高
    uint8_t heat_temp_out_low_check;//制热出水温度低

}UnitErrorTimeType;

static UnitErrorTimeType unit_time;
static UnitAlarmType unit_err;




/************************************************************************
@name  	: AlarmUnitTimerCounter
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void AlarmUnitTimerCounter(void)
{
    uint8_t *pTime;
    uint16_t i,len;
//1s
    pTime = &unit_time.ac_power_in_check;
    if (TimerCheck(NO_UNIT_ALARM_BASE) == TRUE)
    {
        TimerSet(NO_UNIT_ALARM_BASE,1); //base 1s
        len = sizeof(unit_time)/sizeof(uint8_t);
        for ( i = 0; i < len; i++)
        {
            if (*(pTime + i) != 0)
            {
                (*(pTime + i))--;
            }
        }
    }
//30mins    
    if (TimerCheck(NO_UNIT_ALARM_LOCK_BASE) == TRUE)//累计故障用
    {
        TimerSet(NO_UNIT_ALARM_LOCK_BASE,30); //base 1mins //
    //清除累计值
        // for (i = 0; i < CYCLE_ERR_MAX; i++)
        // {
        //     cycle_err_cnt[i] = 0;//clear cnt
        // }
    }
}


/************************************************************************
@name  	: UnitChillerlinkCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 1.0#主机与单元通信连续断开60s
            上电检测,显示故障,单元关机;自动复位
*************************************************************************/
static void UnitChillerlinkCheck(void)
{    
    if (atw.unit.local.chillerlink_state == TRUE)
    {
        unit_err.cell.chillerlink = FALSE;
    }
    else unit_err.cell.chillerlink = TRUE;
}

/************************************************************************
@name  	: UnitPowerPowerAcinCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 2.单元电源故障
            共用水泵:非0#主机,上电连续3s检测到电源缺相或逆相
            非共用水泵,上电,主板连续3s检测到电源缺相或逆相    //手动复位
*************************************************************************/
static void UnitPowerPowerAcinCheck(void)
{



}

/************************************************************************
@name  	: UnitPumpOverloadCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 3.单元水泵过载
            非共用水泵,水泵启动20s后检测水泵过载反馈信号,检测连续5s断开,水泵停止,报故障 //
*************************************************************************/
static void UnitPumpOverloadCheck(void)
{
    uint8_t pump_type = atw.unit.table.init.dsw1.bit.pump_type;
    uint8_t pump_state = atw.unit.table.io.DIO.bit.water_pump;//
    uint8_t overload_state = atw.unit.table.io.DIO.bit.pump_over_fb;
//水泵关闭时重置计时器20s
    if (pump_state == OFF)//水泵关闭
    {
        unit_time.pump_run_wait = 20;
    }
//检测    
    if (pump_type == PUMP_PRIVATE)//非共用水泵
    {
        if (unit_err.cell.pump_overload == FALSE)//无故障
        {
            if ((overload_state == TRUE) && (unit_time.pump_run_wait == 0))//20s后检测
            {
                if (unit_time.pump_overload_check == 0)
                {
                    unit_err.cell.pump_overload = TRUE;
                    unit_time.pump_overload_check = 5;//计时重置
                }
            }
            else
            {
                unit_time.pump_overload_check = 5;//计时重置
            }
        }
        else//有故障
        {
            if (overload_state == TRUE)//故障存在
            {
                unit_time.pump_overload_check = 5;//计时重置
            }
            else
            {
                if (unit_time.pump_overload_check == 0)//持续5s无故障
                {
                    unit_err.cell.pump_overload = FALSE;
                    unit_time.pump_overload_check = 5;//计时重置
                }
            }
        }
    }
    else unit_err.cell.pump_overload = FALSE; //共用水泵不检测
}

/************************************************************************
@name  	: UnitSensorTamCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 4.环境温度传感器故障
            超出范围(-40 ~ 80)持续5s,显示故障,正常运行,故障后,持续5s正常则故障自动复位
*************************************************************************/
static void UnitSensorTamCheck(void)
{
    int16_t Tam = atw.io.sensor[SENSOR_Tam].rt_value;//atw.unit.table.io.Tam;

    if (unit_err.cell.sensor_tam == FALSE)//无故障时
    {
        if ((Tam <= -400) || (Tam >= 800))//故障
        {
            if (unit_time.sensor_tam_check == 0)
            {
                unit_err.cell.sensor_tam = TRUE;
                unit_time.sensor_tam_check = 5;//重置计时器
            }
        }
        else
        {
            unit_time.sensor_tam_check = 5;//重置计时器
        }
    }
    else//存在故障
    {
        if ((Tam <= -400) || (Tam >= 800))//故障未恢复
        {
            unit_time.sensor_tam_check = 5;//重置计时器
        }
        else//故障恢复
        {
            if (unit_time.sensor_tam_check == 0)//持续时间达到
            {
                unit_err.cell.sensor_tam = FALSE;//clear error
                unit_time.sensor_tam_check = 5;//重置计时器
            }
        }
    }
}

/************************************************************************
@name  	: UnitCoolTempTamLowCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 5.制冷环温过低报警
            单元制冷
*************************************************************************/
static void UnitCoolTempTamLowCheck(void)
{
    int16_t Tam = atw.unit.table.io.Tam;
    int16_t temp_low;//TODO add  = atw.unit.table.sys_cmd.c

    if (unit_err.cell.sensor_tam == FALSE)//Tam无故障
    {
        if (unit_err.cell.cool_tam_low == FALSE)//无故障
        {
            if (GetUnitRealRunMode() == UNIT_MODE_COOL)//单元制冷
            {
                if (Tam < temp_low)//
                {
                    unit_err.cell.cool_tam_low = TRUE;
                }
            }
        }
        else//有故障
        {
            if (Tam > (temp_low + 10))//
            {
                unit_err.cell.cool_tam_low = FALSE;
            }
        }
    }
    else unit_err.cell.cool_tam_low = FALSE;
}


/************************************************************************
@name  	: UnitCoolTempTamHighCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 6.制冷环温过高报警
            单元制冷
*************************************************************************/
static void UnitCoolTempTamHighCheck(void)
{
    int16_t Tam = atw.unit.table.io.Tam;
    int16_t temp_high;//TODO add  = atw.unit.table.sys_cmd

    if (unit_err.cell.sensor_tam == FALSE)//Tam无故障
    {
        if (unit_err.cell.cool_tam_high == FALSE)//无故障
        {
            if (GetUnitRealRunMode() == UNIT_MODE_COOL)//单元制冷
            {
                if (Tam > temp_high)//
                {
                    unit_err.cell.cool_tam_high = TRUE;
                }
            }
        }
        else//有故障
        {
            if (Tam < (temp_high - 10))//
            {
                unit_err.cell.cool_tam_high = FALSE;
            }
        }
    }
    else unit_err.cell.cool_tam_high = FALSE;
}


/************************************************************************
@name  	: UnitHeatTempTamLowCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 7.制热环温过低报警
            单元制热
*************************************************************************/
static void UnitHeatTempTamLowCheck(void)
{
    int16_t Tam = atw.unit.table.io.Tam;
    int16_t temp_low;//TODO add  = atw.unit.table.sys_cmd.c

    if (unit_err.cell.sensor_tam == FALSE)//Tam无故障
    {
        if (unit_err.cell.heat_tam_low == FALSE)//无故障
        {
            if (GetUnitRealRunMode() == UNIT_MODE_HEAT)//单元制热
            {
                if (Tam < temp_low)//
                {
                    unit_err.cell.heat_tam_low = TRUE;
                }
            }
        }
        else//有故障
        {
            if (Tam > (temp_low + 10))//
            {
                unit_err.cell.heat_tam_low = FALSE;
            }
        }
    }
    else unit_err.cell.heat_tam_low = FALSE;
}

/************************************************************************
@name  	: UnitHeatTempTamHighCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 8.制热环温过高报警
            单元制热
*************************************************************************/
static void UnitHeatTempTamHighCheck(void)
{
    int16_t Tam = atw.unit.table.io.Tam;
    int16_t temp_high;//TODO add  = atw.unit.table.sys_cmd

    if (unit_err.cell.sensor_tam == FALSE)//Tam无故障
    {
        if (unit_err.cell.heat_tam_high == FALSE)//无故障
        {
            if (GetUnitRealRunMode() == UNIT_MODE_HEAT)//单元制冷
            {
                if (Tam > temp_high)//
                {
                    unit_err.cell.heat_tam_high = TRUE;
                }
            }
        }
        else//有故障
        {
            if (Tam < (temp_high - 10))//
            {
                unit_err.cell.heat_tam_high = FALSE;
            }
        }
    }
    else unit_err.cell.heat_tam_high = FALSE;
}

/************************************************************************
@name  	: UnitSensorTwinCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 9.回水温度传感器
            超出范围(-40 ~ 80)持续5s,显示故障,正常运行,故障后,持续5s正常则故障自动复位
*************************************************************************/
static void UnitSensorTwinCheck(void)
{
    int16_t Tw_in = atw.io.sensor[SENSOR_Tw_in].rt_value;//atw.unit.table.io.Tw_in;

    if (unit_err.cell.sensor_tw_in == FALSE)//无故障时
    {
        if ((Tw_in <= -400) || (Tw_in >= 800))//故障
        {
            if (unit_time.sensor_Tw_in_check == 0)
            {
                unit_err.cell.sensor_tw_in = TRUE;
                unit_time.sensor_Tw_in_check = 5;//重置计时器
            }
        }
        else
        {
            unit_time.sensor_Tw_in_check = 5;//重置计时器
        }
    }
    else//存在故障
    {
        if ((Tw_in <= -400) || (Tw_in >= 800))//故障未恢复
        {
            unit_time.sensor_Tw_in_check = 5;//重置计时器
        }
        else//故障恢复
        {
            if (unit_time.sensor_Tw_in_check == 0)//持续时间达到
            {
                unit_err.cell.sensor_tw_in = FALSE;//clear error
                unit_time.sensor_Tw_in_check = 5;//重置计时器
            }
        }
    }
}

/************************************************************************
@name  	: UnitSensorTwoutCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 10.出水温度传感器
            超出范围(-40 ~ 80)持续5s,显示故障,正常运行,故障后,持续5s正常则故障自动复位
*************************************************************************/
static void UnitSensorTwoutCheck(void)
{
    int16_t Tw_out = atw.unit.table.io.Tw_out;

    if (unit_err.cell.sensor_tw_out == FALSE)//无故障时
    {
        if ((Tw_out <= -400) || (Tw_out >= 800))//故障
        {
            if (unit_time.sensor_Tw_out_check == 0)
            {
                unit_err.cell.sensor_tw_out = TRUE;
                unit_time.sensor_Tw_out_check = 5;//重置计时器
            }
        }
        else
        {
            unit_time.sensor_Tw_out_check = 5;//重置计时器
        }
    }
    else//存在故障
    {
        if ((Tw_out <= -400) || (Tw_out >= 800))//故障未恢复
        {
            unit_time.sensor_Tw_out_check = 5;//重置计时器
        }
        else//故障恢复
        {
            if (unit_time.sensor_Tw_out_check == 0)//持续时间达到
            {
                unit_err.cell.sensor_tw_out = FALSE;//clear error
                unit_time.sensor_Tw_out_check = 5;//重置计时器
            }
        }
    }
}

/************************************************************************
@name  	: UnitCoolTempoutLowCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 11.制冷出水温度过低报警
            单元制冷,出水温度小于等于设定值2s时,报故障;大于设定值+6°后,自动复位
*************************************************************************/
static void UnitCoolTempoutLowCheck(void)
{
    int16_t Tw_out = atw.unit.table.io.Tw_out;
    uint8_t refri_type = atw.unit.table.init.dsw1.bit.refrigent_type;
    int16_t temp_low = atw.unit.table.sys_cmd.Tw_out_low_threshold1;//出水温度过低报警准位

    if (refri_type == ADD_ANTIFREEZE)
    {
        temp_low = atw.unit.table.sys_cmd.Tw_out_low_threshold2;//乙二醇
    }

    if (unit_err.cell.sensor_tw_out == FALSE)//传感器无故障 //TODO 待确认
    {
        if (unit_err.cell.cool_out_low == FALSE)//无故障
        {
            if (GetUnitRealRunMode() == UNIT_MODE_COOL)//单元制冷
            {
                if (Tw_out <= temp_low)//
                {
                    if (unit_time.cool_temp_out_low_check == 0)//持续2s
                    {
                        unit_err.cell.cool_out_low = TRUE;
                    }
                }
                else
                {
                    unit_time.cool_temp_out_low_check = 2;
                }
            }
        }
        else//有故障
        {
            unit_time.cool_temp_out_low_check = 2;
            if (Tw_out > (temp_low + 60))//
            {
                unit_err.cell.cool_out_low = FALSE;
            }
        }
    }
    else unit_err.cell.cool_out_low = FALSE;
}

/************************************************************************
@name  	: UnitHeatTempoutHighCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 12.制热出水温度过高报警
            单元制热,出水温度>=设定值2s时,报故障;大于设定值-6°后,自动复位
*************************************************************************/
static void UnitHeatTempoutHighCheck(void)
{
    int16_t Tw_out = atw.unit.table.io.Tw_out;
    int16_t temp_high = atw.unit.table.sys_cmd.Tw_out_high_threshold;//出水温度过高报警准位

    if (unit_err.cell.sensor_tw_out == FALSE)//传感器无故障 //TODO 待确认
    {
        if (unit_err.cell.heat_out_high == FALSE)//无故障
        {
            if (GetUnitRealRunMode() == UNIT_MODE_HEAT)//单元制热
            {
                if (Tw_out >= temp_high)//
                {
                    if (unit_time.heat_temp_out_high_check == 0)//持续2s
                    {
                        unit_err.cell.heat_out_high = TRUE;
                    }
                }
                else unit_time.heat_temp_out_high_check = 2;
            }
        }
        else//有故障
        {
            unit_time.heat_temp_out_high_check = 2;
            if (Tw_out < (temp_high - 60))//
            {
                unit_err.cell.heat_out_high = FALSE;
            }
        }
    }
    else unit_err.cell.heat_out_high = FALSE;
}


/************************************************************************
@name  	: UnitHeatTempoutLowCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 13_condition.制热出水温度过低报警
            单元制热,且压缩机运行满3mins:检测任一台压缩机运行时间
*************************************************************************/
static uint8_t UnitHeatTempoutLowCheckCompRunTime(void)
{
    uint8_t res = FALSE;
    uint8_t cycle_active = atw.unit.local.cycle_active_cnt;
    uint8_t cycle;
    uint32_t comp_run_time[CYCLE_CH_MAX];

    for (cycle = 0; cycle < cycle_active; cycle++)
    {
        comp_run_time[cycle] = atw.cycle[cycle].comp[COMP_1].local.current_run_time;
        if (comp_run_time[cycle] > 2)
        {
            return TRUE;
        }
    }

    return res;
}

/************************************************************************
@name  	: UnitHeatTempoutLowCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 13.制热出水温度过低报警
            单元制热,且压缩机运行满3mins, 出水温度<=5°,持续2s时,报故障;手动复位
*************************************************************************/
static void UnitHeatTempoutLowCheck(void)
{
    int16_t Tw_out = atw.unit.table.io.Tw_out;

    if (unit_err.cell.sensor_tw_out == FALSE)//传感器无故障 //TODO 待确认
    {
        if (unit_err.cell.heat_out_low == FALSE)//无故障
        {
            if ((GetUnitRealRunMode() == UNIT_MODE_COOL) && (UnitHeatTempoutLowCheckCompRunTime()== TRUE))//单元制热,压缩机运行满3mins
            {
                if (Tw_out <= 50)
                {
                    if (unit_time.heat_temp_out_low_check == 0)
                    {
                        unit_err.cell.heat_out_low = TRUE;
                    }
                }
                else unit_time.heat_temp_out_low_check = 2;
            }
        }
    }
    else unit_err.cell.heat_out_high = FALSE;
}

/************************************************************************
@name  	: UnitHeatTempoutLowCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 13_condition.制热出水温度过低报警
            单元制热,且压缩机运行满3mins:检测任一台压缩机运行时间
*************************************************************************/
static uint8_t UnitBoardFreezeCheckCompRunTime(void)
{
    uint8_t res = FALSE;
    uint8_t cycle_active = atw.unit.local.cycle_active_cnt;
    uint8_t cycle;
    uint32_t comp_run_time[CYCLE_CH_MAX];

    for (cycle = 0; cycle < cycle_active; cycle++)
    {
        comp_run_time[cycle] = atw.cycle[cycle].comp[COMP_1].local.current_run_time;
        if (comp_run_time[cycle] > 2)
        {
            return TRUE;
        }
    }

    return res;
}


/************************************************************************
@name  	: UnitBoardFreezeCheckState1
@brief 	: 
@param 	: None
@return	: None
@note   : 14_state1.板换冻结保护
            1.制冷运行,压缩机启动120后,环温<=0,或环温故障时,连续5s检测到Ts<=-4
*************************************************************************/
static uint8_t UnitBoardFreezeCheckState1(void)
{
    uint8_t res = FALSE;

    if ((GetUnitRealRunMode() == UNIT_MODE_HEAT) && (UnitBoardFreezeCheckCompRunTime()== TRUE))//单元制冷,压缩机运行满120s
    {

    }

    return res;
}

/************************************************************************
@name  	: UnitBoardFreezeCheckState2
@brief 	: 
@param 	: None
@return	: None
@note   : 14_state2.板换冻结保护
            2.制冷运行,环温<=0,或环温故障时,发生低压故障时,Ts<= -2
*************************************************************************/
static uint8_t UnitBoardFreezeCheckState2(void)
{
    uint8_t res = FALSE;


    return res;
}

/************************************************************************
@name  	: UnitBoardFreezeCheckState3
@brief 	: 
@param 	: None
@return	: None
@note   : 14_state3.板换冻结保护
            3.除霜中,压缩机运行3mins后,连续5s检测到Ts<=-4或Tevp<=-4
*************************************************************************/
static uint8_t UnitBoardFreezeCheckState3(void)
{
    uint8_t res = FALSE;


    return res;
}

/************************************************************************
@name  	: UnitBoardFreezeCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 14.板换冻结保护
            1.制冷运行,压缩机启动120后,环温<=0,或环温故障时,连续5s检测到Ts<=-4
            2.制冷运行,环温<=0,或环温故障时,发生低压故障时,Ts<= -2
            3.除霜中,压缩机运行3mins后,连续5s检测到Ts<=-4或Tevp<=-4
            任一发生,单元急停并报故障 //断电不复位
            复位判断:接到线控器复位命令 + 制冷出水过低报警值为12或5 + 制热出水过高报警值为30 //TODO ??没太明白
*************************************************************************/
static void UnitBoardFreezeCheck(void)
{
    if (unit_err.cell.board_freeze == FALSE)//无故障
    {
        if ((UnitBoardFreezeCheckState1() == TRUE)//单元制冷,压缩机运行满120s
            || (UnitBoardFreezeCheckState2() == TRUE)
            || (UnitBoardFreezeCheckState3() == TRUE))
        {
            unit_err.cell.board_freeze = TRUE;
        }
    }
}

/************************************************************************
@name  	: UnitWaterflowCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 15.单元水流故障
            水泵启动20s后,单元检测水流开关,检测连续5s断开,(水泵循环,防冻运行为10s),水泵停止
            显示故障,单元停止,60s后,连续5s接通,自动复位
            30mins内检测到3次水流故障(非共用水泵),不接受除防冻外的任何命令,水泵循环模式下,自动复位
*************************************************************************/
static void UnitWaterflowCheck(void)
{

    if (unit_err.cell.waterflow == FALSE)//无故障
    {

    }
}

/************************************************************************
@name  	: UnitComplinkCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 16.单元与压机驱动通信故障
            上电30s后,检测连续断开20s
            显示故障报警,单元关机,自动复位
*************************************************************************/
static void UnitComplinkCheck(void)
{
    // if (atw.unit.local.complink_state == TRUE)
    // {
    //     unit_err.cell.comp_link = FALSE;
    // }
    // else
    // {
    //     unit_err.cell.comp_link = TRUE;
    // }
}

/************************************************************************
@name  	: UnitFanlinkCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 17.单元与风机驱动通信故障
            上电30s后,检测连续断开20s
            显示故障报警,单元关机,自动复位
*************************************************************************/
static void UnitFanlinkCheck(void)
{
    // if (atw.unit.local.fanlink_state == TRUE)
    // {
    //     unit_err.cell.fan_link = FALSE;
    // }
    // else
    // {
    //     unit_err.cell.fan_link = TRUE;
    // }
}

/************************************************************************
@name  	: UnitInverterMatchCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 18.单元与驱动器不匹配
            机组上电初始化时,检测单元型号及驱动器型号,若不匹配,则报故障
            故障报警,断电复位
*************************************************************************/
static void UnitInverterMatchCheck(void)
{

    if (unit_err.cell.inv_match == FALSE)//无故障
    {

    }
}

/************************************************************************
@name  	: UnitAlarmStateCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 9.故障处理
            9.2 单元故障
*************************************************************************/
uint32_t UnitAlarmStateCheck(void)
{
    UnitAlarmType res;

    UnitChillerlinkCheck();         //1.主机与单元通信连续断开60s
    UnitPowerPowerAcinCheck();      //2.单元电源故障
    UnitPumpOverloadCheck();        //3.单元水泵过载
    UnitSensorTamCheck();           //4.环境温度传感器故障
    UnitCoolTempTamLowCheck();      //5.制冷环温过低报警
    UnitCoolTempTamHighCheck();     //6.制冷环温过高报警
    UnitHeatTempTamLowCheck();      //7.制热环温过低报警
    UnitHeatTempTamHighCheck();     //8.制热环温过高报警
    UnitSensorTwinCheck();          //9.回水温度传感器
    UnitSensorTwoutCheck();         //10.出水温度传感器
    UnitCoolTempoutLowCheck();      //11.制冷出水温度过低报警
    UnitHeatTempoutHighCheck();     //12.制热出水温度过高报警
    UnitHeatTempoutLowCheck();      //13.制热出水温度过低报警
    UnitBoardFreezeCheck();         //14.板换冻结保护
    UnitWaterflowCheck();           //15.单元水流故障
    UnitComplinkCheck();            //16.单元与压机驱动通信故障
    UnitFanlinkCheck();             //17.单元与风机驱动通信故障
    UnitInverterMatchCheck();       //18.单元与驱动器不匹配

    res = unit_err;

    return res.data;
}




