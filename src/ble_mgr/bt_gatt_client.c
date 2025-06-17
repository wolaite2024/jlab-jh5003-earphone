#if CONFIG_REALTEK_BT_GATT_CLIENT_SUPPORT
#include <string.h>
#include "stdlib.h"
#include "os_queue.h"
#include "trace.h"
#include "bt_types.h"
#include "bt_gatt_client.h"
#include "bt_gatt_client_int.h"
#if F_BT_GATT_CLIENT_EXT_API
#include "profile_client_ext.h"
#endif
#include "profile_client.h"
#include "bt_gattc_storage.h"
#include "ble_mgr_int.h"
#include "gatt_service_client.h"

#define LE_AUDIO_DEBUG 1

T_OS_QUEUE spec_gattc_queue;
T_OS_QUEUE bt_gattc_queue;
static T_CLIENT_ID gattc_storage_cl_id = CLIENT_PROFILE_GENERAL_ID;
P_FUN_GATT_STORAGE_CB gatt_storage_cb = NULL;
uint16_t gattc_dis_mode = GATT_CLIENT_DISCOV_MODE_CLOSED;

static void gatt_client_send_dis_result(uint16_t conn_handle, T_GATTC_STORAGE_CB  *p_gattc_cb,
                                        bool load_from_ftl,
                                        bool is_success);
static void gatt_client_check_req(T_GATTC_STORAGE_CB  *p_gattc_cb);

static T_GATTC_STORAGE_CB *gatt_client_find(uint16_t conn_handle)
{
    uint8_t i;

    for (i = 0; i < bt_gattc_queue.count; i++)
    {
        T_GATTC_STORAGE_CB *p_gatt_client = (T_GATTC_STORAGE_CB *)os_queue_peek(&bt_gattc_queue, i);
        if (p_gatt_client->conn_handle == conn_handle)
        {
            return p_gatt_client;
        }
    }
    return NULL;
}

static T_GATTC_STORAGE_CB *gatt_client_find_by_conn_id(uint8_t conn_id)
{
    uint8_t i;

    for (i = 0; i < bt_gattc_queue.count; i++)
    {
        T_GATTC_STORAGE_CB *p_gatt_client = (T_GATTC_STORAGE_CB *)os_queue_peek(&bt_gattc_queue, i);
        if (p_gatt_client->conn_id == conn_id &&
            p_gatt_client->conn_type == T_GATTC_CONN_TYPE_LE)
        {
            return p_gatt_client;
        }
    }
    return NULL;
}

T_GATTC_STORAGE_CB  *gatt_client_check_link_int(const char *p_func_name,
                                                uint16_t conn_handle)
{
    T_GATTC_STORAGE_CB *p_gatt_client = NULL;

    p_gatt_client = gatt_client_find(conn_handle);
    if (p_gatt_client == NULL)
    {
        BTM_PRINT_ERROR2("gatt_client_check_link_int: failed %s, conn_handle 0x%x",
                         TRACE_STRING(p_func_name), conn_handle);
        return NULL;
    }

    if (p_gatt_client->conn_type == T_GATTC_CONN_TYPE_LE &&
        p_gatt_client->conn_state != GAP_CONN_STATE_CONNECTED)
    {
        BTM_PRINT_ERROR3("gatt_client_check_link_int: failed %s, conn_handle 0x%x, invalid state %d",
                         TRACE_STRING(p_func_name), conn_handle, p_gatt_client->conn_state);
        return NULL;
    }
    return p_gatt_client;
}

static T_GATTC_STORAGE_CB  *gatt_client_allocate(uint8_t conn_id)
{
    T_GATTC_STORAGE_CB *p_gatt_client = gatt_client_find_by_conn_id(conn_id);

    if (p_gatt_client)
    {
        return p_gatt_client;
    }

    p_gatt_client = (T_GATTC_STORAGE_CB *)calloc(1,
                                                 sizeof(T_GATTC_STORAGE_CB));
    if (p_gatt_client == NULL)
    {
        return NULL;
    }
    p_gatt_client->conn_id = conn_id;
    p_gatt_client->conn_type = T_GATTC_CONN_TYPE_LE;
    os_queue_in(&bt_gattc_queue, (void *)p_gatt_client);
    BTM_PRINT_INFO2("gatt_client_allocate: conn_id 0x%x, total count %d",
                    conn_id, bt_gattc_queue.count);
    return p_gatt_client;
}

#if BT_GATT_CLIENT_BREDR_SUPPORT
static T_GATTC_STORAGE_CB  *gatt_client_allocate_for_bredr(uint16_t conn_handle)
{
    T_GATTC_STORAGE_CB *p_gatt_client = gatt_client_find(conn_handle);

    if (p_gatt_client)
    {
        return p_gatt_client;
    }

    p_gatt_client = (T_GATTC_STORAGE_CB *)calloc(1, sizeof(T_GATTC_STORAGE_CB));

    if (p_gatt_client == NULL)
    {
        return NULL;
    }
    p_gatt_client->conn_handle = conn_handle;
    p_gatt_client->conn_type = T_GATTC_CONN_TYPE_BREDR;
    os_queue_in(&bt_gattc_queue, (void *)p_gatt_client);
    BTM_PRINT_INFO2("gatt_client_allocate_for_bredr: conn_handle 0x%x, total count %d",
                    conn_handle, bt_gattc_queue.count);
    return p_gatt_client;
}
#endif

static void gatt_client_del(T_GATTC_STORAGE_CB *p_gatt_client)
{
    if (p_gatt_client)
    {
        BTM_PRINT_INFO1("gatt_client_del: conn_handle 0x%x",
                        p_gatt_client->conn_handle);
        if (os_queue_delete(&bt_gattc_queue, (void *)p_gatt_client))
        {
            if (p_gatt_client->p_cccd_timer_handle)
            {
                os_timer_delete(&p_gatt_client->p_cccd_timer_handle);
            }
            free((void *) p_gatt_client);
        }
        else
        {
            BTM_PRINT_ERROR1("gatt_client_del: failed, not find conn_handle 0x%x",
                             p_gatt_client->conn_handle);
        }
    }
}

static T_GATTC_CHANNEL *gatt_client_channel_find_unused(T_GATTC_STORAGE_CB *p_gattc_cb,
                                                        uint16_t data_len)
{
    uint8_t i;

    for (i = 0; i < p_gattc_cb->channel_list.count; i++)
    {
        T_GATTC_CHANNEL *p_chann = (T_GATTC_CHANNEL *)os_queue_peek(&p_gattc_cb->channel_list, i);
        if (p_chann->is_req == false)
        {
            if (data_len == 0 ||
                p_chann->mtu_size >= data_len)
            {
                return p_chann;
            }

        }
    }
    return NULL;
}

static T_GATTC_CHANNEL *gatt_client_channel_find_by_cid(T_GATTC_STORAGE_CB *p_gattc_cb,
                                                        uint16_t cid)
{
    uint8_t i;

    for (i = 0; i < p_gattc_cb->channel_list.count; i++)
    {
        T_GATTC_CHANNEL *p_chann = (T_GATTC_CHANNEL *)os_queue_peek(&p_gattc_cb->channel_list, i);
        if (p_chann->cid == cid)
        {
            return p_chann;
        }
    }
    return NULL;
}

static T_GATTC_CHANNEL  *gatt_client_channel_allocate(T_GATTC_STORAGE_CB *p_gattc_cb, uint16_t cid)
{
    T_GATTC_CHANNEL *p_chann = gatt_client_channel_find_by_cid(p_gattc_cb, cid);

    if (p_chann)
    {
        return p_chann;
    }

    p_chann = (T_GATTC_CHANNEL *)calloc(1, sizeof(T_GATTC_CHANNEL));
    if (p_chann == NULL)
    {
        return NULL;
    }
    p_chann->cid = cid;
    p_chann->mtu_size = 23;
    os_queue_in(&p_gattc_cb->channel_list, (void *)p_chann);
    BTM_PRINT_INFO1("gatt_client_channel_allocate: cid 0x%x", cid);
    return p_chann;
}

static void gatt_client_channel_del(T_GATTC_STORAGE_CB *p_gattc_cb, T_GATTC_CHANNEL *p_chann)
{
    if (p_chann)
    {
        BTM_PRINT_INFO1("gatt_client_channel_del: cid 0x%x",
                        p_chann->cid);
        if (os_queue_delete(&p_gattc_cb->channel_list, (void *)p_chann))
        {
            if (p_chann->is_req)
            {
                p_gattc_cb->channel_list.flags--;
            }
            free((void *) p_chann);
        }
        else
        {
            BTM_PRINT_ERROR1("gatt_client_channel_del: failed, not find cid 0x%x",
                             p_chann->cid);
        }
    }
}

static void gatt_client_channel_release(T_GATTC_STORAGE_CB *p_gattc_cb)
{
    T_GATTC_CHANNEL *p_chann;

    while (p_gattc_cb->channel_list.count > 0)
    {
        p_chann = os_queue_out(&p_gattc_cb->channel_list);
        if (p_chann)
        {
            free(p_chann);
        }
    }
    p_gattc_cb->channel_list.flags = 0;
}

static void gatt_client_del_wait_rsp(T_GATTC_STORAGE_CB *p_gattc_cb, T_GATTC_REQ *p_req)
{
    if (p_req)
    {
        T_GATTC_CHANNEL *p_chann = gatt_client_channel_find_by_cid(p_gattc_cb, p_req->cid);
        if (p_chann)
        {
            if (p_chann->is_req == false)
            {
                BTM_PRINT_ERROR1("[CLIENT_REQ] gatt_client_del_wait_rsp: failed, cid 0x%x is_req error",
                                 p_req->cid);
            }
            else
            {
                p_gattc_cb->channel_list.flags--;
                p_chann->is_req = false;
            }
        }
        BTM_PRINT_INFO4("[CLIENT_REQ] gatt_client_del_wait_rsp: conn_handle 0x%x, cid 0x%x, req_type %d, handle 0x%x",
                        p_gattc_cb->conn_handle, p_req->cid, p_req->req_type, p_req->handle);
        if (os_queue_delete(&p_gattc_cb->gattc_wait_rsp_list, p_req))
        {
            free(p_req);
        }
        gatt_client_check_req(p_gattc_cb);
    }
}

static T_GATTC_REQ *gatt_client_find_wait_rsp(T_GATTC_STORAGE_CB *p_gattc_cb, uint16_t handle,
                                              uint16_t cid,
                                              bool is_read, uint16_t cause)
{
    for (uint8_t i = 0; i < p_gattc_cb->gattc_wait_rsp_list.count; i++)
    {
        T_GATTC_REQ *p_req = (T_GATTC_REQ *)os_queue_peek(&p_gattc_cb->gattc_wait_rsp_list, i);

        if (p_req->cid == cid)
        {
            if (is_read)
            {
                if (p_req->req_type == GATTC_REQ_TYPE_READ_UUID)
                {
                    if (cause != GAP_SUCCESS && handle == 0)
                    {
                        //Work around for not info the error handle
                        //The Attribute Handle In Error parameter shall be set to the Starting Handle parameter.
                        handle = p_req->p.read_uuid.start_handle;
                    }

                    if (handle >= p_req->p.read_uuid.start_handle &&
                        handle <= p_req->p.read_uuid.end_handle)
                    {
                        return p_req;
                    }
                }
                else if (p_req->req_type == GATTC_REQ_TYPE_READ)
                {
                    if (p_req->handle == handle)
                    {
                        return p_req;
                    }
                }
            }
            else
            {
                if (p_req->handle == handle)
                {
                    if (p_req->req_type == GATTC_REQ_TYPE_WRITE ||
                        p_req->req_type == GATTC_REQ_TYPE_CCCD)
                    {
                        return p_req;
                    }
                }
            }
        }
    }
    BTM_PRINT_ERROR3("[CLIENT_REQ] gatt_client_find_wait_rsp: failed, not find handle 0x%x, cid 0x%x, is_read %d",
                     handle, cid, is_read);
    return NULL;
}

T_SPEC_GATTC_CB *gatt_spec_client_find_by_uuid(T_ATTR_UUID *p_srv_uuid)
{
    for (uint8_t i = 0; i < spec_gattc_queue.count; i++)
    {
        T_SPEC_GATTC_CB *p_db = (T_SPEC_GATTC_CB *)os_queue_peek(&spec_gattc_queue, i);

        if (p_db && p_srv_uuid)
        {
            if (p_srv_uuid->is_uuid16)
            {
                if (p_db->srv_uuid.is_uuid16 &&
                    p_db->srv_uuid.p.uuid16 == p_srv_uuid->p.uuid16)
                {
                    return p_db;
                }
            }
            else
            {
                if (p_db->srv_uuid.is_uuid16 == false &&
                    memcmp(p_db->srv_uuid.p.uuid128, p_srv_uuid->p.uuid128, 16) == 0)
                {
                    return p_db;
                }
            }
        }
    }
    return NULL;
}

