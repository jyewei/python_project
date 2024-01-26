#ifndef __ALARMCHECK_H
#define __ALARMCHECK_H
#include "common.h"







// //压机变频故障
// typedef enum
// {
//     COMP_ERR_HW_OVERCURRENT     = 0x00000001,   //b0:驱动器硬件过流保护
//     COMP_ERR_DRIVER             = 0x00000002,   //b1:驱动失败,转速异常
//     COMP_ERR_OVERCURRENT_STOP   = 0x00000004,   //b2:压缩机过电流停机保护
//     COMP_ERR_CURRENT_SENSOR     = 0x00000008,   //b3:压缩机电流采样故障
//     COMP_ERR_SINK_OVERTEMP      = 0x00000010,   //b4:散热器/IPM过热停机保护
//     COMP_ERR_PRECHARGE_FAIL     = 0x00000020,   //b5:预充电失败
//     COMP_ERR_BUS_OVERVOLTAGE    = 0x00000040,   //b6:母线过压保护
//     COMP_ERR_BUS_UNDERVOLTAGE   = 0x00000080,   //b7:母线欠压保护
//     COMP_ERR_AC_UNDERVOLTAGE    = 0x00000100,   //b8:交流输入欠压
//     COMP_ERR_AC_OVERCURRENT     = 0x00000200,   //b9:交流输入过流保护
//     COMP_ERR_PARITY_FAIL        = 0x00000400,   //b10:校验失败
//     COMP_ERR_AC_HW_OVERCURRENT  = 0x00000800,   //b11:交流输入硬件过流保护
//     COMP_ERR_SINK_SENSOR        = 0x00001000,   //b12:散热器温度传感器故障
//     COMP_ERR_PFC_OVERCURRENT    = 0x00002000,   //b13:PFC过流保护
//     COMP_ERR_RESERVED1          = 0x00004000,   //b14:
//     COMP_ERR_RESERVED2          = 0x00008000,   //b15:

//     COMP_ERR_MAX = 18

// }CompInverterAlarmEnum;







//系统故障
typedef enum
{
    SYS_ERR_SENSOR_TW_SYS_OUT       = 0x00000001,   //bit0:总出水温度传感器故障
    SYS_ERR_COOL_TW_OUT_TEMP_LOW    = 0x00000002,   //bit1:制冷总出水温度过低
    SYS_ERR_HEAT_TW_OUT_TEMP_HIGH   = 0x00000004,   //bit2:制热总出水温度过高
    SYS_ERR_WATERFLOW_SWITCH        = 0x00000008,   //bit3:系统总水流开关异常
    SYS_ERR_PUBLIC_PUMP_OVERLOAD    = 0x00000010,   //bit4:共用水泵过载
    SYS_ERR_GROUPLINK               = 0x00000020,   //bit5:线控器通信故障
    SYS_ERR_THRMLINK                = 0x00000040,   //bit6:智能温控器通信异常
    SYS_ERR_BASLINK                 = 0x00000080,   //bit7:BAS通信异常
    SYS_ERR_CCLINK                  = 0x00000100,   //bit8:CC通信异常
    SYS_ERR_WATER_FREEZE            = 0x00000200,   //bit9:冻结故障
    SYS_ERR_TIMEOUT                 = 0x00000400,   //bit10:时限保护
    SYS_ERR_UNIT_CNT                = 0x00000800,   //bit11:单元数量不匹配
    SYS_ERR_UNIT_ADDR_DUPLICATE     = 0x00001000,   //bit12:单元地址冲突
    SYS_ERR_THRM_CNT                 = 0x00002000,   //bit13:智能温控器数量不匹配
    SYS_ERR_THRM_ADDR_DUPLICATE     = 0x00004000,   //bit14:智能温控器地址冲突
    SYS_ERR_POW                     = 0x00008000,   //bit15:主机电源故障
    SYS_ERR_UNIT_BLEND              = 0x00010000,   //bit16:单元混拼故障
    SYS_ERR_CHILLERSLAVELINK        = 0x00020000,   //bit17:主从板通信故障
    SYS_ERR_HW_SW_MATCH             = 0x00040000,   //bit18:硬件和软件不匹配  
    
    SYS_ERR_MAX = 19

}SysAlarmEnum;


//cycle
typedef union
{
    uint32_t data;
    struct
    {
        uint16_t sensor_Tw_sys_out:1;       //b0:总出水温度传感器故障
        uint16_t cool_tw_out_temp_low:1;    //b1:制冷总出水温度过低
        uint16_t heat_tw_out_temp_high:1;   //b2:制热总出水温度过高
        uint16_t waterflow_switch:1;        //b3:系统总水流开关异常
        uint16_t public_pump_overload:1;    //b4:共用水泵过载
        uint16_t grouplink:1;               //b5:线控器通信故障
        uint16_t thrmlink:1;                //b6:智能温控器通信异常
        uint16_t BASlink:1;                 //b7:BAS通信异常
        uint16_t centrelink:1;              //b8:CC通信异常
        uint16_t water_freeze:1;            //b9:冻结故障
        uint16_t timeout:1;                 //b10:时限保护
        uint16_t unit_cnt:1;                //b11:单元数量不匹配
        uint16_t unit_addr_duplicate:1;     //b12:单元地址冲突
        uint16_t thrm_cnt:1;                //b13:智能温控器数量不匹配
        uint16_t thrm_addr_duplicate:1;     //b14:智能温控器地址冲突
        uint16_t pow:1;                     //b15:主机电源故障
        uint16_t unit_blend:1;              //b16:单元混拼故障
        uint16_t chillerslavelink:1;        //b17:主从板通信故障
        uint16_t hw_sw_match:16;            //b18:硬件和软件不匹配

    }cell;
    
}SysAlarmType;











uint32_t GetFanAlarmState(uint8_t cycle,uint8_t fan);
uint8_t CheckFanAlarmState(uint8_t cycle,uint8_t fan);
uint32_t GetCompAlarmState(uint8_t cycle,uint8_t comp);
uint8_t CheckCompAlarmState(uint8_t cycle,uint8_t comp);

uint32_t GetCycleAlarmUpdate(uint8_t cycle);
uint8_t CheckCycleAlarmState(uint8_t cycle);
uint32_t GetUnitAlarmState(void);
uint8_t CheckUnitAlarmState(void);
uint32_t GetkSysAlarmState(void);
uint8_t CheckkSysAlarmState(void);


void AlarmCheck(uint8_t taskNo,uint8_t flag_init);

#endif//__ALARMCHECK_H
