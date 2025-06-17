#include <string.h>
#include "ble_audio_mgr.h"
#include "aics_client.h"
#include "trace.h"
#include "ble_audio_dm.h"
#include "ble_link_util.h"
#include "ble_audio_group_int.h"
#include "cap_int.h"
#include "cap.h"

#if LE_AUDIO_AICS_CLIENT_SUPPORT
typedef struct
{
    T_AICS_CP_OP      cur_op;
    T_AICS_SRV_DATA   srv_data;
} T_AICS_SRV_DB;

typedef struct
{
    uint8_t           srv_num;
    T_AICS_SRV_DB     srv_db[];
} T_AICS_CLIENT_DB;

uint8_t aics_cfg = 0;

#define aics_check_link(conn_handle) aics_check_link_int(__func__, conn_handle)

T_AICS_CLIENT_DB *aics_check_link_int(const char *p_func_name, uint16_t conn_handle)
{
    T_BLE_AUDIO_LINK *p_link = NULL;

    PROTOCOL_PRINT_TRACE2("aics_check_link_int: %s, conn_handle 0x%x",
                          TRACE_STRING(p_func_name), conn_handle);

    p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL || p_link->p_aics_client == NULL)
    {
        PROTOCOL_PRINT_ERROR2("aics_check_link_int: %s failed, conn_handle 0x%x", TRACE_STRING(p_func_name),
                              conn_handle);
        return NULL;
    }
    return p_link->p_aics_client;
}

T_AICS_SRV_DB *aics_find_srv_db(T_AICS_CLIENT_DB *p_aics_db, uint8_t srv_instance_id)
{
    if (p_aics_db == NULL || srv_instance_id > p_aics_db->srv_num)
    {
        PROTOCOL_PRINT_ERROR1("vocs_find_srv_db: invalid srv_instance_id: %d", srv_instance_id);
        return NULL;
    }
    return &p_aics_db->srv_db[srv_instance_id];
}

void aics_link_free(T_BLE_AUDIO_LINK *p_link)
{
    if (p_link->p_aics_client != NULL)
    {
        T_AICS_CLIENT_DB *p_aics_db = (T_AICS_CLIENT_DB *)p_link->p_aics_client;
        for (uint8_t i = 0; i < p_aics_db->srv_num; i++)
        {
            uint8_t *p_data = p_aics_db->srv_db[i].srv_data.input_des.p_input_des;
            if (p_data)
            {
                ble_audio_mem_free(p_data);
            }
        }
        ble_audio_mem_free(p_link->p_aics_client);
        p_link->p_aics_client = NULL;
    }
}

void aics_cfg_cccd(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t cfg_flags, bool enable)
{
    T_AICS_CLIENT_DB *p_client = aics_check_link(conn_handle);
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = GATT_UUID_AICS;
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

    if (cfg_flags & AICS_INPUT_STATE_FLAG)
    {
        char_uuid.p.uuid16 = AICS_UUID_CHAR_INPUT_STATE;
        if (gatt_client_check_cccd_enabled(conn_handle, &srv_uuid, &char_uuid) != enable)
        {
            gatt_client_enable_char_cccd(conn_handle, &srv_uuid,
                                         &char_uuid, cccd_cfg);
        }
    }
    if (cfg_flags & AICS_INPUT_STATUS_FLAG)
    {
        char_uuid.p.uuid16 = AICS_UUID_CHAR_INPUT_STATUS;
        if (gatt_client_check_cccd_enabled(conn_handle, &srv_uuid, &char_uuid) != enable)
        {
            gatt_client_enable_char_cccd(conn_handle, &srv_uuid,
                                         &char_uuid, cccd_cfg);
        }
    }
    if (cfg_flags & AICS_INPUT_DES_FLAG)
    {
        char_uuid.p.uuid16 = AICS_UUID_CHAR_INPUT_DES;
        if (gatt_client_check_cccd_enabled(conn_handle, &srv_uuid, &char_uuid) != enable)
        {
            gatt_client_enable_char_cccd(conn_handle, &srv_uuid,
                                         &char_uuid, cccd_cfg);
        }
    }

    return;
}

bool aics_get_srv_data(uint16_t conn_handle, uint8_t srv_instance_id, T_AICS_SRV_DATA *p_value)
{
    T_AICS_CLIENT_DB *p_client = aics_check_link(conn_handle);
    if (p_client == NULL || srv_instance_id > p_client->srv_num || p_value == NULL)
    {
        PROTOCOL_PRINT_ERROR1("aics_get_srv_data: not find data srv_instance_id: %d", srv_instance_id);
        return false;
    }
    memcpy(p_value, &p_client->srv_db[srv_instance_id].srv_data, sizeof(T_AICS_SRV_DATA));
    return true;
}


