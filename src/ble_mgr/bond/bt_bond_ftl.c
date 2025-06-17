/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      bt_bond_flash.c
* @brief     This file provides all the key storage related functions.
* @details
* @author    jane
* @date      2016-02-18
* @version   v0.1
* *********************************************************************************************************
*/
#if CONFIG_REALTEK_APP_BOND_MGR_SUPPORT
#include <string.h>
#include <ftl.h>
#include <bt_bond_ftl_int.h>
#include <bt_bond_le.h>
#include <trace.h>
#include "bt_bond_common_int.h"


#define FLASH_LOCAL_NAME_OFFSET               0
#define FLASH_LOCAL_APPEARANCE_OFFSET         0x28
#define FLASH_LOCAL_IRK_OFFSET                0x2C
#define FLASH_LOCAL_DATABASE_HASH_OFFSET      0x3C
#define FLASH_LINK_INFO_START_OFFSET          100
#define FLASH_LEGACY_BOND_PRIORITY_OFFSET     80
#define FLASH_LEGACY_LINK_INFO_BD_OFFSET      100
#define FLASH_LEGACY_LINK_INFO_LINKKEY_OFFSET (FLASH_LEGACY_LINK_INFO_BD_OFFSET + sizeof(T_BT_BOND_INFO))
#define FLASH_LEGACY_LINK_INFO_SIZE           (sizeof(T_BT_BOND_INFO)+ sizeof(T_LEGACY_LINK_KEY))

#define FLASH_ERROR_CODE_GAP_BOND_MANAGER_FALSE 0xFFFF

uint32_t imp_ftl_save(void *p_data, uint16_t start_offset, uint16_t block_size,
                      uint8_t size, uint8_t idx)
{
    uint32_t has_error;
    uint16_t offset = start_offset + idx * block_size;
    has_error = ftl_save_to_storage(p_data, offset, size);
#if F_APP_BOND_MGR_DEBUG
    if (has_error == 0)
    {
        FLASH_PRINT_TRACE3("w[%d]:size %d, data %b",
                           offset, size,
                           TRACE_BINARY(size, p_data));
    }
    else
    {
        FLASH_PRINT_ERROR3("w[%d]:size %d, failed 0x%x",
                           offset, size, has_error);
    }
#endif
    return has_error;
}

uint32_t imp_ftl_load(void *p_data, uint16_t start_offset, uint16_t block_size,
                      uint8_t size, uint8_t idx)
{
    uint16_t offset = start_offset + idx * block_size;
    uint32_t has_error =  ftl_load_from_storage(p_data, offset, size);
    if (has_error) { memset(p_data, 0, size); }
    if (has_error)
    {
#if F_APP_BOND_MGR_DEBUG
        FLASH_PRINT_ERROR3("r[%d]:size %d, failed 0x%x",
                           offset, size, has_error);
#endif
        memset(p_data, 0, size);
    }
    else
    {
#if F_APP_BOND_MGR_DEBUG
        FLASH_PRINT_TRACE3("r[%d]:size %d, data %b",
                           offset, size,
                           TRACE_BINARY(size, p_data));
#endif
    }
    return has_error;
}

uint32_t ftl_save_bond_priority(T_DEV_TYPE type, T_BOND_PRIORITY *p_data)
{
    uint16_t offset;
    BTM_PRINT_TRACE1("[BTBond] ftl_save_bond_priority: type %d", type);
    if (type == LE_BT_DEV)
    {
        offset = p_bt_bond_cb->bond_ftl_cb.bt_le_fs_start_offset;
    }
    else
    {
        offset = FLASH_LEGACY_BOND_PRIORITY_OFFSET;
    }
    return imp_ftl_save(p_data, offset, 0, sizeof(T_BOND_PRIORITY), 0);
}

uint32_t ftl_load_bond_priority(T_DEV_TYPE type, T_BOND_PRIORITY *p_data)
{
    uint16_t offset;

    if (type == LE_BT_DEV)
    {
        offset = p_bt_bond_cb->bond_ftl_cb.bt_le_fs_start_offset;
    }
    else
    {
        offset = FLASH_LEGACY_BOND_PRIORITY_OFFSET;
    }
    return imp_ftl_load(p_data, offset, 0, sizeof(T_BOND_PRIORITY), 0);
}

