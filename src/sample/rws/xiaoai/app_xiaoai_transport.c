#if XM_XIAOAI_FEATURE_SUPPORT
#include "stdint.h"
#include "string.h"
#include "xiaoai_mem.h"
#include "trace.h"
#include "vector.h"
#include "remote.h"
#include "app_cfg.h"
#include "app_relay.h"
#include "app_xiaoai_transport.h"


#define MAX_XIAOAI_TRANSPORTS_NUM      (4)

typedef struct
{
    VECTOR transport_vector;
} T_XIAOAI_TRANSPORT_MGR;


typedef struct
{
    uint8_t service_id;
    uint8_t cmd_opcode_sn;
    uint16_t mtu;
    T_XM_CMD_PHONE_INFO_TYPE phone_type;
    uint32_t virtual_addr;
    bool auth_result;
    uint8_t auth_state;
} T_XM_XIAOAI_LINK_REMOTE_INFO;

typedef struct
{
    uint8_t bd_addr[6];
    T_XM_XIAOAI_LINK_REMOTE_INFO bt_link[2];
} SINGLE_TRANSPORT_INFO;

typedef struct
{
    uint8_t transport_num;
    SINGLE_TRANSPORT_INFO transport_infos[1]; //the only one is used for created and deleted
} XIAOAI_TRANSPORT_REMOTE_INFO;

static T_XIAOAI_TRANSPORT_MGR  transport_mgr = {.transport_vector = NULL};

T_XM_XIAOAI_LINK *app_xiaoai_find_link(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr)
{
    T_XM_XIAOAI_LINK *p_link = NULL;
    T_XIAOAI_TRANSPORT *p_transport = app_xiaoai_find_transport_by_bd_addr(bd_addr);
    if (p_transport)
    {
        if (type == T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP)
        {
            p_link = p_transport->stream_for_spp;
        }
        else if (type == T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE)
        {
            p_link = p_transport->stream_for_ble;
        }
    }
    return p_link;
}

T_XM_XIAOAI_LINK *app_xiaoai_find_br_link(uint8_t *bd_addr)
{
    T_XM_XIAOAI_LINK *p_link = NULL;
    T_XIAOAI_TRANSPORT *p_transport = app_xiaoai_find_transport_by_bd_addr(bd_addr);
    if (p_transport)
    {
        p_link = p_transport->stream_for_spp;
    }
    return p_link;
}

T_XM_XIAOAI_LINK *app_xiaoai_alloc_br_link(uint8_t *bd_addr)
{
    T_XM_XIAOAI_LINK *p_link = xiaoai_mem_zalloc(sizeof(T_XM_XIAOAI_LINK));

    if (p_link != NULL)
    {
        p_link->used = true;
        p_link->communication_way = T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP;
        memcpy(p_link->bd_addr, bd_addr, 6);
    }

    return p_link;
}

bool app_xiaoai_free_br_link(uint8_t *bd_addr)
{

    return false;
}

T_XM_XIAOAI_LINK *app_xiaoai_find_le_link_by_addr(uint8_t *bd_addr)
{
    T_XM_XIAOAI_LINK *p_link = NULL;
    T_XIAOAI_TRANSPORT *p_transport = app_xiaoai_find_transport_by_bd_addr(bd_addr);
    if (p_transport)
    {
        p_link = p_transport->stream_for_ble;
    }
    return p_link;
}

T_XM_XIAOAI_LINK *app_xiaoai_alloc_le_link_by_conn_id(uint8_t conn_id, uint8_t *bd_addr)
{
    T_XM_XIAOAI_LINK *p_link = xiaoai_mem_zalloc(sizeof(T_XM_XIAOAI_LINK));

    if (p_link)
    {
        p_link->used    = true;
        p_link->conn_id = conn_id;
        p_link->communication_way = T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE;
        memcpy(p_link->bd_addr, bd_addr, sizeof(p_link->bd_addr));
    }

    return p_link;
}

static bool transport_register(T_XIAOAI_TRANSPORT *p_transport)
{
    bool ret = false;

    ret = vector_add(transport_mgr.transport_vector, p_transport);
    if (ret == false)
    {
        APP_PRINT_ERROR0("transport_register: register failed");
    }

    return ret;
}

static bool transport_unregister(T_XIAOAI_TRANSPORT *p_transport)
{
    bool ret = false;

    ret = vector_remove(transport_mgr.transport_vector, p_transport);
    if (ret == false)
    {
        APP_PRINT_ERROR0("transport_unregister: unregister failed");
    }

    return ret;
}

static bool match_transport_by_bd_addr(T_XIAOAI_TRANSPORT *p_transport, uint8_t *bd_addr)
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

