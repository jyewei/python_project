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
#include "r_cg_sci.h"
#include "r_cg_userdefine.h"
#include "common.h"

/***********************************************************************************************************************
* Function Name: R_SCI0_Create
* Description  : This function initializes SCI0.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SCI0_Create(void)
{
    /* Cancel SCI0 module stop state */
    MSTP(SCI0) = 0U;

    /* Set interrupt priority */
    IPR(SCI0,RXI0) = 10;
    IPR(SCI0,TXI0) = 10;

    /* Clear the control register */
    SCI0.SCR.BYTE = 0x00U;

    /* Set clock enable */
    SCI0.SCR.BYTE = _00_SCI_INTERNAL_SCK_UNUSED;

    /* Clear the SIMR1.IICM, SPMR.CKPH, and CKPOL bit */
    SCI0.SIMR1.BIT.IICM = 0U;
    SCI0.SPMR.BIT.CKPH = 0U;
    SCI0.SPMR.BIT.CKPOL = 0U;

    /* Set control registers */
    SCI0.SPMR.BYTE = _00_SCI_RTS;//no parity
    SCI0.SMR.BYTE = _00_SCI_CLOCK_PCLK | _00_SCI_STOP_1 | _00_SCI_PARITY_EVEN | _20_SCI_PARITY_ENABLE | 
                     _00_SCI_DATA_LENGTH_8 | _00_SCI_MULTI_PROCESSOR_DISABLE | _00_SCI_ASYNCHRONOUS_OR_I2C_MODE;
    // SCI0.SMR.BYTE = 0;                
    SCI0.SCMR.BYTE = _00_SCI_SERIAL_MODE | _00_SCI_DATA_LSB_FIRST | _10_SCI_DATA_LENGTH_8_OR_7 | _62_SCI_SCMR_DEFAULT;
    SCI0.SEMR.BYTE = 0x00;
    /* Set bit rate */
#ifdef CLK_HSI_16MHz
    SCI0.BRR = 0x19U;
#else
	SCI0.BRR = 0xC2U;
#endif
    /* Set RXD0 pin */
    MPC.P33PFS.BYTE = 0x0BU;
    PORT3.PMR.BYTE |= 0x08U;

    /* Set TXD0 pin */
    MPC.P32PFS.BYTE = 0x0BU;
    PORT3.PODR.BYTE |= 0x04U;
    PORT3.PDR.BYTE |= 0x04U;
    PORT3.PMR.BYTE |= 0x04U;

//TODO
    SCI0.SCR.BIT.CKE = 1;  //clk_pin output  SCI0_CLK
    //P22 SCK0
    MPC.P22PFS.BIT.PSEL = 10;		// P22-PSEL: SCK0
    PORT2.PODR.BIT.B2 = 1U;
    PORT2.PDR.BIT.B2 = 1U;
    PORT2.PMR.BIT.B2 = 1U;
}
/***********************************************************************************************************************
* Function Name: R_SCI0_Start
* Description  : This function starts SCI0.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SCI0_Start(void)
{
    /* Clear interrupt flag */
    IR(SCI0,TXI0) = 0U;
    IR(SCI0,RXI0) = 0U;

    /* Enable SCI interrupt */
    IEN(SCI0,TXI0) = 1U;
    ICU.GENBL0.BIT.EN0 = 1U;
    IEN(SCI0,RXI0) = 1U;
    ICU.GENBL0.BIT.EN1 = 1U;
}
/***********************************************************************************************************************
* Function Name: R_SCI0_Stop
* Description  : This function stops SCI0.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SCI0_Stop(void)
{
    /* Set TXD0 pin */
    PORT3.PMR.BYTE &= 0xFBU;

    /* Disable serial transmit */
    SCI0.SCR.BIT.TE = 0U;

    /* Disable serial receive */
    SCI0.SCR.BIT.RE = 0U;

    /* Disable SCI interrupt */
    SCI0.SCR.BIT.TIE = 0U;     /* disable TXI interrupt */
    SCI0.SCR.BIT.RIE = 0U;     /* disable RXI and ERI interrupt */
    IEN(SCI0,TXI0) = 0U;
    ICU.GENBL0.BIT.EN0 = 0U;
    IR(SCI0,TXI0) = 0U;
    IEN(SCI0,RXI0) = 0U;
    ICU.GENBL0.BIT.EN1 = 0U;
    IR(SCI0,RXI0) = 0U;
}


