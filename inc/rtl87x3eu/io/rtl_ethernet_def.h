/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl_ethernet_def.h
* \brief    Ethernet related definitions
* \details
* \author   colin
* \date     2024-06-28
* \version  v1.0
* *********************************************************************************************************
*/

#ifndef RTL_ETHERNET_DEF_H
#define RTL_ETHERNET_DEF_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "rtl876x.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/*============================================================================*
 *                          ETHERNET Defines
 *============================================================================*/
#define ETH_SUPPORT_CLOCK_CONFIG                    (1)

/// Defines the max. timeout value when checking the flag of MDIO operations
#define ETH_TIMEOUT_CNT_MAX                 2000000

#define ETH_PHY_DEFAULT_RX_SETUP_TIME       ETH_PHY_RX_SETUP_TIME_16NS
#define ETH_PHY_DEFAULT_TX_SETUP_TIME       ETH_PHY_TX_SETUP_TIME_12NS

#define DEBUG_ETH

#ifdef DEBUG_ETH
#define ETH_DBG_MODULE      MODULE_IO
#define DEBUG_FPGA
#ifdef DEBUG_FPGA
#define ETH_DBG_BUFFER(MODULE, LEVEL, fmt, para_num,...) DBG_DIRECT(fmt, ##__VA_ARGS__)
#else
#define ETH_DBG_BUFFER(MODULE, LEVEL, fmt, para_num,...) DBG_BUFFER_##LEVEL(TYPE_BB2, SUBTYPE_FORMAT, MODULE, fmt, para_num, ##__VA_ARGS__)
#endif
#else
#define ETH_DBG_BUFFER(MODULE, LEVEL, fmt, para_num,...) ((void)0)
#endif

/*============================================================================*
 *                         Types
 *============================================================================*/
#if (ETH_SUPPORT_CLOCK_CONFIG == 1)
typedef enum
{
    ETH_CLK_100M,
    ETH_CLK_50M,
} ETHClk_TypeDef;

#define IS_ETH_CLK(CLK)                 (((CLK) == ETH_CLK_100M) || \
                                         ((CLK) == ETH_CLK_50M))

typedef enum
{
    ETH_CLK_PLL1,
    ETH_CLK_PLL3,
} ETHClkSrc_TypeDef;

#define IS_ETH_CLK_SRC(CLK_SRC)         (((CLK_SRC) == ETH_CLK_PLL1) || \
                                         ((CLK_SRC) == ETH_CLK_PLL3))

typedef enum
{
    ETH_CLK_DIV_1 = 0x0,
    ETH_CLK_DIV_2 = 0x1,
    ETH_CLK_DIV_4 = 0x2,
    ETH_CLK_DIV_8 = 0x3,
    ETH_CLK_DIV_16 = 0x4,
    ETH_CLK_DIV_32 = 0x5,
    ETH_CLK_DIV_40 = 0x6,
    ETH_CLK_DIV_64 = 0x7,
} ETHClkDiv_TypeDef;

#define IS_ETH_CLK_DIV(DIV)             (((DIV) == ETH_CLK_DIV_1)  || \
                                         ((DIV) == ETH_CLK_DIV_2)  || \
                                         ((DIV) == ETH_CLK_DIV_4)  || \
                                         ((DIV) == ETH_CLK_DIV_8)  || \
                                         ((DIV) == ETH_CLK_DIV_16) || \
                                         ((DIV) == ETH_CLK_DIV_32) || \
                                         ((DIV) == ETH_CLK_DIV_40) || \
                                         ((DIV) == ETH_CLK_DIV_64))
#endif

/*============================================================================*
 *                          ETHERNET Registers and Field Descriptions
 *============================================================================*/
