/**
 * @file   app_teams_bt_policy.h
 * @brief  struct and interface about teams rws for app
 * @author leon
 * @date   2021.7.16
 * @version 1.0
 * @par Copyright (c):
         Realsil Semiconductor Corporation. All rights reserved.
 */


#ifndef _APP_TEAMS_BT_POLICY_H_
#define _APP_TEAMS_BT_POLICY_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if F_APP_TEAMS_BT_POLICY
#include "app_bt_policy_int.h"
/** @defgroup RWS_APP RWS APP
  * @brief Provides rws app interfaces.
  * @{
  */

/** @defgroup APP_TEAMS_BT_POLICY APP TEAMS BT POLICY module init
  * @brief Provides teams audio profile interfaces.
  * @{
  */

/**
 *  @brief function teams bt module init
 *  @note  task init call this function, the rfcomm channel would init completly
 */

#define TEAMS_MAX_BOND_INFO_NUM (8)

#define TEAMS_DEVICE_COMPUTER_MASK ((uint32_t)1<<8)
#define TEAMS_DEVICE_PHONE_MASK ((uint32_t)1<<9)

typedef enum
{
    APP_TEAMS_COD_SAVE_NONE,
    APP_TEAMS_COD_SAVE_DONGLE,
    APP_TEAMS_COD_SAVE_ALL,
} T_APP_TEAMS_CMD_SAVE_TYPE;

typedef enum
{
    T_TEAMS_DEVICE_DEFAULT_TYPE       = 0x00,
    T_TEAMS_DEVICE_PHONE_TYPE         = 0x01,
    T_TEAMS_DEVICE_COMPUTER_TYPE      = 0x02,
    T_TEAMS_DEVICE_DONGLE_TYPE        = 0x03,
} T_TEAMS_DEVICE_TYPE;

typedef struct
{
    uint8_t  bd_addr[6];
    bool    used;
    uint8_t key_type;
    uint32_t cod;
    uint8_t link_key[16];
} T_APP_DEVICE_TEMP_COD_INFO;

typedef struct
{
    uint8_t bd_addr[6];
    bool used;
    T_TEAMS_DEVICE_TYPE teams_device_type;
    uint8_t master_device_name[40];
    uint8_t master_device_name_len;
} T_APP_DEVICE_COD; //49 bytes, not larger than 64 bytes unless adjust flash size

typedef struct
{
    T_APP_DEVICE_COD cod_info[TEAMS_MAX_BOND_INFO_NUM];
} T_APP_TEAMS_RELAY_BT_POLICY_INFO;  //392 bytes

void app_teams_bt_policy_clear_all_cod_info(void);
T_TEAMS_DEVICE_TYPE app_teams_bt_policy_get_cod_type_by_addr(uint8_t *bd_addr);
bool app_teams_bt_policy_check_device_cod_info_exist_by_addr(uint8_t *bd_addr);
bool app_teams_bt_policy_check_device_cod_info_exist_by_type(T_TEAMS_DEVICE_TYPE device_type);
uint8_t *app_teams_bt_policy_get_dongle_addr(void);
void app_teams_bt_policy_build_relay_info(T_APP_TEAMS_RELAY_BT_POLICY_INFO *relay_info);
void app_teams_bt_policy_handle_relay_info(T_APP_TEAMS_RELAY_BT_POLICY_INFO *relay_info,
                                           uint16_t data_size);

bool app_teams_bt_policy_prepare_for_dedicated_enter_pairing_mode(void);

bool app_teams_bt_policy_handle_auth_link_key_info(T_BT_PARAM *param);

void app_teams_bt_policy_handle_acl_conn_ind(uint8_t *bd_addr, uint32_t cod);

void app_teams_bt_policy_handle_sdp_info(uint8_t *bd_addr, T_BT_SDP_ATTR_INFO *sdp_info);

void app_teams_bt_policy_handle_sdp_cmpl_or_stop(uint8_t *bd_addr);

void app_teams_bt_policy_delete_cod_temp_info_by_addr(uint8_t *bd_addr);

bool app_teams_bt_policy_del_cod(uint8_t *bd_addr);

void app_teams_bt_policy_handle_factory_reset(void);

bool app_teams_bt_policy_save_cod_info(void);

bool app_teams_bt_policy_load_cod_info(void);

void app_teams_bt_policy_handle_link_auth_cmpl(uint8_t *bd_addr);

bool app_teams_bt_policy_master_device_name_get_by_addr(uint8_t *bd_addr,
                                                        uint8_t *master_device_name,
                                                        uint8_t *master_device_name_len);

bool app_teams_bt_policy_get_link_state_by_device_type(T_TEAMS_DEVICE_TYPE device_type);

void app_teams_bt_policy_init(void);
/** End of APP_TEAMS_BT_POLICY
* @}
*/

/** End of RWS_APP
* @}
*/
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_TEAMS_BT_POLICY_H_ */
