#include <stdint.h>
#include <string.h>
#include "trace.h"
#include "bt_types.h"
#include "ble_audio_dm.h"
#include "ble_audio_mgr.h"
#include "ble_link_util.h"
#include "ble_audio_ual.h"
#include "has_mgr.h"
#include "has_int.h"

/* base on HAS_v09 */
#if LE_AUDIO_HAS_SUPPORT
#define HAS_MAX_CTL_PNT_VALUE                       42
#define HAS_HEARING_AID_FEATURES_CCCD_FLAG          0x0001
#define HAS_HEARING_CP_CCCD_FLAG                    0x0002
#define HAS_ACTIVE_PRESET_IDX_CCCD_FLAG             0x0004

#define HAS_CTL_PNT_OPERATE_ACTIVE(x)                     \
    ( (x == HAS_CP_OP_READ_PRESETS_REQ))


typedef struct
{
    T_SERVER_ID       service_id;
    uint8_t           attr_feature;
    T_HAS_HA_FEATURES hearing_aid_features;
    uint8_t           active_preset_idx;
    bool              is_ha_cp_notify;
    uint8_t           ctrl_pnt_param[HAS_MAX_CTL_PNT_VALUE];
    uint16_t          attr_num;
    T_ATTRIB_APPL     *p_attr_tbl;
} T_HAS;


T_HAS *p_has = NULL;

/**< @brief  profile/service definition.  */
const T_ATTRIB_APPL has_attr_tbl[] =
{
    /*----------------- Hearing Access Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_HAS),                 /* service UUID */
            HI_WORD(GATT_UUID_HAS)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },
    /* <<Characteristic>>, ..Hearing aid features */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ |                  /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* Hearing aid features value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(HAS_UUID_CHAR_HA_FEATURES),
            HI_WORD(HAS_UUID_CHAR_HA_FEATURES)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ |             /* permissions */
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)
    },
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ)          /* permissions */
    }
};

const T_ATTRIB_APPL has_cp_char_tbl[] =
{
    /* <<Characteristic>>, .. Hearing aid preset control point*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_WRITE |                  /* characteristic properties */
#if F_BT_5_2_EATT_SUPPORT
             GATT_CHAR_PROP_NOTIFY |
#endif
             GATT_CHAR_PROP_INDICATE)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* Hearing aid preset control point value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(HAS_UUID_CHAR_HA_PRESET_CP),
            HI_WORD(HAS_UUID_CHAR_HA_PRESET_CP)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_WRITE_ENCRYPTED_REQ |             /* permissions */
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)
    },
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ)          /* permissions */
    },
    /* <<Characteristic>>, ..Active preset index */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ |                  /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* Active preset index value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(HAS_UUID_CHAR_ACTIVE_PRESET_IDX),
            HI_WORD(HAS_UUID_CHAR_ACTIVE_PRESET_IDX)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ |             /* permissions */
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)
    },
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ)          /* permissions */
    }
};

uint16_t has_get_attr_idx(uint16_t uuid)
{
    T_CHAR_UUID char_uuid;
    uint16_t attrib_idx = 0;
    char_uuid.index = 1;
    char_uuid.uuid_size = UUID_16BIT_SIZE;
    char_uuid.uu.char_uuid16 = uuid;

    if (!gatt_svc_find_char_index_by_uuid(p_has->p_attr_tbl, char_uuid,
                                          p_has->attr_num,
                                          &attrib_idx))
    {
        PROFILE_PRINT_ERROR0("has_get_attr_idx: idx = 0");
        return 0;
    }

    return attrib_idx;
}

void has_clear_cp_flag(void)
{
    p_has->ctrl_pnt_param[0] = HAS_CP_OP_RESERVED;
    return;
}

