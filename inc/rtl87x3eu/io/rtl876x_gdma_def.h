/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_gdma_def.h
* @brief     GDMA Structures Section
* @details
* @author    colin
* @date      2024-06-07
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef RTL_GDMA_DEF_H
#define RTL_GDMA_DEF_H

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>
#include "rtl876x.h"

#define CHIP_GDMA_CHANNEL_NUM                        (16)

#define GDMA_SUPPORT_GATHER_SCATTER_FUNCTION         (1)
#define GDMA_SUPPORT_INT_HAIF_BLOCK                  (1)
#define GDMA_SUPPORT_INT_HAIF_BLOCK_ALL              (1)
#define GDMA_SUPPORT_TIMER_DMA_EN                    (1)

/*============================================================================*
 *                         GDMA Registers Memory Map
 *============================================================================*/
typedef struct
{
    __IO uint32_t GDMA_SARx;                /*!< 0x00 */
    __I  uint32_t GDMA_CURR_SARx;           /*!< 0x04 */
    __IO uint32_t GDMA_DARx;                /*!< 0x08 */
    __I  uint32_t GDMA_CURR_DARx;           /*!< 0x0C */
    __IO uint32_t GDMA_LLPx;                /*!< 0x10 */
    __IO uint32_t GDMA_RSVD;                /*!< 0x14 */
    __IO uint32_t GDMA_CTL_LOWx;            /*!< 0x18 */
    __IO uint32_t GDMA_CTL_HIGHx;           /*!< 0x1C */
    __IO uint32_t GDMA_RSVD1[8];            /*!< 0x20 ~ 0x3C */
    __IO uint32_t GDMA_CFG_LOWx;            /*!< 0x40 */
    __IO uint32_t GDMA_CFG_HIGHx;           /*!< 0x44 */
    __IO uint32_t GDMA_SGR_LOW;             /*!< 0x48 */
    __IO uint32_t GDMA_SGR_HIGH;            /*!< 0x4C */
    __IO uint32_t GDMA_DSR_LOW;             /*!< 0x50 */
    __IO uint32_t GDMA_DSR_HIGH;            /*!< 0x54 */
} GDMA_ChannelTypeDef;

typedef struct
{
    uint32_t GDMA_RSVD[10];                 /*!< 0x2C0 - 0x2E4 */

    __I uint32_t GDMA_StatusTfr;            /*!< 0x2E8 */
    uint32_t GDMA_RSVD1;
    __I uint32_t GDMA_StatusBlock;          /*!< 0x2F0 */
    __I uint32_t GDMA_StatusHalfBlock;      /*!< 0x2F4 */
    __I uint32_t GDMA_StatusSrcTran;        /*!< 0x2F8 */
    uint32_t GDMA_RSVD3;
    __I uint32_t GDMA_StatusDstTran;        /*!< 0x300 */
    uint32_t GDMA_RSVD4;
    __I uint32_t GDMA_StatuErr;             /*!< 0x308 */
    __I uint32_t GDMA_StatuErrNonSecure;

    __IO uint32_t GDMA_MaskTfr;             /*!< 0x310 */
    uint32_t GDMA_RSVD6;
    __IO uint32_t GDMA_MaskBlock;           /*!< 0x318 */
    __IO uint32_t GDMA_MaskHalfBlock;       /*!< 0x31C */
    __IO uint32_t GDMA_MaskSrcTran;         /*!< 0x320 */
    uint32_t GDMA_RSVD8;
    __IO uint32_t GDMA_MaskDstTran;         /*!< 0x328 */
    uint32_t GDMA_RSVD9;
    __IO uint32_t GDMA_MaskErr;             /*!< 0x330 */
    __IO uint32_t GDMA_MaskErrNonSecure;

    __O uint32_t GDMA_ClearTfr;             /*!< 0x338 */
    uint32_t GDMA_RSVD11;
    __O uint32_t GDMA_ClearBlock;           /*!< 0x340 */
    __O uint32_t GDMA_ClearHalfBlock;       /*!< 0x344 */
    __O uint32_t GDMA_ClearSrcTran;         /*!< 0x348 */
    uint32_t GDMA_RSVD13;
    __O uint32_t GDMA_ClearDstTran;         /*!< 0x350 */
    uint32_t GDMA_RSVD14;
    __O uint32_t GDMA_ClearErr;             /*!< 0x358 */
    __O uint32_t GDMA_ClearErrNonSecure;

    uint32_t GDMA_RSVD16[14];               /*!< 0x360 ~ 0x394 */

    __IO uint32_t GDMA_DmaCfgReg;           /*!< 0x398 */
    uint32_t GDMA_RSVD17;
    __IO uint32_t GDMA_ChEnReg;             /*!< 0x3A0 */
    uint32_t GDMA_RSVD18;

    uint32_t GDMA_RSVD19[4];                /*!< 0x3A8 ~ 0x3B4 */
    __IO uint32_t GDMA_DmaOsNum;            /*!< 0x3B8 */
    uint32_t GDMA_RSVD20;
} GDMA_TypeDef;

