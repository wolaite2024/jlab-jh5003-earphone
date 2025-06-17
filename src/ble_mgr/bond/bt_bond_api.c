#if CONFIG_REALTEK_APP_BOND_MGR_SUPPORT
#include <string.h>
#include "bt_bond_api.h"
#include "bt_bond_le_int.h"
#include "bt_bond_common_int.h"
#include "gap_bond_manager.h"
#include "trace.h"
#include "gap_conn_le.h"
#include "stdlib.h"

extern bool gap_set_cccd_data_pending(uint16_t conn_handle, uint16_t handle, bool data_pending);
extern bool le_get_conn_local_addr(uint16_t conn_handle, uint8_t *bd_addr, uint8_t *bd_type);
extern P_FUN_BT_BOND_CB bt_bond_get_cback;
void bt_bond_handle_authen_result_ind(T_LE_AUTHEN_RESULT_IND *auth_ind)
{
    BTM_PRINT_INFO5("[BTBond] bt_bond_handle_authen_result_ind: bd_addr %s, remote_addr_type %d, key_len %d, key_type 0x%x, cause 0x%x",
                    TRACE_BDADDR(auth_ind->bd_addr),
                    auth_ind->remote_addr_type,
                    auth_ind->key_len,
                    auth_ind->key_type,
                    auth_ind->cause);
    T_LE_BOND_ENTRY *p_entry = NULL;

    if (auth_ind->key_type == GAP_KEY_LE_LOCAL_LTK ||
        auth_ind->key_type == GAP_KEY_LE_REMOTE_LTK ||
        auth_ind->key_type == GAP_KEY_LE_LOCAL_IRK ||
        auth_ind->key_type == GAP_KEY_LE_REMOTE_IRK)
    {
        T_GAP_CFM_CAUSE cause = GAP_CFM_CAUSE_REJECT;
        T_GAP_REMOTE_ADDR_TYPE remote_addr_type = auth_ind->remote_addr_type;
        uint8_t local_bd[GAP_BD_ADDR_LEN];
        uint8_t local_bd_type = LE_BOND_LOCAL_ADDRESS_ANY;

        /* key generate by BR/EDR secure connections */
        if (auth_ind->remote_addr_type == GAP_REMOTE_ADDR_CLASSIC)
        {
            remote_addr_type = GAP_REMOTE_ADDR_LE_PUBLIC;
            gap_get_param(GAP_PARAM_BD_ADDR, local_bd);
            local_bd_type = GAP_LOCAL_ADDR_LE_PUBLIC;
        }
        else
        {
            uint16_t conn_handle = 0;
            gap_chann_get_handle(auth_ind->bd_addr, auth_ind->remote_addr_type, &conn_handle);
            le_get_conn_local_addr(conn_handle, local_bd, &local_bd_type);
        }

        p_entry = bt_le_find_key_entry(auth_ind->bd_addr, (T_GAP_REMOTE_ADDR_TYPE)remote_addr_type,
                                       local_bd, local_bd_type);
        if (p_entry == NULL)
        {
            p_entry = bt_le_allocate_key_entry(auth_ind->bd_addr, (T_GAP_REMOTE_ADDR_TYPE)remote_addr_type,
                                               local_bd, local_bd_type);
        }
        else
        {
            if (auth_ind->key_type == GAP_KEY_LE_LOCAL_LTK &&
                (p_entry->flags & LE_KEY_STORE_LOCAL_LTK_BIT))
            {
                p_entry->modify_flags |= LE_BOND_MODIFY_REPAIR;
                bt_le_set_bond_state(p_entry, BT_BOND_INFO_REPAIR_FLAG);
            }
        }
        if (p_entry == NULL)
        {
            BTM_PRINT_ERROR0("[BTBond] le_bond_handle_auth_result_ind: no resource");

            if (bt_bond_proc_flag & BT_BOND_PROC_BIT_SEND_BOND_FULL)
            {
                T_BT_LE_BOND_CB_DATA cb_data;
                T_BT_LE_BOND_FULL bond_full = {0};

                bond_full.new_bond = true;
                memcpy(bond_full.bd_addr, auth_ind->bd_addr, 6);
                bond_full.bd_type = auth_ind->remote_addr_type;
                memcpy(bond_full.local_bd_addr, local_bd, 6);
                bond_full.local_bd_type = local_bd_type;
                cb_data.p_le_bond_full = &bond_full;
                bt_bond_msg_post(BT_BOND_MSG_LE_BOND_FULL, &cb_data);
            }
            else
            {
                p_entry = bt_le_get_low_priority_bond();

                if (p_entry != NULL)
                {
                    bt_le_delete_bond(p_entry);
                }
            }

            p_entry = bt_le_allocate_key_entry(auth_ind->bd_addr, (T_GAP_REMOTE_ADDR_TYPE)remote_addr_type,
                                               local_bd, local_bd_type);
        }
        if (p_entry != NULL)
        {
            if (bt_le_save_key(p_entry, (T_GAP_KEY_TYPE)auth_ind->key_type, auth_ind->key_len,
                               auth_ind->link_key))
            {
                cause = GAP_CFM_CAUSE_ACCEPT;
            }
        }
        else
        {
            BTM_PRINT_ERROR0("[BTBond] le_bond_handle_auth_result_ind: allocate failed");
        }

        le_bond_authen_result_confirm(auth_ind->bd_addr, auth_ind->remote_addr_type,
                                      auth_ind->key_type, cause);
    }
    else if (auth_ind->key_type == GAP_KEY_LE_LOCAL_CSRK ||
             auth_ind->key_type == GAP_KEY_LE_REMOTE_CSRK)
    {
        le_bond_authen_result_confirm(auth_ind->bd_addr, auth_ind->remote_addr_type,
                                      auth_ind->key_type, GAP_CFM_CAUSE_ACCEPT);
    }
    return;
}
void bt_bond_handle_authen_key_req_ind(T_LE_AUTHEN_KEY_REQ_IND *key_req_ind)
{
    BTM_PRINT_INFO3("[BTBond] bt_bond_handle_authen_key_req_ind: bd_addr %s, remote_addr_type %d, key_type %d",
                    TRACE_BDADDR(key_req_ind->bd_addr),
                    key_req_ind->remote_addr_type,
                    key_req_ind->key_type);
    if (key_req_ind->key_type == GAP_KEY_LE_LOCAL_CSRK ||
        key_req_ind->key_type == GAP_KEY_LE_REMOTE_CSRK)
    {
        le_bond_authen_key_req_confirm(key_req_ind->bd_addr,
                                       key_req_ind->remote_addr_type,
                                       0,
                                       NULL,
                                       key_req_ind->key_type,
                                       GAP_CFM_CAUSE_REJECT
                                      );
    }
    else
    {
        uint8_t key_length = 0;
        uint8_t link_key[28];
        T_GAP_CFM_CAUSE cause = GAP_CFM_CAUSE_REJECT;
        T_LE_BOND_ENTRY *p_entry;
        uint16_t conn_handle = 0;
        uint8_t local_bd[GAP_BD_ADDR_LEN];
        uint8_t local_bd_type = LE_BOND_LOCAL_ADDRESS_ANY;

        gap_chann_get_handle(key_req_ind->bd_addr, key_req_ind->remote_addr_type, &conn_handle);
        le_get_conn_local_addr(conn_handle, local_bd, &local_bd_type);

        p_entry = bt_le_find_key_entry(key_req_ind->bd_addr,
                                       (T_GAP_REMOTE_ADDR_TYPE)key_req_ind->remote_addr_type, local_bd, local_bd_type);
        if (p_entry == NULL)
        {
            p_entry = bt_le_find_entry_by_aes(key_req_ind->bd_addr, key_req_ind->remote_addr_type, local_bd,
                                              local_bd_type);
        }
        if (p_entry != NULL)
        {
            key_length = bt_le_get_key(p_entry, (T_GAP_KEY_TYPE)key_req_ind->key_type, link_key);
            if (key_length != 0)
            {
                cause = GAP_CFM_CAUSE_ACCEPT;
            }
        }
        else
        {
            if ((key_req_ind->key_type == GAP_KEY_LE_LOCAL_LTK) &&
                bt_bond_get_cback != NULL)
            {
                T_BT_LE_BOND_CB_DATA cb_data;
                T_BT_LE_BOND_GET bond_get = {0};

                memcpy(bond_get.bd_addr, key_req_ind->bd_addr, 6);
                bond_get.bd_type = key_req_ind->remote_addr_type;
                memcpy(bond_get.local_bd_addr, local_bd, 6);
                bond_get.local_bd_type = local_bd_type;
                bond_get.key_type = key_req_ind->key_type;
                bond_get.cfm_cause = GAP_CFM_CAUSE_REJECT;
                cb_data.p_le_bond_get = &bond_get;
                bt_bond_get_cback(BT_BOND_MSG_LE_BOND_GET, &cb_data);
                if (bond_get.cfm_cause == GAP_CFM_CAUSE_ACCEPT)
                {
                    BTM_PRINT_INFO0("[BTBond] bt_bond_handle_authen_key_req_ind:BT_BOND_MSG_LE_BOND_GET accept");
                    key_length = bond_get.key_len;
                    memcpy(link_key, bond_get.key_data, 28);
                    cause = GAP_CFM_CAUSE_ACCEPT;
                }
            }
        }
        le_bond_authen_key_req_confirm(key_req_ind->bd_addr,
                                       key_req_ind->remote_addr_type,
                                       key_length,
                                       link_key,
                                       key_req_ind->key_type,
                                       cause
                                      );

        return;
    }
}

