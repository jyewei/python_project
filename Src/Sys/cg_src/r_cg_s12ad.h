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
* File Name    : r_cg_s12ad.h
* Version      : Code Generator for RX65N V1.01.02.03 [08 May 2018]
* Device(s)    : R5F565N9FxFP
* Tool-Chain   : CCRX
* Description  : This file implements device driver for S12AD module.
* Creation Date: 2023/6/1
***********************************************************************************************************************/
#ifndef S12AD_H
#define S12AD_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    A/D Control Register (ADCSR)
*/
/* Group B Scan End Interrupt Enable (GBADIE) */
#define _0000_AD_GBADI_DISABLE                 (0x0000U) /* Disables S12GBADI interrupt generation upon group B scan
                                                            completion */
#define _0040_AD_GBADI_ENABLE                  (0x0040U) /* Enables S12GBADI interrupt generation upon group B scan
                                                            completion */
/* Double Trigger Mode Select (DBLE) */
#define _0000_AD_DBLTRIGGER_DISABLE            (0x0000U) /* Disable double trigger mode */
#define _0080_AD_DBLTRIGGER_ENABLE             (0x0080U) /* Enable double trigger mode */
/* Trigger Select (EXTRG) */
#define _0000_AD_SYNC_TRIGGER                  (0x0000U) /* A/D conversion started by synchronous trigger */
#define _0100_AD_ASYNC_TRIGGER                 (0x0100U) /* A/D conversion started by asynchronous trigger */
/* Trigger Start Enable (TRGE) */             
#define _0000_AD_SYNCASYNCTRG_DISABLE          (0x0000U) /* A/D conversion synchronous 
                                                            or asynchronous trigger disable */
#define _0200_AD_SYNCASYNCTRG_ENABLE           (0x0200U) /* A/D conversion synchronous or asynchronous trigger enable */
/* Scan End Interrupt Enable (ADIE) */
#define _0000_AD_SCAN_END_INTERRUPT_DISABLE    (0x0000U) /* Disable S12ADI0 interrupt generation upon scan completion */
#define _1000_AD_SCAN_END_INTERRUPT_ENABLE     (0x1000U) /* Enable S12ADI0 interrupt generation upon scan completion */
/* Scan Mode Select (ADCS) */                 
#define _0000_AD_SINGLE_SCAN_MODE              (0x0000U) /* Single scan mode */
#define _2000_AD_GROUP_SCAN_MODE               (0x2000U) /* Group scan mode */
#define _4000_AD_CONTINUOUS_SCAN_MODE          (0x4000U) /* Continuous scan mode */
/* A/D Conversion Start (ADST) */
#define _0000_AD_CONVERSION_STOP               (0x0000U) /* Stop A/D conversion */
#define _8000_AD_CONVERSION_START              (0x8000U) /* Start A/D conversion */

/*
    A/D-Converted Value Addition/Average Count Select Register (ADADC)
*/
/* Addition Count Select (ADC[2:0]) */
#define _00_AD_1_TIME_CONVERSION               (0x00U) /* 1-time conversion */
#define _01_AD_2_TIME_CONVERSION               (0x01U) /* 2-time conversion */
#define _02_AD_3_TIME_CONVERSION               (0x02U) /* 3-time conversion */
#define _03_AD_4_TIME_CONVERSION               (0x03U) /* 4-time conversion */
#define _05_AD_16_TIME_CONVERSION              (0x05U) /* 16-time conversion */
/* Average Mode Enable (AVEE) */           
#define _00_AD_ADDITION_MODE                   (0x00U) /* Addition mode */
#define _80_AD_AVERAGE_MODE                    (0x80U) /* Average mode */

