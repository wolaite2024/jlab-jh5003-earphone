/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* \file     rtl_phy_rtl8201fr.c
* \brief    This file provides all the ETHERNET firmware functions.
* \details
* \author   colin
* \date     2024-07-10
* \version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "rtl_ethernet.h"
#include "rtl_phy_rtl8201fr.h"
#include "trace.h"
#include "platform_utils.h"

#define PHY_TIMEOUT_CNT_MAX             1000000
//6ns,8ns,10ns,12ns,14ns,16ns,18ns
static uint8_t phy_rtl8201fr_tx_setup_time[ETH_PHY_TX_SETUP_TIME_NUM] = {0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0};
//8ns,10ns,12ns,14ns,16ns,18ns
static uint8_t phy_rtl8201fr_rx_setup_time[ETH_PHY_RX_SETUP_TIME_NUM] = {0x8, 0x9, 0x6, 0x7, 0x4, 0x5};

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
PHYRTL8201FRLinkSpeed_Typedef PHY_RTL8201FRGetLinkSpeed(void)
{
    PHY_RTL8201FR_PAGE0_REG0_TypeDef page0_reg0 = {.d16 = 0};

    page0_reg0.d16 = ETH_ReadPHYRegister(PHY_RTL8201FR_ADDR, PHY_RTL8201FR_PAGE0_REG0_CTL_ADDR);

    return (PHYRTL8201FRLinkSpeed_Typedef)(page0_reg0.b.speed_selection);
}

PHYRTL8201FRDuplexMode_Typedef PHY_RTL8201FRGetDuplexMode(void)
{
    PHY_RTL8201FR_PAGE0_REG0_TypeDef page0_reg0 = {.d16 = 0};

    page0_reg0.d16 = ETH_ReadPHYRegister(PHY_RTL8201FR_ADDR, PHY_RTL8201FR_PAGE0_REG0_CTL_ADDR);

    return (PHYRTL8201FRDuplexMode_Typedef)(page0_reg0.b.duplex_mode);
}

bool PHY_RTL8201FRReset(void)
{
    uint32_t i = 0;
    PHY_RTL8201FR_PAGE0_REG0_TypeDef page0_reg0 = {.d16 = 0};

    page0_reg0.b.reset = 1;

    /* reset PHY */
    ETH_WritePHYRegister(PHY_RTL8201FR_ADDR, PHY_RTL8201FR_PAGE0_REG0_CTL_ADDR, page0_reg0.d16);
    platform_delay_ms(10);

    while (1)
    {
        page0_reg0.d16 = ETH_ReadPHYRegister(PHY_RTL8201FR_ADDR, PHY_RTL8201FR_PAGE0_REG0_CTL_ADDR);

        if (!(page0_reg0.b.reset))
        {
#ifdef DEBUG_ETH
            ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_INFO, "PHY_RTL8201FRReset: reset success page0_reg0 0x%x", 1,
                           page0_reg0.d16);
#endif
            return true;
        }
        else
        {
            i++;
            if (i > PHY_TIMEOUT_CNT_MAX)
            {
#ifdef DEBUG_ETH
                ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_ERROR, "PHY_RTL8201FRReset: phy reset failed 0x%x!!", 1,
                               page0_reg0.d16);
#endif
                return false;
            }
        }
    }
    return false;
}

bool PHY_RTL8201FRWaitLinkUp(void)
{
    uint32_t i = 0;
    PHY_RTL8201FR_PAGE0_REG1_TypeDef page0_reg1 = {.d16 = 0};
    /* Wait PHY's link is up */
    do
    {
        /* 1st read */
        page0_reg1.d16 = ETH_ReadPHYRegister(PHY_RTL8201FR_ADDR, PHY_RTL8201FR_PAGE0_REG1_STATUS_ADDR);
        /* 2nd read */
        page0_reg1.d16 = ETH_ReadPHYRegister(PHY_RTL8201FR_ADDR, PHY_RTL8201FR_PAGE0_REG1_STATUS_ADDR);

        if (page0_reg1.b.link_status == RTL8201FR_LINK_UP)
        {
#ifdef DEBUG_ETH
            ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_INFO, "PHY_RTL8201FRWaitLinkUp: link up page0_reg1 0x%x", 1,
                           page0_reg1.d16);
            ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_INFO, "PHY SPEED %d PHY DUPLEX MODE %d", 2,
                           PHY_RTL8201FRGetLinkSpeed(), PHY_RTL8201FRGetDuplexMode());
#endif
            return true;
        }
        else
        {
            i++;
            if (i > PHY_TIMEOUT_CNT_MAX)
            {
#ifdef DEBUG_ETH
                ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_ERROR,
                               "PHY_RTL8201FRWaitLinkUp: Wait PHY's link up timeout!! page0_reg1 0x%x", 1, page0_reg1.d16);
#endif
                return false;
            }
        }
    }
    while (1);

    return false;
}

