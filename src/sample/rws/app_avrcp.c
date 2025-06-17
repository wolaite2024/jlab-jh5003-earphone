/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include "string.h"
#include "trace.h"
#include "app_timer.h"
#include "btm.h"
#include "bt_avrcp.h"
#include "bt_bond.h"
#include "app_cfg.h"
#include "app_dsp_cfg.h"
#include "app_link_util.h"
#include "app_avrcp.h"
#include "app_main.h"
#include "app_relay.h"
#include "audio_volume.h"
#include "app_audio_policy.h"
#include "audio_track.h"
#include "app_bt_policy_api.h"
#include "app_bt_policy_int.h"
#include "app_report.h"
#include "app_cmd.h"
#include "app_multilink.h"
#include "app_bond.h"
#include "app_roleswap_control.h"

#if F_APP_AVRCP_CMD_SUPPORT
#define ALL_ELEMENT_ATTR                        0x00
#define MAX_NUM_OF_ELEMENT_ATTR                 0x07
#endif

#if F_APP_LISTENING_MODE_SUPPORT
#include "app_listening_mode.h"
#endif

#if F_APP_DURIAN_SUPPORT
#include "app_durian.h"
#endif

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
#include "app_dongle_source_ctrl.h"
#endif

static uint8_t timer_idx_check_abs_vol_supported = 0;

typedef enum
{
    APP_AVRCP_CHECK_ABS_VOL_SUPPORTED,
} T_APP_AVRCP_TIMER_ID;

static uint8_t app_avrcp_timer_id = 0;

static bool app_avrcp_vol_change_ongoing = false;
static T_BT_EVENT_PARAM_AVRCP_ABSOLUTE_VOLUME_SET avrcp_abs_vol_set = {0};
static T_BT_EVENT_PARAM_AVRCP_ABSOLUTE_VOLUME_SET avrcp_abs_vol_set_cur = {0};

static void app_avrcp_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_AVRCP_CHECK_ABS_VOL_SUPPORTED:
        {
            app_avrcp_stop_abs_vol_check_timer();

            /* not support abs vol and no buds has vol control; set dac gain to max */
            if (app_cfg_nv.either_bud_has_vol_ctrl_key == false)
            {
                uint8_t pair_idx;
                uint8_t app_idx = (uint8_t)param;
                uint8_t *addr = app_db.br_link[app_idx].bd_addr;

                T_APP_BR_LINK *p_link = app_link_find_br_link(addr);

                if (p_link && app_bond_get_pair_idx_mapping(p_link->bd_addr, &pair_idx))
                {
                    app_cfg_nv.audio_gain_level[pair_idx] = app_dsp_cfg_vol.playback_volume_max;
                    p_link->abs_vol_state = ABS_VOL_NOT_SUPPORTED;

                    if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                    {
                        if (p_link->a2dp_track_handle)
                        {
                            audio_track_volume_out_set(p_link->a2dp_track_handle, app_dsp_cfg_vol.playback_volume_max);
                            app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_PLAYBACK);
                        }
                    }
                    else
                    {
                        app_avrcp_sync_abs_vol_state(p_link->bd_addr, ABS_VOL_NOT_SUPPORTED);
                    }
                }
            }
        }
        break;

    default:
        break;
    }
}

void app_avrcp_sync_status(void)
{
    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
        (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY))
    {
        app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_PLAY_STATUS);
    }
}

