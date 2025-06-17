#if XM_XIAOAI_FEATURE_SUPPORT

/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "string.h"
#include "trace.h"
#include "xiaoai_mem.h"

#include "vector.h"

#include "app_main.h"
#include "app_multilink.h"
#include "app_timer.h"
#include "app_xm_xiaoai_ble_adv.h"
#include "app_cfg.h"
#include "app_ble_gap.h"
#include "app_bt_policy_api.h"
#include "xm_xiaoai_api.h"
#include "app_xiaoai_device.h"
#include "app_xiaoai_transport.h"
#include "app_xiaoai_record.h"
#include "app_xiaoai_rcsp.h"
#include "app_sniff_mode.h"
#include "app_auto_power_off.h"
#include "app_hfp.h"
#include "app_a2dp.h"

#define APP_XIAOAI_DEVICE_NUM      (2)

static uint8_t app_xiaoai_timer_id = 0;
static uint8_t timer_idx_xiaoai_adv = 0;
static uint8_t timer_idx_xiaoai_resend_stop_speech = 0;

typedef enum
{
    APP_TIMER_XIAOAI_ADV,
    APP_TIMER_XIAOAI_RESEND_STOP_SPEECH,
} T_APP_XIAOAI_DEVICE_TIMER;

void app_xiaoai_device_start_resend_stop_speech_timer(uint32_t timeout_ms, uint16_t context);
static struct
{
    VECTOR  device_vector;
    T_XM_CMD_COMM_WAY_TYPE active_bt_type;
    bool is_adv_on;
    uint8_t authenticated_dev_count;
} device_mgr;

typedef enum
{
    DEVICE_CREATED              = 0x00,
    DEVICE_DELETED              = 0x01,
    DEVICE_ALL_CREATED          = 0x02, //sync all created devices' info when b2b connected
    DEVICE_ROLESWAP             = 0x03,
    DEVICE_REMOTE_MSG_NUM       = 0x04,
} REMOTE_MSG_TYPE;

typedef struct
{
    uint8_t                     bd_addr[6];
    T_XM_CMD_COMM_WAY_TYPE      bt_type;
    uint8_t                     conn_id; //only for ble device
} T_XIAOAI_SINGLE_DEVICE_CREATE_REMOTE_INFO;

typedef struct
{
    uint8_t device_num;
    T_XIAOAI_SINGLE_DEVICE_CREATE_REMOTE_INFO single_info[1];
} T_XIAOAI_DEVICE_CREATED_DEVICE_REMOTE_INFO;

typedef struct
{
    uint8_t                     bd_addr[6];
    T_XM_CMD_COMM_WAY_TYPE      bt_type;
    uint8_t                     conn_id; //only for ble device
} T_XIAOAI_SINGLE_DEVICE_DELETE_REMOTE_INFO;

typedef struct
{
    uint8_t                     bd_addr[6];
    T_VA_STATE                  xiaoai_va_state;
    //add device role swap information here

} T_XIAOAI_SINGLE_DEVICE_ROLESWAP_REMOTE_INFO;

typedef struct
{
    uint8_t device_num;
    bool    is_adv_on;
    T_XIAOAI_SINGLE_DEVICE_ROLESWAP_REMOTE_INFO single_info[1];
} T_XIAOAI_DEVICE_ROLESWAP_REMOTE_INFO;

typedef struct
{
    union
    {
        T_XIAOAI_DEVICE_CREATED_DEVICE_REMOTE_INFO created_info_all;
        T_XIAOAI_SINGLE_DEVICE_CREATE_REMOTE_INFO create_info;
        T_XIAOAI_SINGLE_DEVICE_DELETE_REMOTE_INFO delete_info;
        T_XIAOAI_DEVICE_ROLESWAP_REMOTE_INFO      roleswap_info;
    };
} XIAOAI_DEVICE_REMOTE_INFO;

static void xiaoai_device_send_info_to_sec(XIAOAI_DEVICE_REMOTE_INFO *p_remote_info,
                                           REMOTE_MSG_TYPE msg_type);

void app_xiaoai_handle_power_off(void)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        le_xm_xiaoai_adv_stop();
    }
}

void app_xiaoai_handle_power_on(bool is_pairing)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        le_xm_xiaoai_adv_start(extend_app_cfg_const.xiaoai_adv_timeout_val);
    }
    else
    {
        //rws mode, wait role decided info, handle in API app_xiaoai_handle_engage_role_decided
    }
}

void app_xiaoai_handle_enter_pair_mode(void)
{
    APP_PRINT_INFO0("app_xiaoai_handle_enter_pair_mode");

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        le_xm_xiaoai_adv_start(extend_app_cfg_const.xiaoai_adv_timeout_val);
    }
}

void app_xiaoai_handle_engage_role_decided(T_BT_DEVICE_MODE radio_mode)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        uint8_t auth_dev_count = app_xiaoai_device_get_authenticated_device_count();

        APP_PRINT_INFO1("app_xiaoai_handle_engage_role_decided: auth_dev_count %d", auth_dev_count);

        if (!app_cfg_const.enable_multi_link)
        {
            //if no b2s xiaoai profile link, start xiaoai adv here
            if (0 == auth_dev_count)
            {
                le_xm_xiaoai_adv_start(extend_app_cfg_const.xiaoai_adv_timeout_val);
            }
        }
        else
        {
            if (auth_dev_count != 2)
            {
                le_xm_xiaoai_adv_start(extend_app_cfg_const.xiaoai_adv_timeout_val);
            }
        }
    }
    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        le_xm_xiaoai_adv_stop();
    }
}

