/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl876x_lpc_def.h
* \brief    LPC Registers Structures Section
* \details
* \author
* \date
* \version  v1.0
* *********************************************************************************************************
*/
#ifndef RTL876X_LPC_DEF_H
#define RTL876X_LPC_DEF_H

#include <stdint.h>
#include <stdbool.h>
#include "rtl876x.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/*============================================================================*
 *                          LPC Registers Memory Map
 *============================================================================*/
typedef struct
{
    __I  uint32_t LPC_VERSION;          /*!< 0x78 */
    __I  uint32_t RSVD0;                /*!< 0x7C */
    __IO uint32_t LPC_CR0;              /*!< 0x80 */
    __I  uint32_t LPC_SR;               /*!< 0x84 */
    __IO uint32_t LPC_CMP;              /*!< 0x88 */
    __I  uint32_t LPC_CNT;              /*!< 0x8C */
} LPC_TypeDef;

/*============================================================================*
 *                          LPC Declaration
 *============================================================================*/
#define LPC                             ((LPC_TypeDef              *) (SYSON_BASE + 0x178))

/*============================================================================*
 *                          LPC Private Types
 *============================================================================*/

/*============================================================================*
 *                          LPC Registers and Field Descriptions
 *============================================================================*/
/* 0x78
   31:0    R      lpc_version             32'h2406140a
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t lpc_version: 32;
    } b;
} LPC_VERSION_TypeDef;


/* 0x80
   0       R/W    lpcomp_counter_rst      1'h0
   1       R/W    lpcomp_counter_start    1'h0
   2       R/W    lpcomp_flag_clr         1'h0
   7:3     R      reserver080_7_3         5'h0
   8       R/W    lpcomp_ie               1'h0
   9       R/W    lpcomp_src_int_en       1'h0
   10      R/W    lpc_power               1'h0
   11      R/W    lpc_posedge             1'h0
   14:12   R/W    lpc_ch_num              3'h0
   18:15   R/W    lpc_threshold           4'h0
   19      R/W    lpcomp_src_aon_int_en   1'h0
   20      R/W    lpc_wk_en               1'h0
   31:20   R      reserver080_31_20       12'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t lpcomp_counter_rst: 1;
        uint32_t lpcomp_counter_start: 1;
        uint32_t lpcomp_flag_clr: 1;
        const uint32_t reserver080_7_3: 5;
        uint32_t lpcomp_ie: 1;
        uint32_t lpcomp_src_int_en: 1;
        uint32_t lpc_power: 1;
        uint32_t lpc_posedge: 1;
        uint32_t lpc_ch_num: 3;
        uint32_t lpc_threshold: 4;
        uint32_t lpcomp_src_aon_int_en: 1;
        uint32_t lpc_wk_en: 1;
        const uint32_t reserver080_31_21: 11;
    } b;
} LPC_CR0_TypeDef;


/* 0x84
   0       R      lpcomp_flag             1'h0
   1       R      lpcomp_out_aon          1'h0
   31:2    R      reserver084_31_22       30'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t lpcomp_flag: 1;
        const uint32_t lpcomp_out_aon: 1;
        const uint32_t reserver084_31_22: 30;
    } b;
} LPC_SR_TypeDef;


/* 0x88
   11:0    R/W    lpcomp_comp             0x0
   31:12   R      reserver088_31_12       20'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t lpcomp_comp: 12;
        const uint32_t reserver088_31_12: 20;
    } b;
} LPC_CMP_TypeDef;


/* 0x8C
   11:0    R      lpcomp_counter          12'h1
   31:12   R      reserver08c_31_12       20'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t lpcomp_counter: 12;
        const uint32_t reserver08c_31_12: 20;
    } b;
} LPC_CNT_TypeDef;



#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* RTL876X_LPC_DEF_H */
