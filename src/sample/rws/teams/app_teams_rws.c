/*
 * Copyright (c) 2020, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_TEAMS_FEATURE_SUPPORT
#include "string.h"
#include "os_mem.h"
#include "trace.h"
#include "remote.h"
#include "teams_rws.h"
#include "app_bt_sniffing.h"
#include "app_teams_rws.h"
#include "app_cfg.h"
#include "app_relay.h"
#include "app_bt_policy_api.h"
#if F_APP_TEAMS_BT_POLICY
#include "app_teams_bt_policy.h"
#endif
#if F_APP_TEAMS_BLE_POLICY
#include "app_teams_ble_policy.h"
#endif
#include "app_asp_device.h"

static uint8_t *app_teams_asp_remote_info[TEAMS_LINK_NUM] = {0};
static uint8_t *app_teams_hid_remote_info[TEAMS_LINK_NUM] = {0};
static uint16_t app_teams_asp_remote_info_len[TEAMS_LINK_NUM] = {0};
static uint16_t app_teams_hid_remote_info_len[TEAMS_LINK_NUM] = {0};

void app_teams_rws_handle_role_swap_success(T_REMOTE_SESSION_ROLE device_role)
{
    uint8_t *bd_addr = NULL;

    bd_addr = asp_device_api_get_active_link_addr();
    teams_rws_handle_role_swap_success();
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        if (bd_addr)
        {
            bt_link_per_report(bd_addr, true, APP_ASP_PER_REPORT_PERIOD);
        }
    }
    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        if (bd_addr)
        {
            bt_link_per_report(bd_addr, false, APP_ASP_PER_REPORT_PERIOD);
        }
    }
}

void app_teams_rws_handle_role_swap_failed(T_REMOTE_SESSION_ROLE device_role)
{
    teams_rws_handle_role_swap_fail();
}

void app_teams_rws_save_build_roleswap_info(T_TEAMS_ASP_RWS_REMOTE_MSG_TYPE msg_type, uint8_t index,
                                            uint8_t *data, uint16_t data_len)
{
    if (msg_type == T_TEAMS_ASP_RWS_ASP_STACK_MSG)
    {
        app_teams_asp_remote_info[index] = data;
        app_teams_asp_remote_info_len[index] = data_len;
    }
    else if (msg_type == T_TEAMS_ASP_RWS_SPP_HID_CHANNEL_MSG)
    {
        app_teams_hid_remote_info[index] = data;
        app_teams_hid_remote_info_len[index] = data_len;
    }
}

void app_teams_rws_handle_asp_roleswap_info(void *data, uint16_t data_len)
{
    teams_rws_pre_handle_asp_roleswap_info();
    teams_rws_handle_asp_roleswap_info(data, data_len);
}

void app_teams_rws_handle_hid_roleswap_info(void *data, uint16_t data_len)
{
    teams_rws_pre_handle_hid_roleswap_info();
    teams_rws_handle_hid_roleswap_info(data, data_len);
}

static void app_teams_rws_lib_sync_to_primary(T_TEAMS_ASP_RWS_REMOTE_MSG_TYPE msg_type,
                                              void *buf, uint16_t len)
{
    int32_t length_total = len;
    uint16_t length = 0;
    uint8_t *p_head = buf;
    APP_PRINT_TRACE1("app_teams_rws_lib_sync_to_primary: msg_type %d", msg_type);

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }

    while (length_total)
    {
        length = *(uint16_t *)p_head;
        p_head += sizeof(uint16_t);
        length_total -= sizeof(uint16_t);
        if (msg_type == T_TEAMS_ASP_RWS_ASP_STACK_MSG)
        {
            app_teams_rws_handle_asp_roleswap_info(p_head, length);
        }
        else if (msg_type == T_TEAMS_ASP_RWS_SPP_HID_CHANNEL_MSG)
        {
            app_teams_rws_handle_hid_roleswap_info(p_head, length);
        }
        else
        {
            break;
        }

        p_head += length;
        length_total -= length;
    }
}

#if F_APP_ERWS_SUPPORT
static void app_teams_rws_lib_parse_cback(uint8_t msg_type, uint8_t *buf,
                                          uint16_t len, T_REMOTE_RELAY_STATUS status)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }

    APP_PRINT_INFO3("app_teams_rws_lib_parse_cback: msg_type %d, len %d, status %d", msg_type, len,
                    status);

    if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD && buf != NULL && len != 0)
    {
        app_teams_rws_lib_sync_to_primary((T_TEAMS_ASP_RWS_REMOTE_MSG_TYPE)msg_type, buf, len);
    }
}

// relay pkt size not larger than 500 bytes
static uint16_t app_teams_rws_lib_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    bool skip = true;

    APP_PRINT_INFO3("app_teams_rws_lib_relay_cback: msg_type %d, buf %p, total %d", msg_type, buf,
                    total);

    switch (msg_type)
    {
    case T_TEAMS_ASP_RWS_ASP_STACK_MSG:
        {
            if (!buf)
            {
                teams_rws_build_asp_roleswap_info(app_teams_rws_save_build_roleswap_info);
            }
            for (uint8_t i = 0; i < TEAMS_LINK_NUM; i++)
            {
                APP_PRINT_INFO3("app_teams_rws_lib_relay_cback: i %d, remote data len %d, remote data %p", i,
                                app_teams_asp_remote_info_len[i], app_teams_asp_remote_info[i]);
                if (buf && app_teams_asp_remote_info_len[i] && app_teams_asp_remote_info[i])
                {
                    memcpy(&buf[4 + payload_len], &app_teams_asp_remote_info_len[i],
                           sizeof(app_teams_asp_remote_info_len[i]));
                    payload_len += sizeof(app_teams_asp_remote_info_len[i]);
                    memcpy(&buf[4 + payload_len], app_teams_asp_remote_info[i], app_teams_asp_remote_info_len[i]);
                    payload_len += app_teams_asp_remote_info_len[i];
                }
                else if (app_teams_asp_remote_info_len[i] && app_teams_asp_remote_info[i])
                {
                    payload_len += sizeof(app_teams_asp_remote_info_len[i]);
                    payload_len += app_teams_asp_remote_info_len[i];
                }
            }
            skip = false;
        }
        break;
    case T_TEAMS_ASP_RWS_SPP_HID_CHANNEL_MSG:
        {
            if (!buf)
            {
                teams_rws_build_hid_roleswap_info(app_teams_rws_save_build_roleswap_info);
            }
            for (uint8_t i = 0; i < TEAMS_LINK_NUM; i++)
            {
                if (buf && app_teams_hid_remote_info_len[i] && app_teams_hid_remote_info[i])
                {
                    memcpy(&buf[4 + payload_len], &app_teams_hid_remote_info_len[i],
                           sizeof(app_teams_hid_remote_info_len[i]));
                    payload_len += sizeof(app_teams_hid_remote_info_len[i]);
                    memcpy(&buf[4 + payload_len], app_teams_hid_remote_info[i], app_teams_hid_remote_info_len[i]);
                    payload_len += app_teams_hid_remote_info_len[i];
                }
                else if (app_teams_hid_remote_info_len[i] && app_teams_hid_remote_info[i])
                {
                    payload_len += sizeof(app_teams_hid_remote_info_len[i]);
                    payload_len += app_teams_hid_remote_info_len[i];
                }
            }
            skip = false;
        }
        break;
    default:
        break;
    }

    uint16_t msg_len = payload_len + 4;

    if ((total == true) && (skip == true))
    {
        msg_len = 0;
    }

    if (buf != NULL)
    {
        if (((total == true) && (skip == false)) || (total == false))
        {
            buf[0] = (uint8_t)(msg_len & 0xFF);
            buf[1] = (uint8_t)(msg_len >> 8);
            buf[2] = APP_MODULE_TYPE_TEAMS_LIB;
            buf[3] = msg_type;
        }
        APP_PRINT_INFO2("app_teams_rws_lib_relay_cback: msg_type %d, msg_len %d", msg_type, msg_len);
    }

    if (buf)
    {
        for (uint8_t i = 0; i < TEAMS_LINK_NUM; i++)
        {
            if ((msg_type == T_TEAMS_ASP_RWS_ASP_STACK_MSG) && app_teams_asp_remote_info[i] &&
                app_teams_asp_remote_info_len[i])
            {
                os_mem_free(app_teams_asp_remote_info[i]);
                app_teams_asp_remote_info[i] = NULL;
                app_teams_asp_remote_info_len[i] = 0;
            }
            else if ((msg_type == T_TEAMS_ASP_RWS_SPP_HID_CHANNEL_MSG) && app_teams_hid_remote_info[i] &&
                     app_teams_hid_remote_info_len[i])
            {
                os_mem_free(app_teams_hid_remote_info[i]);
                app_teams_hid_remote_info_len[i] = 0;
            }
        }
    }

    return msg_len;
}

static void app_teams_rws_app_asp_sync_to_primary(T_TEAMS_APP_ASP_RWS_REMOTE_MSG_TYPE msg_type,
                                                  void *buf, uint16_t len)
{
    int32_t length_total = len;
    uint16_t length = 0;
    uint8_t *p_head = buf;
    APP_PRINT_TRACE1("app_teams_rws_app_asp_sync_to_primary: msg_type %d", msg_type);

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        APP_PRINT_ERROR0("app_teams_rws_app_asp_sync_to_primary: this dev should be secondary");
        return;
    }

    while (length_total)
    {
        length = *(uint16_t *)p_head;
        p_head += sizeof(uint16_t);
        length_total -= sizeof(uint16_t);

        if (msg_type == TEAMS_APP_RWS_ASP_MSG)
        {
            app_asp_device_handle_relay_info((T_APP_ASP_RELAY_INFO *)p_head, length);
        }
        else
        {
            APP_PRINT_ERROR1("app_teams_rws_app_asp_sync_to_primary: msg_type error %d", msg_type);
            break;
        }

        p_head += length;
        length_total -= length;
    }
}

static void app_teams_rws_app_asp_parse_cback(uint8_t msg_type, uint8_t *buf,
                                              uint16_t len, T_REMOTE_RELAY_STATUS status)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }

    APP_PRINT_INFO3("app_teams_rws_app_asp_parse_cback: msg_type %d, len %d, status %d", msg_type, len,
                    status);

    if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD && buf != NULL && len != 0)
    {
        app_teams_rws_app_asp_sync_to_primary((T_TEAMS_APP_ASP_RWS_REMOTE_MSG_TYPE)msg_type, buf, len);
    }
}

// relay pkt size not larger than 500 bytes
static uint16_t app_teams_rws_app_asp_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    bool skip = true;
    uint16_t pkt_len = 0;
    T_APP_ASP_RELAY_INFO app_asp_relay_info;

    APP_PRINT_INFO3("app_teams_rws_app_asp_relay_cback: msg_type %d, buf %p, total %d", msg_type, buf,
                    total);

    switch (msg_type)
    {
    case TEAMS_APP_RWS_ASP_MSG:
        {
            if (!buf)
            {
                payload_len += sizeof(pkt_len);
                payload_len += sizeof(T_APP_ASP_RELAY_INFO);
            }
            else
            {
                pkt_len = sizeof(T_APP_ASP_RELAY_INFO);
                app_asp_device_build_relay_info(&app_asp_relay_info);
                memcpy(&buf[4 + payload_len], &pkt_len, sizeof(pkt_len));
                payload_len += sizeof(pkt_len);
                memcpy(&buf[4 + payload_len], &app_asp_relay_info, sizeof(T_APP_ASP_RELAY_INFO));
                payload_len += sizeof(T_APP_ASP_RELAY_INFO);
            }
            skip = false;
        }
        break;
    default:
        break;
    }

    uint16_t msg_len = payload_len + 4;

    if ((total == true) && (skip == true))
    {
        msg_len = 0;
    }

    if (buf != NULL)
    {
        if (((total == true) && (skip == false)) || (total == false))
        {
            buf[0] = (uint8_t)(msg_len & 0xFF);
            buf[1] = (uint8_t)(msg_len >> 8);
            buf[2] = APP_MODULE_TYPE_TEAMS_APP_ASP;
            buf[3] = msg_type;
        }
        APP_PRINT_INFO2("app_teams_rws_app_asp_relay_cback: msg_type %d, msg_len %d", msg_type, msg_len);
    }

    return msg_len;
}

#if F_APP_TEAMS_BT_POLICY
static void app_teams_rws_app_bt_policy_sync_to_primary(T_TEAMS_APP_BT_POLICY_RWS_REMOTE_MSG_TYPE
                                                        msg_type,
                                                        void *buf, uint16_t len)
{
    int32_t length_total = len;
    uint16_t length = 0;
    uint8_t *p_head = buf;
    APP_PRINT_TRACE1("app_teams_rws_app_bt_policy_sync_to_primary: msg_type %d", msg_type);

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        APP_PRINT_ERROR0("app_teams_rws_app_bt_policy_sync_to_primary: this dev should be secondary");
        return;
    }

    while (length_total)
    {
        length = *(uint16_t *)p_head;
        p_head += sizeof(uint16_t);
        length_total -= sizeof(uint16_t);

        if (msg_type == TEAMS_APP_RWS_DEV_INFO_MSG)
        {
            app_teams_bt_policy_handle_relay_info((T_APP_TEAMS_RELAY_BT_POLICY_INFO *)p_head, length);
        }
        else
        {
            APP_PRINT_ERROR1("app_teams_rws_app_bt_policy_sync_to_primary: msg_type error %d", msg_type);
            break;
        }

        p_head += length;
        length_total -= length;
    }
}

static void app_teams_rws_app_bt_policy_parse_cback(uint8_t msg_type, uint8_t *buf,
                                                    uint16_t len, T_REMOTE_RELAY_STATUS status)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }

    APP_PRINT_INFO3("app_teams_rws_app_bt_policy_parse_cback: msg_type %d, len %d, status %d", msg_type,
                    len,
                    status);

    if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD && buf != NULL && len != 0)
    {
        app_teams_rws_app_bt_policy_sync_to_primary((T_TEAMS_APP_BT_POLICY_RWS_REMOTE_MSG_TYPE)msg_type,
                                                    buf, len);
    }
}

// relay pkt size not larger than 500 bytes
static uint16_t app_teams_rws_app_bt_policy_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    bool skip = true;
    uint16_t pkt_len = 0;
    T_APP_TEAMS_RELAY_BT_POLICY_INFO app_teams_bt_policy_relay_info;
    APP_PRINT_INFO3("app_teams_rws_app_bt_policy_relay_cback: msg_type %d, buf %p, total %d", msg_type,
                    buf,
                    total);

    switch (msg_type)
    {
    case TEAMS_APP_RWS_DEV_INFO_MSG:
        {
            if (!buf)
            {
                payload_len += sizeof(pkt_len);
                payload_len += sizeof(T_APP_TEAMS_RELAY_BT_POLICY_INFO);
            }
            else
            {
                pkt_len = sizeof(T_APP_TEAMS_RELAY_BT_POLICY_INFO);
                memcpy(&buf[4 + payload_len], &pkt_len, sizeof(pkt_len));
                payload_len += sizeof(pkt_len);
                app_teams_bt_policy_build_relay_info(&app_teams_bt_policy_relay_info);
                memcpy(&buf[4 + payload_len], &app_teams_bt_policy_relay_info,
                       sizeof(T_APP_TEAMS_RELAY_BT_POLICY_INFO));
                payload_len += sizeof(T_APP_TEAMS_RELAY_BT_POLICY_INFO);
            }
            skip = false;
        }
        break;
    default:
        break;
    }

    uint16_t msg_len = payload_len + 4;

    if ((total == true) && (skip == true))
    {
        msg_len = 0;
    }

    if (buf != NULL)
    {
        if (((total == true) && (skip == false)) || (total == false))
        {
            buf[0] = (uint8_t)(msg_len & 0xFF);
            buf[1] = (uint8_t)(msg_len >> 8);
            buf[2] = APP_MODULE_TYPE_TEAMS_APP_BT_POLICY;
            buf[3] = msg_type;
        }
        APP_PRINT_INFO2("app_teams_rws_app_bt_policy_relay_cback: msg_type %d, msg_len %d", msg_type,
                        msg_len);
    }

    return msg_len;
}
#endif

#if F_APP_TEAMS_BLE_POLICY
static void app_teams_rws_app_ble_policy_sync_to_primary(T_TEAMS_APP_BLE_POLICY_RWS_REMOTE_MSG_TYPE
                                                         msg_type,
                                                         void *buf, uint16_t len)
{
    int32_t length_total = len;
    uint16_t length = 0;
    uint8_t *p_head = buf;
    APP_PRINT_TRACE1("app_teams_rws_app_ble_policy_sync_to_primary: msg_type %d", msg_type);

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        APP_PRINT_ERROR0("app_teams_rws_app_ble_policy_sync_to_primary: this dev should be secondary");
        return;
    }

    while (length_total)
    {
        length = *(uint16_t *)p_head;
        p_head += sizeof(uint16_t);
        length_total -= sizeof(uint16_t);

        if (msg_type == TEAMS_APP_RWS_BLE_ADV_MSG)
        {
            app_teams_ble_policy_handle_adv_relay_info((T_APP_TEAMS_RELAY_BLE_POLICY_ADV_INFO *)p_head, length);
        }
        else if (msg_type == TEAMS_APP_RWS_BLE_DEV_BOND_MSG)
        {
            app_teams_ble_policy_handle_dev_bond_relay_info((T_APP_TEAMS_RELAY_BLE_POLICY_DEV_BOND_INFO *)
                                                            p_head, length);
        }
        else
        {
            APP_PRINT_ERROR1("app_teams_rws_app_ble_policy_sync_to_primary: msg_type error %d", msg_type);
            break;
        }

        p_head += length;
        length_total -= length;
    }
}

static void app_teams_rws_app_ble_policy_parse_cback(uint8_t msg_type, uint8_t *buf,
                                                     uint16_t len, T_REMOTE_RELAY_STATUS status)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }

    APP_PRINT_INFO3("app_teams_rws_app_ble_policy_parse_cback: msg_type %d, len %d, status %d",
                    msg_type, len,
                    status);

    if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD && buf != NULL && len != 0)
    {
        app_teams_rws_app_ble_policy_sync_to_primary((T_TEAMS_APP_BLE_POLICY_RWS_REMOTE_MSG_TYPE)msg_type,
                                                     buf, len);
    }
}

// relay pkt size not larger than 500 bytes
static uint16_t app_teams_rws_app_ble_policy_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    bool skip = true;
    uint16_t pkt_len = 0;
    T_APP_TEAMS_RELAY_BLE_POLICY_ADV_INFO app_teams_ble_policy_relay_adv_info;
    T_APP_TEAMS_RELAY_BLE_POLICY_DEV_BOND_INFO app_teams_ble_policy_relay_dev_bond_info;
    APP_PRINT_INFO3("app_teams_rws_app_ble_policy_relay_cback: msg_type %d, buf %p, total %d", msg_type,
                    buf,
                    total);

    switch (msg_type)
    {
    case TEAMS_APP_RWS_BLE_ADV_MSG:
        {
            if (!buf)
            {
                payload_len += sizeof(pkt_len);
                payload_len += sizeof(T_APP_TEAMS_RELAY_BLE_POLICY_ADV_INFO);
            }
            else
            {
                pkt_len = sizeof(T_APP_TEAMS_RELAY_BLE_POLICY_ADV_INFO);
                memcpy(&buf[4 + payload_len], &pkt_len, sizeof(pkt_len));
                payload_len += sizeof(pkt_len);
                app_teams_ble_policy_build_relay_adv_info(&app_teams_ble_policy_relay_adv_info);
                memcpy(&buf[4 + payload_len], &app_teams_ble_policy_relay_adv_info,
                       sizeof(T_APP_TEAMS_RELAY_BLE_POLICY_ADV_INFO));
                payload_len += sizeof(T_APP_TEAMS_RELAY_BLE_POLICY_ADV_INFO);
            }
            skip = false;
        }
        break;

    case TEAMS_APP_RWS_BLE_DEV_BOND_MSG:
        {
            if (!buf)
            {
                payload_len += sizeof(pkt_len);
                payload_len += sizeof(T_APP_TEAMS_RELAY_BLE_POLICY_DEV_BOND_INFO);
            }
            else
            {
                pkt_len = sizeof(T_APP_TEAMS_RELAY_BLE_POLICY_DEV_BOND_INFO);
                memcpy(&buf[4 + payload_len], &pkt_len, sizeof(pkt_len));
                payload_len += sizeof(pkt_len);
                app_teams_ble_policy_build_relay_dev_bond_info(&app_teams_ble_policy_relay_dev_bond_info);
                memcpy(&buf[4 + payload_len], &app_teams_ble_policy_relay_dev_bond_info,
                       sizeof(T_APP_TEAMS_RELAY_BLE_POLICY_DEV_BOND_INFO));
                payload_len += sizeof(T_APP_TEAMS_RELAY_BLE_POLICY_DEV_BOND_INFO);
            }
            skip = false;
        }
        break;

    default:
        break;
    }

    uint16_t msg_len = payload_len + 4;

    if ((total == true) && (skip == true))
    {
        msg_len = 0;
    }

    if (buf != NULL)
    {
        if (((total == true) && (skip == false)) || (total == false))
        {
            buf[0] = (uint8_t)(msg_len & 0xFF);
            buf[1] = (uint8_t)(msg_len >> 8);
            buf[2] = APP_MODULE_TYPE_TEAMS_APP_BLE_POLICY;
            buf[3] = msg_type;
        }
        APP_PRINT_INFO2("app_teams_rws_app_ble_policy_relay_cback: msg_type %d, msg_len %d", msg_type,
                        msg_len);
    }

    return msg_len;
}
#endif

//static void app_teams_send_roleswap_info(void)
//{
//    T_APP_RELAY_MSG_LIST_HANDLE relay_msg_handle = app_relay_async_build();
//    app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_TEAMS_LIB, T_TEAMS_ASP_RWS_ASP_STACK_MSG);
//    app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_TEAMS_LIB, T_TEAMS_ASP_RWS_SPP_HID_CHANNEL_MSG);
//    app_relay_async_send(relay_msg_handle);
//}

//void app_teams_roleswap_init(void)
//{
// app_bt_sniffing_roleswap_cb_reg(app_teams_send_roleswap_info);
//}

void app_teams_rws_init(void)
{
    app_relay_cback_register(app_teams_rws_lib_relay_cback, app_teams_rws_lib_parse_cback,
                             APP_MODULE_TYPE_TEAMS_LIB, T_TEAMS_LIB_RWS_MAX_TYPE);
    app_relay_cback_register(app_teams_rws_app_asp_relay_cback, app_teams_rws_app_asp_parse_cback,
                             APP_MODULE_TYPE_TEAMS_APP_ASP, TEAMS_APP_RWS_ASP_MAX_TYPE);
#if F_APP_TEAMS_BT_POLICY
    app_relay_cback_register(app_teams_rws_app_bt_policy_relay_cback,
                             app_teams_rws_app_bt_policy_parse_cback,
                             APP_MODULE_TYPE_TEAMS_APP_BT_POLICY, TEAMS_APP_RWS_BT_POLICY_MAX_TYPE);
#endif

#if F_APP_TEAMS_BLE_POLICY
    app_relay_cback_register(app_teams_rws_app_ble_policy_relay_cback,
                             app_teams_rws_app_ble_policy_parse_cback,
                             APP_MODULE_TYPE_TEAMS_APP_BLE_POLICY, TEAMS_APP_RWS_BLE_POLICY_MAX_TYPE);
#endif
    //app_teams_roleswap_init();
}
#endif
#endif

