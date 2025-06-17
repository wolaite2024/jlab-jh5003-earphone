#include <string.h>
#include "trace.h"
#include "bt_types.h"
#include "ble_audio_dm.h"
#include "ble_audio_mgr.h"
#include "tbs_server.h"
#include "ccp_def.h"
#include "ccp_mgr.h"
#include "ccp_mgr_int.h"

#if LE_AUDIO_CCP_SERVER_SUPPORT
T_TBS_CB *p_tbs_mem = NULL;
uint8_t total_tbs_num = 0;

static bool set_tbs_num_init(uint8_t tbs_num)
{
    if (tbs_num == 0 || p_tbs_mem != NULL)
    {
        PROTOCOL_PRINT_ERROR0("[CCP]set_tbs_num_init: invalid param");
        return false;
    }
    p_tbs_mem = ble_audio_mem_zalloc(tbs_num * sizeof(T_TBS_CB));
    if (p_tbs_mem == NULL)
    {
        PROTOCOL_PRINT_ERROR0("[CCP]set_tbs_num_init: allocate mem failed");
        return false;
    }
    total_tbs_num = tbs_num;
    return true;
}

bool ccp_send_all_notify(T_TBS_CB *p_entry, uint16_t uuid, uint8_t *p_data, uint16_t data_len)
{
    uint8_t i;
    uint16_t attrib_idx;

    T_CHAR_UUID char_uuid;
    char_uuid.index = 1;
    char_uuid.uuid_size = UUID_16BIT_SIZE;
    char_uuid.uu.char_uuid16 = uuid;

    if (!gatt_svc_find_char_index_by_uuid(p_entry->p_attr_tbl, char_uuid,
                                          p_entry->attr_num,
                                          &attrib_idx))
    {
        PROTOCOL_PRINT_ERROR1("[CCP]ccp_send_all_notify: find index failed, uuid 0x%x", uuid);
        return false;
    }

    for (i = 0; i < ble_audio_db.acl_link_num; i++)
    {
        if ((ble_audio_db.le_link[i].used == true) &&
            (ble_audio_db.le_link[i].state == GAP_CONN_STATE_CONNECTED))
        {
            tbs_send_notify(p_entry, ble_audio_db.le_link[i].conn_handle, attrib_idx, p_data, data_len);
        }
    }
    return true;
}


static bool ccp_send_notify(T_TBS_CB *p_entry, uint16_t conn_handle,
                            uint16_t uuid, uint8_t *p_data, uint16_t data_len)
{
    T_CHAR_UUID char_uuid;
    uint16_t attrib_idx = 0;
    char_uuid.index = 1;
    char_uuid.uuid_size = UUID_16BIT_SIZE;
    char_uuid.uu.char_uuid16 = uuid;

    if (!gatt_svc_find_char_index_by_uuid(p_entry->p_attr_tbl, char_uuid,
                                          p_entry->attr_num,
                                          &attrib_idx))
    {
        return false;
    }

    if (tbs_send_notify(p_entry, conn_handle, attrib_idx, p_data, data_len))
    {
        return true;
    }
    return false;
}

static void ccp_update_list_cur_calls(T_TBS_CB *p_entry)
{
    int i;
    T_CALL_STATE *p_call_state;
    uint16_t  call_state_list_size = tbs_calculate_call_list_size(p_entry);
    uint8_t notify_data[call_state_list_size];
    uint16_t idx = 0;
    for (i = 0; i < p_entry->call_state_queue.count; i++)
    {
        p_call_state = (T_CALL_STATE *)os_queue_peek(&p_entry->call_state_queue, i);
        notify_data[idx++] = 3 + p_call_state->uri_len;
        notify_data[idx++] = p_call_state->call_idx;
        notify_data[idx++] = p_call_state->call_state;
        notify_data[idx++] = p_call_state->call_flags;
        memcpy(&notify_data[idx], p_call_state->call_uri, p_call_state->uri_len);
        idx += p_call_state->uri_len;
    }

    ccp_send_all_notify(p_entry, TBS_UUID_CHAR_BEARER_LIST_CURRENT_CALLS, notify_data,
                        call_state_list_size);

}


static void ccp_update_call_state(T_TBS_CB *p_entry)
{
    int i;
    T_CALL_STATE *p_call_state;
    uint8_t notify_data[3 * p_entry->call_state_queue.count];
    uint16_t idx = 0;
    for (i = 0; i < p_entry->call_state_queue.count; i++)
    {
        p_call_state = (T_CALL_STATE *)os_queue_peek(&p_entry->call_state_queue, i);
        notify_data[idx++] = p_call_state->call_idx;
        notify_data[idx++] = p_call_state->call_state;
        notify_data[idx++] = p_call_state->call_flags;
    }

    ccp_send_all_notify(p_entry, TBS_UUID_CHAR_CALL_STATE, notify_data, sizeof(notify_data));

    p_call_state = tbs_get_call_state(p_entry, p_entry->incoming_call_idx);

    if (p_call_state)
    {
        if (p_call_state->need_notify_incoming_call)
        {
            uint8_t incoming_call_notify_data[1 + p_call_state->uri_len];

            incoming_call_notify_data[0] = p_call_state->call_idx;
            memcpy(&incoming_call_notify_data[1], p_call_state->call_uri, p_call_state->uri_len);

            if (ccp_send_all_notify(p_entry, TBS_UUID_CHAR_INCOMING_CALL, incoming_call_notify_data,
                                    1 + p_call_state->uri_len))
            {
                p_call_state->need_notify_incoming_call = false;
            }
        }
    }
}

