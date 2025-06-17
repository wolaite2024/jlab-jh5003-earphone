/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_i2c.c
* @brief    This file provides all the I2C firmware functions.
* @details
* @author   elliot chen
* @date     2023-02-10
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "rtl876x_i2c.h"
#include "rtl876x_rcc.h"

/**
  * @brief  I2C DLPS restore register.
  */
typedef struct
{
    uint32_t i2c_reg[21];
} I2CStoreReg_Typedef;

#define USE_TIMEOUT_MECHANISM       1

#define I2C_5M_SOURCE_CLOCK_NS                                      (200) // specify I2C 5MHz source clock, the unit is ns
#define I2C_10M_SOURCE_CLOCK_NS                                     (100) // specify I2C 10MHz source clock, the unit is ns
#define I2C_20M_SOURCE_CLOCK_NS                                     (50)  // specify I2C 20MHz source clock, the unit is ns
#define I2C_40M_SOURCE_CLOCK_NS                                     (25)  // specify I2C 40MHz source clock, the unit is ns
#define I2C_SCL_HIGH_PERIOD_COMPENSATE                              (7)
#define I2C_SCL_LOW_PERIOD_COMPENSATE                               (1)
#define NS_PER_SECOND                                               (1000000000)
#define I2C_IC_FS_SPKLEN                                            (I2Cx->IC_FS_SPKLEN & 0xff)
#define I2C_FAST_MODE_PLUS_SCL_LOW_PERIOD_MIN_NS                    (500)
#define I2C_FAST_MODE_PLUS_SCL_HIGH_PERIOD_MIN_NS                   (260)
#define I2C_FAST_MODE_SCL_LOW_PERIOD_MIN_NS                         (1300)
#define I2C_FAST_MODE_SCL_HIGH_PERIOD_MIN_NS                        (600)
#define I2C_STANDARD_MODE_SCL_LOW_PERIOD_MIN_NS                     (4700)
#define I2C_STANDARD_MODE_SCL_HIGH_PERIOD_MIN_NS                    (4000)

uint32_t I2C_TimeOut = 0xfff;
static uint32_t I2C_RisingTimeNs;

/**
  * @brief  Deinitializes the I2Cx peripheral registers to their default reset values.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @retval None
  */
void I2C_DeInit(I2C_TypeDef *I2Cx)
{
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));

    /*Disable I2C IP*/
    if (I2Cx == I2C0)
    {
        RCC_PeriphClockCmd(APBPeriph_I2C0, APBPeriph_I2C0_CLOCK, DISABLE);
    }
    else if (I2Cx == I2C1)
    {
        RCC_PeriphClockCmd(APBPeriph_I2C1, APBPeriph_I2C1_CLOCK, DISABLE);
    }
    else if (I2Cx == I2C2)
    {
        RCC_PeriphClockCmd(APBPeriph_I2C2, APBPeriph_I2C2_CLOCK, DISABLE);
    }
}

/**
  * @brief  Initializes the I2Cx peripheral according to the specified
  *   parameters in the I2C_InitStruct.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  I2C_InitStruct: pointer to a I2C_InitTypeDef structure that
  *   contains the configuration information for the specified I2C peripheral.
  * @retval None
  */
