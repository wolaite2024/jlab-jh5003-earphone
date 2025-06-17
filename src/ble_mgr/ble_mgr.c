#include <stdlib.h>
#include "ble_mgr.h"
#include "ble_ext_adv_int.h"
#include "ble_adv_data_int.h"
#include "ble_conn_int.h"
#include "ble_scan_int.h"
#include "gap_ext_adv.h"
#include "ble_mgr_int.h"
#include "bt_gatt_client_int.h"
#include "bt_gatt_svc_int.h"

typedef struct CB_ELEM
{
    struct CB_ELEM *p_next;
    BLE_MGR_MSG_CB cb;
} CB_ELEM;

T_BLE_MGR_CB *p_ble_mgr = NULL;

void ble_mgr_init(BLE_MGR_PARAMS *p_params)
{
#if BLE_MGR_DEINIT
    if (ble_mgr_deinit_flow != 0)
    {
        return;
    }
#endif
    if (p_ble_mgr != NULL)
    {
        return;
    }
    p_ble_mgr = calloc(1, sizeof(T_BLE_MGR_CB));
    if (p_ble_mgr == NULL)
    {
        return;
    }
    if (p_params->ble_adv_data.enable == true)
    {
        p_params->ble_ext_adv.enable = true;
    }

    if (p_params->ble_ext_adv.enable)
    {
        ble_ext_adv_mgr_init(p_params->ble_ext_adv.adv_num);
        p_ble_mgr->ble_ext_adv.enable = true;
    }

    if (p_params->ble_conn.enable)
    {
        ble_conn_update_init(p_params->ble_conn.link_num);
        p_ble_mgr->ble_conn.enable = true;
    }

    if (p_params->ble_adv_data.enable)
    {
        bool                    update_scan_data  = p_params->ble_adv_data.update_scan_data;
        T_GAP_LOCAL_ADDR_TYPE   own_address_type  = p_params->ble_adv_data.own_address_type;
        T_GAP_REMOTE_ADDR_TYPE  peer_address_type = p_params->ble_adv_data.peer_address_type;
        uint32_t                adv_interval = p_params->ble_adv_data.adv_interval;
        uint32_t                scan_rsp_len = p_params->ble_adv_data.scan_rsp_len;
        uint8_t                 *scan_rsp_data = p_params->ble_adv_data.scan_rsp_data;
        ble_adv_data_init(update_scan_data, own_address_type, peer_address_type, adv_interval, scan_rsp_len,
                          scan_rsp_data);
        p_ble_mgr->ble_adv_data.enable = true;
    }

    if (p_params->ble_scan.enable)
    {
        ble_scan_init();
        p_ble_mgr->ble_scan.enable = true;
    }

    os_queue_init(&p_ble_mgr->msg_cb_queue);
}


bool ble_mgr_msg_cback_register(BLE_MGR_MSG_CB cb)
{
    if (p_ble_mgr == NULL)
    {
        return false;
    }
    CB_ELEM *p_ele = calloc(1, sizeof(*p_ele));
    if (p_ele)
    {
        p_ele->cb = cb;
        os_queue_in(&p_ble_mgr->msg_cb_queue, p_ele);
        return true;
    }

    return false;
}


void ble_mgr_handle_gap_cb(uint8_t cb_type, T_LE_CB_DATA *cb_data)
{
#if BLE_MGR_DEINIT
    if (ble_mgr_deinit_flow != 0)
    {
        return;
    }
#endif
    if (p_ble_mgr == NULL)
    {
        return;
    }
    switch (cb_type)
    {
    case GAP_MSG_LE_EXT_ADV_STATE_CHANGE_INFO:
        {
            uint8_t adv_handle = cb_data->p_le_ext_adv_state_change_info->adv_handle;
            T_GAP_EXT_ADV_STATE state = (T_GAP_EXT_ADV_STATE)cb_data->p_le_ext_adv_state_change_info->state;
            uint16_t cause = cb_data->p_le_ext_adv_state_change_info->cause;
            if (p_ble_mgr->ble_ext_adv.enable)
            {
                ble_ext_adv_mgr_handle_adv_state(adv_handle, state, cause);
            }
        }
        break;

    case GAP_MSG_LE_EXT_SCAN_STATE_CHANGE_INFO:
        if (p_ble_mgr->ble_scan.enable)
        {
            ble_scan_handle_state(cb_data->p_le_ext_scan_state_change_info->state,
                                  cb_data->p_le_ext_scan_state_change_info->cause);
        }
        break;

    case GAP_MSG_LE_EXT_ADV_START_SETTING:
        if (p_ble_mgr->ble_ext_adv.enable)
        {
            ble_ext_adv_mgr_handle_gap_cb(cb_type, cb_data);
        }
        break;

    case GAP_MSG_LE_EXT_ADV_REPORT_INFO:
        if (p_ble_mgr->ble_scan.enable)
        {
            ble_scan_handle_gap_cb(cb_type, cb_data);
        }
        break;

    default:
        break;
    }
}

void ble_mgr_handle_gap_common_cb(uint8_t cb_type, void *p_cb_data)
{
#if BLE_MGR_DEINIT
    if (ble_mgr_deinit_flow != 0)
    {
        return;
    }
#endif

#if CONFIG_REALTEK_BT_GATT_CLIENT_SUPPORT
    if (gattc_dis_mode != GATT_CLIENT_DISCOV_MODE_CLOSED)
    {
        gatt_client_handle_gap_common_cb(cb_type, p_cb_data);
    }
#endif

#if CONFIG_REALTEK_BT_GATT_SVC_SUPPORT
    if (gatt_svc_mode != GATT_SVC_DISABLE)
    {
        gatt_svc_handle_gap_common_cb(cb_type, p_cb_data);
    }
#endif
}

