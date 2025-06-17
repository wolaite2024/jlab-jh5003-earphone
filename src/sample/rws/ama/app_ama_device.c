#if AMA_FEATURE_SUPPORT

/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "trace.h"
#include "vector.h"
#include "stdlib_corecrt.h"
#include "app_main.h"
#include "app_cfg.h"


#include "app_relay.h"
#include "app_ama_device.h"
#include "ama_service_api.h"
#include "app_ama_timer.h"
#include "app_ama_ble.h"
#include "app_ama_tone.h"


#define APP_AMA_DEVICE_NUM      (5)


static struct
{
    VECTOR                  device_vector;
    uint8_t                 dev_connected_num;
} device_mgr;


/**  @brief app ama remote msg*/
typedef enum
{
    DEVICE_CREATED          = 0x00,
    DEVICE_DELETED          = 0x01,
    REMOTE_MSG_TYPE_MAX     = 0x02,
} REMOTE_MSG_TYPE;


typedef struct
{
    uint8_t                     bd_addr[BLUETOOTH_ADDR_LENGTH];
    COMMON_STREAM_BT_TYPE       bt_type;
    bool                        bt_connected[COMMON_STREAM_BT_TYPE_MAX];
    uint8_t                     alexa_device_network_conn_status;
    Platform                    alexa_device_os_platform;
    uint32_t                    alexa_speech_dialog_id;
}   SINGLE_DEV_REMOTE_INFO;


typedef struct
{
    uint8_t device_num;
    SINGLE_DEV_REMOTE_INFO device_infos[1];
} AMA_DEVICE_REMOTE_INFO;


static void app_ama_deivce_default(APP_AMA_DEVICE *p_device)
{
    p_device->alexa_device_network_conn_status = 0;
    p_device->alexa_device_os_platform = (Platform)0;
    p_device->alexa_eap_session_id = 0;
    p_device->alexa_eap_session_status = 0;
    p_device->alexa_speech_dialog_id = 0;
    p_device->alexa_va_state = ALEXA_VA_STATE_IDLE;
    memset(p_device->bd_addr, 0, sizeof(p_device->bd_addr));
    p_device->p_proc = NULL;
    p_device->p_transport = NULL;
}


static bool app_ama_device_match_by_addr(APP_AMA_DEVICE *p_device,
                                         uint8_t bd_addr[BLUETOOTH_ADDR_LENGTH])
{
    if (memcmp(p_device->bd_addr, bd_addr, sizeof(p_device->bd_addr)) != 0)
    {
        return false;
    }

    return true;
}


static bool app_ama_device_delete_not_in_primary(APP_AMA_DEVICE *p_device,
                                                 AMA_DEVICE_REMOTE_INFO *p_info)
{
    uint32_t i = 0;
    uint32_t device_num = p_info->device_num;
    SINGLE_DEV_REMOTE_INFO *p_single_info = NULL;

    for (i = 0; i < device_num; i++)
    {
        p_single_info = &p_info->device_infos[i];

        if (memcmp(p_device->bd_addr, p_single_info->bd_addr, sizeof(p_device->bd_addr)) == 0)
        {
            break;
        }
    }

    if (i == device_num)
    {
        APP_PRINT_INFO0("app_ama_device_delete_not_in_primary: device not exists in primary");
        app_ama_device_delete(COMMON_STREAM_BT_BREDR, p_single_info->bd_addr);
        app_ama_device_delete(COMMON_STREAM_BT_LE, p_single_info->bd_addr);
    }

    return true;
}


