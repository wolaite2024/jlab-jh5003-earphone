/**
 * @file   app_teams_rws.h
 * @brief  struct and interface about teams rws for app
 * @author leon
 * @date   2021.4.6
 * @version 1.0
 * @par Copyright (c):
         Realsil Semiconductor Corporation. All rights reserved.
 */


#ifndef _APP_TEAMS_RWS_H_
#define _APP_TEAMS_RWS_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if F_APP_TEAMS_FEATURE_SUPPORT
/** @defgroup RWS_APP RWS APP
  * @brief Provides rws app interfaces.
  * @{
  */

/** @defgroup APP_TEAMS_RWS APP TEAMS RWS module init
  * @brief Provides teams rws profile interfaces.
  * @{
  */

typedef enum
{
    TEAMS_APP_RWS_ASP_MSG             = 0x00,
    TEAMS_APP_RWS_ASP_MAX_TYPE        = 0x01,
} T_TEAMS_APP_ASP_RWS_REMOTE_MSG_TYPE;

typedef enum
{
    TEAMS_APP_RWS_DEV_INFO_MSG        = 0x00,
    TEAMS_APP_RWS_BT_POLICY_MAX_TYPE  = 0x01,
} T_TEAMS_APP_BT_POLICY_RWS_REMOTE_MSG_TYPE;

typedef enum
{
    TEAMS_APP_RWS_BLE_ADV_MSG         = 0x00,
    TEAMS_APP_RWS_BLE_DEV_BOND_MSG    = 0x01,
    TEAMS_APP_RWS_BLE_POLICY_MAX_TYPE = 0x02,
} T_TEAMS_APP_BLE_POLICY_RWS_REMOTE_MSG_TYPE;

/**
 *  @brief function teams rws module init
 *  @note  task init call this function, the rfcomm channel would init completly
 */
void app_teams_rws_init(void);

void app_teams_rws_handle_role_swap_success(T_REMOTE_SESSION_ROLE device_role);

void app_teams_rws_handle_role_swap_failed(T_REMOTE_SESSION_ROLE device_role);

/** End of APP_TEAMS_RWS
* @}
*/

/** End of RWS_APP
* @}
*/
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_TEAMS_RWS_H_ */
