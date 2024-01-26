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
* File Name    : r_cg_s12ad.c
* Version      : Code Generator for RX65N V1.01.02.03 [08 May 2018]
* Device(s)    : R5F565N9FxFP
* Tool-Chain   : CCRX
* Description  : This file implements device driver for S12AD module.
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
#include "r_cg_s12ad.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_S12AD0_Create
* Description  : This function initializes the AD0 converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_S12AD0_Create(void)
{
    /* Cancel S12AD0 module stop state */
    MSTP(S12AD) = 0U;  

    /* Disable and clear interrupt flags of S12AD module  */
    S12AD.ADCSR.BIT.ADIE = 0U;
    S12AD.ADCSR.BIT.GBADIE = 0U;
    S12AD.ADGCTRGR.BIT.GCADIE = 0U;
    S12AD.ADCMPCR.BIT.CMPAIE = 0U;
    S12AD.ADCMPCR.BIT.CMPBIE = 0U;
    IEN(PERIB,INTB130) = 0U;

    /* Set S12AD0 control registers */
    S12AD.ADSHCR.WORD |= _0000_AD0_DSH_CHANNEL_SELECT;
    S12AD.ADDISCR.BYTE = _00_AD0_DISCONECT_SETTING;
    S12AD.ADCSR.WORD = _1000_AD_SCAN_END_INTERRUPT_ENABLE | _4000_AD_CONTINUOUS_SCAN_MODE;
    S12AD.ADCER.WORD = _0000_AD_AUTO_CLEARING_DISABLE | _0000_AD_RIGHT_ALIGNMENT | _0000_AD_SELFTDIAGST_DISABLE | 
                       _0002_AD_RESOLUTION_10BIT;
    S12AD.ADADC.BYTE = _02_AD_3_TIME_CONVERSION | _00_AD_ADDITION_MODE;

    /* Set writing to conversion time register */
    S12AD.ADSAMPR.BYTE = _03_AD_WRITE_CONVERSION_ENABLE;
    S12AD.ADSAM.WORD = _0000_AD_CONVERSION_FOR_HIGHSPEED;
    S12AD.ADSAMPR.BYTE = _02_AD_WRITE_CONVERSION_DISABLE;

    /* Set channels and sampling time */
    S12AD.ADANSA0.WORD = _00FF_AD0_CHANNEL_SELECT_A0;
    S12AD.ADADS0.WORD = _00FF_AD0_ADDAVG_CHANNEL_SELECT0;
    S12AD.ADSSTR0 = _FA_AD0_SAMPLING_STATE_0;
    S12AD.ADSSTR1 = _FA_AD0_SAMPLING_STATE_1;
    S12AD.ADSSTR2 = _FA_AD0_SAMPLING_STATE_2;
    S12AD.ADSSTR3 = _FA_AD0_SAMPLING_STATE_3;
    S12AD.ADSSTR4 = _FA_AD0_SAMPLING_STATE_4;
    S12AD.ADSSTR5 = _FA_AD0_SAMPLING_STATE_5;
    S12AD.ADSSTR6 = _FA_AD0_SAMPLING_STATE_6;
    S12AD.ADSSTR7 = _FA_AD0_SAMPLING_STATE_7;

    /* Set compare control register */
    S12AD.ADCMPCR.WORD = _0000_AD_WINDOWFUNCTION_DISABLE | _0000_AD_WINDOWA_DISABLE | _0000_AD_WINDOWB_DISABLE;

    /* Set interrupt and priority level */
    ICU.SLIBXR130.BYTE = 0x40U;
    IPR(PERIB,INTB130) = _06_AD_PRIORITY_LEVEL6;

    /* Set AN000 pin */
    PORT4.PMR.BYTE &= 0xFEU;
    PORT4.PDR.BYTE &= 0xFEU;
    MPC.P40PFS.BYTE = 0x80U;

    /* Set AN001 pin */
    PORT4.PMR.BYTE &= 0xFDU;
    PORT4.PDR.BYTE &= 0xFDU;
    MPC.P41PFS.BYTE = 0x80U;

    /* Set AN002 pin */
    PORT4.PMR.BYTE &= 0xFBU;
    PORT4.PDR.BYTE &= 0xFBU;
    MPC.P42PFS.BYTE = 0x80U;

    /* Set AN003 pin */
    PORT4.PMR.BYTE &= 0xF7U;
    PORT4.PDR.BYTE &= 0xF7U;
    MPC.P43PFS.BYTE = 0x80U;

    /* Set AN004 pin */
    PORT4.PMR.BYTE &= 0xEFU;
    PORT4.PDR.BYTE &= 0xEFU;
    MPC.P44PFS.BYTE = 0x80U;

    /* Set AN005 pin */
    PORT4.PMR.BYTE &= 0xDFU;
    PORT4.PDR.BYTE &= 0xDFU;
    MPC.P45PFS.BYTE = 0x80U;

    /* Set AN006 pin */
    PORT4.PMR.BYTE &= 0xBFU;
    PORT4.PDR.BYTE &= 0xBFU;
    MPC.P46PFS.BYTE = 0x80U;

    /* Set AN007 pin */
    PORT4.PMR.BYTE &= 0x7FU;
    PORT4.PDR.BYTE &= 0x7FU;
    MPC.P47PFS.BYTE = 0x80U;
}
/***********************************************************************************************************************
* Function Name: R_S12AD0_Start
* Description  : This function starts the AD0 converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_S12AD0_Start(void)
{
    IR(PERIB,INTB130) = 0U;
    IEN(PERIB,INTB130) = 1U;
    S12AD.ADCSR.BIT.ADST = 1U;
}
/***********************************************************************************************************************
* Function Name: R_S12AD0_Stop
* Description  : This function stops the AD0 converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_S12AD0_Stop(void)
{
    S12AD.ADCSR.BIT.ADST = 0U;
    IEN(PERIB,INTB130) = 0U;
    IR(PERIB,INTB130) = 0U;
}
/***********************************************************************************************************************
* Function Name: R_S12AD0_Get_ValueResult
* Description  : This function gets result from the AD0 converter.
* Arguments    : channel -
*                    channel of data register to be read
*                buffer -
*                    buffer pointer
* Return Value : None
***********************************************************************************************************************/
void R_S12AD0_Get_ValueResult(ad_channel_t channel, uint16_t * const buffer)
{
   if (channel == ADSELFDIAGNOSIS)
    {
        *buffer = (uint16_t)(S12AD.ADRD.WORD);
    }
    else if (channel == ADCHANNEL0)
    {
        *buffer = (uint16_t)(S12AD.ADDR0);
    }
    else if (channel == ADCHANNEL1)
    {
        *buffer = (uint16_t)(S12AD.ADDR1);
    }
    else if (channel == ADCHANNEL2)
    {
        *buffer = (uint16_t)(S12AD.ADDR2);
    }
    else if (channel == ADCHANNEL3)
    {
        *buffer = (uint16_t)(S12AD.ADDR3);
    }
    else if (channel == ADCHANNEL4)
    {
        *buffer = (uint16_t)(S12AD.ADDR4);
    }
    else if (channel == ADCHANNEL5)
    {
        *buffer = (uint16_t)(S12AD.ADDR5);
    }
    else if (channel == ADCHANNEL6)
    {
        *buffer = (uint16_t)(S12AD.ADDR6);
    }
    else if (channel == ADCHANNEL7)
    {
        *buffer = (uint16_t)(S12AD.ADDR7);
    }
    else if (channel == ADDATADUPLICATION)
    {
        *buffer = (uint16_t)(S12AD.ADDBLDR.WORD);
    }
    else if (channel == ADDATADUPLICATIONA)
    {
        *buffer = (uint16_t)(S12AD.ADDBLDRA);
    }
    else if (channel == ADDATADUPLICATIONB)
    {
        *buffer = (uint16_t)(S12AD.ADDBLDRB);
    }
    else
    {
         /* Do Nothing */ 
    }
}
/***********************************************************************************************************************
* Function Name: R_S12AD1_Create
* Description  : This function initializes the AD1 converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_S12AD1_Create(void)
{
    /* Cancel S12AD1 module stop state */
    MSTP(S12AD1) = 0U;  

    /* Disable and clear interrupt flags of S12AD1 module  */
    S12AD1.ADCSR.BIT.ADIE = 0U;
    S12AD1.ADCSR.BIT.GBADIE = 0U;
    S12AD1.ADGCTRGR.BIT.GCADIE = 0U;
    S12AD1.ADCMPCR.BIT.CMPAIE = 0U;
    S12AD1.ADCMPCR.BIT.CMPBIE = 0U;
    IEN(PERIB,INTB131) = 0U;

    /* Set S12AD1 control registers */
    S12AD1.ADDISCR.BYTE = _00_AD1_DISCONECT_SETTING;
    S12AD1.ADCSR.WORD = _1000_AD_SCAN_END_INTERRUPT_ENABLE | _4000_AD_CONTINUOUS_SCAN_MODE;
    S12AD1.ADCER.WORD = _0000_AD_AUTO_CLEARING_DISABLE | _0000_AD_RIGHT_ALIGNMENT | _0000_AD_SELFTDIAGST_DISABLE | 
                        _0002_AD_RESOLUTION_10BIT;
    S12AD1.ADADC.BYTE = _00_AD_1_TIME_CONVERSION | _00_AD_ADDITION_MODE;
    S12AD1.ADEXICR.WORD = _0000_AD_EXTNANEX1_IN_DISABLE | _0000_AD_IREF_GROUPA_DISABLE | _0000_AD_TEMP_GROUPA_DISABLE;

    /* Set writing to conversion time register */
    S12AD1.ADSAMPR.BYTE = _03_AD_WRITE_CONVERSION_ENABLE;
    S12AD1.ADSAM.WORD = _0020_AD_CONVERSION_FOR_MIDDLESPEED;
    S12AD1.ADSAMPR.BYTE = _02_AD_WRITE_CONVERSION_DISABLE;

    /* Set channels and sampling time */
    S12AD1.ADANSA0.WORD = _3FFF_AD1_CHANNEL_SELECT_A0;
    S12AD1.ADANSA1.WORD = _0000_AD1_CHANNEL_SELECT_A1;
    S12AD1.ADADS0.WORD = _0000_AD1_ADDAVG_CHANNEL_SELECT0;
    S12AD1.ADADS1.WORD = _0000_AD1_ADDAVG_CHANNEL_SELECT1;
    S12AD1.ADSSTR0 = _B7_AD1_SAMPLING_STATE_0;
    S12AD1.ADSSTR1 = _B7_AD1_SAMPLING_STATE_1;
    S12AD1.ADSSTR2 = _B7_AD1_SAMPLING_STATE_2;
    S12AD1.ADSSTR3 = _B7_AD1_SAMPLING_STATE_3;
    S12AD1.ADSSTR4 = _B7_AD1_SAMPLING_STATE_4;
    S12AD1.ADSSTR5 = _B7_AD1_SAMPLING_STATE_5;
    S12AD1.ADSSTR6 = _B7_AD1_SAMPLING_STATE_6;
    S12AD1.ADSSTR7 = _B7_AD1_SAMPLING_STATE_7;
    S12AD1.ADSSTR8 = _B7_AD1_SAMPLING_STATE_8;
    S12AD1.ADSSTR9 = _B7_AD1_SAMPLING_STATE_9;
    S12AD1.ADSSTR10 = _B7_AD1_SAMPLING_STATE_10;
    S12AD1.ADSSTR11 = _B7_AD1_SAMPLING_STATE_11;
    S12AD1.ADSSTR12 = _B7_AD1_SAMPLING_STATE_12;
    S12AD1.ADSSTR13 = _B7_AD1_SAMPLING_STATE_13;

    /* Set compare control register */
    S12AD1.ADCMPCR.WORD = _0000_AD_WINDOWFUNCTION_DISABLE | _0000_AD_WINDOWA_DISABLE | _0000_AD_WINDOWB_DISABLE;

    /* Set interrupt and priority level */
    ICU.SLIBXR131.BYTE = 0x44U;
    IPR(PERIB,INTB131) = _0F_AD_PRIORITY_LEVEL15;

    /* Set AN100 pin */
    PORTE.PMR.BYTE &= 0xFBU;
    PORTE.PDR.BYTE &= 0xFBU;
    MPC.PE2PFS.BYTE = 0x80U;

    /* Set AN101 pin */
    PORTE.PMR.BYTE &= 0xF7U;
    PORTE.PDR.BYTE &= 0xF7U;
    MPC.PE3PFS.BYTE = 0x80U;

    /* Set AN102 pin */
    PORTE.PMR.BYTE &= 0xEFU;
    PORTE.PDR.BYTE &= 0xEFU;
    MPC.PE4PFS.BYTE = 0x80U;

    /* Set AN103 pin */
    PORTE.PMR.BYTE &= 0xDFU;
    PORTE.PDR.BYTE &= 0xDFU;
    MPC.PE5PFS.BYTE = 0x80U;

    /* Set AN104 pin */
    PORTE.PMR.BYTE &= 0xBFU;
    PORTE.PDR.BYTE &= 0xBFU;
    MPC.PE6PFS.BYTE = 0x80U;

    /* Set AN105 pin */
    PORTE.PMR.BYTE &= 0x7FU;
    PORTE.PDR.BYTE &= 0x7FU;
    MPC.PE7PFS.BYTE = 0x80U;

    /* Set AN106 pin */
    PORTD.PMR.BYTE &= 0xBFU;
    PORTD.PDR.BYTE &= 0xBFU;
    MPC.PD6PFS.BYTE = 0x80U;

    /* Set AN107 pin */
    PORTD.PMR.BYTE &= 0x7FU;
    PORTD.PDR.BYTE &= 0x7FU;
    MPC.PD7PFS.BYTE = 0x80U;

    /* Set AN108 pin */
    PORTD.PMR.BYTE &= 0xFEU;
    PORTD.PDR.BYTE &= 0xFEU;
    MPC.PD0PFS.BYTE = 0x80U;

    /* Set AN109 pin */
    PORTD.PMR.BYTE &= 0xFDU;
    PORTD.PDR.BYTE &= 0xFDU;
    MPC.PD1PFS.BYTE = 0x80U;

    /* Set AN110 pin */
    PORTD.PMR.BYTE &= 0xFBU;
    PORTD.PDR.BYTE &= 0xFBU;
    MPC.PD2PFS.BYTE = 0x80U;

    /* Set AN111 pin */
    PORTD.PMR.BYTE &= 0xF7U;
    PORTD.PDR.BYTE &= 0xF7U;
    MPC.PD3PFS.BYTE = 0x80U;

    /* Set AN112 pin */
    PORTD.PMR.BYTE &= 0xEFU;
    PORTD.PDR.BYTE &= 0xEFU;
    MPC.PD4PFS.BYTE = 0x80U;

    /* Set AN113 pin */
    PORTD.PMR.BYTE &= 0xDFU;
    PORTD.PDR.BYTE &= 0xDFU;
    MPC.PD5PFS.BYTE = 0x80U;
}
/***********************************************************************************************************************
* Function Name: R_S12AD1_Start
* Description  : This function starts the AD1 converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_S12AD1_Start(void)
{
    IR(PERIB,INTB131) = 0U;
    IEN(PERIB,INTB131) = 1U;
    S12AD1.ADCSR.BIT.ADST = 1U;
}
/***********************************************************************************************************************
* Function Name: R_S12AD1_Stop
* Description  : This function stops the AD1 converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_S12AD1_Stop(void)
{
    S12AD1.ADCSR.BIT.ADST = 0U;
    IEN(PERIB,INTB131) = 0U;
    IR(PERIB,INTB131) = 0U;
}
/***********************************************************************************************************************
* Function Name: R_S12AD1_Get_ValueResult
* Description  : This function gets result from the AD1 converter.
* Arguments    : channel -
*                    channel of data register to be read
*                buffer -
*                    buffer pointer
* Return Value : None
***********************************************************************************************************************/
void R_S12AD1_Get_ValueResult(ad_channel_t channel, uint16_t * const buffer)
{
   if (channel == ADSELFDIAGNOSIS)
    {
        *buffer = (uint16_t)(S12AD1.ADRD.WORD);
    }
    else if (channel == ADCHANNEL0)
    {
        *buffer = (uint16_t)(S12AD1.ADDR0);
    }
    else if (channel == ADCHANNEL1)
    {
        *buffer = (uint16_t)(S12AD1.ADDR1);
    }
    else if (channel == ADCHANNEL2)
    {
        *buffer = (uint16_t)(S12AD1.ADDR2);
    }
    else if (channel == ADCHANNEL3)
    {
        *buffer = (uint16_t)(S12AD1.ADDR3);
    }
    else if (channel == ADCHANNEL4)
    {
        *buffer = (uint16_t)(S12AD1.ADDR4);
    }
    else if (channel == ADCHANNEL5)
    {
        *buffer = (uint16_t)(S12AD1.ADDR5);
    }
    else if (channel == ADCHANNEL6)
    {
        *buffer = (uint16_t)(S12AD1.ADDR6);
    }
    else if (channel == ADCHANNEL7)
    {
        *buffer = (uint16_t)(S12AD1.ADDR7);
    }
    else if (channel == ADCHANNEL8)
    {
        *buffer = (uint16_t)(S12AD1.ADDR8);
    }
    else if (channel == ADCHANNEL9)
    {
        *buffer = (uint16_t)(S12AD1.ADDR9);
    }
    else if (channel == ADCHANNEL10)
    {
        *buffer = (uint16_t)(S12AD1.ADDR10);
    }
    else if (channel == ADCHANNEL11)
    {
        *buffer = (uint16_t)(S12AD1.ADDR11);
    }
    else if (channel == ADCHANNEL12)
    {
        *buffer = (uint16_t)(S12AD1.ADDR12);
    }
    else if (channel == ADCHANNEL13)
    {
        *buffer = (uint16_t)(S12AD1.ADDR13);
    }
    else if (channel == ADCHANNEL14)
    {
        *buffer = (uint16_t)(S12AD1.ADDR14);
    }
    else if (channel == ADCHANNEL15)
    {
        *buffer = (uint16_t)(S12AD1.ADDR15);
    }
    else if (channel == ADCHANNEL16)
    {
        *buffer = (uint16_t)(S12AD1.ADDR16);
    }
    else if (channel == ADCHANNEL17)
    {
        *buffer = (uint16_t)(S12AD1.ADDR17);
    }
    else if (channel == ADCHANNEL18)
    {
        *buffer = (uint16_t)(S12AD1.ADDR18);
    }
    else if (channel == ADCHANNEL19)
    {
        *buffer = (uint16_t)(S12AD1.ADDR19);
    }
    else if (channel == ADCHANNEL20)
    {
        *buffer = (uint16_t)(S12AD1.ADDR20);
    }
    else if (channel == ADTEMPSENSOR)
    {
        *buffer = (uint16_t)(S12AD1.ADTSDR);
    }
    else if (channel == ADINTERREFVOLT)
    {
        *buffer = (uint16_t)(S12AD1.ADOCDR);
    }
    else if (channel == ADDATADUPLICATION)
    {
        *buffer = (uint16_t)(S12AD1.ADDBLDR);
    }
    else if (channel == ADDATADUPLICATIONA)
    {
        *buffer = (uint16_t)(S12AD1.ADDBLDRA);
    }
    else if (channel == ADDATADUPLICATIONB)
    {
        *buffer = (uint16_t)(S12AD1.ADDBLDRB);
    }
    else
    {
         /* Do Nothing */ 
    }
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
