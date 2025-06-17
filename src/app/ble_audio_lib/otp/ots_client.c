/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     ots_client.c
* @brief    Object transfer service source file.
* @details  Interfaces to access Media Control service.
* @author   cheng_cai
* @date
* @version  v1.0
*********************************************************************************************************
*/
#include <string.h>
#include "ble_audio_mgr.h"
#include "otp_client_int.h"
#include "ots_coc.h"
#include "trace.h"
#include "ble_link_util.h"
#include "ble_audio.h"
#include "bt_gatt_client.h"
#include "ble_audio_dm.h"

#if LE_AUDIO_OTS_CLIENT_SUPPORT

typedef struct
{
    uint8_t    *name;
    uint16_t    name_len;

    T_CHAR_UUID type;
    uint32_t    size[2];
    uint64_t    first_created;
    uint64_t    last_modify;
    uint64_t    id;       //48 bytes
    uint32_t    props;

    uint16_t    name_index;
    uint16_t    type_index;
    uint16_t    size_index;
    uint16_t    first_created_index;
    uint16_t    last_modify_index;
    uint16_t    id_index;
    uint16_t    props_index;
} T_CUR_OBJ_CB;

typedef struct
{
    uint32_t          ots_features[2];
    T_CUR_OBJ_CB      cur_obj;

    uint8_t           olcp_opcode;
    P_OTS_OLCP_CB     p_ots_olcp_cb;
} T_OTS_SRV_CB;

typedef struct
{
    uint8_t           oacp_opcode;
    P_OTS_OACP_CB     p_ots_oacp_cb;
    uint8_t           srv_instance_id;
    uint16_t          wr_len;
    union
    {
        T_OTS_OACP_READ     oacp_read;
        T_OTS_OACP_WRITE    oacp_write;
    } value;
} T_HANDLE_OACP_CB;


typedef struct
{
    uint16_t          conn_handle;
    uint8_t           srv_num;
    uint16_t          tx_mtu;
    uint16_t          cid;

    T_HANDLE_OACP_CB  oacp_op_cb;
    T_OTS_SRV_CB      ots_client_cb[];
} T_OTS_CLIENT_DB;


/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
static T_APP_RESULT ots_client_coc_callback(uint8_t coc_type, void *p_coc_data, uint8_t id);
static T_OTS_CLIENT_DB *ots_check_link_int(uint16_t conn_handle)
{
    T_BLE_AUDIO_LINK *p_link = NULL;

    p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL || p_link->p_ots_client == NULL)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]ots_check_link_int failed, conn_handle 0x%x", conn_handle);
        return NULL;
    }
    return (T_OTS_CLIENT_DB *)p_link->p_ots_client;
}


static T_APP_RESULT read_features_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                     void *p_data)
{
    T_GATT_CLIENT_DATA *p = (T_GATT_CLIENT_DATA *) p_data;
    T_GATT_CLIENT_READ_RESULT *read_result = &p->read_result;
    T_OTS_CLIENT_DB *pp_ots_db = ots_check_link_int(conn_handle);
    T_OTS_CLT_READ_RES read_res;

    if (pp_ots_db == NULL)
    {
        return APP_RESULT_APP_ERR;
    }

    read_res.conn_handle = conn_handle;
    read_res.srv_instance_id = read_result->srv_instance_id;
    read_res.cause = read_result->cause;
    read_res.uuid = read_result->char_uuid.p.uuid16;

    if (read_result->cause != GATT_SUCCESS)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]read_features_cb: error: %d", read_result->cause);
        return APP_RESULT_APP_ERR;
    }

    if (read_result->value_size != 8)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]read_features_cb: wrong length: %d", read_result->value_size);
        return APP_RESULT_APP_ERR;
    }

    memcpy((uint8_t *)pp_ots_db->ots_client_cb[read_result->srv_instance_id].ots_features,
           read_result->p_value, read_result->value_size);
    memcpy((uint8_t *)read_res.value.ots_features, read_result->p_value, read_result->value_size);
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_OTS_CLIENT_READ_RES, &read_res);
    PROTOCOL_PRINT_TRACE3("[OTS]read_features_cb: srv_instance_id %d, oacp_feature 0x%x, olcp_feature 0x%x",
                          read_result->srv_instance_id,
                          pp_ots_db->ots_client_cb[read_result->srv_instance_id].ots_features[0],
                          pp_ots_db->ots_client_cb[read_result->srv_instance_id].ots_features[1]);
    return APP_RESULT_SUCCESS;
}

static T_APP_RESULT read_obj_name_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                     void *p_data)
{
    T_GATT_CLIENT_DATA *p = (T_GATT_CLIENT_DATA *) p_data;
    T_GATT_CLIENT_READ_RESULT *read_result = &p->read_result;
    T_OTS_CLIENT_DB *p_client = ots_check_link_int(conn_handle);
    uint16_t value_size;
    T_OTS_CLT_READ_RES read_res;

    if (p_client == NULL)
    {
        return APP_RESULT_APP_ERR;
    }

    read_res.conn_handle = conn_handle;
    read_res.srv_instance_id = read_result->srv_instance_id;
    read_res.cause = read_result->cause;
    read_res.uuid = read_result->char_uuid.p.uuid16;

    if (read_result->cause != GATT_SUCCESS)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]read_obj_name_cb: error: %d", read_result->cause);
        return APP_RESULT_APP_ERR;
    }

    value_size = read_result->value_size > OTS_OBJ_NAME_LEN_MAX ?
                 OTS_OBJ_NAME_LEN_MAX : read_result->value_size;

    if (p_client->ots_client_cb[read_result->srv_instance_id].cur_obj.name == NULL)
    {
        p_client->ots_client_cb[read_result->srv_instance_id].cur_obj.name = ble_audio_mem_zalloc(
                                                                                 OTS_OBJ_NAME_LEN_MAX + 1);
        if (p_client->ots_client_cb[read_result->srv_instance_id].cur_obj.name == NULL)
        {
            PROTOCOL_PRINT_ERROR0("[OTS]read_obj_name_cb: alloc fail");
            return APP_RESULT_APP_ERR;
        }
    }

    read_res.value.ots_name_cb.p_name = read_result->p_value;
    read_res.value.ots_name_cb.name_len = read_result->value_size;
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_OTS_CLIENT_READ_RES, &read_res);

    memcpy(p_client->ots_client_cb[read_result->srv_instance_id].cur_obj.name, read_result->p_value,
           value_size);
    p_client->ots_client_cb[read_result->srv_instance_id].cur_obj.name_len = value_size;
    return APP_RESULT_SUCCESS;
}

