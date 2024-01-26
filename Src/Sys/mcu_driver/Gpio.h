#ifndef __GPIO_H
#define __GPIO_H
#include "iodefine.h"
#include "Common.h"

/***********************************************************
硬件连接说明
1.hlink reset pin
    (1)PJ3  HLINK_1  reset 
    (2)PE0  hlink_2  reset
2.RS485 control pin
    (1)P22
    (2)P55
    (3)P54
    (4)P53
    (5)PC4 
3.pump control pin




************************************************************/

#define CH_CC_RST_PIN       PORTJ.PODR.BIT.B3
#define CH_CHILLER_RST_PIN  PORTE.PODR.BIT.B0






void LedToggle(void);

#endif//__GPIO_H
