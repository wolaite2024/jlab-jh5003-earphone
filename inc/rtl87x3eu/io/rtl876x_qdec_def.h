/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl876x_qdec_reg.h
* \brief    QDEC Registers Structures Section
* \details
* \author   colin
* \date     2024-06-19
* \version  v1.0
* *********************************************************************************************************
*/
#ifndef RTL_QDEC_REG_H
#define RTL_QDEC_REG_H

#include <stdint.h>
#include <stdbool.h>

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/*============================================================================*
 *                         QDEC Features
 *============================================================================*/

#define CORE_QDEC_SUPPORT                              (1)
#define QDEC_CLOCK_INIT_USE_KHZ                        (1)

/*============================================================================*
 *                         QDEC Registers Memory Map
 *============================================================================*/
typedef struct
{
    __IO uint32_t QDEC_DIV;              /*!< 0x00 */
    __IO uint32_t QDEC_CR_X;             /*!< 0x04 */
    __IO uint32_t QDEC_SR_X;             /*!< 0x08 */
    __IO uint32_t QDEC_CR_Y;             /*!< 0x0C */
    __IO uint32_t QDEC_SR_Y;             /*!< 0x10 */
    __IO uint32_t QDEC_CR_Z;             /*!< 0x14 */
    __IO uint32_t QDEC_SR_Z;             /*!< 0x18 */
    __IO uint32_t QDEC_INT_MASK;         /*!< 0x1C */
    __IO uint32_t QDEC_INT_SR;           /*!< 0x20 */
    __IO uint32_t QDEC_INT_CLR;          /*!< 0x24 */
    __IO uint32_t QDEC_DBG;              /*!< 0x28 */
    __I  uint32_t QDEC_VERSION;          /*!< 0x2C */
} QDEC_TypeDef;

/*============================================================================*
 *                         QDEC Declaration
 *============================================================================*/
#define QDEC                    ((QDEC_TypeDef *) QDEC_BASE)

#define IS_QDEC_PERIPH(PERIPH) ((PERIPH) == QDEC)
#define QDEC_SOURCE_CLOCK       20000   //KHz

/*============================================================================*
 *                         QDEC Registers and Field Descriptions
 *============================================================================*/
