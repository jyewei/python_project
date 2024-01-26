/***********************************************************************
@file   : HlinkDriver.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "HlinkDriver.h"
#include "Serial.h"
#include "Timer.h"
#include "Gpio.h"
#include "iodefine.h"

#define	SELF_TEST	((uint8_t)0x02)		/* b1:自己診断時					*/

#define RX_ING      ((uint8_t)0x01)		/* bit0: receiving	    */
#define RX_ERR      ((uint8_t)0x02)		/* bit1: receive err    */
#define RX_EXIST    ((uint8_t)0x04)		/* bit2: receive exist  */

#define	TX_RETRY_REQ    ((uint8_t)0x01)	/* bit0:	        */
#define	TX_ING			((uint8_t)0x02)	/* bit1: sending	*/
#define	TX_SUCCESS		((uint8_t)0x04)	/* bit2: send ok	*/
#define	TX_FAIL			((uint8_t)0x08)	/* bit3: send fail	*/
#define	TX_WAIT_ACK		((uint8_t)0x10)	/* bit4: wait ack	*/
#define	TX_REQ			((uint8_t)0x80)	/* bit7: send request*/

#define COL_EXIST		((uint8_t)0x01)	/* bit0: collision exist	 */
#define	COL_SENDED_AA	((uint8_t)0x02)	/* bit1: after send 0xAA	 */

#define	ACK_SNDING		((uint8_t)0x01)	/* bit0: sending 0xAA		 */
#define	ACK_SENDED_06	((uint8_t)0x02)	/* bit1: after send 0x06(ACK)*/

#define	POS_ID		    ((uint8_t)0)    /* +0 byte:id code*/
#define	POS_ACK			((uint8_t)1)	/* +1 byte: ACK(06h)			*/
#define	POS_TYPE		((uint8_t)1)	/* +1 byte:	type code			*/
#define	POS_LEN			((uint8_t)2)	/* +2 byte:	frame length		*/
#define	POS_SRC_SYS	    ((uint8_t)3)	/* +3 byte:	source system		*/
#define	POS_SRC_ADDR    ((uint8_t)4)	/* +4 byte:	source address	    */
#define	POS_DST_SYS	    ((uint8_t)5)	/* +5 byte:	destination system	*/
#define	POS_DST_ADDR	((uint8_t)6)	/* +6 byte:	destination address	*/

#define	TM_ACK_TIMEOUT	((uint8_t)6)	/* wait ack timeout(ack receive in 6ms when send over)  */
#define	TM_APP_TIMEOUT	((uint8_t)30)	/* app not handle frame timeout(30s)                    */
#define	TM_LINE_RELEASE	((uint8_t)200)	/* re_transmit wait time(200ms)	                        */

#define	ACK				((uint8_t)0x06)		/* ack code				*/
#define	COLLISION		((uint8_t)0xAA)		/* when check collision,send 0xAA(announce collision on bus)*/

#define	ACK_SEND_ENTRY_MAX	((uint8_t)32)	/* 	*/
#define	ID_ENTRY_MAX		((uint8_t)16)	/* 	*/

#define	SEND_RETRY_MAX		((uint8_t)3)	/* re_send time			*/
#define	FRAME_SIZE_MIN		((uint8_t)8)	/* length min size		*/


//
typedef struct
{
	uint8_t	id;	         /* id code				*/
	uint8_t	ref_sys;	/* refrigerant system	*/
	uint8_t	addr;	    /* address				*/

} AckTable;

//
typedef struct 
{
	uint8_t	mode;
	uint8_t	frame_length;
	uint8_t	rx_status;
	uint8_t	tx_status;
	uint8_t	col_status;
	uint8_t	ack_status;	
	AckTable ack_send_tab[ACK_SEND_ENTRY_MAX];	/* judge if need send ack		*/
	uint8_t	id_table[ID_ENTRY_MAX];			/* id code 				*/

	uint8_t	rx_buf[HL_BUF_SIZE];			/* rx buff	*/
	uint8_t	rx_cnt;
    uint8_t	rx_len;
    uint8_t	rx_data;
	uint8_t	tx_cnt;
	uint8_t	tx_len;
	uint8_t	tx_retry_cnt;
	uint8_t	bcc_work;
	
	uint8_t	 ack_option;					/* if ack need 	(0:ack need  1:ack not need)    */  
	int8_t	 ack_entry_cnt;					/* ack table counter	                        */
	int8_t	 id_entry_cnt;					/* Identification code table counter		    */
	uint8_t	 tx_complete;					/* 1:tx complete				                */
	uint8_t	 rx_exist;						/* 1:exist				                        */
	uint16_t tm_carry;						/* decrease every 1ms		                    */
	uint8_t	 tm_tx_retry;					/* time tx retry                            	*/

	uint8_t	tm_1ms;							/* 100ms base                                   */
	uint8_t	tm_100ms;						/* 1s base                                      */
	uint8_t	tm_app_err;						/* frame tiomeout check                 		*/
	uint8_t	tm_1ms_free;					/* General purpose timer	                	*/
	uint8_t	carry_counter;					/* Career detection						        */
	uint8_t	tx_retry_cnt_max;				/* when no ack,retry  numbermax		            */

} HLRxTxType;


//
uint8_t HLTxBuf[HL_CH_MAX][HL_BUF_SIZE];
uint8_t HLRxBuf[HL_CH_MAX][HL_BUF_SIZE];
HLRxTxType HLRxTx[HL_CH_MAX];



/***********************************************************************
@name  	: HLinkICReset
@brief 	: ic reset set
@param 	: state:ON;OFF
@return	: None
************************************************************************/
static void HLinkICReset(uint8_t ch,uint8_t state)
{
    switch (ch)
    {
		case HL_CH_CHILLER: 	CH_CC_RST_PIN = state;		break;
		case HL_CH_CC:			CH_CHILLER_RST_PIN = state;	break;
		default:
			break;
    }
}

/***********************************************************************
@name  	: HLDelay10us
@brief 	: 
@param 	: None
@return	: None
************************************************************************/
static void HLDelay10us(void) //wait 10us
{
	uint16_t i;

    for ( i = 0; i < 100; i++)
    {
        __nop();
    }
}

