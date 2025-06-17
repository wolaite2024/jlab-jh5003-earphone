/**
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_VAD_SUPPORT
#include "trace.h"
#include "audio.h"
#include "btm.h"
#include "vad.h"
#include "kws.h"
#include "app_cfg.h"
#include "app_mmi.h"
#include "app_vad.h"
#include "remote.h"
#if AMA_FEATURE_SUPPORT
#include "app_ama.h"
#endif

typedef struct t_app_vad_type
{
    T_VAD_SM_STATE vad_state;
    uint8_t        vad_type;
} T_APP_VAD_TYPE;

static T_APP_VAD_TYPE vad;
static T_AUDIO_EFFECT_INSTANCE kws;

static void app_vad_audio_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;

    switch (event_type)
    {
    case AUDIO_EVENT_VAD_ENABLED:
        {
            vad.vad_state = VAD_STARTED;
        }
        break;

    case AUDIO_EVENT_VAD_DATA_IND:
        {
#if APP_AMA_VAD_SUPPORT
            if (app_cfg_const.enable_vad == 1)
            {
                app_vad_disable();
                app_ama_va_start();
            }
#else
#endif
        }
        break;

    case AUDIO_EVENT_VAD_DISABLED:
        {
            vad.vad_state = VAD_STOPPED;
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
        APP_PRINT_TRACE1("app_vad_audio_cback: event 0x%04x", event_type);
    }
}

static void app_vad_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_HFP_CONN_CMPL:
        {
            if (app_cfg_const.enable_vad == 1)
            {
                app_vad_enable(VAD_TYPE_SW, 0, 0);
            }
        }
        break;

    case BT_EVENT_HFP_DISCONN_CMPL:
        {
            if (app_cfg_const.enable_vad == 1)
            {
                app_vad_disable();
            }
        }
        break;

    case BT_EVENT_REMOTE_ROLESWAP_STATUS:
        {
            if (app_cfg_const.enable_vad == 1)
            {
                T_BT_EVENT_PARAM *param = event_buf;
                T_BT_ROLESWAP_STATUS event;

                event = param->remote_roleswap_status.status;

                if (event == BT_ROLESWAP_STATUS_SUCCESS)
                {
                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        app_vad_enable(VAD_TYPE_SW, 0, 0);
                    }
                    else
                    {
                        app_vad_disable();
                    }
                }
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_vad_bt_cback: event_type 0x%04x", event_type);
    }
}

bool app_vad_enable(uint8_t  vad_type,
                    uint8_t  bit_width,
                    uint16_t frame_length)
{
    vad.vad_type = vad_type;
    if (vad.vad_state != VAD_STARTED)
    {
        T_AUDIO_FORMAT_INFO        format_info;
        T_VAD_AGGRESSIVENESS_LEVEL aggressiveness_level;

        vad.vad_state = VAD_STARTING;
        aggressiveness_level = VAD_AGGRESSIVENESS_LEVEL_LOW;

        if (vad_type == VAD_TYPE_SW)
        {
            format_info.type = AUDIO_FORMAT_TYPE_SBC;
            format_info.frame_num = 1;
            format_info.attr.sbc.sample_rate = 16000;
            format_info.attr.sbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_JOINT_STEREO;
            format_info.attr.sbc.chann_location = AUDIO_CHANNEL_LOCATION_FL | AUDIO_CHANNEL_LOCATION_FR;
            format_info.attr.sbc.block_length = 16;
            format_info.attr.sbc.subband_num = 8;
            format_info.attr.sbc.allocation_method = 0;
            format_info.attr.sbc.bitpool = 0x22;
        }
        else
        {
            format_info.type = AUDIO_FORMAT_TYPE_PCM;
            format_info.attr.pcm.bit_width = bit_width;
            format_info.attr.pcm.chann_num = 1;
            format_info.attr.pcm.frame_length = frame_length;
            format_info.attr.pcm.sample_rate = 16000;
            format_info.attr.pcm.chann_location = AUDIO_CHANNEL_LOCATION_MONO;
        }

        vad_enable((T_VAD_TYPE)vad_type, aggressiveness_level, format_info);

        if (vad_type != VAD_TYPE_HW)
        {
            vad_filter(VAD_FILTER_LEVEL_DEFAULT);
            kws = kws_effect_create(KWS_EFFECT_CONTENT_TYPE_VAD);
            vad_effect_attach(kws);
            kws_effect_enable(kws);
        }

        return true;
    }

    return false;
}

bool app_vad_disable(void)
{
    if (vad.vad_state != VAD_STOPPED)
    {
        vad.vad_state = VAD_STOPPING;
        if (vad.vad_type != VAD_TYPE_FUSION)
        {
            kws_effect_disable(kws);
            vad_effect_detach(kws);
            kws_effect_release(kws);
        }
        vad_disable();

        return true;
    }

    return false;
}

void app_vad_init(void)
{
    vad.vad_state = VAD_STOPPED;
    audio_mgr_cback_register(app_vad_audio_cback);
    bt_mgr_cback_register(app_vad_bt_cback);
}
#endif
