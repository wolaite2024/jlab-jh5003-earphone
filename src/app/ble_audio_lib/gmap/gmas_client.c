#include "gmas_client.h"
#include "trace.h"
#include "ble_audio.h"
#include "ble_audio_mgr.h"
#include "bt_gatt_client.h"

#if LE_AUDIO_GMAS_CLIENT_SUPPORT
T_APP_RESULT gmas_client_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;

    switch (type)
    {
    case GATT_CLIENT_EVENT_DIS_DONE:
        {
            T_GMAS_CLIENT_DIS_DONE dis_done = {0};
            PROTOCOL_PRINT_INFO2("[GMAP] GATT_CLIENT_EVENT_DIS_DONE: is_found %d, srv_instance_num %d",
                                 p_client_cb_data->dis_done.is_found,
                                 p_client_cb_data->dis_done.srv_instance_num);
            dis_done.conn_handle = conn_handle;
            dis_done.is_found = p_client_cb_data->dis_done.is_found;
            dis_done.load_from_ftl = p_client_cb_data->dis_done.load_from_ftl;
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_GMAS_CLIENT_DIS_DONE, &dis_done);
        }
        break;

    case GATT_CLIENT_EVENT_READ_RESULT:
        {
            PROTOCOL_PRINT_INFO3("[GMAP] GATT_CLIENT_EVENT_READ_RESULT: conn_handle 0x%x, char_uuid 0x%x, cause 0x%x",
                                 conn_handle,
                                 p_client_cb_data->read_result.char_uuid.p.uuid16,
                                 p_client_cb_data->read_result.cause);
            if (p_client_cb_data->read_result.char_uuid.p.uuid16 == GMAS_UUID_CHAR_GMAP_ROLE)
            {
                T_GMAS_CLIENT_READ_ROLE_RESULT read_role = {0};

                read_role.conn_handle = conn_handle;
                read_role.cause = p_client_cb_data->read_result.cause;

                if (p_client_cb_data->read_result.value_size == 1)
                {
                    read_role.gmap_role = *p_client_cb_data->read_result.p_value;
                }
                ble_audio_mgr_dispatch(LE_AUDIO_MSG_GMAS_CLIENT_READ_ROLE_RESULT, &read_role);
            }
            else
            {
                T_GMAS_CLIENT_READ_FEATURES_RESULT read_features = {0};
                read_features.conn_handle = conn_handle;
                read_features.char_uuid = p_client_cb_data->read_result.char_uuid.p.uuid16;
                read_features.cause = p_client_cb_data->read_result.cause;

                if (p_client_cb_data->read_result.value_size == 1)
                {
                    read_features.features = *p_client_cb_data->read_result.p_value;
                }
                ble_audio_mgr_dispatch(LE_AUDIO_MSG_GMAS_CLIENT_READ_FEATURE_RESULT, &read_features);
            }
        }
        break;

    default:
        break;
    }

    return result;
}

bool gmas_read_gmap_role(uint16_t conn_handle)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_GMAS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = GMAS_UUID_CHAR_GMAP_ROLE;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_read(conn_handle, handle, NULL);
    }
    return (cause == GAP_CAUSE_SUCCESS) ? true : false;
}

bool gmas_read_features(uint16_t conn_handle, uint16_t char_uuid)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid16;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_GMAS;
    char_uuid16.is_uuid16 = true;
    char_uuid16.instance_id = 0;
    char_uuid16.p.uuid16 = char_uuid;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid16, &handle))
    {
        cause = gatt_client_read(conn_handle, handle, NULL);
    }
    return (cause == GAP_CAUSE_SUCCESS) ? true : false;
}

bool gmas_client_init(void)
{
    T_ATTR_UUID srv_uuid = {0};
    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = GATT_UUID_GMAS;
    gatt_client_spec_register(&srv_uuid, gmas_client_cb);
    return true;
}
#endif
