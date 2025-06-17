/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     mcp_client.c
* @brief    Media Control service source file.
* @details  Interfaces to access Media Control service.
* @author   cheng_cai
* @date
* @version  v1.0
*********************************************************************************************************
*/
#include <string.h>
#include "trace.h"
#include "ble_audio_mgr.h"
#include "ble_link_util.h"
#include "ble_audio.h"
#include "bt_gatt_client.h"
#include "ble_audio_dm.h"
#include "mcp_client_int.h"
#include "mcp_def_int.h"
#include "ots_def.h"
#include "ots_client.h"

#if LE_AUDIO_MCP_CLIENT_SUPPORT
typedef struct
{
    bool              has_ots;
    uint8_t           ots_idx;

    uint64_t          cur_tk_obj_id;
    uint64_t          cur_tk_seg_obj_id;
    uint64_t          next_tk_obj_id;
    uint64_t          cur_gp_obj_id;
    uint64_t          paraent_gp_obj_id;
    uint64_t          search_res_obj_id;

    P_MCS_SCP_CB      p_scp_cb;
} T_MCS_CLT_CB;

typedef struct
{
    uint16_t            conn_handle;
    uint8_t             srv_num;
    uint8_t             read_mcs_srv_idx;
    bool                general_read;
    uint16_t            read_obj_uuid;
    uint64_t            read_obj_id;

    T_MCS_CLT_CB        *p_gmcs_client;
    T_MCS_CLT_CB        *p_mcs_clients;
} T_MCS_CLIENT_DB;

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
static T_MCS_CLIENT_DB *mcs_check_link_int(uint16_t conn_handle)
{
    T_BLE_AUDIO_LINK *p_link = NULL;

    p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL || p_link->p_mcs_client == NULL)
    {
        PROTOCOL_PRINT_ERROR1("mcs_check_link_int failed, conn_handle 0x%x", conn_handle);
        return NULL;
    }
    return (T_MCS_CLIENT_DB *)p_link->p_mcs_client;
}

#if LE_AUDIO_OTS_CLIENT_SUPPORT
static void mcs_read_obj_value_cb(uint16_t conn_handle, uint8_t opcode, uint8_t result,
                                  uint8_t *p_value,
                                  uint16_t len)
{
    T_MCS_CLIENT_READ_OBJ_RES obj_val;
    T_MCS_CLIENT_DB *p_client = mcs_check_link_int(conn_handle);

    if (p_client == NULL)
    {
        return;
    }

    obj_val.conn_handle = conn_handle;
    obj_val.gmcs = p_client->general_read;
    obj_val.srv_instance_id = p_client->read_mcs_srv_idx;
    obj_val.is_success = result == OACP_SUCCESS_RES_CODE ? true : false;
    obj_val.obj_id = p_client->read_obj_id;
    obj_val.uuid = p_client->read_obj_uuid;
    obj_val.p_value = p_value;
    obj_val.len = len;

    p_client->general_read = false;
    p_client->read_obj_id = 0;
    p_client->read_obj_uuid = 0;
    p_client->read_mcs_srv_idx = 0;

    ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCS_CLIENT_READ_OBJ_RES, &obj_val);
}

