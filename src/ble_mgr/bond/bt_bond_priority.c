/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      ftl_key_priorty_mgr.c
* @brief     This file provides all the key storage related functions.
* @details
* @author    jane
* @date      2016-02-18
* @version   v0.1
* *********************************************************************************************************
*/
#if CONFIG_REALTEK_APP_BOND_MGR_SUPPORT
#include <string.h>
#include <trace.h>
#include <bt_bond_ftl_int.h>
#include <bt_bond_priority.h>
#include "bt_bond_common_int.h"


bool bt_bond_priority_queue_init(T_DEV_TYPE type, uint8_t max_bond_num)
{
    if (p_bt_bond_cb == NULL)
    {
        return false;
    }
    if (type == LE_BT_DEV)
    {
#if F_BT_LE_SUPPORT
        if (ftl_load_bond_priority(LE_BT_DEV, &p_bt_bond_cb->bt_le_bond_sort) == 0)
        {
#if BLE_MGR_INIT_DEBUG
            BTM_PRINT_INFO1("[BTBond] bt_bond_priority_queue_init: le bond num 0x%02x",
                            p_bt_bond_cb->bt_le_bond_sort.bond_num);
#endif
            if (p_bt_bond_cb->bt_le_bond_sort.bond_num > 19)
            {
                p_bt_bond_cb->bt_le_bond_sort.bond_num = 0;
                return false;
            }
        }
#endif
    }
#if F_BT_BREDR_SUPPORT
    else
    {
        if (ftl_load_bond_priority(LEGACY_BT_DEV, &p_bt_bond_cb->bt_legacy_bond_sort) == 0)
        {
#if BLE_MGR_INIT_DEBUG
            uint8_t i = 0;
            BTM_PRINT_INFO1("[BTBond] bt_bond_priority_queue_init: legacy bond num 0x%02x",
                            p_bt_bond_cb->bt_legacy_bond_sort.bond_num);
#endif
            if (p_bt_bond_cb->bt_legacy_bond_sort.bond_num > 19)
            {
                p_bt_bond_cb->bt_legacy_bond_sort.bond_num = 0;
                return false;
            }
#if BLE_MGR_INIT_DEBUG
            for (i = p_bt_bond_cb->bt_legacy_bond_sort.bond_num; i > 0; i--)
            {
                BTM_PRINT_INFO2("[BTBond] priority 0x%02x, index  0x%02x", i,
                                p_bt_bond_cb->bt_legacy_bond_sort.bond_idx[p_bt_bond_cb->bt_legacy_bond_sort.bond_num - i]);
            }
#endif
        }
    }
#endif
    return true;
}

#define bt_bond_priority_sort_get(type) bt_bond_priority_sort_get_int(__func__, type)

T_BOND_PRIORITY *bt_bond_priority_sort_get_int(const char *p_func_name, T_DEV_TYPE type)
{
    T_BOND_PRIORITY *p_bt_bond_sort = NULL;
    BTM_PRINT_TRACE2("bt_bond_priority_sort_get: %s, type 0x%x", TRACE_STRING(p_func_name), type);
    if (p_bt_bond_cb)
    {
        if (type == LE_BT_DEV)
        {
#if F_BT_LE_SUPPORT
            p_bt_bond_sort = &p_bt_bond_cb->bt_le_bond_sort;
#endif
        }
#if F_BT_BREDR_SUPPORT
        else
        {
            p_bt_bond_sort = &p_bt_bond_cb->bt_legacy_bond_sort;
        }
#endif
    }
    if (p_bt_bond_sort != NULL && p_bt_bond_sort->bond_num == 0)
    {
        p_bt_bond_sort = NULL;
    }
    if (p_bt_bond_sort == NULL)
    {
        BTM_PRINT_ERROR2("[BTBond] bt_bond_priority_sort_get: failed, %s, type 0x%02x",
                         TRACE_STRING(p_func_name), type);
    }
    return p_bt_bond_sort;
}

void bt_bond_priority_queue_clear(T_DEV_TYPE type)
{
    T_BOND_PRIORITY *p_bt_bond_sort = bt_bond_priority_sort_get(type);

    if (p_bt_bond_sort == NULL)
    {
        return;
    }
    p_bt_bond_sort->bond_num = 0;
    ftl_save_bond_priority(type, p_bt_bond_sort);
}

uint8_t bt_bond_priority_queue_find(T_DEV_TYPE type, uint8_t idx)
{
    uint8_t i = 0;
    T_BOND_PRIORITY *p_bt_bond_sort = bt_bond_priority_sort_get(type);
    if (p_bt_bond_sort == NULL)
    {
        return 0xff;
    }
    for (i = 0; i < p_bt_bond_sort->bond_num; i++)
    {
        if (p_bt_bond_sort->bond_idx[i] == idx)
        {
            return i;
        }
    }
    BTM_PRINT_ERROR2("[BTBond] bt_bond_priority_queue_find: failed, type 0x%02x, idx %d", type, idx);
    return 0xff;
}

