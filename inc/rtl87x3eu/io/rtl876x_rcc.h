/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_rcc.h
* @brief     header file of reset and clock control driver.
* @details
* @author   colin
* @date     2024-06-14
* @version  v1.0
* *********************************************************************************************************
*/


#ifndef _RTL876X_RCC_H_
#define _RTL876X_RCC_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"

/** @addtogroup RCC RCC
  * @brief RCC driver module
  * @{
  */

/** @defgroup RCC_Exported_Constants RCC Exported Constants
  * @{
  */

/*============================================================================*
 *                              Macros  BB2Ultra_IO_module_20241008_v0.xlsx
 *============================================================================*/

/** @defgroup RCC_Peripheral_Clock  RCC Peripheral Clock
  * @{
  */
/*start  offset address  28| --> bit28, (0x01 << 29) --> adress 0x230,(0x02 << 29) -->0x234, 0x00 << 10) -->0x00 sleep clock cfg?  yes*/
#define APBPeriph_SLEEP_CLOCK_EXIST       (0)
#define APBPeriph_SLEEP_CLOCK_NOT_EXIST   (BIT10)

/* 0x220 */
#define APBPeriph_I2S2_CLOCK              ((uint32_t)(1 << 12))
#define APBPeriph_I2S1_CLOCK              ((uint32_t)((1 << 6) | (1 << 8)))
#define APBPeriph_I2S0_CLOCK              ((uint32_t)((1 << 5) | (1 << 8)))
#define APBPeriph_CODEC_CLOCK             ((uint32_t)(1 << 4))

/* 0x230 */
#define APBPeriph_SD_HOST1_CLOCK            ((uint32_t)( 28 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_SD_HOST_CLOCK             ((uint32_t)( 26 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_GPIOA_CLOCK               ((uint32_t)( 24 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_GPIO_CLOCK                APBPeriph_GPIOA_CLOCK
#define APBPeriph_GPIOB_CLOCK               ((uint32_t)( 22 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_FLASH2_CLOCK              ((uint32_t)( 20 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_FLASH1_CLOCK              ((uint32_t)( 18 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_GDMA_CLOCK                ((uint32_t)( 16 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_TIMER_CLOCK               ((uint32_t)( 14 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_TIMERA_CLOCK              APBPeriph_TIMER_CLOCK
#define APBPeriph_FLASH3_CLOCK              ((uint32_t)( 12 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_AHBC_CLOCK                ((uint32_t)( 10 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_FLASH_CLOCK               ((uint32_t)(  8 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_VENDOR_REG_CLOCK          ((uint32_t)(  6 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_CKE_BTV_CLOCK             ((uint32_t)(  5 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_BUS_RAM_SLP_CLOCK         ((uint32_t)(  4 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CKE_CTRLAP_CLOCK          ((uint32_t)(  3 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CKE_PLFM_CLOCK            ((uint32_t)(  2 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_GPIO1_DEB_CLOCK           ((uint32_t)(  1 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_GPIO0_DEB_CLOCK           ((uint32_t)(  0 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))

/* 0x234 */
#define APBPeriph_IDU_CLOCK                 ((uint32_t)( 30 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_PKE_CLOCK                 ((uint32_t)( 28 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_PPE_CLOCK                 ((uint32_t)( 26 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_CAN0_CLOCK                ((uint32_t)( 24 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_SPI2_CLOCK                ((uint32_t)( 22 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_IR_CLOCK                  ((uint32_t)( 20 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_SPI1_CLOCK                ((uint32_t)( 18 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_SPI0_CLOCK                ((uint32_t)( 16 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_TIMER1_CLOCK              ((uint32_t)( 14 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_GMAC_CLOCK                ((uint32_t)( 13 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CKE_SM3_CLOCK             ((uint32_t)( 12 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CKE_SHA256_CLOCK          ((uint32_t)( 11 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CKE_AAC_XTAL_CLOCK        ((uint32_t)( 10 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CKE_PDCK_CLOCK            ((uint32_t)(  9 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_RNG_CLOCK                 ((uint32_t)(  8 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_SWR_SS_CLOCK              ((uint32_t)(  6 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CAL_32K_CLOCK             ((uint32_t)(  5 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CKE_MODEM_CLOCK           ((uint32_t)(  4 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_SPI0_SLAVE_CLOCK          ((uint32_t)(  2 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_2P4G_CLOCK                ((uint32_t)(  0 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))

/* 0x238 */
#define APBPeriph_EFUSE_CLOCK             ((uint32_t)( 31 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CKE_DSP_WDT_CLOCK       ((uint32_t)( 30 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CKE_DSP_CLOCK           ((uint32_t)( 28 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_CKE_H2D_D2H             ((uint32_t)( 26 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_ADC_CLOCK               ((uint32_t)( 24 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_DSP_MEM_CLOCK           ((uint32_t)( 22 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_ASRC_CLOCK              ((uint32_t)( 20 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_DISP_CLOCK              ((uint32_t)( 18 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_DATA_MEM1_CLOCK         ((uint32_t)( 16 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_DATA_MEM0_CLOCK         ((uint32_t)( 14 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_I2C2_CLOCK              ((uint32_t)( 12 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_SIMC_CLOCK              ((uint32_t)( 10 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_ISO7816_CLOCK           APBPeriph_SIMC_CLOCK
#define APBPeriph_AES_CLOCK               ((uint32_t)(  8 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_KEYSCAN_CLOCK           ((uint32_t)(  6 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_QDEC_CLOCK              ((uint32_t)(  4 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_I2C1_CLOCK              ((uint32_t)(  2 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_I2C0_CLOCK              ((uint32_t)(  0 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))

