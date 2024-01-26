/*********************************************************************
 * 1.单元地址:0~31
 * 2.所有的温度为10倍值(0.1℃)
 * 3.压力为KPa(0.001MPa)
 * 4.频率采用0.1Hz
 * 5.电流值0.01A
 * 
 * @note1:组内通信的sensor值为采样值,在系统或单元要处理时再添加系数或补偿值
 * @note2:sensor异常时更新异常值到I/O部分,atw结构体内sensor值不更新,可根据采样值进行异常判断,而不影响系统处理
***********************************************************************/
#ifndef __DATA_H
#define __DATA_H
#include "Common.h"
#include "data_unit.h"
#include "data_cycle.h"
#include "config.h"
#include "data_group.h"
#include "IOProcess.h"




#define SW_VERSION          ((uint16_t)0x9001)   //version
#define CHILLER_MAIN_ADDR   ((uint8_t)0x00)         //chiller main 
#define CHILLER_SUB_MAX     ((uint8_t)31)           //


//eeprom parameters
typedef struct 
{
    uint8_t  reserved0;
//chillerlink     
    uint32_t chiller_recv_addr_act; //








}EepromParameterType;






//sys parameters
typedef struct
{
    uint8_t work_step;
    uint8_t last_step;
    uint8_t sys_set_mode;           //sys的设置模式
    uint8_t sys_rt_mode;            //sys的实际运转模式
    uint32_t alarm_state;
//通信相关
    uint8_t  chillerlink_cnt_err;   //单元数量不匹配
    uint8_t  chillerlink_addr_err;  //单元地址冲突
    uint8_t  grouplink_state;       //组控器通信连接状态
    uint8_t  centrelink_state;      //集控器通信连接状态
    uint8_t  BASlink_state;         //BAS
    uint8_t  thrmlink_state;        //智能温控器
    uint8_t  thrmlink_cnt_err;      //智能温控器数量不匹配
    uint8_t  thrmlink_addr_err;     //智能温控器地址冲突
    uint8_t  slavelink_state;       //主从板通信故障

//水温   
    int16_t Tw;                 //实际水温
    int16_t cool_Tw_set;        //制冷设定水温
    int16_t heat_Tw_set;        //制热设定水温
    int16_t Tw_sys_out;         //系统总出水温度
    int16_t Tw_sys_in;          //系统总回水温度

//
    uint32_t total_run_time;    //机组运行时间      //mins
    uint16_t standby_run_time;  //系统待机运行时间  //
    uint16_t pump_run_time;     //水泵运行时间      //
    uint16_t pump_stop_time;    //水泵停止时间      //


//unit sys local
    uint8_t unit_active_cnt;    //单元有效数
    struct
    {
        struct
        {
            uint16_t comp_run_time; //压机当前运行时间 //mins


        }cycle[CYCLE_CH_MAX];
        
        uint8_t active;             //有效
        uint8_t link_state;         //通信状态
        uint8_t cycle_active_cnt;   //单元中cycle有效数
        uint8_t comp_type;          //压机种类 //与单元容量/型号有关


    }unit[CHILLER_CH_MAX];
    



}SysLocalParameterType;


