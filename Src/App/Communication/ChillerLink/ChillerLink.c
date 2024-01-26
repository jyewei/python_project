/***********************************************************************
@file   : ChillerLink.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "ChillerLink.h"
#include "HlinkDriver.h"
#include "safety_function.h"
#include "Data.h"
#include "TimerCounter.h"
#include "ChillerLinkMain.h"
#include "ChillerLinkSub.h"
#include "ChillerLinkData.h"
#include "IOProcess.h"





LinkQueueType recv_queue;










/************************************************************************
@name  	: ChillerLinkQueueSet
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerLinkQueueSet(void)
{
    uint8_t i,last;
    uint8_t ch = HL_CH_CHILLER;

    if ((recv_queue.last == recv_queue.first) && (recv_queue.empty == FALSE))
    {
        return;
    }
    
    last = recv_queue.last;
    for ( i = 0; i < MAX_BUF_LEN; i++)
    {
        recv_queue.buf[last][i] = HLRxBuf[ch][i];
    }
    recv_queue.last = (last + 1) % MAX_BUF_NO;
    recv_queue.empty = FALSE;

}

/************************************************************************
@name  	: ChillerLinkQueueGet
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerLinkQueueGet(uint8_t *recv_buf)
{    
    uint8_t first,i;

    first = recv_queue.first;
    for ( i = 0; i < MAX_BUF_LEN; i++)
    {
        recv_buf[i] = recv_queue.buf[first][i];
    }
    recv_queue.first = (first + 1) % MAX_BUF_NO;;
    if (recv_queue.first == recv_queue.last)
    {
        recv_queue.empty = TRUE;
    }
}


/************************************************************************
@name  	: ChillerLinkParamInit
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerLinkParamInit(void)
{
    recv_queue.empty = TRUE;
    recv_queue.first = 0;
    recv_queue.last = 0;

    link.sys_no = 0;//GetLocalSystemNO();       //
#ifdef UNIT_MULTI
    link.unit_no = UNIT_SLAVE_START;
#else    
    link.unit_no = GetLocalAddress();       //
#endif
    link.addr_duplicate_state = FALSE;      //
    link.comm_status = LINK_STATUS_INIT;  //

    link.pre_recv_addr = 0;
    link.recv_addr_act = 0;
    link.pre_recv_cnt = 0;
    link.recv_cnt_act = 0;

    ChillerLinkSetDefaultParameters();
    if (link.unit_no == CHILLER_MAIN_ADDR) //
    {
        ChillerLinkMainInit();
    }
    else//chiller sub
    {
        ChillerLinkSubInit();
    }
}

/************************************************************************
@name  	: ChillerlinkDataHandle
@brief 	: 
@param 	: None
@return	: None
@note   : 更新通信用的数据表
            1.若为0号机,将系统参数更新到table种,1~31号不需要更新,由通信接收而来
            2.将table内的cmd数据更新到本地unit的cmd数据内
*************************************************************************/
static void ChillerlinkDataHandle(void)
{
    uint8_t addr = link.unit_no;
    uint8_t ch;

//table参数
    //系统参数
    if (addr == CHILLER_MAIN_ADDR)
    {
        for (ch = CHILLER_0; ch < CHILLER_CH_MAX; ch++)//主机需要将系统处理后的cmd指令更新到table的cmd中
        {
            table[ch].unit_cmd = atw.sys.table.unit[ch].cmd;
        }
    }
    //单元参数   
    atw.unit.table.sys_cmd = table[addr].unit_cmd;  //将table内的cmd指令更新到本地unit的cmd中
    table[addr].unit_init = atw.unit.table.init;    //将unit本地数据更新到table中
    table[addr].unit_event = atw.unit.table.event;
    table[addr].unit_io = atw.unit.table.io;
    table[addr].unit_alarmh = atw.unit.table.alarmh;
    //cycle参数
    for (ch = CYCLE_1; ch < CYCLE_CH_MAX; ch++)
    {
        table[addr].cycle[ch].event = GetTableCyleEvent(ch);
        table[addr].cycle[ch].io = GetTableCyleIO(ch);
        table[addr].cycle[ch].init_sum = GetTableCyleInitSum(ch);
    }
//其它参数
    //单元参数
    atw.unit.local.chillerlink_state = link.unit_state[link.unit_no].bit.connect;


}

/************************************************************************
@name  	: ChillerLinkInit
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
static void ChillerLinkInit(void)
{
    uint8_t ch = HL_CH_CHILLER;

//state init
    ChillerLinkParamInit();
//hlink init
    HLinkInit(ch,OFF);
    HLSetAckOpt(ch,0);
    HLSetIDTable(ch,HL_MAIN_TO_SUB);
    HLSetIDTable(ch,HL_SUB_TO_MAIN);
    HLSetAckTable(ch,HL_MAIN_TO_SUB,link.sys_no,link.unit_no);
    HLSetRcvOK(ch);

#ifdef UNIT_MULTI
    for ( ch = UNIT_SLAVE_START; ch <= UNIT_SALVE_END; ch++)
    {
        HLSetAckTable(HL_CH_CHILLER,HL_MAIN_TO_SUB,0,ch);
    }
#endif
}

/************************************************************************
@name  	: ChillerLink
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ChillerLink(uint8_t taskNo,uint8_t flag_init)
{
    uint8_t ch = HL_CH_CHILLER;
    uint8_t recv_buf[MAX_BUF_LEN];

    SFSetRunTaskNo( taskNo );
    ChillerlinkDataHandle();
    if (flag_init == FALSE)
    {
        ChillerLinkInit();
    }
//==receive    
    //receive data,put in queue  
    if (HLCheckRecvData(ch) == TRUE)//
    {
        ChillerLinkQueueSet();
        HLSetRcvOK(ch);
    }
    //frame handle
    if (recv_queue.empty == FALSE)
    {
        ChillerLinkQueueGet(recv_buf);
        if (link.unit_no == CHILLER_MAIN_ADDR) //chiller main
        {
            ChillerMainReceive(recv_buf);
        }
        else//chiller sub
        {
            ChillerSubReceive(recv_buf);
        }
    }
//==transmit   
    if (link.unit_no == CHILLER_MAIN_ADDR) //chiller main
    {
        ChillerMainTransmit();
        ChillerMainTimerHandle();   //timer handle
    }
    else//chiller sub
    {
        ChillerSubTransmit();
        ChillerSubTimerHandle();    //timer handle
    }
    HLSendFirstByte(HL_CH_CHILLER);
}