bool ftl_get_common_block_offset(T_DEV_TYPE type, uint8_t idx, uint16_t *p_offset)
{
    uint16_t offset = 0;

    if (type == LE_BT_DEV)
    {
        if (idx >= p_bt_bond_cb->otp_bond_cfg.max_le_paired_device)
        {
            return false;
        }
        offset = p_bt_bond_cb->bond_ftl_cb.bt_common_fs_le_start_offset;
    }
#if F_BT_BREDR_SUPPORT
    if (type == LEGACY_BT_DEV)
    {
        if (idx >= p_bt_bond_cb->otp_bond_cfg.max_legacy_paired_device)
        {
            return false;
        }
        offset = p_bt_bond_cb->bond_ftl_cb.bt_common_fs_bredr_start_offset;
    }
#endif
    if (offset != 0)
    {
        *p_offset = offset;
        return true;
    }
    return false;
}

#if F_BT_LE_SUPPORT
uint32_t ftl_save_le_remote_bd(T_BT_BOND_INFO *p_data, uint8_t idx)
{
    if (p_bt_bond_cb == NULL || idx >= p_bt_bond_cb->otp_bond_cfg.max_le_paired_device)
    {
        return 1;
    }
    BTM_PRINT_TRACE6("[BTBond] ftl_save_le_remote_bd: idx %d, addr %s, remote_bd_type %d, bond_flag 0x%02x, local_bd %s, local_bd_type %d",
                     idx, TRACE_BDADDR(p_data->remote_bd), p_data->remote_bd_type, p_data->bond_flag,
                     TRACE_BDADDR(p_data->local_bd), p_data->local_bd_type);
    return imp_ftl_save(p_data, (p_bt_bond_cb->bond_ftl_cb.bt_le_fs_start_offset +
                                 p_bt_bond_cb->bond_ftl_cb.bt_le_fs_remote_bd_offset),
                        p_bt_bond_cb->bond_ftl_cb.bt_le_link_block_size,
                        sizeof(T_BT_BOND_INFO), idx);
}

uint32_t ftl_load_le_remote_bd(T_BT_BOND_INFO *p_data, uint8_t idx)
{
    if (p_bt_bond_cb == NULL || idx >= p_bt_bond_cb->otp_bond_cfg.max_le_paired_device)
    {
        return 1;
    }

    return imp_ftl_load(p_data, (p_bt_bond_cb->bond_ftl_cb.bt_le_fs_start_offset +
                                 p_bt_bond_cb->bond_ftl_cb.bt_le_fs_remote_bd_offset),
                        p_bt_bond_cb->bond_ftl_cb.bt_le_link_block_size,
                        sizeof(T_BT_BOND_INFO), idx);
}

uint32_t ftl_save_le_local_ltk(T_LE_LOCAL_LTK *p_data, uint8_t idx, uint8_t size)
{
    if (p_bt_bond_cb == NULL ||
        (p_bt_bond_cb->otp_bond_cfg.le_key_storage_flag & LE_KEY_STORE_LOCAL_LTK_BIT) == 0)
    {
        return 2;
    }
    if (idx >= p_bt_bond_cb->otp_bond_cfg.max_le_paired_device)
    {
        return 1;
    }
    BTM_PRINT_TRACE1("[BTBond] ftl_save_le_local_ltk: idx %d", idx);
    return imp_ftl_save(p_data, (p_bt_bond_cb->bond_ftl_cb.bt_le_fs_start_offset +
                                 p_bt_bond_cb->bond_ftl_cb.bt_le_fs_local_ltk_offset),
                        p_bt_bond_cb->bond_ftl_cb.bt_le_link_block_size,
                        size, idx);
}