/*
    A/D Control Extended Register (ADCER)
*/
/* A/D Conversion Resolution Setting (ADPRC) */
#define _0000_AD_RESOLUTION_12BIT              (0x0000U) /* 12-bit resolution */
#define _0002_AD_RESOLUTION_10BIT              (0x0002U) /* 10-bit resolution */
#define _0004_AD_RESOLUTION_8BIT               (0x0004U) /* 8-bit resolution */
/* A/D Data Register Automatic Clearing Enable (ACE) */
#define _0000_AD_AUTO_CLEARING_DISABLE         (0x0000U) /* Disable auto clearing */
#define _0020_AD_AUTO_CLEARING_ENABLE          (0x0020U) /* Enable auto clearing */
/* Self-Diagnosis Conversion Voltage Select (DIAGVAL) */
#define _0000_AD_SELFTDIAGST_DISABLE           (0x0000U) /* Disable self-diagnosis */
#define _0100_AD_SELFTDIAGST_VREFH0_0          (0x0100U) /* Self-diagnosis using a voltage of 0V */
#define _0200_AD_SELFTDIAGST_VREFH0_HALF       (0x0200U) /* Self-diagnosis using a voltage of VREFH0_1/2 */
#define _0300_AD_SELFTDIAGST_VREFH0            (0x0300U) /* Self-diagnosis using a voltage of VREFH0_1 */
#define _0100_AD_SELFTDIAGST_AVCC1_0           (0x0100U) /* Self-diagnosis using a voltage of 0V */
#define _0200_AD_SELFTDIAGST_AVCC1_HALF        (0x0200U) /* Self-diagnosis using a voltage of AVCC1_1/2*/
#define _0300_AD_SELFTDIAGST_AVCC1             (0x0300U) /* Self-diagnosis using a voltage of AVCC1_1*/
/* Self-Diagnosis Mode Select (DIAGLD) */
#define _0000_AD_SELFTDIAGST_ROTATION          (0x0000U) /* Rotation mode for self-diagnosis voltage */
#define _0400_AD_SELFTDIAGST_FIX               (0x0400U) /* Fixed mode for self-diagnosis voltage */
/* Self-Diagnosis Enable (DIAGM) */       
#define _0000_AD_SELFTDIAGST_DISABLE           (0x0000U) /* 12-bit self-diagnosis disable */
#define _0800_AD_SELFTDIAGST_ENABLE            (0x0800U) /* 12-bit self-diagnosis enable */
/* A/D Data Register Format Select (ADRFMT) */
#define _0000_AD_RIGHT_ALIGNMENT               (0x0000U) /* Right-alignment for data register format */
#define _8000_AD_LEFT_ALIGNMENT                (0x8000U) /* Left-alignment for data register format */

/*
    A/D Conversion Start Trigger Select Register (ADSTRGR)
*/
/* A/D Conversion Start Trigger Select for Group B (TRSB) */
#define _0001_AD_TRSB_TRGA0N                   (0x0001U) /* Compare match/input capture from MTU0.TGRA */
#define _0002_AD_TRSB_TRGA1N                   (0x0002U) /* Compare match/input capture from MTU1.TGRA */
#define _0003_AD_TRSB_TRGA2N                   (0x0003U) /* Compare match/input capture from MTU2.TGRA */
#define _0004_AD_TRSB_TRGA3N                   (0x0004U) /* Compare match/input capture from MTU3.TGRA */
#define _0005_AD_TRSB_TRGA4N                   (0x0005U) /* Compare match/input capture from MTU4.TGRA,or an
                                                            underflow of MTU4.TCNT (in the trough) in complementary
                                                            PWM mode */
#define _0006_AD_TRSB_TRGA6N                   (0x0006U) /* Compare match/input capture from MTU6.TGRA */
#define _0007_AD_TRSB_TRGA7N                   (0x0007U) /* Compare match/input capture from MTU7.TGRA,or an
                                                            underflow of MTU7.TCNT (in the trough) in complementary
                                                            PWM mode */
#define _0008_AD_TRSB_TRG0N                    (0x0008U) /* Compare match from MTU0.TGRE */
#define _0009_AD_TRSB_TRG4AN                   (0x0009U) /* Compare match between MTU4.TADCORA and MTU4.TCNT */
#define _000A_AD_TRSB_TRG4BN                   (0x000AU) /* Compare match between MTU4.TADCORB and MTU4.TCNT */
#define _000B_AD_TRSB_TRG4BN_TRG4AN            (0x000BU) /* Compare match between MTU4.TADCORA and MTU4.TCNT, or
                                                            between MTU4.TADCORB and MTU4.TCNT */
#define _000C_AD_TRSB_TRG4ABN                  (0x000CU) /* Compare match between MTU4.TADCORA and MTU4.TCNT, and
                                                            between MTU4.TADCORB and MTU4.TCNT (when interrupt skipping
                                                            function 2 is in use) */
#define _000D_AD_TRSB_TRG7AN                   (0x000DU) /* Compare match between MTU7.TADCORA and MTU7.TCNT */
#define _000E_AD_TRSB_TRG7BN                   (0x000EU) /* Compare match between MTU7.TADCORB and MTU7.TCNT */
#define _000F_AD_TRSB_TRG7AN_TRG7BN            (0x000FU) /* Compare match between MTU7.TADCORA and MTU7.TCNT, or between
                                                             MTU7.TADCORB and MTU7.TCNT */
