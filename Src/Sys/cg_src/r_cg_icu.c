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
* File Name    : r_cg_icu.c
* Version      : Code Generator for RX65N V1.01.02.03 [08 May 2018]
* Device(s)    : R5F565N9FxFP
* Tool-Chain   : CCRX
* Description  : This file implements device driver for ICU module.
* Creation Date: 2023/6/14
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
#include "r_cg_icu.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_ICU_Create
* Description  : This function initializes ICU module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ICU_Create(void)
{
    /* Disable IRQ interrupts */
    ICU.IER[0x08].BYTE = _00_ICU_IRQ0_DISABLE | _00_ICU_IRQ1_DISABLE | _00_ICU_IRQ2_DISABLE | _00_ICU_IRQ3_DISABLE |
                         _00_ICU_IRQ4_DISABLE | _00_ICU_IRQ5_DISABLE | _00_ICU_IRQ6_DISABLE | _00_ICU_IRQ7_DISABLE;
    ICU.IER[0x09].BYTE = _00_ICU_IRQ8_DISABLE | _00_ICU_IRQ9_DISABLE | _00_ICU_IRQ10_DISABLE | _00_ICU_IRQ11_DISABLE |
                         _00_ICU_IRQ12_DISABLE | _00_ICU_IRQ13_DISABLE | _00_ICU_IRQ14_DISABLE | _00_ICU_IRQ15_DISABLE;

    /* Disable group interrupts */
    IEN(ICU,GROUPBL0) = 0U;
    IEN(ICU,GROUPBL1) = 0U;
    IEN(ICU,GROUPAL0) = 0U;

    /* Disable software interrupt */
    IEN(ICU,SWINT) = 0U;

    /* Set IRQ settings */
    ICU.IRQCR[11].BYTE = _0C_ICU_IRQ_EDGE_BOTH;
    ICU.IRQCR[13].BYTE = _0C_ICU_IRQ_EDGE_BOTH;

    /* Set SWINT Priority level */
    IPR(ICU,SWINT) = _0F_ICU_PRIORITY_LEVEL15;

    /* Set IRQ11 priority level */
    IPR(ICU,IRQ11) = _09_ICU_PRIORITY_LEVEL9;

    /* Set IRQ13 priority level */
    IPR(ICU,IRQ13) = _0A_ICU_PRIORITY_LEVEL10;

    /* Set Group BL0 priority level */
    IPR(ICU,GROUPBL0) = _0C_ICU_PRIORITY_LEVEL12;

    /* Set Group BL1 priority level */
    IPR(ICU,GROUPBL1) = _04_ICU_PRIORITY_LEVEL4;

    /* Set Group AL0 priority level */
    IPR(ICU,GROUPAL0) = _05_ICU_PRIORITY_LEVEL5;

    /* Enable group BL0 interrupt */
    IEN(ICU,GROUPBL0) = 1U;

    /* Enable group BL1 interrupt */
    IEN(ICU,GROUPBL1) = 1U;

    /* Enable group AL0 interrupt */
    IEN(ICU,GROUPAL0) = 1U;

    /* Set IRQ11 pin */
    MPC.PA1PFS.BYTE = 0x40U;
    PORTA.PDR.BYTE &= 0xFDU;
    PORTA.PMR.BYTE &= 0xFDU;

    /* Set IRQ13 pin */
    MPC.P05PFS.BYTE = 0x40U;
    PORT0.PDR.BYTE &= 0xDFU;
    PORT0.PMR.BYTE &= 0xDFU;
}
/***********************************************************************************************************************
* Function Name: R_ICU_Software_Start
* Description  : This function enables SWINT interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ICU_Software_Start(void)
{
    /* Enable software interrupt */
    IEN(ICU,SWINT) = 1U;
}
/***********************************************************************************************************************
* Function Name: R_ICU_SoftwareInterrupt_Generate
* Description  : This function generates SWINT interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ICU_SoftwareInterrupt_Generate(void)
{
    /* Generate software interrupt */
    ICU.SWINTR.BIT.SWINT = 1U;
}
/***********************************************************************************************************************
* Function Name: R_ICU_Software_Stop
* Description  : This function disables SWINT interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ICU_Software_Stop(void)
{
    /* Disable software interrupt */
    IEN(ICU,SWINT) = 0U;
}
/***********************************************************************************************************************
* Function Name: R_ICU_IRQ11_Start
* Description  : This function enables IRQ11 interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ICU_IRQ11_Start(void)
{
    /* Enable IRQ11 interrupt */
    IEN(ICU,IRQ11) = 1U;
}
/***********************************************************************************************************************
* Function Name: R_ICU_IRQ11_Stop
* Description  : This function disables IRQ11 interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ICU_IRQ11_Stop(void)
{
    /* Disable IRQ11 interrupt */
    IEN(ICU,IRQ11) = 0U;
}
/***********************************************************************************************************************
* Function Name: R_ICU_IRQ13_Start
* Description  : This function enables IRQ13 interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ICU_IRQ13_Start(void)
{
    /* Enable IRQ13 interrupt */
    IEN(ICU,IRQ13) = 1U;
}
/***********************************************************************************************************************
* Function Name: R_ICU_IRQ13_Stop
* Description  : This function disables IRQ13 interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ICU_IRQ13_Stop(void)
{
    /* Disable IRQ13 interrupt */
    IEN(ICU,IRQ13) = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
