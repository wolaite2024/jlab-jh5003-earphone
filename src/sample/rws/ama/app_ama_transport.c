#if AMA_FEATURE_SUPPORT

#include "app_ama.h"
#include "app_ama_transport.h"
#include "app_ama_spp.h"
#include "app_ama_ble.h"

#include "vector.h"
#include "stdlib_corecrt.h"
#include "app_cfg.h"
#include "app_iap.h"
#include "app_link_util.h"
#include "app_relay.h"


#define MAX_AMA_TRANSPORTS_NUM      (3)


typedef struct
{
    T_SERVER_ID gatt_service_id;
    VECTOR transport_vector;
    AMA_TRANSPORT_CB cb;
} AMA_TRANSPORT_MGR;


static AMA_TRANSPORT_MGR  transport_mgr = {.transport_vector = NULL, .gatt_service_id = 0xff};


typedef struct
{
    uint8_t             bd_addr[BLUETOOTH_ADDR_LENGTH];
    AMA_STREAM_TYPE     active_stream_type;
    uint8_t             conn_id;
    COMMON_STREAM       streams[AMA_MAX_STREAM];
} SINGLE_TRANSPORT_INFO;


typedef struct
{
    uint8_t transport_num;
    SINGLE_TRANSPORT_INFO transport_infos[1]; //the only one is used for created and deleted
} AMA_TRANSPORT_REMOTE_INFO;


/**  @brief app ama remote msg*/
typedef enum
{
    TRANSPORT_CREATED           = 0x00,
    TRANSPORT_DELETED           = 0x01,
    STREAM_CREATED              = 0x02,
    STREAM_DELETED              = 0x03,
    REMOTE_MSG_TYPE_MAX         = 0x04,
} REMOTE_MSG_TYPE;


static bool app_ama_transport_register(AMA_TRANSPORT *p_transport)
{
    bool ret = false;

    ret = vector_add(transport_mgr.transport_vector, p_transport);
    if (ret == false)
    {
    }

    return ret;
}


static bool app_ama_transport_unregister(AMA_TRANSPORT *p_transport)
{
    bool ret = false;

    ret = vector_remove(transport_mgr.transport_vector, p_transport);
    if (ret == false)
    {
    }

    return ret;
}


static bool app_ama_transport_match_transport_by_bd_addr(AMA_TRANSPORT *p_transport,
                                                         uint8_t *bd_addr)
{
    if (memcmp(p_transport->bd_addr, bd_addr, sizeof(p_transport->bd_addr)) != 0)
    {
        return false;
    }

    return true;
}


static bool app_ama_transport_match_transport_by_conn_id(AMA_TRANSPORT *p_transport,
                                                         uint8_t conn_id)
{
    if (p_transport->conn_id != conn_id)
    {
        APP_PRINT_ERROR2("app_ama_transport_match_transport_by_conn_id: conn_id %d, target conn_id %d",
                         p_transport->conn_id,
                         conn_id);
        return false;
    }

    return true;
}



AMA_TRANSPORT *app_ama_transport_find_transport_by_bd_addr(uint8_t bd_addr[BLUETOOTH_ADDR_LENGTH])
{
    AMA_TRANSPORT *p_transport = NULL;

    p_transport = vector_search(transport_mgr.transport_vector,
                                (V_PREDICATE)app_ama_transport_match_transport_by_bd_addr, bd_addr);

    return p_transport;
}


AMA_TRANSPORT *app_ama_transport_find_transport_by_conn_id(uint8_t conn_id)
{
    AMA_TRANSPORT *p_transport = NULL;

    p_transport = vector_search(transport_mgr.transport_vector,
                                (V_PREDICATE)app_ama_transport_match_transport_by_conn_id, (void *)conn_id);

    return p_transport;
}



void app_ama_transport_select_active_stream(AMA_TRANSPORT *p_transport,
                                            AMA_STREAM_TYPE select_stream_type)
{
    APP_PRINT_TRACE1("app_ama_transport_select_active_stream: select_stream_type %d",
                     select_stream_type);

    p_transport->active_stream_type = select_stream_type;

    if (select_stream_type > AMA_MAX_STREAM)
    {

    }
}


