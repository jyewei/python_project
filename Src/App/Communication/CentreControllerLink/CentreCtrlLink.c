/***********************************************************************
@file   : CentreCtrlLink.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "CentreCtrlLink.h"
#include "safety_function.h"
#include "HlinkDriver.h"









/************************************************************************
@name  	: CentreLinkInit
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void CentreLinkInit(void)
{
    uint8_t ch = HL_CH_CC;
    
//hlink init
    HLinkInit(ch,OFF);
    HLSetAckOpt(ch,0);
    HLSetIDTable(ch,HL_CC_TO_CHILLER);
    HLSetIDTable(ch,HL_SUB_TO_MAIN);
    HLSetAckTable(ch,HL_CC_TO_CHILLER,0,0); //TODO
    HLSetRcvOK(ch);

}


/************************************************************************
@name  	: CentreCtrlLink
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void CentreCtrlLink(uint8_t taskNo,uint8_t flag_init)
{
    SFSetRunTaskNo( taskNo );
	if (flag_init == FALSE)
    {
        CentreLinkInit();
    }






}
