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

/***********************************************************************************************************************
* File Name    : r_cg_dmac.c
* Version      : Code Generator for RX65N V1.01.02.03 [08 May 2018]
* Device(s)    : R5F565N9FxFP
* Tool-Chain   : CCRX
* Description  : This file implements device driver for DMAC module.
* Creation Date: 2023/6/1
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_dmac.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_DMAC_Create
* Description  : This function initializes the DMAC module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DMAC_Create(void)
{
    /* Cancel DMAC/DTC module stop state */
    MSTP(DMAC) = 0U;

    /* Disable DMAC interrupts */
    IEN(DMAC,DMAC0I) = 0U;
    IEN(DMAC,DMAC1I) = 0U;
    IEN(DMAC,DMAC2I) = 0U;
    IEN(DMAC,DMAC3I) = 0U;
    IEN(DMAC,DMAC74I) = 0U;

    /* Settings for DMAC7 */
    /* Disable DMAC7 transfer */
    DMAC7.DMCNT.BIT.DTE = 0U;

    /* Set DMAC7 activation source */
    ICU.DMRSR7 = _83_DMAC7_ACTIVATION_SOURCE;

    /* Set DMAC7 transfer address update and extended repeat setting */
    DMAC7.DMAMD.WORD = _0000_DMAC_SRC_ADDR_UPDATE_FIXED | _0000_DMAC_DST_ADDR_UPDATE_FIXED | 
                       _0000_DMAC7_SRC_EXT_RPT_AREA | _0000_DMAC7_DST_EXT_RPT_AREA;

    /* Set DMAC7 transfer mode, data size and repeat area */
    DMAC7.DMTMD.WORD = _0000_DMAC_TRANS_MODE_NORMAL | _2000_DMAC_REPEAT_AREA_NONE | _0100_DMAC_TRANS_DATA_SIZE_16 | 
                       _0001_DMAC_TRANS_REQ_SOURCE_INT;

    /* Set DMAC7 interrupt flag control */
    DMAC7.DMCSL.BYTE = _00_DMAC_INT_TRIGGER_FLAG_CLEAR;

    /* Set DMAC7 source address */
    DMAC7.DMSAR = (void *)_00000000_DMAC7_SRC_ADDR;

    /* Set DMAC7 destination address */
    DMAC7.DMDAR = (void *)_00000000_DMAC7_DST_ADDR;

    /* Set DMAC7 transfer count */
    DMAC7.DMCRA = _00000001_DMAC7_DMCRA_COUNT;

    /* Set DMAC7 interrupt settings */
    DMAC7.DMINT.BIT.DTIE = 1U;

    /* Set DMAC74I interrupt settings */
    IPR(DMAC,DMAC74I) = _03_DMAC_PRIORITY_LEVEL3;

    /* Enable DMAC activation */
    DMAC.DMAST.BIT.DMST = 1U;
}
/***********************************************************************************************************************
* Function Name: R_DMAC7_Start
* Description  : This function enable DMAC7 activation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DMAC7_Start(void)
{
    if ((DMAC4.DMINT.BYTE != 0x00U) || (DMAC5.DMINT.BYTE != 0x00U) || (DMAC6.DMINT.BYTE != 0x00U) || 
        (DMAC7.DMINT.BYTE != 0x00U))
    {
        /* Enable DMAC interrupt */
        IR(DMAC,DMAC74I) = 0U;
        IEN(DMAC,DMAC74I) = 1U;
    }

    DMAC7.DMCNT.BIT.DTE = 1U;
}
/***********************************************************************************************************************
* Function Name: R_DMAC7_Stop
* Description  : This function disable DMAC7 activation (note: Interrupts for channels 4~7 should be disabled manually).
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DMAC7_Stop(void)
{
    DMAC7.DMCNT.BIT.DTE = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