#define _0010_AD_TRSB_TRG7ABN                  (0x0010U) /* Compare match between MTU7.TADCORA and MTU7.TCNT,
                                                            and between MTU7.TADCORB and MTU7.TCNT 
                                                            (when interrupt skipping function 2 is in use) */
#define _001D_AD_TRSB_TMTRG0AN_0               (0x001DU) /* Compare match between TMR0.TCORA0
                                                            and TMR0.TCNT0(unit0,ch0) */
#define _001E_AD_TRSB_TMTRG0AN_1               (0x001EU) /* Compare match between TMR2.TCORA0
                                                            and TMR2.TCNT0(unit1,ch0) */
#define _001F_AD_TRSB_TPTRGAN                  (0x001FU) /* Compare match/input capture from TPUn.TGRAn(n = 0to5) */
#define _0020_AD_TRSB_TPTRG0AN                 (0x0020U) /* Compare match/input capture from TPU0.TGRA0 */
#define _0030_AD_TRSB_ELCTRG0N_ELCTRG1N        (0x0030U) /* Trigger from ELC */
/* A/D Conversion Start Trigger Select (TRSA) */
#define _0000_AD_TRSA_ADTRG                    (0x0000U) /* Input pin for the trigger */
#define _0100_AD_TRSA_TRGA0N                   (0x0100U) /* Compare match/input capture from MTU0.TGRA */
#define _0200_AD_TRSA_TRGA1N                   (0x0200U) /* Compare match/input capture from MTU1.TGRA */
#define _0300_AD_TRSA_TRGA2N                   (0x0300U) /* Compare match/input capture from MTU2.TGRA */
#define _0400_AD_TRSA_TRGA3N                   (0x0400U) /* Compare match/input capture from MTU3.TGRA */
#define _0500_AD_TRSA_TRGA4N                   (0x0500U) /* Compare match/input capture from MTU4.TGRA or, in
                                                            complementary PWM mode,an underflow of MTU4.TCNT
                                                            (in the trough)*/
#define _0600_AD_TRSA_TRGA6N                   (0x0600U) /* Compare match/input capture from MTU6.TGRA */
#define _0700_AD_TRSA_TRGA7N                   (0x0700U) /* Compare match/input capture from MTU7.TGRA or, in
                                                            complementary PWM mode,an underflow of MTU7.TCNT
                                                            (in the trough)*/
#define _0800_AD_TRSA_TRG0N                    (0x0800U) /* Compare match from MTU0.TGRE */
#define _0900_AD_TRSA_TRG4AN                   (0x0900U) /* Compare match between MTU4.TADCORA and MTU4.TCNT */
#define _0A00_AD_TRSA_TRG4BN                   (0x0A00U) /* Compare match between MTU4.TADCORB and MTU4.TCNT */
#define _0B00_AD_TRSA_TRG4BN_TRG4AN            (0x0B00U) /* Compare match between MTU4.TADCORA and MTU4.TCNT, or between
                                                            MTU4.TADCORB and MTU4.TCNT */
#define _0C00_AD_TRSA_TRG4ABN                  (0x0C00U) /* Compare match between MTU4.TADCORA and MTU4.TCNT,
                                                            and between MTU4.TADCORB and MTU4.TCNT
                                                            (when interrupt skipping function 2 is in use) */
#define _0D00_AD_TRSA_TRG7AN                   (0x0D00U) /* Compare match between MTU7.TADCORA and MTU7.TCNT */
#define _0E00_AD_TRSA_TRG7BN                   (0x0E00U) /* Compare match between MTU7.TADCORB and MTU7.TCNT */
#define _0F00_AD_TRSA_TRG7AN_TRG7BN            (0x0F00U) /* Compare match between MTU7.TADCORA and MTU7.TCNT, or between
                                                            MTU7.TADCORB and MTU7.TCNT */
#define _1000_AD_TRSA_TRG7ABN                  (0x1000U) /* Compare match between MTU7.TADCORA and MTU7.TCNT,
                                                            and between MTU7.TADCORB and MTU7.TCNT
                                                            (when interrupt skipping function 2 is in use) */