static void gattc_req_list_free(T_GATTC_STORAGE_CB *p_gattc_cb)
{
    T_GATTC_REQ *p_req;

    while (p_gattc_cb->gattc_req_list.count > 0)
    {
        p_req = os_queue_out(&p_gattc_cb->gattc_req_list);
        if (p_req)
        {
            if (p_req->req_type == GATTC_REQ_TYPE_WRITE && p_req->p.write.p_data)
            {
                free(p_req->p.write.p_data);
            }
            free(p_req);
        }
    }
    while (p_gattc_cb->gattc_write_cmd_list.count > 0)
    {
        p_req = os_queue_out(&p_gattc_cb->gattc_write_cmd_list);
        if (p_req)
        {
            if (p_req->p.write.p_data)
            {
                free(p_req->p.write.p_data);
            }
            free(p_req);
        }
    }
    while (p_gattc_cb->gattc_wait_rsp_list.count > 0)
    {
        p_req = os_queue_out(&p_gattc_cb->gattc_wait_rsp_list);
        if (p_req)
        {
            if (p_req->req_type == GATTC_REQ_TYPE_WRITE && p_req->p.write.p_data)
            {
                free(p_req->p.write.p_data);
            }
            free(p_req);
        }
    }
}

static void gatt_client_release_all(T_GATTC_STORAGE_CB *p_gatt_client)
{
    if (p_gatt_client != NULL)
    {
        T_GATT_CLIENT_CONN_DEL conn_del;

        BTM_PRINT_TRACE2("gatt_client_release_all: conn_handle 0x%x, conn_type %d",
                         p_gatt_client->conn_handle, p_gatt_client->conn_type);

        conn_del.transport_le = true;
#if BT_GATT_CLIENT_BREDR_SUPPORT
        if (p_gatt_client->conn_type == T_GATTC_CONN_TYPE_BREDR)
        {
            conn_del.transport_le = false;
        }
#endif
        for (uint8_t i = 0; i < spec_gattc_queue.count; i++)
        {
            T_SPEC_GATTC_CB *p_spec_cb = (T_SPEC_GATTC_CB *)os_queue_peek(&spec_gattc_queue, i);
            if (p_spec_cb && p_spec_cb->p_fun_cb != NULL)
            {
                p_spec_cb->p_fun_cb(p_gatt_client->conn_handle, GATT_CLIENT_EVENT_CONN_DEL, (void *)&conn_del);
            }
        }
        gattc_req_list_free(p_gatt_client);
        gattc_storage_dis_list_release(p_gatt_client);
        gattc_storage_srv_list_release(p_gatt_client);
        gatt_client_channel_release(p_gatt_client);
        gatt_client_del(p_gatt_client);
    }
}

static void gatt_client_handle_read_result(T_GATTC_STORAGE_CB  *p_gattc_cb,
                                           uint16_t cause, uint16_t handle, uint16_t value_size,
                                           uint8_t *p_value, T_GATTC_REQ *p_req)
{
    T_ATTR_DATA char_data;
    T_GATT_CLIENT_READ_RESULT read_result;
    T_ATTR_SRV_CB *p_srv_cb = gattc_storage_find_srv_by_handle(p_gattc_cb, handle);

    if (p_srv_cb)
    {
        if (gattc_storage_find_char_desc(p_srv_cb, handle, &read_result.char_type, &char_data) == false)
        {
            return;
        }
    }
    else
    {
        return;
    }

    read_result.cause = cause;
    read_result.handle = handle;
    read_result.value_size = value_size;
    read_result.p_value = p_value;
    read_result.srv_instance_id = p_srv_cb->p_srv_data->hdr.instance_id;
    att_data_covert_to_uuid(&char_data, &read_result.char_uuid);
#if 0
    BTM_PRINT_INFO5("gatt_client_handle_read_result: cause 0x%x, handle %d, char_type %d, instance_id %d, uuid16 0x%x",
                    cause, handle, read_result.char_type, read_result.char_uuid.instance_id,
                    read_result.char_uuid.p.uuid16);
#endif
    if (p_req->req_cb != NULL)
    {
        p_req->req_cb(p_gattc_cb->conn_handle, GATT_CLIENT_EVENT_READ_RESULT,
                      (void *)&read_result);
    }
    else
    {
        if (p_srv_cb->p_spec_cb != NULL && p_srv_cb->p_spec_cb->p_fun_cb != NULL)
        {
            p_srv_cb->p_spec_cb->p_fun_cb(p_gattc_cb->conn_handle, GATT_CLIENT_EVENT_READ_RESULT,
                                          (void *)&read_result);
        }
    }
}

static void gatt_client_handle_read_uuid_result(T_GATTC_STORAGE_CB  *p_gattc_cb,
                                                uint16_t cause, uint16_t handle, uint16_t value_size,
                                                uint8_t *p_value, T_GATTC_REQ *p_req)
{
    T_ATTR_DATA char_data;
    T_GATT_CLIENT_READ_RESULT read_result;
    T_ATTR_SRV_CB *p_srv_cb = gattc_storage_find_srv_by_handle(p_gattc_cb, handle);

    memset(&read_result, 0, sizeof(T_GATT_CLIENT_READ_RESULT));
    read_result.char_uuid.is_uuid16 = true;
    read_result.char_uuid.p.uuid16 = p_req->p.read_uuid.uuid16;
    if (p_srv_cb)
    {
        read_result.srv_instance_id = p_srv_cb->p_srv_data->hdr.instance_id;
        if (gattc_storage_find_char_desc(p_srv_cb, handle, &read_result.char_type, &char_data))
        {
            read_result.char_uuid.instance_id = char_data.hdr.instance_id;
        }
    }
    read_result.cause = cause;
    read_result.handle = handle;
    read_result.value_size = value_size;
    read_result.p_value = p_value;
    if (p_req->req_cb != NULL)
    {
        p_req->req_cb(p_gattc_cb->conn_handle, GATT_CLIENT_EVENT_READ_UUID_RESULT,
                      (void *)&read_result);
    }
    else
    {
        if (p_srv_cb && p_srv_cb->p_spec_cb != NULL && p_srv_cb->p_spec_cb->p_fun_cb != NULL)
        {
            p_srv_cb->p_spec_cb->p_fun_cb(p_gattc_cb->conn_handle, GATT_CLIENT_EVENT_READ_UUID_RESULT,
                                          (void *)&read_result);
        }
    }
}

static void gatt_client_handle_write_result(T_GATTC_STORAGE_CB  *p_gattc_cb,
                                            T_GATT_WRITE_TYPE type,
                                            uint16_t handle, uint16_t cause, T_GATTC_REQ *p_req)
{
    T_ATTR_DATA char_data;
    T_ATTR_SRV_CB *p_srv_cb = gattc_storage_find_srv_by_handle(p_gattc_cb, handle);
    T_GATT_CLIENT_WRITE_RESULT write_result;

    if (p_srv_cb)
    {
        if (gattc_storage_find_char_desc(p_srv_cb, handle, &write_result.char_type, &char_data) == false)
        {
            return;
        }
    }
    else
    {
        return;
    }

    write_result.cause = cause;
    write_result.handle = handle;
    write_result.type = type;
    write_result.srv_instance_id = p_srv_cb->p_srv_data->hdr.instance_id;
    att_data_covert_to_uuid(&char_data, &write_result.char_uuid);
    if (p_req != NULL && p_req->req_cb != NULL)
    {
        p_req->req_cb(p_gattc_cb->conn_handle, GATT_CLIENT_EVENT_WRITE_RESULT,
                      (void *)&write_result);
    }
    else
    {
        if (p_srv_cb->p_spec_cb != NULL && p_srv_cb->p_spec_cb->p_fun_cb != NULL)
        {
            p_srv_cb->p_spec_cb->p_fun_cb(p_gattc_cb->conn_handle, GATT_CLIENT_EVENT_WRITE_RESULT,
                                          (void *)&write_result);
        }
    }
}

static void gatt_client_send_cccd_cfg_result(T_GATTC_STORAGE_CB  *p_gattc_cb, uint16_t cause,
                                             T_GATTC_REQ *p_req)
{
    T_GATT_CHAR_TYPE char_type;
    T_ATTR_DATA char_data;
    T_ATTR_SRV_CB *p_srv_cb = gattc_storage_find_srv_by_handle(p_gattc_cb, p_req->handle);

    if (p_srv_cb)
    {
        if (gattc_storage_find_char_desc(p_srv_cb, p_req->handle, &char_type, &char_data) == false)
        {
            return;
        }
    }
    else
    {
        return;
    }
    if (p_srv_cb->p_spec_cb != NULL &&
        p_srv_cb->p_spec_cb->p_fun_cb != NULL)
    {
        T_GATT_CLIENT_CCCD_CFG cccd_cfg = {0};
        cccd_cfg.cause = cause;
        cccd_cfg.srv_instance_id = p_srv_cb->p_srv_data->hdr.instance_id;
        cccd_cfg.cccd_cfg = p_req->p.cccd_cfg.cfg_value;
        cccd_cfg.srv_cfg = p_req->p.cccd_cfg.srv_cfg;
        if (cccd_cfg.srv_cfg)
        {
            att_data_covert_to_uuid(p_srv_cb->p_srv_data, &cccd_cfg.srv_uuid);
        }
        else
        {
            att_data_covert_to_uuid(p_srv_cb->p_srv_data, &cccd_cfg.srv_uuid);
            att_data_covert_to_uuid(&char_data, &cccd_cfg.char_uuid);
        }
        p_srv_cb->p_spec_cb->p_fun_cb(p_gattc_cb->conn_handle, GATT_CLIENT_EVENT_CCCD_CFG,
                                      (void *)&cccd_cfg);
    }
}

static void gatt_client_handle_req_result(T_GATTC_STORAGE_CB  *p_gattc_cb, T_GATTC_REQ *p_req,
                                          uint16_t cause)
{
    if (p_req->req_type == GATTC_REQ_TYPE_READ)
    {
        gatt_client_handle_read_result(p_gattc_cb, cause,
                                       p_req->handle, 0, NULL, p_req);
    }
    else if (p_req->req_type == GATTC_REQ_TYPE_READ_UUID)
    {
        gatt_client_handle_read_uuid_result(p_gattc_cb, cause,
                                            p_req->handle, 0, NULL, p_req);
    }
    else if (p_req->req_type == GATTC_REQ_TYPE_WRITE)
    {
        gatt_client_handle_write_result(p_gattc_cb, p_req->p.write.write_type,
                                        p_req->handle, cause, p_req);
    }
    else if (p_req->req_type == GATTC_REQ_TYPE_CCCD)
    {
        if (p_req->p.cccd_cfg.cfg_end)
        {
            gatt_client_send_cccd_cfg_result(p_gattc_cb, cause, p_req);
        }
    }
}

static T_GAP_CAUSE gatt_client_execute_req(T_GATTC_STORAGE_CB  *p_gattc_cb,
                                           T_GATTC_REQ *p_req)
{
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    if (p_req->req_type == GATTC_REQ_TYPE_READ)
    {
#if F_BT_GATT_CLIENT_EXT_API
        cause = client_ext_attr_read(p_gattc_cb->conn_handle, p_req->cid, gattc_storage_cl_id,
                                     p_req->handle);
#else
        cause = client_attr_read(p_gattc_cb->conn_id, gattc_storage_cl_id, p_req->handle);
#endif
    }
    else if (p_req->req_type == GATTC_REQ_TYPE_READ_UUID)
    {
#if F_BT_GATT_CLIENT_EXT_API
        cause = client_ext_attr_read_using_uuid(p_gattc_cb->conn_handle, p_req->cid, gattc_storage_cl_id,
                                                p_req->p.read_uuid.start_handle,
                                                p_req->p.read_uuid.end_handle, p_req->p.read_uuid.uuid16, NULL);
#else
        cause = client_attr_read_using_uuid(p_gattc_cb->conn_id, gattc_storage_cl_id,
                                            p_req->p.read_uuid.start_handle,
                                            p_req->p.read_uuid.end_handle, p_req->p.read_uuid.uuid16, NULL);
#endif
    }
    else if (p_req->req_type == GATTC_REQ_TYPE_WRITE)
    {
#if F_BT_GATT_CLIENT_EXT_API
        cause = client_ext_attr_write(p_gattc_cb->conn_handle, p_req->cid, gattc_storage_cl_id,
                                      p_req->p.write.write_type,
                                      p_req->handle,
                                      p_req->p.write.length, p_req->p.write.p_data);
#else
        cause = client_attr_write(p_gattc_cb->conn_id, gattc_storage_cl_id, p_req->p.write.write_type,
                                  p_req->handle,
                                  p_req->p.write.length, p_req->p.write.p_data);
#endif

        if (p_req->p.write.p_data)
        {
            free(p_req->p.write.p_data);
            p_req->p.write.p_data = NULL;
            p_req->p.write.length = 0;
        }
    }
    else if (p_req->req_type == GATTC_REQ_TYPE_CCCD)
    {
        uint8_t data[2];
        LE_UINT16_TO_ARRAY(data, p_req->p.cccd_cfg.cfg_value);
#if F_BT_GATT_CLIENT_EXT_API
        cause = client_ext_attr_write(p_gattc_cb->conn_handle, p_req->cid, gattc_storage_cl_id,
                                      GATT_WRITE_TYPE_REQ,
                                      p_req->handle, 2, data);
#else
        cause = client_attr_write(p_gattc_cb->conn_id, gattc_storage_cl_id, GATT_WRITE_TYPE_REQ,
                                  p_req->handle, 2, data);
#endif
    }

    BTM_PRINT_INFO5("[CLIENT_REQ] Send req: conn_handle 0x%x, cid 0x%x, req_type %d, handle 0x%x, cause 0x%x",
                    p_gattc_cb->conn_handle, p_req->cid, p_req->req_type, p_req->handle, cause);
    if (cause == GAP_CAUSE_SUCCESS)
    {
        os_queue_in(&p_gattc_cb->gattc_wait_rsp_list, p_req);
    }
    else
    {
        BTM_PRINT_ERROR4("[CLIENT_REQ] gatt_client_execute_req: conn_handle 0x%x, cause %d, req_type %d, handle 0x%x",
                         p_gattc_cb->conn_handle, cause, p_req->req_type, p_req->handle);
    }
    return cause;
}

