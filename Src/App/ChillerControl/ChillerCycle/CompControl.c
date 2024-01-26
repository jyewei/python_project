/***********************************************************************
@file   : CompControl.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
@note   : 1.运行中增加对回油频率的检测,需要对累计回油进入时间清零
          2.回油用的计时判断条件
************************************************************************/
#include "CompControl.h"
#include "Data.h"
#include "safety_function.h"
#include "data.h"
#include "TimerCounter.h"
#include "CycleStateControl.h"
#include "CompProtect.h"
#include "AlarmCheck.h"


#define RESONANT_FREQUENCY  ((uint16_t)100)//共振频率 //TODO to ensure
#define TM_PRECHARGE        ((uint8_t)20)  //预充电时间
#define TM_3MINS_PROTECT    ((uint8_t)180) //3mins保护时间

//time //decrease //increase
typedef enum
{
    DEFROST_STATUS_INIT,
    DEFROST_STATUS_FREQ_UP,
    DEFROST_STATUS_FREQ_FIX,
    DEFROST_STATUS_MAX
    
}DefrostStepEnum;




//time //decrease //increase
typedef struct
{
    uint16_t base_1mins;         //
    uint16_t pecharge_wait;      //预充电等待
    uint16_t freq_set_wait;      //频率控制等待
    uint16_t comp_run_wait;      //1mins base
    uint16_t protect_3mins;      //

    uint16_t fimax4_refresh;     //fimax calculate period
    uint16_t P09_state_wait;     //P09中除霜运转准备开始50s~知道差压控制中 用
    uint16_t P03_state1_check;   //P03 2次电流保护


}CompressorTimeType;

static CompressorTimeType time[CYCLE_CH_MAX][COMP_NO_MAX];

/************************************************************************
@name  	: CompGetFimax4RefreshTimer
@brief 	: 
@param 	: None
@return	: None
@note   : Fimax4用计时器
*************************************************************************/
uint16_t CompGetFimax4RefreshTimer(uint8_t cycle,uint8_t comp)
{
    return time[cycle][comp].fimax4_refresh;
}

/************************************************************************
@name  	: CompGetFimax4RefreshTimer
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void CompSetFimax4RefreshTimer(uint8_t cycle,uint8_t comp,uint16_t tm)
{
    time[cycle][comp].fimax4_refresh = tm;
}

/************************************************************************
@name  	: CompGetInv2CheckTimer
@brief 	: 
@param 	: None
@return	: None
@note   : P03电流保护中,2次电流检测用
*************************************************************************/
uint16_t CompGetInv2CheckTimer(uint8_t cycle,uint8_t comp)
{
    return time[cycle][comp].P03_state1_check;
}

/************************************************************************
@name  	: CompSetInv2CheckTimer
@brief 	: 
@param 	: None
@return	: None
@note   : P03电流保护中,2次电流检测用
*************************************************************************/
void CompSetInv2CheckTimer(uint8_t cycle,uint8_t comp,uint16_t tm)
{
    time[cycle][comp].P03_state1_check = tm;
}

