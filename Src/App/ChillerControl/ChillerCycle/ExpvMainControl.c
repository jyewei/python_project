/***********************************************************************
@file   : ExpvMainControl.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "ExpvMainControl.h"
#include "Data.h"
#include "CycleStateControl.h"
#include "ExpvSubControl.h"
#include "TimerCounter.h"
#include <stdlib.h>


//time
typedef struct
{
    uint16_t period_adjust;      //主EXV调节周期
    uint16_t init_step_keep;     //初始开度保持时间 //TODO 考虑压缩机异常时

    uint16_t N_exv_run_time;    //5mins运行时间判断用
    uint16_t fix_period_adjust; //
    // uint16_t fixop1_N_cool_c1;  //
    // uint16_t fixop1_N_cool_c2;  //
    // uint16_t fixop1_N_cool_c3;  //
    // uint16_t fixop1_N_cool_c4;  //
    // uint16_t fixop1_N_heat_c1;  //
    // uint16_t fixop1_N_heat_c2;  //
    // uint16_t fixop1_N_heat_c3;  //
    // uint16_t fixop1_N_heat_c4;  //
    // uint16_t fixop1_N_defrost_c1;  //
    // uint16_t fixop1_N_defrost_c2;  //
    uint16_t fixop1_start_finish;  //△OP1修正中启动结束200s计时用,压缩机启动200s
    
    uint16_t fixop_c1;//Tevp > Te2的条件下,5种条件下的
    uint16_t fixop_c2;
    uint16_t fixop_c3;
    uint16_t fixop_c4;
    uint16_t fixop_c5;

    uint16_t Tevp_protect_adjust; //Tevp过低时EXV的开度修正周期

}ExpvMainTimeType;

static ExpvMainTimeType time[CYCLE_CH_MAX];




/************************************************************************
@name  	: ExpvMainTimerCounter
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ExpvMainTimerCounter(uint8_t cycle)//
{
    uint16_t *pTime;
    uint16_t i,len;
//decrease
    pTime = &time[cycle].period_adjust;
    if (TimerCheck(NO_CYCLE1_EXPVMAIN_BASE+cycle) == TRUE)
    {
        len = sizeof(ExpvMainTimeType)/sizeof(uint16_t);
        TimerSet(NO_CYCLE1_EXPVMAIN_BASE+cycle,1); //base 1s
        for ( i = 0; i < len; i++)
        {
            if (*(pTime + i) != 0)
            {
                (*(pTime + i))--;
            }
        }
    }
//N值计算用
    if (atw.cycle[cycle].main_exv_step_sum == 0 )
    {
        time[cycle].N_exv_run_time = 300; //5mins,N值周期用,运行5mins内
    }
//压缩机启动200s
    if (atw.cycle[cycle].comp[COMP_1].link.state.freq_output == 0)//未启动
    {
        time[cycle].fixop1_start_finish = 200;
    }

}



/************************************************************************
@name  	: ExpvMainGetFixop1_N
@brief 	: 计算fix_op1 N值
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (4-2-2)
                a)N赋值逻辑  //N取10倍值
*************************************************************************/
static int16_t ExpvMainGetFixop1_N(uint8_t cycle)
{
    // uint8_t cycle_step = atw.cycle[cycle].work_step;
    uint16_t main_exv_step_sum_last = atw.cycle[cycle].main_exv_step_sum_last;
    int16_t SH_TS_act = atw.cycle[cycle].comp[COMP_1].local.SH_TS_act;
    int16_t SH_TS_Target = atw.cycle[cycle].comp[COMP_1].local.SH_TS_Target;
    int16_t SH_differ = SH_TS_act - SH_TS_Target;
    int16_t N = 10;

    if (atw.cycle[cycle].work_step == CYCLE_STATUS_DEFROST_RUN)//cycle除霜
    {
        if (abs(SH_differ) > 40)//TODO test abs
        {
            N = 15;
            if ((main_exv_step_sum_last >= 130) && (time[cycle].N_exv_run_time > 0))//除霜按制冷算
                time[cycle].fix_period_adjust = 10;
            else time[cycle].fix_period_adjust = 10;
        }
        else
        {
            N = 30;
            if ((main_exv_step_sum_last >= 130) && (time[cycle].N_exv_run_time > 0))//
                time[cycle].fix_period_adjust = 5;
            else time[cycle].fix_period_adjust = 5;
        }
    }
    else if (GetCycleSetRunMode(cycle) == CYCLE_MODE_COOL)//制冷
    {
        if (SH_differ > 30)
        {
            N = 20;
            if ((main_exv_step_sum_last >= 130) && (time[cycle].N_exv_run_time > 0))//
                time[cycle].fix_period_adjust = 10;
            else time[cycle].fix_period_adjust = 20;
        }
        else if (SH_differ >= 10)
        {
            N = 20;
            if ((main_exv_step_sum_last >= 130) && (time[cycle].N_exv_run_time > 0))//
                time[cycle].fix_period_adjust = 30;
            else time[cycle].fix_period_adjust = 30;
        }
        else if (SH_differ >= 0)
        {
            N = 15;
            if ((main_exv_step_sum_last >= 130) && (time[cycle].N_exv_run_time > 0))//
                time[cycle].fix_period_adjust = 40;
            else time[cycle].fix_period_adjust = 40;
        }
        else
        {
            N = 30;
            if ((main_exv_step_sum_last >= 130) && (time[cycle].N_exv_run_time > 0))//
                time[cycle].fix_period_adjust = 5;
            else time[cycle].fix_period_adjust = 20;
        }
    }
    else if (GetCycleSetRunMode(cycle) == CYCLE_MODE_HEAT)//制热
    {
        if (SH_differ > 20)
        {
            N = 20; //TODO 未赋值
            if ((main_exv_step_sum_last >= 60) && (time[cycle].N_exv_run_time > 0))//
                time[cycle].fix_period_adjust = 10;
            else time[cycle].fix_period_adjust = 20;
        }
        else if (SH_differ >= 0)
        {
            N = 20; //TODO 未赋值
            if ((main_exv_step_sum_last >= 60) && (time[cycle].N_exv_run_time > 0))//
                time[cycle].fix_period_adjust = 30;
            else time[cycle].fix_period_adjust = 30;
        }
        else if (SH_differ > -10)
        {
            N = 15;
            if ((main_exv_step_sum_last >= 60) && (time[cycle].N_exv_run_time > 0))//
                time[cycle].fix_period_adjust = 40;
            else time[cycle].fix_period_adjust = 40;
        }
        else
        {
            N = 30; //TODO 未赋值
            if ((main_exv_step_sum_last >= 60) && (time[cycle].N_exv_run_time > 0))//
                time[cycle].fix_period_adjust = 5;
            else time[cycle].fix_period_adjust = 20;
        }
    }

    return N;
}


