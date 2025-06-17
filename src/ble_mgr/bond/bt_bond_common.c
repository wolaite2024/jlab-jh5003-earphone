#if CONFIG_REALTEK_APP_BOND_MGR_SUPPORT
#include <string.h>
#include "stdlib.h"
#include "os_queue.h"
#include "gatt.h"
#include "bt_bond_ftl_int.h"
#include "bt_bond_le_int.h"
#include "bt_bond_legacy.h"
#include "trace.h"
#include "bt_bond_common_int.h"
#include "ble_mgr_int.h"
#if F_BT_LE_5_1_CLIENT_SUPPORTED_FEATURES_SERVER_SUPPORT
#include "gap_acl_int.h"
#endif

typedef struct t_bt_bond_cback_item
{
    struct t_bt_bond_cback_item *p_next;
    P_FUN_BT_BOND_CB              cback;
} T_BT_BOND_CBACK_ITEM;

T_BT_BOND_CB *p_bt_bond_cb = NULL;
P_FUN_BT_BOND_CB bt_bond_get_cback = NULL;
uint8_t  bt_bond_proc_flag = 0;

extern void gap_get_otp_bond_cfg(T_OTP_BOND_CFG *p_cfg);
extern void gap_set_otp_gap_bond_mgr(bool gap_bond_mgr);
extern void bt_legacy_key_init(void);

void bt_bond_init(void)
{
    if (p_bt_bond_cb == NULL)
    {
        p_bt_bond_cb = calloc(1, sizeof(T_BT_BOND_CB));
        if (p_bt_bond_cb == NULL)
        {
            return;
        }
    }
    os_queue_init(&p_bt_bond_cb->bt_bond_cback_list);
    gap_set_otp_gap_bond_mgr(false);
    gap_get_otp_bond_cfg(&p_bt_bond_cb->otp_bond_cfg);
    bt_le_key_init();
    bt_legacy_key_init();
}

void bt_bond_cfg_send_bond_full(bool enable)
{
    if (enable)
    {
        bt_bond_proc_flag |= BT_BOND_PROC_BIT_SEND_BOND_FULL;
    }
    else
    {
        bt_bond_proc_flag &= ~BT_BOND_PROC_BIT_SEND_BOND_FULL;
    }
}

bool bt_bond_register_bond_get_cb(P_FUN_BT_BOND_CB app_callback)
{
    bt_bond_get_cback = app_callback;
    return true;
}

bool bt_bond_register_app_cb(P_FUN_BT_BOND_CB cback)
{
    T_BT_BOND_CBACK_ITEM *p_item;
    if (p_bt_bond_cb == NULL)
    {
        return false;
    }

    p_item = (T_BT_BOND_CBACK_ITEM *)p_bt_bond_cb->bt_bond_cback_list.p_first;
    while (p_item != NULL)
    {
        /* Sanity check if callback already registered */
        if (p_item->cback == cback)
        {
            return true;
        }

        p_item = p_item->p_next;
    }

    p_item = calloc(1, sizeof(T_BT_BOND_CBACK_ITEM));
    if (p_item != NULL)
    {
        p_item->cback = cback;
        os_queue_in(&p_bt_bond_cb->bt_bond_cback_list, p_item);
        return true;
    }

    return false;
}

bool bt_bond_unregister_app_cb(P_FUN_BT_BOND_CB app_callback)
{
    T_BT_BOND_CBACK_ITEM *p_item;
    bool              ret = false;

    p_item = (T_BT_BOND_CBACK_ITEM *)p_bt_bond_cb->bt_bond_cback_list.p_first;
    while (p_item != NULL)
    {
        if (p_item->cback == app_callback)
        {
            os_queue_delete(&p_bt_bond_cb->bt_bond_cback_list, p_item);
            free(p_item);
            ret = true;
            break;
        }
        p_item = p_item->p_next;
    }

    return ret;
}

