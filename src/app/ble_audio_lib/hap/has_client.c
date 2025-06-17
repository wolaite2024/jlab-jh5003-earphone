#include <string.h>
#include "has_def.h"
#include "has_client.h"
#include "has_int.h"
#include "trace.h"
#include "ble_audio.h"
#include "ble_audio_mgr.h"
#include "ble_audio_dm.h"
#include "ble_link_util.h"
#include "bt_gatt_client.h"

/* base on HAP_v09 */
#if LE_AUDIO_HAS_CLIENT_SUPPORT
typedef struct
{
    T_HAS_HA_FEATURES ha_features;
    uint8_t active_preset_idx;
} T_HAS_CLIENT_DB;

#define has_check_link(conn_handle) has_check_link_int(__func__, conn_handle)

T_HAS_CLIENT_DB *has_check_link_int(const char *p_func_name, uint16_t conn_handle)
{
    T_BLE_AUDIO_LINK *p_link = NULL;

    p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL || p_link->p_has_client == NULL)
    {
        PROTOCOL_PRINT_ERROR2("has_check_link: %s failed, conn_handle 0x%x", TRACE_STRING(p_func_name),
                              conn_handle);
        return NULL;
    }
    return p_link->p_has_client;
}


void has_cfg_cccd(uint16_t conn_handle, uint8_t cfg_flags, bool enable)
{
    T_HAS_CLIENT_DB *p_client = has_check_link(conn_handle);
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_HAS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    uint8_t cccd_cfg;

    if (enable)
    {
        cccd_cfg = GATT_CLIENT_CONFIG_ALL;
    }
    else
    {
        cccd_cfg = GATT_CLIENT_CONFIG_DISABLE;
    }

    if (p_client == NULL)
    {
        return;
    }

    while (cfg_flags)
    {
        if (cfg_flags & HAS_HEARING_AID_FEATURE_FLAG)
        {
            char_uuid.p.uuid16 = HAS_UUID_CHAR_HA_FEATURES;
            cfg_flags &= ~HAS_HEARING_AID_FEATURE_FLAG;
        }
        else if (cfg_flags & HAS_HEARING_AID_PRESET_CP_FLAG)
        {
            char_uuid.p.uuid16 = HAS_UUID_CHAR_HA_PRESET_CP;
            cfg_flags &= ~HAS_HEARING_AID_PRESET_CP_FLAG;
        }
        else if (cfg_flags & HAS_ACTIVE_PRESET_IDX_FLAG)
        {
            char_uuid.p.uuid16 = HAS_UUID_CHAR_ACTIVE_PRESET_IDX;
            cfg_flags &= ~HAS_ACTIVE_PRESET_IDX_FLAG;
        }
        else
        {
            break;
        }

        if (gatt_client_check_cccd_enabled(conn_handle, &srv_uuid, &char_uuid) != enable)
        {
            gatt_client_enable_char_cccd(conn_handle, &srv_uuid, &char_uuid, cccd_cfg);
        }
    }
}

bool has_read_ha_features(uint16_t conn_handle)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_HAS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = HAS_UUID_CHAR_HA_FEATURES;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_read(conn_handle, handle, NULL);
    }

    return (cause == GAP_CAUSE_SUCCESS) ? true : false;
}

bool has_read_active_preset_idx(uint16_t conn_handle)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_HAS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = HAS_UUID_CHAR_ACTIVE_PRESET_IDX;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_read(conn_handle, handle, NULL);
    }

    return (cause == GAP_CAUSE_SUCCESS) ? true : false;
}

bool has_write_cp_req(uint16_t conn_handle, uint8_t length, uint8_t *p_data)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    T_GATT_WRITE_TYPE type = GATT_WRITE_TYPE_REQ;
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL)
    {
        return false;
    }

    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_HAS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = HAS_UUID_CHAR_HA_PRESET_CP;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, type, handle, length, p_data, NULL);
    }

    return (cause == GAP_CAUSE_SUCCESS) ? true : false;
}