static bool app_ama_transport_delete_not_in_primary(AMA_TRANSPORT *p_transport,
                                                    AMA_TRANSPORT_REMOTE_INFO *p_info)
{
    uint32_t transport_num = p_info->transport_num;
    uint32_t i = 0;

    SINGLE_TRANSPORT_INFO *p_single_info = NULL;

    for (i = 0; i < transport_num; i++)
    {
        p_single_info = &p_info->transport_infos[i];

        if (memcmp(p_transport->bd_addr, p_single_info->bd_addr, sizeof(p_transport->bd_addr)) == 0)
        {
            break;
        }
    }

    if (i == transport_num)
    {
        app_ama_transport_delete(p_transport->bd_addr);
    }

    return true;
}


static void app_ama_transport_sync_transport_in_primary(AMA_TRANSPORT_REMOTE_INFO *p_info)
{
    AMA_TRANSPORT *p_transport = NULL;
    SINGLE_TRANSPORT_INFO *p_single_info = NULL;
    uint32_t transport_num = p_info->transport_num;

    for (uint32_t i = 0; i < transport_num; i++)
    {
        p_single_info = &p_info->transport_infos[i];
        p_transport = app_ama_transport_find_transport_by_bd_addr(p_single_info->bd_addr);
        if (p_transport == NULL)
        {
            p_transport = app_ama_transport_create(p_single_info->bd_addr);
        }

        for (uint32_t j = 0; j < AMA_MAX_STREAM; j++)
        {
            if (p_single_info->streams[j] != NULL)
            {
                app_ama_transport_stream_create((AMA_STREAM_TYPE)j, p_single_info->bd_addr, p_single_info->conn_id);
            }
            else if (p_single_info->streams[j] == NULL)
            {
                app_ama_transport_stream_delete((AMA_STREAM_TYPE)j, p_single_info->bd_addr, p_single_info->conn_id);
            }
        }

        app_ama_transport_select_active_stream(p_transport, p_single_info->active_stream_type);

        APP_PRINT_TRACE2("app_ama_transport_sync_to_primary: bd_addr %s active_stream_type %d",
                         TRACE_BDADDR(p_single_info->bd_addr), p_transport->active_stream_type);
    }
}


static void app_ama_transport_sync_transport_not_in_primary(AMA_TRANSPORT_REMOTE_INFO *p_info)
{
    vector_mapping(transport_mgr.transport_vector,
                   (V_MAPPER) app_ama_transport_delete_not_in_primary, p_info);
}


static void app_ama_transport_sync_to_primary(REMOTE_MSG_TYPE msg_type,
                                              AMA_TRANSPORT_REMOTE_INFO *p_info)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }

    switch (msg_type)
    {
    case TRANSPORT_CREATED:
        {
            if (p_info->transport_num == 1)
            {
                app_ama_transport_create(p_info->transport_infos[0].bd_addr);
            }
        }
        break;

    case TRANSPORT_DELETED:
        {
            if (p_info->transport_num == 1)
            {
                app_ama_transport_delete(p_info->transport_infos[0].bd_addr);
            }
        }
        break;

    case STREAM_CREATED:
        {
            if (p_info->transport_num == 1)
            {
                app_ama_transport_stream_create(p_info->transport_infos[0].active_stream_type,
                                                p_info->transport_infos[0].bd_addr, p_info->transport_infos[0].conn_id);
            }
        }
        break;

    case STREAM_DELETED:
        {
            if (p_info->transport_num == 1)
            {
                app_ama_transport_stream_delete(p_info->transport_infos[0].active_stream_type,
                                                p_info->transport_infos[0].bd_addr, p_info->transport_infos[0].conn_id);
            }
        }

    default:
        break;
    }
}


static void app_ama_transport_send_info_to_sec(AMA_TRANSPORT_REMOTE_INFO *p_info,
                                               REMOTE_MSG_TYPE msg_type)
{
    APP_PRINT_TRACE3("app_ama_transport_send_info_to_sec: msg_type %d, len %d, data %b", msg_type,
                     sizeof(*p_info), TRACE_BINARY(sizeof(*p_info), p_info));

    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AMA_TRANSPORT, msg_type,
                                        (uint8_t *)p_info,
                                        sizeof(*p_info));
}


