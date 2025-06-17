#include <string.h>
#include "trace.h"
#include "bass_mgr.h"
#include "bt_types.h"
#include "ble_audio_dm.h"
#include "ble_audio_mgr.h"
#include "bass_brs_mgr.h"
#include "ble_link_util.h"
#include "ble_audio_sync_int.h"
#include "ble_audio.h"
#include "ble_audio_ual.h"
#include "sys_timer.h"

#if LE_AUDIO_BASS_SUPPORT
#define BASS_CHAR_BAS_CP_INDEX                   2
#define BASS_CHAR_BRS_INDEX                      4
#define BASS_CHAR_BRS_CCCD_INDEX                 5

/**< @brief  profile/service definition.  */
static const T_ATTRIB_APPL bass_attr_tbl[] =
{
    /*----------------- Published Audio Capabilities Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),   /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_BASS),              /* service UUID */
            HI_WORD(GATT_UUID_BASS)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_WRITE_NO_RSP |                   /* characteristic properties */
             GATT_CHAR_PROP_WRITE)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* ASE Control Point value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(BASS_UUID_CHAR_BROADCAST_AUDIO_SCAN_CP),
            HI_WORD(BASS_UUID_CHAR_BROADCAST_AUDIO_SCAN_CP)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_WRITE_AUTHEN_REQ /* permissions */
    },
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* ASE value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(BASS_UUID_CHAR_BROADCAST_RECEIVE_STATE),
            HI_WORD(BASS_UUID_CHAR_BROADCAST_RECEIVE_STATE)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ /* permissions */
    }
    ,
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
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* ASE value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(BASS_UUID_CHAR_BROADCAST_RECEIVE_STATE),
            HI_WORD(BASS_UUID_CHAR_BROADCAST_RECEIVE_STATE)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ /* permissions */
    }
    ,
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
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* ASE value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(BASS_UUID_CHAR_BROADCAST_RECEIVE_STATE),
            HI_WORD(BASS_UUID_CHAR_BROADCAST_RECEIVE_STATE)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ /* permissions */
    }
    ,
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
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* ASE value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(BASS_UUID_CHAR_BROADCAST_RECEIVE_STATE),
            HI_WORD(BASS_UUID_CHAR_BROADCAST_RECEIVE_STATE)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ /* permissions */
    }
    ,
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

uint16_t bass_attr_num = 0;

bool bass_send_brs_notify(T_BLE_AUDIO_LINK *p_link, uint16_t attrib_idx, T_BASS_BRS_DB *p_db)
{
    uint8_t *p_data = NULL;
    uint16_t len = 0;
    bool ret = true;

    if (bass_brs_gen_char_data(p_db->brs_data.source_id, &p_data,  &len))
    {
        if (len > p_link->mtu_size - 3)
        {
            len = p_link->mtu_size - 3;
        }
        if (gatt_svc_send_data(p_link->conn_handle, L2C_FIXED_CID_ATT, p_bass->service_id, attrib_idx,
                               p_data, len,
                               GATT_PDU_TYPE_NOTIFICATION) == false)
        {
            ret = false;
        }
    }
    if (p_data)
    {
        ble_audio_mem_free(p_data);
    }
    return ret;
}

bool bass_send_all_notify(T_BASS_BRS_DB *p_db)
{
    uint16_t cccd_handle;
    uint16_t bass_cccd_flag = 0x0001;
    T_BASS_BRS_MODIFY brs_msg;
    uint16_t attrib_idx;

    bass_cccd_flag = bass_cccd_flag << p_db->brs_data.source_id;
    attrib_idx = BASS_CHAR_BRS_INDEX + 3 * (p_db->brs_data.source_id - 1);
    cccd_handle = server_get_start_handle(p_bass->service_id);
    cccd_handle += (attrib_idx + 1);

    p_db->send_notify = false;
    p_db->brs_modify = false;

    for (uint8_t i = 0; i < ble_audio_db.acl_link_num; i++)
    {
        if ((ble_audio_db.le_link[i].used == true) &&
            (ble_audio_db.le_link[i].state == GAP_CONN_STATE_CONNECTED))
        {
            T_LE_SRV_CFG *p_srv = ble_srv_find_by_srv_id(ble_audio_db.le_link[i].conn_handle,
                                                         p_bass->service_id);
            if (p_srv != NULL && (p_srv->cccd_cfg[0] & bass_cccd_flag))
            {
                bass_send_brs_notify(&ble_audio_db.le_link[i], attrib_idx, p_db);
            }
        }
    }
    ble_audio_ual_set_pending_cccd(cccd_handle);
    brs_msg.handle = p_db->handle;
    brs_msg.source_id = p_db->brs_data.source_id;
    brs_msg.p_brs_data = &p_db->brs_data;
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_BASS_BRS_MODIFY, &brs_msg);
    return true;
}

void bass_handle_base_data(T_BASS_BRS_DB *p_db,
                           T_BLE_AUDIO_BASE_DATA_MODIFY_INFO *p_info)
{
    if (p_info->p_base_mapping)
    {
        p_db->bis_sync_info.base_data_subgroup_num = p_info->p_base_mapping->num_subgroups;
        for (uint8_t i = 0; i < p_info->p_base_mapping->num_subgroups; i++)
        {
            p_db->bis_sync_info.base_data_subgroup_bis[i] = p_info->p_base_mapping->p_subgroup[i].bis_array;
        }
    }
    else
    {
        p_db->bis_sync_info.base_data_subgroup_num = 0;
        memset(p_db->bis_sync_info.base_data_subgroup_bis, 0, BASS_SUBGROUP_MAX_NUM * sizeof(uint32_t));
    }
    if (p_info->p_base_mapping)
    {
        if (p_db->brs_data.p_cp_bis_info == NULL)
        {
            uint8_t i;
            uint16_t meta_data_len = 0;
            uint16_t bis_info_size;
            T_BASS_CP_BIS_INFO *p_cp_bis_info = NULL;
            uint8_t *p_meta_data;

            for (i = 0; i < p_info->p_base_mapping->num_subgroups; i++)
            {
                meta_data_len += p_info->p_base_mapping->p_subgroup[i].metadata_len;
            }
            bis_info_size = p_info->p_base_mapping->num_subgroups * sizeof(
                                T_BASS_CP_BIS_INFO) + meta_data_len;
            p_cp_bis_info = ble_audio_mem_zalloc(bis_info_size);
            if (p_cp_bis_info == NULL)
            {
                return;
            }
            p_meta_data = (uint8_t *)(p_cp_bis_info + p_info->p_base_mapping->num_subgroups);
            for (i = 0; i < p_info->p_base_mapping->num_subgroups; i++)
            {
                p_cp_bis_info[i].bis_sync = 0;//p_info->p_base_mapping->p_subgroup[i].bis_array;
                p_cp_bis_info[i].metadata_len = p_info->p_base_mapping->p_subgroup[i].metadata_len;
                if (p_cp_bis_info[i].metadata_len > 0)
                {
                    memcpy(p_meta_data, p_info->p_base_mapping->p_subgroup[i].p_metadata,
                           p_cp_bis_info[i].metadata_len);
                    p_cp_bis_info[i].p_metadata = p_meta_data;
                    p_meta_data += p_cp_bis_info->metadata_len;
                }
            }
            p_db->brs_data.p_cp_bis_info = p_cp_bis_info;
            p_db->brs_data.bis_info_size = bis_info_size;
            p_db->brs_data.num_subgroups = p_info->p_base_mapping->num_subgroups;
            bass_send_all_notify(p_db);
        }
    }
}

