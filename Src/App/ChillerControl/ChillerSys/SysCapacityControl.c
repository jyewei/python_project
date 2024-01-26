/***********************************************************************
@file   : SysCapacityControl.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
@note   : 
************************************************************************/
#include "SysCapacityControl.h"
#include "Data.h"
#include "TimerCounter.h"
#include "SysStateControl.h"



enum
{
    D_TS_R1,     //range1
    D_TS_R2,
    D_TS_R3,
    D_TS_R4,
    D_TS_R5,
    D_TS_R6,
    D_TS_R7,

    D_TS_MAX
}DeltaTsRangeEnum;

enum
{
    D_TW_R1,     //range1
    D_TW_R2,
    D_TW_R3,
    D_TW_R4,
    D_TW_R5,
    D_TW_R6,
    D_TW_R7,

    D_TW_MAX
}DeltaTwRangeEnum;


enum
{
    N1,     
    N2,
    N3,
    N4,
    N5,
    N6,

    N_MAX
}DeltaTsRangeEnum;


#define TM_TW_REFRESH           ((uint16_t)40)  //水温采样周期
#define TM_CYCLE_START_INTERVAL ((uint16_t)60)  //cycle

//time
typedef struct
{
    uint16_t period_tw_refresh;         //水温采样周期
    uint16_t period_cap_ctrl;           //能调周期
    uint16_t cycle_start_interval;      //
    // uint16_t unit_cool_urgency_check[CHILLER_CH_MAX];
    // uint16_t unit_heat_urgency_check[CHILLER_CH_MAX];

}SysCapacityTimeType;



//capacity control parameters
typedef struct
{
    int16_t PI;

    int16_t Tw;             //水温
    int16_t Tw_last;        //上一次水温
    int16_t Tw_set;         //设置的水温
    int16_t DTw_h_set;      //制热能调时,目标水温修正值
    uint16_t Nt;            //N值,10倍值 //能调周期计算中用到的N值
    uint16_t Nt_last;       //上一次的N值
    uint16_t time_PI_up;    //PI>0的累计时间

//
    int32_t  sys_F_total_rating;    //
    int16_t  delta_F_total;


//系统频率为换算后的值
    uint32_t sys_set_freq;          //设置的总频率
    uint32_t sys_running_freq;      //运行中的系统总频率
    uint32_t sys_F_up_total;        //系统可加载频率
    uint32_t sys_F_down_total;      //系统可减载频率

    uint8_t  sys_running_unit;      //运行中的单元数量
    uint8_t  sys_add_unit;          //可加载的单元
    uint8_t  sys_reduce_unit;       //可减载的单元

    uint8_t  sys_running_cycle;     //运行中的cycle
    uint8_t  sys_close_able_cycle;  //可关闭的cycle
    uint8_t  sys_add_cycle;         //可加载的cycle
    uint8_t  sys_reduce_cycle;      //可减载的cycle

}SysCapacityParamType;



static SysCapacityTimeType time;
static SysCapacityParamType cap_ctrl;



//能调系数PI
const int16_t PI_tab[D_TW_MAX][D_TS_MAX] = 
{
    {-25,   -25,    0,      0,      25,     50,     100},
    {-50,   -25,    -25,    0,      0,      25,     50},
    {-100,  -50,    -25,    0,      0,      0,      25},
    {-200,  -100,   -50,    -25,    0,      0,      0},
    {-300,  -200,   -100,   -50,    -25,    0,      0},
    {-400,  -300,   -200,   -100,   -50,    0,      0},
    {-500,  -400,   -300,   -200,   -100,   -50,    0}
};


const int16_t N_tab[N_MAX][7] = 
{
    {2,  2,  2,  3,   3,   4,   4},
    {-1, -1, -1, -1,  -1,  -1,  -1},
    {2,  2,  2,  3,   3,   4,   4},
    {3,  3,  4,  5,   6,   7,   8},
    {4,  4,  6,  7,   9,   10,  12},
    {5,  6,  8,  10,  12,  14,  16}
};

// const uint16_t k_com_tab[COMP_TYPE_MAX] = {73,100,114,136};//频率换算系数


