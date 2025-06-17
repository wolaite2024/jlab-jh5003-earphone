/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl876x_adc_def.h
* \brief    ADC related definitions
* \details
* \author
* \date
* \version  v1.0
* *********************************************************************************************************
*/

#ifndef RTL876X_ADC_DEF_H
#define RTL876X_ADC_DEF_H

#include <stdint.h>
#include <stdbool.h>
#include "rtl876x.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ================================================================================ */
/* ================                  ADC  Defines                  ================ */
/* ================================================================================ */
/** \defgroup ADC_Exported_Constants ADC Exported Constants
  * \brief
  * \{
  */

/**
 * \defgroup ADC_Defines ADC Defines
 * \{
 * \ingroup  ADC_Exported_Constants
 */
#define CHIP_ADC_CHANNEL_NUM                           (8)
#define CHIP_ADC_SCHEDULE_NUM                          (16)
#define CHIP_ADC_MODE_OFFSET                           (3)
#define ADC_SUPPORT_DMA_EN                             (0)
#define ADC_SUPPORT_VADPIN_MODE                        (1)
#define ADC_SUPPORT_POWER_OFF                          (0)
#define ADC_SUPPORT_MANUAL_MODE                        (1)
#define ADC_SUPPORT_GET_RESULT                         (1)
#define ADC_DRIVER_NOT_MOVE_TO_PATCH                   (0)

/** End of ADC_Defines
  * \}
  */

/** End of ADC_Exported_Constants
  * \}
  */

/*============================================================================*
 *                         ADC Registers Memory Map
 *============================================================================*/
typedef struct
{
    __I  uint32_t ADC_FIFO_READ;              /*!< 0x00 */
    __IO uint32_t ADC_DIG_CTRL;               /*!< 0x04 */
    __IO uint32_t ADC_SCHED_CTRL;             /*!< 0x08 */
    __IO uint32_t ADC_CTRL_INT;               /*!< 0x0C */
    __IO uint16_t ADC_SCHTAB0;                /*!< 0x10 */
    __IO uint16_t ADC_SCHTAB1;
    __IO uint16_t ADC_SCHTAB2;                /*!< 0x14 */
    __IO uint16_t ADC_SCHTAB3;
    __IO uint16_t ADC_SCHTAB4;                /*!< 0x18 */
    __IO uint16_t ADC_SCHTAB5;
    __IO uint16_t ADC_SCHTAB6;                /*!< 0x1C */
    __IO uint16_t ADC_SCHTAB7;
    __IO uint16_t ADC_SCHTAB8;                /*!< 0x20 */
    __IO uint16_t ADC_SCHTAB9;
    __IO uint16_t ADC_SCHTAB10;               /*!< 0x24 */
    __IO uint16_t ADC_SCHTAB11;
    __IO uint16_t ADC_SCHTAB12;               /*!< 0x28 */
    __IO uint16_t ADC_SCHTAB13;
    __IO uint16_t ADC_SCHTAB14;               /*!< 0x2C */
    __IO uint16_t ADC_SCHTAB15;
    __I  uint16_t ADC_SCHD0;                  /*!< 0x30 */
    __I  uint16_t ADC_SCHD1;
    __I  uint16_t ADC_SCHD2;                  /*!< 0x34 */
    __I  uint16_t ADC_SCHD3;
    __I  uint16_t ADC_SCHD4;                  /*!< 0x38 */
    __I  uint16_t ADC_SCHD5;
    __I  uint16_t ADC_SCHD6;                  /*!< 0x3C */
    __I  uint16_t ADC_SCHD7;
    __I  uint16_t ADC_SCHD8;                  /*!< 0x40 */
    __I  uint16_t ADC_SCHD9;
    __I  uint16_t ADC_SCHD10;                 /*!< 0x44 */
    __I  uint16_t ADC_SCHD11;
    __I  uint16_t ADC_SCHD12;                 /*!< 0x48 */
    __I  uint16_t ADC_SCHD13;
    __I  uint16_t ADC_SCHD14;                 /*!< 0x4C */
    __I  uint16_t ADC_SCHD15;
    __IO uint32_t ADC_POW_DATA_DLY_CTRL;      /*!< 0x50 */
    __IO uint32_t ADC_DATA_CLK_CTRL;          /*!< 0x54 */
    __IO uint32_t ADC_ANALOG_CTRL;            /*!< 0x58 */
    __IO uint32_t ADC_TIME_PERIOD;            /*!< 0x5C */
    __O  uint32_t ADC_RTL_VERSION;            /*!< 0x60 */
} ADC_TypeDef;

