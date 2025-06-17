/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* \file     rtl_ethernet.c
* \brief    This file provides all the ETHERNET firmware functions.
* \details
* \author   colin
* \date     2024-06-28
* \version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "rtl_ethernet.h"
#include "rtl876x_rcc.h"
#include "trace.h"
#include "platform_utils.h"

/*============================================================================*
 *                          Private Variable
 *============================================================================*/
ETHPHYOps_TypeDef ETH_PHY_OPS = {0};

/*============================================================================*
 *                          Private Functions
 *============================================================================*/
extern void ETH_ClockDeInit(void);

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
/**
  * \brief  Init PHY operations.
  * \param  phy_ops: PHY operations.
  * \return none.
  */
void ETH_PHYOpsInit(ETHPHYOps_TypeDef *phy_ops)
{
    memcpy(&ETH_PHY_OPS, phy_ops, sizeof(ETHPHYOps_TypeDef));
}

/**
  * \brief  Write PHY register.
  * \param  phy_addr: PHY address.
  * \param  address: PHY's register address.
  * \param  data: The data to be written to the above register address.
  * \return The result of the write operation.
  *         true: write success
  *         false: write failed
  */
bool ETH_WritePHYRegister(uint8_t phy_addr, uint8_t reg_address, uint16_t reg_data)
{
    uint32_t i = 0;

    ETH_MIIAR_TypeDef eth_reg_0x5c = {.d32 = ETH->ETH_MIIAR.d32};
    eth_reg_0x5c.b.flag = PHY_REG_WRITE_MODE;
    eth_reg_0x5c.b.phy_address = phy_addr;
    eth_reg_0x5c.b.reg_addr4_0 = reg_address;
    eth_reg_0x5c.b.data15_0 = reg_data;
    ETH->ETH_MIIAR.d32 = eth_reg_0x5c.d32;

    while (ETH->ETH_MIIAR.b.flag)
    {
        platform_delay_us(1);
        i++;
        if (i > ETH_TIMEOUT_CNT_MAX)
        {
#ifdef DEBUG_ETH
            ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_ERROR,
                           "ETH_WritePHYRegister: Wait write operation flag timeout!!", 0);
#endif
            return false;
        }
    }

    return true;

}

/**
  * \brief  Read PHY register.
  * \param  phy_addr: PHY address.
  * \param  address: PHY's register address.
  * \return The result of the read register data.
  */
uint16_t ETH_ReadPHYRegister(uint8_t phy_addr, uint8_t reg_address)
{
    uint32_t i = 0;

    ETH_MIIAR_TypeDef eth_reg_0x5c = {.d32 = ETH->ETH_MIIAR.d32};
    eth_reg_0x5c.b.flag = PHY_REG_READ_MODE;
    eth_reg_0x5c.b.phy_address = phy_addr;
    eth_reg_0x5c.b.reg_addr4_0 = reg_address;
    eth_reg_0x5c.b.data15_0 = 0;
    ETH->ETH_MIIAR.d32 = eth_reg_0x5c.d32;

    while (ETH->ETH_MIIAR.b.flag == 0)
    {
        platform_delay_us(1);
        i++;
        if (i > ETH_TIMEOUT_CNT_MAX)
        {
#ifdef DEBUG_ETH
            ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_ERROR,
                           "ETH_ReadPHYRegister: Wait read operation flag timeout!!", 0);
#endif
            return 0;
        }
    }

    return (uint16_t)((ETH->ETH_MIIAR.d32) & 0xFFFF);
}

/**
  * \brief  Set the Tx/Rx descriptor number.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  tx_desc_no: The specified Tx descriptor number.
  * \param  rx_desc_no: The specified Rx descriptor number.
  * \return ETHErrorStatus_Typedef: The status of ETH. \ref ETHErrorStatus_Typedef.
  */
ETHErrorStatus_Typedef ETH_SetDescNum(ETH_InitTypeDef *ETH_InitStruct, uint8_t tx_desc_no,
                                      uint8_t rx_desc_no)
{
    if ((ETH_InitStruct == NULL) || (tx_desc_no == 0) || (rx_desc_no == 0))
    {
#ifdef DEBUG_ETH
        ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_ERROR, "ETH_SetDescNum: Invalid parameter!!", 0);
#endif
        return ETH_STATUS_ERROR;
    }
    if (((tx_desc_no * sizeof(ETHTxDesc_TypeDef)) % 32) ||
        ((rx_desc_no * sizeof(ETHRxDesc_TypeDef)) % 32))
    {
#ifdef DEBUG_ETH
        ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_ERROR,
                       "ETH_SetDescNum: The size of Tx/Rx descriptor ring must be 32-Byte alignment!!", 0);
#endif
        return ETH_STATUS_ERROR;
    }

    ETH_InitStruct->ETH_TxDescNum = tx_desc_no;
    ETH_InitStruct->ETH_RxDescNum = rx_desc_no;

#ifdef DEBUG_ETH
    ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_INFO, "ETH_SetDescNum: ETH_TxDescNum=%d,ETH_RxDescNum=%d", 2,
                   ETH_InitStruct->ETH_TxDescNum, ETH_InitStruct->ETH_RxDescNum) ;
#endif

    return ETH_STATUS_OK;
}

/**
  * \brief  Set the start address of Tx/Rx descriptor ring.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  tx_desc: The start address of Tx descriptor ring.
  * \param  rx_desc: The start address of Rx descriptor ring.
  * \return ETHErrorStatus_Typedef: The status of ETH. \ref ETHErrorStatus_Typedef.
  */
ETHErrorStatus_Typedef ETH_SetDescAddr(ETH_InitTypeDef *ETH_InitStruct, uint8_t *tx_desc,
                                       uint8_t *rx_desc)
{
    if ((ETH_InitStruct == NULL) || (tx_desc == NULL) || (rx_desc == NULL))
    {
#ifdef DEBUG_ETH
        ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_ERROR, "ETH_SetDescAddr: Invalid parameter!!", 0);
#endif
        return ETH_STATUS_ERROR;
    }
    if ((((uint32_t)tx_desc) & 0x1F) || (((uint32_t)rx_desc) & 0x1F))
    {
#ifdef DEBUG_ETH
        ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_ERROR,
                       "ETH_SetDescAddr: The descriptor address must be 32-Byte alignment!!", 0);