/***********************************************************************
@name  	: IdCodeExchange
@brief 	: exchange 4bit
@param 	: None
@return	: None
************************************************************************/
static uint8_t IdCodeExchange(uint8_t dat)
{
	uint8_t dat1,dat2;

	dat1 = dat2 = dat;
	dat1 <<= 4;
	dat2 >>= 4;
	dat1 = (dat1 & 0xF0) | dat2;

	return dat1;
}

/***********************************************************************
@name  	: HLReadRDR
@brief 	: 
@param 	: ch
@return	: none
************************************************************************/
void HLReadRDR(uint8_t ch)
{
	HLRxTxType *p;
	uint8_t err_type;
	
	p = &HLRxTx[ch];
	err_type = UartErrorFlagGet(ch);//
	UartErrorFlagClear(ch,err_type);	
	p->rx_data = UartRxDataGet(ch); //data recv
}

/***********************************************************************
@name  	: HLReHLRecvErradRDR
@brief 	: 
@param 	: ch
@return	: none
************************************************************************/
static void HLRecvErr(uint8_t ch)
{
	HLRxTxType *p;

	p = &HLRxTx[ch];
	p->rx_status = RX_ERR;
	p->bcc_work  = NULL;
}

/***********************************************************************
@name  	: HLInit2
@brief 	: 
@param 	: none
@return	: none
************************************************************************/
static void HLInitUart(uint8_t ch)
{
	// UartStop(ch);					//transmission stop
	// UartParityChange(ch,PARITY_EVEN);
	// HLReadRDR(ch);						//Clear error flag, read received data
	// UartINTSRxEnable(ch);			//UART receive interrupt enabled
	// UartSendEndIntEnable(ch);		//Change to transmission completion interrupt
	// UartINTSTxEnable(ch);			//UART transmission interrupt enabled
	// UartTxClockStop(ch);				//Transmission clock stop//pwm
	// UartStart(ch);					//UART transmission start
	// HLReadRDR(ch);						
}

/***********************************************************************
@name  	: HLInit2
@brief 	: 
@param 	: none
@return	: none
************************************************************************/
void HLInit2(uint8_t ch,uint8_t self)
{
    HLRxTxType *p;

	p = &HLRxTx[ch];
    //param init
    p->rx_status = RX_ING;
    p->tx_status = HL_OFF;
    p->ack_status = HL_OFF;
    p->col_status = HL_OFF;
    p->rx_cnt = 0;
    p->tx_cnt   = 0;
    p->rx_exist = HL_OFF;
    p->frame_length   = 48;   //default
    p->tx_retry_cnt_max = 3;//send retry max
    p->mode =HL_OFF;

    if (self != HL_OFF)
    {
        p->mode |= SELF_TEST;
    }
    HLInitUart(ch);
}

/***********************************************************************
@name  	: HLClearAllAckTbl
@brief 	: Erase all ACK transmission judgment table (API)
@param 	: none
@return	: none
************************************************************************/
void HLClearAllAckTab(uint8_t ch)
{
    uint8_t i;
    HLRxTxType *p;

	p = &HLRxTx[ch];
	p->ack_entry_cnt = 0;					//ACK
	for ( i=0; i<ACK_SEND_ENTRY_MAX; i++ )  //ACK transmission judgment table clear
    {
		p->ack_send_tab[i].id = NULL;		//identifying code
		p->ack_send_tab[i].ref_sys = NULL;		//Destination system
		p->ack_send_tab[i].addr = NULL;		//Destination address
	}
}

/***********************************************************************
@name  	: HLClearAllIDTab
@brief 	: Erase all ID table (API)
@param 	: none
@return	: none
************************************************************************/
void HLClearAllIDTab(uint8_t ch)
{
    uint8_t i;
    HLRxTxType *p;

	p = &HLRxTx[ch];
	p->id_entry_cnt = 0;
	for ( i=0; i<ID_ENTRY_MAX; i++ )
    {				
		p->id_table[i] = NULL;
	}
}

/***********************************************************************
@name  	: HLInit
@brief 	: 
@param 	: none
@return	: none
************************************************************************/
void HLInit(uint8_t ch,uint8_t self)
{
	HLInit2(ch,self);
	HLClearAllAckTab(ch);
	HLClearAllIDTab(ch);
}

/***********************************************************************
@name  	: HLSetAckOpt
@brief 	: Identification code registration (API)
@param 	: 
@return	: 
************************************************************************/
void HLSetAckOpt(uint8_t ch, uint8_t dat)
{
    HLRxTxType *p;

	p = &HLRxTx[ch];
    if (dat != 0)
    {
        p->ack_option = HL_ON;
    }
    else
    {
        p->ack_option = HL_OFF;
    }
}


/***********************************************************************
@name  	: HLCarryTMSet
@brief 	: 
@param 	: 
@return	: 
************************************************************************/
void HLCarryTMSet(uint8_t ch, uint16_t time)
{
    HLRxTxType *p;

	p = &HLRxTx[ch];
	p->tm_carry = time;
}


/***********************************************************************
@name  	: HLCheckSelfSendData
@brief 	: check data send by self 
@param 	: ch
@return	: none
************************************************************************/
static void HLCheckSelfSendData(uint8_t ch,uint8_t err_type)
{
	HLRxTxType	*p;

	p = &HLRxTx[ch];
	p->rx_cnt++;	//receive counter +1	

	if( p->rx_cnt == 1) //When sending a normal telegram, the first byte collision is detected.
	{
		if ((p->ack_status == HL_OFF) && (p->col_status == HL_OFF))
		{
			if ((err_type & (UART_ERR_FE | UART_ERR_OE))
				|| (HLTxBuf[ch][p->rx_cnt-1] != p->rx_data))
			{
				p->col_status = COL_EXIST;
				UartRxDisable(ch);
				UartINTSRxDisable(ch);
			}
		}
		return;
	}

	if( (p->col_status & COL_SENDED_AA) != HL_OFF )//send 0xAA
	{
		return;
	}

    if (err_type != 0U) //if have err
	{
		p->col_status = COL_EXIST;	//Flag set with collision
		UartRxDisable(ch);
		UartINTSRxDisable(ch);	//UART receive interrupt disabled
		return;
	}

	if ( HLTxBuf[ch][p->rx_cnt-1] != p->rx_data )//When the received data and the transmitted data are different
	{													
		p->col_status = COL_EXIST;	//collision flag set 	//TODO tx_cnt!=rx_cnt happens too much
		UartRxDisable(ch);
		UartINTSRxDisable(ch);	//
	}
}