/* 0x23C */
#define APBPeriph_CAN1_CLOCK              ((uint32_t)( 30 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_UART5_CLOCK             ((uint32_t)( 28 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_UART4_CLOCK             ((uint32_t)( 26 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_UART3_CLOCK             ((uint32_t)( 24 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_UART2_CLOCK             ((uint32_t)( 22 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_UART1_CLOCK             ((uint32_t)( 20 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_UART0_CLOCK             ((uint32_t)( 18 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_JPEG_CLOCK              ((uint32_t)( 16 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_ZIGBEE_CLOCK            ((uint32_t)( 14 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_CAN2_CLOCK              ((uint32_t)( 12 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))

/* 0x244 */
#define APBPeriph_BTBUS_CLOCK             ((uint32_t)( 0 | (0x06UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))

#define IS_APB_PERIPH_CLOCK(CLOCK)  (((CLOCK) == APBPeriph_I2S2_CLOCK) || ((CLOCK) == APBPeriph_I2S1_CLOCK)\
                                     || ((CLOCK) == APBPeriph_I2S0_CLOCK)|| ((CLOCK) == APBPeriph_CODEC_CLOCK) \
                                     || ((CLOCK) == APBPeriph_SD_HOST_CLOCK)|| ((CLOCK) == APBPeriph_GPIOA_CLOCK)\
                                     || ((CLOCK) == APBPeriph_GPIOB_CLOCK) || ((CLOCK) == APBPeriph_FLASH2_CLOCK)\
                                     || ((CLOCK) == APBPeriph_FLASH1_CLOCK) || ((CLOCK) == APBPeriph_GDMA_CLOCK)\
                                     || ((CLOCK) == APBPeriph_TIMER_CLOCK) || ((CLOCK) == APBPeriph_FLASH_CLOCK)\
                                     || ((CLOCK) == APBPeriph_VENDOR_REG_CLOCK) || ((CLOCK) == APBPeriph_CKE_BTV_CLOCK)\
                                     || ((CLOCK) == APBPeriph_BUS_RAM_SLP_CLOCK) || ((CLOCK) == APBPeriph_CKE_CTRLAP_CLOCK)\
                                     || ((CLOCK) == APBPeriph_CKE_PLFM_CLOCK)|| ((CLOCK) == APBPeriph_GPIO1_DEB_CLOCK)\
                                     || ((CLOCK) == APBPeriph_GPIO0_DEB_CLOCK) || ((CLOCK) == APBPeriph_IDU_CLOCK)\
                                     || ((CLOCK) == APBPeriph_PKE_CLOCK)|| ((CLOCK) == APBPeriph_PPE_CLOCK)\
                                     || ((CLOCK) == APBPeriph_CAN0_CLOCK)||(CLOCK) == (APBPeriph_SPI2_CLOCK)\
                                     || ((CLOCK) ==APBPeriph_IR_CLOCK ) || (CLOCK) == (APBPeriph_SPI1_CLOCK)\
                                     || ((CLOCK) == APBPeriph_SPI0_CLOCK)||((CLOCK) == APBPeriph_TIMER1_CLOCK)\
                                     || ((CLOCK) == APBPeriph_CKE_SM3_CLOCK)|| ((CLOCK) == APBPeriph_CKE_SHA256_CLOCK)\
                                     || ((CLOCK) == APBPeriph_CKE_AAC_XTAL_CLOCK) || ((CLOCK) == APBPeriph_CKE_PDCK_CLOCK)\
                                     || ((CLOCK) == APBPeriph_RNG_CLOCK)||((CLOCK) == APBPeriph_SWR_SS_CLOCK)\
                                     || ((CLOCK) == APBPeriph_CAL_32K_CLOCK) || ((CLOCK) == APBPeriph_CKE_MODEM_CLOCK)\
                                     || ((CLOCK) == APBPeriph_SPI0_SLAVE_CLOCK) || ((CLOCK) == APBPeriph_EFUSE_CLOCK)\
                                     || ((CLOCK) == APBPeriph_DSP_MEM_CLOCK) || ((CLOCK) == APBPeriph_ASRC_CLOCK)\
                                     || ((CLOCK) == APBPeriph_DISP_CLOCK) || ((CLOCK) == APBPeriph_DATA_MEM1_CLOCK)\
                                     || ((CLOCK) == APBPeriph_DATA_MEM0_CLOCK) || ((CLOCK) == APBPeriph_CAN2_CLOCK)\
                                     || ((CLOCK) == APBPeriph_I2C2_CLOCK) || ((CLOCK) == APBPeriph_SIMC_CLOCK)\
                                     || ((CLOCK) == APBPeriph_AES_CLOCK)||((CLOCK) == APBPeriph_KEYSCAN_CLOCK)\
                                     || ((CLOCK) == APBPeriph_QDEC_CLOCK)||((CLOCK) == APBPeriph_I2C1_CLOCK )\
                                     || ((CLOCK) == APBPeriph_I2C0_CLOCK) || ((CLOCK) == APBPeriph_CAN1_CLOCK )\
                                     || ((CLOCK) == APBPeriph_UART5_CLOCK) || ((CLOCK) == APBPeriph_UART4_CLOCK )\
                                     || ((CLOCK) == APBPeriph_UART3_CLOCK) || ((CLOCK) == APBPeriph_UART2_CLOCK )\
                                     || ((CLOCK) == APBPeriph_UART1_CLOCK )|| ((CLOCK) == APBPeriph_UART0_CLOCK )\
                                     || ((CLOCK) == APBPeriph_JPEG_CLOCK )|| ((CLOCK) == APBPeriph_ZIGBEE_CLOCK )\
                                     || ((CLOCK) == APBPeriph_BTBUS_CLOCK) || ((CLOCK) == APBPeriph_GMAC_CLOCK)\
                                     || ((CLOCK) == APBPeriph_SD_HOST1_CLOCK) || ((CLOCK) == APBPeriph_FLASH3_CLOCK)\
                                     || ((CLOCK) == APBPeriph_AHBC_CLOCK) || ((CLOCK) == APBPeriph_2P4G_CLOCK)\
                                    )