#endif
        return ETH_STATUS_ERROR;
    }

    ETH_InitStruct->ETH_TxDesc = (ETHTxDesc_TypeDef *)tx_desc;
    ETH_InitStruct->ETH_RxDesc = (ETHRxDesc_TypeDef *)rx_desc;

    return ETH_STATUS_OK;
}

/**
  * \brief  Set the start address of Tx/Rx packet buffer.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  tx_pkt_buf: The start address of Tx packet buffer.
  * \param  rx_pkt_buf: The start address of Rx packet buffer.
  * \return ETHErrorStatus_Typedef: The status of ETH. \ref ETHErrorStatus_Typedef.
  */
ETHErrorStatus_Typedef ETH_SetPktBuf(ETH_InitTypeDef *ETH_InitStruct, uint8_t *tx_pkt_buf,
                                     uint8_t *rx_pkt_buf)
{
    if ((ETH_InitStruct == NULL) || (tx_pkt_buf == NULL) || (rx_pkt_buf == NULL))
    {
#ifdef DEBUG_ETH
        ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_ERROR, "ETH_SetPktBuf: Invalid parameter!!", 0);
#endif
        return ETH_STATUS_ERROR;
    }
    if ((((uint32_t)tx_pkt_buf) & 0x1F) || (((uint32_t)rx_pkt_buf) & 0x1F))
    {
#ifdef DEBUG_ETH
        ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_ERROR,
                       "ETH_SetPktBuf: The packet buffer address must be 32-Byte alignment!!", 0);
#endif
        return ETH_STATUS_ERROR;
    }

    ETH_InitStruct->ETH_TxPktBuf = tx_pkt_buf;
    ETH_InitStruct->ETH_RxPktBuf = rx_pkt_buf;

    return ETH_STATUS_OK;
}

/**
  * \brief  Set the ethernet MAC address.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  addr: The specified MAC address.
  * \return ETHErrorStatus_Typedef: The status of ETH. \ref ETHErrorStatus_Typedef.
  */
ETHErrorStatus_Typedef ETH_SetMacAddr(ETH_InitTypeDef *ETH_InitStruct, uint8_t *addr)
{
    if ((ETH_InitStruct == NULL) || (addr == NULL))
    {
#ifdef DEBUG_ETH
        ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_ERROR, "ETH_SetMacAddr: Invalid parameter!!", 0);
#endif
        return ETH_STATUS_ERROR;
    }

    memcpy((void *)(ETH_InitStruct->ETH_MacAddr), addr, MAC_ADDR_LEN);

    return ETH_STATUS_OK;
}

/**
  * \brief  Get the ethernet MAC address.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  addr: The buffer of MAC address.
  * \return ETHErrorStatus_Typedef: The status of ETH. \ref ETHErrorStatus_Typedef.
  */
ETHErrorStatus_Typedef ETH_GetMacAddr(ETH_InitTypeDef *ETH_InitStruct, uint8_t *addr)
{
    if ((ETH_InitStruct == NULL) || (addr == NULL))
    {
#ifdef DEBUG_ETH
        ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_ERROR, "ETH_GetMacAddr: Invalid parameter!!", 0);
#endif
        return ETH_STATUS_ERROR;
    }

    memcpy((void *)addr, ETH_InitStruct->ETH_MacAddr, MAC_ADDR_LEN);

    return ETH_STATUS_OK;
}

/**
  * \brief  Set buffer size.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  tx_alloc_buf_size: Allocated tx buffer size.
  * \param  rx_alloc_buf_size: Allocated rx buffer size.
  * \param  tx_buf_size: The tx buffer size that can be used actually.
  * \param  rx_buf_size: The rx buffer size that can be used actually.
  * \return ETHErrorStatus_Typedef: The status of ETH. \ref ETHErrorStatus_Typedef.
  */
ETHErrorStatus_Typedef ETH_SetBufSize(ETH_InitTypeDef *ETH_InitStruct, uint16_t tx_alloc_buf_size,
                                      uint16_t rx_alloc_buf_size, uint16_t tx_buf_size, uint16_t rx_buf_size)
{
    if ((ETH_InitStruct == NULL) ||
        ((tx_alloc_buf_size - 8) < tx_buf_size) ||
        ((rx_alloc_buf_size - 8) < rx_buf_size))
    {
#ifdef DEBUG_ETH
        ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_ERROR, "ETH_SetBufSize: Invalid parameter!!", 0);
#endif
        return ETH_STATUS_ERROR;
    }

    ETH_InitStruct->ETH_TxAllocBufSize = tx_alloc_buf_size;
    ETH_InitStruct->ETH_RxAllocBufSize = rx_alloc_buf_size;
    ETH_InitStruct->ETH_TxBufSize = tx_buf_size;
    ETH_InitStruct->ETH_RxBufSize = rx_buf_size;

    return ETH_STATUS_OK;
}

/**
  * @brief  Init ETH clock.
  * @param  None.
  * @return None.
  */
void ETH_ClkInit(void)
{
    RCC_PeriphClockCmd(APBPeriph_GMAC, APBPeriph_GMAC_CLOCK, ENABLE);
}

/**
  * \brief  Initialize the ethernet MAC controller and PHY RTL8201FR.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \return ETHErrorStatus_Typedef: The status of ETH. \ref ETHErrorStatus_Typedef.
  */
