#if CONFIG_REALTEK_BT_GATT_CLIENT_SUPPORT
#include <string.h>
#include "trace.h"
#include "gatt_service_client.h"
#include "bt_gattc_storage.h"
#include "gatt.h"
#include "bt_gatt_client.h"

typedef struct
{
    uint8_t  char_exist;
    uint16_t data_base_hash_handle;
    uint16_t client_supported_handle;
    uint16_t server_supported_handle;
} T_GATT_SERVICE_CHAR_INFO;

uint8_t client_supported_feature = 0;

void gatt_service_save_data(T_GATTC_STORAGE_CB *p_gattc_cb)
{
    if (gatt_storage_cb)
    {
        uint8_t remote_bd[GAP_BD_ADDR_LEN];
        uint8_t remote_bd_type;

        if (gap_chann_get_addr(p_gattc_cb->conn_handle, remote_bd, &remote_bd_type))
        {
            T_GATT_STORAGE_SVC_GATT_SET_IND set_ind = {0};
            set_ind.remote_bd_type = remote_bd_type;
            memcpy(set_ind.addr, remote_bd, 6);
            set_ind.p_gatt_data = &p_gattc_cb->gatt_svc_data;
#if 0
            BTM_PRINT_TRACE7("[GATT] gatt_service_save_data:char_exist 0x%x, data_exist 0x%x, database %b, client len %d, 0x%x, server %d, 0x%x",
                             p_gattc_cb->gatt_svc_data.char_exist,
                             p_gattc_cb->gatt_svc_data.data_exist,
                             TRACE_BINARY(GATT_SVC_DATABASE_HASH_LEN, p_gattc_cb->gatt_svc_data.database_hash),
                             p_gattc_cb->gatt_svc_data.client_supported_features_len,
                             p_gattc_cb->gatt_svc_data.client_supported_features[0],
                             p_gattc_cb->gatt_svc_data.server_supported_features_len,
                             p_gattc_cb->gatt_svc_data.server_supported_features[0]);
#endif
            gatt_storage_cb(GATT_STORAGE_EVENT_SVC_GATT_SET_IND, &set_ind);
        }
    }
}

bool gatt_service_load_data(T_GATTC_STORAGE_CB *p_gattc_cb)
{
    if (gatt_storage_cb)
    {
        uint8_t remote_bd[GAP_BD_ADDR_LEN];
        uint8_t remote_bd_type;

        if (gap_chann_get_addr(p_gattc_cb->conn_handle, remote_bd, &remote_bd_type))
        {
            T_GATT_STORAGE_SVC_GATT_GET_IND get_ind = {0};
            get_ind.remote_bd_type = remote_bd_type;
            memcpy(get_ind.addr, remote_bd, 6);
            get_ind.p_gatt_data = &p_gattc_cb->gatt_svc_data;
            if (gatt_storage_cb(GATT_STORAGE_EVENT_SVC_GATT_GET_IND, &get_ind) == APP_RESULT_SUCCESS)
            {
#if 0
                BTM_PRINT_TRACE7("[GATT] gatt_service_load_data:char_exist 0x%x, data_exist 0x%x, database %b, client len %d, 0x%x, server %d, 0x%x",
                                 p_gattc_cb->gatt_svc_data.char_exist,
                                 p_gattc_cb->gatt_svc_data.data_exist,
                                 TRACE_BINARY(GATT_SVC_DATABASE_HASH_LEN, p_gattc_cb->gatt_svc_data.database_hash),
                                 p_gattc_cb->gatt_svc_data.client_supported_features_len,
                                 p_gattc_cb->gatt_svc_data.client_supported_features[0],
                                 p_gattc_cb->gatt_svc_data.server_supported_features_len,
                                 p_gattc_cb->gatt_svc_data.server_supported_features[0]);
#endif
                return true;
            }
        }
    }
    return false;
}

bool gatt_client_cfg_client_supported_feature(uint8_t feature)
{
    if ((feature | GATT_SVC_CLIENT_SUPPORTED_FEATURES_MASK) == GATT_SVC_CLIENT_SUPPORTED_FEATURES_MASK)
    {
        BTM_PRINT_TRACE1("[GATT] gatt_client_cfg_client_supported_feature:feature 0x%x", feature);
        client_supported_feature = feature;
        return true;
    }
    return false;
}

bool gatt_client_write_client_supported_feature(uint16_t conn_handle,
                                                T_GATTC_STORAGE_CB  *p_gattc_cb)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;

    if (client_supported_feature == p_gattc_cb->gatt_svc_data.client_supported_features[0])
    {
        return false;
    }

    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_GATT;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = GATT_UUID_CHAR_CLIENT_SUPPORTED_FEATURES;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_REQ, handle, 1, &client_supported_feature,
                                  NULL);
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