#define _1D00_AD_TRSA_TMTRG0AN_0               (0x1D00U) /* Compare match between TMR0.TCORA0
                                                            and TMR0.TCNT0(unit0,ch0) */
#define _1E00_AD_TRSA_TMTRG0AN_1               (0x1E00U) /* Compare match between TMR2.TCORA0
                                                            and TMR2.TCNT0(unit1,ch0) */
#define _1F00_AD_TRSA_TPTRGAN                  (0x1F00U) /* Compare match/input capture from TPUn.TGRAn(n=0 to 5) */
#define _2000_AD_TRSA_TPTRG0AN                 (0x2000U) /* Compare match/input capture from TPU0.TGRAn */
#define _3000_AD_TRSA_ELCTRG0N_ELCTRG1N        (0x3000U) /* Trigger from ELC */

/*
    A/D Conversion Extended Input Control Register (ADEXICR)
*/
/* Temperature Sensor Output A/D Converted Value Addition/Averaging Mode Select (TSSAD) */
#define _0000_AD_TEMP_ADDITION_DISABLE         (0x0000U) /* Temperature sensor output A/D converted value 
                                                            addition/average mode disabled */
#define _0001_AD_TEMP_ADDITION_ENABLE          (0x0001U) /* Temperature sensor output A/D converted value
                                                            addition/average mode enabled */
/* Internal Reference Voltage A/DConverted Value Addition/Average Mode Select (OCSAD) */
#define _0000_AD_IREF_ADDITION_DISABLE         (0x0000U) /* Internal ref. voltage A/D converted value addition/average 
                                                            mode disabled */
#define _0002_AD_IREF_ADDITION_ENABLE          (0x0002U) /* Internal ref. voltage A/D converted value addition/average
                                                            mode enabled */
/* Temperature Sensor Output A/D Conversion Select (TSSA) */
#define _0000_AD_TEMP_GROUPA_DISABLE           (0x0000U) /* A/D conversion of temperature sensor output is disabled in 
                                                            group A  */
#define _0100_AD_TEMP_GROUPA_ENABLE            (0x0100U) /* A/D conversion of temperature sensor output is enabled in 
                                                            group A  */
/* Temperature Sensor Output A/D Conversion Select (TSSB) */
#define _0000_AD_TEMP_GROUPB_DISABLE           (0x0000U) /* A/D conversion of temperature sensor output is disabled in 
                                                            group B  */
#define _0400_AD_TEMP_GROUPB_ENABLE            (0x0400U) /* A/D conversion of temperature sensor output is enabled in 
                                                            group B  */
/* Internal Reference Voltage A/D Conversion Select (OCSA) */
#define _0000_AD_IREF_GROUPA_DISABLE           (0x0000U) /* A/D conversion of internal reference voltage is disabled in 
                                                            group A  */
#define _0200_AD_IREF_GROUPA_ENABLE            (0x0200U) /* A/D conversion of internal reference voltage is enabled in 
                                                            group A  */
/* Internal Reference Voltage A/D Conversion Select (OCSB) */
#define _0000_AD_IREF_GROUPB_DISABLE           (0x0000U) /* A/D conversion of internal reference voltage is disabled in 
                                                            group B  */
#define _0800_AD_IREF_GROUPB_ENABLE            (0x0800U) /* A/D conversion of internal reference voltage is enabled in 
                                                            group B  */
/* Extended Analog Input Select (EXSEL) */  
#define _0000_AD_EXTNANEX1_IN_DISABLE          (0x0000U) /* Extended analog input disable */
#define _2000_AD_EXTNANEX1_IN_ENABLE           (0x2000U) /* Extended analog input enable */
/* Extended Analog Output Control (EXOEN) */   
#define _0000_AD_EXTNANEX0_OUT_DISABLE         (0x0000U) /* Extended analog output disable */
#define _8000_AD_EXTNANEX0_OUT_ENABLE          (0x8000U) /* Extended analog output enable */

/*
    A/D Group C Extended Input Control Register (ADGCEXCR)
*/
/* Group C Temperature Sensor Output A/D Conversion Select (TSSC) */
#define _00_AD_TEMP_GROUPC_DISABLE             (0x00U) /* A/D conversion of temperature sensor output is disabled in 
                                                          group C  */
#define _01_AD_TEMP_GROUPC_ENABLE              (0x01U) /* A/D conversion of temperature sensor output is enabled in 
                                                          group C  */