static void app_avrcp_abs_vol_set(uint8_t *bd_addr, uint8_t volume)
{
    uint8_t vol = 0;
    uint8_t pair_idx_mapping = 0;
    T_APP_BR_LINK *p_link = NULL;
    uint8_t level = 0;

    p_link = app_link_find_br_link(bd_addr);

    memcpy(avrcp_abs_vol_set.bd_addr, bd_addr, 6);
    avrcp_abs_vol_set.volume = volume;
    avrcp_abs_vol_set_cur = avrcp_abs_vol_set;

    if (p_link != NULL)
    {
        if (p_link->streaming_fg == true)
        {
            app_avrcp_vol_change_ongoing = true;
        }

        app_bond_get_pair_idx_mapping(p_link->bd_addr, &pair_idx_mapping);

        // if enable_align_default_volume_after_factory_reset, and a support abs vol phone first connect, sync default vol to src
        if ((p_link->abs_vol_state == ABS_VOL_SUPPORTED) &&
            (app_bt_policy_get_first_connect_sync_default_vol_flag()))
        {
            vol = (app_dsp_cfg_vol.playback_volume_default * 0x7F +
                   app_dsp_cfg_vol.playback_volume_max / 2) /
                  app_dsp_cfg_vol.playback_volume_max;

            app_cfg_nv.audio_gain_level[pair_idx_mapping] = app_dsp_cfg_vol.playback_volume_default;

            bt_avrcp_volume_change_req(bd_addr, vol);
            app_bt_policy_set_first_connect_sync_default_vol_flag(false);
        }
        else
        {

            level = (volume * app_dsp_cfg_vol.playback_volume_max + 0x7F / 2) / 0x7F;

#if F_APP_HANDLE_TOO_SMALL_ABS_VOL
            /*
                Add handle for this case.
                Case: If buds connect to pc and set level to 2%,
                it would mapping to level 0. It will be no sound.

                If level is 0 vol is not 0, set level to 1.
            */
            if ((level == 0) && (volume != 0))
            {
                level = 1;
            }
#endif
            app_cfg_nv.audio_gain_level[pair_idx_mapping] = level;
        }

        if (app_cfg_const.play_max_min_tone_when_adjust_vol_from_phone)
        {
            static bool is_first_max_min_vol_set_from_phone = false;

            vol = app_cfg_nv.audio_gain_level[pair_idx_mapping];
            if ((vol == app_dsp_cfg_vol.playback_volume_max)
                || (vol == app_dsp_cfg_vol.playback_volume_min)
               )
            {
                if (!is_first_max_min_vol_set_from_phone)
                {
                    is_first_max_min_vol_set_from_phone = true;
                    app_audio_set_max_min_vol_from_phone_flag(true);
                }
            }
            else
            {
                is_first_max_min_vol_set_from_phone = false;
            }
        }

        if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
        {
            app_audio_vol_set(p_link->a2dp_track_handle, app_cfg_nv.audio_gain_level[pair_idx_mapping]);
            app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_PLAYBACK);
        }
        else
        {
            uint8_t cmd_ptr[9];

            memcpy(&cmd_ptr[0], p_link->bd_addr, 6);
            cmd_ptr[6] = AUDIO_STREAM_TYPE_PLAYBACK;
            cmd_ptr[7] = app_cfg_nv.audio_gain_level[pair_idx_mapping];
            cmd_ptr[8] = 0;
            app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_A2DP_VOLUME_SYNC,
                                               cmd_ptr, 9, REMOTE_TIMER_HIGH_PRECISION, 0, false);
        }
    }
}

void app_avrcp_set_vol_is_changed(void)
{
    if ((memcmp(avrcp_abs_vol_set_cur.bd_addr, avrcp_abs_vol_set.bd_addr, 6) != 0) ||
        (avrcp_abs_vol_set_cur.volume != avrcp_abs_vol_set.volume))
    {
        app_avrcp_abs_vol_set(avrcp_abs_vol_set_cur.bd_addr, avrcp_abs_vol_set_cur.volume);
    }
    else
    {
        app_avrcp_vol_change_ongoing = false;
    }
    APP_PRINT_INFO5("app_avrcp_set_vol_is_changed: bd_addr %s %s,vol %d %d ongoing %d",
                    TRACE_BDADDR(avrcp_abs_vol_set_cur.bd_addr), TRACE_BDADDR(avrcp_abs_vol_set.bd_addr),
                    avrcp_abs_vol_set_cur.volume, avrcp_abs_vol_set.volume, app_avrcp_vol_change_ongoing);
}