static void gatt_client_check_req(T_GATTC_STORAGE_CB  *p_gattc_cb)
{
    T_GATTC_REQ *p_req;
    T_GATTC_CHANNEL *p_chann;

    BTM_PRINT_INFO5("[CLIENT_REQ] gatt_client_check_req: conn_handle 0x%x, req count %d, rsp count %d, channel used %d, channel count %d",
                    p_gattc_cb->conn_handle, p_gattc_cb->gattc_req_list.count,
                    p_gattc_cb->gattc_wait_rsp_list.count,
                    p_gattc_cb->channel_list.flags, p_gattc_cb->channel_list.count);

    for (uint8_t idx = 0; idx < p_gattc_cb->gattc_req_list.count;)
    {
        if (p_gattc_cb->channel_list.flags < p_gattc_cb->channel_list.count)
        {
            T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
            bool queue_delete = false;
            p_req = (T_GATTC_REQ *)os_queue_peek(&p_gattc_cb->gattc_req_list, idx);
            if (p_req)
            {
                uint16_t data_len = 0;
                if (p_req->req_type == GATTC_REQ_TYPE_WRITE)
                {
                    data_len = p_req->p.write.length + 3;
                }
                p_chann = gatt_client_channel_find_unused(p_gattc_cb, data_len);
                if (p_chann)
                {
                    if (os_queue_delete(&p_gattc_cb->gattc_req_list, p_req))
                    {
                        queue_delete = true;
                        p_req->cid = p_chann->cid;
                        cause = gatt_client_execute_req(p_gattc_cb, p_req);
                        if (cause == GAP_CAUSE_SUCCESS)
                        {
                            p_chann->is_req = true;
                            p_gattc_cb->channel_list.flags++;
                        }
                        else
                        {
                            BTM_PRINT_ERROR4("[CLIENT_REQ] gatt_client_check_req: conn_handle 0x%x, cause %d, req_type %d, handle 0x%x",
                                             p_gattc_cb->conn_handle, cause, p_req->req_type, p_req->handle);
                            gatt_client_handle_req_result(p_gattc_cb, p_req, GAP_ERR | GAP_ERR_REQ_FAILED);
                            free(p_req);
                        }
                    }
                    else
                    {
                        BTM_PRINT_ERROR1("[CLIENT_REQ] gatt_client_check_req: failed, conn_handle 0x%x, os_queue_delete failed",
                                         p_gattc_cb->conn_handle);
                        break;
                    }
                }
            }
            else
            {
                BTM_PRINT_ERROR1("[CLIENT_REQ] gatt_client_check_req: failed, conn_handle 0x%x, os_queue_peek failed",
                                 p_gattc_cb->conn_handle);
                break;
            }

            if (queue_delete == false)
            {
                idx++;
            }
        }
        else
        {
            return;
        }
    }
}

static T_GAP_CAUSE gatt_client_check_write_cmd(uint16_t conn_handle,
                                               T_GATTC_STORAGE_CB  *p_gattc_cb)
{
    T_GAP_CAUSE cause = GAP_CAUSE_SUCCESS;
    uint8_t credit = 0;
    T_GATTC_REQ *p_req;

    while (p_gattc_cb->gattc_write_cmd_list.count != 0)
    {
        le_get_gap_param(GAP_PARAM_LE_REMAIN_CREDITS, &credit);
        BTM_PRINT_INFO2("[CLIENT_REQ] gatt_client_check_write_cmd: count 0x%x, credit 0x%x",
                        p_gattc_cb->gattc_write_cmd_list.count, credit);
        if (credit == 0)
        {
            break;
        }

        p_req = os_queue_out(&p_gattc_cb->gattc_write_cmd_list);
        if (p_req)
        {
#if F_BT_GATT_CLIENT_EXT_API
            cause = client_ext_attr_write(p_gattc_cb->conn_handle, 0, gattc_storage_cl_id,
                                          p_req->p.write.write_type,
                                          p_req->handle, p_req->p.write.length, p_req->p.write.p_data);
#else
            cause = client_attr_write(p_gattc_cb->conn_id, gattc_storage_cl_id, p_req->p.write.write_type,
                                      p_req->handle, p_req->p.write.length, p_req->p.write.p_data);
#endif
            if (cause != GAP_CAUSE_SUCCESS)
            {
                gatt_client_handle_write_result(p_gattc_cb, p_req->p.write.write_type, p_req->handle,
                                                GAP_ERR | GAP_ERR_REQ_FAILED, p_req);
            }

            if (p_req->p.write.p_data)
            {
                free(p_req->p.write.p_data);
            }

            free(p_req);
        }
    }
    return cause;
}

T_GAP_CAUSE gatt_client_start_discovery(T_GATTC_STORAGE_CB  *p_gattc_cb, bool load_from_storage,
                                        bool database_hash_check)
{
    T_GAP_CAUSE cause;

    BTM_PRINT_INFO3("gatt_client_start_discovery: conn_handle 0x%x, load_from_storage %d, database_hash_check %d",
                    p_gattc_cb->conn_handle, load_from_storage, database_hash_check);

    if (load_from_storage)
    {
        if (gattc_storage_load(p_gattc_cb, database_hash_check))
        {
            p_gattc_cb->state = GATT_CLIENT_STATE_DONE;
#if LE_AUDIO_DEBUG
            gattc_storage_print(p_gattc_cb);
#endif
            p_gattc_cb->load_from_ftl = true;
            gatt_client_send_dis_result(p_gattc_cb->conn_handle, p_gattc_cb, true, true);
            return GAP_CAUSE_SUCCESS;
        }
    }

    p_gattc_cb->load_from_ftl = false;
    p_gattc_cb->state = GATT_CLIENT_STATE_IDLE;
    p_gattc_cb->p_curr_srv = NULL;

#if F_BT_GATT_CLIENT_EXT_API
    cause = client_ext_all_primary_srv_discovery(p_gattc_cb->conn_handle, 0, gattc_storage_cl_id);
#else
    cause = client_all_primary_srv_discovery(p_gattc_cb->conn_id, gattc_storage_cl_id);
#endif

    if (cause != GAP_CAUSE_SUCCESS)
    {
        BTM_PRINT_ERROR2("gatt_client_start_discovery: failed, conn_handle 0x%x, cause %d",
                         p_gattc_cb->conn_handle, cause);
    }
    else
    {
        p_gattc_cb->state = GATT_CLIENT_STATE_DISCOVERY;
        if (p_gattc_cb->p_general_cb)
        {
            T_GATT_CLIENT_DIS_ALL_DONE dis_all_done;
            dis_all_done.state = p_gattc_cb->state;
            dis_all_done.load_from_ftl = false;
            p_gattc_cb->p_general_cb(p_gattc_cb->conn_handle, GATT_CLIENT_EVENT_DIS_ALL_STATE,
                                     (void *)&dis_all_done);
        }
    }

    return cause;
}

T_APP_RESULT gatt_client_read_database_hash_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                               void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_check_link(conn_handle);
    bool discovery_flow = true;
    bool load_from_storage = true;
    bool database_hash_check = false;

    if (p_gattc_cb == NULL)
    {
        return result;
    }

    gatt_service_load_data(p_gattc_cb);

    BTM_PRINT_INFO1("gatt_client_read_database_hash_cb: cause 0x%x",
                    p_client_cb_data->read_result.cause);

    if (p_client_cb_data->read_result.cause == GAP_SUCCESS)
    {
        if (p_client_cb_data->read_result.value_size == GATT_SVC_DATABASE_HASH_LEN)
        {
            database_hash_check = true;
            if (p_gattc_cb->gatt_svc_data.data_exist & GATT_SVC_DATABASE_HASH_FLAG)
            {
                BTM_PRINT_INFO2("gatt_client_read_database_hash_cb: database_hash %b, read value %b",
                                TRACE_BINARY(GATT_SVC_DATABASE_HASH_LEN, p_gattc_cb->gatt_svc_data.database_hash),
                                TRACE_BINARY(GATT_SVC_DATABASE_HASH_LEN, p_client_cb_data->read_result.p_value));

                if (memcmp(p_gattc_cb->gatt_svc_data.database_hash, p_client_cb_data->read_result.p_value,
                           GATT_SVC_DATABASE_HASH_LEN) != 0)
                {
                    //Database hash is not match, clear the service table.
                    load_from_storage = false;
                    memset(&p_gattc_cb->gatt_svc_data, 0, sizeof(T_GATT_SERVICE_DATA));
                    p_gattc_cb->gatt_svc_data.data_exist |= GATT_SVC_DATABASE_HASH_FLAG;
                    memcpy(p_gattc_cb->gatt_svc_data.database_hash,
                           p_client_cb_data->read_result.p_value, GATT_SVC_DATABASE_HASH_LEN);
                    goto start_discovery;
                }
                else
                {
                    database_hash_check = false;
                }
            }
            p_gattc_cb->gatt_svc_data.data_exist |= GATT_SVC_DATABASE_HASH_FLAG;
            memcpy(p_gattc_cb->gatt_svc_data.database_hash,
                   p_client_cb_data->read_result.p_value, GATT_SVC_DATABASE_HASH_LEN);
        }
    }

start_discovery:
    if (discovery_flow)
    {
        gatt_client_start_discovery(p_gattc_cb, load_from_storage, database_hash_check);
    }

    return result;
}

T_GAP_CAUSE gatt_client_start_discovery_all(uint16_t conn_handle, P_FUN_GATT_CLIENT_CB p_general_cb)
{
    T_GAP_CAUSE cause;
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_check_link(conn_handle);
    bool load_from_storage = true;


    if (p_gattc_cb == NULL)
    {
        return GAP_CAUSE_INVALID_STATE;
    }

    BTM_PRINT_INFO2("gatt_client_start_discovery_all: conn_handle 0x%x, state %d",
                    p_gattc_cb->conn_handle, p_gattc_cb->state);

    if (p_gattc_cb->state == GATT_CLIENT_STATE_DONE)
    {
        gattc_req_list_free(p_gattc_cb);
        gattc_storage_srv_list_release(p_gattc_cb);
        load_from_storage = false;
    }
    else if (p_gattc_cb->state == GATT_CLIENT_STATE_DISCOVERY)
    {
        return GAP_CAUSE_ALREADY_IN_REQ;
    }
    p_gattc_cb->p_general_cb = p_general_cb;

    if ((gattc_dis_mode & GATT_CLIENT_DISCOV_MODE_GATT_SVC) &&
        (client_supported_feature & GATT_SVC_CLIENT_SUPPORTED_FEATURES_ROBUST_CACHING_BIT))
    {
        cause = gatt_client_read_uuid(p_gattc_cb->conn_handle, 0x0001, 0xFFFF, GATT_UUID_CHAR_DATABASE_HASH,
                                      gatt_client_read_database_hash_cb);
        if (cause == GAP_CAUSE_SUCCESS)
        {
            p_gattc_cb->state = GATT_CLIENT_STATE_DISCOVERY;
            return cause;
        }
    }

    cause = gatt_client_start_discovery(p_gattc_cb, load_from_storage, false);

    return cause;
}

