/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_spi.c
* @brief    This file provides all the Spi firmware functions.
* @details
* @author   elliot chen
* @date     2023-02-10
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "rtl876x_spi.h"
#include "rtl876x_rcc.h"

/**
  * @brief  SPI DLPS store register.
  */
typedef struct
{
    uint32_t spi_reg[14];
} SPIStoreReg_Typedef;

/**
  * @brief  Deinitializes the SPIx peripheral registers to their default reset values.
  * @param  SPIx: where x can be 0 or 1 to select the SPI peripheral.
  * @retval None
  */
void SPI_DeInit(SPI_TypeDef *SPIx)
{
    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));

    /*Disable SPI clock */
    if (SPIx == SPI0)
    {
        RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, DISABLE);
    }
    else if (SPIx == SPI1)
    {
        RCC_PeriphClockCmd(APBPeriph_SPI1, APBPeriph_SPI1_CLOCK, DISABLE);
    }
    else
    {
        RCC_PeriphClockCmd(APBPeriph_SPI2, APBPeriph_SPI2_CLOCK, DISABLE);
    }
}

/**
  * @brief  Initializes the SPIx peripheral according to the specified
  *   parameters in the SPI_InitStruct.
  * @param  SPIx: where x can be 0 or 1 to select the SPI peripheral.
  * @param  SPI_InitStruct: pointer to a SPI_InitTypeDef structure that
  *   contains the configuration information for the specified SPI peripheral.
  * @retval None
  */
