/***********************************************************************
@file   : ServiceLink.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "ServiceLink.h"
#include "safety_function.h"
#include "modbus.h"
#include "TimerCounter.h"


uint8_t flag_send = FALSE;



/************************************************************************
@name  	: ServiceLinkTransmit
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ServiceLinkTransmit(void)
{
    uint8_t tx_status;

    tx_status = RsCheckTxStatus(RS_CH_SERVICE);
    if (tx_status == FALSE)//没有正在发送
    {
        if (flag_send == TRUE)
        {
            flag_send = FALSE;
            RsRxTx[0].tx_len = 5;
            RsTxBuf[RS_CH_SERVICE][0] = 0xAA;
            RsTxBuf[RS_CH_SERVICE][1] = 0x55;
            RsTxBuf[RS_CH_SERVICE][2] = 0x00;
            RsTxBuf[RS_CH_SERVICE][3] = 0xFF;
            RsTxBuf[RS_CH_SERVICE][4] = 0xCD;
            // for(i = 0;i < RsRxTx[0].tx_len;i++)
            // {
            //     RsTxBuf[RS_CH_SERVICE][i] = RsRxTx[0].rx_buf[i];
            // }
            RsSend(RS_CH_SERVICE);
        }
    }
}


/************************************************************************
@name  	: ServiceLinkReceive
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ServiceLinkReceive(void)
{
    flag_send = TRUE;
}

/************************************************************************
@name  	: ServiceLink
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ServiceLink(uint8_t taskNo,uint8_t flag_init)
{
    SFSetRunTaskNo( taskNo );
	if (flag_init == FALSE) //init
    {
        //RsInit(RS_CH_SERVICE);
    }
    // if (RsCheckRecvData(RS_CH_SERVICE) == TRUE)//
    // {
    //     ServiceLinkReceive();
    //     RsSetRcvOK(RS_CH_SERVICE);
    // }
    // ServiceLinkTransmit();
    // RsSendFirstByte(RS_CH_SERVICE);

    // // if ((TimerCheck(NO_SERVERlLINK_TIMEOUT) == TRUE) && (RsRxTx[0].rx_cnt))
    // if ((TimerCheck(NO_SERVERlLINK_TIMEOUT) == TRUE))
    // {
    //     TimerSet(NO_SERVERlLINK_TIMEOUT,10);//1s
    //     flag_send = TRUE;
    //     // RsRxTx[0].tx_len = RsRxTx[0].rx_cnt;
    //     // RsRxTx[0].rx_cnt = 0;
    // }

}
