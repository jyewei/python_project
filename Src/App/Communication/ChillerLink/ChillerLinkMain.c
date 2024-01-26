/***********************************************************************
@file   : ChillerLinkMain.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "ChillerLinkMain.h"
#include "ChillerLinkData.h"
#include "Data.h"
#include "HlinkDriver.h"
#include "TimerCounter.h"


//单位s
#define TIME_INIT_CONNECT               ((uint8_t)8)    //
#define TIME_REGULAR_UNIT_EVENT_REQ     ((uint8_t)4)    //
#define TIME_REGULAR_UNIT_IO_REQ        ((uint8_t)1)    //
#define TIME_REGULAR_UNIT_INIT_REQ      ((uint8_t)18)   //
#define TIME_REGULAR_UNIT_CMD_REQ       ((uint8_t)7)    //
#define TIME_REGULAR_CYCLE_IO_REQ       ((uint8_t)1)    //
#define TIME_REGULAR_CYCLE_EVENT_REQ    ((uint8_t)17)   //
#define TIME_REGULAR_CYCLE_INIT_SUM_REQ ((uint8_t)12)   //

#define TIME_DISCONNECT                 ((uint8_t)60)   //
#define TIME_SUB_ADDR_DUPLICATE         ((uint8_t)60)   //
#define TIME_MAIN_ADDR_DUPLICATE        ((uint8_t)30)

//time
typedef struct
{
    uint8_t init_connect;               //after power on,time to ensure connect state
    uint8_t sub_addr_duplicate;        //during init communication ,check address duplicate
    uint8_t regular_unit_io_req;
    uint8_t regular_unit_init_req;
    uint8_t regular_unit_cmd_req;
    uint8_t regular_unit_event_req;
    uint8_t addr_duplicate;
    uint8_t reserved2;
    uint8_t regular_cycle_io_req;
    uint8_t regular_cycle_init_sum_req;
    uint8_t regular_cycle_event_req;
    uint8_t disconnect[CHILLER_CH_MAX]; //time to check disconnect state

}ChillerLinkMainTimeType;

static ChillerLinkMainTimeType time;




/************************************************************************
@name  	: ChillerLinkMainTimerCounter
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerLinkMainTimerCounter(void)
{
    uint8_t *pTime;
    uint16_t i,len;

    pTime = &time.init_connect;
    if (TimerCheck(NO_CHILLER_LINK_TIME_BASE) == TRUE)
    {
        len = sizeof(ChillerLinkMainTimeType)/sizeof(uint8_t);
        TimerSet(NO_CHILLER_LINK_TIME_BASE,1); //base 1s
        for ( i = 0; i < len; i++)
        {
            if (*(pTime + i) != 0)
            {
                (*(pTime + i))--;
            }
        }
    }
}

/************************************************************************
@name  	: ChillerLinkUnitCmdInform1Set
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerLinkUnitCmdInform1Set(void)
{
    uint8_t ch = HL_CH_CHILLER;
    UnitTableCmdType *pCmd; 

    pCmd = &table[link.unit_no].unit_cmd; //
//update data frame
    HLTxBuf[ch][11] = (uint8_t)(pCmd->data1.word >> 8);
    HLTxBuf[ch][12] = (uint8_t)(pCmd->data1.word);
}

/************************************************************************
@name  	: ChillerLinkUnitCmdInform2Set
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerLinkUnitCmdInform2Set(void)
{
    uint8_t ch = HL_CH_CHILLER;
    UnitTableCmdType *pCmd; 

    pCmd = &table[link.unit_no].unit_cmd; //
//update data frame
    HLTxBuf[ch][11] = (uint8_t)(pCmd->exv_cool_init_step >> 8);
    HLTxBuf[ch][12] = (uint8_t)(pCmd->exv_cool_init_step);
}

/************************************************************************
@name  	: ChillerLinkSendUnitCmdInform
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerLinkSendUnitCmdInform(ChillerLinkUpdateReqEnum update_type)
{
    uint8_t ch = HL_CH_CHILLER;
    uint8_t chiller_ch;

    HLTxBuf[ch][7] = 0xA3;
    HLTxBuf[ch][8] = 0x23;
    HLTxBuf[ch][10] = 0x12;

    if (HLTxBuf[ch][6] != 0xFF)
    {
        chiller_ch = HLTxBuf[ch][6];
        // time.regular_unit_cmd_req[chiller_ch] = TIME_REGULAR_UNIT_CMD_REQ;//reset regular time when send cmd
    }    
    switch (update_type)
    {
        case UPDATE_UNIT_CMD1_INFORM:
        {
            HLTxBuf[ch][9] = 0x00;
            ChillerLinkUnitCmdInform1Set();
        }
            break;

        case UPDATE_UNIT_CMD2_INFORM:
        {
            HLTxBuf[ch][9] = 0x12;
            ChillerLinkUnitCmdInform2Set();
        }
            break;

        default:
            break;
    }
}

//===
/************************************************************************
@name  	: ChillerLinkSendReq
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerLinkSendReq(uint8_t chiller_ch,uint8_t update_type)
{
    uint8_t ch = HL_CH_CHILLER;

    if (chiller_ch == 0)
    {
        chiller_ch = 0xFF;
    }

    HLTxBuf[ch][0] = HL_MAIN_TO_SUB;    //id code
    HLTxBuf[ch][1] = 0x00;              //type code
    HLTxBuf[ch][2] = 0x0C;              //length
    HLTxBuf[ch][3] = link.sys_no;       //src_sys
    HLTxBuf[ch][4] = link.unit_no;      //src_addr
    HLTxBuf[ch][5] = link.dst_sys;      //dst_sys
    HLTxBuf[ch][6] = chiller_ch;        //dst_addr
    HLTxBuf[ch][7] = 0xA7;              //
    HLTxBuf[ch][8] = 0x22;              //
    HLTxBuf[ch][9] = 0x00;
    HLTxBuf[ch][10] = 0x12;
    HLTxBuf[ch][11] = 0x00;

    switch (update_type)
    {
        case UPDATE_UNIT_INIT_REQ:      HLTxBuf[ch][8] = 0x21;  HLTxBuf[ch][9] = 0x03;  HLTxBuf[ch][10] = 0x12;       break;
        case UPDATE_UNIT_EVENT_REQ:     HLTxBuf[ch][8] = 0x22;  HLTxBuf[ch][9] = 0x00;  HLTxBuf[ch][10] = 0x0B;       break;
        case UPDATE_UNIT_IO_REQ:        HLTxBuf[ch][8] = 0x25;  HLTxBuf[ch][9] = 0x00;  HLTxBuf[ch][10] = 0x10;       break;
        case UPDATE_UNIT_ALARM1_REQ:    HLTxBuf[ch][8] = 0x27;  HLTxBuf[ch][9] = 0x00;  HLTxBuf[ch][10] = 0x12;       break;
        case UPDATE_UNIT_ALARM2_REQ:    HLTxBuf[ch][8] = 0x27;  HLTxBuf[ch][9] = 0x12;  HLTxBuf[ch][10] = 0x12;       break;
        case UPDATE_UNIT_CMD1_INFORM:   ChillerLinkSendUnitCmdInform(UPDATE_UNIT_CMD1_INFORM);    break;
        case UPDATE_UNIT_CMD2_INFORM:   ChillerLinkSendUnitCmdInform(UPDATE_UNIT_CMD2_INFORM);    break;

        case UPDATE_CYCLE1_EVENT_REQ:   
        {
            HLTxBuf[ch][8] = 0x42;  HLTxBuf[ch][9] = 0x00;  HLTxBuf[ch][10] = 0x05;       
        }
            break;
        case UPDATE_CYCLE1_IO1_REQ:     HLTxBuf[ch][8] = 0x45;  HLTxBuf[ch][9] = 0x00;  HLTxBuf[ch][10] = 0x12;       break;
        case UPDATE_CYCLE1_IO2_REQ:     HLTxBuf[ch][8] = 0x45;  HLTxBuf[ch][9] = 0x12;  HLTxBuf[ch][10] = 0x10;       break;
        case UPDATE_CYCLE1_INIT_SUM_REQ:HLTxBuf[ch][8] = 0x44;  HLTxBuf[ch][9] = 0x00;  HLTxBuf[ch][10] = 0x0A;       break;
        case UPDATE_CYCLE2_EVENT_REQ:   
        {
            HLTxBuf[ch][8] = 0x52;  HLTxBuf[ch][9] = 0x00;  HLTxBuf[ch][10] = 0x05;       
        }
            break;
        case UPDATE_CYCLE2_IO1_REQ:     HLTxBuf[ch][8] = 0x55;  HLTxBuf[ch][9] = 0x00;  HLTxBuf[ch][10] = 0x12;       break;
        case UPDATE_CYCLE2_IO2_REQ:     HLTxBuf[ch][8] = 0x55;  HLTxBuf[ch][9] = 0x12;  HLTxBuf[ch][10] = 0x10;       break;
        case UPDATE_CYCLE2_INIT_SUM_REQ:HLTxBuf[ch][8] = 0x54;  HLTxBuf[ch][9] = 0x00;  HLTxBuf[ch][10] = 0x0A;       break;
        case UPDATE_CYCLE3_EVENT_REQ:   
        {
            HLTxBuf[ch][8] = 0x62;  HLTxBuf[ch][9] = 0x00;  HLTxBuf[ch][10] = 0x05;       
        }
            break;
        case UPDATE_CYCLE3_IO1_REQ:     HLTxBuf[ch][8] = 0x65;  HLTxBuf[ch][9] = 0x00;  HLTxBuf[ch][10] = 0x12;       break;
        case UPDATE_CYCLE3_IO2_REQ:     HLTxBuf[ch][8] = 0x65;  HLTxBuf[ch][9] = 0x12;  HLTxBuf[ch][10] = 0x10;       break;
        case UPDATE_CYCLE3_INIT_SUM_REQ:HLTxBuf[ch][8] = 0x64;  HLTxBuf[ch][9] = 0x00;  HLTxBuf[ch][10] = 0x0A;       break;
        case UPDATE_CYCLE4_EVENT_REQ:   
        {
            HLTxBuf[ch][8] = 0x72;  HLTxBuf[ch][9] = 0x00;  HLTxBuf[ch][10] = 0x05;       
        }
            break;
        case UPDATE_CYCLE4_IO1_REQ:     HLTxBuf[ch][8] = 0x75;  HLTxBuf[ch][9] = 0x00;  HLTxBuf[ch][10] = 0x12;       break;
        case UPDATE_CYCLE4_IO2_REQ:     HLTxBuf[ch][8] = 0x75;  HLTxBuf[ch][9] = 0x12;  HLTxBuf[ch][10] = 0x10;       break;
        case UPDATE_CYCLE4_INIT_SUM_REQ:HLTxBuf[ch][8] = 0x74;  HLTxBuf[ch][9] = 0x00;  HLTxBuf[ch][10] = 0x0A;       break;
        default:
            break;
    }
}

//====send end


/************************************************************************
@name  	: ChillerMainRecvUnitInit
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerMainRecvUnitInit(uint8_t *buf)
{
    uint16_t *pdat;
    uint8_t unit_no = buf[4];
    uint8_t len,i;

    pdat = &table[unit_no].unit_init.dsw1.word;
    if ((buf[9] == 0x03) && (buf[10] == 0x12))
    {//get data
        len = sizeof(UnitTableInitType)/sizeof(uint16_t);
        for (i = 0; i < len; i++)
        {
            *(pdat+i) = ((uint16_t)buf[11+i*2] << 8) | buf[12+i*2]; 
        }
    }
}

/************************************************************************
@name  	: ChillerMainRecvUnitEvent
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerMainRecvUnitEvent(uint8_t *buf)
{
    uint16_t *pEvent; 
    uint8_t unit_no = buf[4];
    uint8_t len,i;

    pEvent = &table[unit_no].unit_event.state.word;
    if ((buf[9] == 0x00) && (buf[10] == 0x0B))
    {//get data
        len = sizeof(UnitTableEventType)/sizeof(uint16_t);
        for (i = 0; i < len; i++)
        {
            *(pEvent+i) = ((uint16_t)buf[11+i*2] << 8) | buf[12+i*2]; 
        }
    }
}

/************************************************************************
@name  	: ChillerMainRecvUnitCmd
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerMainRecvUnitCmd(uint8_t *buf)
{
    //can not receive
}

/************************************************************************
@name  	: ChillerMainRecvUnitIO
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerMainRecvUnitIO(uint8_t *buf)
{
    uint16_t *pIO; 
    uint8_t unit_no = buf[4];
    uint8_t len,i;

    pIO = &table[unit_no].unit_io.DIO.word;
    if ((buf[9] == 0x00) && (buf[10] == 0x10))
    {//get data
        len = sizeof(UnitTableIOType)/sizeof(uint16_t);
        for (i = 0; i < len; i++)
        {
            *(pIO+i) = ((uint16_t)buf[11+i*2] << 8) | buf[12+i*2]; 
        }
    }
}

/************************************************************************
@name  	: ChillerMainRecvUnitAlarmHistory
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerMainRecvUnitAlarmHistory(uint8_t *buf)
{
    uint16_t *pdata;
    uint8_t len,i;

    if ((buf[9] == 0x00) && (buf[10] == 0x12))
    {//get data
        pdata = &table[link.unit_no].unit_alarmh.total_num;
        len = 18;
        for ( i = 0; i < len; i++)
        {
            *(pdata+i) = ((uint16_t)buf[11+i*2] << 8) | buf[12+i*2]; 
        }
    }
    else if ((buf[9] == 0x12) && (buf[10] == 0x12))
    {
        pdata = &table[link.unit_no].unit_alarmh.alarm_history[17];
        len = 18;
        for ( i = 0; i < 0x12; i++)
        {
            *(pdata+i) = ((uint16_t)buf[11+i*2] << 8) | buf[12+i*2];
        }
    }
}

/************************************************************************
@name  	: ChillerMainRecvCycleEvent
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerMainRecvCycleEvent(uint8_t cycle_ch,uint8_t *buf)
{
    uint16_t *pEvent;
    uint8_t len,i;

    pEvent = &table[link.unit_no].cycle[cycle_ch].event.state1.word;
    len = sizeof(CycleTableEventType)/sizeof(uint16_t);
    for ( i = 0; i < len; i++)
    {
        *(pEvent+i) = ((uint16_t)buf[11+i*2] << 8) | buf[12+i*2];
    }
}

/************************************************************************
@name  	: ChillerMainRecvCycleIO
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerMainRecvCycleIO(uint8_t cycle_ch,uint8_t *buf)
{
    uint16_t *pIO;
    uint8_t len,i;

    if ((buf[9] == 0x00) && (buf[10] == 0x12))
    {
        pIO = &table[link.unit_no].cycle[cycle_ch].io.io1.word;
        len = 18;
        for ( i = 0; i < len; i++)
        {
            *(pIO+i) = ((uint16_t)buf[11+i*2] << 8) | buf[12+i*2];
        }
    }
    else if ((buf[9] == 0x12) && (buf[10] == 0x10))
    {
        pIO = &table[link.unit_no].cycle[cycle_ch].io.fan2_rt_freq;
        len = 14;
        for ( i = 0; i < len; i++)
        {
            *(pIO+i) = ((uint16_t)buf[11+i*2] << 8) | buf[12+i*2];
        }
    }
}

/************************************************************************
@name  	: ChillerMainRecvCycleInitSum
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerMainRecvCycleInitSum(uint8_t cycle_ch,uint8_t *buf)
{
    CycleTableInitSumType *pInitsum; 
    uint8_t unit_no = buf[4];

    pInitsum = &table[unit_no].cycle[cycle_ch].init_sum;
    if ((buf[9] == 0x00) && (buf[10] == 0x0A))
    {
        pInitsum->comp_romNO = ((uint16_t)buf[11] << 8) | buf[12];
        pInitsum->fan_romNO = ((uint16_t)buf[13] << 8) | buf[14];
        pInitsum->comp_run_time_sum = ((uint32_t)buf[15] << 24) | (uint32_t)buf[16] << 16 | (uint32_t)buf[17] << 8 | buf[18];
        pInitsum->comp_power_sum = ((uint32_t)buf[19] << 24) | (uint32_t)buf[20] << 16 | (uint32_t)buf[21] << 8 | buf[22];
        pInitsum->sys_run_time_sum = ((uint32_t)buf[23] << 24) | (uint32_t)buf[24] << 16 | (uint32_t)buf[25] << 8 | buf[26];
    }
}

/************************************************************************
@name  	: ChillerMainRecvDataTable
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerMainRecvDataTable(uint8_t *buf)
{
    switch (buf[8])
    {
        case 0x21: ChillerMainRecvUnitInit(buf);            break;
        case 0x22: ChillerMainRecvUnitEvent(buf);           break;
        case 0x23: ChillerMainRecvUnitCmd(buf);             break;
        case 0x25: ChillerMainRecvUnitIO(buf);              break;
        case 0x27: ChillerMainRecvUnitAlarmHistory(buf);    break;

        case 0x42: ChillerMainRecvCycleEvent(CYCLE_1,buf);  break;
        case 0x44: ChillerMainRecvCycleIO(CYCLE_1,buf);     break;
        case 0x45: ChillerMainRecvCycleInitSum(CYCLE_1,buf);break;

        case 0x52: ChillerMainRecvCycleEvent(CYCLE_2,buf);  break;
        case 0x54: ChillerMainRecvCycleIO(CYCLE_2,buf);     break;
        case 0x55: ChillerMainRecvCycleInitSum(CYCLE_2,buf);break;

        case 0x62: ChillerMainRecvCycleEvent(CYCLE_3,buf);  break;
        case 0x64: ChillerMainRecvCycleIO(CYCLE_3,buf);     break;
        case 0x65: ChillerMainRecvCycleInitSum(CYCLE_3,buf);break;

        case 0x72: ChillerMainRecvCycleEvent(CYCLE_4,buf);  break;
        case 0x74: ChillerMainRecvCycleIO(CYCLE_4,buf);     break;
        case 0x75: ChillerMainRecvCycleInitSum(CYCLE_4,buf);break;

        default:    break;
    }
}

/************************************************************************
@name  	: ChillerMainRecvConnectRequest
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerMainRecvConnectRequest(uint8_t *buf)
{
    uint8_t ch = buf[4];

    if ((link.recv_addr_act & ((uint32_t)(1<<(ch)))) != 0)//receive before
    {
        link.recv_addr_act |= (uint32_t)(1<<(ch)); //set connect state
        link.unit_state[ch].bit.connect = TRUE;
        //req data
        ChillerlinkUpdateRequest(ch,UPDATE_UNIT_EVENT_REQ);
        ChillerlinkUpdateRequest(ch,UPDATE_UNIT_IO_REQ);
        ChillerlinkUpdateRequest(ch,UPDATE_UNIT_INIT_REQ);

        for (ch = 0; ch < atw.unit.local.cycle_active_cnt; ch++)
        {
            ChillerlinkUpdateRequest(ch,UPDATE_CYCLE1_IO1_REQ + ch*4);
            ChillerlinkUpdateRequest(ch,UPDATE_CYCLE1_IO2_REQ + ch*4);
            ChillerlinkUpdateRequest(ch,UPDATE_CYCLE1_INIT_SUM_REQ + ch*4);
        }
    }
}

/************************************************************************
@name  	: CheckLinkMainRecvStatus
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static uint8_t CheckLinkMainRecvStatus(uint8_t *buf)
{
    uint8_t res = FALSE;
    uint8_t src_ch = buf[4];

    switch (link.comm_status)
    {
        case LINK_STATUS_SEND_FIRST: //first recv
        {
            link.pre_recv_cnt++;
            if ((link.pre_recv_addr & (1<<(src_ch))) == 0)//address not exist
            {
                link.pre_recv_addr |= (uint32_t)(1<<(src_ch));
            }
            else //address already exist
            {
                link.addr_duplicate_state |= (uint32_t)(1<<(src_ch));//address duplicate
            }        
        }
            break;

        case LINK_STATUS_SEND_SECOND: //second recv
        {
            link.recv_cnt_act++;
            if ((link.recv_addr_act & (1<<(src_ch))) == 0)//address not exist
            {
                link.recv_addr_act |= (uint32_t)(1<<(src_ch));
            }
            else 
            {
                link.addr_duplicate_state |= (uint32_t)(1<<(src_ch));//address duplicate
            }
        }
            break;

        case LINK_STATUS_SEND_REQ:
        case LINK_STATUS_NORMAL:
        {
            res = TRUE;    //
        }
            break;

        default: 
            break;
    }

    return res;
}

//====receive end

/************************************************************************
@name  	: LinkInitRequireData
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static uint8_t LinkInitRequireData(void)//interval 100ms to send req to every chiller online
{
    uint8_t res = FALSE;
    uint8_t ch = link.req_ch,send_cnt = 0;
    uint8_t tab_update[15] = {UPDATE_UNIT_EVENT_REQ, UPDATE_UNIT_IO_REQ,    UPDATE_UNIT_INIT_REQ, 
                              UPDATE_CYCLE1_IO1_REQ, UPDATE_CYCLE1_IO2_REQ , UPDATE_CYCLE1_INIT_SUM_REQ,
                              UPDATE_CYCLE2_IO1_REQ, UPDATE_CYCLE2_IO2_REQ , UPDATE_CYCLE2_INIT_SUM_REQ,
                              UPDATE_CYCLE3_IO1_REQ, UPDATE_CYCLE3_IO2_REQ , UPDATE_CYCLE3_INIT_SUM_REQ,
                              UPDATE_CYCLE4_IO1_REQ, UPDATE_CYCLE4_IO2_REQ , UPDATE_CYCLE4_INIT_SUM_REQ };

    
    if (link.recv_addr_act != 0)
    {
        if (TimerCheck(NO_CHILLER_INIT_REQ) == TRUE)
        {
            if (link.unit_state[ch].bit.connect == TRUE )
            {
                send_cnt = atw.unit.local.cycle_active_cnt*3 + 3;
                TimerSet(NO_CHILLER_INIT_REQ,2); //200ms
                ChillerlinkUpdateRequest(ch,tab_update[link.req_cnt]);
                link.req_cnt++;
                if (link.req_cnt == send_cnt)
                {
                    link.req_cnt = 0;
                    link.req_ch++;
                    if (link.req_ch == CHILLER_CH_MAX)
                    {
                        link.req_ch = 1;
                        res = TRUE;
                    }
                }
            }
            else
            {
                link.req_ch++;
                if (link.req_ch == CHILLER_CH_MAX)
                {
                    link.req_ch = 1;
                    res = TRUE;
                }
            }
        }
    }
    else res = TRUE; //no other chiller exist

    return res;
}

/************************************************************************
@name  	: ChillerMainRegularTransmit
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerMainRegularTransmit(void)
{
    uint8_t ch;
    static uint8_t unit_event_ch = CHILLER_2;
    static uint8_t unit_io_ch = CHILLER_2;
    static uint8_t unit_init_ch = CHILLER_2;
    static uint8_t unit_cmd_ch = CHILLER_2;
    static uint8_t cycle_io_ch = CHILLER_2;
    static uint8_t cycle_event_ch = CHILLER_2;
    static uint8_t cycle_init_sum_ch = CHILLER_2;

    if (link.recv_addr_act == 0)
    {
        return;
    }
    
//unit    
    //regular  unit_event_req
    if (time.regular_unit_event_req == 0) //time over
    {
        if (link.unit_state[unit_event_ch].bit.connect == TRUE)
        {
            time.regular_unit_event_req = TIME_REGULAR_UNIT_EVENT_REQ; //set time
            ChillerlinkUpdateRequest(unit_event_ch,UPDATE_UNIT_EVENT_REQ);
        }
        unit_event_ch++;
        if (unit_event_ch == CHILLER_CH_MAX)
        {
            unit_event_ch = CHILLER_2;
        }
    }
    //regular  unit_io_req
    if (time.regular_unit_io_req == 0) //
    {
        if (link.unit_state[unit_io_ch].bit.connect == TRUE)
        {
            time.regular_unit_io_req = TIME_REGULAR_UNIT_IO_REQ; 
            ChillerlinkUpdateRequest(unit_io_ch,UPDATE_UNIT_IO_REQ);
        }
        unit_io_ch++;
        if (unit_io_ch == CHILLER_CH_MAX)
        {
            unit_io_ch = CHILLER_2;
        }
    }
    //regular  unit_init_req
    if (time.regular_unit_init_req == 0) //
    {
        if (link.unit_state[unit_init_ch].bit.connect == TRUE)
        {
            time.regular_unit_init_req = TIME_REGULAR_UNIT_INIT_REQ; 
            ChillerlinkUpdateRequest(unit_init_ch,UPDATE_UNIT_INIT_REQ);
        }
        unit_init_ch++;
        if (unit_init_ch == CHILLER_CH_MAX)
        {
            unit_init_ch = CHILLER_2;
        }
    }
    //regular  unit_cmd_req
    if (time.regular_unit_cmd_req == 0) //
    {
        if (link.unit_state[unit_cmd_ch].bit.connect == TRUE)
        {
            time.regular_unit_cmd_req = TIME_REGULAR_UNIT_CMD_REQ;
            ChillerlinkUpdateRequest(unit_cmd_ch,UPDATE_UNIT_CMD1_INFORM);
            ChillerlinkUpdateRequest(unit_cmd_ch,UPDATE_UNIT_CMD2_INFORM);
        }
        unit_cmd_ch++;
        if (unit_cmd_ch == CHILLER_CH_MAX)
        {
            unit_cmd_ch = CHILLER_2;
        }
    }
//cycle
    //regular  cycle_io_req
    if (time.regular_cycle_io_req == 0) //
    {
        if (link.unit_state[cycle_io_ch].bit.connect == TRUE)
        {
            time.regular_cycle_io_req = TIME_REGULAR_CYCLE_IO_REQ;

            for (ch = 0; ch < atw.unit.local.cycle_active_cnt; ch++)
            {
                ChillerlinkUpdateRequest(cycle_io_ch,UPDATE_CYCLE1_IO1_REQ + ch*4);
                ChillerlinkUpdateRequest(cycle_io_ch,UPDATE_CYCLE1_IO2_REQ + ch*4);
            }
        }
        cycle_io_ch++;
        if (cycle_io_ch == CHILLER_CH_MAX)
        {
            cycle_io_ch = CHILLER_2;
        }
    }
//     //regular  cycle_event_req
    if (time.regular_cycle_event_req == 0) //
    {
        if (link.unit_state[cycle_event_ch].bit.connect == TRUE)
        {
            time.regular_cycle_event_req = TIME_REGULAR_CYCLE_EVENT_REQ;
            for (ch = 0; ch < atw.unit.local.cycle_active_cnt; ch++)
            {
                ChillerlinkUpdateRequest(cycle_event_ch,UPDATE_CYCLE1_EVENT_REQ + ch*4);
            }
        }
        cycle_event_ch++;
        if (cycle_event_ch == CHILLER_CH_MAX)
        {
            cycle_event_ch = CHILLER_2;
        }
    }
    //regular  cycle_init_sum_req
    if (time.regular_cycle_init_sum_req == 0) //
    {
        if (link.unit_state[cycle_init_sum_ch].bit.connect == TRUE)
        {
            time.regular_cycle_init_sum_req = TIME_REGULAR_CYCLE_INIT_SUM_REQ;
            for (ch = 0; ch < atw.unit.local.cycle_active_cnt; ch++)
            {
                ChillerlinkUpdateRequest(cycle_event_ch,UPDATE_CYCLE1_INIT_SUM_REQ + ch*4);
            }
        }
        cycle_init_sum_ch++;
        if (cycle_init_sum_ch == CHILLER_CH_MAX)
        {
            cycle_init_sum_ch = CHILLER_2;
        }
    }
}
//====time handle end

/************************************************************************
@name  	: CheckConnectState
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void CheckConnectState(uint8_t *buf)
{
    uint8_t unit_ch = buf[4];

    link.unit_state[unit_ch].bit.connect = TRUE;    //set connect state
    time.disconnect[unit_ch] = TIME_DISCONNECT;     //reset connect time

}

/************************************************************************
@name  	: AddressDuplicateCheck
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void AddressDuplicateCheck(void)
{
    if ((link.comm_status == LINK_STATUS_SEND_FIRST)
        || (link.comm_status == LINK_STATUS_SEND_SECOND))//during communication init state,check sub address
    {
        if ((link.addr_duplicate_state != 0) && (time.sub_addr_duplicate == 0))
        {
            //TODO set address duplicate alarm
        }
    }
    else
    {
        if ((link.comm_status == LINK_STATUS_SEND_REQ)
            || (link.comm_status == LINK_STATUS_NORMAL))//during communication normal state,check mani address
        {
            if ((link.addr_duplicate_state != 0) && (time.addr_duplicate == 0))
            {
                //TODO set address duplicate alarm
            }
        }
    }
}

/************************************************************************
@name  	: CheckConnectState
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerMainCheckState(void)
{
    uint8_t ch;
//connect state check
    link.unit_state[CHILLER_0].bit.connect = TRUE;  //main chiller always online
    for (ch = CHILLER_2; ch < CHILLER_CH_MAX; ch++)
    {
        if ((time.disconnect[ch] == 0) && ( link.recv_addr_act & (1 << ch) != 0))//sub exist and disconnect
        {
            link.unit_state[ch].bit.connect = FALSE;    //clear connect state
            //TODO set alarm
        }
    }
//address duplicate check
    AddressDuplicateCheck();
    
}

/************************************************************************
@name  	: ChillerLinkMainInit
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ChillerLinkMainInit(void)
{
    uint32_t i;

    //get eeprom 
    if (0)//eeprom have parameter
    {
        link.recv_addr_act = atw.eep.chiller_recv_addr_act;
        for ( i = 0; i < CHILLER_CH_MAX; i++)
        {
            if ((link.recv_addr_act & (1<<i)) != 0 )
            {
                link.unit_state[i].bit.connect = TRUE;
            }
        }
        
        link.comm_status = LINK_STATUS_SEND_REQ;
    }
    else//no parameter,start 
    {
        time.init_connect = 5; //5s
        link.comm_status = LINK_STATUS_INIT;
    }
    

}

/************************************************************************
@name  	: ChillerMainReceive
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ChillerMainReceive(uint8_t *buf)
{
    uint8_t src_unit_no = buf[4];

    if (((src_unit_no < CHILLER_CH_MAX) && (src_unit_no > CHILLER_0)) //from 1~31 
        && (buf[6] == CHILLER_MAIN_ADDR))   //and to CHILLER_0
    {
        CheckConnectState(buf);
        switch (buf[7])
        {
            case 0xA8://data table
            {
                if (CheckLinkMainRecvStatus(buf) == TRUE)//connect state not update data table
                {
                    ChillerMainRecvDataTable(buf);
                }
            }
            break;

            case 0xA3://data inform
            {

            }
            break;

            case 0xA1://connect request from sub chiller
            {
                ChillerMainRecvConnectRequest(buf);
            }
            break;
        
            default:
                break;
        }
    }
    else
    {
        //@note: receive from other CHILLER_0 more than twice in 60s 
        if(src_unit_no == 1) //other CHILLER_0 exist
        {
            link.addr_duplicate_cnt++;
            if (time.addr_duplicate == 0)
            {
                time.addr_duplicate = TIME_MAIN_ADDR_DUPLICATE;
                link.addr_duplicate_cnt = 0;
            }
            else
            {
                if (link.addr_duplicate_cnt > 1)
                {
                    link.addr_duplicate_state |= 0x01;
                }
            }
        }
    }
}

/************************************************************************
@name  	: ChillerMainTransmit
@brief 	: 
@param 	: None
@return	: None
@note   : for main chiller,address_1 as broadcast address
*************************************************************************/
void ChillerMainTransmit(void)
{
    uint8_t ch,tx_status;
    uint32_t i = 0;

    tx_status = HLCheckTxStatus(HL_CH_CHILLER);
    if (tx_status == HL_SUCCESS)
    {
        for ( ch = CHILLER_0; ch < CHILLER_CH_MAX; ch++)
        {
            if (link.unit_state[ch].bit.connect == TRUE)
            {
                //req
                if (update[ch].req.data != NULL)
                {
                    for ( i = 0; i < UPDATE_REQ_NO_MAX; i++)
                    {
                        if (update[ch].req.data & (1<<i))
                        {
                            update[ch].req.data &= (uint32_t)(~(1<<i));
                            ChillerLinkSendReq(ch,i);
                            HLSend(HL_CH_CHILLER);
                            return;
                        }
                    }
                }
            }
        }
    }
    else if (tx_status == HL_FAIL)
    {
        HLClearSendStatus(HL_CH_CHILLER);
    }
}