bool has_cp_read_presets(uint16_t conn_handle, uint8_t start_preset_idx, uint8_t preset_num)
{
    uint8_t data[3];
    data[0] = HAS_CP_OP_READ_PRESETS_REQ;
    data[1] = start_preset_idx;
    data[2] = preset_num;

    return has_write_cp_req(conn_handle, 3, data);
}

bool has_cp_write_preset_name(uint16_t conn_handle, uint8_t preset_idx,
                              uint8_t name_len, char *p_name)
{
    bool ret = false;
    T_HAS_CLIENT_DB *p_has_db = has_check_link(conn_handle);

    if (p_has_db != NULL && p_has_db->ha_features.writable_preset_support == 1)
    {
        uint8_t *p_data = ble_audio_mem_zalloc(name_len + 2);

        if (p_data != NULL)
        {
            p_data[0] = HAS_CP_OP_WRITE_PRESET_NAME;
            p_data[1] = preset_idx;
            memcpy(&p_data[2], p_name, name_len);

            ret = has_write_cp_req(conn_handle, name_len + 2, p_data);
            ble_audio_mem_free(p_data);
        }
    }

    return ret;
}

bool has_cp_set_active_preset(uint16_t conn_handle, uint8_t idx, bool is_sync_local)
{
    uint8_t data[2];

    if (is_sync_local)
    {
        T_HAS_CLIENT_DB *p_has_db = has_check_link(conn_handle);
        if (p_has_db == NULL || p_has_db->ha_features.preset_sync_support == 0)
        {
            PROTOCOL_PRINT_ERROR0("has_cp_set_active_preset: failed");
            return false;
        }

        data[0] = HAS_CP_OP_SET_ACTIVE_PRESET_SYNC_LOCAL;
    }
    else
    {
        data[0] = HAS_CP_OP_SET_ACTIVE_PRESET;
    }

    data[1] = idx;

    return has_write_cp_req(conn_handle, 2, data);
}

bool has_cp_set_next_preset(uint16_t conn_handle, bool is_sync_local)
{
    uint8_t data[1];

    if (is_sync_local)
    {
        T_HAS_CLIENT_DB *p_has_db = has_check_link(conn_handle);
        if (p_has_db == NULL || p_has_db->ha_features.preset_sync_support == 0)
        {
            PROTOCOL_PRINT_ERROR0("has_cp_set_next_preset: failed");
            return false;
        }

        data[0] = HAS_CP_OP_SET_NEXT_PRESET_SYNC_LOCAL;
    }
    else
    {
        data[0] = HAS_CP_OP_SET_NEXT_PRESET;
    }

    return has_write_cp_req(conn_handle, 1, data);
}

bool has_cp_set_previous_preset(uint16_t conn_handle, bool is_sync_local)
{
    uint8_t data[1];

    if (is_sync_local)
    {
        T_HAS_CLIENT_DB *p_has_db = has_check_link(conn_handle);
        if (p_has_db == NULL || p_has_db->ha_features.preset_sync_support == 0)
        {
            PROTOCOL_PRINT_ERROR0("has_cp_set_previous_preset: failed");
            return false;
        }

        data[0] = HAS_CP_OP_SET_PREVIOUS_PRESET_SYNC_LOCAL;
    }
    else
    {
        data[0] = HAS_CP_OP_SET_PREVIOUS_PRESET;
    }

    return has_write_cp_req(conn_handle, 1, data);
}

