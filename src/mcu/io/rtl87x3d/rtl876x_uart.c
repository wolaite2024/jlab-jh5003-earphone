/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_uart.c
* @brief    This file provides all the UART firmware functions.
* @details
* @author   tifnan_ge
* @date     2023-02-10
* @version  v0.1
*********************************************************************************************************
*/
#include <string.h>
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "rtl876x_rcc.h"
#include "rtl876x_uart.h"
#include "section.h"

#define PERI_DEVELOP                        0
#define PERI_TEST_MP                        1
#define LCR_DLAB_Set                    ((uint32_t)(1 << 7))
#define LCR_DLAB_Reset                  ((uint32_t)~(1 << 7))

/**
 * @brief UART DLPS restore register.
 */
typedef struct
{
    uint32_t uart_reg[11];
} UARTStoreReg_Typedef;

/**
  * @brief Initializes the UART peripheral according to the specified
  *   parameters in the UART_InitStruct
  * @param  UARTx: selected UART peripheral.
  * @param  UART_InitStruct: pointer to a UART_InitTypeDef structure that
  *   contains the configuration information for the specified UART peripheral
  * @retval None
  */
void UART_Init(UART_TypeDef *UARTx, UART_InitTypeDef *UART_InitStruct)
{
    assert_param(IS_UART_PERIPH(UARTx));
    assert_param(IS_UART_WORD_LENGTH(UART_InitStruct->wordLen));
    assert_param(IS_UART_PARITY(UART_InitStruct->parity));
    assert_param(IS_UART_STOPBITS(UART_InitStruct->stopBits));
    assert_param(IS_UART_AUTO_FLOW_CTRL(UART_InitStruct->autoFlowCtrl));
    assert_param(IS_UART_DMA_CFG(UART_InitStruct->dmaEn));
    assert_param(IS_UART_RX_FIFO_TRIGGER_LEVEL(UART_InitStruct->rxTriggerLevel));
    assert_param(IS_UART_IDLE_TIME(UART_InitStruct->idle_time));

    // force clock always on, uart bug
    HAL_UPDATE32(VENDOR_REG_BASE + 0x24, BIT29 | BIT30, BIT29 | BIT30);

    //clear DLAB bit
    UARTx->LCR &= LCR_DLAB_Reset;
    //disable all interrupt
    UARTx->DLH_INTCR = 0x00;

    //read to clear Line Status Reg
    UARTx->LSR;
    //clear FIFO
    UARTx->INTID_FCR |= FCR_CLEAR_RX_FIFO_Set;

    //set baudrate, firstly set DLAB bit
    UARTx->LCR |= LCR_DLAB_Set;
    //set calibration parameters(OVSR)
    UARTx->STSR &= ~0xF0;
    UARTx->STSR |= (UART_InitStruct->ovsr << 4);
    //set calibration parameters(OVSR_adj)
    UARTx->SPR &= (~(0x7ff << 16));
    UARTx->SPR |= (UART_InitStruct->ovsr_adj << 16);
    //set DLL and DLH
    UARTx->DLL = (UART_InitStruct->div & 0x00FF);
    UARTx->DLH_INTCR = ((UART_InitStruct->div & 0xFF00) >> 8);
    //after set baudrate, clear DLAB bit
    UARTx->LCR &= LCR_DLAB_Reset;

    //set LCR reg
    UARTx->LCR = (UART_InitStruct->parity | UART_InitStruct->stopBits | UART_InitStruct->wordLen);
#if  PERI_DEVELOP
    //set FCR reg, FIFO must enable
    UARTx->INTID_FCR = ((1 << 0) | UART_InitStruct->rxTriggerLevel | UART_InitStruct->dmaEn);
    //write-only register, must be saved in initialization!
    UartFcrRegVal = ((1 << 0) | UART_InitStruct->rxTriggerLevel | UART_InitStruct->dmaEn);
#endif
#if  PERI_TEST_MP
    //set FCR reg, FIFO must enable
    UARTx->INTID_FCR = ((1 << 0) | UART_InitStruct->rxTriggerLevel << 8 | UART_InitStruct->dmaEn);
#endif

    /* auto flow control */
    UARTx->MCR &= (~((1 << 5) | (1 << 1)));
    UARTx->MCR |= UART_InitStruct->autoFlowCtrl;

    /* set rx idle time */
    UARTx->RX_IDLE_INTTCR = (UART_InitStruct->idle_time);

    if (UART_InitStruct->dmaEn == UART_DMA_ENABLE)
    {
        /* Config UART Tx dma parameter */
        if (UART_InitStruct->TxDmaEn != DISABLE)
        {
            /* Mask uart TX threshold value */
            UARTx->MISCR &= ~(0x1f << 3);
            UARTx->MISCR |= ((UART_InitStruct->TxWaterlevel) << 3) | BIT(1);
        }
        /* Config UART Rx dma parameter */
        if (UART_InitStruct->RxDmaEn != DISABLE)
        {
            /* Mask uart RX threshold value */
            UARTx->MISCR &= ~(0x3f << 8);
            UARTx->MISCR |= (UART_InitStruct->RxWaterlevel << 8) | BIT(2);
        }
    }

    return;
}

