/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     ots.h
  * @brief    Head file for Object Transfer Service.
  * @details  mcs data structs and external functions declaration.
  * @author   cheng_cai
  * @date
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _OTS_SERVER_H_
#define _OTS_SERVER_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "bt_gatt_svc.h"
#include "ots_comm.h"
#include "os_queue.h"
#include "gap_credit_based_conn.h"
#if LE_AUDIO_MCP_SERVER_SUPPORT
#define OTS_OLF_CHAR_SUPPORT              0
#define OTS_OBJ_IND_CHANGE_SUPPORT        1
#define OTS_SPECIAL_OBJS_SUPPORT          1


#define OTS_MAX_CONCURRENCY_NUM           3
#define OTS_OBJ_ID_ARRAY_MAX              2

#define OTS_OBJ_LOCK_IDLE_STATE           0
#define OTS_OBJ_LOCK_READ_STATE           1
#define OTS_OBJ_LOCK_WRITE_PATCH_STATE    2
#define OTS_OBJ_LOCK_WRITE_APPEND_STATE   3

#define OTS_OACP_CRAETE_DEFAULT_PROPS       0x0F

#define OTS_OBJ_CHG_CHAR_VAL_LEN          7

typedef struct
{
    uint16_t char_index;
} T_OACP_CB;

typedef struct
{
    uint16_t char_index;
} T_OLCP_CB;

typedef struct
{
    uint8_t   filt_type;
    uint8_t   *p_param;
    uint16_t  param_len;
} T_OLF_CB;

typedef struct t_ots_cl_cb
{
    struct t_ots_cl_cb  *p_next;
    uint16_t          conn_handle;
    T_SERVER_ID       service_id;
    uint16_t          mtu;
    uint16_t          cid;
    T_OLF_CB          olf_cb[3];
    T_OBJECT_CB      *current_obj;
} T_OTS_CL_CB;


typedef struct
{
    bool          used;
    T_SERVER_ID   service_id;
    uint16_t      char_msk;
    uint16_t      attr_feature;
    uint16_t      attr_num;
    uint16_t      olf_attr_idx[3];
    T_ATTRIB_APPL *p_attr_tbl;
    T_OS_QUEUE    obj_queue;
    uint32_t      ots_features[2];
    T_OBJECT_CB  *dir_list_obj;

    uint32_t      id_array[OTS_OBJ_ID_ARRAY_MAX];

    T_OACP_CB     oacp_cb;
    T_OLCP_CB     olcp_cb;
    uint16_t      obj_change_index;
} T_OTS_SERV_CB;

T_OTS_SERV_CB *ots_get_attr_tbl(T_SERVER_ID service_id);

#endif /* LE_AUDIO_MCP_SERVER_SUPPORT */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _OTS_SERVER_H_ */
