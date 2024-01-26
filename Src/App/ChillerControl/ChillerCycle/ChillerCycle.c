/***********************************************************************
@file   : ChillerCycle.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "ChillerCycle.h"
#include "CompControl.h"
#include "FanControl.h"
#include "config.h"
#include "data.h"
#include "AlarmCheck.h"
#include "CycleDeviceControl.h"
#include "CycleStateControl.h"
#include "CompControl.h"
#include "TimerCounter.h"
#include "ExpvControl.h"


#define TM_START2_WAIT          ((uint16_t)60)
#define TM_DIFFER_WAIT          ((uint16_t)60)   //60S
#define TM_FIN_TEMP_HOLD        ((uint16_t)50)
#define TM_DEFROST_START_HOLD   ((uint16_t)30)

//time //decrease //increase
typedef struct
{
    uint16_t start1_comp_run;    //压缩机启动超5s
    uint16_t start2_over_wait;   //
    uint16_t start2_pd_hold;     //

    uint16_t Tcoil_hold;
    uint16_t Tevp_hold;
    uint16_t comp_freq_check;
    uint16_t defrost_meet;      //
    uint16_t differ_wait;       //
    uint16_t fin_temp_hold;     //除霜退出条件中,翅片温度持续50s>=除霜退出温度Tout
    uint16_t Tw_out_hold;       //
    uint16_t Td_defrost_hold;   //
    uint16_t defrost_start_hold;//除霜后启动持续时间
    uint16_t oil_freq_hold;

    // uint16_t cycle_total_oil_run;//cycle累计回油进入时间
    // uint16_t oil_current_run;    //回油运转时间
    // uint16_t total_oil_run;     //累计回油运转时间

}CycleControlTimeType;

static CycleControlTimeType time[CYCLE_CH_MAX];





/************************************************************************
@name  	: CheckOilTotalRunTimeAddState
@brief 	: 
@param 	: None
@return	: None
@note   : cycle累计回油进入时间累加判断
*************************************************************************/
static uint8_t CheckCycleOilTotalRunTimeAddState(uint8_t cycle)
{
    uint8_t res = FALSE;
    uint8_t work_step = atw.cycle[cycle].work_step;
    uint16_t Fi_run = atw.cycle[cycle].comp[COMP_1].link.state.freq_output; //
    uint32_t comp_run_time = atw.cycle[cycle].comp[COMP_1].local.current_run_time;//压缩机当前运行时间

    if (((work_step == CYCLE_STATUS_RUN_COOL) || (work_step == CYCLE_STATUS_RUN_HEAT))
        && (comp_run_time > 1)
        && (Fi_run < 350))
    {
        res = TRUE;
    }

    return res;
}

/************************************************************************
@name  	: CheckOilTotalRunTimeAddState
@brief 	: 
@param 	: None
@return	: None
@note   : cycle累计回油进入时间清零判断
*************************************************************************/
static uint8_t CheckCycleOilTotalRunTimeClearState(uint8_t cycle)
{
    uint8_t res = FALSE;
    uint8_t work_step = atw.cycle[cycle].work_step;
    uint16_t Fi_run = atw.cycle[cycle].comp[COMP_1].link.state.freq_output; //

    if ((Fi_run < 500) || (Fi_run > 700))
    {
        time[cycle].oil_freq_hold = 300;//5mins
    }

    if (((work_step == CYCLE_STATUS_DEFROST_RUN) || (work_step == CYCLE_STATUS_RUN_OIL_RETURN))
        && (time[cycle].oil_freq_hold == 0))
    {
        res = TRUE;
    }

    return res;
}

/************************************************************************
@name  	: CycleControlTimerCounter
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void CycleControlTimerCounter(uint8_t cycle)
{
    uint8_t work_step = atw.cycle[cycle].work_step;
    uint16_t *pTime;
    uint16_t i,len;
//decrease
    pTime = &time[cycle].start1_comp_run;
    if (TimerCheck(NO_CYCLE1_CTRL_BASE+cycle) == TRUE)
    {
        len = sizeof(CycleControlTimeType)/sizeof(uint16_t);
        TimerSet(NO_CYCLE1_CTRL_BASE+cycle,1); //base 1s
        for ( i = 0; i < len; i++)
        {
            if (*(pTime + i) != 0)
            {
                (*(pTime + i))--;
            }
        }
    //1s
        //除霜运转时间累加
        if ( work_step == CYCLE_STATUS_DEFROST_RUN)
        {
            atw.cycle[cycle].defrost_run_time++;//累加
        }

    }
//cycle计时用 increase//时间累计用 //1mins
    if (TimerCheck(NO_CYCLE1_INC_BASE+cycle) == TRUE)
    {
        TimerSet(NO_CYCLE1_INC_BASE+cycle,1); //base 1mins
        atw.cycle[cycle].power_on_time++;   //cycle累计上电时间
        if (CheckCycleOilTotalRunTimeAddState(cycle) == TRUE)
        {
            atw.cycle[cycle].comp[COMP_1].local.oil_total_run_time++;//累计回油进入时间
        }
        if (CheckCycleOilTotalRunTimeClearState(cycle) == TRUE)
        {
            atw.cycle[cycle].comp[COMP_1].local.oil_total_run_time = 0;
        }
        if (work_step == CYCLE_STATUS_RUN_OIL_RETURN)//回油运行时间
        {
            atw.cycle[cycle].comp[COMP_1].local.oil_run_time ++;
        }
        if ((work_step == CYCLE_STATUS_RUN_HEAT) || (work_step == CYCLE_STATUS_DEFROST_PREPARE))//制热累计运行时间//制热,除霜准备
        {
            atw.cycle[cycle].comp[COMP_1].local.total_heat_run_time++;
        }
    }
}

//====
/************************************************************************
@name  	: CheckCycleStatusOffToStart1
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2.1 cycle无故障,处于可运转状态
*************************************************************************/
static uint8_t CheckCycleStatusOffToStart1(uint8_t cycle)//cycle关机 -> cycle启动1
{
    uint8_t res = FALSE;

    if (CheckCycleAlarmState(cycle) == FALSE)//无cycle禁止运行故障
    {
        if ((GetCycleSetRunMode(cycle) != CYCLE_MODE_NULL)//收到开机指令
            && (GetCycleOperateState(cycle) == TRUE))//cycle可运转
        {
            res = TRUE;
        }
    }

    return res;
}

