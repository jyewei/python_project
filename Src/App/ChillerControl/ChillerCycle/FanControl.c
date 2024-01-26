/***********************************************************************
@file   : FanControl.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
@note   : 7.2 风机控制逻辑
            2.有单元独立控制
           7.2.1 风扇电机控制
            1.转速变速速度为1rps ->60rpm??
************************************************************************/
#include "FanControl.h"
#include "data.h"
#include "TimerCounter.h"
#include "CycleStateControl.h"
#include "ChillerUnit.h"

#define SNOW_DEFENCE_TEMP_CHECK   (30)  //防雪温度


#define TM_FAN_STATE_KEEP   ((uint16_t)60)  //风机保持时间
#define TM_SYNC_WAIT        ((uint16_t)6)   //TODO to get the parameter 
#define TM_SNOW_RUN         ((uint16_t)30)  //
#define TM_SNOW_CHECK       ((uint16_t)600) //

//ΔPd
typedef enum
{
    DELTA_PD1,
    DELTA_PD2,
    DELTA_PD3,
    DELTA_PD4,
    DELTA_PD5,
    DELTA_PD_MAX

}DeltaPdEnum;

//
typedef enum
{
    FAN1_65KW_CYCLE1,
    FAN2_65KW_CYCLE1,
    FAN1_130KW_CYCLE1,
    FAN2_130KW_CYCLE1,
    FAN1_130KW_CYCLE2,
    FAN2_130KW_CYCLE2,
    FAN_TYPE_MAX,

}FanTypeEnum;


//time //decrease //increase
typedef struct
{
    uint16_t fan_state_keep;        //状态保持
    uint16_t sync_freq_wait;        //同步频率到达时间
    uint16_t heatsink_temp_keep;    //温度过高保持时间
    uint16_t snow_denfence_run;     //防雪运行时间
    uint16_t snow_denfence_period;  //防雪检测周期
    
    uint16_t heatsink_check;        //变频翅片检测周期
    uint16_t normal_cool_check;
    uint16_t normal_heat_check;
    uint16_t defrost_wait;              //除霜过程中的控制等待
    uint16_t start2_wait;

}FanControlTimeType;

//ΔPd 
const uint16_t delta_pd_tab[DELTA_PD_MAX] = {70,70,300,300,300};

//fan_speed & real rpm value table
// const
const uint16_t fan_speed_rpm_tab[FAN_TYPE_MAX][FAN_SPEED_MAX] = 
{
  //0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27
   {0,108,120,144,156,192,216,228,282,306,336,372,408,444,498,570,600,642,690,714,786,834,882,930,972,1008,1044,1080},  //65KW cycle1 fan1
   {0,108,120,144,156,192,216,228,282,306,336,372,408,444,498,570,600,642,690,714,786,834,882,930,972,1008,1044,1080},  //65KW cycle1 fan2
   {0,108,120,144,156,192,216,228,282,306,336,372,408,444,498,570,600,642,690,714,786,834,882,930,972,1008,1044,1080},  //130KW cycle1 fan1
   {0,108,120,144,156,192,216,228,282,306,336,372,408,444,498,570,600,642,690,714,786,834,882,930,972,1008,1044,1080},  //130KW cycle1 fan2
   {0,108,120,144,156,192,216,228,282,306,336,372,408,444,498,570,600,642,690,714,786,834,882,930,972,1008,1044,1080},  //130KW cycle2 fan1
   {0,108,120,144,156,192,216,228,282,306,336,372,408,444,498,570,600,642,690,714,786,834,882,930,972,1008,1044,1080},  //130KW cycle2 fan2

};



static FanControlTimeType time[CYCLE_CH_MAX][FAN_NO_MAX];



