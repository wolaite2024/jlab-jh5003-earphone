/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     mcs.h
  * @brief    Head file for Media Control Service.
  * @details  mcs data structs and external functions declaration.
  * @author   cheng_cai
  * @date
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _MCP_MGR_INT_H_
#define _MCP_MGR_INT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "mcp_def_int.h"
#include "mcp_mgr.h"
#include "ots_server.h"

#define LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL (LE_AUDIO_MCP_SERVER_SUPPORT && 0)

#define LE_AUDIO_MSG_MCS_WRITE_REQ_IND      (LE_AUDIO_MSG_GROUP_MCP_SERVER |0x22)
#define LE_AUDIO_MSG_SCP_RESULT             (LE_AUDIO_MSG_GROUP_MCP_SERVER |0x23)

//FIX TODO this function may complete in the future
#define SEARCH_RESULT_FUNC_SUPPORT                  0

#define MCS_ALL_PLAYING_ORDER_SUP       0x3FF
#define MCS_ALL_MEDIA_CTLP_OP_SUP       0x1FFFFF

typedef struct
{
    uint64_t  obj_id;
    uint16_t  char_index;
} T_CURTK_CB;

typedef struct
{
    uint64_t  obj_id;
    uint16_t  char_index;
} T_NEXTTK_CB;

typedef struct
{
    uint64_t  obj_id;
    uint16_t  char_index;
} T_PARENTGP_CB;

typedef struct
{
    uint64_t  obj_id;
    uint16_t  char_index;
} T_CURGP_CB;

typedef struct
{
    uint64_t  obj_id;
    uint16_t  char_index;
} T_SRCH_RES_CB;


typedef struct
{
    uint8_t *p_search_param;
    uint16_t search_param_len;
} T_MCS_SCP_REQ;

typedef struct
{
    bool          used;
    T_SERVER_ID   service_id;
    bool          gmcs;

    T_SERVER_ID   ots_service_id;

    uint16_t      attr_num;
    T_ATTRIB_APPL *p_attr_tbl;
    uint16_t      mcs_gp_tk_num;
    uint16_t      play_order_sup;

    uint8_t       media_state;
    uint32_t      media_control_point_opcodes_supported;
#if LE_AUDIO_OTS_SERV_SUPPORT
    uint64_t      root_grp_obj_id;
    uint64_t      ply_icon_obj_id;
    uint64_t      cur_tkseg_obj_id;
    T_CURTK_CB    cur_track_obj;
    T_NEXTTK_CB   next_track_obj;
    T_PARENTGP_CB parent_group_obj;
    T_CURGP_CB    current_group_obj;
    T_SRCH_RES_CB search_res_obj;

    T_OS_QUEUE    gp_obj_queue;
#endif
} T_MCS_CB;

typedef struct
{
    uint8_t       service_id;
    uint64_t      search_obj_id;
    T_MCS_GP_OBJ  *p_value;
    uint8_t       num;
} T_MCS_SCP_RES;


typedef struct
{
    uint8_t      seg_name_len;
    uint8_t       *p_seg_name;
    int32_t      seg_pos;
} T_MCS_TR_SEG;

typedef struct
{
    uint8_t       *p_tk_name;
    uint16_t      tk_name_len;
    uint8_t       *p_artist_name;
    uint16_t      artist_name_len;
    uint8_t       *p_alb_name;
    uint16_t      alb_name_len;
    uint8_t       *p_elst_year;
    uint16_t      elst_year_len;
    uint8_t       *p_latest_year;
    uint16_t      latest_year_len;
    uint8_t       *p_genre;
    uint16_t      genre_len;

    uint8_t       tk_seg_num;
    T_MCS_TR_SEG  *p_tk_seg;
} T_MCS_TK_ELEM;

typedef struct
{
    uint16_t      gp_name_len;
    uint8_t       *p_gp_name;
    uint8_t       *p_artist_name;
    uint16_t      artist_name_len;

    uint16_t      gp_value_len;
    uint8_t       *p_gp_value;
} T_MCS_GP_ELEM;

typedef struct
{
    uint16_t  conn_handle;
    uint8_t   service_id;
    uint16_t  uuid;
    union
    {
        int32_t             pos;
        int8_t              playback_speed;
        uint8_t             playing_order;
        uint64_t            obj_id;
    } value;
} T_MCS_WRITE_REQ;

uint64_t mcs_create_root_group(uint8_t service_id, T_MCS_GP_ELEM *p_gp_elem);
uint64_t mcs_create_major_group(uint8_t service_id, T_MCS_GP_ELEM *p_gp_elem,
                                uint64_t parent_obj_id);
uint64_t mcs_create_major_track(uint8_t service_id, T_MCS_TK_ELEM *p_tk_elem,
                                uint64_t gp_obj_id,
                                uint8_t *p_id3v2_fmt, uint16_t fmt_len);
void mcs_delete_major_gp(uint8_t service_id, uint64_t gp_obj_id);
void mcs_delete_major_track(uint8_t service_id, uint64_t gp_obj_id, uint64_t tk_obj_id);
void mcs_update_major_gp_value(uint8_t service_id, uint64_t gp_obj_id);
void mcs_make_audio_struct_complt(uint8_t service_id);
bool mcs_local_search_op(uint8_t service_id, uint8_t *p_value, uint16_t len);

uint8_t ots_add_server(uint16_t char_msk, uint16_t attr_feature);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _MCP_MGR_INT_H_ */
