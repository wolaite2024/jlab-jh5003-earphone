/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include "string.h"
#include "stdlib_corecrt.h"
#include "wdg.h"
#include "os_timer.h"
#include "trace.h"
#include "app_timer.h"
#include "gap_le.h"
#include "app_charger.h"
#if F_APP_APT_SUPPORT
#include "audio_passthrough.h"
#endif
#include "sysm.h"
#include "btm.h"
#include "ble_mgr.h"
#include "app_device.h"
#include "app_ble_device.h"
#include "app_bt_sniffing.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_dsp_cfg.h"
#include "app_dlps.h"
#include "app_report.h"
#include "app_key_process.h"
#include "app_key_gpio.h"
#include "app_charger.h"
#include "app_led.h"
#include "bt_hfp.h"
#include "bt_avrcp.h"
#include "bt_bond.h"
#include "remote.h"
#include "ringtone.h"
#include "voice_prompt.h"
#include "app_mmi.h"
#include "app_types.h"
#if F_APP_GPIO_ONOFF_SUPPORT
#include "app_gpio_on_off.h"
#endif
#include "engage.h"
#include "app_bt_policy_api.h"
#include "app_roleswap.h"
#if F_APP_LISTENING_MODE_SUPPORT
#include "app_listening_mode.h"
#endif
#if F_APP_BUZZER_SUPPORT
#include "app_buzzer.h"
#endif
#if AMA_FEATURE_SUPPORT
#include "app_ama_device.h"
#endif
#include "app_relay.h"
#include "app_sensor.h"
#include "app_multilink.h"
#include "app_audio_policy.h"
#include "app_hfp.h"
#include "app_a2dp.h"
#include "app_bond.h"
#include "app_iap_rtk.h"
#include "eq_utils.h"
#include "pm.h"
#include "app_dlps.h"
#include "app_adp.h"
#include "app_adp_cmd.h"
#include "app_ota.h"
#include "app_auto_power_off.h"
#include "app_bt_policy_int.h"
#include "app_bud_loc.h"
#include "app_ble_common_adv.h"
#include "app_roleswap_control.h"
#include "app_ble_service.h"
#include "app_vendor.h"

#if (F_APP_AIRPLANE_SUPPORT == 1)
#include "app_airplane.h"
#endif

#include "test_mode.h"
#include "app_cmd.h"
#include "app_ota_tooling.h"

#if F_APP_ANC_SUPPORT
#include "app_anc.h"
#include "anc.h"
#include "anc_tuning.h"
#endif

#if F_APP_MALLEUS_SUPPORT
#include "app_malleus.h"
#endif
#include "app_bt_policy_api.h"
#include "wdg.h"
#include "app_audio_passthrough.h"

#if (F_APP_SLIDE_SWITCH_SUPPORT == 1)
#include "app_slide_switch.h"
#endif
#include "app_sniff_mode.h"

#if F_APP_DURIAN_SUPPORT
#include "app_durian.h"
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
#include "app_dongle_common.h"
#include "app_dongle_spp.h"
#include "app_dongle_record.h"
#endif

#if F_APP_TEAMS_FEATURE_SUPPORT
#include "app_asp_device.h"
#include "app_teams_audio_policy.h"
#if F_APP_TEAMS_BT_POLICY
#include "app_teams_bt_policy.h"
#endif
#if F_APP_TEAMS_BLE_POLICY
#include "app_teams_ble_policy.h"
#endif
#include "app_teams_cmd.h"
#include "app_teams_extend_led.h"
#endif

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
#include "app_single_multilink_customer.h"
#elif F_APP_MUTLILINK_SOURCE_PRIORITY_UI
#include "app_multilink_customer.h"
#endif

#if (F_APP_VENDOR_SENSOR_INTERFACE_SUPPORT == 1)
#include "app_vendor_driver_interface.h"
#endif

#include "single_tone.h"

#if F_APP_DATA_CAPTURE_SUPPORT
#include "app_data_capture.h"
#endif

#if CONFIG_REALTEK_GFPS_FEATURE_SUPPORT
#include "app_gfps_device.h"
#include "app_gfps_finder.h"
#include "app_dult_device.h"
#include "app_dult.h"
#endif

#if F_APP_LEA_SUPPORT
#include "app_lea_mgr.h"
#endif

#if F_APP_SENSOR_MEMS_SUPPORT
#include "app_sensor_mems.h"
#endif

#include "app_loc_mgr.h"

#if F_APP_TUYA_SUPPORT
#include "rtk_tuya_ble_app_demo.h"
#endif

#include "app_ipc.h"

#include "hal_debug.h"

#if F_APP_COMMON_DONGLE_SUPPORT
#include "stdlib.h"
#endif

#if F_APP_HEARABLE_SUPPORT
#include "app_hearable.h"
#endif

#if CONFIG_REALTEK_GFPS_SASS_SUPPORT
#include "app_sass_policy.h"
#endif

#if F_APP_DISCHARGER_NTC_DETECT_PROTECT
#include "app_adc.h"
#endif

#if F_APP_GAMING_CHAT_MIXING_SUPPORT
#include "app_dongle_common.h"
#endif

#if (F_APP_PERIODIC_WAKEUP == 1)
#include "rtl876x_rtc.h"
#endif

static bool linkback_disable_power_off_fg = false;
static bool pairing_disable_power_off_fg = false;
static bool enable_pairing_complete_led = false;


static uint8_t device_timer_id = 0;
static uint8_t timer_idx_device_reboot = 0;
static uint8_t timer_idx_dut_mode_wait_link_disc = 0;
static uint8_t timer_idx_play_bud_role_tone = 0;

#if F_APP_ERWS_SUPPORT
static void app_device_new_pri_apply_app_db_info_when_roleswap_suc(void);
static T_ROLESWAP_APP_DB roleswap_app_db_temp = {0};
#endif

#if F_APP_TEAMS_BT_POLICY
uint8_t temp_master_device_name[40] = {0};
uint8_t temp_master_device_name_len = 0;
#endif

typedef enum
{
    APP_TIMER_DEVICE_REBOOT,
    APP_TIMER_DUT_MODE_WAIT_LINK_DISC,
    APP_TIMER_PLAY_BUD_ROLE_TONE,
} T_APP_DEVICE_TIMER;

static void app_device_timerout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_device_timerout_cb: timer_evt 0x%02x, param %d", timer_evt, param);

    switch (timer_evt)
    {
    case APP_TIMER_DEVICE_REBOOT:
        {
            app_stop_timer(&timer_idx_device_reboot);
            chip_reset((T_WDG_MODE)param);
        }
        break;

    case APP_TIMER_DUT_MODE_WAIT_LINK_DISC:
        {
            if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
            {
                app_stop_timer(&timer_idx_dut_mode_wait_link_disc);
                switch_into_single_tone_test_mode();
            }
            else
            {
                app_bt_policy_disconnect_all_link();
            }
        }
        break;

    case APP_TIMER_PLAY_BUD_ROLE_TONE:
        {
            if (ringtone_remaining_count_get() == 0 &&
                voice_prompt_remaining_count_get() == 0 &&
                tts_remaining_count_get() == 0)
            {
                app_stop_timer(&timer_idx_play_bud_role_tone);
                app_relay_async_single(APP_MODULE_TYPE_DEVICE, APP_REMOTE_MSG_REMOTE_SPK2_PLAY_SYNC);
            }
            else
            {

            }
        }
        break;

    default:
        break;
    }
}

void app_device_reboot(uint32_t timeout_ms, T_WDG_MODE wdg_mode)
{
    app_start_timer(&timer_idx_device_reboot, "device_reboot",
                    device_timer_id, APP_TIMER_DEVICE_REBOOT, wdg_mode, false,
                    timeout_ms);
    app_timer_register_pm_excluded(&timer_idx_device_reboot);
}

static bool app_device_disconn_need_play(uint8_t *bd_addr, uint16_t cause,
                                         bool is_b2s_connected_vp_played)
{
    bool need_play = true;

#if F_APP_DURIAN_SUPPORT
    if ((app_db.local_loc == BUD_LOC_IN_CASE) && (app_db.remote_loc == BUD_LOC_IN_CASE))
    {
        need_play = false;
    }
#endif

    if ((app_db.disconnect_inactive_link_actively == true) &&
        !(memcmp(app_db.resume_addr, bd_addr, 6)) &&
        (cause == (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE)))
    {
        //If link disconnection is initiated by roleswap module, it needn't play disconnect tone.
        need_play = false;
    }

    if (!is_b2s_connected_vp_played)
    {
        // needn't play disconn tone when conn tone not play
        need_play = false;
    }

    return need_play;
}

