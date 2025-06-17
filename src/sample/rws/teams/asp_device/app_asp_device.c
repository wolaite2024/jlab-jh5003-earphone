/*
 * Copyright (c) 2020, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "os_mem.h"
#include "trace.h"
#include "app_main.h"

#if F_APP_TEAMS_FEATURE_SUPPORT
#include <string.h>
#include "stdlib.h"
#include "gap_conn_le.h"
#include "app_timer.h"
#include "app_link_util.h"
#include "asp_device_link.h"
#include "app_bt_policy_api.h"
#include "btm.h"
#include "app_asp_device.h"
#include "app_sdp.h"
#include "app_cfg.h"
#include "app_teams_cmd.h"
#include "version.h"
#include "app_teams_extend_led.h"
#include "app_led.h"
#include "app_hfp.h"
#include "app_a2dp.h"
#include "asp_ble_service.h"
#if CONFIG_REALTEK_BT_GATT_CLIENT_SUPPORT
#include "asp_svc.h"
#endif
#include "os_sched.h"
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
#include "app_single_multilink_customer.h"
#endif
#if F_APP_USB_HID_SUPPORT
#include "hal_adp.h"
#endif

#define TEAMSNUM3STR(a,b,c)         #a "." #b "." #c
#define TEAMSVERSIONBUILDSTR(a,b,c) TEAMSNUM3STR(a,b,c)
#define TEAMS_VERSION_BUILD_STR        TEAMSVERSIONBUILDSTR(VERSION_MAJOR,VERSION_MINOR,VERSION_BUILDNUM)

static uint8_t teams_launch = 0;
static uint8_t teams_notify = 0;

/* msg data field*/
static uint8_t telemetry_data[64] = {0};

/*telemetry data */
static uint8_t current_firmware[] = {TEAMS_VERSION_BUILD_STR};
static uint8_t sidetone_level[] = {'1', '.', '2', '5'};
#if F_APP_TEAMS_CUSTOMIZED_CMD_SUPPORT
#else
static uint8_t serial_num[16] = {0};
#endif
static T_APP_ASP_DEVICE_TELEMETRY_AUDIO_CODEC_TYPE audio_codec_type =
    T_APP_ASP_DEVICE_TELEMETRY_AUDIO_CODEC_WIDEBAND_TYPE;
static uint32_t dsp_effect = (TELEMETRY_DSP_EFFECT_DYNAMIC_RANGE_COMPRESSION +
                              TELEMETRY_DSP_EFFECT_EQUALIZER + TELEMETRY_DSP_EFFECT_BEAMFORMING
                              + TELEMETRY_DSP_EFFECT_NOISE_SUPPRESSION + TELEMETRY_DSP_EFFECT_ACOUSTIC_ECHO_CANCELLATION);
static bool mute_lock = false;
static T_APP_ASP_DEVICE_TELEMETRY_BATTERY_LEVEL_TYPE battery_level =
    T_APP_ASP_DEVICE_TELEMETRY_BATTERY_LEVEL_OFF;
static bool device_ready = false;
static uint8_t button_info[3] = {0x11, 0x00, 0x00};
static uint32_t local_conference_count = 0;

/*asp manager data*/
static uint8_t app_asp_timer_id = 0;
static T_APP_ASP_LINK_INFO app_asp_link_data[TEAMS_LINK_NUM];

bool app_asp_device_send_pkt_by_ble(uint8_t conn_id, uint8_t *p_data, uint16_t data_len)
{
    if (*p_data == CLIENT_TO_DEVICE_REPORT_ID)
    {
#if CONFIG_REALTEK_BT_GATT_CLIENT_SUPPORT
        return asp_send_data(le_get_conn_handle(conn_id), ASP_DATA_9A, p_data, data_len);
#else
        return asps_send_mesaage_notify(conn_id, p_data, data_len);
#endif
    }
    else if (*p_data == DEVICE_TO_CLIENT_REPORT_ID)
    {
#if CONFIG_REALTEK_BT_GATT_CLIENT_SUPPORT
        return asp_send_data(le_get_conn_handle(conn_id), ASP_DATA_9B, p_data, data_len);
#else
        return asps_send_attn_notify(conn_id, p_data, data_len);
#endif
    }
    return false;
}

T_APP_ASP_LINK_INFO *app_asp_device_find_link_by_addr(uint8_t *bd_addr)
{
    if (!bd_addr)
    {
        return NULL;
    }


    for (uint8_t i = 0; i < TEAMS_LINK_NUM; i++)
    {
        APP_PRINT_INFO3("app_asp_device_find_link_by_addr: index %d, bd_addr %s, bd_addr1 %s", i,
                        TRACE_BDADDR(bd_addr), TRACE_BDADDR(app_asp_link_data[i].bd_addr));
        if (app_asp_link_data[i].used && !memcmp(app_asp_link_data[i].bd_addr, bd_addr, 6))
        {
            return &app_asp_link_data[i];
        }
    }
    return NULL;
}

T_APP_ASP_LINK_INFO *app_asp_device_find_link_by_index(uint8_t index)
{
    uint8_t *bd_addr = NULL;

    if (index == teams_asp_usb_idx)
    {
        bd_addr = teams_asp_usb_addr;
    }
    else if (index < (TEAMS_LINK_NUM - 1))
    {
        bd_addr = app_db.br_link[index].bd_addr;
    }
    else
    {
        return NULL;
    }

    return app_asp_device_find_link_by_addr(bd_addr);
}

uint8_t app_asp_device_get_index_by_addr(uint8_t *bd_addr)
{
    T_APP_BR_LINK *app_br_link = NULL;
    if (!bd_addr)
    {
        return TEAMS_LINK_NUM;
    }

    if (memcmp(bd_addr, teams_asp_usb_addr, 6) == 0)
    {
        return teams_asp_usb_idx;
    }
    else
    {
        app_br_link = app_link_find_br_link(bd_addr);
        if (app_br_link)
        {
            return app_br_link->id;
        }
        else
        {
            return TEAMS_LINK_NUM;
        }
    }
}

