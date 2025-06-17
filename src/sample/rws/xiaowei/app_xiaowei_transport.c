#if F_APP_XIAOWEI_FEATURE_SUPPORT
#include "stdint.h"

#include "string.h"
#include "trace.h"
#include "vector.h"
#include "remote.h"
#include "app_cfg.h"
#include "xiaowei_protocol.h"
#include "app_xiaowei_transport.h"
#include "stdlib.h"
#include "app_relay.h"
#define MAX_XIAOWEI_TRANSPORTS_NUM      (4)

typedef struct
{
    VECTOR transport_vector;
} T_XIAOWEI_TRANSPORT_MGR;

static T_XIAOWEI_TRANSPORT_MGR  transport_mgr = {.transport_vector = NULL};

T_XIAOWEI_LINK *app_xiaowei_find_link(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr)
{
    T_XIAOWEI_LINK *p_link = NULL;
    T_XIAOWEI_TRANSPORT *p_transport = app_xiaowei_find_transport_by_bd_addr(bd_addr);
    if (p_transport)
    {
        if (type == XIAOWEI_CONNECTION_TYPE_SPP)
        {
            p_link = p_transport->stream_for_spp;
        }
        else if (type == XIAOWEI_CONNECTION_TYPE_BLE)
        {
            p_link = p_transport->stream_for_ble;
        }
    }
    APP_PRINT_INFO0("app_xiaowei_find_link: p_link %d", p_link);
    return p_link;
}

T_XIAOWEI_LINK *app_xiaowei_find_br_link(uint8_t *bd_addr)
{
    T_XIAOWEI_LINK *p_link = NULL;
    T_XIAOWEI_TRANSPORT *p_transport = app_xiaowei_find_transport_by_bd_addr(bd_addr);
    if (p_transport)
    {
        p_link = p_transport->stream_for_spp;
    }
    return p_link;
}

T_XIAOWEI_LINK *app_xiaowei_alloc_br_link(uint8_t *bd_addr)
{
    T_XIAOWEI_LINK *p_link = calloc(1, sizeof(T_XIAOWEI_LINK));

    if (p_link != NULL)
    {
        p_link->used = true;
        p_link->communication_way = XIAOWEI_CONNECTION_TYPE_SPP;
        memcpy(p_link->bd_addr, bd_addr, 6);
    }

    return p_link;
}

bool app_xiaowei_free_br_link(uint8_t *bd_addr)
{

    return false;
}

T_XIAOWEI_LINK *app_xiaowei_find_le_link_by_addr(uint8_t *bd_addr)
{
    T_XIAOWEI_LINK *p_link = NULL;
    T_XIAOWEI_TRANSPORT *p_transport = app_xiaowei_find_transport_by_bd_addr(bd_addr);
    if (p_transport)
    {
        p_link = p_transport->stream_for_ble;
    }
    APP_PRINT_INFO0("app_xiaowei_find_le_link_by_addr: p_link %d", p_link);
    return p_link;
}

T_XIAOWEI_LINK *app_xiaowei_alloc_le_link_by_conn_id(uint8_t conn_id, uint8_t *bd_addr)
{
    T_XIAOWEI_LINK *p_link = calloc(1, sizeof(T_XIAOWEI_LINK));

    if (p_link)
    {
        p_link->used    = true;
        p_link->conn_id = conn_id;
        p_link->communication_way = XIAOWEI_CONNECTION_TYPE_BLE;
        memcpy(p_link->bd_addr, bd_addr, sizeof(p_link->bd_addr));
    }

    return p_link;
}

static bool transport_register(T_XIAOWEI_TRANSPORT *p_transport)
{
    bool ret = false;

    ret = vector_add(transport_mgr.transport_vector, p_transport);
    if (ret == false)
    {
        APP_PRINT_ERROR0("transport_register: register failed");
    }

    return ret;
}

static bool transport_unregister(T_XIAOWEI_TRANSPORT *p_transport)
{
    bool ret = false;

    ret = vector_remove(transport_mgr.transport_vector, p_transport);
    if (ret == false)
    {
        APP_PRINT_ERROR0("transport_unregister: unregister failed");
    }

    return ret;
}

static bool match_transport_by_bd_addr(T_XIAOWEI_TRANSPORT *p_transport, uint8_t *bd_addr)
{
    if (p_transport == NULL)
    {
        return false;
    }

    if (memcmp(p_transport->bd_addr, bd_addr, sizeof(p_transport->bd_addr)) != 0)
    {
        return false;
    }

    return true;
}