/************************************************************************
@name  	: CompTimerCounter
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void CompTimerCounter(uint8_t cycle,uint8_t comp)
{
    uint16_t *pTime;
    uint16_t i,len;

    pTime = &time[cycle][comp].base_1mins;
    if (TimerCheck(NO_CYCLE1_COMP_BASE+cycle) == TRUE)
    {
        len = sizeof(CompressorTimeType)/sizeof(uint16_t);
        TimerSet(NO_CYCLE1_COMP_BASE+cycle,1); //base 1s
        for ( i = 0; i < len; i++)
        {
            if (*(pTime + i) != 0)
            {
                (*(pTime + i))--;
            }
        }
    }
//1mins
    if (time[cycle][comp].base_1mins == 0)
    {
        time[cycle][comp].base_1mins = 60;
    //压机运行时间
        if (atw.cycle[cycle].comp[comp].link.state.freq_output > 0)
        {
            atw.cycle[cycle].comp[comp].local.current_run_time++;//当前运行时间
            atw.cycle[cycle].comp[comp].local.total_run_time++; //累计运行时间
        }
        else atw.cycle[cycle].comp[comp].local.current_run_time = 0;
        
        
    }
    


}

/************************************************************************
@name  	: CompCheckColdStartRequire
@brief 	: 
@param 	: None
@return	: 检测压缩机是否需要冷启动
          7.1 压缩机
            1)压缩机冷启动保护(电源上电后首次启动)
*************************************************************************/
uint8_t CompProtectP09StepCheck(uint8_t cycle,uint8_t comp)
{
    uint8_t res = FALSE;
    uint8_t last_step = atw.cycle[cycle].last_step;
    uint8_t work_step = atw.cycle[cycle].work_step;

    if ((work_step == CYCLE_STATUS_DEFROST_PREPARE) && (work_step != CYCLE_STATUS_DEFROST_PREPARE))
    {
        time[cycle][comp].P09_state_wait = 50;
    }
    if (work_step == CYCLE_STATUS_DEFROST_PREPARE)
    {
        if (time[cycle][comp].P09_state_wait == 0)
        {
            res = TRUE;
        }
    }

    return res;
}

/************************************************************************
@name  	: CompCheckColdStartRequire
@brief 	: 
@param 	: None
@return	: 检测压缩机是否需要冷启动
          7.1 压缩机
            1)压缩机冷启动保护(电源上电后首次启动)
*************************************************************************/
static uint8_t CompCheckColdStartRequire(uint8_t cycle,uint8_t comp)
{
    uint8_t res = FALSE;
    uint8_t unit_work_state = atw.unit.table.init.dsw1.bit.unit_work_state;
    int16_t Td = atw.cycle[cycle].comp[comp].local.Td;
    int16_t tam_temp = atw.unit.table.io.Tam;
    uint32_t cycle_power_on_time = atw.cycle[cycle].power_on_time;
    uint16_t comp_preheat_time = atw.unit.table.sys_cmd.comp_init_prewarm_time;//初始上电压缩机预热时间

    if (atw.unit.local.sns_err.bit.Tam == TRUE)
    {
        tam_temp = 180;
    }
    if (unit_work_state != UNIT_STATE_TEST)//非测试模式
    {
        if ((tam_temp <= 180)                       //环温<=18°
            && ((Td-tam_temp <= 180) || (Td < 160)) //且
            && (cycle_power_on_time < comp_preheat_time))
        {
            res = TRUE;
        }
    }
    return res;
}

/************************************************************************
@name  	: CompCheckColdStartQuit
@brief 	: 
@param 	: None
@return	: 检测压缩机冷启动是否需要退出
          7.1 压缩机
            1)压缩机冷启动保护(电源上电后首次启动)
*************************************************************************/
static uint8_t CompCheckColdStartQuit(uint8_t cycle,uint8_t comp)
{
    int16_t tam_temp = atw.unit.table.io.Tam;
    int16_t Td = atw.cycle[cycle].comp[comp].local.Td;
    uint32_t cycle_power_on_time = atw.cycle[cycle].power_on_time;
    uint8_t res = FALSE;

    if (atw.unit.local.sns_err.bit.Tam == TRUE)
    {
        tam_temp = 180;
    }

    if ((tam_temp >= 200)
        || ((Td - tam_temp >= 220) && (Td >= 160))
        || (Td >= 210)
        || (cycle_power_on_time >= 180))
    {
        res = TRUE;
    }

    return res;
}

/************************************************************************
@name  	: CheckCompOperateState
@brief 	: 查看压缩机是否可运行
@param 	: None
@return	: None
@note   : 7.1
            (4)压缩机不可以启动运转情形
                a.处于冷启动保护
                b.处于3mins屏蔽保护
                c.因保护控制停机,尚未退出保护状态
                d.处于锁定状态,,未复位
*************************************************************************/
uint8_t CheckCompOperateState(uint8_t cycle,uint8_t comp)
{
    uint8_t res = TRUE;
    uint8_t step = atw.cycle[cycle].comp[comp].local.work_step;

    if ((step == COMP_RUN_COLD_START)       //冷启动
        || (step == COMP_3MINS_PROTECT)     //3mins保护 
        || (CheckCompAlarmState(cycle,comp) != FALSE))//无告警
    {
        res = FALSE;
    }
	return res;
}