static void app_ama_device_sync_in_primary(AMA_DEVICE_REMOTE_INFO *p_info)
{
    uint32_t i = 0;
    uint32_t device_num = 0;
    SINGLE_DEV_REMOTE_INFO *p_single_info = NULL;
    APP_AMA_DEVICE *p_device = NULL;

    device_num = p_info->device_num;

    APP_PRINT_TRACE1("app_ama_device_sync_in_primary: device_num %d", device_num);

    for (i = 0; i < device_num; i++)
    {
        p_single_info = &p_info->device_infos[i];
        p_device = vector_search(device_mgr.device_vector, (V_PREDICATE)app_ama_device_match_by_addr,
                                 p_single_info->bd_addr);

        if (p_device == NULL)
        {
            APP_PRINT_INFO1("app_ama_device_sync_in_primary: no device match %s",
                            TRACE_BDADDR(p_single_info->bd_addr));
            if (p_single_info->bt_connected[COMMON_STREAM_BT_BREDR] == true)
            {
                p_device = app_ama_device_create(COMMON_STREAM_BT_BREDR, p_single_info->bd_addr);
            }
            if (p_single_info->bt_connected[COMMON_STREAM_BT_LE] == true)
            {
                p_device = app_ama_device_create(COMMON_STREAM_BT_LE, p_single_info->bd_addr);
            }
            if (p_device == NULL)
            {
                return;
            }
        }
        p_device->alexa_device_network_conn_status = p_single_info->alexa_device_network_conn_status;
        p_device->alexa_device_os_platform = p_single_info->alexa_device_os_platform;
        p_device->alexa_speech_dialog_id = p_single_info->alexa_speech_dialog_id;
        memcpy(p_device->bt_connected, p_single_info->bt_connected, sizeof(p_device->bt_connected));
        APP_PRINT_TRACE6("app_ama_device_sync_to_primary: bd_addr %s, alexa_device_network_conn_status %d"
                         "alexa_device_os_platform %d, alexa_speech_dialog_id %d, bt_connected[0] %d, bt_connected[1] %d",
                         TRACE_BDADDR(p_single_info->bd_addr), p_device->alexa_device_network_conn_status,
                         p_device->alexa_device_os_platform,
                         p_device->alexa_speech_dialog_id, p_device->bt_connected[0], p_device->bt_connected[1]);
    }
}


static void app_ama_device_sync_not_in_primary(AMA_DEVICE_REMOTE_INFO *p_info)
{
    vector_mapping(device_mgr.device_vector, (V_MAPPER) app_ama_device_delete_not_in_primary, p_info);
}


static void app_ama_device_sync_to_primary(REMOTE_MSG_TYPE msg_type,
                                           AMA_DEVICE_REMOTE_INFO *p_info)
{
    APP_PRINT_TRACE1("app_ama_device_sync_to_primary: device_num %d", p_info->device_num);

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }

    switch (msg_type)
    {
    case DEVICE_CREATED:
        {
            if (p_info->device_num == 1)
            {
                app_ama_device_create(p_info->device_infos[0].bt_type, p_info->device_infos[0].bd_addr);
            }
        }
        break;

    case DEVICE_DELETED:
        {
            if (p_info->device_num == 1)
            {
                app_ama_device_delete(p_info->device_infos[0].bt_type, p_info->device_infos[0].bd_addr);
            }
        }
        break;

    default:
        break;
    }
}


static void app_ama_device_send_info_to_sec(AMA_DEVICE_REMOTE_INFO *p_remote_info,
                                            REMOTE_MSG_TYPE msg_type)
{
    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AMA_DEV, msg_type,
                                        (uint8_t *)p_remote_info,
                                        sizeof(*p_remote_info));
}


static bool app_ama_device_prepare_single_device_info(APP_AMA_DEVICE *p_device,
                                                      AMA_DEVICE_REMOTE_INFO *p_info)
{
    SINGLE_DEV_REMOTE_INFO *p_single_info = &p_info->device_infos[p_info->device_num];

    memcpy(p_single_info->bd_addr, p_device->bd_addr, sizeof(p_single_info->bd_addr));
    p_single_info->alexa_device_network_conn_status = p_device->alexa_device_network_conn_status;
    p_single_info->alexa_device_os_platform = p_device->alexa_device_os_platform;
    p_single_info->alexa_speech_dialog_id = p_device->alexa_eap_session_id;
    memcpy(p_single_info->bt_connected, p_device->bt_connected, sizeof(p_single_info->bt_connected));
    p_info->device_num++;

    APP_PRINT_TRACE7("app_ama_device_prepare_single_device_info: device_num %d, bd_addr %s, alexa_device_network_conn_status %d"
                     "alexa_device_os_platform %d, alexa_speech_dialog_id %d, bt_connected[0] %d, bt_connected[1] %d",
                     p_info->device_num, p_single_info->bd_addr, p_single_info->alexa_device_network_conn_status,
                     p_single_info->alexa_device_os_platform,
                     p_single_info->alexa_speech_dialog_id, p_single_info->bt_connected[0],
                     p_single_info->bt_connected[1]);

    return true;
}