static void tbs_rmv_calls(T_TBS_CB *p_entry, T_CALL_STATE *p_call_state)
{
    uint8_t call_idx = 0;

    if ((p_entry == NULL) || (p_call_state == NULL))
    {
        return ;
    }

    call_idx = p_call_state->call_idx;

    os_queue_delete(&p_entry->call_state_queue, p_call_state);

    ccp_update_call_state(p_entry);
    ccp_update_list_cur_calls(p_entry);

    if (!(p_call_state->call_flags & TBS_CALL_FLAGS_BIT_INCOMING_OUTGOING))
    {
        //FIX TODO should notify client these charac changed
        if (call_idx == p_entry->in_call_tg_uri_cb.call_index)
        {
            if (p_entry->in_call_tg_uri_cb.p_incoming_call_target_uri)
            {
                ble_audio_mem_free(p_entry->in_call_tg_uri_cb.p_incoming_call_target_uri);
            }
            p_entry->in_call_tg_uri_cb.call_index = 0;
            p_entry->in_call_tg_uri_cb.p_incoming_call_target_uri = NULL;
            p_entry->in_call_tg_uri_cb.incoming_call_target_uri_len = 0;
        }
        if (call_idx == p_entry->incoming_call_idx)
        {
            p_entry->incoming_call_idx = 0;
        }
    }
    if (call_idx == p_entry->call_fri_name_cb.call_index)
    {
        if (p_entry->call_fri_name_cb.p_friendly_name)
        {
            ble_audio_mem_free(p_entry->call_fri_name_cb.p_friendly_name);
        }
        p_entry->call_fri_name_cb.call_index = 0;
        p_entry->call_fri_name_cb.p_friendly_name = NULL;
        p_entry->call_fri_name_cb.friendly_name_len = 0;
    }
    if (p_call_state->call_uri)
    {
        ble_audio_mem_free(p_call_state->call_uri);
    }
    ble_audio_mem_free(p_call_state);
}

static T_TBS_CB *tbs_allocate_entry(T_CCP_SERVER_REG_SRV_PARAM *p_param)
{
    uint8_t i;
    if ((p_tbs_mem == NULL) || (p_param == NULL))
    {
        return NULL;
    }
    for (i = 0; i < total_tbs_num; i++)
    {
        if (p_tbs_mem[i].used == false)
        {
            memset(&p_tbs_mem[i], 0, sizeof(T_TBS_CB));
            p_tbs_mem[i].gtbs = p_param->gtbs;
            p_tbs_mem[i].used = true;
            os_queue_init(&p_tbs_mem[i].call_state_queue);
            return &p_tbs_mem[i];
        }
    }
    return NULL;
}

T_TBS_CB *tbs_find_by_srv_id(T_SERVER_ID service_id)
{
    uint8_t  i;

    if (p_tbs_mem == NULL)
    {
        return NULL;
    }

    for (i = 0; i < total_tbs_num; i++)
    {
        if (p_tbs_mem[i].used
            && (p_tbs_mem[i].service_id == service_id))
        {
            return (&p_tbs_mem[i]);
        }
    }

    PROTOCOL_PRINT_ERROR1("[CCP]tbs_find_by_srv_id: failed, service_id 0x%x", service_id);

    return NULL;
}

