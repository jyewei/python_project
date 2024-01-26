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
#include "r_cg_cmt.h"
#include "r_cg_userdefine.h"
#include "common.h"


/***********************************************************************************************************************
* Function Name: R_CMT2_Create
* Description  : This function initializes the CMT2 channel.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CMT2_Create(void)
{
    /* Disable CMI interrupt */
    IEN(PERIB,INTB128) = 0U;

    /* Cancel CMT stop state in LPC */
    MSTP(CMT2) = 0U;

    /* Set control registers */
    CMT2.CMCR.WORD = _0001_CMT_CMCR_CKS_PCLK32 | _0040_CMT_CMCR_CMIE_ENABLE | _0080_CMT_CMCR_DEFAULT;
    CMT2.CMCOR = _7A11_CMT2_CMCOR_VALUE;
    ICU.SLIBXR128.BYTE = 0x01U;

    /* Set CMI2 priority level */
    IPR(PERIB,INTB128) = _0E_CMT_PRIORITY_LEVEL14;
}
/***********************************************************************************************************************
* Function Name: R_CMT2_Start
* Description  : This function starts the CMT2 channel counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CMT2_Start(void)
{
    /* Enable CMI2 interrupt in ICU */
    IEN(PERIB,INTB128) = 1U;

    /* Start CMT2 count */
    CMT.CMSTR1.BIT.STR2 = 1U;
}
/***********************************************************************************************************************
* Function Name: R_CMT2_Stop
* Description  : This function stops the CMT2 channel counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CMT2_Stop(void)
{
    /* Disable CMI2 interrupt in ICU */
    IEN(PERIB,INTB128) = 0U;

    /* Stop CMT2 count */
    CMT.CMSTR1.BIT.STR2 = 0U;
}
/***********************************************************************************************************************
* Function Name: R_CMT3_Create
* Description  : This function initializes the CMT3 channel.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CMT3_Create(void)
{
    /* Disable CMI interrupt */
    IEN(PERIB,INTB129) = 0U;

    /* Cancel CMT stop state in LPC */
    MSTP(CMT3) = 0U;

    /* Set control registers */
    CMT3.CMCR.WORD = _0000_CMT_CMCR_CKS_PCLK8 | _0040_CMT_CMCR_CMIE_ENABLE | _0080_CMT_CMCR_DEFAULT;
#ifdef CLK_HSI_16MHz
    CMT3.CMCOR = 0x03E7;
#else
	CMT3.CMCOR = 0x1D4B;
#endif
    ICU.SLIBXR129.BYTE = 0x02U;

    /* Set CMI3 priority level */
    // IPR(PERIB,INTB129) = _0F_CMT_PRIORITY_LEVEL15;
    IPR(PERIB,INTB129) = 15;//_0F_CMT_PRIORITY_LEVEL15;

}
/***********************************************************************************************************************
* Function Name: R_CMT3_Start
* Description  : This function starts the CMT3 channel counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CMT3_Start(void)
{
    /* Enable CMI3 interrupt in ICU */
    IEN(PERIB,INTB129) = 1U;

    /* Start CMT3 count */
    CMT.CMSTR1.BIT.STR3 = 1U;
}
/***********************************************************************************************************************
* Function Name: R_CMT3_Stop
* Description  : This function stops the CMT3 channel counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CMT3_Stop(void)
{
    /* Disable CMI3 interrupt in ICU */
    IEN(PERIB,INTB129) = 0U;

    /* Stop CMT3 count */
    CMT.CMSTR1.BIT.STR3 = 0U;
}