/***********************************************************************
@name  	: IsFirstByte
@brief 	: first byte judge if the id is right,id_table
@param 	: ch ; dat
@return	: 
************************************************************************/
static uint8_t IsFirstByte(uint8_t ch,uint8_t dat)
{
	HLRxTxType *p;
	uint8_t ret = HL_FALSE;
	uint8_t i;

	p = &HLRxTx[ch];
	for (i = 0; i < p->id_entry_cnt; i++)
	{
		if (p->id_table[i] == dat)
		{
			ret = HL_TRUE;
			break;
		}
	}
	return ret;
}

/***********************************************************************
@name  	: HLRecvFirstByte
@brief 	: first byte judge if the id is right
@param 	: ch ; dat
@return	: none
@note 	: parity err is used to judge the first byte,first byte is odd parity,other is even parity
************************************************************************/
static void HLRecvFirstByte(uint8_t ch,uint8_t err_type)
{
	HLRxTxType *p;

	p = &HLRxTx[ch];
	if ((err_type & UART_ERR) != UART_ERR_PE )
	{
		return;
	}

	if( IsFirstByte(ch, p->rx_data) == HL_FALSE ) //check id_table 
	{
		return;	// Discard if it is not the default identification code
	}
	p->rx_buf[0] = p->rx_data;	//Set the data to the primary buffer
	p->rx_cnt = 1;		
	p->bcc_work	 = NULL;		// Work clear for BCC calculation
}

/***********************************************************************
@name  	: HLRecvACK
@brief 	: handle with ACK ,when recv ack,tx success
@param 	: ch
@return	: none
@note	: // thrm ack_format is not same as wrc ack_format 
************************************************************************/
static void HLRecvACK(uint8_t ch)
{
	HLRxTxType *p;

	p = &HLRxTx[ch];
	if ((p->tx_status &TX_WAIT_ACK) != HL_OFF)	//waiting for ACK
	{
		if( HLTxBuf[ch][0] == (uint8_t)(IdCodeExchange( p->rx_buf[0])))	//Identification code for your own station
		{
			p->tx_status = TX_SUCCESS;
		}	
	}
	p->bcc_work = NULL;
	p->rx_status = RX_ING;
	p->rx_cnt = 0;
}

/***********************************************************************
@name  	: HLCheckAckRequire
@brief 	: ACK transmission necessity judgment processing
@param 	: ch
@return	: 0 = ACK transmission not required, 1 = ACK transmission required
@note	: all frame need ack-zyx20220513
************************************************************************/
static uint8_t HLCheckAckRequire(uint8_t ch)
{
	HLRxTxType	*p;
	uint8_t i;

	p = &HLRxTx[ch];
//1.broadcast
	if (p->rx_buf[POS_DST_ADDR] == BROADCAST) //
	{
		return HL_FALSE;
	}
//2.ack option
	if (p->ack_option == HL_ON)
	{
		return HL_FALSE;
	}
//3.ack table
	for ( i = 0; i < p->ack_entry_cnt; i++)
	{
		if (p->ack_send_tab[i].id == p->rx_buf[POS_ID] || p->ack_send_tab[i].id == 0xAA)//id code is same
		{
			if (p->ack_send_tab[i].ref_sys == p->rx_buf[POS_DST_SYS] || p->ack_send_tab[i].ref_sys == 0xAA)//refrigerant sys is same
			{
				if (p->ack_send_tab[i].addr == p->rx_buf[POS_DST_ADDR] || p->ack_send_tab[i].addr == 0xAA)//address is same
				{
					return HL_TRUE;
				}
			}
		}
	}
	return (FALSE);
}

/***********************************************************************
@name  	: HLSendAckFirstByte
@brief 	: 
@param 	: none
@return	: none
************************************************************************/
static void	HLSendAckFirstByte(uint8_t ch)
{
	HLRxTxType	*p;
	uint8_t	dat;

	p = &HLRxTx[ch];
	UartRxEnable(ch);
	UartINTSRxDisable(ch);			//receive interrupt disable
	UartParityChange(ch, PARITY_ODD);			//0724
	TimerHLClockStop(ch);				//
	dat = IdCodeExchange(p->rx_buf[POS_ID]); 
	UartTxDataSet(ch,dat);		//
	TimerHLClockStart(ch);

	p->ack_status = ACK_SNDING;			//
	p->tx_complete = HL_OFF;			//
	p->rx_cnt = 0;				//
	p->bcc_work = NULL;					//
	UartSendEndIntEnable(ch);	//
}

/***********************************************************************
@name  	: HLSetRcvOK_IR
@brief 	: 
@param 	: none
@return	: none
************************************************************************/
static void HLSetRcvOK_IR(uint8_t ch)
{
	HLRxTxType	*p;
	uint8_t i;

	p = &HLRxTx[ch];
	for ( i=0; i<HL_BUF_SIZE; i++ )//Primary receive buffer all clear
	{				
		p->rx_buf[i] = NULL;
	}
	HLReadRDR(ch);
	UartRxEnable(ch);			
	UartINTSRxEnable(ch);	
	p->rx_status = RX_ING;	
}

