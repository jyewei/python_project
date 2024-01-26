/***********************************************************************
@file   : ChillerSys.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "ChillerSys.h"
#include "Data.h"
#include "TimerCounter.h"
#include "SysDeviceControl.h"
#include "SysStateControl.h"
#include "AlarmCheck.h"
#include "IOProcess.h"
#include "AntiFreezeControl.h"


#define COMM_CHECK_WAIT                 ((uint16_t)30)   //3.4.3 系统关机 延时30s,待单元自检后,进行系统通讯检测
#define INIT_PUMP_CHECK_WAIT            ((uint16_t)90)   //3.4.3 
#define INIT_WATER_HEATER_CHECK_WAIT    ((uint16_t)90)
#define TM_STANDBY_TO_RUN               ((uint16_t)20)   //运转时,水温持续满足制冷/制热开机条件时,方可进入开机



//time
typedef struct
{
    uint16_t prepare_wait_time;      //运转准备等待时间
    uint16_t init_wait;              //上电自检等待时间         
    uint16_t pump_run_wait;          //
    uint16_t water_heater_run_wait;  //辅助电加热上电后开启
    uint16_t run_standby_to_wait;    //运转进入待机需等待的时间
    uint16_t pump_Tw_high_check;     //待机时检测水泵循环导致水温过高检测用

}SysControlTimeType;

static SysControlTimeType time;
static uint8_t flag_pump_run_state; //用于检测是否 有水泵运转中


/************************************************************************
@name  	: SysControlTimerCounter
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void SysStandbyTimerCounter(void)
{
    uint8_t sys_step = atw.sys.local.work_step;
    uint8_t last_step = atw.sys.local.last_step;

    if (sys_step == SYS_STATUS_STANDBY)//系统待机
    {
        if (last_step != SYS_STATUS_STANDBY)//进入待机时
        {
            atw.sys.local.standby_run_time = 0; //待机时间清零
            atw.sys.local.pump_run_time = 0; //水泵运行时间清零
        }
        else
        {
            if (TimerCheck(NO_SYS_ADD_BASE) == TRUE)
            {
                TimerSet(NO_SYS_ADD_BASE,1); //1mins
                atw.sys.local.standby_run_time++;//待机时间
        //水泵运行时间
                if (flag_pump_run_state == TRUE)
                {
                    atw.sys.local.pump_run_time++;
                    atw.sys.local.pump_stop_time = 0;
                }
                else
                {
                    atw.sys.local.pump_stop_time++;
                    atw.sys.local.pump_run_time = 0;
                }
            }
        }
    }
}

/************************************************************************
@name  	: SysControlTimerCounter
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void SysControlTimerCounter(void)
{
    uint16_t *pTime;
    uint16_t i,len;
    

    pTime = &time.prepare_wait_time;
    if (TimerCheck(NO_SYS_CTRL_BASE) == TRUE)
    {
        len = sizeof(SysControlTimeType)/sizeof(uint16_t);
        TimerSet(NO_SYS_CTRL_BASE,1); //base 1s
        for ( i = 0; i < len; i++)
        {
            if (*(pTime + i) != 0)
            {
                (*(pTime + i))--;
            }
        }
    }
    SysStandbyTimerCounter();
}

/************************************************************************
@name  	: SysStandbyCheckPumpOffRequire
@brief 	: 
@param 	: None
@return	: None
@note   : sys待机时,水泵关闭状态检测
*************************************************************************/
static uint8_t SysStandbyCheckPumpOffRequire(void)
{
    uint8_t res = FALSE;

    if (atw.sys.param.set.cell.pump_run_temp_meet == 1)
    {
        
    }
    
    return res;
}

