/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if AMA_FEATURE_SUPPORT


#include "app_link_util.h"
#include "app_bond.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_a2dp.h"
#include "app_mmi.h"
#include "bt_bond.h"
#include "os_mem.h"
#include "trace.h"


#include "ama_service_api.h"
#include "app_ama.h"
#include "app_ama_record.h"
#include "app_ama_timer.h"
#include "app_ama_transport.h"


#include "state.pb.h"
#include "media.pb.h"


#include "app_ama_ble.h"


#include "app_ama_roleswap.h"
#include "app_ama_device.h"
#include "app_ama_accessory.h"
#include "app_ama_tone.h"
#include "app_relay.h"


#if F_APP_IAP_SUPPORT == 0
#error AMA needs iAP support by F_APP_IAP_SUPPORT
#endif


#if APP_AMA_VAD_SUPPORT == 1
#if F_APP_VAD_SUPPORT == 0
#error AMA VAD needs rtk vad support by F_APP_VAD_SUPPORT
#endif
#endif

void app_ama_bond_add(uint8_t *bd_addr)
{
    bt_bond_flag_add(bd_addr, BOND_FLAG_AMA);
}

void app_ama_bond_remove(uint8_t *bd_addr)
{
    bt_bond_flag_remove(bd_addr, BOND_FLAG_AMA);
}


bool app_ama_bond_exist(uint8_t *bd_addr)
{
    uint32_t bond_flag;

    if (bt_bond_flag_get(bd_addr, &bond_flag) == true)
    {
        if ((bond_flag & BOND_FLAG_AMA) == BOND_FLAG_AMA)
        {
            return true;
        }
    }

    return false;
}


static void app_ama_handle_synchronize_state(uint8_t *bd_addr, State synchronize_state)
{
    APP_PRINT_TRACE2("app_ama_handle_synchronize_state: feature 0x%x, state %d",
                     synchronize_state.feature, (synchronize_state.which_value - State_boolean_tag) ?
                     synchronize_state.value.integer : synchronize_state.value.boolean);

    switch (synchronize_state.feature)
    {
    case AUXILIARY_CONNECTED:
        {

        }
        break;

    case BLUETOOTH_A2DP_ENABLED:
        {

        }
        break;

    case BLUETOOTH_HFP_ENABLED:
        {

        }
        break;

    case BLUETOOTH_A2DP_CONNECTED:
        {

        }
        break;

    case BLUETOOTH_HFP_CONNECTED:
        {

        }
        break;

    case BLUETOOTH_CLASSIC_DISCOVERABLE:
        {

        }
        break;

    case BLUETOOTH_A2DP_ACTIVE:
        {

        }
        break;

    case BLUETOOTH_HFP_ACTIVE:
        {

        }
        break;

    case SCO_PRIORITIZATION_ENABLED:
        {

        }
        break;

    case DEVICE_CALIBRATION_REQUIRED:
        {

        }
        break;

    case DEVICE_THEME:
        {

        }
        break;

    case DEVICE_DND_ENABLED:
        {

        }
        break;

    case DEVICE_NETWORK_CONNECTIVITY_STATUS:
        {
            app_ama_device_set_network_conn_status(bd_addr, (uint8_t)synchronize_state.value.integer);

            if (synchronize_state.value.integer == UNAVAILABLE || synchronize_state.value.integer == TOO_SLOW)
            {
                APP_PRINT_ERROR1("app_ama_handle_synchronize_state: DEVICE_NETWORK_CONNECTIVITY_STATUS synchronize_state %d",
                                 synchronize_state.value.integer);
            }
        }
        break;

    case PRIVACY_MODE_ENABLED:
        {

        }
        break;

    case ACTIVE_NOISE_CANCELLATION_LEVEL:
        {

        }
        break;

    case PASSTHROUGH_LEVEL:
        {

        }
        break;

    case SETUP_MODE_ENABLED:
        {

        }
        break;

    case EXTERNAL_MICROPHONE_ENABLED:
        {

        }
        break;

    case FEEDBACK_ANC:
        {

        }
        break;

    case MESSAGE_NOTIFICATION:
        {

        }
        break;

    case CALL_NOTIFICATION:
        {

        }
        break;

    case REMOTE_NOTIFICATION:
        {

        }
        break;

    case START_OF_SPEECH_EARCON_ENABLED:
        {

        }
        break;

    case END_OF_SPEECH_EARCON_ENABLED:
        {

        }
        break;

    case WAKE_WORD_DETECTION_ENABLED:
        {

        }
        break;

    case ALEXA_FOLLOW_UP_MODE_ENABLED:
        {

        }
        break;

    case MULTI_TURN_DELAY_ENABLED:
        {

        }
        break;

    case AVRCP_OVERRIDE:
        {

        }
        break;

    default:
        {

        }
        break;
    }
}