void SPI_Init(SPI_TypeDef *SPIx, SPI_InitTypeDef *SPI_InitStruct)
{
    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));

    /* Check the SPI parameters */
    assert_param(IS_SPI_DIRECTION_MODE(SPI_InitStruct->SPI_Direction));
    assert_param(IS_SPI_MODE(SPI_InitStruct->SPI_Mode));
    assert_param(IS_SPI_DATASIZE(SPI_InitStruct->SPI_DataSize));
    assert_param(IS_SPI_CPOL(SPI_InitStruct->SPI_CPOL));
    assert_param(IS_SPI_CPHA(SPI_InitStruct->SPI_CPHA));
    assert_param(IS_SPI_FRAME_FORMAT(SPI_InitStruct->SPI_FrameFormat));

    /* Disable SPI device before change configuration */
    SPIx->SSIENR &= ~0x01;

    /* Configure SPI0 mode if select SPI0 */
    if (SPIx == SPI0)
    {
        /*Open SPI0 APB Bridge for high speed*/
        SYSBLKCTRL->u_308.BITS_308.r_PON_SPI0_BRG_EN = 1;

        /* Close SPI0 AHB bridge*/
        SYSBLKCTRL->u_308.BITS_308.r_PON_SPI0_H2S_BRG_EN = 0;

        if (SPI_InitStruct->SPI_Mode == SPI_Mode_Master)
        {
            /* Enable SPI0 master mode */
            SYSBLKCTRL->u_308.BITS_308.r_PON_SPIO_MST = 1;

            /* configure SPI parameters */
            SPIx->CTRLR0 = (SPI_InitStruct->SPI_DataSize << 16)\
                           | (SPI_InitStruct->SPI_FrameFormat << 4) \
                           | (SPI_InitStruct->SPI_CPHA << 6)\
                           | (SPI_InitStruct->SPI_CPOL << 7) \
                           | (SPI_InitStruct->SPI_Direction << 8) \
                           | (SPI_InitStruct->SPI_SwapTxBitEn << 22) \
                           | (SPI_InitStruct->SPI_SwapRxBitEn << 24) \
                           | (SPI_InitStruct->SPI_SwapTxByteEn << 21) \
                           | (SPI_InitStruct->SPI_SwapRxByteEn << 23) \
                           | (SPI_InitStruct->SPI_ToggleEn << 31);
        }
        else
        {
            /* Enable SPI0 slave mode */
            SYSBLKCTRL->u_308.BITS_308.r_PON_SPIO_MST = 0;
            /* configure SPI parameters */
            SPIx->CTRLR0 = (SPI_InitStruct->SPI_DataSize)
                           | (SPI_InitStruct->SPI_FrameFormat << 4)
                           | (SPI_InitStruct->SPI_CPHA << 6)
                           | (SPI_InitStruct->SPI_CPOL << 7)
                           | (SPI_InitStruct->SPI_Direction << 8)
                           | (SPI_InitStruct->SPI_SwapTxBitEn << 22)
                           | (SPI_InitStruct->SPI_SwapRxBitEn << 24)
                           | (SPI_InitStruct->SPI_SwapTxByteEn << 21)
                           | (SPI_InitStruct->SPI_SwapRxByteEn << 23)
                           | (SPI_InitStruct->SPI_ToggleEn << 31);
        }
    }
    else if (SPIx == SPI1 || SPIx == SPI2)
    {
        /*Open SPI1/SPI2 APB Bridge for high speed*/
        if (SPIx == SPI1)
        {
            SYSBLKCTRL->u_308.BITS_308.r_PON_SPI1_BRG_EN = 1;
        }
        else
        {
            SYSBLKCTRL->u_308.BITS_308.r_PON_SPI2_BRG_EN = 1;
        }

        /* configure SPI parameters */
        SPIx->CTRLR0 = (SPI_InitStruct->SPI_DataSize << 16)
                       | (SPI_InitStruct->SPI_FrameFormat << 4)
                       | (SPI_InitStruct->SPI_CPHA << 6)
                       | (SPI_InitStruct->SPI_CPOL << 7)
                       | (SPI_InitStruct->SPI_Direction << 8)
                       | (SPI_InitStruct->SPI_SwapTxBitEn << 22)
                       | (SPI_InitStruct->SPI_SwapRxBitEn << 24)
                       | (SPI_InitStruct->SPI_SwapTxByteEn << 21)
                       | (SPI_InitStruct->SPI_SwapRxByteEn << 23)
                       | (SPI_InitStruct->SPI_ToggleEn << 31);
    }
    else if (SPIx == SPI0_HS)
    {
        /* Enable SPI0 master mode */
        SYSBLKCTRL->u_308.BITS_308.r_PON_SPIO_MST = 1;

        /* Open SPI0 AHB bridge*/
        SYSBLKCTRL->u_308.BITS_308.r_PON_SPI0_H2S_BRG_EN = 1;

        /* configure SPI parameters */
        SPIx->CTRLR0 = (SPI_InitStruct->SPI_DataSize << 16)\
                       | (SPI_InitStruct->SPI_FrameFormat << 4) \
                       | (SPI_InitStruct->SPI_CPHA << 6)\
                       | (SPI_InitStruct->SPI_CPOL << 7) \
                       | (SPI_InitStruct->SPI_Direction << 8) \
                       | (SPI_InitStruct->SPI_SwapTxBitEn << 22) \
                       | (SPI_InitStruct->SPI_SwapRxBitEn << 24) \
                       | (SPI_InitStruct->SPI_SwapTxByteEn << 21) \
                       | (SPI_InitStruct->SPI_SwapRxByteEn << 23) \
                       | (SPI_InitStruct->SPI_ToggleEn << 31);
    }

    /* configure SPI clock speed  in master mode or  enable slave output in slave mode */
    if (SPI_InitStruct->SPI_Mode == SPI_Mode_Master)
    {
        SPIx->BAUDR = (SPI_InitStruct->SPI_BaudRatePrescaler) % 0xFFFF;
        /* Enable slave Select function in master mode */
        SPIx->SER |= BIT(0);
    }
    else
    {
        /* Enable slave output */
        SPIx->CTRLR0 &= ~(BIT(10));
    }

    /* Set rx sample dly */
    SPIx->RX_SAMPLE_DLY = SPI_InitStruct->SPI_RxSampleDly;

    /*set SPI Tx and Rx threshold level ,below this level or equal this level would trigger Tx and Rx FIFO empty interrupt */
    SPIx->TXFTLR = SPI_InitStruct->SPI_TxThresholdLevel;

    /* mask SPI interrupt in REG_DW_SSI_IMR */
    SPIx->IMR = 0;

    /* set read length in SPI EEPROM & rx only mode */
    if ((SPI_InitStruct->SPI_Direction == 0x02) || (SPI_InitStruct->SPI_Direction == 0x03))
    {
        SPIx->CTRLR1 = SPI_InitStruct->SPI_NDF;
    }
