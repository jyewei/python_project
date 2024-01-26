#ifndef __COMPPROTECT_H
#define __COMPPROTECT_H
#include "common.h"






typedef enum
{
    PROT_NULL,              //正常控制
    PROT_FREQ_FORCED_DROP,  //频率强制下降
    PROT_FREQ_FORCED_RISE,  //频率强制上升
    PROT_FREQ_FORBID_DROP,  //频率禁止下降
    PROT_FREQ_FORBID_RISE,  //频率禁止上升
    PROT_STATUS_MAX

}ProtectStatusEnum;




uint8_t CompGetPressureRatio(uint8_t cycle,uint8_t comp);    //压力比 Pd/Ps




#endif//__COMPPROTECT_H
