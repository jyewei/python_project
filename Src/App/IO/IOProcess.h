#ifndef __IOPROCESS_H
#define __IOPROCESS_H
#include "Common.h"



//sensor channel enum //@note:local sensor
typedef enum
{
    SENSOR_SHORT,       //0:瞬停检测
    SENSOR_Pd1,         //1:排气压力传感器1
    SENSOR_Ps1,         //2:吸气压力传感器1
    SENSOR_Pd2,         //3:排气压力传感器2
    SENSOR_Ps2,         //4:吸气压力传感器2
    SENSOR_Td1,         //5:1#压缩机排气温度
    SENSOR_Td2,         //6:2#压缩机排气温度
    SENSOR_Ts1,         //7:1#气分进口温度
    SENSOR_Ts2,         //8:2#气分进口温度
    SENSOR_Tsub_in1,    //9:1#经济器进口温度
    SENSOR_Tsub_out1,   //10:1#经济器出口温度
    SENSOR_Tsub_in2,    //11:2#经济器进口温度
    SENSOR_Tsub_out2,   //12:2#经济器出口温度
    SENSOR_Tam,         //13:室外环境温度
    SENSOR_Tcoil1,      //14:1#化霜温度传感器   //翅片盘管
    SENSOR_Tcoil2,      //15:2#化霜温度传感器
    SENSOR_Tw_in,       //16:模块回水温度传感器
    SENSOR_Tw_out,      //17:模块出水温度传感器
    SENSOR_Tw_sys_out,  //18:系统出水温度传感器
    SENSOR_RESERVED1,   //19:预留
    SENSOR_RESERVED2,   //20:预留
    SENSOR_RESERVED3,   //21:预留

    SENSOR_CH_MAX

}SensorChannelEnum;

//DO channel enum
typedef enum
{

    DO_CH_FOUR_WAY_V1,          //DO1:四通阀1
    DO_CH_FOUR_WAY_V2,          //DO2:四通阀2
    DO_CH_WATER_HEATER,         //DO3:辅助水电加热
    DO_CH_WATER_PUMP,           //DO4:水泵
    DO_CH_COMP_HC1,             //DO5:压缩机曲轴电加热带1
    DO_CH_COMP_HC2,             //DO6:压缩机曲轴电加热带2
    DO_CH_HEAT_BELT1,           //DO7:底盘加热带1
    DO_CH_HEAT_BELT2,           //DO8:底盘加热带2
    DO_CH_EVI_SV1,              //DO9:EVI电磁阀1
    DO_CH_OIL_RETURN_SV1,       //DO10:回油电磁阀1
    DO_CH_EVI_SV2,              //DO11:EVI电磁阀2
    DO_CH_OIL_RETURN_SV2,       //DO12:回油电磁阀2
    DO_CH_WATER_VALVE,          //DO13:水阀
    DO_CH_BOARD_HEATER,         //DO14:板换电加热
    DO_CH_HOT_WATER_VALVE,      //DO15:热水水阀
    DO_CH_BPHE_SV1,             //DO16:空调BPHE电磁阀1
    DO_CH_BPHE_SV2,             //DO17:空调BPHE电磁阀2
    DO_CH_COIL_BRANCH_SV1,      //DO18:coil支路电磁阀1
    DO_CH_COIL_BRANCH_SV2,      //DO19:coil支路电磁阀2
    DO_CH_DEFROST_SV1,          //DO20:融霜电磁阀1
    DO_CH_DEFROST_SV2,          //DO21:融霜电磁阀2
    DO_CH_HOT_WATER_PUMP,       //DO22:系统热水水泵控制
    DO_CH_ANTIFREEZE_HEATER,    //DO23:防冻电加热2

    DO_CH_MAX

}DoutChannelEnum;

//DI channel enum
typedef enum
{

    DI_CH_UNIT_WATERFLOW_SWITCH,                        //DI1:单元水流开关
    DI_CH_SYS_WATERFLOW_SWITCH,                         //DI2:系统水流开关
    DI_CH_REMOTE_MODE_CHANGE,                           //DI3:远程冷热模式切换
    DI_CH_REMOTE,                                       //DI4:远程DI //TODO
    DI_CH_REMOTEURGENCY_OFF,                            //DI5:紧急联动关
    DI_CH_WATER_PUMP_OVERLOAD,                          //DI6:水泵过载反馈
    DI_CH_SECOND_PUMP_FB,                               //DI7:二次泵反馈
    DI_CH_RESERVED_DI8,                                 //DI8:预留
    DI_CH_UNIT_HOT_WATERFLOW,
    DI_CH_FAN_PROTECT_FB1 = DI_CH_UNIT_HOT_WATERFLOW,   //DI9:模块热水水流开关/风机保护反馈1
    DI_CH_SYS_HOT_WATERFLOW,
    DI_CH_FAN_PROTECT_FB2 = DI_CH_SYS_HOT_WATERFLOW,    //DI10:系统热水水流开关/风机保护反馈2
    DI_CH_HEAT_LOWER_PRESSURE_SWITCH,
    DI_CH_POWER_PROTECT = DI_CH_HEAT_LOWER_PRESSURE_SWITCH,//DI11:制热低压开关/电源保护
    DI_CH_COOL_LOWER_PRESSURE_SWITCH,                   //DI12:制冷低压开关
    DI_CH_RESERVED_DI13,                                //DI13:预留

    DI_CH_MAX

}DinChannelEnum;


