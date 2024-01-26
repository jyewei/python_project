#ifndef __DATAUNIT_H
#define __DATAUNIT_H
#include "Common.h"
#include "config.h"





//unit init
typedef struct
{
    union                           //0:
    {
        uint16_t word;  
        struct
        {
            uint16_t unit_addr:5;       //unit address
            uint16_t capacity:3;        //
            uint16_t unit_work_state:1; //system work state//测试模式/非测试模式
            uint16_t power_on_reset:1;  //
            uint16_t refrigent_type:1;
            uint16_t pump_type:1;
            uint16_t unit_series:2;
            uint16_t temp_type:1;
            uint16_t tier_type:1;

        }bit;
    }dsw1;

    union                           //1:
    {
        uint16_t word;  
        struct
        {
            uint16_t reserved1:12;  //
            uint16_t esp_type:2;    //
            uint16_t reserved2:2;   //

        }bit;
    }dsw2;

    uint16_t main_rom_NO;           //2:
    uint16_t slave_rom_NO;          //3:
    uint16_t type_word1;            //4:
    uint16_t type_word2;            //5:
    uint16_t type_word3;            //6:
    uint16_t type_word4;            //7:
    uint16_t type_word5;            //8:
    uint16_t type_word6;            //9:
    uint16_t type_word7;            //10:
    uint16_t type_word8;            //11:
    uint16_t product_word1;         //12:
    uint16_t product_word2;         //13:
    uint16_t product_word3;         //14:
    uint16_t product_word4;         //15:
    uint16_t reserved16;            //16:
    uint16_t reserved17;            //17:
    
}UnitTableInitType;

//unit event
typedef struct
{
    union
    {
        uint16_t word;                      //0:
        struct
        {
            uint8_t unit_run_state:1;       //unit运转状态:1-run  0-stop
            uint8_t thermo_state:1;         //thermo on/off:1-thermo_on 0-thermo_off
            uint8_t cut_off_state:1;        //切离状态:1-切离中 0-通常
            uint8_t defrost_state:1;        //defrost state:1-defrost 0-normal
            uint8_t waring_state:1;         //warning state: 1-warning exist 0-no warning
            uint8_t defrost_done:1;         //除霜完成 
            uint8_t defrost_allow:1;        //除霜许可
            uint8_t night_shift_state:1;    //夜间静音
            uint8_t force_full_load_state:1;//强制满载状态
            uint8_t unit_opt_state:1;        //单元可运行状态
            uint8_t current_limit_state:1;  //电流限制
            uint8_t manual_fan_state:1;     //手动送风状态
            uint8_t alarm_state:1;          //alalrm state:1-alalrm exist 0-no alalrm
            uint8_t reserved_b13:1;         //13
            uint8_t all_cycle_alarm:1;      //
            uint8_t reserved_b15:1;         //15

        }bit;
    }state;
    
    union
    {
        uint16_t word;                      //1:
        struct
        {
            uint8_t reserved_b0:1;      //
            uint8_t reserved_b1:1;      //
            uint8_t reserved_b2:1;      //
            uint8_t reserved_b3:1;      //
            uint8_t reserved_b4:1;      //
            uint8_t reserved_b5:1;      // 
            uint8_t reserved_b6:1;      //
            uint8_t reserved_b7:1;      //
            uint8_t force_defrost:1;    //强制除霜
            uint8_t antifreeze:1;       //防冻保护
            uint8_t force_stop:1;       //强制停机
            uint8_t urgency_stop:1;     //急停中
            uint8_t firmware_update:1;  //固件升级中
            uint8_t reserved_b13:1;     //13
            uint8_t reserved_b14:1;     //
            uint8_t reserved_b15:1;     //15

        }bit;
    }state2;

    uint16_t reserved2;             //2:
    uint16_t alarm_code;            //3:
    
    union
    {
        uint16_t word;                      //1:
        struct
        {
            uint8_t reserved:4;     //
            uint8_t close_able:4;   //可关闭
            uint8_t running:4;      //运转中
            uint8_t operate:4;      //可运转
        }bit;
    }cycle_cnt;

    uint16_t pump_state;            //5:水泵状态
    uint16_t unit_run_req;          //6:运转要求
    uint16_t unit_run_mode;         //7:运转模式
    uint16_t comp_Fi_up;            //8:压缩机可加载频率之和
    uint16_t comp_Fi_down;          //9:压缩机可加载频率之和
    uint16_t comp_Fi_rating;        //10:额定频率之和
    
}UnitTableEventType;

