/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl876x_iso7816.c
* \brief    This file provides all the ISO7816 firmware functions.
* \details
* \author   qinyuan_hu
* @date     2024-06-18
* \version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "rtl876x_iso7816.h"
#include "rtl876x_rcc.h"
/*============================================================================*
 *                          Private Macros
 *============================================================================*/
uint32_t ISO7816_Clock = 5000000;//5MHz

#define iso7816_delay_us(us) {delay_us_internal(us);}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
/**
 * \brief Internal delay us.
 */
void delay_us_internal(uint32_t us)
{
    for (uint32_t i = us; i > 0; i--)
    {
        for (uint32_t j = 0; j < 2; j++);
    }
}

/**
 * \brief This function aims to provide delay based on cycle count Which should be count/Freq (s),
 *        given unit of Freq is Hz. Yet the actual delay function takes input parameters with unit us.
 *        We should have called simc_delay_us(count * 10^6 / Freq). However, there would be possible
 *        integer overflow of (count * 10^6). Thus we call iso7816_delay_us((count * 10^3) / (Freq / 10^3)) instead.
 */
void ISO7816_DelayCycle(uint32_t cycle)
{
    if ((cycle * 1000) < (ISO7816_Clock / 1000))
    {
        iso7816_delay_us(1);
    }
    else
    {
        iso7816_delay_us((cycle * 1000) / (ISO7816_Clock / 1000));
    }
}


/**
 * \brief   Deinitialize the ISO7816 peripheral registers to their default reset values(turn off ISO7816 clock).
 * \param   None.
 * \return  None.
 */
void ISO7816_DeInit(void)
{
    RCC_PeriphClockCmd(APBPeriph_ISO7816, APBPeriph_ISO7816_CLOCK, DISABLE);
}

/**
  * \brief  Initializes the ISO7816 peripheral according to the specified
  *         parameters in the ISO7816_InitStruct.
  * \param  ISO7816: selected ISO7816 peripheral.
  * \param  ISO7816_InitStruct: pointer to a ISO7816_InitTypeDef structure that
  *         contains the configuration information for the specified ISO7816 peripheral.
  * \return None.
  */
void ISO7816_Init(ISO7816_InitTypeDef *ISO7816_InitStruct)
{
    assert_param(IS_ISO7816_PERIPH(ISO7816));

    //Enable ISO7816.
    ISO7816->LP_CLKG |= ISO7816_ENABLE;

    //Disable all interrupt.
    ISO7816->IER = 0x00;
    //Read to clear interrupt status reg.
    ISO7816->ISR;

    //Set default Fd Dd.
    uint16_t prescaler = ISO7816_InitStruct->ISO7816_IntegerF / ISO7816_InitStruct->ISO7816_IntegerD;
    ISO7816->GCR = prescaler - 1;

    //Set ISO7816 ctrl, Clear Rx&Tx FIFO.
    ISO7816->CR |= ISO7816_InitStruct->ISO7816_IOMode | ISO7816_RX_FIFO_CLEAR_MSK |
                   ISO7816_TX_FIFO_CLEAR_MSK;
    ISO7816->CR &= ISO7816_RX_FIFO_CLEAR_CLR;
    ISO7816->CR &= ISO7816_TX_FIFO_CLEAR_CLR;

    ISO7816->CR &= ISO7816_GP_COUNTER_CLR; //General purpose counter limitation

    ISO7816->CR &= ISO7816_InitStruct->ISO7816_ProtocolType | ISO7816_InitStruct->ISO7816_ClockStopPolar
                   | ISO7816_InitStruct->ISO7816_CodingConvention;

    ISO7816->RCR = ISO7816_InitStruct->ISO7816_IOStateSample | (ISO7816_InitStruct->ISO7816_WaitingTime
                                                                & 0xFFFE);

    ISO7816->THR = (ISO7816_InitStruct->ISO7816_RxAFWaterLevel << 8) \
                   | (ISO7816_InitStruct->ISO7816_RxRetryCountLevel << 4) \
                   | (ISO7816_InitStruct->ISO7816_TxRetryCountLevel);

    //Reset pin init state is L.
    ISO7816->CR &= ISO7816_RST_CONTACT_Reset;

    ISO7816->TCR = ISO7816_InitStruct->ISO7816_ExtraGuardTime & 0xFF;

    delay_us_internal(5 * 1000);
}

/**
  * \brief  Fills each ISO7816_InitStruct member with its default value.
  * \param  ISO7816_InitStruct: pointer to an ISO7816_InitTypeDef structure which will be initialized.
  * \return None.
  */