static T_APP_RESULT read_obj_type_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                     void *p_data)
{
    T_GATT_CLIENT_DATA *p = (T_GATT_CLIENT_DATA *) p_data;
    T_GATT_CLIENT_READ_RESULT *read_result = &p->read_result;
    T_OTS_CLIENT_DB *p_client = ots_check_link_int(conn_handle);
    T_OTS_CLT_READ_RES read_res;

    if (p_client == NULL)
    {
        return APP_RESULT_APP_ERR;
    }

    read_res.conn_handle = conn_handle;
    read_res.srv_instance_id = read_result->srv_instance_id;
    read_res.cause = read_result->cause;
    read_res.uuid = read_result->char_uuid.p.uuid16;

    if (read_result->cause != GATT_SUCCESS)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]read_obj_type_cb: error: %d", read_result->cause);
        return APP_RESULT_APP_ERR;
    }

    p_client->ots_client_cb[read_result->srv_instance_id].cur_obj.type.uuid_size =
        read_result->value_size;
    memcpy(p_client->ots_client_cb[read_result->srv_instance_id].cur_obj.type.uu.char_uuid128,
           read_result->p_value, read_result->value_size);

    read_res.value.obj_type.uuid_size = read_result->value_size;
    memcpy(read_res.value.obj_type.uu.char_uuid128,
           read_result->p_value, read_result->value_size);
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_OTS_CLIENT_READ_RES, &read_res);
    return APP_RESULT_SUCCESS;
}

static T_APP_RESULT read_obj_size_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                     void *p_data)
{
    T_GATT_CLIENT_DATA *p = (T_GATT_CLIENT_DATA *) p_data;
    T_GATT_CLIENT_READ_RESULT *read_result = &p->read_result;
    T_OTS_CLIENT_DB *p_client = ots_check_link_int(conn_handle);
    T_OTS_CLT_READ_RES read_res;

    if (p_client == NULL)
    {
        return APP_RESULT_APP_ERR;
    }

    read_res.conn_handle = conn_handle;
    read_res.srv_instance_id = read_result->srv_instance_id;
    read_res.cause = read_result->cause;
    read_res.uuid = read_result->char_uuid.p.uuid16;

    if (read_result->cause != GATT_SUCCESS)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]read_obj_size_cb: error: %d", read_result->cause);
        return APP_RESULT_APP_ERR;
    }

    if (read_result->value_size != 8)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]read_obj_size_cb: wrong length: %d", read_result->value_size);
        return APP_RESULT_APP_ERR;
    }

    memcpy((uint8_t *)p_client->ots_client_cb[read_result->srv_instance_id].cur_obj.size,
           read_result->p_value, read_result->value_size);

    memcpy((uint8_t *)&read_res.value.size,
           read_result->p_value, read_result->value_size);
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_OTS_CLIENT_READ_RES, &read_res);

    return APP_RESULT_SUCCESS;

}

static T_APP_RESULT read_obj_id_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                   void *p_data)
{
    T_GATT_CLIENT_DATA *p = (T_GATT_CLIENT_DATA *) p_data;
    T_GATT_CLIENT_READ_RESULT *read_result = &p->read_result;
    T_OTS_CLIENT_DB *p_client = ots_check_link_int(conn_handle);
    T_OTS_CLT_READ_RES read_res;

    if (p_client == NULL)
    {
        return APP_RESULT_APP_ERR;
    }

    read_res.conn_handle = conn_handle;
    read_res.srv_instance_id = read_result->srv_instance_id;
    read_res.cause = read_result->cause;
    read_res.uuid = read_result->char_uuid.p.uuid16;

    if (read_result->cause != GATT_SUCCESS)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]read_obj_id_cb: error: %d", read_result->cause);
        return APP_RESULT_APP_ERR;
    }

    p_client->ots_client_cb[read_result->srv_instance_id].cur_obj.id = 0;
    if (read_result->value_size == 6)
    {
        memcpy((uint8_t *)&p_client->ots_client_cb[read_result->srv_instance_id].cur_obj.id,
               read_result->p_value, read_result->value_size);
    }

    memcpy((uint8_t *)&read_res.value.id, read_result->p_value, read_result->value_size);
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_OTS_CLIENT_READ_RES, &read_res);

    return APP_RESULT_SUCCESS;
}

static T_APP_RESULT read_obj_props_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                      void *p_data)
{
    T_GATT_CLIENT_DATA *p = (T_GATT_CLIENT_DATA *) p_data;
    T_GATT_CLIENT_READ_RESULT *read_result = &p->read_result;
    T_OTS_CLIENT_DB *p_client = ots_check_link_int(conn_handle);
    T_OTS_CLT_READ_RES read_res;

    if (p_client == NULL)
    {
        return APP_RESULT_APP_ERR;
    }

    read_res.conn_handle = conn_handle;
    read_res.srv_instance_id = read_result->srv_instance_id;
    read_res.cause = read_result->cause;
    read_res.uuid = read_result->char_uuid.p.uuid16;

    if (read_result->cause != GATT_SUCCESS)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]read_obj_size_cb: error: %d", read_result->cause);
        return APP_RESULT_APP_ERR;
    }

    if (read_result->value_size == 4)
    {
        memcpy((uint8_t *)&p_client->ots_client_cb[read_result->srv_instance_id].cur_obj.props,
               read_result->p_value, read_result->value_size);
    }

    memcpy((uint8_t *)&read_res.value.props, read_result->p_value, read_result->value_size);
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_OTS_CLIENT_READ_RES, &read_res);

    return APP_RESULT_SUCCESS;
}

