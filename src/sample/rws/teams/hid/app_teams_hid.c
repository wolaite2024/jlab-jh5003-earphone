/*
 * Copyright (c) 2020, Realsil Semiconductor Corporation. All rights reserved.
 */
#include "os_mem.h"
#include "trace.h"
#include "btm.h"
#if (F_APP_TEAMS_HID_SUPPORT|F_APP_TEAMS_VP_UPDATE_SUPPORT|F_APP_TEAMS_FEATURE_SUPPORT)
#include "string.h"
#endif
#include "app_cfg.h"
#include "app_main.h"
#include "hid_link.h"
#include "hid_api.h"
#if F_APP_CFU_FEATURE_SUPPORT
#include "app_cfu_transfer.h"
#endif
#include "hid_vp_update.h"
#include "voice_prompt.h"
#include "bt_hid_device.h"
#include "app_bt_policy_api.h"
#include "app_teams_hid.h"
#include "app_teams_cmd.h"
#if F_APP_USB_HID_SUPPORT
#include "app_usb_audio_hid.h"
#endif
#include "app_dongle_spp.h"
#include "app_dongle_data_ctrl.h"

#if F_APP_COMMON_DONGLE_SUPPORT
#include "app_gaming_sync.h"
#endif

#if F_APP_TEAMS_VP_UPDATE_SUPPORT
static bool vp_update_is_running = false;
static uint8_t vp_update_bd_addr[6] = {0};
#endif
#if F_APP_TEAMS_FEATURE_SUPPORT
uint8_t *teams_rsp_cache = NULL;
uint16_t teams_rsp_len = 62;
#endif

#if F_APP_TEAMS_HID_SUPPORT
bool app_teams_hid_send_classic_hid_data(uint8_t *bd_addr, uint8_t report_id, uint8_t report_type,
                                         T_APP_TEAMS_HID_REPORT_ACTION_TYPE report_action, uint16_t len,
                                         uint8_t *data)
{
    uint8_t *header = NULL;
    uint16_t data_len = sizeof(report_id) + len;
    bool ret = false;

    header = os_mem_zalloc(OS_MEM_TYPE_DATA, data_len);
    if (!header)
    {
        APP_PRINT_ERROR0("app_teams_hid_send_classic_hid_data: no merroy for data");
        return ret;
    }

    header[0] = report_id;
    memcpy(&header[1], data, len);

    if (report_action == APP_TEAMS_HID_REPORT_ACTION_GET)
    {
        ret = bt_hid_device_get_report_rsp(bd_addr, (T_BT_HID_DEVICE_REPORT_TYPE)report_type, header,
                                           data_len);
    }
    else if (report_action == APP_TEAMS_HID_REPORT_ACTION_SET)
    {
        ret = bt_hid_device_interrupt_data_send(bd_addr, (T_BT_HID_DEVICE_REPORT_TYPE)report_type, header,
                                                data_len);
    }

    os_mem_free(header);
    return ret;
}
#endif

//this api only for bt check
bool app_teams_hid_vp_update_is_process_check(T_APP_BR_LINK **p_link)
{
#if F_APP_TEAMS_VP_UPDATE_SUPPORT
    if (vp_update_is_running)
    {
        if (!memcmp(vp_update_bd_addr, teams_asp_usb_addr, 6))
        {
            return false;
        }

        if (p_link)
        {
            *p_link = app_link_find_br_link(vp_update_bd_addr);
        }
        return true;
    }
#endif
    return false;
}

#if F_APP_TEAMS_VP_UPDATE_SUPPORT
void app_teams_hid_vp_update_cback(uint8_t *bd_addr, uint8_t type, uint8_t ret)
{
    T_VOICE_PROMPT_LANGUAGE_ID index = (T_VOICE_PROMPT_LANGUAGE_ID)app_cfg_nv.voice_prompt_language;
    T_APP_BR_LINK *p_link;
    APP_PRINT_TRACE3("app_teams_hid_vp_update_cback: type %d, ret %d, index %d",
                     type, ret, index);
    p_link = app_link_find_br_link(bd_addr);
    /* vp update start*/
    if ((type == HID_VP_UPDATE_START))
    {
        /* save update bd addr and set flag*/
        vp_update_is_running = true;
        memcpy(vp_update_bd_addr, bd_addr, 6);
        /* due to spec, if vp update the current language, change language to index 0 */
        if ((index == (VP_UPDATE_SUPPORT_LANGUAGE_NUM - 1)) &&
            (voice_prompt_language_set((T_VOICE_PROMPT_LANGUAGE_ID)0)))
        {
            app_cfg_nv.voice_prompt_language = 0;
        }
        if (p_link)
        {
            app_bt_policy_b2s_tpoll_update(bd_addr, BP_TPOLL_EVENT_VP_UPDATE_START);
        }
    }
    /* vp update end*/
    else if ((type == HID_VP_UPDATE_DATA_VERIFY) || (type == HID_VP_UPDATE_ABORT) || ret)
    {
        /* reset vp update flag*/
        vp_update_is_running = false;
        if (p_link)
        {
            app_bt_policy_b2s_tpoll_update(bd_addr, BP_TPOLL_EVENT_VP_UPDATE_STOP);
            //app_handle_upgrading_end_matter(p_link);
        }
    }
}
#endif

