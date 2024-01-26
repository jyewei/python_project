#ifndef __DATA_GROUP_H
#define __DATA_GROUP_H
#include "Common.h"


//组位控信息
typedef struct
{
    union                       //byte0
    {
        uint16_t word;
        struct
        {
            uint16_t run:1;                 //b0:运行
            uint16_t cool:1;                //b1:制冷
            uint16_t heat:1;                //b2:制热
            uint16_t pump:1;                //b3:热水
            uint16_t snow_defence:1;        //b4:防雪
            uint16_t cool_temp_chose:2;     //b5~b6:制冷控制选择:0-系统回水,1-系统出水,2-变出水控制,4-变回水控制
            uint16_t heat_temp_chose:2;     //b7~b8:制热控制选择:0-系统回水,1-系统出水,2-变出水控制,4-变回水控制
            uint16_t reserved1:2;           //b9~b10:
            uint16_t unit_time_limit:1;     //b11:机组运转时间设置启用
            uint16_t reserved2:2;           //b12~b13:
            uint16_t pump_run_temp_meet:1;  //b14:达到设定温度停机后水泵运转
            uint16_t pump_run_power_off:1;  //b15:关机后水泵运转
            
        }bit;
    }state1;

    union                       //byte1
    {
        uint16_t word;
        struct 
        {
            uint16_t eheat_enable:1;     //b0:电加热启用
            uint16_t reserved1:7;        //b1~b7:
            uint16_t defrost_mode:1;     //b8:除霜模式:0-自动,1-手动
            uint16_t reserved9:1;        //b9:
            uint16_t auto_mode:1;        //b10:运行模式:自动
            uint16_t fast_hot_water:1;   //b11:快速热水
            uint16_t silence_enable:1;   //b12:静音有效
            uint16_t reserved2:3;        //b13~b15
            
        }bit;
    }state2;

    uint16_t reserved2;         //byte2

    union                       //byte3
    {
        uint16_t word;
        struct 
        {
            uint16_t chillerlink_set:1;     //b0:组内通信节点连接设定
            uint16_t reserved1:1;           //b1:
            uint16_t reserved2:1;           //b2:
            uint16_t clear_total_run_time:1;//b3:清除累计运行时间
            uint16_t open_all_thrm:1;       //b4:智能温控器全开
            uint16_t close_all_thrm:1;      //b5:智能温控器全关
            uint16_t reserved6:12;
            
        }bit;
    }state3;

    uint16_t reserved4;         //byte4

}GroupBitControlType;


//组参数
typedef struct
{
    uint16_t unit_counter;      //byte0:组内模块数量
    uint16_t BAC_modbus_addr;   //byte1:
    uint16_t period_temp_ctrl;  //byte2:温控周期 defauf 32
    int16_t  temp_ctrl_fix;     //byte3:空调动作回差
    int16_t  temp_water_heater_open_fix;    //byte4:开辅助电加热水温回差
    int16_t  temp_water_heater_close_fix;   //byte5:关辅助电加热水温回差
    int16_t  temp_water_heater_open;        //byte6:开辅助电加热环温
    int16_t  tam_hot_water_heater_open;     //byte7:热水开电加热环温
    int16_t  temp_out_water_too_low;        //byte8:出水温度过低报警准位
    int16_t  temp_out_water_refri_too_low;  //byte9:出水温度过低报警(乙二醇)
    int16_t  temp_out_water_too_high;       //byte10:出水温度过高报警准位
    uint16_t mode_set_device;               //byte11:模式设置有效选择
    uint16_t switch_set_device;             //byte12:开关机设置有效选择
    uint16_t smart_thrm_cnt;                //byte13:智能温控器数量
    int16_t  Tw_set_cool_in;                //byte14:制冷进水目标温度
    int16_t  Tw_set_cool_out;               //byte15:制冷出水目标温度
    int16_t  Tw_set_heat_in;                //byte16:制热进水目标温度
    int16_t  Tw_set_heat_out;               //byte17:制热出水目标温度
    int16_t  Tw_set_cool_A;                 //byte18:制冷水温设定值A
    int16_t  Tw_set_cool_B;                 //byte19:制冷水温设定值B
    int16_t  Tw_set_cool_C;                 //byte20:制冷水温设定值C
    int16_t  Tw_set_cool_D;                 //byte21:制冷水温设定值D
    uint16_t reserved22;                    //byte22:
    int16_t  Tw_set_heat_A;                 //byte23:制热水温设定值A
    int16_t  Tw_set_heat_B;                 //byte24:制热水温设定值B
    int16_t  Tw_set_heat_C;                 //byte25:制热水温设定值C
    int16_t  Tw_set_heat_D;                 //byte26:制热水温设定值D
    int16_t  Tw_set_heat_F;                 //byte27:制热水温设定值F
    uint16_t time_water_valve_act;          //byte28:水阀动作时间
    uint16_t time_waterflow_alarm_delay;    //byte29:水流开关报警延时
    int16_t  Tw_out_fix;                    //byte30:总出水传感器补偿设定
    int16_t  Tw_set;                        //byte31:制冷,制热设定水温
    uint16_t time_unit_set;                 //byte32:机组运转时间设置
    uint16_t time_pump_run_sys_close;       //byte33:关机后水泵运转时间

}GroupParamType;



