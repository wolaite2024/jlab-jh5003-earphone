/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_XIAOAI_DEVICE_H_
#define _APP_XIAOAI_DEVICE_H_

#include "btm.h"
#include "app_link_util.h"
#include "app_xiaoai_transport.h"
#include "remote.h"

#if XM_XIAOAI_FEATURE_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_RWS_AMA App Ama
  * @brief App Ama
  * @{
  */

typedef enum
{
    VA_STATE_IDLE           = 0,
    VA_STATE_ACTIVE_PENDING = 1,
    VA_STATE_ACTIVE         = 2,
    VA_STATE_CANCEL_PENDING = 3,
    VA_STATE_INVALID        = 4
} T_VA_STATE;

typedef struct APP_XIAOAI_DEVICE
{
    uint8_t             bd_addr[6];
    bool                bt_connected[2];
    T_VA_STATE          xiaoai_va_state;
    T_XIAOAI_TRANSPORT  *p_transport;
} APP_XIAOAI_DEVICE;

void app_xiaoai_handle_power_off(void);
void app_xiaoai_handle_power_on(bool is_pairing);
void app_xiaoai_handle_enter_pair_mode(void);
void app_xiaoai_handle_engage_role_decided(T_BT_DEVICE_MODE radio_mode);
void app_xiaoai_handle_role_swap_success(T_REMOTE_SESSION_ROLE device_role);
void app_xiaoai_handle_role_swap_fail(T_REMOTE_SESSION_ROLE device_role);
void app_xiaoai_handle_remote_conn_cmpl(void);
void app_xiaoai_handle_remote_disconn_cmpl(void);
void app_xiaoai_handle_b2s_bt_connected(uint8_t *bd_addr);
void app_xiaoai_handle_b2s_bt_disconnected(uint8_t *bd_addr);
void app_xiaoai_handle_b2s_ble_connected(uint8_t *bd_addr, uint8_t conn_id);
void app_xiaoai_handle_b2s_ble_disconnected(uint8_t *bd_addr, uint8_t conn_id,
                                            uint8_t local_disc_cause, uint16_t disc_cause);

bool app_xiaoai_device_set_va_state(uint8_t *bd_addr, T_VA_STATE va_state);
T_VA_STATE app_xiaoai_device_get_va_state(uint8_t *bd_addr);
bool app_xiaoai_device_set_mtu(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                               uint16_t mtu);
bool app_xiaoai_device_get_mtu(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                               uint16_t *mtu);
T_XM_CMD_COMM_WAY_TYPE app_xiaoai_device_get_active_comm_way(void);
bool app_xiaoai_device_va_start(uint8_t *bd_addr, bool from_local,
                                bool accept_start_speech_from_remote);

APP_XIAOAI_DEVICE *app_xiaoai_device_create(T_XM_CMD_COMM_WAY_TYPE bt_type, uint8_t *bd_addr,
                                            uint8_t conn_id);
void xiaoai_dev_send_all_create_info_to_sec(void);

bool app_xiaoai_device_delete(T_XM_CMD_COMM_WAY_TYPE bt_type, uint8_t *bd_addr, uint8_t conn_id);
void app_xiaoai_device_secondary_delete_all(void);
void app_xiaoai_device_secondary_delete_ble_all(void);

void app_xiaoai_device_init(void);

bool app_xiaoai_device_get_roleswap_is_adv_on(void);
uint8_t app_xiaoai_device_get_authenticated_device_count(void);
void app_xiaoai_device_start_adv_timer(uint16_t timeout_sec);

/** End of APP_RWS_AMA
* @}
*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
#endif
