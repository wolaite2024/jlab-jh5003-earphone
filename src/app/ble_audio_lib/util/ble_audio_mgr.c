#include <string.h>
#include "trace.h"
#include "ble_link_util.h"
#include "ble_audio_mgr.h"
#include "ble_audio_dm.h"
#include "bt_gatt_client.h"
#include "os_msg.h"
#include "ble_audio_group_int.h"
#include "ble_audio_ual.h"
#include "gap_bond_manager.h"
#include "bt_gatt_svc.h"
#include "gap_privacy.h"
#if CONFIG_REALTEK_BLE_USE_UAL_API
#include "ual_adapter.h"
#include "adapter_int.h"
#include "broadcast_source_sm.h"
#include "ble_extend_adv.h"
#include "ble_pa_sync.h"
#include "ble_audio_sync.h"
#include "bap_int.h"
#endif
#if LE_AUDIO_BROADCAST_SOURCE_ROLE
extern void broadcast_source_handle_gap_cb(uint8_t cb_type, T_LE_CB_DATA *p_data);
#endif
#if LE_AUDIO_BROADCAST_SINK_ROLE
void ble_audio_sync_handle_gap_cb(uint8_t cb_type, T_LE_CB_DATA *p_data);
#endif

typedef struct t_ble_audio_cback_item
{
    struct t_ble_audio_cback_item *p_next;
    P_FUN_BLE_AUDIO_CB             cback;
} T_BLE_AUDIO_CBACK_ITEM;

extern const T_BT_BOND_MGR bt_bond_gap;

bool ble_audio_check_remote_features(uint16_t conn_handle, uint8_t array_index,
                                     uint8_t feature_mask)
{
    T_BLE_AUDIO_LINK *p_link;

    p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link && (p_link->remote_feats[array_index] & feature_mask))
    {
        return true;
    }

    return false;
}

void ble_audio_handle_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state,
                                     uint16_t disc_cause)
{
    T_BLE_AUDIO_LINK *p_link;
    if (conn_id >= ble_audio_db.acl_link_num)
    {
        return;
    }

    p_link = ble_audio_link_find_by_conn_id(conn_id);
    switch (new_state)
    {
    case GAP_CONN_STATE_DISCONNECTING:
        if (p_link != NULL)
        {
            p_link->state = GAP_CONN_STATE_DISCONNECTING;
        }
        break;

    case GAP_CONN_STATE_DISCONNECTED:
        if (p_link != NULL)
        {
            T_BLE_DM_EVENT_PARAM_CONN_STATE param;
            param.conn_id = conn_id;
            param.state = GAP_CONN_STATE_DISCONNECTED;
            param.disc_cause = disc_cause;
            ble_dm_event_post(BLE_DM_EVENT_CONN_STATE, &param, sizeof(param));
            ble_audio_link_free(p_link);
        }
        break;

    case GAP_CONN_STATE_CONNECTING:
        if (p_link == NULL)
        {
            uint8_t svc_num = gatt_svc_get_num();
            p_link = ble_audio_link_alloc_by_conn_id(conn_id, svc_num);
            if (p_link != NULL)
            {
                p_link->state = GAP_CONN_STATE_CONNECTING;
            }
        }
        break;

    case GAP_CONN_STATE_CONNECTED:
        if (p_link != NULL)
        {
            T_BLE_DM_EVENT_PARAM_CONN_STATE param;
            p_link->state = GAP_CONN_STATE_CONNECTED;
            p_link->conn_handle = le_get_conn_handle(conn_id);
            le_get_conn_param(GAP_PARAM_CONN_MTU_SIZE, &p_link->mtu_size, conn_id);
            le_get_conn_addr(conn_id, p_link->remote_bd, &p_link->remote_bd_type);
            if (p_link->remote_bd_type == GAP_REMOTE_ADDR_LE_RANDOM &&
                (p_link->remote_bd[5] & 0xC0) == 0x40)
            {
                p_link->is_rpa = true;
                if (ble_audio_ual_resolve_rpa(p_link->remote_bd, p_link->identity_addr,
                                              &p_link->identity_addr_type) == false)
                {
                    p_link->identity_addr_type = 0xff;
                }
            }
            else if (p_link->remote_bd_type == GAP_REMOTE_ADDR_LE_PUBLIC_IDENTITY ||
                     p_link->remote_bd_type == GAP_REMOTE_ADDR_LE_RANDOM_IDENTITY)
            {
                p_link->identity_addr_type = le_privacy_convert_addr_type((T_GAP_REMOTE_ADDR_TYPE)
                                                                          p_link->remote_bd_type);
                p_link->is_rpa = true;
                memcpy(p_link->identity_addr, p_link->remote_bd, GAP_BD_ADDR_LEN);

            }

            param.conn_id = conn_id;
            param.state = GAP_CONN_STATE_CONNECTED;
            param.disc_cause = disc_cause;
            ble_dm_event_post(BLE_DM_EVENT_CONN_STATE, &param, sizeof(param));
        }
        break;

    default:
        break;
    }
}