void I2C_Init(I2C_TypeDef *I2Cx, I2C_InitTypeDef *I2C_InitStruct)
{
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
    assert_param(IS_I2C_CLOCK_SPEED(I2C_InitStruct->I2C_ClockSpeed));

    /* Disable I2C device before change configuration */
    I2Cx->IC_ENABLE &= ~0x0001;

    /* ------------------------------ Initialize I2C device ------------------------------*/
    if (I2C_DeviveMode_Master == I2C_InitStruct->I2C_DeviveMode)
    {
        /*configure I2C device mode which can be selected for master or slave*/
        I2Cx->IC_CON = I2C_InitStruct->I2C_DeviveMode | (I2C_InitStruct->I2C_AddressMode << 4) | BIT(5);

        /*set target address*/
        I2Cx->IC_TAR = (I2C_InitStruct->I2C_SlaveAddress & 0x3ff)
                       | (I2C_InitStruct->I2C_AddressMode << 12);
        /*set SDA hold time in master mode*/
        I2Cx->IC_SDA_HOLD = 0x01;

        I2C_RisingTimeNs = I2C_InitStruct->I2C_RisingTimeNs;
        /*------------------------------ configure I2C speed ------------------------------*/
        I2C_SetClockSpeed(I2Cx, I2C_InitStruct->I2C_ClockSpeed);
    }
    else
    {
        /* set to slave mode */
        I2Cx->IC_CON = (I2C_InitStruct->I2C_DeviveMode) | (I2C_InitStruct->I2C_AddressMode << 3);
        /* set Ack in slave mode */
        I2Cx->IC_ACK_GENERAL_CALL &= I2C_InitStruct->I2C_Ack;
        /* set slave address */
        I2Cx->IC_SAR = I2C_InitStruct->I2C_SlaveAddress;
        /* set SDA hold time in slave mode */
        I2Cx->IC_SDA_HOLD = 0x08;
        /* set SDA setup time delay only in slave transmitter mode(greater than 2) ,delay time:[(IC_SDA_SETUP - 1) * (ic_clk_period)]*/
        I2Cx->IC_SDA_SETUP = 0x02;
    }

#if 1
    /*set Tx empty level*/
    I2Cx->IC_TX_TL = I2C_InitStruct->I2C_TxThresholdLevel;
    /*set Rx full level*/
    I2Cx->IC_RX_TL = I2C_InitStruct->I2C_RxThresholdLevel;
#endif

    /*Config I2C dma mode*/
    I2Cx->IC_DMA_CR = ((I2C_InitStruct->I2C_RxDmaEn)\
                       | ((I2C_InitStruct->I2C_TxDmaEn) << 1));

    /*Config I2C waterlevel*/
    I2Cx->IC_DMA_RDLR = I2C_InitStruct->I2C_RxWaterlevel;
    I2Cx->IC_DMA_TDLR = I2C_InitStruct->I2C_TxWaterlevel;

    I2Cx->IC_INTR_MASK = 0;
}

/**
  * @brief  Fills each I2C_InitStruct member with its default value.
  * @param  I2C_InitStruct : pointer to a I2C_InitTypeDef structure which will be initialized.
  * @retval None
  */
void I2C_StructInit(I2C_InitTypeDef *I2C_InitStruct)
{
    I2C_InitStruct->I2C_Clock             = 40000000;               /* depend on clock divider */
    I2C_InitStruct->I2C_ClockSpeed        = 400000;
    I2C_InitStruct->I2C_DeviveMode        = I2C_DeviveMode_Master;  /* Master mode */
    I2C_InitStruct->I2C_AddressMode       = I2C_AddressMode_7BIT;   /* 7-bit address mode */
    I2C_InitStruct->I2C_SlaveAddress      = 0;
    I2C_InitStruct->I2C_Ack               = I2C_Ack_Enable;
    I2C_InitStruct->I2C_TxThresholdLevel  = 0x00;                 /* tx fifo depth: 24 * 8bits */
    I2C_InitStruct->I2C_RxThresholdLevel  = 0x00;                 /* rx fifo depth: 16 * 8bits */
    I2C_InitStruct->I2C_TxDmaEn           = DISABLE;
    I2C_InitStruct->I2C_RxDmaEn           = DISABLE;
    I2C_InitStruct->I2C_RxWaterlevel      = 1;                    /* Best to equal GDMA Source MSize */
    I2C_InitStruct->I2C_TxWaterlevel      = 15;                   /* Best to equal Tx fifo minus
                                                                      GDMA Source MSize */
    /* Specify I2C scl rising time with 2.2K ohm pull up resistor, the unit is ns*/
    I2C_InitStruct->I2C_RisingTimeNs     = 100;
}


/**
  * @brief  Enables or disables the specified I2C peripheral.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  NewState: new state of the I2Cx peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void I2C_Cmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the selected I2C peripheral */
        I2Cx->IC_ENABLE |= 0x0001;
    }
    else
    {
        /* Disable the selected I2C peripheral */
        I2Cx->IC_ENABLE &= ~0x0001;
    }
}


/**
  * @brief  Checks whether the last I2Cx abort status.
  * @param  I2Cx: where x can be 1 or 2 to select the I2C peripheral.
  * @retval I2C_Status: the status of I2Cx.
  */