/** End of group RCC_Peripheral_Clock
  * @}
  */

/** @defgroup APB_Peripheral_Define APB Peripheral Define
  * @{
  */
/* 0x210 */
#define APBPeriph_TIMER1_DEB            ((uint32_t)( 31 | (0x00UL << 26)))
#define APBPeriph_ZIGBEE                ((uint32_t)( 30 | (0x00UL << 26)))
#define APBPeriph_GMAC                  ((uint32_t)( 29 | (0x00UL << 26)))
#define APBPeriph_JPEG                  ((uint32_t)( 28 | (0x00UL << 26)))
#define APBPeriph_CAN1                  ((uint32_t)( 27 | (0x00UL << 26)))
#define APBPeriph_IDU                   ((uint32_t)( 26 | (0x00UL << 26)))
#define APBPeriph_PKE                   ((uint32_t)( 25 | (0x00UL << 26)))
#define APBPeriph_PPE                   ((uint32_t)( 24 | (0x00UL << 26)))
#define APBPeriph_CAN0                  ((uint32_t)( 23 | (0x00UL << 26)))
#define APBPeriph_TIMER1                ((uint32_t)( 22 | (0x00UL << 26)))
#define APBPeriph_AAC_XTAL              ((uint32_t)( 20 | (0x00UL << 26)))
#define APBPeriph_PDCK                  ((uint32_t)( 19 | (0x00UL << 26)))
#define APBPeriph_SWR_SS                ((uint32_t)( 18 | (0x00UL << 26)))
#define APBPeriph_CAN2                  ((uint32_t)( 17 | (0x00UL << 26)))
#define APBPeriph_TIMER                 ((uint32_t)( 16 | (0x00UL << 26)))
#define APBPeriph_TIMERA                APBPeriph_TIMER
#define APBPeriph_USB                   ((uint32_t)( 15 | (0x00UL << 26)))
#define APBPeriph_SD_HOST               ((uint32_t)( 14 | (0x00UL << 26)))
#define APBPeriph_GDMA                  ((uint32_t)( 13 | (0x00UL << 26)))
#define APBPeriph_UART5                 ((uint32_t)( 12 | (0x00UL << 26)))
#define APBPeriph_UART4                 ((uint32_t)( 11 | (0x00UL << 26)))
#define APBPeriph_UART3                 ((uint32_t)( 10 | (0x00UL << 26)))
#define APBPeriph_UART2                 ((uint32_t)( 9  | (0x00UL << 26)))
#define APBPeriph_UART1                 ((uint32_t)( 8  | (0x00UL << 26)))
#define APBPeriph_UART0                 ((uint32_t)( 7  | (0x00UL << 26)))
#define APBPeriph_FLASH2                ((uint32_t)( 6  | (0x00UL << 26)))
#define APBPeriph_FLASH1                ((uint32_t)( 5  | (0x00UL << 26)))
#define APBPeriph_FLASH                 ((uint32_t)( 4  | (0x00UL << 26)))
#define APBPeriph_FLASH3                ((uint32_t)( 3  | (0x00UL << 26)))
#define APBPeriph_BTBUS                 ((uint32_t)( 2  | (0x00UL << 26)))
#define APBPeriph_SD_HOST1              ((uint32_t)( 1  | (0x00UL << 26)))
#define APBPeriph_2P4G                  ((uint32_t)( 0  | (0x00UL << 26)))

/* 0x218 */
#define APBPeriph_DATA_MEM              ((uint32_t)( 31 | (0x02UL << 26)))
#define APBPeriph_EFUSE                 ((uint32_t)( 30 | (0x02UL << 26)))
#define APBPeriph_DSP_WDT               ((uint32_t)( 29 | (0x02UL << 26)))
#define APBPeriph_ASRC                  ((uint32_t)( 28 | (0x02UL << 26)))
#define APBPeriph_DSP_MEM               ((uint32_t)( 27 | (0x02UL << 26)))
#define APBPeriph_DSP_H2D_D2H           ((uint32_t)( 26 | (0x02UL << 26)))
#define APBPeriph_DSP_CORE              ((uint32_t)( 25 | (0x02UL << 26)))
#define APBPeriph_SPI0_SLAVE            ((uint32_t)( 24 | (0x02UL << 26)))
#define APBPeriph_PSRAM3                ((uint32_t)( 23 | (0x02UL << 26)))
#define APBPeriph_PSRAM                 ((uint32_t)( 22 | (0x02UL << 26)))
#define APBPeriph_I2C2                  ((uint32_t)( 20 | (0x02UL << 26)))
#define APBPeriph_KEYSCAN               ((uint32_t)( 19 | (0x02UL << 26)))
#define APBPeriph_QDEC                  ((uint32_t)( 18 | (0x02UL << 26)))
#define APBPeriph_I2C1                  ((uint32_t)( 17 | (0x02UL << 26)))
#define APBPeriph_I2C0                  ((uint32_t)( 16 | (0x02UL << 26)))
#define APBPeriph_SPI2                  ((uint32_t)( 11 | (0x02UL << 26)))
#define APBPeriph_IR                    ((uint32_t)( 10 | (0x02UL << 26)))
#define APBPeriph_SPI1                  ((uint32_t)( 9  | (0x02UL << 26)))
#define APBPeriph_SPI0                  ((uint32_t)( 8  | (0x02UL << 26)))
#define APBPeriph_SM3                   ((uint32_t)( 7  | (0x02UL << 26)))
#define APBPeriph_SHA256                ((uint32_t)( 6  | (0x02UL << 26)))
#define APBPeriph_DISP                  ((uint32_t)( 5  | (0x02UL << 26)))
#define APBPeriph_SIMC                  ((uint32_t)( 4  | (0x02UL << 26)))
#define APBPeriph_ISO7816               APBPeriph_SIMC
#define APBPeriph_RNG                   ((uint32_t)( 3  | (0x02UL << 26)))
#define APBPeriph_AES                   ((uint32_t)( 2  | (0x02UL << 26)))