T_APP_ASP_LINK_INFO *app_asp_device_find_active_link(void)
{
    for (uint8_t i = 0; i < TEAMS_LINK_NUM; i++)
    {
        APP_PRINT_INFO3("app_asp_device_find_link_by_addr: index %d, bd_addr %s, link state %d", i,
                        TRACE_BDADDR(app_asp_link_data[i].bd_addr), app_asp_link_data[i].link_state);
        if (app_asp_link_data[i].used &&
            (app_asp_link_data[i].link_state == ASP_DEVICE_LINK_CONNECTED_ACTIVE))
        {
            return &app_asp_link_data[i];
        }
    }
    return NULL;
}

uint8_t app_asp_device_find_active_link_num(void)
{
    uint8_t num = 0;
    for (uint8_t i = 0; i < TEAMS_LINK_NUM; i++)
    {
        if (app_asp_link_data[i].used &&
            (app_asp_link_data[i].link_state == ASP_DEVICE_LINK_CONNECTED_ACTIVE))
        {
            num++;
        }
    }
    return num;
}

void app_asp_device_alloc_link(uint8_t *bd_addr)
{
    uint8_t index_free = TEAMS_LINK_NUM;
    for (uint8_t i = 0; i < TEAMS_LINK_NUM; i++)
    {
        if (app_asp_link_data[i].used && !memcmp(app_asp_link_data[i].bd_addr, bd_addr, 6))
        {
            APP_PRINT_INFO2("app_asp_device_alloc_link: link %d with this bd_addr %s is already exists", i,
                            TRACE_BDADDR(bd_addr));
            return;
        }
        else if ((app_asp_link_data[i].used == false) && (index_free == TEAMS_LINK_NUM))
        {
            index_free = i;
        }
    }

    if (index_free != TEAMS_LINK_NUM)
    {
        APP_PRINT_INFO2("app_asp_device_alloc_link: index %d, bd_addr %s", index_free,
                        TRACE_BDADDR(bd_addr));
        app_asp_link_data[index_free].used = true;
        memcpy(app_asp_link_data[index_free].bd_addr, bd_addr, 6);
        app_asp_link_data[index_free].pending_flag = true;
        app_asp_link_data[index_free].link_state = ASP_DEVICE_LINK_CONNECTED_IDLE;
        app_asp_link_data[index_free].radio_quality = T_APP_ASP_DEVICE_TELEMETRY_RADIO_LINK_QUALITY_OFF;
        app_start_timer(&app_asp_link_data[index_free].app_asp_timer_handle_pending, "app_asp_pending",
                        app_asp_timer_id,
                        APP_ASP_LINK_PENDING, index_free, false, APP_ASP_PENDING_TIMEOUT);
    }
    return;
}

void app_asp_device_alloc_link_by_idx(uint8_t *bd_addr, uint8_t index)
{
    APP_PRINT_INFO2("app_asp_device_alloc_link_by_idx: index %d, bd_addr %s", index,
                    TRACE_BDADDR(bd_addr));
    app_asp_link_data[index].used = true;
    app_asp_link_data[index].idx = index;
    memcpy(app_asp_link_data[index].bd_addr, bd_addr, 6);
    app_asp_link_data[index].pending_flag = true;
    app_asp_link_data[index].radio_quality = T_APP_ASP_DEVICE_TELEMETRY_RADIO_LINK_QUALITY_OFF;
}

void app_asp_device_delete_link(uint8_t *bd_addr)
{
    for (uint8_t i = 0; i < TEAMS_LINK_NUM; i++)
    {
        if (app_asp_link_data[i].used && !memcmp(app_asp_link_data[i].bd_addr, bd_addr, 6))
        {
            APP_PRINT_INFO2("app_asp_device_delete_link: index %d, bd_addr %s", i,
                            TRACE_BDADDR(app_asp_link_data[i].bd_addr));
            app_stop_timer(&app_asp_link_data[i].app_asp_timer_handle_pending);
            memset(&app_asp_link_data[i], 0, sizeof(T_APP_ASP_LINK_INFO));
            break;
        }
    }
    return;
}

void app_asp_device_delete_all_link(void)
{
    APP_PRINT_INFO0("app_asp_device_delete_all_link");
    for (uint8_t i = 0; i < TEAMS_LINK_NUM; i++)
    {
        if (app_asp_link_data[i].used)
        {
            app_stop_timer(&app_asp_link_data[i].app_asp_timer_handle_pending);
            memset(&app_asp_link_data[i], 0, sizeof(T_APP_ASP_LINK_INFO));
        }
    }
}

void app_asp_device_set_active_index(uint8_t *bd_addr)
{
    uint8_t temp_addr[6] = {0};
    uint8_t *asp_device_active_link_addr = asp_device_api_get_active_link_addr();
    if (bd_addr && (memcmp(bd_addr, temp_addr, 6)) &&
        (!asp_device_active_link_addr || (asp_device_active_link_addr &&
                                          memcmp(asp_device_active_link_addr, bd_addr, 6))))
    {
        teams_extend_led_control_when_teams_launch();
    }

    APP_PRINT_INFO1("app_asp_device_set_active_index: bd_addr %s", TRACE_BDADDR(bd_addr));
    asp_device_api_set_active_link_addr(bd_addr);
}

