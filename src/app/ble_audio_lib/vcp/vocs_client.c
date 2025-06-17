#include <string.h>
#include "ble_audio_mgr.h"
#include "vocs_client.h"
#include "trace.h"
#include "ble_audio_dm.h"
#include "ble_link_util.h"
#include "ble_audio_group_int.h"
#include "cap_int.h"
#include "cap.h"

#if LE_AUDIO_VOCS_CLIENT_SUPPORT

typedef struct
{
    T_VOCS_CP_OP      cur_op;
    T_VOCS_SRV_DATA   srv_data;
} T_VOCS_SRV_DB;

typedef struct
{
    uint8_t           srv_num;
    T_VOCS_SRV_DB     srv_db[];
} T_VOCS_CLIENT_DB;

uint8_t vocs_cfg = 0;

#define vocs_check_link(conn_handle) vocs_check_link_int(__func__, conn_handle)

T_VOCS_CLIENT_DB *vocs_check_link_int(const char *p_func_name, uint16_t conn_handle)
{
    T_BLE_AUDIO_LINK *p_link = NULL;

    PROTOCOL_PRINT_TRACE2("vocs_check_link_int: %s, conn_handle 0x%x",
                          TRACE_STRING(p_func_name), conn_handle);

    p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL || p_link->p_vocs_client == NULL)
    {
        PROTOCOL_PRINT_ERROR2("vocs_check_link_int: %s failed, conn_handle 0x%x", TRACE_STRING(p_func_name),
                              conn_handle);
        return NULL;
    }
    return (T_VOCS_CLIENT_DB *)p_link->p_vocs_client;
}

T_VOCS_SRV_DB *vocs_find_srv_db(T_VOCS_CLIENT_DB *p_vocs_db, uint8_t srv_instance_id)
{
    if (p_vocs_db == NULL || srv_instance_id > p_vocs_db->srv_num)
    {
        PROTOCOL_PRINT_ERROR1("vocs_find_srv_db: invalid srv_instance_id: %d", srv_instance_id);
        return NULL;
    }
    return &p_vocs_db->srv_db[srv_instance_id];
}

void vocs_link_free(T_BLE_AUDIO_LINK *p_link)
{
    if (p_link->p_vocs_client != NULL)
    {
        T_VOCS_CLIENT_DB *p_vocs_db = (T_VOCS_CLIENT_DB *)p_link->p_vocs_client;
        for (uint8_t i = 0; i < p_vocs_db->srv_num; i++)
        {
            uint8_t *p_data = p_vocs_db->srv_db[i].srv_data.output_des.p_output_des;
            if (p_data)
            {
                ble_audio_mem_free(p_data);
            }
        }
        ble_audio_mem_free(p_link->p_vocs_client);
        p_link->p_vocs_client = NULL;
    }
}

void vocs_cfg_cccd(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t cfg_flags, bool enable)
{
    T_VOCS_CLIENT_DB *p_client = vocs_check_link(conn_handle);
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = GATT_UUID_VOCS;
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

    if (cfg_flags & VOCS_VOLUME_OFFSET_STATE_FLAG)
    {
        char_uuid.p.uuid16 = VOCS_UUID_CHAR_VOLUME_OFFSET_STATE;
        if (gatt_client_check_cccd_enabled(conn_handle, &srv_uuid, &char_uuid) != enable)
        {
            gatt_client_enable_char_cccd(conn_handle, &srv_uuid,
                                         &char_uuid, cccd_cfg);
        }
    }
    if (cfg_flags & VOCS_AUDIO_LOCATION_FLAG)
    {
        char_uuid.p.uuid16 = VOCS_UUID_CHAR_AUDIO_LOCATION;
        if (gatt_client_check_cccd_enabled(conn_handle, &srv_uuid, &char_uuid) != enable)
        {
            gatt_client_enable_char_cccd(conn_handle, &srv_uuid,
                                         &char_uuid, cccd_cfg);
        }
    }
    if (cfg_flags & VOCS_AUDIO_OUTPUT_DES_FLAG)
    {
        char_uuid.p.uuid16 = VOCS_UUID_CHAR_AUDIO_OUTPUT_DES;
        if (gatt_client_check_cccd_enabled(conn_handle, &srv_uuid, &char_uuid) != enable)
        {
            gatt_client_enable_char_cccd(conn_handle, &srv_uuid,
                                         &char_uuid, cccd_cfg);
        }
    }

    return;
}