//模块机位控制信息
typedef struct
{
    union                       //byte0
    {
        uint16_t word;
        struct 
        {
            uint16_t no0:1;
            uint16_t no1:1;
            uint16_t no2:1;
            uint16_t no3:1;
            uint16_t no4:1;
            uint16_t no5:1;
            uint16_t no6:1;
            uint16_t no7:1;
            uint16_t no8:1;
            uint16_t no9:1;
            uint16_t no10:1;
            uint16_t no11:1;
            uint16_t no12:1;
            uint16_t no13:1;
            uint16_t no14:1;
            uint16_t no15:1;
            
        }bit;
    }unit_1;

    union                       //byte1
    {
        uint16_t word;
        struct 
        {

            uint16_t no16:1;
            uint16_t no17:1;
            uint16_t no18:1;
            uint16_t no19:1;
            uint16_t no20:1;
            uint16_t no21:1;
            uint16_t no22:1;
            uint16_t no23:1;
            uint16_t no24:1;
            uint16_t no25:1;
            uint16_t no26:1;
            uint16_t no27:1;
            uint16_t no28:1;
            uint16_t no29:1;
            uint16_t no30:1;
            uint16_t no31:1;     
        }bit;
    }unit_2;

    union                       //byte2
    {
        uint16_t word;
        struct 
        {
            uint16_t night_silence_duration:1;      //b0:夜间静音时长设定
            uint16_t capacity_limit_set_finished:1; //b1:容量(电流)限制设定完成
            uint16_t pump_run_single:1;             //b2:水泵循环单独运行
            uint16_t defrost_invertal_total_time:1; //b3:除霜间隔累计制热时间
            uint16_t defrost_max_time:1;            //b4:除霜最长时间
            uint16_t defrost_done_sink_temp:1;      //b5:除霜结束翅片温度准位
            uint16_t tam_down_zero_defrost_temp:1;  //b6:环温小于0度除霜温度准位
            uint16_t tam_up_zero_defrost_temp:1;    //b7:环温不小0度除霜温度准位
            uint16_t exv12_cool_init_step:1;        //b8:EXV1/2制冷初始步数
            uint16_t exv12_heat_init_step:1;        //b9:EXV1/2制热初始步数
            uint16_t exv12_cool_min_step:1;         //b10:EXV1/2制冷最小步数
            uint16_t exv12_heat_min_step:1;         //b11:EXV1/2制热最小步数
            uint16_t exv12_defrost_init_step:1;     //b12:EXV1/2除霜初始步数
            uint16_t comp_preheat_time:1;           //b13:系统初始上电压缩机预热时间
            uint16_t test_mdoe_freq_bias:1;         //b14:测试模式频率偏差
            uint16_t Tw_in_fix:1;                   //b15:回水传感器补偿     
        }bit;
    }event1;

    union                       //byte3
    {
        uint16_t word;
        struct 
        {
            uint16_t Tw_out_fix:1;              //b0:出水传感器补偿 
            uint16_t manual_defrost_max_time:1; //b1:手动除霜最长时间
            uint16_t standard_temp_differ:1;    //b2:标准温度差和大温差
            uint16_t cool_dst_temp_over:1;      //b3:制冷目标过热度
            uint16_t defrost_type:1;            //b4:除霜类型
            uint16_t manual_defrost:1;          //b5:手动除霜
            uint16_t heat_dst_temp_over:1;      //b6:制热目标过热度
            uint16_t economizer_dst_temp_over:1;//b7:经济器目标过热度
            uint16_t factory_reset:1;           //b8:恢复出厂设定
            uint16_t clear_alarm_history:1;     //b9:清除历史故障
            uint16_t get_alarm_history:1;       //b10:获取历史故障
            uint16_t water_valve_time:1;        //b11:水阀动作时间
            uint16_t wateflow_switch_alarm_delay:1; //b12:水流开关报警延时
            uint16_t silence_set:1;             //b13:静音设置
            uint16_t alarm_manual_reset:1;      //b14:手动恢复故障
            uint16_t unit_state_monitor_reset:1;//b15:模块机状态位复位
            
        }bit;
    }event2;

    union                       //byte4
    {
        uint16_t word;
        struct 
        {
            uint16_t heat_belt_enable:1;      //b0:压缩机底盘加热带启用
            uint16_t pump_run_switch_off:1;     //b1:关机后水泵运转
            uint16_t tam_cool_up_limit:1;       //b2:禁止制冷室外环温上限设定
            uint16_t tam_cool_down_limit:1;     //b3:禁止制冷室外环温下限设定
            uint16_t tam_heat_up_limit:1;       //b4:禁止制热室外环温上限设定
            uint16_t tam_heat_down_limit:1;     //b5:禁止制热室外环温下限设定
            uint16_t snow_defence:1;            //b6:外机防雪
            uint16_t reserved7:1;      //b7:
            uint16_t reserved8:1;      //b8:
            uint16_t reserved9:1;      //b9:
            uint16_t reserved10:1;      //b10:
            uint16_t reserved11:1;      //b11:
            uint16_t reserved12:1;      //b12:
            uint16_t reserved13:1;      //b13:
            uint16_t reserved14:1;      //b14:
            uint16_t reserved15:1;      //b15:
            
        }bit;
    }event3;

}GroupUnitBitControlType;


