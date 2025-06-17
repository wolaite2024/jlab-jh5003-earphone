#include "trace.h"
#include "ble_audio_mgr.h"
#include "pacs_client.h"
#include "ble_link_util.h"
#include "ble_audio.h"
#include "pacs_def.h"
#include "ble_audio_dm.h"
#include "bap_int.h"
#include "pacs_client_int.h"

#if LE_AUDIO_PACS_CLIENT_SUPPORT

typedef struct
{
    uint8_t sink_pac_num;
    uint8_t source_pac_num;
    bool    sink_loc_writable;
    bool    source_loc_writable;

    bool read_pac_snk_pending;
    uint8_t pac_snk_char_instance_id;

    bool read_pac_src_pending;
    uint8_t pac_src_char_instance_id;
} T_PACS_CLIENT_DB;

#define pacs_check_link(conn_handle) pacs_check_link_int(__func__, conn_handle)

T_PACS_CLIENT_DB *pacs_check_link_int(const char *p_func_name, uint16_t conn_handle)
{
    T_BLE_AUDIO_LINK *p_link = NULL;

    p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL || p_link->p_pacs_client == NULL)
    {
        PROTOCOL_PRINT_ERROR2("pacs_check_link: %s failed, conn_handle 0x%x", TRACE_STRING(p_func_name),
                              conn_handle);
        return NULL;
    }
    return p_link->p_pacs_client;
}

bool pacs_read_char_value_int(uint16_t conn_handle, T_PACS_TYPE type, uint8_t instance_id)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    T_PACS_CLIENT_DB *p_pacs_db = pacs_check_link(conn_handle);
    if (p_pacs_db == NULL)
    {
        return false;
    }
    if (type == PACS_SINK_PAC  && p_pacs_db->read_pac_snk_pending)
    {
        return false;
    }
    if (type == PACS_SOURCE_PAC && p_pacs_db->read_pac_src_pending)
    {
        return false;
    }
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_PACS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = instance_id;

    switch (type)
    {
    case PACS_AUDIO_AVAILABLE_CONTEXTS:
        {
            char_uuid.p.uuid16 = PACS_UUID_CHAR_AUDIO_AVAILABILITY_CONTEXTS;
        }
        break;

    case PACS_AUDIO_SUPPORTED_CONTEXTS:
        {
            char_uuid.p.uuid16 = PACS_UUID_CHAR_SUPPORTED_AUDIO_CONTEXTS;
        }
        break;

    case PACS_SINK_AUDIO_LOC:
        {
            if (p_pacs_db->sink_pac_num == 0)
            {
                goto failed;
            }
            char_uuid.p.uuid16 = PACS_UUID_CHAR_SINK_AUDIO_LOCATIONS;
        }
        break;

    case PACS_SOURCE_AUDIO_LOC:
        {
            if (p_pacs_db->source_pac_num == 0)
            {
                goto failed;
            }
            char_uuid.p.uuid16 = PACS_UUID_CHAR_SOURCE_AUDIO_LOCATIONS;
        }
        break;

    case PACS_SINK_PAC:
        {
            if (p_pacs_db->sink_pac_num == 0)
            {
                goto failed;
            }
            char_uuid.p.uuid16 = PACS_UUID_CHAR_SINK_PAC;
        }
        break;

    case PACS_SOURCE_PAC:
        {
            if (p_pacs_db->source_pac_num == 0)
            {
                goto failed;
            }
            char_uuid.p.uuid16 = PACS_UUID_CHAR_SOURCE_PAC;
        }
        break;
    default:
        break;
    }

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_read(conn_handle, handle, NULL);
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        if (type == PACS_SINK_PAC)
        {
            p_pacs_db->read_pac_snk_pending = true;
            p_pacs_db->pac_snk_char_instance_id = instance_id;
        }
        else if (type == PACS_SOURCE_PAC)
        {
            p_pacs_db->read_pac_src_pending = true;
            p_pacs_db->pac_src_char_instance_id = instance_id;
        }
    }
    else
    {
        goto failed;
    }

    return true;