static T_APP_RESULT oacp_read_obj_size_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                          void *p_data)
{
    T_GATT_CLIENT_DATA *p = (T_GATT_CLIENT_DATA *) p_data;
    T_GATT_CLIENT_READ_RESULT *read_result = &p->read_result;
    T_OTS_CLIENT_DB *p_client = ots_check_link_int(conn_handle);
    T_APP_RESULT res = APP_RESULT_SUCCESS;

    if (p_client == NULL)
    {
        return APP_RESULT_APP_ERR;
    }

    if (read_result->cause != GATT_SUCCESS)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]oacp_read_obj_size_cb: error: %d", read_result->cause);
        res =  APP_RESULT_APP_ERR;
    }
    else
    {
        if (read_result->value_size != 8)
        {
            PROTOCOL_PRINT_ERROR1("[OTS]oacp_read_obj_size_cb: wrong length: %d", read_result->value_size);
            res = APP_RESULT_APP_ERR;
        }
        //FIX TODO
        //we can check size here, or just let server check offset and length
        if (p_client->oacp_op_cb.value.oacp_read.length == 0)   // if it is 0, set length to current size
        {
            p_client->oacp_op_cb.value.oacp_read.length = *(uint32_t *)read_result->p_value;
            if (p_client->oacp_op_cb.value.oacp_read.length == 0)
            {
                if (p_client->oacp_op_cb.p_ots_oacp_cb)
                {
                    p_client->oacp_op_cb.p_ots_oacp_cb(p_client->conn_handle, OTS_OACP_READ_OP, OACP_SUCCESS_RES_CODE,
                                                       NULL, 0);
                }
                memset((uint8_t *)&p_client->oacp_op_cb, 0, sizeof(T_HANDLE_OACP_CB));
                return APP_RESULT_SUCCESS;
            }
        }

    }

    if (res == APP_RESULT_SUCCESS)
    {
        uint8_t conn_id;
        le_get_conn_id_by_handle(conn_handle, &conn_id);
        memcpy((uint8_t *)p_client->ots_client_cb[read_result->srv_instance_id].cur_obj.size,
               read_result->p_value, read_result->value_size);
        if (!le_coc_register_cback(conn_id, ots_client_coc_callback, read_result->srv_instance_id,
                                   false))
        {
            res = APP_RESULT_APP_ERR;
        }
    }

    if (res != APP_RESULT_SUCCESS)
    {
        if (p_client->oacp_op_cb.oacp_opcode == OTS_OACP_WRITE_OP)
        {
            if (p_client->oacp_op_cb.value.oacp_write.length > 0)
            {
                ble_audio_mem_free(p_client->oacp_op_cb.value.oacp_write.p_value);
            }
        }
        if (p_client->oacp_op_cb.p_ots_oacp_cb)
        {
            p_client->oacp_op_cb.p_ots_oacp_cb(conn_handle, p_client->oacp_op_cb.oacp_opcode,
                                               OACP_PROCE_NOT_PERM_RES_CODE,
                                               NULL, 0);
        }
        memset((uint8_t *)&p_client->oacp_op_cb, 0, sizeof(T_HANDLE_OACP_CB));
    }

    return res;
}

static bool oacp_read_obj_size(uint8_t srv_instance_id, uint16_t conn_handle)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    uint16_t handle = 0;

    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = OBJECT_TRANS_SERVICE_UUID;
    srv_uuid.instance_id = srv_instance_id;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = OTS_OBJ_SIZE_CHAR_UUID;

    PROTOCOL_PRINT_TRACE1("[OTS]oacp_read_obj_size conn_handle: 0x%x, uuid: 0x%x", conn_handle);

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        if (gatt_client_read(conn_handle, handle, oacp_read_obj_size_cb) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
    }

    return false;
}

void ots_cfg_cccd(uint8_t srv_instance_id, uint16_t conn_handle, uint8_t cfg_flags, bool enable)
{
    T_OTS_CLIENT_DB *p_client = ots_check_link_int(conn_handle);
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = OBJECT_TRANS_SERVICE_UUID;
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
        if (cfg_flags & OTS_OACP_CCCD_FLAG)
        {
            char_uuid.p.uuid16 = OTS_OACP_CHAR_UUID;
            cfg_flags &= ~OTS_OACP_CCCD_FLAG;
        }
        else if (cfg_flags & OTS_OLCP_CCCD_FLAG)
        {
            char_uuid.p.uuid16 = OTS_OLCP_CHAR_UUID;
            cfg_flags &= ~OTS_OLCP_CCCD_FLAG;
        }
        else if (cfg_flags & OTS_OBJ_CHG_CCCD_FLAG)
        {
            char_uuid.p.uuid16 = OTS_OBJ_CHANGE_CHAR_UUID;
            cfg_flags &= ~OTS_OBJ_CHG_CCCD_FLAG;
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

void ots_read_char(uint8_t srv_instance_id, uint16_t conn_handle, uint16_t uuid,
                   uint8_t char_inst_id)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    uint16_t handle = 0;

    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = OBJECT_TRANS_SERVICE_UUID;
    srv_uuid.instance_id = srv_instance_id;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = char_inst_id;
    char_uuid.p.uuid16 = uuid;

    PROTOCOL_PRINT_TRACE2("[OTS]ots_read_char conn_handle: 0x%x, uuid: 0x%x", conn_handle, uuid);

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        switch (uuid)
        {
        case OTS_FEATURE_CHAR_UUID:
            gatt_client_read(conn_handle, handle, read_features_cb);
            break;

        case OTS_OBJ_NAME_CHAR_UUID:
            gatt_client_read(conn_handle, handle, read_obj_name_cb);
            break;

        case OTS_OBJ_TYPE_CHAR_UUID:
            gatt_client_read(conn_handle, handle, read_obj_type_cb);
            break;

        case OTS_OBJ_SIZE_CHAR_UUID:
            gatt_client_read(conn_handle, handle, read_obj_size_cb);
            break;

        case OTS_OBJ_FIRST_CR_CHAR_UUID:
        case OTS_OBJ_LAST_MOD_CHAR_UUID:
            gatt_client_read(conn_handle, handle, NULL);
            break;

        case OTS_OBJ_ID_CHAR_UUID:
            gatt_client_read(conn_handle, handle, read_obj_id_cb);
            break;

        case OTS_OBJ_PROPS_CHAR_UUID:
            gatt_client_read(conn_handle, handle, read_obj_props_cb);
            break;

        case OTS_OLF_CHAR_UUID:
            gatt_client_read(conn_handle, handle, NULL);
            break;
        default:
            break;
        }

    }
}


static void free_pending_oacp_op(T_OTS_CLIENT_DB *p_client)
{
    if (p_client->oacp_op_cb.oacp_opcode == OTS_OACP_READ_OP &&
        p_client->oacp_op_cb.value.oacp_read.p_value != NULL)
    {
        ble_audio_mem_free(p_client->oacp_op_cb.value.oacp_read.p_value);
    }
    else if (p_client->oacp_op_cb.oacp_opcode == OTS_OACP_WRITE_OP &&
             p_client->oacp_op_cb.value.oacp_write.p_value != NULL)
    {
        ble_audio_mem_free(p_client->oacp_op_cb.value.oacp_write.p_value);
    }
    memset((uint8_t *)&p_client->oacp_op_cb, 0, sizeof(T_HANDLE_OACP_CB));
}

