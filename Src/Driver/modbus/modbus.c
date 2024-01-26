/***********************************************************************
@file   : modbus.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "modbus.h"
#include "Serial.h"
#include "TimerCounter.h"




#define RS_CH_OFFSET    (2) //note:offset with uart channel

#define MODBUS_READ_REGISTER 			((uint8_t)0x03)
#define MOBUS_WRITE_SINGLE_REGISTER		((uint8_t)0x06)
#define MOBUS_WRITE_MULTI_REGISTER		((uint8_t)0x10)

//tx ststus
typedef enum
{
    TX_NULL,
    TX_REQ,
    TX_ING,
    TX_COMPLETE,
    TX_WAIT_ACK,
    TX_SUCCESS,
    TX_FAIL,

    RX_ING,
    RX_SUCCESS,
    
}RsTxRxStataEnum;


uint8_t RsTxBuf[RS_CH_MAX][RS_BUF_MAX];
uint8_t RsRxBuf[RS_CH_MAX][RS_BUF_MAX];
RsRxTxType RsRxTx[RS_CH_MAX];


/***********************************************************************
@name  	: RsReadRDR
@brief 	: 
@param 	: ch
@return	: none
************************************************************************/
void RsReadRDR(uint8_t ch)
{
    RsRxTxType *p;
	//uint8_t err_type;
	
	p = &RsRxTx[ch];
	//err_type = UartErrorFlagGet(ch+RS_CH_OFFSET);//
	// UartErrorFlagClear(ch+RS_CH_OFFSET,err_type);	
	p->rx_data = UartRxDataGet(ch+RS_CH_OFFSET); //data recv
}

/***********************************************************************
@name  	: RsCheckRecvData
@brief 	: check data if is recv ok
@param 	: ch
@return	: none
************************************************************************/
uint8_t RsCheckRecvData(uint8_t ch) //need check cache works
{
	uint8_t ret = FALSE;
	RsRxTxType *p;
	
	p = &RsRxTx[ch];
    if (p->rx_exist == TRUE)
    {
        ret = TRUE;
    }
	return (ret);
}

/***********************************************************************
@name  	: HLSetRcvOK
@brief 	: clear HLRxBuf
@param 	: ch:uint8_t
@return	: none
************************************************************************/
void RsSetRcvOK(uint8_t ch)
{
	RsRxTxType *p;

	p = &RsRxTx[ch];
	InterruptDisable();
	
	p->rx_exist = OFF;
	InterruptEnable();

}


/************************************************************************
@name  	: RsRecvData
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static uint16_t RsRecvGetFrameLength(uint8_t ch)
{
    uint8_t slave_state = FALSE;
    uint16_t len,temp;
    RsRxTxType *p;

    p = &RsRxTx[ch];
    if ((ch == RS_CH_GROUP) || (ch == RS_CH_SERVICE) || (ch == RS_CH_BMS)) //as modbus slave
    {
        slave_state = TRUE;
    }
    switch (p->rx_buf[1])
    {
        case MODBUS_READ_REGISTER://0x03
        {
            if (slave_state == TRUE)//作为从机收到0x03
                len = 8;
            else
                len = p->rx_buf[2]*2 +5;
        }
            break;
        
        case MOBUS_WRITE_SINGLE_REGISTER://0x06
        {
            len = 8;
        }
            break;

        case MOBUS_WRITE_MULTI_REGISTER://0x10
        {
            if (slave_state == TRUE)//作为从机收到0x10
            {
                temp = (uint16_t)(p->rx_buf[4] << 8) | p->rx_buf[5];
                len = temp*2 + 8;
            }
            else
                len = 8;
        }
            break;

        default:
            break;
    }

    return len;
}

/************************************************************************
@name  	: RsRecvData
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void RsRecvData(uint8_t ch)
{
    RsRxTxType *p;
    uint16_t i;

    p = &RsRxTx[ch];
    if (p->rx_cnt < 5)//6th byte
    {
        p->rx_len = RS_BUF_MAX;
    }
    else if (p->rx_cnt == 5)
    {
        p->rx_len = RsRecvGetFrameLength(ch);
    }

    p->rx_buf[p->rx_cnt] = p->rx_data;
    p->rx_cnt++;
    if (p->rx_cnt < p->rx_len)
    {
        return;
    }
    if(p->rx_exist == TRUE)
	{
		p->rx_cnt = 0;
		return;
	}
    p->rx_exist = TRUE;
    for ( i = 1; i < RS_BUF_MAX; i++)
    {
        RsRxBuf[ch][i] = p->rx_buf[i];  //

    }
    RsRxBuf[ch][0] = p->rx_cnt;
    //reset init state
    p->rx_cnt = 0;
    for ( i = 0; i < RS_BUF_MAX; i++)
    {
        p->rx_buf[i] = 0;               //clear
    }
}



/************************************************************************
@name  	: RsRecvData
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void RsSendData(uint8_t ch)
{
    RsRxTxType *p;
    uint8_t dat;

	p = &RsRxTx[ch];
    dat = RsTxBuf[ch][p->tx_cnt];
    UartTxDataSet(ch+RS_CH_OFFSET,dat);
    p->tx_cnt++;
    if (p->tx_cnt < p->tx_len)
    {
        return;
    }
    p->tx_state = TX_COMPLETE;
}

/***********************************************************************
@name  	: HLSendFirstByte
@brief 	: 1st byte transmission process
@param 	: ch
@return	: none
************************************************************************/
void RsSendFirstByte(uint8_t ch)
{
    RsRxTxType *p;

	p = &RsRxTx[ch];
    if (p->tx_state == TX_REQ)
    {
        InterruptDisable();
        if( UartIsTxDataEmpty(ch+RS_CH_OFFSET))//
        {
            UartTxDataSet(ch+RS_CH_OFFSET,RsTxBuf[ch][0]);
            p->tx_cnt++;
            p->tx_state = TX_ING;
            UartSendEndIntEnable(ch+RS_CH_OFFSET);
        }
        InterruptEnable();
    }
}

