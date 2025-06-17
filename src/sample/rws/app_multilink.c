/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <string.h>
#include "trace.h"
#include "app_timer.h"
#include "gap_br.h"
#include "btm.h"
#include "bt_bond.h"
#include "remote.h"
#include "bt_a2dp.h"
#include "bt_avrcp.h"
#include "bt_hfp.h"
#include "app_a2dp.h"
#include "app_cfg.h"
#include "app_hfp.h"
#include "audio_track.h"
#include "app_link_util.h"
#include "app_main.h"
#include "app_roleswap.h"
#include "app_bond.h"
#include "app_listening_mode.h"
#include "app_multilink.h"
#include "app_audio_policy.h"
#include "app_bt_policy_int.h"
#include "app_bt_policy_api.h"
#include "app_bt_point.h"
#include "app_bt_sniffing.h"
#include "audio_volume.h"
#include "audio_track.h"
#include "app_relay.h"
#include "app_lea_adv.h"
#include "app_auto_power_off.h"
#include "app_device.h"
#include "app_report.h"
#include "app_sensor.h"

#if F_APP_GAMING_DONGLE_SUPPORT
#include "app_dongle_dual_mode.h"
#endif

#if F_APP_LINEIN_SUPPORT
#include "app_line_in.h"
#endif

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
#include "app_single_multilink_customer.h"
#elif F_APP_MUTLILINK_SOURCE_PRIORITY_UI
#include "app_multilink_customer.h"
#endif

#if F_APP_QOL_MONITOR_SUPPORT
#include "app_qol.h"
#endif

#if F_APP_MALLEUS_SUPPORT
#if F_APP_MALLEUS_TWS_SPATIAL_AUDIO == 0
#include "app_sensor_mems.h"
#endif
#endif

#if F_APP_DURIAN_SUPPORT
#include "app_durian.h"
#endif

#if CONFIG_REALTEK_GFPS_SASS_SUPPORT
#include "app_gfps_msg.h"
#include "app_sass_policy.h"
#include "bt_gfps.h"
#endif

#if F_APP_GAMING_CHAT_MIXING_SUPPORT
#include "app_dongle_dual_mode.h"
#endif

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
#include "app_dongle_dual_mode.h"
#endif

#if F_APP_LEA_SUPPORT
#include "multitopology_ctrl.h"
#endif

#define ERWS_MULTILINK_SUPPORT
static bool le_sniffing_stop = false;
static uint8_t multilink_timer_id = 0;
static uint8_t timer_idx_multilink_disconn_inctive_acl = 0;
static bool sco_wait_from_sco_sniffing_stop = false;
static bool multilink_is_on_by_mmi = false;
static uint8_t pending_sco_index_from_sco = 0xFF;
static uint8_t timer_idx_multilink_silence_packet_judge = 0;
static uint8_t timer_idx_multilink_silence_packet_detect = 0;
#if F_APP_MUTILINK_VA_PREEMPTIVE
static uint8_t timer_idx_multilink_disc_va_sco = 0;
static uint8_t stream_only[MAX_BR_LINK_NUM] = {false};
#if F_APP_DURIAN_SUPPORT
static uint8_t pre_avrcp[MAX_BR_LINK_NUM] = {0};
#endif
static bool va_wait_from_va_sniffing_stop = false;
static uint8_t pending_va_index_from_va = 0xFF;
static uint8_t pending_va_sniffing_type = 0xFF;
static uint8_t active_va_idx = MAX_BR_LINK_NUM;
void app_check_voice_dial(T_APP_BR_LINK *p_link);
#endif
static uint8_t active_hfp_transfer = 0xFF;
#if CONFIG_REALTEK_GFPS_SASS_SUPPORT
static uint8_t sass_active_idx = MAX_BR_LINK_NUM;
#endif
static bool app_multi_check_computer(uint8_t *bd_addr);

typedef enum
{
    APP_TIMER_MULTILINK_DISCONN_INACTIVE_ACL,
    APP_TIMER_MULTILINK_DISC_VA_SCO,
    APP_TIMER_MULTILINK_SILENCE_PACKET_JUDGE,
    APP_TIMER_MULTILINK_SILENCE_PACKET_DETECT,
} T_MULTILINK_TIMER;

static bool app_multi_check_computer(uint8_t *bd_addr)
{
    uint32_t bond_flag = 0;
    bool is_computer = false;

    if (bt_bond_flag_get(bd_addr, &bond_flag))
    {
        if (bond_flag & BOND_FLAG_COMPUTER)
        {
            is_computer = true;
        }
    }

    return is_computer;
}

void app_multi_active_hfp_transfer(uint8_t idx, bool disc, bool force)
{
    uint8_t active_hf_idx = app_hfp_get_active_idx();

    if (idx != active_hf_idx)
    {
        active_hfp_transfer = idx;

        if (force)
        {
            if (app_db.br_link[idx].call_status != BT_HFP_CALL_IDLE)
            {
                sco_wait_from_sco_sniffing_stop = true;
                pending_sco_index_from_sco = idx;
            }
            bt_hfp_audio_disconnect_req(app_db.br_link[active_hf_idx].bd_addr);
        }
        else if ((app_db.br_link[active_hf_idx].call_status == APP_CALL_INCOMING) ||
                 ((app_db.br_link[active_hf_idx].sco_handle == 0) && disc))
        {
            if (app_multi_check_in_sniffing())
            {
                app_bt_sniffing_stop(app_db.br_link[active_hf_idx].bd_addr,
                                     app_db.br_link[active_hf_idx].sniffing_type);
            }
            else
            {
                app_multi_handle_sniffing_link_disconnect_event(0xFF);
            }
        }
        else
        {
            if ((app_db.br_link[idx].call_status != BT_HFP_CALL_IDLE) &&
                (app_db.br_link[idx].call_status != APP_CALL_INCOMING))
            {
                sco_wait_from_sco_sniffing_stop = true;
                pending_sco_index_from_sco = idx;
            }

            if (disc)
            {
#if CONFIG_REALTEK_GFPS_SASS_SUPPORT
                if (app_multi_check_in_sniffing())
                {
                    app_bt_sniffing_stop(app_db.br_link[active_hf_idx].bd_addr,
                                         app_db.br_link[active_hf_idx].sniffing_type);
                }
                else
                {
                    app_multi_handle_sniffing_link_disconnect_event(0xFF);
                }
#else
                bt_hfp_audio_disconnect_req(app_db.br_link[active_hf_idx].bd_addr); //terminate call so no disc
#endif
            }
        }
    }
}

bool app_multi_check_in_sniffing(void)
{
    uint8_t i;
    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].bt_sniffing_state > APP_BT_SNIFFING_STATE_READY)
        {
            return true;
        }
    }
    return false;
}

bool app_multi_allow_a2dp_sniffing(uint8_t id)
{
#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
    if ((app_dongle_disallow_bt_streaming() || mtc_is_lea_cis_stream(NULL)) &&
        !app_cfg_const.enable_24g_bt_audio_source_switch)
    {
        return false;
    }
#endif

    if ((app_hfp_get_call_status() == APP_CALL_IDLE) ||
        ((app_link_get_sco_conn_num() == 0) &&
         (app_hfp_get_active_idx() == id)))
    {
        return true;
    }

#if F_APP_GAMING_CHAT_MIXING_SUPPORT
    return true;
#else
    return false;
#endif
}

void app_multi_stream_avrcp_set(uint8_t idx)
{
    app_a2dp_set_active_idx(idx);
#if F_APP_MUTILINK_VA_PREEMPTIVE
    if (app_db.br_link[idx].avrcp_play_status != BT_AVRCP_PLAY_STATUS_PLAYING)
    {
#if F_APP_DURIAN_SUPPORT
        stream_only[idx] = (pre_avrcp[idx] != BT_AVRCP_PLAY_STATUS_PLAYING);
#else
        stream_only[idx] = true;
#endif
    }
#endif
    app_db.br_link[idx].avrcp_play_status = BT_AVRCP_PLAY_STATUS_PLAYING;
    app_db.br_link[idx].avrcp_ready_to_pause = true;
    app_multi_a2dp_active_link_set(app_db.br_link[idx].bd_addr);
    app_bond_set_priority(app_db.br_link[idx].bd_addr);
}

bool app_multi_active_link_check(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link = app_link_find_br_link(bd_addr);

    if (p_link)
    {
        APP_PRINT_TRACE4("app_multi_active_link_check: p_link %x, streaming_fg %d, sco_handle %d, call status %d",
                         p_link, p_link->streaming_fg, p_link->sco_handle, app_hfp_get_call_status());
        if (p_link->streaming_fg || p_link->sco_handle != 0)
        {
            if (p_link->sco_handle != 0 && app_hfp_get_active_idx() == p_link->id)
            {
                return true;
            }

            if (app_multi_allow_a2dp_sniffing(p_link->id))
            {
                if (p_link->streaming_fg && app_a2dp_get_active_idx() == p_link->id)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void app_multi_disconnect_inactive_link(uint8_t app_idx)
{
    uint8_t call_num = 0;
    uint8_t i;
    T_APP_BR_LINK *p_link = NULL;
    uint8_t active_hf_idx = app_hfp_get_active_idx();
#if CONFIG_REALTEK_GFPS_SASS_SUPPORT
    uint8_t target_drop_index = app_multi_get_inactive_index(app_idx, call_num, false);
#endif

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].used == true &&
            app_db.br_link[i].call_status != APP_CALL_IDLE)
        {
            call_num++;
        }
    }

    APP_PRINT_TRACE4("app_multi_disconnect_inactive_link: active_a2dp_idx %d, app_idx %d, "
                     "call_num %d, max_legacy_multilink_devices %d",
                     app_a2dp_get_active_idx(), app_idx, call_num,
                     app_cfg_const.max_legacy_multilink_devices);

    if (call_num >= app_cfg_const.max_legacy_multilink_devices)
    {
        app_bt_policy_disconnect(app_db.br_link[app_idx].bd_addr, ALL_PROFILE_MASK);
        return;
    }

    if (call_num > 0)
    {
#if CONFIG_REALTEK_GFPS_SASS_SUPPORT
        if (app_cfg_const.enable_multi_link)
        {
            //inactive index may change according to switch preference
            if (target_drop_index != MAX_BR_LINK_NUM)
            {
                if (extend_app_cfg_const.gfps_sass_support)
                {
                    app_bt_policy_disconnect(app_db.br_link[target_drop_index].bd_addr, ALL_PROFILE_MASK);
                }
                else
                {
                    if (call_num >= app_cfg_const.max_legacy_multilink_devices)
                    {
                        app_bt_policy_disconnect(app_db.br_link[app_idx].bd_addr, ALL_PROFILE_MASK);
                    }
                    else
                    {
                        app_bt_policy_disconnect(app_db.br_link[target_drop_index].bd_addr, ALL_PROFILE_MASK);
                    }
                }
            }
        }
        else
        {
            //single link disconnect active/new link directly
            if (extend_app_cfg_const.gfps_sass_support)
            {
                app_bt_policy_disconnect(app_db.br_link[app_hfp_get_active_idx()].bd_addr, ALL_PROFILE_MASK);
            }
            else
            {
                app_bt_policy_disconnect(app_db.br_link[app_idx].bd_addr, ALL_PROFILE_MASK);
            }
        }
#else
        for (i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (app_link_check_b2s_link_by_id(i))
            {
                if ((i != app_idx) && (i != active_hf_idx))
                {
                    app_bt_policy_disconnect(app_db.br_link[i].bd_addr, ALL_PROFILE_MASK);
                    return;
                }
            }
        }
#endif
    }
    else
    {
#if CONFIG_REALTEK_GFPS_SASS_SUPPORT
        //disconnect one device if no bond and priority information when ACL connected
        if ((extend_app_cfg_const.gfps_sass_support) && (target_drop_index != MAX_BR_LINK_NUM))
        {
            app_bt_policy_disconnect(app_db.br_link[target_drop_index].bd_addr, ALL_PROFILE_MASK);
        }
#else
        uint8_t priority;
        uint8_t bond_num;
        uint8_t bd_addr[6];

        bond_num = app_bond_b2s_num_get();
        if (bond_num)
        {
            for (priority = bond_num; priority > 0; priority--)
            {
                if (app_bond_b2s_addr_get(priority, bd_addr))
                {
                    p_link = app_link_find_b2s_link(bd_addr);
                    if (p_link != NULL)
                    {
                        if ((p_link->id != app_idx))
                        {
                            app_bt_policy_disconnect(bd_addr, ALL_PROFILE_MASK);
                            return;
                        }
                    }
                }
            }
        }
#endif
#if CONFIG_REALTEK_GFPS_SASS_SUPPORT
        //disconnect one device if no bond and priority information when ACL connected
        if ((extend_app_cfg_const.gfps_sass_support) && (target_drop_index == MAX_BR_LINK_NUM))
#endif
        {
            //disconnect one device if no bond and priority information when ACL connected
            for (i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if (app_link_check_b2s_link_by_id(i))
                {
                    if (i != app_idx)
                    {
                        app_bt_policy_disconnect(app_db.br_link[i].bd_addr, ALL_PROFILE_MASK);
                        return;
                    }
                }
            }
        }
    }
}

void app_multi_roleswap_disconnect_inactive_link(void)
{
    uint8_t call_num = 0;
    uint8_t i;
    uint8_t exit_cause = 0;
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();
    uint8_t active_hf_idx = app_hfp_get_active_idx();

    app_db.disconnect_inactive_link_actively = true;

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].used == true &&
            app_db.br_link[i].sco_handle != 0)
        {
            call_num++;
        }
    }

    app_db.connected_num_before_roleswap = app_link_get_b2s_link_num();

    if (call_num > 0)
    {
        for (i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (app_link_check_b2s_link_by_id(i))
            {
                if (i != active_hf_idx)
                {
                    memcpy(app_db.resume_addr, app_db.br_link[i].bd_addr, 6);

                    if ((!app_db.br_link[i].connected_profile) &&
                        (memcmp(app_db.br_link[i].bd_addr, app_db.resume_addr, 6) == 0))
                    {
                        //If no profile connected before disconnecting acl, connected tone need.
                        app_audio_set_connected_tone_need(true);
                    }

                    app_bt_policy_disconnect(app_db.br_link[i].bd_addr, ALL_PROFILE_MASK);
                    app_start_timer(&timer_idx_multilink_disconn_inctive_acl, "multilink_disconn_inctive_acl",
                                    multilink_timer_id, APP_TIMER_MULTILINK_DISCONN_INACTIVE_ACL, i, false,
                                    TIMER_TO_DISCONN_ACL);
                    exit_cause = 1;
                    goto exit;
                }
            }
        }
    }
    else
    {
        for (i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (app_link_check_b2s_link_by_id(i))
            {
                if (i != active_a2dp_idx)
                {
                    memcpy(app_db.resume_addr, app_db.br_link[i].bd_addr, 6);

                    if ((!app_db.br_link[i].connected_profile) &&
                        (memcmp(app_db.br_link[i].bd_addr, app_db.resume_addr, 6) == 0))
                    {
                        //If no profile connected before disconnecting acl, connected tone need.
                        app_audio_set_connected_tone_need(true);
                    }

                    app_bt_policy_disconnect(app_db.br_link[i].bd_addr, ALL_PROFILE_MASK);
                    app_start_timer(&timer_idx_multilink_disconn_inctive_acl, "multilink_disconn_inctive_acl",
                                    multilink_timer_id, APP_TIMER_MULTILINK_DISCONN_INACTIVE_ACL, i, false,
                                    TIMER_TO_DISCONN_ACL);
                    exit_cause = 2;
                    goto exit;
                }
            }
        }
    }