static bool app_ama_transport_prepare_single_transport_roleswap_info(AMA_TRANSPORT *p_transport,
                                                                     AMA_TRANSPORT_REMOTE_INFO *p_info)
{
    uint32_t transport_idx = p_info->transport_num;
    SINGLE_TRANSPORT_INFO *p_single_info = &p_info->transport_infos[transport_idx];

    memcpy(p_single_info->bd_addr, p_transport->bd_addr, sizeof(p_single_info->bd_addr));
    p_single_info->active_stream_type = p_transport->active_stream_type;
    p_single_info->streams[AMA_SPP_STREAM] = p_transport->streams[AMA_SPP_STREAM];
    p_single_info->streams[AMA_IAP_STREAM] = p_transport->streams[AMA_IAP_STREAM];
    p_single_info->streams[AMA_BLE_STREAM] = p_transport->streams[AMA_BLE_STREAM];

    if (p_transport->streams[AMA_BLE_STREAM] != NULL)
    {
        p_single_info->conn_id = ((BLE_STREAM)p_transport->streams[AMA_BLE_STREAM])->conn_id;
    }
    else
    {
        p_single_info->conn_id = AMA_STREAM_INVALID_CONN_ID;
    }

    p_info->transport_num++;

    APP_PRINT_TRACE4("app_ama_transport_prepare_single_transport_roleswap_info: transport_num %d, bd_addr %s, active_stream_type %d, conn_id %d",
                     p_info->transport_num, TRACE_BDADDR(p_single_info->bd_addr), p_single_info->active_stream_type,
                     p_single_info->conn_id);


    return true;
}


int32_t app_ama_transport_calc_pure_transport_len(uint32_t transport_num)
{
    return SIZE_ALIGN4((int32_t)(offsetof(AMA_TRANSPORT_REMOTE_INFO, transport_infos)
                                 + transport_num * sizeof(SINGLE_TRANSPORT_INFO)));
}


int32_t app_ama_transport_get_info_len(void)
{
    int32_t transport_info_len = 0;

    uint32_t transport_num = vector_depth(transport_mgr.transport_vector);

    transport_info_len = app_ama_transport_calc_pure_transport_len(transport_num);

    return transport_info_len;
}


void app_ama_transport_get_info(uint8_t *buf)
{
    AMA_TRANSPORT_REMOTE_INFO *p_info = (AMA_TRANSPORT_REMOTE_INFO *)buf;
    int32_t transport_info_len = app_ama_transport_get_info_len();
    uint32_t transport_num = vector_depth(transport_mgr.transport_vector);


    p_info->transport_num = 0;
    memset(p_info->transport_infos, 0,
           sizeof(p_info->transport_infos[0]) * transport_num); //extra one info
    vector_mapping(transport_mgr.transport_vector,
                   (V_MAPPER)app_ama_transport_prepare_single_transport_roleswap_info,
                   p_info);

    APP_PRINT_TRACE2("app_ama_transport_get_info: len %d, data %b", transport_info_len,
                     TRACE_BINARY(transport_info_len, buf));
}


int32_t app_ama_transport_set_info(uint8_t *buf)
{
    AMA_TRANSPORT_REMOTE_INFO *p_info = (AMA_TRANSPORT_REMOTE_INFO *)buf;
    int32_t transport_info_len = 0;

    transport_info_len = app_ama_transport_calc_pure_transport_len(p_info->transport_num);

    APP_PRINT_TRACE3("app_ama_transport_set_info: transport_num %d, len %b, data %b",
                     p_info->transport_num, transport_info_len, TRACE_BINARY(transport_info_len, p_info));

    app_ama_transport_sync_transport_in_primary(p_info);
    app_ama_transport_sync_transport_not_in_primary(p_info);

    return transport_info_len;
}

#if F_APP_ERWS_SUPPORT
static void app_ama_transport_parse(REMOTE_MSG_TYPE msg_type, uint8_t *buf, uint16_t len,
                                    T_REMOTE_RELAY_STATUS status)
{
    if (status != REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        return;
    }

    APP_PRINT_TRACE3("app_ama_transport_parse: msg_type %d, len %d, data %b", msg_type, len,
                     TRACE_BINARY(len, buf));

    AMA_TRANSPORT_REMOTE_INFO info = {0};

    memcpy_s(&info, sizeof(AMA_TRANSPORT_REMOTE_INFO), buf, len);

    app_ama_transport_sync_to_primary(msg_type, &info);
}
#endif

