#ifndef __DATAINV_H
#define __DATAINV_H
#include "Common.h"
/*****************************************
 * 风机地址为奇数,压机地址为偶数
 * 
 * 
 * 
 * **************************************/





//单cycle中最多2个风机
typedef enum
{
    FAN_1,
    FAN_2,

    FAN_NO_MAX

}FanEnum;


//单cycle可能有多个压缩机,预留
typedef enum
{
    COMP_1,
    COMP_NO_MAX

}CompEnum;



//===压缩机通信
//register 1000
typedef struct
{    
    uint16_t freq_set;      //0:value:0-2000 = 0.0-200.0Hz
    union
    {
        uint16_t word;      //1:command
        struct
        {
            uint16_t start:1;                   //b0: 启动/停止
            uint16_t precharge_enable:1;        //b1: 预充电使能
            uint16_t reserved:14;               //

        }bit;
    }cmd_set;
    
    uint16_t reserved2;         //2:
    uint16_t compressor_type;   //3:压缩机电机型号
    uint16_t fre_step;          //4:升/降速频率设置
    uint16_t reserved5;         //5:
    uint16_t reserved6;         //6:
    uint16_t reserved7;         //7:
    uint16_t reserved8;         //8:
    uint16_t reserved9;         //9:
    uint16_t reserved10;        //10:
    uint16_t reserved11;        //11:
    uint16_t reserved12;        //12:
    uint16_t reserved13;        //13:
    uint16_t reserved14;        //14:
    uint16_t reserved15;        //15:
//register end 1015
}CompControlType;

//register start 1100
typedef struct
{

    union                   //0:
    {
        uint16_t word;
        struct
        {
            uint16_t run:1;             //b0:运行状态
            uint16_t reserved1:1;       //b1:
            uint16_t alarm:1;           //b2:故障状态
            uint16_t freq_fall_overlocad:1; //b3:过载降频状态
            uint16_t freq_steady:1;     //b4:频率稳定状态
            uint16_t freq_rise:1;       //b5:升频状态
            uint16_t freq_fall:1;       //b6:降频状态
            uint16_t reserved7:1;       //b7:
            uint16_t precharge_done:1;  //b8:预充电完成状态
            uint16_t reserved9:1;       //b9:
            uint16_t reserved10:1;      //
            uint16_t reserved11:1;      //
            uint16_t reserved12:1;      //
            uint16_t reserved13:1;      //
            uint16_t voltage_unbalance:1;  //b14:电源电压不平衡
            uint16_t rise_forbid:1;  //b15:上升禁止
        }bit;
    }state;
    
    union                   //1:
    {
        uint16_t word;
        struct
        {
            uint16_t overload_current:1;    //b0:过流与过载
            uint16_t rotate_speed_err:1;    //b1:转速异常
            uint16_t mcu_reset:1;           //b2:mcu 复位
            uint16_t power_in_err:1;        //b3:电源输入异常
            uint16_t current_sensor_err:1;  //b4:电流传感器异常
            uint16_t over_temp:1;           //b5:过温
            uint16_t precharge_err:1;       //b6:预充电故障
            uint16_t over_voltage:1;        //b7:过电压
            uint16_t under_voltage:1;       //b8:欠电压
            uint16_t reserved9:1;           //b9:
            uint16_t reserved10:1;          //
            uint16_t ISPM_err:1;            //b11:ISPM异常
            uint16_t reserved12:1;          //b12:
            uint16_t reserved13:1;          //
            uint16_t discharge_pressure:1;  //b14:排气压力高故障
            uint16_t reserved14:1;          //
        }bit;
    }alarm_state1;

    union                   //2:
    {
        uint16_t word;
        struct
        {
            uint16_t overcurrent_moment:1;  //b0:瞬时过流
            uint16_t power_module_heating:1;//b1:功率模块发热
            uint16_t IPM_error:1;           //b2:
            uint16_t short_circuit_ground:1;//b3:对地短路
            uint16_t reserved94:1;          //b4:
            uint16_t off_sync_check:1;      //b5:脱调检出
            uint16_t freq_err:1;            //b6:转速指令错误
            uint16_t precharge_fail:1;      //b7:预充电失败
            uint16_t relay_not_close:1;     //b8:继电器未闭合
            uint16_t reserved9:1;           //b9:
            uint16_t reserved10:1;          //b10:
            uint16_t _63H:1;                //b11:63H
            uint16_t _63H_circuit_err:1;    //b12:63H电路异常
            uint16_t _63H_reset_ahead:1;    //b13:63H提前复位
            uint16_t power_in_drop:1;       //b14:电源输入瞬时跌落
            uint16_t in_phase_open:1;       //b15:输入缺相
        }bit;
    }alarm_state2;

    uint16_t alarm_state3;  //3:
    union                   //4:
    {
        uint16_t word;
        struct
        {
            uint16_t sw1:1;         //b0:
            uint16_t sw2:1;         //b1:
            uint16_t sw3:1;         //b2:
            uint16_t sw4:1;         //b3:
            uint16_t sw5:1;         //b4:
            uint16_t sw6:1;         //b5:
            uint16_t sw7:1;         //b6:
            uint16_t sw8:1;         //b7:
            uint16_t reserved8:1;   //b8:
            uint16_t reserved9:1;   //b9:
            uint16_t reserved10:1;  //b10:
            uint16_t reserved11:1;  //b11:
            uint16_t reserved12:1;  //b12:
            uint16_t reserved13:1;  //b13:
            uint16_t reserved14:1;  //b14:
            uint16_t reserved15:1;  //b15:
        }bit;
    }switch_state;

    uint16_t freq_output;       //5:输出机械频率 0-2000 = 0.0-200.0Hz
    uint16_t freq_max;          //6:允许的最大频率 0-2000 = 0.0-200.0Hz
    uint16_t reseerved2_7;      //7:
    uint16_t ac_current_in;     //8:交流输入电流值 0-8000 = 0.0-80.00A
    uint16_t reseerved2_9;      //9:
    uint16_t current;           //10:电流值 0-8000 = 0.0-80.00A
    uint16_t dc_bus_voltage;    //11:直流母线电压 0-10000 = 0.0-1000.0Vdc
    uint16_t PIM_temp;          //12:PIM温度 0-2550 = -55.0 - 200.0℃
    uint16_t reseerved2_13;     //13:
    uint16_t reseerved2_14;     //14:
    uint16_t reseerved2_15;     //15:
//end register 1115
}CompStateType;