void app_xiaoai_handle_role_swap_success(T_REMOTE_SESSION_ROLE device_role)
{
    if (device_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        app_xiaoai_device_secondary_delete_ble_all();

        if (app_xiaoai_device_get_roleswap_is_adv_on())
        {
            le_xm_xiaoai_adv_start(extend_app_cfg_const.xiaoai_adv_timeout_val);
        }
        else
        {
            uint8_t auth_dev_count = app_xiaoai_device_get_authenticated_device_count();

            APP_PRINT_INFO1("app_xiaoai_handle_role_swap_success: auth_dev_count %d", auth_dev_count);

            if (!app_cfg_const.enable_multi_link)
            {
                //if no b2s xiaoai profile link, start xiaoai adv here
                if (0 == auth_dev_count)
                {
                    le_xm_xiaoai_adv_start(extend_app_cfg_const.xiaoai_adv_timeout_val);
                }
            }
            else
            {
                if (auth_dev_count != 2)
                {
                    le_xm_xiaoai_adv_start(extend_app_cfg_const.xiaoai_adv_timeout_val);
                }
            }
        }
    }
    else if (device_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        le_xm_xiaoai_adv_stop();
    }
}

void app_xiaoai_handle_role_swap_fail(T_REMOTE_SESSION_ROLE device_role)
{
    if (device_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        le_xm_xiaoai_adv_start(extend_app_cfg_const.xiaoai_adv_timeout_val);
    }
}

//TODO use new interface B2B connected
void app_xiaoai_handle_remote_conn_cmpl(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        T_APP_RELAY_MSG_LIST_HANDLE relay_msg_handle = app_relay_async_build();

        app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_XIAOAI_DEVICE, DEVICE_ALL_CREATED);
        app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_XIAOAI_TRANSPORT, TRANSPORT_ROLESWAP);
        app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_XIAOAI_DEVICE, DEVICE_ROLESWAP);

        app_relay_async_send(relay_msg_handle);
    }
}

void app_xiaoai_handle_remote_disconn_cmpl(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_xiaoai_device_secondary_delete_all();
    }
}
static void app_xiaoai_device_default(APP_XIAOAI_DEVICE *p_device)
{

    memset(p_device->bd_addr, 0, sizeof(p_device->bd_addr));

    p_device->p_transport = NULL;
}

static bool match_device_by_addr(APP_XIAOAI_DEVICE *p_device, uint8_t bd_addr[6])
{
    if (memcmp(p_device->bd_addr, bd_addr, sizeof(p_device->bd_addr)) != 0)
    {
        return false;
    }
    return true;
}

static void xiaoai_dev_send_create_info_to_sec(T_XM_CMD_COMM_WAY_TYPE bt_type, uint8_t *bd_addr,
                                               uint8_t conn_id)
{
    XIAOAI_DEVICE_REMOTE_INFO info = {0};

    info.create_info.bt_type = bt_type;
    info.create_info.conn_id = conn_id;
    memcpy(info.create_info.bd_addr, bd_addr, sizeof(info.create_info.bd_addr));

    xiaoai_device_send_info_to_sec(&info, DEVICE_CREATED);
}

static void xiaoai_dev_send_delete_info_to_sec(uint8_t *bd_addr)
{
    XIAOAI_DEVICE_REMOTE_INFO info = {0};

    memcpy(info.delete_info.bd_addr, bd_addr, sizeof(info.delete_info.bd_addr));

    xiaoai_device_send_info_to_sec(&info, DEVICE_DELETED);
}

bool app_xiaoai_device_delete(T_XM_CMD_COMM_WAY_TYPE bt_type, uint8_t *bd_addr, uint8_t conn_id)
{
    APP_XIAOAI_DEVICE *p_device = NULL;
    bool delete_device = true;

    APP_PRINT_TRACE2("app_xiaoai_device_delete: bt_type %d, bd_addr %s", bt_type,
                     TRACE_BDADDR(bd_addr));

    p_device = vector_search(device_mgr.device_vector, (V_PREDICATE) match_device_by_addr,
                             bd_addr);
    if (p_device == NULL)
    {
        APP_PRINT_ERROR0("app_xiaoai_device_delete: VectorSearch failed");
        return false;
    }

    if (p_device->bt_connected[bt_type] == false)
    {
        APP_PRINT_ERROR2("app_xiaoai_device_delete: already deleted bt_type %d, bd_addr %s", bt_type,
                         TRACE_BDADDR(bd_addr));
        return false;
    }

    p_device->bt_connected[bt_type] = false;

    app_xiaoai_transport_delete(bt_type, bd_addr);

    for (uint32_t i = 0; i < T_XM_CMD_COMMUNICATION_WAY_TYPE_NUM; i++)
    {
        if (p_device->bt_connected[bt_type] == true)
        {
            delete_device = false;
            break;
        }
    }

    if (delete_device)
    {
        vector_remove(device_mgr.device_vector, p_device);
        xiaoai_mem_free(p_device, sizeof(APP_XIAOAI_DEVICE));
    }

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        if (bt_type == T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP)
        {
            xiaoai_dev_send_delete_info_to_sec(bd_addr);
        }
    }

    return true;
}

