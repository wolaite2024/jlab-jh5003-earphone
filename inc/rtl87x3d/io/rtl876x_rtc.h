/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_rtc.h
* @brief
* @details
* @author    justin
* @date      2024-07-18
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _RTL876X_RTC_H_
#define _RTL876X_RTC_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"


/* define ------------------------------------------------------------------*/
typedef enum _COMPX_INDEX
{
    COMP0_INDEX = 0,            /*0x110*/
    COMP1_INDEX,                /*0x114*/
    COMP2_INDEX,                /*0x118*/
    COMP3_INDEX,                /*0x11C*/
    COMP0GT_INDEX,               /*0x120*/
    COMP1GT_INDEX,               /*0x124*/
    COMP2GT_INDEX,               /*0x128*/
    COMP3GT_INDEX,               /*0x12C*/
    BACKUP0,                     /*0x130*/
    BACKUP1,
    PRESCALER_COMP_INDEX,        /*0x138*/
} COMPX_INDEX_t;

typedef enum _INT_STATUS_POS
{
    RTC_TICK_CLR = 0,
    RTC_CNT_OV_CLR,
    RTC_PRECMP_CLR,
    RTC_PRECMP_CMP3_CLR,
    RTC_CMP0GT_CLR,           /*4*/
    RTC_CMP1GT_CLR,
    RTC_CMP2GT_CLR,
    RTC_CMP3GT_CLR,
    RTC_CMP0_NV_CLR,        /*8*/
    RTC_CMP1_NV_CLR,
    RTC_CMP2_NV_CLR,
    RTC_CMP3_NV_CLR,
    RTC_CMP0_WK_CLR,         /*12*/
    RTC_CMP1_WK_CLR,
    RTC_CMP2_WK_CLR,
    RTC_CMP3_WK_CLR,
} INT_STATUS_POS;


typedef enum _RTC_INT
{
    RTC_INT_TICK        = BIT(8),
    RTC_INT_OVF         = BIT(9),
    RTC_PRECMP_INT      = BIT(10),
    RTC_PRECMP_CMP3_INT = BIT(11),

    RTC_CMP0GT_INT      = BIT(12),
    RTC_CMP1GT_INT      = BIT(13),
    RTC_CMP2GT_INT      = BIT(14),
    RTC_CMP3GT_INT      = BIT(15),

    RTC_CMP0_NV_INT      = BIT(16),
    RTC_INT_CMP0     = BIT(16),        /*compatible  bbpro*/

    RTC_CMP1_NV_INT     = BIT(17),
    RTC_INT_CMP1     = BIT(17),        /*compatible  bbpro*/

    RTC_CMP2_NV_INT     = BIT(18),
    RTC_INT_CMP2     = BIT(18),       /*compatible  bbpro*/

    RTC_CMP3_NV_INT     = BIT(19),
    RTC_INT_CMP3     = BIT(19),       /*compatible  bbpro*/

    RTC_CMP0_WK_INT     = BIT(20),
    RTC_CMP1_WK_INT     = BIT(21),
    RTC_CMP2_WK_INT     = BIT(22),
    RTC_CMP3_WK_INT     = BIT(23),

} RTC_INT_t;
/* Register: CR0 bit29~31 */
#define RTC_START_EN          BIT(0)
#define RTC_START_CLR         (~(RTC_START_EN))
/*compatible  bbpro*/
#define RTC_START_Msk                     RTC_START_EN

#define RTC_COUNTER_RST_EN  BIT(1)
/*compatible  bbpro*/
#define RTC_COUNTER_RST_Msk          RTC_COUNTER_RST_EN

#define RTC_DIV_COUNTER_RST_EN    BIT(2)
/*compatible  bbpro*/
#define RTC_DIV_COUNTER_RST_Msk          RTC_DIV_COUNTER_RST_EN

#define RTC_WAKEUP_EN      BIT(29)
#define RTC_NV_IE_EN_GLB   BIT(30)
#define RTC_REST_EN        BIT(31)


/* Peripheral: RTC */
/* Description: Real time counter register defines */

/* Register: sleep control global register0 */
#define RTC_SLEEP_CR                    (0x40000100UL)

/* Clear all interrupt */
#define RTC_ALL_INT_CLR_SET             0x3fffff

#define RTC_ALL_INT_CLR_RESET           0xffc00000

/** @addtogroup 87x3d_RTC RTC
  * @brief RTC driver module.
  * @{
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/


/** @defgroup RTC_Exported_Constants RTC Exported Constants
  * @{
  */

