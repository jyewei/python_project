/***********************************************************************
@file   : main.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "main.h"
#include "common.h"
#include "safety_function.h"
#include "AlarmCheck.h"
#include "ProtectProcess.h"
#include "ChillerProcess.h"
#include "GroupLink.h"
#include "BMSLink.h"
#include "CentreCtrlLink.h"
#include "ChillerLink.h"
#include "ChillerSlaveLink.h"
#include "InverterLink.h"
#include "ServiceLink.h"
#include "ThermostatLink.h"
#include "Data.h"
#include "EepromProcess.h"
#include "IOProcess.h"
#include "UsbProcess.h"
#include "BoardCheck.h"
#include "BoxCheck.h"
#include "Wdt.h"
#include "Reset.h"


#define	TNUL		(0U)	//Do not execute tasks
#define	TEXE		(1U)	//Execute unconditional tasks
#define	TTMW		(2U)	//Execute on time
#define TASK_NO_MAX (17)	//max task number


//
RUN_MODE run_mode;

//
static const struct 
{
	void 	(*task)(uint8_t, uint8_t);//taskNo,init_flag
	uint8_t		mode[4];

} TaskTbl[TASK_NO_MAX] = 
{
//	  function		 	 normal	unit  box   self				TaskNo
	{ ChillerLink,			{ TEXE, TNUL, TEXE, TNUL }	},		//	0:chiller link
	{ GroupLink,			{ TEXE, TNUL, TEXE, TNUL }	},		//	1:group link
	{ ChillerSlaveLink,		{ TEXE, TNUL, TEXE, TNUL }	},		//	2:chiller slave link
	{ InverterLink,			{ TEXE, TNUL, TEXE, TNUL }	},		// 	3:fan & comp link
	{ CentreCtrlLink,		{ TEXE, TNUL, TEXE, TNUL }	},		//	4:CentreController link
	{ ThermostatLink,		{ TEXE, TNUL, TEXE, TNUL }	},		//	5:smart thermostat link
	{ ServiceLink,			{ TEXE, TNUL, TEXE, TNUL }	},		//	6:service link
	{ BMSLink,				{ TEXE, TNUL, TEXE, TNUL }	},		//	7:BMS link			//communication
	{ DataProcess,			{ TEXE, TNUL, TEXE, TEXE }	},		//	8:data process   //
	{ AlarmCheck,			{ TEXE, TNUL, TNUL, TNUL }	},		//	9:alarm check
	{ ProtectProcess,		{ TEXE, TNUL, TNUL, TNUL }	},		// 10:Protect control
	{ IOProcess,			{ TEXE, TNUL, TNUL, TNUL }	},		// 11:IO process & seg display //DI,DO,SENSOR
	{ ChillerProcess,		{ TEXE, TNUL, TNUL, TNUL }	},		// 12:chiller control // run control //deforest and other
	{ EepromProcess,		{ TEXE, TNUL, TNUL, TNUL }	},		// 13:eeprom process 
	{ BoardCheck,			{ TNUL, TNUL, TNUL, TTMW }	},		// 14:BoardCheck
	{ BoxCheck,				{ TNUL, TNUL, TTMW, TNUL }	},		// 15:boxcheck
	{ UsbProcess,			{ TEXE, TNUL, TNUL, TNUL }	}		// 16:usb handle //none
};

/************************************************************************
@name  	: RunTask
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void RunTask( uint8_t taskNo, uint8_t flag_init )
{
	SFClearRunTaskNo();

	TaskTbl[ taskNo ].task( taskNo, flag_init );

	if( SFTestRunTaskNo( taskNo ) != SF_NORMAL )
	{
		Err_reset( SF_ERRNO_TASKNO_ABNORMAL );
	}
}

// /************************************************************************
// @name  	: InspectMemory
// @brief 	: 
// @param 	: None
// @return	: None
// *************************************************************************/
// static void InspectMemory( void )
// {
// 	if( SFTestPeriodicROM() != SF_NORMAL )
// 	{
// 		Err_reset( SF_ERRNO_ROM_CHECK_PERIODIC );
// 	}

// 	if( SFTestPeriodicRAM() != SF_NORMAL )
// 	{
// 		Err_reset( SF_ERRNO_RAM1_CHECK_PERIODIC );
// 	}
// }

// /************************************************************************
// @name  	: InspectRunaway
// @brief 	: 
// @param 	: None
// @return	: None
// *************************************************************************/
// static void InspectRunaway( void )
// {
// 	if( SFPCErrcheck() != SF_NORMAL )
// 	{
// 		Err_reset( SF_ERRNO_SP_ILVL_ABNORMAL );
// 	}
// }

/************************************************************************
@name  	: main
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void main(void)
{
	volatile uint8_t i;
	volatile uint8_t flag_init = FALSE;	

	Reset();
	run_mode = NORMAL_MODE;

	while (1)
	{
		// InspectMemory();	//ROM,RAM check
		for ( i=0; i < TASK_NO_MAX ; i++ )
		{
			// InspectRunaway();
			switch ( TaskTbl[i].mode[run_mode] ) 
			{
				case TEXE: 	RunTask( i, flag_init );break;
				default:	break;
			}
			WdtRestart();
		}
		flag_init = TRUE;

	}
}