failed:
    PROTOCOL_PRINT_ERROR3("pacs_read_char_value_int: failed, conn_handle 0x%x, type %d, instance_id %d",
                          conn_handle, type, instance_id);
    if (type == PACS_SINK_PAC)
    {
        p_pacs_db->read_pac_snk_pending = false;
        p_pacs_db->pac_snk_char_instance_id = 0;
    }
    else if (type == PACS_SOURCE_PAC)
    {
        p_pacs_db->read_pac_src_pending = false;
        p_pacs_db->pac_src_char_instance_id = 0;
    }
    return false;
}

bool pacs_read_char_value(uint16_t conn_handle, T_PACS_TYPE type)
{
    return pacs_read_char_value_int(conn_handle, type, 0);
}

void pacs_enable_cccd(uint16_t conn_handle, T_PACS_CCCD_OP_TYPE type)
{
    T_ATTR_UUID srv_uuid;
    T_PACS_CLIENT_DB *p_pacs_db = pacs_check_link(conn_handle);
    if (p_pacs_db == NULL)
    {
        return;
    }
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_PACS;
    if (type == PACS_OP_ALL)
    {
        if (gatt_client_check_cccd_enabled(conn_handle, &srv_uuid, NULL) == false)
        {
            gatt_client_enable_srv_cccd(conn_handle, &srv_uuid, GATT_CLIENT_CONFIG_ALL);
        }
    }
}

bool pacs_write_sink_audio_locations(uint16_t conn_handle, uint32_t sink_audio_location)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    T_PACS_CLIENT_DB *p_pacs_db = pacs_check_link(conn_handle);

    if (p_pacs_db && p_pacs_db->sink_loc_writable)
    {
        srv_uuid.is_uuid16 = true;
        srv_uuid.instance_id = 0;
        srv_uuid.p.uuid16 = GATT_UUID_PACS;
        char_uuid.is_uuid16 = true;
        char_uuid.instance_id = 0;
        char_uuid.p.uuid16 = PACS_UUID_CHAR_SINK_AUDIO_LOCATIONS;
        if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
        {
            uint8_t data[4];
            LE_UINT32_TO_ARRAY(data, sink_audio_location);
            cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_REQ, handle, 4, data, NULL);
        }
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        PROTOCOL_PRINT_ERROR1("pacs_write_sink_audio_locations: failed, conn_handle 0x%x", conn_handle);
        return false;
    }
}

bool pacs_write_source_audio_locations(uint16_t conn_handle, uint32_t source_audio_location)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    T_PACS_CLIENT_DB *p_pacs_db = pacs_check_link(conn_handle);

    if (p_pacs_db && p_pacs_db->source_loc_writable)
    {
        srv_uuid.is_uuid16 = true;
        srv_uuid.instance_id = 0;
        srv_uuid.p.uuid16 = GATT_UUID_PACS;
        char_uuid.is_uuid16 = true;
        char_uuid.instance_id = 0;
        char_uuid.p.uuid16 = PACS_UUID_CHAR_SOURCE_AUDIO_LOCATIONS;
        if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
        {
            uint8_t data[4];
            LE_UINT32_TO_ARRAY(data, source_audio_location);
            cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_REQ, handle, 4, data, NULL);
        }
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        PROTOCOL_PRINT_ERROR1("pacs_write_source_audio_locations: failed, conn_handle 0x%x", conn_handle);
        return false;
    }
}