void ble_audio_handle_gap_msg(uint16_t subtype, T_LE_GAP_MSG gap_msg)
{
#if LE_AUDIO_DEINIT
    if (ble_audio_deinit_flow != 0)
    {
        return;
    }
#endif
    switch (subtype)
    {
    case GAP_MSG_LE_DEV_STATE_CHANGE:
        ble_audio_db.dev_state = gap_msg.msg_data.gap_dev_state_change.new_state;
        break;

    case GAP_MSG_LE_CONN_STATE_CHANGE:
        {
            ble_audio_handle_conn_state_evt(gap_msg.msg_data.gap_conn_state_change.conn_id,
                                            (T_GAP_CONN_STATE)gap_msg.msg_data.gap_conn_state_change.new_state,
                                            gap_msg.msg_data.gap_conn_state_change.disc_cause);
        }
        break;

    case GAP_MSG_LE_CONN_MTU_INFO:
        {
            T_BLE_DM_EVENT_PARAM_MTU_INFO param;
            param.conn_id = gap_msg.msg_data.gap_conn_mtu_info.conn_id;
            param.mtu_size = gap_msg.msg_data.gap_conn_mtu_info.mtu_size;
            T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_id(param.conn_id);
            if (p_link != NULL)
            {
                p_link->mtu_size = param.mtu_size;
            }
            ble_dm_event_post(BLE_DM_EVENT_MTU_INFO, &param, sizeof(param));
        }
        break;

    case GAP_MSG_LE_AUTHEN_STATE_CHANGE:
        {
            T_BLE_DM_EVENT_PARAM_AUTHEN_INFO param;
            param.conn_id = gap_msg.msg_data.gap_authen_state.conn_id;
            param.new_state = gap_msg.msg_data.gap_authen_state.new_state;
            param.cause = gap_msg.msg_data.gap_authen_state.status;
            if (gap_msg.msg_data.gap_authen_state.status == GAP_SUCCESS &&
                gap_msg.msg_data.gap_authen_state.new_state == GAP_AUTHEN_STATE_COMPLETE)
            {
                T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_id(
                                               gap_msg.msg_data.gap_authen_state.conn_id);
                if (p_link != NULL && p_link->is_rpa && p_link->identity_addr_type == 0xff)
                {
                    (void)ble_audio_ual_resolve_rpa(p_link->remote_bd, p_link->identity_addr,
                                                    &p_link->identity_addr_type);
                }
            }
            ble_dm_event_post(BLE_DM_EVENT_AUTHEN_INFO, &param, sizeof(param));
        }
        break;

    default:
        break;
    }
}

void ble_audio_bond_modify(T_BT_BOND_MODIFY *p_value)
{
    ble_dm_event_post(BLE_DM_EVENT_BOND_MODIFY, p_value, sizeof(T_BT_BOND_MODIFY));
}