void ccp_write_post_proc(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                         uint16_t attrib_index, uint16_t length,
                         uint8_t *p_value)
{
    uint8_t *pp = p_value;
    T_TBS_CB *p_entry;
    uint16_t cb_result;

    if (p_value == NULL)
    {
        return;
    }

    p_entry = tbs_find_by_srv_id(service_id);
    if (p_entry == NULL)
    {
        return;
    }

    uint8_t opcode = *pp++;
    T_CCP_SERVER_WRITE_CALL_CP_IND  write_call_cp;
    write_call_cp.conn_handle = conn_handle;
    write_call_cp.cid = cid;
    write_call_cp.service_id = service_id;
    write_call_cp.opcode = opcode;
    T_CALL_STATE *p_call_state = NULL;
    uint8_t res[3] = {0};
    uint8_t call_index = *pp;

    res[0] = opcode;
    res[2] = TBS_CALL_CONTROL_POINT_NOTIFICATION_RESULT_CODES_SUCCESS;

    if (opcode < TBS_CALL_CONTROL_POINT_CHAR_OPCODE_ORIGINATE)
    {
        p_call_state = tbs_get_call_state(p_entry, *pp);
        if (p_call_state == NULL)
        {
            res[2] = TBS_CALL_CONTROL_POINT_NOTIFICATION_RESULT_CODES_INVALID_CALL_INDEX;
            goto done;
        }
    }
    switch (opcode)
    {
    case TBS_CALL_CONTROL_POINT_CHAR_OPCODE_ACCEPT:
        {
            if (p_call_state->call_state != TBS_CALL_STATE_INCOMING)
            {
                res[2] = TBS_CALL_CONTROL_POINT_NOTIFICATION_RESULT_CODES_STATE_MISMATCH;
                break;
            }

            write_call_cp.param.accept_opcode_call_index = *pp;
        }
        break;

    case TBS_CALL_CONTROL_POINT_CHAR_OPCODE_TERMINATE:
        {
            write_call_cp.param.terminate_opcode_call_index = *pp;
        }
        break;

    case TBS_CALL_CONTROL_POINT_CHAR_OPCODE_LOCAL_HOLD:
        {
            if ((p_entry->call_control_point_optional_opcodes & (1 <<
                                                                 TBS_CALL_CONTROL_POINT_OPTIONAL_OPCODES_CHAR_BIT_LOCAL_HOLD))
                == 0)
            {
                res[2] = TBS_CALL_CONTROL_POINT_NOTIFICATION_RESULT_CODES_OPCODE_NOT_SUPPORTED;
                break;
            }
            else if (p_call_state->call_state != TBS_CALL_STATE_INCOMING &&
                     p_call_state->call_state != TBS_CALL_STATE_ACTIVE &&
                     p_call_state->call_state != TBS_CALL_STATE_REMOTELY_HELD)
            {
                res[2] = TBS_CALL_CONTROL_POINT_NOTIFICATION_RESULT_CODES_STATE_MISMATCH;
                break;
            }
            write_call_cp.param.local_hold_opcode_call_index = *pp;
        }
        break;

    case TBS_CALL_CONTROL_POINT_CHAR_OPCODE_LOCAL_RETRIEVE:
        {
            if ((p_entry->call_control_point_optional_opcodes & (1 <<
                                                                 TBS_CALL_CONTROL_POINT_OPTIONAL_OPCODES_CHAR_BIT_LOCAL_HOLD))
                == 0)
            {
                res[2] = TBS_CALL_CONTROL_POINT_NOTIFICATION_RESULT_CODES_OPCODE_NOT_SUPPORTED;
                break;
            }
            else if (p_call_state->call_state != TBS_CALL_STATE_LOCALLY_HELD &&
                     p_call_state->call_state != TBS_CALL_STATE_LOCALLY_AND_REMOTELY_HELD)
            {
                res[2] = TBS_CALL_CONTROL_POINT_NOTIFICATION_RESULT_CODES_STATE_MISMATCH;
                break;
            }
            write_call_cp.param.local_retrieve_opcode_call_index = *pp;
        }
        break;

    case TBS_CALL_CONTROL_POINT_CHAR_OPCODE_ORIGINATE:
        {
            write_call_cp.param.originate_opcode_param.p_uri = pp;
            write_call_cp.param.originate_opcode_param.uri_len = length - 1;
        }
        break;

    case TBS_CALL_CONTROL_POINT_CHAR_OPCODE_JOIN:
        {
            if ((p_entry->call_control_point_optional_opcodes & (1 <<
                                                                 TBS_CALL_CONTROL_POINT_OPTIONAL_OPCODES_CHAR_BIT_JOIN)) == 0)
            {
                res[2] = TBS_CALL_CONTROL_POINT_NOTIFICATION_RESULT_CODES_OPCODE_NOT_SUPPORTED;
                break;
            }

            if ((p_entry->call_state_queue.count < 2) || (length < 3))
            {
                res[2] = TBS_CALL_CONTROL_POINT_NOTIFICATION_RESULT_CODES_OPERATION_NOT_POSSIBLE;
                break;
            }

            int i;
            for (i = 0; i < (length - 1); i++)
            {
                p_call_state = tbs_get_call_state(p_entry, pp[i]);
                if (p_call_state == NULL)
                {
                    res[2] = TBS_CALL_CONTROL_POINT_NOTIFICATION_RESULT_CODES_INVALID_CALL_INDEX;
                    goto done;
                }
                if (p_call_state->call_state == TBS_CALL_STATE_INCOMING)
                {
                    res[2] = TBS_CALL_CONTROL_POINT_NOTIFICATION_RESULT_CODES_OPERATION_NOT_POSSIBLE;
                    goto done;
                }
            }
            write_call_cp.param.join_opcode_param.p_list_of_call_indexes = pp;
            write_call_cp.param.join_opcode_param.list_of_call_indexes_len = length - 1;
        }
        break;

    default:
        {
            res[2] = TBS_CALL_CONTROL_POINT_NOTIFICATION_RESULT_CODES_OPCODE_NOT_SUPPORTED;
        }
        break;
    }

done:
    if (res[2] != TBS_CALL_CONTROL_POINT_NOTIFICATION_RESULT_CODES_SUCCESS)
    {
        ccp_send_notify(p_entry, conn_handle, TBS_UUID_CHAR_CALL_CONTROL_POINT, res, 3);
        return;
    }

    //If the opcode is originate or join, the return result shall be APP_RESULT_PENDING.
    //The handle result shall be return in ccp_handle_op_result
    //the retur result for other opcode can be APP_RESULT_SUCCESS or APP_RESULT_PENDING
    cb_result = ble_audio_mgr_dispatch(LE_AUDIO_MSG_CCP_SERVER_WRITE_CALL_CP_IND, &write_call_cp);

    if (cb_result == BLE_AUDIO_CB_RESULT_SUCCESS)
    {
        if (opcode == TBS_CALL_CONTROL_POINT_CHAR_OPCODE_ORIGINATE ||
            opcode == TBS_CALL_CONTROL_POINT_CHAR_OPCODE_JOIN)
        {
            PROTOCOL_PRINT_ERROR0("[CCP]ccp_write_post_proc: opcode with improper app result!");
            return;
        }

        res[1] = call_index;

        ccp_send_notify(p_entry, conn_handle, TBS_UUID_CHAR_CALL_CONTROL_POINT, res, 3);
    }
}