/* Group C Internal Reference Voltage A/D Conversion Select (OCSC) */
#define _00_AD_IREF_GROUPC_DISABLE             (0x00U) /* A/D conversion of internal reference voltage is disabled in 
                                                          group C  */
#define _02_AD_IREF_GROUPC_ENABLE              (0x02U) /* A/D conversion of internal reference voltage is enabled in 
                                                          group C  */

/*
    A/D Group C Trigger Select Register (ADGCTRGR)
*/
/* Group C A/D Conversion Start Trigger Select (TRSC) */
#define _01_AD_TRSC_TRGA0N                     (0x01U) /* Compare match/input capture from MTU0.TGRA */
#define _02_AD_TRSC_TRGA1N                     (0x02U) /* Compare match/input capture from MTU1.TGRA */
#define _03_AD_TRSC_TRGA2N                     (0x03U) /* Compare match/input capture from MTU2.TGRA */
#define _04_AD_TRSC_TRGA3N                     (0x04U) /* Compare match/input capture from MTU3.TGRA */
#define _05_AD_TRSC_TRGA4N                     (0x05U) /* Compare match/input capture from MTU4.TGRA,or an
                                                          underflow of MTU4.TCNT (in the trough) in complementary
                                                          PWM mode */
#define _06_AD_TRSC_TRGA6N                     (0x06U) /* Compare match/input capture from MTU6.TGRA */
#define _07_AD_TRSC_TRGA7N                     (0x07U) /* Compare match/input capture from MTU7.TGRA,or an
                                                          underflow of MTU7.TCNT (in the trough) in complementary
                                                          PWM mode */
#define _08_AD_TRSC_TRG0N                      (0x08U) /* Compare match from MTU0.TGRE */
#define _09_AD_TRSC_TRG4AN                     (0x09U) /* Compare match between MTU4.TADCORA and MTU4.TCNT */
#define _0A_AD_TRSC_TRG4BN                     (0x0AU) /* Compare match between MTU4.TADCORB and MTU4.TCNT */
#define _0B_AD_TRSC_TRG4BN_TRG4AN              (0x0BU) /* Compare match between MTU4.TADCORA and MTU4.TCNT, or
                                                          between MTU4.TADCORB and MTU4.TCNT */
#define _0C_AD_TRSC_TRG4ABN                    (0x0CU) /* Compare match between MTU4.TADCORA and MTU4.TCNT, and
                                                          between MTU4.TADCORB and MTU4.TCNT (when interrupt skipping
                                                          function 2 is in use) */
#define _0D_AD_TRSC_TRG7AN                     (0x0DU) /* Compare match between MTU7.TADCORA and MTU7.TCNT */
#define _0E_AD_TRSC_TRG7BN                     (0x0EU) /* Compare match between MTU7.TADCORB and MTU7.TCNT */
#define _0F_AD_TRSC_TRG7AN_TRG7BN              (0x0FU) /* Compare match between MTU7.TADCORA and MTU7.TCNT, or between
                                                          MTU7.TADCORB and MTU7.TCNT */
#define _10_AD_TRSC_TRG7ABN                    (0x10U) /* Compare match between MTU7.TADCORA and MTU7.TCNT, and between
                                                          MTU7.TADCORB and MTU7.TCNT (when interrupt skipping function
                                                          2 is in use) */
#define _1D_AD_TRSC_TMTRG0AN_0                 (0x1DU) /* Compare match between TMR0.TCORA0 and TMR0.TCNT0(unit0,ch0) */
#define _1E_AD_TRSC_TMTRG0AN_1                 (0x1EU) /* Compare match between TMR2.TCORA0 and TMR2.TCNT0(unit1,ch0) */
#define _1F_AD_TRSC_TPTRGAN                    (0x1FU) /* Compare match/input capture from TPUn.TGRAn(n = 0to5) */
#define _20_AD_TRSC_TPTRG0AN                   (0x20U) /* Compare match/input capture from TPU0.TGRA0 */
#define _30_AD_TRSC_ELCTRG0N_ELCTRG1N          (0x30U) /* Trigger from ELC */
/* Group C Scan End Interrupt Enable (GCADIE) */
#define _00_AD_GCADI_DISABLE                   (0x00U) /* Disables interrupt generation after completion
                                                          of group C scan */
#define _40_AD_GCADI_ENABLE                    (0x40U) /* Enables interrupt generation after completion
                                                          of group C scan */