bool app_ama_transport_supported(AMA_STREAM_TYPE stream_type)
{
    bool ret = false;
    if (extend_app_cfg_const.ama_tranport & (0x1 << stream_type))
    {
        ret = true;
    }

    return ret;
}


static bool app_ama_transport_stream_connected(AMA_TRANSPORT *p_transport,
                                               AMA_STREAM_TYPE stream_type)
{
    if (p_transport->streams[stream_type] == NULL)
    {
        return false;
    }

    if (p_transport->streams[stream_type]->is_connected == false)
    {
        return false;
    }

    return true;
}


static void app_ama_transport_update_streams(AMA_TRANSPORT *p_transport, uint8_t *bd_addr)
{
    if (p_transport == NULL)
    {
        return;
    }

    p_transport->streams[AMA_SPP_STREAM] = (COMMON_STREAM)spp_stream_find_by_server_chann(
                                               RFC_SPP_AMA_CHANN_NUM,
                                               bd_addr);
    p_transport->streams[AMA_IAP_STREAM] = (COMMON_STREAM)iap_stream_find_by_ea_protocol_id(
                                               EA_PROTOCOL_ID_ALEXA,
                                               bd_addr);

    p_transport->streams[AMA_BLE_STREAM] = (COMMON_STREAM)ble_stream_search_by_addr(
                                               transport_mgr.gatt_service_id, bd_addr, AMA_TX_DATA_INDEX, AMA_RX_DATA_INDEX);
    p_transport->conn_id = ((BLE_STREAM)(p_transport->streams[AMA_BLE_STREAM]))->conn_id;

    APP_PRINT_TRACE4("app_ama_transport_update_streams: spp_stream 0x%08x, iap_stream 0x%08x, ble_stream 0x%08x, conn_id %d",
                     p_transport->streams[AMA_SPP_STREAM], p_transport->streams[AMA_IAP_STREAM],
                     p_transport->streams[AMA_BLE_STREAM], p_transport->conn_id);

    if (p_transport->active_stream_type == AMA_MAX_STREAM)
    {
        if (app_ama_transport_stream_connected(p_transport, AMA_SPP_STREAM) == false &&
            app_ama_transport_stream_connected(p_transport, AMA_IAP_STREAM) == false)
        {
            if (p_transport->streams[AMA_BLE_STREAM] != NULL)
            {
                APP_PRINT_TRACE0("app_ama_transport_update_streams: only le connected, set it as active stream");
                app_ama_transport_select_active_stream(p_transport, AMA_BLE_STREAM);
            }
        }
    }
}


static AMA_STREAM_TYPE app_ama_transport_get_stream_type_by_stream(AMA_TRANSPORT *p_transport,
                                                                   COMMON_STREAM stream)
{
    uint32_t i = 0;


    for (i = 0; i < AMA_MAX_STREAM; i++)
    {
        if (stream == p_transport->streams[i])
        {
            break;
        }
    }

    if (i == AMA_MAX_STREAM)
    {
        APP_PRINT_ERROR0("app_ama_transport_get_stream_type_by_stream: not found this stream, update transport and try again");
        app_ama_transport_update_streams(p_transport, stream->bd_addr);
        for (i = 0; i < AMA_MAX_STREAM; i++)
        {
            if (stream == p_transport->streams[i])
            {
                break;
            }
        }
    }

    return (AMA_STREAM_TYPE)i;
}


bool app_ama_transport_avaiable(uint8_t *bd_addr)
{
    AMA_TRANSPORT *p_transport = app_ama_transport_find_transport_by_bd_addr(bd_addr);

    if (p_transport && (p_transport->active_stream_type != AMA_MAX_STREAM))
    {
        return true;
    }
    else
    {
        return false;
    }
}


static void app_ama_transport_default(AMA_TRANSPORT *p)
{
    p->active_stream_type = AMA_MAX_STREAM;
    for (uint32_t i = 0; i < AMA_MAX_STREAM; i++)
    {
        p->streams[i] = NULL;
    }

    p->conn_id = AMA_STREAM_INVALID_CONN_ID;

    p->transport_connected = false;
}


static void app_ama_transport_send_delete_or_create_info_to_sec(uint8_t *bd_addr,
                                                                REMOTE_MSG_TYPE msg_type)
{
    AMA_TRANSPORT_REMOTE_INFO info = {0};
    info.transport_num = 1;
    memcpy(info.transport_infos[0].bd_addr, bd_addr, sizeof(info.transport_infos[0].bd_addr));
    app_ama_transport_send_info_to_sec(&info, msg_type);
}


