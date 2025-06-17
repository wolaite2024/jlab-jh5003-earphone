/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_qdec.h
* @brief     The header file of the peripheral QDECODER driver.
* @details   This file provides all QDECODER firmware functions.
* @author    howie_wang
* @date      2024-07-18
* @version   v1.0
* *********************************************************************************************************
*/


#ifndef _RTL876X_QDECODER_H_
#define _RTL876X_QDECODER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "rtl876x.h"
#include "rtl876x_bitfields.h"

/** @addtogroup 87x3d_QDEC QDEC
  * @brief Qdecoder driver module.
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/

/** @defgroup QDEC_Exported_Types QDEC Exported Types
  * @{
  */

/**
 * @brief Qdecoder initialize parameters.
 */
typedef struct _X3D_QDEC_InitTypeDef
{
    uint16_t ScanClockKHZ;                 /*!< Specifies the scan clock(KHz).
                                                  Scan clock = source clock / (DIV+1), source clock is 20000KHz, DIV range is 0x0 to 0xFFF. */

    uint16_t DebonceClockKHZ;             /*!< Specifies the debounce clock(KHz).
                                                  Debounce clock = scan clock / (DEB_DIV+1), DEB_DIV range is 0x0 to 0xF. */

    uint8_t axisConfigX;                /*!< Specifies the axis X function.
                                                  This parameter can be a value of ENABLE or DISABLE. */
    uint8_t axisConfigY;                /*!< Specifies the axis Y function.
                                                  This parameter can be a value of ENABLE or DISABLE. */
    uint8_t axisConfigZ;                /*!< Specifies the axis Z function.
                                                  This parameter can be a value of ENABLE or DISABLE. */
    uint8_t autoLoadInitPhase;          /*!< Specifies Auto-load Initphase function.
                                                  This parameter can be a value of ENABLE or DISABLE. */
    uint16_t counterScaleX;             /*!< Specifies the axis X conter scale.
                                                  This parameter can be a value of @ref QDEC_Axis_counter_Scale. */
    uint16_t debounceEnableX;           /*!< Specifies the axis X debounce.
                                                  This parameter can be a value of @ref QDEC_Debounce. */
    uint8_t debounceTimeX;             /*!< Specifies the axis X debounce time.
                                                  This parameter can be a value from 0 to 0xFF. */
    uint16_t initPhaseX;                /*!< Specifies the axis X init phase.
                                                  This parameter can be a value of @ref QDEC_init_phase. */
    uint16_t counterScaleY;             /*!< Specifies the axis Y conter scale.
                                                  This parameter can be a value of @ref QDEC_Axis_counter_Scale. */
    uint16_t debounceEnableY;           /*!< Specifies the axis Y debounce.
                                                  This parameter can be a value of @ref QDEC_Debounce. */
    uint8_t debounceTimeY;             /*!< Specifies the axis Y debounce time.
                                                  This parameter can be a value from 0 to 0xFF. */
    uint16_t initPhaseY;                /*!< Specifies the axis Y init phase.
                                                  This parameter can be a value of @ref QDEC_init_phase.  */
    uint16_t counterScaleZ;             /*!< Specifies the axis Z conter scale.
                                                  This parameter can be a value of @ref QDEC_Axis_counter_Scale. */
    uint16_t debounceEnableZ;           /*!< Specifies the axis Z debounce.
                                                  This parameter can be a value of @ref QDEC_Debounce. */
    uint8_t debounceTimeZ;             /*!< Specifies the axis Z debounce time.
                                                  This parameter can be a value from 0 to 0xFF. */
    uint16_t initPhaseZ;                /*!< Specifies the axis Z init phase.
                                                  This parameter can be a value of @ref QDEC_init_phase. */
} QDEC_InitTypeDef;

/** End of group QDEC_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/


/** @defgroup QDEC_Exported_Constants QDEC Exported Constants
  * @{
  */

/** @defgroup QDEC_Periph Check QDEC Peripheral
  * @{
  */
#define IS_QDEC_PERIPH(PERIPH) ((PERIPH) == QDEC)   //!< Check whether is the QDEC peripheral.
/** End of group QDEC_Periph
  * @}
  */

/** @defgroup QDEC_Source_Clock QDEC Source Clock
  * @{
  */
#define QDEC_SOURCE_CLOCK       20000   //!< The clock source of QDEC(KHz). 
/** End of group QDEC_Source_Clock
  * @}
  */

/** @defgroup QDEC_Interrupts_Definition QDEC Interrupts Definition
  * @{
  */