bool bt_bond_msg_post(uint8_t cb_type, void *p_cb_data)
{
    T_BT_BOND_CBACK_ITEM *p_item;
    if (p_bt_bond_cb == NULL)
    {
        return false;
    }
#if F_APP_BOND_MGR_DEBUG
    BTM_PRINT_INFO1("bt_bond_msg_post: cb_type 0x%04x", cb_type);
#endif
    p_item = (T_BT_BOND_CBACK_ITEM *)p_bt_bond_cb->bt_bond_cback_list.p_first;
    while (p_item != NULL)
    {
        p_item->cback(cb_type, p_cb_data);
        p_item = p_item->p_next;
    }

    return true;
}

bool bt_bond_get_info_from_addr(uint8_t *bd_addr, uint8_t bd_type,
                                uint8_t *local_bd_addr, uint8_t local_bd_type,
                                T_DEV_TYPE *p_type, uint8_t *p_idx)
{
#if F_BT_BREDR_SUPPORT
    if (bd_type == GAP_REMOTE_ADDR_CLASSIC)
    {
        if (bt_legacy_get_paired_idx(bd_addr, p_idx))
        {
            *p_type = LEGACY_BT_DEV;
            return true;
        }
    }
    else
#endif
    {
        T_LE_BOND_ENTRY *p_entry = bt_le_find_key_entry(bd_addr, (T_GAP_REMOTE_ADDR_TYPE)bd_type,
                                                        local_bd_addr, local_bd_type);
        if (p_entry == NULL)
        {
            p_entry = bt_le_find_entry_by_aes(bd_addr, bd_type, local_bd_addr, local_bd_type);
        }
        if (p_entry)
        {
            *p_type = LE_BT_DEV;
            *p_idx = p_entry->idx;
            return true;
        }
    }
    BTM_PRINT_ERROR2("[BTBond] bt_bond_get_info_from_addr: failed, bd_addr %s, bd_type %d",
                     TRACE_BDADDR(bd_addr), bd_type);
    return false;
}

bool bt_bond_remove(T_DEV_TYPE type, uint8_t idx)
{
    BTM_PRINT_INFO2("[BTBond] bt_bond_remove: type %d, idx %d", type, idx);
#if F_BT_LE_5_1_CLIENT_SUPPORTED_FEATURES_SERVER_SUPPORT
    T_REMOTE_CLIENT_SUPPORTED_FEATURES remote_client_supp_feats;
    remote_client_supp_feats.length = 0;
    ftl_save_remote_client_supported_features(LE_BT_DEV, &remote_client_supp_feats, idx, 4);
#endif
    T_BT_CCCD_DATA cccd_data;
    cccd_data.data_length = 0;
    ftl_save_cccd(type, &cccd_data, idx, 4);
    return true;
}

