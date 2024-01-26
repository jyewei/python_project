/***********************************************************************
@file   : AlarmCycle.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "AlarmCycle.h"
#include "config.h"
#include "Data.h"
#include "TimerCounter.h"


#define TM_SENSOR_CHECK ((uint8_t)5)    //sensor检测时间5s
#define CNT_ERR_LOCK    ((uint8_t)4)    //故障发生的次数后锁定



enum
{
    ERR_TCIOL_SENSOR,       //0:翅片盘管温度传感器故障
    ERR_TD_SENSOR,          //1:排气温度传感器故障
    ERR_TS_SENSOR,          //2:气分温度传感器故障
    ERR_TSUB_IN_SENSOR,     //3:经济器进口温度传感器故障
    ERR_TSUB_OUT_SENSOR,    //4:经济器出口温度传感器故障
    ERR_PD_SENSOR,          //5:高压传感器故障 //Pd
    ERR_PS_SENSOR,          //6:低压传感器故障 //Ps
    ERR_TD_HIGH,            //7:排气温度过高
    ERR_PD_HIGH,            //8:排气压力过高
    ERR_PS_LOW,             //9:吸气压力过低
    ERR_FAN1_SPEED_LOW,     //10:风机转速过低
    ERR_FAN2_SPEED_LOW,     //11:风机转速过低
    ERR_FAN1_SPEED_HIGH,    //12:风机转速过高
    ERR_FAN2_SPEED_HIGH,    //13:风机转速过高
    ERR_INVERTER,           //14:变频器故障

    ERR_MAX

}CycleERREnum;


//
typedef struct
{
    uint8_t Tcoil_sensor_check;     //1.翅片盘管温度传感器
    uint8_t Td_sensor_check;        //2.排气温度传感器
    uint8_t Ts_sensor_check;        //3.气分进口温度传感器
    uint8_t Tsub_in_sensor_check;   //4.经济器进口温度传感器
    uint8_t Tsub_out_sensor_check;  //5.经济器出口温度传感器
    uint8_t Pd_sensor_check;        //6.高压传感器
    uint8_t Ps_sensor_check;        //7.低压传感器
    uint8_t Td_high_check;          //8.排气温度高
    uint8_t Pd_high_check;          //9.排气压力高
    uint8_t Ps_low_check;           //10.吸气压力低
    uint8_t fan1_speed_low_check;   //11.fan1风速低
    uint8_t fan2_speed_low_check;   //12.fan2风速低
    uint8_t fan1_speed_high_check;  //13.fan1风速高
    uint8_t fan2_speed_high_check;  //14.fan2风速高
    uint8_t inv_check;              //15.变频器故障

}CycleErrorTimeType;

static CycleErrorTimeType cycle_time[CYCLE_CH_MAX];
static CycleAlarmType cycle_err;            //
static CycleAlarmType cycle_sum_err;        //累计错误,需锁定
static CycleAlarmType cycle_err_reset;      //收到复位指令
static uint8_t cycle_err_cnt[ERR_MAX];//1hour的累加次数


/************************************************************************
@name  	: AlarmCycleTimerCounter
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void AlarmCycleTimerCounter(uint8_t cycle)
{
    uint8_t *pTime;
    uint16_t i,len;
//1s
    pTime = &cycle_time[cycle].Tcoil_sensor_check;
    if (TimerCheck(NO_CYCLE1_ALARM_BASE+cycle) == TRUE)
    {
        TimerSet(NO_CYCLE1_ALARM_BASE+cycle,1); //base 1s
        len = sizeof(CycleErrorTimeType)/sizeof(uint8_t);
        for ( i = 0; i < len; i++)
        {
            if (*(pTime + i) != 0)
            {
                (*(pTime + i))--;
            }
        }
    }
//1hour    
    if (TimerCheck(NO_CYCLE1_ALARM_1HOUR_BASE+cycle) == TRUE)//1hour累计故障用
    {
        TimerSet(NO_CYCLE1_ALARM_1HOUR_BASE+cycle,60); //base 1mins //1hour timer
    //清除累计值
        for (i = 0; i < ERR_MAX; i++)
        {
            cycle_err_cnt[i] = 0;//clear cnt
        }
    }
}


//===fan inverter
/************************************************************************
@name  	: GetFanAlarmState
@brief 	: 
@param 	: None
@return	: None
@note   : 9.故障处理  风机
*************************************************************************/
uint32_t GetFanAlarmState(uint8_t cycle,uint8_t fan)
{
    uint32_t res = FALSE;
    


    return res;
}

