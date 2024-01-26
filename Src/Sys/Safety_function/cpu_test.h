/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*******************************************************************************/
/* Copyright (C) 2010 Renesas Electronics Corporation. All rights reserved. */
/**********************************************************************
* File Name : cpu_test.h
* Version : 1.00
* Device : RX62T
* Tool Chain : Renesas RX Standard Toolchain
* H/W Platform : RSK RX62T
* Description : Self Test code.
*               CPU Tests.
*				This file doesn't include the implementation of the
*				CPU General registers coupling tests
*				 - see cpu_test_coupling.c
*				 
*				 This file should be built with optimisation off.
*				 A prgama is specified below to ensure this.
**********************************************************************/


/**********************************************************************************
Revision History
DD-MMM-YYYY OSO-UID Description
20-Jan-2011 RTE-PJI First Release
03-Feb-2011 VDE (Version 1.0)
***********************************************************************************/

#ifndef CPU_TEST_H
#define CPU_TEST_H

/***********************************************************************************
 User Includes
***********************************************************************************/
//#include "misratypes.h"
// #include	"typedefine.h"
#include "common.h"
// #define 	uint32_t	uint32_t

/***********************************************************************************
 Function Prototypes
***********************************************************************************/
/*NOTE: Interrupts must be prevented during these test*/

/* This function must be provided by the user of these tests.
It will be called if an error is detected by any of these tests.*/
//extern void CPU_Test_ErrorHandler(void);

/*
NOTE: These functions do not need a return value because if they return
without having called CPU_Test_ErrorHandler then that indicates
that they have passed.
*/

/*This function performs all CPU tests.
The general purpose registers are tested either using the coupling or
the non-coupling tests depending upon #define USE_TestGPRsCoupling.*/
//void CPU_Test_All(void);
uint32_t CPU_Test_All( void );

/*These individual tests can be called in any order but it makes sense
to call them in the order they are presented here.*/

/*GPR Register tests - coupling test version.
Split into two functions - PartA and PartB.
NOTE: If using these it is not necessary to also use the non-coupling version. */
void CPU_Test_GPRsCouplingPartA(void);
void CPU_Test_GPRsCouplingPartB(void);

/*GPR Register tests - non coupling test version */
void CPU_Test_GeneralA(void);
void CPU_Test_GeneralB(void);

void CPU_Test_Control(void);
void CPU_Test_PC(void);
void CPU_Test_Accumulator(void);

#endif