/************************************************************************
@name  	: CheckCycleStatusStart1ToOff
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2 cycle状态及控制程序
            条件5:cycle启动运转 -> cycle关机
                1.cycle启动1,出现单元禁止运行故障,或cycle关机指令
                2.cycle启动2,出现单元禁止运行故障
                3.除霜后启动,出现单元禁止运行故障,或cycle关机指令
*************************************************************************/
static uint8_t CheckCycleStatusStart1ToOff(uint8_t cycle)//cycle启动1->cycle关机
{
    uint8_t res = FALSE;

    if ((CheckCycleAlarmState(cycle) != FALSE)//有cycle禁止运行故障
        || (GetCycleSetRunMode(cycle) == CYCLE_MODE_NULL))//cycle关机指令
    {
        res = TRUE;
    }

    return res;
}

/************************************************************************
@name  	: CheckCycleStatusStart1ToStart2
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2 条件2 满足cycle启动1结束条件
                本cycle无故障,且制热/制冷,
                压缩机运转频率达到31Hz,且压缩机启动时间超过5s
*************************************************************************/
static uint8_t CheckCycleStatusStart1ToStart2(uint8_t cycle)//cycle启动1->cycle启动2
{
    uint8_t res = FALSE;
    uint16_t Fi_run = atw.cycle[cycle].comp[COMP_1].link.state.freq_output; //
    uint16_t Fi_ini = atw.cycle[cycle].comp[COMP_1].local.Fi_c_ini;

    if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)
    {
        Fi_ini = atw.cycle[cycle].comp[COMP_1].local.Fi_h_ini;
    }
//
    if (CheckCycleAlarmState(cycle) == FALSE)//无cycle故障
    {
        if (GetCycleSetRunMode(cycle) != CYCLE_MODE_NULL)//有运转指令
        {
            if (Fi_run == 0)
            {
                time[cycle].start1_comp_run = 5;    //运行时间超5s
            }
            else if ((Fi_run >= Fi_ini) && (time[cycle].start1_comp_run == 0))//达到目标频率,时间达到
            {
                res = TRUE;
            }
        }
    }

    return res;
}

/************************************************************************
@name  	: CheckCycleStatusStart2ToOff
@brief 	: 
@param 	: None
@return	: None
@note   : 
*************************************************************************/
static uint8_t CheckCycleStatusStart2ToOff(uint8_t cycle)//cycle启动2->cycle关机
{
    uint8_t res = FALSE;

    if (CheckCycleAlarmState(cycle) != FALSE)//有cycle禁止运行故障
    {
        res = TRUE;
    }
    else if (GetCycleSetRunMode(cycle) == CYCLE_MODE_NULL)//cycle关机指令
    {
        res = TRUE;
    }

    return res;
}

/************************************************************************
@name  	: CheckCycleStatusStart2ToRunHeatCool
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2 条件3 满足cycle启动2结束条件,包括时间,温度,压力
*************************************************************************/
static uint8_t CheckCycleStatusStart2ToRunHeatCool(uint8_t cycle)//cycle启动2->制热/制冷运转
{
    uint8_t res = FALSE;
    uint16_t Tdmax = atw.cycle[cycle].comp[COMP_1].local.Td;
    uint16_t Pd = atw.cycle[cycle].comp[COMP_1].local.Pd;
    uint16_t Ps = atw.cycle[cycle].comp[COMP_1].local.Ps;

    if (GetCycleSetRunMode(cycle) == CYCLE_MODE_COOL)
    {
        if (Ps >= 600)//ps>=600时,重新计时
        {
            time[cycle].start2_pd_hold = 5;
        }
    }
    else
    {
        if (Ps >= 150)
        {
            time[cycle].start2_pd_hold = 5;
        }
    }
//
    if (CheckCycleAlarmState(cycle) == FALSE)//无cycle故障
    {
        if (GetCycleSetRunMode(cycle) != CYCLE_MODE_NULL)//有运转指令
        {
            if (GetCycleSetRunMode(cycle) == CYCLE_MODE_COOL)
            {
                if ((time[cycle].start2_over_wait == 0) //进入时间超1mins(初始值为60)
                    || ((Tdmax > 900) && (time[cycle].start2_over_wait < 55))//进入改状态5s,且Tdmax>90
                    || ((Pd > 3500) && (time[cycle].start2_over_wait < 55))//进入时间超5s,且Pd>3500kPa
                    || ((Ps < 600) && (time[cycle].start2_over_wait < 55) && (time[cycle].start2_pd_hold == 0)))//进入时间超5s,且Ps<0.6MPa,且持续5s
                {
                    res = TRUE;
                }
            }
            else if (GetCycleSetRunMode(cycle) == CYCLE_MODE_HEAT)
            {
                if ((time[cycle].start2_over_wait < 30) //进入时间超1mins(初始值为90,减到30为1mins)
                    || ((Tdmax > 900) && (time[cycle].start2_over_wait < 85))//进入改状态5s,且Tdmax>90
                    || ((Pd > 3000) && (time[cycle].start2_over_wait < 85))//进入时间超5s,且Pd>3500kPa
                    || ((Ps < 150) && (time[cycle].start2_over_wait < 85) && (time[cycle].start2_pd_hold == 0)))//进入时间超5s,且且Ps<0.15MPa,且持续5s
                {
                    res = TRUE;
                }
            }
        }
    }

    return res;
}