void app_device_factory_reset(void)
{
    uint8_t temp_first_engaged;
    uint8_t temp_case_battery;
    uint16_t temp_magic;

    app_ble_device_handle_factory_reset();

    if (app_cfg_const.enable_rtk_charging_box)
    {
#if F_APP_ADP_5V_CMD_SUPPORT || F_APP_ONE_WIRE_UART_SUPPORT
        app_adp_cmd_case_bat_check(&app_db.case_battery, &app_cfg_nv.case_battery);
#endif
    }

    temp_first_engaged = app_cfg_nv.first_engaged;
    temp_case_battery = app_cfg_nv.case_battery;
    temp_magic = app_cfg_nv.peer_valid_magic;

    app_cfg_reset();

    if (app_db.factory_reset_clear_mode == FACTORY_RESET_CLEAR_ALL)
    {
        bt_bond_clear();
    }
    else if (app_db.factory_reset_clear_mode == FACTORY_RESET_CLEAR_NORMAL)
    {
        app_bond_clear_non_rws_keys();
        app_cfg_nv.peer_valid_magic = temp_magic;
    }
    else if (app_db.factory_reset_clear_mode == FACTORY_RESET_CLEAR_PHONE)
    {
        app_bond_clear_non_rws_keys();
        app_cfg_nv.peer_valid_magic = temp_magic;
        app_cfg_nv.first_engaged = temp_first_engaged;
    }
    else if (app_db.factory_reset_clear_mode == FACTORY_RESET_CLEAR_CFG)
    {
        app_cfg_nv.peer_valid_magic = temp_magic;
        app_cfg_nv.first_engaged = temp_first_engaged;
    }
    else
    {
        /* no action */
    }

    if (app_cfg_const.enable_rtk_charging_box)
    {
        if (((temp_case_battery & 0x7F) > 100) || ((temp_case_battery & 0x7F) == 0))//invalid battery level
        {
            app_cfg_nv.case_battery = (temp_case_battery & 0x80) | 0x50;
        }
        else
        {
            app_cfg_nv.case_battery = temp_case_battery;
        }
    }

    remote_session_role_set((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);

    voice_prompt_volume_set(app_dsp_cfg_vol.voice_prompt_volume_default);
    ringtone_volume_set(app_dsp_cfg_vol.ringtone_volume_default);

    app_cfg_nv.factory_reset_done = 1;
    if (app_cfg_nv.adp_factory_reset_power_on)
    {
        app_cfg_nv.power_off_cause_cmd = 0;
        app_cfg_nv.auto_power_on_after_factory_reset = 0;
    }
    else if (app_db.ota_tooling_start)
    {
        app_cfg_nv.auto_power_on_after_factory_reset = 0;
    }
    else if (app_cfg_const.auto_power_on_after_factory_reset)
    {
        app_cfg_nv.auto_power_on_after_factory_reset = 1;
    }

    if (app_cfg_store_all() != 0)
    {
        APP_PRINT_ERROR0("app_device_factory_reset: save nv cfg data error");
    }

    app_ipc_publish(APP_DEVICE_IPC_TOPIC, APP_DEVICE_IPC_EVT_FACTORY_RESET, NULL);

#if F_APP_HEARABLE_SUPPORT
    app_ha_clear_param();
#endif
}
extern uint8_t Linklostflag ;
static void app_device_link_policy_ind(T_BP_EVENT event, T_BP_EVENT_PARAM *event_param)
{
    APP_PRINT_INFO1("app_device_link_policy_ind: event 0x%02x", event);

#if F_APP_AUTO_POWER_TEST_LOG
    TEST_PRINT_INFO1("app_device_link_policy_ind: event 0x%02x", event);
#endif

    if (event_param == NULL)
    {
        return;
    }

    switch (event)
    {
    case BP_EVENT_STATE_CHANGED:
        {
            if (event_param->is_shut_down)
            {
                if (app_db.device_state == APP_DEVICE_STATE_OFF_ING)
                {
                }
            }
            else
            {
                if (app_bt_policy_get_state() == BP_STATE_PAIRING_MODE)
                {
                    enable_pairing_complete_led = true;

                    pairing_disable_power_off_fg = true;

                    app_auto_power_off_disable(AUTO_POWER_OFF_MASK_PAIRING_MODE);

                    /*If press key0 from power off --> power on -->enter pairing mode, then release, will not play tone here*/
                    if (!app_key_is_enter_pairing() && !event_param->is_ignore)
                    {
                        app_audio_tone_type_play(TONE_PAIRING, false, false);
                    }
                    app_key_reset_enter_pairing();
                }
                else
                {
                    if (pairing_disable_power_off_fg)
                    {
                        pairing_disable_power_off_fg = false;

                        if (app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off &&
                            (app_link_get_b2s_link_num() != 0))
                        {
                            app_auto_power_off_enable(AUTO_POWER_OFF_MASK_PAIRING_MODE,
                                                      app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off);
                        }
                        else
                        {
                            app_auto_power_off_enable(AUTO_POWER_OFF_MASK_PAIRING_MODE, app_cfg_const.timer_auto_power_off);
                        }
                    }

                    if (app_bt_policy_get_state() != BP_STATE_CONNECTED)
                    {
                        enable_pairing_complete_led = false;
                    }
                }

                if (app_bt_policy_get_state() == BP_STATE_LINKBACK)
                {
                    linkback_disable_power_off_fg = true;
                    app_auto_power_off_disable(AUTO_POWER_OFF_MASK_LINKBACK);
#if F_APP_ERWS_SUPPORT
                    app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_LEGACY_LINK_CHANGE);
                    app_bt_sniffing_judge_dynamic_set_gaming_mode();
                    if (app_bt_policy_get_linkback_device())
                    {
                        app_hfp_adjust_sco_window(app_bt_policy_get_linkback_device(), APP_SCO_ADJUST_LINKBACK_EVT);
                    }
#endif
                }
                else
                {
                    if (linkback_disable_power_off_fg)
                    {
                        linkback_disable_power_off_fg = false;

                        if (app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off &&
                            (app_link_get_b2s_link_num() != 0))
                        {
                            app_auto_power_off_enable(AUTO_POWER_OFF_MASK_LINKBACK,
                                                      app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off);
                        }
                        else
                        {
                          if (!Linklostflag)
                           {
                              app_auto_power_off_enable(AUTO_POWER_OFF_MASK_LINKBACK, app_cfg_const.timer_auto_power_off);
                          	}
						   else
						   	{
                              app_auto_power_off_enable(AUTO_POWER_OFF_MASK_LINKBACK, app_cfg_const.timer_link_back_loss);
						    }
                        }

#if F_APP_ERWS_SUPPORT
                        app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_LEGACY_LINK_CHANGE);
                        app_bt_sniffing_judge_dynamic_set_gaming_mode();
#endif
                    }

                    if (app_link_get_b2s_link_num() > 1)
                    {
                        app_hfp_adjust_sco_window(NULL, APP_SCO_ADJUST_LINKBACK_END_EVT);
                    }
                }
            }
        }
        break;

    case BP_EVENT_RADIO_MODE_CHANGED:
        {
            T_BT_DEVICE_MODE radio_mode = app_bt_policy_get_radio_mode();
            app_ble_device_handle_radio_mode(radio_mode);
        }
        break;

    case BP_EVENT_ROLE_DECIDED:
        {
            T_BT_DEVICE_MODE radio_mode = app_bt_policy_get_radio_mode();
            app_ble_device_handle_engage_role_decided(radio_mode);

#if F_APP_DURIAN_SUPPORT
            app_durian_adv_bud_connected();
#endif

#if F_APP_LEA_SUPPORT
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_lea_mgr_update_sniff_mask();
            }

            mtc_check_reopen_mic();
#endif
        }
        break;

    case BP_EVENT_BUD_AUTH_SUC:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_auto_power_off_enable(AUTO_POWER_OFF_MASK_BUD_COUPLING, app_cfg_const.timer_auto_power_off);
            }
            else
            {
                app_auto_power_off_disable(AUTO_POWER_OFF_MASK_BUD_COUPLING);
            }

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
            if (app_db.ota_tooling_start)
            {
                if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                    (app_link_get_b2s_link_num() == 0))
                {
                    app_bt_policy_enter_ota_mode(true);
                    app_ota_tooling_adv_start(app_db.jig_dongle_id, 30);
                }
                else
                {
                    app_bt_policy_enter_ota_mode(false);
                }
            }
#endif

#if F_APP_DURIAN_SUPPORT
            app_durian_adv_bud_connected();
#endif
        }
        break;

    case BP_EVENT_BUD_DISCONN_NORMAL:
    case BP_EVENT_BUD_DISCONN_LOST:
        {
            if (event == BP_EVENT_BUD_DISCONN_NORMAL)
            {
                app_audio_tone_type_play(TONE_REMOTE_DISCONNECT, false, false);
            }
            else if (event == BP_EVENT_BUD_DISCONN_LOST)
            {
                app_audio_tone_type_play(TONE_REMOTE_LOSS, false, false);
            }

            app_auto_power_off_enable(AUTO_POWER_OFF_MASK_BUD_COUPLING, app_cfg_const.timer_auto_power_off);

#if F_APP_DURIAN_SUPPORT
            app_durian_link_bud_disconn();
#endif

            for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if (app_db.br_link[i].connected_profile & HFP_PROFILE_MASK)
                {
                    app_hfp_batt_level_report(app_db.br_link[i].bd_addr);
                }
            }
#if F_APP_LEA_SUPPORT
            for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
            {
                if (app_db.le_link[i].used == true &&
                    app_db.le_link[i].conn_handle != 0)
                {
                    app_lea_mgr_bas_batt_level_report(app_db.le_link[i].conn_handle);
                }
            }
#endif

            app_report_rws_bud_info();

#if F_APP_LISTENING_MODE_SUPPORT
            if (app_cfg_const.disallow_listening_mode_before_bud_connected)
            {
                app_listening_judge_conn_disc_evnet(APPLY_LISTENING_MODE_BUD_DISCONNECTED);
            }
#endif
        }
        break;

    case BP_EVENT_BUD_REMOTE_CONN_CMPL:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_audio_tone_type_play(TONE_REMOTE_CONNECTED, false, true);

                if (ringtone_remaining_count_get() == 0 &&
                    voice_prompt_remaining_count_get() == 0 &&
                    tts_remaining_count_get() == 0)
                {
                    app_relay_async_single(APP_MODULE_TYPE_DEVICE, APP_REMOTE_MSG_REMOTE_SPK2_PLAY_SYNC);
                }
                else
                {
                    app_start_timer(&timer_idx_play_bud_role_tone, "play_bud_role_tone",
                                    device_timer_id, APP_TIMER_PLAY_BUD_ROLE_TONE, 0, true,
                                    500);
                }
            }

            app_ble_device_handle_remote_conn_cmpl();

#if F_APP_LISTENING_MODE_SUPPORT
            if (app_cfg_const.disallow_listening_mode_before_bud_connected)
            {
                app_listening_judge_conn_disc_evnet(APPLY_LISTENING_MODE_BUD_CONNECTED);
            }
#endif

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
#if F_APP_MALLEUS_SUPPORT
                malleus_effect_trig();
#if F_APP_ERWS_SUPPORT
                malleus_relay_param();
#endif
#endif
            }

#if (F_APP_USER_EQ_SUPPORT && F_APP_ERWS_SUPPORT)
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_eq_sync_user_eq_when_connected();
            }