void ble_audio_handle_gap_cb(uint8_t cb_type, void *p_cb_data)
{
    T_LE_CB_DATA *p_data = (T_LE_CB_DATA *)p_cb_data;
#if LE_AUDIO_DEINIT
    if (ble_audio_deinit_flow != 0)
    {
        return;
    }
#endif
    switch (cb_type)
    {
    case GAP_MSG_LE_REMOTE_FEATS_INFO:
        PROTOCOL_PRINT_INFO3("GAP_MSG_LE_REMOTE_FEATS_INFO: conn id %d, cause 0x%x, remote_feats %b",
                             p_data->p_le_remote_feats_info->conn_id,
                             p_data->p_le_remote_feats_info->cause,
                             TRACE_BINARY(8, p_data->p_le_remote_feats_info->remote_feats));
        if (p_data->p_le_remote_feats_info->cause == GAP_SUCCESS)
        {
            T_BLE_AUDIO_LINK *p_link;
            p_link = ble_audio_link_find_by_conn_id(p_data->p_le_remote_feats_info->conn_id);
            if (p_link)
            {
                memcpy(p_link->remote_feats, p_data->p_le_remote_feats_info->remote_feats, 8);
            }
        }
        break;
    case GAP_MSG_LE_BOND_MODIFY_INFO:
        {
            T_BT_BOND_MODIFY bond_modify = {(T_BT_BOND_MODIFY_TYPE)0};

            if (p_data->p_le_bond_modify_info->type == LE_BOND_DELETE)
            {
                bond_modify.type = BT_BOND_DELETE;
            }
            else if (p_data->p_le_bond_modify_info->type == LE_BOND_CLEAR)
            {
                bond_modify.type = BT_BOND_CLEAR;
            }
            else if (p_data->p_le_bond_modify_info->type == LE_BOND_ADD)
            {
                bond_modify.type = BT_BOND_ADD;
            }
            else
            {
                break;
            }
            if (p_data->p_le_bond_modify_info->p_entry)
            {
                memcpy(bond_modify.remote_addr, p_data->p_le_bond_modify_info->p_entry->remote_bd.addr, 6);
                bond_modify.remote_bd_type = p_data->p_le_bond_modify_info->p_entry->remote_bd.remote_bd_type;
            }
            ble_audio_bond_modify(&bond_modify);
        }
        break;

    default:
        break;
    }
#if LE_AUDIO_BROADCAST_SOURCE_ROLE
    broadcast_source_handle_gap_cb(cb_type, p_data);
#endif
#if LE_AUDIO_BROADCAST_SINK_ROLE
    ble_audio_sync_handle_gap_cb(cb_type, p_data);
#endif
}

void ble_audio_mgr_send_server_cccd_info(uint16_t conn_handle, const T_ATTRIB_APPL *p_srv,
                                         T_SERVER_ID service_id, uint16_t attrib_index,
                                         uint16_t ccc_bits, uint32_t param, uint16_t attr_num)
{
    T_SERVER_ATTR_CCCD_INFO cccd_info;
    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_srv, attrib_index, attr_num);

    if (char_uuid.uuid_size != UUID_16BIT_SIZE)
    {
        PROTOCOL_PRINT_ERROR2("ble_audio_mgr_send_server_cccd_info: invalid uuid, service_id %d, attrib_index 0x%x",
                              service_id, attrib_index);
        return;
    }


    cccd_info.conn_handle = conn_handle;
    cccd_info.service_id = service_id;
    cccd_info.char_uuid = char_uuid.uu.char_uuid16;
    cccd_info.char_attrib_index = char_uuid.index;
    cccd_info.ccc_bits = ccc_bits;
    cccd_info.param = param;

    ble_audio_mgr_dispatch(LE_AUDIO_MSG_SERVER_ATTR_CCCD_INFO, &cccd_info);
}

