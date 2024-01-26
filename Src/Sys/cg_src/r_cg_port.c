/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2015, 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"

#include "r_cg_userdefine.h"




/***********************************************************************************************************************
* Function Name: R_PORT_Create
* Description  : This function initializes the Port I/O.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_PORT_Create(void)
{
    PORTA.PCR.BYTE = _40_Pm6_PULLUP_ON;
    PORT3.DSCR2.BYTE = _00_Pm0_HISPEED_OFF;
    PORT5.DSCR.BYTE = _08_Pm3_HIDRV_ON | _10_Pm4_HIDRV_ON | _20_Pm5_HIDRV_ON;
    PORT5.DSCR2.BYTE = _00_Pm3_HISPEED_OFF | _00_Pm4_HISPEED_OFF | _00_Pm5_HISPEED_OFF;
    PORTC.DSCR.BYTE = _00_Pm4_HIDRV_OFF;
    PORTC.DSCR2.BYTE = _00_Pm4_HISPEED_OFF;
    PORTE.DSCR.BYTE = _00_Pm0_HIDRV_OFF;
    PORTE.DSCR2.BYTE = _00_Pm0_HISPEED_OFF;
    PORT2.PMR.BYTE = 0x00U;
    PORT2.PDR.BYTE = _04_Pm2_MODE_OUTPUT;
    PORT3.PMR.BYTE = 0x00U;
    PORT3.PDR.BYTE = _01_Pm0_MODE_OUTPUT;
    PORT5.PMR.BYTE = 0x00U;
    PORT5.PDR.BYTE = _08_Pm3_MODE_OUTPUT | _10_Pm4_MODE_OUTPUT | _20_Pm5_MODE_OUTPUT | _C0_PDR5_DEFAULT;
    PORTA.PMR.BYTE = 0x00U;
    PORTA.PDR.BYTE = _00_Pm6_MODE_INPUT;
    PORTC.PMR.BYTE = 0x00U;
    PORTC.PDR.BYTE = _10_Pm4_MODE_OUTPUT;
    PORTE.PMR.BYTE = 0x00U;
    PORTE.PDR.BYTE = _01_Pm0_MODE_OUTPUT;
    PORTJ.PMR.BYTE = 0x00U;
    PORTJ.PDR.BYTE = _08_Pm3_MODE_OUTPUT | _F7_PDRJ_DEFAULT;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
