#ifndef __MODBUS_H
#define __MODBUS_H
#include "Common.h"


#define RS_BUF_MAX ((uint16_t)128)

//
typedef enum
{
    RS_CH_SERVICE,  //service
    RS_CH_GROUP,    //group controller
    RS_CH_BMS,      //BMS
    RS_CH_SLAVE,
    RS_CH_THRM,     //smart thermostat
    RS_CH_INV,      //inverter

    RS_CH_MAX

}RSChannel;




//link use
typedef struct
{
    uint8_t tx_state;
    uint8_t tx_data;
    uint8_t tx_buf[RS_BUF_MAX];
    uint16_t tx_cnt;
    uint16_t tx_len;
    
    uint8_t rx_state;
    uint8_t rx_exist;
    uint8_t rx_buf[RS_BUF_MAX];
    uint8_t rx_data;
    uint16_t rx_cnt;
    uint16_t rx_len;
    

}RsRxTxType;


extern uint8_t RsTxBuf[RS_CH_MAX][RS_BUF_MAX];
extern uint8_t RsRxBuf[RS_CH_MAX][RS_BUF_MAX];
extern RsRxTxType RsRxTx[RS_CH_MAX];


uint8_t RsCheckRecvData(uint8_t ch);
void RsSetRcvOK(uint8_t ch);
void RsIntReceive(uint8_t ch);
void RsIntTransmit(uint8_t ch);
void RsSendFirstByte(uint8_t ch);
uint8_t RsCheckTxStatus(uint8_t ch);
uint8_t RsSend(uint8_t ch);
void RsInit(uint8_t ch);

#endif//__MODBUS_H