uint32_t ftl_load_le_local_ltk(T_LE_LOCAL_LTK *p_data, uint8_t idx)
{
    if (p_bt_bond_cb == NULL ||
        (p_bt_bond_cb->otp_bond_cfg.le_key_storage_flag & LE_KEY_STORE_LOCAL_LTK_BIT) == 0)
    {
        return 2;
    }
    if (idx >= p_bt_bond_cb->otp_bond_cfg.max_le_paired_device)
    {
        return 1;
    }
    BTM_PRINT_TRACE1("[BTBond] ftl_load_le_local_ltk: idx %d", idx);

    return imp_ftl_load(p_data, (p_bt_bond_cb->bond_ftl_cb.bt_le_fs_start_offset +
                                 p_bt_bond_cb->bond_ftl_cb.bt_le_fs_local_ltk_offset),
                        p_bt_bond_cb->bond_ftl_cb.bt_le_link_block_size,
                        sizeof(T_LE_LOCAL_LTK), idx);
}

uint32_t ftl_save_le_remote_ltk(T_LE_REMOTE_LTK *p_data, uint8_t idx, uint8_t size)
{
    if (p_bt_bond_cb == NULL ||
        (p_bt_bond_cb->otp_bond_cfg.le_key_storage_flag & LE_KEY_STORE_REMOTE_LTK_BIT) == 0)
    {
        return 2;
    }
    if (idx >= p_bt_bond_cb->otp_bond_cfg.max_le_paired_device)
    {
        return 1;
    }
    BTM_PRINT_TRACE1("[BTBond] ftl_save_le_remote_ltk: idx %d", idx);

    return imp_ftl_save(p_data, (p_bt_bond_cb->bond_ftl_cb.bt_le_fs_start_offset +
                                 p_bt_bond_cb->bond_ftl_cb.bt_le_fs_remote_ltk_offset),
                        p_bt_bond_cb->bond_ftl_cb.bt_le_link_block_size,
                        size, idx);
}

uint32_t ftl_load_le_remote_ltk(T_LE_REMOTE_LTK *p_data, uint8_t idx)
{
    if (p_bt_bond_cb == NULL ||
        (p_bt_bond_cb->otp_bond_cfg.le_key_storage_flag & LE_KEY_STORE_REMOTE_LTK_BIT) == 0)
    {
        return 2;
    }
    if (idx >= p_bt_bond_cb->otp_bond_cfg.max_le_paired_device)
    {
        return 1;
    }
    BTM_PRINT_TRACE1("[BTBond] ftl_load_le_remote_ltk: idx %d", idx);
    return imp_ftl_load(p_data, (p_bt_bond_cb->bond_ftl_cb.bt_le_fs_start_offset +
                                 p_bt_bond_cb->bond_ftl_cb.bt_le_fs_remote_ltk_offset),
                        p_bt_bond_cb->bond_ftl_cb.bt_le_link_block_size,
                        sizeof(T_LE_REMOTE_LTK), idx);
}

uint32_t ftl_save_le_remote_irk(T_LE_REMOTE_IRK *p_data, uint8_t idx)
{
    if (p_bt_bond_cb == NULL ||
        (p_bt_bond_cb->otp_bond_cfg.le_key_storage_flag & LE_KEY_STORE_REMOTE_IRK_BIT) == 0)
    {
        return 2;
    }
    if (idx >= p_bt_bond_cb->otp_bond_cfg.max_le_paired_device)
    {
        return 1;
    }
    BTM_PRINT_TRACE1("[BTBond] ftl_save_le_remote_irk: idx %d", idx);

    return imp_ftl_save(p_data, (p_bt_bond_cb->bond_ftl_cb.bt_le_fs_start_offset +
                                 p_bt_bond_cb->bond_ftl_cb.bt_le_fs_remote_irk_offset),
                        p_bt_bond_cb->bond_ftl_cb.bt_le_link_block_size,
                        sizeof(T_LE_REMOTE_IRK), idx);
}