#define QDEC_X_INT_NEW_DATA                                   BIT(0)    //!< The counter interrupt for the X axis triggers when the counter value changes.
#define QDEC_X_INT_ILLEAGE                                    BIT(1)    //!< The illegal interrupt for X axis.
#define QDEC_Y_INT_NEW_DATA                                   BIT(2)    //!< The counter interrupt for the Y axis triggers when the counter value changes.
#define QDEC_Y_INT_ILLEAGE                                    BIT(3)    //!< The illegal interrupt for Y axis.
#define QDEC_Z_INT_NEW_DATA                                   BIT(4)    //!< The counter interrupt for the Z axis triggers when the counter value changes.
#define QDEC_Z_INT_ILLEAGE                                    BIT(5)    //!< The illegal interrupt for Z axis.

#define IS_QDEC_INT_CONFIG(CONFIG) (((CONFIG) == QDEC_X_INT_NEW_DATA) || ((CONFIG) == QDEC_X_INT_ILLEAGE)\
                                    || ((CONFIG) == QDEC_Y_INT_NEW_DATA) || ((CONFIG) == QDEC_Y_INT_ILLEAGE)\
                                    || ((CONFIG) == QDEC_Z_INT_NEW_DATA) || ((CONFIG) == QDEC_Z_INT_ILLEAGE))   //!< Check whether is the QDEC interrupt.
/** End of group QDEC_Interrupts_Definition
  * @}
  */

/** @defgroup QDEC_Interrupts_Mask QDEC Interrupts Mask
  * @{
  */

#define QDEC_X_CT_INT_MASK                                   BIT(0)     //!< Mask the counter interrupt for the X axis.
#define QDEC_X_ILLEAGE_INT_MASK                              BIT(4)     //!< Mask the illegal interrupt for X axis.
#define QDEC_Y_CT_INT_MASK                                   BIT(1)     //!< Mask the counter interrupt for the Y axis.
#define QDEC_Y_ILLEAGE_INT_MASK                              BIT(5)     //!< Mask the illegal interrupt for Y axis.
#define QDEC_Z_CT_INT_MASK                                   BIT(2)     //!< Mask the counter interrupt for the Z axis.
#define QDEC_Z_ILLEAGE_INT_MASK                              BIT(6)     //!< Mask the illegal interrupt for Z axis.

#define IS_QDEC_INT_MASK_CONFIG(CONFIG) (((CONFIG) == QDEC_X_CT_INT_MASK) || ((CONFIG) == QDEC_X_ILLEAGE_INT_MASK)\
                                         || ((CONFIG) == QDEC_Y_CT_INT_MASK) || ((CONFIG) == QDEC_Y_ILLEAGE_INT_MASK)\
                                         || ((CONFIG) == QDEC_Z_CT_INT_MASK) || ((CONFIG) == QDEC_Z_ILLEAGE_INT_MASK))  //!< Check whether is the mask of QDEC interrupt.
/** End of group QDEC_Interrupts_Mask
  * @}
  */

/** @defgroup QDEC_Axis_counter_Scale QDEC Axis Counter
  * @{
  */

#define counterScaleEnable                        true      //!< Update counter when 2 phase change.
#define counterScaleDisable                       false     //!< Update counter when 1 phase change. 

/** End of group QDEC_Axis_counter_Scale
  * @}
  */

/** @defgroup QDEC_Debounce QDEC Debounce
  * @{
  */

#define Debounce_Enable                        true         //!< Enable debounce function.
#define Debounce_Disable                       false        //!< Disable debounce function.

/** End of group QDEC_Debounce
  * @}
  */

/** @defgroup QDEC_manual_phase QDEC Manual Phase
  * @{
  */

#define manualPhaseEnable                        true       //!< Enable manual set initial phase.
#define manualPhaseDisable                       false      //!< Disable manual set initial phase.

/** End of group QDEC_manual_phase
  * @}
  */

/** @defgroup QDEC_Init_Phase QDEC Init Phase
  * @{
  */

#define phaseMode0                        0       //!< Set the initial phase to 00.
#define phaseMode1                        1       //!< Set the initial phase to 01.
#define phaseMode2                        2       //!< Set the initial phase to 10.
#define phaseMode3                        3       //!< Set the initial phase to 11.

/** End of group QDEC_Init_Phase
  * @}
  */

/** @defgroup QDEC_Clr_Flag QDEC Clear Flag
  * @{
  */
#define QDEC_CLR_ILLEGAL_CT_X                            ((uint32_t)(1 << 20))      //!< Clear the illegal counter for X axis.
#define QDEC_CLR_ILLEGAL_CT_Y                            ((uint32_t)(1 << 21))      //!< Clear the illegal counter for Y axis.
#define QDEC_CLR_ILLEGAL_CT_Z                            ((uint32_t)(1 << 22))      //!< Clear the illegal counter for Z axis.