/************************************************************************
@name  	: SysCapacityControlTimerCounter
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void SysCapacityControlTimerCounter(void)
{
    uint16_t *pTime;
    uint16_t i,len;
    
    pTime = &time.period_tw_refresh;
    if (TimerCheck(NO_SYSCAPACITY_BASE) == TRUE)
    {
        len = sizeof(SysCapacityTimeType)/sizeof(uint16_t);
        TimerSet(NO_SYSCAPACITY_BASE,1); //base 1s
        for ( i = 0; i < len; i++)
        {
            if (*(pTime + i) != 0)
            {
                (*(pTime + i))--;
            }
        }

//PI>0 累计时间
        if (cap_ctrl.PI > 0)
        {
            cap_ctrl.time_PI_up++;
            if (cap_ctrl.time_PI_up >= 6000)//100mins
            {
                cap_ctrl.time_PI_up = 6000;
            }
        }
        else cap_ctrl.time_PI_up = 0;
    }
//Nt在系统停机时为1.5
    if (atw.sys.local.work_step == SYS_STATUS_OFF)//系统停机
    {
        cap_ctrl.Nt = 15;
        cap_ctrl.Nt_last = 15;
    }
    
}


/************************************************************************
@name  	: SysCapacityDTwhsetUpdate
@brief 	: ΔTw_h_set
@param 	: None
@return	: None
@note   : 3.2.1 制热目标水温修正值ΔTw_h_set
            制热能调时,制热目标水温修正值ΔTw_h_set
*************************************************************************/
static void SysCapacityDTwhsetUpdate(void)
{
    int16_t Tam = atw.sys.table.unit[CHILLER_0].io.Tam;
    uint8_t temp_type = atw.sys.table.unit[CHILLER_0].init.dsw1.bit.temp_type;
    int16_t MaxT;

    if (atw.sys.param.set.cell.heat_temp_chose == 0)//制热控制选择=0
    {
        if (temp_type == TEMP_SA)//标准热泵
        {
            if (Tam <= 0)//MaxT=max⁡(40,min⁡(45,70+Tam))
            {
                if (450 > (Tam + 700))  //
                {
                    MaxT = Tam + 700;
                }
                else MaxT = 450;
                
                if (MaxT < 400)
                {
                    MaxT = 400;
                }
            }
            else if (Tam <= 250)//MaxT_((回水))=min⁡(55,45+Tam)
            {
                if (550 > (450 + Tam))
                {
                    MaxT = 450 + Tam;
                }
                else MaxT = 550;
            }
            else//MaxT_((回水))=max⁡(50,80-Tam)
            {
                if (500 > (800 - Tam))
                {
                    MaxT = 500;
                }
                else MaxT = 800 - Tam;
            }
        }
        else//低温热泵
        {
            if (Tam <= 50)//MaxT_((回水))=max⁡(40,min⁡(55,75+Tam))
            {
                if (550 > (Tam + 750))  //
                {
                    MaxT = Tam + 750;
                }
                else MaxT = 550;
                
                if (MaxT < 400)
                {
                    MaxT = 400;
                }
            }
            else if (Tam <= 150)//MaxT_((回水))=min⁡(60,50+Tam)
            {
                if (600 > (500 + Tam))
                {
                    MaxT = 500 + Tam;
                }
                else MaxT = 600;
            }
            else//MaxT_((回水))=Max(50,90-Tam*2)
            {
                if (500 < (900 - Tam*2))
                {
                    MaxT = 900 - Tam*2;
                }
                else MaxT = 500;
            }
        }
        if (0 > (MaxT - cap_ctrl.Tw))
        {
            cap_ctrl.DTw_h_set = MaxT - cap_ctrl.Tw;
        }
        else cap_ctrl.DTw_h_set = 0;
        
    }
    else if (atw.sys.param.set.cell.heat_temp_chose == 1)//制热控制选择=1
    {
        if (temp_type == TEMP_SA)//标准热泵
        {
            if (Tam <= 0)//MaxT=max⁡(45,min⁡(50,70+Tam))
            {
                if (500 > (Tam + 700))  //
                {
                    MaxT = Tam + 700;
                }
                else MaxT = 500;
                
                if (MaxT < 450)
                {
                    MaxT = 450;
                }
            }
            else if (Tam <= 250)//MaxT_((回水))=min⁡(60,50+Tam)
            {
                if (600 > (500 + Tam))
                {
                    MaxT = 500 + Tam;
                }
                else MaxT = 600;
            }
            else//MaxT_((回水))=max⁡(55,85-Tam)
            {
                if (550 > (850 - Tam))
                {
                    MaxT = 500;
                }
                else MaxT = 850 - Tam;
            }
        }
        else//低温热泵
        {
            if (Tam <= 50)//MaxT_((回水))=max⁡(45,min⁡(60,80+Tam))
            {
                if (600 > (Tam + 800))  //
                {
                    MaxT = Tam + 800;
                }
                else MaxT = 600;
                
                if (MaxT < 450)
                {
                    MaxT = 450;
                }
            }
            else if (Tam <= 150)//MaxT_((回水))=min⁡(65,55+Tam)
            {
                if (650 > (550 + Tam))
                {
                    MaxT = 550 + Tam;
                }
                else MaxT = 650;
            }
            else//MaxT_((回水))=Max(55,95-Tam*2)
            {
                if (550 < (950 - Tam*2))
                {
                    MaxT = 995000 - Tam*2;
                }
                else MaxT = 550;
            }
        }
        if (0 > (MaxT - cap_ctrl.Tw))
        {
            cap_ctrl.DTw_h_set = MaxT - cap_ctrl.Tw;
        }
        else cap_ctrl.DTw_h_set = 0;
    }    
}