//unit io
typedef struct
{
    union                           //0:
    {
        uint16_t word;
        struct 
        {
            uint8_t unit_flow_switch:1;     //b0:单元水流开关
            uint8_t sys_flow_switch:1;      //b1:系统水流开关
            uint8_t remote_heat_cool_switch:1;//b2:远程冷热模式切换
            uint8_t remote_di:1;            //b3:远程DI
            uint8_t urgency_off:1;          //b4:紧急联动关
            uint8_t pump_over_fb:1;         //b5:水泵过载反馈
            uint8_t sec_pump_fb:1;          //b6:二次泵反馈
            uint8_t reserved_bit7:1;        //b7:
            uint8_t water_pump:1;           //b8:水泵
            uint8_t antifreeze_heat:1;      //b9:防冻电加热
            uint8_t water_heater:1;         //b10:辅助水电加热
            uint8_t water_valve:1;          //b11:水阀
            uint8_t hot_water_valve:1;      //b12:热水水阀
            uint8_t heat_belt1_out:1;       //b13:1#底盘加热
            uint8_t heat_belt2_out:1;       //b14:2#底盘加热
            uint8_t alarm_out:1;            //b15:故障输出
            
        }bit;
    }DIO;

    union                           //1:
    {
        uint16_t word;
        struct 
        {
            uint8_t reserved_bit0:1;
            uint8_t reserved_bit1:1;
            uint8_t reserved_bit2:1;
            uint8_t reserved_bit3:1;
            uint8_t reserved_bit4:1;
            uint8_t reserved_bit5:1;
            uint8_t reserved_bit6:1;
            uint8_t reserved_bit7:1;
            uint8_t unit_hot_water_flow_switch:1;   //b8:模块热水水流开关
            uint8_t sys_hot_water_pump_switch:1;    //b9:系统热水水泵控制 //out
            uint8_t sys_hot_water_flow_switch:1;    //b10:系统热水水流开关
            uint8_t reserved_bit11:1;               //b11:
            uint8_t heat_low_pres_switch:1;         //b12:制热低压开关
            uint8_t power_protect:1;                //b13:电源保护
            uint8_t cool_low_pres_switch:1;         //b14:制冷低压开关
            uint8_t reserved_bit15:1;               //b15:
            
        }bit;
    }DIO2;

    uint16_t reserved2;                //2:

    int16_t  Tw_in;                     //3:回水温度
    int16_t  Tw_out;                    //4:出水温度
    int16_t  Tw_sys_out;                //5:系统总出水温度
    uint16_t reserved6;                 //6:
    int16_t  Tam;                       //7:室外环境温度
    uint16_t reserved8;                 //8:
    uint16_t reserved9;                 //9:
    uint16_t reserved10;                //10:
    uint16_t reserved11;                //11:
    uint16_t reserved12;                //12:
    uint16_t reserved13;                //13:
    uint32_t power_sum;                 //14~15:累计消耗电量

}UnitTableIOType;

typedef struct
{
    uint16_t total_num;             //0:故障记录总条数
    uint16_t alarm_history[32];     //1~32 :历史故障码
    uint16_t reserved33;            //33:
    uint16_t reserved34;            //34:
    uint16_t reserved35;            //35:

}UnitTableAlarmType;

