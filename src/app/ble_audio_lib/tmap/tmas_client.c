#include "tmas_client.h"
#include "trace.h"
#include "ble_audio.h"
#include "ble_audio_mgr.h"
#include "bt_gatt_client.h"

#if LE_AUDIO_TMAS_CLIENT_SUPPORT
#if 0
//LE_AUDIO_MSG_TMAS_CLIENT_READ_ROLE_BY_UUID_RESULT
typedef struct
{
    uint16_t conn_handle;
    uint16_t role;
    uint16_t cause;
} T_TMAS_CLIENT_READ_ROLE_BY_UUID_RESULT;

T_APP_RESULT tmap_client_read_by_uuid_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                         void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;
    T_TMAS_CLIENT_READ_ROLE_BY_UUID_RESULT read_role;
    read_role.conn_handle = conn_handle;
    read_role.cause = p_client_cb_data->read_result.cause;
    read_role.role = 0;
    if (p_client_cb_data->read_result.value_size == 2)
    {
        LE_ARRAY_TO_UINT16(read_role.role, (uint8_t *)p_client_cb_data->read_result.p_value);
    }
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_TMAS_CLIENT_READ_ROLE_BY_UUID_RESULT, &read_role);

    return result;
}

bool tmas_read_role_by_uuid(uint16_t conn_handle)
{
    if (gatt_client_read_uuid(conn_handle, 0x01,
                              0xffff, TMAS_UUID_CHAR_ROLE, tmap_client_read_by_uuid_cb) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}
#endif
T_APP_RESULT tmas_client_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;

    switch (type)
    {
    case GATT_CLIENT_EVENT_DIS_DONE:
        {
            T_TMAS_CLIENT_DIS_DONE dis_done;
            PROTOCOL_PRINT_INFO2("[TMAP] GATT_CLIENT_EVENT_DIS_DONE: is_found %d, srv_instance_num %d",
                                 p_client_cb_data->dis_done.is_found,
                                 p_client_cb_data->dis_done.srv_instance_num);
            dis_done.conn_handle = conn_handle;
            dis_done.is_found = p_client_cb_data->dis_done.is_found;
            dis_done.load_from_ftl = p_client_cb_data->dis_done.load_from_ftl;
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_TMAS_CLIENT_DIS_DONE, &dis_done);
        }
        break;

    case GATT_CLIENT_EVENT_READ_RESULT:
        {
            T_TMAS_CLIENT_READ_ROLE_RESULT read_role;
            read_role.conn_handle = conn_handle;
            read_role.cause = p_client_cb_data->read_result.cause;
            read_role.role = 0;
            if (p_client_cb_data->read_result.value_size == 2)
            {
                LE_ARRAY_TO_UINT16(read_role.role, (uint8_t *)p_client_cb_data->read_result.p_value);
            }
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_TMAS_CLIENT_READ_ROLE_RESULT, &read_role);
        }
        break;

    default:
        break;
    }

    return result;
}

bool tmas_read_role(uint16_t conn_handle)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_TMAS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = TMAS_UUID_CHAR_ROLE;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_read(conn_handle, handle, NULL);
    }
    return (cause == GAP_CAUSE_SUCCESS) ? true : false;
}

bool tmas_client_init(void)
{
    T_ATTR_UUID srv_uuid = {0};
    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = GATT_UUID_TMAS;
    gatt_client_spec_register(&srv_uuid, tmas_client_cb);
    return true;
}
#endif
