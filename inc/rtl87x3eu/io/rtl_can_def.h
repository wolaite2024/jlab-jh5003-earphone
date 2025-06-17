/**
*****************************************************************************************
*     Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* \file    rtl_can_def.h
* \brief   CAN related definitions for RTL87x3e.
* \author
* \date    2024-06-26
* \version v1.0
* *************************************************************************************
*/

#ifndef RTL_CAN_DEF_H
#define RTL_CAN_DEF_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "rtl876x.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/*============================================================================*
 *                          CAN Features
 *============================================================================*/
#define CAN_SUPPORT_EXT_LOOPBACK                         (1)
#define CAN_SUPPORT_INT_MSK_STS                          (1)
#define CAN_SUPPORT_SLEEP_MODE                           (1)

#define CAN_DFS_BIT_FIELD_FD_SSP_AUTO   can_reg_0x00.b.can_fd_ssp_auto
#define CAN_DFS_BIT_FIELD_FD_SSP_DCO_2  can_reg_0x14.b.can_fd_ssp_dco_2

/** @addtogroup 87x3eultra_CAN CAN
  * @brief Can driver module
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** \addtogroup 87x3eultra_CAN_Exported_Types
  * \brief
  * \{
  */

/**
* \brief       CAN clock div definition.
*
* \ingroup     87x3eultra_CAN_Exported_Types
*/
typedef enum
{
    CAN_CLK_DIV_1 = 0,      //!< DVI = 1(40M / 1).
    CAN_CLK_DIV_2 = 1,      //!< DVI = 2(40M / 2).
} CANClkDIV_TypeDef;

#define IS_CAN_CLK_DIV(DIV)              (((DIV) == CAN_CLK_DIV_1) || \
                                          ((DIV) == CAN_CLK_DIV_2))

/**
* \brief       CAN frame IDE mask value when in rx mode.
*
* \ingroup     87x3eultra_CAN_Exported_Types
*/
typedef enum
{
    CAN_RX_FRAME_MASK_IDE = 0,
    CAN_RX_FRAME_UNMASK_IDE = 1,
} CANFrameIDEMask_TypeDef;

/**
* \brief       CAN frame RTR mask value when in rx mode.
*
* \ingroup     87x3eultra_CAN_Exported_Types
*/
typedef enum
{
    CAN_RX_FRAME_MASK_RTR = 0,
    CAN_RX_FRAME_UNMASK_RTR = 1,
} CANFrameRTRMask_TypeDef;

/**
* \brief       CAN frame ID mask value when in rx mode.
*
* \ingroup     87x3eultra_CAN_Exported_Types
*/
typedef enum
{
    CAN_RX_FRAME_MASK_ID = 0,
} CANFrameIDMask_TypeDef;

#if (CAN_SUPPORT_SLEEP_MODE == 1)
/**
* \brief       CAN low power clock div definition.
*
* \ingroup     87x3eultra_CAN_Exported_Types
*/
typedef enum
{
    CAN_LOW_CLK_DISABLE = 0,        //!< Disable low power clock.
    CAN_LOW_CLK_DIV_5 = 1,          //!< DVI = 5(40M / 5).
    CAN_LOW_CLK_DIV_10 = 2,         //!< DVI = 10(40M / 10).
    CAN_LOW_CLK_DIV_20 = 3,         //!< DVI = 20(40M / 20).
} CANLowPowerClkDIV_TypeDef;

#define IS_CAN_LOWER_CLK_DIV(DIV)              (((DIV) == CAN_LOW_CLK_DISABLE) || \
                                                ((DIV) == CAN_LOW_CLK_DIV_5) || \
                                                ((DIV) == CAN_LOW_CLK_DIV_10) || \
                                                ((DIV) == CAN_LOW_CLK_DIV_20))
#endif

/** End of 87x3eultra_CAN_Exported_Types
  * \}
  */
/** @} */ /* End of group 87x3eultra_CAN */

/*============================================================================*
 *                          CAN Registers Memory Map
 *============================================================================*/