/* ================================================================================ */
/* ================                GDMA Declaration                ================ */
/* ================================================================================ */
#define GDMA_REG_BASE                   (RXI350_DMA_CFG_BASE + 0x02c0)
#define GDMA_Channel0_BASE              (RXI350_DMA_CFG_BASE + 0x0000)
#define GDMA_Channel1_BASE              (RXI350_DMA_CFG_BASE + 0x0058)
#define GDMA_Channel2_BASE              (RXI350_DMA_CFG_BASE + 0x00b0)
#define GDMA_Channel3_BASE              (RXI350_DMA_CFG_BASE + 0x0108)
#define GDMA_Channel4_BASE              (RXI350_DMA_CFG_BASE + 0x0160)
#define GDMA_Channel5_BASE              (RXI350_DMA_CFG_BASE + 0x01b8)
#define GDMA_Channel6_BASE              (RXI350_DMA_CFG_BASE + 0x0210)
#define GDMA_Channel7_BASE              (RXI350_DMA_CFG_BASE + 0x0268)
#define GDMA_Channel8_BASE              (RXI350_DMA_CFG_BASE + 0x0400)
#define GDMA_Channel9_BASE              (RXI350_DMA_CFG_BASE + 0x0458)
#define GDMA_Channel10_BASE             (RXI350_DMA_CFG_BASE + 0x04b0)
#define GDMA_Channel11_BASE             (RXI350_DMA_CFG_BASE + 0x0508)
#define GDMA_Channel12_BASE             (RXI350_DMA_CFG_BASE + 0x0560)
#define GDMA_Channel13_BASE             (RXI350_DMA_CFG_BASE + 0x05B8)
#define GDMA_Channel14_BASE             (RXI350_DMA_CFG_BASE + 0x0610)
#define GDMA_Channel15_BASE             (RXI350_DMA_CFG_BASE + 0x0668)

#define GDMA0                           ((GDMA_TypeDef             *) GDMA_REG_BASE)
#define GDMA_BASE                       ((GDMA_TypeDef             *) GDMA_REG_BASE)
#define GDMA_Channel0                   ((GDMA_ChannelTypeDef      *) GDMA_Channel0_BASE)
#define GDMA_Channel1                   ((GDMA_ChannelTypeDef      *) GDMA_Channel1_BASE)
#define GDMA_Channel2                   ((GDMA_ChannelTypeDef      *) GDMA_Channel2_BASE)
#define GDMA_Channel3                   ((GDMA_ChannelTypeDef      *) GDMA_Channel3_BASE)
#define GDMA_Channel4                   ((GDMA_ChannelTypeDef      *) GDMA_Channel4_BASE)
#define GDMA_Channel5                   ((GDMA_ChannelTypeDef      *) GDMA_Channel5_BASE)
#define GDMA_Channel6                   ((GDMA_ChannelTypeDef      *) GDMA_Channel6_BASE)
#define GDMA_Channel7                   ((GDMA_ChannelTypeDef      *) GDMA_Channel7_BASE)
#define GDMA_Channel8                   ((GDMA_ChannelTypeDef      *) GDMA_Channel8_BASE)
#define GDMA_Channel9                   ((GDMA_ChannelTypeDef      *) GDMA_Channel9_BASE)
#define GDMA_Channel10                  ((GDMA_ChannelTypeDef      *) GDMA_Channel10_BASE)
#define GDMA_Channel11                  ((GDMA_ChannelTypeDef      *) GDMA_Channel11_BASE)
#define GDMA_Channel12                  ((GDMA_ChannelTypeDef      *) GDMA_Channel12_BASE)
#define GDMA_Channel13                  ((GDMA_ChannelTypeDef      *) GDMA_Channel13_BASE)
#define GDMA_Channel14                  ((GDMA_ChannelTypeDef      *) GDMA_Channel14_BASE)
#define GDMA_Channel15                  ((GDMA_ChannelTypeDef      *) GDMA_Channel15_BASE)

