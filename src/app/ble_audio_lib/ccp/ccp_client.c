#include <string.h>
#include "trace.h"
#include "ble_audio_mgr.h"
#include "ble_audio_dm.h"
#include "ble_link_util.h"
#include "tbs_client.h"
#include "ccp_client_int.h"

#if LE_AUDIO_CCP_CLIENT_SUPPORT
typedef struct
{
    uint8_t             tbs_num;
    T_TBS_CLIENT_DB    *p_gtbs_cb;
    T_TBS_CLIENT_DB    *p_tbs_cb;
} T_CCP_CLIENT_DB;


static T_CCP_CLIENT_DB *ccp_check_link_int(uint16_t conn_handle)
{
    T_BLE_AUDIO_LINK *p_link = NULL;

    PROTOCOL_PRINT_TRACE1("ccp_check_link_int: conn_handle 0x%x", conn_handle);

    p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL || p_link->p_ccp_client == NULL)
    {
        PROTOCOL_PRINT_ERROR1("ccp_check_link_int: conn_handle 0x%x", conn_handle);
        return NULL;
    }
    return (T_CCP_CLIENT_DB *)p_link->p_ccp_client;
}

static T_APP_RESULT tbs_client_handle_callback(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                               void *p_data, bool gtbs)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;
    T_CCP_CLIENT_DB *p_ccp_db = ccp_check_link_int(conn_handle);

    if (p_ccp_db == NULL && type != GATT_CLIENT_EVENT_DIS_DONE)
    {
        return APP_RESULT_APP_ERR;
    }

    switch (type)
    {
    case GATT_CLIENT_EVENT_DIS_DONE:
        {
            uint8_t srv_num = 0;
            T_CCP_CLIENT_DIS_DONE dis_done;

            if (p_client_cb_data->dis_done.is_found)
            {
                T_BLE_AUDIO_LINK *p_link = NULL;
                srv_num = p_client_cb_data->dis_done.srv_instance_num;
                p_link = ble_audio_link_find_by_conn_handle(conn_handle);
                if (p_link == NULL)
                {
                    return APP_RESULT_APP_ERR;
                }

                if (p_link->p_ccp_client == NULL)
                {
                    p_link->p_ccp_client = ble_audio_mem_zalloc(sizeof(T_CCP_CLIENT_DB));
                    if (p_link->p_ccp_client == NULL)
                    {
                        return APP_RESULT_APP_ERR;
                    }
                }
                T_CCP_CLIENT_DB  *p_ccp_client = (T_CCP_CLIENT_DB *)p_link->p_ccp_client;
                if (gtbs)
                {
                    if (p_ccp_client->p_gtbs_cb)
                    {
                        ble_audio_mem_free(p_ccp_client->p_gtbs_cb);
                    }

                    p_ccp_client->p_gtbs_cb = ble_audio_mem_zalloc(sizeof(T_TBS_CLIENT_DB));

                    if (p_ccp_client->p_gtbs_cb == NULL)
                    {
                        return APP_RESULT_APP_ERR;
                    }
                    p_ccp_client->p_gtbs_cb->srv_num = 1;
                }
                else
                {
                    if (p_ccp_client->p_tbs_cb)
                    {
                        ble_audio_mem_free(p_ccp_client->p_tbs_cb);
                    }

                    p_ccp_client->p_tbs_cb = ble_audio_mem_zalloc(sizeof(T_TBS_CLIENT_DB) * srv_num);

                    if (p_ccp_client->p_tbs_cb == NULL)
                    {
                        return APP_RESULT_APP_ERR;
                    }
                    p_ccp_client->tbs_num = srv_num;
                    p_ccp_client->p_tbs_cb->srv_num = srv_num;
                }
            }

            dis_done.conn_handle = conn_handle;
            dis_done.gtbs = gtbs;
            dis_done.is_found = p_client_cb_data->dis_done.is_found;
            dis_done.srv_num = srv_num;
            dis_done.load_from_ftl = p_client_cb_data->dis_done.load_from_ftl;
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_CCP_CLIENT_DIS_DONE, &dis_done);
        }
        break;

    case GATT_CLIENT_EVENT_READ_RESULT:
        {
            T_CCP_CLIENT_READ_RESULT read_result = {0};
            read_result.conn_handle = conn_handle;
            read_result.srv_instance_id = p_client_cb_data->read_result.srv_instance_id;
            read_result.cause = p_client_cb_data->read_result.cause;
            read_result.char_uuid = p_client_cb_data->read_result.char_uuid.p.uuid16;
            read_result.gtbs = gtbs;

            if (p_client_cb_data->read_result.value_size == 0)
            {
                p_client_cb_data->read_result.p_value = NULL;
            }

            if (read_result.cause == ATT_SUCCESS)
            {
                switch (read_result.char_uuid)
                {
                case TBS_UUID_CHAR_BEARER_PROVIDER_NAME:
                    {
                        read_result.data.bearer_provider_name.p_bearer_provider_name =
                            p_client_cb_data->read_result.p_value;
                        read_result.data.bearer_provider_name.bearer_provider_name_len =
                            p_client_cb_data->read_result.value_size;
                    }
                    break;

                case TBS_UUID_CHAR_BEARER_UCI:
                    {
                        read_result.data.bearer_uci.p_bearer_uci = p_client_cb_data->read_result.p_value;
                        read_result.data.bearer_uci.bearer_uci_len = p_client_cb_data->read_result.value_size;
                    }
                    break;

                case TBS_UUID_CHAR_BEARER_TECHNOLOGY:
                    {
                        read_result.data.bearer_technology.p_bearer_technology = p_client_cb_data->read_result.p_value;
                        read_result.data.bearer_technology.bearer_technology_len = p_client_cb_data->read_result.value_size;
                    }
                    break;

                case TBS_UUID_CHAR_BEARER_URI_SCHEMES_SUPPORTED_LIST:
                    {
                        read_result.data.bearer_uri_schemes_supported_list.p_bearer_uri_schemes_supported_list =
                            p_client_cb_data->read_result.p_value;
                        read_result.data.bearer_uri_schemes_supported_list.bearer_uri_schemes_supported_list_len =
                            p_client_cb_data->read_result.value_size;
                    }
                    break;

                case TBS_UUID_CHAR_BEARER_SIGNAL_STRENGTH:
                    {
                        if (p_client_cb_data->read_result.value_size != 1)
                        {
                            read_result.cause = APP_RESULT_INVALID_VALUE_SIZE;
                        }
                        else
                        {
                            read_result.data.bearer_signal_strength = *p_client_cb_data->read_result.p_value;
                        }
                    }
                    break;

                case TBS_UUID_CHAR_BEARER_SIGNAL_STRENGTH_REPORTING_INTERVAL:
                    {
                        if (p_client_cb_data->read_result.value_size != 1)
                        {
                            read_result.cause = APP_RESULT_INVALID_VALUE_SIZE;
                        }
                        else
                        {
                            read_result.data.bearer_signal_strength_reporting_interval = *p_client_cb_data->read_result.p_value;
                        }
                    }
                    break;

                case TBS_UUID_CHAR_BEARER_LIST_CURRENT_CALLS:
                    {
                        read_result.data.bearer_list_current_calls.p_bearer_list_current_calls =
                            p_client_cb_data->read_result.p_value;
                        read_result.data.bearer_list_current_calls.bearer_list_current_calls_len =
                            p_client_cb_data->read_result.value_size;
                    }
                    break;

                case TBS_UUID_CHAR_CONTENT_CONTROL_ID:
                    {
                        if (p_client_cb_data->read_result.value_size != 1)
                        {
                            read_result.cause = APP_RESULT_INVALID_VALUE_SIZE;
                        }
                        else
                        {
                            read_result.data.content_control_id = p_client_cb_data->read_result.p_value[0];
                        }
                    }
                    break;

                case TBS_UUID_CHAR_STATUS_FLAGS:
                    {
                        if (p_client_cb_data->read_result.value_size != 2)
                        {
                            read_result.cause = APP_RESULT_INVALID_VALUE_SIZE;
                        }
                        else
                        {
                            LE_ARRAY_TO_UINT16(read_result.data.status_flags, p_client_cb_data->read_result.p_value);
                        }
                    }
                    break;

                case TBS_UUID_CHAR_INCOMING_CALL_TARGET_BEARER_URI:
                    {
                        if (p_client_cb_data->read_result.value_size < 1)
                        {
                            read_result.cause = APP_RESULT_INVALID_VALUE_SIZE;
                        }
                        else
                        {
                            read_result.data.incoming_call_target_bearer_uri.call_index =
                                *p_client_cb_data->read_result.p_value;
                            read_result.data.incoming_call_target_bearer_uri.incoming_call_target_uri_len =
                                p_client_cb_data->read_result.value_size - 1;
                            if (read_result.data.incoming_call_target_bearer_uri.incoming_call_target_uri_len != 0)
                            {
                                read_result.data.incoming_call_target_bearer_uri.p_incoming_call_target_uri =
                                    p_client_cb_data->read_result.p_value + 1;
                            }
                        }
                    }
                    break;

                case TBS_UUID_CHAR_CALL_STATE:
                    {
                        read_result.data.call_state.p_call_state = p_client_cb_data->read_result.p_value;
                        read_result.data.call_state.call_state_len = p_client_cb_data->read_result.value_size;
                    }
                    break;

                case TBS_UUID_CHAR_CALL_CONTROL_POINT_OPTIONAL_OPCODES:
                    {
                        if (p_client_cb_data->read_result.value_size != 2)
                        {
                            read_result.cause = APP_RESULT_INVALID_VALUE_SIZE;
                        }
                        else
                        {
                            LE_ARRAY_TO_UINT16(read_result.data.call_control_point_optional_opcodes,
                                               p_client_cb_data->read_result.p_value);
                        }
                    }
                    break;

                case TBS_UUID_CHAR_INCOMING_CALL:
                    {
                        if (p_client_cb_data->read_result.value_size < 1)
                        {
                            read_result.cause = APP_RESULT_INVALID_VALUE_SIZE;
                        }
                        else
                        {
                            read_result.data.incoming_call.call_index = *p_client_cb_data->read_result.p_value;
                            read_result.data.incoming_call.uri_len = p_client_cb_data->read_result.value_size - 1;
                            if (read_result.data.incoming_call.uri_len != 0)
                            {
                                read_result.data.incoming_call.p_uri = p_client_cb_data->read_result.p_value + 1;
                            }
                        }
                    }
                    break;

                case TBS_UUID_CHAR_CALL_FRIENDLY_NAME:
                    {
                        if (p_client_cb_data->read_result.value_size < 1)
                        {
                            read_result.cause = APP_RESULT_INVALID_VALUE_SIZE;
                        }
                        else
                        {
                            read_result.data.call_friendly_name.call_index = *p_client_cb_data->read_result.p_value;
                            read_result.data.call_friendly_name.friendly_name_len = p_client_cb_data->read_result.value_size -
                                                                                    1;
                            if (read_result.data.call_friendly_name.friendly_name_len != 0)
                            {
                                read_result.data.call_friendly_name.p_friendly_name = p_client_cb_data->read_result.p_value + 1;
                            }
                        }
                    }
                    break;

                default:
                    return APP_RESULT_APP_ERR;
                }
            }

            ble_audio_mgr_dispatch(LE_AUDIO_MSG_CCP_CLIENT_READ_RESULT, &read_result);
        }
        break;

    case GATT_CLIENT_EVENT_WRITE_RESULT:
        {

        }
        break;

    case GATT_CLIENT_EVENT_NOTIFY_IND:
        {
            T_BLE_AUDIO_LINK *p_link = NULL;
            p_link = ble_audio_link_find_by_conn_handle(conn_handle);
            if (p_link == NULL)
            {
                return APP_RESULT_APP_ERR;
            }
            T_CCP_CLIENT_NOTIFY notify_data = {0};

            notify_data.conn_handle = conn_handle;
            notify_data.char_uuid = p_client_cb_data->notify_ind.char_uuid.p.uuid16;
            notify_data.gtbs = gtbs;
            notify_data.srv_instance_id = p_client_cb_data->notify_ind.srv_instance_id;

            if (p_client_cb_data->notify_ind.value_size == 0)
            {
                p_client_cb_data->notify_ind.p_value = NULL;
            }

            switch (notify_data.char_uuid)
            {
            case TBS_UUID_CHAR_BEARER_PROVIDER_NAME:
                {
                    if (p_client_cb_data->notify_ind.value_size == (p_link->mtu_size - 3))
                    {
                        tbs_read_char_value(conn_handle, p_client_cb_data->notify_ind.srv_instance_id,
                                            TBS_UUID_CHAR_BEARER_PROVIDER_NAME, gtbs, NULL);
                        return result;
                    }
                    else
                    {
                        notify_data.data.bearer_provider_name.p_bearer_provider_name = p_client_cb_data->notify_ind.p_value;
                        notify_data.data.bearer_provider_name.bearer_provider_name_len =
                            p_client_cb_data->notify_ind.value_size;
                    }
                }
                break;

            case TBS_UUID_CHAR_BEARER_TECHNOLOGY:
                {
                    notify_data.data.bearer_technology.p_bearer_technology = p_client_cb_data->notify_ind.p_value;
                    notify_data.data.bearer_technology.bearer_technology_len = p_client_cb_data->notify_ind.value_size;
                }
                break;

            case TBS_UUID_CHAR_BEARER_URI_SCHEMES_SUPPORTED_LIST:
                {
                    if (p_client_cb_data->notify_ind.value_size == (p_link->mtu_size - 3))
                    {
                        tbs_read_char_value(conn_handle, p_client_cb_data->notify_ind.srv_instance_id,
                                            TBS_UUID_CHAR_BEARER_URI_SCHEMES_SUPPORTED_LIST, gtbs, NULL);
                        return result;
                    }
                    else
                    {
                        notify_data.data.bearer_uri_schemes_supported_list.p_bearer_uri_schemes_supported_list =
                            p_client_cb_data->notify_ind.p_value;
                        notify_data.data.bearer_uri_schemes_supported_list.bearer_uri_schemes_supported_list_len =
                            p_client_cb_data->notify_ind.value_size;
                    }
                }
                break;

            case TBS_UUID_CHAR_BEARER_SIGNAL_STRENGTH:
                {
                    if (p_client_cb_data->notify_ind.value_size != 1)
                    {
                        return APP_RESULT_APP_ERR;
                    }

                    notify_data.data.bearer_signal_strength = *p_client_cb_data->notify_ind.p_value;
                }
                break;

            case TBS_UUID_CHAR_BEARER_LIST_CURRENT_CALLS:
                {
                    if (p_client_cb_data->notify_ind.value_size == (p_link->mtu_size - 3))
                    {
                        tbs_read_char_value(conn_handle, p_client_cb_data->notify_ind.srv_instance_id,
                                            TBS_UUID_CHAR_BEARER_LIST_CURRENT_CALLS, gtbs, NULL);
                        return result;
                    }
                    else
                    {
                        notify_data.data.bearer_list_current_calls.p_bearer_list_current_calls =
                            p_client_cb_data->notify_ind.p_value;
                        notify_data.data.bearer_list_current_calls.bearer_list_current_calls_len =
                            p_client_cb_data->notify_ind.value_size;
                    }
                }
                break;

            case TBS_UUID_CHAR_STATUS_FLAGS:
                {
                    if (p_client_cb_data->notify_ind.value_size != 2)
                    {
                        return APP_RESULT_APP_ERR;
                    }
                    LE_ARRAY_TO_UINT16(notify_data.data.status_flags, p_client_cb_data->notify_ind.p_value);
                }
                break;

            case TBS_UUID_CHAR_INCOMING_CALL_TARGET_BEARER_URI:
                {
                    if (p_client_cb_data->notify_ind.value_size == (p_link->mtu_size - 3))
                    {
                        tbs_read_char_value(conn_handle, p_client_cb_data->notify_ind.srv_instance_id,
                                            TBS_UUID_CHAR_INCOMING_CALL_TARGET_BEARER_URI, gtbs, NULL);
                        return result;
                    }
                    else
                    {
                        if (p_client_cb_data->notify_ind.value_size < 1)
                        {
                            return APP_RESULT_APP_ERR;
                        }
                        notify_data.data.incoming_call_target_bearer_uri.call_index =
                            p_client_cb_data->notify_ind.p_value[0];
                        notify_data.data.incoming_call_target_bearer_uri.incoming_call_target_uri_len =
                            p_client_cb_data->notify_ind.value_size - 1;
                        if (notify_data.data.incoming_call_target_bearer_uri.incoming_call_target_uri_len != 0)
                        {
                            notify_data.data.incoming_call_target_bearer_uri.p_incoming_call_target_uri =
                                p_client_cb_data->notify_ind.p_value + 1;
                        }
                    }
                }
                break;

            case TBS_UUID_CHAR_CALL_STATE:
                {
                    if (p_client_cb_data->notify_ind.value_size == (p_link->mtu_size - 3))
                    {
                        tbs_read_char_value(conn_handle, p_client_cb_data->notify_ind.srv_instance_id,
                                            TBS_UUID_CHAR_CALL_STATE, gtbs, NULL);
                        return result;
                    }
                    else
                    {
                        notify_data.data.call_state.p_call_state = p_client_cb_data->notify_ind.p_value;
                        notify_data.data.call_state.call_state_len = p_client_cb_data->notify_ind.value_size;
                    }
                }
                break;

            case TBS_UUID_CHAR_CALL_CONTROL_POINT:
                {
                    T_CCP_CLIENT_CALL_CP_NOTIFY call_cp_notify = {0};

                    if (p_client_cb_data->notify_ind.value_size != 3)
                    {
                        return APP_RESULT_APP_ERR;
                    }

                    call_cp_notify.conn_handle = conn_handle;
                    call_cp_notify.srv_instance_id = p_client_cb_data->notify_ind.srv_instance_id;
                    call_cp_notify.gtbs = gtbs;

                    call_cp_notify.requested_opcode = *p_client_cb_data->notify_ind.p_value;
                    call_cp_notify.call_index = p_client_cb_data->notify_ind.p_value[1];
                    call_cp_notify.result_code = p_client_cb_data->notify_ind.p_value[2];
                    ble_audio_mgr_dispatch(LE_AUDIO_MSG_CCP_CLIENT_CALL_CP_NOTIFY, &call_cp_notify);
                    return result;
                }

            case TBS_UUID_CHAR_TERMINATION_REASON:
                {
                    if (p_client_cb_data->notify_ind.value_size != 2)
                    {
                        return APP_RESULT_APP_ERR;
                    }
                    notify_data.data.termination_reason.call_index = *p_client_cb_data->notify_ind.p_value;
                    notify_data.data.termination_reason.reason_code = p_client_cb_data->notify_ind.p_value[1];
                }
                break;

            case TBS_UUID_CHAR_INCOMING_CALL:
                {
                    if (p_client_cb_data->notify_ind.value_size == (p_link->mtu_size - 3))
                    {
                        tbs_read_char_value(conn_handle, p_client_cb_data->notify_ind.srv_instance_id,
                                            TBS_UUID_CHAR_INCOMING_CALL, gtbs, NULL);
                        return result;
                    }
                    else
                    {
                        if (p_client_cb_data->notify_ind.value_size < 1)
                        {
                            return APP_RESULT_APP_ERR;
                        }
                        notify_data.data.incoming_call.call_index = *p_client_cb_data->notify_ind.p_value;
                        notify_data.data.incoming_call.uri_len = p_client_cb_data->notify_ind.value_size - 1;
                        if (notify_data.data.incoming_call.uri_len != 0)
                        {
                            notify_data.data.incoming_call.p_uri = p_client_cb_data->notify_ind.p_value + 1;
                        }
                    }
                }
                break;

            case TBS_UUID_CHAR_CALL_FRIENDLY_NAME:
                {
                    if (p_client_cb_data->notify_ind.value_size == (p_link->mtu_size - 3))
                    {
                        tbs_read_char_value(conn_handle, p_client_cb_data->notify_ind.srv_instance_id,
                                            TBS_UUID_CHAR_CALL_FRIENDLY_NAME, gtbs, NULL);
                        return result;
                    }
                    else
                    {
                        if (p_client_cb_data->notify_ind.value_size < 1)
                        {
                            return APP_RESULT_APP_ERR;
                        }
                        notify_data.data.call_friendly_name.call_index = *p_client_cb_data->notify_ind.p_value;
                        notify_data.data.call_friendly_name.friendly_name_len = p_client_cb_data->notify_ind.value_size - 1;
                        if (notify_data.data.call_friendly_name.friendly_name_len)
                        {
                            notify_data.data.call_friendly_name.p_friendly_name = p_client_cb_data->notify_ind.p_value + 1;
                        }
                    }
                }
                break;
            }
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_CCP_CLIENT_NOTIFY, &notify_data);
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

