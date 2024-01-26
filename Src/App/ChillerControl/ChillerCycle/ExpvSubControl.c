/***********************************************************************
@file   : ExpvSubControl.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "ExpvSubControl.h"
#include "data.h"
#include "ChillerCycle.h"
#include "config.h"
#include "CycleStateControl.h"
#include "TimerCounter.h"





#define TM_EXPV3_PERIOD     ((uint8_t)20)   //EXV3的调节周期
#define TM_INIT_STEP_KEEP   ((uint8_t)60)   //EXV3初始开度保持时间


//time
typedef struct
{
    uint8_t period_adjust;      //EXV3调节周期
    uint8_t t_sub_period;       //1s取样周期
    uint8_t t_sub_calculate;    //20s计算周期
    uint8_t init_step_keep;     //初始开度保持时间
    uint8_t run_200s;           //200s运行时间计时
    uint8_t op_protect_fix;     //高排温开度修正

}ExpvSubTimeType;

static ExpvSubTimeType time[CYCLE_CH_MAX];



/************************************************************************
@name  	: ExpvSubTimerCounter
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ExpvSubTimerCounter(uint8_t cycle)//
{
    uint8_t *pTime;
    uint16_t i,len;
//decrease
    pTime = &time[cycle].period_adjust;
    if (TimerCheck(NO_CYCLE1_EXPVSUB_BASE+cycle) == TRUE)
    {
        len = sizeof(ExpvSubTimeType)/sizeof(uint8_t);
        TimerSet(NO_CYCLE1_EXPVSUB_BASE+cycle,1); //base 1s
        for ( i = 0; i < len; i++)
        {
            if (*(pTime + i) != 0)
            {
                (*(pTime + i))--;
            }
        }
    }
//
    if (atw.cycle[cycle].expv[EXPV3].rt_exv_step == 0 )
    {
        time[cycle].run_200s = 200;
    }
        

}

/************************************************************************
@name  	: ExpvMainControl
@brief 	: 主膨胀阀步数设置
@param 	: 
@return	: None
@note   : EXPV1/EXPV2 的步数控制
*************************************************************************/
static void ExpvSubControl(uint8_t cycle,uint8_t expv,uint16_t step)
{
    uint8_t cycle_mode = GetCycleRealRunMode(cycle);
    uint16_t exv_min_c = atw.cycle[cycle].expv[expv].exv_min_c;
    uint16_t exv_min_h = atw.cycle[cycle].expv[expv].exv_min_h;
    uint16_t exv_max_c = atw.cycle[cycle].expv[expv].exv_max_c;
    uint16_t exv_max_h = atw.cycle[cycle].expv[expv].exv_max_h;

    if (step > 0)
    {
        if (cycle_mode == CYCLE_MODE_COOL)
        {
            if (step < exv_min_c)
            {
                step = exv_min_c;
            }
            else if (step > exv_max_c)
            {
                step = exv_max_c;
            }
        }
        else if (cycle_mode == CYCLE_MODE_HEAT)
        {
            if (step < exv_min_h)
            {
                step = exv_min_h;
            }
            else if (step > exv_max_h)
            {
                step = exv_max_h;
            }
        }
    }
    atw.cycle[cycle].expv[expv].set_exv_step = step;
}


/************************************************************************
@name  	: ExpvSubGetStepParam
@brief 	: 经济器膨胀阀的控制
@param 	: 
@return	: None
@note   : 7.3.4 经济器电子膨胀阀EXV3控制
            获取启动初始步数,最大开度,正常调节时最小步数
*************************************************************************/
static ExpvSubGetStepParam(uint8_t cycle)
{
    atw.cycle[cycle].expv[EXPV3].exv_Ini_c = 40;
    atw.cycle[cycle].expv[EXPV3].exv_Ini_h = 40;
    atw.cycle[cycle].expv[EXPV3].exv_Ini_defrost = 0;

    atw.cycle[cycle].expv[EXPV3].exv_max_c = 200;
    atw.cycle[cycle].expv[EXPV3].exv_max_h = 200;
    atw.cycle[cycle].expv[EXPV3].exv_max_defrost = 0;

    atw.cycle[cycle].expv[EXPV3].exv_min_c = 35;
    atw.cycle[cycle].expv[EXPV3].exv_min_h = 35;
    atw.cycle[cycle].expv[EXPV3].exv_min_defrost = 0;
}



