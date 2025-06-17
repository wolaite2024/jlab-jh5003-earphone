/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      bt_bond_le.c
* @brief     This file provides all the key storage related functions.
* @details
* @author    jane
* @date      2016-02-18
* @version   v0.1
* *********************************************************************************************************
*/
#if CONFIG_REALTEK_APP_BOND_MGR_SUPPORT
#include <bt_bond_le.h>
#include <bt_bond_le_int.h>
#include <bt_bond_le_sync.h>
#include <trace.h>
#include <string.h>
#include <stdlib.h>
#include <bt_bond_priority.h>
#include <aes_api.h>
#include "bt_bond_common_int.h"
#include "ble_mgr_int.h"

#if F_BT_LE_SUPPORT
#define LE_ENCRYPT_DATA_LENGTH       0x10

T_LE_BOND_ENTRY *le_key_store_table = NULL;
uint8_t le_bond_storage_num = 0;
const uint8_t null_bd[6] = {0, 0, 0, 0, 0, 0};
T_OS_QUEUE bt_le_local_irk_queue;

typedef struct t_bt_le_local_irk
{
    struct t_bt_le_local_irk *p_next;
    uint8_t  local_identity_address_type;
    uint8_t  local_identity_address[GAP_BD_ADDR_LEN];
    uint8_t  local_irk[16];
} T_BT_LE_LOCAL_IRK;

void bt_le_delete_key_entry(T_LE_BOND_ENTRY *p_entry);

void bt_le_key_send_update_info(T_LE_BOND_ENTRY *p_entry)
{
    T_BT_LE_BOND_CB_DATA cb_data;
    T_BT_LE_BOND_UPDATE bond_update;
    bond_update.modify_flags = p_entry->modify_flags;
    bond_update.p_entry = p_entry;
    cb_data.p_le_bond_update = &bond_update;
    bt_bond_msg_post(BT_BOND_MSG_LE_BOND_UPDATE, &cb_data);
    p_entry->modify_flags = 0;
}

void bt_le_update_identity_addr(T_LE_BOND_ENTRY *p_entry)
{
    if (p_entry->remote_bd_type == GAP_REMOTE_ADDR_LE_RANDOM)
    {
        if ((p_entry->remote_bd[5] & RANDOM_ADDR_MASK) == RANDOM_ADDR_MASK_STATIC)
        {
            p_entry->remote_identity_addr_type = GAP_IDENT_ADDR_RAND;
        }
        else
        {
            return;
        }
    }
    else if (p_entry->remote_bd_type == GAP_REMOTE_ADDR_LE_PUBLIC ||
             p_entry->remote_bd_type == GAP_REMOTE_ADDR_LE_PUBLIC_IDENTITY)
    {
        p_entry->remote_identity_addr_type = GAP_IDENT_ADDR_PUBLIC;
    }
    if (p_entry->remote_bd_type == GAP_REMOTE_ADDR_LE_PUBLIC_IDENTITY)
    {
        p_entry->remote_identity_addr_type = GAP_IDENT_ADDR_RAND;
    }
    memcpy(p_entry->remote_identity_addr, p_entry->remote_bd, 6);
}

bool bt_le_key_entry_init(T_LE_BOND_ENTRY *p_entry)
{
    if (p_entry == NULL)
    {
        return false;
    }
    if (ftl_load_le_remote_bd(&p_entry->bond_info, p_entry->idx) == 0)
    {
        if (p_entry->bond_info.bond_flag & LE_BOND_EXIST_BIT)
        {
            p_entry->is_used = true;
            p_entry->flags |= LE_KEY_STORE_REMOTE_BD_BIT;
            if (p_entry->bond_info.bond_flag & LE_LOCAL_IRK_DIST_BIT)
            {
                p_entry->flags |= LE_KEY_STORE_LOCAL_IRK_BIT;
            }
            p_entry->remote_bd_type = p_entry->bond_info.remote_bd_type;
            if (p_entry->remote_bd_type > GAP_REMOTE_ADDR_LE_RANDOM_IDENTITY)
            {
                BTM_PRINT_ERROR0("[BTBond] bt_le_key_entry_init: check remote addr failed");
                return false;
            }
            memcpy(p_entry->remote_bd, p_entry->bond_info.remote_bd, 6);
            bt_le_update_identity_addr(p_entry);
        }
    }
    if (p_entry->is_used == true)
    {
        T_LE_LOCAL_LTK local_ltk;
        T_LE_REMOTE_LTK remote_ltk;
        T_LE_REMOTE_IRK remote_irk;

        if (ftl_load_le_local_ltk(&local_ltk, p_entry->idx) == 0)
        {
            if (local_ltk.link_key_length != 0)
            {
                p_entry->flags |= LE_KEY_STORE_LOCAL_LTK_BIT;
            }
        }
        if (ftl_load_le_remote_ltk(&remote_ltk, p_entry->idx) == 0)
        {
            if (remote_ltk.link_key_length != 0)
            {
                p_entry->flags |= LE_KEY_STORE_REMOTE_LTK_BIT;
            }
        }
        if (ftl_load_le_remote_irk(&remote_irk, p_entry->idx) == 0)
        {
            if (remote_irk.key_exist != 0)
            {
                memcpy(p_entry->remote_identity_addr, remote_irk.addr, 6);
                p_entry->remote_identity_addr_type = remote_irk.addr_type;
                if (remote_irk.addr_type > GAP_IDENT_ADDR_RAND)
                {
                    BTM_PRINT_ERROR0("[BTBond] bt_le_key_entry_init: check indentity addr failed");
                    return false;
                }
                p_entry->flags |= LE_KEY_STORE_REMOTE_IRK_BIT;
            }
        }
        BTM_PRINT_INFO6("[BTBond] bt_le_key_entry_init: idx %d, addr %s, remote_bd_type %d, flags 0x%02x, local_bd %s, local_bd_type %d",
                        p_entry->idx, TRACE_BDADDR(p_entry->bond_info.remote_bd),
                        p_entry->bond_info.remote_bd_type, p_entry->flags,
                        TRACE_BDADDR(p_entry->bond_info.local_bd), p_entry->bond_info.local_bd_type);
    }
    return true;
}

void bt_le_key_init(void)
{
    uint8_t i;
    uint16_t size = 0;

    if (p_bt_bond_cb->otp_bond_cfg.max_le_paired_device == 0)
    {
        return;
    }

    le_bond_storage_num = p_bt_bond_cb->otp_bond_cfg.max_le_paired_device;
    size = le_bond_storage_num * sizeof(T_LE_BOND_ENTRY);
    le_key_store_table = (T_LE_BOND_ENTRY *)calloc(1, size);
    if (le_key_store_table == NULL)
    {
        le_bond_storage_num = 0;
        return;
    }
    ftl_le_storage_init();
    ftl_storage_common_init();
    if (bt_bond_priority_queue_init(LE_BT_DEV, le_bond_storage_num))
    {
        for (i = 0; i < le_bond_storage_num; i++)
        {
            T_LE_BOND_ENTRY *p_entry = &le_key_store_table[i];
            p_entry->idx = i;
            if (bt_le_key_entry_init(p_entry) == false)
            {
                for (uint8_t j = 0; j <= i; j++)
                {
                    T_LE_BOND_ENTRY *p_entry_del = &le_key_store_table[i];
                    if (p_entry_del->is_used == true)
                    {
                        bt_le_delete_key_entry(p_entry_del);
                    }
                }

                bt_bond_priority_queue_clear(LE_BT_DEV);
                break;
            }
        }
    }
}