exit:
    APP_PRINT_TRACE7("app_multi_roleswap_disconnect_inactive_link: active_a2dp_idx %d, "
                     "call_num %d, active_hf_idx %d, connected_num_before_roleswap %d, resume_addr %s exit_cause %d connected_tone_need %d",
                     active_a2dp_idx, call_num, active_hf_idx, app_db.connected_num_before_roleswap,
                     TRACE_BDADDR(app_db.resume_addr), exit_cause, app_db.connected_tone_need);
}

void app_multi_reconnect_inactive_link()
{
    uint8_t b2s_connected_num = app_link_get_b2s_link_num();

    APP_PRINT_TRACE3("app_multi_reconnect_inactive_link: connected_num_before_roleswap %d, resume_addr %s b2s_num %d",
                     app_db.connected_num_before_roleswap, TRACE_BDADDR(app_db.resume_addr), b2s_connected_num);

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY &&
        app_db.connected_num_before_roleswap > app_link_get_b2s_link_num())
    {
        uint32_t bond_flag;
        uint32_t plan_profs;

        app_db.connected_num_before_roleswap = 0;
        bt_bond_flag_get(app_db.resume_addr, &bond_flag);
        if (bond_flag & (BOND_FLAG_HFP | BOND_FLAG_HSP | BOND_FLAG_A2DP))
        {
            plan_profs = app_bt_policy_get_profs_by_bond_flag(bond_flag);
            app_bt_policy_default_connect(app_db.resume_addr, plan_profs, false);
        }
    }
}

void app_multi_stop_acl_disconn_timer(void)
{
    app_stop_timer(&timer_idx_multilink_disconn_inctive_acl);
}

void app_multi_switch_by_mmi(bool is_on_by_mmi)
{
    multilink_is_on_by_mmi = is_on_by_mmi;
}

bool app_multi_is_on_by_mmi(void)
{
    return multilink_is_on_by_mmi;
}

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
#else
static uint8_t app_multi_find_new_active_a2dp_link_by_priority(uint8_t curr_idx)
{
    uint8_t priority;
    uint8_t bond_num;
    uint8_t bd_addr[6];
    T_APP_BR_LINK *p_link = NULL;
    uint8_t active_idx = 0;

    bond_num = app_bond_b2s_num_get();

    for (priority = 1; priority <= bond_num; priority++)
    {
        if (app_bond_b2s_addr_get(priority, bd_addr) == true)
        {
            p_link = app_link_find_b2s_link(bd_addr);
            if (p_link != NULL)
            {
                if (memcmp(p_link->bd_addr, app_db.br_link[curr_idx].bd_addr, 6))
                {
                    active_idx = p_link->id;
                    break;
                }
            }
        }
    }
    return active_idx;
}
#endif

bool app_multi_a2dp_active_link_set(uint8_t *bd_addr)
{
#if F_APP_MUTILINK_VA_PREEMPTIVE
    uint8_t active_a2dp_idx = 0;
#endif
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link)
    {
        app_a2dp_set_active_idx(p_link->id);
#if F_APP_MUTILINK_VA_PREEMPTIVE
        active_a2dp_idx = p_link->id;
#endif

#if F_APP_MUTILINK_VA_PREEMPTIVE
        if (app_cfg_const.enable_multi_link)
        {
            uint8_t pair_idx_mapping;
            app_bond_get_pair_idx_mapping(bd_addr, &pair_idx_mapping);
            audio_track_volume_out_set(app_db.br_link[active_a2dp_idx].a2dp_track_handle,
                                       app_cfg_nv.audio_gain_level[pair_idx_mapping]);
            app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_PLAYBACK);
            if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                uint8_t cmd_ptr[9] = {0};
                memcpy(&cmd_ptr[0], app_db.br_link[active_a2dp_idx].bd_addr, 6);
                cmd_ptr[6] = AUDIO_STREAM_TYPE_PLAYBACK;
                cmd_ptr[7] = app_cfg_nv.audio_gain_level[pair_idx_mapping];
                cmd_ptr[8] = 0;
                app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_A2DP_VOLUME_SYNC,
                                                   cmd_ptr, 9, REMOTE_TIMER_HIGH_PRECISION, 0, false);
            }
        }
#endif
#if F_APP_LOCAL_PLAYBACK_SUPPORT
        if ((app_db.sd_playback_switch == 0) && (p_link->a2dp_track_handle))
#else
        if (p_link->a2dp_track_handle)
#endif
        {
#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
            audio_track_start(p_link->a2dp_track_handle);
#else
#if F_APP_LINEIN_SUPPORT
            if (app_cfg_const.line_in_support)
            {
                if (app_line_in_start_a2dp_check())
                {
                    audio_track_start(p_link->a2dp_track_handle);
                }
            }
            else
#endif
            {
                audio_track_start(p_link->a2dp_track_handle);
            }
#endif
        }

        app_audio_set_avrcp_status(p_link->avrcp_play_status);
        app_avrcp_sync_status();

        return bt_active_link_set(bd_addr);
    }
    return false;
}

//multi-link a2dp
bool app_multi_pause_inactive_a2dp_link_stream(uint8_t keep_active_a2dp_idx, uint8_t resume_fg)
{
    uint8_t i;
    bool play_pause = false;

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if ((i != keep_active_a2dp_idx) &&
            (app_db.br_link[i].connected_profile & (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK)))
        {
#if F_APP_MUTILINK_VA_PREEMPTIVE
            APP_PRINT_TRACE5("app_multi_pause_inactive_a2dp_link_stream: i %d, streaming_fg %d, avrcp_play_status %d, avrcp_ready_to_pause %d, stream_only[i] %d",
                             i, app_db.br_link[i].streaming_fg, app_db.br_link[i].avrcp_play_status,
                             app_db.br_link[i].avrcp_ready_to_pause, stream_only[i]);
#else
            APP_PRINT_TRACE4("app_multi_pause_inactive_a2dp_link_stream: i %d, streaming_fg %d, avrcp_play_status %d, avrcp_ready_to_pause %d",
                             i, app_db.br_link[i].streaming_fg, app_db.br_link[i].avrcp_play_status,
                             app_db.br_link[i].avrcp_ready_to_pause);
#endif

            if ((app_db.br_link[i].streaming_fg == true) ||
                ((app_db.br_link[i].streaming_fg == false) &&
                 (app_db.br_link[i].bt_sniffing_state == APP_BT_SNIFFING_STATE_STARTING) &&
                 (app_db.br_link[i].sniffing_type == BT_SNIFFING_TYPE_A2DP)))
            {
#if F_APP_MUTILINK_VA_PREEMPTIVE
                if (resume_fg && app_db.br_link[i].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING &&
                    !stream_only[i])
#else
                if (resume_fg)
#endif
                {
                    app_db.wait_resume_a2dp_idx = i;
                }
#if F_APP_MUTILINK_VA_PREEMPTIVE
                if (((app_db.br_link[i].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING && !stream_only[i]) ||
                     (stream_only[i] == true && app_multi_check_computer(app_db.br_link[i].bd_addr) == true)) &&
                    (app_db.br_link[i].avrcp_ready_to_pause == true))
#else
                if (app_db.br_link[i].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
#endif
                {
                    bool pause = true;

#if F_APP_GAMING_DONGLE_SUPPORT
                    if (app_link_check_dongle_link(app_db.br_link[i].bd_addr))
                    {
                        pause = false;
                    }
#endif

#if F_APP_GAMING_CHAT_MIXING_SUPPORT
                    if (app_db.remote_is_dongle)
                    {
                        pause = false;
                        if (app_db.br_link[keep_active_a2dp_idx].streaming_fg)
                        {
                            app_a2dp_set_sec_active_idx(i);
                        }
                    }
#endif

                    if (pause)
                    {
                        app_db.br_link[i].avrcp_ready_to_pause = false;
                        bt_avrcp_pause(app_db.br_link[i].bd_addr);
                    }
                }
#if F_APP_GAMING_DONGLE_SUPPORT
                if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE) && (app_cfg_const.enable_multi_link))
                {
                    if (i != app_hfp_get_active_idx())
                    {
#if F_APP_GAMING_CHAT_MIXING_SUPPORT
                        if (app_db.remote_is_dongle)
                        {
                            if (app_db.br_link[keep_active_a2dp_idx].streaming_fg)
                            {
                                app_a2dp_set_sec_active_idx(i);
                            }
                        }
                        else
#endif
                        {
                            audio_track_stop(app_db.br_link[i].a2dp_track_handle);
                        }
                    }
                }
                else
                {
                    audio_track_stop(app_db.br_link[i].a2dp_track_handle);
                }
#else
                audio_track_stop(app_db.br_link[i].a2dp_track_handle);
#endif

#if F_APP_GAMING_CHAT_MIXING_SUPPORT

#else
                app_db.br_link[i].avrcp_play_status = BT_AVRCP_PLAY_STATUS_PAUSED;
                play_pause = true;
                // Stop Sniffing Now
                app_bt_sniffing_stop(app_db.br_link[i].bd_addr, BT_SNIFFING_TYPE_A2DP);
#endif
            }

            APP_PRINT_TRACE4("app_multi_pause_inactive_a2dp_link_stream: streaming %d play_status %d keep_active_a2dp_idx %d wait_resume_a2dp_idx %d",
                             app_db.br_link[i].streaming_fg, app_db.br_link[i].avrcp_play_status, keep_active_a2dp_idx,
                             app_db.wait_resume_a2dp_idx);
        }
    }

    return play_pause;
}

void app_multi_resume_a2dp_link_stream(uint8_t app_idx)
{
    if (app_db.wait_resume_a2dp_idx < MAX_BR_LINK_NUM)
    {
        //if (app_db.wait_resume_a2dp_idx != app_idx)
        {
            if (app_db.br_link[app_db.wait_resume_a2dp_idx].connected_profile &
                (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK))
            {
                app_multi_a2dp_active_link_set(app_db.br_link[app_db.wait_resume_a2dp_idx].bd_addr);
                app_bond_set_priority(app_db.br_link[app_db.wait_resume_a2dp_idx].bd_addr);
                if (app_db.br_link[app_db.wait_resume_a2dp_idx].avrcp_play_status !=
                    BT_AVRCP_PLAY_STATUS_PLAYING)
                {
                    bool play = true;

#if F_APP_GAMING_DONGLE_SUPPORT
                    if (app_link_check_dongle_link(app_db.br_link[app_db.wait_resume_a2dp_idx].bd_addr))
                    {
                        play = false;
                    }
#endif
                    if (play)
                    {
                        app_db.br_link[app_db.wait_resume_a2dp_idx].avrcp_play_status =
                            BT_AVRCP_PLAY_STATUS_PLAYING;
                        app_db.br_link[app_db.wait_resume_a2dp_idx].avrcp_ready_to_pause = true;
                        bt_avrcp_play(app_db.br_link[app_db.wait_resume_a2dp_idx].bd_addr);
                    }
                }
            }
        }
        //all sco are removed, the variable must be clear
        app_db.wait_resume_a2dp_idx = MAX_BR_LINK_NUM;
    }
    APP_PRINT_TRACE4("app_multi_resume_a2dp_link_stream: active_a2dp_idx %d, wait_resume_a2dp_idx %d, "
                     "app_idx %d, avrcp_play_status 0x%02x",
                     app_a2dp_get_active_idx(), app_db.wait_resume_a2dp_idx, app_idx,
                     app_db.br_link[app_a2dp_get_active_idx()].avrcp_play_status);
}

