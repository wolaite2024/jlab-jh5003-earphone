/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl_ethernet_def.h
* \brief    Ethernet cpu definitions for RTL8762G
* \details
* \author   colin
* \date     2024-06-28
* \version  v1.0
* *********************************************************************************************************
*/

#ifndef RTL_ETHERNET_CPU_REG_H
#define RTL_ETHERNET_CPU_REG_H

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ================================================================================ */
/* ================            ETH CPU Registers                  ================ */
/* ================================================================================ */
/**
  * @brief ETH_CPU
  */
/* 0x00
    31:0    R/W    txfdp1                      32'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t txfdp1: 32;
    } b;
} ETH_TXFDP1_TypeDef;

/* 0x04
    11:0    R      txcdo1                      12'h0
    15:12   R      reserved15_12               4'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __I uint16_t txcdo1: 12;
        __I uint16_t reserved_0: 4;
    } b;
} ETH_TXCDO1_TypeDef;

/* 0x10
    31:0    R/W    txfdp2                      32'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t txfdp2: 32;
    } b;
} ETH_TXFDP2_TypeDef;

/* 0x14
    11:0    R      txcdo2                      12'h0
    15:12   R      reserved15_12               4'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __I uint16_t txcdo2: 12;
        __I uint16_t reserved_0: 4;
    } b;
} ETH_TXCDO2_TypeDef;

/* 0x20
    31:0    R/W    txfdp3                      32'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t txfdp3: 32;
    } b;
} ETH_TXFDP3_TypeDef;

/* 0x24
    11:0    R      txcdo3                      12'h0
    15:12   R      reserved15_12               4'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __I uint16_t txcdo3: 12;
        __I uint16_t reserved_0: 4;
    } b;
} ETH_TXCDO3_TypeDef;

/* 0x30
    31:0    R/W    txfdp4                      32'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t txfdp4: 32;
    } b;
} ETH_TXFDP4_TypeDef;

/* 0x34
    11:0    R      txcdo4                      12'h0
    15:12   R      reserved15_12               4'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __I uint16_t txcdo4: 12;
        __I uint16_t reserved_0: 4;
    } b;
} ETH_TXCDO4_TypeDef;

/* 0x40
    31:0    R/W    txfdp5                      32'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t txfdp5: 32;
    } b;
} ETH_TXFDP5_TypeDef;

/* 0x44
    11:0    R      txcdo5                      12'h0
    15:12   R      reserved15_12               4'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __I uint16_t txcdo5: 12;
        __I uint16_t reserved_0: 4;
    } b;
} ETH_TXCDO5_TypeDef;

/* 0x70
    2:0     R/W    ext1_pri0_route             3'h0
    3       R/W    reserved3                   1'h0
    6:4     R/W    ext1_pri1_route             3'h0
    7       R/W    reserved7                   1'h0
    10:8    R/W    ext1_pri2_route             3'h0
    11      R/W    reserved11                  1'h0
    14:12   R/W    ext1_pri3_route             3'h0
    15      R/W    reserved15                  1'h0
    18:16   R/W    ext1_pri4_route             3'h0
    19      R/W    reserved19                  1'h0
    22:20   R/W    ext1_pri5_route             3'h0
    23      R/W    reserved25                  1'h0
    26:24   R/W    ext1_pri6_route             3'h0
    27      R/W    reserved27                  1'h0
    30:28   R/W    ext1_pri7_route             3'h0
    31      R/W    reserved31                  1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t ext1_pri0_route: 3;
        __IO uint32_t reserved_7: 1;
        __IO uint32_t ext1_pri1_route: 3;
        __IO uint32_t reserved_6: 1;
        __IO uint32_t ext1_pri2_route: 3;
        __IO uint32_t reserved_5: 1;
        __IO uint32_t ext1_pri3_route: 3;
        __IO uint32_t reserved_4: 1;
        __IO uint32_t ext1_pri4_route: 3;
        __IO uint32_t reserved_3: 1;
        __IO uint32_t ext1_pri5_route: 3;
        __IO uint32_t reserved_2: 1;
        __IO uint32_t ext1_pri6_route: 3;
        __IO uint32_t reserved_1: 1;
        __IO uint32_t ext1_pri7_route: 3;
        __IO uint32_t reserved_0: 1;
    } b;
} ETH_RRING_MAP1_TypeDef;

/* 0x74
    2:0     R/W    ext2_pri0_route             3'h0
    3       R/W    reserved3                   1'h0
    6:4     R/W    ext2_pri1_route             3'h0
    7       R/W    reserved7                   1'h0
    10:8    R/W    ext2_pri2_route             3'h0
    11      R/W    reserved11                  1'h0
    14:12   R/W    ext2_pri3_route             3'h0
    15      R/W    reserved15                  1'h0
    18:16   R/W    ext2_pri4_route             3'h0
    19      R/W    reserved19                  1'h0
    22:20   R/W    ext2_pri5_route             3'h0
    23      R/W    reserved25                  1'h0
    26:24   R/W    ext2_pri6_route             3'h0
    27      R/W    reserved27                  1'h0
    30:28   R/W    ext2_pri7_route             3'h0
    31      R/W    reserved31                  1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t ext2_pri0_route: 3;
        __IO uint32_t reserved_7: 1;
        __IO uint32_t ext2_pri1_route: 3;
        __IO uint32_t reserved_6: 1;
        __IO uint32_t ext2_pri2_route: 3;
        __IO uint32_t reserved_5: 1;
        __IO uint32_t ext2_pri3_route: 3;
        __IO uint32_t reserved_4: 1;
        __IO uint32_t ext2_pri4_route: 3;
        __IO uint32_t reserved_3: 1;
        __IO uint32_t ext2_pri5_route: 3;
        __IO uint32_t reserved_2: 1;
        __IO uint32_t ext2_pri6_route: 3;
        __IO uint32_t reserved_1: 1;
        __IO uint32_t ext2_pri7_route: 3;
        __IO uint32_t reserved_0: 1;
    } b;
} ETH_RRING_MAP2_TypeDef;

/* 0x78
    2:0     R/W    ext3_pri0_route             3'h0
    3       R/W    reserved3                   1'h0
    6:4     R/W    ext3_pri1_route             3'h0
    7       R/W    reserved7                   1'h0
    10:8    R/W    ext3_pri2_route             3'h0
    11      R/W    reserved11                  1'h0
    14:12   R/W    ext3_pri3_route             3'h0
    15      R/W    reserved15                  1'h0
    18:16   R/W    ext3_pri4_route             3'h0
    19      R/W    reserved19                  1'h0
    22:20   R/W    ext3_pri5_route             3'h0
    23      R/W    reserved25                  1'h0
    26:24   R/W    ext3_pri6_route             3'h0
    27      R/W    reserved27                  1'h0
    30:28   R/W    ext3_pri7_route             3'h0
    31      R/W    reserved31                  1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t ext3_pri0_route: 3;
        __IO uint32_t reserved_7: 1;
        __IO uint32_t ext3_pri1_route: 3;
        __IO uint32_t reserved_6: 1;
        __IO uint32_t ext3_pri2_route: 3;
        __IO uint32_t reserved_5: 1;
        __IO uint32_t ext3_pri3_route: 3;
        __IO uint32_t reserved_4: 1;
        __IO uint32_t ext3_pri4_route: 3;
        __IO uint32_t reserved_3: 1;
        __IO uint32_t ext3_pri5_route: 3;
        __IO uint32_t reserved_2: 1;
        __IO uint32_t ext3_pri6_route: 3;
        __IO uint32_t reserved_1: 1;
        __IO uint32_t ext3_pri7_route: 3;
        __IO uint32_t reserved_0: 1;
    } b;
} ETH_RRING_MAP3_TypeDef;

/* 0x7C
    2:0     R/W    ext4_pri0_route             3'h0
    3       R/W    reserved3                   1'h0
    6:4     R/W    ext4_pri1_route             3'h0
    7       R/W    reserved7                   1'h0
    10:8    R/W    ext4_pri2_route             3'h0
    11      R/W    reserved11                  1'h0
    14:12   R/W    ext4_pri3_route             3'h0
    15      R/W    reserved15                  1'h0
    18:16   R/W    ext4_pri4_route             3'h0
    19      R/W    reserved19                  1'h0
    22:20   R/W    ext4_pri5_route             3'h0
    23      R/W    reserved25                  1'h0
    26:24   R/W    ext4_pri6_route             3'h0
    27      R/W    reserved27                  1'h0
    30:28   R/W    ext4_pri7_route             3'h0
    31      R/W    reserved31                  1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t ext4_pri0_route: 3;
        __IO uint32_t reserved_7: 1;
        __IO uint32_t ext4_pri1_route: 3;
        __IO uint32_t reserved_6: 1;
        __IO uint32_t ext4_pri2_route: 3;
        __IO uint32_t reserved_5: 1;
        __IO uint32_t ext4_pri3_route: 3;
        __IO uint32_t reserved_4: 1;
        __IO uint32_t ext4_pri4_route: 3;
        __IO uint32_t reserved_3: 1;
        __IO uint32_t ext4_pri5_route: 3;
        __IO uint32_t reserved_2: 1;
        __IO uint32_t ext4_pri6_route: 3;
        __IO uint32_t reserved_1: 1;
        __IO uint32_t ext4_pri7_route: 3;
        __IO uint32_t reserved_0: 1;
    } b;
} ETH_RRING_MAP4_TypeDef;

/* 0x80
    2:0     R/W    ext5_pri0_route             3'h0
    3       R/W    reserved3                   1'h0
    6:4     R/W    ext5_pri1_route             3'h0
    7       R/W    reserved7                   1'h0
    10:8    R/W    ext5_pri2_route             3'h0
    11      R/W    reserved11                  1'h0
    14:12   R/W    ext5_pri3_route             3'h0
    15      R/W    reserved15                  1'h0
    18:16   R/W    ext5_pri4_route             3'h0
    19      R/W    reserved19                  1'h0
    22:20   R/W    ext5_pri5_route             3'h0
    23      R/W    reserved25                  1'h0
    26:24   R/W    ext5_pri6_route             3'h0
    27      R/W    reserved27                  1'h0
    30:28   R/W    ext5_pri7_route             3'h0
    31      R/W    reserved31                  1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t ext5_pri0_route: 3;
        __IO uint32_t reserved_7: 1;
        __IO uint32_t ext5_pri1_route: 3;
        __IO uint32_t reserved_6: 1;
        __IO uint32_t ext5_pri2_route: 3;
        __IO uint32_t reserved_5: 1;
        __IO uint32_t ext5_pri3_route: 3;
        __IO uint32_t reserved_4: 1;
        __IO uint32_t ext5_pri4_route: 3;
        __IO uint32_t reserved_3: 1;
        __IO uint32_t ext5_pri5_route: 3;
        __IO uint32_t reserved_2: 1;
        __IO uint32_t ext5_pri6_route: 3;
        __IO uint32_t reserved_1: 1;
        __IO uint32_t ext5_pri7_route: 3;
        __IO uint32_t reserved_0: 1;
    } b;
} ETH_RRING_MAP5_TypeDef;

/* 0x84
    2:0     R/W    ext6_pri0_route             3'h0
    3       R/W    reserved3                   1'h0
    6:4     R/W    ext6_pri1_route             3'h0
    7       R/W    reserved7                   1'h0
    10:8    R/W    ext6_pri2_route             3'h0
    11      R/W    reserved11                  1'h0
    14:12   R/W    ext6_pri3_route             3'h0
    15      R/W    reserved15                  1'h0
    18:16   R/W    ext6_pri4_route             3'h0
    19      R/W    reserved19                  1'h0
    22:20   R/W    ext6_pri5_route             3'h0
    23      R/W    reserved25                  1'h0
    26:24   R/W    ext6_pri6_route             3'h0
    27      R/W    reserved27                  1'h0
    30:28   R/W    ext6_pri7_route             3'h0
    31      R/W    reserved31                  1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t ext6_pri0_route: 3;
        __IO uint32_t reserved_7: 1;
        __IO uint32_t ext6_pri1_route: 3;
        __IO uint32_t reserved_6: 1;
        __IO uint32_t ext6_pri2_route: 3;
        __IO uint32_t reserved_5: 1;
        __IO uint32_t ext6_pri3_route: 3;
        __IO uint32_t reserved_4: 1;
        __IO uint32_t ext6_pri4_route: 3;
        __IO uint32_t reserved_3: 1;
        __IO uint32_t ext6_pri5_route: 3;
        __IO uint32_t reserved_2: 1;
        __IO uint32_t ext6_pri6_route: 3;
        __IO uint32_t reserved_1: 1;
        __IO uint32_t ext6_pri7_route: 3;
        __IO uint32_t reserved_0: 1;
    } b;
} ETH_RRING_MAP6_TypeDef;

/* 0x90
    31:0    R/W    rxfdp2                      32'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t rxfdp2: 32;
    } b;
} ETH_RXFDP2_TypeDef;

/* 0x94
    11:0    R      rxcdo2                      12'h0
    15:12   R/W    reserved15_12               4'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __I uint16_t rxcdo2: 12;
        __IO uint16_t reserved_0: 4;
    } b;
} ETH_RXCDO2_TypeDef;

/* 0x96
    11:0    R/W    rx_ring_size2               12'h0
    15:12   R/W    reserved15_12               4'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __IO uint16_t rx_ring_size2: 12;
        __IO uint16_t reserved_0: 4;
    } b;
} ETH_RXRINGSIZE2_TypeDef;

/* 0x98
    11:0    R/W    eth_rx_cpu_des_num2         12'h0
    31:12   R/W    reserved31_12               20'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t eth_rx_cpu_des_num2: 12;
        __IO uint32_t reserved_0: 20;
    } b;
} ETH_RX_CPU_DESN2_TypeDef;

/* 0x9C
    11:0    R/W    rx_pse_des_thres_off2       12'h0
    15:12   R/W    reserved15_12               4'h0
    27:16   R/W    rx_pse_des_thres_on2        12'h0
    31:28   R/W    reserved31_28               4'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t rx_pse_des_thres_off2: 12;
        __IO uint32_t reserved_1: 4;
        __IO uint32_t rx_pse_des_thres_on2: 12;
        __IO uint32_t reserved_0: 4;
    } b;
} ETH_RX_DES_THRES2_TypeDef;

/* 0xA0
    31:0    R/W    rxfdp3                      32'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t rxfdp3: 32;
    } b;
} ETH_RXFDP3_TypeDef;

/* 0xA4
    11:0    R      rxcdo3                      12'h0
    15:12   R/W    reserved15_12               4'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __I uint16_t rxcdo3: 12;
        __IO uint16_t reserved_0: 4;
    } b;
} ETH_RXCDO3_TypeDef;

/* 0xA6
    11:0    R/W    rx_ring_size3               12'h0
    15:12   R/W    reserved15_12               4'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __IO uint16_t rx_ring_size3: 12;
        __IO uint16_t reserved_0: 4;
    } b;
} ETH_RXRINGSIZE3_TypeDef;

/* 0xA8
    11:0    R/W    eth_rx_cpu_des_num3         12'h0
    31:12   R/W    reserved31_12               20'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t eth_rx_cpu_des_num3: 12;
        __IO uint32_t reserved_0: 20;
    } b;
} ETH_RX_CPU_DESN3_TypeDef;

/* 0xAC
    11:0    R/W    rx_pse_des_thres_off3       12'h0
    15:12   R/W    reserved15_12               4'h0
    27:16   R/W    rx_pse_des_thres_on3        12'h0
    31:28   R/W    reserved31_28               4'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t rx_pse_des_thres_off3: 12;
        __IO uint32_t reserved_1: 4;
        __IO uint32_t rx_pse_des_thres_on3: 12;
        __IO uint32_t reserved_0: 4;
    } b;
} ETH_RX_DES_THRES3_TypeDef;

/* 0xB0
    31:0    R/W    rxfdp4                      32'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t rxfdp4: 32;
    } b;
} ETH_RXFDP4_TypeDef;

/* 0xB4
    11:0    R      rxcdo4                      12'h0
    15:12   R/W    reserved15_12               4'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __I uint16_t rxcdo4: 12;
        __IO uint16_t reserved_0: 4;
    } b;
} ETH_RXCDO4_TypeDef;

/* 0xB6
    11:0    R/W    rx_ring_size4               12'h0
    15:12   R/W    reserved15_12               4'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __IO uint16_t rx_ring_size4: 12;
        __IO uint16_t reserved_0: 4;
    } b;
} ETH_RXRINGSIZE4_TypeDef;

/* 0xB8
    11:0    R/W    eth_rx_cpu_des_num4         12'h0
    31:12   R/W    reserved31_12               20'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t eth_rx_cpu_des_num4: 12;
        __IO uint32_t reserved_0: 20;
    } b;
} ETH_RX_CPU_DESN4_TypeDef;

/* 0xBC
    11:0    R/W    rx_pse_des_thres_off4       12'h0
    15:12   R/W    reserved15_12               4'h0
    27:16   R/W    rx_pse_des_thres_on4        12'h0
    31:28   R/W    reserved31_28               4'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t rx_pse_des_thres_off4: 12;
        __IO uint32_t reserved_1: 4;
        __IO uint32_t rx_pse_des_thres_on4: 12;
        __IO uint32_t reserved_0: 4;
    } b;
} ETH_RX_DES_THRES4_TypeDef;

/* 0xC0
    31:0    R/W    rxfdp5                      32'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t rxfdp5: 32;
    } b;
} ETH_RXFDP5_TypeDef;

/* 0xC4
    11:0    R      rxcdo5                      12'h0
    15:12   R/W    reserved15_12               4'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __I uint16_t rxcdo5: 12;
        __IO uint16_t reserved_0: 4;
    } b;
} ETH_RXCDO5_TypeDef;

/* 0xC6
    11:0    R/W    rx_ring_size5               12'h0
    15:12   R/W    reserved15_12               4'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __IO uint16_t rx_ring_size5: 12;
        __IO uint16_t reserved_0: 4;
    } b;
} ETH_RXRINGSIZE5_TypeDef;

/* 0xC8
    11:0    R/W    eth_rx_cpu_des_num5         12'h0
    31:12   R/W    reserved31_12               20'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t eth_rx_cpu_des_num5: 12;
        __IO uint32_t reserved_0: 20;
    } b;
} ETH_RX_CPU_DESN5_TypeDef;

/* 0xCC
    11:0    R/W    rx_pse_des_thres_off5       12'h0
    15:12   R/W    reserved15_12               4'h0
    27:16   R/W    rx_pse_des_thres_on5        12'h0
    31:28   R/W    reserved31_28               4'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t rx_pse_des_thres_off5: 12;
        __IO uint32_t reserved_1: 4;
        __IO uint32_t rx_pse_des_thres_on5: 12;
        __IO uint32_t reserved_0: 4;
    } b;
} ETH_RX_DES_THRES5_TypeDef;

/* 0xD0
    31:0    R/W    rxfdp6                      32'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t rxfdp6: 32;
    } b;
} ETH_RXFDP6_TypeDef;

/* 0xD4
    11:0    R      rxcdo6                      12'h0
    15:12   R/W    reserved15_12               4'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __I uint16_t rxcdo6: 12;
        __IO uint16_t reserved_0: 4;
    } b;
} ETH_RXCDO6_TypeDef;

/* 0xD6
    11:0    R/W    rx_ring_size6               12'h0
    15:12   R/W    reserved15_12               4'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __IO uint16_t rx_ring_size6: 12;
        __IO uint16_t reserved_0: 4;
    } b;
} ETH_RXRINGSIZE6_TypeDef;

/* 0xD8
    11:0    R/W    eth_rx_cpu_des_num6         12'h0
    31:12   R/W    reserved31_12               20'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t eth_rx_cpu_des_num6: 12;
        __IO uint32_t reserved_0: 20;
    } b;
} ETH_RX_CPU_DESN6_TypeDef;

/* 0xDC
    11:0    R/W    rx_pse_des_thres_off6       12'h0
    15:12   R/W    reserved15_12               4'h0
    27:16   R/W    rx_pse_des_thres_on6        12'h0
    31:28   R/W    reserved31_28               4'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t rx_pse_des_thres_off6: 12;
        __IO uint32_t reserved_1: 4;
        __IO uint32_t rx_pse_des_thres_on6: 12;
        __IO uint32_t reserved_0: 4;
    } b;
} ETH_RX_DES_THRES6_TypeDef;

/* 0xF0
    31:0    R/W    rxfdp1                      32'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t rxfdp1: 32;
    } b;
} ETH_RXFDP1_TypeDef;

/* 0xF4
    11:0    R      rxcdo1                      12'h0
    15:12   R/W    reserved15_12               4'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __I uint16_t rxcdo1: 12;
        __IO uint16_t reserved_0: 4;
    } b;
} ETH_RXCDO1_TypeDef;

/* 0xF6
    3:0     R/W    rx_ring_size1_3_0           4'h0
    7:4     R/W    reserved7_4                 4'h0
    15:8    R/W    rx_ring_size1_15_8          8'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __IO uint16_t rx_ring_size1_3_0: 4;
        __IO uint16_t reserved_0: 4;
        __IO uint16_t rx_ring_size1_15_8: 8;
    } b;
} ETH_RXRINGSIZE1_TypeDef;

/* 0xFC
    31:0    R/W    smsa                        32'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t smsa: 32;
    } b;
} ETH_SMSA_TypeDef;

/* 0x100
    3:0     R/W    probesignalselect           4'h0
    31:4    R/W    reserved31_4                28'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t probesignalselect: 4;
        __IO uint32_t reserved_0: 28;
    } b;
} ETH_PROBE_SELEXCT_TypeDef;

/* 0x104
    2:0     R      lso_stats                   3'h0
    5:3     R      rx_mring_state              3'h0
    7:6     R      reserved7_6                 2'h1
    31:8    R      reserved31_8                24'h111003
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __I uint32_t lso_stats: 3;
        __I uint32_t rx_mring_state: 3;
        __I uint32_t reserved_1: 2;
        __I uint32_t reserved_0: 24;
    } b;
} ETH_DIAG1_TypeDef;

/* 0x12C
    3:0     R/W    rx_pse_des_thres_off_1      4'h0
    15:4    R/W    reserved15_4                12'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __IO uint16_t rx_pse_des_thres_off_1: 4;
        __IO uint16_t reserved_0: 12;
    } b;
} ETH_RX_PSE_DES_THRES_H_TypeDef;

/* 0x12E
    7:0     R/W    reserved7_0                 8'h0
    12:8    R/W    rxc_dly_sel                 5'h0
    13      R/W    set_d_txc                   1'h0
    15:14   R/W    reserved15_14               2'h0
*/
typedef union
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        __IO uint16_t reserved_1: 8;
        __IO uint16_t rxc_dly_sel: 5;
        __IO uint16_t set_d_txc: 1;
        __IO uint16_t reserved_0: 2;
    } b;
} ETH_CONFIG_CLK_TypeDef;