static void send_pending_oacp_op(T_OTS_CLIENT_DB *p_client)
{
    if (p_client == NULL)
    {
        return;
    }
    uint8_t data[10] = {0};
    uint16_t data_len = 1;
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = p_client->oacp_op_cb.srv_instance_id;
    srv_uuid.p.uuid16 = OBJECT_TRANS_SERVICE_UUID;
    data[0] = p_client->oacp_op_cb.oacp_opcode;

    if (p_client->oacp_op_cb.oacp_opcode == OTS_OACP_READ_OP)
    {
        data_len += 8;
        memcpy(&data[1], (uint8_t *)&p_client->oacp_op_cb.value.oacp_read.offset, 4);
        memcpy(&data[5], (uint8_t *)&p_client->oacp_op_cb.value.oacp_read.length, 4);
        if (p_client->oacp_op_cb.value.oacp_read.length > 0)
        {
            p_client->oacp_op_cb.value.oacp_read.p_value = ble_audio_mem_zalloc(
                                                               p_client->oacp_op_cb.value.oacp_read.length);
            if (p_client->oacp_op_cb.value.oacp_read.p_value == NULL)
            {
                PROTOCOL_PRINT_ERROR0("[OTS]send_pending_oacp_op alloc p_value fail!");
                goto handle_error;
            }
        }
    }
    else if (p_client->oacp_op_cb.oacp_opcode == OTS_OACP_WRITE_OP)
    {
        data_len += 9;
        memcpy(&data[1], (uint8_t *)&p_client->oacp_op_cb.value.oacp_write.offset, 4);
        memcpy(&data[5], (uint8_t *)&p_client->oacp_op_cb.value.oacp_write.length, 4);
        data[8] = p_client->oacp_op_cb.value.oacp_write.mode;
    }
    else
    {
        le_coc_disconnect(p_client->cid);
        return;
    }
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = OTS_OACP_CHAR_UUID;


    if (gatt_client_find_char_handle(p_client->conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(p_client->conn_handle, GATT_WRITE_TYPE_REQ, handle, data_len, data, NULL);
    }

handle_error:
    if (cause != GAP_CAUSE_SUCCESS)
    {
        if (p_client->oacp_op_cb.p_ots_oacp_cb)
        {
            p_client->oacp_op_cb.p_ots_oacp_cb(p_client->conn_handle, p_client->oacp_op_cb.oacp_opcode,
                                               OACP_PROCE_NOT_PERM_RES_CODE,
                                               NULL, 0);
        }
        free_pending_oacp_op(p_client);
        le_coc_disconnect(p_client->cid);
    }
}

static void handle_recv_coc_data(T_OTS_CLIENT_DB *p_client, uint8_t *p_value, uint16_t len)
{
    uint8_t *pp = p_client->oacp_op_cb.value.oacp_read.p_value;
    if (p_client->oacp_op_cb.oacp_opcode == OTS_OACP_READ_OP)
    {
        if ((p_client->oacp_op_cb.wr_len + len) > p_client->oacp_op_cb.value.oacp_read.length)
        {
            PROTOCOL_PRINT_ERROR0("[OTS]handle_recv_coc_data recv data length too long");
            len = p_client->oacp_op_cb.value.oacp_read.length - p_client->oacp_op_cb.wr_len;
        }
        if (pp != NULL)
        {
            memcpy(&pp[p_client->oacp_op_cb.wr_len], p_value, len);
        }
        p_client->oacp_op_cb.wr_len += len;
        PROTOCOL_PRINT_TRACE2("[OTS]handle_recv_coc_data wr_len: %d, read_length: %d",
                              p_client->oacp_op_cb.wr_len,
                              p_client->oacp_op_cb.value.oacp_read.length);
        if (p_client->oacp_op_cb.wr_len == p_client->oacp_op_cb.value.oacp_read.length)
        {
            if (p_client->oacp_op_cb.p_ots_oacp_cb)
            {
                p_client->oacp_op_cb.p_ots_oacp_cb(p_client->conn_handle, OTS_OACP_READ_OP, OACP_SUCCESS_RES_CODE,
                                                   pp, p_client->oacp_op_cb.value.oacp_read.length);
                free_pending_oacp_op(p_client);
                le_coc_disconnect(p_client->cid);
            }
        }
    }
}

static void handle_send_coc_data(T_OTS_CLIENT_DB *p_client, uint16_t cause, uint8_t credit)
{
    uint8_t *pp = p_client->oacp_op_cb.value.oacp_write.p_value;
    T_GAP_CAUSE res;
    uint16_t  send_len;

    if (p_client->oacp_op_cb.oacp_opcode == OTS_OACP_WRITE_OP)
    {
        if (cause != 0 || credit == 0)
        {
            PROTOCOL_PRINT_ERROR2("[OTS]handle_send_coc_data error cause: 0x%x, credit:%d", cause, credit);
            goto send_error;
        }

        send_len = (p_client->oacp_op_cb.wr_len + p_client->tx_mtu <
                    p_client->oacp_op_cb.value.oacp_write.length) ?
                   p_client->tx_mtu : (p_client->oacp_op_cb.value.oacp_write.length - p_client->oacp_op_cb.wr_len);
        res = le_coc_send_data(p_client->cid, &pp[p_client->oacp_op_cb.wr_len], send_len);
        if (res != GAP_CAUSE_SUCCESS)
        {
            PROTOCOL_PRINT_ERROR1("[OTS]handle_send_coc_data coc_send data fail: 0x%x", res);
            goto send_error;
        }

        p_client->oacp_op_cb.wr_len += send_len;

        if (send_len == 0)
        {
            if (p_client->oacp_op_cb.p_ots_oacp_cb)
            {
                p_client->oacp_op_cb.p_ots_oacp_cb(p_client->conn_handle, OTS_OACP_READ_OP, OACP_SUCCESS_RES_CODE,
                                                   pp, p_client->oacp_op_cb.value.oacp_write.length);
                free_pending_oacp_op(p_client);
                le_coc_disconnect(p_client->cid);
            }
        }
    }

    return;
send_error:
    p_client->oacp_op_cb.p_ots_oacp_cb(p_client->conn_handle, OTS_OACP_WRITE_OP,
                                       OACP_CHAN_UNAV_RES_CODE,
                                       NULL, 0);
    free_pending_oacp_op(p_client);
    le_coc_disconnect(p_client->cid);
}


static T_APP_RESULT ots_client_cback(uint16_t conn_handle, T_GATT_CLIENT_EVENT type, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;
    T_OTS_CLIENT_DB *p_ots_db = ots_check_link_int(conn_handle);

    if (p_ots_db == NULL && type != GATT_CLIENT_EVENT_DIS_DONE)
    {
        goto error;
    }

    switch (type)
    {
    case GATT_CLIENT_EVENT_DIS_DONE:
        {
            if (p_client_cb_data->dis_done.is_found)
            {
                T_BLE_AUDIO_LINK *p_link = NULL;
                uint8_t srv_num = p_client_cb_data->dis_done.srv_instance_num;
                T_OTS_CLIENT_DIS_DONE dis_done;
                p_link = ble_audio_link_find_by_conn_handle(conn_handle);
                if (p_link == NULL)
                {
                    goto error;
                }

                if (p_link->p_ots_client)
                {
                    ble_audio_mem_free(p_link->p_ots_client);
                }
                p_link->p_ots_client = ble_audio_mem_zalloc(srv_num * sizeof(T_OTS_SRV_CB) + sizeof(
                                                                T_OTS_CLIENT_DB));

                if (p_link->p_ots_client == NULL)
                {
                    goto error;
                }
                T_OTS_CLIENT_DB *p_ots_client = (T_OTS_CLIENT_DB *) p_link->p_ots_client;
                p_ots_client->conn_handle = conn_handle;
                p_ots_client->srv_num = srv_num;

                dis_done.conn_handle = conn_handle;
                dis_done.is_found = p_client_cb_data->dis_done.is_found;
                dis_done.ots_srv_num = srv_num;
                dis_done.load_from_ftl = p_client_cb_data->dis_done.load_from_ftl;
                ble_audio_mgr_dispatch(LE_AUDIO_MSG_OTS_CLIENT_DIS_DONE, &dis_done);

                for (int i = 0; i < p_client_cb_data->dis_done.srv_instance_num; i ++)
                {
                    ots_read_char(i, conn_handle, OTS_FEATURE_CHAR_UUID, 0);
                    ots_cfg_cccd(i, conn_handle, OTS_OACP_CCCD_FLAG | OTS_OLCP_CCCD_FLAG, true);
                }
            }
        }
        break;

    case GATT_CLIENT_EVENT_CCCD_CFG:
        {
            ble_audio_mgr_dispatch_client_attr_cccd_info(conn_handle, &p_client_cb_data->cccd_cfg);
        }
        break;

    case GATT_CLIENT_EVENT_READ_RESULT:
        {
            T_OTS_CLT_READ_RES read_res;
            read_res.conn_handle = conn_handle;
            read_res.srv_instance_id = p_client_cb_data->read_result.srv_instance_id;
            read_res.cause = p_client_cb_data->read_result.cause;
            read_res.uuid = p_client_cb_data->read_result.char_uuid.p.uuid16;
            if (p_client_cb_data->read_result.char_uuid.p.uuid16 == OTS_OLF_CHAR_UUID)
            {
                read_res.value.olf_param_cb.filt_type = p_client_cb_data->read_result.p_value[0];
                if (read_res.value.olf_param_cb.filt_type < OTS_OBJ_TYPE)
                {
                    read_res.value.olf_param_cb.value.olf_name.p_name = p_client_cb_data->read_result.p_value + 1;
                    read_res.value.olf_param_cb.value.olf_name.name_len = p_client_cb_data->read_result.value_size - 1;
                }
                else if (read_res.value.olf_param_cb.filt_type == OTS_OBJ_TYPE)
                {
                    read_res.value.olf_param_cb.value.char_uuid.uuid_size = p_client_cb_data->read_result.value_size -
                                                                            1;
                    memcpy(read_res.value.olf_param_cb.value.char_uuid.uu.char_uuid128,
                           p_client_cb_data->read_result.p_value + 1, p_client_cb_data->read_result.value_size - 1);
                }
                else if (read_res.value.olf_param_cb.filt_type < OTS_CUR_SIZE_BETWEEN)
                {
                    break;
                }
                else if (read_res.value.olf_param_cb.filt_type < OTS_MARKED_OBJS)
                {
                    memcpy((uint8_t *)&read_res.value.olf_param_cb.value.size,
                           p_client_cb_data->read_result.p_value + 1, 8);
                }
                ble_audio_mgr_dispatch(LE_AUDIO_MSG_OTS_CLIENT_OLCP_INFO, &read_res);
            }
        }
        break;

    case GATT_CLIENT_EVENT_NOTIFY_IND:
        {
            if (p_client_cb_data->notify_ind.char_uuid.p.uuid16 == OTS_OACP_CHAR_UUID)
            {
                uint8_t opcode = p_client_cb_data->notify_ind.p_value[0];
                if (p_ots_db->oacp_op_cb.oacp_opcode != opcode)
                {
                    PROTOCOL_PRINT_ERROR2("[OTS]ots_client_cback oacp notify opcode mis-match: 0x%x : 0x%x",
                                          p_ots_db->oacp_op_cb.oacp_opcode, opcode);
                    break;
                }
                uint8_t res = p_client_cb_data->notify_ind.p_value[1];
                gatt_client_ind_confirm(p_ots_db->conn_handle, p_client_cb_data->notify_ind.cid);
                result = APP_RESULT_PENDING;
                switch (opcode)
                {
                case OTS_OACP_CREATE_OP:
                case OTS_OACP_DELETE_OP:
                case OTS_OACP_CAL_CHECKSUM_OP:
                case OTS_OACP_ABORT_OP:
                    if (p_ots_db->oacp_op_cb.p_ots_oacp_cb != NULL)
                    {
                        p_ots_db->oacp_op_cb.p_ots_oacp_cb(conn_handle, opcode, res, NULL, 0);
                    }

                    break;
                case OTS_OACP_READ_OP:
                    if (res != OACP_SUCCESS_RES_CODE)
                    {
                        if (p_ots_db->oacp_op_cb.p_ots_oacp_cb != NULL)
                        {
                            p_ots_db->oacp_op_cb.p_ots_oacp_cb(conn_handle, opcode, res, NULL, 0);
                        }
                        le_coc_disconnect(p_ots_db->cid);
                    }
                    else
                    {
                        return result;
                    }
                    break;
                case OTS_OACP_WRITE_OP:
                    if (res != OACP_SUCCESS_RES_CODE)
                    {
                        if (p_ots_db->oacp_op_cb.p_ots_oacp_cb != NULL)
                        {
                            p_ots_db->oacp_op_cb.p_ots_oacp_cb(conn_handle, opcode, res, NULL, 0);
                        }
                        le_coc_disconnect(p_ots_db->cid);
                    }
                    else
                    {
                        return result;
                    }
                    break;
                }
                free_pending_oacp_op(p_ots_db);
            }
            else if (p_client_cb_data->notify_ind.char_uuid.p.uuid16 == OTS_OLCP_CHAR_UUID)
            {
                T_OTS_SRV_CB *p_client =
                    &p_ots_db->ots_client_cb[p_client_cb_data->notify_ind.char_uuid.instance_id];
                T_OTS_CLT_OLCP_IND olcp_ind;
                gatt_client_ind_confirm(p_ots_db->conn_handle, p_client_cb_data->notify_ind.cid);
                result = APP_RESULT_PENDING;

                olcp_ind.conn_handle = conn_handle;
                olcp_ind.notify = false;
                olcp_ind.ind_res = p_client_cb_data->notify_ind.p_value[1];
                olcp_ind.instance_id = p_client_cb_data->notify_ind.char_uuid.instance_id;
                olcp_ind.olcp_opcode = p_client_cb_data->notify_ind.p_value[0];
                if (olcp_ind.olcp_opcode == OTS_OLCP_REQ_NUM_OBJS_OP)
                {
                    memcpy((uint8_t *)&olcp_ind.obj_num, &p_client_cb_data->notify_ind.p_value[2], 4);
                }
                if (p_client->p_ots_olcp_cb)
                {
                    p_client->p_ots_olcp_cb(&olcp_ind);
                }
                p_client->p_ots_olcp_cb = 0;
                p_client->olcp_opcode = 0;

                ble_audio_mgr_dispatch(LE_AUDIO_MSG_OTS_CLIENT_OLCP_INFO, &olcp_ind);

            }
        }
        break;

    case GATT_CLIENT_EVENT_WRITE_RESULT:
        {
            if (p_client_cb_data->write_result.char_uuid.p.uuid16 == OTS_OACP_CHAR_UUID)
            {
                if (p_client_cb_data->write_result.cause != GAP_CAUSE_SUCCESS)
                {
                    uint8_t oacp_opcode = p_ots_db->oacp_op_cb.oacp_opcode;
                    if (p_ots_db->oacp_op_cb.p_ots_oacp_cb)
                    {
                        p_ots_db->oacp_op_cb.p_ots_oacp_cb(p_ots_db->conn_handle, p_ots_db->oacp_op_cb.oacp_opcode,
                                                           OACP_PROCE_NOT_PERM_RES_CODE,
                                                           NULL, 0);
                    }
                    free_pending_oacp_op(p_ots_db);
                    if (oacp_opcode == OTS_OACP_READ_OP || oacp_opcode == OTS_OACP_WRITE_OP)
                    {
                        le_coc_disconnect(p_ots_db->cid);
                    }
                }
            }
        }
        break;

    default:
        break;
    }

    return result;
error:
    return APP_RESULT_APP_ERR;
}

static T_APP_RESULT ots_client_coc_callback(uint8_t coc_type, void *p_coc_data, uint8_t id)
{
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_LE_COC_DATA cb_data;
    memcpy(&cb_data, p_coc_data, sizeof(T_LE_COC_DATA));
    PROTOCOL_PRINT_TRACE1("[OTS]ots_client_coc_callback: coc_type = %d", coc_type);
    T_BLE_AUDIO_LINK *p_link;
    p_link = ble_audio_link_find_by_conn_id(cb_data.p_le_chann_state->conn_id);
    if (p_link == NULL)
    {
        return APP_RESULT_APP_ERR;
    }
    T_OTS_CLIENT_DB *p_ots_client = ((T_OTS_CLIENT_DB *)p_link->p_ots_client) + id;
    switch (coc_type)
    {
    case GAP_COC_MSG_LE_CHANN_STATE:
        PROTOCOL_PRINT_INFO4("[OTS]ots_client_coc_callback GAP_COC_MSG_LE_CHANN_STATE: conn_id %d, cid 0x%x, conn_state %d, cause 0x%x",
                             cb_data.p_le_chann_state->conn_id,
                             cb_data.p_le_chann_state->cid,
                             cb_data.p_le_chann_state->conn_state,
                             cb_data.p_le_chann_state->cause);

        if (cb_data.p_le_chann_state->conn_state == GAP_CHANN_STATE_CONNECTED)
        {
            if (p_ots_client != NULL)
            {
                le_coc_get_chann_param(COC_CHANN_PARAM_MTU, &p_ots_client->tx_mtu, cb_data.p_le_chann_state->cid);
                PROTOCOL_PRINT_INFO1("[OTS]ots_client_coc_callback GAP_CHANN_STATE_CONNECTED: mtu %d",
                                     p_ots_client->tx_mtu);
                p_ots_client->cid = cb_data.p_le_chann_state->cid;
            }
            send_pending_oacp_op(p_ots_client);
        }
        else if (cb_data.p_le_chann_state->conn_state == GAP_CHANN_STATE_DISCONNECTED)
        {
            if (p_ots_client->oacp_op_cb.oacp_opcode != 0)
            {
                if (p_ots_client->oacp_op_cb.p_ots_oacp_cb)
                {
                    p_ots_client->oacp_op_cb.p_ots_oacp_cb(p_link->conn_handle,
                                                           OTS_OACP_READ_OP, OACP_CHAN_UNAV_RES_CODE, NULL, 0);
                }
                free_pending_oacp_op(p_ots_client);
            }
            PROTOCOL_PRINT_INFO0("[OTS]ots_client_coc_callback: coc disconnected");
        }
        break;

    case GAP_COC_MSG_LE_REG_PSM:
        PROTOCOL_PRINT_INFO2("[OTS]ots_client_coc_callback GAP_COC_MSG_LE_REG_PSM: le_psm 0x%x, cause 0x%x",
                             cb_data.p_le_reg_psm_rsp->le_psm,
                             cb_data.p_le_reg_psm_rsp->cause);
        break;

    case GAP_COC_MSG_LE_SET_PSM_SECURITY:
        PROTOCOL_PRINT_INFO1("[OTS]ots_client_coc_callback GAP_COC_MSG_LE_SET_PSM_SECURITY: cause 0x%x",
                             cb_data.p_le_set_psm_security_rsp->cause);
        break;

    case GAP_COC_MSG_LE_SEND_DATA:
        PROTOCOL_PRINT_INFO4("[OTS]ots_client_coc_callback GAP_COC_MSG_LE_SEND_DATA: conn_id %d, cid 0x%x, cause 0x%x, credit %d",
                             cb_data.p_le_send_data->conn_id,
                             cb_data.p_le_send_data->cid,
                             cb_data.p_le_send_data->cause,
                             cb_data.p_le_send_data->credit);
        handle_send_coc_data(p_ots_client, cb_data.p_le_send_data->cause, cb_data.p_le_send_data->credit);
        break;

    case GAP_COC_MSG_LE_RECEIVE_DATA:
        PROTOCOL_PRINT_INFO3("[OTS]ots_client_coc_callback GAP_COC_MSG_LE_RECEIVE_DATA: conn_id %d, cid 0x%x, value_len %d",
                             cb_data.p_le_receive_data->conn_id,
                             cb_data.p_le_receive_data->cid,
                             cb_data.p_le_receive_data->value_len);
        handle_recv_coc_data(p_ots_client, cb_data.p_le_receive_data->p_data,
                             cb_data.p_le_receive_data->value_len);
        break;

    default:
        break;
    }
    return result;
}


static void ots_client_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BLE_DM_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BLE_DM_EVENT_CONN_STATE:
        {
            int i;
            T_BLE_AUDIO_LINK *p_link = NULL;
            p_link = ble_audio_link_find_by_conn_id(param->conn_state.conn_id);
            if (p_link == NULL)
            {
                PROTOCOL_PRINT_ERROR1("[OTS]mcs_client_dm_cback: not find the link, conn_id %d",
                                      param->conn_state.conn_id);
                break;
            }
            if (param->conn_state.state == GAP_CONN_STATE_DISCONNECTED)
            {
                T_OTS_CLIENT_DB *ots_client_cb = (T_OTS_CLIENT_DB *)p_link->p_ots_client;

                if (p_link->p_ots_client != NULL)
                {
                    free_pending_oacp_op(ots_client_cb);
                    for (i = 0; i < ots_client_cb->srv_num; i++)
                    {
                        if (ots_client_cb->ots_client_cb[i].cur_obj.name != NULL)
                        {
                            ble_audio_mem_free(ots_client_cb->ots_client_cb[i].cur_obj.name);
                        }
                    }
                    ble_audio_mem_free(p_link->p_ots_client);
                    p_link->p_ots_client = NULL;
                }
            }
        }
        break;

    default:
        break;
    }
}
bool ots_client_write_obj_name(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t *p_name,
                               uint16_t name_len)
{
    T_OTS_CLIENT_DB *p_client = ots_check_link_int(conn_handle);
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
    srv_uuid.p.uuid16 = OBJECT_TRANS_SERVICE_UUID;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = OTS_OBJ_NAME_CHAR_UUID;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_REQ, handle, name_len, p_name, NULL);
    }

    return (cause == GAP_CAUSE_SUCCESS) ? true : false;

}