bool bt_le_resolve_local_rpa(uint8_t *unresolved_addr, uint8_t *resolved_addr,
                             uint8_t *resolved_addr_type)
{
    T_BT_LE_LOCAL_IRK *p_local_irk;
    uint8_t buffer[LE_ENCRYPT_DATA_LENGTH];
    uint8_t irk[LE_ENCRYPT_DATA_LENGTH];
    uint8_t encrypt_buffer[LE_ENCRYPT_DATA_LENGTH];

    memset(buffer, 0x00, LE_ENCRYPT_DATA_LENGTH);
    buffer[13] = unresolved_addr[5];
    buffer[14] = unresolved_addr[4];
    buffer[15] = unresolved_addr[3];

    for (uint8_t i = 0; i < bt_le_local_irk_queue.count; i++)
    {
        p_local_irk = (T_BT_LE_LOCAL_IRK *)os_queue_peek(&bt_le_local_irk_queue, i);
        if (p_local_irk != NULL)
        {
            for (uint8_t j = 0; j < LE_ENCRYPT_DATA_LENGTH; j++)
            {
                irk[j] = p_local_irk->local_irk[LE_ENCRYPT_DATA_LENGTH - 1 - j];
            }
            aes128_ecb_encrypt(buffer, irk, encrypt_buffer);

            if ((encrypt_buffer[13] == unresolved_addr[2])
                && (encrypt_buffer[14] == unresolved_addr[1])
                && (encrypt_buffer[15] == unresolved_addr[0]))
            {
                *resolved_addr_type = p_local_irk->local_identity_address_type;
                memcpy(resolved_addr, p_local_irk->local_identity_address, 6);
                BTM_PRINT_INFO2("[BTBond] bt_le_resolve_local_rpa: address %s, type %d",
                                TRACE_BDADDR(resolved_addr),
                                p_local_irk->local_identity_address_type);
                return true;
            }
        }
    }
    return false;
}

bool bt_le_get_local_irk(T_LE_BOND_ENTRY *p_entry, uint8_t *p_irk)
{
    T_BT_LE_LOCAL_IRK *p_local_irk;

    for (uint8_t i = 0; i < bt_le_local_irk_queue.count; i++)
    {
        p_local_irk = (T_BT_LE_LOCAL_IRK *)os_queue_peek(&bt_le_local_irk_queue, i);
        if (p_local_irk != NULL &&
            p_local_irk->local_identity_address_type == p_entry->bond_info.local_bd_type &&
            memcmp(p_local_irk->local_identity_address, p_entry->bond_info.local_bd, GAP_BD_ADDR_LEN) == 0)
        {
            memcpy(p_irk, p_local_irk->local_irk, 16);
            return true;
        }
    }
    return false;
}

bool bt_le_set_local_irk(uint8_t local_identity_address_type,
                         uint8_t *local_identity_address, uint8_t *local_irk)
{
    T_BT_LE_LOCAL_IRK *p_local_irk;

    for (uint8_t i = 0; i < bt_le_local_irk_queue.count; i++)
    {
        p_local_irk = (T_BT_LE_LOCAL_IRK *)os_queue_peek(&bt_le_local_irk_queue, i);
        if (p_local_irk != NULL &&
            p_local_irk->local_identity_address_type == local_identity_address_type &&
            memcmp(p_local_irk->local_identity_address, local_identity_address, GAP_BD_ADDR_LEN) == 0)
        {
            return p_local_irk;
        }
    }

    p_local_irk = calloc(1, sizeof(T_BT_LE_LOCAL_IRK));
    if (p_local_irk)
    {
        p_local_irk->local_identity_address_type = local_identity_address_type;
        memcpy(p_local_irk->local_identity_address, local_identity_address, GAP_BD_ADDR_LEN);
        memcpy(p_local_irk->local_irk, local_irk, 16);
        os_queue_in(&bt_le_local_irk_queue, p_local_irk);
        BTM_PRINT_INFO2("[BTBond] bt_le_set_local_irk: address %s, type %d",
                        TRACE_BDADDR(local_identity_address),
                        local_identity_address_type);
        return true;
    }
    return false;
}

T_LE_BOND_ENTRY *bt_le_find_key_entry(uint8_t *bd_addr, T_GAP_REMOTE_ADDR_TYPE bd_type,
                                      uint8_t *local_bd_addr, uint8_t local_bd_type)
{
    uint8_t   i;
    T_LE_BOND_ENTRY *p_entry;
    bool local_addr_not_check = false;
    uint8_t identity_bd_addr[6];
    uint8_t peer_bd_addr[6];

    memcpy(peer_bd_addr, bd_addr, 6);

    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return NULL;
    }

    /* BB2PLUSRD-584 */
    if ((bd_type == GAP_REMOTE_ADDR_LE_RANDOM) && ((peer_bd_addr[5] & 0xC0) == 0x80))
    {
        peer_bd_addr[5] &= 0x3F;
        peer_bd_addr[5] |= 0x40;
    }

    if (local_bd_addr == NULL)
    {
        local_addr_not_check = true;
    }
    else
    {
        memcpy(identity_bd_addr, local_bd_addr, 6);
        if (local_bd_type > GAP_LOCAL_ADDR_LE_RAP_OR_RAND)
        {
            local_addr_not_check = true;
        }
        else if (local_bd_type == GAP_LOCAL_ADDR_LE_RAP_OR_PUBLIC ||
                 local_bd_type == GAP_LOCAL_ADDR_LE_RAP_OR_RAND)
        {
            local_bd_type -= 2;
        }
        if ((local_bd_type == GAP_LOCAL_ADDR_LE_RANDOM) && ((local_bd_addr[5] & 0xC0) == 0x40))
        {
            if (bt_le_resolve_local_rpa(local_bd_addr, identity_bd_addr, &local_bd_type) == false)
            {
                local_addr_not_check = true;
            }
        }
    }

    if ((bd_type == GAP_REMOTE_ADDR_LE_PUBLIC_IDENTITY)
        || (bd_type == GAP_REMOTE_ADDR_LE_RANDOM_IDENTITY))
    {
        bd_type -= 2;
        for (i = 0; i < le_bond_storage_num; i++)
        {
            p_entry = &le_key_store_table[i];
            if ((p_entry->is_used == true) &&
                (memcmp(p_entry->remote_identity_addr, peer_bd_addr, 6) == 0) &&
                ((p_entry->remote_identity_addr_type == bd_type))
               )
            {
                if (local_addr_not_check || (p_entry->bond_info.local_bd_type == local_bd_type
                                             && memcmp(p_entry->bond_info.local_bd, identity_bd_addr, 6) == 0))
                {
                    BTM_PRINT_TRACE1("[BTBond] bt_le_find_key_entry: find resolved1 %d", i);
                    return p_entry;
                }
            }
        }
    }
    else
    {
        for (i = 0; i < le_bond_storage_num; i++)
        {
            p_entry = &le_key_store_table[i];
            if (p_entry->is_used)
            {
                if (((memcmp(p_entry->remote_bd, peer_bd_addr, 6) == 0) &&
                     ((p_entry->remote_bd_type == bd_type) || (bd_type == 0x20))) || //BTIF_REMOTE_ADDR_ANY = 0x20
                    ((memcmp(p_entry->remote_identity_addr, peer_bd_addr, 6) == 0) &&
                     (p_entry->remote_identity_addr_type == bd_type))
                   )
                {
                    if (local_addr_not_check || (p_entry->bond_info.local_bd_type == local_bd_type
                                                 && memcmp(p_entry->bond_info.local_bd, identity_bd_addr, 6) == 0))
                    {
                        BTM_PRINT_TRACE1("[BTBond] bt_le_find_key_entry: find %d", i);
                        return p_entry;
                    }
                }
            }

        }
    }

    BTM_PRINT_ERROR4("[BTBond] bt_le_find_key_entry: not find, peer_bd_addr %s, bd_type %d, local_bd_addr %s, local_bd_type %d",
                     TRACE_BDADDR(peer_bd_addr),
                     bd_type,
                     TRACE_BDADDR(local_bd_addr),
                     local_bd_type);
    return NULL;
}

