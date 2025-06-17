/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_rtc.h
* @brief
* @details
* @author    colin
* @date      2024-06-18
* @version   v1.0
* *********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef RTL_RTC_H
#define RTL_RTC_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "rtl876x.h"
#include "rtl876x_rtc_def.h"

/** \defgroup RTC         RTC
  * \brief
  * \{
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** \defgroup RTC_Exported_Constants RTC Exported Constants
  * \brief
  * \{
  */

#if (RTC_SUPPORT_CLK_INPUT_FROM_PAD_SEL == 1)
/* 32K Register BIT MASK */
#define EXT32K_GPIO_MSK              (0x1UL << 6)
#define RTC_OUT_SEL_MSK              (0x1FFFUL)
#define EN_32K_GPIO_IN_MSK           (0x1UL << 12)
#define RTC_IN_SEL_MSK               (0x1FUL)
#endif

/**
 * \defgroup    RTC_Comparator_Definition RTC Comparator Definition
 * \{
 * \ingroup     RTC_Exported_Constants
 */

typedef enum
{
    RTC_COMP0 = 0x00,
    RTC_COMP1 = 0x01,
    RTC_COMP2 = 0x02,
    RTC_COMP3 = 0x03,
} RTCComIndex_TypeDef;

#define IS_RTC_COMP(COMP) (((COMP) == RTC_COMP0) || \
                           ((COMP) == RTC_COMP1) || \
                           ((COMP) == RTC_COMP2) || \
                           ((COMP) == RTC_COMP3))

/** End of RTC_Comparator_Definition
  * \}
  */

#if (RTC_SUPPORT_COMPARE_GUARDTIME == 1)
/**
 * \defgroup    RTC_ComparatorGT_Definition RTC ComparatorGT Definition
 * \{
 * \ingroup     RTC_Exported_Constants
 */

typedef enum
{
    RTC_COMP0GT = 0x0,
    RTC_COMP1GT = 0x1,
    RTC_COMP2GT = 0x2,
    RTC_COMP3GT = 0x3,
} RTCCmopGTIndex_TypeDef;

#define IS_RTC_COMPGT(COMP) (((COMP) == RTC_COMP0GT) || \
                             ((COMP) == RTC_COMP1GT) || \
                             ((COMP) == RTC_COMP2GT) || \
                             ((COMP) == RTC_COMP3GT))

/** End of RTC_ComparatorGT_Definition
  * \}
  */
#endif

/**
 * \brief       RTC in selection type
 *
 * \ingroup     RTC_Exported_Constants
 */
#if (RTC_SUPPORT_CLK_INPUT_FROM_PAD_SEL == 1)
typedef enum
{
    PAD_RTC_IN_DISABLE,
    PAD_RTC_IN_P0_1,
    PAD_RTC_IN_P1_2,
    PAD_RTC_IN_P2_0,
    PAD_RTC_IN_P2_7,
    PAD_RTC_IN_P4_7,
    PAD_RTC_IN_P5_0,
    PAD_RTC_IN_P8_6,
    PAD_RTC_IN_P9_3,
    PAD_RTC_IN_P10_6,
    PAD_RTC_IN_P13_7,
    PAD_RTC_IN_P14_7,
    PAD_RTC_IN_P16_3,
    PAD_RTC_IN_P20_5,
} RTCInSel_TypeDef;

#define RTC_ALL_IN_SEL         (PAD_RTC_IN_DISABLE | PAD_RTC_IN_P0_1 | \
                                PAD_RTC_IN_P1_2 | PAD_RTC_IN_P2_0 | \
                                PAD_RTC_IN_P2_7 | PAD_RTC_IN_P4_7 | \
                                PAD_RTC_IN_P5_0 | PAD_RTC_IN_P8_6 | \
                                PAD_RTC_IN_P9_3 | PAD_RTC_IN_P10_6 | \
                                PAD_RTC_IN_P13_7 | PAD_RTC_IN_P14_7 | \
                                PAD_RTC_IN_P16_3 | PAD_RTC_IN_P20_5)
