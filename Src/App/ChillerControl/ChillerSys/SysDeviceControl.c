/***********************************************************************
@file   : SysDeviceControl.c
@brief  : 
@note	: Copyright(C) 2023 JCH Appliances, Inc. All Rights Reserved.
@note   : 3.5.3 水泵,水阀,水流开关的控制
          3.5.3 地板采暖二通阀的控制
          3.5.4 辅助水电加热控制
@note   : 共用水泵的情况下,0#单元无水阀,系统控制共用水泵和和单元水阀,单元只能控制水阀
          非共用水泵的情况下,无水阀,系统控制单元水泵,单元控制各自的水泵
************************************************************************/
#include "SysDeviceControl.h"
#include "Data.h"
#include "SysStateControl.h"








//===地板采暖二通阀控制
/************************************************************************
@name  	: SysFloorHeatTwowayValveControl
@brief 	: 
@param 	: state:ON/OFF
@return	: None
*************************************************************************/
void SysFloorHeatTwowayValveControl(uint8_t state)
{

}

//===风机盘管二通阀
/************************************************************************
@name  	: SysFanCiolTwowayValveControl
@brief 	: 
@param 	: state:ON/OFF
@return	: None
*************************************************************************/
void SysFanCiolTwowayValveControl(uint8_t state)//TODO
{
    uint8_t sys_step = atw.sys.local.work_step;
    uint8_t work_state = atw.sys.table.unit[CHILLER_0].init.dsw1.bit.unit_work_state;

    switch (sys_step)
    {
        case SYS_STATUS_OFF: 
        case SYS_STATUS_STANDBY: 
        {
            
        }
            break;
        case SYS_STATUS_PREPARE:
        case SYS_STATUS_RUN:
        {
            if (work_state == UNIT_STATE_NORMAL)//非测试模式
            {
                
            }
            else//测试模式
            {
                
            }
        }
            break;
        default:
            break;
    }
}

//===辅助水电加热控制
/************************************************************************
@name  	: WaterHeaterControl
@brief 	: 
@param 	: state:ON/OFF
@return	: None
*************************************************************************/
void SysWaterHeaterControl(void)
{
    uint8_t state = atw.sys.param.set.cell.eheat_enable;//启用状态
    uint8_t sys_step = atw.sys.local.work_step;
    int16_t Tam = atw.sys.table.unit[CHILLER_0].io.Tam;

    if (state == OFF)
    {
        atw.sys.table.unit[CHILLER_0].io.DIO.bit.water_heater = OFF;
    }
    else
    {
        switch (sys_step)
        {
            case SYS_STATUS_INIT:
            case SYS_STATUS_OFF: 
            case SYS_STATUS_PREPARE:
            case SYS_STATUS_STANDBY: atw.sys.table.unit[CHILLER_0].io.DIO.bit.water_heater = OFF;
                break;
            case SYS_STATUS_RUN:
            {
                if (GetSysRealRunMode() == SYS_MODE_HEAT)
                {
                    if (Tam <= -400 || Tam >= 800 )//有故障
                    {
                        atw.sys.table.unit[CHILLER_0].io.DIO.bit.water_heater = OFF;
                    }
                    else//无故障
                    {
                        if ((Tam < atw.sys.param.temp_water_heater_open)
                            && (atw.sys.local.Tw <= (atw.sys.local.heat_Tw_set - atw.sys.param.temp_water_heater_open_fix)))//TODO 总水温的确定
                        {
                            atw.sys.table.unit[CHILLER_0].io.DIO.bit.water_heater = ON;
                        }
                        else if (atw.sys.local.Tw >= (atw.sys.local.heat_Tw_set - atw.sys.param.temp_water_heater_close_fix))
                        {
                            atw.sys.table.unit[CHILLER_0].io.DIO.bit.water_heater = OFF;
                        }
                    }
                }
                else
                {
                    atw.sys.table.unit[CHILLER_0].io.DIO.bit.water_heater = OFF;
                }
            }
                break;
            default:
                break;
        }
    }
} 