uint8_t bt_le_find_key_entry_num(uint8_t *identity_bd_addr, uint8_t identity_bd_type)
{
    uint8_t   i;
    uint8_t bond_num = 0;
    T_LE_BOND_ENTRY *p_entry;
    if (memcmp(identity_bd_addr, null_bd, 6) == 0)
    {
        return bond_num;
    }
    for (i = 0; i < le_bond_storage_num; i++)
    {
        p_entry = &le_key_store_table[i];
        if ((p_entry->is_used == true) &&
            (memcmp(p_entry->remote_identity_addr, identity_bd_addr, 6) == 0) &&
            ((p_entry->remote_identity_addr_type == identity_bd_type))
           )
        {
            bond_num++;
        }
    }
    return bond_num;
}

T_LE_BOND_ENTRY *bt_le_find_key_entry_by_idx(uint8_t idx)
{
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return NULL;
    }

    T_LE_BOND_ENTRY *p_entry = NULL;
    if (idx >= le_bond_storage_num)
    {
        return p_entry;
    }
    if (le_key_store_table[idx].is_used == true)
    {
        p_entry = &le_key_store_table[idx];
    }
    return p_entry;
}

T_LE_BOND_ENTRY *bt_le_find_key_entry_by_priority(uint8_t priority)
{
    uint8_t idx;
    /*search assign priority device*/
    idx = bt_bond_get_index_by_priority(LE_BT_DEV, priority);
    if (idx != 0xff)
    {
        return bt_le_find_key_entry_by_idx(idx);
    }

    return NULL;
}

T_LE_BOND_ENTRY *bt_le_allocate_key_entry(uint8_t *bd_addr, T_GAP_REMOTE_ADDR_TYPE bd_type,
                                          uint8_t *local_bd, uint8_t local_bd_type)
{
    uint8_t   i;

    uint8_t peer_bd_addr[6];

    memcpy(peer_bd_addr, bd_addr, 6);

    /* BB2PLUSRD-584 */
    if ((bd_type == GAP_REMOTE_ADDR_LE_RANDOM) && ((peer_bd_addr[5] & 0xC0) == 0x80))
    {
        peer_bd_addr[5] &= 0x3F;
        peer_bd_addr[5] |= 0x40;
    }

    for (i = 0; i < le_bond_storage_num; i++)
    {
        T_LE_BOND_ENTRY *p_entry = &le_key_store_table[i];
        if (p_entry->is_used == false)
        {
            uint8_t identity_bd_addr[6];

            p_entry->idx = i;
            p_entry->is_used = true;
            p_entry->flags = 0;
            p_entry->modify_flags = LE_BOND_MODIFY_NEW;
            p_entry->bond_info.remote_bd_type = bd_type;
            memcpy(p_entry->bond_info.remote_bd, peer_bd_addr, 6);
            if (local_bd == NULL)
            {
                memset(identity_bd_addr, 0, 6);
            }
            else
            {
                memcpy(identity_bd_addr, local_bd, 6);
                if ((local_bd_type == GAP_LOCAL_ADDR_LE_RANDOM) && ((local_bd[5] & 0xC0) == 0x40))
                {
                    bt_le_resolve_local_rpa(local_bd, identity_bd_addr, &local_bd_type);
                }
            }
            memcpy(p_entry->bond_info.local_bd, identity_bd_addr, 6);
            p_entry->bond_info.local_bd_type = local_bd_type;
            p_entry->remote_bd_type = bd_type;
            memcpy(p_entry->remote_bd, peer_bd_addr, 6);
            bt_le_update_identity_addr(p_entry);
            BTM_PRINT_TRACE1("[BTBond] bt_le_allocate_key_entry: idx %d", i);
            return p_entry;
        }
    }
    BTM_PRINT_ERROR0("[BTBond] bt_le_allocate_key_entry: failed");
    return NULL;
}

uint8_t bt_le_get_bond_dev_num(void)
{
    if (p_bt_bond_cb)
    {
        return p_bt_bond_cb->bt_le_bond_sort.bond_num;
    }
    return 0;
}

bool bt_le_set_high_priority_bond(T_LE_BOND_ENTRY *p_entry)
{
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return false;
    }

    if (p_entry != NULL)
    {
        return bt_bond_set_high_priority(LE_BT_DEV, p_entry->idx);
    }
    return false;
}

uint8_t bt_le_get_key(T_LE_BOND_ENTRY *p_entry, T_GAP_KEY_TYPE key_type, uint8_t *key)
{
    uint8_t key_len = 0;
    switch (key_type)
    {
    default:
        break;
    case GAP_KEY_LE_LOCAL_LTK:
        if (p_entry->flags & LE_KEY_STORE_LOCAL_LTK_BIT)
        {
            T_LE_LOCAL_LTK local_ltk;
            if (0 == ftl_load_le_local_ltk(&local_ltk, p_entry->idx))
            {
                if (local_ltk.link_key_length <= 28)
                {
                    memcpy(key, local_ltk.key, local_ltk.link_key_length);
                    key_len = local_ltk.link_key_length;
                }
            }
        }
        break;
    case GAP_KEY_LE_REMOTE_LTK:
        if (p_entry->flags & LE_KEY_STORE_REMOTE_LTK_BIT)
        {
            T_LE_REMOTE_LTK remote_ltk;
            if (0 == ftl_load_le_remote_ltk(&remote_ltk, p_entry->idx))
            {
                if (remote_ltk.link_key_length <= 28)
                {
                    memcpy(key, remote_ltk.key, remote_ltk.link_key_length);
                    key_len = remote_ltk.link_key_length;
                }
            }
        }
        break;
    case GAP_KEY_LE_REMOTE_IRK:
        if (p_entry->flags & LE_KEY_STORE_REMOTE_IRK_BIT)
        {
            if (0 == ftl_load_le_remote_irk((T_LE_REMOTE_IRK *)key, p_entry->idx))
            {
                key_len = sizeof(T_LE_REMOTE_IRK);
            }
        }
        break;
    }
    BTM_PRINT_TRACE3("[BTBond] bt_le_get_key:idx %d, key_type %d, key_len %d", p_entry->idx, key_type,
                     key_len);
    return key_len;
}

bool bt_le_set_bond_state(T_LE_BOND_ENTRY *p_entry, uint8_t flags)
{
    if (p_entry->flags & LE_KEY_STORE_REMOTE_BD_BIT)
    {
        p_entry->bond_info.bond_state |= flags;
        BTM_PRINT_TRACE2("[BTBond] bt_le_set_bond_state: idx %d, bond_state 0x%x", p_entry->idx,
                         p_entry->bond_info.bond_state);
        if (ftl_save_le_remote_bd(&p_entry->bond_info, p_entry->idx) == 0)
        {
            return true;
        }
    }
    return false;
}

bool bt_le_clear_bond_state(T_LE_BOND_ENTRY *p_entry)
{
    if (p_entry->flags & LE_KEY_STORE_REMOTE_BD_BIT)
    {
        if (p_entry->bond_info.bond_state == 0)
        {
            return true;
        }
        p_entry->bond_info.bond_state = 0;
        BTM_PRINT_TRACE1("[BTBond] bt_le_clear_bond_state: idx %d", p_entry->idx);
        if (ftl_save_le_remote_bd(&p_entry->bond_info, p_entry->idx) == 0)
        {
            return true;
        }
    }
    return false;
}

