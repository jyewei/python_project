#ifndef __CHILLERLINKSUB_H
#define __CHILLERLINKSUB_H
#include "common.h"













void ChillerSubTimerHandle(void);
void ChillerSubTransmit(void);
void ChillerSubReceive(uint8_t *buf);
void ChillerLinkSubInit(void);

#endif//__CHILLERLINKSUB_H