#if 1

    /* Config SPI dma mode */
    SPIx->DMACR = ((SPI_InitStruct->SPI_RxDmaEn)\
                   | ((SPI_InitStruct->SPI_TxDmaEn) << 1));

    /* Config SPI tx waterlevel */
    SPIx->DMATDLR = SPI_InitStruct->SPI_TxWaterlevel;

    if (SPI_InitStruct->SPI_RxDmaEn)
    {
        /* DMA mode should config DMARDLR later if open AHB */
        SPIx->RXFTLR = SPI_InitStruct->SPI_RxThresholdLevel;
        SPIx->DMARDLR = SPI_InitStruct->SPI_RxWaterlevel;
    }
    else
    {
        /* CPU mode should config RXFTLR later if open AHB */
        SPIx->DMARDLR = SPI_InitStruct->SPI_RxWaterlevel;
        SPIx->RXFTLR = SPI_InitStruct->SPI_RxThresholdLevel;
    }

#endif
}

/**
  * @brief  Fills each SPI_InitStruct member with its default value.
  * @param  SPI_InitStruct : pointer to a SPI_InitTypeDef structure which will be initialized.
  * @retval None
  */
void SPI_StructInit(SPI_InitTypeDef *SPI_InitStruct)
{
    SPI_InitStruct->SPI_Mode              = SPI_Mode_Master;
    SPI_InitStruct->SPI_DataSize          = SPI_DataSize_8b;    /* 8-bit serial data transfer */
    SPI_InitStruct->SPI_FrameFormat       = 0;                  /* 0:FRF_MOTOROLA_SPI; 1,2,3:Reserved */
    SPI_InitStruct->SPI_Direction         =
        0;                  /* 0:TX and RX Mode; 1:TX Only Mode; 2:RX Only Mode;
                                                                   3:EEPROM Read Mode */
    SPI_InitStruct->SPI_CPOL              =
        1;                  /* 0:inactive state of clock is low; 1:inactive
                                                                   state of clock is high */
    SPI_InitStruct->SPI_CPHA              =
        1;                  /* 1:Serial clock toggles in first of first data bit;
                                                                   0:Serial clock toggles in middle of first data bit*/
    SPI_InitStruct->SPI_BaudRatePrescaler            =
        80;             /* Speed = SPI Clock source/ SPI_ClkDIV*/
    SPI_InitStruct->SPI_TxThresholdLevel  = 1;                  /* Transmit FIFO Threshold */
    SPI_InitStruct->SPI_RxThresholdLevel  = 0;                  /* Receive FIFO Threshold */
    SPI_InitStruct->SPI_NDF               = 1;
    SPI_InitStruct->SPI_SwapRxBitEn       = SPI_SWAP_DISABLE;       /* reverse the rx bit or not */
    SPI_InitStruct->SPI_SwapTxBitEn       = SPI_SWAP_DISABLE;
    SPI_InitStruct->SPI_SwapRxByteEn      = SPI_SWAP_DISABLE;
    SPI_InitStruct->SPI_SwapTxByteEn      = SPI_SWAP_DISABLE;
    SPI_InitStruct->SPI_ToggleEn          = DISABLE;

    SPI_InitStruct->SPI_RxDmaEn           = DISABLE;
    SPI_InitStruct->SPI_TxDmaEn           = DISABLE;
    SPI_InitStruct->SPI_RxWaterlevel      =
        1;                   /* SPI Rx waterlevel, should be less than fifo threshold, the best value is
                                                                    GDMA Msize */
    SPI_InitStruct->SPI_TxWaterlevel      =
        35;                  /* SPI Tx waterlevel, should be less than fifo threshold, the best value is
                                                                    SPI FIFO Depth minus GDMA Msize */
    SPI_InitStruct->SPI_RxSampleDly       =
        0;                   /* SPI Rx sample dly, should be set to 1 if the spi master clock greater
                                                                    than  10M*/
}
/**
  * @brief  Set spi prescaler, Fsclk_out = Fssi_clk/prescaler  use this fuction to get spi sclk. before use this api spi should be disable first.
  * @param  SPIx: where x can be 0 or 1  2to select the SPI peripheral.
  * @param  prescaler: any even value is between 2 and 65534 , means the prescaler of spi clk source ,clk source is 40Mhz by default.
  *  Specifies the speed of SCK clock. SPI Clock Speed = clk source/prescaler
    @note The communication clock is derived from the master
      clock. The slave clock does not need to be set.
  * @retval None
  */