/************************************************************************
@name  	: FanControlTimerCounter
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void FanControlTimerCounter(uint8_t cycle,uint8_t fan)
{
    uint16_t *pTime;
    uint16_t i,len;

    if (TimerCheck(NO_CYCLE1_FAN1_CTRL_BASE+cycle*2+fan) == TRUE)
    {
        TimerSet(NO_CYCLE1_FAN1_CTRL_BASE+cycle*2+fan,1); //base 1s
        len = sizeof(FanControlTimeType)/sizeof(uint16_t);
        pTime = &time[cycle][fan].fan_state_keep;
        for ( i = 0; i < len; i++)
        {
            if (*(pTime + i) != 0)
            {
                (*(pTime + i))--;
            }
        }
    }
}

/************************************************************************
@name  	: GetFanJudgeState
@brief 	: 
@param 	: None
@return	: None
@note   : 7.2.2 
            状态A/B迁移条件
            (1)风机档位下限值
*************************************************************************/
static uint8_t GetFanJudgeState(uint8_t cycle,uint8_t fan)
{
    static uint8_t judge_state = STATE_B;
    int16_t temp = atw.cycle[cycle].fan[fan].link.state.heat_sink_temp;

    if (temp < 720)
    {
        time[cycle][fan].heatsink_temp_keep = 300;//5mins/300s
    }

    if (judge_state == STATE_B)
    {
        if (temp >= 750)//>=75°
        {
            judge_state = STATE_A;
        }
        else 
        {
            if (temp >= 720)//>=72°,且持续5mins
            {
                if (time[cycle][fan].heatsink_temp_keep == 0)
                {
                    judge_state = STATE_A;
                }
            }
        }
    }
    else
    {
        if (temp <= 670)
        {
            judge_state = STATE_B;
        }
    }
    return judge_state;
}

/************************************************************************
@name  	: GetMinFanSpeed
@brief 	: 
@param 	: None
@return	: None
@note   : 7.2.2 (1)风机档位下限值
*************************************************************************/
static void GetMinFanSpeed(uint8_t cycle,uint8_t fan)
{
    static int16_t last_temp = 0;
    uint8_t fan_speed = 0;
    int16_t Tam_temp = atw.unit.table.io.Tam;
    uint8_t state = GetFanJudgeState(cycle,fan);
    uint8_t temp_type = atw.unit.table.init.dsw1.bit.temp_type;
    uint8_t refrigent_type = atw.unit.table.init.dsw1.bit.refrigent_type;
    uint8_t fourway_state = CycleGetFourWayValveState(cycle);

    if (state ==  STATE_A)//stateA
    {
        if (fourway_state == OFF)
        {
            if (Tam_temp >= last_temp)//温度上升
            {
                if (Tam_temp < 150)
                {
                    fan_speed = FAN_SPEED_1;
                }
                else if (Tam_temp < 250)
                {
                    fan_speed = FAN_SPEED_3;
                }
                else
                {
                    fan_speed = FAN_SPEED_6;
                }
            }
            else//温度下降
            {
                if (Tam_temp > 150)
                {
                    fan_speed = FAN_SPEED_6;
                }
                else if (Tam_temp > 50)
                {
                    fan_speed = FAN_SPEED_3;
                }
                else
                {
                    fan_speed = FAN_SPEED_1;
                }
            }
        }
        else//four_way on
        {
            fan_speed = FAN_SPEED_6;
        }
    }
    else//stateB
    {
        if (fourway_state == OFF)
        {   
            if ((temp_type == TEMP_SA) //标准热泵(加防冻液)为0,其它为1;
                && (refrigent_type == ADD_ANTIFREEZE))
            {
                fan_speed = FAN_SPEED_0;
            }
            else fan_speed = FAN_SPEED_1;
        }
        else
        {
            fan_speed = FAN_SPEED_1;
        }
    }
    atw.cycle[cycle].fan[fan].local.Fomin = fan_speed;
    last_temp = Tam_temp;
}