#endif

/**
 * \defgroup    RTC_Interrupts RTC Interrupts
 * \{
 * \ingroup     RTC_Exported_Constants
 */
#define RTC_INT_TICK           BIT8
#define RTC_INT_OVF            BIT9
#define RTC_INT_PRE_COMP       BIT10
#define RTC_INT_PRE_COMP3      BIT11
#define RTC_INT_COMP0          BIT16
#define RTC_INT_COMP1          BIT17
#define RTC_INT_COMP2          BIT18
#define RTC_INT_COMP3          BIT19

#define IS_RTC_INT(INT) (((INT) == RTC_INT_TICK) || \
                         ((INT) == RTC_INT_OVF) || \
                         ((INT) == RTC_INT_COMP0) || \
                         ((INT) == RTC_INT_COMP1) || \
                         ((INT) == RTC_INT_COMP2) || \
                         ((INT) == RTC_INT_COMP3) || \
                         ((INT) == RTC_INT_PRE_COMP) || \
                         ((INT) == RTC_INT_PRE_COMP3))

/** End of RTC_Interrupts
  * \}
  */

/**
 * \defgroup    RTC_Wakeup RTC Wakeup
 * \{
 * \ingroup     RTC_Exported_Constants
 */
#if (RTC_SUPPORT_PRE_COMP_OVF_TICK_WAKE_UP == 1)
#define RTC_WK_TICK            BIT8
#define RTC_WK_OVF             BIT9
#define RTC_WK_PRE_COMP        BIT10
#define RTC_WK_PRE_COMP3       BIT11
#endif
#if (RTC_SUPPORT_COMPARE_GUARDTIME == 1)
#define RTC_WK_COMP0GT         BIT12
#define RTC_WK_COMP1GT         BIT13
#define RTC_WK_COMP2GT         BIT14
#define RTC_WK_COMP3GT         BIT15
#endif
#define RTC_WK_COMP0           BIT20
#define RTC_WK_COMP1           BIT21
#define RTC_WK_COMP2           BIT22
#define RTC_WK_COMP3           BIT23

#if (RTC_SUPPORT_PRE_COMP_OVF_TICK_WAKE_UP == 1)
#define IS_RTC_WK(WK) (((WK) == RTC_WK_TICK) || \
                       ((WK) == RTC_WK_OVF) || \
                       ((WK) == RTC_WK_PRE_COMP) || \
                       ((WK) == RTC_WK_PRE_COMP3) || \
                       ((WK) == RTC_WK_COMP0) || \
                       ((WK) == RTC_WK_COMP1) || \
                       ((WK) == RTC_WK_COMP2) || \
                       ((WK) == RTC_WK_COMP3))
#else
#define IS_RTC_WK(WK) (((WK) == RTC_WK_COMP0) || \
                       ((WK) == RTC_WK_COMP1) || \
                       ((WK) == RTC_WK_COMP2) || \
                       ((WK) == RTC_WK_COMP3))
#endif

/** End of RTC_Wakeup
  * \}
  */

/**
 * \defgroup    RTC_Int_Clear RTC Int Clear
 * \{
 * \ingroup     RTC_Exported_Constants
 */
#define RTC_COMP3_CLR               (RTC_INT_COMP3     >> 8)
#define RTC_COMP2_CLR               (RTC_INT_COMP2     >> 8)
#define RTC_COMP1_CLR               (RTC_INT_COMP1     >> 8)
#define RTC_COMP0_CLR               (RTC_INT_COMP0     >> 8)
#define RTC_PRE_COMP3_CLR           (RTC_INT_PRE_COMP3 >> 8)
#define RTC_PRE_COMP_CLR            (RTC_INT_PRE_COMP  >> 8)
#define RTC_OVERFLOW_CLR            (RTC_INT_OVF       >> 8)
#define RTC_TICK_CLR                (RTC_INT_TICK      >> 8)

