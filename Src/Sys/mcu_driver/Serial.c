/***********************************************************************
@file   : Serial.c
@brief  : 
@Device : R5F5651EHGFP
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "r_cg_sci.h"
#include "Serial.h"
#include "Timer.h"



/************************************************************************
@name  	: UartErrorFlagGet
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
uint8_t UartErrorFlagGet(uint8_t ch)
{
    uint8_t res = 0;

    switch (ch)
    {
        case UART0: res = (SCI0.SSR.BYTE & UART_ERR);   break; 
        case UART3: res = (SCI3.SSR.BYTE & UART_ERR);   break;
        case UART1: res = (SCI1.SSR.BYTE & UART_ERR);   break;
        case UART2: res = (SCI2.SSR.BYTE & UART_ERR);   break;
        case UART5: res = (SCI5.SSR.BYTE & UART_ERR);   break;
        case UART6: res = (SCI6.SSR.BYTE & UART_ERR);   break;
        case UART8: res = (SCI8.SSR.BYTE & UART_ERR);   break;
        case UART9: res = (SCI9.SSR.BYTE & UART_ERR);   break;
        default:    break;
    }

    return (res);
}

/************************************************************************
@name  	: UartErrorFlagClear
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void UartErrorFlagClear(uint8_t ch,uint8_t dat)
{
    switch (ch)
    {
        case UART0: SCI0.SSR.BYTE &= ~UART_ERR;    break; 
        case UART3: SCI3.SSR.BYTE &= ~UART_ERR;    break;
        case UART1: SCI1.SSR.BYTE &= ~UART_ERR;    break; 
        case UART2: SCI2.SSR.BYTE &= ~UART_ERR;    break;
        case UART5: SCI5.SSR.BYTE &= ~UART_ERR;    break; 
        case UART6: SCI6.SSR.BYTE &= ~UART_ERR;    break;
        case UART8: SCI8.SSR.BYTE &= ~UART_ERR;    break; 
        case UART9: SCI9.SSR.BYTE &= ~UART_ERR;    break;
        default:    break;
    }
}

/************************************************************************
@name  	: UartRxDataGet
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
uint8_t UartRxDataGet(uint8_t ch)
{
    uint8_t res;

    switch (ch)
    {
        case UART0: res = SCI0.RDR;    break;
        case UART3: res = SCI3.RDR;    break;
        case UART1: res = SCI1.RDR;    break;
        case UART2: res = SCI2.RDR;    break;
        case UART5: res = SCI5.RDR;    break;
        case UART6: res = SCI6.RDR;    break;
        case UART8: res = SCI8.RDR;    break;
        case UART9: res = SCI9.RDR;    break;
        default:    break;
    }
    return (res);
}

/***********************************************************************
@name  	: UartINTSRxDisable
@brief 	: 
@param 	: None
@return	: None
************************************************************************/
void UartINTSRxDisable(uint8_t ch)
{
    switch (ch)
    {
        case UART0: SCI0.SCR.BIT.RIE = 0U; break; 
        case UART3: SCI3.SCR.BIT.RIE = 0U; break;
        case UART1: SCI1.SCR.BIT.RIE = 0U; break; 
        case UART2: SCI2.SCR.BIT.RIE = 0U; break;
        case UART5: SCI5.SCR.BIT.RIE = 0U; break; 
        case UART6: SCI6.SCR.BIT.RIE = 0U; break;
        case UART8: SCI8.SCR.BIT.RIE = 0U; break; 
        case UART9: SCI9.SCR.BIT.RIE = 0U; break;
        default:    break;
    }
}

/***********************************************************************
@name  	: UartINTSRxEnable
@brief 	: 
@param 	: None
@return	: None
************************************************************************/
void UartINTSRxEnable(uint8_t ch)
{
    switch (ch)
    {
        case UART0: SCI0.SCR.BIT.RIE = 1U; break;
        case UART3: SCI3.SCR.BIT.RIE = 1U; break;
        case UART1: SCI1.SCR.BIT.RIE = 1U; break;
        case UART2: SCI2.SCR.BIT.RIE = 1U; break;
        case UART5: SCI5.SCR.BIT.RIE = 1U; break;
        case UART6: SCI6.SCR.BIT.RIE = 1U; break;
        case UART8: SCI8.SCR.BIT.RIE = 1U; break;
        case UART9: SCI9.SCR.BIT.RIE = 1U; break;
        default:    break;
    }
}