static void mcs_ots_goto_op_cb(T_OTS_CLT_OLCP_IND *p_olcp_res)
{
    T_MCS_CLIENT_READ_OBJ_RES obj_val;
    T_MCS_CLIENT_DB *p_client = mcs_check_link_int(p_olcp_res->conn_handle);

    if (p_client == NULL)
    {
        return;
    }

    obj_val.conn_handle = p_olcp_res->conn_handle;
    obj_val.gmcs = p_client->general_read;
    obj_val.srv_instance_id = p_client->read_mcs_srv_idx;
    obj_val.is_success = false;
    obj_val.obj_id = p_client->read_obj_id;
    obj_val.uuid = p_client->read_obj_uuid;

    if (p_olcp_res->olcp_opcode == OTS_OLCP_GOTO_OP)
    {
        if (p_olcp_res->ind_res != OLCP_SUCCESS_RES_CODE)
        {
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCS_CLIENT_READ_OBJ_RES, &obj_val);
            p_client->general_read = false;
            p_client->read_obj_id = 0;
            p_client->read_obj_uuid = 0;
            p_client->read_mcs_srv_idx = 0;
        }
        else
        {
            T_OTS_OACP_PARAM param;
            memset((uint8_t *)&param, 0, sizeof(T_OTS_OACP_PARAM));
            param.opcode = OTS_OACP_READ_OP;
            param.p_oacp_cb = mcs_read_obj_value_cb;

            if (!ots_client_oacp_op(p_olcp_res->conn_handle, p_olcp_res->instance_id, &param))
            {
                ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCS_CLIENT_READ_OBJ_RES, &obj_val);
                p_client->general_read = false;
                p_client->read_obj_id = 0;
                p_client->read_obj_uuid = 0;
                p_client->read_mcs_srv_idx = 0;
            }
        }
    }
}
#endif
static void mcs_handle_charac_notify(uint16_t conn_handle, bool gmcs, void *p_data)
{
    T_GATT_CLIENT_DATA *p = (T_GATT_CLIENT_DATA *) p_data;
    T_GATT_CLIENT_NOTIFY_IND *nofiy_result = &p->notify_ind;
    T_MCS_CLIENT_DB *p_client = mcs_check_link_int(conn_handle);
    T_MCP_CLIENT_NOTIFY notify_res;
    T_MCS_CLT_CB  *p_mcs_client = NULL;
    uint16_t len = nofiy_result->value_size;

    if ((p_client == NULL) || (((gmcs) && (p_client->p_gmcs_client == NULL)) ||
                               ((!gmcs) &&
                                (p_client->p_mcs_clients == NULL))))
    {
        return;
    }

    p_mcs_client = gmcs == true ? p_client->p_gmcs_client : (p_client->p_mcs_clients +
                                                             nofiy_result->srv_instance_id);

    notify_res.conn_handle = conn_handle;
    notify_res.gmcs = gmcs;
    notify_res.srv_instance_id = nofiy_result->srv_instance_id;
    notify_res.char_uuid = nofiy_result->char_uuid.p.uuid16;

    switch (nofiy_result->char_uuid.p.uuid16)
    {
    case MCS_UUID_CHAR_MEDIA_PLAYER_NAME :
        notify_res.data.media_player_name.p_media_player_name = nofiy_result->p_value;
        notify_res.data.media_player_name.media_player_name_len = nofiy_result->value_size;
        break;

    case MCS_UUID_CHAR_TRACK_CHANGED :
        break;

    case MCS_UUID_CHAR_TRACK_TITLE :
        notify_res.data.track_title.p_track_title = nofiy_result->p_value;
        notify_res.data.track_title.track_title_len = nofiy_result->value_size;
        break;

    case MCS_UUID_CHAR_TRACK_DURATION :
        if (len != 4)
        {
            PROTOCOL_PRINT_ERROR2("mcs_handle_charac_notify: uuid : 0x%x, wrong length: %d",
                                  nofiy_result->char_uuid.p.uuid16, len);
            return;
        }

        LE_ARRAY_TO_UINT32(notify_res.data.track_duration, nofiy_result->p_value);
        break;

    case MCS_UUID_CHAR_TRACK_POSITION :
        if (len != 4)
        {
            PROTOCOL_PRINT_ERROR2("mcs_handle_charac_notify: uuid : 0x%x, wrong length: %d",
                                  nofiy_result->char_uuid.p.uuid16, len);
            return;
        }

        LE_ARRAY_TO_UINT32(notify_res.data.track_position, nofiy_result->p_value);
        break;

    case MCS_UUID_CHAR_PLAYBACK_SPEED :
        if (len != 1)
        {
            PROTOCOL_PRINT_ERROR2("mcs_handle_charac_notify: uuid : 0x%x, wrong length: %d",
                                  nofiy_result->char_uuid.p.uuid16, len);
            return;
        }

        notify_res.data.playback_speed = (int8_t)(*(nofiy_result->p_value));
        break;

    case MCS_UUID_CHAR_SEEKING_SPEED :
        if (len != 1)
        {
            PROTOCOL_PRINT_ERROR2("mcs_handle_charac_notify: uuid : 0x%x, wrong length: %d",
                                  nofiy_result->char_uuid.p.uuid16, len);
            return;
        }

        notify_res.data.seeking_speed = (int8_t)(*(nofiy_result->p_value));
        break;

    case MCS_UUID_CHAR_PLAYING_ORDER :
        if (len != 1)
        {
            PROTOCOL_PRINT_ERROR2("mcs_handle_charac_notify: uuid : 0x%x, wrong length: %d",
                                  nofiy_result->char_uuid.p.uuid16, len);
            return;
        }

        notify_res.data.playing_order = *(nofiy_result->p_value);
        break;

    case MCS_UUID_CHAR_MEDIA_STATE :
        if (len != 1)
        {
            PROTOCOL_PRINT_ERROR2("mcs_handle_charac_notify: uuid : 0x%x, wrong length: %d",
                                  nofiy_result->char_uuid.p.uuid16, len);
            return;
        }
        notify_res.data.media_state = nofiy_result->p_value[0];
        break;

    case MCS_UUID_CHAR_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED :
        if (len != MCS_MEDIA_CTRL_POINT_OPCODES_SUP_ATTR_VALUE_LEN)
        {
            PROTOCOL_PRINT_ERROR2("mcs_handle_charac_notify: uuid : 0x%x, wrong length: %d",
                                  nofiy_result->char_uuid.p.uuid16, len);
            return;
        }

        LE_ARRAY_TO_UINT32(notify_res.data.media_control_point_opcodes_supported, nofiy_result->p_value);
        break;

#if LE_AUDIO_MCP_CLIENT_SUPPORT_INTERNAL
    case MCS_UUID_CHAR_CURRENT_TRACK_OBJECT_ID :
        if (len != OTS_OBJ_ID_LEN)
        {
            PROTOCOL_PRINT_ERROR2("mcs_handle_charac_notify: uuid : 0x%x, wrong length: %d",
                                  nofiy_result->char_uuid.p.uuid16, len);
            return;
        }

        memcpy((uint8_t *)&notify_res.data.cur_tk_obj_id, nofiy_result->p_value, len);
        break;
    case MCS_UUID_CHAR_NEXT_TRACK_OBJECT_ID :
        if (len != OTS_OBJ_ID_LEN)
        {
            PROTOCOL_PRINT_ERROR2("mcs_handle_charac_notify: uuid : 0x%x, wrong length: %d",
                                  nofiy_result->char_uuid.p.uuid16, len);
            return;
        }
        memcpy((uint8_t *)&notify_res.data.next_tk_obj_id, nofiy_result->p_value, len);
        break;
    case MCS_UUID_CHAR_PARENT_GROUP_OBJECT_ID :
        if (len != OTS_OBJ_ID_LEN)
        {
            PROTOCOL_PRINT_ERROR2("mcs_handle_charac_notify: uuid : 0x%x, wrong length: %d",
                                  nofiy_result->char_uuid.p.uuid16, len);
            return;
        }
        memcpy((uint8_t *)&notify_res.data.paraent_gp_obj_id, nofiy_result->p_value, len);
        break;
    case MCS_UUID_CHAR_CURRENT_GROUP_OBJECT_ID :
        if (len != OTS_OBJ_ID_LEN)
        {
            PROTOCOL_PRINT_ERROR2("mcs_handle_charac_notify: uuid : 0x%x, wrong length: %d",
                                  nofiy_result->char_uuid.p.uuid16, len);
            return;
        }
        memcpy((uint8_t *)&notify_res.data.cur_gp_obj_id, nofiy_result->p_value, len);
        break;
#endif

    case MCS_UUID_CHAR_MEDIA_CONTROL_POINT:
        {
            T_MCP_CLIENT_MEDIA_CP_NOTIFY cp_notify = {0};

            cp_notify.conn_handle = conn_handle;
            cp_notify.gmcs = gmcs;
            cp_notify.srv_instance_id = nofiy_result->srv_instance_id;
            cp_notify.requested_opcode = nofiy_result->p_value[0];
            cp_notify.result_code = nofiy_result->p_value[1];

            ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCP_CLIENT_MEDIA_CP_NOTIFY, &cp_notify);
        }
        return;

    case MCS_UUID_CHAR_SEARCH_CONTROL_POINT:
        if (p_mcs_client->p_scp_cb)
        {
            p_mcs_client->p_scp_cb(nofiy_result->p_value[0]);
        }
        p_mcs_client->p_scp_cb = NULL;
        return;

#if LE_AUDIO_MCP_CLIENT_SUPPORT_INTERNAL
    case MCS_UUID_CHAR_SEARCH_RESULTS_OBJECT_ID:
        if (len != OTS_OBJ_ID_LEN)
        {
            PROTOCOL_PRINT_ERROR2("mcs_handle_charac_notify: uuid : 0x%x, wrong length: %d",
                                  nofiy_result->char_uuid.p.uuid16, len);
            return;
        }
        else
        {
            memcpy((uint8_t *)&p_mcs_client->search_res_obj_id, nofiy_result->p_value, OTS_OBJ_ID_LEN);
            memcpy((uint8_t *)&notify_res.data.search_res_obj_id, nofiy_result->p_value, OTS_OBJ_ID_LEN);
        }
        break;
#endif

    default:
        return;
    }
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCP_CLIENT_NOTIFY, &notify_res);
}