#define RTC_ALL_INT_CLR             (RTC_PRE_COMP3_CLR | RTC_PRE_COMP_CLR | \
                                     RTC_COMP3_CLR | RTC_COMP2_CLR | \
                                     RTC_COMP1_CLR | RTC_COMP0_CLR | \
                                     RTC_OVERFLOW_CLR | RTC_TICK_CLR)

/** End of RTC_Int_Clear
  * \}
  */

/**
 * \defgroup    RTC_WK_Clear RTC WK Clear
 * \{
 * \ingroup     RTC_Exported_Constants
 */
#define RTC_COMP3_WK_CLR            (RTC_WK_COMP3   >> 8)
#define RTC_COMP2_WK_CLR            (RTC_WK_COMP2   >> 8)
#define RTC_COMP1_WK_CLR            (RTC_WK_COMP1   >> 8)
#define RTC_COMP0_WK_CLR            (RTC_WK_COMP0   >> 8)
#if (RTC_SUPPORT_COMPARE_GUARDTIME == 1)
#define RTC_COMP3GT_CLR             (RTC_WK_COMP3GT >> 8)
#define RTC_COMP2GT_CLR             (RTC_WK_COMP2GT >> 8)
#define RTC_COMP1GT_CLR             (RTC_WK_COMP1GT >> 8)
#define RTC_COMP0GT_CLR             (RTC_WK_COMP0GT >> 8)
#endif

#define RTC_ALL_WAKEUP_CLR          (RTC_COMP3_WK_CLR | RTC_COMP2_WK_CLR | \
                                     RTC_COMP1_WK_CLR | RTC_COMP0_WK_CLR)

/** End of RTC_WK_Clear
  * \}
  */

/** End of RTC_Exported_Constants
  * \}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** \defgroup RTC_Exported_Functions RTC Exported Functions
  * \brief
  * \{
  */

/**
 * \brief     Deinitializes the RTC peripheral registers to their default reset values(turn off clock).
 *
 * \param[in] None.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_DeInit();
 * }
 * \endcode
 */
void RTC_DeInit(void);

/**
 * \brief     Set RTC prescaler value.
 *
 * \param[in] value: The prescaler value to be set.Should be no more than 12 bits!
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_PRESCALER_VALUE     49
 * #define RTC_COMP_INDEX          RTC_COMP3
 * #define RTC_COMP_INDEX_INT      RTC_INT_COMP3
 * #define RTC_COMP_VALUE          (1000)
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_DeInit();
 *
 *     RTC_SetPrescaler(RTC_PRESCALER_VALUE);
 *     RTC_SetCompValue(RTC_COMP_INDEX, RTC_COMP_VALUE);
 *
 *     RTC_INTConfig(RTC_COMP_INDEX_INT, ENABLE);
 *
 *     RTC_NvCmd(ENABLE);
 *     RTC_Cmd(ENABLE);
 * }
 * \endcode
 */
void RTC_SetPrescaler(uint16_t value);

/**
 * \brief     Start or stop RTC peripheral.
 *
 * \param[in] NewState: New state of RTC peripheral.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Start RTC.
 *            \arg DISABLE: Stop RTC.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_PRESCALER_VALUE     49
 * #define RTC_COMP_INDEX          RTC_COMP3
 * #define RTC_COMP_INDEX_INT      RTC_INT_COMP3
 * #define RTC_COMP_VALUE          (1000)
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_DeInit();
 *
 *     RTC_SetPrescaler(RTC_PRESCALER_VALUE);
 *     RTC_SetCompValue(RTC_COMP_INDEX, RTC_COMP_VALUE);
 *
 *     RTC_INTConfig(RTC_COMP_INDEX_INT, ENABLE);
 *
 *     RTC_NvCmd(ENABLE);
 *     RTC_Cmd(ENABLE);
 * }
 * \endcode
 */
void RTC_Cmd(FunctionalState NewState);

