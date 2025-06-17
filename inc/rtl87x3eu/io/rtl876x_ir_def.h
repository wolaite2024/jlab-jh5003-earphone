/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl876x_ir_def.h
* \brief    IR related definitions
* \details
* \author   qinyuan_hu
* \date     2024-06-27
* \version  v1.0
* *********************************************************************************************************
*/

#ifndef RTL876X_IR_DEF_H
#define RTL876X_IR_DEF_H

#include "rtl876x.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/*============================================================================*
 *                         IR Defines
 *============================================================================*/
#define IR_SUPPORT_TX_FINISH_INTERRUPT                 (1)
#define IR_SUPPORT_CLOCK_SOURCE_CONFIG                 (1)
#define IR_SUPPORT_CLOCK_SOURCE_DIV_CONFIG             (0)
#define IR_SUPPORT_RAP_MODE                            (0)

/*============================================================================*
 *                          IR Registers Memory Map
 *============================================================================*/
typedef struct
{
    __IO uint32_t IR_CLK_DIV;                   /*!< 0x00 */
    __IO uint32_t IR_TX_CONFIG;                 /*!< 0x04 */
    __IO uint32_t IR_TX_SR;                     /*!< 0x08 */
    __IO uint32_t RSVD_0C;                      /*!< 0x0C */
    __IO uint32_t IR_TX_INT_CLR;                /*!< 0x10 */
    __IO uint32_t IR_TX_FIFO;                   /*!< 0x14 */
    __IO uint32_t IR_RX_CONFIG;                 /*!< 0x18 */
    __IO uint32_t IR_RX_SR;                     /*!< 0x1C */
    __IO uint32_t IR_RX_INT_CLR;                /*!< 0x20 */
    __IO uint32_t IR_RX_CNT_INT_SEL;            /*!< 0x24 */
    __IO uint32_t IR_RX_FIFO;                   /*!< 0x28 */
    __IO uint32_t IR_VERSION;                   /*!< 0x2C */
    __IO uint32_t IR_RX_CUR_CNT;                /*!< 0x30 */
    __IO uint32_t IR_RX_EX_INT;                 /*!< 0x34 */
    __IO uint32_t RSVD_38[2];                   /*!< 0x38 */
    __IO uint32_t IR_CTRL0;                     /*!< 0x40 */
    __IO uint32_t IR_CTRL1;                     /*!< 0x44 */
    __IO uint32_t IR_CTRL2;                     /*!< 0x48 */
    __IO uint32_t IR_RX_LEVEL;                  /*!< 0x4C */
    __IO uint32_t IR_DMA_CONFIG;                /*!< 0x50 */
} IR_TypeDef;


/*============================================================================*
 *                          IR Declaration
 *============================================================================*/
#define IR                      ((IR_TypeDef *) IR_BASE)

#define IS_IR_PERIPH(PERIPH)    ((PERIPH) == IR)

/*============================================================================*
 *                          IR Registers and Field Descriptions
 *============================================================================*/