I2C_Status I2C_CheckAbortStatus(I2C_TypeDef *I2Cx)
{
    uint32_t abort_status = 0;

    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));

    /* Get abort status */
    abort_status = I2Cx->IC_TX_ABRT_SOURCE;

    if (abort_status & MS_ALL_ABORT)
    {
        /* Clear abort status */
        I2Cx->IC_CLR_TX_ABRT;

        /* Check abort type */
        if (abort_status & ABRT_TXDATA_NOACK)
        {
            return I2C_ABRT_TXDATA_NOACK;
        }

        if (abort_status & ABRT_7B_ADDR_NOACK)
        {
            return I2C_ABRT_7B_ADDR_NOACK;
        }

        if (abort_status & ARB_LOST)
        {
            return I2C_ARB_LOST;
        }

        if (abort_status & ABRT_MASTER_DIS)
        {
            return I2C_ABRT_MASTER_DIS;
        }

        if (abort_status & ABRT_10ADDR1_NOACK)
        {
            return I2C_ABRT_10ADDR1_NOACK;
        }

        if (abort_status & ABRT_10ADDR2_NOACK)
        {
            return I2C_ABRT_10ADDR2_NOACK;
        }
    }

    return I2C_Success;
}

/**
  * @brief  Send data in master mode through the I2Cx peripheral.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  Data: Byte to be transmitted..
  * @retval None
  */
I2C_Status I2C_MasterWrite(I2C_TypeDef *I2Cx, uint8_t *pBuf, uint16_t len)
{
    uint16_t cnt = 0;
    uint32_t time_out = I2C_TimeOut;
    I2C_Status abort_status = I2C_Success;

    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));

    /* Write in the DR register the data to be sent */
    for (cnt = 0; cnt < len; cnt++)
    {
        if (cnt >= len - 1)
        {
            /*generate stop signal*/
            I2Cx->IC_DATA_CMD = (*pBuf++) | (1 << 9);
        }
        else
        {
            I2Cx->IC_DATA_CMD = *pBuf++;
        }

        /* wait for flag of I2C_FLAG_TFNF */
        time_out = I2C_TimeOut;
        while (((I2Cx->IC_STATUS & (1 << 1)) == 0) && (time_out != 0))
        {
            /* Check abort status */
            abort_status = I2C_CheckAbortStatus(I2Cx);
            if (abort_status != I2C_Success)
            {
                return abort_status;
            }

            time_out--;
        }
    }
    while (((I2C_GetFlagState(I2Cx, I2C_FLAG_ACTIVITY) == SET) ||   \
            (I2C_GetFlagState(I2Cx, I2C_FLAG_TFE) != SET)) && (time_out != 0))
    {
        abort_status = I2C_CheckAbortStatus(I2Cx);
        if (abort_status != I2C_Success)
        {
            return abort_status;
        }
        time_out--;
    }
    if (time_out == 0)
    {
        abort_status = I2C_TIMEOUT;
    }
    return abort_status;
}

/**
  * @brief  Send device write data in master mode through the I2Cx peripheral.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  Data: Byte to be transmitted..
  * @retval None
  */
I2C_Status I2C_MasterWriteDevice(I2C_TypeDef *I2Cx, uint8_t *pBuf, uint16_t len, uint8_t *pbuf2,
                                 uint32_t len2)
{
    uint16_t cnt = 0;
    uint32_t time_out = I2C_TimeOut;
    I2C_Status abort_status = I2C_Success;

    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));

    /* Write in the DR register the data to be sent */
    for (cnt = 0; cnt < len; cnt++)
    {
        I2Cx->IC_DATA_CMD = *pBuf++;

        /* wait for flag of I2C_FLAG_TFNF */
        time_out = I2C_TimeOut;
        while (((I2Cx->IC_STATUS & (1 << 1)) == 0) && (time_out != 0))
        {
            /* Check abort status */
            abort_status = I2C_CheckAbortStatus(I2Cx);
            if (abort_status != I2C_Success)
            {
                return abort_status;
            }

            time_out--;
        }
    }

    for (cnt = 0; cnt < len2; cnt++)
    {
        if (cnt >= len2 - 1)
        {
            /*generate stop signal*/
            I2Cx->IC_DATA_CMD = (*pbuf2++) | (1 << 9);
        }
        else
        {
            I2Cx->IC_DATA_CMD = *pbuf2++;
        }

        /* wait for flag of I2C_FLAG_TFNF */
        time_out = I2C_TimeOut;
        while (((I2Cx->IC_STATUS & (1 << 1)) == 0) && (time_out != 0))
        {
            /* Check abort status */
            abort_status = I2C_CheckAbortStatus(I2Cx);
            if (abort_status != I2C_Success)
            {
                return abort_status;
            }

            time_out--;
        }
    }

    while (((I2C_GetFlagState(I2Cx, I2C_FLAG_ACTIVITY) == SET) ||   \
            (I2C_GetFlagState(I2Cx, I2C_FLAG_TFE) != SET)) && (time_out != 0))
    {
        abort_status = I2C_CheckAbortStatus(I2Cx);
        if (abort_status != I2C_Success)
        {
            return abort_status;
        }
        time_out--;
    }

    if (time_out == 0)
    {
        abort_status = I2C_TIMEOUT;
    }

    return abort_status;
}