void bass_audio_sync_cb(T_BLE_AUDIO_SYNC_HANDLE handle, uint8_t cb_type, void *p_cb_data)
{
    T_BASS_BRS_DB *p_db = NULL;
    T_BLE_AUDIO_SYNC_CB_DATA *p_sync_cb = (T_BLE_AUDIO_SYNC_CB_DATA *)p_cb_data;
    p_db = bass_brs_find_by_handle(handle);
    if (p_db == NULL && cb_type != MSG_BLE_AUDIO_SYNC_LOCAL_ADD)
    {
        return;
    }
    switch (cb_type)
    {
    case MSG_BLE_AUDIO_SYNC_LOCAL_ADD:
        {
            T_BLE_AUDIO_SYNC_INFO sync_info;
            T_BASS_LOCAL_ADD_SOURCE brs_msg;
            if (ble_audio_sync_get_info(handle, &sync_info))
            {
                p_db = bass_brs_add(sync_info.advertiser_address_type,
                                    sync_info.advertiser_address,
                                    sync_info.adv_sid,
                                    sync_info.broadcast_id);
                if (p_db)
                {
                    p_db->handle = handle;
                    brs_msg.source_id = p_db->brs_data.source_id;
                    brs_msg.handle = p_db->handle;
                    ble_audio_mgr_dispatch(LE_AUDIO_MSG_BASS_LOCAL_ADD_SOURCE, &brs_msg);
                    bass_send_all_notify(p_db);
                    break;
                }
            }
            PROTOCOL_PRINT_ERROR1("MSG_BLE_AUDIO_SYNC_LOCAL_ADD: failed, handle 0x%x", handle);
        }
        break;

    case MSG_BLE_AUDIO_ADDR_UPDATE:
        {
            memcpy(p_db->brs_data.source_address, p_sync_cb->p_addr_update->advertiser_address,
                   GAP_BD_ADDR_LEN);
            bass_send_all_notify(p_db);
        }
        break;

    case MSG_BLE_AUDIO_SYNC_HANDLE_RELEASED:
        {
            p_db->handle = NULL;
            if (p_sync_cb->p_sync_handle_released->action_role == BLE_AUDIO_ACTION_ROLE_LOCAL_API)
            {
                if (bass_brs_delete(p_db, true))
                {
                    if (p_db->send_notify)
                    {
                        bass_send_all_notify(p_db);
                    }
                }
            }
        }
        break;
    case MSG_BLE_AUDIO_SYNC_PAST_SRV_DATA:
        {
            T_LE_PAST_RECIPIENT_PERIODIC_ADV_SYNC_TRANSFER_RECEIVED_INFO *p_info =
                (T_LE_PAST_RECIPIENT_PERIODIC_ADV_SYNC_TRANSFER_RECEIVED_INFO *)p_cb_data;
            if (p_info->service_data & BASS_PAST_SRV_DATA_MATCH_SRC_ADDR_FLAG)
            {
                uint8_t source_id = p_info->service_data >> 8;
                if (p_db->brs_data.source_id == source_id)
                {
                    ble_audio_sync_update_addr(p_db->handle, p_info->adv_addr);
                }
            }
        }
        break;

    case MSG_BLE_AUDIO_PA_SYNC_STATE:
        {
            p_db->pa_sync_state = p_sync_cb->p_pa_sync_state->sync_state;
            if (p_sync_cb->p_pa_sync_state->sync_state == GAP_PA_SYNC_STATE_TERMINATED)
            {
                p_db->big_info_flags = 0;
                p_db->biginfo_bis_num = 0;
            }
            if (p_sync_cb->p_pa_sync_state->sync_state == GAP_PA_SYNC_STATE_TERMINATED)
            {
                bass_pa_proc_sm_execute(p_db, BASS_PA_STATE_IDLE, NULL, true);
            }
            else if (p_sync_cb->p_pa_sync_state->sync_state == GAP_PA_SYNC_STATE_SYNCHRONIZED)
            {
                bass_pa_proc_sm_execute(p_db, BASS_PA_STATE_SYNCHRONIZED, NULL, true);
                bass_big_proc_sm_execute(p_db, BASS_BIG_PA_SYNC_EVT, NULL, true);
            }
            else if (p_sync_cb->p_pa_sync_state->sync_state == GAP_PA_SYNC_STATE_TERMINATING)
            {
                bass_pa_proc_sm_execute(p_db, BASS_PA_STATE_TERMINATING, NULL, false);
            }
            else
            {
                bass_pa_proc_sm_execute(p_db, BASS_PA_STATE_SYNCHRONIZING, NULL, false);
            }
        }
        break;

    case MSG_BLE_AUDIO_PA_BIGINFO:
        {
            p_db->biginfo_bis_num = p_sync_cb->p_le_biginfo_adv_report_info->num_bis;
            if ((p_db->big_info_flags & BASS_BIG_INFO_RECEIVE_FLAG) == 0)
            {
                p_db->big_info_flags |= BASS_BIG_INFO_RECEIVE_FLAG;
                if (p_sync_cb->p_le_biginfo_adv_report_info->encryption)
                {
                    p_db->big_sync_param.encryption = 1;
                    p_db->big_info_flags |= BASS_BIG_BROADCAST_REQUIRED_FLAG;
                }
                else
                {
                    p_db->big_sync_param.encryption = 0;
                    p_db->big_info_flags &= ~(BASS_BIG_BROADCAST_REQUIRED_FLAG | BASS_BIG_BROADCAST_CODE_SET_FLAG);
                }
                bass_big_proc_sm_execute(p_db, BASS_BIG_BIG_INFO_EVT, NULL, true);
            }
        }
        break;

    case MSG_BLE_AUDIO_BASE_DATA_MODIFY_INFO:
        {
            bass_handle_base_data(p_db, p_sync_cb->p_base_data_modify_info);
        }
        break;

    case MSG_BLE_AUDIO_BIG_SYNC_STATE:
        {
            p_db->big_sync_state = p_sync_cb->p_big_sync_state->sync_state;
            if (p_sync_cb->p_big_sync_state->sync_state == BIG_SYNC_RECEIVER_SYNC_STATE_SYNCHRONIZED)
            {
                bass_big_proc_sm_execute(p_db, BASS_BIG_STATE_SYNCHRONIZED, (uint8_t *)p_sync_cb->p_big_sync_state,
                                         true);
            }
            else if (p_sync_cb->p_big_sync_state->sync_state == BIG_SYNC_RECEIVER_SYNC_STATE_TERMINATED)
            {
                if (p_sync_cb->p_big_sync_state->action_role == BLE_AUDIO_ACTION_ROLE_BASS &&
                    p_db->bis_sync_info.cp_bis_switch == true)
                {
                    p_db->bis_sync_info.cp_bis_switch = false;
                    bass_big_proc_sm_execute(p_db, BASS_BIG_STATE_IDLE, (uint8_t *)p_sync_cb->p_big_sync_state, false);
                    bass_big_proc_sm_execute(p_db, BASS_BIG_CP_SYNC_EVT, (uint8_t *)p_sync_cb->p_big_sync_state, false);
                    if (p_db->pa_sync_state == GAP_PA_SYNC_STATE_SYNCHRONIZED)
                    {
                        bass_big_proc_sm_execute(p_db, BASS_BIG_PA_SYNC_EVT, NULL, false);
                    }
                }
                else
                {
                    bass_big_proc_sm_execute(p_db, BASS_BIG_STATE_IDLE, (uint8_t *)p_sync_cb->p_big_sync_state, true);
                }
            }
            else if (p_sync_cb->p_big_sync_state->sync_state == BIG_SYNC_RECEIVER_SYNC_STATE_SYNCHRONIZING)
            {
                bass_big_proc_sm_execute(p_db, BASS_BIG_STATE_SYNCHRONIZING,
                                         (uint8_t *)p_sync_cb->p_big_sync_state, false);
            }
            else
            {
                bass_big_proc_sm_execute(p_db, BASS_BIG_STATE_TERMINATING, (uint8_t *)p_sync_cb->p_big_sync_state,
                                         false);
            }
        }
        break;

    default:
        break;
    }
    return;
}

