/***********************************************************************
@file   : ChillerLinkSub.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "ChillerLinkSub.h"
#include "TimerCounter.h"
#include "ChillerLinkData.h"
#include "HlinkDriver.h"




#define TIME_DATA_CHECK            ((uint8_t)20)    //timeout to send frame data_check
#define TIME_CONNECT               ((uint8_t)60)    //frame receive from main chiller time check 
#define TIME_ADDRESS_DUPLICATE     ((uint8_t)30)    //address duplicate sustain check


//time
typedef struct
{
    uint8_t connect;                //check main chiller connect state
    uint8_t addr_duplicate;         //check address duplicate state
    uint8_t regular_connect;
    uint8_t reserved2;

}ChillerLinkSubTimeType;

static ChillerLinkSubTimeType time;


/************************************************************************
@name  	: ChillerSubSendUnitInit
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerSubSendUnitInit(void)
{
    uint8_t ch = HL_CH_CHILLER;
    uint16_t *pdat;
    uint8_t len,i;
    
    pdat = &table[link.unit_no].unit_init.dsw1.word;
    HLTxBuf[ch][2] = 0x30;              //length
    HLTxBuf[ch][7] = 0xA8;              //
    HLTxBuf[ch][8] = 0x21;              //
    HLTxBuf[ch][9] = 0x03;  //address
    HLTxBuf[ch][10] = 0x12; //length

    len = sizeof(UnitTableInitType)/sizeof(uint16_t);
    for (i = 0; i < len; i++)
    {
        HLTxBuf[ch][11+i*2] = (uint8_t)((*(pdat+i)) >> 8);
        HLTxBuf[ch][12+i*2] = (uint8_t)(*(pdat+i));
    }
}

/************************************************************************
@name  	: ChillerSubSendUnitEvent
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerSubSendUnitEvent(void)
{
    uint8_t ch = HL_CH_CHILLER;
    uint16_t *pdat;
    uint8_t len,i;

    pdat = &table[link.unit_no].unit_event.state.word; //
    HLTxBuf[ch][2] = 0x22;              //length
    HLTxBuf[ch][7] = 0xA8;              //
    HLTxBuf[ch][8] = 0x22;              //
    HLTxBuf[ch][9] = 0x00;  //address
    HLTxBuf[ch][10] = 0x0B; //length

    len = sizeof(UnitTableEventType)/sizeof(uint16_t);
    for (i = 0; i < len; i++)
    {
        HLTxBuf[ch][11+i*2] = (uint8_t)((*(pdat+i)) >> 8);
        HLTxBuf[ch][12+i*2] = (uint8_t)(*(pdat+i));
    }

}

/************************************************************************
@name  	: ChillerSubSendUnitEventInform
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerSubSendUnitEventInform(void)
{
    uint8_t ch = HL_CH_CHILLER;

    ChillerSubSendUnitEvent();
    HLTxBuf[ch][7] = 0xA3;
}

/************************************************************************
@name  	: ChillerSubSendUnitIO
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerSubSendUnitIO(void)
{
    uint8_t ch = HL_CH_CHILLER;
    uint16_t *pdat;
    uint8_t len,i;

    pdat = &table[link.unit_no].unit_io.DIO.word; //
    HLTxBuf[ch][2] = 0x2C;              //length
    HLTxBuf[ch][7] = 0xA8;              //
    HLTxBuf[ch][8] = 0x25;              //
    HLTxBuf[ch][9] = 0x00;  //address
    HLTxBuf[ch][10] = 0x10; //length

    len = sizeof(UnitTableIOType)/sizeof(uint16_t);
    for (i = 0; i < len; i++)
    {
        HLTxBuf[ch][11+i*2] = (uint8_t)((*(pdat+i)) >> 8);
        HLTxBuf[ch][12+i*2] = (uint8_t)(*(pdat+i));
    }
}

/************************************************************************
@name  	: ChillerSubSendUnitAlarm1
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerSubSendUnitAlarm1(void)
{
    uint8_t ch = HL_CH_CHILLER;
    uint16_t *pdat;
    uint8_t len,i;

    pdat = &table[link.unit_no].unit_alarmh.total_num;
    HLTxBuf[ch][2] = 0x30;              //length
    HLTxBuf[ch][7] = 0xA8;              //
    HLTxBuf[ch][8] = 0x27;              //
    HLTxBuf[ch][9] = 0x00;  //address
    HLTxBuf[ch][10] = 0x12; //length

    len = 18;
    for (i = 0; i < len; i++)
    {
        HLTxBuf[ch][11+i*2] = (uint8_t)((*(pdat+i)) >> 8);
        HLTxBuf[ch][12+i*2] = (uint8_t)(*(pdat+i));
    }
}

/************************************************************************
@name  	: ChillerSubSendUnitAlarm2
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerSubSendUnitAlarm2(void)
{
    uint8_t ch = HL_CH_CHILLER;
    uint16_t *pdat;
    uint8_t len,i;

    pdat = &table[link.unit_no].unit_alarmh.alarm_history[17];
    HLTxBuf[ch][2] = 0x30;              //length
    HLTxBuf[ch][7] = 0xA8;              //
    HLTxBuf[ch][8] = 0x27;              //
    HLTxBuf[ch][9] = 0x12;  //address
    HLTxBuf[ch][10] = 0x12; //length

    len = 18;
    for (i = 0; i < len; i++)
    {
        HLTxBuf[ch][11+i*2] = (uint8_t)((*(pdat+i)) >> 8);
        HLTxBuf[ch][12+i*2] = (uint8_t)(*(pdat+i));
    }

}

/************************************************************************
@name  	: ChillerSubSendCycleEvent
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerSubSendCycleEvent(uint8_t cycle_ch)
{
    uint8_t tab[CYCLE_CH_MAX] = {0x42,0x52,0x62,0x72};
    uint8_t ch = HL_CH_CHILLER;
    uint16_t *pdat;
    uint8_t len,i;

    pdat = &table[link.unit_no].cycle[cycle_ch].event.state1.word;
    HLTxBuf[ch][2] = 0x16;              //length
    HLTxBuf[ch][7] = 0xA8;              //
    HLTxBuf[ch][8] = tab[cycle_ch];              //
    HLTxBuf[ch][9] = 0x00;  //address
    HLTxBuf[ch][10] = 0x05; //length

    len = sizeof(CycleTableEventType)/sizeof(uint16_t);
    for (i = 0; i < len; i++)
    {
        HLTxBuf[ch][11+i*2] = (uint8_t)((*(pdat+i)) >> 8);
        HLTxBuf[ch][12+i*2] = (uint8_t)(*(pdat+i));
    }

}

/************************************************************************
@name  	: ChillerSubSendCycleIO1
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerSubSendCycleIO1(uint8_t cycle_ch)
{
    uint8_t tab[CYCLE_CH_MAX] = {0x45,0x55,0x65,0x75};
    uint8_t ch = HL_CH_CHILLER;
    uint16_t *pdat;
    uint8_t len,i;

    pdat = &table[link.unit_no].cycle[cycle_ch].io.io1.word;
    HLTxBuf[ch][2] = 0x30;              //length
    HLTxBuf[ch][7] = 0xA8;              //
    HLTxBuf[ch][8] = tab[cycle_ch];              //
    HLTxBuf[ch][9] = 0x00;  //address
    HLTxBuf[ch][10] = 0x12; //length

    // len = sizeof(CycleTableIOType)/sizeof(uint16_t);
    len = 18;
    for (i = 0; i < len; i++)
    {
        HLTxBuf[ch][11+i*2] = (uint8_t)((*(pdat+i)) >> 8);
        HLTxBuf[ch][12+i*2] = (uint8_t)(*(pdat+i));
    }

}

/************************************************************************
@name  	: ChillerSubSendCycleIO2
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerSubSendCycleIO2(uint8_t cycle_ch)
{
    uint8_t tab[CYCLE_CH_MAX] = {0x45,0x55,0x65,0x75};
    uint8_t ch = HL_CH_CHILLER;
    uint16_t *pdat;
    uint8_t len,i;

    pdat = &table[link.unit_no].cycle[cycle_ch].io.fan2_rt_freq;
    HLTxBuf[ch][2] = 0x2C;              //length
    HLTxBuf[ch][7] = 0xA8;              //
    HLTxBuf[ch][8] = tab[cycle_ch];              //
    HLTxBuf[ch][9] = 0x12;  //address
    HLTxBuf[ch][10] = 0x10; //length

    len = 14;
    for (i = 0; i < len; i++)
    {
        HLTxBuf[ch][11+i*2] = (uint8_t)((*(pdat+i)) >> 8);
        HLTxBuf[ch][12+i*2] = (uint8_t)(*(pdat+i));
    }
}

/************************************************************************
@name  	: ChillerSubSendCycleInitSum
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerSubSendCycleInitSum(uint8_t cycle_ch)
{
    uint8_t tab[CYCLE_CH_MAX] = {0x44,0x54,0x64,0x74};
    uint8_t ch = HL_CH_CHILLER;
    CycleTableInitSumType *pInitsum; 

    pInitsum = &table[link.unit_no].cycle[cycle_ch].init_sum;
    HLTxBuf[ch][2] = 0x20;              //length
    HLTxBuf[ch][7] = 0xA8;              //
    HLTxBuf[ch][8] = tab[cycle_ch];     //
    HLTxBuf[ch][9] = 0x00;  //address
    HLTxBuf[ch][10] = 0x0A; //length

    HLTxBuf[ch][11] = (uint8_t)(pInitsum->comp_romNO >> 8);
    HLTxBuf[ch][12] = (uint8_t)(pInitsum->comp_romNO);
    HLTxBuf[ch][13] = (uint8_t)(pInitsum->fan_romNO >> 8);
    HLTxBuf[ch][14] = (uint8_t)(pInitsum->fan_romNO);
    HLTxBuf[ch][15] = (uint8_t)(pInitsum->comp_run_time_sum >> 24);
    HLTxBuf[ch][16] = (uint8_t)(pInitsum->comp_run_time_sum >> 16);
    HLTxBuf[ch][17] = (uint8_t)(pInitsum->comp_run_time_sum >> 8);
    HLTxBuf[ch][18] = (uint8_t)(pInitsum->comp_run_time_sum );
    HLTxBuf[ch][19] = (uint8_t)(pInitsum->comp_power_sum >> 24);
    HLTxBuf[ch][20] = (uint8_t)(pInitsum->comp_power_sum >> 16);
    HLTxBuf[ch][21] = (uint8_t)(pInitsum->comp_power_sum >> 8);
    HLTxBuf[ch][22] = (uint8_t)(pInitsum->comp_power_sum );
    HLTxBuf[ch][23] = (uint8_t)(pInitsum->sys_run_time_sum >> 24);
    HLTxBuf[ch][24] = (uint8_t)(pInitsum->sys_run_time_sum >> 16);
    HLTxBuf[ch][25] = (uint8_t)(pInitsum->sys_run_time_sum >> 8);
    HLTxBuf[ch][26] = (uint8_t)(pInitsum->sys_run_time_sum );

}

/************************************************************************
@name  	: ChillerSubSendCheckData
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerSubSendCheckData(void)
{
    uint8_t ch = HL_CH_CHILLER;

    HLTxBuf[ch][2] = 0x09;      //length
    HLTxBuf[ch][7] = 0xA1;      //

}

/************************************************************************
@name  	: ChillerLinkSendData
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerLinkSendData(uint8_t update_type)
{
    uint8_t ch = HL_CH_CHILLER;

    HLTxBuf[ch][0] = HL_SUB_TO_MAIN; //id code
    HLTxBuf[ch][1] = 0x00;           //type code
    // HLTxBuf[ch][2] = 0x0C;        //length
    HLTxBuf[ch][3] = link.sys_no;    //src_sys
    HLTxBuf[ch][4] = link.unit_no;  //src_addr
    HLTxBuf[ch][5] = link.dst_sys;  //dst_sys
    HLTxBuf[ch][6] = 0x00;          //dst_addr
    HLTxBuf[ch][7] = 0xA8;          //

    switch (update_type)
    {
        case UPDATE_UNIT_INIT:          ChillerSubSendUnitInit();           break;
        case UPDATE_UNIT_EVENT:         ChillerSubSendUnitEvent();          break;
        case UPDATE_UNIT_EVENT_INFORM:  ChillerSubSendUnitEventInform();    break;
        case UPDATE_UNIT_IO:            ChillerSubSendUnitIO();             break;
        case UPDATE_UNIT_ALARM1:        ChillerSubSendUnitAlarm1();         break;
        case UPDATE_UNIT_ALARM2:        ChillerSubSendUnitAlarm2();         break;

        case UPDATE_CYCLE1_EVENT:       ChillerSubSendCycleEvent(CYCLE_1);  break;
        case UPDATE_CYCLE1_IO1:         ChillerSubSendCycleIO1(CYCLE_1);    break;
        case UPDATE_CYCLE1_IO2:         ChillerSubSendCycleIO2(CYCLE_1);    break;
        case UPDATE_CYCLE1_INIT_SUM:    ChillerSubSendCycleInitSum(CYCLE_1);break;

        case UPDATE_CYCLE2_EVENT:       ChillerSubSendCycleEvent(CYCLE_2);  break;
        case UPDATE_CYCLE2_IO1:         ChillerSubSendCycleIO1(CYCLE_2);    break;
        case UPDATE_CYCLE2_IO2:         ChillerSubSendCycleIO2(CYCLE_2);    break;
        case UPDATE_CYCLE2_INIT_SUM:    ChillerSubSendCycleInitSum(CYCLE_2);break;

        case UPDATE_CYCLE3_EVENT:       ChillerSubSendCycleEvent(CYCLE_3);  break;
        case UPDATE_CYCLE3_IO1:         ChillerSubSendCycleIO1(CYCLE_3);    break;
        case UPDATE_CYCLE3_IO2:         ChillerSubSendCycleIO2(CYCLE_3);    break;
        case UPDATE_CYCLE3_INIT_SUM:    ChillerSubSendCycleInitSum(CYCLE_3);break;

        case UPDATE_CYCLE4_EVENT:       ChillerSubSendCycleEvent(CYCLE_4);  break;
        case UPDATE_CYCLE4_IO1:         ChillerSubSendCycleIO1(CYCLE_4);    break;
        case UPDATE_CYCLE4_IO2:         ChillerSubSendCycleIO2(CYCLE_4);    break;
        case UPDATE_CYCLE4_INIT_SUM:    ChillerSubSendCycleInitSum(CYCLE_4);break;

        case UPDATE_CHECK_DATA:         ChillerSubSendCheckData();break;
        default:
            break;
    }
}
//==== sub send end 


/************************************************************************
@name  	: ChillerSubRecvUnitCmdInform
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerSubRecvUnitCmdInform(uint8_t *buf)
{
    UnitTableCmdType *pCmd; 
    uint8_t unit_no = buf[4];

    pCmd = &table[unit_no].unit_cmd;
    if ((buf[9] == 0x00) && (buf[10] == 0x12))//cmd1
    {//get data
        pCmd->data1.word = ((uint16_t)buf[11] << 8) | buf[12];

    }
    else if ((buf[9] == 0x12) && (buf[10] == 0x12))//cmd2
    {
        pCmd->exv_cool_init_step = ((uint16_t)buf[11] << 8) | buf[12];
    }

}

/************************************************************************
@name  	: ChillerSubRecvDataReq
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerSubRecvDataReq(uint8_t *buf)
{
    uint8_t ch = link.unit_no;

#ifdef UNIT_MULTI
    ch = buf[6];
#endif

    switch (buf[8])
    {
//unit        
        case 0x21:// 
        {
            if ((buf[9] == 0x03) && (buf[10] == 0x12))
            {
#ifdef UNIT_MULTI
            if (buf[6] == 0xFF)
                for ( ch = UNIT_SLAVE_START; ch <= UNIT_SALVE_END; ch++)
                    ChillerlinkUpdateData(ch,UPDATE_UNIT_INIT);
            else        
#endif                
                ChillerlinkUpdateData(ch,UPDATE_UNIT_INIT);
            }
        }
            break;
        case 0x22: 
        {
            if ((buf[9] == 0x00) && (buf[10] == 0x0B))
            {
                ChillerlinkUpdateData(ch,UPDATE_UNIT_EVENT);
            }
        }
            break;
        // case 0x23: 
        // {
        //     ChillerSubRecvUnitCmdInform(buf);             
        // }
        //     break;
        case 0x25: 
        {
            if ((buf[9] == 0x00) && (buf[10] == 0x10))
            {
                ChillerlinkUpdateData(ch,UPDATE_UNIT_IO);
            }
        }              
            break;
        case 0x27: 
        {
            if ((buf[9] == 0x00) && (buf[10] == 0x12))
            {
                ChillerlinkUpdateData(ch,UPDATE_UNIT_ALARM1);
            }
            else if ((buf[9] == 0x12) && (buf[10] == 0x12))
            {
                ChillerlinkUpdateData(ch,UPDATE_UNIT_ALARM2);
            }
        }              
            break;
//cycle1
        case 0x42: 
        {
            if ((buf[9] == 0x00) && (buf[10] == 0x05))
            {
                ChillerlinkUpdateData(ch,UPDATE_CYCLE1_EVENT);
            }
        }              
            break;
        case 0x44: 
        {
            if ((buf[9] == 0x00) && (buf[10] == 0x0A))
            {
                ChillerlinkUpdateData(ch,UPDATE_CYCLE1_INIT_SUM);
            }
        }              
            break;
        case 0x45: 
        {
            if ((buf[9] == 0x00) && (buf[10] == 0x12))
            {
                ChillerlinkUpdateData(ch,UPDATE_CYCLE1_IO1);
            }
            else if ((buf[9] == 0x12) && (buf[10] == 0x10))
            {
                ChillerlinkUpdateData(ch,UPDATE_CYCLE1_IO2);
            }
        }              
            break;
//cycle2
        case 0x52: 
        {
            if ((buf[9] == 0x00) && (buf[10] == 0x05))
            {
                ChillerlinkUpdateData(ch,UPDATE_CYCLE2_EVENT);
            }
        }              
            break;
        case 0x54: 
        {
            if ((buf[9] == 0x00) && (buf[10] == 0x0A))
            {
                ChillerlinkUpdateData(ch,UPDATE_CYCLE2_INIT_SUM);
            }
        }              
            break;
        case 0x55: 
        {
            if ((buf[9] == 0x00) && (buf[10] == 0x12))
            {
                ChillerlinkUpdateData(ch,UPDATE_CYCLE2_IO1);
            }
            else if ((buf[9] == 0x12) && (buf[10] == 0x10))
            {
                ChillerlinkUpdateData(ch,UPDATE_CYCLE2_IO2);
            }
        }              
            break;
//cycle3
        case 0x62: 
        {
            if ((buf[9] == 0x00) && (buf[10] == 0x05))
            {
                ChillerlinkUpdateData(ch,UPDATE_CYCLE3_EVENT);
            }
        }              
            break;
        case 0x64: 
        {
            if ((buf[9] == 0x00) && (buf[10] == 0x0A))
            {
                ChillerlinkUpdateData(ch,UPDATE_CYCLE3_INIT_SUM);
            }
        }              
            break;
        case 0x65: 
        {
            if ((buf[9] == 0x00) && (buf[10] == 0x12))
            {
                ChillerlinkUpdateData(ch,UPDATE_CYCLE3_IO1);
            }
            else if ((buf[9] == 0x12) && (buf[10] == 0x10))
            {
                ChillerlinkUpdateData(ch,UPDATE_CYCLE3_IO2);
            }
        }              
            break;
//cycle4
        case 0x72: 
        {
            if ((buf[9] == 0x00) && (buf[10] == 0x05))
            {
                ChillerlinkUpdateData(ch,UPDATE_CYCLE4_EVENT);
            }
        }              
            break;
        case 0x74: 
        {
            if ((buf[9] == 0x00) && (buf[10] == 0x0A))
            {
                ChillerlinkUpdateData(ch,UPDATE_CYCLE4_INIT_SUM);
            }
        }              
            break;
        case 0x75: 
        {
            if ((buf[9] == 0x00) && (buf[10] == 0x12))
            {
                ChillerlinkUpdateData(ch,UPDATE_CYCLE4_IO1);
            }
            else if ((buf[9] == 0x12) && (buf[10] == 0x10))
            {
                ChillerlinkUpdateData(ch,UPDATE_CYCLE4_IO2);
            }
        }              
            break;

        default:    break;
    }
}
//==== sub receive end


/************************************************************************
@name  	: ChillerLinkMainTimerCounter
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerLinkSubTimerCounter(void)
{
    uint8_t *pTime;
    uint16_t i,len;

    pTime = &time.connect;
    len = sizeof(ChillerLinkSubTimeType)/sizeof(uint8_t);
    if (TimerCheck(NO_CHILLER_LINK_TIME_BASE) == TRUE)
    {
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
@name  	: ChillerSubInit
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ChillerLinkSubInit(void)
{
    ChillerLinkSetDefaultParameters();
    time.connect = TIME_CONNECT;
    time.regular_connect = TIME_DATA_CHECK;
}

/************************************************************************
@name  	: ChillerReceiveExecute
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ChillerSubReceive(uint8_t *buf)
{
    uint8_t src_unit_no = buf[4];

#ifdef UNIT_MULTI
    if ((src_unit_no == CHILLER_MAIN_ADDR) && (buf[6] != 0) && ((buf[6] < 32 ) || (buf[6] == 0xFF)))
    {
        link.unit_state[buf[6]].bit.connect = TRUE;
#else
    if ((src_unit_no == CHILLER_MAIN_ADDR) && ((buf[6] == link.unit_no) || (buf[6] == 0xFF))) //from CHILLER_0,and to local unit or broadcast
    {
        link.unit_state[link.unit_no].bit.connect_check = TRUE;
        link.unit_state[link.unit_no].bit.connect = TRUE;
#endif    
        time.connect = TIME_CONNECT;            //reset time counter
        time.regular_connect = TIME_DATA_CHECK;
        atw.unit.local.chillerlink_state = TRUE; //组内通信建立
        switch (buf[7])
        {
            case 0xA7://data table
            {
                ChillerSubRecvDataReq(buf);
            }
            break;
            case 0xA3://data table
            {
                if (buf[8] == 0x23)
                {
                    ChillerSubRecvUnitCmdInform(buf);             
                }
            }
            default:
                break;
        }
    }
    else
    {
        if (src_unit_no == link.unit_no) //address duplicate exist
        {
            link.addr_duplicate_cnt++;
        }
        if ((time.addr_duplicate == 0) && (link.addr_duplicate_cnt > 3))
        {
            time.addr_duplicate = TIME_ADDRESS_DUPLICATE;
            link.addr_duplicate_state |= 1<<(src_unit_no);
        }
    }
}

/************************************************************************
@name  	: ChillerSubTransmit
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ChillerSubTransmit(void)
{
    uint8_t ch = link.unit_no;
    uint8_t tx_status;
    uint32_t i;

    tx_status = HLCheckTxStatus(HL_CH_CHILLER);
    if (tx_status == HL_SUCCESS)
    {
#ifdef UNIT_MULTI        
        for ( ch = UNIT_SLAVE_START; ch <= UNIT_SALVE_END; ch++)
        {
            link.unit_no = ch;
#endif
            if (update[ch].dat.data != NULL)
            {
                for ( i = 0; i < UPDATE_DATA_NO_MAX; i++)
                {
                    if (update[ch].dat.data & (1<<i))
                    {
                        update[ch].dat.data &= (uint32_t)(~(1<<i));
                        ChillerLinkSendData(i);
                        HLSend(HL_CH_CHILLER);
                        return;
                    }
                }
            }
#ifdef UNIT_MULTI  
        }
#endif
    }
    else if (tx_status == HL_FAIL)
    {
        HLClearSendStatus(HL_CH_CHILLER);
    }
}

/************************************************************************
@name  	: ChillerSubTimerHandle
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ChillerSubTimerHandle(void)
{
    ChillerLinkSubTimerCounter();
    if (time.connect == 0)//offline check
    {
        time.connect = TIME_CONNECT;
        link.unit_state[link.unit_no].bit.connect = FALSE;//
    }
    
    if (link.unit_state[link.unit_no].bit.connect_check == TRUE)
    {
        if (time.regular_connect == 0)//send connect frame every 20s
        {
            time.regular_connect = TIME_DATA_CHECK;
            ChillerlinkUpdateData(link.unit_no,UPDATE_CHECK_DATA);
        }
    }
}