void ble_audio_mgr_dispatch_client_attr_cccd_info(uint16_t conn_handle,
                                                  T_GATT_CLIENT_CCCD_CFG *p_cfg)
{
    if (p_cfg->cause)
    {
        T_CLIENT_ATTR_CCCD_INFO cccd_info = {0};

        cccd_info.cause = p_cfg->cause;
        cccd_info.srv_instance_id = p_cfg->srv_instance_id;
        cccd_info.conn_handle = conn_handle;
        cccd_info.cccd_cfg =  p_cfg->cccd_cfg;
        cccd_info.srv_cfg =  p_cfg->srv_cfg;
        cccd_info.srv_uuid =  p_cfg->srv_uuid.p.uuid16;
        if (p_cfg->srv_cfg == false)
        {
            cccd_info.char_instance_id = p_cfg->char_uuid.instance_id;
            cccd_info.char_uuid = p_cfg->char_uuid.p.uuid16;
        }

        ble_audio_mgr_dispatch(LE_AUDIO_MSG_CLIENT_ATTR_CCCD_INFO, &cccd_info);
    }
}

#if LE_AUDIO_IO_MESSAGE_HANDLER
#if LE_AUDIO_DEINIT
typedef struct t_ble_audio_pending_msg
{
    struct t_ble_audio_pending_msg *p_next;
    void *p_buf;
} T_BLE_AUDIO_PENDING_MSG;

T_OS_QUEUE ble_audio_pending_msg_queue;

T_BLE_AUDIO_PENDING_MSG *ble_audio_pending_msg_find(void *p_buf)
{
    T_BLE_AUDIO_PENDING_MSG *p_msg;
    for (uint8_t i = 0; i < ble_audio_pending_msg_queue.count; i++)
    {
        p_msg = (T_BLE_AUDIO_PENDING_MSG *)os_queue_peek(&ble_audio_pending_msg_queue, i);
        if (p_msg->p_buf == p_buf)
        {
            return p_msg;
        }
    }
    return NULL;
}

void ble_audio_pending_msg_add(void *p_buf)
{
    T_BLE_AUDIO_PENDING_MSG *p_msg;
    p_msg = ble_audio_mem_zalloc(sizeof(T_BLE_AUDIO_PENDING_MSG));
    if (p_msg)
    {
        p_msg->p_buf = p_buf;
        os_queue_in(&ble_audio_pending_msg_queue, p_msg);
    }
}

void ble_audio_pending_msg_del(void *p_buf)
{
    T_BLE_AUDIO_PENDING_MSG *p_msg = ble_audio_pending_msg_find(p_buf);

    if (p_msg)
    {
        os_queue_delete(&ble_audio_pending_msg_queue, p_msg);
        ble_audio_mem_free(p_msg);
    }
}

void ble_audio_pending_msg_free(void)
{
    T_BLE_AUDIO_PENDING_MSG *p_msg;

    while ((p_msg = os_queue_out(&ble_audio_pending_msg_queue)) != NULL)
    {
        if (p_msg->p_buf)
        {
            ble_audio_mem_free(p_msg->p_buf);
        }
        ble_audio_mem_free(p_msg);
    }
}
#endif

bool ble_audio_send_msg_to_app(uint16_t subtype, void *buf)
{
    T_IO_MSG io_msg;
    uint8_t event = EVENT_IO_TO_APP;
    io_msg.type = ble_audio_db.io_event_type;
    io_msg.subtype = subtype;
    io_msg.u.buf = buf;
    if (ble_audio_db.io_queue_handle == NULL ||
        ble_audio_db.evt_queue_handle == NULL)
    {
        PROTOCOL_PRINT_ERROR0("ble_audio_send_msg_to_app handle is NULL");
        return false;
    }
    if (os_msg_send(ble_audio_db.io_queue_handle, &io_msg, 0) == false)
    {
        PROTOCOL_PRINT_ERROR0("ble_audio_send_msg_to_app fail1");
        return false;
    }
    else if (os_msg_send(ble_audio_db.evt_queue_handle, &event, 0) == false)
    {
        PROTOCOL_PRINT_ERROR0("ble_audio_send_msg_to_app fail2");
    }
#if LE_AUDIO_DEINIT
    ble_audio_pending_msg_add(buf);
#endif
    return true;
}