/* 0x00
    7:0     R/W    idr3                    8'h0
    15:8    R/W    idr2                    8'h0
    23:16   R/W    idr1                    8'h0
    31:24   R/W    idr0                    8'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t idr3: 8;
        uint32_t idr2: 8;
        uint32_t idr1: 8;
        uint32_t idr0: 8;
    } b;
} ETH_IDR0_TypeDef;

/* 0x04
    15:0    R/W    reserved15_0            16'h0
    23:16   R/W    idr5                    8'h0
    31:24   R/W    idr4                    8'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t reserved_0: 16;
        uint32_t idr5: 8;
        uint32_t idr4: 8;
    } b;
} ETH_IDR4_TypeDef;

/* 0x08
    7:0     R/W    mar3                    8'h0
    15:8    R/W    mar2                    8'h0
    23:16   R/W    mar1                    8'h0
    31:24   R/W    mar0                    8'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t mar3: 8;
        uint32_t mar2: 8;
        uint32_t mar1: 8;
        uint32_t mar0: 8;
    } b;
} ETH_MAR0_TypeDef;

/* 0x0C
    7:0     R/W    mar7                    8'h0
    15:8    R/W    mar6                    8'h0
    23:16   R/W    mar5                    8'h0
    31:24   R/W    mar4                    8'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t mar7: 8;
        uint32_t mar6: 8;
        uint32_t mar5: 8;
        uint32_t mar4: 8;
    } b;
} ETH_MAR4_TypeDef;

/* 0x10
    15:0    R/WC   tx_ok                   16'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t txok: 16;
    } b;
} ETH_TXOKCNT_TypeDef;

/* 0x12
    15:0    R/WC   rx_ok                   16'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t rx_ok: 16;
    } b;
} ETH_RXOKCNT_TypeDef;

/* 0x14
    15:0    R/WC   tx_err                  16'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t tx_err: 16;
    } b;
} ETH_TXERR_TypeDef;

/* 0x16
    15:0    R/WC   rx_err                  16'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t rx_err: 16;
    } b;
} ETH_RXERR_TypeDef;

/* 0x18
    15:0    R/WC   miss_pkt                16'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t miss_pkt: 16;
    } b;
} ETH_MISSPKT_TypeDef;

/* 0x1A
    15:0    R/WC   fae                     16'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t fae: 16;
    } b;
} ETH_FAE_TypeDef;

/* 0x1C
    15:0    R/WC   tx1_col                 16'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t tx1_col: 16;
    } b;
} ETH_TX1COL_TypeDef;

/* 0x1E
    15:0    R/WC   txm_col                 16'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t txm_col: 16;
    } b;
} ETH_TXMCOL_TypeDef;

/* 0x20
    15:0    R/WC   rx_ok_phy               16'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t rx_ok_phy: 16;
    } b;
} ETH_RXOKPHY_TypeDef;

/* 0x22
    15:0    R/WC   rx_ok_brd               16'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t rx_ok_brd: 16;
    } b;
} ETH_RXOKBRD_TypeDef;

/* 0x24
    15:0    R/WC   rx_ok_mul               16'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t rx_ok_mul: 16;
    } b;
} ETH_RXOKMU1_TypeDef;

/* 0x26
    15:0    R/WC   tx_abt                  16'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t tx_abt: 16;
    } b;
} ETH_TXABT_TypeDef;

/* 0x28
    15:0    R/WC   tx_under_run            16'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t tx_under_run: 16;
    } b;
} ETH_TXUNDERC_TypeDef;

/* 0x2A
    15:0    R/WC   rdu_miss_pktc           16'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t rdu_miss_pktc: 16;
    } b;
} ETH_MISSPKT_RDU_TypeDef;

/* 0x34
    0       R      rdu                     1'h0
    1       R      rx_fifo_empty           1'h1
    2       R      tx_under_run            1'h0
    3       R      tok                     1'h0
    31:4    R      reserved31_4            28'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t rdu: 1;
        const uint32_t rx_fifo_empty: 1;
        const uint32_t tx_under_run: 1;
        const uint32_t tok: 1;
        const uint32_t reserved_0: 28;
    } b;
} ETH_TRSR_TypeDef;

/* 0x38
    0       W      rst                     1'h0
    1       R/W    rx_chk_sum              1'h0
    2       R/W    rx_vlan                 1'h0
    3       R/W    rx_jumbo                1'h0
    4       R/W    dis_lso_err             1'h0
    5       R/W    tdsc_debug              1'h0
    31:6    R/W    reserved31_6            26'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t rst: 1;
        uint32_t rx_chk_sum: 1;
        uint32_t rx_vlan: 1;
        uint32_t rx_jumbo: 1;
        uint32_t dis_lso_err: 1;
        uint32_t tdsc_debug: 1;
        uint32_t reserved_0: 26;
    } b;
} ETH_CR_TypeDef;

/* 0x3C
    0       R/W1C  s_rok                   1'h0
    1       R/W1C  s_cnt_wrap              1'h0
    2       R/W1C  s_rer_runt              1'h0
    3       R/W    reserved3               1'h0
    4       R/W1C  s_rer_ovf               1'h0
    5       R/W1C  s_rdu                   1'h0
    6       R/W1C  s_tok_or_ti             1'h0
    7       R/W1C  s_ter                   1'h0
    8       R/W1C  s_link_chg              1'h0
    9       R/W1C  s_tdu                   1'h0
    10      R/W1C  s_swint                 1'h1
    11      R/W1C  s_rdu2                  1'h0
    12      R/W1C  s_rdu3                  1'h0
    13      R/W1C  s_rdu4                  1'h0
    14      R/W1C  s_rdu5                  1'h0
    15      R/W1C  s_rdu6                  1'h0
    16      R/W    rok                     1'h0
    17      R/W    cnt_wrap                1'h0
    18      R/W    rer_runt                1'h0
    19      R/W    reserved19              1'h0
    20      R/W    rer_ovf                 1'h0
    21      R/W    rdu                     1'h0
    22      R/W    tok_or_ti               1'h0
    23      R/W    ter                     1'h0
    24      R/W    link_chg                1'h0
    25      R/W    tdu                     1'h0
    26      R/W    swint                   1'h0
    27      R/W    rdu2                    1'h0
    28      R/W    rdu3                    1'h0
    29      R/W    rdu4                    1'h0
    30      R/W    rdu5                    1'h0
    31      R/W    rdu6                    1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t s_rok: 1;
        uint32_t s_cnt_wrap: 1;
        uint32_t s_rer_runt: 1;
        uint32_t reserved_1: 1;
        uint32_t s_rer_ovf: 1;
        uint32_t s_rdu: 1;
        uint32_t s_tok_or_ti: 1;
        uint32_t s_ter: 1;
        uint32_t s_link_chg: 1;
        uint32_t s_tdu: 1;
        uint32_t s_swint: 1;
        uint32_t s_rdu2: 1;
        uint32_t s_rdu3: 1;
        uint32_t s_rdu4: 1;
        uint32_t s_rdu5: 1;
        uint32_t s_rdu6: 1;
        uint32_t rok: 1;
        uint32_t cnt_wrap: 1;
        uint32_t rer_runt: 1;
        uint32_t reserved_0: 1;
        uint32_t rer_ovf: 1;
        uint32_t rdu: 1;
        uint32_t tok_or_ti: 1;
        uint32_t ter: 1;
        uint32_t link_chg: 1;
        uint32_t tdu: 1;
        uint32_t swint: 1;
        uint32_t rdu2: 1;
        uint32_t rdu3: 1;
        uint32_t rdu4: 1;
        uint32_t rdu5: 1;
        uint32_t rdu6: 1;
    } b;
} ETH_ISR_IMR_TypeDef;

/* 0x40
    7:0     R/W    reserved7_0             8'h0
    9:8     R/W    lpbk                    2'h0
    12:10   R/W    inter_frm_gp_tm         3'h0
    31:13   R/W    reserved31_13           20'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t reserved_1: 8;
        uint32_t lpbk: 2;
        uint32_t inter_frm_gp_tm: 3;
        uint32_t reserved_0: 19;
    } b;
} ETH_TCR_TypeDef;

/* 0x44
    0       R/W    accept_all_pkt          1'h0
    1       R/W    accept_phy_mtch_pkt     1'h0
    2       R/W    accept_mltcst_pkt       1'h0
    3       R/W    accept_brdcst_pkt       1'h0
    4       R/W    accept_runt_pkt         1'h0
    5       R/W    accept_err_pkt          1'h0
    6       R/W    accept_flw_ctl_pkt      1'h0
    7       R/W    home_pna                1'h0
    31:8    R/W    reserved31_8            24'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t accept_all_pkt: 1;
        uint32_t accept_phy_mtch_pkt: 1;
        uint32_t accept_mltcst_pkt: 1;
        uint32_t accept_brdcst_pkt: 1;
        uint32_t accept_runt_pkt: 1;
        uint32_t accept_err_pkt: 1;
        uint32_t accept_flw_ctl_pkt: 1;
        uint32_t home_pna: 1;
        uint32_t reserved_0: 24;
    } b;
} ETH_RCR_TypeDef;

/* 0x48
    7:0     R/W    cpu_tg_prtcl_val        8'h0
    15:8    R/W    cpu_tg_prtcl_msk        8'h0
    17:16   R/W    ct_rsize_1_0            2'h0
    21:18   R/W    cpu_tg_switch           4'h0
    22      R/W    cpu_tg_aspri            1'h1
    23      R/W    cpu_tg_normk            1'h0
    24      R/W    cpu_tg_dsbl_lrn         1'h1
    26:25   R/W    cpu_tg_rsize_3_2        2'h0
    30:27   R/W    cpu_tg_tsize            4'h0
    31      R/W    cpu_tg_rx_en            1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t cpu_tg_prtcl_val: 8;
        uint32_t cpu_tg_prtcl_msk: 8;
        uint32_t ct_rsize_1_0: 2;
        uint32_t cpu_tg_switch: 4;
        uint32_t cpu_tg_aspri: 1;
        uint32_t cpu_tg_normk: 1;
        uint32_t cpu_tg_dsbl_lrn: 1;
        uint32_t cpu_tg_rsize_3_2: 2;
        uint32_t cpu_tg_tsize: 4;
        uint32_t cpu_tg_rx_en: 1;
    } b;
} ETH_CPUTAG_TypeDef;

/* 0x4C
    23:0    R/W    reserved23_0            24'h0
    24      R/W    en_int_split            1'h0
    25      R/W    en_int_route            1'h0
    26      R/W    r_mdslv_out_edge        1'h0
    27      R/W    r_tso_id_sel            1'h0
    29:28   R/W    rff_size_sel            2'h2
    31:30   R/W    reserved31_30           2'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t reserved_1: 24;
        uint32_t en_int_split: 1;
        uint32_t en_int_route: 1;
        uint32_t r_mdslv_out_edge: 1;
        uint32_t r_tso_id_sel: 1;
        uint32_t rff_size_sel: 2;
        uint32_t reserved_0: 2;
    } b;
} ETH_CONFIG_TypeDef;

/* 0x50
    2:0     R/W    spa_pon                 3'h0
    3       R/W    reserved3               1'h0
    6:4     R/W    spa_dsl                 3'h0
    31:7    R/W    reserved31_7            26'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t spa_pon: 3;
        uint32_t reserved_1: 1;
        uint32_t spa_dsl: 3;
        uint32_t reserved_0: 25;
    } b;
} ETH_CPUTAG1_TypeDef;

/* 0x58
    0       R/W    refclk_on               1'h1
    1       R/W    tx_refclk_phase         1'h0
    3:2     R/W    reserved3_2             1'h0
    4       R/W    rx_refclk_phase         1'h0
    5       R/W    reserved5               1'h0
    6       R      eth_md_mst              1'h1
    7       R/W    reg_rmii2mii_en         1'h0
    9:8     R      eee_aby                 2'h0
    10      R/W    force_spd_mode          1'h0
    11      R/W    rgmii_tx_encode_sts     1'h0
    12      R/W    rgmii_rx_decode_sts     1'h0
    13      R/W    eth_phy_mode            1'h0
    14      R/W    reserved14              1'h0
    15      R      eth_sel_phyif_0         1'h1
    17:16   R/W    force_spd               2'h3
    18      R/W    force_link              1'h0
    19      R/W    force_full_dup          1'h0
    20      R      sel_mii                 1'h1
    21      R      nw_complete             1'h1
    22      R      full_dupreg             1'h1
    23      R/W    sel_rgmii               1'h0
    24      R      rx_pf                   1'h0
    25      R      tx_pf                   1'h0
    26      R      link_status             1'h0
    27      R      speed_10                1'h0
    28      R      speed_1000              1'h0
    29      R/W    tx_flow_ctr_en          1'h0
    30      R/W    rx_flow_ctr_en          1'h0
    31      R/W    force_txrx_flow_ctr_en  1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t refclk_on: 1;
        uint32_t tx_refclk_phase: 1;
        uint32_t reserved_2: 2;
        uint32_t rx_refclk_phase: 1;
        uint32_t reserved_1: 1;
        const uint32_t eth_md_mst: 1;
        uint32_t reg_rmii2mii_en: 1;
        const uint32_t eee_aby: 2;
        uint32_t force_spd_mode: 1;
        uint32_t rgmii_tx_encode_sts: 1;
        uint32_t rgmii_rx_decode_sts: 1;
        uint32_t eth_phy_mode: 1;
        uint32_t reserved_0: 1;
        const uint32_t eth_sel_phyif_0: 1;
        uint32_t force_spd: 2;
        uint32_t force_link: 1;
        uint32_t force_full_dup: 1;
        const uint32_t sel_mii: 1;
        const uint32_t nw_complete: 1;
        const uint32_t full_dupreg: 1;
        uint32_t sel_rgmii: 1;
        const uint32_t rx_pf: 1;
        const uint32_t tx_pf: 1;
        const uint32_t link_status: 1;
        const uint32_t link_speed: 2;
        uint32_t tx_flow_ctr_en: 1;
        uint32_t rx_flow_ctr_en: 1;
        uint32_t force_txrx_flow_ctr_en: 1;
    } b;
} ETH_MSR_TypeDef;

/* 0x5C
    15:0    R/W    data15_0                16'h0
    20:16   R/W    reg_addr4_0             5'h0
    21      R/W    polling_eee_reg         1'h0
    22      R/W    disable_auto_polling    1'h0
    23      R/W    en_eeepauto_write       1'h0
    24      R/W    reserved24              1'h0
    25      R      mdio_busy               1'h0
    30:26   R/W    phy_address             5'h0
    31      R/W    flag                    1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t data15_0: 16;
        uint32_t reg_addr4_0: 5;
        uint32_t polling_eee_reg: 1;
        uint32_t disable_auto_polling: 1;
        uint32_t en_eeepauto_write: 1;
        uint32_t reserved_0: 1;
        const uint32_t mdio_busy: 1;
        uint32_t phy_address: 5;
        uint32_t flag: 1;
    } b;
} ETH_MIIAR_TypeDef;

/* 0x60
    23:0    R/W    reserved23_0            24'h0
    24      W/SC   swint                   1'h0
    31:25   R/W    reserved31_25           7'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t reserved_1: 24;
        uint32_t swint: 1;
        uint32_t reserved_0: 7;
    } b;
} ETH_SWINT_TypeDef;

/* 0x64
    14:0    R/W    reserved14_0            15'h0
    15      R/W    tdsc_vlan_type          1'h0
    31:16   R/W    stag_pid                15'h88a8
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t reserved_0: 15;
        uint32_t tdsc_vlan_type: 1;
        uint32_t stag_pid: 16;
    } b;
} ETH_VLAN_TypeDef;

/* 0x70
    3:0     R/W    led_sel_0               4'h0
    7:4     R/W    led_sel_1               4'h0
    11:8    R/W    led_sel_2               4'h0
    15:12   R/W    led_sel_3               4'h0
    17:16   R/W    led_sel                 2'h0
    18      R/W    custom_led              1'h0
    19      R/W    eee_en_led              1'h0
    31:20   R/W    reserved31_20           12'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t led_sel_0: 4;
        uint32_t led_sel_1: 4;
        uint32_t led_sel_2: 4;
        uint32_t led_sel_3: 4;
        uint32_t led_sel: 2;
        uint32_t custom_led: 1;
        uint32_t eee_en_led: 1;
        uint32_t reserved_0: 12;
    } b;
} ETH_LED_CTRL_TypeDef;

/* 0x80
    3:0     R/W    csr_tff_dvs             4'h0
    4       R/W    csr_tff_dvse            1'h0
    5       R/W    csr_tff_nap             1'h0
    6       R/W    csr_tff_tmck            1'h0
    7       R/W    reserved7               1'h0
    11:8    R/W    csr_rff_dvs             4'h0
    12      R/W    csr_rff_dvse            1'h0
    13      R/W    csr_rff_nap             1'h0
    14      R/W    csr_rff_tmck            1'h0
    31:15   R/W    reserved31_15           17'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t csr_tff_dvs: 4;
        uint32_t csr_tff_dvse: 1;
        uint32_t csr_tff_nap: 1;
        uint32_t csr_tff_tmck: 1;
        uint32_t reserved_1: 1;
        uint32_t csr_rff_dvs: 4;
        uint32_t csr_rff_dvse: 1;
        uint32_t csr_rff_nap: 1;
        uint32_t csr_rff_tmck: 1;
        uint32_t reserved_0: 17;
    } b;
} ETH_BIST_TypeDef;

/* 0x84
    7:0     R/W    pglbk_data_pat          8'h0
    8       R/W    r_disable_txcrc         1'h0
    10:9    R/W    pglbk_pkt_num_sel       2'h0
    11      R/W    pblbk_pkt_length_sel    1'h0
    12      R      pglbk_fail              1'h0
    13      R      pglbk_done              1'h0
    14      R/W    pglbk_data_pat_sel      1'h0
    15      R/W    en_pglbk                1'h0
    31:16   R/W    reserved31_16           16'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t pglbk_data_pat: 8;
        uint32_t r_disable_txcrc: 1;
        uint32_t pglbk_pkt_num_sel: 2;
        uint32_t pblbk_pkt_length_sel: 1;
        const uint32_t pglbk_fail: 1;
        const uint32_t pglbk_done: 1;
        uint32_t pglbk_data_pat_sel: 1;
        uint32_t en_pglbk: 1;
        uint32_t reserved_0: 16;
    } b;
} ETH_PGLBK_TypeDef;

/* 0x88
    1:0     R/W    eee_timer_unit_100_2    1'h0
    3:2     R/W    eee_timer_unit_100_1    1'h0
    5:4     R/W    eee_timer_unit_giga_2   1'h0
    7:6     R/W    eee_timer_unit_giga_1   1'h0
    8       R/W    eee_tx_timer_en         1'h0
    9       R/W    eee_rx_timer_en         1'h0
    10      R/W    eee_reqset0             1'h0
    11      R/W    eee_reqset1             1'h0
    12      R/W    eee_reqset2             1'h0
    13      R/W    eee_wake_set0           1'h0
    14      R/W    eee_wake_set1           1'h0
    15      R/W    reserved15              1'h0
    16      R      eee_pause_flag          1'h0
    17      R      eee_rx_sts              1'h0
    18      R      eee_tx_sts              1'h0
    19      R      eee_sts                 1'h0
    22:20   R/W    reserved22_20           3'h0
    23      R/W    eee_outqlowq_over       1'h0
    25:24   R/W    reserved25_24           2'h0
    26      R/W    en_eee_rx               1'h0
    27      R/W    en_eee_tx               1'h0
    28      R/W    en_eee_10m              1'h0
    29      R/W    frc_eee_100m            1'h0
    30      R/W    frc_eee_giga            1'h0
    31      R/W    en_frc_eee              1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t eee_timer_unit_100_2: 2;
        uint32_t eee_timer_unit_100_1: 2;
        uint32_t eee_timer_unit_giga_2: 2;
        uint32_t eee_timer_unit_giga_1: 2;
        uint32_t eee_tx_timer_en: 1;
        uint32_t eee_rx_timer_en: 1;
        uint32_t eee_reqset0: 1;
        uint32_t eee_reqset1: 1;
        uint32_t eee_reqset2: 1;
        uint32_t eee_wake_set0: 1;
        uint32_t eee_wake_set1: 1;
        uint32_t reserved_2: 1;
        const uint32_t eee_pause_flag: 1;
        const uint32_t eee_rx_sts: 1;
        const uint32_t eee_tx_sts: 1;
        const uint32_t eee_sts: 1;
        uint32_t reserved_1: 3;
        uint32_t eee_outqlowq_over: 1;
        uint32_t reserved_0: 2;
        uint32_t en_eee_rx: 1;
        uint32_t en_eee_tx: 1;
        uint32_t en_eee_10m: 1;
        uint32_t frc_eee_100m: 1;
        uint32_t frc_eee_giga: 1;
        uint32_t en_frc_eee: 1;
    } b;
} ETH_EEE_CR1_TypeDef;

/* 0x8C
    7:0     R/W    eee_timer_tp_giga       8'h0a
    15:8    R/W    eee_timer_td_giga       8'hc4
    23:16   R/W    eee_timer_tr_giga       8'h0a
    31:24   R/W    eee_timer_tw_giga       8'h10
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t eee_timer_tp_giga: 8;
        uint32_t eee_timer_td_giga: 8;
        uint32_t eee_timer_tr_giga: 8;
        uint32_t eee_timer_tw_giga: 8;
    } b;
} ETH_EEE_CR2_TypeDef;

/* 0x90
    7:0     R/W    eee_timer_tp_100        8'h0a
    15:8    R/W    eee_timer_td_100        8'hf4
    23:16   R/W    eee_timer_tr_100        8'h0a
    31:24   R/W    eee_timer_tw_100        8'h1e
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t eee_timer_tp_100: 8;
        uint32_t eee_timer_td_100: 8;
        uint32_t eee_timer_tr_100: 8;
        uint32_t eee_timer_tw_100: 8;
    } b;
} ETH_EEE_CR3_TypeDef;

/* 0x94
    15:0    R/W    eee_tx_thr_100          15'h003e
    31:16   R/W    eee_tx_thr_giga         15'h0271
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t eee_tx_thr_100: 16;
        uint32_t eee_tx_thr_giga: 16;
    } b;
} ETH_EEE_CR4_TypeDef;

/* 0x98
    21:0    RC     eee_rxlpi_time          22'h0
    31:22   R/W    reserved31_22           10'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t eee_rxlpi_time: 22;
        uint32_t reserved_0: 10;
    } b;
} ETH_EEE_LPI_TMO_TypeDef;

/* 0x9C
    21:0    RC     eee_txlpi_time          22'h0
    22      R/W    eee_trxctl_level        1'h0
    23      R/W    eee_stop_txc            1'h0
    31:24   R/W    reserved31_24           8'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t eee_txlpi_time: 22;
        uint32_t eee_trxctl_level: 1;
        uint32_t eee_stop_txc: 1;
        uint32_t reserved_0: 8;
    } b;
} ETH_EEE_LPI_TM1_TypeDef;

/* 0xB0
    15:0    R/W    reserved15_0            16'h0
    22:16   R/W    eeep_dbg_sgl_sel        1'h0
    23      R/W    reserved23              1'h0
    24      R/W    phy_mst_mode            1'h0
    25      R/W    reserved25              1'h0
    26      R/W    cfg_eeep_defer_txlpi    1'h0
    27      R/W    cfg_en_eeep_100m        1'h0
    28      R/W    cfg_en_eeep_giga        1'h0
    29      R/W    cfg_en_eeep_tx          1'h0
    30      R/W    cfg_en_eeep_rx          1'h0
    31      R/W    en_eeep                 1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t reserved_2: 16;
        uint32_t eeep_dbg_sgl_sel: 7;
        uint32_t reserved_1: 1;
        uint32_t phy_mst_mode: 1;
        uint32_t reserved_0: 1;
        uint32_t cfg_eeep_defer_txlpi: 1;
        uint32_t cfg_en_eeep_100m: 1;
        uint32_t cfg_en_eeep_giga: 1;
        uint32_t cfg_en_eeep_tx: 1;
        uint32_t cfg_en_eeep_rx: 1;
        uint32_t en_eeep: 1;
    } b;
} ETH_EEEP_CR1_TypeDef;

/* 0xB4
    7:0     R/W    eeep_sleep_step         8'h1
    31:8    R/W    reserved31_8            24'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t eeep_sleep_step: 8;
        uint32_t reserved_0: 24;
    } b;
} ETH_EEEP_CR2_TypeDef;

/* 0xB8
    15:0    R/W    eeep_rx_thr             16'hf0
    31:16   R/W    eeep_tx_thr             16'hf0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t eeep_rx_thr: 16;
        uint32_t eeep_tx_thr: 16;
    } b;
} ETH_EEEP_CR3_TypeDef;

/* 0xBC
    7:0     R/W    eeep_timer_pause        8'h1
    15:8    R/W    eeep_timer_tx_wait      8'h1
    23:16   R/W    eeep_timer_rx_wake      8'h2
    31:24   R/W    eeep_time_rx_rate       8'hc
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t eeep_timer_pause: 8;
        uint32_t eeep_timer_tx_wait: 8;
        uint32_t eeep_timer_rx_wake: 8;
        uint32_t eeep_time_rx_rate: 8;
    } b;
} ETH_EEEP_CR4_TypeDef;

/* 0xC0
    7:0     R/W    eeep_timer_rx_sleep     8'hc
    9:8     R/W    eeep_timer_unit         2'h1
    15:10   R/W    reserved1               6'h0
    23:16   R/W    eeep_tx_rate            8'h0
    31:24   R/W    reserved31_24           8'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t eeep_timer_rx_sleep: 8;
        uint32_t eeep_timer_unit: 2;
        uint32_t reserved_1: 6;
        uint32_t eeep_tx_rate: 8;
        uint32_t reserved_0: 8;
    } b;
} ETH_EEEP_CR5_TypeDef;

/* 0xC4
    15:0    R/W    eeep_abig_off           16'h800a
    31:16   R/W    eeep_abig_on            16'hb80a
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t eeep_abig_off: 16;
        uint32_t eeep_abig_on: 16;
    } b;
} ETH_EEEP_CR6_TypeDef;

/* 0xC8
    15:0    R/W    eeep_ldvbias_off        15'h0550
    31:16   R/W    eeep_ldvbias_on         15'h7550
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t eeep_ldvbias_off: 16;
        uint32_t eeep_ldvbias_on: 16;
    } b;
} ETH_EEEP_CR7_TypeDef;

/* 0xD0
    0       RO     imr0_rok1               1'h0
    1       R/W    imr0_rok2               1'h0
    2       R/W    imr0_rok3               1'h0
    3       R/W    imr0_rok4               1'h0
    4       R/W    imr0_rok5               1'h0
    5       R/W    imr0_rok6               1'h0
    15:6    R/W    reserved15_6            10'h0
    16      RO     imr0_tok1               1'h0
    17      R/W    imr0_tok2               1'h0
    18      R/W    imr0_tok3               1'h0
    19      R/W    imr0_tok4               1'h0
    20      R/W    imr0_tok5               1'h0
    23:21   R/W    reserved23_21           3'h0
    24      RO     imr0_tdu1               1'h0
    25      R/W    imr0_tdu2               1'h0
    26      R/W    imr0_tdu3               1'h0
    27      R/W    imr0_tdu4               1'h0
    28      R/W    imr0_tdu5               1'h0
    31:29   R/W    reserved31_29           3'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t imr0_rok1: 1;
        uint32_t imr0_rok2: 1;
        uint32_t imr0_rok3: 1;
        uint32_t imr0_rok4: 1;
        uint32_t imr0_rok5: 1;
        uint32_t imr0_rok6: 1;
        uint32_t reserved_2: 10;
        const uint32_t imr0_tok1: 1;
        uint32_t imr0_tok2: 1;
        uint32_t imr0_tok3: 1;
        uint32_t imr0_tok4: 1;
        uint32_t imr0_tok5: 1;
        uint32_t reserved_1: 3;
        const uint32_t imr0_tdu1: 1;
        uint32_t imr0_tdu2: 1;
        uint32_t imr0_tdu3: 1;
        uint32_t imr0_tdu4: 1;
        uint32_t imr0_tdu5: 1;
        uint32_t reserved_0: 3;
    } b;
} ETH_IMR0_TypeDef;

/* 0xD4
    0       R/W    imr1_rok1               1'h0
    1       R/W    imr1_rok2               1'h0
    2       R/W    imr1_rok3               1'h0
    3       R/W    imr1_rok4               1'h0
    4       R/W    imr1_rok5               1'h0
    5       R/W    imr1_rok6               1'h0
    7:6     R/W    reserved7_6             2'h0
    8       R/W    imr1_rdu1               1'h0
    9       R/W    imr1_rdu2               1'h0
    10      R/W    imr1_rdu3               1'h0
    11      R/W    imr1_rdu4               1'h0
    12      R/W    imr1_rdu5               1'h0
    13      R/W    imr1_rdu6               1'h0
    15:14   R/W    reserved15_14           2'h0
    16      R/W    imr1_tok1               1'h0
    17      R/W    imr1_tok2               1'h0
    18      R/W    imr1_tok3               1'h0
    19      R/W    imr1_tok4               1'h0
    20      R/W    imr1_tok5               1'h0
    23:21   R/W    reserved23_21           3'h0
    24      R/W    imr1_tdu1               1'h0
    25      R/W    imr1_tdu2               1'h0
    26      R/W    imr1_tdu3               1'h0
    27      R/W    imr1_tdu4               1'h0
    28      R/W    imr1_tdu5               1'h0
    31:29   R/W    reserved31_29           3'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t imr1_rok1: 1;
        uint32_t imr1_rok2: 1;
        uint32_t imr1_rok3: 1;
        uint32_t imr1_rok4: 1;
        uint32_t imr1_rok5: 1;
        uint32_t imr1_rok6: 1;
        uint32_t reserved_3: 2;
        uint32_t imr1_rdu1: 1;
        uint32_t imr1_rdu2: 1;
        uint32_t imr1_rdu3: 1;
        uint32_t imr1_rdu4: 1;
        uint32_t imr1_rdu5: 1;
        uint32_t imr1_rdu6: 1;
        uint32_t reserved_2: 2;
        uint32_t imr1_tok1: 1;
        uint32_t imr1_tok2: 1;
        uint32_t imr1_tok3: 1;
        uint32_t imr1_tok4: 1;
        uint32_t imr1_tok5: 1;
        uint32_t reserved_1: 3;
        uint32_t imr1_tdu1: 1;
        uint32_t imr1_tdu2: 1;
        uint32_t imr1_tdu3: 1;
        uint32_t imr1_tdu4: 1;
        uint32_t imr1_tdu5: 1;
        uint32_t reserved_0: 3;
    } b;
} ETH_IMR1_TypeDef;

/* 0xD8
    0       RO     isr_rok1                1'h0
    1       R/W1C  isr_rok2                1'h0
    2       R/W1C  isr_rok3                1'h0
    3       R/W1C  isr_rok4                1'h0
    4       R/W1C  isr_rok5                1'h0
    5       R/W1C  isr_rok6                1'h0
    15:6    R/W    reserved15_6            10'h0
    16      RO     isr_tok1                1'h0
    17      R/W1C  isr_tok2                1'h0
    18      R/W1C  isr_tok3                1'h0
    19      R/W1C  isr_tok4                1'h0
    20      R/W1C  isr_tok5                1'h0
    23:21   R/W    reserved23_21           3'h0
    24      RO     isr_tdu1                1'h0
    25      R/W1C  isr_tdu2                1'h0
    26      R/W1C  isr_tdu3                1'h0
    27      R/W1C  isr_tdu4                1'h0
    28      R/W1C  isr_tdu5                1'h0
    31:29   R/W    reserved31_29           3'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t isr_rok1: 1;
        uint32_t isr_rok2: 1;
        uint32_t isr_rok3: 1;
        uint32_t isr_rok4: 1;
        uint32_t isr_rok5: 1;
        uint32_t isr_rok6: 1;
        uint32_t reserved_2: 10;
        const uint32_t isr_tok1: 1;
        uint32_t isr_tok2: 1;
        uint32_t isr_tok3: 1;
        uint32_t isr_tok4: 1;
        uint32_t isr_tok5: 1;
        uint32_t reserved_1: 3;
        const uint32_t isr_tdu1: 1;
        uint32_t isr_tdu2: 1;
        uint32_t isr_tdu3: 1;
        uint32_t isr_tdu4: 1;
        uint32_t isr_tdu5: 1;
        uint32_t reserved_0: 3;
    } b;
} ETH_ISR1_TypeDef;

/* 0xDC
    0       R/W    rr1_int_routing         1'h0
    1       R/W    reserved1               1'h0
    2       R/W    rr2_int_routing         1'h0
    3       R/W    reserved3               1'h0
    4       R/W    rr3_int_routing         1'h0
    5       R/W    reserved5               1'h0
    6       R/W    rr4_int_routing         1'h0
    7       R/W    reserved7               1'h0
    8       R/W    rr5_int_routing         1'h0
    9       R/W    reserved9               1'h0
    10      R/W    rr6_int_routing         1'h0
    15:11   R/W    reserved15_11           5'h0
    16      R/W    tr1_int_routing         1'h0
    17      R/W    reserved17              1'h0
    18      R/W    tr2_int_routing         1'h0
    19      R/W    reserved19              1'h0
    20      R/W    tr3_int_routing         1'h0
    21      R/W    reserved21              1'h0
    22      R/W    tr4_int_routing         1'h0
    23      R/W    reserved23              1'h0
    24      R/W    tr5_int_routing         1'h0
    31:25   R/W    reserved31_25           7'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t rr1_int_routing: 1;
        uint32_t reserved_10: 1;
        uint32_t rr2_int_routing: 1;
        uint32_t reserved_9: 1;
        uint32_t rr3_int_routing: 1;
        uint32_t reserved_8: 1;
        uint32_t rr4_int_routing: 1;
        uint32_t reserved_7: 1;
        uint32_t rr5_int_routing: 1;
        uint32_t reserved_6: 1;
        uint32_t rr6_int_routing: 1;
        uint32_t reserved_5: 5;
        uint32_t tr1_int_routing: 1;
        uint32_t reserved_4: 1;
        uint32_t tr2_int_routing: 1;
        uint32_t reserved_3: 1;
        uint32_t tr3_int_routing: 1;
        uint32_t reserved_2: 1;
        uint32_t tr4_int_routing: 1;
        uint32_t reserved_1: 1;
        uint32_t tr5_int_routing: 1;
        uint32_t reserved_0: 7;
    } b;
} ETH_INT_R_TypeDef;

/* 0xE0
    0       R/W    auto_link_st_upd        1'h0
    1       R/W    auto_iso_upd            1'h0
    2       R/W    auto_tx_clk_dis         1'h0
    3       R/W    auto_rx_clk_dis         1'h0
    31:4    R/W    reserved31_4            28'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t auto_link_st_upd: 1;
        uint32_t auto_iso_upd: 1;
        uint32_t auto_tx_clk_dis: 1;
        uint32_t auto_rx_clk_dis: 1;
        uint32_t reserved_0: 28;
    } b;
} ETH_RT_PDOCR_TypeDef;

/* 0xE4
    0       R/W    tx_clk_en               1'h1
    1       R/W    rx_clk_en               1'h1
    2       R/W    dma_data_swap           1'h1
    3       R/W    rev_00e4_3              1'h1
    4       R/W    rev_00e4_4              1'h1
    5       R/W    rev_00e4_5              1'h0
    6       R/W    rev_00e4_6              1'h0
    7       R/W    rev_00e4_7              1'h0
    31:8    R/W    reserved31_8            24'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t tx_clk_en: 1;
        uint32_t rx_clk_en: 1;
        uint32_t dma_data_swap: 1;
        uint32_t rev_00e4_3: 1;
        uint32_t rev_00e4_4: 1;
        uint32_t rev_00e4_5: 1;
        uint32_t rev_00e4_6: 1;
        uint32_t rev_00e4_7: 1;
        uint32_t reserved_0: 24;
    } b;
} ETH_CCR_TypeDef;

/* 0xF0
    0       R/W    rst_ntf                 1'h0
    1       R/W    lpbk_ntf                1'h0
    2       R/W    pwr_dwn_ntf             1'h0
    3       R/W    iso_ntf                 1'h0
    4       R/W    lx_slv_to_inten         1'h0
    5       R/W    lx_mst_to_inten         1'h0
    31:6    R/W    reserved31_6            26'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t rst_ntf: 1;
        uint32_t lpbk_ntf: 1;
        uint32_t pwr_dwn_ntf: 1;
        uint32_t iso_ntf: 1;
        uint32_t lx_slv_to_inten: 1;
        uint32_t lx_mst_to_inten: 1;
        uint32_t reserved_0: 26;
    } b;
} ETH_IMR_TypeDef;

/* 0xF4
    0       R/W1C  rst_ntf_st              1'h0
    1       R/W1C  lpbk_ntf_st             1'h0
    2       R/W1C  pwr_dwn_ntf_st          1'h0
    3       R/W1C  iso_nft_st              1'h0
    4       R/W    lx_slv_to_int           1'h0
    5       R/W    lx_mst_to_int           1'h0
    31:6    R/W    reserved31_6            26'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t rst_ntf_st: 1;
        uint32_t lpbk_ntf_st: 1;
        uint32_t pwr_dwn_ntf_st: 1;
        uint32_t iso_nft_st: 1;
        uint32_t lx_slv_to_int: 1;
        uint32_t lx_mst_to_int: 1;
        uint32_t reserved_0: 26;
    } b;
} ETH_ISR_TypeDef;


/*============================================================================*
 *                          ETHERNET Registers Memory Map
 *============================================================================*/