// /************************************************************************
// @name  	: ExpvSubGetSHsub
// @brief 	: SH_sub
// @param 	: 
// @return	: None
// @note   : 7.3.4 经济器电子膨胀阀EXV3控制
//             (1)参数定义
//                 经济器辅路制冷剂出口的过热度SH_sub = Tsub_out - Tsub_in(Tsub_out,Tsub_in取20s平均值)
//                 压缩机排气过热度 SH_Td = Td - Tc
//                 目标过冷器过热度 SH_sub_tar = [经济器目标过热度](默认5,可设)
// *************************************************************************/
// static int16_t ExpvSubGetSHsub(uint8_t cycle)
// {
//     int16_t SH_sub = atw.cycle[cycle].io.SH_sub;
//     static int32_t SH_sub_sum = 0;
//     static int16_t SH_sub_average,cnt;

//     if (time[cycle].t_sub_period == 0)
//     {
//         time[cycle].t_sub_period = 1;//TODO 暂定1s取样周期
//         SH_sub_sum += SH_sub;
//         cnt++;
//     }

//     if (time[cycle].t_sub_calculate == 0)//计算周期20s
//     {
//         time[cycle].t_sub_calculate = 20;
//         if (cnt)
//         {
//             SH_sub_average = SH_sub_sum/cnt;
//         }
//         else SH_sub_average = SH_sub;
//         cnt = 0;
//         SH_sub_sum = 0;
//     }
//     return SH_sub_average;
// }


/************************************************************************
@name  	: ExpvSubGetSHsubtar
@brief 	: SH_sub_tar
@param 	: 
@return	: None
@note   : 7.3.4 经济器电子膨胀阀EXV3控制
            (1)参数定义
                经济器辅路制冷剂出口的过热度SH_sub = Tsub_out - Tsub_in(Tsub_out,Tsub_in取20s平均值)
                压缩机排气过热度 SH_Td = Td - Tc
                目标过冷器过热度 SH_sub_tar = [经济器目标过热度](默认5,可设)
*************************************************************************/
static int16_t ExpvSubGetSHsubtar(uint8_t cycle)
{
    return (atw.unit.table.sys_cmd.SH_sub_tar);
}

/************************************************************************
@name  	: ExpvSubTestModeControl
@brief 	: 
@param 	: 
@return	: None
@note   : 7.3.4 经济器电子膨胀阀EXV3控制
            (4)cycle正常运转阶段
                测试模式下,根据EEPROM设定,若EXV3>0,则开启该阀到初始开度,此后开到设定值 //TODO eeprom什么设定??? 
*************************************************************************/
static void ExpvSubTestModeControl(uint8_t cycle)
{

}

/************************************************************************
@name  	: ExpvSubTestModeControl
@brief 	: 
@param 	: 
@return	: None
@note   : 7.3.4 经济器电子膨胀阀EXV3控制
            (4)cycle正常运转阶段
                非测试模式下,当EXV3=0时,满足一下任一条件,开启EXV3到初始步数,否则关闭
                条件1:  Fi_run>= 40Hz;
                        且压缩机运转超5mins;
                        且Td>=Tc+18,且Td(n)>=Td(n-1),或Td>=Tc+20
                        且该系统内变频压缩机电流I_inv<=Ku2-3.5(待定义)
*************************************************************************/
static uint8_t ExpvSubCheckOpenState(uint8_t cycle)
{
    uint8_t res = FALSE;
    uint8_t capacity = atw.unit.table.init.dsw1.bit.capacity;
    uint16_t Fi_run = atw.cycle[cycle].comp[COMP_1].link.state.freq_output;
    int16_t Td = atw.cycle[cycle].comp[COMP_1].local.Td;
    int16_t Td_last = atw.cycle[cycle].comp[COMP_1].local.Td_last;
    int16_t Tc = atw.cycle[cycle].comp[COMP_1].local.Tc;
    uint32_t comp_run_time = atw.cycle[cycle].comp[COMP_1].local.current_run_time;
    uint16_t I_inv =  atw.cycle[cycle].comp[COMP_1].link.state.current;//
    uint16_t Ku2 = 2000;//TODO 待定义

//get Ku2
    switch (capacity)//TODO 
    {
        case CAPACITY_45KW:  Ku2 = 1900;   break;
        case CAPACITY_65KW:  Ku2 = 1900;   break;
        case CAPACITY_130KW: Ku2 = 1900;   break;
        default:  break;
    } 

    if (I_inv <= (Ku2-350))
    {
        if ((Fi_run >= 400)         //运转频率>=40Hz
            && (comp_run_time > 5)  //压缩机运转超5mins
            && (((Td >= Tc +180) && (Td >= Td_last)) || (Td >= Tc + 200)))
        {
            res = TRUE;
        }
        else if ((Fi_run >= 300)
                && (Td > 980))
        {
            res = TRUE;
        }
    }
    return res;
}