bool PHY_RTL8201FRLoopback(ETH_InitTypeDef *ETH_InitStruct)
{
    PHY_RTL8201FR_PAGE0_REG0_TypeDef page0_reg0 = {.d16 = 0};

    page0_reg0.d16 = ETH_ReadPHYRegister(PHY_RTL8201FR_ADDR, PHY_RTL8201FR_PAGE0_REG0_CTL_ADDR);

    page0_reg0.b.loopback = 1;
    page0_reg0.b.auto_negotiation_enable = 0;
    if (ETH_InitStruct->ETH_ForceSpeed == ETH_LOOPBACK_FORCE_SPEED_100M)
    {
        page0_reg0.b.speed_selection = 1;
    }
    else
    {
        page0_reg0.b.speed_selection = 0;
    }

    page0_reg0.b.duplex_mode = 1;   //Must set full duplex

    ETH_WritePHYRegister(PHY_RTL8201FR_ADDR, PHY_RTL8201FR_PAGE0_REG0_CTL_ADDR, page0_reg0.d16);

    return true;
}

bool PHY_RTL8201FRConfig(ETH_InitTypeDef *ETH_InitStruct)
{
    PHY_RTL8201FR_PAGE7_REG16_TypeDef page7_reg16 = {.d16 = 0};
    PHY_RTL8201FR_PAGE0_REG31_TypeDef page0_reg31 = {.d16 = 0};

    /* set tx/rx timing of PHY RTL8201FR */
    page0_reg31.b.page_select = 0x7;
    ETH_WritePHYRegister(PHY_RTL8201FR_ADDR, PHY_RTL8201FR_PAGE0_REG31_ADDR, page0_reg31.d16);
    page7_reg16.d16 = ETH_ReadPHYRegister(PHY_RTL8201FR_ADDR, PHY_RTL8201FR_PAGE7_REG16_ADDR);

    page7_reg16.b.rg_rmii_clkdir = 1;
    page7_reg16.b.rg_rmii_rx_offset = phy_rtl8201fr_rx_setup_time[ETH_InitStruct->ETH_PhyRxSetupTime];
    page7_reg16.b.rg_rmii_tx_offset = phy_rtl8201fr_tx_setup_time[ETH_InitStruct->ETH_PhyTxSetupTime];

    ETH_WritePHYRegister(PHY_RTL8201FR_ADDR, PHY_RTL8201FR_PAGE7_REG16_ADDR, page7_reg16.d16);

#ifdef DEBUG_ETH
    page7_reg16.d16 = ETH_ReadPHYRegister(PHY_RTL8201FR_ADDR, PHY_RTL8201FR_PAGE7_REG16_ADDR);
    ETH_DBG_BUFFER(ETH_DBG_MODULE, LEVEL_INFO, "PHY_RTL8201FRConfig: page7_reg16 = 0x%x", 1,
                   page7_reg16.d16);
#endif

    page0_reg31.b.page_select = 0;
    ETH_WritePHYRegister(PHY_RTL8201FR_ADDR, PHY_RTL8201FR_PAGE0_REG31_ADDR, page0_reg31.d16);

    return true;
}

/**
  * \brief  Initialize the PHY API.
  * \return none.
  */
void PHY_RTL8201FRRegisterOps(void)
{
    ETHPHYOps_TypeDef phy_ops;

    phy_ops.ETH_ResetPHY = PHY_RTL8201FRReset;
    phy_ops.ETH_WaitPHYLinkUp = PHY_RTL8201FRWaitLinkUp;
    phy_ops.ETH_PHYConfig = PHY_RTL8201FRConfig;
    phy_ops.ETH_PHYLoopback = PHY_RTL8201FRLoopback;

    ETH_PHYOpsInit(&phy_ops);
}
/******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/
