#if F_APP_XIAOWEI_FEATURE_SUPPORT

/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "string.h"
#include "trace.h"
#include "app_timer.h"
#include "xiaowei_protocol.h"

#include "remote.h"
#include "vector.h"

#include "app_main.h"
#include "app_multilink.h"
#include "app_xiaowei_ble_adv.h"
#include "app_cfg.h"
#include "app_ble_gap.h"
#include "app_bt_policy_api.h"

#include "app_xiaowei_device.h"
#include "app_xiaowei_transport.h"
#include "app_xiaowei_record.h"
#include "app_xiaowei.h"
#include "ble_adv_data.h"
#include "stdlib.h"
#include "ble_conn.h"
#include "app_hfp.h"

#define APP_XIAOWEI_DEVICE_NUM                (4)
#define XIAOWEI_MAX_CONNECTED_DEVICE_NUMBER   (2)
#define XIAOWEI_PKT_HEADER                    (6) //cmd_id(1),cmd_type(1),sn(1),rsp_code(1), payload_len(2)
static uint8_t xiaowei_timer_id         = 0;
static uint8_t timer_idx_xiaowei_adv    = 0;
static uint8_t timer_idx_xiaowei_resend = 0;

extern T_XIAOWEI_ADV_DATA xiaowei_adv_data;
extern T_XIAOWEI_SCAN_RSP_DATA xiaowei_scan_rsp_data;
extern void *p_le_xiaowei_adv_handle;

typedef enum
{
    APP_TIMER_XIAOWEI_ADV,
    APP_TIMER_XIAOWEI_RESEND,
} T_XIAOWEI_DEVICE_TIMER;

void app_xiaowei_device_start_resend_stop_speech_timer(uint32_t timeout_ms, uint16_t context);
static struct
{
    VECTOR  device_vector;
    T_XIAOWEI_CONNECTION_TYPE active_bt_type;
    bool is_adv_on;
    uint8_t authenticated_dev_count;//it means how many xiaowei app have been connected to the headset.
} device_mgr;

typedef enum
{
    DEVICE_CREATED          = 0x00,
    DEVICE_DELETED          = 0x01,
    DEVICE_ROLESWAP         = 0x02,
    DEVICE_CREATED_ALL      = 0x03,
    DEVICE_MAX_MSG_NUM      = 0x04,/*send all device create info to secondary, when b2b connected */
} REMOTE_MSG_TYPE;

typedef struct
{
    uint8_t                     bd_addr[6];
    T_XIAOWEI_CONNECTION_TYPE      bt_type;
    uint8_t                     conn_id;// only for ble device
} T_XIAOWEI_SINGLE_DEVICE_CREATE_REMOTE_INFO;

typedef struct
{
    uint8_t                     bd_addr[6];
    T_XIAOWEI_CONNECTION_TYPE      bt_type;
    uint8_t                     conn_id; // only for ble device
} T_XIAOWEI_SINGLE_DEVICE_DELETE_REMOTE_INFO;

typedef struct
{
    uint8_t                     bd_addr[6];
    T_XIAOWEI_VA_STATE                  xiaowei_va_state;
    //add device role swap information here

} T_XIAOWEI_SINGLE_DEVICE_ROLESWAP_REMOTE_INFO;

typedef struct
{
    uint8_t device_num;
    bool    is_adv_on;
    T_XIAOWEI_SINGLE_DEVICE_ROLESWAP_REMOTE_INFO single_info[1];
} T_XIAOWEI_DEVICE_ROLESWAP_REMOTE_INFO;

/**
 * @brief used to send all device create info to secondary,when b2b connected
 *
 */
typedef struct
{
    uint8_t device_num;
    T_XIAOWEI_SINGLE_DEVICE_CREATE_REMOTE_INFO single_info[1];
} T_XIAOWEI_DEVICE_CREATE_REMOTE_INFO_ALL;
typedef struct
{
    union
    {
        T_XIAOWEI_SINGLE_DEVICE_CREATE_REMOTE_INFO create_info;
        T_XIAOWEI_SINGLE_DEVICE_DELETE_REMOTE_INFO delete_info;
        T_XIAOWEI_DEVICE_ROLESWAP_REMOTE_INFO      roleswap_info;
        T_XIAOWEI_DEVICE_CREATE_REMOTE_INFO_ALL    create_info_all;
    };
} XIAOWEI_DEVICE_REMOTE_INFO;

static void xiaowei_device_send_info_to_sec(XIAOWEI_DEVICE_REMOTE_INFO *p_remote_info,
                                            REMOTE_MSG_TYPE msg_type);

void app_xiaowei_handle_power_off(void)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        le_xiaowei_adv_stop();
    }
}

void app_xiaowei_handle_power_on(bool is_pairing)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        le_xiaowei_adv_start(extend_app_cfg_const.xiaowei_adv_timeout);
    }
    else
    {
        //rws mode, wait role decided info, handle in API app_xiaowei_handle_engage_role_decided
    }
}

void app_xiaowei_handle_enter_pair_mode(void)
{
    APP_PRINT_INFO0("app_xiaowei_handle_enter_pair_mode");

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        le_xiaowei_adv_start(extend_app_cfg_const.xiaowei_adv_timeout);
    }
}