/************************************************************************
@name  	: ExpvSubTestModeControl
@brief 	: 
@param 	: 
@return	: None
@note   : 7.3.4 经济器电子膨胀阀EXV3控制
            (4)cycle正常运转阶段
                非测试模式下,满足以下任一条件,强制关闭
                    1.cycle内压缩机off,或压缩机频率Fi_run <= 30Hz
                    2.压缩机排气温度Td<=Tc+15
                    3.该系统变频压缩机电流I_inv>=Ku2-1.0(待定义) //电流0.01A
*************************************************************************/
static uint8_t ExpvSubCheckCloseState(uint8_t cycle)
{
    uint8_t res = FALSE;
    uint8_t capacity = atw.unit.table.init.dsw1.bit.capacity;
    uint16_t Fi_run = atw.cycle[cycle].comp[COMP_1].link.state.freq_output;
    int16_t Td = atw.cycle[cycle].comp[COMP_1].local.Td;
    int16_t Tc = atw.cycle[cycle].comp[COMP_1].local.Tc;
    uint16_t I_inv =  atw.cycle[cycle].comp[COMP_1].link.state.ac_current_in;//1次电流
    uint16_t Ku2 = 2000;//TODO 待定义

//get Ku2
    switch (capacity)
    {
        case CAPACITY_45KW:  Ku2 = 1900;   break;
        case CAPACITY_65KW:  Ku2 = 1900;   break;
        case CAPACITY_130KW: Ku2 = 1900;   break;
        default:  break;
    }    
//
    if ((Fi_run < 300)//1.频率小于30Hz
        || (Td <= (Tc + 150))
        || (I_inv >= (Ku2 - 100)))
    {
        res = TRUE;
    }
    return res;
}

/************************************************************************
@name  	: ExpvSubGetOPEXV3_N
@brief 	: 
@param 	: 
@return	: None
@note   : 7.3.4 经济器电子膨胀阀EXV3控制
            (a-3-1)N取值如表  1000倍值
*************************************************************************/
static int16_t ExpvSubGetOPEXV3_N(uint8_t cycle)
{
    int16_t SH_TS_act = atw.cycle[cycle].comp[COMP_1].local.SH_TS_act;
    int16_t SH_TS_Target = atw.cycle[cycle].comp[COMP_1].local.SH_TS_Target;
    int16_t SH_sub = atw.cycle[cycle].io.SH_sub;
    int16_t SH_sub_tar = atw.cycle[cycle].io.SH_sub_tar;
    int16_t ts_diff = SH_TS_act - SH_TS_Target;
    int16_t sub_diff = SH_sub - SH_sub_tar;
    int16_t N = 0;

    if (GetCycleRealRunMode(cycle) == CYCLE_MODE_COOL)//制冷
    {
        if (ts_diff > 20)
        {
            if (sub_diff > 30)
                N = 7;
            else if (sub_diff >= -10)
                N = 0;
            else N = 5;
        }
        else if (ts_diff >= -10)
        {
            if (sub_diff > 30)
                N = 5;
            else if (sub_diff >= -10)
                N = 0;
            else N = 1;
        }
        else if (ts_diff > -20)
        {
            if (sub_diff > 30)
                N = 3;
            else if (sub_diff >= -10)
                N = 0;
            else N = 15;
        }
        else
        {
            if (sub_diff > 30)
                N = 1;
            else if (sub_diff >= -10)
                N = 0;
            else N = 20;
        }
    }
    else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)//制热
    {
        if (ts_diff > 20)
        {
            if (sub_diff > 30)
                N = 5;
            else if (sub_diff >= -10)
                N = 0;
            else N = 10;
        }
        else if (ts_diff >= 0)
        {
            if (sub_diff > 30)
                N = 5;
            else if (sub_diff >= -10)
                N = 0;
            else N = 10;
        }
        else if (ts_diff > -10)
        {
            if (sub_diff > 30)
                N = 3;
            else if (sub_diff >= -10)
                N = 0;
            else N = 15;
        }
        else
        {
            if (sub_diff > 30)
                N = 1;
            else if (sub_diff >= -10)
                N = 0;
            else N = 20;
        }
    }
    return N;
}

