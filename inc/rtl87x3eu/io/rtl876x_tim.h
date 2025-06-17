/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl876x_tim.h
* \brief    The header file of the peripheral TIMER driver.
* \details  This file provides all TIMER firmware functions.
* \author
* \date
* \version  v1.0
* *********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef RTL876X_TIM_H
#define RTL876X_TIM_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "rtl876x_tim_def.h"

/** \defgroup TIM         TIM
  * \brief
  * \{
  */
/*============================================================================*
 *                         Constants
 *============================================================================*/
/** \defgroup TIM_Exported_Constants TIM Exported Constants
  * \brief
  * \{
  */

/**
 * \defgroup    TIM_CASE TIM CASE
 * \{
 * \ingroup     TIM_Exported_Constants
 */
typedef enum
{
    TIM_CASE1 = 1,
    TIM_CASE7 = 2,
    TIM_CASE6 = 3,
    TIM_CASE2 = 4,
    TIM_CASE3 = 5,
    TIM_CASE4 = 6,
    TIM_CASE5 = 7,
} TIMCASE_TypeDef;

/** End of TIM_CASE
  * \}
  */

/**
 * \defgroup    TIM_Mode TIM Mode
 * \{
 * \ingroup     TIM_Exported_Constants
 */
typedef enum
{
    TIM_Mode_FreeRun = 0x0,
    TIM_Mode_UserDefine_Auto = 0x1,
    TIM_Mode_UserDefine = 0x2,
} TIMMode_TypeDef;

#define IS_TIM_MODE(mode) (((mode) == TIM_Mode_FreeRun) || \
                           ((mode) == TIM_Mode_UserDefine_Auto) || \
                           ((mode) == TIM_Mode_UserDefine))

/** End of TIM_Mode
  * \}
  */

#if (TIM_SUPPORT_PWM_FUNCTION == 1)
/**
 * \defgroup    PWM_Enable PWM Enable
 * \{
 * \ingroup     TIM_Exported_Constants
 */
#define IS_PWM_En(mode) (((mode) == ENABLE) || \
                         ((mode) == DISABLE))

/** End of PWM_Enable
  * \}
  */

/**
 * \defgroup    PWM_Polarity PWM Polarity
 * \{
 * \ingroup     TIM_Exported_Constants
 */
typedef enum
{
    PWM_START_WITH_LOW = 0x00,
    PWM_START_WITH_HIGH = 0x01,
} PWMPolarity_TypeDef;

#define IS_PWM_POLARITY(Pol) (((Pol) == PWM_START_WITH_HIGH) || \
                              ((Pol) == PWM_START_WITH_LOW))

/** End of PWM_Polarity
  * \}
  */

/**
 * \defgroup    PWM_Output_Mode PWM Output Mode
 * \{
 * \ingroup     TIM_Exported_Constants
 */
typedef enum
{
    PWM_PUSH_PULL = 0x0,
    PWM_OPEN_DRAIN = 0x1,
}
PWMOutputMode_TypeDef;

/** End of PWM_Output_Mode
  * \}
  */

#if (TIM_SUPPORT_PWM_DEADZONE == 1)
/**
 * \defgroup    PWM_DeadZone_Enable PWM DeadZone Enable
 * \{
 * \ingroup     TIM_Exported_Constants
 */
#define IS_PWM_DeadZone_En(mode) (((mode) == ENABLE) || \
                                  ((mode) == DISABLE))

/** End of PWM_DeadZone_Enable
  * \}
  */

/**
 * \defgroup    PWM_DeadZone_Clock_Source PWM DeadZone Clock Source
 * \{
 * \ingroup     TIM_Exported_Constants
 */
typedef enum
{
    PWM_DZ_CLOCK_TIMER     = 0x0,
    PWM_DZ_CLOCK_32K       = 0x1,
} PWMDZClockSrc_TypeDef;

#define IS_PWM_DeadZone_SOURCE(PERIPH) (((PERIPH) == PWM_DZ_CLOCK_TIMER) || \
                                        ((PERIPH) == PWM_DZ_CLOCK_32K))

/** End of PWM_DeadZone_Clock_Source
  * \}
  */