static bool gatt_client_check_cccd_prop(uint16_t properties, uint8_t cccd_cfg,
                                        uint16_t *p_cfg_value)
{
    if (properties & (GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_INDICATE))
    {
        *p_cfg_value = 0;
        if (cccd_cfg == 0)
        {
            return true;
        }

        if (cccd_cfg & GATT_CLIENT_CONFIG_NOTIFICATION)
        {
            if (properties & GATT_CHAR_PROP_NOTIFY)
            {
                *p_cfg_value |= GATT_CLIENT_CONFIG_NOTIFICATION;
            }
        }

        if (cccd_cfg & GATT_CLIENT_CONFIG_INDICATION)
        {
            if (properties & GATT_CHAR_PROP_INDICATE)
            {
                *p_cfg_value |= GATT_CLIENT_CONFIG_INDICATION;
            }
        }

        if (*p_cfg_value != 0)
        {
            return true;
        }
    }
    return false;
}

T_GAP_CAUSE gatt_client_enable_char_cccd(uint16_t conn_handle, T_ATTR_UUID *p_srv_uuid,
                                         T_ATTR_UUID *p_char_uuid, uint8_t cccd_cfg)
{
    bool is_found = false;
    T_GATTC_REQ *p_cccd_req = NULL;
    T_GAP_CAUSE cause = GAP_CAUSE_SUCCESS;
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_check_link(conn_handle);
    T_ATTR_SRV_CB *p_srv_cb;
    uint16_t cfg_value;

    if (p_gattc_cb == NULL || p_gattc_cb->state != GATT_CLIENT_STATE_DONE)
    {
        cause = GAP_CAUSE_INVALID_STATE;
        goto result;
    }
    p_srv_cb = gattc_storage_find_srv_by_uuid(p_gattc_cb, p_srv_uuid);
    if (p_srv_cb == NULL)
    {
        cause = GAP_CAUSE_NOT_FIND;
        goto result;
    }

    if (p_char_uuid == NULL || p_char_uuid->is_uuid16 == true)
    {
        for (uint8_t j = 0; j < p_srv_cb->char16_num; j++)
        {
            T_ATTR_CHAR16_CB *p_attr_char = &p_srv_cb->p_char16_table[j];

            if (p_char_uuid != NULL && p_char_uuid->p.uuid16 != p_attr_char->char_data.uuid16)
            {
                continue;
            }

            if (p_attr_char->cccd_descriptor.hdr.att_handle)
            {
                if (gatt_client_check_cccd_prop(p_attr_char->char_data.properties, cccd_cfg, &cfg_value))
                {
                    is_found = true;
                    p_cccd_req = calloc(1, sizeof(T_GATTC_REQ));
                    if (p_cccd_req == NULL)
                    {
                        goto result;
                    }
                    p_cccd_req->req_type = GATTC_REQ_TYPE_CCCD;
                    p_cccd_req->handle = p_attr_char->cccd_descriptor.hdr.att_handle;
                    if (p_char_uuid == NULL)
                    {
                        p_cccd_req->p.cccd_cfg.srv_cfg = true;
                    }
                    p_cccd_req->p.cccd_cfg.cfg_end = false;
                    p_cccd_req->p.cccd_cfg.cfg_value = cfg_value;
                    BTM_PRINT_INFO2("[CLIENT_REQ] write cccd: 11 conn_handle 0x%x, handle 0x%x", conn_handle,
                                    p_cccd_req->handle);
                    os_queue_in(&p_gattc_cb->gattc_req_list, p_cccd_req);
                }
            }
        }
    }
    if (p_char_uuid == NULL || p_char_uuid->is_uuid16 == false)
    {
        for (uint8_t j = 0; j < p_srv_cb->char128_num; j++)
        {
            T_ATTR_CHAR128_CB *p_attr_char = &p_srv_cb->p_char128_table[j];

            if (p_char_uuid != NULL && memcmp(p_char_uuid->p.uuid128, p_attr_char->char_data.uuid128, 16) != 0)
            {
                continue;
            }

            if (p_attr_char->cccd_descriptor.hdr.att_handle)
            {
                if (gatt_client_check_cccd_prop(p_attr_char->char_data.properties, cccd_cfg, &cfg_value))
                {
                    is_found = true;
                    p_cccd_req = calloc(1, sizeof(T_GATTC_REQ));
                    if (p_cccd_req == NULL)
                    {
                        goto result;
                    }
                    p_cccd_req->req_type = GATTC_REQ_TYPE_CCCD;
                    p_cccd_req->handle = p_attr_char->cccd_descriptor.hdr.att_handle;
                    if (p_char_uuid == NULL)
                    {
                        p_cccd_req->p.cccd_cfg.srv_cfg = true;
                    }
                    p_cccd_req->p.cccd_cfg.cfg_end = false;
                    p_cccd_req->p.cccd_cfg.cfg_value = cfg_value;
                    BTM_PRINT_INFO2("[CLIENT_REQ] write cccd: 22 conn_handle 0x%x, handle 0x%x", conn_handle,
                                    p_cccd_req->handle);
                    os_queue_in(&p_gattc_cb->gattc_req_list, p_cccd_req);
                }
            }
        }
    }

    if (is_found && p_cccd_req != NULL)
    {
        p_cccd_req->p.cccd_cfg.cfg_end = true;
        gatt_client_check_req(p_gattc_cb);
    }
result:
    if (cause != GAP_CAUSE_SUCCESS)
    {
        if (p_char_uuid)
        {
            BTM_PRINT_ERROR5("[CLIENT_REQ] gatt_client_enable_char_cccd: failed, cause 0x%x, conn_handle 0x%x, is_found %d, srv uuid 0x%x, char uuid 0x%x",
                             cause, conn_handle, is_found,
                             p_srv_uuid->p.uuid16, p_char_uuid->p.uuid16);
        }
        else
        {
            BTM_PRINT_ERROR4("[CLIENT_REQ] gatt_client_enable_char_cccd: failed, cause 0x%x, conn_handle 0x%x, is_found %d, srv uuid 0x%x",
                             cause, conn_handle, is_found, p_srv_uuid->p.uuid16);
        }

    }
    return cause;
}

T_GAP_CAUSE gatt_client_enable_srv_cccd(uint16_t conn_handle, T_ATTR_UUID *p_srv_uuid,
                                        uint8_t cccd_cfg)
{
    return gatt_client_enable_char_cccd(conn_handle, p_srv_uuid, NULL, cccd_cfg);
}

bool gatt_client_get_char_cccd(uint16_t conn_handle, uint16_t handle, uint16_t *p_ccc_bits)
{
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_check_link(conn_handle);

    if (p_gattc_cb == NULL || handle == 0 || p_ccc_bits == NULL ||
        p_gattc_cb->state != GATT_CLIENT_STATE_DONE)
    {
        goto error;
    }
    *p_ccc_bits = gattc_get_cccd_data(p_gattc_cb, handle);
    return true;
error:
    BTM_PRINT_ERROR2("gatt_client_get_char_cccd: failed, conn_handle 0x%x, handle 0x%x",
                     conn_handle, handle);
    return false;
}

bool gatt_client_check_cccd_enabled(uint16_t conn_handle, T_ATTR_UUID *p_srv_uuid,
                                    T_ATTR_UUID *p_char_uuid)
{
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_check_link(conn_handle);
    T_ATTR_SRV_CB *p_srv_cb;

    if (p_gattc_cb == NULL || p_srv_uuid == 0 ||
        p_gattc_cb->state != GATT_CLIENT_STATE_DONE)
    {
        goto error;
    }
    p_srv_cb = gattc_storage_find_srv_by_uuid(p_gattc_cb, p_srv_uuid);
    if (p_srv_cb == NULL)
    {
        goto error;
    }

    if (p_char_uuid == NULL || p_char_uuid->is_uuid16 == true)
    {
        for (uint8_t j = 0; j < p_srv_cb->char16_num; j++)
        {
            T_ATTR_CHAR16_CB *p_attr_char = &p_srv_cb->p_char16_table[j];
            if (p_char_uuid != NULL && p_char_uuid->p.uuid16 != p_attr_char->char_data.uuid16)
            {
                continue;
            }
            if (p_attr_char->cccd_descriptor.hdr.att_handle)
            {
                if (gattc_get_cccd_data(p_gattc_cb, p_attr_char->cccd_descriptor.hdr.att_handle) == 0)
                {
                    goto disabled;
                }
            }
        }
    }

    if (p_char_uuid == NULL || p_char_uuid->is_uuid16 == false)
    {
        for (uint8_t j = 0; j < p_srv_cb->char128_num; j++)
        {
            T_ATTR_CHAR128_CB *p_attr_char = &p_srv_cb->p_char128_table[j];
            if (p_char_uuid != NULL && memcmp(p_char_uuid->p.uuid128, p_attr_char->char_data.uuid128, 16) != 0)
            {
                continue;
            }
            if (p_attr_char->cccd_descriptor.hdr.att_handle)
            {
                if (gattc_get_cccd_data(p_gattc_cb, p_attr_char->cccd_descriptor.hdr.att_handle) == 0)
                {
                    goto disabled;
                }
            }
        }
    }
    BTM_PRINT_INFO2("gatt_client_check_cccd_enabled: all enabled, conn_handle 0x%x, uuid16 0x%x",
                    conn_handle, p_srv_uuid->p.uuid16);
    return true;
disabled:
    BTM_PRINT_INFO2("gatt_client_check_cccd_enabled: not all enabled, conn_handle 0x%x, uuid16 0x%x",
                    conn_handle, p_srv_uuid->p.uuid16);
    return false;
error:
    BTM_PRINT_ERROR1("gatt_client_check_cccd_enabled: failed, conn_handle 0x%x",
                     conn_handle);
    return false;
}

T_GAP_CAUSE gatt_client_read(uint16_t conn_handle, uint16_t handle, P_FUN_GATT_CLIENT_CB p_req_cb)
{
    T_GATTC_REQ *p_read_req;
    T_GAP_CAUSE cause = GAP_CAUSE_SUCCESS;
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_check_link(conn_handle);

    if (p_gattc_cb == NULL || p_gattc_cb->state != GATT_CLIENT_STATE_DONE)
    {
        cause = GAP_CAUSE_INVALID_STATE;
        goto result;
    }

    if (gattc_storage_check_prop(p_gattc_cb, handle, GATT_CHAR_PROP_READ) == false)
    {
        goto result;
    }
    p_read_req = calloc(1, sizeof(T_GATTC_REQ));
    if (p_read_req == NULL)
    {
        cause = GAP_CAUSE_NO_RESOURCE;
        goto result;
    }
    p_read_req->req_type = GATTC_REQ_TYPE_READ;
    p_read_req->handle = handle;
    p_read_req->req_cb = p_req_cb;
    BTM_PRINT_INFO2("[CLIENT_REQ] read req: conn_handle 0x%x, handle 0x%x", conn_handle, handle);
    os_queue_in(&p_gattc_cb->gattc_req_list, p_read_req);
    gatt_client_check_req(p_gattc_cb);

result:
    if (cause != GAP_CAUSE_SUCCESS)
    {
        BTM_PRINT_ERROR3("[CLIENT_REQ] gatt_client_read: failed, conn_handle 0x%x, handle 0x%x, cause 0x%x",
                         conn_handle, handle, cause);
    }
    return cause;
}

T_GAP_CAUSE gatt_client_read_uuid(uint16_t conn_handle, uint16_t start_handle,
                                  uint16_t end_handle, uint16_t uuid16, P_FUN_GATT_CLIENT_CB p_req_cb)
{
    T_GATTC_REQ *p_read_req;
    T_GAP_CAUSE cause = GAP_CAUSE_SUCCESS;
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_check_link(conn_handle);

    if (p_gattc_cb == NULL)
    {
        cause = GAP_CAUSE_INVALID_STATE;
        goto result;
    }
    p_read_req = calloc(1, sizeof(T_GATTC_REQ));
    if (p_read_req == NULL)
    {
        cause = GAP_CAUSE_NO_RESOURCE;
        goto result;
    }
    p_read_req->req_type = GATTC_REQ_TYPE_READ_UUID;
    p_read_req->handle = start_handle;
    p_read_req->p.read_uuid.start_handle = start_handle;
    p_read_req->p.read_uuid.end_handle = end_handle;
    p_read_req->p.read_uuid.uuid16 = uuid16;
    p_read_req->req_cb = p_req_cb;
    BTM_PRINT_INFO2("[CLIENT_REQ] read uuid req: conn_handle 0x%x, start_handle 0x%x", conn_handle,
                    start_handle);
    os_queue_in(&p_gattc_cb->gattc_req_list, p_read_req);
    gatt_client_check_req(p_gattc_cb);

result:
    if (cause != GAP_CAUSE_SUCCESS)
    {
        BTM_PRINT_ERROR3("[CLIENT_REQ] gatt_client_read_uuid: failed, conn_handle 0x%x, uuid16 0x%x, cause 0x%x",
                         conn_handle, uuid16, cause);
    }
    return cause;
}