/************************************************************************
@name  	: ExpvMainGetFixop1_X
@brief 	: 计算fix_op1 N值
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (4-2-2)
                b)X赋值逻辑
*************************************************************************/
static int16_t ExpvMainGetFixop1_X(uint8_t cycle,int16_t fix_op1_last)
{
    int16_t SH_TS_act = atw.cycle[cycle].comp[COMP_1].local.SH_TS_act; 
    int16_t SH_TS_act_last = atw.cycle[cycle].comp[COMP_1].local.SH_TS_act_last;  
    int16_t SH_TS_Target = atw.cycle[cycle].comp[COMP_1].local.SH_TS_Target;
    int16_t SH_differ = SH_TS_act - SH_TS_Target;
    int16_t X = 1;

    if ((fix_op1_last >= 0) && ((SH_differ >= 0) && (SH_differ <= 50)) && ((SH_TS_act - SH_TS_act_last) <= -2))
    {
        X = 0;
    }
    else if (((fix_op1_last < 0) && ((SH_differ >=-40) && (SH_differ <= 0)) && ((SH_TS_act - SH_TS_act_last) >= 2)))
    {
        X = 0;
    }
    else X = 1;

	return X;
}

/************************************************************************
@name  	: ExpvMainGetFixop1_Y
@brief 	: 计算fix_op1 N值
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (4-2-2)
                b)Y赋值逻辑     //10倍值
*************************************************************************/
static int16_t ExpvMainGetFixop1_Y(uint8_t cycle)
{
    int16_t SH_TS_act = atw.cycle[cycle].comp[COMP_1].local.SH_TS_act; 
    int16_t SH_TS_act_last = atw.cycle[cycle].comp[COMP_1].local.SH_TS_act_last;  
    int16_t SH_TS_Target = atw.cycle[cycle].comp[COMP_1].local.SH_TS_Target;
    int16_t SH_TS_Target_last = atw.cycle[cycle].comp[COMP_1].local.SH_TS_Target_last;
    int16_t SH_differ = SH_TS_act - SH_TS_Target;
    int16_t SH_differ_last = SH_TS_act_last - SH_TS_Target_last;
    uint32_t comp_run_time = atw.cycle[cycle].comp[COMP_1].local.current_run_time;
    int16_t Y = 10;
    static uint8_t cnt = 0;

    if (comp_run_time > 8)
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
        if (cnt == 0)
        {    
            if (((SH_differ_last > 0) && (SH_differ < 0))
                || (SH_differ_last < 0) && (SH_differ > 0))
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
@name  	: ExpvMainGetFixop1_Fix_T
@brief 	: 
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (4-2-2)ΔOP1修正
*************************************************************************/
static int16_t ExpvMainGetFixop1_Fix_T(uint8_t cycle)
{
    int16_t T;
    int16_t Tevp = atw.cycle[cycle].comp[COMP_1].local.Tevp;

    if (Tevp > 0)
    {
        T= 0;
    }
    else if (Tevp >= -250)
    {
        T= -Tevp;
    }
    else T= -20;

    return T;
}

/************************************************************************
@name  	: ExpvMainGetFixop1Fix
@brief 	: 计算fix_op1的修正值
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (4-2-2)ΔOP1修正
*************************************************************************/
static int16_t ExpvMainGetFixop1_Fix(uint8_t cycle,int16_t fix_op1)
{
    int16_t fix = fix_op1,T;
    int16_t Tam = atw.unit.table.io.Tam;
    int16_t Td = atw.cycle[cycle].comp[COMP_1].local.Td;
    int16_t Tc = atw.cycle[cycle].comp[COMP_1].local.Tc;
    int16_t Tevp = atw.cycle[cycle].comp[COMP_1].local.Tevp;
    int16_t SH_TS_act = atw.cycle[cycle].comp[COMP_1].local.SH_TS_act;
    int16_t SH_TS_Target = atw.cycle[cycle].comp[COMP_1].local.SH_TS_Target;
    int16_t Tw_out = atw.unit.table.io.Tw_out;
    uint16_t silence = atw.unit.table.sys_cmd.data1.bit.silence_set;
    int16_t T1,T2,T3;
//Tam
    if (atw.unit.local.sns_err.bit.Tam == TRUE)//Tam有故障时,Tam<=100
    {
        if (Tam > 100)
        {
            Tam = 100;
        }
    }
//Tw_out
    if (Tw_out < 350)
    {
        Tw_out = 350;
    }
    else if (Tw_out > 550)
    {
        Tw_out = 550;
    }
//Tc  
    if (Tc < 180)
    {
        Tc = 180;
    }
    else if (Tc > 580)
    {
        Tc = 580;
    }

    if (atw.cycle[cycle].work_step == CYCLE_STATUS_DEFROST_RUN)//cycle除霜
    {
        //不修正
    }
    else if (GetCycleSetRunMode(cycle) == CYCLE_MODE_COOL)//制冷
    {
        if (((Td - Tc) < 150) && (Tevp >= -30))
        {
            if (fix > 0)
            {
                fix = 0;
            }
        }
        else if ((Td - Tc) >= 420)
        {
            if (fix < 0)
            {
                fix = 0;
            }
        }
    }
    else //制热
    {
        if (((Tw_out >= 350) && (Tw_out <= 550))
            && ((Tc >= 180) && (Tc <= 580)))
        {

            if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)
            {
                if (time[cycle].fixop1_start_finish == 0)//压缩机启动200s后
                {
                    T = ExpvMainGetFixop1_Fix_T(cycle);
                    T1 = 100 - 20*T/3;
                    T2 = 450-20*T/3 + (Tw_out - 450);
                    T3 = 490-20*T/3 + (Tw_out - 450);

                    if (((silence != SILENCE_NULL) && (Tam <= 120))      //tam和静音模式的判断
                        || ((silence == SILENCE_NULL) && (Tam <= 100))) //
                    {
                        if ((Td - Tc) < T1)
                        {
                            if ((SH_TS_act - SH_TS_Target) < 30)
                            {
                                if (fix > 0)
                                {
                                    fix = 0;
                                }
                            }
                        }
                        else if ((Td - Tc) <= T2)
                        {
                            //无修正
                        }
                        else if ((Td - Tc) < (T3))
                        {
                            if (Td <= 1060)
                            {
                                if (fix < -10)
                                {
                                    fix = -10;
                                }
                            }
                            else if (fix < 0)
                            {
                                fix = 0;
                            }
                        }
                        else
                        {
                            if (fix < 0)
                            {
                                fix = 0;
                            }
                        }
                        //
                        if (((Td - Tc) < 150) && (Tevp >= -280))
                        {
                            if (fix > 0)
                            {
                                fix = 0;
                            }
                        }
                    }
                    else
                    {
                        if (((Td - Tc) < 150) && (Tevp >= -280))
                        {
                            if (fix > 0)
                            {
                                fix = 0;
                            }
                        }
                    }
                }
            }
        }
    }

    return fix;
}

/************************************************************************
@name  	: ExpvMainGetFixop1
@brief 	: 计算fix_op1
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (4-2-2)ΔOP1＝N*X*Y*( SH_TS_act - SH_TS_Target)  //op1取10倍值
*************************************************************************/
static int16_t ExpvMainGetFixop1(uint8_t cycle)
{
    static int16_t fix_op1_last;
    int32_t fix_op1;
    int16_t N,X,Y;
    int16_t SH_TS_act = atw.cycle[cycle].comp[COMP_1].local.SH_TS_act;
    int16_t SH_TS_Target = atw.cycle[cycle].comp[COMP_1].local.SH_TS_Target;

    N = ExpvMainGetFixop1_N(cycle);  //10倍值
    X = ExpvMainGetFixop1_X(cycle,fix_op1_last);
    Y = ExpvMainGetFixop1_Y(cycle); //10倍值
    
    fix_op1 = N*X*Y*(SH_TS_act - SH_TS_Target);//1000倍值 //温度为10倍值
    fix_op1 = fix_op1/100; //op1取10倍值
    fix_op1 = ExpvMainGetFixop1_Fix(cycle,(int16_t)fix_op1);
    fix_op1_last = fix_op1;

    return fix_op1;
}

/************************************************************************
@name  	: ExpvMainGetFixop2
@brief 	: 计算fix_op2
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (4-2-2)ΔOP2 制冷运行  //10倍值
*************************************************************************/
static int16_t ExpvMainGetFixop2_cool(uint8_t cycle)
{
    int16_t fix_op2 = 0;
    int16_t Tevp = atw.cycle[cycle].comp[COMP_1].local.Tevp;
    int16_t Td = atw.cycle[cycle].comp[COMP_1].local.Td;
    int16_t Td_last = atw.cycle[cycle].comp[COMP_1].local.Td_last;
    int16_t Tc = atw.cycle[cycle].comp[COMP_1].local.Tc;
    int16_t Tw_out = atw.unit.table.io.Tw_out;
    int16_t SH_TS_act = atw.cycle[cycle].comp[COMP_1].local.SH_TS_act;
    int16_t SH_TS_Target = atw.cycle[cycle].comp[COMP_1].local.SH_TS_Target;
    int16_t T_differ;

//Tc
    if (Tc < 250)
    {
        Tc = 250;
    }
    else if (Tc > 600)
    {
        Tc = 600;
    }

    T_differ = Td - Tc;
    if ((T_differ < 150) && (Tevp >= -30))
    {
        if ((Tw_out < 50) && (SH_TS_act > 50))
        {
            fix_op2 = 0;
        }
        else
        {
            if (T_differ < 50)
            {
                if ((Td - Td_last) < 5)
                {
                    fix_op2 = -70;
                }
                else fix_op2 = (T_differ -70);
            }
            else if (T_differ <= 100)
            {
                if ((Td - Td_last) < 5)
                {
                    fix_op2 = -50;
                }
                else fix_op2 = (T_differ -100);
            }
            else if (T_differ < 150)
            {
                if ((Td - Td_last) < 5)
                {
                    fix_op2 = (T_differ -150)/2;
                }
                else fix_op2 = 0;
            }
        }
    }
    else if ((T_differ < 150) && (Tevp < -30))
    {
        fix_op2 = 0;
    }
    else if ((T_differ >= 150) && (T_differ < 420))
    {
        fix_op2 = 0;
    }
    else if (T_differ >= 420)
    {
        if ( (Td <= 1000) && ((Td - Td_last) > 5) && ((SH_TS_act - SH_TS_Target) > 10))
        {
            fix_op2 = 30;
        }
        else fix_op2 = 0;
    }

    return fix_op2;
}

/************************************************************************
@name  	: ExpvMainGetFixop2
@brief 	: 计算fix_op2
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (4-2-2)ΔOP2 制热运行   //取10倍值
*************************************************************************/
static int16_t ExpvMainGetFixop2_heat(uint8_t cycle)
{
    int16_t fix_op2 = 0;
    int16_t Tevp = atw.cycle[cycle].comp[COMP_1].local.Tevp;
    int16_t Td = atw.cycle[cycle].comp[COMP_1].local.Td;
    int16_t Td_last = atw.cycle[cycle].comp[COMP_1].local.Td_last;
    int16_t Tc = atw.cycle[cycle].comp[COMP_1].local.Tc;
    int16_t SH_TS_act = atw.cycle[cycle].comp[COMP_1].local.SH_TS_act;
    int16_t T_differ = Td - Tc;

    if ((T_differ < 150) && (Tevp >= -280))
    {
        if (T_differ < 50)
        {
            if ((Td - Td_last) < 5)
            {
                fix_op2 = -70;
            }
            else fix_op2 = (T_differ - 70);
        }
        else if (T_differ <= 100)
        {
            if ((Td - Td_last) < 5)
            {
                fix_op2 = -50;
            }
            else fix_op2 = (T_differ - 100);
        }
        else if (T_differ < 150)
        {
            if ((Td - Td_last) < 5)
            {
                fix_op2 = (T_differ - 150)/2;
            }
            else fix_op2 = 0;
        }
    }
    else if ((T_differ < 150) && (Tevp < -280))
    {
        fix_op2 = 0;
    }
    else if ((T_differ >= 150) && (T_differ <= 460))
    {
        fix_op2 = 0;
    }
    else if ((T_differ >= 460))
    {
        if ((Td <= 1000) && ((Td - Td_last) >5) && (SH_TS_act > 30))
        {
            fix_op2 = 10;
        }
        else fix_op2 = 0;
    }

    return fix_op2;
}


/************************************************************************
@name  	: ExpvMainGetFixop2
@brief 	: 计算fix_op2
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (4-2-2)ΔOP2 //op2取10倍值
*************************************************************************/
static int16_t ExpvMainGetFixop2(uint8_t cycle)
{
    int16_t fix_op2 = 0;

    if (atw.cycle[cycle].work_step == CYCLE_STATUS_DEFROST_RUN)
    {
        fix_op2 = 0;
    }
    else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)
    {
        fix_op2 = ExpvMainGetFixop2_heat( cycle);
    }
    else fix_op2 = ExpvMainGetFixop2_cool( cycle);

    // switch (atw.cycle[cycle].work_step)
    // {
    //     case CYCLE_STATUS_RUN_COOL:      fix_op2 = ExpvMainGetFixop2_cool( cycle);       break;
    //     case CYCLE_STATUS_RUN_HEAT:      fix_op2 = ExpvMainGetFixop2_heat( cycle);      break;
    //     case CYCLE_STATUS_DEFROST_RUN:   fix_op2 = 0;       break;
    //     default:    break;
    // }
    return fix_op2;
}

