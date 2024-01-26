/***********************************************************************
@file   : ChillerUnit.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "ChillerUnit.h"
#include "Data.h"
#include "ChillerCycle.h"
#include "data_unit.h"
#include "AlarmCheck.h"
#include "TimerCounter.h"
#include "UnitStateControl.h"
#include "UnitDeviceControl.h"
#include "config.h"


#define TM_POW_ON_CHECK         ((uint8_t)20)//unit上电自检时间
#define TM_UNIT_PREPARE_KEEP    ((uint8_t)40)//unit运转准备持续时间
#define TM_URGENCY_STOP_WAIT    ((uint8_t)180)//unit急停等待时间



//time //decrease //increase
typedef struct
{
    uint8_t unit_prepare_wait;  //单元运转准备持续40s
    uint8_t urgency_stop_wait;  //急停等待 //3mins内单元不可运转

}UnitControlTimeType;

static UnitControlTimeType time;






/************************************************************************
@name  	: UnitControlTimerCounter
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void UnitControlTimerCounter(void)
{
    uint8_t *pTime;
    uint16_t i,len;

//1s
    pTime = &time.unit_prepare_wait;
    if (TimerCheck(NO_UNIT_CTRL_BASE) == TRUE)
    {
        len = sizeof(UnitControlTimeType)/sizeof(uint8_t);
        TimerSet(NO_UNIT_CTRL_BASE,1); //base 1s
        for ( i = 0; i < len; i++)
        {
            if (*(pTime + i) != 0)
            {
                (*(pTime + i))--;
            }
        }
    }
//1hour
    if (TimerCheck(NO_UNIT_FOR_TIMING) == TRUE)//base 1mins
    {
        TimerSet(NO_UNIT_FOR_TIMING,59); //1mins base
        
    }
}





/************************************************************************
@name  	: UnitUrgencyStopCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 单元急停检测
            单元检测到当前水温处于急停区时,单元发生急停
            1.立即关闭所有cycle
            2.单元处于不可运转状态
            3.累计3mins后,变为可运转状态
*************************************************************************/
uint8_t UnitUrgencyStopCheck(void)
{
    uint8_t res = FALSE;

    if (time.urgency_stop_wait != 0 )
    {
        res = TRUE;
    }
    else
    {
        //TODO 水温判断
        if (0)
        {
            time.urgency_stop_wait = TM_URGENCY_STOP_WAIT;
        }
    }
    return res;
}


/************************************************************************
@name  	: UnitStatusInitFinish
@brief 	: 
@param 	: None
@return	: None
@note   : 模块单元上电,自检流程
            1.组内通信
            2.cycle自检完成
            3.硬件初始化
*************************************************************************/
static uint8_t UnitStatusInitFinish(void)
{
    uint8_t res = FALSE;

    if ((atw.unit.local.chillerlink_state == TRUE)//组内通信建立
        && (1))  //cycle自检完成
    {
        /* code */
    }
    


    return res;
}

/************************************************************************
@name  	: UnitStatusOff
@brief 	: 
@param 	: None
@return	: None
@note   :  4.2.3 模块单元关机
            1.上电20s后,通信检测(组控器,及配置了可调节开关和模式的设备)
            2.上电40s后,进行下述动作 //chiller通信同步
            上电40s后
                a.上报cycle状态:可运转cycle数量,已运转数量
                c.根据需要执行底盘电加热控制逻辑
                d.根据系统发送的水泵运转指令,控制本单元水泵开关,以及本单元中水阀的开关
*************************************************************************/
static void UnitStatusOff(void)
{
    uint8_t cycle;
//关闭cycle
    for (cycle = 0; cycle < atw.unit.local.cycle_active_cnt; cycle++)
    {
        UnitSetCycleClose(cycle);
    }

}