void ble_audio_handle_register(uint8_t id, const P_BLE_AUDIO_EVT_HDLR p_reg)
{
    if (id >= BLE_AUDIO_ID_MAX)
    {
        return;
    }
    ble_audio_db.hdl_reg[id] = p_reg;
    ble_audio_db.is_reg[id] = true;
}
#endif

void ble_audio_handle_msg(T_IO_MSG *p_io_msg)
{
#if LE_AUDIO_IO_MESSAGE_HANDLER
    uint8_t id;
#if LE_AUDIO_DEINIT
    if (ble_audio_deinit_flow != 0)
    {
        if (p_io_msg->u.buf)
        {
            ble_audio_pending_msg_del(p_io_msg->u.buf);
            ble_audio_mem_free(p_io_msg->u.buf);
        }
        return;
    }
#endif
    /* get subsystem id from event */
    id = (uint8_t)(p_io_msg->subtype >> 8);

    /* verify id and call subsystem event handler */
    if ((id < BLE_AUDIO_ID_MAX) && (ble_audio_db.hdl_reg[id] != NULL))
    {
        (ble_audio_db.hdl_reg[id])(p_io_msg->u.buf, p_io_msg->subtype);
    }
    else
    {
        PROTOCOL_PRINT_ERROR1("Received unregistered event id %d", id);
    }
#if LE_AUDIO_DEINIT
    ble_audio_pending_msg_del(p_io_msg->u.buf);
#endif
    ble_audio_mem_free(p_io_msg->u.buf);
#endif
}

uint16_t ble_audio_mgr_dispatch(uint16_t msg, void *buf)
{
    uint16_t cb_result = BLE_AUDIO_CB_RESULT_SUCCESS;
    uint16_t temp_cause;
    T_BLE_AUDIO_CBACK_ITEM *p_item;

#if LE_AUDIO_DEINIT
    if (ble_audio_deinit_flow != 0)
    {
        return cb_result;
    }
#endif
#if (LE_AUDIO_BASS_CLIENT_SUPPORT || LE_AUDIO_PACS_CLIENT_SUPPORT || LE_AUDIO_ASCS_CLIENT_SUPPORT||LE_AUDIO_CSIS_CLIENT_SUPPORT ||LE_AUDIO_VCS_CLIENT_SUPPORT ||LE_AUDIO_MICS_CLIENT_SUPPORT)
    if (ble_audio_db.p_cap_cb)
    {
        bool handled = false;
        cb_result = ble_audio_db.p_cap_cb((T_LE_AUDIO_MSG)msg, buf, &handled);
        if (handled)
        {
            return cb_result;
        }
    }
#endif
    p_item = (T_BLE_AUDIO_CBACK_ITEM *)ble_audio_db.cback_list.p_first;
    while (p_item != NULL)
    {
        temp_cause = p_item->cback((T_LE_AUDIO_MSG)msg, buf);
        if (temp_cause != BLE_AUDIO_CB_RESULT_SUCCESS &&
            temp_cause != BLE_AUDIO_CB_RESULT_PENDING)
        {
            PROTOCOL_PRINT_INFO2("ble_audio_mgr_dispatch: not success, msg 0x%x, cb_result 0x%x", msg,
                                 temp_cause);
            cb_result = temp_cause;
        }
        p_item = p_item->p_next;
    }
    return cb_result;
}

bool ble_audio_cback_register(P_FUN_BLE_AUDIO_CB cback)
{
    T_BLE_AUDIO_CBACK_ITEM *p_item;

    p_item = (T_BLE_AUDIO_CBACK_ITEM *)ble_audio_db.cback_list.p_first;
    while (p_item != NULL)
    {
        /* Sanity check if callback already registered */
        if (p_item->cback == cback)
        {
            return true;
        }

        p_item = p_item->p_next;
    }

    p_item = ble_audio_mem_zalloc(sizeof(T_BLE_AUDIO_CBACK_ITEM));
    if (p_item != NULL)
    {
        p_item->cback = cback;
        os_queue_in(&ble_audio_db.cback_list, p_item);
        return true;
    }

    return false;
}