//this api only used in bt link
bool app_asp_device_check_device_is_hold_state(uint8_t index)
{
    if (index >= MAX_BR_LINK_NUM)
    {
        return false;
    }

#if F_APP_CALL_HOLD_SUPPORT
    if (app_db.br_link[index].used &&
        (app_db.br_link[index].call_status == APP_CALL_ACTIVE_WITH_CALL_HELD ||
         app_db.br_link[index].call_status == APP_CALL_HELD))
#else
    if (app_db.br_link[index].used &&
        (app_db.br_link[index].call_status == APP_CALL_ACTIVE_WITH_CALL_HELD))
#endif
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool app_asp_device_get_select_hold_active_index(uint8_t *return_index)
{
    uint8_t index = 0;
    uint8_t hold_num = 0;
    uint8_t hold_index = TEAMS_LINK_NUM;
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
    bool app_status = false;
    uint32_t active_timestamp = 0;
    T_APP_TEAMS_MULTILINK_APP_HIGH_PRIORITY_ARRAY_DATA *p_high_priority_array_data = NULL;
    app_status = app_teams_multilink_check_app_active();
    hold_num = app_teams_multilink_get_hold_link_num();

    APP_PRINT_TRACE2("app_asp_device_get_select_hold_index: hold num %d, app status %d",
                     hold_num, app_status);

    if ((app_status == true) || !hold_num)
    {
        return false;
    }

    for (uint8_t i = 0; i < TEAMS_LINK_NUM; i++)
    {
        if (app_asp_link_data[i].used)
        {
            index = app_asp_device_get_index_by_addr(app_asp_link_data[i].bd_addr);
            p_high_priority_array_data = app_teams_multilink_find_app_high_priority_array_data_by_link_idx(
                                             index);
            if (p_high_priority_array_data && p_high_priority_array_data->hold_status &&
                (app_asp_link_data[i].link_state == ASP_DEVICE_LINK_CONNECTED_ACTIVE) &&
                (app_asp_link_data[i].asp_link_active_timestamp > active_timestamp))
            {
                hold_index = index;
                active_timestamp = app_asp_link_data[i].asp_link_active_timestamp;
            }
        }
    }
#else
    T_APP_CALL_STATUS voice_status;
    voice_status = app_hfp_get_call_status();
    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_asp_device_check_device_is_hold_state(i))
        {
            hold_num++;
        }
    }

    APP_PRINT_TRACE2("app_asp_device_get_select_hold_index: hold num %d, voice status %d",
                     hold_num, voice_status);

    if ((voice_status != APP_CALL_IDLE) || !hold_num)
    {
        return false;
    }

    index = app_a2dp_get_active_idx();

    if (app_asp_device_check_device_is_hold_state(index))
    {
        hold_index = index;
    }
    else
    {
        for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (app_db.br_link[i].used == true && (i != index) && app_asp_device_check_device_is_hold_state(i))
            {
                hold_index = i;
                break;
            }
        }
    }
#endif
    *return_index = hold_index;
    if (hold_index == TEAMS_LINK_NUM)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool app_asp_device_get_select_active_index(uint8_t *return_index)
{
    uint32_t active_timestamp = 0;
    uint8_t index = 0;
    uint8_t active_index = TEAMS_LINK_NUM;

    for (uint8_t i = 0; i < TEAMS_LINK_NUM; i++)
    {
        if (app_asp_link_data[i].used)
        {
            index = app_asp_device_get_index_by_addr(app_asp_link_data[i].bd_addr);
            if ((app_asp_link_data[i].link_state == ASP_DEVICE_LINK_CONNECTED_ACTIVE) &&
                (app_asp_link_data[i].asp_link_active_timestamp > active_timestamp))
            {
                active_timestamp = app_asp_link_data[i].asp_link_active_timestamp;
                active_index = index;
            }
        }
    }

    *return_index = active_index;
    if (active_index == TEAMS_LINK_NUM)
    {
        return false;
    }
    else
    {
        return true;
    }
}

uint8_t *app_asp_device_get_active_index(void)
{
    uint8_t index = TEAMS_LINK_NUM;
    uint8_t *active_asp_link_addr = asp_device_api_get_active_link_addr();
    uint8_t link_num = app_asp_device_find_active_link_num();
    T_APP_ASP_LINK_INFO *app_asp_link = NULL;
    uint8_t multilink_active_idx = 0;
    uint8_t *multilink_active_idx_addr = NULL;
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
    multilink_active_idx = app_teams_multilink_get_active_index();
    multilink_active_idx_addr = app_teams_multilink_get_bd_addr_from_idx(multilink_active_idx);
#else
    T_APP_CALL_STATUS call_status = app_hfp_get_call_status();
    if (call_status)
    {
        multilink_active_idx = app_hfp_get_active_idx();
    }
    else
    {
        multilink_active_idx = app_a2dp_get_active_idx();
    }
    multilink_active_idx_addr = app_db.br_link[multilink_active_idx].bd_addr;
#endif

    APP_PRINT_TRACE3("app_asp_device_get_active_index: link num %d, multilink active addr %s, asp active addr %s",
                     link_num, TRACE_BDADDR(multilink_active_idx_addr), TRACE_BDADDR(active_asp_link_addr));

    if (!link_num)
    {
        return NULL;
    }
    else if (link_num == 1)
    {
        app_asp_link = app_asp_device_find_active_link();
        return app_asp_link->bd_addr;
    }
    else
    {
        /* select hold active device*/
        if (app_asp_device_get_select_hold_active_index(&index))
        {

        }
        /* select active device*/
        else if (app_asp_device_get_select_active_index(&index))
        {

        }

        /*Find active device*/
        if (index != TEAMS_LINK_NUM)
        {
            app_asp_link = app_asp_device_find_link_by_index(index);
            if (app_asp_link)
            {
                return app_asp_link->bd_addr;
            }
            else
            {
                APP_PRINT_ERROR0("app_asp_device_get_active_index: no possible scenario");
                return NULL;
            }
        }
        else
        {
            /*if no active device, follow audio active device which has authed or keep previous asp active device*/
            app_asp_link = app_asp_device_find_link_by_addr(multilink_active_idx_addr);
            if (app_asp_link && (app_asp_link->link_state == ASP_DEVICE_LINK_CONNECTED_ACTIVE))
            {
                return multilink_active_idx_addr;
            }
            else
            {
                return active_asp_link_addr;
            }
        }
    }
}

uint32_t app_asp_device_convert_elementid_to_mask(T_ASP_DEVICE_TELEMETRY_TYPE element_id)
{
    if (!element_id)
    {
        return 0;
    }
    return (1 << (element_id - 1));
}