void bt_bond_handle_gatt_server_store_ind(T_GATT_SERVER_STORE_IND *store_ind)
{
    T_DEV_TYPE type;
    uint8_t idx = 0xff;

    if ((store_ind->op == GATT_STORE_OP_GET_CCC_BITS)
        || (store_ind->op == GATT_STORE_OP_SET_CCC_BITS))
    {
        uint16_t conn_handle = 0;
        uint8_t local_bd[GAP_BD_ADDR_LEN];
        uint8_t local_bd_type = LE_BOND_LOCAL_ADDRESS_ANY;
        if (gap_chann_get_handle(store_ind->bd_addr, store_ind->remote_addr_type, &conn_handle))
        {
            le_get_conn_local_addr(conn_handle, local_bd, &local_bd_type);
        }
        if (bt_bond_get_info_from_addr(store_ind->bd_addr, store_ind->remote_addr_type, local_bd,
                                       local_bd_type, &type, &idx) == false)
        {
            goto failed;
        }
    }

    switch (store_ind->op)
    {
    case GATT_STORE_OP_GET_CCC_BITS: /*----------------------------------*/
        {
            T_BT_CCCD_DATA *p_cccd_data = NULL;
            p_cccd_data = calloc(1,
                                 4 + p_bt_bond_cb->otp_bond_cfg.gatt_storage_ccc_bits_count * 4);
            if (p_cccd_data == NULL)
            {
                goto failed;
            }
            if (bt_bond_get_cccd_data(type, idx, p_cccd_data))
            {
                uint8_t conn_id = 0;
                uint16_t  conn_handle;
                le_get_conn_id(store_ind->bd_addr, store_ind->remote_addr_type, &conn_id);
                le_get_conn_param(GAP_PARAM_CONN_HANDLE, &conn_handle, conn_id);
                gap_update_cccd_info(conn_handle, p_cccd_data->data_length, p_cccd_data->data);
                free(p_cccd_data);
                return;
            }
            else
            {
                free(p_cccd_data);
                goto failed;
            }
        }

    case GATT_STORE_OP_SET_CCC_BITS: /*----------------------------------*/
        {
            if (bt_bond_set_cccd_data(type, idx, store_ind->data_len, store_ind->p_data, true) == false)
            {
                goto failed;
            }
        }
        break;

    default: /*-------------------------------------------------------------*/
        break;
    }
    return;
failed:
    BTM_PRINT_ERROR0("[BTBond] bt_bond_handle_gatt_server_store_ind: failed");
    return;
}

