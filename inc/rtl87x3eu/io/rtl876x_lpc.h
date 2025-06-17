/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_lpc.h
* @brief
* @details
* @author
* @date
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef __RTL876X_LPC_H
#define __RTL876X_LPC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_lpc_def.h"

/** @addtogroup LPC LPC
  * @brief Low Power Comparator driver module
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/


/** @defgroup LPC_Exported_Types LPC Exported Types
  * @{
  */

/**
  * @brief  LPC Init structure definition
  */
typedef struct
{
    uint16_t LPC_Channel;          /*!< Specifies the input pin.
                                                    This parameter can be a value of ADC_0 to ADC_7 */

    uint32_t LPC_Edge;             /*!< Specifies the comparator output edge */

    uint32_t LPC_Threshold;        /*!< Specifies the threshold value of comparator voltage. */

} LPC_InitTypeDef;

/** End of group LPC_Exported_Types
      * @}
      */

/*============================================================================*
 *                         Constants
 *============================================================================*/


/** @defgroup LPC_Exported_constants LPC Exported Constants
  * @{
  */

/** @defgroup LPC_Threshold LPC threshold
  * @{
  */
#define LPC_200_mV                 ((uint32_t)0x0000)
#define LPC_400_mV                 ((uint32_t)0x0001)
#define LPC_600_mV                 ((uint32_t)0x0002)
#define LPC_800_mV                 ((uint32_t)0x0003)
#define LPC_1000_mV                ((uint32_t)0x0004)
#define LPC_1200_mV                ((uint32_t)0x0005)
#define LPC_1400_mV                ((uint32_t)0x0006)
#define LPC_1600_mV                ((uint32_t)0x0007)
#define LPC_1800_mV                ((uint32_t)0x0008)
#define LPC_2000_mV                ((uint32_t)0x0009)
#define LPC_2200_mV                ((uint32_t)0x000a)
#define LPC_2400_mV                ((uint32_t)0x000b)
#define LPC_2600_mV                ((uint32_t)0x000c)
#define LPC_2800_mV                ((uint32_t)0x000d)
#define LPC_3000_mV                ((uint32_t)0x000e)
#define LPC_3200_mV                ((uint32_t)0x000f)

#define IS_LPC_THRESHOLD(THRESHOLD) (((THRESHOLD) == LPC_200_mV) || \
                                     ((THRESHOLD) == LPC_400_mV) || \
                                     ((THRESHOLD) == LPC_600_mV) || \
                                     ((THRESHOLD) == LPC_800_mV) || \
                                     ((THRESHOLD) == LPC_1000_mV) || \
                                     ((THRESHOLD) == LPC_1200_mV) || \
                                     ((THRESHOLD) == LPC_1400_mV) || \
                                     ((THRESHOLD) == LPC_1600_mV) || \
                                     ((THRESHOLD) == LPC_1800_mV) || \
                                     ((THRESHOLD) == LPC_2000_mV) || \
                                     ((THRESHOLD) == LPC_2200_mV) || \
                                     ((THRESHOLD) == LPC_2400_mV) || \
                                     ((THRESHOLD) == LPC_2800_mV) || \
                                     ((THRESHOLD) == LPC_3000_mV) || \
                                     ((THRESHOLD) == LPC_3200_mV))

/** End of group LPC_Threshold
      * @}
      */

/** @defgroup LPC_Channel LPC channel
  * @{
  */
#define IS_LPC_CHANNEL(CHANNEL) ((CHANNEL) <= 0x07)

/** End of group LPC_Channel
      * @}
      */

/** @defgroup LPC_Edge LPC output edge
  * @{
  */
#define LPC_Vin_Below_Vth               ((uint32_t)0x0000)
#define LPC_Vin_Over_Vth                ((uint32_t)0x0001)

#define IS_LPC_EDGE(EDGE) (((EDGE) == LPC_Vin_Below_Vth) || \
                           ((EDGE) == LPC_Vin_Over_Vth))

/** End of group LPC_Edge
      * @}
      */

/** @defgroup LPC_interrupts_definition LPC Interrupts Definition
      * @{
      */
#define LPC_INT_VOLTAGE_COMP                (BIT9)
#define LPC_INT_COUNT_COMP                  (BIT8)
#define LPC_INT_LPCOMP_AON                  (BIT19)