uint32_t ftl_clear_le_remote_irk(uint8_t idx)
{
    uint8_t data[4] = {0, 0, 0, 0};

    if (p_bt_bond_cb == NULL ||
        (p_bt_bond_cb->otp_bond_cfg.le_key_storage_flag & LE_KEY_STORE_REMOTE_IRK_BIT) == 0)
    {
        return 2;
    }
    if (idx >= p_bt_bond_cb->otp_bond_cfg.max_le_paired_device)
    {
        return 1;
    }
    BTM_PRINT_TRACE1("[BTBond] ftl_clear_le_remote_irk: idx %d", idx);

    return imp_ftl_save(data, (p_bt_bond_cb->bond_ftl_cb.bt_le_fs_start_offset +
                               p_bt_bond_cb->bond_ftl_cb.bt_le_fs_remote_irk_offset + 20),
                        p_bt_bond_cb->bond_ftl_cb.bt_le_link_block_size,
                        4, idx);
}

uint32_t ftl_load_le_remote_irk(T_LE_REMOTE_IRK *p_data, uint8_t idx)
{
    if (p_bt_bond_cb == NULL ||
        (p_bt_bond_cb->otp_bond_cfg.le_key_storage_flag & LE_KEY_STORE_REMOTE_IRK_BIT) == 0)
    {
        return 2;
    }
    if (idx >= p_bt_bond_cb->otp_bond_cfg.max_le_paired_device)
    {
        return 1;
    }
    BTM_PRINT_TRACE1("[BTBond] ftl_load_le_remote_irk: idx %d", idx);

    return imp_ftl_load(p_data, (p_bt_bond_cb->bond_ftl_cb.bt_le_fs_start_offset +
                                 p_bt_bond_cb->bond_ftl_cb.bt_le_fs_remote_irk_offset),
                        p_bt_bond_cb->bond_ftl_cb.bt_le_link_block_size,
                        sizeof(T_LE_REMOTE_IRK), idx);
}

#if F_BT_LE_5_1_CLIENT_SUPPORTED_FEATURES_SERVER_SUPPORT
uint32_t ftl_save_remote_client_supported_features(T_DEV_TYPE type,
                                                   T_REMOTE_CLIENT_SUPPORTED_FEATURES
                                                   *p_data, uint8_t idx, uint8_t size)
{
    uint16_t start_offset;
    if ((p_bt_bond_cb->otp_bond_cfg.use_gatts_client_supported_features) == 0)
    {
        return 2;
    }

    if (ftl_get_common_block_offset(type, idx, &start_offset) == false)
    {
        return 1;
    }

    BTM_PRINT_TRACE2("[BTBond] ftl_save_remote_client_supported_features: type %d, idx %d", type, idx);
    return imp_ftl_save(p_data, (start_offset +
                                 p_bt_bond_cb->bond_ftl_cb.bt_common_fs_remote_client_supp_feats_offset),
                        p_bt_bond_cb->bond_ftl_cb.bt_common_fs_link_block_size,
                        size, idx);
}

uint32_t ftl_load_remote_client_supported_features(T_DEV_TYPE type,
                                                   T_REMOTE_CLIENT_SUPPORTED_FEATURES
                                                   *p_data, uint8_t idx)
{
    uint16_t start_offset;
    if ((p_bt_bond_cb->otp_bond_cfg.use_gatts_client_supported_features) == 0)
    {
        return 2;
    }
    if (ftl_get_common_block_offset(type, idx, &start_offset) == false)
    {
        return 1;
    }
    BTM_PRINT_TRACE1("[BTBond] ftl_load_remote_client_supported_features: idx %d", idx);

    return imp_ftl_load(p_data, (start_offset +
                                 p_bt_bond_cb->bond_ftl_cb.bt_common_fs_remote_client_supp_feats_offset),
                        p_bt_bond_cb->bond_ftl_cb.bt_common_fs_link_block_size,
                        sizeof(T_REMOTE_CLIENT_SUPPORTED_FEATURES), idx);
}
#endif

uint32_t ftl_save_cccd(T_DEV_TYPE type, T_BT_CCCD_DATA *p_data, uint8_t idx, uint8_t size)
{
    uint16_t start_offset;
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gatt_cccd_storage == 0)
    {
        return 2;
    }
    if (ftl_get_common_block_offset(type, idx, &start_offset) == false)
    {
        return 1;
    }
    BTM_PRINT_TRACE2("[BTBond] ftl_save_cccd: idx %d, size %d", idx, size);

    return imp_ftl_save(p_data, (start_offset +
                                 p_bt_bond_cb->bond_ftl_cb.bt_common_fs_cccd_data_offset),
                        p_bt_bond_cb->bond_ftl_cb.bt_common_fs_link_block_size,
                        size, idx);
}