void app_multi_handle_sniffing_link_disconnect_event(uint8_t id)
{
    APP_PRINT_TRACE6("app_multi_handle_sniffing_link_disconnect_event:  %d, %d, %d, %d, %d, %d",
                     sco_wait_from_sco_sniffing_stop, pending_sco_index_from_sco,
                     app_db.sco_wait_a2dp_sniffing_stop, app_db.pending_sco_idx,
                     app_db.a2dp_wait_a2dp_sniffing_stop, app_db.pending_a2dp_idx);

    if (active_hfp_transfer != 0xFF)
    {
        app_hfp_set_active_idx(app_db.br_link[active_hfp_transfer].bd_addr);
        app_bond_set_priority(app_db.br_link[active_hfp_transfer].bd_addr);
        if (app_db.br_link[active_hfp_transfer].sco_handle)
        {
            if (!app_bt_sniffing_start(app_db.br_link[active_hfp_transfer].bd_addr,
                                       BT_SNIFFING_TYPE_SCO))
            {

            }
        }
        else if (app_db.br_link[active_hfp_transfer].streaming_fg) //hfp data go a2dp
        {
            if (app_bt_sniffing_start(app_db.br_link[active_hfp_transfer].bd_addr,
                                      BT_SNIFFING_TYPE_A2DP)) {}
        }
        else if (sco_wait_from_sco_sniffing_stop == true)
        {
            app_bt_sniffing_hfp_connect(app_db.br_link[pending_sco_index_from_sco].bd_addr);
        }
        sco_wait_from_sco_sniffing_stop = false;
        pending_sco_index_from_sco = 0xFF;
        active_hfp_transfer = 0xFF;

        for (int inactive_hf_index = 0; inactive_hf_index < MAX_BR_LINK_NUM; inactive_hf_index++)
        {
            if (inactive_hf_index != active_hfp_transfer)
            {
                if (app_db.br_link[inactive_hf_index].call_status >= BT_HFP_CALL_ACTIVE &&
                    app_db.br_link[inactive_hf_index].streaming_fg == false)
                {
                    sco_wait_from_sco_sniffing_stop = true;
                    pending_sco_index_from_sco = inactive_hf_index;
                    break;
                }
            }
        }
    }
    else if (sco_wait_from_sco_sniffing_stop == true && id != pending_sco_index_from_sco)
    {
        app_bt_sniffing_hfp_connect(app_db.br_link[pending_sco_index_from_sco].bd_addr);
        sco_wait_from_sco_sniffing_stop = false;
        pending_sco_index_from_sco = 0xFF;
        active_hfp_transfer = 0xFF;
    }
    else if (app_db.sco_wait_a2dp_sniffing_stop == true)
    {
        if (!app_bt_sniffing_start(app_db.br_link[app_db.pending_sco_idx].bd_addr, BT_SNIFFING_TYPE_SCO))
        {
            bt_hfp_audio_connect_cfm(app_db.br_link[app_db.pending_sco_idx].bd_addr, true);
        }
        app_db.sco_wait_a2dp_sniffing_stop = false;
    }
    else if (app_db.a2dp_wait_a2dp_sniffing_stop == true)
    {
        if (app_bt_sniffing_start(app_db.br_link[app_db.pending_a2dp_idx].bd_addr,
                                  BT_SNIFFING_TYPE_A2DP)) {};
        app_db.a2dp_wait_a2dp_sniffing_stop = false;

        app_audio_set_bud_stream_state(BUD_STREAM_STATE_AUDIO);
    }
#if F_APP_MUTILINK_VA_PREEMPTIVE
    else if (va_wait_from_va_sniffing_stop == true &&
             (pending_va_sniffing_type == BT_SNIFFING_TYPE_SCO ||
              pending_va_sniffing_type == BT_SNIFFING_TYPE_A2DP))
    {
        APP_PRINT_TRACE2("app_multi_handle_sniffing_link_disconnect_event: va_wait_va %d %d",
                         pending_va_index_from_va,
                         pending_va_sniffing_type);
        if (pending_va_sniffing_type == BT_SNIFFING_TYPE_SCO)
        {
            if (!app_bt_sniffing_start(app_db.br_link[pending_va_index_from_va].bd_addr,
                                       BT_SNIFFING_TYPE_SCO))
            {

            }
        }
        else if (pending_va_sniffing_type == BT_SNIFFING_TYPE_A2DP)
        {
            if (app_bt_sniffing_start(app_db.br_link[pending_va_index_from_va].bd_addr,
                                      BT_SNIFFING_TYPE_A2DP)) {}

            app_audio_set_bud_stream_state(BUD_STREAM_STATE_AUDIO);
        }
        va_wait_from_va_sniffing_stop = false;
        pending_va_index_from_va = 0xFF;
        pending_va_sniffing_type = 0xFF;
    }
#endif
    else
    {
        // Check what ???
        // will cause single link problem: https://jira.realtek.com/browse/BBPRO2BUG-3827
        if ((app_db.br_link[app_hfp_get_active_idx()].sco_handle != NULL) &&
            ((app_cfg_const.enable_rtk_charging_box && !app_db.remote_case_closed) ||
             (!app_cfg_const.enable_rtk_charging_box && app_db.remote_loc != BUD_LOC_IN_CASE)))
        {
            if (!app_bt_sniffing_start(app_db.br_link[app_hfp_get_active_idx()].bd_addr,
                                       BT_SNIFFING_TYPE_SCO))
            {
                APP_PRINT_TRACE0("app_multi_handle_sniffing_link_disconnect_event: app_bt_sniffing_start error");
            }
        }
        else
        {
            app_multi_judge_active_a2dp_idx_and_qos(app_a2dp_get_active_idx(), JUDGE_EVENT_SNIFFING_STOP);
        }
        if (sco_wait_from_sco_sniffing_stop == true && id != pending_sco_index_from_sco)
        {
            sco_wait_from_sco_sniffing_stop = false;
            pending_sco_index_from_sco = 0xFF;
        }
#if F_APP_MUTILINK_VA_PREEMPTIVE
        va_wait_from_va_sniffing_stop = false;
        pending_va_index_from_va = 0xFF;
        pending_va_sniffing_type = 0xFF;
#endif
    }

#if CONFIG_REALTEK_GFPS_SASS_SUPPORT
    if (extend_app_cfg_const.gfps_sass_support)
    {
        app_gfps_notify_conn_status();
    }
#endif
}

bool app_multi_check_pend(void)
{
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();

    if (app_db.br_link[active_a2dp_idx].streaming_fg == false &&
        app_db.br_link[active_a2dp_idx].bt_sniffing_state == APP_BT_SNIFFING_STATE_STARTING)
    {
        return false;
    }
    return true;
}

void app_multi_update_a2dp_play_status(T_APP_JUDGE_A2DP_EVENT event)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        if (event == JUDGE_EVENT_A2DP_START)
        {
            app_audio_a2dp_play_status_update(APP_A2DP_STREAM_A2DP_START);

            if (app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off)
            {
                app_auto_power_off_disable(AUTO_POWER_OFF_MASK_AUDIO);
            }
        }
        else if (event == JUDGE_EVENT_A2DP_STOP)
        {
            uint8_t idx;

            for (idx = 0; idx < MAX_BR_LINK_NUM; idx++)
            {
                if (app_db.br_link[idx].streaming_fg)
                {
                    break;
                }
            }

            if (idx == MAX_BR_LINK_NUM) // all links no streaming
            {
                app_audio_a2dp_play_status_update(APP_A2DP_STREAM_A2DP_STOP);

                if (app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off)
                {
                    app_auto_power_off_enable(AUTO_POWER_OFF_MASK_AUDIO,
                                              app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off);
                }
            }
        }
    }
}

