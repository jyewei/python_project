/***********************************************************************
@file   : AlarmCheck.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
@note   : 9.故障处理
            自动复位:指不需要经过人为动作,故障可以自己复位
            手动复位:分为断电手动复位和线控器手动复位
                断电手动复位:指需要切断机组电源才可以复位
                线控器手动复位:指线控器上有手动复位功能,通过线控器可以复位
            //TODO 增加复位的处理
************************************************************************/
#include "AlarmCheck.h"
#include "safety_function.h"
#include "config.h"
#include "Data.h"
#include "TimerCounter.h"
#include "AlarmCycle.h"
#include "SysStateControl.h"
#include "IOProcess.h"


#define TM_PUMP_RUN_CHECK_WAIT  ((uint8_t)20)   //水泵启动20s后检测水流开关


typedef struct
{
    uint8_t sensor_Tw_sys_out;          //总出水温度传感器检测时间
    uint8_t cool_tw_out_temp_low_wait;  //
    uint8_t heat_tw_out_temp_high;
//水泵,水流
    uint8_t pump_run_check_wait;
    uint8_t waterflow_switch_on_check;
    uint8_t waterflow_switch_reset_wait;
    uint8_t waterflow_switch_on_keep;
    uint8_t waterflow_switch_off_keep;
    uint8_t pump_overload_fb_check;

}SysErrorTimeType;

static SysErrorTimeType sys_time;
static SysAlarmType     sys_err;        //系统故障
static SysAlarmType     sys_err_reset;  //复位

static uint8_t err_Tw_sys_out_lock;     //故障锁定,手动复位
static uint8_t err_waterflow_switch_lock;   //水流开关故障锁定
static uint8_t flag_waterflow_switch_on;

/************************************************************************
@name  	: AlarmSysTimerCounter
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void AlarmSysTimerCounter(void)
{
    uint8_t *pTime;
    uint16_t i,len;
//1s
    pTime = &sys_time.sensor_Tw_sys_out;
    if (TimerCheck(NO_SYS_ALARM_BASE) == TRUE)
    {
        TimerSet(NO_SYS_ALARM_BASE,1); //base 1s
        len = sizeof(SysErrorTimeType)/sizeof(uint8_t);
        for ( i = 0; i < len; i++)
        {
            if (*(pTime + i) != 0)
            {
                (*(pTime + i))--;
            }
        }
    }
}

/************************************************************************
@name  	: SysSensorTwsysoutCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 1.总出水温度传感器检测
*************************************************************************/
static void SysSensorTwsysoutCheck(void) 
{
    int16_t Tw_sys_out = atw.io.sensor[SENSOR_Tw_sys_out].rt_value;

//需要检测
    if (((atw.sys.param.set.cell.cool_temp_chose == 1) || (atw.sys.param.set.cell.heat_temp_chose == 1))//出水温控控制
        && (atw.sys.local.unit_active_cnt > 1))//有多台unit
    {
        if (sys_err.cell.sensor_Tw_sys_out == FALSE)//无故障时
        {
            if ((Tw_sys_out <= -400) || (Tw_sys_out >= 800))//故障
            {
                if (sys_time.sensor_Tw_sys_out == 0)
                {
                    if ((GetSysSetRunMode() == SYS_MODE_COOL) || (GetSysSetRunMode() == SYS_MODE_HEAT))
                    {
                        err_Tw_sys_out_lock = TRUE;
                        sys_time.sensor_Tw_sys_out = 5;//重置计时器
                    }
                    sys_err.cell.sensor_Tw_sys_out = TRUE;
                }
            }
            else
            {
                sys_time.sensor_Tw_sys_out = 5;//重置计时器
            }
        }
        else//存在故障
        {
            if ((Tw_sys_out <= -400) || (Tw_sys_out >= 800))//故障未恢复
            {
                sys_time.sensor_Tw_sys_out = 5;//重置计时器
            }
            else//故障恢复 //TODO 手动复位
            {
                if (sys_time.sensor_Tw_sys_out == 0)//持续时间达到
                {
                    if (err_Tw_sys_out_lock == FALSE)
                    {
                        sys_err.cell.sensor_Tw_sys_out = FALSE;//clear error
                        sys_time.sensor_Tw_sys_out = 5;//重置计时器
                    }
                }
            }
        }
    }
//不需要检测,没用到    
    else
    {
        sys_err.cell.sensor_Tw_sys_out = FALSE;
    }
}

