/***********************************************************************
@file   : Reset.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "Reset.h"
#include "Timer.h"
#include "lowsrc.h"
#include "IOProcess.h"
#include "TimerCounter.h"
#include <stdio.h>








/************************************************************************
@name  	: Reset
@brief 	: 
@param 	: None
@return	: None
@note	: 1.硬件初始化
		  2.update local parameters(include DSW,DI,DO,sensor)
		  3.
*************************************************************************/
void Reset(void)
{
	_INIT_IOLIB();
	Timer1msStart();  //1ms
	// printf("reset");


	TimerSet(NO_RESET_WAIT,3); //3s
	while (TimerCheck(NO_RESET_WAIT) == FALSE)
	{
		IOProcess(11,0); //update dsw,di,do,sensor
	}
	

}