const static uint32_t uart_baudrate_map[] =
{
    1200, 9600, 14400, 19200, 28800, 38400, 57600, 76800, 115200, 128000, 153600, 230400, 460800, 500000, 921600, 1000000, 1382400, 1444400, 1500000, 1843200, 2000000, 3000000, 4000000, 6000000
};

/**
  *@brief  baudrate convert to UartBaudRate_TypeDef
  *@param  baudrate: select UART uint32_t baudrate
  *@retval Unsupport baudrate return 0xff
  */
UartBaudRate_TypeDef UART_ConvUartBaudRate(uint32_t baudrate)
{
    for (uint8_t i = 0; i < sizeof(uart_baudrate_map) / sizeof(uint32_t); i++)
    {
        if (uart_baudrate_map[i] == baudrate)
        {
            return (UartBaudRate_TypeDef)i;
        }
    }
    return (UartBaudRate_TypeDef)0xff;
}

/**
  *@brief  UartBaudRate_TypeDef convert to baudrate
  *@param  baudrate: select UART UartBaudRate_TypeDef baudrate
  *@retval Unsupport baudrate return 0
  */
uint32_t UART_ConvRateValue(UartBaudRate_TypeDef baudrate)
{
    if (baudrate >= sizeof(uart_baudrate_map) / sizeof(uint32_t))
    {
        return 0;
    }
    return uart_baudrate_map[baudrate];
}

/**
  *@brief  According to baudrate get UART param
  *@param[out]  div: div for setting baudrate
  *@param[out]  ovsr: ovsr for setting baudrate
  *@param[out]  ovsr_adj: ovsr_adj for setting baudrate
  *@param[in]   rate: select UART UartBaudRate_TypeDef baudrate
  *@retval Unsupport baudrate return false
  */
