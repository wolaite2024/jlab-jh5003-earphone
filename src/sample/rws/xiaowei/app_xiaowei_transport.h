#ifndef _APP_XIAOWEI_TRANSPORTS_H_
#define _APP_XIAOWEI_TRANSPORTS_H_

//#include "xiaowei_api.h"
#include "stdint.h"
#include "stdbool.h"
#include "xiaowei_protocol.h"

typedef struct
{
    uint8_t used;
    uint8_t bd_addr[6];
    T_XIAOWEI_CONNECTION_TYPE communication_way;//T_XIAOWEI_CONNECTION_TYPE
    uint8_t conn_id;
    uint8_t service_id;
    uint8_t cmd_sn;
    uint16_t mtu;
    T_XIAOWEI_OS_TYPE phone_type;
    //uint32_t virtual_addr;
    bool auth_result;
    //uint8_t auth_state;
    //uint8_t rand[17];
} T_XIAOWEI_LINK;

typedef struct _XIAOWEI_TRANSPORT
{
    uint8_t         bd_addr[6];
    T_XIAOWEI_CONNECTION_TYPE active_stream_type;
    T_XIAOWEI_LINK   *stream_for_spp;
    T_XIAOWEI_LINK   *stream_for_ble;
} T_XIAOWEI_TRANSPORT;

typedef struct
{
    uint8_t service_id;
    uint8_t cmd_sn;//cmd_sn
    uint16_t mtu;
    T_XIAOWEI_OS_TYPE phone_type;//T_XM_CMD_PHONE_INFO_TYPE
    //uint32_t virtual_addr;
    bool auth_result;
    //uint8_t auth_state;
} T_XIAOWEI_LINK_REMOTE_INFO;

typedef struct
{
    uint8_t bd_addr[6];
    T_XIAOWEI_LINK_REMOTE_INFO bt_link[4];
} T_XIAOWEI_SINGLE_TRANSPORT_INFO;

typedef struct
{
    uint8_t transport_num;
    T_XIAOWEI_SINGLE_TRANSPORT_INFO transport_infos[1]; //the only one is used for created and deleted
} XIAOWEI_TRANSPORT_REMOTE_INFO;

/**  @brief app xiaowei remote msg*/
typedef enum
{
    XIAOWEI_TRANSPORT_CREATED           = 0x00,
    XIAOWEI_TRANSPORT_DELETED           = 0x01,
    XIAOWEI_TRANSPORT_ROLESWAP          = 0x02, //this evt is used for sync active stream when roleswap
    XIAOWEI_TRANSPORT_MAX_MSG_NUM       = 0x03,
} T_XIAOWEI_TRANSPORT_REMOTE_MSG_TYPE;

T_XIAOWEI_CONNECTION_TYPE app_xiaowei_get_active_stream_type(T_XIAOWEI_TRANSPORT *p_transport);
void app_xiaowei_select_active_stream(T_XIAOWEI_TRANSPORT *p_transport,
                                      T_XIAOWEI_CONNECTION_TYPE select_transport);
bool app_xiaowei_transport_init(void);

T_XIAOWEI_TRANSPORT *app_xiaowei_transport_create(T_XIAOWEI_CONNECTION_TYPE bt_type,
                                                  uint8_t bd_addr[6],
                                                  uint8_t conn_id);
void app_xiaowei_transport_delete(T_XIAOWEI_CONNECTION_TYPE bt_type, uint8_t bd_addr[6]);

void app_xiaowei_transport_send_roleswap_info_all(XIAOWEI_TRANSPORT_REMOTE_INFO *p_info);
void app_xiaowei_transport_send_roleswap_info(void);

bool app_xiaowei_over_ble_supported(void);
bool app_xiaowei_over_spp_supported(void);
T_XIAOWEI_TRANSPORT *app_xiaowei_find_transport_by_bd_addr(uint8_t bd_addr[6]);
T_XIAOWEI_LINK *app_xiaowei_find_link(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr);
T_XIAOWEI_LINK *app_xiaowei_find_br_link(uint8_t *bd_addr);
T_XIAOWEI_LINK *app_xiaowei_alloc_br_link(uint8_t *bd_addr);
bool app_xiaowei_free_br_link(uint8_t *bd_addr);
T_XIAOWEI_LINK *app_xiaowei_find_le_link_by_addr(uint8_t *bd_addr);
T_XIAOWEI_LINK *app_xiaowei_alloc_le_link_by_conn_id(uint8_t conn_id, uint8_t *bd_addr);
bool app_xiaowei_free_le_link(uint8_t conn_id);
bool xiaowei_set_access_auth_result(T_XIAOWEI_CONNECTION_TYPE comm_way, uint8_t *bd_addr,
                                    uint8_t conn_id,
                                    bool result);
#endif