/**
  * @brief  Read data in master mode through the I2Cx peripheral.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  Data: Byte to be transmitted..
  * @retval None
  */
I2C_Status I2C_MasterRead(I2C_TypeDef *I2Cx, uint8_t *pBuf, uint16_t len)
{
    uint16_t cnt = 0;
    uint32_t reg_value = 0;
    uint32_t time_out = I2C_TimeOut;
    I2C_Status abort_status = I2C_Success;

    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));

    /* read in the DR register the data to be sent */
    for (cnt = 0; cnt < len; cnt++)
    {
        if (cnt >= len - 1)
        {
            /* generate stop singal */
            I2Cx->IC_DATA_CMD = (reg_value) | (0x0003 << 8);
        }
        else
        {
            I2Cx->IC_DATA_CMD = (reg_value) | (0x0001 << 8);
        }

        /* read data */
        if (cnt > 0)
        {
            /* wait for I2C_FLAG_RFNE flag */
            time_out = I2C_TimeOut;
            while (((I2Cx->IC_STATUS & (1 << 3)) == 0) && (time_out != 0))
            {
                /* Check abort status */
                abort_status = I2C_CheckAbortStatus(I2Cx);
                if (abort_status != I2C_Success)
                {
                    return abort_status;
                }

                time_out--;
            }

            if (time_out > 0)
            {
                *pBuf++ = (uint8_t)I2Cx->IC_DATA_CMD;
            }
        }
    }

    /* wait for I2C_FLAG_RFNE flag */
    time_out = I2C_TimeOut;
    while (((I2Cx->IC_STATUS & (1 << 3)) == 0) && (time_out != 0))
    {
        /* Check abort status */
        abort_status = I2C_CheckAbortStatus(I2Cx);
        if (abort_status != I2C_Success)
        {
            return abort_status;
        }

        time_out--;
    }

    if (time_out > 0)
    {
        *pBuf = (uint8_t)I2Cx->IC_DATA_CMD;
    }
    else
    {
        abort_status = I2C_TIMEOUT;
    }

    return abort_status;
}

/**
  * @brief  Sends data and read data in master mode through the I2Cx peripheral.Attention:Read data with time out mechanism.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  Data: Byte to be transmitted..
  * @retval Actual length of read data
  */
I2C_Status I2C_RepeatRead(I2C_TypeDef *I2Cx, uint8_t *pWriteBuf, uint16_t Writelen,
                          uint8_t *pReadBuf, uint16_t Readlen)
{
    uint16_t cnt = 0;
    uint16_t success_read_cnt = 0;
    uint32_t reg_value = 0;
    uint32_t time_out = I2C_TimeOut;
    I2C_Status abort_status = I2C_Success;

    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));


    /*------------------------------ write data section ------------------------------*/
    /* write data in the IC_DATA_CMD register */
    for (cnt = 0; cnt < Writelen; cnt++)
    {
        I2Cx->IC_DATA_CMD = *pWriteBuf++;

        /*wait for I2C_FLAG_TFNF flag that Tx FIFO is not full*/
        time_out = I2C_TimeOut;
        while (((I2Cx->IC_STATUS & BIT(1)) == 0) && (time_out != 0))
        {
            /* Check abort status */
            abort_status = I2C_CheckAbortStatus(I2Cx);
            if (abort_status != I2C_Success)
            {
                return abort_status;
            }

            time_out--;
        }
    }

    /*wait for I2C_FLAG_TFNF flag that Tx FIFO is not full*/
    time_out = I2C_TimeOut;
    while (((I2Cx->IC_STATUS & BIT(1)) == 0) && (time_out != 0))
    {
        time_out--;
    }

    /*------------------------------ read data section ------------------------------*/
    for (cnt = 0; cnt < Readlen; cnt++)
    {
        if (cnt >= Readlen - 1)
        {
            /*generate stop singal in last byte which to be sent*/
            I2Cx->IC_DATA_CMD = reg_value | BIT(8) | BIT(9);
        }
        else
        {
            I2Cx->IC_DATA_CMD = reg_value | BIT(8);
        }

        /*wait for I2C_FLAG_RFNE flag*/
        time_out = I2C_TimeOut;
        while (((I2Cx->IC_STATUS & BIT(3)) == 0) && (time_out != 0))
        {
            /* Check abort status */
            abort_status = I2C_CheckAbortStatus(I2Cx);
            if (abort_status != I2C_Success)
            {
                return abort_status;
            }

            time_out--;
        }
        if (time_out > 0)
        {
            *pReadBuf++ = (uint8_t)I2Cx->IC_DATA_CMD;
            success_read_cnt++;
        }
    }

    if (success_read_cnt != Readlen)
    {
        abort_status = I2C_TIMEOUT;
    }

    return abort_status;
}

