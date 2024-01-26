/***********************************************************************
@file   : ExpvControl.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "ExpvControl.h"
#include "ExpvDriver.h"
#include "data.h"
#include "ExpvSubControl.h"
#include "ExpvMainControl.h"



/************************************************************************
@name  	: ExpvZeroSet
@brief 	: 
@param 	: cycle:CYCLE1~CYCLE4,expv:EXPV1/EXPV2,EXPV3
@return	: None
@note   : 膨胀阀的归零控制
*************************************************************************/
void ExpvZeroSet(uint8_t cycle,uint8_t expv)
{


}

/************************************************************************
@name  	: ExpvZeroSet
@brief 	: 
@param 	: cycle:CYCLE1~CYCLE4,expv:EXPV1/EXPV2,EXPV3
@return	: None
@note   : 膨胀阀的复位
*************************************************************************/
void ExpvResetSet(uint8_t cycle,uint8_t expv)
{


}

/************************************************************************
@name  	: ExpvInit
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ExpvInit(uint8_t cycle)
{

}

/************************************************************************
@name  	: ExpvControl
@brief 	: 
@param 	: cycle:CYCLE1~CYCLE4,expv:EXPV1/EXPV2,EXPV3
@return	: None
*************************************************************************/
void ExpvControl(uint8_t cycle)
{
    ExpvSubStatusControl(cycle);
    ExpvMainStatusControl(cycle);
}