/* 0x21C */
#define APBPeriph_TIMER1_9_PWM          ((uint32_t)( 13  | (0x03UL << 26)))
#define APBPeriph_TIMER1_8_PWM          ((uint32_t)( 12  | (0x03UL << 26)))
#define APBPeriph_TIMER1_7_PWM          ((uint32_t)( 11  | (0x03UL << 26)))
#define APBPeriph_TIMER1_6_PWM          ((uint32_t)( 10  | (0x03UL << 26)))
#define APBPeriph_GPIOB                 ((uint32_t)( 9  | (0x03UL << 26)))
#define APBPeriph_GPIOA                 ((uint32_t)( 8  | (0x03UL << 26)))
#define APBPeriph_GPIO                   APBPeriph_GPIOA
#define APBPeriph_ADC                   ((uint32_t)(0  | (0x03UL << 26)))

/* 0x220 */
#define APBPeriph_I2S2                  ((uint32_t)((1 << 10) | (0x04UL << 26)))
#define APBPeriph_I2S1                  ((uint32_t)((1 << 2)  | (0x04UL << 26)))
#define APBPeriph_I2S0                  ((uint32_t)((1 << 1)  | (0x04UL << 26)))
#define APBPeriph_CODEC                 ((uint32_t)((1 << 0)  | (0x04UL << 26)))

/* No periph function bit */
#define APBPeriph_NO_FUNCTION_BIT       (0xff)
#define APBPeriph_CKE_MODEM             (APBPeriph_NO_FUNCTION_BIT)
#define APBPeriph_VENDOR_REG            (APBPeriph_NO_FUNCTION_BIT)
#define APBPeriph_CKE_BTV               (APBPeriph_NO_FUNCTION_BIT)
#define APBPeriph_BUS_RAM_SLP           (APBPeriph_NO_FUNCTION_BIT)
#define APBPeriph_CKE_CTRLAP            (APBPeriph_NO_FUNCTION_BIT)
#define APBPeriph_CKE_PLFM              (APBPeriph_NO_FUNCTION_BIT)
#define APBPeriph_GPIO1_DEB             (APBPeriph_NO_FUNCTION_BIT)
#define APBPeriph_GPIO0_DEB             (APBPeriph_NO_FUNCTION_BIT)
#define APBPeriph_AHBC                  (APBPeriph_NO_FUNCTION_BIT)

#define IS_APB_PERIPH(PERIPH) (((PERIPH) == APBPeriph_TIMER1_DEB) || ((PERIPH) == APBPeriph_ZIGBEE)\
                               || ((PERIPH) == APBPeriph_GMAC)\
                               || ((PERIPH) == APBPeriph_JPEG) || ((PERIPH) == APBPeriph_CAN1)\
                               || ((PERIPH) == APBPeriph_IDU) || ((PERIPH) == APBPeriph_PKE)\
                               || ((PERIPH) == APBPeriph_PPE) || ((PERIPH) == APBPeriph_CAN0)\
                               || ((PERIPH) == APBPeriph_TIMER1)\
                               || ((PERIPH) == APBPeriph_AAC_XTAL)|| ((PERIPH) == APBPeriph_PDCK)\
                               || ((PERIPH) == APBPeriph_SWR_SS) || ((PERIPH) == APBPeriph_TIMER)\
                               || ((PERIPH) == APBPeriph_USB) || ((PERIPH) == APBPeriph_SD_HOST)\
                               || ((PERIPH) == APBPeriph_GDMA) || ((PERIPH) == APBPeriph_UART5)\
                               || ((PERIPH) == APBPeriph_UART4) || ((PERIPH) == APBPeriph_UART3)\
                               || ((PERIPH) == APBPeriph_UART2) || ((PERIPH) == APBPeriph_UART1)\
                               || ((PERIPH) == APBPeriph_UART0) || ((PERIPH) == APBPeriph_FLASH2)\
                               || ((PERIPH) == APBPeriph_FLASH1) || ((PERIPH) == APBPeriph_FLASH)\
                               || ((PERIPH) == APBPeriph_BTBUS)|| ((PERIPH) == APBPeriph_DATA_MEM)\
                               || ((PERIPH) == APBPeriph_EFUSE) || ((PERIPH) == APBPeriph_DSP_WDT)\
                               || ((PERIPH) == APBPeriph_ASRC) || ((PERIPH) == APBPeriph_DSP_MEM)\
                               || ((PERIPH) == APBPeriph_DSP_H2D_D2H)\
                               || ((PERIPH) == APBPeriph_DSP_CORE) || ((PERIPH) == APBPeriph_SPI0_SLAVE)\
                               |  ((PERIPH) == APBPeriph_PSRAM) || ((PERIPH) == APBPeriph_I2C2)\
                               || ((PERIPH) == APBPeriph_KEYSCAN) || ((PERIPH) == APBPeriph_QDEC)\
                               || ((PERIPH) == APBPeriph_I2C1)|| ((PERIPH) == APBPeriph_I2C0)\
                               || ((PERIPH) == APBPeriph_SPI2) || ((PERIPH) == APBPeriph_IR)\
                               || (PERIPH == APBPeriph_SPI1) || (PERIPH == APBPeriph_SPI0)\
                               || (PERIPH == APBPeriph_SM3) || (PERIPH == APBPeriph_SHA256)\
                               || (PERIPH == APBPeriph_DISP) || (PERIPH == APBPeriph_SIMC)\
                               || (PERIPH == APBPeriph_RNG)|| (PERIPH == APBPeriph_AES) \
                               || (PERIPH == APBPeriph_GPIOB)|| (PERIPH == APBPeriph_GPIOA) \
                               || (PERIPH == APBPeriph_ADC)|| (PERIPH == APBPeriph_I2S2) \
                               || (PERIPH == APBPeriph_I2S1)|| (PERIPH == APBPeriph_I2S0) \
                               || (PERIPH == APBPeriph_CODEC)|| (PERIPH == APBPeriph_CKE_MODEM) \
                               || (PERIPH == APBPeriph_VENDOR_REG)|| (PERIPH == APBPeriph_CKE_BTV) \
                               || (PERIPH == APBPeriph_BUS_RAM_SLP)|| (PERIPH == APBPeriph_CKE_CTRLAP) \
                               || (PERIPH == APBPeriph_CKE_PLFM)|| (PERIPH == APBPeriph_GPIO1_DEB) \
                               || (PERIPH == APBPeriph_GPIO0_DEB) || (PERIPH == APBPeriph_CAN2) \
                               || (PERIPH == APBPeriph_FLASH3) || (PERIPH == APBPeriph_SD_HOST1)\
                               || (PERIPH == APBPeriph_2P4G) || (PERIPH == APBPeriph_PSRAM3)\
                               || (PERIPH == APBPeriph_TIMER1_6_PWM) || (PERIPH == APBPeriph_TIMER1_7_PWM)\
                               || (PERIPH == APBPeriph_TIMER1_8_PWM) || (PERIPH == APBPeriph_TIMER1_9_PWM)\
                               || (PERIPH == APBPeriph_AHBC))