bool bt_le_save_key(T_LE_BOND_ENTRY *p_entry, T_GAP_KEY_TYPE key_type, uint8_t key_length,
                    uint8_t *key)
{
    bool result = false;

    if (p_bt_bond_cb == NULL)
    {
        return false;
    }

    switch (key_type)
    {
    default:
        break;
    case GAP_KEY_LE_LOCAL_LTK:
        if ((p_bt_bond_cb->otp_bond_cfg.le_key_storage_flag & LE_KEY_STORE_LOCAL_LTK_BIT) == 0)
        {
            result = true;
        }
        else
        {
            T_LE_LOCAL_LTK local_ltk;
            memset(&local_ltk, 0, sizeof(T_LE_LOCAL_LTK));
            local_ltk.link_key_length = key_length;
            memcpy(local_ltk.key, key, key_length);
            if (ftl_save_le_local_ltk(&local_ltk, p_entry->idx, sizeof(T_LE_LOCAL_LTK)) == 0)
            {
                p_entry->flags |= LE_KEY_STORE_LOCAL_LTK_BIT;
                result = true;
            }
        }
        break;
    case GAP_KEY_LE_REMOTE_LTK:
        if ((p_bt_bond_cb->otp_bond_cfg.le_key_storage_flag & LE_KEY_STORE_REMOTE_LTK_BIT) == 0)
        {
            result = true;
        }
        else
        {
            T_LE_REMOTE_LTK remote_ltk;
            memset(&remote_ltk, 0, sizeof(T_LE_REMOTE_LTK));
            remote_ltk.link_key_length = key_length;
            memcpy(remote_ltk.key, key, key_length);
            if (ftl_save_le_remote_ltk(&remote_ltk, p_entry->idx, sizeof(T_LE_REMOTE_LTK)) == 0)
            {
                p_entry->flags |= LE_KEY_STORE_REMOTE_LTK_BIT;
                result = true;
            }
        }
        break;
    case GAP_KEY_LE_REMOTE_IRK:
        if ((p_bt_bond_cb->otp_bond_cfg.le_key_storage_flag & LE_KEY_STORE_REMOTE_IRK_BIT) == 0)
        {
            result = true;
        }
        else
        {
            T_LE_REMOTE_IRK *p_irk = (T_LE_REMOTE_IRK *)key;
            p_irk->key_exist = 1;
            if (ftl_save_le_remote_irk(p_irk, p_entry->idx) == 0)
            {
                memcpy(p_entry->remote_identity_addr, p_irk->addr, 6);
                p_entry->remote_identity_addr_type = p_irk->addr_type;
                p_entry->flags |= LE_KEY_STORE_REMOTE_IRK_BIT;
                result = true;
            }
        }
        break;
    case GAP_KEY_LE_LOCAL_IRK:
        {
            T_LE_LOCAL_IRK *p_irk = (T_LE_LOCAL_IRK *)key;
            p_entry->bond_info.local_bd_type = p_irk->addr_type;
            memcpy(p_entry->bond_info.local_bd, p_irk->addr, 6);
            p_entry->bond_info.bond_flag |= LE_BOND_EXIST_BIT;
            p_entry->bond_info.bond_flag |= LE_LOCAL_IRK_DIST_BIT;
            if (ftl_save_le_remote_bd(&p_entry->bond_info, p_entry->idx) == 0)
            {
                bt_bond_priority_queue_add(LE_BT_DEV, p_entry->idx);
                p_entry->flags |= LE_KEY_STORE_REMOTE_BD_BIT;
                p_entry->flags |= LE_KEY_STORE_LOCAL_IRK_BIT;
                result = true;
            }
        }
        break;

    }
    if (result)
    {
        if (!(p_entry->flags & LE_KEY_STORE_REMOTE_BD_BIT))
        {
            p_entry->bond_info.bond_flag |= LE_BOND_EXIST_BIT;
            if (p_entry->flags & LE_KEY_STORE_LOCAL_IRK_BIT)
            {
                p_entry->bond_info.bond_flag |= LE_LOCAL_IRK_DIST_BIT;
            }
            if (ftl_save_le_remote_bd(&p_entry->bond_info, p_entry->idx) == 0)
            {
                bt_bond_priority_queue_add(LE_BT_DEV, p_entry->idx);
                p_entry->flags |= LE_KEY_STORE_REMOTE_BD_BIT;
                result = true;
            }
        }
        else
        {
            bt_le_set_bond_state(p_entry, BT_BOND_INFO_NEED_UPDATE_FLAG);
        }
    }
    else
    {
    }
    BTM_PRINT_TRACE4("[BTBond] bt_le_save_key:idx %d, key_type %d, key_length %d result %d",
                     p_entry->idx,
                     key_type,
                     key_length, result);
    return result;
}

void bt_le_delete_key_entry(T_LE_BOND_ENTRY *p_entry)
{
    T_BT_BOND_INFO remote_bd;

    BTM_PRINT_TRACE1("[BTBond] bt_le_delete_key_entry:idx %d", p_entry->idx);
    memset(&remote_bd, 0, sizeof(T_BT_BOND_INFO));
    if (ftl_save_le_remote_bd(&remote_bd, p_entry->idx) != 0)
    {
        return;
    }

    if (p_entry->flags & LE_KEY_STORE_LOCAL_LTK_BIT)
    {
        T_LE_LOCAL_LTK local_ltk;
        local_ltk.link_key_length = 0;
        ftl_save_le_local_ltk(&local_ltk, p_entry->idx, 4);
    }
    if (p_entry->flags & LE_KEY_STORE_REMOTE_LTK_BIT)
    {
        T_LE_REMOTE_LTK remote_ltk;
        remote_ltk.link_key_length = 0;
        ftl_save_le_remote_ltk(&remote_ltk, p_entry->idx, 4);
    }
    if (p_entry->flags & LE_KEY_STORE_REMOTE_IRK_BIT)
    {
        ftl_clear_le_remote_irk(p_entry->idx);
    }
    bt_bond_remove(LE_BT_DEV, p_entry->idx);
    memset(p_entry, 0, sizeof(T_LE_BOND_ENTRY));
}

void bt_le_clear_all_keys(void)
{
    BTM_PRINT_INFO0("[BTBond] bt_le_clear_all_keys");
    uint8_t   i;
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return;
    }
    for (i = 0; i < le_bond_storage_num; i++)
    {
        T_LE_BOND_ENTRY *p_entry = &le_key_store_table[i];
        if (p_entry->is_used == true)
        {
            bt_le_delete_key_entry(p_entry);
        }
    }

    bt_bond_priority_queue_clear(LE_BT_DEV);
    T_BT_LE_BOND_CB_DATA cb_data;
    T_BT_LE_BOND_CLEAR bond_clear;
    bond_clear.modify_flags = 0;
    cb_data.p_le_bond_clear = &bond_clear;
    bt_bond_msg_post(BT_BOND_MSG_LE_BOND_CLEAR, &cb_data);
}

void bt_le_delete_bond(T_LE_BOND_ENTRY *p_entry)
{
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return;
    }
    if (p_entry != NULL)
    {
        T_BT_LE_BOND_CB_DATA cb_data;
        T_BT_LE_BOND_REMOVE bond_remove;
        bond_remove.p_entry = p_entry;
        bond_remove.modify_flags = 0;
        cb_data.p_le_bond_remove = &bond_remove;
        bt_bond_msg_post(BT_BOND_MSG_LE_BOND_REMOVE, &cb_data);
        bt_bond_priority_queue_delete(LE_BT_DEV, p_entry->idx);
        bt_le_delete_key_entry(p_entry);
    }
}

T_LE_BOND_ENTRY *bt_le_get_low_priority_bond(void)
{
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return NULL;
    }

    uint8_t idx = bt_bond_get_low_priority(LE_BT_DEV);
    if (idx < le_bond_storage_num)
    {
        T_LE_BOND_ENTRY *p_entry = &le_key_store_table[idx];
        return p_entry;
    }
    return NULL;
}

bool bt_le_get_cccd_data(T_LE_BOND_ENTRY *p_entry, T_BT_CCCD_DATA *p_data)
{
    if (p_entry != NULL)
    {
        return bt_bond_get_cccd_data(LE_BT_DEV, p_entry->idx, p_data);
    }
    return false;
}