//unit cmd //TODO
typedef struct
{
    union                               //0:
    {
        uint16_t word;
        struct 
        {
            uint8_t silence_set:2;          //静音设置 0-无静音设置 1-静音1 2-静音2 3-静音3
            uint8_t pump_run_set_temp:1;    //达到设定温度停机后水泵运转 0-关闭 1-开启
            uint8_t punp_run_unit_stop:1;   //关机后水泵运转 0-关闭 1-开启
            uint8_t reserved_b4:1;
            uint8_t comp_heat_belt_enable:1;//压缩机底盘加热带启用 0-关闭 1-启用
            uint8_t factory_set:1;          //出厂设定  (0->1 触发)
            uint8_t clear_sys_run_time:1;   //清除累计运行时间 (0->1 触发)
            uint8_t unit_run:1;             //0-stop 1-run
            uint8_t snow_defence:1;         //防雪 0-关闭 1-开启
            uint8_t cool_temp_select:2;     //制冷控制温度选择 0-系统回水 1-系统出水 2-变出水控制 3-变回水控制
            uint8_t heat_temp_select:2;     //制热---
            uint8_t reserved_b14:2;         //
        }bit;
    }data1;

    union                               //1:
    {
        uint16_t word;
        struct 
        {
            uint8_t water_valve_ctrl:1;     //b0:水阀控制
            uint8_t water_valve_active:1;   //b1:水阀控制有效
            uint8_t urgency_stop:1;         //b2:急停
            uint8_t firmware_update:1;      //b3:固件升级
            uint8_t new_firmware_enable:1;  //b4:新固件启用
            uint8_t firmware_type:1;        //b5:固件类型:0-主CPU,1-从CPU
            uint8_t defrost_env:2;          //b6:除霜环境:0-标准除霜 1-高湿环境
            uint8_t reserved_b7:1;          //b7
            uint8_t manual_alarm_reset:1;   //b8:手动复位
            uint8_t defrost_allow:1;        //b9:除霜许可
            uint8_t defrost_manual:1;       //b10:手动除霜
            uint8_t pump_type:1;            //b11:水泵类型
            uint8_t cycle_run_cnt:3;        //b12-b14:允许运转cycle数量
            uint8_t pump_open:1;            //b15:水泵开
            
        }bit;
    }data2;

    uint16_t reserved2;                 //2:
    uint16_t unit_run_mode;             //3:unit run mode:1-cool 2-heat 3-pump 4-antifreeze
    uint16_t temp_control_period;       //4:温控周期:10-120s(default 30s)
    int16_t  Tw_out_low_threshold1;     //5:出水温度过低报警准位 (-12~12°)(default 3)
    int16_t  Tw_out_low_threshold2;     //6:出水温度过低报警准位(乙二醇) (-12~12°)(default -12)
    int16_t  Tw_out_high_threshold;     //7:出水温度过高报警准位 (30~70°)(default 50)
    uint16_t current_capity_limit;      //8:容量(电流)限制:0-无限制 1-100% 2-90% -----  10-10%
    int16_t  Tw_set;                    //9:目标水温
    uint16_t pump_run_time_after_stop;  //10:关机后水泵运转时间(1 ~ 60mins)(default 3)
    uint16_t heat_time_defrost_interval;//11:除霜间隔累计制热时间(10 ~ 120mins)(default 40)
    uint16_t defrost_max_time;          //12:除霜最长时间(180 ~ 600s)(default 360)
    uint16_t fin_temp_defrost_finish;   //13:除霜结束翅片温度(6 ~ 18°)(default 10)
    int16_t  defrost_temp_Tam_less_zero;//14:环温小于0度除霜温度 (-10 ~ -3)(default -7)
    int16_t  defrost_temp_Tam_more_zero;//15:环温不小于0度除霜温度 (-10 ~ -3)(default -7)

    union                               //16:故障代码
    {
        uint16_t word;
        struct 
        {
            uint16_t code:12;     //b0~b12
            uint16_t reset:4;
        }bit;
    }alarm;
    int16_t  fix_temp;                  //17:空调动作回差
//18 ~ 35
    uint16_t exv_cool_init_step;        //18:EXV1/EXV2制冷初始步数 (60 ~ 480)(default 200)
    uint16_t exv_heat_init_step;        //19:EXV1/EXV2制热初始步数 (60 ~ 480)(default 200)
    uint16_t exv_cool_min_step;         //20:EXV1/EXV2制冷最小步数 (60 ~ 480)(default 70)
    uint16_t exv_heat_min_step;         //21:EXV1/EXV2制热最小步数 (60 ~ 480)(default 40)
    uint16_t comp_init_prewarm_time;    //22:系统初始上电压缩机预热时间(0 ~ 600mins)(default 180)
    int16_t  comp_fix_freq_test_mode;   //23:测试模式压缩机频率补正(-4 ~ 4)(default 0)
    int16_t  Tw_in_fix;                 //24:回水温度补偿值(-5 ~ 5)(default 0)
    int16_t  Tw_out_fix;                //25:出水温度补偿值(-5 ~ 5)(default 0)
    uint16_t manual_defrost_max_time;   //26:手动除霜最大时间 (300 ~ 600s)(default 420)
    uint16_t night_silence_time;        //27:夜间静音时长(0 ~ 12h)(default 8)
    uint16_t temp_differ_standard;      //28:标准温差和大温差(0 ~ 10°)(default 5)
    int16_t  SH_cool_tar;               //29:制冷目标过热度(-10 ~ 10°)(default 5)
    int16_t  SH_heat_tar;               //30:制热目标过热度(-10 ~ 10°)(default 4)
    int16_t  SH_sub_tar;                //31:经济器目标过热度(-10 ~ 10°)(default 5)
    int16_t  unit_freq_add;             //32:单元压缩机频率加减载量(-200 ~ 200)
    int16_t  unit_cycle_add;            //33:启停cycle数量(-4 ~ 4)
    uint16_t exv_Ini_defrost;           //34:除霜初始步数
    uint16_t watervalve_act_time;       //35:水阀动作时间
//36 ~54
    uint16_t waterflow_alarm_delay;     //36:水流开关报警延时
    int16_t  cool_tam_up_limit;         //37:制冷环温上限值
    int16_t  cool_tam_down_limit;       //37:制冷环温下限值
    int16_t  heat_tam_up_limit;         //37:制热环温上限值
    int16_t  heat_tam_down_limit;       //37:制热环温下限值

}UnitTableCmdType;

