#ifndef __CHILLERLINK_H
#define __CHILLERLINK_H
#include "common.h"




#define MAX_BUF_NO      4
#define MAX_BUF_LEN     48



//接受数据缓存用
typedef struct
{
    uint8_t first;
    uint8_t last;
    uint8_t empty;
    uint8_t buf[MAX_BUF_NO][MAX_BUF_LEN];

}LinkQueueType;






void ChillerLink(uint8_t taskNo,uint8_t flag_init);


#endif//__CHILLERLINK_H