void ccp_handle_op_result(T_CCP_OP_RES res)
{
    T_TBS_CB *p_entry;
    uint8_t   notify_data[3];

    p_entry = tbs_find_by_srv_id(res.service_id);
    if (p_entry == NULL)
    {
        return;
    }

    notify_data[0] = res.opcode;
    notify_data[1] = res.call_idx;
    notify_data[2] = res.result;
    ccp_send_notify(p_entry, res.conn_handle, TBS_UUID_CHAR_CALL_CONTROL_POINT, notify_data, 3);
}

uint8_t ccp_server_create_call(uint8_t service_id, uint8_t *p_call_uri, uint16_t call_uri_len)
{
    T_CALL_STATE  *p_call_state;
    T_TBS_CB *p_entry = tbs_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        return 0;
    }
    if (p_entry->cur_call_idx == 0)
    {
        p_entry->cur_call_idx++;
    }

    //The roll over index may still be used
    while ((p_call_state = tbs_get_call_state(p_entry, p_entry->cur_call_idx)) != NULL)
    {
        p_entry->cur_call_idx++;
        if (p_entry->cur_call_idx == 0)
        {
            p_entry->cur_call_idx++;
        }
    }

    p_call_state = ble_audio_mem_zalloc(sizeof(T_CALL_STATE));

    if (p_call_state == NULL)
    {
        PROTOCOL_PRINT_ERROR1("[CCP]ccp_server_create_call, allocp_call_state fail : %d",
                              sizeof(T_CALL_STATE));
        return 0;
    }
    if (call_uri_len > 0 && p_call_uri)
    {
        p_call_state->call_uri = ble_audio_mem_zalloc(call_uri_len);
        if (p_call_state->call_uri == NULL)
        {
            PROTOCOL_PRINT_ERROR1("[CCP]ccp_server_create_call, alloc call uri fail : %d", call_uri_len);
        }
        else
        {
            memcpy(p_call_state->call_uri, p_call_uri, call_uri_len);
            p_call_state->uri_len = call_uri_len;
        }
    }
    p_call_state->call_idx = p_entry->cur_call_idx++;
    p_call_state->call_state = TBS_CALL_STATE_RFU;

    os_queue_in(&p_entry->call_state_queue, p_call_state);
    return p_call_state->call_idx;
}

