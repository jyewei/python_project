/***********************************************************************
@file   : CompProtect.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "CompProtect.h"
#include "Data.h"
#include "ChillerCycle.h"
#include "CycleStateControl.h"
#include "TimerCounter.h"
#include "CompControl.h"







enum
{
    P01,
    P02,
    P03_1,
    P03_2,
    P03_3,
    P04,
    P05,
    P06,
    P0A,
    P0D,
    P09, 
    P_MAX

};

enum
{
    STATE_NORMAL, 
    STATE_A,
    STATE_B,
    STATE_C,
    STATE_D,
    STATE_E,
    STATE_MAX
}PdStateEnum;

enum
{
    I1,
    I2,
    I3,
    I4,
    I_MAX

}TLimitEnum;

typedef enum
{
    T1,
    T2,
    T3,
    T4,
    T5,
    T_MAX

}TempProtectEnum;

enum
{
    UNIT_SA_PREM,
    UNIT_SA_MASS,
    UNIT_CA_PREM,
    UNIT_CA_MASS,
    UNIT_MAX
    
}UnitType;

enum
{
    UNIT_CAP_45,
    UNIT_CAP_65,
    UNIT_CAP_130,
    UNIT_CAP_160,
    UNIT_CAP_260,
    UNIT_CAP_300,
    UNIT_CAP_MAX
    
}UnitType;


static uint8_t prot_status[P_MAX];



/************************************************************************
@name  	: CompGetPressureRatio
@brief 	: 
@param 	: None
@return	: None
@note   : 压力比的计算
            高压压力Pd/低压压力Ps,取10倍值
*************************************************************************/
uint8_t CompGetPressureRatio(uint8_t cycle,uint8_t comp)
{
    int16_t Pd = atw.cycle[cycle].comp[comp].local.Pd;
    int16_t Ps = atw.cycle[cycle].comp[comp].local.Ps;
    float temp1,temp2,res;

    temp1 = Pd *10;
    temp2 = Ps;

    if (temp2 != 0)
    {
        res = temp1/temp2;
    }
    return ((uint8_t)res);
}

//===压缩机保护控制
/************************************************************************
@name  	: CompGetPressureRatioHighState
@brief 	: 高压力比控制
@param 	: None
@return	: None
@note   : 7.1.8.1 压缩机压比控制P01
            (1)高压力比控制
            a.频率强制下降时,0.25Hz/s   //TODO 最小单位
            b.cycle启动1~cycle启动2,cycle除霜中,cycle差压控制中,不受高压力比控制
*************************************************************************/
static uint8_t CompGetPressureRatioHighState(uint8_t cycle,uint8_t comp)
{
    uint8_t status = prot_status[P01];
    uint8_t ratio = CompGetPressureRatio(cycle,comp); //压力比
    uint8_t cycle_step = atw.cycle[cycle].work_step;
    int16_t tam_temp = atw.unit.table.io.Tam;

    if ((cycle_step == CYCLE_STATUS_START1)
        || (cycle_step == CYCLE_STATUS_START2)
        || (cycle_step == CYCLE_STATUS_DIFFER_PRESSURE)
        || (cycle_step == CYCLE_STATUS_DEFROST_RUN))
    {
        status = PROT_NULL;
    }
    else
    {
        if (tam_temp <= -150)
        {
            if (status == PROT_NULL)//正常控制
            {
                if (ratio >= 98)
                {
                    status = PROT_FREQ_FORCED_DROP;//频率强制下降
                }
            }
            else if (status == PROT_FREQ_FORCED_DROP)
            {
                if (ratio <= 95)//
                {
                    status == PROT_FREQ_FORBID_RISE;
                }
            }
            else if (status == PROT_FREQ_FORBID_RISE)
            {
                if (ratio >= 98)
                {
                    status = PROT_FREQ_FORCED_DROP;//频率强制下降
                }
                else if (ratio <= 90)
                {
                    status == PROT_NULL;
                }
            }
        }
        else
        {
            if (status == PROT_NULL)//正常控制
            {
                if (ratio >= 80)
                {
                    status = PROT_FREQ_FORCED_DROP;//频率强制下降
                }
            }
            else if (status == PROT_FREQ_FORCED_DROP)
            {
                if (ratio <= 75)//
                {
                    status == PROT_FREQ_FORBID_RISE;
                }
            }
            else if (status == PROT_FREQ_FORBID_RISE)
            {
                if (ratio >= 80)
                {
                    status = PROT_FREQ_FORCED_DROP;//频率强制下降
                }
                else if (ratio <= 70)
                {
                    status == PROT_NULL;
                }
            }
        }
    }
    prot_status[P01] = status;
    return status;
}

/************************************************************************
@name  	: CompGetPressureRatioLowState
@brief 	: 低压力比控制
@param 	: None
@return	: None
@note   : 7.1.8.1 压缩机压比控制P01
            (2)低压力比控制
            a.频率强制下降时,0.25Hz/s   //TODO 最小单位
            b.cycle启动1~cycle启动2,cycle除霜中,cycle差压控制中,不受高压力比控制
*************************************************************************/
static uint8_t CompGetPressureRatioLowState(uint8_t cycle,uint8_t comp)
{
    uint8_t status = prot_status[P01];
    uint8_t ratio = CompGetPressureRatio(cycle,comp); //压力比
    uint8_t cycle_step = atw.cycle[cycle].work_step;
    uint8_t unit_state = atw.unit.table.init.dsw1.bit.unit_work_state; 

    if ((cycle_step == CYCLE_STATUS_START1)
        || (cycle_step == CYCLE_STATUS_START2)
        || (cycle_step == CYCLE_STATUS_DIFFER_PRESSURE)
        || (cycle_step == CYCLE_STATUS_DEFROST_RUN)
        || (unit_state == UNIT_STATE_TEST))//测试模式
    {
        status = PROT_NULL;
    }
    else
    {
        if (status == PROT_NULL)
        {
            if (ratio <= 16)
            {
                status = PROT_FREQ_FORBID_DROP;
            }
        }
        else if (status == PROT_FREQ_FORCED_RISE)
        {
            if (ratio >= 16)
            {
                status = PROT_FREQ_FORBID_DROP;
            }
        }
        else if (status == PROT_FREQ_FORBID_DROP)
        {
            if (ratio >= 17)
            {
                status = PROT_NULL;
            }
            else if (ratio <= 15)
            {
                status = PROT_FREQ_FORCED_RISE;
            }
        }
    }
    prot_status[P01] = status;
    return status;
}

/************************************************************************
@name  	: CompGetPressureRatioProtectState
@brief 	: 低压力比控制
@param 	: None
@return	: None
@note   : 7.1.8.1 压缩机压比控制P01
            (1)高压力比控制
            (2)低压力比控制
*************************************************************************/
static uint8_t CompGetPressureRatioProtectState(uint8_t cycle,uint8_t comp)
{
    uint8_t status = PROT_NULL;

    if (CompGetPressureRatioHighState(cycle,comp) == PROT_NULL)
    {
        status = CompGetPressureRatioLowState(cycle,comp);
    }
    
    return status;
}

