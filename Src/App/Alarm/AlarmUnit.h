#ifndef __ALARMUNIT_H
#define __ALARMUNIT_H
#include "common.h"









//单元故障
typedef enum
{
    UNIT_ERR_CHILLERLINK    = 0x00000001,      //bit0:单元与0#主机通讯故障
    UNIT_ERR_POWER          = 0x00000002,      //bit1:单元电源故障
    UNIT_ERR_PUMP_OVERLOAD  = 0x00000004,      //bit2:水泵过载
    UNIT_ERR_SENSOR_TAM     = 0x00000008,      //bit3:环境温度传感器故障
    UNIT_ERR_COOL_TAM_LOW   = 0x00000010,      //bit4:制冷环温过低报警
    UNIT_ERR_COOL_TAM_HIGH  = 0x00000020,      //bit5:制冷环温过高报警
    UNIT_ERR_HEAT_TAM_LOW   = 0x00000040,      //bit6:制热环温过低报警
    UNIT_ERR_HEAT_TAM_HIGH  = 0x00000080,      //bit7:制热环温过高报警
    UNIT_ERR_SENSOR_TW_IN   = 0x00000100,      //bit8:单元回水温度传感器故障
    UNIT_ERR_SENSOR_TW_OUT  = 0x00000200,      //bit9:单元出水温度传感器故障
    UNIT_ERR_COOL_OUT_LOW   = 0x00000400,      //bit10:制冷出水温度过低
    UNIT_ERR_HEAT_OUT_HIGH  = 0x00000800,      //bit11:制热出水温度过高
    UNIT_ERR_HEAT_OUT_LOW   = 0x00001000,      //bit12:制热出水温度过低
    UNIT_ERR_BOARD_FREEZE   = 0x00002000,      //bit13:板换冻结保护
    UNIT_ERR_WATER_FLOW     = 0x00004000,      //bit14:水流故障
    UNIT_ERR_COMPLINK       = 0x00008000,      //bit15:单元与压机通讯失败
    UNIT_ERR_FAN_LINK       = 0x00010000,      //bit16:与风机通讯失败
    UNIT_ERR_INV_MATCH      = 0x00020000,      //bit17:与驱动器不匹配

    UNIT_ERR_MAX = 18

}UnitAlarmEnum;



//cycle
typedef union
{
    uint32_t data;
    struct
    {
        uint16_t chillerlink:1;     //b0:单元与0#主机通讯故障
        uint16_t ac_power_in:1;     //b1:单元电源故障
        uint16_t pump_overload:1;   //b2:水泵过载
        uint16_t sensor_tam:1;      //b3:环境温度传感器故障
        uint16_t cool_tam_low:1;    //b4:制冷环温过低报警
        uint16_t cool_tam_high:1;   //b5:制冷环温过高报警
        uint16_t heat_tam_low:1;    //b6:制热环温过低报警
        uint16_t heat_tam_high:1;   //b7:制热环温过高报警
        uint16_t sensor_tw_in:1;    //b8:单元回水温度传感器故障
        uint16_t sensor_tw_out:1;   //b9:单元出水温度传感器故障
        uint16_t cool_out_low:1;    //b10:制冷出水温度过低
        uint16_t heat_out_high:1;   //b11:制热出水温度过高
        uint16_t heat_out_low:1;    //b12:制热出水温度过低
        uint16_t board_freeze:1;    //b13:板换冻结保护
        uint16_t waterflow:1;       //b14:水流故障
        uint16_t comp_link:1;       //b15:单元与压机通讯失败
        uint16_t fan_link:1;        //b16:与风机通讯失败
        uint16_t inv_match:1;       //b17:与驱动器不匹配
        uint16_t reserved2:16;

    }cell;
    
}UnitAlarmType;


void AlarmUnitTimerCounter(void);
uint32_t UnitAlarmStateCheck(void);


#endif//__ALARMUNIT_H