/************************************************************************
@name  	: CheckFanAlarmState
@brief 	: 
@param 	: None
@return	: None
@note   : 9.故障处理
            9.3 压机变频故障
*************************************************************************/
uint8_t CheckFanAlarmState(uint8_t cycle,uint8_t fan)
{
    uint8_t res = FALSE;
    


    return res;
}


//===comp inverter
/************************************************************************
@name  	: GetCompAlarmState
@brief 	: 
@param 	: None
@return	: None
@note   : 9.故障处理 压机故障
*************************************************************************/
uint32_t GetCompAlarmState(uint8_t cycle,uint8_t comp)
{
    uint32_t res = FALSE;
    


    return res;
}

/************************************************************************
@name  	: CheckCompAlarmState
@brief 	: 
@param 	: None
@return	: None
@note   : 9.故障处理
*************************************************************************/
uint8_t CheckCompAlarmState(uint8_t cycle,uint8_t comp)
{
    uint8_t res = FALSE;
    


    return res;
}


//===cycle alarm check
/************************************************************************
@name  	: CycleSensorTcoilCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 1.翅片盘管温度传感器故障
            上电检测,(-40 ~ 80)超出范围,持续5s,显示故障,正常运行;持续5s在范围内,自动复位
*************************************************************************/
static void CycleSensorTcoilCheck(uint8_t cycle)
{
    int16_t Tcoil = atw.cycle[cycle].io.Tcoil;

    if (cycle_err.cell.Tcoil_sensor == FALSE)//无故障时
    {
        if ((Tcoil <= -400) || (Tcoil >= 800))//故障
        {
            if (cycle_time[cycle].Tcoil_sensor_check == 0)
            {
                cycle_err.cell.Tcoil_sensor = TRUE;
                cycle_time[cycle].Tcoil_sensor_check = TM_SENSOR_CHECK;//重置计时器
            }
        }
        else
        {
            cycle_time[cycle].Tcoil_sensor_check = TM_SENSOR_CHECK;//重置计时器
        }
    }
    else//存在故障
    {
        if ((Tcoil <= -400) || (Tcoil >= 800))//故障未恢复
        {
            cycle_time[cycle].Tcoil_sensor_check = TM_SENSOR_CHECK;//重置计时器
        }
        else//故障恢复
        {
            if (cycle_time[cycle].Tcoil_sensor_check == 0)//持续时间达到
            {
                cycle_err.cell.Tcoil_sensor = FALSE;//clear error
                cycle_time[cycle].Tcoil_sensor_check = TM_SENSOR_CHECK;//重置计时器
            }
        }
    }
}