#define QDEC_CLR_ACC_CT_X                                ((uint32_t)(1 << 16))      //!< Clear the counter for X axis.
#define QDEC_CLR_ACC_CT_Y                                ((uint32_t)(1 << 17))      //!< Clear the counter for Y axis.
#define QDEC_CLR_ACC_CT_Z                                ((uint32_t)(1 << 18))      //!< Clear the counter for Z axis.

#define QDEC_CLR_ILLEGAL_INT_X                           ((uint32_t)(1 << 12))      //!< Clear the illegal interrupt for X axis.
#define QDEC_CLR_ILLEGAL_INT_Y                           ((uint32_t)(1 << 13))      //!< Clear the illegal interrupt for Y axis.
#define QDEC_CLR_ILLEGAL_INT_Z                           ((uint32_t)(1 << 14))      //!< Clear the illegal interrupt for Z axis.

#define QDEC_CLR_UNDERFLOW_X                             ((uint32_t)(1 << 8))       //!< Clear the unferflow interrupt for X axis.
#define QDEC_CLR_UNDERFLOW_Y                             ((uint32_t)(1 << 9))       //!< Clear the unferflow interrupt for Y axis.
#define QDEC_CLR_UNDERFLOW_Z                             ((uint32_t)(1 << 10))      //!< Clear the unferflow interrupt for Z axis.

#define QDEC_CLR_OVERFLOW_X                              ((uint32_t)(1 << 4))       //!< Clear the overflow interrupt for X axis.
#define QDEC_CLR_OVERFLOW_Y                              ((uint32_t)(1 << 5))       //!< Clear the overflow interrupt for Y axis.
#define QDEC_CLR_OVERFLOW_Z                              ((uint32_t)(1 << 6))       //!< Clear the overflow interrupt for Z axis.

#define QDEC_CLR_NEW_CT_X                                ((uint32_t)(1 << 0))       //!< Clear the counter interrupt for X axis.
#define QDEC_CLR_NEW_CT_Y                                ((uint32_t)(1 << 1))       //!< Clear the counter interrupt for Y axis.
#define QDEC_CLR_NEW_CT_Z                                ((uint32_t)(1 << 2))       //!< Clear the counter interrupt for Z axis.

#define IS_QDEC_INT_CLR_CONFIG(CONFIG) (((CONFIG) == QDEC_CLR_ACC_CT_X) || ((CONFIG) == QDEC_CLR_ACC_CT_Y)\
                                        || ((CONFIG) == QDEC_CLR_ACC_CT_Z) || ((CONFIG) == QDEC_CLR_ILLEGAL_INT_Y)\
                                        || ((CONFIG) == QDEC_CLR_ILLEGAL_INT_Z) || ((CONFIG) == QDEC_CLR_UNDERFLOW_X)\
                                        || ((CONFIG) == QDEC_CLR_UNDERFLOW_Y) || ((CONFIG) == QDEC_CLR_UNDERFLOW_Z)\
                                        || ((CONFIG) == QDEC_CLR_OVERFLOW_X) || ((CONFIG) == QDEC_CLR_OVERFLOW_Y)\
                                        || ((CONFIG) == QDEC_CLR_OVERFLOW_Z) || ((CONFIG) == QDEC_CLR_NEW_CT_X)\
                                        || ((CONFIG) == QDEC_CLR_NEW_CT_Y) || ((CONFIG) == QDEC_CLR_NEW_CT_Z))  //!< Check whether is the clear bit of QDEC.
/** End of group QDEC_Clr_Flag
  * @}
  */

/** @defgroup QDEC_Flag QDEC Flag
  * @{
  */

