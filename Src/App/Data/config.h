#ifndef __CONFIG_H
#define __CONFIG_H
#include "Common.h"




/******************************************************************************************************************
1.1 
室外机组系列分为二管制系列、热回收系列、四管制系列
二管制系列分为标温Tier1机组、标温Tier3机组、低温Tier1机组、低温Tier3机组
标温以下简称SA,低温以下简称CA
	SA Tier1二管制机组
	SA Tier3二管制机组
	CA Tier1二管制机组
	CA Tier3二管制机组

1.9	模块单元构成及电气部品
 （1）机组型号
No.	室外机型号	       容量kW	    机组电源	        DC压缩机	BLDC风机	出风方式	类型	冷却形式	冷媒
1	TBD（SA Tier1）	    65	    380V±10%/3PH-50Hz	    1	        2	        顶出风	    热泵	风冷	R32
2	TBD（SA Tier1）	    130	    380V±10%/3PH-50Hz	    2	        4	        顶出风	    热泵	风冷	R32
3	TBD（SA Tier3） 	65	    380V±10%/3PH-50Hz	    1	        2	        顶出风	    热泵	风冷	R32
4	TBD（SA Tier3）	    130	    380V±10%/3PH-50Hz	    2	        4	        顶出风  	热泵	风冷	R32
5	TBD（CA Tier1）	    65	    380V±10%/3PH-50Hz	    1	        2	        顶出风	    热泵	风冷	R32
6	TBD（CA Tier1）	    130	    380V±10%/3PH-50Hz	    2	        4	        顶出风	    热泵	风冷	R32
7	TBD（CA Tier3）	    65	    380V±10%/3PH-50Hz	    1	        2	        顶出风	    热泵	风冷	R32
8	TBD（CA Tier3）	    130	    380V±10%/3PH-50Hz	    2	        4	        顶出风	    热泵	风冷	R32


*******************************************************************************************************************/



//单元运行模式
typedef enum
{
    UNIT_STATE_NORMAL,    //
    UNIT_STATE_TEST

}UnitWorkStateEnum;

//
typedef enum
{
    POWER_STANDBY,      //来电待机
    POWER_RESET         //来电重启

}PowerResetTypeEnum;

//
typedef enum
{
    STANDARD_WATER,     //标准水
    ADD_ANTIFREEZE      //加防冻液

}RefrigentTypeEnum;

//水泵类型
typedef enum
{
    PUMP_PRIVATE,   //非共用水泵/无水阀
    PUMP_PUBLIC     //共用水泵/有水阀

}PumpTypeEnum;

//水泵状态
typedef enum
{
    PUMP_STATE_STOP,        //停止中
    PUMP_STATE_WAIT_FB,     //等待反馈中
    PUMP_STATE_RUN,         //运转中    
    PUMP_STATE_FORCE_RUN,   //手动强制运转
    PUMP_STATE_ANTIFREEZE,  //防冻结运转
    PUMP_STATE_ERROR,       //故障
    PUMP_STATE_MAX

}PumpStateTypeEnum;



//静压
typedef enum
{
    ESP_NULL,
    ESP_30Pa,
    ESP_50Pa,
    ESP_80Pa

}ESPEnum;

//框体系列
typedef enum
{
    FRAME_A,    //框体A
    FRAME_B,    //框体B
    FRAME_C,    //框体C
    FRAME_D,    //框体D
    FRAME_MAX

}FrameTypeEnum;

//机组系列
typedef enum
{
    UNIT_TWO_PIPE,      //二管制
    UNIT_FOUR_PIPE,     //四管制
    UNIT_HEAT_RECYCLE,  //热回收
    UNIT_HEAT_WATER     //热水机

}UnitSeriesEnum;

//温度系列
typedef enum
{
    TEMP_SA,      //标温
    TEMP_CA       //低温

}TempSeriesEnum;

//Tier系列
typedef enum
{
    TIER_1,
    TIER_3

}TierTypeEnum;