bool vocs_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, T_VOCS_CHAR_TYPE type)
{
    T_VOCS_CLIENT_DB *p_client = vocs_check_link(conn_handle);
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = GATT_UUID_VOCS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    if (type == VOCS_CHAR_OFFSET_STATE)
    {
        char_uuid.p.uuid16 = VOCS_UUID_CHAR_VOLUME_OFFSET_STATE;
    }
    else if (type == VOCS_CHAR_AUDIO_LOCATION)
    {
        char_uuid.p.uuid16 = VOCS_UUID_CHAR_AUDIO_LOCATION;
    }
    else if (type == VOCS_CHAR_AUDIO_OUTPUT_DESC)
    {
        char_uuid.p.uuid16 = VOCS_UUID_CHAR_AUDIO_OUTPUT_DES;
    }

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_read(conn_handle, handle, NULL);
    }
    return (cause == GAP_CAUSE_SUCCESS) ? true : false;
}

bool vocs_write_audio_location(uint16_t conn_handle, uint8_t srv_instance_id,
                               uint32_t audio_location)
{
    T_VOCS_CLIENT_DB *p_client = vocs_check_link(conn_handle);
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    uint8_t location[4];
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = GATT_UUID_VOCS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = VOCS_UUID_CHAR_AUDIO_LOCATION;

    LE_UINT32_TO_ARRAY(location, audio_location);

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_CMD, handle, 4, location, NULL);
    }

    return (cause == GAP_CAUSE_SUCCESS) ? true : false;
}

bool vocs_write_output_des(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t *p_output_des,
                           uint16_t output_des_len)
{
    T_VOCS_CLIENT_DB *p_client = vocs_check_link(conn_handle);
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = GATT_UUID_VOCS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = VOCS_UUID_CHAR_AUDIO_OUTPUT_DES;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_CMD, handle, output_des_len, p_output_des,
                                  NULL);
    }

    return (cause == GAP_CAUSE_SUCCESS) ? true : false;
}

bool vocs_write_cp(uint16_t conn_handle, uint8_t srv_instance_id, T_VOCS_CP_OP op,
                   T_VOCS_CP_PARAM *p_param)
{
    T_VOCS_CLIENT_DB *p_client = vocs_check_link(conn_handle);
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = GATT_UUID_VOCS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = VOCS_UUID_CHAR_VOLUME_OFFSET_CP;
    uint8_t data[4];
    T_VOCS_SRV_DB *p_vocs_srv_db = vocs_find_srv_db(p_client, srv_instance_id);

    if (p_vocs_srv_db == NULL || op >= VOCS_CP_MAX || p_param == NULL)
    {
        return false;
    }
    if (p_vocs_srv_db->cur_op != VOCS_CP_MAX)
    {
        PROTOCOL_PRINT_ERROR2("vocs_write_cp: failed, conn_handle 0x%x, already has operation cur_op %d",
                              conn_handle, p_vocs_srv_db->cur_op);
        return false;
    }
    data[0] = op;
    if (p_vocs_srv_db->srv_data.type_exist & VOCS_VOLUME_OFFSET_STATE_FLAG)
    {
        data[1] = p_vocs_srv_db->srv_data.volume_offset.change_counter;
    }
    else
    {
        PROTOCOL_PRINT_ERROR0("vocs_write_cp: offset state not read");
        return false;
    }
    data[2] = p_param->volume_offset >> 0;
    data[3] = p_param->volume_offset >> 8;;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_REQ, handle, 4, data, NULL);
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        p_vocs_srv_db->cur_op = op;
        return true;
    }
    else
    {
        return false;
    }
}
#if LE_AUDIO_GROUP_SUPPORT
T_LE_AUDIO_CAUSE vocs_write_cp_by_group(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                                        uint8_t srv_instance_id,
                                        T_VOCS_CP_OP op,
                                        T_VOCS_CP_PARAM *p_param)
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
                if (vocs_write_cp(p_set_member->p_link->conn_handle, srv_instance_id, op, p_param) == false)
                {
                    cause = LE_AUDIO_CAUSE_REQ_FAILED;
                }
            }
        }
    }
    return cause;
}
#endif
static void vocs_client_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
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
                PROTOCOL_PRINT_ERROR1("vocs_client_dm_cback: not find the link, conn_id %d",
                                      param->conn_state.conn_id);
                break;
            }

            if (param->conn_state.state == GAP_CONN_STATE_DISCONNECTED)
            {
                PROTOCOL_PRINT_INFO0("vocs_client_dm_cback: GAP_CONN_STATE_DISCONNECTED");
                vocs_link_free(p_link);
            }
        }
        break;

    default:
        break;
    }
}