static void __app_ama_transport_delete(uint8_t bd_addr[BLUETOOTH_ADDR_LENGTH],
                                       bool send_remote_info)
{
    APP_PRINT_TRACE1("app_ama_transport_delete: bd_addr %s", TRACE_BDADDR(bd_addr));

    AMA_TRANSPORT *p_transport = app_ama_transport_find_transport_by_bd_addr(bd_addr);
    if (p_transport == NULL)
    {
        APP_PRINT_ERROR0("app_ama_transport_delete: cannot found transport by this bd_addr ");
        return;
    }

    if (send_remote_info)
    {
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            app_ama_transport_send_delete_or_create_info_to_sec(bd_addr, TRANSPORT_DELETED);
        }
    }

    app_ama_transport_unregister(p_transport);

    ama_mem_free(p_transport);
}


void app_ama_transport_delete(uint8_t bd_addr[BLUETOOTH_ADDR_LENGTH])
{
    __app_ama_transport_delete(bd_addr, true);
}


bool app_ama_transport_delete_for_temp_ble(void)
{
    APP_PRINT_TRACE0("app_ama_transport_delete_for_temp_ble");

    __app_ama_transport_delete((uint8_t *)COMMON_STREAM_TEMP_ADDR, false);

    return true;
}


static AMA_TRANSPORT *__app_ama_transport_create(uint8_t bd_addr[BLUETOOTH_ADDR_LENGTH],
                                                 bool send_remote_info)
{
    bool ret = false;
    AMA_TRANSPORT *p_transport = NULL;

    APP_PRINT_TRACE1("__app_ama_transport_create: bd_addr %s", TRACE_BDADDR(bd_addr));

    p_transport = app_ama_transport_find_transport_by_bd_addr(bd_addr);
    if (p_transport != NULL)
    {
        goto UPDATE_STREAMS;
    }

    p_transport = ama_mem_alloc(AMA_NONVOLATILE_MEMORY, sizeof(*p_transport));
    if (p_transport == NULL)
    {
        return NULL;
    }

    app_ama_transport_default(p_transport);

    memcpy(p_transport->bd_addr, bd_addr, sizeof(p_transport->bd_addr));

    ret = app_ama_transport_register(p_transport);
    if (ret != true)
    {
        APP_PRINT_ERROR0("__app_ama_transport_create: transport register failed");
        ama_mem_free(p_transport);
        p_transport = NULL;
    }

    if (send_remote_info)
    {
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            app_ama_transport_send_delete_or_create_info_to_sec(bd_addr, TRANSPORT_CREATED);
        }
    }

UPDATE_STREAMS:
    app_ama_transport_update_streams(p_transport, bd_addr);

    return p_transport;
}


AMA_TRANSPORT *app_ama_transport_create(uint8_t bd_addr[BLUETOOTH_ADDR_LENGTH])
{
    return __app_ama_transport_create(bd_addr, true);
}


static AMA_TRANSPORT *app_ama_transport_create_for_temp_ble(COMMON_STREAM stream)
{
    AMA_TRANSPORT *p_transport = NULL;

    APP_PRINT_TRACE0("app_ama_transport_create_for_temp_ble");

    p_transport = __app_ama_transport_create((uint8_t *)COMMON_STREAM_TEMP_ADDR, false);
    p_transport->streams[AMA_BLE_STREAM] = stream;

    return p_transport;
}


void app_ama_transport_tx(AMA_TRANSPORT *p_transport, uint8_t *data, uint16_t length)
{
    COMMON_STREAM stream = NULL;

    if (p_transport->active_stream_type >= AMA_MAX_STREAM)
    {
        APP_PRINT_ERROR0("app_ama_transport_tx: invalid stream type");
        return;
    }

    stream = p_transport->streams[p_transport->active_stream_type];

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        common_stream_write(stream, data, length);
    }
}