static int32_t  app_ama_device_calc_pure_dev_len(uint32_t device_num)
{
    return SIZE_ALIGN4((int32_t)(offsetof(AMA_DEVICE_REMOTE_INFO, device_infos) \
                                 + device_num * sizeof(SINGLE_DEV_REMOTE_INFO)));
}


int32_t app_ama_device_get_info_len(void)
{
    int32_t device_info_len = 0;
    int32_t transport_info_len = 0; //size return by API must be 4 align
    uint32_t device_num = vector_depth(device_mgr.device_vector);

    device_info_len = app_ama_device_calc_pure_dev_len(device_num);
    transport_info_len = app_ama_transport_get_info_len();

    //size calculated should 4 align itself, size return by API must be 4 align
    return  device_info_len + transport_info_len;
}


void app_ama_device_get_info(uint8_t *buf)
{
    uint32_t device_num = vector_depth(device_mgr.device_vector);
    AMA_DEVICE_REMOTE_INFO *p_info = (AMA_DEVICE_REMOTE_INFO *)buf;
    int32_t device_info_len = app_ama_device_calc_pure_dev_len(device_num);

    APP_PRINT_TRACE1("app_ama_device_get_info: device_num %d", device_num);

    p_info->device_num = 0;
    memset(p_info, 0, device_num * sizeof(p_info->device_infos[0]));

    vector_mapping(device_mgr.device_vector, (V_MAPPER) app_ama_device_prepare_single_device_info,
                   p_info);

    APP_PRINT_TRACE2("app_ama_device_get_info: pure dev len %d, data %b", device_info_len,
                     TRACE_BINARY(device_info_len, buf));

    app_ama_transport_get_info(buf + device_info_len);
}


int32_t app_ama_device_set_info(uint8_t *buf)
{
    AMA_DEVICE_REMOTE_INFO *p_info = (AMA_DEVICE_REMOTE_INFO *)buf;

    APP_PRINT_TRACE1("app_ama_device_set_info: device_num %d", p_info->device_num);

    app_ama_device_sync_in_primary(p_info);
    app_ama_device_sync_not_in_primary(p_info);

    int32_t device_info_len = 0;
    device_info_len = app_ama_device_calc_pure_dev_len(p_info->device_num);

    APP_PRINT_TRACE2("app_ama_device_set_info: pure dev len %d, data %b", device_info_len,
                     TRACE_BINARY(device_info_len, buf));

    device_info_len += app_ama_transport_set_info(buf + device_info_len);

    return device_info_len;
}


static void app_ama_device_send_delete_or_create_info_to_sec(REMOTE_MSG_TYPE msg_type,
                                                             COMMON_STREAM_BT_TYPE bt_type, uint8_t *bd_addr)
{
    AMA_DEVICE_REMOTE_INFO info = {0};
    info.device_num = 1;
    info.device_infos[0].bt_type = bt_type;
    memcpy(info.device_infos[0].bd_addr, bd_addr, sizeof(info.device_infos[0].bd_addr));
    app_ama_device_send_info_to_sec(&info, msg_type);
}


bool app_ama_device_delete(COMMON_STREAM_BT_TYPE bt_type, uint8_t *bd_addr)
{
    APP_AMA_DEVICE *p_device = NULL;

    APP_PRINT_TRACE2("app_ama_device_delete: bt_type %d, bd_addr %s", bt_type, TRACE_BDADDR(bd_addr));

    if (bt_type >= COMMON_STREAM_BT_TYPE_MAX)
    {
        return false;
    }

    p_device = vector_search(device_mgr.device_vector, (V_PREDICATE) app_ama_device_match_by_addr,
                             bd_addr);
    if (p_device == NULL)
    {
        return false;
    }

    p_device->bt_connected[bt_type] = false;

    for (uint32_t i = 0; i < COMMON_STREAM_BT_TYPE_MAX; i++)
    {
        if (p_device->bt_connected[bt_type] == true)
        {
            return false;
        }
    }

    app_ama_transport_delete(bd_addr);
    ama_stream_proc_delete(bd_addr);

    vector_remove(device_mgr.device_vector, p_device);
    ama_mem_free(p_device);

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        app_ama_device_send_delete_or_create_info_to_sec(DEVICE_DELETED, bt_type, bd_addr);
    }

    return true;
}