/** @defgroup RTC_interrupts_Definition RTC Interrupts Definition
  * @{
  */

#define IS_RTC_CONFIG_INT(INT) (((INT) == RTC_INT_TICK) || \
                                ((INT) == RTC_CNT_OV_INT_EN) || \
                                ((INT) == RTC_PRECMP_INT_EN) || \
                                ((INT) == RTC_PRECMPCP3_INT_EN) || \
                                ((INT) == RTC_CMP0_NV_INT_EN) || \
                                ((INT) == RTC_CMP1_NV_INT_EN) || \
                                ((INT) == RTC_CMP2_NV_INT_EN) || \
                                ((INT) == RTC_CMP3_NV_INT_EN) || \
                                ((INT) == RTC_CMP0GT_INT_EN) || \
                                ((INT) == RTC_CMP1GT_INT_EN) || \
                                ((INT) == RTC_CMP2GT_INT_EN) || \
                                ((INT) == RTC_CMP3GT_INT_EN))

#define IS_RTC_COMP_INT(INT) ( ((INT) == RTC_CMP0_NV_INT_EN) || \
                               ((INT) == RTC_CMP1_NV_INT_EN) || \
                               ((INT) == RTC_CMP2_NV_INT_EN) || \
                               ((INT) == RTC_CMP3_NV_INT_EN))

/** End of group RTC_interrupts_Definition
  * @}
  */

/** @defgroup RTC_sleep_mode_clock_definition RTC Sleep Mode Clock Definition
  * @{
  */
#define RTC_EXTERNAL_CLK            EXTERNAL_CLK
#define RTC_INTERNAL_CLK            INTERNAL_CLK
#define IS_RTC_SLEEP_CLK(CLK) ( ((CLK) == EXTERNAL_CLK) || \
                                ((CLK) == INTERNAL_CLK))

/** End of group RTC_sleep_mode_clock_definition
  * @}
  */

/** End of group RTC_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/


/** @defgroup RTC_Exported_Functions RTC Exported Functions
  * @{
  */

/**
 * rtl876x_rtc.h
 *
 * \brief     Deinitializes the RTC peripheral registers to their default reset values(turn off clock).
 *
 * \param[in] None.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_PRESCALER_VALUE     320
 * #define RTC_COMP_INDEX          1
 * #define RTC_INT_CMP_1      RTC_INT_COMP1
 * #define RTC_COMP_VALUE          (1000)
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_DeInit();
 *     RTC_SetPrescaler(RTC_PRESCALER_VALUE);
 *     RTC_SetComp(RTC_COMP_INDEX, RTC_COMP_VALUE);
 *     RTC_MaskINTConfig(RTC_INT_CMP_1, DISABLE);
 *     RTC_CompINTConfig(RTC_INT_CMP_1, ENABLE);
 *
 *     //Start RTC
 *     RTC_SystemWakeupConfig(ENABLE);
 *     RTC_RunCmd(ENABLE);
 * }
 * \endcode
 */
void RTC_DeInit(void);

/**
 * rtl876x_rtc.h
 *
 * \brief     Set RTC prescaler value.
 *
 * \param[in] value: The prescaler value to be set. Should be no more than 12 bits!
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_PRESCALER_VALUE     320
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_SetPrescaler(RTC_PRESCALER_VALUE);
 * }
 * \endcode
 */
void RTC_SetPrescaler(uint32_t value);

/**
 * rtl876x_rtc.h
 *
 * \brief     Set RTC comparator value.
 *
 * \param[in] index: The comparator number to be set, please refer to COMPX_INDEX_t.
 *            \arg This parameter can be 0 ~ 3.
 * \param[in] value: The comparator value to be set. Should be no more than 24 bits!
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_COMP_INDEX          1
 * #define RTC_COMP_VALUE          (1000)
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_SetComp(RTC_COMP_INDEX, RTC_COMP_VALUE);
 * }
 * \endcode
 */
void RTC_SetComp(uint8_t index, uint32_t value);

/**
 * rtl876x_rtc.h
 *
 * \brief     Start or stop RTC peripheral.
 *
 * \param[in] NewState: New state of RTC peripheral.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Start RTC peripheral.
 *            \arg DISABLE: Stop RTC peripheral.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_rtc_init(void)
 * {
 *     //Start RTC
 *     RTC_SystemWakeupConfig(ENABLE);
 *     RTC_RunCmd(ENABLE);
 * }
 * \endcode
 */