//start register 2000
typedef struct
{
    uint16_t output_power;      //0:输出功率
    uint16_t reseerved3_1;      //1:
    uint16_t freq_min;          //2
    uint16_t reseerved3_3;      //3:
    uint16_t reseerved3_4;      //4:
    uint16_t reseerved3_5;      //5:
    uint16_t reseerved3_6;      //6:
    uint16_t reseerved3_7;      //7:
    uint16_t reseerved3_8;      //8:
    uint16_t reseerved3_9;      //9:
    uint16_t mcu_flash_checksum;//10:
    uint16_t eeprom_checksum;   //11:
    uint16_t motor_type_in;     //12:压缩机或风机电机型号定义(主控写入)
    uint16_t software_version;  //13:bit15~bit0 驱动芯片软件版本 eg.0x0110 -> V1.10
    uint16_t reseerved3_14;     //14:
    uint16_t reseerved3_15;     //15:

}CompParamType;


//===风机通信
//register 1000
typedef struct
{    
    uint16_t freq_set;      //0:value:0-2000 = 0-2000rpm,=0时表示停机,>0时表示启动运行
    union
    {
        uint16_t word;      //1:command
        struct
        {
            uint16_t foc_start:1;               //b0:启动FOCx/APFCx
            uint16_t precharge_enable:1;        //b1:预充电使能(APFC无效)
            uint16_t heat_enable:1;             //b2:电加热使能(APFC无效)
            uint16_t heat_belt_enable:1;        //b3:电加热带使能
            uint16_t reserved:12;               //b4

        }bit;
    }cmd_set;
    
    uint16_t work_mode;         //2:工作模式:0-正常模式 5-FCT模式 A-StrifeTest模式 0x50-内部操作模式 default:0
    uint16_t compressor_type;   //3:压缩机电机型号
    uint16_t fre_step_up;       //4:升频速率设置 50-100=50-100rpm/s,default:50rpm
    uint16_t fre_step_down;     //5:降速频率设置 50-100=50-100rpm/s,default:100rpm
    uint16_t reserved6;         
    union
    {
        uint16_t word;          //6:
        struct
        {
            uint16_t hardware:8;        //b0~b7:硬件型号
            uint16_t pow:4;             //b8~b11:电源制式及电压等级
            uint16_t reserved:4;        //

        }bit;
    }type;

    uint16_t reserved7;         //7:输入电流档位(驱动存储,停机时写入):1-100%MAX Current;2-80%Max;4-70%Max;4-60%Max,other:100%
    uint16_t reserved8;         //8:
    uint16_t reserved9;         //9:
    uint16_t reserved10;        //10:
    uint16_t reserved11;        //11:
    uint16_t reserved12;        //12:
    uint16_t reserved13;        //13:
    uint16_t reserved14;        //14:
    uint16_t reserved15;        //15:
//register end 1015
}FanControlType;