/** End of group APB_Peripheral_Define
  * @}
  */

/** @defgroup RCC_Exported_Macros RCC Exported Macros
  * @{
  */

/** @defgroup 87x3eu_Display_Clock_Source Display Clock Source
  * @{
  */

#define DISPLAY_CLOCK_SOURCE_PLL1                               ((uint16_t) 0x0)
#define DISPLAY_CLOCK_SOURCE_PLL2                               ((uint16_t) 0x1)
#define DISPLAY_CLOCK_SOURCE_PLL3                               ((uint16_t) 0x3)
#define DISPLAY_CLOCK_SOURCE_40MHZ                              ((uint16_t) 0x4)
#define IS_DISPLAY_CLOCK_SOURCE(CLOCK)                          (((CLOCK) == DISPLAY_CLOCK_SOURCE_PLL1) || \
                                                                 ((CLOCK) == DISPLAY_CLOCK_SOURCE_PLL2) || \
                                                                 ((CLOCK) == DISPLAY_CLOCK_SOURCE_PLL3) || \
                                                                 ((CLOCK) == DISPLAY_CLOCK_SOURCE_40MHZ))

/** End of group 87x3eu_Display_Clock_Source
  * @}
  */

/** @defgroup 87x3eu_Display_Clock_Divider Display Clock Divider
  * @{
  */

#define DISPLAY_CLOCK_DIV_1                     ((uint16_t)0x0)
#define DISPLAY_CLOCK_DIV_2                     ((uint16_t)0x1)
#define DISPLAY_CLOCK_DIV_4                     ((uint16_t)0x2)
#define DISPLAY_CLOCK_DIV_8                     ((uint16_t)0x3)
#define DISPLAY_CLOCK_DIV_16                    ((uint16_t)0x4)
#define DISPLAY_CLOCK_DIV_32                    ((uint16_t)0x5)
#define DISPLAY_CLOCK_DIV_40                    ((uint16_t)0x6)
#define DISPLAY_CLOCK_DIV_64                    ((uint16_t)0x7)
#define IS_DISPLAY_DIV(DIV)                     (((DIV) == DISPLAY_CLOCK_DIV_1) || \
                                                 ((DIV) == DISPLAY_CLOCK_DIV_2) || \
                                                 ((DIV) == DISPLAY_CLOCK_DIV_4) || \
                                                 ((DIV) == DISPLAY_CLOCK_DIV_8) || \
                                                 ((DIV) == DISPLAY_CLOCK_DIV_16) || \
                                                 ((DIV) == DISPLAY_CLOCK_DIV_32) || \
                                                 ((DIV) == DISPLAY_CLOCK_DIV_40) || \
                                                 ((DIV) == DISPLAY_CLOCK_DIV_64))

/** End of group 87x3eu_Display_Clock_Divider
  * @}
  */

/**End of group RCC_Exported_Macros
  * @}
  */

/** @defgroup RCC_Exported_Functions RCC Exported Functions
  * @{
  */