/************************************************************************
@name  	: SysStandbyCheckPumpOffTemp
@brief 	: 
@param 	: None
@return	: None
@note   : sys待机时,预防水泵循环时导致水温过高的处理 //pump_Tw_high_check
            待机时检测水泵是否需要关闭
*************************************************************************/
static uint8_t SysStandbyCheckPumpOffTemp(void)
{
    uint8_t res = FALSE;

    if (GetSysSetRunMode() == SYS_MODE_HEAT)
    {
        if (atw.sys.local.Tw < (atw.sys.local.heat_Tw_set + 30))//TODO
        {
            time.pump_Tw_high_check = 30;
        }

        if ((atw.sys.local.standby_run_time > 5) && (time.pump_Tw_high_check == 0) )
        {
            res = TRUE;
        }
    }

    return res;
}

/************************************************************************
@name  	: SysStandbyCheckPumpOffTemp
@brief 	: 
@param 	: None
@return	: None
@note   : sys待机时,预防水泵循环时导致水温过高的处理 //pump_Tw_high_check
            待机时检测水泵是否需要开启(在关闭的情况下)
*************************************************************************/
static uint8_t SysStandbyCheckPumpOnTemp(void)
{
    uint8_t res = FALSE;

    if (GetSysSetRunMode() == SYS_MODE_HEAT)
    {
        if ((atw.sys.local.Tw <= (atw.sys.local.heat_Tw_set))
            || (atw.sys.local.pump_stop_time > 8))
        {
            res = TRUE;
        }
    }

    return res;
}


/************************************************************************
@name  	: SysStandbyPumpControl
@brief 	: 
@param 	: None
@return	: None
@note   : sys待机时,水泵运转控制
@note   : 1.当前有水泵运转
                共用水泵保持运转,非共用水泵,正在运转的水泵中,序号最小的单元水泵保持运转,其余水泵关闭
                共用水泵时,将0#单元水阀打开,其余单元水阀关闭;非共用水泵,水阀全部关闭
          2.无水泵运转
                将0#单元水泵开启
                共用水泵时,将0#单元水阀打开,其余单元水阀关闭;非共用水泵,水阀全部关闭
*************************************************************************/
static void SysStandbyPumpControl(void)
{
    uint8_t ch,ch_run,cnt = 0;
    uint8_t pump_type = atw.sys.table.unit[CHILLER_0].init.dsw1.bit.pump_type;

    for (ch = CHILLER_0; ch < CHILLER_CH_MAX; ch++)
    {
        if (atw.sys.table.unit[ch].event.pump_state == PUMP_STATE_RUN)
        {
            ch_run = ch;
            cnt++;
            break;
        }
    }
//
    if (cnt > 0)//有水泵运转
    {
        flag_pump_run_state = TRUE;
        if (pump_type == PUMP_PUBLIC)//共用水泵
        {
            atw.sys.table.unit[CHILLER_0].cmd.data2.bit.water_valve_ctrl = ON;
            for (ch = CHILLER_1; ch < CHILLER_CH_MAX; ch++)
            {
                atw.sys.table.unit[ch].cmd.data2.bit.water_valve_ctrl = OFF;
            }
        }
        else//非共用水泵
        {
            atw.sys.table.unit[ch_run].cmd.data2.bit.pump_open = ON;
            for (ch = ch_run + 1; ch < CHILLER_CH_MAX; ch++)
            {
                atw.sys.table.unit[ch].cmd.data2.bit.pump_open = OFF;
            }
            for (ch = CHILLER_0; ch < CHILLER_CH_MAX; ch++)
            {
                atw.sys.table.unit[ch].cmd.data2.bit.water_valve_ctrl = OFF;
            }
        }
    //检测是否需要关闭
        if (SysStandbyCheckPumpOffTemp() == TRUE)//
        {
            for (ch = CHILLER_0; ch < CHILLER_CH_MAX; ch++)
            {
                atw.sys.table.unit[CHILLER_0].cmd.data2.bit.pump_open = OFF;
            }
        }
    }
    else 
    {
        flag_pump_run_state = FALSE;
        if (SysStandbyCheckPumpOnTemp() == TRUE)//需要运转
        {
            atw.sys.table.unit[CHILLER_0].cmd.data2.bit.pump_open = ON;
            for (ch = CHILLER_0; ch < CHILLER_CH_MAX; ch++)
            {
                atw.sys.table.unit[ch].cmd.data2.bit.water_valve_ctrl = OFF;
            }
            if (pump_type == PUMP_PUBLIC)//共用水泵
            {
                atw.sys.table.unit[CHILLER_0].cmd.data2.bit.water_valve_ctrl = ON;
            }
        }
    }
}