bool aics_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, T_AICS_CHAR_TYPE type)
{
    T_AICS_CLIENT_DB *p_client = aics_check_link(conn_handle);
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = GATT_UUID_AICS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;

    if (p_client == NULL)
    {
        return false;
    }

    if (type == AICS_CHAR_INPUT_STATE)
    {
        char_uuid.p.uuid16 = AICS_UUID_CHAR_INPUT_STATE;
    }
    else if (type == AICS_CHAR_GAIN_SETTING_PROP)
    {
        char_uuid.p.uuid16 = AICS_UUID_CHAR_GAIN_SETTING_PROP;
    }
    else if (type == AICS_CHAR_INPUT_TYPE)
    {
        char_uuid.p.uuid16 = AICS_UUID_CHAR_INPUT_TYPE;
    }
    else if (type == AICS_CHAR_INPUT_STATUS)
    {
        char_uuid.p.uuid16 = AICS_UUID_CHAR_INPUT_STATUS;
    }
    else if (type == AICS_CHAR_INPUT_DES)
    {
        char_uuid.p.uuid16 = AICS_UUID_CHAR_INPUT_DES;
    }

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_read(conn_handle, handle, NULL);
    }
    return (cause == GAP_CAUSE_SUCCESS) ? true : false;

}

bool aics_write_input_des(uint16_t conn_handle, uint8_t srv_instance_id,
                          uint8_t *p_input_des, uint16_t input_des_len)
{
    T_AICS_CLIENT_DB *p_client = aics_check_link(conn_handle);
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = GATT_UUID_AICS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = AICS_UUID_CHAR_INPUT_DES;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_CMD, handle, input_des_len, p_input_des,
                                  NULL);
    }

    return (cause == GAP_CAUSE_SUCCESS) ? true : false;
}

bool aics_write_cp(uint16_t conn_handle, uint8_t srv_instance_id, T_AICS_CP_OP op,
                   T_AICS_CP_PARAM *p_param)
{
    T_AICS_CLIENT_DB *p_client = aics_check_link(conn_handle);
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = GATT_UUID_AICS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = AICS_UUID_CHAR_INPUT_CP;
    uint8_t data[3];
    uint16_t len = 2;
    T_AICS_SRV_DB *p_aics_srv_db = aics_find_srv_db(p_client, srv_instance_id);

    if (p_aics_srv_db == NULL || op >= AICS_CP_MAX || p_param == NULL)
    {
        PROTOCOL_PRINT_ERROR1("aics_write_cp op %d wrong or p_param is NULL", op);
        return false;
    }
    if (p_aics_srv_db->cur_op != AICS_CP_MAX)
    {
        PROTOCOL_PRINT_ERROR2("aics_write_cp: failed, conn_handle 0x%x, already has operation cur_op %d",
                              conn_handle, p_aics_srv_db->cur_op);
        return false;
    }
    data[0] = op;
    if (p_aics_srv_db->srv_data.type_exist & AICS_INPUT_STATE_FLAG)
    {
        data[1] = p_aics_srv_db->srv_data.input_state.change_counter;
    }
    else
    {
        PROTOCOL_PRINT_ERROR0("aics_write_cp: input state not read");
        return false;
    }

    if (op == AICS_CP_SET_GAIN_SETTING)
    {
        if (p_aics_srv_db->srv_data.input_state.gain_mode == AICS_GAIN_MODE_AUTOMATIC ||
            p_aics_srv_db->srv_data.input_state.gain_mode == AICS_GAIN_MODE_AUTOMATIC_ONLY)
        {
            PROTOCOL_PRINT_ERROR0("aics_write_cp: gain setting fail with wrong gain mode");
            return false;
        }
        data[2] = p_param->gaining_setting;
        len = 3;
    }

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_REQ, handle, len, data, NULL);
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        p_aics_srv_db->cur_op = op;
        return true;
    }
    else
    {
        return false;
    }
}
#if LE_AUDIO_GROUP_SUPPORT
T_LE_AUDIO_CAUSE aics_write_cp_by_group(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                                        uint8_t srv_instance_id,
                                        T_AICS_CP_OP op,
                                        T_AICS_CP_PARAM *p_param)
{
    T_LE_AUDIO_CAUSE cause;
    cause = cap_check_group_state(group_handle, false);
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
                if (aics_write_cp(p_set_member->p_link->conn_handle, srv_instance_id, op, p_param) == false)
                {
                    cause = LE_AUDIO_CAUSE_REQ_FAILED;
                }
            }
        }
    }
    return cause;
}
#endif