void app_xiaowei_handle_engage_role_decided(T_BT_DEVICE_MODE radio_mode)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        uint8_t auth_dev_count = app_xiaowei_device_get_authenticated_device_count();

        APP_PRINT_INFO1("app_xiaowei_handle_engage_role_decided: auth_dev_count %d", auth_dev_count);

        if (!app_cfg_const.enable_multi_link)
        {
            //if no b2s xiaowei profile link, start xiaowei adv here
            if (0 == auth_dev_count)
            {
                le_xiaowei_adv_start(extend_app_cfg_const.xiaowei_adv_timeout);
            }
        }
        else
        {
            if (auth_dev_count < XIAOWEI_MAX_CONNECTED_DEVICE_NUMBER)
            {
                le_xiaowei_adv_start(extend_app_cfg_const.xiaowei_adv_timeout);
            }
        }
    }
    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        le_xiaowei_adv_stop();
    }
}

void app_xiaowei_handle_role_swap_success(T_REMOTE_SESSION_ROLE device_role)
{
    if (device_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        app_xiaowei_device_secondary_delete_ble_all();

        if (app_xiaowei_device_get_roleswap_is_adv_on())
        {
            le_xiaowei_adv_start(extend_app_cfg_const.xiaowei_adv_timeout);
        }
        else
        {
            uint8_t auth_dev_count = app_xiaowei_device_get_authenticated_device_count();

            APP_PRINT_INFO1("app_xiaowei_handle_role_swap_success: auth_dev_count %d", auth_dev_count);

            if (!app_cfg_const.enable_multi_link)
            {
                //if no b2s xiaowei profile link, start xiaowei adv here
                if (0 == auth_dev_count)
                {
                    le_xiaowei_adv_start(extend_app_cfg_const.xiaowei_adv_timeout);
                }
            }
            else
            {
                if (auth_dev_count < XIAOWEI_MAX_CONNECTED_DEVICE_NUMBER)
                {
                    le_xiaowei_adv_start(extend_app_cfg_const.xiaowei_adv_timeout);
                }
            }
        }
    }
    else if (device_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        le_xiaowei_adv_stop();
    }
}

void app_xiaowei_handle_role_swap_fail(T_REMOTE_SESSION_ROLE device_role)
{
    if (device_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        le_xiaowei_adv_start(extend_app_cfg_const.xiaowei_adv_timeout);
    }
}

void xiaowei_dev_send_fvalue_info_to_sec()
{
    APP_PRINT_INFO0("xiaowei_dev_send_fvalue_info_to_sec");
    memcpy(app_xiaowei_fvalues, xiaowei_fvalue_get_all(),
           sizeof(T_XIAOWEI_FVALUE_STORE)*XIAOWEI_FVALUE_MAX_NUMBER);
    app_xiaowei_remote_fvalue(app_xiaowei_fvalues,
                              sizeof(T_XIAOWEI_FVALUE_STORE)*XIAOWEI_FVALUE_MAX_NUMBER);
};

void app_xiaowei_handle_remote_conn_cmpl(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        //sync all xiaowei device info to secondary except ble
        T_APP_RELAY_MSG_LIST_HANDLE relay_msg_handle = app_relay_async_build();
        app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_XIAOWEI_DEVICE, DEVICE_CREATED_ALL);
        app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_XIAOWEI_TRANSPORT,
                            XIAOWEI_TRANSPORT_ROLESWAP);
        app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_XIAOWEI_DEVICE, DEVICE_ROLESWAP);
        app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_XIAOWEI_FVALUE,
                            APP_REMOTE_MSG_XIAOWEI_FVALUE);
        app_relay_async_send(relay_msg_handle);
    }
}

void app_xiaowei_handle_remote_disconn_cmpl(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_xiaowei_device_secondary_delete_all();
    }
}

static void app_xiaowei_device_default(APP_XIAOWEI_DEVICE *p_device)
{
    memset(p_device->bd_addr, 0, sizeof(p_device->bd_addr));
    p_device->p_transport = NULL;
}

static bool match_device_by_addr(APP_XIAOWEI_DEVICE *p_device, uint8_t bd_addr[6])
{
    if (memcmp(p_device->bd_addr, bd_addr, sizeof(p_device->bd_addr)) != 0)
    {
        return false;
    }
    return true;
}

static void xiaowei_dev_send_create_info_to_sec(T_XIAOWEI_CONNECTION_TYPE bt_type, uint8_t *bd_addr,
                                                uint8_t conn_id)
{
    XIAOWEI_DEVICE_REMOTE_INFO info = {0};

    info.create_info.bt_type = bt_type;
    info.create_info.conn_id = conn_id;
    memcpy(info.create_info.bd_addr, bd_addr, sizeof(info.create_info.bd_addr));

    xiaowei_device_send_info_to_sec(&info, DEVICE_CREATED);
}

static void xiaowei_dev_send_delete_info_to_sec(uint8_t *bd_addr)
{
    XIAOWEI_DEVICE_REMOTE_INFO info = {0};

    memcpy(info.delete_info.bd_addr, bd_addr, sizeof(info.delete_info.bd_addr));

    xiaowei_device_send_info_to_sec(&info, DEVICE_DELETED);
}