T_LE_BOND_ENTRY *bt_le_find_entry_by_aes(uint8_t *unresolved_addr, uint8_t addr_type,
                                         uint8_t *local_bd, uint8_t local_bd_type)
{
    T_LE_BOND_ENTRY *p_entry;
    T_LE_REMOTE_IRK remote_irk;

    uint8_t unresolved_peer_addr[6];

    memcpy(unresolved_peer_addr, unresolved_addr, 6);

    /* BB2PLUSRD-584 */
    if ((addr_type == GAP_REMOTE_ADDR_LE_RANDOM) && ((unresolved_peer_addr[5] & 0xC0) == 0x80))
    {
        unresolved_peer_addr[5] &= 0x3F;
        unresolved_peer_addr[5] |= 0x40;
    }

    if ((addr_type == GAP_REMOTE_ADDR_LE_RANDOM)
        && ((unresolved_peer_addr[5] & 0xC0) == 0x40))
    {
        uint8_t i;
        bool local_addr_not_check = false;
        uint8_t identity_bd_addr[6];

        if (local_bd == NULL)
        {
            local_addr_not_check = true;
        }
        else
        {
            memcpy(identity_bd_addr, local_bd, 6);

            if (local_bd_type > GAP_LOCAL_ADDR_LE_RAP_OR_RAND)
            {
                local_addr_not_check = true;
            }
            else if (local_bd_type == GAP_LOCAL_ADDR_LE_RAP_OR_PUBLIC ||
                     local_bd_type == GAP_LOCAL_ADDR_LE_RAP_OR_RAND)
            {
                local_bd_type -= 2;
            }
            if ((local_bd_type == GAP_LOCAL_ADDR_LE_RANDOM) && ((local_bd[5] & 0xC0) == 0x40))
            {
                if (bt_le_resolve_local_rpa(local_bd, identity_bd_addr, &local_bd_type) == false)
                {
                    local_addr_not_check = true;
                }
            }
        }

        for (i = 0; i < le_bond_storage_num; i++)
        {
            p_entry = &le_key_store_table[i];

            if ((p_entry->is_used == true) &&
                (p_entry->flags & LE_KEY_STORE_REMOTE_IRK_BIT)
               )
            {
                uint8_t buffer[LE_ENCRYPT_DATA_LENGTH];
                uint8_t irk[LE_ENCRYPT_DATA_LENGTH];
                uint8_t encrypt_buffer[LE_ENCRYPT_DATA_LENGTH];
                uint8_t j;

                if (ftl_load_le_remote_irk(&remote_irk, p_entry->idx) != 0)
                {
                    continue;
                }
                memset(buffer, 0x00, LE_ENCRYPT_DATA_LENGTH);
                buffer[13] = unresolved_peer_addr[5];
                buffer[14] = unresolved_peer_addr[4];
                buffer[15] = unresolved_peer_addr[3];
                for (j = 0; j < LE_ENCRYPT_DATA_LENGTH; j++)
                {
                    irk[j] = remote_irk.key[LE_ENCRYPT_DATA_LENGTH - 1 - j];
                }

                aes128_ecb_encrypt(buffer, irk, encrypt_buffer);

                if ((encrypt_buffer[13] == unresolved_peer_addr[2])
                    && (encrypt_buffer[14] == unresolved_peer_addr[1])
                    && (encrypt_buffer[15] == unresolved_peer_addr[0]))
                {
                    if (local_addr_not_check || (p_entry->bond_info.local_bd_type == local_bd_type
                                                 && memcmp(p_entry->bond_info.local_bd, identity_bd_addr, 6) == 0))
                    {
                        BTM_PRINT_INFO1("[BTBond] bt_le_find_entry_by_aes: match IRK idx %d", p_entry->idx);
                        memcpy(p_entry->remote_bd, unresolved_peer_addr, 6);
                        p_entry->remote_bd_type = addr_type;
                        return p_entry;
                    }
                }
                else
                {
                    BTM_PRINT_INFO1("[BTBond] bt_le_find_entry_by_aes: not match idx %d", p_entry->idx);
                }
            }
        }
    }

    return NULL;
}

bool bt_le_resolve_random_address(uint8_t *unresolved_addr, uint8_t *resolved_addr,
                                  T_GAP_IDENT_ADDR_TYPE *resolved_addr_type)
{
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return false;
    }

    /* BB2PLUSRD-584 */
    if (((unresolved_addr[5] & 0xC0) == 0x40) || ((unresolved_addr[5] & 0xC0) == 0x80))
    {
        T_LE_BOND_ENTRY *p_entry;
        p_entry = bt_le_find_key_entry(unresolved_addr, GAP_REMOTE_ADDR_LE_RANDOM, NULL,
                                       LE_BOND_LOCAL_ADDRESS_ANY);
        if (p_entry == NULL)
        {
            p_entry = bt_le_find_entry_by_aes(unresolved_addr, GAP_REMOTE_ADDR_LE_RANDOM, NULL,
                                              LE_BOND_LOCAL_ADDRESS_ANY);
        }
        if (p_entry == NULL)
        {
            BTM_PRINT_ERROR0("[BTBond] bt_le_resolve_random_address: not find entry");
            return false;
        }
        if (p_entry->flags & LE_KEY_STORE_REMOTE_IRK_BIT)
        {
            memcpy(resolved_addr, p_entry->remote_identity_addr, 6);
            *resolved_addr_type = (T_GAP_IDENT_ADDR_TYPE)p_entry->remote_identity_addr_type;
            BTM_PRINT_TRACE2("[BTBond] bt_le_resolve_random_address: resolved_addr %s, resolved_addr_type %d",
                             TRACE_BDADDR(resolved_addr), p_entry->remote_identity_addr_type);
            return true;
        }
        else
        {
            BTM_PRINT_ERROR0("[BTBond] bt_le_resolve_random_address: not find entry");
            return false;
        }

    }
    else
    {
        BTM_PRINT_ERROR0("[BTBond] bt_le_resolve_random_address: not resolved random addr");
        return false;
    }

}

uint8_t bt_le_get_max_le_paired_device_num(void)
{
    return le_bond_storage_num;
}

bool bt_le_get_dev_ltk(T_LE_BOND_ENTRY *p_entry, bool remote, uint8_t *p_key_len, uint8_t *p_ltk)
{
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return false;
    }

    T_LE_LTK_DATA ltk_data;
    if (p_entry == NULL || p_key_len == NULL || p_ltk == NULL)
    {
        goto failed;
    }
    if (remote)
    {
        if (p_entry->flags & LE_KEY_STORE_REMOTE_LTK_BIT)
        {
            T_LE_REMOTE_LTK remote_ltk;
            if (ftl_load_le_remote_ltk(&remote_ltk, p_entry->idx) == 0)
            {
                if (remote_ltk.link_key_length != 0)
                {
                    memcpy(&ltk_data, remote_ltk.key, sizeof(T_LE_LTK_DATA));
                    goto success;
                }
            }
        }
    }
    else
    {
        if (p_entry->flags & LE_KEY_STORE_LOCAL_LTK_BIT)
        {
            T_LE_LOCAL_LTK local_ltk;
            if (ftl_load_le_local_ltk(&local_ltk, p_entry->idx) == 0)
            {
                if (local_ltk.link_key_length != 0)
                {
                    memcpy(&ltk_data, local_ltk.key, sizeof(T_LE_LTK_DATA));
                    goto success;
                }
            }
        }
    }
failed:
    return false;
success:
    *p_key_len = ltk_data.key_size;
    memcpy(p_ltk, ltk_data.key, 16);
    return true;
}

