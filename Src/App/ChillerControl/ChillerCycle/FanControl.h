#ifndef __FANPCONTROL_H
#define __FANPCONTROL_H
#include "common.h"





typedef enum
{
    FAN_SPEED_0,
    FAN_SPEED_1,
    FAN_SPEED_2,
    FAN_SPEED_3,
    FAN_SPEED_4,
    FAN_SPEED_5,
    FAN_SPEED_6,
    FAN_SPEED_7,
    FAN_SPEED_8,
    FAN_SPEED_9,
    FAN_SPEED_10,
    FAN_SPEED_11,
    FAN_SPEED_12,
    FAN_SPEED_13,
    FAN_SPEED_14,
    FAN_SPEED_15,
    FAN_SPEED_16,
    FAN_SPEED_17,
    FAN_SPEED_18,
    FAN_SPEED_19,
    FAN_SPEED_20,
    FAN_SPEED_21,
    FAN_SPEED_22,
    FAN_SPEED_23,
    FAN_SPEED_24,
    FAN_SPEED_25,
    FAN_SPEED_26,
    FAN_SPEED_27,

    FAN_SPEED_MAX

}FanSpeedEnum;


typedef enum
{
    STATE_B,
    STATE_A

}JudgeStateEnum;









void FanRunStatusControl(uint8_t cycle);







#endif//__FANPCONTROL_H