/************************************************************************
@name  	: ExpvMainGetFixop3
@brief 	: 计算fix_op3
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (4-2-2)ΔOP3 //op3取10倍值
*************************************************************************/
static int16_t ExpvMainGetFixop3_heat(uint8_t cycle)
{
    int16_t SH_TS_act = atw.cycle[cycle].comp[COMP_1].local.SH_TS_act;
    int16_t SH_TS_Target = atw.cycle[cycle].comp[COMP_1].local.SH_TS_Target;
    uint16_t exv3_step = atw.cycle[cycle].expv[EXPV3].rt_exv_step;
    uint16_t exv3_step_last = atw.cycle[cycle].expv[EXPV3].last_exv_step;
    int16_t fix_op3 = 0,SH_differ = SH_TS_act - SH_TS_Target;

    if ((GetExpvSubInitStepKeep(cycle) == TRUE) && (SH_differ <= 10))//1.EXV3置于初始开度
    {
        fix_op3 = -3*exv3_step/10;
    }
    else if ((GetExpvSubInitStepKeep(cycle) == FALSE) && ((exv3_step - exv3_step_last) >=1) && (SH_differ <= 0))//2
    {
        fix_op3 = 20*(exv3_step_last - exv3_step);
    }
    else if ((GetExpvSubInitStepKeep(cycle) == FALSE) && ((exv3_step - exv3_step_last) <= -1) && (SH_differ >= 0))//3
    {
        fix_op3 = 20*(exv3_step_last - exv3_step);
    }
    else if ((GetExpvSubNormalCloseState(cycle) == TRUE) && (SH_differ>= -10))//4
    {
        fix_op3 = 3*exv3_step/10;
        if (fix_op3 > 200)
        {
            fix_op3 = 200;
        }
    }
    else fix_op3 = 0;

    return fix_op3;
}