T_XIAOWEI_TRANSPORT *app_xiaowei_find_transport_by_bd_addr(uint8_t bd_addr[6])
{
    T_XIAOWEI_TRANSPORT *p_transport = NULL;

    p_transport = vector_search(transport_mgr.transport_vector,
                                (V_PREDICATE)match_transport_by_bd_addr, bd_addr);

    return p_transport;
}

bool app_xiaowei_over_ble_supported(void)
{
    bool ret = false;
    if (extend_app_cfg_const.xiaowei_tranport & TRANSPORT_BIT_LE)
    {
        ret = true;
    }

    return ret;
}

bool app_xiaowei_over_spp_supported(void)
{
    bool ret = false;
    if (extend_app_cfg_const.xiaowei_tranport & TRANSPORT_BIT_SPP)
    {
        ret = true;
    }

    return ret;
}

void app_xiaowei_select_active_stream(T_XIAOWEI_TRANSPORT *p_transport,
                                      T_XIAOWEI_CONNECTION_TYPE select_transport)
{
    APP_PRINT_TRACE1("app_xiaowei_select_active_stream: p_transport select_transport %d",
                     select_transport);

    switch (select_transport)
    {
    case XIAOWEI_CONNECTION_TYPE_SPP:
        {
            if (p_transport->stream_for_spp->used == true)
            {
                p_transport->active_stream_type = XIAOWEI_CONNECTION_TYPE_SPP;
            }
        }
        break;

    case XIAOWEI_CONNECTION_TYPE_BLE:
        {
            if (p_transport->stream_for_ble->used == true)
            {
                p_transport->active_stream_type = XIAOWEI_CONNECTION_TYPE_BLE;
            }
        }
        break;

    case XIAOWEI_CONNECTION_TYPE_IAP:
        {
//            if (p_transport->stream_for_spp->used == true)
//            {
//                p_transport->active_stream_type = XIAOWEI_CONNECTION_TYPE_SPP;
//            }
        }
        break;

    case XIAOWEI_CONNECTION_TYPE_UNKNOWN:
        {
            p_transport->active_stream_type = XIAOWEI_CONNECTION_TYPE_BLE;
            return;
        }
    }
}

T_XIAOWEI_CONNECTION_TYPE app_xiaowei_get_active_stream_type(T_XIAOWEI_TRANSPORT *p_transport)
{
    T_XIAOWEI_CONNECTION_TYPE cur_transport = XIAOWEI_CONNECTION_TYPE_UNKNOWN;

    cur_transport = p_transport->active_stream_type;

    return cur_transport;
}

void app_xiaowei_transport_delete(T_XIAOWEI_CONNECTION_TYPE bt_type, uint8_t bd_addr[6])
{
    APP_PRINT_TRACE6("app_xiaowei_transport_delete: bd_addr %02x:%02x:%02x:%02x:%02x:%02x",
                     bd_addr[0], bd_addr[1], bd_addr[2], bd_addr[3], bd_addr[4], bd_addr[5]);

    T_XIAOWEI_TRANSPORT *p_transport = app_xiaowei_find_transport_by_bd_addr(bd_addr);
    if (p_transport == NULL)
    {
        APP_PRINT_ERROR0("app_xiaowei_transport_delete: cannot found transport by this bd_addr ");
        return;
    }

    if (app_xiaowei_over_spp_supported() && (bt_type == XIAOWEI_CONNECTION_TYPE_SPP))
    {
        if (p_transport->stream_for_spp)
        {
            //xiaowei_mem_free(p_transport->stream_for_spp, sizeof(T_XIAOWEI_LINK));
            p_transport->stream_for_spp = NULL;
        }
    }

    if (app_xiaowei_over_ble_supported() && (bt_type == XIAOWEI_CONNECTION_TYPE_BLE))
    {
        if (p_transport->stream_for_ble)
        {
            //xiaowei_mem_free(p_transport->stream_for_ble, sizeof(T_XIAOWEI_LINK));
            p_transport->stream_for_ble = NULL;
        }
    }

    if ((p_transport->stream_for_spp == NULL) && (p_transport->stream_for_ble == NULL))
    {
        transport_unregister(p_transport);
    }
}

static void transport_default(T_XIAOWEI_TRANSPORT *p)
{
    p->active_stream_type = XIAOWEI_CONNECTION_TYPE_UNKNOWN;
    p->stream_for_ble = NULL;
    p->stream_for_spp = NULL;
}