/************************************************************************
@name  	: SysCapacityNValueUpdate
@brief 	: N
@param 	: None
@return	: None
*************************************************************************/
static void SysCapacityNValueUpdate(int16_t *N_value)
{
    uint8_t select = 4; //TODO 可设置

    N_value[N1] = N_tab[N1][select];
    N_value[N2] = N_tab[N2][select];
    N_value[N3] = N_tab[N3][select];
    N_value[N4] = N_tab[N4][select];
    N_value[N5] = N_tab[N5][select];
    N_value[N6] = N_tab[N6][select];
}

/************************************************************************
@name  	: SysCapacityPIUpdate
@brief 	: PI
@param 	: None
@return	: None
*************************************************************************/
static void SysCapacityPIUpdate(void)
{
    int16_t D = atw.sys.param.temp_ctrl_fix;
    int16_t N_value[N_MAX];
    int16_t DTs,DTw;
    uint8_t RTs,RTw;

    if (GetSysSetRunMode() == SYS_MODE_COOL)
    {
        DTs = cap_ctrl.Tw - cap_ctrl.Tw_set;
        DTw = cap_ctrl.Tw - cap_ctrl.Tw_last;
    }
    else if (GetSysSetRunMode() == SYS_MODE_HEAT)
    {
        DTs = cap_ctrl.Tw_set - cap_ctrl.Tw;
        DTw = cap_ctrl.Tw_last - cap_ctrl.Tw;
    }

    SysCapacityNValueUpdate(N_value);
//get RTw
    if (DTw >= N_value[N1])
    {
        RTw = D_TW_R1;
    }
    else if (DTw >= N_value[N2])
    {
        RTw = D_TW_R2;
    }
    else if (DTw >= N_value[N3])
    {
        RTw = D_TW_R3;
    }
    else if (DTw >= N_value[N4])
    {
        RTw = D_TW_R4;
    }
    else if (DTw >= N_value[N5])
    {
        RTw = D_TW_R5;
    }
    else if (DTw >= N_value[N6])
    {
        RTw = D_TW_R6;
    }
    else
    {
        RTw = D_TW_R7;
    }
    
//get RTs
    if (DTs < (-D-10))
    {
        RTs = D_TS_R1;
    }
    else if (DTs < (-D))
    {
        RTs = D_TS_R2;
    }
    else if (DTs < (-D/2))
    {
        RTs = D_TS_R3;
    }
    else if (DTs < 0)
    {
        RTs = D_TS_R4;
    }
    else if (DTs < (D/2))
    {
        RTs = D_TS_R5;
    }
    else if (DTs < (D+20))
    {
        RTs = D_TS_R6;
    }
    else
    {
        RTs = D_TS_R7;
    }

    cap_ctrl.PI = PI_tab[RTw][RTs];
}