/************************************************************************
@name  	: ExpvMainGetFixop3
@brief 	: 计算fix_op3
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (4-2-2)ΔOP3 //op3取10倍值
*************************************************************************/
static int16_t ExpvMainGetFixop3(uint8_t cycle)
{
    int16_t fix_op3 = 0;
    //uint8_t cycle_step = atw.cycle[cycle].work_step;

    if (atw.cycle[cycle].work_step == CYCLE_STATUS_DEFROST_RUN)
    {
        fix_op3 = 0;
    }
    else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)
    {
        fix_op3 = ExpvMainGetFixop3_heat(cycle);
    }
    else fix_op3 = 0;

    // switch (cycle_step)
    // {
    //     case CYCLE_STATUS_RUN_HEAT:      fix_op3 = ExpvMainGetFixop3_heat(cycle);       break;
    //     case CYCLE_STATUS_RUN_COOL:      fix_op3 = 0;      break;
    //     case CYCLE_STATUS_DEFROST_RUN:   fix_op3 = 0;      break;
    //     default:    break;
    // }
    return fix_op3;
}

/************************************************************************
@name  	: ExpvMainFixopSetLimit
@brief 	: 主膨胀阀的控制
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (4-2-2)电子膨胀阀开度控制
                ΔOP取值范围判断 //10倍值
*************************************************************************/
static int16_t ExpvMainFixopSetLimit(uint8_t cycle,int16_t fix_op)
{
    int16_t fix = fix_op;
    int16_t Tevp = atw.cycle[cycle].comp[COMP_1].local.Tevp;
    int16_t Td = atw.cycle[cycle].comp[COMP_1].local.Td;
    int16_t Tc = atw.cycle[cycle].comp[COMP_1].local.Tc;
    int16_t Te1,Te2;
    uint16_t step_sum_last = atw.cycle[cycle].main_exv_step_sum_last;
    uint16_t step_sum = atw.cycle[cycle].main_exv_step_sum;
    // uint8_t cycle_step = atw.cycle[cycle].work_step;
  
    if (atw.cycle[cycle].work_step == CYCLE_STATUS_DEFROST_RUN)
    {
        Te1 = 90; 
        Te2 = 130;
    }
    else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)
    {
        Te1 = 70; 
        Te2 = 100;
    }
    else 
    {
        Te1 = 70; 
        Te2 = 100;
    }

    // switch (cycle_step)
    // {
    //     case CYCLE_STATUS_RUN_HEAT:     Te1 = 70; Te2 = 100;   break;
    //     case CYCLE_STATUS_RUN_COOL:     Te1 = 70; Te2 = 100;   break;
    //     case CYCLE_STATUS_DEFROST_RUN:  Te1 = 90; Te2 = 130;   break;
    //     default:    break;
    // }

    if (Tevp <= Te1)
    {
        if (step_sum_last < 100)//3. OP (i-1)＜100
        {
            if (fix > 80)
            {
                fix = 40;
            }
            else if (fix < -80)
            {
                fix = -40;
            }
            else if (fix < 7)
            {
                fix = 0;
            }
            else 
            {
                fix = fix*4/10;
                if (fix > 10)
                {
                    fix = 10;
                }
                else if (fix < -10)
                {
                    fix = -10;
                }
            }
        }
        else if (step_sum_last < 150)//2. 100≤OP (i-1)＜150
        {
            if (fix > 100)
            {
                fix = 60;
            }
            else if (fix < -100)
            {
                fix = -60;
            }
            else if (fix < 7)
            {
                fix = 0;
            }
            else 
            {
                fix = fix*6/10;
                if (fix > 10)
                {
                    fix = 10;
                }
                else if (fix < -10)
                {
                    fix = -10;
                }
            }
        }
        else if (step_sum_last < 200)//1. 150≤OP(i-1)＜200
        {
            if (fix > 120)
            {
                fix = 90;
            }
            else if (fix < -120)
            {
                fix = -90;
            }
            else if (fix < 10)
            {
                fix = 0;
            }
            else 
            {
                fix = fix*8/10;
                if (fix > 10)
                {
                    fix = 10;
                }
                else if (fix < -10)
                {
                    fix = -10;
                }
            }
        }
        else
        {
            //不修正
        }
    }
    else if (Tevp <= Te2)
    {
        if (fix > 30)
        {
            fix = 30;
        }
    }
    else//>Te2
    {
        if ((Td - Tc) <= 150)//
        {
            if (time[cycle].fixop_c1 == 0)//
            {
                time[cycle].fixop_c1 = 5;//5s周期
                fix = -3*step_sum/100;
            }
        }
        else
        {
            if (Td <= 1000)//
            {
                if (time[cycle].fixop_c2 == 0)//
                {
                    time[cycle].fixop_c2 = 5;//5s周期
                    fix = -2*step_sum/100;
                }
            }
            else
            {
                if (Tevp >= 180)
                {
                    if (time[cycle].fixop_c3 == 0)//
                    {
                        time[cycle].fixop_c3 = 5;//5s周期
                        fix = -1*step_sum/100;
                        if (fix < -20)
                        {
                            fix = -20;
                        }
                    }
                }
                else if (Tevp >= 150)
                {
                    if (time[cycle].fixop_c4 == 0)//
                    {
                        time[cycle].fixop_c4 = 5;//5s周期
                        fix = -10;
                    }
                }
                else
                {
                    if (time[cycle].fixop_c5 == 0)//
                    {
                        time[cycle].fixop_c5 = 20;//20s周期
                        fix = -10;
                    }
                }
            }
        }
    }

    return fix;
}