void pacs_client_handle_dis_done(uint16_t conn_handle, T_GATT_CLIENT_DIS_DONE *p_dis_done)
{
    T_PACS_CLIENT_DIS_DONE dis_done = {0};
    T_PACS_CLIENT_DB *p_pacs_db;
    uint16_t properties;

    if (p_dis_done->is_found)
    {
        T_BLE_AUDIO_LINK *p_link = NULL;
        T_ATTR_UUID srv_uuid;
        T_ATTR_UUID char_uuid;
        srv_uuid.is_uuid16 = true;
        srv_uuid.instance_id = 0;
        srv_uuid.p.uuid16 = GATT_UUID_PACS;
        char_uuid.is_uuid16 = true;
        char_uuid.instance_id = 0;

        p_link = ble_audio_link_find_by_conn_handle(conn_handle);
        if (p_link == NULL)
        {
            goto error;
        }

        if (p_link->p_pacs_client)
        {
            ble_audio_mem_free(p_link->p_pacs_client);
        }
        p_link->p_pacs_client = ble_audio_mem_zalloc(sizeof(T_PACS_CLIENT_DB));

        if (p_link->p_pacs_client == NULL)
        {
            goto error;
        }

        p_pacs_db = p_link->p_pacs_client;
        char_uuid.p.uuid16 = PACS_UUID_CHAR_SINK_PAC;
        p_pacs_db->sink_pac_num = gatt_client_get_char_num(conn_handle, &srv_uuid, &char_uuid);
        dis_done.sink_pac_num =  p_pacs_db->sink_pac_num;
        if (p_pacs_db->sink_pac_num)
        {
            char_uuid.p.uuid16 = PACS_UUID_CHAR_SINK_AUDIO_LOCATIONS;
            if (gatt_client_get_char_prop(conn_handle, &srv_uuid, &char_uuid, &properties) == false)
            {
                dis_done.sink_loc_exist = false;
            }
            else
            {
                if (properties & GATT_CHAR_PROP_WRITE)
                {
                    p_pacs_db->sink_loc_writable = true;
                    dis_done.sink_loc_writable = true;
                }
            }
        }
        char_uuid.p.uuid16 = PACS_UUID_CHAR_SOURCE_PAC;
        p_pacs_db->source_pac_num = gatt_client_get_char_num(conn_handle, &srv_uuid, &char_uuid);
        dis_done.source_pac_num = p_pacs_db->source_pac_num;
        if (p_pacs_db->source_pac_num)
        {
            char_uuid.p.uuid16 = PACS_UUID_CHAR_SOURCE_AUDIO_LOCATIONS;
            if (gatt_client_get_char_prop(conn_handle, &srv_uuid, &char_uuid, &properties) == false)
            {
                dis_done.source_loc_exist = false;
            }
            else
            {
                if (properties & GATT_CHAR_PROP_WRITE)
                {
                    p_pacs_db->source_loc_writable = true;
                    dis_done.source_loc_writable = true;
                }
            }
        }
    }
    dis_done.conn_handle = conn_handle;
    dis_done.is_found = p_dis_done->is_found;
    dis_done.load_from_ftl = p_dis_done->load_from_ftl;
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_PACS_CLIENT_DIS_DONE, &dis_done);
    return;
error:
    PROTOCOL_PRINT_ERROR1("pacs_client_handle_dis_done: failed, conn_handle 0x%x", conn_handle);
    return;
}

