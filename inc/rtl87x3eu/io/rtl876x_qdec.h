/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_qdec.h
* @brief     header file of qdec driver.
* @details
* @author    colin
* @date      2024-06-19
* @version   v1.0
* *********************************************************************************************************
*/


#ifndef RTL_QDEC_H
#define RTL_QDEC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup  IO          Peripheral Drivers
 * \defgroup    QDEC        QDEC
 *
 * \brief       Manage the QDEC peripheral functions.
 *
 * \ingroup     IO
 */

/*============================================================================*
 *                         Includes
 *============================================================================*/
#include <stdbool.h>
#include "rtl876x.h"
#include "rtl876x_qdec_def.h"

/*============================================================================*
 *                         Constants
 *============================================================================*/

/**
 * \defgroup    QDEC_Exported_Constants     QDEC Exported Constants
 *
 * \ingroup     QDEC
 */

/**
 * \brief       QDEC Axis Counter Scale
 *
 * \ingroup     QDEC_Exported_Constants
 */

typedef enum
{
    CounterScale_1_Phase = 0x00,
    CounterScale_2_Phase = 0x01,
} QDECAxisCntScale_TypeDef;

#define IS_QDEC_AXIS_CNT_SCALE_TYPE(TYPE) ((TYPE) <= 0x01)

/**
 * \brief       Qdec Init Phase
 *
 * \ingroup     QDEC_Exported_Constants
 */

typedef enum
{
    phaseMode0 = 0x00,//phase 00
    phaseMode1 = 0x01,//phase 01
    phaseMode2 = 0x02,//phase 10
    phaseMode3 = 0x03,//phase 11
} QDECInitPhase_TypeDef;

#define IS_QDEC_INIT_PHASE_TYPE(TYPE) ((TYPE) <= 0x03)

/**
 * \brief       QDEC Axis Direction
 *
 * \ingroup     QDEC_Exported_Constants
 */

typedef enum
{
    QDEC_AXIS_DIR_DOWN = 0x00,
    QDEC_AXIS_DIR_UP = 0x01,
} QDECAxisDir_TypeDef;

#define IS_QDEC_AXIS_DIR(QDEC_AXIS)     ((QDEC_AXIS == QDEC_AXIS_DIR_UP) || (QDEC_AXIS == QDEC_AXIS_DIR_DOWN))

/**
 * \brief       QDEC Interrupts Definition
 *
 * \ingroup     QDEC_Exported_Constants
 */

#define QDEC_X_INT_NEW_DATA           BIT0//get New data and state change
#define QDEC_X_INT_ILLEAGE            BIT1//illeage
#define QDEC_Y_INT_NEW_DATA           BIT2//get New data and state change
#define QDEC_Y_INT_ILLEAGE            BIT3//illeage
#define QDEC_Z_INT_NEW_DATA           BIT4//get New data and state change
#define QDEC_Z_INT_ILLEAGE            BIT5//illeage

#define IS_QDEC_INT_CONFIG(CONFIG) (((CONFIG) == QDEC_X_INT_NEW_DATA) || ((CONFIG) == QDEC_X_INT_ILLEAGE)\
                                    || ((CONFIG) == QDEC_Y_INT_NEW_DATA) || ((CONFIG) == QDEC_Y_INT_ILLEAGE)\
                                    || ((CONFIG) == QDEC_Z_INT_NEW_DATA) || ((CONFIG) == QDEC_Z_INT_ILLEAGE))

/**
 * \brief       QDEC Interrupts Mask
 *
 * \ingroup     QDEC_Exported_Constants
 */

#define QDEC_X_CT_INT_MASK            BIT0//get New data and state change
#define QDEC_X_ILLEAGE_INT_MASK       BIT4//illeage
#define QDEC_Y_CT_INT_MASK            BIT1//get New data and state change
#define QDEC_Y_ILLEAGE_INT_MASK       BIT5//illeage
#define QDEC_Z_CT_INT_MASK            BIT2//get New data and state change
#define QDEC_Z_ILLEAGE_INT_MASK       BIT6//illeage

