/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_XIAOWEI_FEATURE_SUPPORT

#include "stdlib.h"
#include "trace.h"
#include "xiaowei_bt.h"
#include "app_xiaowei_transport.h"
#include "app_xiaowei.h"
#include "app_xiaowei_device.h"
#include "app_xiaowei_ble_adv.h"
#include "app_link_util.h"
#include "app_cfg.h"
#include "app_sdp.h"
#include "app_device.h"
#include "bt_rfc.h"
#include "app_xiaowei_record.h"
#include "app_main.h"
void app_xiaowei_handle_rfcomm_connected(uint8_t *bd_addr)
{
    APP_PRINT_INFO1("app_xiaowei_handle_rfcomm_connected: bd_addr %b", TRACE_BDADDR(bd_addr));
    app_xiaowei_device_create(XIAOWEI_CONNECTION_TYPE_SPP, bd_addr, 0xFF);
}

void app_xiaowei_handle_rfcomm_disconnected(uint8_t *bd_addr)
{
    APP_PRINT_INFO1("app_xiaowei_handle_rfcomm_disconnected: bd_addr %b", TRACE_BDADDR(bd_addr));

    if (app_db.device_state == APP_DEVICE_STATE_ON)
    {
        T_XIAOWEI_VA_STATE va_state = app_xiaowei_device_get_va_state(bd_addr);

        if (va_state != XIAOWEI_VA_STATE_IDLE)
        {
            app_xiaowei_stop_recording(bd_addr);
            app_xiaowei_device_set_va_state(bd_addr, XIAOWEI_VA_STATE_IDLE);
        }

        le_xiaowei_adv_start(extend_app_cfg_const.xiaowei_adv_timeout);
    }
}

static void app_xiaowei_rfc_cback(uint8_t *bd_addr, T_BT_RFC_MSG_TYPE msg_type, void *msg_buf)
{
    T_APP_BR_LINK *p_link;
    bool handle = true;

    //APP_PRINT_INFO1("app_xm_xiaowei_rfc_cback: msg_type 0x%02x", msg_type);

    switch (msg_type)
    {
    case BT_RFC_MSG_CONN_IND:
        {
            T_BT_RFC_CONN_IND *p_ind = (T_BT_RFC_CONN_IND *)msg_buf;

            p_link = app_link_find_br_link(bd_addr);
            if (p_link != NULL)
            {
                bt_xiaowei_connect_cfm(p_link->bd_addr, true, p_ind->frame_size, 7);
            }
            else
            {
//                APP_PRINT_WARN0("app_xm_xiaowei_rfc_cback: no acl link found");
                bt_xiaowei_connect_cfm(bd_addr, false, p_ind->frame_size, 7);
            }
        }
        break;

    case BT_RFC_MSG_CONN_CMPL:
        {
            T_BT_RFC_CONN_CMPL *p_cmpl = (T_BT_RFC_CONN_CMPL *)msg_buf;
            app_xiaowei_handle_rfcomm_connected(bd_addr);
            xiaowei_set_access_auth_result(XIAOWEI_CONNECTION_TYPE_SPP, bd_addr, 0xFF, true);
            if (!app_cfg_const.enable_multi_link)
            {
                le_xiaowei_adv_stop();
            }

            app_xiaowei_device_set_mtu(XIAOWEI_CONNECTION_TYPE_SPP, bd_addr, 0xFF, p_cmpl->frame_size);

            p_link = app_link_find_br_link(bd_addr);
            if (p_link != NULL)
            {
                app_xiaowei_alloc_br_link(bd_addr);
            }
        }
        break;

    case BT_RFC_MSG_DISCONN_CMPL:
        {
            p_link = app_link_find_br_link(bd_addr);
            if (p_link != NULL)
            {
                app_xiaowei_handle_rfcomm_disconnected(bd_addr);
            }
        }
        break;

    case BT_RFC_MSG_DATA_IND:
        {
            T_BT_RFC_DATA_IND *p_ind = (T_BT_RFC_DATA_IND *)msg_buf;
            p_link = app_link_find_br_link(bd_addr);
            if (p_link == NULL)
            {
//                APP_PRINT_WARN0("app_xm_xiaowei_rfc_cback: no acl link found");
                return;
            }

            app_xiaowei_receive_data(XIAOWEI_CONNECTION_TYPE_SPP, bd_addr, 0xFF, p_ind->buf, p_ind->length);

            bt_rfc_credits_give(bd_addr, p_ind->local_server_chann, 1);
        }
        break;

    case BT_RFC_MSG_CREDIT_INFO:
        break;

    default:
        {
            handle = false;
        }
        break;
    }

    if (handle == true)
    {
//        APP_PRINT_INFO1("app_xm_xiaowei_rfc_cback: msg_type 0x%02x", msg_type);
    }

}


void app_xiaowei_rfc_init(void)
{
    APP_PRINT_INFO1("app_xiaowei_rfc_init: xiaowei_tranport 0x%x",
                    extend_app_cfg_const.xiaowei_tranport);

    if (extend_app_cfg_const.xiaowei_tranport & TRANSPORT_BIT_SPP)
    {
        bt_xiaowei_rfc_init(app_xiaowei_rfc_cback, RFC_SPP_XIAOWEI_CHANN_NUM);
    }
}
#endif
