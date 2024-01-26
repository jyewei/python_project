/***********************************************************************
@file   : ChillerProcess.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "ChillerProcess.h"
#include "safety_function.h"
#include "ChillerSys.h"
#include "ChillerUnit.h"
#include "Data.h"
#include "IOProcess.h"


/************************************************************************
@name  	: ChillerProcess
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ChillerProcess(uint8_t taskNo,uint8_t flag_init)
{
    SFSetRunTaskNo( taskNo );

	if (flag_init == FALSE)
    {
        ChillerSysInit();
        ChillerUnitInit();
    }

    if (GetLocalAddress() == CHILLER_MAIN_ADDR)
    {
        ChillerSysControl();
    }
    ChillerUnitControl();
      
}
