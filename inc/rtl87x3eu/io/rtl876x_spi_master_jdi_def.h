/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_spi_master_jdi_def.h
* @brief     SPI Master JDI Structures Section
* @details
* @author    qinyuan_hu
* @date      2024-06-13
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef RTL876X_SPI_Master_JDI_DEF_H
#define RTL876X_SPI_Master_JDI_DEF_H

#include "rtl876x.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/*============================================================================*
 *                          SPI Defines
 *============================================================================*/
#define SPI1_TX_FIFO_SIZE                               (32)
#define SPI1_RX_FIFO_SIZE                               (32)
#define SPI1_SUPPORT_WRAP_MODE                          (1)
#define SPI1_SUPPORT_HS                                 (1)
#define SPI1_SUPPORT_APH_BRIDGE_FOR_HIGH_SPEED          (1)
#define SPI1_SUPPORT_JDI                                (1)
//#define SPI1_SUPPORT_CLOCK_SOURCE_SWITCH                (1)

/*============================================================================*
 *                          SPI_Master_JDI Registers Memory Map
 *============================================================================*/
typedef struct
{
    __IO uint32_t SPI_CTRL0;                        /*!< 0x00 */
    __IO uint32_t SPI_CTRL1;                        /*!< 0x04 */
    __IO uint32_t SPI_ENR;                          /*!< 0x08 */
    __IO uint32_t RSVD_0C;                          /*!< 0x0C */
    __IO uint32_t SPI_SER;                          /*!< 0x10 */
    __IO uint32_t SPI_BAUDR;                        /*!< 0x14 */
    __IO uint32_t SPI_TXFTLR;                       /*!< 0x18 */
    __IO uint32_t SPI_RXFTLR;                       /*!< 0x1C */
    __IO uint32_t SPI_TXFLR;                        /*!< 0x20 */
    __IO uint32_t SPI_RXFLR;                        /*!< 0x24 */
    __IO uint32_t SPI_SR;                           /*!< 0x28 */
    __IO uint32_t SPI_IMR;                          /*!< 0x2C */
    __IO uint32_t SPI_ISR;                          /*!< 0x30 */
    __IO uint32_t SPI_RISR;                         /*!< 0x34 */
    __IO uint32_t SPI_TXOICR;                       /*!< 0x38 */
    __IO uint32_t SPI_RXOICR;                       /*!< 0x3C */
    __IO uint32_t SPI_RXUICR;                       /*!< 0x40 */
    __IO uint32_t RSVD_044;                         /*!< 0x44 */
    __IO uint32_t SPI_ICR;                          /*!< 0x48 */
    __IO uint32_t SPI_DMACR;                        /*!< 0x4C */
    __IO uint32_t SPI_DMATDLR;                      /*!< 0x50 */
    __IO uint32_t SPI_DMARDLR;                      /*!< 0x54 */
    __IO uint32_t RSVD_058[2];                      /*!< 0x58 */
    __IO uint32_t SPI_DR[1];                        /*!< 0x60 */
    __IO uint32_t RSVD_064[35];                     /*!< 0x64 */
    __IO uint32_t SPI_RSDR;                         /*!< 0xF0 */
    __IO uint32_t RSVD_0F4[67];                     /*!< 0xF4 */
    __IO uint32_t SPI_WRAP_CTRL;                    /*!< 0x200 */
    __IO uint32_t SPI_WRAP_TXFTLR;                  /*!< 0x204 */
    __IO uint32_t SPI_MST_TXFLR;                    /*!< 0x208 */
    __IO uint32_t SPI_WRAP_SR;                      /*!< 0x20C */
    __IO uint32_t SPI_WRAP_IMR;                     /*!< 0x210 */
    __IO uint32_t SPI_WRAP_ISR;                     /*!< 0x214 */
    __IO uint32_t SPI_WRAP_RISR;                    /*!< 0x218 */
    __IO uint32_t SPI_WRAP_ICR;                     /*!< 0x21C */
    __IO uint32_t SPI_WRAP_DMACR;                   /*!< 0x220 */
    __IO uint32_t SPI_WRAP_DMATDLR;                 /*!< 0x224 */
    __IO uint32_t SPI_WRAP_TXNDF_CNT;               /*!< 0x228 */
    __IO uint32_t SPI_WRAP_CS_DLY;                  /*!< 0x22C */
    __IO uint32_t RSVD_0230[116];                   /*!< 0x230 */
    __IO uint32_t CTL0;                             /*!< 0x400 */
    __IO uint32_t CTL1;                             /*!< 0x404 */
    __IO uint32_t MODE_SEL;                         /*!< 0x408 */
    __IO uint32_t DATA_TRANSFER_DUMMY;              /*!< 0x40C */
    __IO uint32_t DATA_WRITTEN_HORIZONTAL;          /*!< 0x410 */
    __IO uint32_t DATA_WRITTEN_VERTICAL;            /*!< 0x414 */
    __IO uint32_t OUTPUT_COUNTER;                   /*!< 0x418 */
    __IO uint32_t OUTPUT_LINE_COUNTER;              /*!< 0x41C */
    __IO uint32_t RX_FIFO;                          /*!< 0x420 */
    __IO uint32_t TX_FIFO;                          /*!< 0x424 */
    __IO uint32_t RX_FIFO_OFFSET;                   /*!< 0x428 */
    __IO uint32_t TX_FIFO_OFFSET;                   /*!< 0x42C */
    __IO uint32_t RX_FIFO_DMA_THRESHOLD;            /*!< 0x430 */
    __IO uint32_t TX_FIFO_DMA_THRESHOLD;            /*!< 0x434 */
    __IO uint32_t RX_FIFO_INT_THRESHOLD;            /*!< 0x438 */
    __IO uint32_t TX_FIFO_INT_THRESHOLD;            /*!< 0x43C */
    __IO uint32_t INT_ENABLE;                       /*!< 0x440 */
    __IO uint32_t INT_MASK;                         /*!< 0x444 */
    __IO uint32_t INT_RAW_STATUS;                   /*!< 0x448 */
    __IO uint32_t INT_STATUS;                       /*!< 0x44C */
    __IO uint32_t INT_CLEAR;                        /*!< 0x450 */
} SPI_JDI_TypeDef;