static void app_avrcp_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    T_APP_BR_LINK *p_link;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_AVRCP_CONN_IND:
        {
            p_link = app_link_find_br_link(param->avrcp_conn_ind.bd_addr);
            if (p_link != NULL)
            {
#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
                bool is_dongle_link = false;

                if (!memcmp(app_cfg_nv.dongle_addr, param->avrcp_conn_ind.bd_addr, 6))
                {
                    is_dongle_link = true;
                }

                if (app_cfg_const.enable_24g_bt_audio_source_switch &&
                    (((app_cfg_nv.allowed_source == ALLOWED_SOURCE_24G) && !is_dongle_link) ||
                     ((app_cfg_nv.allowed_source == ALLOWED_SOURCE_BT) && is_dongle_link)))
                {
                    APP_PRINT_TRACE3("app_avrcp_bt_cback: BT_EVENT_AVRCP_CONN_IND, bd_addr %s, allowed_source %d, is_dongle_link %d",
                                     TRACE_BDADDR(param->avrcp_conn_ind.bd_addr), app_cfg_nv.allowed_source, is_dongle_link);

                    bt_avrcp_connect_cfm(p_link->bd_addr, false); // reject
                }
                else
#endif
                {
                    bool accept = (app_bt_policy_get_profs_by_bond_flag(ALL_PROFILE_MASK) & AVRCP_PROFILE_MASK) ? true :
                                  false;
                    bt_avrcp_connect_cfm(p_link->bd_addr, accept);
                }
            }
        }
        break;

    case BT_EVENT_AVRCP_BROWSING_CONN_IND:
        {
            p_link = app_link_find_br_link(param->avrcp_browsing_conn_ind.bd_addr);
            if (p_link != NULL)
            {
                bt_avrcp_browsing_connect_cfm(p_link->bd_addr, true);
            }
        }
        break;

    case BT_EVENT_AVRCP_GET_CAPABILITIES_RSP:
        {
            p_link = app_link_find_br_link(param->avrcp_browsing_conn_ind.bd_addr);
            if (p_link != NULL)
            {
                uint8_t  capability_count;
                uint8_t *capabilities;

                capability_count = param->avrcp_get_capabilities_rsp.capability_count;
                capabilities = param->avrcp_get_capabilities_rsp.capabilities;
                while (capability_count != 0)
                {
                    bt_avrcp_register_notification_req(p_link->bd_addr, *capabilities);
                    capability_count -= 1;
                    capabilities += 1;
                }
            }
        }
        break;

    case BT_EVENT_AVRCP_ABSOLUTE_VOLUME_SET:
        {
#if F_APP_CHARGER_CASE_SUPPORT
            app_report_level_to_charger_case(param->avrcp_absolute_volume_set.volume,
                                             param->avrcp_absolute_volume_set.bd_addr);
#endif

            if (!app_avrcp_vol_change_ongoing ||
                (memcmp(avrcp_abs_vol_set_cur.bd_addr, param->avrcp_absolute_volume_set.bd_addr, 6) != 0))

            {
                app_avrcp_abs_vol_set(param->avrcp_absolute_volume_set.bd_addr,
                                      param->avrcp_absolute_volume_set.volume);
            }
            else
            {
                avrcp_abs_vol_set_cur = param->avrcp_absolute_volume_set;
                break;
            }
        }
        break;

    case BT_EVENT_AVRCP_VOLUME_UP:
        {
            p_link = app_link_find_br_link(param->avrcp_volume_up.bd_addr);
            if (p_link != NULL)
            {
                uint8_t pair_idx_mapping;
                uint8_t curr_volume;

                app_bond_get_pair_idx_mapping(p_link->bd_addr, &pair_idx_mapping);
                curr_volume = app_cfg_nv.audio_gain_level[pair_idx_mapping];

                if (curr_volume < app_dsp_cfg_vol.playback_volume_max)
                {
                    curr_volume++;
                }
                else
                {
                    curr_volume = app_dsp_cfg_vol.playback_volume_max;
                }

                if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    audio_track_volume_out_set(p_link->a2dp_track_handle, curr_volume);
                    app_cfg_nv.audio_gain_level[pair_idx_mapping] = curr_volume;
                    app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_PLAYBACK);
                }
                else
                {
                    uint8_t cmd_ptr[9];
                    memcpy(&cmd_ptr[0], p_link->bd_addr, 6);
                    cmd_ptr[6] = AUDIO_STREAM_TYPE_PLAYBACK;
                    cmd_ptr[7] = curr_volume;
                    app_cfg_nv.audio_gain_level[pair_idx_mapping] = curr_volume;
                    cmd_ptr[8] = 0;
                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_A2DP_VOLUME_SYNC,
                                                       cmd_ptr, 9, REMOTE_TIMER_HIGH_PRECISION, 0, false);
                }
            }
        }
        break;

    case BT_EVENT_AVRCP_VOLUME_DOWN:
        {
            p_link = app_link_find_br_link(param->avrcp_volume_down.bd_addr);
            if (p_link != NULL)
            {
                uint8_t pair_idx_mapping;
                uint8_t curr_volume;

                app_bond_get_pair_idx_mapping(p_link->bd_addr, &pair_idx_mapping);

                curr_volume = app_cfg_nv.audio_gain_level[pair_idx_mapping];

                if (curr_volume > app_dsp_cfg_vol.playback_volume_min)
                {
                    curr_volume--;
                }
                else
                {
                    curr_volume = app_dsp_cfg_vol.playback_volume_min;
                }

                if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    audio_track_volume_out_set(p_link->a2dp_track_handle, curr_volume);
                    app_cfg_nv.audio_gain_level[pair_idx_mapping] = curr_volume;
                    app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_PLAYBACK);
                }
                else
                {
                    uint8_t cmd_ptr[9];
                    memcpy(&cmd_ptr[0], p_link->bd_addr, 6);
                    cmd_ptr[6] = AUDIO_STREAM_TYPE_PLAYBACK;
                    cmd_ptr[7] = curr_volume;
                    app_cfg_nv.audio_gain_level[pair_idx_mapping] = curr_volume;
                    cmd_ptr[8] = 0;
                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_A2DP_VOLUME_SYNC,
                                                       cmd_ptr, 9, REMOTE_TIMER_HIGH_PRECISION, 0, false);
                }
            }
        }
        break;

    case BT_EVENT_AVRCP_REG_VOLUME_CHANGED:
        {
            p_link = app_link_find_br_link(param->avrcp_reg_volume_changed.bd_addr);
            if (p_link != NULL)
            {
                uint8_t pair_idx_mapping;
                uint8_t vol;

                app_bond_get_pair_idx_mapping(p_link->bd_addr, &pair_idx_mapping);

                if (app_bt_policy_get_first_connect_sync_default_vol_flag())
                {
                    app_cfg_nv.audio_gain_level[pair_idx_mapping] = app_dsp_cfg_vol.playback_volume_default;
                }

                vol = (app_cfg_nv.audio_gain_level[pair_idx_mapping] * 0x7F +
                       app_dsp_cfg_vol.playback_volume_max / 2) /
                      app_dsp_cfg_vol.playback_volume_max;

#if F_APP_CHARGER_CASE_SUPPORT
                app_report_level_to_charger_case(vol, param->avrcp_reg_volume_changed.bd_addr);
#endif

                if (bt_avrcp_volume_change_register_rsp(p_link->bd_addr, vol))
                {
                    app_avrcp_stop_abs_vol_check_timer();

                    p_link->abs_vol_state = ABS_VOL_SUPPORTED;
                    app_avrcp_sync_abs_vol_state(p_link->bd_addr, ABS_VOL_SUPPORTED);
                }
            }
        }
        break;

    case BT_EVENT_AVRCP_CONN_CMPL:
        {
            p_link = app_link_find_br_link(param->avrcp_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
#if F_APP_DURIAN_SUPPORT
                if (app_durian_audio_abs_is_support(p_link->abs_vol_state))
                {
                    break;
                }
#endif
                if (timer_idx_check_abs_vol_supported == 0)
                {
                    app_start_timer(&timer_idx_check_abs_vol_supported, "check_abs_vol_supported",
                                    app_avrcp_timer_id, APP_AVRCP_CHECK_ABS_VOL_SUPPORTED, p_link->id, false,
                                    1500);
                }
            }
        }
        break;

    case BT_EVENT_AVRCP_DISCONN_CMPL:
        {
            p_link = app_link_find_br_link(param->avrcp_disconn_cmpl.bd_addr);

            if (p_link != NULL)
            {
                p_link->abs_vol_state = ABS_VOL_NOT_SUPPORTED;
                app_avrcp_sync_abs_vol_state(p_link->bd_addr, ABS_VOL_NOT_SUPPORTED);
            }
        }
        break;

    case BT_EVENT_AVRCP_PLAY_STATUS_RSP:
    case BT_EVENT_AVRCP_PLAY_STATUS_CHANGED:
        {
            p_link = app_link_find_br_link(param->avrcp_play_status_changed.bd_addr);

            if (p_link != NULL)
            {
                p_link->avrcp_play_status = param->avrcp_play_status_changed.play_status;
            }
        }
        break;