/***********************************************************************
@name  	: SfrUartRxDisable
@brief 	: 
@param 	: None
@return	: None
************************************************************************/
void UartRxDisable(uint8_t ch)
{
    switch (ch)
    {
        case UART0: SCI0.SCR.BIT.RE = 0;   break; 
        case UART3: SCI3.SCR.BIT.RE = 0;   break;
        case UART1: SCI1.SCR.BIT.RE = 0;   break; 
        case UART2: SCI2.SCR.BIT.RE = 0;   break;
        case UART5: SCI5.SCR.BIT.RE = 0;   break; 
        case UART6: SCI6.SCR.BIT.RE = 0;   break;
        case UART8: SCI8.SCR.BIT.RE = 0;   break; 
        case UART9: SCI9.SCR.BIT.RE = 0;   break;
        default:    break;
    }
}

/***********************************************************************
@name  	: UartRxEnable
@brief 	: 
@param 	: None
@return	: None
************************************************************************/
void UartRxEnable(uint8_t ch)
{
    switch (ch)
    {
        case UART0:  SCI0.SCR.BIT.RE = 1; break;
        case UART3:  SCI3.SCR.BIT.RE = 1; break;
        case UART1:  SCI1.SCR.BIT.RE = 1; break;
        case UART2:  SCI2.SCR.BIT.RE = 1; break;
        case UART5:  SCI5.SCR.BIT.RE = 1; break;
        case UART6:  SCI6.SCR.BIT.RE = 1; break;
        case UART8:  SCI8.SCR.BIT.RE = 1; break;
        case UART9:  SCI9.SCR.BIT.RE = 1; break;
        default:    break;
    }
}

/***********************************************************************
@name  	: UartIsRxEnable
@brief 	: 
@param 	: None
@return	: None
************************************************************************/
uint8_t UartIsRxEnable(uint8_t ch)
{
	uint8_t	ret = FALSE;		//Return value: Receive prohibited
	
    switch (ch)
    {
        case UART0: ret = SCI0.SCR.BIT.RE; break; 
        case UART3: ret = SCI3.SCR.BIT.RE; break;
        case UART1: ret = SCI1.SCR.BIT.RE; break; 
        case UART2: ret = SCI2.SCR.BIT.RE; break;
        case UART5: ret = SCI5.SCR.BIT.RE; break; 
        case UART6: ret = SCI6.SCR.BIT.RE; break;
        case UART8: ret = SCI8.SCR.BIT.RE; break; 
        case UART9: ret = SCI9.SCR.BIT.RE; break;
        default:    break;
    }
	return(ret);
}

/***********************************************************************
@name  	: UartIsTxEnable
@brief 	: 
@param 	: None
@return	: None
@note	: thrm uart change 
************************************************************************/
uint8_t UartIsTxEnable(uint8_t ch)
{
	uint8_t	ret = FALSE;
	
    switch (ch)
    {
        case UART0: ret = SCI0.SCR.BIT.TE; break;
        case UART3: ret = SCI3.SCR.BIT.TE; break;
        case UART1: ret = SCI1.SCR.BIT.TE; break; 
        case UART2: ret = SCI2.SCR.BIT.TE; break;
        case UART5: ret = SCI5.SCR.BIT.TE; break; 
        case UART6: ret = SCI6.SCR.BIT.TE; break;
        case UART8: ret = SCI8.SCR.BIT.TE; break; 
        case UART9: ret = SCI9.SCR.BIT.TE; break;
        default:    break;
    }
	return(ret);	
}

/***********************************************************************
@name  	: UartTxDisable
@brief 	: 
@param 	: None
@return	: None
************************************************************************/
void UartTxDisable(uint8_t ch)
{
    switch (ch)
    {
        case UART0: SCI0.SCR.BIT.TE = 0; break;
        case UART3: SCI3.SCR.BIT.TE = 0; break;
        case UART1: SCI1.SCR.BIT.TE = 0; break;
        case UART2: SCI2.SCR.BIT.TE = 0; break;
        case UART5: SCI5.SCR.BIT.TE = 0; break;
        case UART6: SCI6.SCR.BIT.TE = 0; break;
        case UART8: SCI8.SCR.BIT.TE = 0; break;
        case UART9: SCI9.SCR.BIT.TE = 0; break;
        default:    break;
    }    
}