void gatt_service_get_char_info(T_GATT_SERVICE_CHAR_INFO *p_info, T_GATTC_STORAGE_CB *p_gattc_cb)
{
    T_ATTR_SRV_CB *p_srv_cb;
    T_ATTR_DATA attr_data;
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;

    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_GATT;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;

    p_srv_cb = gattc_storage_find_srv_by_uuid(p_gattc_cb, &srv_uuid);
    if (p_srv_cb == NULL)
    {
        return;
    }

    char_uuid.p.uuid16 = GATT_UUID_CHAR_SERVICE_CHANGED;
    if (gattc_storage_find_char_by_uuid(p_srv_cb, &char_uuid, &attr_data))
    {
        p_info->char_exist |= GATT_SVC_SERVICE_CHANGED_FLAG;

        if (gatt_client_check_cccd_enabled(p_gattc_cb->conn_handle, &srv_uuid, NULL) == false)
        {
            gatt_client_enable_srv_cccd(p_gattc_cb->conn_handle, &srv_uuid, GATT_CLIENT_CONFIG_ALL);
        }
    }

    char_uuid.p.uuid16 = GATT_UUID_CHAR_DATABASE_HASH;
    if (gattc_storage_find_char_by_uuid(p_srv_cb, &char_uuid, &attr_data))
    {
        p_info->char_exist |= GATT_SVC_DATABASE_HASH_FLAG;
        p_info->data_base_hash_handle = attr_data.char_uuid16.value_handle;
    }
    char_uuid.p.uuid16 = GATT_UUID_CHAR_CLIENT_SUPPORTED_FEATURES;
    if (gattc_storage_find_char_by_uuid(p_srv_cb, &char_uuid, &attr_data))
    {
        p_info->char_exist |= GATT_SVC_CLIENT_SUPPORTED_FEATURES_FLAG;
        p_info->client_supported_handle = attr_data.char_uuid16.value_handle;
    }
    char_uuid.p.uuid16 = GATT_UUID_CHAR_SERVER_SUPPORTED_FEATURES;
    if (gattc_storage_find_char_by_uuid(p_srv_cb, &char_uuid, &attr_data))
    {
        p_info->char_exist |= GATT_SVC_SERVER_SUPPORTED_FEATURES_FLAG;
        p_info->server_supported_handle = attr_data.char_uuid16.value_handle;
    }
}

void gatt_service_send_info(T_GATTC_STORAGE_CB *p_gattc_cb, bool save_data)
{

    if (p_gattc_cb->p_general_cb)
    {
        T_GATT_CLIENT_GATT_SERVICE_INFO srv_info;
        srv_info.load_from_ftl = gatt_client_is_load_from_ftl(p_gattc_cb->conn_handle);
        if (p_gattc_cb->gatt_svc_data.char_exist)
        {
            srv_info.is_found = true;
            srv_info.p_gatt_data = &p_gattc_cb->gatt_svc_data;
        }
        else
        {
            srv_info.is_found = false;
            srv_info.p_gatt_data = NULL;
        }
        BTM_PRINT_TRACE2("[GATT] gatt_service_send_info:is_found 0x%x, load_from_ftl %d",
                         srv_info.is_found, srv_info.load_from_ftl);
        p_gattc_cb->p_general_cb(p_gattc_cb->conn_handle, GATT_CLIENT_EVENT_GATT_SERVICE_INFO,
                                 (void *)&srv_info);
    }

    if (save_data)
    {
        gatt_service_save_data(p_gattc_cb);
    }
}

