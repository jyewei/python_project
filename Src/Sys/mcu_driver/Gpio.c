/***********************************************************************
@file   : Gpio.c
@brief  : 
@Device : R5F5651EHGFP
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "Gpio.h"













/************************************************************************
@name  	: LedToggle
@brief 	: 
@param 	: None
@return	: None //PB5 GPIO_LED
*************************************************************************/
void LedToggle(void)
{
	PORTB.PODR.BIT.B5 ^= 1; //PB5

}