#endif
        }
        break;

    case BP_EVENT_BUD_REMOTE_DISCONN_CMPL:
        {
            app_ble_device_handle_remote_disconn_cmpl();

            if ((app_link_conn_profiles() & HFP_PROFILE_MASK) == 0)
            {
                app_hfp_set_call_status(APP_CALL_IDLE);
            }

            if ((app_link_conn_profiles() & A2DP_PROFILE_MASK) == 0)
            {
                app_audio_set_bud_stream_state(BUD_STREAM_STATE_IDLE);
            }
        }
        break;

    case BP_EVENT_BUD_LINKLOST_TIMEOUT:
        {
            app_db.power_off_cause = POWER_OFF_CAUSE_SEC_LINKLOST_TIMEOUT;
            app_mmi_handle_action(MMI_DEV_POWER_OFF);
        }
        break;

    case BP_EVENT_SRC_AUTH_SUC:
        {

            T_APP_BR_LINK *p_link = NULL;
            p_link = app_link_find_br_link(event_param->bd_addr);
            if (p_link != NULL)
            {
                p_link->cmd.tx_mask |= TX_ENABLE_AUTHEN_BIT;
#if F_APP_GATT_OVER_BREDR_SUPPORT
                p_link->cmd.tx_mask |= TX_ENABLE_CCCD_BIT;
#endif
            }

#if F_APP_IAP_RTK_SUPPORT
            app_iap_rtk_create(event_param->bd_addr);
#endif
            app_ble_device_handle_b2s_bt_connected(event_param->bd_addr);

#if F_APP_TEAMS_FEATURE_SUPPORT
            if (!app_teams_is_launch())
            {
                teams_extend_led_control_when_power_on();
            }
#endif
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
            T_APP_BR_LINK *p_teams_link = app_link_find_br_link(event_param->bd_addr);
            if (p_teams_link)
            {
                app_teams_multilink_handle_link_connected(p_teams_link->id);
            }
#elif F_APP_MUTLILINK_SOURCE_PRIORITY_UI
            T_APP_BR_LINK *p_speaker_link = app_link_find_br_link(event_param->bd_addr);
            if (p_speaker_link)
            {
                app_multilink_customer_handle_link_connected(p_speaker_link->id);
            }
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
            if (app_dongle_get_connected_dongle_link() == NULL)
            {
                app_audio_update_dongle_flag(false);
            }
#endif

            app_auto_power_off_disable(AUTO_POWER_OFF_MASK_SOURCE_LINK);

            if (app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off)
            {
                app_auto_power_off_enable(AUTO_POWER_OFF_MASK_SOURCE_LINK,
                                          app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off);
            }

            app_relay_async_single(APP_MODULE_TYPE_BT_POLICY, BT_POLICY_MSG_B2S_ACL_CONN);
            app_bt_policy_b2s_conn_start_timer();

#if F_APP_DURIAN_SUPPORT
            app_durian_link_src_connect();
#endif

#if F_APP_ANC_SUPPORT
            if (app_anc_is_anc_on_state(app_db.current_listening_state))
            {
                if (app_cfg_const.rws_sniff_negotiation)
                {
                    app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_ANC_APT);
                }
            }
            else
            {
#if F_APP_APT_SUPPORT
                if (app_apt_is_apt_on_state(app_db.current_listening_state))
                {
                    if (app_cfg_const.rws_sniff_negotiation)
                    {
                        app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_ANC_APT);
                    }
                }
                else
#endif
                {
                    if (app_cfg_const.rws_sniff_negotiation)
                    {
                        app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_ANC_APT);
                    }
                }

            }
#endif

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
            if (app_db.ota_tooling_start)
            {
                app_ota_tooling_adv_stop();
                app_bt_policy_enter_ota_mode(false);
            }