/************************************************************************
@name  	: CycleSensorTdCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 2.排气温度传感器故障
            上电检测,(-40 ~ 125)超出范围,持续5s,cycle停止运行并锁定;    手动复位
*************************************************************************/
static void CycleSensorTdCheck(uint8_t cycle)
{
    int16_t Td = atw.cycle[cycle].comp[COMP_1].local.Td;

    if (cycle_err.cell.Td_sensor == FALSE)//无故障时
    {
        if ((Td <= -400) || (Td >= 1250))//故障
        {
            if (cycle_time[cycle].Td_sensor_check == 0)
            {
                cycle_err.cell.Td_sensor = TRUE;
                // cycle_time[cycle].Td_sensor_check = TM_SENSOR_CHECK;//重置计时器
            }
        }
        else
        {
            cycle_time[cycle].Td_sensor_check = TM_SENSOR_CHECK;//重置计时器
        }
    }
    // else//存在故障   //手动复位
    // {
    //     if ((Td <= -400) || (Td >= 1250))//故障未恢复
    //     {
    //         cycle_time[cycle].Td_sensor_check = TM_SENSOR_CHECK;//重置计时器
    //     }
    //     else//故障恢复
    //     {
    //         if (cycle_time[cycle].Td_sensor_check == 0)//持续时间达到
    //         {
    //             cycle_err.cell.Td_sensor = FALSE;//clear error
    //             cycle_time[cycle].Td_sensor_check = TM_SENSOR_CHECK;//重置计时器
    //         }
    //     }
    // }
}

/************************************************************************
@name  	: CycleSensorTsCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 3.吸气温度传感器故障
            上电检测,(-40 ~ 80)超出范围,持续5s,cycle停止运行并锁定;    手动复位
*************************************************************************/
static void CycleSensorTsCheck(uint8_t cycle)
{
    int16_t Ts = atw.cycle[cycle].comp[COMP_1].local.Ts;

    if (cycle_err.cell.Ts_sensor == FALSE)//无故障时
    {
        if ((Ts <= -400) || (Ts >= 800))//故障
        {
            if (cycle_time[cycle].Ts_sensor_check == 0)
            {
                cycle_err.cell.Ts_sensor = TRUE;
                // cycle_time[cycle].Ts_sensor_check = TM_SENSOR_CHECK;//重置计时器
            }
        }
        else
        {
            cycle_time[cycle].Ts_sensor_check = TM_SENSOR_CHECK;//重置计时器
        }
    }
    // else//存在故障   //手动复位
    // {
    //     if ((Ts <= -400) || (Ts >= 800))//故障未恢复
    //     {
    //         cycle_time[cycle].Ts_sensor_check = TM_SENSOR_CHECK;//重置计时器
    //     }
    //     else//故障恢复
    //     {
    //         if (cycle_time[cycle].Ts_sensor_check == 0)//持续时间达到
    //         {
    //             cycle_err.cell.Ts_sensor = FALSE;//clear error
    //             cycle_time[cycle].Ts_sensor_check = TM_SENSOR_CHECK;//重置计时器
    //         }
    //     }
    // }
}

/************************************************************************
@name  	: CycleSensorTsubinCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 4.经济器进口温度传感器故障
            上电检测,(-40 ~ 80)超出范围,持续5s,显示故障,正常运行;持续5s在范围内,自动复位   //强制EXV3过关7步,EXV3不能开启,机组正常运行
*************************************************************************/
static void CycleSensorTsubinCheck(uint8_t cycle)
{
    int16_t Tsub_in = atw.cycle[cycle].io.Tsub_in;

    if (cycle_err.cell.Tsub_in_sensor == FALSE)//无故障时
    {
        if ((Tsub_in <= -400) || (Tsub_in >= 800))//故障
        {
            if (cycle_time[cycle].Tsub_in_sensor_check == 0)
            {
                atw.cycle[cycle].expv[EXPV3].flag_Tsub_err = TRUE;
                cycle_err.cell.Tsub_in_sensor = TRUE;
                cycle_time[cycle].Tsub_in_sensor_check = TM_SENSOR_CHECK;//重置计时器
            }
        }
        else
        {
            cycle_time[cycle].Tsub_in_sensor_check = TM_SENSOR_CHECK;//重置计时器
        }
    }
    else//存在故障
    {
        if ((Tsub_in <= -400) || (Tsub_in >= 800))//故障未恢复
        {
            cycle_time[cycle].Tsub_in_sensor_check = TM_SENSOR_CHECK;//重置计时器
        }
        else//故障恢复
        {
            if (cycle_time[cycle].Tsub_in_sensor_check == 0)//持续时间达到
            {
                cycle_err.cell.Tsub_in_sensor = FALSE;//clear error
                cycle_time[cycle].Tsub_in_sensor_check = TM_SENSOR_CHECK;//重置计时器
            }
        }
    }
}