#define IS_GDMA_PERIPH(PERIPH)     (((PERIPH) == GDMA_Channel0) || \
                                    ((PERIPH) == GDMA_Channel1) || \
                                    ((PERIPH) == GDMA_Channel2) || \
                                    ((PERIPH) == GDMA_Channel3) || \
                                    ((PERIPH) == GDMA_Channel4) || \
                                    ((PERIPH) == GDMA_Channel5) || \
                                    ((PERIPH) == GDMA_Channel6) || \
                                    ((PERIPH) == GDMA_Channel7) || \
                                    ((PERIPH) == GDMA_Channel8) || \
                                    ((PERIPH) == GDMA_Channel9) || \
                                    ((PERIPH) == GDMA_Channel10) || \
                                    ((PERIPH) == GDMA_Channel11) || \
                                    ((PERIPH) == GDMA_Channel12) || \
                                    ((PERIPH) == GDMA_Channel13) || \
                                    ((PERIPH) == GDMA_Channel14) || \
                                    ((PERIPH) == GDMA_Channel15))

#define IS_GDMA_ALL_PERIPH(PERIPH) (IS_GDMA_PERIPH(PERIPH))

/*============================================================================*
 *                         GDMA Registers and Field Descriptions
 *============================================================================*/
/* 0x00
   31:0    R/W    SAR                 undefined
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t SAR: 32;
    } b;
} GDMA_SARx_TypeDef;



/* 0x04
   31:0    R      CURR_SAR            0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t CURR_SAR: 32;
    } b;
} GDMA_CURR_SARx_TypeDef;



/* 0x08
31:0    R/W    DAR                 undefined
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t DAR: 32;
    } b;
} GDMA_DARx_TypeDef;



/* 0x0C
31:0    R      CURR_DAR            0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t CURR_DAR: 32;
    } b;
} GDMA_CURR_DARx_TypeDef;



/* 0x10
1:0     R/W    reserved13          0x0
31:2    R/W    LOC                 0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t reserved_0: 2;
        uint32_t LOC: 30;
    } b;
} GDMA_LLPx_TypeDef;


/* 0x18
0       R/W    INT_EN              0x1
3:1     R/W    DST_TR_WIDTH        0x0
6:4     R/W    SRC_TR_WIDTH        0x0
8:7     R/W    DINC                0x0
10:9    R/W    SINC                0x0
13:11   R/W    DEST_MSIZE          0x1
16:14   R/W    SRC_MSIZE           0x1
17      R/W    SRC_SCATTER_EN      0x0
18      R/W    DST_SCATTER_EN      0x0
19      R/W    reserved20          0x0
22:20   R/W    TT_FC               0x0
26:23   R      reserved18          0x0
27      R/W    LLP_DST_EN          0x0
28      R/W    LLP_SRC_EN          0x0
31:29   R      reserved15          0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t INT_EN: 1;
        uint32_t DST_TR_WIDTH: 3;
        uint32_t SRC_TR_WIDTH: 3;
        uint32_t DINC: 2;
        uint32_t SINC: 2;
        uint32_t DEST_MSIZE: 3;
        uint32_t SRC_MSIZE: 3;
        uint32_t SRC_GATHER_EN: 1;
        uint32_t DST_SCATTER_EN: 1;
        uint32_t reserved_2: 1;
        uint32_t TT_FC: 3;
        const uint32_t reserved_1: 4;
        uint32_t LLP_DST_EN: 1;
        uint32_t LLP_SRC_EN: 1;
        const uint32_t reserved_0: 3;
    } b;
} GDMA_CTL_LOWx_TypeDef;



/* 0x1C
31:0    R      TRANS_DATA_CNT      0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t TRANS_DATA_CNT: 32;
    } b;
} GDMA_CTL_HIGHx_R_TypeDef;



/* 0x1C
19:0    W      BLOCK_TS            0x2
31:20   W      reserved31          0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t BLOCK_TS: 20;
        uint32_t reserved_0: 12;
    } b;
} GDMA_CTL_HIGHx_W_TypeDef;


/* 0x40
0       R      INACTIVE            0x1
1       R      SRC_PCTL_OVER       0x1
2       R      DST_PCTL_OVER       0x1
3       R      reserved45          0x0
7:4     R/W    CH_PRIOR            0x0
8       R/W    CH_SUSP             0x0
9       R      FIFO_EMPTY          0x1
10      R/W    HS_SEL_DST          0x1
11      R/W    HS_SEL_SRC          0x1
17:12   R      reserved39          0x0
18      R/W    DST_HS_POL          0x0
19      R/W    SRC_HS_POL          0x0
29:20   R/W    reserved36          0x0
30      R/W    RELOAD_SRC          0x0
31      R/W    RELOAD_DST          0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t INACTIVE: 1;
        const uint32_t SRC_PCTL_OVER: 1;
        const uint32_t DST_PCTL_OVER: 1;
        const uint32_t reserved_2: 1;
        uint32_t CH_PRIOR: 4;
        uint32_t CH_SUSP: 1;
        const uint32_t FIFO_EMPTY: 1;
        uint32_t HS_SEL_DST: 1;
        uint32_t HS_SEL_SRC: 1;
        const uint32_t reserved_1: 6;
        uint32_t DST_HS_POL: 1;
        uint32_t SRC_HS_POL: 1;
        uint32_t reserved_0: 10;
        uint32_t RELOAD_SRC: 1;
        uint32_t RELOAD_DST: 1;
    } b;
} GDMA_CFG_LOWx_TypeDef;



/* 0x44
0       R/W    reserved63          0x0
1       R/W    reserved62          0x0
2       R      reserved61          0x0
3       R/W    PROTCTL             0x1
6:4     R      reserved59          0x0
10:7    R/W    SRC_PER             0x0
14:11   R/W    DEST_PER            0x0
15      R/W    ExtendedSRC_PER1    0x0
16      R/W    ExtendedDEST_PER1   0x0
17      R/W    ExtendedSRC_PER2    0x0
18      R/W    ExtendedDEST_PER2   0x0
19      R      ExtendedSRC_PER3    0x0
20      R      ExtendedDEST_PER3   0x0
31:21   R      reserved50          0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t reserved_5: 1;
        uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        uint32_t reserved_2: 1;
        const uint32_t reserved_1: 3;
        uint32_t SRC_PER: 4;
        uint32_t DEST_PER: 4;
        uint32_t ExtendedSRC_PER1: 1;
        uint32_t ExtendedDEST_PER1: 1;
        uint32_t ExtendedSRC_PER2: 1;
        uint32_t ExtendedDEST_PER2: 1;
        uint32_t ExtendedSRC_PER3: 1;
        uint32_t ExtendedDEST_PER3: 1;
        const uint32_t reserved_0: 11;
    } b;
} GDMA_CFG_HIGHx_TypeDef;

/* 0x48
19:0    R/W    SGI                 0x0
31:20   R/W    SGC                 0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t SGI: 20;
        uint32_t SGC: 12;
    } b;
} GDMA_SGR_LOW_TypeDef;

/* 0x4C
15:0    R/W    SGSN                0x0
31:16   R      reserved68          0x2
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t SGSN: 16;
        const uint32_t reserved_0: 16;
    } b;
} GDMA_SGR_HIGH_TypeDef;

/* 0x50
19:0    R/W    DSI                 0x0
31:20   R      DSC                 0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t DSI: 20;
        uint32_t DSC: 12;
    } b;
} GDMA_DSR_LOW_TypeDef;

/* 0x54
15:0    R/W    DSSN                0x0
31:16   R      reserved74          0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t DSSN: 16;
        const uint32_t reserved_0: 16;
    } b;
} GDMA_DSR_HIGH_TypeDef;



/* 0x2E8
31:0    R      STATUS              0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t STATUS: 32;
    } b;
} GDMA_StatusTfr_TypeDef;



/* 0x2F0
31:0    R      STATUS              0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t STATUS: 32;
    } b;
} GDMA_StatusBlock_TypeDef;



/* 0x2F8
31:0    R      STATUS              0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t STATUS: 32;
    } b;
} GDMA_StatusSrcTran_TypeDef;



/* 0x300
31:0    R      STATUS              0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t STATUS: 32;
    } b;
} GDMA_StatusDstTran_TypeDef;



/* 0x308
31:0    R      STATUS              0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t STATUS: 32;
    } b;
} GDMA_StatusErr_TypeDef;



/* 0x310
7:0     R/W    INT_MASK_L          0x0
15:8    W      INT_MASK_WE_L       0x0
23:16   R/W    INT_MASK_H          0x0
31:24   W      INT_MASK_WE_H       0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t INT_MASK_L: 8;
        uint32_t INT_MASK_WE_L: 8;
        uint32_t INT_MASK_H: 8;
        uint32_t INT_MASK_WE_H: 8;
    } b;
} GDMA_MaskTfr_TypeDef;



/* 0x318
7:0     R/W    INT_MASK_L          0x0
15:8    W      INT_MASK_WE_L       0x0
23:16   R/W    INT_MASK_H          0x0
31:24   W      INT_MASK_WE_H       0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t INT_MASK_L: 8;
        uint32_t INT_MASK_WE_L: 8;
        uint32_t INT_MASK_H: 8;
        uint32_t INT_MASK_WE_H: 8;
    } b;
} GDMA_MaskBlock_TypeDef;



/* 0x320
7:0     R/W    INT_MASK_L          0x0
15:8    W      INT_MASK_WE_L       0x0
23:16   R/W    INT_MASK_H          0x0
31:24   W      INT_MASK_WE_H       0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t INT_MASK_L: 8;
        uint32_t INT_MASK_WE_L: 8;
        uint32_t INT_MASK_H: 8;
        uint32_t INT_MASK_WE_H: 8;
    } b;
} GDMA_MaskSrcTran_TypeDef;



/* 0x328
7:0     R/W    INT_MASK_L          0x0
15:8    W      INT_MASK_WE_L       0x0
23:16   R/W    INT_MASK_H          0x0
31:24   W      INT_MASK_WE_H       0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t INT_MASK_L: 8;
        uint32_t INT_MASK_WE_L: 8;
        uint32_t INT_MASK_H: 8;
        uint32_t INT_MASK_WE_H: 8;
    } b;
} GDMA_MaskDstTran_TypeDef;



/* 0x330
7:0     R/W    INT_MASK_L          0x0
15:8    W      INT_MASK_WE_L       0x0
23:16   R/W    INT_MASK_H          0x0
31:24   W      INT_MASK_WE_H       0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t INT_MASK_L: 8;
        uint32_t INT_MASK_WE_L: 8;
        uint32_t INT_MASK_H: 8;
        uint32_t INT_MASK_WE_H: 8;
    } b;
} GDMA_MaskErr_TypeDef;



/* 0x338
31:0    W      CLEAR               0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t CLEAR: 32;
    } b;
} GDMA_ClearTfr_TypeDef;



/* 0x340
31:0    W      CLEAR               0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t CLEAR: 32;
    } b;
} GDMA_ClearBlock_TypeDef;



/* 0x348
31:0    W      CLEAR               0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t CLEAR: 32;
    } b;
} GDMA_ClearSrcTran_TypeDef;



/* 0x350
31:0    W      CLEAR               0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t CLEAR: 32;
    } b;
} GDMA_ClearDstTran_TypeDef;



/* 0x358
31:0    W      CLEAR               0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t CLEAR: 32;
    } b;
} GDMA_ClearErr_TypeDef;



/* 0x398
0       R/W    DMAC_EN             0x0
31:1    R      reserved130         0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t DMAC_EN: 1;
        __I uint32_t reserved_0: 31;
    } b;
} GDMA_DmaCfgReg_TypeDef;



/* 0x3A0
7:0     R/W    CH_EN_L             0x0
15:8    W      CH_EN_WE_L          0x0
23:16   R/W    CH_EN_H             0x0
31:24   W      CH_EN_WE_H          0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t CH_EN_L: 8;
        uint32_t CH_EN_WE_L: 8;
        uint32_t CH_EN_H: 8;
        uint32_t CH_EN_WE_H: 8;
    } b;
} GDMA_ChEnReg_TypeDef;



/* 0x3B8
4:0     R    OSR                 0xC
7:5     R    reserved_0          0x0
10:8    R    OSW                 0x4
31:11   R    reserved_1          0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t OSR: 5;
        const uint32_t reserved_0: 3;
        const uint32_t OSW: 3;
        const uint32_t reserved_1: 21;
    } b;
} GDMA_DmaOsNum_TypeDef;


/* ================================================================================ */
/* ================                   GDMA  Constants              ================ */
/* ================================================================================ */