bool app_xiaowei_device_delete(T_XIAOWEI_CONNECTION_TYPE bt_type, uint8_t *bd_addr, uint8_t conn_id)
{
    APP_XIAOWEI_DEVICE *p_device = NULL;
    bool delete_device = true;

    APP_PRINT_TRACE2("app_xiaowei_device_delete: bt_type %d, bd_addr %s", bt_type,
                     TRACE_BDADDR(bd_addr));

    p_device = vector_search(device_mgr.device_vector, (V_PREDICATE) match_device_by_addr,
                             bd_addr);
    if (p_device == NULL)
    {
        //APP_PRINT_ERROR0("app_xiaowei_device_delete: VectorSearch failed");
        return false;
    }

    if (p_device->bt_connected[bt_type] == false)
    {
        //APP_PRINT_ERROR2("app_xiaowei_device_delete: already deleted bt_type %d, bd_addr %s", bt_type,TRACE_BDADDR(bd_addr));
        return false;
    }

    p_device->bt_connected[bt_type] = false;

    app_xiaowei_transport_delete(bt_type, bd_addr);

    for (uint32_t i = 0; i < XIAOWEI_CONNECTION_TYPE_NUM; i++)
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
        free(p_device);
    }

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        if (bt_type == XIAOWEI_CONNECTION_TYPE_SPP)
        {
            xiaowei_dev_send_delete_info_to_sec(bd_addr);
        }
    }

    return true;
}

static bool prepare_device_delete_ble(APP_XIAOWEI_DEVICE *p_device)
{
    APP_PRINT_INFO1("app_xiaowei_device_secondary_delete_ble_all: prepare_device_delete, bd_addr %s",
                    TRACE_BDADDR(p_device->bd_addr));
    if (p_device->p_transport->stream_for_ble)
    {
        app_xiaowei_device_delete(XIAOWEI_CONNECTION_TYPE_BLE, p_device->bd_addr, 0);
    }

    return true;
}

void app_xiaowei_device_secondary_delete_ble_all(void)
{
    APP_PRINT_INFO0("app_xiaowei_device_secondary_delete_ble_all");
    vector_mapping(device_mgr.device_vector, (V_MAPPER) prepare_device_delete_ble, NULL);
}

static bool prepare_device_delete(APP_XIAOWEI_DEVICE *p_device)
{
    APP_PRINT_INFO1("app_xiaowei_device_secondary_delete_all: prepare_device_delete, bd_addr %s",
                    TRACE_BDADDR(p_device->bd_addr));
    if (p_device->p_transport->stream_for_ble)
    {
        app_xiaowei_device_delete(XIAOWEI_CONNECTION_TYPE_BLE, p_device->bd_addr, 0);
    }

    if (p_device->p_transport->stream_for_spp)
    {
        app_xiaowei_device_delete(XIAOWEI_CONNECTION_TYPE_SPP, p_device->bd_addr, 0);
    }

    return true;
}

void app_xiaowei_device_secondary_delete_all(void)
{
    APP_PRINT_INFO0("app_xiaowei_device_secondary_delete_all");
    vector_mapping(device_mgr.device_vector, (V_MAPPER) prepare_device_delete, NULL);
}

