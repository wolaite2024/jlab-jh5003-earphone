/**
*****************************************************************************************
*     Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * \file    rtl876x_iso7816_def.h
  * \brief   ISO7816 related definitions.
  * \author
  * \date    2024-06-18
  * \version v1.0
  * *************************************************************************************
  */

#ifndef RTL876X_ISO7816_DEF_H
#define RTL876X_ISO7816_DEF_H

#include "rtl876x.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/*============================================================================*
 *                          ISO7816 Defines
 *============================================================================*/

/*============================================================================*
 *                          ISO7816 Registers Memory Map
 *============================================================================*/
typedef struct                      /*!< ISO7816 Structure */
{
    __IO  uint32_t CR;              /*!<0x00 */
    __IO  uint32_t GCR;             /*!<0x04 */
    __IO  uint32_t TCR;             /*!<0x08 */
    __IO  uint32_t RCR;             /*!<0x0C */
    __IO  uint32_t THR;             /*!<0x10 */
    __I   uint32_t ISR;             /*!<0x14 */
    __IO  uint32_t IER;             /*!<0x18 */
    __IO  uint32_t TX_FIFO;         /*!<0x1C */
    __IO  uint32_t RX_FIFO;         /*!<0x20 */
    __I   uint32_t TSR;             /*!<0x24 */
    __I   uint32_t RSR;             /*!<0x28 */
    __IO  uint32_t ESR;             /*!<0x2C */
    __IO  uint32_t LP_CLKG;         /*!<0x30 */
} ISO7816_TypeDef;

/*============================================================================*
 *                         ISO7816 Declaration
 *============================================================================*/
#define ISO7816                         ((ISO7816_TypeDef*) ISO7816_BASE)

#define IS_ISO7816_PERIPH(PERIPH) ((PERIPH) == ISO7816)

/*============================================================================*
 *                         ISO7816 Registers and Field Descriptions
 *============================================================================*/

/* Peripheral: ISO7816 */
/* Description: ISO7816 register defines */

/* Register: CR -------------------------------------------------------*/
/* Description: Control register. Offset: 0x00. Address: 0x400xxxxx. */

