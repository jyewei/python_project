/***********************************************************************
@file   : Data.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
@note   : data内的数据用于sys数据处理
          data_unit内的数据用于local unit的数据处理
          DataProcess用于将local unit的数据更新到sys table数据中
************************************************************************/
#include "Data.h"
#include "safety_function.h"
#include "TimerCounter.h"
#include "IOProcess.h"
#include "config.h"



AtwParamType atw;




/************************************************************************
@name  	: SysTwsysoutUpdate
@brief 	: 
@param 	: None
@return	: None
@note   : 3.2
            1-1)Tw_sys_out赋值
            1.如果只有1台单元,不安装总出水温度传感器,也不检测0#单元总出水温度,将0#单元的出水温度赋值给总出水温度
            2.单元数量>1,则为0#单元上的总出水温度
            系统总出水温度Tw_sys_out = 总出水温度传感器检测值(3s) +[总出水传感器补偿设定]
*************************************************************************/
int16_t SysTwsysoutUpdate(void)
{
    if (atw.sys.local.unit_active_cnt > 1)//多台单元
    {
        atw.sys.local.Tw_sys_out = atw.sys.table.unit[CHILLER_0].io.Tw_sys_out + atw.sys.param.Tw_out_fix;
    }
    else
    {
        atw.sys.local.Tw_sys_out = atw.sys.table.unit[CHILLER_0].io.Tw_out + atw.sys.param.Tw_out_fix;
    }
    
    return atw.sys.local.Tw_sys_out;
}

/************************************************************************
@name  	: SysTwsysinUpdate
@brief 	: 
@param 	: None
@return	: None
@note   : 3.2.1
            1-2)Tw_sys_in赋值
            Tw_sys_in = 所有单元回水温度Tw_in(有效值)的平均值
            有效值:
                如果单元运转(制冷/制热/水泵),采用正在运转的单元回水温度平均值
                否则,采用具备以下条件单元回水温度平均值
                    1.单元回水温度传感器正常
                    2.单元无通讯故障
                    3.无禁止运转故障
*************************************************************************/
int16_t SysTwsysinUpdate(void)
{
    uint16_t ch;
    uint16_t cnt = 0,cnt_run = 0,cnt_normal;
    int32_t  sum = 0,sum_run = 0,sum_normal;

    if (atw.sys.local.unit_active_cnt > 1)//多台单元
    {
        for (ch = CHILLER_0; ch < CHILLER_CH_MAX; ch++)
        {
            sum += atw.sys.table.unit[ch].io.Tw_in + atw.sys.table.unit[ch].cmd.Tw_in_fix;//回水温度传感器+[回水传感器补偿设定]
            cnt++;
            if ((atw.sys.local.unit[ch].active == TRUE))//单元有效
            {
                if (atw.sys.table.unit[ch].event.state.bit.unit_run_state == TRUE) //单元运转中
                {
                    sum_run += atw.sys.table.unit[ch].io.Tw_in + atw.sys.table.unit[ch].cmd.Tw_in_fix;
                    cnt_run++;
                }
                else if ((((atw.sys.table.unit[ch].io.Tw_in > -400)) && (atw.sys.table.unit[ch].io.Tw_in < 800))//1.传感器正常
                        && ((atw.sys.local.unit[ch].link_state) == TRUE)//通信正常
                        && (atw.sys.table.unit[ch].event.state.bit.alarm_state == FALSE))//本单元无禁止运转故障
                {
                    sum_normal += atw.sys.table.unit[ch].io.Tw_in + atw.sys.table.unit[ch].cmd.Tw_in_fix;
                    cnt_normal++;
                }
            }            
        }
    }
    else//单台
    {
    
    }
//get value    
    if (cnt_run > 0)
    {
        atw.sys.local.Tw_sys_in = sum_run/cnt_run;
    }
    else if (cnt_normal > 0)
    {
        atw.sys.local.Tw_sys_in = sum_normal/cnt_normal;
    }
    else
    {
        if (cnt)//TODO
        {
            atw.sys.local.Tw_sys_in = sum/cnt;
        }
    }

    return atw.sys.local.Tw_sys_in;
}