/**
 * \brief     Enable or disable the specified RTC interrupt source.
 *
 * \param[in] RTC_INT: Specifies the RTC interrupt source which to be enabled or disabled.
 *            This parameter can be any combination of the following values:
 *            \arg RTC_INT_TICK: Tick interrupt source.
 *            \arg RTC_INT_OVF: counter overflow interrupt
 *            \arg RTC_INT_COMP0: Compare 0 interrupt source.
 *            \arg RTC_INT_COMP1: Compare 1 interrupt source.
 *            \arg RTC_INT_COMP2: Compare 2 interrupt source.
 *            \arg RTC_INT_COMP3: Compare 3 interrupt source.
 *            \arg RTC_INT_PRE_COMP: Prescale compare interrupt source.
 *            \arg RTC_INT_PRE_COMP3: Prescale & compare 3 interrupt source.
 * \param[in] NewState: New state of the specified RTC interrupt.
 *            This parameter can be: ENABLE or DISABLE.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_PRESCALER_VALUE     49
 * #define RTC_COMP_INDEX          RTC_COMP3
 * #define RTC_COMP_INDEX_INT      RTC_INT_COMP3
 * #define RTC_COMP_VALUE          (1000)
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_DeInit();
 *
 *     RTC_SetPrescaler(RTC_PRESCALER_VALUE);
 *     RTC_SetCompValue(RTC_COMP_INDEX, RTC_COMP_VALUE);
 *
 *     RTC_INTConfig(RTC_COMP_INDEX_INT, ENABLE);
 *
 *     RTC_NvCmd(ENABLE);
 *     RTC_Cmd(ENABLE);
 * }
 * \endcode
 */
void RTC_INTConfig(uint32_t RTC_INT, FunctionalState NewState);

/**
 * \brief  Enable or disable the specified RTC wakeup function.
 *
 * \param[in] RTC_WK: specifies the RTC wakeup function to be enabled or disabled.
 *            This parameter can be any combination of the following values:
 *            \arg RTC_WK_TICK: tick wakeup function
 *            \arg RTC_WK_OVF: tick wakeup function
 *            \arg RTC_WK_PRE_CMP: prescale compare wakeup function
 *            \arg RTC_WK_PRE_CMP3: prescale & compare 3 wakeup function
 *            \arg RTC_WK_COMP0GT: compare 0 gt wakeup function
 *            \arg RTC_WK_COMP1GT: compare 1 gt wakeup function
 *            \arg RTC_WK_COMP2GT: compare 2 gt wakeup function
 *            \arg RTC_WK_COMP3GT: compare 3 gt wakeup function
 *            \arg RTC_WK_CMP0: compare 0 wakeup function
 *            \arg RTC_WK_CMP1: compare 1 wakeup function
 *            \arg RTC_WK_CMP2: compare 2 wakeup function
 *            \arg RTC_WK_CMP3: compare 3 wakeup function
 * \param[in] NewState: new state of the specified RTC wakeup function.
 *            This parameter can be: ENABLE or DISABLE.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_PRESCALER_VALUE     49
 * #define RTC_COMP_INDEX          RTC_COMP3
 * #define RTC_COMP_INDEX_WK      RTC_WK_COMP2
 * #define RTC_COMP_VALUE          (1000)
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_DeInit();
 *
 *     RTC_SetPrescaler(RTC_PRESCALER_VALUE);
 *     RTC_SetCompValue(RTC_COMP_INDEX, RTC_COMP_VALUE);
 *
 *     RTC_WKConfig(RTC_COMP_INDEX_WK, ENABLE);
 *
 *     RTC_SystemWakeupConfig(ENABLE);
 *     RTC_Cmd(ENABLE);
 * }
 * \endcode
 */
void RTC_WKConfig(uint32_t RTC_WK, FunctionalState NewState);