APP_AMA_DEVICE *app_ama_device_create(COMMON_STREAM_BT_TYPE bt_type, uint8_t *bd_addr)
{
    bool ret = false;

    APP_PRINT_TRACE2("app_ama_device_create: bt_type %d, bd_addr %s", bt_type, TRACE_BDADDR(bd_addr));

    if (bt_type >= COMMON_STREAM_BT_TYPE_MAX)
    {
        return NULL;
    }

    APP_AMA_DEVICE *p_device = vector_search(device_mgr.device_vector,
                                             (V_PREDICATE) app_ama_device_match_by_addr,
                                             bd_addr);
    if (p_device == NULL)
    {
        p_device = ama_mem_alloc(AMA_NONVOLATILE_MEMORY, sizeof(APP_AMA_DEVICE));
        if (p_device == NULL)
        {
            return NULL;
        }

        app_ama_deivce_default(p_device);

        memcpy(p_device->bd_addr, bd_addr, sizeof(p_device->bd_addr));

        ret = vector_add(device_mgr.device_vector, p_device);
        if (ret == false)
        {
            APP_PRINT_ERROR0("app_ama_device_create: VectorAdd failed");
            return NULL;
        }

        p_device->p_transport = app_ama_transport_create(p_device->bd_addr);
        p_device->p_proc = ama_stream_proc_create(p_device->bd_addr);

        APP_PRINT_TRACE2("app_ama_device_create p_transport 0x%p, p_proc 0x%p", p_device->p_transport,
                         p_device->p_proc);
    }
    else
    {
        APP_PRINT_ERROR0("app_ama_device_create: device exists");
    }

    p_device->bt_connected[bt_type] = true;

    if (bt_type == COMMON_STREAM_BT_LE)
    {
        //new transport creating for ble, can delete the temp transport
        app_ama_transport_delete_for_temp_ble();
    }

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        app_ama_device_send_delete_or_create_info_to_sec(DEVICE_CREATED, bt_type, bd_addr);
    }

    return p_device;
}


void app_ama_device_handle_b2s_connected(uint8_t *bd_addr)
{
    app_ama_transport_stream_create(AMA_SPP_STREAM, bd_addr, AMA_STREAM_INVALID_CONN_ID);
    app_ama_transport_stream_create(AMA_IAP_STREAM, bd_addr, AMA_STREAM_INVALID_CONN_ID);
    app_ama_device_create(COMMON_STREAM_BT_BREDR, bd_addr);
}


void app_ama_device_handle_b2s_disconnected(uint8_t *bd_addr)
{
    app_ama_transport_stream_delete(AMA_SPP_STREAM, bd_addr, AMA_STREAM_INVALID_CONN_ID);
    app_ama_transport_stream_delete(AMA_IAP_STREAM, bd_addr, AMA_STREAM_INVALID_CONN_ID);
    app_ama_device_delete(COMMON_STREAM_BT_BREDR, bd_addr);
}