static bool prepare_device_delete_ble(APP_XIAOAI_DEVICE *p_device)
{
    APP_PRINT_INFO1("app_xiaoai_device_secondary_delete_ble_all: prepare_device_delete, bd_addr %s",
                    TRACE_BDADDR(p_device->bd_addr));
    if (p_device->p_transport->stream_for_ble)
    {
        app_xiaoai_device_delete(T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE, p_device->bd_addr, 0);
    }

    return true;
}

void app_xiaoai_device_secondary_delete_ble_all(void)
{
    APP_PRINT_INFO0("app_xiaoai_device_secondary_delete_ble_all");
    vector_mapping(device_mgr.device_vector, (V_MAPPER) prepare_device_delete_ble, NULL);
}

static bool prepare_device_delete(APP_XIAOAI_DEVICE *p_device)
{
    APP_PRINT_INFO1("app_xiaoai_device_secondary_delete_all: prepare_device_delete, bd_addr %s",
                    TRACE_BDADDR(p_device->bd_addr));
    if (p_device->p_transport->stream_for_ble)
    {
        app_xiaoai_device_delete(T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE, p_device->bd_addr, 0);
    }

    if (p_device->p_transport->stream_for_spp)
    {
        app_xiaoai_device_delete(T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP, p_device->bd_addr, 0);
    }

    return true;
}

void app_xiaoai_device_secondary_delete_all(void)
{
    APP_PRINT_INFO0("app_xiaoai_device_secondary_delete_all");
    vector_mapping(device_mgr.device_vector, (V_MAPPER) prepare_device_delete, NULL);
}

static bool prepare_authenticated_device_count(APP_XIAOAI_DEVICE *p_device)
{
    bool is_auth = false;
    if (p_device->p_transport)
    {
        if (p_device->p_transport->stream_for_ble)
        {
            if (p_device->p_transport->stream_for_ble->auth_result)
            {
                APP_PRINT_TRACE0("app_xiaoai_device_get_authenticated_device_count: prepare_authenticated_device_count ble");
                is_auth = true;
            }
        }

        if (p_device->p_transport->stream_for_spp)
        {
            if (p_device->p_transport->stream_for_spp->auth_result)
            {
                APP_PRINT_TRACE0("app_xiaoai_device_get_authenticated_device_count: prepare_authenticated_device_count spp");
                is_auth = true;
            }
        }
    }

    if (is_auth)
    {
        device_mgr.authenticated_dev_count++;
    }
    return true;
}

uint8_t app_xiaoai_device_get_authenticated_device_count(void)
{
    device_mgr.authenticated_dev_count = 0;
    vector_mapping(device_mgr.device_vector, (V_MAPPER) prepare_authenticated_device_count, NULL);
    return device_mgr.authenticated_dev_count;
}

APP_XIAOAI_DEVICE *app_xiaoai_device_create(T_XM_CMD_COMM_WAY_TYPE bt_type, uint8_t *bd_addr,
                                            uint8_t conn_id)
{
    bool ret = false;

    APP_PRINT_TRACE2("app_xiaoai_device_create: bt_type %d, bd_addr %s", bt_type,
                     TRACE_BDADDR(bd_addr));

    APP_XIAOAI_DEVICE *p_device = vector_search(device_mgr.device_vector,
                                                (V_PREDICATE) match_device_by_addr,
                                                bd_addr);
    if (p_device == NULL)
    {
        p_device = xiaoai_mem_zalloc(sizeof(APP_XIAOAI_DEVICE));
        if (p_device == NULL)
        {
            return NULL;
        }

        app_xiaoai_device_default(p_device);
        p_device->xiaoai_va_state = VA_STATE_IDLE;

        memcpy(p_device->bd_addr, bd_addr, sizeof(p_device->bd_addr));

        ret = vector_add(device_mgr.device_vector, p_device);
        if (ret == false)
        {
            APP_PRINT_ERROR0("app_xiaoai_device_create: VectorAdd failed");
            return NULL;
        }
    }
    else
    {
        APP_PRINT_WARN0("app_xiaoai_device_create: device exists");
    }

    if (p_device->p_transport == NULL)
    {
        p_device->p_transport = app_xiaoai_transport_create(bt_type, p_device->bd_addr, 0);
        if (p_device->p_transport == NULL)
        {
            return NULL;
        }

    }
    else
    {
        APP_PRINT_WARN0("app_xiaoai_device_create: transport exists");
    }

    if (app_xiaoai_over_spp_supported() && (bt_type == T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP))
    {
        if (p_device->p_transport->stream_for_spp == NULL)
        {
            p_device->p_transport->stream_for_spp = app_xiaoai_alloc_br_link(bd_addr);
        }
    }

    if (app_xiaoai_over_ble_supported() && (bt_type == T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE))
    {
        if (p_device->p_transport->stream_for_ble == NULL)
        {
            p_device->p_transport->stream_for_ble = app_xiaoai_alloc_le_link_by_conn_id(conn_id, bd_addr);
        }
    }

    p_device->bt_connected[bt_type] = true;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        //only sync spp link info, ble doesn't support roleswap
        if (bt_type == T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP)
        {
            xiaoai_dev_send_create_info_to_sec(bt_type, bd_addr, 0);
        }
    }

    return p_device;
}