void app_asp_device_send_notify_info(T_ASP_DEVICE_TELEMETRY_TYPE element_type)
{
    uint8_t *bd_addr = NULL;

    APP_PRINT_INFO1("app_asp_device_send_notify_info: element type %d", element_type);

    bd_addr = asp_device_api_get_active_link_addr();
    if (bd_addr)
    {
        app_asp_device_send_teams_telemetry_update_msg(bd_addr,
                                                       app_asp_device_convert_elementid_to_mask(element_type));
    }
    else
    {
        APP_PRINT_WARN0("app_asp_device_send_notify_info: no active asp link");
    }
}

void app_asp_device_handle_battery(uint8_t battery_level_from_charger, bool is_first_power_on)
{
    APP_PRINT_INFO2("app_asp_device_handle_battery: battery level %d, is_first_power_on %d",
                    battery_level_from_charger, is_first_power_on);
    T_APP_ASP_DEVICE_TELEMETRY_BATTERY_LEVEL_TYPE battery_level_temp =
        T_APP_ASP_DEVICE_TELEMETRY_BATTERY_LEVEL_OFF;
    uint8_t *active_asp_link_addr = NULL;
    bool notify_flag = false;
    if (!battery_level_from_charger)
    {
        battery_level_temp = T_APP_ASP_DEVICE_TELEMETRY_BATTERY_LEVEL_OFF;
    }
    else if (battery_level_from_charger <= 10)
    {
        battery_level_temp = T_APP_ASP_DEVICE_TELEMETRY_BATTERY_LEVEL_LOW;
    }
    else if (battery_level_from_charger <= 70)
    {
        battery_level_temp = T_APP_ASP_DEVICE_TELEMETRY_BATTERY_LEVEL_MED;
    }
    else
    {
        battery_level_temp = T_APP_ASP_DEVICE_TELEMETRY_BATTERY_LEVEL_HI;
    }

    if (battery_level_temp != battery_level)
    {
        notify_flag = true;
    }

    battery_level = battery_level_temp;

    if (notify_flag && !is_first_power_on && (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY))
    {
        active_asp_link_addr = asp_device_api_get_active_link_addr();
        if (memcmp(active_asp_link_addr, teams_asp_usb_addr, 6))
        {
            app_asp_device_send_notify_info(T_ASP_DEVICE_TELEMETRY_BATTERY_LEVEL);
        }
    }
}

void app_asp_device_handle_per(uint8_t *bd_addr, uint8_t per)
{
    APP_PRINT_INFO2("app_asp_device_handle_per: bd_addr %s, per %d",
                    TRACE_BDADDR(bd_addr), per);
    bool notify_flag = false;
    T_APP_ASP_DEVICE_TELEMETRY_RADIO_LINK_QUALITY_TYPE radio_quality_type =
        T_APP_ASP_DEVICE_TELEMETRY_RADIO_LINK_QUALITY_OFF;
    T_APP_ASP_LINK_INFO *p_app_asp_link = app_asp_device_find_link_by_addr(bd_addr);
    if (p_app_asp_link)
    {
        if (per < 1)
        {
            radio_quality_type = T_APP_ASP_DEVICE_TELEMETRY_RADIO_LINK_QUALITY_HI;
        }
        else
        {
            radio_quality_type = T_APP_ASP_DEVICE_TELEMETRY_RADIO_LINK_QUALITY_LOW;
        }

        if ((p_app_asp_link->radio_quality != radio_quality_type) &&
            (p_app_asp_link->radio_quality != T_APP_ASP_DEVICE_TELEMETRY_RADIO_LINK_QUALITY_OFF))
        {
            notify_flag = true;
        }

        p_app_asp_link->radio_quality = radio_quality_type;
        if (notify_flag && (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY))
        {
            if (asp_device_api_get_link_state(bd_addr) == ASP_DEVICE_LINK_CONNECTED_ACTIVE)
            {
                app_asp_device_send_teams_telemetry_update_msg(bd_addr,
                                                               app_asp_device_convert_elementid_to_mask(T_ASP_DEVICE_TELEMETRY_RADIO_LINK_QUALITY));
            }
        }
    }
}

void app_asp_device_handle_device_state(bool device_state)
{
    APP_PRINT_INFO1("app_asp_device_handle_device_state: device_state %d", device_state);
    bool notify_flag = false;
    if (device_ready != device_state)
    {
        notify_flag = true;
    }
    device_ready = device_state;

    if (notify_flag && (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY))
    {
        app_asp_device_send_notify_info(T_ASP_DEVICE_TELEMETRY_DEVICE_READY);
    }
}

void app_asp_device_handle_mute_lock(bool is_mute_lock)
{
    APP_PRINT_INFO1("app_asp_device_handle_mute_lock: mute lock %d", is_mute_lock);
    bool notify_flag = false;
    if (mute_lock != is_mute_lock)
    {
        notify_flag = true;
    }
    mute_lock = is_mute_lock;

    if (notify_flag && (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY))
    {
        app_asp_device_send_notify_info(T_ASP_DEVICE_TELEMETRY_HARDMUTE_LOCK);
    }
}

void app_asp_device_handle_conference_change(uint8_t conference_num)
{
    APP_PRINT_INFO1("app_asp_device_handle_conference_change: conference_num %d", conference_num);
    bool notify_flag = false;

    if (local_conference_count != conference_num)
    {
        notify_flag = true;
    }
    local_conference_count = conference_num;

    if (notify_flag && (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY))
    {
        app_asp_device_send_notify_info(T_ASP_DEVICE_TELEMETRY_LOCAL_CONFERENCE_COUNT);
    }
}

void app_asp_device_send_hook_button_status(bool is_off_hook)
{
    APP_PRINT_INFO1("app_asp_device_send_hook_button_status: is_off_hook %d", is_off_hook);
    button_info[1] = 0x20;
    button_info[2] = is_off_hook;

    app_asp_device_send_notify_info(T_ASP_DEVICE_TELEMETRY_BUTTON_PRESS_INFO);
}

void app_asp_device_send_mute_button_status(bool is_mute)
{
    APP_PRINT_INFO1("app_asp_device_send_mute_button_status: is_mute %d", is_mute);
    button_info[1] = 0x2f;
    button_info[2] = is_mute;

    app_asp_device_send_notify_info(T_ASP_DEVICE_TELEMETRY_BUTTON_PRESS_INFO);
}