/*============================================================================*
 *                         ADC Declaration
 *============================================================================*/
#define ADC                ((ADC_TypeDef *) AUX_ADC_BASE)

#define IS_ADC_PERIPH(PERIPH)   ((PERIPH) == ADC)
/*============================================================================*
 *                         ADC Registers and Field Descriptions
 *============================================================================*/
/**
* @brief Analog to digital converter. (ADC)
*/
/* 0x00
   15:0    R      adc_fifo_data_out               16'h0
   27:16   R      reserved00_27_16                12'h0
   31:28   R      adc_schedule_index              4'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t adc_fifo_data_out: 16;
        const uint32_t reserved_0: 12;
        const uint32_t adc_schedule_index: 4;
    } b;
} ADC_FIFO_READ_TypeDef;


/* 0x04
   0       R/W    en_adc_continous_mode           1'h0
   1       W1C    en_adc_one_shot_mode            1'h0
   2       R/W    adc_data_latch_edge             1'h0
   7:3     R      adc_fifo_wpt                    5'h0
   12:8    R      adc_fifo_rpt                    5'h0
   13      R/W    adc_fifo_overwrite              1'h0
   19:14   R/W    adc_burst_size                  6'h0
   25:20   R/W    adc_fifo_thd                    6'h0
   26      W1C    adc_fifo_clr                    1'h0
   27      R/W    adc_one_shot_fifo               1'h0
   29:28   R      reserved04_29_28                2'h0
   31:30   R/W    adc_dbg_sel                     2'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t en_adc_continous_mode: 1;
        uint32_t en_adc_one_shot_mode: 1;
        uint32_t adc_data_latch_edge: 1;
        const uint32_t adc_fifo_wpt: 5;
        const uint32_t adc_fifo_rpt: 5;
        uint32_t adc_fifo_overwrite: 1;
        uint32_t adc_burst_size: 6;
        uint32_t adc_fifo_thd: 6;
        uint32_t adc_fifo_clr: 1;
        uint32_t adc_one_shot_fifo: 1;
        const uint32_t reserved_0: 2;
        uint32_t adc_dbg_sel: 2;
    } b;
} ADC_DIG_CTRL_TypeDef;


/* 0x08
   15:0    R/W    adc_schedule_idx_sel            16'h0
   21:16   R      adc_fifo_data_level             6'h0
   31:22   R      reserved08_31_22                10'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t adc_schedule_idx_sel: 16;
        const uint32_t adc_fifo_data_level: 6;
        const uint32_t reserved_0: 10;
    } b;
} ADC_SCHED_CTRL_TypeDef;


/* 0x0C
   0       R/W    en_adc_fifio_rd_req_interrupt   1'h0
   1       R/W    en_adc_fifo_rd_error_interrupt  1'h0
   2       R/W    en_adc_fifo_thd_interrupt       1'h0
   3       R/W    en_adc_fifo_full_interrupt      1'h0
   4       R/W    en_adc_one_shot_done_interrupt  1'h0
   7:5     R      reserved0c_7_5                  3'h0
   8       W1C    clr_adc_fifio_rd_req_interrupt  1'h0
   9       W1C    clr_adc_fifo_rd_error_interrupt 1'h0
   10      W1C    clr_adc_fifo_thd_interrupt      1'h0
   11      W1C    clr_adc_fifo_full_interrupt     1'h0
   12      W1C    clr_adc_one_shot_done_interrupt 1'h0
   15:13   R      reserved0c_15_13                3'h0
   16      R      st_adc_fifio_rd_req_interrupt   1'h0
   17      R      st_adc_fifo_rd_error_interrupt  1'h0
   18      R      st_adc_fifo_thd_interrupt       1'h0
   19      R      st_adc_fifo_full_interrupt      1'h0
   20      R      st_adc_one_shot_done_interrupt  1'h0
   31:21   R      reserved0c_31_21                11'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t en_adc_fifio_rd_req_interrupt: 1;
        uint32_t en_adc_fifo_rd_error_interrupt: 1;
        uint32_t en_adc_fifo_thd_interrupt: 1;
        uint32_t en_adc_fifo_full_interrupt: 1;
        uint32_t en_adc_one_shot_done_interrupt: 1;
        const uint32_t reserved_2: 3;
        uint32_t clr_adc_fifio_rd_req_interrupt: 1;
        uint32_t clr_adc_fifo_rd_error_interrupt: 1;
        uint32_t clr_adc_fifo_thd_interrupt: 1;
        uint32_t clr_adc_fifo_full_interrupt: 1;
        uint32_t clr_adc_one_shot_done_interrupt: 1;
        const uint32_t reserved_1: 3;
        const uint32_t st_adc_fifio_rd_req_interrupt: 1;
        const uint32_t st_adc_fifo_rd_error_interrupt: 1;
        const uint32_t st_adc_fifo_thd_interrupt: 1;
        const uint32_t st_adc_fifo_full_interrupt: 1;
        const uint32_t st_adc_one_shot_done_interrupt: 1;
        const uint32_t reserved_0: 11;
    } b;
} ADC_CTRL_INT_TypeDef;


/* 0x10
   2:0     R/W    ch_num_idx0                     3'h0
   4:3     R/W    adc_mode_idx0                   2'h0
   15:5    R      reserved10_15_5                 11'h0
   18:16   R/W    ch_num_idx1                     3'h0
   20:19   R/W    adc_mode_idx1                   2'h0
   31:21   R      reserved10_31_21                11'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t ch_num_idx0: 3;
        uint32_t adc_mode_idx0: 2;
        const uint32_t reserved_1: 11;
        uint32_t ch_num_idx1: 3;
        uint32_t adc_mode_idx1: 2;
        const uint32_t reserved_0: 11;
    } b;
} ADC_SCHED_TAB_IDX0_IDX1_TypeDef;


/* 0x14
   2:0     R/W    ch_num_idx2                     3'h0
   4:3     R/W    adc_mode_idx2                   2'h0
   15:5    R      reserved14_15_5                 11'h0
   18:16   R/W    ch_num_idx3                     3'h0
   20:19   R/W    adc_mode_idx3                   2'h0
   31:21   R      reserved14_31_21                11'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t ch_num_idx2: 3;
        uint32_t adc_mode_idx2: 2;
        const uint32_t reserved_1: 11;
        uint32_t ch_num_idx3: 3;
        uint32_t adc_mode_idx3: 2;
        const uint32_t reserved_0: 11;
    } b;
} ADC_SCHED_TAB_IDX2_TypeDef;


/* 0x18
   2:0     R/W    ch_num_idx4                     3'h0
   4:3     R/W    adc_mode_idx4                   2'h0
   15:5    R      reserved18_15_5                 11'h0
   18:16   R/W    ch_num_idx5                     3'h0
   20:19   R/W    adc_mode_idx5                   2'h0
   31:21   R      reserved18_31_21                11'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t ch_num_idx4: 3;
        uint32_t adc_mode_idx4: 2;
        const uint32_t reserved_1: 11;
        uint32_t ch_num_idx5: 3;
        uint32_t adc_mode_idx5: 2;
        const uint32_t reserved_0: 11;
    } b;
} ADC_SCHED_TAB_IDX4_TypeDef;


/* 0x1C
   2:0     R/W    ch_num_idx6                     3'h0
   4:3     R/W    adc_mode_idx6                   2'h0
   15:5    R      reserved1c_15_5                 11'h0
   18:16   R/W    ch_num_idx7                     3'h0
   20:19   R/W    adc_mode_idx7                   2'h0
   31:21   R      reserved1c_31_21                11'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t ch_num_idx6: 3;
        uint32_t adc_mode_idx6: 2;
        const uint32_t reserved_1: 11;
        uint32_t ch_num_idx7: 3;
        uint32_t adc_mode_idx7: 2;
        const uint32_t reserved_0: 11;
    } b;
} ADC_SCHED_TAB_IDX6_TypeDef;


/* 0x20
   2:0     R/W    ch_num_idx8                     3'h0
   4:3     R/W    adc_mode_idx8                   2'h0
   15:5    R      reserved20_15_5                 11'h0
   18:16   R/W    ch_num_idx9                     3'h0
   20:19   R/W    adc_mode_idx9                   2'h0
   31:21   R      reserved20_31_21                11'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t ch_num_idx8: 3;
        uint32_t adc_mode_idx8: 2;
        const uint32_t reserved_1: 11;
        uint32_t ch_num_idx9: 3;
        uint32_t adc_mode_idx9: 2;
        const uint32_t reserved_0: 11;
    } b;
} ADC_SCHED_TAB_IDX8_TypeDef;


/* 0x24
   2:0     R/W    ch_num_idx10                    3'h0
   4:3     R/W    adc_mode_idx10                  2'h0
   15:5    R      reserved24_15_5                 11'h0
   18:16   R/W    ch_num_idx11                    3'h0
   20:19   R/W    adc_mode_idx11                  2'h0
   31:21   R      reserved24_31_21                11'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t ch_num_idx10: 3;
        uint32_t adc_mode_idx10: 2;
        const uint32_t reserved_1: 11;
        uint32_t ch_num_idx11: 3;
        uint32_t adc_mode_idx11: 2;
        const uint32_t reserved_0: 11;
    } b;
} ADC_SCHED_TAB_IDX10_TypeDef;


/* 0x28
   2:0     R/W    ch_num_idx[12]                  3'h0
   4:3     R/W    adc_mode_idx[12]                2'h0
   15:5    R      reserved28_15_5                 11'h0
   18:16   R/W    ch_num_idx13                    3'h0
   20:19   R/W    adc_mode_idx13                  2'h0
   31:21   R      reserved28_31_21                11'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t ch_num_idx_12: 3;
        uint32_t adc_mode_idx_12: 2;
        const uint32_t reserved_1: 11;
        uint32_t ch_num_idx13: 3;
        uint32_t adc_mode_idx13: 2;
        const uint32_t reserved_0: 11;
    } b;
} ADC_SCHED_TAB_IDX12_TypeDef;


/* 0x2C
   2:0     R/W    ch_num_idx14                    3'h0
   4:3     R/W    adc_mode_idx14                  2'h0
   15:5    R      reserved2c_15_5                 11'h0
   18:16   R/W    ch_num_idx15                    3'h0
   20:19   R/W    adc_mode_idx15                  2'h0
   31:21   R      reserved2c_31_21                11'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t ch_num_idx14: 3;
        uint32_t adc_mode_idx14: 2;
        const uint32_t reserved_1: 11;
        uint32_t ch_num_idx15: 3;
        uint32_t adc_mode_idx15: 2;
        const uint32_t reserved_0: 11;
    } b;
} ADC_SCHED_TAB_IDX14_TypeDef;


/* 0x30
   15:0    R      adc_idx0_output_data            16'h0
   31:16   R      adc_idx1_output_data            16'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t adc_idx0_output_data: 16;
        const uint32_t adc_idx1_output_data: 16;
    } b;
} ADC_IDX0_OUTPUT_DATA_TypeDef;


/* 0x34
   15:0    R      adc_idx2_output_data            16'h0
   31:16   R      adc_idx3_output_data            16'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t adc_idx2_output_data: 16;
        const uint32_t adc_idx3_output_data: 16;
    } b;
} ADC_IDX2_OUTPUT_DATA_TypeDef;


/* 0x38
   15:0    R      adc_idx4_output_data            16'h0
   31:16   R      adc_idx5_output_data            16'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t adc_idx4_output_data: 16;
        const uint32_t adc_idx5_output_data: 16;
    } b;
} ADC_IDX4_OUTPUT_DATA_TypeDef;


/* 0x3C
   15:0    R      adc_idx6_output_data            16'h0
   31:16   R      adc_idx7_output_data            16'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t adc_idx6_output_data: 16;
        const uint32_t adc_idx7_output_data: 16;
    } b;
} ADC_IDX6_OUTPUT_DATA_TypeDef;


/* 0x40
   15:0    R      adc_idx8_output_data            16'h0
   31:16   R      adc_idx9_output_data            16'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t adc_idx8_output_data: 16;
        const uint32_t adc_idx9_output_data: 16;
    } b;
} ADC_IDX8_OUTPUT_DATA_TypeDef;


/* 0x44
   15:0    R      adc_idx10_output_data           16'h0
   31:16   R      adc_idx11_output_data           16'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t adc_idx10_output_data: 16;
        const uint32_t adc_idx11_output_data: 16;
    } b;
} ADC_IDX10_OUTPUT_DATA_TypeDef;


/* 0x48
   15:0    R      adc_idx12_output_data           16'h0
   31:16   R      adc_idx13_output_data           16'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t adc_idx12_output_data: 16;
        const uint32_t adc_idx13_output_data: 16;
    } b;
} ADC_IDX12_OUTPUT_DATA_TypeDef;


/* 0x4C
   15:0    R      adc_idx14_output_data           16'h0
   31:16   R      adc_idx15_output_data           16'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t adc_idx14_output_data: 16;
        const uint32_t adc_idx15_output_data: 16;
    } b;
} ADC_IDX14_OUTPUT_DATA_TypeDef;


/* 0x50
   1:0     R/W    adc_rg0x_auxadc_0_delay_sel     2'h0
   3:2     R/W    adc_rg0x_auxadc_1_delay_sel     2'h0
   5:4     R/W    adc_rg2x_auxadc_0_delay_sel     2'h0
   8:6     R/W    adc_data_delay                  3'h0
   9       R      reserved50_9                    1'h0
   14:10   R/W    adc_poweron_select              5'h0
   15      R/W    adc_poweron_only_en             1'h0
   18:16   R      reserved50_18_16                3'h0
   19      R/W    adc_manual_poweron              1'h0
   20      R/W    dummy50_20                      1'h0
   21      R/W    adc_fifo_stop_wr                1'h0
   23:22   R      reserved50_23_22                2'h0
   24      R/W    adc_data_avg_en                 1'h0
   27:25   R/W    adc_data_avg_sel                3'h0
   30:28   R      reserved50_30_28                3'h0
   31      R/W    adc_ctc_dis                     1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t adc_rg0x_auxadc_0_delay_sel: 2;
        uint32_t adc_rg0x_auxadc_1_delay_sel: 2;
        uint32_t adc_rg2x_auxadc_0_delay_sel: 2;
        uint32_t adc_data_delay: 3;
        const uint32_t reserved_3: 1;
        uint32_t adc_poweron_select: 5;
        uint32_t adc_poweron_only_en: 1;
        const uint32_t reserved_2: 3;
        uint32_t adc_manual_poweron: 1;
        uint32_t dummy50_20: 1;
        uint32_t adc_fifo_stop_wr: 1;
        const uint32_t reserved_1: 2;
        uint32_t adc_data_avg_en: 1;
        uint32_t adc_data_avg_sel: 3;
        const uint32_t reserved_0: 3;
        uint32_t adc_ctc_dis: 1;
    } b;
} ADC_POW_DATA_DLY_CTRL_TypeDef;


/* 0x54
   7:0     R      reserved54_7_0                  8'h0
   8       R/W    adc_keep_ck_ad_high             1'h0
   10:9    R      reserved54_10_9                 2'h0
   12:11   R/W    adc_delay_tune_sel              2'h0
   15:13   R      reserved54_15_13                3'h0
   27:16   R/W    adc_data_offset                 12'h0
   28      R      reserved54_28                   1'h0
   29      R/W    adc_timer_trigger_en            1'h0
   30      R/W    adc_data_align_msb              1'h0
   31      R/W    adc_data_offset_en              1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_3: 8;
        uint32_t adc_keep_ck_ad_high: 1;
        const uint32_t reserved_2: 2;
        uint32_t adc_delay_tune_sel: 2;
        const uint32_t reserved_1: 3;
        uint32_t adc_data_offset: 12;
        const uint32_t reserved_0: 1;
        uint32_t adc_timer_trigger_en: 1;
        uint32_t adc_data_align_msb: 1;
        uint32_t adc_data_offset_en: 1;
    } b;
} ADC_DATA_CLK_CTRL_TypeDef;


/* 0x58
   0       R/W    pow                             1'h0
   1       R/W    pow_ref                         1'h0
   2       R/W    reg_ck                          1'h0
   3       R/W    reg_delay                       1'h0
   5:4     R/W    reg_vref_sel[1:0]               2'h2
   6       R/W    reg_revck                       1'h0
   8:7     R/W    reg_cmpdec[1:0]                 2'h0
   9       R/W    reg_meta                        1'h0
   10      R/W    sel_ln                          1'h0
   11      R/W    sel_lna                         1'h0
   13:12   R/W    reg_vcm_sel                     2'h0
   14      R/W    RG0X_AUXADC_DUMMY               1'h0
   15      R/W    RG0X_AUXADC_CTRL_SEL            1'h0
   31:16   R      reserved58_31_16                16'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t pow: 1;
        uint32_t pow_ref: 1;
        uint32_t reg_ck: 1;
        uint32_t reg_delay: 1;
        uint32_t reg_vref_sel_1_0: 2;
        uint32_t reg_revck: 1;
        uint32_t reg_cmpdec_1_0: 2;
        uint32_t reg_meta: 1;
        uint32_t sel_ln: 1;
        uint32_t sel_lna: 1;
        uint32_t reg_vcm_sel: 2;
        uint32_t RG0X_AUXADC_DUMMY: 1;
        uint32_t RG0X_AUXADC_CTRL_SEL: 1;
        const uint32_t reserved_0: 16;
    } b;
} ADC_ANALOG_CTRL_TypeDef;


/* 0x5C
   13:0    R/W    adc_sample_time_period          14'h0
   29:14   R      reserved5c_29_14                16'h0
   31:30   R/W    adc_convert_time_period_sel     2'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t adc_sample_time_period: 14;
        const uint32_t reserved_0: 16;
        uint32_t adc_convert_time_period_sel: 2;
    } b;
} ADC_TIME_PERIOD_TypeDef;


/* 0x60
   31:0    R      RTLversion                      32'h20240530
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t RTLversion: 32;
    } b;
} ADC_RTL_VERSION_TypeDef;


/*============================================================================*
 *                         Constants
 *============================================================================*/