/**
 * \defgroup    PWM_DeadZone_Stop_State PWM DeadZone Stop State
 * \{
 * \ingroup     TIM_Exported_Constants
 */
typedef enum
{
    PWM_DZ_STOP_AT_LOW = 0x0,
    PWM_DZ_STOP_AT_HIGH = 0x1,
} PWMDZStopState_TypeDef;

#define IS_PWM_DeadZone_STOP_STATE(STATE) (((STATE) == PWM_DZ_STOP_AT_LOW) || \
                                           ((STATE) == PWM_DZ_STOP_AT_HIGH))

/** End of PWM_DeadZone_Stop_State
  * \}
  */

/**
 * \defgroup    PWM_Reference PWM Reference
 * \{*
 * \ingroup     TIM_Exported_Constants
 */
typedef enum
{
    PWM_DZ_REF_PWMPN = 0x0,
    PWM_DZ_REF_PWMNN = 0x1,
    PWM_DZ_REF_PWMPP = 0x2,
    PWM_DZ_REF_PWMNP = 0x3,
} PWMDZRef_TypeDef;

#define IS_PWM_DeadZone_Refenrence(STATE) (((STATE) == PWM_DZ_REF_PWMPN) || \
                                           ((STATE) == PWM_DZ_REF_PWMNN) || \
                                           ((STATE) == PWM_DZ_REF_PWMPP) || \
                                           ((STATE) == PWM_DZ_REF_PWMNP))
#endif
#endif

#if (TIM_SUPPORT_LATCH_CNT_0 == 1)
/**
 * \defgroup    TIM_Latch_Trigger_Mode TIM Latch Trigger Mode
 * \{
 * \ingroup     TIM_Exported_Constants
 */
typedef enum
{
    TIM_LATCH_TRIGGER_RISING_EDGE = 0x00,
    TIM_LATCH_TRIGGER_FALLING_EDGE = 0x01,
    TIM_LATCH_TRIGGER_BOTH_EDGE = 0x02,
} TIMLatchTriggleMode_TypeDef;

#define IS_TIM_LATCH_TRIG_Mode(mode) (((mode) == TIM_LATCH_TRIGGER_BOTH_EDGE) || \
                                      ((mode) == TIM_LATCH_TRIGGER_FALLING_EDGE) || \
                                      ((mode) == TIM_LATCH_TRIGGER_RISING_EDGE))

/** End of TIM_Latch_Trigger_Mode
  * \}
  */
#endif

#if (TIM_SUPPORT_DMA_FUNCTION == 1)
/**
 * \defgroup    TIM_DMA_TARGET TIM DMA TARGET
 * \{
 * \ingroup     TIM_Exported_Constants
 */
typedef enum
{
    TIM_DMA_CCR_FIFO = 0x00,
    TIM_DMA_LC_FIFO = 0x01,
} TIMDmaTarget_TypeDef;

#define IS_TIM_DMA_TARGET(mode) (((mode) == TIM_DMA_CCR_FIFO) || \
                                 ((mode) == TIM_DMA_LC_FIFO))

/** End of TIM_DMA_TARGET
  * \}
  */
#endif

/**
 * \defgroup    TIM_Interrupts TIM Interrupts
 * \{
 * \ingroup     TIM_Exported_Constants
 */
#define TIM_INT_TIMEOUT                        (1 << 0)
#define TIM_INT_LATCH_CNT_FIFO_FULL            (1 << 1)
#define TIM_INT_LATCH_CNT_FIFO_THD             (1 << 2)
#define TIM_INT_PAUSE                          (1 << 3)

#define IS_TIM_INT(INT) (((INT) == TIM_INT_TIMEOUT) || \
                         ((INT) == TIM_INT_LATCH_CNT_FIFO_FULL)  || \
                         ((INT) == TIM_INT_LATCH_CNT_FIFO_THD)   || \
                         ((INT) == TIM_INT_PAUSE))

/** End of TIM_Interrupts
  * \}
  */


/**
 * \defgroup    TIM_Flag TIM Flag
 * \{
 * \ingroup     TIM_Exported_Constants
 */