void ISO7816_StructInit(ISO7816_InitTypeDef *ISO7816_InitStruct)
{
    ISO7816_InitStruct->ISO7816_IntegerF              = 372;
    ISO7816_InitStruct->ISO7816_IntegerD              = 1;
    ISO7816_InitStruct->ISO7816_NACKCmd               = 0;
    ISO7816_InitStruct->ISO7816_ProtocolType          = ISO7816_PROTOCOL_TYPE_CLR;
    ISO7816_InitStruct->ISO7816_ClockStopPolar        = ISO7816_CLOCK_STOP_POLAR_LOW;
    ISO7816_InitStruct->ISO7816_IOMode                = ISO7816_IO_MODE_RX; //default rx
    ISO7816_InitStruct->ISO7816_CodingConvention      = ISO7816_CODING_CONVENTION_DIRECT;
    ISO7816_InitStruct->ISO7816_ExtraGuardTime        = 0x0;
    ISO7816_InitStruct->ISO7816_WaitingTime           = 0x2580; //default 9600etu
    ISO7816_InitStruct->ISO7816_IOStateSample         = ISO7816_IO_STATE_SAMPLE_ENABLE;
    ISO7816_InitStruct->ISO7816_RxAFWaterLevel        = 0x0E;
    ISO7816_InitStruct->ISO7816_RxRetryCountLevel     = 0x0F;
    ISO7816_InitStruct->ISO7816_TxRetryCountLevel     = 0x03;
    return;
}

/**
 * \brief   Enables or disables the specified ISO7816 interrupts.
 * \param   ISO7816_INT: specifies the interrupt source to check.
 *          This parameter can be one of the following values:
 *     \arg ISO7816_INT_RESET_TIMING_VIOLATION Reset timing violation.
 *     \arg ISO7816_INT_TX_NAK_THD             TNAKTH exceeded.
 *     \arg ISO7816_INT_RX_WAIT_TIMEOUT        Character wait timeout.
 *     \arg ISO7816_INT_GP_COUNTER             General purpose counter hit.
 *     \arg ISO7816_INT_TX_FIFO_EMPTY          TXFIFO empty.
 *     \arg ISO7816_INT_TX_FIFO_NOT_FULL       TXFIFO not full, triggered when TX FIFO changes from full to non-full.
 *     \arg ISO7816_INT_TX_FIFO_OVERFLOW       TXFIFO overflow.
 *     \arg ISO7816_INT_RX_FIFO_NOT_EMPTY      RXFIFO not empty, triggered when RX FIFO changes from empty to non-empty.
 *     \arg ISO7816_INT_RX_FIFO_ALMOST_FULL    RXFIFO almost full
 *     \arg ISO7816_INT_RX_FIFO_FULL           RXFIFO full.
 *     \arg ISO7816_INT_RX_FIFO_OVERFLOW       RXFIFO overflow.
 *     \arg ISO7816_INT_RX_FIFO_UNDERFLOW      RXFIFO underflow.
 *     \arg ISO7816_INT_TX_DONE                Transmission completed, triggered when TX FIFO is empty and the last bit has been sent out.
 * \param   NewState: new state of the specified ISO7816 interrupts.
 *          This parameter can be: ENABLE or DISABLE.
 * \return None
 */
void ISO7816_INTConfig(uint32_t ISO7816_INT, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_ISO7816_INT(ISO7816_INT));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        ISO7816->IER |= ISO7816_INT & 0x1FFF;
    }
    else
    {
        ISO7816->IER &= ~(ISO7816_INT & 0x1FFF);
    }
}

/**
  * \brief  Send data to tx FIFO.
  * \param  ISO7816: selected ISO7816 peripheral.
  * \param  inBuf: buffer to be written to Tx FIFO.
  * \param  num: number of data to be written.
  * \return None.
  */
void ISO7816_SendData(const uint8_t *inBuf, uint16_t len)
{
    /* Check the parameters */
    assert_param(IS_ISO7816_PERIPH(ISO7816));

    //Transmitter enabled
    ISO7816->CR |= ISO7816_IO_MODE_MSK;

    while (len--)
    {
        ISO7816->TX_FIFO = *inBuf++;
    }
}

/**
  * \brief  Receive data from rx FIFO.
  * \param  ISO7816: selected ISO7816 peripheral.
  * \param[out]  outBuf: buffer to save data read from ISO7816 FIFO.
  * \param  count: number of data to be read.
  * \return None
  */
