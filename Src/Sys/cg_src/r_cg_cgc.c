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
#include "r_cg_cgc.h"
#include "r_cg_userdefine.h"
#include "common.h"






#ifdef CLK_HSI_20MHz_PLL_120MHz
/***********************************************************************************************************************
* Function Name: R_CGC_Create
* Description  : This function initializes the clock generator.
* Arguments    : None
* Return Value : None
@note:  HSI/20MHz -> PLL/120MHz
***********************************************************************************************************************/
void R_CGC_Create(void)
{
    // uint16_t w_count;

    /* Set main clock control registers */
    // SYSTEM.MOFCR.BYTE = _40_CGC_MAINOSC_EXTERNAL | _20_CGC_MAINOSC_UNDER16M;
    SYSTEM.MOFCR.BYTE = 0x20;
    SYSTEM.MOFCR.BIT.MOFXIN = 1U;

    // for (w_count = 0U; w_count <= 0x014D; w_count++)
    // {
    //     nop();
    // }

    /* Set main clock operation */
    SYSTEM.MOSCCR.BIT.MOSTP = 0U;

    /* Wait for main clock oscillator wait counter overflow */
    while (1U != SYSTEM.OSCOVFSR.BIT.MOOVF)
    {
        /* Do nothing */
    }

    /* Set system clock */
    // SYSTEM.SCKCR.LONG = _00000001_CGC_PCLKD_DIV_2 | _00000010_CGC_PCLKC_DIV_2 | _00000100_CGC_PCLKB_DIV_2 | 
    //                     _00000000_CGC_PCLKA_DIV_1 | _00000000_CGC_BCLK_DIV_1 | _00000000_CGC_ICLK_DIV_1 | 
    //                     _20000000_CGC_FCLK_DIV_4;
    // SYSTEM.SCKCR.LONG = 0x20000111;
    SYSTEM.SCKCR.LONG = 0x00000001 | 0x00000010 | 0x00000100 | 0x20000000;
    /* Set PLL circuit */
    // SYSTEM.PLLCR.WORD = _0000_CGC_PLL_FREQ_DIV_1 | _0000_CGC_PLL_SOURCE_MAIN | _1700_CGC_PLL_FREQ_MUL_12_0;
    SYSTEM.PLLCR.WORD = 0x1700;
    SYSTEM.PLLCR2.BIT.PLLEN = 0U;

    /* Wait for PLL wait counter overflow */
    while (1U != SYSTEM.OSCOVFSR.BIT.PLOVF)
    {
        /* Do nothing */
    }

    /* Stop sub-clock */
    RTC.RCR3.BIT.RTCEN = 0U;

    /* Wait for the register modification to complete */
    while (0U != RTC.RCR3.BIT.RTCEN)
    {
        /* Do nothing */
    }

    /* Stop sub-clock */
    SYSTEM.SOSCCR.BIT.SOSTP = 1U;

    /* Wait for the register modification to complete */
    while (1U != SYSTEM.SOSCCR.BIT.SOSTP)
    {
        /* Do nothing */
    }

    /* Wait for sub-clock oscillation stopping */
    while (0U != SYSTEM.OSCOVFSR.BIT.SOOVF)
    {
        /* Do nothing */
    }

    /* Set UCLK */
    // SYSTEM.SCKCR2.WORD = _0020_CGC_UCLK_DIV_3 | _0001_SCKCR2_BIT0;
    SYSTEM.SCKCR2.WORD = 0x0020 | 0x0001;
    /* Set ROM wait cycle */
    // SYSTEM.ROMWT.BYTE = _02_CGC_ROMWT_CYCLE_2;
    SYSTEM.ROMWT.BYTE = 0x02;
    /* Set SDCLK */
    SYSTEM.SCKCR.BIT.PSTOP0 = 1U;

    /* Set clock source */
    // SYSTEM.SCKCR3.WORD = _0400_CGC_CLOCKSOURCE_PLL;
    SYSTEM.SCKCR3.WORD = 0x0400;
    /* Set LOCO */
    SYSTEM.LOCOCR.BIT.LCSTP = 1U;
}
#endif


