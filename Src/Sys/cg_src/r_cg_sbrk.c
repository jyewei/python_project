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
* File Name    : r_cg_sbrk.c
* Version      : Code Generator for RX65N V1.01.02.03 [08 May 2018]
* Device(s)    : R5F565N9FxFP
* Tool-Chain   : CCRX
* Description  : Program of sbrk.
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
#include <stddef.h>
#include <stdio.h>
#include "r_cg_sbrk.h"
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/

int8_t  *sbrk(size_t size);

extern int8_t *_s1ptr;

union HEAP_TYPE
{
    int16_t  dummy ;            /* Dummy for 4-byte boundary */
    int8_t heap[HEAPSIZE];      /* Declaration of the area managed by sbrk */
};

static union HEAP_TYPE heap_area ;

/* End address allocated by sbrk    */
static int8_t *brk = (int8_t *) &heap_area;

/***********************************************************************************************************************
* Function Name: sbrk
* Description  : This function allocates memory area.
* Arguments    : size -
                  assigned area size
* Return Value : p -
                  start address of allocated area (Pass)
                  or (-1)                         (Failure)
***********************************************************************************************************************/
int8_t *sbrk(size_t size)                       /* Assigned area size   */
{
    int8_t  *p;

    if ((brk+size) > (heap_area.heap + HEAPSIZE))   /* Empty area size      */
    {
        p = (int8_t *)-1;
    }
    else
    {
        p = brk;                                /* Area assignment      */
        brk += size;                            /* End address update   */
    }

    return p;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