/**
  * @brief mask the specified I2C interrupt.
  * @param  I2Cx: where x can be 0 or 1
  * @param  I2C_INT
  * This parameter can be one of the following values:
  *     @arg I2C_INT_GEN_CALL: Set only when a General Call address is received and it is acknowledged.
  *     @arg I2C_INT_START_DET: Indicates whether a START or RESTART condition has occurred on the I2C
                              interface regardless of whether I2C is operating in slave or master mode.
  *     @arg I2C_INT_STOP_DET:  Indicates whether a STOP condition has occurred on the I2C interface regardless
                              of whether I2C is operating in slave or master mode
  *     @arg I2C_INT_ACTIVITY:  This bit captures I2C activity and stays set until it is cleared.
  *     @arg I2C_INT_RX_DONE:   When the I2C is acting as a slave-transmitter, this bit is set to 1 if the
                              master does not acknowledge a transmitted byte. This occurs on the last byte of
                              the transmission, indicating that the transmission is done.
  *     @arg I2C_INT_TX_ABRT:   This bit indicates if I2C as an I2C transmitter, is unable to complete the
                              intended actions on the contents of the transmit FIFO.
  *     @arg I2C_INT_RD_REQ:    This bit is set to 1 when acting as a slave and another I2C master
                              is attempting to read data.
  *     @arg I2C_INT_TX_EMPTY:  This bit is set to 1 when the transmit buffer is at or below the threshold value set
                              in the IC_TX_TL register.
  *     @arg I2C_INT_TX_OVER:   Set during transmit if the transmit buffer is filled to IC_TX_BUFFER_DEPTH and
                              the processor attempts to issue another I2C command.
  *     @arg I2C_INT_RX_FULL:   Set when the receive buffer reaches or goes above the RX_TL threshold in the
                              IC_RX_TL register
  *     @arg I2C_INT_RX_OVER:   Set if the receive buffer is completely filled to IC_RX_BUFFER_DEPTH and an
                               additional byte is received from an external I2C device.
  *     @arg I2C_INT_RX_UNDER:   Set if the processor attempts to read the receive buffer when it is empty by reading.
  * @retval None.
  */
void I2C_INTConfig(I2C_TypeDef *I2Cx, uint16_t I2C_INT, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
    assert_param(I2C_GET_INT(I2C_INT));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the selected I2C interrupts */
        I2Cx->IC_INTR_MASK |= I2C_INT;
    }
    else
    {
        /* Disable the selected I2C interrupts */
        I2Cx->IC_INTR_MASK &= (uint16_t)~I2C_INT;
    }
}

/**
  * @brief clear the specified I2C interrupt.
  * @param  I2Cx: where x can be 0 or 1
  * @retval None.
  */
void I2C_ClearINTPendingBit(I2C_TypeDef *I2Cx, uint16_t I2C_IT)
{
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
    assert_param(I2C_GET_INT(I2C_IT));

    switch (I2C_IT)
    {
    case I2C_INT_RX_UNDER:
        I2Cx->IC_CLR_RX_UNDER;
        break;
    case I2C_INT_RX_OVER:
        I2Cx->IC_CLR_RX_OVER;
        break;
    case I2C_INT_TX_OVER:
        I2Cx->IC_CLR_TX_OVER;
        break;
    case I2C_INT_RD_REQ:
        I2Cx->IC_CLR_RD_REQ;
        break;
    case I2C_INT_TX_ABRT:
        I2Cx->IC_CLR_TX_ABRT;
        break;
    case I2C_INT_RX_DONE:
        I2Cx->IC_CLR_RX_DONE;
        break;
    case I2C_INT_ACTIVITY:
        I2Cx->IC_CLR_ACTIVITY;
        break;
    case I2C_INT_STOP_DET:
        I2Cx->IC_CLR_STOP_DET;
        break;
    case I2C_INT_START_DET:
        I2Cx->IC_CLR_START_DET;
        break;
    case I2C_INT_GEN_CALL:
        I2Cx->IC_CLR_GEN_CALL;
        break;
    default:
        break;
    }
}

