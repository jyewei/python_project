/***********************************************************************
@file   : ChillerLinkData.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
************************************************************************/
#include "ChillerLinkData.h"





ChillerLinkDataLocalType link;

ChillerLinkDataTableType table[CHILLER_CH_MAX];
ChillerLinkUpdate update[CHILLER_CH_MAX];






/************************************************************************
@name  	: ChillerLinkSetDefaultParameters
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ChillerLinkSetDefaultParameters(void)
{
    uint8_t ch;
//TODO for test //just set first parameter
    for (ch = CHILLER_0; ch < CHILLER_CH_MAX; ch++)
    {
        table[ch].unit_init.dsw1.word = 0xA001;
        table[ch].unit_init.reserved17 = 0xA010;

        table[ch].unit_event.state.word = 0xA002;
        table[ch].unit_io.DIO.word = 0xA003;
        table[ch].unit_alarmh.total_num = 0xA004;
        table[ch].unit_cmd.data1.word = 0xA005;

        table[ch].cycle[CYCLE_1].event.state1.word = 0xB101;
        table[ch].cycle[CYCLE_1].io.io1.word = 0xB102;
        table[ch].cycle[CYCLE_1].io.fan2_rt_freq = 0xB103;
        table[ch].cycle[CYCLE_1].init_sum.comp_romNO = 0xB104;

        table[ch].cycle[CYCLE_2].event.state1.word = 0xB201;
        table[ch].cycle[CYCLE_2].io.io1.word = 0xB202;
        table[ch].cycle[CYCLE_2].io.fan2_rt_freq = 0xB203;
        table[ch].cycle[CYCLE_2].init_sum.comp_romNO = 0xB204;

        table[ch].cycle[CYCLE_3].event.state1.word = 0xB301;
        table[ch].cycle[CYCLE_3].io.io1.word = 0xB302;
        table[ch].cycle[CYCLE_3].io.fan2_rt_freq = 0xB303;
        table[ch].cycle[CYCLE_3].init_sum.comp_romNO = 0xB304;

        table[ch].cycle[CYCLE_4].event.state1.word = 0xB401;
        table[ch].cycle[CYCLE_4].io.io1.word = 0xB402;
        table[ch].cycle[CYCLE_4].io.fan2_rt_freq = 0xB403;
        table[ch].cycle[CYCLE_4].init_sum.comp_romNO = 0xB404;

    }
    
}


/************************************************************************
@name  	: ChillerlinkUpdateRequest
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ChillerlinkUpdateRequest(uint8_t ch,uint8_t update_type)
{
    if (ch == 0xFF) //broadcast
    {
        ch = CHILLER_0;//main chiller will not send data to CHILLER_0,so address CHILLER_0 as broadcast address
    }
    
    if (ch < CHILLER_CH_MAX)
    {
        switch (update_type)
        {
        case UPDATE_UNIT_INIT_REQ:      update[ch].req.bit.unit_init_req = 1;       break;
        case UPDATE_UNIT_EVENT_REQ:     update[ch].req.bit.unit_event_req = 1;      break;
        case UPDATE_UNIT_IO_REQ:        update[ch].req.bit.unit_io_req = 1;         break;
        case UPDATE_UNIT_ALARM1_REQ:    update[ch].req.bit.unit_alarm1_req = 1;     break;
        case UPDATE_UNIT_ALARM2_REQ:    update[ch].req.bit.unit_alarm2_req = 1;     break;
		case UPDATE_UNIT_CMD1_INFORM:   update[ch].req.bit.unit_cmd1_inform = 1;    break;
		case UPDATE_UNIT_CMD2_INFORM:   update[ch].req.bit.unit_cmd2_inform = 1;    break;
		
        case UPDATE_CYCLE1_EVENT_REQ:   update[ch].req.bit.cycle1_event_req = 1;    break;
        case UPDATE_CYCLE1_IO1_REQ:     update[ch].req.bit.cycle1_io1_req = 1;      break;
        case UPDATE_CYCLE1_IO2_REQ:     update[ch].req.bit.cycle1_io2_req = 1;      break;
		case UPDATE_CYCLE1_INIT_SUM_REQ:update[ch].req.bit.cycle1_init_sum_req = 1; break;
		
		case UPDATE_CYCLE2_EVENT_REQ:   update[ch].req.bit.cycle2_event_req = 1;    break;
        case UPDATE_CYCLE2_IO1_REQ:     update[ch].req.bit.cycle2_io1_req = 1;      break;
        case UPDATE_CYCLE2_IO2_REQ:     update[ch].req.bit.cycle2_io2_req = 1;      break;
		case UPDATE_CYCLE2_INIT_SUM_REQ:update[ch].req.bit.cycle2_init_sum_req = 1; break;
		
		case UPDATE_CYCLE3_EVENT_REQ:   update[ch].req.bit.cycle3_event_req = 1;    break;
        case UPDATE_CYCLE3_IO1_REQ:     update[ch].req.bit.cycle3_io1_req = 1;      break;
        case UPDATE_CYCLE3_IO2_REQ:     update[ch].req.bit.cycle3_io2_req = 1;      break;
		case UPDATE_CYCLE3_INIT_SUM_REQ:update[ch].req.bit.cycle3_init_sum_req = 1; break;
		
		case UPDATE_CYCLE4_EVENT_REQ:   update[ch].req.bit.cycle4_event_req = 1;    break;
        case UPDATE_CYCLE4_IO1_REQ:     update[ch].req.bit.cycle4_io1_req = 1;      break;
        case UPDATE_CYCLE4_IO2_REQ:     update[ch].req.bit.cycle4_io2_req = 1;      break;
		case UPDATE_CYCLE4_INIT_SUM_REQ:update[ch].req.bit.cycle4_init_sum_req = 1; break;

        default:
            break;
        }
    //check
        // if (ch != CHILLER_0)
        // {
        //     if (link.unit_state[ch].bit.cycle2_enable == FALSE)//if cycle2 is disable,not send date
        //     {
        //         update[ch].req.data &= ~0x0000F000;
        //         // update[ch].req.bit.cycle2_event_req = 0;  //bit12~15
        //         // update[ch].req.bit.cycle2_io1_req = 0;
        //         // update[ch].req.bit.cycle2_io2_req = 0;
        //         // update[ch].req.bit.cycle2_init_sum_req = 0;
        //     }
        //     if (link.unit_state[ch].bit.cycle3_enable == FALSE)//if cycle3 is disable,not send date
        //     {
        //         update[ch].req.data &= ~0x000F0000;
        //         // update[ch].req.bit.cycle3_event_req = 0;//bit16~19
        //         // update[ch].req.bit.cycle3_io1_req = 0;
        //         // update[ch].req.bit.cycle3_io2_req = 0;
        //         // update[ch].req.bit.cycle3_init_sum_req = 0;
        //     }
        //     if (link.unit_state[ch].bit.cycle4_enable == FALSE)//if cycle4 is disable,not send date
        //     {
        //         update[ch].req.data &= ~0x00F00000;
        //         // update[ch].req.bit.cycle4_event_req = 0;//bit20~23
        //         // update[ch].req.bit.cycle4_io1_req = 0;
        //         // update[ch].req.bit.cycle4_io2_req = 0;
        //         // update[ch].req.bit.cycle4_init_sum_req = 0;
        //     }
        // }
    }
}

/************************************************************************
@name  	: ChillerlinkUpdateData
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void ChillerlinkUpdateData(uint8_t ch,uint8_t update_type)
{
    switch (update_type)
    {
        case UPDATE_UNIT_INIT:      	update[ch].dat.bit.unit_init = 1;       	break;
        case UPDATE_UNIT_EVENT:     	update[ch].dat.bit.unit_event = 1;      	break;
        case UPDATE_UNIT_EVENT_INFORM:	update[ch].dat.bit.unit_event_inform = 1;	break;
        case UPDATE_UNIT_IO:        	update[ch].dat.bit.unit_io = 1;         	break;
        case UPDATE_UNIT_ALARM1:    	update[ch].dat.bit.unit_alarm1 = 1;     	break;
        case UPDATE_UNIT_ALARM2:    	update[ch].dat.bit.unit_alarm2 = 1;     	break;
        
        case UPDATE_CYCLE1_EVENT:   	update[ch].dat.bit.cycle1_event = 1;    	break;
        case UPDATE_CYCLE1_IO1:     	update[ch].dat.bit.cycle1_io1 = 1;      	break;
        case UPDATE_CYCLE1_IO2:     	update[ch].dat.bit.cycle1_io2 = 1;      	break;
        case UPDATE_CYCLE1_INIT_SUM:    update[ch].dat.bit.cycle1_init_sum = 1;     break;

        case UPDATE_CYCLE2_EVENT:   	update[ch].dat.bit.cycle2_event = 1;    	break;
        case UPDATE_CYCLE2_IO1:     	update[ch].dat.bit.cycle2_io1 = 1;      	break;
        case UPDATE_CYCLE2_IO2:     	update[ch].dat.bit.cycle2_io2 = 1;      	break;
        case UPDATE_CYCLE2_INIT_SUM:    update[ch].dat.bit.cycle2_init_sum = 1;     break;
        
        case UPDATE_CYCLE3_EVENT:   	update[ch].dat.bit.cycle3_event = 1;    	break;
        case UPDATE_CYCLE3_IO1:     	update[ch].dat.bit.cycle3_io1 = 1;      	break;
        case UPDATE_CYCLE3_IO2:     	update[ch].dat.bit.cycle3_io2 = 1;      	break;
        case UPDATE_CYCLE3_INIT_SUM:    update[ch].dat.bit.cycle3_init_sum = 1;     break;
        
        case UPDATE_CYCLE4_EVENT:   	update[ch].dat.bit.cycle4_event = 1;    	break;
        case UPDATE_CYCLE4_IO1:     	update[ch].dat.bit.cycle4_io1 = 1;      	break;
        case UPDATE_CYCLE4_IO2:     	update[ch].dat.bit.cycle4_io2 = 1;      	break;
        case UPDATE_CYCLE4_INIT_SUM:    update[ch].dat.bit.cycle4_init_sum = 1;     break;

        case UPDATE_CHECK_DATA:         update[ch].dat.bit.check_data = 1;          break;
        
        default:
            break;
    }
}