bool ccp_server_set_param(uint8_t service_id, T_CCP_SERVER_SET_PARAM *p_param)
{
    T_TBS_CB *p_entry = tbs_find_by_srv_id(service_id);

    if (p_entry == NULL || p_param == NULL)
    {
        return false;
    }

    switch (p_param->char_uuid)
    {
    case TBS_UUID_CHAR_INCOMING_CALL_TARGET_BEARER_URI:
        {
            p_entry->in_call_tg_uri_cb.call_index = 0;

            if (p_entry->in_call_tg_uri_cb.p_incoming_call_target_uri)
            {
                ble_audio_mem_free(p_entry->in_call_tg_uri_cb.p_incoming_call_target_uri);
                p_entry->in_call_tg_uri_cb.p_incoming_call_target_uri = 0;
                p_entry->in_call_tg_uri_cb.incoming_call_target_uri_len = 0;
            }
            if (p_param->param.incoming_call_target_bearer_uri.incoming_call_target_uri_len > 0)
            {
                p_entry->in_call_tg_uri_cb.p_incoming_call_target_uri = ble_audio_mem_zalloc(
                                                                            p_param->param.incoming_call_target_bearer_uri.incoming_call_target_uri_len);
                if (p_entry->in_call_tg_uri_cb.p_incoming_call_target_uri == NULL)
                {
                    PROTOCOL_PRINT_ERROR1("[CCP]ccp_server_set_param alloc len: %d incoming target uri fail",
                                          p_param->param.incoming_call_target_bearer_uri.incoming_call_target_uri_len);

                    return false;
                }
                memcpy(p_entry->in_call_tg_uri_cb.p_incoming_call_target_uri,
                       p_param->param.incoming_call_target_bearer_uri.p_incoming_call_target_uri,
                       p_param->param.incoming_call_target_bearer_uri.incoming_call_target_uri_len);
            }
            p_entry->in_call_tg_uri_cb.incoming_call_target_uri_len =
                p_param->param.incoming_call_target_bearer_uri.incoming_call_target_uri_len;
            p_entry->in_call_tg_uri_cb.call_index = p_param->param.incoming_call_target_bearer_uri.call_index;

            uint8_t notify_data[1 + p_entry->in_call_tg_uri_cb.incoming_call_target_uri_len];
            notify_data[0] = p_entry->in_call_tg_uri_cb.call_index;
            memcpy(&notify_data[1], p_entry->in_call_tg_uri_cb.p_incoming_call_target_uri,
                   p_entry->in_call_tg_uri_cb.incoming_call_target_uri_len);

            ccp_send_all_notify(p_entry, TBS_UUID_CHAR_INCOMING_CALL_TARGET_BEARER_URI, notify_data,
                                sizeof(notify_data));
        }
        break;

    case TBS_UUID_CHAR_INCOMING_CALL:
        {
            T_CALL_STATE *p_call_state = tbs_get_call_state(p_entry, p_param->param.incoming_call.call_index);

            if ((p_call_state == NULL) || (p_call_state->call_flags & TBS_CALL_FLAGS_BIT_INCOMING_OUTGOING))
            {
                PROTOCOL_PRINT_ERROR1("[CCP]ccp_server_set_param can't find index: 0x%x call",
                                      p_param->param.incoming_call.call_index);
                return false;
            }

            p_entry->incoming_call_idx = 0;

            if (p_call_state->call_uri)
            {
                ble_audio_mem_free(p_call_state->call_uri);
                p_call_state->call_uri = NULL;
                p_call_state->uri_len = 0;
            }
            if (p_param->param.incoming_call.uri_len > 0)
            {
                p_call_state->call_uri = ble_audio_mem_zalloc(p_param->param.incoming_call.uri_len);
                if (p_call_state->call_uri == NULL)
                {
                    PROTOCOL_PRINT_ERROR1("[CCP]ccp_server_set_param alloc len: %d incoming call uri fail",
                                          p_param->param.incoming_call.uri_len);

                    return false;
                }
                memcpy(p_call_state->call_uri, p_param->param.incoming_call.p_uri,
                       p_param->param.incoming_call.uri_len);
            }
            p_call_state->uri_len = p_param->param.incoming_call.uri_len;
            p_entry->incoming_call_idx = p_param->param.incoming_call.call_index;
        }
        break;

    case TBS_UUID_CHAR_CALL_FRIENDLY_NAME:
        {
            p_entry->call_fri_name_cb.call_index = 0;

            if (p_entry->call_fri_name_cb.p_friendly_name)
            {
                ble_audio_mem_free(p_entry->call_fri_name_cb.p_friendly_name);
                p_entry->call_fri_name_cb.p_friendly_name = NULL;
                p_entry->call_fri_name_cb.friendly_name_len = 0;
            }

            if (p_param->param.call_friendly_name.friendly_name_len > 0)
            {
                p_entry->call_fri_name_cb.p_friendly_name = ble_audio_mem_zalloc(
                                                                p_param->param.call_friendly_name.friendly_name_len);
                if (p_entry->call_fri_name_cb.p_friendly_name == NULL)
                {
                    PROTOCOL_PRINT_ERROR1("[CCP]ccp_server_set_param alloc len: %d call friendly name fail",
                                          p_param->param.call_friendly_name.friendly_name_len);

                    return false;
                }
                memcpy(p_entry->call_fri_name_cb.p_friendly_name, p_param->param.call_friendly_name.p_friendly_name,
                       p_param->param.call_friendly_name.friendly_name_len);
            }
            p_entry->call_fri_name_cb.friendly_name_len = p_param->param.call_friendly_name.friendly_name_len;
            p_entry->call_fri_name_cb.call_index = p_param->param.call_friendly_name.call_index;

            uint8_t notify_data[1 + p_entry->call_fri_name_cb.friendly_name_len];
            notify_data[0] = p_entry->call_fri_name_cb.call_index;
            memcpy(&notify_data[1], p_entry->call_fri_name_cb.p_friendly_name,
                   p_entry->call_fri_name_cb.friendly_name_len);

            ccp_send_all_notify(p_entry, TBS_UUID_CHAR_CALL_FRIENDLY_NAME, notify_data, sizeof(notify_data));
        }
        break;

    case TBS_UUID_CHAR_CALL_CONTROL_POINT_OPTIONAL_OPCODES:
        {
            p_entry->call_control_point_optional_opcodes = p_param->param.call_control_point_optional_opcodes;
        }
        break;

    default:
        return false;
    }

    return true;
}