/************************************************************************
@name  	: CheckCycleStatusRunToOff
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2 条件4 接收到cycle关机指令或出现模块单元禁止运行故障, //执行pump dowm,减少制冷剂迁移风险
*************************************************************************/
static uint8_t CheckCycleStatusRunToOff(uint8_t cycle)//制热/制冷运转->cycle关机
{
    uint8_t res = FALSE;

    if (CheckCycleAlarmState(cycle) != FALSE)//有cycle禁止运行故障
    {
        res = TRUE;
    }
    else if (GetCycleSetRunMode(cycle) == CYCLE_MODE_NULL)//cycle关机指令
    {
        res = TRUE;
    }
    return res;
}

/************************************************************************
@name  	: CheckCycleStatusRunHeatCoolToOilReturn
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2 条件9 制冷或制热满足回油进入条件
*************************************************************************/
static uint8_t CheckCycleStatusRunHeatCoolToOilReturn(uint8_t cycle)//制热/制冷运转->回油运转
{
    uint8_t res = FALSE;
    uint16_t comp_run_time = atw.cycle[cycle].comp[COMP_1].local.current_run_time;//压缩机当前运行时间
    uint16_t Fi_run = atw.cycle[cycle].comp[COMP_1].link.state.freq_output;//压缩机实际运转频率
    uint16_t oil_time = atw.cycle[cycle].comp[COMP_1].local.oil_total_run_time;//累计回油进入时间

    if (GetCycleRealRunMode(cycle) == CYCLE_MODE_COOL)//制冷
    {
        if ((comp_run_time >= 8) && (Fi_run < 350) && (oil_time >= 180))//压缩机持续运转时间超8分钟,实际频率<35Hz,累计回油进入时间>=3h
        {
            res = TRUE;
        }
        else if ((comp_run_time >= 8) && (Fi_run > 950) && (oil_time >= 60))//或压缩机持续运转时间超8分钟,实际频率>95Hz,累计回油进入时间>=1h
        {
            res = TRUE;
        }
    }
    else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)//制热
    {
        if ((comp_run_time >= 8) && (Fi_run < 350) && (oil_time >= 120))//压缩机持续运转时间超8分钟,实际频率<35Hz,累计回油进入时间>=2h
        {
            res = TRUE;
        }
        else if ((comp_run_time >= 8) && (Fi_run > 950) && (oil_time >= 60))//压缩机持续运转时间超8分钟,实际频率>95Hz,累计回油进入时间>=1h
        {
            res = TRUE;
        }
    }

    return res;
}


/************************************************************************
@name  	: GetTargetTempValue
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2.4 cycle除霜控制
            1)除霜进入条件 cycle制热运转,,且压缩机连续运行时间>=10mins //
*************************************************************************/
static uint8_t GetTargetTempValue(uint8_t cycle)
{
    int16_t Tam = atw.unit.table.io.Tam;
    int16_t defrost_temp_tam_less_zero = atw.unit.table.sys_cmd.defrost_temp_Tam_less_zero; //环温小于0度除霜温度
    int16_t defrost_temp_tam_more_zero = atw.unit.table.sys_cmd.defrost_temp_Tam_more_zero; //环温不小于0度除霜温度
    int16_t target_temp;

    if (atw.unit.local.sns_err.bit.Tam == TRUE)//Tam故障时,Tam = 0;
    {
        Tam = 0;
    }
    if (Tam >= 0)
    {
        if (Tam + defrost_temp_tam_more_zero > -40)
        {
            target_temp = Tam + defrost_temp_tam_more_zero;
        }
        else target_temp = -40;
    }
    else
    {
        if (Tam + defrost_temp_tam_less_zero > -40)
        {
            target_temp = Tam + defrost_temp_tam_less_zero;
        }
        else target_temp = -40;
    }

    return target_temp;
}