bool bt_bond_get_cccd_data(T_DEV_TYPE type, uint8_t idx, T_BT_CCCD_DATA *p_data)
{
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return false;
    }

    if (ftl_load_cccd(type, p_data, idx) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void bt_bond_cccd_modify(T_DEV_TYPE type, uint8_t idx)
{
    if (type == LE_BT_DEV)
    {
        T_LE_BOND_ENTRY *p_entry = bt_le_find_key_entry_by_idx(idx);
        if (p_entry)
        {
            bt_le_set_bond_state(p_entry, BT_BOND_INFO_NEED_UPDATE_FLAG);
            if ((p_entry->modify_flags & LE_BOND_MODIFY_NEW) == 0)
            {
                p_entry->modify_flags |= LE_BOND_MODIFY_CCCD;
                bt_le_key_send_update_info(p_entry);
            }
        }
    }
}

bool bt_bond_set_cccd_data(T_DEV_TYPE type, uint8_t idx, uint8_t data_len, uint8_t *p_data,
                           bool is_info)
{
    bool result = false;
    T_BT_CCCD_DATA *p_cccd_data = NULL;
    bool cccd_modify = false;

    p_cccd_data = calloc(1,
                         4 + p_bt_bond_cb->otp_bond_cfg.gatt_storage_ccc_bits_count * 4);
    if (p_cccd_data == NULL)
    {
        goto ret;
    }
    if (bt_bond_get_cccd_data(type, idx, p_cccd_data))
    {
        if ((p_cccd_data->data_length != 0) && (p_cccd_data->data_length <= data_len))
        {
            if (memcmp(p_cccd_data->data, p_data, p_cccd_data->data_length) == 0)
            {
                if (p_cccd_data->data_length < data_len)
                {
                    T_BT_CCCD_DATA cccd_data;
                    uint8_t add_size = data_len - p_cccd_data->data_length;
                    cccd_data.data_length = p_cccd_data->data_length + add_size;
                    if (ftl_add_cccd(type, p_data + p_cccd_data->data_length, idx, add_size,
                                     4 + p_cccd_data->data_length))
                    {
                        goto ret;
                    }
                    if (ftl_save_cccd(type, &cccd_data, idx, 4))
                    {
                        goto ret;
                    }
                    result = true;
                    cccd_modify = true;
                    goto ret;
                }
                else if (p_cccd_data->data_length == data_len)
                {
                    result = true;
                    goto ret;
                }
            }
        }
    }

    p_cccd_data->data_length = data_len;
    memcpy(p_cccd_data->data, p_data, data_len);
    if (ftl_save_cccd(type, p_cccd_data, idx,
                      (4 + p_cccd_data->data_length)) == 0)
    {
        cccd_modify = true;
        result = true;
    }
ret:
    if (p_cccd_data)
    {
        free(p_cccd_data);
    }
    if (cccd_modify && is_info)
    {
        bt_bond_cccd_modify(type, idx);
    }
    return result;
}

bool bt_bond_set_cccd_pending(T_DEV_TYPE type, uint8_t idx, uint16_t handle, bool data_pending)
{
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return false;
    }
    T_BT_CCCD_DATA *p_cccd_data;
    bool ret = false;
    uint16_t cccd_bits = 0;
    bool cccd_modify = false;
    p_cccd_data = calloc(1,
                         4 + p_bt_bond_cb->otp_bond_cfg.gatt_storage_ccc_bits_count * 4);
    if (p_cccd_data == NULL)
    {
        return false;
    }

    if (bt_bond_get_cccd_data(type, idx, p_cccd_data))
    {
        T_BT_CCCD_ELEM *p_elem;
        uint8_t elem_cnt = p_cccd_data->data_length / sizeof(T_BT_CCCD_ELEM);

        p_elem = (T_BT_CCCD_ELEM *)p_cccd_data->data;
        for (uint8_t i = 0; i < elem_cnt; i++)
        {
            if (p_elem[i].handle == handle)
            {
                cccd_bits = p_elem[i].ccc_bits;
                if (data_pending)
                {
                    if (cccd_bits != 0)
                    {
                        cccd_bits |= GATT_CLIENT_CHAR_NOTIF_IND_DATA_PENGDING;
                    }
                }
                else
                {
                    cccd_bits &= ~GATT_CLIENT_CHAR_NOTIF_IND_DATA_PENGDING;
                }
                if (p_elem[i].ccc_bits != cccd_bits)
                {
                    uint8_t offset = 4 + 4 * i;
                    p_elem[i].ccc_bits = cccd_bits;
                    if (ftl_add_cccd(type, (uint8_t *)&p_elem[i], idx, 4, offset) == 0)
                    {
                        cccd_modify = true;
                        ret = true;
                    }
                }
                break;
            }
        }
    }
    BTM_PRINT_INFO4("[BTBond] gap_storage_set_cccd_pending: ret %d, idx %d, handle 0x%x, cccd_bits 0x%x",
                    ret, idx, handle, cccd_bits);
    free(p_cccd_data);
    if (cccd_modify)
    {
        bt_bond_cccd_modify(type, idx);
    }
    return ret;
}