#endif
            app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_LAST_CONN_INDEX);
        }
        break;

    case BP_EVENT_SRC_AUTH_FAIL:
        {
            app_audio_tone_type_play(TONE_PAIRING_FAIL, false, false);
        }
        break;

    case BP_EVENT_SRC_DISCONN_LOST:
        {
#if F_APP_BUZZER_SUPPORT
            buzzer_set_mode(LINK_LOSS_BUZZER);
#endif
            if (app_db.device_state == APP_DEVICE_STATE_ON)
            {
                bool link_loss_play = true;

                link_loss_play = app_device_disconn_need_play(event_param->bd_addr, event_param->cause,
                                                              event_param->is_b2s_connected_vp_played);

                if (event_param->is_first_src)
                {
                    if (link_loss_play)
                    {
#if F_APP_TEAMS_FEATURE_SUPPORT
                        app_teams_audio_disconnected_state_vp_play(event_param->bd_addr);
#else
                        app_audio_tone_type_play(TONE_LINK_LOSS, false, true);
#endif
                    }
                }
                else
                {
#if F_APP_TEAMS_FEATURE_SUPPORT
                    app_teams_audio_disconnected_state_vp_play(event_param->bd_addr);
#else
                    app_audio_tone_type_play(TONE_LINK_LOSS2, false, true);
#endif
                }
            }

            if (0 == app_link_get_b2s_link_num())
            {
                app_auto_power_off_enable(AUTO_POWER_OFF_MASK_SOURCE_LINK, app_cfg_const.timer_auto_power_off);
            }

#if F_APP_DURIAN_SUPPORT
            app_durian_link_src_disconn();
#endif

            memcpy(app_db.bt_addr_disconnect, event_param->bd_addr, 6);

            app_ble_device_handle_b2s_bt_disconnected(event_param->bd_addr);

#if F_APP_IAP_RTK_SUPPORT
            app_iap_rtk_delete(event_param->bd_addr);
#endif
        }
        break;

    case BP_EVENT_SRC_DISCONN_NORMAL:
        {
#if F_APP_ANC_SUPPORT
            //For ANC secondary measurement
            app_anc_wdg_reset();
#endif

            if (app_db.device_state == APP_DEVICE_STATE_ON)
            {
#if (F_APP_AIRPLANE_SUPPORT == 1)
                if (!app_airplane_mode_get())
#endif
                {
                    bool link_disconn_play = true;
                    link_disconn_play = app_device_disconn_need_play(event_param->bd_addr, event_param->cause,
                                                                     event_param->is_b2s_connected_vp_played);
                    if (link_disconn_play)
                    {
                        if (event_param->is_first_src)
                        {
#if F_APP_TEAMS_FEATURE_SUPPORT
                            app_teams_audio_disconnected_state_vp_play(event_param->bd_addr);
#else
                            app_audio_tone_type_play(TONE_LINK_DISCONNECT, false, true);
#endif
                        }
                        else
                        {
#if F_APP_TEAMS_FEATURE_SUPPORT
                            app_teams_audio_disconnected_state_vp_play(event_param->bd_addr);
#else
                            app_audio_tone_type_play(TONE_LINK_DISCONNECT2, false, true);
#endif
                        }
                    }
                }
            }

            if (0 == app_link_get_b2s_link_num())
            {
#if CONFIG_REALTEK_GFPS_FEATURE_SUPPORT
                if (extend_app_cfg_const.gfps_support)
                {
                    app_gfps_device_handle_b2s_link_disconnected();
                }
#endif

                app_auto_power_off_enable(AUTO_POWER_OFF_MASK_SOURCE_LINK, app_cfg_const.timer_auto_power_off);
            }

#if F_APP_DURIAN_SUPPORT
            app_durian_link_src_disconn();
#endif

            memcpy(app_db.bt_addr_disconnect, event_param->bd_addr, 6);
            app_ble_device_handle_b2s_bt_disconnected(event_param->bd_addr);
        }
        break;

    case BP_EVENT_PAIRING_MODE_TIMEOUT:
        {
            app_audio_tone_type_play(TONE_PAIRING_TIMEOUT, false, false);
#if F_APP_DURIAN_SUPPORT
            app_durian_adv_pairing_timeout();
#endif
        }
        break;

    case BP_EVENT_DEDICATED_PAIRING:
        {
            if (app_db.avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
            {
#if F_APP_TEAMS_BT_POLICY
#else
                app_mmi_handle_action(MMI_AV_PLAY_PAUSE);
#endif
            }
#if F_APP_BUZZER_SUPPORT
            buzzer_set_mode(ENTER_PAIRING_BUZZER);
#endif
            app_ble_device_handle_enter_pair_mode();
        }
        break;

    case BP_EVENT_PROFILE_CONN_SUC:
        {
             static uint8_t is_first_src_connect = 0;
            if (event_param->is_first_prof)
            {
                if (enable_pairing_complete_led)
                {
                    enable_pairing_complete_led = false;
#if F_APP_BUZZER_SUPPORT
                    buzzer_set_mode(PAIRING_COMPLETE_BUZZER);
#endif
                    app_led_change_mode(LED_MODE_PAIRING_COMPLETE, true, false);
                }

                T_APP_BR_LINK *p_link = app_link_find_br_link(event_param->bd_addr);
                if (!p_link)
                {
                    break;
                }

                if ((memcmp(app_db.resume_addr, event_param->bd_addr, 6) != 0) ||
                    (app_db.disallow_connected_tone_after_inactive_connected == false))
                {
                    if (!p_link->b2s_connected_vp_is_played)
                    {
#if F_APP_TEAMS_FEATURE_SUPPORT
                        extern uint8_t conn_to_new_device;
                        app_teams_audio_connected_state_vp_play(event_param->bd_addr, conn_to_new_device);
#else
                        if (app_cfg_const.only_primary_bud_play_connected_tone)
                        {
                            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                            {
                                app_audio_tone_type_play(TONE_LINK_CONNECTED, false, false);
                            }
                        }
                        else
                        {
                           if(!app_link_check_dongle_link(event_param->bd_addr))
                                app_audio_tone_type_play(TONE_LINK_CONNECTED, false, false);
						   else
						    	{
						    	 if(app_hfp_get_call_status() != APP_CALL_ACTIVE)
						    	  {
						    	    extern void set_dongle_enter_flag(uint8_t flag);
						    	    set_dongle_enter_flag(0);
						    	    app_audio_tone_type_play(TONE_APT_VOL_3, false, false);
						    	 }
						   	  }
						   	
						   
                        }
#endif 
  
                        APP_PRINT_INFO1("app_device_link_policy_ind: is_first_src_connect %d ",is_first_src_connect);
                        //Enable battery report when first phone connected
                       // if (app_cfg_const.enable_bat_report_when_phone_conn && event_param->is_first_src)
                       if (app_cfg_const.enable_bat_report_when_phone_conn && !is_first_src_connect)
                        {
                            uint8_t bat_level = 0;
                            uint8_t state_of_charge = app_db.local_batt_level;
							is_first_src_connect = 1;

                            state_of_charge = ((state_of_charge + 9) / 10) * 10;
                            bat_level = state_of_charge / 10;

                            app_audio_tone_type_play((T_APP_AUDIO_TONE_TYPE)(TONE_BATTERY_PERCENTAGE_10 + bat_level - 1), false,
                                                     false);
                        }
                    }
                }
                else
                {
                    memset(app_db.resume_addr, 0, 6);
                }

                app_db.disallow_connected_tone_after_inactive_connected = false;
                p_link->b2s_connected_vp_is_played = true;

#if (F_APP_SENSOR_SUPPORT == 1)
                if (LIGHT_SENSOR_ENABLED && (app_cfg_const.enable_rtk_charging_box == 0))
                {
                    app_sensor_ld_start();
                    app_relay_async_single(APP_MODULE_TYPE_BUD_LOC, APP_BUD_LOC_EVENT_SENSOR_LD_CONFIG);
                }
#endif

#if F_APP_DURIAN_SUPPORT
                app_durian_link_profile_conn();
#endif
            }

#if CONFIG_REALTEK_GFPS_SASS_SUPPORT
            if (extend_app_cfg_const.gfps_sass_support)
            {
                app_sass_policy_profile_conn_handle(app_link_find_br_link(event_param->bd_addr)->id);
            }
#endif

        }
        break;

    case BP_EVENT_PROFILE_DISCONN:
        {
            ENGAGE_PRINT_TRACE3("app_device_link_policy_ind: disconn %s, prof 0x%08x, is_last %d",
                                TRACE_BDADDR(event_param->bd_addr), event_param->prof, event_param->is_last_prof);

            if (memcmp(app_db.br_link[app_a2dp_get_active_idx()].bd_addr, event_param->bd_addr, 6) == 0)
            {
                if (event_param->prof == A2DP_PROFILE_MASK)
                {
                    app_audio_set_avrcp_status(BT_AVRCP_PLAY_STATUS_STOPPED);
                    app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_PLAY_STATUS);

                    if (app_hfp_get_call_status() == APP_CALL_IDLE)
                    {
                        app_audio_set_bud_stream_state(BUD_STREAM_STATE_IDLE);
                    }
                }
            }

            if ((app_link_conn_profiles() & (HFP_PROFILE_MASK | A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK)) == 0)
            {
                if (app_hfp_get_call_status() != APP_CALL_IDLE)
                {
                    app_audio_set_bud_stream_state(BUD_STREAM_STATE_IDLE);
                }
            }

            if (event_param->prof == SPP_PROFILE_MASK)
            {
                if (app_ota_link_loss_stop())
                {
                    app_ota_error_clear_local(OTA_SPP_DISC);
                }

#if F_APP_ANC_SUPPORT
                T_ANC_FEATURE_MAP feature_map;
                feature_map.d32 = anc_tool_get_feature_map();

                if (feature_map.user_mode == DISABLE &&
                    app_anc_get_measure_mode() != ANC_RESP_MEAS_MODE_EXIT)
                {
                    if ((anc_tool_check_resp_meas_mode()) &&
                        (event_param->is_last_prof) &&
                        (app_anc_get_measure_mode() == ANC_RESP_MEAS_MODE_NONE))
                    {
                        //Reset to normal mode and reboot system in case of
                        //terminating ANC design tool without exiting ANC measurement mode
                        app_anc_enter_measure_mode((uint8_t)ANC_RESP_MEAS_MODE_NONE);
                    }
                    else
                    {
                        app_anc_exit_test_mode(feature_map.mic_path);
                        if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                        {
                            app_relay_async_single(APP_MODULE_TYPE_ANC, APP_REMOTE_MSG_EXIT_TEST_MODE);
                        }
                    }
                }
#endif

#if F_APP_APT_SUPPORT
                if (app_apt_llapt_scenario_is_busy())
                {
                    app_apt_exit_llapt_scenario_choose_mode();

                    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                    {
                        app_relay_async_single(APP_MODULE_TYPE_APT, APP_REMOTE_MSG_EXIT_LLAPT_CHOOSE_MODE);
                    }
                }
#endif
#if F_APP_ANC_SUPPORT
                if (app_anc_scenario_select_is_busy())
                {
                    app_anc_exit_scenario_select_mode();

                    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                    {
                        app_relay_async_single(APP_MODULE_TYPE_ANC, APP_REMOTE_MSG_EXIT_ANC_CHOOSE_MODE);
                    }
                }
#endif
            }

            if (event_param->prof == SPP_PROFILE_MASK || event_param->prof == IAP_PROFILE_MASK)
            {
                if (app_roleswap_ctrl_get_status() == APP_ROLESWAP_STATUS_IDLE)
                {
                    app_cmd_update_eq_ctrl(app_cmd_get_tool_connect_status(), true);
                }
            }

            T_APP_BR_LINK *p_link;
            p_link = app_link_find_br_link(event_param->bd_addr);

            if (p_link != NULL &&
                ((p_link->connected_profile & (HFP_PROFILE_MASK | A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK)) == 0))
            {
                if (((p_link->connected_profile & SPP_PROFILE_MASK) != 0)
#if F_APP_DATA_CAPTURE_SUPPORT
                    && (app_data_capture_get_state() == 0)
#endif
                   )
                {
#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
                    if (app_cfg_const.enable_24g_bt_audio_source_switch &&
                        app_link_check_dongle_link(event_param->bd_addr))
                    {
                        /* keep dongle spp profile */
                    }
                    else
#endif
                    {
                        app_bt_policy_disconnect(app_db.br_link[p_link->id].bd_addr, SPP_PROFILE_MASK);
                    }
                }
            }

            if (p_link && event_param->prof == A2DP_PROFILE_MASK)
            {
                p_link->streaming_fg = false;
                p_link->avrcp_ready_to_pause = false;
                if (app_link_get_a2dp_start_num() == 0)
                {
                    app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_A2DP);

                    app_bt_policy_primary_engage_action_adjust();
                }
#if F_APP_DYNAMIC_ADJUST_B2B_TX_POWER
                app_bt_policy_dynamic_adjust_b2b_tx_power(BP_TX_POWER_A2DP_STOP);
#endif
            }
        }
        break;

    default:
        break;
    }
}

void app_device_state_change(T_APP_DEVICE_STATE state)
{
    uint8_t report_data = 0;
    APP_PRINT_INFO2("app_device_state_change: cur_state 0x%02x, next_state 0x%02x",
                    app_db.device_state, state);
    app_db.device_state = state;
    report_data = state;
    if (app_cfg_const.enable_data_uart)
    {
        app_report_event(CMD_PATH_UART, EVENT_DEVICE_STATE, 0, &report_data, sizeof(report_data));
    }
}