void bt_bond_handle_acl_status_info(T_LE_ACL_STATUS_INFO *acl_info)
{
    T_LE_BOND_ENTRY *p_entry = NULL;
    uint8_t conn_id = 0;
    uint16_t conn_handle = 0;
    uint8_t local_bd[GAP_BD_ADDR_LEN];
    uint8_t local_bd_type = LE_BOND_LOCAL_ADDRESS_ANY;
    if (le_get_conn_id(acl_info->bd_addr, acl_info->remote_addr_type, &conn_id) == false)
    {
        return;
    }
    le_get_conn_param(GAP_PARAM_CONN_HANDLE, &conn_handle, conn_id);
    le_get_conn_local_addr(conn_handle, local_bd, &local_bd_type);
    p_entry = bt_le_find_key_entry(acl_info->bd_addr,
                                   (T_GAP_REMOTE_ADDR_TYPE)acl_info->remote_addr_type, local_bd, local_bd_type);
    switch (acl_info->status)
    {
    case LE_ACL_AUTHEN_FAIL:
        {
            if (acl_info->p.authen.cause == (HCI_ERR | HCI_ERR_KEY_MISSING))
            {
                if (p_entry != NULL)
                {
                    bt_le_delete_bond(p_entry);
                }
            }
        }
        break;

    case LE_ACL_AUTHEN_SUCCESS:
        {
            if (p_entry != NULL)
            {
                bt_bond_save_info_before_add_bond(p_entry->idx, conn_handle);
                if (p_entry->modify_flags & LE_BOND_MODIFY_NEW)
                {
                    p_entry->modify_flags = 0;
                    T_BT_LE_BOND_CB_DATA cb_data;
                    T_BT_LE_BOND_ADD bond_add;
                    bond_add.modify_flags = 0;
                    bond_add.p_entry = p_entry;
                    cb_data.p_le_bond_add = &bond_add;
                    bt_bond_msg_post(BT_BOND_MSG_LE_BOND_ADD, &cb_data);
                }
                else if (p_entry->modify_flags & LE_BOND_MODIFY_REPAIR)
                {
                    bt_le_key_send_update_info(p_entry);
                }
            }
        }
        break;

    case LE_ACL_CONN_ENCRYPTED:
        {
            if (p_entry)
            {
                bt_le_set_high_priority_bond(p_entry);
            }
        }
        break;

    case LE_ACL_ADDR_RESOLVED:
        {
            BTM_PRINT_TRACE2("[BTBond] bt_bond_handle_acl_status_info: LE_ACL_ADDR_RESOLVED, bd_addr %s, type %d",
                             TRACE_BDADDR(acl_info->p.resolve.bd_addr),
                             acl_info->p.resolve.remote_addr_type);
            uint16_t null_bd[6] = {0, 0, 0, 0, 0, 0};
            if (memcmp(acl_info->p.resolve.bd_addr, null_bd, 6) == 0)
            {
                break;
            }
            if (memcmp(acl_info->bd_addr, acl_info->p.resolve.bd_addr, 6))
            {
                T_LE_BOND_ENTRY *p_old_entry;
                p_old_entry = bt_le_find_key_entry(acl_info->p.resolve.bd_addr,
                                                   (T_GAP_REMOTE_ADDR_TYPE)(acl_info->p.resolve.remote_addr_type + 2),
                                                   local_bd, local_bd_type);
                if ((p_entry != NULL) && (p_old_entry != NULL) && (p_entry->idx != p_old_entry->idx))
                {
                    bt_le_delete_bond(p_old_entry);
                }
            }

        }
        break;

    default:
        break;
    }
    return;
}