/************************************************************************
@name  	: CompSpeedStepSet
@brief 	: 
@param 	: freq_step:单位0.1Hz
@return	: None
@note   : 7.1.4 升降频速率
            link:1~1000 ->   0.1 ~ 100Hz/s
*************************************************************************/
void CompSpeedStepSet(uint8_t cycle,uint8_t comp)//
{
    uint16_t freq_step;
    uint16_t freq_out = atw.cycle[cycle].comp[comp].link.state.freq_output;
    uint16_t freq_set = atw.cycle[cycle].comp[comp].link.ctrl.freq_set;
    int16_t freq_difference;

    freq_difference = freq_set - freq_out;
    if (freq_out >= 400) //1.频率大于40Hz时,2Hz/5s
    {
        freq_step = 4;  //2Hz/5s
    }
    else if (0)//2.进入保护条件,需要强制降频时,3Hz/s //TODO 强制降频条件
    {
        freq_step = 30;
    }
    else if (((freq_difference >= 300) && (CompGetPressureRatio(cycle,comp) <= 65) )//3.频率增加值>=30,且压力比ξmax<=6.5,3Hz/s
            || (freq_difference <= -300))
    {
        freq_step = 30;
    }
    else//4.其它,1Hz/s
    {
        freq_step = 10;
    }

    atw.cycle[cycle].comp[comp].link.ctrl.fre_step = freq_step;
}

/************************************************************************
@name  	: CompControl
@brief 	: 
@param 	: None
@return	: None
@note   : 压缩机的控制,设置输出频率
*************************************************************************/
void CompControl(uint8_t cycle,uint8_t comp,uint16_t freq_set)//TODO add protect
{
    if (freq_set > 0)
    {
        atw.cycle[cycle].comp[comp].link.ctrl.cmd_set.bit.start = TRUE;
    }
    else atw.cycle[cycle].comp[comp].link.ctrl.cmd_set.bit.start = FALSE;

    atw.cycle[cycle].comp[comp].link.ctrl.freq_set = freq_set;  //freq out set
    CompSpeedStepSet(cycle,comp);//step set
}

/************************************************************************
@name  	: CompOilReturnControl
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2.3 cycle回油
            2)回油运行动作
                压缩机目标频率设置为Fi_max回油,不接收单元的分配运转,执行压缩机保护控制逻辑中的运转频率调整
*************************************************************************/
void CompOilReturnControl(uint8_t cycle,uint8_t comp)
{
    uint16_t rt_freq = atw.cycle[cycle].comp[comp].link.state.freq_output;
    uint16_t freq_set = atw.cycle[cycle].comp[comp].link.ctrl.freq_set;
    uint16_t Fi_max_oil = atw.cycle[cycle].comp[comp].local.Fi_c_oil;  //TODO 确定具体参数 

    if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)
    {
        Fi_max_oil = atw.cycle[cycle].comp[comp].local.Fi_h_oil;
    }

    if (time[cycle][comp].freq_set_wait == 0)
    {
        time[cycle][comp].freq_set_wait = 5;
        if (rt_freq < Fi_max_oil)
        {
            freq_set += 10;
        }
        else if (rt_freq > Fi_max_oil)
        {
            freq_set -= 20;
        }
    }
    CompControl(cycle,comp,freq_set);
}