#define QDEC_FLAG_NEW_CT_STATUS_X                             ((uint32_t)(1 << 0))  //!< The counter flag for X axis.
#define QDEC_FLAG_NEW_CT_STATUS_Y                             ((uint32_t)(1 << 1))  //!< The counter flag for Y axis.
#define QDEC_FLAG_NEW_CT_STATUS_Z                             ((uint32_t)(1 << 2))  //!< The counter flag for Z axis.
#define QDEC_FLAG_OVERFLOW_X                                  ((uint32_t)(1 << 3))  //!< The overflow flag for X axis.
#define QDEC_FLAG_OVERFLOW_Y                                  ((uint32_t)(1 << 4))  //!< The overflow flag for Y axis.
#define QDEC_FLAG_OVERFLOW_Z                                  ((uint32_t)(1 << 5))  //!< The overflow flag for Z axis.
#define QDEC_FLAG_UNDERFLOW_X                                 ((uint32_t)(1 << 6))  //!< The underflow flag for X axis.
#define QDEC_FLAG_UNDERFLOW_Y                                 ((uint32_t)(1 << 7))  //!< The underflow flag for Y axis.
#define QDEC_FLAG_UNDERFLOW_Z                                 ((uint32_t)(1 << 8))  //!< The underflow flag for Z axis.
#define QDEC_FLAG_ILLEGAL_STATUS_X                            ((uint32_t)(1 << 12)) //!< The illegal flag for X axis.
#define QDEC_FLAG_ILLEGAL_STATUS_Y                            ((uint32_t)(1 << 13)) //!< The illegal flag for Y axis.
#define QDEC_FLAG_ILLEGAL_STATUS_Z                            ((uint32_t)(1 << 14)) //!< The illegal flag for Z axis.

#define IS_QDEC_STATUS_FLAG(QDEC_FLAG) (((QDEC_FLAG) == QDEC_FLAG_ILLEGAL_STATUS_X) || ((QDEC_FLAG) == QDEC_FLAG_ILLEGAL_STATUS_Y)\
                                        || ((QDEC_FLAG) == QDEC_FLAG_ILLEGAL_STATUS_Z) || ((QDEC_FLAG) == QDEC_FLAG_NEW_CT_STATUS_X)\
                                        || ((QDEC_FLAG) == QDEC_FLAG_NEW_CT_STATUS_Y) || ((QDEC_FLAG) == QDEC_FLAG_NEW_CT_STATUS_Z)\
                                        || ((QDEC_FLAG) == QDEC_FLAG_OVERFLOW_X) || ((QDEC_FLAG) == QDEC_FLAG_OVERFLOW_Y)\
                                        || ((QDEC_FLAG) == QDEC_FLAG_OVERFLOW_Z) || ((QDEC_FLAG) == QDEC_FLAG_UNDERFLOW_X)\
                                        || ((QDEC_FLAG) == QDEC_FLAG_UNDERFLOW_Y) || ((QDEC_FLAG) == QDEC_FLAG_UNDERFLOW_Z))    //!< Check whether is the flag of QDEC.


/** End of group QDEC_Flag
  * @}
  */

/** @defgroup QDEC_Axis QDEC Axis
  * @{
  */

#define QDEC_AXIS_X                                     ((uint32_t)(1 << 0))    //!< The X axis of QDEC.
#define QDEC_AXIS_Y                                     ((uint32_t)(1 << 2))    //!< The Y axis of QDEC.
#define QDEC_AXIS_Z                                     ((uint32_t)(1 << 3))    //!< The Z axis of QDEC.

/** End of group QDEC_Axis
  * @}
  */

/** @defgroup QDEC_Axis_Direction QDEC Axis Direction
  * @{
  */

#define QDEC_AXIS_DIR_UP                                 ((uint16_t)0x01)       //!< The direction of QDEC is up.
#define QDEC_AXIS_DIR_DOWN                               ((uint16_t)0x00)       //!< The direction of QDEC is down.

#define IS_QDEC_AXIS_DIR(QDEC_AXIS)     ((QDEC_AXIS == QDEC_AXIS_DIR_UP) || (QDEC_AXIS == QDEC_AXIS_DIR_DOWN))  //!< Check whether is the direction of QDEC.

/** End of group QDEC_Axis_Direction
  * @}
  */


/** End of group QDEC_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/


/** @defgroup QDEC_Exported_Functions QDEC Exported Functions
  * @{
  */
/**
 * rtl876x_qdec.h
 *
 * \brief   Initializes the Qdecoder peripheral according to the specified
 *          parameters in the QDEC_InitStruct.
 *
 * \param[in]  QDECx: Selected Qdecoder peripheral, which can be QDEC. \ref x3d_QDEC_Declaration
 * \param[in]  QDEC_InitStruct: Pointer to a QDEC_InitTypeDef structure that
 *             contains the configuration information for the specified Qdecoder peripheral.
 *
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_qdec_init(void)
 * {
 *     QDEC_DeInit(QDEC);
 *     RCC_PeriphClockCmd(APBPeriph_QDEC, APBPeriph_QDEC_CLOCK, ENABLE);
 *
 *     QDEC_InitTypeDef QDEC_InitStruct;
 *     QDEC_StructInit(&QDEC_InitStruct);
 *     QDEC_InitStruct.axisConfigY       = ENABLE;
 *     QDEC_InitStruct.debounceEnableY   = Debounce_Enable;
 *     QDEC_Init(QDEC, &QDEC_InitStruct);
 * }
 * \endcode
 */