ETHErrorStatus_Typedef ETH_Init(ETH_InitTypeDef *ETH_InitStruct)
{
    uint32_t i, start_us;
    uint16_t tmp;

    /* reset MAC */
    ETH->ETH_CR.b.rst = 1;
    i = 0;
    do
    {
        i++;
        if (i > ETH_TIMEOUT_CNT_MAX)
        {
#ifdef DEBUG_ETH
            ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_ERROR, "ETH_Init: Wait reset MAC timeout!!", 0);
#endif
            break;
        }
    }
    while (ETH->ETH_CR.b.rst);

    /* Init PHY */
    if (ETH_InitStruct->ETH_Mode != ETH_LOOPBACK_T2R_MODE)
    {
        /* Disable MAC's auto-polling */
        ETH->ETH_MIIAR.b.disable_auto_polling = 1;

        /* Need delay, otherwise will cause write phy failed */
        platform_delay_ms(1);

        if (ETH_PHY_OPS.ETH_ResetPHY != NULL)
        {
            ETH_PHY_OPS.ETH_ResetPHY();
        }

        if (ETH_InitStruct->ETH_PhyMode == ETH_PHY_LOOPBACK_MODE)
        {
            if (ETH_PHY_OPS.ETH_PHYLoopback != NULL)
            {
                ETH_PHY_OPS.ETH_PHYLoopback(ETH_InitStruct);
            }
        }
    }

    /* RMII interface */
    ETH->ETH_MSR.b.reg_rmii2mii_en = 1;
    /* REFCLK on */
    ETH->ETH_MSR.b.refclk_on = 1;
    ETH->ETH_MSR.b.sel_rgmii = 0;

    /* set refclk phase */
    ETH->ETH_MSR.b.rx_refclk_phase = ETH_InitStruct->ETH_RxRefclkPhase;
    ETH->ETH_MSR.b.tx_refclk_phase = ETH_InitStruct->ETH_TxRefclkPhase;

    ETH->ETH_TCR.b.inter_frm_gp_tm = ETH_InitStruct->ETH_InterFrameGapTime;
    ETH->ETH_TCR.b.lpbk = ETH_InitStruct->ETH_Mode;

    /* Rx settings */
    ETH->ETH_IDR0.d32 = ((ETH_InitStruct->ETH_MacAddr[0]) << 24) | ((
                                                                        ETH_InitStruct->ETH_MacAddr[1]) << 16) | ((ETH_InitStruct->ETH_MacAddr[2]) << 8) |
                        (ETH_InitStruct->ETH_MacAddr[3]);
    ETH->ETH_IDR4.d32 = ((ETH_InitStruct->ETH_MacAddr[4]) << 24) | ((
                                                                        ETH_InitStruct->ETH_MacAddr[5]) << 16);
    ETH->ETH_RCR.d32 = ETH_InitStruct->ETH_ReceiveConfig;
    ETH->ETH_CR.b.rx_jumbo = ETH_InitStruct->ETH_RxJumboConfig;

    ETH_CPU->ETH_ETHRNTRXCPU_DES_NUM1.d32 = ETH_CPU_ETH_0X130_DEFAULT_VALUE;

    /* I/O command */
    ETH_CPU->ETH_IO_CMD1.d32 = ETH_CPU_ETH_0X138_DEFAULT_VALUE;
    ETH_CPU->ETH_ETHER_IO_CMD.d32 =
        ETH_CPU_ETH_0X134_DEFAULT_VALUE;
    ETH_CPU->ETH_ETHER_IO_CMD.b.tsh = ETH_InitStruct->ETH_TxThreshold;
    ETH_CPU->ETH_ETHER_IO_CMD.b.rxfth = ETH_InitStruct->ETH_RxThreshold;
    ETH_CPU->ETH_ETHER_IO_CMD.b.tx_int_mitigation_2_0 = ETH_InitStruct->ETH_TxTriggerLevel;
    ETH_CPU->ETH_ETHER_IO_CMD.b.rx_int_mitigation_2_0 = ETH_InitStruct->ETH_RxTriggerLevel;

    ETH_CPU->ETH_TXFDP1.d32 = (uint32_t)(ETH_InitStruct->ETH_TxDesc);
    ETH_CPU->ETH_RXFDP1.d32 = (uint32_t)(ETH_InitStruct->ETH_RxDesc);

    /* initialize Tx descriptors */
    for (i = 0; i < (ETH_InitStruct->ETH_TxDescNum); i++)
    {
        if (i == ((ETH_InitStruct->ETH_TxDescNum) - 1))
        {
            ETH_InitStruct->ETH_TxDesc[i].dw1 = ETH_TX_DESC_EOR;
        }
        ETH_InitStruct->ETH_TxDesc[i].addr = (uint32_t)(ETH_InitStruct->ETH_TxPktBuf +
                                                        (i * ETH_InitStruct->ETH_TxAllocBufSize));
        ETH_InitStruct->ETH_TxDesc[i].dw3 = 0;
        ETH_InitStruct->ETH_TxDesc[i].dw4 = 0;
    }

    /* initialize Rx descriptors */
    for (i = 0; i < (ETH_InitStruct->ETH_RxDescNum); i++)
    {
        if (i == ((ETH_InitStruct->ETH_RxDescNum) - 1))
        {
            ETH_InitStruct->ETH_RxDesc[i].dw1 = ETH_RX_DESC_OWN | ETH_RX_DESC_EOR |
                                                ETH_InitStruct->ETH_RxBufSize;
        }
        else
        {
            ETH_InitStruct->ETH_RxDesc[i].dw1 = ETH_RX_DESC_OWN | ETH_InitStruct->ETH_RxBufSize;
        }
        ETH_InitStruct->ETH_RxDesc[i].addr = (uint32_t)(ETH_InitStruct->ETH_RxPktBuf +
                                                        (i * ETH_InitStruct->ETH_RxAllocBufSize));
        ETH_InitStruct->ETH_RxDesc[i].dw2 = 0;
        ETH_InitStruct->ETH_RxDesc[i].dw3 = 0;
    }

    /* clear cnt */
    ETH->ETH_TXOKCNT.d16 = 1;
    ETH->ETH_RXOKCNT.d16 = 1;
    ETH->ETH_TXERR.d16 = 1;
    ETH->ETH_RXERR.d16 = 1;
    ETH->ETH_MISSPKT.d16 = 1;

    /* disable int and clear int status */
    ETH->ETH_ISR_IMR.d32 = 0xffff;

    /* enable Tx & Rx */
    ETH_CPU->ETH_ETHER_IO_CMD.b.te = 1;
    ETH_CPU->ETH_ETHER_IO_CMD.b.re = 1;

    if (ETH_InitStruct->ETH_Mode == ETH_LOOPBACK_T2R_MODE)
    {
        /* disable auto-polling */
        ETH->ETH_MIIAR.b.disable_auto_polling = 1;
        ETH->ETH_MSR.b.force_spd_mode = 1;
        ETH->ETH_MSR.b.force_spd = ETH_InitStruct->ETH_ForceSpeed;
        ETH->ETH_MSR.b.force_link = 1;
        ETH->ETH_MSR.b.force_full_dup = 1;
    }
    else
    {
        /* enable auto-polling */
        ETH->ETH_MIIAR.b.disable_auto_polling = 0;

        if (ETH_PHY_OPS.ETH_WaitPHYLinkUp != NULL)
        {
            ETH_PHY_OPS.ETH_WaitPHYLinkUp();
        }

        /* Wait MAC's link is up */
        i = 0;
        do
        {
            if ((ETH->ETH_MSR.b.link_status) == ETH_LINK_UP)
            {
                break;
            }
            else
            {
                i++;
                if (i > ETH_TIMEOUT_CNT_MAX)
                {
#ifdef DEBUG_ETH
                    ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_ERROR, "ETH_Init: Wait MAC's link up timeout!!", 0);
#endif
                    return ETH_STATUS_ERROR;
                }
            }
        }
        while (1);
    }

    /* Get MAC's link info. */