/************************************************************************
@name  	: GetMaxFanSpeed
@brief 	: 
@param 	: None
@return	: None
@note   : 7.2.2 (2)获取风机档位上限值
*************************************************************************/
static void GetMaxFanSpeed(uint8_t cycle,uint8_t fan)//
{
    uint8_t fan_speed;
    uint8_t temp_type = atw.unit.table.init.dsw1.bit.temp_type;
    uint8_t work_state = atw.unit.table.init.dsw1.bit.unit_work_state;  
    uint8_t cycle_step = atw.cycle[cycle].work_step;
    int16_t tam_temp = atw.unit.table.io.Tam;
    uint8_t frame_type = atw.unit.local.frame_type;
    uint8_t capacity = atw.unit.table.init.dsw1.bit.capacity;
    uint8_t esp = atw.unit.table.init.dsw2.bit.esp_type;
    uint8_t silence = atw.unit.table.sys_cmd.data1.bit.silence_set;
    
//priority1 //测试模式,且非除霜后启动中,且非除霜过程中
    if ((work_state == UNIT_STATE_TEST) && (cycle_step != CYCLE_STATUS_START_DEFROST_DONE) && (cycle_step != CYCLE_STATUS_DEFROST_RUN))
    {
        fan_speed = FAN_SPEED_27;
    }
//priority2 //低噪音设置,且Tam<=44°    
    else if ((tam_temp <= 440) && (silence != SILENCE_NULL))
    {
        switch (silence)
        {
            case SILENCE_1://静音1
            {
                switch (frame_type)
                {
                    case FRAME_A:  fan_speed = FAN_SPEED_20;  break;
                    case FRAME_B:  fan_speed = FAN_SPEED_20;  break;
                    case FRAME_C:  fan_speed = FAN_SPEED_20;  break;
                    default:    break;
                }
            }
                break;
            case SILENCE_2://静音2
            {
                switch (frame_type)
                {
                    case FRAME_A:  fan_speed = FAN_SPEED_18;  break;
                    case FRAME_B:  fan_speed = FAN_SPEED_18;  break;
                    case FRAME_C:  fan_speed = FAN_SPEED_17;  break;
                    default:    break;
                }
            }
                break;
            case SILENCE_3://静音3
            {
                switch (frame_type)
                {
                    case FRAME_A:  fan_speed = FAN_SPEED_16;  break;
                    case FRAME_B:  fan_speed = FAN_SPEED_16;  break;
                    case FRAME_C:  fan_speed = FAN_SPEED_15;  break;
                    default:    break;
                }
            }
                break;
        default:
            break;
        }
    }
//priority3 高静压设置
    else if (esp != ESP_NULL) //
    {
        switch (esp)
        {
            case ESP_30Pa:  fan_speed = FAN_SPEED_25;  break;
            case ESP_50Pa:  fan_speed = FAN_SPEED_26;  break;
            case ESP_80Pa:  fan_speed = FAN_SPEED_27;  break;
            default:    break;
        }
    }
//priority4 //夜间运转模式设置,且制冷模式
    else if (0)//TODO 组控器协议定
    {
        
    }
//priority5 //夜间运转模式设置
    else if (0)//TODO 组控器协议定
    {
        
    }
//priority6 上述以外
    else
    {
        if (temp_type == TEMP_SA)//标准热泵
        {
            switch (capacity)
            {
                case CAPACITY_45KW:   fan_speed = FAN_SPEED_22;  break;
                case CAPACITY_65KW:   fan_speed = FAN_SPEED_23;  break;
                case CAPACITY_130KW:  fan_speed = FAN_SPEED_25;  break;
                default:    break;
            }
        }
        else//低温热泵
        {
            switch (capacity)
            {
                case CAPACITY_45KW:   fan_speed = FAN_SPEED_23;  break;
                case CAPACITY_65KW:   fan_speed = FAN_SPEED_23;  break;
                default:    break;
            }
        }
    }
     

    //TODO
    fan_speed = FAN_SPEED_27;

    atw.cycle[cycle].fan[fan].local.Fomax = fan_speed;
}

/************************************************************************
@name  	: FanControl
@brief 	: 
@param 	: None
@return	: None
@note   : 风机的控制,控制风机档位
*************************************************************************/
void FanSpeedControl(uint8_t cycle,uint8_t fan,uint8_t fan_speed)
{ 
    uint16_t rt_freq = atw.cycle[cycle].fan[fan].link.state.foc_freq_output;
    uint16_t set_freq = 0;
    uint8_t fan_speed_max = atw.cycle[cycle].fan[fan].local.Fomax;
    uint8_t fan_speed_min = atw.cycle[cycle].fan[fan].local.Fomin;
    uint16_t sync_freq = atw.cycle[cycle].fan[fan].local.sync_freq;
    uint8_t cycle_step = atw.cycle[cycle].work_step;

//上下限的判断
    if (cycle_step != CYCLE_STATUS_OFF)//TODO 待确定
    {
        if (fan_speed > fan_speed_max)//上限
        {
            fan_speed = fan_speed_max;
        }
        else if (fan_speed < fan_speed_min)//下限
        {
            fan_speed = fan_speed_min;
        }
    }    
    set_freq = fan_speed_rpm_tab[FAN1_65KW_CYCLE1][fan_speed];
//同步频率的判断
    if ((rt_freq == 0) && (set_freq < sync_freq))//风机实际频率为0,且设置的频率小于同步频率
    {
        set_freq = sync_freq;
        time[cycle][fan].sync_freq_wait = TM_SYNC_WAIT;
    }
    else
    {
        if (time[cycle][fan].sync_freq_wait != 0)//等待中
        {
            set_freq = sync_freq;
        }
    }
    atw.cycle[cycle].fan[fan].link.ctrl.freq_set = set_freq;
}

