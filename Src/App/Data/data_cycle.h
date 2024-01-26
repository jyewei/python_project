#ifndef __DATA_CYCLE_H
#define __DATA_CYCLE_H
#include "Common.h"
#include "data_inv.h"



typedef enum
{
    EXPV1,      //主电子膨胀阀1
    EXPV2,      //主电子膨胀阀2
    EXPV3,      //经济器电子膨胀阀
    EXPV_NO_MAX

}ExpvTypeEnum;

//cycle运转状态
typedef enum
{
    CYCLE_STATUS_INIT,  //初上电
//关机    
    CYCLE_STATUS_OFF,           
//cycle启动运转
    CYCLE_STATUS_START1,                //cycle启动1
    CYCLE_STATUS_START2,                //cycle启动2
    CYCLE_STATUS_START_DEFROST_DONE,    //除霜后启动
//cycle运转 
    CYCLE_STATUS_RUN_HEAT,              //制热运转
    CYCLE_STATUS_RUN_COOL,              //制冷运转
    CYCLE_STATUS_RUN_OIL_RETURN,        //回油运转
//cycle化霜运转状态
    CYCLE_STATUS_DEFROST_PREPARE,       //除霜准备
    CYCLE_STATUS_DIFFER_PRESSURE,       //差压控制
    CYCLE_STATUS_DEFROST_RUN,           //除霜运转
//强制化霜
    CYCLE_STATUS_DEFROST_FORCE

}CycleRunStatusEnum;





//===压缩机参数
//
typedef struct
{
//
    uint16_t set_freq;      //设置频率  unit的设置频率
    uint8_t  work_step;     //
    uint8_t  opt_state;     //压机的可运转状态

    uint8_t  freq_protect_status;   //频率保护状态
    uint16_t frq_protect_step;      //频率保护状态下的频率变化速度
    uint8_t  link_state;            //与压机的通信
//压机保护控制用
    uint8_t  voltage_unbalance;     //电源不平衡
    uint8_t  Iinv2_retreat;         //变频2次电流的缩退控制状态



//sensor
    int16_t Pd;         //排气压力
    int16_t Pd_last;    //上一次值
    int16_t Ps;         //吸气压力
    int16_t Ps_last;    //
    int16_t Td;         //排气温度  //只有Td是跟随压机数量的
    int16_t Td_last;
    int16_t Ts;         //气分进口温度
    int16_t Ts_last;    //
    int16_t Tevp;               //Ps查表得到的饱和温度值
    int16_t Tc;                 //排气饱和温度(冷凝温度)  //根据排气压力传感器检测的压力值,查表计算出温度值
    int16_t Tc_last;
    // int16_t Te;              //蒸发温度  //根据吸气压力传感器 =Tevp
    int16_t SH_Td;              //压缩机排气过热度  //Td - Tc
    int16_t SH_TS_act;          //实际吸气过热度    //Ts-Tevp
    int16_t SH_TS_act_last;
    int16_t SH_TS_Target;       //目标吸气过热度
    int16_t SH_TS_Target_last;  //

    union                           //sensor error state
    {
        uint8_t byte;
        struct
        {
            uint8_t Pd:1;    //b0:
            uint8_t Ps:1;    //b1:
            uint8_t Td:1;    //b2:
            uint8_t Ts:1;    //b3:

        }bit;
    }sns_err;


    int16_t P04_T2;     //P04保护中的T2值,在风机保护控制中用
    int16_t P04_T4;     //

//时间
    uint32_t current_run_time;     //压缩机当前运行时间     //mins
    uint32_t total_run_time;       //压缩机的累计运行时间   //mins
    uint32_t total_heat_run_time;  //压缩机制热累计运行时间


    uint32_t total_electric_power_use;     //累计消耗电力
//频率
    uint16_t Fi_c_ini;      //制冷初始频率
    uint16_t Fi_h_ini;      //制热初始频率
    uint16_t Fi_c_rating;   //制冷额定频率
    uint16_t Fi_h_rating;   //制热额定频率 
    uint16_t Fimax;         //最大频率
    uint16_t Fimin;         //最大频率

    uint16_t Fi_c_oil;      //制冷回油频率
    uint16_t Fi_h_oil;      //制热回油频率
    uint16_t Fi_ini_frost;  //制热初始除霜频率
    uint16_t Fi_frost;      //制热除霜频率

//回油
    uint16_t oil_run_time;          //回油运转时间          //mins
    uint16_t oil_total_run_time;    //cycle累计回油进入时间      //mins 
    uint16_t oil_heat_interval_time;//制热回油运转间隔时间  //mins

//除霜
    uint8_t  defrost_step;

}CompLocalParameterType;