uint32_t ftl_add_cccd(T_DEV_TYPE type, uint8_t *p_data, uint8_t idx, uint8_t size,
                      uint8_t offset)
{
    uint16_t start_offset;
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gatt_cccd_storage == 0)
    {
        return 2;
    }
    if (ftl_get_common_block_offset(type, idx, &start_offset) == false)
    {
        return 1;
    }
    BTM_PRINT_TRACE3("[BTBond] ftl_add_cccd: idx %d, size %d, offset %d", idx, size, offset);

    return imp_ftl_save(p_data, (start_offset + p_bt_bond_cb->bond_ftl_cb.bt_common_fs_cccd_data_offset
                                 + offset),
                        p_bt_bond_cb->bond_ftl_cb.bt_common_fs_link_block_size,
                        size, idx);
}

uint32_t ftl_load_cccd(T_DEV_TYPE type, T_BT_CCCD_DATA *p_data, uint8_t idx)
{
    if (p_bt_bond_cb == NULL)
    {
        return 3;
    }
    uint16_t start_offset;
    uint8_t max_len = 4 + 4 * p_bt_bond_cb->otp_bond_cfg.gatt_storage_ccc_bits_count;
    uint8_t length;

    if (p_bt_bond_cb->otp_bond_cfg.gatt_cccd_storage == 0)
    {
        return 2;
    }
    if (ftl_get_common_block_offset(type, idx, &start_offset) == false)
    {
        return 1;
    }

    length = ftl_read_cccd_length(type, idx);
    length += 4;
    if ((length == 4) || (length > max_len))
    {
        p_data->data_length = 0;
        return 2;
    }

    return imp_ftl_load(p_data, (start_offset +
                                 p_bt_bond_cb->bond_ftl_cb.bt_common_fs_cccd_data_offset),
                        p_bt_bond_cb->bond_ftl_cb.bt_common_fs_link_block_size,
                        length, idx);
}

uint8_t ftl_read_cccd_length(T_DEV_TYPE type, uint8_t idx)
{
    uint16_t start_offset;
    uint8_t data[4] = {0};
    uint32_t ret = 0;
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gatt_cccd_storage == 0)
    {
        ret = 2;
    }
    if (ftl_get_common_block_offset(type, idx, &start_offset) == false)
    {
        ret = 1;
    }
    if (ret == 0)
    {
        ret = imp_ftl_load(data, (start_offset + p_bt_bond_cb->bond_ftl_cb.bt_common_fs_cccd_data_offset),
                           p_bt_bond_cb->bond_ftl_cb.bt_common_fs_link_block_size,
                           4, idx);
    }
    BTM_PRINT_TRACE3("[BTBond] ftl_read_cccd_length: ret %d, idx %d, length %d", ret, idx, data[0]);
    if (ret == 0)
    {
        return data[0];
    }
    else
    {
        return 0;
    }
}
#endif