static bool prepare_authenticated_device_count(APP_XIAOWEI_DEVICE *p_device)
{
    bool is_auth = false;
    if (p_device->p_transport)
    {
        if (p_device->p_transport->stream_for_ble)
        {
            if (p_device->p_transport->stream_for_ble->auth_result)
            {
                //APP_PRINT_TRACE0("app_xiaowei_device_get_authenticated_device_count: prepare_authenticated_device_count ble");
                is_auth = true;
            }
        }

        if (p_device->p_transport->stream_for_spp)
        {
            if (p_device->p_transport->stream_for_spp->auth_result)
            {
                //APP_PRINT_TRACE0("app_xiaowei_device_get_authenticated_device_count: prepare_authenticated_device_count spp");
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

uint8_t app_xiaowei_device_get_authenticated_device_count(void)
{
    device_mgr.authenticated_dev_count = 0;
    vector_mapping(device_mgr.device_vector, (V_MAPPER) prepare_authenticated_device_count, NULL);
    return device_mgr.authenticated_dev_count;
}

APP_XIAOWEI_DEVICE *app_xiaowei_device_create(T_XIAOWEI_CONNECTION_TYPE bt_type, uint8_t *bd_addr,
                                              uint8_t conn_id)
{
    bool ret = false;

    APP_PRINT_TRACE2("app_xiaowei_device_create: bt_type %d, bd_addr %s", bt_type,
                     TRACE_BDADDR(bd_addr));

    APP_XIAOWEI_DEVICE *p_device = vector_search(device_mgr.device_vector,
                                                 (V_PREDICATE) match_device_by_addr,
                                                 bd_addr);
    if (p_device == NULL)
    {
        p_device = calloc(1, sizeof(APP_XIAOWEI_DEVICE));
        if (p_device == NULL)
        {
            return NULL;
        }

        app_xiaowei_device_default(p_device);
        p_device->xiaowei_va_state = XIAOWEI_VA_STATE_IDLE;

        memcpy(p_device->bd_addr, bd_addr, sizeof(p_device->bd_addr));

        ret = vector_add(device_mgr.device_vector, p_device);
        if (ret == false)
        {
            //APP_PRINT_ERROR0("app_xiaowei_device_create: VectorAdd failed");
            return NULL;
        }
    }
    else
    {
        //APP_PRINT_WARN0("app_xiaowei_device_create: device exists");
    }

    if (p_device->p_transport == NULL)
    {
        p_device->p_transport = app_xiaowei_transport_create(bt_type, p_device->bd_addr, 0);
        if (p_device->p_transport == NULL)
        {
            return NULL;
        }

    }
    else
    {
        //APP_PRINT_WARN0("app_xiaowei_device_create: transport exists");
    }

    if (app_xiaowei_over_spp_supported() && (bt_type == XIAOWEI_CONNECTION_TYPE_SPP))
    {
        if (p_device->p_transport->stream_for_spp == NULL)
        {
            p_device->p_transport->stream_for_spp = app_xiaowei_alloc_br_link(bd_addr);
        }
    }

    if (app_xiaowei_over_ble_supported() && (bt_type == XIAOWEI_CONNECTION_TYPE_BLE))
    {
        if (p_device->p_transport->stream_for_ble == NULL)
        {
            p_device->p_transport->stream_for_ble = app_xiaowei_alloc_le_link_by_conn_id(conn_id, bd_addr);
        }
    }

    p_device->bt_connected[bt_type] = true;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        //only sync spp link info, ble doesn't support roleswap
        if (bt_type == XIAOWEI_CONNECTION_TYPE_SPP)
        {
            xiaowei_dev_send_create_info_to_sec(bt_type, bd_addr, 0);
        }
    }

    return p_device;
}

static bool prepare_dev_send_create_info_to_sec(APP_XIAOWEI_DEVICE *p_device,
                                                XIAOWEI_DEVICE_REMOTE_INFO *p_info)
{
    if (p_device->p_transport)
    {
        /*only spp(android) device need sync created info to secondary*/
        if (p_device->p_transport->stream_for_spp)
        {
            T_XIAOWEI_SINGLE_DEVICE_CREATE_REMOTE_INFO *p_single_info =
                &p_info->create_info_all.single_info[p_info->create_info_all.device_num];

            memcpy(p_single_info->bd_addr, p_device->bd_addr, sizeof(p_single_info->bd_addr));
            p_single_info->conn_id = 0;
            p_single_info->bt_type = XIAOWEI_CONNECTION_TYPE_SPP;
            //roleswap more device information, such as xiaowei_va_state

            p_info->create_info_all.device_num++;
        }
    }

    return true;
}

void app_xiaowei_handle_b2s_bt_disconnected(uint8_t *bd_addr)
{
    APP_PRINT_INFO0("app_xiaowei_handle_b2s_bt_disconnected");
    T_XIAOWEI_VA_STATE va_state = app_xiaowei_device_get_va_state(bd_addr);
    if (va_state != XIAOWEI_VA_STATE_IDLE)
    {
        app_xiaowei_stop_recording(bd_addr);
        app_xiaowei_device_set_va_state(bd_addr, XIAOWEI_VA_STATE_IDLE);
    }

    app_xiaowei_device_delete(XIAOWEI_CONNECTION_TYPE_SPP, bd_addr, 0);

    if (app_db.device_state == APP_DEVICE_STATE_ON)
    {
        le_xiaowei_adv_start(extend_app_cfg_const.xiaowei_adv_timeout);
    }
}

void app_xiaowei_handle_b2s_ble_connected(uint8_t *bd_addr, uint8_t conn_id)
{
    APP_PRINT_INFO0("app_xiaowei_handle_b2s_ble_connected");
    app_xiaowei_device_create(XIAOWEI_CONNECTION_TYPE_BLE, bd_addr, conn_id);
    ble_set_prefer_conn_param(conn_id, 12, 12, 0, 200);
}

void app_xiaowei_handle_b2s_ble_disconnected(uint8_t *bd_addr, uint8_t conn_id,
                                             uint8_t local_disc_cause, uint16_t disc_cause)
{
    APP_PRINT_INFO1("app_xiaowei_handle_b2s_ble_disconnected: local_disc_cause %d", local_disc_cause);
    T_XIAOWEI_VA_STATE va_state = app_xiaowei_device_get_va_state(bd_addr);
    if (va_state != XIAOWEI_VA_STATE_IDLE)
    {
        app_xiaowei_stop_recording(bd_addr);
        app_xiaowei_device_set_va_state(bd_addr, XIAOWEI_VA_STATE_IDLE);
    }

    app_xiaowei_device_delete(XIAOWEI_CONNECTION_TYPE_BLE, bd_addr, conn_id);

    if (local_disc_cause != LE_LOCAL_DISC_CAUSE_POWER_OFF &&
        local_disc_cause != LE_LOCAL_DISC_CAUSE_ROLESWAP)
    {
        le_xiaowei_adv_start(extend_app_cfg_const.xiaowei_adv_timeout);
    }
}

T_XIAOWEI_CONNECTION_TYPE app_xiaowei_device_get_active_stream_type(uint8_t *bd_addr)
{
    APP_XIAOWEI_DEVICE *p_device = NULL;
    p_device = vector_search(device_mgr.device_vector, (V_PREDICATE) match_device_by_addr,
                             bd_addr);

    T_XIAOWEI_TRANSPORT *p_transport;

    if (p_device)
    {
        p_transport = p_device->p_transport;
        if (p_transport)
        {
            return app_xiaowei_get_active_stream_type(p_transport);
        }
    }
    return XIAOWEI_CONNECTION_TYPE_UNKNOWN;
}

bool app_xiaowei_device_set_va_state(uint8_t *bd_addr, T_XIAOWEI_VA_STATE va_state)
{
    bool ret = true;
    APP_XIAOWEI_DEVICE *p_device = NULL;

    APP_PRINT_INFO2("app_xiaowei_device_set_va_state: bd_addr %s, va_state %d", TRACE_BDADDR(bd_addr),
                    va_state);

    p_device = vector_search(device_mgr.device_vector, (V_PREDICATE) match_device_by_addr,
                             bd_addr);
    if (p_device == NULL)
    {
        //APP_PRINT_ERROR0("app_xiaowei_device_set_va_state: device search failed");
        ret = false;
    }
    else
    {
        p_device->xiaowei_va_state = va_state;
    }

    return ret;
}

T_XIAOWEI_VA_STATE app_xiaowei_device_get_va_state(uint8_t *bd_addr)
{
    APP_XIAOWEI_DEVICE *p_device = NULL;
    p_device = vector_search(device_mgr.device_vector, (V_PREDICATE) match_device_by_addr,
                             bd_addr);
    if (p_device == NULL)
    {
        //APP_PRINT_ERROR0("app_xiaowei_device_get_va_state: device search failed");
        return XIAOWEI_VA_STATE_INVALID;
    }
    APP_PRINT_TRACE2("app_xiaowei_device_get_va_state: bd_addr %s, va_state %d", TRACE_BDADDR(bd_addr),
                     p_device->xiaowei_va_state);
    return p_device->xiaowei_va_state;
}

T_XIAOWEI_CONNECTION_TYPE app_xiaowei_device_get_active_comm_way(void)
{
    return device_mgr.active_bt_type;
}

void app_xiaowei_device_va_stop(uint8_t *bd_addr)
{
    uint8_t conn_id = 0xFF;
    APP_XIAOWEI_DEVICE *p_device = NULL;
    T_XIAOWEI_VA_STATE xiaowei_va_state = XIAOWEI_VA_STATE_IDLE;
    T_XIAOWEI_LINK *p_link = NULL;
    T_XIAOWEI_CONNECTION_TYPE way = XIAOWEI_CONNECTION_TYPE_UNKNOWN;

    p_device = vector_search(device_mgr.device_vector, (V_PREDICATE) match_device_by_addr,
                             bd_addr);
    if (p_device == NULL)
    {
        //APP_PRINT_ERROR0("app_xiaowei_device_va_stop: device search failed");
        return;
    }

    p_link = app_xiaowei_find_le_link_by_addr(bd_addr);
    if (p_link)
    {
        way = XIAOWEI_CONNECTION_TYPE_BLE;
        conn_id = p_link->conn_id;
    }
    else
    {
        p_link = app_xiaowei_find_br_link(bd_addr);
        if (p_link)
        {
            way = XIAOWEI_CONNECTION_TYPE_SPP;
        }
        else
        {
            //APP_PRINT_ERROR0("app_xiaowei_device_va_stop: no authenticated xiaowei link exist");
        }
    }
    xiaowei_va_state = p_device->xiaowei_va_state;
    APP_PRINT_INFO3("app_xiaowei_device_va_stop: bd_addr %s, way %d, xiaowei_va_state %d",
                    TRACE_BDADDR(bd_addr), way, xiaowei_va_state);

    if (xiaowei_va_state != XIAOWEI_VA_STATE_IDLE)
    {
        app_xiaowei_108_send_stop_record_request(way, bd_addr, conn_id);
    }
}

bool app_xiaowei_device_va_start(uint8_t *bd_addr, bool from_local,
                                 bool accept_start_speech_from_remote)
{
    bool ret = true;
    uint8_t conn_id = 0xFF;
    APP_XIAOWEI_DEVICE *p_device = NULL;
    T_XIAOWEI_VA_STATE xiaowei_va_state = XIAOWEI_VA_STATE_IDLE;
    T_XIAOWEI_LINK *p_link = NULL;
    T_XIAOWEI_CONNECTION_TYPE way = XIAOWEI_CONNECTION_TYPE_UNKNOWN;

    p_device = vector_search(device_mgr.device_vector, (V_PREDICATE) match_device_by_addr,
                             bd_addr);
    if (p_device == NULL)
    {
        //APP_PRINT_ERROR0("app_xiaowei_device_va_start: device search failed");
        return false;
    }

    p_link = app_xiaowei_find_le_link_by_addr(bd_addr);
    if (p_link && p_link->auth_result)
    {
        way = XIAOWEI_CONNECTION_TYPE_BLE;
        conn_id = p_link->conn_id;
    }
    else
    {
        p_link = app_xiaowei_find_br_link(bd_addr);
        if (p_link && p_link->auth_result)
        {
            way = XIAOWEI_CONNECTION_TYPE_SPP;
        }
        else
        {
            APP_PRINT_ERROR0("app_xiaowei_device_va_start: no authenticated xiaowei link exist");
        }
    }

    xiaowei_va_state = p_device->xiaowei_va_state;
    APP_PRINT_INFO4("app_xiaowei_device_va_start: bd_addr %s, way %d, from_local %d, xiaowei_va_state %d",
                    TRACE_BDADDR(bd_addr), way, from_local, xiaowei_va_state);

    if ((xiaowei_va_state == XIAOWEI_VA_STATE_IDLE) && (app_hfp_get_call_status() == APP_CALL_IDLE))
    {
        bt_sniff_mode_disable(bd_addr);
        device_mgr.active_bt_type = way;
        if (from_local)
        {
            app_xiaowei_101_send_wakeup_request(way, bd_addr, conn_id);
            app_xiaowei_device_set_va_state(bd_addr, XIAOWEI_VA_STATE_ACTIVE_PENDING);
        }
        else
        {
            if (accept_start_speech_from_remote)
            {
                app_xiaowei_device_set_va_state(bd_addr, XIAOWEI_VA_STATE_ACTIVE);
                app_xiaowei_start_recording(bd_addr);
            }
            else//reject start speech from remote
            {
                app_xiaowei_device_set_va_state(bd_addr, XIAOWEI_VA_STATE_IDLE);
            }
        }
    }
    else if (xiaowei_va_state == XIAOWEI_VA_STATE_ACTIVE)
    {
        if (from_local)
        {
            //APP_PRINT_WARN0("app_xiaowei_device_va_start: do nothing, doesn't support cancel va from local");
        }
        else
        {
            ret = false;
            // APP_PRINT_ERROR0("app_xiaowei_device_va_start: failed");
        }
    }
    else
    {
        ret = false;
        //APP_PRINT_ERROR1("app_xiaowei_device_va_start: xiaowei_va_state %d", xiaowei_va_state);
    }
    return ret;
}

bool app_xiaowei_device_set_mtu(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                                uint16_t mtu)
{
    bool ret = false;
    T_XIAOWEI_TRANSPORT *p_transport = app_xiaowei_find_transport_by_bd_addr(bd_addr);

    APP_PRINT_INFO2("app_xiaowei_device_set_mtu: bd_addr %s, mtu %d", TRACE_BDADDR(bd_addr), mtu);

    if (p_transport)
    {
        if (type == XIAOWEI_CONNECTION_TYPE_BLE)
        {
            if (p_transport->stream_for_ble)
            {
                p_transport->stream_for_ble->mtu = mtu;
                ret = true;
            }

        }
        if (type == XIAOWEI_CONNECTION_TYPE_SPP)
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

bool app_xiaowei_device_get_mtu(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                                uint16_t *mtu)
{
    bool ret = false;
    T_XIAOWEI_TRANSPORT *p_transport = app_xiaowei_find_transport_by_bd_addr(bd_addr);

    if (p_transport)
    {
        if (type == XIAOWEI_CONNECTION_TYPE_BLE)
        {
            if (p_transport->stream_for_ble)
            {
                *mtu = p_transport->stream_for_ble->mtu - XIAOWEI_PKT_HEADER;
                //convert mtu to multiple of 4
                *mtu = *mtu & 0xFFFC;
                ret = true;
            }
        }
        else if (type == XIAOWEI_CONNECTION_TYPE_SPP)
        {
            if (p_transport->stream_for_spp)
            {
                *mtu = p_transport->stream_for_spp->mtu - XIAOWEI_PKT_HEADER;
                //convert mtu to multiple of 4
                *mtu = *mtu & 0xFFFC;
                ret = true;
            }
        }
    }

    if (ret == false)
    {
        //APP_PRINT_ERROR1("app_xiaowei_device_get_mtu: bd_addr %s can't get mtu", TRACE_BDADDR(bd_addr));
    }

    APP_PRINT_INFO2("app_xiaowei_device_get_mtu: bd_addr %s, mtu %d", TRACE_BDADDR(bd_addr), *mtu);

    return ret;
}

static void xiaowei_device_sync_to_primary(REMOTE_MSG_TYPE msg_type,
                                           XIAOWEI_DEVICE_REMOTE_INFO *p_info)
{
    APP_PRINT_TRACE2("xiaowei_device_sync_to_primary: msg_type %d, bud_role %d", msg_type,
                     app_cfg_nv.bud_role);

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        //APP_PRINT_ERROR0("xiaowei_device_sync_to_primary: this dev should be secondary");
        return;
    }

    switch (msg_type)
    {
    case DEVICE_CREATED:
        {
            app_xiaowei_device_create(p_info->create_info.bt_type, p_info->create_info.bd_addr,
                                      p_info->create_info.conn_id);
        }
        break;

    case DEVICE_DELETED:
        {
            app_xiaowei_device_delete(p_info->delete_info.bt_type, p_info->create_info.bd_addr,
                                      p_info->create_info.conn_id);
        }
        break;

    case DEVICE_ROLESWAP:
        {
            uint32_t i = 0;
            uint32_t device_num = p_info->roleswap_info.device_num;
            device_mgr.is_adv_on = p_info->roleswap_info.is_adv_on;

            T_XIAOWEI_SINGLE_DEVICE_ROLESWAP_REMOTE_INFO *p_single_info;
            APP_XIAOWEI_DEVICE *p_device = NULL;

            for (i = 0; i < device_num; i++)
            {
                p_single_info = &p_info->roleswap_info.single_info[i];
                p_device = vector_search(device_mgr.device_vector, (V_PREDICATE)match_device_by_addr,
                                         p_single_info->bd_addr);
                if (p_device == NULL)
                {
                    //APP_PRINT_ERROR1("xiaowei_device_sync_to_primary: no device match %s",TRACE_BDADDR(p_single_info->bd_addr));
                    continue;
                }

                p_device->xiaowei_va_state = p_single_info->xiaowei_va_state;
            }
        }
        break;

    case DEVICE_CREATED_ALL:
        {
            uint32_t i = 0;
            uint32_t device_num = p_info->create_info_all.device_num;

            T_XIAOWEI_SINGLE_DEVICE_CREATE_REMOTE_INFO *p_single_info;

            for (i = 0; i < device_num; i++)
            {
                p_single_info = &p_info->create_info_all.single_info[i];
                app_xiaowei_device_create(p_single_info->bt_type, p_single_info->bd_addr,
                                          p_single_info->conn_id);
            }
        }
        break;
    default:
        break;
    }
}

static void xiaowei_device_send_info_to_sec(XIAOWEI_DEVICE_REMOTE_INFO *p_remote_info,
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
            len = offsetof(T_XIAOWEI_DEVICE_ROLESWAP_REMOTE_INFO, single_info) +
                  p_remote_info->roleswap_info.device_num * sizeof(p_remote_info->roleswap_info.single_info[0]);
        }
        break;

    default:
        break;
    }

    APP_PRINT_INFO2("xiaowei_device_send_info_to_sec: msg_type %d, len = %d", msg_type, len);

    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_XIAOWEI_DEVICE, msg_type,
                                        (uint8_t *)p_remote_info, len);
}

static bool prepare_single_device_roleswap_info(APP_XIAOWEI_DEVICE *p_device,
                                                XIAOWEI_DEVICE_REMOTE_INFO *p_info)
{
    T_XIAOWEI_SINGLE_DEVICE_ROLESWAP_REMOTE_INFO *p_single_info =
        &p_info->roleswap_info.single_info[p_info->roleswap_info.device_num];

    memcpy(p_single_info->bd_addr, p_device->bd_addr, sizeof(p_single_info->bd_addr));
    p_single_info->xiaowei_va_state = p_device->xiaowei_va_state;
    //roleswap more device information, such as xiaowei_va_state

    p_info->roleswap_info.device_num++;

    return true;
}

void app_xiaowei_device_send_roleswap_info(void)
{
    uint8_t device_num = 0;

    app_xiaowei_transport_send_roleswap_info();

    device_num = vector_depth(device_mgr.device_vector);
    XIAOWEI_DEVICE_REMOTE_INFO *p_info = calloc(1,
                                                sizeof(p_info->roleswap_info.device_num) \
                                                + sizeof(p_info->roleswap_info.is_adv_on) \
                                                + device_num * sizeof(p_info->roleswap_info.single_info));
    if (p_info == NULL)
    {
        return;
    }

    p_info->roleswap_info.device_num = 0;
    p_info->roleswap_info.is_adv_on = le_xiaowei_adv_ongoing();

    vector_mapping(device_mgr.device_vector, (V_MAPPER) prepare_single_device_roleswap_info, p_info);

    if (p_info->roleswap_info.device_num != device_num)
    {
        APP_PRINT_ERROR0("app_xiaowei_device_send_roleswap_info: device num wrong");
    }

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        xiaowei_device_send_info_to_sec(p_info, DEVICE_ROLESWAP);
    }

    free(p_info);
}