/************************************************************************
@name  	: ExpvSubGetOPEXV3_X
@brief 	: 
@param 	: 
@return	: None
@note   : 7.3.4 经济器电子膨胀阀EXV3控制
            (a-3-2)X取值 满足条件
*************************************************************************/
static int16_t ExpvSubGetOPEXV3_X(uint8_t cycle)
{
    int16_t op_exv3_last = atw.cycle[cycle].expv[EXPV3].delta_op_last;
    int16_t SH_sub_last = atw.cycle[cycle].io.SH_sub_last;
    int16_t SH_sub = atw.cycle[cycle].io.SH_sub;
    int16_t SH_sub_tar = atw.cycle[cycle].io.SH_sub_tar;
    int16_t sub_diff = SH_sub - SH_sub_tar;
    int16_t X;

    if (op_exv3_last >= 0)
    {
        if (((sub_diff >= -10) && (sub_diff <= 80)) && ((SH_sub - SH_sub_last) <= -5))
        {
            X = 0;
        }
        else X = 1;
    }
    else
    {
        if (((sub_diff >= -30) && (sub_diff <= 50)) && ((SH_sub - SH_sub_last) >= 5))
        {
            X = 0;
        }
        else X = 1;
    }
    return X;
}

/************************************************************************
@name  	: ExpvSubGetOPEXV3_Y
@brief 	: 
@param 	: 
@return	: None
@note   : 7.3.4 经济器电子膨胀阀EXV3控制
            (a-3-3)Y取值 10倍值
                压缩机运行8mins后,满足以下任一条件,则从本周期开始连续5个周期Y=1/2,否则Y=1
*************************************************************************/
static int16_t ExpvSubGetOPEXV3_Y(uint8_t cycle)
{
    int16_t Y = 10;
    int16_t SH_sub_last = atw.cycle[cycle].io.SH_sub_last;
    int16_t SH_sub = atw.cycle[cycle].io.SH_sub;
    int16_t SH_sub_tar = atw.cycle[cycle].io.SH_sub_tar;
    uint32_t comp_run_time = atw.cycle[cycle].comp[COMP_1].local.current_run_time;
    static uint8_t cnt = 0;

    if (comp_run_time > 8)//压缩机连续运转8mins后
    {
        if (cnt > 0)
        {
            Y = 5;
           cnt++;
           if (cnt > 5)//连续5个周期
           {
               cnt = 0;
           }
        }
    //
        if (cnt == 0)
        {
            if ((SH_sub_last > SH_sub_tar) && (SH_sub < SH_sub_tar)
                || (SH_sub_last < SH_sub_tar) && (SH_sub > SH_sub_tar))
            {
                Y = 5;
                cnt++;
            }
            else Y = 10;
        }
    }
    else cnt = 0;

    return Y;
}