#define TIM_FLAG_CCR_FIFO_EMPTY                (0)
#define TIM_FLAG_CCR_FIFO_FULL                 (1)
#define TIM_FLAG_LATCH_CNT_FIFO_EMPTY          (2)
#define TIM_FLAG_LATCH_CNT_FIFO_FULL           (3)
#define TIM_FLAG_LATCH_CNT_FIFO_THD            (4)

#define IS_TIM_FLAG(flag) (((flag) == TIM_FLAG_CCR_FIFO_FULL) || \
                           ((flag) == TIM_FLAG_CCR_FIFO_EMPTY) || \
                           ((flag) == TIM_FLAG_LATCH_CNT_FIFO_EMPTY) || \
                           ((flag) == TIM_FLAG_LATCH_CNT_FIFO_FULL) || \
                           ((flag) == TIM_FLAG_LATCH_CNT_FIFO_THD))

/** End of TIM_Flag
  * \}
  */

/**
 * \defgroup    TIM_FIFO_Clear_Flag TIM FIFO Clear Flag
 * \{
 * \ingroup     TIM_Exported_Constants
 */
#define TIM_FIFO_CLR_CCR                      (0)
#define TIM_FIFO_CLR_CNT                      (1)

/** End of TIM_FIFO_Clear_Flag
  * \}
  */

#if (TIM_SUPPORT_RAP_FUNCTION == 1)

/**
 * \defgroup    TIM_Task TIM Task
 * \{
 * \ingroup     TIM_Exported_Constants
 */
typedef enum
{
    TIM_TASK_START = 0,
    TIM_TASK_PAUSE = 1,
    TIM_TASK_STOP  = 2,
} TIMTask_TypeDef;

/** End of TIM_Task
  * \}
  */

/**
 * \defgroup    TIM_Shortcut_Task TIM Shortcut Task
 * \{
 * \ingroup     TIM_Exported_Constants
 */
typedef enum
{
    TIM_SHORTCUT_TASK_PAUSE = 1,
    TIM_SHORTCUT_TASK_STOP  = 2,
} TIMShortcutTask_TypeDef;

/** End of TIM_Shortcut_Task
  * \}
  */

/**
 * \defgroup    TIM_Shortcut_Event TIM Shortcut Event
 * \{
 * \ingroup     TIM_Exported_Constants
 */
typedef enum
{
    TIM_SHORTCUT_EVENT_TIMEOUT = 0,
} TIMShortcutEvent_TypeDef;

/** End of TIM_Shortcut_Event
  * \}
  */
#endif

/** End of TIM_Exported_Constants
  * \}
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** \defgroup TIM_Exported_Types TIM Exported Types
  * \brief
  * \{
  */

#if (TIM_SUPPORT_PWM_DEADZONE == 1)
/**
 * \brief       TIM init structure definition.
 *
 * \ingroup     TIM_Exported_Types
 */
typedef struct
{
    PWMDZClockSrc_TypeDef PWM_DZClockSrc;     /*!< Specifies the pwm deazone clock source. */

    TIMClockDiv_TypeDef PWM_DZClockDiv;       /*!< Specifies the clock source div. */

    uint32_t PWM_DZSize;                      /*!< Size of deadzone time.
                                                   DeadzoneTime = deadzonesize / (32000 or 32768).
                                                   This parameter must range from 1 to 0xff. */

    FunctionalState PWM_DZEn;                 /*!< PWM Deadzone enable.
                                                   This parameter can be a value of DISABLE or ENABLE. */

    PWMDZStopState_TypeDef PWM_DZStopStateP;  /*!< Specifies the PWM P stop state. */

    PWMDZStopState_TypeDef PWM_DZStopStateN;  /*!< Specifies the PWM N stop state. */

    FunctionalState PWM_DZInvertP;            /*!< Specifies ENHTIM invertion of pwm_p.
                                                   This parameter can be a value of PWM Reference */

    FunctionalState PWM_DZInvertN;            /*!< Specifies ENHTIM invertion of pwm_n.
                                                   This parameter can be a value of PWM Reference */
} PWM_DeadZoneTypeDef;
#endif