bool app_xiaowei_device_get_roleswap_is_adv_on(void)
{
    return device_mgr.is_adv_on;
}

#if F_APP_ERWS_SUPPORT
static void app_xiaowei_device_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                           T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE4("app_xiaowei_device_parse_cback: msg_type 0x%04x, status %d,len %d, buf %b",
                     msg_type, status, len, TRACE_BINARY(len, buf));

    if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD && buf != NULL && len != 0)
    {
        xiaowei_device_sync_to_primary((REMOTE_MSG_TYPE)msg_type, (XIAOWEI_DEVICE_REMOTE_INFO *)buf);
    }
}

uint16_t app_xiaowei_device_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;
    XIAOWEI_DEVICE_REMOTE_INFO *p_info_created_all = NULL;
    XIAOWEI_DEVICE_REMOTE_INFO *p_info_roleswap = NULL;
    /*all device in device mgr,include spp(android) and ble(ios) device*/
    uint8_t device_num = vector_depth(device_mgr.device_vector);
    APP_PRINT_INFO1("app_xiaowei_device_relay_cback: msg_type %d", msg_type);

    switch (msg_type)
    {
    case DEVICE_ROLESWAP:
        {
            p_info_roleswap = calloc(1,
                                     sizeof(uint8_t) \
                                     + sizeof(bool) \
                                     + device_num * sizeof(T_XIAOWEI_SINGLE_DEVICE_ROLESWAP_REMOTE_INFO));
            if (p_info_roleswap != NULL && device_num != 0)
            {
                p_info_roleswap->roleswap_info.device_num = 0;
                p_info_roleswap->roleswap_info.is_adv_on = le_xiaowei_adv_ongoing();

                vector_mapping(device_mgr.device_vector, (V_MAPPER) prepare_single_device_roleswap_info,
                               p_info_roleswap);

                if (p_info_roleswap->roleswap_info.device_num != device_num)
                {
                    //APP_PRINT_ERROR0("app_xiaowei_device_relay_cback: device num wrong");
                }

                payload_len = offsetof(T_XIAOWEI_DEVICE_ROLESWAP_REMOTE_INFO, single_info) +
                              p_info_roleswap->roleswap_info.device_num * sizeof(p_info_roleswap->roleswap_info.single_info[0]);
                msg_ptr = (uint8_t *)p_info_roleswap;
                skip = false;
            }
            else
            {
                //APP_PRINT_ERROR0("app_xiaowei_device_relay_cback: nothing");
            }
        }
        break;

    case DEVICE_CREATED_ALL:
        {
            p_info_created_all = calloc(1,
                                        1 + sizeof(T_XIAOWEI_SINGLE_DEVICE_CREATE_REMOTE_INFO) * device_num);

            if (p_info_created_all != NULL && device_num != 0)
            {
                vector_mapping(device_mgr.device_vector, (V_MAPPER) prepare_dev_send_create_info_to_sec,
                               p_info_created_all);

                /*only spp(android) device need sync created info to secondary*/
                if (p_info_created_all->create_info_all.device_num)
                {
                    /*has spp(android) device exist*/
                    payload_len = 1 + sizeof(T_XIAOWEI_SINGLE_DEVICE_CREATE_REMOTE_INFO) *
                                  p_info_created_all->create_info_all.device_num;
                    msg_ptr = (uint8_t *)p_info_created_all;
                    skip = false;
                }
                else
                {
                    payload_len = 0;
                    msg_ptr = NULL;
                    skip = true;
                }

            }
            else
            {
                //APP_PRINT_INFO0("app_xiaowei_device_relay_cback: nothing");
            }
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
            buf[2] = APP_MODULE_TYPE_XIAOWEI_DEVICE;
            buf[3] = msg_type;
            if (payload_len != 0 && msg_ptr != NULL)
            {
                memcpy(&buf[4], msg_ptr, payload_len);
            }
        }
        APP_PRINT_INFO1("app_xiaowei_device_relay_cback: buf %b", TRACE_BINARY(msg_len, buf));
    }

    if (p_info_created_all != NULL)
    {
        free(p_info_created_all);
        p_info_created_all = NULL;
    }

    if (p_info_roleswap != NULL)
    {
        free(p_info_roleswap);
        p_info_roleswap = NULL;
    }

    return msg_len;
}
#endif