T_XIAOAI_TRANSPORT *app_xiaoai_find_transport_by_bd_addr(uint8_t bd_addr[6])
{
    T_XIAOAI_TRANSPORT *p_transport = NULL;

    p_transport = vector_search(transport_mgr.transport_vector,
                                (V_PREDICATE)match_transport_by_bd_addr, bd_addr);

    return p_transport;
}

bool app_xiaoai_over_ble_supported(void)
{
    bool ret = false;
    if (extend_app_cfg_const.xiaoai_tranport & TRANSPORT_BIT_LE)
    {
        ret = true;
    }

    return ret;
}

bool app_xiaoai_over_spp_supported(void)
{
    bool ret = false;
    if (extend_app_cfg_const.xiaoai_tranport & TRANSPORT_BIT_SPP)
    {
        ret = true;
    }

    return ret;
}

void app_xiaoai_select_active_stream(T_XIAOAI_TRANSPORT *p_transport,
                                     T_XM_CMD_COMM_WAY_TYPE select_transport)
{
    APP_PRINT_TRACE1("app_xiaoai_select_active_stream: p_transport select_transport %d",
                     select_transport);

    switch (select_transport)
    {
    case T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP:
        {
            if (p_transport->stream_for_spp->used == true)
            {
                p_transport->active_stream_type = T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP;
            }
        }
        break;

    case T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE:
        {
            if (p_transport->stream_for_ble->used == true)
            {
                p_transport->active_stream_type = T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE;
            }
        }
        break;

    case T_XM_CMD_COMMUNICATION_WAY_TYPE_INVALID:
        {
            p_transport->active_stream_type = T_XM_CMD_COMMUNICATION_WAY_TYPE_INVALID;
            return;
        }
    }
}

T_XM_CMD_COMM_WAY_TYPE app_xiaoai_get_active_stream_type(T_XIAOAI_TRANSPORT *p_transport)
{
    T_XM_CMD_COMM_WAY_TYPE cur_transport = T_XM_CMD_COMMUNICATION_WAY_TYPE_INVALID;

    cur_transport = p_transport->active_stream_type;

    return cur_transport;
}

void app_xiaoai_transport_delete(T_XM_CMD_COMM_WAY_TYPE bt_type, uint8_t bd_addr[6])
{
    APP_PRINT_TRACE6("app_xiaoai_transport_delete: bd_addr %02x:%02x:%02x:%02x:%02x:%02x",
                     bd_addr[0], bd_addr[1], bd_addr[2], bd_addr[3], bd_addr[4], bd_addr[5]);

    T_XIAOAI_TRANSPORT *p_transport = app_xiaoai_find_transport_by_bd_addr(bd_addr);
    if (p_transport == NULL)
    {
        APP_PRINT_ERROR0("app_xiaoai_transport_delete: cannot found transport by this bd_addr ");
        return;
    }

    if (app_xiaoai_over_spp_supported() && (bt_type == T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP))
    {
        if (p_transport->stream_for_spp)
        {
            xiaoai_mem_free(p_transport->stream_for_spp, sizeof(T_XM_XIAOAI_LINK));
            p_transport->stream_for_spp = NULL;
        }
    }

    if (app_xiaoai_over_ble_supported() && (bt_type == T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE))
    {
        if (p_transport->stream_for_ble)
        {
            xiaoai_mem_free(p_transport->stream_for_ble, sizeof(T_XM_XIAOAI_LINK));
            p_transport->stream_for_ble = NULL;
        }
    }

    if ((p_transport->stream_for_spp == NULL) && (p_transport->stream_for_ble == NULL))
    {
        transport_unregister(p_transport);
    }
}

static void transport_default(T_XIAOAI_TRANSPORT *p)
{
    p->active_stream_type = T_XM_CMD_COMMUNICATION_WAY_TYPE_INVALID;
    p->stream_for_ble = NULL;
    p->stream_for_spp = NULL;
}

bool app_xiaoai_transport_avaiable(uint8_t *bd_addr)
{
    T_XIAOAI_TRANSPORT *p_transport = app_xiaoai_find_transport_by_bd_addr(bd_addr);

    if (p_transport && (p_transport->active_stream_type != T_XM_CMD_COMMUNICATION_WAY_TYPE_INVALID))
    {
        return true;
    }
    else
    {
        return false;
    }
}