#ifdef DEBUG_ETH
    ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_INFO, "ETH_Init: get mac link info = 0x%x", 1,
                   ETH->ETH_MSR.d32);
    ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_INFO, "[ETH_Init] link speed %d duplex mode %d", 2,
                   ETH_GetLinkSpeed(), ETH_GetDuplexMode());
#endif

    if (ETH_InitStruct->ETH_Mode != ETH_LOOPBACK_T2R_MODE)
    {
        if (ETH_PHY_OPS.ETH_PHYConfig != NULL)
        {
            ETH_PHY_OPS.ETH_PHYConfig(ETH_InitStruct);
        }
    }

    return ETH_STATUS_OK;
}

/**
  * \brief  Initialize ETH_InitTypeDef.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \return None.
  */
void ETH_StructInit(ETH_InitTypeDef *ETH_InitStruct)
{
    ETH_InitStruct->ETH_InterFrameGapTime = ETH_IFG_3;
    ETH_InitStruct->ETH_Mode = ETH_NORMAL_MODE;
    ETH_InitStruct->ETH_PhyMode = ETH_PHY_NORMAL_MODE;
    ETH_InitStruct->ETH_PhyRxSetupTime = ETH_PHY_DEFAULT_RX_SETUP_TIME;
    ETH_InitStruct->ETH_PhyTxSetupTime = ETH_PHY_DEFAULT_TX_SETUP_TIME;
    ETH_InitStruct->ETH_ReceiveConfig = ETH_RX_CONFIG_AAP | ETH_RX_CONFIG_APM | ETH_RX_CONFIG_AM |
                                        ETH_RX_CONFIG_AB;
    ETH_InitStruct->ETH_RxJumboConfig = ETH_RX_JUMBO_ENABLE;
    ETH_InitStruct->ETH_RxRefclkPhase = ETH_RX_SAMPLE_ON_FALLING_EDGE;
    ETH_InitStruct->ETH_TxRefclkPhase = ETH_TX_CHANGE_ON_FALLING_EDGE;
    ETH_InitStruct->ETH_RxThreshold = ETH_RX_THRESHOLD_256B;
    ETH_InitStruct->ETH_TxThreshold = ETH_TX_THRESHOLD_256B;
    ETH_InitStruct->ETH_RxTriggerLevel = ETH_RX_TRIGGER_LEVEL_1_PKT;
    ETH_InitStruct->ETH_TxTriggerLevel = ETH_TX_TRIGGER_LEVEL_1_PKT;
    ETH_InitStruct->ETH_RxDescNum = 8;
    ETH_InitStruct->ETH_TxDescNum = 8;
    ETH_InitStruct->ETH_RxDesc = NULL;
    ETH_InitStruct->ETH_TxDesc = NULL;
    ETH_InitStruct->ETH_RxPktBuf = NULL;
    ETH_InitStruct->ETH_TxPktBuf = NULL;
    ETH_InitStruct->ETH_RxDescCurrentNum = 0;
    ETH_InitStruct->ETH_TxDescCurrentNum = 0;
    ETH_InitStruct->ETH_RxFrameStartDescIdx = 0;
    ETH_InitStruct->ETH_RxFrameLen = 0;
    ETH_InitStruct->ETH_RxSegmentCount = 0;
    ETH_InitStruct->ETH_TxAllocBufSize = 1600;
    ETH_InitStruct->ETH_RxAllocBufSize = 1600;
    ETH_InitStruct->ETH_TxBufSize = 1524;
    ETH_InitStruct->ETH_RxBufSize = 1524;
    ETH_InitStruct->ETH_ForceSpeed = ETH_LOOPBACK_FORCE_SPEED_100M;

    return;
}

/**
  * \brief  Get ethernet link speed.
  * \return ETHLinkSpeed_Typedef: ethernet link speed. \ref ETHLinkSpeed_Typedef.
  */
ETHLinkSpeed_Typedef ETH_GetLinkSpeed(void)
{
    return (ETHLinkSpeed_Typedef)(ETH->ETH_MSR.b.link_speed);
}

/**
  * \brief  Get ethernet duplex mode.
  * \return ETHDuplexMode_Typedef: ethernet duplex mode. \ref ETHDuplexMode_Typedef.
  */
ETHDuplexMode_Typedef ETH_GetDuplexMode(void)
{
    return (ETHDuplexMode_Typedef)(ETH->ETH_MSR.b.full_dupreg);
}

/**
  * \brief  Enable ethernet RX.
  * \param  None.
  * \return None.
  */
void ETH_EnableRx(void)
{
    /* enable Rx ring1 */
    ETH_CPU->ETH_IO_CMD1.b.rxring1 = 1;
}

/**
  * \brief  Close the clock of ETH and disable intterrupt.
  * \param  None.
  * \return None.
  */
void ETH_DeInit(void)
{
    ETH_ClockDeInit();
}

