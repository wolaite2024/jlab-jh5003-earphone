/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if XM_XIAOAI_FEATURE_SUPPORT

#include "os_mem.h"
#include "trace.h"
#include "app_main.h"
#include "xm_xiaoai_bt.h"
#include "app_xiaoai_transport.h"
#include "xm_xiaoai_api.h"
#include "app_xiaoai_device.h"
#include "app_xm_xiaoai_ble_adv.h"
#include "app_link_util.h"
#include "app_cfg.h"
#include "app_sdp.h"
#include "app_device.h"
#include "app_xiaoai_record.h"
#include "app_auto_power_off.h"

static void app_xm_xiaoai_rfc_cback(uint8_t *bd_addr, T_BT_RFC_MSG_TYPE msg_type, void *msg_buf)
{
    T_APP_BR_LINK *p_link;
    bool handle = true;

    APP_PRINT_INFO1("app_xm_xiaoai_rfc_cback: msg_type 0x%02x", msg_type);

    switch (msg_type)
    {
    case BT_RFC_MSG_CONN_IND:
        {
            T_BT_RFC_CONN_IND *p_ind = (T_BT_RFC_CONN_IND *)msg_buf;

            p_link = app_link_find_br_link(bd_addr);
            if (p_link != NULL)
            {
                bt_xm_xiaoai_connect_cfm(p_link->bd_addr, true, p_ind->frame_size, 7);
            }
            else
            {
                APP_PRINT_WARN0("app_xm_xiaoai_rfc_cback: no acl link found");
                bt_xm_xiaoai_connect_cfm(bd_addr, false, p_ind->frame_size, 7);
            }
        }
        break;

    case BT_RFC_MSG_CONN_CMPL:
        {
            T_BT_RFC_CONN_CMPL *p_cmpl = (T_BT_RFC_CONN_CMPL *)msg_buf;

            if (!app_cfg_const.enable_multi_link)
            {
                le_xm_xiaoai_adv_stop();
            }

            app_xiaoai_device_set_mtu(T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP, bd_addr, 0xFF, p_cmpl->frame_size);

            p_link = app_link_find_br_link(bd_addr);
            if (p_link != NULL)
            {
                //xm_xiaoai_alloc_br_link(bd_addr);
            }
        }
        break;

    case BT_RFC_MSG_DISCONN_CMPL:
        {
            p_link = app_link_find_br_link(bd_addr);
            if (p_link != NULL)
            {
                if (app_db.device_state == APP_DEVICE_STATE_ON)
                {
                    T_VA_STATE va_state = app_xiaoai_device_get_va_state(bd_addr);
                    if (va_state != VA_STATE_IDLE)
                    {
                        app_xiaoai_stop_recording(bd_addr);
                        app_xiaoai_device_set_va_state(bd_addr, VA_STATE_IDLE);
                    }

                    app_auto_power_off_enable(AUTO_POWER_OFF_MASK_XIAOAI_OTA, app_cfg_const.timer_auto_power_off);

                    le_xm_xiaoai_adv_start(extend_app_cfg_const.xiaoai_adv_timeout_val);
                }
            }
        }
        break;

    case BT_RFC_MSG_DATA_IND:
        {
            T_BT_RFC_DATA_IND *p_ind = (T_BT_RFC_DATA_IND *)msg_buf;
            p_link = app_link_find_br_link(bd_addr);
            if (p_link == NULL)
            {
                APP_PRINT_WARN0("app_xm_xiaoai_rfc_cback: no acl link found");
                return;
            }

            xm_rx_pkt(T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP, bd_addr, 0xFF, p_ind->buf, p_ind->length);

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
        APP_PRINT_INFO1("app_xm_xiaoai_rfc_cback: msg_type 0x%02x", msg_type);
    }

}


void app_xiaoai_rfc_init(void)
{
    APP_PRINT_INFO1("app_xm_xiaoai_rfc_init: xiaoai_tranport 0x%x",
                    extend_app_cfg_const.xiaoai_tranport);

    if (extend_app_cfg_const.xiaoai_tranport & TRANSPORT_BIT_SPP)
    {
        bt_xm_xiaoai_rfc_init(app_xm_xiaoai_rfc_cback, RFC_SPP_XIAOAI_CHANN_NUM);
    }
}
#endif