void RTC_RunCmd(FunctionalState NewState);


/**
 * rtl876x_rtc.h
 *
 * \brief     Enable or disable interrupt signal to CPU NVIC.
 *
 * \param[in] NewState: New state of interrupt signal to CPU NVIC.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable interrupt signal to CPU NVIC.
 *            \arg DISABLE: Disable interrupt signal to CPU NVIC.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_CpuNVICEnable(ENABLE);
 * }
 * \endcode
 */
void RTC_CpuNVICEnable(FunctionalState NewState);

/**
 * rtl876x_rtc.h
 *
 * \brief     Config the specified RTC interrupts.
 *
 * \param[in] RTC_INT: Specifies the RTC interrupt source, please reference to RTC_INT_t.
 *            This parameter can be any combination of the following values:
 *            \arg RTC_INT_TICK: RTC tick interrupt source.
 *            \arg RTC_INT_OVF: RTC overflow interrupt source.
 *            \arg RTC_PRECMP_INT: RTC prescale comparator interrupt source.
 *            \arg RTC_CMPx_WK_INT: RTC comparator x wake up interrupt source, x =0~3.
 *            \arg RTC_CMPx_NV_INT(bbpro RTC_INT_CMPx): RTC comparator x interrupt source, x =0~3.
 *            \arg RTC_CMPxGT_INT: RTC comparator GT x interrupt source, x =0~3.
 *            \arg RTC_PRECMP_CMP3_INT: RTC prescale comparator 3 interrupt source.
 *
 * \param[in] NewState: new state of the specified RTC interrupt.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable the specified the RTC interrupt.
 *            \arg DISABLE: Disable the specified the RTC interrupt.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_INT_CMP_1      RTC_INT_COMP1
 * void driver_rtc_init(void)
 * {
 *     RTC_INTEnableConfig(RTC_CMP0_WK_INT, ENABLE);
 *
 *     RTC_MaskINTConfig(RTC_INT_CMP_1, DISABLE);
 *     RTC_CompINTConfig(RTC_INT_CMP_1, ENABLE);
 * }
 * \endcode
 */
void RTC_INTEnableConfig(uint32_t RTC_INT, FunctionalState NewState);

/**
 * rtl876x_rtc.h
 *
 * \brief     Mask bit remove from the bbpro2, for compatible with bbpro and bblite overflow keep this alive.
 *
 * \param[in] RTC_INT: Specifies the RTC interrupt source, please reference to RTC_INT_t.
 *            This parameter can be any combination of the following values:
 *            \arg RTC_INT_TICK: RTC tick interrupt source.
 *            \arg RTC_INT_OVF: RTC overflow interrupt source.
 *            \arg RTC_PRECMP_INT: RTC prescale comparator interrupt source.
 *            \arg RTC_CMPx_WK_INT: RTC comparator x wake up interrupt source, x =0~3.
 *            \arg RTC_CMPx_NV_INT(bbpro RTC_INT_CMPx): RTC comparator x interrupt source, x =0~3.
 *            \arg RTC_CMPxGT_INT: RTC comparator GT x interrupt source, x =0~3.
 *            \arg RTC_PRECMP_CMP3_INT: RTC prescale comparator 3 interrupt source.
 *
 * \param[in] NewState: new state of the specified RTC interrupt.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Mask the selected RTC interrupt.
 *            \arg DISABLE: Unmask the selected RTC interrupt.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_INT_CMP_1      RTC_INT_COMP1
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_MaskINTConfig(RTC_INT_CMP_1, DISABLE);
 *     RTC_CompINTConfig(RTC_INT_CMP_1, ENABLE);
 * }
 * \endcode
 */
#define RTC_EN_INTConfig     RTC_MaskINTConfig
void RTC_MaskINTConfig(uint32_t RTC_INT, FunctionalState NewState);