void ISO7816_ReceiveData(uint8_t *outBuf, uint16_t len)
{
    /* Check the parameters */
    assert_param(IS_ISO7816_PERIPH(ISO7816));

    while (len--)
    {
        *outBuf++ = (uint8_t)ISO7816->RX_FIFO;
    }
}

/**
 * \brief   Active a physically connected card.
 * \param   None.
 * \return  None.
 */
void ISO7816_CardActivtion(void)
{
    uint32_t value = 0;

    value = ISO7816_VCC_CONTACT_Set | ISO7816_CLK_CONTACT_Set | ISO7816_CR_ENABLE;

    ISO7816->CR &= ISO7816_RST_CONTACT_Reset;//RST = 0.
    ISO7816->CR |= value;

}

/**
 * \brief   Cold reset the smartcard.
 * \param   None.
 * \return  None.
 */
void ISO7816_CardColdReset(void)
{
    uint32_t value = 0;

    value = ISO7816_VCC_CONTACT_Set | ISO7816_CLK_CONTACT_Set | ISO7816_CONTROLLER_ENBALE_MSK;

    ISO7816->CR &= ISO7816_RST_CONTACT_Reset;//RST = 0.
    ISO7816->CR |= value;
    //to sample I/O line
    ISO7816->RCR |= BIT(16);
}

/**
 * \brief   Warm reset the smartcard.
 * \param   None.
 * \return  None.
 */
void ISO7816_CardWarmReset(void)
{
    ISO7816->CR &= ISO7816_RST_CONTACT_Reset;//RST = 0.
    ISO7816->CR |= ISO7816_VCC_CONTACT_Set;
    ISO7816->CR |= ISO7816_CLK_CONTACT_Set;

    // Waits at least 400/f before RST goes high.
    // RST should not change during ATR dialog
    ISO7816_DelayCycle(400);
    ISO7816->CR |= ISO7816_RST_CONTACT_Set;//RST = 1.

    //wait ATR
    ISO7816_DelayCycle(40000);
}

/**
 * \brief   Stop the clock output of the smartcard.
 * \param   None.
 * \return  None.
 */
void ISO7816_CardClockStop(void)
{
    //must wait until tx&rx fifo empty
    while ((ISO7816_GetFlagStatus(ISO7816_FLAG_TX_IDLE) == RESET)
           && (ISO7816_GetFlagStatus(ISO7816_FLAG_TX_FIFO_EMPTY) == RESET)
           && (ISO7816_GetFlagStatus(ISO7816_FLAG_RX_FIFO_NOT_EMPTY) != RESET));

    /* delay >=1860*clk */
    ISO7816_DelayCycle(1860);

    ISO7816->CR &= ISO7816_CLK_CONTACT_Reset;
}

/**
 * \brief   Restart the clock output of the smartcard.
 * \param   None.
 * \return  None.
 */
void ISO7816_CardClockRestart(void)
{
    ISO7816->CR |= ISO7816_CLK_CONTACT_Set;

    /* delay >=700*clk */
    ISO7816_DelayCycle(700);
}

/**
 * \brief   Deactive a physically connected smartcard.
 * \param   None.
 * \return  None.
 */
void ISO7816_CardDeactive(void)
{
    uint32_t value = ISO7816_RESET_ENABLE_MSK | ISO7816_VCC_ENABLE_MSK | ISO7816_CLOCK_ENABLE_MSK |
                     ISO7816_CONTROLLER_ENBALE_MSK;
    ISO7816->CR &= ~value;
}

/**
 * \brief  Check whether the specified ISO7816 flag is set or not.
           Software reads to clear all interrupts.
 * \param  ISO7816_FLAG: specifies the flag to check.
 *         This parameter can be one of the following values:
 *         \arg ISO7816_FLAG_TX_FIFO_EMPTY           ((uint16_t)(1 << 0))
 *         \arg ISO7816_FLAG_TX_FIFO_NOT_FULL        ((uint16_t)(1 << 1))
 *         \arg ISO7816_FLAG_TX_IDLE                 ((uint16_t)(1 << 2))
 *         \arg ISO7816_FLAG_RX_FIFO_NOT_EMPTY       ((uint16_t)(1 << 4))
 *         \arg ISO7816_FLAG_RX_FIFO_ALMOST_FULL     ((uint16_t)(1 << 5))
 *         \arg ISO7816_FLAG_RX_FIFO_FULL            ((uint16_t)(1 << 6))
 *         \arg ISO7816_FLAG_RX_AD_TIMING_VIOLATION  ((uint16_t)(1 << 8))
 *         \arg ISO7816_FLAG_RX_CF_TIMING_VIOLATION  ((uint16_t)(1 << 9))
 *         \arg ISO7816_FLAG_TX_NAK_CNT_THD          ((uint16_t)(1 << 10))
 *         \arg ISO7816_FLAG_RX_NAK_CNT_THD          ((uint16_t)(1 << 11))
 *         \arg ISO7816_FLAG_TX_FIFO_OVERFLOW        ((uint16_t)(1 << 12))
 *         \arg ISO7816_FLAG_RX_FIFO_OVERFLOW        ((uint16_t)(1 << 13))
 *         \arg ISO7816_FLAG_RX_FIFO_UNDERFLOW       ((uint16_t)(1 << 14))
 *         \arg ISO7816_FLAG_RX_PARITY_ERR           ((uint16_t)(1 << 15))
 * \return The new state of ISO7816_FLAG (SET or RESET).
 */
