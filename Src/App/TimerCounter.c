/***********************************************************************
@file   : TimerCounter.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "TimerCounter.h"
#include "wdt.h"


uint16_t timercounter_1ms[COUNT_1MS];
uint16_t timercounter_10ms[COUNT_10MS];
uint16_t timercounter_100ms[COUNT_100MS];
uint16_t timercounter_1s[COUNT_1S];
uint16_t timercounter_1mins[COUNT_1MINS];
uint16_t timercounter_1hour[COUNT_1HOUR];





DataTime data_time;


/************************************************************************
@name  	: DataTimeHandle
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void DataTimeHandle(void)
{
	data_time.cell.ms++;
	if (data_time.cell.ms > 999)
	{
		data_time.cell.ms = 0;
		data_time.cell.s++;
		if (data_time.cell.s > 59)
		{
			data_time.cell.s = 0;
			data_time.cell.mins++;
			data_time.cell.total_mins++;
			if (data_time.cell.mins > 59)
			{
				data_time.cell.mins = 0;
				data_time.cell.hour++;
			}
		}
	}
}


/************************************************************************
@name  	: TimerByteDecrease
@brief 	: decrease byte in time
@param 	: None
@return	: None
*************************************************************************/
static void TimerDownByte(uint16_t *p,uint16_t count)
{
	uint16_t i;

	for ( i = 0; i < count; i++)
	{
		if (p[i] != 0)
		{
			p[i]--;
		}
	}
}

/***********************************************************************
@name  	: TimerDown1hour
@brief 	: every 1hour
@param 	: None
@return	: None
************************************************************************/
static void TimerDown1hour(void)
{
	if (TimerCheck(NO_BASE_1HOUR) == TRUE)
	{
		TimerSet(NO_BASE_1HOUR,60); //60mins
		TimerDownByte(timercounter_1hour,COUNT_1HOUR);
	}
}

/***********************************************************************
@name  	: TimerDown1mins
@brief 	: every 1mins 
@param 	: None
@return	: None
************************************************************************/
static void TimerDown1mins(void)
{
	if (TimerCheck(NO_BASE_1MINS) == TRUE)
	{
		TimerSet(NO_BASE_1MINS,60); //1mins
		TimerDownByte(timercounter_1mins,COUNT_1MINS);
		TimerDown1hour();
	}
}

/***********************************************************************
@name  	: TimerDown1s
@brief 	: every 1s 
@param 	: None
@return	: None
************************************************************************/
static void TimerDown1s(void)
{
	if (TimerCheck(NO_BASE_1S) == TRUE)
	{
		TimerSet(NO_BASE_1S,10); //1s
		TimerDownByte(timercounter_1s,COUNT_1S);
		TimerDown1mins();
	}
}

/***********************************************************************
@name  	: TimerDown100ms
@brief 	: every 100ms 
@param 	: None
@return	: None
************************************************************************/
static void TimerDown100ms(void)
{
	if (TimerCheck(NO_BASE_100MS) == TRUE)
	{
		TimerSet(NO_BASE_100MS,10); //100ms
		TimerDownByte(timercounter_100ms,COUNT_100MS);
		TimerDown1s();
	}
}

/***********************************************************************
@name  	: TimerDown10ms
@brief 	: 
@param 	: None
@return	: None
************************************************************************/
static void TimerDown10ms(void)
{
	if (TimerCheck(NO_BASE_10MS) == TRUE)
	{
		TimerSet(NO_BASE_10MS,10); //10ms
		TimerDownByte(timercounter_10ms,COUNT_10MS);
		TimerDown100ms();
	}
}

/************************************************************************
@name  	: TimerCheck
@brief 	: 
@param 	: None
@return	: true/false
*************************************************************************/
uint8_t TimerCheck(uint16_t timerNo)
{
	uint8_t res = FALSE;
	uint8_t type,no;

	type = (uint8_t)(timerNo / 0x0100);
    no = (uint8_t)(timerNo % 0x0100);

	switch (type)
	{
	case 0:
		if (timercounter_1ms[no] == 0)
		{
			res = TRUE;
		}
		break;
	
	case 1:
		if (timercounter_10ms[no] == 0)
		{
			res = TRUE;
		}
		break;

	case 2:
		if (timercounter_100ms[no] == 0)
		{
			res = TRUE;
		}
		break;

	case 3:
		if (timercounter_1s[no] == 0)
		{
			res = TRUE;
		}
		break;

	case 4:
		if (timercounter_1mins[no] == 0)
		{
			res = TRUE;
		}
		break;

	case 5:
		if (timercounter_1hour[no] == 0)
		{
			res = TRUE;
		}
		break;				
	default:
		break;
	}

	return res;
}

/************************************************************************
@name  	: TimerSet
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void TimerSet(uint16_t timerNo,uint16_t val)
{
	uint8_t type,no;

	type = (uint8_t)(timerNo / 0x0100);
    no = (uint8_t)(timerNo % 0x0100);

	switch (type)
	{
	case 0: timercounter_1ms[no]   = val;	break; 	//1ms
	case 1: timercounter_10ms[no]  = val;	break;	//10ms
	case 2: timercounter_100ms[no] = val;	break;	//100ms
	case 3: timercounter_1s[no]    = val;	break;	//1s
	case 4: timercounter_1mins[no] = val;	break;	//1mins
	case 5: timercounter_1hour[no] = val;	break;	//1hour
	default: break;
	}
}

/************************************************************************
@name  	: TimerCounterHandle
@brief 	: 1ms interrupt to update timercounter
@param 	: None
@return	: None
*************************************************************************/
void TimerCounterHandle(void) //1ms interrupt
{
	WdtRestart();
	DataTimeHandle();
	TimerDownByte(timercounter_1ms,COUNT_1MS);
	TimerDown10ms();
}

DataTime GetDataTime(void)
{
	return data_time;
}