/************************************************************************
@name  	: SysWaterPumpControl
@brief 	: 
@param 	: state:ON/OFF
@return	: None
@note   : 3.5.2 水泵,水阀,水流开关
            3)水泵,水阀动作
                3-1)系统待机
                    a)进入该状态后,共用水泵和正在运转的非共同水泵继续保持运转,若有水泵故障,水泵立即停止运转
                    b)延时[关机后水泵运转时间]后,水泵执行以下动作
                        b-1)当[达到设定温度停机后水泵运转设置]=1时,
                            满足所有条件,执行水泵停机
                                -系统用户侧只有智能温控器
                                -且,至少有一台智能温控器执行制冷(含除湿)待机,或制热待机
                                -且,没有
                        b-2)当[达到设定温度停机后水泵运转设置]=0时,水泵运转
                3-2)模块系统关机
                    a)进入该状态后,共用水泵和正在运转的非共同水泵继续保持运转,若有水泵故障,水泵立即停止运转
                    b)延时[关机后水泵运转时间]后,水泵停止运转
                3-3)模块系统运转准备
                    共用水泵:将单元中的所有水阀打开,3s后开共用水泵
                    非共用水泵:单元中的水泵不运转
                3-4)模块系统运转
                共用水泵:保持运转,单元运转准备/运转:水阀打开;单元
                非共用水泵:单元中的水泵不运转
*************************************************************************/
void SysWaterPumpControl(uint8_t unit_ch,uint8_t state)
{
    uint8_t ch;
    uint8_t sys_step = atw.sys.local.work_step;
    uint8_t last_step = atw.sys.local.last_step;

    switch (sys_step)
    {
        case SYS_STATUS_OFF://系统停机
        {
        //get pump run time    
            if (last_step == SYS_STATUS_INIT )
            {
                time.pump_run_wait = 90; 
            }
            else if (last_step != SYS_STATUS_OFF)
            {
                time.pump_run_wait = atw.sys.param.time_pump_run_sys_close*60;//1mins 60s
            }
        //control
            for (ch = CHILLER_0; ch < CHILLER_CH_MAX; ch++)
            {
                if (atw.sys.table.unit[ch].event.pump_state == PUMP_STATE_ERROR)//水泵故障
                {
                    atw.sys.table.unit[ch].cmd.data2.bit.pump_open = FALSE;
                    atw.sys.table.unit[ch].cmd.data2.bit.water_valve_ctrl = OFF;
                }
                else if (time.pump_run_wait == 0)//无故障,等待时间完成
                {
                    atw.sys.table.unit[ch].cmd.data2.bit.pump_open = FALSE;
                    atw.sys.table.unit[ch].cmd.data2.bit.water_valve_ctrl = OFF;
                }
            }
        }
            break;

        case SYS_STATUS_STANDBY://sys待机
        {
            if (last_step != SYS_STATUS_OFF)
            {
                time.pump_run_wait = atw.sys.param.time_pump_run_sys_close*60;//1mins/60s
            }
            else if (time.pump_run_wait == 0)
            {
                if (SysStandbyCheckPumpOffRequire() == TRUE)//关闭需求
                {
                    atw.sys.table.unit[ch].cmd.data2.bit.pump_open = FALSE;
                    atw.sys.table.unit[ch].cmd.data2.bit.water_valve_ctrl = OFF;
                }
                else//
                {
                    SysStandbyPumpControl();
                }
            }
        }
            break;
        case SYS_STATUS_PREPARE://sys运转准备 //TODO 待确认水泵逻辑
        {
            if (last_step != SYS_STATUS_PREPARE)
            {
                time.pump_run_wait = 3;
            }
            else
            {
                for (ch = CHILLER_0; ch < CHILLER_CH_MAX; ch++)
                {
                    atw.sys.table.unit[ch].cmd.data2.bit.water_valve_ctrl = ON;
                    if (time.pump_run_wait == 0)
                    {
                        atw.sys.table.unit[ch].cmd.data2.bit.pump_open = ON;
                    }
                }
            }
        }
            break;

        case SYS_STATUS_RUN:
        {
           
        }
            break;
        default:
            break;
    }
} 