//cycle event
typedef struct
{
    union
    {
        uint16_t word;
        struct 
        {
            uint16_t reserved0_7:8; //b0~b7
            uint16_t oiling:1;      //
            uint16_t oil_request:1; //
            uint16_t comp_err:1;    //压机故障
            uint16_t opt_state:1;   //可运转状态
            

        }bit;
    }state1;
    
    uint16_t reserved1;                 //1:
    uint16_t reserved2;                 //2:
    uint16_t alarm_code;                //3:报警号
    uint16_t reserved4;                 //4:

}CycleTableEventType;

//cycle io
typedef struct
{
    union                               //0:
    {
        uint16_t word;
        struct 
        {
            uint8_t reserved:8; //b0~7
            uint8_t four_way:1;         //四通阀
            uint8_t comp_heat_belt:1;   //压缩机曲轴电加热带
            uint8_t evi_valve:1;        //EVI电磁阀
            uint8_t oil_valve:1;        //回油电磁阀
            uint8_t BPHE_valve:1;       //空调BPHE电磁阀
            uint8_t coil_valve:1;       //coil支路电磁阀
            uint8_t defrost_valve:1;    //融霜电磁阀
            uint8_t fan_protect_fb:1;   //风机保护反馈
        }bit;
    }io1;

    uint16_t io2;                 //1:
    uint16_t io3;                 //2:
    uint16_t Ps;                 //3:吸气压力Ps
    uint16_t Pd;                 //4:排气压力Pd
    uint16_t Ts;                 //5:气分进口温度Ts
    uint16_t Td;                 //6:压缩机排气温度
    uint16_t Tsub_in;            //7:经济器进口温度
    uint16_t Tsub_out;           //8:经济器出口温度
    uint16_t Tcoil;              //9:化霜温度
    uint16_t exv1_opening;       //10:电子膨胀阀1 开度
    uint16_t exv2_opening;       //11:电子膨胀阀2 开度
    uint16_t exv3_opening;       //12:电子膨胀阀3 开度
    uint16_t comp_current;       //13:压缩机电流
    uint16_t comp_rt_freq;       //14:压缩机实际频率
    uint16_t comp_set_freq;      //15:压缩机设置频率
    uint16_t fan1_rt_freq;        //16:风机实际频率
    uint16_t fan1_set_freq;       //17:风机设置频率

    uint16_t fan2_rt_freq;        //18:
    uint16_t fan2_set_freq;       //19:
    uint16_t reserved20;                //20:
    uint16_t reserved21;                //21:
    uint16_t reserved22;                //22:
    uint16_t reserved23;                //23:
    uint16_t reserved24;                //24:
    uint16_t reserved25;                //25:
    uint16_t reserved26;                //26:
    uint16_t reserved27;                //27:
    uint16_t reserved28;                //28:
    uint16_t reserved29;                //29:
    uint16_t reserved30;                //30:
    uint16_t reserved31;                //31:
    uint16_t reserved32;                //32:
    uint16_t reserved33;                //33:

}CycleTableIOType;