/************************************************************************
@name  	: ExpvMainGetInitStepCool
@brief 	: 
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            获取启动初始步数expv1_ini 制冷
*************************************************************************/
static void ExpvMainGetInitStepCool(uint8_t cycle)
{
    int16_t Tam = atw.unit.table.io.Tam;
    uint8_t capacity = atw.unit.table.init.dsw1.bit.capacity;
    uint16_t ini_step = atw.unit.table.sys_cmd.exv_cool_init_step;    //组控器参数[EXV制冷初始步数]
    int16_t  op1 = 0,op2 = 0;

    if (atw.unit.local.sns_err.bit.Tam == FALSE)//Tam无故障
    {
        if (Tam >= 350)
        {
            op1 = 0;
        }
        else if (Tam >= 230)
        {
            op1 = 10;
        }
        else if (Tam >= 150)
        {
            op1 = 20;
        }
        else if (Tam >= 50)
        {
            op1 = 30;
        }
        else
        {
            op1 = 40;
        }
    }
    else op1 = 20;

    if ((capacity == CAPACITY_65KW) || (capacity == CAPACITY_130KW))
    {
        op2 = -50;
    }
    
    atw.cycle[cycle].expv[EXPV1].exv_Ini_c = ini_step + op1 + op2;
    atw.cycle[cycle].expv[EXPV2].exv_Ini_c = ini_step + op1 + op2;

}

/************************************************************************
@name  	: ExpvMainGetInitStepHeat
@brief 	: 
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            获取启动初始步数expv1_ini 制热
*************************************************************************/
static void ExpvMainGetInitStepHeat(uint8_t cycle)
{
    int16_t Tam = atw.unit.table.io.Tam;
    uint8_t capacity = atw.unit.table.init.dsw1.bit.capacity;
    uint16_t ini_step = atw.unit.table.sys_cmd.exv_heat_init_step;//组控器参数[EXV制热初始步数]
    int16_t  op1 = 0,op2 = 0;

    if (atw.unit.local.sns_err.bit.Tam == FALSE)//Tam无故障
    {
        if (Tam >= 100)
        {
            op1 = 0;
        }
        else if (Tam >= 20)
        {
            op1 = -20;
        }
        else if (Tam >= -50)
        {
            op1 = -30;
        }
        else if (Tam >= -120)
        {
            op1 = -35;
        }
        else
        {
            op1 = -40;
        }
    }
    else op1 = -30;

    if ((capacity == CAPACITY_65KW) || (capacity == CAPACITY_130KW))
    {
        op2 = 40;
    }
    
    atw.cycle[cycle].expv[EXPV1].exv_Ini_h = ini_step + op1 + op2;
    atw.cycle[cycle].expv[EXPV2].exv_Ini_h = ini_step + op1 + op2;
}

/************************************************************************
@name  	: ExpvMainGetInitStepHeat
@brief 	: 
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            获取启动初始步数expv1_ini 除霜
*************************************************************************/
static void ExpvMainGetInitStepDefrost(uint8_t cycle)
{
    uint16_t exv_Ini_defrost = atw.unit.table.sys_cmd.exv_Ini_defrost;  //组控器参数[EXV除霜初始步数]

    atw.cycle[cycle].expv[EXPV1].exv_Ini_defrost = exv_Ini_defrost; 
    atw.cycle[cycle].expv[EXPV2].exv_Ini_defrost = exv_Ini_defrost;
}

/************************************************************************
@name  	: ExpvMainGetMaxStep
@brief 	: 
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            最大开度
*************************************************************************/
static void ExpvMainGetMaxStep(uint8_t cycle)
{
    atw.cycle[cycle].expv[EXPV1].exv_max_c = 480; //固定480
    atw.cycle[cycle].expv[EXPV1].exv_max_h = 480;
    atw.cycle[cycle].expv[EXPV1].exv_max_defrost = 480;
    atw.cycle[cycle].expv[EXPV2].exv_max_c = 480;
    atw.cycle[cycle].expv[EXPV2].exv_max_h = 480;
    atw.cycle[cycle].expv[EXPV2].exv_max_defrost = 480;
}

/************************************************************************
@name  	: ExpvMainGetMaxStep
@brief 	: 
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            最小开度
*************************************************************************/
static void ExpvMainGetMinStep(uint8_t cycle)
{
    uint16_t Fi_run = atw.cycle[cycle].comp[COMP_1].link.state.freq_output;
    uint16_t exv_min_c_set = atw.unit.table.sys_cmd.exv_cool_min_step;    //组控器参数[EXV制冷最小步数]
    uint16_t exv_min_h_set = atw.unit.table.sys_cmd.exv_heat_min_step;    //组控器参数[EXV制热最小步数]
    uint16_t exv_min_c = atw.cycle[cycle].expv[EXPV1].exv_min_c;
    uint16_t exv_min_h = atw.cycle[cycle].expv[EXPV1].exv_min_h;
//exv_min_c
    exv_min_c = exv_min_c_set + (Fi_run - 500)/10;
    if (exv_min_c < exv_min_c_set)
    {
        exv_min_c = exv_min_c_set;
    }
    else if (exv_min_c > (exv_min_c_set + 30))
    {
        exv_min_c = exv_min_c_set + 30;
    }
//exv_min_h
    exv_min_h = exv_min_h_set + (Fi_run - 400)/10; //
    if (exv_min_h < exv_min_h_set)
    {
        exv_min_h = exv_min_h_set;
    }
    else if (exv_min_h > (exv_min_h_set + 20))
    {
        exv_min_h = exv_min_h_set + 20;
    }

    atw.cycle[cycle].expv[EXPV1].exv_min_c = exv_min_c; //
    atw.cycle[cycle].expv[EXPV1].exv_min_h = exv_min_h;
    atw.cycle[cycle].expv[EXPV1].exv_min_defrost = 150;   //除霜 固定150
    atw.cycle[cycle].expv[EXPV2].exv_min_c = exv_min_c;
    atw.cycle[cycle].expv[EXPV2].exv_min_h = exv_min_h;
    atw.cycle[cycle].expv[EXPV2].exv_min_defrost = 150;
}

