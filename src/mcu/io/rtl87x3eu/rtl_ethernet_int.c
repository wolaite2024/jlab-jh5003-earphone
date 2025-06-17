/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl_ethernet_int.c
* @brief    This file provides all the ethernet firmware internal functions.
* @details
* @author
* @date     2024-07-01
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl_ethernet.h"
#include "rtl876x_rcc.h"
#include "pcc_reg.h"

#if (ETH_SUPPORT_CLOCK_CONFIG == 1)
/**
  * \brief  Config ethernet clock.
  * \param  clk_src: Select ethernet clock src. \ref ETHClkSrc_TypeDef.
  * \param  clk: Select ethernet clock. \ref ETHClk_TypeDef.
  * \param  div: Select ethernet clock div. \ref ETHClkDiv_TypeDef.
  * \return none.
  */
void ETH_ClkConfig(ETHClkSrc_TypeDef clk_src, ETHClk_TypeDef clk, ETHClkDiv_TypeDef div)
{
    assert_param(IS_ETH_CLK_SRC(clk_src));
    assert_param(IS_ETH_CLK(clk));
    assert_param(IS_ETH_CLK_DIV(div));

    if (clk == ETH_CLK_100M)
    {
        SYSBLKCTRL->u_214.BITS_214.GMAC_CLK_50M_CLK_SRC_EN = 0;
        if (div == ETH_CLK_DIV_1)
        {
            SYSBLKCTRL->u_214.BITS_214.GMAC_CLK_125M_DIV_EN = 0;
        }
        else
        {
            SYSBLKCTRL->u_214.BITS_214.GMAC_CLK_125M_DIV_EN = 1;
        }
        SYSBLKCTRL->u_214.BITS_214.GMAC_CLK_125M_DIV = div;
        SYSBLKCTRL->u_214.BITS_214.GMAC_CLK_125M_CLK_SEL = clk_src;
        SYSBLKCTRL->u_214.BITS_214.GMAC_CLK_125M_CLK_SRC_EN = 1;
    }
    else if (clk == ETH_CLK_50M)
    {
        SYSBLKCTRL->u_214.BITS_214.GMAC_CLK_125M_CLK_SRC_EN = 0;
        if (div == ETH_CLK_DIV_1)
        {
            SYSBLKCTRL->u_214.BITS_214.GMAC_CLK_50M_DIV_EN = 0;
        }
        else
        {
            SYSBLKCTRL->u_214.BITS_214.GMAC_CLK_50M_DIV_EN = 1;
        }
        SYSBLKCTRL->u_214.BITS_214.GMAC_CLK_50M_DIV = div;
        SYSBLKCTRL->u_214.BITS_214.GMAC_CLK_50M_CLK_SEL = clk_src;
        SYSBLKCTRL->u_214.BITS_214.GMAC_CLK_50M_CLK_SRC_EN = 1;
    }
}
#endif

void ETH_ClockDeInit(void)
{
    RCC_PeriphClockCmd(APBPeriph_GMAC, APBPeriph_GMAC_CLOCK, DISABLE);
#if (ETH_SUPPORT_CLOCK_CONFIG == 1)
    SYSBLKCTRL->u_214.BITS_214.GMAC_CLK_50M_CLK_SRC_EN = 0;
    SYSBLKCTRL->u_214.BITS_214.GMAC_CLK_50M_DIV_EN = 0;
    SYSBLKCTRL->u_214.BITS_214.GMAC_CLK_125M_CLK_SRC_EN = 0;
    SYSBLKCTRL->u_214.BITS_214.GMAC_CLK_125M_DIV_EN = 0;
#endif
}

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/