#define IS_QDEC_INT_MASK_CONFIG(CONFIG) (((CONFIG) == QDEC_X_CT_INT_MASK) || ((CONFIG) == QDEC_X_ILLEAGE_INT_MASK)\
                                         || ((CONFIG) == QDEC_Y_CT_INT_MASK) || ((CONFIG) == QDEC_Y_ILLEAGE_INT_MASK)\
                                         || ((CONFIG) == QDEC_Z_CT_INT_MASK) || ((CONFIG) == QDEC_Z_ILLEAGE_INT_MASK))

/**
 * \brief       QDEC Clr Flag
 *
 * \ingroup     QDEC_Exported_Constants
 */

#define QDEC_CLR_ILLEGAL_CT_X       BIT20
#define QDEC_CLR_ILLEGAL_CT_Y       BIT21
#define QDEC_CLR_ILLEGAL_CT_Z       BIT22

#define QDEC_CLR_ACC_CT_X           BIT16
#define QDEC_CLR_ACC_CT_Y           BIT17
#define QDEC_CLR_ACC_CT_Z           BIT18

#define QDEC_CLR_ILLEGAL_INT_X      BIT12
#define QDEC_CLR_ILLEGAL_INT_Y      BIT13
#define QDEC_CLR_ILLEGAL_INT_Z      BIT14

#define QDEC_CLR_UNDERFLOW_X        BIT8
#define QDEC_CLR_UNDERFLOW_Y        BIT9
#define QDEC_CLR_UNDERFLOW_Z        BIT10

#define QDEC_CLR_OVERFLOW_X         BIT4
#define QDEC_CLR_OVERFLOW_Y         BIT5
#define QDEC_CLR_OVERFLOW_Z         BIT6

#define QDEC_CLR_NEW_CT_X           BIT0
#define QDEC_CLR_NEW_CT_Y           BIT1
#define QDEC_CLR_NEW_CT_Z           BIT2

#define IS_QDEC_INT_CLR_CONFIG(CONFIG) (((CONFIG) == QDEC_CLR_ACC_CT_X) || ((CONFIG) == QDEC_CLR_ACC_CT_Y)\
                                        || ((CONFIG) == QDEC_CLR_ACC_CT_Z) || ((CONFIG) == QDEC_CLR_ILLEGAL_INT_Y)\
                                        || ((CONFIG) == QDEC_CLR_ILLEGAL_INT_Z) || ((CONFIG) == QDEC_CLR_UNDERFLOW_X)\
                                        || ((CONFIG) == QDEC_CLR_UNDERFLOW_Y) || ((CONFIG) == QDEC_CLR_UNDERFLOW_Z)\
                                        || ((CONFIG) == QDEC_CLR_OVERFLOW_X) || ((CONFIG) == QDEC_CLR_OVERFLOW_Y)\
                                        || ((CONFIG) == QDEC_CLR_OVERFLOW_Z) || ((CONFIG) == QDEC_CLR_NEW_CT_X)\
                                        || ((CONFIG) == QDEC_CLR_NEW_CT_Y) || ((CONFIG) == QDEC_CLR_NEW_CT_Z))

/**
 * \brief       QDEC Flag
 *
 * \ingroup     QDEC_Exported_Constants
 */

#define QDEC_FLAG_NEW_CT_STATUS_X       BIT0
#define QDEC_FLAG_NEW_CT_STATUS_Y       BIT1
#define QDEC_FLAG_NEW_CT_STATUS_Z       BIT2
#define QDEC_FLAG_OVERFLOW_X            BIT3
#define QDEC_FLAG_OVERFLOW_Y            BIT4
#define QDEC_FLAG_OVERFLOW_Z            BIT5
#define QDEC_FLAG_UNDERFLOW_X           BIT6
#define QDEC_FLAG_UNDERFLOW_Y           BIT7
#define QDEC_FLAG_UNDERFLOW_Z           BIT8
#define QDEC_FLAG_ILLEGAL_STATUS_X      BIT12
#define QDEC_FLAG_ILLEGAL_STATUS_Y      BIT13
#define QDEC_FLAG_ILLEGAL_STATUS_Z      BIT14