/**
 * rtl876x_rtc.h
 *
 * \brief     Enable or disable the specified RTC interrupts for comparator.
 *
 * \param[in] RTC_INT: Specifies the RTC interrupt source, please reference to RTC_INT_t.
 *            This parameter can be any combination of the following values:
 *            \arg RTC_INT_TICK: RTC tick interrupt source.
 *            \arg RTC_INT_OVF: RTC overflow interrupt source.
 *            \arg RTC_PRECMP_INT: RTC prescale comparator interrupt source.
 *            \arg RTC_CMPx_WK_INT: RTC comparator x wake up interrupt source, x =0~3.
 *            \arg RTC_CMPx_NV_INT(bbpro RTC_INT_CMPx): RTC comparator x interrupt source, x =0~3.
 *            \arg RTC_CMPxGT_INT: RTC comparator GT x interrupt source, x =0~3.
 *            \arg RTC_PRECMP_CMP3_INT: RTC prescale comparator 3 interrupt source.
 *
 * \param[in] NewState: new state of the specified RTC interrupt.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable the selected RTC interrupt.
 *            \arg DISABLE: Disable the selected RTC interrupt.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_INT_CMP_1      RTC_INT_COMP1
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_CompINTConfig(RTC_INT_CMP_1, ENABLE);
 * }
 * \endcode
 */
void RTC_CompINTConfig(uint32_t RTC_INT, FunctionalState NewState);

/**
 * rtl876x_rtc.h
 *
 * \brief     Enable or disable RTC tick interrupts.
 *
 * \param[in] NewState: New state of RTC tick interrupt.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable RTC tick interrupts.
 *            \arg DISABLE: Disable RTC tick interrupts.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_PRESCALER_VALUE     0
 *
 * void driver_rtc_init(void)
 * {
 *     RTC_DeInit();
 *     RTC_SetPrescaler(RTC_PRESCALER_VALUE);
 *     RTC_MaskINTConfig(RTC_INT_TICK, DISABLE);
 *     RTC_TickINTConfig(ENABLE);
 * }
 * \endcode
 */
void RTC_TickINTConfig(FunctionalState NewState);

/**
 * rtl876x_rtc.h
 *
 * \brief  Check whether the specified RTC interrupt is set or not.
 *
 * \param[in] RTC_INT: Specifies the RTC interrupt source, please reference to RTC_INT_t.
 *            This parameter can be any combination of the following values:
 *            \arg RTC_INT_TICK: RTC tick interrupt source.
 *            \arg RTC_INT_OVF: RTC overflow interrupt source.
 *            \arg RTC_PRECMP_INT: RTC prescale comparator interrupt source.
 *            \arg RTC_CMPx_WK_INT: RTC comparator x wake up interrupt source, x =0~3.
 *            \arg RTC_CMPx_NV_INT(bbpro RTC_INT_CMPx): RTC comparator x interrupt source, x =0~3.
 *            \arg RTC_CMPxGT_INT: RTC comparator GT x interrupt source, x =0~3.
 *            \arg RTC_PRECMP_CMP3_INT: RTC prescale comparator 3 interrupt source.
 *
 * \return The new state of RTC interrupt.
 * \retval SET: The specified RTC interrupt has occurred.
 * \retval RESET: The specified RTC interrupt has not occurred.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_dlps_restore(void)
 * {
 *    RTC_SystemWakeupConfig(DISABLE);
 *    RTC_RunCmd(DISABLE);
 *    if (RTC_GetINTStatus(RTC_CMP0_WK_INT) == SET)
 *    {
 *        RTC_ClearINTStatus(RTC_CMP0_WK_INT);
 *        RTC_INTEnableConfig(RTC_CMP0_WK_INT, DISABLE);
 *    }
 *    power_mode_set(POWER_ACTIVE_MODE);
 *    APP_PRINT_INFO0("RTC EXIT DLPS");
 * }
 * \endcode
 */
ITStatus RTC_GetINTStatus(uint32_t RTC_INT);

/**
 * rtl876x_rtc.h
 *
 * \brief     Enable or disable system wake up function of RTC.
 *
 * \param[in] NewState: new state of the RTC wake up function.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable system wake up function of RTC.
 *            \arg DISABLE: Disable system wake up function of RTC.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_rtc_init(void)
 * {
 *     //Start RTC
 *     RTC_SystemWakeupConfig(ENABLE);
 *     RTC_RunCmd(ENABLE);
 * }
 * \endcode
 */
void RTC_SystemWakeupConfig(FunctionalState NewState);

/* RTC internal function: Fast write RTC register. */
void RTC_WriteReg(uint32_t offset, uint32_t data);
void RTC_WriteReg_Safe(uint32_t offset, uint32_t data);