/************************************************************************
@name  	: ExpvMainGetMaxStep
@brief 	: 
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (4)通常运转
                (4-2-1)启动阶段,制热
*************************************************************************/
static void ExpvMainInitStepHeatControl(uint8_t cycle,uint8_t exv)
{
    int16_t SH_TS_act = atw.cycle[cycle].comp[COMP_1].local.SH_TS_act;
    int16_t Tevp = atw.cycle[cycle].comp[COMP_1].local.Tevp;
    uint16_t exv_step = atw.cycle[cycle].expv[exv].set_exv_step;

    if (((Tevp > -200) && (Tevp <=-120)) && (SH_TS_act >= 150))
    {
        if (time[cycle].period_adjust == 0)
        {
            time[cycle].period_adjust = 5;
            exv_step += 1;
        }
    }
    else if (((Tevp > -260) && (Tevp <=-200)) && (SH_TS_act >= 100))
    {
        if (time[cycle].period_adjust == 0)
        {
            time[cycle].period_adjust = 2;
            exv_step += 1;
        }
    }
    else if ((Tevp <= -260) && (SH_TS_act >= 70))
    {
        if (time[cycle].period_adjust == 0)
        {
            time[cycle].period_adjust = 2;
            exv_step += 2;
        }
    }
    atw.cycle[cycle].expv[exv].set_exv_step = exv_step;
    atw.cycle[cycle].main_exv_step_sum = atw.cycle[cycle].expv[EXPV1].rt_exv_step + atw.cycle[cycle].expv[EXPV2].rt_exv_step;
}

/************************************************************************
@name  	: ExpvMainControl
@brief 	: 主膨胀阀步数设置
@param 	: 
@return	: None
@note   : EXPV1/EXPV2 的步数控制
*************************************************************************/
static void ExpvMainControl(uint8_t cycle,uint8_t expv,int16_t step)
{
    uint8_t cycle_step = atw.cycle[cycle].work_step;
    uint8_t cycle_mode = GetCycleRealRunMode(cycle);
    uint16_t exv_min;
    uint16_t exv_max;

    if (step > 0)//运行中
    {
        ExpvMainGetMaxStep(cycle);
        ExpvMainGetMinStep(cycle);
        if ((cycle_step == CYCLE_STATUS_DEFROST_RUN) || (cycle_step == CYCLE_STATUS_START_DEFROST_DONE))
        {
            exv_min = atw.cycle[cycle].expv[expv].exv_min_defrost;
            exv_max = atw.cycle[cycle].expv[expv].exv_max_defrost;
        }
        else
        {
            if (cycle_mode == CYCLE_MODE_COOL)
            {
                exv_min = atw.cycle[cycle].expv[expv].exv_min_c;
                exv_max = atw.cycle[cycle].expv[expv].exv_max_c;
            }
            else if (cycle_mode == CYCLE_MODE_HEAT)
            {
                exv_min = atw.cycle[cycle].expv[expv].exv_min_h;
                exv_max = atw.cycle[cycle].expv[expv].exv_max_h;
            }
        }
    //
        if (step < exv_min)
        {
            step = exv_min;
        }
        else if (step > exv_max)
        {
            step = exv_max;
        }
    }
    atw.cycle[cycle].expv[expv].set_exv_step = step;
    atw.cycle[cycle].expv[expv].rt_exv_step = atw.cycle[cycle].expv[expv].set_exv_step;//相等,无FB
}

/************************************************************************
@name  	: ExpvMainCheckInitStepQuit
@brief 	: 
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (4)通常运转
                (4-2-1)启动阶段
*************************************************************************/
static uint8_t ExpvMainCheckInitStepQuit(uint8_t cycle)
{
    uint8_t res = FALSE;
    int16_t Tevp = atw.cycle[cycle].comp[COMP_1].local.Tevp;
    int16_t SH_TS_act = atw.cycle[cycle].comp[COMP_1].local.SH_TS_act;
    int16_t SH_TS_Target = atw.cycle[cycle].comp[COMP_1].local.SH_TS_Target;

    if (time[cycle].init_step_keep == 0)//时间满足
    {
        res = TRUE;
    }
    else 
    {
        if (GetCycleRealRunMode(cycle) == CYCLE_MODE_COOL)//制冷
        {
            if ((Tevp <= -30) && ((SH_TS_act - SH_TS_Target) >= 30))
            {
                res = TRUE;
            }
        }
        else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)//制热
        {
            ExpvMainInitStepHeatControl(cycle,EXPV1);
            ExpvMainInitStepHeatControl(cycle,EXPV2);
            ExpvMainControl(cycle,EXPV1,atw.cycle[cycle].expv[EXPV1].set_exv_step);
            ExpvMainControl(cycle,EXPV2,atw.cycle[cycle].expv[EXPV2].set_exv_step);
        }        
    }
    return res;
}

/************************************************************************
@name  	: ExpvMainSumStepAllot
@brief 	: 主膨胀阀的控制
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (4-6)EXV1/2开度分配
*************************************************************************/
static void ExpvMainSumStepAllot(uint8_t cycle,uint16_t sum_step)
{
    uint8_t exv2_enable = atw.cycle[cycle].expv[EXPV2].enable;
    uint16_t exv1_step = atw.cycle[cycle].expv[EXPV1].rt_exv_step;
    uint16_t exv2_step = atw.cycle[cycle].expv[EXPV2].rt_exv_step;

    if (exv2_enable == TRUE)//
    {
        if (exv2_step > 0)//已开启
        {
            if (sum_step < 140)
            {
                exv2_step = 0;
                exv1_step = sum_step - 30;
            }
            else if (sum_step < 180)
            {
                exv2_step = 60;
                exv1_step = sum_step - exv2_step;
            }
            else if (sum_step < 200)
            {
                exv1_step = 300 - sum_step;
                exv2_step = sum_step - exv1_step;
            }
            else// >= 200
            {
                exv1_step = exv2_step = sum_step/2;
            }
        }
        else//未开启
        {
            if (sum_step <= 150)
            {
                exv2_step = 0;
                exv1_step = sum_step;
            }
            else
            {
                exv1_step = 120;
                exv1_step = sum_step - 90;//sum_step - exv1_step + 30;
            }
        }
    }
    else
    {
        exv1_step = sum_step;
    }
    ExpvMainControl(cycle,EXPV1,exv1_step);
    ExpvMainControl(cycle,EXPV2,exv2_step);
}

/************************************************************************
@name  	: ExpvMainGetFixop
@brief 	: 主膨胀阀的控制
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (4-2-2)电子膨胀阀开度控制
                OP(i) = OP(i-1) + ΔOP
                ΔOP = ΔOP1 + ΔOP2 + ΔOP3
                    OP (i):当前开度计算值
                    OP(i-1)：上一次的实际开度
                    ΔOP1——根据吸气过热度修正值
                    ΔOP2——根据排气过热度修正值
                    ΔOP3——根据经济器EXV3动作修正值
*************************************************************************/
static int16_t ExpvMainGetFixop(uint8_t cycle)
{
    int16_t  fix_op,fix_op1,fix_op2,fix_op3;

    fix_op1 = ExpvMainGetFixop1(cycle); //10倍值
    fix_op2 = ExpvMainGetFixop2(cycle); //10倍值
    fix_op3 = ExpvMainGetFixop3(cycle); //10倍值
    fix_op = fix_op1 + fix_op2 + fix_op3;
    fix_op = ExpvMainFixopSetLimit(cycle,fix_op);
    fix_op = fix_op/10;

    return fix_op;
}