/************************************************************************
@name  	: CycleSensorTsuboutCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 5.经济器出口温度传感器故障
            上电检测,(-40 ~ 80)超出范围,持续5s,显示故障,正常运行;持续5s在范围内,自动复位   //强制EXV3过关7步,EXV3不能开启,机组正常运行
*************************************************************************/
static void CycleSensorTsuboutCheck(uint8_t cycle)
{
    int16_t Tsub_out = atw.cycle[cycle].io.Tsub_out;

    if (cycle_err.cell.Tsub_out_sensor == FALSE)//无故障时
    {
        if ((Tsub_out <= -400) || (Tsub_out >= 800))//故障
        {
            if (cycle_time[cycle].Tsub_out_sensor_check == 0)
            {
                atw.cycle[cycle].expv[EXPV3].flag_Tsub_err = TRUE; //用于EXV3控制
                cycle_err.cell.Tsub_out_sensor = TRUE;
                cycle_time[cycle].Tsub_out_sensor_check = TM_SENSOR_CHECK;//重置计时器
            }
        }
        else
        {
            cycle_time[cycle].Tsub_out_sensor_check = TM_SENSOR_CHECK;//重置计时器
        }
    }
    else//存在故障
    {
        if ((Tsub_out <= -400) || (Tsub_out >= 800))//故障未恢复
        {
            cycle_time[cycle].Tsub_out_sensor_check = TM_SENSOR_CHECK;//重置计时器
        }
        else//故障恢复
        {
            if (cycle_time[cycle].Tsub_out_sensor_check == 0)//持续时间达到
            {
                cycle_err.cell.Tsub_out_sensor = FALSE;//clear error
                cycle_time[cycle].Tsub_out_sensor_check = TM_SENSOR_CHECK;//重置计时器
            }
        }
    }
}

/************************************************************************
@name  	: CycleSensorPdCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 6.高压传感器故障
            持续3mins吸气压力传感器压力低于0.07Mpa或高于4.6Mpa,则有故障,显示故障,cycle停止运行,手动复位
*************************************************************************/
static void CycleSensorPdCheck(uint8_t cycle)
{
    int16_t Pd = atw.cycle[cycle].comp[COMP_1].local.Pd;

    if (cycle_err.cell.Pd_sensor == FALSE)//无故障时
    {
        if ((Pd <= 70) || (Pd >= 4600))//故障
        {
            if (cycle_time[cycle].Pd_sensor_check == 0)
            {
                cycle_err.cell.Pd_sensor = TRUE;
                // cycle_time[cycle].Pd_sensor_check = 180;//重置计时器
            }
        }
        else
        {
            cycle_time[cycle].Pd_sensor_check = 180;//重置计时器
        }
    }
    // else//存在故障
    // {
    //     if ((Pd <= -400) || (Pd >= 800))//故障未恢复
    //     {
    //         cycle_time[cycle].Pd_sensor_check = 180;//重置计时器
    //     }
    //     else//故障恢复
    //     {
    //         if (cycle_time[cycle].Pd_sensor_check == 0)//持续时间达到
    //         {
    //             cycle_err.cell.Pd_sensor = FALSE;//clear error
    //             cycle_time[cycle].Pd_sensor_check = 180;//重置计时器
    //         }
    //     }
    // }
}