bool app_asp_device_check_is_multilink_active_addr(uint8_t *bd_addr)
{
    uint8_t active_stream_bd_addr[6] = {0};
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
    uint8_t multilink_active_index = app_teams_multilink_get_active_index();
#else
    uint8_t multilink_active_index;
    if ((app_hfp_get_call_status() != APP_CALL_IDLE) ||
        (app_link_get_sco_conn_num() != 0))
    {
        multilink_active_index = app_hfp_get_active_idx();
    }
    else
    {
        multilink_active_index = app_a2dp_get_active_idx();
    }
#endif

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
    if (multilink_usb_idx == multilink_active_index)
    {
        memcpy(active_stream_bd_addr, multilink_usb_addr, 6);
    }
    else
#endif
    {
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
        if (multilink_active_index != APP_TEAMS_MAX_LINK_NUM)
#endif
        {
            memcpy(active_stream_bd_addr, app_db.br_link[multilink_active_index].bd_addr, 6);
        }
    }

    APP_PRINT_INFO3("app_asp_device_check_is_multilink_active_addr: multilink_active_index %d, active_stream_bd_addr %s, bd_addr %s",
                    multilink_active_index, TRACE_BDADDR(active_stream_bd_addr), TRACE_BDADDR(bd_addr));

    if (!memcmp(active_stream_bd_addr, bd_addr, 6))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void app_asp_device_handle_notification_cback(uint8_t *bd_addr,
                                              T_ASP_DEVICE_CTOD_NOTIFICATION_TYPE event_type)
{
    if (event_type == T_ASP_DEVICE_CTOD_NOTIFICATION_CLEAR_ALL)
    {
        /* handle clear all notification */
        if (app_asp_device_check_is_multilink_active_addr(bd_addr))
        {
            teams_extend_led_control_when_clear_notification();
            app_teams_set_notify_status(2);
        }
    }
    else if (event_type == T_ASP_DEVICE_CTOD_NOTIFICATION_MISSING_MEETING)
    {
        /* handle missing meeting notification */
        app_teams_led_handle_notification(bd_addr);
    }
    else if (event_type == T_ASP_DEVICE_CTOD_NOTIFICATION_UPCOMING_MEETING)
    {
        /* handle upcoming meeting notification */
        app_teams_led_handle_notification(bd_addr);
    }
    else if (event_type == T_ASP_DEVICE_CTOD_NOTIFICATION_UNCHECKED_VOICE_MAIL)
    {
        /* handle unchecked voice mail notification */
        app_teams_led_handle_notification(bd_addr);
    }

    APP_PRINT_INFO2("app_asp_device_handle_notification_cback: bd_addr %s, event type %d",
                    TRACE_BDADDR(bd_addr), event_type);
}

void app_asp_device_handle_call_event_cback(uint8_t *bd_addr,
                                            T_ASP_DEVICE_CTOD_CALL_TYPE event_type, uint16_t length,
                                            uint8_t *data)
{
    APP_PRINT_INFO2("app_asp_device_handle_call_event_cback: bd_addr %s, event type %d",
                    TRACE_BDADDR(bd_addr), event_type);
}

/*data and data_len can not be local var, and we suggest use global var due to lower stack would not free data pointer */
void app_asp_device_handle_telemetry_cback(uint8_t *bd_addr, T_ASP_DEVICE_TELEMETRY_TYPE element_id,
                                           uint8_t *data_len, uint8_t **data)
{
    uint8_t *temp = NULL;
    T_APP_ASP_LINK_INFO *p_app_asp_link = NULL;


    switch (element_id)
    {
    case T_ASP_DEVICE_TELEMETRY_ENDPOINT_FIRMWARE_VERSION:
        memcpy(telemetry_data, current_firmware, sizeof(current_firmware));
        *data_len = sizeof(current_firmware) - 1 ;
        *data = telemetry_data;
        break;

    case T_ASP_DEVICE_TELEMETRY_DONGLE_FIRMWARE_VERSION:
        break;

    case T_ASP_DEVICE_TELEMETRY_DON_ANSWER_SETTING:

        break;

    case T_ASP_DEVICE_TELEMETRY_ENDPOINT_DEVICE_MODEL_ID:
        memcpy(telemetry_data, app_cfg_nv.device_name_legacy,
               strlen((char *)app_cfg_nv.device_name_legacy));
        *data_len = strlen((char *)app_cfg_nv.device_name_legacy);
        *data = telemetry_data;
        break;

    case T_ASP_DEVICE_TELEMETRY_ENDPOINT_DEVICE_SERIAL_NUM:
#if F_APP_TEAMS_CUSTOMIZED_CMD_SUPPORT
        if (app_teams_cmd_get_device_serial_number((const void **)data, (void *)data_len))
        {
            APP_PRINT_INFO2("app_asp_device_handle_telemetry_cback: serial num, length %d, data %b",
                            *data_len, TRACE_BINARY(*data_len, *data));
        }
        else
        {
            APP_PRINT_INFO1("app_asp_device_handle_telemetry_cback: bd_addr %s, get serial number failed",
                            TRACE_BDADDR(bd_addr));
        }
#else
        *data_len = sizeof(serial_num);
        *data = serial_num;
#endif
        break;

    case T_ASP_DEVICE_TELEMETRY_DONGLE_DEVICE_SERIAL_NUM:

        break;

    case T_ASP_DEVICE_TELEMETRY_USER_MODIFIED_SIDETONE_LEVEL:
        memcpy(telemetry_data, sidetone_level, sizeof(sidetone_level));
        *data_len = sizeof(sidetone_level);
        *data = telemetry_data;
        break;

    case T_ASP_DEVICE_TELEMETRY_AUDIO_CODEC_USED:
        memcpy(telemetry_data, &audio_codec_type, sizeof(audio_codec_type));
        *data_len = sizeof(audio_codec_type);
        *data = telemetry_data;
        break;

    case T_ASP_DEVICE_TELEMETRY_DSP_EFFECTS:
        temp = telemetry_data;
        BE_UINT32_TO_STREAM(temp, dsp_effect);
        *data_len = sizeof(dsp_effect);
        *data = telemetry_data;
        break;

    case T_ASP_DEVICE_TELEMETRY_HARDMUTE_LOCK:
        memcpy(telemetry_data, &mute_lock, sizeof(mute_lock));
        *data_len = sizeof(mute_lock);
        *data = telemetry_data;
        break;

    case T_ASP_DEVICE_TELEMETRY_BATTERY_LEVEL:
        memcpy(telemetry_data, &battery_level, sizeof(battery_level));
        *data_len = sizeof(battery_level);
        *data = telemetry_data;
        break;

    case T_ASP_DEVICE_TELEMETRY_DEVICE_READY:
        memcpy(telemetry_data, &device_ready, sizeof(device_ready));
        *data_len = sizeof(device_ready);
        *data = telemetry_data;
        break;

    case T_ASP_DEVICE_TELEMETRY_RADIO_LINK_QUALITY:
        p_app_asp_link = app_asp_device_find_link_by_addr(bd_addr);
        if (p_app_asp_link)
        {
            APP_PRINT_INFO2("app_asp_device_handle_telemetry_cback: bd_addr %s, radio level %d",
                            TRACE_BDADDR(bd_addr), p_app_asp_link->radio_quality);

            memcpy(telemetry_data, &p_app_asp_link->radio_quality, sizeof(p_app_asp_link->radio_quality));
            *data_len = sizeof(p_app_asp_link->radio_quality);
            *data = telemetry_data;
        }
        break;

    case T_ASP_DEVICE_TELEMETRY_BUTTON_PRESS_INFO:
        memcpy(telemetry_data, button_info, sizeof(button_info));
        *data_len = sizeof(button_info);
        *data = telemetry_data;
        break;

    case T_ASP_DEVICE_TELEMETRY_LOCAL_CONFERENCE_COUNT:
        memcpy(telemetry_data, &local_conference_count, sizeof(local_conference_count));
        *data_len = sizeof(local_conference_count);
        *data = telemetry_data;
        break;
    default:
        break;
    }

    APP_PRINT_INFO2("app_asp_device_handle_telemetry_cback: bd_addr %s, element_id %d",
                    TRACE_BDADDR(bd_addr), element_id);
}

void app_asp_device_handle_asp_event(uint8_t *bd_addr, T_APP_ASP_DEVICE_ASP_EVENT_PYTE event_type)
{
    T_APP_ASP_LINK_INFO *app_asp_link = app_asp_device_find_link_by_addr(bd_addr);
    uint8_t *active_asp_device_addr = asp_device_api_get_active_link_addr();
    uint8_t *addr = NULL;

    if (bd_addr)
    {
        APP_PRINT_TRACE2("app_asp_device_handle_asp_event: bd_addr %s, event %d", TRACE_BDADDR(bd_addr),
                         event_type);
    }
    else
    {
        APP_PRINT_TRACE1("app_asp_device_handle_asp_event: event %d", event_type);
    }


    //record auth or active timestamp
    if (app_asp_link)
    {
        if (event_type == T_APP_ASP_DEVICE_ASP_AUTH)
        {
            app_asp_link->asp_link_auth_timestamp = os_sys_time_get();
            app_asp_link->link_state = ASP_DEVICE_LINK_CONNECTED_ACTIVE;
        }
        else if (event_type == T_APP_ASP_DEVICE_ASP_DEAUTH)
        {
            app_asp_link->asp_link_auth_timestamp = 0;
            app_asp_link->link_state = ASP_DEVICE_LINK_CONNECTED_IDLE;
        }
        else if (event_type == T_APP_ASP_DEVICE_AUDIO_ACTIVE_IDX_CHANGED)
        {
            app_asp_link->asp_link_active_timestamp = os_sys_time_get();
        }
        else
        {
            //do nothing
        }
    }

    //recheck active asp idx
    addr = app_asp_device_get_active_index();
    if (addr && active_asp_device_addr)
    {
        APP_PRINT_TRACE2("app_asp_device_handle_asp_event: addr %s, active_asp_device_addr %s",
                         TRACE_BDADDR(addr), TRACE_BDADDR(active_asp_device_addr));
        if (memcmp(active_asp_device_addr, addr, 6))
        {
            app_asp_device_set_active_index(addr);
        }
    }
    else if (addr || active_asp_device_addr)
    {
        if (active_asp_device_addr)
        {
            uint8_t temp_addr[6] = {0};
            app_asp_device_set_active_index(temp_addr);
        }
        else
        {
            app_asp_device_set_active_index(addr);
        }
    }
    else
    {
        APP_PRINT_TRACE0("app_asp_device_handle_asp_event: no used device");
        //do nothing
    }
}

void app_asp_device_handle_asp_info_cback(T_ASP_INFO_MSG *msg_data)
{
    uint8_t report_id = 0;
    uint8_t msg_id    = 0;
    uint8_t metadata_flag = 0;

    uint16_t primary_data_len = 0;
    uint8_t *primary_data = NULL;
    uint16_t metadata_data_len = 0;
    uint8_t *metadata_data = NULL;
    uint8_t *data_temp = NULL;
    uint16_t data_len_temp = 0;
    uint8_t *data_head = NULL;
    uint16_t data_len = 0;

    switch (msg_data->asp_info_type)
    {
    case T_ASP_DEVICE_ASP_INFO_STATE:
        /* 1 : connected; 0 : disconnected */
        APP_PRINT_INFO2("app_asp_device_handle_asp_info_cback: bd_addr %s, asp link state %d",
                        TRACE_BDADDR(msg_data->asp_info_data.asp_link_state_data.bd_addr),
                        msg_data->asp_info_data.asp_link_state_data.asp_link_state);

        if (msg_data->asp_info_data.asp_link_state_data.asp_link_state)
        {
            bt_link_per_report(msg_data->asp_info_data.asp_link_state_data.bd_addr, true,
                               APP_ASP_PER_REPORT_PERIOD);

            app_asp_device_handle_asp_event(msg_data->asp_info_data.asp_link_state_data.bd_addr,
                                            T_APP_ASP_DEVICE_ASP_AUTH);
        }
        else
        {
            bt_link_per_report(msg_data->asp_info_data.asp_link_state_data.bd_addr, false,
                               APP_ASP_PER_REPORT_PERIOD);

            app_asp_device_handle_asp_event(msg_data->asp_info_data.asp_link_state_data.bd_addr,
                                            T_APP_ASP_DEVICE_ASP_DEAUTH);

            if (!asp_device_api_get_active_link_addr())
            {
                app_teams_set_launch_status(2);
                teams_extend_led_stop_timer();
                //Add teams led off
            }
        }
        break;
    case T_ASP_DEVICE_ASP_INFO_PKT_DATA:
        /* data buffer will free after this callback finish*/
        data_temp = msg_data->asp_info_data.asp_pkt_data.data;
        data_len_temp = msg_data->asp_info_data.asp_pkt_data.data_len;
        report_id = *data_temp++;
        msg_id = ((*data_temp & 0xf0) >> 4);
        metadata_flag = (*data_temp++ & 0x0f);

        BE_STREAM_TO_UINT16(primary_data_len, data_temp);
        if (primary_data_len)
        {
            primary_data = data_temp;
        }
        data_temp += primary_data_len;

        if (metadata_flag && (data_len_temp > (primary_data_len + 4)))
        {
            BE_STREAM_TO_UINT16(metadata_data_len, data_temp);
            if (metadata_data_len)
            {
                metadata_data = data_temp;
            }
        }

        APP_PRINT_INFO7("app_asp_device_handle_asp_info_cback: report id %d, msg_id %d, metadata_flag %d, primary_data_len %d, primary_data %p, metadata_data_len %d, metadata_data %p",
                        report_id, msg_id, metadata_flag, primary_data_len, primary_data, metadata_data_len, metadata_data);

        switch (msg_id)
        {
        case ASP_DEVICE_REPORT_CTOD_MESSAGE_ID_CALL_EVENT:
            data_len = sizeof(report_id) + sizeof(msg_id) + sizeof(primary_data_len) + primary_data_len;
            data_head = malloc(data_len);
            data_temp = data_head;
            *data_temp++ = report_id;
            *data_temp++ = (msg_id << 4);
            BE_UINT16_TO_STREAM(data_temp, primary_data_len);
            memcpy(data_temp, primary_data, primary_data_len);
            asp_device_send_report_from_app(msg_data->asp_info_data.asp_pkt_data.bd_addr, data_len, data_head);
            free(data_head);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

/*bd_addr get from api asp_device_api_get_active_link_addr*/
bool app_asp_device_send_teams_button_invoked_msg(uint8_t *br_addr,
                                                  T_ASP_DEVICE_SOURCE_ID_TYPE source_id)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        APP_PRINT_INFO0("app_asp_device_send_teams_button_invoked_msg: bud role is sec, not send pkt");
        return false;
    }
    return asp_device_api_send_teams_button_invoked_msg(br_addr, source_id);
}

/*only dt categories can send*/
/*bd_addr get from api asp_device_api_get_active_link_addr*/
bool app_asp_device_send_teams_telemetry_update_msg(uint8_t *br_addr,
                                                    uint32_t mask)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        APP_PRINT_INFO0("app_asp_device_send_teams_telemetry_update_msg: bud role is sec, not send pkt");
        return false;
    }

    T_APP_ASP_LINK_INFO *p_app_asp_link = app_asp_device_find_link_by_addr(br_addr);
    if (p_app_asp_link)
    {
        APP_PRINT_INFO2("app_asp_device_send_teams_telemetry_update_msg: bd_addr %s, pending_flag 0x%x",
                        TRACE_BDADDR(br_addr), p_app_asp_link->pending_flag);
        /* device can send dt data after 10s from link active, and if send data using multiple data format in 10s, it would error, only support single data format in 10s*/
        if (p_app_asp_link->pending_flag &&
            (mask != app_asp_device_convert_elementid_to_mask(T_ASP_DEVICE_TELEMETRY_BUTTON_PRESS_INFO)))
        {
            p_app_asp_link->pending_dt_mask |= mask;
            return true;
        }
        else
        {
            return asp_device_api_send_telemetry_msg(br_addr, mask);
        }
    }
    else
    {
        APP_PRINT_INFO1("app_asp_device_send_teams_telemetry_update_msg: asp link not found, bd_addr %s",
                        TRACE_BDADDR(br_addr));
        return false;
    }
}