/* Group C A/D Conversion Operation Enable (GRCE) */
#define _00_AD_GROUPC_DISABLE                  (0x00U) /* Disables A/D conversion operation for group C. */
#define _80_AD_GROUPC_ENABLE                   (0x80U) /* Enables A/D conversion operation for group C */

/*
    A/D Group Scan Priority Control Register (ADGSPCR)
*/
/* Group Priority Control Setting (PGS) */
#define _0000_AD_GPAPRIORITY_DISABLE           (0x0000U) /* Operation is without group A priority control */
#define _0001_AD_GPAPRIORITY_ENABLE            (0x0001U) /* Operation is with group A priority control */
/* Low-Priority Group Restart Setting (GBRSCN) */
#define _0000_AD_GPBRESTART_DISABLE            (0x0000U) /* Group B not restart after discontinued due to Group A
                                                            priority */
#define _0002_AD_GPBRESTART_ENABLE             (0x0002U) /* Group B restart after discontinued
                                                            due to Group A priority */
/* Restart Channel Select (LGRRS) */           
#define _0000_AD_RESTART_START_CHANNEL         (0x0000U) /* Scanning is restarted from the scan start channel */
#define _4000_AD_RESTART_INCOMPLETED_CHANNEL   (0x4000U) /* Scanning is restarted from the channel on which A/D 
                                                            conversion is not completed */
/* Single Scan Continuous Start* (GBRP) */
#define _0000_AD_GPBSCSCS_DISABLE              (0x0000U) /* Single cycle scan for group B not continuously activated */
#define _8000_AD_GPBSCSCS_ENABLE               (0x8000U) /* Single cycle scan for group B is continuously activated */

/* 
    A/D Comparison Function Control Register (ADCMPCR)
*/
/* Window A/B Complex Conditions Setting (CMPAB) */
#define _0000_AD_COMPLEX_CONDITION_OR          (0x0000U) /* Window A comparison condition matched OR window B
                                                           comparison condition matched */ 
#define _0001_AD_COMPLEX_CONDITION_EXOR        (0x0001U) /* Window A comparison condition matched EXOR window B
                                                           comparison condition matched */ 
#define _0002_AD_COMPLEX_CONDITION_AND         (0x0002U) /* Window A comparison condition matched AND window B
                                                           comparison condition matched */ 
/* Comparison Window B Enable (CMPBE) */
#define _0000_AD_WINDOWB_DISABLE               (0x0000U) /* Comparison window B disabled */ 
#define _0200_AD_WINDOWB_ENABLE                (0x0200U) /* Comparison window B enabled */
/* Comparison Window A Enable (CMPAE) */
#define _0000_AD_WINDOWA_DISABLE               (0x0000U) /* Comparison window B disabled */ 
#define _0800_AD_WINDOWA_ENABLE                (0x0800U) /* Comparison window B enabled */
/* Comparison Window B Interrupt Enable (CMPBIE) */
#define _0000_AD_COMPARISON_INTERRUPTB_DISABLE (0x0000U) /* Comparison interrupt by a match with the comparison 
                                                            condition (window B) is disabled */ 
#define _2000_AD_COMPARISON_INTERRUPTB_ENABLE  (0x2000U) /* Comparison interrupt by a match with the comparison 
                                                           condition (window B) is enabled */ 
/* Window Function Setting (WCMPE) */
#define _0000_AD_WINDOWFUNCTION_DISABLE        (0x0000U) /* Window function disabled */ 
#define _4000_AD_WINDOWFUNCTION_ENABLE         (0x4000U) /* Window function enabled */
/* Comparison Window A Interrupt Enable (CMPAIE) */
#define _0000_AD_COMPARISON_INTERRUPTA_DISABLE (0x0000U) /* Comparison interrupt by a match with the comparison 
                                                            condition (window A) is disabled */ 
#define _8000_AD_COMPARISON_INTERRUPTA_ENABLE  (0x8000U) /* Comparison interrupt by a match with the comparison 
                                                            condition (window A) is enabled */ 

/* 
    A/D Comparison Function Window A Extended Input Select Register (ADCMPANSER)
*/
/* Temperature Sensor Output Comparison Select (CMPSTS) */
#define _00_AD_TEMP_COMPARE_DISABLE            (0x00U) /* Temperature sensor output is not a target for comparison. */ 
#define _01_AD_TEMP_COMPARE_ENABLE             (0x01U) /* Temperature sensor output is a target for comparison. */
/* Internal Reference Voltage Compare Select (CMPSOC) */
#define _00_AD_IREF_COMPARE_DISABLE            (0x00U) /* Internal reference voltage is not a target for comparison. */ 
#define _02_AD_IREF_COMPARE_ENABLE             (0x02U) /* Internal reference voltage is a target for comparison. */

