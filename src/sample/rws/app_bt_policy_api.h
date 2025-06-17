/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_BT_POLICY_H_
#define _APP_BT_POLICY_H_

#include <stdint.h>
#include <stdbool.h>
#include "btm.h"
#include "app_linkback.h"
#include "app_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_BT_POLICY App BT Policy
  * @brief App BLE Device
  * @{
  */

#define IOS_VENDOR_ID   0x004c

typedef enum
{
    BP_EVENT_STATE_CHANGED             = 0x00,
    BP_EVENT_RADIO_MODE_CHANGED        = 0x10,
    BP_EVENT_ROLE_DECIDED              = 0x20,
    BP_EVENT_BUD_CONN_START            = 0x30,
    BP_EVENT_BUD_AUTH_SUC              = 0x31,
    BP_EVENT_BUD_CONN_FAIL             = 0x32,
    BP_EVENT_BUD_AUTH_FAIL             = 0x33,
    BP_EVENT_BUD_DISCONN_NORMAL        = 0x34,
    BP_EVENT_BUD_DISCONN_LOST          = 0x35,
    BP_EVENT_BUD_REMOTE_CONN_CMPL      = 0x36,
    BP_EVENT_BUD_REMOTE_DISCONN_CMPL   = 0x37,
    BP_EVENT_BUD_LINKLOST_TIMEOUT      = 0x38,
    BP_EVENT_SRC_AUTH_SUC              = 0x40,
    BP_EVENT_SRC_AUTH_FAIL             = 0x41,
    BP_EVENT_SRC_DISCONN_LOST          = 0x42,
    BP_EVENT_SRC_DISCONN_NORMAL        = 0x43,
    BP_EVENT_SRC_DISCONN_ROLESWAP      = 0x44,
    BP_EVENT_PROFILE_CONN_SUC          = 0x50,
    BP_EVENT_PROFILE_DISCONN           = 0x51,
    BP_EVENT_PAIRING_MODE_TIMEOUT      = 0x60,
    BP_EVENT_DEDICATED_PAIRING         = 0x61,
} T_BP_EVENT;

typedef struct
{
    uint8_t *bd_addr;
    bool is_first_prof;
    bool is_last_prof;
    bool is_ignore;
    bool is_first_src;
    bool is_shut_down;
    uint32_t prof;
    uint16_t cause;
    bool is_b2s_connected_vp_played;
} T_BP_EVENT_PARAM;

typedef enum
{
    BP_STATE_IDLE                = 0x00,
    BP_STATE_FIRST_ENGAGE        = 0x01,
    BP_STATE_ENGAGE              = 0x02,
    BP_STATE_LINKBACK            = 0x03,
    BP_STATE_CONNECTED           = 0x04,
    BP_STATE_PAIRING_MODE        = 0x05,
    BP_STATE_STANDBY             = 0x06,
    BP_STATE_SECONDARY           = 0x07,
    BP_STATE_PREPARE_ROLESWAP    = 0x08,
} T_BP_STATE;

typedef enum
{
    BP_TPOLL_EVENT_ACL_CONN            = 0x00,
    BP_TPOLL_EVENT_ACL_DISCONN         = 0x01,
    BP_TPOLL_EVENT_HID_CONN            = 0x10,
    BP_TPOLL_EVENT_VP_UPDATE_START     = 0x20,
    BP_TPOLL_EVENT_VP_UPDATE_STOP      = 0x21,
    BP_TPOLL_EVENT_CFU_START           = 0x22,
    BP_TPOLL_EVENT_CFU_STOP            = 0x23,
    BP_TPOLL_EVENT_DATA_CAPTURE_START  = 0x30,
    BP_TPOLL_EVENT_DATA_CAPTURE_STOP   = 0x31,
    BP_TPOLL_EVENT_DONGLE_SPP_START    = 0x40,
    BP_TPOLL_EVENT_DONGLE_SPP_STOP     = 0x41,
    BP_TPOLL_EVENT_LEA_START           = 0x50,
    BP_TPOLL_EVENT_LEA_STOP            = 0x51,
    BP_TPOLL_EVENT_SCO_START           = 0x60,
    BP_TPOLL_EVENT_SCO_STOP            = 0x61,
    BP_TPOLL_EVENT_A2DP_START          = 0x70,
    BP_TPOLL_EVENT_A2DP_STOP           = 0x71,
    BP_TPOLL_EVENT_LINKBACK_START      = 0x80,
    BP_TPOLL_EVENT_LINKBACK_STOP       = 0x81,
} T_BP_TPOLL_EVENT;

typedef enum
{
    BP_TX_POWER_B2B_CONN               = 0x00,
    BP_TX_POWER_A2DP_START             = 0x01,
    BP_TX_POWER_A2DP_STOP              = 0x02,
    BP_TX_POWER_GAMING_MODE_SWITCH     = 0x03,
} T_BP_SET_TX_POWER;

typedef enum
{
    BP_CPU_FREQ_EVENT_SET_CPU_CLK_TIMEOUT       = 0x00,
    BP_CPU_FREQ_EVENT_A2DP_STREAM               = 0x01,
    BP_CPU_FREQ_EVENT_ACL_CONN_CMPL             = 0x02,
    BP_CPU_FREQ_EVENT_SCO_CONN                  = 0x03,
    BP_CPU_FREQ_EVENT_ROLESWAP                  = 0x04,
    BP_CPU_FREQ_EVENT_BP_STATE_LINKBACK         = 0x05,
    BP_CPU_FREQ_EVENT_LE_LINK_STATE             = 0x06,
    BP_CPU_FREQ_EVENT_LEA_BCA_RELEASE           = 0x07,
    BP_CPU_FREQ_EVENT_LEA_BIG_SETUP_DATA_PATH   = 0x08,
    BP_CPU_FREQ_EVENT_LEA_BCA_BIS_SYNC          = 0x09,
    BP_CPU_FREQ_EVENT_LEA_BCA_STATE             = 0x0A,
    BP_CPU_FREQ_EVENT_LEA_UCA_LINK_STATE        = 0x0B,
    BP_CPU_FREQ_EVENT_GAMING_MMI_TRIGGER        = 0x0C,
    BP_CPU_FREQ_EVENT_GAMING_STREAMING_START    = 0x0D,
    BP_CPU_FREQ_EVENT_GAMING_STREAMING_STOP     = 0x0E,
    BP_CPU_FREQ_EVENT_GAMING_RECORD_START       = 0x0F,
    BP_CPU_FREQ_EVENT_GAMING_RECORD_STOP        = 0x10,

    // add new event
} T_BP_CPU_FREQ_EVENT;

typedef void (*T_BP_IND_FUN)(T_BP_EVENT event, T_BP_EVENT_PARAM *event_param);

void app_bt_policy_init(void);

void app_bt_policy_stop_a2dp_and_sco(void);

void app_bt_policy_startup(T_BP_IND_FUN fun, bool at_once_trigger);
void app_bt_policy_shutdown(void);
void app_bt_policy_stop(void);
void app_bt_policy_restore(void);
void app_bt_policy_prepare_for_roleswap(void);

void app_bt_policy_msg_prof_conn(uint8_t *bd_addr, uint32_t prof);
void app_bt_policy_msg_prof_disconn(uint8_t *bd_addr, uint32_t prof, uint16_t cause);

void app_bt_policy_default_connect(uint8_t *bd_addr, uint32_t plan_profs, bool check_bond_flag);
void app_bt_policy_special_connect(uint8_t *bd_addr, uint32_t plan_prof,
                                   T_LINKBACK_SEARCH_PARAM *search_param);
void app_bt_policy_disconnect(uint8_t *bd_addr, uint32_t plan_profs);
void app_bt_policy_disc_all_b2s(void);
void app_bt_policy_conn_all_b2s(void);
void app_bt_policy_disconnect_all_link(void);

void app_bt_policy_enter_pairing_mode(bool force, bool visiable);
void app_bt_policy_exit_pairing_mode(void);

void app_bt_policy_enter_dut_test_mode(void);

void app_bt_policy_start_ota_shaking(void);
void app_bt_policy_enter_ota_mode(bool connectable);

T_BP_STATE app_bt_policy_get_state(void);

T_BT_DEVICE_MODE app_bt_policy_get_radio_mode(void);

bool app_bt_policy_b2b_is_conn(void);
void app_bt_policy_sync_b2s_connected(void);

bool app_bt_policy_get_first_connect_sync_default_vol_flag(void);
void app_bt_policy_set_first_connect_sync_default_vol_flag(bool flag);
void app_bt_policy_primary_engage_action_adjust(void);
void app_bt_policy_b2s_tpoll_update(uint8_t *bd_addr, T_BP_TPOLL_EVENT event);
void app_bt_policy_stop_reconnect_timer(void);
void app_bt_policy_roleswitch_when_sco(uint8_t *bd_addr);
void app_bt_policy_b2s_conn_start_timer(void);

void app_bt_policy_abandon_engage(void);
void app_bt_policy_update_pair_idx_mapping(void);
uint8_t *app_bt_policy_get_linkback_device(void);
bool app_bt_policy_src_conn_ind_check(uint8_t *bd_addr);
void app_bt_policy_update_cpu_freq(T_BP_CPU_FREQ_EVENT req_event);
void app_bt_policy_dynamic_adjust_b2b_tx_power(T_BP_SET_TX_POWER evt);

#if F_APP_GAMING_DONGLE_SUPPORT
bool app_bt_policy_check_keep_pairing_state(uint8_t *bd_addr);
#endif

#if F_APP_LEA_SUPPORT
void app_bt_policy_set_legacy(uint8_t para);
bool app_bt_policy_is_pairing(void);
bool app_bt_policy_listening_allow_poweroff(void);
#endif

T_APP_CALL_STATUS app_bt_policy_get_call_status(void);

#if F_APP_B2B_ENGAGE_IMPROVE_BY_LEA_DONGLE
void app_bt_policy_change_to_sec_and_conn_pri(void);
#endif

/**
 * app_bt_policy_api.h
 *
 * \brief   delete bonding record and save local/remote bud coupling address.
 *
 * \param[in]   none
 *
 * @return      none
 *
 * \ingroup  APP_BT_POLICY
 */
void app_bt_policy_update_bud_coupling_info(void);
/** End of APP_BT_POLICY
* @}
*/

void app_bt_policy_roleswitch_check_by_le_link(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_BT_POLICY_H_ */