typedef struct            /*!< CAN Structure */
{
    __IO uint32_t  CAN_CTL;              //!<0X00
    __IO uint32_t  CAN_STS;              //!<0X04
    __IO uint32_t  CAN_FIFO_STS;         //!<0X08
    __IO uint32_t  CAN_BIT_TIMING;       //!<0X0C
    __IO uint32_t  CAN_FD_BIT_TIMING;    //!<0X10
    __IO uint32_t  CAN_FD_SSP_CAL;       //!<0X14
    __IO uint32_t  CAN_INT_EN;           //!<0X18
    __IO uint32_t  CAN_MB_RXINT_EN;      //!<0X1C
    __IO uint32_t  CAN_MB_TXINT_EN;      //!<0X20
    __IO uint32_t  CAN_INT_FLAG;         //!<0X24
    __IO uint32_t  CAN_ERR_STATUS;       //!<0X28
    __IO uint32_t  CAN_ERR_CNT_CTL;      //!<0X2C
    __IO uint32_t  CAN_ERR_CNT_STS;      //!<0X30
    __IO uint32_t  CAN_TX_ERROR_FLAG;    //!<0X34
    __IO uint32_t  CAN_TX_DONE;          //!<0X38
    __IO uint32_t  CAN_RX_DONE;          //!<0X3C
    __IO uint32_t  CAN_TIME_STAMP;       //!<0X40
    __IO uint32_t  CAN_MB_TRIGGER;       //!<0X44
    __IO uint32_t  CAN_RXDMA_MSIZE;      //!<0X48
    __IO uint32_t  CAN_RX_DMA_DATA;      //!<0X4C
    __IO uint32_t  CAN_SLEEP_MODE;       //!<0X50
    __IO uint32_t  CAN_TEST;             //!<0X54
    uint32_t  CAN_RSVD[42];         //!<0X58:0xFC
    __IO uint32_t  CAN_MB0_15_STS[16];   //!<0X100:0x13C
    uint32_t  CAN_RSVD1[48];        //!<0X140:0x1FC
    __IO uint32_t  CAN_MB0_15_CTRL[16];  //!<0X200:0x23C
    uint32_t  CAN_RSVD2[44];        //!<0X240:0x2EC
    __IO uint32_t  CAN_MB_BA_END;        //!<0X2F0
    uint32_t  CAN_RSVD3[3];         //!<0X2F4:0x2FC
    __IO uint32_t  CAN_RAM_DATA15_0[16]; //!<0X300:0x33C
    __IO uint32_t  CAN_RAM_ARB;          //!<0X340
    __IO uint32_t  CAN_RAM_MASK;         //!<0X344
    __IO uint32_t  CAN_RAM_CS;           //!<0X348
    __IO uint32_t  CAN_RAM_CMD;          //!<0X34C
} CAN_TypeDef;

/*============================================================================*
 *                          CAN Declaration
 *============================================================================*/
#define CAN0                       ((CAN_TypeDef *) CAN0_BASE)
#define CAN1                       ((CAN_TypeDef *) CAN1_BASE)
#define CAN2                       ((CAN_TypeDef *) CAN2_BASE)

/*============================================================================*
 *                          CAN Registers and Field Descriptions
 *============================================================================*/
/* 0x000        CAN_CTL
    0       R/W    can_en                  1'h0
    1       R/W/ES bus_on_req              1'h0
    2       R/W    can_tri_sample          1'h0
    3       R/W    auto_re_tx_en           1'h0
    4       R/W    test_mode_en            1'h0
    5       R/W    rx_fifo_en              1'h0
    6       R/W    can_fd_en               1'h0
    7       R/W    can_rxdma_en            1'h0
    8       R/W    can_fd_crc_mode         1'h0
    9       R/W    can_fd_ssp_auto         1'h0
    10      R/W    force_clk_lp_can_on     1'h0
    11      R/W    force_clk_apb_can_on    1'h0
    12      R/W    force_clk_can_on        1'h0
    31:13   R      reserved_dummy00_31_10  19'h0
    */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_en: 1;
        uint32_t bus_on_req: 1;
        uint32_t can_tri_sample: 1;
        uint32_t auto_re_tx_en: 1;
        uint32_t test_mode_en: 1;
        uint32_t rx_fifo_en: 1;
        uint32_t can_fd_en: 1;
        uint32_t can_rxdma_en: 1;
        uint32_t can_fd_crc_mode: 1;
        uint32_t can_fd_ssp_auto: 1;
        uint32_t force_clk_lp_can_on: 1;
        uint32_t force_clk_apb_can_on: 1;
        uint32_t force_clk_can_on: 1;
        const uint32_t reserved_0: 19;
    } b;
} CAN_0x00_TYPE_TypeDef;