bool UART_ComputeDiv(uint16_t *div, uint16_t *ovsr, uint16_t *ovsr_adj, UartBaudRate_TypeDef rate)
{
    switch (rate)
    {
    case BAUD_RATE_1200:
        *div = 2589;
        *ovsr = 7;
        *ovsr_adj = 0x7F7;
        break;
    case BAUD_RATE_9600:
        *div = 271;
        *ovsr = 10;
        *ovsr_adj = 0x24A;
        break;
    case BAUD_RATE_14400:
        *div = 271;
        *ovsr = 5;
        *ovsr_adj = 0x222;
        break;
    case BAUD_RATE_19200:
        *div = 123;
        *ovsr = 11;
        *ovsr_adj = 0x6FF;
        break;
    case BAUD_RATE_28800:
        *div = 82;
        *ovsr = 11;
        *ovsr_adj = 0x6FF;
        break;
    case BAUD_RATE_38400:
        *div = 85;
        *ovsr = 7;
        *ovsr_adj = 0x222;
        break;
    case BAUD_RATE_57600:
        *div = 41;
        *ovsr = 11;
        *ovsr_adj = 0x6FF;
        break;
    case BAUD_RATE_76800:
        *div = 35;
        *ovsr = 9;
        *ovsr_adj = 0x7EF;
        break;
    case BAUD_RATE_115200:
        *div = 20;
        *ovsr = 12;
        *ovsr_adj = 0x252;
        break;
    case BAUD_RATE_128000:
        *div = 25;
        *ovsr = 7;
        *ovsr_adj = 0x555;
        break;
    case BAUD_RATE_153600:
        *div = 15;
        *ovsr = 12;
        *ovsr_adj = 0x252;
        break;
    case BAUD_RATE_230400:
        *div = 10;
        *ovsr = 12;
        *ovsr_adj = 0x252;
        break;
    case BAUD_RATE_460800:
        *div = 5;
        *ovsr = 12;
        *ovsr_adj = 0x252;
        break;
    case BAUD_RATE_500000:
        *div = 8;
        *ovsr = 5;
        *ovsr_adj = 0x0;
        break;
    case BAUD_RATE_921600:
        *div = 3;
        *ovsr = 9;
        *ovsr_adj = 0x2aa;
        break;
    case BAUD_RATE_1000000:
        *div = 4;
        *ovsr = 5;
        *ovsr_adj = 0x0;
        break;
    case BAUD_RATE_1382400:
        *div = 2;
        *ovsr = 9;
        *ovsr_adj = 0x2AA;
        break;
    case BAUD_RATE_1444400:
        *div = 2;
        *ovsr = 8;
        *ovsr_adj = 0x5F7;
        break;
    case BAUD_RATE_1500000:
        *div = 2;
        *ovsr = 8;
        *ovsr_adj = 0x492;
        break;
    case BAUD_RATE_1843200:
        *div = 2;
        *ovsr = 5;
        *ovsr_adj = 0x3F7;
        break;
    case BAUD_RATE_2000000:
        *div = 2;
        *ovsr = 5;
        *ovsr_adj = 0;
        break;
    case BAUD_RATE_3000000:
        *div = 1;
        *ovsr = 8;
        *ovsr_adj = 0x492;
        break;
    case BAUD_RATE_4000000:
        *div = 1;
        *ovsr = 5;
        *ovsr_adj = 0;
        break;
    case BAUD_RATE_6000000:
        *div = 1;
        *ovsr = 1;
        *ovsr_adj = 0x36D;
        break;
    default:
        return false;
    }
    return true;
}

/**
  * @brief Set baud rate of uart.
  * @param  UARTx: selected UART peripheral.
  * @param  baud_rate: baud rate to be set. value reference UartBaudRate_TypeDef.
  * @retval  true means set success ,false not support this baud rate.
  */
bool UART_SetBaudRate(UART_TypeDef *UARTx, UartBaudRate_TypeDef baud_rate)
{
    uint16_t div;
    uint16_t ovsr;
    uint16_t ovsr_adj;

    if (!UART_ComputeDiv(&div, &ovsr, &ovsr_adj, baud_rate))
    {
        return false;
    }
    /*set baudrate, firstly set DLAB bit*/
    UARTx->LCR |= LCR_DLAB_Set;
    /*set calibration parameters(OVSR)*/
    UARTx->STSR &= ~0xF0;
    UARTx->STSR |= (ovsr << 4);
    /*set calibration parameters(OVSR_adj)*/
    UARTx->SPR &= (~(0x7ff << 16));
    UARTx->SPR |= (ovsr_adj << 16);
    /*set DLL and DLH*/
    UARTx->DLL = (div & 0x00FF);
    UARTx->DLH_INTCR = ((div & 0xFF00) >> 8);
    /*after set baudrate, clear DLAB bit*/
    UARTx->LCR &= LCR_DLAB_Reset;
    return true;
}