/* 0x130
    3:0     R/W    rx_pse_des_thres_on1_11_8   4'h0
    7:4     R/W    eth_rx_cpu_des_num1_11_8    4'h0
    15:8    R/W    rx_pse_des_thres_off1_7_0   8'h0
    23:16   R/W    rx_pse_des_thres_on1_7_0    8'h0
    31:24   R/W    eth_rx_cpu_des_num1_7_0     8'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t rx_pse_des_thres_on1_11_8: 4;
        __IO uint32_t eth_rx_cpu_des_num1_11_8: 4;
        __IO uint32_t rx_pse_des_thres_off1_7_0: 8;
        __IO uint32_t rx_pse_des_thres_on1_7_0: 8;
        __IO uint32_t eth_rx_cpu_des_num1_7_0: 8;
    } b;
} ETH_ETHRNTRXCPU_DES_NUM1_TypeDef;

/* 0x134
    0       W      tx_fn1st                    1'h0
    1       W      tx_fn2nd                    1'h0
    2       W      tx_fn3rd                    1'h0
    3       W      tx_fn4th                    1'h0
    4       R/W    te                          1'h0
    5       R/W    re                          1'h0
    7:6     R/W    reg_int_timer_sel           2'h0
    10:8    R/W    rx_int_mitigation_2_0       3'h0
    12:11   R/W    rxfth                       2'h0
    15:13   R/W    rx_pkt_timer_2_0            3'h0
    18:16   R/W    tx_int_mitigation_2_0       3'h0
    20:19   R/W    tsh                         2'h0
    21      R/W    rx_int_mitigation_3         1'h0
    22      R/W    rx_pkt_timer_3              1'h0
    23      R/W    tx_int_mitigation_3         1'h0
    27:24   R/W    tx_pkt_timer                4'h0
    28      R/W    en_early_tx                 1'h0
    29      R/W    max_dma_sel_1               1'h0
    30      R/W    short_des_format            1'h0
    31      R/W    max_dma_sel_0               1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __O uint32_t tx_fn1st: 1;
        __O uint32_t tx_fn2nd: 1;
        __O uint32_t tx_fn3rd: 1;
        __O uint32_t tx_fn4th: 1;
        __IO uint32_t te: 1;
        __IO uint32_t re: 1;
        __IO uint32_t reg_int_timer_sel: 2;
        __IO uint32_t rx_int_mitigation_2_0: 3;
        __IO uint32_t rxfth: 2;
        __IO uint32_t rx_pkt_timer_2_0: 3;
        __IO uint32_t tx_int_mitigation_2_0: 3;
        __IO uint32_t tsh: 2;
        __IO uint32_t rx_int_mitigation_3: 1;
        __IO uint32_t rx_pkt_timer_3: 1;
        __IO uint32_t tx_int_mitigation_3: 1;
        __IO uint32_t tx_pkt_timer: 4;
        __IO uint32_t en_early_tx: 1;
        __IO uint32_t max_dma_sel_1: 1;
        __IO uint32_t short_des_format: 1;
        __IO uint32_t max_dma_sel_0: 1;
    } b;
} ETH_ETHER_IO_CMD_TypeDef;

/* 0x138
    0       R/W    txq1_h                      1'h0
    1       R/W    txq2_h                      1'h0
    2       R/W    txq3_h                      1'h0
    3       R/W    txq4_h                      1'h0
    4       R/W    txq5_h                      1'h0
    7:5     R      reserved7_5                 3'h0
    8       R/W    tx_fn5th                    1'h0
    13:9    R/W    reserved13_9                5'h0
    15:14   R/W    tx_hl_pri_sel               2'h0
    16      R/W    rxring1                     1'h0
    17      R/W    rxring2                     1'h0
    18      R/W    rxring3                     1'h0
    19      R/W    rxring4                     1'h0
    20      R/W    rxring5                     1'h0
    21      R/W    rxring6                     1'h0
    23:22   R/W    reserved23_22               2'h0
    24      R/W    en_4_gbyte                  1'h1
    25      R/W    en_rx_mring                 1'h0
    26      R/W    rx_ok_int_msk_128_byte      1'h1
    27      R/W    en_precise_dma              1'h0
    30:28   R/W    dsc_format_extra            3'h3
    31      R/W    cf_orgcol_sel               1'h1
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t txq1_h: 1;
        __IO uint32_t txq2_h: 1;
        __IO uint32_t txq3_h: 1;
        __IO uint32_t txq4_h: 1;
        __IO uint32_t txq5_h: 1;
        __I uint32_t reserved_2: 3;
        __IO uint32_t tx_fn5th: 1;
        __IO uint32_t reserved_1: 5;
        __IO uint32_t tx_hl_pri_sel: 2;
        __IO uint32_t rxring1: 1;
        __IO uint32_t rxring2: 1;
        __IO uint32_t rxring3: 1;
        __IO uint32_t rxring4: 1;
        __IO uint32_t rxring5: 1;
        __IO uint32_t rxring6: 1;
        __IO uint32_t reserved_0: 2;
        __IO uint32_t en_4_gbyte: 1;
        __IO uint32_t en_rx_mring: 1;
        __IO uint32_t rx_ok_int_msk_128_byte: 1;
        __IO uint32_t en_precise_dma: 1;
        __IO uint32_t dsc_format_extra: 3;
        __IO uint32_t cf_orgcol_sel: 1;
    } b;
} ETH_IO_CMD1_TypeDef;

/* 0x13C
    0       R/W    wol_cmd                     1'h0
    1       R      wol_sts                     1'h0
    2       R      wol_pme                     1'h0
    3       R/W    wol_imr                     1'h0
    31:4    R      reserved31_4                28'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t wol_cmd: 1;
        __I uint32_t wol_sts: 1;
        __I uint32_t wol_pme: 1;
        __IO uint32_t wol_imr: 1;
        __I uint32_t reserved_0: 28;
    } b;
} ETH_WOL_TypeDef;

/* ================================================================================ */
/* ================          ETH CPU Registers_Structures         ================ */
/* ================================================================================ */
/**
  * @brief ETH_CPU
  */
