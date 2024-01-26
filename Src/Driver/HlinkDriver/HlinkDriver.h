#ifndef __HLINKDRIVER_H
#define __HLINKDRIVER_H
#include "common.h"



#define HL_MAIN_TO_SUB      ((uint8_t)0x22)
#define HL_SUB_TO_MAIN      ((uint8_t)0x22)
#define HL_CC_TO_CHILLER    ((uint8_t)0x22) //TODO
#define HL_CHILLER_TO_CC    ((uint8_t)0x22)


#define HL_ON       ((uint8_t)1)
#define HL_OFF      ((uint8_t)0)
#define HL_OK       ((uint8_t)1)
#define HL_NG       ((uint8_t)0) 
#define HL_TRUE     ((uint8_t)1)
#define HL_FALSE    ((uint8_t)0) 

#define HL_BUF_SIZE ((uint8_t)48)
#define BROADCAST   ((uint8_t)0xFF)

#define HL_SUCCESS  ((uint8_t)0)    //transmit success
#define HL_CONTINUE ((uint8_t)1)    //hlink sending
#define HL_FAIL     ((uint8_t)2)    //transmit fail

//
typedef enum
{
    HL_CH_CHILLER,     //
    HL_CH_CC,          //centre controller
    HL_CH_MAX

}HLChannel;


//
extern uint8_t HLTxBuf[HL_CH_MAX][HL_BUF_SIZE];
extern uint8_t HLRxBuf[HL_CH_MAX][HL_BUF_SIZE];


//==function
uint8_t HLCheckTxStatus(uint8_t ch);
uint8_t HLCheckRecvData(uint8_t ch);
void HLSetRcvOK(uint8_t ch);
void HLSetAckOpt(uint8_t ch,uint8_t opt);
int HLSetAckTable(uint8_t ch,uint8_t id,uint8_t ref_sys,uint8_t addr);
void HLClearAckTable(uint8_t ch);
uint8_t HLSetIDTable(uint8_t ch, uint8_t dat);
void HLinkInit(uint8_t ch,uint8_t self_state);
uint8_t HLSendByte(uint8_t ch,uint8_t dat);
int HLGetByte(uint8_t ch,uint8_t *dat);
void HLClearCarryCount(uint8_t ch);
uint8_t HLGetCarryCount(uint8_t ch);
void HLClearSendStatus(uint8_t ch);
uint8_t HLSetFrameSize(uint8_t ch,uint8_t len);
void HLSetTxRetryCntMax(uint8_t ch,uint8_t val);
uint8_t HLGetTxRetryCntMax(uint8_t ch);
uint8_t HLSend(uint8_t ch);
void HLSendFirstByte(uint8_t ch);
void HLInt1ms(uint8_t ch);
void HLCarryInt(uint8_t ch);
void HLIntReceive(uint8_t ch);
void HLIntTransmit(uint8_t ch);




#endif//__HLINKDRIVER_H
