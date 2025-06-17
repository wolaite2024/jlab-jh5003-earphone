/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      bt_bond_common_int.h
* @brief     key storage function.
* @details
* @author    jane
* @date      2016-02-18
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef     BT_BOND_COMMON_INT_H
#define     BT_BOND_COMMON_INT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "bt_bond_ftl_int.h"
#include "os_queue.h"

#define BT_BOND_PROC_BIT_SEND_BOND_FULL 0x01

typedef struct
{
    bool gap_bond_manager;
    bool gatt_cccd_storage;
    uint8_t gatt_storage_ccc_bits_count;
    uint16_t le_key_storage_flag;
    uint8_t max_le_paired_device;
    uint8_t max_legacy_paired_device;
    bool bredr_ext_info_storage;
    bool use_gatts_client_supported_features;
} T_OTP_BOND_CFG;

typedef struct
{
    T_OTP_BOND_CFG otp_bond_cfg;
    T_BT_BOND_FTL_CB bond_ftl_cb;
    T_OS_QUEUE bt_bond_cback_list;
#if F_BT_LE_SUPPORT
    T_BOND_PRIORITY bt_le_bond_sort;
#endif
#if F_BT_BREDR_SUPPORT
    T_BOND_PRIORITY bt_legacy_bond_sort;
#endif
} T_BT_BOND_CB;
extern T_BT_BOND_CB *p_bt_bond_cb;
extern uint8_t  bt_bond_proc_flag;

bool bt_bond_msg_post(uint8_t cb_type, void *p_cb_data);

bool bt_bond_get_info_from_addr(uint8_t *bd_addr, uint8_t bd_type,
                                uint8_t *local_bd_addr, uint8_t local_bd_type,
                                T_DEV_TYPE *p_type, uint8_t *p_idx);

bool bt_bond_get_cccd_data(T_DEV_TYPE type, uint8_t idx, T_BT_CCCD_DATA *p_data);
bool bt_bond_set_cccd_data(T_DEV_TYPE type, uint8_t idx, uint8_t data_len, uint8_t *p_data,
                           bool is_info);
bool bt_bond_set_cccd_pending(T_DEV_TYPE type, uint8_t idx, uint16_t handle, bool data_pending);
bool bt_bond_remove(T_DEV_TYPE type, uint8_t idx);

#if F_BT_LE_5_1_CLIENT_SUPPORTED_FEATURES_SERVER_SUPPORT
bool bt_bond_get_remote_client_supported_features(T_DEV_TYPE type, uint8_t idx,
                                                  T_REMOTE_CLIENT_SUPPORTED_FEATURES *p_data);

bool bt_bond_save_remote_client_supported_features(T_DEV_TYPE type, uint8_t idx,
                                                   T_REMOTE_CLIENT_SUPPORTED_FEATURES *p_data);
#endif


bool bt_bond_save_info_before_add_bond(uint8_t idx, uint16_t conn_handle);

#ifdef __cplusplus
}
#endif

#endif /* BT_BOND_COMMON_INT_H */