typedef struct
{
    __IO ETH_TXFDP1_TypeDef  ETH_TXFDP1;                              /*!< 0x00 */
    __IO ETH_TXCDO1_TypeDef  ETH_TXCDO1;                              /*!< 0x04 */
    __IO uint16_t  RSVD0[5];                                    /*!< 0x06 */
    __IO ETH_TXFDP2_TypeDef  ETH_TXFDP2;                              /*!< 0x10 */
    __IO ETH_TXCDO2_TypeDef  ETH_TXCDO2;                              /*!< 0x14 */
    __IO uint16_t  RSVD1[5];                                    /*!< 0x16 */
    __IO ETH_TXFDP3_TypeDef  ETH_TXFDP3;                              /*!< 0x20 */
    __IO ETH_TXCDO3_TypeDef  ETH_TXCDO3;                              /*!< 0x24 */
    __IO uint16_t  RSVD2[5];                                    /*!< 0x26 */
    __IO ETH_TXFDP4_TypeDef  ETH_TXFDP4;                              /*!< 0x30 */
    __IO ETH_TXCDO4_TypeDef  ETH_TXCDO4;                              /*!< 0x34 */
    __IO uint16_t  RSVD3[5];                                    /*!< 0x36 */
    __IO ETH_TXFDP5_TypeDef  ETH_TXFDP5;                              /*!< 0x40 */
    __IO ETH_TXCDO5_TypeDef  ETH_TXCDO5;                              /*!< 0x44 */
    __IO uint16_t  RSVD4[5];                                    /*!< 0x46 */
    __IO uint32_t  RSVD5[8];                                    /*!< 0x50 */
    __IO ETH_RRING_MAP1_TypeDef  ETH_RRING_MAP1;                      /*!< 0x70 */
    __IO ETH_RRING_MAP2_TypeDef  ETH_RRING_MAP2;                      /*!< 0x74 */
    __IO ETH_RRING_MAP3_TypeDef  ETH_RRING_MAP3;                      /*!< 0x78 */
    __IO ETH_RRING_MAP4_TypeDef  ETH_RRING_MAP4;                      /*!< 0x7C */
    __IO ETH_RRING_MAP5_TypeDef  ETH_RRING_MAP5;                      /*!< 0x80 */
    __IO ETH_RRING_MAP6_TypeDef  ETH_RRING_MAP6;                      /*!< 0x84 */
    __IO uint32_t  RSVD6[2];                                    /*!< 0x88 */
    __IO ETH_RXFDP2_TypeDef  ETH_RXFDP2;                              /*!< 0x90 */
    __IO ETH_RXCDO2_TypeDef  ETH_RXCDO2;                              /*!< 0x94 */
    __IO ETH_RXRINGSIZE2_TypeDef  ETH_RXRINGSIZE2;                    /*!< 0x96 */
    __IO ETH_RX_CPU_DESN2_TypeDef  ETH_RX_CPU_DESN2;                  /*!< 0x98 */
    __IO ETH_RX_DES_THRES2_TypeDef  ETH_RX_DES_THRES2;                /*!< 0x9C */
    __IO ETH_RXFDP3_TypeDef  ETH_RXFDP3;                              /*!< 0xA0 */
    __IO ETH_RXCDO3_TypeDef  ETH_RXCDO3;                              /*!< 0xA4 */
    __IO ETH_RXRINGSIZE3_TypeDef  ETH_RXRINGSIZE3;                    /*!< 0xA6 */
    __IO ETH_RX_CPU_DESN3_TypeDef  ETH_RX_CPU_DESN3;                  /*!< 0xA8 */
    __IO ETH_RX_DES_THRES3_TypeDef  ETH_RX_DES_THRES3;                /*!< 0xAC */
    __IO ETH_RXFDP4_TypeDef  ETH_RXFDP4;                              /*!< 0xB0 */
    __IO ETH_RXCDO4_TypeDef  ETH_RXCDO4;                              /*!< 0xB4 */
    __IO ETH_RXRINGSIZE4_TypeDef  ETH_RXRINGSIZE4;                    /*!< 0xB6 */
    __IO ETH_RX_CPU_DESN4_TypeDef  ETH_RX_CPU_DESN4;                  /*!< 0xB8 */
    __IO ETH_RX_DES_THRES4_TypeDef  ETH_RX_DES_THRES4;                /*!< 0xBC */
    __IO ETH_RXFDP5_TypeDef  ETH_RXFDP5;                              /*!< 0xC0 */
    __IO ETH_RXCDO5_TypeDef  ETH_RXCDO5;                              /*!< 0xC4 */
    __IO ETH_RXRINGSIZE5_TypeDef  ETH_RXRINGSIZE5;                    /*!< 0xC6 */
    __IO ETH_RX_CPU_DESN5_TypeDef  ETH_RX_CPU_DESN5;                  /*!< 0xC8 */
    __IO ETH_RX_DES_THRES5_TypeDef  ETH_RX_DES_THRES5;                /*!< 0xCC */
    __IO ETH_RXFDP6_TypeDef  ETH_RXFDP6;                              /*!< 0xD0 */
    __IO ETH_RXCDO6_TypeDef  ETH_RXCDO6;                              /*!< 0xD4 */
    __IO ETH_RXRINGSIZE6_TypeDef  ETH_RXRINGSIZE6;                    /*!< 0xD6 */
    __IO ETH_RX_CPU_DESN6_TypeDef  ETH_RX_CPU_DESN6;                  /*!< 0xD8 */
    __IO ETH_RX_DES_THRES6_TypeDef  ETH_RX_DES_THRES6;                /*!< 0xDC */
    __IO uint32_t  RSVD7[4];                                    /*!< 0xE0 */
    __IO ETH_RXFDP1_TypeDef  ETH_RXFDP1;                              /*!< 0xF0 */
    __IO ETH_RXCDO1_TypeDef  ETH_RXCDO1;                              /*!< 0xF4 */
    __IO ETH_RXRINGSIZE1_TypeDef  ETH_RXRINGSIZE1;                    /*!< 0xF6 */
    __IO uint32_t  RSVD8;                                       /*!< 0xF8 */
    __IO ETH_SMSA_TypeDef  ETH_SMSA;                                  /*!< 0xFC */
    __IO ETH_PROBE_SELEXCT_TypeDef  ETH_PROBE_SELEXCT;                /*!< 0x100 */
    __IO ETH_DIAG1_TypeDef  ETH_DIAG1;                                /*!< 0x104 */
    __IO uint32_t  RSVD9[9];                                    /*!< 0x108 */
    __IO ETH_RX_PSE_DES_THRES_H_TypeDef   ETH_RX_PSE_DES_THRES_H;     /*!< 0x12C */
    __IO ETH_CONFIG_CLK_TypeDef   ETH_CONFIG_CLK;                     /*!< 0x12E */
    __IO ETH_ETHRNTRXCPU_DES_NUM1_TypeDef  ETH_ETHRNTRXCPU_DES_NUM1;  /*!< 0x130 */
    __IO ETH_ETHER_IO_CMD_TypeDef  ETH_ETHER_IO_CMD;                  /*!< 0x134 */
    __IO ETH_IO_CMD1_TypeDef  ETH_IO_CMD1;                            /*!< 0x138 */
    __IO ETH_WOL_TypeDef  ETH_WOL;                                    /*!< 0x13C */
} ETH_CPU_TypeDef;

#define ETH_CPU                        ((ETH_CPU_TypeDef *) (ETHERNET_CFG_BASE + 0x1300))

#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* RTL_ETHERNET_CPU_REG_H */