static bool prepare_single_device_create_info(APP_XIAOAI_DEVICE *p_device,
                                              XIAOAI_DEVICE_REMOTE_INFO *p_info)
{
    T_XIAOAI_SINGLE_DEVICE_CREATE_REMOTE_INFO *p_single_info =
        &p_info->created_info_all.single_info[p_info->created_info_all.device_num];

    if (p_device->p_transport)
    {
        if (p_device->p_transport->stream_for_spp)
        {
            p_single_info->bt_type = T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP;
            p_single_info->conn_id = 0;
            memcpy(p_single_info->bd_addr, p_device->bd_addr, sizeof(p_single_info->bd_addr));
            p_info->roleswap_info.device_num++;
        }
    }

    return true;
}

void app_xiaoai_handle_b2s_bt_connected(uint8_t *bd_addr)
{
    T_XM_XIAOAI_LINK *p_le_link = app_xiaoai_find_le_link_by_addr(bd_addr);
    if (p_le_link != NULL)
    {
        xm_cmd_f2a_edr_status(T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE, bd_addr, p_le_link->conn_id,
                              T_XM_CMD_F2A_EDR_STATUS_CONNECTED);
    }
    else
    {
        APP_PRINT_ERROR0("app_xiaoai_handle_b2s_bt_connected: p_le_link is NULL");
    }

    app_xiaoai_device_create(T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP, bd_addr, 0xFF);

#if 1
    xm_cmd_f2a_edr_status(T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP, bd_addr, 0xFF,
                          T_XM_CMD_F2A_EDR_STATUS_CONNECTED);
#endif
}

void app_xiaoai_handle_b2s_bt_disconnected(uint8_t *bd_addr)
{
    T_VA_STATE va_state = app_xiaoai_device_get_va_state(bd_addr);
    if (va_state != VA_STATE_IDLE)
    {
        app_xiaoai_stop_recording(bd_addr);
        app_xiaoai_device_set_va_state(bd_addr, VA_STATE_IDLE);
    }
    app_auto_power_off_enable(AUTO_POWER_OFF_MASK_XIAOAI_OTA, app_cfg_const.timer_auto_power_off);

    app_xiaoai_device_delete(T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP, bd_addr, 0);

    if (app_db.device_state == APP_DEVICE_STATE_ON)
    {
        le_xm_xiaoai_adv_start(extend_app_cfg_const.xiaoai_adv_timeout_val);
    }
}

void app_xiaoai_handle_b2s_ble_connected(uint8_t *bd_addr, uint8_t conn_id)
{
    app_xiaoai_device_create(T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE, bd_addr, conn_id);
}

void app_xiaoai_handle_b2s_ble_disconnected(uint8_t *bd_addr, uint8_t conn_id,
                                            uint8_t local_disc_cause, uint16_t disc_cause)
{
    T_VA_STATE va_state = app_xiaoai_device_get_va_state(bd_addr);
    if (va_state != VA_STATE_IDLE)
    {
        app_xiaoai_stop_recording(bd_addr);
        app_xiaoai_device_set_va_state(bd_addr, VA_STATE_IDLE);
    }
    app_auto_power_off_enable(AUTO_POWER_OFF_MASK_XIAOAI_OTA, app_cfg_const.timer_auto_power_off);

    app_xiaoai_device_delete(T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE, bd_addr, conn_id);

    if (local_disc_cause != LE_LOCAL_DISC_CAUSE_POWER_OFF &&
        local_disc_cause != LE_LOCAL_DISC_CAUSE_ROLESWAP)
    {
        le_xm_xiaoai_adv_start(extend_app_cfg_const.xiaoai_adv_timeout_val);
    }
}

T_XM_CMD_COMM_WAY_TYPE app_xiaoai_device_get_active_stream_type(uint8_t *bd_addr)
{
    APP_XIAOAI_DEVICE *p_device = NULL;
    p_device = vector_search(device_mgr.device_vector, (V_PREDICATE) match_device_by_addr,
                             bd_addr);

    T_XIAOAI_TRANSPORT *p_transport;

    if (p_device)
    {
        p_transport = p_device->p_transport;
        if (p_transport)
        {
            return app_xiaoai_get_active_stream_type(p_transport);
        }
    }
    return T_XM_CMD_COMMUNICATION_WAY_TYPE_INVALID;
}

bool app_xiaoai_device_set_va_state(uint8_t *bd_addr, T_VA_STATE va_state)
{
    bool ret = true;
    APP_XIAOAI_DEVICE *p_device = NULL;

    APP_PRINT_INFO2("app_xiaoai_device_set_va_state: bd_addr %s, va_state %d", TRACE_BDADDR(bd_addr),
                    va_state);

    p_device = vector_search(device_mgr.device_vector, (V_PREDICATE) match_device_by_addr,
                             bd_addr);
    if (p_device == NULL)
    {
        APP_PRINT_ERROR0("app_xiaoai_device_set_va_state: device search failed");
        ret = false;
    }
    else
    {
        p_device->xiaoai_va_state = va_state;
    }

    return ret;
}

