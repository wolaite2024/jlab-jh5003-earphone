/*
 * Copyright (c) 2020, Realsil Semiconductor Corporation. All rights reserved.
 */
#include "os_mem.h"
#include "trace.h"
#include "app_link_util.h"
#include "app_sdp.h"

#if F_APP_TEAMS_FEATURE_SUPPORT
#include "string.h"
#include "asp_device_bt.h"
#include "asp_device_link.h"
#include "asp_device_api.h"
#include "app_asp_device_rfc.h"
#include "app_asp_device.h"

static void app_asp_device_rfc_cback(uint8_t *bd_addr, T_BT_RFC_MSG_TYPE msg_type, void *msg_buf)
{
    T_APP_BR_LINK *p_link;
    bool handle = true;
    T_ASP_INFO_LINK_STATE_DATA asp_link_state;
    T_ASP_DEVICE_LINK_ALLOC_PARAM param_data = {0};

    switch (msg_type)
    {
    case BT_RFC_MSG_CONN_IND:
        {
            T_BT_RFC_CONN_IND *p_ind = (T_BT_RFC_CONN_IND *)msg_buf;

            p_link = app_link_find_br_link(bd_addr);
            if (p_link != NULL)
            {
                bt_asp_device_connect_cfm(p_link->bd_addr, true, p_ind->frame_size, 7);
            }
            else
            {
                APP_PRINT_WARN0("app_asp_device_rfc_cback: no acl link found");
                bt_asp_device_connect_cfm(bd_addr, false, p_ind->frame_size, 7);
            }
        }
        break;

    case BT_RFC_MSG_CONN_CMPL:
        {
            p_link = app_link_find_br_link(bd_addr);
            if (p_link != NULL)
            {
                param_data.bd_addr = bd_addr;
                asp_device_alloc_link(ASP_DEVICE_LINK_DONGLE, &param_data);
                app_asp_device_alloc_link(bd_addr);
            }
        }
        break;

    case BT_RFC_MSG_DISCONN_CMPL:
        {
            p_link = app_link_find_br_link(bd_addr);
            if (p_link != NULL)
            {
                T_ASP_DEVICE_LINK *p_asp_link = NULL;
                p_asp_link = asp_device_find_link_by_addr(bd_addr);
                if (p_asp_link && p_asp_link->link_state == ASP_DEVICE_LINK_CONNECTED_ACTIVE)
                {
                    asp_link_state.asp_link_state = false;
                    memcpy(asp_link_state.bd_addr, bd_addr, 6);
                    asp_device_handle_asp_info(T_ASP_DEVICE_ASP_INFO_STATE, &asp_link_state, sizeof(asp_link_state));
                }
                asp_device_free_link(bd_addr);
                app_asp_device_delete_link(bd_addr);
            }
        }
        break;

    case BT_RFC_MSG_DATA_IND:
        {
            T_BT_RFC_DATA_IND *p_ind = (T_BT_RFC_DATA_IND *)msg_buf;
            p_link = app_link_find_br_link(bd_addr);
            if (p_link == NULL)
            {
                APP_PRINT_WARN0("app_asp_device_rfc_cback: no acl link found");
                return;
            }
            APP_PRINT_INFO3("app_asp_device_rfc_cback: br_addr %s, buf %b, length %d",
                            TRACE_BDADDR(bd_addr), TRACE_BINARY(p_ind->length, p_ind->buf), p_ind->length);
            asp_device_api_receive_pkt(bd_addr, p_ind->length, p_ind->buf);

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
        APP_PRINT_INFO1("app_asp_device_rfc_cback: msg_type 0x%02x", msg_type);
    }

}


void app_asp_device_rfc_init(void)
{
    bt_asp_device_rfc_init(app_asp_device_rfc_cback, RFC_HS_ASP_CHANN_NUM);
}

#endif

