/***********************************************************************
@file   : InputInterrupt.c
@brief  : interrupt handle
@detail	: 
@Device : R5F5651E
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "r_cg_macrodriver.h"
#include "Interrupt.h"
#include "HlinkDriver.h"
#include "TimerCounter.h"
#include "modbus.h"



//cmt2 1ms
#pragma interrupt  r_cmt_cmi3_interrupt(vect=VECT(PERIB,INTB129))
//uart0 hlink1   send/receive interrupt
#pragma interrupt r_icu_irq13_interrupt(vect=VECT(ICU,IRQ13))
#pragma interrupt r_sci0_transmit_interrupt(vect=VECT(SCI0,TXI0))   
#pragma interrupt r_sci0_receive_interrupt(vect=VECT(SCI0,RXI0))
//uart3 hlink2
#pragma interrupt r_icu_irq11_interrupt(vect=VECT(ICU,IRQ11))
#pragma interrupt r_sci3_transmit_interrupt(vect=VECT(SCI3,TXI3))
#pragma interrupt r_sci3_receive_interrupt(vect=VECT(SCI3,RXI3))





uint16_t _1ms_cnt;
/************************************************************************
@name  	: r_cmt_cmi3_interrupt
@brief 	: 1ms 
@param 	: None
@return	: None
*************************************************************************/
static void r_cmt_cmi3_interrupt(void)
{
    _1ms_cnt++;
    TimerCounterHandle();   //TimerCounter
    // LedToggle();
    HLInt1ms(HL_CH_CHILLER);
}

//==uart0 Hlink1
/************************************************************************
@name  	: r_icu_irq13_interrupt
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void r_icu_irq13_interrupt(void)
{
    HLCarryInt(HL_CH_CHILLER);
}

/************************************************************************
@name  	: r_sci0_receive_interrupt
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void r_sci0_receive_interrupt(void)
{
    HLIntReceive(HL_CH_CHILLER);
}

/************************************************************************
@name  	: r_sci0_receiveerror_interrupt
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void r_sci0_receiveerror_interrupt(void)
{
    HLIntReceive(HL_CH_CHILLER);
}

/************************************************************************
@name  	: r_sci0_transmit_interrupt
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void r_sci0_transmit_interrupt(void)
{
    // HLIntTransmit(HL_CH_CHILLER);
}

/************************************************************************
@name  	: r_sci0_transmitend_interrupt
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void r_sci0_transmitend_interrupt(void)
{
	HLIntTransmit(HL_CH_CHILLER);
}

//==uart3 Hlink2
/************************************************************************
@name  	: r_icu_irq11_interrupt
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void r_icu_irq11_interrupt(void)
{
    HLCarryInt(HL_CH_CC);
}

/************************************************************************
@name  	: r_sci3_receive_interrupt
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void r_sci3_receive_interrupt(void)
{
    HLIntReceive(HL_CH_CC);
}

/************************************************************************
@name  	: r_sci0_receiveerror_interrupt
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void r_sci3_receiveerror_interrupt(void)
{
    HLIntReceive(HL_CH_CC);
}

/************************************************************************
@name  	: r_sci3_transmit_interrupt
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void r_sci3_transmit_interrupt(void)
{

}

/************************************************************************
@name  	: r_sci3_transmitend_interrupt
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void r_sci3_transmitend_interrupt(void)
{
    HLIntTransmit(HL_CH_CC);
}


/************************************************************************
@name  	: r_wdt_wuni_interrupt
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void r_wdt_wuni_interrupt(void)
{
    _1ms_cnt = 0;
}