/************************************************************************
@name  	: CompControl
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2.4压缩机的差压控制
            3)接收到允许除霜的指令后,进入cycle差压控制
                根据Pd和Ps间的压差进行调节(1barG=0.1MPa)
                a.高于15barG时,压缩机降频 3Hz/5s
                b.介于12barG与15barG,降频 2Hz/5s
                c.介于5barG与12barG,维持
                d.低于5barG,升频2Hz/5s
*************************************************************************/
void CompDifferPressureControl(uint8_t cycle,uint8_t comp)
{
    int16_t Pd = atw.cycle[cycle].comp[comp].local.Pd;
    int16_t Ps = atw.cycle[cycle].comp[comp].local.Ps;
    uint16_t rt_freq = atw.cycle[cycle].comp[comp].link.state.freq_output;
    uint16_t freq_set = rt_freq;
    int16_t differ;

    if (Pd > Ps)
    {
        differ = Pd - Ps;
    }
    else differ = Ps - Pd;
//
    if (time[cycle][comp].freq_set_wait == 0)
    {
        time[cycle][comp].freq_set_wait = 5;  //5s调节周期
        if (differ > 1500)//1.5MPa
        {
            freq_set = rt_freq - 30;
        }
        else if (differ > 1200)
        {
            freq_set = rt_freq - 20;
        }
        else if (differ > 500)
        {
            //维持
        }
        else
        {
            freq_set = rt_freq + 20;
        }
    }
//
    if (freq_set < atw.cycle[cycle].comp[comp].local.Fi_ini_frost)
    {
        freq_set = atw.cycle[cycle].comp[comp].local.Fi_ini_frost;
    }
    CompControl(cycle,comp,freq_set);
}

/************************************************************************
@name  	: CompDefrostRunControl
@brief 	: 
@param 	: None
@return	: None
@note   : 5.2.4压缩机的差压控制
            4)进入除霜运转动作
                除霜运转中的压缩机运转频率控制
                a.进入除霜状态后,压缩机运转频率为Fi_ini_frost
                b.之后,以1Hz/4s速度上升置40Hz
                c.此后以5s为周期,以Fi_frost作为目标频率,调节
                    当Ps<400kPa时,压缩机频率降1Hz,但不小于Fimin
                    当Ps>420kPa时,压缩机频率升2Hz,但不大于Fi_frost
                d.其它,维持当前运转频率
*************************************************************************/
void CompDefrostRunControl(uint8_t cycle,uint8_t comp)
{
    int16_t Ps = atw.cycle[cycle].comp[comp].local.Ps;
    uint16_t freq_set = atw.cycle[cycle].comp[comp].link.ctrl.freq_set;
    uint8_t  defrost_step = atw.cycle[cycle].comp[comp].local.defrost_step;
    uint16_t Fimin = atw.cycle[cycle].comp[comp].local.Fimin;

    switch (defrost_step)
    {
        case DEFROST_STATUS_INIT://初始频率设置
        {
            freq_set = atw.cycle[cycle].comp[comp].local.Fi_ini_frost; //进入时设置的频率
            defrost_step = DEFROST_STATUS_FREQ_UP;
        }
            break;

        case DEFROST_STATUS_FREQ_UP://频率上升   
        {
            if (freq_set < 400) //上升置40Hz,4s周期调节
            {
                if (time[cycle][comp].freq_set_wait == 0)
                {
                    time[cycle][comp].freq_set_wait = 4;  //4s period
                    freq_set += 10;
                }
            }
            else
            {
                defrost_step = DEFROST_STATUS_FREQ_FIX;
            }
        }
            break;

        case DEFROST_STATUS_FREQ_FIX://频率调节
        {
            if (time[cycle][comp].freq_set_wait == 0)
            {
                time[cycle][comp].freq_set_wait = 5;  //5s period
                if (Ps < 400)
                {
                    freq_set -= 10;
                }
                else if (Ps > 420)
                {
                    freq_set += 20;
                }
            //limit value judge
                if (freq_set < Fimin)
                {
                    freq_set = Fimin;
                }
                else if (freq_set > atw.cycle[cycle].comp[comp].local.Fi_frost)
                {
                    freq_set = atw.cycle[cycle].comp[comp].local.Fi_frost;
                }            
            }
        }
            break;
        default:
            break;
    }

    atw.cycle[cycle].comp[comp].local.defrost_step = defrost_step;
    CompControl(cycle,comp,freq_set);
}

/************************************************************************
@name  	: CompControlFromCycleStatus
@brief 	: 
@param 	: None
@return	: None
@note   : 根据cycle的迁移条件对压缩机的控制,设置输出频率
*************************************************************************/
static void CompSet3minsProtectStatus(uint8_t cycle,uint8_t comp)
{
    atw.cycle[cycle].comp[comp].local.work_step = COMP_3MINS_PROTECT;
    time[cycle][comp].protect_3mins = TM_3MINS_PROTECT; //3mins
}