bool app_xiaowei_transport_avaiable(uint8_t *bd_addr)
{
    T_XIAOWEI_TRANSPORT *p_transport = app_xiaowei_find_transport_by_bd_addr(bd_addr);

    if (p_transport && (p_transport->active_stream_type != XIAOWEI_CONNECTION_TYPE_UNKNOWN))
    {
        return true;
    }
    else
    {
        return false;
    }
}

T_XIAOWEI_TRANSPORT *app_xiaowei_transport_create(T_XIAOWEI_CONNECTION_TYPE bt_type,
                                                  uint8_t bd_addr[6],
                                                  uint8_t conn_id)
{
    bool ret = false;
    T_XIAOWEI_TRANSPORT *p_transport = app_xiaowei_find_transport_by_bd_addr(bd_addr);

    APP_PRINT_TRACE1("app_xiaowei_transport_create: bd_addr %s", TRACE_BDADDR(bd_addr));

    if (p_transport == NULL)
    {
        p_transport = calloc(1, sizeof(*p_transport));
        if (p_transport == NULL)
        {
            APP_PRINT_ERROR0("app_xiaowei_transport_create: transports init failed");
            return NULL;
        }
        ret = transport_register(p_transport);
        if (ret != true)
        {
            free(p_transport);
            p_transport = NULL;
            return NULL;
        }

        transport_default(p_transport);
        memcpy(p_transport->bd_addr, bd_addr, sizeof(p_transport->bd_addr));
    }

    return p_transport;
}

static void xiaowei_transport_sync_to_primary(T_XIAOWEI_TRANSPORT_REMOTE_MSG_TYPE msg_type,
                                              XIAOWEI_TRANSPORT_REMOTE_INFO *p_info)
{
    APP_PRINT_TRACE1("xiaowei_transport_sync_to_primary: msg_type %d", msg_type);

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        APP_PRINT_ERROR0("xiaowei_transport_sync_to_primary: this dev should be secondary");
        return;
    }

    switch (msg_type)
    {
    case XIAOWEI_TRANSPORT_ROLESWAP:
        {
            uint32_t i = 0;
            T_XIAOWEI_TRANSPORT *p_transport = NULL;
            T_XIAOWEI_SINGLE_TRANSPORT_INFO *p_single_info = NULL;
            uint32_t transport_num = p_info->transport_num;

            for (i = 0; i < transport_num; i++)
            {
                p_single_info = &p_info->transport_infos[i];

                p_transport = app_xiaowei_find_transport_by_bd_addr(p_single_info->bd_addr);
                if (p_transport == NULL)
                {
                    continue;
                }

                if (p_transport->stream_for_ble)
                {
                    p_transport->stream_for_ble->service_id     =
                        p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_BLE].service_id;
                    p_transport->stream_for_ble->cmd_sn  =
                        p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_BLE].cmd_sn;
                    p_transport->stream_for_ble->mtu            =
                        p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_BLE].mtu;
                    p_transport->stream_for_ble->phone_type     =
                        p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_BLE].phone_type;
                    p_transport->stream_for_ble->auth_result    =
                        p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_BLE].auth_result;
                }

                if (p_transport->stream_for_spp)
                {
                    p_transport->stream_for_spp->service_id     =
                        p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_SPP].service_id;
                    p_transport->stream_for_spp->cmd_sn  =
                        p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_SPP].cmd_sn;
                    p_transport->stream_for_spp->mtu            =
                        p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_SPP].mtu;
                    p_transport->stream_for_spp->phone_type     =
                        p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_SPP].phone_type;
                    p_transport->stream_for_spp->auth_result    =
                        p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_SPP].auth_result;
                }

            }
        }
        break;

    default:
        break;
    }
}

static void xiaowei_transport_send_info_to_sec(XIAOWEI_TRANSPORT_REMOTE_INFO *p_info,
                                               T_XIAOWEI_TRANSPORT_REMOTE_MSG_TYPE msg_type)
{
    APP_PRINT_TRACE1("xiaowei_transport_send_info_to_sec: msg_type %d", msg_type);

    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_XIAOWEI_TRANSPORT, msg_type,
                                        (uint8_t *)p_info, sizeof(*p_info) + p_info->transport_num * sizeof(p_info->transport_infos[0]));
}