static void app_device_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;
    bool ota_tooling_not_force_engage = false;
    bool airplane_mode = false;

    switch (event_type)
    {
    case SYS_EVENT_POWER_ON:
        {
            if (app_cfg_const.open_dbg_log_for_system_busy == 1)
            {
                hal_debug_init();
                hal_debug_task_time_proportion_init(100);
            }
#if F_APP_DEBUG_TASK_PROFILING
            hal_debug_init();
            hal_debug_task_time_proportion_init(5000);
#endif
            app_cfg_nv.gpio_out_detected = 0;
            app_cfg_nv.app_is_power_on = 1;
            app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);

            APP_PRINT_INFO5("app_device_dm_cback: bud_role %d, factory_reset_done %d, first_engaged %d, key enter pairing %d, %d",
                            app_cfg_nv.bud_role, app_cfg_nv.factory_reset_done, app_cfg_nv.first_engaged,
                            app_key_is_enter_pairing(), app_cfg_const.enable_multi_link);

            app_device_state_change(APP_DEVICE_STATE_ON);

            app_vendor_enable_send_tx_right_away();

            app_sniff_mode_startup();

            pairing_disable_power_off_fg = false;
            linkback_disable_power_off_fg = false;
            app_dlps_enable_auto_poweroff_stop_wdg_timer();

            app_db.mic_mp_verify_pri_sel_ori = APP_MIC_SEL_DISABLE;
            app_db.power_off_cause = POWER_OFF_CAUSE_UNKNOWN;
            app_db.play_min_max_vol_tone = true;

            app_db.disallow_play_gaming_mode_vp = false;
            app_db.hfp_report_batt = true;

            if (app_cfg_const.enable_enter_gaming_mode_after_power_on)
            {
                app_db.enter_gaming_mode_after_power_on = true;
            }

            app_db.recovery_connected = false;
            app_db.pre_recovery_connected = false;

            if (app_db.ignore_led_mode)
            {
                app_db.ignore_led_mode = false;
            }
            else
            {
                app_led_change_mode(LED_MODE_POWER_ON, true, false);
            }

            app_key_check_vol_mmi();
#if F_APP_KEY_EXTEND_FEATURE
            if (app_cfg_nv.reboot_after_ota)
            {
                app_cfg_nv.reboot_after_ota = 0;
                app_key_reset_user_param();
#if F_APP_RWS_KEY_SUPPORT
                app_key_reset_rws_key_user_param(true);
#endif
            }
#endif

            //Update battery volume after power on
            if (app_cfg_const.discharger_support)
            {
                app_charger_update();
            }

            // clear this flag when power on to prevent flag not clear
            if (app_cfg_nv.report_box_bat_lv_again_after_sw_reset)
            {
                app_cfg_nv.report_box_bat_lv_again_after_sw_reset = false;
                app_cfg_store(&app_cfg_nv.offset_notification_vol, 8);
                app_cfg_store(&app_cfg_nv.case_battery, 4);
            }

            //reset to default eq after power on
            if (app_cfg_const.reset_eq_when_power_on)
            {
                app_cfg_nv.eq_idx_normal_mode_record = app_eq_get_default_idx(SPK_SW_EQ, NORMAL_MODE);
                app_cfg_nv.eq_idx_gaming_mode_record = app_eq_get_default_idx(SPK_SW_EQ, GAMING_MODE);
                app_cfg_nv.eq_idx_anc_mode_record = app_eq_get_default_idx(SPK_SW_EQ, ANC_MODE);
#if F_APP_LINEIN_SUPPORT
                app_cfg_nv.eq_idx_line_in_mode_record = app_eq_get_default_idx(MIC_SW_EQ, APT_MODE);
#endif

                if (app_cfg_const.enable_enter_gaming_mode_after_power_on)
                {
                    app_cfg_nv.eq_idx = app_cfg_nv.eq_idx_gaming_mode_record;
                }
                else
                {
                    app_cfg_nv.eq_idx = app_cfg_nv.eq_idx_normal_mode_record;
                }
            }

#if F_APP_BUZZER_SUPPORT
            buzzer_set_mode(POWER_ON_BUZZER);
#endif

#if F_APP_ANC_SUPPORT
            if (anc_tool_check_resp_meas_mode() == ANC_RESP_MEAS_MODE_NONE)
            {
                app_ble_device_handle_power_on();
            }
#else
            app_ble_device_handle_power_on();
#endif

#if (F_APP_VENDOR_SENSOR_INTERFACE_SUPPORT == 1)
            if (app_cfg_const.customized_driver_support)
            {
                app_vendor_driver_start_device();
            }
#endif

            if (app_cfg_const.enable_dlps)
            {
                power_mode_set(POWER_DLPS_MODE);
            }

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
            if (app_db.ota_tooling_start && (app_db.jig_subcmd != APP_ADP_SPECIAL_CMD_RWS_FORCE_ENGAGE))
            {
                ota_tooling_not_force_engage = true;
                APP_PRINT_INFO2("app_device_dm_cback: ota_tooling_start %d, jig_subcmd %d",
                                app_db.ota_tooling_start, app_db.jig_subcmd);
            }
#endif

#if F_APP_LISTENING_MODE_SUPPORT
            //before airplane_power_on_handle
            app_listening_apply_when_power_on();
#endif

#if (F_APP_AIRPLANE_SUPPORT == 1)
            //before bt_policy_startup
            if (app_airplane_combine_key_power_on_get() || app_cfg_const.airplane_mode_when_power_on)
            {
                app_airplane_power_on_handle();
            }

            if (app_airplane_mode_get())
            {
                airplane_mode = true;
                APP_PRINT_INFO0("app_device_dm_cback: airplane mode");
            }
#endif

            if (!app_cfg_nv.factory_reset_done || app_key_is_enter_pairing()
                || ota_tooling_not_force_engage
                || airplane_mode
               )
            {
                app_bt_policy_startup(app_device_link_policy_ind, false);
            }
            else
            {
                app_bt_policy_startup(app_device_link_policy_ind, true);
            }

            if (!app_cfg_nv.factory_reset_done && !app_device_is_in_the_box()
                && app_cfg_const.auto_power_on_and_enter_pairing_mode_before_factory_reset)
            {
                app_bt_policy_enter_pairing_mode(false, true);
            }

#if (F_APP_ADP_5V_CMD_SUPPORT == 1) && (F_APP_OTA_TOOLING_SUPPORT == 1)
            if (app_db.ota_tooling_start)
            {
                app_adp_cmd_special_cmd_handle(app_db.jig_subcmd, app_db.jig_dongle_id);
            }
#endif

#if F_APP_DURIAN_SUPPORT
            app_durian_cfg_power_on();
#endif

            app_auto_power_off_enable(AUTO_POWER_OFF_MASK_POWER_ON, app_cfg_const.timer_auto_power_off);
            app_dlps_enable(APP_DLPS_ENTER_CHECK_WAIT_RESET);
        }
        break;

    case SYS_EVENT_POWER_OFF:
        {
            mp_hci_test_set_mode(false);

#if F_APP_BUZZER_SUPPORT
            buzzer_set_mode(POWER_OFF_BUZZER);
#endif

            app_ble_device_handle_power_off();
            app_auto_power_off_enable(~AUTO_POWER_OFF_MASK_ALREADY_POWER_OFF,
                                      app_cfg_const.timer_auto_power_off);

            if (app_cfg_const.discharger_support)
            {
                app_cfg_nv.local_level = app_db.local_batt_level;
                app_cfg_nv.remote_level = app_db.remote_batt_level;
            }

#if F_APP_DURIAN_SUPPORT
            app_durian_cfg_power_off();
#endif

            app_cfg_nv.app_is_power_on = 0;
            app_cfg_store_all();

            app_bt_policy_stop_a2dp_and_sco();
            app_bt_policy_shutdown();

#if (F_APP_AIRPLANE_SUPPORT == 1)
            //after app_bt_policy_shutdown
            app_airplane_power_off_handle();
#endif
            app_bond_clear_sec_diff_link_record();

#if (F_APP_VENDOR_SENSOR_INTERFACE_SUPPORT == 1)
            if (app_cfg_const.customized_driver_support)
            {
                app_vendor_driver_handle_power_state(SENSOR_POWER_LOW_POWER_OFF_MODE);
            }
#endif

            app_dlps_power_off();
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_device_dm_cback: event_type 0x%04x", event_type);
    }
}

/* @brief  wheather the test equipment is required device
*
* @param  name and addr
* @return true/false
*/
static bool app_device_is_test_equipment(const char *name, uint8_t *addr)
{
    T_APP_TEST_EQUIPMENT_INFO equipment_info[] =
    {
        {{0x00, 0x30, 0xD3}, "AGILENT TECHNOLOGIES"},
        {{0x8C, 0x14, 0x7D}, "AGILENT TECHNOLOGIES"},
        {{0x00, 0x04, 0x43}, "AGILENT TECHNOLOGIES"},
        {{0xBD, 0xBD, 0xBD}, "AGILENT TECHNOLOGIES"},
        {{0x00, 0x02, 0xB1}, "ANRITSU MT8852"},
        {{0x00, 0x00, 0x91}, "ANRITSU MT8852"},
        {{0x00, 0x00, 0x91}, "ANRITSU MT8850A Bluetooth Test set"},
        {{0x70, 0xB3, 0xD5}, "R&S CMW"},
        {{0x00, 0x90, 0xB8}, "R&S CMW"},
        {{0xDC, 0x44, 0x27}, "R&S CMW"},
        {{0x12, 0x34, 0x56}, "R&S CMW"},
        {{0x70, 0xB3, 0xD5}, "R&S CMU"},
        {{0x00, 0x90, 0xB8}, "R&S CMU"},
        {{0xDC, 0x44, 0x27}, "R&S CMU"},
        {{0x70, 0xB3, 0xD5}, "R&S CBT"},
        {{0x00, 0x90, 0xB8}, "R&S CBT"},
        {{0xDC, 0x44, 0x27}, "R&S CBT"},
        {{0x12, 0x34, 0x56}, "R&S CBT"},
    };

    bool ret = false;
    uint8_t i = 0;
    uint8_t device_oui[3] = {0};

    device_oui[0] = addr[5];
    device_oui[1] = addr[4];
    device_oui[2] = addr[3];

    for (i = 0; i < sizeof(equipment_info) / sizeof(T_APP_TEST_EQUIPMENT_INFO); i++)
    {
        if (!strncmp(equipment_info[i].name, name, strlen(equipment_info[i].name)) &&
            !memcmp(device_oui, equipment_info[i].oui, sizeof(equipment_info[i].oui)))
        {
            ret = true;
            break;
        }
    }

    APP_PRINT_TRACE3("app_device_is_test_equipment: ret %d name %s oui %b", ret, TRACE_STRING(name),
                     TRACE_BINARY(3, device_oui));

    return ret;
}

static bool app_device_auto_power_on_before_factory_reset(void)
{
    bool ret = false;
    uint8_t state_of_charge;

    state_of_charge = app_charger_get_soc();

    if (state_of_charge > BAT_CAPACITY_0)
    {
        if (app_cfg_const.auto_power_on_and_enter_pairing_mode_before_factory_reset &&
            !app_cfg_nv.factory_reset_done &&
            !app_device_is_in_the_box())
        {
            ret = true;
        }
    }

    APP_PRINT_INFO1("app_device_auto_power_on_before_factory_reset: ret = %d", ret);
    return ret;
}

static bool app_device_boot_up_directly(void)
{
    bool ret = false;
    bool is_in_box = false;
    uint8_t bat_capacity = app_charger_get_soc();

    APP_PRINT_INFO5("app_device_boot_up_directly: %d, %d, %d, %d, bat_capacity: %d",
                    app_cfg_nv.adp_factory_reset_power_on, app_cfg_nv.ota_tooling_start,
                    app_cfg_nv.power_off_cause_cmd, app_db.executing_charger_box_special_cmd, bat_capacity);

    if (bat_capacity == BAT_CAPACITY_0)
    {
        goto boot_up_directly;
    }

    if (!app_cfg_const.mfb_replace_key0)
    {
        if (app_dlps_check_short_press_power_on())
        {
            ret = true;
            goto boot_up_directly;
        }
    }

    if (app_device_auto_power_on_before_factory_reset())
    {
        ret = true;
        goto boot_up_directly;
    }

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
    if (app_cfg_nv.ota_tooling_start)
    {
        // Add protect for timing issue
        if (app_db.executing_charger_box_special_cmd == 0)
        {
            app_db.local_loc = BUD_LOC_IN_CASE;

            // Update data to ram
            app_db.ota_tooling_start = app_cfg_nv.ota_tooling_start;
            app_db.jig_subcmd = app_cfg_nv.jig_subcmd;
            app_db.jig_dongle_id = app_cfg_nv.jig_dongle_id;

            if (app_db.jig_subcmd == APP_ADP_SPECIAL_CMD_RWS_FORCE_ENGAGE)
            {
                app_cfg_const.rws_pairing_required_rssi = 0x7F;
            }

            // Clear flash data
            app_cfg_nv.ota_tooling_start = 0;
            app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);

            app_cfg_nv.jig_subcmd = 0;
            app_cfg_nv.jig_dongle_id = 0;
            app_cfg_store(&app_cfg_nv.offset_notification_vol, 8);

            ret = true;
            goto boot_up_directly;
        }
    }
