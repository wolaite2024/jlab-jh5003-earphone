#include <string.h>
#include "trace.h"
#include "ble_link_util.h"
#include "ble_audio_ual.h"
#include "ble_audio_mgr.h"

T_BLE_AUDIO_DB ble_audio_db;

T_BLE_AUDIO_LINK *ble_audio_link_find_by_conn_id(uint8_t conn_id)
{
    T_BLE_AUDIO_LINK *p_link = NULL;
    uint8_t        i;

    for (i = 0; i < ble_audio_db.acl_link_num; i++)
    {
        if (ble_audio_db.le_link[i].used == true &&
            ble_audio_db.le_link[i].conn_id == conn_id)
        {
            p_link = &ble_audio_db.le_link[i];
            break;
        }
    }

    return p_link;
}

T_BLE_AUDIO_LINK *ble_audio_link_find_by_conn_handle(uint16_t conn_handle)
{
    T_BLE_AUDIO_LINK *p_link = NULL;
    uint8_t        i;

    for (i = 0; i < ble_audio_db.acl_link_num; i++)
    {
        if (ble_audio_db.le_link[i].used == true &&
            ble_audio_db.le_link[i].conn_handle == conn_handle)
        {
            p_link = &ble_audio_db.le_link[i];
            break;
        }
    }

    return p_link;
}

T_BLE_AUDIO_LINK *ble_audio_link_find_by_addr(uint8_t *bd_addr, uint8_t bd_type)
{
    T_BLE_AUDIO_LINK *p_link = NULL;
    uint8_t        i;

    for (i = 0; i < ble_audio_db.acl_link_num; i++)
    {
        if (ble_audio_db.le_link[i].used == true &&
            ble_audio_db.le_link[i].remote_bd_type == bd_type
            && memcmp(ble_audio_db.le_link[i].remote_bd, bd_addr, GAP_BD_ADDR_LEN) == 0)
        {
            p_link = &ble_audio_db.le_link[i];
            break;
        }
    }
    if (p_link == NULL)
    {
        if (bd_type == GAP_REMOTE_ADDR_LE_RANDOM && (bd_addr[5] & 0xC0) == 0x40)
        {
            uint8_t resolved_addr[GAP_BD_ADDR_LEN];
            T_GAP_IDENT_ADDR_TYPE resolved_addr_type;
            if (ble_audio_ual_resolve_rpa(bd_addr, resolved_addr, &resolved_addr_type))
            {
                for (i = 0; i < ble_audio_db.acl_link_num; i++)
                {
                    if (ble_audio_db.le_link[i].used == true && ble_audio_db.le_link[i].is_rpa &&
                        ble_audio_db.le_link[i].identity_addr_type != 0xff &&
                        ble_audio_db.le_link[i].identity_addr_type == resolved_addr_type
                        && memcmp(ble_audio_db.le_link[i].identity_addr, resolved_addr, GAP_BD_ADDR_LEN) == 0)
                    {
                        p_link = &ble_audio_db.le_link[i];
                        break;
                    }
                }
            }
        }
    }
    return p_link;
}

T_BLE_AUDIO_LINK *ble_audio_link_alloc_by_conn_id(uint8_t conn_id, uint8_t srv_num)
{
    T_BLE_AUDIO_LINK *p_link = NULL;
    uint8_t        i;

    p_link = ble_audio_link_find_by_conn_id(conn_id);
    if (p_link != NULL)
    {
        return p_link;
    }

    for (i = 0; i < ble_audio_db.acl_link_num; i++)
    {
        if (ble_audio_db.le_link[i].used == false)
        {
            p_link = &ble_audio_db.le_link[i];
            break;
        }
    }
    if (p_link)
    {
        p_link->used    = true;
        p_link->id      = i;
        p_link->conn_id = conn_id;
        p_link->identity_addr_type = 0xff;
        if (srv_num)
        {
            p_link->srv_cfg_tbl = ble_audio_mem_zalloc(sizeof(T_LE_SRV_CFG) * srv_num);
            if (p_link->srv_cfg_tbl)
            {
                p_link->srv_cfg_num = srv_num;
            }
        }
    }

    return p_link;
}