//register start 1100
typedef struct
{

    union                   //0:
    {
        uint16_t word;
        struct
        {
            uint16_t foc_run:1;         //b0:FOCx/APFCx运行状态
            uint16_t phase_pfc:1;       //b1:单相PFC运行状态
            uint16_t foc_alarm:1;       //b2:FOCx/APFCx故障状态
            uint16_t foc_warn:1;        //b3:FOCx/APFCx报警状态
            uint16_t foc_freq_steady:1; //b4:FOCx/APFCx频率稳定状态
            uint16_t foc_freq_rise:1;   //b5:FOCx/APFCx升频状态
            uint16_t foc_freq_fall:1;   //b6:FOCx/APFCx降频状态
            uint16_t foc_flux_weaken:1; //b7:FOCx/APFCx弱磁工作状态
            uint16_t precharge_done:1;  //b8:预充电完成状态
            uint16_t charge_done:1;     //b9:放电完成状态
            uint16_t pre_heat:1;        //b10:预加热工作状态
            uint16_t fct_work:1;        //b11:FCT运行状态
            uint16_t reserved12:1;      //b12:
            uint16_t reserved13:1;      //b13:
            uint16_t reserved14:1;      //b14:
            uint16_t reserved15:1;      //b15:
        }bit;
    }state;
    
    union                   //1:
    {
        uint16_t word;
        struct
        {
            uint16_t foc_over_current:1;    //b0:FOCx/APFCx过流
            uint16_t foc_driver_err:1;      //b1:FOCx/APFCx驱动失败
            uint16_t in_driver_in:1;        //b2:驱动内部故障
            uint16_t power_in_phase_err:1;  //b3:输入缺相故障
            uint16_t current_sensor_err:1;  //b4:电流传感器故障
            uint16_t foc_over_temp:1;       //b5:FOCx/APFCx过温故障
            uint16_t precharge_err:1;       //b6:预充电故障
            uint16_t bus_over_voltage:1;    //b7:母线过电压
            uint16_t bus_under_voltage:1;   //b8:母线欠电压
            uint16_t heat_sink_over_temp:1; //b9:散热器过温故障
            uint16_t ac_in_over_currrent:1; //b10:交流输入过流(硬件+软件)
            uint16_t i_sensor_check_err:1;  //b11:电流传感器校验失败
            uint16_t driver_type_err:1;     //b12:驱动器或电机型号错误
            uint16_t foc_sensor_err:1;      //b13:FOCx/APFCx温度传感器故障
            uint16_t discharge_pressure:1;  //b14:排气压力高故障
            uint16_t comm_err:1;            //b15:通讯故障
        }bit;
    }alarm_state1;

    union                   //2:
    {
        uint16_t word;
        struct
        {
            uint16_t foc_sw_over_current:1;     //b0:FOCx/APFCx软件过流
            uint16_t foc_overload:1;            //b1:FOCx/APFCx过载
            uint16_t mcu_comparer_A_protect:1;  //b2:MCU内部比较器A保护
            uint16_t mcu_comparer_B_protect:1;  //b3:MCU内部比较器B保护
            uint16_t mcu_comparer_C_protect:1;  //b4:MCU内部比较器C保护
            uint16_t foc_speed_err:1;           //b5:FOCx转速异常
            uint16_t foc_out_phase_err:1;       //b6:FOCx输出缺相
            uint16_t mcu_ram:1;                 //b7:
            uint16_t mcu_register:1;            //b8:
            uint16_t mcu_clock:1;               //b9:
            uint16_t mcu_flash:1;               //b10:
            uint16_t eeprom_check:1;            //b11:eeprom校验
            uint16_t v_ref_err:1;               //b12:参考电压异常
            uint16_t sw_protect_err:1;          //b13:软件保护模块异常
            uint16_t ac_in_sw_over_current:1;   //b14:电源输入瞬时跌落
            uint16_t ac_in_overload:1;          //b15:交流输入过载
        }bit;
    }alarm_state2;

    union                   //3:
    {
        uint16_t word;
        struct
        {
            uint16_t foc_current_heavy:1;       //b0:FOCx/APFCx电流大报警
            uint16_t foc_temp_high:1;           //b1:FOCx/APFCx温度高
            uint16_t in_current_heavy:1;        //b2:输入电流大
            uint16_t reserved:13;
        }bit;
    }alarm_state3;

    union                   //4:
    {
        uint16_t word;
        struct
        {
            uint16_t sw1:1;         //b0:
            uint16_t sw2:1;         //b1:
            uint16_t sw3:1;         //b2:
            uint16_t sw4:1;         //b3:
            uint16_t reserved4:1;   //b4:
            uint16_t reserved5:1;   //b5:
            uint16_t reserved6:1;   //b6:
            uint16_t reserved7:1;       //b7:
            uint16_t parallel_relay:1;  //b8:并联继电器状态
            uint16_t serial_relay:1;    //b9:串联继电器状态
            uint16_t reserved10:1;  //b10:
            uint16_t reserved11:1;  //b11:
            uint16_t reserved12:1;  //b12:
            uint16_t reserved13:1;  //b13:
            uint16_t reserved14:1;  //b14:
            uint16_t high_voltage_switch:1;  //b15:高压开关状态
        }bit;
    }switch_state;

    uint16_t foc_freq_output;   //5:输出机械频率 0-2000 = 0-2000rpm
    uint16_t foc_freq_max;      //6:允许的最大频率 0-2000 = 0-2000rpm
    uint16_t ac_in_voltage;     //7:交流输入电压值(RMS) 0-10000=0-1000.0V
    uint16_t ac_in_current;      //8:交流输入电流值 0-5000 = 0.0-50.00A
    uint16_t foc_voltage_out;   //9:FOCx输出/APFCx输入电压值0-10000=0-1000.0V
    uint16_t foc_phase_current; //10:FOCx/APFCx相电流值 0-5000=0-50.00A
    uint16_t dc_bus_voltage;    //11:直流母线电压 0-10000 = 0.0-1000.0Vdc
    uint16_t foc_temp;          //12:FOCx/APFCx温度 0-2550=-55.0-200.0°
    uint16_t heat_sink_temp;    //13:散热器温度 0-2550=-55.0-200.0°
    uint16_t L_temp;            //14:电感器温度 0-2550=-55.0-200.0°
    uint16_t total_run_time;    //15:0-65000=0-65000hours,掉电保持,溢出维持最大值
//end register 1115
}FanStateType;