/**
 * \brief     Enable RTC interrupt signal to CPU NVIC.
 *
 * \param[in] NewState: Enable or disable RTC interrupt signal to MCU.
 *            This parameter can be: ENABLE or DISABLE.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_PRESCALER_VALUE     49
 * #define RTC_COMP_INDEX          RTC_COMP3
 * #define RTC_COMP_INDEX_INT      RTC_INT_COMP3
 * #define RTC_COMP_VALUE          (1000)
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_DeInit();
 *
 *     RTC_SetPrescaler(RTC_PRESCALER_VALUE);
 *     RTC_SetCompValue(RTC_COMP_INDEX, RTC_COMP_VALUE);
 *
 *     RTC_INTConfig(RTC_COMP_INDEX_INT, ENABLE);
 *
 *     RTC_NvCmd(ENABLE);
 *     RTC_Cmd(ENABLE);
 * }
 * \endcode
 */
void RTC_NvCmd(FunctionalState NewState);

/**
 * \brief     Enable or disable system wake up function of RTC.
 *
 * \param[in] NewState: new state of the wake up function.
 *            This parameter can be: ENABLE or DISABLE.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_demo(void)
 * {
 *     RTC_SystemWakeupConfig(ENABLE);
 * }
 * \endcode
 */
void RTC_SystemWakeupConfig(FunctionalState NewState);

/**
 * \brief     Reset counter value of RTC.
 *
 * \param[in] None.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_demo(void)
 * {
 *     RTC_ResetCounter();
 *     RTC_Cmd(ENABLE);
 * }
 * \endcode
 */
void RTC_ResetCounter(void);

/**
 * \brief     Reset prescaler counter value of RTC.
 *
 * \param[in] None.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_demo(void)
 * {
 *     RTC_ResetPrescalerCounter();
 *     RTC_Cmd(ENABLE);
 * }
 * \endcode
 */
void RTC_ResetPrescalerCounter(void);

/**
 * \brief  Check whether the specified RTC interrupt is set.
 *
 * \param[in] RTC_INT: Specifies the RTC interrupt source to be enabled or disabled.
 *            This parameter can be any combination of the following values:
 *            \arg RTC_INT_TICK: RTC tick interrupt source.
 *            \arg RTC_INT_COMP0: Compare 0 interrupt source.
 *            \arg RTC_INT_COMP1: Compare 1 interrupt source.
 *            \arg RTC_INT_COMP2: Compare 2 interrupt source.
 *            \arg RTC_INT_COMP3: Compare 3 interrupt source.
 *            \arg RTC_INT_PRE_COMP: Prescale compare interrupt source.
 *            \arg RTC_INT_PRE_COMP3: Prescale & compare 3 interrupt source.
 *
 * \return The new state of RTC_INT (SET or RESET).
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_demo(void)
 * {
 *     ITStatus int_status = RTC_GetINTStatus(RTC_INT_COMP0);
 * }
 * \endcode
 */
ITStatus RTC_GetINTStatus(uint32_t RTC_INT);

/**
 * \brief     Clear the interrupt pending bits of RTC.
 *
 * \param[in] RTC_INT: specifies the RTC interrupt flag to clear.
 *            This parameter can be any combination of the following values:
 *            \arg RTC_INT_TICK: RTC tick interrupt source.
 *            \arg RTC_INT_OVF: RTC counter overflow interrupt source.
 *            \arg RTC_INT_COMP0: Compare 0 interrupt source.
 *            \arg RTC_INT_COMP1: Compare 1 interrupt source.
 *            \arg RTC_INT_COMP2: Compare 2 interrupt source.
 *            \arg RTC_INT_COMP3: Compare 3 interrupt source.
 *            \arg RTC_INT_PRE_COMP: Prescale compare interrupt source.
 *            \arg RTC_INT_PRE_COMP3: Prescale & compare 3 interrupt source.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_demo(void)
 * {
 *     RTC_ClearINTPendingBit(RTC_INT_COMP0);
 * }
 * \endcode
 */
void RTC_ClearINTPendingBit(uint32_t RTC_INT);