bool has_send_hearing_aid_feature_notify(uint16_t conn_handle)
{
    if (p_has == NULL)
    {
        return false;
    }

    return gatt_svc_send_data(conn_handle, L2C_FIXED_CID_ATT, p_has->service_id,
                              has_get_attr_idx(HAS_UUID_CHAR_HA_FEATURES),
                              (uint8_t *)&p_has->hearing_aid_features,
                              sizeof(T_HAS_HA_FEATURES), GATT_PDU_TYPE_NOTIFICATION);
}


bool has_send_cp_data(uint16_t conn_handle, uint8_t *p_data, uint16_t data_len)
{
    if (p_has == NULL)
    {
        return false;
    }

#if F_BT_5_2_EATT_SUPPORT
    if (p_has->is_ha_cp_notify)
    {
        return gatt_svc_send_data(conn_handle, L2C_FIXED_CID_ATT, p_has->service_id,
                                  has_get_attr_idx(HAS_UUID_CHAR_HA_PRESET_CP),
                                  p_data, data_len, GATT_PDU_TYPE_NOTIFICATION);
    }
    else
#endif
    {
        return gatt_svc_send_data(conn_handle, L2C_FIXED_CID_ATT, p_has->service_id,
                                  has_get_attr_idx(HAS_UUID_CHAR_HA_PRESET_CP),
                                  p_data, data_len, GATT_PDU_TYPE_INDICATION);
    }
}

bool has_send_active_idx_notify(uint16_t conn_handle)
{
    if (p_has == NULL)
    {
        return false;
    }

    return gatt_svc_send_data(conn_handle, L2C_FIXED_CID_ATT, p_has->service_id,
                              has_get_attr_idx(HAS_UUID_CHAR_ACTIVE_PRESET_IDX),
                              &p_has->active_preset_idx, sizeof(uint8_t),
                              GATT_PDU_TYPE_NOTIFICATION);
}

bool has_send_hearing_aid_feature_notify_all(void)
{
    if (p_has == NULL)
    {
        return false;
    }

    uint8_t i;
    uint16_t cccd_handle;
    cccd_handle = server_get_start_handle(p_has->service_id);
    cccd_handle += (has_get_attr_idx(HAS_UUID_CHAR_HA_FEATURES) + 1);

    for (i = 0; i < ble_audio_db.acl_link_num; i++)
    {
        if ((ble_audio_db.le_link[i].used == true) &&
            (ble_audio_db.le_link[i].state == GAP_CONN_STATE_CONNECTED))
        {
            T_LE_SRV_CFG *p_srv = ble_srv_find_by_srv_id(ble_audio_db.le_link[i].conn_handle,
                                                         p_has->service_id);
            if (p_srv != NULL && (p_srv->cccd_cfg[0] & HAS_HEARING_AID_FEATURES_CCCD_FLAG))
            {
                has_send_hearing_aid_feature_notify(ble_audio_db.le_link[i].conn_handle);
            }
        }
    }

    ble_audio_ual_set_pending_cccd(cccd_handle);
    return true;
}

bool has_handle_read_preset_rsp(uint16_t conn_handle, uint8_t value_len,
                                uint8_t *p_value, bool is_last)
{
    uint8_t *p_data = ble_audio_mem_zalloc(value_len + 2);

    if (p_data != NULL)
    {
        p_data[0] = HAS_CP_OP_READ_PRESET_RSP;

        if (is_last)
        {
            p_data[1] = 0x01;
        }
        else
        {
            p_data[1] = 0x00;
        }

        memcpy(&p_data[2], p_value, value_len);

        if (has_send_cp_data(conn_handle, p_data, value_len + 2))
        {
            ble_audio_mem_free(p_data);
            if (is_last == true)
            {
                has_clear_cp_flag();
            }

            return true;
        }

        ble_audio_mem_free(p_data);
    }

    return false;
}

