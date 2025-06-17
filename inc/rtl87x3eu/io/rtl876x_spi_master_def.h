/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_spi_master_def.h
* @brief     SPI Master Structures Section
* @details
* @author    qinyuan_hu
* @date      2024-06-13
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef RTL876X_SPI_Master_DEF_H
#define RTL876X_SPI_Master_DEF_H

#include "rtl876x.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/*============================================================================*
 *                          SPI Defines
 *============================================================================*/
#define SPI_TX_FIFO_SIZE                               (16)
#define SPI_RX_FIFO_SIZE                               (16)
#define SPI_SUPPORT_WRAP_MODE                          (0)
#define SPI0_SUPPORT_HS                                (0)
#define SPI_SUPPORT_APH_BRIDGE_FOR_HIGH_SPEED          (1)

#define SPI0_SUPPORT_MASTER_SLAVE                      (0)
#define SPI_SUPPORT_DFS_4BIT_TO_16BIT                  (1)
#define SPI_SUPPORT_CLOCK_SOURCE_CONFIG                (0)
#define SPI_SUPPORT_CLOCK_SOURCE_SWITCH                (1)

/*============================================================================*
 *                          SPI_Master Registers Memory Map
 *============================================================================*/
typedef struct
{
    __IO uint32_t SPI_CTRL0;            /*!< 0x00 */
    __IO uint32_t SPI_CTRL1;            /*!< 0x04 */
    __IO uint32_t SPI_ENR;              /*!< 0x08 */
    __IO uint32_t RSVD_0C;              /*!< 0x0C */
    __IO uint32_t SPI_SER;              /*!< 0x10 */
    __IO uint32_t SPI_BAUDR;            /*!< 0x14 */
    __IO uint32_t SPI_TXFTLR;           /*!< 0x18 */
    __IO uint32_t SPI_RXFTLR;           /*!< 0x1C */
    __IO uint32_t SPI_TXFLR;            /*!< 0x20 */
    __IO uint32_t SPI_RXFLR;            /*!< 0x24 */
    __IO uint32_t SPI_SR;               /*!< 0x28 */
    __IO uint32_t SPI_IMR;              /*!< 0x2C */
    __IO uint32_t SPI_ISR;              /*!< 0x30 */
    __IO uint32_t SPI_RISR;             /*!< 0x34 */
    __IO uint32_t SPI_TXOICR;           /*!< 0x38 */
    __IO uint32_t SPI_RXOICR;           /*!< 0x3C */
    __IO uint32_t SPI_RXUICR;           /*!< 0x40 */
    __IO uint32_t RSVD_044;             /*!< 0x44 */
    __IO uint32_t SPI_ICR;              /*!< 0x48 */
    __IO uint32_t SPI_DMACR;            /*!< 0x4C */
    __IO uint32_t SPI_DMATDLR;          /*!< 0x50 */
    __IO uint32_t SPI_DMARDLR;          /*!< 0x54 */
    __IO uint32_t RSVD_058[2];          /*!< 0x58 */
    __IO uint32_t SPI_DR[1];            /*!< 0x60 */
    __IO uint32_t RSVD_064[35];         /*!< 0x64 */
    __IO uint32_t SPI_RSDR;             /*!< 0xF0 */
} SPI_TypeDef;

/*============================================================================*
 *                          SPI Declaration
 *============================================================================*/

#define SPI0               ((SPI_TypeDef *) SPI0_BASE)
#define SPI1               ((SPI_TypeDef *) SPI1_JDI_BASE)
#define SPI2               ((SPI_TypeDef *) SPI2_BASE)
#define SPI1_HS            ((SPI_TypeDef *) SPI1_HS_BASE)//only SPI1 support AHB bridge
#define SPI_SLAVE          ((SPI_TypeDef *) SPI_SLAVE_BASE)

#define IS_SPIM_PERIPH(PERIPH)        (((PERIPH) == SPI0) || \
                                       ((PERIPH) == SPI2))

#define IS_SPIM_JDI_PERIPH(PERIPH)    (((PERIPH) == SPI1) || \
                                       ((PERIPH) == SPI1_HS))