/************************************************************************
@name  	: CompGetPdProtectState
@brief 	: 低压力比控制
@param 	: None
@return	: None
@note   : 7.1.8.2 Pd保护控制P02
            保证cycle排气压力在合理范围内
            cycle启动1,不执行
*************************************************************************/
static uint8_t CompGetPdProtectState(uint8_t cycle,uint8_t comp)
{
    uint8_t status = prot_status[P02];
    int16_t Pd = atw.cycle[cycle].comp[comp].local.Pd;
    uint8_t cycle_step = atw.cycle[cycle].work_step;

    if (cycle_step == CYCLE_STATUS_START1)
    {
        status = PROT_NULL;
    }
    else 
    {
        if (GetCycleRealRunMode(cycle) == CYCLE_MODE_COOL)//制冷
        {
            if (status == PROT_NULL)
            {
                if (Pd >= 3600)
                {
                    status = PROT_FREQ_FORCED_DROP;
                }
            }
            else if (status == PROT_FREQ_FORCED_DROP)
            {
                if (Pd <= 3550)
                {
                    status = PROT_FREQ_FORBID_RISE;
                }
            }
            else if (status == PROT_FREQ_FORBID_RISE)//
            {
                if (Pd <= 3450)
                {
                    status = PROT_NULL;
                }
                else if (Pd >= 3600)
                {
                    status = PROT_FREQ_FORCED_DROP;
                }
            }
        }
        else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)//制热
        {
            if (status == PROT_NULL)
            {
                if (Pd >= 4200)
                {
                    status = PROT_FREQ_FORCED_DROP;
                }
            }
            else if (status == PROT_FREQ_FORCED_DROP)
            {
                if (Pd <= 4100)
                {
                    status = PROT_FREQ_FORBID_RISE;
                }
            }
            else if (status == PROT_FREQ_FORBID_RISE)//
            {
                if (Pd <= 3900)
                {
                    status = PROT_NULL;
                }
                else if (Pd >= 4200)
                {
                    status = PROT_FREQ_FORCED_DROP;
                }
            }
        }
    }
    prot_status[P02] = status;
    return status;
}

/************************************************************************
@name  	: CompGetCurrentProtectState1Ilimit
@brief 	: 
@param 	: None
@return	: None
@note   : 
*************************************************************************/
static uint8_t CompGetCurrentProtectState1Ilimit(uint8_t cycle,uint8_t comp,int16_t *limit)//
{
    const uint16_t Ku2_tab[UNIT_MAX][UNIT_CAP_MAX] = //0.01A
    {//  45     65      130     160     260     300                  
        {5100,  5100,   5100,   5100,   5100,   5100},  //SA Premium
        {5100,  5100,   5100,   5100,   5100,   5100},  //SA Mass
        {5100,  5100,   5100,   5100,   5100,   5100},  //CA Premium
        {5100,  5100,   5100,   5100,   5100,   5100}   //CA Mass
    };

    uint8_t  temp_type = atw.unit.table.init.dsw1.bit.temp_type;
    uint8_t  capacity = atw.unit.table.init.dsw1.bit.capacity;
    uint8_t  tier = atw.unit.table.init.dsw1.bit.tier_type;
    uint16_t I_Kideg[I_MAX];
    uint8_t  unit,unit_cap;
    uint16_t ku2,Kideg;
//get SA/CA/TIER1/TIER3
    if (temp_type == TEMP_SA)
    {
        if (tier == TIER_1)
            unit = UNIT_SA_PREM;
        else unit = UNIT_SA_MASS;
    }
    else
    {
        if (tier == TIER_1)
            unit = UNIT_CA_PREM;
        else unit = UNIT_CA_MASS;
    }
//get capacity
    switch (capacity)
    {
        case CAPACITY_45KW:  unit_cap = UNIT_CAP_45;    break;
        case CAPACITY_65KW:  unit_cap = UNIT_CAP_65;    break;
        case CAPACITY_130KW: unit_cap = UNIT_CAP_130;   break;
        case CAPACITY_160KW: unit_cap = UNIT_CAP_160;   break;
        case CAPACITY_260KW: unit_cap = UNIT_CAP_260;   break;
        case CAPACITY_300KW: unit_cap = UNIT_CAP_300;   break;
        default:             unit_cap = UNIT_CAP_65;    break;
    }
//get ku2
    ku2 = Ku2_tab[unit][unit_cap];
//get kideg
    if (atw.cycle[cycle].comp[comp].local.voltage_unbalance == TRUE)//电源电压不平衡
    {
        Kideg = 700;
    }
    else if (atw.cycle[cycle].comp[comp].local.Iinv2_retreat == TRUE)//缩退控制中
    {
        Kideg = 100;
    }
    else
    {
        Kideg = 0;
    }

    limit[I1] = ku2 - 1500 - Kideg;
    limit[I2] = ku2 - 1000 - Kideg;
    limit[I3] = ku2 - 500 - Kideg;
    limit[I4] = ku2 - Kideg;

}

/************************************************************************
@name  	: CompGetCurrentProtectState1
@brief 	: 变频2次电流保护
@param 	: None
@return	: None
@note   : 7.1.8.3 电流保护与限制 P03-1
            变频2次电流保护
            变频2次电流Iinv2的保护缩退控制：
            当发生交流输入过流保护故障31C/31d以后，压缩机满足 3分钟保护延时控制等条件后，重新启动时
        (1) 将保护控制的｢变频2次电流Iinv2的保护控制｣转换到｢变频2次电流Iinv2的保护缩退控制｣
        (2) 缩退控制的解除条件
        　　・当｢电流保护缩退控制｣的保护控制超过90分钟没有动作时
        　　・Cycle关机
            ・当模块单元的电源被切断时

*************************************************************************/
static uint8_t CompGetCurrentProtectState1(uint8_t cycle,uint8_t comp)//TODO 缩退控制
{
    uint8_t status = prot_status[P03_1];
    uint8_t cycle_step = atw.cycle[cycle].work_step;
    uint16_t Iinv2 = atw.cycle[cycle].comp[comp].link.state.current;
    int16_t  I_limit[I_MAX];//TODO to get value

    CompGetCurrentProtectState1Ilimit(cycle,comp,I_limit);//获取I限值
    if (status != PROT_NULL)
    {
        if((Iinv2 <= I_limit[I1] ) && ( Iinv2 >= I_limit[I2]))
        {
            CompSetInv2CheckTimer(cycle,comp,300);
        }
    }

    if (((Iinv2 < I_limit[I2]) && (CompGetInv2CheckTimer(cycle,comp) == 0))//持续5mins在I1~I2之间
        || ((cycle_step == CYCLE_STATUS_START1)))
    {
        status = PROT_NULL;
    }
    else 
    {
        if (status == PROT_NULL)
        {
            if (Iinv2 >= I_limit[I2])
            {
                CompSetInv2CheckTimer(cycle,comp,300); //进入时赋初值
                status = PROT_FREQ_FORBID_RISE;
            }
        }
        else if (status == PROT_FREQ_FORBID_RISE)
        {
            if (Iinv2 >= I_limit[I4])
            {
                status = PROT_FREQ_FORCED_DROP;
            }
            else if (Iinv2 <= I_limit[I1])
            {
                status = PROT_NULL;
            }
        }
        else if (status == PROT_FREQ_FORCED_DROP)
        {
            if (Iinv2 <= I_limit[I3])
            {
                status = PROT_FREQ_FORBID_RISE;
            }
        }
    }
    prot_status[P03_1] = status;
    return status;
}

/************************************************************************
@name  	: CompGetCurrentProtectState2Ilimit
@brief 	: 
@param 	: None
@return	: None
@note   : 
*************************************************************************/
static uint8_t CompGetCurrentProtectState2Ilimit(uint8_t cycle,uint8_t comp,int16_t *limit,uint16_t total)//TODO to get value
{
    uint16_t Kus_slv = 5700;

    if (total == 0)
    {
        Kus_slv = 5100;
    }
    
    limit[I1] = Kus_slv - 1500;
    limit[I2] = Kus_slv - 500;
    limit[I3] = Kus_slv - 500;
    limit[I4] = Kus_slv;

}


