/***********************************************************************
@file   : Timer.c
@brief  : 
@Device : R5F5651EHGFP
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "Timer.h"
#include "iodefine.h"
#include "r_cg_mtu3.h"
#include "r_cg_tmr.h"
#include "r_cg_cmt.h"

/************************************************************************
@name  	: TimerRandomNoGet
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
uint8_t TimerRandomNoGet(void)
{
    uint8_t res;

    res = (uint8_t)(CMT3.CMCNT & 0xFF);

    return res;
}

/***********************************************************************
@name  	: TimerDelay
@brief 	: 
@param 	: None
@return	: None
************************************************************************/
static void TimerDelay(void) //wait 2us
{
	uint16_t i;

    for ( i = 0; i < 20; i++)
    {
        __nop();
    }
}

/************************************************************************
@name  	: TimerHLClockStart
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void TimerHLClockStart(uint8_t ch)
{
    // TimerDelay();
    switch (ch)
    {
        // case MTU1_CHB:  R_MTU3_C1_Start(); break;
        // case MTU4_CHA:  R_MTU3_C4_Start(); break;
        default:    break;
    }
}

/************************************************************************
@name  	: TimerHLClockStop
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void TimerHLClockIdleSet(uint8_t ch)
{
    switch (ch)
    {
        case MTU1_CHB:  MTU1.TIOR.BYTE = 0x70; break;
        case MTU4_CHA:  MTU4.TIORH.BYTE = 0x70; break;
        default:    break;
    }
}

/************************************************************************
@name  	: TimerHLClockStop
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void TimerHLClockStop(uint8_t ch)
{
    switch (ch)
    {
        // case MTU1_CHB:  R_MTU3_C1_Stop(); break;
        // case MTU4_CHA:  R_MTU3_C4_Stop(); break;
        default:    break;
    }
    // TimerHLClockIdleSet(ch);
}

/************************************************************************
@name  	: Timer1msStart
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void Timer20msStart(void)
{
    R_CMT2_Start();
}

/************************************************************************
@name  	: Timer1msStart
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void Timer1msStart(void)
{
    R_CMT3_Start();
}