/************************************************************************
@name  	: CompControlFromCycleStatus
@brief 	: 
@param 	: None
@return	: None
@note   : 根据cycle的迁移条件对压缩机的控制,设置输出频率
*************************************************************************/
void CompControlFromCycleStatus(uint8_t cycle,uint8_t comp)
{
    uint8_t last_step = atw.cycle[cycle].last_step;
    uint8_t work_step = atw.cycle[cycle].work_step;
    uint16_t freq_set = atw.cycle[cycle].comp[comp].local.set_freq;//单元设置的频率

    CompTimerCounter(cycle,comp);//计时用
    switch (work_step)
    {
        case CYCLE_STATUS_OFF://cycle关闭
        {
            switch (last_step)
            {
                case CYCLE_STATUS_START1:
                case CYCLE_STATUS_START2:
                case CYCLE_STATUS_RUN_HEAT:
                case CYCLE_STATUS_RUN_COOL: CompSet3minsProtectStatus(cycle,comp); break;
                default:    break;
            }
            CompControl(cycle,comp,0);//压缩机关闭
        }
            break;
        case CYCLE_STATUS_START1://cycle启动1
        {
            if (GetCycleRealRunMode(cycle) == CYCLE_MODE_COOL)//制冷
            {
                CompControl(cycle,comp,atw.cycle[cycle].comp[comp].local.Fi_c_ini);//制冷初始频率
            }
            else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)//制热
            {
                CompControl(cycle,comp,atw.cycle[cycle].comp[comp].local.Fi_h_ini);//制热初始频率
            }
        }
            break;
        case CYCLE_STATUS_START2://cycle启动2
        {
            //维持cycle1的频率
        }
            break;
        case CYCLE_STATUS_RUN_HEAT://cycle制热
        case CYCLE_STATUS_RUN_COOL://cycle制冷
        case CYCLE_STATUS_DEFROST_PREPARE://cycle除霜准备
        {
            CompControl(cycle,comp,freq_set);
        }
            break;
        case CYCLE_STATUS_RUN_OIL_RETURN://cycle回油运转
        {
            CompOilReturnControl(cycle,comp);
        }
            break;
        case CYCLE_STATUS_DIFFER_PRESSURE://cycle差压控制
        {
            CompDifferPressureControl(cycle,comp);//压缩机的差压控制
        }
            break;
        case CYCLE_STATUS_DEFROST_RUN://cycle除霜运转
        {
            if (last_step != CYCLE_STATUS_DEFROST_RUN)//进入时
            {
                atw.cycle[cycle].comp[comp].local.defrost_step = DEFROST_STATUS_INIT;
            }
            CompDefrostRunControl(cycle,comp);//
        }
            break;
        case CYCLE_STATUS_START_DEFROST_DONE://除霜后启动
        {
            CompControl(cycle,comp,310); //降频至31Hz
        }
            break;    
        
        default:
            break;
    }
}

/************************************************************************
@name  	: CompStatusControl
@brief 	: 
@param 	: None
@return	: None
@note   : comp的工作状态
*************************************************************************/
static void CompStatusSet(uint8_t cycle,uint8_t comp,uint8_t status)
{
    atw.cycle[cycle].comp[comp].local.work_step = status;
}