/************************************************************************
@name  	: SysTwUpdate
@brief 	: 
@param 	: None
@return	: None
@note   : 3.2.1
            2)实际水温Tw,设定水温Tw_set
            2-1)实际水温Tw
                [制冷控制选择] = 0,采用总回水温度 Tw = Tw_sys_in
                [制冷控制选择] = 1,采用总出水温度 Tw = Tw_sys_out
                [制冷控制选择] = 2,预留
                [制冷控制选择] = 3,预留
                [制热控制选择] = 0,采用总回水温度 Tw = Tw_sys_in
                [制热控制选择] = 1,采用总出水温度 Tw = Tw_sys_out
                [制热控制选择] = 2,预留
                [制热控制选择] = 3,预留
*************************************************************************/
int16_t SysTwUpdate(void)
{
    if (atw.sys.param.set.cell.cool_temp_chose == 0 || atw.sys.param.set.cell.heat_temp_chose == 0)
    {
        atw.sys.local.Tw = atw.sys.local.Tw_sys_in;
    }
    else if ((atw.sys.param.set.cell.cool_temp_chose == 1) || (atw.sys.param.set.cell.heat_temp_chose == 1))
    {
        atw.sys.local.Tw = atw.sys.local.Tw_sys_out;
    }

    return atw.sys.local.Tw;
}


/************************************************************************
@name  	: SysTwsetUpdate
@brief 	: 
@param 	: None
@return	: None
@note   : 3.2.1
            2)设定水温Tw,设定水温Tw_set
            2-2)设定水温Tw_set
                [制冷控制选择] = 0,采用总回水温度 Tw = Tw_sys_in
                [制冷控制选择] = 1,采用总出水温度 Tw = Tw_sys_out
                [制冷控制选择] = 2,预留
                [制冷控制选择] = 3,预留
                [制热控制选择] = 0,采用总回水温度 Tw = Tw_sys_in
                [制热控制选择] = 1,采用总出水温度 Tw = Tw_sys_out
                [制热控制选择] = 2,预留
                [制热控制选择] = 3,预留
*************************************************************************/
void SysTwsetUpdate(void)//TODO
{
    uint8_t refrigent_type = atw.sys.table.unit[CHILLER_0].init.dsw1.bit.refrigent_type;
    // uint8_t type = 

    if (refrigent_type == STANDARD_WATER)
    {
        switch (atw.sys.param.set.cell.cool_temp_chose)
        {
            case 0://10 ~ 25
            {
                if (atw.sys.param.Tw_set_cool_in < 100)
                {
                    atw.sys.param.Tw_set_cool_in = 100;
                }
                else if (atw.sys.param.Tw_set_cool_in > 250)
                {
                    atw.sys.param.Tw_set_cool_in = 250;
                }                
                atw.sys.local.cool_Tw_set = atw.sys.param.Tw_set_cool_in;  
            }
                break;

            case 1:  //5 ~ 20
            {
                if (atw.sys.param.Tw_set_cool_out < 50)
                {
                    atw.sys.param.Tw_set_cool_out = 50;
                }
                else if (atw.sys.param.Tw_set_cool_out > 200)
                {
                    atw.sys.param.Tw_set_cool_out = 200;
                }
                atw.sys.local.cool_Tw_set = atw.sys.param.Tw_set_cool_out; 
            }
                break;
            case 2:    break;
            case 3:    break;
            default: break;
        }

        switch (atw.sys.param.set.cell.heat_temp_chose)
        {
            case 0:  atw.sys.local.heat_Tw_set = atw.sys.param.Tw_set_heat_in;  break;
            case 1:  atw.sys.local.heat_Tw_set = atw.sys.param.Tw_set_heat_out; break;
            case 2:    break;
            case 3:    break;
            default: break;
        }
    }
    else//ADD_ANTIFREEZE
    {
        switch (atw.sys.param.set.cell.cool_temp_chose)
        {
            case 0:  atw.sys.local.cool_Tw_set = atw.sys.param.Tw_set_cool_in;  break;
            case 1:  atw.sys.local.cool_Tw_set = atw.sys.param.Tw_set_cool_out; break;
            case 2:    break;
            case 3:    break;
            default: break;
        }

        switch (atw.sys.param.set.cell.heat_temp_chose)
        {
            case 0:  atw.sys.local.heat_Tw_set = atw.sys.param.Tw_set_heat_in;  break;
            case 1:  atw.sys.local.heat_Tw_set = atw.sys.param.Tw_set_heat_out; break;
            case 2:    break;
            case 3:    break;
            default: break;
        }
        
    }
}
































/************************************************************************
@name  	: DataProcess
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void DataInit(void)
{
    
}

/************************************************************************
@name  	: DataProcess
@brief 	: 
@param 	: None
@return	: None
*************************************************************************/
void DataProcess(uint8_t taskNo,uint8_t flag_init)
{
    SFSetRunTaskNo( taskNo );
    if (flag_init == TRUE)
    {
        
    }


}







