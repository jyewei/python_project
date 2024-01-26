/***********************************************************************
@file   : InverterLink.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "InverterLink.h"
#include "safety_function.h"
#include "TimerCounter.h"



// //base 100ms
// #define TIME_INTERVAL_SEND  ((uint18_t)5)    //500ms
// #define TIME_CONNECT_OUT    ((uint16_t)300)  //30s
// #define TIME_TIMEOUT        ((uint18_t)5)    //200ms  timeout wait

// //time 
// typedef struct
// {
// 	uint16_t interval_transmit;  // 
//     uint16_t connect_out[INV_NO_MAX];
               


// }InverterLinkTimeType;

// static InverterLinkTimeType time;



// InverterParametersType inverter[INV_NO_MAX];


// /************************************************************************
// @name  	: ChillerLinkMainTimerCounter
// @brief 	: 
// @param 	: None
// @return	: None
// *************************************************************************/
// static void InverterLinkTimerCounter(void)
// {
//     uint16_t *pTime;
//     uint16_t i,len;

//     pTime = &time.interval_transmit;
//     len = sizeof(InverterLinkTimeType)/sizeof(uint16_t);
//     if (TimerCheck(NO_INV_LINK_TIME_BASE) == TRUE)
//     {
//         TimerSet(NO_INV_LINK_TIME_BASE,1); //base 100ms
//         for ( i = 0; i < len; i++)
//         {
//             if (*(pTime + i) != 0)
//             {
//                 (*(pTime + i))--;
//             }
//         }
//     }
// }


// /************************************************************************
// @name  	: InverterLinkInit
// @brief 	: 
// @param 	: None
// @return	: None
// *************************************************************************/
// static void InverterLinkInit(void)
// {
//     uint8_t i;

//     time.interval_transmit = 50;   //5s
//     inverter[INV_COMP1].addr = 2;
//     inverter[INV_COMP2].addr = 4;
//     inverter[INV_FAN1].addr = 1;
//     inverter[INV_FAN2].addr = 3;
//     inverter[INV_FAN3].addr = 5;
//     inverter[INV_FAN4].addr = 7;

//     for (i = INV_COMP1; i < INV_NO_MAX; i++)
//     {
//         inverter[i].connect_state = FALSE;
//     }
    
// }

// /************************************************************************
// @name  	: InverterLinkReceive
// @brief 	: 
// @param 	: None
// @return	: None
// *************************************************************************/
// static void InverterLinkReceive(void)
// {

// }

// /************************************************************************
// @name  	: InverterLinkTransmit
// @brief 	: 
// @param 	: None
// @return	: None
// *************************************************************************/
// static void InverterLinkTransmit(void)
// {
//     uint8_t ch;

//     for (ch = INV_COMP1; ch < INV_NO_MAX; ch++)
//     {
//         if (inverter[ch].enable_state == TRUE)
//         {
            
//         }
//     }

// }

/************************************************************************
@name  	: FanLinkHandle
@brief 	: 
@param 	: None
@return	: None
@note   : 无关驱动,通过API函数更新来自link的数据,并对数据进行更新及处理
*************************************************************************/
void InverterLink(uint8_t taskNo,uint8_t flag_init)
{
    SFSetRunTaskNo( taskNo );
    // if (flag_init == FALSE)
    // {
    //     InverterLinkInit();
    // }
    // InverterLinkReceive();
    // InverterLinkTransmit();
	// InverterLinkTimerCounter();
//1.通过API函数更新cycle中link的数据

//2.进行数据处理
    

}