/************************************************************************
@name  	: CheckCycleRunDefrostRequire
@brief 	: 检测除霜进入条件
@param 	: None
@return	: None
@note   : 5.2.4 cycle除霜控制
            1)除霜进入条件 cycle制热运转,,且压缩机连续运行时间>=10mins //
*************************************************************************/
static uint8_t CheckCycleRunDefrostRequire(uint8_t cycle)//制热运转->化霜准备
{
    uint8_t res = FALSE;
    uint16_t comp_run_time = atw.cycle[cycle].comp[COMP_1].local.current_run_time;//压缩机当前运行时间
    uint16_t oil_interval_time = atw.cycle[cycle].comp[COMP_1].local.oil_heat_interval_time;//回油制热运转间隔时间
    uint32_t heat_run_time = atw.cycle[cycle].comp[COMP_1].local.total_heat_run_time;//制热累计运行时间
    uint16_t Fi_run = atw.cycle[cycle].comp[COMP_1].link.state.freq_output;
    uint16_t Fi_rating = atw.cycle[cycle].comp[COMP_1].local.Fi_h_rating;
    int16_t  SH_TS_act = atw.cycle[cycle].comp[COMP_1].local.SH_TS_act;
    uint16_t defrost_interval_time = atw.unit.table.sys_cmd.heat_time_defrost_interval;//除霜间隔累计制热时间 //mins
    uint8_t  defrost_env = atw.unit.table.sys_cmd.data2.bit.defrost_env;
    int16_t  Tcoil = atw.cycle[cycle].io.Tcoil;
    int16_t  Tam = atw.unit.table.io.Tam;
    uint8_t  unit_state = atw.unit.table.init.dsw1.bit.unit_work_state;
    int16_t TT;
    int16_t target_temp = GetTargetTempValue(cycle);

    if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)
    {
//条件1
         TT = 4-4*Fi_run/Fi_rating;
    //Tcoil    
        if (Tcoil > (target_temp + TT))//
        {
            time[cycle].Tcoil_hold == 180;  //clear timer cnt
        }
    //Tevp
        if (Tcoil > (target_temp - 60 + TT))//
        {
            time[cycle].Tevp_hold == 180;  //clear timer cnt
        }
    //
        if ((comp_run_time >= 10) && (oil_interval_time > 5))//1.除霜进入条件:制热运转,且压缩机连续运行时间>=10mins,且制热回油运转间隔超5mins
        {
            if (defrost_env == DEFROST_ENV_STANDARD)//标准除霜
            {
                if (heat_run_time > defrost_interval_time)//运行累计时间>=[除霜间隔累计制热时间]
                {
                    if (((time[cycle].Tcoil_hold == 0) && (SH_TS_act <= 80))//制热累计运行时间 且持续3mins Tcoil<= 目标值+TT
                        || ((time[cycle].Tevp_hold == 0) && (SH_TS_act <= 80)))//或持续3minsTevp<=目标温度-6+TT,且SH_TS_act<=8
                    {
                        res = TRUE;
                    }
                }
            }
            else if (defrost_env == DEFROST_ENV_WET)//高湿除霜
            {
                if (heat_run_time > (defrost_interval_time - 10))//运行累计时间>=[除霜间隔累计制热时间]
                {
                    if (((time[cycle].Tcoil_hold == 0) && (SH_TS_act <= 80))//制热累计运行时间 且持续3mins Tcoil<= 目标值+TT
                        || ((time[cycle].Tevp_hold == 0) && (SH_TS_act <= 80)))//或持续3minsTevp<=目标温度-6+TT,且SH_TS_act<=8
                    {
                        res = TRUE;
                    }
                }
            }
        }
//条件2
        if (Tam < -100)
        {
            if ((unit_state == UNIT_STATE_NORMAL) && (atw.cycle[cycle].comp[COMP_1].local.total_run_time > 180))//非测试模式,压缩机累计运行时间超180mins
            {
                res = TRUE;
            }
        }
//条件3
        if (atw.unit.table.sys_cmd.data2.bit.defrost_manual == TRUE)//手动除霜请求
        {
            res = TRUE;
        }
    }

    return res;
}

/************************************************************************
@name  	: CheckCycleStatusRunHeatToDefrostPrepare
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2.4 cycle除霜控制
            1)进入条件 cycle制热运转,,且压缩机连续运行时间>=10mins //
*************************************************************************/
static uint8_t CheckCycleStatusRunHeatToDefrostPrepare(uint8_t cycle)//制热运转->化霜准备
{
    uint8_t res;
    uint8_t defrost_allow = atw.unit.table.sys_cmd.data2.bit.defrost_allow; //除霜许可

    if (CheckCycleRunDefrostRequire(cycle) == TRUE)//满足除霜申请条件
    {
        //是否需要进入除霜运转准备
        if (defrost_allow == TRUE) //有除霜需求,且允许除霜
        {
            res = TRUE;
        }
    }
    return res;
}

/************************************************************************
@name  	: CheckCycleStatusOilReturnToOff
@brief 	: 
@param 	: None
@return	: None
@note   : 5.3 cycle回油
            3)回油退出条件
                cycle故障,或收到cycle关机指令;cycle累计回油进入时间清零,回油运转时间清零
*************************************************************************/
static uint8_t CheckCycleStatusOilReturnToOff(uint8_t cycle)//回油运转->cycle关机
{
    uint8_t res = FALSE;

    if ((CheckCycleAlarmState(cycle) != FALSE)//cycle故障
        || (GetCycleSetRunMode(cycle) == CYCLE_MODE_NULL))//cycle关机指令
    {
        res = TRUE;
        // atw.cycle[cycle].comp[COMP_1].local.oil_total_run_time = 0;//累计回油进入时间
        atw.cycle[cycle].comp[COMP_1].local.oil_run_time = 0;//回油运转时间
    }
    return res;
}