/* CR[27]: ISO7816_RX_FIFO_CLEAR. 0x1: Clear. */
#define ISO7816_RX_FIFO_CLEAR_POS                (27)
#define ISO7816_RX_FIFO_CLEAR_MSK                ((uint32_t)0x1 << ISO7816_RX_FIFO_CLEAR_POS)
#define ISO7816_RX_FIFO_CLEAR_CLR                (~ISO7816_RX_FIFO_CLEAR_MSK)
/* CR[26]: ISO7816_TX_FIFO_CLEAR. 0x1: Clear.  */
#define ISO7816_TX_FIFO_CLEAR_POS                (26)
#define ISO7816_TX_FIFO_CLEAR_MSK                ((uint32_t)0x1 << ISO7816_TX_FIFO_CLEAR_POS)
#define ISO7816_TX_FIFO_CLEAR_CLR                (~ISO7816_TX_FIFO_CLEAR_MSK)
/* CR[25]: ISO7816_INIT_GUARD_TIME. */
#define ISO7816_RX_FIFO_OVERWRITE_POS          (25)
#define ISO7816_INIT_GUARD_TIME_MSK         (0xF << ISO7816_INIT_GUARD_TIME_POS)
#define ISO7816_INIT_GUARD_TIME_CLR         (~ISO7816_INIT_GUARD_TIME_MSK)
/* CR[24]: ISO7816_PROTOCOL_TYPE. 0x1: T = 1. 0x0: T = 0,mandatory error signal and character repetition. */
#define ISO7816_PROTOCOL_TYPE_POS                (24)
#define ISO7816_PROTOCOL_TYPE_MSK                (0x1 << ISO7816_PROTOCOL_TYPE_POS)
#define ISO7816_PROTOCOL_TYPE_CLR                (~ISO7816_PROTOCOL_TYPE_MSK)
/* CR[23]: ISO7816_CLOCK_ENABLE. */
#define ISO7816_CLOCK_ENABLE_POS                 (23)
#define ISO7816_CLOCK_ENABLE_MSK                 (0x1 << ISO7816_CLOCK_ENABLE_POS)
#define ISO7816_CLOCK_ENABLE_CLR                 (~ISO7816_CLOCK_ENABLE_MSK)
/* CR[22]: ISO7816_CLOCK_STOP_POLAR. 0x1: high. 0x0: low. */
#define ISO7816_CLOCK_STOP_POLAR_POS             (22)
#define ISO7816_CLOCK_STOP_POLAR_MSK             (0x1 << ISO7816_CLOCK_STOP_POLAR_POS)
#define ISO7816_CLOCK_STOP_POLAR_CLR             (~ISO7816_CLOCK_STOP_POLAR_MSK)
/* CR[21]: ISO7816_VCC_ENABLE. */
#define ISO7816_VCC_ENABLE_POS                   (21)
#define ISO7816_VCC_ENABLE_MSK                   (0x1 << ISO7816_VCC_ENABLE_POS)
#define ISO7816_VCC_ENABLE_CLR                   (~ISO7816_VCC_ENABLE_MSK)
/* CR[20]: ISO7816_RESET_ENABLE. 0x1: bypass. 0x0: not bypass. */
#define ISO7816_RESET_ENABLE_POS                 (20)
#define ISO7816_RESET_ENABLE_MSK                 (0x1 << ISO7816_RESET_ENABLE_POS)
#define ISO7816_RESET_ENABLE_CLR                 (~ISO7816_RESET_ENABLE_MSK)
/* CR[19]: ISO7816_GP_COUNTER_ENABLE. */
#define ISO7816_GP_COUNTER_ENABLE_POS            (19)
#define ISO7816_GP_COUNTER_ENABLE_MSK            (0x1 << ISO7816_GP_COUNTER_ENABLE_POS)
#define ISO7816_GP_COUNTER_ENABLE_CLR            (~ISO7816_GP_COUNTER_ENABLE_MSK)
/* CR[18]: ISO7816_CONTROLLER_ENBALE. */
#define ISO7816_CONTROLLER_ENBALE_POS            (18)
#define ISO7816_CONTROLLER_ENBALE_MSK            (0x1 << ISO7816_CONTROLLER_ENBALE_POS)
#define ISO7816_CONTROLLER_ENBALE_CLR            (~ISO7816_CONTROLLER_ENBALE_MSK)
/* CR[17]: ISO7816_IO_MODE. */
#define ISO7816_IO_MODE_POS                      (17)
#define ISO7816_IO_MODE_MSK                      (0x1 << ISO7816_IO_MODE_POS)
#define ISO7816_IO_MODE_CLR                      (~ISO7816_IO_MODE_MSK)
/* CR[16]: ISO7816_CODING_CONVENTION. 0: direct convention 1: inverse convention. */
#define ISO7816_CODING_CONVENTION_POS            (16)
#define ISO7816_CODING_CONVENTION_MSK            (0x1 << ISO7816_CODING_CONVENTION_POS)
#define ISO7816_CODING_CONVENTION_CLR            (~ISO7816_CODING_CONVENTION_MSK)
/* CR[15:0]: ISO7816_GP_COUNTER. */
#define ISO7816_GP_COUNTER_POS                   (0)
#define ISO7816_GP_COUNTER_MSK                   (0xFFFF << ISO7816_GP_COUNTER_POS)
#define ISO7816_GP_COUNTER_CLR                   (~ISO7816_GP_COUNTER_MSK)

/* Register: GCR -------------------------------------------------------*/
/* Description: Device Global Configuration Register. Offset: 0x04. Address: 0x40024804. */

/* GCR[11:0]: ISO7816_CLOCK_DIV. */
#define ISO7816_CLOCK_DIV_POS                    (0)
#define ISO7816_CLOCK_DIV_MSK                    (0xFFF << ISO7816_CLOCK_DIV_POS)
#define ISO7816_CLOCK_DIV_CLR                    (~ISO7816_CLOCK_DIV_MSK)

/* Register: TCR ---------------------------------------------------------*/
/* Description: Transmitting Configuration Register. Offset: 0x08. Address: 0x40024808. */