/************************************************************************
@name  	: SysStatusOff
@brief 	: 
@param 	: None
@return	: None
@note   : 3.4.3 系统待机与关机
            模块系统关机
            1.发送关机指令至所有单元
            2.无水泵故障,水泵状态保持[关机后水泵运转时间],之后,执行关机状态下的水泵运转逻辑
            4.关闭辅助电加热;90s后,执行关机状态下的辅助电加热运转逻辑
*************************************************************************/
static void SysStatusOff(void)
{ 
    SysSetAllUnitSwitchState(OFF);

}

/************************************************************************
@name  	: SysStatusPrepare
@brief 	: 
@param 	: None
@return	: None
@note   : 3.4.1 模块系统运转准备
            控制逻辑
            1.模块单元保持关机状态
            2.执行水泵,水阀,水流开关中水泵和水阀动作
            3.执行辅助水电加热控制逻辑
            4.执行地板采暖二通阀控制逻辑
          3.5.2 水泵,水阀,水流开关
           3)水泵,水阀动作
            3-3)模块系统运转准备
             1.对于共用水泵,将单元中所有水阀打开,3s后开共用水泵
             2.对于非共用水泵,单元中的水泵不运转
          3.5.3 地板采暖二通阀控制
            3)系统运转准备,系统运转:非测试模式,测试模式下 ...
          3.5.4 辅助水电加热控制逻辑
           2)启用,运转准备时:关闭
*************************************************************************/
static void SysStatusPrepare(void)
{    

}

/************************************************************************
@name  	: SysStatusRun
@brief 	: 
@param 	: None
@return	: None
@note   : 3.4.2 模块系统运转
            1.检测总水温,确定系统需执行的运转模式
            2.制冷/制热时,调用能调计算与分配逻辑,并将运转需求发送给相应的单元
            3.制热运转中,系统接收单元的除霜申请,并根据以下原则,来决定允许除霜的单元
                将申请的单元按照申请先后排序
                检测当前水温,当水温Tw>=15°时,相排名最优先的单元发送允许除霜指令一次,并保证系统中同时进行除霜的单元数量N<=MAX(1,总单元/2)
            4.水泵循环运转,发送水泵循环需求给所有单元
            5.防冻保护模式,调用水系统防冻结保护来决定单元的运转
*************************************************************************/
static void SysStatusRun(void)
{  
    uint8_t mode = GetSysSetRunMode();
	
    switch (mode)
    {
        case SYS_MODE_COOL:          SysStatusRunCool();        break;
        case SYS_MODE_HEAT:          SysStatusRunHeat();        break;
        case SYS_MODE_ANTIFREEZE:    SysStatusRunAntifreeze();  break;
        case SYS_MODE_PUMP:          SysStatusRunPump();        break;
        default:    break;
    }
}

/************************************************************************
@name  	: SysStatusStandby
@brief 	: 
@param 	: None
@return	: None
@note   : 3.4.3 模块系统待机与关机
            进入时,发送关机指令给所有单元,水泵运转状态保持
            水泵运转[关机后水泵运转时间,默认3mins]后,执行待机状态下的水泵运转逻辑
                1.确定系统待机时水泵是否运转:当[达到设定温度停机后水泵运转设置]=1时,满足以下所有条件,执行水泵停机
                    a.系统用户侧只有智能温控器,
                    b.且至少一台智能温控器执行制冷/除湿待机,或制热待机,
                    c.且没有智能温控器执行制冷/除湿运转,或制热运转
                否则,水泵运转
            当[达到设定温度停机后水泵运转设置]=0时,水泵运转 //TODO to ensure
*************************************************************************/
static void SysStatusStandby(void)
{
    SysSetAllUnitSwitchState(OFF);//所有单元关机
    //TODO
}

