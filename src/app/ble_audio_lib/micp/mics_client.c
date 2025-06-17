#include "mics_client.h"
#include "trace.h"
#include "ble_audio_dm.h"
#include "ble_link_util.h"
#include "aics_def.h"
#include "cap_int.h"

#if LE_AUDIO_MICS_CLIENT_SUPPORT
bool mics_check_group_data(T_LE_AUDIO_GROUP *p_group, T_MICS_CHECK_TYPE type, uint8_t data)
{
    T_LE_AUDIO_DEV *p_set_member;
    uint8_t temp_data = 0xff;
    PROTOCOL_PRINT_INFO2("mics_check_group_data: type %d, data %d", type, data);
    for (int i = 0; i < p_group->dev_queue.count; i++)
    {
        p_set_member = (T_LE_AUDIO_DEV *)os_queue_peek(&p_group->dev_queue, i);
        if (p_set_member != NULL && p_set_member->p_link &&
            p_set_member->p_link->state == GAP_CONN_STATE_CONNECTED)
        {
            if (p_set_member->p_link->mics_mic_value != MICS_RFU &&
                p_set_member->p_link->mics_mic_value != MICS_DISABLED)
            {
                if (type == MICS_CHECK_PREFER_MUTE)
                {
                    if (p_set_member->p_link->mics_mic_value != data)
                    {
                        return false;
                    }
                }
                else if (type == MICS_CHECK_MUTE_IS_EQUAL)
                {
                    if (temp_data == 0xff)
                    {
                        temp_data = p_set_member->p_link->mics_mic_value;
                    }
                    else
                    {
                        if (p_set_member->p_link->mics_mic_value != temp_data)
                        {
                            return false;
                        }
                    }
                }
            }
        }
    }
    PROTOCOL_PRINT_INFO0("mics_check_group_data: success");
    return true;
}

bool mics_read_mute_value(uint16_t conn_handle)
{
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_MICS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = MICS_UUID_CHAR_MUTE;

    if (p_link == NULL)
    {
        return false;
    }

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_read(conn_handle, handle, NULL);
    }
    return (cause == GAP_CAUSE_SUCCESS) ? true : false;
}

bool mics_write_mute_value(uint16_t conn_handle, T_MICS_MUTE mic_mute)
{
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);

    if (p_link == NULL)
    {
        return false;
    }

    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_MICS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = MICS_UUID_CHAR_MUTE;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_REQ, handle, 1, &mic_mute, NULL);
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool mics_get_all_inc_aics(uint16_t conn_handle, T_ATTR_INSTANCE *p_aics_instance)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID inc_uuid;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_MICS;
    inc_uuid.is_uuid16 = true;
    inc_uuid.instance_id = 0;
    inc_uuid.p.uuid16 = GATT_UUID_AICS;
    if (p_aics_instance)
    {
        if (gatt_client_find_include_srv_by_primary(conn_handle, &srv_uuid, &inc_uuid, p_aics_instance))
        {
            if (p_aics_instance->instance_num)
            {
                return true;
            }
        }
    }
    return false;
}

bool mics_get_mute_value(uint16_t conn_handle, T_MICS_MUTE *p_value)
{
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_value != NULL && p_link != NULL &&
        p_link->mics_mic_value != MICS_RFU)
    {
        *p_value = (T_MICS_MUTE)p_link->mics_mic_value;
        return true;
    }
    return false;
}

static void mics_client_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BLE_DM_EVENT_PARAM *param = (T_BLE_DM_EVENT_PARAM *)event_buf;

    switch (event_type)
    {
    case BLE_DM_EVENT_CONN_STATE:
        {
            T_BLE_AUDIO_LINK *p_link = NULL;
            p_link = ble_audio_link_find_by_conn_id(param->conn_state.conn_id);

            if (p_link && param->conn_state.state == GAP_CONN_STATE_CONNECTED)
            {
                p_link->mics_mic_value = MICS_RFU;
            }
        }
        break;

    default:
        break;
    }
}