/***********************************************************************
@name  	: UartTxEnable
@brief 	: 
@param 	: 
@return	: None
************************************************************************/
void UartTxEnable(uint8_t ch)
{
    switch (ch)
    {
        case UART0: SCI0.SCR.BIT.TE = 1; break;
        case UART3: SCI3.SCR.BIT.TE = 1; break;
        case UART1: SCI1.SCR.BIT.TE = 1; break;
        case UART2: SCI2.SCR.BIT.TE = 1; break;
        case UART5: SCI5.SCR.BIT.TE = 1; break;
        case UART6: SCI6.SCR.BIT.TE = 1; break;
        case UART8: SCI8.SCR.BIT.TE = 1; break;
        case UART9: SCI9.SCR.BIT.TE = 1; break;
        default:    break;
    }    
}

/***********************************************************************
@name  	: UartTxEnable
@brief 	: 
@param 	: 
@return	: None
************************************************************************/
void UartParityEnable(uint8_t ch)
{
    switch (ch)
    {
        case UART0: SCI0.SMR.BIT.PE = 1; break;
        case UART3: SCI3.SMR.BIT.PE = 1; break;
        case UART1: SCI1.SMR.BIT.PE = 1; break;
        case UART2: SCI2.SMR.BIT.PE = 1; break;
        case UART5: SCI5.SMR.BIT.PE = 1; break;
        case UART6: SCI6.SMR.BIT.PE = 1; break;
        case UART8: SCI8.SMR.BIT.PE = 1; break;
        case UART9: SCI9.SMR.BIT.PE = 1; break;
        default:    break;
    }    
}

/***********************************************************************
@name  	: UartParityChange
@brief 	: 
@param 	: None
@return	: None
************************************************************************/
void UartParityChange(uint8_t ch,uint8_t parity)
{
    uint8_t smr_te,smr_re;

    switch (ch)
    {
        case UART0: 
        {
            smr_te = SCI0.SCR.BIT.TE;
            smr_re = SCI0.SCR.BIT.RE;
            SCI0.SCR.BIT.TE = 0;
            SCI0.SCR.BIT.RE = 0;
            SCI0.SMR.BIT.PM = parity;
            SCI0.SCR.BIT.TE = smr_te;
            SCI0.SCR.BIT.RE = smr_re;
        }
        break;
        case UART3: 
        {
            smr_te = SCI3.SCR.BIT.TE;
            smr_re = SCI3.SCR.BIT.RE;
            SCI3.SCR.BIT.TE = 0;
            SCI3.SCR.BIT.RE = 0;
            SCI3.SMR.BIT.PM = parity;
            SCI3.SCR.BIT.TE = smr_te;
            SCI3.SCR.BIT.RE = smr_re;       
        }
        break;
        default:    break;
    }
}

/***********************************************************************
@name  	: UartSendEndIntEnable
@brief 	: 
@param 	: None
@return	: None
************************************************************************/
void UartSendEndIntEnable(uint8_t ch)
{	
    switch (ch)
    {
        case UART0: SCI0.SCR.BIT.TEIE = 1; break;
        case UART3: SCI3.SCR.BIT.TEIE = 1; break;
        case UART1: SCI1.SCR.BIT.TEIE = 1; break;
        case UART2: SCI2.SCR.BIT.TEIE = 1; break;
        case UART5: SCI5.SCR.BIT.TEIE = 1; break;
        case UART6: SCI6.SCR.BIT.TEIE = 1; break;
        case UART8: SCI8.SCR.BIT.TEIE = 1; break;
        case UART9: SCI9.SCR.BIT.TEIE = 1; break;
        default:    break;
    }    											
}

/***********************************************************************
@name  	: UartSendEndIntDisable
@brief 	: 
@param 	: None
@return	: None
************************************************************************/
void UartSendEndIntDisable(uint8_t ch)
{	
    switch (ch)
    {
        case UART0: SCI0.SCR.BIT.TEIE = 0; break;
        case UART3: SCI3.SCR.BIT.TEIE = 0; break;
        case UART1: SCI1.SCR.BIT.TEIE = 0; break;
        case UART2: SCI2.SCR.BIT.TEIE = 0; break;
        case UART5: SCI5.SCR.BIT.TEIE = 0; break;
        case UART6: SCI6.SCR.BIT.TEIE = 0; break;
        case UART8: SCI8.SCR.BIT.TEIE = 0; break;
        case UART9: SCI9.SCR.BIT.TEIE = 0; break;
        default:    break;
    }    											
}

/***********************************************************************
@name  	: UartINTSTxDisable
@brief 	: 
@param 	: None
@return	: None
************************************************************************/
void UartINTSTxDisable(uint8_t ch)
{
    switch (ch)
    {
        case UART0: SCI0.SCR.BIT.TIE = 0; break; 
        case UART3: SCI3.SCR.BIT.TIE = 0; break;
        case UART1: SCI1.SCR.BIT.TIE = 0; break; 
        case UART2: SCI2.SCR.BIT.TIE = 0; break;
        case UART5: SCI5.SCR.BIT.TIE = 0; break; 
        case UART6: SCI6.SCR.BIT.TIE = 0; break;
        case UART8: SCI8.SCR.BIT.TIE = 0; break; 
        case UART9: SCI9.SCR.BIT.TIE = 0; break;
        default:    break;
    }    
}

