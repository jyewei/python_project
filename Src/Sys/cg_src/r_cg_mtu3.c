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
* File Name    : r_cg_mtu3.c
* Version      : Code Generator for RX65N V1.01.02.03 [08 May 2018]
* Device(s)    : R5F565N9FxFP
* Tool-Chain   : CCRX
* Description  : This file implements device driver for MTU3 module.
* Creation Date: 2023/6/15
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
#include "r_cg_mtu3.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_MTU3_Create
* Description  : This function initializes the MTU3 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MTU3_Create(void)
{
    /* Cancel MTU stop state in LPC */
    MSTP(MTU3) = 0U;

    /* Enable read/write to MTU registers */
    MTU.TRWERA.BIT.RWE = 1U;
    MTU.TRWERB.BIT.RWE = 1U;

    /* Stop all channels */
    MTU.TSTRA.BYTE = _00_MTU_CST0_OFF | _00_MTU_CST1_OFF | _00_MTU_CST2_OFF | _00_MTU_CST3_OFF | _00_MTU_CST4_OFF | 
                     _00_MTU_CST8_OFF;
    MTU5.TSTR.BYTE = _00_MTU_CSTW5_OFF | _00_MTU_CSTV5_OFF | _00_MTU_CSTU5_OFF;
    MTU.TSTRB.BYTE = _00_MTU_CST6_OFF | _00_MTU_CST7_OFF;

    // /* Channel 1 is used as normal mode */
    MTU.TSYRA.BIT.SYNC1 = 0U;
    MTU1.TCR.BYTE = _00_MTU_PCLK_1 | _40_MTU_CKCL_B;
    MTU1.TCR2.BYTE = _00_MTU_PCLK_1;
    MTU1.TIER.BYTE = _00_MTU_TGIEA_DISABLE | _00_MTU_TGIEB_DISABLE | _00_MTU_TCIEV_DISABLE | _00_MTU_TTGE_DISABLE;
    MTU1.TMDR1.BYTE = _00_MTU_NORMAL;
    MTU1.TIOR.BYTE = _70_MTU_IOB_HT | _00_MTU_IOA_DISABLE;//
    // MTU1.TIOR.BYTE = _30_MTU_IOB_LT | _00_MTU_IOA_DISABLE;//

    MTU1.TGRA = _270F_TGRA1_VALUE;
    MTU1.TGRB = _144F_TGRB1_VALUE;

    // /* Channel 1 is used as PWM2 mode */
    // MTU1.TCR.BYTE = _00_MTU_PCLK_1 | _20_MTU_CKCL_A;
    // MTU1.TCR2.BYTE = _00_MTU_PCLK_1;
    // MTU.TSYRA.BIT.SYNC1 = 0U;
    // MTU1.TMDR1.BYTE = _03_MTU_PWM2;
    // MTU1.TIOR.BYTE = _70_MTU_IOB_HT | _00_MTU_IOA_DISABLE;
    // MTU1.TGRA = 0x144F;//_144F_TGRA1_VALUE;
    // MTU1.TGRB = 0x0A28;//_0A28_TGRB1_VALUE;
    // MTU1.TIER.BYTE = _00_MTU_TGIEA_DISABLE | _00_MTU_TGIEB_DISABLE | _00_MTU_TCIEV_DISABLE | _00_MTU_TTGE_DISABLE;


    /* Channel 4 is used as normal mode */
    MTU.TOERA.BYTE |= _C0_MTU_OE4D_DISABLE | _C0_MTU_OE4C_DISABLE | _C0_MTU_OE4B_DISABLE | _C2_MTU_OE4A_ENABLE;
    MTU.TSYRA.BIT.SYNC4 = 0U;
    MTU4.TCR.BYTE = _00_MTU_PCLK_1 | _00_MTU_CKCL_DIS;
    MTU4.TCR2.BYTE = _00_MTU_PCLK_1;
    MTU4.TIER.BYTE = _00_MTU_TGIEA_DISABLE | _00_MTU_TGIEB_DISABLE | _00_MTU_TGIEC_DISABLE | _00_MTU_TGIED_DISABLE | 
                     _00_MTU_TCIEV_DISABLE | _00_MTU_TTGE_DISABLE;
    MTU4.TMDR1.BYTE = _00_MTU_NORMAL;
    MTU4.TCNT = 0x0000U;
    MTU4.TADCR.WORD = _0000_MTU_UTAE_DISABLE | _0000_MTU_UTBE_DISABLE | _0000_MTU_BF_DISABLE;
    MTU4.TIORH.BYTE = _00_MTU_IOB_DISABLE | _07_MTU_IOA_HT;
    MTU4.TIORL.BYTE = _00_MTU_IOD_DISABLE | _00_MTU_IOC_DISABLE;
    MTU4.TMDR1.BYTE |= _00_MTU_BFA_NORMAL | _00_MTU_BFB_NORMAL;
    MTU4.TGRA = _270F_TGRA4_VALUE;
    MTU4.TGRB = _270F_TGRB4_VALUE;
    MTU4.TGRC = _270F_TGRC4_VALUE;
    MTU4.TGRD = _270F_TGRD4_VALUE;
    
    /* Disable read/write to MTU registers */
    MTU.TRWERA.BIT.RWE = 0U;
    MTU.TRWERB.BIT.RWE = 0U;

    /* Set MTIOC1B pin */
    MPC.P21PFS.BYTE = 0x01U;
    PORT2.PMR.BYTE |= 0x02U;

    /* Set MTIOC4A pin */
    MPC.P24PFS.BYTE = 0x01U;
    PORT2.PMR.BYTE |= 0x10U;
}

/***********************************************************************************************************************
* Function Name: R_MTU3_C1_Start
* Description  : This function starts MTU3 channel 1 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MTU3_C1_Start(void)
{
    // R_MTU3_Create();
    MTU.TSTRA.BYTE |= _02_MTU_CST1_ON;
}

/***********************************************************************************************************************
* Function Name: R_MTU3_C1_Stop
* Description  : This function stops MTU3 channel 1 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MTU3_C1_Stop(void)
{
    MTU.TSTRA.BIT.CST1 = 0U;
}
/***********************************************************************************************************************
* Function Name: R_MTU3_C4_Start
* Description  : This function starts MTU3 channel 4 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MTU3_C4_Start(void)
{
    MTU.TSTRA.BYTE |= _80_MTU_CST4_ON;
}
/***********************************************************************************************************************
* Function Name: R_MTU3_C4_Stop
* Description  : This function stops MTU3 channel 4 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MTU3_C4_Stop(void)
{
    MTU.TSTRA.BIT.CST4 = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