static void app_ama_device_handle_transport_evt(AMA_TRANSPORT_EVT evt,
                                                AMA_TRANSPORT_CB_PARAM *p_param)
{
    uint32_t connected_dev_limit = 0;
    connected_dev_limit = app_cfg_const.enable_multi_link == 1 ? 2 : 1;

    APP_PRINT_TRACE2("app_ama_device_handle_transport_evt: evt %d, bd_addr %s", evt,
                     TRACE_BDADDR(p_param->bd_addr));

    switch (evt)
    {
    case AMA_TRANSPORT_CONNECTED_EVT:
        device_mgr.dev_connected_num++;
        if (device_mgr.dev_connected_num >= connected_dev_limit &&
            app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
        {
            app_ama_ble_adv_stop();
        }
        break;


    case AMA_TRANSPORT_DISCONNECTED_EVT:
        device_mgr.dev_connected_num--;
        if (device_mgr.dev_connected_num < connected_dev_limit &&
            app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY &&
            app_db.device_state == APP_DEVICE_STATE_ON)
        {
            app_ama_ble_adv_start();
        }
        break;

    case AMA_TRANSPORT_DATA_RECEIVED:
        APP_PRINT_TRACE1("app_ama_device_handle_transport_evt: AMA_TRANSPORT_DATA_RECEIVED, length %d",
                         p_param->length);
        ama_process_control_pdu(p_param->bd_addr, p_param->buf, p_param->length);
        break;


    default:
        break;
    }
}

#if F_APP_ERWS_SUPPORT
static void app_ama_device_parse(REMOTE_MSG_TYPE msg_type, uint8_t *buf, uint16_t len,
                                 T_REMOTE_RELAY_STATUS status)
{
    if (status != REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        return;
    }

    APP_PRINT_TRACE3("app_ama_device_parse: msg_type %d, len %d, data %b", msg_type, len,
                     TRACE_BINARY(len, buf));

    AMA_DEVICE_REMOTE_INFO info = {0};
    memcpy_s(&info, sizeof(AMA_DEVICE_REMOTE_INFO), buf, len);

    app_ama_device_sync_to_primary(msg_type, &info);
}
#endif

void app_ama_device_init(void)
{
    app_ama_transport_init(app_ama_device_handle_transport_evt);

    device_mgr.device_vector = vector_create(APP_AMA_DEVICE_NUM);
    if (device_mgr.device_vector == NULL)
    {
        return;
    }

#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(NULL, (P_APP_PARSE_CBACK)app_ama_device_parse, APP_MODULE_TYPE_AMA_DEV,
                             REMOTE_MSG_TYPE_MAX);
#endif

    device_mgr.dev_connected_num = 0;
}


void app_ama_device_set_network_conn_status(uint8_t bd_addr[BLUETOOTH_ADDR_LENGTH], uint8_t status)
{
    APP_AMA_DEVICE *p_device = NULL;

    p_device = vector_search(device_mgr.device_vector, (V_PREDICATE) app_ama_device_match_by_addr,
                             bd_addr);
    if (p_device == NULL)
    {
        return;
    }

    p_device->alexa_device_network_conn_status = status;
}


void app_ama_device_set_os_platform(uint8_t *bd_addr, Platform os_platform)
{
    APP_AMA_DEVICE *p_device = NULL;

    p_device = vector_search(device_mgr.device_vector, (V_PREDICATE) app_ama_device_match_by_addr,
                             bd_addr);
    if (p_device == NULL)
    {
        return;
    }
    APP_PRINT_TRACE1("app_ama_device_set_os_platform: os_platform %d", os_platform);

    p_device->alexa_device_os_platform = os_platform;
}


void app_ama_device_set_va_state(uint8_t *bd_addr, T_ALEXA_VA_STATE state)
{
    APP_AMA_DEVICE *p_device = NULL;

    p_device = vector_search(device_mgr.device_vector, (V_PREDICATE) app_ama_device_match_by_addr,
                             bd_addr);
    if (p_device == NULL)
    {
        return;
    }

    p_device->alexa_va_state = state;
}


void app_ama_device_va_start(uint8_t *bd_addr)
{
    APP_AMA_DEVICE *p_device = NULL;
    T_ALEXA_VA_STATE alexa_va_state = ALEXA_VA_STATE_IDLE;
    static uint32_t invalid_cnt = 0;


    APP_PRINT_TRACE1("app_ama_device_va_start: bd_addr %s", TRACE_BDADDR(bd_addr));

    p_device = vector_search(device_mgr.device_vector, (V_PREDICATE) app_ama_device_match_by_addr,
                             bd_addr);
    if (p_device == NULL)
    {
        app_ama_tone_play(TONE_AMA_ERROR_INDICATION);
        return;
    }

    alexa_va_state = p_device->alexa_va_state;

    if (alexa_va_state == ALEXA_VA_STATE_IDLE)
    {
        if (ama_send_start_speech(bd_addr))
        {
            p_device->alexa_va_state = ALEXA_VA_STATE_ACTIVE_PENDING;
            app_ama_timer_start_wait_speech_endpoint_timer();
        }
    }
    else if (alexa_va_state == ALEXA_VA_STATE_ACTIVE)
    {
        if (ama_send_stop_speech(bd_addr))
        {
            p_device->alexa_va_state = ALEXA_VA_STATE_CANCEL_PENDING;
        }
    }
    else
    {
        APP_PRINT_ERROR1("app_ama_device_va_start: alexa_va_state %d",
                         alexa_va_state);
        if (alexa_va_state == ALEXA_VA_STATE_ACTIVE_PENDING ||
            alexa_va_state == ALEXA_VA_STATE_CANCEL_PENDING)
        {
            invalid_cnt++;
            if (invalid_cnt > 2)
            {
                p_device->alexa_va_state = ALEXA_VA_STATE_IDLE;
                invalid_cnt = 0;
                APP_PRINT_TRACE0("app_ama_device_va_start: reset alexa_va_state to ALEXA_VA_STATE_IDLE");
            }
        }
    }
}