#endif

    if (app_cfg_const.enable_rtk_charging_box)
    {
        if (app_cfg_nv.adp_factory_reset_power_on)
        {
            if (!app_cfg_nv.power_off_cause_cmd)
            {
                app_db.power_on_by_cmd = true;
                app_db.local_loc = app_loc_mgr_local_detect();

#if F_APP_ADP_5V_CMD_SUPPORT || F_APP_ONE_WIRE_UART_SUPPORT
                app_adp_cmd_case_bat_check(&app_cfg_nv.case_battery, &app_db.case_battery);
#endif

                app_cfg_nv.adp_factory_reset_power_on = 0;

                ret = true;
            }
            else
            {
                app_cfg_nv.power_off_cause_cmd = 0;
                app_cfg_store(&app_cfg_nv.remote_loc, 4);
                app_cfg_nv.adp_factory_reset_power_on = 0;
            }

#if F_APP_DURIAN_SUPPORT
            app_durian_adv_set_power_on_cause();
#endif
            app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);
            goto boot_up_directly;
        }
    }

    if (app_cfg_nv.auto_power_on_after_factory_reset)
    {
        app_cfg_nv.auto_power_on_after_factory_reset = 0;
        app_cfg_store(&app_cfg_nv.eq_idx_gaming_mode_record, 4);

        ret = true;
        goto boot_up_directly;
    }

    if (app_cfg_const.charging_disallow_power_on && (app_adp_get_plug_state() == ADAPTOR_PLUG))
    {
        goto boot_up_directly;
    }

    is_in_box = app_device_is_in_the_box();

    if (app_cfg_const.box_detect_method == ADAPTOR_DETECT)
    {
        if (!is_in_box)
        {
            ret = true;
        }
    }
#if F_APP_GPIO_ONOFF_SUPPORT
    else if (app_cfg_const.box_detect_method == GPIO_DETECT)
    {
        if (is_in_box)
        {
            app_cfg_nv.pre_3pin_status_unplug = 0;
        }
        else
        {
            app_cfg_nv.pre_3pin_status_unplug = 1;
            ret = true;
        }
    }
#endif
    else // DISABLE_DETECT
    {
        if (app_cfg_nv.app_is_power_on)
        {
            ret = true;
        }
    }

    app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);

boot_up_directly:
    APP_PRINT_INFO1("app_device_boot_up_directly: ret %d", ret);
    return ret;
}

static bool app_device_power_on_check(void)
{
    bool ret = false;
    uint8_t bat_capacity = app_charger_get_soc();
    APP_PRINT_INFO3("app_device_power_on_check: %d, %d, bat_capacity: %d",
                    app_cfg_nv.gpio_out_detected, app_cfg_nv.pre_3pin_status_unplug, bat_capacity);

    if (bat_capacity == BAT_CAPACITY_0)
    {
        return ret;
    }

    if (app_cfg_const.box_detect_method == ADAPTOR_DETECT)
    {
        if (app_cfg_const.enable_outbox_power_on)
        {
            if (!app_device_is_in_the_box())
            {
                if ((app_db.wake_up_reason & WAKE_UP_ADP) || app_cfg_nv.adaptor_changed)
                {
                    // ADP 5V outbox to power on
                    ret = true;
                }
                else if ((!(app_db.wake_up_reason & WAKE_UP_MFB))
                         && (!(app_db.wake_up_reason & WAKE_UP_COMBINE_KEY_POWER_ONOFF))
                         && (!app_cfg_nv.is_app_reboot)
#if F_APP_CAP_TOUCH_SUPPORT
                         && (!(app_db.wake_up_reason & WAKE_UP_CTC))
#endif
                        )
                {
                    // bud is outbox, then do HW reset to power on
                    ret = true;
                }
            }
        }
    }
#if F_APP_GPIO_ONOFF_SUPPORT
    else if (app_cfg_const.box_detect_method == GPIO_DETECT)
    {
        if (!app_device_is_in_the_box())
        {
            if (app_cfg_nv.factory_reset_done)
            {
                if (app_cfg_nv.is_app_reboot)
                {
                    if (!app_cfg_nv.pre_3pin_status_unplug)
                    {
                        ret = true;
                    }
                }
                else if ((app_db.wake_up_reason & WAKE_UP_3PIN) || // 3pin wake up
                         (app_cfg_nv.gpio_out_detected == 1) || // 3pin wake up
                         (app_cfg_nv.pre_3pin_status_unplug == 0) || // Abnormal reset
                         ((!(app_db.wake_up_reason & WAKE_UP_MFB)) &&
                          (!(app_db.wake_up_reason & WAKE_UP_COMBINE_KEY_POWER_ONOFF))
#if F_APP_CAP_TOUCH_SUPPORT
                          && (!(app_db.wake_up_reason & WAKE_UP_CTC))
#endif
                         )) // 5v wake up
                {
                    ret = true;
                }
            }

            app_cfg_nv.gpio_out_detected = 0;
            app_cfg_nv.pre_3pin_status_unplug = 1;
        }
        else
        {
            app_cfg_nv.pre_3pin_status_unplug = 0;
        }
        app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);
    }
#endif

    return ret;
}

static bool app_device_reboot_handle(void)
{
    bool ret = false;
    APP_PRINT_INFO3("app_device_reboot_handle: %d, %d, %d",
                    app_cfg_nv.power_on_cause_cmd,
                    app_cfg_nv.ota_tooling_start,
                    app_cfg_nv.auto_power_on_after_factory_reset);

    if (app_cfg_const.enable_rtk_charging_box && app_cfg_nv.power_on_cause_cmd)
    {
        app_cfg_nv.power_on_cause_cmd = false;
        app_cfg_store(&app_cfg_nv.offset_smart_chargerbox, 1);

        ret = true;
    }
    else if ((!(app_db.wake_up_reason & WAKE_UP_PAD))
             && (app_cfg_nv.adp_factory_reset_power_on
                 || app_cfg_nv.ota_tooling_start
                 || app_cfg_nv.auto_power_on_after_factory_reset))
    {
        if (app_device_boot_up_directly())
        {
            ret = true;
        }
    }
    else if (app_device_power_on_check())
    {
        ret = true;
    }

    return ret;
}

#if F_APP_COMMON_DONGLE_SUPPORT
bool app_device_has_dongle_record(void)
{
    bool ret = false;
    uint8_t i = 0;
    uint8_t bond_num = 0;
    T_APP_LINK_RECORD *link_record = NULL;

    bond_num = app_bond_b2s_num_get();
    link_record = malloc(sizeof(T_APP_LINK_RECORD) * bond_num);

    if (link_record != NULL)
    {
        bond_num = app_bond_get_b2s_link_record(link_record, bond_num);

        for (i = 0; i < bond_num; i++)
        {
            if (link_record[i].bond_flag & BOND_FLAG_DONGLE)
            {
                ret = true;
                break;
            }
        }

        free(link_record);
    }

    return ret;
}
#endif