bool xiaowei_device_authenticated_link_exist(uint8_t *bd_addr)
{
    bool ret = true;
    T_XIAOWEI_LINK *p_link = NULL;
    p_link = app_xiaowei_find_le_link_by_addr(bd_addr);
    if (p_link && p_link->auth_result)
    {
        ret = true;
    }
    else
    {
        p_link = app_xiaowei_find_br_link(bd_addr);
        if (p_link && p_link->auth_result)
        {
            ret = true;
        }
        else
        {
            APP_PRINT_ERROR0("xiaowei_device_authenticated_link_exist: no authenticated xiaowei link exist");
            ret = false;
        }
    }

    return ret;
}

static void xiaowei_device_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("xiaowei_device_timer_callback: timer_evt 0x%02x, param %d", timer_evt, param);

    switch (timer_evt)
    {
    case APP_TIMER_XIAOWEI_ADV:
        {
            le_xiaowei_adv_stop();
        }
        break;

    case APP_TIMER_XIAOWEI_RESEND:
        {
            uint8_t way = param & 0x00FF;
            uint8_t app_index = (param >> 8) & 0x00FF;
            bool ret = true;
            uint8_t conn_id = 0;
            uint8_t *bd_addr = NULL;

            if (way == XIAOWEI_CONNECTION_TYPE_BLE)
            {
                T_APP_LE_LINK *p_app_le_link = &app_db.le_link[app_index];
                conn_id = p_app_le_link->conn_id;
                bd_addr = p_app_le_link->bd_addr;
            }
            else
            {
                bd_addr = app_db.br_link[app_index].bd_addr;
            }

            APP_PRINT_INFO3("xiaowei_device_timer_callback: way %d, bd_addr %s, conn_id %d", way,
                            TRACE_BDADDR(bd_addr), conn_id);
            if (xiaowei_device_authenticated_link_exist(bd_addr))
            {
//                ret = xm_cmd_speech_stop((T_XIAOWEI_CONNECTION_TYPE)way, bd_addr, conn_id);
                if (!ret)
                {
                    //need to retry
                    //APP_PRINT_ERROR0("xiaowei_device_timer_callback: xm_cmd_speech_stop failed, need retry");
                    app_xiaowei_device_start_resend_stop_speech_timer(100, param);
                }
            }
        }
        break;

    default:
        break;
    }
}