/**
  * @brief  Enables or disables the APB peripheral clock.
  * @param  APBPeriph: specifies the APB peripheral to gates its clock.
  *      this parameter can be one of the following values:
  *     @arg APBPeriph_TIMER1_DEB
  *     @arg APBPeriph_ZIGBEE
  *     @arg APBPeriph_GMAC
  *     @arg APBPeriph_CAN1
  *     @arg APBPeriph_IDU
  *     @arg APBPeriph_PKE
  *     @arg APBPeriph_PPE
  *     @arg APBPeriph_CAN0
  *     @arg APBPeriph_TIMER1
  *     @arg APBPeriph_AAC_XTAL
  *     @arg APBPeriph_PDCK
  *     @arg APBPeriph_SWR_SS
  *     @arg APBPeriph_TIMER
  *     @arg APBPeriph_USB
  *     @arg APBPeriph_SD_HOST
  *     @arg APBPeriph_GDMA
  *     @arg APBPeriph_UART5
  *     @arg APBPeriph_UART4
  *     @arg APBPeriph_UART3
  *     @arg APBPeriph_UART2
  *     @arg APBPeriph_UART1
  *     @arg APBPeriph_UART0
  *     @arg APBPeriph_FLASH2
  *     @arg APBPeriph_FLASH1
  *     @arg APBPeriph_FLASH
  *     @arg APBPeriph_BTBUS
  *     @arg APBPeriph_DATA_MEM
  *     @arg APBPeriph_EFUSE
  *     @arg APBPeriph_DSP_WDT
  *     @arg APBPeriph_ASRC
  *     @arg APBPeriph_DSP_MEM
  *     @arg APBPeriph_DSP_H2D_D2H
  *     @arg APBPeriph_DSP_CORE
  *     @arg APBPeriph_SPI0_SLAVE
  *     @arg APBPeriph_PSRAM
  *     @arg APBPeriph_I2C2
  *     @arg APBPeriph_KEYSCAN
  *     @arg APBPeriph_QDEC
  *     @arg APBPeriph_I2C1
  *     @arg APBPeriph_I2C0
  *     @arg APBPeriph_SPI2
  *     @arg APBPeriph_IR
  *     @arg APBPeriph_SPI1
  *     @arg APBPeriph_SPI0
  *     @arg APBPeriph_SM3
  *     @arg APBPeriph_SHA256
  *     @arg APBPeriph_DISP
  *     @arg APBPeriph_ISO7816
  *     @arg APBPeriph_RNG
  *     @arg APBPeriph_AES
  *     @arg APBPeriph_GPIOB
  *     @arg APBPeriph_GPIOA
  *     @arg APBPeriph_ADC
  *     @arg APBPeriph_I2S2
  *     @arg APBPeriph_I2S1
  *     @arg APBPeriph_I2S0
  *     @arg APBPeriph_CODEC
  *     @arg APBPeriph_CKE_MODEM
  *     @arg APBPeriph_VENDOR_REG
  *     @arg APBPeriph_CKE_BTV
  *     @arg APBPeriph_BUS_RAM_SLP
  *     @arg APBPeriph_CKE_CTRLAP
  *     @arg APBPeriph_CKE_PLFM
  *     @arg APBPeriph_GPIO1_DEB
  *     @arg APBPeriph_GPIO0_DEB
  *     @arg APBPeriph_CAN2
  *     @arg APBPeriph_FLASH3
  *     @arg APBPeriph_SD_HOST1
  *     @arg APBPeriph_2P4G
  *     @arg APBPeriph_PSRAM3
  *     @arg APBPeriph_TIMER1_6_PWM
  *     @arg APBPeriph_TIMER1_7_PWM
  *     @arg APBPeriph_TIMER1_8_PWM
  *     @arg APBPeriph_TIMER1_9_PWM
  *     @arg APBPeriph_AHBC
  * @param  APBPeriph_Clock: specifies the APB peripheral clock config.
  *      this parameter can be one of the following values(must be the same with APBPeriph):
  *     @arg APBPeriph_I2S2_CLOCK
  *     @arg APBPeriph_I2S1_CLOCK
  *     @arg APBPeriph_I2S0_CLOCK
  *     @arg APBPeriph_CODEC_CLOCK
  *     @arg APBPeriph_SD_HOST_CLOCK
  *     @arg APBPeriph_GPIOA_CLOCK
  *     @arg APBPeriph_GPIOB_CLOCK
  *     @arg APBPeriph_FLASH2_CLOCK
  *     @arg APBPeriph_FLASH1_CLOCK
  *     @arg APBPeriph_GDMA_CLOCK
  *     @arg APBPeriph_TIMER_CLOCK
  *     @arg APBPeriph_FLASH_CLOCK
  *     @arg APBPeriph_VENDOR_REG_CLOCK
  *     @arg APBPeriph_CKE_BTV_CLOCK
  *     @arg APBPeriph_BUS_RAM_SLP_CLOCK
  *     @arg APBPeriph_CKE_CTRLAP_CLOCK
  *     @arg APBPeriph_CKE_PLFM_CLOCK
  *     @arg APBPeriph_GPIO1_DEB_CLOCK
  *     @arg APBPeriph_GPIO0_DEB_CLOCK
  *     @arg APBPeriph_IDU_CLOCK
  *     @arg APBPeriph_PKE_CLOCK
  *     @arg APBPeriph_PPE_CLOCK
  *     @arg APBPeriph_CAN0_CLOCK
  *     @arg APBPeriph_SPI2_CLOCK
  *     @arg APBPeriph_IR_CLOCK
  *     @arg APBPeriph_SPI1_CLOCK
  *     @arg APBPeriph_SPI0_CLOCK
  *     @arg APBPeriph_TIMER1_CLOCK
  *     @arg APBPeriph_CKE_SM3_CLOCK
  *     @arg APBPeriph_CKE_SHA256_CLOCK
  *     @arg APBPeriph_CKE_AAC_XTAL_CLOCK
  *     @arg APBPeriph_CKE_PDCK_CLOCK
  *     @arg APBPeriph_RNG_CLOCK
  *     @arg APBPeriph_SWR_SS_CLOCK
  *     @arg APBPeriph_CAL_32K_CLOCK
  *     @arg APBPeriph_CKE_MODEM_CLOCK
  *     @arg APBPeriph_SPI0_SLAVE_CLOCK
  *     @arg APBPeriph_EFUSE_CLOCK
  *     @arg APBPeriph_CKE_DSP_WDT_CLOCK
  *     @arg APBPeriph_CKE_DSP_CLOCK
  *     @arg APBPeriph_CKE_H2D_D2H
  *     @arg APBPeriph_ADC_CLOCK
  *     @arg APBPeriph_DSP_MEM_CLOCK
  *     @arg APBPeriph_ASRC_CLOCK
  *     @arg APBPeriph_DISP_CLOCK
  *     @arg APBPeriph_DATA_MEM1_CLOCK
  *     @arg APBPeriph_DATA_MEM0_CLOCK
  *     @arg APBPeriph_I2C2_CLOCK
  *     @arg APBPeriph_ISO7816_CLOCK
  *     @arg APBPeriph_AES_CLOCK
  *     @arg APBPeriph_KEYSCAN_CLOCK
  *     @arg APBPeriph_QDEC_CLOCK
  *     @arg APBPeriph_I2C1_CLOCK
  *     @arg APBPeriph_I2C0_CLOCK
  *     @arg APBPeriph_CAN1_CLOCK
  *     @arg APBPeriph_UART5_CLOCK
  *     @arg APBPeriph_UART4_CLOCK
  *     @arg APBPeriph_UART3_CLOCK
  *     @arg APBPeriph_UART2_CLOCK
  *     @arg APBPeriph_UART1_CLOCK
  *     @arg APBPeriph_UART0_CLOCK
  *     @arg APBPeriph_JPEG_CLOCK
  *     @arg APBPeriph_ZIGBEE_CLOCK
  *     @arg APBPeriph_BTBUS_CLOCK
  *     @arg APBPeriph_CAN2_CLOCK
  *     @arg APBPeriph_SD_HOST1_CLOCK
  *     @arg APBPeriph_FLASH3_CLOCK
  *     @arg APBPeriph_AHBC_CLOCK
  *     @arg APBPeriph_2P4G_CLOCK
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_PeriphClockCmd(uint32_t APBPeriph, uint32_t APBPeriph_Clock, FunctionalState NewState);

/**
  * @brief  Enables or disables the APB peripheral clock.
  * @param  APBPeriph_Clock: specifies the APB peripheral clock config.
  *      this parameter can be one of the following values(must be the same with APBPeriph):
*     @arg APBPeriph_I2S2_CLOCK
  *     @arg APBPeriph_I2S1_CLOCK
  *     @arg APBPeriph_I2S0_CLOCK
  *     @arg APBPeriph_CODEC_CLOCK
  *     @arg APBPeriph_SD_HOST_CLOCK
  *     @arg APBPeriph_GPIOA_CLOCK
  *     @arg APBPeriph_GPIOB_CLOCK
  *     @arg APBPeriph_FLASH2_CLOCK
  *     @arg APBPeriph_FLASH1_CLOCK
  *     @arg APBPeriph_GDMA_CLOCK
  *     @arg APBPeriph_TIMER_CLOCK
  *     @arg APBPeriph_FLASH_CLOCK
  *     @arg APBPeriph_VENDOR_REG_CLOCK
  *     @arg APBPeriph_CKE_BTV_CLOCK
  *     @arg APBPeriph_BUS_RAM_SLP_CLOCK
  *     @arg APBPeriph_CKE_CTRLAP_CLOCK
  *     @arg APBPeriph_CKE_PLFM_CLOCK
  *     @arg APBPeriph_GPIO1_DEB_CLOCK
  *     @arg APBPeriph_GPIO0_DEB_CLOCK
  *     @arg APBPeriph_IDU_CLOCK
  *     @arg APBPeriph_PKE_CLOCK
  *     @arg APBPeriph_PPE_CLOCK
  *     @arg APBPeriph_CAN0_CLOCK
  *     @arg APBPeriph_SPI2_CLOCK
  *     @arg APBPeriph_IR_CLOCK
  *     @arg APBPeriph_SPI1_CLOCK
  *     @arg APBPeriph_SPI0_CLOCK
  *     @arg APBPeriph_TIMER1_CLOCK
  *     @arg APBPeriph_CKE_SM3_CLOCK
  *     @arg APBPeriph_CKE_SHA256_CLOCK
  *     @arg APBPeriph_CKE_AAC_XTAL_CLOCK
  *     @arg APBPeriph_CKE_PDCK_CLOCK
  *     @arg APBPeriph_RNG_CLOCK
  *     @arg APBPeriph_SWR_SS_CLOCK
  *     @arg APBPeriph_CAL_32K_CLOCK
  *     @arg APBPeriph_CKE_MODEM_CLOCK
  *     @arg APBPeriph_SPI0_SLAVE_CLOCK
  *     @arg APBPeriph_EFUSE_CLOCK
  *     @arg APBPeriph_CKE_DSP_WDT_CLOCK
  *     @arg APBPeriph_CKE_DSP_CLOCK
  *     @arg APBPeriph_CKE_H2D_D2H
  *     @arg APBPeriph_ADC_CLOCK
  *     @arg APBPeriph_DSP_MEM_CLOCK
  *     @arg APBPeriph_ASRC_CLOCK
  *     @arg APBPeriph_DISP_CLOCK
  *     @arg APBPeriph_DATA_MEM1_CLOCK
  *     @arg APBPeriph_DATA_MEM0_CLOCK
  *     @arg APBPeriph_I2C2_CLOCK
  *     @arg APBPeriph_ISO7816_CLOCK
  *     @arg APBPeriph_AES_CLOCK
  *     @arg APBPeriph_KEYSCAN_CLOCK
  *     @arg APBPeriph_QDEC_CLOCK
  *     @arg APBPeriph_I2C1_CLOCK
  *     @arg APBPeriph_I2C0_CLOCK
  *     @arg APBPeriph_CAN1_CLOCK
  *     @arg APBPeriph_UART5_CLOCK
  *     @arg APBPeriph_UART4_CLOCK
  *     @arg APBPeriph_UART3_CLOCK
  *     @arg APBPeriph_UART2_CLOCK
  *     @arg APBPeriph_UART1_CLOCK
  *     @arg APBPeriph_UART0_CLOCK
  *     @arg APBPeriph_JPEG_CLOCK
  *     @arg APBPeriph_ZIGBEE_CLOCK
  *     @arg APBPeriph_BTBUS_CLOCK
  *     @arg APBPeriph_CAN2_CLOCK
  *     @arg APBPeriph_CAN2_CLOCK
  *     @arg APBPeriph_SD_HOST1_CLOCK
  *     @arg APBPeriph_FLASH3_CLOCK
  *     @arg APBPeriph_AHBC_CLOCK
  *     @arg APBPeriph_2P4G_CLOCK
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_PeriClockConfig(uint32_t APBPeriph_Clock, FunctionalState NewState);

/**
  * @brief  Enables or disables the APB peripheral clock.
  * @param  APBPeriph: specifies the APB peripheral to gates its clock.
  *      this parameter can be one of the following values:
  *     @arg APBPeriph_TIMER1_DEB
  *     @arg APBPeriph_ZIGBEE
  *     @arg APBPeriph_GMAC
  *     @arg APBPeriph_CAN1
  *     @arg APBPeriph_IDU
  *     @arg APBPeriph_PKE
  *     @arg APBPeriph_PPE
  *     @arg APBPeriph_CAN0
  *     @arg APBPeriph_TIMER1
  *     @arg APBPeriph_AAC_XTAL
  *     @arg APBPeriph_PDCK
  *     @arg APBPeriph_SWR_SS
  *     @arg APBPeriph_TIMER
  *     @arg APBPeriph_USB
  *     @arg APBPeriph_SD_HOST
  *     @arg APBPeriph_GDMA
  *     @arg APBPeriph_UART5
  *     @arg APBPeriph_UART4
  *     @arg APBPeriph_UART3
  *     @arg APBPeriph_UART2
  *     @arg APBPeriph_UART1
  *     @arg APBPeriph_UART0
  *     @arg APBPeriph_FLASH2
  *     @arg APBPeriph_FLASH1
  *     @arg APBPeriph_FLASH
  *     @arg APBPeriph_BTBUS
  *     @arg APBPeriph_DATA_MEM
  *     @arg APBPeriph_EFUSE
  *     @arg APBPeriph_DSP_WDT
  *     @arg APBPeriph_ASRC
  *     @arg APBPeriph_DSP_MEM
  *     @arg APBPeriph_DSP_H2D_D2H
  *     @arg APBPeriph_DSP_CORE
  *     @arg APBPeriph_SPI0_SLAVE
  *     @arg APBPeriph_PSRAM
  *     @arg APBPeriph_I2C2
  *     @arg APBPeriph_KEYSCAN
  *     @arg APBPeriph_QDEC
  *     @arg APBPeriph_I2C1
  *     @arg APBPeriph_I2C0
  *     @arg APBPeriph_SPI2
  *     @arg APBPeriph_IR
  *     @arg APBPeriph_SPI1
  *     @arg APBPeriph_SPI0
  *     @arg APBPeriph_SM3
  *     @arg APBPeriph_SHA256
  *     @arg APBPeriph_DISP
  *     @arg APBPeriph_ISO7816
  *     @arg APBPeriph_RNG
  *     @arg APBPeriph_AES
  *     @arg APBPeriph_GPIOB
  *     @arg APBPeriph_GPIOA
  *     @arg APBPeriph_ADC
  *     @arg APBPeriph_I2S2
  *     @arg APBPeriph_I2S1
  *     @arg APBPeriph_I2S0
  *     @arg APBPeriph_CODEC
  *     @arg APBPeriph_CKE_MODEM
  *     @arg APBPeriph_VENDOR_REG
  *     @arg APBPeriph_CKE_BTV
  *     @arg APBPeriph_BUS_RAM_SLP
  *     @arg APBPeriph_CKE_CTRLAP
  *     @arg APBPeriph_CKE_PLFM
  *     @arg APBPeriph_GPIO1_DEB
  *     @arg APBPeriph_GPIO0_DEB
  *     @arg APBPeriph_CAN2
  *     @arg APBPeriph_FLASH3
  *     @arg APBPeriph_SD_HOST1
  *     @arg APBPeriph_2P4G
  *     @arg APBPeriph_PSRAM3
  *     @arg APBPeriph_TIMER1_6_PWM
  *     @arg APBPeriph_TIMER1_7_PWM
  *     @arg APBPeriph_TIMER1_8_PWM
  *     @arg APBPeriph_TIMER1_9_PWM
  *     @arg APBPeriph_AHBC
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_PeriFunctionConfig(uint32_t APBPeriph, FunctionalState NewState);

/**
 * rtl876x_rcc.h
 *
 * \brief     Select the display clock source and divider.
 *
 * \param[in] ClockSource: Display clock source.
 *            This parameter can be one of the following values:
 *            \arg DISPLAY_CLOCK_SOURCE_PLL1: Select PLL1 as the clock source for display.
 *            \arg DISPLAY_CLOCK_SOURCE_PLL2: Select PLL2 as the clock source for display.
 *            \arg DISPLAY_CLOCK_SOURCE_PLL2: Select PLL3 as the clock source for display.
 *            \arg DISPLAY_CLOCK_SOURCE_40MHZ: Select 40MHz as the clock source for display.
 * \param[in] ClockDiv: Display clock divider.
 *            This parameter can be one of the following values:
 *            \arg DISPLAY_CLOCK_DIV_x: where x can be 1, 2, 4, 8, 16, 32, 40, 64 to select the specified clock divider.
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_xx_init(void)
 * {
 *     RCC_DisplayClockConfig(DISPLAY_CLOCK_SOURCE_40MHZ, DISPLAY_CLOCK_DIV_1);
 * }
 * \endcode
 */
extern void RCC_DisplayClockConfig(uint16_t ClockSource, uint16_t ClockDiv);
#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_RCC_H_ */

/**End of group RCC_Exported_Functions
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor *****END OF FILE****/