bool has_send_preset_change_data(T_HAS_PRESET_CHANGE_ID change_id, uint16_t conn_handle,
                                 uint16_t value_len, uint8_t *p_value, bool is_last)
{
    uint8_t *p_data = ble_audio_mem_zalloc(value_len + 3);
    if (p_data == NULL)
    {
        return false;
    }

    p_data[0] = HAS_CP_OP_PRESET_CHANGED;
    p_data[1] = change_id;

    if (is_last)
    {
        p_data[2] = 0x01;
    }
    else
    {
        p_data[2] = 0x00;
    }

    memcpy(&p_data[3], p_value, value_len);

    if (has_send_cp_data(conn_handle, p_data, value_len + 3) == false)
    {
        ble_audio_mem_free(p_data);
        return false;
    }

    ble_audio_mem_free(p_data);
    return true;
}

bool has_send_preset_change_data_all(T_HAS_PRESET_CHANGE_ID change_id,
                                     uint16_t value_len, uint8_t *p_value, bool is_last)
{
    if (p_has == NULL)
    {
        return false;
    }

    uint8_t i;
    uint16_t cccd_handle;
    cccd_handle = server_get_start_handle(p_has->service_id);
    cccd_handle += (has_get_attr_idx(HAS_UUID_CHAR_HA_PRESET_CP) + 1);

    if (change_id == PRESET_RECORD_DELETED || change_id == PRESET_RECORD_UNAVAILABLE)
    {
        uint8_t preset_idx = p_value[0];
        if (preset_idx == p_has->active_preset_idx)
        {
            preset_idx = 0;
            has_update_active_preset_idx(preset_idx);
        }
    }

    for (i = 0; i < ble_audio_db.acl_link_num; i++)
    {
        if ((ble_audio_db.le_link[i].used == true) &&
            (ble_audio_db.le_link[i].state == GAP_CONN_STATE_CONNECTED))
        {
            T_LE_SRV_CFG *p_srv = ble_srv_find_by_srv_id(ble_audio_db.le_link[i].conn_handle,
                                                         p_has->service_id);
            if (p_srv != NULL && (p_srv->cccd_cfg[0] & HAS_HEARING_CP_CCCD_FLAG))
            {
                has_send_preset_change_data(change_id, ble_audio_db.le_link[i].conn_handle,
                                            value_len, p_value, is_last);
            }
        }
    }

    ble_audio_ual_set_pending_cccd(cccd_handle);
    return true;
}

bool has_send_active_preset_notify_all(void)
{
    if (p_has == NULL)
    {
        return false;
    }

    uint8_t i;
    uint16_t cccd_handle;
    cccd_handle = server_get_start_handle(p_has->service_id);
    cccd_handle += (has_get_attr_idx(HAS_UUID_CHAR_ACTIVE_PRESET_IDX) + 1);

    for (i = 0; i < ble_audio_db.acl_link_num; i++)
    {
        if ((ble_audio_db.le_link[i].used == true) &&
            (ble_audio_db.le_link[i].state == GAP_CONN_STATE_CONNECTED))
        {
            T_LE_SRV_CFG *p_srv = ble_srv_find_by_srv_id(ble_audio_db.le_link[i].conn_handle,
                                                         p_has->service_id);
            if (p_srv != NULL && (p_srv->cccd_cfg[0] & HAS_ACTIVE_PRESET_IDX_CCCD_FLAG))
            {
                has_send_active_idx_notify(ble_audio_db.le_link[i].conn_handle);
            }
        }
    }

    ble_audio_ual_set_pending_cccd(cccd_handle);
    return true;
}

bool has_check_hearing_aid_features(T_HAS_HA_FEATURES hearing_aid_features)
{
    if (hearing_aid_features.hearing_aid_type == HA_FEATURES_HA_TYPE_RFU_MASK)
    {
        return false;
    }

    if (hearing_aid_features.hearing_aid_type == HA_FEATURES_MONAURAL_HA_MASK ||
        hearing_aid_features.hearing_aid_type == HA_FEATURES_BANDED_HA_MASK)
    {
        if (hearing_aid_features.preset_sync_support == 1 ||
            hearing_aid_features.independent_preset == 1)
        {
            return false;
        }
    }

    if (hearing_aid_features.preset_sync_support == 1 && hearing_aid_features.independent_preset == 1)
    {
        return false;
    }

    return true;
}