/**
 * \defgroup    ADC_Interrupts_Definition ADC Interrupts Definition
 * \{
 * \ingroup     ADC_Exported_Constants
 */
#define ADC_INT_FIFO_RD_REQ           ((uint32_t)(1 << 0))
#define ADC_INT_FIFO_RD_ERR           ((uint32_t)(1 << 1))
#define ADC_INT_FIFO_THD              ((uint32_t)(1 << 2))
#define ADC_INT_FIFO_FULL             ((uint32_t)(1 << 3))
#define ADC_INT_ONE_SHOT_DONE         ((uint32_t)(1 << 4))

#define IS_ADC_INT(INT) (((INT) == ADC_INT_FIFO_RD_REQ) || \
                         ((INT) == ADC_INT_FIFO_RD_ERR) || \
                         ((INT) == ADC_INT_FIFO_THD) || \
                         ((INT) == ADC_INT_ONE_SHOT_DONE) || \
                         ((INT) == ADC_INT_FIFO_FULL))

/** End of ADC_Interrupts_Definition
  * \}
  */

/*============================================================================*
 *                         ADC WRAPPER
 *============================================================================*/
#define adcClock                              ADC_SampleTime
#define adcConvertTimePeriod                  ADC_ConvertTime
#define dataWriteToFifo                       ADC_DataWriteToFifo
#define adcFifoThd                            ADC_FifoThdLevel
#define adcBurstSize                          ADC_WaterLevel
#define adcFifoOverWritEn                     ADC_FifoOverWriteEn
#define dataLatchEdge                         ADC_DataLatchEdge
#define schIndex                              ADC_SchIndex
#define bitmap                                ADC_Bitmap
#define timerTriggerEn                        ADC_TimerTriggerEn
#define dataAligned                           ADC_DataAlign
#define dataMinusEn                           ADC_DataMinusEn
#define dataMinusOffset                       ADC_DataMinusOffset
#define pwrmode                               ADC_PowerOnMode
#define datalatchDly                          ADC_DataLatchDly
#define adcRG2X0Dly                           ADC_RG2X0Dly
#define adcRG0X1Dly                           ADC_RG0X1Dly
#define adcRG0X0Dly                           ADC_RG0X0Dly
#define adc_fifo_stop_write                   ADC_FifoStopWriteEn
#define adcPowerAlwaysOnCmd                   ADC_PowerAlwaysOnEn