T_APP_RESULT vocs_client_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;
    T_VOCS_CLIENT_DB *p_vocs_db = vocs_check_link(conn_handle);
    T_VOCS_SRV_DB *p_vocs_srv_db = NULL;

    if (p_vocs_db == NULL && type != GATT_CLIENT_EVENT_DIS_DONE)
    {
        return APP_RESULT_APP_ERR;
    }

    switch (type)
    {
    case GATT_CLIENT_EVENT_DIS_DONE:
        {
            uint8_t srv_num = 0;
            T_VOCS_CLIENT_DIS_DONE dis_done = {0};

            if (p_client_cb_data->dis_done.is_found)
            {
                T_BLE_AUDIO_LINK *p_link = NULL;
                srv_num = p_client_cb_data->dis_done.srv_instance_num;
                p_link = ble_audio_link_find_by_conn_handle(conn_handle);
                if (p_link == NULL || srv_num == 0)
                {
                    return APP_RESULT_APP_ERR;
                }
                vocs_link_free(p_link);

                p_link->p_vocs_client = ble_audio_mem_zalloc(sizeof(T_VOCS_CLIENT_DB) + srv_num * sizeof(
                                                                 T_VOCS_SRV_DB));

                if (p_link->p_vocs_client == NULL)
                {
                    return APP_RESULT_APP_ERR;
                }

                p_vocs_db = (T_VOCS_CLIENT_DB *)p_link->p_vocs_client;
                p_vocs_db->srv_num = srv_num;
                for (uint8_t i = 0; i < srv_num; i++)
                {
                    p_vocs_db->srv_db[i].srv_data.srv_instance_id = i;
                    p_vocs_db->srv_db[i].cur_op = VOCS_CP_MAX;
                }
            }
            dis_done.conn_handle = conn_handle;
            dis_done.is_found = p_client_cb_data->dis_done.is_found;
            dis_done.srv_num = srv_num;
            dis_done.load_from_ftl = p_client_cb_data->dis_done.load_from_ftl;
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_VOCS_CLIENT_DIS_DONE, &dis_done);
        }
        break;

    case GATT_CLIENT_EVENT_READ_RESULT:
        {
            p_vocs_srv_db = vocs_find_srv_db(p_vocs_db, p_client_cb_data->read_result.srv_instance_id);
            T_VOCS_CLIENT_READ_RESULT read_result;

            if (p_vocs_srv_db == NULL)
            {
                return APP_RESULT_APP_ERR;
            }

            read_result.conn_handle = conn_handle;
            read_result.cause = p_client_cb_data->read_result.cause;
            read_result.srv_instance_id = p_client_cb_data->read_result.srv_instance_id;
            if (p_client_cb_data->read_result.char_uuid.p.uuid16 == VOCS_UUID_CHAR_VOLUME_OFFSET_STATE)
            {
                if (p_client_cb_data->read_result.value_size != 3)
                {
                    return APP_RESULT_INVALID_VALUE_SIZE;
                }
                read_result.type = VOCS_CHAR_OFFSET_STATE;
                LE_ARRAY_TO_UINT16(read_result.data.volume_offset.volume_offset,
                                   p_client_cb_data->read_result.p_value);
                read_result.data.volume_offset.change_counter = p_client_cb_data->read_result.p_value[2];
                p_vocs_srv_db->srv_data.volume_offset.volume_offset =
                    *(int16_t *)p_client_cb_data->read_result.p_value;
                p_vocs_srv_db->srv_data.volume_offset.change_counter =
                    p_client_cb_data->read_result.p_value[2];
                p_vocs_srv_db->srv_data.type_exist |= VOCS_VOLUME_OFFSET_STATE_FLAG;
            }
            else if (p_client_cb_data->read_result.char_uuid.p.uuid16 == VOCS_UUID_CHAR_AUDIO_LOCATION)
            {
                if (p_client_cb_data->read_result.value_size != 4)
                {
                    return APP_RESULT_INVALID_VALUE_SIZE;
                }

                read_result.type = VOCS_CHAR_AUDIO_LOCATION;
                memcpy(&read_result.data.audio_location, p_client_cb_data->read_result.p_value, 4);
                memcpy(&p_vocs_srv_db->srv_data.audio_location,
                       p_client_cb_data->read_result.p_value, 4);
                p_vocs_srv_db->srv_data.type_exist |= VOCS_AUDIO_LOCATION_FLAG;
            }
            else if (p_client_cb_data->read_result.char_uuid.p.uuid16 == VOCS_UUID_CHAR_AUDIO_OUTPUT_DES)
            {
                read_result.data.output_des.output_des_len = p_client_cb_data->read_result.value_size;
                read_result.data.output_des.p_output_des = p_client_cb_data->read_result.p_value;
                read_result.type = VOCS_CHAR_AUDIO_OUTPUT_DESC;
                if (vocs_cfg & AUDIO_DESCRIPTION_STORAGE)
                {
                    uint8_t *p_value = p_vocs_srv_db->srv_data.output_des.p_output_des;
                    uint16_t len = p_vocs_srv_db->srv_data.output_des.output_des_len;
                    p_vocs_srv_db->srv_data.output_des.output_des_len = 0;
                    if (len < p_client_cb_data->read_result.value_size)
                    {
                        if (p_value != NULL)
                        {
                            ble_audio_mem_free(p_value);
                            p_value = ble_audio_mem_zalloc(p_client_cb_data->read_result.value_size + 1);
                            p_vocs_srv_db->srv_data.output_des.p_output_des = p_value;
                        }
                    }

                    if (p_value != NULL)
                    {
                        memcpy(p_value, p_client_cb_data->read_result.p_value, p_client_cb_data->read_result.value_size);
                        p_vocs_srv_db->srv_data.output_des.output_des_len =
                            p_client_cb_data->read_result.value_size;
                    }
                    p_vocs_srv_db->srv_data.type_exist |= VOCS_AUDIO_OUTPUT_DES_FLAG;
                }
            }
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_VOCS_CLIENT_READ_RESULT, &read_result);
        }
        break;

    case GATT_CLIENT_EVENT_WRITE_RESULT:
        {
            if (p_client_cb_data->write_result.char_type == GATT_CHAR_VALUE &&
                p_client_cb_data->write_result.char_uuid.p.uuid16 == VOCS_UUID_CHAR_VOLUME_OFFSET_CP)
            {
                p_vocs_srv_db = vocs_find_srv_db(p_vocs_db, p_client_cb_data->write_result.srv_instance_id);

                if (p_vocs_srv_db == NULL)
                {
                    return APP_RESULT_APP_ERR;
                }

                T_VOCS_CLIENT_CP_RESULT cp_result;
                cp_result.conn_handle = conn_handle;
                cp_result.cause = p_client_cb_data->write_result.cause;
                cp_result.srv_instance_id = p_client_cb_data->write_result.srv_instance_id;
                cp_result.cp_op = p_vocs_srv_db->cur_op;
                p_vocs_srv_db->cur_op = VOCS_CP_MAX;
                ble_audio_mgr_dispatch(LE_AUDIO_MSG_VOCS_CLIENT_CP_RESULT, &cp_result);
            }
        }
        break;

    case GATT_CLIENT_EVENT_NOTIFY_IND:
        {
            p_vocs_srv_db = vocs_find_srv_db(p_vocs_db, p_client_cb_data->notify_ind.srv_instance_id);
            T_VOCS_CLIENT_NOTIFY notify_data;

            if (p_vocs_srv_db == NULL)
            {
                return APP_RESULT_APP_ERR;
            }
            notify_data.conn_handle = conn_handle;
            notify_data.srv_instance_id = p_client_cb_data->notify_ind.srv_instance_id;
            if (p_client_cb_data->notify_ind.char_uuid.p.uuid16 == VOCS_UUID_CHAR_VOLUME_OFFSET_STATE)
            {
                if (p_client_cb_data->notify_ind.value_size != 3)
                {
                    return APP_RESULT_INVALID_VALUE_SIZE;
                }
                notify_data.type = VOCS_CHAR_OFFSET_STATE;
                LE_ARRAY_TO_UINT16(notify_data.data.volume_offset.volume_offset,
                                   p_client_cb_data->notify_ind.p_value);
                notify_data.data.volume_offset.change_counter = p_client_cb_data->notify_ind.p_value[2];
                p_vocs_srv_db->srv_data.volume_offset.volume_offset =
                    *(int16_t *)p_client_cb_data->notify_ind.p_value;
                p_vocs_srv_db->srv_data.volume_offset.change_counter =
                    p_client_cb_data->notify_ind.p_value[2];
                p_vocs_srv_db->srv_data.type_exist |= VOCS_VOLUME_OFFSET_STATE_FLAG;
            }
            else if (p_client_cb_data->notify_ind.char_uuid.p.uuid16 == VOCS_UUID_CHAR_AUDIO_LOCATION)
            {
                if (p_client_cb_data->notify_ind.value_size != 4)
                {
                    return APP_RESULT_INVALID_VALUE_SIZE;
                }

                notify_data.type = VOCS_CHAR_AUDIO_LOCATION;
                memcpy(&notify_data.data.audio_location, p_client_cb_data->notify_ind.p_value, 4);
                memcpy(&p_vocs_srv_db->srv_data.audio_location,
                       p_client_cb_data->notify_ind.p_value, 4);
                p_vocs_srv_db->srv_data.type_exist |= VOCS_AUDIO_LOCATION_FLAG;
            }
            else if (p_client_cb_data->notify_ind.char_uuid.p.uuid16 == VOCS_UUID_CHAR_AUDIO_OUTPUT_DES)
            {
                notify_data.data.output_des.output_des_len = p_client_cb_data->notify_ind.value_size;
                notify_data.data.output_des.p_output_des = p_client_cb_data->notify_ind.p_value;
                notify_data.type = VOCS_CHAR_AUDIO_OUTPUT_DESC;
                if (vocs_cfg & AUDIO_DESCRIPTION_STORAGE)
                {
                    uint8_t *p_value = p_vocs_srv_db->srv_data.output_des.p_output_des;
                    uint16_t len = p_vocs_srv_db->srv_data.output_des.output_des_len;
                    p_vocs_srv_db->srv_data.output_des.output_des_len = 0;
                    if (len < p_client_cb_data->notify_ind.value_size)
                    {
                        if (p_value != NULL)
                        {
                            ble_audio_mem_free(p_value);
                            p_value = ble_audio_mem_zalloc(p_client_cb_data->notify_ind.value_size + 1);
                            p_vocs_srv_db->srv_data.output_des.p_output_des =
                                p_value;
                        }
                    }

                    if (p_value != NULL)
                    {
                        memcpy(p_value, p_client_cb_data->notify_ind.p_value, p_client_cb_data->notify_ind.value_size);
                        p_vocs_srv_db->srv_data.output_des.output_des_len =
                            p_client_cb_data->notify_ind.value_size;
                    }
                    p_vocs_srv_db->srv_data.type_exist |= VOCS_AUDIO_OUTPUT_DES_FLAG;
                }
            }
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_VOCS_CLIENT_NOTIFY, &notify_data);
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

