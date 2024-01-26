/***********************************************************************
@file   : AntiFreezeControl.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
@note   : 
************************************************************************/
#include "AntiFreezeControl.h"
#include "Data.h"
#include "SysStateControl.h"
#include "AlarmCheck.h"


/************************************************************************
@name  	: CheckSysAntifreezeRequire
@brief 	: 
@param 	: None
@return	: None
@note   : 
*************************************************************************/
uint8_t CheckSysAntifreezeRequire(void)//TODO
{
    uint8_t res = FALSE;

//系统关机或待机状态
    if (atw.sys.local.work_step == SYS_STATUS_OFF || atw.sys.local.work_step == SYS_STATUS_STANDBY)
    {
    //1)无0#单元水流故障和无0#单元电源故障
        //if (atw.sys.local.alarm_state & )
        {
            /* code */
        }
        
    }
//系统运转状态
    else if (atw.sys.local.work_step == SYS_STATUS_RUN)
    {
        
    }
    







    return res;
}





