/**
 * \brief  Checks whether the specified RTC wakeup state is set or not.
 *
 * \param[in] RTC_WK: specifies the RTC interrupt source to be enabled or disabled.
 *            This parameter can be any combination of the following values:
 *            \arg RTC_WK_TICK: tick wakeup function
 *            \arg RTC_WK_OVF: tick wakeup function
 *            \arg RTC_WK_PRE_CMP: prescale compare wakeup function
 *            \arg RTC_WK_PRE_CMP3: prescale & compare 3 wakeup function
 *            \arg RTC_WK_CMP0: compare 0 wakeup function
 *            \arg RTC_WK_CMP1: compare 1 wakeup function
 *            \arg RTC_WK_CMP2: compare 2 wakeup function
 *            \arg RTC_WK_CMP3: compare 3 wakeup function
 *
 * \return The new state of RTC_INT (SET or RESET).
 */
ITStatus RTC_GetWakeupStatus(uint32_t RTC_WK);

/**
 * \brief  Clear the wakeup status bits of RTC.
 *
 * \param[in] RTC_WK: specifies the RTC wakeup flag to clear.
 *            This parameter can be any combination of the following values:
 *            \arg RTC_WK_TICK: tick wakeup function
 *            \arg RTC_WK_OVF: tick wakeup function
 *            \arg RTC_WK_PRE_CMP: prescale compare wakeup function
 *            \arg RTC_WK_PRE_CMP3: prescale & compare 3 wakeup function
 *            \arg RTC_WK_CMP0: compare 0 wakeup function
 *            \arg RTC_WK_CMP1: compare 1 wakeup function
 *            \arg RTC_WK_CMP2: compare 2 wakeup function
 *            \arg RTC_WK_CMP3: compare 3 wakeup function
 *
 * \return None.
 */
void RTC_ClearWakeupStatusBit(uint32_t RTC_WK);

/**
 * \brief  Clear the comparator wakeup status.
 *
 * \param[in] index: The comparator number.
 *
 * \return None.
 */
void RTC_ClearCompWkINT(RTCComIndex_TypeDef index);

/**
 * \brief     Clear the interrupt pending bit of the select comparator of RTC.
 *
 * \param[in] index: the comparator number.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_demo(void)
 * {
 *     RTC_ClearCompINT(0);
 * }
 * \endcode
 */
void RTC_ClearCompINT(RTCComIndex_TypeDef index);

/**
 * \brief     Clear the overflow interrupt pending bit of RTC.
 *
 * \param[in] None.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_demo(void)
 * {
 *     RTC_ClearOverFlowINT();
 * }
 * \endcode
 */
void RTC_ClearOverFlowINT(void);

/**
 * \brief     Clear the tick interrupt pending bit of RTC.
 *
 * \param[in] None.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_demo(void)
 * {
 *     RTC_ClearTickINT();
 * }
 * \endcode
 */
void RTC_ClearTickINT(void);

#if (RTC_SUPPORT_CLK_INPUT_FROM_PAD_SEL == 1)
/**
 * \brief  Select source clock to gpio input of RTC.
 *
 * \param[in]  gpio: the selected gpio.
 *
 * \return None
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_demo(void)
 * {
 *     RTC_SelectSrcToGpioInput(PAD_RTC_IN_P0_1);
 * }
 * \endcode
 */
void RTC_SelectSrcToGpioInput(RTCInSel_TypeDef rtc_in);
#endif

/**
 * \brief     Set RTC comparator value.
 *
 * \param[in] index: The comparator number,can be 0 ~ 3.
 * \param[in] value: The comparator value to be set.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_PRESCALER_VALUE     49
 * #define RTC_COMP_INDEX          RTC_COMP3
 * #define RTC_COMP_INDEX_INT      RTC_INT_COMP3
 * #define RTC_COMP_VALUE          (1000)
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_DeInit();
 *
 *     RTC_SetPrescaler(RTC_PRESCALER_VALUE);
 *     RTC_SetCompValue(RTC_COMP_INDEX, RTC_COMP_VALUE);
 *
 *     RTC_MaskINTConfig(RTC_COMP_INDEX_INT, DISABLE);
 *     RTC_INTConfig(RTC_COMP_INDEX_INT, ENABLE);
 *
 *     RTC_NvCmd(ENABLE);
 *     RTC_Cmd(ENABLE);
 * }
 * \endcode
 */