/**
  * \brief  To send frame.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  FrameLength: The length of data to be sent.
  * \return ETHSendFrameStatus_Typedef: The result of the operation. \ref ETHSendFrameStatus_Typedef.
  */
ETHSendFrameStatus_Typedef ETH_SendFrame(ETH_InitTypeDef *ETH_InitStruct, uint32_t FrameLength)
{
    if ((ETH_InitStruct == NULL) || (FrameLength == 0))
    {
#ifdef DEBUG_ETH
        ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_ERROR, "ETH_SendFrame: Invalid parameter!!", 0);
#endif
        return ETH_TX_INVALID_PARAMETER;
    }

    if (FrameLength > ETH_MAX_TX_PACKET_SIZE)
    {
        return ETH_TX_PACKET_SIZE_OVERFLOW;
    }

    uint8_t tx_desc_current_num = ETH_InitStruct->ETH_TxDescCurrentNum;
    uint32_t *p = (uint32_t *)(ETH_InitStruct->ETH_TxDesc[tx_desc_current_num].addr);
    uint32_t bufcount = 0, i = 0, size = 0;

#ifdef DEBUG_ETH
    ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_INFO, "ETH_SendFrame: tx_idx=%d frame_length = %d", 2,
                   tx_desc_current_num, FrameLength);
#endif

    /* Get the number of needed Tx buffers for the current frame */
    if (FrameLength > ETH_InitStruct->ETH_TxBufSize)
    {
        bufcount = FrameLength / ETH_InitStruct->ETH_TxBufSize;
        if (FrameLength % ETH_InitStruct->ETH_TxBufSize)
        {
            bufcount++;
        }
    }
    else
    {
        bufcount = 1U;
    }

    if (bufcount == 1U)
    {
        /* check if current Tx descriptor is available */
        if (!ETH_IsTxDescAvailable(ETH_InitStruct->ETH_TxDesc[tx_desc_current_num].dw1))
        {
#ifdef DEBUG_ETH
            ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_WARN, "ETH_SendFrame: Tx descriptor ring is full cur %d!! ", 1,
                           tx_desc_current_num);
#endif
            return ETH_TX_DSC_IS_FULL;
        }

        ETH_InitStruct->ETH_TxDesc[tx_desc_current_num].dw1 &= ETH_TX_DESC_EOR;
        ETH_InitStruct->ETH_TxDesc[tx_desc_current_num].dw1 |= (ETH_TX_DESC_OWN | ETH_TX_DESC_FS |
                                                                ETH_TX_DESC_LS | ETH_TX_DESC_CRC | FrameLength);
        if (tx_desc_current_num == ((ETH_InitStruct->ETH_TxDescNum) - 1))
        {
            ETH_InitStruct->ETH_TxDescCurrentNum = 0;
        }
        else
        {
            ETH_InitStruct->ETH_TxDescCurrentNum++;
        }
    }
    else
    {
        for (i = 0U; i < bufcount; i++)
        {
#ifdef DEBUG_ETH
            ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_INFO, "ETH_SendFrame: multi tx desc tx_idx=%d", 1,
                           tx_desc_current_num);
#endif
            /* check if current Tx descriptor is available */
            if (!ETH_IsTxDescAvailable(ETH_InitStruct->ETH_TxDesc[tx_desc_current_num].dw1))
            {
#ifdef DEBUG_ETH
                ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_WARN, "ETH_SendFrame: Tx descriptor ring is full cur %d!! ", 1,
                               tx_desc_current_num);
#endif
                return ETH_TX_DSC_IS_FULL;
            }

            ETH_InitStruct->ETH_TxDesc[tx_desc_current_num].dw1 &= ETH_TX_DESC_EOR;
            /* Clear FIRST and LAST segment bits */
            ETH_InitStruct->ETH_TxDesc[tx_desc_current_num].dw1 &= ~(ETH_TX_DESC_FS | ETH_TX_DESC_LS);

            if (i == 0U)
            {
                /* Setting the first segment bit */
                ETH_InitStruct->ETH_TxDesc[tx_desc_current_num].dw1 |= ETH_TX_DESC_FS;
            }

            if (i == (bufcount - 1U))
            {
                /* Setting the last segment bit */
                ETH_InitStruct->ETH_TxDesc[tx_desc_current_num].dw1 |= ETH_TX_DESC_LS;
                size = FrameLength - (bufcount - 1U) * ETH_InitStruct->ETH_TxBufSize;
                ETH_InitStruct->ETH_TxDesc[tx_desc_current_num].dw1 |= (size & 0x1ffff);
            }
            else
            {
                /* Program size */
                ETH_InitStruct->ETH_TxDesc[tx_desc_current_num].dw1 |= (ETH_InitStruct->ETH_TxBufSize &
                                                                        0x1ffff);
            }

            /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
            ETH_InitStruct->ETH_TxDesc[tx_desc_current_num].dw1 |= (ETH_TX_DESC_OWN | ETH_TX_DESC_CRC);

            /* point to next descriptor */
            if (tx_desc_current_num == ((ETH_InitStruct->ETH_TxDescNum) - 1))
            {
                ETH_InitStruct->ETH_TxDescCurrentNum = 0;
            }
            else
            {
                ETH_InitStruct->ETH_TxDescCurrentNum++;
            }
            tx_desc_current_num = ETH_InitStruct->ETH_TxDescCurrentNum;
        }
    }

    ETH_Enable1stPriorityTxDMAETH();

    return ETH_TX_FRAME_SUCCESS;
}

/**
  * \brief  Copy data to ethernet tx buffer.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  send_data: The address of data to be sent, must be continuous.
  * \param  send_len: The len of data to be sent.
  * \return ETHSendFrameStatus_Typedef: The result of the operation. \ref ETHSendFrameStatus_Typedef.
  */