/************************************************************************
@name  	: SysCoolTwoutLowCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 2.制冷总出水温度过低
*************************************************************************/
static void SysCoolTwoutLowCheck(void) 
{
    int16_t Tw_sys_out = atw.sys.table.unit[CHILLER_0].io.Tw_sys_out;   //总出水温度
    int16_t Tw_alarm = atw.sys.param.temp_out_water_too_low;            //[出水温度过低报警]

    if (sys_err.cell.cool_tw_out_temp_low == FALSE)//无故障时
    {
        if ((Tw_sys_out <= Tw_alarm)&& (GetSysSetRunMode() == SYS_MODE_COOL))
        {
            if (sys_time.cool_tw_out_temp_low_wait == 0) 
            {
                sys_err.cell.cool_tw_out_temp_low = TRUE;
            }
        }
        else
        {
            sys_time.cool_tw_out_temp_low_wait = 2;
        }
    }
    else//有故障 //
    {
        if (Tw_sys_out >= (Tw_alarm + 60))
        {
            sys_err.cell.cool_tw_out_temp_low = FALSE;
        }
    }
}

/************************************************************************
@name  	: SysHeatTwoutHighCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 3.制热总出水温度过高
*************************************************************************/
static void SysHeatTwoutHighCheck(void) 
{
    int16_t Tw_sys_out = atw.sys.table.unit[CHILLER_0].io.Tw_sys_out;   //总出水温度
    int16_t Tw_alarm = atw.sys.param.temp_out_water_too_high;            //[出水温度过高报警]

    if (sys_err.cell.heat_tw_out_temp_high == FALSE)//无故障时
    {
        if ((Tw_sys_out >= Tw_alarm)&& (GetSysSetRunMode() == SYS_MODE_HEAT))
        {
            if (sys_time.heat_tw_out_temp_high == 0) 
            {
                sys_err.cell.heat_tw_out_temp_high = TRUE;
            }
        }
        else
        {
            sys_time.heat_tw_out_temp_high = 2;
        }
    }
    else//有故障 //
    {
        if (Tw_sys_out <= (Tw_alarm - 60))
        {
            sys_err.cell.heat_tw_out_temp_high = FALSE;
        }
    }
}

/************************************************************************
@name  	: SysWaterflowSwitchCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 4.系统总水流故障
            水泵启动20s后检测水流开关,
                180s内未检测到总水流开关变成接通状态(持续5s),
                或,断开持续5s
                水泵停止运行,报警
@note   :       
*************************************************************************/
static uint8_t SysWaterflowSwitchOnCheck(void)
{
    uint8_t res = TRUE;
    uint8_t switch_state = atw.sys.table.unit[CHILLER_0].io.DIO.bit.sys_flow_switch;

    if (flag_waterflow_switch_on == FALSE)
    {
        if (switch_state == OFF)//水流开关断开
        {
            sys_time.waterflow_switch_off_keep = 5;
        }

        if (sys_time.waterflow_switch_off_keep == 0)//接通保持5s
        {
            res = FALSE;
            flag_waterflow_switch_on = TRUE;
        }
    }
    
    return res;
}