#define IS_QDEC_INT_STATUS(INT) (((INT) == QDEC_FLAG_ILLEGAL_STATUS_X) || ((INT) == QDEC_FLAG_ILLEGAL_STATUS_Y)\
                                 || ((INT) == QDEC_FLAG_ILLEGAL_STATUS_Z) || ((INT) == QDEC_FLAG_NEW_CT_STATUS_X)\
                                 || ((INT) == QDEC_FLAG_NEW_CT_STATUS_Y) || ((INT) == QDEC_FLAG_NEW_CT_STATUS_Z)\
                                 || ((INT) == QDEC_FLAG_OVERFLOW_X) || ((INT) == QDEC_FLAG_OVERFLOW_Y)\
                                 || ((INT) == QDEC_FLAG_OVERFLOW_Z) || ((INT) == QDEC_FLAG_UNDERFLOW_X)\
                                 || ((INT) == QDEC_FLAG_UNDERFLOW_Y) || ((INT) == QDEC_FLAG_UNDERFLOW_Z))

/**
 * \brief       QDEC Axis
 *
 * \ingroup     QDEC_Exported_Constants
 */

#define QDEC_AXIS_X                 BIT0
#define QDEC_AXIS_Y                 BIT2
#define QDEC_AXIS_Z                 BIT3

/**
 * \brief       QDEC Immediate Number definition
 *
 * \ingroup     QDEC_Exported_Constants
 */

#define QDEC_0X00_CNT_PAUSE         BIT3
#define QDEC_0X08_CNT_DIR           BIT16
#define QDEC_0X04_AXIS_EN           BIT31

/** \} */ /* End of group QDEC_Exported_Constants */

/** @addtogroup QDEC QDEC
  * @brief Qdecoder driver module
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/


/** @defgroup QDEC_Exported_Types QDEC Exported Types
  * @{
  */

/**
 * \brief       Qdecoder init structure definition.
 *
 * \ingroup     QDEC_Exported_Types
 */

typedef struct
{
#if (QDEC_CLOCK_INIT_USE_KHZ == 1)
    uint16_t ScanClockKHZ;                 /*!< Specifies the scan clock(KHz).
                                                  Scan clock = source clock / (DIV+1), source clock is 20000KHz, DIV range is 0x0 to 0xFFF*/

    uint16_t DebonceClockKHZ;             /*!< Specifies the debounce clock(KHz).
                                                  Debounce clock = scan clock / (DEB_DIV+1), DEB_DIV range is 0x0 to 0xF  */
#else
    uint16_t scanClockDiv;                 /*!< Specifies DIV for Scan clock. */
    uint16_t debounceClockDiv;             /*!< Specifies DEB_DIV for debounce clock. */
#endif
    uint8_t axisConfigX;                   /*!< Specifies the axis X function.
                                                 This parameter can be a value of ENABLE or DISABLE. */
    uint8_t axisConfigY;                   /*!< Specifies the axis Y function.
                                                This parameter can be a value of ENABLE or DISABLE. */
    uint8_t axisConfigZ;                   /*!< Specifies the axis Z function.
                                                This parameter can be a value of ENABLE or DISABLE. */
    FunctionalState manualLoadInitPhase;   /*!< Specifies manual-load Initphase function.
                                                This parameter can be a value of ENABLE or DISABLE. */
    QDECAxisCntScale_TypeDef counterScaleX;/*!< Specifies the axis X conter scale. */
    FunctionalState debounceEnableX;       /*!< Specifies the axis X debounce.
                                                This parameter can be a value of ENABLE or DISABLE. */
    uint16_t debounceTimeX;                /*!< Specifies the axis X debounce time. */
    QDECInitPhase_TypeDef initPhaseX;      /*!< Specifies the axis X function. */
    QDECAxisCntScale_TypeDef counterScaleY;/*!< Specifies the axis Y conter scale. */
    FunctionalState debounceEnableY;       /*!< Specifies the axis Y debounce.
                                                This parameter can be a value of ENABLE or DISABLE. */
    uint16_t debounceTimeY;                /*!< Specifies the axis Y debounce time. */
    QDECInitPhase_TypeDef initPhaseY;      /*!< Specifies the axis Y function. */
    QDECAxisCntScale_TypeDef counterScaleZ;/*!< Specifies the axis Z conter scale. */
    FunctionalState debounceEnableZ;       /*!< Specifies the axis Z debounce.
                                                This parameter can be a value of ENABLE or DISABLE. */
    uint16_t debounceTimeZ;                /*!< Specifies the axis Z debounce time. */
    QDECInitPhase_TypeDef initPhaseZ;      /*!< Specifies the axis Z function. */
} QDEC_InitTypeDef;