bool ble_audio_cback_unregister(P_FUN_BLE_AUDIO_CB cback)
{
    T_BLE_AUDIO_CBACK_ITEM *p_item;
    bool              ret = false;

    p_item = (T_BLE_AUDIO_CBACK_ITEM *)ble_audio_db.cback_list.p_first;
    while (p_item != NULL)
    {
        if (p_item->cback == cback)
        {
            os_queue_delete(&ble_audio_db.cback_list, p_item);
            ble_audio_mem_free(p_item);
            ret = true;
            break;
        }
        p_item = p_item->p_next;
    }

    return ret;
}

#if (LE_AUDIO_BASS_CLIENT_SUPPORT || LE_AUDIO_PACS_CLIENT_SUPPORT || LE_AUDIO_ASCS_CLIENT_SUPPORT||LE_AUDIO_CSIS_CLIENT_SUPPORT ||LE_AUDIO_VCS_CLIENT_SUPPORT ||LE_AUDIO_MICS_CLIENT_SUPPORT)
void ble_audio_reg_cap_cb(P_FUN_BLE_AUDIO_INT_CB p_cap_cb)
{
    ble_audio_db.p_cap_cb = p_cap_cb;
}
#endif

void ble_audio_reg_bond_mgr(const T_BT_BOND_MGR *p_mgr)
{
    ble_audio_db.bond_mgr = p_mgr;
}

bool ble_audio_init(T_BLE_AUDIO_PARAMS *p_param)
{
    if (p_param == NULL)
    {
        goto failed;
    }

    if (ble_audio_db.acl_link_num != 0)
    {
        return true;
    }

    ble_audio_db.acl_link_num = p_param->acl_link_num;
#if LE_AUDIO_IO_MESSAGE_HANDLER
    ble_audio_db.evt_queue_handle = p_param->evt_queue_handle;
    ble_audio_db.io_queue_handle = p_param->io_queue_handle;
    ble_audio_db.io_event_type = p_param->io_event_type;
#endif
    if (ble_audio_db.acl_link_num)
    {
        ble_audio_db.le_link = ble_audio_mem_zalloc(sizeof(T_BLE_AUDIO_LINK) * ble_audio_db.acl_link_num);
        if (ble_audio_db.le_link == NULL)
        {
            ble_audio_db.acl_link_num = 0;
            goto failed;
        }
    }

    if (gap_get_gap_bond_manager())
    {
        ble_audio_reg_bond_mgr(&bt_bond_gap);
    }

    if (p_param->bt_gatt_client_init)
    {
        gatt_client_init(p_param->bt_gatt_client_init);
    }
#if LE_AUDIO_GROUP_SUPPORT
    ble_audio_group_init();
#endif
#if CONFIG_REALTEK_BLE_USE_UAL_API
    ual_le_register_app_cb(ble_audio_handle_gap_cb);
    ual_le_register_gap_msg_cb(ble_audio_handle_gap_msg);
#if LE_AUDIO_BROADCAST_SOURCE_ROLE
    ble_big_register_iso_cb_int(broadcast_source_isoc_broadcaster_cb);
#endif
    ble_audio_sync_init();
#endif
    return true;
failed:
    PROTOCOL_PRINT_ERROR0("ble_audio_init: failed");
    return false;
}