void SPI_SetPrescaler(SPI_TypeDef *SPIx, uint16_t prescaler)
{
    SPIx->BAUDR = (prescaler) % 0xFFFF;
}

/**
  * @brief  Enables or disables the specified SPI peripheral.
  * @param  SPIx: where x can be 0 or 1 to select the SPI peripheral.
  * @param  NewState: new state of the SPIx peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SPI_Cmd(SPI_TypeDef *SPIx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the selected SPI peripheral */
        SPIx->SSIENR |= 0x01;
    }
    else
    {
        /* Disable the selected SPI peripheral */
        SPIx->SSIENR &= ~0x01;
    }
}

/**
  * @brief  Transmits a number of bytes through the SPIx peripheral.
  * @param  SPIx: where x can be 0 or 1
  * @param  Data : bytes to be transmitted.
  * @retval None
  */
void SPI_SendBuffer(SPI_TypeDef *SPIx, uint8_t *pBuf, uint16_t len)
{
    uint16_t i = 0;

    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));

    for (i = 0; i < len; i++)
    {
        SPIx->DR[0] = (*pBuf++);
        /*read TFNF bit status in SR register: SET is Tx FIFO is not full*/
        while (!(SPIx->SR & BIT(1)));
    }
}

/**
  * @brief  Transmits a number of words through the SPIx peripheral.
  * @param  SPIx: where x can be 0 or 1
  * @param  Data : words to be transmitted.
  * @retval None
  */
void SPI_SendWord(SPI_TypeDef *SPIx, uint32_t *pBuf, uint16_t len)
{
    uint16_t i = 0;

    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));

    for (i = 0; i < len; i++)
    {
        SPIx->DR[0] = (*pBuf++);
        /*read TFNF bit status in SR register: SET is Tx FIFO is not full*/
        while (!(SPIx->SR & BIT(1)));
    }
}

/**
  * @brief  Transmits a number of halfWords through the SPIx peripheral.
  * @param  SPIx: where x can be 0 or 1
  * @param  Data : Halfwords to be transmitted.
  * @retval None
  */
void SPI_SendHalfWord(SPI_TypeDef *SPIx, uint16_t *pBuf, uint16_t len)
{
    uint16_t i = 0;

    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));

    for (i = 0; i < len; i++)
    {
        SPIx->DR[0] = (*pBuf++);
        /*read TFNF bit status in SR register: SET is Tx FIFO is not full*/
        while (!(SPIx->SR & BIT(1)));
    }
}

/**
  * @brief  Enables or disables the specified SPI/I2S interrupts.
  * @param  SPIx: where x can be 0 or 1
  * @param  SPI_IT: specifies the SPI/I2S interrupt source to be enabled or disabled.
  *   This parameter can be one of the following values:
  *     @arg SPI_INT_TXE: Tx buffer empty interrupt mask
  *     @arg SPI_INT_TXO: Tx buffer overflow interrupt mask
  *     @arg SPI_INT_RXU: receive FIFO Underflow Interrupt mask
  *     @arg SPI_INT_RXO: receive FIFO Overflow Interrupt mask
  *     @arg SPI_INT_RXF: receive FIFO Full Interrupt mask which equal RXNE Interrupt!!!
  *     @arg SPI_INT_MST: multi-Master Contention Interrupt mask
  * @param  NewState: new state of the specified SPI interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SPI_INTConfig(SPI_TypeDef *SPIx, uint8_t SPI_IT, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    assert_param(IS_SPI_CONFIG_IT(SPI_IT));

    if (NewState != DISABLE)
    {
        /* Enable the selected SPI interrupt */
        SPIx->IMR |= SPI_IT;
    }
    else
    {
        /* Disable the selected SPI interrupt */
        SPIx->IMR &= (uint16_t)~(SPI_IT);
    }
}