#if (TIM_SUPPORT_LATCH_CNT_0 == 1)
typedef struct
{
    FunctionalState TIM_LatchCountEn[3];    /*!< Specifies enbale TIM Latch_cnt.
                                                 This parameter can be a value of DISABLE or ENABLE. */

    TIMLatchTriggleMode_TypeDef
    TIM_LatchCountTrigger[3];               /*!< Specifies TIM latch count trigger mode. */

    uint16_t TIM_LatchCountThd;             /*!< Specifies TIM latched counter fifo threshold.
                                                 This parameter can be value of 0~8. */

    uint16_t TIM_LatchTriggerPad;           /*!< Specifies the PWM mode.
                                                 This parameter can be a value of P0_0 to P19_0. */

    uint16_t TIM_LatchTriggerDebSize;       /*!< Size of GPIO trigger debounce time.
                                                 This parameter must range from 0 to 65535. */

    FunctionalState TIM_LatchTriggerDebEn;  /*!< GPIO trigger debounce enable.
                                                 This parameter can be a value of DISABLE or ENABLE. */

    TIMClockDiv_TypeDef
    TIM_LatchTrigDebClkDiv;                 /*!< Specifies GPIO trigger debounce clock source div. */
} TIM_LatchCountTypeDef;
#endif

typedef struct
{
    TIMClockSrc_TypeDef TIM_ClockSrc;       /*!< Specifies the clock source. */

    TIMClockDiv_TypeDef TIM_ClockDiv;       /*!< Specifies the clock source div.
                                                 This parameter can be a value of TIM_Clock_Divider. */

    TIMMode_TypeDef TIM_Mode;               /*!< Specifies the operating mode. */

    uint32_t TIM_Period;                    /*!< Specifies the period value to be loaded into the active
                                                 Auto-Reload Register at the next update event.
                                                 This parameter must range from 0x0 to 0xFFFFFFFF.
                                                 period = PWM_HighCount + PWM_LowCount */

    FunctionalState TIM_OneShotEn;          /*!< Specifies the one shot mode. */

#if (TIM_SUPPORT_PWM_FUNCTION == 1)
    FunctionalState PWM_En;                 /*!< Specifies the PWM mode.
                                                 This parameter can be a value of DISABLE or ENABLE. */

    uint32_t PWM_HighCount;                 /*!< Specifies the PWM High Count.
                                                 This parameter must range from 0x0 to 0xFFFFFFFF. */

    PWMPolarity_TypeDef PWM_Polarity;       /*!< Specifies enhtim toggle output polarity
                                                 for user-define PWM mode.*/

    PWMOutputMode_TypeDef PWM_OutputMode;   /*!< Specifies enable PWM open drain. */
#endif

#if (TIM_SUPPORT_PWM_DEADZONE == 1)
    PWM_DeadZoneTypeDef PWM_DZ;             /*!< Specifies PWM Dead Zone. */
#endif

#if (TIM_SUPPORT_LATCH_CNT_0 == 1)
    TIM_LatchCountTypeDef TIM_LC;           /*!< Specifies TIM Latch Count. */
#endif

#if (TIM_SUPPORT_DMA_FUNCTION == 1)
    FunctionalState TIM_DmaEn;              /*!< Specifies enable TIM DMA.
                                                 This parameter can be a value of DISABLE or ENABLE. */

    TIMDmaTarget_TypeDef TIM_DmaTarget;     /*!< Specifies TIM DMA target. */
#endif

#if (TIM_SUPPORT_AUTO_CLOCK == 1)
    FunctionalState TIM_DynConfigEn;        /*!< Specifies the functon to dynamic adjustment of
                                                 CCR & MAX_CNT.  When this feature is enabled,
                                                 dynamic adjustment is supported. When this feature
                                                 is disabled, dynamic adjustment is not supported,
                                                 resulting in lower power consumption.*/
#endif
} TIM_TimeBaseInitTypeDef;

/** End of TIM_Exported_Types
  * \}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** \defgroup TIM_Exported_Functions TIM Exported Functions
  * \brief
  * \{
  */

/**
  * \brief  Enable or Disable TIM peripheral clock.
  * \param[in] TIMx: Select the TIM peripheral. \ref TIMER_Declaration
  * \param[in]  is_enable:
  *  \ref  true:  enable hw timer hw clock
  *  \ref  false: disable hw timer hw clock
  * \return None
  */