/************************************************************************
@name  	: CompGetCurrentProtectState2
@brief 	: 变频1次电流保护
@param 	: None
@return	: None
@note   : 7.1.8.3 电流保护与限制 P03
            变频1次电流保护
            a.cycle启动1,不执行
*************************************************************************/
static uint8_t CompGetCurrentProtectState2(uint8_t cycle,uint8_t comp)
{
    uint8_t status = prot_status[P03_2];
    uint16_t IDC_comp = atw.cycle[cycle].comp[comp].link.state.current;
    uint16_t IDC_fan1 = atw.cycle[cycle].fan[FAN_1].link.state.foc_phase_current;
    uint16_t IDC_fan2 = atw.cycle[cycle].fan[FAN_2].link.state.foc_phase_current;
    uint16_t IDC_fan_total = IDC_fan1 + IDC_fan2;
    uint32_t Iinv_step_max,Iinv_step;
    int16_t I_limit[I_MAX];

    CompGetCurrentProtectState2Ilimit(cycle,comp,I_limit,IDC_fan_total);
    Iinv_step = (IDC_comp + IDC_fan_total)*816/1000; //TODO 待测试确认
    Iinv_step_max = Iinv_step;

    if (status == PROT_NULL)
    {
        if (Iinv_step_max >= I_limit[I2])
        {
            status = PROT_FREQ_FORBID_RISE;
        }
    }
    else if (status == PROT_FREQ_FORBID_RISE)
    {
        if (Iinv_step_max <= I_limit[I1])
        {
            status = PROT_NULL;
        }
        else if (Iinv_step_max >= I_limit[I4])
        {
            status = PROT_FREQ_FORCED_DROP;
        }
    }
    else if (status == PROT_FREQ_FORCED_DROP)
    {
        if (Iinv_step_max <= I_limit[I3])
        {
            status = PROT_FREQ_FORBID_RISE;
        }
    }
    prot_status[P03_2] = status;
    return status;
}

/************************************************************************
@name  	: CompGetCurrentProtectState3Ilimit
@brief 	: 
@param 	: None
@return	: None
@note   : 
*************************************************************************/
static uint8_t CompGetCurrentProtectState3Ilimit(uint8_t cycle,uint8_t comp,int16_t *limit)
{
    const uint16_t Kua_slv_tab[UNIT_MAX][UNIT_CAP_MAX] = //0.01A
    {//  45     65      130     160     260     300                  
        {4700,  4700,   9100,   9100,   9100,   9100},  //SA Premium
        {4700,  4700,   9100,   9100,   9100,   9100},  //SA Mass
        {4700,  4700,   9100,   9100,   9100,   9100},  //CA Premium
        {4700,  4700,   9100,   9100,   9100,   9100}   //CA Mass
    };

    uint8_t  temp_type = atw.unit.table.init.dsw1.bit.temp_type;
    uint8_t  capacity = atw.unit.table.init.dsw1.bit.capacity;
    uint8_t  tier = atw.unit.table.init.dsw1.bit.tier_type;
    uint16_t I_Kideg[I_MAX];
    uint8_t  unit,unit_cap;
    uint16_t ku_all,kua_slv,kv;
//get SA/CA/TIER1/TIER3
    if (temp_type == TEMP_SA)
    {
        if (tier == TIER_1)
            unit = UNIT_SA_PREM;
        else unit = UNIT_SA_MASS;
    }
    else
    {
        if (tier == TIER_1)
            unit = UNIT_CA_PREM;
        else unit = UNIT_CA_MASS;
    }
//get capacity
    switch (capacity)
    {
        case CAPACITY_45KW:  unit_cap = UNIT_CAP_45;    break;
        case CAPACITY_65KW:  unit_cap = UNIT_CAP_65;    break;
        case CAPACITY_130KW: unit_cap = UNIT_CAP_130;   break;
        case CAPACITY_160KW: unit_cap = UNIT_CAP_160;   break;
        case CAPACITY_260KW: unit_cap = UNIT_CAP_260;   break;
        case CAPACITY_300KW: unit_cap = UNIT_CAP_300;   break;
        default:             unit_cap = UNIT_CAP_65;    break;
    }
//get ku2
    kua_slv = Kua_slv_tab[unit][unit_cap];
    kv = 1;
    ku_all = kua_slv*kv;

    limit[I1] = ku_all - 1500;
    limit[I2] = ku_all - 500;
    limit[I3] = ku_all - 500;
    limit[I4] = ku_all;
}

/************************************************************************
@name  	: CompGetCurrentProtectState3
@brief 	: 单元变频总电流保护
@param 	: None
@return	: None
@note   : 7.1.8.3 电流保护与限制 P03
            单元变频总电流保护         
            a.cycle启动1,不执行
*************************************************************************/
static uint8_t CompGetCurrentProtectState3(uint8_t cycle,uint8_t comp)
{
    uint8_t status = prot_status[P03_3];
    uint32_t Iinv_slv;
    uint16_t I_limit[I_MAX];//TODO to get value

    CompGetCurrentProtectState3Ilimit(cycle,comp,I_limit);
    if (0)//TODO 过度控制?
    {
        status = PROT_NULL;
    }
    else
    {
        if (status == PROT_NULL)
        {
            if (Iinv_slv >= I_limit[I2])
            {
                status = PROT_FREQ_FORBID_RISE;
            }
        }
        else if (status == PROT_FREQ_FORBID_RISE)
        {
            if (Iinv_slv <= I_limit[I1])
            {
                status = PROT_NULL;
            }
            else if (Iinv_slv >= I_limit[I4])
            {
                status = PROT_FREQ_FORCED_DROP;
            }
        }
        else if (status == PROT_FREQ_FORCED_DROP)
        {
            if (Iinv_slv <= I_limit[I3])
            {
                status = PROT_FREQ_FORBID_RISE;
            }
        }
    }
    prot_status[P03_3] = status;
    return status;
}

/************************************************************************
@name  	: CompGetFinTempProtectState
@brief 	: 低压力比控制
@param 	: None
@return	: None
@note   : 7.1.8.4 变频翅片温度过高保护控制 P04
            a.cycle启动1,不执行
*************************************************************************/
static void CompGetFinTempProtectTlimit(uint8_t cycle,uint8_t comp,int16_t *Tlimit)//TODO 参数确定
{
    typedef enum
    {
        INV_TYPEA_NORMAL,   //通常时
        INV_TYPEA_RETREAT,  //频率退缩
        INV_TYPEB_NORMAL,
        INV_TYPEB_RETREAT,  //频率退缩
        INV_TYPE_MAX
    }InverterTypeEnum;


    uint16_t temp_protect_table[INV_TYPE_MAX][T_MAX] = 
    {   //T1    T2      T3      T4      T5
        {960,   980,    980,    1000,   1030},
        {940,   960,    960,    980,    1010},
        {1010,  1030,   1030,   1050,   1080},
        {990,   1010,   1010,   1030,   1060},
    };

    Tlimit[T1] = temp_protect_table[INV_TYPEA_NORMAL][T1];
    Tlimit[T2] = temp_protect_table[INV_TYPEA_NORMAL][T2];
    Tlimit[T3] = temp_protect_table[INV_TYPEA_NORMAL][T3];
    Tlimit[T4] = temp_protect_table[INV_TYPEA_NORMAL][T4];
    Tlimit[T5] = temp_protect_table[INV_TYPEA_NORMAL][T5];
}