#if F_APP_DEVICE_CMD_SUPPORT
    case BT_EVENT_SDP_ATTR_INFO:
        {
            T_BT_SDP_ATTR_INFO *sdp_info = &param->sdp_attr_info.info;

            if (sdp_info->srv_class_uuid_data.uuid_16 == UUID_AV_REMOTE_CONTROL_TARGET)
            {
                if (app_cmd_get_report_attr_info_flag())
                {
                    uint8_t temp_buff[5];

                    temp_buff[0] = GET_AVRCP_ATTR_INFO;
                    memcpy(&temp_buff[1], &(sdp_info->profile_version), 2);
                    memcpy(&temp_buff[3], &(sdp_info->supported_feat), 2);

                    app_cmd_set_report_attr_info_flag(false);
                    app_report_event(CMD_PATH_UART, EVENT_REPORT_REMOTE_DEV_ATTR_INFO, 0, temp_buff,
                                     sizeof(temp_buff));
                }
            }
        }
        break;
#endif

    case BT_EVENT_AVRCP_VENDOR_CMD_IND:
        {
            p_link = app_link_find_br_link(param->avrcp_vendor_cmd_ind.bd_addr);
            if (p_link != NULL)
            {
                bt_avrcp_vendor_rsp_send(p_link->bd_addr, BT_AVRCP_RESPONSE_NOT_IMPLEMENTED,
                                         param->avrcp_vendor_cmd_ind.company_id, NULL, 0);
            }
        }
        break;

    default:
        {
            handle = false;
        }
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_avrcp_bt_cback: event_type 0x%04x", event_type);
    }
}