void TIM_HardwareClock(TIM_TypeDef *TIMx, bool is_enable);

/**
 * \brief   Initialize the TIMx time base unit peripheral according to
 *          the specified parameters in TIM_TimeBaseInitStruct.
 *
 * \param[in] TIMx: Select the TIM peripheral. \ref TIMER_Declaration
 * \param[in] TIM_TimeBaseInitStruct: Pointer to a TIM_TimeBaseInitTypeDef
 *            structure that contains the configuration information for the selected TIM peripheral.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_timer_init(void)
 * {
 *     RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);
 *
 *     TIM_TimeBaseInitTypeDef TIM_InitStruct;
 *     TIM_StructInit(&TIM_InitStruct);
 *     TIM_InitStruct.TIM_PWM_En = DISABLE;
 *     TIM_InitStruct.TIM_Period = 1000000 - 1 ;
 *     TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
 *     TIM_TimeBaseInit(TIM6, &TIM_InitStruct);
 * }
 * \endcode
 */
void TIM_TimeBaseInit(TIM_TypeDef *TIMx, TIM_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct);

/**
 * \brief   Fills each TIM_InitStruct member with its default value.
 *
 * \param[in] TIM_TimeBaseInitStruct: Pointer to a TIM_TimeBaseInitTypeDef structure which will be initialized.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_timer_init(void)
 * {
 *     RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);
 *
 *     TIM_TimeBaseInitTypeDef TIM_InitStruct;
 *     TIM_StructInit(&TIM_InitStruct);
 *     TIM_InitStruct.TIM_PWM_En = DISABLE;
 *     TIM_InitStruct.TIM_Period = 1000000 - 1;
 *     TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
 *     TIM_TimeBaseInit(TIM6, &TIM_InitStruct);
 * }
 * \endcode
 */
void TIM_StructInit(TIM_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct);

/**
 * \brief   Enables or disables the specified TIM peripheral.
 *
 * \param[in] TIMx: Select the TIM peripheral. \ref TIMER_Declaration
 * \param[in] NewState: New state of the TIMx peripheral.
 *            This parameter can be: ENABLE or DISABLE.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_timer_init(void)
 * {
 *     RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);
 *
 *     TIM_TimeBaseInitTypeDef TIM_InitStruct;
 *     TIM_StructInit(&TIM_InitStruct);
 *     TIM_InitStruct.TIM_PWM_En = DISABLE;
 *     TIM_InitStruct.TIM_Period = 1000000 - 1;
 *     TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
 *     TIM_TimeBaseInit(TIM6, &TIM_InitStruct);
 *     TIM_Cmd(TIM6, ENABLE);
 * }
 * \endcode
 */
void TIM_Cmd(TIM_TypeDef *TIMx, FunctionalState NewState);

/**
 * \brief       Enables or disables the specified TIMx interrupt.
 *
 * \param[in]   TIMx: Select the TIM peripheral. \ref TIMER_Declaration
 * \param[in]   NewState: New state of the TIMx peripheral.
 *              This parameter can be: ENABLE or DISABLE.
 *
 * \return      None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_timer_init(void)
 * {
 *     RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);
 *
 *     TIM_TimeBaseInitTypeDef TIM_InitStruct;
 *     TIM_StructInit(&TIM_InitStruct);
 *     TIM_InitStruct.TIM_PWM_En = DISABLE;
 *     TIM_InitStruct.TIM_Period = 1000000 - 1;
 *     TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
 *     TIM_TimeBaseInit(TIM6, &TIM_InitStruct);
 *     TIM_ClearInterrupt(TIM6);
 *     TIM_InterruptConfig(TIM6, ENABLE);
 */
void TIM_InterruptConfig(TIM_TypeDef *TIMx, uint8_t TIM_INT, FunctionalState NewState);


/**
 * \brief   Check whether the TIM interrupt has occurred or not.
 *
 * \param[in] TIMx: Select the TIM peripheral. \ref TIMER_Declaration
 *
 * \return  The new state of the TIM_IT(SET or RESET).
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void timer_demo(void)
 * {
 *     ITStatus int_status = TIM_GetInterruptStatus(TIM6);
 * }
 * \endcode
 */