T_VA_STATE app_xiaoai_device_get_va_state(uint8_t *bd_addr)
{
    APP_XIAOAI_DEVICE *p_device = NULL;
    p_device = vector_search(device_mgr.device_vector, (V_PREDICATE) match_device_by_addr,
                             bd_addr);
    if (p_device == NULL)
    {
        APP_PRINT_ERROR0("app_xiaoai_device_get_va_state: device search failed");
        return VA_STATE_INVALID;
    }
    APP_PRINT_TRACE2("app_xiaoai_device_get_va_state: bd_addr %s, va_state %d", TRACE_BDADDR(bd_addr),
                     p_device->xiaoai_va_state);
    return p_device->xiaoai_va_state;
}

T_XM_CMD_COMM_WAY_TYPE app_xiaoai_device_get_active_comm_way(void)
{
    return device_mgr.active_bt_type;
}

bool app_xiaoai_device_va_start(uint8_t *bd_addr, bool from_local,
                                bool accept_start_speech_from_remote)
{
    bool ret = true;
    uint8_t conn_id = 0xFF;
    APP_XIAOAI_DEVICE *p_device = NULL;
    T_VA_STATE xiaoai_va_state = VA_STATE_IDLE;
    T_XM_XIAOAI_LINK *p_link = NULL;
    T_XM_CMD_COMM_WAY_TYPE way = T_XM_CMD_COMMUNICATION_WAY_TYPE_INVALID;

    p_device = vector_search(device_mgr.device_vector, (V_PREDICATE) match_device_by_addr,
                             bd_addr);
    if (p_device == NULL)
    {
        APP_PRINT_ERROR0("app_xiaoai_device_va_start: device search failed");
        return false;
    }

    p_link = app_xiaoai_find_le_link_by_addr(bd_addr);
    if (p_link && p_link->auth_result)
    {
        way = T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE;
        conn_id = p_link->conn_id;
    }
    else
    {
        p_link = app_xiaoai_find_br_link(bd_addr);
        if (p_link && p_link->auth_result)
        {
            way = T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP;
        }
        else
        {
            APP_PRINT_ERROR0("app_xiaoai_device_va_start: no authenticated xiaoai link exist");
        }
    }
    xiaoai_va_state = p_device->xiaoai_va_state;
    APP_PRINT_INFO4("app_xiaoai_device_va_start: bd_addr %s, way %d, from_local %d, xiaoai_va_state %d",
                    TRACE_BDADDR(bd_addr), way, from_local, xiaoai_va_state);
    if (xiaoai_va_state == VA_STATE_IDLE && app_hfp_get_call_status() == APP_CALL_IDLE)
    {
        app_sniff_mode_b2s_disable(bd_addr, SNIFF_DISABLE_MASK_VA);
        device_mgr.active_bt_type = way;
        if (from_local)
        {
            ret = xm_cmd_speech_start(way, bd_addr, conn_id, 0); //0 : short press, 1: long press
            if (ret)
            {
                app_xiaoai_device_set_va_state(bd_addr, VA_STATE_ACTIVE_PENDING);
            }
        }
        else
        {
            if (accept_start_speech_from_remote)
            {
                app_xiaoai_device_set_va_state(bd_addr, VA_STATE_ACTIVE);
                app_xiaoai_start_recording(bd_addr);
            }
            else//reject start speech from remote
            {
                app_xiaoai_device_set_va_state(bd_addr, VA_STATE_IDLE);
            }
        }
    }
    else if (xiaoai_va_state == VA_STATE_ACTIVE && app_hfp_get_call_status() == APP_CALL_IDLE)
    {
        device_mgr.active_bt_type = way;
        if (from_local)
        {
            APP_PRINT_INFO0("app_xiaoai_device_va_start: Restart xiaoai va");
            ret = xm_cmd_speech_start(way, bd_addr, conn_id, 0); //0 : short press, 1: long press
            if (ret)
            {
                app_xiaoai_device_set_va_state(bd_addr, VA_STATE_ACTIVE_PENDING);
            }
#if 0
            app_xiaoai_stop_recording(bd_addr);
            uint16_t context = 0;
            uint8_t app_index = 0;
            ret = xm_cmd_speech_stop(way, bd_addr, conn_id);
            if (!ret)
            {
                //need to retry
                APP_PRINT_INFO3("app_xiaoai_device_va_start: xm_cmd_speech_stop failed, retry 100ms later, way %d, bd_addr %s, conn_id %d",
                                way, TRACE_BDADDR(bd_addr), conn_id);

                if (way == T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE)
                {
                    T_APP_LE_LINK *p_app_le_link = app_find_le_link_by_addr(bd_addr);
                    app_index = p_app_le_link->id;
                }
                else
                {
                    app_index = app_a2dp_get_active_idx();
                }
                context = (way | (app_index << 8));
                app_xiaoai_device_start_resend_stop_speech_timer(100, context);
            }
            app_sniff_mode_b2s_enable(bd_addr, SNIFF_DISABLE_MASK_VA);
            app_xiaoai_device_set_va_state(bd_addr, VA_STATE_CANCEL_PENDING);
#endif
        }
        else
        {
            if (accept_start_speech_from_remote)
            {
                app_xiaoai_start_recording(bd_addr);
            }
            else//reject start speech from remote
            {
                app_xiaoai_device_set_va_state(bd_addr, VA_STATE_IDLE);
            }
        }
    }
    else
    {
        ret = false;
        APP_PRINT_ERROR1("app_xiaoai_device_va_start: xiaoai_va_state %d", xiaoai_va_state);
    }
    return ret;
}