void ftl_le_storage_init(void)
{
#if F_BT_LE_SUPPORT
    if (p_bt_bond_cb == NULL)
    {
        return;
    }
    uint16_t offset = 0;
    offset += sizeof(T_BOND_PRIORITY);
    p_bt_bond_cb->bond_ftl_cb.bt_le_fs_remote_bd_offset = offset;
    offset += sizeof(T_BT_BOND_INFO);
    if (p_bt_bond_cb->otp_bond_cfg.le_key_storage_flag & LE_KEY_STORE_LOCAL_LTK_BIT)
    {
        p_bt_bond_cb->bond_ftl_cb.bt_le_fs_local_ltk_offset = offset;
        offset += sizeof(T_LE_LOCAL_LTK);
    }
    if (p_bt_bond_cb->otp_bond_cfg.le_key_storage_flag & LE_KEY_STORE_REMOTE_LTK_BIT)
    {
        p_bt_bond_cb->bond_ftl_cb.bt_le_fs_remote_ltk_offset = offset;
        offset += sizeof(T_LE_REMOTE_LTK);
    }
    if (p_bt_bond_cb->otp_bond_cfg.le_key_storage_flag & LE_KEY_STORE_REMOTE_IRK_BIT)
    {
        p_bt_bond_cb->bond_ftl_cb.bt_le_fs_remote_irk_offset = offset;
        offset += sizeof(T_LE_REMOTE_IRK);
    }
    p_bt_bond_cb->bond_ftl_cb.bt_le_link_block_size = offset - sizeof(T_BOND_PRIORITY);
#endif
#if F_BT_BREDR_SUPPORT
    p_bt_bond_cb->bond_ftl_cb.bt_le_fs_start_offset = FLASH_LINK_INFO_START_OFFSET +
                                                      p_bt_bond_cb->otp_bond_cfg.max_legacy_paired_device *
                                                      FLASH_LEGACY_LINK_INFO_SIZE;
#else
    p_bt_bond_cb->bond_ftl_cb.bt_le_fs_start_offset = FLASH_LINK_INFO_START_OFFSET;
#endif

#if F_BT_LE_SUPPORT
    p_bt_bond_cb->bond_ftl_cb.bt_stack_ftl_used_size = p_bt_bond_cb->bond_ftl_cb.bt_le_fs_start_offset +
                                                       p_bt_bond_cb->bond_ftl_cb.bt_le_link_block_size *
                                                       p_bt_bond_cb->otp_bond_cfg.max_le_paired_device
                                                       +
                                                       sizeof(T_BOND_PRIORITY);
#else
    p_bt_bond_cb->bond_ftl_cb.bt_stack_ftl_used_size = FLASH_LINK_INFO_START_OFFSET +
                                                       p_bt_bond_cb->otp_bond_cfg.max_legacy_paired_device *
                                                       FLASH_LEGACY_LINK_INFO_SIZE;
#endif

    BTM_PRINT_TRACE2("[BTBond] ftl_le_storage_init: otp_bond_cfg %b, bond_ftl_cb %b",
                     TRACE_BINARY(sizeof(T_OTP_BOND_CFG), &p_bt_bond_cb->otp_bond_cfg),
                     TRACE_BINARY(sizeof(T_BT_BOND_FTL_CB), &p_bt_bond_cb->bond_ftl_cb));
}

void ftl_storage_common_init(void)
{
    if (p_bt_bond_cb == NULL)
    {
        return;
    }
    if (p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return;
    }

    uint16_t offset = 0;

#if F_BT_LE_5_1_CLIENT_SUPPORTED_FEATURES_SERVER_SUPPORT
    if (p_bt_bond_cb->otp_bond_cfg.use_gatts_client_supported_features)
    {
        p_bt_bond_cb->bond_ftl_cb.bt_common_fs_remote_client_supp_feats_offset = offset;
        offset += sizeof(T_REMOTE_CLIENT_SUPPORTED_FEATURES);
    }
#endif
    if (p_bt_bond_cb->otp_bond_cfg.gatt_cccd_storage)
    {
        uint16_t cccd_size = 4 + 4 * p_bt_bond_cb->otp_bond_cfg.gatt_storage_ccc_bits_count;
        p_bt_bond_cb->bond_ftl_cb.bt_common_fs_cccd_data_offset = offset;
        offset += cccd_size;
    }
    p_bt_bond_cb->bond_ftl_cb.bt_common_fs_link_block_size = offset;

#if F_BT_LE_SUPPORT
    if (p_bt_bond_cb->bond_ftl_cb.bt_stack_ftl_used_size != 0)
    {
        p_bt_bond_cb->bond_ftl_cb.bt_common_fs_le_start_offset =
            p_bt_bond_cb->bond_ftl_cb.bt_stack_ftl_used_size;
        p_bt_bond_cb->bond_ftl_cb.bt_stack_ftl_used_size += p_bt_bond_cb->otp_bond_cfg.max_le_paired_device
                                                            * p_bt_bond_cb->bond_ftl_cb.bt_common_fs_link_block_size;
    }
#endif
#if F_BT_BREDR_SUPPORT
    if (p_bt_bond_cb->otp_bond_cfg.bredr_ext_info_storage)
    {
        if (p_bt_bond_cb->bond_ftl_cb.bt_stack_ftl_used_size == 0)
        {
            p_bt_bond_cb->bond_ftl_cb.bt_stack_ftl_used_size = FLASH_LINK_INFO_START_OFFSET +
                                                               p_bt_bond_cb->otp_bond_cfg.max_legacy_paired_device *
                                                               FLASH_LEGACY_LINK_INFO_SIZE;
        }
        p_bt_bond_cb->bond_ftl_cb.bt_common_fs_bredr_start_offset =
            p_bt_bond_cb->bond_ftl_cb.bt_stack_ftl_used_size;
        p_bt_bond_cb->bond_ftl_cb.bt_stack_ftl_used_size +=
            p_bt_bond_cb->otp_bond_cfg.max_legacy_paired_device *
            p_bt_bond_cb->bond_ftl_cb.bt_common_fs_link_block_size;
    }
#endif
#if BLE_MGR_INIT_DEBUG
    BTM_PRINT_TRACE1("[BTBond] ftl_storage_common_init: bond_ftl_cb %b",
                     TRACE_BINARY(sizeof(T_BT_BOND_FTL_CB), &p_bt_bond_cb->bond_ftl_cb));
#endif
}