static T_APP_RESULT gtbs_client_callback(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                         void *p_data)
{
    return tbs_client_handle_callback(conn_handle, type, p_data, true);
}

static T_APP_RESULT tbs_client_callback(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                        void *p_data)
{
    return tbs_client_handle_callback(conn_handle, type, p_data, false);
}

static void tbs_client_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
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
                PROTOCOL_PRINT_ERROR1("tbs_client_dm_cback: not find the link, conn_id %d",
                                      param->conn_state.conn_id);
                break;
            }

            if (param->conn_state.state == GAP_CONN_STATE_DISCONNECTED)
            {
                PROTOCOL_PRINT_INFO0("tbs_client_dm_cback: GAP_CONN_STATE_DISCONNECTED");

                T_CCP_CLIENT_DB  *p_ccp_client = (T_CCP_CLIENT_DB *)p_link->p_ccp_client;
                if (p_ccp_client)
                {
                    if (p_ccp_client->p_gtbs_cb)
                    {
                        ble_audio_mem_free(p_ccp_client->p_gtbs_cb);
                    }
                    if (p_ccp_client->p_tbs_cb)
                    {
                        ble_audio_mem_free(p_ccp_client->p_tbs_cb);
                    }
                    ble_audio_mem_free(p_link->p_ccp_client);
                    p_link->p_ccp_client = NULL;
                }
            }
        }
        break;

    default:
        break;
    }
}
void ccp_client_cfg_cccd(uint16_t conn_handle, uint8_t srv_instance_id, uint32_t cfg_flags,
                         bool enable,
                         bool gtbs)
{
    return tbs_cfg_cccd(conn_handle, srv_instance_id, cfg_flags, enable, gtbs);
}