bool ccp_server_send_data(uint8_t service_id, T_CCP_SERVER_SEND_DATA_PARAM *p_param)
{
    T_TBS_CB *p_entry = tbs_find_by_srv_id(service_id);

    if (p_entry == NULL || p_param == NULL)
    {
        return false;
    }

    switch (p_param->char_uuid)
    {
    case TBS_UUID_CHAR_BEARER_PROVIDER_NAME:
        {
            return ccp_send_all_notify(p_entry, TBS_UUID_CHAR_BEARER_PROVIDER_NAME,
                                       p_param->param.bearer_provider_name.p_bearer_provider_name,
                                       p_param->param.bearer_provider_name.bearer_provider_name_len);
        }

    case TBS_UUID_CHAR_BEARER_TECHNOLOGY:
        {
            return ccp_send_all_notify(p_entry, TBS_UUID_CHAR_BEARER_TECHNOLOGY,
                                       p_param->param.bearer_technology.p_bearer_technology,
                                       p_param->param.bearer_technology.bearer_technology_len);
        }

    case TBS_UUID_CHAR_BEARER_URI_SCHEMES_SUPPORTED_LIST:
        {
            return ccp_send_all_notify(p_entry, TBS_UUID_CHAR_BEARER_URI_SCHEMES_SUPPORTED_LIST,
                                       p_param->param.bearer_uri_schemes_supported_list.p_bearer_uri_schemes_supported_list,
                                       p_param->param.bearer_uri_schemes_supported_list.bearer_uri_schemes_supported_list_len);
        }

    case TBS_UUID_CHAR_BEARER_SIGNAL_STRENGTH:
        {
            return ccp_send_all_notify(p_entry, TBS_UUID_CHAR_BEARER_SIGNAL_STRENGTH,
                                       &p_param->param.bearer_signal_strength, 1);
        }

    case TBS_UUID_CHAR_STATUS_FLAGS:
        {
            uint8_t data[2];

            LE_UINT16_TO_ARRAY(data, p_param->param.status_flags);

            return ccp_send_all_notify(p_entry, TBS_UUID_CHAR_STATUS_FLAGS, data, 2);
        }

    default:
        return false;
    }
}

bool ccp_server_update_call_state_by_call_index(uint8_t service_id, uint8_t call_index,
                                                uint8_t call_state,
                                                uint8_t call_flags, bool send_notification)
{
    T_TBS_CB *p_entry = tbs_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR1("[CCP]ccp_update_call_state can't find call state service_id: 0x%x",
                              service_id);
        return false;
    }

    T_CALL_STATE *p_call_state = tbs_get_call_state(p_entry, call_index);
    if (p_call_state == NULL)
    {
        PROTOCOL_PRINT_ERROR1("[CCP]ccp_update_call_state can't find index: 0x%x call", call_index);
        return false;
    }

    if ((p_call_state->call_state == TBS_CALL_STATE_RFU) && (call_state == TBS_CALL_STATE_INCOMING))
    {
        p_call_state->need_notify_incoming_call = true;
        p_entry->incoming_call_idx = call_index;
    }

    p_call_state->call_state = call_state;
    p_call_state->call_flags = call_flags;

    if (send_notification)
    {
        ccp_update_call_state(p_entry);
        ccp_update_list_cur_calls(p_entry);
    }

    return true;
}

void ccp_update_call_list_and_state(uint8_t service_id)
{
    T_TBS_CB *p_entry = tbs_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR1("[CCP]ccp_update_call_state can't find call state service_id: 0x%x",
                              service_id);
        return;
    }

    ccp_update_call_state(p_entry);
    ccp_update_list_cur_calls(p_entry);
}

bool ccp_server_terminate_call(uint8_t service_id,
                               T_CCP_SERVER_TERMINATION_REASON *p_termination_reason)
{
    T_TBS_CB *p_entry = tbs_find_by_srv_id(service_id);
    T_CALL_STATE *p_call_state = NULL;
    uint8_t data[2];

    if ((p_entry == NULL) || (p_termination_reason == NULL))
    {
        return false;
    }

    p_call_state = tbs_get_call_state(p_entry, p_termination_reason->call_index);

    if (p_call_state == NULL)
    {
        return false;
    }

    data[0] = p_termination_reason->call_index;
    data[1] = p_termination_reason->reason_code;

    ccp_send_all_notify(p_entry, TBS_UUID_CHAR_TERMINATION_REASON, data, 2);

    tbs_rmv_calls(p_entry, p_call_state);

    return true;
}