void I2C_SetClockSpeed(I2C_TypeDef *I2Cx, uint32_t I2C_ClockSpeed)
{
    uint32_t i2c_source_clock_ns = 0;
    uint32_t scl_period_ns = 0;
    uint32_t total_hcnt_lcnt = 0;
    uint32_t scl_low_period_min_ns = 0;
    uint32_t scl_high_period_min_ns = 0;
    uint32_t scl_low_period_min_of_i2c_spec_ns = 0;
    uint32_t scl_high_period_min_of_i2c_spec_ns = 0;
    uint32_t scl_hcnt_min = 0;
    uint32_t scl_lcnt_min = 0;
    uint32_t scl_hcnt = 0;
    uint32_t scl_lcnt = 0;
    int32_t remaining_cnt = 0;
    uint32_t total_high_low_period = 0;
    uint8_t  remainder = 0;
    uint8_t i2c_div = 1;
    uint8_t compensate_index = 0;
    uint32_t I2CSrcClk = 0;

    if (I2Cx == I2C0)
    {
        compensate_index = SYSBLKCTRL->u_35C.BITS_35C.BIT_PERI_I2C0_CLK_DIV;
    }
    else if (I2Cx == I2C1)
    {
        compensate_index = SYSBLKCTRL->u_35C.BITS_35C.BIT_PERI_I2C1_CLK_DIV;
    }
    else if (I2Cx == I2C2)
    {
        compensate_index = SYSBLKCTRL->u_35C.BITS_35C.BIT_PERI_I2C2_CLK_DIV;
    }
    else
    {
        return;
    }

    i2c_div = BIT(compensate_index);
    I2CSrcClk = 40000000 / i2c_div;

    if (I2CSrcClk == 5000000)
    {
        i2c_source_clock_ns = I2C_5M_SOURCE_CLOCK_NS;
        I2Cx->IC_FS_SPKLEN = 3;
        if (I2C_ClockSpeed > 100000)
        {
            I2C_ClockSpeed = 100000;
        }
    }
    else if (I2CSrcClk == 10000000)
    {
        i2c_source_clock_ns = I2C_10M_SOURCE_CLOCK_NS;
        if (I2C_ClockSpeed > 100000)
        {
            I2C_ClockSpeed = 100000;
        }
    }
    else if (I2CSrcClk == 20000000)
    {
        i2c_source_clock_ns = I2C_20M_SOURCE_CLOCK_NS;
        I2Cx->IC_FS_SPKLEN = 1;
        if (I2C_ClockSpeed > 400000)
        {
            I2C_ClockSpeed = 400000;
        }
    }
    else
    {
        i2c_source_clock_ns = I2C_40M_SOURCE_CLOCK_NS;
    }

    /* Obtain the minimum value of scl low period and scl high period specified in the spec */
    if (I2C_ClockSpeed <= 100000)
    {
        scl_low_period_min_of_i2c_spec_ns = I2C_STANDARD_MODE_SCL_LOW_PERIOD_MIN_NS;
        scl_high_period_min_of_i2c_spec_ns = I2C_STANDARD_MODE_SCL_HIGH_PERIOD_MIN_NS;
    }
    else if (I2C_ClockSpeed <= 400000)
    {
        scl_low_period_min_of_i2c_spec_ns = I2C_FAST_MODE_SCL_LOW_PERIOD_MIN_NS;
        scl_high_period_min_of_i2c_spec_ns = I2C_FAST_MODE_SCL_HIGH_PERIOD_MIN_NS;
    }
    else
    {
        if (I2C_ClockSpeed > 1000000)
        {
            I2C_ClockSpeed = 1000000;
        }
        I2Cx->IC_FS_SPKLEN = 2;
        scl_low_period_min_of_i2c_spec_ns = I2C_FAST_MODE_PLUS_SCL_LOW_PERIOD_MIN_NS;
        scl_high_period_min_of_i2c_spec_ns = I2C_FAST_MODE_PLUS_SCL_HIGH_PERIOD_MIN_NS;
    }

    /* IC_SS_SCL_LCNT and IC_FS_SCL_LCNT register values must be larger than IC_FS_SPKLEN + 7 */
    scl_lcnt_min = I2C_IC_FS_SPKLEN + 7 + 1;
    /* IC_SS_SCL_HCNT and IC_FS_SCL_HCNT register values must be larger than IC_FS_SPKLEN + 5 */
    scl_hcnt_min = I2C_IC_FS_SPKLEN + 5 + 1;

    scl_low_period_min_ns = (scl_lcnt_min + I2C_SCL_LOW_PERIOD_COMPENSATE) * i2c_source_clock_ns;
    scl_high_period_min_ns = (scl_hcnt_min + I2C_IC_FS_SPKLEN + I2C_SCL_HIGH_PERIOD_COMPENSATE) *
                             i2c_source_clock_ns;

    if (scl_low_period_min_ns < scl_low_period_min_of_i2c_spec_ns)
    {
        scl_low_period_min_ns = scl_low_period_min_of_i2c_spec_ns;
    }

    if (scl_high_period_min_ns < scl_high_period_min_of_i2c_spec_ns)
    {
        scl_high_period_min_ns = scl_high_period_min_of_i2c_spec_ns;
    }
    /* Calculate the period of SCL based on the frequency of I2C SCL */
    scl_period_ns = NS_PER_SECOND / (I2C_ClockSpeed);
    /* Calculate the sum of scl high period and scl low period */
    total_high_low_period = scl_period_ns - I2C_RisingTimeNs;
    remainder = total_high_low_period % i2c_source_clock_ns;
    if (remainder != 0)
    {
        total_high_low_period += i2c_source_clock_ns - remainder;
    }
    /* Calculate the sum of scl_hcnt and scl_lcnt */
    total_hcnt_lcnt = (total_high_low_period / i2c_source_clock_ns) - I2C_IC_FS_SPKLEN
                      - I2C_SCL_HIGH_PERIOD_COMPENSATE - I2C_SCL_LOW_PERIOD_COMPENSATE;
    /* Calculate the minimum value of scl_lcnt */
    scl_lcnt_min = scl_low_period_min_ns / i2c_source_clock_ns - I2C_SCL_LOW_PERIOD_COMPENSATE;
    /* Calculate the minimum value of scl_hcnt */
    scl_hcnt_min = scl_high_period_min_ns / i2c_source_clock_ns - I2C_IC_FS_SPKLEN -
                   I2C_SCL_HIGH_PERIOD_COMPENSATE;
    remaining_cnt = total_hcnt_lcnt - scl_lcnt_min - scl_hcnt_min;
    if (remaining_cnt > 0)
    {
        scl_lcnt = scl_lcnt_min + remaining_cnt / 2;
        scl_hcnt = total_hcnt_lcnt - scl_lcnt;
    }
    else
    {
        scl_lcnt = scl_lcnt_min;
        scl_hcnt = scl_hcnt_min;
    }
    /*Configure I2C speed in standard mode*/
    if (I2C_ClockSpeed <= 100000)
    {
        I2Cx->IC_CON = (I2Cx->IC_CON | (0x3 << 1)) & 0xfffb;
        /*configure I2C speed*/
        I2Cx->IC_SS_SCL_HCNT = scl_hcnt;
        I2Cx->IC_SS_SCL_LCNT = scl_lcnt;
    }
    /* Configure I2C speed in fast mode or fast mode plus*/
    else
    {
        I2Cx->IC_CON = (I2Cx->IC_CON | (0x3 << 1)) & 0xfffd;
        /*configure I2C speed*/
        I2Cx->IC_FS_SCL_HCNT = scl_hcnt;
        I2Cx->IC_FS_SCL_LCNT = scl_lcnt;
    }
}