#if F_APP_TEAMS_FEATURE_SUPPORT
/*note: 1. data on spp or classic hid channel not include report id, usb need report id in first byte
        2. classic hid and usb , app user should manager data size which equal to length in descripor*/
bool app_teams_hid_send_wl_report(uint8_t report_id, T_APP_TEAMS_HID_REPORT_SOURCE source_type,
                                  T_APP_TEAMS_HID_DATA *hid_msg)
{
    APP_PRINT_TRACE2("app_teams_hid_send_wl_report: report_id %d, source_type %d",
                     report_id, source_type);

    if (source_type == APP_TEAMS_HID_REPORT_SOURCE_SPP)
    {
        return hid_api_send_wl_report(hid_msg->teams_spp_data.bd_addr, report_id,
                                      hid_msg->teams_spp_data.data_len, hid_msg->teams_spp_data.p_data);
    }
    else if (source_type == APP_TEAMS_HID_REPORT_SOURCE_CLASSIC_HID)
    {
#if F_APP_TEAMS_HID_SUPPORT
        return app_teams_hid_send_classic_hid_data(hid_msg->teams_classic_hid_data.bd_addr,
                                                   hid_msg->teams_classic_hid_data.report_id,
                                                   hid_msg->teams_classic_hid_data.report_type,
                                                   hid_msg->teams_classic_hid_data.report_action,
                                                   hid_msg->teams_classic_hid_data.data_len,
                                                   hid_msg->teams_classic_hid_data.p_data);
#endif
    }
    else if (source_type == APP_TEAMS_HID_REPORT_SOURCE_USB)
    {
#if F_APP_USB_HID_SUPPORT
        app_hid_interrupt_in(hid_msg->teams_usb_data.p_data, hid_msg->teams_usb_data.data_len);
#endif
    }
    else if (source_type == APP_TEAMS_HID_REPORT_SOURCE_BLE)
    {
        //TO_DO
    }
    return false;
}

void app_teams_hid_handle_wl_report(uint8_t report_id, T_APP_TEAMS_HID_REPORT_SOURCE source_type,
                                    T_APP_TEAMS_HID_DATA *p_hid_data)
{

}

bool app_teams_hid_send_config_report(uint8_t report_id, T_APP_TEAMS_HID_REPORT_SOURCE source_type,
                                      T_APP_TEAMS_HID_DATA *hid_msg)
{
    APP_PRINT_TRACE2("app_teams_hid_send_config_report: report_id %d, source_type %d",
                     report_id, source_type);

    if (source_type == APP_TEAMS_HID_REPORT_SOURCE_SPP)
    {
        return hid_api_send_app_config_report(hid_msg->teams_spp_data.bd_addr, report_id,
                                              hid_msg->teams_spp_data.data_len, hid_msg->teams_spp_data.p_data);
    }
    else if (source_type == APP_TEAMS_HID_REPORT_SOURCE_CLASSIC_HID)
    {
#if F_APP_TEAMS_HID_SUPPORT
        return app_teams_hid_send_classic_hid_data(hid_msg->teams_classic_hid_data.bd_addr,
                                                   hid_msg->teams_classic_hid_data.report_id,
                                                   hid_msg->teams_classic_hid_data.report_type,
                                                   hid_msg->teams_classic_hid_data.report_action,
                                                   hid_msg->teams_classic_hid_data.data_len,
                                                   hid_msg->teams_classic_hid_data.p_data);
#endif
    }
    else if (source_type == APP_TEAMS_HID_REPORT_SOURCE_USB)
    {
        //user handle in owner module
    }
    else if (source_type == APP_TEAMS_HID_REPORT_SOURCE_BLE)
    {
        //TO_DO
    }
    return false;
}