//====status change
/************************************************************************
@name  	: CheckSysStatusInitToOff
@brief 	: 
@param 	: None
@return	: None
@note   : 3.4.3 系统待机与关机
            模块系统关机
            1.延时30s,单元自检后,进行系统通讯检测
            2.通讯检测成功后,发送关机指令至所有单元
            3.关闭水泵;90s后,执行关机状态下的水泵运转逻辑;若有水泵故障,水泵立即停止运转
            4.关闭辅助电加热;90s后,执行关机状态下的辅助电加热运转逻辑
*************************************************************************/
static uint8_t CheckSysStatusInitToOff(void)//上电->关机 //TODO
{
    uint8_t res = FALSE;
        
    if (time.init_wait == 0)//延时等待自检完成
    {
        
    }

    return res;
}

/************************************************************************
@name  	: CheckSysStatusOffToPrepare
@brief 	: 
@param 	: None
@return	: None
@note   : 3.4 条件
            1.无系统运转限制
            2.且接收关开指令(制冷,制热,水泵循环)
            3.且满足制冷,制热,水泵循环开机条件
            4.或系统检测到满足防冻保护条件,需要进入防冻保护运转
*************************************************************************/
static uint8_t CheckSysStatusOffToPrepare(void)//关机->运转准备
{
    uint8_t res = FALSE;

    if (((CheckSysOperateState() == TRUE) && (GetSysSetRunMode() != SYS_MODE_NULL))//可运转,且有开机指令
        || (CheckSysAntifreezeRequire() == TRUE))//或有防冻保护需求
    {
        res = TRUE;
    }
    return res;
}

/************************************************************************
@name  	: CheckSysStatusPrepareToOff
@brief 	: 
@param 	: None
@return	: None
@note   : 3.4 条件3,满足下任一条件 
            1.接收关机指令
            2.发生系统运转限制
            3.进入本状态超181s
*************************************************************************/
static uint8_t CheckSysStatusPrepareToOff(void)//运转准备->关机
{
    uint8_t res = FALSE;

    if (((GetSysSetRunMode() == SYS_MODE_NULL))//关机指令
        || (CheckSysOperateState() == FALSE) //系统运转限制
        || (time.prepare_wait_time == 0))//进入本状态超时
    {
        res = TRUE;
    }

    return res;
}

/************************************************************************
@name  	: CheckSysStatusPrepareToRun
@brief 	: 
@param 	: None
@return	: None
@note   : 3.4 条件s4
            1. 非共用水泵,进入本状态超过3s,且无系统运转限制
            2. 共用水泵,进入本状态超过25s,且总水流开关持续5s导通,且无系统运转限制
*************************************************************************/
static uint8_t CheckSysStatusPrepareToRun(void)//运转准备->运转
{
    uint8_t res = FALSE;
    uint8_t pump_type = atw.unit.table.init.dsw1.bit.pump_type;
    uint8_t time_judge;

    if (pump_type == PUMP_PUBLIC)
        time_judge = 156; //181 - 25 = 156
    else time_judge = 178;

    if (CheckSysOperateState() == TRUE)//无系统运转限制
    {
       if (time.prepare_wait_time < time_judge)//
       {
           res = TRUE;
       }
    }

    return res;
}

