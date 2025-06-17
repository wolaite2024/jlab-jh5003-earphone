#include <string.h>
#include "ble_audio_mgr.h"
#include "vcs_client.h"
#include "trace.h"
#include "ble_audio_dm.h"
#include "ble_link_util.h"
#include "ble_audio_group_int.h"
#include "cap_int.h"
#include "aics_def.h"

#if LE_AUDIO_VCS_CLIENT_SUPPORT
#define VCS_CCCD_CFG_FLAG 0x04

typedef struct
{
    uint8_t           volume_flags;
    T_VCS_CP_OP       cur_op;
    uint8_t           init_flags;
    uint8_t           type_exist;
    T_VOLUME_STATE    volume_state;
} T_VCS_CLIENT_DB;

#define vcs_check_link(conn_handle) vcs_check_link_int(__func__, conn_handle)

T_VCS_CLIENT_DB *vcs_check_link_int(const char *p_func_name, uint16_t conn_handle)
{
    T_BLE_AUDIO_LINK *p_link = NULL;

    PROTOCOL_PRINT_TRACE2("vcs_check_link_int: %s, conn_handle 0x%x",
                          TRACE_STRING(p_func_name), conn_handle);

    p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL || p_link->p_vcs_client == NULL)
    {
        PROTOCOL_PRINT_ERROR2("vcs_check_link_int: %s failed, conn_handle 0x%x", TRACE_STRING(p_func_name),
                              conn_handle);
        return NULL;
    }
    return (T_VCS_CLIENT_DB *)p_link->p_vcs_client;
}

bool vcs_read_char_value(uint16_t conn_handle, T_VCS_CHAR_TYPE type)
{
    T_VCS_CLIENT_DB *p_client = vcs_check_link(conn_handle);
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_VCS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;

    if (p_client == NULL)
    {
        return false;
    }
    if (type == VCS_CHAR_VOLUME_STATE)
    {
        char_uuid.p.uuid16 = VCS_UUID_CHAR_VOLUME_STATE;
    }
    else if (type == VCS_CHAR_VOLUME_FLAGS)
    {
        char_uuid.p.uuid16 = VCS_UUID_CHAR_VOLUME_FLAGS;
    }

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_read(conn_handle, handle, NULL);
    }
    return (cause == GAP_CAUSE_SUCCESS) ? true : false;
}

bool vcs_write_cp(uint16_t conn_handle, T_VCS_CP_OP op, T_VCS_VOLUME_CP_PARAM *p_param)
{
    T_VCS_CLIENT_DB *p_client = vcs_check_link(conn_handle);
    if (op >= VCS_CP_MAX)
    {
        return false;
    }

    if (p_client == NULL)
    {
        PROTOCOL_PRINT_ERROR1("vcs_write_cp: failed, conn_handle 0x%x  not link", conn_handle);
        return false;
    }

    if (p_client->cur_op != VCS_CP_MAX)
    {
        PROTOCOL_PRINT_ERROR2("vcs_write_cp: failed, conn_handle 0x%x  already has operation cur_op %d",
                              conn_handle, p_client->cur_op);
        return false;
    }

    uint8_t len = 2;
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_VCS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = VCS_UUID_CHAR_VOLUME_CP;
    uint8_t data[3];
    data[0] = op;
    data[1] = p_client->volume_state.change_counter;

    if (op == VCS_CP_SET_ABSOLUTE_VOLUME)
    {
        if (p_param)
        {
            data[2] = p_param->volume_setting;
            len = 3;
        }
        else
        {
            return false;
        }
    }

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_REQ, handle, len, data, NULL);
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        p_client->cur_op = op;
        PROTOCOL_PRINT_INFO2("vcs_write_cp: conn_handle 0x%x, op %d", conn_handle, op);
        return true;
    }
    else
    {
        return false;
    }
}