/************************************************************************
@name  	: SysWaterflowSwitchCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 4.系统总水流故障
            水泵启动20s后检测水流开关,
                180s内未检测到总水流开关变成接通状态(持续5s),
                或,断开持续5s
                水泵停止运行,报警
@note   :       
*************************************************************************/
static void SysWaterflowSwitchCheck(void)//TODO 
{
    uint8_t switch_state = atw.sys.table.unit[CHILLER_0].io.DIO.bit.sys_flow_switch;

    if (atw.sys.table.unit[CHILLER_0].io.DIO.bit.water_pump == OFF)
    {
        sys_time.pump_run_check_wait = TM_PUMP_RUN_CHECK_WAIT;
        sys_time.waterflow_switch_on_check = 180;
        sys_time.waterflow_switch_off_keep = 5;
        sys_time.waterflow_switch_on_keep = 5;
        flag_waterflow_switch_on = FALSE;
    }
//check
    if (sys_err.cell.waterflow_switch == FALSE)//无故障
    {
        if (sys_time.pump_run_check_wait == 0)//水泵启动20s后检测
        {
        //1.180s后仍未检测到总水流开关从断开变成接通(持续5s)
            if (SysWaterflowSwitchOnCheck() == FALSE)//未接通过
            {
                if (sys_time.waterflow_switch_on_check == 0)//持续180s
                {
                    sys_err.cell.waterflow_switch = TRUE;
                }
            }
            else//接通过 //2.总水流开关从导通变成断开(持续断开5s以上)
            {
                if (switch_state == ON)//水流开关接通
                {
                    sys_time.waterflow_switch_on_keep = 5;
                }
                if (sys_time.waterflow_switch_on_keep == 0)//有持续5s断开
                {
                    sys_err.cell.waterflow_switch = TRUE;
                }
            }
            //
            if (sys_err.cell.waterflow_switch == TRUE)//TODO 具体逻辑
            {
                switch (atw.sys.local.sys_set_mode)
                {
                    case SYS_MODE_COOL:     
                    case SYS_MODE_HEAT:  sys_time.waterflow_switch_reset_wait = 60;   break;
                    case SYS_MODE_ANTIFREEZE:       break;
                    case SYS_MODE_PUMP:             break;
                    case SYS_MODE_NULL:             break;
                    default:break;
                }
            }
        }
    }
    else//有故障
    {
        if (sys_time.waterflow_switch_reset_wait == 0)//复位条件
        {
            sys_err.cell.waterflow_switch = FALSE;
        }
    }
}

/************************************************************************
@name  	: SysPumpOverloadCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 5.共用水泵过载
            水泵启动20s后检测水泵过载信号,连续5s断开,水泵停止,报故障
*************************************************************************/
static void SysPumpOverloadCheck(void) 
{
    uint8_t pump_fb_state = atw.sys.table.unit[CHILLER_0].io.DIO.bit.pump_over_fb;

//check
    if (sys_err.cell.public_pump_overload == FALSE)//无故障
    {
        if (sys_time.pump_run_check_wait == 0)//水泵启动20s后检测水泵过载信号
        {
            if (pump_fb_state == OFF)//故障
            {
                if (sys_time.pump_overload_fb_check == 0)//持续5s
                {
                    sys_time.pump_overload_fb_check = 5;//重置计时器
                    sys_err.cell.public_pump_overload = TRUE;
                }
            }
            else
            {
                sys_time.pump_overload_fb_check = 5;//重置计时器
            }
        }
    }
    else//有故障
    {
        if (sys_time.pump_run_check_wait == 0)//水泵启动20s后检测水泵过载信号
        {
            if (pump_fb_state == OFF)//故障
            {
                sys_time.pump_overload_fb_check = 5;//重置计时器
            }
            else
            {
                if (sys_time.pump_overload_fb_check == 0)//持续5s接通
                {
                    sys_err.cell.public_pump_overload = FALSE;//复位
                }
            }
        }
    }
}

/************************************************************************
@name  	: SysGrouplinkCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 6.线控器通信故障
            断开60s
*************************************************************************/
static void SysGrouplinkCheck(void) 
{
    if ((atw.sys.param.device_valid.bit.mode_group == TRUE)
        || (atw.sys.param.device_valid.bit.switch_group == TRUE))
    {
        if (atw.sys.local.grouplink_state == TRUE)
        {
            sys_err.cell.grouplink = TRUE;
        }
        else sys_err.cell.grouplink = FALSE;
    }
    else
    {
        sys_err.cell.grouplink = TRUE;
    }
}

/************************************************************************
@name  	: SysSmartThrmlinkCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 7.智能温控器通信故障
            断开60s
*************************************************************************/
static void SysSmartThrmlinkCheck(void) 
{
    if ((atw.sys.param.device_valid.bit.mode_thrm == TRUE)
        || (atw.sys.param.device_valid.bit.switch_thrm == TRUE))
    {
        if (atw.sys.local.thrmlink_state == TRUE)
        {
            sys_err.cell.thrmlink = TRUE;
        }
        else sys_err.cell.thrmlink = FALSE;
    }
    else
    {
        sys_err.cell.thrmlink = TRUE;
    }
}