#ifdef CLK_HSE_10MHz_PLL_120MHz
/***********************************************************************************************************************
* Function Name: R_CGC_Create
* Description  : This function initializes the clock generator.
* Arguments    : None
* Return Value : None
@note  : HSE/10MHz -> PLL/120MHz
***********************************************************************************************************************/
void R_CGC_Create(void)
{
	// uint16_t w_count;

    /* Set main clock control registers */
    // SYSTEM.MOFCR.BYTE = _40_CGC_MAINOSC_EXTERNAL | _20_CGC_MAINOSC_UNDER16M;
    SYSTEM.MOFCR.BYTE = 0x20;
    SYSTEM.MOFCR.BIT.MOFXIN = 1U;

    // for (w_count = 0U; w_count <= 0x014D; w_count++)
    // {
    //     nop();
    // }

    /* Set main clock operation */
    SYSTEM.MOSCCR.BIT.MOSTP = 0U;

    /* Wait for main clock oscillator wait counter overflow */
    while (1U != SYSTEM.OSCOVFSR.BIT.MOOVF)
    {
        /* Do nothing */
    }

    /* Set system clock */
    // SYSTEM.SCKCR.LONG = _00000001_CGC_PCLKD_DIV_2 | _00000010_CGC_PCLKC_DIV_2 | _00000100_CGC_PCLKB_DIV_2 | 
    //                     _00000000_CGC_PCLKA_DIV_1 | _00000000_CGC_BCLK_DIV_1 | _00000000_CGC_ICLK_DIV_1 | 
    //                     _20000000_CGC_FCLK_DIV_4;
    // SYSTEM.SCKCR.LONG = 0x20000111;
    SYSTEM.SCKCR.LONG = 0x00000001 | 0x00000010 | 0x00000100 | 0x20000000;
    /* Set PLL circuit */
    // SYSTEM.PLLCR.WORD = _0000_CGC_PLL_FREQ_DIV_1 | _0000_CGC_PLL_SOURCE_MAIN | _1700_CGC_PLL_FREQ_MUL_12_0;
    SYSTEM.PLLCR.WORD = 0x1700;
    SYSTEM.PLLCR2.BIT.PLLEN = 0U;

    /* Wait for PLL wait counter overflow */
    while (1U != SYSTEM.OSCOVFSR.BIT.PLOVF)
    {
        /* Do nothing */
    }

    /* Stop sub-clock */
    RTC.RCR3.BIT.RTCEN = 0U;

    /* Wait for the register modification to complete */
    while (0U != RTC.RCR3.BIT.RTCEN)
    {
        /* Do nothing */
    }

    /* Stop sub-clock */
    SYSTEM.SOSCCR.BIT.SOSTP = 1U;

    /* Wait for the register modification to complete */
    while (1U != SYSTEM.SOSCCR.BIT.SOSTP)
    {
        /* Do nothing */
    }

    /* Wait for sub-clock oscillation stopping */
    while (0U != SYSTEM.OSCOVFSR.BIT.SOOVF)
    {
        /* Do nothing */
    }

    /* Set UCLK */
    // SYSTEM.SCKCR2.WORD = _0020_CGC_UCLK_DIV_3 | _0001_SCKCR2_BIT0;
    SYSTEM.SCKCR2.WORD = 0x0020 | 0x0001;
    /* Set ROM wait cycle */
    // SYSTEM.ROMWT.BYTE = _02_CGC_ROMWT_CYCLE_2;
    SYSTEM.ROMWT.BYTE = 0x02;
    /* Set SDCLK */
    SYSTEM.SCKCR.BIT.PSTOP0 = 1U;

    /* Set clock source */
    // SYSTEM.SCKCR3.WORD = _0400_CGC_CLOCKSOURCE_PLL;
    SYSTEM.SCKCR3.WORD = 0x0400;
    /* Set LOCO */
    SYSTEM.LOCOCR.BIT.LCSTP = 1U;

}
#endif

#ifdef CLK_HSI_16MHz
/***********************************************************************************************************************
* Function Name: R_CGC_Create
* Description  : This function initializes the clock generator.
* Arguments    : None
* Return Value : None
@note:  HSI/16MHz
***********************************************************************************************************************/
void R_CGC_Create(void)
{
    uint16_t w_count;

    /* Set system clock */
    // SYSTEM.SCKCR.LONG = _00000001_CGC_PCLKD_DIV_2 | _00000010_CGC_PCLKC_DIV_2 | _00000100_CGC_PCLKB_DIV_2 | 
    //                     _00000000_CGC_PCLKA_DIV_1 | _00000000_CGC_BCLK_DIV_1 | _00000000_CGC_ICLK_DIV_1 | 
    //                     _10000000_CGC_FCLK_DIV_2;
    SYSTEM.SCKCR.LONG = 0x10000111;
    /* Set HOCO */
    SYSTEM.HOCOCR.BIT.HCSTP = 1U;
    SYSTEM.HOCOPCR.BIT.HOCOPCNT = 1U;
    // SYSTEM.HOCOCR2.BYTE = _00_CGC_HOCO_CLK_16;
    SYSTEM.HOCOCR2.BYTE = 0x00;
    SYSTEM.HOCOPCR.BIT.HOCOPCNT = 0U;

    // for (w_count = 0U; w_count <= _0009_CGC_HOCOP_WAIT; w_count++)
    for (w_count = 0U; w_count <= 0x0009; w_count++)
    {
        nop();
    }

    SYSTEM.HOCOCR.BIT.HCSTP = 0U;

    /* Wait for HOCO wait counter overflow */
    while (1U != SYSTEM.OSCOVFSR.BIT.HCOVF)
    {
        /* Do nothing */
    }

    /* Stop sub-clock */
    RTC.RCR3.BIT.RTCEN = 0U;

    /* Wait for the register modification to complete */
    while (0U != RTC.RCR3.BIT.RTCEN)
    {
        /* Do nothing */
    }

    /* Stop sub-clock */
    SYSTEM.SOSCCR.BIT.SOSTP = 1U;

    /* Wait for the register modification to complete */
    while (1U != SYSTEM.SOSCCR.BIT.SOSTP)
    {
        /* Do nothing */
    }

    /* Wait for sub-clock oscillation stopping */
    while (0U != SYSTEM.OSCOVFSR.BIT.SOOVF)
    {
        /* Do nothing */
    }

    /* Set UCLK */
    // SYSTEM.SCKCR2.WORD = _0020_CGC_UCLK_DIV_3 | _0001_SCKCR2_BIT0;
    SYSTEM.SCKCR2.WORD = 0x0021;
    /* Set ROM wait cycle */
    // SYSTEM.ROMWT.BYTE = _02_CGC_ROMWT_CYCLE_2;
    SYSTEM.ROMWT.BYTE = 0x02;
    /* Set SDCLK */
    SYSTEM.SCKCR.BIT.PSTOP0 = 1U;

    /* Set clock source */
    // SYSTEM.SCKCR3.WORD = _0100_CGC_CLOCKSOURCE_HOCO;
    SYSTEM.SCKCR3.WORD = 0x0100;
    /* Set LOCO */
    SYSTEM.LOCOCR.BIT.LCSTP = 1U;
}
#endif