bool ots_client_write_olf(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t char_inst_id,
                          T_OTS_OLF_PARAM *p_param)
{
    T_OTS_CLIENT_DB *p_client = ots_check_link_int(conn_handle);
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
    srv_uuid.p.uuid16 = OBJECT_TRANS_SERVICE_UUID;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = char_inst_id;
    char_uuid.p.uuid16 = OTS_OLF_CHAR_UUID;

    uint8_t   *p_data = NULL;
    uint16_t  len = 0;
    switch (p_param->filt_type)
    {
    case OTS_NO_FILTER:
    case OTS_MARKED_OBJS:
        p_data = ble_audio_mem_zalloc(1);
        if (p_data == NULL)
        {
            return false;
        }
        p_data[0] = p_param->filt_type;
        len++;
        break;

    case OTS_NAME_STARTS_WITH:
    case OTS_NAME_ENDS_WITH:
    case OTS_NAME_CONTAINS:
    case OTS_NAME_IS_EXACTLY:
        p_data = ble_audio_mem_zalloc(1 + p_param->value.olf_name.name_len);
        if (p_data == NULL)
        {
            return false;
        }
        p_data[0] = p_param->filt_type;
        len++;
        memcpy(p_data + 1, p_param->value.olf_name.p_name, p_param->value.olf_name.name_len);
        len += p_param->value.olf_name.name_len;
        break;

    case OTS_OBJ_TYPE:
        p_data = ble_audio_mem_zalloc(1 + p_param->value.char_uuid.uuid_size);
        if (p_data == NULL)
        {
            return false;
        }
        p_data[0] = p_param->filt_type;
        memcpy(p_data + 1, p_param->value.char_uuid.uu.char_uuid128, p_param->value.char_uuid.uuid_size);
        len = 1 + p_param->value.char_uuid.uuid_size;
        break;

    case OTS_CREATE_BETWEEN_TMS:
    case OTS_MOD_BETWEEN_TMS:

        return false;

    case OTS_CUR_SIZE_BETWEEN:
    case OTS_ALLOC_SIZE_BETWEEN:
        p_data = ble_audio_mem_zalloc(1 + 8);
        if (p_data == NULL)
        {
            return false;
        }
        p_data[0] = p_param->filt_type;

        memcpy(p_data + 1, (uint8_t *)p_param->value.size, 8);
        len = 9;

        break;
    }

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_REQ, handle, len, p_data, NULL);
    }
    ble_audio_mem_free(p_data);
    return (cause == GAP_CAUSE_SUCCESS) ? true : false;
}