/**
  * @brief  I2C enter dlps callback function(Save I2C register values when system enter DLPS)
  * @param  None
  * @retval None
  */
void I2C_DLPSEnter(void *peri_reg, void *p_store_buf)
{
    I2CStoreReg_Typedef *store_buf = (I2CStoreReg_Typedef *)p_store_buf;
    I2C_TypeDef *I2Cx = (I2C_TypeDef *)peri_reg;

    store_buf->i2c_reg[0] = I2Cx->IC_CON;
    store_buf->i2c_reg[1] = I2Cx->IC_TAR;
    store_buf->i2c_reg[2] = I2Cx->IC_SAR;
    store_buf->i2c_reg[3] = I2Cx->IC_HS_MADDR;

    store_buf->i2c_reg[4] = I2Cx->IC_SS_SCL_HCNT;
    store_buf->i2c_reg[5] = I2Cx->IC_SS_SCL_LCNT;
    store_buf->i2c_reg[6] = I2Cx->IC_FS_SCL_HCNT;
    store_buf->i2c_reg[7] = I2Cx->IC_FS_SCL_LCNT;
    store_buf->i2c_reg[8] = I2Cx->IC_HS_SCL_HCNT;
    store_buf->i2c_reg[9] = I2Cx->IC_HS_SCL_LCNT;

    store_buf->i2c_reg[10] = I2Cx->IC_INTR_MASK;
    store_buf->i2c_reg[11] = I2Cx->IC_RX_TL;
    store_buf->i2c_reg[12] = I2Cx->IC_TX_TL;
    store_buf->i2c_reg[13] = I2Cx->IC_ENABLE;
    store_buf->i2c_reg[14] = I2Cx->IC_SDA_HOLD;
    store_buf->i2c_reg[15] = I2Cx->IC_SLV_DATA_NACK_ONLY;
    store_buf->i2c_reg[16] = I2Cx->IC_DMA_CR;
    store_buf->i2c_reg[17] = I2Cx->IC_DMA_TDLR;
    store_buf->i2c_reg[18] = I2Cx->IC_DMA_RDLR;

    store_buf->i2c_reg[19] = I2Cx->IC_SDA_SETUP;
    store_buf->i2c_reg[20] = I2Cx->IC_FS_SPKLEN;
}

