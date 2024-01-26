#ifndef __COMPCONTROL_H
#define __COMPCONTROL_H
#include "common.h"



typedef enum
{
    COMP_INIT,              //上电
    COMP_COLD_START_CHECK,  //冷启动检测
    COMP_RUN_COLD_START,    //冷启动
    COMP_OFF_NORMAL,        //正常停止
    COMP_3MINS_PROTECT,     //3mins保护状态
    COMP_RUN_NORMAL,        //正常运行中

    COMP_STATUS_MAX

}CompModeEnum;






void CompSetFimax4RefreshTimer(uint8_t cycle,uint8_t comp,uint16_t tm);
uint16_t CompGetFimax4RefreshTimer(uint8_t cycle,uint8_t comp);
uint16_t CompGetInv2CheckTimer(uint8_t cycle,uint8_t comp);
void CompSetInv2CheckTimer(uint8_t cycle,uint8_t comp,uint16_t tm);

uint8_t CompProtectP09StepCheck(uint8_t cycle,uint8_t comp);
void CompRunStatusControl(uint8_t cycle);



#endif//__COMPCONTROL_H