/* 0x00
   13:0    R/W    ir_clk_div                      14'h0
   31:14   R      reserved31_13                   18'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t ir_clk_div: 14;
        const uint32_t reserved_0: 18;
    } b;
} IR_CLK_DIV_TypeDef;


/* 0x04
   0       R/W    ir_tx_fifo_empty_int_en         1'h0
   1       R/W    ir_tx_fifo_level_int_en         1'h0
   2       R/W    ir_tx_fifo_empty_int_mask       1'h0
   3       R/W    ir_tx_fifo_level_int_mask       1'h0
   4       R/W    ir_tx_fifo_over_int_en          1'h0
   5       R/W    ir_tx_fifo_over_int_mask        1'h0
   6       R/W    ir_tx_idel_state                1'h0
   7       R/W    ir_tx_output_mode               1'h0
   12:8    R/W    ir_tx_fifo_level_th             5'h0
   13      R/W    ir_tx_de_inverse                1'h0
   14      R/W    ir_tx_output_inverse            1'h0
   15      R/W    reg_flag_opt                    1'h0
   29:16   R/W    ir_tx_duty_num                  14'h0
   30      R/W    ir_tx_start                     1'h0
   31      R/W    ir_mode_sel                     1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t ir_tx_fifo_empty_int_en: 1;
        uint32_t ir_tx_fifo_level_int_en: 1;
        uint32_t ir_tx_fifo_empty_int_mask: 1;
        uint32_t ir_tx_fifo_level_int_mask: 1;
        uint32_t ir_tx_fifo_over_int_en: 1;
        uint32_t ir_tx_fifo_over_int_mask: 1;
        uint32_t ir_tx_idel_state: 1;
        uint32_t ir_tx_output_mode: 1;
        uint32_t ir_tx_fifo_level_th: 5;
        uint32_t ir_tx_de_inverse: 1;
        uint32_t ir_tx_output_inverse: 1;
        uint32_t reg_flag_opt: 1;
        uint32_t ir_tx_duty_num: 14;
        uint32_t ir_tx_start: 1;
        uint32_t ir_mode_sel: 1;
    } b;
} IR_TX_CONFIG_TypeDef;


/* 0x08
   0       R      ir_tx_fifo_empty_int_status     1'h0
   1       R      ir_tx_fifo_level_int_status     1'h0
   2       R      ir_tx_fifo_over_int_status      1'h0
   3       R      ir_tx_finish_int_status         1'h0
   4       R      ir_tx_status                    1'h0
   5       R/W    ir_tx_finish_int_en             1'h0
   6       R/W    ir_tx_finish_int_mask           1'h0
   7       R      reserved7                       1'h0
   13:8    R      ir_tx_fifo_offset               6'h0
   14      R      ir_tx_fifo_full                 1'h0
   15      R      ir_tx_fifo_empty                1'h0
   31:16   R      reserved31_16                   16'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t ir_tx_fifo_empty_int_status: 1;
        const uint32_t ir_tx_fifo_level_int_status: 1;
        const uint32_t ir_tx_fifo_over_int_status: 1;
        const uint32_t ir_tx_finish_int_status: 1;
        const uint32_t ir_tx_status: 1;
        uint32_t ir_tx_finish_int_en: 1;
        uint32_t ir_tx_finish_int_mask: 1;
        const uint32_t reserved_1: 1;
        const uint32_t ir_tx_fifo_offset: 6;
        const uint32_t ir_tx_fifo_full: 1;
        const uint32_t ir_tx_fifo_empty: 1;
        const uint32_t reserved_0: 16;
    } b;
} IR_TX_SR_TypeDef;




/* 0x10
   0       W1C    ir_tx_fifo_clr                  1'h0
   1       W1C    ir_tx_fifo_empty_int_clr        1'h0
   2       W1C    ir_tx_fifo_level_int_clr        1'h0
   3       W1C    ir_tx_fifo_over_int_clr         1'h0
   4       W1C    ir_tx_finish_int_clr            1'h0
   31:5    R      reserved31_5                    27'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t ir_tx_fifo_clr: 1;
        uint32_t ir_tx_fifo_empty_int_clr: 1;
        uint32_t ir_tx_fifo_level_int_clr: 1;
        uint32_t ir_tx_fifo_over_int_clr: 1;
        uint32_t ir_tx_finish_int_clr: 1;
        const uint32_t reserved_0: 27;
    } b;
} IR_TX_INT_CLR_TypeDef;


/* 0x14
   31:0    W      ir_tx_fifo                      32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t ir_tx_fifo: 32;
    } b;
} IR_TX_FIFO_TypeDef;


/* 0x18
   0       R/W    ir_rx_fifo_full_int_en          1'h0
   1       R/W    ir_rx_fifo_levle_int_en         1'h0
   2       R/W    ir_rx_cnt_of_int_en             1'h0
   3       R/W    ir_rx_fifo_of_int_en            1'h0
   4       R/W    ir_rx_cnt_thr_int_en            1'h0
   5       R/W    ir_rx_fifo_error_int_en         1'h0
   7:6     R      reserved7_6                     2'h0
   12:8    R/W    ir_rx_fifo_level_th             5'h0
   13      R/W    ir_rx_fifo_discard_set          1'h0
   14      R/W    ir_rx_fifo_full_int_mask        1'h0
   15      R/W    ir_rx_fifo_level_int_mask       1'h0
   16      R/W    ir_rx_cnt_of_int_mask           1'h0
   17      R/W    ir_rx_fifo_of_int_mask          1'h0
   18      R/W    ir_rx_cnt_thr_int_mask          1'h0
   19      R/W    ir_rx_fifo_error_int_mask       1'h0
   20      R      reserved20                      1'h0
   23:21   R/W    ir_rx_filter_stage              3'h0
   25:24   R/W    ir_rx_trigger_mode              2'h0
   26      R/W    ir_rx_man_start                 1'h0
   27      R/W    ir_rx_start_mode                1'h0
   28      R/W    ir_rx_start                     1'h0
   31:29   R      reserved31_29                   3'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t ir_rx_fifo_full_int_en: 1;
        uint32_t ir_rx_fifo_levle_int_en: 1;
        uint32_t ir_rx_cnt_of_int_en: 1;
        uint32_t ir_rx_fifo_of_int_en: 1;
        uint32_t ir_rx_cnt_thr_int_en: 1;
        uint32_t ir_rx_fifo_error_int_en: 1;
        const uint32_t reserved_2: 2;
        uint32_t ir_rx_fifo_level_th: 5;
        uint32_t ir_rx_fifo_discard_set: 1;
        uint32_t ir_rx_fifo_full_int_mask: 1;
        uint32_t ir_rx_fifo_level_int_mask: 1;
        uint32_t ir_rx_cnt_of_int_mask: 1;
        uint32_t ir_rx_fifo_of_int_mask: 1;
        uint32_t ir_rx_cnt_thr_int_mask: 1;
        uint32_t ir_rx_fifo_error_int_mask: 1;
        const uint32_t reserved_1: 1;
        uint32_t ir_rx_filter_stage: 3;
        uint32_t ir_rx_trigger_mode: 2;
        uint32_t ir_rx_man_start: 1;
        uint32_t ir_rx_start_mode: 1;
        uint32_t ir_rx_start: 1;
        const uint32_t reserved_0: 3;
    } b;
} IR_RX_CONFIG_TypeDef;


/* 0x1C
   0       R      ir_rx_fifo_full_int_status      1'h0
   1       R      ir_rx_fifo_level_int_status     1'h0
   2       R      ir_rx_cnt_of_int_status         1'h0
   3       R      ir_rx_fifo_of_int_status        1'h0
   4       R      ir_rx_cnt_thr_int_status        1'h0
   5       R      ir_rx_fifo_error_int_status     1'h0
   6       R      reserved6                       1'h0
   7       R      ir_rx_state                     1'h0
   13:8    R      ir_rx_fifo_offset               6'h0
   15:14   R      reserved15_14                   2'h0
   16      R      ir_rx_fifo_full                 1'h0
   17      R      ir_rx_fifo_empty                1'h0
   31:18   R      reserved31_18                   14'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t ir_rx_fifo_full_int_status: 1;
        const uint32_t ir_rx_fifo_level_int_status: 1;
        const uint32_t ir_rx_cnt_of_int_status: 1;
        const uint32_t ir_rx_fifo_of_int_status: 1;
        const uint32_t ir_rx_cnt_thr_int_status: 1;
        const uint32_t ir_rx_fifo_error_int_status: 1;
        const uint32_t reserved_2: 1;
        const uint32_t ir_rx_state: 1;
        const uint32_t ir_rx_fifo_offset: 6;
        const uint32_t reserved_1: 2;
        const uint32_t ir_rx_fifo_full: 1;
        const uint32_t ir_rx_fifo_empty: 1;
        const uint32_t reserved_0: 14;
    } b;
} IR_RX_SR_TypeDef;


/* 0x20
   0       W1C    ir_rx_fifo_full_int_clr         1'h0
   1       W1C    ir_rx_fifo_level_int_clr        1'h0
   2       W1C    ir_rx_cnt_of_int_clr            1'h0
   3       W1C    ir_rx_fifo_of_int_clr           1'h0
   4       W1C    ir_rx_cnt_thr_int_clr           1'h0
   5       W1C    ir_rx_fifo_error_int_clr        1'h0
   6       W1C    ir_rx_falling_edge_int_clr      1'h0
   7       W1C    ir_rx_rising_edge_int_clr       1'h0
   8       W1C    ir_rx_fifo_clr                  1'h0
   31:9    R      reserved31_9                    23'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t ir_rx_fifo_full_int_clr: 1;
        uint32_t ir_rx_fifo_level_int_clr: 1;
        uint32_t ir_rx_cnt_of_int_clr: 1;
        uint32_t ir_rx_fifo_of_int_clr: 1;
        uint32_t ir_rx_cnt_thr_int_clr: 1;
        uint32_t ir_rx_fifo_error_int_clr: 1;
        uint32_t ir_rx_falling_edge_int_clr: 1;
        uint32_t ir_rx_rising_edge_int_clr: 1;
        uint32_t ir_rx_fifo_clr: 1;
        const uint32_t reserved_0: 23;
    } b;
} IR_RX_INT_CLR_TypeDef;


/* 0x24
   30:0    R/W    ir_rx_cnt_thr                   31'h0
   31      R/W    ir_rx_cnt_thr_trigger_lv        1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t ir_rx_cnt_thr: 31;
        uint32_t ir_rx_cnt_thr_trigger_lv: 1;
    } b;
} IR_RX_CNT_INT_SEL_TypeDef;


/* 0x28
   31:0    R      ir_rx_fifo                      32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t ir_rx_fifo: 32;
    } b;
} IR_RX_FIFO_TypeDef;


/* 0x2C
   31:0    R      rtl_version                     32'h20240530
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t rtl_version: 32;
    } b;
} IR_VERSION_TypeDef;


/* 0x30
   30:0    R      ir_rx_cur_cnt                   31'h0
   31      R      ir_rx_cur_lv                    1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t ir_rx_cur_cnt: 31;
        const uint32_t ir_rx_cur_lv: 1;
    } b;
} IR_RX_CUR_CNT_TypeDef;


/* 0x34
   0       R/W    ir_rx_falling_edge_int_en       1'h0
   1       R/W    ir_rx_rising_edge_int_en        1'h0
   2       R/W    ir_rx_falling_edge_int_mask     1'h0
   3       R/W    ir_rx_rising_edge_int_mask      1'h0
   4       R      ir_rx_falling_edge_int_status   1'h0
   5       R      ir_rx_rising_edge_int_status    1'h0
   31:6    R      reserved31_6                    26'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t ir_rx_falling_edge_int_en: 1;
        uint32_t ir_rx_rising_edge_int_en: 1;
        uint32_t ir_rx_falling_edge_int_mask: 1;
        uint32_t ir_rx_rising_edge_int_mask: 1;
        const uint32_t ir_rx_falling_edge_int_status: 1;
        const uint32_t ir_rx_rising_edge_int_status: 1;
        const uint32_t reserved_0: 26;
    } b;
} IR_RX_EX_INT_TypeDef;




/* 0x40
   7:0     R      tx_time                         8'h0
   13:8    R      tx_num                          6'h0
   14      R      tx_int_n                        1'h0
   15      R/W    reserved15                      1'h0
   31:16   R/W    reg_div_1ms_num                 16'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t tx_time: 8;
        const uint32_t tx_num: 6;
        const uint32_t tx_int_n: 1;
        uint32_t reserved_0: 1;
        uint32_t reg_div_1ms_num: 16;
    } b;
} IR_CTRL0_TypeDef;


/* 0x44
   7:0     R      rx_time                         8'h0
   19:8    R      rx_num                          12'h0
   20      R/W    reg_rx_inv                      1'h0
   21      R      rx_int_n                        1'h0
   31:22   R      reserved31_22                   10'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t rx_time: 8;
        const uint32_t rx_num: 12;
        uint32_t reg_rx_inv: 1;
        const uint32_t rx_int_n: 1;
        const uint32_t reserved_0: 10;
    } b;
} IR_CTRL1_TypeDef;


/* 0x48
   1:0     R/W    reg_dbg_sel                     2'h0
   2       R/W    reg_dbg_tx_data                 1'h0
   3       R/W    reg_dbg_tx_man                  1'h0
   4       R/W    reg_dbg_rx_fifo_sel             1'h0
   5       R/W    reg_dbg_tx_fifo_sel             1'h0
   15:6    R/W    reserved15_6                    10'h0
   29:16   R/W    reg_tx_dummy_num                14'h0
   31:30   R/W    reserved31_30                   2'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_dbg_sel: 2;
        uint32_t reg_dbg_tx_data: 1;
        uint32_t reg_dbg_tx_man: 1;
        uint32_t reg_dbg_rx_fifo_sel: 1;
        uint32_t reg_dbg_tx_fifo_sel: 1;
        uint32_t reserved_1: 10;
        uint32_t reg_tx_dummy_num: 14;
        uint32_t reserved_0: 2;
    } b;
} IR_CTRL2_TypeDef;


/* 0x4C
   0       R      ir_rx_level                     1'h0
   31:1    R      reserved31_1                    31'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t ir_rx_level: 1;
        const uint32_t reserved_0: 31;
    } b;
} IR_RX_LEVEL_TypeDef;


/* 0x50
   0       R/W    reg_dma_tx_en                   1'h0
   6:1     R/W    reg_dma_tx_fifo_th              6'h0
   7       R      reserved7                       1'h0
   8       R/W    reg_dma_rx_en                   1'h0
   14:9    R/W    reg_dma_rx_fifo_th              6'h0
   31:15   R      reserved31_15                   17'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_dma_tx_en: 1;
        uint32_t reg_dma_tx_fifo_th: 6;
        const uint32_t reserved_1: 1;
        uint32_t reg_dma_rx_en: 1;
        uint32_t reg_dma_rx_fifo_th: 6;
        const uint32_t reserved_0: 17;
    } b;
} IR_DMA_CONFIG_TypeDef;

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** \defgroup IR          IR
  * \brief
  * \{
  */