ITStatus TIM_GetInterruptStatus(TIM_TypeDef *TIMx, uint8_t TIM_INT);

/**
 * \brief   Clear TIM interrupt.
 *
 * \param[in] TIMx: Select the TIM peripheral. \ref TIMER_Declaration
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void timer_demo(void)
 * {
 *     TIM_ClearInterrupt(TIM6);
 * }
 * \endcode
 */
void TIM_ClearInterrupt(TIM_TypeDef *TIMx, uint8_t TIM_INT);

#if (TIM_SUPPORT_CCR_FIFO == 1 || TIM_SUPPORT_LATCH_CNT_0 == 1)
/**
  * \brief  Check whether the specified TIM flag is set.
  * \param  TIMx: Select the TIM peripheral. \ref TIM_Declaration
  * \param  TIM_FLAG: Specifies the flag to check.
  * \return The new state of TIM_FLAG (SET or RESET).
  */
FlagStatus TIM_GetFIFOFlagStatus(TIM_TypeDef *TIMx, uint32_t TIM_FLAG);
#endif

/**
 * \brief       Change TIM period value.
 *
 * \param[in]   TIMx: Select the TIM peripheral. \ref TIMER_Declaration
 * \param[in]   period: Period value to be changed.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void timer_demo(void)
 * {
 *     uint32_t new_period = 1000000 - 1;
 *     TIM_Cmd(TIM6, DISABLE);
 *     TIM_ChangePeriod(TIM6, new_period);
 *
 * }
 * \endcode
 */
void TIM_ChangePeriod(TIM_TypeDef *TIMx, uint32_t period);

/**
 * \brief   Get TIMx period value.
 *
 * \param[in] TIMx: Select the TIM peripheral. \ref TIMER_Declaration
 *
 * \return  TIM period value.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void timer_demo(void)
 * {
 *     uint32_t period = TIM_GetPeriod(TIM6);
 * }
 * \endcode
 */
uint32_t TIM_GetPeriod(TIM_TypeDef *TIMx);

/**
 * \brief   Get TIMx current value when timer is running.
 *
 * \param[in] TIMx: Select the TIM peripheral. \ref TIMER_Declaration
 *
 * \return  The counter value.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void timer_demo(void)
 * {
 *     uint32_t cur_value = TIM_GetCurrentValue(TIM6);
 * }
 * \endcode
 */
uint32_t TIM_GetCurrentValue(TIM_TypeDef *TIMx);

/**
 * \brief Get TIMx elapsed value when timer is running.
 *
 * \param[in] TIMx: Select the TIM peripheral. \ref TIMER_Declaration
 *
 * \return The elapsed counter value.
 */
uint32_t TIM_GetElapsedValue(TIM_TypeDef *TIMx);

/**
 * \brief   Check whether the TIM is in operation or not.
 *
 * \param[in] TIMx: Select the TIM peripheral. \ref TIMER_Declaration
 *
 * \return  The new state of the timer operation status (SET or RESET).
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void timer_demo(void)
 * {
 *     ITStatus intstatus = TIM_GetOperationStatus(TIM6);
 * }
 * \endcode
 */
FlagStatus TIM_GetOperationStatus(TIM_TypeDef *TIMx);

/**
  * \brief  Enable/disable to pause timer counter.
  * \param  TIMx: Select the TIM peripheral. \ref TIM_Declaration
  * \param  NewState: New state of the TIMx peripheral.
  * \return None.
  */
void TIM_PauseCmd(TIM_TypeDef *TIMx, FunctionalState NewState);

/**
  * \brief  Enables or disables the specified TIM peripheral.
  * \param  TIMx: Select the TIM peripheral. \ref TIM_Declaration
  * \param  NewState: New state of the TIMx peripheral.
  *         This parameter can be: ENABLE or DISABLE.
  * \return None.
  */
void TIM_OneShotEnable(TIM_TypeDef *TIMx);