/************************************************************************
@name  	: ExpvSubGetOPEXV3_Z
@brief 	: 
@param 	: 
@return	: None
@note   : 7.3.4 经济器电子膨胀阀EXV3控制
            (a-3-4)Z取值 
                若EXV1/2打开后,若驱动电流Iinv2>23.0A,且SH_sub>=5,则Z=0;否则Z=1
*************************************************************************/
static int16_t ExpvSubGetOPEXV3_Z(uint8_t cycle)
{
    int16_t Z = 1;
    int16_t SH_sub = atw.cycle[cycle].io.SH_sub;
    uint16_t exv1_step = atw.cycle[cycle].expv[EXPV1].rt_exv_step;
    uint16_t Iinv2 = atw.cycle[cycle].comp[COMP_1].link.state.current;

    if (exv1_step > 0)//
    {
        if ((Iinv2 > 2300) && (SH_sub >= 50))
        {
            Z = 0;
        }
    }
    return Z;
}

/************************************************************************
@name  	: ExpvSubGetOPEXV3_fix
@brief 	: OP_eXV3修正值
@param 	: 
@return	: None
@note   : 
*************************************************************************/
static int16_t ExpvSubGetOPEXV3_fix(uint8_t cycle,int16_t op)
{
    int16_t OP_EXV3 = op;
    int16_t Td = atw.cycle[cycle].comp[COMP_1].local.Td;
    int16_t Tc = atw.cycle[cycle].comp[COMP_1].local.Tc;
    int16_t Tevp = atw.cycle[cycle].comp[COMP_1].local.Tevp;
    uint16_t rt_step = atw.cycle[cycle].expv[EXPV3].rt_exv_step;
    int16_t T;

//开阀步数修正
    if (rt_step <= 70)
    {
        if (OP_EXV3 > 2)
        {
            OP_EXV3 = 2;
        }
        else if (OP_EXV3 < -4)
        {
            OP_EXV3 = -4;
        }
    }
    else if (rt_step <= 120)
    {
        if (OP_EXV3 > 3)
        {
            OP_EXV3 = 3;
        }
        else if (OP_EXV3 < -6)
        {
            OP_EXV3 = -6;
        }
    }
    else if (rt_step <= 200)
    {
        if (OP_EXV3 > 4)
        {
            OP_EXV3 = 4;
        }
        else if (OP_EXV3 < -8)
        {
            OP_EXV3 = -8;
        }
    }
//打开200s后
    if (time[cycle].run_200s == 0)//打开200s后
    {
        if (GetCycleRealRunMode(cycle) == CYCLE_MODE_COOL)//制冷
        {
        //get OP_EXV3
            if ((Td - Tc) <= 70)
            {
                if (OP_EXV3 > -5)
                {
                    OP_EXV3 = -5;
                }
            }
            else if ((Td - Tc) < 150)
            {
                if (OP_EXV3 > 0)
                {
                    OP_EXV3 = 0;
                }
            }
        }
        else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_COOL)//制热
        {
        //get T
            if (Tevp > -200)
            {
                T = Tevp;
            }
            else T = -200;

            if (T > 0)
            {
                T = 0;
            }
        //get OP_EXV3    
            if ((Td - Tc) <= (70-T/2))
            {
                if (OP_EXV3 > -5)
                {
                    OP_EXV3 = -5;
                }
            }
            else if ((Td - Tc) < (120 - T/2) )
            {
                if (OP_EXV3 > 0)
                {
                    OP_EXV3 = 0;
                }
            }
        }
    }

    return OP_EXV3;
}