/************************************************************************
@name  	: SysBASlinkCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 8.BAS通信故障
            断开60s
*************************************************************************/
static void SysBASlinkCheck(void) 
{
    if ((atw.sys.param.device_valid.bit.mode_BAS == TRUE)
        || (atw.sys.param.device_valid.bit.switch_BAS == TRUE))
    {
        if (atw.sys.local.BASlink_state == TRUE)
        {
            sys_err.cell.BASlink = TRUE;
        }
        else sys_err.cell.BASlink = FALSE;
    }
    else
    {
        sys_err.cell.BASlink = TRUE;
    }
}

/************************************************************************
@name  	: SysCentrelinkCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 9.CC通信故障
            断开60s
*************************************************************************/
static void SysCentrelinkCheck(void) 
{
    if ((atw.sys.param.device_valid.bit.mode_cc == TRUE)
        || (atw.sys.param.device_valid.bit.switch_cc == TRUE))
    {
        if (atw.sys.local.centrelink_state == TRUE)
        {
            sys_err.cell.centrelink = TRUE;
        }
        else sys_err.cell.centrelink = FALSE;
    }
    else
    {
        sys_err.cell.centrelink = TRUE;
    }
}

/************************************************************************
@name  	: SysAntifreezeCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 10.冻结故障
*************************************************************************/
static void SysAntifreezeCheck(void)//TODO tam故障
{
    int16_t Tam = atw.sys.table.unit[CHILLER_0].io.Tam;
    uint8_t tam_err = atw.unit.local.sns_err.bit.Tam;

    if (GetSysRealRunMode() == SYS_MODE_ANTIFREEZE)//防冻模式
    {
        if (sys_err.cell.water_freeze == FALSE)
        {
            if (sys_err.cell.waterflow_switch == TRUE)//水流开关故障
            {
                if ((Tam <= -10) || (tam_err == TRUE))//Tam<=-1或故障
                {
                    /* code */
                }
            }
        }
    }
//恢复检测   
    if (sys_err.cell.water_freeze == TRUE)//有故障
    {
        if ((Tam >= 10) || (atw.sys.local.Tw >= 100))//自动恢复
        {
            sys_err.cell.water_freeze = FALSE;
        }
    }
}

/************************************************************************
@name  	: SysTimeoutCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 11.时限保护
*************************************************************************/
static void SysTimeoutCheck(void)
{
    uint32_t run_time = atw.sys.local.total_run_time;   //机组运行时间//mins
    uint8_t  limit_state = atw.sys.param.set.cell.unit_time_limit;
    
    if (limit_state == TRUE)
    {
        if (run_time >= (atw.sys.param.time_unit_set*60))
        {
            sys_err.cell.timeout = TRUE;
        }
    }
    else
    {
        sys_err.cell.timeout = FALSE;
    }
}

/************************************************************************
@name  	: SysTimeoutCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 12.单元数量不匹配
*************************************************************************/
static void SysUnitCounterCheck(void) 
{
    if (atw.sys.local.chillerlink_cnt_err == TRUE)
    {
        sys_err.cell.unit_cnt = TRUE;
    }
    else sys_err.cell.unit_cnt = FALSE;

}

/************************************************************************
@name  	: SysUnitAddrDuplicateCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 13.单元地址冲突
*************************************************************************/
static void SysUnitAddrDuplicateCheck(void) 
{
    if (atw.sys.local.chillerlink_addr_err == TRUE)
    {
        sys_err.cell.unit_addr_duplicate = TRUE;
    }
    else sys_err.cell.unit_addr_duplicate = FALSE;
}

/************************************************************************
@name  	: SysThrmCounterCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 14.智能温控器数量不匹配
*************************************************************************/
static void SysThrmCounterCheck(void) 
{
    if (atw.sys.local.thrmlink_cnt_err == TRUE)
    {
        sys_err.cell.thrm_cnt = TRUE;
    }
    else sys_err.cell.thrm_cnt = FALSE;
}