/************************************************************************
@name  	: FanCheckControl
@brief 	: 
@param 	: None
@return	: None
@note   : 风机检测运转
*************************************************************************/
static uint8_t FanCheckControl(uint8_t cycle,uint8_t fan)
{
    uint8_t res = FALSE;
    uint8_t unit_step = atw.unit.local.work_step;

    if (unit_step == UNIT_STATUS_PREPARE)
    {
        res = TRUE;
    }

    return res;
}

/************************************************************************
@name  	: GetFanSnowDefenceState
@brief 	: 
@param 	: None
@return	: None
@note   : 检测是否需要进行防雪
*************************************************************************/
static uint8_t GetFanSnowDefenceState(void) 
{
    int16_t tam_temp = atw.unit.table.io.Tam;
    uint8_t snow_require = atw.unit.table.sys_cmd.data1.bit.snow_defence;
    static uint8_t snow_state = FALSE;

    if ((snow_require == TRUE) && (atw.unit.local.sns_err.bit.Tam == FALSE))//防雪设置,且无传感器异常
    {
        if (tam_temp <= SNOW_DEFENCE_TEMP_CHECK)//
        {
            snow_state = TRUE;
        }
        if (snow_state == TRUE)
        {
            if (tam_temp > (SNOW_DEFENCE_TEMP_CHECK + 10))
            {
                snow_state = FALSE;
            }
        }
    }
    else snow_state = FALSE;

    return snow_state;
}

/************************************************************************
@name  	: FanSnowDefenceControl
@brief 	: 
@param 	: None
@return	: None
@note   : 防雪的处理
*************************************************************************/
static uint8_t FanSnowDefenceControl(uint8_t cycle,uint8_t fan)
{
    uint8_t res = FALSE;
    uint8_t fan_speed = 0;
    uint8_t capacity = atw.unit.table.init.dsw1.bit.capacity;
    uint8_t frame_type = atw.unit.local.frame_type;

    if (GetFanSnowDefenceState() == TRUE)
    {
        res = TRUE;
        switch (frame_type)
        {
            case FRAME_A:  fan_speed = FAN_SPEED_18;  break;
            case FRAME_B:  fan_speed = FAN_SPEED_17;  break;
            case FRAME_C:  fan_speed = FAN_SPEED_18;  break;
            default:    break;
        }

        if (time[cycle][fan].snow_denfence_period == 0)//snow_denfence_run
        {
            time[cycle][fan].snow_denfence_period = 600;//防雪运转周期
            time[cycle][fan].snow_denfence_run = 30;//运转时间
        }
    }
    else fan_speed = FAN_SPEED_0;

    if (time[cycle][fan].snow_denfence_run == 0)
    {
        fan_speed = FAN_SPEED_0;
    }
    FanSpeedControl(cycle,fan,fan_speed);

    return res;
}

/************************************************************************
@name  	: FanControlInCycleStart1
@brief 	: 
@param 	: None
@return	: None
@note   : 7.2.3风扇运转控制
            (3)cycle启动1
*************************************************************************/
static void FanControlInCycleStart1(uint8_t cycle,uint8_t fan)
{
    int16_t tam_temp = atw.unit.table.io.Tam;
    uint8_t fan_speed_max = atw.cycle[cycle].fan[fan].local.Fomax;

    if (CycleGetFourWayValveState(cycle) == OFF)
    {
        if (tam_temp > 150)
        {
            FanSpeedControl(cycle,fan,FAN_SPEED_16);
        }
        else FanSpeedControl(cycle,fan,FAN_SPEED_1);
    }
    else if ((CycleGetFourWayValveState(cycle) == ON) && (tam_temp >= 60))
    {
        if (tam_temp >= 150)
        {
            FanSpeedControl(cycle,fan,FAN_SPEED_1);
        }
        else FanSpeedControl(cycle,fan,FAN_SPEED_16);
    }
    else
    {
        FanSpeedControl(cycle,fan,fan_speed_max);
    }
}