//FIX TODO if the write type is write cmd, for now p_req_cb shall be null
T_GAP_CAUSE gatt_client_write(uint16_t conn_handle, T_GATT_WRITE_TYPE write_type,
                              uint16_t handle, uint16_t length, uint8_t *p_data, P_FUN_GATT_CLIENT_CB p_req_cb)
{
    T_GATTC_REQ *p_write_req;
    T_GAP_CAUSE cause = GAP_CAUSE_SUCCESS;
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_check_link(conn_handle);

    if (p_gattc_cb == NULL || p_gattc_cb->state != GATT_CLIENT_STATE_DONE)
    {
        cause = GAP_CAUSE_INVALID_STATE;
        goto result;
    }
    if (write_type == GATT_WRITE_TYPE_REQ)
    {
        if (gattc_storage_check_prop(p_gattc_cb, handle, GATT_CHAR_PROP_WRITE) == false)
        {
            goto result;
        }
    }
    else if (write_type == GATT_WRITE_TYPE_CMD)
    {
        if (gattc_storage_check_prop(p_gattc_cb, handle, GATT_CHAR_PROP_WRITE_NO_RSP) == false)
        {
            goto result;
        }
    }
    else
    {
        goto result;
    }
    p_write_req = calloc(1, sizeof(T_GATTC_REQ));
    if (p_write_req == NULL)
    {
        cause = GAP_CAUSE_NO_RESOURCE;
        goto result;
    }
    p_write_req->req_type = GATTC_REQ_TYPE_WRITE;
    p_write_req->p.write.write_type = write_type;
    p_write_req->handle = handle;
    p_write_req->req_cb = p_req_cb;
    if (length != 0)
    {
        p_write_req->p.write.p_data = calloc(1, length);
        if (p_write_req->p.write.p_data == NULL)
        {
            free(p_write_req);
            goto result;
        }
        memcpy(p_write_req->p.write.p_data, p_data, length);
        p_write_req->p.write.length = length;
    }
    if (write_type == GATT_WRITE_TYPE_REQ)
    {
        BTM_PRINT_INFO3("[CLIENT_REQ] write req: conn_handle 0x%x, handle 0x%x, length %d", conn_handle,
                        handle, length);
        os_queue_in(&p_gattc_cb->gattc_req_list, p_write_req);
        gatt_client_check_req(p_gattc_cb);
    }
    else
    {
        os_queue_in(&p_gattc_cb->gattc_write_cmd_list, p_write_req);
        gatt_client_check_write_cmd(conn_handle, p_gattc_cb);
    }
result:
    if (cause != GAP_CAUSE_SUCCESS)
    {
        BTM_PRINT_ERROR3("[CLIENT_REQ] gatt_client_write: failed, conn_handle 0x%x, handle 0x%x, cause 0x%x",
                         conn_handle, handle, cause);
    }
    return cause;
}

static bool gatt_client_dis_next_secondary_service(uint16_t conn_handle,
                                                   T_GATTC_STORAGE_CB *p_gattc_cb,
                                                   bool *p_cmpl)
{
    T_GAP_CAUSE  cause;
    T_ATTR_DIS_SRV *p_srv_dis_cb = NULL;

    *p_cmpl = false;
    if (p_gattc_cb->p_curr_srv == NULL)
    {
        goto error;
    }
    else
    {
        p_srv_dis_cb = p_gattc_cb->p_curr_srv->p_next;
    }

    if (p_srv_dis_cb)
    {
        if (p_srv_dis_cb->srv_data.hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID16 ||
            p_srv_dis_cb->srv_data.hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID128)
        {
#if F_BT_GATT_CLIENT_EXT_API
            cause = client_ext_all_char_discovery(p_gattc_cb->conn_handle, 0, gattc_storage_cl_id,
                                                  p_srv_dis_cb->srv_data.srv_uuid16.hdr.att_handle,
                                                  p_srv_dis_cb->srv_data.srv_uuid16.end_group_handle);
#else
            cause = client_all_char_discovery(p_gattc_cb->conn_id, gattc_storage_cl_id,
                                              p_srv_dis_cb->srv_data.srv_uuid16.hdr.att_handle,
                                              p_srv_dis_cb->srv_data.srv_uuid16.end_group_handle);
#endif
            if (cause == GAP_CAUSE_SUCCESS)
            {
                p_gattc_cb->p_curr_srv = p_srv_dis_cb;
                return true;
            }
            else
            {
                goto error;
            }
        }
        else
        {
            goto error;
        }
    }
    p_gattc_cb->p_curr_srv = NULL;
    *p_cmpl = true;
    return true;
error:
    return false;
}

T_GAP_CAUSE gatt_client_ind_confirm(uint16_t conn_handle, uint16_t cid)
{
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_STATE;

#if F_BT_GATT_CLIENT_EXT_API
    cause = client_ext_attr_ind_confirm(conn_handle, cid);
#else
    uint8_t conn_id;

    if (le_get_conn_id_by_handle(conn_handle, &conn_id))
    {
        cause = client_attr_ind_confirm(conn_id);
    }
#endif
    return cause;
}

static bool gatt_client_dis_next_service(uint16_t conn_handle, T_GATTC_STORAGE_CB *p_gattc_cb,
                                         bool *p_cmpl)
{
    T_GAP_CAUSE  cause;
    T_ATTR_DIS_SRV *p_srv_dis_cb = NULL;
    *p_cmpl = false;

    if (p_gattc_cb == NULL)
    {
        return false;
    }

    if (p_gattc_cb->p_curr_srv == NULL)
    {
        p_srv_dis_cb = (T_ATTR_DIS_SRV *)p_gattc_cb->gattc_dis_list.p_first;
    }
    else
    {
        p_srv_dis_cb = p_gattc_cb->p_curr_srv->p_next;
    }

    if (p_srv_dis_cb)
    {
#if F_BT_GATT_CLIENT_EXT_API
        cause = client_ext_relationship_discovery(p_gattc_cb->conn_handle, 0, gattc_storage_cl_id,
                                                  p_srv_dis_cb->srv_data.srv_uuid16.hdr.att_handle,
                                                  p_srv_dis_cb->srv_data.srv_uuid16.end_group_handle);
#else
        cause = client_relationship_discovery(p_gattc_cb->conn_id, gattc_storage_cl_id,
                                              p_srv_dis_cb->srv_data.srv_uuid16.hdr.att_handle,
                                              p_srv_dis_cb->srv_data.srv_uuid16.end_group_handle);
#endif
        if (cause == GAP_CAUSE_SUCCESS)
        {
            p_gattc_cb->p_curr_srv = p_srv_dis_cb;
            return true;
        }
    }
    else
    {
        p_gattc_cb->p_curr_srv = (T_ATTR_DIS_SRV *)p_gattc_cb->gattc_dis_list.p_last;
        if (gattc_storage_add_secondary_service(p_gattc_cb))
        {
            return gatt_client_dis_next_secondary_service(conn_handle, p_gattc_cb, p_cmpl);
        }
        else
        {
            p_gattc_cb->p_curr_srv = NULL;
            *p_cmpl = true;
            return true;
        }
    }

    return false;
}

static bool gatt_client_dis_next_desc(uint16_t conn_handle, T_GATTC_STORAGE_CB *p_gattc_cb,
                                      T_ATTR_DIS_SRV *p_srv_dis_cb, bool start, bool *p_cmpl)
{
    T_GAP_CAUSE  cause;
    T_ATTR_DIS_CHAR *p_char_cb = NULL;
    *p_cmpl = false;
    if (p_srv_dis_cb->char_queue.count == 0)
    {
        goto next_srv;
    }

    if (start)
    {
        p_srv_dis_cb->char_queue.flags = 0;
    }
    else
    {
        p_srv_dis_cb->char_queue.flags++;
    }

    for (uint8_t i = p_srv_dis_cb->char_queue.flags; i < p_srv_dis_cb->char_queue.count; i++)
    {
        uint16_t end_handle;
        uint16_t start_handle;
        p_char_cb = (T_ATTR_DIS_CHAR *)os_queue_peek(&p_srv_dis_cb->char_queue, i);
        if (p_char_cb == NULL)
        {
            goto failed;
        }
        start_handle = p_char_cb->char_data.char_uuid16.value_handle + 1;
        if (p_char_cb->p_next == NULL)
        {
            end_handle = p_srv_dis_cb->srv_data.srv_uuid16.end_group_handle;
        }
        else
        {
            T_ATTR_DIS_CHAR *p_next_char = p_char_cb->p_next;
            end_handle = p_next_char->char_data.char_uuid16.value_handle - 2;
        }

        if (p_char_cb->char_data.hdr.attr_type == ATTR_TYPE_CHAR_UUID16)
        {
            p_char_cb->char_data.char_uuid16.end_handle = end_handle;
        }
        else if (p_char_cb->char_data.hdr.attr_type == ATTR_TYPE_CHAR_UUID128)
        {
            p_char_cb->char_data.char_uuid128.end_handle = end_handle;
        }
        if (start_handle > end_handle)
        {
            if (p_char_cb->char_data.char_uuid16.properties & (GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_INDICATE))
            {
                BTM_PRINT_ERROR2("gatt_client_dis_next_desc: error handle range, start_handle 0x%x, end_handle %d",
                                 start_handle, end_handle);
                p_char_cb->char_data.char_uuid16.properties &= ~(GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_INDICATE);
                continue;
            }

        }
        else
        {

#if F_BT_GATT_CLIENT_EXT_API
            cause = client_ext_all_char_descriptor_discovery(p_gattc_cb->conn_handle, 0, gattc_storage_cl_id,
                                                             start_handle, end_handle);
#else
            cause = client_all_char_descriptor_discovery(p_gattc_cb->conn_id, gattc_storage_cl_id,
                                                         start_handle, end_handle);
#endif
            if (cause == GAP_CAUSE_SUCCESS)
            {
                p_srv_dis_cb->char_queue.flags = i;
                goto success;
            }
            else
            {
                goto failed;
            }
        }
    }
next_srv:
    if (p_srv_dis_cb->srv_data.hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID16 ||
        p_srv_dis_cb->srv_data.hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID128)
    {
        return gatt_client_dis_next_service(conn_handle, p_gattc_cb, p_cmpl);
    }
    else
    {
        return gatt_client_dis_next_secondary_service(conn_handle, p_gattc_cb, p_cmpl);
    }
success:
    return true;
failed:
    return false;
}

static void gatt_client_send_dis_result(uint16_t conn_handle, T_GATTC_STORAGE_CB  *p_gattc_cb,
                                        bool load_from_ftl,
                                        bool is_success)
{
    T_SPEC_GATTC_CB *p_spec_cb = NULL;

    BTM_PRINT_TRACE3("gatt_client_send_dis_result: conn_handle 0x%x, load_from_ftl %d, is_success %d",
                     conn_handle, load_from_ftl, is_success);
    for (uint8_t i = 0; i < spec_gattc_queue.count; i++)
    {
        p_spec_cb = (T_SPEC_GATTC_CB *)os_queue_peek(&spec_gattc_queue, i);
        if (p_spec_cb)
        {
            T_GATT_CLIENT_DIS_DONE dis_done = {false, 0};
            if (p_spec_cb->p_fun_cb != NULL && is_success)
            {
                if (p_spec_cb->srv_uuid.is_uuid16)
                {
                    dis_done.srv_instance_num = gattc_storage_get_srv_num(p_gattc_cb, true,
                                                                          p_spec_cb->srv_uuid.p.uuid16, NULL);
                }
                else
                {
                    dis_done.srv_instance_num = gattc_storage_get_srv_num(p_gattc_cb, false,
                                                                          0, p_spec_cb->srv_uuid.p.uuid128);
                }

                if (dis_done.srv_instance_num)
                {
                    dis_done.is_found = true;
                }
                dis_done.load_from_ftl = load_from_ftl;
                p_spec_cb->p_fun_cb(conn_handle, GATT_CLIENT_EVENT_DIS_DONE, (void *)&dis_done);
            }
        }
    }
    if (p_gattc_cb->p_general_cb)
    {
        T_GATT_CLIENT_DIS_ALL_DONE dis_all_done;
        dis_all_done.state = p_gattc_cb->state;
        dis_all_done.load_from_ftl = load_from_ftl;
        p_gattc_cb->p_general_cb(conn_handle, GATT_CLIENT_EVENT_DIS_ALL_STATE, (void *)&dis_all_done);
    }
}