void RTC_SetCompValue(RTCComIndex_TypeDef index, uint32_t value);

#if (RTC_SUPPORT_COMPARE_GUARDTIME == 1)
/**
 * \brief     Set RTC comparator GT value.
 *
 * \param[in] index: The comparator gt number, can be 0 ~ 3.
 * \param[in] value: The comparator value to be set.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_PRESCALER_VALUE     49
 * #define RTC_COMP_INDEX          RTC_COMP3
 * #define RTC_COMP_INDEX_INT      RTC_INT_COMP3
 * #define RTC_COMP_VALUE          (1000)
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_DeInit();
 *
 *     RTC_SetPrescaler(RTC_PRESCALER_VALUE);
 *     RTC_SetCompValue(RTC_COMP_INDEX, RTC_COMP_VALUE);
 *
 *     RTC_MaskINTConfig(RTC_COMP_INDEX_INT, DISABLE);
 *     RTC_INTConfig(RTC_COMP_INDEX_INT, ENABLE);
 *
 *     RTC_NvCmd(ENABLE);
 *     RTC_Cmd(ENABLE);
 * }
 * \endcode
 */
void RTC_SetCompGTValue(RTCCmopGTIndex_TypeDef index, uint32_t value);
#endif

/**
 * \brief     Set RTC prescaler comparator value.
 *
 * \param[in] value: The comparator value to be set.Should be no more than 12 bits!
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_PRESCALER_VALUE     (3200 - 1)//max 4095
 * #define RTC_PRECOMP_VALUE       (320)//max 4095
 * #define RTC_COMP3_VALUE         (10)
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_DeInit();
 *
 *     RTC_SetPrescaler(RTC_PRESCALER_VALUE);
 *     RTC_SetPreCompValue(RTC_PRECOMP_VALUE);
 *     RTC_SetCompValue(RTC_COMP3, RTC_COMP3_VALUE);
 *
 *     RTC_MaskINTConfig(RTC_INT_PRE_COMP3, DISABLE);
 *     RTC_INTConfig(RTC_INT_PRE_COMP3, ENABLE);
 *
 *     RTC_NvCmd(ENABLE);
 *     RTC_Cmd(ENABLE);
 * }
 * \endcode
 */
void RTC_SetPreCompValue(uint32_t value);

/**
 * \brief     Get counter value of RTC.
 *
 * \param[in] None.
 *
 * \return    The counter value.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_demo(void)
 * {
 *     uitn32_t counter = RTC_GetCounter();
 * }
 * \endcode
 */
uint32_t RTC_GetCounter(void);

/**
 * \brief     Get prescaler counter value of RTC.
 *
 * \param[in] None.
 *
 * \return    The prescaler counter value.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_demo(void)
 * {
 *     uitn32_t pre_counter = RTC_GetPreCounter();
 * }
 * \endcode
 */
uint32_t RTC_GetPreCounter(void);

/**
 * \brief     Get RTC comparator value.
 *
 * \param[in] index: The comparator number.
 *
 * \return    The comparator value.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_demo(void)
 * {
 *     uint32_t data = RTC_GetCompValue(RTC_COMP0);
 * }
 * \endcode
 */
uint32_t RTC_GetCompValue(RTCComIndex_TypeDef index);

#if (RTC_SUPPORT_COMPARE_GUARDTIME == 1)
/**
 * \brief     Get RTC comparator gt value.
 *
 * \param[in] index: The comparator number 0~3.
 *
 * \return    The comparator value.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_demo(void)
 * {
 *     uitn32_t data = RTC_GetCompGTValue(0);
 * }
 * \endcode
 */
uint32_t RTC_GetCompGTValue(RTCCmopGTIndex_TypeDef index);
#endif