/************************************************************************
@name  	: FanControlInCycleStart2Begin进入cycle启动2时的风扇档位
@brief 	: 
@param 	: None
@return	: None
@note   : 7.2.3风扇运转控制
            (5)cycle启动2
                (5-1)制冷模式 
                (5-2)制热模式
*************************************************************************/
static void FanControlInCycleStart2Begin(uint8_t cycle,uint8_t fan)
{   
    uint16_t Fi_run = atw.cycle[cycle].comp[COMP_1].link.state.freq_output;//实际输出频率;
    int16_t tam_temp = atw.unit.table.io.Tam;
    uint8_t speed_max = atw.cycle[cycle].fan[fan].local.Fomax;
    uint8_t fan_speed;

    if (GetCycleSetRunMode(cycle) == CYCLE_MODE_COOL)//制冷
    {
        if (Fi_run>= 900)//priority1
        {
            if (tam_temp >= 330)
                fan_speed = speed_max;
            else if (tam_temp >= 250)
                fan_speed = FAN_SPEED_23;
            else if (tam_temp >= 180)
                fan_speed = FAN_SPEED_18;
            else if (tam_temp >= 110)
                fan_speed = FAN_SPEED_14;
            else if (tam_temp >= 50)
                fan_speed = FAN_SPEED_12;
            else if (tam_temp >= -50)
                fan_speed = FAN_SPEED_9;
            else fan_speed = FAN_SPEED_1;
        }
        else if (Fi_run>= 600)//priority2
        {
            if (tam_temp >= 330)
                fan_speed = speed_max;
            else if (tam_temp >= 250)
                fan_speed = FAN_SPEED_22;
            else if (tam_temp >= 180)
                fan_speed = FAN_SPEED_18;
            else if (tam_temp >= 110)
                fan_speed = FAN_SPEED_12;
            else if (tam_temp >= 50)
                fan_speed = FAN_SPEED_8;
            else if (tam_temp >= -50)
                fan_speed = FAN_SPEED_5;
            else fan_speed = FAN_SPEED_1;
        }
        else
        {
            if (tam_temp >= 330)
                fan_speed = FAN_SPEED_22;
            else if (tam_temp >= 250)
                fan_speed = FAN_SPEED_17;
            else if (tam_temp >= 180)
                fan_speed = FAN_SPEED_13;
            else if (tam_temp >= 110)
                fan_speed = FAN_SPEED_5;
            else fan_speed = FAN_SPEED_1;
        }
    }
    else if (GetCycleSetRunMode(cycle) == CYCLE_MODE_HEAT)//制热
    {
        if (tam_temp>= 300)
            fan_speed = FAN_SPEED_5;
        else if (tam_temp >= 200)
            fan_speed = FAN_SPEED_10;
        else if (tam_temp >= 150)
            fan_speed = FAN_SPEED_16;
        else if (tam_temp >= 100)
            fan_speed = FAN_SPEED_22;
        else fan_speed = speed_max;
    }
    FanSpeedControl(cycle,fan,fan_speed);
}

/************************************************************************
@name  	: FanControlInCycleStart2  cycle启动2过程中的风扇档位
@brief 	: 
@param 	: None
@return	: None
@note   : 7.2.3风扇运转控制
            (5)cycle启动2
                (5-1)制冷模式
                (5-2)制热模式   //Pdstart = 1900
*************************************************************************/
static void FanControlInCycleStart2(uint8_t cycle,uint8_t fan)
{
    int16_t Pd = atw.cycle[cycle].comp[COMP_1].local.Pd;
    int16_t Pd_last = atw.cycle[cycle].comp[COMP_1].local.Pd_last;
    int16_t Td = atw.cycle[cycle].comp[COMP_1].local.Td;
    int16_t Tc = atw.cycle[cycle].comp[COMP_1].local.Tc;
    uint8_t speed_max = atw.cycle[cycle].fan[fan].local.Fomax;
    uint8_t speed_min = atw.cycle[cycle].fan[fan].local.Fomin;
    uint8_t fan_speed = atw.cycle[cycle].fan[fan].local.rt_fan_speed;

    if (GetCycleSetRunMode(cycle) == CYCLE_MODE_COOL)//制冷
    {
        if (time[cycle][fan].start2_wait == 0)
        {
            time[cycle][fan].start2_wait = 30; //30s
            if ((Pd >= 1900)
                || (((Pd - Pd_last)>=200) && (Pd > 1700)))
            {
                fan_speed += 1;
                if (fan_speed > speed_max)
                {
                    fan_speed = speed_max;
                }
            }
            else if (((Pd < 1500) && (Td > Tc+100))
                    || ((Pd_last-Pd)>=100 ) && (Pd<=1750))
            {
                if (fan_speed > 0)
                {
                    fan_speed -= 1;
                    if (fan_speed < speed_min)
                    {
                        fan_speed = speed_min;
                    }
                }
            }
        }
    }
    else if (GetCycleSetRunMode(cycle) == CYCLE_MODE_HEAT)//制热
    {
        //维持进入时的档位
    }
}