static void app_ama_transport_streams_cback(COMMON_STREAM stream, COMMON_STREAM_EVENT event)
{
    AMA_TRANSPORT *p_transport = NULL;
    AMA_STREAM_TYPE stream_type = AMA_MAX_STREAM;

    APP_PRINT_TRACE2("app_ama_transport_streams_cback: bd_addr %s, bt_type %d",
                     TRACE_BDADDR(stream->bd_addr), stream->bt_type);

    p_transport = app_ama_transport_find_transport_by_bd_addr(stream->bd_addr);
    if (p_transport != NULL)
    {
        stream_type = app_ama_transport_get_stream_type_by_stream(p_transport, stream);
        if (stream_type == AMA_MAX_STREAM)
        {
            APP_PRINT_ERROR0("app_ama_transport_streams_cback: this transport have no such stream and try to update streams");
            return;
        }
    }
    else
    {
        if (stream->bt_type == COMMON_STREAM_BT_LE)
        {
            stream_type = AMA_BLE_STREAM;
            APP_PRINT_TRACE0("app_ama_transport_streams_cback: no transport before ble auth, need a temp transport");
            p_transport = app_ama_transport_create_for_temp_ble(stream);

        }
    }

    if (p_transport == NULL)
    {
        return;
    }

    APP_PRINT_TRACE2("app_ama_transport_streams_cback: COMMON_STREAM_EVENT %d, stream_type %d", event,
                     stream_type);

    switch (event)
    {
    case COMMON_STREAM_CONNECTED_EVENT:
        {
            APP_PRINT_TRACE1("app_ama_transport_streams_cback: stream_type %d", stream_type);

            if (p_transport->active_stream_type == AMA_MAX_STREAM
                || p_transport->active_stream_type == AMA_BLE_STREAM)
            {
                app_ama_transport_select_active_stream(p_transport, stream_type);
                ama_process_event_handler(p_transport->bd_addr, AMA_EVENT_CONNECTED, NULL, 0);
                APP_PRINT_TRACE0("app_ama_transport_streams_cback: stream connected");
                p_transport->transport_connected = true;
                AMA_TRANSPORT_CB_PARAM param = {.bd_addr = p_transport->bd_addr};
                transport_mgr.cb(AMA_TRANSPORT_CONNECTED_EVT, &param);
            }
        }
        break;

    case COMMON_STREAM_DISCONNECTED_EVENT:
        {
            APP_PRINT_TRACE1("app_ama_transport_streams_cback: stream_type %d", stream_type);

            if (stream_type == p_transport->active_stream_type)
            {
                app_ama_reset(p_transport->bd_addr);
                app_ama_transport_select_active_stream(p_transport, AMA_MAX_STREAM);
                ama_process_event_handler(NULL, AMA_EVENT_DISCONNECTED, NULL, 0);
                APP_PRINT_ERROR0("app_ama_transport_streams_cback: no active stream now");
                p_transport->transport_connected = false;
                AMA_TRANSPORT_CB_PARAM param = {.bd_addr = p_transport->bd_addr};
                transport_mgr.cb(AMA_TRANSPORT_DISCONNECTED_EVT, &param);
            }
        }
        break;

    case COMMON_STREAM_READY_TO_READ_EVENT:
        {
            AMA_TRANSPORT_CB_PARAM param = {0};
            param.bd_addr = stream->bd_addr;
            common_stream_read(stream, &param.buf, &param.length);

            if (p_transport != NULL)
            {
                if (stream_type == p_transport->active_stream_type)
                {
                    transport_mgr.cb(AMA_TRANSPORT_DATA_RECEIVED, &param);
                }
            }
        }
        break;

    case COMMON_STREAM_READY_TO_WRITE_EVENT:
        break;

    default:
        break;
    }
}


bool app_ama_transport_init(AMA_TRANSPORT_CB cb)
{
    app_ama_spp_init();
    transport_mgr.gatt_service_id = app_ama_ble_init();

    APP_PRINT_TRACE1("app_ama_transport_init: gatt_service_id %d", transport_mgr.gatt_service_id);

    transport_mgr.transport_vector = vector_create(MAX_AMA_TRANSPORTS_NUM);
    if (transport_mgr.transport_vector == NULL)
    {
        return false;
    }

    if (TRANSPORT_BIT_LE != 0x1 << AMA_BLE_STREAM
        || TRANSPORT_BIT_SPP != 0x1 << AMA_SPP_STREAM
        || TRANSPORT_BIT_IAP != 0x1 << AMA_IAP_STREAM)
    {
        APP_PRINT_ERROR0("app_ama_transport_init: transport supported setting wrong");
        return false;
    }

    transport_mgr.cb = cb;
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(NULL, app_ama_transport_parse, APP_MODULE_TYPE_AMA_TRANSPORT,
                             REMOTE_MSG_TYPE_MAX);
#endif
    return true;
}