bool ots_client_oacp_op(uint16_t conn_handle, uint8_t srv_instance_id, T_OTS_OACP_PARAM *p_param)
{
    T_OTS_CLIENT_DB *p_client = ots_check_link_int(conn_handle);
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
    srv_uuid.p.uuid16 = OBJECT_TRANS_SERVICE_UUID;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = OTS_OACP_CHAR_UUID;


    uint8_t data[21] = {0};
    data[0] = p_param->opcode;
    uint16_t data_len = 1;

    //The oacp operation should be done one by one, except abort opcode
    if (p_client->oacp_op_cb.oacp_opcode &&
        p_param->opcode != OTS_OACP_ABORT_OP)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]ots_client_oacp_op: already has a oacp operation: 0x%x",
                              p_client->oacp_op_cb.oacp_opcode);
        return false;
    }
    switch (p_param->opcode)
    {
    case OTS_OACP_CREATE_OP:
        {
            if ((p_client->ots_client_cb[srv_instance_id].ots_features[0] & (1 << OTS_OACP_CREATE_FEAT_MSK)) ==
                0)
            {
                return false;
            }

            memcpy(&data[1], (uint8_t *)&p_param->value.oacp_create.size, 4);
            data_len += 4;
            memcpy(&data[5], p_param->value.oacp_create.gatt_uuid.uu.char_uuid128, 16);
            data_len += p_param->value.oacp_create.gatt_uuid.uuid_size;
        }
        break;
    case OTS_OACP_DELETE_OP:
        if ((p_client->ots_client_cb[srv_instance_id].ots_features[0] & (1 << OTS_OACP_DELETE_FEAT_MSK)) ==
            0)
        {
            return false;
        }
        break;

    case OTS_OACP_CAL_CHECKSUM_OP:
        {
            if ((p_client->ots_client_cb[srv_instance_id].ots_features[0] & (1 <<
                                                                             OTS_OACP_CAL_CHECKSUM_FEAT_MSK)) == 0)
            {
                return false;
            }
            memcpy(&data[1], (uint8_t *)&p_param->value.oacp_chksum.offset, 4);
            data_len += 4;
            memcpy(&data[5], (uint8_t *)&p_param->value.oacp_chksum.length, 4);
            data_len += 4;
        }
        break;

    case OTS_OACP_EXECUTE_OP:
        {
            if ((p_client->ots_client_cb[srv_instance_id].ots_features[0] & (1 << OTS_OACP_EXECUTE_FEAT_MSK)) ==
                0)
            {
                return false;
            }
            memcpy(&data[1], (uint8_t *)p_param->value.oacp_exc.p_value, p_param->value.oacp_exc.length);
            data_len += p_param->value.oacp_exc.length;
        }
        break;

    case OTS_OACP_READ_OP:
        {
            if ((p_client->ots_client_cb[srv_instance_id].ots_features[0] & (1 << OTS_OACP_READ_FEAT_MSK)) == 0)
            {
                return false;
            }
            p_client->oacp_op_cb.oacp_opcode = OTS_OACP_READ_OP;
            p_client->oacp_op_cb.p_ots_oacp_cb = (P_OTS_OACP_CB)p_param->p_oacp_cb;
            p_client->oacp_op_cb.value.oacp_read = p_param->value.oacp_read;
            p_client->oacp_op_cb.srv_instance_id = srv_instance_id;

            return oacp_read_obj_size(srv_instance_id, conn_handle);
        }

    case OTS_OACP_WRITE_OP:
        {
            uint8_t *pp;
            uint8_t conn_id;

            if ((p_client->ots_client_cb[srv_instance_id].ots_features[0] & (1 << OTS_OACP_WRITE_FEAT_MSK)) ==
                0)
            {
                return false;
            }

            pp = ble_audio_mem_zalloc(sizeof(p_param->value.oacp_write.length));
            p_client->oacp_op_cb.value.oacp_write.p_value = pp;
            if (p_client->oacp_op_cb.value.oacp_write.p_value == NULL)
            {
                PROTOCOL_PRINT_ERROR0("[OTS]ots_client_oacp_op: alloc write buffer fail!");
                return false;
            }
            le_get_conn_id_by_handle(conn_handle, &conn_id);
            memcpy(pp, p_param->value.oacp_write.p_value, p_param->value.oacp_write.length);
            p_client->oacp_op_cb.oacp_opcode = OTS_OACP_WRITE_OP;
            p_client->oacp_op_cb.p_ots_oacp_cb = (P_OTS_OACP_CB)p_param->p_oacp_cb;
            p_client->oacp_op_cb.value.oacp_write = p_param->value.oacp_write;
            p_client->oacp_op_cb.srv_instance_id = srv_instance_id;
            return le_coc_register_cback(conn_id, ots_client_coc_callback, srv_instance_id, false);
        }

    case OTS_OACP_ABORT_OP:
        if ((p_client->ots_client_cb[srv_instance_id].ots_features[0] & (1 << OTS_OACP_ABORT_FEAT_MSK)) ==
            0)
        {
            return false;
        }

        if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
        {
            cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_REQ, handle, data_len, data, NULL);
        }

        return cause == GAP_CAUSE_SUCCESS ? true : false;

    default:
        return false;
    }

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_REQ, handle, data_len, data, NULL);
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        p_client->oacp_op_cb.oacp_opcode = p_param->opcode;
        p_client->oacp_op_cb.p_ots_oacp_cb = (P_OTS_OACP_CB)p_param->p_oacp_cb;
        p_client->oacp_op_cb.srv_instance_id = srv_instance_id;
        return true;
    }

    return false;
}