/***********************************************************************
@name  	: HLSend
@brief 	: calculate bcc value,and change tx_status
@param 	: ch
@return	: none
************************************************************************/
uint8_t RsSend(uint8_t ch)
{
	uint8_t ret = FALSE;
	//uint8_t i,len;
	RsRxTxType	*p;

	p = &RsRxTx[ch];
    //TODO set crc and get length
    p->tx_cnt = 0;
    p->tx_state = TX_REQ;
    ret = TRUE;

	return ret;
}

/***********************************************************************
@name  	: HLCheckTxStatus
@brief 	: 
@param 	: ch
@return	: none
@note	: none
************************************************************************/
uint8_t RsCheckTxStatus(uint8_t ch)
{
	uint8_t ret = TRUE;
	RsRxTxType	*p;

	p = &RsRxTx[ch];
// check tx_staus
	if ((p->tx_state == TX_NULL) || (p->tx_state == TX_SUCCESS) || (p->tx_state == TX_FAIL))//
	{
		ret = FALSE;
	}

	return ret;
}

/***********************************************************************
@name  	: RsInit
@brief 	: 
@param 	: none
@return	: none
************************************************************************/
void RsInit(uint8_t ch)
{
    UartStart(ch+RS_CH_OFFSET);
	UartRxDisable(ch+RS_CH_OFFSET);
	UartTxDisable(ch+RS_CH_OFFSET);
	UartINTSRxDisable(ch+RS_CH_OFFSET);
	UartINTSTxDisable(ch+RS_CH_OFFSET);
	UartSendEndIntDisable(ch+RS_CH_OFFSET);
	
	UartParityChange(ch+RS_CH_OFFSET,PARITY_EVEN);
	UartINTSRxEnable(ch+RS_CH_OFFSET);
	UartRxEnable(ch+RS_CH_OFFSET);
	UartTxEnable(ch+RS_CH_OFFSET);
	HLReadRDR(ch+RS_CH_OFFSET);
}

/************************************************************************
@name  	: RsIntReceive
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void RsIntReceive(uint8_t ch)
{
    RsRxTxType *p;
	uint8_t err_type;

	p = &RsRxTx[ch];	
	err_type = UartErrorFlagGet(ch+RS_CH_OFFSET);	//@note:RS_CH_OFFSET except hlink channel
	p->rx_data = UartRxDataGet(ch+RS_CH_OFFSET); 	//data recv
	UartErrorFlagClear(ch+RS_CH_OFFSET,err_type);	//

    // TimerSet(NO_SERVERlLINK_TIMEOUT,5);//500ms
    p->rx_buf[p->rx_cnt] = p->rx_data;
    p->rx_cnt++;
    // if (err_type != 0)
    // {
    //     RsRecvData(ch);
    // }
}

/************************************************************************
@name  	: RsIntTransmit
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void RsIntTransmit(uint8_t ch)
{
    RsRxTxType *p;

	p = &RsRxTx[ch];
    if (p->tx_state == TX_COMPLETE)
    {
        UartSendEndIntDisable(ch+RS_CH_OFFSET);
        p->tx_cnt = 0;
        p->tx_state = TX_SUCCESS; //TODO 
        // p->tx_state = TX_WAIT_ACK;
        p->rx_cnt = 0;
    }
    else if (p->tx_state == TX_ING)
    {
        RsSendData(ch);
    }
}