ETHSendFrameStatus_Typedef ETH_CopyDataToTxBuffer(ETH_InitTypeDef *ETH_InitStruct,
                                                  uint8_t *send_data, uint32_t send_len)
{
    if ((ETH_InitStruct == NULL) || (send_data == NULL))
    {
        return ETH_TX_INVALID_PARAMETER;
    }

    if (send_len > ETH_MAX_TX_PACKET_SIZE)
    {
        return ETH_TX_PACKET_SIZE_OVERFLOW;
    }

    uint32_t buffer_offset = 0;
    uint32_t buffer_left_to_copy = send_len;
    uint8_t tx_serach_idx = ETH_InitStruct->ETH_TxDescCurrentNum;
    uint8_t *buffer = (uint8_t *)(ETH_InitStruct->ETH_TxDesc[tx_serach_idx].addr);

    while (buffer_left_to_copy >= ETH_InitStruct->ETH_TxBufSize)
    {
        if (ETH_IsTxDescAvailable(ETH_InitStruct->ETH_TxDesc[tx_serach_idx].dw1))
        {
            memcpy(buffer, (send_data + buffer_offset), ETH_InitStruct->ETH_TxBufSize);
            buffer_offset += ETH_InitStruct->ETH_TxBufSize;
            buffer_left_to_copy -= ETH_InitStruct->ETH_TxBufSize;
            /* Point to next descriptor */
            if (tx_serach_idx == ((ETH_InitStruct->ETH_TxDescNum) - 1))
            {
                tx_serach_idx = 0;
            }
            else
            {
                tx_serach_idx++;
            }
            buffer = (uint8_t *)(ETH_InitStruct->ETH_TxDesc[tx_serach_idx].addr);
        }
        else
        {
#ifdef DEBUG_ETH
            ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_WARN,
                           "[ETH_CopyDataToTxBuffer] Tx descriptor ring is full %d !!", 1, tx_serach_idx);
#endif
            return ETH_TX_DSC_IS_FULL;
        }
    }

    if (buffer_left_to_copy != 0)
    {
        if (ETH_IsTxDescAvailable(ETH_InitStruct->ETH_TxDesc[tx_serach_idx].dw1))
        {
            memcpy(buffer, (send_data + buffer_offset), buffer_left_to_copy);
        }
        else
        {
#ifdef DEBUG_ETH
            ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_WARN,
                           "[ETH_CopyDataToTxBuffer] Tx descriptor ring is full %d !!", 1, tx_serach_idx);
#endif
            return ETH_TX_DSC_IS_FULL;
        }
    }

    return ETH_TX_COPY_DATA_SUCCESS;
}

/**
  * \brief  Enable 1st Priority DMA-Ethernet Transmit.
  * \return none.
  */
void ETH_Enable1stPriorityTxDMAETH(void)
{
    ETH_CPU->ETH_ETHER_IO_CMD.b.tx_fn1st = 1;
}

/**
  * \brief  Check tx desc is available.
  * \return true: tx desc is available.
  *         false: tx desc is not available.
  */
bool ETH_IsTxDescAvailable(uint32_t dw1)
{
    return !(dw1 & ETH_TX_DESC_OWN);
}

/**
  * \brief  Check rx desc is available.
  * \return true: rx desc is available.
  *         false: rx desc is not available.
  */
bool ETH_IsRxDescAvailable(uint32_t dw1)
{
    return !(dw1 & ETH_RX_DESC_OWN);
}

/**
  * \brief  Get TX OK count.
  * \return TX OK count, rolls over automatically.
  */
uint16_t ETH_GetTxOkCnt(void)
{
    return ETH->ETH_TXOKCNT.d16;
}

/**
  * \brief  Clear TX OK count.
  * \return none.
  */
void ETH_ClearTxOkCnt(void)
{
    ETH->ETH_TXOKCNT.d16 = 1;
}

/**
  * \brief  Get RX OK count.
  * \return RX OK count, rolls over automatically.
  */
uint16_t ETH_GetRxOkCnt(void)
{
    return ETH->ETH_RXOKCNT.d16;
}

/**
  * \brief  Clear RX OK count.
  * \return none.
  */
void ETH_ClearRxOkCnt(void)
{
    ETH->ETH_RXOKCNT.d16 = 1;
}

/**
  * \brief  Get TX error count.
  * \return TX error count, rolls over automatically.
  */
uint16_t ETH_GetTxErrCnt(void)
{
    return ETH->ETH_TXERR.d16;
}

/**
  * \brief  Clear TX error count.
  * \return none.
  */
void ETH_ClearTxErrCnt(void)
{
    ETH->ETH_TXERR.d16 = 1;
}

/**
  * \brief  Get RX error count.
  * \return RX error count, rolls over automatically.
  */
uint16_t ETH_GetRxErrCnt(void)
{
    return ETH->ETH_RXERR.d16;
}

/**
  * \brief  Clear RX error count.
  * \return none.
  */
void ETH_ClearRxErrCnt(void)
{
    ETH->ETH_RXERR.d16 = 1;
}

/**
  * \brief  Get missed packets count.
  * \return Missed packets count, rolls over automatically.
  */
uint16_t ETH_GetMissedPacketCnt(void)
{
    return ETH->ETH_MISSPKT.d16;
}

/**
  * \brief  Clear missed packets count.
  * \return none.
  */
void ETH_ClearMissedPacketCnt(void)
{
    ETH->ETH_MISSPKT.d16 = 1;
}

/**
  * \brief  Config ethernet interrupt.
  * \param  ETH_INT: The interrupt of ethernet. \ref ETHINT_TypeDef.
  * \param  newState: Disable or enable ethernet interrupt.
  * \return none.
  */
void ETH_INTConfig(ETHINT_TypeDef ETH_INT, FunctionalState newState)
{
    /* Check the parameters */
    assert_param(IS_ETH_INT(ETH_INT));

    ETH_ISR_IMR_TypeDef eth_reg_0x3c = {.d32 = ETH->ETH_ISR_IMR.d32};

    eth_reg_0x3c.d32 &= 0xffff0000;

    if (newState == ENABLE)
    {
        eth_reg_0x3c.d32 |= (ETH_INT << 16);
    }
    else
    {
        eth_reg_0x3c.d32 &= (~(ETH_INT << 16));
    }

    ETH->ETH_ISR_IMR.d32 = eth_reg_0x3c.d32;
}