static void app_ama_transport_stream_create_delete_send_info_to_sec(AMA_STREAM_TYPE stream_type,
                                                                    uint8_t *pub_addr, uint8_t conn_id, REMOTE_MSG_TYPE msg_type)
{
    AMA_TRANSPORT_REMOTE_INFO info = {0};

    info.transport_num = 1;
    info.transport_infos[0].active_stream_type = stream_type;
    memcpy(info.transport_infos[0].bd_addr, pub_addr, sizeof(info.transport_infos[0].bd_addr));

    if (conn_id != AMA_STREAM_INVALID_CONN_ID)
    {
        info.transport_infos[0].conn_id = conn_id;
    }

    app_ama_transport_send_info_to_sec(&info, msg_type);
}


COMMON_STREAM app_ama_transport_stream_create(AMA_STREAM_TYPE stream_type, uint8_t *pub_addr,
                                              uint8_t conn_id)
{
    COMMON_STREAM stream = NULL;

    APP_PRINT_TRACE3("app_ama_transport_stream_create, stream_type %d, bd_addr %s, conn_id %d",
                     stream_type, TRACE_BDADDR(pub_addr), conn_id);

    switch (stream_type)
    {
    case AMA_SPP_STREAM:
        if (app_ama_transport_supported(AMA_SPP_STREAM))
        {
            SPP_INIT_SETTINGS spp_settings = {0};
            spp_settings.bd_addr = pub_addr;
            spp_settings.server_chann = RFC_SPP_AMA_CHANN_NUM;
            spp_settings.stream_cb = app_ama_transport_streams_cback;
            stream = (COMMON_STREAM)spp_stream_create(&spp_settings);
        }
        break;

    case AMA_IAP_STREAM:
        if (app_ama_transport_supported(AMA_IAP_STREAM))
        {
            IAP_INIT_SETTINGS iap_settings = {0};
            iap_settings.bd_addr = pub_addr;
            iap_settings.ea_protocol_id = EA_PROTOCOL_ID_ALEXA;
            iap_settings.stream_cb = app_ama_transport_streams_cback;
            stream = (COMMON_STREAM)iap_stream_create(&iap_settings);
        }
        break;

    case AMA_BLE_STREAM:
        if (app_ama_transport_supported(AMA_BLE_STREAM))
        {
            BLE_INIT_SETTINGS ble_settings = {0};
            ble_settings.conn_id = conn_id;
            ble_settings.service_id = transport_mgr.gatt_service_id;
            //tx is client write to server, rx is service write to client
            ble_settings.read_attr_idx = AMA_TX_DATA_INDEX;
            ble_settings.read_pdu_type = GATT_PDU_TYPE_ANY;
            ble_settings.write_attr_idx = AMA_RX_DATA_INDEX;
            ble_settings.write_pdu_type = GATT_PDU_TYPE_NOTIFICATION;
            ble_settings.stream_cb = app_ama_transport_streams_cback;
            stream = (COMMON_STREAM)ble_stream_create(&ble_settings);
        }
        break;

    default:
        break;
    }

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        app_ama_transport_stream_create_delete_send_info_to_sec(stream_type, pub_addr, conn_id,
                                                                STREAM_CREATED);
    }

    return stream;
}