bool ble_audio_vc_mic_init(T_BLE_AUDIO_VC_MIC_PARAMS *p_param)
{
    uint8_t err_idx = 0;
    if (p_param == NULL)
    {
        err_idx = 1;
        goto failed;
    }
#if LE_AUDIO_INIT_DEBUG
    PROTOCOL_PRINT_INFO6("ble_audio_vc_mic_init: vocs_num %d, aics_vcs_num %d, aics_mics_num %d, aics_total_num %d, vcs_enable %d, mics_enable %d",
                         p_param->vocs_num,
                         p_param->aics_vcs_num,
                         p_param->aics_mics_num,
                         p_param->aics_total_num,
                         p_param->vcs_enable,
                         p_param->mics_enable);
#endif
    if ((p_param->vocs_num != 0 && p_param->p_vocs_feature_tbl == NULL) ||
        (p_param->aics_vcs_num != 0 && p_param->p_aics_vcs_tbl == NULL) ||
        (p_param->aics_mics_num != 0 && p_param->p_aics_mics_tbl == NULL) ||
        (p_param->aics_mics_num != 0 && p_param->mics_enable == false) ||
        (p_param->aics_mics_num > p_param->aics_total_num) ||
        (p_param->aics_vcs_num > p_param->aics_total_num))
    {
        err_idx = 2;
        goto failed;
    }
#if (LE_AUDIO_AICS_SUPPORT && LE_AUDIO_VCS_SUPPORT)
    for (uint8_t i = 0; i < p_param->aics_vcs_num; i++)
    {
        //Check aics id
        if (p_param->p_aics_vcs_tbl[i] >= p_param->aics_total_num)
        {
            err_idx = 3;
            goto failed;
        }
    }
#endif
#if (LE_AUDIO_AICS_SUPPORT && LE_AUDIO_MICS_SUPPORT)
    for (uint8_t i = 0; i < p_param->aics_mics_num; i++)
    {
        //Check aics id
        if (p_param->p_aics_mics_tbl[i] >= p_param->aics_total_num)
        {
            err_idx = 5;
            goto failed;
        }
    }
#endif
#if LE_AUDIO_VOCS_SUPPORT
    if (p_param->vocs_num != 0)
    {
        if (vocs_init(p_param->vocs_num, p_param->p_vocs_feature_tbl) == false)
        {
            err_idx = 7;
            goto failed;
        }
    }
#endif
#if LE_AUDIO_AICS_SUPPORT
    if (p_param->aics_total_num != 0)
    {
        if (aics_init(p_param->aics_total_num) == false)
        {
            err_idx = 8;
            goto failed;
        }
    }
#endif
#if LE_AUDIO_VCS_SUPPORT
    if (p_param->vcs_enable)
    {
        if (vcs_init(p_param->vocs_num, p_param->aics_vcs_num, p_param->p_aics_vcs_tbl) == false)
        {
            err_idx = 9;
            goto failed;
        }
    }
#endif
#if LE_AUDIO_MICS_SUPPORT
    if (p_param->mics_enable)
    {
        if (mics_init(p_param->aics_mics_num, p_param->p_aics_mics_tbl) == false)
        {
            err_idx = 10;
            goto failed;
        }
    }
#endif
    return true;
failed:
    PROTOCOL_PRINT_ERROR1("ble_audio_vc_mic_init: failed, err_idx %d", err_idx);
    return false;
}

#if LE_AUDIO_DEINIT
uint8_t ble_audio_deinit_flow = 0;