/************************************************************************
@name  	: CheckCycleStatusOilReturnToHeatCool
@brief 	: 
@param 	: None
@return	: None
@note   : 5.3 cycle回油
            3)回油退出条件
                满足以下任一条件退出回油,进入制冷/制热
                1.
*************************************************************************/
static uint8_t CheckCycleStatusOilReturnToHeatCool(uint8_t cycle)//回油运转->制热/制冷运转
{
    uint8_t res = FALSE;
    uint16_t Fi_run = atw.cycle[cycle].comp[COMP_1].link.state.freq_output;
    uint16_t oil_run_time = atw.cycle[cycle].comp[COMP_1].local.oil_run_time;
    
    if ((Fi_run < 500) || (Fi_run > 700))
    {
        time[cycle].comp_freq_check = 300;  //5mins
    }
 //
    if ((time[cycle].comp_freq_check == 0)
        || (oil_run_time > 10))
    {
        res = TRUE;
        atw.cycle[cycle].comp[COMP_1].local.oil_total_run_time = 0;//累计回油进入时间
        atw.cycle[cycle].comp[COMP_1].local.oil_run_time = 0;//回油运转时间
    }
    return res;
}

/************************************************************************
@name  	: CheckCycleStatusDifferToOff
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static uint8_t CheckCycleStatusDifferToOff(uint8_t cycle)//差压控制->cycle关机
{
    uint8_t res = FALSE;

    if (CheckCycleAlarmState(cycle) != FALSE)//有cycle禁止运行故障
    {
        res = TRUE;
    }

    return res;
}

/************************************************************************
@name  	: CheckCycleStatusDefrosPreparetToOff
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2 条件6 出现模块单元禁止运行故障
*************************************************************************/
static uint8_t CheckCycleStatusDefrosPreparetToOff(uint8_t cycle)//除霜准备->cycle关机
{
    uint8_t res = FALSE;

    if ((CheckCycleAlarmState(cycle) != FALSE)//cycle故障
        || (GetCycleSetRunMode(cycle) == CYCLE_MODE_NULL))//cycle关机指令
    {
        res = TRUE;
    }
    return res;

}

/************************************************************************
@name  	: CheckCycleStatusDefrostPrepareToHeat
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2.4 cycle除霜控制
            满足条件,退出cycle除霜准备,进入cycle制热
                不满足cycle除霜条件,且持续5s
*************************************************************************/
static uint8_t CheckCycleStatusDefrostPrepareToHeat(uint8_t cycle)//除霜准备->制热运转
{
    uint8_t res = FALSE;

    if (CheckCycleRunDefrostRequire(cycle) == TRUE)
    {
        time[cycle].defrost_meet = 5;
    }
    else if (time[cycle].defrost_meet == 0)//不满足除霜条件,且持续5s
    {
        res = TRUE;
    }

    return res;
}

/************************************************************************
@name  	: CheckCycleStatusDefrostPrepareToDifferPressure
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2.4 cycle除霜控制
            1.接收到系统允许除霜指令
            2.或接收到强制除霜指令,且cycle制热,且压缩机运行时间超5mins
*************************************************************************/
static uint8_t CheckCycleStatusDefrostPrepareToDifferPressure(uint8_t cycle)//除霜准备->差压控制
{
    uint8_t res = FALSE;
    uint8_t defrost_allow = atw.unit.table.sys_cmd.data2.bit.defrost_allow; //除霜许可
    uint8_t defrost_manual = atw.unit.table.sys_cmd.data2.bit.defrost_manual; //手动除霜
    uint16_t comp_run_time = atw.cycle[cycle].comp[COMP_1].local.current_run_time;//压缩机当前运行时间

    if ((defrost_allow == TRUE)
        || ((defrost_manual == TRUE) && (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT) && (comp_run_time > 5)))
    {
        res = TRUE;
    }
    return res;
}

/************************************************************************
@name  	: CheckCycleStatusDifferPressureToDefrost
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2.4 cycle除霜控制
            进入差压控制5s后,检测到压差(Pd-Ps)在0.5MPa~1.0MPa之间
            或进入差压控制状态超60s
*************************************************************************/
static uint8_t CheckCycleStatusDifferPressureToDefrost(uint8_t cycle)//差压控制->化霜运转
{
    uint8_t res = FALSE;
    uint16_t Pd = atw.cycle[cycle].comp[COMP_1].local.Pd;
    uint16_t Ps = atw.cycle[cycle].comp[COMP_1].local.Ps;

    if (time[cycle].differ_wait == 0)//本状态超60s
    {
        res = TRUE;
    }
    else if (time[cycle].differ_wait < 55)//60s计时器
    {
        if (((Pd - Ps) > 500) && ((Pd - Ps) < 1000))
        {
            res = TRUE;
        }
    }
    return res;
}



/************************************************************************
@name  	: GetDefrostTout
@brief 	: 获取除霜退出温度Tout
@param 	: None
@return	: None
@note   : 4-3)除霜退出条件1
            1.风机未运转,Tout = [除霜结束翅片温度]
*************************************************************************/
static int16_t GetDefrostTout(uint8_t cycle)
{
    int16_t  fin_temp = atw.unit.table.sys_cmd.fin_temp_defrost_finish; //除霜结束翅片温度
    uint16_t fan_speed = atw.cycle[cycle].fan[FAN_1].link.state.foc_freq_output;
    int16_t Tout;

    if (fan_speed == 0)//1.风机未运转
    {
        Tout = fin_temp;
    }
    else//风机运转
    {
        if (fin_temp > 100)//1.
        {
            if ((fin_temp +230) > 460)
            {
                Tout = 460;
            }
            else Tout = fin_temp + 230;
        }
        else
        {
            Tout = fin_temp + 30;
        }
    }
    return Tout;
}