static void gatt_client_discover_state_ext_cb(uint16_t conn_handle, uint16_t cid,
                                              T_DISCOVERY_STATE discovery_state)
{
    bool is_cmpl = false;
    uint8_t error_idx = 0;
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_find(conn_handle);

    if (p_gattc_cb == NULL)
    {
        return;
    }

    switch (discovery_state)
    {
    case DISC_STATE_SRV_DONE:
        {
            if (gatt_client_dis_next_service(p_gattc_cb->conn_handle, p_gattc_cb, &is_cmpl) == false)
            {
                error_idx = 1;
                goto failed;
            }
        }
        break;

    case DISC_STATE_RELATION_DONE:
        {
            T_GAP_CAUSE  cause;

            if (p_gattc_cb->p_curr_srv != NULL)
            {
#if F_BT_GATT_CLIENT_EXT_API
                cause = client_ext_all_char_discovery(p_gattc_cb->conn_handle, 0, gattc_storage_cl_id,
                                                      p_gattc_cb->p_curr_srv->srv_data.srv_uuid16.hdr.att_handle,
                                                      p_gattc_cb->p_curr_srv->srv_data.srv_uuid16.end_group_handle);
#else
                cause = client_all_char_discovery(p_gattc_cb->conn_id, gattc_storage_cl_id,
                                                  p_gattc_cb->p_curr_srv->srv_data.srv_uuid16.hdr.att_handle,
                                                  p_gattc_cb->p_curr_srv->srv_data.srv_uuid16.end_group_handle);
#endif
                if (cause != GAP_CAUSE_SUCCESS)
                {
                    error_idx = 2;
                    goto failed;
                }
            }
            else
            {
                error_idx = 3;
                goto failed;
            }
        }
        break;

    case DISC_STATE_CHAR_DONE:
        {
            if (p_gattc_cb->p_curr_srv != NULL)
            {
                if (gatt_client_dis_next_desc(p_gattc_cb->conn_handle, p_gattc_cb, p_gattc_cb->p_curr_srv, true,
                                              &is_cmpl) == false)
                {
                    error_idx = 4;
                    goto failed;
                }
            }
            else
            {
                error_idx = 5;
                goto failed;
            }
        }
        break;

    case DISC_STATE_CHAR_DESCRIPTOR_DONE:
        {
            if (p_gattc_cb->p_curr_srv != NULL)
            {
                if (gatt_client_dis_next_desc(p_gattc_cb->conn_handle, p_gattc_cb, p_gattc_cb->p_curr_srv, false,
                                              &is_cmpl) == false)
                {
                    error_idx = 6;
                    goto failed;
                }
            }
            else
            {
                error_idx = 7;
                goto failed;
            }
        }
        break;

    case DISC_STATE_FAILED:
        {
            error_idx = 8;
            goto failed;
        }

    default:
        break;
    }

    if (is_cmpl)
    {
        p_gattc_cb->state = GATT_CLIENT_STATE_DONE;
        BTM_PRINT_TRACE1("gatt_client_discover_state_cb: conn_handle 0x%x", p_gattc_cb->conn_handle);
        //gattc_storage_dis_print(p_gattc_cb);
        if (gattc_storage_srv_list_gen(p_gattc_cb) == false)
        {
            error_idx = 9;
            goto failed;
        }
#if LE_AUDIO_DEBUG
        gattc_storage_print(p_gattc_cb);
#endif
        gattc_storage_write(p_gattc_cb);
        p_gattc_cb->load_from_ftl = false;
        gatt_client_send_dis_result(p_gattc_cb->conn_handle, p_gattc_cb, false, true);
    }
    return;
failed:
    {
        p_gattc_cb->state = GATT_CLIENT_STATE_FAILED;
        BTM_PRINT_ERROR2("gatt_client_discover_state_cb: conn_handle 0x%x, error_idx %d",
                         p_gattc_cb->conn_handle, error_idx);
        gattc_storage_dis_list_release(p_gattc_cb);
        gatt_client_send_dis_result(p_gattc_cb->conn_handle, p_gattc_cb, false, false);
    }
    return;
}

static void gatt_client_discover_result_ext_cb(uint16_t conn_handle, uint16_t cid,
                                               T_DISCOVERY_RESULT_TYPE result_type,
                                               T_DISCOVERY_RESULT_DATA result_data)
{
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_find(conn_handle);

    if (p_gattc_cb == NULL)
    {
        return;
    }

    switch (result_type)
    {
    case DISC_RESULT_ALL_SRV_UUID16:
        {
            gattc_storage_add_primary_service(p_gattc_cb, true, &result_data);
        }
        break;

    case DISC_RESULT_ALL_SRV_UUID128:
        {
            gattc_storage_add_primary_service(p_gattc_cb, false, &result_data);
        }
        break;

    case DISC_RESULT_RELATION_UUID16:
        {
            gattc_storage_add_include(p_gattc_cb->p_curr_srv, true, &result_data);
        }
        break;

    case DISC_RESULT_RELATION_UUID128:
        {
            gattc_storage_add_include(p_gattc_cb->p_curr_srv, false, &result_data);
        }
        break;

    case DISC_RESULT_CHAR_UUID16:
        {
            gattc_storage_add_char(p_gattc_cb->p_curr_srv, true, &result_data);
        }
        break;

    case DISC_RESULT_CHAR_UUID128:
        {
            gattc_storage_add_char(p_gattc_cb->p_curr_srv, false, &result_data);
        }
        break;

    case DISC_RESULT_CHAR_DESC_UUID128:
    case DISC_RESULT_CHAR_DESC_UUID16:
        {
            if (p_gattc_cb->p_curr_srv != NULL)
            {
                T_ATTR_DIS_CHAR *p_char_disc = (T_ATTR_DIS_CHAR *)os_queue_peek(&p_gattc_cb->p_curr_srv->char_queue,
                                                                                p_gattc_cb->p_curr_srv->char_queue.flags);
                if (p_char_disc == NULL)
                {
                    return;
                }
                if (result_type == DISC_RESULT_CHAR_DESC_UUID16)
                {
                    if (result_data.p_char_desc_uuid16_disc_data->uuid16 == GATT_UUID_CHAR_CLIENT_CONFIG)
                    {
                        p_char_disc->cccd_descriptor.hdr.attr_type = ATTR_TYPE_CCCD_DESC;
                        p_char_disc->cccd_descriptor.hdr.att_handle = result_data.p_char_desc_uuid16_disc_data->handle;
                    }

                    if (p_char_disc->desc_end_handle < result_data.p_char_desc_uuid16_disc_data->handle)
                    {
                        p_char_disc->desc_end_handle = result_data.p_char_desc_uuid16_disc_data->handle;
                    }

                }
                else
                {
                    if (p_char_disc->desc_end_handle < result_data.p_char_desc_uuid128_disc_data->handle)
                    {
                        p_char_disc->desc_end_handle = result_data.p_char_desc_uuid128_disc_data->handle;
                    }
                }
            }
        }
        break;

    default:
        break;
    }

    return;
}

static void gatt_client_read_result_ext_cb(uint16_t conn_handle, uint16_t cid, uint16_t cause,
                                           uint16_t handle,
                                           uint16_t value_size, uint8_t *p_value)
{
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_find(conn_handle);
    T_GATTC_REQ *p_req;

    if (p_gattc_cb == NULL)
    {
        return;
    }
    BTM_PRINT_INFO3("[CLIENT_REQ] gatt_client_read_result_cb: conn_handle 0x%x, cause 0x%x, handle 0x%x",
                    conn_handle, cause, handle);
    p_req = gatt_client_find_wait_rsp(p_gattc_cb, handle, cid, true, cause);
    if (p_req == NULL)
    {
        return;
    }

    if (p_req->req_type == GATTC_REQ_TYPE_READ_UUID)
    {
        gatt_client_handle_read_uuid_result(p_gattc_cb, cause,
                                            handle, value_size, p_value, p_req);
    }
    else
    {
        gatt_client_handle_read_result(p_gattc_cb, cause,
                                       handle, value_size, p_value, p_req);
    }
    gatt_client_del_wait_rsp(p_gattc_cb, p_req);
    return;
}

static void gatt_client_write_result_ext_cb(uint16_t conn_handle, uint16_t cid,
                                            T_GATT_WRITE_TYPE type, uint16_t handle,
                                            uint16_t cause, uint8_t credits)
{
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_find(conn_handle);
    T_GATTC_REQ *p_req = NULL;

    if (p_gattc_cb == NULL)
    {
        return;
    }
    BTM_PRINT_INFO4("[CLIENT_REQ] gatt_client_write_result_cb: conn_handle 0x%x, cause 0x%x, handle 0x%x, type %d",
                    p_gattc_cb->conn_handle, cause, handle, type);
    if (type == GATT_WRITE_TYPE_REQ)
    {
        p_req = gatt_client_find_wait_rsp(p_gattc_cb, handle, cid, false, cause);
        if (p_req == NULL)
        {
            return;
        }
    }

    if (p_req && p_req->req_type == GATTC_REQ_TYPE_CCCD)
    {
        if ((gattc_dis_mode & GATT_CLIENT_DISCOV_MODE_CCCD_STORAGE_BIT) != 0)
        {
            if (cause == GAP_SUCCESS)
            {
                gattc_set_cccd_data(p_gattc_cb, p_req->handle, p_req->p.cccd_cfg.cfg_value);
            }
        }
        if (p_req->p.cccd_cfg.cfg_end)
        {
            gatt_client_send_cccd_cfg_result(p_gattc_cb, cause, p_req);
        }
    }
    else
    {
        gatt_client_handle_write_result(p_gattc_cb, type, handle, cause, p_req);
        if (type == GATT_WRITE_TYPE_CMD)
        {
            gatt_client_check_write_cmd(conn_handle, p_gattc_cb);
        }
    }
    gatt_client_del_wait_rsp(p_gattc_cb, p_req);
}

static T_APP_RESULT gatt_client_notify_ind_ext_cb(uint16_t conn_handle, uint16_t cid,
                                                  bool notify, uint16_t handle,
                                                  uint16_t value_size, uint8_t *p_value)
{
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_GATT_CHAR_TYPE char_type;
    T_ATTR_DATA char_data;
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_find(conn_handle);

    if (p_gattc_cb == NULL)
    {
        return APP_RESULT_SUCCESS;
    }

    T_ATTR_SRV_CB *p_srv_cb = gattc_storage_find_srv_by_handle(p_gattc_cb, handle);

    if (p_srv_cb)
    {
        if (gattc_storage_find_char_desc(p_srv_cb, handle, &char_type, &char_data) == false)
        {
            return APP_RESULT_SUCCESS;
        }
    }
    else
    {
        return APP_RESULT_SUCCESS;
    }

    if (p_srv_cb->p_spec_cb != NULL && p_srv_cb->p_spec_cb->p_fun_cb != NULL)
    {
        T_GATT_CLIENT_NOTIFY_IND notify_ind;
        notify_ind.notify = notify;
        notify_ind.handle = handle;
        notify_ind.value_size = value_size;
        notify_ind.p_value = p_value;
        notify_ind.cid = cid;
        notify_ind.srv_instance_id = p_srv_cb->p_srv_data->hdr.instance_id;
        att_data_covert_to_uuid(&char_data, &notify_ind.char_uuid);
        result = p_srv_cb->p_spec_cb->p_fun_cb(p_gattc_cb->conn_handle, GATT_CLIENT_EVENT_NOTIFY_IND,
                                               (void *)&notify_ind);
    }
    return result;
}

#if BT_GATT_CLIENT_USE_NORMAL_API
static void gatt_client_discover_state_cb(uint8_t conn_id,  T_DISCOVERY_STATE discovery_state)
{
    uint16_t conn_handle = le_get_conn_handle(conn_id);

    gatt_client_discover_state_ext_cb(conn_handle, L2C_FIXED_CID_ATT, discovery_state);
}

static void gatt_client_discover_result_cb(uint8_t conn_id,
                                           T_DISCOVERY_RESULT_TYPE result_type,
                                           T_DISCOVERY_RESULT_DATA result_data)
{
    uint16_t conn_handle = le_get_conn_handle(conn_id);

    gatt_client_discover_result_ext_cb(conn_handle, L2C_FIXED_CID_ATT,
                                       result_type, result_data);
}

static void gatt_client_read_result_cb(uint8_t conn_id,  uint16_t cause,
                                       uint16_t handle, uint16_t value_size, uint8_t *p_value)
{
    uint16_t conn_handle = le_get_conn_handle(conn_id);

    gatt_client_read_result_ext_cb(conn_handle, L2C_FIXED_CID_ATT,
                                   cause, handle, value_size, p_value);
}

static void gatt_client_write_result_cb(uint8_t conn_id, T_GATT_WRITE_TYPE type,
                                        uint16_t handle, uint16_t cause,
                                        uint8_t credits)
{
    uint16_t conn_handle = le_get_conn_handle(conn_id);

    gatt_client_write_result_ext_cb(conn_handle, L2C_FIXED_CID_ATT,
                                    type, handle, cause, credits);
}

static T_APP_RESULT gatt_client_notify_ind_cb(uint8_t conn_id, bool notify,
                                              uint16_t handle,
                                              uint16_t value_size, uint8_t *p_value)
{
    uint16_t conn_handle = le_get_conn_handle(conn_id);

    return gatt_client_notify_ind_ext_cb(conn_handle, L2C_FIXED_CID_ATT,
                                         notify, handle, value_size, p_value);
}