bool bt_le_set_dev_ltk(T_LE_BOND_ENTRY *p_entry, bool remote, uint8_t key_len,
                       uint8_t *p_ltk)
{
    T_LE_LTK_DATA ltk_data = {0};
    T_GAP_KEY_TYPE key_type = GAP_KEY_LE_LOCAL_LTK;

    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return false;
    }
    if (p_entry == NULL || p_ltk == NULL ||
        key_len < 7 || key_len > 16)
    {
        return false;
    }
    if (remote)
    {
        key_type = GAP_KEY_LE_REMOTE_LTK;
    }
    if (bt_le_get_key(p_entry, key_type, (uint8_t *)&ltk_data) != 0)
    {
        ltk_data.key_size = key_len;
        memcpy(ltk_data.key, p_ltk, key_len);

        if (bt_le_save_key(p_entry, key_type, sizeof(T_LE_LTK_DATA), (uint8_t *)&ltk_data))
        {
            BTM_PRINT_INFO0("[BTBond] bt_le_set_dev_ltk: update success");
            return true;
        }
    }
    return false;
}

bool bt_le_get_dev_irk(T_LE_BOND_ENTRY *p_entry, bool remote, uint8_t *p_irk)
{
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return false;
    }

    if (p_entry == NULL || p_irk == NULL)
    {
        goto failed;
    }
    if (remote)
    {
        if (p_entry->flags & LE_KEY_STORE_REMOTE_IRK_BIT)
        {
            T_LE_REMOTE_IRK remote_irk;
            if (ftl_load_le_remote_irk(&remote_irk, p_entry->idx) == 0)
            {
                if (remote_irk.key_exist != 0)
                {
                    memcpy(p_irk, remote_irk.key, 16);
                    return true;
                }
            }
        }
    }
    else
    {
        if (p_entry->bond_info.bond_flag & LE_LOCAL_IRK_DIST_BIT)
        {
            return bt_le_get_local_irk(p_entry, p_irk);
        }
    }
failed:
    return false;
}

void bt_le_dev_info_clear(void)
{
    BTM_PRINT_INFO0("[BTBond] bt_le_dev_info_clear");
    uint8_t   i;

    for (i = 0; i < le_bond_storage_num; i++)
    {
        T_LE_BOND_ENTRY *p_entry = &le_key_store_table[i];
        if (p_entry->is_used == true)
        {
            bt_le_delete_key_entry(p_entry);
        }
    }

    bt_bond_priority_queue_clear(LE_BT_DEV);
    T_BT_LE_BOND_CB_DATA cb_data;
    T_BT_LE_BOND_CLEAR bond_clear;
    bond_clear.modify_flags = LE_BOND_MODIFY_REMOTE_SYNC;
    cb_data.p_le_bond_clear = &bond_clear;
    bt_bond_msg_post(BT_BOND_MSG_LE_BOND_CLEAR, &cb_data);
}

bool bt_le_dev_info_cmp(T_BT_LE_DEV_DATA *p_info1, T_BT_LE_DEV_DATA *p_info2)
{
    //uint8_t idx = 0;
    if (p_info1->bond_info.remote_bd_type != p_info2->bond_info.remote_bd_type ||
        p_info1->bond_info.local_bd_type != p_info2->bond_info.local_bd_type ||
        p_info1->bond_info.bond_flag != p_info2->bond_info.bond_flag ||
        memcmp(p_info1->bond_info.remote_bd, p_info2->bond_info.remote_bd, 6) != 0 ||
        memcmp(p_info1->bond_info.local_bd, p_info2->bond_info.local_bd, 6) != 0)
    {
        // idx = 1;
        goto no_equal;
    }
    if (p_info1->dev_info.flags != p_info2->dev_info.flags ||
        p_info1->dev_info.cccd_len != p_info2->dev_info.cccd_len)
    {
        //idx = 2;
        goto no_equal;
    }
    if (memcmp(&p_info1->dev_info, &p_info2->dev_info,
               (sizeof(T_BT_LE_DEV_INFO) + p_info1->dev_info.cccd_len)) != 0)
    {
        //idx = 3;
        goto no_equal;
    }
    return true;
no_equal:
    //BTM_PRINT_TRACE1("[BTBond] bt_le_dev_info_cmp: not equal %d", idx);
    return false;
}

bool bt_le_dev_info_get(T_LE_BOND_ENTRY *p_entry, T_BT_LE_DEV_DATA **pp_info, uint16_t *p_info_len)
{
    T_BT_LE_DEV_DATA *p_info = NULL;
    T_BT_LE_DEV_INFO *p_dev_info = NULL;
    T_BT_CCCD_DATA *p_cccd;
    uint16_t data_len;
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        goto failed;
    }

    if (p_entry == NULL || pp_info == NULL || p_info_len == NULL)
    {
        goto failed;
    }
    data_len = ftl_read_cccd_length(LE_BT_DEV, p_entry->idx);
    data_len += 4;
    data_len += sizeof(T_BT_LE_DEV_DATA);
    p_info = calloc(1, data_len);
    if (p_info == NULL)
    {
        goto failed;
    }
    memcpy(&p_info->bond_info, &p_entry->bond_info, sizeof(T_BT_BOND_INFO));
    p_dev_info = &p_info->dev_info;
    p_dev_info->flags = p_entry->flags;
    if (p_entry->flags & LE_KEY_STORE_LOCAL_LTK_BIT)
    {
        if (ftl_load_le_local_ltk((T_LE_LOCAL_LTK *)p_dev_info->local_ltk, p_entry->idx) != 0)
        {
            goto failed;
        }
    }
    if (p_entry->flags & LE_KEY_STORE_REMOTE_LTK_BIT)
    {
        if (ftl_load_le_remote_ltk((T_LE_REMOTE_LTK *)p_dev_info->remote_ltk, p_entry->idx) != 0)
        {
            goto failed;
        }
    }
    if (p_entry->flags & LE_KEY_STORE_REMOTE_IRK_BIT)
    {
        if (ftl_load_le_remote_irk((T_LE_REMOTE_IRK *)p_dev_info->remote_irk, p_entry->idx) != 0)
        {
            goto failed;
        }
    }
#if F_BT_LE_5_1_CLIENT_SUPPORTED_FEATURES_SERVER_SUPPORT
    if (p_bt_bond_cb->otp_bond_cfg.use_gatts_client_supported_features)
    {
        T_REMOTE_CLIENT_SUPPORTED_FEATURES remote_client_supp_feats;
        if (ftl_load_remote_client_supported_features(LE_BT_DEV, &remote_client_supp_feats,
                                                      p_entry->idx) == 0)
        {
            if (remote_client_supp_feats.length != 0)
            {
                memcpy(p_dev_info->client_supported_feature, &remote_client_supp_feats,
                       sizeof(T_REMOTE_CLIENT_SUPPORTED_FEATURES));
                p_dev_info->flags |= LE_KEY_STORE_REMOTE_CLIENT_SUPPORTED_FEATURES_BIT;
            }
        }
    }
#endif
    p_cccd = (T_BT_CCCD_DATA *)p_dev_info->cccd;
    if (bt_bond_get_cccd_data(LE_BT_DEV, p_entry->idx, p_cccd))
    {
        if (p_cccd->data_length != 0)
        {
            p_dev_info->cccd_len = p_cccd->data_length + 4;
            p_dev_info->flags |= LE_KEY_STORE_CCCD_DATA_BIT;
        }
    }
    else
    {
        p_dev_info->cccd_len = 4;
    }
    *pp_info = p_info;
    *p_info_len = data_len;
    return true;
failed:
    if (p_info)
    {
        free(p_info);
    }
    return false;
}

