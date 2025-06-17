/**
 * @file   app_teams_audio_policy.h
 * @brief  struct and interface about teams rws for app
 * @author leon
 * @date   2021.7.16
 * @version 1.0
 * @par Copyright (c):
         Realsil Semiconductor Corporation. All rights reserved.
 */


#ifndef _APP_TEAMS_AUDIO_POLICY_H_
#define _APP_TEAMS_AUDIO_POLICY_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if F_APP_TEAMS_FEATURE_SUPPORT
/* call held status of the phone as indicated */
typedef enum t_app_hfp_call_held_status
{
    APP_HFP_CALL_HELD_STATUS_IDLE                 = 0x00,
    APP_HFP_CALL_HELD_STATUS_HOLD_AND_ACTIVE_CALL = 0x01,
    APP_HFP_CALL_HELD_STATUS_HOLD_NO_ACTIVE_CALL  = 0x02,
} T_APP_HFP_CALL_HELD_STATUS;

/** @defgroup RWS_APP RWS APP
  * @brief Provides rws app interfaces.
  * @{
  */

/** @defgroup APP_TEAMS_AUDIO_POLICY APP TEAMS AUDIO POLICY module init
  * @brief Provides teams audio profile interfaces.
  * @{
  */

/**
 *  @brief function teams audio module init
 *  @note  task init call this function, the rfcomm channel would init completly
 */
bool app_teams_audio_language_set_by_lcid(uint8_t *lcid);

void app_teams_audio_battery_vp_play(uint8_t power_on_bat);

void app_teams_audio_disconnected_state_vp_play(uint8_t *bd_addr);

void app_teams_audio_connected_state_vp_play(uint8_t *bd_addr, uint8_t is_new_device);

void app_teams_audio_set_global_mute_status(bool mute_status);

bool app_teams_audio_get_global_mute_status(void);

void app_teams_audio_set_bt_mute_status(uint8_t index, bool mute_status);

void app_teams_audio_sync_mute_status_handle(void);

bool app_teams_audio_is_call_hold_without_active_call(uint8_t index);

/** End of APP_TEAMS_AUDIO_POLICY
* @}
*/

/** End of RWS_APP
* @}
*/
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_TEAMS_AUDIO_POLICY_H_ */