static void app_ama_cback(uint8_t *bd_addr, T_AMA_CMD event_type, void *event_buf, uint16_t buf_len)
{
    T_AMA_CMD_PARAM_INFO *param = (T_AMA_CMD_PARAM_INFO *)event_buf;

    APP_PRINT_INFO1("app_ama_cback: event_type %d", event_type);

    switch (event_type)
    {
    case AMA_EVT_GET_CENTRAL_INFORMATION:
        {
            app_ama_device_set_os_platform(bd_addr, param->value.os_platform);
        }
        break;

    case AMA_EVT_START_SPEECH_RSP:
        {
            if (param->value.start_speech_rsp.error_code == ErrorCode_SUCCESS)
            {
                app_ama_tone_play(TONE_AMA_START_SPEECH);
                app_ama_record_start_recording(bd_addr);
                app_ama_device_set_va_state(bd_addr, ALEXA_VA_STATE_ACTIVE);
            }
            else
            {
                app_ama_timer_stop_wait_speech_endpoint_timer();
                app_ama_record_stop_recording(bd_addr);
            }
        }
        break;

    case AMA_EVT_STOP_SPEECH:
        {
            app_ama_tone_play(TONE_AMA_STOP_SPEECH);
            app_ama_timer_stop_wait_speech_endpoint_timer();
            app_ama_record_stop_recording(bd_addr);
        }
        break;

    case AMA_EVT_STOP_SPEECH_RSP:
        {
            app_ama_record_stop_recording(bd_addr);
            app_ama_device_set_va_state(bd_addr, ALEXA_VA_STATE_IDLE);
        }
        break;

    case AMA_EVT_ENDPOINT_SPEECH:
        {
            app_ama_timer_stop_wait_speech_endpoint_timer();
            app_ama_record_stop_recording(bd_addr);

#if APP_AMA_VAD_SUPPORT
            if (app_cfg_const.enable_vad == 1)
            {
                app_ama.need_deactivate = true;
            }
#endif
        }
        break;

    case AMA_EVT_PROVIDE_SPEECH:
        {
            app_ama_record_start_recording(bd_addr);
            app_ama_timer_start_wait_speech_endpoint_timer();
        }
        break;

    case AMA_EVT_NOTIFY_SPEECH_STATE:
        {
            APP_PRINT_INFO1("app_ama_cback: AMA_EVT_NOTIFY_SPEECH_STATE speech_state %d",
                            param->value.speech_state);

#if APP_AMA_VAD_SUPPORT
            if (app_cfg_const.enable_vad == 1)
            {
                if ((app_ama.need_deactivate == true) && (param->value.speech_state == SpeechState_IDLE))
                {
                    app_ama.need_deactivate = false;
                    app_ama_va_start();
                }
            }
#endif
        }
        break;

    case AMA_EVT_GET_DEVICE_INFORMATION:
        {
        }
        break;

    case AMA_EVT_GET_DEVICE_CONFIGURATION:
        {
        }
        break;

    case AMA_EVT_OVERRIDE_ASSISTANT:
        {
        }
        break;

    case AMA_EVT_START_SETUP:
        {

        }
        break;

    case AMA_EVT_COMPLETE_SETUP:
        {

            if (!app_cfg_const.enable_multi_link)
            {
                app_ama_ble_adv_stop();
            }
            else
            {

            }

            app_ama_bond_add(bd_addr);

        }
        break;

    case AMA_EVT_NOTIFY_DEVICE_CONFIGURATION:
        {
        }
        break;

    case AMA_EVT_UPGRADE_TRANSPORT:
        {
        }
        break;

    case AMA_EVT_SWITCH_TRANSPORT:
        {
        }
        break;

    case AMA_EVT_FORWARD_AT_COMMAND:
        {
        }
        break;

    case AMA_EVT_INCOMING_CALL:
        {
        }
        break;

    case AMA_EVT_SYNCHRONIZE_SETTINGS:
        {
            APP_PRINT_INFO2("app_ama_cback: AMA_EVT_SYNCHRONIZE_SETTINGS timestamp_hi 0x%08x, timestamp_lo 0x%08x",
                            param->value.synchronize_settings.timestamp_hi, param->value.synchronize_settings.timestamp_lo);
            app_ama_timer_start_get_platform_timer();
        }
        break;

    case AMA_EVT_RESET_CONNECTION:
        {
        }
        break;

    case AMA_EVT_KEEP_ALIVE:
        {
        }
        break;

    case AMA_EVT_ISSUE_MEDIA_CONTROL:
        {
            switch (*(MediaControl *)event_buf)
            {
            case MediaControl_PLAY:
                bt_avrcp_play(bd_addr);
                break;

            case MediaControl_PAUSE:
                bt_avrcp_pause(bd_addr);
                break;

            case MediaControl_NEXT:
                bt_avrcp_forward(bd_addr);
                break;

            case MediaControl_PREVIOUS:
                bt_avrcp_backward(bd_addr);
                break;

            case MediaControl_PLAY_PAUSE:
                {
                    T_APP_BR_LINK *p_link = NULL;
                    p_link = app_link_find_br_link(bd_addr);

                    if (p_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
                    {
                        bt_avrcp_pause(bd_addr);
                    }
                    else if (p_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PAUSED)
                    {
                        bt_avrcp_play(bd_addr);
                    }
                    else
                    {
                        APP_PRINT_ERROR1("app_ama_cback: ERROR avrcp_play_status %d", p_link->avrcp_play_status);
                    }
                }
                break;

            default:
                break;
            }
        }
        break;

    case AMA_EVT_GET_STATE:
        {
        }
        break;

    case AMA_EVT_SET_STATE:
        {
        }
        break;

    case AMA_EVT_SYNCHRONIZE_STATE:
        {
            app_ama_handle_synchronize_state(bd_addr, param->value.synchronize_state);
        }
        break;
    default:
        break;
    }

}


void app_ama_va_start(void)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY && extend_app_cfg_const.ama_support)
    {
        uint8_t app_idx = app_a2dp_get_active_idx();
        app_ama_device_va_start(app_db.br_link[app_idx].bd_addr);
    }
}