static void app_asp_device_timer_callback(uint8_t timer_id, uint16_t timer_chann)
{
    APP_PRINT_TRACE2("app_asp_device_timer_callback: timer_id 0x%02x, timer_chann %d",
                     timer_id, timer_chann);

    switch (timer_id)
    {
    case APP_ASP_LINK_PENDING:
        {
            app_asp_link_data[timer_chann].pending_flag = false;

            app_asp_link_data[timer_chann].pending_dt_mask |= app_asp_device_convert_elementid_to_mask(
                                                                  T_ASP_DEVICE_TELEMETRY_DEVICE_READY);
            APP_PRINT_TRACE1("app_asp_device_timer_callback: pending dt mask 0x%x",
                             app_asp_link_data[timer_chann].pending_dt_mask);

            app_asp_device_send_teams_telemetry_update_msg(app_asp_link_data[timer_chann].bd_addr,
                                                           app_asp_link_data[timer_chann].pending_dt_mask);
            app_asp_link_data[timer_chann].pending_dt_mask = 0;

        }
        break;

    default:
        break;
    }
}

static void app_asp_device_event_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BT_EVENT_LINK_PER_INFO:
        {
            APP_PRINT_TRACE2("app_asp_device_event_cback: err pkt %ul, total pkt %ul",
                             param->link_per_info.err_pkts, param->link_per_info.total_pkts);
            app_asp_device_handle_per(param->link_per_info.bd_addr,
                                      ((param->link_per_info.err_pkts * 100) / param->link_per_info.total_pkts));
        }
        break;

    case BT_EVENT_LINK_RSSI_INFO:
        {

        }
        break;

    case BT_EVENT_HFP_CALL_STATUS:
        {
            if (param->hfp_call_status.curr_status > APP_VOICE_ACTIVATION_ONGOING)
            {
                app_asp_device_handle_conference_change(1);
            }
            else
            {
                app_asp_device_handle_conference_change(0);
            }
        }
        break;
    }
}