/************************************************************************
@name  	: ChillerMainTimerHandle
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ChillerMainTimerHandle(void)
{
    ChillerLinkMainTimerCounter();  //
    ChillerMainCheckState();        //check alarm and connect state
    switch (link.comm_status)
    {
        case LINK_STATUS_INIT: //power on send unit_init_req 
        {
            if (time.init_connect == 0) //TODO add wait for group connect allow
            {
                time.init_connect = TIME_INIT_CONNECT;  //set init time to send next connect req
                time.sub_addr_duplicate = TIME_SUB_ADDR_DUPLICATE;
                link.addr_duplicate_cnt = 0;
                ChillerlinkUpdateRequest(BROADCAST,UPDATE_UNIT_INIT_REQ);   //first send unit_init_req
                link.comm_status = LINK_STATUS_SEND_FIRST;                  //next status
            }
        }
        break;

        case LINK_STATUS_SEND_FIRST: //first time send connect
        {
            if (time.init_connect == 0)//wait time over
            {
                time.init_connect = TIME_INIT_CONNECT;  //set init time
                ChillerlinkUpdateRequest(BROADCAST,UPDATE_UNIT_INIT_REQ);//second send unit_init_req
                link.comm_status = LINK_STATUS_SEND_SECOND;
                // link.comm_status = LINK_STATUS_SEND_FIRST;  //TODO for test
            }
        }
        break;

        case LINK_STATUS_SEND_SECOND: //second time send connect //and check state
        {
            if (time.init_connect == 0)//wait time over
            {
                if ((link.pre_recv_cnt == link.recv_cnt_act)        //receive sub chiller number is same as the first time //include 0
                    && (link.pre_recv_addr == link.recv_addr_act)   //and address is same in two communication
                    && (link.addr_duplicate_state == 0))            //no address duplicate
                {
                    link.comm_status = LINK_STATUS_SEND_REQ;
                    TimerSet(NO_CHILLER_INIT_REQ,1);    //wait 100ms to send req data
                    link.req_ch = CHILLER_2;            //sub chiller from CHILLER_2
                    link.req_cnt = 0;
                    atw.eep.chiller_recv_addr_act = link.recv_addr_act;//save to eeprom
                }
                else
                {
                    if (link.addr_duplicate_state == 0)         //address duplicate not exist
                    {
                        time.sub_addr_duplicate = TIME_SUB_ADDR_DUPLICATE;  //reset time counter
                    }
                    time.init_connect = TIME_INIT_CONNECT;      //set init time
                    link.pre_recv_cnt = link.recv_cnt_act = 0;  //reset state
                    link.pre_recv_addr = link.recv_addr_act = 0;//
                    link.addr_duplicate_state = 0;              //reset address duplicate state
                    link.comm_status = LINK_STATUS_SEND_FIRST; 
                    ChillerlinkUpdateRequest(BROADCAST,UPDATE_UNIT_INIT_REQ);
                }            
            }
        }
        break;

        case LINK_STATUS_SEND_REQ: //interval 100ms to send req to every chiller online
        {
            if (LinkInitRequireData() == TRUE)
            {
                link.comm_status = LINK_STATUS_NORMAL;
                link.req_ch = CHILLER_2; //regular start from CHILLER_2
            }
        }
        break;

        case LINK_STATUS_NORMAL: //normal status
        {
            atw.unit.local.chillerlink_state = TRUE; //组内通信建立完成
            ChillerMainRegularTransmit();   //regular communication  handle
            
        }
        break;
        
        default:
            break;
    }

}