/**
  * \brief  Check ethernet interrupt is enable.
  * \param  ETH_INT: The interrupt of ethernet. \ref ETHINT_TypeDef.
  * \return SET: The interrupt is enable.
  *         RESET: The interrupt is disable.
  */
ITStatus ETH_IsINTEnable(ETHINT_TypeDef ETH_INT)
{
    ITStatus bit_status = RESET;
    /* Check the parameters */
    assert_param(IS_ETH_INT(ETH_INT));

    if ((ETH->ETH_ISR_IMR.d32 & (ETH_INT << 16)) != (uint32_t)RESET)
    {
        bit_status = SET;
    }
    /* Return the status */
    return  bit_status;
}

/**
  * \brief  Get ethernet interrupt status.
  * \param  ETH_INT: The interrupt of ethernet. \ref ETHINT_TypeDef.
  * \return SET: The interrupt is pending.
  *         RESET: The interrupt is not pending.
  */
ITStatus ETH_GetINTStatus(ETHINT_TypeDef ETH_INT)
{
    ITStatus bit_status = RESET;
    /* Check the parameters */
    assert_param(IS_ETH_INT(ETH_INT));

    if ((ETH->ETH_ISR_IMR.d32 & ETH_INT) != (uint32_t)RESET)
    {
        bit_status = SET;
    }
    /* Return the status */
    return  bit_status;
}

/**
  * \brief  Clear ethernet interrupt status.
  * \param  ETH_INT: The interrupt of ethernet. \ref ETHINT_TypeDef.
  * \return none
  */
void ETH_ClearINTStatus(ETHINT_TypeDef ETH_INT)
{
    /* Check the parameters */
    assert_param(IS_ETH_INT_STATUS(status));
    ETH_ISR_IMR_TypeDef eth_reg_0x3c = {.d32 = ETH->ETH_ISR_IMR.d32};

    eth_reg_0x3c.d32 &= 0xffff0000;
    eth_reg_0x3c.d32 |= ETH_INT;

    ETH->ETH_ISR_IMR.d32 = eth_reg_0x3c.d32;
}

/**
  * \brief  Get ethernet link status.
  * \return ETHLinkStatus_Typedef: ethernet link status. \ref ETHLinkStatus_Typedef.
  */
ETHLinkStatus_Typedef ETH_GetLinkStatus(void)
{
    if (ETH->ETH_MSR.b.link_status)
    {
        return ETH_LINK_DOWN;
    }
    else
    {
        return ETH_LINK_UP;
    }
}

/**
  * \brief  Set rx desc back to ethernet.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  rx_desc_index: Rx desc index.
  * \param  buffer_size: Rx desc buffer size.
  * \return none.
  */
void ETH_SetRxDescBackToETH(ETH_InitTypeDef *ETH_InitStruct, uint8_t rx_desc_index,
                            uint16_t buffer_size)
{
    ETH_InitStruct->ETH_RxDesc[rx_desc_index].dw1 &= BIT30;
    ETH_InitStruct->ETH_RxDesc[rx_desc_index].dw1 |= (BIT31 | buffer_size);
    ETH_InitStruct->ETH_RxDesc[rx_desc_index].dw2 = 0;
    ETH_InitStruct->ETH_RxDesc[rx_desc_index].dw3 = 0;
}

/**
  * \brief  To receive frame.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \return ETHReceiveFrameStatus_Typedef: receive frame result. \ref ETHReceiveFrameStatus_Typedef.
  */
ETHReceiveFrameStatus_Typedef ETH_ReceiveFrame(ETH_InitTypeDef *ETH_InitStruct)
{
    if (ETH_InitStruct == NULL)
    {
#ifdef DEBUG_ETH
        ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_ERROR, "[ETH_ReceiveFrame] Invalid parameter!!", 0);
#endif
        return ETH_RX_INVALID_PARAMETER;
    }

    uint8_t rx_desc_current_num = ETH_InitStruct->ETH_RxDescCurrentNum;
    uint8_t *buffer;
    bool is_rx_data_received = false;

#ifdef DEBUG_ETH
    ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_INFO, "ETH_ReceiveFrame: rx_idx=%d\n", 1, rx_desc_current_num);
#endif

    while (ETH_IsRxDescAvailable(ETH_InitStruct->ETH_RxDesc[rx_desc_current_num].dw1))
    {
        is_rx_data_received = true;
        /* Check if last segment */
        if (((ETH_InitStruct->ETH_RxDesc[rx_desc_current_num].dw1 & ETH_RX_DESC_LS) != (uint32_t)RESET))
        {
            /* increment segment count */
            ETH_InitStruct->ETH_RxSegmentCount++;
#ifdef DEBUG_ETH
            ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_INFO,
                           "[ETH_ReceiveFrame] end frame, seg_count=%d,frame_len=%d", 2, ETH_InitStruct->ETH_RxSegmentCount,
                           (ETH_InitStruct->ETH_RxDesc[rx_desc_current_num].dw1) & 0xFFF);
#endif
            /* Check if last segment is first segment: one segment contains the frame */
            if (ETH_InitStruct->ETH_RxSegmentCount == 1U)
            {
                ETH_InitStruct->ETH_RxFrameStartDescIdx = rx_desc_current_num;
                ETH_InitStruct->ETH_RxFrameLen = 0;
            }

            /* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
            if (((ETH_InitStruct->ETH_RxDesc[rx_desc_current_num].dw1) & 0xFFF) >= 4)
            {
                ETH_InitStruct->ETH_RxFrameLen += ((ETH_InitStruct->ETH_RxDesc[rx_desc_current_num].dw1) &
                                                   0xFFF) - 4;
            }
            else
            {
#ifdef DEBUG_ETH
                ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_ERROR, "[ETH_ReceiveFrame] rx data len error", 0);
#endif
                break;
            }

            /* point to next descriptor */
            if (rx_desc_current_num != ((ETH_InitStruct->ETH_RxDescNum) - 1))
            {
                ETH_InitStruct->ETH_RxDescCurrentNum++;
            }
            else
            {
                ETH_InitStruct->ETH_RxDescCurrentNum = 0;
            }

            /* Receive frame success */
            return ETH_RX_FRAME_SUCCESS;
        }
        /* Check if first segment */
        else if ((ETH_InitStruct->ETH_RxDesc[rx_desc_current_num].dw1 & ETH_RX_DESC_FS) !=
                 (uint32_t)RESET)
        {
            ETH_InitStruct->ETH_RxFrameStartDescIdx = rx_desc_current_num;
            ETH_InitStruct->ETH_RxSegmentCount = 1U;

            ETH_InitStruct->ETH_RxFrameLen = ((ETH_InitStruct->ETH_RxDesc[rx_desc_current_num].dw1) &
                                              0xFFF);
#ifdef DEBUG_ETH
            ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_INFO,
                           "ETH_ReceiveFrame: start frame frame_len=%d,rx_frame_len=%d", 2,
                           (ETH_InitStruct->ETH_RxDesc[rx_desc_current_num].dw1) & 0xFFF, ETH_InitStruct->ETH_RxFrameLen);
