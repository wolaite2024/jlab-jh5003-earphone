/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      bt_bond_ftl_int.h
* @brief     key storage function.
* @details
* @author    jane
* @date      2016-02-18
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef     BT_BOND_FLT_INT_H
#define     BT_BOND_FLT_INT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <bt_bond_le.h>
#include "bt_bond_common.h"

#define F_BT_LE_SUPPORT 1
#define F_BT_BREDR_SUPPORT 1

typedef struct
{
    uint16_t bt_le_fs_remote_bd_offset;
    uint16_t bt_le_fs_local_ltk_offset;
    uint16_t bt_le_fs_remote_ltk_offset;
    uint16_t bt_le_fs_remote_irk_offset;
    uint16_t bt_le_link_block_size;
    uint16_t bt_le_fs_start_offset;
    uint16_t bt_common_fs_cccd_data_offset;
    uint16_t bt_stack_ftl_used_size;
    uint16_t bt_common_fs_link_block_size;
    uint16_t bt_common_fs_le_start_offset;
#if F_BT_BREDR_SUPPORT
    uint16_t bt_common_fs_bredr_start_offset;
#endif
#if F_BT_LE_5_1_CLIENT_SUPPORTED_FEATURES_SERVER_SUPPORT
    uint16_t bt_common_fs_remote_client_supp_feats_offset;
#endif
} T_BT_BOND_FTL_CB;

/* legacy link info*/
typedef enum
{
    LEGACY_BT_DEV,
    LE_BT_DEV
} T_DEV_TYPE;

typedef struct
{
    uint8_t bond_num;
    uint8_t bond_idx[19];
} T_BOND_PRIORITY;

typedef struct
{
    uint8_t key[16];
} T_LEGACY_LINK_KEY;

typedef struct
{
    uint8_t         key[16];
    uint8_t         rand[8];
    uint16_t        ediv;
    uint8_t         key_size;
    uint8_t         key_type;
} T_LE_LTK_DATA;

typedef struct
{
    uint8_t link_key_length;
    uint8_t padding[3];
    uint8_t key[28];
} T_LE_LOCAL_LTK;

typedef struct
{
    uint8_t link_key_length;
    uint8_t padding[3];
    uint8_t key[28];
} T_LE_REMOTE_LTK;

typedef struct
{
    uint8_t key[16];
    uint8_t addr[6];
    uint8_t addr_type;
    uint8_t key_exist;
} T_LE_REMOTE_IRK;

typedef struct
{
    uint8_t key[16];
    uint8_t addr[6];
    uint8_t addr_type;
} T_LE_LOCAL_IRK;

#if F_BT_LE_5_1_CLIENT_SUPPORTED_FEATURES_SERVER_SUPPORT
typedef struct
{
    uint8_t length;
    uint8_t client_supported_features[3];
} T_REMOTE_CLIENT_SUPPORTED_FEATURES;
#endif

void ftl_le_storage_init(void);
void ftl_storage_common_init(void);
uint32_t ftl_save_bond_priority(T_DEV_TYPE type, T_BOND_PRIORITY *p_data);
uint32_t ftl_load_bond_priority(T_DEV_TYPE type, T_BOND_PRIORITY *p_data);
uint32_t ftl_save_legacy_remote_bd(T_BT_BOND_INFO *p_data, uint8_t idx);
uint32_t ftl_load_legacy_remote_bd(T_BT_BOND_INFO *p_data, uint8_t idx);
uint32_t ftl_save_legacy_link_key(T_LEGACY_LINK_KEY *p_data, uint8_t idx);
uint32_t ftl_load_legacy_link_key(T_LEGACY_LINK_KEY *p_data, uint8_t idx);

uint32_t ftl_save_le_remote_bd(T_BT_BOND_INFO *p_data, uint8_t idx);
uint32_t ftl_load_le_remote_bd(T_BT_BOND_INFO *p_data, uint8_t idx);
uint32_t ftl_save_le_local_ltk(T_LE_LOCAL_LTK *p_data, uint8_t idx, uint8_t size);
uint32_t ftl_load_le_local_ltk(T_LE_LOCAL_LTK *p_data, uint8_t idx);
uint32_t ftl_save_le_remote_ltk(T_LE_REMOTE_LTK *p_data, uint8_t idx, uint8_t size);
uint32_t ftl_load_le_remote_ltk(T_LE_REMOTE_LTK *p_data, uint8_t idx);
uint32_t ftl_save_le_remote_irk(T_LE_REMOTE_IRK *p_data, uint8_t idx);
uint32_t ftl_clear_le_remote_irk(uint8_t idx);
uint32_t ftl_load_le_remote_irk(T_LE_REMOTE_IRK *p_data, uint8_t idx);
uint32_t ftl_add_cccd(T_DEV_TYPE type, uint8_t *p_data, uint8_t idx, uint8_t size,
                      uint8_t offset);
uint32_t ftl_save_cccd(T_DEV_TYPE type, T_BT_CCCD_DATA *p_data, uint8_t idx, uint8_t size);
uint32_t ftl_load_cccd(T_DEV_TYPE type, T_BT_CCCD_DATA *p_data, uint8_t idx);
uint8_t  ftl_read_cccd_length(T_DEV_TYPE type, uint8_t idx);
uint32_t imp_ftl_save(void *p_data, uint16_t start_offset, uint16_t block_size,
                      uint8_t size, uint8_t idx);
uint32_t imp_ftl_load(void *p_data, uint16_t start_offset, uint16_t block_size,
                      uint8_t size, uint8_t idx);

#if F_BT_LE_5_1_CLIENT_SUPPORTED_FEATURES_SERVER_SUPPORT
uint32_t ftl_save_remote_client_supported_features(T_DEV_TYPE type,
                                                   T_REMOTE_CLIENT_SUPPORTED_FEATURES
                                                   *p_data, uint8_t idx, uint8_t size);

uint32_t ftl_load_remote_client_supported_features(T_DEV_TYPE type,
                                                   T_REMOTE_CLIENT_SUPPORTED_FEATURES
                                                   *p_data, uint8_t idx);
#endif


#ifdef __cplusplus
}
#endif

#endif /* FLASH_STACK_STORAGE_INT_H */