void pacs_client_handle_read_result(T_PACS_CLIENT_DB *p_pacs_db, uint16_t conn_handle,
                                    T_GATT_CLIENT_READ_RESULT *p_read_result)
{
    uint16_t cause = p_read_result->cause;
    T_PACS_CLIENT_READ_RESULT read_result = {0};

    read_result.conn_handle = conn_handle;

    switch (p_read_result->char_uuid.p.uuid16)
    {
    case PACS_UUID_CHAR_SINK_AUDIO_LOCATIONS:
    case PACS_UUID_CHAR_SOURCE_AUDIO_LOCATIONS:
        {
            if (p_read_result->char_uuid.p.uuid16 == PACS_UUID_CHAR_SINK_AUDIO_LOCATIONS)
            {
                read_result.type = PACS_SINK_AUDIO_LOC;
            }
            else
            {
                read_result.type = PACS_SOURCE_AUDIO_LOC;
            }

            if (cause == GAP_SUCCESS)
            {
                if (p_read_result->value_size != 4)
                {
                    cause = GAP_ERR | GAP_ERR_INVALID_PARAM;
                }
                else
                {
                    LE_ARRAY_TO_UINT32(read_result.data.audio_locations, p_read_result->p_value);
                }
            }
        }
        break;

    case PACS_UUID_CHAR_AUDIO_AVAILABILITY_CONTEXTS:
    case PACS_UUID_CHAR_SUPPORTED_AUDIO_CONTEXTS:
        {
            if (p_read_result->char_uuid.p.uuid16 == PACS_UUID_CHAR_AUDIO_AVAILABILITY_CONTEXTS)
            {
                read_result.type = PACS_AUDIO_AVAILABLE_CONTEXTS;
            }
            else
            {
                read_result.type = PACS_AUDIO_SUPPORTED_CONTEXTS;
            }

            if (cause == GAP_SUCCESS)
            {
                if (p_read_result->value_size != 4)
                {
                    cause = GAP_ERR | GAP_ERR_INVALID_PARAM;
                }
                else
                {
                    LE_ARRAY_TO_UINT16(read_result.data.contexts_data.sink_contexts, p_read_result->p_value);
                    LE_ARRAY_TO_UINT16(read_result.data.contexts_data.source_contexts, p_read_result->p_value + 2);
                }
            }
        }
        break;

    case PACS_UUID_CHAR_SINK_PAC:
    case PACS_UUID_CHAR_SOURCE_PAC:
        {
            uint8_t pac_num;
            if (p_read_result->char_uuid.p.uuid16 == PACS_UUID_CHAR_SINK_PAC)
            {
                read_result.type = PACS_SINK_PAC;
                pac_num = p_pacs_db->sink_pac_num;
            }
            else
            {
                read_result.type = PACS_SOURCE_PAC;
                pac_num = p_pacs_db->source_pac_num;
            }
            if (p_read_result->value_size < PAC_RECORD_MIN_LEN)
            {
                cause = GAP_ERR | GAP_ERR_INVALID_PARAM;
            }
            if (cause == GAP_SUCCESS)
            {
                if (p_read_result->char_uuid.p.uuid16 == PACS_UUID_CHAR_SINK_PAC)
                {
                    p_pacs_db->pac_snk_char_instance_id++;
                    if (p_pacs_db->pac_snk_char_instance_id == pac_num)
                    {
                        read_result.data.pac_data.is_complete = true;
                        read_result.data.pac_data.handle = p_read_result->handle;
                        read_result.data.pac_data.pac_record_len = p_read_result->value_size;
                        read_result.data.pac_data.p_record = p_read_result->p_value;
                        p_pacs_db->read_pac_snk_pending = false;
                        p_pacs_db->pac_snk_char_instance_id = 0;
                    }
                    else
                    {
                        p_pacs_db->read_pac_snk_pending = false;
                        if (pacs_read_char_value_int(conn_handle, read_result.type,
                                                     p_pacs_db->pac_snk_char_instance_id) == false)
                        {
                            cause = GAP_ERR | GAP_ERR_INVALID_PARAM;
                        }
                        else
                        {
                            read_result.data.pac_data.is_complete = false;
                            read_result.data.pac_data.handle = p_read_result->handle;
                            read_result.data.pac_data.pac_record_len = p_read_result->value_size;
                            read_result.data.pac_data.p_record = p_read_result->p_value;
                        }
                    }
                }
                else
                {
                    p_pacs_db->pac_src_char_instance_id++;
                    if (p_pacs_db->pac_src_char_instance_id == pac_num)
                    {
                        read_result.data.pac_data.is_complete = true;
                        read_result.data.pac_data.handle = p_read_result->handle;
                        read_result.data.pac_data.pac_record_len = p_read_result->value_size;
                        read_result.data.pac_data.p_record = p_read_result->p_value;
                        p_pacs_db->read_pac_src_pending = false;
                        p_pacs_db->pac_src_char_instance_id = 0;
                    }
                    else
                    {
                        p_pacs_db->read_pac_src_pending = false;
                        if (pacs_read_char_value_int(conn_handle, read_result.type,
                                                     p_pacs_db->pac_src_char_instance_id) == false)
                        {
                            cause = GAP_ERR | GAP_ERR_INVALID_PARAM;
                        }
                        else
                        {
                            read_result.data.pac_data.is_complete = false;
                            read_result.data.pac_data.handle = p_read_result->handle;
                            read_result.data.pac_data.pac_record_len = p_read_result->value_size;
                            read_result.data.pac_data.p_record = p_read_result->p_value;
                        }
                    }
                }
            }
        }
        break;

    default:
        break;
    }
    read_result.cause = cause;
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_PACS_CLIENT_READ_RESULT, &read_result);
}