/***********************************************************************
@name  	: UartINTSTxEnable
@brief 	: 
@param 	: None
@return	: None
************************************************************************/
void UartINTSTxEnable(uint8_t ch)
{
    switch (ch)
    {
        case UART0: SCI0.SCR.BIT.TIE = 1; break;
        case UART3: SCI3.SCR.BIT.TIE = 1; break;
        case UART1: SCI1.SCR.BIT.TIE = 1; break;
        case UART2: SCI2.SCR.BIT.TIE = 1; break;
        case UART5: SCI5.SCR.BIT.TIE = 1; break;
        case UART6: SCI6.SCR.BIT.TIE = 1; break;
        case UART8: SCI8.SCR.BIT.TIE = 1; break;
        case UART9: SCI9.SCR.BIT.TIE = 1; break;
        default:    break;
    }    
}

/***********************************************************************
@name  	: UartIsTxDataEmpty
@brief 	: 
@param 	: None
@return	: None
************************************************************************/
uint8_t UartIsTxDataEmpty(uint8_t ch)
{
	uint8_t	ret = FALSE;

    switch (ch)
    {
        case UART0: ret = SCI0.SSR.BIT.TDRE; break;
        case UART3: ret = SCI3.SSR.BIT.TDRE; break;
        case UART1: ret = SCI1.SSR.BIT.TDRE; break;
        case UART2: ret = SCI2.SSR.BIT.TDRE; break;
        case UART5: ret = SCI5.SSR.BIT.TDRE; break;
        case UART6: ret = SCI6.SSR.BIT.TDRE; break;
        case UART8: ret = SCI8.SSR.BIT.TDRE; break;
        case UART9: ret = SCI9.SSR.BIT.TDRE; break;
        default:    break;
    }	

	return(ret);	//
}

/***********************************************************************
@name  	: SfrUartTxDataSet
@brief 	: 
@param 	: None
@return	: None
************************************************************************/
void UartTxDataSet(uint8_t ch,uint8_t data)
{
    switch (ch)
    {
        case UART0:  SCI0.TDR = data; break;
        case UART3:  SCI3.TDR = data; break;
        case UART1:  SCI1.TDR = data; break;
        case UART2:  SCI2.TDR = data; break;
        case UART5:  SCI5.TDR = data; break;
        case UART6:  SCI6.TDR = data; break;
        case UART8:  SCI8.TDR = data; break;
        case UART9:  SCI9.TDR = data; break;
        default:    break;
    }
}

/***********************************************************************
@name  	: UartTxClockStop
@brief 	: 
@param 	: None
@return	: None
************************************************************************/
void UartTxClockStop(uint8_t ch) // timer pwm need to change
{
    TimerHLClockStop(ch);
}

/***********************************************************************
@name  	: UartStop
@brief 	: 
@param 	: ch: CH_SUBBOARD;HL_CH_CHILLER ;HL_CH_CC
@return	: None
@note	: thrm uart change 
************************************************************************/
void UartStop(uint8_t ch)
{
    switch (ch)
    {
        case UART0:   R_SCI0_Stop();    break;  
        case UART3:   R_SCI3_Stop();    break;
        // case UART1:   R_SCI1_Stop();    break;
        // case UART2:   R_SCI2_Stop();    break;
        // case UART5:   R_SCI5_Stop();    break;
        // case UART6:   R_SCI6_Stop();    break;
        // case UART8:   R_SCI8_Stop();    break;
        // case UART9:   R_SCI9_Stop();    break;
        default:    break;
    }    
}

/***********************************************************************
@name  	: UartStart
@brief 	: 
@param 	: None
@return	: None
************************************************************************/
void UartStart(uint8_t ch)
{
    switch (ch)
    {
        case UART0:   R_SCI0_Start();    break;  
        case UART3:   R_SCI3_Start();    break;
        // case UART1:   R_SCI1_Start();    break;
        // case UART2:   R_SCI2_Start();    break;
        // case UART5:   R_SCI5_Start();    break;
        // case UART6:   R_SCI6_Start();    break;
        // case UART8:   R_SCI8_Start();    break;
        // case UART9:   R_SCI9_Start();    break;
        default:    break;
    }
}