/************************************************************************
@name  	: FanControlNormalCoolHeatSinkCheck
@brief 	: 
@param 	: None
@return	: None
@note   : 7.2.3风扇运转控制
            (6)cycle通常运转
                (6-1)制冷模式
                    (c)变频翅片温度上升防止控制
                        (c_1)进入条件:Tf>=T4
                        (c-2)控制内容:提高3个档位,60s后未退出,继续提高3个,不大于Fomax
*************************************************************************/
static uint8_t FanControlNormalCoolHeatSinkCheck(uint8_t cycle,uint8_t fan)
{
    static uint8_t res = FALSE;
    int16_t T4 = atw.cycle[cycle].comp[COMP_1].local.P04_T4;
    int16_t T2 = atw.cycle[cycle].comp[COMP_1].local.P04_T2;   
    int16_t temp = atw.cycle[cycle].comp[COMP_1].link.state.PIM_temp;
    
    if (res == FALSE)
    {
        if (temp >= T4)
        {
            res = TRUE;
        }
    }
    else if (temp < T2)
    {
        res = FALSE;
    }
   
    return res;
}

/************************************************************************
@name  	: GetPdsetValue
@brief 	: 
@param 	: None
@return	: None
@note   : Pdset
*************************************************************************/
static uint16_t GetPdsetValue(uint8_t cycle)
{
    int16_t Pdset,tam_temp;
    uint16_t Fi_run,Fi_rating;

    Fi_run = atw.cycle[cycle].comp[COMP_1].link.state.freq_output;//实际输出频率
    Fi_rating = atw.cycle[cycle].comp[COMP_1].local.Fi_c_rating;    //压缩机制冷额定频率
    tam_temp = atw.unit.table.io.Tam;
    //pdset     
    Pdset = (tam_temp*7 - 490 + Fi_run*10/Fi_rating*80);//1000倍    //kPa
    if (Pdset < 2030)
    {
        Pdset = 2030;
    }
    else if (Pdset > 3110)
    {
        Pdset = 3110;
    }
    return Pdset;
}

/************************************************************************
@name  	: GetPsominValue
@brief 	: 
@param 	: None
@return	: None
@note   : 7.2.3风扇运转控制
            (6)cycle通常运转 
*************************************************************************/
static uint16_t GetPsominValue(uint8_t cycle)
{
    uint16_t Psomin;
    int16_t tam_temp = atw.unit.table.io.Tam;

    Psomin = 12*tam_temp/10 + 340;//Psomin = 2/163*Tam +0.34MPa  2000/163=12 //tam->10倍值
    if (Psomin < 160)
    {
        Psomin = 160;
    }
    else if (Psomin > 600)
    {
        Psomin = 600;
    }

    return Psomin;
}

/************************************************************************
@name  	: GetPsfoValue
@brief 	: 
@param 	: None
@return	: None
@note   : 7.2.3风扇运转控制
            (6)cycle通常运转 
*************************************************************************/
static int16_t GetPsfoValue(uint8_t cycle)
{
    int16_t Psfo;
    uint16_t Fi_run,Fi_rating,tansform;

    Fi_run = atw.cycle[cycle].comp[COMP_1].link.state.freq_output;//实际输出频率
    Fi_rating = atw.cycle[cycle].comp[COMP_1].local.Fi_c_rating;    //压缩机制冷额定频率

    tansform = Fi_run*10/Fi_rating; //10倍值
    if (tansform > 10)
    {
        tansform = 10;
    }
    Psfo = 7900 - tansform*80;
    Psfo = Psfo/10;

    return Psfo;
}