extern void QDEC_Init(QDEC_TypeDef *QDECx, QDEC_InitTypeDef *QDEC_InitStruct);

/**
 * rtl876x_qdec.h
 *
 * \brief   Deinitializes the Qdecoder peripheral registers to their
 *          default reset values(turn off Qdecoder clock).
 *
 * \param[in] QDECx: Selected Qdecoder peripheral, which can be QDEC. \ref x3d_QDEC_Declaration
 *
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_qdec_init(void)
 * {
 *     QDEC_DeInit(QDEC);
 * }
 * \endcode
 */
extern void QDEC_DeInit(QDEC_TypeDef *QDECx);

/**
 * rtl876x_qdec.h
 *
 * \brief  Fills each QDEC_InitStruct member with its default value.
 *
 * \note   The default settings for the QDEC_InitStruct member are shown in the following table:
 *         | QDEC_InitStruct member | Default value             |
 *         |:----------------------:|:-------------------------:|
 *         | ScanClockKHZ           | 50                        |
 *         | DebonceClockKHZ        | 5                         |
 *         | axisConfigX            | DISABLE                   |
 *         | debounceTimeX          | 25                        |
 *         | counterScaleX          | \ref counterScaleDisable  |
 *         | debounceEnableX        | \ref Debounce_Enable      |
 *         | initPhaseX             | \ref phaseMode0           |
 *         | axisConfigY            | DISABLE                   |
 *         | debounceTimeY          | 25                        |
 *         | counterScaleY          | \ref counterScaleDisable  |
 *         | debounceEnableY        | \ref Debounce_Enable      |
 *         | initPhaseY             | \ref phaseMode0           |
 *         | axisConfigZ            | DISABLE                   |
 *         | debounceTimeZ          | 25                        |
 *         | counterScaleZ          | \ref counterScaleDisable  |
 *         | debounceEnableZ        | \ref Debounce_Enable      |
 *         | initPhaseZ             | \ref phaseMode0           |
 *
 * \param[in]  QDEC_InitStruct: Pointer to an QDEC_InitStruct structure which will be initialized.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_qdec_init(void)
 * {
 *     QDEC_DeInit(QDEC);
 *     RCC_PeriphClockCmd(APBPeriph_QDEC, APBPeriph_QDEC_CLOCK, ENABLE);
 *
 *     QDEC_InitTypeDef QDEC_InitStruct;
 *     QDEC_StructInit(&QDEC_InitStruct);
 *     QDEC_InitStruct.axisConfigY       = ENABLE;
 *     QDEC_InitStruct.debounceEnableY   = Debounce_Enable;
 *     QDEC_Init(QDEC, &QDEC_InitStruct);
 * }
 * \endcode
 */
extern void QDEC_StructInit(QDEC_InitTypeDef *QDEC_InitStruct);

/**
 * rtl876x_qdec.h
 *
 * \brief  Enable or disable the specified Qdecoder interrupt source.
 *
 * \param[in] QDECx: Selected Qdecoder peripheral, which can be QDEC. \ref x3d_QDEC_Declaration
 * \param[in] QDEC_IT: Specifies the QDECODER interrupts sources to be enabled or disabled.
 *            This parameter parameter can be one of the following values:
 *            -  QDEC_X_INT_NEW_DATA: The counter interrupt for X axis.
 *            -  QDEC_X_INT_ILLEAGE: The illegal interrupt for X axis.
 *            -  QDEC_Y_INT_NEW_DATA: The counter interrupt for Y axis.
 *            -  QDEC_Y_INT_ILLEAGE: The illegal interrupt for Y axis.
 *            -  QDEC_Z_INT_NEW_DATA: The counter interrupt for Z axis.
 *            -  QDEC_Z_INT_ILLEAGE: The illegal interrupt for Z axis.
 * \param[in] newState: New state of the specified QDECODER interrupt.
 *            This parameter can be one of the following values:
 *            - ENABLE: Enable the specified Qdecoder interrupt source.
 *            - DISABLE: Disable the specified Qdecoder interrupt source.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_qdec_init(void)
 * {
 *     QDEC_INTConfig(QDEC, QDEC_Y_INT_NEW_DATA, ENABLE);
 * }
 * \endcode
 */
extern void QDEC_INTConfig(QDEC_TypeDef *QDECx, uint32_t QDEC_IT, FunctionalState newState);