static T_APP_RESULT gmcs_read_relevant_tk_obj_id_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                                    void *p_data)
{
    T_GATT_CLIENT_DATA *p = (T_GATT_CLIENT_DATA *) p_data;
    T_GATT_CLIENT_READ_RESULT *read_result = &p->read_result;
    T_MCS_CLIENT_DB *mcs_client_cb = mcs_check_link_int(conn_handle);
    T_APP_RESULT  res = APP_RESULT_SUCCESS;
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    uint16_t handle = 0;

    if ((mcs_client_cb == NULL) || (mcs_client_cb->p_gmcs_client == NULL))
    {
        return APP_RESULT_APP_ERR;
    }

    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = read_result->srv_instance_id;
    srv_uuid.p.uuid16 = GATT_UUID_GENERIC_MEDIA_CONTROL_SERVICE;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = 0;

    if (read_result->cause != GATT_SUCCESS)
    {
        PROTOCOL_PRINT_ERROR1("gmcs_read_relevant_tk_obj_id_cb: error: %d", read_result->cause);
        res = APP_RESULT_APP_ERR;
    }

    if (read_result->value_size != OTS_OBJ_ID_LEN)
    {
        PROTOCOL_PRINT_ERROR1("gmcs_read_relevant_tk_obj_id_cb: wrong length: %d", read_result->value_size);
        res = APP_RESULT_APP_ERR;
    }

    switch (read_result->char_uuid.p.uuid16)
    {
    case MCS_UUID_CHAR_CURRENT_TRACK_OBJECT_ID:
        {
            if (res == APP_RESULT_SUCCESS)
            {
                memcpy((uint8_t *)&mcs_client_cb->p_gmcs_client->cur_tk_obj_id, read_result->p_value,
                       OTS_OBJ_ID_LEN);
            }
            char_uuid.p.uuid16 = MCS_UUID_CHAR_CURRENT_TRACK_SEGMENTS_OBJECT_ID;
        }
        break;

    case MCS_UUID_CHAR_CURRENT_TRACK_SEGMENTS_OBJECT_ID:
        {
            if (res == APP_RESULT_SUCCESS)
            {
                memcpy((uint8_t *)&mcs_client_cb->p_gmcs_client->cur_tk_seg_obj_id, read_result->p_value,
                       OTS_OBJ_ID_LEN);
            }
            char_uuid.p.uuid16 = MCS_UUID_CHAR_NEXT_TRACK_OBJECT_ID;
        }
        break;

    case MCS_UUID_CHAR_NEXT_TRACK_OBJECT_ID:
        {
            if (res == APP_RESULT_SUCCESS)
            {
                memcpy((uint8_t *)&mcs_client_cb->p_gmcs_client->next_tk_obj_id, read_result->p_value,
                       OTS_OBJ_ID_LEN);
            }
            char_uuid.p.uuid16 = MCS_UUID_CHAR_CURRENT_GROUP_OBJECT_ID;
        }
        break;

    case MCS_UUID_CHAR_CURRENT_GROUP_OBJECT_ID:
        {
            if (res == APP_RESULT_SUCCESS)
            {
                memcpy((uint8_t *)&mcs_client_cb->p_gmcs_client->cur_gp_obj_id, read_result->p_value,
                       OTS_OBJ_ID_LEN);
            }
            char_uuid.p.uuid16 = MCS_UUID_CHAR_PARENT_GROUP_OBJECT_ID;
        }
        break;

    case MCS_UUID_CHAR_PARENT_GROUP_OBJECT_ID:
        {
            if (res == APP_RESULT_SUCCESS)
            {
                memcpy((uint8_t *)&mcs_client_cb->p_gmcs_client->paraent_gp_obj_id, read_result->p_value,
                       OTS_OBJ_ID_LEN);
            }
            T_MCS_CLIENT_READ_OBJ_IDS   obj_ids;
            obj_ids.conn_handle = conn_handle;
            obj_ids.cause = read_result->cause;
            obj_ids.gmcs = true;
            obj_ids.srv_instance_id = read_result->srv_instance_id;
            obj_ids.cur_tk_obj_id = mcs_client_cb->p_gmcs_client->cur_tk_obj_id;
            obj_ids.cur_tk_seg_obj_id = mcs_client_cb->p_gmcs_client->cur_tk_seg_obj_id;
            obj_ids.next_tk_obj_id = mcs_client_cb->p_gmcs_client->next_tk_obj_id;
            obj_ids.cur_gp_obj_id = mcs_client_cb->p_gmcs_client->cur_gp_obj_id;
            obj_ids.paraent_gp_obj_id = mcs_client_cb->p_gmcs_client->paraent_gp_obj_id;
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCS_CLIENT_READ_OBJ_IDS, &obj_ids);
        }
        return res;
    }


    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        gatt_client_read(conn_handle, handle, gmcs_read_relevant_tk_obj_id_cb);
    }

    return res;
}

static T_APP_RESULT mcs_read_relevant_tk_obj_id_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                                   void *p_data)
{
    T_GATT_CLIENT_DATA *p = (T_GATT_CLIENT_DATA *) p_data;
    T_GATT_CLIENT_READ_RESULT *read_result = &p->read_result;
    T_MCS_CLIENT_DB *mcs_client_cb = mcs_check_link_int(conn_handle);
    T_APP_RESULT  res = APP_RESULT_SUCCESS;
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    uint16_t handle = 0;

    if ((mcs_client_cb == NULL) || (mcs_client_cb->p_mcs_clients == NULL))
    {
        return APP_RESULT_APP_ERR;
    }

    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = read_result->srv_instance_id;
    srv_uuid.p.uuid16 = GATT_UUID_MEDIA_CONTROL_SERVICE;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = 0;

    if (read_result->cause != GATT_SUCCESS)
    {
        PROTOCOL_PRINT_ERROR1("mcs_read_relevant_tk_obj_id_cb: error: %d", read_result->cause);
        res = APP_RESULT_APP_ERR;;
    }

    if (read_result->value_size != OTS_OBJ_ID_LEN)
    {
        PROTOCOL_PRINT_ERROR1("mcs_read_relevant_tk_obj_id_cb: wrong length: %d", read_result->value_size);
        res = APP_RESULT_APP_ERR;
    }

    switch (read_result->char_uuid.p.uuid16)
    {
    case MCS_UUID_CHAR_CURRENT_TRACK_OBJECT_ID:
        {
            if (res == APP_RESULT_SUCCESS)
            {
                memcpy((uint8_t *)&mcs_client_cb->p_mcs_clients[read_result->srv_instance_id].cur_tk_obj_id,
                       read_result->p_value, OTS_OBJ_ID_LEN);
            }
            char_uuid.p.uuid16 = MCS_UUID_CHAR_CURRENT_TRACK_SEGMENTS_OBJECT_ID;
        }
        break;

    case MCS_UUID_CHAR_CURRENT_TRACK_SEGMENTS_OBJECT_ID:
        {
            if (res == APP_RESULT_SUCCESS)
            {
                memcpy((uint8_t *)&mcs_client_cb->p_mcs_clients[read_result->srv_instance_id].cur_tk_seg_obj_id,
                       read_result->p_value, OTS_OBJ_ID_LEN);
            }
            char_uuid.p.uuid16 = MCS_UUID_CHAR_NEXT_TRACK_OBJECT_ID;
        }
        break;

    case MCS_UUID_CHAR_NEXT_TRACK_OBJECT_ID:
        {
            if (res == APP_RESULT_SUCCESS)
            {
                memcpy((uint8_t *)&mcs_client_cb->p_mcs_clients[read_result->srv_instance_id].next_tk_obj_id,
                       read_result->p_value, OTS_OBJ_ID_LEN);
            }
            char_uuid.p.uuid16 = MCS_UUID_CHAR_CURRENT_GROUP_OBJECT_ID;
        }
        break;

    case MCS_UUID_CHAR_CURRENT_GROUP_OBJECT_ID:
        {
            if (res == APP_RESULT_SUCCESS)
            {
                memcpy((uint8_t *)&mcs_client_cb->p_mcs_clients[read_result->srv_instance_id].cur_gp_obj_id,
                       read_result->p_value, OTS_OBJ_ID_LEN);
            }
            char_uuid.p.uuid16 = MCS_UUID_CHAR_PARENT_GROUP_OBJECT_ID;
        }
        break;

    case MCS_UUID_CHAR_PARENT_GROUP_OBJECT_ID:
        {
            if (res == APP_RESULT_SUCCESS)
            {
                memcpy((uint8_t *)&mcs_client_cb->p_mcs_clients[read_result->srv_instance_id].paraent_gp_obj_id,
                       read_result->p_value, OTS_OBJ_ID_LEN);
            }
            T_MCS_CLIENT_READ_OBJ_IDS   obj_ids;
            obj_ids.conn_handle = conn_handle;
            obj_ids.cause = read_result->cause;
            obj_ids.gmcs = false;
            obj_ids.srv_instance_id = read_result->srv_instance_id;
            obj_ids.cur_tk_obj_id = mcs_client_cb->p_mcs_clients[read_result->srv_instance_id].cur_tk_obj_id;
            obj_ids.cur_tk_seg_obj_id =
                mcs_client_cb->p_mcs_clients[read_result->srv_instance_id].cur_tk_seg_obj_id;
            obj_ids.next_tk_obj_id = mcs_client_cb->p_mcs_clients[read_result->srv_instance_id].next_tk_obj_id;
            obj_ids.cur_gp_obj_id = mcs_client_cb->p_mcs_clients[read_result->srv_instance_id].cur_gp_obj_id;
            obj_ids.paraent_gp_obj_id =
                mcs_client_cb->p_mcs_clients[read_result->srv_instance_id].paraent_gp_obj_id;
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCS_CLIENT_READ_OBJ_IDS, &obj_ids);
        }
        return res;
    }

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        gatt_client_read(conn_handle, handle, mcs_read_relevant_tk_obj_id_cb);
    }

    return res;
}

