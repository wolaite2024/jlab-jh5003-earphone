/**
*****************************************************************************************
*     Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    wdg.h
  * @brief   This file provides wdg API wrapper for bbpro compatibility.
  * @author  sandy_jiang
  * @date    2024-10-11
  * @version v1.1
  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

#ifndef __WDG_H_
#define __WDG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdint.h>
#include <stdbool.h>

/** @defgroup  HAL_WDG_API    WDG API
    * @brief This file introduces the watch dog timer (WDG) APIs.
    * @{
    */

/*============================================================================*
 *                                  Types
 *============================================================================*/

/** @defgroup HAL_WDG_API_Exported_Types WDG API Exported Types
  * @{
  */

typedef enum _WDG_MODE
{
    INTERRUPT_CPU = 0,          //!< Interrupt CPU only.
    RESET_ALL_EXCEPT_AON = 1,   //!< Reset all except RTC and some AON register.
    RESET_CORE_DOMAIN = 2,      //!<  Reset core domain.
    RESET_ALL = 3               //!<  Reset all.
} T_WDG_MODE;

typedef void (*WDG_RESET_CALLBACK)(void);       //!<  WDG callback.

/** End of group HAL_WDG_API_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/

/** @defgroup HAL_WDG_API_Exported_Functions WDG API Exported Functions
 * @{
 */

/**
    * @brief  Get watch dog default mode.
    * @return Watch dog mode @ref T_WDG_MODE.
    */
T_WDG_MODE wdg_get_default_mode(void);

/**
    * @brief  Get watch dog current mode.
    * @return Watch dog mode @ref T_WDG_MODE.
    */
T_WDG_MODE wdg_get_mode(void);

/**
    * @brief  Get watch dog current timeout value.
    * @return The current timeout value, unit ms.
    */
uint32_t wdg_get_timeout_ms(void);

/**
    * @brief  Get watch dog default timeout value.
    * @return The default timeout value, unit ms.
    */
uint32_t wdg_get_default_timeout_ms(void);

/**
    * @brief  Change watch dog timeout period.
    * @param[in]  timeout_ms: New timeout value.
    */
void wdg_change_timeout_period(uint32_t timeout_ms);

/**
    * @brief  Change watch dog mode.
    * @param[in]  mode: Watch dog mode @ref T_WDG_MODE.
    */
void wdg_change_mode(T_WDG_MODE mode);

/**
    * @brief  Check if watch dog is enabled.
    * @return The result of watch dog is enabled.
    * @retval true: Watch dog is enabled.
    * @retval false: Watch dog is disabled.
    */
bool wdg_is_enable(void);

/**
    * @brief  Start watch dog with timeout period and mode.
    * @param[in]  ms: New timeout value.
    * @param[in]  wdg_mode: Watch dog mode @ref T_WDG_MODE.
    * @return The result of start watch.
    * @retval true: Set the period and mode success.
    * @retval false: Set the period and mode fail.
    */
extern bool (*wdg_start)(uint32_t ms, T_WDG_MODE  wdg_mode);
#define WDG_Start(ms, wdg_mode) wdg_start(ms, wdg_mode);    //!< Start watch dog.

/**
    * @brief  Disable watch dog.
    */
extern void (*wdg_disable)(void);
#define WDG_Disable() wdg_disable();    //!< Disable watch dog.

/**
    * @brief  Kick watch dog.
    */
extern void (*wdg_kick)(void);
#define WDG_Kick() wdg_kick();          //!< Kick watch dog.

/**
    * @brief  Reset the MCU at specified mode.
    * @param[in]  wdg_mode: Watch dog mode @ref T_WDG_MODE.
    * @param[in]  function: Function which calls the wdt_rest.
    * @param[in]  function: The line of function which calls the wdt_rest.
    */
extern void wdt_reset(T_WDG_MODE wdg_mode, const char *function, uint32_t line);
#define chip_reset(mode)    wdt_reset(mode,  __FUNCTION__, __LINE__)    //!< Reset the MCU at specified mode.

/**
    * @brief  Reset the MCU at specified mode with reset reason.
    * @param  wdg_mode: Watch dog mode @ref T_WDG_MODE.
    * @param  reset_reason: Reset reason.
    */
void wdg_reset_with_reason(T_WDG_MODE wdg_mode, uint8_t reset_reason);

/**
    * @brief  Get watch dog reset reason.
    * @return The watch dog reset reason.
    */
uint8_t wdg_get_reset_reason(void);

/**
    * @brief  Register chip reset callback.
    * @param  func: callback to be registered. \ref WDG_RESET_CALLBACK
    */
void chip_reset_register_callback(WDG_RESET_CALLBACK func);

#ifdef __cplusplus
}
#endif
/** @} */ /* End of group HAL_WDG_API_Exported_Functions */
/** @} */ /* End of group HAL_WDG_API */
#endif