/************************************************************************
@name  	: CompGetFinTempProtectState
@brief 	: 低压力比控制
@param 	: None
@return	: None
@note   : 7.1.8.4 变频翅片温度过高保护控制 P04
            a.cycle启动1,不执行
*************************************************************************/
static uint8_t CompGetFinTempProtectState(uint8_t cycle,uint8_t comp)
{
    uint8_t status = prot_status[P04];
    int16_t fin_temp = atw.cycle[cycle].comp[comp].link.state.PIM_temp; //TODO 
    uint8_t cycle_step = atw.cycle[cycle].work_step;
    int16_t T_limit[T_MAX];

    CompGetFinTempProtectTlimit(cycle,comp,&T_limit);
//T1<Tf <T2状态持续5mins,退出控制
    if (status != PROT_NULL)
    {
        if ((T_limit[T2] <= fin_temp) || (fin_temp <= T_limit[T1]))
        {
            TimerSet(NO_CYCLE1_FIN_TEMP_PROTECT+cycle,300);//5mins,300s
        }
    }
//
    atw.cycle[cycle].comp[comp].local.P04_T2 = T_limit[T2];//fan use the value 
    atw.cycle[cycle].comp[comp].local.P04_T4 = T_limit[T4];

    if ((cycle_step == CYCLE_STATUS_START1)
        || ((fin_temp < T_limit[T2]) && (TimerCheck(NO_CYCLE1_FIN_TEMP_PROTECT+cycle) == TRUE)))//持续5mins
    {
        status = PROT_NULL;
    }
    else 
    {
        if (status == PROT_NULL)
        {
            if (fin_temp >= T_limit[T2])
            {
                TimerSet(NO_CYCLE1_FIN_TEMP_PROTECT+cycle,300);//5mins,300s 进入时
                status = PROT_FREQ_FORBID_RISE;
            }
        }
        else if (status == PROT_FREQ_FORCED_DROP)
        {
            if (fin_temp >= T_limit[T3])
            {
                status = PROT_FREQ_FORBID_RISE;
            }
        }
        else if (status == PROT_FREQ_FORBID_RISE)
        {
            if (fin_temp <= T_limit[T1])
            {
                status = PROT_NULL;
            }
            else if (fin_temp >= T_limit[T4])
            {
                status = PROT_FREQ_FORCED_DROP;
            }
        } 
    }
    prot_status[P04] = status;
    return status;
}

/************************************************************************
@name  	: CompGetPdProtectState
@brief 	: 低压力比控制
@param 	: None
@return	: None
@note   : 7.1.8.5 Td保护控制P05
            cycle启动1,不执行
*************************************************************************/
static uint8_t CompGetTdProtectState(uint8_t cycle,uint8_t comp)
{
    uint8_t status = prot_status[P05];
    int16_t Td = atw.cycle[cycle].comp[comp].local.Td;
    uint8_t cycle_step = atw.cycle[cycle].work_step;
    int16_t T_limit[3];

    if (1)//通常控制 //TODO
    {
        T_limit[T1] = 1050;
        T_limit[T2] = 1100;
        T_limit[T3] = 1120;
    }
    else//退缩控制
    {
        T_limit[T1] = 1000;
        T_limit[T2] = 1080;
        T_limit[T3] = 1100;
    }
    
//
    if (cycle_step == CYCLE_STATUS_START1)
    {
        status = PROT_NULL;
    }
    else 
    {
        if (status == PROT_NULL)
        {
            if (Td >= T_limit[T3])
            {
                status = PROT_FREQ_FORCED_DROP;
            }
        }
        else if (status == PROT_FREQ_FORCED_DROP)
        {
            if (Td <= T_limit[T2])
            {
                status = PROT_FREQ_FORBID_RISE;
            }
        }
        else if (status == PROT_FREQ_FORBID_RISE)
        {
            if (Td <= T_limit[T1])
            {
                status = PROT_NULL;
            }
            else if (Td >= T_limit[T3])
            {
                status = PROT_FREQ_FORCED_DROP;
            }
        }
    }
    prot_status[P05] = status;
    return status;
}

