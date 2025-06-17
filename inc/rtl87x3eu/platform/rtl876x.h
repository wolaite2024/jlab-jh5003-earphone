
/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    rtl876x.h
  * @brief   CMSIS Cortex-M4 Peripheral Access Layer Header File for
  *          RTL876X from Realtek Semiconductor.
  * @date    2016.3.3
  * @version v1.0

 * @date     3. March 2015
 *
 * @note     Generated with SVDConv Vx.xxp
 *           from CMSIS SVD File 'RTL876X.xml' Version x.xC,
 *
 * @par      Copyright (c) 2015 Realtek Semiconductor. All Rights Reserved.
 *
 *           The information contained herein is property of Realtek Semiconductor.
 *           Terms and conditions of usage are described in detail in Realtek
 *           SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 *           Licensees are granted free, non-transferable use of the information. NO
 *           WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 *           the file.
 *
 *

  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

#ifndef RTL876X_H
#define RTL876X_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                               Header Files
*============================================================================*/
#include "cpu_setting.h"                      /* Processor and core peripherals */
#include "pcc_reg.h"
#include "platform_reg.h"
#include "io.h"
#include "address_map.h"
#include "bitops.h"
#include "pin_def.h"
#include "rtl876x_tim.h"

/************************ For lowerstack-dev ************************/
#define LOW_STACK_BB_BASE_ADDR                BLUEWIZ_BASE
#define LOW_STACK_BZDMA_REG_BASE              BT_VENDOR_REG_BASE
#define TIMER_A0_REG_BASE                     TIMER0_BASE
#define BTMAC_VECTORn                         IRQn_TO_VECTORn(BTMAC_IRQn)
#define BTMAC_WRAP_AROUND_VECTORn             IRQn_TO_VECTORn(BTMAC_WRAP_AROUND_IRQn)
#define GDMA0_Channel1_VECTORn                IRQn_TO_VECTORn(GDMA0_Channel1_IRQn)
#define GDMA0_Channel2_VECTORn                IRQn_TO_VECTORn(GDMA0_Channel2_IRQn)
#define UART0_VECTORn                         IRQn_TO_VECTORn(UART0_IRQn)
#define TIM0_REG_BASE                         TIMER0_BASE
#define __STATIC_ALWAYS_INLINE                __STATIC_FORCEINLINE
#define barrier()                             __COMPILER_BARRIER()
#define TIMA_CH0                              TIM0_CH0
#define TIMA_CH1                              TIM0_CH1
#define TIM_ClearINT(TIMx)                    TIM_ClearInterrupt(TIMx, TIM_INT_TIMEOUT)
#define TIM_INTConfig(TIMx, STATE)            TIM_InterruptConfig(TIMx, TIM_INT_TIMEOUT, STATE)
#define TIM_GetINTStatus(TIMx)                TIM_GetInterruptStatus(TIMx, TIM_INT_TIMEOUT)
#define TIM_HwClock(STATE)                    TIM_HardwareClock(TIM0_CH0, STATE)

/* Uncomment the line below to expanse the "assert_param" macro in the
   Standard Peripheral Library drivers code */
//#define USE_FULL_ASSERT


/** @} */ /* End of group RTL876X_Exported_Macros */


/*============================================================================*
  *                                Functions
 *============================================================================*/
/** @defgroup RTL876X_Exported_Functions RTL876X Sets Exported Functions
    * @brief
    * @{
    */
#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function which reports
  *         the name of the source file and the source line number of the call
  *         that failed. If expr is true, it returns no value.
  * @retval None
  */
#define assert_param(expr) ((expr) ? (void)0 : io_assert_failed((uint8_t *)__FILE__, __LINE__))
void io_assert_failed(uint8_t *file, uint32_t line);
#else
#define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */


/** @} */ /* End of RTL876X_Exported_Functions */


/** @} */ /* End of group RTL876X */

#ifdef __cplusplus
}
#endif
#endif  /* RTL876X_H */