/** \defgroup IR_Exported_Constants IR Exported Constants
  * \brief
  * \{
  */

/**
 * \defgroup    IR_Source_Clock IR Source Clock
 * \{
 * \ingroup     IR_Exported_Constants
 */
typedef enum
{
    IR_SRC_CLOCK_40M,
    IR_SRC_CLOCK_PPL1,
    IR_SRC_CLOCK_PPL2,
} IRSrcClock_TypeDef;

#define IS_IR_SRC_CLOCK(CLK) (((CLK) == IR_SRC_CLOCK_40M) \
                              || ((CLK) == IR_SRC_CLOCK_PPL1) \
                              || ((CLK) == IR_SRC_CLOCK_PPL2))

/** End of IR_Source_Clock
  * \}
  */

/**
 * \defgroup    IR_Clock IR Clock
 * \{
 * \ingroup     IR_Exported_Constants
 */
typedef enum
{
    IR_CLOCK_40M = 40000000,
    IR_CLOCK_100M = 100000000,
} IRClock_TypeDef;

#define IS_IR_CLOCK(CLK) (((CLK) == IR_CLOCK_40M)\
                          || ((CLK) == IR_CLOCK_100M))

#define IS_IR_FREQUENCY_40M(F) (((F) >= 2442) && ((F) <= 2000000))
#define IS_IR_FREQUENCY_100M(F) (((F) >= 6104) && ((F) <= 2000000))

/** End of IR_Clock
  * \}
  */


/** End of IR_Exported_Constants
  * \}
  */

/** End of IR
  * \}
  */


#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* RTL876X_IR_DEF_H */