bool bt_bond_priority_queue_delete(T_DEV_TYPE type, uint8_t idx)
{
    uint8_t result = 0;
    T_BOND_PRIORITY *p_bt_bond_sort = bt_bond_priority_sort_get(type);
    if (p_bt_bond_sort == NULL)
    {
        return false;
    }
    result = bt_bond_priority_queue_find(type, idx);
    if (result != 0xff)
    {
        uint8_t i;
        for (i = result; i < p_bt_bond_sort->bond_num; i++)
        {
            p_bt_bond_sort->bond_idx[i] = p_bt_bond_sort->bond_idx[i + 1];
        }
        p_bt_bond_sort->bond_num--;
        BTM_PRINT_INFO3("[BTBond] bt_bond_priority_queue_delete: bond num 0x%02x, idx %d, bond index %b",
                        p_bt_bond_sort->bond_num, idx, TRACE_BINARY(19, p_bt_bond_sort->bond_idx));
        ftl_save_bond_priority(type, p_bt_bond_sort);
        return true;

    }
    else
    {
        return false;
    }

}

bool bt_bond_priority_queue_add(T_DEV_TYPE type, uint8_t idx)
{
    BTM_PRINT_TRACE2("[BTBond] bt_bond_priority_queue_add: type %d, idx %d", type, idx);
    uint8_t result = 0;
    uint8_t max_num = 0;
    T_BOND_PRIORITY *p_bt_bond_sort = NULL;

    if (type == LE_BT_DEV)
    {
#if F_BT_LE_SUPPORT
        p_bt_bond_sort = &p_bt_bond_cb->bt_le_bond_sort;
        max_num = p_bt_bond_cb->otp_bond_cfg.max_le_paired_device;
#endif
    }
#if F_BT_BREDR_SUPPORT
    else
    {
        p_bt_bond_sort = &p_bt_bond_cb->bt_legacy_bond_sort;
        max_num = p_bt_bond_cb->otp_bond_cfg.max_legacy_paired_device;
    }
#endif
    if (p_bt_bond_sort == NULL)
    {
        return false;
    }
    result = bt_bond_priority_queue_find(type, idx);
    if (result != 0xff)
    {
        return bt_bond_set_high_priority(type, idx);
    }
    else
    {
        if (p_bt_bond_sort->bond_num == max_num)
        {
            BTM_PRINT_ERROR0("[BTBond] bt_bond_priority_queue_add: add failed");
            return false;
        }
        p_bt_bond_sort->bond_idx[p_bt_bond_sort->bond_num] = idx;
        p_bt_bond_sort->bond_num++;
        BTM_PRINT_INFO2("[BTBond] bt_bond_priority_queue_add: bond num 0x%02x bond index %b",
                        p_bt_bond_sort->bond_num, TRACE_BINARY(19, p_bt_bond_sort->bond_idx));
        ftl_save_bond_priority(type, p_bt_bond_sort);
        return true;
    }
}

bool bt_bond_set_high_priority(T_DEV_TYPE type, uint8_t idx)
{
    uint8_t result = 0;
    T_BOND_PRIORITY *p_bt_bond_sort = bt_bond_priority_sort_get(type);
    if (p_bt_bond_sort == NULL)
    {
        return false;
    }
    result = bt_bond_priority_queue_find(type, idx);
    if (result != 0xff)
    {
        if (result == (p_bt_bond_sort->bond_num - 1))
        {
            return true;
        }
        else
        {
            uint8_t i;
            for (i = result; i < p_bt_bond_sort->bond_num; i++)
            {
                p_bt_bond_sort->bond_idx[i] = p_bt_bond_sort->bond_idx[i + 1];
            }
            p_bt_bond_sort->bond_idx[p_bt_bond_sort->bond_num - 1] = idx;
            BTM_PRINT_INFO3("[BTBond] bt_bond_set_high_priority: bond num 0x%02x, idx %d, bond index %b",
                            p_bt_bond_sort->bond_num, idx, TRACE_BINARY(19, p_bt_bond_sort->bond_idx));
            ftl_save_bond_priority(type, p_bt_bond_sort);
            return true;
        }
    }
    else
    {
        return false;
    }
}

uint8_t bt_bond_get_low_priority(T_DEV_TYPE type)
{
    uint8_t idx;
    T_BOND_PRIORITY *p_bt_bond_sort = bt_bond_priority_sort_get(type);
    if (p_bt_bond_sort == NULL)
    {
        return 0xff;
    }
    idx = p_bt_bond_sort->bond_idx[0];
    return idx;
}

uint8_t bt_bond_get_index_by_priority(T_DEV_TYPE type, uint8_t priority)
{
    uint8_t idx;
    T_BOND_PRIORITY *p_bt_bond_sort = bt_bond_priority_sort_get(type);
    if (p_bt_bond_sort == NULL)
    {
        return 0xff;
    }
    if ((p_bt_bond_sort->bond_num < priority) || (priority == 0))
    {
        BTM_PRINT_ERROR2("[BTBond] bt_bond_get_index_by_priority: failed, bond_num %d, priority %d",
                         p_bt_bond_sort->bond_num, priority);
        return 0xff;
    }
    idx = p_bt_bond_sort->bond_idx[p_bt_bond_sort->bond_num - priority];
    return idx;
}
#endif