bool vocs_get_srv_data(uint16_t conn_handle, uint8_t srv_instance_id, T_VOCS_SRV_DATA *p_value)
{
    T_VOCS_CLIENT_DB *p_client = vocs_check_link(conn_handle);
    if (p_client == NULL || srv_instance_id > p_client->srv_num || p_value == NULL)
    {
        PROTOCOL_PRINT_ERROR1("vocs_get_srv_data: not find data srv_instance_id: %d", srv_instance_id);
        return false;
    }
    memcpy(p_value, &p_client->srv_db[srv_instance_id].srv_data, sizeof(T_VOCS_SRV_DATA));
    return true;
}

bool vocs_client_init(uint8_t cfg)
{
    T_ATTR_UUID srv_uuid = {0};
    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = GATT_UUID_VOCS;
    vocs_cfg = cfg;
    if (gatt_client_spec_register(&srv_uuid, vocs_client_cb) == GAP_CAUSE_SUCCESS)
    {
        ble_dm_cback_register(vocs_client_dm_cback);
        return true;
    }
    return false;
}

#if LE_AUDIO_DEINIT
void vocs_client_deinit(void)
{
    vocs_cfg = 0;
    for (uint8_t k = 0; k < ble_audio_db.acl_link_num; k++)
    {
        vocs_link_free(&ble_audio_db.le_link[k]);
    }
}
#endif
#endif