#define IS_SPIS_PERIPH(PERIPH)        (((PERIPH) == SPI_SLAVE))

/*============================================================================*
 *                          SPI_Master Registers and Field Descriptions
 *============================================================================*/
/* 0x00
   3:0     R/W    DFS             4'h0
   5:4     R/W    FRF             2'b0
   6       R/W    SCPH            1'b0
   7       R/W    SCPOL           1'b0
   9:8     R/W    TMOD            2'b0
   11:10   R      reserved0_11_10 2'b0
   15:12   R      reserved0_15_12 4'b0
   20:16   R      DFS_31          5'h0
   22:21   R      SPI_FRF         2'b0
   23      R      reserved0_23    1'b0
   24      R/W    SSTE            1'b1
   31:25   R      reserved0_31_25 7'b0
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
} SPI_CTRL0_TypeDef;


/* 0x04
   15:0    R/W    NDF             16'h0
   31:16   R      reserved4_31_16 16'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t NDF: 16;
        const uint32_t reserved_0: 16;
    } b;
} SPI_CTRL1_TypeDef;


/* 0x08
   0       R/W    SPI_EN          1'b0
   31:1    R      reserved8_31_1  31'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t SPI_EN: 1;
        const uint32_t reserved_0: 31;
    } b;
} SPI_ENR_TypeDef;


/* 0x10
   0       R/W    SER             1'b0
   31:1    R      reserved10_31_1 31'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t SER: 1;
        const uint32_t reserved_0: 31;
    } b;
} SPI_SER_TypeDef;


/* 0x14
   15:0    R/W    SCKDV           16'b0
   31:16   R      reserved14_31_1616'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t SCKDV: 16;
        const uint32_t reserved_0: 16;
    } b;
} SPI_BAUDR_TypeDef;


/* 0x18
   3:0     R/W    TFT             4'b0
   31:4    R      reserved18_31_4 28'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t TFT: 4;
        const uint32_t reserved_0: 28;
    } b;
} SPI_TXFTLR_TypeDef;


/* 0x1C
   3:0     R/W    RFT             4'b0
   31:4    R      reserved1c_31_4 28'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t RFT: 4;
        const uint32_t reserved_0: 28;
    } b;
} SPI_RXFTLR_TypeDef;


/* 0x20
   4:0     R      TXTFL           5'b0
   31:5    R      reserved20_31_5 27'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t TXTFL: 5;
        const uint32_t reserved_0: 27;
    } b;
} SPI_TXFLR_TypeDef;


/* 0x24
   4:0     R      RXTFL           5'b0
   31:5    R      reserved24_31_5 27'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t RXTFL: 5;
        const uint32_t reserved_0: 27;
    } b;
} SPI_RXFLR_TypeDef;


/* 0x28
   0       R      BUSY            1'b0
   1       R      TFNF            1'b1
   2       R      TFE             1'b1
   3       R      RFNE            1'b0
   4       R      RFF             1'b0
   5       R      reserved28_5    1'b0
   6       R      DCOL            1'b0
   31:7    R      reserved28_31_7 25'h0
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
} SPI_SR_TypeDef;


/* 0x2C
   0       R/W    TXEIM           1'b1
   1       R/W    TXOIM           1'b1
   2       R/W    RXUIM           1'b1
   3       R/W    RXOIM           1'b1
   4       R/W    RXFIM           1'b1
   31:5    R      reserved2c_31_5 27'h0
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
} SPI_IMR_TypeDef;


/* 0x30
   0       R      TXEIS           1'b0
   1       R      TXOIS           1'b0
   2       R      RXUIS           1'b0
   3       R      RXOIS           1'b0
   4       R      RXFIS           1'b0
   31:5    R      reserved30_31_5 27'h0
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
} SPI_ISR_TypeDef;


/* 0x34
   0       R      TXEIR           1'b0
   1       R      TXOIR           1'b0
   2       R      RXUIR           1'b0
   3       R      RXOIR           1'b0
   4       R      RXFIR           1'b0
   31:5    R      reserved34_31_6 27'h0
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
} SPI_RISR_TypeDef;


/* 0x38
   0       R      TXOICR          1'b0
   31:1    R      reserved38_31_1 31'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t TXOICR: 1;
        const uint32_t reserved_0: 31;
    } b;
} SPI_TXOICR_TypeDef;


/* 0x3C
   0       R      RXOICR          1'b0
   31:1    R      reserved3c_31_1 31'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t RXOICR: 1;
        const uint32_t reserved_0: 31;
    } b;
} SPI_RXOICR_TypeDef;


/* 0x40
   0       R      RXUICR          1'b0
   31:1    R      reserved40_31_1 31'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t RXUICR: 1;
        const uint32_t reserved_0: 31;
    } b;
} SPI_RXUICR_TypeDef;


/* 0x48
   0       R      ICR             1'b0
   31:1    R      reserved48_31_1 31'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t ICR: 1;
        const uint32_t reserved_0: 31;
    } b;
} SPI_ICR_TypeDef;


/* 0x4C
   0       R/W    RDMAE           1'b0
   1       R/W    TDMAE           1'b0
   31:2    R      reserved4c_31_2 30'h0
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
} SPI_DMACR_TypeDef;


/* 0x50
   3:0     R/W    DMATDL          4'b0
   31:4    R      reserved50_31_4 28'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t DMATDL: 4;
        const uint32_t reserved_0: 28;
    } b;
} SPI_DMATDLR_TypeDef;


/* 0x54
   3:0     R/W    DMARDL          4'b0
   31:4    R      reserved54_31_4 28'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t DMARDL: 4;
        const uint32_t reserved_0: 28;
    } b;
} SPI_DMARDLR_TypeDef;


/* 0x60
   31:0    R/W    DR              32'b0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t DR: 32;
    } b;
} SPI_DR_TypeDef;


/* 0xF0
   7:0     R/W    RSD             8'b0
   31:8    R      reservedf0_31_8 24'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t RSD: 8;
        const uint32_t reserved_0: 24;
    } b;
} SPI_RSDR_TypeDef;

/* ================================================================================ */
/* ================                   SPI  Constants               ================ */
/* ================================================================================ */
/** \defgroup SPI         SPI
  * \brief
  * \{
  */