bool bt_le_dev_info_get_local_ltk(T_LE_BOND_ENTRY *p_entry, uint8_t *p_key)
{
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return false;
    }
    if (p_entry == NULL || p_key == NULL)
    {
        return false;
    }
    if (p_entry->flags & LE_KEY_STORE_LOCAL_LTK_BIT)
    {
        if (ftl_load_le_local_ltk((T_LE_LOCAL_LTK *)p_key, p_entry->idx) == 0)
        {
            return true;
        }
    }
    return false;
}
T_LE_BOND_ENTRY *bt_le_dev_info_allocate(T_BT_BOND_INFO *p_bond_info)
{
    uint8_t i;
    T_LE_BOND_ENTRY *p_entry = NULL;
    for (i = 0; i < le_bond_storage_num; i++)
    {
        if (le_key_store_table[i].is_used == false)
        {
            p_entry = &le_key_store_table[i];
            break;
        }
    }

    if (p_entry == NULL)
    {
        if (bt_bond_proc_flag & BT_BOND_PROC_BIT_SEND_BOND_FULL)
        {
            T_BT_LE_BOND_CB_DATA cb_data;
            T_BT_LE_BOND_FULL bond_full = {0};
            cb_data.p_le_bond_full = &bond_full;
            bt_bond_msg_post(BT_BOND_MSG_LE_BOND_FULL, &cb_data);
            for (i = 0; i < le_bond_storage_num; i++)
            {
                if (le_key_store_table[i].is_used == false)
                {
                    p_entry = &le_key_store_table[i];
                    break;
                }
            }
        }
        else
        {
            p_entry = bt_le_get_low_priority_bond();
            if (p_entry != NULL)
            {
                i = p_entry->idx;
                bt_le_delete_bond(p_entry);
            }
        }
    }

    if (p_entry)
    {
        p_entry->idx = i;
        p_entry->is_used = true;
        p_entry->flags = 0;
        memcpy(&p_entry->bond_info, p_bond_info, sizeof(T_BT_BOND_INFO));
        p_entry->remote_bd_type = p_bond_info->remote_bd_type;
        memcpy(p_entry->remote_bd, p_bond_info->remote_bd, 6);
        bt_le_update_identity_addr(p_entry);
        BTM_PRINT_TRACE1("[BTBond] bt_le_dev_info_allocate: idx %d", p_entry->idx);
    }
    return p_entry;
}

T_LE_BOND_ENTRY *bt_le_dev_info_pre_set(T_BT_BOND_INFO *p_bond_info, uint8_t *p_key)
{
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return NULL;
    }

    T_LE_BOND_ENTRY *p_entry = NULL;
    if (p_bond_info == NULL || p_key == NULL)
    {
        return NULL;
    }
    p_entry = bt_le_dev_info_find(p_bond_info);

    if (p_entry == NULL)
    {
        p_entry = bt_le_dev_info_allocate(p_bond_info);
        if (p_entry == NULL)
        {
            return NULL;
        }
    }
    if (p_bond_info->bond_flag & LE_LOCAL_IRK_DIST_BIT)
    {
        p_entry->flags |= LE_KEY_STORE_LOCAL_IRK_BIT;
    }
    T_LE_LOCAL_LTK *p_ltk = (T_LE_LOCAL_LTK *)p_key;
    bt_le_save_key(p_entry, GAP_KEY_LE_LOCAL_LTK, p_ltk->link_key_length, p_ltk->key);
    bt_bond_set_high_priority(LE_BT_DEV, p_entry->idx);
    bt_le_clear_bond_state(p_entry);
    p_entry->modify_flags = 0;
    return p_entry;
}

bool bt_le_dev_info_set_cccd(T_LE_BOND_ENTRY *p_entry, T_BT_CCCD_DATA *p_cccd)
{
    if (ftl_save_cccd(LE_BT_DEV, p_cccd, p_entry->idx,
                      (4 + p_cccd->data_length)) == 0)
    {
        return true;
    }
    return false;
}

T_LE_BOND_ENTRY *bt_le_dev_info_set(T_BT_BOND_INFO *p_bond_info, T_BT_LE_DEV_INFO *p_info)
{
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return NULL;
    }

    T_LE_BOND_ENTRY *p_entry = NULL;
    bool bond_exist = true;
    bool update_entry = false;
    if (p_bond_info == NULL || p_info == NULL)
    {
        return NULL;
    }
    p_entry = bt_le_dev_info_find(p_bond_info);

    if (p_entry == NULL)
    {
        bond_exist = false;
        p_entry = bt_le_dev_info_allocate(p_bond_info);
        if (p_entry == NULL)
        {
            return NULL;
        }
    }
    if (p_bond_info->bond_flag & LE_LOCAL_IRK_DIST_BIT)
    {
        p_entry->flags |= LE_KEY_STORE_LOCAL_IRK_BIT;
    }
    if (p_info->flags & LE_KEY_STORE_LOCAL_LTK_BIT)
    {
        bool update = true;
        if (bond_exist)
        {
            T_LE_LOCAL_LTK local_ltk;
            if (ftl_load_le_local_ltk(&local_ltk, p_entry->idx) == 0)
            {
                if (memcmp(p_info->local_ltk, &local_ltk, sizeof(T_LE_LOCAL_LTK)) == 0)
                {
                    update = false;
                }
            }
        }
        if (update)
        {
            T_LE_LOCAL_LTK *p_ltk = (T_LE_LOCAL_LTK *)p_info->local_ltk;
            update_entry = true;
            bt_le_save_key(p_entry, GAP_KEY_LE_LOCAL_LTK, p_ltk->link_key_length, p_ltk->key);
        }
    }
    if (p_info->flags & LE_KEY_STORE_REMOTE_LTK_BIT)
    {
        bool update = true;
        if (bond_exist)
        {
            T_LE_REMOTE_LTK remote_ltk;
            if (ftl_load_le_remote_ltk(&remote_ltk, p_entry->idx) == 0)
            {
                if (memcmp(p_info->remote_ltk, &remote_ltk, sizeof(T_LE_REMOTE_LTK)) == 0)
                {
                    update = false;
                }
            }
        }
        if (update)
        {
            T_LE_REMOTE_LTK *p_ltk = (T_LE_REMOTE_LTK *)p_info->remote_ltk;
            update_entry = true;
            bt_le_save_key(p_entry, GAP_KEY_LE_REMOTE_LTK, p_ltk->link_key_length, p_ltk->key);
        }
    }
    if (p_info->flags & LE_KEY_STORE_REMOTE_IRK_BIT)
    {
        bool update = true;
        if (bond_exist)
        {
            T_LE_REMOTE_IRK remote_irk;
            if (ftl_load_le_remote_irk(&remote_irk, p_entry->idx) == 0)
            {
                if (memcmp(p_info->remote_irk, &remote_irk, sizeof(T_LE_REMOTE_IRK)) == 0)
                {
                    update = false;
                }
            }
        }
        if (update)
        {
            update_entry = true;
            bt_le_save_key(p_entry, GAP_KEY_LE_REMOTE_IRK, sizeof(T_LE_REMOTE_IRK), p_info->remote_irk);
        }
    }
    if (p_info->flags & LE_KEY_STORE_CCCD_DATA_BIT)
    {
        T_BT_CCCD_DATA *p_new_cccd = (T_BT_CCCD_DATA *)p_info->cccd;
        bt_bond_set_cccd_data(LE_BT_DEV, p_entry->idx, p_new_cccd->data_length, p_new_cccd->data, false);
    }
#if F_BT_LE_5_1_CLIENT_SUPPORTED_FEATURES_SERVER_SUPPORT
    if (p_info->flags & LE_KEY_STORE_REMOTE_CLIENT_SUPPORTED_FEATURES_BIT)
    {
        ftl_save_remote_client_supported_features(LE_BT_DEV,
                                                  (T_REMOTE_CLIENT_SUPPORTED_FEATURES *)p_info->client_supported_feature,
                                                  p_entry->idx, sizeof(T_REMOTE_CLIENT_SUPPORTED_FEATURES));
    }
#endif
    if (update_entry)
    {
        T_BT_LE_BOND_CB_DATA cb_data;
        T_BT_LE_BOND_ADD bond_add;
        bond_add.modify_flags = LE_BOND_MODIFY_REMOTE_SYNC;
        bond_add.p_entry = p_entry;
        cb_data.p_le_bond_add = &bond_add;
        bt_bond_msg_post(BT_BOND_MSG_LE_BOND_ADD, &cb_data);
    }
    bt_le_clear_bond_state(p_entry);
    p_entry->modify_flags = 0;
    return p_entry;
}