/**
 * rtl876x_rtc.h
 *
 * \brief  Clear interrupt status of RTC.
 *
 * \param[in] RTC_INT: Specifies the RTC interrupt source, please reference to RTC_INT_t.
 *            This parameter can be any combination of the following values:
 *            \arg RTC_INT_TICK: RTC tick interrupt source.
 *            \arg RTC_INT_OVF: RTC overflow interrupt source.
 *            \arg RTC_PRECMP_INT: RTC prescale comparator interrupt source.
 *            \arg RTC_CMPx_WK_INT: RTC comparator x wake up interrupt source, x =0~3.
 *            \arg RTC_CMPx_NV_INT(bbpro RTC_INT_CMPx): RTC comparator x interrupt source, x =0~3.
 *            \arg RTC_CMPxGT_INT: RTC comparator GT x interrupt source, x =0~3.
 *            \arg RTC_PRECMP_CMP3_INT: RTC prescale comparator 3 interrupt source.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_dlps_restore(void)
 * {
 *    if (RTC_GetINTStatus(RTC_CMP0_WK_INT) == SET)
 *    {
 *        RTC_ClearINTStatus(RTC_CMP0_WK_INT);
 *        RTC_INTEnableConfig(RTC_CMP0_WK_INT, DISABLE);
 *    }
 * }
 * \endcode
 */
void RTC_ClearINTStatus(uint32_t RTC_INT);

/**
 * rtl876x_rtc.h
 *
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
 *     RTC_RunCmd(ENABLE);
 * }
 * \endcode
 */
__STATIC_ALWAYS_INLINE void RTC_ResetPrescalerCounter(void)
{
    RTC_WriteReg((uint32_t)(&(RTC->CR0)), (RTC->CR0) | RTC_DIV_COUNTER_RST_EN);
    __NOP();
    __NOP();
    RTC_WriteReg((uint32_t)(&(RTC->CR0)), (RTC->CR0) & (~(RTC_DIV_COUNTER_RST_EN)));
}

/**
 * rtl876x_rtc.h
 *
 * \brief     Get counter value of RTC.
 *
 * \param[in] None.
 *
 * \return    The counter value.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_COMP_INDEX          1
 * #define RTC_COMP_VALUE          (1000)
 * #define RTC_INT_CMP_1      RTC_INT_COMP1
 *
 * void RTC_Handler(void)
 * {
 *     if (RTC_GetINTStatus(RTC_INT_CMP_1) == SET)
 *    {
 *         RTC_SetComp(RTC_COMP_INDEX, RTC_GetCounter() + RTC_COMP_VALUE);
 *         RTC_ClearCompINT(RTC_COMP_INDEX);
 *    }
 * }
 * \endcode
 */
__STATIC_ALWAYS_INLINE uint32_t RTC_GetCounter(void)
{
    return RTC->CNT;
}

/**
 * rtl876x_rtc.h
 *
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
 *     RTC_RunCmd(ENABLE);
 * }
 * \endcode
 */
__STATIC_ALWAYS_INLINE void RTC_ResetCounter(void)
{
    RTC_WriteReg((uint32_t)(&(RTC->CR0)), (RTC->CR0) | RTC_COUNTER_RST_Msk);
    __NOP();
    __NOP();
    RTC_WriteReg((uint32_t)(&(RTC->CR0)), (RTC->CR0) & (~(RTC_COUNTER_RST_EN)));
}

/**
 * rtl876x_rtc.h
 *
 * \brief     Get RTC comparator value.
 *
 * \param[in] index: The comparator number to be set, please reference to COMPX_INDEX_t.
 *            \arg This parameter can be 0 ~ 3.
 *
 * \return    The RTC comparator value.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_demo(void)
 * {
 *     uint32_t CompareValue = RTC_GetComp(0);
 * }
 * \endcode
 */
__STATIC_ALWAYS_INLINE uint32_t RTC_GetComp(uint8_t index)
{
    return *((volatile uint32_t *)(&(RTC->COMP0) + index));
}

/**
 * rtl876x_rtc.h
 *
 * \brief     Clear the interrupt pending bit of the select comparator of RTC.
 *
 * \param[in] index: The comparator number to be set, please reference to COMPX_INDEX_t.
 *            \arg This parameter can be 0 ~ 3.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_COMP_INDEX          1
 * #define RTC_INT_CMP_1      RTC_INT_COMP1
 *
 * void RTC_Handler(void)
 * {
 *     if (RTC_GetINTStatus(RTC_INT_CMP_1) == SET)
 *     {
 *         RTC_ClearCompINT(RTC_COMP_INDEX);
 *     }
 * }
 * \endcode
 */