bool bass_delete_source_int(T_BASS_BRS_DB *p_db, T_BLE_AUDIO_ACTION_ROLE role)
{
    if (p_db->handle)
    {
        if (ble_audio_sync_release_int(&p_db->handle, role) == false)
        {
            goto failed;
        }
    }
    if (bass_brs_delete(p_db, true) == false)
    {
        goto failed;
    }
    return true;
failed:
    PROTOCOL_PRINT_ERROR0("bass_delete_source_int: failed");
    return false;
}

bool bass_cfg_broadcast_code(uint8_t source_id, uint8_t broadcast_code[BROADCAST_CODE_LEN])
{
    T_BASS_BRS_DB *p_db = NULL;
    p_db = bass_brs_find_by_id(source_id);
    if (p_db && broadcast_code != NULL)
    {
        memcpy(p_db->big_sync_param.broadcast_code, broadcast_code, 16);
        p_db->big_info_flags |= BASS_BIG_BROADCAST_CODE_SET_FLAG;
        return true;
    }
    return false;
}

bool bass_send_broadcast_code_required(uint8_t source_id)
{
    T_BASS_BRS_DB *p_db = NULL;
    p_db = bass_brs_find_by_id(source_id);
    if (p_db)
    {
        if (p_db->brs_data.big_encryption == BIG_NOT_ENCRYPTED ||
            p_db->brs_data.big_encryption == BIG_BAD_CODE)
        {
            p_db->brs_data.big_encryption = BIG_BROADCAST_CODE_REQUIRED;
            bass_send_all_notify(p_db);
            return true;
        }
    }
    return false;
}

bool bass_get_brs_char_data(uint8_t source_id, T_BASS_BRS_DATA *p_value,
                            T_BLE_AUDIO_SYNC_HANDLE *p_handle)
{
    T_BASS_BRS_DB *p_db = NULL;
    p_db = bass_brs_find_by_id(source_id);
    if (p_db && p_value && p_handle)
    {
        memcpy(p_value, &p_db->brs_data, sizeof(T_BASS_BRS_DATA));
        *p_handle = p_db->handle;
        return true;
    }
    return false;
}

bool bass_update_metadata(uint8_t source_id, uint8_t num_subgroups,
                          T_BASS_METADATA_INFO *p_metadata_tbl)
{
    T_BASS_BRS_DB *p_db = NULL;
    uint8_t err_idx = 0;

    if (num_subgroups == 0 || (num_subgroups != 0 && p_metadata_tbl == NULL))
    {
        err_idx = 1;
        goto failed;
    }
    p_db = bass_brs_find_by_id(source_id);
    if (p_db)
    {
        T_BASS_CP_BIS_INFO  *p_temp_bis_info;
        uint8_t i;
        uint16_t bis_info_size = 0;
        uint16_t meta_data_len = 0;
        if (num_subgroups == p_db->brs_data.num_subgroups)
        {
            for (i = 0; i < num_subgroups; i++)
            {
                if (p_db->brs_data.p_cp_bis_info[i].metadata_len == p_metadata_tbl[i].metadata_len)
                {
                    if (p_metadata_tbl[i].metadata_len != 0)
                    {
                        if (memcmp(p_db->brs_data.p_cp_bis_info[i].p_metadata, p_metadata_tbl[i].p_metadata,
                                   p_metadata_tbl[i].metadata_len) != 0)
                        {
                            break;
                        }
                    }
                }
                else
                {
                    break;
                }
            }
            if (i == num_subgroups)
            {
                return true;
            }
        }
        else
        {
            if (p_db->brs_data.p_cp_bis_info != NULL)
            {
                err_idx = 3;
                goto failed;
            }
        }
        for (i = 0; i < num_subgroups; i++)
        {
            meta_data_len += p_metadata_tbl[i].metadata_len;
        }
        bis_info_size = num_subgroups * sizeof(T_BASS_CP_BIS_INFO) + meta_data_len;
        p_temp_bis_info = ble_audio_mem_zalloc(bis_info_size);
        if (p_temp_bis_info == NULL)
        {
            err_idx = 4;
            goto failed;
        }
        if (num_subgroups > 0)
        {
            uint8_t *p_meta_data = (uint8_t *)(p_temp_bis_info +
                                               num_subgroups);
            for (i = 0; i < num_subgroups; i++)
            {
                if (p_db->brs_data.p_cp_bis_info)
                {
                    p_temp_bis_info[i].bis_sync = p_db->brs_data.p_cp_bis_info[i].bis_sync;
                }
                else
                {
                    p_temp_bis_info[i].bis_sync = 0;
                }
                p_temp_bis_info[i].metadata_len = p_metadata_tbl[i].metadata_len;
                if (p_temp_bis_info[i].metadata_len > 0)
                {
                    memcpy(p_meta_data, p_metadata_tbl[i].p_metadata,
                           p_metadata_tbl[i].metadata_len);
                    p_temp_bis_info[i].p_metadata = p_meta_data;
                    p_meta_data += p_temp_bis_info[i].metadata_len;
                }
            }
        }
        if (p_db->brs_data.p_cp_bis_info)
        {
            ble_audio_mem_free(p_db->brs_data.p_cp_bis_info);
        }
        p_db->brs_data.num_subgroups = num_subgroups;
        p_db->brs_data.bis_info_size = bis_info_size;
        p_db->brs_data.p_cp_bis_info = p_temp_bis_info;
        bass_send_all_notify(p_db);
    }
    else
    {
        err_idx = 5;
        goto failed;
    }
    return true;
failed:
    PROTOCOL_PRINT_ERROR1("bass_update_metadata: failed, err_idx %d", err_idx);
    return false;
}

void bass_handle_pa_sync(uint16_t conn_handle, T_BASS_BRS_DB *p_db, T_BASS_PA_SYNC pa_sync)
{
    if (pa_sync == BASS_PA_SYNC_PAST)
    {
        if ((p_db->brs_data.pa_sync_state != PA_SYNC_STATE_SYNC) &&
            (p_db->brs_data.pa_sync_state != PA_SYNC_STATE_SYNC_INFO_REQ))
        {
            if (ble_audio_check_remote_features(conn_handle, LE_SUPPORT_FEATURES_MASK_ARRAY_INDEX3,
                                                LE_SUPPORT_FEATURES_PAST_SENDER_MASK_BIT))
            {
                if (le_check_supported_features(LE_SUPPORT_FEATURES_MASK_ARRAY_INDEX3,
                                                LE_SUPPORT_FEATURES_PAST_RECIPIENT_MASK_BIT))
                {
                    T_GAP_PAST_RECIPIENT_PERIODIC_ADV_SYNC_TRANSFER_PARAM param;
                    T_BASS_GET_PA_SYNC_PARAM sync_param;
                    sync_param.source_id = p_db->brs_data.source_id;
                    sync_param.handle = p_db->handle;
                    sync_param.pa_interval = p_db->pa_interval;
                    sync_param.is_past = true;
                    ble_audio_mgr_dispatch(LE_AUDIO_MSG_BASS_GET_PA_SYNC_PARAM, &sync_param);
                    param.mode =
                        PERIODIC_ADV_SYNC_TRANSFER_MODE_PERIODIC_ADV_REPORT_ENABLED_WITH_DUPLICATE_FILTER_DISABLED;
                    param.cte_type = PAST_RECIPIENT_PERIODIC_ADV_SYNC_TRANSFER_CTE_TYPE_CTE_IRRELEVANT;
                    param.skip = p_db->pa_sync_skip;
                    param.sync_timeout = p_db->pa_sync_timeout;
                    if (ble_audio_set_past_recipient_param(conn_handle, &param))
                    {
                        T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
                        if (p_link)
                        {
                            p_link->past_mode =
                                PERIODIC_ADV_SYNC_TRANSFER_MODE_PERIODIC_ADV_REPORT_ENABLED_WITH_DUPLICATE_FILTER_DISABLED;
                        }
                    }
                    bass_pa_proc_sm_execute(p_db, BASS_PA_CP_SYNC_PAST_EVT, NULL, false);
                    return;
                }
            }
            return;
        }

        bass_pa_proc_sm_execute(p_db, BASS_PA_CP_SYNC_NO_PAST_EVT, NULL, false);
    }
    else if (pa_sync == BASS_PA_SYNC_NO_PAST)
    {
        bass_pa_proc_sm_execute(p_db, BASS_PA_CP_SYNC_NO_PAST_EVT, NULL, false);
    }
    else
    {
        bass_pa_proc_sm_execute(p_db, BASS_PA_CP_NO_SYNC_EVT, NULL, false);
    }
}