uint8_t ccp_server_reg_srv(T_CCP_SERVER_REG_SRV_PARAM *p_param)
{
    T_TBS_CB *p_entry;

    p_entry = tbs_allocate_entry(p_param);

    if (p_entry)
    {
        return tbs_add_service(p_entry, p_param);
    }
    return 0xFF;
}

bool ccp_server_init(uint8_t tbs_num)
{
    return set_tbs_num_init(tbs_num);
}

bool ccp_server_read_cfm(T_CCP_SERVER_READ_CFM *p_read_cfm)
{
    if (p_read_cfm)
    {
        T_TBS_CB *p_entry = tbs_find_by_srv_id(p_read_cfm->service_id);
        uint16_t attrib_idx;
        T_CHAR_UUID char_uuid;
        char_uuid.index = 1;
        char_uuid.uuid_size = UUID_16BIT_SIZE;
        char_uuid.uu.char_uuid16 = p_read_cfm->char_uuid;

        if (p_entry == NULL)
        {
            return false;
        }

        if (!gatt_svc_find_char_index_by_uuid(p_entry->p_attr_tbl, char_uuid,
                                              p_entry->attr_num,
                                              &attrib_idx))
        {
            return false;
        }

        switch (p_read_cfm->char_uuid)
        {
        case TBS_UUID_CHAR_BEARER_PROVIDER_NAME:
            {
                uint8_t data[p_read_cfm->param.bearer_provider_name.bearer_provider_name_len];

                memcpy(data, p_read_cfm->param.bearer_provider_name.p_bearer_provider_name,
                       p_read_cfm->param.bearer_provider_name.bearer_provider_name_len);

                if (p_read_cfm->offset > p_read_cfm->param.bearer_provider_name.bearer_provider_name_len)
                {
                    return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid, p_read_cfm->service_id,
                                                 attrib_idx, NULL, 0, 0, APP_RESULT_INVALID_OFFSET);
                }

                return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid,
                                             p_read_cfm->service_id,
                                             attrib_idx,
                                             data + p_read_cfm->offset, p_read_cfm->param.bearer_provider_name.bearer_provider_name_len -
                                             p_read_cfm->offset,
                                             p_read_cfm->param.bearer_provider_name.bearer_provider_name_len, (T_APP_RESULT)(p_read_cfm->cause));
            }

        case TBS_UUID_CHAR_BEARER_TECHNOLOGY:
            {
                uint8_t data[p_read_cfm->param.bearer_technology.bearer_technology_len];

                memcpy(data, p_read_cfm->param.bearer_technology.p_bearer_technology,
                       p_read_cfm->param.bearer_technology.bearer_technology_len);

                if (p_read_cfm->offset > p_read_cfm->param.bearer_technology.bearer_technology_len)
                {
                    return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid, p_read_cfm->service_id,
                                                 attrib_idx, NULL, 0, 0, APP_RESULT_INVALID_OFFSET);
                }

                return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid,
                                             p_read_cfm->service_id,
                                             attrib_idx,
                                             data + p_read_cfm->offset, p_read_cfm->param.bearer_technology.bearer_technology_len -
                                             p_read_cfm->offset,
                                             p_read_cfm->param.bearer_technology.bearer_technology_len, (T_APP_RESULT)(p_read_cfm->cause));
            }

        case TBS_UUID_CHAR_BEARER_URI_SCHEMES_SUPPORTED_LIST:
            {
                uint8_t data[p_read_cfm->param.bearer_uri_schemes_supported_list.bearer_uri_schemes_supported_list_len];

                memcpy(data,
                       p_read_cfm->param.bearer_uri_schemes_supported_list.p_bearer_uri_schemes_supported_list,
                       p_read_cfm->param.bearer_uri_schemes_supported_list.bearer_uri_schemes_supported_list_len);

                if (p_read_cfm->offset >
                    p_read_cfm->param.bearer_uri_schemes_supported_list.bearer_uri_schemes_supported_list_len)
                {
                    return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid, p_read_cfm->service_id,
                                                 attrib_idx, NULL, 0, 0, APP_RESULT_INVALID_OFFSET);
                }

                return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid,
                                             p_read_cfm->service_id,
                                             attrib_idx,
                                             data + p_read_cfm->offset,
                                             p_read_cfm->param.bearer_uri_schemes_supported_list.bearer_uri_schemes_supported_list_len -
                                             p_read_cfm->offset,
                                             p_read_cfm->param.bearer_uri_schemes_supported_list.bearer_uri_schemes_supported_list_len,
                                             (T_APP_RESULT)(p_read_cfm->cause));
            }

        case TBS_UUID_CHAR_BEARER_SIGNAL_STRENGTH:
            {
                uint8_t data[1];

                data[0] = p_read_cfm->param.bearer_signal_strength;

                if (p_read_cfm->offset > 1)
                {
                    return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid, p_read_cfm->service_id,
                                                 attrib_idx, NULL, 0, 0, APP_RESULT_INVALID_OFFSET);
                }

                return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid,
                                             p_read_cfm->service_id,
                                             attrib_idx,
                                             data + p_read_cfm->offset, 1 - p_read_cfm->offset, 1, (T_APP_RESULT)(p_read_cfm->cause));
            }

        case TBS_UUID_CHAR_STATUS_FLAGS:
            {
                uint8_t data[2];

                LE_UINT16_TO_ARRAY(data, p_read_cfm->param.status_flags);

                if (p_read_cfm->offset > 2)
                {
                    return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid, p_read_cfm->service_id,
                                                 attrib_idx, NULL, 0, 0, APP_RESULT_INVALID_OFFSET);
                }

                return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid,
                                             p_read_cfm->service_id,
                                             attrib_idx,
                                             data + p_read_cfm->offset, 2 - p_read_cfm->offset, 2, (T_APP_RESULT)(p_read_cfm->cause));
            }

        case TBS_UUID_CHAR_BEARER_UCI:
            {
                uint8_t data[p_read_cfm->param.bearer_uci.bearer_uci_len];

                memcpy(data, p_read_cfm->param.bearer_uci.p_bearer_uci,
                       p_read_cfm->param.bearer_uci.bearer_uci_len);

                if (p_read_cfm->offset > p_read_cfm->param.bearer_uci.bearer_uci_len)
                {
                    return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid, p_read_cfm->service_id,
                                                 attrib_idx, NULL, 0, 0, APP_RESULT_INVALID_OFFSET);
                }

                return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid,
                                             p_read_cfm->service_id,
                                             attrib_idx,
                                             data + p_read_cfm->offset, p_read_cfm->param.bearer_uci.bearer_uci_len - p_read_cfm->offset,
                                             p_read_cfm->param.bearer_uci.bearer_uci_len, (T_APP_RESULT)(p_read_cfm->cause));
            }

        case TBS_UUID_CHAR_BEARER_SIGNAL_STRENGTH_REPORTING_INTERVAL:
            {
                uint8_t data[1];

                data[0] = p_read_cfm->param.bearer_signal_strength_reporting_interval;

                if (p_read_cfm->offset > 1)
                {
                    return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid, p_read_cfm->service_id,
                                                 attrib_idx, NULL, 0, 0, APP_RESULT_INVALID_OFFSET);
                }

                return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid,
                                             p_read_cfm->service_id,
                                             attrib_idx,
                                             data + p_read_cfm->offset, 1 - p_read_cfm->offset, 1, (T_APP_RESULT)(p_read_cfm->cause));
            }

        case TBS_UUID_CHAR_CONTENT_CONTROL_ID:
            {
                uint8_t data[1];

                data[0] = p_read_cfm->param.content_control_id;

                if (p_read_cfm->offset > 1)
                {
                    return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid, p_read_cfm->service_id,
                                                 attrib_idx, NULL, 0, 0, APP_RESULT_INVALID_OFFSET);
                }

                return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid,
                                             p_read_cfm->service_id,
                                             attrib_idx,
                                             data + p_read_cfm->offset, 1 - p_read_cfm->offset, 1, (T_APP_RESULT)(p_read_cfm->cause));
            }

        default:
            break;
        }
    }

    return false;
}