/**
 * \brief    GDMA Channe Type Re_define
 *
 * \ingroup  GDMA_Exported_Constants
 */

#define GDMA_Channel0_IRQn              GDMA0_Channel0_IRQn
#define GDMA_Channel1_IRQn              GDMA0_Channel1_IRQn
#define GDMA_Channel2_IRQn              GDMA0_Channel2_IRQn
#define GDMA_Channel3_IRQn              GDMA0_Channel3_IRQn
#define GDMA_Channel4_IRQn              GDMA0_Channel4_IRQn
#define GDMA_Channel5_IRQn              GDMA0_Channel5_IRQn
#define GDMA_Channel6_IRQn              GDMA0_Channel6_IRQn
#define GDMA_Channel7_IRQn              GDMA0_Channel7_IRQn
#define GDMA_Channel8_IRQn              GDMA0_Channel8_IRQn
#define GDMA_Channel9_IRQn              GDMA0_Channel9_IRQn
#define GDMA_Channel10_IRQn             GDMA0_Channel10_IRQn
#define GDMA_Channel11_IRQn             GDMA0_Channel11_IRQn

#define GDMA_Channel0_Handler           GDMA0_Channel0_Handler
#define GDMA_Channel1_Handler           GDMA0_Channel1_Handler
#define GDMA_Channel2_Handler           GDMA0_Channel2_Handler
#define GDMA_Channel3_Handler           GDMA0_Channel3_Handler
#define GDMA_Channel4_Handler           GDMA0_Channel4_Handler
#define GDMA_Channel5_Handler           GDMA0_Channel5_Handler
#define GDMA_Channel6_Handler           GDMA0_Channel6_Handler
#define GDMA_Channel7_Handler           GDMA0_Channel7_Handler
#define GDMA_Channel8_Handler           GDMA0_Channel8_Handler
#define GDMA_Channel9_Handler           GDMA0_Channel9_Handler
#define GDMA_Channel10_Handler          GDMA0_Channel10_Handler
#define GDMA_Channel11_Handler          GDMA0_Channel11_Handler

