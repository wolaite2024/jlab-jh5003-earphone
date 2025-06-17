#include "trace.h"
#include "ble_audio_dm.h"
#include "ble_link_util.h"
#include "tbs_client.h"
#include "ccp_client.h"

#if LE_AUDIO_CCP_CLIENT_SUPPORT
void tbs_cfg_cccd(uint16_t conn_handle, uint8_t srv_instance_id, uint32_t cfg_flags, bool enable,
                  bool gtbs)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = gtbs ? GATT_UUID_GENERIC_TELEPHONE_BEARER_SERVICE :
                        GATT_UUID_TELEPHONE_BEARER_SERVICE;
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

    while (cfg_flags)
    {
        if (cfg_flags & CCP_CLIENT_CFG_CCCD_FLAG_BEARER_PROVIDER_NAME)
        {
            char_uuid.p.uuid16 = TBS_UUID_CHAR_BEARER_PROVIDER_NAME;
            cfg_flags &= ~CCP_CLIENT_CFG_CCCD_FLAG_BEARER_PROVIDER_NAME;
        }
        else if (cfg_flags & CCP_CLIENT_CFG_CCCD_FLAG_BEARER_TECHNOLOGY)
        {
            char_uuid.p.uuid16 = TBS_UUID_CHAR_BEARER_TECHNOLOGY;
            cfg_flags &= ~CCP_CLIENT_CFG_CCCD_FLAG_BEARER_TECHNOLOGY;
        }
        else if (cfg_flags & CCP_CLIENT_CFG_CCCD_FLAG_BEARER_URI_SCHEMES_SUPPORTED_LIST)
        {
            char_uuid.p.uuid16 = TBS_UUID_CHAR_BEARER_URI_SCHEMES_SUPPORTED_LIST;
            cfg_flags &= ~CCP_CLIENT_CFG_CCCD_FLAG_BEARER_URI_SCHEMES_SUPPORTED_LIST;
        }
        else if (cfg_flags & CCP_CLIENT_CFG_CCCD_FLAG_BEARER_SIGNAL_STRENGTH)
        {
            char_uuid.p.uuid16 = TBS_UUID_CHAR_BEARER_SIGNAL_STRENGTH;
            cfg_flags &= ~CCP_CLIENT_CFG_CCCD_FLAG_BEARER_SIGNAL_STRENGTH;
        }
        else if (cfg_flags & CCP_CLIENT_CFG_CCCD_FLAG_BEARER_LIST_CURRENT_CALLS)
        {
            char_uuid.p.uuid16 = TBS_UUID_CHAR_BEARER_LIST_CURRENT_CALLS;
            cfg_flags &= ~CCP_CLIENT_CFG_CCCD_FLAG_BEARER_LIST_CURRENT_CALLS;
        }
        else if (cfg_flags & CCP_CLIENT_CFG_CCCD_FLAG_STATUS_FLAGS)
        {
            char_uuid.p.uuid16 = TBS_UUID_CHAR_STATUS_FLAGS;
            cfg_flags &= ~CCP_CLIENT_CFG_CCCD_FLAG_STATUS_FLAGS;
        }
        else if (cfg_flags & CCP_CLIENT_CFG_CCCD_FLAG_INCOMING_CALL_TARGET_BEARER_URI)
        {
            char_uuid.p.uuid16 = TBS_UUID_CHAR_INCOMING_CALL_TARGET_BEARER_URI;
            cfg_flags &= ~CCP_CLIENT_CFG_CCCD_FLAG_INCOMING_CALL_TARGET_BEARER_URI;
        }
        else if (cfg_flags & CCP_CLIENT_CFG_CCCD_FLAG_CALL_STATE)
        {
            char_uuid.p.uuid16 = TBS_UUID_CHAR_CALL_STATE;
            cfg_flags &= ~CCP_CLIENT_CFG_CCCD_FLAG_CALL_STATE;
        }
        else if (cfg_flags & CCP_CLIENT_CFG_CCCD_FLAG_CALL_CONTROL_POINT)
        {
            char_uuid.p.uuid16 = TBS_UUID_CHAR_CALL_CONTROL_POINT;
            cfg_flags &= ~CCP_CLIENT_CFG_CCCD_FLAG_CALL_CONTROL_POINT;
        }
        else if (cfg_flags & CCP_CLIENT_CFG_CCCD_FLAG_TERMINATION_REASON)
        {
            char_uuid.p.uuid16 = TBS_UUID_CHAR_TERMINATION_REASON;
            cfg_flags &= ~CCP_CLIENT_CFG_CCCD_FLAG_TERMINATION_REASON;
        }
        else if (cfg_flags & CCP_CLIENT_CFG_CCCD_FLAG_INCOMING_CALL)
        {
            char_uuid.p.uuid16 = TBS_UUID_CHAR_INCOMING_CALL;
            cfg_flags &= ~CCP_CLIENT_CFG_CCCD_FLAG_INCOMING_CALL;
        }
        else if (cfg_flags & CCP_CLIENT_CFG_CCCD_FLAG_CALL_FRIENDLY_NAME)
        {
            char_uuid.p.uuid16 = TBS_UUID_CHAR_CALL_FRIENDLY_NAME;
            cfg_flags &= ~CCP_CLIENT_CFG_CCCD_FLAG_CALL_FRIENDLY_NAME;
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

bool tbs_write_char_value(uint16_t conn_handle, uint8_t srv_instance_id, T_GATT_WRITE_TYPE type,
                          T_TBS_CLI_WRITE_DATA *p_data)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);

    if (p_link == NULL)
    {
        return false;
    }

    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = p_data->gtbs ? GATT_UUID_GENERIC_TELEPHONE_BEARER_SERVICE :
                        GATT_UUID_TELEPHONE_BEARER_SERVICE;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = p_data->uuid;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, (((type == GATT_WRITE_TYPE_CMD) &&
                                                 (p_data->len > p_link->mtu_size - 3)) ? GATT_WRITE_TYPE_REQ : type), handle, p_data->len,
                                  p_data->p_value, NULL);
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


bool tbs_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, uint16_t uuid, bool gtbs,
                         P_FUN_GATT_CLIENT_CB p_req_cb)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = gtbs ? GATT_UUID_GENERIC_TELEPHONE_BEARER_SERVICE :
                        GATT_UUID_TELEPHONE_BEARER_SERVICE;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = uuid;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_read(conn_handle, handle, p_req_cb);
    }

    return (cause == GAP_CAUSE_SUCCESS) ? true : false;
}

bool tbs_client_init(P_FUN_GATT_CLIENT_CB app_cb, bool gtbs)
{
    T_ATTR_UUID srv_uuid = {0};
    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = gtbs ? GATT_UUID_GENERIC_TELEPHONE_BEARER_SERVICE :
                        GATT_UUID_TELEPHONE_BEARER_SERVICE;
    if (gatt_client_spec_register(&srv_uuid, app_cb) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    return false;
}

#endif
