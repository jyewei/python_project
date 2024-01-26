/***********************************************************************
@file   : Adc.c
@brief  : 
@Device : R5F5651EHGFP
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "Adc.h"
#include "r_cg_s12ad.h"



/************************************************************************
@name  	: AdcStart
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void AdcStart(void)
{
    R_S12AD0_Start();
    R_S12AD1_Start();
}

/************************************************************************
@name  	: AdcSTop
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void AdcSTop(void)
{
    R_S12AD0_Stop();
    R_S12AD1_Stop();
}

/************************************************************************
@name  	: GetAdcValue
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void GetAdcValue(uint8_t ch,uint16_t * const buffer)
{
    if (ch <= AD0_CH7)
    {
        //R_S12AD0_Get_ValueResult(ADCHANNEL0 + ch, buffer);
    }
    else
    {
        //R_S12AD1_Get_ValueResult(ch - AD1_CH0, buffer);
    }
}







