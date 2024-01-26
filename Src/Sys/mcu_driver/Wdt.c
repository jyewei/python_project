/***********************************************************************
@file   : Wdt.c
@brief  : 
@Device : R5F5651EHGFP
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "Wdt.h"



/************************************************************************
@name  	: WdtRestart
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void WdtRestart(void)
{
    /* Refreshed by writing 00h and then writing FFh */
    WDT.WDTRR = 0x00U;
    WDT.WDTRR = 0xFFU;
}