T_XIAOAI_TRANSPORT *app_xiaoai_transport_create(T_XM_CMD_COMM_WAY_TYPE bt_type, uint8_t bd_addr[6],
                                                uint8_t conn_id)
{
    bool ret = false;
    T_XIAOAI_TRANSPORT *p_transport = app_xiaoai_find_transport_by_bd_addr(bd_addr);

    APP_PRINT_TRACE1("app_xiaoai_transport_create: bd_addr %s", TRACE_BDADDR(bd_addr));

    if (p_transport == NULL)
    {
        p_transport = xiaoai_mem_zalloc(sizeof(*p_transport));
        if (p_transport == NULL)
        {
            APP_PRINT_ERROR0("app_xiaoai_transport_create: transports init failed");
            return NULL;
        }
        ret = transport_register(p_transport);
        if (ret != true)
        {
            os_mem_free(p_transport);
            p_transport = NULL;
            return NULL;
        }

        transport_default(p_transport);
        memcpy(p_transport->bd_addr, bd_addr, sizeof(p_transport->bd_addr));
    }

    return p_transport;
}

static void xiaoai_transport_sync_to_primary(T_XIAOAI_TRANSPORT_REMOTE_MSG msg_type,
                                             XIAOAI_TRANSPORT_REMOTE_INFO *p_info)
{
    APP_PRINT_TRACE1("xiaoai_transport_sync_to_primary: msg_type %d", msg_type);

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        APP_PRINT_ERROR0("xiaoai_transport_sync_to_primary: this dev should be secondary");
        return;
    }

    switch (msg_type)
    {
    case TRANSPORT_ROLESWAP:
        {
            uint32_t i = 0;
            T_XIAOAI_TRANSPORT *p_transport = NULL;
            SINGLE_TRANSPORT_INFO *p_single_info = NULL;
            uint32_t transport_num = p_info->transport_num;

            for (i = 0; i < transport_num; i++)
            {
                p_single_info = &p_info->transport_infos[i];

                p_transport = app_xiaoai_find_transport_by_bd_addr(p_single_info->bd_addr);
                if (p_transport == NULL)
                {
                    continue;
                }

                if (p_transport->stream_for_ble)
                {
                    p_transport->stream_for_ble->service_id     =
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE].service_id;
                    p_transport->stream_for_ble->cmd_opcode_sn  =
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE].cmd_opcode_sn;
                    p_transport->stream_for_ble->mtu            =
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE].mtu;
                    p_transport->stream_for_ble->phone_type     =
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE].phone_type;
                    p_transport->stream_for_ble->virtual_addr   =
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE].virtual_addr;
                    p_transport->stream_for_ble->auth_result    =
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE].auth_result;
                    p_transport->stream_for_ble->auth_state     =
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE].auth_state;
                }

                if (p_transport->stream_for_spp)
                {
                    p_transport->stream_for_spp->service_id     =
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP].service_id;
                    p_transport->stream_for_spp->cmd_opcode_sn  =
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP].cmd_opcode_sn;
                    p_transport->stream_for_spp->mtu            =
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP].mtu;
                    p_transport->stream_for_spp->phone_type     =
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP].phone_type;
                    p_transport->stream_for_spp->virtual_addr   =
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP].virtual_addr;
                    p_transport->stream_for_spp->auth_result    =
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP].auth_result;
                    p_transport->stream_for_spp->auth_state     =
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP].auth_state;
                }

            }
        }
        break;

    default:
        break;
    }
}

static bool prepare_single_transport_roleswap_info(T_XIAOAI_TRANSPORT *p_transport,
                                                   XIAOAI_TRANSPORT_REMOTE_INFO *p_info)
{
    uint32_t transport_idx = p_info->transport_num;
    SINGLE_TRANSPORT_INFO *p_single_info = &p_info->transport_infos[transport_idx];

    memcpy(p_single_info->bd_addr, p_transport->bd_addr, sizeof(p_single_info->bd_addr));

    p_info->transport_num++;

    return true;
}

#if F_APP_ERWS_SUPPORT
static void app_xiaoai_transport_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                             T_REMOTE_RELAY_STATUS status)
{
    XIAOAI_TRANSPORT_REMOTE_INFO *p_info = NULL;
    uint8_t *aligned_buf = NULL;

    if ((uint32_t)buf & 0b11)
    {
        APP_PRINT_WARN0("app_xiaoai_transport_parse_cback: buf for struct should be alligned");

        aligned_buf = xiaoai_mem_zalloc(len);
        if (aligned_buf == NULL)
        {
            return;
        }

        memcpy((uint8_t *)aligned_buf, (uint8_t *)buf, len);
        p_info = (XIAOAI_TRANSPORT_REMOTE_INFO *)aligned_buf;
    }
    else
    {
        p_info = (XIAOAI_TRANSPORT_REMOTE_INFO *)buf;
    }

    if (len != sizeof(*p_info) + sizeof(p_info->transport_infos[0]) * p_info->transport_num)
    {
        APP_PRINT_ERROR0("app_xiaoai_transport_parse_cback: struct len was wrong");
        return;
    }

    if (status != REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        APP_PRINT_ERROR1("app_xiaoai_transport_parse_cback: invalid status %d", status);
        return;
    }

    APP_PRINT_TRACE0("app_xiaoai_transport_parse_cback");

    xiaoai_transport_sync_to_primary((T_XIAOAI_TRANSPORT_REMOTE_MSG)msg_type, p_info);

    if (aligned_buf)
    {
        xiaoai_mem_free(aligned_buf, len);
    }
}