//start register 2000
typedef struct
{
    uint16_t foc_output_power;  //0:输出功率:0-50000=0-50000w
    uint16_t mcu_temp;          //1:0-2550=-55.0-200.0°
    uint16_t foc_freq_min;      //2:FOCx允许最小频率0-2000=0.0-200.0
    uint16_t current_in_speed;  //3:输入电流档位 1-100%Max,2-80%,3-70%,4-60%,other-100%
    uint16_t reseerved3_4;      //4:
    uint16_t reseerved3_5;      //5:
    uint16_t reseerved3_6;      //6:
    uint16_t reseerved3_7;      //7:
    uint16_t reseerved3_8;      //8:
    uint16_t mcu_flash_checksumH;//9:mcu flash checksum HI
    uint16_t mcu_flash_checksumL;//10:mcu flash checksum LOW
    uint16_t eeprom_checksum;   //11:
    uint16_t motor_type_in;     //12:压缩机或风机电机型号定义(主控写入)
    uint16_t software_version;  //13:bit15~bit0 驱动芯片软件版本 eg.0x0110 -> V1.10
    uint16_t eeprom_ver;        //14:eeprom版本
    uint16_t driver_type;       //15:驱动器类型

}FanParamType;



//compressor parameters
typedef struct
{
    CompControlType ctrl;
    CompStateType   state;
    CompParamType   param;

}ComplinkParameterType;


//fan parameters
typedef struct
{
    FanControlType ctrl;
    FanStateType   state;
    FanParamType   param;

}FanlinkParameterType;



#endif//__DATAINV_H