T_APP_RESULT has_client_handle_cp_notify_ind(uint16_t conn_handle,
                                             T_GATT_CLIENT_NOTIFY_IND *p_notify)
{
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_HAS_CLIENT_CP_NOTIFY_IND_DATA notify_ind_data = {0};

    notify_ind_data.conn_handle = conn_handle;
    notify_ind_data.notify = p_notify->notify;

    notify_ind_data.cp_data.cp_op = (T_HAS_CP_OP)p_notify->p_value[0];

    switch (p_notify->p_value[0])
    {
    case HAS_CP_OP_READ_PRESET_RSP:
        {
            if (p_notify->value_size < HAS_CP_READ_PRESET_RSP_PARAM_MIN_LEN ||
                p_notify->value_size > HAS_CP_READ_PRESET_RSP_PARAM_MAX_LEN)
            {
                result = APP_RESULT_INVALID_VALUE_SIZE;
            }
            else
            {
                notify_ind_data.cp_data.is_last = p_notify->p_value[1];
                notify_ind_data.cp_data.preset.index = p_notify->p_value[2];
                notify_ind_data.cp_data.preset.properties = p_notify->p_value[3];
                notify_ind_data.cp_data.preset.p_name = (char *)&p_notify->p_value[4];
                notify_ind_data.cp_data.name_length = p_notify->value_size - 4;
            }
        }
        break;

    case HAS_CP_OP_PRESET_CHANGED:
        {
            notify_ind_data.cp_data.change_id = p_notify->p_value[1];
            switch (p_notify->p_value[1])
            {
            case GENERIC_UPDATE:
                {
                    if (p_notify->value_size < HAS_CP_GENERIC_UPDATE_PARAM_MIN_LEN ||
                        p_notify->value_size > HAS_CP_GENERIC_UPDATE_PARAM_MAX_LEN)
                    {
                        result = APP_RESULT_INVALID_VALUE_SIZE;
                    }
                    else
                    {
                        notify_ind_data.cp_data.is_last = p_notify->p_value[2];
                        notify_ind_data.cp_data.pre_idx = p_notify->p_value[3];
                        notify_ind_data.cp_data.preset.index = p_notify->p_value[4];
                        notify_ind_data.cp_data.preset.properties = p_notify->p_value[5];
                        notify_ind_data.cp_data.preset.p_name = (char *)&p_notify->p_value[6];
                        notify_ind_data.cp_data.name_length = p_notify->value_size - 6;
                    }
                }
                break;

            case PRESET_RECORD_DELETED:
            case PRESET_RECORD_AVAILABLE:
            case PRESET_RECORD_UNAVAILABLE:
                {
                    if (p_notify->value_size < HAS_CP_PRESET_RECORD_CHANGE_PARAM_LEN)
                    {
                        result = APP_RESULT_INVALID_VALUE_SIZE;
                    }
                    else
                    {
                        notify_ind_data.cp_data.is_last = p_notify->p_value[2];
                        notify_ind_data.cp_data.preset.index = p_notify->p_value[3];
                    }
                }
                break;

            default:
                break;
            }
        }
        break;

    default:
        break;
    }

    if (result == APP_RESULT_SUCCESS)
    {
        ble_audio_mgr_dispatch(LE_AUDIO_MSG_HAS_CLIENT_CP_NOTIFY_IND_DATA, &notify_ind_data);
    }

    return result;
}

static void has_client_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BLE_DM_EVENT_PARAM *param = (T_BLE_DM_EVENT_PARAM *)event_buf;

    switch (event_type)
    {
    case BLE_DM_EVENT_CONN_STATE:
        {
            T_BLE_AUDIO_LINK *p_link = NULL;
            p_link = ble_audio_link_find_by_conn_id(param->conn_state.conn_id);
            if (p_link == NULL)
            {
                PROTOCOL_PRINT_ERROR1("has_client_cback: not find the link, conn_id %d",
                                      param->conn_state.conn_id);
                break;
            }

            if (param->conn_state.state == GAP_CONN_STATE_DISCONNECTED)
            {
                PROTOCOL_PRINT_INFO0("has_client_cback: GAP_CONN_STATE_DISCONNECTED");
                if (p_link->p_has_client != NULL)
                {
                    ble_audio_mem_free(p_link->p_has_client);
                    p_link->p_has_client = NULL;
                }
            }
        }
        break;

    default:
        break;
    }
}