bool ccp_client_read_char_value(uint16_t conn_handle,  uint8_t srv_instance_id, uint16_t char_uuid,
                                bool gtbs)
{
    return tbs_read_char_value(conn_handle, srv_instance_id, char_uuid, gtbs, NULL);
}

bool ccp_write_sig_rpt_interval(uint16_t conn_handle,  uint8_t srv_instance_id, uint8_t interval,
                                bool is_cmd,
                                bool gtbs)
{
    T_TBS_CLI_WRITE_DATA data;
    data.gtbs = gtbs;
    data.len = 1;
    data.p_value = &interval;
    data.uuid = TBS_UUID_CHAR_BEARER_SIGNAL_STRENGTH_REPORTING_INTERVAL;
    return tbs_write_char_value(conn_handle, srv_instance_id,
                                is_cmd ? GATT_WRITE_TYPE_CMD : GATT_WRITE_TYPE_REQ,
                                &data);
}

bool ccp_client_write_call_cp(uint16_t conn_handle, uint8_t srv_instance_id, bool gtbs, bool is_req,
                              T_CCP_CLIENT_WRITE_CALL_CP_PARAM *p_param)
{
    T_TBS_CLI_WRITE_DATA write_data = {0};
    uint8_t data[2] = {0};

    write_data.gtbs = gtbs;
    write_data.uuid = TBS_UUID_CHAR_CALL_CONTROL_POINT;

    if (p_param->opcode == TBS_CALL_CONTROL_POINT_CHAR_OPCODE_ACCEPT)
    {
        data[0] = TBS_CALL_CONTROL_POINT_CHAR_OPCODE_ACCEPT;
        data[1] = p_param->param.accept_opcode_call_index;
    }
    else if (p_param->opcode == TBS_CALL_CONTROL_POINT_CHAR_OPCODE_TERMINATE)
    {
        data[0] = TBS_CALL_CONTROL_POINT_CHAR_OPCODE_TERMINATE;
        data[1] = p_param->param.terminate_opcode_call_index;
    }
    else if (p_param->opcode == TBS_CALL_CONTROL_POINT_CHAR_OPCODE_LOCAL_HOLD)
    {
        data[0] = TBS_CALL_CONTROL_POINT_CHAR_OPCODE_LOCAL_HOLD;
        data[1] = p_param->param.local_hold_opcode_call_index;
    }
    else if (p_param->opcode == TBS_CALL_CONTROL_POINT_CHAR_OPCODE_LOCAL_RETRIEVE)
    {
        data[0] = TBS_CALL_CONTROL_POINT_CHAR_OPCODE_LOCAL_RETRIEVE;
        data[1] = p_param->param.local_retrieve_opcode_call_index;
    }
    else if (p_param->opcode == TBS_CALL_CONTROL_POINT_CHAR_OPCODE_ORIGINATE)
    {
        uint8_t originate_data[p_param->param.originate_opcode_param.uri_len + 1];

        originate_data[0] = TBS_CALL_CONTROL_POINT_CHAR_OPCODE_ORIGINATE;
        memcpy(&originate_data[1], p_param->param.originate_opcode_param.p_uri,
               p_param->param.originate_opcode_param.uri_len);

        write_data.len = p_param->param.originate_opcode_param.uri_len + 1;
        write_data.p_value = originate_data;

        return tbs_write_char_value(conn_handle, srv_instance_id,
                                    is_req ? GATT_WRITE_TYPE_REQ : GATT_WRITE_TYPE_CMD, &write_data);
    }
    else if (p_param->opcode == TBS_CALL_CONTROL_POINT_CHAR_OPCODE_JOIN)
    {
        uint8_t join_data[p_param->param.join_opcode_param.list_of_call_indexes_len + 1];

        join_data[0] = TBS_CALL_CONTROL_POINT_CHAR_OPCODE_JOIN;
        memcpy(&join_data[1], p_param->param.join_opcode_param.p_list_of_call_indexes,
               p_param->param.join_opcode_param.list_of_call_indexes_len);

        write_data.len = p_param->param.join_opcode_param.list_of_call_indexes_len + 1;
        write_data.p_value = join_data;

        return tbs_write_char_value(conn_handle, srv_instance_id,
                                    is_req ? GATT_WRITE_TYPE_REQ : GATT_WRITE_TYPE_CMD, &write_data);
    }
    else
    {
        return false;
    }

    write_data.len = 2;
    write_data.p_value = data;

    return tbs_write_char_value(conn_handle, srv_instance_id,
                                is_req ? GATT_WRITE_TYPE_REQ : GATT_WRITE_TYPE_CMD, &write_data);
}

bool ccp_client_init()
{
    if (tbs_client_init(gtbs_client_callback, true))
    {
        if (tbs_client_init(tbs_client_callback, false))
        {
            ble_dm_cback_register(tbs_client_dm_cback);
            return true;
        }
    }
    return false;
}

#if LE_AUDIO_DEINIT
void ccp_client_deinit(void)
{
    for (uint8_t k = 0; k < ble_audio_db.acl_link_num; k++)
    {
        if (ble_audio_db.le_link[k].p_ccp_client != NULL)
        {
            T_CCP_CLIENT_DB  *p_ccp_client = (T_CCP_CLIENT_DB *)ble_audio_db.le_link[k].p_ccp_client;
            if (p_ccp_client->p_gtbs_cb)
            {
                ble_audio_mem_free(p_ccp_client->p_gtbs_cb);
            }
            if (p_ccp_client->p_tbs_cb)
            {
                ble_audio_mem_free(p_ccp_client->p_tbs_cb);
            }
            ble_audio_mem_free(p_ccp_client);
            ble_audio_db.le_link[k].p_ccp_client = NULL;
        }
    }
}
#endif

#endif