void app_multi_judge_active_a2dp_idx_and_qos(uint8_t app_idx, T_APP_JUDGE_A2DP_EVENT event)
{
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();
    uint8_t active_hf_idx = app_hfp_get_active_idx();

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
    app_teams_multilink_music_priority_rearrangment(app_idx, event);

#elif F_APP_MUTLILINK_SOURCE_PRIORITY_UI
    app_multilink_customer_music_event_handle(app_idx, event);

#else

    switch (event)
    {
    case JUDGE_EVENT_A2DP_CONNECTED:
        {
            uint8_t link_number = app_link_get_b2s_link_num_with_profile(A2DP_PROFILE_MASK);
            if (link_number <= 1)
            {
                app_multi_a2dp_active_link_set(app_db.br_link[app_idx].bd_addr);
                app_bond_set_priority(app_db.br_link[app_idx].bd_addr);
                if (link_number <= 0)
                {
                    //exception
                }
            }
            else
            {
                if ((app_db.br_link[active_a2dp_idx].streaming_fg == false) &&
                    (app_hfp_get_call_status() == APP_CALL_IDLE))
                {
                    if (app_cfg_const.enable_multi_link)
                    {
                        app_bond_set_priority(app_db.br_link[app_idx].bd_addr);
                        app_bond_set_priority(app_db.br_link[app_multi_find_new_active_a2dp_link_by_priority(
                                                                 app_idx)].bd_addr);
                    }
                    else
                    {
                        app_multi_a2dp_active_link_set(app_db.br_link[app_idx].bd_addr);
                        app_bond_set_priority(app_db.br_link[app_idx].bd_addr);
                    }
                }
            }
        }
        break;

    case JUDGE_EVENT_A2DP_START:
        {
            APP_PRINT_TRACE3("JUDGE_EVENT_A2DP_START: active_a2dp_idx %d, avrcp %d, stream %d",
                             active_a2dp_idx,
                             app_db.br_link[active_a2dp_idx].avrcp_play_status,
                             app_db.br_link[active_a2dp_idx].streaming_fg);
            if (app_multi_allow_a2dp_sniffing(app_idx))
            {
#if F_APP_MALLEUS_SUPPORT
#if F_APP_SENSOR_MEMS_SUPPORT
#if F_APP_MALLEUS_TWS_SPATIAL_AUDIO == 0
                app_sensor_mems_start(app_idx);
#endif
#endif
#endif
                if (active_a2dp_idx != app_idx)
                {
                    if (app_db.update_active_a2dp_idx == false)
                    {
                        if (app_cfg_const.disable_multilink_preemptive == 0)
                        {
                            APP_PRINT_TRACE3("JUDGE_EVENT_A2DP_START: preemptive app_idx %d, avrcp_play_status %d, stream %d",
                                             app_idx,
                                             app_db.br_link[app_idx].avrcp_play_status,
                                             app_db.br_link[app_idx].streaming_fg);

                            if (app_multi_check_pend()
#if F_APP_GAMING_CHAT_MIXING_SUPPORT
                                || (app_db.remote_is_dongle)
#endif
                               )
                            {
                                //ios: a2dp streaming + avrcp playing both satisfied -> preemptive
                                //others: a2dp streaming satisfied -> preemptive
                                if ((app_db.br_link[app_idx].remote_device_vendor_id == APP_REMOTE_DEVICE_IOS) &&
                                    ((app_db.br_link[app_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING) ||
#if F_APP_DURIAN_SUPPORT&F_APP_MULTI_ALLOW_STREAM_ONLY_PREEMPTIVE
                                     (app_db.br_link[app_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PAUSED) ||
#endif
                                     (timer_idx_multilink_silence_packet_detect != 0) ||
                                     (app_idx == active_hf_idx && app_hfp_get_call_status() != APP_CALL_IDLE) ||
                                     ((app_db.br_link[active_a2dp_idx].streaming_fg == false) &&
                                      (app_db.br_link[app_idx].streaming_fg == true) &&
                                      (app_db.br_link[app_idx].avrcp_play_status != BT_AVRCP_PLAY_STATUS_PLAYING))) ||
                                    (app_db.br_link[app_idx].remote_device_vendor_id != APP_REMOTE_DEVICE_IOS))
                                {
                                    app_stop_timer(&timer_idx_multilink_silence_packet_detect);
                                    app_multi_pause_inactive_a2dp_link_stream(app_idx, false);
                                    app_multi_stream_avrcp_set(app_idx);

                                    if (!app_bt_sniffing_start(app_db.br_link[active_a2dp_idx].bd_addr, BT_SNIFFING_TYPE_A2DP)) {};
                                }
                            }
                        }
                        else
                        {
                            if ((app_db.br_link[active_a2dp_idx].streaming_fg == false) && app_multi_check_pend())
                            {
                                app_a2dp_set_active_idx(app_idx);
                                app_multi_a2dp_active_link_set(app_db.br_link[app_idx].bd_addr);
                                app_bond_set_priority(app_db.br_link[app_idx].bd_addr);
                            }
                            else
                            {
                                if (app_db.active_media_paused)
                                {
                                    // A2DP Paused
                                    app_db.a2dp_wait_a2dp_sniffing_stop = true;
                                    app_db.pending_a2dp_idx = app_idx;
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (app_db.br_link[app_idx].avrcp_play_status != BT_AVRCP_PLAY_STATUS_PLAYING)
                    {
                        app_db.br_link[app_idx].avrcp_play_status = BT_AVRCP_PLAY_STATUS_PLAYING;
#if F_APP_MUTILINK_VA_PREEMPTIVE
#if F_APP_DURIAN_SUPPORT
                        stream_only[app_idx] = (pre_avrcp[app_idx] != BT_AVRCP_PLAY_STATUS_PLAYING);
#else
                        stream_only[app_idx] = true;
#endif
#endif
                    }

                    if ((app_cfg_const.enable_multi_link) &&
                        (app_link_get_b2s_link_num() == MULTILINK_SRC_CONNECTED))
                    {
                        app_multi_pause_inactive_a2dp_link_stream(app_idx, false);
                    }
                }
            }
            else
            {
                if (app_cfg_const.enable_multi_sco_disc_resume)
                {
                    app_multi_pause_inactive_a2dp_link_stream(active_hf_idx, true);
                }
                else
                {
                    app_multi_pause_inactive_a2dp_link_stream(active_hf_idx, false);
                }
            }

            app_bt_policy_b2s_tpoll_update(app_db.br_link[app_idx].bd_addr, BP_TPOLL_EVENT_A2DP_START);
        }
        break;

    case JUDGE_EVENT_A2DP_DISC:
        {
            if (active_a2dp_idx == app_idx)
            {
#if F_APP_MALLEUS_SUPPORT
#if F_APP_SENSOR_MEMS_SUPPORT
#if F_APP_MALLEUS_TWS_SPATIAL_AUDIO == 0
                app_sensor_mems_stop(active_a2dp_idx);
#endif
#endif
#endif
                if (app_multi_check_in_sniffing())
                {
                    app_bt_sniffing_stop(app_db.br_link[app_idx].bd_addr, BT_SNIFFING_TYPE_A2DP);
                }
#if CONFIG_REALTEK_GFPS_SASS_SUPPORT
                else if ((extend_app_cfg_const.gfps_sass_support) && (app_idx == sass_active_idx))
                {
                    app_multi_handle_sniffing_link_disconnect_event(app_idx);
                }
#endif
                if (app_link_get_b2s_link_num_with_profile(A2DP_PROFILE_MASK))
                {
                    app_multi_a2dp_active_link_set(app_db.br_link[app_multi_find_new_active_a2dp_link_by_priority(
                                                                      app_idx)].bd_addr);
                    app_bond_set_priority(app_db.br_link[app_multi_find_new_active_a2dp_link_by_priority(
                                                             app_idx)].bd_addr);
                }
            }
            app_bt_policy_b2s_tpoll_update(app_db.br_link[app_idx].bd_addr, BP_TPOLL_EVENT_A2DP_STOP);
        }
        break;

    case JUDGE_EVENT_A2DP_STOP:
        {
            if (app_db.update_active_a2dp_idx == false)
            {
                app_db.br_link[app_idx].avrcp_play_status = BT_AVRCP_PLAY_STATUS_PAUSED;
#if F_APP_MUTILINK_VA_PREEMPTIVE
                stream_only[app_idx] = false;
                if (va_wait_from_va_sniffing_stop && pending_va_index_from_va == app_idx)
                {
                    va_wait_from_va_sniffing_stop = false;
                    pending_va_index_from_va = 0xFF;
                    pending_va_sniffing_type = 0xFF;
                }
#endif

#if F_APP_MALLEUS_SUPPORT
#if F_APP_SENSOR_MEMS_SUPPORT
#if F_APP_MALLEUS_TWS_SPATIAL_AUDIO == 0
                if (active_a2dp_idx == app_idx)
                {
                    app_sensor_mems_stop(active_a2dp_idx);
                }
#endif
#endif
#endif
                if ((active_a2dp_idx == app_idx) && (app_db.wait_resume_a2dp_idx == MAX_BR_LINK_NUM))
                {
                    app_stop_timer(&timer_idx_multilink_silence_packet_judge);
                    app_stop_timer(&timer_idx_multilink_silence_packet_detect);
                    uint8_t idx = app_multi_find_new_active_a2dp_link_by_priority(active_a2dp_idx);
                    uint8_t i;
                    for (i = 0; i < MAX_BR_LINK_NUM; i++)
                    {
                        if (app_db.br_link[idx].connected_profile &
                            (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK))
                        {
                            if ((idx != active_a2dp_idx) &&
                                (app_db.br_link[idx].streaming_fg == true))
                            {
                                APP_PRINT_TRACE2("JUDGE_EVENT_A2DP_STOP: active_a2dp_idx %d, idx %d", active_a2dp_idx, idx);
                                app_multi_stream_avrcp_set(idx);
                                break;
                            }
                        }
                    }
                }
                /*  a2dp pause to resume
                else if ((active_a2dp_idx == app_idx) && (app_db.wait_resume_a2dp_idx != active_a2dp_idx))
                {
                    app_multi_a2dp_active_link_set(app_db.br_link[app_db.wait_resume_a2dp_idx].bd_addr);
                    APP_PRINT_TRACE2("JUDGE_EVENT_A2DP_STOP: active_a2dp_idx %d, wait_resume_a2dp_idx %d",
                                     active_a2dp_idx, app_db.wait_resume_a2dp_idx);
                    app_db.active_media_paused = false;
                    app_a2dp_set_active_idx(app_db.wait_resume_a2dp_idx);
                    app_multi_resume_a2dp_link_stream(app_idx);
                    app_bond_set_priority(app_db.br_link[app_db.wait_resume_a2dp_idx].bd_addr);
                }*/
            }

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                //Clear a2dp preemptive flag if any link is disconnected during preemptive process.
                app_db.a2dp_preemptive_flag = false;

#if F_APP_ERWS_SUPPORT
                if (app_idx != active_a2dp_idx &&
                    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
                {
                    //preemp ends, htpoll param return to normal value
                    app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_MULTILINK_CHANGE);
                    app_bt_sniffing_judge_dynamic_set_gaming_mode();
                }

#if F_APP_QOL_MONITOR_SUPPORT
                if (app_a2dp_is_streaming() == false)
                {
                    //close b2b link monitor when no a2dp stream
                    app_qol_link_monitor(app_cfg_nv.bud_peer_addr, false);
                }
#endif
#endif
            }
            app_bt_policy_b2s_tpoll_update(app_db.br_link[app_idx].bd_addr, BP_TPOLL_EVENT_A2DP_STOP);
        }
        break;

    case JUDGE_EVENT_MEDIAPLAYER_PLAYING:
        {
            if (app_cfg_const.disable_multilink_preemptive)
            {
                if (app_multi_allow_a2dp_sniffing(app_idx))
                {
                    APP_PRINT_TRACE2("JUDGE_EVENT_MEDIAPLAYER_PLAYING: active_a2dp_idx %d, app_idx %d",
                                     active_a2dp_idx, app_idx);
                    if (active_a2dp_idx == app_idx)
                    {
                        app_db.active_media_paused = false;
                    }
                }
            }
            else
            {
                APP_PRINT_TRACE3("JUDGE_EVENT_MEDIAPLAYER_PLAYING: preemptive active_a2dp_idx %d, app_idx %d, streaming_fg %d",
                                 active_a2dp_idx, app_idx, app_db.br_link[app_idx].streaming_fg);
                if (app_db.br_link[app_idx].streaming_fg == true)
                {
                    if (app_hfp_get_call_status() != APP_CALL_IDLE)
                    {
                        if (app_cfg_const.enable_multi_sco_disc_resume)
                        {
                            app_multi_pause_inactive_a2dp_link_stream(active_hf_idx, true);
                        }
                        else
                        {
                            app_multi_pause_inactive_a2dp_link_stream(active_hf_idx, false);
                        }
                    }
                    else
                    {
                        app_multi_a2dp_active_link_set(app_db.br_link[app_idx].bd_addr);
                        app_multi_pause_inactive_a2dp_link_stream(app_idx, false);
                        app_bond_set_priority(app_db.br_link[app_idx].bd_addr);
                    }
                }
            }
            app_bt_policy_b2s_tpoll_update(app_db.br_link[app_idx].bd_addr, BP_TPOLL_EVENT_A2DP_START);
        }
        break;

    case JUDGE_EVENT_MEDIAPLAYER_PAUSED:
        {
            if (app_multi_allow_a2dp_sniffing(app_idx))
            {
                APP_PRINT_TRACE3("JUDGE_EVENT_MEDIAPLAYER_PAUSED: active_a2dp_idx %d, app_idx %d, streaming_fg %d",
                                 active_a2dp_idx, app_idx, app_db.br_link[active_a2dp_idx].streaming_fg);
                if (active_a2dp_idx == app_idx)
                {
                    app_db.active_media_paused = true;
                    ///TODO: mute Music ???
                    // if (app_db.br_link[active_a2dp_idx].streaming_fg == true)
                    // {
                    //     app_db.br_link[active_a2dp_idx].streaming_fg = false;
                    //     app_bt_sniffing_stop(app_db.br_link[active_a2dp_idx].bd_addr, BT_SNIFFING_TYPE_A2DP);
                    // }
                    if (app_db.wait_resume_a2dp_idx == MAX_BR_LINK_NUM)
                    {
                        uint8_t idx = app_multi_find_new_active_a2dp_link_by_priority(active_a2dp_idx);
                        if ((app_db.br_link[idx].connected_profile &
                             (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK)) && (idx != active_a2dp_idx))
                        {
#if F_APP_GAMING_DONGLE_SUPPORT
                            if (!app_cfg_const.enable_multi_link)
                            {
                                app_start_timer(&timer_idx_multilink_silence_packet_judge, "multilink_silence_packet_judge",
                                                multilink_timer_id, APP_TIMER_MULTILINK_SILENCE_PACKET_JUDGE, idx, false,
                                                750);
                                break;
                            }
#else
                            app_start_timer(&timer_idx_multilink_silence_packet_judge, "multilink_silence_packet_judge",
                                            multilink_timer_id, APP_TIMER_MULTILINK_SILENCE_PACKET_JUDGE, idx, false,
                                            750);
                            break;
#endif
                        }
                    }
                }
            }

            app_bt_policy_b2s_tpoll_update(app_db.br_link[app_idx].bd_addr, BP_TPOLL_EVENT_A2DP_STOP);
        }
        break;

    case JUDGE_EVENT_SNIFFING_STOP:
        {
#if F_APP_LEA_SUPPORT
            bool need_return = false;
#endif
            ///TODO: check Sniffing state for active link
            APP_PRINT_TRACE4("JUDGE_EVENT_SNIFFING_STOP: active_a2dp_idx %d, bd_addr %s, stream %d, remote_loc %d",
                             active_a2dp_idx,
                             TRACE_BDADDR(app_db.br_link[active_a2dp_idx].bd_addr),
                             app_db.br_link[active_a2dp_idx].streaming_fg,
                             app_db.remote_loc);
            uint8_t inactive_a2dp_idx = MAX_BR_LINK_NUM;
            for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if ((app_db.br_link[i].connected_profile & (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK)) &&
                    (i != active_a2dp_idx))
                {
                    inactive_a2dp_idx = i;
                    break;
                }
            }

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
            if (app_dongle_disallow_bt_streaming() && !app_cfg_const.enable_24g_bt_audio_source_switch)
            {
                break;
            }
#endif

#if F_APP_LEA_SUPPORT
            if ((mtc_if_fm_ml_handle(ML_TO_MTC_CH_SNIFFING, NULL, &need_return) == MTC_RESULT_SUCCESS) &&
                need_return)
            {
                APP_PRINT_TRACE0("JUDGE_EVENT_SNIFFING_STOP: STOP SNIFFING");
                le_sniffing_stop = true;
                break;
            }
#endif
            if ((app_db.br_link[active_a2dp_idx].streaming_fg) &&
                ((app_cfg_const.enable_rtk_charging_box && !app_db.remote_case_closed) ||
                 (!app_cfg_const.enable_rtk_charging_box && app_db.remote_loc != BUD_LOC_IN_CASE)))
            {
                app_audio_set_bud_stream_state(BUD_STREAM_STATE_AUDIO);

#if F_APP_MUTILINK_VA_PREEMPTIVE
                if (app_multi_allow_a2dp_sniffing(active_a2dp_idx))
                {

#if F_APP_QOL_MONITOR_SUPPORT
                    int8_t b2b_rssi = 0;
                    app_qol_get_aggregate_rssi(true, &b2b_rssi);

                    if (((b2b_rssi != 0) && (b2b_rssi > B2B_RSSI_THRESHOLD_START_SNIFFING)) || (!app_db.sec_going_away))
                    {
                        if (app_bt_sniffing_start(app_db.br_link[active_a2dp_idx].bd_addr, BT_SNIFFING_TYPE_A2DP)) {};
                    }
#else
                    if (app_bt_sniffing_start(app_db.br_link[active_a2dp_idx].bd_addr, BT_SNIFFING_TYPE_A2DP)) {};
#endif

                    if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        if (app_db.br_link[active_a2dp_idx].a2dp_track_handle)
                        {
                            audio_track_start(app_db.br_link[active_a2dp_idx].a2dp_track_handle);
                        }
                    }
                }
#else

#if F_APP_QOL_MONITOR_SUPPORT
                int8_t b2b_rssi = 0;
                app_qol_get_aggregate_rssi(true, &b2b_rssi);

                if (((b2b_rssi != 0) && (b2b_rssi > B2B_RSSI_THRESHOLD_START_SNIFFING)) || (!app_db.sec_going_away))
                {
                    if (app_bt_sniffing_start(app_db.br_link[active_a2dp_idx].bd_addr, BT_SNIFFING_TYPE_A2DP)) {};
                }
#else
                if (app_bt_sniffing_start(app_db.br_link[active_a2dp_idx].bd_addr, BT_SNIFFING_TYPE_A2DP)) {};
#endif

#endif
            }
            else if (app_db.br_link[active_a2dp_idx].streaming_fg == false)
            {
                if (inactive_a2dp_idx == MAX_BR_LINK_NUM)
                {
                }
                else if (app_db.br_link[inactive_a2dp_idx].streaming_fg == false)
                {
                    APP_PRINT_TRACE3("JUDGE_EVENT_SNIFFING_STOP: inactive_a2dp_idx %d, bd_addr %s, stream %d",
                                     inactive_a2dp_idx,
                                     TRACE_BDADDR(app_db.br_link[inactive_a2dp_idx].bd_addr),
                                     app_db.br_link[inactive_a2dp_idx].streaming_fg);
                }
#if F_APP_MUTILINK_VA_PREEMPTIVE
                else if (app_db.br_link[inactive_a2dp_idx].streaming_fg == true)
                {
                    if (app_db.remote_loc != BUD_LOC_IN_CASE)
                    {
                        app_multi_stream_avrcp_set(inactive_a2dp_idx);

                        app_audio_set_bud_stream_state(BUD_STREAM_STATE_AUDIO);
                        if (app_multi_allow_a2dp_sniffing(inactive_a2dp_idx))
                        {
                            if (app_bt_sniffing_start(app_db.br_link[inactive_a2dp_idx].bd_addr, BT_SNIFFING_TYPE_A2DP)) {};
                        }
                    }
                }
#endif
            }
        }
        break;

    default:
        break;
    }

    app_multi_update_a2dp_play_status(event);

    if (LIGHT_SENSOR_ENABLED && app_db.detect_suspend_by_out_ear == true &&
        app_db.a2dp_play_status == true)
    {
        app_audio_update_detect_suspend_by_out_ear(false);
    }
#endif
}

static void app_multi_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;
    T_APP_BR_LINK *p_link = NULL;
    T_BT_EVENT_PARAM *param = event_buf;
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();
    uint8_t active_hf_idx = app_hfp_get_active_idx();

    switch (event_type)
    {
    case BT_EVENT_SCO_CONN_IND:
        {
#ifdef ERWS_MULTILINK_SUPPORT
            p_link = app_link_find_br_link(param->sco_conn_ind.bd_addr);
            if (p_link != NULL)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    app_hfp_adjust_sco_window(param->sco_conn_ind.bd_addr, APP_SCO_ADJUST_SCO_CONN_IND_EVT);
                    if (app_cfg_const.enable_multi_sco_disc_resume)
                    {
#if F_APP_MUTILINK_VA_PREEMPTIVE
                        if (va_wait_from_va_sniffing_stop && pending_va_index_from_va == p_link->id)
                        {
                            pending_va_sniffing_type = BT_SNIFFING_TYPE_SCO;
                        }
                        if (app_multi_pause_inactive_a2dp_link_stream(p_link->id, true) && app_multi_check_in_sniffing())
#else
                        if (app_multi_pause_inactive_a2dp_link_stream(p_link->id, true))
#endif
                        {
                            // A2DP Paused
                            app_db.sco_wait_a2dp_sniffing_stop = true;
                            app_db.pending_sco_idx = p_link->id;
                        }
                        else
                        {
                            // No A2DP
                            if (!app_bt_sniffing_start(param->sco_conn_ind.bd_addr, BT_SNIFFING_TYPE_SCO))
                            {
                                p_link = app_link_find_br_link(param->sco_conn_ind.bd_addr);

                                if (p_link != NULL)
                                {
#if (F_APP_LEA_SUPPORT && !F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST)
                                    bool need_return = false;
                                    if (mtc_if_fm_ml_handle(ML_TO_MTC_CH_LEA_CALL, NULL, &need_return) == MTC_RESULT_SUCCESS)
                                    {
                                        if (need_return)
                                        {
                                            bt_hfp_audio_connect_cfm(param->sco_conn_ind.bd_addr, false);
                                            break;
                                        }
                                    }
#endif
                                    bt_hfp_audio_connect_cfm(param->sco_conn_ind.bd_addr, true);
                                }
                            }
                        }
                    }
                    else
                    {
#if F_APP_MUTILINK_VA_PREEMPTIVE
                        if (va_wait_from_va_sniffing_stop && pending_va_index_from_va == p_link->id)
                        {
                            pending_va_sniffing_type = BT_SNIFFING_TYPE_SCO;
                        }
                        if (app_multi_pause_inactive_a2dp_link_stream(p_link->id, false) && app_multi_check_in_sniffing())
#else
                        if (app_multi_pause_inactive_a2dp_link_stream(p_link->id, false))
#endif
                        {
                            // A2DP Paused
                            app_db.sco_wait_a2dp_sniffing_stop = true;
                            app_db.pending_sco_idx = p_link->id;
                        }
                        else
                        {
                            // No A2DP
                            if (!app_bt_sniffing_start(param->sco_conn_ind.bd_addr, BT_SNIFFING_TYPE_SCO))
                            {
                                p_link = app_link_find_br_link(param->sco_conn_ind.bd_addr);

                                if (p_link != NULL)
                                {
#if (F_APP_LEA_SUPPORT && !F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST)
                                    bool need_return = false;
                                    if (mtc_if_fm_ml_handle(ML_TO_MTC_CH_LEA_CALL, NULL, &need_return) == MTC_RESULT_SUCCESS)
                                    {
                                        if (need_return)
                                        {
                                            bt_hfp_audio_connect_cfm(param->sco_conn_ind.bd_addr, false);
                                            break;
                                        }
                                    }
#endif
                                    bt_hfp_audio_connect_cfm(param->sco_conn_ind.bd_addr, true);
                                }
                            }
                        }
                    }
                }
                else
                {

#if (F_APP_LEA_SUPPORT && !F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST)
                    bool need_return = false;
                    if (mtc_if_fm_ml_handle(ML_TO_MTC_CH_LEA_CALL, NULL, &need_return) == MTC_RESULT_SUCCESS)
                    {
                        if (need_return)
                        {
                            bt_hfp_audio_connect_cfm(param->sco_conn_ind.bd_addr, false);
                            break;
                        }
                    }
#endif
                    bt_hfp_audio_connect_cfm(param->sco_conn_ind.bd_addr, true);
                }
            }

#else
            if (!app_bt_sniffing_start(param->sco_conn_ind.bd_addr, BT_SNIFFING_TYPE_SCO))
            {
                p_link = app_link_find_br_link(param->sco_conn_ind.bd_addr);

                if (p_link != NULL)
                {
#if (F_APP_LEA_SUPPORT && !F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST)
                    bool need_return = false;
                    if (mtc_if_fm_ml_handle(ML_TO_MTC_CH_LEA_CALL, NULL, &need_return) == MTC_RESULT_SUCCESS)
                    {
                        if (need_return)
                        {
                            bt_hfp_audio_connect_cfm(param->sco_conn_ind.bd_addr, false);
                            break;
                        }
                    }
#endif
                    bt_hfp_audio_connect_cfm(param->sco_conn_ind.bd_addr, true);
                }
            }
#endif
        }
        break;

    case BT_EVENT_SCO_CONN_RSP:
        {
        }
        break;

    case BT_EVENT_SCO_CONN_CMPL:
        {
            p_link = app_link_find_br_link(param->sco_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                if (param->sco_conn_cmpl.cause == 0)
                {
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                    //if (!p_link->call_status)
                    //{
                    app_teams_multilink_handle_bt_stream_event(p_link->id, true);
                    //app_teams_multilink_high_app_action_trigger_low_app_action(p_link->id, T_APP_TEAMS_MULTILINK_HIGH_PRIORITY_APP_START);
                    //}
#else
#if F_APP_GAMING_DONGLE_SUPPORT
                    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE) &&
                        (app_cfg_const.enable_multi_link) &&
                        (app_link_check_dongle_link(app_db.br_link[active_hf_idx].bd_addr)))
                    {
                        APP_PRINT_TRACE0("BT_EVENT_SCO_CONN_CMPL: PAUSE SCO_CONN");
                        gap_br_vendor_set_active_sco(p_link->sco_handle, 0, 2);
                    }
                    else if (app_link_get_sco_conn_num() == 1)
#else
                    if (app_link_get_sco_conn_num() == 1)
#endif
                    {
                        bt_sco_link_switch(param->sco_conn_cmpl.bd_addr);
                    }
                    else
                    {
                        APP_PRINT_TRACE2("app_multi_bt_cback: active sco link %s, current link %s",
                                         TRACE_BDADDR(app_db.br_link[active_hf_idx].bd_addr),
                                         TRACE_BDADDR(param->sco_conn_cmpl.bd_addr));

                        bt_sco_link_switch(app_db.br_link[active_hf_idx].bd_addr);
                    }

                    if (app_cfg_const.enable_multi_sco_disc_resume)
                    {
                        app_multi_pause_inactive_a2dp_link_stream(p_link->id, true);
                    }
                    else
                    {
                        app_multi_pause_inactive_a2dp_link_stream(p_link->id, false);
                    }

#if F_APP_MUTILINK_VA_PREEMPTIVE
                    if (p_link->call_status == APP_VOICE_ACTIVATION_ONGOING)
                    {
                        if (p_link->id != active_va_idx ||
                            ((active_hf_idx != p_link->id) &&
                             (p_link->sco_handle) &&
                             (app_db.br_link[active_hf_idx].call_status != APP_VOICE_ACTIVATION_ONGOING)))
                        {
#if F_APP_MUTILINK_ALLOW_TWO_SCO
#else
                            bt_hfp_voice_recognition_disable_req(p_link->bd_addr);
                            app_stop_timer(&timer_idx_multilink_disc_va_sco);
                            app_start_timer(&timer_idx_multilink_disc_va_sco, "multilink_disc_va_sco",
                                            multilink_timer_id, APP_TIMER_MULTILINK_DISC_VA_SCO, p_link->id, false,
                                            1500);
#endif
                        }
                    }
                    else if ((active_hf_idx != p_link->id) && (p_link->sco_handle))
                    {
#if F_APP_MUTILINK_ALLOW_TWO_SCO
#else
                        if (app_db.br_link[p_link->id].call_status != APP_CALL_INCOMING &&
                            p_link->id != active_hfp_transfer)
                        {
                            APP_PRINT_TRACE1("app_multi_bt_cback: dis inactive sco link %s", TRACE_BDADDR(p_link->bd_addr));
                            bt_hfp_audio_disconnect_req(p_link->bd_addr);
                            if (app_cfg_const.enable_multi_link)
                            {
                                sco_wait_from_sco_sniffing_stop = true;
                                pending_sco_index_from_sco = p_link->id;
                            }
                        }
#endif
                    }
#else
#if F_APP_MUTILINK_ALLOW_TWO_SCO
#else
                    if ((active_hf_idx != p_link->id) && (p_link->sco_handle))
                    {
                        bt_hfp_audio_disconnect_req(p_link->bd_addr);
                        sco_wait_from_sco_sniffing_stop = true;
                        pending_sco_index_from_sco = p_link->id;
                    }
#endif
#endif
#endif
                }
                else
                {
                    if ((active_hf_idx != p_link->id))
                    {
                        sco_wait_from_sco_sniffing_stop = true;
                        pending_sco_index_from_sco = p_link->id;
                    }
                }
            }
            if (app_cfg_const.always_play_hf_incoming_tone_when_incoming_call)
            {
                app_hfp_inband_tone_mute_ctrl();
            }

            app_bt_policy_primary_engage_action_adjust();

#if F_APP_ROLESWITCH_WHEN_SCO_CHANGE
            app_bt_policy_roleswitch_when_sco(app_db.br_link[app_multi_find_inacitve(active_hf_idx)].bd_addr);
#endif
        }
        break;

    case BT_EVENT_SCO_DISCONNECTED:
        {
            if (param->sco_disconnected.cause == (HCI_ERR | HCI_ERR_CMD_DISALLOWED))
            {
                break;
            }

            app_bt_sniffing_stop(param->sco_disconnected.bd_addr, BT_SNIFFING_TYPE_SCO);

            p_link = app_link_find_br_link(param->sco_disconnected.bd_addr);
            if (p_link != NULL)
            {

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                app_teams_multilink_handle_bt_stream_event(p_link->id, false);
                //if (app_teams_multiink_get_record_status_by_link_id(p_link->id))
                //{
                //    app_teams_multilink_record_priority_rearrangment(p_link->id, false);
                //}
#else

                //if (p_link->id == active_hf_idx)
                {
                    uint8_t i;

                    //set actvie sco
                    for (i = 0; i < MAX_BR_LINK_NUM; i++)
                    {
                        if (app_db.br_link[i].sco_handle)
                        {
                            bt_sco_link_switch(app_db.br_link[i].bd_addr);
                            break;
                        }
                    }
                }

                if (active_hfp_transfer != 0xFF)
                {
                    if (!app_multi_check_in_sniffing())
                    {
                        app_multi_handle_sniffing_link_disconnect_event(p_link->id);
                    }
                }

                if (app_cfg_const.enable_multi_sco_disc_resume)
                {
                    if ((app_cfg_const.enable_multi_link) &&
                        (app_link_get_b2s_link_num() == MULTILINK_SRC_CONNECTED))
                    {
                        APP_PRINT_TRACE4("sco_disc_resume %d,%d,%d,%d", app_link_get_sco_conn_num(),
                                         active_hf_idx,
                                         p_link->id,
                                         app_hfp_get_call_status());
                        if ((app_link_get_sco_conn_num() == 0) &&
                            (app_hfp_get_call_status() == APP_CALL_IDLE))
                        {
#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
                            app_multilink_customer_sco_disconneted();
#else
                            app_multi_resume_a2dp_link_stream(p_link->id);
#endif
                        }
                    }
                }

#endif
#if F_APP_MUTILINK_VA_PREEMPTIVE
                if (p_link->id == pending_va_index_from_va)
                {
                    va_wait_from_va_sniffing_stop = false;
                    pending_va_index_from_va = 0xFF;
                    pending_va_sniffing_type = 0xFF;
                }
#endif
            }
            if (app_cfg_const.always_play_hf_incoming_tone_when_incoming_call)
            {
                app_hfp_inband_tone_mute_ctrl();
            }
            app_bt_policy_primary_engage_action_adjust();

#if F_APP_ROLESWITCH_WHEN_SCO_CHANGE
            app_bt_policy_roleswitch_when_sco(app_db.br_link[app_multi_find_inacitve(active_hf_idx)].bd_addr);
#endif
        }
        break;

    case BT_EVENT_A2DP_CONN_CMPL:
        {
            p_link = app_link_find_br_link(param->a2dp_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_CONNECTED);
            }
        }
        break;

    case BT_EVENT_A2DP_DISCONN_CMPL:
        {
            p_link = app_link_find_br_link(param->a2dp_disconn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_DISC);
            }
        }
        break;

    case BT_EVENT_AVRCP_PLAY_STATUS_RSP:
    case BT_EVENT_AVRCP_PLAY_STATUS_CHANGED:
        {
            p_link = app_link_find_br_link(param->avrcp_play_status_changed.bd_addr);

            if (p_link != NULL)
            {
#if F_APP_CHARGER_CASE_SUPPORT
                if (app_db.charger_case_connected &&
                    app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t evt_buf[8];

                    memcpy(&evt_buf[0], param->avrcp_play_status_changed.bd_addr, 6);
                    evt_buf[6] = param->avrcp_play_status_changed.play_status;
                    evt_buf[7] = param->a2dp_config_cmpl.codec_type;

                    app_report_event(CMD_PATH_LE, EVENT_PLAYER_STATUS, app_db.charger_case_link_id, evt_buf,
                                     sizeof(evt_buf));
                }
#endif
                APP_PRINT_TRACE2("BT_EVENT_AVRCP_PLAY_STATUS_CHANGED %d ,%d", p_link->id,
                                 param->avrcp_play_status_changed.play_status);

#if F_APP_DURIAN_SUPPORT
                pre_avrcp[p_link->id] = param->avrcp_play_status_changed.play_status;
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
#if F_APP_GAMING_CHAT_MIXING_SUPPORT
#else
                if (event_type == BT_EVENT_AVRCP_PLAY_STATUS_RSP &&
                    app_link_check_dongle_link(param->avrcp_play_status_changed.bd_addr))
                {
                    break;
                }
#endif
#endif

                if (param->avrcp_play_status_changed.play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
                {
                    app_audio_set_bud_stream_state(BUD_STREAM_STATE_AUDIO);
#if F_APP_MUTILINK_VA_PREEMPTIVE
                    stream_only[p_link->id] = false;
#endif
                    app_stop_timer(&timer_idx_multilink_silence_packet_judge);
                    app_stop_timer(&timer_idx_multilink_silence_packet_detect);
                    app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_MEDIAPLAYER_PLAYING);
                }
                else if (param->avrcp_play_status_changed.play_status == BT_AVRCP_PLAY_STATUS_PAUSED)
                {
                    app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_MEDIAPLAYER_PAUSED);
                }
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_START_IND:
        {
            p_link = app_link_find_br_link(param->a2dp_stream_start_ind.bd_addr);

#if F_APP_GAMING_CHAT_MIXING_SUPPORT
            if (app_dongle_reject_dongle_stream_start(param->a2dp_stream_start_ind.bd_addr))
            {
                break;
            }
#endif

#if F_APP_MALLEUS_SUPPORT
#if F_APP_SENSOR_MEMS_SUPPORT
#if F_APP_MALLEUS_TWS_SPATIAL_AUDIO == 0
            if (p_link != NULL)
            {
                app_sensor_mems_start(p_link->id);
            }
#endif
#endif
#endif
            if (app_cfg_const.rws_gaming_mode == 0)
            {
                if (p_link != NULL)
                {
                    APP_PRINT_TRACE5("app_multi_bt_cback: p_link %p, id %d, active id %d, streaming_fg %d / %d",
                                     p_link, p_link->id, active_a2dp_idx, p_link->streaming_fg,
                                     app_db.br_link[active_a2dp_idx].streaming_fg);
#if F_APP_MUTILINK_VA_PREEMPTIVE
                    uint8_t i;
                    for (i = 0; i < MAX_BR_LINK_NUM; i++)
                    {
                        if (app_link_check_b2s_link_by_id(i))
                        {
                            if (i != p_link->id)
                            {
                                break;
                            }
                        }
                    }

                    if ((app_cfg_const.enable_multi_link) &&
                        (i < MAX_BR_LINK_NUM) &&
                        (app_db.br_link[i].streaming_fg == true))
                    {
                        app_db.a2dp_preemptive_flag = true;
                        app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_MULTILINK_CHANGE);
                    }

                    if (va_wait_from_va_sniffing_stop && pending_va_index_from_va == p_link->id)
                    {
                        pending_va_sniffing_type = BT_SNIFFING_TYPE_A2DP;
                    }
#endif
                    if (p_link->id != active_a2dp_idx)
                    {
                        // not active link
                        if (app_db.br_link[active_a2dp_idx].streaming_fg == false)
                        {
#if F_APP_MUTILINK_VA_PREEMPTIVE
                            if (!app_multi_allow_a2dp_sniffing(p_link->id) ||
                                !app_bt_sniffing_start(param->a2dp_stream_start_ind.bd_addr, BT_SNIFFING_TYPE_A2DP))
#else
                            if (!app_bt_sniffing_start(param->a2dp_stream_start_ind.bd_addr, BT_SNIFFING_TYPE_A2DP))
#endif
                            {
                                bt_a2dp_stream_start_cfm(param->a2dp_stream_start_ind.bd_addr, true);
                                p_link->streaming_fg = true;
                                p_link->avrcp_ready_to_pause = true;
                                app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_START);
                            }

                            if (app_cfg_const.disable_multilink_preemptive)
                            {
                                app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_START);
                            }
                        }
                        else
                        {
                            bt_a2dp_stream_start_cfm(param->a2dp_stream_start_ind.bd_addr, true);
                            p_link->streaming_fg = true;
                            p_link->avrcp_ready_to_pause = true;
                            app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_START);
                        }
                    }
                    else
                    {
                        // active link
#if F_APP_MUTILINK_VA_PREEMPTIVE
                        if (p_link->streaming_fg == false && (va_wait_from_va_sniffing_stop ||
                                                              !app_multi_allow_a2dp_sniffing(p_link->id) ||
                                                              !app_bt_sniffing_start(param->a2dp_stream_start_ind.bd_addr, BT_SNIFFING_TYPE_A2DP)))
                        {
                            bt_a2dp_stream_start_cfm(param->a2dp_stream_start_ind.bd_addr, true);
                            p_link->streaming_fg = true;
                            p_link->avrcp_ready_to_pause = true;
                            app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_START);
                        }