/* TCR[7:0]: ISO7816_GUARD_TIME. */
#define ISO7816_GUARD_TIME_POS                   (0)
#define ISO7816_GUARD_TIME_MSK                   (0xFF << ISO7816_GUARD_TIME_POS)
#define ISO7816_GUARD_TIME_CLR                   (~ISO7816_GUARD_TIME_MSK)

/* Register: RCR ---------------------------------------------------------*/
/* Description: Receiving Configuration Register. Offset: 0x0C. Address: 0x4002480C. */

/* RCR[16]: ISO7816_IO_STATE_SAMPLE. 1: sample. 0: no sample. */
#define ISO7816_IO_STATE_SAMPLE_POS              (16)
#define ISO7816_IO_STATE_SAMPLE_MSK              (0x1 << ISO7816_IO_STATE_SAMPLE_POS)
#define ISO7816_IO_STATE_SAMPLE_CLR              (~ISO7816_IO_STATE_SAMPLE_MSK)
/* RCR[15:0]: ISO7816_WAITING_TIME. */
#define ISO7816_WAITING_TIME_POS                 (0)
#define ISO7816_WAITING_TIME_MSK                 (0xFFFF << ISO7816_WAITING_TIME_POS)
#define ISO7816_WAITING_TIME_CLR                 (~ISO7816_WAITING_TIME_MSK)

/* Register: THR ---------------------------------------------------------*/
/* Description: Threshold and Watermark Configuration Register. Offset: 0x10. Address: 0x40024810. */

/* THR[11:8]: ISO7816_AF_WATER_LEVEL. */
#define ISO7816_AF_WATER_LEVEL_POS               (8)
#define ISO7816_AF_WATER_LEVEL_MSK               (0xF << ISO7816_AF_WATER_LEVEL_POS)
#define ISO7816_AF_WATER_LEVEL_CLR               (~ISO7816_AF_WATER_LEVEL_MSK)
/* THR[7:4]: ISO7816_RX_NACK_THD_LEVEL. */
#define ISO7816_RX_NACK_THD_LEVEL_POS            (4)
#define ISO7816_RX_NACK_THD_LEVEL_MSK            (0xF << ISO7816_RX_NACK_THD_LEVEL_POS)
#define ISO7816_RX_NACK_THD_LEVEL_CLR            (~ISO7816_RX_NACK_THD_LEVEL_MSK)
/* THR[3:0]: ISO7816_TX_NACK_THD_LEVEL. */
#define ISO7816_TX_NACK_THD_LEVEL_POS            (0)
#define ISO7816_TX_NACK_THD_LEVEL_MSK            (0xF << ISO7816_TX_NACK_THD_LEVEL_POS)
#define ISO7816_TX_NACK_THD_LEVEL_CLR            (~ISO7816_TX_NACK_THD_LEVEL_MSK)

/* Register: ISR ---------------------------------------------------------*/
/* Description: Interrupt Status Register. Offset: 0x14. Address: 0x40024814. */


/* Register: IER -----------------------------------------------------*/
/* Description: Interrupt Enable Register . Offset: 0x18. Address: 0x40024818. */


/* Register: TSR -----------------------------------------------------*/
/* Description: Transmitter Status Register. Offset: 0x24. Address: 0x40024824. */


/* Register: RSR -----------------------------------------------------*/
/* Description: Receiver Status Register. Offset: 0x28. Address: 0x40024828. */


/* Register: ESR ----------------------------------------------------*/
/* Description: Error Status Register. Offset: 0x2C. Address: 0x4002482C. */


/* Register: CMD -----------------------------------------------------*/
/* Description: ISO7816 Enable Register. Offset: 0x30. Address: 0x40024830. */

/* CMD[0]: ISO7816_ENABLE. */
#define ISO7816_ENABLE_POS                 (0)
#define ISO7816_ENABLE_MSK                 (0x1 << ISO7816_ENABLE_POS)
#define ISO7816_ENABLE_CLR                 (~ISO7816_ENABLE_MSK)

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* RTL876X_ISO7816_DEF_H */