#if LE_AUDIO_GROUP_SUPPORT
T_LE_AUDIO_CAUSE vcs_write_cp_by_group(T_BLE_AUDIO_GROUP_HANDLE group_handle, T_VCS_CP_OP op,
                                       T_VCS_VOLUME_CP_PARAM *p_param)
{
    T_LE_AUDIO_CAUSE cause;
    if (op == VCS_CP_SET_ABSOLUTE_VOLUME ||
        op == VCS_CP_UNMUTE ||
        op == VCS_CP_MUTE)
    {
        cause = LE_AUDIO_CAUSE_INVALID_PARAM;
    }
    else
    {
        cause = cap_check_group_state(group_handle, false);
    }
    if (cause == LE_AUDIO_CAUSE_SUCCESS)
    {
        T_LE_AUDIO_GROUP *p_group = (T_LE_AUDIO_GROUP *)group_handle;
        T_LE_AUDIO_DEV *p_set_member;
        for (int i = 0; i < p_group->dev_queue.count; i++)
        {
            p_set_member = (T_LE_AUDIO_DEV *)os_queue_peek(&p_group->dev_queue, i);
            if (p_set_member != NULL && p_set_member->p_link &&
                p_set_member->p_link->state == GAP_CONN_STATE_CONNECTED)
            {
                if (vcs_write_cp(p_set_member->p_link->conn_handle, op, p_param) == false)
                {
                    cause = LE_AUDIO_CAUSE_REQ_FAILED;
                }
            }
        }
    }
    return cause;
}

bool vcs_check_group_data(T_LE_AUDIO_GROUP *p_group, T_VCS_CHECK_TYPE type, uint8_t data)
{
    T_LE_AUDIO_DEV *p_set_member;
    uint16_t temp_data = 0xffff;
    PROTOCOL_PRINT_INFO2("vcs_check_group_data: type %d, data %d", type, data);
    for (int i = 0; i < p_group->dev_queue.count; i++)
    {
        p_set_member = (T_LE_AUDIO_DEV *)os_queue_peek(&p_group->dev_queue, i);
        if (p_set_member != NULL && p_set_member->p_link &&
            p_set_member->p_link->state == GAP_CONN_STATE_CONNECTED)
        {
            T_VCS_CLIENT_DB *p_client = p_set_member->p_link->p_vcs_client;
            if (p_client != NULL && (p_client->type_exist & VCS_VOLUME_STATE_FLAG))
            {
                if (type == VCS_CHECK_PREFER_VOLUME)
                {
                    if (p_client->volume_state.volume_setting != data)
                    {
                        return false;
                    }
                }
                else if (type == VCS_CHECK_PREFER_MUTE)
                {
                    if (p_client->volume_state.mute != data)
                    {
                        return false;
                    }
                }
                else if (type == VCS_CHECK_VOLUME_IS_EQUAL)
                {
                    if (temp_data == 0xffff)
                    {
                        temp_data = p_client->volume_state.volume_setting;
                    }
                    else
                    {
                        if (p_client->volume_state.volume_setting != temp_data)
                        {
                            return false;
                        }
                    }

                }
                else if (type == VCS_CHECK_MUTE_IS_EQUAL)
                {
                    if (temp_data == 0xffff)
                    {
                        temp_data = p_client->volume_state.mute;
                    }
                    else
                    {
                        if (p_client->volume_state.mute != temp_data)
                        {
                            return false;
                        }
                    }
                }
            }
        }
    }
    PROTOCOL_PRINT_INFO0("vcs_check_group_data: success");
    return true;
}
#endif
bool vcs_get_all_inc_aics(uint16_t conn_handle, T_ATTR_INSTANCE *p_aics_instance)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID inc_uuid;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_VCS;
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


bool vcs_get_volume_state(uint16_t conn_handle, T_VOLUME_STATE *p_value)
{
    T_VCS_CLIENT_DB *p_client = vcs_check_link(conn_handle);
    if (p_value != NULL && p_client != NULL &&
        (p_client->type_exist & VCS_VOLUME_STATE_FLAG))
    {
        memcpy(p_value, &p_client->volume_state, sizeof(T_VOLUME_STATE));
        return true;
    }
    return false;
}