/* 0x00
   3:0     R/W    deb_div                     4'h1
   15:4    R/W    scan_div                    12'h27
   31:16   R/W    reserved31_16               16'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t deb_div: 4;
        uint32_t scan_div: 12;
        uint32_t reserved_0: 16;
    } b;
} QDEC_DIV_TypeDef;


/* 0x04
   1:0     R/W    x_initial_phase         2'h0
   2       R/W    manual_set_initial_phase1'h0
   3       R/W    x_cnt_pause             1'h0
   11:4    R/W    x_debounce_cnt          8'h0
   12      R/W    x_debounce_en           1'h0
   13      R/W    x_cnt_scale             1'h0
   14      R/W    x_illegal_int_en        1'h0
   15      R/W    x_cnt_int_en            1'h0
   28:16   R      reserved28_16           13'h0
   29      R/W    reserved29              1'h0
   30      R/W1C  x_fsm_en                1'h0
   31      R/W    x_axis_en               1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t x_initial_phase: 2;
        uint32_t manual_set_initial_phase: 1;
        uint32_t x_cnt_pause: 1;
        uint32_t x_debounce_cnt: 8;
        uint32_t x_debounce_en: 1;
        uint32_t x_cnt_scale: 1;
        uint32_t x_illegal_int_en: 1;
        uint32_t x_cnt_int_en: 1;
        const uint32_t reserved_1: 13;
        uint32_t reserved_0: 1;
        uint32_t x_fsm_en: 1;
        uint32_t x_axis_en: 1;
    } b;
} QDEC_CR_X_TypeDef;


/* 0x08
   15:0    R      x_acc_cnt               16'h0
   16      R      x_cnt_dir               1'h0
   17      R      reserved17              1'h0
   18      R      x_cnt_of_flg            1'h0
   19      R      x_cnt_uf_flg            1'h0
   31:20   R      reserved31_20           12'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t x_acc_cnt: 16;
        const uint32_t x_cnt_dir: 1;
        const uint32_t reserved_1: 1;
        const uint32_t x_cnt_of_flg: 1;
        const uint32_t x_cnt_uf_flg: 1;
        const uint32_t reserved_0: 12;
    } b;
} QDEC_SR_X_TypeDef;


/* 0x0C
   1:0     R/W    y_initial_phase         2'h0
   2       R/W    manual_set_initial_phase1'h0
   3       R/W    y_cnt_pause             1'h0
   11:4    R/W    y_debounce_cnt          8'h0
   12      R/W    y_debounce_en           1'h0
   13      R/W    y_cnt_scale             1'h0
   14      R/W    y_illegal_int_en        1'h0
   15      R/W    y_cnt_int_en            1'h0
   29:16   R/W    reserved29_16           14'h0
   30      R/W1C  y_fsm_en                1'h0
   31      R/W    y_axis_en               1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t y_initial_phase: 2;
        uint32_t manual_set_initial_phase: 1;
        uint32_t y_cnt_pause: 1;
        uint32_t y_debounce_cnt: 8;
        uint32_t y_debounce_en: 1;
        uint32_t y_cnt_scale: 1;
        uint32_t y_illegal_int_en: 1;
        uint32_t y_cnt_int_en: 1;
        const uint32_t reserved_0: 14;
        uint32_t y_fsm_en: 1;
        uint32_t y_axis_en: 1;
    } b;
} QDEC_CR_Y_TypeDef;


/* 0x10
   15:0    R      y_acc_cnt               16'h0
   16      R      y_cnt_dir               1'h0
   17      R      reserved17              1'h0
   18      R      y_cnt_of_flg            1'h0
   19      R      y_cnt_uf_flg            1'h0
   31:20   R      reserved31_20           12'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t y_acc_cnt: 16;
        const uint32_t y_cnt_dir: 1;
        const uint32_t reserved_1: 1;
        const uint32_t y_cnt_of_flg: 1;
        const uint32_t y_cnt_uf_flg: 1;
        const uint32_t reserved_0: 12;
    } b;
} QDEC_SR_Y_TypeDef;


/* 0x14
   1:0     R/W    z_initial_phase         2'h0
   2       R/W    manual_set_initial_phase1'h0
   3       R/W    z_cnt_pause             1'h0
   11:4    R/W    z_debounce_cnt          8'h0
   12      R/W    z_debounce_en           1'h0
   13      R/W    z_cnt_scale             1'h0
   14      R/W    z_illegal_int_en        1'h0
   15      R/W    z_cnt_int_en            1'h0
   29:16   R/W    reserved29_16           14'h0
   30      R/W    z_fsm_en                1'h0
   31      R/W    z_axis_en               1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t z_initial_phase: 2;
        uint32_t manual_set_initial_phase: 1;
        uint32_t z_cnt_pause: 1;
        uint32_t z_debounce_cnt: 8;
        uint32_t z_debounce_en: 1;
        uint32_t z_cnt_scale: 1;
        uint32_t z_illegal_int_en: 1;
        uint32_t z_cnt_int_en: 1;
        const uint32_t reserved_0: 14;
        uint32_t z_fsm_en: 1;
        uint32_t z_axis_en: 1;
    } b;
} QDEC_CR_Z_TypeDef;


/* 0x18
   15:0    R      z_acc_cnt               16'h0
   16      R      z_cnt_dir               1'h0
   17      R      reserved17              1'h0
   18      R      z_cnt_of_flg            1'h0
   19      R      z_cnt_uf_flg            1'h0
   31:20   R      reserved31_20           12'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t z_acc_cnt: 16;
        const uint32_t z_cnt_dir: 1;
        const uint32_t reserved_1: 1;
        const uint32_t z_cnt_of_flg: 1;
        const uint32_t z_cnt_uf_flg: 1;
        const uint32_t reserved_0: 12;
    } b;
} QDEC_SR_Z_TypeDef;


/* 0x1C
   2:0     R/W    cnt_int_mask            3'h0
   3       R      reserved3               1'h0
   6:4     R/W    illegal_int_mask        3'h0
   31:7    R      reserved31_7            25'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t cnt_int_mask: 3;
        const uint32_t reserved_1: 1;
        uint32_t illegal_int_mask: 3;
        const uint32_t reserved_0: 25;
    } b;
} QDEC_INT_MASK_TypeDef;


/* 0x20
   2:0     R      cnt_int_sta             3'h0
   11:3    R      reserved11_3            9'h0
   14:12   R      illegal_int_sta         3'h0
   31:15   R      reserved31_15           17'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t cnt_int_sta: 3;
        const uint32_t reserved_1: 9;
        const uint32_t illegal_int_sta: 3;
        const uint32_t reserved_0: 17;
    } b;
} QDEC_INT_SR_TypeDef;


/* 0x24
   2:0     W1C    cnt_int_clr             3'h0
   3       R      reserved3               1'h0
   6:4     W1C    of_flg_clr              3'h0
   7       R      reserved7               1'h0
   10:8    W1C    uf_flg_clr              3'h0
   11      R      reserved11              1'h0
   14:12   W1C    illegal_int_clr         3'h0
   15      R      reserved15              1'h0
   18:16   W1C    acc_cnt_clr             3'h0
   19      R      reserved19              1'h0
   22:20   W1C    illegal_cnt_clr         3'h0
   31:23   R      reserved31_23           9'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t cnt_int_clr: 3;
        const uint32_t reserved_5: 1;
        uint32_t of_flg_clr: 3;
        const uint32_t reserved_4: 1;
        uint32_t uf_flg_clr: 3;
        const uint32_t reserved_3: 1;
        uint32_t illegal_int_clr: 3;
        const uint32_t reserved_2: 1;
        uint32_t acc_cnt_clr: 3;
        const uint32_t reserved_1: 1;
        uint32_t illegal_cnt_clr: 3;
        const uint32_t reserved_0: 9;
    } b;
} QDEC_INT_CLR_TypeDef;


/* 0x28
   1:0     R/W    debugsignalselect       2'h0
   3:2     R      debug_state_x           2'h0
   5:4     R      debug_state_y           2'h0
   7:6     R      debug_state_z           2'h0
   15:8    R      debug_illegal_counter_x 8'h0
   23:16   R      debug_illegal_counter_y 8'h0
   31:24   R      debug_illegal_counter_z 8'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t debugsignalselect: 2;
        const uint32_t debug_state_x: 2;
        const uint32_t debug_state_y: 2;
        const uint32_t debug_state_z: 2;
        const uint32_t debug_illegal_counter_x: 8;
        const uint32_t debug_illegal_counter_y: 8;
        const uint32_t debug_illegal_counter_z: 8;
    } b;
} QDEC_DBG_TypeDef;


/* 0x2C
   31:0    R      rtlversion              32'h2606140a
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t rtlversion: 32;
    } b;
} QDEC_VERSION_TypeDef;

/*============================================================================*
 *                         RTC Wrappers
 *============================================================================*/

/**
 * \brief    QDEC InitTypeDef Wrapper
 *
 * \ingroup  QDEC_Exported_Constants
 */
#define autoLoadInitPhase       manualLoadInitPhase

/**
 * \brief    QDEC Macro Wrapper
 *
 * \ingroup  QDEC_Exported_Constants
 */
#define counterScaleDisable     CounterScale_1_Phase
#define counterScaleEnable      CounterScale_2_Phase
#define IS_QDEC_STATUS_FLAG     IS_QDEC_INT_STATUS
#define Debounce_Enable         ENABLE
#define Debounce_Disable        DISABLE
#define manualPhaseEnable       ENABLE
#define manualPhaseDisable      DISABLE

/**
 * \brief    QDEC API Wrapper
 *
 * \ingroup  QDEC_Exported_Constants
 */
#define QDEC_ClearFlags         QDEC_ClearINTPendingBit


#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* RTL_QDEC_REG_H */