/**
 * rtl876x_qdec.h
 *
 * \brief  Check whether the specified Qdecoder flag is set.
 *
 * \param[in] QDECx: Selected Qdecoder peripheral, which can be QDEC. \ref x3d_QDEC_Declaration
 * \param[in] QDEC_FLAG: Specifies the Qdecoder flag to check.
 *            This parameter can be one of the following values:
 *            - QDEC_FLAG_NEW_CT_STATUS_X: Status of the counter interrupt for X axis.
 *            - QDEC_FLAG_NEW_CT_STATUS_Y: Status of the counter interrupt for Y axis.
 *            - QDEC_FLAG_NEW_CT_STATUS_Z: Status of the counter interrupt for Z axis.
 *            - QDEC_FLAG_ILLEGAL_STATUS_X: Status of the illegal interrupt for X axis.
 *            - QDEC_FLAG_ILLEGAL_STATUS_Y: Status of the illegal interrupt for Y axis.
 *            - QDEC_FLAG_ILLEGAL_STATUS_Z: Status of the illegal interrupt for Z axis.
 *            - QDEC_FLAG_OVERFLOW_X: The overflow flag for x-axis accumulation counter.
 *            - QDEC_FLAG_OVERFLOW_Y: The overflow flag for y-axis accumulation counter.
 *            - QDEC_FLAG_OVERFLOW_Z: The overflow flag for z-axis accumulation counter.
 *            - QDEC_FLAG_UNDERFLOW_X: The underflow flag for x-axis accumulation counter.
 *            - QDEC_FLAG_UNDERFLOW_Y: The underflow flag for y-axis accumulation counter.
 *            - QDEC_FLAG_UNDERFLOW_Z: The underflow flag for z-axis accumulation counter.
 *
 * \return The new state of QDEC_FLAG (SET or RESET).
 * \retval SET: The specified Qdecoder flag is set.
 * \retval RESET: The specified Qdecoder flag is unset.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void QDEC_Handler(void)
 * {
 *     if (QDEC_GetFlagState(QDEC, QDEC_FLAG_NEW_CT_STATUS_Y) == SET)
 *     {
 *         //add user code here.
 *     }
 * }
 * \endcode
 */
extern FlagStatus QDEC_GetFlagState(QDEC_TypeDef *QDECx, uint32_t QDEC_FLAG);

/**
 * rtl876x_qdec.h
 *
 * \brief  Enables or disables mask the specified Qdecoder axis interrupts.
 *
 * \param[in]  QDECx: Selected Qdecoder peripheral, which can be QDEC. \ref x3d_QDEC_Declaration
 * \param[in]  QDEC_AXIS: Specifies the Qdecoder axis interrupts to mask.
 *             This parameter can be one or logical OR of the following values:
 *             - QDEC_X_CT_INT_MASK: The x-axis counter interrupt mask.
 *             - QDEC_X_ILLEAGE_INT_MASK: The x-axis illegal interrupt mask.
 *             - QDEC_Y_CT_INT_MASK: The y-axis counter interrupt mask.
 *             - QDEC_Y_ILLEAGE_INT_MASK: The y-axis illegal interrupt mask.
 *             - QDEC_Z_CNT_INT_MASK: The z-axis counter interrupt mask.
 *             - QDEC_Z_ILLEAGE_INT_MASK: The z-axis illegal interrupt mask.
 * \param[in]  newState: New state of the specified Qdecoder interrupts mask.
 *             This parameter can be one of the following values:
 *             - ENABLE: Enable mask the specified Qdecoder axis interrupts.
 *             - DISABLE: Disable mask the specified Qdecoder axis interrupts.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void QDEC_Handler(void)
 * {
 *     if (QDEC_GetFlagState(QDEC, QDEC_FLAG_NEW_CT_STATUS_Y) == SET)
 *     {
 *         QDEC_INTMask(QDEC, QDEC_Y_CT_INT_MASK, ENABLE);
 *
 *         //add user code here
 *         QDEC_INTMask(QDEC, QDEC_Y_CT_INT_MASK, DISABLE);
 *     }
 * }
 * \endcode
 */
extern void QDEC_INTMask(QDEC_TypeDef *QDECx, uint32_t QDEC_AXIS, FunctionalState newState);