/** End of group QDEC_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * \defgroup    QDEC_Exported_Functions QDEC Exported Functions
 * \{
 * \ingroup     QDEC
 */

/**
 * \brief   Deinitializes the Qdecoder peripheral registers to their default reset values(turn off Qdecoder clock).
 *
 * \param[in] QDECx: Selected Qdecoder peripheral.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_qdec_init(void)
 * {
 *     QDEC_DeInit();
 * }
 * \endcode
 */
void QDEC_DeInit(QDEC_TypeDef *QDECx);

/**
 * \brief   Initializes the Qdecoder peripheral according to the specified
 *          parameters in the QDEC_InitStruct
 *
 * \param[in]  QDECx: Selected Qdecoder peripheral.
 * \param[in]  QDEC_InitStruct: Pointer to a QDEC_InitStruct structure that
 *             contains the configuration information for the specified Qdecoder peripheral
 *
 * \return None.
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
 *
 *     QDEC_Cmd(QDEC, QDEC_AXIS_Y, ENABLE);
 * }
 * \endcode
 */
void QDEC_Init(QDEC_TypeDef *QDECx, QDEC_InitTypeDef *QDEC_InitStruct);

/**
 * \brief  Fills each QDEC_InitStruct member with its default value.
 *
 * \param[in]  QDEC_InitStruct: Pointer to an QDEC_InitStruct structure which will be initialized.
 *
 * \return None.
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
 *
 *     QDEC_Cmd(QDEC, QDEC_AXIS_Y, ENABLE);
 * }
 * \endcode
 */
void QDEC_StructInit(QDEC_InitTypeDef *QDEC_InitStruct);

/**
 * \brief  Enables or disables the specified Qdecoder interrupt source.
 *
 * \param[in] QDECx: Selected Qdecoder peripheral.
 * \param[in] QDEC_IT: Specifies the QDECODER interrupts sources to be enabled or disabled.
 *            This parameter parameter can be one of the following values:
 *            \arg  QDEC_X_INT_NEW_DATA: The counter interrupt for X axis.
 *            \arg  QDEC_X_INT_ILLEAGE: The illegal interrupt for X axis.
 *            \arg  QDEC_Y_INT_NEW_DATA: The counter interrupt for Y axis.
 *            \arg  QDEC_Y_INT_ILLEAGE: The illegal interrupt for Y axis.
 *            \arg  QDEC_Z_INT_NEW_DATA: The counter interrupt for Z axis.
 *            \arg  QDEC_Z_INT_ILLEAGE: The illegal interrupt for Z axis.
 * \param[in] NewState: New state of the specified QDECODER interrupt.
 *            This parameter parameter can be: ENABLE or DISABLE.
 *
 * \return None.
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
void QDEC_INTConfig(QDEC_TypeDef *QDECx, uint32_t QDEC_IT, FunctionalState NewState);

/**
 * \brief  Check whether the specified Qdecoder flag is set.
 *
 * \param[in] QDECx: Selected Qdecoder peripheral.
 * \param[in] QDEC_FLAG: Specifies the flag to check.
 *            This parameter can be one of the following values:
 *            \arg QDEC_FLAG_NEW_CT_STATUS_X: Status of the counter interrupt for X axis.
 *            \arg QDEC_FLAG_NEW_CT_STATUS_Y: Status of the counter interrupt for Y axis.
 *            \arg QDEC_FLAG_NEW_CT_STATUS_Z: Status of the counter interrupt for Z axis.
 *            \arg QDEC_FLAG_ILLEGAL_STATUS_X: Status of the illegal interrupt for X axis.
 *            \arg QDEC_FLAG_ILLEGAL_STATUS_Y: Status of the illegal interrupt for Y axis.
 *            \arg QDEC_FLAG_ILLEGAL_STATUS_Z: Status of the illegal interrupt for Z axis.
 *            \arg QDEC_FLAG_OVERFLOW_X: The overflow flag for x-axis accumulation counter.
 *            \arg QDEC_FLAG_OVERFLOW_Y: The overflow flag for y-axis accumulation counter.
 *            \arg QDEC_FLAG_OVERFLOW_Z: The overflow flag for z-axis accumulation counter.
 *            \arg QDEC_FLAG_UNDERFLOW_X: The underflow flag for x-axis accumulation counter.
 *            \arg QDEC_FLAG_UNDERFLOW_Y: The underflow flag for y-axis accumulation counter.
 *            \arg QDEC_FLAG_UNDERFLOW_Z: The underflow flag for z-axis accumulation counter.
 *
 * \return The new state of QDEC_FLAG (SET or RESET).
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void qdec_demo(void)
 * {
 *     FlagStatus flag_status = QDEC_GetFlagState(QDEC, QDEC_Y_INT_NEW_DATA);
 * }
 * \endcode
 */