void mcp_client_cfg_cccd(uint16_t conn_handle, uint32_t cfg_flags, bool gmcs,
                         uint8_t srv_instance_id,
                         bool enable)
{
    T_MCS_CLIENT_DB *p_client = mcs_check_link_int(conn_handle);
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = gmcs == true ? GATT_UUID_GENERIC_MEDIA_CONTROL_SERVICE :
                        GATT_UUID_MEDIA_CONTROL_SERVICE;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    uint8_t cccd_cfg;
    if (enable)
    {
        cccd_cfg = GATT_CLIENT_CONFIG_ALL;
    }
    else
    {
        cccd_cfg = GATT_CLIENT_CONFIG_DISABLE;
    }
    if (p_client == NULL)
    {
        return;
    }

    while (cfg_flags)
    {
        if (cfg_flags & MCP_CLIENT_CFG_CCCD_FLAG_TRACK_CHANGED)
        {
            char_uuid.p.uuid16 = MCS_UUID_CHAR_TRACK_CHANGED;
            cfg_flags &= ~MCP_CLIENT_CFG_CCCD_FLAG_TRACK_CHANGED;
        }
        else if (cfg_flags & MCP_CLIENT_CFG_CCCD_FLAG_MEDIA_STATE)
        {
            char_uuid.p.uuid16 = MCS_UUID_CHAR_MEDIA_STATE;
            cfg_flags &= ~MCP_CLIENT_CFG_CCCD_FLAG_MEDIA_STATE;
        }
        else if (cfg_flags & MCP_CLIENT_CFG_CCCD_FLAG_MEDIA_PLAYER_NAME)
        {
            char_uuid.p.uuid16 = MCS_UUID_CHAR_MEDIA_PLAYER_NAME;
            cfg_flags &= ~MCP_CLIENT_CFG_CCCD_FLAG_MEDIA_PLAYER_NAME;
        }
        else if (cfg_flags & MCP_CLIENT_CFG_CCCD_FLAG_TRACK_TITLE)
        {
            char_uuid.p.uuid16 = MCS_UUID_CHAR_TRACK_TITLE;
            cfg_flags &= ~MCP_CLIENT_CFG_CCCD_FLAG_TRACK_TITLE;
        }
        else if (cfg_flags & MCP_CLIENT_CFG_CCCD_FLAG_TRACK_DURATION)
        {
            char_uuid.p.uuid16 = MCS_UUID_CHAR_TRACK_DURATION;
            cfg_flags &= ~MCP_CLIENT_CFG_CCCD_FLAG_TRACK_DURATION;
        }
        else if (cfg_flags & MCP_CLIENT_CFG_CCCD_FLAG_TRACK_POSITION)
        {
            char_uuid.p.uuid16 = MCS_UUID_CHAR_TRACK_POSITION;
            cfg_flags &= ~MCP_CLIENT_CFG_CCCD_FLAG_TRACK_POSITION;
        }
        else if (cfg_flags & MCP_CLIENT_CFG_CCCD_FLAG_PLAYBACK_SPEED)
        {
            char_uuid.p.uuid16 = MCS_UUID_CHAR_PLAYBACK_SPEED;
            cfg_flags &= ~MCP_CLIENT_CFG_CCCD_FLAG_PLAYBACK_SPEED;
        }
        else if (cfg_flags & MCP_CLIENT_CFG_CCCD_FLAG_SEEKING_SPEED)
        {
            char_uuid.p.uuid16 = MCS_UUID_CHAR_SEEKING_SPEED;
            cfg_flags &= ~MCP_CLIENT_CFG_CCCD_FLAG_SEEKING_SPEED;
        }
        else if (cfg_flags & MCP_CLIENT_CFG_CCCD_FLAG_CURRENT_TRACK_OBJECT_ID)
        {
            char_uuid.p.uuid16 = MCS_UUID_CHAR_CURRENT_TRACK_OBJECT_ID;
            cfg_flags &= ~MCP_CLIENT_CFG_CCCD_FLAG_CURRENT_TRACK_OBJECT_ID;
        }
        else if (cfg_flags & MCP_CLIENT_CFG_CCCD_FLAG_NEXT_TRACK_OBJECT_ID)
        {
            char_uuid.p.uuid16 = MCS_UUID_CHAR_NEXT_TRACK_OBJECT_ID;
            cfg_flags &= ~MCP_CLIENT_CFG_CCCD_FLAG_NEXT_TRACK_OBJECT_ID;
        }
        else if (cfg_flags & MCP_CLIENT_CFG_CCCD_FLAG_PARENT_GROUP_OBJECT_ID)
        {
            char_uuid.p.uuid16 = MCS_UUID_CHAR_PARENT_GROUP_OBJECT_ID;
            cfg_flags &= ~MCP_CLIENT_CFG_CCCD_FLAG_PARENT_GROUP_OBJECT_ID;
        }
        else if (cfg_flags & MCP_CLIENT_CFG_CCCD_FLAG_CURRENT_GROUP_OBJECT_ID)
        {
            char_uuid.p.uuid16 = MCS_UUID_CHAR_CURRENT_GROUP_OBJECT_ID;
            cfg_flags &= ~MCP_CLIENT_CFG_CCCD_FLAG_CURRENT_GROUP_OBJECT_ID;
        }
        else if (cfg_flags & MCP_CLIENT_CFG_CCCD_FLAG_PLAYING_ORDER)
        {
            char_uuid.p.uuid16 = MCS_UUID_CHAR_PLAYING_ORDER;
            cfg_flags &= ~MCP_CLIENT_CFG_CCCD_FLAG_PLAYING_ORDER;
        }
        else if (cfg_flags & MCP_CLIENT_CFG_CCCD_FLAG_MEDIA_CONTROL_POINT)
        {
            char_uuid.p.uuid16 = MCS_UUID_CHAR_MEDIA_CONTROL_POINT;
            cfg_flags &= ~MCP_CLIENT_CFG_CCCD_FLAG_MEDIA_CONTROL_POINT;
        }
        else if (cfg_flags & MCP_CLIENT_CFG_CCCD_FLAG_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED)
        {
            char_uuid.p.uuid16 = MCS_UUID_CHAR_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED;
            cfg_flags &= ~MCP_CLIENT_CFG_CCCD_FLAG_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED;
        }
        else if (cfg_flags & MCP_CLIENT_CFG_CCCD_FLAG_SEARCH_CONTROL_POINT)
        {
            char_uuid.p.uuid16 = MCS_UUID_CHAR_SEARCH_CONTROL_POINT;
            cfg_flags &= ~MCP_CLIENT_CFG_CCCD_FLAG_SEARCH_CONTROL_POINT;
        }
        else if (cfg_flags & MCP_CLIENT_CFG_CCCD_FLAG_SEARCH_RESULTS_OBJECT_ID)
        {
            char_uuid.p.uuid16 = MCS_UUID_CHAR_SEARCH_RESULTS_OBJECT_ID;
            cfg_flags &= ~MCP_CLIENT_CFG_CCCD_FLAG_SEARCH_RESULTS_OBJECT_ID;
        }
        else
        {
            break;
        }
        if (gatt_client_check_cccd_enabled(conn_handle, &srv_uuid, &char_uuid) != enable)
        {
            gatt_client_enable_char_cccd(conn_handle, &srv_uuid, &char_uuid, cccd_cfg);
        }
    }
}