/***********************************************************************************************************************
* Function Name: R_SCI3_Create
* Description  : This function initializes SCI3.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SCI3_Create(void)
{
    /* Cancel SCI3 module stop state */
    MSTP(SCI3) = 0U;

    /* Set interrupt priority */
    IPR(SCI3,RXI3) = _0D_SCI_PRIORITY_LEVEL13;
    IPR(SCI3,TXI3) = _0D_SCI_PRIORITY_LEVEL13;

    /* Clear the control register */
    SCI3.SCR.BYTE = 0x00U;

    /* Set clock enable */
    SCI3.SCR.BYTE = 0x01;

    /* Clear the SIMR1.IICM, SPMR.CKPH, and CKPOL bit */
    SCI3.SIMR1.BIT.IICM = 0U;
    SCI3.SPMR.BIT.CKPH = 0U;
    SCI3.SPMR.BIT.CKPOL = 0U;

    /* Set control registers */
    SCI3.SPMR.BYTE = _00_SCI_RTS;
    SCI3.SMR.BYTE = _00_SCI_CLOCK_PCLK | _00_SCI_STOP_1 | _00_SCI_PARITY_EVEN | _20_SCI_PARITY_ENABLE | 
                    _00_SCI_DATA_LENGTH_8 | _00_SCI_MULTI_PROCESSOR_DISABLE | _00_SCI_ASYNCHRONOUS_OR_I2C_MODE;
    SCI3.SCMR.BYTE = _00_SCI_SERIAL_MODE | _00_SCI_DATA_LSB_FIRST | _10_SCI_DATA_LENGTH_8_OR_7 | _62_SCI_SCMR_DEFAULT;
    SCI3.SEMR.BYTE = _00_SCI_LOW_LEVEL_START_BIT | _00_SCI_NOISE_FILTER_DISABLE | _00_SCI_16_BASE_CLOCK | 
                     _00_SCI_BAUDRATE_SINGLE | _00_SCI_BIT_MODULATION_DISABLE;

#ifdef CLK_HSI_16MHz
    SCI3.BRR = 0x19U;
#else
	SCI3.BRR = 0xC2U;
#endif
    /* Set RXD3 pin */
    MPC.P25PFS.BYTE = 0x0AU;
    PORT2.PMR.BYTE |= 0x20U;

    /* Set TXD3 pin */
    MPC.P23PFS.BYTE = 0x0AU;
    PORT2.PODR.BYTE |= 0x08U;
    PORT2.PDR.BYTE |= 0x08U;
    PORT2.PMR.BYTE |= 0x08U;

    //P24 SCK1
    MPC.P24PFS.BYTE = 0x0A;
    PORT2.PODR.BIT.B4 = 1U;
    PORT2.PDR.BIT.B4 = 1U;
    PORT2.PMR.BIT.B4 = 1U;
}
/***********************************************************************************************************************
* Function Name: R_SCI3_Start
* Description  : This function starts SCI3.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SCI3_Start(void)
{
    /* Clear interrupt flag */
    IR(SCI3,TXI3) = 0U;
    IR(SCI3,RXI3) = 0U;

    /* Enable SCI interrupt */
    IEN(SCI3,TXI3) = 1U;
    ICU.GENBL0.BIT.EN6 = 1U;
    IEN(SCI3,RXI3) = 1U;
}
/***********************************************************************************************************************
* Function Name: R_SCI3_Stop
* Description  : This function stops SCI3.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SCI3_Stop(void)
{
    /* Set TXD3 pin */
    PORT2.PMR.BYTE &= 0xF7U;

    /* Disable serial transmit */
    SCI3.SCR.BIT.TE = 0U;

    /* Disable serial receive */
    SCI3.SCR.BIT.RE = 0U;

    /* Disable SCI interrupt */
    SCI3.SCR.BIT.TIE = 0U;     /* disable TXI interrupt */
    SCI3.SCR.BIT.RIE = 0U;     /* disable RXI and ERI interrupt */
    IEN(SCI3,TXI3) = 0U;
    ICU.GENBL0.BIT.EN6 = 0U;
    IR(SCI3,TXI3) = 0U;
    IEN(SCI3,RXI3) = 0U;
    IR(SCI3,RXI3) = 0U;
}