bool vcs_get_volume_flags(uint16_t conn_handle, uint8_t *p_value)
{
    T_VCS_CLIENT_DB *p_client = vcs_check_link(conn_handle);
    if (p_value != NULL && p_client != NULL &&
        (p_client->type_exist & VCS_VOLUME_FLAGS_FLAG))
    {
        *p_value = p_client->volume_flags;
        return true;
    }
    return false;
}

static void vcs_client_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
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
                PROTOCOL_PRINT_ERROR1("vcs_client_dm_cback: not find the link, conn_id %d",
                                      param->conn_state.conn_id);
                break;
            }

            if (param->conn_state.state == GAP_CONN_STATE_DISCONNECTED)
            {
                PROTOCOL_PRINT_INFO0("vcs_client_dm_cback: GAP_CONN_STATE_DISCONNECTED");
                if (p_link->p_vcs_client != NULL)
                {
                    ble_audio_mem_free(p_link->p_vcs_client);
                    p_link->p_vcs_client = NULL;
                }
            }
        }
        break;

    default:
        break;
    }
}

static void vcs_client_check_disc_done(T_VCS_CLIENT_DB *p_vcs_db, uint16_t conn_handle,
                                       uint8_t flags)
{
    if (p_vcs_db->init_flags & flags)
    {
        p_vcs_db->init_flags &= ~flags;
        if (p_vcs_db->init_flags == 0)
        {
            T_VCS_CLIENT_DIS_DONE dis_done;
            dis_done.conn_handle = conn_handle;
            dis_done.is_found = true;
            dis_done.load_from_ftl = gatt_client_is_load_from_ftl(conn_handle);
            dis_done.type_exist = p_vcs_db->type_exist;
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_VCS_CLIENT_DIS_DONE, &dis_done);
        }
    }
}