typedef struct
{
    uint16_t defrost_invertal_total_time;   //byte0:除霜间隔累计制热时间
    uint16_t defrost_type;                  //byte1:除霜类型
    uint16_t defrost_max_time:1;            //byte2:除霜最长时间
    uint16_t defrost_done_sink_temp:1;      //byte3:除霜结束翅片温度准位
    uint16_t tam_down_zero_defrost_temp:1;  //byte4:环温小于0度除霜温度准位
    uint16_t tam_up_zero_defrost_temp:1;    //byte5:环温不小0度除霜温度准位
    uint16_t exv12_cool_init_step:1;        //byte6:EXV1/2制冷初始步数
    uint16_t exv12_heat_init_step:1;        //byte7:EXV1/2制热初始步数
    uint16_t exv12_cool_min_step:1;         //byte8:EXV1/2制冷最小步数
    uint16_t exv12_heat_min_step:1;         //byte9:EXV1/2制热最小步数
    uint16_t exv12_defrost_init_step:1;     //byte10:EXV1/2除霜初始步数
    uint16_t comp_preheat_time:1;           //byte11:系统初始上电压缩机预热时间
    uint16_t test_mdoe_freq_bias:1;         //byte12:测试模式频率偏差
    uint16_t Tw_in_fix:1;                   //byte13:回水传感器补偿   
    uint16_t Tw_out_fix:1;                  //byte14:出水传感器补偿 
    uint16_t manual_defrost_max_time:1;     //byte15:手动除霜最长时间
    uint16_t time_nightsilence:1;           //byte16:夜间静音时长 
    uint16_t standard_temp_differ:1;        //byte17:标准温度差和大温差
    uint16_t cool_dst_temp_over:1;          //byte18:制冷目标过热度            
    uint16_t heat_dst_temp_over:1;          //byte19:制热目标过热度        
    uint16_t economizer_dst_temp_over:1;    //byte20:经济器目标过热度
    uint16_t current_capacity_limit;        //byte21:容量电流限制
    uint16_t water_valve_time:1;            //byte22:水阀动作时间
    uint16_t wateflow_switch_alarm_delay:1; //byte23:水流开关报警延时
    uint16_t silence_set:1;                 //byte24:静音设置
    uint16_t tam_cool_up_limit:1;           //byte25:禁止制冷室外环温上限设定
    uint16_t tam_cool_down_limit:1;         //byte26:禁止制冷室外环温下限设定
    uint16_t tam_heat_up_limit:1;           //byte27:禁止制热室外环温上限设定
    uint16_t tam_heat_down_limit:1;         //byte28:禁止制热室外环温下限设定
    uint16_t alarm_manual_reset:1;          //byte29:手动恢复故障
    uint16_t unit_start_priority:1;         //byte30:模块机启动优先级

}GroupUnitParamType;






//group link use
typedef struct
{
    GroupBitControlType     grp_ctrl;  //组控制信息
    GroupParamType          grp_param; //组参数
    GroupUnitBitControlType unit_ctrl; //单元控制信息
    GroupUnitParamType      unit_param;//单元参数

}GrouplinkParameterType;








#endif//__DATA_GROUP_H
