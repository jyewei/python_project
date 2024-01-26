#ifndef __CHILLERLINKMAIN_H
#define __CHILLERLINKMAIN_H
#include "common.h"











void ChillerLinkMainInit(void);
void ChillerMainTimerHandle(void);
void ChillerMainTransmit(void);
void ChillerMainReceive(uint8_t *buf);



#endif//__CHILLERLINKMAIN_H