/************************************************************************
@name  	: ExpvSubGetOPEXV3
@brief 	: 
@param 	: 
@return	: None
@note   : 7.3.4 经济器电子膨胀阀EXV3控制
            (a-3)根据经济器实际过热度来计算
                OP_EXV3 = N*X*Y*Z*(SH_sub - SH_sub_tar)*OP_EXV3(i-1),且结果四舍五入取整
*************************************************************************/
static int16_t ExpvSubGetOPEXV3(uint8_t cycle)
{
    static uint16_t N,X,Y,Z;
    int16_t SH_sub = atw.cycle[cycle].io.SH_sub;
    int16_t SH_sub_tar = atw.cycle[cycle].io.SH_sub_tar;
    uint16_t last_step = atw.cycle[cycle].expv[EXPV3].last_exv_step; 
    int32_t OP_EXV3;

    N = ExpvSubGetOPEXV3_N(cycle);//1000倍值
    X = ExpvSubGetOPEXV3_X(cycle);//
    Y = ExpvSubGetOPEXV3_Y(cycle);//10倍值
    Z = ExpvSubGetOPEXV3_Z(cycle);

    OP_EXV3 = N*X*Y*Z*(SH_sub - SH_sub_tar)*last_step;
    OP_EXV3 = OP_EXV3/100000;//温度为10倍值

    return ((int16_t)OP_EXV3);
}

/************************************************************************
@name  	: ExpvSubGetEXV3Step
@brief 	: 
@param 	: 
@return	: None
@note   : 7.3.4 经济器电子膨胀阀EXV3控制
            (a)EXV3开度计算  初始开度维持90s(若Td > 98时为0)
*************************************************************************/
static int16_t ExpvSubGetEXV3Step(uint8_t cycle)
{
    int16_t Td = atw.cycle[cycle].comp[COMP_1].local.Td;
    int16_t Td_last = atw.cycle[cycle].comp[COMP_1].local.Td_last;
    int16_t SH_sub = atw.cycle[cycle].io.SH_sub;
    uint16_t Iinv2 = atw.cycle[cycle].comp[COMP_1].link.state.current;
    uint16_t last_step = atw.cycle[cycle].expv[EXPV3].last_exv_step;
    uint16_t set_step;
    int16_t OP_EXV3;

    OP_EXV3 = ExpvSubGetOPEXV3(cycle);
    OP_EXV3 = ExpvSubGetOPEXV3_fix(cycle,OP_EXV3);

//高排温EXV3开度修正
    if (((Td > 1030) && ((Td - Td_last) >= 3)) || (Td >= 1080))
    {
        if ((atw.cycle[cycle].expv[EXPV3].rt_exv_step > 0)  //开度>0
            && (atw.cycle[cycle].expv[EXPV3].rt_exv_step  <= 200) //开度<=200
            && (SH_sub >= 40)
            && (Iinv2 < 2000)) //TODO 待确认
        {
            if (time[cycle].op_protect_fix == 0)
            {
                time[cycle].op_protect_fix = 2;//2s增加1步
                OP_EXV3 = 1;
                atw.cycle[cycle].expv[EXPV3].delta_op_last = atw.cycle[cycle].expv[EXPV3].delta_op;
                atw.cycle[cycle].expv[EXPV3].delta_op = OP_EXV3;
            }
        }
        else
        {
            atw.cycle[cycle].expv[EXPV3].delta_op_last = atw.cycle[cycle].expv[EXPV3].delta_op;
            atw.cycle[cycle].expv[EXPV3].delta_op = OP_EXV3;
        }
    }
    else
    {
        atw.cycle[cycle].expv[EXPV3].delta_op_last = atw.cycle[cycle].expv[EXPV3].delta_op;
        atw.cycle[cycle].expv[EXPV3].delta_op = OP_EXV3;
    }
    set_step = last_step + OP_EXV3;

    return set_step;
}


/************************************************************************
@name  	: ExpvSubTestModeControl
@brief 	: 
@param 	: 
@return	: None
@note   : 7.3.4 经济器电子膨胀阀EXV3控制
            (a)EXV3开度计算  初始开度维持90s(若Td > 98时为0)
*************************************************************************/
static void ExpvSubNormalRunControl(uint8_t cycle)
{
    int16_t Td = atw.cycle[cycle].comp[COMP_1].local.Td;

    if ((Td >= 980) && (time[cycle].init_step_keep != 0))//Td>98时,clear计时器
    {
        time[cycle].init_step_keep = 0;
    }
//初始开度保持时间    
    if (time[cycle].init_step_keep == 0)
    {
        ExpvSubControl(cycle,EXPV3,ExpvSubGetEXV3Step(cycle));
    }
}