/************************************************************************
@name  	: CompGetPsProtectJudgeState
@brief 	: 低压力比控制获取P1,P2,P3的值
@param 	: None
@return	: None
@note   : 
*************************************************************************/
static void CompGetPsProtectJudgeState(uint8_t cycle,uint8_t comp,uint16_t *p1,uint16_t *p2,uint16_t *p3)
{
    enum
    {
        SA_C_START,
        SA_C_RUN,
        SA_H_DEFROST,
        SA_H_START2,
        SA_OTHER,
        SA_ADD_C_STATR,
        SA_ADD_C_RUN,
        SA_ADD_H_DEFROST,
        SA_ADD_H_START2,
        SA_ADD_OTHER,
        CA_C_START,
        CA_C_RUN,
        CA_H_DEFROST,
        CA_H_START2,
        CA_OTHER,
        TYPE_MAX

    }PsStatusType;

    enum
    {
        P1,
        P2,
        P3,
        P_MAX

    }PsType;


    uint8_t temp_type = atw.unit.table.init.dsw1.bit.temp_type;
    uint8_t refrigent_type = atw.unit.table.init.dsw1.bit.refrigent_type;
    uint8_t cycle_step = atw.cycle[cycle].work_step;
    uint16_t p1_value,p2_value,p3_value;

    uint16_t temp_protect_table[TYPE_MAX][P_MAX] =
    {   //P1    P2      P3     
        {620,   630,    650},
        {650,   660,    670},
        {70,    100,    120},
        {100,   120,    150},
        {120,   180,    220},
        {130,   180,    200},
        {450,   460,    470},
        {70,    100,    120},
        {100,   120,    150},
        {120,   180,    220},
        {620,   630,    650},
        {650,   660,    670},
        {70,    100,    120},
        {100,   120,    150},
        {120,   150,    170}
    };

    if (temp_type == TEMP_SA)//标准热泵
    {
        if (refrigent_type == STANDARD_WATER)//
        {
            if (GetCycleRealRunMode(cycle) == CYCLE_MODE_COOL)//制冷
            {
                if (((cycle_step == CYCLE_STATUS_START1) || (cycle_step == CYCLE_STATUS_START2)))
                {

                    p1_value = temp_protect_table[SA_C_START][P1];
                    p2_value = temp_protect_table[SA_C_START][P2];
                    p3_value = temp_protect_table[SA_C_START][P3];
                }
                else if ((cycle_step == CYCLE_STATUS_RUN_HEAT) || (cycle_step == CYCLE_STATUS_RUN_COOL) || (cycle_step == CYCLE_STATUS_RUN_OIL_RETURN))
                {
                    p1_value = temp_protect_table[SA_C_RUN][P1];
                    p2_value = temp_protect_table[SA_C_RUN][P2];
                    p3_value = temp_protect_table[SA_C_RUN][P3];
                }
                else
                {
                    p1_value = temp_protect_table[SA_OTHER][P1];
                    p2_value = temp_protect_table[SA_OTHER][P2];
                    p3_value = temp_protect_table[SA_OTHER][P3];
                }
            }
            else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)//制热
            {
                if (cycle_step == CYCLE_STATUS_START2)
                {
                    p1_value = temp_protect_table[SA_H_START2][P1];
                    p2_value = temp_protect_table[SA_H_START2][P2];
                    p3_value = temp_protect_table[SA_H_START2][P3];
                }
                else if (cycle_step == CYCLE_STATUS_DEFROST_RUN)
                {
                    p1_value = temp_protect_table[SA_H_DEFROST][P1];
                    p2_value = temp_protect_table[SA_H_DEFROST][P2];
                    p3_value = temp_protect_table[SA_H_DEFROST][P3];
                }
                else
                {
                    p1_value = temp_protect_table[SA_OTHER][P1];
                    p2_value = temp_protect_table[SA_OTHER][P2];
                    p3_value = temp_protect_table[SA_OTHER][P3];
                }
            }
            else 
            {
                p1_value = temp_protect_table[SA_OTHER][P1];
                p2_value = temp_protect_table[SA_OTHER][P2];
                p3_value = temp_protect_table[SA_OTHER][P3];
            }
            
        }
        else//加防冻液
        {
            if (GetCycleRealRunMode(cycle) == CYCLE_MODE_COOL)//制冷
            {
                if (((cycle_step == CYCLE_STATUS_START1) || (cycle_step == CYCLE_STATUS_START2)))
                {
                    p1_value = temp_protect_table[SA_ADD_C_STATR][P1];
                    p2_value = temp_protect_table[SA_ADD_C_STATR][P2];
                    p3_value = temp_protect_table[SA_ADD_C_STATR][P3];
                }
                else if ((cycle_step == CYCLE_STATUS_RUN_HEAT) || (cycle_step == CYCLE_STATUS_RUN_COOL) || (cycle_step == CYCLE_STATUS_RUN_OIL_RETURN))
                {
                    p1_value = temp_protect_table[SA_ADD_C_RUN][P1];
                    p2_value = temp_protect_table[SA_ADD_C_RUN][P2];
                    p3_value = temp_protect_table[SA_ADD_C_RUN][P3];
                }
                else
                {
                    p1_value = temp_protect_table[SA_ADD_OTHER][P1];
                    p2_value = temp_protect_table[SA_ADD_OTHER][P2];
                    p3_value = temp_protect_table[SA_ADD_OTHER][P3];
                }
            }
            else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)//制热
            {
                if (cycle_step == CYCLE_STATUS_START2)
                {
                    p1_value = temp_protect_table[SA_ADD_H_START2][P1];
                    p2_value = temp_protect_table[SA_ADD_H_START2][P2];
                    p3_value = temp_protect_table[SA_ADD_H_START2][P3];
                }
                else if (cycle_step == CYCLE_STATUS_DEFROST_RUN)
                {
                    p1_value = temp_protect_table[SA_ADD_H_DEFROST][P1];
                    p2_value = temp_protect_table[SA_ADD_H_DEFROST][P2];
                    p3_value = temp_protect_table[SA_ADD_H_DEFROST][P3];
                }
                else
                {
                    p1_value = temp_protect_table[SA_ADD_OTHER][P1];
                    p2_value = temp_protect_table[SA_ADD_OTHER][P2];
                    p3_value = temp_protect_table[SA_ADD_OTHER][P3];
                }
            }
            else 
            {
                p1_value = temp_protect_table[SA_ADD_OTHER][P1];
                p2_value = temp_protect_table[SA_ADD_OTHER][P2];
                p3_value = temp_protect_table[SA_ADD_OTHER][P3];
            }
        }
    }
    else//低温热泵
    {
        if (GetCycleRealRunMode(cycle) == CYCLE_MODE_COOL)//制冷
        {
            if (((cycle_step == CYCLE_STATUS_START1) || (cycle_step == CYCLE_STATUS_START2)))
            {
                p1_value = temp_protect_table[CA_C_START][P1];
                p2_value = temp_protect_table[CA_C_START][P2];
                p3_value = temp_protect_table[CA_C_START][P3];
            }
            else if ((cycle_step == CYCLE_STATUS_RUN_HEAT) || (cycle_step == CYCLE_STATUS_RUN_COOL) || (cycle_step == CYCLE_STATUS_RUN_OIL_RETURN))
            {
                p1_value = temp_protect_table[CA_C_RUN][P1];
                p2_value = temp_protect_table[CA_C_RUN][P2];
                p3_value = temp_protect_table[CA_C_RUN][P3];
            }
            else
            {
                p1_value = temp_protect_table[CA_OTHER][P1];
                p2_value = temp_protect_table[CA_OTHER][P2];
                p3_value = temp_protect_table[CA_OTHER][P3];
            }
        }
        else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)//制热
        {
            if (cycle_step == CYCLE_STATUS_START2)
            {
                p1_value = temp_protect_table[CA_H_START2][P1];
                p2_value = temp_protect_table[CA_H_START2][P2];
                p3_value = temp_protect_table[CA_H_START2][P3];
            }
            else if (cycle_step == CYCLE_STATUS_DEFROST_RUN)
            {
                p1_value = temp_protect_table[CA_H_DEFROST][P1];
                p2_value = temp_protect_table[CA_H_DEFROST][P2];
                p3_value = temp_protect_table[CA_H_DEFROST][P3];
            }
            else
            {
                p1_value = temp_protect_table[CA_OTHER][P1];
                p2_value = temp_protect_table[CA_OTHER][P2];
                p3_value = temp_protect_table[CA_OTHER][P3];
            }
        }
        else 
        {
            p1_value = temp_protect_table[CA_OTHER][P1];
            p2_value = temp_protect_table[CA_OTHER][P2];
            p3_value = temp_protect_table[CA_OTHER][P3];
        } 
    }
    *p1 = p1_value;
    *p2 = p2_value;
    *p3 = p3_value;
    
}
    
/************************************************************************
@name  	: CompGetPsProtectState
@brief 	: 低压力比控制
@param 	: None
@return	: None
@note   : 7.1.8.6 Ps过低保护控制P06
            cycle启动1,不执行
*************************************************************************/
static uint8_t CompGetPsLowProtectState(uint8_t cycle,uint8_t comp)
{
    uint8_t status = prot_status[P06];
    int16_t Ps = atw.cycle[cycle].comp[comp].local.Ps;
    uint8_t cycle_step = atw.cycle[cycle].work_step;
    uint16_t P1,P2,P3;

    if (cycle_step == CYCLE_STATUS_START1)
    {
        status = PROT_NULL;
    }
    else
    {
        CompGetPsProtectJudgeState(cycle,comp,&P1,&P2,&P3);
        if (status == PROT_NULL)
        {
            if (Ps <= P1)
            {
                status = PROT_FREQ_FORCED_DROP;
            }
        }
        else if (status == PROT_FREQ_FORCED_DROP)
        {
            if (Ps >= P2)
            {
                status = PROT_FREQ_FORBID_RISE;
            }
        }
        else if (status == PROT_FREQ_FORBID_RISE)
        {
            if (Ps >= P3)
            {
                status = PROT_NULL;
            }
            else if (Ps <= P1)
            {
                status = PROT_FREQ_FORCED_DROP;
            }
        }
    }
    prot_status[P06] = status;
    return status;
}

/************************************************************************
@name  	: CompGetCapacityProtectState
@brief 	: 
@param 	: None
@return	: None
@note   : 7.1.8.7 机组运转容量限制P0A
*************************************************************************/
static void CompGetCapacityProtectILimit(uint8_t cycle,uint8_t comp,uint16_t *Ilimit)//I 100倍值
{

//TODO 
    Ilimit[I1] = 5000;
    Ilimit[I2] = 5100;
    Ilimit[I3] = 5200;
    Ilimit[I4] = 5300;

}



/************************************************************************
@name  	: CompGetCapacityProtectStatelimit
@brief 	: 
@param 	: None
@return	: None
@note   : 
*************************************************************************/
static uint8_t CompGetCapacityProtectStatelimit(uint8_t cycle,uint8_t comp,int16_t *limit)//TODO to get value
{
    int16_t  I_rate = 10000; //额定电流 //TODO get value
    uint16_t capacity = 0; 

    limit[I1] = 0;
    limit[I2] = 0;
    limit[I3] = 0;
    limit[I4] = 0;

}

/************************************************************************
@name  	: CompGetCapacityProtectState
@brief 	: 
@param 	: None
@return	: None
@note   : 7.1.8.7 机组运转容量限制P0A
*************************************************************************/
static uint8_t CompGetCapacityProtectState(uint8_t cycle,uint8_t comp) //TODO
{
    uint8_t status = prot_status[P0A];
    int16_t I_limit[I_MAX];
    uint16_t I_comp = atw.cycle[cycle].comp[comp].link.state.ac_current_in;//1次电流

    CompGetCapacityProtectStatelimit(cycle,comp,I_limit);

    if (status == PROT_NULL)
    {
        if (I_comp >= I_limit[I2])
        {
            status = PROT_FREQ_FORBID_RISE;
        }
    }
    else if (status == PROT_FREQ_FORBID_RISE)
    {
        if (I_comp >= I_limit[I4])
        {
            status = PROT_FREQ_FORCED_DROP;
        }
        else if (I_comp <= I_limit[I1])
        {
            status = PROT_NULL;
        }
    }
    else if (status == PROT_FREQ_FORCED_DROP)
    {
        if (I_comp <= I_limit[I3])
        {
            status = PROT_FREQ_FORBID_RISE;
        }
    }
    prot_status[P0A] = status;
    return status;
}

