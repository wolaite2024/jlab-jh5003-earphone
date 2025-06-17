#ifndef _MCP_CLIENT_INT_H_
#define _MCP_CLIENT_INT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "mcp_client.h"

#define LE_AUDIO_MCP_CLIENT_SUPPORT_INTERNAL (LE_AUDIO_MCP_CLIENT_SUPPORT && 0)

#define LE_AUDIO_MSG_MCS_CLIENT_READ_OBJ_IDS      (LE_AUDIO_MSG_GROUP_MCP_CLIENT | 0x21)
#define LE_AUDIO_MSG_MCS_CLIENT_READ_OBJ_RES      (LE_AUDIO_MSG_GROUP_MCP_CLIENT | 0x22)

#define MCP_CLIENT_CFG_CCCD_FLAG_PLAYBACK_SPEED                          0x00000100
#define MCP_CLIENT_CFG_CCCD_FLAG_SEEKING_SPEED                           0x00000200
#define MCP_CLIENT_CFG_CCCD_FLAG_CURRENT_TRACK_OBJECT_ID                 0x00000400
#define MCP_CLIENT_CFG_CCCD_FLAG_NEXT_TRACK_OBJECT_ID                    0x00000800
#define MCP_CLIENT_CFG_CCCD_FLAG_PARENT_GROUP_OBJECT_ID                  0x00001000
#define MCP_CLIENT_CFG_CCCD_FLAG_CURRENT_GROUP_OBJECT_ID                 0x00002000
#define MCP_CLIENT_CFG_CCCD_FLAG_PLAYING_ORDER                           0x00004000
#define MCP_CLIENT_CFG_CCCD_FLAG_SEARCH_CONTROL_POINT                    0x00008000
#define MCP_CLIENT_CFG_CCCD_FLAG_SEARCH_RESULTS_OBJECT_ID                0x00010000

typedef struct
{
    uint8_t  *p_icon_url;     //UTF-8 string
    uint16_t  url_len;
} T_CL_MPIURL_CB;

typedef struct
{
    uint16_t    conn_handle;
    bool        gmcs;
    uint8_t     srv_instance_id;
    uint16_t    cause;

    uint64_t    cur_tk_obj_id;
    uint64_t    cur_tk_seg_obj_id;
    uint64_t    next_tk_obj_id;
    uint64_t    cur_gp_obj_id;
    uint64_t    paraent_gp_obj_id;
} T_MCS_CLIENT_READ_OBJ_IDS;

typedef struct
{
    uint8_t     type;
    uint8_t     *p_value;
    uint16_t    value_len;
} T_MCS_CLIENT_SCP_PARAM;

typedef struct
{
    uint16_t    conn_handle;
    bool        gmcs;
    bool        is_success;
    uint8_t     srv_instance_id;
    uint64_t    obj_id;
    uint16_t    uuid;

    uint8_t     *p_value;
    uint16_t    len;
} T_MCS_CLIENT_READ_OBJ_RES;

typedef void (*P_MCS_SCP_CB)(uint8_t result);

bool mcs_set_playback_speed(uint16_t conn_handle, uint8_t srv_instance_id, bool is_cmd,
                            bool gmcs,
                            int8_t playback_speed);

bool mcs_set_playing_order(uint16_t conn_handle, uint8_t srv_instance_id, bool is_cmd,
                           bool gmcs,
                           uint8_t order);

bool mcs_set_abs_track_pos(uint16_t conn_handle, uint8_t srv_instance_id, bool is_cmd,
                           bool gmcs,
                           int32_t pos);

bool mcs_read_obj_value(uint16_t conn_handle, uint8_t srv_instance_id, bool gmcs,
                        uint16_t uuid,
                        uint64_t obj_id);

bool mcs_read_relevant_tk_obj_id(uint16_t conn_handle, uint8_t srv_instance_id, bool gmcs);

bool mcs_send_scp_op(uint16_t conn_handle, uint8_t srv_instance_id, bool gmcs,
                     T_MCS_CLIENT_SCP_PARAM *p_param, uint8_t param_num,
                     P_MCS_SCP_CB p_scp_cb);

bool mcs_set_cur_tk_obj_id(uint16_t conn_handle, uint8_t srv_instance_id, bool is_cmd,
                           bool gmcs, uint64_t obj_id);

bool mcs_set_next_tk_obj_id(uint16_t conn_handle, uint8_t srv_instance_id, bool is_cmd,
                            bool gmcs, uint64_t obj_id);

bool mcs_set_cur_gp_obj_id(uint16_t conn_handle, uint8_t srv_instance_id, bool is_cmd,
                           bool gmcs, uint64_t obj_id);

uint8_t mcs_get_ots_idx(uint16_t conn_handle, uint8_t srv_instance_id, bool gmcs);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
