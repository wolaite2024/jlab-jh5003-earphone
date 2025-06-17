/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      led.h
* @brief     Modulize LED driver for more simple led implementation
* @details   Led module is to group led_driver to provide more simple and easy usage for LED module.
* @author    brenda_li
* @date      2016-12-15
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef __APP_TEAMS_EXTEND_LED_H_
#define __APP_TEAMS_EXTEND_LED_H_

#if F_APP_TEAMS_FEATURE_SUPPORT
/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdbool.h>
#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup MODULE_LED  LED
  * @brief modulization for sleep led module for simple application usage.
  * @{
  */

/*============================================================================*
  *                                   Types
  *============================================================================*/
/** @defgroup LED_MODULE_Exported_Types Led Module Exported Types
  * @{
  */
/**  @brief  Define led different mode */
typedef enum t_teams_led_mode
{
    LED_MODE_MUTE_ON = 0x00,
    LED_MODE_MUTE_OFF = 0x01,
    LED_MODE_HOOK_SWITCH_ON = 0x02,
    LED_MODE_HOOK_SWITCH_OFF = 0x03,
    LED_MODE_HOOK_SWITCH_FLASH = 0x04,
    LED_MODE_EXTEND_ALL_ON = 0x05,
    LED_MODE_EXTEND_ALL_OFF = 0x06,
} T_TEAMS_LED_MODE;

extern void teams_extend_led_init(void);

extern void teams_extend_led_set_mode(T_TEAMS_LED_MODE mode);

extern void teams_extend_led_breath_init(void);

extern void teams_extend_led_breath_timer_handler(void *handle);

extern void teams_extend_led_check_mode(void);

extern void teams_extend_led_stop_timer(void);

extern void teams_extend_led_control_when_power_on(void);

extern void teams_extend_led_control_when_teams_launch(void);

extern void teams_extend_led_control_when_notify(void);

extern void teams_extend_led_control_when_clear_notification(void);

extern void teams_extend_led_tone_control_when_press_teams_button(void);
/** @} */ /* End of group LED_MODULE_Exported_Functions */

/** @} */ /* End of group MODULE_LED */

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif

#endif /* __LED_H_ */