/************************************************************************
@name  	: ExpvMainStepSet
@brief 	: None
@param 	: 
@return	: None
*************************************************************************/
static void ExpvMainStepSet(uint8_t cycle,uint16_t step)
{
    atw.cycle[cycle].main_exv_step_sum_last = atw.cycle[cycle].main_exv_step_sum;
    atw.cycle[cycle].main_exv_step_sum = step; 
}

/************************************************************************
@name  	: ExpvMainStepProtect_Fi
@brief 	: 主膨胀阀的控制
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (4-3)升/降频EXV修正
            (4-4)蒸发温度过低EXV修正
            (4-5)排气温度过低EXV修正 优先级(4-5)>(4-4)>(4-3)
*************************************************************************/
static int16_t ExpvMainStepProtect_Fi(uint8_t cycle) //(4-3)升降频开度修正 //TODO 频率相关
{
    uint16_t Fi_run = atw.cycle[cycle].comp[COMP_1].link.state.freq_output;
    uint16_t Fi_set = atw.cycle[cycle].comp[COMP_1].link.ctrl.freq_set;
    int16_t SH_TS_act = atw.cycle[cycle].comp[COMP_1].local.SH_TS_act;
    int16_t SH_TS_Target = atw.cycle[cycle].comp[COMP_1].local.SH_TS_Target;
    int16_t SH_TS_differ = SH_TS_act - SH_TS_Target;
    int16_t  delta_freq = Fi_set - Fi_run;  //TODO delta的计算
    uint16_t step = atw.cycle[cycle].main_exv_step_sum;
    uint16_t step_last = atw.cycle[cycle].main_exv_step_sum_last;

    if (GetCycleRealRunMode(cycle) == CYCLE_MODE_COOL)//制冷
    {
        if ((SH_TS_differ > 30) && (delta_freq > 0))//升频
        {
            step = step_last + delta_freq;
        }
        else if ((SH_TS_differ < 10) && (delta_freq < 0))//降频
        {
            step = step_last - delta_freq*2;
        }
    }
    else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)//制热
    {
        if ((SH_TS_differ > 10) && (delta_freq > 0))//升频
        {
            step = step_last + delta_freq/2;
        }
        else if ((SH_TS_differ < 20) && (delta_freq < 0))//降频
        {
            step = step_last*(100-95*delta_freq/Fi_run)/100;
            if (step < 70)
            {
                step = 70;
            }
        }
    }
    ExpvMainStepSet(cycle,step);
    return TRUE;
}

/************************************************************************
@name  	: ExpvMainStepProtect_Tevp
@brief 	: 主膨胀阀的控制
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (4-3)升/降频EXV修正
            (4-4)蒸发温度过低EXV修正
            (4-5)排气温度过低EXV修正 优先级(4-5)>(4-4)>(4-3)
*************************************************************************/
static int16_t ExpvMainStepProtect_Tevp(uint8_t cycle) //(4-4)蒸发压力低开度修正
{
    int16_t SH_TS_act = atw.cycle[cycle].comp[COMP_1].local.SH_TS_act;
    int16_t Tevp = atw.cycle[cycle].comp[COMP_1].local.Tevp;
    int16_t step = atw.cycle[cycle].main_exv_step_sum;

    if (GetCycleRealRunMode(cycle) == CYCLE_MODE_COOL)//制冷
    {
        if (step < 200)
        {
            if ((Tevp <= 0) && (SH_TS_act >= 80))
            {
                if (time[cycle].Tevp_protect_adjust == 0)
                {
                    time[cycle].Tevp_protect_adjust = 5;
                    step += 3;
                }
            }
            ExpvMainStepSet(cycle,step);
        }
    }
    else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)//制热
    {
        if (step < 150)
        {
            if  ((Tevp > -200) && (Tevp <= -120) && (SH_TS_act >= 100))
            {
                if (time[cycle].Tevp_protect_adjust == 0)
                {
                    time[cycle].Tevp_protect_adjust = 5;
                    step += 1;
                }
            }
            else if  ((Tevp > -260) && (Tevp <= -200) && (SH_TS_act >= 70))
            {
                if (time[cycle].Tevp_protect_adjust == 0)
                {
                    time[cycle].Tevp_protect_adjust = 5;
                    step += 2;
                }
            }
            else if  ((Tevp <= -260) && (SH_TS_act >= 70))
            {
                if (time[cycle].Tevp_protect_adjust == 0)
                {
                    time[cycle].Tevp_protect_adjust = 2;
                    step += 1;
                }
            }
            ExpvMainStepSet(cycle,step);
        }
    }

    return TRUE;
}

/************************************************************************
@name  	: ExpvMainStepUpdate
@brief 	: None
@param 	: 
@return	: None
*************************************************************************/
static void ExpvMainStepUpdate(uint8_t cycle,int16_t fix)
{
    atw.cycle[cycle].main_exv_step_sum_last = atw.cycle[cycle].main_exv_step_sum;
    atw.cycle[cycle].main_exv_step_sum = atw.cycle[cycle].main_exv_step_sum_last + fix; 
}