/*============================================================================*
 *                          SPI_Master_JDI Declaration
 *============================================================================*/

/*============================================================================*
 *                          SPI_Master_JDI Registers and Field Descriptions
 *============================================================================*/
/* 0x00
   3:0     R/W    DFS                             4'h0
   5:4     R/W    FRF                             2'b0
   6       R/W    SCPH                            1'b0
   7       R/W    SCPOL                           1'b0
   9:8     R/W    TMOD                            2'b0
   11:10   R      reserved0_11_10                 2'b0
   15:12   R      reserved0_15_12                 4'b0
   20:16   R      DFS_31                          5'h0
   22:21   R      SPI_FRF                         2'b0
   23      R      reserved0_23                    1'b0
   24      R/W    SSTE                            1'b1
   31:25   R      reserved0_31_25                 7'b0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t DFS: 4;
        uint32_t FRF: 2;
        uint32_t SCPH: 1;
        uint32_t SCPOL: 1;
        uint32_t TMOD: 2;
        const uint32_t reserved_3: 2;
            const uint32_t reserved_2: 4;
            const uint32_t DFS_31: 5;
            const uint32_t SPI_FRF: 2;
            const uint32_t reserved_1: 1;
            uint32_t SSTE: 1;
            const uint32_t reserved_0: 7;
        } b;
    } SPI_JDI_CTRL0_TypeDef;


    /* 0x04
       15:0    R/W    NDF                             16'h0
       31:16   R      reserved4_31_16                 16'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t NDF: 16;
                const uint32_t reserved_0: 16;
            } b;
        } SPI_JDI_CTRL1_TypeDef;


    /* 0x08
       0       R/W    SPI_EN                          1'b0
       31:1    R      reserved8_31_1                  31'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t SPI_EN: 1;
                const uint32_t reserved_0: 31;
            } b;
        } SPI_JDI_ENR_TypeDef;


    /* 0x10
       0       R/W    SER                             1'b0
       31:1    R      reserved10_31_1                 31'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t SER: 1;
                const uint32_t reserved_0: 31;
            } b;
        } SPI_JDI_SER_TypeDef;


    /* 0x14
       15:0    R/W    SCKDV                           16'b0
       31:16   R      reserved14_31_16                16'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t SCKDV: 16;
                const uint32_t reserved_0: 16;
            } b;
        } SPI_JDI_BAUDR_TypeDef;


    /* 0x18
       4:0     R/W    TFT                             5'b0
       31:5    R      reserved18_31_5                 27'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t TFT: 5;
                const uint32_t reserved_0: 27;
            } b;
        } SPI_JDI_TXFTLR_TypeDef;


    /* 0x1C
       5:0     R/W    RFT                             5'b0
       31:5    R      reserved1c_31_5                 27'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t RFT: 6;
                const uint32_t reserved_0: 26;
            } b;
        } SPI_JDI_RXFTLR_TypeDef;


    /* 0x20
       5:0     R      TXTFL                           6'b0
       31:6    R      reserved20_31_6                 26'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t TXTFL: 6;
                const uint32_t reserved_0: 26;
            } b;
        } SPI_JDI_TXFLR_TypeDef;


    /* 0x24
       6:0     R      RXTFL                           6'b0
       31:6    R      reserved24_31_6                 26'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t RXTFL: 7;
                const uint32_t reserved_0: 25;
            } b;
        } SPI_JDI_RXFLR_TypeDef;


    /* 0x28
       0       R      BUSY                            1'b0
       1       R      TFNF                            1'b1
       2       R      TFE                             1'b1
       3       R      RFNE                            1'b0
       4       R      RFF                             1'b0
       5       R      reserved28_5                    1'b0
       6       R      DCOL                            1'b0
       31:7    R      reserved28_31_7                 25'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t BUSY: 1;
                const uint32_t TFNF: 1;
                const uint32_t TFE: 1;
                const uint32_t RFNE: 1;
                const uint32_t RFF: 1;
                const uint32_t reserved_1: 1;
                const uint32_t DCOL: 1;
                const uint32_t reserved_0: 25;
            } b;
        } SPI_JDI_SR_TypeDef;


    /* 0x2C
       0       R/W    TXEIM                           1'b1
       1       R/W    TXOIM                           1'b1
       2       R/W    RXUIM                           1'b1
       3       R/W    RXOIM                           1'b1
       4       R/W    RXFIM                           1'b1
       31:5    R      reserved2c_31_5                 27'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t TXEIM: 1;
                uint32_t TXOIM: 1;
                uint32_t RXUIM: 1;
                uint32_t RXOIM: 1;
                uint32_t RXFIM: 1;
                const uint32_t reserved_0: 27;
            } b;
        } SPI_JDI_IMR_TypeDef;


    /* 0x30
       0       R      TXEIS                           1'b0
       1       R      TXOIS                           1'b0
       2       R      RXUIS                           1'b0
       3       R      RXOIS                           1'b0
       4       R      RXFIS                           1'b0
       31:5    R      reserved30_31_5                 27'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t TXEIS: 1;
                const uint32_t TXOIS: 1;
                const uint32_t RXUIS: 1;
                const uint32_t RXOIS: 1;
                const uint32_t RXFIS: 1;
                const uint32_t reserved_0: 27;
            } b;
        } SPI_JDI_ISR_TypeDef;


    /* 0x34
       0       R      TXEIR                           1'b0
       1       R      TXOIR                           1'b0
       2       R      RXUIR                           1'b0
       3       R      RXOIR                           1'b0
       4       R      RXFIR                           1'b0
       31:5    R      reserved34_31_6                 27'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t TXEIR: 1;
                const uint32_t TXOIR: 1;
                const uint32_t RXUIR: 1;
                const uint32_t RXOIR: 1;
                const uint32_t RXFIR: 1;
                const uint32_t reserved_0: 27;
            } b;
        } SPI_JDI_RISR_TypeDef;


    /* 0x38
       0       R      TXOICR                          1'b0
       31:1    R      reserved38_31_1                 31'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t TXOICR: 1;
                const uint32_t reserved_0: 31;
            } b;
        } SPI_JDI_TXOICR_TypeDef;


    /* 0x3C
       0       R      RXOICR                          1'b0
       31:1    R      reserved3c_31_1                 31'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t RXOICR: 1;
                const uint32_t reserved_0: 31;
            } b;
        } SPI_JDI_RXOICR_TypeDef;


    /* 0x40
       0       R      RXUICR                          1'b0
       31:1    R      reserved40_31_1                 31'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t RXUICR: 1;
                const uint32_t reserved_0: 31;
            } b;
        } SPI_JDI_RXUICR_TypeDef;


    /* 0x48
       0       R      ICR                             1'b0
       31:1    R      reserved48_31_1                 31'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t ICR: 1;
                const uint32_t reserved_0: 31;
            } b;
        } SPI_JDI_ICR_TypeDef;


    /* 0x4C
       0       R/W    RDMAE                           1'b0
       1       R/W    TDMAE                           1'b0
       31:2    R      reserved4c_31_2                 30'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t RDMAE: 1;
                uint32_t TDMAE: 1;
                const uint32_t reserved_0: 30;
            } b;
        } SPI_JDI_DMACR_TypeDef;


    /* 0x50
       4:0     R/W    DMATDL                          5'b0
       31:5    R      reserved50_31_5                 27'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t DMATDL: 5;
                const uint32_t reserved_0: 27;
            } b;
        } SPI_JDI_DMATDLR_TypeDef;


    /* 0x54
       4:0     R/W    DMARDL                          5'b0
       31:5    R      reserved54_31_5                 27'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t DMARDL: 5;
                const uint32_t reserved_0: 27;
            } b;
        } SPI_JDI_DMARDLR_TypeDef;


    /* 0x60
       31:0    R/W    DR                              32'b0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t DR: 32;
            } b;
        } SPI_JDI_DR_TypeDef;


    /* 0xF0
       7:0     R/W    RSD                             8'b0
       31:8    R      reservedf0_31_8                 24'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t RSD: 8;
                const uint32_t reserved_0: 24;
            } b;
        } SPI_JDI_RSDR_TypeDef;


    /* 0x200
       0       R/W    wrap_ctrl_mode                  1'b0
       1       R/WAC  mst_tx_fifo_en                  1'b0
       2       R/W    tx_fifo_rst                     1'b0
       3       R/W    mosi_pull_en                    1'b0
       4       R/W    mosi_drv_low_en                 1'b0
       5       R/W    cs_inv_en                       1'b0
       6       R/W    Reserved200_6                   1'b0
       15:7    R      reserved200_15_7                9'b0
       31:16   R/W    mst_tx_ndf                      16'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t wrap_ctrl_mode: 1;
                uint32_t mst_tx_fifo_en: 1;
                uint32_t tx_fifo_rst: 1;
                uint32_t mosi_pull_en: 1;
                uint32_t mosi_drv_low_en: 1;
                uint32_t cs_inv_en: 1;
                uint32_t reserved_1: 1;
                const uint32_t reserved_0: 9;
                uint32_t mst_tx_ndf: 16;
            } b;
        } SPI_WRAP_CTRL_TypeDef;


    /* 0x204
       5:0     R/W    tx_fifo_tl                      6'b0
       31:6    R      reserved204_31_6                26'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t tx_fifo_tl: 6;
                const uint32_t reserved_0: 26;
            } b;
        } SPI_WRAP_TXFTLR_TypeDef;


    /* 0x208
       5:0     R      tx_fifo_lv                      6'b0
       31:6    R      reserved208_31_6                26'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t tx_fifo_lv: 6;
                const uint32_t reserved_0: 26;
            } b;
        } SPI_JDI_MST_TXFLR_TypeDef;


    /* 0x20C
       0       R      cs_en                           1'b0
       1       R      tx_fifo_not_full                1'b1
       2       R      tx_fifo_empty                   1'b1
       31:3    R      reserved20c_31_3                29'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t cs_en: 1;
                const uint32_t tx_fifo_not_full: 1;
                const uint32_t tx_fifo_empty: 1;
                const uint32_t reserved_0: 29;
            } b;
        } SPI_WRAP_SR_TypeDef;


    /* 0x210
       0       R/W    tx_fifo_empty_int_m             1'b0
       1       R/W    tx_fifo_ov_int_m                1'b0
       2       R/W    tx_ndf_done_int_m               1'b0
       31:3    R      reserved210_31_3                29'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t tx_fifo_empty_int_m: 1;
                uint32_t tx_fifo_ov_int_m: 1;
                uint32_t tx_ndf_done_int_m: 1;
                const uint32_t reserved_0: 29;
            } b;
        } SPI_WRAP_IMR_TypeDef;


    /* 0x214
       0       R      tx_fifo_empty_int_st            1'b0
       1       R      tx_fifo_ov_int_st               1'b0
       2       R      tx_ndf_done_int_st              1'b0
       31:3    R      reserved214_31_3                29'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t tx_fifo_empty_int_st: 1;
                const uint32_t tx_fifo_ov_int_st: 1;
                const uint32_t tx_ndf_done_int_st: 1;
                const uint32_t reserved_0: 29;
            } b;
        } SPI_WRAP_ISR_TypeDef;


    /* 0x218
       0       R      tx_fifo_empty_r_st              1'b0
       1       R      tx_fifo_ov_r_st                 1'b0
       2       R      tx_ndf_done_r_st                1'b0
       31:3    R      reserved218_31_3                29'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t tx_fifo_empty_r_st: 1;
                const uint32_t tx_fifo_ov_r_st: 1;
                const uint32_t tx_ndf_done_r_st: 1;
                const uint32_t reserved_0: 29;
            } b;
        } SPI_WRAP_RISR_TypeDef;


    /* 0x21C
       0       R      int_clr                         1'b0
       31:1    R      reserved21c_31_1                31'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t int_clr: 1;
                const uint32_t reserved_0: 31;
            } b;
        } SPI_WRAP_ICR_TypeDef;


    /* 0x220
       0       R/W    tx_dma_en                       1'b0
       31:1    R      reserved220_31_1                31'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t tx_dma_en: 1;
                const uint32_t reserved_0: 31;
            } b;
        } SPI_WRAP_DMACR_TypeDef;


    /* 0x224
       5:0     R/W    dma_tx_dl                       6'b0
       31:6    R      reserved224_31_6                26'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t dma_tx_dl: 6;
                const uint32_t reserved_0: 26;
            } b;
        } SPI_WRAP_DMATDLR_TypeDef;


    /* 0x228
       16:0    R      tx_ndf_cnt                      17'b0
       31:17   R      reserved228_31_17               15'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t tx_ndf_cnt: 17;
                const uint32_t reserved_0: 15;
            } b;
        } SPI_WRAP_TXNDF_CNT_TypeDef;


    /* 0x22C
       3:0     R/W    cs_setup_dly                    4'b0
       7:4     R      reserved22c_7_4                 4'b0
       11:8    R/W    cs_hold_dly                     4'b0
       15:12   R      reserved22c_15_12               4'b0
       23:16   R/W    cs_clk_div_sel                  8'b0
       31:24   R      reserved22c_31_24               8'b0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t cs_setup_dly: 4;
                const uint32_t reserved_2: 4;
                uint32_t cs_hold_dly: 4;
                const uint32_t reserved_1: 4;
                uint32_t cs_clk_div_sel: 8;
                const uint32_t reserved_0: 8;
            } b;
        } SPI_WRAP_CS_DLY_TypeDef;


    /* 0x400
       0       R/WAC  rt_jdi_mip_start                1'h0
       31:1    R      reserved_0                      31'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t rt_jdi_mip_start: 1;
                const uint32_t reserved_0: 31;
            } b;
        } CTL0_TypeDef;


    /* 0x404
       0       W1C    output_counter_clear            1'h0
       1       W1C    output_line_counter_clear       1'h0
       2       W1C    tx_fifo_clear                   1'h0
       3       W1C    rx_fifo_clear                   1'h0
       5:4     R/W    output_format                   2'h1
       6       R/W    dummy_bit_for_4bit              1'h0
       8:7     R/W    input_format                    2'h0
       9       R/W    ext_tx_fifo_en                  1'h0
       31:10   R      reserved_0                      23'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t output_counter_clear: 1;
                uint32_t output_line_counter_clear: 1;
                uint32_t tx_fifo_clear: 1;
                uint32_t rx_fifo_clear: 1;
                uint32_t output_format: 2;
                uint32_t dummy_bit_for_4bit: 1;
                uint32_t input_format: 2;
                uint32_t ext_tx_fifo_en: 1;
                const uint32_t reserved_0: 22;
            } b;
        } CTL1_TypeDef;


    /* 0x408
       5:0     R/W    mode_select                     6'h0
       31:6    R      reserved_0                      26'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t mode_select: 6;
                const uint32_t reserved_0: 26;
            } b;
        } MODE_SEL_TypeDef;


    /* 0x40C
       15:0    R/W    data_transfer_dummy_end         16'h0
       21:16   R/W    data_transfer_dummy_mid         6'h0
       31:22   R      reserved_0                      10'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t data_transfer_dummy_end: 16;
                uint32_t data_transfer_dummy_mid: 6;
                const uint32_t reserved_0: 10;
            } b;
        } DATA_TRANSFER_DUMMY_TypeDef;


    /* 0x410
       11:0    R/W    jdi_mip_lcd_width               12'h0
       31:12   R      reserved_0                      20'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t jdi_mip_lcd_width: 12;
                const uint32_t reserved_0: 20;
            } b;
        } DATA_WRITTEN_HORIZONTAL_TypeDef;


    /* 0x414
       9:0     R/W    refresh_start_line_addr         10'h0
       15:10   R      reserved_0                      6'h0
       25:16   R/W    refresh_line_num                10'h0
       27:26   R      reserved_1                      2'h0
       30:28   R/W    refresh_line_addr_incr_num      3'h0
       31      R/W    refresh_line_addr_incr          1'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t refresh_start_line_addr: 10;
                const uint32_t reserved_1: 6;
                uint32_t refresh_line_num: 10;
                const uint32_t reserved_0: 2;
                uint32_t refresh_line_addr_incr_num: 3;
                uint32_t refresh_line_addr_incr: 1;
            } b;
        } DATA_WRITTEN_VERTICAL_TypeDef;


    /* 0x418
       31:0    R      output_counter                  32'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t output_counter: 32;
            } b;
        } OUTPUT_COUNTER_TypeDef;


    /* 0x41C
       9:0     R      output_line_counter             10'h0
       31:10   R      reserved_0                      22'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t output_line_counter: 10;
                const uint32_t reserved_0: 22;
            } b;
        } OUTPUT_LINE_COUNTER_TypeDef;


    /* 0x420
       7:0     R/W    rx_fifo_data                    8'h0
       31:8    R      reserved_0                      24'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t rx_fifo_data: 8;
                const uint32_t reserved_0: 24;
            } b;
        } RX_FIFO_TypeDef;


    /* 0x424
       7:0     R/W    tx_fifo_data                    8'h0
       31:8    R      reserved_0                      24'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t tx_fifo_data: 8;
                const uint32_t reserved_0: 24;
            } b;
        } TX_FIFO_TypeDef;


    /* 0x428
       7:0     R      rx_fifo_offset                  8'h0
       31:8    R      reserved_0                      24'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t rx_fifo_offset: 8;
                const uint32_t reserved_0: 24;
            } b;
        } RX_FIFO_OFFSET_TypeDef;


    /* 0x42C
       7:0     R      tx_fifo_offset                  8'h0
       31:8    R      reserved_0                      24'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t tx_fifo_offset: 8;
                const uint32_t reserved_0: 24;
            } b;
        } TX_FIFO_OFFSET_TypeDef;


    /* 0x430
       7:0     R/W    rx_fifo_dma_threshold           8'h0
       30:8    R      reserved_0                      23'h0
       31      R/W    rx_dma_enable                   1'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t rx_fifo_dma_threshold: 8;
                const uint32_t reserved_0: 23;
                uint32_t rx_dma_enable: 1;
            } b;
        } RX_FIFO_DMA_THRESHOLD_TypeDef;


    /* 0x434
       7:0     R/W    tx_fifo_dma_threshold           8'h0
       30:8    R      reserved_0                      23'h0
       31      R/W    tx_dma_enable                   1'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t tx_fifo_dma_threshold: 8;
                const uint32_t reserved_0: 23;
                uint32_t tx_dma_enable: 1;
            } b;
        } TX_FIFO_DMA_THRESHOLD_TypeDef;


    /* 0x438
       7:0     R/W    rx_fifo_int_threshold           8'h0
       31:8    R      reserved_0                      24'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t rx_fifo_int_threshold: 8;
                const uint32_t reserved_0: 24;
            } b;
        } RX_FIFO_INT_THRESHOLD_TypeDef;


    /* 0x43C
       7:0     R/W    tx_fifo_int_threshold           8'h0
       31:8    R      reserved_0                      24'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t tx_fifo_int_threshold: 8;
                const uint32_t reserved_0: 24;
            } b;
        } TX_FIFO_INT_THRESHOLD_TypeDef;


    /* 0x440
       0       R/W    tx_fifo_underflow_int_en        1'h0
       1       R/W    tx_fifo_threshold_int_en        1'h0
       2       R/W    rx_fifo_overflow_int_en         1'h0
       3       R/W    rx_fifo_threshold_int_en        1'h0
       4       R/W    output_finish_int_en            1'h0
       31:5    R      reserved_0                      27'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t tx_fifo_underflow_int_en: 1;
                uint32_t tx_fifo_threshold_int_en: 1;
                uint32_t rx_fifo_overflow_int_en: 1;
                uint32_t rx_fifo_threshold_int_en: 1;
                uint32_t output_finish_int_en: 1;
                const uint32_t reserved_0: 27;
            } b;
        } INT_ENABLE_TypeDef;


    /* 0x444
       0       R/W    tx_fifo_underflow_int_msk       1'h1
       1       R/W    tx_fifo_threshold_int_msk       1'h1
       2       R/W    rx_fifo_overflow_int_msk        1'h1
       3       R/W    rx_fifo_threshold_int_msk       1'h1
       4       R/W    output_finish_int_msk           1'h1
       31:5    R      reserved_0                      27'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t tx_fifo_underflow_int_msk: 1;
                uint32_t tx_fifo_threshold_int_msk: 1;
                uint32_t rx_fifo_overflow_int_msk: 1;
                uint32_t rx_fifo_threshold_int_msk: 1;
                uint32_t output_finish_int_msk: 1;
                const uint32_t reserved_0: 27;
            } b;
        } INT_MASK_TypeDef;


    /* 0x448
       0       R      tx_fifo_underflow_int_raw_status1'h0
       1       R      tx_fifo_threshold_int_raw_status1'h0
       2       R      rx_fifo_overflow_int_raw_status 1'h0
       3       R      rx_fifo_threshold_int_raw_status1'h0
       4       R      output_finish_int_raw_status    1'h0
       31:5    R      reserved_0                      27'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t tx_fifo_underflow_int_raw_status: 1;
                const uint32_t tx_fifo_threshold_int_raw_status: 1;
                const uint32_t rx_fifo_overflow_int_raw_status: 1;
                const uint32_t rx_fifo_threshold_int_raw_status: 1;
                const uint32_t output_finish_int_raw_status: 1;
                const uint32_t reserved_0: 27;
            } b;
        } INT_RAW_STATUS_TypeDef;


    /* 0x44C
       0       R      tx_fifo_underflow_int_status    1'h0
       1       R      tx_fifo_threshold_int_status    1'h0
       2       R      rx_fifo_overflow_int_status     1'h0
       3       R      rx_fifo_threshold_int_status    1'h0
       4       R      output_finish_int_status        1'h0
       31:5    R      reserved_0                      27'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t tx_fifo_underflow_int_status: 1;
                const uint32_t tx_fifo_threshold_int_status: 1;
                const uint32_t rx_fifo_overflow_int_status: 1;
                const uint32_t rx_fifo_threshold_int_status: 1;
                const uint32_t output_finish_int_status: 1;
                const uint32_t reserved_0: 27;
            } b;
        } INT_STATUS_TypeDef;


    /* 0x450
       0       W1C    tx_fifo_underflow_int_clr       1'h0
       1       W1C    tx_fifo_threshold_int_clr       1'h0
       2       W1C    rx_fifo_overflow_int_clr        1'h0
       3       W1C    rx_fifo_threshold_int_clr       1'h0
       4       W1C    output_finish_int_clr           1'h0
       31:5    R      reserved_0                      27'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t tx_fifo_underflow_int_clr: 1;
                uint32_t tx_fifo_threshold_int_clr: 1;
                uint32_t rx_fifo_overflow_int_clr: 1;
                uint32_t rx_fifo_threshold_int_clr: 1;
                uint32_t output_finish_int_clr: 1;
                const uint32_t reserved_0: 27;
            } b;
        } INT_CLEAR_TypeDef;

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* RTL876X_SPI_Master_JDI_DEF_H */

    /******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/