/************************************************************************
@name  	: CycleSensorPsCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 7.低压传感器故障
            持续3mins吸气压力传感器压力低于0.07Mpa或高于2.0Mpa,则有故障,显示故障,cycle停止运行,手动复位
*************************************************************************/
static void CycleSensorPsCheck(uint8_t cycle)
{
    int16_t Ps = atw.cycle[cycle].comp[COMP_1].local.Ps;

    if (cycle_err.cell.Ps_sensor == FALSE)//无故障时
    {
        if ((Ps <= 70) || (Ps >= 4600))//故障
        {
            if (cycle_time[cycle].Ps_sensor_check == 0)
            {
                cycle_err.cell.Ps_sensor = TRUE;
                // cycle_time[cycle].Ps_sensor_check = 180;//重置计时器
            }
        }
        else
        {
            cycle_time[cycle].Ps_sensor_check = 180;//重置计时器
        }
    }
    // else//存在故障
    // {
    //     if ((Ps <= -400) || (Ps >= 800))//故障未恢复
    //     {
    //         cycle_time[cycle].Ps_sensor_check = 180;//重置计时器
    //     }
    //     else//故障恢复
    //     {
    //         if (cycle_time[cycle].Ps_sensor_check == 0)//持续时间达到
    //         {
    //             cycle_err.cell.Ps_sensor = FALSE;//clear error
    //             cycle_time[cycle].Ps_sensor_check = 180;//重置计时器
    //         }
    //     }
    // }
}

/************************************************************************
@name  	: CycleTempTdHighCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 8.排气温度过高故障
            显示故障报警,cycle停止运行,自动复位  //1小时内连续发生4次,需手动复位
            除霜时,不显示故障报警,退出除霜
*************************************************************************/
static void CycleTempTdHighCheck(uint8_t cycle)//TODO 进入/退出条件,时间
{
    int16_t Td = atw.cycle[cycle].comp[COMP_1].local.Td;

    if (cycle_err.cell.Td_high == FALSE)//无故障时
    {
        if (Td >= 1500)//故障 //TODO 值待定 
        {
            if (cycle_time[cycle].Td_high_check == 0)
            {
                cycle_err.cell.Td_high = TRUE;
                cycle_err_cnt[ERR_TD_HIGH]++;
                cycle_time[cycle].Td_high_check = 180;//重置计时器
            }
        }
        else
        {
            cycle_time[cycle].Td_high_check = 180;//重置计时器
        }
    }
    else//存在故障
    {
        if (Td >= 1400)//故障未恢复
        {
            cycle_time[cycle].Td_sensor_check = 180;//重置计时器
        }
        else//故障恢复
        {
            if (cycle_time[cycle].Td_sensor_check == 0)//持续时间达到
            {
                cycle_err.cell.Td_sensor = FALSE;//clear error
            }
        }
    }
//锁定故障判断
    if (cycle_err_cnt[ERR_TD_HIGH] > (CNT_ERR_LOCK - 1))//
    {
        cycle_sum_err.cell.Td_high = TRUE; //
    }
    if (cycle_sum_err.cell.Td_high == TRUE)//锁定时,不能自动清除
    {
        cycle_err.cell.Td_high = TRUE;
    } 
}

/************************************************************************
@name  	: CyclePressurePdHighCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 9.排气压力过高故障
            显示故障报警,cycle停止运行,自动复位  //1小时内连续发生4次,需手动复位
*************************************************************************/
static void CyclePressurePdHighCheck(uint8_t cycle)//TODO 进入/退出条件,时间
{
    int16_t Pd = atw.cycle[cycle].comp[COMP_1].local.Pd;

    if (cycle_err.cell.Pd_high == FALSE)//无故障时
    {
        if (Pd >= 4600)//故障 //TODO 值待定 
        {
            if (cycle_time[cycle].Pd_high_check == 0)
            {
                cycle_err.cell.Pd_high = TRUE;
                cycle_err_cnt[ERR_PD_HIGH]++;
                cycle_time[cycle].Pd_high_check = 180;//重置计时器
            }
        }
        else
        {
            cycle_time[cycle].Pd_high_check = 180;//重置计时器
        }
    }
    else//存在故障
    {
        if (Pd >= 1400)//故障未恢复
        {
            cycle_time[cycle].Pd_sensor_check = 180;//重置计时器
        }
        else//故障恢复
        {
            if (cycle_time[cycle].Pd_sensor_check == 0)//持续时间达到
            {
                cycle_err.cell.Pd_sensor = FALSE;//clear error
            }
        }
    }
//锁定故障判断
    if (cycle_err_cnt[ERR_PD_HIGH] > (CNT_ERR_LOCK - 1))//
    {
        cycle_sum_err.cell.Pd_high = TRUE; //
    }
    if (cycle_sum_err.cell.Pd_high == TRUE)//锁定时,不能自动清除
    {
        cycle_err.cell.Pd_high = TRUE;
    } 
}