static bool prepare_single_transport_roleswap_info(T_XIAOWEI_TRANSPORT *p_transport,
                                                   XIAOWEI_TRANSPORT_REMOTE_INFO *p_info)
{
    uint32_t transport_idx = p_info->transport_num;
    T_XIAOWEI_SINGLE_TRANSPORT_INFO *p_single_info = &p_info->transport_infos[transport_idx];

    memcpy(p_single_info->bd_addr, p_transport->bd_addr, sizeof(p_single_info->bd_addr));

    p_info->transport_num++;

    return true;
}

void app_xiaowei_transport_send_roleswap_info(void)
{
    XIAOWEI_TRANSPORT_REMOTE_INFO *p_info = NULL;
    uint32_t transport_num = vector_depth(transport_mgr.transport_vector);
    uint32_t i = 0;
    T_XIAOWEI_TRANSPORT *p_transport = NULL;
    T_XIAOWEI_SINGLE_TRANSPORT_INFO *p_single_info = NULL;

    APP_PRINT_INFO1("app_xiaowei_transport_send_roleswap_info: transport_num %d", transport_num);

    if (transport_num == 0)
    {
        return;
    }

    p_info = calloc(1, sizeof(*p_info)
                    + transport_num * sizeof(T_XIAOWEI_SINGLE_TRANSPORT_INFO));
    if (p_info != NULL)
    {
        p_info->transport_num = 0;
        memset(p_info->transport_infos, 0,
               sizeof(p_info->transport_infos[0]) * transport_num); //extra one info
        vector_mapping(transport_mgr.transport_vector, (V_MAPPER)prepare_single_transport_roleswap_info,
                       p_info);
        if (transport_num != p_info->transport_num)
        {
            APP_PRINT_ERROR0("app_xiaowei_transport_send_roleswap_info: transport_num wrong");
        }

        for (i = 0; i < transport_num; i++)
        {
            p_single_info = &p_info->transport_infos[i];

            p_transport = app_xiaowei_find_transport_by_bd_addr(p_single_info->bd_addr);
            if (p_transport == NULL)
            {
                continue;
            }

            if (p_transport->stream_for_ble)
            {
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_BLE].service_id      =
                    p_transport->stream_for_ble->service_id;
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_BLE].cmd_sn   =
                    p_transport->stream_for_ble->cmd_sn;
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_BLE].mtu             =
                    p_transport->stream_for_ble->mtu;
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_BLE].phone_type      =
                    p_transport->stream_for_ble->phone_type;
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_BLE].auth_result     =
                    p_transport->stream_for_ble->auth_result;
            }

            if (p_transport->stream_for_spp)
            {
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_SPP].service_id      =
                    p_transport->stream_for_spp->service_id;
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_SPP].cmd_sn   =
                    p_transport->stream_for_spp->cmd_sn;
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_SPP].mtu             =
                    p_transport->stream_for_spp->mtu;
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_SPP].phone_type      =
                    p_transport->stream_for_spp->phone_type;
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_SPP].auth_result     =
                    p_transport->stream_for_spp->auth_result;
            }

        }

        xiaowei_transport_send_info_to_sec(p_info, XIAOWEI_TRANSPORT_ROLESWAP);
        free(p_info);
    }
}

void app_xiaowei_transport_send_roleswap_info_all(XIAOWEI_TRANSPORT_REMOTE_INFO *p_info)
{
    uint32_t transport_num = vector_depth(transport_mgr.transport_vector);
    uint32_t i = 0;
    T_XIAOWEI_TRANSPORT *p_transport = NULL;
    T_XIAOWEI_SINGLE_TRANSPORT_INFO *p_single_info = NULL;

    APP_PRINT_INFO1("app_xiaowei_transport_send_roleswap_info_all: transport_num %d", transport_num);

    if (transport_num == 0 || p_info == NULL)
    {
        return;
    }

    if (p_info != NULL)
    {
        p_info->transport_num = 0;
        memset(p_info->transport_infos, 0,
               sizeof(p_info->transport_infos[0]) * transport_num); //extra one info
        vector_mapping(transport_mgr.transport_vector, (V_MAPPER)prepare_single_transport_roleswap_info,
                       p_info);
        if (transport_num != p_info->transport_num)
        {
            APP_PRINT_ERROR0("app_xiaowei_transport_send_roleswap_info_all: transport_num wrong");
        }

        for (i = 0; i < transport_num; i++)
        {
            p_single_info = &p_info->transport_infos[i];

            p_transport = app_xiaowei_find_transport_by_bd_addr(p_single_info->bd_addr);
            if (p_transport == NULL)
            {
                continue;
            }

            if (p_transport->stream_for_ble)
            {
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_BLE].service_id      =
                    p_transport->stream_for_ble->service_id;
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_BLE].cmd_sn   =
                    p_transport->stream_for_ble->cmd_sn;
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_BLE].mtu             =
                    p_transport->stream_for_ble->mtu;
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_BLE].phone_type      =
                    p_transport->stream_for_ble->phone_type;
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_BLE].auth_result     =
                    p_transport->stream_for_ble->auth_result;
            }

            if (p_transport->stream_for_spp)
            {
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_SPP].service_id      =
                    p_transport->stream_for_spp->service_id;
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_SPP].cmd_sn   =
                    p_transport->stream_for_spp->cmd_sn;
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_SPP].mtu             =
                    p_transport->stream_for_spp->mtu;
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_SPP].phone_type      =
                    p_transport->stream_for_spp->phone_type;
                p_single_info->bt_link[XIAOWEI_CONNECTION_TYPE_SPP].auth_result     =
                    p_transport->stream_for_spp->auth_result;
            }

        }
    }
}