/************************************************************************
@name  	: CompGetCapacityProtectState
@brief 	: 
@param 	: None
@return	: None
@note   : 7.1.8.8 Ps过高保护逻辑 P0d
            根据吸入压力传感器的最大值(Psmax)的值,来进行频率控制
            cycle启动1,cycle启动2,cycle除霜中,cycle差压控制中,不执行
*************************************************************************/
static uint8_t CompGetPsHighProtectState(uint8_t cycle,uint8_t comp)
{
    uint8_t status = prot_status[P0D];
    uint8_t cycle_step = atw.cycle[cycle].work_step;
    int16_t Ps = atw.cycle[cycle].comp[comp].local.Ps;

    if ((cycle_step == CYCLE_STATUS_START1)
        || (cycle_step == CYCLE_STATUS_START2)
        || (cycle_step == CYCLE_STATUS_DIFFER_PRESSURE)
        || (cycle_step == CYCLE_STATUS_DEFROST_RUN))
    {
        status = PROT_NULL;
    }
    else
    {
        if (status == PROT_NULL)
        {
            if (Ps >= 1500) //Psat=1500
            {
                status = PROT_FREQ_FORCED_RISE;
            }
        }
        else if (status == PROT_FREQ_FORCED_RISE)
        {
            if (Ps <= 1490)//Psat-0.01 = 1500-10
            {
                status = PROT_FREQ_FORBID_DROP;
            }
        }
        else if (status == PROT_FREQ_FORBID_DROP)
        {
            if (Ps <= 1440)//Psat-0.06 = 1500-60
            {
                status = PROT_NULL;
            }
            else if (Ps >= 1500) //Psat=1500
            {
                status = PROT_FREQ_FORCED_RISE;
            }
        }
    }
    prot_status[P0D] = status;
    return status;
}

/************************************************************************
@name  	: CompGetCapacityProtectState
@brief 	: 
@param 	: None
@return	: None
@note   : 7.1.8.9 Pd过低保护逻辑 P09
            
*************************************************************************/
static uint8_t CompGetPdLowProtectState(uint8_t cycle,uint8_t comp)
{
    uint8_t status = prot_status[P09];
    uint8_t cycle_step = atw.cycle[cycle].work_step;
    int16_t Pd = atw.cycle[cycle].comp[comp].local.Pd;

    if ((cycle_step == CYCLE_STATUS_START1)
        || (cycle_step == CYCLE_STATUS_START2)
        || (cycle_step == CYCLE_STATUS_START_DEFROST_DONE)
        || (cycle_step == CYCLE_STATUS_DEFROST_RUN)
        || (CompProtectP09StepCheck(cycle,comp) == TRUE))
    {
        status = PROT_NULL;
    }
    else
    {
        if (status == PROT_NULL)
        {
            if (Pd <= 1000)
            {
                status = PROT_FREQ_FORCED_RISE;
            }            
        }
        else if (status == PROT_FREQ_FORCED_RISE)
        {
            if (Pd >= 1200)
            {
                status = PROT_FREQ_FORBID_DROP;
            }
        }
        else if (status == PROT_FREQ_FORBID_DROP)
        {
            if (Pd >= 1500)
            {
                status = PROT_NULL;
            }
            else if (Pd <= 1000)
            {
                status = PROT_FREQ_FORCED_RISE;
            }
        }
    }
    prot_status[P09] = status;
    return status;
}

//===频率上下限
/************************************************************************
@name  	: CompGetFimin1
@brief 	: 
@param 	: None
@return	: None
@note   : 7.1.6 inverter频率的下限值Fimin1
*************************************************************************/
static uint16_t CompGetFimin1(uint8_t cycle,uint8_t comp)
{
    enum
    {
        P1,
        P2,
        P3,
        P4,
        P5,
        P6,
        P_MAX
    }PdTypeEnum;

    uint16_t Pd_table[COMP_TYPE_MAX][P_MAX] = 
    {//   P1    P2      P3      P4      P5      P6
        {2800,  2900,   3700,   3800,   2500,   2600},
        {2800,  2900,   3700,   3800,   2500,   2600},
        {2800,  2900,   3700,   3800,   2500,   2600}
    };

    
    int16_t Pd = atw.cycle[cycle].comp[comp].local.Pd;
    uint8_t cycle_step = atw.cycle[cycle].work_step;
    uint8_t unit_work_state = atw.unit.table.init.dsw1.bit.unit_work_state;
    uint8_t ratio = CompGetPressureRatio(cycle,comp);
    static uint8_t state = STATE_D;
    uint16_t res = 0;
    uint8_t comp_type = COMP_110CC;

    if (unit_work_state == UNIT_STATE_TEST)//测试模式
    {
        res = 150;
    }
    else if ((cycle_step == CYCLE_STATUS_START1) || (cycle_step == CYCLE_STATUS_START_DEFROST_DONE))//cycle启动1或除霜后启动
    {
        res = 310;
    }
    else
    {
        if (state == STATE_D)
        {
            if (Pd >= Pd_table[comp_type][P6])
            {
                state = STATE_C;
            }
        }
        else if (state == STATE_C)//
        {
            if ((Pd <= Pd_table[comp_type][P5]) && (ratio < 18))
            {
                state = STATE_D;
            }
            else if (Pd >= Pd_table[comp_type][P2])
            {
                state = STATE_B;
            }
        }
        else if (state == STATE_B)//
        {
            if (Pd >= Pd_table[comp_type][P4])
            {
                state = STATE_A;
            }
            else if (Pd <= Pd_table[comp_type][P1])
            {
                state = STATE_C;
            }
        }
        else if (state == STATE_A)
        {
            if (Pd <= Pd_table[comp_type][P3])
            {
                state = STATE_B;
            }
        }

        switch (state)
        {
            case STATE_A:  res = 400;    break;
            case STATE_B:  res = 300;    break;
            case STATE_C:  res = 250;    break;
            case STATE_D:  res = 150;    break;
            default:    break;
        }
    }
    return res;
}

/************************************************************************
@name  	: CompGetFimax1
@brief 	: 
@param 	: None
@return	: None
@note   : 7.1.6 inverter频率的上限值Fimax1
*************************************************************************/
static uint16_t CompGetFimax1(uint8_t cycle,uint8_t comp)
{
    int16_t Pd = atw.cycle[cycle].comp[comp].local.Pd;
    uint8_t ratio = CompGetPressureRatio(cycle,comp);
    static uint8_t state = STATE_NORMAL;
    uint16_t res = 0;
    // uint8_t comp = COMP_110CC;

    if (state == STATE_NORMAL)
    {
        if (Pd >= 2900)
        {
            state = STATE_B; 
        }
        else
        {
            if (ratio >= 80)
            {
                state = STATE_D;
            }
        }
    }
    else if (state == STATE_B)
    {
        if (Pd >= 3800)
        {
            state = STATE_C; 
        }
        else if (Pd <= 2800)
        {
            state = STATE_NORMAL; 
        }
    }
    else if (state == STATE_C)
    {
        if (Pd <= 3700)
        {
            state = STATE_B; 
        }
    }
    else if (state == STATE_D)
    {
        if (Pd >= 2900)
        {
            state = STATE_B; 
        }
        else if (ratio <= 70)
        {
            state = STATE_NORMAL;
        }
    }

    switch (state)
    {
        case STATE_NORMAL:  res = 1300;    break;
        case STATE_B:       res = 1200;    break;
        case STATE_C:       res = 900;     break;
        case STATE_D:       res = 1200;    break;
        // case STATE_E:       res = 1300;    break;
        default:            res = 1300;    break;
    }

    return res;
}


