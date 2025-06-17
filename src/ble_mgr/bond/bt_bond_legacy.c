/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      bt_bond_legacy.c
* @brief     This file provides all the key storage related functions.
* @details
* @author    jane
* @date      2016-02-18
* @version   v0.1
* *********************************************************************************************************
*/
#if CONFIG_REALTEK_APP_BOND_MGR_SUPPORT
#include <trace.h>
#include <string.h>
#include <stdlib.h>
#include "bt_bond.h"
#include "ble_mgr_int.h"
#include "bt_bond_common_int.h"
#if F_BT_BREDR_SUPPORT
#include <bt_bond_legacy.h>
#include <bt_bond_priority.h>

uint8_t  bt_legacy_bond_store_number = 0;
T_BT_BOND_INFO *bt_legacy_bond_store_table = NULL;

const T_LEGACY_BOND_HAL legacy_bond =
{
    .p_bt_max_bond_num_get  = bt_legacy_get_max_bond_num,
    .p_bt_bond_num_get      = bt_legacy_get_bond_num,
    .p_bt_bond_addr_get     = bt_legacy_get_addr_by_priority,
    .p_bt_bond_index_get    = bt_legacy_get_paired_idx,
    .p_bt_bond_flag_get     = bt_legacy_get_bond_flag,
    .p_bt_bond_flag_set     = bt_legacy_set_bond_flag,
    .p_bt_bond_key_get      = bt_legacy_get_bond,
    .p_bt_bond_key_set      = bt_legacy_save_bond,
    .p_bt_bond_priority_set = bt_legacy_adjust_dev_priority,
    .p_bt_bond_delete       = bt_legacy_delete_bond,
    .p_bt_bond_clear        = bt_legacy_clear_all_keys
};

uint8_t bt_legacy_search_empty_dev(void)
{
    uint8_t i;
    for (i = 0; i < bt_legacy_bond_store_number; i++)
    {
        if (bt_legacy_bond_store_table[i].bond_state == 0)
        {
            break;
        }
    }

    return i;
}

void bt_legacy_key_init(void)
{
    uint8_t size;
    uint8_t i;
    if (p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return;
    }
    bt_bond_register_hal(&legacy_bond);

    bt_legacy_bond_store_number = p_bt_bond_cb->otp_bond_cfg.max_legacy_paired_device;
    size = bt_legacy_bond_store_number * sizeof(T_BT_BOND_INFO);
    bt_legacy_bond_store_table = calloc(1, size);
    if (bt_legacy_bond_store_table == NULL)
    {
        bt_legacy_bond_store_number = 0;
    }
    if (bt_bond_priority_queue_init(LEGACY_BT_DEV, bt_legacy_bond_store_number))
    {
        for (i = 0; i < bt_legacy_bond_store_number; i++)
        {
            ftl_load_legacy_remote_bd(&bt_legacy_bond_store_table[i], i);
            if (bt_legacy_bond_store_table[i].bond_state != 0)
            {
                BTM_PRINT_TRACE5("[BTBond] bt_legacy_key_init: idx %d, addr %s, bond_flag 0x%02x, key_type 0x%02x, bond_state 0x%02x",
                                 i, TRACE_BDADDR(&bt_legacy_bond_store_table[i].remote_bd),
                                 bt_legacy_bond_store_table[i].bond_flag,
                                 bt_legacy_bond_store_table[i].key_type,
                                 bt_legacy_bond_store_table[i].bond_state);
            }

            if (bt_legacy_bond_store_table[i].key_type > GAP_KEY_AUTHEN_P256 ||
                (bt_legacy_bond_store_table[i].bond_state != BT_BOND_INFO_EXIST_FLAG &&
                 bt_legacy_bond_store_table[i].bond_state != 0))
            {
                BTM_PRINT_ERROR0("[BTBond] bt_legacy_key_init: error, clear all keys");
                bt_legacy_clear_all_keys();
                break;
            }
        }
    }
}

void bt_legacy_clear_all_keys(void)
{
    uint8_t   i;
    if (p_bt_bond_cb == NULL)
    {
        return;
    }
    memset(bt_legacy_bond_store_table, 0x00, bt_legacy_bond_store_number * sizeof(T_BT_BOND_INFO));
    bt_bond_priority_queue_clear(LEGACY_BT_DEV);
    for (i = 0; i < bt_legacy_bond_store_number; i++)
    {
        ftl_save_legacy_remote_bd(&bt_legacy_bond_store_table[i], i);
        if (p_bt_bond_cb->otp_bond_cfg.bredr_ext_info_storage)
        {
            bt_bond_remove(LEGACY_BT_DEV, i);
        }
    }
}

bool bt_legacy_get_paired_idx(uint8_t *bd_addr, uint8_t *p_idx)
{
    uint8_t i;
    if (bd_addr == NULL || p_idx == NULL)
    {
        return false;
    }

    for (i = 0; i < bt_legacy_bond_store_number; i++)
    {
        if ((bt_legacy_bond_store_table[i].bond_state != 0) &&
            memcmp(bt_legacy_bond_store_table[i].remote_bd, bd_addr, 6) == 0)
        {
            break;
        }
    }

    if (i == bt_legacy_bond_store_number)
    {
        return false;
    }
    else
    {
        *p_idx = i;
        return true;
    }
}

bool bt_legacy_get_addr_by_priority(uint8_t priority, uint8_t *bd_addr)
{
    uint8_t i;
    /*search assign priority device*/
    i = bt_bond_get_index_by_priority(LEGACY_BT_DEV, priority);
    if (i != 0xff && bd_addr != NULL)
    {
        memcpy(bd_addr, bt_legacy_bond_store_table[i].remote_bd, 6);
        return true;
    }

    return false;
}