bool app_teams_no_launch(void)
{
    return (teams_launch == 0);
}

bool app_teams_is_launch(void)
{
    return (teams_launch == 1);
}

bool app_teams_is_close(void)
{
    return (teams_launch == 2);
}

bool app_teams_notify(void)
{
    return (teams_notify == 1);
}

bool app_teams_clear_notification(void)
{
    return (teams_notify == 2);
}

void app_teams_set_launch_status(uint8_t status)
{
    teams_launch = status;
}

void app_teams_set_notify_status(uint8_t status)
{
    teams_notify = status;
}

void app_teams_led_handle_notification(uint8_t *bd_addr)
{
    if (app_asp_device_check_is_multilink_active_addr(bd_addr))
    {
        teams_extend_led_control_when_notify();
        app_teams_set_notify_status(1);
    }
}

void app_asp_device_build_relay_info(T_APP_ASP_RELAY_INFO *info)
{
    uint8_t *active_asp_device_bd_addr = NULL;
    for (uint8_t i = 0; i < TEAMS_LINK_NUM; i++)
    {
        if (app_asp_link_data[i].used)
        {
            info->app_asp_link_relay_info[i].used = true;
            memcpy(info->app_asp_link_relay_info[i].bd_addr, app_asp_link_data[i].bd_addr, 6);
            info->app_asp_link_relay_info[i].pending_flag = app_asp_link_data[i].pending_flag;
            info->app_asp_link_relay_info[i].pending_dt_mask = app_asp_link_data[i].pending_dt_mask;
            info->app_asp_link_relay_info[i].asp_link_auth_timestamp =
                app_asp_link_data[i].asp_link_auth_timestamp;
            info->app_asp_link_relay_info[i].asp_link_active_timestamp =
                app_asp_link_data[i].asp_link_active_timestamp;
            bt_link_per_report(app_asp_link_data[i].bd_addr, false, APP_ASP_PER_REPORT_PERIOD);
        }
    }
    memcpy(info->button_info, button_info, 3);
    info->mute_lock = mute_lock;
    info->conference_num = local_conference_count;
    active_asp_device_bd_addr = asp_device_api_get_active_link_addr();
    if (active_asp_device_bd_addr)
    {
        memcpy(info->active_asp_idx, active_asp_device_bd_addr, 6);
    }
}