/**
  * \brief  Get one shot status for the specified TIM peripheral.
  * \param  TIMx: Select the TIM peripheral. \ref TIM_Declaration
  * \param  NewState: New state of the TIMx peripheral.
  *         This parameter can be: ENABLE or DISABLE.
  * \return None.
  */
FlagStatus TIM_GetOneshotStatus(TIM_TypeDef *TIMx);

#if (TIM_SUPPORT_CCR_FIFO == 1)
/**
  * \brief  Set TIMx capture/compare value for user-define PWM auto mode.
  * \param  TIMx: Select the TIM peripheral. \ref TIM_Declaration
  * \param  value:  User-defined TIM capture/compare value for PWM auto mode,
  *         ranging from 0 to 2^31.
  * \return None
  */
void TIM_WriteCCFIFO(TIM_TypeDef *TIMx, uint32_t value);
#endif

#if (TIM_SUPPORT_CCR_FIFO == 1 || TIM_SUPPORT_LATCH_CNT_0 == 1)
/**
  * \brief  Clear capture/compare or latch count0 fifo.
  * \param  TIMx: Select the ENHTIM peripheral. \ref TIM_Declaration
  * \param  FIFO_CLR: Specifies the FIFO type which to be clear.
  * \return FIFO data length.
  */
void TIM_ClearFIFO(TIM_TypeDef *TIMx, uint8_t FIFO_CLR);
#endif

#if (TIM_SUPPORT_LATCH_CNT_0 == 1)
/**
  * \brief  Read TIMx latch counter0 fifo data.
  * \param  TIMx: Select the TIM peripheral. \ref TIM_Declaration
  * \param  length: Latch count0 fifo length, max 8.
  * \pBuf   pBuf: FIFO data out buffer.
  * \return None.
  */
void TIM_ReadLatchCountFIFO(TIM_TypeDef *TIMx, uint32_t *pBuf, uint8_t length);

/**
  * \brief  Get TIM latch counter0 fifo length.
  * \param  TIMx: Select the TIM peripheral. \ref TIM_Declaration
  * \return FIFO data length.
  */
uint8_t TIM_GetLatchCountFIFOLength(TIM_TypeDef *TIMx);
#endif

#if (TIM_SUPPORT_PWM_FUNCTION == 1)
/**
 * \brief   Change PWM freq and duty according high_cnt and low_cnt
 *
 * \param[in]   TIMx: Select the TIM peripheral. \ref TIMER_Declaration
 * \param[in]   high_count: This parameter can be 0x00~0xFFFFFFFF.
 * \param[in]  low_count: This parameter can be 0x00~0xFFFFFFFF.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define TIM_DEMO TIMB_6
 *
 * void timer_demo(void)
 * {
 *     uint32_t high_count = 1000000 - 1;
 *     uint32_t low_count = 1000000 - 1;
 *     TIM_Cmd(TIM_DEMO, DISABLE);
 *     TIM_ChangePeriod(TIM_DEMO, high_count, low_count);
 * }
 * \endcode
 */
void TIM_PWMChangeFreqAndDuty(TIM_TypeDef *TIMx, uint32_t period, uint32_t high_count);

/**
  * \brief  Get TIM toggle state
  * \param  TIMx: Select the TIM peripheral. \ref TIM_Declaration
  * \return The new state of toggle state(SET or RESET).
  */
bool TIM_GetToggleState(TIM_TypeDef *TIMx);

#if (TIM_SUPPORT_PWM_PHASE_SHIFT == 1)
/**
  * \brief  Change TIM pwm_phase_shift count
  * \param  TIMx: Select the TIM peripheral. \ref TIM_Declaration
  * \param  PhaseShiftCnt:This parameter can be 0 to ccr;
  * \return None
  */
void TIM_PhaseShiftCnt(TIM_TypeDef *TIMx, uint32_t PhaseShiftCnt);
#endif

#if (TIM_SUPPORT_PWM_DEADZONE == 1)
/**
  * \brief  TIM PWMP/N Source Select.
  * \param  TIMx: Select the TIM peripheral. \ref TIM_Declaration
  * \param  PWMSrcSel: State of the TIMx PWMP/N.
  * \return None.
  */