/************************************************************************
@name  	: CheckSysStatusRunToOff
@brief 	: 
@param 	: None
@return	: None
@note   : 3.4 s5,满足下任一条件 
            1.接收关机指令, 或发生系统运转限制
            2.防冻保护时,总回水温度>20
*************************************************************************/
static uint8_t CheckSysStatusRunToOff(void)//运转->关机
{
    uint8_t res = FALSE;

    if ((CheckSysOperateState() != TRUE) || (GetSysSetRunMode() == SYS_MODE_NULL))//系统运转限制 //关机指令      //TODO 防冻
    {
        res = TRUE;
    }
    return res;
}

/************************************************************************
@name  	: CheckSysStatusRunToStandby
@brief 	: 
@param 	: None
@return	: None
@note   : 3.4 s7 
            1.系统制冷/制热,满足制冷/制热待机条件,且无系统运转限制 3.2.2
            2.系统接收到运转模式切换(制冷->制热,制热->制冷)
            3.制冷/制热,系统急停,且无系统运转限制
*************************************************************************/
static uint8_t CheckSysStatusRunToStandby(void)//运转->待机
{
    uint8_t res = FALSE;

    if (CheckSysOperateState() == TRUE)//无系统运转限制
    {
        if ((GetSysRealRunMode() == SYS_MODE_COOL) && (GetSysSetRunMode() == SYS_MODE_HEAT))//模式切换
        {
            res = TRUE;
        }
        else if ((GetSysRealRunMode() == SYS_MODE_HEAT) && (GetSysSetRunMode() == SYS_MODE_COOL))//
        {
            res = TRUE;
        }
        else if (GetSysUrgencyStopState() == TRUE) //系统急停
        {
            res = TRUE;
        }
        else//水温满足待机条件
        {
            if (GetSysRealRunMode() == SYS_MODE_COOL)//制冷 Tw <= Tw_set -DTw
            {
                if (atw.sys.local.Tw <= (atw.sys.local.cool_Tw_set - atw.sys.param.temp_ctrl_fix))//Tw<=Tw_set - DTW
                {
                    res = TRUE;
                }
            }
            else if (GetSysRealRunMode() == SYS_MODE_HEAT)//制热 Tw >= Tw_set + DTw
            {
                if (atw.sys.local.Tw <= (atw.sys.local.heat_Tw_set + atw.sys.param.temp_ctrl_fix))
                {
                    res = TRUE;
                }
            }
        }
        
    }
    return res;
}

/************************************************************************
@name  	: CheckSysStatusStandbyToOff
@brief 	: 
@param 	: None
@return	: None
@note   : 3.4 条件8,满足下任一条件 
            1.接收到关机指令
            2.或系统运转限制
*************************************************************************/
static uint8_t CheckSysStatusStandbyToOff(void)//待机->关机
{
    uint8_t res = FALSE;

    if ((GetSysSetRunMode() == SYS_MODE_NULL)//收到关机指令
        || (CheckSysOperateState() == FALSE))//系统运转限制
    {
        res = TRUE;
    }
    return res;
}

/************************************************************************
@name  	: CheckSysStatusStandbyToRun
@brief 	: 
@param 	: None
@return	: None
@note   : 3.4 条件6,满足下任一条件 
            1.运转模式为制冷或制热,且水温满足制冷/制热开机条件3.2.2,且无系统运转限制
            2.系统产生防冻保护需求,需要进入防冻保护运转
*************************************************************************/
static uint8_t CheckSysStatusStandbyToRun(void)//待机->运转 //TODO 防冻
{
    uint8_t res = FALSE;
//温度计时
    if (GetSysSetRunMode() == SYS_MODE_COOL)//制冷
    {
        if (atw.sys.local.Tw <= (atw.sys.local.cool_Tw_set + atw.sys.param.temp_ctrl_fix))//Tw > Tw_set + DTW 运转
        {
            time.run_standby_to_wait = TM_STANDBY_TO_RUN;
        }
    }
    else if (GetSysSetRunMode() == SYS_MODE_HEAT)//制热 Tw < Tw_set - DTw 运转
    {
        if (atw.sys.local.Tw >= atw.sys.local.heat_Tw_set - atw.sys.param.temp_ctrl_fix)
        {
            time.run_standby_to_wait = TM_STANDBY_TO_RUN;
        }
    }
//条件判断
    if ((CheckSysOperateState() == TRUE)//无系统运转限制
        && (time.run_standby_to_wait == 0))//时间满足
    {
        res = TRUE;  
    }
    return res;
}