bool app_xiaoai_device_set_mtu(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                               uint16_t mtu)
{
    bool ret = false;
    T_XIAOAI_TRANSPORT *p_transport = app_xiaoai_find_transport_by_bd_addr(bd_addr);

    APP_PRINT_INFO2("app_xiaoai_device_set_mtu: bd_addr %s, mtu %d", TRACE_BDADDR(bd_addr), mtu);

    if (p_transport)
    {
        if (type == T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE)
        {
            if (p_transport->stream_for_ble)
            {
                p_transport->stream_for_ble->mtu = mtu;
                ret = true;
            }

        }
        if (type == T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP)
        {
            if (p_transport->stream_for_spp)
            {
                p_transport->stream_for_spp->mtu = mtu;
                ret = true;
            }
        }
    }

    return ret;
}

bool app_xiaoai_device_get_mtu(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                               uint16_t *mtu)
{
    bool ret = false;
    T_XIAOAI_TRANSPORT *p_transport = app_xiaoai_find_transport_by_bd_addr(bd_addr);

    if (p_transport)
    {
        if (type == T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE)
        {
            if (p_transport->stream_for_ble)
            {
                *mtu = p_transport->stream_for_ble->mtu - XM_PKT_PREAMBLE_SIZE - XM_PKT_TRAILER_SIZE -
                       XM_PKT_TRAILER_HDR_SIZE - 1;

                //convert mtu to multiple of 4
                *mtu = *mtu & 0xFFFC;
                ret = true;
            }
        }
        else if (type == T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP)
        {
            if (p_transport->stream_for_spp)
            {
                *mtu = p_transport->stream_for_spp->mtu - XM_PKT_PREAMBLE_SIZE - XM_PKT_TRAILER_SIZE -
                       XM_PKT_TRAILER_HDR_SIZE - 1;

                //convert mtu to multiple of 4
                *mtu = *mtu & 0xFFFC;
                ret = true;
            }
        }
    }

    if (ret == false)
    {
        APP_PRINT_ERROR1("app_xiaoai_device_get_mtu: bd_addr %s can't get mtu", TRACE_BDADDR(bd_addr));
    }

    APP_PRINT_INFO2("app_xiaoai_device_get_mtu: bd_addr %s, mtu %d", TRACE_BDADDR(bd_addr), *mtu);

    return ret;
}

static void xiaoai_device_sync_to_primary(REMOTE_MSG_TYPE msg_type,
                                          XIAOAI_DEVICE_REMOTE_INFO *p_info)
{

    APP_PRINT_TRACE1("xiaoai_device_sync_to_primary: msg_type %d", msg_type);

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        APP_PRINT_ERROR0("xiaoai_device_sync_to_primary: this dev should be secondary");
        return;
    }

    switch (msg_type)
    {
    case DEVICE_CREATED:
        {
            app_xiaoai_device_create(p_info->create_info.bt_type, p_info->create_info.bd_addr,
                                     p_info->create_info.conn_id);
        }
        break;

    case DEVICE_DELETED:
        {
            app_xiaoai_device_delete(p_info->delete_info.bt_type, p_info->create_info.bd_addr,
                                     p_info->create_info.conn_id);
        }
        break;

    case DEVICE_ALL_CREATED:
        {
            uint32_t i = 0;
            uint32_t device_num = p_info->created_info_all.device_num;

            T_XIAOAI_SINGLE_DEVICE_CREATE_REMOTE_INFO *p_single_info;

            for (i = 0; i < device_num; i++)
            {
                p_single_info = &p_info->created_info_all.single_info[i];
                app_xiaoai_device_create(p_single_info->bt_type, p_single_info->bd_addr,
                                         p_single_info->conn_id);

                APP_PRINT_TRACE2("xiaoai_device_sync_to_primary: bd_addr %s, device number %d",
                                 TRACE_BDADDR(p_single_info->bd_addr), i);
            }
        }
        break;

    case DEVICE_ROLESWAP:
        {
            uint32_t i = 0;
            uint32_t device_num = p_info->roleswap_info.device_num;
            device_mgr.is_adv_on = p_info->roleswap_info.is_adv_on;

            T_XIAOAI_SINGLE_DEVICE_ROLESWAP_REMOTE_INFO *p_single_info;
            APP_XIAOAI_DEVICE *p_device = NULL;

            for (i = 0; i < device_num; i++)
            {
                p_single_info = &p_info->roleswap_info.single_info[i];
                p_device = vector_search(device_mgr.device_vector, (V_PREDICATE)match_device_by_addr,
                                         p_single_info->bd_addr);
                if (p_device == NULL)
                {
                    APP_PRINT_ERROR1("xiaoai_device_sync_to_primary: no device match %s",
                                     TRACE_BDADDR(p_single_info->bd_addr));
                    continue;
                }

                p_device->xiaoai_va_state = p_single_info->xiaoai_va_state;

                APP_PRINT_TRACE2("xiaoai_device_sync_to_primary: bd_addr %s, xiaoai_va_state %d",
                                 TRACE_BDADDR(p_single_info->bd_addr), p_single_info->xiaoai_va_state);
            }
        }

    default:
        break;
    }
}