/***********************************************************************
@name  	: HLRecvData
@brief 	: handle with data received 
@param 	: ch
@return	: none
************************************************************************/
static void HLRecvData(uint8_t ch)
{
	uint8_t i;
	HLRxTxType *p;

	p = &HLRxTx[ch];
	if (p->rx_cnt < POS_LEN)//recv data is less than 3
	{
		p->rx_len = p->frame_length; //length init = 48/32
	}
	
	if (p->rx_cnt == POS_LEN)//
	{//if msg_len is more than cansh size or less  than 8(_min)
		if ((p->rx_data > p->frame_length) || (p->rx_data < FRAME_SIZE_MIN))
		{
			HLRecvErr(ch);
			p->rx_cnt = 0;
			return;
		}
		p->rx_len = p->rx_data;
	}
	p->rx_buf[p->rx_cnt] = p->rx_data;	//
	p->bcc_work ^= p->rx_data;			//bcc
	p->rx_cnt++;						//counter++
//recv data until frame_len=recv_data_length 
	if ((p->rx_cnt < p->rx_len) && (p->rx_cnt < p->frame_length))
	{
		return;
	}
//BCC err
	if (p->bcc_work != 0)
	{
		HLRecvErr(ch);
		p->rx_cnt = 0;
		return;
	}
//app not get last msg ; last msg not handle over
	if(p->rx_exist == HL_ON)
	{
		HLRecvErr(ch);
		p->rx_cnt = 0;
		return;
	}
//
	p->tm_app_err = 0;	//30s err check timer reset
	p->rx_status = RX_EXIST;
	p->rx_exist = HL_ON;
	for(i = 0; i < HL_BUF_SIZE; i++)
	{
		HLRxBuf[ch][i] = p->rx_buf[i];
	}
//check ack
	if(HLCheckAckRequire(ch) == HL_TRUE)//Check if ACK transmission is necessary
	{
		HLSendAckFirstByte(ch);//Send identification code if necessary		
	}
	else
	{
		p->bcc_work	= NULL;
		p->rx_cnt = 0;
		HLSetRcvOK_IR(ch);		//Primary buffer reception permission
	}
}

/***********************************************************************
@name  	: HLSetRcvOK
@brief 	: clear HLRxBuf
@param 	: ch:uint8_t
@return	: none
************************************************************************/
void HLSetRcvOK(uint8_t ch)
{
	uint8_t i;
	HLRxTxType	*p;

	p = &HLRxTx[ch];
	InterruptDisable();
	for ( i=0; i<HL_BUF_SIZE; i++ )
	{				
		HLRxBuf[ch][i] = NULL;
	}
	p->rx_exist = HL_OFF;
	InterruptEnable();

}

/***********************************************************************
@name  	: HLSetAckTable
@brief 	: 
@param 	: opt:0-ACK not send ; 1-ACK send
@return	: none
************************************************************************/
int HLSetAckTable(uint8_t ch,uint8_t id,uint8_t ref_sys,uint8_t addr)
{
	int ret;
	HLRxTxType	*p;
	uint8_t i;

	p = &HLRxTx[ch];
	i = p->ack_entry_cnt;
	if (i >= ACK_SEND_ENTRY_MAX)
	{
		ret = HL_NG;
	}
	else
	{
		p->ack_send_tab[i].id = id;
		p->ack_send_tab[i].ref_sys = ref_sys;
		p->ack_send_tab[i].addr = addr;
		i++;
		p->ack_entry_cnt = i;
		ret = HL_OK;
	}
	return ret;
}

/***********************************************************************
@name  	: HLClearAckTable
@brief 	: 
@param 	: none
@return	: none
************************************************************************/
void HLClearAckTable(uint8_t ch)
{
	HLRxTxType	*p;

	p = &HLRxTx[ch];
	p->ack_entry_cnt = 0;
}

/***********************************************************************
@name  	: HLSetIDTable
@brief 	: Identification code registration (API)
@param 	: 
@return	: 
************************************************************************/
uint8_t HLSetIDTable(uint8_t ch, uint8_t dat)
{
    uint8_t ret;
    HLRxTxType *p;

	p = &HLRxTx[ch];
	if( p->id_entry_cnt >= ID_ENTRY_MAX ) //Full
    {
		ret = HL_NG;
	}
	else
    {
		p->id_table[p->id_entry_cnt] = dat;
		p->id_entry_cnt++;
		ret = HL_OK;
	}
	return (ret);
}

/***********************************************************************
@name  	: HLSendByte
@brief 	: 
@param 	: ch
@return	: none
************************************************************************/
uint8_t HLSendByte(uint8_t ch,uint8_t dat)
{
	uint8_t ret = HL_OK;

	UartTxEnable(ch);
	UartRxEnable(ch);
	UartErrorFlagClear(ch,UART_ERR);
	if (UartIsTxDataEmpty(ch))
	{
		TimerHLClockStop(ch);
		InterruptDisable();
		UartTxDataSet(ch,dat);
		TimerHLClockStart(ch);
		InterruptEnable();
	}
	else
	{
		ret = HL_NG;;
	}
	return ret;
}

/***********************************************************************
@name  	: HLGetByte
@brief 	: 
@param 	: ch
@return	: none
************************************************************************/
int HLGetByte(uint8_t ch,uint8_t *dat)
{
	int ret = 0;

	if (dat != (uint8_t *)NULL)
	{
		if (0) //check uart  //TODO
 		{
			 ret = 1;
			if ((UartErrorFlagGet(ch) & UART_ERR ) != 0)
			{
				ret = -1;
			}
		}
	}
	else
	{
		ret = -2;
	}
	return ret;
}

/***********************************************************************
@name  	: HLClearCarryCount
@brief 	: 
@param 	: ch
@return	: none
************************************************************************/
void HLClearCarryCount(uint8_t ch)
{
	HLRxTxType *p;
	
	p = &HLRxTx[ch];
	p->carry_counter = 0;
}

/***********************************************************************
@name  	: HLGetCarryCount
@brief 	: 
@param 	: ch
@return	: none
************************************************************************/
uint8_t HLGetCarryCount(uint8_t ch)
{
	HLRxTxType *p;
	
	p = &HLRxTx[ch];

	return p->carry_counter;
}

/***********************************************************************
@name  	: HLClearSendStatus
@brief 	: 
@param 	: ch
@return	: none
************************************************************************/
void HLClearSendStatus(uint8_t ch)
{
	HLRxTxType *p;
	
	p = &HLRxTx[ch];
	p->tx_status = HL_OFF;
}