/************************************************************************
@name  	: SysCapacityNtUpdate
@brief 	: 能调周期计算中N值的更新Z
@param 	: None
@return	: None
*************************************************************************/
static uint8_t SysCapacityCheckCompRunTime(void)
{
    uint8_t i,j;

    for (i = CHILLER_0; i < CHILLER_CH_MAX; i++)
    {
        for (j = CYCLE_1; j < CYCLE_CH_MAX; j++)
        {
            if (atw.sys.table.unit[i].cycle[j].io.comp_rt_freq > 0)//运行中
            {
                if (atw.sys.local.unit[i].cycle[j].comp_run_time <= 10 )
                {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

/************************************************************************
@name  	: SysCapacityNtUpdate
@brief 	: 能调周期计算中N值的更新
@param 	: None
@return	: None
*************************************************************************/
static uint16_t SysCapacityNtUpdate(void)
{
    int16_t DTs = cap_ctrl.Tw - cap_ctrl.Tw_set;

//1.系统内出现任一单元的cycle运转时间<=10mins,下一能调周期N=Ni-1 + 1(每个能调周期最多加一次),N=[1.5,3.5]    
    if (SysCapacityCheckCompRunTime() == TRUE)
    {
        cap_ctrl.Nt = cap_ctrl.Nt_last + 10;
        if (cap_ctrl.Nt < 15)
        {
            cap_ctrl.Nt = 15;
        }
        else if (cap_ctrl.Nt > 35)
        {
            cap_ctrl.Nt = 35;
        }
    }
//2.系统连续10mins处于升档(PI>0),下一个能调周期N=N(i-1) -1    
    else if (cap_ctrl.time_PI_up > 600)//10mins/600s
	{
        cap_ctrl.Nt = cap_ctrl.Nt_last -1;
        if (cap_ctrl.Nt < 15) //TODO 待确认
        {
            cap_ctrl.Nt = 15;
        }
        else if (cap_ctrl.Nt > 35)
        {
            cap_ctrl.Nt = 35;
        }
	}
}

/************************************************************************
@name  	: SysCapacityPeroidUpdate
@brief 	: 能调周期
@param 	: None
@return	: None
*************************************************************************/
static uint16_t SysCapacityPeroidUpdate(void)
{
    int16_t  D = atw.sys.param.temp_ctrl_fix;
    uint16_t Time_set = atw.sys.param.period_temp_ctrl;
    int16_t  delta_Ts;
    uint16_t T;
    
    if (GetSysSetRunMode() == SYS_MODE_COOL)
    {
        delta_Ts = cap_ctrl.Tw - cap_ctrl.Tw_set;
    }
    else if (GetSysSetRunMode() == SYS_MODE_HEAT)
    {
        SysCapacityDTwhsetUpdate();
        delta_Ts = cap_ctrl.Tw_set + cap_ctrl.DTw_h_set - cap_ctrl.Tw;
    }
    
//1.当前控制水温与设定水温差值ΔTs>=D+2
    if (delta_Ts >= (D + 20))
    {
        T = Time_set;
    }
//2.  0 =< ΔTs < D+2    
    else if (delta_Ts >= 0)
    {
        if (cap_ctrl.PI <= 0)
        {
            T = Time_set;
        }
        else
        {
            SysCapacityNtUpdate();
            T = Time_set*cap_ctrl.Nt/10; //Nt为10倍值
        }
    }
//3.  -D =< ΔTs < 0    
    else if (delta_Ts >= -D)//3.
    {
        T = Time_set;
    }
//4.     ΔTs < -D
    else
    {
        T = Time_set/2;
    }

    return T;
}

// /************************************************************************
// @name  	: SysCapacityUnitUrgencyCheck
// @brief 	: 单元急停区的检测
// @param 	: None
// @return	: None
// *************************************************************************/
// static uint8_t SysCapacityUnitUrgencyCheck(uint8_t unit)
// {
//     uint8_t res = FALSE;
//     uint8_t refri_type = atw.sys.table.unit[CHILLER_0].init.dsw1.bit.refrigent_type;
//     int16_t T_temp_low = atw.sys.param.temp_out_water_too_low;
//     int16_t T_temp_high = atw.sys.param.temp_out_water_too_high;

//     if (GetSysRealRunMode() == SYS_MODE_COOL)
//     {
//         if (atw.sys.table.unit[unit].io.Tw_out < (T_temp_low + 10))
//         {
//             if (time.unit_cool_urgency_check[unit] == 0)
//             {
//                 res = TRUE;
//             }
//         }
//         else time.unit_cool_urgency_check[unit] = 10;
//     }
//     else if (GetSysRealRunMode() == SYS_MODE_HEAT)
//     {
//         if (atw.sys.table.unit[unit].io.Tw_out < (T_temp_low + 10))
//         {
//             if (time.unit_heat_urgency_check[unit] == 0)
//             {
//                 res = TRUE;
//             }
//         }
//         else time.unit_heat_urgency_check[unit] = 10;
//     }

//     return res;
// }

/************************************************************************
@name  	: SysCapacitySysUrgencyCheck
@brief 	: 系统急停区的检测
@param 	: None
@return	: None
*************************************************************************/
static uint8_t SysCapacitySysUrgencyCheck(void)
{
    uint8_t res = FALSE;
    uint8_t refri_type = atw.sys.table.unit[CHILLER_0].init.dsw1.bit.refrigent_type;
    int16_t T_temp_low = atw.sys.param.temp_out_water_too_low;
    int16_t T_temp_high = atw.sys.param.temp_out_water_too_high;
    int16_t D = atw.sys.param.temp_ctrl_fix;

    if (refri_type == ADD_ANTIFREEZE)
    {
        T_temp_low = atw.sys.param.temp_out_water_refri_too_low;
    }

    if (GetSysRealRunMode() == SYS_MODE_COOL)
    {
        if (atw.sys.param.set.cell.cool_temp_chose == 0)//系统回水
        {
            if (((cap_ctrl.Tw - cap_ctrl.Tw_set) < (-D -30))
                || (atw.sys.local.Tw_sys_in < (T_temp_low + 40)))
            {
                res = TRUE;
            }
        }
        else if (atw.sys.param.set.cell.cool_temp_chose == 1)//系统出水
        {
            if (((cap_ctrl.Tw - cap_ctrl.Tw_set) < (-D -30))
                || (atw.sys.local.Tw_sys_out < (T_temp_low + 10)))
            {
                res = TRUE;
            }
        }
    }
    else if (GetSysRealRunMode() == SYS_MODE_HEAT)
    {
        if (atw.sys.param.set.cell.heat_temp_chose == 0)//系统回水
        {
            if (((cap_ctrl.Tw_set - cap_ctrl.Tw) < (-D -30))
                || (atw.sys.local.Tw_sys_in > (T_temp_high - 50)))
            {
                res = TRUE;
            }
        }
        else if (atw.sys.param.set.cell.heat_temp_chose == 1)//系统出水
        {
            if (((cap_ctrl.Tw_set - cap_ctrl.Tw) < (-D -30))
                || (atw.sys.local.Tw_sys_out > (T_temp_high - 10)))
            {
                res = TRUE;
            }
        }
    }

    return res;
}


/************************************************************************
@name  	: SysCapacityGetUnitOptNo
@brief 	: 获取单元的可运转数量
@param 	: None
@return	: None
*************************************************************************/
static uint8_t SysCapacityGetUnitOptNo(void)
{
    uint8_t ch;
    uint8_t cnt = 0;

    for (ch = CHILLER_0; ch < CHILLER_CH_MAX; ch++)
    {
        if ((atw.sys.table.unit[ch].event.state.bit.unit_opt_state == TRUE) //单元可运行
            && (atw.sys.local.unit[ch].link_state == TRUE))//通信连接正常
        {
            cnt++;
        }
    }
    return cnt;
}

/************************************************************************
@name  	: SysCapacityGetUnitRunningNo
@brief 	: 获取单元的运转中数量
@param 	: None
@return	: None
*************************************************************************/
static uint8_t SysCapacityGetUnitRunningNo(void)
{
    uint8_t ch;
    uint8_t cnt = 0;

    for (ch = CHILLER_0; ch < CHILLER_CH_MAX; ch++)
    {
        if ((atw.sys.table.unit[ch].event.state.bit.unit_run_state == TRUE) //单元运行中
            && (atw.sys.local.unit[ch].link_state == TRUE))//通信连接正常
        {
            cnt++;
        }
    }
    return cnt;
}

/************************************************************************
@name  	: SysCapacityGetCycleTotalRunTimeShortest
@brief 	: 获取单元中cycle累计时间最短的单元号
@param 	: None
@return	: None
*************************************************************************/
static uint8_t SysCapacityGetCycleTotalRunTimeShortest(void)
{
    uint8_t ch,cycle;
    uint8_t unit = 0;
    uint32_t run_time = atw.sys.table.unit[CHILLER_0].cycle[cycle].init_sum.comp_run_time_sum;

    for (ch = CHILLER_0; ch < CHILLER_CH_MAX; ch++)
    {
        if (((atw.sys.local.unit[ch].link_state == TRUE))//通信连接正常
            && (atw.sys.table.unit[ch].event.cycle_cnt.bit.operate > 0)//cycle可运转数量 > 0
            && (atw.sys.table.unit[ch].event.state.bit.unit_opt_state == TRUE))//单元可运转
        {
            for ( cycle = 0; cycle < atw.sys.local.unit[ch].cycle_active_cnt; cycle++)
            {
                if ((atw.sys.table.unit[ch].cycle[cycle].init_sum.comp_run_time_sum < run_time)//累计运转时间判断
                    && (atw.sys.table.unit[ch].cycle[cycle].event.state1.bit.opt_state == TRUE))//cycle可运转
                {
                    unit = ch;
                    run_time = atw.sys.table.unit[ch].cycle[cycle].init_sum.comp_run_time_sum;
                }
            }
        }
    }
    return unit;
}

/************************************************************************
@name  	: SysCapacityGetCycleTotalRunTimeLongest
@brief 	: 获取单元中cycle累计时间最长的单元号
@param 	: None
@return	: None
*************************************************************************/
static uint8_t SysCapacityGetCycleTotalRunTimeLongest(void)//TODO 待确认,累计运行时间最长
{
    uint8_t ch,cycle;
    uint8_t unit = 0;
    uint32_t run_time = atw.sys.table.unit[CHILLER_0].cycle[cycle].init_sum.comp_run_time_sum;

    for (ch = CHILLER_0; ch < CHILLER_CH_MAX; ch++)
    {
        if (((atw.sys.local.unit[ch].link_state == TRUE))//通信连接正常
            // && (atw.sys.table.unit[ch].event.cycle_cnt.bit.operate > 0)//cycle可运转数量 > 0
            && (atw.sys.table.unit[ch].event.state.bit.unit_run_state == TRUE))//单元运行中
        {
            for ( cycle = 0; cycle < atw.sys.local.unit[ch].cycle_active_cnt; cycle++)
            {
                if ((atw.sys.table.unit[ch].cycle[cycle].init_sum.comp_run_time_sum > run_time))//累计运转时间判断
                    // && (atw.sys.table.unit[ch].cycle[cycle].event.state1.bit.opt_state == TRUE))//cycle可运转
                {
                    unit = ch;
                    run_time = atw.sys.table.unit[ch].cycle[cycle].init_sum.comp_run_time_sum;
                }
            }
        }
    }
    return unit;
}

// /************************************************************************
// @name  	: SysCapacityGetCycleTotalRunTimeShortest
// @brief 	: 获取单元中cycle累计时间最短的单元号
// @param 	: None
// @return	: None
// *************************************************************************/
// static uint16_t FrunToFirunChange(uint8_t unit,uint16_t F_run)
// {
//     uint32_t Fi_run;
//     int8_t comp_type = atw.sys.local.unit[unit].comp_type;
    
//     Fi_run = F_run *100 / k_com_tab[comp_type];

//     return ((uint16_t)Fi_run);
// }

// /************************************************************************
// @name  	: SysCapacityGetCycleTotalRunTimeShortest
// @brief 	: 获取单元中cycle累计时间最短的单元号
// @param 	: None
// @return	: None
// *************************************************************************/
// static uint16_t FirunToFrunChange(uint8_t unit,uint16_t Fi_run)
// {
//     uint32_t F_run;
//     uint8_t comp_type = atw.sys.local.unit[unit].comp_type;
    
//     F_run = Fi_run * k_com_tab[comp_type]/100;

//     return ((uint16_t)F_run);
// }

/************************************************************************
@name  	: SysCapacityFtotalRatingUpdate
@brief 	: F_total_rating
@param 	: None
@return	: None
*************************************************************************/
static void SysCapacityFtotalRatingUpdate(void)
{
    uint32_t freq_sum = 0;
    uint8_t unit;

    for (unit = CHILLER_0; unit < CHILLER_CH_MAX; unit++)
    {
        if (atw.sys.local.unit[unit].active == TRUE)//unit active //if not active,is 0
        {        
            freq_sum += atw.sys.table.unit[unit].event.comp_Fi_rating;
        }
    }

    cap_ctrl.sys_F_total_rating = freq_sum;
}

/************************************************************************
@name  	: SysCapacityDeltaFtotalUpdate
@brief 	: △F_total
@param 	: None
@return	: None
*************************************************************************/
static void SysCapacityDeltaFtotalUpdate(void)
{
    int32_t delta = 0,temp;

    temp = atw.sys.local.unit_active_cnt*5;
    if (temp < 30)
    {
        temp = 30;
    }
    else if (temp > 60)
    {
        temp = 60;
    }    
    delta = cap_ctrl.sys_F_total_rating * cap_ctrl.PI;
    if (delta > temp)
    {
        delta = temp;
    }
    cap_ctrl.delta_F_total = delta;
}

/************************************************************************
@name  	: SysCapacityFuptotalUpdate
@brief 	: △F_up_total
@param 	: None
@return	: None
*************************************************************************/
static void SysCapacityFuptotalUpdate(void)
{
    uint8_t unit;
    uint32_t total = 0;

    for (unit = CHILLER_0; unit < CHILLER_CH_MAX; unit++)
    {
        if (atw.sys.local.unit[unit].active == TRUE)//unit active //if not active,is 0
        {     
            total += atw.sys.table.unit[unit].event.comp_Fi_up;
        }
    }
    
    cap_ctrl.sys_F_up_total = total;
}

/************************************************************************
@name  	: SysCapacityFdowntotalUpdate
@brief 	: △F_down_total
@param 	: None
@return	: None
*************************************************************************/
static void SysCapacityFdowntotalUpdate(void)
{
    uint8_t unit;
    uint32_t total = 0;

    for (unit = CHILLER_0; unit < CHILLER_CH_MAX; unit++)
    {
        if (atw.sys.local.unit[unit].active == TRUE)//unit active //if not active,is 0
        {     
            total += atw.sys.table.unit[unit].event.comp_Fi_down;
        }
    }
    
    cap_ctrl.sys_F_down_total = total;
}

/************************************************************************
@name  	: SysCapacitySetUnitSwitch
@brief 	: 设置单元开关状态
@param 	: ch:单元;state:ON/OFF;cycle_cnt:开启的cycle数量,freq_set:设置的单元频率
@return	: None
*************************************************************************/
static void SysCapacitySetUnitSwitch(uint8_t unit,uint8_t state,uint8_t cycle_cnt,uint16_t freq_set)
{
    if (state == ON)
    {
        atw.sys.table.unit[unit].cmd.data1.bit.unit_run = TRUE;
        atw.sys.table.unit[unit].cmd.unit_cycle_add = cycle_cnt;
        atw.sys.table.unit[unit].cmd.unit_freq_add = freq_set;
    }
    else
    {
        atw.sys.table.unit[unit].cmd.data1.bit.unit_run = FALSE;
        atw.sys.table.unit[unit].cmd.unit_cycle_add = 0;
        atw.sys.table.unit[unit].cmd.unit_freq_add = 0;
    }
}

/************************************************************************
@name  	: SysCapacityUnitOptcntUpdate
@brief 	: 可运转的单元
@param 	: None
@return	: None
*************************************************************************/
static uint8_t SysCapacityUnitOptcntUpdate(void)
{
    uint8_t unit,cnt = 0;

    for (unit = CHILLER_0; unit < CHILLER_CH_MAX; unit++)
    {
        if (atw.sys.table.unit[unit].event.state.bit.unit_opt_state == TRUE)//unit opt state
        {     
            cnt ++;
        }
    }
    
    cap_ctrl.sys_add_unit = cnt;
    return cap_ctrl.sys_add_unit;
}

/************************************************************************
@name  	: SysCapacityFup
@brief 	: 系统加载
@param 	: None
@return	: None
@note   : 6.3.5 系统能调分配
            2-2)单元加减载
                a)单元加减载及单元中压缩机运转频率分配
                    (1)单元加载条件
*************************************************************************/
static void SysCapacityFup(void)
{
    uint8_t unit;
    int8_t comp_type = atw.sys.local.unit[unit].comp_type;
    int16_t F_up = 0,k;

    SysCapacityFuptotalUpdate();    //△F_up_total
    if ((SysCapacityUnitOptcntUpdate() > 0) //有可加载的单元
        && ((cap_ctrl.delta_F_total > 100) && (cap_ctrl.sys_F_up_total < 100))//且△F_total>10Hz，且, △F_up_total<10Hz
        && (time.cycle_start_interval == 0))//且距离上一次启动单元cycle的时间间隔大于60s
    {
    //单元加载
        F_up = 300;
        time.cycle_start_interval = TM_CYCLE_START_INTERVAL;
        unit = SysCapacityGetCycleTotalRunTimeShortest();//加载一台unit
        SysCapacitySetUnitSwitch(unit,ON,1,0);//TODO
    }

    if ((cap_ctrl.delta_F_total - F_up) < 0)
    {
        k = 0;
    }
    else k = cap_ctrl.delta_F_total - F_up;

//计算每个运转单元中压缩机频率的加载量   
    for (unit = CHILLER_0; unit < CHILLER_CH_MAX; unit++)
    {
        if (atw.sys.table.unit[unit].cmd.data1.bit.unit_run == TRUE)//unit设置为ON
        {     
            comp_type = atw.sys.local.unit[unit].comp_type;
            atw.sys.table.unit[unit].cmd.unit_freq_add = k*atw.sys.table.unit[unit].event.comp_Fi_up/100/cap_ctrl.sys_F_up_total;
        }
    }
}

/************************************************************************
@name  	: SysCapacityFdown
@brief 	: 系统减载
@param 	: None
@return	: None
@note   : 6.3.5 系统能调分配
            2-2)单元加减载
                b)单元加减载及单元中压缩机运转频率分配
                    (1)单元减载条件
*************************************************************************/
static void SysCapacityFdown(void)//TODO 是否可以全关
{
    uint8_t unit,i;
    int8_t comp_type = atw.sys.local.unit[unit].comp_type;
    int16_t F_down = 0,n;

    SysCapacityFdowntotalUpdate();//△F_down_total
    if (cap_ctrl.delta_F_total > cap_ctrl.sys_F_down_total)
    {
        n = cap_ctrl.delta_F_total / cap_ctrl.sys_F_down_total;//
        F_down = -30*n;

        while (n)//TODO 单元减载条件
        {
            unit = SysCapacityGetCycleTotalRunTimeLongest();
            SysCapacitySetUnitSwitch(unit,OFF,0,0);//关闭
            n--;
        }
    }
    //每个单元的减载量(运转中)
    for (unit = CHILLER_0; unit < CHILLER_CH_MAX; unit++)
    {
        if (atw.sys.table.unit[unit].cmd.data1.bit.unit_run == TRUE)//unit设置为ON
        {     
            comp_type = atw.sys.local.unit[unit].comp_type;
            atw.sys.table.unit[unit].cmd.unit_freq_add = atw.sys.table.unit[unit].event.comp_Fi_up/100/cap_ctrl.sys_F_up_total;
        }
    }
}

/************************************************************************
@name  	: SysCapacityControl
@brief 	: 系统能调控制
@param 	: None
@return	: None
@note   : 6.3.3 系统加载,减载单元的原则
          加载
            1.每个能调周期最多只能加载一个cycle
            2.优先向已有cycle运转,且还存在可运转cycle的单元发送加载指令
            3.选择新单元时,该单元中需有可运转的cycle,累计运转时间短的cycle优先,后发送加载指令
          减载
            1.每个能调周期可减载多个cycle
            2.只对可减载的cycle单元进行减载
            3.优先向已发生过cycle减载,且该单元中还有可减载cycle的单元发送减载指令
            4.减载时,单元需要有可减载cycle,按运转时间排序,优先减载cycle运转时间长的,多个cycle减载,优先减载同一个单元的cycle
            5.故障或急停时,直接将运转的cycle减载,该cycle需要满足可减载条件
*************************************************************************/
void SysCapacityControl(void)//TODO  待添加开多台,实际频率参与计算
{
    uint8_t unit;
    // uint16_t freq_set = 0;

    if (time.period_cap_ctrl == 0)
    {
    //能调周期的更新    
        if (SysCapacitySysUrgencyCheck() == FALSE)
        {
            time.period_cap_ctrl = SysCapacityPeroidUpdate();
        }
        else time.period_cap_ctrl = 0;
    //加减载的计算
        if (SysCapacityGetUnitRunningNo() == 0)//无运转中的单元 //6.3.5 1)开机时,启动一台单元,目标频率为60/k_comHz
        {
            unit = SysCapacityGetCycleTotalRunTimeShortest();
            // freq_set = FrunToFirunChange(unit,600);
            SysCapacitySetUnitSwitch(unit,ON,1,600);
        }
        else//系统制冷/制热运转中
        {
            SysCapacityPIUpdate();          //get PI
            SysCapacityFtotalRatingUpdate();//F_total_rating
            SysCapacityDeltaFtotalUpdate(); //△F_total
            
            // SysCapacityFdowntotalUpdate();  //△F_down_total
            if (cap_ctrl.delta_F_total >= 50)
            {
                SysCapacityFup();//加载
            }
            else if (cap_ctrl.delta_F_total <= -50)
            {
                SysCapacityFdown();//减载
            }
        }
    }
}