bool ots_client_olcp_op(uint16_t conn_handle, uint8_t srv_instance_id, T_OTS_OLCP_PARAM *p_param)
{
    T_OTS_CLIENT_DB *p_client = ots_check_link_int(conn_handle);
    if (p_client == NULL)
    {
        return false;
    }

    if (p_client->ots_client_cb[srv_instance_id].olcp_opcode != 0)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]ots_client_olcp_op: already has a olcp operation 0x%x",
                              p_client->ots_client_cb[srv_instance_id].olcp_opcode);
        return false;
    }

    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = srv_instance_id;
    srv_uuid.p.uuid16 = OBJECT_TRANS_SERVICE_UUID;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = OTS_OLCP_CHAR_UUID;

    uint8_t data[7] = {0};
    data[0] = p_param->opcode;
    uint16_t data_len = 1;
    PROTOCOL_PRINT_ERROR2("[OTS]ots_client_olcp_op: srv_instance_id: %d, olcp feature: 0x%x",
                          srv_instance_id,
                          p_client->ots_client_cb[srv_instance_id].ots_features[1]);

    switch (p_param->opcode)
    {
    case OTS_OLCP_FIRST_OP:
    case OTS_OLCP_LAST_OP:
    case OTS_OLCP_PREVIOUS_OP:
    case OTS_OLCP_NEXT_OP:
        break;
    case OTS_OLCP_REQ_NUM_OBJS_OP:
        if ((p_client->ots_client_cb[srv_instance_id].ots_features[1] & (1 <<
                                                                         OTS_OLCP_REQ_NUM_OBJ_FEAT_MSK)) == 0)
        {
            PROTOCOL_PRINT_ERROR0("[OTS]ots_client_olcp_op: not support request num obj opcode");
            return false;
        }
        break;
    case OTS_OLCP_CLR_MARK_OP:
        if ((p_client->ots_client_cb[srv_instance_id].ots_features[1] & (1 << OTS_OLCP_CLR_MARK_FEAT_MSK))
            == 0)
        {
            PROTOCOL_PRINT_ERROR0("[OTS]ots_client_olcp_op: not support clear mark opcode");
            return false;
        }
        break;
    case OTS_OLCP_GOTO_OP:
        if ((p_client->ots_client_cb[srv_instance_id].ots_features[1] & (1 << OTS_OLCP_GOTO_FEAT_MSK)) == 0)
        {
            PROTOCOL_PRINT_ERROR0("[OTS]ots_client_olcp_op: not support goto opcode");
            return false;
        }
        memcpy(&data[1], (uint8_t *)&p_param->value.obj_id, OTS_OBJ_ID_LEN);
        data_len += OTS_OBJ_ID_LEN;
        break;
    case OTS_OLCP_ORDER_OP:
        if ((p_client->ots_client_cb[srv_instance_id].ots_features[1] & (1 << OTS_OLCP_ORDER_FEAT_MSK)) ==
            0)
        {
            PROTOCOL_PRINT_ERROR0("[OTS]ots_client_olcp_op: not support order opcode");
            return false;
        }
        data[1] = p_param->value.list_sort_order;
        data_len++;
        break;
    }
    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_REQ, handle, data_len, data, NULL);
    }
    if (cause == GAP_CAUSE_SUCCESS)
    {
        p_client->ots_client_cb[srv_instance_id].olcp_opcode = p_param->opcode;
        p_client->ots_client_cb[srv_instance_id].p_ots_olcp_cb = (P_OTS_OLCP_CB)p_param->p_olcp_cb;
        return true;
    }
    return false;
}

bool ots_client_init(void)
{
    T_ATTR_UUID srv_uuid = {0};
    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = OBJECT_TRANS_SERVICE_UUID;
    gatt_client_spec_register(&srv_uuid, ots_client_cback);
    ble_dm_cback_register(ots_client_dm_cback);
    le_coc_enable();
    return true;
}

#if LE_AUDIO_DEINIT
void ots_client_deinit(void)
{
    for (uint8_t k = 0; k < ble_audio_db.acl_link_num; k++)
    {
        if (ble_audio_db.le_link[k].p_ots_client != NULL)
        {
            T_OTS_CLIENT_DB *p_client = (T_OTS_CLIENT_DB *)ble_audio_db.le_link[k].p_ots_client;
            free_pending_oacp_op(p_client);

            for (uint8_t i = 0; i < p_client->srv_num; i++)
            {
                if (p_client->ots_client_cb[i].cur_obj.name != NULL)
                {
                    ble_audio_mem_free(p_client->ots_client_cb[i].cur_obj.name);
                }
            }
            ble_audio_mem_free(p_client);
            ble_audio_db.le_link[k].p_ots_client = NULL;
        }
    }
}
#endif
#endif