/************************************************************************
@name  	: CyclePressurePsLowCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 10.吸气压力过低故障
            显示故障报警,cycle停止运行,自动复位  //1小时内连续发生4次,需手动复位
*************************************************************************/
static void CyclePressurePsLowCheck(uint8_t cycle)//TODO 进入/退出条件,时间
{
    int16_t Ps = atw.cycle[cycle].comp[COMP_1].local.Ps;

    if (cycle_err.cell.Ps_low == FALSE)//无故障时
    {
        if (Ps <= 600)//故障 //TODO 值待定 
        {
            if (cycle_time[cycle].Ps_low_check == 0)
            {
                cycle_err.cell.Ps_low = TRUE;
                cycle_err_cnt[ERR_PS_LOW]++;
                cycle_time[cycle].Ps_low_check = 5;//重置计时器
            }
        }
        else
        {
            cycle_time[cycle].Ps_low_check = 5;//重置计时器
        }
    }
    else//存在故障
    {
        if (Ps >= 1400)//故障未恢复
        {
            cycle_time[cycle].Ps_low_check = 5;//重置计时器
        }
        else//故障恢复
        {
            if (cycle_time[cycle].Ps_low_check == 0)//持续时间达到
            {
                cycle_err.cell.Ps_low = FALSE;//clear error
            }
        }
    }
//锁定故障判断
    if (cycle_err_cnt[ERR_PS_LOW] > (CNT_ERR_LOCK - 1))//
    {
        cycle_sum_err.cell.Ps_low = TRUE; //
    }
    if (cycle_sum_err.cell.Ps_low == TRUE)//锁定时,不能自动清除
    {
        cycle_err.cell.Ps_low = TRUE;
    } 
}

/************************************************************************
@name  	: CycleFan1SpeedLowCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 12.风机1转速过低故障
            显示故障报警,cycle停止运行,自动复位  //1小时内连续发生9次,需手动复位
*************************************************************************/
static void CycleFan1SpeedLowCheck(uint8_t cycle)
{
    uint16_t fan_speed = atw.cycle[cycle].fan[FAN_1].link.state.foc_freq_output;

    if (fan_speed > 0)//运行中
    {   
        if (cycle_err.cell.fan1_speed_low == FALSE)//无故障时
        {
            if (fan_speed < 100)//故障
            {
                if (cycle_time[cycle].fan1_speed_low_check == 0)
                {
                    cycle_err.cell.fan1_speed_low = TRUE;
                    cycle_err_cnt[ERR_FAN1_SPEED_LOW]++;
                    cycle_time[cycle].fan1_speed_low_check = 35;//重置计时器
                }
            }
            else
            {
                cycle_time[cycle].fan1_speed_low_check = 35;//重置计时器
            }
        }
    }
    else//speed = 0
    {
        cycle_err.cell.fan1_speed_low = FALSE;//clear error
    }            
//锁定故障判断
    if (cycle_err_cnt[ERR_FAN1_SPEED_LOW] > (CNT_ERR_LOCK +4))//默认4次,此处为9次
    {
        cycle_sum_err.cell.fan1_speed_low = TRUE; //
    }
    if (cycle_sum_err.cell.fan1_speed_low == TRUE)//锁定时,不能自动清除
    {
        cycle_err.cell.fan1_speed_low = TRUE;
    } 
}