/************************************************************************
@name  	: CheckCycleStatusDefrostToStartDefrostDone
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2 条件15 满足除霜退出条件
            4-3)除霜退出条件1
                当持续50s翅片温度Tcoil>=除霜退出温度Tout
*************************************************************************/
static uint8_t CheckCycleDefrostQuitState1(uint8_t cycle)
{
    uint8_t res = FALSE;
    int16_t Tcoil = atw.cycle[cycle].io.Tcoil;
    int16_t Tc = atw.cycle[cycle].comp[COMP_1].local.Tc;
    int16_t Tout = GetDefrostTout(cycle);

    if (atw.cycle[cycle].io.sns_err.bit.Tcoil == TRUE)
    {
        Tcoil = Tc - 30;
    }
    if (Tcoil < Tout)
    {
        time[cycle].fin_temp_hold = TM_FIN_TEMP_HOLD;
    }
    if (time[cycle].fin_temp_hold == 0)
    {
        res = TRUE;
    }

    return res;
}

/************************************************************************
@name  	: CheckCycleDefrostQuitState2
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2 条件15 满足除霜退出条件
            4-3)除霜退出条件2
                1.除霜运转时间>=Time_DF
                2.出水温度持续5s<=10°
                3.该cycle出现高压开关动作(不报故障,接在驱动上)
                4.排气温度持续10s大于110°(不报故障)
*************************************************************************/
static uint8_t CheckCycleDefrostQuitState2(uint8_t cycle)
{
    uint8_t res = FALSE;
    uint8_t  defrost_type = atw.unit.table.sys_cmd.data2.bit.defrost_manual;
    uint16_t Time_DF = 360;
    int16_t Tw_out = atw.unit.table.io.Tw_out;
    int16_t Td = atw.cycle[cycle].comp[COMP_1].local.Td;

//1.除霜运转时间
    //get Time_DF
    if (defrost_type == DEFROST_TYPE_AUTO)//自动除霜
    {
        Time_DF = atw.unit.table.sys_cmd.defrost_max_time;  //除霜最长时间
    }
    else Time_DF = atw.unit.table.sys_cmd.manual_defrost_max_time;  //手动除霜最大时间
    //时间判断   
    if (atw.cycle[cycle].defrost_run_time >= Time_DF)
    {
        res = TRUE;
    }

//2.出水温度持续5s<=10
    if (Tw_out > 100)
    {
        time[cycle].Tw_out_hold = 5;
    }
    if (time[cycle].Tw_out_hold == 0)
    {
        res = TRUE;
    }
//3.该cycle出现高压开关动作  
    if (atw.cycle[cycle].comp[COMP_1].link.state.alarm_state2.bit._63H_circuit_err == TRUE)//TODO 高压开关动作确定,故障停机
    {
        res = TRUE;
    }
//4.排气温度持续10s大于110
    if (Td <= 1100)
    {
        time[cycle].Td_defrost_hold = 10;
    }
    if (time[cycle].Td_defrost_hold == 0)
    {
        res = TRUE;
    }

    return res;
}

/************************************************************************
@name  	: CheckCycleStatusDefrostToOff
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2 条件6 出现模块单元禁止运行故障
*************************************************************************/
static uint8_t CheckCycleStatusDefrostToOff(uint8_t cycle)//除霜->cycle关机
{
    uint8_t res = FALSE;

    if (CheckCycleAlarmState(cycle) != FALSE)//有cycle禁止运行故障
    {
        res = TRUE;
    }
    else if (GetCycleSetRunMode(cycle) == CYCLE_MODE_NULL)//关机指令
    {
        if ((CheckCycleDefrostQuitState1(cycle) == TRUE))//除霜退出条件1
        {
            res = TRUE;
            atw.cycle[cycle].total_heat_run_time = 0;//制热累计运行时间
        }
        else if ((CheckCycleDefrostQuitState2(cycle) == TRUE))//除霜退出条件2
        {
            res = TRUE;
        }
    }
    
    return res;
}

/************************************************************************
@name  	: CheckCycleStatusDefrostToStartDefrostDone
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2 条件15 满足除霜退出条件
            满足除霜退出条件,条件1:
*************************************************************************/
static uint8_t CheckCycleStatusDefrostToStartDefrostDone(uint8_t cycle)//除霜运转->除霜后启动
{
    uint8_t res = FALSE;

    if ((CheckCycleDefrostQuitState1(cycle) == TRUE))//除霜退出条件1
    {
        atw.cycle[cycle].total_heat_run_time = 0;//制热累计时间清零
        res = TRUE;
    }
    else if ((CheckCycleDefrostQuitState2(cycle) == TRUE))//除霜退出条件2,不清零
    {
        res = TRUE;
    }

    return res;
}

/************************************************************************
@name  	: CheckCycleStatusDefrostDoneToOff
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2 条件15 满足除霜退出条件
*************************************************************************/
static uint8_t CheckCycleStatusDefrostDoneToOff(uint8_t cycle)//除霜后启动->off
{
    uint8_t res = FALSE;

    if (CheckCycleAlarmState(cycle) != FALSE)//有cycle禁止运行故障
    {
        res = TRUE;
    }
    else if (GetCycleSetRunMode(cycle) == CYCLE_MODE_NULL)//cycle关机指令
    {
        res = TRUE;
    }
    
    return res;
}