#if F_APP_ERWS_SUPPORT
static void app_xiaowei_transport_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                              T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE2("app_xiaowei_transport_parse_cback: msg_type 0x%04x, status %d", msg_type, status);
    XIAOWEI_TRANSPORT_REMOTE_INFO *p_info = (XIAOWEI_TRANSPORT_REMOTE_INFO *)buf;

    if (p_info != NULL && len != 0)
    {
        if (len != sizeof(*p_info) + sizeof(p_info->transport_infos[0]) * p_info->transport_num)
        {
            APP_PRINT_ERROR0("app_xiaowei_transport_parse_cback: wrong");
            return;
        }

        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            xiaowei_transport_sync_to_primary((T_XIAOWEI_TRANSPORT_REMOTE_MSG_TYPE)msg_type, p_info);
        }
    }
}

uint16_t app_xiaowei_transport_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;
    XIAOWEI_TRANSPORT_REMOTE_INFO *p_info = NULL;
    APP_PRINT_INFO1("app_xiaowei_transport_relay_cback: msg_type %d", msg_type);

    switch (msg_type)
    {
    case XIAOWEI_TRANSPORT_ROLESWAP:
        {
            uint32_t transport_num = vector_depth(transport_mgr.transport_vector);
            p_info = calloc(1, sizeof(*p_info)
                            + transport_num * sizeof(T_XIAOWEI_SINGLE_TRANSPORT_INFO));
            if (p_info != NULL && transport_num != 0)
            {
                app_xiaowei_transport_send_roleswap_info_all(p_info);
                payload_len = sizeof(*p_info) + p_info->transport_num * sizeof(p_info->transport_infos[0]);
                msg_ptr = (uint8_t *)p_info;
                skip = false;
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
            buf[2] = APP_MODULE_TYPE_XIAOWEI_TRANSPORT;
            buf[3] = msg_type;
            if (payload_len != 0 && msg_ptr != NULL)
            {
                memcpy(&buf[4], msg_ptr, payload_len);
            }
        }
        APP_PRINT_INFO1("app_xiaowei_transport_relay_cback: buf %b", TRACE_BINARY(msg_len, buf));
    }

    if (p_info != NULL)
    {
        free(p_info);
        p_info = NULL;
    }

    return msg_len;
}
#endif

bool app_xiaowei_transport_init()
{

    transport_mgr.transport_vector = vector_create(MAX_XIAOWEI_TRANSPORTS_NUM);
    if (transport_mgr.transport_vector == NULL)
    {
        return false;
    }
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_xiaowei_transport_relay_cback, app_xiaowei_transport_parse_cback,
                             APP_MODULE_TYPE_XIAOWEI_TRANSPORT, XIAOWEI_TRANSPORT_MAX_MSG_NUM);
#endif
    return true;
}

bool xiaowei_set_access_auth_result(T_XIAOWEI_CONNECTION_TYPE comm_way, uint8_t *bd_addr,
                                    uint8_t conn_id,
                                    bool result)
{
    bool ret = false;
    T_XIAOWEI_LINK *p_link = app_xiaowei_find_link(comm_way, bd_addr);

    if (p_link)
    {
        p_link->auth_result = result;
        ret = true;
        APP_PRINT_INFO1("xiaowei_set_access_auth_result: %d", p_link->auth_result);
    }
    else
    {
        APP_PRINT_ERROR0("xiaowei_set_access_auth_result: app_xiaowei_find_link fail");
    }

    return ret;
}
#endif
