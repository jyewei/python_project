#ifndef __CHILLERLINKDATA_H
#define __CHILLERLINKDATA_H
#include "common.h"
#include "data_unit.h"
#include "Data.h"







//
typedef enum
{
    LINK_STATUS_INIT,       //init status
    LINK_STATUS_SEND_FIRST, //
    LINK_STATUS_SEND_SECOND,//
    LINK_STATUS_SEND_REQ,   //
    LINK_STATUS_NORMAL,     //

    LINK_STATUS_MAX

}ChillerLinkStatusEnum;


//update date type enum //main -> sub
typedef enum
{
    UPDATE_UNIT_INIT_REQ,
    UPDATE_UNIT_EVENT_REQ,
    UPDATE_UNIT_IO_REQ,
    UPDATE_UNIT_ALARM1_REQ,
    UPDATE_UNIT_ALARM2_REQ,
    UPDATE_UNIT_CMD1_INFORM,
    UPDATE_UNIT_CMD2_INFORM,

    UPDATE_CYCLE1_EVENT_REQ,
    UPDATE_CYCLE1_IO1_REQ,
    UPDATE_CYCLE1_IO2_REQ,
    UPDATE_CYCLE1_INIT_SUM_REQ,

    UPDATE_CYCLE2_EVENT_REQ,
    UPDATE_CYCLE2_IO1_REQ,
    UPDATE_CYCLE2_IO2_REQ,
    UPDATE_CYCLE2_INIT_SUM_REQ,

    UPDATE_CYCLE3_EVENT_REQ,
    UPDATE_CYCLE3_IO1_REQ,
    UPDATE_CYCLE3_IO2_REQ,
    UPDATE_CYCLE3_INIT_SUM_REQ,

    UPDATE_CYCLE4_EVENT_REQ,
    UPDATE_CYCLE4_IO1_REQ,
    UPDATE_CYCLE4_IO2_REQ,
    UPDATE_CYCLE4_INIT_SUM_REQ,

    UPDATE_REQ_NO_MAX

}ChillerLinkUpdateReqEnum;

//update date type enum //sub -> main
typedef enum
{
    UPDATE_UNIT_INIT,       //
    UPDATE_UNIT_EVENT,      //
    UPDATE_UNIT_EVENT_INFORM,
    UPDATE_UNIT_IO,         //
    UPDATE_UNIT_ALARM1,      //
    UPDATE_UNIT_ALARM2,      //

    UPDATE_CYCLE1_EVENT, 
    UPDATE_CYCLE1_IO1,
    UPDATE_CYCLE1_IO2,
    UPDATE_CYCLE1_INIT_SUM,

    UPDATE_CYCLE2_EVENT, 
    UPDATE_CYCLE2_IO1,
    UPDATE_CYCLE2_IO2,
    UPDATE_CYCLE2_INIT_SUM,

    UPDATE_CYCLE3_EVENT, 
    UPDATE_CYCLE3_IO1,
    UPDATE_CYCLE3_IO2,
    UPDATE_CYCLE3_INIT_SUM,

    UPDATE_CYCLE4_EVENT, 
    UPDATE_CYCLE4_IO1,
    UPDATE_CYCLE4_IO2,
    UPDATE_CYCLE4_INIT_SUM,

    UPDATE_CHECK_DATA,

    UPDATE_DATA_NO_MAX

}ChillerLinkUpdateDateEnum;



//update request type
typedef union
{
    uint32_t data;
    struct
    {
        uint8_t unit_init_req:1;    //b0
        uint8_t unit_event_req:1;   //b1
        uint8_t unit_io_req:1;      //b3
        uint8_t unit_alarm1_req:1;  //b4
        uint8_t unit_alarm2_req:1;  //b5
        uint8_t unit_cmd1_inform:1; //b6
        uint8_t unit_cmd2_inform:1; //b7

        uint8_t cycle1_event_req:1;     //b8
        uint8_t cycle1_io1_req:1;       //b9
        uint8_t cycle1_io2_req:1;       //b10
        uint8_t cycle1_init_sum_req:1;  //b11

        uint8_t cycle2_event_req:1;     //b12
        uint8_t cycle2_io1_req:1;       //b13
        uint8_t cycle2_io2_req:1;       //b14
        uint8_t cycle2_init_sum_req:1;  //b15

        uint8_t cycle3_event_req:1;     //b16
        uint8_t cycle3_io1_req:1;       //b17
        uint8_t cycle3_io2_req:1;       //b18
        uint8_t cycle3_init_sum_req:1;  //b19

        uint8_t cycle4_event_req:1;     //b20
        uint8_t cycle4_io1_req:1;       //b21
        uint8_t cycle4_io2_req:1;       //b22
        uint8_t cycle4_init_sum_req:1;  //b23

        uint8_t reserved:8;             //b24~31
    }bit;

}ChillerUpdateReqType;