bool has_update_ha_features(T_HAS_HA_FEATURES hearing_aid_features)
{
    if (!has_check_hearing_aid_features(hearing_aid_features))
    {
        PROTOCOL_PRINT_ERROR1("has hearing aid feature error: has feature 0x%x", hearing_aid_features);
        return false;
    }

    if (!memcmp(&p_has->hearing_aid_features, &hearing_aid_features, 1))
    {
        PROTOCOL_PRINT_TRACE1("[HAS] has_update_ha_features: hearing aid features not change 0x%x",
                              hearing_aid_features);
        return false;
    }

    p_has->hearing_aid_features = hearing_aid_features;
    if (!has_send_hearing_aid_feature_notify_all())
    {
        return false;
    }

    return true;
}

bool has_update_active_preset_idx(uint8_t preset_idx)
{
    if (p_has->active_preset_idx == preset_idx)
    {
        PROTOCOL_PRINT_TRACE1("[HAS] has_update_active_preset_idx: preset idx not change, index 0x%x",
                              preset_idx);
        return false;
    }

    p_has->active_preset_idx = preset_idx;
    if (!has_send_active_preset_notify_all())
    {
        return false;
    }

    return true;
}

void has_handle_pending_cccd(uint16_t conn_handle, T_LE_SRV_CFG *p_srv)
{
    uint16_t cccd_handle = 0;

    PROTOCOL_PRINT_TRACE2("[HAS] has_handle_pending_cccd: start, conn_handle 0x%x, cccd_pending 0x%x",
                          conn_handle, p_srv->cccd_pending);
    if (p_srv->cccd_pending[0] & HAS_HEARING_AID_FEATURES_CCCD_FLAG)
    {
        if (has_send_hearing_aid_feature_notify(conn_handle))
        {
            cccd_handle = server_get_start_handle(p_srv->service_id);
            cccd_handle += (has_get_attr_idx(HAS_UUID_CHAR_HA_FEATURES) + 1);
            if (ble_audio_ual_clear_pending_cccd(conn_handle, cccd_handle))
            {
                p_srv->cccd_pending[0] &= ~HAS_HEARING_AID_FEATURES_CCCD_FLAG;
            }
        }
    }
    PROTOCOL_PRINT_TRACE1("[HAS] has_handle_pending_cccd: cccd_pending[0] 0x%x",
                          p_srv->cccd_pending[0]);
    if (p_srv->cccd_pending[0] & HAS_ACTIVE_PRESET_IDX_CCCD_FLAG)
    {
        PROTOCOL_PRINT_TRACE0("[HAS] has_handle_pending_cccd: HAS_ACTIVE_PRESET_IDX_CCCD_FLAG");
        if (has_send_active_idx_notify(conn_handle))
        {
            cccd_handle = server_get_start_handle(p_srv->service_id);
            cccd_handle += (has_get_attr_idx(HAS_UUID_CHAR_ACTIVE_PRESET_IDX) + 1);
            if (ble_audio_ual_clear_pending_cccd(conn_handle, cccd_handle))
            {
                p_srv->cccd_pending[0] &= ~HAS_ACTIVE_PRESET_IDX_CCCD_FLAG;
            }
        }
    }
    if (p_srv->cccd_pending[0] & HAS_HEARING_CP_CCCD_FLAG)
    {
        T_HAS_PENDING_PRESET_CHANGE cccd_info;
        cccd_info.conn_handle = conn_handle;

        if (ble_audio_mgr_dispatch(LE_AUDIO_MSG_HAS_PENDING_PRESET_CHANGE,
                                   &cccd_info) == BLE_AUDIO_CB_RESULT_SUCCESS)
        {
            cccd_handle = server_get_start_handle(p_srv->service_id);
            cccd_handle += (has_get_attr_idx(HAS_UUID_CHAR_HA_PRESET_CP) + 1);
            if (ble_audio_ual_clear_pending_cccd(conn_handle, cccd_handle))
            {
                p_srv->cccd_pending[0] &= ~HAS_HEARING_CP_CCCD_FLAG;
            }
        }
    }
}