#else
                        if (p_link->streaming_fg == false &&
                            !app_bt_sniffing_start(param->a2dp_stream_start_ind.bd_addr, BT_SNIFFING_TYPE_A2DP))
                        {
                            bt_a2dp_stream_start_cfm(param->a2dp_stream_start_ind.bd_addr, true);
                            p_link->streaming_fg = true;
                            p_link->avrcp_ready_to_pause = true;
                            app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_START);
                        }
#endif
                    }
                }
            }
            else
            {
                p_link = app_link_find_br_link(param->a2dp_stream_start_ind.bd_addr);
                if (p_link != NULL)
                {
                    if (bt_a2dp_stream_start_cfm(param->a2dp_stream_start_ind.bd_addr, true))
                    {
                        p_link->streaming_fg = true;
                        p_link->avrcp_ready_to_pause = true;
                    }
                    app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_START);
                }
            }
            app_bt_policy_update_cpu_freq(BP_CPU_FREQ_EVENT_A2DP_STREAM);
            app_bt_policy_primary_engage_action_adjust();
#if F_APP_DYNAMIC_ADJUST_B2B_TX_POWER
            app_bt_policy_dynamic_adjust_b2b_tx_power(BP_TX_POWER_A2DP_START);
#endif
        }
        break;

    case BT_EVENT_A2DP_STREAM_START_RSP:
        {
            p_link = app_link_find_br_link(param->a2dp_stream_start_rsp.bd_addr);
            if (p_link != NULL)
            {
                p_link->streaming_fg = true;
                p_link->avrcp_ready_to_pause = true;
                app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_START);
            }
            app_bt_policy_primary_engage_action_adjust();
