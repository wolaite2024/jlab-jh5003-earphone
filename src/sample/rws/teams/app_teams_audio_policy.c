/*
 * Copyright (c) 2020, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_TEAMS_FEATURE_SUPPORT
#include "string.h"
#include "os_mem.h"
#include "trace.h"
#include "app_cfg.h"
#include "app_main.h"
#include "voice_prompt.h"
#include "hid_vp_update.h"
#include "app_audio_policy.h"
#include "app_hfp.h"
#include "app_teams_bt_policy.h"
#include "app_teams_audio_policy.h"
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
#include "app_single_multilink_customer.h"
#endif

#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
static bool teams_global_mute_status = false;

void app_teams_audio_set_global_mute_status(bool mute_status)
{
    APP_PRINT_TRACE1("app_teams_audio_set_global_mute_status: %d", mute_status);
    teams_global_mute_status = mute_status;
}

bool app_teams_audio_get_global_mute_status(void)
{
    return teams_global_mute_status;
}

void app_teams_audio_set_bt_mute_status(uint8_t index, bool mute_status)
{
    app_teams_audio_set_global_mute_status(mute_status);
    T_APP_BR_LINK *p_link = NULL;
    p_link = &app_db.br_link[index];
    p_link->is_mute = mute_status;
}

void app_teams_audio_sync_mute_status_handle(void)
{
    app_audio_set_mic_mute_status(1);
    uint8_t teams_active_index = app_hfp_get_active_idx();
    app_teams_audio_set_bt_mute_status(teams_active_index, true);

    T_APP_BR_LINK *p_link = NULL;
    p_link = &app_db.br_link[teams_active_index];
    if (p_link->sco_track_handle)
    {
        audio_track_volume_in_mute(p_link->sco_track_handle);
        bt_hfp_microphone_gain_level_report(p_link->bd_addr, 0);
    }
}

bool app_teams_audio_is_call_hold_without_active_call(uint8_t index)
{
    if (app_db.br_link[index].call_hold_status == APP_HFP_CALL_HELD_STATUS_HOLD_NO_ACTIVE_CALL)
    {
        APP_PRINT_ERROR1("app_teams_audio_is_call_hold_without_active_call: active index %d 's call hold status is 2",
                         index);
        return false;
    }
    return true;
}
#endif

bool app_teams_audio_language_set_by_lcid(uint8_t *lcid)
{
#if F_APP_TEAMS_VP_UPDATE_SUPPORT
    uint8_t lcid_flash[4] = {0};

    APP_PRINT_TRACE1("app_teams_audio_language_set_by_lcid: lcid %b",
                     TRACE_BINARY(4, lcid));

    if (!lcid)
    {
        return false;
    }

    for (uint8_t i = 0; i < VP_UPDATE_SUPPORT_LANGUAGE_NUM ; i++)
    {
        if (hid_vp_update_get_lcid(i, lcid_flash))
        {
            if (!memcmp(lcid, lcid_flash, 4))
            {
                APP_PRINT_TRACE3("app_teams_audio_language_set_by_lcid: i %d, vp change %d, vp valid %d",
                                 i, hid_vp_update_get_vp_changed_flag(), hid_vp_update_get_vp_valid_flag());
                if ((i == (VP_UPDATE_SUPPORT_LANGUAGE_NUM - 1)) &&
                    (hid_vp_update_get_vp_changed_flag()) &&
                    (!hid_vp_update_get_vp_valid_flag()))
                {
                    /* if select index is update index and vp resource has change by update and update failed,
                    this language set action not pass*/
                    return false;
                }
                if (voice_prompt_language_set((T_VOICE_PROMPT_LANGUAGE_ID)i))
                {
                    app_cfg_nv.voice_prompt_language = (T_VOICE_PROMPT_LANGUAGE_ID)i;
                    return true;
                }
            }
        }
    }
#endif

    return false;
}

