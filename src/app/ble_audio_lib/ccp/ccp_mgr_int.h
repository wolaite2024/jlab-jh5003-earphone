
/* Define to prevent recursive inclusion */
#ifndef _CCP_MGR_INT_H_
#define _CCP_MGR_INT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ccp_mgr.h"

#define LE_AUDIO_MSG_CCP_WRITE_RPT_INT     (LE_AUDIO_MSG_GROUP_CCP_SERVER | 0x21)

typedef struct
{
    uint16_t     char_uuid;

    union
    {
        T_CCP_SERVER_BEARER_PROVIDER_NAME               bearer_provider_name;
        T_CCP_SERVER_BEARER_TECHNOLOGY                  bearer_technology;
        T_CCP_SERVER_BEARER_URI_SCHEMES_SUPPORTED_LIST  bearer_uri_schemes_supported_list;
        uint8_t                                         bearer_signal_strength;
        uint16_t                                        status_flags;
    } param;
} T_CCP_SERVER_SEND_DATA_PARAM;

typedef struct
{
    uint16_t        conn_handle;
    uint8_t         service_id;
    uint8_t         opcode;
    uint8_t         call_idx;
    uint8_t         result;
} T_CCP_OP_RES;

typedef struct
{
    uint16_t conn_handle;
    uint16_t cid;
    uint8_t service_id;
    uint8_t rpt_int;
} T_CCP_WRITE_RPT_INT;

void ccp_update_call_list_and_state(uint8_t service_id);
void ccp_handle_op_result(T_CCP_OP_RES res);
bool ccp_server_send_data(uint8_t service_id, T_CCP_SERVER_SEND_DATA_PARAM *p_param);

void ccp_write_post_proc(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                         uint16_t attrib_index, uint16_t length, uint8_t *p_value);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _CCP_MGR_INT_H_ */