FlagStatus ISO7816_GetFlagStatus(uint32_t ISO7816_FLAG)
{
    /* Check the parameters */
    assert_param(IS_ISO7816_FLAG(ISO7816_FLAG));

    FlagStatus flagstatus = RESET;
    uint32_t flag_status_bits = 0;

    if (ISO7816_FLAG & 0x07)
    {
        flag_status_bits = ISO7816->TSR & 0x07;
    }
    else if (ISO7816_FLAG & 0x70)
    {
        flag_status_bits = ISO7816->RSR & 0x70;
    }
    else if (ISO7816_FLAG & 0xFF00)
    {
        uint32_t temp = ISO7816->ESR & 0xFF;
        flag_status_bits = (temp << 8) & 0xFF00;
    }

    if (flag_status_bits & ISO7816_FLAG)
    {
        flagstatus = SET;
    }

    return flagstatus;
}

/**
 * \brief  Check whether the specified ISO7816 interrupt source is set or not.
           Software reads to clear all interrupts.
 * \param  ISO7816_INT: specifies the interrupt source to check.
 *         This parameter can be one of the following values:
 *         \arg ISO7816_INT_RESET_TIMING_VIOLATION: Reset timing violation.
 *         \arg ISO7816_INT_TX_NAK_THD:             TNAKTH exceeded.
 *         \arg ISO7816_INT_RX_WAIT_TIMEOUT:        Character wait timeout.
 *         \arg ISO7816_INT_GP_COUNTER:             General purpose counter hit.
 *         \arg ISO7816_INT_TX_FIFO_EMPTY:          TX FIFO empty.
 *         \arg ISO7816_INT_TX_FIFO_NOT_FULL:       TX FIFO not full, triggered when TX FIFO changes from full to non-full.
 *         \arg ISO7816_INT_TX_FIFO_OVERFLOW:       TX FIFO overflow.
 *         \arg ISO7816_INT_RX_FIFO_NOT_EMPTY:      RX FIFO not empty, triggered when RX FIFO changes from empty to non-empty.
 *         \arg ISO7816_INT_RX_FIFO_ALMOST_FULL:    RX FIFO almost full
 *         \arg ISO7816_INT_RX_FIFO_FULL:           RX FIFO full.
 *         \arg ISO7816_INT_RX_FIFO_OVERFLOW:       RX FIFO overflow.
 *         \arg ISO7816_INT_RX_FIFO_UNDERFLOW:      RX FIFO underflow.
 *         \arg ISO7816_INT_TX_DONE:                Transmission completed, triggered when TX FIFO is empty and the last bit has been sent out.
 * \return The new state of ISO7816_INT (SET or RESET).
 */
uint16_t ISO7816_GetINTStatus(void)
{
    return (uint16_t)(ISO7816->ISR & 0x1FFF);
}


/**
 * \brief       Check whether the specified ISO7816 error status is set or not.
 * \param[in]   None.
 * \return      The new state of ISO7816_Flags (SET or RESET).
 */
uint8_t ISO7816_GetErrStatus(void)
{
    return (uint8_t)(ISO7816->ESR & 0xFF);
}


/**
 * \brief       Clear the specified ISO7816 error status.
 * \param[in]   status: 0~0xFFFF.
 * \return      None.
 */
void ISO7816_ClearErrStatus(uint8_t status)
{
    ISO7816->ESR |= status & 0xFF;
}

/**
 * \brief       Send one byte of data.
 * \param[in]   data: Byte data to send.
 * \return      None.
 */
void ISO7816_SendByte(uint8_t data)
{
    ISO7816->TX_FIFO = data;
}

/**
 * \brief   Read one byte of data from SmartCard RX FIFO.
 * \param   None.
 * \return  Which byte data has been read.
 */