void pacs_client_handle_notify(T_PACS_CLIENT_DB *p_pacs_db, uint16_t conn_handle,
                               T_GATT_CLIENT_NOTIFY_IND *p_notify)
{
    T_PACS_CLIENT_NOTIFY notify_data = {0};

    notify_data.conn_handle = conn_handle;

    switch (p_notify->char_uuid.p.uuid16)
    {
    case PACS_UUID_CHAR_SINK_AUDIO_LOCATIONS:
    case PACS_UUID_CHAR_SOURCE_AUDIO_LOCATIONS:
        {
            if (p_notify->char_uuid.p.uuid16 == PACS_UUID_CHAR_SINK_AUDIO_LOCATIONS)
            {
                notify_data.type = PACS_SINK_AUDIO_LOC;
            }
            else
            {
                notify_data.type = PACS_SOURCE_AUDIO_LOC;
            }

            if (p_notify->value_size != 4)
            {
                goto error;
            }
            else
            {
                LE_ARRAY_TO_UINT32(notify_data.data.audio_locations, p_notify->p_value);
            }
        }
        break;

    case PACS_UUID_CHAR_AUDIO_AVAILABILITY_CONTEXTS:
    case PACS_UUID_CHAR_SUPPORTED_AUDIO_CONTEXTS:
        {
            if (p_notify->char_uuid.p.uuid16 == PACS_UUID_CHAR_AUDIO_AVAILABILITY_CONTEXTS)
            {
                notify_data.type = PACS_AUDIO_AVAILABLE_CONTEXTS;
            }
            else
            {
                notify_data.type = PACS_AUDIO_SUPPORTED_CONTEXTS;
            }
            if (p_notify->value_size != 4)
            {
                goto error;
            }
            else
            {
                LE_ARRAY_TO_UINT16(notify_data.data.contexts_data.sink_contexts, p_notify->p_value);
                LE_ARRAY_TO_UINT16(notify_data.data.contexts_data.source_contexts, p_notify->p_value + 2);
            }
        }
        break;

    case PACS_UUID_CHAR_SINK_PAC:
    case PACS_UUID_CHAR_SOURCE_PAC:
        {
            if (p_notify->value_size < PAC_RECORD_MIN_LEN)
            {
                goto error;
            }
            if (p_notify->char_uuid.p.uuid16 == PACS_UUID_CHAR_SINK_PAC)
            {
                notify_data.type = PACS_SINK_PAC;
            }
            else
            {
                notify_data.type = PACS_SOURCE_PAC;
            }
            notify_data.data.pac_data.is_complete = true;
            notify_data.data.pac_data.handle = p_notify->handle;
            notify_data.data.pac_data.pac_record_len = p_notify->value_size;
            notify_data.data.pac_data.p_record = p_notify->p_value;
        }
        break;

    default:
        break;
    }
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_PACS_CLIENT_NOTIFY, &notify_data);
    return;
error:
    return;
}

void pacs_client_handle_write_result(T_PACS_CLIENT_DB *p_pacs_db, uint16_t conn_handle,
                                     T_GATT_CLIENT_WRITE_RESULT *p_write_result)
{
    if (p_write_result->char_type == GATT_CHAR_VALUE)
    {
        if (p_write_result->char_uuid.p.uuid16 == PACS_UUID_CHAR_SINK_AUDIO_LOCATIONS)
        {
            T_PACS_CLIENT_WRITE_SINK_AUDIO_LOC_RESULT write_result;
            write_result.conn_handle = conn_handle;
            write_result.cause = p_write_result->cause;
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_PACS_CLIENT_WRITE_SINK_AUDIO_LOC_RESULT, &write_result);
        }
        else if (p_write_result->char_uuid.p.uuid16 == PACS_UUID_CHAR_SOURCE_AUDIO_LOCATIONS)
        {
            T_PACS_CLIENT_WRITE_SOURCE_AUDIO_LOC_RESULT write_result;
            write_result.conn_handle = conn_handle;
            write_result.cause = p_write_result->cause;
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_PACS_CLIENT_WRITE_SOURCE_AUDIO_LOC_RESULT, &write_result);
        }
    }
}