/************************************************************************
@name  	: CycleFan2SpeedLowCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 13.风机2转速过低故障
            显示故障报警,cycle停止运行,自动复位  //1小时内连续发生9次,需手动复位
*************************************************************************/
static void CycleFan2SpeedLowCheck(uint8_t cycle)
{
    uint16_t fan_speed = atw.cycle[cycle].fan[FAN_2].link.state.foc_freq_output;

    if (fan_speed > 0)//运行中
    {   
        if (cycle_err.cell.fan2_speed_low == FALSE)//无故障时
        {
            if (fan_speed < 100)//故障
            {
                if (cycle_time[cycle].fan2_speed_low_check == 0)
                {
                    cycle_err.cell.fan2_speed_low = TRUE;
                    cycle_err_cnt[ERR_FAN2_SPEED_LOW]++;
                    cycle_time[cycle].fan2_speed_low_check = 35;//重置计时器
                }
            }
            else
            {
                cycle_time[cycle].fan2_speed_low_check = 35;//重置计时器
            }
        }
    }
    else//speed = 0
    {
        cycle_err.cell.fan2_speed_low = FALSE;//clear error
    }            
//锁定故障判断
    if (cycle_err_cnt[ERR_FAN2_SPEED_LOW] > (CNT_ERR_LOCK +4))//默认4次,此处为9次
    {
        cycle_sum_err.cell.fan2_speed_low = TRUE; //
    }
    if (cycle_sum_err.cell.fan2_speed_low == TRUE)//锁定时,不能自动清除
    {
        cycle_err.cell.fan2_speed_low = TRUE;
    } 
}

/************************************************************************
@name  	: CycleFan1SpeedHighCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 14.风机1转速高低故障
            显示故障报警,cycle停止运行,自动复位  //1小时内连续发生9次,需手动复位
*************************************************************************/
static void CycleFan1SpeedHighCheck(uint8_t cycle)
{
    uint16_t fan_speed = atw.cycle[cycle].fan[FAN_1].link.state.foc_freq_output;

    if (fan_speed > 0)//运行中
    {   
        if (cycle_err.cell.fan1_speed_high == FALSE)//无故障时
        {
            if (fan_speed > 1100)//故障
            {
                if (cycle_time[cycle].fan1_speed_high_check == 0)
                {
                    cycle_err.cell.fan1_speed_high = TRUE;
                    cycle_err_cnt[ERR_FAN1_SPEED_HIGH]++;
                    cycle_time[cycle].fan1_speed_high_check = 35;//重置计时器
                }
            }
            else
            {
                cycle_time[cycle].fan1_speed_high_check = 35;//重置计时器
            }
        }
    }
    else//speed = 0
    {
        cycle_err.cell.fan1_speed_high = FALSE;//clear error
    }            
//锁定故障判断
    if (cycle_err_cnt[ERR_FAN1_SPEED_HIGH] > (CNT_ERR_LOCK +4))//默认4次,此处为9次
    {
        cycle_sum_err.cell.fan1_speed_high = TRUE; //
    }
    if (cycle_sum_err.cell.fan1_speed_high == TRUE)//锁定时,不能自动清除
    {
        cycle_err.cell.fan1_speed_high = TRUE;
    } 
}