static void xiaoai_device_send_info_to_sec(XIAOAI_DEVICE_REMOTE_INFO *p_remote_info,
                                           REMOTE_MSG_TYPE msg_type)
{
    uint16_t len = 0;

    switch (msg_type)
    {
    case DEVICE_CREATED:
        {
            len = sizeof(p_remote_info->create_info);
        }
        break;

    case DEVICE_DELETED:
        {
            len = sizeof(p_remote_info->delete_info);
        }
        break;

    case DEVICE_ROLESWAP:
        {
            len = offsetof(T_XIAOAI_DEVICE_ROLESWAP_REMOTE_INFO, single_info) +
                  p_remote_info->roleswap_info.device_num * sizeof(p_remote_info->roleswap_info.single_info[0]);
        }
        break;

    default:
        break;
    }

    APP_PRINT_INFO2("xiaoai_device_send_info_to_sec: msg_type %d, len = %d", msg_type, len);

    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_XIAOAI_DEVICE, msg_type,
                                        (uint8_t *)p_remote_info, len);
}

static bool prepare_single_device_roleswap_info(APP_XIAOAI_DEVICE *p_device,
                                                XIAOAI_DEVICE_REMOTE_INFO *p_info)
{
    T_XIAOAI_SINGLE_DEVICE_ROLESWAP_REMOTE_INFO *p_single_info =
        &p_info->roleswap_info.single_info[p_info->roleswap_info.device_num];

    memcpy(p_single_info->bd_addr, p_device->bd_addr, sizeof(p_single_info->bd_addr));
    p_single_info->xiaoai_va_state = p_device->xiaoai_va_state;
    //roleswap more device information, such as xiaoai_va_state

    p_info->roleswap_info.device_num++;

    return true;
}

bool app_xiaoai_device_get_roleswap_is_adv_on(void)
{
    return device_mgr.is_adv_on;
}

#if F_APP_ERWS_SUPPORT
static void app_xiaoai_device_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                          T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_INFO1("app_xiaoai_device_parse_cback: len %d", len);
    XIAOAI_DEVICE_REMOTE_INFO *p_info = NULL;
    uint8_t *aligned_buf = NULL;

    if ((uint32_t)buf & 0b11)
    {
        APP_PRINT_WARN0("app_xiaoai_device_parse_cback: buf for struct should be alligned");

        aligned_buf = xiaoai_mem_zalloc(len);
        if (aligned_buf == NULL)
        {
            return;
        }

        memcpy((uint8_t *)aligned_buf, (uint8_t *)buf, len);
        p_info = (XIAOAI_DEVICE_REMOTE_INFO *)aligned_buf;
    }
    else
    {
        p_info = (XIAOAI_DEVICE_REMOTE_INFO *)buf;
    }

    if (status != REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        APP_PRINT_ERROR1("app_xiaoai_device_parse_cback: invalid status %d", status);
        return;
    }

    xiaoai_device_sync_to_primary((REMOTE_MSG_TYPE)msg_type, p_info);

    if (aligned_buf)
    {
        xiaoai_mem_free(aligned_buf, len);
    }
}

uint16_t app_xiaoai_device_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;
    APP_PRINT_INFO1("app_xiaoai_device_relay_cback: msg_type %d", msg_type);
    XIAOAI_DEVICE_REMOTE_INFO *p_info = NULL;

    uint16_t mem_len = 0;
    switch (msg_type)
    {
    case DEVICE_ALL_CREATED:
        {
            uint8_t device_num = 0;
            device_num = vector_depth(device_mgr.device_vector);

            mem_len = sizeof(p_info->created_info_all.device_num) \
                      + device_num * sizeof(p_info->created_info_all.single_info);
            p_info = xiaoai_mem_zalloc(mem_len);
            if (p_info == NULL)
            {
                break;
            }

            p_info->created_info_all.device_num = 0;

            vector_mapping(device_mgr.device_vector, (V_MAPPER) prepare_single_device_create_info, p_info);

            if (p_info->created_info_all.device_num != device_num)
            {
                APP_PRINT_ERROR0("app_xiaoai_device_relay_cback: device num wrong");
            }

            payload_len = offsetof(T_XIAOAI_DEVICE_CREATED_DEVICE_REMOTE_INFO, single_info) +
                          p_info->created_info_all.device_num * sizeof(p_info->created_info_all.single_info[0]);

            msg_ptr = (uint8_t *)p_info;
            skip = false;
        }
        break;

    case DEVICE_ROLESWAP:
        {
            uint8_t device_num = 0;
            mem_len = sizeof(p_info->roleswap_info.device_num) \
                      + sizeof(p_info->roleswap_info.is_adv_on) \
                      + device_num * sizeof(p_info->roleswap_info.single_info);
            device_num = vector_depth(device_mgr.device_vector);
            p_info = xiaoai_mem_zalloc(mem_len);
            if (p_info == NULL)
            {
                break;
            }

            p_info->roleswap_info.device_num = 0;
            p_info->roleswap_info.is_adv_on = le_xm_xiaoai_adv_ongoing();

            vector_mapping(device_mgr.device_vector, (V_MAPPER) prepare_single_device_roleswap_info,
                           p_info); //get all of the device roleswap info

            if (p_info->roleswap_info.device_num != device_num)
            {
                APP_PRINT_ERROR0("app_xiaoai_device_relay_cback: device num wrong");
            }

            payload_len = offsetof(T_XIAOAI_DEVICE_ROLESWAP_REMOTE_INFO, single_info) +
                          p_info->roleswap_info.device_num * sizeof(p_info->roleswap_info.single_info[0]);

            msg_ptr = (uint8_t *)p_info;
            skip = false;

        }
        break;
    default:
        break;
    }

    uint16_t msg_len = payload_len + 4;

    if ((total == true) && (skip == true))
    {
        msg_len = 0;
    }

    if (buf != NULL)
    {
        if (((total == true) && (skip == false)) || (total == false))
        {
            buf[0] = (uint8_t)(msg_len & 0xFF);
            buf[1] = (uint8_t)(msg_len >> 8);
            buf[2] = APP_MODULE_TYPE_XIAOAI_DEVICE;
            buf[3] = msg_type;
            if (payload_len != 0 && msg_ptr != NULL)
            {
                memcpy(&buf[4], msg_ptr, payload_len);
            }
        }
        APP_PRINT_INFO1("app_xiaoai_device_relay_cback: buf %b", TRACE_BINARY(msg_len, buf));
    }

    if (p_info != NULL)
    {
        xiaoai_mem_free(p_info, mem_len);
    }

    return msg_len;
}
#endif