bool bt_bond_handle_cccd_pending(uint8_t *bd_addr, uint8_t bd_type, uint16_t conn_handle,
                                 uint8_t *local_bd_addr, uint8_t local_bd_type,
                                 uint16_t cccd_handle, bool data_pending)
{
    T_DEV_TYPE dev_type;
    uint8_t idx;
    bool ret = false;
    if (bt_bond_get_info_from_addr(bd_addr, bd_type,
                                   local_bd_addr, local_bd_type, &dev_type, &idx))
    {
        if (bt_bond_set_cccd_pending(dev_type, idx, cccd_handle, data_pending))
        {
            if (conn_handle != 0)
            {
                ret = gap_set_cccd_data_pending(conn_handle, cccd_handle, data_pending);
            }
        }
    }
    BTM_PRINT_INFO3("[BTBond] bt_bond_handle_cccd_pending: ret %d, idx %d, cccd_handle 0x%x",
                    ret, idx, cccd_handle);
    return ret;
}

T_APP_RESULT bt_bond_mgr_handle_gap_msg(void *p_data)
{
    T_APP_RESULT ret = APP_RESULT_SUCCESS;
    T_APP_BOND_MGR_CB *p_info = (T_APP_BOND_MGR_CB *)p_data;

    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return ret;
    }

    BTM_PRINT_INFO1("[BTBond] bt_bond_mgr_handle_gap_msg: opcode 0x%x", p_info->opcode);
    switch (p_info->opcode)
    {
#if 0
    case APP_BOND_MGR_SET_CCCD_DATA_PENDING_INFO:
        BTM_PRINT_INFO4("[BTBond] APP_BOND_MGR_SET_CCCD_DATA_PENDING_INFO: bd_addr %s, remote_addr_type 0x%x, cccd handle 0x%x, data_pending 0x%x",
                        TRACE_BDADDR(p_info->data.p_set_cccd_data_pending_info->bd_addr),
                        p_info->data.p_set_cccd_data_pending_info->remote_addr_type,
                        p_info->data.p_set_cccd_data_pending_info->handle,
                        p_info->data.p_set_cccd_data_pending_info->data_pending);
        if (bt_bond_handle_cccd_pending(p_info->data.p_set_cccd_data_pending_info) == false)
        {
            ret = APP_RESULT_REJECT;
        }
        break;
#endif
    case APP_BOND_MGR_GATT_SERVER_STORE_IND:
        BTM_PRINT_INFO7("[BTBond] APP_BOND_MGR_GATT_SERVER_STORE_IND: op %d, cccd_handle 0x%x, cccd_bits 0x%x, bd_addr %s, remote_addr_type 0x%x, data_len %d, p_data %b",
                        p_info->data.p_gatt_server_store_ind->op,
                        p_info->data.p_gatt_server_store_ind->cccd_handle,
                        p_info->data.p_gatt_server_store_ind->cccd_bits,
                        TRACE_BDADDR(p_info->data.p_gatt_server_store_ind->bd_addr),
                        p_info->data.p_gatt_server_store_ind->remote_addr_type,
                        p_info->data.p_gatt_server_store_ind->data_len,
                        TRACE_BINARY(p_info->data.p_gatt_server_store_ind->data_len,
                                     p_info->data.p_gatt_server_store_ind->p_data));
        bt_bond_handle_gatt_server_store_ind(p_info->data.p_gatt_server_store_ind);
        break;

    case APP_BOND_MGR_LE_ACL_STATUS_INFO:
        BTM_PRINT_INFO3("[BTBond] APP_BOND_MGR_LE_ACL_STATUS_INFO: bd_addr %s, remote_addr_type 0x%x, status 0x%x",
                        TRACE_BDADDR(p_info->data.p_le_acl_status_info->bd_addr),
                        p_info->data.p_le_acl_status_info->remote_addr_type,
                        p_info->data.p_le_acl_status_info->status);
        if ((p_info->data.p_le_acl_status_info->status == LE_ACL_AUTHEN_START) ||
            (p_info->data.p_le_acl_status_info->status == LE_ACL_AUTHEN_SUCCESS) ||
            (p_info->data.p_le_acl_status_info->status == LE_ACL_AUTHEN_FAIL) ||
            (p_info->data.p_le_acl_status_info->status == LE_ACL_CONN_ENCRYPTED) ||
            (p_info->data.p_le_acl_status_info->status == LE_ACL_CONN_NOT_ENCRYPTED))
        {
            BTM_PRINT_INFO7("[BTBond] LE_ACL_: statue 0x%x, bd_addr %s, remote_addr_type 0x%x, authen.key_type 0x%x, authen.key_size 0x%x, authen.encrypt_type 0x%x, authen.cause 0x%x",
                            p_info->data.p_le_acl_status_info->status,
                            TRACE_BDADDR(p_info->data.p_le_acl_status_info->bd_addr),
                            p_info->data.p_le_acl_status_info->remote_addr_type,
                            p_info->data.p_le_acl_status_info->p.authen.key_type,
                            p_info->data.p_le_acl_status_info->p.authen.key_size,
                            p_info->data.p_le_acl_status_info->p.authen.encrypt_type,
                            p_info->data.p_le_acl_status_info->p.authen.cause);
        }
        if (p_info->data.p_le_acl_status_info->status == LE_ACL_ADDR_RESOLVED)
        {
            BTM_PRINT_INFO5("[BTBond] LE_ACL_ADDR_RESOLVED: statue 0x%x, bd_addr %s, remote_addr_type 0x%x, resolve.bd_addr %s, resolve.remote_addr_type 0x%x",
                            p_info->data.p_le_acl_status_info->status,
                            TRACE_BDADDR(p_info->data.p_le_acl_status_info->bd_addr),
                            p_info->data.p_le_acl_status_info->remote_addr_type,
                            TRACE_BDADDR(p_info->data.p_le_acl_status_info->p.resolve.bd_addr),
                            p_info->data.p_le_acl_status_info->p.resolve.remote_addr_type);
        }
        bt_bond_handle_acl_status_info(p_info->data.p_le_acl_status_info);
        break;

    case APP_BOND_MGR_LE_AUTHEN_RESULT_IND:
        bt_bond_handle_authen_result_ind(p_info->data.p_le_authen_result_ind);
        break;

    case APP_BOND_MGR_LE_AUTHEN_KEY_REQ_IND:
        bt_bond_handle_authen_key_req_ind(p_info->data.p_le_authen_key_req_ind);
        break;

    default:
        break;
    }
    return ret;
}
#endif