#define IS_LPC_CONFIG_INT(INT) (((INT) == LPC_INT_VOLTAGE_COMP) || \
                                ((INT) == LPC_INT_COUNT_COMP) || \
                                ((INT) == LPC_INT_LPCOMP_AON))
#define IS_LPC_CLEAR_INT(INT)  ((INT) == LPC_INT_COUNT_COMP)
#define IS_LPC_STATUS_INT(INT) (((INT) == LPC_INT_COUNT_COMP) || \
                                ((INT) == LPC_INT_LPCOMP_AON))

/** End of group LPC_interrupts_definition
      * @}
      */


/** End of group LPC_Exported_constants
      * @}
      */

/*============================================================================*
 *                         Functions
 *============================================================================*/


/** @defgroup LPC_Exported_Functions LPC Exported Functions
  * @{
  */

/**
  * @brief  Deinitializes the LPC peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void LPC_DeInit(void);

/**
  * @brief  Initializes LPC peripheral according to
  *    the specified parameters in the LPC_InitStruct.
  * @param  LPC_InitStruct: pointer to a LPC_InitTypeDef
  *    structure that contains the configuration information for the
  *    specified LPC peripheral.
  * @retval None
  */
void LPC_Init(LPC_InitTypeDef *LPC_InitStruct);

/**
  * @brief  Enables or disables LPC peripheral.
  * @param  NewState: new state of LPC peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void LPC_Cmd(FunctionalState NewState);

/**
  * @brief Start or stop the LPC counter.
  * @param  NewState: new state of the LPC counter.
  *   This parameter can be one of the following values:
  *     @arg ENABLE: Start LPCOMP counter.
  *     @arg DISABLE: Stop LPCOMP counter.
  * @retval None
  */
void LPC_CounterCmd(FunctionalState NewState);

/**
  * @brief Reset the LPC counter.
  * @retval None
  */
void LPC_CounterReset(void);

/**
  * @brief  Fills each LPC_InitStruct member with its default value.
  * @param  LPC_InitStruct : pointer to a LPC_InitTypeDef structure which will be initialized.
  * @retval None
  */
void LPC_StructInit(LPC_InitTypeDef *LPC_InitStruct);

/**
  * @brief  Configure LPCOMP counter's comparator value.
  * @param  data: LPCOMP counter's comparator value which can be 0 to 0xfff.
  * @retval None.
  */
void LPC_WriteComparator(uint32_t data);

/**
  * @brief  read LPCOMP comparator value.
  * @param none.
  * @retval LPCOMP comparator value.
  */
uint16_t LPC_ReadComparator(void);

/**
  * @brief  Read LPC counter value.
  * @retval LPCOMP counter value.
  */
uint16_t LPC_ReadCounter(void);

/**
  * @brief  Enables or disables the specified LPC interrupts.
  * @param  LPC_INT: specifies the LPC interrupt source to be enabled or disabled.
  *   This parameter can be one of the following values:
  *     @arg LPC_INT_VOLTAGE_COMP: voltage detection interrupt.If Vin<Vth, cause this interrupt.
  *     @arg LPC_INT_COUNT_COMP: couter comparator interrupt.
  * @param  NewState: new state of the specified LPC interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void LPC_INTConfig(uint32_t LPC_INT, FunctionalState NewState);

/**
  * @brief  Clear the specified LPC interrupt.
  * @param  LPC_INT: specifies the LPC interrupt to clear.
  *   This parameter can be one of the following values:
  *     @arg LPC_INT_COUNT_COMP: couter comparator interrupt.
  * @retval None.
  */
void LPC_ClearINTPendingBit(uint32_t LPC_INT);

/**
  * @brief  Checks whether the specified LPC interrupt is set or not.
  * @param  LPC_INT: specifies the LPC interrupt to check.
  *   This parameter can be one of the following values:
  *     @arg LPC_INT_COUNT_COMP: couter comparator interrupt.
  * @retval The new state of SPI_IT (SET or RESET).
  */
ITStatus LPC_GetINTStatus(uint32_t LPC_INT);

/**
  * @brief  Enable or disable LPC wake up function.
  * @param  NewState: New state of the LPC wake up function.
  * @retval None.
  */
void LPC_SystemWakeupConfig(FunctionalState NewState);

#ifdef __cplusplus
}
#endif

#endif /*__RTL876X_LPC_H*/

/** @} */ /* End of group LPC_Exported_Functions */
/** @} */ /* End of group LPC */


/******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/

