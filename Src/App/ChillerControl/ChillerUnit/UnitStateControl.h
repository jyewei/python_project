#ifndef __UNITSTATECONTROL_H
#define __UNITSTATECONTROL_H
#include "Common.h"










void UnitSetRunModeUpdate(void);
uint8_t GetUnitSetRunMode(void);
uint8_t GetUnitRealRunMode(void);

void UnitOperableStateUpdate(void);
uint8_t UnitOperableStateCheck(void);
void UnitSetCycleRun(uint8_t cycle,uint8_t cycle_mode);
void UnitSetCycleFreq(uint8_t cycle,uint16_t freq);
void UnitSetCycleClose(uint8_t cycle);


void UnitRunCool(void);
void UnitRunHeat(void);
void UnitRunPump(void);



#endif//__UNITSTATECONTROL_H