/************************************************************************
@name  	: GetExpvSubInitStepKeep
@brief 	: 
@param 	: 
@return	: None
@note   : EVX3是否处于初始开度保持
*************************************************************************/
uint8_t GetExpvSubInitStepKeep(uint8_t cycle)
{
    uint8_t res = FALSE;

    if (time[cycle].init_step_keep != 0)
    {
        res = TRUE;
    }

    return res;
}

/************************************************************************
@name  	: GetExpvSubNormalCloseState
@brief 	: 
@param 	: 
@return	: None
@note   : EVX3非关机,急停,减载,除霜等原因关闭
            7.3.4 经济器EXV3
            (4)cycle正常运转阶段
                满足以下任一条件,EXV3强制关闭
*************************************************************************/
uint8_t GetExpvSubNormalCloseState(uint8_t cycle)
{
    // atw.cycle[cycle].exv3_force_close = FALSE;
    return atw.cycle[cycle].exv3_force_close;
}

/************************************************************************
@name  	: ExpvMainStatusControl
@brief 	: 经济器膨胀阀的控制
@param 	: 
@return	: None
@note   : 7.3.4 经济器电子膨胀阀EXV3控制
            (2)~(4)cycle关机 ~ cycle运转
*************************************************************************/
void ExpvSubStatusControl(uint8_t cycle)
{
    uint8_t cycle_step = atw.cycle[cycle].work_step;
    // uint8_t last_step = atw.cycle[cycle].last_step;
    uint8_t unit_state = atw.unit.table.init.dsw1.bit.unit_work_state;
    uint16_t rt_step = atw.cycle[cycle].expv[EXPV3].rt_exv_step;

    ExpvSubTimerCounter(cycle);//定时用
    switch (cycle_step)
    {
        case CYCLE_STATUS_OFF:   //cycle关机
        case CYCLE_STATUS_START1://cycle启动1
        case CYCLE_STATUS_START2://cycle启动2
        case CYCLE_STATUS_START_DEFROST_DONE://除霜后启动
        case CYCLE_STATUS_DIFFER_PRESSURE://差压控制
        case CYCLE_STATUS_DEFROST_RUN://除霜运转
        {
            ExpvSubControl(cycle,EXPV3,0);
            atw.cycle[cycle].exv3_force_close = FALSE;
        }
            break;
        case CYCLE_STATUS_RUN_HEAT://cycle制热
        case CYCLE_STATUS_RUN_COOL://cycle制冷
        case CYCLE_STATUS_RUN_OIL_RETURN://回油运转
        case CYCLE_STATUS_DEFROST_PREPARE://除霜准备
        {
            if (time[cycle].period_adjust == 0)//EXV3调节周期20s
            {
                time[cycle].period_adjust = TM_EXPV3_PERIOD;
                if (unit_state == UNIT_STATE_TEST)//测试模式
                {
                    ExpvSubTestModeControl(cycle);
                }
                else//非测试模式
                {
                    if (rt_step == 0)//未开启
                    {
                        if (ExpvSubCheckOpenState(cycle) == TRUE)//满足EXV3开启条件
                        {
                            if (GetCycleRealRunMode(cycle) == CYCLE_MODE_COOL)
                            {
                                ExpvSubControl(cycle,EXPV3,atw.cycle[cycle].expv[EXPV3].exv_Ini_c);
                            }
                            else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_COOL)
                            {
                                ExpvSubControl(cycle,EXPV3,atw.cycle[cycle].expv[EXPV3].exv_Ini_h);
                            }
                            time[cycle].init_step_keep = TM_INIT_STEP_KEEP;
                        }
                    }
                    else//运转中
                    {
                        if (ExpvSubCheckCloseState(cycle) == TRUE)//强制关闭条件
                        {
                            atw.cycle[cycle].exv3_force_close = TRUE;
                            ExpvSubControl(cycle,EXPV3,0);
                        }
                        else
                        {
                            atw.cycle[cycle].exv3_force_close = FALSE;
                            ExpvSubNormalRunControl(cycle);
                        }
                    }
                }
            }
        }
            break;
        default:
            break;
    }
}