/************************************************************************
@name  	: UnitStatusPrepare
@brief 	: 
@param 	: None
@return	: None
@note   : 4.2.1 模块单元运转准备
            控制内容
                1.单元风机检测运转
                    进入本状态后,风机执行6档转速,若20s后,实际风机转速与目标值差10rps以上,则该cycle风机故障,累计一次cycle风机故障,该cycle不可运转,否则,该cycle风机正常.
                2.单元水泵检测运转
                    进入本状态后,20s后,检测本单元的水流开关,若持续5s保持通的状态,则判断水流正常,若持续5s保持断开,则判断水流不正常,累计单元水泵故障1次
                3.非共用水泵,延迟3s打开
*************************************************************************/
static void UnitStatusPrepare(void)
{
    atw.unit.local.rt_run_mode = atw.unit.local.set_run_mode; //update rt_mode

}

/************************************************************************
@name  	: UnitStatusRun
@brief 	: 
@param 	: None
@return	: None
@note   : 4.2.2 单元运转
            目前是单机运行,单元只控制CYCLE1
*************************************************************************/
static void UnitStatusRun(void)
{
    uint8_t unit_mode = GetUnitRealRunMode();

    switch (unit_mode)
    {
        case UNIT_MODE_COOL:  UnitRunCool();    break;
        case UNIT_MODE_HEAT:  UnitRunHeat();    break;
        case UNIT_MODE_PUMP:  UnitRunPump();    break;
        default:   break;
    }
}

//===
/************************************************************************
@name  	: CheckUnitStatusOffToPrepare
@brief 	: 
@param 	: None
@return	: None
@note   : 4.2 满足所有条件,进入unit运转准备状态
            1.单元可运转
            2.接收到系统的开机指令
*************************************************************************/
static uint8_t CheckUnitStatusOffToPrepare(void)//
{
    uint8_t res = FALSE;

    if ((GetUnitSetRunMode() != UNIT_MODE_NULL)//收到系统开机指令
        && (UnitOperableStateCheck() == TRUE))//且单元可运行
    {
        res = TRUE;
    }

    return res;
}

/************************************************************************
@name  	: CheckUnitStatusPrepareToOff
@brief 	: 
@param 	: None
@return	: None
@note   : 4.2 满足任一条件,进入unit关机状态
            1.接收到关机指令
            2.检测到单元不可运转
*************************************************************************/
static uint8_t CheckUnitStatusPrepareToOff(void)
{
    uint8_t res = FALSE;

    if ((UnitOperableStateCheck() == FALSE) //单元不可运转
        || (GetUnitSetRunMode() == UNIT_MODE_NULL))//收到关机指令
    {
        res = TRUE;
    }

    return res;
}

/************************************************************************
@name  	: CheckUnitStatusPrepareToRun
@brief 	: 
@param 	: None
@return	: None
@note   : 4.2.2 满足所有条件,进入模块单元运转
            1.运转时间超40s
            2.单元可运转
            3.运转模式为制冷/制热/水泵运转
*************************************************************************/
static uint8_t CheckUnitStatusPrepareToRun(void)
{
    uint8_t res = FALSE;

    if ((UnitOperableStateCheck() == TRUE)          //可运转
        &&(GetUnitSetRunMode() != UNIT_MODE_NULL)   //没有关机
        &&((time.unit_prepare_wait == 0)))          //时间超40s
    {
        res = TRUE;
    }

    return res;
}

/************************************************************************
@name  	: CheckUnitStatusRunToOff
@brief 	: 
@param 	: None
@return	: None
@note   : 4.2 满足任一条件,进入unit关机状态
            1.检测到单元不可运转
            2.单元发生急停
            3.系统关机
*************************************************************************/
static uint8_t CheckUnitStatusRunToOff(void)
{
    uint8_t res = FALSE;

    if ((UnitOperableStateCheck() == FALSE)//单元禁止运行
        // ||(UnitGetOperableCycleCounter() == 0)//
        ||(GetUnitSetRunMode() == UNIT_MODE_NULL))//收到关机指令
    {
        res = TRUE;
    }
    return res;
}