bool app_ama_transport_stream_delete(AMA_STREAM_TYPE stream_type, uint8_t *pub_addr,
                                     uint8_t conn_id)
{
    COMMON_STREAM stream = NULL;
    AMA_TRANSPORT *p_transport = NULL;

    APP_PRINT_TRACE3("app_ama_transport_stream_delete: stream_type %d, bd_addr %s, conn_id %d",
                     stream_type, TRACE_BDADDR(pub_addr), conn_id);

    switch (stream_type)
    {
    case AMA_SPP_STREAM:
        stream = (COMMON_STREAM)spp_stream_find_by_server_chann(RFC_SPP_AMA_CHANN_NUM, pub_addr);
        p_transport = app_ama_transport_find_transport_by_bd_addr(pub_addr);
        break;

    case AMA_IAP_STREAM:
        stream = (COMMON_STREAM)iap_stream_find_by_ea_protocol_id(EA_PROTOCOL_ID_ALEXA, pub_addr);
        p_transport = app_ama_transport_find_transport_by_bd_addr(pub_addr);
        break;

    case AMA_BLE_STREAM:
        stream = (COMMON_STREAM)ble_stream_search_by_id(transport_mgr.gatt_service_id, conn_id,
                                                        AMA_TX_DATA_INDEX,
                                                        AMA_RX_DATA_INDEX);
        p_transport = app_ama_transport_find_transport_by_conn_id(conn_id);
        break;

    default:
        break;
    }

    common_stream_delete(stream);

    if (p_transport == NULL)
    {
        APP_PRINT_ERROR0("app_ama_transport_stream_delete: p_transport is NULL");
        return false;
    }

    app_ama_transport_update_streams(p_transport, pub_addr);

    if (p_transport->active_stream_type == stream_type)
    {
        app_ama_transport_select_active_stream(p_transport, AMA_MAX_STREAM);
    }

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        app_ama_transport_stream_create_delete_send_info_to_sec(stream_type, pub_addr, conn_id,
                                                                STREAM_DELETED);
    }

    return true;
}


static uint8_t app_ama_transport_convert_profile_type(T_AMA_PROFILE profile)
{
    uint8_t profile_mask = 0;

    switch (profile)
    {
    case AMA_A2DP_PROFILE:
        {
            profile_mask = A2DP_PROFILE_MASK;
        }
        break;

    case AMA_HFP_PROFILE:
        {
            profile_mask = HFP_PROFILE_MASK;
        }
        break;

    default:
        break;
    }

    return profile_mask;
}


static bool app_ama_transport_get_bluetooth_classic_addr(uint8_t *local_bt_addr)
{
    if (gap_get_param(GAP_PARAM_BD_ADDR, local_bt_addr) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}


static bool app_ama_transport_check_bluetooth_classic_connected(AMA_TRANSPORT *p_transport)
{
    return p_transport->streams[AMA_SPP_STREAM]->is_connected;
}


static bool app_ama_transport_check_profile_connected(AMA_TRANSPORT *p_transport,
                                                      T_AMA_PROFILE profile)
{
    uint8_t profile_mask = app_ama_transport_convert_profile_type(profile);

    if (profile_mask)
    {
        return true;
    }
    else
    {
        return false;
    }
}


static bool app_ama_transport_check_transport_bluetooth_classic_discoverable(
    AMA_TRANSPORT *p_transport)
{
    if (1)
    {
        return true;
    }
    else
    {
        return false;
    }
}


T_AMA_BT_INFO  app_ama_transport_get_bt_info(uint8_t *bd_addr)
{
    T_AMA_BT_INFO bt_info = {0};

    AMA_TRANSPORT *p_transport = NULL;
    p_transport = app_ama_transport_find_transport_by_bd_addr(bd_addr);

    if (p_transport == NULL)
    {
        return bt_info;
    }

    app_ama_transport_get_bluetooth_classic_addr(bt_info.local_bd_addr);
    bt_info.is_bt_connected = app_ama_transport_check_bluetooth_classic_connected(p_transport);
    bt_info.is_a2dp_connected = app_ama_transport_check_profile_connected(p_transport,
                                                                          AMA_A2DP_PROFILE);
    bt_info.is_hfp_connected = app_ama_transport_check_profile_connected(p_transport, AMA_HFP_PROFILE);
    bt_info.is_bt_discoverable = app_ama_transport_check_transport_bluetooth_classic_discoverable(
                                     p_transport);
    return bt_info;
}


uint32_t app_ama_transport_get_mtu(AMA_TRANSPORT *p_transport)
{
    uint32_t mtu = 0;

    if (p_transport == NULL)
    {
        APP_PRINT_ERROR0("app_ama_transport_get_mtu: p_transport is NULL");
        return 0;
    }

    if (p_transport->active_stream_type < AMA_MAX_STREAM)
    {
        mtu = common_stream_get_mtu(p_transport->streams[p_transport->active_stream_type]);
        APP_PRINT_TRACE2("app_ama_transport_get_mtu: active stream type %d, mtu %d",
                         p_transport->active_stream_type, mtu);
        return mtu;
    }
    else
    {
        return 0;
    }
}


#endif
