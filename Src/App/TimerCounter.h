#ifndef __TIMERCOUNTER_H
#define __TIMERCOUNTER_H
#include "Common.h"



//use sys time 
typedef union
{
	uint64_t data;
	struct 
	{
		uint16_t ms:10;     //b0~b9: 2^10 = 1024
		uint16_t s:6;       //b10~b15: 2^6 = 64
		uint16_t mins:6;    //b16~b21: 2^6 = 64
		uint16_t hour:5;    //b22~b26: 2^5 = 32
        uint16_t day:5;     //b27~b31: 2^5 = 32
        uint16_t month:4;   //b32~b35: 2^4 = 16
        uint16_t year:3;    //b36~b38: 2^3 = 8
        uint32_t total_mins:25;

	}cell;

}DataTime;

//timer type enum
typedef enum
{
//1ms timer 
    NO_BASE_10MS,

    NO_MAX_1MS,
//10ms timer
    NO_BASE_100MS = 0X0100,
    NO_TIME_BASE,               //time base

    NO_MAX_10MS,
//100ms timer 
    NO_BASE_1S = 0X0200,
    //chiller link use
    NO_CHILLER_INIT_REQ,
    //inverter link 
    NO_INV_LINK_TIME_BASE,

    NO_SERVERlLINK_TIMEOUT,//TODO for test

    NO_MAX_100MS,
//1s timer
    NO_BASE_1MINS = 0X0300,

    NO_CHILLER_LINK_TIME_BASE,  //use for chiller link regular or other time base
    NO_RESET_WAIT,              //reset(); use
    NO_SYS_CTRL_BASE,           //sys control use
    NO_UNIT_CTRL_BASE,
    NO_UNIT_FOR_TIMING,

    NO_CYCLE1_CTRL_BASE,
    NO_CYCLE2_CTRL_BASE,
    NO_CYCLE3_CTRL_BASE,
    NO_CYCLE4_CTRL_BASE,
    NO_CYCLE1_FAN1_CTRL_BASE,
    NO_CYCLE1_FAN2_CTRL_BASE,
    NO_CYCLE2_FAN1_CTRL_BASE,
    NO_CYCLE2_FAN2_CTRL_BASE,
    NO_CYCLE3_FAN1_CTRL_BASE,
    NO_CYCLE3_FAN2_CTRL_BASE,
    NO_CYCLE4_FAN1_CTRL_BASE,
    NO_CYCLE4_FAN2_CTRL_BASE,
    NO_CYCLE1_EXPVMAIN_BASE,
    NO_CYCLE2_EXPVMAIN_BASE,
    NO_CYCLE3_EXPVMAIN_BASE,
    NO_CYCLE4_EXPVMAIN_BASE,
    NO_CYCLE1_EXPVSUB_BASE,
    NO_CYCLE2_EXPVSUB_BASE,
    NO_CYCLE3_EXPVSUB_BASE,
    NO_CYCLE4_EXPVSUB_BASE,
    NO_CYCLE1_COMP_BASE,
    NO_CYCLE2_COMP_BASE,
    NO_CYCLE3_COMP_BASE,
    NO_CYCLE4_COMP_BASE,
    NO_CYCLE1_ALARM_BASE,
    NO_CYCLE2_ALARM_BASE,
    NO_CYCLE3_ALARM_BASE,
    NO_CYCLE4_ALARM_BASE,
    NO_UNIT_ALARM_BASE,
    NO_SYS_ALARM_BASE,
    NO_AD_REFRESH,
    NO_CYCLE1_OIL_VALVE,         //
    NO_CYCLE2_OIL_VALVE,
    NO_CYCLE3_OIL_VALVE,
    NO_CYCLE4_OIL_VALVE,
    NO_CYCLE1_FIN_TEMP_PROTECT,   //压机翅片温度过高保护计时
    NO_CYCLE2_FIN_TEMP_PROTECT,
    NO_CYCLE3_FIN_TEMP_PROTECT,
    NO_CYCLE4_FIN_TEMP_PROTECT,
    NO_SYSCAPACITY_BASE,

    NO_MAX_1S,
//1mins timer
    NO_BASE_1HOUR = 0X0400,
    NO_CYCLE1_INC_BASE,     //cycle累加用计时器 //1mins
    NO_CYCLE2_INC_BASE,
    NO_CYCLE3_INC_BASE,
    NO_CYCLE4_INC_BASE,
    NO_CYCLE1_ALARM_1HOUR_BASE,
    NO_CYCLE2_ALARM_1HOUR_BASE,
    NO_CYCLE3_ALARM_1HOUR_BASE,
    NO_CYCLE4_ALARM_1HOUR_BASE,
    NO_UNIT_ALARM_LOCK_BASE,
    NO_SYS_ADD_BASE,        //sys时间累计用1mins


    NO_MAX_1MINS,
//1h timer
    NO_BASE_1DAY = 0x0500,

    NO_MAX_1HOUR,

}TimerSetNOEnum;


//timer value enum
typedef enum
{
    TM_SNS_UPDATE = 10,     //10s sensor update time



}TimerSetValueEnum;



#define COUNT_1MS   NO_MAX_1MS
#define COUNT_10MS  (NO_MAX_10MS -NO_BASE_100MS)
#define COUNT_100MS (NO_MAX_100MS -NO_BASE_1S)
#define COUNT_1S    (NO_MAX_1S -NO_BASE_1MINS)
#define COUNT_1MINS (NO_MAX_1MINS -NO_BASE_1HOUR)
#define COUNT_1HOUR (NO_MAX_1HOUR -NO_BASE_1DAY)




uint8_t TimerCheck(uint16_t timerNo);
void TimerSet(uint16_t timerNo,uint16_t val);
void TimerCounterHandle(void);
DataTime GetDataTime(void);



#endif//__TIMERCOUNTER_H