#if F_APP_DYNAMIC_ADJUST_B2B_TX_POWER
            app_bt_policy_dynamic_adjust_b2b_tx_power(BP_TX_POWER_A2DP_START);
#endif
        }
        break;

    case BT_EVENT_A2DP_STREAM_STOP:
    case BT_EVENT_A2DP_STREAM_CLOSE:
        {
            p_link = app_link_find_br_link(param->a2dp_stream_stop.bd_addr);
            if (p_link != NULL)
            {
                p_link->streaming_fg = false;
                p_link->avrcp_ready_to_pause = false;
                app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_STOP);
            }

            if (app_cfg_const.rws_gaming_mode == 0)
            {
                if (app_multi_check_in_sniffing())
                {
                    app_bt_sniffing_stop(param->a2dp_stream_stop.bd_addr, BT_SNIFFING_TYPE_A2DP);
                }
#if CONFIG_REALTEK_GFPS_SASS_SUPPORT
                else if ((extend_app_cfg_const.gfps_sass_support) && (p_link->id == sass_active_idx))
                {
                    app_multi_handle_sniffing_link_disconnect_event(p_link->id);
                }
#endif
            }

            app_bt_policy_primary_engage_action_adjust();
            app_bt_policy_update_cpu_freq(BP_CPU_FREQ_EVENT_A2DP_STREAM);
#if F_APP_DYNAMIC_ADJUST_B2B_TX_POWER
            app_bt_policy_dynamic_adjust_b2b_tx_power(BP_TX_POWER_A2DP_STOP);
#endif
        }
        break;

    case BT_EVENT_A2DP_SNIFFING_STARTED:
        {
#if F_APP_DYNAMIC_ADJUST_B2B_TX_POWER
            app_bt_policy_dynamic_adjust_b2b_tx_power(BP_TX_POWER_A2DP_START);
#endif
        }
        break;

    case BT_EVENT_SCO_SNIFFING_CONN_CMPL:
        {
        }
        break;

    case BT_EVENT_SCO_SNIFFING_DISCONN_CMPL:
        {

        }
        break;

    case BT_EVENT_HFP_VOICE_RECOGNITION_ACTIVATION:
        {
            if (param->hfp_voice_recognition_activation.result == BT_HFP_CMD_OK)
            {
                uint8_t app_idx;
                T_APP_BR_LINK *p_link;
                app_idx = app_a2dp_get_active_idx();
                p_link = &(app_db.br_link[app_idx]);

                if (p_link != NULL)
                {
#if F_APP_MUTILINK_VA_PREEMPTIVE
                    T_APP_BR_LINK *p_link_va;

                    p_link_va = app_link_find_br_link(param->hfp_voice_recognition_activation.bd_addr);
                    if (p_link_va != NULL)
                    {
                        active_va_idx = p_link_va->id;
                        app_check_voice_dial(p_link_va);
                    }
#endif
                }
            }
        }
        break;

    case BT_EVENT_HFP_VOICE_RECOGNITION_DEACTIVATION:
        {
            if (param->hfp_voice_recognition_deactivation.result == BT_HFP_CMD_OK)
            {

            }
        }
        break;

    case BT_EVENT_HFP_CALL_STATUS:
        {
            uint8_t cmd_ptr[7];
            uint8_t inactive_hf_idx = MAX_BR_LINK_NUM;
            T_APP_CALL_STATUS inactive_hf_status = APP_CALL_IDLE;
            p_link = &(app_db.br_link[active_a2dp_idx]);

            //find inactive device
            for (uint8_t j = 0; j < MAX_BR_LINK_NUM; j++)
            {
                if (j != active_hf_idx)
                {
                    if ((app_db.br_link[j].connected_profile & HFP_PROFILE_MASK) ||
                        (app_db.br_link[j].connected_profile & HSP_PROFILE_MASK))
                    {
                        inactive_hf_idx = j;
                        inactive_hf_status = (T_APP_CALL_STATUS)app_db.br_link[inactive_hf_idx].call_status;
                        break;
                    }
                }
            }

            if ((inactive_hf_idx == pending_sco_index_from_sco)
                && inactive_hf_status == APP_CALL_IDLE)
            {
                sco_wait_from_sco_sniffing_stop = false;
                pending_sco_index_from_sco = 0xFF;
            }

            if (p_link != NULL)
            {
                if (param->hfp_call_status.curr_status != BT_HFP_CALL_IDLE)
                {
#if F_APP_MUTILINK_VA_PREEMPTIVE
                    if (app_link_find_br_link(param->hfp_call_status.bd_addr))
                    {
                        app_check_voice_dial(app_link_find_br_link(param->hfp_call_status.bd_addr));
                    }

#endif
#if F_APP_GAMING_DONGLE_SUPPORT
                    if ((!app_cfg_const.enable_multi_link) || (p_link->id != app_hfp_get_active_idx()))
#endif
                    {
                        if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                        {
                            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                            {
#if F_APP_MUTILINK_VA_PREEMPTIVE && F_APP_ERWS_SUPPORT
                                if (app_db.br_link[active_a2dp_idx].bt_sniffing_state == APP_BT_SNIFFING_STATE_STARTING)
                                {
                                    app_bt_sniffing_a2dp_mute(app_db.br_link[active_a2dp_idx].bd_addr, BT_SNIFFING_TYPE_A2DP);
                                }
                                else
#endif
                                {
                                    memcpy(&cmd_ptr[0], app_db.br_link[active_a2dp_idx].bd_addr, 6);
                                    cmd_ptr[6] = AUDIO_STREAM_TYPE_PLAYBACK;
                                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_MULTI_LINK,
                                                                        APP_REMOTE_MSG_MUTE_AUDIO_WHEN_MULTI_CALL_NOT_IDLE,
                                                                        cmd_ptr, 7);
                                }
                            }
                        }
                        else
                        {
#if F_APP_TEAMS_BT_POLICY || F_APP_GAMING_CHAT_MIXING_SUPPORT || F_APP_GAMING_DONGLE_SUPPORT
#else
                            audio_track_volume_out_mute(p_link->a2dp_track_handle);
#endif
                        }
                    }
                }
                else
                {
                    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                    {
                        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                        {
#if F_APP_MUTILINK_VA_PREEMPTIVE && F_APP_ERWS_SUPPORT
                            if (app_db.br_link[active_a2dp_idx].bt_sniffing_state == APP_BT_SNIFFING_STATE_STARTING)
                            {
                                app_bt_sniffing_a2dp_unmute(app_db.br_link[active_a2dp_idx].bd_addr, BT_SNIFFING_TYPE_A2DP);
                            }
#endif
                            memcpy(&cmd_ptr[0], app_db.br_link[active_a2dp_idx].bd_addr, 6);
                            cmd_ptr[6] = AUDIO_STREAM_TYPE_PLAYBACK;
                            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_MULTI_LINK,
                                                                APP_REMOTE_MSG_UNMUTE_AUDIO_WHEN_MULTI_CALL_IDLE,
                                                                cmd_ptr, 7);
                        }
                    }
                    else
                    {
#if F_APP_TEAMS_BT_POLICY
#else
                        audio_track_volume_out_unmute(p_link->a2dp_track_handle);
                        p_link->playback_muted = false;
#endif
                    }
#if F_APP_MUTILINK_VA_PREEMPTIVE
                    if (!app_multi_check_in_sniffing())
                    {
                        T_APP_BR_LINK *p_link;
                        p_link = app_link_find_br_link(param->hfp_call_status.bd_addr);
                        if (p_link != NULL)
                        {
                            app_multi_handle_sniffing_link_disconnect_event(p_link->id);
                        }
                    }
#endif
                }
            }
        }
        break;

    case BT_EVENT_HFP_DISCONN_CMPL:
        {
            p_link = app_link_find_br_link(param->hfp_disconn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                if (p_link->id == pending_sco_index_from_sco)
                {
                    sco_wait_from_sco_sniffing_stop = false;
                    pending_sco_index_from_sco = 0xFF;
                }
#if F_APP_MUTILINK_VA_PREEMPTIVE
                if (p_link->id == pending_va_index_from_va)
                {
                    va_wait_from_va_sniffing_stop = false;
                    pending_va_index_from_va = 0xFF;
                    pending_va_sniffing_type = 0xFF;
                }
#endif
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_multi_bt_cback: event_type 0x%04x", event_type);
    }
}

static void app_multi_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_multi_timeout_cb: timer_evt %d, param %d", timer_evt, param);
    switch (timer_evt)
    {
    case APP_TIMER_MULTILINK_DISCONN_INACTIVE_ACL:
        {
            app_stop_timer(&timer_idx_multilink_disconn_inctive_acl);

            if (app_db.br_link[param].used == true)
            {
                gap_br_send_acl_disconn_req(app_db.br_link[param].bd_addr);
            }
        }
        break;

#if F_APP_MUTILINK_VA_PREEMPTIVE
    case APP_TIMER_MULTILINK_DISC_VA_SCO:
        {
            app_stop_timer(&timer_idx_multilink_disc_va_sco);
            if (app_db.br_link[param].sco_handle != NULL)
            {
                bt_hfp_audio_disconnect_req(app_db.br_link[param].bd_addr);
                app_bt_sniffing_stop(app_db.br_link[param].bd_addr, BT_SNIFFING_TYPE_SCO);
            }
        }
        break;
#endif
    case APP_TIMER_MULTILINK_SILENCE_PACKET_JUDGE:
        {
            app_stop_timer(&timer_idx_multilink_silence_packet_judge);
#if F_APP_LEA_SUPPORT
            bool need_return = false;
            if ((mtc_if_fm_ml_handle(ML_TO_MTC_CH_SNIFFING, NULL, &need_return) == MTC_RESULT_SUCCESS) &&
                need_return)
            {
                APP_PRINT_TRACE0("APP_TIMER_MULTILINK_SILENCE_PACKET_JUDGE: STOP SNIFFING");
                le_sniffing_stop = true;
                break;

            }
#endif
            if (app_db.br_link[param].streaming_fg == true &&
                app_db.br_link[param].avrcp_ready_to_pause == true)
            {
                app_multi_stream_avrcp_set(param);
                app_multi_pause_inactive_a2dp_link_stream(param, false);
            }
            else
            {
                app_start_timer(&timer_idx_multilink_silence_packet_detect, "multilink_silence_packet_detect",
                                multilink_timer_id, APP_TIMER_MULTILINK_SILENCE_PACKET_DETECT, 0, false,
                                7500);
            }
        }
        break;

    case APP_TIMER_MULTILINK_SILENCE_PACKET_DETECT:
        {
            app_stop_timer(&timer_idx_multilink_silence_packet_detect);
        }
        break;

    default:
        break;
    }
}