T_APP_RESULT has_client_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;
    T_HAS_CLIENT_DB *p_has_db = has_check_link(conn_handle);

    if (p_has_db == NULL && type != GATT_CLIENT_EVENT_DIS_DONE)
    {
        goto error;
    }

    switch (type)
    {
    case GATT_CLIENT_EVENT_DIS_DONE:
        {
            T_HAS_CLIENT_DIS_DONE dis_done = {0};

            PROTOCOL_PRINT_INFO1("[HAS] GATT_CLIENT_EVENT_DIS_DONE: is_found %d",
                                 p_client_cb_data->dis_done.is_found);

            if (p_client_cb_data->dis_done.is_found)
            {
                T_BLE_AUDIO_LINK *p_link = NULL;

                p_link = ble_audio_link_find_by_conn_handle(conn_handle);
                if (p_link == NULL)
                {
                    goto error;
                }

                if (p_link->p_has_client)
                {
                    ble_audio_mem_free(p_link->p_has_client);
                }
                p_link->p_has_client = ble_audio_mem_zalloc(sizeof(T_HAS_CLIENT_DB));
                if (p_link->p_has_client == NULL)
                {
                    goto error;
                }

                p_has_db = p_link->p_has_client;

                T_ATTR_UUID srv_uuid;
                T_ATTR_UUID char_uuid;
                uint16_t handle = 0;
                srv_uuid.is_uuid16 = true;
                srv_uuid.instance_id = 0;
                srv_uuid.p.uuid16 = GATT_UUID_HAS;
                char_uuid.is_uuid16 = true;
                char_uuid.instance_id = 0;
                char_uuid.p.uuid16 = HAS_UUID_CHAR_HA_FEATURES;
                uint16_t ha_feature_properties = 0;

                if (gatt_client_get_char_prop(conn_handle, &srv_uuid, &char_uuid, &ha_feature_properties))
                {
                    if (ha_feature_properties & GATT_CHAR_PROP_NOTIFY)
                    {
                        dis_done.is_ha_features_notify_support = true;
                    }
                }

                char_uuid.p.uuid16 = HAS_UUID_CHAR_HA_PRESET_CP;
                if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
                {
                    dis_done.is_ha_cp_exist = true;
                    uint16_t properties;
                    gatt_client_get_char_prop(conn_handle, &srv_uuid, &char_uuid, &properties);
                    if (properties & GATT_CHAR_PROP_NOTIFY)
                    {
                        dis_done.is_ha_cp_notify = true;
                    }
                }

                has_read_ha_features(conn_handle);
            }

            dis_done.conn_handle = conn_handle;
            dis_done.is_found = p_client_cb_data->dis_done.is_found;
            dis_done.load_from_ftl = p_client_cb_data->dis_done.load_from_ftl;

            ble_audio_mgr_dispatch(LE_AUDIO_MSG_HAS_CLIENT_DIS_DONE, &dis_done);
        }
        break;

    case GATT_CLIENT_EVENT_READ_RESULT:
        {
            PROTOCOL_PRINT_INFO3("[HAS] GATT_CLIENT_EVENT_READ_RESULT: conn_handle 0x%x, uuid 0x%x, cause 0x%x",
                                 conn_handle, p_client_cb_data->read_result.char_uuid.p.uuid16,
                                 p_client_cb_data->read_result.cause);

            if (p_client_cb_data->read_result.char_uuid.p.uuid16 == HAS_UUID_CHAR_HA_FEATURES)
            {
                T_HAS_CLIENT_READ_HA_FEATURES_RESULT read_result;
                read_result.conn_handle = conn_handle;
                read_result.cause = p_client_cb_data->read_result.cause;
                memcpy(&read_result.has_feature, &p_client_cb_data->read_result.p_value[0], 1);
                memcpy(&p_has_db->ha_features, &p_client_cb_data->read_result.p_value[0], 1);
                ble_audio_mgr_dispatch(LE_AUDIO_MSG_HAS_CLIENT_READ_HA_FEATURES_RESULT, &read_result);
            }
            else if (p_client_cb_data->read_result.char_uuid.p.uuid16 == HAS_UUID_CHAR_ACTIVE_PRESET_IDX)
            {
                T_HAS_CLIENT_READ_ACTIVE_PRESET_IDX_RESULT read_result;
                read_result.conn_handle = conn_handle;
                read_result.cause = p_client_cb_data->read_result.cause;
                read_result.active_preset_idx = p_client_cb_data->read_result.p_value[0];
                p_has_db->active_preset_idx = p_client_cb_data->read_result.p_value[0];
                ble_audio_mgr_dispatch(LE_AUDIO_MSG_HAS_CLIENT_READ_ACTIVE_PRESET_IDX_RESULT, &read_result);
            }
        }
        break;

    case GATT_CLIENT_EVENT_NOTIFY_IND:
        {
            PROTOCOL_PRINT_INFO3("[HAS] GATT_CLIENT_EVENT_NOTIFY_IND: conn_handle 0x%x, is_notify %d, uuid 0x%x",
                                 conn_handle, p_client_cb_data->notify_ind.notify,
                                 p_client_cb_data->notify_ind.char_uuid.p.uuid16);
            uint16_t uuid = p_client_cb_data->notify_ind.char_uuid.p.uuid16;

            switch (uuid)
            {
            case HAS_UUID_CHAR_HA_FEATURES:
                {
                    T_HAS_CLIENT_HA_FEATURES_NOTIFY notify_data;
                    notify_data.conn_handle = conn_handle;
                    memcpy(&notify_data.has_feature, &p_client_cb_data->notify_ind.p_value[0], 1);
                    memcpy(&p_has_db->ha_features, &p_client_cb_data->notify_ind.p_value[0], 1);
                    ble_audio_mgr_dispatch(LE_AUDIO_MSG_HAS_CLIENT_HA_FEATURES_NOTIFY, &notify_data);
                }
                break;

            case HAS_UUID_CHAR_ACTIVE_PRESET_IDX:
                {
                    T_HAS_CLIENT_ACTIVE_PRESET_IDX_NOTIFY notify_data;
                    notify_data.conn_handle = conn_handle;
                    notify_data.active_preset_idx = p_client_cb_data->notify_ind.p_value[0];
                    p_has_db->active_preset_idx = p_client_cb_data->notify_ind.p_value[0];
                    ble_audio_mgr_dispatch(LE_AUDIO_MSG_HAS_CLIENT_ACTIVE_PRESET_IDX_NOTIFY, &notify_data);
                }
                break;

            case HAS_UUID_CHAR_HA_PRESET_CP:
                {
                    has_client_handle_cp_notify_ind(conn_handle, &p_client_cb_data->notify_ind);
                }
                break;

            default:
                break;
            }
        }
        break;

    case GATT_CLIENT_EVENT_WRITE_RESULT:
        {
            PROTOCOL_PRINT_INFO3("[HAS] GATT_CLIENT_EVENT_WRITE_RESULT: conn_handle 0x%x, cause 0x%x, uuid 0x%x",
                                 conn_handle, p_client_cb_data->write_result.cause,
                                 p_client_cb_data->write_result.char_uuid.p.uuid16);
            if (p_client_cb_data->write_result.char_type == GATT_CHAR_VALUE &&
                p_client_cb_data->write_result.char_uuid.p.uuid16 == HAS_UUID_CHAR_HA_PRESET_CP)
            {
                T_HAS_CLIENT_CP_RESULT write_result;
                write_result.conn_handle = conn_handle;
                write_result.cause = p_client_cb_data->write_result.cause;
                ble_audio_mgr_dispatch(LE_AUDIO_MSG_HAS_CLIENT_CP_RESULT, &write_result);
            }
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
error:
    PROTOCOL_PRINT_ERROR1("[HAS] has_client_cb: error, type %d", type);
    return APP_RESULT_APP_ERR;
}

bool has_client_init(void)
{
    T_ATTR_UUID srv_uuid = {0};
    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = GATT_UUID_HAS;
    if (gatt_client_spec_register(&srv_uuid, has_client_cb) == GAP_CAUSE_SUCCESS)
    {
        ble_dm_cback_register(has_client_dm_cback);
        return true;
    }

    return false;
}

#if LE_AUDIO_DEINIT
void has_client_deinit(void)
{
    for (uint8_t k = 0; k < ble_audio_db.acl_link_num; k++)
    {
        if (ble_audio_db.le_link[k].p_has_client != NULL)
        {
            ble_audio_mem_free(ble_audio_db.le_link[k].p_has_client);
            ble_audio_db.le_link[k].p_has_client = NULL;
        }
    }
}
#endif
#endif