/**
  * @brief  Deinitializes the UART peripheral registers to their default reset values(turn off UART clock).
  * @param  UARTx: selected UART peripheral.
  * @retval None
  */
void UART_DeInit(UART_TypeDef *UARTx)
{
    assert_param(IS_UART_PERIPH(UARTx));

    if (UARTx == UART0)
    {
        RCC_PeriphClockCmd(APBPeriph_UART0, APBPeriph_UART0_CLOCK, DISABLE);
    }
    else if (UARTx == UART1)
    {
        RCC_PeriphClockCmd(APBPeriph_UART1, APBPeriph_UART1_CLOCK, DISABLE);
    }
    else if (UARTx == UART2)
    {
        RCC_PeriphClockCmd(APBPeriph_UART2, APBPeriph_UART2_CLOCK, DISABLE);
    }
    else if (UARTx == UART3)
    {
        RCC_PeriphClockCmd(APBPeriph_UART3, APBPeriph_UART3_CLOCK, DISABLE);
    }
    return;
}

/**
  * @brief  Fills each UART_InitStruct member with its default value.
  * @param  UART_InitStruct: pointer to an UART_InitTypeDef structure which will be initialized.
  * @retval None
  */
void UART_StructInit(UART_InitTypeDef *UART_InitStruct)
{
    //115200 default
    UART_InitStruct->div            = 20;
    UART_InitStruct->ovsr           = 12;
    UART_InitStruct->ovsr_adj       = 0x252;

    UART_InitStruct->parity         = UART_PARITY_NO_PARTY;
    UART_InitStruct->stopBits       = UART_STOP_BITS_1;
    UART_InitStruct->wordLen        = UART_WROD_LENGTH_8BIT;
    UART_InitStruct->dmaEn          = UART_DMA_DISABLE;
    UART_InitStruct->autoFlowCtrl   = UART_AUTO_FLOW_CTRL_DIS;
    UART_InitStruct->rxTriggerLevel = 16;                       //1~29
    UART_InitStruct->idle_time      = UART_RX_IDLE_2BYTE;      //idle interrupt wait time
    UART_InitStruct->TxWaterlevel   = 15;     //Better to equal TX_FIFO_SIZE(16)- GDMA_MSize
    UART_InitStruct->RxWaterlevel   = 1;      //Better to equal GDMA_MSize
    UART_InitStruct->TxDmaEn   = DISABLE;
    UART_InitStruct->RxDmaEn   = DISABLE;
    return;
}

/**
  * @brief  Receive data from rx FIFO.
  * @param  UARTx: selected UART peripheral.
  * @param[out]  outBuf: buffer to save data read from UART FIFO.
  * @param  count: number of data to be read.
  * @retval None
  */
RAM_TEXT_SECTION void UART_ReceiveData(UART_TypeDef *UARTx, uint8_t *outBuf, uint16_t count)
{
    /* Check the parameters */
    assert_param(IS_UART_PERIPH(UARTx));

    while (count--)
    {
        *outBuf++ = (uint8_t)UARTx->RB_THR;
    }

    return;
}

/**
  * @brief  Send data to tx FIFO.
  * @param  UARTx: selected UART peripheral.
  * @param  inBuf: buffer to be written to Tx FIFO.
  * @param  count: number of data to be written.
  * @retval None
  */