void app_teams_hid_handle_config_report(uint8_t report_id,
                                        T_APP_TEAMS_HID_REPORT_SOURCE source_type,
                                        T_APP_TEAMS_HID_DATA *p_hid_data)
{
    uint8_t report_type;
    T_APP_TEAMS_HID_REPORT_ACTION_TYPE report_action;

    APP_PRINT_INFO2("app_teams_hid_handle_config_report: report_id 0x%x, source_type %d",
                    report_id, source_type);

    if (source_type == APP_TEAMS_HID_REPORT_SOURCE_SPP)
    {
        if (report_id == APP_CONFIG_H2D)
        {
            APP_PRINT_TRACE0("dongle command");

            app_teams_cmd_handle_data(p_hid_data->teams_spp_data.bd_addr, APP_CONFIG_H2D,
                                      BT_HID_DEVICE_REPORT_TYPE_RESERVED, APP_TEAMS_HID_REPORT_ACTION_NONE,
                                      APP_TEAMS_HID_REPORT_SOURCE_SPP,
                                      p_hid_data->teams_spp_data.data_len, p_hid_data->teams_spp_data.p_data);

        }
    }
    else if (source_type == APP_TEAMS_HID_REPORT_SOURCE_CLASSIC_HID)
    {
        report_type = p_hid_data->teams_classic_hid_data.report_type;
        report_action = p_hid_data->teams_classic_hid_data.report_action;
        if (report_action == APP_TEAMS_HID_REPORT_ACTION_GET)
        {
            if (report_id == APP_CONFIG_D2H)
            {
                if (teams_rsp_cache)
                {
#if F_APP_TEAMS_HID_SUPPORT
                    app_teams_hid_send_classic_hid_data(p_hid_data->teams_classic_hid_data.bd_addr, APP_CONFIG_D2H,
                                                        report_type,
                                                        report_action,
                                                        teams_rsp_len, teams_rsp_cache);
#endif
                    os_mem_free(teams_rsp_cache);
                    teams_rsp_cache = NULL;
                }
            }
            else if (report_id == APP_CONFIG_H2D)
            {
                if (teams_rsp_cache)
                {
#if F_APP_TEAMS_HID_SUPPORT
                    app_teams_hid_send_classic_hid_data(p_hid_data->teams_classic_hid_data.bd_addr, APP_CONFIG_H2D,
                                                        report_type,
                                                        report_action,
                                                        teams_rsp_len, teams_rsp_cache);
#endif
                    os_mem_free(teams_rsp_cache);
                    teams_rsp_cache = NULL;
                }
            }
        }
        else if (report_action == APP_TEAMS_HID_REPORT_ACTION_SET)
        {
            if (report_id == APP_CONFIG_H2D)
            {
                app_teams_cmd_handle_data(p_hid_data->teams_classic_hid_data.bd_addr, APP_CONFIG_H2D, report_type,
                                          report_action,
                                          APP_TEAMS_HID_REPORT_SOURCE_CLASSIC_HID,
                                          p_hid_data->teams_classic_hid_data.data_len, p_hid_data->teams_classic_hid_data.p_data);
            }
        }
    }
    else if (source_type == APP_TEAMS_HID_REPORT_SOURCE_USB)
    {
#if F_APP_USB_HID_SUPPORT
        app_teams_cmd_handle_data(NULL, REPORT_ID_USB_COMMAND_SET, BT_HID_DEVICE_REPORT_TYPE_RESERVED,
                                  APP_TEAMS_HID_REPORT_ACTION_NONE, APP_TEAMS_HID_REPORT_SOURCE_USB,
                                  p_hid_data->teams_usb_data.data_len, p_hid_data->teams_usb_data.p_data);
#endif
    }
    else if (source_type == APP_TEAMS_HID_REPORT_SOURCE_BLE)
    {
        //TO_DO
    }
    else
    {

    }
}

bool app_teams_hid_send_spp_data(T_HID_CMD_TYPE cmd, uint8_t *data, uint16_t len)
{
#if F_APP_COMMON_DONGLE_SUPPORT
    if (cmd == HID_MTE_CMD)
    {
        return app_dongle_send_cmd(DONGLE_CMD_MTE_DATA, data, len);
    }
    else if (cmd == HID_VP_UPDATE_CMD)
    {
        return app_dongle_send_cmd(DONGLE_CMD_VP_UPDATE_DATA, data, len);
    }
#endif
    return false;
}

void app_teams_hid_handle_report_cback(uint8_t *bd_addr, uint8_t report_id, uint8_t len,
                                       uint8_t *data)
{
    APP_PRINT_INFO3("app_teams_hid_handle_report_cback: bd_addr %s, report_id 0x%x, data_len %d",
                    TRACE_BDADDR(bd_addr), report_id, len);
    T_APP_TEAMS_HID_DATA hid_msg;
    memcpy(hid_msg.teams_spp_data.bd_addr, bd_addr, 6);
    hid_msg.teams_spp_data.data_len = len;
    hid_msg.teams_spp_data.p_data = data;

    switch (report_id)
    {
    case VP_UPDATE_PC_TO_HEADSET:
#if F_APP_TEAMS_VP_UPDATE_SUPPORT
        hid_vp_update_handle_data_ind(bd_addr, len, data);
#endif
        break;

    case APP_CONFIG_H2D:
        {
            app_teams_hid_handle_config_report(report_id, APP_TEAMS_HID_REPORT_SOURCE_SPP, &hid_msg);
        }
        break;
    default:
        break;

    }
}

bool app_teams_hid_init()
{
#if (F_APP_TEAMS_VP_UPDATE_SUPPORT)
    return hid_api_init(app_dongle_send_cmd, app_teams_hid_handle_report_cback,
                        app_teams_hid_vp_update_cback);
#else
    return hid_api_init(app_dongle_send_cmd, app_teams_hid_handle_report_cback, NULL);
#endif
}

#endif