static void has_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BLE_DM_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BLE_DM_EVENT_MTU_INFO:
        {
            PROTOCOL_PRINT_INFO2("has_dm_cback: conn_id %d, mtu_size %d",
                                 param->mtu_info.conn_id, param->mtu_info.mtu_size);
        }
        break;
    case BLE_DM_EVENT_AUTHEN_INFO:
        {
            if (param->authen_info.new_state == GAP_AUTHEN_STATE_COMPLETE &&
                param->authen_info.cause == GAP_SUCCESS)
            {
                T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_id(param->authen_info.conn_id);
                if (p_link == NULL)
                {
                    return;
                }
                if (ble_audio_ual_check_le_bond(p_link->conn_handle))
                {
                    T_LE_SRV_CFG *p_srv;
                    p_srv = ble_srv_find_by_srv_id(p_link->conn_handle, p_has->service_id);
                    if (p_srv != NULL && p_srv->cccd_pending[0] != 0)
                    {
                        has_handle_pending_cccd(p_link->conn_handle, p_srv);
                    }
                }
            }
        }
        break;
    default:
        break;
    }
}

T_APP_RESULT has_hanlde_ctl_pnt_proc(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                     uint16_t length, uint8_t *p_value,
                                     P_FUN_EXT_WRITE_IND_POST_PROC *p_write_post_proc)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    memcpy(p_has->ctrl_pnt_param, p_value, length);

    T_HAS_CP_IND write_req = {0};

    write_req.conn_handle = conn_handle;
    write_req.cp_op = (T_HAS_CP_OP)p_has->ctrl_pnt_param[0];

    uint8_t param_len = length - 1;

    switch (p_has->ctrl_pnt_param[0])
    {
    case HAS_CP_OP_READ_PRESETS_REQ:
        {
            if (param_len != 2)
            {
                cause = APP_RESULT_INVALID_VALUE_SIZE;
            }
            else
            {
                write_req.index = p_has->ctrl_pnt_param[1];
                write_req.preset_num = p_has->ctrl_pnt_param[2];

                if (write_req.index == 0 || write_req.preset_num == 0)
                {
                    cause = (T_APP_RESULT)(ATT_ERR | ATT_ERR_OUT_OF_RANGE);
                }
                else
                {
                    cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_HAS_CP_IND, &write_req);
                }
            }
        }
        break;

    case HAS_CP_OP_WRITE_PRESET_NAME:
        {
            if (p_has->hearing_aid_features.writable_preset_support == 1 &&
                p_has->hearing_aid_features.dynamic_preset == 1)
            {
                if (param_len < HAS_CP_WRITE_PRESET_NAME_PARAM_MIN_LEN ||
                    param_len > HAS_CP_WRITE_PRESET_NAME_PARAM_MAX_LEN)
                {
                    cause = (T_APP_RESULT)ATT_ERR_HAS_OP_PRESET_NAME_TOO_LONG;
                }
                else
                {
                    write_req.index = p_has->ctrl_pnt_param[1];
                    write_req.name_len = param_len - 1;
                    write_req.p_name = (char *)&p_has->ctrl_pnt_param[2];
                    cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_HAS_CP_IND, &write_req);
                }
            }
            else
            {
                cause = (T_APP_RESULT)ATT_ERR_HAS_OP_INVALID_OPCODE;
            }
        }
        break;

    case HAS_CP_OP_SET_ACTIVE_PRESET:
        {
            if (param_len != 1)
            {
                cause = APP_RESULT_INVALID_VALUE_SIZE;
            }
            else
            {
                write_req.index = p_has->ctrl_pnt_param[1];
                cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_HAS_CP_IND, &write_req);
            }
        }
        break;

    case HAS_CP_OP_SET_NEXT_PRESET:
    case HAS_CP_OP_SET_PREVIOUS_PRESET:
        {
            if (param_len != 0)
            {
                cause = APP_RESULT_INVALID_VALUE_SIZE;
            }
            else
            {
                write_req.active_preset_idx = p_has->active_preset_idx;
                cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_HAS_CP_IND, &write_req);
            }
        }
        break;

    case HAS_CP_OP_SET_ACTIVE_PRESET_SYNC_LOCAL:
        {
            if (p_has->hearing_aid_features.preset_sync_support == 1)
            {
                if (param_len != 1)
                {
                    cause = APP_RESULT_INVALID_VALUE_SIZE;
                }
                else
                {
                    write_req.index = p_has->ctrl_pnt_param[1];
                    cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_HAS_CP_IND, &write_req);
                }
            }
            else
            {
                cause = (T_APP_RESULT)ATT_ERR_HAS_OP_PRESET_SYNC_NOT_SUPPORT;
            }
        }
        break;

    case HAS_CP_OP_SET_NEXT_PRESET_SYNC_LOCAL:
    case HAS_CP_OP_SET_PREVIOUS_PRESET_SYNC_LOCAL:
        {
            if (p_has->hearing_aid_features.preset_sync_support == 1)
            {
                if (param_len != 0)
                {
                    cause = APP_RESULT_INVALID_VALUE_SIZE;
                }
                else
                {
                    write_req.active_preset_idx = p_has->active_preset_idx;
                    cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_HAS_CP_IND, &write_req);
                }
            }
            else
            {
                cause = (T_APP_RESULT)ATT_ERR_HAS_OP_PRESET_SYNC_NOT_SUPPORT;
            }
        }
        break;

    default:
        {
            cause = (T_APP_RESULT)ATT_ERR_HAS_OP_INVALID_OPCODE;
        }
        break;
    }

    if (p_has->ctrl_pnt_param[0] != HAS_CP_OP_READ_PRESETS_REQ)
    {
        p_has->ctrl_pnt_param[0] = HAS_CP_OP_RESERVED;
    }

    return cause;
}