RAM_TEXT_SECTION void UART_SendData(UART_TypeDef *UARTx, const uint8_t *inBuf, uint16_t count)
{
    /* Check the parameters */
    assert_param(IS_UART_PERIPH(UARTx));

    while (count--)
    {
        UARTx->RB_THR = *inBuf++;
    }

    return;
}
void UART_TxData(UART_TypeDef *UARTx, uint8_t *data, uint32_t len)
{
    uint32_t blkcount  = len / UART_TX_FIFO_SIZE;
    uint32_t remainder = len % UART_TX_FIFO_SIZE;
    uint32_t i = 0;

    for (i = 0; i < blkcount; ++i)
    {
        UART_SendData(UARTx, data + UART_TX_FIFO_SIZE * i, UART_TX_FIFO_SIZE);
        /* wait tx fifo empty */
        while (UART_GetFlagState(UARTx, UART_FLAG_THR_TSR_EMPTY) != SET);
    }

    /* send left bytes */
    if (remainder)
    {
        UART_SendData(UARTx, data + UART_TX_FIFO_SIZE * i, remainder);
        /* wait tx fifo empty */
        while (UART_GetFlagState(UARTx, UART_FLAG_THR_TSR_EMPTY) != SET);
    }
}
/**
  * @brief  Enables or disables the specified UART interrupts.
  * @param  UARTx: selected UARTx peripheral.
  * @param  UART_IT: specifies the UART interrupts sources to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg UART_INT_RD_AVA: enable Rx data avaliable interrupt.
  *     @arg UART_INT_FIFO_EMPTY: enable FIFO empty interrupt.
  *     @arg UART_INT_LINE_STS: enable line status interrupt.
  *     @arg UART_INT_MODEM_STS: enable modem status interrupt.
  * @param  NewState: new state of the specified UART interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
RAM_TEXT_SECTION void UART_INTConfig(UART_TypeDef *UARTx, uint32_t UART_IT,
                                     FunctionalState newState)
{
    /* Check the parameters */
    assert_param(IS_UART_PERIPH(UARTx));
    assert_param(IS_FUNCTIONAL_STATE(newState));
    assert_param(IS_UART_IT(UART_IT));

    if (UART_IT & UART_INT_IDLE)
    {
        if (newState == ENABLE)
        {
            UARTx->RXIDLE_INTCR |= BIT0;
            UARTx->RX_IDLE_INTTCR |= BIT31;
        }
        else
        {
            UARTx->RX_IDLE_INTTCR &= (~BIT31);
            UARTx->RX_IDLE_SR |= BIT0;
            UARTx->RXIDLE_INTCR &= (~BIT0);
        }
    }
    if (UART_IT & 0x1f)
    {
        if (newState == ENABLE)
        {
            /* Enable the selected UARTx interrupts */
            UARTx->DLH_INTCR |= UART_IT;
        }
        else
        {
            /* Disable the selected UARTx interrupts */
            UARTx->DLH_INTCR &= (uint32_t)~UART_IT;
        }
    }
    return;
}

/**
  * @brief  Checks whether the specified UART flag is set or not.
  * @param  UARTx: selected UART peripheral.
  * @param  UART_FLAG: specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg UART_FLAG_RX_DATA_RDY: rx data is avaliable.
  *     @arg UART_FLAG_RX_OVERRUN: rx overrun.
  *     @arg UART_FLAG_PARTY_ERR: parity error.
  *     @arg UART_FLAG_FRAME_ERR: UARTx frame error.
  *     @arg UART_FLAG_BREAK_ERR: UARTx break error.
  *     @arg UART_FLAG_THR_EMPTY: tx FIFO is empty.
  *     @arg UART_FLAG_THR_TSR_EMPTY: tx FIFO and tx shift reg are both empty.
  *     @arg UART_FLAG_RX_FIFO_ERR: rx FIFO error.
  *     @arg UART_FLAG_RX_IDLE.
  * @retval The new state of UART_FLAG (SET or RESET).
  */
RAM_TEXT_SECTION FlagStatus UART_GetFlagState(UART_TypeDef *UARTx, uint32_t UART_FLAG)
{
    FlagStatus bitstatus = RESET;

    /* Check the parameters */
    assert_param(IS_UART_PERIPH(UARTx));
    assert_param(IS_UART_GET_FLAG(UART_FLAG));

    if (UART_FLAG == UART_FLAG_RX_IDLE)
    {
        if (UARTx->RX_IDLE_SR & BIT(0))
        {
            bitstatus = SET;
        }
    }
    else if (UART_FLAG == UART_FLAG_TX_DONE)
    {
        if (UARTx->REG_TXDONE_INT & BIT(0))
        {
            bitstatus = SET;
        }
    }
    else
    {
        if (UARTx->LSR & UART_FLAG)
        {
            bitstatus = SET;
        }
    }
    return bitstatus;

}