static T_APP_RESULT vcs_client_callback(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                        void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;
    T_VCS_CLIENT_DB *p_vcs_db = vcs_check_link(conn_handle);

    if (p_vcs_db == NULL && type != GATT_CLIENT_EVENT_DIS_DONE)
    {
        PROTOCOL_PRINT_ERROR1("vcs_client_callback: client db is NULL, conn_handle 0x%x", conn_handle);
        return APP_RESULT_APP_ERR;
    }

    switch (type)
    {
    case GATT_CLIENT_EVENT_DIS_DONE:
        {
            T_VCS_CLIENT_DIS_DONE dis_done = {0};
            if (p_client_cb_data->dis_done.is_found)
            {
                T_BLE_AUDIO_LINK *p_link = NULL;
                T_ATTR_UUID srv_uuid;
                srv_uuid.is_uuid16 = true;
                srv_uuid.instance_id = 0;
                srv_uuid.p.uuid16 = GATT_UUID_VCS;
                p_link = ble_audio_link_find_by_conn_handle(conn_handle);
                if (p_link == NULL)
                {
                    PROTOCOL_PRINT_ERROR1("GATT_CLIENT_EVENT_DIS_DONE not find link, conn_handle 0x%x", conn_handle);
                    return APP_RESULT_APP_ERR;
                }
                if (p_link->p_vcs_client)
                {
                    ble_audio_mem_free(p_link->p_vcs_client);
                }

                p_link->p_vcs_client = ble_audio_mem_zalloc(sizeof(T_VCS_CLIENT_DB));

                if (p_link->p_vcs_client == NULL)
                {
                    PROTOCOL_PRINT_ERROR1("vcs_client_callback: client alloc fail, conn_handle 0x%x", conn_handle);
                    return APP_RESULT_APP_ERR;
                }

                p_vcs_db = (T_VCS_CLIENT_DB *)p_link->p_vcs_client;
                p_vcs_db->cur_op = VCS_CP_MAX;
                if (gatt_client_check_cccd_enabled(conn_handle, &srv_uuid, NULL) == false)
                {
                    if (gatt_client_enable_srv_cccd(conn_handle, &srv_uuid,
                                                    GATT_CLIENT_CONFIG_ALL) == GAP_CAUSE_SUCCESS)
                    {
                        p_vcs_db->init_flags |= VCS_CCCD_CFG_FLAG;
                    }
                }
                if (vcs_read_char_value(conn_handle, VCS_CHAR_VOLUME_STATE))
                {
                    p_vcs_db->init_flags |= VCS_VOLUME_STATE_FLAG;
                }
                if (vcs_read_char_value(conn_handle, VCS_CHAR_VOLUME_FLAGS))
                {
                    p_vcs_db->init_flags |= VCS_VOLUME_FLAGS_FLAG;
                }
                if (p_vcs_db->init_flags)
                {
                    break;
                }
            }
            dis_done.conn_handle = conn_handle;
            dis_done.is_found = p_client_cb_data->dis_done.is_found;
            dis_done.load_from_ftl = p_client_cb_data->dis_done.load_from_ftl;
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_VCS_CLIENT_DIS_DONE, &dis_done);
        }
        break;

    case GATT_CLIENT_EVENT_CCCD_CFG:
        {
            vcs_client_check_disc_done(p_vcs_db, conn_handle, VCS_CCCD_CFG_FLAG);
            ble_audio_mgr_dispatch_client_attr_cccd_info(conn_handle, &p_client_cb_data->cccd_cfg);
        }
        break;

    case GATT_CLIENT_EVENT_READ_RESULT:
        {
            if (p_client_cb_data->read_result.cause == GAP_SUCCESS)
            {
                T_VCS_CLIENT_VOLUME_STATE_DATA read_result = {0};
                read_result.conn_handle = conn_handle;
                read_result.is_notify = false;
                if (p_client_cb_data->read_result.char_uuid.p.uuid16 == VCS_UUID_CHAR_VOLUME_STATE)
                {
                    if (p_client_cb_data->read_result.value_size != 3)
                    {
                        return APP_RESULT_INVALID_VALUE_SIZE;
                    }
                    read_result.type = VCS_CHAR_VOLUME_STATE;
                    read_result.data.volume_state.volume_setting = p_client_cb_data->read_result.p_value[0];
                    read_result.data.volume_state.mute = p_client_cb_data->read_result.p_value[1];
                    read_result.data.volume_state.change_counter = p_client_cb_data->read_result.p_value[2];
                    p_vcs_db->volume_state = read_result.data.volume_state;
                    p_vcs_db->type_exist |= VCS_VOLUME_STATE_FLAG;
                }
                else if (p_client_cb_data->read_result.char_uuid.p.uuid16 == VCS_UUID_CHAR_VOLUME_FLAGS)
                {
                    if (p_client_cb_data->read_result.value_size != 1)
                    {
                        return APP_RESULT_INVALID_VALUE_SIZE;
                    }
                    read_result.type = VCS_CHAR_VOLUME_FLAGS;
                    read_result.data.volume_flags = p_client_cb_data->read_result.p_value[0];
                    p_vcs_db->volume_flags = read_result.data.volume_flags;
                    p_vcs_db->type_exist |= VCS_VOLUME_FLAGS_FLAG;
                }
                if (p_vcs_db->init_flags == 0)
                {
                    ble_audio_mgr_dispatch(LE_AUDIO_MSG_VCS_CLIENT_VOLUME_STATE_DATA, &read_result);
                }
                else
                {
                    if (p_client_cb_data->read_result.char_uuid.p.uuid16 == VCS_UUID_CHAR_VOLUME_STATE)
                    {
                        vcs_client_check_disc_done(p_vcs_db, conn_handle, VCS_VOLUME_STATE_FLAG);
                    }
                    else if (p_client_cb_data->read_result.char_uuid.p.uuid16 == VCS_UUID_CHAR_VOLUME_FLAGS)
                    {
                        vcs_client_check_disc_done(p_vcs_db, conn_handle, VCS_VOLUME_FLAGS_FLAG);
                    }
                }
            }
        }
        break;

    case GATT_CLIENT_EVENT_WRITE_RESULT:
        {
            if (p_client_cb_data->write_result.char_type == GATT_CHAR_VALUE &&
                p_client_cb_data->write_result.char_uuid.p.uuid16 == VCS_UUID_CHAR_VOLUME_CP)
            {
                T_VCS_CLIENT_CP_RESULT cp_result;
                cp_result.conn_handle = conn_handle;
                cp_result.cause = p_client_cb_data->write_result.cause;
                cp_result.cp_op = p_vcs_db->cur_op;
                p_vcs_db->cur_op = VCS_CP_MAX;
                ble_audio_mgr_dispatch(LE_AUDIO_MSG_VCS_CLIENT_CP_RESULT, &cp_result);
                if (p_client_cb_data->write_result.cause == ATT_ERR_VCS_INVALID_CHANGE_COUNTER)
                {
                    vcs_read_char_value(conn_handle, VCS_CHAR_VOLUME_STATE);
                }
            }
        }
        break;

    case GATT_CLIENT_EVENT_NOTIFY_IND:
        {
            T_VCS_CLIENT_VOLUME_STATE_DATA notify_data;
            notify_data.conn_handle = conn_handle;
            notify_data.is_notify = true;
            if (p_client_cb_data->notify_ind.char_uuid.p.uuid16 == VCS_UUID_CHAR_VOLUME_STATE)
            {
                if (p_client_cb_data->notify_ind.value_size != 3)
                {
                    return APP_RESULT_INVALID_VALUE_SIZE;
                }
                notify_data.type = VCS_CHAR_VOLUME_STATE;
                notify_data.data.volume_state.volume_setting = p_client_cb_data->notify_ind.p_value[0];
                notify_data.data.volume_state.mute = p_client_cb_data->notify_ind.p_value[1];
                notify_data.data.volume_state.change_counter = p_client_cb_data->notify_ind.p_value[2];
                p_vcs_db->volume_state = notify_data.data.volume_state;
            }
            else if (p_client_cb_data->notify_ind.char_uuid.p.uuid16 == VCS_UUID_CHAR_VOLUME_FLAGS)
            {
                if (p_client_cb_data->notify_ind.value_size != 1)
                {
                    return APP_RESULT_INVALID_VALUE_SIZE;
                }
                notify_data.type = VCS_CHAR_VOLUME_FLAGS;
                notify_data.data.volume_flags = p_client_cb_data->notify_ind.p_value[0];
                p_vcs_db->volume_flags = notify_data.data.volume_flags;
            }
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_VCS_CLIENT_VOLUME_STATE_DATA, &notify_data);
        }
        break;

    default:
        break;
    }

    return result;
}

bool vcs_client_init(void)
{
    T_ATTR_UUID srv_uuid = {0};
    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = GATT_UUID_VCS;
    if (gatt_client_spec_register(&srv_uuid, vcs_client_callback) == GAP_CAUSE_SUCCESS)
    {
        ble_dm_cback_register(vcs_client_dm_cback);
        return true;
    }
    return false;
}

#if LE_AUDIO_DEINIT
void vcs_client_deinit(void)
{
    for (uint8_t k = 0; k < ble_audio_db.acl_link_num; k++)
    {
        if (ble_audio_db.le_link[k].p_vcs_client != NULL)
        {
            ble_audio_mem_free(ble_audio_db.le_link[k].p_vcs_client);
            ble_audio_db.le_link[k].p_vcs_client = NULL;
        }
    }
}
#endif
#endif
