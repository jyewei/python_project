#ifndef __TIMER_H
#define __TIMER_H
#include "Common.h"




//PWM out channel
typedef enum
{
    MTU1_CHB,
    MTU4_CHA,
    PWM_CH_MAX

}PWMChannelEnum;




uint8_t TimerRandomNoGet(void);
void TimerHLClockStart(uint8_t ch);
void TimerHLClockStop(uint8_t ch);
void Timer1msStart(void);


#endif//__TIMER_H