/**
 * rtl876x_qdec.h
 *
 * \brief  Enable or disable the selected Qdecoder axis(x/y/z).
 *
 * \param[in]  QDECx: Selected Qdecoder peripheral, which can be QDEC. \ref x3d_QDEC_Declaration
 * \param[in]  QDEC_AXIS: Specifies the Qdecoder axis.
 *             This parameter can be one of the following values:
 *             - QDEC_AXIS_X: The qdecoder X axis.
 *             - QDEC_AXIS_Y: The qdecoder Y axis.
 *             - QDEC_AXIS_Z: The qdecoder Z axis.
 * \param[in]  newState: New state of the selected Qdecoder axis.
 *             This parameter can be one of the following values:
 *             - ENABLE: Enable the selected Qdecoder axis(x/y/z).
 *             - DISABLE: Disable the selected Qdecoder axis(x/y/z).
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void qdec_demo(void)
 * {
 *     // add init code here.
 *     QDEC_Cmd(QDEC, QDEC_AXIS_X, ENABLE);
 * }
 * \endcode
 */
extern void QDEC_Cmd(QDEC_TypeDef *QDECx, uint32_t QDEC_AXIS, FunctionalState newState);

/**
 * rtl876x_qdec.h
 *
 * \brief   Clears Qdecoder pending flags.
 *
 * \param[in]  QDECx: Selected Qdecoder peripheral, which can be QDEC. \ref x3d_QDEC_Declaration
 * \param[in]  QDEC_FLAG: Specifies the Qdecoder flag to clear.
 *             This parameter parameter can be one of the following values:
 *             - QDEC_CLR_OVERFLOW_X: The overflow flag for x-axis accumulation counter.
 *             - QDEC_CLR_OVERFLOW_Y: The overflow flag for y-axis accumulation counter.
 *             - QDEC_CLR_OVERFLOW_Z: The overflow flag for z-axis accumulation counter.
 *             - QDEC_CLR_ILLEGAL_INT_X: The illegal interrupt for X axis.
 *             - QDEC_CLR_ILLEGAL_INT_Y: The illegal interrupt for Y axis.
 *             - QDEC_CLR_ILLEGAL_INT_Z: The illegal interrupt for Z axis.
 *             - QDEC_CLR_ILLEGAL_CT_X: The illegal counter for X axis.
 *             - QDEC_CLR_ILLEGAL_CT_Y: The illegal counter for Y axis.
 *             - QDEC_CLR_ILLEGAL_CT_Z: The illegal counter for Z axis.
 *             - QDEC_CLR_UNDERFLOW_X: The underflow flag for x-axis accumulation counter.
 *             - QDEC_CLR_UNDERFLOW_Y: The underflow flag for y-axis accumulation counter.
 *             - QDEC_CLR_UNDERFLOW_Z: The underflow flag for z-axis accumulation counter.
 *             - QDEC_CLR_NEW_CT_X: The counter interrupt for X axis.
 *             - QDEC_CLR_NEW_CT_Y: The counter interrupt for Y axis.
 *             - QDEC_CLR_NEW_CT_Z: The counter interrupt for Z axis.
 *
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void QDEC_Handler(void)
 * {
 *     if (QDEC_GetFlagState(QDEC, QDEC_FLAG_NEW_CT_STATUS_Y) == SET)
 *     {
 *         //clear QDEC interrupt flags
 *         QDEC_ClearFlags(QDEC, QDEC_CLR_NEW_CT_Y);
 *         //Unmask QDEC interrupt
 *         QDEC_INTMask(QDEC, QDEC_Y_CT_INT_MASK, DISABLE);
 *     }
 * }
 * \endcode
 */
__STATIC_ALWAYS_INLINE void QDEC_ClearFlags(QDEC_TypeDef *QDECx, uint32_t QDEC_FLAG)
{
    /* Check the parameters */
    assert_param(IS_QDEC_PERIPH(QDECx));
    assert_param(IS_QDEC_STATUS_FLAG(QDEC_FLAG));

    QDECx->INT_CLR |= QDEC_FLAG;
    QDECx->INT_CLR;

    return;
}

/**
 * rtl876x_qdec.h
 *
 * \brief  Get Qdecoder Axis(x/y/z) direction.
 *
 * \param[in]  QDECx: Selected Qdecoder peripheral, which can be QDEC. \ref x3d_QDEC_Declaration
 * \param[in]  QDEC_AXIS: Specifies the Qdecoder axis.
 *             This parameter parameter can be one of the following values:
 *             - QDEC_AXIS_X: The qdecoder X axis.
 *             - QDEC_AXIS_Y: The qdecoder Y axis.
 *             - QDEC_AXIS_Z: The qdecoder Z axis.
 *
 * \return The direction of the Qdecoder axis.
 * \retval QDEC_AXIS_DIR_UP: The axis is rolling up.
 * \retval QDEC_AXIS_DIR_DOWN: The axis is rolling down.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * int16_t   y_axis;
 * uint16_t  dir;
 *
 * void QDEC_Handler(void)
 * {
 *     if (QDEC_GetFlagState(QDEC, QDEC_FLAG_NEW_CT_STATUS_Y) == SET)
 *     {
 *         //Mask QDEC interrupt
 *         QDEC_INTMask(QDEC, QDEC_Y_CT_INT_MASK, ENABLE);
 *
 *         //Read direction
 *         dir = QDEC_GetAxisDirection(QDEC, QDEC_AXIS_Y);
 *     }
 * }
 * \endcode
 */