/************************************************************************
@name  	: CheckCycleStatusStartDefrostDoneToStart2
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2 条件16 满足除霜后启动退出条件
*************************************************************************/
static uint8_t CheckCycleStatusStartDefrostDoneToStart2(uint8_t cycle)//除霜后启动->cycle启动2
{
    uint8_t res = FALSE;
    int16_t Pd = atw.cycle[cycle].comp[COMP_1].local.Pd;
    int16_t Ps = atw.cycle[cycle].comp[COMP_1].local.Ps;
    uint16_t Fi_run = atw.cycle[cycle].comp[COMP_1].link.state.freq_output;

    if ((time[cycle].defrost_start_hold == 0) && (Fi_run < 320))
    {
        res = TRUE;
    }
    else if ((time[cycle].defrost_start_hold < 25))//进入5s后(30s计时器)
    {
        if (((Pd - Ps) >= 900) && ((Pd - Ps) <= 1000))
        {
            res = TRUE;
        }
    }

    return res;
}
/************************************************************************
@name  	: ChillerCycleFourayControl
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerCycleFourayControl(uint8_t cycle)
{
    if (GetCycleRealRunMode(cycle) == CYCLE_MODE_COOL)
    {
        CycleFourWayValveControl(cycle,OFF);
    }
    else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)
    {
        CycleFourWayValveControl(cycle,ON);
    }
    
}

/************************************************************************
@name  	: ChillerCycleControl
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerCycleStatusControl(uint8_t cycle)
{
    uint8_t last_step = atw.cycle[cycle].last_step;
    uint8_t work_step = atw.cycle[cycle].work_step;

    switch (work_step)
    {
        case CYCLE_STATUS_INIT://cycle初上电 //TODO 添加膨胀阀的归零设置
        {
            time[cycle].oil_freq_hold = 300;
            work_step = CYCLE_STATUS_OFF;
        }
            break;
        case CYCLE_STATUS_OFF://cycle关机状态
        {
            if (last_step != CYCLE_STATUS_OFF)
            {
                if ((atw.cycle[cycle].comp[COMP_1].local.oil_total_run_time - 5) < 0)
                {
                    atw.cycle[cycle].comp[COMP_1].local.oil_total_run_time = 0;
                }
            }
            else if (CheckCycleStatusOffToStart1(cycle) == TRUE)//off -> start1
            {
                work_step = CYCLE_STATUS_START1;
            }
        }
            break;
        case CYCLE_STATUS_START1://cycle启动1
        {
            if (last_step != CYCLE_STATUS_START1)//初次进入
            {
                time[cycle].start1_comp_run = 5; //清计时器 //压缩机运行时间5s
            }
            
            if (CheckCycleStatusStart1ToOff(cycle) == TRUE)//start1 -> off
            {
                work_step = CYCLE_STATUS_OFF;
            }
            else if (CheckCycleStatusStart1ToStart2(cycle) == TRUE)//start1 -> start2
            {
                work_step = CYCLE_STATUS_START2;
            }
        }    
            break;    
        case CYCLE_STATUS_START2://cycle启动2
        {
            ChillerCycleFourayControl(cycle);
            if (last_step != CYCLE_STATUS_START2)//初次进入
            {
                time[cycle].start2_over_wait = TM_START2_WAIT;
                time[cycle].start2_pd_hold = 5;//pd<600计时持续时间,进入时初值
            }
            if (CheckCycleStatusStart2ToOff(cycle) == TRUE)//start2 -> off
            {
                work_step = CYCLE_STATUS_OFF;
            }
            else if (CheckCycleStatusStart2ToRunHeatCool(cycle) == TRUE)//start2 -> run(heat/cool)
            {
                if (GetCycleSetRunMode(cycle) == CYCLE_MODE_COOL)//cycle run mode
                {
                    work_step = CYCLE_STATUS_RUN_COOL;
                }
                else if (GetCycleSetRunMode(cycle) == CYCLE_MODE_HEAT)
                {
                    work_step = CYCLE_STATUS_RUN_HEAT;
                }
            }
        }    
            break;
        case CYCLE_STATUS_RUN_COOL://cycle通常运转/制冷
        case CYCLE_STATUS_RUN_HEAT://cycle通常运转/制热
        {
            ChillerCycleFourayControl(cycle);
            if (CheckCycleStatusRunToOff(cycle) == TRUE)//heat/cool -> off
            {
                work_step = CYCLE_STATUS_OFF;
            }
            else if (CheckCycleStatusRunHeatCoolToOilReturn(cycle) == TRUE)//heat/cool -> oil return//通常运转->回油
            {
                work_step = CYCLE_STATUS_RUN_OIL_RETURN;
            }
            else if (CheckCycleStatusRunHeatToDefrostPrepare(cycle) == TRUE)//heat -> defrost prepare//制热->除霜准备
            {
                work_step = CYCLE_STATUS_DEFROST_PREPARE;
            }
        }    
            break;
        case CYCLE_STATUS_RUN_OIL_RETURN://回油运转
        {
            if (last_step != CYCLE_STATUS_RUN_OIL_RETURN)//初次进入
            {
                time[cycle].comp_freq_check = 300;  //set init value
            }
            else if (CheckCycleStatusOilReturnToOff(cycle) == TRUE)//oil return -> off
            {
                work_step = CYCLE_STATUS_OFF;
            }
            else if (CheckCycleStatusOilReturnToHeatCool(cycle) == TRUE)//oil return -> heat/cool
            {
                if (GetCycleSetRunMode(cycle) == CYCLE_MODE_COOL)//cycle run mode
                {
                    work_step = CYCLE_STATUS_RUN_COOL;
                }
                else if (GetCycleSetRunMode(cycle) == CYCLE_MODE_HEAT)
                {
                    work_step = CYCLE_STATUS_RUN_HEAT;
                }
            }
        }    
            break;
        case CYCLE_STATUS_DEFROST_PREPARE://化霜准备
        {
            CycleFourWayValveControl(cycle,ON);
            if (last_step != CYCLE_STATUS_DEFROST_PREPARE)//初次进入
            {
                time[cycle].defrost_meet = 5;  //set init value
            }
            else if (CheckCycleStatusDefrosPreparetToOff(cycle) == TRUE)//defrost prepare -> off
            {
                work_step = CYCLE_STATUS_OFF;
            }
            else if (CheckCycleStatusDefrostPrepareToHeat(cycle) == TRUE)//defrost prepare -> heat
            {
                work_step = CYCLE_STATUS_RUN_HEAT;
            }
            else if (CheckCycleStatusDefrostPrepareToDifferPressure(cycle) == TRUE)//defrost prepare -> differ pressure
            {
                work_step = CYCLE_STATUS_DIFFER_PRESSURE;
            }
        }    
            break;
        case CYCLE_STATUS_DIFFER_PRESSURE://差压控制
        {
            CycleFourWayValveControl(cycle,OFF);
            if (last_step != CYCLE_STATUS_DIFFER_PRESSURE)//初次进入
            {
                time[cycle].differ_wait = TM_DIFFER_WAIT;
            }
            else if (CheckCycleStatusDifferToOff(cycle) == TRUE)//differ pressure -> off
            {
                work_step = CYCLE_STATUS_OFF;
            }
            else if (CheckCycleStatusDifferPressureToDefrost(cycle) == TRUE)//differ pressure -> defrost
            {
                work_step = CYCLE_STATUS_DEFROST_RUN;
            }
        }    
            break;
        case CYCLE_STATUS_DEFROST_RUN://除霜运转
        {
            CycleFourWayValveControl(cycle,OFF);
            if (last_step != CYCLE_STATUS_DIFFER_PRESSURE)//初次进入
            {
                time[cycle].fin_temp_hold = TM_FIN_TEMP_HOLD;//初值
                time[cycle].Tw_out_hold = 5;
                time[cycle].Td_defrost_hold = 10;
                atw.cycle[cycle].defrost_run_time = 0;//清零
            }
            else if (CheckCycleStatusDefrostToOff(cycle) == TRUE)//defrost -> off
            {
                work_step = CYCLE_STATUS_OFF;
            }
            else if (CheckCycleStatusDefrostToStartDefrostDone(cycle) == TRUE)//defrost ->  start_defrost_done
            {
                work_step = CYCLE_STATUS_START_DEFROST_DONE;
            }
        }    
            break;
        case CYCLE_STATUS_START_DEFROST_DONE://除霜后启动
        {
            CycleFourWayValveControl(cycle,OFF);
            if (last_step != CYCLE_STATUS_START_DEFROST_DONE)//初次进入
            {
                time[cycle].defrost_start_hold = TM_DEFROST_START_HOLD;
            }
            else if (CheckCycleStatusDefrostDoneToOff(cycle) == TRUE)//start_defrost_done -> off
            {
                work_step = CYCLE_STATUS_OFF;
            }
            else if (CheckCycleStatusStartDefrostDoneToStart2(cycle) == TRUE)//start_defrost_done ->  start2
            {
                work_step = CYCLE_STATUS_START2;
            }
        }    
            break;
        default: work_step = CYCLE_STATUS_OFF;
            break;
    }
    atw.cycle[cycle].last_step = atw.cycle[cycle].work_step; //last step
    atw.cycle[cycle].work_step = work_step;//current step
    
}

/************************************************************************
@name  	: ChillerCycleInit
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ChillerCycleInit(uint8_t cycle)
{
    TimerSet(NO_CYCLE1_INC_BASE+cycle,1); //set init
    ExpvInit(cycle);
}

/************************************************************************
@name  	: ChillerCycleControl
@brief 	: 
@param 	: None
@return	: None
@note   : cycle:包含风机,压缩机,四通阀,电子膨胀阀,电磁阀
          5.cycle控制逻辑
@note   : 5.2 cycle状态及控制程序(最多4个)
*************************************************************************/
void ChillerCycleControl(uint8_t cycle)
{
//控制    
    ChillerCycleStatusControl(cycle);           //cycle status judge
    CycleControlTimerCounter(cycle);            //time use
    CycleOilReturnValveControl(cycle);          //回油电磁阀的控制
    CycleCompEviValveControl(cycle);            //压缩机喷焓电磁阀
    CycleCompCrankcaseHeatBeltControl(cycle);   //压缩机曲轴箱电加热带
    FanRunStatusControl(cycle);                 //fan control
    CompRunStatusControl(cycle);                //comp control
    ExpvControl(cycle);                         //exv control
//状态
    CheckCycleOperateState(cycle);              //检测cycle可运转状态
    CheckCycleCloseableState(cycle);            //可关闭状态
}