bool app_avrcp_abs_vol_check_timer_exist(void)
{
    bool ret = false;

    if (timer_idx_check_abs_vol_supported)
    {
        ret = true;
    }

    return ret;
}

void app_avrcp_stop_abs_vol_check_timer(void)
{
    app_stop_timer(&timer_idx_check_abs_vol_supported);

    app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_ABS_VOL_CHECK_TIMEOUT);
}

bool app_avrcp_sync_abs_vol_state(uint8_t *bd_addr, T_APP_ABS_VOL_STATE abs_vol_state)
{
    T_APP_BR_LINK *p_link = NULL;
    uint8_t cmd_ptr[9] = {0};
    uint8_t pair_idx_mapping;
    bool ret = false;

    if (app_bond_get_pair_idx_mapping(bd_addr, &pair_idx_mapping) == false)
    {
        return ret;
    }

    APP_PRINT_INFO3("app_avrcp_sync_abs_vol_state: bd_addr %s, abs_vol_state %d audio_gain_level %d",
                    TRACE_BDADDR(bd_addr),
                    abs_vol_state, app_cfg_nv.audio_gain_level[pair_idx_mapping]);

    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
        (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
    {
        memcpy(&cmd_ptr[0], bd_addr, 6);
        cmd_ptr[6] = abs_vol_state;
        cmd_ptr[7] = app_cfg_nv.audio_gain_level[pair_idx_mapping];

        p_link = app_link_find_br_link(bd_addr);
        if (p_link != NULL)
        {
            cmd_ptr[8] = p_link->playback_muted;
        }

        ret = app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY,
                                                 APP_REMOTE_MSG_SYNC_ABS_VOL_STATE,
                                                 cmd_ptr, 9, REMOTE_TIMER_HIGH_PRECISION, 0, false);
    }

    return ret;
}

