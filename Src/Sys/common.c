/***********************************************************************
@file   : main.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "common.h"
#include "TimerCounter.h"
#include <stdio.h>




void logout(const char* string,uint32_t data)
{
    DataTime dt = GetDataTime();
    //uint16_t string_len = strlen(string);

    // printf("h-%d:mins-%d:s-%d:ms-%lu",dt.cell.hour,dt.cell.mins,dt.cell.s,dt.cell.ms);
    // printf("h-%u:mins-%u:s-%u:ms-%u",65530,59,59,666);
    printf(string,data);

}