bool mcp_client_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, uint16_t char_uuid,
                                bool gmcs)
{
    T_MCS_CLIENT_DB *p_client = mcs_check_link_int(conn_handle);
    if (p_client == NULL)
    {
        return false;
    }
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid16;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = gmcs == true ? GATT_UUID_GENERIC_MEDIA_CONTROL_SERVICE :
                        GATT_UUID_MEDIA_CONTROL_SERVICE;
    char_uuid16.is_uuid16 = true;
    char_uuid16.instance_id = 0;
    char_uuid16.p.uuid16 = char_uuid;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid16, &handle))
    {
        cause = gatt_client_read(conn_handle, handle, NULL);
    }
    return (cause == GAP_CAUSE_SUCCESS) ? true : false;
}


bool mcs_read_relevant_tk_obj_id(uint16_t conn_handle, uint8_t srv_instance_id, bool gmcs)
{
    T_MCS_CLIENT_DB *p_client = mcs_check_link_int(conn_handle);
    if (p_client == NULL)
    {
        return false;
    }
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = gmcs == true ? GATT_UUID_GENERIC_MEDIA_CONTROL_SERVICE :
                        GATT_UUID_MEDIA_CONTROL_SERVICE;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = MCS_UUID_CHAR_CURRENT_TRACK_OBJECT_ID;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_read(conn_handle, handle, gmcs == true ?
                                 gmcs_read_relevant_tk_obj_id_cb : mcs_read_relevant_tk_obj_id_cb);
    }
    return (cause == GAP_CAUSE_SUCCESS) ? true : false;
}

bool mcp_client_write_media_cp(uint16_t conn_handle, uint8_t srv_instance_id, bool gmcs,
                               T_MCP_CLIENT_WRITE_MEDIA_CP_PARAM *p_param, bool is_req)
{
    T_GATT_WRITE_TYPE write_type = (is_req) ? GATT_WRITE_TYPE_REQ : GATT_WRITE_TYPE_CMD;
    uint8_t data[5] = {0};
    uint16_t len = 1;
    T_MCS_CLIENT_DB *p_client = mcs_check_link_int(conn_handle);

    if ((p_client == NULL) || (((gmcs) && (p_client->p_gmcs_client == NULL)) ||
                               ((!gmcs) &&
                                (p_client->p_mcs_clients == NULL))))
    {
        return false;
    }

    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = gmcs == true ? GATT_UUID_GENERIC_MEDIA_CONTROL_SERVICE :
                        GATT_UUID_MEDIA_CONTROL_SERVICE;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = MCS_UUID_CHAR_MEDIA_CONTROL_POINT;

    data[0] = p_param->opcode;

    if (p_param->opcode == MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_MOVE_RELATIVE)
    {
        LE_UINT32_TO_ARRAY(&data[1], p_param->param.move_relative_opcode_offset);
        len += 4;
    }
    else if (p_param->opcode == MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_SEGMENT)
    {
        LE_UINT32_TO_ARRAY(&data[1], p_param->param.goto_segment_opcode_n);
        len += 4;
    }
    else if (p_param->opcode == MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_TRACK)
    {
        LE_UINT32_TO_ARRAY(&data[1], p_param->param.goto_track_opcode_n);
        len += 4;
    }
    else if (p_param->opcode == MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_GROUP)
    {
        LE_UINT32_TO_ARRAY(&data[1], p_param->param.goto_group_opcode_n);
        len += 4;
    }

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, write_type, handle, len, data, NULL);
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        return true;
    }

    return false;
}

bool mcs_send_scp_op(uint16_t conn_handle, uint8_t srv_instance_id, bool gmcs,
                     T_MCS_CLIENT_SCP_PARAM *p_param, uint8_t param_num,
                     P_MCS_SCP_CB p_scp_cb)
{
    int i;
    T_MCS_CLIENT_DB *p_client = mcs_check_link_int(conn_handle);

    if ((p_client == NULL) || (((gmcs) && (p_client->p_gmcs_client == NULL)) ||
                               ((!gmcs) &&
                                (p_client->p_mcs_clients == NULL))))
    {
        return false;
    }

    T_MCS_CLT_CB  *p_mcs_client = gmcs == true ? p_client->p_gmcs_client :
                                  (p_client->p_mcs_clients + srv_instance_id);
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = gmcs == true ? GATT_UUID_GENERIC_MEDIA_CONTROL_SERVICE :
                        GATT_UUID_MEDIA_CONTROL_SERVICE;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = MCS_UUID_CHAR_SEARCH_CONTROL_POINT;

    PROTOCOL_PRINT_TRACE1("mcs_send_scp_op: scp operate 0x%x", p_param->type);

    uint16_t len = 0;
    for (i = 0; i < param_num; i++)
    {
        len += p_param[i].value_len + 2;
    }

    uint8_t data[len];
    len = 0;
    for (i = 0; i < param_num; i++)
    {
        data[len++] = p_param[i].value_len + 1;
        data[len++] = p_param[i].type;
        memcpy(&data[len], p_param[i].p_value, p_param[i].value_len);
        len += p_param[i].value_len;
    }

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_REQ, handle, len, data, NULL);
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        p_mcs_client->p_scp_cb = p_scp_cb;
        return true;
    }

    return false;
}
#if LE_AUDIO_OTS_CLIENT_SUPPORT
bool mcs_read_obj_value(uint16_t conn_handle, uint8_t srv_instance_id, bool gmcs,
                        uint16_t uuid,
                        uint64_t obj_id)
{
    T_MCS_CLIENT_DB *p_client = mcs_check_link_int(conn_handle);

    if ((p_client == NULL) || (((gmcs) && (p_client->p_gmcs_client == NULL)) ||
                               ((!gmcs) &&
                                (p_client->p_mcs_clients == NULL))))
    {
        PROTOCOL_PRINT_ERROR0("mcs_read_obj_value: client is NULL");
        return false;
    }

    T_MCS_CLT_CB  *p_mcs_client = gmcs == true ? p_client->p_gmcs_client :
                                  (p_client->p_mcs_clients + srv_instance_id);
    if (p_client->read_obj_id != 0)
    {
        PROTOCOL_PRINT_ERROR0("mcs_read_obj_value: already in reading obj");
        return false;
    }
    T_OTS_OLCP_PARAM param;
    param.opcode = OTS_OLCP_GOTO_OP;
    param.p_olcp_cb = mcs_ots_goto_op_cb;
    param.value.obj_id = obj_id;

    if (ots_client_olcp_op(conn_handle, p_mcs_client->ots_idx, &param))
    {
        p_client->read_obj_id = obj_id;
        p_client->general_read = gmcs;
        p_client->read_obj_uuid = uuid;
        p_client->read_mcs_srv_idx = srv_instance_id;
        return true;
    }

    return false;
}