uint8_t bt_legacy_get_bond_num(void)
{
    uint8_t i;
    uint8_t paired_num = 0;

    for (i = 0; i < bt_legacy_bond_store_number; i++)
    {
        if (bt_legacy_bond_store_table[i].bond_state != 0)
        {
            paired_num++;
        }
    }

    return paired_num;
}

bool bt_legacy_adjust_dev_priority(uint8_t *bd_addr)
{
    uint8_t index;

    if (bt_legacy_get_paired_idx(bd_addr, &index) == false)
    {
        return false;
    }

    return bt_bond_set_high_priority(LEGACY_BT_DEV, index);
}

bool bt_legacy_set_bond_flag(uint8_t *bd_addr, uint32_t bond_flag)
{
    uint8_t index;

    if (bt_legacy_get_paired_idx(bd_addr, &index) == false)
    {
        return false;
    }

    bt_legacy_bond_store_table[index].bond_flag = bond_flag;
    ftl_save_legacy_remote_bd(&bt_legacy_bond_store_table[index], index);

    return true;
}

bool bt_legacy_get_bond_flag(uint8_t *bd_addr, uint32_t *bond_flag)
{
    uint8_t index;
    if (bd_addr == NULL || bond_flag == NULL)
    {
        return false;
    }

    if (bt_legacy_get_paired_idx(bd_addr, &index) == false)
    {
        return false;
    }

    *bond_flag = bt_legacy_bond_store_table[index].bond_flag;

    return true;
}

bool bt_legacy_save_bond(uint8_t *bd_addr, uint8_t *linkkey, uint8_t key_type)
{
    uint8_t index;
    T_LEGACY_LINK_KEY legacy_link_key;
    uint8_t tmp_addr[6];
    if (bd_addr == NULL || linkkey == NULL)
    {
        return false;
    }
    if (bt_legacy_get_paired_idx(bd_addr, &index) == false)
    {
        index = bt_legacy_search_empty_dev();
        if (bt_legacy_bond_store_number == index)
        {
            if (bt_legacy_get_addr_by_priority(bt_legacy_bond_store_number, tmp_addr) == false)
            {
                return false;
            }
            if (bt_legacy_get_paired_idx(tmp_addr, &index) == false)
            {
                return false;
            }
        }
    }

    if (key_type != GAP_KEY_AUTHEN_P256 && key_type != GAP_KEY_UNAUTHEN_P256)
    {
        bt_legacy_bond_store_table[index].bond_flag = 0;
    }

    memcpy(bt_legacy_bond_store_table[index].remote_bd, bd_addr, 6);
    memcpy(legacy_link_key.key, linkkey, 16);
    bt_legacy_bond_store_table[index].bond_state = BT_BOND_INFO_EXIST_FLAG;
    bt_legacy_bond_store_table[index].key_type = key_type;
    bt_legacy_bond_store_table[index].remote_bd_type = GAP_REMOTE_ADDR_CLASSIC;
    //gap_get_param(GAP_PARAM_BD_ADDR, bt_legacy_bond_store_table[index].local_bd);
    //bt_legacy_bond_store_table[index].local_bd_type = 0;

    if (ftl_save_legacy_remote_bd(&bt_legacy_bond_store_table[index], index) != 0)
    {
        return false;
    }

    if (ftl_save_legacy_link_key(&legacy_link_key, index) != 0)
    {
        return false;
    }

    return bt_bond_priority_queue_add(LEGACY_BT_DEV, index);  // add to Priority table

}

bool bt_legacy_get_bond(uint8_t *bd_addr, uint8_t *legacy_link_key, uint8_t *key_type)
{
    uint8_t index;
    if (bd_addr == NULL || legacy_link_key == NULL || key_type == NULL)
    {
        return false;
    }
    if (bt_legacy_get_paired_idx(bd_addr, &index) == false)
    {
        return false;
    }

    if (ftl_load_legacy_link_key((T_LEGACY_LINK_KEY *)legacy_link_key, index) == 0)
    {
        *key_type = bt_legacy_bond_store_table[index].key_type;
        return true;
    }
    else
    {
        return false;
    }
}

bool bt_legacy_delete_bond(uint8_t *bd_addr)
{
    uint8_t index;
    if (p_bt_bond_cb == NULL)
    {
        return false;
    }
    if (bt_legacy_get_paired_idx(bd_addr, &index) == false)
    {
        return false;
    }

    bt_bond_priority_queue_delete(LEGACY_BT_DEV, index);
    memset(&bt_legacy_bond_store_table[index], 0x00, sizeof(T_BT_BOND_INFO));
    ftl_save_legacy_remote_bd(&bt_legacy_bond_store_table[index], index);
    if (p_bt_bond_cb->otp_bond_cfg.bredr_ext_info_storage)
    {
        bt_bond_remove(LEGACY_BT_DEV, index);
    }
    return true;
}

uint8_t bt_legacy_get_max_bond_num(void)
{
    return bt_legacy_bond_store_number;
}

#if BLE_MGR_DEINIT
void bt_legacy_key_deinit(void)
{
    if (bt_legacy_bond_store_table)
    {
        bt_legacy_bond_store_number = 0;
        free(bt_legacy_bond_store_table);
        bt_legacy_bond_store_table = NULL;
    }
}
#endif
#endif
#endif