/**
 * \brief     Get RTC prescaler comparator value.
 *
 * \param[in] None.
 *
 * \return    The prescaler comparator value.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_demo(void)
 * {
 *     uitn32_t data = RTC_GetPreCompValue();
 * }
 * \endcode
 */
uint32_t RTC_GetPreCompValue(void);

/**
 * \brief     Write backup register for store time information.
 *
 * \param[in] value: valuer=write to back up reister
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_demo(void)
 * {
 *     RTC_WriteBackupReg(0x01020304);
 * }
 * \endcode
 */
void RTC_WriteBackupReg(uint32_t value);

/**
 * \brief     Read backup register.
 *
 * \param[in] None.
 *
 * \return    Register value.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_demo(void)
 * {
 *     uint32_t reg_data = RTC_ReadBackupReg();
 * }
 * \endcode
 */
uint32_t RTC_ReadBackupReg(void);


/**
  * @brief  mask bit remove from the bbpro2 ,for compatible with bbpro and bblite overflow keep this alive.
  * @param  RTC_INT: specifies the RTC interrupt source to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  * @param  NewState: new state of the specified RTC interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
#define RTC_EN_INTConfig     RTC_MaskINTConfig
void RTC_MaskINTConfig(uint32_t RTC_INT, FunctionalState NewState);

/**
  * @brief  Enable or disable the specified RTC interrupts for comb.
  * @param  RTC_INT: specifies the RTC interrupt source
  *         RTC_INT arge can reference to RTC_INT_t .
  *     @arg RTC_INT_COMPX : compare  x interrupt x =0~3
  *     @arg RTC_WK_COMPX : RTC_CMPx wakeup  Enable x =0~3
  * @param  NewState: new state of the specified RTC interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RTC_CompINTConfig(uint32_t RTC_INT, FunctionalState NewState);

/**
  * @brief  Enable or disable RTC tick interrupts.
  * @param  NewState: new state of RTC tick interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RTC_TickINTConfig(FunctionalState NewState);

#if (RTC_SUPPORT_COMPARE_AUTO_RELOAD == 1)
/**
 * \brief     Set RTC comparator auto reload value.
 *
 * \param[in] index: The comparator number,can be 0 ~ 3.
 * \param[in] value: The comparator value to be set.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_COMP_INDEX          RTC_COMP3
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_SetCompReloadValue(RTC_COMP_INDEX, RTC_COMP_VALUE);
 * }
 * \endcode
 */
void RTC_SetCompReloadValue(RTCComIndex_TypeDef index, uint32_t value);

/**
 * \brief     Get RTC comparator auto reload value.
 *
 * \param[in] index: The comparator number.
 *
 * \return    The comparator value.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_demo(void)
 * {
 *     uint32_t data = RTC_GetCompReloadValue(RTC_COMP0);
 * }
 * \endcode
 */
uint32_t RTC_GetCompReloadValue(RTCComIndex_TypeDef index);

/**
 * \brief     Enable RTC comparator auto reload.
 * When the counter value reaches the value of comparator, it automatically adds the cmp value,
 * which equals Current Compare + Reload Value.
 * \param[in] comp_value: The initialize value of comparator.
 * \param[in] reload_value: The comparator auto reload value.
 *
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_EnableCompAutoReload(RTC_COMP0, 1000, 1000);
 * }
 * \endcode
 */
void RTC_EnableCompAutoReload(RTCComIndex_TypeDef index, uint32_t comp_value,
                              uint32_t reload_value);

/**
 * \brief     Disable RTC comparator auto reload.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_DisableCompAutoReload(RTC_COMP0);
 * }
 * \endcode
 */
void RTC_DisableCompAutoReload(RTCComIndex_TypeDef index);
#endif
/** End of RTC_Exported_Functions
  * \}
  */

/** End of RTC
  * \}
  */

#ifdef __cplusplus
}
#endif

#endif /* RTL_RTC_H */


/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/