void ble_mgr_handle_gap_msg(uint8_t subtype, T_LE_GAP_MSG *gap_msg)
{
#if BLE_MGR_DEINIT
    if (ble_mgr_deinit_flow != 0)
    {
        return;
    }
#endif
    switch (subtype)
    {
    case GAP_MSG_LE_DEV_STATE_CHANGE:
        if (p_ble_mgr && p_ble_mgr->ble_scan.enable)
        {
            ble_scan_handle_state(gap_msg->msg_data.gap_dev_state_change.new_state.gap_scan_state,
                                  gap_msg->msg_data.gap_dev_state_change.cause);
        }
        break;

    case GAP_MSG_LE_CONN_STATE_CHANGE:
        {
            uint8_t conn_id = gap_msg->msg_data.gap_conn_state_change.conn_id;
            T_GAP_CONN_STATE new_state = (T_GAP_CONN_STATE)gap_msg->msg_data.gap_conn_state_change.new_state;
            uint16_t disc_cause = gap_msg->msg_data.gap_conn_state_change.disc_cause;

            if (p_ble_mgr && p_ble_mgr->ble_ext_adv.enable)
            {
                ble_ext_adv_mgr_handle_conn_state(conn_id, new_state, disc_cause);
            }

            if (p_ble_mgr && p_ble_mgr->ble_adv_data.enable)
            {
                ble_adv_data_handle_conn_state(conn_id, new_state, disc_cause);
            }

            if (p_ble_mgr && p_ble_mgr->ble_conn.enable)
            {
                ble_handle_conn_state_evt(conn_id, new_state);
            }
#if CONFIG_REALTEK_BT_GATT_CLIENT_SUPPORT
            if (gattc_dis_mode != GATT_CLIENT_DISCOV_MODE_CLOSED)
            {
                gatt_client_handle_conn_state_evt(conn_id, new_state);
            }
#endif
#if CONFIG_REALTEK_BT_GATT_SVC_SUPPORT
            if (gatt_svc_mode != GATT_SVC_DISABLE)
            {
                gatt_svc_handle_conn_state_evt(conn_id, new_state);
            }
#endif
        }
        break;

    case GAP_MSG_LE_CONN_MTU_INFO:
        {
#if CONFIG_REALTEK_BT_GATT_CLIENT_SUPPORT
            if (gattc_dis_mode != GATT_CLIENT_DISCOV_MODE_CLOSED)
            {
                gatt_client_handle_mtu_info(gap_msg->msg_data.gap_conn_mtu_info.conn_id,
                                            gap_msg->msg_data.gap_conn_mtu_info.mtu_size);
            }
#endif
        }
        break;

    case GAP_MSG_LE_CONN_PARAM_UPDATE:
        {
            if (p_ble_mgr && p_ble_mgr->ble_conn.enable)
            {
                ble_handle_conn_update_info(gap_msg->msg_data.gap_conn_param_update);
            }
        }
        break;

    case GAP_MSG_LE_EXT_ADV_STATE_CHANGE:
        {
            if (p_ble_mgr && p_ble_mgr->ble_ext_adv.enable)
            {
                uint8_t adv_handle = gap_msg->msg_data.gap_ext_adv_state_change.adv_handle;
                T_GAP_EXT_ADV_STATE new_state = (T_GAP_EXT_ADV_STATE)
                                                gap_msg->msg_data.gap_ext_adv_state_change.new_state;
                uint16_t cause = gap_msg->msg_data.gap_ext_adv_state_change.cause;

                ble_ext_adv_mgr_handle_adv_state(adv_handle, new_state, cause);
            }
        }
        break;

    default:
        break;
    }
    if (p_ble_mgr)
    {
        CB_ELEM *p_ele = (CB_ELEM *)p_ble_mgr->msg_cb_queue.p_first;
        for (; p_ele != NULL; p_ele = p_ele->p_next)
        {
            p_ele->cb(subtype, gap_msg);
        }
    }
}

#if BLE_MGR_DEINIT
uint8_t ble_mgr_deinit_flow = 0;

void ble_mgr_deinit(void)
{
    ble_mgr_deinit_flow |= BLE_MGR_DEINIT_FLAG;
    if (p_ble_mgr)
    {
        CB_ELEM *p_ele;
        while ((p_ele = os_queue_out(&p_ble_mgr->msg_cb_queue)) != NULL)
        {
            free(p_ele);
        }

        ble_conn_update_deinit();
        ble_adv_data_deinit();
        ble_ext_adv_mgr_deinit();
        ble_scan_deinit();
#if CONFIG_REALTEK_BT_GATT_CLIENT_SUPPORT
        gatt_client_deinit();
#endif
#if CONFIG_REALTEK_BT_GATT_SVC_SUPPORT
        gatt_svc_deinit();
#endif
#if CONFIG_REALTEK_APP_BOND_MGR_SUPPORT
        bt_bond_deinit();
#endif
        free(p_ble_mgr);
        p_ble_mgr = NULL;
    }
    ble_mgr_deinit_flow = 0;
}
#endif

