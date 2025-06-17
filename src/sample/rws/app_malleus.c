/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_MALLEUS_SUPPORT
#include "trace.h"
#include "ftl.h"
#include "malleus.h"
#include "app_malleus.h"
#include "app_cfg.h"
#include "app_audio_policy.h"
#include "app_bt_policy_api.h"
#include "app_report.h"
#include "app_cmd.h"
#include "app_a2dp.h"
#include "app_main.h"

static const uint8_t malleus_ack_map[] = {CMD_SET_STATUS_COMPLETE, CMD_SET_STATUS_DISALLOW, CMD_SET_STATUS_UNKNOW_CMD, CMD_SET_STATUS_PARAMETER_ERROR};

#if F_APP_SUPPORT_SPATIAL_AUDIO
static void app_malleus_spatial_status_set(uint8_t status)
{
    app_cfg_nv.malleus_spatial_status = status;
}
#endif

static void app_malleus_play_effect_tone(uint8_t idx)
{
    T_APP_AUDIO_TONE_TYPE effect_tone_type = TONE_AUDIO_EQ_0;

    effect_tone_type = (T_APP_AUDIO_TONE_TYPE)(TONE_AUDIO_EQ_0 + (idx + 1));
    app_audio_tone_type_play(effect_tone_type, false, false);
}

static void app_malleus_report_event(uint8_t cmd_path, uint8_t app_index, uint8_t *data,
                                     uint16_t len)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }

    app_report_event(cmd_path, EVENT_MALLEUS_CUSTOMIZED_REPORT, app_index, data, len);
}

static void app_malleus_relay_async(uint8_t msg, uint8_t *data, uint16_t len)
{
    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_MALLEUS_EFFECT, msg, data, len);
}

static void app_malleus_relay_sync(uint8_t msg, uint8_t *data, uint16_t len)
{
    app_relay_sync_single(APP_MODULE_TYPE_MALLEUS_EFFECT, msg, REMOTE_TIMER_HIGH_PRECISION, 150, false);
}

static void app_malleus_param_init(void)
{
    malleus.normal_type = &app_cfg_nv.malleus_effect_normal_type;
    malleus.gaming_type = &app_cfg_nv.malleus_effect_gaming_type;
    malleus.gaming_mode = (uint8_t *)&app_db.gaming_mode;
    malleus.role = &app_cfg_nv.bud_role;

    malleus.factory_addr = app_db.factory_addr;
    malleus.key_flash_offset = APP_RW_MALLEUS_KEY_VAL_ADDR;
    malleus.key_flash_len = APP_RW_MALLEUS_KEY_VAL_SIZE;

    malleus.report_event = app_malleus_report_event;
    malleus.b2b_conn = app_bt_policy_b2b_is_conn;
    malleus.play_tone = app_malleus_play_effect_tone;

#if F_APP_ERWS_SUPPORT
    malleus.relay_async = app_malleus_relay_async;
#endif

#if F_APP_SUPPORT_SPATIAL_AUDIO
    malleus.spatial_status_set = app_malleus_spatial_status_set;
    malleus.spatial_pos = app_cfg_const.spatial_pos;
    malleus.spatial_status = app_cfg_nv.malleus_spatial_status;
#endif

    app_cfg_const.rws_disable_codec_mute_when_linkback = 1;
    app_cfg_const.enable_vp_mixing = 0;
}

#if F_APP_ERWS_SUPPORT
static void app_malleus_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                    T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE2("malleus_parse_cback: event 0x%04x, status %u", msg_type, status);

    malleus_relay_parse(msg_type, buf, len, status);
}

static uint16_t app_malleus_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0, len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;

    malleus_relay_load(msg_type, &msg_ptr, &payload_len);

    if (msg_ptr != NULL)
    {
        len = app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_MALLEUS_EFFECT, payload_len, msg_ptr,
                                 skip, total);

        free(msg_ptr);
    }

    return len;
}
#endif

void app_malleus_cfg_rst(void)
{
    malleus_cfg_rst(app_cfg_const.malleus_normal, app_cfg_const.malleus_gaming);
}

void app_malleus_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                            uint8_t *ack_pkt)
{
    uint8_t ack_idx = malleus_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx);

    ack_pkt[2] = malleus_ack_map[ack_idx];
    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
}

void app_malleus_init(uint8_t normal_cycle, uint8_t gaming_cycle)
{
    app_malleus_param_init();
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_malleus_relay_cback, app_malleus_parse_cback,
                             APP_MODULE_TYPE_MALLEUS_EFFECT, MALLEUS_SYNC_MSG_MAX);
#endif
    malleus_init(normal_cycle, gaming_cycle);
}
#endif