/************************************************************************
@name  	: ChillerSysStatusControl
@brief 	: 
@param 	: None
@return	: None
@note   : 3.4 模块系统状态及控制程序
*************************************************************************/
static void ChillerSysStatusControl(void)
{
    uint8_t sys_work_step = atw.sys.local.work_step;
    // uint8_t last_step = atw.sys.local.last_step;
	
    switch (sys_work_step)
    {
        case SYS_STATUS_INIT://上电
        {
            if (CheckSysStatusInitToOff() == TRUE)//init -> off
            {
                sys_work_step = SYS_STATUS_OFF; //
                SysSetAllUnitSwitchState(OFF);  //
            }
        }
            break;
        case SYS_STATUS_OFF://关机
        {
            if (CheckSysStatusOffToPrepare() == TRUE)//off -> prepare
            {
                sys_work_step = SYS_STATUS_PREPARE; //进入运转准备
                time.prepare_wait_time = 181;
            }
            else SysStatusOff();
        }
            break;
        case SYS_STATUS_PREPARE://运转准备
        {
            if (CheckSysStatusPrepareToOff() == TRUE)//prepare -> off
            {
                sys_work_step = SYS_STATUS_OFF;
            }
            else if (CheckSysStatusPrepareToRun() == TRUE)//prepare -> run
            {
                sys_work_step = SYS_STATUS_RUN;
            }
            else SysStatusPrepare();
        }
            break;
        case SYS_STATUS_RUN://运转
        {
            if (CheckSysStatusRunToOff() == TRUE)//run -> off
            {
                sys_work_step = SYS_STATUS_OFF;
            }
            else if (CheckSysStatusRunToStandby() == TRUE)//run -> standby
            {
                sys_work_step = SYS_STATUS_STANDBY;
                time.run_standby_to_wait = TM_STANDBY_TO_RUN;
            }
            else SysStatusRun();
        }
            break;
        case SYS_STATUS_STANDBY://待机
        {
            if (CheckSysStatusStandbyToOff() == TRUE)//standby -> off
            {
                sys_work_step = SYS_STATUS_OFF;
            }
            else if (CheckSysStatusStandbyToRun() == TRUE)//standby -> run
            {
                sys_work_step = SYS_STATUS_STANDBY;
            }
            else SysStatusStandby();
        }
            break;
        default:    sys_work_step = SYS_STATUS_OFF;
            break;
    }
    atw.sys.local.last_step = atw.sys.local.work_step;
    atw.sys.local.work_step = sys_work_step;
}

/************************************************************************
@name  	: ChillerSysInit
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ChillerSysInit(void)
{
    time.init_wait = COMM_CHECK_WAIT; //TODO 自检时间
}

/************************************************************************
@name  	: ChillerSysControl
@brief 	: 
@param 	: None
@return	: None
@note   : 2.4.3模块系统控制功能
            1.系统故障处理
            2.防冻进入和退出条件判断
            3.能调计算与分配
            4.水阀的开启和关闭
            5.报警指示
            6.冷热模式切换
            7.系统开关
            8.机组运行时间限制
            9.化霜进入判断
            10.共用水泵开启
            11.来电重启和来电待机
            12.辅助电加热的开启和关闭
*************************************************************************/
void ChillerSysControl(void)
{
    SysControlTimerCounter();   //timer counter use in sys control
    ChillerSysStatusControl();  //状态迁移
    SysWaterHeaterControl();    //辅助电加热控制

    SysTwsysoutUpdate();
    SysTwsysinUpdate();
    SysTwUpdate();
    SysTwsetUpdate();

}