//cycle init_sum
typedef struct
{
    uint16_t comp_romNO;            //0:compressor rom NO
    uint16_t fan_romNO;             //1:fan rom NO
    uint32_t comp_run_time_sum;     //2:compressor run time
                                    //3:
    uint32_t comp_power_sum;        //4:compressor power sum
                                    //5:
    uint32_t sys_run_time_sum;      //6:sys run time
                                    //7:
    uint16_t reserved8;             //8:
    uint16_t reserved9;             //9:

}CycleTableInitSumType;


//cycle table
typedef struct
{
    CycleTableEventType     event;
    CycleTableIOType        io;
    CycleTableInitSumType   init_sum;

}CycleDataTableType;


//parameters about unit use
typedef struct
{
//data table    
    UnitTableInitType   init;
    UnitTableEventType  event;
    UnitTableIOType     io;
    UnitTableAlarmType  alarmh;    //alarm history
    UnitTableCmdType    cmd;
    CycleDataTableType  cycle[CYCLE_CH_MAX];

}UnitParamType;








//unit use in local
typedef struct
{
    uint8_t  work_step;             //status control
    uint8_t  last_step;
    uint8_t  set_run_mode;          //设置的运行模式
    uint8_t  rt_run_mode;           //实际的运行模式
    uint8_t  cycle_active_cnt;      //根据配置得到cycle的有效数量
    uint8_t  frame_type;            //单元框体类型

    uint8_t  chillerlink_state;     //组内通信连接状态

    int16_t Tw_in;                  //单元回水温度
    int16_t Tw_out;                 //单元出水温度
//sensor err state   
    union                           //sensor error state
    {
        uint16_t word;
        struct
        {
            uint16_t Tam:1;    //b0:


        }bit;
    }sns_err;
//IO
    union                           //unit使用的DO/DI
    {
        uint16_t word;
        struct
        {
            uint16_t board_heater:1;    //b0:板换电加热


        }bit;
    }DIO;

//    
    uint32_t alarm_state;           //unit alarm state//bit
    uint32_t pow_on_run_time;   //unit上电运行时间 //单位1mins
    uint32_t total_run_time;    //unit累计运行时间 

}UnitLocalParamType;


typedef struct
{
    UnitTableInitType   init;
    UnitTableEventType  event;
    UnitTableIOType     io;
    UnitTableAlarmType  alarmh;    //alarm history
    UnitTableCmdType    sys_cmd;   //cmd from sys     

}UnitLocalTableType;




void UnitGetParamFromConfig(void);
CycleTableEventType GetTableCyleEvent(uint8_t cycle);
CycleTableIOType GetTableCyleIO(uint8_t cycle);
CycleTableInitSumType GetTableCyleInitSum(uint8_t cycle);





#endif//__DATAUNIT_H