//compressor parameters
typedef struct
{
    ComplinkParameterType   link; //通信获取的数据
    CompLocalParameterType  local;//

}CompParameterType;



//===风机参数
typedef struct
{
    uint8_t  set_fan_speed; //设置的风档
    uint8_t  rt_fan_speed;  //实际的风档
    uint8_t  link_state;    //与风机的通信连接状态
    uint16_t Fomin;         //风机档位下限值
    uint16_t Fomax;         //风机档位上限值
    uint16_t sync_freq;     //风机同步启动频率
    int16_t  radiator_temp; //散热器温度 //由fan_link中转换而来

}FanLocalParameterType;

//fan parameters
typedef struct
{
    FanlinkParameterType    link; //通信获取的数据
    FanLocalParameterType   local;//

}FanParameterType;



//===膨胀阀参数
//expv parameters
typedef struct
{
    uint8_t  enable;        //是否有效
    uint8_t  zero_set;      //零点控制,+8步
    uint8_t  zero_reset;    //置零复位
    uint8_t  flag_Tsub_err; //经济器传感器故障,用于当有故障时,需要强制关闭EXV3过关7步
    int16_t  set_exv_step;  //设置的电子膨胀阀开度
    uint16_t rt_exv_step;   //电子膨胀阀的当前开度
    uint16_t last_exv_step; //上一次的开度
    int16_t delta_op;       //开度变化量
    int16_t delta_op_last;  //上一次的开度变化量

    uint16_t exv_Ini_c;         //启动初始步数 制冷
    uint16_t exv_Ini_h;         //启动初始步数 制热
    uint16_t exv_Ini_defrost;   //启动初始步数 除霜
    uint16_t exv_max_c;         //最大开度 制冷
    uint16_t exv_max_h;         //最大开度 制热
    uint16_t exv_max_defrost;   //最大开度 除霜
    uint16_t exv_min_c;         //最小开度 制冷
    uint16_t exv_min_h;         //最小开度 制热
    uint16_t exv_min_defrost;   //最小开度 除霜

}EXpvParameterType;



//===cycle参数
typedef struct
{
    union                               //0:
    {
        uint16_t word;
        struct 
        {
            uint8_t four_way:1;         //四通阀
            uint8_t comp_heat_belt:1;   //压缩机曲轴电加热带
            uint8_t evi_valve:1;        //EVI电磁阀
            uint8_t oil_valve:1;        //回油电磁阀
            uint8_t BPHE_valve:1;       //空调BPHE电磁阀
            uint8_t coil_valve:1;       //coil支路电磁阀
            uint8_t defrost_valve:1;    //融霜电磁阀
            uint8_t fan_protect_fb:1;   //风机保护反馈
            uint8_t reserved:8;         //b0~7

        }bit;
    }dout;

    union
    {
        uint8_t byte;
        struct
        {
            uint8_t Tsub_in:1;  //sensor error state
            uint8_t Tsub_out:1;
            uint8_t Tcoil:1;
            
        }bit;
    }sns_err;
    
    int16_t Tsub_in;       //经济器进口温度
    int16_t Tsub_out;      //经济器出口温度
    int16_t Tcoil;          //化霜温度
    int16_t SH_sub;         //经济器辅路制冷剂出口的过热度  //Tsub_out-Tsub_in
    int16_t SH_sub_last;    //
    int16_t SH_sub_tar;     //经济器目标过热度 //目标过冷器过热度 [经济器目标过热度]组控器设置

}CycleIOParameterType;





// void CompRefreshInitFrequency(uint8_t comp);
void CycleGetComplinkData(uint8_t cycle);
void CycleGeFanlinkData(uint8_t cycle);
void CycleSetRunMode(uint8_t cycle,uint8_t mode);






#endif//__DATA_CYCLE_H