T_LE_BOND_ENTRY *bt_le_dev_info_find(T_BT_BOND_INFO *p_info)
{
    uint8_t   i;
    T_LE_BOND_ENTRY *p_entry;
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return NULL;
    }
    for (i = 0; i < le_bond_storage_num; i++)
    {
        p_entry = &le_key_store_table[i];
        if ((p_entry->is_used == true) &&
            (memcmp(p_entry->bond_info.remote_bd, p_info->remote_bd, 6) == 0) &&
            (memcmp(p_entry->bond_info.local_bd, p_info->local_bd, 6) == 0) &&
            p_entry->bond_info.remote_bd_type ==  p_info->remote_bd_type &&
            p_entry->bond_info.local_bd_type ==  p_info->local_bd_type
           )
        {
            BTM_PRINT_TRACE1("[BTBond] bt_le_dev_info_find: find %d", i);
            return p_entry;
        }
    }

    BTM_PRINT_ERROR2("[BTBond] bt_le_dev_info_find: not find, bd_addr %s, bd_type %d",
                     TRACE_BDADDR(p_info->remote_bd),
                     p_info->remote_bd_type);
    return NULL;
}

bool bt_le_dev_info_remove(T_BT_BOND_INFO *p_info)
{
    T_LE_BOND_ENTRY *p_entry = bt_le_dev_info_find(p_info);
    if (p_entry)
    {
        T_BT_LE_BOND_CB_DATA cb_data;
        T_BT_LE_BOND_REMOVE bond_remove;
        bond_remove.p_entry = p_entry;
        bond_remove.modify_flags = LE_BOND_MODIFY_REMOTE_SYNC;
        cb_data.p_le_bond_remove = &bond_remove;
        bt_bond_msg_post(BT_BOND_MSG_LE_BOND_REMOVE, &cb_data);
        bt_bond_priority_queue_delete(LE_BT_DEV, p_entry->idx);
        bt_le_delete_key_entry(p_entry);
        return true;
    }
    return false;
}

bool bt_le_dev_info_set_high_priority(T_BT_BOND_INFO *p_info)
{
    T_LE_BOND_ENTRY *p_entry = bt_le_dev_info_find(p_info);
    if (p_entry)
    {
        return bt_bond_set_high_priority(LE_BT_DEV, p_entry->idx);
    }
    return false;
}

bool bt_le_set_bond_flag(T_LE_BOND_ENTRY *p_entry, uint16_t flag)
{
    if (p_entry != NULL)
    {
        uint32_t bond_flag = flag << 16;
        bond_flag |= (p_entry->bond_info.bond_flag & 0x0000FFFF);
        if (p_entry->bond_info.bond_flag != bond_flag)
        {
            p_entry->bond_info.bond_flag = bond_flag;
            bt_le_set_bond_state(p_entry, BT_BOND_INFO_NEED_UPDATE_FLAG);
            p_entry->modify_flags |= LE_BOND_MODIFY_FLAG;
            bt_le_key_send_update_info(p_entry);
        }
        return true;
    }
    return false;
}

bool bt_le_get_bond_flag(T_LE_BOND_ENTRY *p_entry, uint16_t *p_flag)
{
    if (p_entry != NULL && p_flag != NULL)
    {
        uint16_t flag = p_entry->bond_info.bond_flag >> 16;
        *p_flag = flag;
        return true;
    }
    return false;
}

#if F_APP_BOND_MGR_DEBUG
void bt_le_bond_print_bond_info(T_BT_BOND_INFO *p_bond_info)
{
    BTM_PRINT_TRACE7("[BTBond] bond info: bond_state 0x%x, key_type %d, remote bd 0x%x [%s], local bd 0x%x [%s], bond_flag 0x%x",
                     p_bond_info->bond_state,
                     p_bond_info->key_type,
                     p_bond_info->remote_bd_type,
                     TRACE_BDADDR(p_bond_info->remote_bd),
                     p_bond_info->local_bd_type,
                     TRACE_BDADDR(p_bond_info->local_bd),
                     p_bond_info->bond_flag);
}

void bt_le_bond_print_dev_info(T_LE_BOND_ENTRY *p_entry)
{
    T_BT_LE_DEV_DATA *p_info = NULL;
    uint16_t info_len;
    if (bt_le_dev_info_get(p_entry, &p_info, &info_len))
    {
        bt_le_bond_print_bond_info(&p_info->bond_info);
        BTM_PRINT_TRACE1("[BTBond] dev info: flags 0x%x",
                         p_info->dev_info.flags);
    }
    else
    {
        BTM_PRINT_TRACE0("[BTBond] bt_le_bond_print_dev_info: failed");
    }
    if (p_info)
    {
        free(p_info);
    }
    return;
}

void bt_le_bond_print(uint8_t param)
{
    if (param == 0)
    {
        T_LE_BOND_ENTRY *p_entry;
        for (uint8_t i = 0; i < le_bond_storage_num; i++)
        {
            p_entry = &le_key_store_table[i];
            if (p_entry->is_used)
            {
                BTM_PRINT_TRACE7("[BTBond] T_LE_BOND_ENTRY[%d]: flags 0x%x, modify_flags 0x%x, remote bd 0x%x [%s], identity bd 0x%x [%s]",
                                 i, p_entry->flags, p_entry->modify_flags,
                                 p_entry->remote_bd_type,
                                 TRACE_BDADDR(p_entry->remote_bd),
                                 p_entry->remote_identity_addr_type,
                                 TRACE_BDADDR(p_entry->remote_identity_addr));
                bt_le_bond_print_bond_info(&p_entry->bond_info);
            }
            else
            {
                BTM_PRINT_TRACE1("[BTBond] T_LE_BOND_ENTRY[%d]: not used", i);
            }
        }
    }
    else if (param == 1)
    {
        T_LE_BOND_ENTRY *p_entry = NULL;
        uint8_t bond_num = bt_le_get_bond_dev_num();
        for (uint8_t i = 1; i <= bond_num; i++)
        {
            p_entry = bt_le_find_key_entry_by_priority(i);
            if (p_entry)
            {
                BTM_PRINT_TRACE2("[BTBond] Device information priority %d, idx %d", i, p_entry->idx);
                bt_le_bond_print_dev_info(p_entry);
            }
            else
            {
                BTM_PRINT_TRACE1("[BTBond] Device information priority %d: get failed", i);
            }
        }
    }
    else if (param == 2)
    {
        uint8_t local_bond_num = bt_le_get_bond_dev_num();
        for (uint32_t i = 0; i < local_bond_num; i++)
        {
            T_LE_BOND_ENTRY *p_entry = NULL;
            p_entry = bt_le_find_key_entry_by_priority(i + 1);
            if (p_entry != NULL)
            {
                BTM_PRINT_TRACE8("[BTBond] T_LE_BOND_ENTRY: priority %d, idx %d, flags 0x%x, modify_flags 0x%x, remote bd 0x%x [%s], identity bd 0x%x [%s]",
                                 i + 1, p_entry->idx, p_entry->flags, p_entry->modify_flags,
                                 p_entry->remote_bd_type,
                                 TRACE_BDADDR(p_entry->remote_bd),
                                 p_entry->remote_identity_addr_type,
                                 TRACE_BDADDR(p_entry->remote_identity_addr));
                bt_le_bond_print_bond_info(&p_entry->bond_info);
            }
        }
    }
}
#endif

#if BLE_MGR_DEINIT
void bt_le_key_deinit(void)
{
    if (le_key_store_table)
    {
        le_bond_storage_num = 0;
        free(le_key_store_table);
        le_key_store_table = NULL;
    }
    T_BT_LE_LOCAL_IRK *p_local_irk;
    while ((p_local_irk = os_queue_out(&bt_le_local_irk_queue)) != NULL)
    {
        free(p_local_irk);
    }
}
#endif

#endif
#endif