__STATIC_ALWAYS_INLINE uint16_t QDEC_GetAxisDirection(QDEC_TypeDef *QDECx, uint32_t QDEC_AXIS)
{
    /* Check the parameters */
    assert_param(IS_QDEC_PERIPH(QDECx));
    assert_param(IS_QDEC_AXIS_DIR(QDEC_AXIS));

    return ((*((volatile uint32_t *)(&QDECx->REG_SR_X) + QDEC_AXIS / 2) & (1 << 16)) == BIT(16));
}

/**
 * rtl876x_qdec.h
 *
 * \brief  Get Qdecoder Axis(x/y/z) count.
 *
 * \param[in]  QDECx: Selected Qdecoder peripheral, which can be QDEC. \ref x3d_QDEC_Declaration
 * \param[in]  QDEC_AXIS: Specifies the Qdecoder axis.
 *             This parameter parameter can be one of the following values:
 *             - QDEC_AXIS_X: The qdecoder X axis.
 *             - QDEC_AXIS_Y: The qdecoder Y axis.
 *             - QDEC_AXIS_Z: The qdecoder Z axis.
 *
 * \return The count of the Qdecoder axis.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * int16_t   y_axis;
 * uint16_t  dir;
 *
 * void QDEC_Handler(void)
 * {
 *     if (QDEC_GetFlagState(QDEC, QDEC_FLAG_NEW_CT_STATUS_Y) == SET)
 *     {
 *         //Mask QDEC interrupt
 *         QDEC_INTMask(QDEC, QDEC_Y_CT_INT_MASK, ENABLE);
 *
 *         //Read count
 *         y_axis = QDEC_GetAxisCount(QDEC, QDEC_AXIS_Y);
 *     }
 * }
 * \endcode
 */
__STATIC_ALWAYS_INLINE uint16_t QDEC_GetAxisCount(QDEC_TypeDef *QDECx, uint32_t QDEC_AXIS)
{
    /* Check the parameters */
    assert_param(IS_QDEC_PERIPH(QDECx));
    assert_param(IS_QDEC_AXIS_DIR(QDEC_AXIS));

    return ((uint16_t)(*((volatile uint32_t *)(&QDECx->REG_SR_X) + QDEC_AXIS / 2)));
}

/**
 * rtl876x_qdec.h
 *
 * \brief  Pause or resume Qdecoder Axis(x/y/z).
 *
 * \param[in]  QDECx: Selected Qdecoder peripheral, which can be QDEC. \ref x3d_QDEC_Declaration
 * \param[in]  QDEC_AXIS: Specifies the Qdecoder axis.
 *             This parameter parameter can be one of the following values:
 *             - QDEC_AXIS_X: The qdecoder X axis.
 *             - QDEC_AXIS_Y: The qdecoder Y axis.
 *             - QDEC_AXIS_Z: The qdecoder Z axis.
 * \param[in]  newState: New state of the specified Qdecoder Axis.
 *             This parameter parameter can be one of the following values:
 *             - ENABLE: Pause the selected Qdecoder Axis.
 *             - DISABLE: Resume the selected Qdecoder Axis.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void qdec_demo(void)
 * {
 *     QDEC_CounterPauseCmd(QDEC, QDEC_AXIS_X, ENABLE);
 * }
 * \endcode
 */
__STATIC_ALWAYS_INLINE void QDEC_CounterPauseCmd(QDEC_TypeDef *QDECx, uint32_t QDEC_AXIS,
                                                 FunctionalState newState)
{
    /* Check the parameters */
    assert_param(IS_QDEC_PERIPH(QDECx));
    assert_param(IS_QDEC_AXIS_DIR(QDEC_AXIS));

    if (newState == ENABLE)
    {
        *((volatile uint32_t *)(&QDECx->REG_CR_X) + QDEC_AXIS / 2) |= BIT3;
    }
    else
    {
        *((volatile uint32_t *)(&QDECx->REG_CR_X) + QDEC_AXIS / 2) &= ~BIT3;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_QDECODER_H_ */

/** @} */ /* End of group QDEC_Exported_Functions */
/** @} */ /* End of group 87x3d_QDEC */

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor *****END OF FILE****/