static const T_FUN_CLIENT_CBS gatt_client_cbs =
{
    gatt_client_discover_state_cb,   //!< Discovery State callback function pointer
    gatt_client_discover_result_cb,  //!< Discovery result callback function pointer
    gatt_client_read_result_cb,     //!< Read response callback function pointer
    gatt_client_write_result_cb,     //!< Write result callback function pointer
    gatt_client_notify_ind_cb,     //!< Notify Indicate callback function pointer
    NULL      //!< Link disconnection callback function pointer
};
#endif
#if F_BT_GATT_CLIENT_EXT_API
static const T_FUN_EXT_CLIENT_CBS gatt_client_ext_cbs =
{
    gatt_client_discover_state_ext_cb,   //!< Discovery State callback function pointer
    gatt_client_discover_result_ext_cb,  //!< Discovery result callback function pointer
    gatt_client_read_result_ext_cb,     //!< Read response callback function pointer
    gatt_client_write_result_ext_cb,     //!< Write result callback function pointer
    gatt_client_notify_ind_ext_cb,     //!< Notify Indicate callback function pointer
};
#endif

void gatt_client_handle_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state)
{
    T_GATTC_STORAGE_CB *p_gatt_client;

    if (gattc_storage_cl_id == CLIENT_PROFILE_GENERAL_ID)
    {
        return;
    }
    p_gatt_client = gatt_client_find_by_conn_id(conn_id);
    BTM_PRINT_TRACE2("gatt_client_handle_conn_state_evt: conn_id %d, new_state %d",
                     conn_id, new_state);
    switch (new_state)
    {
    case GAP_CONN_STATE_DISCONNECTING:
        if (p_gatt_client != NULL)
        {
            p_gatt_client->conn_state = GAP_CONN_STATE_DISCONNECTING;
        }
        break;

    case GAP_CONN_STATE_CONNECTED:
        if (p_gatt_client != NULL)
        {
            p_gatt_client->conn_state = GAP_CONN_STATE_CONNECTED;
            p_gatt_client->conn_handle = le_get_conn_handle(conn_id);
            gatt_client_channel_allocate(p_gatt_client, L2C_FIXED_CID_ATT);
        }
        break;

    case GAP_CONN_STATE_DISCONNECTED:
        gatt_client_release_all(p_gatt_client);
        break;

    case GAP_CONN_STATE_CONNECTING:
        if (p_gatt_client == NULL)
        {
            p_gatt_client = gatt_client_allocate(conn_id);
            if (p_gatt_client != NULL)
            {
                p_gatt_client->conn_state = GAP_CONN_STATE_CONNECTING;
            }
        }
        break;

    default:
        break;
    }
}

void gatt_client_handle_mtu_info(uint8_t conn_id, uint16_t mtu_size)
{
    T_GATTC_STORAGE_CB  *p_gatt_client = gatt_client_find_by_conn_id(conn_id);
    if (p_gatt_client)
    {
        T_GATTC_CHANNEL  *p_chann = gatt_client_channel_find_by_cid(p_gatt_client, L2C_FIXED_CID_ATT);
        if (p_chann)
        {
            p_chann->mtu_size = mtu_size;
            gatt_client_check_req(p_gatt_client);
        }
    }
}

static void gatt_client_handle_chann_add(T_GAP_CHANN_ADDED *p_gap_chann_added)
{
    T_GATTC_STORAGE_CB *p_gatt_client = gatt_client_find(p_gap_chann_added->conn_handle);
#if BT_GATT_CLIENT_BREDR_SUPPORT
    if (p_gatt_client == NULL &&
        (p_gap_chann_added->chann_type == GAP_CHANN_TYPE_BREDR_ATT ||
         p_gap_chann_added->chann_type == GAP_CHANN_TYPE_BREDR_ECFC))
    {
        p_gatt_client = gatt_client_allocate_for_bredr(p_gap_chann_added->conn_handle);
    }
#endif

    if (p_gatt_client)
    {
        T_GATTC_CHANNEL  *p_chann = gatt_client_channel_allocate(p_gatt_client, p_gap_chann_added->cid);
        if (p_chann)
        {
            p_chann->mtu_size = p_gap_chann_added->mtu_size;
            gatt_client_check_req(p_gatt_client);
        }
    }
}

static void gatt_client_handle_chann_del(T_GAP_CHANN_DEL *p_gap_chann_del)
{
    T_GATTC_STORAGE_CB *p_gatt_client = gatt_client_find(p_gap_chann_del->conn_handle);
    if (p_gatt_client)
    {
        T_GATTC_CHANNEL  *p_chann = gatt_client_channel_find_by_cid(p_gatt_client, p_gap_chann_del->cid);
        if (p_chann)
        {
            if (p_chann->is_req)
            {
                BTM_PRINT_INFO2("[CLIENT_REQ] gatt_client_handle_chann_del: pending write rsp, conn_handle 0x%x, cid 0x%x",
                                p_gap_chann_del->conn_handle,
                                p_chann->cid);
                for (uint8_t i = 0; i < p_gatt_client->gattc_wait_rsp_list.count; i++)
                {
                    T_GATTC_REQ *p_req = (T_GATTC_REQ *)os_queue_peek(&p_gatt_client->gattc_wait_rsp_list, i);

                    if (p_req->cid == p_chann->cid)
                    {
                        BTM_PRINT_INFO2("[CLIENT_REQ] gatt_client_handle_chann_del: pending write rsp, req_type 0x%x, handle 0x%x",
                                        p_req->req_type,
                                        p_req->handle);

                        if (os_queue_delete(&p_gatt_client->gattc_wait_rsp_list, p_req))
                        {
                            gatt_client_handle_req_result(p_gatt_client, p_req, GAP_ERR | GAP_ERR_CONN_DISCONNECT);
                            free(p_req);
                        }
                    }
                }
            }
            gatt_client_channel_del(p_gatt_client, p_chann);
#if BT_GATT_CLIENT_BREDR_SUPPORT
            if (p_gatt_client->conn_type == T_GATTC_CONN_TYPE_BREDR)
            {
                if (p_gatt_client->channel_list.count == 0)
                {
                    gatt_client_release_all(p_gatt_client);
                }
            }
#endif
        }
    }
}

static void gatt_client_handle_chann_mtu_update(T_GAP_CHANN_MTU_UPDATE *p_gap_chann_mtu_update)
{
    T_GATTC_STORAGE_CB *p_gatt_client = gatt_client_find(p_gap_chann_mtu_update->conn_handle);
    if (p_gatt_client)
    {
        T_GATTC_CHANNEL  *p_chann = gatt_client_channel_find_by_cid(p_gatt_client,
                                                                    p_gap_chann_mtu_update->cid);
        if (p_chann)
        {
            p_chann->mtu_size = p_gap_chann_mtu_update->mtu_size;
            gatt_client_check_req(p_gatt_client);
        }
    }
}

void gatt_client_handle_gap_common_cb(uint8_t cb_type, void *p_cb_data)
{
    T_GAP_CB_DATA cb_data;
    memcpy(&cb_data, p_cb_data, sizeof(T_GAP_CB_DATA));
    switch (cb_type)
    {
    case GAP_MSG_GAP_CHANN_ADDED:
        {
            BTM_PRINT_INFO4("GAP_MSG_GAP_CHANN_ADDED: conn_handle 0x%x, cid 0x%x, chann_type %d, mtu_size %d",
                            cb_data.p_gap_chann_added->conn_handle,
                            cb_data.p_gap_chann_added->cid,
                            cb_data.p_gap_chann_added->chann_type,
                            cb_data.p_gap_chann_added->mtu_size);
            gatt_client_handle_chann_add(cb_data.p_gap_chann_added);
        }
        break;

    case GAP_MSG_GAP_CHANN_DEL:
        {
            BTM_PRINT_INFO2("GAP_MSG_GAP_CHANN_DEL: conn_handle 0x%x, cid 0x%x",
                            cb_data.p_gap_chann_del->conn_handle,
                            cb_data.p_gap_chann_del->cid);
            gatt_client_handle_chann_del(cb_data.p_gap_chann_del);

        }
        break;

    case GAP_MSG_GAP_CHANN_MTU_UPDATE:
        {
            BTM_PRINT_INFO3("GAP_MSG_GAP_CHANN_MTU_UPDATE: conn_handle 0x%x, cid 0x%x, mtu_size %d",
                            cb_data.p_gap_chann_mtu_update->conn_handle,
                            cb_data.p_gap_chann_mtu_update->cid,
                            cb_data.p_gap_chann_mtu_update->mtu_size);
            gatt_client_handle_chann_mtu_update(cb_data.p_gap_chann_mtu_update);
        }
        break;

    default:
        break;
    }
}

bool gatt_client_init(uint16_t mode)
{
    if (gattc_storage_cl_id == CLIENT_PROFILE_GENERAL_ID && mode != GATT_CLIENT_DISCOV_MODE_CLOSED)
    {
        bool ret = false;

        if (mode & GATT_CLIENT_DISCOV_MODE_USE_EXT_CLIENT)
        {
#if F_BT_GATT_CLIENT_EXT_API
            client_cfg_use_ext_api(true);
            client_init(1);
            ret = client_ext_register_spec_client_cb(&gattc_storage_cl_id, &gatt_client_ext_cbs);
#endif
        }
        else
        {
#if BT_GATT_CLIENT_USE_NORMAL_API
            client_init(1);
            ret = client_register_spec_client_cb(&gattc_storage_cl_id, &gatt_client_cbs);
#endif
        }

        if (ret == false)
        {
            gattc_storage_cl_id = CLIENT_PROFILE_GENERAL_ID;
            BTM_PRINT_ERROR1("gatt_client_init: register fail, mode 0x%x", mode);
            return false;
        }
        os_queue_init(&bt_gattc_queue);
        gattc_dis_mode = mode;
        if (gattc_dis_mode & GATT_CLIENT_DISCOV_MODE_GATT_SVC)
        {
            gatt_service_add();
        }
#if BLE_MGR_INIT_DEBUG
        BTM_PRINT_INFO2("gatt_client_init: client id %d, mode %d", gattc_storage_cl_id, mode);
#endif
        return true;
    }
    return true;
}

bool gatt_client_storage_register(P_FUN_GATT_STORAGE_CB p_fun_cb)
{
    gatt_storage_cb = p_fun_cb;
    return true;
}

T_GAP_CAUSE gatt_client_spec_register(T_ATTR_UUID *p_srv_uuid, P_FUN_GATT_CLIENT_CB p_fun_cb)
{
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    T_SPEC_GATTC_CB *p_db;

    p_db = gatt_spec_client_find_by_uuid(p_srv_uuid);
    if (p_db)
    {
        cause = GAP_CAUSE_ALREADY_IN_REQ;
        goto error;
    }
    else
    {
        p_db = calloc(1, sizeof(T_SPEC_GATTC_CB));

        if (p_db == NULL)
        {
            cause = GAP_CAUSE_NO_RESOURCE;
            goto error;
        }
        memcpy(&p_db->srv_uuid, p_srv_uuid, sizeof(T_ATTR_UUID));
        p_db->p_fun_cb = p_fun_cb;
        os_queue_in(&spec_gattc_queue, p_db);
    }
#if BLE_MGR_INIT_DEBUG
    BTM_PRINT_TRACE1("gatt_client_spec_register: uuid 0x%x", p_srv_uuid->p.uuid16);
#endif
    return GAP_CAUSE_SUCCESS;
error:
    BTM_PRINT_ERROR1("gatt_client_spec_register: failed, cause %d", cause);
    return cause;
}

bool gatt_client_find_char_descriptor_range(uint16_t conn_handle, T_ATTR_UUID *p_srv_uuid,
                                            T_ATTR_UUID *p_char_uuid, uint16_t *p_start_handle,
                                            uint16_t *p_end_handle, uint16_t *p_cccd_handle)
{
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_check_link(conn_handle);
    T_ATTR_SRV_CB *p_srv_cb;
    T_ATTR_CHAR16_CB *p_char16 = NULL;
    T_ATTR_CHAR128_CB *p_char128 = NULL;

    if (p_gattc_cb == NULL || p_start_handle == NULL || p_end_handle == NULL ||
        p_cccd_handle == NULL || p_gattc_cb->state != GATT_CLIENT_STATE_DONE)
    {
        goto error;
    }
    *p_start_handle = 0;
    *p_end_handle = 0;
    *p_cccd_handle = 0;
    p_srv_cb = gattc_storage_find_srv_by_uuid(p_gattc_cb, p_srv_uuid);
    if (p_srv_cb == NULL)
    {
        goto error;
    }

    if (gattc_storage_find_char(p_srv_cb, p_char_uuid, &p_char16, &p_char128))
    {
        if (p_char16)
        {
            *p_start_handle = p_char16->char_data.value_handle + 1;
            *p_end_handle = p_char16->char_data.end_handle;
            *p_cccd_handle = p_char16->cccd_descriptor.hdr.att_handle;
        }

        if (p_char128)
        {
            *p_start_handle = p_char128->char_data.value_handle + 1;
            *p_end_handle = p_char128->char_data.end_handle;
            *p_cccd_handle = p_char128->cccd_descriptor.hdr.att_handle;
        }

        if (*p_start_handle > *p_end_handle)
        {
            goto error;
        }
        return true;
    }
error:
    BTM_PRINT_ERROR3("gatt_client_find_char_descriptor_range: failed, conn_handle 0x%x, srv_uuid 0x%x, char_uuid 0x%x",
                     conn_handle,
                     p_srv_uuid->p.uuid16, p_char_uuid->p.uuid16);
    return false;
}