/************************************************************************
@name  	: SysUnitAddrDuplicateCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 15.智能温控器地址冲突
*************************************************************************/
static void SysThrmAddrDuplicateCheck(void)
{
    if (atw.sys.local.thrmlink_addr_err == TRUE)
    {
        sys_err.cell.thrm_addr_duplicate = TRUE;
    }
    else sys_err.cell.thrm_addr_duplicate = FALSE;
}

/************************************************************************
@name  	: SysMasterPowerCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 16.0#主机电源故障
*************************************************************************/
static void SysMasterPowerCheck(void)//TODO
{
    uint8_t pump_type = atw.sys.table.unit[CHILLER_0].init.dsw1.bit.pump_type;
    
    if (pump_type == PUMP_PUBLIC)
    {
        
    }
}

/************************************************************************
@name  	: SysUnitBlendCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 17.单元混拼故障
            3.3.3不同类型单元混拼限制
                1.标准热泵(加防冻液),不能与标准热泵混拼
                2.标准热泵,不能与低温热泵混拼
*************************************************************************/
static void SysUnitBlendCheck(void)//TODO 混拼待定
{
    uint8_t SA_cnt = 0,SA_add_cnt = 0,CA_cnt = 0,CA_add_cnt = 0;
    uint8_t ch;
    
    for (ch = CHILLER_0; ch < CHILLER_CH_MAX; ch++)
    {
        if (atw.sys.local.unit[ch].active == TRUE)
        {
            if (atw.sys.table.unit[ch].init.dsw1.bit.temp_type == TEMP_SA)
            {
                if (atw.sys.table.unit[ch].init.dsw1.bit.refrigent_type == ADD_ANTIFREEZE)
                {
                    SA_add_cnt++;
                }
                else
                {
                    SA_cnt++;
                }
            }
            else
            {
                if (atw.sys.table.unit[ch].init.dsw1.bit.refrigent_type == ADD_ANTIFREEZE)
                {
                    CA_add_cnt++;
                }
                else
                {
                    CA_cnt++;
                }
            }
        }
    }

}

/************************************************************************
@name  	: SysChillerSlavelinkCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 18.主从通信故障
*************************************************************************/
static void SysChillerSlavelinkCheck(void)//TODO 单元故障
{
    // uint8_t capacity = atw.sys.table.unit[CHILLER_0].init.dsw1.bit.capacity;
    
    // if (atw.sys.local.centrelink_state == TRUE)
    // {
    //     sys_err.cell.centrelink = TRUE;
    // }
    // else sys_err.cell.centrelink = FALSE;

    sys_err.cell.chillerslavelink = FALSE;
}

/************************************************************************
@name  	: SysHwSwMatchCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 19.软件和硬件不匹配故障
*************************************************************************/
static void SysHwSwMatchCheck(void)//TODO
{

    
    sys_err.cell.hw_sw_match = FALSE;
}


/************************************************************************
@name  	: SysAlarmStateCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 9.故障处理
            9.2 单元故障
*************************************************************************/
uint32_t SysAlarmStateCheck(void)
{
    SysAlarmType res;

    SysSensorTwsysoutCheck();       //1.总出水温度传感器检测
    SysCoolTwoutLowCheck();         //2.制冷总出水温度过低
    SysHeatTwoutHighCheck();        //3.制热总出水温度过高
    SysWaterflowSwitchCheck();      //4.系统总水流故障
    SysPumpOverloadCheck();         //5.共用水泵过载
    SysGrouplinkCheck();            //6.线控器通信故障
    SysSmartThrmlinkCheck();        //7.智能温控器通信故障
    SysBASlinkCheck();              //8.BAS通信故障
    SysCentrelinkCheck();           //9.CC通信故障
    SysAntifreezeCheck();           //10.冻结故障
    SysTimeoutCheck();              //11.时限保护
    SysUnitCounterCheck();          //12.单元数量不匹配
    SysUnitAddrDuplicateCheck();    //13.单元地址冲突
    SysThrmCounterCheck();          //14.智能温控器数量不匹配
    SysThrmAddrDuplicateCheck();    //15.智能温控器地址冲突
    SysMasterPowerCheck();          //16.0#主机电源故障
    SysUnitBlendCheck();            //17.单元混拼故障
    SysChillerSlavelinkCheck();     //18.主从通信故障
    SysHwSwMatchCheck();            //19.软件和硬件不匹配故障

    res = sys_err;

    return res.data;
}