//参数设置 //来自外部控制器
typedef struct
{
    union
    {
        uint32_t data;
        struct
        {
            uint16_t snow_defence:1;        //b0:防雪
            uint16_t cool_temp_chose:2;     //b1~b2:制冷控制选择:0-系统回水,1-系统出水,2-变出水控制,4-变回水控制
            uint16_t heat_temp_chose:2;     //b3~b4:制热控制选择:0-系统回水,1-系统出水,2-变出水控制,4-变回水控制
            uint16_t unit_time_limit:1;     //b5:机组运转时间设置启用
            uint16_t pump_run_temp_meet:1;  //b6:达到设定温度停机后水泵运转
            uint16_t pump_run_power_off:1;  //b7:关机后水泵运转
            uint16_t eheat_enable:1;        //b8:电加热启用
            uint16_t defrost_mode:1;        //b9:除霜模式:0-自动,1-手动
            uint16_t auto_mode:1;           //b10:运行模式:自动
            uint16_t fast_hot_water:1;      //b11:快速热水
            uint16_t silence_enable:1;      //b12:静音有效
            uint16_t chillerlink_set:1;     //b13:组内通信节点连接设定
            uint16_t clear_total_run_time:1;//b14:清除累计运行时间
            uint16_t open_all_thrm:1;       //b15:智能温控器全开
            uint16_t close_all_thrm:1;      //b16:智能温控器全关
            uint16_t reserved:15;

        }cell;
    }set;
    
    uint16_t unit_counter;                  //byte0:组内模块数量
    uint16_t BAC_modbus_addr;               //byte1:
    uint16_t period_temp_ctrl;              //byte2:温控周期 defauf 32
    int16_t  temp_ctrl_fix;                 //byte3:空调动作回差
    int16_t  temp_water_heater_open_fix;    //byte4:开辅助电加热水温回差
    int16_t  temp_water_heater_close_fix;   //byte5:关辅助电加热水温回差
    int16_t  temp_water_heater_open;        //byte6:开辅助电加热环温
    int16_t  tam_hot_water_heater_open;     //byte7:热水开电加热环温
    int16_t  temp_out_water_too_low;        //byte8:出水温度过低报警准位
    int16_t  temp_out_water_refri_too_low;  //byte9:出水温度过低报警(乙二醇)
    int16_t  temp_out_water_too_high;       //byte10:出水温度过高报警准位

    union
    {
        uint16_t word;
        struct
        {
            uint8_t mode_group:1;       //模式选择
            uint8_t mode_cc:1;
            uint8_t mode_BAS:1;
            uint8_t mode_thrm:1;
            uint8_t reserved_mode:4;

            uint8_t switch_group:1;     //开关机选择
            uint8_t switch_cc:1;
            uint8_t switch_BAS:1;
            uint8_t switch_thrm:1;
            uint8_t reserved_switch:4;

        }bit;
        
    }device_valid;
    // uint16_t mode_set_device;               //byte11:模式设置有效选择
    // uint16_t switch_set_device;             //byte12:开关机设置有效选择
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

    

}SysParameterSetType;


//sys parameters
typedef struct
{
//sys table,from chillerlink    
    struct
    {
        UnitParamType   unit[CHILLER_CH_MAX];
    }table;

    SysLocalParameterType   local;
    SysParameterSetType     param;      //参数设置,来自外部控制器
//@note 系统控制只使用以上参数

    GrouplinkParameterType  grplink;

}SysParameterType;


//unit parameters
typedef struct
{
    UnitLocalTableType table;
    UnitLocalParamType local;

}UnitParameterType;

//cycle parameters
typedef struct
{
    uint8_t enable;             //使能状态
    uint8_t work_step;          //cycle迁移图状态
    uint8_t last_step;          //上一次状态
    uint8_t set_run_mode;       //设置的运行模式
    uint8_t rt_run_mode;        //实际的运行模式
    uint8_t opt_state;          //可运行状态
    uint8_t close_able_state;   //可关机状态
    uint16_t set_freq;          //单元设置的频率
//时间累计
    uint32_t power_on_time;         //cycle上电时间 //单位mins
    uint32_t total_heat_run_time;   //制热累计运行时间
    // uint32_t oil_total_run_time;    //cycle累计回油运转时间
    uint16_t defrost_run_time;      //除霜运转时间  //单位s

    uint16_t main_exv_step_sum;   //主膨胀阀的开度之和
    uint16_t main_exv_step_sum_last;
    uint8_t  exv3_force_close;      //EXV3强制关闭状态
//
    uint32_t alarm_state;

    CycleIOParameterType    io;
    CompParameterType       comp[COMP_NO_MAX];
    FanParameterType        fan[FAN_NO_MAX];
    EXpvParameterType       expv[EXPV_NO_MAX];

}CycleParameterType;

//use global,include sys, local_unit,cycle[CYCLE_MAX],and other link
typedef struct
{
    EepromParameterType eep;
    SysParameterType    sys;
    UnitParameterType   unit;
    CycleParameterType  cycle[CYCLE_CH_MAX];
    IOParamType         io; //local hardware data
    
}AtwParamType;


extern AtwParamType atw;
















int16_t SysTwsysoutUpdate(void);//Tw_sys_out
int16_t SysTwsysinUpdate(void); //Tw_sys_in
int16_t SysTwUpdate(void);      //Tw
void SysTwsetUpdate(void);      //Tw_set



void DataInit(void);
void DataProcess(uint8_t taskNo,uint8_t flag_init);



#endif//__DATA_H