uint8_t ISO7816_ReceiveByte(void)
{
    return (uint8_t)(ISO7816->RX_FIFO);
}

/**
 * \brief   Clear Tx FIFO of SmartCard peripheral.
 * \param   None.
 * \return  None.
 */
void ISO7816_ClearTxFIFO(void)
{
    ISO7816->CR |= ISO7816_TX_FIFO_CLEAR_MSK;
}

/**
 * \brief   Clear Rx FIFO of SmartCard peripheral.
 * \param   None.
 * \return  None.
 */
void ISO7816_ClearRxFIFO(void)
{
    ISO7816->CR |= ISO7816_RX_FIFO_CLEAR_MSK;
}

/**
 * \brief       Restart the general purpose counter.
 * \param[in]   value: counter limitation, 0~0xFFFF.
 * \return      None.
 */
void ISO7816_GPCounterRestart(uint16_t value)
{
    ISO7816->CR &= ISO7816_GP_COUNTER_ENABLE_CLR;
    ISO7816->CR &= ISO7816_GP_COUNTER_CLR;
    ISO7816->CR |= ISO7816_GP_COUNTER_ENABLE_MSK | value ;
}

/**
 * \brief       Set the general purpose counter value.
 * \param[in]   value: counter limitation, 0~0xFFFF.
 * \return      None.
 */
void ISO7816_GPCounterSet(uint16_t value)
{
    ISO7816->CR &= ISO7816_GP_COUNTER_CLR;
    ISO7816->CR |= value ;
}

/**
 * \brief       Enable the general purpose counter.
 * \param[in]   None.
 * \return      None.
 */
void ISO7816_GPCounterEnable(void)
{
    ISO7816->CR |= ISO7816_GP_COUNTER_ENABLE_MSK;
}

/**
 * \brief       Disable the general purpose counter.
 * \param[in]   None.
 * \return      None.
 */
void ISO7816_GPCounterDisable(void)
{
    ISO7816->CR &= ISO7816_GP_COUNTER_ENABLE_CLR;
}

/**
 * \brief       Set RST output high level.
 * \param[in]   None.
 * \return      None.
 */
void ISO7816_SetRST(void)
{
    ISO7816->CR |= ISO7816_RESET_ENABLE_MSK;//RST = 1.
}

/**
 * \brief       Config the specified ISO7816 protocol type.
 * \param[in]   protocol:  0: T=0, mandatory error signal and character repetition; 1: T=1.
 * \return      None.
 */
void ISO7816_ProtocolTypeConfig(uint8_t protocol)
{
    if (protocol)
    {
        ISO7816->CR |= ISO7816_PROTOCOL_TYPE_MSK;
    }
    else
    {
        ISO7816->CR &= ISO7816_PROTOCOL_TYPE_CLR;
    }
}

/**
 * \brief       Config the clk output polar when clock stop.
 * \param[in]   polar: 0: low level, 1: high level.
 * \return      None.
 */
void ISO7816_ClockPolarConfig(uint8_t polar)
{
    if (polar)
    {
        ISO7816->CR |= ISO7816_CLOCK_STOP_POLAR_MSK;
    }
    else
    {
        ISO7816->CR &= ISO7816_CLOCK_STOP_POLAR_CLR;
    }
}

/**
 * \brief       Config the character convention of TS.
 * \param[in]   convention: 0: direct convention, 1: inverse convention.
 * \return      None.
 */
void ISO7816_ConventionConfig(uint8_t convention)
{
    if (convention)
    {
        ISO7816->CR |= ISO7816_CODING_CONVENTION_MSK;
    }
    else
    {
        ISO7816->CR &= ISO7816_CODING_CONVENTION_CLR;
    }
}

/**
 * \brief       Set speed of etu time (equal to F/D).
 * \param[in]   value: 0~0xFFF.
 * \return      None.
 */
void ISO7816_SetSpeed(uint16_t speed)
{
    ISO7816->GCR = speed & 0xFFF;
}

/**
 * \brief       Set extra guard time N (in unit of etu).
 * \param[in]   time: 0~0xFF.
 * \return      None.
 */
void ISO7816_SetGT(uint8_t time)
{
    ISO7816->TCR = time & 0xFF;
}

/**
 * \brief       Set waiting time (WWT in T=0 or CWT in T=1).
 * \param[in]   value: 0~0xFFFE.
 * \return      None.
 */
void ISO7816_SetWT(uint16_t time)
{
    ISO7816->RCR |= time & 0xFFFE;
}

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor *****END OF FILE****/