void app_teams_audio_battery_vp_play(uint8_t power_on_bat)
{
    APP_PRINT_INFO1("app_teams_audio_battery_vp_play: power_on_bat %d", power_on_bat);
    if (power_on_bat == 100)
    {
        app_audio_tone_type_play(TONE_HOURS_LEFT_30, false, true);
    }
    else if (power_on_bat >= 50)
    {
        app_audio_tone_type_play(TONE_HOURS_LEFT_15, false, true);
    }
    else if (power_on_bat >= 20)
    {
        app_audio_tone_type_play(TONE_HOURS_LEFT_6, false, true);
    }
    else if (power_on_bat >= 17)
    {
        app_audio_tone_type_play(TONE_HOURS_LEFT_5, false, true);
    }
    else if (power_on_bat >= 13)
    {
        app_audio_tone_type_play(TONE_HOURS_LEFT_4, false, true);
    }
    else if (power_on_bat >= 10)
    {
        app_audio_tone_type_play(TONE_HOURS_LEFT_3, false, true);
    }
    else if (power_on_bat >= 6)
    {
        app_audio_tone_type_play(TONE_HOURS_LEFT_2, false, true);
    }
    else if (power_on_bat >= 2)
    {
        app_audio_tone_type_play(TONE_HOURS_LEFT_1, false, true);
    }
    else
    {
    }
}

void app_teams_audio_disconnected_state_vp_play(uint8_t *bd_addr)
{
#if F_APP_TEAMS_BT_POLICY
    T_TEAMS_DEVICE_TYPE device_type = T_TEAMS_DEVICE_DEFAULT_TYPE;
    device_type = app_teams_bt_policy_get_cod_type_by_addr(bd_addr);

    if (device_type == T_TEAMS_DEVICE_PHONE_TYPE)
    {
        app_audio_tone_type_play(TONE_DISCONNECTED_FROM_PHONE, false, true);
    }
    else if (device_type == T_TEAMS_DEVICE_COMPUTER_TYPE)
    {
        app_audio_tone_type_play(TONE_DISCONNECTED_FROM_COMPUTER, false, true);
    }
    else if (device_type == T_TEAMS_DEVICE_DONGLE_TYPE)
    {
        app_audio_tone_type_play(TONE_DISCONNECTED_FROM_USB_LINK, false, true);
    }
    else
    {
        app_audio_tone_type_play(TONE_DISCONNECTED, false, true);//Disconnected
    }
    APP_PRINT_INFO1("app_teams_audio_disconnected_state_vp_play: device_type %d", device_type);
#else
    app_audio_tone_type_play(TONE_DISCONNECTED, false, true);//Disconnected
#endif
}

void app_teams_audio_connected_state_vp_play(uint8_t *bd_addr, uint8_t is_new_device)
{
#if F_APP_TEAMS_BT_POLICY
    T_TEAMS_DEVICE_TYPE device_type = T_TEAMS_DEVICE_DEFAULT_TYPE;
    device_type = app_teams_bt_policy_get_cod_type_by_addr(bd_addr);

    if (device_type == T_TEAMS_DEVICE_PHONE_TYPE)
    {
        app_audio_tone_type_play(TONE_CONNECTED_TO_PHONE, false, true);
    }
    else if (device_type == T_TEAMS_DEVICE_COMPUTER_TYPE)
    {
        app_audio_tone_type_play(TONE_CONNECTED_TO_COMPUTER, false, true);
    }
    else
    {
        if (is_new_device == 0)
        {
            if (device_type == T_TEAMS_DEVICE_DONGLE_TYPE)
            {
                app_audio_tone_type_play(TONE_CONNECTED_TO_USB_LINK, false, true);
            }
            else
            {
                app_audio_tone_type_play(TONE_CONNECTED, false, true);//Connected
            }
        }
    }
    APP_PRINT_INFO1("app_teams_audio_connected_state_vp_play: device_type %d", device_type);
#else
    app_audio_tone_type_play(TONE_CONNECTED, false, true);//Connected
#endif
}
#endif