#if F_APP_ERWS_SUPPORT
uint16_t app_multi_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;

    switch (msg_type)
    {
    case APP_REMOTE_MSG_ACTIVE_BD_ADDR_SYNC:
        {
            payload_len = 6;
            msg_ptr = (uint8_t *)app_db.br_link[app_hfp_get_active_idx()].bd_addr;
        }
        break;

    case APP_REMOTE_MSG_PHONE_CONNECTED:
        {
            static uint8_t num = 0;
            num = app_link_get_b2s_link_num();
            payload_len = 1;
            msg_ptr = &num;
        }
        break;

#if CONFIG_REALTEK_GFPS_SASS_SUPPORT
    case APP_REMOTE_MSG_SASS_PREEM_BIT_SYNC:
        {
            skip = false;
            payload_len = 2;
            msg_ptr = (uint8_t *)&app_cfg_nv.sass_switching_preference;
        }
        break;

    case APP_REMOTE_MSG_SASS_SWITCH_SYNC:
        {
            skip = false;
            payload_len = 1;
            msg_ptr = (uint8_t *)&app_cfg_nv.sass_advanced_switching_setting;
        }
        break;

    case APP_REMOTE_MSG_SASS_MULTILINK_STATE_SYNC:
        {
            skip = false;
            payload_len = 1;
            msg_ptr = (uint8_t *) &app_cfg_nv.sass_multipoint_state;
        }
        break;

    case APP_REMOTE_MSG_SASS_DEVICE_SUPPORT_SYNC:
        {
            skip = false;
            payload_len = 1;
            msg_ptr = (uint8_t *) &app_cfg_nv.sass_bitmap;
        }
        break;

    case APP_REMOTE_MSG_SASS_DEVICE_BITMAP_SYNC:
        {
            skip = false;
            payload_len = 1;
            msg_ptr = (uint8_t *) &app_db.conn_bitmap;
        }
        break;
#endif

    default:
        break;
    }

    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_MULTI_LINK, payload_len, msg_ptr, skip,
                              total);
}

static void app_multi_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                  T_REMOTE_RELAY_STATUS status)
{
    switch (msg_type)
    {
    case APP_REMOTE_MSG_MUTE_AUDIO_WHEN_MULTI_CALL_NOT_IDLE:
        {
            if ((status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT) ||
                (status == REMOTE_RELAY_STATUS_ASYNC_RCVD))
            {
                uint8_t *p_info = (uint8_t *)buf;
                uint8_t bd_addr[6];
                T_AUDIO_STREAM_TYPE stream_type;
                T_APP_BR_LINK *p_link;

                memcpy(bd_addr, &p_info[0], 6);
                p_link = app_link_find_br_link(bd_addr);
                stream_type = (T_AUDIO_STREAM_TYPE)p_info[6];
                if (p_link != NULL)
                {
                    if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
                    {
                        audio_track_volume_out_mute(p_link->a2dp_track_handle);
                        p_link->playback_muted = true;
                    }
                }
            }
        }
        break;

    case APP_REMOTE_MSG_UNMUTE_AUDIO_WHEN_MULTI_CALL_IDLE:
        {
            if ((status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT) ||
                (status == REMOTE_RELAY_STATUS_ASYNC_RCVD))
            {
                uint8_t *p_info = (uint8_t *)buf;
                uint8_t bd_addr[6];
                T_AUDIO_STREAM_TYPE stream_type;
                T_APP_BR_LINK *p_link;

                memcpy(bd_addr, &p_info[0], 6);
                p_link = app_link_find_br_link(bd_addr);
                stream_type = (T_AUDIO_STREAM_TYPE)p_info[6];
                if (p_link != NULL)
                {
                    if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
                    {
                        audio_track_volume_out_unmute(p_link->a2dp_track_handle);
                        p_link->playback_muted = false;
                    }
                }
            }
        }
        break;

    case APP_REMOTE_MSG_RESUME_BT_ADDRESS:
        {
            memcpy(app_db.resume_addr, (uint8_t *)buf, 6);
            app_db.connected_num_before_roleswap = *((uint8_t *)buf + 6);
            APP_PRINT_TRACE2("app_multi_parse_cback: connected_num_before_roleswap %d, resume_addr %s",
                             app_db.connected_num_before_roleswap, TRACE_BDADDR(app_db.resume_addr));
        }
        break;

#if F_APP_MUTILINK_VA_PREEMPTIVE
    case APP_REMOTE_MSG_ACTIVE_BD_ADDR_SYNC:
        {
            if (app_cfg_const.enable_multi_link)
            {
                if ((status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT) ||
                    (status == REMOTE_RELAY_STATUS_ASYNC_RCVD))
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    memcpy(app_db.active_hfp_addr, &p_info[0], 6);
                }
            }
        }
        break;
#endif

    case APP_REMOTE_MSG_PROFILE_CONNECTED:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            app_bond_handle_remote_profile_connected_msg(buf);
        }
        break;

    case APP_REMOTE_MSG_PHONE_CONNECTED:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD &&
                app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                uint8_t num = *((uint8_t *)buf);
                app_link_set_b2s_link_num(num);
                APP_PRINT_TRACE1("app_multi_parse_cback: b2s_connected_num %d", num);
#if F_APP_LEA_SUPPORT
                if (app_bt_point_lea_link_is_full())
                {
                    app_lea_adv_stop();
                }
                else
                {
                    app_lea_adv_start();
                }
#endif

#if F_APP_LISTENING_MODE_SUPPORT
                if (app_cfg_const.disallow_listening_mode_before_phone_connected)
                {
                    if (app_link_get_b2s_link_num())
                    {
                        app_listening_judge_conn_disc_evnet(APPLY_LISTENING_MODE_SRC_CONNECTED);
                    }
                    else
                    {
                        app_listening_judge_conn_disc_evnet(APPLY_LISTENING_MODE_SRC_DISCONNECTED);
                    }
                }
#endif
            }
        }
        break;

#if CONFIG_REALTEK_GFPS_SASS_SUPPORT
    case APP_REMOTE_MSG_SASS_PREEM_BIT_SYNC:
        {
            if ((status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT) ||
                (status == REMOTE_RELAY_STATUS_ASYNC_RCVD))
            {
                uint8_t *p_info = (uint8_t *)buf;
                memcpy(&app_cfg_nv.sass_switching_preference, &p_info[0], 2);
            }
        }
        break;

    case APP_REMOTE_MSG_SASS_SWITCH_SYNC:
        {
            if ((status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT) ||
                (status == REMOTE_RELAY_STATUS_ASYNC_RCVD))
            {
                uint8_t *p_info = (uint8_t *)buf;
                memcpy(&app_cfg_nv.sass_advanced_switching_setting, &p_info[0], 1);
            }
        }
        break;

    case APP_REMOTE_MSG_SASS_MULTILINK_STATE_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t *p_info = (uint8_t *)buf;
                app_cfg_nv.sass_multipoint_state = *p_info;
            }
        }
        break;

    case APP_REMOTE_MSG_SASS_DEVICE_BITMAP_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t *p_info = (uint8_t *)buf;
                app_sass_policy_sync_conn_bit_map(*p_info);
            }

        }
        break;

    case APP_REMOTE_MSG_SASS_DEVICE_SUPPORT_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t *p_info = (uint8_t *)buf;
                app_cfg_nv.sass_bitmap = *p_info;
            }
        }
        break;
#endif

    case APP_REMOTE_MSG_COD_INFO:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t *p_info = (uint8_t *)buf;
                uint8_t bd_addr[6];
                bool is_computer = (bool)p_info[0];

                memcpy(bd_addr, &p_info[1], 6);

                if (is_computer == true)
                {
                    bt_bond_flag_add(bd_addr, BOND_FLAG_COMPUTER);
                }
            }
        }
        break;

    default:
        break;
    }
}
#endif

#if F_APP_LEA_SUPPORT
T_MTC_RESULT app_multi_mtc_if_handle(T_MTC_IF_MSG msg, void *inbuf, void *outbuf)
{
    T_MTC_RESULT app_result = MTC_RESULT_SUCCESS;
    APP_PRINT_INFO2("app_multi_mtc_if_handle: msg %x, %d", msg, app_cfg_nv.bud_role);

    switch (msg)
    {
    case ML_FM_MTC_LEA_START:
        {
        }
        break;

    case ML_FM_MTC_LEA_STOP:
        {
            if (le_sniffing_stop == true)
            {
                le_sniffing_stop = false;
                if (!app_multi_check_in_sniffing())
                {
                    app_multi_handle_sniffing_link_disconnect_event(0xFF);
                }
            }
        }
        break;

    default:
        break;
    }
    return app_result;
}
#endif

void app_multi_init(void)
{
    app_db.wait_resume_a2dp_idx = MAX_BR_LINK_NUM;
    bt_mgr_cback_register(app_multi_bt_cback);
    app_timer_reg_cb(app_multi_timeout_cb, &multilink_timer_id);
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_multi_relay_cback, app_multi_parse_cback,
                             APP_MODULE_TYPE_MULTI_LINK, APP_REMOTE_MSG_MULTILINK_TOTAL);
#endif
#if F_APP_LEA_SUPPORT
    mtc_cback_register(app_multi_mtc_if_handle);
#endif
}

#if F_APP_MUTILINK_ALLOW_TWO_SCO
#else
void app_multilink_sco_preemptive(uint8_t inactive_idx)
{
    APP_PRINT_TRACE1("app_multilink_sco_preemptive:%d", inactive_idx);
    bt_hfp_audio_disconnect_req(app_db.br_link[inactive_idx].bd_addr);
    if (app_cfg_const.enable_multi_link)
    {
        sco_wait_from_sco_sniffing_stop = true;
        pending_sco_index_from_sco = inactive_idx;
    }
}
#endif

#if F_APP_MUTILINK_VA_PREEMPTIVE

#if F_APP_MUTILINK_ALLOW_TWO_SCO
#else
static void app_multi_voice_ongoing_preemptive(uint8_t disable_idx, uint8_t idx)
{
#if F_APP_DURIAN_SUPPORT
    if (app_db.br_link[disable_idx].connected_profile & AVP_PROFILE_MASK)
    {
        app_durian_mmi_voice_recognition_disable(disable_idx);
    }
    else
#endif
    {
        bt_hfp_voice_recognition_disable_req(app_db.br_link[disable_idx].bd_addr);

        va_wait_from_va_sniffing_stop = true;
        pending_va_index_from_va = idx;
        pending_va_sniffing_type = 0xFF;
        app_stop_timer(&timer_idx_multilink_disc_va_sco);
        app_start_timer(&timer_idx_multilink_disc_va_sco, "multilink_disc_va_sco",
                        multilink_timer_id, APP_TIMER_MULTILINK_DISC_VA_SCO, disable_idx, false,
                        1500);
    }
}
#endif

void app_check_voice_dial(T_APP_BR_LINK *p_link)
{
#if F_APP_MUTILINK_ALLOW_TWO_SCO
#else
    uint8_t active_hf_idx = app_hfp_get_active_idx();
    uint8_t idx = p_link->id;

    if (idx != active_hf_idx)
    {
        if (app_db.br_link[active_hf_idx].call_status == APP_VOICE_ACTIVATION_ONGOING)
        {
            app_multi_voice_ongoing_preemptive(active_hf_idx, idx);
        }
        else if (app_db.br_link[active_hf_idx].call_status == APP_CALL_IDLE)
        {
            if (app_db.br_link[active_hf_idx].sco_handle)
            {
                bt_hfp_audio_disconnect_req(app_db.br_link[active_hf_idx].bd_addr);
            }
        }
    }
    else
    {
        for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (i != active_hf_idx)
            {
                if (app_db.br_link[i].call_status == APP_VOICE_ACTIVATION_ONGOING)
                {
                    app_multi_voice_ongoing_preemptive(i, idx);
                }
                else if ((app_db.br_link[i].sco_handle) &&
                         (app_db.br_link[i].call_status != APP_CALL_INCOMING))
                {
                    bt_hfp_audio_disconnect_req(app_db.br_link[i].bd_addr);
                }
            }
        }
    }
#endif
}
#endif

uint8_t app_multi_find_inacitve(uint8_t idx)
{
    uint8_t inactive_idx = MAX_BR_LINK_NUM;

    if (app_link_get_b2s_link_num() == MULTILINK_SRC_CONNECTED)
    {
        for (uint8_t j = 0; j < MAX_BR_LINK_NUM; j++)
        {
            if ((app_link_check_b2s_link_by_id(j)) && (j != idx))
            {
                inactive_idx = j;
            }
        }
    }
    return inactive_idx;
}

#if CONFIG_REALTEK_GFPS_SASS_SUPPORT
void app_multi_set_active_idx(uint8_t idx)
{
    APP_PRINT_TRACE2("app_multi_set_active_idx: %d, %d", sass_active_idx, idx);

    if (sass_active_idx != idx)
    {
        sass_active_idx = idx;

        if (idx < MAX_BR_LINK_NUM)
        {
            memcpy(app_db.active_hfp_addr, app_db.br_link[idx].bd_addr, 6);
        }
        else
        {
            memset(app_db.active_hfp_addr, 0, 6);
        }

        if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
        {
            app_relay_async_single(APP_MODULE_TYPE_MULTI_LINK, APP_REMOTE_MSG_ACTIVE_BD_ADDR_SYNC);
        }
    }

    if (extend_app_cfg_const.gfps_support)
    {
        app_gfps_notify_conn_status();

        if (app_db.br_link[idx].call_status != BT_HFP_CALL_IDLE)
        {
            app_gfps_msg_notify_multipoint_switch(idx, GFPS_MULTIPOINT_SWITCH_REASON_HFP);
        }
        else if (app_db.br_link[idx].streaming_fg)
        {
            app_gfps_msg_notify_multipoint_switch(idx, GFPS_MULTIPOINT_SWITCH_REASON_A2DP);
        }
        else
        {
            app_gfps_msg_notify_multipoint_switch(idx, GFPS_MULTIPOINT_SWITCH_REASON_UNKNOWN);
        }
    }
}