void app_ama_device_handle_ble_disconnected(uint8_t *bd_addr, uint8_t conn_id,
                                            uint8_t local_disc_cause, uint16_t disc_cause)
{
    T_BLE_STREAM_EVENT_PARAM ble_stream_param;
    ble_stream_param.disconnect_param.conn_id = conn_id;
    ble_stream_event_process(BLE_STREAM_DISCONNECT_EVENT, &ble_stream_param);
    app_ama_ble_adv_start();

    app_ama_transport_stream_delete(AMA_BLE_STREAM, NULL, conn_id);

    uint8_t bd_addr[6] = {0};
    T_GAP_REMOTE_ADDR_TYPE bd_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    le_get_conn_addr(conn_id, bd_addr, (uint8_t *)&bd_type);
    if (bd_type == GAP_REMOTE_ADDR_LE_PUBLIC)
    {
        app_ama_device_delete(COMMON_STREAM_BT_LE, bd_addr);
    }
    else
    {
        APP_PRINT_ERROR1("app_ble_gap_handle_new_conn_state_evt: bd_type %d, should be GAP_REMOTE_ADDR_LE_PUBLIC",
                         bd_type);
    }
}

uint32_t app_ama_device_get_mtu(uint8_t *bd_addr)
{
    APP_AMA_DEVICE *p_device = NULL;

    p_device = vector_search(device_mgr.device_vector, (V_PREDICATE) app_ama_device_match_by_addr,
                             bd_addr);
    if (p_device == NULL)
    {
        return 0;
    }

    return app_ama_transport_get_mtu(p_device->p_transport);
}

void app_ama_device_tx(uint8_t *bd_addr, uint8_t *data, uint16_t length)
{
    APP_AMA_DEVICE *p_device = NULL;
    AMA_TRANSPORT *p_transport = NULL;

    APP_PRINT_TRACE1("app_ama_device_tx: bd_addr %s", TRACE_BDADDR(bd_addr));

    if (memcmp(bd_addr, COMMON_STREAM_TEMP_ADDR, 6) == 0)
    {
        p_transport = app_ama_transport_find_transport_by_bd_addr((uint8_t *)COMMON_STREAM_TEMP_ADDR);
    }
    else
    {
        p_device = vector_search(device_mgr.device_vector, (V_PREDICATE) app_ama_device_match_by_addr,
                                 bd_addr);
        if (p_device == NULL)
        {
            return;
        }
        p_transport = p_device->p_transport;
    }
    if (p_transport != NULL)
    {
        app_ama_transport_tx(p_transport, data, length);
    }

}


static void app_ama_device_ble_disconnect(APP_AMA_DEVICE *p_device, void *client_data)
{
    APP_PRINT_TRACE1("app_ama_device_ble_disconnect: bd_addr %s", TRACE_BDADDR(p_device->bd_addr));
    if (p_device->p_transport->active_stream_type == AMA_BLE_STREAM)
    {
        app_ama_transport_select_active_stream(p_device->p_transport, AMA_MAX_STREAM);
    }
}


void app_ama_device_ble_disconnect_all(void)
{
    vector_mapping(device_mgr.device_vector, (V_MAPPER)app_ama_device_ble_disconnect, NULL);
}


uint32_t app_ama_device_get_connected_device_num(void)
{
    return device_mgr.dev_connected_num;
}

#endif