/**
 * \brief    ADC Macro Wrapper
 *
 * \ingroup  ADC_Exported_Constants
 */
#define ADC_Latch_Data_Positive               ADC_LATCH_DATA_Positive
#define ADC_Latch_Data_Negative               ADC_LATCH_DATA_Negative
#define ADC_DATA_LSB                          ADC_DATA_ALIGN_LSB
#define ADC_DATA_MSB                          ADC_DATA_ALIGN_MSB
#define T_ADC_DTAT_AVG_SEL                    ADC_DATA_AVG_SEL_TypeDef
#define ADC_DTAT_AVG_SEL_BY2                  ADC_DATA_AVERAGE_OF_2
#define ADC_DTAT_AVG_SEL_BY4                  ADC_DATA_AVERAGE_OF_4
#define ADC_DTAT_AVG_SEL_BY8                  ADC_DATA_AVERAGE_OF_8
#define ADC_DTAT_AVG_SEL_BY16                 ADC_DATA_AVERAGE_OF_16
#define ADC_DTAT_AVG_SEL_BY32                 ADC_DATA_AVERAGE_OF_32
#define ADC_DTAT_AVG_SEL_BY64                 ADC_DATA_AVERAGE_OF_64
#define ADC_DTAT_AVG_SEL_BY128                ADC_DATA_AVERAGE_OF_128
#define ADC_DTAT_AVG_SEL_BY256                ADC_DATA_AVERAGE_OF_256
#define ADC_DTAT_AVG_SEL_MAX                  ADC_DATA_AVERAGE_MAX
#define ADC_POWER_AUTO                        ADC_POWER_ON_AUTO
#define ADC_POWER_MANNUAL                     ADC_POWER_ON_MANUAL
#define ADC_Continuous_Mode                   ADC_CONTINUOUS_MODE
#define ADC_One_Shot_Mode                     ADC_ONE_SHOT_MODE
#define ADC_INT_FIFO_TH                       ADC_INT_FIFO_THD