#endif
        }
        /* Check if intermediate segment */
        else
        {
#ifdef DEBUG_ETH
            ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_INFO,
                           "ETH_ReceiveFrame: middle frame frame_len=%d,rx_frame_len=%d", 2,
                           (ETH_InitStruct->ETH_RxDesc[rx_desc_current_num].dw1) & 0xFFF, ETH_InitStruct->ETH_RxFrameLen);
#endif
            ETH_InitStruct->ETH_RxSegmentCount++;

            ETH_InitStruct->ETH_RxFrameLen += ((ETH_InitStruct->ETH_RxDesc[rx_desc_current_num].dw1) &
                                               0xFFF);
        }

        if (rx_desc_current_num != ((ETH_InitStruct->ETH_RxDescNum) - 1))
        {
            ETH_InitStruct->ETH_RxDescCurrentNum++;
        }
        else
        {
            ETH_InitStruct->ETH_RxDescCurrentNum = 0;
        }

        rx_desc_current_num = ETH_InitStruct->ETH_RxDescCurrentNum;
    }

    if (is_rx_data_received == false)
    {
        return ETH_RX_NO_DATA;
    }

    return ETH_RX_FRAME_ERROR;
}

/**
  * \brief  Copy ethernet rx buffer to data.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  recv_data: The address of data to be received, must be continuous.
  * \param  recv_len: The len of data to be received.
  * \param  data_size: The len of recv_data, must be large enough to receive data.
  * \return ETHReceiveFrameStatus_Typedef: copy frame result. \ref ETHReceiveFrameStatus_Typedef.
  */
ETHReceiveFrameStatus_Typedef ETH_CopyRxBufferToData(ETH_InitTypeDef *ETH_InitStruct,
                                                     uint8_t *recv_data, uint32_t *recv_len, uint32_t data_size)
{
    if ((ETH_InitStruct == NULL) || (recv_data == NULL) || (recv_len == NULL))
    {
        return ETH_RX_INVALID_PARAMETER;
    }

    uint8_t rx_serach_idx = ETH_InitStruct->ETH_RxFrameStartDescIdx;

    *recv_len = ETH_InitStruct->ETH_RxFrameLen;

    if (data_size < ETH_InitStruct->ETH_RxFrameLen)
    {
        return ETH_RX_COPY_DATA_LEN_ERROR;
    }

    if (ETH_InitStruct->ETH_RxSegmentCount == 1)
    {
        /* The data of first desc has two bytes offset */
        memcpy(recv_data, (uint8_t *)((ETH_InitStruct->ETH_RxDesc[rx_serach_idx].addr) +
                                      ETH_RX_PACKET_FIRST_DESC_DATA_OFFSET), ETH_InitStruct->ETH_RxFrameLen);
    }
    else
    {
        uint32_t buffer_offset = 0;
        uint32_t buffer_left_to_copy = ETH_InitStruct->ETH_RxFrameLen;

        for (uint32_t i = 0; i < ETH_InitStruct->ETH_RxSegmentCount; i++)
        {
            if (i == 0)
            {
                /* The data of first desc has two bytes offset */
                memcpy(recv_data, (uint8_t *)((ETH_InitStruct->ETH_RxDesc[rx_serach_idx].addr) +
                                              ETH_RX_PACKET_FIRST_DESC_DATA_OFFSET),
                       ETH_InitStruct->ETH_RxBufSize - ETH_RX_PACKET_FIRST_DESC_DATA_OFFSET);
                buffer_offset = ETH_InitStruct->ETH_RxBufSize - 2;
                buffer_left_to_copy -= buffer_offset;
            }
            else
            {
                if (buffer_left_to_copy > ETH_InitStruct->ETH_RxBufSize)
                {
                    memcpy((recv_data + buffer_offset), (uint8_t *)((ETH_InitStruct->ETH_RxDesc[rx_serach_idx].addr)),
                           ETH_InitStruct->ETH_RxBufSize);
                    buffer_offset += ETH_InitStruct->ETH_RxBufSize;
                    buffer_left_to_copy -= ETH_InitStruct->ETH_RxBufSize;
                }
                else
                {
                    memcpy((recv_data + buffer_offset), (uint8_t *)((ETH_InitStruct->ETH_RxDesc[rx_serach_idx].addr)),
                           buffer_left_to_copy);
                }
            }

            /* Point to next descriptor */
            if (rx_serach_idx == ((ETH_InitStruct->ETH_RxDescNum) - 1))
            {
                rx_serach_idx = 0;
            }
            else
            {
                rx_serach_idx++;
            }
        }
    }

    rx_serach_idx = ETH_InitStruct->ETH_RxFrameStartDescIdx;
    for (uint32_t i = 0; i < ETH_InitStruct->ETH_RxSegmentCount; i++)
    {
        ETH_SetRxDescBackToETH(ETH_InitStruct, rx_serach_idx, ETH_InitStruct->ETH_RxBufSize);
        if (rx_serach_idx == ((ETH_InitStruct->ETH_RxDescNum) - 1))
        {
            rx_serach_idx = 0;
        }
        else
        {
            rx_serach_idx++;
        }
    }

    return ETH_RX_COPY_DATA_SUCCESS;
}

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/