bool xiaoai_device_authenticated_link_exist(uint8_t *bd_addr)
{
    bool ret = true;
    T_XM_XIAOAI_LINK *p_link = NULL;
    p_link = app_xiaoai_find_le_link_by_addr(bd_addr);
    if (p_link && p_link->auth_result)
    {
        ret = true;
    }
    else
    {
        p_link = app_xiaoai_find_br_link(bd_addr);
        if (p_link && p_link->auth_result)
        {
            ret = true;
        }
        else
        {
            APP_PRINT_ERROR0("xiaoai_device_authenticated_link_exist: no authenticated xiaoai link exist");
            ret = false;
        }
    }

    return ret;
}

static void xiaoai_device_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("xiaoai_device_timeout_cb: timer_evt 0x%02x, param %d", timer_evt, param);

    switch (timer_evt)
    {
    case APP_TIMER_XIAOAI_ADV:
        {
            le_xm_xiaoai_adv_stop();
        }
        break;

    case APP_TIMER_XIAOAI_RESEND_STOP_SPEECH:
        {
            uint8_t way = param & 0x00FF;
            uint8_t app_index = (param >> 8) & 0x00FF;
            bool ret = true;
            uint8_t conn_id = 0;
            uint8_t *bd_addr = NULL;

            if (way == T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE)
            {
                T_APP_LE_LINK *p_app_le_link = &app_db.le_link[app_index];
                conn_id = p_app_le_link->conn_id;
                bd_addr = p_app_le_link->bd_addr;
            }
            else
            {
                bd_addr = app_db.br_link[app_index].bd_addr;
            }

            APP_PRINT_INFO3("xiaoai_device_timeout_cb: way %d, bd_addr %s, conn_id %d", way,
                            TRACE_BDADDR(bd_addr), conn_id);
            if (xiaoai_device_authenticated_link_exist(bd_addr))
            {
                ret = xm_cmd_speech_stop((T_XM_CMD_COMM_WAY_TYPE)way, bd_addr, conn_id);
                if (!ret)
                {
                    //need to retry
                    APP_PRINT_ERROR0("xiaoai_device_timeout_cb: xm_cmd_speech_stop failed, need retry");
                    app_xiaoai_device_start_resend_stop_speech_timer(100, param);
                }
            }
        }
        break;

    default:
        break;
    }
}

void app_xiaoai_device_start_adv_timer(uint16_t timeout_sec)
{
    if (timeout_sec != 0)
    {
        app_start_timer(&timer_idx_xiaoai_adv, "xiaoai_adv",
                        app_xiaoai_timer_id, APP_TIMER_XIAOAI_ADV, 0, false,
                        timeout_sec * 1000);
    }
}

void app_xiaoai_device_start_resend_stop_speech_timer(uint32_t timeout_ms, uint16_t context)
{
    app_start_timer(&timer_idx_xiaoai_resend_stop_speech, "xiaoai_resend_stop_speech",
                    app_xiaoai_timer_id, APP_TIMER_XIAOAI_RESEND_STOP_SPEECH, context, false,
                    timeout_ms);
}

void app_xiaoai_device_stop_resend_stop_speech_timer(uint32_t timeout_ms)
{
    app_stop_timer(&timer_idx_xiaoai_resend_stop_speech);
}

void app_xiaoai_device_init(void)
{
    device_mgr.device_vector = vector_create(APP_XIAOAI_DEVICE_NUM);

    if (device_mgr.device_vector == NULL)
    {
        APP_PRINT_ERROR0("app_xiaoai_device_init: vector create failed");
        return;
    }
    device_mgr.active_bt_type = T_XM_CMD_COMMUNICATION_WAY_TYPE_INVALID;
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_xiaoai_device_relay_cback, app_xiaoai_device_parse_cback,
                             APP_MODULE_TYPE_XIAOAI_DEVICE, DEVICE_REMOTE_MSG_NUM);
#endif

    app_timer_reg_cb(xiaoai_device_timeout_cb, &app_xiaoai_timer_id);
}
#endif
