/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl876x_lppwm_def.h
* \brief    LPPWM related definitions
* \details
* \author
* \date
* \version
* *********************************************************************************************************
*/

#ifndef RTL876X_LPPWM_DEF_H
#define RTL876X_LPPWM_DEF_H

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "rtl876x.h"

/*============================================================================*
 *                          LPPWM Defines
 *============================================================================*/

/*============================================================================*
 *                          LPPWM Registers Memory Map
 *============================================================================*/
typedef struct
{
    __IO uint16_t LPPWM_CH0_CTL;            /*!< 0x00 */
    __I  uint16_t LPPWM_CH0_RSVD0;          /*!< 0x02 */
    __IO uint16_t LPPWM_CH0_P0_H;           /*!< 0x04 */
    __I  uint16_t LPPWM_CH0_RSVD1;          /*!< 0x06 */
    __IO uint16_t LPPWM_CH0_P0_L;           /*!< 0x08 */
    __I  uint16_t LPPWM_CH0_RSVD2;          /*!< 0x0A */
    __I  uint16_t LPPWM_CH0_CURRENT;        /*!< 0x0C */
    __I  uint16_t LPPWM_CH0_RSVD3[39];      /*!< 0x0E - 0x5A */
    __I  uint16_t LPPWM_VERSION_LO;         /*!< 0x5C */
    __I  uint16_t LPPWM_VERSION_HI;         /*!< 0x5E */
} LPPWM_TypeDef;

/*============================================================================*
 *                          LPPWM Declaration
 *============================================================================*/
#define LPPWM_REG_BASE           (0)
#define LPPWM                    ((LPPWM_TypeDef *) LPPWM_REG_BASE)

#define IS_LPPWM_PERIPH(PERIPH)  ((PERIPH) == LPPWM)

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* RTL_LPPWM_DEF_H */
