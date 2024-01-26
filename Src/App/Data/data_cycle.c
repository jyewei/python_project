
/***********************************************************************
@file   : data_cycle.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
@note   : cycle与unit相关的参数传递,根据配置得到的参数信息
************************************************************************/
#include "data_cycle.h"
#include "config.h"
#include "Data.h"
#include "IOProcess.h"


//
typedef enum
{
    DEVICE_045KW_COMP1,
    DEVICE_065KW_COMP1,
    DEVICE_100KW_COMP1,
    DEVICE_100KW_COMP2,
    DEVICE_130KW_COMP1,
    DEVICE_130KW_COMP2,

    DEVICE_DEVICE_MAX

}FreqDeviceTypeEnum;

//
typedef enum
{
    FREQ_FI_C_INI,                  //制冷初始频率Fi_c_ini 
    FREQ_FI_H_INI,                  //制热初始频率Fi_h_ini
    FREQ_FI_C_RATING,               //制冷额定频率Fi_c_rating
    FREQ_FI_H_RATING,               //制热额定频率Fi_h_rating
    FREQ_FMAX_STANDARD,             //最大频率Fmax,标准
    FREQ_FMAX_SILENCE1,             //最大频率Fmax,静音1
    FREQ_FMAX_SILENCE2,             //最大频率Fmax,静音2

    FREQ_FI_C_OIL,                  //制冷回油频率Fi_c_oil
    FREQ_FI_H_OIL,                  //制热回油频率Fi_h_oil
    FREQ_FI_INI_FROST,              //制热初始除霜Fi_ini_frost
    FREQ_FI_FROST,                  //制热除霜Fi_frost

    FREQ_TYPE_MAX

}FreqTypeEnum;


//7.1.1初始运行频率与额定运行频率
const uint16_t Fi_ini_table[FREQ_TYPE_MAX][DEVICE_DEVICE_MAX] = 
{
    //45kw  65kw    100kw_1 100kw_2 130kw_1 130kw_2   
    {310,   310,    310,    310,    310,    310},   //制冷初始频率Fi_c_ini 
    {310,   310,    310,    310,    310,    310},   //制热初始频率Fi_h_ini
    {940,   1020,   980,    980,    1050,   1050},  //制冷额定频率Fi_c_rating
    {1000,  1050,   1040,   1040,   1150,   1150},  //制热额定频率Fi_h_rating
    {1200,  1100,   1100,   1100,   1100,   1100},  //最大频率Fmax,标准
    {900,   960,    920,    920,    990,    990},   //最大频率Fmax,静音1
    {850,   910,    860,    860,    930,    930},   //最大频率Fmax,静音2
    {450,   450,    450,    450,    450,    450},   //制冷回油频率Fi_c_oil
    {540,   540,    540,    540,    540,    540},   //制热回油频率Fi_h_oil
    {310,   310,    310,    310,    310,    310},   //制热初始除霜Fi_ini_frost
    {940,   1020,    980,   980,    1050,   1050}   //制热除霜Fi_frost

};