/***********************************************************************
@name  	: HLSetFrameSize
@brief 	: 
@param 	: ch
@return	: none
************************************************************************/
uint8_t HLSetFrameSize(uint8_t ch,uint8_t len)
{
	uint8_t ret = FALSE;

	HLRxTxType *p;
	p = &HLRxTx[ch];
	if ((len >= FRAME_SIZE_MIN ) && (len <= HL_BUF_SIZE))
	{
		p->frame_length = len;
		ret = TRUE;;
	}
	return ret;
}

/***********************************************************************
@name  	: HLSetTxRetryCntMax
@brief 	: 
@param 	: ch
@return	: none
************************************************************************/
void HLSetTxRetryCntMax(uint8_t ch,uint8_t val)
{
	HLRxTxType *p;
	
	p = &HLRxTx[ch];
	p->tx_retry_cnt_max = val;
}

/***********************************************************************
@name  	: HLGetTxRetryCntMax
@brief 	: 
@param 	: ch
@return	: none
************************************************************************/
uint8_t HLGetTxRetryCntMax(uint8_t ch)
{
	HLRxTxType *p;
	
	p = &HLRxTx[ch];
	return p->tx_retry_cnt_max;
}

/***********************************************************************
@name  	: HLSetFrameTable
@brief 	: id_table
@param 	: ch:uint8_t,dat:IdentificationEnum
@return	: none
************************************************************************/
uint8_t HLSetFrameTable(uint8_t ch,uint8_t dat)
{
	uint8_t ret;

	HLRxTxType *p;
	p = &HLRxTx[ch];
	if (p->id_entry_cnt >= ID_ENTRY_MAX)
	{
		ret = FALSE;
	}
	else
	{
		p->id_table[p->id_entry_cnt] = dat;
		p->id_entry_cnt++;
		ret = TRUE;
	}
	return ret;
}

//=========================================   ================================================
/***********************************************************************
@name  	: HLSendEndBroatcast
@brief 	: Broadcast data transmission completion processing
@param 	: ch
@return	: none
************************************************************************/
static void HLSendEndBroatcast(uint8_t ch )
{
	HLRxTxType	*p;

	p = &HLRxTx[ch];
	p->tx_retry_cnt	= 0;	
	p->rx_cnt		= 0;	
	p->tx_cnt		= 0;	
	p->rx_status	= RX_ING;
	p->tx_status	= TX_SUCCESS;	
}

/***********************************************************************
@name  	: HLSendEndNormal
@brief 	: Individual data transmission completion processing
@param 	: ch
@return	: none
************************************************************************/
static void	HLSendEndNormal(uint8_t ch)
{
	HLRxTxType	*p;

	p = &HLRxTx[ch];
	p->tm_tx_retry = 8;			//8ms to wait ack
	p->tm_carry	 = (uint16_t)0;		
	p->rx_cnt	 = (uint8_t)0;		
	p->tx_cnt	 = (uint8_t)0;		
	p->rx_status = RX_ING;			
	p->tx_status = TX_WAIT_ACK;		
}

/***********************************************************************
@name  	: HLTxAfter
@brief 	: send over process
@param 	: ch
@return	: none
@note	: (1) attention about HL_LINK IC reset ON time
			H-LINK / remote control transmission IC reset ON time secures 200nsec or more
			Be careful when changing the process because it is necessary.
			(Minami transmission IC reset ON time specification: 200nsec or more)		 
		  (2) attention about HL_LINK IC reset OFF time
			After the H-LINK / remote control transmission IC reset is turned off, the transmission standby time is set.
			Since it is necessary to secure 500 nsec or more, when changing the process
			Be careful.(Minami transmission IC reset OFF time specification: 500nsec or more)
@note 	: Fixed a bug that the transmission IC reset at the time of a motor transmission collision was twice.
************************************************************************/
static void	HLTxComplete(uint8_t ch)
{
	HLRxTxType	*p;

	p = &HLRxTx[ch];
	TimerHLClockStop(ch);	
	p->tx_complete = HL_OFF;//All data transmission completed flag clear
	p->rx_cnt = 0;			//Receive counter 0 clear
//After sending ACK or After sending AAH
	if( (p->ack_status == ACK_SENDED_06) || (p->col_status == COL_SENDED_AA) )
	{
		if(p->col_status == COL_SENDED_AA )//After sending AAH
		{
			HLinkICReset(ch,ON);			//H-LINK transmission IC reset ON
			UartParityChange(ch,PARITY_EVEN);	//Even Parity Set//0724
			p->tm_carry = ((uint16_t)0x0007 & TimerRandomNoGet()) + 1;// Carrier detection timer 1-8ms sets
			HLDelay10us();
			HLDelay10us();
			HLDelay10us();
			HLDelay10us();
			HLinkICReset(ch,OFF);		//H-LINK transmission IC reset OFF
		}
		p->ack_status = HL_OFF;		//ACK transmission status status clear
		p->col_status = HL_OFF;		//Collision detection status clear
	}
	else
	{
		p->tx_retry_cnt++;
		if(( HLTxBuf[ch][POS_DST_ADDR] == BROADCAST )) //Broadcast data Or if the transmission data is IN_TO_IN
		{
			HLSendEndBroatcast(ch);	//Broadcast data transmission completion processing
		}
		else //If not broadcast
		{
			HLSendEndNormal(ch);	//Individual data transmission completion processing
		}
	}
	HLReadRDR(ch);
	UartErrorFlagClear(ch,UART_ERR);
	UartRxEnable(ch);
	UartINTSRxEnable(ch);
}

/***********************************************************************
@name  	: HLSndAck2
@brief 	: ACK2 byte transmission processing
@param 	: ch
@return	: none
************************************************************************/
static void HLSenddAck2(uint8_t ch)
{
	HLRxTxType	*p;

	p = &HLRxTx[ch];
	UartParityChange(ch,PARITY_EVEN);//0724
	TimerHLClockStop(ch);
	UartTxDataSet(ch, ACK);
	TimerHLClockStart(ch);
	p->ack_status	= ACK_SENDED_06;	//Set after ACK transmission
	p->tx_complete	= HL_ON;			//All data transmission completed flag set
	HLSetRcvOK_IR(ch);					//Primary buffer reception permission

	UartINTSRxDisable(ch);
	UartINTSRxDisable(ch);
	HLReadRDR(ch);						
}

