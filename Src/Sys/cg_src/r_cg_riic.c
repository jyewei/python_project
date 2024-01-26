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
* File Name    : r_cg_riic.c
* Version      : Code Generator for RX65N V1.01.02.03 [08 May 2018]
* Device(s)    : R5F565N9FxFP
* Tool-Chain   : CCRX
* Description  : This file implements device driver for RIIC module.
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
#include "r_cg_riic.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
volatile uint8_t  g_riic2_mode_flag;               /* RIIC2 master transmit receive flag */
volatile uint8_t  g_riic2_state;                   /* RIIC2 state */
volatile uint16_t g_riic2_slave_address;           /* RIIC2 slave address */
volatile uint8_t *gp_riic2_tx_address;             /* RIIC2 transmit buffer address */
volatile uint16_t g_riic2_tx_count;                /* RIIC2 transmit data number */
volatile uint8_t *gp_riic2_rx_address;             /* RIIC2 receive buffer address */
volatile uint16_t g_riic2_rx_count;                /* RIIC2 receive data number */
volatile uint16_t g_riic2_rx_length;               /* RIIC2 receive data length */
volatile uint8_t  g_riic2_dummy_read_count;        /* RIIC2 count for dummy read */
volatile uint8_t  g_riic2_stop_generation;         /* RIIC2 stop condition generation flag */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_RIIC2_Create
* Description  : This function initializes the RIIC2 Bus Interface.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_RIIC2_Create(void)
{
    MSTP(RIIC2) = 0U;                /* Cancel RIIC2 module stop state */
    RIIC2.ICCR1.BIT.ICE = 0U;        /* SCL and SDA pins in inactive state */
    RIIC2.ICCR1.BIT.IICRST = 1U;     /* RIIC reset */
    RIIC2.ICCR1.BIT.ICE = 1U;        /* Internal reset */

    /* Set transfer bit rate */
    RIIC2.ICMR1.BYTE |= _30_IIC_PCLK_DIV_8;
    RIIC2.ICBRL.BYTE = _FF_IIC2_SCL_LOW_LEVEL_PERIOD;
    RIIC2.ICBRH.BYTE = _F5_IIC2_SCL_HIGH_LEVEL_PERIOD;

    /* Set ICMR2 and ICMR3 */
    RIIC2.ICMR2.BYTE = _00_IIC_NO_OUTPUT_DELAY;
    RIIC2.ICMR3.BIT.NF = _00_IIC_NOISE_FILTER_1;
    RIIC2.ICMR3.BIT.SMBS = 0U;

    /* Set ICFER */
    RIIC2.ICFER.BYTE = _00_IIC_TIMEOUT_FUNCTION_DISABLE | _02_IIC_MASTER_ARBITRATION_ENABLE | 
                       _00_IIC_NACK_ARBITRATION_DISABLE | _10_IIC_NACK_SUSPENSION_ENABLE | _20_IIC_NOISE_FILTER_USED | 
                       _40_IIC_SCL_SYNCHRONOUS_USED;

    /* Set ICIER */
    RIIC2.ICIER.BYTE = _00_IIC_TIMEOUT_INTERRUPT_DISABLE | _02_IIC_ARBITRATION_LOST_INTERRUPT_ENABLE | 
                       _04_IIC_START_CONDITION_INTERRUPT_ENABLE | _08_IIC_STOP_CONDITION_INTERRUPT_ENABLE | 
                       _00_IIC_NACK_INTERRUPT_DISABLE | _20_IIC_RECEIVE_DATA_INTERRUPT_ENABLE | 
                       _40_IIC_TRANSMIT_END_INTERRUPT_ENABLE | _80_IIC_TRANSMIT_EMPTY_INTERRUPT_ENABLE;

    /* Cancel internal reset */
    RIIC2.ICCR1.BIT.IICRST = 0U;

    /* Set interrupt priority */
    IPR(RIIC2, TXI2) = _0B_IIC_PRIORITY_LEVEL11;
    IPR(RIIC2, RXI2) = _0B_IIC_PRIORITY_LEVEL11;

    /* Set SCL2 pin */
    MPC.P16PFS.BYTE = 0x0FU;
    PORT1.PMR.BYTE |= 0x40U;

    /* Set SDA2 pin */
    MPC.P17PFS.BYTE = 0x0FU;
    PORT1.PMR.BYTE |= 0x80U;
}
/***********************************************************************************************************************
* Function Name: R_RIIC2_Start
* Description  : This function starts the RIIC2 Bus Interface.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_RIIC2_Start(void)
{
    /* Clear interrupt flag */
    IR(RIIC2, TXI2) = 0U;
    IR(RIIC2, RXI2) = 0U;

    /* Enable RIIC2 interrupt */
    IEN(RIIC2, TXI2) = 1U;
    IEN(RIIC2, RXI2) = 1U;
    ICU.GENBL1.BIT.EN15 = 1U;
    ICU.GENBL1.BIT.EN16 = 1U;
}
/***********************************************************************************************************************
* Function Name: R_RIIC2_Stop
* Description  : This function stops the RIIC2 Bus Interface.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_RIIC2_Stop(void)
{
    /* Clear interrupt flag */
    IR(RIIC2, TXI2) = 0U;
    IR(RIIC2, RXI2) = 0U;

    /* Disable RIIC2 interrupt */
    IEN(RIIC2, TXI2) = 0U;
    IEN(RIIC2, RXI2) = 0U;
    ICU.GENBL1.BIT.EN15 = 0U;
    ICU.GENBL1.BIT.EN16 = 0U;
}
/***********************************************************************************************************************
* Function Name: R_RIIC2_Master_Send
* Description  : This function writes data to a slave device and generates stop condition when transmission finishes.
* Arguments    : adr -
*                    address of slave device
*                tx_buf -
*                    transmit buffer pointer
*                tx_num -
*                    transmit data length
* Return Value : status -
*                    MD_OK or MD_ERROR1 or MD_ERROR2
***********************************************************************************************************************/
MD_STATUS R_RIIC2_Master_Send(uint16_t adr, uint8_t * const tx_buf, uint16_t tx_num)
{
    MD_STATUS status = MD_OK;

    if (1U == RIIC2.ICCR2.BIT.BBSY)
    {
        status = MD_ERROR1;
    }
    else if (0x07FFU < adr)
    {
        status = MD_ERROR2;
    }
    else 
    {
        /* Set parameter */
        g_riic2_tx_count = tx_num;
        gp_riic2_tx_address = tx_buf;
        g_riic2_slave_address = adr;
        g_riic2_mode_flag = _0D_IIC_MASTER_TRANSMIT;
        
        if (0xFFU > g_riic2_slave_address)
        {
            g_riic2_state = _01_IIC_MASTER_SENDS_ADR_7_W;
        } 
        else 
        {
            g_riic2_state = _02_IIC_MASTER_SENDS_ADR_10A_W;
        }

        /* Issue a start condition */
        R_RIIC2_StartCondition();

        /* Set flag for generating stop condition when transmission finishes */
        g_riic2_stop_generation = 1;
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_RIIC2_Master_Send_Without_Stop
* Description  : This function write data to a slave device but not generate stop condition when transmission finishes.
* Arguments    : adr -
*                    address of slave device
*                txbuf -
*                    transmit buffer pointer
*                txnum -
*                    transmit data length
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_RIIC2_Master_Send_Without_Stop(uint16_t adr, uint8_t * const tx_buf, uint16_t tx_num)
{
    MD_STATUS status = MD_OK;

    if (1U == RIIC2.ICCR2.BIT.BBSY)
    {
        status = MD_ERROR1;
    }
    else if (0x07FFU < adr)
    {
        status = MD_ERROR2;
    }
    else
    {
        /* Set parameter */
        g_riic2_tx_count = tx_num;
        gp_riic2_tx_address = tx_buf;
        g_riic2_slave_address = adr;
        g_riic2_mode_flag = _0D_IIC_MASTER_TRANSMIT;

        if (0xFFU > g_riic2_slave_address)
        {
            g_riic2_state = _01_IIC_MASTER_SENDS_ADR_7_W;
        }
        else
        {
            g_riic2_state = _02_IIC_MASTER_SENDS_ADR_10A_W;
        }

        /* Issue a start condition */
        R_RIIC2_StartCondition();

        /* Set flag for generating stop condition when transmission finishes */
        g_riic2_stop_generation = 0;
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_RIIC2_Master_Receive
* Description  : This function reads data from a slave device.
* Arguments    : adr -
*                    address of slave device
*                rx_buf -
*                    receive buffer pointer
*                rx_num -
*                    receive data length
* Return Value : status -
*                    MD_OK or MD_ERROR1 or MD_ERROR2 or MD_ERROR3
***********************************************************************************************************************/
MD_STATUS R_RIIC2_Master_Receive(uint16_t adr, uint8_t * const rx_buf, uint16_t rx_num)
{
    MD_STATUS status = MD_OK;

    if (0x07FFU < adr)
    {
        status = MD_ERROR2;
    }
    else if (0xFFU < adr)
    {
        /* Master receive for 10bit address is not supported */
        status = MD_ERROR3;
    }
    else 
    {
        /* Set parameter */
        g_riic2_rx_length = rx_num;
        g_riic2_rx_count = 0U;
        gp_riic2_rx_address = rx_buf;
        g_riic2_slave_address = adr;
        g_riic2_dummy_read_count = 0U;
        g_riic2_mode_flag = _0C_IIC_MASTER_RECEIVE;
        g_riic2_state = _00_IIC_MASTER_SENDS_ADR_7_R;

        if (1U == RIIC2.ICCR2.BIT.BBSY)
        {
            /* Has a stop been issued or detected? */
            if ((1U == RIIC2.ICCR2.BIT.SP) || (1U == RIIC2.ICSR2.BIT.STOP))
            {
                /* Wait for the bus to become idle */
                do
                {
                    /* Arbitration lost or timeout? */
                    if ((1U == RIIC2.ICSR2.BIT.TMOF) || (1U == RIIC2.ICSR2.BIT.AL))
                    {
                        return (MD_ERROR4);
                    }
                } while (1U == RIIC2.ICCR2.BIT.BBSY);

                /* Issue a start condition */
                R_RIIC2_StartCondition();;
            }
            /* Bus is busy and it is master mode (MST = 1) */
            else if (1U == RIIC2.ICCR2.BIT.MST)
            {
                /* Issue a restart condition */
                RIIC2.ICSR2.BIT.START = 0U;
                RIIC2.ICIER.BIT.STIE = 1U;
                RIIC2.ICCR2.BIT.RS = 1U;
            }
            else
            {
                /* Another master must have the bus */
                status = MD_ERROR5;
            }
        }
        else
        {
            /* Issue a start condition */
            R_RIIC2_StartCondition();;
        }
    }
    
    return (status);
}
/***********************************************************************************************************************
* Function Name: R_RIIC2_StartCondition
* Description  : This function generates I2C start condition.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_RIIC2_StartCondition(void)
{
    RIIC2.ICCR2.BIT.ST = 1U;    /* Set start condition flag */
}
/***********************************************************************************************************************
* Function Name: R_RIIC2_StopCondition
* Description  : This function generates I2C stop condition.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_RIIC2_StopCondition(void)
{
    RIIC2.ICCR2.BIT.SP = 1U;    /* Set stop condition flag */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