static T_APP_RESULT has_attr_read_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                     uint16_t attrib_index,
                                     uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    PROTOCOL_PRINT_INFO2("has_attr_read_cb: attrib_index %d, offset %x", attrib_index, offset);

    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_has->p_attr_tbl, attrib_index,
                                                             p_has->attr_num);
    if (char_uuid.uuid_size != UUID_16BIT_SIZE)
    {
        PROTOCOL_PRINT_ERROR1("has_attr_read_cb Error: attrib_index 0x%x", attrib_index);
        return APP_RESULT_ATTR_NOT_FOUND;
    }
    *p_length = 0;
    switch (char_uuid.uu.char_uuid16)
    {
    case HAS_UUID_CHAR_HA_FEATURES:
        {
            *pp_value = (uint8_t *)&p_has->hearing_aid_features;
            *p_length = sizeof(T_HAS_HA_FEATURES);
        }
        break;
    case HAS_UUID_CHAR_ACTIVE_PRESET_IDX:
        {
            *pp_value = &p_has->active_preset_idx;
            *p_length = sizeof(uint8_t);
        }
        break;
    default:
        {
            cause = APP_RESULT_ATTR_NOT_FOUND;
        }
        break;
    }

    return (cause);
}

static T_APP_RESULT has_attr_write_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                      uint16_t attrib_index,
                                      T_WRITE_TYPE write_type,
                                      uint16_t length, uint8_t *p_value, P_FUN_EXT_WRITE_IND_POST_PROC *p_write_post_proc)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    PROTOCOL_PRINT_INFO2("has_attr_write_cb: attrib_index %d, length %x", attrib_index, length);

    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_has->p_attr_tbl, attrib_index,
                                                             p_has->attr_num);

    if (char_uuid.uuid_size != UUID_16BIT_SIZE)
    {
        PROTOCOL_PRINT_ERROR1("has_attr_read_cb Error: attrib_index 0x%x", attrib_index);
        return APP_RESULT_ATTR_NOT_FOUND;
    }
    if (char_uuid.uu.char_uuid16 == HAS_UUID_CHAR_HA_PRESET_CP)
    {
        if (HAS_CTL_PNT_OPERATE_ACTIVE(p_has->ctrl_pnt_param[0]))
        {
            uint8_t opcode;
            memcpy(&opcode, p_value, sizeof(uint8_t));

            if ((opcode == HAS_CP_OP_READ_PRESETS_REQ) ||
                (opcode == HAS_CP_OP_WRITE_PRESET_NAME))
            {
                cause = APP_RESULT_PROC_ALREADY_IN_PROGRESS;
            }
        }
        else
        {
            cause = has_hanlde_ctl_pnt_proc(conn_handle, cid, service_id, length, p_value, p_write_post_proc);
        }
    }
    else
    {
        cause =  APP_RESULT_ATTR_NOT_FOUND;
    }

    return (cause);
}