T_APP_RESULT pacs_client_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;
    T_PACS_CLIENT_DB *p_pacs_db = pacs_check_link(conn_handle);

    if (p_pacs_db == NULL && type != GATT_CLIENT_EVENT_DIS_DONE)
    {
        goto error;
    }

    switch (type)
    {
    case GATT_CLIENT_EVENT_DIS_DONE:
        {
            PROTOCOL_PRINT_INFO2("[BAP][PACS] GATT_CLIENT_EVENT_DIS_DONE: is_found %d, srv_instance_num %d",
                                 p_client_cb_data->dis_done.is_found,
                                 p_client_cb_data->dis_done.srv_instance_num);
            pacs_client_handle_dis_done(conn_handle, &p_client_cb_data->dis_done);
        }
        break;

    case GATT_CLIENT_EVENT_READ_RESULT:
        {
            PROTOCOL_PRINT_INFO7("[BAP][PACS] GATT_CLIENT_EVENT_READ_RESULT: conn_handle 0x%x, cause 0x%x, char_type %d, srv_instance_id %d, uuid16[%d] 0x%x, handle 0x%x",
                                 conn_handle, p_client_cb_data->read_result.cause,
                                 p_client_cb_data->read_result.char_type,
                                 p_client_cb_data->read_result.srv_instance_id,
                                 p_client_cb_data->read_result.char_uuid.instance_id,
                                 p_client_cb_data->read_result.char_uuid.p.uuid16,
                                 p_client_cb_data->read_result.handle);
            PROTOCOL_PRINT_INFO2("[BAP][PACS] GATT_CLIENT_EVENT_READ_RESULT: value[%d] %b",
                                 p_client_cb_data->read_result.value_size,
                                 TRACE_BINARY(p_client_cb_data->read_result.value_size,
                                              p_client_cb_data->read_result.p_value));
            pacs_client_handle_read_result(p_pacs_db, conn_handle, &p_client_cb_data->read_result);
        }
        break;

    case GATT_CLIENT_EVENT_NOTIFY_IND:
        {
            PROTOCOL_PRINT_INFO7("[BAP][PACS] GATT_CLIENT_EVENT_NOTIFY_IND: conn_handle 0x%x, srv_instance_id %d, uuid16[%d] 0x%x, handle 0x%x, data[%d] %b",
                                 conn_handle,
                                 p_client_cb_data->notify_ind.srv_instance_id,
                                 p_client_cb_data->notify_ind.char_uuid.instance_id,
                                 p_client_cb_data->notify_ind.char_uuid.p.uuid16,
                                 p_client_cb_data->notify_ind.handle,
                                 p_client_cb_data->notify_ind.value_size,
                                 TRACE_BINARY(p_client_cb_data->notify_ind.value_size,
                                              p_client_cb_data->notify_ind.p_value));
            pacs_client_handle_notify(p_pacs_db, conn_handle, &p_client_cb_data->notify_ind);
        }

        break;

    case GATT_CLIENT_EVENT_WRITE_RESULT:
        {
            PROTOCOL_PRINT_INFO8("[BAP][BASS] GATT_CLIENT_EVENT_WRITE_RESULT: conn_handle 0x%x, cause 0x%x, type %d, char_type %d, srv_instance_id %d, uuid16[%d] 0x%x, handle 0x%x",
                                 conn_handle, p_client_cb_data->write_result.cause,
                                 p_client_cb_data->write_result.type,
                                 p_client_cb_data->write_result.char_type,
                                 p_client_cb_data->write_result.srv_instance_id,
                                 p_client_cb_data->write_result.char_uuid.instance_id,
                                 p_client_cb_data->write_result.char_uuid.p.uuid16,
                                 p_client_cb_data->write_result.handle);
            pacs_client_handle_write_result(p_pacs_db, conn_handle, &p_client_cb_data->write_result);
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
    return APP_RESULT_APP_ERR;
}

static void pacs_client_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
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
                PROTOCOL_PRINT_ERROR1("pacs_client_dm_cback: not find the link, conn_id %d",
                                      param->conn_state.conn_id);
                break;
            }
            if (param->conn_state.state == GAP_CONN_STATE_DISCONNECTED)
            {
                if (p_link->p_pacs_client != NULL)
                {
                    ble_audio_mem_free(p_link->p_pacs_client);
                    p_link->p_pacs_client = NULL;
                }
            }
        }
        break;

    default:
        break;
    }
}

bool pacs_client_init(void)
{
    T_ATTR_UUID srv_uuid = {0};
    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = GATT_UUID_PACS;
    gatt_client_spec_register(&srv_uuid, pacs_client_cb);
    ble_dm_cback_register(pacs_client_dm_cback);
    return true;
}

#if LE_AUDIO_DEINIT
void pacs_client_deinit(void)
{
    for (uint8_t k = 0; k < ble_audio_db.acl_link_num; k++)
    {
        if (ble_audio_db.le_link[k].p_pacs_client != NULL)
        {
            ble_audio_mem_free(ble_audio_db.le_link[k].p_pacs_client);
            ble_audio_db.le_link[k].p_pacs_client = NULL;
        }
    }
}
#endif
#endif