/************************************************************************
@name  	: ExpvMainStepProtectFix_Td
@brief 	: 主膨胀阀的控制
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            排气温度过低EXV修正
*************************************************************************/
static int16_t ExpvMainStepProtectFix_Td(uint8_t cycle)//
{
    int16_t step_last = atw.cycle[cycle].main_exv_step_sum_last;
    int16_t Ps = atw.cycle[cycle].comp[COMP_1].local.Ps;
    int16_t Td = atw.cycle[cycle].comp[COMP_1].local.Td;
    int16_t Td_last = atw.cycle[cycle].comp[COMP_1].local.Td_last;
    int16_t Tc = atw.cycle[cycle].comp[COMP_1].local.Tc;
    int16_t Tevp = atw.cycle[cycle].comp[COMP_1].local.Tevp;
    int16_t Td_differ;
    int16_t fix_op =  ExpvMainGetFixop(cycle);

    if (GetCycleRealRunMode(cycle) == CYCLE_MODE_COOL)//制冷
    {
        if (Td < 1000)
        {
            if ((((Td - Tc) > 150) && ((Td - Tc) < 300)) && (Tevp >= -20))
            {
                Td_differ = Td - Td_last;
                if (Td_differ < -2)
                {
                    if (fix_op > 0)
                    {
                        fix_op = 0; 
                    }
                }
                else if (Td_differ < 0)
                {
                    if (fix_op > 1)
                    {
                        fix_op = 1; 
                    }
                }
                else if (Td_differ < 5)
                {
                    if (fix_op > 2)
                    {
                        fix_op = 2; 
                    }
                }
            }
            ExpvMainStepUpdate(cycle,fix_op);
        }
        else if (Td < 1050)
        {
            if (fix_op < -1)
            {
                fix_op = -1;
            }
            //fixop修正 
            ExpvMainStepUpdate(cycle,fix_op);
        }
        else// >= 105
        {
            if (((Td - Td_last) > 0) && (step_last <= 350) && (Ps <= 1220))
            {
                if (fix_op < 5)
                {
                    fix_op = 5;
                }
            }
            ExpvMainStepUpdate(cycle,fix_op);//fixop修正
        }
    }
    else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)//制热
    {
        if (Td < 1050)
        {
            if ((((Td - Tc) > 150) && ((Td - Tc) < 350)) && (Tevp >= -280))
            {
                Td_differ = Td - Td_last;
                if (Td_differ < -2)
                {
                    if (fix_op > -1)
                    {
                        fix_op = -1; 
                    }
                }
                else if (Td_differ < 0)
                {
                    if (fix_op > 0)
                    {
                        fix_op = 0; 
                    }
                }
                else if (Td_differ < 5)
                {
                    if (fix_op > 1)
                    {
                        fix_op = 1; 
                    }
                }
            }
            ExpvMainStepUpdate(cycle,fix_op);//fixop修正 
        }
        else// >= 105
        {
            if (((Td - Td_last) > 0) && (step_last <= 200))
            {
                if (fix_op < 1)
                {
                    fix_op = 1;
                }
            }
            ExpvMainStepUpdate(cycle,fix_op);//fixop修正
        }
    }

    return TRUE;
}

/************************************************************************
@name  	: ExpvMainNormalRun
@brief 	: 主膨胀阀的控制
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
*************************************************************************/
static void ExpvMainNormalRun(uint8_t cycle)
{
    if (time[cycle].fix_period_adjust == 0)//调节周期
    {
        ExpvMainStepProtectFix_Td(cycle);
    }
    ExpvMainStepProtect_Fi(cycle);
    ExpvMainStepProtect_Tevp(cycle);
    ExpvMainSumStepAllot(cycle,atw.cycle[cycle].main_exv_step_sum);
}

/************************************************************************
@name  	: ExpvMainStatusControl
@brief 	: 主膨胀阀的控制
@param 	: 
@return	: None
@note   : 7.3.3 主电子膨胀阀控制
            (2)~(4)cycle关机 ~ cycle运转
*************************************************************************/
void ExpvMainStatusControl(uint8_t cycle)
{
    uint8_t cycle_step = atw.cycle[cycle].work_step;
    uint8_t last_step = atw.cycle[cycle].last_step;

    ExpvMainTimerCounter(cycle);
    switch (cycle_step)
    {
        case CYCLE_STATUS_OFF:   //cycle关机
        case CYCLE_STATUS_START1://cycle启动1
        {
            ExpvMainControl(cycle,EXPV1,0);
            ExpvMainControl(cycle,EXPV2,0);
        }
            break;
        case CYCLE_STATUS_START2://cycle启动2
        case CYCLE_STATUS_START_DEFROST_DONE://cycle除霜后启动
        case CYCLE_STATUS_RUN_HEAT://cycle制热
        case CYCLE_STATUS_RUN_COOL://cycle制冷
        case CYCLE_STATUS_RUN_OIL_RETURN://cycle回油运转
        case CYCLE_STATUS_DEFROST_PREPARE://除霜准备
        case CYCLE_STATUS_DIFFER_PRESSURE://差压控制
        case CYCLE_STATUS_DEFROST_RUN://除霜运转
        {
            if (((cycle_step == CYCLE_STATUS_DEFROST_RUN) && (last_step != CYCLE_STATUS_DEFROST_RUN))//进入除霜运转,调用初始开度
                || ((cycle_step == CYCLE_STATUS_START_DEFROST_DONE) && (last_step != CYCLE_STATUS_START_DEFROST_DONE)))//除霜后启动,调用初始开度
            {
                ExpvMainGetInitStepDefrost(cycle);
                ExpvMainControl(cycle,EXPV1,atw.cycle[cycle].expv[EXPV1].exv_Ini_defrost);
                ExpvMainControl(cycle,EXPV2,atw.cycle[cycle].expv[EXPV2].exv_Ini_defrost);
                time[cycle].init_step_keep = 120;   //120s wait
                atw.cycle[cycle].main_exv_step_sum = atw.cycle[cycle].expv[EXPV1].exv_Ini_defrost + atw.cycle[cycle].expv[EXPV2].exv_Ini_defrost;
            }
            else if ((cycle_step == CYCLE_STATUS_START2) && (last_step != CYCLE_STATUS_START2))//cycle启动2,调用初始开度
            // else if (((cycle_step == CYCLE_STATUS_START2) && (last_step != CYCLE_STATUS_START2))//cycle启动2,调用初始开度
                    // || ((cycle_step == CYCLE_STATUS_START_DEFROST_DONE) && (last_step != CYCLE_STATUS_START_DEFROST_DONE)))//除霜后启动,调用初始开度
            {//进入
                if(GetCycleRealRunMode(cycle) == CYCLE_MODE_COOL)
                {
                    ExpvMainGetInitStepCool(cycle);
                    
                    ExpvMainControl(cycle,EXPV1,atw.cycle[cycle].expv[EXPV1].exv_Ini_c);
                    ExpvMainControl(cycle,EXPV2,atw.cycle[cycle].expv[EXPV2].exv_Ini_c);
                    time[cycle].init_step_keep = 120;   //120s wait
                    atw.cycle[cycle].main_exv_step_sum = atw.cycle[cycle].expv[EXPV1].exv_Ini_c + atw.cycle[cycle].expv[EXPV2].exv_Ini_c;
                }
                else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)
                {
                    ExpvMainGetInitStepHeat(cycle);
                    ExpvMainControl(cycle,EXPV1,atw.cycle[cycle].expv[EXPV1].exv_Ini_h);
                    ExpvMainControl(cycle,EXPV2,atw.cycle[cycle].expv[EXPV2].exv_Ini_h);
                    time[cycle].init_step_keep = 150;   //150s wait
                    atw.cycle[cycle].main_exv_step_sum = atw.cycle[cycle].expv[EXPV1].exv_Ini_h + atw.cycle[cycle].expv[EXPV2].exv_Ini_h;
                }
            }
            else//运行过程
            {
                if (ExpvMainCheckInitStepQuit(cycle) == TRUE)//初始开度退出条件
                {
                    ExpvMainNormalRun(cycle);
                }
            }            
        }
            break;
        default:
            break;
    }
}




