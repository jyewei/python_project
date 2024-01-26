#ifndef __CYCLEDEVICECONTROL_H
#define __CYCLEDEVICECONTROL_H
#include "common.h"









void CycleFourWayValveControl(uint8_t cycle,uint8_t state);
uint8_t CycleGetFourWayValveState(uint8_t cycle);
uint8_t CycleGetFourWayValveState(uint8_t cycle);
void CycleCompCrankcaseHeatBeltControl(uint8_t cycle);
void CycleCompEviValveControl(uint8_t cycle);
void CycleOilReturnValveControl(uint8_t cycle);


#endif//__CYCLEDEVICECONTROL_H
