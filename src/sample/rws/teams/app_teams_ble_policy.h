/**
 * @file   app_teams_ble_policy.h
 * @brief  struct and interface about teams rws for app ble
 * @author leon
 * @date   2022.6.2
 * @version 1.0
 * @par Copyright (c):
         Realsil Semiconductor Corporation. All rights reserved.
 */


#ifndef _APP_TEAMS_BLE_POLICY_H_
#define _APP_TEAMS_BLE_POLICY_H_
#include "ble_ext_adv.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if F_APP_TEAMS_BT_POLICY
/** @defgroup RWS_APP RWS APP
  * @brief Provides rws app interfaces.
  * @{
  */

/** @defgroup APP_TEAMS_BT_POLICY APP TEAMS BT POLICY module init
  * @brief Provides teams audio profile interfaces.
  * @{
  */

#define APP_TEAMS_BLE_POLICY_ADV_DURATION (60*1000)
#define APP_TEAMS_BLE_POLICY_ADV_PERIOD (120*1000)
#define APP_TEAMS_BLE_BOND_MAX_NUM    (8)
#define APP_TEAMS_BLE_POLICY_BOND_INFO_NAME_MAX_LEN (40)

extern uint8_t teams_bas_id;

typedef enum
{
    APP_TEAMS_BLE_POLICY_TIMER_ADV_PERIOD_TIMEOUT
} T_APP_TEAMS_BLE_POLICY_TIMER;

typedef enum
{
    APP_TEAMS_BLE_DEVICE_DEFAILT,
    APP_TEAMS_BLE_DEVICE_DONGLE
} T_APP_TEAMS_BLE_DEVICE_TYPE;

typedef enum
{
    APP_TEAMS_BLE_POLICY_SET_DEVICE_TYPE,
    APP_TEAMS_BLE_POLICY_SET_DEVICE_NAME
} T_APP_TEAMS_BLE_POLICY_BOND_INFO_SET_TYPE;

typedef struct
{
    bool used;
    T_APP_TEAMS_BLE_DEVICE_TYPE device_type;
    uint8_t  bd_addr[6];
    uint8_t device_name[APP_TEAMS_BLE_POLICY_BOND_INFO_NAME_MAX_LEN];
} T_TEAMS_BLE_DEVICE_INFO;

typedef struct
{
    uint8_t  handle;
    T_BLE_EXT_ADV_MGR_STATE adv_state;
    uint8_t  bd_addr[6];
    uint8_t adv_period_timer_handle;
    uint8_t conn_id;
    uint8_t res[2];
} T_TEAMS_BLE_ADV_MGR;

typedef struct
{
    T_BLE_EXT_ADV_MGR_STATE adv_state;
    bool     adv_period_timer_start_flag;
    uint8_t  bd_addr[6];
    uint8_t conn_id;
} T_APP_TEAMS_RELAY_BLE_LINK_POLICY_INFO;  //9 bytes

typedef struct
{
    T_APP_TEAMS_RELAY_BLE_LINK_POLICY_INFO teams_ble_link_relay_info[MAX_BLE_LINK_NUM];
} T_APP_TEAMS_RELAY_BLE_POLICY_ADV_INFO;

typedef struct
{
    T_TEAMS_BLE_DEVICE_INFO teams_ble_dev_bond_relay_info[APP_TEAMS_BLE_BOND_MAX_NUM];
} T_APP_TEAMS_RELAY_BLE_POLICY_DEV_BOND_INFO;

/**
 *  @brief function teams bt module init
 *  @note  task init call this function, the rfcomm channel would init completly
 */
void app_teams_ble_policy_init(void);

T_TEAMS_BLE_ADV_MGR *app_teams_ble_policy_find_ble_adv_mgr_by_idx(uint8_t index);

bool app_teams_ble_policy_set_adv_param(uint8_t adv_handle, uint8_t *peer_address);

void app_teams_ble_policy_handle_br_link_disconn(uint8_t *bd_addr);

void app_teams_ble_policy_handle_le_link_disconn(uint8_t *bd_addr, uint8_t local_disc_cause,
                                                 uint16_t disc_cause);

void app_teams_ble_policy_handle_br_link_connected(uint8_t *bd_addr);

bool app_teams_ble_policy_adv_start(uint8_t adv_handle, uint16_t duration);

bool app_teams_ble_policy_adv_stop(uint8_t adv_handle, int8_t app_cause);

void app_teams_ble_policy_handle_battery_change(uint8_t battery_level);

void app_teams_ble_policy_build_relay_adv_info(T_APP_TEAMS_RELAY_BLE_POLICY_ADV_INFO *relay_info);

void app_teams_ble_policy_handle_adv_relay_info(T_APP_TEAMS_RELAY_BLE_POLICY_ADV_INFO *relay_info,
                                                uint16_t data_size);

void app_teams_ble_policy_build_relay_dev_bond_info(T_APP_TEAMS_RELAY_BLE_POLICY_DEV_BOND_INFO
                                                    *relay_info);

void app_teams_ble_policy_handle_dev_bond_relay_info(T_APP_TEAMS_RELAY_BLE_POLICY_DEV_BOND_INFO
                                                     *relay_info,
                                                     uint16_t data_size);

void app_teams_ble_policy_set_dev_bond_info(uint8_t *bd_addr,
                                            T_APP_TEAMS_BLE_POLICY_BOND_INFO_SET_TYPE set_type, uint8_t data_length, uint8_t *p_data);

bool app_teams_ble_policy_add_dev_bond_info(uint8_t *bd_addr,
                                            T_APP_TEAMS_BLE_DEVICE_TYPE device_type);

void app_teams_ble_policy_del_dev_bond_info(uint8_t *bd_addr);

void app_teams_ble_policy_clear_dev_bond_info(void);
/** End of APP_TEAMS_BLE_POLICY
* @}
*/

/** End of RWS_APP
* @}
*/
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_TEAMS_BLE_POLICY_H_ */