T_APP_RESULT gatt_client_cbs(uint16_t conn_handle, T_GATT_CLIENT_EVENT type, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;
    T_GATTC_STORAGE_CB  *p_gattc_cb = gatt_client_check_link(conn_handle);

    if (p_gattc_cb == NULL)
    {
        return result;
    }

    switch (type)
    {
    case GATT_CLIENT_EVENT_DIS_DONE:
        {
            T_GATT_SERVICE_CHAR_INFO char_info = {0};

            if (p_client_cb_data->dis_done.is_found)
            {
                gatt_service_get_char_info(&char_info, p_gattc_cb);

                if (p_gattc_cb->gatt_svc_data.char_exist == 0 &&
                    p_gattc_cb->gatt_svc_data.data_exist == 0)
                {
                    gatt_service_load_data(p_gattc_cb);
                }

                p_gattc_cb->gatt_svc_data.char_exist = char_info.char_exist;

                if ((char_info.char_exist & GATT_SVC_CLIENT_SUPPORTED_FEATURES_FLAG) &&
                    (p_gattc_cb->gatt_svc_data.data_exist & GATT_SVC_CLIENT_SUPPORTED_FEATURES_FLAG) == 0)
                {
                    if (client_supported_feature != 0)
                    {
                        if (gatt_client_write_client_supported_feature(conn_handle, p_gattc_cb))
                        {
                            p_gattc_cb->gatt_svc_pro_req |= GATT_SVC_WRITE_CLIENT_SUPPORTED_FEATURES_FLAG;
                        }
                    }
                    else
                    {
                        p_gattc_cb->gatt_svc_data.client_supported_features_len = 1;
                        p_gattc_cb->gatt_svc_data.data_exist |= GATT_SVC_CLIENT_SUPPORTED_FEATURES_FLAG;
                        p_gattc_cb->gatt_svc_data.client_supported_features[0] = 0;
                    }
                }

                if ((char_info.char_exist & GATT_SVC_SERVER_SUPPORTED_FEATURES_FLAG) &&
                    (p_gattc_cb->gatt_svc_data.data_exist & GATT_SVC_SERVER_SUPPORTED_FEATURES_FLAG) == 0)
                {
                    if (gatt_client_read(conn_handle, char_info.server_supported_handle, NULL) == GAP_CAUSE_SUCCESS)
                    {
                        p_gattc_cb->gatt_svc_pro_req |= GATT_SVC_SERVER_SUPPORTED_FEATURES_FLAG;
                    }
                }
            }
            BTM_PRINT_TRACE4("[GATT] gatt_client_cbs: GATT_CLIENT_EVENT_DIS_DONE, conn_handle 0x%x, is_found %d, char_exist 0x%x, gatt_svc_pro_req 0x%x",
                             conn_handle, p_client_cb_data->dis_done.is_found,
                             char_info.char_exist, p_gattc_cb->gatt_svc_pro_req);
            if (p_gattc_cb->gatt_svc_pro_req == 0)
            {
                gatt_service_send_info(p_gattc_cb, false);
            }
        }
        break;

    case GATT_CLIENT_EVENT_WRITE_RESULT:
        {
            if (p_client_cb_data->write_result.char_uuid.p.uuid16 == GATT_UUID_CHAR_CLIENT_SUPPORTED_FEATURES)
            {
                if (p_client_cb_data->write_result.cause == GAP_SUCCESS)
                {
                    p_gattc_cb->gatt_svc_data.client_supported_features_len = 1;
                    p_gattc_cb->gatt_svc_data.data_exist |= GATT_SVC_CLIENT_SUPPORTED_FEATURES_FLAG;
                    p_gattc_cb->gatt_svc_data.client_supported_features[0] = client_supported_feature;
                }

                if (p_gattc_cb->gatt_svc_pro_req & GATT_SVC_WRITE_CLIENT_SUPPORTED_FEATURES_FLAG)
                {
                    p_gattc_cb->gatt_svc_pro_req &= ~GATT_SVC_WRITE_CLIENT_SUPPORTED_FEATURES_FLAG;
                }

                if (p_gattc_cb->gatt_svc_pro_req == 0)
                {
                    gatt_service_send_info(p_gattc_cb, true);
                }
            }
        }
        break;

    case GATT_CLIENT_EVENT_READ_RESULT:
        {
            switch (p_client_cb_data->read_result.char_uuid.p.uuid16)
            {
            case GATT_UUID_CHAR_SERVER_SUPPORTED_FEATURES:
                {
                    if (p_client_cb_data->read_result.cause == GAP_SUCCESS)
                    {
                        if (p_client_cb_data->read_result.value_size <= GATT_SVC_CLIENT_SUPPORTED_FEATURES_MAX_LEN)
                        {
                            p_gattc_cb->gatt_svc_data.server_supported_features_len = p_client_cb_data->read_result.value_size;
                            p_gattc_cb->gatt_svc_data.data_exist |= GATT_SVC_SERVER_SUPPORTED_FEATURES_FLAG;
                            if (p_client_cb_data->read_result.value_size)
                            {
                                memcpy(p_gattc_cb->gatt_svc_data.server_supported_features,
                                       p_client_cb_data->read_result.p_value, p_client_cb_data->read_result.value_size);
                            }
                        }
                    }
                    p_gattc_cb->gatt_svc_pro_req &= ~GATT_SVC_SERVER_SUPPORTED_FEATURES_FLAG;
                }
                break;

            default:
                break;
            }

            if (p_gattc_cb->gatt_svc_pro_req == 0)
            {
                gatt_service_send_info(p_gattc_cb, true);
            }
        }
        break;

    case GATT_CLIENT_EVENT_NOTIFY_IND:
        {
            if (p_client_cb_data->notify_ind.char_uuid.p.uuid16 == GATT_UUID_CHAR_SERVICE_CHANGED)
            {
                if (p_client_cb_data->notify_ind.value_size == 4)
                {
                    if (p_gattc_cb->p_general_cb)
                    {
                        T_GATT_CLIENT_SERVICE_CHANGED_INDICATION srv_change;

                        LE_STREAM_TO_UINT16(srv_change.start_handle,
                                            p_client_cb_data->notify_ind.p_value);
                        LE_STREAM_TO_UINT16(srv_change.end_handle,
                                            p_client_cb_data->notify_ind.p_value);
                        BTM_PRINT_INFO3("[GATT] service changed indication: conn_handle 0x%x, start_handle 0x%x, end_handle 0x%x",
                                        conn_handle, srv_change.start_handle, srv_change.end_handle);
                        p_gattc_cb->p_general_cb(p_gattc_cb->conn_handle, GATT_CLIENT_EVENT_SERVICE_CHANGED_INDICATION,
                                                 (void *)&srv_change);
                    }
                }
            }
        }
        break;

    default:
        break;
    }

    return result;
}

bool gatt_service_add(void)
{
    T_ATTR_UUID srv_uuid = {0};

    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = GATT_UUID_GATT;
    if (gatt_client_spec_register(&srv_uuid, gatt_client_cbs) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }

    return false;
}
#endif