uint16_t app_xiaoai_transport_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;
    APP_PRINT_INFO1("app_xiaoai_transport_relay_cback: msg_type %d", msg_type);
    XIAOAI_TRANSPORT_REMOTE_INFO *p_info = NULL;
    switch (msg_type)
    {
    case TRANSPORT_ROLESWAP:
        {
            uint32_t transport_num = vector_depth(transport_mgr.transport_vector);
            uint32_t i = 0;
            T_XIAOAI_TRANSPORT *p_transport = NULL;
            SINGLE_TRANSPORT_INFO *p_single_info = NULL;

            APP_PRINT_INFO1("app_xiaoai_transport_send_roleswap_info: transport_num %d", transport_num);

            if (transport_num == 0)
            {
                break;
            }

            p_info = xiaoai_mem_zalloc(sizeof(*p_info)
                                       + transport_num * sizeof(SINGLE_TRANSPORT_INFO));
            if (p_info != NULL)
            {
                p_info->transport_num = 0;
                memset(p_info->transport_infos, 0,
                       sizeof(p_info->transport_infos[0]) * transport_num); //extra one info
                vector_mapping(transport_mgr.transport_vector, (V_MAPPER)prepare_single_transport_roleswap_info,
                               p_info);
                if (transport_num != p_info->transport_num)
                {
                    APP_PRINT_ERROR0("app_xiaoai_transport_send_roleswap_info: transport_num wrong");
                }

                for (i = 0; i < transport_num; i++)
                {
                    p_single_info = &p_info->transport_infos[i];

                    p_transport = app_xiaoai_find_transport_by_bd_addr(p_single_info->bd_addr);
                    if (p_transport == NULL)
                    {
                        continue;
                    }

                    if (p_transport->stream_for_ble)
                    {
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE].service_id      =
                            p_transport->stream_for_ble->service_id;
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE].cmd_opcode_sn   =
                            p_transport->stream_for_ble->cmd_opcode_sn;
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE].mtu             =
                            p_transport->stream_for_ble->mtu;
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE].phone_type      =
                            p_transport->stream_for_ble->phone_type;
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE].virtual_addr    =
                            p_transport->stream_for_ble->virtual_addr;
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE].auth_result     =
                            p_transport->stream_for_ble->auth_result;
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE].auth_state      =
                            p_transport->stream_for_ble->auth_state;
                    }

                    if (p_transport->stream_for_spp)
                    {
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP].service_id      =
                            p_transport->stream_for_spp->service_id;
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP].cmd_opcode_sn   =
                            p_transport->stream_for_spp->cmd_opcode_sn;
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP].mtu             =
                            p_transport->stream_for_spp->mtu;
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP].phone_type      =
                            p_transport->stream_for_spp->phone_type;
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP].virtual_addr    =
                            p_transport->stream_for_spp->virtual_addr;
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP].auth_result     =
                            p_transport->stream_for_spp->auth_result;
                        p_single_info->bt_link[T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP].auth_state      =
                            p_transport->stream_for_spp->auth_state;
                    }

                }
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
            buf[2] = APP_MODULE_TYPE_XIAOAI_TRANSPORT;
            buf[3] = msg_type;
            if (payload_len != 0 && msg_ptr != NULL)
            {
                memcpy(&buf[4], msg_ptr, payload_len);
            }
        }
        APP_PRINT_INFO1("app_xiaoai_transport_relay_cback: buf %b", TRACE_BINARY(msg_len, buf));
    }

    if (p_info != NULL)
    {
        os_mem_free(p_info);
    }

    return msg_len;
}
#endif

bool app_xiaoai_transport_init()
{

    transport_mgr.transport_vector = vector_create(MAX_XIAOAI_TRANSPORTS_NUM);
    if (transport_mgr.transport_vector == NULL)
    {
        return false;
    }
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_xiaoai_transport_relay_cback, app_xiaoai_transport_parse_cback,
                             APP_MODULE_TYPE_XIAOAI_TRANSPORT, TRANSPORT_REMOTE_MSG_NUM);
#endif

    return true;
}
#endif