/* 
    A/D Comparison Function Window A Extended Input Comparison Condition Setting Register (ADCMPLER)
*/
/* Comparison Window A Temperature Sensor Output Comparison Condition Select (CMPLTS) */
#define _00_AD_TEMP_AD_TEMP_COMPARELEVEL       (0x00U) /* AD-converted value < ADCMPDR0 register value or A/D-converted
                                                          value > ADCMPDR1 register value */ 
#define _01_AD_TEMP_AD_TEMP_COMPARELEVEL       (0x01U) /* ADCMPDR0 register value < A/D-converted value < ADCMPDR1
                                                          register value */
/* Comparison Window A Internal Reference Voltage Comparison Condition Select (CMPLOC) */
#define _00_AD_TEMP_AD_IREF_COMPARELEVEL       (0x00U) /* AD-converted value < ADCMPDR0 register value or A/D-converted
                                                          value > ADCMPDR1 register value */ 
#define _02_AD_TEMP_AD_IREF_COMPARELEVEL       (0x02U) /* ADCMPDR0 register value < A/D-converted value < ADCMPDR1
                                                          register value */

/* 
    A/D Conversion Time Setting Register (ADSAM)
*/
/* Conversion Time Setting (SAM) */
#define _0000_AD_CONVERSION_FOR_HIGHSPEED      (0x0000U) /* Sets conversion time for high-speed. */ 
#define _0020_AD_CONVERSION_FOR_MIDDLESPEED    (0x0020U) /* Sets conversion time for middle-speed. */

/* 
    A/D Conversion Time Setting Protection Release Register (ADSAMPR)
*/
/* A/D Conversion Time Setting Register Protection (PRO[1:0]) */
#define _00_AD_WRITE_CONVERSION_INIT           (0x00U) /* Disable writing to the A/D conversion time setting register
                                                          (initial value). */ 
#define _02_AD_WRITE_CONVERSION_DISABLE        (0x02U) /* Disable writing to the A/D conversion time setting 
                                                          register. */
#define _03_AD_WRITE_CONVERSION_ENABLE         (0x03U) /* Enable writing to the A/D conversion time setting register. */