void app_avrcp_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                          uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));

    switch (cmd_id)
    {
#if F_APP_AVRCP_CMD_SUPPORT
    case CMD_AVRCP_LIST_SETTING_ATTR:
        {
            uint8_t app_index = cmd_ptr[2];

            if (bt_avrcp_app_setting_attrs_list(app_db.br_link[app_index].bd_addr) == false)
            {
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_AVRCP_LIST_SETTING_VALUE:
        {
            uint8_t app_index = cmd_ptr[2];

            if (bt_avrcp_app_setting_values_list(app_db.br_link[app_index].bd_addr, cmd_ptr[3]) == false)
            {
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_AVRCP_GET_CURRENT_VALUE:
        {
            uint8_t app_index = cmd_ptr[2];
            uint8_t attr_num = cmd_ptr[3];
            uint8_t attr_list[attr_num];

            memcpy(attr_list, &cmd_ptr[4], attr_num);
            if (bt_avrcp_app_setting_value_get(app_db.br_link[app_index].bd_addr, attr_num, attr_list) == false)
            {
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_AVRCP_SET_VALUE:
        {
            uint8_t app_index = cmd_ptr[2];
            uint8_t attr_num = cmd_ptr[3];
            uint8_t attr_list[attr_num * 2];

            memcpy(attr_list, &cmd_ptr[4], attr_num * 2);
            if (bt_avrcp_app_setting_value_set(app_db.br_link[app_index].bd_addr, attr_num,
                                               (T_BT_AVRCP_APP_SETTING *)attr_list) == false)
            {
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_AVRCP_ABORT_DATA_TRANSFER:
        {
            uint8_t app_index = cmd_ptr[2];

            if (bt_avrcp_continuing_rsp_abort(app_db.br_link[app_index].bd_addr) == false)
            {
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_AVRCP_SET_ABSOLUTE_VOLUME:
        {
            uint8_t app_index = cmd_ptr[2];

            if (cmd_ptr[3] <= 0x7f)// abs volume: 0x00 ~ 0x7f
            {
                if (bt_avrcp_absolute_volume_set(app_db.br_link[app_index].bd_addr, cmd_ptr[3]) == false)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_AVRCP_GET_PLAY_STATUS:
        {
            uint8_t app_index = cmd_ptr[2];

            if (bt_avrcp_get_play_status_req(app_db.br_link[app_index].bd_addr) == false)
            {
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_AVRCP_GET_ELEMENT_ATTR:
        {
            uint8_t app_index = cmd_ptr[2];

            uint8_t attr_list[7] = {1, 2, 3, 4, 5, 6, 7};
            uint8_t number_attr = 7;

            if (cmd_ptr[3] == ALL_ELEMENT_ATTR)
            {
                if (bt_avrcp_get_element_attr_req(app_db.br_link[app_index].bd_addr, number_attr,
                                                  attr_list) == false)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            else if (cmd_ptr[3] <= MAX_NUM_OF_ELEMENT_ATTR)
            {
                number_attr = cmd_ptr[3];
                memcpy(attr_list, &cmd_ptr[4], number_attr);

                if (bt_avrcp_get_element_attr_req(app_db.br_link[app_index].bd_addr, number_attr,
                                                  attr_list) == false)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;
#endif

    default:
        break;
    }
}

void app_avrcp_get_supported_features(uint8_t *ct_features, uint8_t *tg_features)
{
    *ct_features = (app_cfg_const.avrcp_features_ct.features_value & 0x0F);
    *tg_features = (app_cfg_const.avrcp_features_tg.features_value & 0x0F);

    if (*ct_features == 0 && *tg_features == 0)
    {
        /* illegal condition, set the features to default */
        *ct_features = BT_AVRCP_FEATURE_CATEGORY_1;
        *tg_features = BT_AVRCP_FEATURE_CATEGORY_2;
    }
}

void app_avrcp_init(void)
{
    if (app_cfg_const.supported_profile_mask & AVRCP_PROFILE_MASK)
    {
        uint8_t ct_features;
        uint8_t tg_features;

        app_avrcp_get_supported_features(&ct_features, &tg_features);
        bt_avrcp_init(ct_features, tg_features);

        bt_mgr_cback_register(app_avrcp_bt_cback);

        app_timer_reg_cb(app_avrcp_timeout_cb, &app_avrcp_timer_id);
    }
}