typedef struct                       /*!< ETH Structure */
{
    __IO ETH_IDR0_TypeDef  ETH_IDR0;                   /*!< 0x00 */
    __IO ETH_IDR4_TypeDef  ETH_IDR4;                   /*!< 0x04 */
    __IO ETH_MAR0_TypeDef  ETH_MAR0;                   /*!< 0x08 */
    __IO ETH_MAR4_TypeDef  ETH_MAR4;                   /*!< 0x0C */
    __IO ETH_TXOKCNT_TypeDef  ETH_TXOKCNT;             /*!< 0x10 */
    __IO ETH_RXOKCNT_TypeDef  ETH_RXOKCNT;             /*!< 0x12 */
    __IO ETH_TXERR_TypeDef  ETH_TXERR;                 /*!< 0x14 */
    __IO ETH_RXERR_TypeDef  ETH_RXERR;                 /*!< 0x16 */
    __IO ETH_MISSPKT_TypeDef  ETH_MISSPKT;             /*!< 0x18 */
    __IO ETH_FAE_TypeDef  ETH_FAE;                     /*!< 0x1A */
    __IO ETH_TX1COL_TypeDef  ETH_TX1COL;               /*!< 0x1C */
    __IO ETH_TXMCOL_TypeDef  ETH_TXMCOL;               /*!< 0x1E */
    __IO ETH_RXOKPHY_TypeDef  ETH_RXOKPHY;             /*!< 0x20 */
    __IO ETH_RXOKBRD_TypeDef  ETH_RXOKBRD;             /*!< 0x22 */
    __IO ETH_RXOKMU1_TypeDef  ETH_RXOKMU1;             /*!< 0x24 */
    __IO ETH_TXABT_TypeDef  ETH_TXABT;                 /*!< 0x26 */
    __IO ETH_TXUNDERC_TypeDef  ETH_TXUNDERC;           /*!< 0x28 */
    __IO ETH_MISSPKT_RDU_TypeDef  ETH_MISSPKT_RDU;     /*!< 0x2A */
    __IO uint32_t  RSVD0[2];                           /*!< 0x2C */
    __IO ETH_TRSR_TypeDef  ETH_TRSR;                   /*!< 0x34 */
    __IO ETH_CR_TypeDef  ETH_CR;                       /*!< 0x38 */
    __IO ETH_ISR_IMR_TypeDef  ETH_ISR_IMR;             /*!< 0x3C */
    __IO ETH_TCR_TypeDef  ETH_TCR;                     /*!< 0x40 */
    __IO ETH_RCR_TypeDef  ETH_RCR;                     /*!< 0x44 */
    __IO ETH_CPUTAG_TypeDef  ETH_CPUTAG;               /*!< 0x48 */
    __IO ETH_CONFIG_TypeDef  ETH_CONFIG;               /*!< 0x4C */
    __IO ETH_CPUTAG1_TypeDef  ETH_CPUTAG1;             /*!< 0x50 */
    __IO uint32_t  RSVD1;                              /*!< 0x54 */
    __IO ETH_MSR_TypeDef  ETH_MSR;                     /*!< 0x58 */
    __IO ETH_MIIAR_TypeDef  ETH_MIIAR;                 /*!< 0x5C */
    __IO ETH_SWINT_TypeDef  ETH_SWINT;                 /*!< 0x60 */
    __IO ETH_VLAN_TypeDef  ETH_VLAN;                   /*!< 0x64 */
    __IO uint32_t  RSVD2[2];                           /*!< 0x68 */
    __IO ETH_LED_CTRL_TypeDef  ETH_LED_CTRL;           /*!< 0x70 */
    __IO uint32_t  RSVD3[3];                           /*!< 0x74 */
    __IO ETH_BIST_TypeDef  ETH_BIST;                   /*!< 0x80 */
    __IO ETH_PGLBK_TypeDef  ETH_PGLBK;                 /*!< 0x84 */
    __IO ETH_EEE_CR1_TypeDef  ETH_EEE_CR1;             /*!< 0x88 */
    __IO ETH_EEE_CR2_TypeDef  ETH_EEE_CR2;             /*!< 0x8C */
    __IO ETH_EEE_CR3_TypeDef  ETH_EEE_CR3;             /*!< 0x90 */
    __IO ETH_EEE_CR4_TypeDef  ETH_EEE_CR4;             /*!< 0x94 */
    __IO ETH_EEE_LPI_TMO_TypeDef  ETH_EEE_LPI_TMO;     /*!< 0x98 */
    __IO ETH_EEE_LPI_TM1_TypeDef  ETH_EEE_LPI_TM1;     /*!< 0x9C */
    __IO uint32_t  RSVD4[4];                           /*!< 0xA0 */
    __IO ETH_EEEP_CR1_TypeDef  ETH_EEEP_CR1;           /*!< 0xB0 */
    __IO ETH_EEEP_CR2_TypeDef  ETH_EEEP_CR2;           /*!< 0xB4 */
    __IO ETH_EEEP_CR3_TypeDef  ETH_EEEP_CR3;           /*!< 0xB8 */
    __IO ETH_EEEP_CR4_TypeDef  ETH_EEEP_CR4;           /*!< 0xBC */
    __IO ETH_EEEP_CR5_TypeDef  ETH_EEEP_CR5;           /*!< 0xC0 */
    __IO ETH_EEEP_CR6_TypeDef  ETH_EEEP_CR6;           /*!< 0xC4 */
    __IO ETH_EEEP_CR7_TypeDef  ETH_EEEP_CR7;           /*!< 0xC8 */
    __IO uint32_t  RSVD5;                              /*!< 0xCC */
    __IO ETH_IMR0_TypeDef  ETH_IMR0;                   /*!< 0xD0 */
    __IO ETH_IMR1_TypeDef  ETH_IMR1;                   /*!< 0xD4 */
    __IO ETH_ISR1_TypeDef  ETH_ISR1;                   /*!< 0xD8 */
    __IO ETH_INT_R_TypeDef  ETH_INT_R;                 /*!< 0xDC */
    __IO ETH_RT_PDOCR_TypeDef  ETH_RT_PDOCR;           /*!< 0xE0 */
    __IO ETH_CCR_TypeDef  ETH_CCR;                     /*!< 0xE4 */
    __IO uint32_t  RSVD6[2];                           /*!< 0xE8 */
    __IO ETH_IMR_TypeDef  ETH_IMR;                     /*!< 0xF0 */
    __IO ETH_ISR_TypeDef  ETH_ISR;                     /*!< 0xF4 */
} ETH_TypeDef;

/*============================================================================*
 *                          ETHERNET Declaration
 *============================================================================*/
#define ETH                ((ETH_TypeDef *) ETHERNET_CFG_BASE)


#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* RTL_ETHERNET_DEF_H */