/***********************************************************************
@name  	: HLSndDat
@brief 	: Transmission processing after the second byte
@param 	: ch
@return	: none
************************************************************************/
static void	HLSendDat(uint8_t ch)
{
	HLRxTxType	*p;
	uint8_t	dat;

	p = &HLRxTx[ch];
	UartParityChange(ch,PARITY_EVEN);
	TimerHLClockStop(ch);
	dat = HLTxBuf[ch][p->tx_cnt];//Data 1 byte set
	UartTxDataSet(ch,dat);		//UART transmit data set
	TimerHLClockStart(ch);
	p->tx_cnt++;					
	if( p->tx_cnt < p->tx_len )	//If it has not been sent for the length
	{					
		return;		//end
	}
	p->tx_complete = HL_ON;		//All data transmission completed flag set
	UartRxDisable(ch);
	UartINTSRxDisable(ch);		//UART receive interrupt disabled
	HLReadRDR(ch);					
}

/***********************************************************************
@name  	: HLSndDat1
@brief 	: 1st byte transmission process
@param 	: ch
@return	: TRUE:send success  ; FALSE: send fail
************************************************************************/
void HLSendFirstDat(uint8_t ch)
{
	HLRxTxType	*p;

	p = &HLRxTx[ch];
	if( !UartIsTxDataEmpty(ch))//If transmission cannot be started
	{
		return;
	}
	UartRxEnable(ch);
	UartINTSRxDisable(ch);
	UartParityChange(ch,PARITY_ODD);
	p->tx_complete = HL_OFF;
	TimerHLClockStop(ch);
	UartTxDataSet(ch,HLTxBuf[ch][POS_ID]);
	TimerHLClockStart(ch);
	p->tx_cnt++;
	UartSendEndIntEnable(ch);
	p->tx_status = TX_ING;			//Sending set
	p->tx_len	 = HLTxBuf[ch][POS_LEN];	//Length set
	p->rx_cnt = 0;		//Receive counter 0 clear
	
	UartINTSRxEnable(ch);
	UartRxEnable(ch);
	HLReadRDR(ch);

}

/***********************************************************************
@name  	: HLSend
@brief 	: calculate bcc value,and change tx_status
@param 	: ch
@return	: none
************************************************************************/
uint8_t HLSend(uint8_t ch)
{
	uint8_t ret = FALSE;
	uint8_t i,bcc,len;
	HLRxTxType	*p;

	p = &HLRxTx[ch];
	len = HLTxBuf[ch][POS_LEN];
	if ((len > 48) || (len<8))
	{
		p->tx_status = TX_FAIL;
	}
	else 
	{
		bcc = 0;
		len--;
		for (i = 1; i < len; i++)
		{
			bcc ^= HLTxBuf[ch][i];
		}
		HLTxBuf[ch][len] = bcc;

		p->tx_cnt = 0;
		p->tx_status = TX_REQ;
		ret = TRUE;
	}
	
	return ret;
}

/***********************************************************************
@name  	: HLSendFirstByte
@brief 	: 1st byte transmission process
@param 	: ch
@return	: none
************************************************************************/
void HLSendFirstByte(uint8_t ch)
{
	HLRxTxType *p;

	p = &HLRxTx[ch];
	if( (p->tx_status & TX_REQ) != HL_OFF )
	{
		if( p->tm_carry == (uint16_t)0) 	//If you don't have a carrier
		{
			InterruptDisable();
			p->tx_retry_cnt = 0;//
			HLSendFirstDat(ch);	//1st byte send
			InterruptEnable();
		}
	}
}

/***********************************************************************
@name  	: HLCheckRecvData
@brief 	: check data if is recv ok
@param 	: ch
@return	: none
************************************************************************/
uint8_t HLCheckRecvData(uint8_t ch) //need check cache works
{
	uint8_t ret = FALSE;
	uint8_t len,i;
	uint8_t bcc = 0;
	HLRxTxType *p;
	p = &HLRxTx[ch];

	if (p->rx_exist != HL_ON)
	{
		return ret;	
	}		
	len = HLRxBuf[ch][2];
	for (i = 1; i < len; i++)
	{			
		bcc ^= HLRxBuf[ch][i];
	}
	if (bcc != 0)
	{
		return ret;
	}
	ret = TRUE;

	return (ret);
}

/***********************************************************************
@name  	: HLSendDataSet
@brief 	: calculate bcc value,and change tx_status
@param 	: ch
@return	: none
************************************************************************/
static uint8_t HLSendDataSet(uint8_t ch)
{
	uint8_t ret = FALSE;
	uint8_t i,bcc,len;
	HLRxTxType	*p;

	p = &HLRxTx[ch];
	len = HLTxBuf[ch][POS_LEN];
	if ((len > p->frame_length) || (len < FRAME_SIZE_MIN))
	{
		p->tx_status = TX_FAIL;
	}
	else 
	{
		bcc = 0;
		len--;
		for (i = 1; i < len; i++)
		{
			bcc ^= HLTxBuf[ch][i];
		}
		HLTxBuf[ch][len] = bcc;

		p->tx_cnt = 0;
		p->tx_status = TX_REQ;
		ret = TRUE;
	}
	
	return ret;
}