void app_xiaowei_device_start_adv_timer(uint16_t timeout_sec)
{
    if (timeout_sec != 0)
    {
        app_start_timer(&timer_idx_xiaowei_adv, "xiaowei_adv", xiaowei_timer_id,
                        APP_TIMER_XIAOWEI_ADV, 0, false,
                        timeout_sec * 1000);
    }
}

void app_xiaowei_device_start_resend_stop_speech_timer(uint32_t timeout_ms, uint16_t context)
{
    app_start_timer(&timer_idx_xiaowei_resend, "xiaowei_resend", xiaowei_timer_id,
                    APP_TIMER_XIAOWEI_RESEND, context, false,
                    timeout_ms);
}

void app_xiaowei_device_stop_resend_stop_speech_timer(uint32_t timeout_ms)
{
    app_stop_timer(&timer_idx_xiaowei_resend);
}

void app_xiaowei_device_init(void)
{
    device_mgr.device_vector = vector_create(APP_XIAOWEI_DEVICE_NUM);

    if (device_mgr.device_vector == NULL)
    {
        //APP_PRINT_ERROR0("app_xiaowei_device_init: vector create failed");
        return;
    }
    device_mgr.active_bt_type = XIAOWEI_CONNECTION_TYPE_UNKNOWN;
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_xiaowei_device_relay_cback, app_xiaowei_device_parse_cback,
                             APP_MODULE_TYPE_XIAOWEI_DEVICE, DEVICE_MAX_MSG_NUM);
#endif

    app_timer_reg_cb(xiaowei_device_timeout_cb, &xiaowei_timer_id);
}
#endif