void app_ama_reset(uint8_t bd_addr[6])
{
    app_ama_timer_stop_wait_speech_endpoint_timer();
    app_ama_record_stop_recording(bd_addr);
}


static bool app_ama_b2b_sync_send_for_ama_service(AMA_B2B_EVT evt, uint8_t *buf, uint32_t len)
{
    APP_PRINT_TRACE3("app_ama_b2b_sync_send_for_ama_service: evt %d, len %d, data %b", evt, len,
                     TRACE_BINARY(len, buf));

    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AMA_SERVICE, evt, buf, len);

    return true;
}

#if F_APP_ERWS_SUPPORT
static bool app_ama_parse(AMA_B2B_EVT evt, uint8_t *buf, uint16_t len, T_REMOTE_RELAY_STATUS status)
{
    if (status != REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        return false;
    }

    APP_PRINT_TRACE3("app_ama_parse: evt %d, len %d, data %b", evt, len, TRACE_BINARY(len, buf));

    ama_stream_proc_b2b_sync_recv(evt, buf, len);

    return true;
}
#endif

void app_ama_init(void)
{
    /* Initial AMA service */
    if (ama_service_process_init(app_ama_device_tx, b2b_sync_send_for_ama_service) == true)
    {
        app_ama_role_update((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
#if F_APP_ERWS_SUPPORT
        app_relay_cback_register(NULL, (P_APP_PARSE_CBACK)app_ama_parse, APP_MODULE_TYPE_AMA_SERVICE,
                                 AMA_B2B_EVT_MAX);
#endif
    }
    else
    {
        return;
    }

    app_ama_accessory_init();

    ama_cback_register(app_ama_cback, app_ama_transport_get_bt_info, app_ama_device_get_mtu);

    app_ama_device_init();

    app_ama_timer_init();

    app_ama_roleswap_init();

    app_ama_tone_init();
}

#endif