/************************************************************************
@name  	: CompGetFimin2
@brief 	: 
@param 	: None
@return	: None
@note   : 7.1.7 模块系统运行中的对于压缩机运转频率的限制Fimin2,Fimax2
            当系统存在可以运转的模块单元时,
                Fimin2=min(40Hz, Fi_run+1.0Hz)
*************************************************************************/
static uint16_t CompGetFimin2(uint8_t cycle,uint8_t comp)
{
    uint8_t  state = TRUE; //TODO 系统参数//系统中存在可以运转的单元(处于关机状态)
    uint16_t Fi_run = atw.cycle[cycle].comp[comp].link.state.freq_output;
    uint16_t res;

    if (state == TRUE)
    {
        if (Fi_run + 10 < 400)
        {
            res = Fi_run + 10;
        }
        else
        {
            res = 400;
        }
    }
    else res = 150;

    return res;
}

/************************************************************************
@name  	: CompGetFimax2
@brief 	: 
@param 	: None
@return	: None
@note   : 7.1.7 模块系统运行中的对于压缩机运转频率的限制Fimin2,Fimax2
*************************************************************************/
static uint16_t CompGetFimax2(uint8_t cycle,uint8_t comp)
{
    uint8_t  state = TRUE; //TODO 系统参数//系统中存在可以运转的单元(处于关机状态)
    uint16_t Fi_run = atw.cycle[cycle].comp[comp].link.state.freq_output;
    uint16_t res;

    if (state == TRUE)
    {
        if (Fi_run > 1000)
        {
            res = Fi_run - 10;
        }
        else
        {
            res = 900;
        }
    }
    else res = 1300;

    return res;
}

/************************************************************************
@name  	: CompGetFimin3
@brief 	: 
@param 	: None
@return	: None
@note   : 7.1.8 压缩机保护控制及Fimin3,Fimax3
            cycle启动1结束,压缩机启动成功,进入cycle启动2,此时,以1s为周期,单元进行保护控制动作判断.且保护控制的频率变化速度优先.
            a.当要求压缩机频率强制下降或禁止上升条件成立时,运行频率F_run(n)=min(压缩机换算频率,保护加减载后频率)
            b.当要求压缩机频率强制上升或禁止下降条件成立时,运行频率F_run(n)=max(压缩机换算频率,保护加减载后频率)
            1)压缩机保护控制优先顺序

优先顺序 保护控制名称	                保护控制                        频率调节速度Hz/s
Code		                            
高
	    压力比控制	                    P01	         高压力比保护	     0.25
                                                    低压力比保护	
        Pd保护控制	                    P02	         制冷运转	        1.5
                                                    制热运转	        1.5
        电流保护控制	                P03		                        1.0
        Inverter fin温度过高保护控制	P04		                        0.125
        Td上升保护控制                  P05		                        1.0
        Ps过低保护控制	                P06		                        1.0
        机组运转容量控制	            P0A		                        0.5
        防止Ps过高控制	                P0d		                        0.125
低      防止Pd过低控制	                P09		                        0.125

*************************************************************************/
static uint8_t CompGetProtectState(uint8_t cycle,uint8_t comp)
{

    uint8_t status_tab[P_MAX];
    uint8_t protect_step[P_MAX] = {2,15,10,10,10,1,10,10,5,1,1};
    uint8_t i,j;

    status_tab[P01] = CompGetPressureRatioProtectState(cycle,comp); //
    status_tab[P02] = CompGetPdProtectState(cycle,comp);
    status_tab[P03_1] = CompGetCurrentProtectState1(cycle,comp);
    status_tab[P03_2] = CompGetCurrentProtectState2(cycle,comp);
    status_tab[P03_3] = CompGetCurrentProtectState3(cycle,comp);
    status_tab[P04] = CompGetFinTempProtectState(cycle,comp);
    status_tab[P05] = CompGetTdProtectState(cycle,comp);
    status_tab[P06] = CompGetPsLowProtectState(cycle,comp);
    status_tab[P0A] = CompGetCapacityProtectState(cycle,comp);
    status_tab[P0D] = CompGetPsHighProtectState(cycle,comp);
    status_tab[P09] = CompGetPdLowProtectState(cycle,comp);

    atw.cycle[cycle].comp[comp].local.freq_protect_status = PROT_NULL;
    for ( i = 0; i < P_MAX; i++)
    {
        switch (status_tab[i])
        {
            case PROT_FREQ_FORCED_DROP://上位强制下降
            {
                atw.cycle[cycle].comp[comp].local.freq_protect_status = PROT_FREQ_FORCED_DROP;
                atw.cycle[cycle].comp[comp].local.frq_protect_step = protect_step[i];
                return atw.cycle[cycle].comp[comp].local.freq_protect_status;
            }
                break;
            case PROT_FREQ_FORCED_RISE://上位强制上升
            {
                atw.cycle[cycle].comp[comp].local.freq_protect_status = PROT_FREQ_FORCED_RISE;
                atw.cycle[cycle].comp[comp].local.frq_protect_step = protect_step[i];
                return atw.cycle[cycle].comp[comp].local.freq_protect_status;
            }
                break;
            case PROT_FREQ_FORBID_RISE://禁止上升
            {
                for (j = i; j < P_MAX; j++)
                {
                    if (status_tab[i] == PROT_FREQ_FORCED_DROP)//下位存在频率强制下降
                    {
                        atw.cycle[cycle].comp[comp].local.freq_protect_status = PROT_FREQ_FORCED_DROP;
                        atw.cycle[cycle].comp[comp].local.frq_protect_step = protect_step[j];
                        return atw.cycle[cycle].comp[comp].local.freq_protect_status;
                    }
                }
                //不存在下位频率强制下降
                atw.cycle[cycle].comp[comp].local.freq_protect_status = PROT_FREQ_FORBID_RISE;
                atw.cycle[cycle].comp[comp].local.frq_protect_step = protect_step[i];
                return atw.cycle[cycle].comp[comp].local.freq_protect_status;
            }
                break;
            case PROT_FREQ_FORBID_DROP://禁止下降
            {
                //检测下位是否存在强制下降
                for (j = i; j < P_MAX; j++)
                {
                    if (status_tab[i] == PROT_FREQ_FORCED_DROP)//下位存在频率强制下降
                    {
                        atw.cycle[cycle].comp[comp].local.freq_protect_status = PROT_FREQ_FORCED_DROP;
                        atw.cycle[cycle].comp[comp].local.frq_protect_step = protect_step[j];
                        return atw.cycle[cycle].comp[comp].local.freq_protect_status;
                    }
                }
                //检测下位是否存在强制上升
                for (j = i; j < P_MAX; j++)
                {
                    if (status_tab[i] == PROT_FREQ_FORCED_RISE)//下位存在频率强制上升
                    {
                        atw.cycle[cycle].comp[comp].local.freq_protect_status = PROT_FREQ_FORCED_RISE;
                        atw.cycle[cycle].comp[comp].local.frq_protect_step = protect_step[j];
                        return atw.cycle[cycle].comp[comp].local.freq_protect_status;
                    }
                }
                //检测下位是否存在禁止上升
                for (j = i; j < P_MAX; j++)
                {
                    if (status_tab[i] == PROT_FREQ_FORBID_RISE)//下位存在频率禁止上升
                    {
                        atw.cycle[cycle].comp[comp].local.freq_protect_status = PROT_FREQ_FORBID_RISE;
                        return atw.cycle[cycle].comp[comp].local.freq_protect_status;
                    }
                }
                //禁止下降
                atw.cycle[cycle].comp[comp].local.freq_protect_status = PROT_FREQ_FORBID_DROP;
                return atw.cycle[cycle].comp[comp].local.freq_protect_status;
            }
                break;   
            default:
                break;
        }
    }
    return atw.cycle[cycle].comp[comp].local.freq_protect_status;
}