/************************************************************************
@name  	: CompStatusControl
@brief 	: 
@param 	: None
@return	: None
@note   : comp的工作状态
@note   : 1.压缩机不可以启动运转情形
            a.冷启动保护中
            b.3mins屏蔽保护状态
            c.保护原因停机,尚未退出保护状态
            d.压缩机处于锁定状态,譬如,压机在30mins发生4次启动故障  //故障状态
          2.冷启动保护(上电后首次启动)
            压缩机不可运转
          3.3mins屏蔽保护
            a.压缩机停止运转后,在3mins内,该压缩机不允许重新启动
            b.下列情况下,不需要; 处于除霜状态,包括除霜准备,差压控制,除霜运转,除霜后启动;或处于回油状态
*************************************************************************/
static void CompStatusControl(uint8_t cycle,uint8_t comp)
{
    switch (atw.cycle[cycle].comp[comp].local.work_step)
    {
        case COMP_INIT: //上电
        {
            if (1)//TODO add link connect
            {
                time[cycle][comp].pecharge_wait = TM_PRECHARGE;
                CompStatusSet(cycle,comp,COMP_COLD_START_CHECK);
                CompControl(cycle,comp,0);
            }
        }
            break;
        case COMP_COLD_START_CHECK: //冷启动检测
        {
            if(time[cycle][comp].pecharge_wait == 0)//预充电
            {
                atw.cycle[cycle].comp[comp].link.ctrl.cmd_set.bit.precharge_enable = TRUE;//预充电
                // CompStatusSet(cycle,comp,COMP_COLD_START_CHECK);
                if (CompCheckColdStartRequire(cycle,comp) == TRUE)//冷启动检测
                {
                    CompStatusSet(cycle,comp,COMP_RUN_COLD_START);
                }
                else 
                {
                    CompSet3minsProtectStatus(cycle,comp);
                }
            }
            CompControl(cycle,comp,0);
        }
            break;
        case COMP_RUN_COLD_START: //冷启动中
        {
            if (CompCheckColdStartQuit(cycle,comp) == TRUE)//冷启动退出条件
            {
                CompSet3minsProtectStatus(cycle,comp);
            }
            else CompControl(cycle,comp,0);
        }
            break;   
        case COMP_OFF_NORMAL://正常停止中
        {
            if (CheckCompAlarmState(cycle,comp) == FALSE)//无故障
            {
                if (GetCycleSetRunMode(cycle) != CYCLE_MODE_NULL)//收到开机指令
                {
                    CompStatusSet(cycle,comp,COMP_RUN_NORMAL);
                }
            }
            else CompControl(cycle,comp,0);
        }
            break;
        case COMP_RUN_NORMAL://正常运行中
        {
            CompControlFromCycleStatus(cycle,comp);
        }
            break;
        case COMP_3MINS_PROTECT: //3mins保护
        {
            if (time[cycle][comp].protect_3mins == 0)//3mins保护计时结束
            {
                CompStatusSet(cycle,comp,COMP_OFF_NORMAL);
            }
            else CompControl(cycle,comp,0);
        }
            break;
        default:
            break;
    }
}

/************************************************************************
@name  	: CompGetOptState
@brief 	: 
@param 	: None
@return	: None
@note   : comp的可运转状态判断
            7.1
            4)b不可启动运转情形
                a.处于冷保护状态
                b.处于3mins屏蔽保护
                c.压缩机有故障报警
                d.处于锁定
*************************************************************************/
static void CompGetOptState(uint8_t cycle,uint8_t comp)
{
    uint8_t comp_step = atw.cycle[cycle].comp[comp].local.work_step;

    if ((CheckCompAlarmState(cycle,comp) == TRUE)//有故障
        || ((comp_step == COMP_3MINS_PROTECT) || (comp_step == COMP_RUN_COLD_START))
        || (0))//TODO 锁定状态
    {
        atw.cycle[cycle].comp[comp].local.opt_state = FALSE;
    }
    else
    {
        atw.cycle[cycle].comp[comp].local.opt_state = TRUE;
    }
}

/************************************************************************
@name  	: CompGetLoadAddReduceState
@brief 	: 
@param 	: None
@return	: None
@note   : 可加载和不可加载状态
            1.回油中,不可加载和不可减载
*************************************************************************/
static void CompGetLoadAddReduceState(uint8_t cycle,uint8_t comp)
{

}

/************************************************************************
@name  	: CompStatusControl
@brief 	: 
@param 	: None
@return	: None
@note   : comp的工作状态
*************************************************************************/
void CompRunStatusControl(uint8_t cycle)
{
    uint8_t comp;

    for (comp = COMP_1; comp < COMP_NO_MAX; comp++)
    {
        CompStatusControl(cycle,comp);
        CompGetOptState(cycle,comp);
    }

}






