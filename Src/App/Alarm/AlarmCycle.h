#ifndef __ALARMCYCLE_H
#define __ALARMCYCLE_H
#include "common.h"





//cycle故障
typedef enum
{
    CYCLE_ERR_TCIOL_SENSOR      = 0x00000001,   //b0:翅片盘管温度传感器故障
    CYCLE_ERR_TD_SENSOR         = 0x00000002,   //b1:排气温度传感器故障
    CYCLE_ERR_TS_SENSOR         = 0x00000004,   //b2:气分温度传感器故障
    CYCLE_ERR_TSUB_IN_SENSOR    = 0x00000008,   //b3:经济器进口温度传感器故障
    CYCLE_ERR_TSUB_OUT_SENSOR   = 0x00000010,   //b4:经济器出口温度传感器故障
    CYCLE_ERR_PD_SENSOR         = 0x00000020,   //b5:高压传感器故障 //Pd
    CYCLE_ERR_PS_SENSOR         = 0x00000040,   //b6:低压传感器故障 //Ps
    CYCLE_ERR_TD_HIGH           = 0x00000080,   //b7:排气温度过高
    CYCLE_ERR_PD_HIGH           = 0x00000100,   //b8:排气压力过高
    CYCLE_ERR_PS_LOW            = 0x00000200,   //b9:吸气压力过低
    CYCLE_ERR_FAN1_SPEED_LOW    = 0x00000400,   //b10:风机转速过低
    CYCLE_ERR_FAN2_SPEED_LOW    = 0x00000800,   //b11:风机转速过低
    CYCLE_ERR_FAN1_SPEED_HIGH   = 0x00001000,   //b12:风机转速过高
    CYCLE_ERR_FAN2_SPEED_HIGH   = 0x00002000,   //b13:风机转速过高
    CYCLE_ERR_INVERTER          = 0x00004000,   //b14:变频器故障

    CYCLE_ERR_MAX = 15

}CycleAlarmEnum;

//cycle
typedef union
{
    uint32_t data;
    struct
    {
        uint16_t Tcoil_sensor:1;    //b0:翅片盘管温度传感器故障
        uint16_t Td_sensor:1;       //b1:排气温度传感器故障
        uint16_t Ts_sensor:1;       //b2:气分温度传感器故障
        uint16_t Tsub_in_sensor:1;  //b3:经济器进口温度传感器故障
        uint16_t Tsub_out_sensor:1; //b4:经济器出口温度传感器故障
        uint16_t Pd_sensor:1;       //b5:高压传感器故障
        uint16_t Ps_sensor:1;       //b6:低压传感器故障
        uint16_t Td_high:1;         //b7:排气温度过高
        uint16_t Pd_high:1;         //b8:排气压力过高
        uint16_t Ps_low:1;          //b9:吸气压力过低
        uint16_t fan1_speed_low:1;  //b10:风机转速过低
        uint16_t fan2_speed_low:1;  //b11:
        uint16_t fan1_speed_high:1; //b12:风机转速过高
        uint16_t fan2_speed_high:1; //b13:
        uint16_t inverter:1;        //b14:变频器故障
        uint16_t reserved1:1;
        uint16_t reserved2:16;

    }cell;
    
}CycleAlarmType;

//comp
typedef union
{
    uint32_t data;
    struct
    {
        uint16_t overload_current:1;      //b0:过载与过流
        uint16_t transient_overcurrent:1; //b1:瞬时过流
        uint16_t power_model_heat:1;      //b2:功率模块发热
        uint16_t IPM_error:1;              //b3:IPM error
        uint16_t short_circuit:1;          //b4:对地短路

        uint16_t Pd_sensor:1;       //b5:高压传感器故障
        uint16_t Ps_sensor:1;       //b6:低压传感器故障
        uint16_t Td_high:1;         //b7:排气温度过高
        uint16_t Pd_high:1;         //b8:排气压力过高
        uint16_t Ps_low:1;          //b9:吸气压力过低
        uint16_t HP:1;              //b10:高压开关异常
        uint16_t fan1_speed_low:1;  //b11:风机转速过低
        uint16_t fan2_speed_low:1;  //b12:
        uint16_t fan1_speed_high:1; //b13:风机转速过高
        uint16_t fan2_speed_high:1; //b14:
        uint16_t inverter:1;        //b15:变频器故障
        uint16_t reserved:16;

    }cell;
    
}CompAlarmType;







void AlarmCycleTimerCounter(uint8_t cycle);
uint32_t CycleAlarmStateCheck(uint8_t cycle);

#endif//__ALARMCYCLE_H