void ble_srv_set_cccd_flags(T_LE_SRV_CFG *p_srv, uint16_t cccd_bits, uint8_t idx,
                            uint16_t flag_bit, bool pending)
{
    if (idx > 0)
    {
        return;
    }
    if (p_srv)
    {
        if (cccd_bits == GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        {
            p_srv->cccd_cfg[idx] &= ~flag_bit;
        }
        else if ((cccd_bits & GATT_CLIENT_CHAR_CONFIG_NOTIFY) ||
                 (cccd_bits & GATT_CLIENT_CHAR_CONFIG_INDICATE))
        {
            p_srv->cccd_cfg[idx] |= flag_bit;
            if (pending)
            {
                if (cccd_bits & GATT_CLIENT_CHAR_NOTIF_IND_DATA_PENGDING)
                {
                    p_srv->cccd_pending[idx] |= flag_bit;
                }
                else
                {
                    p_srv->cccd_pending[idx] &= ~flag_bit;
                }
            }
        }
    }
}

T_LE_SRV_CFG *ble_srv_find_by_srv_id(uint16_t conn_handle, T_SERVER_ID service_id)
{
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    T_LE_SRV_CFG *p_srv = NULL;
    uint8_t        i;

    if (p_link == NULL)
    {
        PROTOCOL_PRINT_ERROR1("ble_srv_find_by_srv_id: failed, not find link conn_handle 0x%x ",
                              conn_handle);
        return NULL;
    }

    for (i = 0; i < p_link->srv_cfg_num; i++)
    {
        if (p_link->srv_cfg_tbl[i].used == true &&
            p_link->srv_cfg_tbl[i].service_id == service_id)
        {
            p_srv = &p_link->srv_cfg_tbl[i];
            return p_srv;
        }
    }
    PROTOCOL_PRINT_ERROR2("ble_srv_find_by_srv_id: failed, not find service, conn_handle 0x%x, service_id %d ",
                          conn_handle, service_id);
    return NULL;
}

T_LE_SRV_CFG *ble_srv_alloc_by_srv_id(uint16_t conn_handle, T_SERVER_ID service_id)
{
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    T_LE_SRV_CFG *p_srv = NULL;
    uint8_t        i;

    if (p_link == NULL)
    {
        uint8_t conn_id;
        if (le_get_conn_id_by_handle(conn_handle, &conn_id))
        {
            p_link = ble_audio_link_find_by_conn_id(conn_id);
            if (p_link)
            {
                p_link->conn_handle = conn_handle;
            }
            else
            {
                return NULL;
            }
        }
        else
        {
            return NULL;
        }
    }

    p_srv = ble_srv_find_by_srv_id(conn_handle, service_id);
    if (p_srv != NULL)
    {
        return p_srv;
    }

    for (i = 0; i < p_link->srv_cfg_num; i++)
    {
        if (p_link->srv_cfg_tbl[i].used == false)
        {
            p_srv = &p_link->srv_cfg_tbl[i];
            p_srv->used = true;
            p_srv->service_id = service_id;
            break;
        }
    }

    return p_srv;
}

bool ble_audio_link_free(T_BLE_AUDIO_LINK *p_link)
{
    if (p_link != NULL)
    {
        if (p_link->used == true)
        {
            if (p_link->srv_cfg_tbl)
            {
                ble_audio_mem_free(p_link->srv_cfg_tbl);
                p_link->srv_cfg_tbl = NULL;
            }
            memset(p_link, 0, sizeof(T_BLE_AUDIO_LINK));
            p_link->conn_id = 0xFF;
            return true;
        }
    }

    return false;
}

void ble_audio_send_notify_all(uint16_t cid, T_SERVER_ID service_id, uint16_t attrib_index,
                               uint8_t *p_data, uint16_t data_len)
{
    for (uint8_t i = 0; i < ble_audio_db.acl_link_num; i++)
    {
        if ((ble_audio_db.le_link[i].used == true) &&
            (ble_audio_db.le_link[i].state == GAP_CONN_STATE_CONNECTED))
        {
            gatt_svc_send_data(ble_audio_db.le_link[i].conn_handle, cid, service_id,
                               attrib_index, p_data,
                               data_len > (ble_audio_db.le_link[i].mtu_size - 3) ? (ble_audio_db.le_link[i].mtu_size - 3) :
                               data_len, GATT_PDU_TYPE_NOTIFICATION);
        }
    }
}

#if LE_AUDIO_DEINIT
void ble_audio_link_deinit(void)
{
    for (uint8_t i = 0; i < ble_audio_db.acl_link_num; i++)
    {
        if (ble_audio_db.le_link[i].srv_cfg_tbl)
        {
            ble_audio_mem_free(ble_audio_db.le_link[i].srv_cfg_tbl);
        }
    }
    ble_audio_db.acl_link_num = 0;
    ble_audio_mem_free(ble_audio_db.le_link);
}
#endif