/** \defgroup SPI_Exported_Constants SPI Exported Constants
  * \brief
  * \{
  */

/**
 * \defgroup    SPI_Clock_Source SPI Clock Source
 * \{
 * \ingroup     SPI_Exported_Constants
 */
typedef enum
{
    SPI_CLOCK_SOURCE_40M  = 0x0,
    SPI_CLOCK_SOURCE_PLL1 = 0x1,
} SPIClockSrc_TypeDef;

#define IS_SPI_CLK_SOURCE(PERIPH)     (((PERIPH) == SPI_CLOCK_SOURCE_40M) || \
                                       ((PERIPH) == SPI_CLOCK_SOURCE_PLL1))

/** End of SPI_Clock_Source
  * \}
  */

/**
 * \defgroup    SPI_Clock_Divider SPI Clock Divider
 * \{
 * \ingroup     SPI_Exported_Constants
 */
typedef enum
{
    SPI_CLOCK_DIVIDER_1 = 0x0,
    SPI_CLOCK_DIVIDER_2 = 0x1,
    SPI_CLOCK_DIVIDER_4 = 0x2,
    SPI_CLOCK_DIVIDER_8 = 0x3,
    SPI_CLOCK_DIVIDER_3_4 = 0x4,
} SPIClockDiv_TypeDef;

#define IS_SPI_CLK_DIV(DIV) (((DIV) == SPI_CLOCK_DIVIDER_1) || \
                             ((DIV) == SPI_CLOCK_DIVIDER_2) || \
                             ((DIV) == SPI_CLOCK_DIVIDER_4) || \
                             ((DIV) == SPI_CLOCK_DIVIDER_8) || \
                             ((DIV) == SPI_CLOCK_DIVIDER_3_4))

/** End of SPI_Clock_Divider
  * \}
  */

/*============================================================================*
 *                          SPI TYPE/API Wrappers
 *============================================================================*/
#define SPI_DFS_BIT_FIELD               DFS
#define SPI_NDF                         SPI_RXNDF

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* RTL876X_SPI_Master_DEF_H */

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/