/************************************************************************
@name  	: CompGetFimin3
@brief 	: 
@param 	: None
@return	: None
@note   : 7.1.8 压缩机保护控制及Fimin3,Fimax3
*************************************************************************/
static uint16_t CompGetFimin3(uint8_t cycle,uint8_t comp)//TODO 不满足时的取值
{
    uint16_t res;
    uint16_t Fi_run = atw.cycle[cycle].comp[comp].link.state.freq_output;
    uint8_t protect_status = atw.cycle[cycle].comp[comp].local.freq_protect_status;
    
    if ((protect_status == PROT_FREQ_FORBID_RISE) || (protect_status == PROT_FREQ_FORCED_DROP))//频率禁止上升,或频率强制下降
    {
        res = 150;
    }
    else if ((protect_status == PROT_FREQ_FORBID_DROP) || (protect_status == PROT_FREQ_FORCED_RISE))//禁止下降,或强制上升
    {
        res = Fi_run;
    }
    else//正常控制区域
    {
        
    }

    return res;
}

/************************************************************************
@name  	: CompGetFimax3
@brief 	: 
@param 	: None
@return	: None
@note   : 7.1.8 压缩机保护控制及Fimin3,Fimax3
*************************************************************************/
static uint16_t CompGetFimax3(uint8_t cycle,uint8_t comp)//TODO 不满足时的取值
{
    uint16_t res;
    uint16_t Fi_run = atw.cycle[cycle].comp[comp].link.state.freq_output;
    uint8_t protect_status = atw.cycle[cycle].comp[comp].local.freq_protect_status;
    
    if ((protect_status == PROT_FREQ_FORBID_RISE) || (protect_status == PROT_FREQ_FORCED_DROP))
    {
        res = Fi_run;
    }
    else if ((protect_status == PROT_FREQ_FORBID_DROP) || (protect_status == PROT_FREQ_FORCED_RISE))
    {
        res = 1300;
    }
    else//正常控制区域
    {
        
    }

    return res;
}

/************************************************************************
@name  	: CompGetFimax4
@brief 	: 
@param 	: None
@return	: None
@note   : 7.1.9 出水温度限制Fimax4
            cycle运转后,需要检测本单元的出水温度Tw_out
*************************************************************************/
static uint16_t CompGetFimax4(uint8_t cycle,uint8_t comp)//TODO 制冷默认值
{
    uint16_t res;
    uint8_t cycle_step = atw.cycle[cycle].work_step;
    uint16_t Fi_run = atw.cycle[cycle].comp[comp].link.state.freq_output;
    int16_t  Tw_out = atw.unit.table.io.Tw_out;
    uint8_t refrigent_type = atw.unit.table.init.dsw1.bit.refrigent_type;
    uint8_t temp_type = atw.unit.table.init.dsw1.bit.temp_type;
    int16_t Tam = atw.unit.table.io.Tam;
    int16_t Tmax_unit;

    if (cycle_step !=  CYCLE_STATUS_OFF)//运行中
    {
        if (GetCycleRealRunMode(cycle) == CYCLE_MODE_COOL)//cool
        {
            if (refrigent_type == STANDARD_WATER)//标准水
            {
                if (CompGetFimax4RefreshTimer(cycle,comp) == 0)//周期
                {
                    if (Tw_out <= 35)
                    {
                        res = Fi_run - 20;
                        CompSetFimax4RefreshTimer(cycle,comp,5);
                    }
                    else if (Tw_out <= 45)
                    {
                        res = Fi_run;
                        CompSetFimax4RefreshTimer(cycle,comp,0);
                    }
                    else if (Tw_out <= 55)
                    {
                        res = Fi_run + 10;
                        CompSetFimax4RefreshTimer(cycle,comp,20);
                    }
                    else res = 1300;
                }
            }
            else if (temp_type == TEMP_SA)//+防冻液 标准热泵
            {
                if (CompGetFimax4RefreshTimer(cycle,comp) == 0)//周期
                {
                    if (Tw_out <= -105)
                    {
                        res = Fi_run - 20;
                        CompSetFimax4RefreshTimer(cycle,comp,5);
                    }
                    else if (Tw_out <= -100)
                    {
                        res = Fi_run;
                        CompSetFimax4RefreshTimer(cycle,comp,0);
                    }
                    else if (Tw_out <= -80)
                    {
                        res = Fi_run + 10;
                        CompSetFimax4RefreshTimer(cycle,comp,20);
                    }
                    else res = 1300;
                }
            }
        }
        else if (GetCycleRealRunMode(cycle) == CYCLE_MODE_HEAT)//heat
        {
        //get Tmax_unit
            if (atw.unit.local.sns_err.bit.Tam == TRUE)
            {
                Tmax_unit = 450;
            }
            else Tmax_unit = Tam + 610;
        //get value limit    
            if (temp_type == TEMP_SA)//标准热泵
            {
                if (Tmax_unit < 390)
                {
                    Tmax_unit = 390;
                }
                else if (Tmax_unit > 550)
                {
                    Tmax_unit = 550;
                }
            }
            else if (refrigent_type == STANDARD_WATER)//标准水+低温热泵
            {
                if (Tmax_unit < 390)
                {
                    Tmax_unit = 390;
                }
                else if (Tmax_unit > 580)
                {
                    Tmax_unit = 580;
                }
            }
        //get Fimax4    
            if (CompGetFimax4RefreshTimer(cycle,comp) == 0)//周期
            {
                if ((Tw_out >= Tmax_unit - 10) && (Fi_run > 400))
                {
                    res = Fi_run;
                    CompSetFimax4RefreshTimer(cycle,comp,0);
                }
                else if ((Tw_out >= Tmax_unit) && (Fi_run > 400))
                {
                    res = Fi_run - 20;
                    CompSetFimax4RefreshTimer(cycle,comp,5);//5s更新周期
                }
            }
        }
    }
    return res;
}


/************************************************************************
@name  	: CompGetFimax
@brief 	: 
@param 	: None
@return	: None
@note   : 7.1.5 压缩机运行频率上下限Fimin/Fimax
            Fimax = min（Fimax1，Fimax2， Fimax3， Fimax4）
*************************************************************************/
uint16_t CompGetFimax(uint8_t cycle,uint8_t comp)
{
    uint16_t res = CompGetFimax1(cycle,comp);//Fi_max1未确定
    uint16_t Fimax2 = CompGetFimax2(cycle,comp);
    uint16_t Fimax3 = CompGetFimax3(cycle,comp);
    uint16_t Fimax4 = CompGetFimax4(cycle,comp);

    if (res > Fimax2)
    {
        res = Fimax2;
    }
    if (res > Fimax3)
    {
        res = Fimax3;
    }
    if (res > Fimax4)
    {
        res = Fimax4;
    }

    return res;
}

/************************************************************************
@name  	: CompGetFimax
@brief 	: 
@param 	: None
@return	: None
@note   : 7.1.5 压缩机运行频率上下限Fimin/Fimax
            Fimin = max（Fimin1，Fimin2， Fimin3）
*************************************************************************/
uint16_t CompGetFimin(uint8_t cycle,uint8_t comp)
{
    uint16_t res = CompGetFimin1(cycle,comp);
    uint16_t Fimin2 = CompGetFimin2(cycle,comp);
    uint16_t Fimin3 = CompGetFimin3(cycle,comp);

    if (res < Fimin2)
    {
        res = Fimin2;
    }
    if (res < Fimin3)
    {
        res = Fimin3;
    }

    return res;
}