void ble_audio_deinit(void)
{
    PROTOCOL_PRINT_INFO0("ble_audio_deinit: +++");
    ble_audio_deinit_flow |= LE_AUDIO_DEINIT_FLAG;

#if (LE_AUDIO_BASS_CLIENT_SUPPORT || LE_AUDIO_PACS_CLIENT_SUPPORT || LE_AUDIO_ASCS_CLIENT_SUPPORT||LE_AUDIO_CSIS_CLIENT_SUPPORT ||LE_AUDIO_VCS_CLIENT_SUPPORT ||LE_AUDIO_MICS_CLIENT_SUPPORT)
    ble_audio_db.p_cap_cb = NULL;
#endif
    ble_audio_db.bond_mgr = NULL;

    T_BLE_AUDIO_CBACK_ITEM *p_item;
    while ((p_item = os_queue_out(&ble_audio_db.cback_list)) != NULL)
    {
        ble_audio_mem_free(p_item);
    }
#if LE_AUDIO_ASCS_CLIENT_SUPPORT
    ascs_client_deinit();
#endif
#if LE_AUDIO_BAP_SUPPORT
    bap_role_deinit();
#endif
#if LE_AUDIO_ASCS_SUPPORT
    ascs_deinit();
#endif
#if LE_AUDIO_BASE_DATA_GENERATE
    base_data_gen_deinit();
#endif
#if LE_AUDIO_BASE_DATA_PARSE
    base_data_parse_deinit();
#endif
#if LE_AUDIO_VOCS_SUPPORT
    vocs_deinit();
#endif
#if LE_AUDIO_VOCS_CLIENT_SUPPORT
    vocs_client_deinit();
#endif
#if LE_AUDIO_AICS_SUPPORT
    aics_deinit();
#endif
#if LE_AUDIO_AICS_CLIENT_SUPPORT
    aics_client_deinit();
#endif
#if LE_AUDIO_VCS_SUPPORT
    vcs_deinit();
#endif
#if LE_AUDIO_VCS_CLIENT_SUPPORT
    vcs_client_deinit();
#endif
#if LE_AUDIO_MICS_SUPPORT
    mics_deinit();
#endif
#if LE_AUDIO_BASS_CLIENT_SUPPORT
    bass_client_deinit();
#endif
#if LE_AUDIO_BASS_SUPPORT
    bass_deinit();
#endif
#if LE_AUDIO_PACS_CLIENT_SUPPORT
    pacs_client_deinit();
#endif
#if LE_AUDIO_PACS_SUPPORT
    pacs_deinit();
#endif
#if LE_AUDIO_CAS_SUPPORT
    cas_deinit();
#endif
#if LE_AUDIO_CAP_SUPPORT
    cap_deinit();
#endif
#if LE_AUDIO_CSIS_SUPPORT
    csis_deinit();
#endif
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
    csis_client_deinit();
    set_coordinator_deinit();
#endif
#if LE_AUDIO_HAS_SUPPORT
    has_deinit();
#endif
#if LE_AUDIO_HAS_CLIENT_SUPPORT
    has_client_deinit();
#endif
#if LE_AUDIO_TMAS_SUPPORT
    tmas_deinit();
#endif
#if LE_AUDIO_GMAS_SUPPORT
    gmas_deinit();
#endif

#if (LE_AUDIO_BROADCAST_SINK_ROLE | LE_AUDIO_BROADCAST_ASSISTANT_ROLE)
    ble_audio_sync_deinit();
#endif
#if LE_AUDIO_BROADCAST_SOURCE_ROLE
    broadcast_source_deinit();
#endif
#if LE_AUDIO_CCP_SERVER_SUPPORT
    ccp_server_deinit();
#endif
#if LE_AUDIO_CCP_CLIENT_SUPPORT
    ccp_client_deinit();
#endif
#if (LE_AUDIO_OTS_CLIENT_SUPPORT|LE_AUDIO_OTS_SERV_SUPPORT)
    le_coc_deinit();
#endif
#if LE_AUDIO_OTS_CLIENT_SUPPORT
    ots_client_deinit();
#endif
#if LE_AUDIO_OTS_SERV_SUPPORT
    ots_server_deinit();
#endif
#if LE_AUDIO_MCP_SERVER_SUPPORT
    mcs_server_deinit();
#endif
#if LE_AUDIO_MCP_CLIENT_SUPPORT
    mcs_client_deinit();
#endif
    ble_dm_deinit();
#if LE_AUDIO_GROUP_SUPPORT
    ble_audio_group_deinit();
#endif
#if LE_AUDIO_ASCS_CLIENT_SUPPORT
    audio_stream_session_deinit();
#endif
    ble_audio_link_deinit();
#if LE_AUDIO_IO_MESSAGE_HANDLER
    ble_audio_pending_msg_free();
#endif
    memset(&ble_audio_db, 0, sizeof(ble_audio_db));
#if LE_AUDIO_MEM_DUMP
    ble_audio_mem_deinit();
#endif
    ble_audio_deinit_flow = 0;
    PROTOCOL_PRINT_INFO0("ble_audio_deinit: ---");
}
#endif