//update data type
typedef union
{
    uint32_t data;
    struct
    {
        uint8_t unit_init:1;            //b0
        uint8_t unit_event:1;           //b1
        uint8_t unit_event_inform:1;    //b2
        uint8_t unit_io:1;              //b3
        uint8_t unit_alarm1:1;          //b4
        uint8_t unit_alarm2:1;          //b5

        uint8_t cycle1_event:1;         //b6
        uint8_t cycle1_io1:1;           //b7
        uint8_t cycle1_io2:1;           //b8
        uint8_t cycle1_init_sum:1;      //b9

        uint8_t cycle2_event:1;         //b10
        uint8_t cycle2_io1:1;           //b11
        uint8_t cycle2_io2:1;           //b12
        uint8_t cycle2_init_sum:1;      //b13

        uint8_t cycle3_event:1;         //b14
        uint8_t cycle3_io1:1;           //b15
        uint8_t cycle3_io2:1;           //b16
        uint8_t cycle3_init_sum:1;      //b17

        uint8_t cycle4_event:1;         //b18
        uint8_t cycle4_io1:1;           //b19
        uint8_t cycle4_io2:1;           //b20
        uint8_t cycle4_init_sum:1;      //b21

        uint8_t check_data:1;

        uint16_t reserved:9;     //b23~31
    }bit;

}ChillerUpdateDataType;



//=====
//data table struct
typedef struct
{
    UnitTableInitType   unit_init;
    UnitTableEventType  unit_event;
    UnitTableIOType     unit_io;
    UnitTableAlarmType  unit_alarmh;    //alarm history
    UnitTableCmdType    unit_cmd;

    CycleDataTableType cycle[CYCLE_CH_MAX];
    
}ChillerLinkDataTableType;



//link use parameters
typedef struct
{
    uint8_t sys_no;     //system no
    uint8_t unit_no;    //local address
//
    uint8_t dst_sys;
    uint8_t dst_addr;

//connect
    uint8_t pre_recv_cnt;           //first time connect number
    uint8_t recv_cnt_act;           //secondary time connect number,actual number
    uint32_t pre_recv_addr;         //first time receive sub board address bit0~bit30 -> address 2~31
    uint32_t recv_addr_act;         //
    uint32_t addr_duplicate_state;   //address duplicate state //local
    uint8_t  comm_status;            //
    uint8_t  addr_duplicate_cnt;
    uint8_t req_ch;                 //require table data when connect ensure
    uint8_t req_cnt;                 //

    union
    {
        uint8_t state;
        struct ChillerLinkData
        {
            uint8_t connect:1;          //
            uint8_t cycle1_enable:1;    //
            uint8_t cycle2_enable:1;    //
            uint8_t cycle3_enable:1;    //
            uint8_t cycle4_enable:1;    //
            uint8_t connect_check:1;    //use to judge if can send check frame
            uint8_t reserved:2;

        }bit;
    }unit_state[CHILLER_CH_MAX];
    

}ChillerLinkDataLocalType;

//update type
typedef struct
{
    ChillerUpdateReqType  req;
    ChillerUpdateDataType dat;

}ChillerLinkUpdate;





//===use chiller link
extern ChillerLinkDataLocalType link;
extern ChillerLinkDataTableType table[];
extern ChillerLinkUpdate update[];


//===function
void ChillerLinkSetDefaultParameters(void);
void ChillerlinkUpdateRequest(uint8_t ch,uint8_t update_type);
void ChillerlinkUpdateData(uint8_t ch,uint8_t update_type);


#endif//__CHILLERLINKDATA_H