uint8_t mcs_get_ots_idx(uint16_t conn_handle, uint8_t srv_instance_id, bool gmcs)
{
    T_MCS_CLIENT_DB *p_client = mcs_check_link_int(conn_handle);
    if (p_client == NULL)
    {
        PROTOCOL_PRINT_ERROR0("mcs_get_ots_idx: client is NULL");
        return false;
    }
    if (gmcs)
    {
        return p_client->p_gmcs_client->ots_idx;
    }
    else
    {
        return p_client->p_mcs_clients[srv_instance_id].ots_idx;
    }
}
#endif
static T_APP_RESULT mcs_client_handle_cback(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                            void *p_data, bool gmcs)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;

    T_MCS_CLIENT_DB *p_mcs_db = mcs_check_link_int(conn_handle);
    if (p_mcs_db == NULL && type != GATT_CLIENT_EVENT_DIS_DONE)
    {
        goto error;
    }

    switch (type)
    {
    case GATT_CLIENT_EVENT_DIS_DONE:
        {
            T_MCP_CLIENT_DIS_DONE dis_done;
            T_ATTR_UUID srv_uuid;
            T_ATTR_UUID inc_uuid;
            srv_uuid.is_uuid16 = true;
            srv_uuid.instance_id = 0;
            inc_uuid.is_uuid16 = true;
            inc_uuid.instance_id = 0;
            inc_uuid.p.uuid16 = OBJECT_TRANS_SERVICE_UUID;
            T_ATTR_INSTANCE attr_instance;
            uint8_t srv_num = p_client_cb_data->dis_done.srv_instance_num;
            PROTOCOL_PRINT_INFO3("[MCS] GATT_CLIENT_EVENT_DIS_DONE: gmcs %d, is_found %d, srv_instance_num %d",
                                 gmcs, p_client_cb_data->dis_done.is_found,
                                 p_client_cb_data->dis_done.srv_instance_num);

            if (p_client_cb_data->dis_done.is_found)
            {
                T_BLE_AUDIO_LINK *p_link = NULL;
                p_link = ble_audio_link_find_by_conn_handle(conn_handle);
                if (p_link == NULL)
                {
                    goto error;
                }

                if (p_link->p_mcs_client == NULL)
                {
                    p_link->p_mcs_client = ble_audio_mem_zalloc(sizeof(T_MCS_CLIENT_DB));
                    if (p_link->p_mcs_client == NULL)
                    {
                        PROTOCOL_PRINT_ERROR0("mcs_client_handle_cback, alloc p_mcs_client fail!");
                        return APP_RESULT_APP_ERR;
                    }
                }

                T_MCS_CLIENT_DB  *p_mcs_client = (T_MCS_CLIENT_DB *)p_link->p_mcs_client;
                if (gmcs)
                {
                    if (p_mcs_client->p_gmcs_client)
                    {
                        ble_audio_mem_free(p_mcs_client->p_gmcs_client);
                    }

                    p_mcs_client->p_gmcs_client = ble_audio_mem_zalloc(sizeof(T_MCS_CLT_CB));

                    if (p_mcs_client->p_gmcs_client == NULL)
                    {
                        PROTOCOL_PRINT_ERROR0("gmcs_client_cback, alloc p_gmcs_client fail!");
                        goto error;
                    }
                    srv_uuid.p.uuid16 = GATT_UUID_GENERIC_MEDIA_CONTROL_SERVICE;
                    gatt_client_find_include_srv_by_primary(conn_handle, &srv_uuid, &inc_uuid, &attr_instance);
                    if (attr_instance.instance_num != 0)
                    {
                        p_mcs_client->p_gmcs_client->ots_idx = attr_instance.instance_id[0];
                        p_mcs_client->p_gmcs_client->has_ots = true;
                    }
                    else
                    {
                        p_mcs_client->p_gmcs_client->ots_idx = 0xFF;
                        p_mcs_client->p_gmcs_client->has_ots = false;
                    }
                }
                else
                {
                    if (p_mcs_client->p_mcs_clients)
                    {
                        ble_audio_mem_free(p_mcs_client->p_mcs_clients);
                    }

                    p_mcs_client->p_mcs_clients = ble_audio_mem_zalloc(sizeof(T_MCS_CLT_CB) * srv_num);

                    if (p_mcs_client->p_mcs_clients == NULL)
                    {
                        PROTOCOL_PRINT_ERROR0("mcs_client_handle_cback, alloc p_mcs_client fail!");
                        goto error;
                    }
                    srv_uuid.p.uuid16 = GATT_UUID_MEDIA_CONTROL_SERVICE;
                    for (int i = 0; i < srv_num; i++)
                    {
                        srv_uuid.instance_id = i;
                        inc_uuid.instance_id = i;
                        gatt_client_find_include_srv_by_primary(conn_handle, &srv_uuid, &inc_uuid, &attr_instance);
                        if (attr_instance.instance_num != 0)
                        {
                            p_mcs_client->p_mcs_clients[i].ots_idx = attr_instance.instance_id[0];
                            p_mcs_client->p_mcs_clients[i].has_ots = true;
                        }
                        else
                        {
                            p_mcs_client->p_mcs_clients[i].ots_idx = 0xFF;
                            p_mcs_client->p_mcs_clients[i].has_ots = false;
                        }
                    }
                }
            }
            dis_done.conn_handle = conn_handle;
            dis_done.is_found = p_client_cb_data->dis_done.is_found;
            dis_done.gmcs = gmcs;
            dis_done.srv_num = srv_num;
            dis_done.load_from_ftl = p_client_cb_data->dis_done.load_from_ftl;
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCP_CLIENT_DIS_DONE, &dis_done);
        }
        break;

    case GATT_CLIENT_EVENT_CCCD_CFG:
        {
            ble_audio_mgr_dispatch_client_attr_cccd_info(conn_handle, &p_client_cb_data->cccd_cfg);
        }
        break;

    case GATT_CLIENT_EVENT_READ_RESULT:
        {
            T_MCP_CLIENT_READ_RESULT read_result = {0};
            read_result.conn_handle = conn_handle;
            read_result.cause = p_client_cb_data->read_result.cause;
            read_result.srv_instance_id = p_client_cb_data->read_result.srv_instance_id;
            read_result.char_uuid = p_client_cb_data->read_result.char_uuid.p.uuid16;
            read_result.gmcs = gmcs;

            if (read_result.cause == ATT_SUCCESS)
            {
                switch (read_result.char_uuid)
                {
                case MCS_UUID_CHAR_CONTENT_CONTROL_ID:
                    {
                        if (p_client_cb_data->read_result.value_size != 1)
                        {
                            read_result.cause = APP_RESULT_INVALID_VALUE_SIZE;
                        }
                        else
                        {
                            read_result.data.content_control_id = p_client_cb_data->read_result.p_value[0];
                        }
                    }
                    break;

                case MCS_UUID_CHAR_MEDIA_PLAYER_NAME:
                    {
                        read_result.data.media_player_name.p_media_player_name = p_client_cb_data->read_result.p_value;
                        read_result.data.media_player_name.media_player_name_len = p_client_cb_data->read_result.value_size;
                    }
                    break;

#if LE_AUDIO_MCP_CLIENT_SUPPORT_INTERNAL
                case MCS_UUID_CHAR_MEDIA_PLAYER_ICON_OBJECT_ID:
                case MCS_UUID_CHAR_CURRENT_TRACK_SEGMENTS_OBJECT_ID:
                case MCS_UUID_CHAR_CURRENT_TRACK_OBJECT_ID:
                case MCS_UUID_CHAR_NEXT_TRACK_OBJECT_ID:
                case MCS_UUID_CHAR_PARENT_GROUP_OBJECT_ID:
                case MCS_UUID_CHAR_CURRENT_GROUP_OBJECT_ID:
                    {
                        if (p_client_cb_data->read_result.value_size != OTS_OBJ_ID_LEN)
                        {
                            result = APP_RESULT_INVALID_VALUE_SIZE;
                            break;
                        }
                        memcpy((uint8_t *)&read_result.data.read_obj_id, p_client_cb_data->read_result.p_value,
                               OTS_OBJ_ID_LEN);

                    }
                    break;

                case MCS_UUID_CHAR_SEARCH_RESULTS_OBJECT_ID:
                    {
                        if (p_client_cb_data->read_result.value_size != OTS_OBJ_ID_LEN)
                        {
                            result = APP_RESULT_INVALID_VALUE_SIZE;
                            break;
                        }
                        memcpy((uint8_t *)&read_result.data.search_res_obj_id, p_client_cb_data->read_result.p_value,
                               OTS_OBJ_ID_LEN);
                    }
                    break;

                case MCS_UUID_CHAR_MEDIA_PLAYER_ICON_URL:
                    {
                        read_result.data.media_player_icon_url.p_icon_url = p_client_cb_data->read_result.p_value;
                        read_result.data.media_player_icon_url.url_len = p_client_cb_data->read_result.value_size;
                    }
                    break;
#endif

                case MCS_UUID_CHAR_TRACK_TITLE:
                    {
                        read_result.data.track_title.p_track_title = p_client_cb_data->read_result.p_value;
                        read_result.data.track_title.track_title_len = p_client_cb_data->read_result.value_size;
                    }
                    break;

                case MCS_UUID_CHAR_TRACK_DURATION:
                    {
                        if (p_client_cb_data->read_result.value_size != 4)
                        {
                            read_result.cause = APP_RESULT_INVALID_VALUE_SIZE;
                        }
                        else
                        {
                            LE_ARRAY_TO_UINT32(read_result.data.track_duration, p_client_cb_data->read_result.p_value);
                        }
                    }
                    break;

                case MCS_UUID_CHAR_TRACK_POSITION:
                    {
                        if (p_client_cb_data->read_result.value_size != 4)
                        {
                            read_result.cause = APP_RESULT_INVALID_VALUE_SIZE;
                        }
                        else
                        {
                            LE_ARRAY_TO_UINT32(read_result.data.track_position, p_client_cb_data->read_result.p_value);
                        }
                    }
                    break;

                case MCS_UUID_CHAR_PLAYBACK_SPEED:
                    {
                        if (p_client_cb_data->read_result.value_size != 1)
                        {
                            read_result.cause = APP_RESULT_INVALID_VALUE_SIZE;
                        }
                        else
                        {
                            read_result.data.playback_speed = (int8_t)p_client_cb_data->read_result.p_value[0];
                        }
                    }
                    break;

                case MCS_UUID_CHAR_SEEKING_SPEED:
                    {
                        if (p_client_cb_data->read_result.value_size != 1)
                        {
                            read_result.cause = APP_RESULT_INVALID_VALUE_SIZE;
                        }
                        else
                        {
                            read_result.data.seeking_speed = (int8_t)p_client_cb_data->read_result.p_value[0];
                        }
                    }
                    break;

                case MCS_UUID_CHAR_PLAYING_ORDER:
                    {
                        if (p_client_cb_data->read_result.value_size != 1)
                        {
                            read_result.cause = APP_RESULT_INVALID_VALUE_SIZE;
                        }
                        else
                        {
                            read_result.data.playing_order = p_client_cb_data->read_result.p_value[0];
                        }
                    }
                    break;

                case MCS_UUID_CHAR_MEDIA_STATE:
                    {
                        if (p_client_cb_data->read_result.value_size != 1)
                        {
                            read_result.cause = APP_RESULT_INVALID_VALUE_SIZE;
                        }
                        else
                        {
                            read_result.data.media_state = p_client_cb_data->read_result.p_value[0];
                        }
                    }
                    break;

                case MCS_UUID_CHAR_PLAYING_ORDERS_SUPPORTED:
                    {
                        if (p_client_cb_data->read_result.value_size != 2)
                        {
                            read_result.cause = APP_RESULT_INVALID_VALUE_SIZE;
                        }
                        else
                        {
                            LE_ARRAY_TO_UINT16(read_result.data.playing_orders_supported,
                                               p_client_cb_data->read_result.p_value);
                        }
                    }
                    break;

                case MCS_UUID_CHAR_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED:
                    {
                        if (p_client_cb_data->read_result.value_size != MCS_MEDIA_CTRL_POINT_OPCODES_SUP_ATTR_VALUE_LEN)
                        {
                            read_result.cause = APP_RESULT_INVALID_VALUE_SIZE;
                        }
                        else
                        {
                            LE_ARRAY_TO_UINT32(read_result.data.media_control_point_opcodes_supported,
                                               p_client_cb_data->read_result.p_value);
                        }
                    }
                    break;

                default:
                    return result;
                }
            }

            ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCP_CLIENT_READ_RESULT, &read_result);
        }
        break;

    case GATT_CLIENT_EVENT_NOTIFY_IND:
        {
            mcs_handle_charac_notify(conn_handle, gmcs, p_data);
        }

        break;

    case GATT_CLIENT_EVENT_WRITE_RESULT:
        {

        }
        break;

    default:
        break;
    }

    return result;