/************************************************************************
@name  	: FanControlInCycleNormal
@brief 	: 
@param 	: None
@return	: None
@note   : 7.2.3风扇运转控制
            (6)cycle通常运转
                (6-1)制冷模式
                    (a)维持cycle启动2退出时的档位
                (6-2)制热模式
*************************************************************************/
static void FanControlInCycleNormal(uint8_t cycle,uint8_t fan)
{
    int16_t Pd = atw.cycle[cycle].comp[COMP_1].local.Pd;
    int16_t Pd_last = atw.cycle[cycle].comp[COMP_1].local.Pd_last;
    int16_t Td = atw.cycle[cycle].comp[COMP_1].local.Td;
    int16_t Ps = atw.cycle[cycle].comp[COMP_1].local.Ps;
    int16_t Tcoil = atw.cycle[cycle].io.Tcoil;
    int16_t tam_temp = atw.unit.table.io.Tam;
    uint8_t fomax = atw.cycle[cycle].fan[fan].local.Fomax;
    //uint8_t speed_min = atw.cycle[cycle].fan[fan].local.Fomin;
    uint8_t rt_fan_speed = atw.cycle[cycle].fan[fan].local.rt_fan_speed;
    uint8_t fan_speed;
    int16_t Pdset;
    int16_t Psomin,Psfo;

    if (GetCycleSetRunMode(cycle) == CYCLE_MODE_COOL)//制冷
    {
        if (Pd >= 3350)//(b)-1 Pd>=3.35MPa 
        {
            fan_speed = fomax;//上限档位
            time[cycle][fan].normal_cool_check = 60;//clear timer
        }
        else if (FanControlNormalCoolHeatSinkCheck(cycle,fan) == TRUE)//(b)-2变频翅片温度上升防止控制
        {
            if (time[cycle][fan].heatsink_check == 0)
            {
                time[cycle][fan].heatsink_check = 60; //60s period to increase fan_speed
                fan_speed += rt_fan_speed + 3;
                if (fan_speed > fomax)
                {
                    fan_speed = fomax;
                }
                time[cycle][fan].normal_cool_check = 60;//clear timer
            }
        }
        else//(b)-3 ~(b)-6
        {
            Pdset = GetPdsetValue(cycle);
            if (time[cycle][fan].normal_cool_check <= 30)
            {
                if (((Pd <= 2030) && (Pd_last >= Pd))//(b)-3  //30s period
                    || (Pd_last >= (Pd + 400)) )
                {
                    time[cycle][fan].normal_cool_check = 60;//clear timer
                    if (rt_fan_speed > 1)
                    {
                        fan_speed = rt_fan_speed -2;
                    }
                    else fan_speed = FAN_SPEED_0;
                }
                else if ((Pd >= (Pdset +  delta_pd_tab[DELTA_PD3]))//(b)-4 //30s period
                        || ((Pd >= (Pd_last + 200)) && (Pd >= (Pdset - 100)))
                        || ((Pd >= 2500) && (Td >= 950)))
                {
                    time[cycle][fan].normal_cool_check = 60;
                    fan_speed = rt_fan_speed +2;
                    if (fan_speed > fomax)
                    {
                        fan_speed = fomax;
                    }
                }
                else if (time[cycle][fan].normal_cool_check == 0)//60s period
                {
                    if ((Pd <= (Pdset -  delta_pd_tab[DELTA_PD2])))//(b)-5
                    {
                        time[cycle][fan].normal_cool_check = 60;
                        if (rt_fan_speed > 0)
                        {
                            fan_speed = rt_fan_speed -1;
                        }
                        else fan_speed = FAN_SPEED_0;
                    }
                    else if ((Pd >= (Pdset +  delta_pd_tab[DELTA_PD1])))//(b)-6
                    {
                        fan_speed = rt_fan_speed + 1;
                        if (fan_speed > fomax)
                        {
                            fan_speed = fomax;
                        }
                    }
                }
            }
        }
    }
    else if (GetCycleSetRunMode(cycle) == CYCLE_MODE_HEAT)//制热
    {
        if (tam_temp < 50)//
        {
            fan_speed = fomax;
            time[cycle][fan].normal_heat_check = 120;//clear timer
        }
        else
        {
            Psfo = GetPsfoValue(cycle);
            Psomin = GetPsominValue(cycle);
            if (time[cycle][fan].normal_heat_check <= 90)//30s period //120-30=90
            {
                if ((Pd > 1290 ) || ((Tcoil>=100 ) && (Pd >= 3500)))//
                {
                    time[cycle][fan].normal_heat_check = 120;//clear timer
                    if (rt_fan_speed > 3)
                    {
                        fan_speed = rt_fan_speed -4;
                    }
                    else fan_speed = FAN_SPEED_0;
                }
                else if (Ps <= (Psomin + 150))
                {
                    time[cycle][fan].normal_heat_check = 120;//clear timer
                    fan_speed = rt_fan_speed + 2;
                    if (fan_speed > fomax)
                    {
                        fan_speed = fomax;
                    }
                }
                else if (time[cycle][fan].normal_heat_check == 0)//120s period
                {
                    if ((Ps > 1140) || ((Ps > Psfo + 30) && (Tcoil > 10)))
                    {
                        time[cycle][fan].normal_heat_check = 120;//clear timer
                        if (rt_fan_speed > 0)
                        {
                            fan_speed = rt_fan_speed -1;
                        }
                        else fan_speed = FAN_SPEED_0;
                    }
                    else if ((Ps < Psfo - 20) || (Tcoil <= -20) ) 
                    {
                        time[cycle][fan].normal_heat_check = 120;//clear timer
                        fan_speed = rt_fan_speed + 1;
                        if (fan_speed > fomax)
                        {
                            fan_speed = fomax;
                        }
                    }
                }
            }
        }
    }
    FanSpeedControl(cycle,fan,fan_speed);
}