static void app_device_bt_and_ble_ready_check(void)
{
    bool power_on_flg = false;

    if (app_db.bt_is_ready && app_db.ble_is_ready)
    {
        APP_PRINT_INFO4("app_device_bt_and_ble_ready_check: %d, %d, %d, %d",
                        app_cfg_nv.is_dut_test_mode,
                        app_cfg_nv.app_is_power_on,
                        app_cfg_nv.factory_reset_done,
                        app_cfg_nv.adaptor_changed);

#if F_APP_DISCHARGER_NTC_DETECT_PROTECT
        if (!app_adc_ntc_voltage_valid_check())
        {
            return;
        }
#endif

#if F_APP_DEVICE_CMD_SUPPORT
        app_report_event(CMD_PATH_UART, EVENT_BT_READY, 0, NULL, 0);
#endif

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
        if (app_cfg_nv.need_set_lps_mode)
        {
            APP_PRINT_INFO1("OTA tooling set shipping mode %d", app_cfg_nv.ota_parking_lps_mode);
            // In shipping mode, DUT should clean flag and ignore power on handle
            app_cfg_nv.need_set_lps_mode = 0;
            app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);

            if (app_cfg_nv.ota_parking_lps_mode)
            {
                // Set power off
                power_mode_set(POWER_SHIP_MODE);

                if (app_cfg_nv.ota_parking_lps_mode == OTA_TOOLING_SHIPPING_5V_WAKEUP_ONLY)
                {
                    System_SetMFBWakeUpFunction(DISABLE);
                }
            }
            else
            {
                power_mode_set(POWER_POWERDOWN_MODE);
            }
            return;
        }
#endif

#if F_APP_PERIODIC_WAKEUP
        if (app_db.wake_up_reason & WAKE_UP_RTC)
        {
            app_cfg_nv.rtc_wakeup_count++;
            DBG_DIRECT("app_device_bt_and_ble_ready_check: Wakeup, rtc_counter %d",
                       app_cfg_nv.rtc_wakeup_count);
            app_cfg_store(&app_cfg_nv.rtc_wakeup_count, 4);
        }
#endif

#if CONFIG_REALTEK_GFPS_FEATURE_SUPPORT
        /*Resolvable private address can only be successfully generate after BLE stack ready,
        app_gfps_adv_init() and app_gfps_finder_init() need to generate RPA, so we call them here*/
        if (extend_app_cfg_const.gfps_support)
        {
            app_gfps_adv_init();
#if CONFIG_REALTEK_GFPS_FINDER_SUPPORT
            if (extend_app_cfg_const.gfps_finder_support)
            {
                app_gfps_finder_init();
                app_dult_device_init();
                app_dult_handle_power_on();

#if F_APP_PERIODIC_WAKEUP
                if (!extend_app_cfg_const.disable_finder_adv_when_power_off
                    && app_gfps_finder_provisioned())
                {
                    uint32_t clock_value_delayed = 0;
                    uint32_t rtc_counter = 0;

                    rtc_counter = RTC_GetCounter();
                    clock_value_delayed = RTC_COUNTER_TO_SECOND(rtc_counter);

                    DBG_DIRECT("app_device_bt_and_ble_ready_check: Wakeup, rtc_counter %d, clock_value_delayed %d, rtc_wakeup_count %d",
                               rtc_counter, clock_value_delayed, app_cfg_nv.rtc_wakeup_count);

                    RTC_RunCmd(DISABLE);
                    power_mode_set(POWER_POWERDOWN_MODE);
                    app_gfps_finder_update_clock_value(clock_value_delayed);
                    if (app_db.wake_up_reason & WAKE_UP_RTC)
                    {
                        power_mode_set(POWER_DLPS_MODE);
                        app_gfps_finder_handle_event_wakeup_by_rtc();
                    }
                }
#endif
            }
#endif
        }
#endif

        if ((app_cfg_const.key_gpio_support) &&
            (app_cfg_const.key_power_on_interval == 0) &&
            (app_db.wake_up_reason & WAKE_UP_MFB))
        {
            app_device_unlock_vbat_disallow_power_on();
        }

        if (app_cfg_nv.disallow_power_on_when_vbat_in)
        {
            // Power on is not allowed for the first time when vbat in after factory reset by mppgtool;
            // Unlock disallow power on mode by MFB power on or slide switch power on or bud starts chargring
#if F_APP_GPIO_ONOFF_SUPPORT
            if (app_cfg_const.box_detect_method == GPIO_DETECT)
            {
                // 3pin location get error in app_cfg_reset if sync_word is not initialized
                if (app_device_is_in_the_box())
                {
                    app_cfg_nv.pre_3pin_status_unplug = 0;
                }
                else
                {
                    app_cfg_nv.pre_3pin_status_unplug = 1;
                }
                app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);
            }
#endif
        }
        else if (app_cfg_nv.is_dut_test_mode)
        {
            app_cfg_nv.is_dut_test_mode = 0;
            app_cfg_store(&app_cfg_nv.offset_notification_vol, 8);

            app_cfg_nv.app_is_power_on = 0;
            app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);
        }
        else if (app_cfg_nv.is_app_reboot)
        {
            if (app_device_reboot_handle())
            {
                power_on_flg = true;
            }

            app_cfg_nv.is_app_reboot = 0;
            app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);
        }
        else
        {
            if (app_device_auto_power_on_before_factory_reset()
                || (app_cfg_const.key_gpio_support && (app_cfg_const.key_power_on_interval == 0))
                || ((!(app_db.wake_up_reason & WAKE_UP_PAD)) &&
                    (app_cfg_nv.app_is_power_on
                     || app_cfg_nv.adp_factory_reset_power_on
                     || app_cfg_nv.ota_tooling_start
                     || app_cfg_nv.auto_power_on_after_factory_reset)))
            {
                if (app_device_boot_up_directly())
                {
                    power_on_flg = true;
                }
            }
            else if (app_device_power_on_check())
            {
                power_on_flg = true;
            }

#if (F_APP_SLIDE_SWITCH_POWER_ON_OFF == 1)
            if (!app_slide_switch_power_on_valid_check())
            {
                power_on_flg = false;
            }
#endif

        }

        if (app_cfg_const.key_gpio_support && (app_cfg_const.key_power_on_interval == 0) &&
            key_gpio_get_press_state(0))
        {
            power_on_flg = false;
        }

        app_ipc_publish(APP_DEVICE_IPC_TOPIC, APP_DEVICE_IPC_EVT_STACK_READY, &power_on_flg);

        if (power_on_flg)
        {
            app_mmi_handle_action(MMI_DEV_POWER_ON);
            app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
        }
    }

#if F_APP_AUTO_POWER_TEST_LOG
    //Don't enable log for power test
#else
    log_module_trace_set(MODULE_HCI, LEVEL_TRACE, true);
    log_module_trace_set(MODULE_BTIF, LEVEL_TRACE, true);
    log_module_trace_set(MODULE_GAP, LEVEL_TRACE, true);
#endif
}

static void app_device_ble_cback(uint8_t subtype, T_LE_GAP_MSG *gap_msg)
{
    switch (subtype)
    {
    case GAP_MSG_LE_DEV_STATE_CHANGE:
        {
            if (!app_db.ble_is_ready)
            {
                if (gap_msg->msg_data.gap_dev_state_change.new_state.gap_init_state == GAP_INIT_STATE_STACK_READY)
                {
                    app_db.ble_is_ready = true;
                    app_device_bt_and_ble_ready_check();
                }
            }
        }
        break;

    default:
        break;
    }
}

static void app_device_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_READY:
        {
            if (!app_db.bt_is_ready)
            {
                app_db.bt_is_ready = true;
                app_device_bt_and_ble_ready_check();
            }
        }
        break;

    case BT_EVENT_ACL_CONN_SUCCESS:
        {
            //we need to get src name to judge whether enter dut mode or not.
            app_db.force_enter_dut_mode_when_acl_connected = false;
            gap_br_get_remote_name(param->acl_conn_success.bd_addr);
        }
        break;

    case BT_EVENT_ACL_AUTHEN_SUCCESS:
        {
            if (app_link_check_b2s_link(param->acl_conn_success.bd_addr))
            {
                if (app_db.enter_gaming_mode_after_power_on)
                {
                    if (!app_db.gaming_mode)
                    {
                        if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                        {
                            app_mmi_handle_action(MMI_DEV_GAMING_MODE_SWITCH);
                        }
                        else
                        {
                            uint8_t action = MMI_DEV_GAMING_MODE_SWITCH;

                            app_relay_async_single(APP_MODULE_TYPE_MMI, action);
                            app_mmi_handle_action(action);
                        }
                    }

                    app_db.enter_gaming_mode_after_power_on = false;
                }
            }
        }
        break;

    case BT_EVENT_ACL_CONN_DISCONN:
        {
            if (app_link_check_b2s_link(param->acl_conn_disconn.bd_addr))
            {
                if (param->acl_conn_disconn.cause != (HCI_ERR | HCI_ERR_CONN_ROLESWAP))
                {
#if F_APP_COMMON_DONGLE_SUPPORT
                    if (app_link_check_dongle_link(param->acl_conn_disconn.bd_addr))
                    {
                        app_audio_update_dongle_flag(false);

#if F_APP_GAMING_DONGLE_SUPPORT
                        app_db.ignore_bau_first_gaming_cmd_handled = false;

#if F_APP_GAMING_CHAT_MIXING_SUPPORT
                        app_db.re_cfg_dongle_link_codec = 0;
#endif

                        if (app_db.gaming_mode)
                        {
                            app_db.restore_gaming_mode = true;
#if F_APP_GAMING_DONGLE_SUPPORT
                            if ((!app_cfg_const.enable_multi_link) || (app_link_get_b2s_link_num() == 0))
#endif
                            {
                                app_mmi_switch_gaming_mode();
#if F_APP_ERWS_SUPPORT
                                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                                {
                                    uint8_t cmd = true;
                                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY,
                                                                        APP_REMOTE_MSG_ASK_TO_EXIT_GAMING_MODE,
                                                                        (uint8_t *)&cmd, sizeof(cmd));
                                }
#endif
                            }
                        }

                        if ((app_db.dongle_is_enable_mic) &&
                            (app_link_check_dongle_link(param->acl_conn_disconn.bd_addr)))
                        {
                            app_dongle_stop_recording(param->acl_conn_disconn.bd_addr);
                            app_dongle_update_is_mic_enable(false);
                        }
#endif //endif for F_APP_GAMING_DONGLE_SUPPORT
                    }
#endif //endif for F_APP_COMMON_DONGLE_SUPPORT
                }

                if (mp_hci_test_mode_is_running())
                {
                    app_auto_power_off_enable(AUTO_POWER_OFF_MASK_DUT_MODE, app_cfg_const.timer_auto_power_off);
                }
            }
        }
        break;

    case BT_EVENT_REMOTE_NAME_RSP:
        {
            if (app_device_is_test_equipment(param->remote_name_rsp.name,
                                             param->remote_name_rsp.bd_addr))
            {
                app_db.force_enter_dut_mode_when_acl_connected = true;

                if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    bt_sniff_mode_disable(param->remote_name_rsp.bd_addr);
                }

                app_mmi_handle_action(MMI_DUT_TEST_MODE);
                app_db.force_enter_dut_mode_when_acl_connected = false;
            }

#if F_APP_TEAMS_BT_POLICY
            temp_master_device_name_len = strlen(param->remote_name_rsp.name);
            memcpy(temp_master_device_name, param->remote_name_rsp.name, temp_master_device_name_len);