error:
    return APP_RESULT_APP_ERR;
}

static T_APP_RESULT mcs_client_cback(uint16_t conn_handle, T_GATT_CLIENT_EVENT type, void *p_data)
{
    return mcs_client_handle_cback(conn_handle, type, p_data, false);
}

static T_APP_RESULT gmcs_client_cback(uint16_t conn_handle, T_GATT_CLIENT_EVENT type, void *p_data)
{
    return mcs_client_handle_cback(conn_handle, type, p_data, true);
}

static void mcs_client_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BLE_DM_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BLE_DM_EVENT_CONN_STATE:
        {
            T_BLE_AUDIO_LINK *p_link = NULL;
            p_link = ble_audio_link_find_by_conn_id(param->conn_state.conn_id);
            if (p_link == NULL)
            {
                PROTOCOL_PRINT_ERROR1("mcs_client_dm_cback: not find the link, conn_id %d",
                                      param->conn_state.conn_id);
                break;
            }
            if (param->conn_state.state == GAP_CONN_STATE_DISCONNECTED)
            {
                T_MCS_CLIENT_DB *p_mcs_client = (T_MCS_CLIENT_DB *)p_link->p_mcs_client;
                if (p_mcs_client != NULL)
                {
                    if (p_mcs_client->p_gmcs_client != NULL)
                    {
                        ble_audio_mem_free(p_mcs_client->p_gmcs_client);
                    }
                    if (p_mcs_client->p_mcs_clients != NULL)
                    {
                        ble_audio_mem_free(p_mcs_client->p_mcs_clients);
                    }
                    ble_audio_mem_free(p_link->p_mcs_client);
                    p_link->p_mcs_client = NULL;
                }
            }
        }
        break;

    default:
        break;
    }
}