/**
  * \defgroup    GDMA_Handshake GDMA Handshake
  * \{
  * \ingroup     GDMA_Exported_Constants
  */


/* GDMA Handshake index for DESCRIPTION (BB2ULTRA) */
#define GDMA_Handshake_UART0_TX                                     (0)
#define GDMA_Handshake_UART0_RX                                     (1)
#define GDMA_Handshake_UART2_TX                                     (2)
#define GDMA_Handshake_UART2_RX                                     (3)
#define GDMA_Handshake_SPI0_TX                                      (4)
#define GDMA_Handshake_SPI0_RX                                      (5)
#define GDMA_Handshake_SPI1_TX                                      (6)
#define GDMA_Handshake_SPI1_RX                                      (7)
#define GDMA_Handshake_I2C0_TX                                      (8)
#define GDMA_Handshake_I2C0_RX                                      (9)
#define GDMA_Handshake_I2C1_TX                                      (10)
#define GDMA_Handshake_I2C1_RX                                      (11)
#define GDMA_Handshake_ADC_RX                                       (12)
#define GDMA_Handshake_AES_TX                                       (13)
#define GDMA_Handshake_AES_RX                                       (14)
#define GDMA_Handshake_UART1_TX                                     (15)
#define GDMA_Handshake_TDM_0_TX                                     (16)
#define GDMA_Handshake_TDM_0_RX                                     (17)
#define GDMA_Handshake_TDM_1_TX                                     (18)
#define GDMA_Handshake_TDM_1_RX                                     (19)
#define GDMA_Handshake_UART1_RX                                     (20)
#define GDMA_Handshake_SPIC0_TX                                     (21)
#define GDMA_Handshake_SPIC0_RX                                     (22)
#define GDMA_Handshake_CANBUS0_RX                                   (23)
#define GDMA_Handshake_CANBUS2_RX                                   (24)
#define GDMA_Handshake_TIM1_CH6_TRX                                 (25)
#define GDMA_Handshake_TIM1_CH4                                     (26)
#define GDMA_Handshake_TIM1_CH5                                     (27)
#define GDMA_Handshake_TIM1_CH6                                     (28)
#define GDMA_Handshake_TIM1_CH7                                     (29)
#define GDMA_Handshake_TIM1_CH8                                     (30)
#define GDMA_Handshake_TIM1_CH9                                     (31)
#define GDMA_Handshake_SPIC1_TX                                     (32)
#define GDMA_Handshake_SPIC1_RX                                     (33)
#define GDMA_Handshake_SPIC2_TX                                     (34)
#define GDMA_Handshake_SPIC2_RX                                     (35)
#define GDMA_Handshake_I2C2_TX                                      (36)
#define GDMA_Handshake_I2C2_RX                                      (37)
#define GDMA_Handshake_SPI2_TX                                      (38)
#define GDMA_Handshake_SPI2_RX                                      (39)
#define GDMA_Handshake_AUDIO_RX                                     (40)
#define GDMA_Handshake_TDM_2_RX                                     (42)
#define GDMA_Handshake_IDU_RX                                       (43)
#define GDMA_Handshake_IDU_TX                                       (44)
#define GDMA_Handshake_SM3                                          (46)
#define GDMA_Handshake_UART3_TX                                     (48)
#define GDMA_Handshake_UART3_RX                                     (49)
#define GDMA_Handshake_UART4_TX                                     (50)
#define GDMA_Handshake_UART4_RX                                     (51)
#define GDMA_Handshake_UART5_TX                                     (52)
#define GDMA_Handshake_UART5_RX                                     (53)
#define GDMA_Handshake_TIM1_CH7_TRX                                 (54)
#define GDMA_Handshake_SPI_SLAVE_TX                                 (55)
#define GDMA_Handshake_SPI_SLAVE_RX                                 (56)
#define GDMA_Handshake_CANBUS1_RX                                   (57)
#define GDMA_Handshake_TIM1_CH9_TRX                                 (58)
#define GDMA_Handshake_TIM1_CH8_TRX                                 (59)
#define GDMA_Handshake_SPIC3_TX                                     (60)
#define GDMA_Handshake_SPIC3_RX                                     (61)
#define GDMA_Handshake_IR_TX                                        (62)
#define GDMA_Handshake_IR_RX                                        (63)

