/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl_phy_rtl8201fr.h
* \brief    The header file of the peripheral phy rtl8201fr driver.
* \details  This file provides all PHY firmware functions.
* \author   colin
* \date     2024-07-08
* \version  v1.0
* *********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef RTL_PHY_RTL8201FR_H_
#define RTL_PHY_RTL8201FR_H_

#ifdef  __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                         Constants
 *============================================================================*/

// Defines the PHY address
#define PHY_RTL8201FR_ADDR                    0x1

// Defines the address of PHY RTL8201FR page0 register 0
#define PHY_RTL8201FR_PAGE0_REG0_CTL_ADDR               0x0
// Defines the address of PHY RTL8201FR page0 register 1
#define PHY_RTL8201FR_PAGE0_REG1_STATUS_ADDR            0x1
// Defines the address of PHY RTL8201FR page0 register 2
#define PHY_RTL8201FR_PAGE0_REG2_ID_ADDR                0x2
// Defines the address of PHY RTL8201FR page0 register 3
#define PHY_RTL8201FR_PAGE0_REG3_ID_ADDR                0x3
// Defines the address of PHY RTL8201FR page0 register 31
#define PHY_RTL8201FR_PAGE0_REG31_ADDR                  0x1F

// Defines the address of PHY RTL8201FR page7 register 16
#define PHY_RTL8201FR_PAGE7_REG16_ADDR                  0x10

// The default value when reading page0 register 2
#define PHY_RTL8201FR_PAGE0_REG2_ID       0x001c

// The default value when reading page0 register 3
#define PHY_RTL8201FR_PAGE0_REG3_ID       0xc816

/// Defines the bit definition of PHY RTL8201FR register 0
#define ETH_PHY_SPEED_MSB               BIT6
#define ETH_PHY_DUPLEX_MODE             BIT8
#define ETH_PHY_RESTART_NWAY            BIT9
#define ETH_PHY_NWAY_EN                 BIT12
#define ETH_PHY_SPEED_LSB               BIT13
#define ETH_PHY_SW_RESET                BIT15

/// Defines the bit definition of PHY RTL8201FR register 1
#define ETH_PHY_LINK_STATUS             BIT2

typedef enum
{
    RTL8201FR_LINK_DOWN = 0,
    RTL8201FR_LINK_UP = 1
} PHYRTL8201FRLinkStatus_Typedef;

typedef enum
{
    RTL8201FR_LINK_SPEED_10M = 0,
    RTL8201FR_LINK_SPEED_100M = 1
} PHYRTL8201FRLinkSpeed_Typedef;

typedef enum
{
    RTL8201FR_HALF_DUPLEX = 0,
    RTL8201FR_FULL_DUPLEX = 1
} PHYRTL8201FRDuplexMode_Typedef;

/*============================================================================*
 *                          PHY Registers and Field Descriptions
 *============================================================================*/
/* PAGE0 REG0 basic mode control register
    5:0     R/W    reserved_0                    6'h0
    6       R/W    speed_selection_1             1'h0
    7       R/W    collision_test                1'h0
    8       R/W    duplex_mode                   1'h0
    9       R/W    restart_auto_negotiation      1'h0
    10      R/W    isolate                       1'h0
    11      R/W    power_down                    1'h0
    12      R/W    auto_negotiation_enable       1'h0
    13      R/W    speed_selection               1'h0
    14      R/W    loopback                      1'h0
    15      R/W    reset                         1'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t reserved_0: 6;
        uint16_t speed_selection_1: 1;
        uint16_t collision_test: 1;
        uint16_t duplex_mode: 1;
        uint16_t restart_auto_negotiation: 1;
        uint16_t isolate: 1;
        uint16_t power_down: 1;
        uint16_t auto_negotiation_enable: 1;
        uint16_t speed_selection: 1;
        uint16_t loopback: 1;
        uint16_t reset: 1;
    } b;
} PHY_RTL8201FR_PAGE0_REG0_TypeDef;

/* PAGE0 REG1 basic mode status register
    0     R    extended_capability              1'h0
    1     R    jabber_detect                    1'h0
    2     R    link_status                      1'h0
    3     R    auto_negotiation_ability         1'h0
    4     RC   remote_fault                     1'h0
    5     R    auto_negotiation_complete        1'h0
    6     R    mf_preamble_suppression          1'h0
    10:7  R    reserved_0                       4'h0
    11    R    base10_t_hd                      1'h0
    12    R    base10_t_fd                      1'h0
    13    R    base100_tx_hd                    1'h0
    14    R    base100_tx_fd                    1'h0
    15    R    base100_t4                       1'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t extended_capability: 1;
        uint16_t jabber_detect: 1;
        uint16_t link_status: 1;
        uint16_t auto_negotiation_ability: 1;
        uint16_t remote_fault: 1;
        uint16_t auto_negotiation_complete: 1;
        uint16_t mf_preamble_suppression: 1;
        uint16_t reserved_0: 4;
        uint16_t base10_t_hd: 1;
        uint16_t base10_t_fd: 1;
        uint16_t base100_tx_hd: 1;
        uint16_t base100_tx_fd: 1;
        uint16_t base100_t4: 1;
    } b;
} PHY_RTL8201FR_PAGE0_REG1_TypeDef;

/* PAGE0 REG31 page select register
    7:0     R/W    page_select              8'h0
    15:8    R/W    reserved_0               8'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t page_select: 8;
        uint16_t reserved_0: 8;
    } b;
} PHY_RTL8201FR_PAGE0_REG31_TypeDef;

/* PAGE7 REG16 rmii mode setting register
    0     R/W    reserved_0             1'h0
    1     R/W    rg_rmii_rxdsel         1'h0
    2     R/W    rg_rmii_rxdv_sel       1'h0
    3     R/W    rmii_mode              1'h0
    7:4   R/W    rg_rmii_rx_offset      1'h0
    11:8  R/W    rg_rmii_tx_offset      1'h0
    12    R/W    rg_rmii_clkdir         1'h0
    15:13 R/W    reserved_1             4'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t reserved_0: 1;
        uint16_t rg_rmii_rxdsel: 1;
        uint16_t rg_rmii_rxdv_sel: 1;
        uint16_t rmii_mode: 1;
        uint16_t rg_rmii_rx_offset: 4;
        uint16_t rg_rmii_tx_offset: 4;
        uint16_t rg_rmii_clkdir: 1;
        uint16_t reserved_1: 3;
    } b;
} PHY_RTL8201FR_PAGE7_REG16_TypeDef;

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
  * \brief  Initialize the PHY API.
  * \return none.
  */
void PHY_RTL8201FRRegisterOps(void);

#ifdef  __cplusplus
}
#endif

#endif /* RTL_PHY_RTL8201FR_H_ */

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor *****END OF FILE****/

