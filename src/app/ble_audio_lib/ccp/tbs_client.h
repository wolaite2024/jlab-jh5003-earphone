#ifndef _TBS_CLIENT_INT_H_
#define _TBS_CLIENT_INT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "bt_gatt_client.h"

#if LE_AUDIO_CCP_CLIENT_SUPPORT
typedef struct
{
    uint8_t srv_num;
} T_TBS_CLIENT_DB;

typedef struct
{
    bool        gtbs;
    uint16_t    uuid;
    uint8_t     *p_value;
    uint16_t    len;
} T_TBS_CLI_WRITE_DATA;


void tbs_cfg_cccd(uint16_t conn_handle, uint8_t srv_instance_id, uint32_t cfg_flags, bool enable,
                  bool gtbs);
bool tbs_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, uint16_t uuid, bool gtbs,
                         P_FUN_GATT_CLIENT_CB p_req_cb);

bool tbs_write_char_value(uint16_t conn_handle, uint8_t srv_instance_id, T_GATT_WRITE_TYPE type,
                          T_TBS_CLI_WRITE_DATA *p_data);
bool tbs_client_init(P_FUN_GATT_CLIENT_CB app_cb, bool gerneral);
#endif
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
