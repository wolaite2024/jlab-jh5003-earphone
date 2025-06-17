/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_XIAOWEI_DEVICE_H_
#define _APP_XIAOWEI_DEVICE_H_

#include "app_flags.h"
#if F_APP_XIAOWEI_FEATURE_SUPPORT
#include "btm.h"
#include "app_link_util.h"
#include "app_xiaowei_transport.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_RWS_XIAOWEI App Xiaowei
  * @brief App XIAOWEI
  * @{
  */

/** @defgroup APP_XIAOWEI_DEV App xiaowei device
  * @brief App xiaowei device
  * @{
  */
typedef enum
{
    XIAOWEI_VA_STATE_IDLE           = 0,
    XIAOWEI_VA_STATE_ACTIVE_PENDING = 1,
    XIAOWEI_VA_STATE_ACTIVE         = 2,
    XIAOWEI_VA_STATE_CANCEL_PENDING = 3,
    XIAOWEI_VA_STATE_INVALID        = 4
} T_XIAOWEI_VA_STATE;

typedef struct APP_XIAOWEI_DEVICE
{
    uint8_t             bd_addr[6];
    bool                bt_connected[XIAOWEI_CONNECTION_TYPE_NUM];// 0:unknown,1:ble,2:spp,3:iap
    T_XIAOWEI_VA_STATE          xiaowei_va_state;
    T_XIAOWEI_TRANSPORT  *p_transport;
} APP_XIAOWEI_DEVICE;

void app_xiaowei_handle_power_off(void);
void app_xiaowei_handle_power_on(bool is_pairing);
void app_xiaowei_handle_enter_pair_mode(void);
void app_xiaowei_handle_engage_role_decided(T_BT_DEVICE_MODE radio_mode);
void app_xiaowei_handle_role_swap_success(T_REMOTE_SESSION_ROLE device_role);
void app_xiaowei_handle_role_swap_fail(T_REMOTE_SESSION_ROLE device_role);
void app_xiaowei_handle_remote_conn_cmpl(void);
void app_xiaowei_handle_remote_disconn_cmpl(void);
void app_xiaowei_handle_b2s_bt_disconnected(uint8_t *bd_addr);
void app_xiaowei_handle_b2s_ble_connected(uint8_t *bd_addr, uint8_t conn_id);
void app_xiaowei_handle_b2s_ble_disconnected(uint8_t *bd_addr, uint8_t conn_id,
                                             uint8_t local_disc_cause, uint16_t disc_cause);

bool app_xiaowei_device_set_va_state(uint8_t *bd_addr, T_XIAOWEI_VA_STATE va_state);
T_XIAOWEI_VA_STATE app_xiaowei_device_get_va_state(uint8_t *bd_addr);
bool app_xiaowei_device_set_mtu(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                                uint16_t mtu);
bool app_xiaowei_device_get_mtu(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                                uint16_t *mtu);
T_XIAOWEI_CONNECTION_TYPE app_xiaowei_device_get_active_comm_way(void);
bool app_xiaowei_device_va_start(uint8_t *bd_addr, bool from_local,
                                 bool accept_start_speech_from_remote);
void app_xiaowei_device_va_stop(uint8_t *bd_addr);
APP_XIAOWEI_DEVICE *app_xiaowei_device_create(T_XIAOWEI_CONNECTION_TYPE bt_type, uint8_t *bd_addr,
                                              uint8_t conn_id);

bool app_xiaowei_device_delete(T_XIAOWEI_CONNECTION_TYPE bt_type, uint8_t *bd_addr,
                               uint8_t conn_id);
void app_xiaowei_device_secondary_delete_all(void);
void app_xiaowei_device_secondary_delete_ble_all(void);

void app_xiaowei_device_init(void);

void app_xiaowei_device_send_roleswap_info(void);
bool app_xiaowei_device_get_roleswap_is_adv_on(void);
uint8_t app_xiaowei_device_get_authenticated_device_count(void);
void app_xiaowei_device_start_adv_timer(uint16_t timeout_sec);

void xiaowei_dev_send_fvalue_info_to_sec(void);

/** End of APP_XIAOWEI_DEV
* @}
*/

/** End of APP_RWS_XIAOWEI
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
#endif