#if F_BT_LE_5_1_CLIENT_SUPPORTED_FEATURES_SERVER_SUPPORT
bool bt_bond_get_remote_client_supported_features(T_DEV_TYPE type, uint8_t idx,
                                                  T_REMOTE_CLIENT_SUPPORTED_FEATURES *p_data)
{
    T_REMOTE_CLIENT_SUPPORTED_FEATURES remote_client_supp_feats;
    if (ftl_load_remote_client_supported_features(type, &remote_client_supp_feats, idx) == 0)
    {
        if (remote_client_supp_feats.length <= GATTS_CLIENT_SUPPORTED_FEATURES_LEN)
        {
            if (remote_client_supp_feats.length > 0)
            {
                memcpy(p_data->client_supported_features,
                       remote_client_supp_feats.client_supported_features,
                       remote_client_supp_feats.length);
            }
            p_data->length = remote_client_supp_feats.length;
            return true;
        }
    }
    return false;
}

bool bt_bond_save_remote_client_supported_features(T_DEV_TYPE type, uint8_t idx,
                                                   T_REMOTE_CLIENT_SUPPORTED_FEATURES *p_data)
{

//    if ((p_bt_bond_cb->otp_bond_cfg.use_gatts_client_supported_features) == 0)
//    {
//        return true;
//    }
//    else
    {
        if ((idx != 0xff) && (p_data) && (p_data->length <= GATTS_CLIENT_SUPPORTED_FEATURES_LEN))
        {
            T_REMOTE_CLIENT_SUPPORTED_FEATURES remote_client_supp_feats;
            remote_client_supp_feats.length = p_data->length;
            memcpy(remote_client_supp_feats.client_supported_features,
                   p_data->client_supported_features, p_data->length);
            if (ftl_save_remote_client_supported_features(type, &remote_client_supp_feats, idx,
                                                          sizeof(T_REMOTE_CLIENT_SUPPORTED_FEATURES)) == 0)
            {
                return true;
            }
        }
    }
    return false;
}
#endif

bool bt_bond_save_info_before_add_bond(uint8_t idx, uint16_t conn_handle)
{
    if (p_bt_bond_cb == NULL || p_bt_bond_cb->otp_bond_cfg.gap_bond_manager)
    {
        return false;
    }

    bool ret = true;

#if F_BT_LE_5_1_CLIENT_SUPPORTED_FEATURES_SERVER_SUPPORT
    if (p_bt_bond_cb->otp_bond_cfg.use_gatts_client_supported_features)
    {
        T_REMOTE_CLIENT_SUPPORTED_FEATURES remote_client_supp_feats;

        if (gap_acl_get_remote_client_supported_features(conn_handle,
                                                         &(remote_client_supp_feats.length),
                                                         remote_client_supp_feats.client_supported_features, NULL, true) == GAP_CAUSE_SUCCESS)
        {
            if (!(bt_bond_save_remote_client_supported_features(LE_BT_DEV, idx, &remote_client_supp_feats)))
            {
                ret = false;
            }
        }
        else
        {
            ret = false;
        }
    }
#endif

    return ret;
}

#if BLE_MGR_DEINIT
void bt_bond_deinit(void)
{
    if (p_bt_bond_cb)
    {
        T_BT_BOND_CBACK_ITEM *p_item;
        while ((p_item = os_queue_out(&p_bt_bond_cb->bt_bond_cback_list)) != NULL)
        {
            free(p_item);
        }
        free(p_bt_bond_cb);
        p_bt_bond_cb = NULL;
    }
    bt_bond_get_cback = NULL;
    bt_bond_proc_flag = 0;
#if F_BT_LE_SUPPORT
    bt_le_key_deinit();
#endif
#if F_BT_BREDR_SUPPORT
    bt_legacy_key_deinit();
#endif
}
#endif
#endif