#if LE_AUDIO_DEINIT
void ccp_server_deinit(void)
{
    if (p_tbs_mem)
    {
        T_TBS_CB *p_entry;
        T_CALL_STATE  *p_call_state;

        for (uint8_t i = 0; i < total_tbs_num; i++)
        {
            p_entry = &p_tbs_mem[i];
            while ((p_call_state = os_queue_out(&p_entry->call_state_queue)) != NULL)
            {
                if (p_call_state->call_uri)
                {
                    ble_audio_mem_free(p_call_state->call_uri);
                }
                ble_audio_mem_free(p_call_state);
            }

            if (p_entry->in_call_tg_uri_cb.p_incoming_call_target_uri)
            {
                ble_audio_mem_free(p_entry->in_call_tg_uri_cb.p_incoming_call_target_uri);
            }

            if (p_entry->call_fri_name_cb.p_friendly_name)
            {
                ble_audio_mem_free(p_entry->call_fri_name_cb.p_friendly_name);
            }

            if (p_entry->p_attr_tbl)
            {
                ble_audio_mem_free(p_entry->p_attr_tbl);
            }
        }
        ble_audio_mem_free(p_tbs_mem);
        p_tbs_mem = NULL;
    }
    total_tbs_num = 0;
}
#endif
#endif