static T_APP_RESULT mics_client_callback(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                         void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);

    if (p_link == NULL)
    {
        return APP_RESULT_APP_ERR;
    }

    switch (type)
    {
    case GATT_CLIENT_EVENT_DIS_DONE:
        {
            T_MICS_CLIENT_DIS_DONE dis_done = {0};
            if (p_client_cb_data->dis_done.is_found)
            {
                T_ATTR_UUID srv_uuid;
                srv_uuid.is_uuid16 = true;
                srv_uuid.instance_id = 0;
                srv_uuid.p.uuid16 = GATT_UUID_MICS;
                if (gatt_client_check_cccd_enabled(conn_handle, &srv_uuid, NULL) == false)
                {
                    gatt_client_enable_srv_cccd(conn_handle, &srv_uuid, GATT_CLIENT_CONFIG_ALL);
                }
                if (mics_read_mute_value(conn_handle))
                {
                    break;
                }
            }

            dis_done.conn_handle = conn_handle;
            dis_done.is_found = false;
            dis_done.load_from_ftl = false;
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_MICS_CLIENT_DIS_DONE, &dis_done);
        }
        break;

    case GATT_CLIENT_EVENT_READ_RESULT:
        {
            T_MICS_CLIENT_DIS_DONE dis_done = {0};

            dis_done.conn_handle = conn_handle;
            dis_done.is_found = true;
            dis_done.load_from_ftl = gatt_client_is_load_from_ftl(conn_handle);
            if (p_client_cb_data->read_result.char_uuid.p.uuid16 == MICS_UUID_CHAR_MUTE)
            {
                if (p_client_cb_data->read_result.value_size != 1)
                {
                    return APP_RESULT_INVALID_VALUE_SIZE;
                }
                if (p_link->mics_mic_value != MICS_RFU)
                {
                    p_link->mics_mic_value = (T_MICS_MUTE)p_client_cb_data->read_result.p_value[0];
                    break;
                }
                p_link->mics_mic_value = (T_MICS_MUTE)p_client_cb_data->read_result.p_value[0];
            }
            dis_done.mic_mute = (T_MICS_MUTE)p_link->mics_mic_value;
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_MICS_CLIENT_DIS_DONE, &dis_done);
        }
        break;

    case GATT_CLIENT_EVENT_WRITE_RESULT:
        {
            if (p_client_cb_data->write_result.char_type == GATT_CHAR_VALUE &&
                p_client_cb_data->write_result.char_uuid.p.uuid16 == MICS_UUID_CHAR_MUTE)
            {
                T_MICS_CLIENT_WRITE_RESULT write_result;
                write_result.conn_handle = conn_handle;
                write_result.cause = p_client_cb_data->write_result.cause;
                ble_audio_mgr_dispatch(LE_AUDIO_MSG_MICS_CLIENT_WRITE_RESULT, &write_result);
            }
        }
        break;

    case GATT_CLIENT_EVENT_NOTIFY_IND:
        {
            T_MICS_CLIENT_NOTIFY notify_data;
            notify_data.conn_handle = conn_handle;
            if (p_client_cb_data->notify_ind.char_uuid.p.uuid16 == MICS_UUID_CHAR_MUTE)
            {
                if (p_client_cb_data->notify_ind.value_size != 1)
                {
                    return APP_RESULT_INVALID_VALUE_SIZE;
                }
                notify_data.mic_mute = (T_MICS_MUTE)p_client_cb_data->notify_ind.p_value[0];
                p_link->mics_mic_value = notify_data.mic_mute;
            }
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_MICS_CLIENT_NOTIFY, &notify_data);
        }
        break;

    case GATT_CLIENT_EVENT_CCCD_CFG:
        {
            ble_audio_mgr_dispatch_client_attr_cccd_info(conn_handle, &p_client_cb_data->cccd_cfg);
        }
        break;

    default:
        break;
    }

    return result;
}
bool mics_client_init(void)
{
    T_ATTR_UUID srv_uuid = {0};
    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = GATT_UUID_MICS;
    if (gatt_client_spec_register(&srv_uuid, mics_client_callback) == GAP_CAUSE_SUCCESS)
    {
        ble_dm_cback_register(mics_client_dm_cback);
        return true;
    }
    return false;
}
#endif
