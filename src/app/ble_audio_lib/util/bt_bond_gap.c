#include <string.h>
#include "gatt.h"
#include "bt_bond_mgr.h"
#include "gap_storage_le.h"
#include "gap_storage_br.h"

extern bool gap_bond_set_cccd_data_pending(uint8_t *bd_addr, uint8_t bd_type, uint16_t handle,
                                           bool data_pending);

bool gap_bond_check(uint8_t *bd_addr, uint8_t bd_type, uint16_t conn_handle)
{
    T_LE_KEY_ENTRY *p_key_entry;
    p_key_entry = le_find_key_entry(bd_addr, (T_GAP_REMOTE_ADDR_TYPE)bd_type);
    if (p_key_entry)
    {
        return true;
    }

    return false;
}

bool gap_bond_get_key(uint8_t *bd_addr, uint8_t bd_type, uint16_t conn_handle,
                      bool remote, uint8_t *p_key_len, uint8_t *p_key)
{
    T_LE_KEY_ENTRY *p_entry;
    if (bd_addr == NULL || p_key_len == NULL || p_key == NULL)
    {
        return false;
    }
    p_entry = le_find_key_entry(bd_addr, (T_GAP_REMOTE_ADDR_TYPE)bd_type);
    if (p_entry)
    {
        if ((remote == true && (p_entry->flags & LE_KEY_STORE_REMOTE_LTK_BIT)) ||
            (remote == false && (p_entry->flags & LE_KEY_STORE_LOCAL_LTK_BIT)))
        {
            if (le_get_dev_ltk(p_entry, remote, p_key_len, p_key))
            {
                return true;
            }
        }
    }
    return false;
}

uint8_t gap_bond_get_max_num(bool is_le)
{
    if (is_le)
    {
        return le_get_max_le_paired_device_num();
    }
    else
    {
        return gap_br_get_max_bond_num();
    }
}

bool gap_bond_get_addr(bool is_le, uint8_t bond_idx, uint8_t *bd_addr, uint8_t *p_bd_type,
                       uint8_t *local_bd_addr, uint8_t *p_local_bd_type)
{
    if (bd_addr == NULL || p_bd_type == NULL)
    {
        return false;
    }
    if (is_le)
    {
        T_LE_KEY_ENTRY *p_key_entry;
        p_key_entry = le_find_key_entry_by_idx(bond_idx);
        if (p_key_entry != NULL)
        {
            memcpy(bd_addr, p_key_entry->remote_bd.addr, 6);
            *p_bd_type = p_key_entry->remote_bd.remote_bd_type;
            return true;
        }
    }
    return false;
}

bool gap_bond_set_cccd_flag(uint8_t *bd_addr, uint8_t bd_type, uint8_t *local_bd_addr,
                            uint8_t local_bd_type,
                            uint16_t cccd_handle, uint16_t flags)
{
    bool data_pending = false;
    if (flags & GATT_CLIENT_CHAR_NOTIF_IND_DATA_PENGDING)
    {
        data_pending = true;
    }
    return gap_bond_set_cccd_data_pending(bd_addr, bd_type, cccd_handle, data_pending);
}

bool gap_bond_clear_cccd_flag(uint8_t *bd_addr, uint8_t bd_type, uint16_t conn_handle,
                              uint16_t cccd_handle)
{
    return gap_bond_set_cccd_data_pending(bd_addr, bd_type, cccd_handle, false);
}

bool gap_bond_le_resolve_rpa(uint8_t *unresolved_addr, uint8_t *identity_addr,
                             uint8_t *p_identity_addr_type)
{
    return le_resolve_random_address(unresolved_addr, identity_addr, p_identity_addr_type);
}

const T_BT_BOND_MGR bt_bond_gap =
{
    .bond_check             = gap_bond_check,
    .bond_get_key           = gap_bond_get_key,
    .bond_get_max_num       = gap_bond_get_max_num,
    .bond_get_addr          = gap_bond_get_addr,
    .bond_set_cccd_flag     = gap_bond_set_cccd_flag,
    .bond_clear_cccd_flag   = gap_bond_clear_cccd_flag,
    .bond_le_resolve_rpa    = gap_bond_le_resolve_rpa,
};
