#ifndef _APP_XIAOAI_TRANSPORTS_H_
#define _APP_XIAOAI_TRANSPORTS_H_

#if XM_XIAOAI_FEATURE_SUPPORT
#include "xm_xiaoai_api.h"



typedef struct
{
    uint8_t used;
    uint8_t bd_addr[6];
    T_XM_CMD_COMM_WAY_TYPE communication_way;
    uint8_t conn_id;
    uint8_t service_id;
    uint8_t cmd_opcode_sn;
    uint16_t mtu;
    T_XM_CMD_PHONE_INFO_TYPE phone_type;
    uint32_t virtual_addr;
    bool auth_result;
    uint8_t auth_state;
    uint8_t rand[17];
} T_XM_XIAOAI_LINK;

typedef enum
{
    TRANSPORT_ROLESWAP, //this evt is used for sync active stream when roleswap
    TRANSPORT_REMOTE_MSG_NUM        = 0x01,
} T_XIAOAI_TRANSPORT_REMOTE_MSG;

typedef struct _XIAOAI_TRANSPORT
{
    uint8_t         bd_addr[6];
    T_XM_CMD_COMM_WAY_TYPE active_stream_type;
    T_XM_XIAOAI_LINK   *stream_for_spp;
    T_XM_XIAOAI_LINK   *stream_for_ble;
} T_XIAOAI_TRANSPORT;

T_XM_CMD_COMM_WAY_TYPE app_xiaoai_get_active_stream_type(T_XIAOAI_TRANSPORT *p_transport);
void app_xiaoai_select_active_stream(T_XIAOAI_TRANSPORT *p_transport,
                                     T_XM_CMD_COMM_WAY_TYPE select_transport);
bool app_xiaoai_transport_init(void);

T_XIAOAI_TRANSPORT *app_xiaoai_transport_create(T_XM_CMD_COMM_WAY_TYPE bt_type, uint8_t bd_addr[6],
                                                uint8_t conn_id);
void app_xiaoai_transport_delete(T_XM_CMD_COMM_WAY_TYPE bt_type, uint8_t bd_addr[6]);

bool app_xiaoai_over_ble_supported(void);
bool app_xiaoai_over_spp_supported(void);
T_XIAOAI_TRANSPORT *app_xiaoai_find_transport_by_bd_addr(uint8_t bd_addr[6]);
T_XM_XIAOAI_LINK *app_xiaoai_find_link(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr);
T_XM_XIAOAI_LINK *app_xiaoai_find_br_link(uint8_t *bd_addr);
T_XM_XIAOAI_LINK *app_xiaoai_alloc_br_link(uint8_t *bd_addr);
bool app_xiaoai_free_br_link(uint8_t *bd_addr);
T_XM_XIAOAI_LINK *app_xiaoai_find_le_link_by_addr(uint8_t *bd_addr);
T_XM_XIAOAI_LINK *app_xiaoai_alloc_le_link_by_conn_id(uint8_t conn_id, uint8_t *bd_addr);
bool app_xiaoai_free_le_link(uint8_t conn_id);
#endif

#endif