/***********************************************************************
@name  	: HLSendRetry
@brief 	: 
@param 	: ch
@return	: none
@note	: when tx_status=TX_FAIL(send twice ,then wait 100ms),tx_retry_10s_cnt++,
************************************************************************/
void HLSendRetry(uint8_t ch)	
{
	// HLRxTxType	*p;

	// p = &HLRxTx[ch];
// 	p->tx_retry_10s_cnt++;
// 	if (p->tx_retry_10s_cnt > 40) //if (p->tx_retry_10s_cnt > 83) //retry time keep 10s,  => 8ms+8ms+100ms+(55ms(48byte/9600))=161ms(116ms) 10s/116ms
// 	{
// 		p->tx_retry_10s_cnt = 0;
// 		//p->tx_status = TX_SUCCESS;
// 		p->tx_status = HL_OFF;

// //when do not receive ack after 10s,as disconnected
// 		if (ch == HL_CH_CHILLER)
// 		{
// 			channel = HLTxBuf[ch][3];
// 		}
// 		else if (ch == HL_CH_CC)
// 		{
// 			channel = HLTxBuf[ch][6];
// 			if (channel == 0x01) //primary wrc
// 			{

// 			}
// 			else if (channel == 0x02) //secondary wrc
// 			{

// 			}
// 		}
// 		return;
// 	}

	// p->tm_carry = 100;	//wait 100ms to retry
	// p->tx_status = TX_WAIT_ACK;	//

}

/***********************************************************************
@name  	: HLCheckTxStatus
@brief 	: 
@param 	: ch
@return	: none
@note	: none
************************************************************************/
uint8_t HLCheckTxStatus(uint8_t ch)
{
	uint8_t ret;
	HLRxTxType	*p;

	p = &HLRxTx[ch];
// check tx_staus
	if ((p->tx_status & TX_FAIL) != HL_OFF)//tx fail
	{
		ret = HL_FAIL;
	}
	else if ((p->tx_status &(TX_REQ + TX_WAIT_ACK + TX_ING + TX_RETRY_REQ)) != HL_OFF)//
	{
		ret = HL_CONTINUE;
	}
	else
	{
		ret = HL_SUCCESS;
	}

	return ret;
}

/***********************************************************************
@name  	: HLinkInit
@brief 	: id_table
@param 	: ch:uint8_t,dat:IdentificationEnum
@return	: none
************************************************************************/
void HLinkInit(uint8_t ch,uint8_t self_state)
{
	uint8_t i;
	HLRxTxType	*p;

	p = &HLRxTx[ch];
	p->rx_status = RX_ING;
	p->tx_status = HL_OFF;
	p->ack_status = HL_OFF;
	p->col_status = HL_OFF;
	p->ack_option = 0;	//N/A
	p->rx_cnt = 0;
	p->tx_cnt = 0;
	p->rx_exist = HL_OFF;
	p->ack_option = HL_OFF;
	p->frame_length = HL_BUF_SIZE;
	p->tx_retry_cnt_max = SEND_RETRY_MAX;

	for ( i = 0; i < ACK_SEND_ENTRY_MAX; i++)
	{
		p->ack_send_tab[i].id = NULL;
		p->ack_send_tab[i].ref_sys = NULL;
		p->ack_send_tab[i].addr = NULL;
	}
	p->ack_entry_cnt = 0;

	for ( i = 0; i < ID_ENTRY_MAX; i++)
	{
		p->id_table[i] = NULL;
	}
	p->id_entry_cnt = 0;
	p->mode = HL_OFF;
	if (self_state == ON)
	{
		p->mode = SELF_TEST;
	}
//uart init
	HLinkICReset(ch,ON);
	UartStart(ch);
	UartRxDisable(ch);
	UartTxDisable(ch);
	UartINTSRxDisable(ch);
	UartINTSTxDisable(ch);
	UartSendEndIntDisable(ch);
	UartParityEnable(ch);
	UartParityChange(ch,PARITY_EVEN);
	UartINTSRxEnable(ch);
	UartRxEnable(ch);
	UartTxEnable(ch);
	HLReadRDR(ch);
	// TimerHLClockStop(ch);
	HLinkICReset(ch,OFF);

	for ( i = 0; i < 48; i++)
	{
		HLTxBuf[ch][i] = 0;
	}
	// TimerHLClockStart(ch);
	// if (self_state == ON)
	// {
	// 	TimerHLClockStart(ch);
	// }
}

/***********************************************************************
@name  	: HLCollision
@brief 	: Collision processing
@param 	: ch
@return	: none
************************************************************************/
static void HLCollision(uint8_t ch)
{
	HLRxTxType	*p;

	p = &HLRxTx[ch];
	UartParityChange(ch,PARITY_ODD);//0724
	TimerHLClockStop(ch);
	UartTxDataSet(ch,0xAA);		//UART transmit data set
	TimerHLClockStart(ch);
	p->tx_complete = HL_ON;			//All data transmission completion flag set
	UartRxDisable(ch);
	UartINTSRxDisable(ch);		//
	HLReadRDR(ch);					//Clear error flag, read received data
	p->col_status = COL_SENDED_AA;	//Set after sending AAh
	p->tx_status  = TX_RETRY_REQ;	//Send request flag set
	p->tx_cnt	  = (uint8_t)0;		//Transmission counter reset
}


/***********************************************************************
@name  	: HLUpdateTM
@brief 	: Timer update
@param 	: None
@return	: None
************************************************************************/
static void HLUpdateTM(uint8_t ch)
{
	HLRxTxType	*p;

	p = &HLRxTx[ch];
	if(p->tm_carry > 0)//1ms down timer count
    { 
        p->tm_carry--; //Carrier detection timer
    }
	if( p->tm_tx_retry > 0) //Resend wait timer
    { 
        p->tm_tx_retry--;     
    }
	if(p->tm_1ms_free > 0)//General-purpose timer
    { 
        p->tm_1ms_free--;
    }
    p->tm_1ms++;	//1ms up timer count
	if( p->tm_1ms >= 100 )//100ms up timer count
    {
		p->tm_1ms = 0;
		p->tm_100ms++;		//100ms timer
		if( p->tm_100ms >= 10 )//1s up timer count
        {
			p->tm_100ms = (uint8_t)0;
			if( p->tm_app_err < (uint8_t)0xff )
            {
				p->tm_app_err++;
			}
		}
	}
}

/***********************************************************************
@name  	: HLAPErr30s
@brief 	: Application software delivery error processing
@param 	: None
@return	: None
************************************************************************/
static void HLAPErr30s(uint8_t ch)
{
	HLRxTxType	*p;

	p = &HLRxTx[ch];
	if(p->rx_exist != HL_OFF)//Is there a delivery message on the application side?
    {
		if(p->tm_app_err >= 30) //30 seconds have passed
        {
			p->rx_status = RX_ING;	//Receiving set (delivery message discarded)
			p->rx_exist	= HL_OFF;
		}
	}
}

