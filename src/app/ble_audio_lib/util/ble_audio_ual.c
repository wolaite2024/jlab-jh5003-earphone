#include <string.h>
#include "trace.h"
#include "ble_link_util.h"

bool ble_audio_ual_check_le_bond(uint16_t conn_handle)
{
    if (ble_audio_db.bond_mgr && ble_audio_db.bond_mgr->bond_check)
    {
        T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
        if (p_link)
        {
            return ble_audio_db.bond_mgr->bond_check(p_link->remote_bd, p_link->remote_bd_type, conn_handle);
        }
    }

    return false;
}

bool ble_audio_ual_resolve_rpa(uint8_t *unresolved_addr, uint8_t *resolved_addr,
                               uint8_t *resolved_addr_type)
{
    if (ble_audio_db.bond_mgr && ble_audio_db.bond_mgr->bond_le_resolve_rpa)
    {
        return ble_audio_db.bond_mgr->bond_le_resolve_rpa(unresolved_addr, resolved_addr,
                                                          resolved_addr_type);
    }
    return false;
}

void ble_audio_ual_set_pending_link_cccd(T_BLE_AUDIO_LINK *p_link, uint16_t cccd_handle)
{
    if (ble_audio_db.bond_mgr && ble_audio_db.bond_mgr->bond_get_max_num &&
        ble_audio_db.bond_mgr->bond_get_addr &&
        ble_audio_db.bond_mgr->bond_set_cccd_flag)
    {
        uint8_t bond_storage_num = ble_audio_db.bond_mgr->bond_get_max_num(true);
        uint8_t bd_addr[6];
        uint8_t bd_type;
        uint8_t local_bd_addr[6];
        uint8_t local_bd_type;
        for (uint8_t i = 0; i < bond_storage_num; i++)
        {
            if (ble_audio_db.bond_mgr->bond_get_addr(true, i, bd_addr, &bd_type, local_bd_addr, &local_bd_type))
            {
                T_BLE_AUDIO_LINK *p_temp_link = ble_audio_link_find_by_addr(bd_addr, bd_type);
                if (p_temp_link == p_link)
                {
                    PROTOCOL_PRINT_INFO2("ble_audio_ual_set_pending_cccd: cccd_handle 0x%x, idx %d", cccd_handle, i);
                    ble_audio_db.bond_mgr->bond_set_cccd_flag(bd_addr, bd_type, local_bd_addr, local_bd_type,
                                                              cccd_handle, GATT_CLIENT_CHAR_NOTIF_IND_DATA_PENGDING);
                }
            }
        }
    }
}

void ble_audio_ual_set_pending_cccd(uint16_t cccd_handle)
{
    ble_audio_ual_set_pending_link_cccd(NULL, cccd_handle);
}

bool ble_audio_ual_clear_pending_cccd(uint16_t conn_handle, uint16_t cccd_handle)
{
    if (ble_audio_db.bond_mgr && ble_audio_db.bond_mgr->bond_check && cccd_handle != 0 &&
        ble_audio_db.bond_mgr->bond_clear_cccd_flag)
    {
        T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
        if (p_link)
        {
            if (ble_audio_db.bond_mgr->bond_check(p_link->remote_bd, p_link->remote_bd_type, conn_handle))
            {
                PROTOCOL_PRINT_INFO1("ble_audio_ual_clear_pending_cccd: cccd_handle 0x%x", cccd_handle);
                return ble_audio_db.bond_mgr->bond_clear_cccd_flag(p_link->remote_bd,
                                                                   p_link->remote_bd_type, conn_handle, cccd_handle);
            }
        }
    }

    return false;
}

bool ble_audio_ual_check_addr(uint8_t *bd_addr1, uint8_t bd_type1,
                              uint8_t *bd_addr2, uint8_t bd_type2)
{
    if (bd_type1 == bd_type2 && memcmp(bd_addr1, bd_addr2, GAP_BD_ADDR_LEN) == 0)
    {
        return true;
    }
    else
    {
        if (bd_type1 == bd_type2 && bd_type1 == GAP_REMOTE_ADDR_LE_RANDOM)
        {
            uint8_t resolved_addr1[6];
            T_GAP_IDENT_ADDR_TYPE resolved_addr_type1;
            uint8_t resolved_addr2[6];
            T_GAP_IDENT_ADDR_TYPE resolved_addr_type2;
            if (ble_audio_ual_resolve_rpa(bd_addr1, resolved_addr1, &resolved_addr_type1))
            {
                if (ble_audio_ual_resolve_rpa(bd_addr2, resolved_addr2, &resolved_addr_type2))
                {
                    if (resolved_addr_type1 == resolved_addr_type2 &&
                        memcmp(resolved_addr1, resolved_addr2, GAP_BD_ADDR_LEN) == 0)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool ble_audio_ual_get_le_ltk(uint8_t *remote_bd, uint8_t remote_bd_type, uint16_t conn_handle,
                              bool remote, uint8_t *p_key_len, uint8_t *p_ltk)
{
    if (ble_audio_db.bond_mgr && ble_audio_db.bond_mgr->bond_get_key)
    {
        return ble_audio_db.bond_mgr->bond_get_key(remote_bd, remote_bd_type, conn_handle, remote,
                                                   p_key_len, p_ltk);
    }
    return false;
}


