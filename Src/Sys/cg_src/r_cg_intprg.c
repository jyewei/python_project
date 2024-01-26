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
* File Name    : r_cg_intprg.c
* Version      : Code Generator for RX65N V1.01.02.03 [08 May 2018]
* Device(s)    : R5F565N9FxFP
* Tool-Chain   : CCRX
* Description  : Setting of B.
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
#include <machine.h>
#include "r_cg_vect.h"
#include "r_cg_sci.h"
#include "r_cg_riic.h"
#include "r_cg_rspi.h"
#include "r_cg_userdefine.h"
#include "Interrupt.h"
/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/

#pragma section IntPRG

/***********************************************************************************************************************
* Function Name: r_undefined_exception
* Description  : This function is undefined instruction exception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void r_undefined_exception(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_privileged_exception
* Description  : This function is privileged instruction exception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void r_privileged_exception(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_floatingpoint_exception
* Description  : This function is floating point exception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void r_floatingpoint_exception(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_access_exception
* Description  : This function is access exception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void r_access_exception(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_reserved_exception
* Description  : This function is reserved.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void r_reserved_exception(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_nmi_exception
* Description  : This function is NMI exception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void r_nmi_exception(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_brk_exception
* Description  : This function is BRK exception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void r_brk_exception(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_icu_group_be0_interrupt
* Description  : This function is ICU GROUPBE0 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void r_icu_group_be0_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_icu_group_bl0_interrupt
* Description  : This function is ICU GROUPBL0 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void r_icu_group_bl0_interrupt(void)
{
    //SCI0
    if (ICU.GRPBL0.BIT.IS1 == 1U)
    {
        r_sci0_receiveerror_interrupt();
    }
    if (ICU.GRPBL0.BIT.IS0 == 1U)
    {
        r_sci0_transmitend_interrupt();
    }
    // //SCI1
    // if (ICU.GRPBL0.BIT.IS2 == 1U)
    // {
    //     r_sci1_transmitend_interrupt();
    // }
    // if (ICU.GRPBL0.BIT.IS3 == 1U)
    // {
    //     r_sci1_receiveerror_interrupt();
    // }
    //SCI3
    if (ICU.GRPBL0.BIT.IS7 == 1U)
    {
        r_sci3_receiveerror_interrupt();
    }
    if (ICU.GRPBL0.BIT.IS6 == 1U)
    {
        r_sci3_transmitend_interrupt();
    }




    // if (ICU.GRPBL0.BIT.IS10 == 1U)
    // {
    //     r_sci5_transmitend_interrupt();
    // }
    // if (ICU.GRPBL0.BIT.IS12 == 1U)
    // {
    //     r_sci6_transmitend_interrupt();
    // }
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_icu_group_bl1_interrupt
* Description  : This function is ICU GROUPBL1 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void r_icu_group_bl1_interrupt(void)
{
    // if (ICU.GRPBL1.BIT.IS15 == 1U)
    // {
    //     r_riic2_transmitend_interrupt();
    // }
    // if (ICU.GRPBL1.BIT.IS16 == 1U)
    // {
    //     r_riic2_error_interrupt();
    // }
    // if (ICU.GRPBL1.BIT.IS26 == 1U)
    // {
    //     r_sci9_transmitend_interrupt();
    // }
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_icu_group_bl2_interrupt
* Description  : This function is ICU GROUPBL2 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void r_icu_group_bl2_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_icu_group_al0_interrupt
* Description  : This function is ICU GROUPAL0 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void r_icu_group_al0_interrupt(void)
{
    // if (ICU.GRPAL0.BIT.IS16 == 1U)
    // {
    //     r_rspi0_idle_interrupt();
    // }
    // if (ICU.GRPAL0.BIT.IS17 == 1U)
    // {
    //     r_rspi0_error_interrupt();
    // }
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_icu_group_al1_interrupt
* Description  : This function is ICU GROUPAL1 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void r_icu_group_al1_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
