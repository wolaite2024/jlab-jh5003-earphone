#include "csis_client_int.h"
#include "trace.h"
#include "ble_audio_mgr.h"
#include "ble_audio_dm.h"
#include "ble_link_util.h"
#include "csis_client_int.h"

#if LE_AUDIO_CSIS_CLIENT_SUPPORT
bool csis_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, T_CSIS_CHAR_TYPE type,
                          P_FUN_GATT_CLIENT_CB p_req_cb)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = GATT_UUID_CSIS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;

    if (type == CSIS_CHAR_SIRK)
    {
        char_uuid.p.uuid16 = CSIS_UUID_CHAR_SIRK;
    }
    else if (type == CSIS_CHAR_SIZE)
    {
        char_uuid.p.uuid16 = CSIS_UUID_CHAR_SIZE;
    }
    else if (type == CSIS_CHAR_LOCK)
    {
        char_uuid.p.uuid16 = CSIS_UUID_CHAR_LOCK;
    }
    else if (type == CSIS_CHAR_RANK)
    {
        char_uuid.p.uuid16 = CSIS_UUID_CHAR_RANK;
    }

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_read(conn_handle, handle, p_req_cb);
    }

    return (cause == GAP_CAUSE_SUCCESS) ? true : false;
}

bool csis_send_lock_req(uint16_t conn_handle, uint8_t srv_instance_id, T_CSIS_LOCK lock,
                        P_FUN_GATT_CLIENT_CB p_req_cb)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    uint16_t handle = 0;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;

    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = GATT_UUID_CSIS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = CSIS_UUID_CHAR_LOCK;
    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_REQ, handle, 1, &lock, p_req_cb);
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        PROTOCOL_PRINT_ERROR1("csis_send_lock_req: failed, conn_handle 0x%x", conn_handle);
        return false;
    }
}

static void csis_client_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BLE_DM_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BLE_DM_EVENT_CONN_STATE:
        {
            T_BLE_AUDIO_LINK *p_link = NULL;
            p_link = ble_audio_link_find_by_conn_id(param->conn_state.conn_id);
            if (p_link == NULL)
            {
                PROTOCOL_PRINT_ERROR1("csis_client_dm_cback: not find the link, conn_id %d",
                                      param->conn_state.conn_id);
                break;
            }
            if (param->conn_state.state == GAP_CONN_STATE_DISCONNECTED)
            {
                PROTOCOL_PRINT_INFO0("csis_client_dm_cback: GAP_CONN_STATE_DISCONNECTED");
                if (p_link->p_csis_client != NULL)
                {
                    ble_audio_mem_free(p_link->p_csis_client);
                    p_link->p_csis_client = NULL;
                }
            }
        }
        break;

    default:
        break;
    }
}

bool csis_client_init(P_FUN_GATT_CLIENT_CB app_cb)
{
    T_ATTR_UUID srv_uuid = {0};
    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = GATT_UUID_CSIS;
    gatt_client_spec_register(&srv_uuid, app_cb);
    ble_dm_cback_register(csis_client_dm_cback);
    return true;
}

#if LE_AUDIO_DEINIT
void csis_client_deinit(void)
{
    for (uint8_t k = 0; k < ble_audio_db.acl_link_num; k++)
    {
        if (ble_audio_db.le_link[k].p_csis_client != NULL)
        {
            ble_audio_mem_free(ble_audio_db.le_link[k].p_csis_client);
            ble_audio_db.le_link[k].p_csis_client = NULL;
        }
    }
}
#endif
#endif