//parameters about sys use
typedef union
{
    uint32_t data;
    struct
    {
        uint16_t unit_waterflow_switch:1;           //DI1:单元水流开关
        uint16_t sys_waterflow_switch:1;            //DI2:系统水流开关
        uint16_t remote_mode_change:1;              //DI3:远程冷热模式切换
        uint16_t remote_di:1;                       //DI4:远程DI //TODO
        uint16_t urgency_off:1;                     //DI5:紧急联动关
        uint16_t water_pump_over_load:1;            //DI6:水泵过载反馈
        uint16_t secondary_pump_fb:1;               //DI7:二次泵反馈
        uint16_t reserved_d8:1;                     //DI8:预留
        uint16_t unit_hot_waterflow_fan_prot_fb1:1; //DI9:模块热水水流开关/风机保护反馈1
        uint16_t sys_hot_waterflow_fan_prot_fb2:1;  //DI10:系统热水水流开关/风机保护反馈2
        uint16_t heat_low_pressure_power_prot:1;    //DI11:制热低压开关/电源保护
        uint16_t cool_low_pressure_switch:1;        //DI12:制冷低压开关
        uint16_t reserved_d13:1;                    //DI13:预留
        uint16_t reserved1:16;
        uint16_t reserved2:3;

    }cell;
}DinParamType;

//
typedef union
{
    uint32_t data;
    struct
    {
        uint16_t four_way_v1:1;         //DO1:四通阀1
        uint16_t four_way_v2:1;         //DO2:四通阀2
        uint16_t water_heater:1;        //DO3:辅助水电加热
        uint16_t water_pump:1;          //DO4:水泵
        uint16_t comp_hc1:1;            //DO5:压缩机曲轴电加热带1
        uint16_t comp_hc2:1;            //DO6:压缩机曲轴电加热带2
        uint16_t chassis_heat_belt1:1;  //DO7:底盘加热带1
        uint16_t chassis_heat_belt2:1;  //DO8:底盘加热带2
        uint16_t evi_sv1:1;             //DO9:EVI电磁阀1
        uint16_t oil_return_sv1:1;      //DO10:回油电磁阀1
        uint16_t evi_sv2:1;             //DO11:EVI电磁阀2
        uint16_t oil_return_sv2:1;      //DO12:回油电磁阀2
        uint16_t water_valve:1;         //DO13:水阀
        uint16_t board_heater:1;        //DO14:板换电加热
        uint16_t hot_water_valve:1;     //DO15:热水水阀
        uint16_t BPHE_sv1:1;            //DO16:空调BPHE电磁阀1
        uint16_t BPHE_sv2:1;            //DO17:空调BPHE电磁阀2
        uint16_t coil_branch_sv1:1;     //DO18:coil支路电磁阀1
        uint16_t coil_branch_sv2:1;     //DO19:coil支路电磁阀2
        uint16_t defrost_sv1:1;         //DO20:融霜电磁阀1
        uint16_t defrost_sv2:1;         //DO21:融霜电磁阀2
        uint16_t hot_water_pump:1;      //DO22:系统热水水泵控制
        uint16_t antifreeze_heater:1;   //DO23:防冻电加热2
        uint16_t reserved:9;
        
    }cell;
}DoutParamType;

//
typedef union
{
    uint32_t data;
    struct
    {
        uint16_t unit_work_state:1; //system work state
        uint16_t power_on_reset:1;  //
        uint16_t refrigent_type:1;  //
        uint16_t pump_type:1;       //
        uint16_t esp_type:2;        //

        uint16_t unit_series:2;     //
        uint16_t temp_type:1;       //
        uint16_t tier_type:1;       //
        uint16_t capacity:3;        //
        uint16_t unit_addr:5;       //unit address
        uint16_t group_addr:4;      //group No
        
        uint16_t reserved:10;       //
        
    }cell;
}DswParamType;

//parameters about sys use
typedef struct
{
    uint16_t ad_value;
    int16_t  rt_value;      //temp:0.1℃ pressure:1KPa/0.001MPa
    int16_t  rt_last;       //last value
    uint8_t  err_state;     //
    uint8_t  reserved;

}SensorParamType;

//
typedef struct
{
    SensorParamType sensor[SENSOR_CH_MAX];
    DinParamType    din;
    DoutParamType   dout;
    DswParamType    dsw;

}IOParamType;








uint8_t GetLocalAddress(void);
void DoSet(uint8_t ch,uint8_t state);
uint8_t GetLocalGroupAddress(void);
void IOProcess(uint8_t taskNo,uint8_t flag_init);


#endif//__IOPROCESS_H