void has_cccd_update_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                        uint16_t attrib_index, uint16_t ccc_bits)
{
    T_LE_SRV_CFG *p_srv = ble_srv_alloc_by_srv_id(conn_handle, service_id);
    if (p_srv == NULL)
    {
        PROTOCOL_PRINT_ERROR1("has_cccd_update_cb srv error: service_id 0x%x", service_id);
        return;
    }

    PROTOCOL_PRINT_INFO2("has_cccd_update_cb: attrib_index %d, ccc_bits 0x%x", attrib_index, ccc_bits);

    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_has->p_attr_tbl, attrib_index,
                                                             p_has->attr_num);

    if (char_uuid.uuid_size != UUID_16BIT_SIZE)
    {
        PROTOCOL_PRINT_ERROR1("has_cccd_update_cb attrib error: attrib_index 0x%x", attrib_index);
    }
    if (char_uuid.uu.char_uuid16 == HAS_UUID_CHAR_HA_FEATURES)
    {
        PROTOCOL_PRINT_INFO1("[HAS] HAS_HEARING_AID_FEATURES_CCCD_FLAG: ccc_bits 0x%x", ccc_bits);
        ble_srv_set_cccd_flags(p_srv, ccc_bits, 0, HAS_HEARING_AID_FEATURES_CCCD_FLAG, true);
    }
    else if (char_uuid.uu.char_uuid16 == HAS_UUID_CHAR_HA_PRESET_CP)
    {
        PROTOCOL_PRINT_INFO1("[HAS] HAS_HEARING_CP_CCCD_FLAG: ccc_bits 0x%x", ccc_bits);
        ble_srv_set_cccd_flags(p_srv, ccc_bits, 0, HAS_HEARING_CP_CCCD_FLAG, true);
    }
    else if (char_uuid.uu.char_uuid16 == HAS_UUID_CHAR_ACTIVE_PRESET_IDX)
    {
        PROTOCOL_PRINT_INFO1("[HAS] HAS_ACTIVE_PRESET_IDX_CCCD_FLAG: ccc_bits 0x%x", ccc_bits);
        ble_srv_set_cccd_flags(p_srv, ccc_bits, 0, HAS_ACTIVE_PRESET_IDX_CCCD_FLAG, true);
    }
    else
    {
        return;
    }

    return;
}