uint32_t I2C_GetDLPSBufferLen(void)
{
    return sizeof(I2CStoreReg_Typedef);
}

/**
  * @brief  I2C exit dlps callback function(Resume I2C register values when system exit DLPS)
  * @param  None
  * @retval None
  */
void I2C_DLPSExit(void *peri_reg, void *p_store_buf)
{
    I2CStoreReg_Typedef *store_buf = (I2CStoreReg_Typedef *)p_store_buf;
    I2C_TypeDef *I2Cx = (I2C_TypeDef *)peri_reg;

    I2Cx->IC_CON = store_buf->i2c_reg[0];
    I2Cx->IC_TAR = store_buf->i2c_reg[1];
    I2Cx->IC_SAR = store_buf->i2c_reg[2];
    I2Cx->IC_HS_MADDR = store_buf->i2c_reg[3];

    I2Cx->IC_FS_SPKLEN = store_buf->i2c_reg[20];

    I2Cx->IC_SS_SCL_HCNT = store_buf->i2c_reg[4];
    I2Cx->IC_SS_SCL_LCNT = store_buf->i2c_reg[5];
    I2Cx->IC_FS_SCL_HCNT = store_buf->i2c_reg[6];
    I2Cx->IC_FS_SCL_LCNT = store_buf->i2c_reg[7];
    I2Cx->IC_HS_SCL_HCNT = store_buf->i2c_reg[8];
    I2Cx->IC_HS_SCL_LCNT = store_buf->i2c_reg[9];

    I2Cx->IC_INTR_MASK = store_buf->i2c_reg[10];
    I2Cx->IC_RX_TL = store_buf->i2c_reg[11];
    I2Cx->IC_TX_TL = store_buf->i2c_reg[12];
    I2Cx->IC_SDA_HOLD = store_buf->i2c_reg[14];
    I2Cx->IC_SLV_DATA_NACK_ONLY = store_buf->i2c_reg[15];
    I2Cx->IC_DMA_CR = store_buf->i2c_reg[16];
    I2Cx->IC_DMA_TDLR = store_buf->i2c_reg[17];
    I2Cx->IC_DMA_RDLR = store_buf->i2c_reg[18];
    I2Cx->IC_SDA_SETUP = store_buf->i2c_reg[19];

    I2Cx->IC_ENABLE = store_buf->i2c_reg[13];
}

/******************* (C) COPYRIGHT 2023 Realtek Semiconductor Corporation *****END OF FILE****/