/************************************************************************
@name  	: CycleFan2SpeedHighCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 15.风机2转速高低故障
            显示故障报警,cycle停止运行,自动复位  //1小时内连续发生9次,需手动复位
*************************************************************************/
static void CycleFan2SpeedHighCheck(uint8_t cycle)
{
    uint16_t fan_speed = atw.cycle[cycle].fan[FAN_2].link.state.foc_freq_output;

    if (fan_speed > 0)//运行中
    {   
        if (cycle_err.cell.fan2_speed_high == FALSE)//无故障时
        {
            if (fan_speed > 1100)//故障
            {
                if (cycle_time[cycle].fan2_speed_high_check == 0)
                {
                    cycle_err.cell.fan2_speed_high = TRUE;
                    cycle_err_cnt[ERR_FAN2_SPEED_HIGH]++;
                    cycle_time[cycle].fan2_speed_high_check = 35;//重置计时器
                }
            }
            else
            {
                cycle_time[cycle].fan2_speed_high_check = 35;//重置计时器
            }
        }
    }
    else//speed = 0
    {
        cycle_err.cell.fan2_speed_high = FALSE;//clear error
    }            
//锁定故障判断
    if (cycle_err_cnt[ERR_FAN2_SPEED_HIGH] > (CNT_ERR_LOCK +4))//默认4次,此处为9次
    {
        cycle_sum_err.cell.fan2_speed_high = TRUE; //
    }
    if (cycle_sum_err.cell.fan2_speed_high == TRUE)//锁定时,不能自动清除
    {
        cycle_err.cell.fan2_speed_high = TRUE;
    } 
}

/************************************************************************
@name  	: CycleInverterCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 16.变频器故障
            待机时,自动复位  //1小时内连续发生6次,需手动复位
            变频故障见变频通信内的故障位(上电130s后检测);
            或正常调节时,系统目标值>=Fmin,如果获取到的实际变频压机频率为0,且维持10s,则报变频故障
*************************************************************************/
static void CycleInverterCheck(uint8_t cycle)//TODO 未完成
{
    uint16_t comp_rt_freq = atw.cycle[cycle].comp[COMP_1].link.state.freq_output;//实际输出频率
    uint16_t comp_set_freq = atw.cycle[cycle].comp[COMP_1].link.ctrl.freq_set;  //设置频率

    //1.获取变频故障状态


    //2.实际值为0,设定值不为0,且维持10s
    if (cycle_err.cell.inverter == FALSE)//无故障时
    {


    }
            
//锁定故障判断
    if (cycle_err_cnt[ERR_INVERTER] > (CNT_ERR_LOCK +1))//默认4次,此处为6次
    {
        cycle_sum_err.cell.inverter = TRUE; //
    }
    if (cycle_sum_err.cell.inverter == TRUE)//锁定时,不能自动清除
    {
        cycle_err.cell.inverter = TRUE;
    } 
}



/************************************************************************
@name  	: CycleAlarmStateCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 9.故障处理
            9.3 cycle故障
*************************************************************************/
uint32_t CycleAlarmStateCheck(uint8_t cycle)
{
    CycleAlarmType res;

    CycleSensorTcoilCheck(cycle);   //1.翅片盘管温度传感器故障
    CycleSensorTdCheck(cycle);      //2.排气温度传感器故障
    CycleSensorTsCheck(cycle);      //3.吸气温度传感器故障
    CycleSensorTsubinCheck(cycle);  //4.经济器进口温度传感器故障
    CycleSensorTsuboutCheck(cycle); //5.经济器出口温度传感器故障
    CycleSensorPdCheck(cycle);      //6.高压传感器故障
    CycleSensorPsCheck(cycle);      //7.低压传感器故障
    CycleTempTdHighCheck(cycle);    //8.排气温度过高故障
    CyclePressurePdHighCheck(cycle);//9.排气压力过高故障
    CyclePressurePsLowCheck(cycle); //10.吸气压力过低故障
    CycleFan1SpeedLowCheck(cycle);  //12.风机1转速过低故障
    CycleFan2SpeedLowCheck(cycle);  //13.风机2转速过低故障
    CycleFan1SpeedHighCheck(cycle); //14.风机1转速高低故障
    CycleFan2SpeedHighCheck(cycle); //15.风机2转速高低故障
    CycleInverterCheck(cycle);      //16.变频器故障

    res = cycle_err;

    return res.data;
}