FlagStatus QDEC_GetFlagState(QDEC_TypeDef *QDECx, uint32_t QDEC_FLAG);

/**
 * \brief  Enables or disables mask the specified Qdecoder axis interrupts.
 *
 * \param[in] QDECx: Selected Qdecoder peripheral.
 * \param[in] QDEC_AXIS: Specifies the Qdecoder axis.
 *            This parameter can be one or logical OR of the following values:
 *            \arg QDEC_X_CT_INT_MASK: The x-axis counter interrupt mask.
 *            \arg QDEC_X_ILLEAGE_INT_MASK: The x-axis illegal interrupt mask.
 *            \arg QDEC_Y_CT_INT_MASK: The y-axis counter interrupt mask.
 *            \arg QDEC_Y_ILLEAGE_INT_MASK: The y-axis illegal interrupt mask.
 *            \arg QDEC_Z_CNT_INT_MASK: The z-axis counter interrupt mask.
 *            \arg QDEC_Z_ILLEAGE_INT_MASK: The z-axis illegal interrupt mask.
 * \param[in] NewState: New state of the specified Qdecoder interrupts mask.
 *            This parameter can be: ENABLE or DISABLE.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void qdec_demo(void)
 * {
 *
 *     QDEC_INTMask(QDEC, QDEC_X_CT_INT_MASK, ENABLE);
 *
 * }
 * \endcode
 */
void QDEC_INTMask(QDEC_TypeDef *QDECx, uint32_t QDEC_AXIS, FunctionalState NewState);

/**
 * \brief  Enable or disable the selected Qdecoder axis(x/y/z).
 *
 * \param[in] QDECx: Selected Qdecoder peripheral.
 * \param[in] QDEC_AXIS: Specifies the Qdecoder axis.
 *            This parameter can be one of the following values:
 *            \arg QDEC_AXIS_X: The qdecoder X axis.
 *            \arg QDEC_AXIS_Y: The qdecoder Y axis.
 *            \arg QDEC_AXIS_Z: The qdecoder Z axis.
 * \param[in] NewState: New state of the selected Qdecoder axis.
 *            This parameter can be : ENABLE or DISABLE.
 *
 * \return The count of the axis.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void qdec_demo(void)
 * {
 *     QDEC_Cmd(QDEC, QDEC_AXIS_X, ENABLE);
 * }
 * \endcode
 */
void QDEC_Cmd(QDEC_TypeDef *QDECx, uint32_t QDEC_AXIS, FunctionalState NewState);