static void aics_client_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
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
                return;
            }
            if (param->conn_state.state == GAP_CONN_STATE_DISCONNECTED)
            {
                aics_link_free(p_link);
                PROTOCOL_PRINT_INFO0("aics_client_dm_cback: GAP_CONN_STATE_DISCONNECTED");
            }
        }
        break;

    default:
        break;
    }
}

T_APP_RESULT aics_client_callback(uint16_t conn_handle, T_GATT_CLIENT_EVENT type, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;
    T_AICS_CLIENT_DB *p_aics_db = aics_check_link(conn_handle);
    T_AICS_SRV_DB *p_aics_srv_db = NULL;

    if (p_aics_db == NULL && type != GATT_CLIENT_EVENT_DIS_DONE)
    {
        return APP_RESULT_APP_ERR;
    }

    switch (type)
    {
    case GATT_CLIENT_EVENT_DIS_DONE:
        {
            uint8_t srv_num = 0;
            T_AICS_CLIENT_DIS_DONE dis_done = {0};

            if (p_client_cb_data->dis_done.is_found)
            {
                T_BLE_AUDIO_LINK *p_link = NULL;
                srv_num = p_client_cb_data->dis_done.srv_instance_num;
                p_link = ble_audio_link_find_by_conn_handle(conn_handle);
                if (p_link == NULL || srv_num == 0)
                {
                    return APP_RESULT_APP_ERR;
                }
                aics_link_free(p_link);
                p_link->p_aics_client = ble_audio_mem_zalloc(sizeof(T_AICS_CLIENT_DB) + srv_num * sizeof(
                                                                 T_AICS_SRV_DB));
                if (p_link->p_aics_client == NULL)
                {
                    return APP_RESULT_APP_ERR;
                }

                p_aics_db = (T_AICS_CLIENT_DB *)p_link->p_aics_client;
                p_aics_db->srv_num = srv_num;
                for (uint8_t i = 0; i < srv_num; i++)
                {
                    p_aics_db->srv_db[i].srv_data.srv_instance_id = i;
                    p_aics_db->srv_db[i].cur_op = AICS_CP_MAX;
                }
            }
            dis_done.conn_handle = conn_handle;
            dis_done.is_found = p_client_cb_data->dis_done.is_found;
            dis_done.srv_num = srv_num;
            dis_done.load_from_ftl = p_client_cb_data->dis_done.load_from_ftl;
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_AICS_CLIENT_DIS_DONE, &dis_done);
        }
        break;

    case GATT_CLIENT_EVENT_READ_RESULT:
        {
            p_aics_srv_db = aics_find_srv_db(p_aics_db, p_client_cb_data->read_result.srv_instance_id);
            T_AICS_CLIENT_READ_RESULT read_result;

            if (p_aics_srv_db == NULL)
            {
                return APP_RESULT_APP_ERR;
            }
            read_result.conn_handle = conn_handle;
            read_result.cause = p_client_cb_data->read_result.cause;
            read_result.srv_instance_id = p_client_cb_data->read_result.srv_instance_id;
            if (p_client_cb_data->read_result.char_uuid.p.uuid16 == AICS_UUID_CHAR_INPUT_STATE)
            {
                if (p_client_cb_data->read_result.value_size != 4)
                {
                    return APP_RESULT_INVALID_VALUE_SIZE;
                }
                read_result.type = AICS_CHAR_INPUT_STATE;
                read_result.data.input_state.gain_setting = p_client_cb_data->read_result.p_value[0];
                read_result.data.input_state.mute = p_client_cb_data->read_result.p_value[1];
                read_result.data.input_state.gain_mode = p_client_cb_data->read_result.p_value[2];
                read_result.data.input_state.change_counter = p_client_cb_data->read_result.p_value[3];
                memcpy(&p_aics_srv_db->srv_data.input_state, &read_result.data.input_state,
                       sizeof(T_AICS_INPUT_STATE));
                p_aics_srv_db->srv_data.type_exist |= AICS_INPUT_STATE_FLAG;
            }
            else if (p_client_cb_data->read_result.char_uuid.p.uuid16 == AICS_UUID_CHAR_GAIN_SETTING_PROP)
            {
                if (p_client_cb_data->read_result.value_size != 3)
                {
                    return APP_RESULT_INVALID_VALUE_SIZE;
                }
                read_result.type = AICS_CHAR_GAIN_SETTING_PROP;
                read_result.data.setting_prop.gain_setting_units = p_client_cb_data->read_result.p_value[0];
                read_result.data.setting_prop.gain_setting_min = p_client_cb_data->read_result.p_value[1];
                read_result.data.setting_prop.gain_setting_max = p_client_cb_data->read_result.p_value[2];
                memcpy(&p_aics_srv_db->srv_data.setting_prop, &read_result.data.setting_prop,
                       sizeof(T_AICS_GAIN_SETTING_PROP));
                p_aics_srv_db->srv_data.type_exist |= AICS_GAIN_SETTING_PROP_FLAG;
            }
            else if (p_client_cb_data->read_result.char_uuid.p.uuid16 == AICS_UUID_CHAR_INPUT_TYPE)
            {
                if (p_client_cb_data->read_result.value_size != 1)
                {
                    return APP_RESULT_INVALID_VALUE_SIZE;
                }
                read_result.type = AICS_CHAR_INPUT_TYPE;
                read_result.data.input_type = p_client_cb_data->read_result.p_value[0];
                p_aics_srv_db->srv_data.input_type =
                    p_client_cb_data->read_result.p_value[0];
                p_aics_srv_db->srv_data.type_exist |= AICS_INPUT_TYPE_FLAG;
            }
            else if (p_client_cb_data->read_result.char_uuid.p.uuid16 == AICS_UUID_CHAR_INPUT_STATUS)
            {
                if (p_client_cb_data->read_result.value_size != 1)
                {
                    return APP_RESULT_INVALID_VALUE_SIZE;
                }
                read_result.type = AICS_CHAR_INPUT_STATUS;
                read_result.data.input_status = p_client_cb_data->read_result.p_value[0];
                p_aics_srv_db->srv_data.input_status =
                    p_client_cb_data->read_result.p_value[0];
                p_aics_srv_db->srv_data.type_exist |= AICS_INPUT_STATUS_FLAG;
            }
            else if (p_client_cb_data->read_result.char_uuid.p.uuid16 == AICS_UUID_CHAR_INPUT_DES)
            {
                read_result.data.input_des.input_des_len = p_client_cb_data->read_result.value_size;
                read_result.data.input_des.p_input_des = p_client_cb_data->read_result.p_value;
                read_result.type = AICS_CHAR_INPUT_DES;
                if (aics_cfg & AUDIO_DESCRIPTION_STORAGE)
                {
                    uint8_t *p_value = p_aics_srv_db->srv_data.input_des.p_input_des;
                    uint16_t len = p_aics_srv_db->srv_data.input_des.input_des_len;
                    p_aics_srv_db->srv_data.input_des.input_des_len = 0;
                    if (len < p_client_cb_data->read_result.value_size)
                    {
                        if (p_value != NULL)
                        {
                            ble_audio_mem_free(p_value);
                            p_value = ble_audio_mem_zalloc(p_client_cb_data->read_result.value_size + 1);
                            p_aics_srv_db->srv_data.input_des.p_input_des = p_value;
                        }
                    }
                    if (p_value != NULL)
                    {
                        memcpy(p_value, p_client_cb_data->read_result.p_value, p_client_cb_data->read_result.value_size);
                        p_aics_srv_db->srv_data.input_des.input_des_len =
                            p_client_cb_data->read_result.value_size;
                    }
                    p_aics_srv_db->srv_data.type_exist |= AICS_INPUT_DES_FLAG;
                }
            }
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_AICS_CLIENT_READ_RESULT, &read_result);
        }
        break;

    case GATT_CLIENT_EVENT_WRITE_RESULT:
        {
            if (p_client_cb_data->write_result.char_type == GATT_CHAR_VALUE &&
                p_client_cb_data->write_result.char_uuid.p.uuid16 == AICS_UUID_CHAR_INPUT_CP)
            {
                p_aics_srv_db = aics_find_srv_db(p_aics_db, p_client_cb_data->write_result.srv_instance_id);

                if (p_aics_srv_db == NULL)
                {
                    return APP_RESULT_APP_ERR;
                }
                T_AICS_CLIENT_CP_RESULT cp_result;
                cp_result.conn_handle = conn_handle;
                cp_result.cause = p_client_cb_data->write_result.cause;
                cp_result.srv_instance_id = p_client_cb_data->write_result.srv_instance_id;
                cp_result.cp_op = p_aics_srv_db->cur_op;
                p_aics_srv_db->cur_op = AICS_CP_MAX;
                ble_audio_mgr_dispatch(LE_AUDIO_MSG_AICS_CLIENT_CP_RESULT, &cp_result);
            }
        }
        break;
    case GATT_CLIENT_EVENT_NOTIFY_IND:
        {
            T_AICS_CLIENT_NOTIFY notify_data;
            p_aics_srv_db = aics_find_srv_db(p_aics_db, p_client_cb_data->notify_ind.srv_instance_id);

            if (p_aics_srv_db == NULL)
            {
                return APP_RESULT_APP_ERR;
            }
            notify_data.conn_handle = conn_handle;
            notify_data.srv_instance_id = p_client_cb_data->notify_ind.srv_instance_id;
            if (p_client_cb_data->notify_ind.char_uuid.p.uuid16 == AICS_UUID_CHAR_INPUT_STATE)
            {
                if (p_client_cb_data->notify_ind.value_size != 4)
                {
                    return APP_RESULT_INVALID_VALUE_SIZE;
                }
                notify_data.type = AICS_CHAR_INPUT_STATE;
                notify_data.data.input_state.gain_setting = p_client_cb_data->notify_ind.p_value[0];
                notify_data.data.input_state.mute = p_client_cb_data->notify_ind.p_value[1];
                notify_data.data.input_state.gain_mode = p_client_cb_data->notify_ind.p_value[2];
                notify_data.data.input_state.change_counter = p_client_cb_data->notify_ind.p_value[3];
                memcpy(&p_aics_srv_db->srv_data.input_state, &notify_data.data.input_state,
                       sizeof(T_AICS_INPUT_STATE));
                p_aics_srv_db->srv_data.type_exist |= AICS_INPUT_STATE_FLAG;
            }
            else if (p_client_cb_data->notify_ind.char_uuid.p.uuid16 == AICS_UUID_CHAR_INPUT_STATUS)
            {
                if (p_client_cb_data->notify_ind.value_size != 1)
                {
                    return APP_RESULT_INVALID_VALUE_SIZE;
                }

                notify_data.type = AICS_CHAR_INPUT_STATUS;
                notify_data.data.input_status = p_client_cb_data->notify_ind.p_value[0];
                p_aics_srv_db->srv_data.input_status = p_client_cb_data->notify_ind.p_value[0];
                p_aics_srv_db->srv_data.type_exist |= AICS_INPUT_STATUS_FLAG;
            }
            else if (p_client_cb_data->notify_ind.char_uuid.p.uuid16 == AICS_UUID_CHAR_INPUT_DES)
            {
                notify_data.data.input_des.input_des_len = p_client_cb_data->notify_ind.value_size;
                notify_data.data.input_des.p_input_des = p_client_cb_data->notify_ind.p_value;
                notify_data.type = AICS_CHAR_INPUT_DES;
                if (aics_cfg & AUDIO_DESCRIPTION_STORAGE)
                {
                    uint8_t *p_value = p_aics_srv_db->srv_data.input_des.p_input_des;
                    uint16_t len = p_aics_srv_db->srv_data.input_des.input_des_len;
                    p_aics_srv_db->srv_data.input_des.input_des_len = 0;
                    if (len < p_client_cb_data->notify_ind.value_size)
                    {
                        if (p_value != NULL)
                        {
                            ble_audio_mem_free(p_value);
                            p_value = ble_audio_mem_zalloc(p_client_cb_data->notify_ind.value_size + 1);
                            p_aics_srv_db->srv_data.input_des.p_input_des = p_value;
                        }
                    }
                    if (p_value != NULL)
                    {
                        memcpy(p_value, p_client_cb_data->notify_ind.p_value, p_client_cb_data->notify_ind.value_size);
                        p_aics_srv_db->srv_data.input_des.input_des_len =
                            p_client_cb_data->notify_ind.value_size;
                    }
                    p_aics_srv_db->srv_data.type_exist |= AICS_INPUT_DES_FLAG;
                }
            }
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_AICS_CLIENT_NOTIFY, &notify_data);
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

bool aics_client_init(uint8_t cfg)
{
    T_ATTR_UUID srv_uuid = {0};
    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = GATT_UUID_AICS;
    aics_cfg = cfg;
    if (gatt_client_spec_register(&srv_uuid, aics_client_callback) == GAP_CAUSE_SUCCESS)
    {
        ble_dm_cback_register(aics_client_dm_cback);
        return true;
    }
    return false;
}

#if LE_AUDIO_DEINIT
void aics_client_deinit(void)
{
    aics_cfg = 0;
    for (uint8_t k = 0; k < ble_audio_db.acl_link_num; k++)
    {
        aics_link_free(&ble_audio_db.le_link[k]);
    }
}
#endif
#endif