uint8_t app_multi_get_active_idx(void)
{
    return sass_active_idx;
}

uint8_t app_multi_find_other_idx(uint8_t app_idx)
{
    uint8_t another_idx = MAX_BR_LINK_NUM;

    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if ((i != app_idx) &&
            (app_db.br_link[i].connected_profile & (A2DP_PROFILE_MASK | HFP_PROFILE_MASK | HSP_PROFILE_MASK)))
        {
            another_idx = i;
            break;
        }
    }

    return another_idx;
}

bool app_multi_get_stream_only(uint8_t idx)
{
    if (idx < MAX_BR_LINK_NUM)
    {
        return stream_only[idx];
    }

    return false;
}

void app_multi_pause_inactive_for_sass(void)
{
    uint8_t active_idx = app_multi_get_active_idx();
    uint8_t inactive_idx  = app_multi_find_other_idx(active_idx);

    APP_PRINT_TRACE4("app_multi_pause_inactive_for_sass active %d,key 0x%x, inactive %d, key 0x%x",
                     active_idx, app_db.br_link[active_idx].gfps_inuse_account_key, inactive_idx,
                     app_db.br_link[inactive_idx].gfps_inuse_account_key);

    if ((app_db.br_link[inactive_idx].gfps_inuse_account_key != 0xFF) &&
        (app_db.br_link[active_idx].gfps_inuse_account_key != 0xFF))
    {
        //both sass device, do not pause directly
    }
    else if ((app_db.br_link[inactive_idx].gfps_inuse_account_key == 0xFF) &&
             (app_db.br_link[active_idx].gfps_inuse_account_key == 0xFF))
    {
        //both non-sass, do not pause, follow orignial behavior
    }
    else
    {
        //non-sass and sass, pause directly
        app_db.br_link[inactive_idx].avrcp_ready_to_pause = false;
        bt_avrcp_pause(app_db.br_link[inactive_idx].bd_addr);
    }
}

bool app_multi_sass_bitmap(uint8_t app_idx, uint8_t profile, bool a2dp_check)
{
    uint8_t another_idx = app_multi_find_other_idx(app_idx);
    bool ret = false;

    if ((another_idx == MAX_BR_LINK_NUM) || (profile == MULTILINK_SASS_FORCE_PREEM) ||
        (app_idx == app_multi_get_active_idx()))
    {
        ret = true;
    }
    else if (profile == MULTILINK_SASS_A2DP_PREEM || profile == MULTILINK_SASS_AVRCP_PREEM)
    {
        APP_PRINT_TRACE2("JUDGE_EVENT_A2DP_START: avrcp_play_status %d, stream %d",
                         app_db.br_link[app_idx].avrcp_play_status, app_db.br_link[app_idx].streaming_fg);

        if (profile == MULTILINK_SASS_AVRCP_PREEM && app_db.br_link[app_idx].streaming_fg == false)
        {
            ret = false;
        }
        else if (app_multi_check_pend() &&
                 ((app_db.br_link[app_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING) ||
                  (timer_idx_multilink_silence_packet_detect != NULL) || (a2dp_check == true) ||
                  (((app_db.br_link[another_idx].streaming_fg == false) &&
                    (app_db.br_link[another_idx].call_status == BT_HFP_CALL_IDLE)) &&
                   (app_db.br_link[app_idx].avrcp_play_status != BT_AVRCP_PLAY_STATUS_PLAYING))))
        {
            if (app_db.br_link[another_idx].call_status != BT_HFP_CALL_IDLE)
            {
                if (app_db.br_link[app_idx].call_status != BT_HFP_CALL_IDLE) //hfp but has a2dp
                {
                    if (app_hfp_get_active_idx() == app_idx)
                    {
                        ret = true;
                    }
                    else
                    {
                        ret = app_multi_sass_bitmap(app_idx, MULTILINK_SASS_HFP_PREEM, false);
                    }
                }
                else if (app_db.br_link[another_idx].call_status == APP_VOICE_ACTIVATION_ONGOING)
                {
                    ret = app_cfg_nv.sass_switching_preference & SASS_A2DP_VA;
                }
                else
                {
                    ret = app_cfg_nv.sass_switching_preference & SASS_A2DP_SCO;
                }
            }
            else if (app_db.br_link[another_idx].streaming_fg)
            {
                if (app_a2dp_get_active_idx() == app_idx || timer_idx_multilink_silence_packet_detect != NULL)
                {
                    ret = true;
                }
                else
                {
                    ret = app_cfg_nv.sass_switching_preference & SASS_A2DP_A2DP;
                }
            }
            else // other idle
            {
                ret = true;
            }
        }
        else
        {
            ret = false;
        }
    }
    else if (profile == MULTILINK_SASS_HFP_PREEM)
    {
        APP_PRINT_TRACE5("app_multi_sass_bitmap VA: app_idx_status = %d %d, another_idx_status = %d %d, hfp_active_idx = %d",
                         app_idx, app_db.br_link[app_idx].call_status, another_idx, app_db.br_link[another_idx].call_status,
                         app_hfp_get_active_idx());

        if (app_db.br_link[another_idx].call_status == APP_VOICE_ACTIVATION_ONGOING)
        {
            if (app_hfp_get_active_idx() == app_idx)
            {
                ret = true;
            }
            else if (app_db.br_link[app_idx].call_status == APP_VOICE_ACTIVATION_ONGOING)
            {
                ret = app_cfg_nv.sass_switching_preference & SASS_VA_VA;
            }
            else if (app_db.br_link[app_idx].call_status != BT_HFP_CALL_IDLE)
            {
                ret = app_cfg_nv.sass_switching_preference & SASS_SCO_VA;
            }
            else // wait hfp
            {
                ret = false;
            }
        }
        else if (app_db.br_link[another_idx].call_status != BT_HFP_CALL_IDLE)
        {
            if (app_hfp_get_active_idx() == app_idx)
            {
                ret = true;
            }
            else if (app_db.br_link[app_idx].call_status == APP_VOICE_ACTIVATION_ONGOING)
            {
                ret = app_cfg_nv.sass_switching_preference & SASS_VA_SCO;
            }
            else if (app_db.br_link[app_idx].call_status != BT_HFP_CALL_IDLE)
            {
                ret = app_cfg_nv.sass_switching_preference & SASS_SCO_SCO;
            }
            else // wait hfp
            {
                ret = false;
            }
        }
        else if (app_db.br_link[another_idx].streaming_fg)
        {
            if (app_db.br_link[app_idx].call_status == APP_VOICE_ACTIVATION_ONGOING)
            {
                ret = app_cfg_nv.sass_switching_preference & SASS_VA_A2DP;
            }
            else if (app_db.br_link[app_idx].call_status != BT_HFP_CALL_IDLE)
            {
                ret = app_cfg_nv.sass_switching_preference & SASS_SCO_A2DP;
            }
            else // wait hfp
            {
                ret = false;
            }
        }
        else //other idle
        {
            ret = true;
        }
    }

    APP_PRINT_TRACE4("app_multi_sass_bitmap: type = %d, idx = %d, active_idx = %d, ret = %d", profile,
                     app_idx, app_multi_get_active_idx(), ret);
    return ret;
}

bool app_multi_preemptive_judge(uint8_t app_idx, uint8_t type)
{
    bool ret = app_multi_sass_bitmap(app_idx, type, false);
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();
    uint8_t org_type = type;
    uint8_t another_idx = app_multi_find_other_idx(app_idx);

    if (type == MULTILINK_SASS_FORCE_PREEM)
    {
        if (app_db.br_link[app_idx].streaming_fg)
        {
            type = MULTILINK_SASS_A2DP_PREEM;
        }
        else if (app_db.br_link[app_idx].sco_handle)
        {
            type = MULTILINK_SASS_HFP_PREEM;
        }
    }

    APP_PRINT_TRACE2("app_multi_preemptive_judge: ret = %d, type = %d", ret, type);

    if (type == MULTILINK_SASS_A2DP_PREEM)
    {
        if (ret)
        {
            if (active_a2dp_idx != app_idx)
            {
                {
                    app_stop_timer(&timer_idx_multilink_silence_packet_detect);
                    app_multi_pause_inactive_a2dp_link_stream(app_idx, false);
                    app_multi_stream_avrcp_set(app_idx);
                }
            }
            else
            {
                if (app_db.br_link[app_idx].avrcp_play_status != BT_AVRCP_PLAY_STATUS_PLAYING)
                {
                    app_db.br_link[app_idx].avrcp_play_status = BT_AVRCP_PLAY_STATUS_PLAYING;
                    stream_only[app_idx] = true;
                }

                if ((app_cfg_const.enable_multi_link) &&
                    (app_link_get_b2s_link_num() == MULTILINK_SRC_CONNECTED))
                {
                    app_multi_pause_inactive_a2dp_link_stream(app_idx, false);
                }
            }
        }
        else
        {
            if (app_db.br_link[another_idx].streaming_fg == true)
            {
                if ((app_db.br_link[active_a2dp_idx].streaming_fg == false) && app_multi_check_pend())
                {
                    active_a2dp_idx = app_idx;

                    app_multi_a2dp_active_link_set(app_db.br_link[app_idx].bd_addr);
                    app_bond_set_priority(app_db.br_link[app_idx].bd_addr);
                }
                else
                {
                    if (app_db.active_media_paused)
                    {
                        // A2DP Paused
                        app_db.a2dp_wait_a2dp_sniffing_stop = true;
                        app_db.pending_a2dp_idx = app_idx;
                    }
                }
            }
            else if (app_db.br_link[another_idx].call_status != BT_HFP_CALL_IDLE)
            {
                if (app_cfg_const.enable_multi_sco_disc_resume)
                {
                    app_multi_pause_inactive_a2dp_link_stream(another_idx, true);
                }
                else
                {
                    app_multi_pause_inactive_a2dp_link_stream(another_idx, false);
                }
            }
        }
    }
    else if (type == MULTILINK_SASS_AVRCP_PREEM)
    {
        if (ret)
        {
            APP_PRINT_TRACE3("JUDGE_EVENT_MEDIAPLAYER_PLAYING: preemptive active_a2dp_idx %d, app_idx %d, streaming_fg %d",
                             active_a2dp_idx, app_idx, app_db.br_link[app_idx].streaming_fg);

            if (active_a2dp_idx == app_idx)
            {
                app_db.active_media_paused = false;
            }

            if (app_db.br_link[app_idx].streaming_fg == true)
            {
                app_multi_a2dp_active_link_set(app_db.br_link[app_idx].bd_addr);
                app_multi_pause_inactive_a2dp_link_stream(app_idx, false);
                app_bond_set_priority(app_db.br_link[app_idx].bd_addr);
            }
        }
        else
        {
            if (app_db.br_link[another_idx].streaming_fg == true)
            {
                if (extend_app_cfg_const.gfps_sass_support)
                {
                    app_multi_pause_inactive_for_sass();
                }
            }
            else if (app_db.br_link[another_idx].call_status != BT_HFP_CALL_IDLE)
            {
                if (app_cfg_const.enable_multi_sco_disc_resume)
                {
                    app_multi_pause_inactive_a2dp_link_stream(another_idx, true);
                }
                else
                {
                    app_multi_pause_inactive_a2dp_link_stream(another_idx, false);
                }
            }
        }
    }
    else if (type == MULTILINK_SASS_HFP_PREEM)
    {
        if (ret)
        {
            if (app_cfg_const.enable_multi_sco_disc_resume)
            {
                app_multi_pause_inactive_a2dp_link_stream(app_idx, true);
            }
            else
            {
                app_multi_pause_inactive_a2dp_link_stream(app_idx, false);
            }

            if (app_db.br_link[app_idx].sco_track_handle)
            {
                audio_track_start(app_db.br_link[app_idx].sco_track_handle);
            }
        }
        else if (app_db.br_link[another_idx].streaming_fg)
        {
            if (app_db.br_link[app_idx].sco_handle)// && app_db.br_link[app_idx].call_status == BT_HFP_VOICE_ACTIVATION_ONGOING)
            {
                gap_br_vendor_set_active_sco(app_db.br_link[app_idx].sco_handle, 0, 2);
            }

            app_db.a2dp_preemptive_flag = true;
            app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_MULTILINK_CHANGE);
        }
    }

    if (ret)
    {
        app_multi_set_active_idx(app_idx);

        if (app_db.br_link[another_idx].call_status != BT_HFP_CALL_IDLE ||
            app_db.br_link[another_idx].sco_handle)
        {
            if (app_db.br_link[another_idx].sco_handle &&
                app_db.br_link[app_idx].streaming_fg)
            {
                gap_br_vendor_set_active_sco(app_db.br_link[another_idx].sco_handle, 0, 2);
            }

            if (app_db.br_link[another_idx].call_status == APP_VOICE_ACTIVATION_ONGOING)
            {
                app_multi_voice_ongoing_preemptive(another_idx, app_idx);
            }
            else
            {
                app_multi_active_hfp_transfer(app_idx, true, org_type == MULTILINK_SASS_FORCE_PREEM);
            }
        }
    }

    return ret;
}

uint8_t app_multi_get_inactive_index(uint8_t new_link_idx, uint8_t call_num, bool force)
{
    uint8_t inactive_index = app_sass_policy_get_disc_link();

    if (inactive_index != MAX_BR_LINK_NUM)
    {
        return inactive_index;
    }

    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (((i != app_multi_get_active_idx()) && (i != new_link_idx) &&
             (app_db.br_link[i].connected_profile & (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK | HFP_PROFILE_MASK |
                                                     HSP_PROFILE_MASK))))
        {
            inactive_index = i;
            break;
        }
    }

    if ((inactive_index == MAX_BR_LINK_NUM) && ((app_sass_get_available_connection_num() == 0) ||
                                                force))
    {
        inactive_index = app_multi_get_active_idx();
    }

    return inactive_index;
}
#endif