/***********************************************************************
@name  	: HLRcvErrChk
@brief 	: Application software delivery error processing
@param 	: None
@return	: None
************************************************************************/
static void HLRcvErrChk(uint8_t ch)
{
	HLRxTxType	*p;

	p = &HLRxTx[ch];
	if ( (p->rx_status & RX_ERR) != HL_OFF )//Receive error
    {
		p->rx_cnt = 0;		
		p->rx_status = RX_ING;			
		HLReadRDR(ch);
		UartErrorFlagClear(ch,UART_ERR);				
		UartINTSRxEnable(ch);
	}
}

/***********************************************************************
@name  	: HLCarryInt
@brief 	: handle data received from serial
@param 	: ch
@return	: none
@note 	: when receive data,set t_carry=10~18ms, to avoid bus collision
		  until t_carry =0,then can send data
************************************************************************/
void HLCarryInt(uint8_t ch)
{
	HLRxTxType *p;
	p = &HLRxTx[ch];

	// p->tm_carry = (TimerRandomNoGet() & 0x0007) + 10;	//Carrier detection timer 10  //last 10~18ms
	p->tm_carry = (TimerRandomNoGet() & 0x000F) + 30; //16+10
	p->carry_counter ++;
}

/***********************************************************************
@name  	: HLInt1ms
@brief 	: Functions called from within a 1ms interrupt
        Counter measures for problems that cause wait timer clear when a carrier is detected during a transmission wait
@param 	: None
@return	: None
************************************************************************/
void HLInt1ms(uint8_t ch)//TODO 
{
	HLRxTxType	*p;

	p = &HLRxTx[ch];
	HLUpdateTM(ch);		//1ms timer update
	HLAPErr30s(ch);		//If the AP does not read for 30 seconds, discard the data
    HLRcvErrChk(ch);

	if((p->tx_status & TX_WAIT_ACK) != HL_OFF)//Waiting for ACK
    {// ignore ack request for now
		if( p->tm_tx_retry == (uint8_t)0)//After 8ms
        {
			if( p->tx_retry_cnt < p->tx_retry_cnt_max )//Retransmission count judgment
            {
				if(p->tm_carry == (uint16_t)0)//If it is less than the specified number of resends
                {			
					HLSendFirstDat(ch);//If carrier is not detected
				}
			}
			else//If it is more than the specified number of resends
            {
				p->tx_status    = TX_FAIL;		//send fail
				p->tx_retry_cnt	= (uint8_t)0;	//
				p->tm_carry = 200;				//200ms
			}
		}
	}
	else//Not waiting for ACK   //NCK 
    {
		if((p->tx_status & TX_RETRY_REQ) != HL_OFF)//If there is a transmission request
        {
			if( p->tm_carry == (uint16_t)0)//Without a carrier
			{
				if( p->tx_retry_cnt < p->tx_retry_cnt_max )//If it is less than the specified number of resends
				{
					// p->tx_retry_cnt = 0;
					HLSendFirstDat(ch);		//Send 1st byte
				}
				else //If it is more than the specified number of resends
				{
					p->tx_status	= TX_FAIL;	
					p->tx_retry_cnt	= (uint8_t)0;	
				}
			}
		}
	}
}

/***********************************************************************
@name  	: HLIntReceive
@brief 	: handle data received from serial
@param 	: ch
@return	: none
************************************************************************/
void HLIntReceive(uint8_t ch)
{
	HLRxTxType *p;
	uint8_t err_type;
	p = &HLRxTx[ch];
	
	err_type = UartErrorFlagGet(ch);	//
	UartErrorFlagClear(ch,err_type);	//
	p->rx_data = UartRxDataGet(ch); 	//data recv
	
	if ((p->tx_status & TX_ING) != HL_OFF)//if data is sending
	{
		HLCheckSelfSendData(ch,err_type);//check data send by self
	}
	else
	{
		if ((p->rx_status & RX_ING) == HL_OFF)//recv disable
		{
			UartRxDisable(ch);
			UartINTSRxDisable(ch);//
		}
		else//data is receiving
		{
			if (p->rx_cnt == 0)//recv first byte
			{
				HLRecvFirstByte(ch,err_type);
			}
			else//second and after
			{
				if (err_type != 0)//err
				{
					HLRecvErr(ch);
				}
				else//no err
				{
					if ((p->rx_cnt == 1) && (p->rx_data == ACK))//if recv ACK
					{
						HLRecvACK(ch); // 
					}
					else
					{	
						HLRecvData(ch);
					}
				}
			}
		}
	}
}

/***********************************************************************
@name  	: HLIntTransmit
@brief 	: 
@param 	: ch
@return	: none
************************************************************************/
void HLIntTransmit(uint8_t ch)
{
	HLRxTxType	*p;
	p = &HLRxTx[ch];

	if( p->tx_complete == HL_ON )//Completed all data transmission
	{
		UartSendEndIntDisable(ch);
		HLTxComplete(ch);	//Transmission completion process
	}
	else if( (p->ack_status & ACK_SNDING) != HL_OFF )//ACK sending
	{
		HLSenddAck2(ch);	//2nd byte = 06h transmission
	}
	else if( (p->tx_status & TX_ING) != HL_OFF )//Sending a telegram
	{
		if( (p->col_status & COL_EXIST) != HL_OFF )//Is there a collision
		{
			HLCollision(ch);	//Collision occurrence processing (AAh transmission)
		}
		else//When there is no collision
		{
			HLSendDat(ch);//Transmission processing after the second byte
		}
	}
	else
	{	
		UartSendEndIntDisable(ch);
	}
}

/***********************************************************************
@name  	: HLStartTransmit
@brief 	: start to Transmit tx_buff
@param 	: ch
@return	: none
@note	: first data set ,to get in tx_interrupt
************************************************************************/
void HLStartTransmit(uint8_t ch)
{
	HLSendDataSet(ch); //set bcc and set tx_status=TX_REQ
}