__STATIC_ALWAYS_INLINE void RTC_ClearCompINT(uint8_t index)
{
    RTC_WriteReg((uint32_t)(&(RTC->INT_MASK)), BIT(RTC_CMP0_NV_CLR + index));
    __NOP();
    __NOP();
    RTC_WriteReg((uint32_t)(&(RTC->INT_MASK)), 0);
    RTC->INT_MASK;
}

/**
 * rtl876x_rtc.h
 *
 * \brief     Clear wakeup interrupt of the select comparator of RTC.
 *
 * \param[in] index: The comparator number to be set, please reference to COMPX_INDEX_t.
 *            \arg This parameter can be 0 ~ 3.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define RTC_COMP_INDEX          1
 * #define RTC_INT_CMP_1      RTC_INT_COMP1
 *
 * void RTC_Handler(void)
 * {
 *     if (RTC_GetINTStatus(RTC_INT_CMP_1) == SET)
 *    {
 *         //add user here.
 *         RTC_ClearCompINT(RTC_COMP_INDEX);
 *         RTC_ClearCompWkINT(RTC_COMP_INDEX);
 *    }
 * }
 * \endcode
 */
__STATIC_ALWAYS_INLINE void RTC_ClearCompWkINT(uint8_t index)
{
    RTC_WriteReg((uint32_t)(&(RTC->INT_MASK)), BIT(RTC_CMP0_WK_CLR + index));
    __NOP();
    __NOP();
    RTC_WriteReg((uint32_t)(&(RTC->INT_MASK)), 0);
    RTC->INT_MASK;
}

/**
 * rtl876x_rtc.h
 *
 * \brief     Clear the overflow interrupt pending bit of RTC.
 *
 * \param[in] None.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void RTC_Handler(void)
 * {
 *     //RTC overflow interrupt handle
 *     if (RTC_GetINTStatus(RTC_INT_OVF) == SET)
 *    {
 *         // Add application code here
 *         RTC_ClearOverFlowINT();
 *    }
 * }
 * \endcode
 */
__STATIC_ALWAYS_INLINE void RTC_ClearOverFlowINT(void)
{
    RTC_WriteReg((uint32_t)(&(RTC->INT_MASK)), BIT(RTC_CNT_OV_CLR));
    __NOP();
    __NOP();
    RTC_WriteReg((uint32_t)(&(RTC->INT_MASK)), 0);
    RTC->INT_MASK;
}

/**
 * rtl876x_rtc.h
 *
 * \brief     Clear the tick interrupt pending bit of RTC.
 *
 * \param[in] None.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void RTC_Handler(void)
 * {
 *     //RTC tick interrupt handle
 *     if (RTC_GetINTStatus(RTC_INT_TICK) == SET)
 *    {
 *         // Add application code here
 *         RTC_ClearTickINT();
 *    }
 * }
 * \endcode
 */
__STATIC_ALWAYS_INLINE void RTC_ClearTickINT(void)
{
    RTC_WriteReg((uint32_t)(&(RTC->INT_MASK)),  BIT(RTC_TICK_CLR));
    __NOP();
    __NOP();
    RTC_WriteReg((uint32_t)(&(RTC->INT_MASK)), 0);
    RTC->INT_MASK;
}

/**
 * rtl876x_rtc.h
 *
 * \brief     Config the clock of sleep mode.
 *
 * \param[in] clock: sleep clock source.
 *            This parameter can be the following values:
 *            \arg RTC_EXTERNAL_CLK: from BTAON (xtal or ring osc)
 *            \arg RTC_INTERNAL_CLK: from 32k sdm(internal 32K after calibration)
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void rtc_driver_init(void)
 * {
 *     RTC_SleepModeClkConfig(RTC_INTERNAL_CLK);
 * }
 * \endcode
 */
__STATIC_ALWAYS_INLINE void RTC_SleepModeClkConfig(uint32_t clock)
{
    /* Check the parameters */
    assert_param(IS_RTC_SLEEP_CLK(clock));

    // RTC_WriteReg(RTC_SLEEP_CR, *(volatile uint32_t *)(RTC_SLEEP_CR) & clock);
}

#ifdef __cplusplus
}
#endif

/** @} */ /* End of group RTC_Exported_Functions */
/** @} */ /* End of group 87x3d_RTC */

#endif /* _RTL876X_RTC_H_ */



/******************* (C) COPYRIGHT 2024 Realtek Semiconductor *****END OF FILE****/