/**
  * @brief  Clear the specified SPI interrupt.
  * @param  SPIx: where x can be 0 or 1
  * @param  SPI_IT: specifies the SPI interrupt to clear.
  *   This parameter can be one of the following values:
  *     @arg SPI_INT_MST: Multi-Master Contention Interrupt.
  *     @arg SPI_INT_RXO: Receive FIFO Overflow Interrupt.
  *     @arg SPI_INT_RXU: Receive FIFO Underflow Interrupt.
  *     @arg SPI_INT_TXO: Transmit FIFO Overflow Interrupt .
  * @retval None.
  */
void SPI_ClearINTPendingBit(SPI_TypeDef *SPIx, uint16_t SPI_IT)
{
    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));
    assert_param(IS_SPI_CONFIG_IT(SPI_IT));

    switch (SPI_IT)
    {
    case SPI_INT_RXO:
        SPIx->RXOICR;
        break;
    case SPI_INT_RXU:
        SPIx->RXUICR;
        break;
    case SPI_INT_TXO:
        SPIx->TXOICR;
        break;
    case SPI_INT_MST:
        SPIx->FAEICR;
        break;
    case SPI_INT_TUF:
        SPIx->TXUICR;
        break;
    case SPI_INT_RIG:
        SPIx->SSRICR;
        break;
    default:
        break;
    }

}

uint32_t SPI_GetDLPSBufferLen(void)
{
    return sizeof(SPIStoreReg_Typedef);
}

/**
  * @brief  SPI0 enter dlps callback function(Save SPI0 register values when system enter DLPS)
  * @param  None
  * @retval None
  */
void SPI_DLPSEnter(void *peri_reg, void *p_store_buf)
{
    SPIStoreReg_Typedef *store_buf = (SPIStoreReg_Typedef *)p_store_buf;
    SPI_TypeDef *SPIx = (SPI_TypeDef *)peri_reg;

    store_buf->spi_reg[0] = SPIx->CTRLR0;
    store_buf->spi_reg[1] = SPIx->CTRLR1;
    store_buf->spi_reg[2] = SPIx->SSIENR;
    store_buf->spi_reg[3] = SPIx->SER;
    store_buf->spi_reg[4] = SPIx->BAUDR;
    store_buf->spi_reg[5] = SPIx->TXFTLR;
    store_buf->spi_reg[6] = SPIx->RXFTLR;
    store_buf->spi_reg[7] = SPIx->IMR;
    store_buf->spi_reg[8] = SPIx->DMACR;
    store_buf->spi_reg[9] = SPIx->DMATDLR;
    store_buf->spi_reg[10] = SPIx->DMARDLR;
    store_buf->spi_reg[11] = SPIx->RX_SAMPLE_DLY;
    store_buf->spi_reg[12] = *(volatile uint32_t *)0x40000308;
    store_buf->spi_reg[13] = *(volatile uint32_t *)0x4000035CUL;
}

/**
  * @brief  SPI0 exit dlps callback function(Resume SPI0 register values when system exit DLPS)
  * @param  None
  * @retval None
  */
void SPI_DLPSExit(void *peri_reg, void *p_store_buf)
{
    SPIStoreReg_Typedef *store_buf = (SPIStoreReg_Typedef *)p_store_buf;
    SPI_TypeDef *SPIx = (SPI_TypeDef *)peri_reg;

    *(volatile uint32_t *)0x4000035CUL = store_buf->spi_reg[13];
    *(volatile uint32_t *)0x40000308 = store_buf->spi_reg[12];

    SPIx->CTRLR0  = store_buf->spi_reg[0];
    SPIx->CTRLR1  = store_buf->spi_reg[1];
    SPIx->SER     = store_buf->spi_reg[3];
    SPIx->BAUDR   = store_buf->spi_reg[4];
    SPIx->TXFTLR  = store_buf->spi_reg[5];
    SPIx->RXFTLR  = store_buf->spi_reg[6];
    SPIx->IMR     = store_buf->spi_reg[7];
    SPIx->DMACR   = store_buf->spi_reg[8];
    SPIx->DMATDLR = store_buf->spi_reg[9];
    SPIx->DMARDLR = store_buf->spi_reg[10];
    SPIx->RX_SAMPLE_DLY = store_buf->spi_reg[11];

    /* Enable the selected SPI peripheral */
    SPIx->SSIENR = store_buf->spi_reg[2];
}

/******************* (C) COPYRIGHT 2023 Realtek Semiconductor Corporation *****END OF FILE****/