#endif
        }
        break;

    case BT_EVENT_HFP_CALL_STATUS:
        {
            switch (param->hfp_call_status.curr_status)
            {
            case BT_HFP_CALL_INCOMING:
#if F_APP_BUZZER_SUPPORT
                buzzer_set_mode(INCOMING_CALL_BUZZER);
#endif
                break;

            case BT_HFP_CALL_ACTIVE:
                {
                }
                break;

            default:
                break;
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_OPEN:
        {
#if F_APP_GAMING_DONGLE_SUPPORT
            app_db.ignore_bau_first_gaming_cmd = APP_IGNORE_DONGLE_SPP_GAMING_CMD_FIRST_CMD;
#endif
        }
        break;

    case BT_EVENT_A2DP_STREAM_START_RSP:
        {

        }
        break;

    case BT_EVENT_A2DP_STREAM_STOP:
        {

        }
        break;

    case BT_EVENT_A2DP_STREAM_CLOSE:
        {

        }
        break;

#if F_APP_ERWS_SUPPORT
    case BT_EVENT_REMOTE_ROLESWAP_STATUS:
        {
            if (param->remote_roleswap_status.status == BT_ROLESWAP_STATUS_SUCCESS)
            {
                app_device_new_pri_apply_app_db_info_when_roleswap_suc();
            }
            else if ((param->remote_roleswap_status.status == BT_ROLESWAP_STATUS_START_RSP) &&
                     (param->remote_roleswap_status.u.start_rsp.accept))
            {
                app_relay_async_single(APP_MODULE_TYPE_DEVICE, APP_REMOTE_MSG_SYNC_DB);
            }
        }
        break;
#endif

    case BT_EVENT_REMOTE_DISCONN_CMPL:
        {
#if F_APP_COMMON_DONGLE_SUPPORT
            if (app_db.remote_is_dongle && (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY))
            {
                app_audio_update_dongle_flag(false);

#if F_APP_GAMING_DONGLE_SUPPORT
                if (app_db.gaming_mode)
                {
                    app_mmi_switch_gaming_mode();
                }
#endif
            }
#endif
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_device_bt_cback: event_type 0x%04x", event_type);
    }
}

bool app_device_is_in_the_box(void)
{
    bool ret = false;

    if (app_cfg_const.box_detect_method == ADAPTOR_DETECT)
    {
        if (app_adp_get_plug_state() == ADAPTOR_PLUG)
        {
            ret = true;
        }
    }
#if F_APP_GPIO_ONOFF_SUPPORT
    else if (app_cfg_const.box_detect_method == GPIO_DETECT)
    {
        if ((app_cfg_const.enable_rtk_charging_box && (app_adp_get_plug_state() == ADAPTOR_PLUG)) ||
            (!app_cfg_const.enable_rtk_charging_box && (app_gpio_on_off_get_location() == IN_CASE)))
        {
            ret = true;
        }
    }
#endif

    APP_PRINT_INFO1("app_device_is_in_the_box: ret = %d", ret);
    return ret;
}

bool app_device_is_in_ear(uint8_t loc)
{
    bool in_ear = false;
    if (LIGHT_SENSOR_ENABLED)
    {
        in_ear = (loc == BUD_LOC_IN_EAR) ? true : false;
    }
    else
    {
        in_ear = (loc != BUD_LOC_IN_CASE) ? true : false;
    }
    return in_ear;
}

uint8_t app_device_get_bud_channel(void)
{
    uint8_t bud_channel;

    // the single bud role key map data was stored at channel left
    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        bud_channel = CHANNEL_L_L;
    }
    else
    {
        if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT)
        {
            bud_channel = CHANNEL_L_L;
        }
        else
        {
            bud_channel = CHANNEL_R_R;
        }
    }

    return bud_channel;
}

void app_device_bt_policy_startup(bool at_once_trigger)
{
    app_bt_policy_startup(app_device_link_policy_ind, at_once_trigger);
}

void app_device_enter_dut_mode(void)
{
    app_bt_policy_disconnect_all_link();
    app_start_timer(&timer_idx_dut_mode_wait_link_disc, "dut_mode_wait_link_disc",
                    device_timer_id, APP_TIMER_DUT_MODE_WAIT_LINK_DISC, 0, true,
                    2000);
}

void app_device_unlock_vbat_disallow_power_on(void)
{
    if (app_cfg_nv.disallow_power_on_when_vbat_in)
    {
        app_cfg_nv.disallow_power_on_when_vbat_in = 0;
        app_cfg_store(&app_cfg_nv.offset_listening_mode_cycle, 1);
    }
}

#if F_APP_ERWS_SUPPORT

static void app_device_pri_collect_app_db_info_for_roleswap(T_ROLESWAP_APP_DB *roleswap_app_db)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        roleswap_app_db->remote_is_dongle = app_db.remote_is_dongle;
    }
}

static void app_device_sec_recv_app_db_info_for_roleswap(void *buf, uint16_t len)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        if (sizeof(T_ROLESWAP_APP_DB) == len)
        {
            memcpy((void *)&roleswap_app_db_temp, buf, len);
        }
    }
}

static void app_device_new_pri_apply_app_db_info_when_roleswap_suc(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        app_db.remote_is_dongle = roleswap_app_db_temp.remote_is_dongle;
    }
}
static void app_device_le_name_sync(uint8_t *buf, uint8_t len)
{
    memset(&app_cfg_nv.device_name_le[0], 0, sizeof(app_cfg_nv.device_name_le));
    memcpy_s(&app_cfg_nv.device_name_le[0], sizeof(app_cfg_nv.device_name_le), (uint8_t *)buf, len);
    le_set_gap_param(GAP_PARAM_DEVICE_NAME, len, (uint8_t *)buf);
    app_cfg_store(app_cfg_nv.device_name_le, 40);
    app_ble_common_adv_update_scan_rsp_data();
}

static void app_device_legacy_name_sync(uint8_t *buf, uint8_t len)
{
    memset(&app_cfg_nv.device_name_legacy[0], 0, sizeof(app_cfg_nv.device_name_legacy));
    memcpy_s(&app_cfg_nv.device_name_legacy[0], sizeof(app_cfg_nv.device_name_legacy), (uint8_t *)buf,
             len);
    bt_local_name_set((uint8_t *)buf, len);
    app_cfg_store(app_cfg_nv.device_name_legacy, 40);
}

#if F_APP_ERWS_SUPPORT
uint16_t app_device_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;

    T_ROLESWAP_APP_DB roleswap_app_db;

    switch (msg_type)
    {
    case APP_REMOTE_MSG_SYNC_DB:
        {
            app_device_pri_collect_app_db_info_for_roleswap(&roleswap_app_db);

            payload_len = sizeof(roleswap_app_db);
            msg_ptr = (uint8_t *)&roleswap_app_db;
        }
        break;

    default:
        break;
    }

    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_DEVICE, payload_len, msg_ptr, skip, total);
}
#endif

#if F_APP_ERWS_SUPPORT
static void app_device_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                   T_REMOTE_RELAY_STATUS status)
{
    switch (msg_type)
    {
    case APP_REMOTE_MSG_SYNC_DB:
        {
            app_device_sec_recv_app_db_info_for_roleswap(buf, len);
        }
        break;

    case APP_REMOTE_MSG_LINK_RECORD_ADD:
    case APP_REMOTE_MSG_LINK_RECORD_DEL:
    case APP_REMOTE_MSG_LINK_RECORD_XMIT:
    case APP_REMOTE_MSG_LINK_RECORD_PRIORITY_SET:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            app_bond_handle_remote_link_record_msg(msg_type, buf);
        }
        break;

    case APP_REMOTE_MSG_DEVICE_NAME_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_device_legacy_name_sync((uint8_t *)buf, len);

                if (app_cfg_const.le_name_sync_to_legacy_name)
                {
                    app_device_le_name_sync((uint8_t *)buf, len);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_LE_NAME_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_device_le_name_sync((uint8_t *)buf, len);

                if (app_cfg_const.le_name_sync_to_legacy_name)
                {
                    app_device_legacy_name_sync((uint8_t *)buf, len);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_POWERING_OFF:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_db.sec_is_power_off = true;
            }
        }
        break;

    case APP_REMOTE_MSG_REMOTE_SPK2_PLAY_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT)
            {
                if (app_cfg_const.rws_connected_show_channel_vp &&
                    (app_cfg_const.couple_speaker_channel == CHANNEL_R_R))
                {
                    app_audio_tone_type_play(TONE_REMOTE_ROLE_SECONDARY, false, false);
                }
                else
                {
                    app_audio_tone_type_play(TONE_REMOTE_ROLE_PRIMARY, false, false);
                }
            }
            else if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_const.rws_connected_show_channel_vp &&
                    (app_cfg_const.couple_speaker_channel == CHANNEL_L_L))
                {
                    app_audio_tone_type_play(TONE_REMOTE_ROLE_PRIMARY, false, false);
                }
                else
                {
                    app_audio_tone_type_play(TONE_REMOTE_ROLE_SECONDARY, false, false);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SPK1_REPLY_SRC_IS_IOS:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    T_APP_BR_LINK *p_link;
                    uint8_t bd_addr[6];

                    memcpy(bd_addr, &p_info[1], 6);

                    p_link = app_link_find_br_link(bd_addr);

                    if (p_link != NULL)
                    {
                        p_link->remote_device_vendor_id = (T_APP_REMOTE_DEVICE_VENDOR_ID)p_info[0];
                        APP_PRINT_TRACE2("SPK2 update ios flag: %s (%d)", TRACE_BDADDR(bd_addr),
                                         p_link->remote_device_vendor_id);
                    }
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SET_LPS_SYNC:
        {
            uint8_t dlps_stay_mode = *(uint8_t *)buf;

            APP_PRINT_INFO1("CMD_OTA_TOOLING_PARKING %d", dlps_stay_mode);

            app_cfg_nv.need_set_lps_mode = 1;

            if (dlps_stay_mode)
            {
                // shipping mode
                power_mode_set(POWER_SHIP_MODE);
                app_cfg_nv.ota_parking_lps_mode = dlps_stay_mode;
            }
            else
            {
                // power down mode
                power_mode_set(POWER_POWERDOWN_MODE);
                app_cfg_nv.ota_parking_lps_mode = dlps_stay_mode;
            }

            app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);
        }
        break;

    case APP_REMOTE_MSG_SYNC_IO_PIN_PULL_HIGH:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t *data = (uint8_t *)buf;
                uint8_t pin_num = data[0];

                Pad_Config(pin_num, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
            }
        }
        break;

#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
    case APP_REMOTE_MSG_SPATIAL_AUDIO:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t *data = (uint8_t *)buf;
                uint8_t mode = data[0];

                app_sensor_mems_mode_sync(mode);
            }
        }
        break;
#endif

    default:
        break;
    }
}
#endif
#endif

void app_device_init(void)
{
    app_db.local_batt_level = 50;
    app_db.remote_batt_level = 50;

    ble_mgr_msg_cback_register(app_device_ble_cback);
    sys_mgr_cback_register(app_device_dm_cback);
    bt_mgr_cback_register(app_device_bt_cback);
    app_timer_reg_cb(app_device_timerout_cb, &device_timer_id);
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_device_relay_cback, app_device_parse_cback,
                             APP_MODULE_TYPE_DEVICE, APP_REMOTE_MSG_DEVICE_TOTAL);
#endif
}