void TIM_PWMDZRefSel(TIM_TypeDef *TIMx, PWMDZRef_TypeDef PWMSrcSel);
#endif

/**
 * \brief   PWM complementary output emergency stop and resume.
 *          PWM_P emergency stop level state is configured by PWM_Stop_State_P,
 *          PWM_N emergency stop level state is configured by PWM_Stop_State_N.
 *
 * \param[in] PWMx: Select the PWM peripheral. \ref PWM_Declaration
 * \param[in] NewState: New state of complementary output.
 *            \ref DISABLE: Resume PWM complementary output.
 *            \ref ENABLE: PWM complementary output emergency stop.
 *
 * \return  None.
 * \note    To use this function, need to configure the corresponding timer.
 *          PWM2 ->> TIM2, PWM3 ->> TIM3.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void board_pwm_init(void)
 * {
 *     Pad_Config(P0_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
 *     Pad_Config(P0_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
 *                PAD_OUT_HIGH);
 *     Pad_Config(P2_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
 *                PAD_OUT_HIGH);
 *
 *     Pinmux_Config(P0_1, PWM_OUT_PIN_PINMUX);
 *     Pinmux_Config(P0_2, PWM_OUT_P_PIN_PINMUX);
 *     Pinmux_Config(P2_2, PWM_OUT_N_PIN_PINMUX);
 * }
 *
 * void driver_pwm_init(void)
 * {
 *     RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);
 *
 *     TIM_TimeBaseInitTypeDef TIM_InitStruct;
 *     TIM_StructInit(&TIM_InitStruct);
 *     TIM_InitStruct.TIM_Mode             = TIM_Mode_UserDefine;
 *     TIM_InitStruct.TIM_PWM_En           = ENABLE;
 *     TIM_InitStruct.TIM_PWM_High_Count   = PWM_HIGH_COUNT;
 *     TIM_InitStruct.TIM_PWM_Low_Count    = PWM_LOW_COUNT;
 *     TIM_InitStruct.PWM_Stop_State_P     = PWM_STOP_AT_HIGH;
 *     TIM_InitStruct.PWM_Stop_State_N     = PWM_STOP_AT_LOW;
 *     TIM_InitStruct.PWMDeadZone_En       = ENABLE;  //enable to use pwn p/n output
 *     TIM_InitStruct.PWM_Deazone_Size     = 255;
 *     TIM_TimeBaseInit(PWM2, &TIM_InitStruct);
 *
 *     TIM_Cmd(PWM2, ENABLE);
 * }
 *
 * void pwm_demo(void)
 * {
 *    board_pwm_init();
 *    driver_pwm_init();
 *    //Add delay.
 *    TIM_PWMComplOutputEMCmd(PWM2,ENABLE);
 * }
 * \endcode
 */
void TIM_PWMComplOutputEMCmd(TIM_TypeDef *TIMx, FunctionalState NewState);
#endif

/**
 * \brief  TIM clock config.
 *
 * \param[in] TIMx: Select the TIM peripheral. \ref TIMER_Declaration
 * \param[in] ClockSrc: specifies the PLL clock source.
 * \param[in] ClockDiv: specifies the APB peripheral to gates its clock.
 *
 * \return None
 */
void TIM_ClkConfig(TIM_TypeDef *TIMx, uint16_t ClockSrc, uint16_t ClockDiv);

#if (TIM_SUPPORT_RAP_FUNCTION == 1)

void TIM_RAPModeCmd(TIM_TypeDef *TIMx, FunctionalState NewState);

void TIM_TaskTrigger(TIM_TypeDef *TIMx, uint32_t Task);

void TIM_ShortcutCmd(TIM_TypeDef *TIMx, uint32_t Task, uint32_t Event, FunctionalState NewState);

#endif

#if (TIM_SUPPORT_AUTO_CLOCK == 1)

void TIM_ClkAutoModeConfig(TIM_TypeDef *TIMx, uint32_t AutoMode);

#endif

/** End of TIM_Exported_Functions
  * \}
  */

/** End of TIM
  * \}
  */

#ifdef __cplusplus
}
#endif

#endif /*_RTL876X_TIM_H_*/


/******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/