/* 0x004        CAN_STS
    0       R      bus_on_state            1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t bus_on_state: 1;
    } b;
} CAN_0x04_TYPE_TypeDef;

/* 0x008        CAN_FIFO_STS
    0       R      fifo_msg_full           1'h0
    1       R      fifo_msg_empty          1'h1
    2       R      fifo_msg_overflow       1'h0
    6:4     R      fifo_msg_lvl            3'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t fifo_msg_full: 1;
        const uint32_t fifo_msg_empty: 1;
        const uint32_t fifo_msg_overflow: 1;
        const uint32_t RESERVED_0:  1;
        const uint32_t fifo_msg_lvl: 3;
    } b;
} CAN_0x08_TYPE_TypeDef;

/* 0x00C        CAN_BIT_TIMING
    7:0     R/W    can_tseg1               8'h0
    15:8    R/W    can_tseg2               8'h0
    18:16   R/W    can_sjw                 3'h0
    31:24   R/W    can_brp                 8'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_tseg1: 8;
        uint32_t can_tseg2: 8;
        uint32_t can_sjw: 3;
        uint32_t RESERVED_0:  5;
        uint32_t can_brp: 8;
    } b;
} CAN_0x0C_TYPE_TypeDef;

/* 0x010        CAN_FD_BIT_TIMING
    7:0     R/W    can_fd_tseg1            8'h0
    15:8    R/W    can_fd_tseg2            8'h0
    18:16   R/W    can_fd_sjw              3'h0
    31:24   R/W    can_fd_brp              8'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_fd_tseg1: 8;
        uint32_t can_fd_tseg2: 8;
        uint32_t can_fd_sjw: 3;
        uint32_t RESERVED_0:  5;
        uint32_t can_fd_brp: 8;
    } b;
} CAN_0x10_TYPE_TypeDef;

/* 0x014        CAN_FD_SSP_CAL
    7:0     R/W/ES can_fd_ssp              8'h0
    15:8    R/W    can_fd_ssp_min          8'h0
    23:16   R/W    can_fd_ssp_dco          8'h0
    31:24   R/W    can_fd_ssp_dco_2        8'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_fd_ssp: 8;
        uint32_t can_fd_ssp_min: 8;
        uint32_t can_fd_ssp_dco: 8;
        uint32_t can_fd_ssp_dco_2: 8;
    } b;
} CAN_0x14_TYPE_TypeDef;

/* 0x018        CAN_INT_EN
    0       R/W    tx_int_en                   1'h0
    1       R/W    rx_int_en                   1'h0
    2       R/W    error_int_en                1'h0
    3       R/W    wakeup_int_en               1'h0
    4       R/W    busoff_int_en               1'h0
    5       R/W    ram_move_done_int_en        1'h0
    15:6    R      reserved_dummy18_15_6       10'h0
    16      R/W    tx_int_mask                 1'h0
    17      R/W    rx_int_mask                 1'h0
    18      R/W    error_int_mask              1'h0
    19      R/W    wakeup_int_mask             1'h0
    20      R/W    busoff_int_mask             1'h0
    21      R/W    ram_move_done_int_mask      1'h0
    31:22   R      reserved_dummy18_31_6       10'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t tx_int_en: 1;
        uint32_t rx_int_en: 1;
        uint32_t error_int_en: 1;
        uint32_t wakeup_int_en: 1;
        uint32_t busoff_int_en: 1;
        uint32_t ram_move_done_int_en: 1;
        const uint32_t reserved_1: 10;
        uint32_t tx_int_mask: 1;
        uint32_t rx_int_mask: 1;
        uint32_t error_int_mask: 1;
        uint32_t wakeup_int_mask: 1;
        uint32_t busoff_int_mask: 1;
        uint32_t ram_move_done_int_mask: 1;
        const uint32_t reserved_0: 10;
    } b;
} CAN_0x18_TYPE_TypeDef;

/* 0x01C        CAN_MB_RXINT_EN
    15:0    R/W    can_mb_rxint_en         16'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_mb_rxint_en: 16;
    } b;
} CAN_0x1C_TYPE_TypeDef;

/* 0x020        CAN_MB_TXINT_EN
    15:0    R/W    can_mb_txint_en         16'h0
    31:16   R      reserved_dummy20_31_16      16'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_mb_txint_en: 16;
        const uint32_t reserved_0: 16;
    } b;
} CAN_0x20_TYPE_TypeDef;

/* 0x024        CAN_INT_FLAG
   0       R/W1C  tx_raw_int_flag             1'h0
   1       R/W1C  rx_raw_int_flag             1'h0
   2       R/W1C  error_raw_int_flag          1'h0
   3       R/W1C  wakeup_raw_int_flag         1'h0
   4       R/W1C  busoff_raw_int_flag         1'h0
   5       R/W1C  ram_move_done_raw_int_flag  1'h0
   15:6    R      reserved_dummy24_15_6       10'h0
   16      R      tx_int_flag                 1'h0
   17      R      rx_int_flag                 1'h0
   18      R      error_int_flag              1'h0
   19      R      wakeup_int_flag             1'h0
   20      R      busoff_int_flag             1'h0
   21      R      ram_move_done_int_flag      1'h0
   31:22   R      reserved_dummy24_31_22      10'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t tx_raw_int_flag: 1;
        uint32_t rx_raw_int_flag: 1;
        uint32_t error_raw_int_flag: 1;
        uint32_t wakeup_raw_int_flag: 1;
        uint32_t busoff_raw_int_flag: 1;
        uint32_t ram_move_done_raw_int_flag: 1;
        const uint32_t reserved_1: 10;
        const uint32_t tx_int_flag: 1;
        const uint32_t rx_int_flag: 1;
        const uint32_t error_int_flag: 1;
        const uint32_t wakeup_int_flag: 1;
        const uint32_t busoff_int_flag: 1;
        const uint32_t ram_move_done_int_flag: 1;
        const uint32_t reserved_0: 10;
    } b;
} CAN_0x24_TYPE_TypeDef;

/* 0x028        CAN_ERR_STATUS
    0       RW1C   can_error_bit0          1'h0
    1       RW1C   can_error_bit1          1'h0
    2       RW1C   can_error_form          1'h0
    3       RW1C   can_error_crc           1'h0
    4       RW1C   can_error_stuff         1'h0
    5       RW1C   can_error_ack           1'h0
    8       RW1C   can_error_tx            1'h0
    9       RW1C   can_error_rx            1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_error_bit0: 1;
        uint32_t can_error_bit1: 1;
        uint32_t can_error_form: 1;
        uint32_t can_error_crc: 1;
        uint32_t can_error_stuff: 1;
        uint32_t can_error_ack: 1;
        uint32_t RESERVED_0:  2;
        uint32_t can_error_tx: 1;
        uint32_t can_error_rx: 1;
    } b;
} CAN_0x28_TYPE_TypeDef;

/* 0x02C        CAN_ERR_CNT_CTL
    0       WA0    tx_err_cnt_clr          1'h0
    1       WA0    rx_err_cnt_clr          1'h0
    16:8    R/W    can_error_warn_th       9'h60
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t tx_err_cnt_clr: 1;
        uint32_t rx_err_cnt_clr: 1;
        uint32_t RESERVED_0:  6;
        uint32_t can_error_warn_th: 9;
    } b;
} CAN_0x2C_TYPE_TypeDef;

/* 0x030        CAN_ERR_CNT_STS
    8:0     R      can_tec                 9'h0
    24:16   R      can_rec                 9'h0
    28      R      can_error_passive       1'h0
    29      R      can_error_busoff        1'h0
    30      R      can_error_warning       1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t can_tec: 9;
        const uint32_t RESERVED_1:  7;
        const uint32_t can_rec: 9;
        const uint32_t RESERVED_0:  3;
        const uint32_t can_error_passive: 1;
        const uint32_t can_error_busoff: 1;
        const uint32_t can_error_warning: 1;
    } b;
} CAN_0x30_TYPE_TypeDef;

/* 0x034        CAN_TX_ERROR_FLAG
    15:0    RW1CB  can_tx_error_flag       16'h0
    31:16   R      reserved_dummy34_31_16      16'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_tx_error_flag: 16;
        const uint32_t reserved_0: 16;
    } b;
} CAN_0x34_TYPE_TypeDef;

/* 0x038        CAN_TX_DONE
    15:0    RW1CB  can_tx_done             16'h0
    31:16   R      reserved_dummy38_31_16      16'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_tx_done: 16;
        const uint32_t reserved_0: 16;
    } b;
} CAN_0x38_TYPE_TypeDef;

/* 0x03C        CAN_RX_DONE
    15:0    RW1CB  can_rx_done             16'h0
    31:16   R      reserved_dummy3c_31_16      16'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_rx_done: 16;
        const uint32_t reserved_0: 16;
    } b;
} CAN_0x3C_TYPE_TypeDef;

/* 0x040        CAN_TIME_STAMP
    15:0    R      can_time_stamp          16'h0
    23:16   R/W    can_time_stamp_div      8'h0
    31      R/W    can_time_stamp_en       1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t can_time_stamp: 16;
        uint32_t can_time_stamp_div: 8;
        uint32_t RESERVED_0:  7;
        uint32_t can_time_stamp_en: 1;
    } b;
} CAN_0x40_TYPE_TypeDef;

/* 0x044        CAN_MB_TRIGGER
    15:0    R/W    tx_trigger_begin        16'h0
    23:16   R/W    tx_trigger_close_offset 8'h0
    24      R/W    tx_trigger_en           1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t tx_trigger_begin: 16;
        uint32_t tx_trigger_close_offset: 8;
        uint32_t tx_trigger_en: 1;
    } b;
} CAN_0x44_TYPE_TypeDef;

/* 0x048        CAN_RXDMA_MSIZE
    13:0    R      can_rxdma_msize         14'h2
    31:14   R      reserved_dummy48_31_14      18'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t can_rxdma_msize: 14;
        const uint32_t reserved_0: 18;
    } b;
} CAN_0x48_TYPE_TypeDef;

/* 0x04C        CAN_RX_DMA_DATA
    31:0    RP     can_rx_dma_data         32'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_rx_dma_data: 32;
    } b;
} CAN_0x4C_TYPE_TypeDef;

/* 0x050        CAN_SLEEP_MODE
    7:0     R/W    can_wakepin_flt_length  8'h1
    8       R/W    can_wakepin_flt_en      1'h0
    9       R/W    can_lp_ck_en            1'h0
    16      R/WAC  can_sleep_req           1'h0
    17      R      can_sleep_state         1'h0
    19:18   R      reserved_dummy50_19_18  2'h0
    20      R/WAC  can_manual_wakeup       1'h0
    31:21   R      reserved_dummy50_31_21  11'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_wakepin_flt_length: 8;
        uint32_t can_wakepin_flt_en: 1;
        uint32_t can_lp_ck_en: 1;
        const uint32_t RESERVED_0: 6;
        uint32_t can_sleep_req: 1;
        const uint32_t can_sleep_state: 1;
        const uint32_t reserved_1: 2;
        uint32_t can_manual_wakeup: 1;
        const uint32_t reserved_0: 11;
    } b;
} CAN_0x50_TYPE_TypeDef;

/* 0x054        CAN_TEST
    1:0     R/W    can_test_cfg            2'h0
    31:2    R      reserved_dummy54_31_2   30'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_test_cfg: 2;
        const uint32_t reserved_0: 30;
    } b;
} CAN_0x54_TYPE_TypeDef;

/* 0x100:0x13C        CAN_MB0_STS:CAN_MB15_STS
    0       R      can_msg_tx_req         1'h0
    1       R      can_msg_tx_done        1'h0
    2       R      can_msg_rx_rdy         1'h0
    3       R      can_msg_rx_vld         1'h0
    31:4    R      reserved_dummy100_31_4 28'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const uint32_t can_msg_tx_req: 1;
        const uint32_t can_msg_tx_done: 1;
        const uint32_t can_msg_rx_rdy: 1;
        const uint32_t can_msg_rx_vld: 1;
        const uint32_t reserved_0: 28;
    } b;
} CAN_0x100_0x13C_TYPE_TypeDef;

/* 0x200:0x23C        CAN_MB0_CTRL:CAN_MB15_CTRL
    8:0     R/W    can_msg@_ba             9'h0
    24      R/W/ES can_msg@_rxdma_en       1'h0
    31:25   R      reserved_dummy200_31_25 7'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_msg_ba: 9;
        uint32_t RESERVED_0:  15;
        uint32_t can_msg_rxdma_en: 1;
        const uint32_t reserved_0: 7;
    } b;
} CAN_0x200_0x23C_TYPE_TypeDef;

/* 0x2F0
   9:0     R/W    can_msg_ba_end              10'h140
   31:10   R      reserved_dummy2f0_31_10     22'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t can_msg_ba_end: 10;
        const uint32_t reserved_0: 22;
    } b;
} CAN_MB_BA_END_TypeDef;

/* 0x300:0x33C        CAN_RAM_FDDATA_15:CAN_RAM_DATA_0
    7:0     R/W/ES can_ram_data0          8'h0
    15:8    R/W/ES can_ram_data1          8'h0
    23:16   R/W/ES can_ram_data2          8'h0
    31:24   R/W/ES can_ram_data3          8'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_ram_data0: 8;
        uint32_t can_ram_data1: 8;
        uint32_t can_ram_data2: 8;
        uint32_t can_ram_data3: 8;
    } b;
} CAN_0x300_0x33C_TYPE_TypeDef;

/* 0x340        CAN_RAM_ARB
    28:0    R/W/ES can_ram_id              29'h0
    29      R/W/ES can_ram_ide             1'h0
    30      R/W/ES can_ram_rtr             1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_ram_id: 29;
        uint32_t can_ram_ide: 1;
        uint32_t can_ram_rtr: 1;
    } b;
} CAN_0x340_TYPE_TypeDef;

/* 0x344        CAN_RAM_MASK
    28:0    R/W/ES can_ram_id_mask         29'h0
    29      R/W/ES can_ram_ide_mask        1'h0
    30      R/W/ES can_ram_rtr_mask        1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_ram_id_mask: 29;
        uint32_t can_ram_ide_mask: 1;
        uint32_t can_ram_rtr_mask: 1;
    } b;
} CAN_0x344_TYPE_TypeDef;

/* 0x348        CAN_RAM_CS
    3:0     R/W/ES can_ram_dlc             4'h0
    4       R/W/ES can_ram_lost            1'h0
    5       R/W/ES can_ram_rxtx            1'h0
    6       R/W/ES can_ram_autoreply       1'h0
    8       R/W/ES can_ram_edl             1'h0
    9       R/W/ES can_ram_brs             1'h0
    10      R/W/ES can_ram_esi             1'h0
    31:16   R/W/ES can_ram_timestamp       16'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_ram_dlc: 4;
        uint32_t can_ram_lost: 1;
        uint32_t can_ram_rxtx: 1;
        uint32_t can_ram_autoreply: 1;
        uint32_t RESERVED_1:  1;
        uint32_t can_ram_edl: 1;
        uint32_t can_ram_brs: 1;
        uint32_t can_ram_esi: 1;
        uint32_t RESERVED_0:  5;
        uint32_t can_ram_timestamp: 16;
    } b;
} CAN_0x348_TYPE_TypeDef;

/* 0x34C        CAN_RAM_CMD
    7:0     R/W    can_ram_acc_num         8'h0
    8       R/W    can_ram_acc_mask        1'h0
    9       R/W    can_ram_acc_cs          1'h0
    10      R/W    can_ram_acc_arb         1'h0
    11:26   R/W    can_ram_acc_data        16'h0
    29      R/W    can_ram_buffer_en       1'h0
    30      R/W    can_ram_dir             1'h0
    31      R/W/ES can_ram_start           1'h0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t can_ram_acc_num: 8;
        uint32_t can_ram_acc_mask: 1;
        uint32_t can_ram_acc_cs: 1;
        uint32_t can_ram_acc_arb: 1;
        uint32_t can_ram_acc_data: 16;
        uint32_t RESERVED_0:  2;
        uint32_t can_ram_buffer_en: 1;
        uint32_t can_ram_dir: 1;
        uint32_t can_ram_start: 1;
    } b;
} CAN_0x34C_TYPE_TypeDef;

typedef struct
{
    __IO uint32_t CAN_RAM_DATA[16];

    union
    {
        __IO uint32_t CAN_RAM_ARB;
        CAN_0x340_TYPE_TypeDef BITS_340;
    } u_340;

    union
    {
        __IO uint32_t CAN_RAM_MASK;
        CAN_0x344_TYPE_TypeDef BITS_344;
    } u_344;

    union
    {
        __IO uint32_t CAN_RAM_CS;
        CAN_0x348_TYPE_TypeDef BITS_348;
    } u_348;

    union
    {
        __IO uint32_t CAN_RAM_CMD;
        CAN_0x34C_TYPE_TypeDef BITS_34C;
    } u_34C;
} CAN_RAM_TypeDef;

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* RTL_CAN_DEF_H */