/************************************************************************
@name  	: CompRefreshInitFrequency
@brief 	: 
@param 	: None
@return	: None
@note   : 获取频率
*************************************************************************/
void CompRefreshInitFrequency(uint8_t comp)
{
    uint8_t capacity = atw.unit.table.init.dsw1.bit.capacity;
    uint8_t silence = atw.unit.table.sys_cmd.data1.bit.silence_set;

    switch (capacity)
    {
        case CAPACITY_45KW:
        {
            atw.cycle[CYCLE_1].comp[comp].local.Fi_c_ini = Fi_ini_table[FREQ_FI_C_INI][DEVICE_045KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_h_ini = Fi_ini_table[FREQ_FI_H_INI][DEVICE_045KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_c_rating = Fi_ini_table[FREQ_FI_C_RATING][DEVICE_045KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_h_rating = Fi_ini_table[FREQ_FI_H_RATING][DEVICE_045KW_COMP1];

            switch (silence)
            {
                case SILENCE_NULL:  atw.cycle[CYCLE_1].comp[comp].local.Fimax = Fi_ini_table[FREQ_FMAX_STANDARD][DEVICE_045KW_COMP1];    break;
                case SILENCE_1:     atw.cycle[CYCLE_1].comp[comp].local.Fimax = Fi_ini_table[FREQ_FMAX_SILENCE1][DEVICE_045KW_COMP1];    break;
                case SILENCE_2:     atw.cycle[CYCLE_1].comp[comp].local.Fimax = Fi_ini_table[FREQ_FMAX_SILENCE2][DEVICE_045KW_COMP1];    break;
                default:    break;
            }
            atw.cycle[CYCLE_1].comp[comp].local.Fi_c_oil = Fi_ini_table[FREQ_FI_C_OIL][DEVICE_045KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_h_oil = Fi_ini_table[FREQ_FI_H_OIL][DEVICE_045KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_ini_frost = Fi_ini_table[FREQ_FI_INI_FROST][DEVICE_045KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_frost = Fi_ini_table[FREQ_FI_FROST][DEVICE_045KW_COMP1];

        }
            break;
        case CAPACITY_65KW:
        {
            atw.cycle[CYCLE_1].comp[comp].local.Fi_c_ini = Fi_ini_table[FREQ_FI_C_INI][DEVICE_065KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_h_ini = Fi_ini_table[FREQ_FI_H_INI][DEVICE_065KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_c_rating = Fi_ini_table[FREQ_FI_C_RATING][DEVICE_065KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_h_rating = Fi_ini_table[FREQ_FI_H_RATING][DEVICE_065KW_COMP1];

            switch (silence)
            {
                case SILENCE_NULL:  atw.cycle[CYCLE_1].comp[comp].local.Fimax = Fi_ini_table[FREQ_FMAX_STANDARD][DEVICE_065KW_COMP1];    break;
                case SILENCE_1:     atw.cycle[CYCLE_1].comp[comp].local.Fimax = Fi_ini_table[FREQ_FMAX_SILENCE1][DEVICE_065KW_COMP1];    break;
                case SILENCE_2:     atw.cycle[CYCLE_1].comp[comp].local.Fimax = Fi_ini_table[FREQ_FMAX_SILENCE2][DEVICE_065KW_COMP1];    break;
                default:    break;
            }
            atw.cycle[CYCLE_1].comp[comp].local.Fi_c_oil = Fi_ini_table[FREQ_FI_C_OIL][DEVICE_065KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_h_oil = Fi_ini_table[FREQ_FI_H_OIL][DEVICE_065KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_ini_frost = Fi_ini_table[FREQ_FI_INI_FROST][DEVICE_065KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_frost = Fi_ini_table[FREQ_FI_FROST][DEVICE_065KW_COMP1];
        }
            break;
        case CAPACITY_100KW:
        {//comp1
            atw.cycle[CYCLE_1].comp[comp].local.Fi_c_ini = Fi_ini_table[FREQ_FI_C_INI][DEVICE_100KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_h_ini = Fi_ini_table[FREQ_FI_H_INI][DEVICE_100KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_c_rating = Fi_ini_table[FREQ_FI_C_RATING][DEVICE_100KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_h_rating = Fi_ini_table[FREQ_FI_H_RATING][DEVICE_100KW_COMP1];

            switch (silence)
            {
                case SILENCE_NULL:  atw.cycle[CYCLE_1].comp[comp].local.Fimax = Fi_ini_table[FREQ_FMAX_STANDARD][DEVICE_100KW_COMP1];    break;
                case SILENCE_1:     atw.cycle[CYCLE_1].comp[comp].local.Fimax = Fi_ini_table[FREQ_FMAX_SILENCE1][DEVICE_100KW_COMP1];    break;
                case SILENCE_2:     atw.cycle[CYCLE_1].comp[comp].local.Fimax = Fi_ini_table[FREQ_FMAX_SILENCE2][DEVICE_100KW_COMP1];    break;
                default:    break;
            }
            atw.cycle[CYCLE_1].comp[comp].local.Fi_c_oil = Fi_ini_table[FREQ_FI_C_OIL][DEVICE_100KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_h_oil = Fi_ini_table[FREQ_FI_H_OIL][DEVICE_100KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_ini_frost = Fi_ini_table[FREQ_FI_INI_FROST][DEVICE_100KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_frost = Fi_ini_table[FREQ_FI_FROST][DEVICE_100KW_COMP1];
        //comp2
            atw.cycle[CYCLE_2].comp[comp].local.Fi_c_ini = Fi_ini_table[FREQ_FI_C_INI][DEVICE_100KW_COMP2];
            atw.cycle[CYCLE_2].comp[comp].local.Fi_h_ini = Fi_ini_table[FREQ_FI_H_INI][DEVICE_100KW_COMP2];
            atw.cycle[CYCLE_2].comp[comp].local.Fi_c_rating = Fi_ini_table[FREQ_FI_C_RATING][DEVICE_100KW_COMP2];
            atw.cycle[CYCLE_2].comp[comp].local.Fi_h_rating = Fi_ini_table[FREQ_FI_H_RATING][DEVICE_100KW_COMP2];

            switch (silence)
            {
                case SILENCE_NULL:  atw.cycle[CYCLE_2].comp[comp].local.Fimax = Fi_ini_table[FREQ_FMAX_STANDARD][DEVICE_100KW_COMP2];    break;
                case SILENCE_1:     atw.cycle[CYCLE_2].comp[comp].local.Fimax = Fi_ini_table[FREQ_FMAX_SILENCE1][DEVICE_100KW_COMP2];    break;
                case SILENCE_2:     atw.cycle[CYCLE_2].comp[comp].local.Fimax = Fi_ini_table[FREQ_FMAX_SILENCE2][DEVICE_100KW_COMP2];    break;
                default:    break;
            }
            atw.cycle[CYCLE_2].comp[comp].local.Fi_c_oil = Fi_ini_table[FREQ_FI_C_OIL][DEVICE_100KW_COMP2];
            atw.cycle[CYCLE_2].comp[comp].local.Fi_h_oil = Fi_ini_table[FREQ_FI_H_OIL][DEVICE_100KW_COMP2];
            atw.cycle[CYCLE_2].comp[comp].local.Fi_ini_frost = Fi_ini_table[FREQ_FI_INI_FROST][DEVICE_100KW_COMP2];
            atw.cycle[CYCLE_2].comp[comp].local.Fi_frost = Fi_ini_table[FREQ_FI_FROST][DEVICE_100KW_COMP2];
        }
            break;
        case CAPACITY_130KW:
        {//comp1
            atw.cycle[CYCLE_1].comp[comp].local.Fi_c_ini = Fi_ini_table[FREQ_FI_C_INI][DEVICE_130KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_h_ini = Fi_ini_table[FREQ_FI_H_INI][DEVICE_130KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_c_rating = Fi_ini_table[FREQ_FI_C_RATING][DEVICE_130KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_h_rating = Fi_ini_table[FREQ_FI_H_RATING][DEVICE_130KW_COMP1];

            switch (silence)
            {
                case SILENCE_NULL:  atw.cycle[CYCLE_1].comp[comp].local.Fimax = Fi_ini_table[FREQ_FMAX_STANDARD][DEVICE_130KW_COMP1];    break;
                case SILENCE_1:     atw.cycle[CYCLE_1].comp[comp].local.Fimax = Fi_ini_table[FREQ_FMAX_SILENCE1][DEVICE_130KW_COMP1];    break;
                case SILENCE_2:     atw.cycle[CYCLE_1].comp[comp].local.Fimax = Fi_ini_table[FREQ_FMAX_SILENCE2][DEVICE_130KW_COMP1];    break;
                default:    break;
            }
            atw.cycle[CYCLE_1].comp[comp].local.Fi_c_oil = Fi_ini_table[FREQ_FI_C_OIL][DEVICE_130KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_h_oil = Fi_ini_table[FREQ_FI_H_OIL][DEVICE_130KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_ini_frost = Fi_ini_table[FREQ_FI_INI_FROST][DEVICE_130KW_COMP1];
            atw.cycle[CYCLE_1].comp[comp].local.Fi_frost = Fi_ini_table[FREQ_FI_FROST][DEVICE_130KW_COMP1];
        //comp2
            atw.cycle[CYCLE_2].comp[comp].local.Fi_c_ini = Fi_ini_table[FREQ_FI_C_INI][DEVICE_130KW_COMP2];
            atw.cycle[CYCLE_2].comp[comp].local.Fi_h_ini = Fi_ini_table[FREQ_FI_H_INI][DEVICE_130KW_COMP2];
            atw.cycle[CYCLE_2].comp[comp].local.Fi_c_rating = Fi_ini_table[FREQ_FI_C_RATING][DEVICE_130KW_COMP2];
            atw.cycle[CYCLE_2].comp[comp].local.Fi_h_rating = Fi_ini_table[FREQ_FI_H_RATING][DEVICE_130KW_COMP2];

            switch (silence)
            {
                case SILENCE_NULL:  atw.cycle[CYCLE_2].comp[comp].local.Fimax = Fi_ini_table[FREQ_FMAX_STANDARD][DEVICE_130KW_COMP2];    break;
                case SILENCE_1:     atw.cycle[CYCLE_2].comp[comp].local.Fimax = Fi_ini_table[FREQ_FMAX_SILENCE1][DEVICE_130KW_COMP2];    break;
                case SILENCE_2:     atw.cycle[CYCLE_2].comp[comp].local.Fimax = Fi_ini_table[FREQ_FMAX_SILENCE2][DEVICE_130KW_COMP2];    break;
                default:    break;
            }
            atw.cycle[CYCLE_2].comp[comp].local.Fi_c_oil = Fi_ini_table[FREQ_FI_C_OIL][DEVICE_130KW_COMP2];
            atw.cycle[CYCLE_2].comp[comp].local.Fi_h_oil = Fi_ini_table[FREQ_FI_H_OIL][DEVICE_130KW_COMP2];
            atw.cycle[CYCLE_2].comp[comp].local.Fi_ini_frost = Fi_ini_table[FREQ_FI_INI_FROST][DEVICE_130KW_COMP2];
            atw.cycle[CYCLE_2].comp[comp].local.Fi_frost = Fi_ini_table[FREQ_FI_FROST][DEVICE_130KW_COMP2];
        }
            break;
        default:
            break;
    }
}


/************************************************************************
@name  	: GetExpvEnableState
@brief 	: 
@param 	: None
@return	: None
@note   : 根据配置检测膨胀阀是否使能
*************************************************************************/
void GetExpvEnableState(uint8_t cycle)
{
    uint8_t tier = atw.unit.table.init.dsw1.bit.tier_type;//
    //uint8_t temp_type = atw.unit.table.init.dsw1.bit.temp_type;//
    uint8_t capacity = atw.unit.table.init.dsw1.bit.capacity;

    atw.cycle[cycle].expv[EXPV1].enable = TRUE;
    if (capacity == CAPACITY_45KW)//45kw
    {
        atw.cycle[cycle].expv[EXPV2].enable = TRUE;
        atw.cycle[cycle].expv[EXPV3].enable = TRUE;
    }
    else if (capacity == CAPACITY_65KW)//65kw
    {
        atw.cycle[cycle].expv[EXPV2].enable = TRUE;
        atw.cycle[cycle].expv[EXPV3].enable = TRUE;
    }
    else if (capacity == CAPACITY_130KW)//130kw
    {
        atw.cycle[cycle].expv[EXPV2].enable = TRUE;
        if (tier == TIER_1)
        {
            atw.cycle[cycle].expv[EXPV3].enable = TRUE;
        }
        else atw.cycle[cycle].expv[EXPV3].enable = FALSE;//TODO 待确定
    }
    else//配置错误
    {
        atw.cycle[cycle].expv[EXPV1].enable = FALSE;
        atw.cycle[cycle].expv[EXPV2].enable = FALSE;
        atw.cycle[cycle].expv[EXPV3].enable = FALSE;
    }
}


/************************************************************************
@name  	: CycleSetRunMode
@brief 	: 
@param 	: None
@return	: None
@note   : 
*************************************************************************/
void CycleSetRunMode(uint8_t cycle,uint8_t mode)
{
    atw.cycle[cycle].set_run_mode = mode;
}

















