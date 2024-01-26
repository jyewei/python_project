#ifndef __SERIAL_H
#define __SERIAL_H
#include "iodefine.h"
#include "Common.h"
/***********************************************************
硬件连接说明
Hlink	
    1.chiller link  
        UART0
        	RXD0	P33
            INT	    P05/IRQ13
            TXD0	P32
            CLK	    P21/MTIOC1B
            RST	    PJ3
    2.centre controller
        UART3
        	RXD3	P25
            INT	    PA1/IRQ11
            TXD3	P23
            CLK	    P24/MTIOC4A
            RST	    PE0
    3.Service link
        UART1	
            RXD1	P31
            TXD1	P26

RS485	
    1.
        UART8
        	RXD8	PC6
            TXD8	PC7
            CTRL	P22
    2.
        UART2
            TXD2	P50
            RXD2	P52
            CTRL	P55
    3.
        UART6
            RXD6	PB0
            TXD6	PB1
            CTRL	P54
    4.
        UART5
            TXD5	PC3
            RXD5	PC2
            CTRL	P53
    5.        
        UART9
            RXD9	PB6
            TXD9	PB7
            CTRL	PC4
************************************************************/
//
#define UART_ERR_PE ((uint8_t)0x08) //bit3 parity error
#define UART_ERR_FE ((uint8_t)0x10) //bit4 frame err
#define UART_ERR_OE ((uint8_t)0x20) //bit5 io error
#define UART_ERR    ((UART_ERR_PE |UART_ERR_FE |UART_ERR_OE))

#define PARITY_ODD 1
#define PARITY_EVEN 0

//uart 
typedef enum
{
    UART0,
    UART3,  //hlink channel

    UART1,
    UART2,
    UART5,
    UART6,
    UART8,
    UART9,  //rs channel
    UART_NO_MAX

}UartChannelEnum;


uint8_t UartErrorFlagGet(uint8_t ch);
void UartErrorFlagClear(uint8_t ch,uint8_t dat);
uint8_t UartRxDataGet(uint8_t ch);
void UartINTSRxDisable(uint8_t ch);
void UartINTSRxEnable(uint8_t ch);
void UartRxDisable(uint8_t ch);
void UartRxEnable(uint8_t ch);
void UartRxBitRateSet(uint8_t ch);
void UartTxDisable(uint8_t ch);
void UartTxEnable(uint8_t ch);
void UartParityEnable(uint8_t ch);
void UartParityChange(uint8_t ch,uint8_t parity);
void UartSendEndIntEnable(uint8_t ch);
void UartSendEndIntDisable(uint8_t ch);
void UartINTSTxDisable(uint8_t ch);
void UartINTSTxEnable(uint8_t ch);
uint8_t UartIsTxDataEmpty(uint8_t ch);
void UartTxDataSet(uint8_t ch,uint8_t data);

void UartStart(uint8_t ch);
void UartStop(uint8_t ch);

#endif//__SERIAL_H