bool bass_handle_update_bis_info(T_BASS_BRS_DB *p_db, bool is_add,
                                 uint8_t num_subgroups, uint16_t bis_info_size, T_BASS_CP_BIS_INFO  *p_cp_bis_info)
{
    uint8_t i;
    uint8_t err_idx = 0;
    bool bis_info_equal = false;
    bool update_metadata = false;
    uint32_t bis_sync = 0;
    if (bis_info_size > 0)
    {
        for (i = 0; i < num_subgroups; i++)
        {
            if (p_cp_bis_info[i].bis_sync != BASS_CP_BIS_SYNC_NO_PREFER)
            {
                bis_sync |= p_cp_bis_info[i].bis_sync;
            }
            else
            {
                bis_sync = BASS_CP_BIS_SYNC_NO_PREFER;
            }
        }
        if (bis_sync == BASS_CP_BIS_SYNC_NO_PREFER && p_db->brs_data.bis_sync_state != 0)
        {
            err_idx = 1;
            goto failed;
        }
        if (p_db->brs_data.p_cp_bis_info)
        {
            if (p_db->brs_data.bis_info_size == bis_info_size &&
                p_db->brs_data.num_subgroups == num_subgroups)
            {
                for (i = 0; i < num_subgroups; i++)
                {
                    if (p_db->brs_data.p_cp_bis_info[i].metadata_len == p_cp_bis_info[i].metadata_len)
                    {
                        if (p_cp_bis_info[i].metadata_len != 0)
                        {
                            if (memcmp(p_db->brs_data.p_cp_bis_info[i].p_metadata, p_cp_bis_info[i].p_metadata,
                                       p_cp_bis_info[i].metadata_len) != 0)
                            {
                                break;
                            }
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                if (i == num_subgroups)
                {
                    bis_info_equal = true;
                }
            }
        }
        if (bis_sync == p_db->brs_data.bis_sync_state && bis_info_equal == false)
        {
            if (is_add == false)
            {
                update_metadata = true;
            }
        }
        if (bis_info_equal == false)
        {
            T_BASS_CP_BIS_INFO  *p_temp_bis_info;
            p_temp_bis_info = ble_audio_mem_zalloc(bis_info_size);
            if (p_temp_bis_info == NULL)
            {
                err_idx = 3;
                goto failed;
            }
            if (num_subgroups > 0)
            {
                uint8_t *p_meta_data = (uint8_t *)(p_temp_bis_info +
                                                   num_subgroups);
                for (i = 0; i < num_subgroups; i++)
                {
                    if (p_cp_bis_info[i].bis_sync != BASS_CP_BIS_SYNC_NO_PREFER)
                    {
                        p_temp_bis_info[i].bis_sync = p_db->brs_data.bis_sync_state & p_cp_bis_info[i].bis_sync;
                    }
                    else
                    {
                        if (p_db->brs_data.p_cp_bis_info)
                        {
                            p_temp_bis_info[i].bis_sync = p_db->brs_data.p_cp_bis_info[i].bis_sync;
                        }
                        else
                        {
                            p_temp_bis_info[i].bis_sync = 0;
                        }
                    }
                    p_temp_bis_info[i].metadata_len = p_cp_bis_info[i].metadata_len;
                    if (p_temp_bis_info[i].metadata_len > 0)
                    {
                        memcpy(p_meta_data, p_cp_bis_info[i].p_metadata,
                               p_cp_bis_info[i].metadata_len);
                        p_temp_bis_info[i].p_metadata = p_meta_data;
                        p_meta_data += p_temp_bis_info[i].metadata_len;
                    }
                }
            }
            if (p_db->brs_data.p_cp_bis_info)
            {
                ble_audio_mem_free(p_db->brs_data.p_cp_bis_info);
            }
            p_db->brs_data.p_cp_bis_info = p_temp_bis_info;
            p_db->brs_modify = true;
        }
    }
    else
    {
        if (p_db->brs_data.p_cp_bis_info)
        {
            ble_audio_mem_free(p_db->brs_data.p_cp_bis_info);
            p_db->brs_data.p_cp_bis_info = NULL;
        }
        bis_sync = 0;
        update_metadata = true;
        p_db->brs_modify = true;
    }
    if (num_subgroups)
    {
        p_db->bis_sync_info.cp_subgroup_num = num_subgroups;
        for (uint8_t i = 0; i < num_subgroups; i++)
        {
            p_db->bis_sync_info.cp_subgroup_bis[i] = p_cp_bis_info[i].bis_sync;
        }
    }
    else
    {
        p_db->bis_sync_info.cp_subgroup_num = 0;
        memset(p_db->bis_sync_info.cp_subgroup_bis, 0, BASS_SUBGROUP_MAX_NUM * sizeof(uint32_t));
    }
    if (bis_sync != 0 && p_db->brs_data.bis_sync_state != 0 &&
        bis_sync != p_db->brs_data.bis_sync_state)
    {
        p_db->bis_sync_info.cp_bis_switch = true;
    }
    else
    {
        p_db->bis_sync_info.cp_bis_switch = false;
    }
    p_db->bis_sync_info.cp_big_sync = bis_sync;
    p_db->brs_data.bis_info_size = bis_info_size;
    p_db->brs_data.num_subgroups = num_subgroups;
    p_db->send_notify = update_metadata;
    PROTOCOL_PRINT_INFO4("bass_handle_update_bis_info: bis_info_equal %d, update_metadata %d, big_sync 0x%x -> 0x%x ",
                         bis_info_equal, update_metadata, p_db->brs_data.bis_sync_state, bis_sync);
    return true;
failed:
    PROTOCOL_PRINT_ERROR1("bass_handle_update_bis_info: failed, err_idx %d", err_idx);
    return false;
}

bool bass_handle_cp_data(uint16_t conn_handle, T_BASS_CP_DATA *p_cp_data)
{
    uint8_t error_idx = 0;
    T_BASS_BRS_DB *p_db = NULL;
    bool cp_data_pending = false;
    switch (p_cp_data->cp_op)
    {
    case BASS_CP_OP_REMOTE_SCAN_STOP:
    case BASS_CP_OP_REMOTE_SCAN_START:
        return cp_data_pending;

    case BASS_CP_OP_ADD_SOURCE:
        {
            T_BLE_AUDIO_SYNC_HANDLE handle;
            p_db = bass_brs_add(p_cp_data->param.add_source.advertiser_address_type,
                                p_cp_data->param.add_source.advertiser_address,
                                p_cp_data->param.add_source.advertiser_sid,
                                p_cp_data->param.add_source.broadcast_id);
            if (p_db == NULL)
            {
                T_BASS_BRS_CHAR_NO_EMPTY no_empty_info;
                no_empty_info.conn_handle = conn_handle;
                no_empty_info.p_cp_data = p_cp_data;
                ble_audio_mgr_dispatch(LE_AUDIO_MSG_BASS_BRS_CHAR_NO_EMPTY, &no_empty_info);
                p_db = bass_brs_add(p_cp_data->param.add_source.advertiser_address_type,
                                    p_cp_data->param.add_source.advertiser_address,
                                    p_cp_data->param.add_source.advertiser_sid,
                                    p_cp_data->param.add_source.broadcast_id);
                if (p_db == NULL)
                {
                    error_idx = 1;
                    goto error;
                }
            }
            p_db->pa_interval = p_cp_data->param.add_source.pa_interval;
            handle = ble_audio_sync_find(p_cp_data->param.add_source.advertiser_sid,
                                         p_cp_data->param.add_source.broadcast_id);
            if (handle)
            {
                PROTOCOL_PRINT_WARN1("bass_handle_cp_data: add src, sync handle 0x%x already exist",
                                     handle);
                p_db->handle = handle;
            }
            else
            {
                handle = ble_audio_sync_create_int(NULL, p_cp_data->param.add_source.advertiser_address_type,
                                                   p_cp_data->param.add_source.advertiser_address, p_cp_data->param.add_source.advertiser_sid,
                                                   p_cp_data->param.add_source.broadcast_id, BLE_AUDIO_ACTION_ROLE_BASS);
                if (handle == NULL)
                {
                    error_idx = 3;
                    bass_brs_delete(p_db, false);
                    goto error;
                }
                p_db->handle = handle;
            }
            if (bass_handle_update_bis_info(p_db, true,
                                            p_cp_data->param.add_source.num_subgroups,
                                            p_cp_data->param.add_source.bis_info_size,
                                            p_cp_data->param.add_source.p_cp_bis_info) == false)
            {
                error_idx = 4;
                bass_brs_delete(p_db, false);
                goto error;

            }
            T_BASS_BA_ADD_SOURCE brs_msg;
            brs_msg.source_id = p_db->brs_data.source_id;
            brs_msg.handle = p_db->handle;
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_BASS_BA_ADD_SOURCE, &brs_msg);
            if (p_db->bis_sync_info.cp_big_sync != 0 &&
                p_cp_data->param.add_source.pa_sync == BASS_PA_NOT_SYNC)
            {
                error_idx = 5;
                bass_brs_delete(p_db, false);
                goto error;
            }
            if (p_db->bis_sync_info.cp_big_sync == 0 && p_cp_data->param.add_source.pa_sync == BASS_PA_NOT_SYNC)
            {
                p_db->send_notify = true;
            }
            bass_handle_pa_sync(conn_handle, p_db, p_cp_data->param.add_source.pa_sync);

            if (p_db->bis_sync_info.cp_big_sync == BASS_CP_BIS_SYNC_NO_PREFER)
            {
                if (p_db->p_pending_cp_data)
                {
                    ble_audio_mem_free(p_db->p_pending_cp_data);
                }
                p_db->p_pending_cp_data = p_cp_data;
                cp_data_pending = true;
            }
            if (p_db->bis_sync_info.cp_big_sync == 0 ||
                p_db->bis_sync_info.cp_bis_switch)
            {
                bass_big_proc_sm_execute(p_db, BASS_BIG_CP_NO_SYNC_EVT, NULL, false);
            }
            else
            {
                bass_big_proc_sm_execute(p_db, BASS_BIG_CP_SYNC_EVT, NULL, false);
            }
        }
        break;

    case BASS_CP_OP_MODIFY_SOURCE:
        {
            p_db = bass_brs_find_by_id(p_cp_data->param.modify_source.source_id);
            if (p_db == NULL)
            {
                error_idx = 7;
                goto error;
            }
            p_db->pa_interval = p_cp_data->param.modify_source.pa_interval;
            if (bass_handle_update_bis_info(p_db, false,
                                            p_cp_data->param.modify_source.num_subgroups,
                                            p_cp_data->param.modify_source.bis_info_size,
                                            p_cp_data->param.modify_source.p_cp_bis_info) == false)
            {
                error_idx = 8;
                goto error;
            }
            bass_handle_pa_sync(conn_handle, p_db, p_cp_data->param.modify_source.pa_sync);
            if (p_db->bis_sync_info.cp_big_sync == BASS_CP_BIS_SYNC_NO_PREFER)
            {
                if (p_db->p_pending_cp_data)
                {
                    ble_audio_mem_free(p_db->p_pending_cp_data);
                }
                p_db->p_pending_cp_data = p_cp_data;
                cp_data_pending = true;
            }
            if (p_db->bis_sync_info.cp_big_sync == 0 ||
                p_db->bis_sync_info.cp_bis_switch)
            {
                bass_big_proc_sm_execute(p_db, BASS_BIG_CP_NO_SYNC_EVT, NULL, false);
            }
            else
            {
                bass_big_proc_sm_execute(p_db, BASS_BIG_CP_SYNC_EVT, NULL, false);
                if (p_db->pa_sync_state == GAP_PA_SYNC_STATE_SYNCHRONIZED)
                {
                    bass_big_proc_sm_execute(p_db, BASS_BIG_PA_SYNC_EVT, NULL, false);
                }
            }
        }
        break;

    case BASS_CP_OP_SET_BROADCAST_CODE:
        {
            p_db = bass_brs_find_by_id(p_cp_data->param.set_broadcast_code.source_id);
            if (p_db == NULL)
            {
                error_idx = 11;
                goto error;
            }
            if (p_db->brs_data.big_encryption == BIG_BROADCAST_CODE_REQUIRED)
            {
                memcpy(p_db->big_sync_param.broadcast_code, p_cp_data->param.set_broadcast_code.broadcast_code,
                       16);
                p_db->big_info_flags |= BASS_BIG_BROADCAST_CODE_SET_FLAG;
            }
            bass_big_proc_sm_execute(p_db, BASS_BIG_SET_BROADCODE_EVT, NULL, false);
        }
        break;

    case BASS_CP_OP_REMOVE_SOURCE:
        {
            p_db = bass_brs_find_by_id(p_cp_data->param.remove_source.source_id);
            if (p_db == NULL)
            {
                error_idx = 12;
                goto error;
            }
            if (p_db->brs_data.brs_is_used == false)
            {
                return cp_data_pending;
            }
            if (p_db->brs_data.pa_sync_state == PA_SYNC_STATE_SYNC ||
                p_db->brs_data.bis_sync_state != 0x0)
            {
                error_idx = 13;
                goto error;
            }
            if (bass_delete_source_int(p_db, BLE_AUDIO_ACTION_ROLE_BASS) == false)
            {
                error_idx = 14;
                goto error;
            }
            p_db->send_notify = true;
            p_db->brs_modify = true;
        }
        break;

    default:
        break;
    }
    if (p_db->send_notify)
    {
        bass_send_all_notify(p_db);
    }
    return cp_data_pending;
error:
    PROTOCOL_PRINT_ERROR2("bass_handle_cp_data: cp_op %d, error_idx %d", p_cp_data->cp_op, error_idx);
    return cp_data_pending;
}

bool bass_cp_check_msg_len(uint8_t *p_data, uint16_t len, uint16_t *buf_len)
{
    T_BASS_CP_OP cp_op;
    bool result = true;
    *buf_len = 0;
    if (len < BASS_CP_OP_MIN_LEN)
    {
        return false;
    }
    cp_op = (T_BASS_CP_OP)p_data[0];
    switch (cp_op)
    {
    case BASS_CP_OP_REMOTE_SCAN_STOP:
        if (len != BASS_CP_OP_REMOTE_SCAN_STOP_LEN)
        {
            result = false;
        }
        break;

    case BASS_CP_OP_REMOTE_SCAN_START:
        if (len != BASS_CP_OP_REMOTE_SCAN_START_LEN)
        {
            result = false;
        }
        break;

    case BASS_CP_OP_ADD_SOURCE:
        {
            uint8_t metadata_len;
            uint16_t size;
            if (len < BASS_CP_OP_ADD_SOURCE_MIN_LEN)
            {
                result = false;
                break;
            }
            size = BASS_CP_OP_ADD_SOURCE_MIN_LEN;
            uint8_t num_subgroups = p_data[BASS_CP_OP_ADD_SOURCE_MIN_LEN - 1];
            uint8_t i;

            for (i = 0; i < num_subgroups; i++)
            {
                size += 5;
                if (size > len)
                {
                    result = false;
                    break;
                }
                metadata_len = p_data[size - 1];
                size += metadata_len;
                *buf_len += metadata_len;
            }
            if (size != len)
            {
                result = false;
            }
            else
            {
                *buf_len += sizeof(T_BASS_CP_BIS_INFO) * num_subgroups;
            }
        }
        break;

    case BASS_CP_OP_MODIFY_SOURCE:
        {
            uint8_t metadata_len;
            uint16_t size;
            if (len < BASS_CP_OP_MODIFY_SOURCE_MIN_LEN)
            {
                result = false;
                break;
            }
            size = BASS_CP_OP_MODIFY_SOURCE_MIN_LEN;
            uint8_t num_subgroups = p_data[BASS_CP_OP_MODIFY_SOURCE_MIN_LEN - 1];
            uint8_t i;

            for (i = 0; i < num_subgroups; i++)
            {
                size += 5;
                if (size > len)
                {
                    result = false;
                    break;
                }
                metadata_len = p_data[size - 1];
                size += metadata_len;
                *buf_len += metadata_len;
            }
            if (size != len)
            {
                result = false;
            }
            else
            {
                *buf_len += sizeof(T_BASS_CP_BIS_INFO) * num_subgroups;
            }
        }
        break;

    case BASS_CP_OP_SET_BROADCAST_CODE:
        if (len != BASS_CP_OP_SET_BROADCAST_CODE_LEN)
        {
            result = false;
        }
        break;

    case BASS_CP_OP_REMOVE_SOURCE:
        if (len != BASS_CP_OP_REMOVE_SOURCE_LEN)
        {
            result = false;
        }
        break;

    default:
        result = false;
        break;
    }
    if (result == false)
    {
        PROTOCOL_PRINT_ERROR2("bass_cp_check_msg_len: failed, cp_op %d, len %d", cp_op, len);
    }
    return result;
}

uint16_t bass_cp_prase_data(uint8_t *p_data, uint16_t len, T_BASS_CP_DATA **pp_cp_data)
{
    uint16_t idx = 0;
    uint8_t source_id = 0xff;
    T_BASS_CP_DATA *p_cp_data;
    T_BASS_CP_OP opcode;
    uint16_t buf_len = 0;

    if (p_data == NULL || pp_cp_data == NULL)
    {
        return (ATT_ERR | ATT_ERR_WRITE_REQUEST_REJECTED);
    }

    opcode = (T_BASS_CP_OP)p_data[idx++];

    if (opcode > BASS_CP_OP_REMOVE_SOURCE)
    {
        PROTOCOL_PRINT_ERROR0("[BASS] bass_cp_prase_data: invalid opcode");
        return  ATT_ERR_BASS_OPCODE_NOT_SUPPORT;
    }


    if (bass_cp_check_msg_len(p_data, len, &buf_len) == false)
    {
        return (ATT_ERR | ATT_ERR_WRITE_REQUEST_REJECTED);
    }
    *pp_cp_data = ble_audio_mem_zalloc(sizeof(T_BASS_CP_DATA) + buf_len);
    if (*pp_cp_data == NULL)
    {
        PROTOCOL_PRINT_INFO0("[BASS] bass_cp_prase_data: alloc T_BASS_CP_DATA fail");
        return (ATT_ERR | ATT_ERR_WRITE_REQUEST_REJECTED);
    }
    p_cp_data = *pp_cp_data;
    p_cp_data->cp_op = opcode;
    switch (p_cp_data->cp_op)
    {
    case BASS_CP_OP_REMOTE_SCAN_STOP:
        PROTOCOL_PRINT_INFO0("[BASS] bass_cp_prase_data: BASS_CP_OP_REMOTE_SCAN_STOP");
        break;
    case BASS_CP_OP_REMOTE_SCAN_START:
        PROTOCOL_PRINT_INFO0("[BASS] bass_cp_prase_data: BASS_CP_OP_REMOTE_SCAN_START");
        break;
    case BASS_CP_OP_ADD_SOURCE:
        {
            uint8_t i;
            uint8_t *p_meta_data;
            T_BASS_CP_BIS_INFO  *p_cp_bis_info;
            uint32_t bis_sync_check = 0;

            p_cp_data->param.add_source.bis_info_size = buf_len;
            p_cp_bis_info = (T_BASS_CP_BIS_INFO *)(p_cp_data + 1);
            p_cp_data->param.add_source.p_cp_bis_info = p_cp_bis_info;
            p_cp_data->param.add_source.advertiser_address_type = p_data[idx++];
            memcpy(p_cp_data->param.add_source.advertiser_address, &p_data[idx], GAP_BD_ADDR_LEN);
            idx += GAP_BD_ADDR_LEN;
            p_cp_data->param.add_source.advertiser_sid = p_data[idx++];
            memcpy(p_cp_data->param.add_source.broadcast_id, &p_data[idx], BROADCAST_ID_LEN);
            idx += BROADCAST_ID_LEN;
            p_cp_data->param.add_source.pa_sync = (T_BASS_PA_SYNC)p_data[idx++];
            LE_ARRAY_TO_UINT16(p_cp_data->param.add_source.pa_interval, &p_data[idx]);
            idx += 2;
            p_cp_data->param.add_source.num_subgroups = p_data[idx++];
            //find meta data buffer pointer
            PROTOCOL_PRINT_INFO3("[BASS] bass_cp_prase_data: BASS_CP_OP_ADD_SOURCE, advertiser_address_type %d, advertiser_address %s, advertiser_sid 0x%x",
                                 p_cp_data->param.add_source.advertiser_address_type,
                                 TRACE_BDADDR(p_cp_data->param.add_source.advertiser_address),
                                 p_cp_data->param.add_source.advertiser_sid);
            PROTOCOL_PRINT_INFO3("[BASS] bass_cp_prase_data: broadcast_id[%b], pa_sync %d, pa_interval 0x%x",
                                 TRACE_BINARY(BROADCAST_ID_LEN, p_cp_data->param.add_source.broadcast_id),
                                 p_cp_data->param.add_source.pa_sync,
                                 p_cp_data->param.add_source.pa_interval);
            p_meta_data = (uint8_t *)(p_cp_bis_info + p_cp_data->param.add_source.num_subgroups);
            for (i = 0; i < p_cp_data->param.add_source.num_subgroups; i++)
            {
                LE_ARRAY_TO_UINT32(p_cp_bis_info[i].bis_sync, &p_data[idx]);
                if (p_cp_bis_info[i].bis_sync != BASS_CP_BIS_SYNC_NO_PREFER)
                {
                    if ((bis_sync_check & p_cp_bis_info[i].bis_sync) != 0)
                    {
                        return (ATT_ERR | ATT_ERR_WRITE_REQUEST_REJECTED);
                    }
                    bis_sync_check |= p_cp_bis_info[i].bis_sync;
                }
                idx += 4;
                PROTOCOL_PRINT_INFO2("[BASS] bass_cp_prase_data: subgroup[%d]: bis_sync 0x%x",
                                     i, p_cp_bis_info[i].bis_sync);
                p_cp_bis_info[i].metadata_len = p_data[idx++];
                if (p_cp_bis_info[i].metadata_len > 0)
                {
                    memcpy(p_meta_data, &p_data[idx], p_cp_bis_info[i].metadata_len);
                    idx += p_cp_bis_info[i].metadata_len;
                    p_cp_bis_info[i].p_metadata = p_meta_data;
                    p_meta_data += p_cp_bis_info[i].metadata_len;
                    PROTOCOL_PRINT_INFO2("[BASS] bass_cp_prase_data: metadata[%d]: %b",
                                         p_cp_bis_info[i].metadata_len,
                                         TRACE_BINARY(p_cp_bis_info[i].metadata_len, p_cp_bis_info[i].p_metadata));
                }
            }
            if (p_cp_data->param.add_source.pa_sync > BASS_PA_SYNC_NO_PAST ||
                p_cp_data->param.add_source.advertiser_address_type > GAP_REMOTE_ADDR_LE_RANDOM ||
                p_cp_data->param.add_source.advertiser_sid > 0x0f)
            {
                PROTOCOL_PRINT_ERROR0("[BASS] bass_cp_prase_data: BASS_CP_OP_ADD_SOURCE, invalid parameter");
                return (ATT_ERR | ATT_ERR_MIN_APPLIC_CODE);
            }
        }
        break;

    case BASS_CP_OP_MODIFY_SOURCE:
        {
            uint8_t i;
            uint8_t *p_meta_data;
            T_BASS_CP_BIS_INFO  *p_cp_bis_info;
            uint32_t bis_sync_check = 0;

            p_cp_data->param.modify_source.bis_info_size = buf_len;
            p_cp_bis_info = (T_BASS_CP_BIS_INFO *)(p_cp_data + 1);
            p_cp_data->param.modify_source.p_cp_bis_info = p_cp_bis_info;

            p_cp_data->param.modify_source.source_id = p_data[idx++];
            source_id = p_cp_data->param.modify_source.source_id;
            p_cp_data->param.modify_source.pa_sync = (T_BASS_PA_SYNC)p_data[idx++];
            LE_ARRAY_TO_UINT16(p_cp_data->param.modify_source.pa_interval, &p_data[idx]);
            idx += 2;
            p_cp_data->param.modify_source.num_subgroups = p_data[idx++];
            PROTOCOL_PRINT_INFO3("[BASS] bass_cp_prase_data: BASS_CP_OP_MODIFY_SOURCE, source_id %d, pa_sync %d, pa_interval 0x%x",
                                 p_cp_data->param.modify_source.source_id,
                                 p_cp_data->param.modify_source.pa_sync,
                                 p_cp_data->param.modify_source.pa_interval);
            //find meta data buffer pointer
            p_meta_data = (uint8_t *)(p_cp_bis_info + p_cp_data->param.modify_source.num_subgroups);
            for (i = 0; i < p_cp_data->param.modify_source.num_subgroups; i++)
            {
                LE_ARRAY_TO_UINT32(p_cp_bis_info[i].bis_sync, &p_data[idx]);
                if (p_cp_bis_info[i].bis_sync != BASS_CP_BIS_SYNC_NO_PREFER)
                {
                    if ((bis_sync_check & p_cp_bis_info[i].bis_sync) != 0)
                    {
                        return (ATT_ERR | ATT_ERR_WRITE_REQUEST_REJECTED);
                    }
                    bis_sync_check |= p_cp_bis_info[i].bis_sync;
                }
                idx += 4;
                PROTOCOL_PRINT_INFO2("[BASS] bass_cp_prase_data: subgroup[%d]: bis_sync 0x%x",
                                     i, p_cp_bis_info[i].bis_sync);
                p_cp_bis_info[i].metadata_len = p_data[idx++];
                if (p_cp_bis_info[i].metadata_len > 0)
                {
                    memcpy(p_meta_data, &p_data[idx], p_cp_bis_info[i].metadata_len);
                    idx += p_cp_bis_info[i].metadata_len;
                    p_cp_bis_info[i].p_metadata = p_meta_data;
                    p_meta_data += p_cp_bis_info[i].metadata_len;
                    PROTOCOL_PRINT_INFO2("[BASS] bass_cp_prase_data: metadata[%d]: %b",
                                         p_cp_bis_info[i].metadata_len,
                                         TRACE_BINARY(p_cp_bis_info[i].metadata_len, p_cp_bis_info[i].p_metadata));
                }
            }

            if (p_cp_data->param.modify_source.pa_sync > BASS_PA_SYNC_NO_PAST)
            {
                PROTOCOL_PRINT_ERROR0("[BASS] bass_cp_prase_data: BASS_CP_OP_MODIFY_SOURCE, invalid parameter");
                return (ATT_ERR | ATT_ERR_MIN_APPLIC_CODE);
            }
        }
        break;

    case BASS_CP_OP_SET_BROADCAST_CODE:
        {
            p_cp_data->param.set_broadcast_code.source_id = p_data[idx++];
            source_id = p_cp_data->param.set_broadcast_code.source_id;
            memcpy(p_cp_data->param.set_broadcast_code.broadcast_code, &p_data[idx], BROADCAST_CODE_LEN);
            PROTOCOL_PRINT_INFO2("[BASS] bass_cp_prase_data: BASS_CP_OP_SET_BROADCAST_CODE, source_id %d, broadcast_code %b",
                                 p_cp_data->param.set_broadcast_code.source_id,
                                 TRACE_BINARY(BROADCAST_CODE_LEN, p_cp_data->param.set_broadcast_code.broadcast_code));
        }
        break;

    case BASS_CP_OP_REMOVE_SOURCE:
        {
            p_cp_data->param.remove_source.source_id = p_data[idx];
            source_id = p_cp_data->param.remove_source.source_id;
            PROTOCOL_PRINT_INFO1("[BASS] bass_cp_prase_data: BASS_CP_OP_REMOVE_SOURCE, source_id %d",
                                 p_cp_data->param.remove_source.source_id);
        }
        break;

    default:
        break;
    }
    if (source_id != 0xff)
    {
        T_BASS_BRS_DB *p_db = bass_brs_find_by_id(source_id);
        if (p_db == NULL)
        {
            PROTOCOL_PRINT_ERROR1("[BASS] bass_cp_prase_data: not find source_id %d", source_id);
            return  ATT_ERR_BASS_INVALID_SRC_ID;
        }
    }
    return ATT_SUCCESS;
}

uint8_t bass_get_source_id(uint16_t attrib_index, bool cccd)
{
    uint8_t source_id = 0;
    uint16_t start_handle = BASS_CHAR_BRS_INDEX;
    if (cccd)
    {
        start_handle = BASS_CHAR_BRS_CCCD_INDEX;
    }
    if (attrib_index >= start_handle)
    {
        uint16_t index = attrib_index - start_handle;
        source_id = (index / 3) + 1;
        if (index % 3 != 0)
        {
            PROTOCOL_PRINT_ERROR1("bass_get_source_id: failed, attrib_index %d", attrib_index);
            return 0xff;
        }
    }
    PROTOCOL_PRINT_INFO2("bass_get_source_id: attrib_index %d, source_id %d", attrib_index, source_id);
    return source_id;
}

T_APP_RESULT bass_attr_read_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                               uint16_t attrib_index,
                               uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    uint8_t *p_data = NULL;
    uint16_t len;
    uint8_t source_id = bass_get_source_id(attrib_index, false);
    PROTOCOL_PRINT_INFO2("bass_attr_read_cb: attrib_index %d, offset %x", attrib_index, offset);
    if (source_id == BASS_INVALID_SOURCE_ID)
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    if (bass_brs_gen_char_data(source_id,
                               &p_data,
                               &len))
    {
        if (offset > len)
        {
            cause = APP_RESULT_INVALID_OFFSET;
        }
        else
        {
            if (gatt_svc_read_confirm(conn_handle, cid, p_bass->service_id,
                                      attrib_index,
                                      p_data + offset,
                                      len - offset, len, APP_RESULT_SUCCESS))
            {
                cause = APP_RESULT_PENDING;
            }
        }
        if (p_data)
        {
            ble_audio_mem_free(p_data);
        }
    }
    return (cause);
}

T_APP_RESULT bass_attr_write_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                uint16_t attrib_index,
                                T_WRITE_TYPE write_type,
                                uint16_t length, uint8_t *p_value, P_FUN_EXT_WRITE_IND_POST_PROC *p_write_post_proc)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;

    if (attrib_index == BASS_CHAR_BAS_CP_INDEX)
    {
        if (p_value != NULL)
        {
            uint16_t ret;
            T_BASS_CP_IND remote_cp;
            remote_cp.conn_handle = conn_handle;
            remote_cp.p_cp_data = NULL;
            PROTOCOL_PRINT_INFO2("bass_callback: control point data[%d]: %b",
                                 length, TRACE_BINARY(length, p_value));
            ret = bass_cp_prase_data(p_value,
                                     length, &remote_cp.p_cp_data);
            if (ret == ATT_SUCCESS)
            {
                uint16_t cb_result;
                //The decision of the server whether to accept an operation written by a client to the Broadcast Audio
                //Scan Control Point characteristic is left to the implementation unless defined by higher layers.
                cb_result = ble_audio_mgr_dispatch(LE_AUDIO_MSG_BASS_CP_IND, &remote_cp);
                if (cb_result == BLE_AUDIO_CB_RESULT_SUCCESS)
                {
                    if (bass_handle_cp_data(conn_handle, remote_cp.p_cp_data))
                    {
                        remote_cp.p_cp_data = NULL;
                    }
                }
                cause = (T_APP_RESULT)cb_result;
            }
            else
            {
                cause = (T_APP_RESULT)ret;
            }
            if (remote_cp.p_cp_data)
            {
                ble_audio_mem_free(remote_cp.p_cp_data);
            }
        }
    }
    else
    {
        cause = APP_RESULT_ATTR_NOT_FOUND;
    }

    return (cause);
}