bool has_create_table(T_HAS_ATTR_FEATURE attr_feature)
{
    uint16_t idx = 3;
    uint16_t attr_tbl_size = 0;

    if (attr_feature.is_ha_features_notify_support)
    {
        idx += 1;
    }

    if (attr_feature.is_ha_cp_exist)
    {
        idx += 2 * 3;
    }
#if LE_AUDIO_INIT_DEBUG
    PROTOCOL_PRINT_TRACE1("[HAS]has_create_table: total idx: %d", idx);
#endif
    p_has->attr_num = idx;
    attr_tbl_size = idx * sizeof(T_ATTRIB_APPL);
    p_has->p_attr_tbl = ble_audio_mem_zalloc(attr_tbl_size);
    if (p_has->p_attr_tbl == NULL)
    {
        PROTOCOL_PRINT_ERROR0("[HAS]has_create_table: allocate p_attr_tbl failed");
        return false;
    }

    idx = 0;
    if (attr_feature.is_ha_features_notify_support)
    {
        memcpy(p_has->p_attr_tbl, (uint8_t *)has_attr_tbl, 4 * sizeof(T_ATTRIB_APPL));
        p_has->p_attr_tbl[1].type_value[2] |= GATT_CHAR_PROP_NOTIFY;
        idx += 4;
    }
    else
    {
        memcpy(p_has->p_attr_tbl, (uint8_t *)has_attr_tbl, 3 * sizeof(T_ATTRIB_APPL));
        p_has->p_attr_tbl[1].type_value[2] &= (~GATT_CHAR_PROP_NOTIFY);
        idx += 3;
    }

    if (attr_feature.is_ha_cp_exist)
    {
        memcpy(&p_has->p_attr_tbl[idx], (uint8_t *)has_cp_char_tbl, 6 * sizeof(T_ATTRIB_APPL));
        if (attr_feature.is_ha_cp_notify)
        {
            p_has->p_attr_tbl[idx].type_value[2] |= GATT_CHAR_PROP_NOTIFY;
        }
        else
        {
            p_has->p_attr_tbl[idx].type_value[2] &= (~GATT_CHAR_PROP_NOTIFY);
        }
    }

    return true;
}

const T_FUN_GATT_EXT_SERVICE_CBS has_cbs =
{
    has_attr_read_cb,  // Read callback function pointer
    has_attr_write_cb, // Write callback function pointer
    has_cccd_update_cb  // CCCD update callback function pointer
};

bool has_init(T_HAS_ATTR_FEATURE attr_feature, T_HAS_HA_FEATURES hearing_aid_features)
{
    uint8_t error_id = 0;
    T_SERVER_ID service_id;

    if (p_has)
    {
        error_id = 1;
        goto error;
    }

    p_has = ble_audio_mem_zalloc(sizeof(T_HAS));

    if (p_has == NULL)
    {
        error_id = 2;
        goto error;
    }

    if (has_create_table(attr_feature))
    {
        if (false == gatt_svc_add(&service_id,
                                  (uint8_t *)p_has->p_attr_tbl,
                                  p_has->attr_num * sizeof(T_ATTRIB_APPL),
                                  &has_cbs, NULL))
        {
            error_id = 3;
            goto error;
        }

        p_has->service_id = service_id;
        p_has->active_preset_idx = 0;
        p_has->is_ha_cp_notify = attr_feature.is_ha_cp_notify;
        p_has->ctrl_pnt_param[0] = HAS_CP_OP_RESERVED;

        if (has_check_hearing_aid_features(hearing_aid_features))
        {
            p_has->hearing_aid_features = hearing_aid_features;
        }
        else
        {
            PROTOCOL_PRINT_ERROR1("has_add_service error: hearing_aid_features %d", hearing_aid_features);
        }
    }

    ble_dm_cback_register(has_dm_cback);

    return true;

error:
    PROTOCOL_PRINT_ERROR1("has_add_service: fail, error_id %d", error_id);


    if (p_has != NULL)
    {
        if (p_has->p_attr_tbl != NULL)
        {
            ble_audio_mem_free(p_has->p_attr_tbl);
        }
        ble_audio_mem_free(p_has);
    }

    return false;
}

#if LE_AUDIO_DEINIT
void has_deinit(void)
{
    if (p_has)
    {
        if (p_has->p_attr_tbl != NULL)
        {
            ble_audio_mem_free(p_has->p_attr_tbl);
        }
        ble_audio_mem_free(p_has);
        p_has = NULL;
    }
}
#endif
#endif
