#ifndef __ADC_H
#define __ADC_H
#include "Common.h"



//
typedef enum
{
    AD0_CH0,
    AD0_CH1,
    AD0_CH2,
    AD0_CH3,
    AD0_CH4,
    AD0_CH5,
    AD0_CH6,
    AD0_CH7,

    AD1_CH0,
    AD1_CH1,
    AD1_CH2,
    AD1_CH3,
    AD1_CH4,
    AD1_CH6,
    AD1_CH7,
    AD1_CH8,
    AD1_CH9,
    AD1_CH10,
    AD1_CH11,
    AD1_CH12,
    AD1_CH13,

    AD_CH_MAX,

}AdcChannelEnum;



void AdcStart(void);
void AdcSTop(void);
void GetAdcValue(uint8_t ch,uint16_t * const buffer);

#endif//__ADC_H
