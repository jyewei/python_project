#ifndef __INTVERTERLINK_H
#define __INTVERTERLINK_H
#include "common.h"






//
typedef enum
{
    INV_COMP1,
    INV_COMP2,
    INV_FAN1,
    INV_FAN2,
    INV_FAN3,
    INV_FAN4,

    INV_NO_MAX
    
}InverterEnum;



//
typedef struct
{
    uint8_t addr;
    uint8_t enable_state;
    uint8_t connect_state; 

    union
    {
        uint8_t byte;
        struct
        {
            uint8_t ctrl:1;
            uint8_t state:1;
            uint8_t param:1;
            uint8_t reserved:5;
            
        }bit;
    }update;
    


}InverterParametersType;





void InverterLink(uint8_t taskNo,uint8_t flag_init);

#endif//__FANCOMPLINK_H