/*
    Interrupt Source Priority Register n (IPRn)
*/
/* Interrupt Priority Level Select (IPR[3:0]) */
#define _00_AD_PRIORITY_LEVEL0                 (0x00U) /* Level 0 (interrupt disabled) */
#define _01_AD_PRIORITY_LEVEL1                 (0x01U) /* Level 1 */
#define _02_AD_PRIORITY_LEVEL2                 (0x02U) /* Level 2 */
#define _03_AD_PRIORITY_LEVEL3                 (0x03U) /* Level 3 */
#define _04_AD_PRIORITY_LEVEL4                 (0x04U) /* Level 4 */
#define _05_AD_PRIORITY_LEVEL5                 (0x05U) /* Level 5 */
#define _06_AD_PRIORITY_LEVEL6                 (0x06U) /* Level 6 */
#define _07_AD_PRIORITY_LEVEL7                 (0x07U) /* Level 7 */
#define _08_AD_PRIORITY_LEVEL8                 (0x08U) /* Level 8 */
#define _09_AD_PRIORITY_LEVEL9                 (0x09U) /* Level 9 */
#define _0A_AD_PRIORITY_LEVEL10                (0x0AU) /* Level 10 */
#define _0B_AD_PRIORITY_LEVEL11                (0x0BU) /* Level 11 */
#define _0C_AD_PRIORITY_LEVEL12                (0x0CU) /* Level 12 */
#define _0D_AD_PRIORITY_LEVEL13                (0x0DU) /* Level 13 */
#define _0E_AD_PRIORITY_LEVEL14                (0x0EU) /* Level 14 */
#define _0F_AD_PRIORITY_LEVEL15                (0x0FU) /* Level 15 (highest) */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define _00FF_AD0_CHANNEL_SELECT_A0            (0x00FFU) /* Channel select register A0 value */
#define _00FF_AD0_ADDAVG_CHANNEL_SELECT0       (0x00FFU) /* Converted value add/average function select register 0 */
#define _00_AD0_DISCONECT_SETTING              (0x00U) /* Disconnection detection assist setting */
#define _FA_AD0_SAMPLING_STATE_0               (0xFAU) /* AN000 sampling state register value */
#define _FA_AD0_SAMPLING_STATE_1               (0xFAU) /* AN001 sampling state register value */
#define _FA_AD0_SAMPLING_STATE_2               (0xFAU) /* AN002 sampling state register value */
#define _FA_AD0_SAMPLING_STATE_3               (0xFAU) /* AN003 sampling state register value */
#define _FA_AD0_SAMPLING_STATE_4               (0xFAU) /* AN004 sampling state register value */
#define _FA_AD0_SAMPLING_STATE_5               (0xFAU) /* AN005 sampling state register value */
#define _FA_AD0_SAMPLING_STATE_6               (0xFAU) /* AN006 sampling state register value */
#define _FA_AD0_SAMPLING_STATE_7               (0xFAU) /* AN007 sampling state register value */
#define _0000_AD0_DSH_CHANNEL_SELECT           (0x0000U) /* Channel-dedicated sample-and-hold circuit bypass select */
#define _3FFF_AD1_CHANNEL_SELECT_A0            (0x3FFFU) /* Channel select register A0 value */
#define _0000_AD1_CHANNEL_SELECT_A1            (0x0000U) /* Channel select register A1 value */
#define _0000_AD1_ADDAVG_CHANNEL_SELECT0       (0x0000U) /* Converted value add/average function select register 0 */
#define _0000_AD1_ADDAVG_CHANNEL_SELECT1       (0x0000U) /* Converted value add/average function select register 1 */
#define _00_AD1_DISCONECT_SETTING              (0x00U) /* Disconnection detection assist setting */
#define _B7_AD1_SAMPLING_STATE_0               (0xB7U) /* AN100 sampling state register value */
#define _B7_AD1_SAMPLING_STATE_1               (0xB7U) /* AN101 sampling state register value */
#define _B7_AD1_SAMPLING_STATE_2               (0xB7U) /* AN102 sampling state register value */
#define _B7_AD1_SAMPLING_STATE_3               (0xB7U) /* AN103 sampling state register value */
#define _B7_AD1_SAMPLING_STATE_4               (0xB7U) /* AN104 sampling state register value */
#define _B7_AD1_SAMPLING_STATE_5               (0xB7U) /* AN105 sampling state register value */
#define _B7_AD1_SAMPLING_STATE_6               (0xB7U) /* AN106 sampling state register value */
#define _B7_AD1_SAMPLING_STATE_7               (0xB7U) /* AN107 sampling state register value */
#define _B7_AD1_SAMPLING_STATE_8               (0xB7U) /* AN108 sampling state register value */
#define _B7_AD1_SAMPLING_STATE_9               (0xB7U) /* AN109 sampling state register value */
#define _B7_AD1_SAMPLING_STATE_10              (0xB7U) /* AN110 sampling state register value */
#define _B7_AD1_SAMPLING_STATE_11              (0xB7U) /* AN111 sampling state register value */
#define _B7_AD1_SAMPLING_STATE_12              (0xB7U) /* AN112 sampling state register value */
#define _B7_AD1_SAMPLING_STATE_13              (0xB7U) /* AN113 sampling state register value */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef enum
{
    ADCHANNEL0, ADCHANNEL1, ADCHANNEL2, ADCHANNEL3, ADCHANNEL4, ADCHANNEL5, ADCHANNEL6,
    ADCHANNEL7, ADCHANNEL8, ADCHANNEL9, ADCHANNEL10, ADCHANNEL11, ADCHANNEL12,
    ADCHANNEL13, ADCHANNEL14, ADCHANNEL15, ADCHANNEL16, ADCHANNEL17, ADCHANNEL18,
    ADCHANNEL19, ADCHANNEL20, ADSELFDIAGNOSIS, ADTEMPSENSOR, ADINTERREFVOLT,ADDATADUPLICATION,
    ADDATADUPLICATIONA, ADDATADUPLICATIONB
} ad_channel_t;

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_S12AD0_Create(void);
void R_S12AD0_Start(void);
void R_S12AD0_Stop(void);
void R_S12AD0_Get_ValueResult(ad_channel_t channel, uint16_t * const buffer);
void R_S12AD1_Create(void);
void R_S12AD1_Start(void);
void R_S12AD1_Stop(void);
void R_S12AD1_Get_ValueResult(ad_channel_t channel, uint16_t * const buffer);

/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
