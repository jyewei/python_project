#ifndef __CHILLERUNIT_H
#define __CHILLERUNIT_H
#include "common.h"





typedef enum
{
    UNIT_STATUS_INIT,           //上电
    UNIT_STATUS_OFF,            //关机
    UNIT_STATUS_PREPARE,        //运转准备
    UNIT_STATUS_RUN             //运转

}UnitRunStatusEnum;










uint8_t UnitUrgencyStopCheck(void);

void ChillerUnitInit(void);
void ChillerUnitControl(void);


#endif//__CHILLERUNIT_H