//容量
typedef enum
{
    CAPACITY_65KW,
    CAPACITY_130KW,
    CAPACITY_45KW,
    CAPACITY_160KW,
    CAPACITY_260KW,
    CAPACITY_300KW,
    CAPACITY_100KW,
    CAPACITY_RESERVED2,
    CAPACITY_MAX

}DeviceCapacaityEnum;


//chiller channel enum
typedef enum
{
    CHILLER_0,
    CHILLER_1,
    CHILLER_2,
    CHILLER_3,
    CHILLER_4,
    CHILLER_5,
    CHILLER_6,
    CHILLER_7,
    CHILLER_8,
    CHILLER_9,
    CHILLER_10,
    CHILLER_11,
    CHILLER_12,
    CHILLER_13,
    CHILLER_14,
    CHILLER_15,
    CHILLER_16,
    CHILLER_17,
    CHILLER_18,
    CHILLER_19,
    CHILLER_20,
    CHILLER_21,
    CHILLER_22,
    CHILLER_23,
    CHILLER_24,
    CHILLER_25,
    CHILLER_26,
    CHILLER_27,
    CHILLER_28,
    CHILLER_29,
    CHILLER_30,
    CHILLER_31,
    CHILLER_CH_MAX

}ChillerChannelEnum;

//group enum
typedef enum
{
    GROUP_1,
    GROUP_2,
    GROUP_3,
    GROUP_4,
    GROUP_5,
    GROUP_6,
    GROUP_7,
    GROUP_8,
    GROUP_9,
    GROUP_10,
    GROUP_11,
    GROUP_12,
    GROUP_13,
    GROUP_14,
    GROUP_15,
    GROUP_16,
    GROUP_CH_MAX

}GroupEnum;

//cycle enum
typedef enum
{
    CYCLE_1,
    CYCLE_2,
    CYCLE_3,
    CYCLE_4,
    CYCLE_CH_MAX

}CycleEnum;


/*************
 2.2 
    1)制冷
    2)制热,包含:制热1,制热2(制热1/2来自智能温控器,对sys而言去控制阀门,对单元仍然是制热模式)
    3)水泵循环
    4)防冻保护(系统产生的防冻)
*************/
//unit
typedef enum
{
    UNIT_MODE_NULL,
    UNIT_MODE_COOL,       //制冷
    UNIT_MODE_HEAT,       //制热
    UNIT_MODE_PUMP,       //水泵运行

    UNIT_MODE_MAX

}UnitRunModeEnum;

//除霜环境类型
typedef enum
{
    DEFROST_ENV_STANDARD,  //标准除霜
    DEFROST_ENV_WET,       //高湿除霜
    DEFROST_ENV_MAX

}DefrostEnvEnum;

//除霜环境类型
typedef enum
{
    DEFROST_TYPE_AUTO,      //自动除霜
    DEFROST_TYPE_MANUAL,    //手动除霜
    DEFROST_TYPE_MAX

}DefrostTypeEnum;

//CYCLE
typedef enum
{
    CYCLE_MODE_NULL,
    CYCLE_MODE_COOL,       //制冷
    CYCLE_MODE_HEAT,       //制热

    CYCLE_MODE_MAX

}CycleRunModeEnum;

//sys mode
typedef enum
{
    SYS_MODE_NULL,       //无
    SYS_MODE_COOL,       //制冷
    SYS_MODE_HEAT,       //制热
    SYS_MODE_PUMP,       //水泵运行
    SYS_MODE_ANTIFREEZE, //防冻保护

    SYS_MODE_MAX

}SysRunModeEnum;


//静音设置 
typedef enum
{
    SILENCE_NULL,
    SILENCE_1,
    SILENCE_2,
    SILENCE_3,

    SILENCE_TYPE_MAX

}SilenceModeEnum;


//压缩机型号
typedef enum
{
    COMP_80CC,
    COMP_110CC,
    COMP_125CC,
    COMP_150CC,

    COMP_TYPE_MAX

}CompTypeEnum;





#endif//__CONFIG_H