/**
  *@brief  UART loop back mode config.
  *@param  UARTx: selected UART peripheral.
  *@param  NewState: new state of the DMA Channelx.
  *   This parameter can be: ENABLE or DISABLE.
  *@retval None.
  */
void UART_LoopBackCmd(UART_TypeDef *UARTx, FunctionalState NewState)
{
    assert_param(IS_UART_PERIPH(UARTx));

    if (NewState == ENABLE)
    {
        UARTx->MCR |= BIT4;
    }
    else
    {
        UARTx->MCR &= ~BIT4;
    }
}

void UART_IdleIntConfig(UART_TypeDef *UARTx, FunctionalState newState)
{
    if (newState == ENABLE)
    {
        UARTx->RXIDLE_INTCR |= BIT0;
        UARTx->RX_IDLE_INTTCR |= BIT31;
    }
    else
    {
        UARTx->RX_IDLE_INTTCR &= (~BIT31);
        UARTx->RX_IDLE_SR |= BIT0;
        UARTx->RXIDLE_INTCR &= (~BIT0);
    }
}


void UART_OneWireConfig(UART_TypeDef *UARTx, bool is_enable)
{
    if (UARTx == UART0)
    {
        SYSBLKCTRL->u_2A8.BITS_2A8.uart0_one_wire = is_enable;
    }
    else if (UARTx == UART1)
    {
        SYSBLKCTRL->u_2A8.BITS_2A8.uart1_one_wire = is_enable;
    }
    else if (UARTx == UART2)
    {
        SYSBLKCTRL->u_2A8.BITS_2A8.uart2_one_wire = is_enable;
    }
    else if (UARTx == UART3)
    {
        SYSBLKCTRL->u_2A8.BITS_2A8.uart3_one_wire = is_enable;
    }
}

void UART_RxDmaCmd(UART_TypeDef *UARTx, FunctionalState newState)
{
    /* Check the parameters */
    assert_param(IS_UART_PERIPH(UARTx));

    if (newState != DISABLE)
    {
        UARTx->MISCR |= BIT2;
    }
    else
    {
        UARTx->MISCR &= ~BIT2;
    }
}

/**
 * rtl876x_uart.h
 *
 * \brief    Mask or unmask the specified UART interrupts.
 *
 * \param[in]  UARTx: selected UARTx peripheral.
 * \param[in]  UART_INT_MASK: specifies the UART interrupts sources to be masked or unmasked.
 *      This parameter can be any combination of the following values:
 *      \arg UART_INT_MASK_RD_AVA: mask Rx data avaliable interrupt.
 *      \arg UART_INT_MASK_TX_FIFO_EMPTY: mask TX FIFO empty interrupt.
 *      \arg UART_INT_MASK_RX_LINE_STS: mask line status interrupt.
 *      \arg UART_INT_MASK_RX_BREAK: mask RX break interrupt.
 *      \arg UART_INT_MASK_RX_IDLE: mask RX idle timeout interrupt.
 *      \arg UART_INT_MASK_TX_DONE: mask the interrupt of TXD done & TX_FIFO_EMPTY = 1.
 *      \arg UART_INT_MASK_TX_THD: mask TX fifo threshold interrupt.
 * \param[in]  NewState: new state of the specified UART interrupts.
 *      This parameter can be: ENABLE or DISABLE.
 *
 * \return   None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void uart_demo(void)
 * {
 *     UART_MaskINTConfig(UART0, UART_INT_MASK_TX_FIFO_EMPTY, ENABLE);
 * }
 * \endcode
 */