#define ADC_DATA_MINUS_EN                     ENABLE
#define ADC_DATA_MINUS_DIS                    DISABLE
#define ADC_ONE_SHOT_FIFO_ENABLE              ENABLE
#define ADC_ONE_SHOT_FIFO_DISABLE             DISABLE
#define ADC_FIFO_OVER_WRITE_ENABLE            ENABLE
#define ADC_FIFO_OVER_WRITE_DISABLE           DISABLE
#define ADC_POWER_ALWAYS_ON_ENABLE            ENABLE
#define ADC_POWER_ALWAYS_ON_DISABLE           DISABLE
#define IS_ADC_GET_IT(IT) IS_ADC_IT(IT)

/**
 * \brief    ADC API Wrapper
 *
 * \ingroup  ADC_Exported_Constants
 */
#define ADC_Read                          ADC_ReadRawData
#define ADC_HwEvgRead                     ADC_ReadAvgRawData
#define ADC_ReadFifoData                  ADC_ReadFIFO
#define ADC_GetFifoData                   ADC_ReadFIFOData
#define ADC_GetFifoLen                    ADC_GetFIFODataLen
#define ADC_OneshotFifoSet                ADC_WriteFIFOCmd
#define ADC_HighBypassCmd                 ADC_BypassCmd
#define ADC_ClearFifo                     ADC_ClearFIFO
#define ADC_SchTableSet                   ADC_BitMapConfig
#define ADC_GetIntStatus                  ADC_GetAllFlagStatus
#define ADC_GetIntFlagStatus              ADC_GetINTStatus

/**
 * \defgroup ADC_Constant_Wrapper ADC Constant Wrapper
 * \{
 * \ingroup  ADC_Exported_Constants
 */
#define ADC_MODE_SINGLE_ENDED_VALUE     0x1
#define ADC_MODE_DIFFERENTIAL_VALUE     0x2
#define ADC_MODE_INTERNAL_VALUE         0x3
#define ADC_MODE_RESERVED_VALUE         0x0


#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* RTL876X_ADC_DEF_H */