// for compatible with BBPRO
#define GDMA_Handshake_UART_TX           GDMA_Handshake_UART0_TX
#define GDMA_Handshake_UART_RX           GDMA_Handshake_UART0_RX
#define GDMA_Handshake_LOG_UART1_TX      GDMA_Handshake_UART2_TX
#define GDMA_Handshake_LOG_UART1_RX      GDMA_Handshake_UART2_RX
#define GDMA_Handshake_LOG_UART_TX       GDMA_Handshake_UART1_TX
#define GDMA_Handshake_LOG_UART_RX       GDMA_Handshake_UART1_RX

#define IS_GDMA_TransferType(Type) (((Type) == GDMA_Handshake_UART0_TX) || \
                                    ((Type) == GDMA_Handshake_UART0_RX) || \
                                    ((Type) == GDMA_Handshake_UART2_TX) || \
                                    ((Type) == GDMA_Handshake_UART2_RX) || \
                                    ((Type) == GDMA_Handshake_SPI0_TX) || \
                                    ((Type) == GDMA_Handshake_SPI0_RX) || \
                                    ((Type) == GDMA_Handshake_SPI1_TX) || \
                                    ((Type) == GDMA_Handshake_SPI1_RX) || \
                                    ((Type) == GDMA_Handshake_I2C0_TX) || \
                                    ((Type) == GDMA_Handshake_I2C0_RX) || \
                                    ((Type) == GDMA_Handshake_I2C1_TX) || \
                                    ((Type) == GDMA_Handshake_I2C1_RX) || \
                                    ((Type) == GDMA_Handshake_ADC_RX) || \
                                    ((Type) == GDMA_Handshake_AES_TX) || \
                                    ((Type) == GDMA_Handshake_AES_RX) || \
                                    ((Type) == GDMA_Handshake_UART1_TX) || \
                                    ((Type) == GDMA_Handshake_TDM_0_TX) || \
                                    ((Type) == GDMA_Handshake_TDM_0_RX) || \
                                    ((Type) == GDMA_Handshake_TDM_1_TX) || \
                                    ((Type) == GDMA_Handshake_TDM_1_RX) || \
                                    ((Type) == GDMA_Handshake_UART1_RX) || \
                                    ((Type) == GDMA_Handshake_SPIC0_TX) ||\
                                    ((Type) == GDMA_Handshake_SPIC0_RX) ||\
                                    ((Type) == GDMA_Handshake_CANBUS0_RX) ||\
                                    ((Type) == GDMA_Handshake_TIM1_CH4)||\
                                    ((Type) == GDMA_Handshake_TIM1_CH5)||\
                                    ((Type) == GDMA_Handshake_TIM1_CH6)||\
                                    ((Type) == GDMA_Handshake_TIM1_CH7)||\
                                    ((Type) == GDMA_Handshake_TIM1_CH8)||\
                                    ((Type) == GDMA_Handshake_TIM1_CH9)||\
                                    ((Type) == GDMA_Handshake_SPIC1_TX)||\
                                    ((Type) == GDMA_Handshake_SPIC1_RX)||\
                                    ((Type) == GDMA_Handshake_SPIC2_TX)||\
                                    ((Type) == GDMA_Handshake_SPIC2_RX)||\
                                    ((Type) == GDMA_Handshake_I2C2_TX)||\
                                    ((Type) == GDMA_Handshake_I2C2_RX)||\
                                    ((Type) == GDMA_Handshake_SPI2_TX)||\
                                    ((Type) == GDMA_Handshake_SPI2_RX)||\
                                    ((Type) == GDMA_Handshake_AUDIO_RX)||\
                                    ((Type) == GDMA_Handshake_TDM_2_RX)||\
                                    ((Type) == GDMA_Handshake_IDU_RX)||\
                                    ((Type) == GDMA_Handshake_IDU_TX)||\
                                    ((Type) == GDMA_Handshake_SM3)||\
                                    ((Type) == GDMA_Handshake_UART3_TX)||\
                                    ((Type) == GDMA_Handshake_UART3_RX)||\
                                    ((Type) == GDMA_Handshake_UART4_TX)||\
                                    ((Type) == GDMA_Handshake_UART4_RX)||\
                                    ((Type) == GDMA_Handshake_UART5_TX)||\
                                    ((Type) == GDMA_Handshake_UART5_RX)||\
                                    ((Type) == GDMA_Handshake_SPI_SLAVE_TX)||\
                                    ((Type) == GDMA_Handshake_SPI_SLAVE_RX)||\
                                    ((Type) == GDMA_Handshake_CANBUS1_RX)||\
                                    ((Type) == GDMA_Handshake_TIM1_CH6_TRX)||\
                                    ((Type) == GDMA_Handshake_IR_TX)||\
                                    ((Type) == GDMA_Handshake_IR_RX)||\
                                    ((Type) == GDMA_Handshake_CANBUS2_RX)||\
                                    ((Type) == GDMA_Handshake_SPIC3_TX)||\
                                    ((Type) == GDMA_Handshake_SPIC3_RX)||\
                                    ((Type) == GDMA_Handshake_TIM1_CH7_TRX)||\
                                    ((Type) == GDMA_Handshake_TIM1_CH8_TRX)||\
                                    ((Type) == GDMA_Handshake_TIM1_CH9_TRX)||\
                                   )


/** End of GDMA_Handshake
  * \}
  */


/**
 * \brief    GDMA Macro Wrapper
 *
 * \ingroup  GDMA_Exported_Constants
 */
#define GDMA_Source_Cir_Gather_Num  GDMA_GatherCircularStreamingNum
#define GDMA_Dest_Cir_Sca_Num       GDMA_ScatterCircularStreamingNum

#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* RTL_GDMA_DEF_H */