//=== 

/************************************************************************
@name  	: GetCycleAlarmUpdate
@brief 	: 
@param 	: None
@return	: None
@note   : 刷新cycle故障
           并将状态更新到cycle中
*************************************************************************/
uint32_t GetCycleAlarmUpdate(uint8_t cycle)
{
    atw.cycle[cycle].alarm_state = CycleAlarmStateCheck(cycle);
//状态更新到cycle中    
    atw.cycle[cycle].io.sns_err.bit.Tcoil = (atw.cycle[cycle].alarm_state & 0x01);
    if ((atw.cycle[cycle].alarm_state & 0x08) != 0 )
        atw.cycle[cycle].io.sns_err.bit.Tsub_in = TRUE;
    else atw.cycle[cycle].io.sns_err.bit.Tsub_in = FALSE;

    if ((atw.cycle[cycle].alarm_state & 0x10) != 0 )
        atw.cycle[cycle].io.sns_err.bit.Tsub_out = TRUE;
    else atw.cycle[cycle].io.sns_err.bit.Tsub_out = FALSE;

    return atw.cycle[cycle].alarm_state;
}

/************************************************************************
@name  	: CheckUnitAlarmState
@brief 	: 
@param 	: None
@return	: None
@note   : 9.故障处理,以下故障不停机
            1).翅片温度传感器故障
            2)经济器进口温度传感器故障
            3)经济器出口温度传感器故障
*************************************************************************/
uint8_t CheckCycleAlarmState(uint8_t cycle)
{
    uint8_t res = FALSE;

    if ((atw.cycle[cycle].alarm_state & 0xFFFFFFE6) != 0)//except bit0,bit3,bi4
    {
        res = TRUE;
    }
    return res;
}

/************************************************************************
@name  	: GetUnitAlarmState
@brief 	: 
@param 	: None
@return	: None
@note   : 9.故障处理
*************************************************************************/
uint32_t GetUnitAlarmState(void)
{
    atw.unit.local.alarm_state = UnitAlarmStateCheck();

    return atw.unit.local.alarm_state;
}

/************************************************************************
@name  	: CheckUnitAlarmState
@brief 	: 
@param 	: None
@return	: None
@note   : 9.故障处理
*************************************************************************/
uint8_t CheckUnitAlarmState(void)
{
    uint8_t res = FALSE;

    if (atw.unit.local.alarm_state != 0)
    {
        res = TRUE;
    }

    return res;
}

/************************************************************************
@name  	: GetkSysAlarmState
@brief 	: 
@param 	: None
@return	: None
@note   : 9.故障处理
*************************************************************************/
uint32_t GetkSysAlarmState(void)
{
    atw.sys.local.alarm_state = SysAlarmStateCheck();

    return atw.sys.local.alarm_state;
}

/************************************************************************
@name  	: CheckkSysAlarmState
@brief 	: 
@param 	: None
@return	: None
@note   : 9.系统故障
*************************************************************************/
uint8_t CheckkSysAlarmState(void)
{
    uint8_t res = FALSE;

    if (atw.sys.local.alarm_state != 0)
    {
        res = TRUE;
    }
    return res;
}

/************************************************************************
@name  	: AlarmCheck
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void AlarmCheck(uint8_t taskNo,uint8_t flag_init)
{
    uint8_t cycle;
    SFSetRunTaskNo( taskNo );
	
//cycle
    for (cycle = 0; cycle < atw.unit.local.cycle_active_cnt; cycle++)
    {
        AlarmCycleTimerCounter(cycle);
        GetCycleAlarmUpdate(cycle);
    }
//unit   
    AlarmUnitTimerCounter();
    GetUnitAlarmState();
//sys
    if (GetLocalAddress() == CHILLER_MAIN_ADDR)//chiller main
    {
        AlarmSysTimerCounter();
        GetkSysAlarmState();
    }
}