/**
 * \brief   Clear Qdecoder interrupt pending bit.
 *
 * \param[in] QDECx: Selected Qdecoder peripheral.
 * \param[in] QDEC_FLAG: Specifies the flag to clear.
 *            This parameter parameter can be one of the following values:
 *            \arg QDEC_CLR_OVERFLOW_X: The overflow flag for x-axis accumulation counter.
 *            \arg QDEC_CLR_OVERFLOW_Y: The overflow flag for y-axis accumulation counter.
 *            \arg QDEC_CLR_OVERFLOW_Z: The overflow flag for z-axis accumulation counter.
 *            \arg QDEC_CLR_ILLEGAL_INT_X: The illegal interrupt for X axis.
 *            \arg QDEC_CLR_ILLEGAL_INT_Y: The illegal interrupt for Y axis.
 *            \arg QDEC_CLR_ILLEGAL_INT_Z: The illegal interrupt for Z axis.
 *            \arg QDEC_CLR_UNDERFLOW_X: The underflow flag for x-axis accumulation counter.
 *            \arg QDEC_CLR_UNDERFLOW_Y: The underflow flag for y-axis accumulation counter.
 *            \arg QDEC_CLR_UNDERFLOW_Z: The underflow flag for z-axis accumulation counter.
 *            \arg QDEC_CLR_NEW_CT_X: The counter interrupt for X axis.
 *            \arg QDEC_CLR_NEW_CT_Y: The counter interrupt for Y axis.
 *            \arg QDEC_CLR_NEW_CT_Z: The counter interrupt for Z axis.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void qdec_demo(void)
 * {
 *     QDEC_ClearINTPendingBit(QDEC, QDEC_CLR_OVERFLOW_X);
 * }
 * \endcode
 */
void QDEC_ClearINTPendingBit(QDEC_TypeDef *QDECx, uint32_t QDEC_CLR_INT);

/**
 * \brief  Get Qdecoder Axis(x/y/z) direction.
 *
 * \param[in] QDECx: Selected Qdecoder peripheral.
 * \param[in] QDEC_AXIS: Specifies the Qdecoder axis.
 *            This parameter parameter can be one of the following values:
 *            \arg QDEC_AXIS_X: The qdecoder X axis.
 *            \arg QDEC_AXIS_Y: The qdecoder Y axis.
 *            \arg QDEC_AXIS_Z: The qdecoder Z axis.
 *
 * \return The direction of the axis.
 * This parameter parameter can be one of the following values:
 * \retval QDEC_AXIS_DIR_UP: The axis is rolling up.
 * \retval QDEC_AXIS_DIR_DOWN: The axis is rolling down.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void qdec_demo(void)
 * {
 *     uint16_t dir = QDEC_GetAxisDirection(QDEC, QDEC_AXIS_X);
 * }
 * \endcode
 */
uint16_t QDEC_GetAxisDirection(QDEC_TypeDef *QDECx, uint32_t QDEC_AXIS);

/**
 * \brief  Get Qdecoder Axis(x/y/z) count.
 *
 * \param[in] QDECx: Selected Qdecoder peripheral.
 * \param[in] QDEC_AXIS: Specifies the Qdecoder axis.
 *            This parameter parameter can be one of the following values:
 *            \arg QDEC_AXIS_X: The qdecoder X axis.
 *            \arg QDEC_AXIS_Y: The qdecoder Y axis.
 *            \arg QDEC_AXIS_Z: The qdecoder Z axis.
 *
 * \return The count of the axis.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void qdec_demo(void)
 * {
 *     uint16_t counter = QDEC_GetAxisCount(QDEC, QDEC_AXIS_X);
 * }
 * \endcode
 */
uint16_t QDEC_GetAxisCount(QDEC_TypeDef *QDECx, uint32_t QDEC_AXIS);

/**
 * \brief  Pause or resume Qdecoder Axis(x/y/z).
 *
 * \param[in] QDECx: Selected Qdecoder peripheral.
 * \param[in] QDEC_AXIS: Specifies the Qdecoder axis.
 *            This parameter parameter can be one of the following values:
 *            \arg QDEC_AXIS_X: The qdecoder X axis.
 *            \arg QDEC_AXIS_Y: The qdecoder Y axis.
 *            \arg QDEC_AXIS_Z: The qdecoder Z axis.
 * \param[in] NewState: New state of the specified Qdecoder Axis.
 *            This parameter parameter can be one of the following values:
 *            \arg  ENABLE: Pause.
 *            \arg  DISABLE: Resume.
 *
 * \return None.
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
void QDEC_CounterPauseCmd(QDEC_TypeDef *QDECx, uint32_t QDEC_AXIS,
                          FunctionalState NewState);

/** @} */ /* End of group QDEC_Exported_Functions */

#ifdef __cplusplus
}
#endif

#endif /* RTL_QDEC_H */

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor *****END OF FILE****/



