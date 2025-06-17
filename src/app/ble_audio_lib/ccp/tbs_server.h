#ifndef _TBS_H_
#define _TBS_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#if LE_AUDIO_CCP_SERVER_SUPPORT
#include "bt_gatt_svc.h"
#include "ccp_mgr_int.h"
#include "os_queue.h"

typedef struct t_call_state
{
    struct t_call_state     *p_next;
    uint8_t                 call_idx;
    uint8_t                 call_state;
    uint8_t                 call_flags;
    bool                    need_notify_incoming_call;
    uint8_t                 *call_uri;
    uint16_t                uri_len;
} T_CALL_STATE;

typedef struct
{
    bool                        used;
    T_SERVER_ID                 service_id;
    bool                        gtbs;
    uint16_t                    attr_num;
    uint8_t                     cur_call_idx;
    uint8_t                     incoming_call_idx;
    uint16_t                    call_control_point_optional_opcodes;
    T_ATTRIB_APPL               *p_attr_tbl;
    T_CCP_SERVER_INCOMING_CALL_TARGET_BEARER_URI        in_call_tg_uri_cb;
    T_CCP_SERVER_CALL_FRIENDLY_NAME         call_fri_name_cb;
    T_OS_QUEUE                  call_state_queue;
} T_TBS_CB;

T_CALL_STATE *tbs_get_call_state(T_TBS_CB *p_entry, uint8_t call_idx);
uint16_t tbs_calculate_call_list_size(T_TBS_CB *p_entry);
bool tbs_send_notify(T_TBS_CB *p_entry, uint16_t conn_handle, uint16_t attrib_idx,
                     uint8_t *p_data, uint16_t data_len);
T_TBS_CB *tbs_find_by_srv_id(T_SERVER_ID service_id);
T_SERVER_ID tbs_add_service(T_TBS_CB *p_entry, T_CCP_SERVER_REG_SRV_PARAM *p_param);

#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