/************************************************************************
@name  	: FanRunStatusControl
@brief 	: 
@param 	: None
@return	: None
@note   : 风机的控制,根据cycle的状态和unit的状态来决定风档
*************************************************************************/
void FanRunStatusControl(uint8_t cycle)
{
    uint8_t fan_ch;
    uint8_t cycle_step = atw.cycle[cycle].work_step;
    uint8_t last_step = atw.cycle[cycle].last_step;
    int16_t Pd = atw.cycle[cycle].comp[COMP_1].local.Pd;
    int16_t Ps = atw.cycle[cycle].comp[COMP_1].local.Ps;

    for (fan_ch = FAN_1; fan_ch < FAN_NO_MAX; fan_ch++)
    {
        GetMinFanSpeed(cycle,fan_ch);   //最小档位
        GetMaxFanSpeed(cycle,fan_ch);   //最大档位
        FanControlTimerCounter(cycle,fan_ch);//fan用定时器

        switch (cycle_step)
        {
            case CYCLE_STATUS_OFF://cycle关机
            {
                if (last_step != CYCLE_STATUS_OFF)//由其它状态进入off
                {
                    time[cycle][fan_ch].fan_state_keep = TM_FAN_STATE_KEEP;
                    time[cycle][fan_ch].snow_denfence_period = 600; //进入防雪关风机时间
                }
                else //off
                {
                    if (time[cycle][fan_ch].fan_state_keep == 0)//延时60s
                    {
                        if (FanCheckControl(cycle,fan_ch) == TRUE)//运转风机检测运转
                        {
                            FanSpeedControl(cycle,fan_ch,FAN_SPEED_6);
                            
                        }
                        else if (FanSnowDefenceControl(cycle,fan_ch) == FALSE)//未运转防雪
                        {
                            FanSpeedControl(cycle,fan_ch,FAN_SPEED_0);//关风机
                        }
                    }
                }
            }
                break;
            case CYCLE_STATUS_START1://cycle启动1
            {
                FanControlInCycleStart1(cycle,fan_ch);
            }
                break;
            case CYCLE_STATUS_START_DEFROST_DONE://除霜后启动
            {
                if (last_step != CYCLE_STATUS_START_DEFROST_DONE)//进入
                {
                    if (Pd >= 2300)//
                    {
                        FanSpeedControl(cycle,fan_ch,FAN_SPEED_16);
                    }
                    else if ((Pd - Ps) >= 1000)
                    {
                        FanSpeedControl(cycle,fan_ch,FAN_SPEED_16);
                    }
                    else FanSpeedControl(cycle,fan_ch,FAN_SPEED_0);
                }
            }
                break;
            case CYCLE_STATUS_START2://cycle启动2
            {
                if (last_step != CYCLE_STATUS_START2)
                {
                    FanControlInCycleStart2Begin(cycle,fan_ch);
                    time[cycle][fan_ch].start2_wait = 30;   //维持30s
                }
                else FanControlInCycleStart2(cycle,fan_ch);                    
            }
                break;
            case CYCLE_STATUS_RUN_COOL://cycle制冷      //cycle 通常运转
            case CYCLE_STATUS_RUN_HEAT://cycle制热
            case CYCLE_STATUS_RUN_OIL_RETURN://cycle回油控制
            case CYCLE_STATUS_DIFFER_PRESSURE://差压控制
            case CYCLE_STATUS_DEFROST_PREPARE://除霜准备
            {
                FanControlInCycleNormal(cycle,fan_ch);
            }
                break;
            case CYCLE_STATUS_DEFROST_RUN://除霜运转
            {
                if (last_step != CYCLE_STATUS_DEFROST_RUN)//进入时 //
                {
                    FanSpeedControl(cycle,fan_ch,FAN_SPEED_0);
                    time[cycle][fan_ch].defrost_wait = 60;
                }
                else if (time[cycle][fan_ch].defrost_wait == 0)//1分钟后进行Pd判断
                {
                    if (Pd > 2300)
                    {
                        FanSpeedControl(cycle,fan_ch,FAN_SPEED_6);
                    }
                    else if (Pd < 1500)
                    {
                        FanSpeedControl(cycle,fan_ch,FAN_SPEED_0);
                    }
                }
            }
                break;
            default:
                break;
        }
    }
}