bool mcs_client_init(void)
{
    T_ATTR_UUID srv_uuid = {0};
    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = GATT_UUID_GENERIC_MEDIA_CONTROL_SERVICE;
    if (gatt_client_spec_register(&srv_uuid, gmcs_client_cback) == GAP_CAUSE_SUCCESS)
    {
        srv_uuid.is_uuid16 = true;
        srv_uuid.p.uuid16 = GATT_UUID_MEDIA_CONTROL_SERVICE;
        if (gatt_client_spec_register(&srv_uuid, mcs_client_cback) == GAP_CAUSE_SUCCESS)
        {

            ble_dm_cback_register(mcs_client_dm_cback);
            return true;
        }
    }
    return false;
}

bool mcs_set_abs_track_pos(uint16_t conn_handle, uint8_t srv_instance_id, bool is_cmd,
                           bool gmcs,
                           int32_t pos)
{
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL)
    {
        return false;
    }

    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    uint8_t data[4] = {0};

    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = gmcs ? GATT_UUID_GENERIC_MEDIA_CONTROL_SERVICE :
                        GATT_UUID_MEDIA_CONTROL_SERVICE;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = MCS_UUID_CHAR_TRACK_POSITION;

    LE_UINT32_TO_ARRAY(data, pos);

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, is_cmd ? GATT_WRITE_TYPE_CMD : GATT_WRITE_TYPE_REQ,
                                  handle, 4, data, NULL);
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool mcs_set_playing_order(uint16_t conn_handle, uint8_t srv_instance_id, bool is_cmd,
                           bool gmcs,
                           uint8_t order)
{
    T_MCS_CLIENT_DB *mcs_client_cb = mcs_check_link_int(conn_handle);

    if ((mcs_client_cb == NULL) || (((gmcs) && (mcs_client_cb->p_gmcs_client == NULL)) ||
                                    ((!gmcs) && (mcs_client_cb->p_mcs_clients == NULL))))
    {
        return false;
    }

    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = gmcs ? GATT_UUID_GENERIC_MEDIA_CONTROL_SERVICE :
                        GATT_UUID_MEDIA_CONTROL_SERVICE;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = MCS_UUID_CHAR_PLAYING_ORDER;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, is_cmd ? GATT_WRITE_TYPE_CMD : GATT_WRITE_TYPE_REQ, handle,
                                  1,
                                  &order, NULL);
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool mcs_set_playback_speed(uint16_t conn_handle, uint8_t srv_instance_id, bool is_cmd,
                            bool gmcs,
                            int8_t playback_speed)
{
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL)
    {
        return false;
    }

    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = gmcs ? GATT_UUID_GENERIC_MEDIA_CONTROL_SERVICE :
                        GATT_UUID_MEDIA_CONTROL_SERVICE;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = MCS_UUID_CHAR_PLAYBACK_SPEED;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, is_cmd ? GATT_WRITE_TYPE_CMD : GATT_WRITE_TYPE_REQ, handle,
                                  1,
                                  (uint8_t *)&playback_speed,
                                  NULL);
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool mcs_set_cur_tk_obj_id(uint16_t conn_handle, uint8_t srv_instance_id, bool is_cmd,
                           bool gmcs, uint64_t obj_id)
{
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL)
    {
        return false;
    }

    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = gmcs ? GATT_UUID_GENERIC_MEDIA_CONTROL_SERVICE :
                        GATT_UUID_MEDIA_CONTROL_SERVICE;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = MCS_UUID_CHAR_CURRENT_TRACK_OBJECT_ID;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, is_cmd ? GATT_WRITE_TYPE_CMD : GATT_WRITE_TYPE_REQ,
                                  handle, OTS_OBJ_ID_LEN, (uint8_t *)&obj_id, NULL);
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }

}


bool mcs_set_next_tk_obj_id(uint16_t conn_handle, uint8_t srv_instance_id, bool is_cmd,
                            bool gmcs, uint64_t obj_id)
{
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL)
    {
        return false;
    }

    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = gmcs ? GATT_UUID_GENERIC_MEDIA_CONTROL_SERVICE :
                        GATT_UUID_MEDIA_CONTROL_SERVICE;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = MCS_UUID_CHAR_NEXT_TRACK_OBJECT_ID;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, is_cmd ? GATT_WRITE_TYPE_CMD : GATT_WRITE_TYPE_REQ,
                                  handle, OTS_OBJ_ID_LEN, (uint8_t *)&obj_id, NULL);
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool mcs_set_cur_gp_obj_id(uint16_t conn_handle, uint8_t srv_instance_id, bool is_cmd,
                           bool gmcs, uint64_t obj_id)
{
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL)
    {
        return false;
    }

    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = gmcs ? GATT_UUID_GENERIC_MEDIA_CONTROL_SERVICE :
                        GATT_UUID_MEDIA_CONTROL_SERVICE;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = MCS_UUID_CHAR_CURRENT_GROUP_OBJECT_ID;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, is_cmd ? GATT_WRITE_TYPE_CMD : GATT_WRITE_TYPE_REQ,
                                  handle, OTS_OBJ_ID_LEN, (uint8_t *)&obj_id, NULL);
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }

}

#if LE_AUDIO_DEINIT
void mcs_client_deinit(void)
{
    for (uint8_t k = 0; k < ble_audio_db.acl_link_num; k++)
    {
        if (ble_audio_db.le_link[k].p_mcs_client != NULL)
        {
            T_MCS_CLIENT_DB  *p_mcs_client = (T_MCS_CLIENT_DB *)ble_audio_db.le_link[k].p_mcs_client;
            if (p_mcs_client->p_gmcs_client)
            {
                ble_audio_mem_free(p_mcs_client->p_gmcs_client);
            }
            if (p_mcs_client->p_mcs_clients)
            {
                ble_audio_mem_free(p_mcs_client->p_mcs_clients);
            }
            ble_audio_mem_free(p_mcs_client);
            ble_audio_db.le_link[k].p_mcs_client = NULL;
        }
    }
}
#endif
#endif