void app_asp_device_pre_handle_relay_info(void)
{
    app_asp_device_delete_all_link();
}

void app_asp_device_handle_relay_info(T_APP_ASP_RELAY_INFO *info, uint16_t data_len)
{
    if (data_len != sizeof(T_APP_ASP_RELAY_INFO))
    {
        return;
    }

    app_asp_device_pre_handle_relay_info();

    for (uint8_t i = 0; i < TEAMS_LINK_NUM; i++)
    {
        if (info->app_asp_link_relay_info[i].used)
        {
            app_asp_device_alloc_link_by_idx(info->app_asp_link_relay_info[i].bd_addr, i);
            if (info->app_asp_link_relay_info[i].pending_flag)
            {
                app_start_timer(&app_asp_link_data[i].app_asp_timer_handle_pending, "app_asp_pending",
                                app_asp_timer_id,
                                APP_ASP_LINK_PENDING, i, 0, APP_ASP_PENDING_TIMEOUT);
            }
            else
            {
                app_asp_link_data[i].pending_flag = false;
            }
            app_asp_link_data[i].asp_link_auth_timestamp =
                info->app_asp_link_relay_info[i].asp_link_auth_timestamp;
            app_asp_link_data[i].asp_link_active_timestamp =
                info->app_asp_link_relay_info[i].asp_link_active_timestamp;
        }
    }

    memcpy(button_info, info->button_info, 3);
    mute_lock = info->mute_lock;
    local_conference_count = info->conference_num;
    app_asp_device_set_active_index(info->active_asp_idx);
}

#if F_APP_USB_HID_SUPPORT
static void app_asp_device_usb_adp_state_change_cb(T_ADP_PLUG_EVENT event, void *user_data)
{
    if (event == ADP_EVENT_PLUG_IN)
    {

    }
    else if (event == ADP_EVENT_PLUG_OUT)
    {
#if F_APP_TEAMS_FEATURE_SUPPORT
        asp_device_free_link(teams_asp_usb_addr);
        T_ASP_INFO_LINK_STATE_DATA asp_link_state;
        T_ASP_DEVICE_LINK *p_asp_link = NULL;
        p_asp_link = asp_device_find_link_by_addr(teams_asp_usb_addr);
        if (p_asp_link && p_asp_link->link_state == ASP_DEVICE_LINK_CONNECTED_ACTIVE)
        {
            asp_link_state.asp_link_state = false;
            memcpy(asp_link_state.bd_addr, teams_asp_usb_addr, 6);
            asp_device_handle_asp_info(T_ASP_DEVICE_ASP_INFO_STATE, &asp_link_state, sizeof(asp_link_state));
        }
        app_asp_device_delete_link(teams_asp_usb_addr);
#endif
    }
}
#endif

bool app_asp_device_init(void)
{
    memset(app_asp_link_data, 0, sizeof(app_asp_link_data));
    bt_mgr_cback_register(app_asp_device_event_cback);
#if F_APP_USB_HID_SUPPORT
    adp_register_state_change_cb(ADP_DETECT_5V,
                                 (P_ADP_PLUG_CBACK)app_asp_device_usb_adp_state_change_cb, NULL);
#endif
    app_timer_reg_cb(app_asp_device_timer_callback, &app_asp_timer_id);
    return asp_device_api_init(app_asp_device_handle_notification_cback,
                               app_asp_device_handle_call_event_cback,
                               app_asp_device_handle_telemetry_cback,
                               app_asp_device_handle_asp_info_cback);
}
#endif
