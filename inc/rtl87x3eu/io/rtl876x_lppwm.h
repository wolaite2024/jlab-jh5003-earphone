/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_lppwm.h
* @brief
* @details
* @author
* @date
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _RTL876X_LPPWM_H_
#define _RTL876X_LPPWM_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_lppwm_def.h"

/** @addtogroup 87x3e_LPPWM LPPWM
  * @brief LPPWM driver module
  * @{
  */

/** @defgroup LPPWM_Exported_Types LPPWM Exported Types
  * @{
  */

/**
 * @brief LPPWM initialize parameters
 *
 * LPPWM initialize parameters
 */
typedef struct
{
    uint8_t LPPWM_Polarity;           /*!< Specifies the LPPWM polarity.
                                         This parameter can be a value of @ref LPPWM_Output_polarity */
    uint16_t LPPWM_PeriodHigh;     /*!< Specifies the LPPWM period high.
                                         This parameter can be a value of 0-65535 */
    uint16_t LPPWM_PeriodLow;      /*!< Specifies the LPPWM period low.
                                         This parameter can be a value of 0-65535 */
} LPPWM_InitTypeDef;

/** End of group LPPWM_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/


/** @defgroup LPPWM_Exported_Constants LPPWM Exported Constants
  * @{
  */

/** @defgroup LPPWM_Output_polarity LPPWM output polarity
  * @{
  */

typedef enum
{
    LPPWM_POLARITY_NORMAL = 0x0,
    LPPWM_POLARITY_INVERT = 0x1,
} LPPWMOutputPolarity_TypeDef;

#define IS_LPPWM_OUTPUT_MODE(MODE)    (((MODE) == LPPWM_POLARITY_NORMAL) || \
                                       ((MODE) == LPPWM_POLARITY_INVERT))
/** End of group LPPWM_Output_polarity
  * @}
  */

/** End of group LPPWM_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/


/** @defgroup LPPWM_Exported_Functions LPPWM Exported Functions
  * @{
  */

/**
 * rtl876x_lppwm.h
 *
 * \brief   Clear all the LPPWM registers to their default reset values.
 *
 * \param[in] LPPWMx: Select the LPPWM peripheral.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_lppwm_init(void)
 * {
 *     LPPWM_Reset(LPPWM);
 * }
 * \endcode
 */
void LPPWM_Reset(LPPWM_TypeDef *LPPWMx);


/**
 * rtl876x_lppwm.h
 *
 * \brief  Initialize the LPPWM according to the specified
 *         parameters in the LPPWM_InitStruct.
 *
 * \param[in] LPPWMx: Select the LPPWM peripheral.
 * \param[in] LPPWM_InitStruct: Pointer to a LPPWM_InitTypeDef structure that
 *            contains the configuration information for the LPPWM.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_lppwm_init(void)
 * {
 *     LPPWM_Reset(LPPWM);
 *     LPPWM_InitTypeDef LPPWM_InitStruct;
 *     LPPWM_StructInit(&LPPWM_InitStruct);
 *     LPPWM_InitStruct.LPPWM_Polarity                   = LPPWM_POLARITY_NORMAL;
 *     LPPWM_InitStruct.LPPWM_PeriodHigh                 = 32;
 *     LPPWM_InitStruct.LPPWM_PeriodLow                  = 32;
 *     LPPWM_Init(LPPWM, &LPPWM_InitStruct);
 *     LPPWM_Cmd(LPPWM, ENABLE);
 * }
 * \endcode
 */
void LPPWM_Init(LPPWM_TypeDef *LPPWMx, LPPWM_InitTypeDef *LPPWM_InitStruct);


/**
 * rtl876x_lppwm.h
 *
 * \brief   Fill each LPPWM_InitStruct member with its default value.
 *
 * \param[in] LPPWM_InitStruct: Pointer to a LPPWM_InitTypeDef structure which will be initialized.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_lppwm_init(void)
 * {
 *     LPPWM_Reset(LPPWM);
 *     LPPWM_InitTypeDef LPPWM_InitStruct;
 *     LPPWM_StructInit(&LPPWM_InitStruct);
 *     LPPWM_InitStruct.LPPWM_Polarity                   = LPPWM_POLARITY_NORMAL;
 *     LPPWM_InitStruct.LPPWM_PeriodHigh                 = 32;
 *     LPPWM_InitStruct.LPPWM_PeriodLow                  = 32;
 *     LPPWM_Init(LPPWM, &LPPWM_InitStruct);
 *     LPPWM_Cmd(LPPWM, ENABLE);
 * }
 * \endcode
 */
void LPPWM_StructInit(LPPWM_InitTypeDef *LPPWM_InitStruct);

/**
 * rtl876x_lppwm.h
 *
 * \brief   Enable or disable the LPPWM.
 *
 * \param[in] LPPWMx: Select the LPPWM peripheral.
 * \param[in] NewState: New state of the LPPWM.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable the LPPWM.
 *            \arg DISABLE: Disable the LPPWM.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_lppwm_init(void)
 * {
 *     LPPWM_Cmd(LPPWM, ENABLE);
 * }
 * \endcode
 */
void LPPWM_Cmd(LPPWM_TypeDef *LPPWMx, FunctionalState NewState);

/**
 * rtl876x_lppwm.h
 *
 * \brief   Change LPPWM frequency and duty according period_high and period_low.
 *
 * \param[in] LPPWMx: Select the LPPWM peripheral.
 * \param[in] period_high: Specifies the LPPWM period high.
 * \param[in] period_low: Specifies the LPPWM period low.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_lppwm_init(void)
 * {
 *     uint32_t high_count = 500;
 *     uint32_t low_count = 500;
 *     LPPWM_ChangeFreqAndDuty(LPPWM, high_count, low_count);
 * }
 * \endcode
 */
void LPPWM_ChangeFreqAndDuty(LPPWM_TypeDef *LPPWMx, uint16_t period_high, uint16_t period_low);

/**
 * rtl876x_lppwm.h
 *
 * \brief   Get the current value of LPPWM's internal counter.
 *
 * \param[in] LPPWMx: Select the LPPWM peripheral.
 *
 * \return  Current value of LPPWM's internal counter.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_lppwm_init(void)
 * {
 *     uint32_t cur_value = LPPWM_GetCurrentValue(LPPWM);
 * }
 * \endcode
 */
uint32_t LPPWM_GetCurrentValue(LPPWM_TypeDef *LPPWMx);

/** @} */ /* End of group LPPWM_Exported_Functions */
/** @} */ /* End of group 87x3e_LPPWM */

#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_LPPWM_H_ */

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor *****END OF FILE****/