/************************************************************************
@name  	: ChillerUnitStatusControl
@brief 	: 
@param 	: None
@return	: None
@note   : 
*************************************************************************/
static void ChillerUnitStatusControl(void)
{
    uint8_t work_step = atw.unit.local.work_step;
    //uint8_t last_step = atw.unit.local.last_step;

    switch (work_step)
    {
        case UNIT_STATUS_INIT://上电
        {
            if (UnitStatusInitFinish() == TRUE)//自检
            {
                work_step = UNIT_STATUS_OFF;
            }
        }
            break;
        case UNIT_STATUS_OFF://关机
        {
            if (CheckUnitStatusOffToPrepare() == TRUE)//off -> prepare
            {
                work_step = UNIT_STATUS_PREPARE;
                time.unit_prepare_wait = TM_UNIT_PREPARE_KEEP;
            }
            else UnitStatusOff();
        }
            break;
        case UNIT_STATUS_PREPARE://运转准备
        {
            if (CheckUnitStatusPrepareToOff() == TRUE) //prepare -> off
            {
                work_step = UNIT_STATUS_OFF;
            }
            else if (CheckUnitStatusPrepareToRun() == TRUE)//prepare -> run
            {
                work_step = UNIT_STATUS_RUN;
            }
            UnitStatusPrepare();
        }
            break;        
        case UNIT_STATUS_RUN://运转
        {
            if (CheckUnitStatusRunToOff() == TRUE) //run -> off
            {
                work_step = UNIT_STATUS_OFF;
            }
            UnitStatusRun();
        }
        default:    work_step = UNIT_STATUS_OFF;
            break;        
    }
    atw.unit.local.last_step = atw.unit.local.work_step;
    atw.unit.local.work_step = work_step;
}

/************************************************************************
@name  	: ChillerUnitInit
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ChillerUnitInit(void)
{
    uint8_t cycle_ch;

    UnitGetParamFromConfig();
    for (cycle_ch = CYCLE_1; cycle_ch < atw.unit.local.cycle_active_cnt; cycle_ch++)
    {
        ChillerCycleInit(cycle_ch);
    }
}

/************************************************************************
@name  	: ChillerUnitControl
@brief 	: 
@param 	: None
@return	: None
@note   : 2.4.4模块单元控制功能
            1.压缩机频率控制
            2.电子膨胀阀控制
            3.风机转速控制
            4.电磁阀控制        //cycle
            5.静压控制          //esp
            6.静音功能          //mute control
            7.单元化霜进入和退出 //defrost control
            8.单元故障处理      //alarm handle
@note   : 4.模块单元控制逻辑
            内部包含:
            cycle:包含风机,压缩机,四通阀,电子膨胀阀,电磁阀
            水系统:水泵,水阀
            其它:底盘加热,板换电加热
          4.2模块单元状态及控制程序
            如果上电或复位:电子膨胀阀EXV1/2/3复位,上电20s,通信检测
            上电40s后
                a.上报cycle状态:可运转cycle数量,已运转数量
                b.检测本单元中水温,若满足防冻保护条件,则向系统上报进入防冻保护申请,见防冻 保护控制逻辑
                c.根据需要执行底盘电加热控制逻辑
                d.根据系统发送的水泵运转指令,控制本单元水泵开关,以及本单元中水阀的开关
*************************************************************************/
void ChillerUnitControl(void)
{
    uint8_t cycle_ch;

    UnitControlTimerCounter();
    ChillerUnitStatusControl();
    for (cycle_ch = CYCLE_1; cycle_ch < atw.unit.local.cycle_active_cnt; cycle_ch++)
    {
        ChillerCycleControl(cycle_ch);
    }

    UnitOperableStateUpdate();//unit可运转状态更新
    UnitSetRunModeUpdate();   //unit运转模式更新,来自系统

}