bool gatt_client_find_char_cccd_handle(uint16_t conn_handle, T_ATTR_UUID *p_srv_uuid,
                                       T_ATTR_UUID *p_char_uuid, uint16_t *p_handle, uint8_t *p_cccd_prop)
{
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_check_link(conn_handle);
    T_ATTR_SRV_CB *p_srv_cb;
    T_ATTR_CHAR16_CB *p_char16 = NULL;
    T_ATTR_CHAR128_CB *p_char128 = NULL;

    if (p_gattc_cb == NULL || p_handle == NULL || p_cccd_prop == NULL ||
        p_gattc_cb->state != GATT_CLIENT_STATE_DONE)
    {
        goto error;
    }
    p_srv_cb = gattc_storage_find_srv_by_uuid(p_gattc_cb, p_srv_uuid);
    if (p_srv_cb == NULL)
    {
        goto error;
    }

    if (gattc_storage_find_char(p_srv_cb, p_char_uuid, &p_char16, &p_char128))
    {
        uint16_t properties;
        uint16_t cccd_handle = 0;
        uint16_t cfg_value = 0;

        if (p_char16)
        {
            properties = p_char16->char_data.properties;
            cccd_handle = p_char16->cccd_descriptor.hdr.att_handle;
        }

        if (p_char128)
        {
            properties = p_char128->char_data.properties;
            cccd_handle = p_char128->cccd_descriptor.hdr.att_handle;
        }

        if (cccd_handle)
        {
            if (gatt_client_check_cccd_prop(properties, GATT_CLIENT_CONFIG_ALL, &cfg_value))
            {
                *p_handle = cccd_handle;
                *p_cccd_prop = cfg_value;
                return true;
            }
        }
    }
error:
    BTM_PRINT_ERROR1("gatt_client_find_char_cccd_handle: failed, conn_handle 0x%x",
                     conn_handle);
    return false;
}

bool gatt_client_find_char_handle(uint16_t conn_handle, T_ATTR_UUID *p_srv_uuid,
                                  T_ATTR_UUID *p_char_uuid, uint16_t *p_handle)
{
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_check_link(conn_handle);
    T_ATTR_SRV_CB *p_srv_cb;
    T_ATTR_DATA attr_data;

    if (p_gattc_cb == NULL || p_gattc_cb->state != GATT_CLIENT_STATE_DONE)
    {
        goto error;
    }
    p_srv_cb = gattc_storage_find_srv_by_uuid(p_gattc_cb, p_srv_uuid);
    if (p_srv_cb == NULL)
    {
        goto error;
    }

    if (gattc_storage_find_char_by_uuid(p_srv_cb, p_char_uuid, &attr_data))
    {
        *p_handle = attr_data.char_uuid16.value_handle;
        return true;
    }
error:
    BTM_PRINT_ERROR3("gatt_client_find_char_handle: failed, conn_handle 0x%x, srv_uuid 0x%x, char_uuid 0x%x",
                     conn_handle,
                     p_srv_uuid->p.uuid16, p_char_uuid->p.uuid16);
    return false;
}

bool gatt_client_find_primary_srv_by_include(uint16_t conn_handle, T_ATTR_UUID *p_included_srv,
                                             T_ATTR_UUID *p_primary_srv)
{
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_check_link(conn_handle);
    T_ATTR_SRV_CB *p_srv_cb;
    T_ATTR_SRV_CB *p_inc_srv_cb;

    if (p_gattc_cb == NULL || p_primary_srv == NULL || p_gattc_cb->state != GATT_CLIENT_STATE_DONE)
    {
        goto error;
    }
    p_srv_cb = gattc_storage_find_srv_by_uuid(p_gattc_cb, p_included_srv);
    if (p_srv_cb == NULL)
    {
        goto error;
    }
    p_inc_srv_cb = gattc_storage_find_inc_srv_by_uuid(p_gattc_cb, p_srv_cb);
    if (p_inc_srv_cb == NULL)
    {
        goto error;
    }
    att_data_covert_to_uuid(p_inc_srv_cb->p_srv_data, p_primary_srv);
    return true;
error:
    return false;
}

bool gatt_client_find_include_srv_by_primary(uint16_t conn_handle, T_ATTR_UUID *p_primary_srv,
                                             T_ATTR_UUID *p_included_srv,
                                             T_ATTR_INSTANCE *p_attr_instance)
{
    uint8_t instance_num = 0;
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_check_link(conn_handle);
    T_ATTR_SRV_CB *p_srv_cb;

    if (p_gattc_cb == NULL || p_primary_srv == NULL || p_included_srv == NULL ||
        p_attr_instance == NULL || p_gattc_cb->state != GATT_CLIENT_STATE_DONE)
    {
        goto error;
    }
    p_attr_instance->instance_num = 0;
    p_srv_cb = gattc_storage_find_srv_by_uuid(p_gattc_cb, p_primary_srv);
    if (p_srv_cb == NULL)
    {
        goto error;
    }

    if (p_included_srv->is_uuid16)
    {
        if (p_srv_cb->inc16_srv_num)
        {
            for (uint8_t j = 0; j < p_srv_cb->inc16_srv_num; j++)
            {
                T_ATTR_INCLUDE_UUID16 *p_include_data = &p_srv_cb->p_inc16_table[j];
                if (instance_num == ATTR_INSTANCE_NUM_MAX)
                {
                    BTM_PRINT_ERROR1("gatt_client_find_include_srv_by_primary: instance num is full %d",
                                     instance_num);
                    break;
                }

                if (p_include_data->uuid16 == p_included_srv->p.uuid16)
                {
                    for (uint8_t i = 0; i < p_gattc_cb->srv_list.count; i++)
                    {
                        T_ATTR_SRV_CB *p_srv_cb = (T_ATTR_SRV_CB *)os_queue_peek(&p_gattc_cb->srv_list, i);
                        if (p_srv_cb)
                        {
                            if (p_srv_cb->p_srv_data->hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID16 ||
                                p_srv_cb->p_srv_data->hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID16)
                            {
                                if (p_include_data->uuid16 == p_srv_cb->p_srv_data->srv_uuid16.uuid16 &&
                                    p_include_data->start_handle == p_srv_cb->p_srv_data->srv_uuid16.hdr.att_handle &&
                                    p_include_data->end_handle == p_srv_cb->p_srv_data->srv_uuid16.end_group_handle)
                                {
                                    p_attr_instance->instance_id[instance_num] = p_srv_cb->p_srv_data->srv_uuid16.hdr.instance_id;
                                    instance_num++;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        if (p_srv_cb->inc128_srv_num)
        {
            for (uint8_t j = 0; j < p_srv_cb->inc128_srv_num; j++)
            {
                T_ATTR_INCLUDE_UUID128 *p_include_data = &p_srv_cb->p_inc128_table[j];
                if (instance_num == ATTR_INSTANCE_NUM_MAX)
                {
                    BTM_PRINT_ERROR1("gatt_client_find_include_srv_by_primary: instance num is full %d",
                                     instance_num);
                    break;
                }

                if (memcmp(p_srv_cb->p_inc128_table[j].uuid128, p_included_srv->p.uuid128, 16) == 0)
                {
                    for (uint8_t i = 0; i < p_gattc_cb->srv_list.count; i++)
                    {
                        T_ATTR_SRV_CB *p_srv_cb = (T_ATTR_SRV_CB *)os_queue_peek(&p_gattc_cb->srv_list, i);
                        if (p_srv_cb)
                        {
                            if (p_srv_cb->p_srv_data->hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID128 ||
                                p_srv_cb->p_srv_data->hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID128)
                            {
                                if (p_include_data->start_handle == p_srv_cb->p_srv_data->srv_uuid128.hdr.att_handle &&
                                    p_include_data->end_handle == p_srv_cb->p_srv_data->srv_uuid128.end_group_handle &&
                                    memcmp(p_include_data->uuid128, p_srv_cb->p_srv_data->srv_uuid128.uuid128, 16) == 0)
                                {
                                    p_attr_instance->instance_id[instance_num] = p_srv_cb->p_srv_data->srv_uuid128.hdr.instance_id;
                                    instance_num++;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (instance_num == 0)
    {
        goto error;
    }
    p_attr_instance->instance_num = instance_num;
    return true;
error:
    return false;
}

uint8_t gatt_client_get_char_num(uint16_t conn_handle, T_ATTR_UUID *p_srv_uuid,
                                 T_ATTR_UUID *p_char_uuid)
{
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_check_link(conn_handle);
    T_ATTR_SRV_CB *p_srv_cb;
    uint8_t instance_id = 0;
    if (p_gattc_cb == NULL || p_char_uuid == NULL || p_gattc_cb->state != GATT_CLIENT_STATE_DONE)
    {
        goto error;
    }
    p_srv_cb = gattc_storage_find_srv_by_uuid(p_gattc_cb, p_srv_uuid);
    if (p_srv_cb == NULL)
    {
        goto error;
    }

    if (p_srv_cb)
    {
        if (p_char_uuid->is_uuid16)
        {
            if (p_srv_cb->char16_num)
            {
                for (uint8_t j = 0; j < p_srv_cb->char16_num; j++)
                {
                    if (p_char_uuid->p.uuid16 == p_srv_cb->p_char16_table[j].char_data.uuid16)
                    {
                        instance_id++;
                    }
                }
            }
        }
        else
        {
            if (p_srv_cb->char128_num)
            {
                for (uint8_t j = 0; j < p_srv_cb->char128_num; j++)
                {
                    if (memcmp(p_char_uuid->p.uuid128, p_srv_cb->p_char128_table[j].char_data.uuid128, 16) == 0)
                    {
                        instance_id++;
                    }
                }
            }
        }
    }
    return instance_id;
error:
    return 0;
}

bool gatt_client_get_char_prop(uint16_t conn_handle, T_ATTR_UUID *p_srv_uuid,
                               T_ATTR_UUID *p_char_uuid, uint16_t *p_properties)
{
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_check_link(conn_handle);
    T_ATTR_SRV_CB *p_srv_cb;
    T_ATTR_DATA attr_data;

    if (p_gattc_cb == NULL || p_gattc_cb->state != GATT_CLIENT_STATE_DONE)
    {
        goto error;
    }
    p_srv_cb = gattc_storage_find_srv_by_uuid(p_gattc_cb, p_srv_uuid);
    if (p_srv_cb == NULL)
    {
        goto error;
    }

    if (gattc_storage_find_char_by_uuid(p_srv_cb, p_char_uuid, &attr_data))
    {
        *p_properties = attr_data.char_uuid16.properties;
        return true;
    }
error:
    return false;
}

bool gatt_client_is_load_from_ftl(uint16_t conn_handle)
{
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_check_link(conn_handle);

    if (p_gattc_cb != NULL)
    {
        return p_gattc_cb->load_from_ftl;
    }
    return false;
}

#if BLE_MGR_DEINIT
extern uint8_t client_supported_feature;
void gatt_client_deinit(void)
{
    T_GATTC_STORAGE_CB *p_gatt_client;

    gattc_dis_mode = GATT_CLIENT_DISCOV_MODE_CLOSED;
    gatt_storage_cb = NULL;
    gattc_storage_cl_id = CLIENT_PROFILE_GENERAL_ID;
    client_supported_feature = 0;

    while ((p_gatt_client = os_queue_out(&bt_gattc_queue)) != NULL)
    {
        gattc_req_list_free(p_gatt_client);
        gattc_storage_dis_list_release(p_gatt_client);
        gattc_storage_srv_list_release(p_gatt_client);
        if (p_gatt_client->p_cccd_timer_handle)
        {
            os_timer_delete(&p_gatt_client->p_cccd_timer_handle);
        }
        gatt_client_channel_release(p_gatt_client);
        free((void *) p_gatt_client);
    }
    T_SPEC_GATTC_CB *p_spec_cb;
    while ((p_spec_cb = os_queue_out(&spec_gattc_queue)) != NULL)
    {
        free(p_spec_cb);
    }
}
#endif
#endif