void bass_cccd_update_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                         uint16_t attrib_index,
                         uint16_t ccc_bits)
{
    uint16_t bass_cccd_flag = 0x0001;
    uint8_t source_id;
    T_LE_SRV_CFG *p_srv = ble_srv_alloc_by_srv_id(conn_handle, service_id);
    if (p_srv == NULL)
    {
        return;
    }
    ble_audio_mgr_send_server_cccd_info(conn_handle, bass_attr_tbl, service_id, attrib_index, ccc_bits,
                                        0, bass_attr_num);
    source_id = bass_get_source_id(attrib_index, true);
    if (source_id == BASS_INVALID_SOURCE_ID)
    {
        return;
    }
    bass_cccd_flag = bass_cccd_flag << source_id;
    PROTOCOL_PRINT_INFO3("bass_cccd_update_cb: source_id %d, ccc_bits 0x%x, bass_cccd_flag 0x%x",
                         source_id, ccc_bits, bass_cccd_flag);
    ble_srv_set_cccd_flags(p_srv, ccc_bits,  0, bass_cccd_flag,
                           true);

    return;
}

const T_FUN_GATT_EXT_SERVICE_CBS bass_cbs =
{
    bass_attr_read_cb,  // Read callback function pointer
    bass_attr_write_cb, // Write callback function pointer
    bass_cccd_update_cb  // CCCD update callback function pointer
};