RAM_TEXT_SECTION void UART_MaskINTConfig(UART_TypeDef *UARTx, uint32_t UART_INT_MASK,
                                         FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_UART_PERIPH(UARTx));
    assert_param(IS_UART_INT_MASK(UART_INT_MASK));
    assert_param(IS_FUNCTIONAL_STATE(newState));

    if (NewState != DISABLE)
    {
        UARTx->REG_INT_MASK |= UART_INT_MASK;
    }
    else
    {
        UARTx->REG_INT_MASK &= ~(UART_INT_MASK);
    }
}

RAM_TEXT_SECTION
void UART_ClearINT(UART_TypeDef *UARTx, uint32_t UART_FLAG)
{
    if (UART_FLAG == UART_FLAG_RX_IDLE)
    {
        UARTx->RX_IDLE_SR |= BIT0;
        UARTx->RX_IDLE_INTTCR |= BIT31;
    }
}

uint32_t UART_GetDLPSBufferLen(void)
{
    return sizeof(UARTStoreReg_Typedef);
}

/**
  * @brief  UART enter dlps callback function(Save UART register values when system enter DLPS)
  * @param  None
  * @retval None
  */
void UART_DLPSEnter(void *peri_reg, void *p_store_buf)
{
    UARTStoreReg_Typedef *store_buf = (UARTStoreReg_Typedef *)p_store_buf;
    UART_TypeDef *UARTx = (UART_TypeDef *)peri_reg;

    //access DLH and DLL
    UARTx->LCR |= (1 << 7);
    store_buf->uart_reg[0] = UARTx->DLL;
    store_buf->uart_reg[1] = UARTx->DLH_INTCR;
    UARTx->LCR &= (~(1 << 7));

    //save other registers
    store_buf->uart_reg[2] = UARTx->DLH_INTCR;
    store_buf->uart_reg[3] = UARTx->REG_INT_MASK;
    store_buf->uart_reg[4] = UARTx->LCR;
    store_buf->uart_reg[5] = UARTx->MCR;
    store_buf->uart_reg[6] = UARTx->SPR;
    store_buf->uart_reg[7] = UARTx->STSR;
    store_buf->uart_reg[8] = UARTx->RX_IDLE_INTTCR;
    store_buf->uart_reg[9] = UARTx->RXIDLE_INTCR;
    store_buf->uart_reg[10] = UARTx->MISCR;

    return;
}

/**
  * @brief  UART exit dlps callback function(Resume UART register values when system exit DLPS)
  * @param  None
  * @retval None
  */
void UART_DLPSExit(void *peri_reg, void *p_store_buf)
{
    UARTStoreReg_Typedef *store_buf = (UARTStoreReg_Typedef *)p_store_buf;
    UART_TypeDef *UARTx = (UART_TypeDef *)peri_reg;

    //access DLH and DLL
    UARTx->LCR |= (1 << 7);
    UARTx->STSR = store_buf->uart_reg[7];
    UARTx->SPR = store_buf->uart_reg[6];
    UARTx->DLL = store_buf->uart_reg[0];
    UARTx->DLH_INTCR =  store_buf->uart_reg[1];
    UARTx->LCR &= (~(1 << 7));

    //access other registers
    UARTx->INTID_FCR = (((store_buf->uart_reg[7] & BIT24) >> 21) | ((store_buf->uart_reg[7] &
                                                                     0x7C000000) >> 18) |
                        (1));
    UARTx->LCR = store_buf->uart_reg[4];
    UARTx->MCR = store_buf->uart_reg[5];
    UARTx->DLH_INTCR = store_buf->uart_reg[2];
    UARTx->REG_INT_MASK = store_buf->uart_reg[3];
    UARTx->RX_IDLE_INTTCR = store_buf->uart_reg[8];
    UARTx->RXIDLE_INTCR = store_buf->uart_reg[9];
    UARTx->MISCR = store_buf->uart_reg[10];

    return;
}