#if F_BT_BREDR_SUPPORT
uint32_t ftl_save_legacy_remote_bd(T_BT_BOND_INFO *p_data, uint8_t idx)
{
    if (p_bt_bond_cb == NULL || idx >= p_bt_bond_cb->otp_bond_cfg.max_legacy_paired_device)
    {
        return 1;
    }

    BTM_PRINT_TRACE4("[BTBond] ftl_save_legacy_remote_bd: idx %d, addr %s, bond_flag 0x%08x, key_type 0x%02x",
                     idx, TRACE_BDADDR(p_data->remote_bd), p_data->bond_flag, p_data->key_type);
    return imp_ftl_save(p_data, FLASH_LEGACY_LINK_INFO_BD_OFFSET, FLASH_LEGACY_LINK_INFO_SIZE,
                        sizeof(T_BT_BOND_INFO), idx);
}

uint32_t ftl_load_legacy_remote_bd(T_BT_BOND_INFO *p_data, uint8_t idx)
{
    if (p_bt_bond_cb == NULL || idx >= p_bt_bond_cb->otp_bond_cfg.max_legacy_paired_device)
    {
        return 1;
    }

    return imp_ftl_load(p_data, FLASH_LEGACY_LINK_INFO_BD_OFFSET, FLASH_LEGACY_LINK_INFO_SIZE,
                        sizeof(T_BT_BOND_INFO), idx);
}

uint32_t ftl_save_legacy_link_key(T_LEGACY_LINK_KEY *p_data, uint8_t idx)
{
    BTM_PRINT_TRACE1("[BTBond] ftl_save_legacy_link_key: idx %d", idx);
    if (p_bt_bond_cb == NULL || idx >= p_bt_bond_cb->otp_bond_cfg.max_legacy_paired_device)
    {
        return 1;
    }

    return imp_ftl_save(p_data, FLASH_LEGACY_LINK_INFO_LINKKEY_OFFSET, FLASH_LEGACY_LINK_INFO_SIZE,
                        sizeof(T_LEGACY_LINK_KEY), idx);
}

uint32_t ftl_load_legacy_link_key(T_LEGACY_LINK_KEY *p_data, uint8_t idx)
{
    BTM_PRINT_TRACE1("[BTBond] ftl_load_legacy_link_key: idx %d", idx);
    if (p_bt_bond_cb == NULL || idx >= p_bt_bond_cb->otp_bond_cfg.max_legacy_paired_device)
    {
        return 1;
    }

    return imp_ftl_load(p_data, FLASH_LEGACY_LINK_INFO_LINKKEY_OFFSET, FLASH_LEGACY_LINK_INFO_SIZE,
                        sizeof(T_LEGACY_LINK_KEY), idx);

}
#endif
#endif