bool bass_add_service(uint8_t brs_num)
{
    T_SERVER_ID service_id;
    uint16_t attr_tbl_size = sizeof(bass_attr_tbl);

    attr_tbl_size -= (BASS_BRS_CHAR_MAX_NUM - brs_num) * 3 * sizeof(T_ATTRIB_APPL);

    bass_attr_num = attr_tbl_size / sizeof(T_ATTRIB_APPL);

    if (false == gatt_svc_add(&service_id,
                              (uint8_t *)bass_attr_tbl,
                              attr_tbl_size,
                              &bass_cbs, NULL))
    {
        PROTOCOL_PRINT_ERROR0("bass_add_service: failed");
        return false;
    }
    p_bass->service_id = service_id;

    return true;
}

static void bass_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BLE_DM_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
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

                if (p_bass == NULL)
                {
                    return;
                }

                if (ble_audio_ual_check_le_bond(p_link->conn_handle))
                {
                    T_LE_SRV_CFG *p_srv;
                    p_srv = ble_srv_find_by_srv_id(p_link->conn_handle, p_bass->service_id);
                    if (p_srv != NULL && p_srv->cccd_pending[0] != 0)
                    {
                        uint8_t source_id = 0;
                        uint16_t bass_cccd_flag;
                        uint16_t cccd_handle;
                        uint16_t attrib_idx;

                        for (uint8_t i = 0; i < p_bass->brs_num; i++)
                        {
                            if (p_bass->brs_tbl[i].brs_data.brs_is_used)
                            {
                                source_id = p_bass->brs_tbl[i].brs_data.source_id;
                                bass_cccd_flag = 0x0001 << source_id;

                                if (p_srv->cccd_pending[0] & bass_cccd_flag)
                                {
                                    attrib_idx = BASS_CHAR_BRS_INDEX + 3 * (source_id - 1);
                                    cccd_handle = server_get_start_handle(p_bass->service_id);
                                    cccd_handle += (attrib_idx + 1);
                                    bass_send_brs_notify(p_link, attrib_idx, &p_bass->brs_tbl[i]);
                                    if (ble_audio_ual_clear_pending_cccd(p_link->conn_handle, cccd_handle))
                                    {
                                        p_srv->cccd_pending[0] &= ~bass_cccd_flag;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        break;

    default:
        break;
    }
}

bool bass_init(uint8_t brs_num)
{
    uint8_t err_idx = 0;
#if LE_AUDIO_INIT_DEBUG
    PROTOCOL_PRINT_INFO1("bass_init: brs_num %d", brs_num);
#endif
    ble_dm_cback_register(bass_dm_cback);
    if (brs_num < 1 || brs_num > BASS_BRS_CHAR_MAX_NUM)
    {
        err_idx = 1;
        goto failed;
    }
    p_bass = ble_audio_mem_zalloc(sizeof(T_BASS_CB) + brs_num * sizeof(T_BASS_BRS_DB));
    if (p_bass == NULL)
    {
        err_idx = 2;
        goto failed;
    }

    if (bass_add_service(brs_num) == false)
    {
        err_idx = 3;
        goto failed;
    }
    for (uint8_t i = 0; i < brs_num; i++)
    {
        p_bass->brs_tbl[i].brs_data.source_id = i + 1;
    }
    p_bass->brs_num = brs_num;
#if LE_AUDIO_BROADCAST_SINK_ROLE
    ble_audio_sync_register_bass_cb(bass_audio_sync_cb);
#endif
    return true;
failed:
    if (p_bass)
    {
        ble_audio_mem_free(p_bass);
        p_bass = NULL;
    }
    PROTOCOL_PRINT_ERROR1("bass_init: failed, err_idx %d", err_idx);
    return false;
}

#if LE_AUDIO_DEINIT
void bass_deinit(void)
{
    if (p_bass)
    {
        for (uint8_t i = 0; i < p_bass->brs_num; i++)
        {
            T_BASS_BRS_DB *p_db = &p_bass->brs_tbl[i];
            if (p_db)
            {
                if (p_db->brs_data.p_cp_bis_info)
                {
                    ble_audio_mem_free(p_db->brs_data.p_cp_bis_info);
                }
                if (p_db->p_past_timer_handle != NULL)
                {
                    sys_timer_delete(p_db->p_past_timer_handle);
                }
                if (p_db->p_pending_cp_data)
                {
                    ble_audio_mem_free(p_db->p_pending_cp_data);
                }
            }
        }
        ble_audio_mem_free(p_bass);
        p_bass = NULL;
    }
}
#endif

#endif
