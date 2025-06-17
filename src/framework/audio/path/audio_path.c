/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "os_msg.h"
#include "os_queue.h"
#include "trace.h"
#include "sys_event.h"
#include "audio_codec.h"
#include "audio_mgr.h"
#include "audio_path.h"
#include "dsp_mgr.h"
#include "codec_mgr.h"
#include "anc_mgr.h"
#include "vad_mgr.h"
#include "gateway.h"

/* TODO Remove Start */
#include "dsp_ipc.h"
#include "bin_loader.h"
#include "dsp_driver.h"
#include "audio_route.h"
#include "app_msg.h"
#include "sys_mgr.h"

void *hAudioPathMsgQueueHandle = NULL;
extern void *hEventQueueHandleAu;
/* TODO Remove End */

#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr)-(size_t)(&((type *)0)->member)))

#define LIST_ENTRY(ptr, type, member) \
    CONTAINER_OF(ptr, type, member)

#define LIST_FIRST_ENTRY(p_queue, type, member) \
    ((p_queue)->p_first ? \
     LIST_ENTRY((p_queue)->p_first, type, member) : NULL)

#define LIST_NEXT_ENTRY(pos, member) \
    (pos && (pos)->member.p_next ? \
     LIST_ENTRY((pos)->member.p_next, typeof(*(pos)), member) : NULL)

#define LIST_FOR_EACH_ENTRY(pos, p_queue, member) \
    for (pos = LIST_FIRST_ENTRY(p_queue, typeof(*(pos)), member); \
         pos != NULL; \
         pos = LIST_NEXT_ENTRY(pos, member))

#define LIST_FOR_EACH_ENTRY_SAFE(pos, n, p_queue, member) \
    for (pos = LIST_FIRST_ENTRY(p_queue, typeof(*(pos)), member), \
         n = LIST_NEXT_ENTRY(pos, member); \
         pos != NULL; \
         pos = n, n = LIST_NEXT_ENTRY(n, member))

#define AUDIO_PATH_EVENT_FLAG_PATH_STARTING                 (0x00000001)
#define AUDIO_PATH_EVENT_FLAG_ANC_LLAPT_ENABLING            (0x00000002)
#define AUDIO_PATH_EVENT_FLAG_ANC_LLAPT_DISABLING           (0x00000004)

#define AUDIO_PATH_DOMAIN_CODEC         (0x00000001)
#define AUDIO_PATH_DOMAIN_DSP           (0x00000002)
#define AUDIO_PATH_DOMAIN_GATEWAY       (0x00000004)
#define AUDIO_PATH_DOMAIN_PLUGIN        (0x00000008)
#define AUDIO_PATH_DOMAIN_VAD           (0x00000010)

#define  AUDIO_BIT      (1U << AUDIO_CATEGORY_AUDIO)
#define  VOICE_BIT      (1U << AUDIO_CATEGORY_VOICE)
#define  RECORD_BIT     (1U << AUDIO_CATEGORY_RECORD)
#define  LINE_BIT       (1U << AUDIO_CATEGORY_LINE)
#define  TONE_BIT       (1U << AUDIO_CATEGORY_TONE)
#define  VP_BIT         (1U << AUDIO_CATEGORY_VP)
#define  APT_BIT        (1U << AUDIO_CATEGORY_APT)
#define  LLAPT_BIT      (1U << AUDIO_CATEGORY_LLAPT)
#define  ANC_BIT        (1U << AUDIO_CATEGORY_ANC)
#define  VAD_BIT        (1U << AUDIO_CATEGORY_VAD)
#define  KWS_BIT        (1U << AUDIO_CATEGORY_KWS)
#define  PIPE_BIT       (1U << AUDIO_CATEGORY_PIPE)

typedef enum t_audio_path_state
{
    AUDIO_PATH_STATE_IDLE,
    AUDIO_PATH_STATE_PENDING,
    AUDIO_PATH_STATE_READY,
    AUDIO_PATH_STATE_RUNNING,
    AUDIO_PATH_STATE_SUSPENDING,
    AUDIO_PATH_STATE_STOPPING,
} T_AUDIO_PATH_STATE;

typedef struct t_vad_path_param
{
    T_VAD_TYPE type;
} T_VAD_PATH_PARAM;

typedef union t_path_param
{
    T_VAD_PATH_PARAM vad;
} T_PAHT_PARAM;

typedef struct t_audio_path
{
    T_OS_QUEUE_ELEM             state_list;
    T_AUDIO_CATEGORY            category;
    T_AUDIO_STREAM_MODE         data_mode;
    T_AUDIO_PATH_STATE          state;
    P_AUDIO_PATH_CBACK          cback;
    T_CODEC_MGR_SESSION_HANDLE  codec_session;
    T_DSP_MGR_SESSION_HANDLE    dsp_session;
    T_VAD_MGR_SESSION_HANDLE    vad_session;
    T_GATEWAY_SESSION_HANDLE    gateway_session;
    T_SYS_EVENT_GROUP_HANDLE    event_group;
    uint32_t                    device;
    int16_t                     dac_left_gain;
    int16_t                     dac_right_gain;
    T_PAHT_PARAM                param;
} T_AUDIO_PATH;

typedef struct t_audio_path_db
{
    T_OS_QUEUE                  path_list;
    T_SYS_EVENT_GROUP_HANDLE    event_group;
    P_AUDIO_PATH_MGR_CBACK      cback;
} T_AUDIO_PATH_DB;

static T_AUDIO_PATH_DB audio_path_db;

static const uint16_t mix_strategy_map[AUDIO_CATEGORY_NUM] =
{
    /* AUDIO */
    [AUDIO_CATEGORY_AUDIO] =
    (AUDIO_BIT | VOICE_BIT | RECORD_BIT | LINE_BIT | TONE_BIT | VP_BIT | APT_BIT | LLAPT_BIT | ANC_BIT | VAD_BIT | PIPE_BIT),
    /* VOICE */
    [AUDIO_CATEGORY_VOICE] =
    (AUDIO_BIT | VOICE_BIT | LINE_BIT | TONE_BIT | VP_BIT | APT_BIT | LLAPT_BIT | ANC_BIT | PIPE_BIT | VAD_BIT),
    /* RECORD */
    [AUDIO_CATEGORY_RECORD] =
    (AUDIO_BIT | RECORD_BIT | LINE_BIT | TONE_BIT | VP_BIT | APT_BIT | LLAPT_BIT | ANC_BIT | PIPE_BIT | VAD_BIT),
    /* LINE */
    [AUDIO_CATEGORY_LINE] =
    (AUDIO_BIT | VOICE_BIT | RECORD_BIT | TONE_BIT | VP_BIT | LLAPT_BIT | ANC_BIT | PIPE_BIT | VAD_BIT),
    /* TONE */
    [AUDIO_CATEGORY_TONE] =
    (AUDIO_BIT | VOICE_BIT | RECORD_BIT | LINE_BIT | APT_BIT | LLAPT_BIT | ANC_BIT | PIPE_BIT | VAD_BIT),
    /* VP */
    [AUDIO_CATEGORY_VP] =
    (AUDIO_BIT | VOICE_BIT | RECORD_BIT | LINE_BIT | APT_BIT | LLAPT_BIT | ANC_BIT | PIPE_BIT | VAD_BIT),
    /* APT */
    [AUDIO_CATEGORY_APT] =
    (AUDIO_BIT | VOICE_BIT | RECORD_BIT | TONE_BIT | VP_BIT | ANC_BIT | VAD_BIT | PIPE_BIT),
    /* LLAPT */
    [AUDIO_CATEGORY_LLAPT] =
    (AUDIO_BIT | VOICE_BIT | RECORD_BIT | LINE_BIT | TONE_BIT | VP_BIT | VAD_BIT | PIPE_BIT),
    /* ANC */
    [AUDIO_CATEGORY_ANC] =
    (AUDIO_BIT | VOICE_BIT | RECORD_BIT | LINE_BIT | TONE_BIT | VP_BIT | APT_BIT | VAD_BIT | PIPE_BIT),
    /* VAD */
    [AUDIO_CATEGORY_VAD] =
    (AUDIO_BIT | VOICE_BIT | RECORD_BIT | LINE_BIT | TONE_BIT | VP_BIT | KWS_BIT | PIPE_BIT),
    /* KWS */
    [AUDIO_CATEGORY_KWS] =
    (AUDIO_BIT | VOICE_BIT | RECORD_BIT | LINE_BIT | TONE_BIT | VP_BIT | PIPE_BIT | ANC_BIT | VAD_BIT | KWS_BIT),
    /* PIPE */
    [AUDIO_CATEGORY_PIPE] =
    (AUDIO_BIT | VOICE_BIT | RECORD_BIT | LINE_BIT | TONE_BIT | VP_BIT | APT_BIT | LLAPT_BIT | ANC_BIT | VAD_BIT | PIPE_BIT),
};

static const uint16_t priority_strategy_map[AUDIO_CATEGORY_NUM] =
{
    /* AUDIO */
    [AUDIO_CATEGORY_AUDIO] =
    (VOICE_BIT | TONE_BIT | VP_BIT | ANC_BIT | LLAPT_BIT),
    /* VOICE */
    [AUDIO_CATEGORY_VOICE] =
    (TONE_BIT | VP_BIT),
    /* RECORD */
    [AUDIO_CATEGORY_RECORD] =
    (VOICE_BIT),
    /* LINE */
    [AUDIO_CATEGORY_LINE] =
    (VOICE_BIT | TONE_BIT | VP_BIT),
    /* TONE */
    [AUDIO_CATEGORY_TONE] = 0,
    /* VP */
    [AUDIO_CATEGORY_VP] = 0,
    /* APT */
    [AUDIO_CATEGORY_APT] =
    (AUDIO_BIT | VOICE_BIT | TONE_BIT | VP_BIT),
    /* LLAPT */
    [AUDIO_CATEGORY_LLAPT] = 0,
    /* ANC */
    [AUDIO_CATEGORY_ANC] = 0,
    /* VAD */
    [AUDIO_CATEGORY_VAD] =
    (VOICE_BIT | RECORD_BIT | LINE_BIT | TONE_BIT | VP_BIT | APT_BIT | LLAPT_BIT | ANC_BIT),
    /* PIPE */
    [AUDIO_CATEGORY_PIPE] = 0,
};

static const int8_t ramp_gain_policy_table[AUDIO_CATEGORY_NUM][AUDIO_CATEGORY_NUM] =
{
    /* AUDIO, VOICE, RECORD, LINE, TONE, VP, APT, LLAPT, ANC, VAD, KWS, PIPE */
    [AUDIO_CATEGORY_AUDIO]  = { 0,  0, 0, 0, +1, +1, 0, 0, 0, 0, 0, 0},
    [AUDIO_CATEGORY_VOICE]  = { 0,  0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0},
    [AUDIO_CATEGORY_RECORD] = { 0,  0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0},
    [AUDIO_CATEGORY_LINE]   = { 0,  0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0},
    [AUDIO_CATEGORY_TONE]   = {-1, -1, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0},
    [AUDIO_CATEGORY_VP]     = {-1, -1, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0},
    [AUDIO_CATEGORY_APT]    = { 0,  0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0},
    [AUDIO_CATEGORY_LLAPT]  = { 0,  0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0},
    [AUDIO_CATEGORY_ANC]    = { 0,  0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0},
    [AUDIO_CATEGORY_VAD]    = { 0,  0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0},
    [AUDIO_CATEGORY_KWS]    = { 0,  0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0},
    [AUDIO_CATEGORY_PIPE]   = { 0,  0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0},
};

void audio_path_schedule(void);

static void audio_path_state_set(T_AUDIO_PATH       *audio_path,
                                 T_AUDIO_PATH_STATE  state);
static uint32_t audio_path_domain_mask_get(T_AUDIO_PATH *path);

static void audio_path_wait_codec_enable_handler(void *handle)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (path != NULL)
    {
        T_AUDIO_PLUGIN_PARAM plugin_param = {0};

        if (path->gateway_session != NULL)
        {
            plugin_param.dac_sample_rate = gateway_session_sample_rate_get(path->gateway_session,
                                                                           AUDIO_ROUTE_GATEWAY_SPORT,
                                                                           AUDIO_ROUTE_GATEWAY_0,
                                                                           AUDIO_ROUTE_GATEWAY_DIR_TX);
            plugin_param.adc_sample_rate = gateway_session_sample_rate_get(path->gateway_session,
                                                                           AUDIO_ROUTE_GATEWAY_SPORT,
                                                                           AUDIO_ROUTE_GATEWAY_0,
                                                                           AUDIO_ROUTE_GATEWAY_DIR_RX);

            gateway_session_enable(path->gateway_session);
        }

        if (path->vad_session != NULL)
        {
            vad_mgr_session_enable(path->vad_session);
        }

        audio_plugin_occasion_occur(path,
                                    path->category,
                                    AUDIO_PLUGIN_OCCASION_ANALOG_DOMAIN_ON,
                                    plugin_param);
    }
}

static bool audio_path_wait_codec_enabled(T_AUDIO_PATH *path)
{
    uint32_t domain_mask;

    domain_mask = audio_path_domain_mask_get(path);

    if (domain_mask & AUDIO_PATH_DOMAIN_CODEC)
    {
        if (!sys_event_flag_check(path->event_group, AUDIO_PATH_DOMAIN_CODEC))
        {
            sys_event_flag_wait(path->event_group,
                                audio_path_wait_codec_enable_handler,
                                (void *)path,
                                AUDIO_PATH_DOMAIN_CODEC,
                                SYS_EVENT_FLAG_TYPE_SET_AND);

            return false;
        }
    }

    return true;
}

static void anc_llapt_wait_enable_finish_cback(void *handle)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (path != NULL)
    {
        audio_path_state_set(path, AUDIO_PATH_STATE_RUNNING);
    }

    audio_path_schedule();
}

static int16_t audio_path_gain_scale(int16_t gain_db,
                                     float   scale)
{
    float gain;

    if (fabs(scale) >= (0.375f / 128.0f))
    {
        gain  = pow(10.0f, gain_db / 128.0f / 20.0f);
        gain *= scale;
    }
    else
    {
        gain = pow(10.0f, (-128 * 128) / 128.0f / 20.0f);
    }

    return (int16_t)(20 * log10(gain) * 128);
}

static void audio_path_ramp_gain_down(T_AUDIO_PATH *initiator_path)
{
    T_AUDIO_PATH *acceptor_path;

    acceptor_path = os_queue_peek(&audio_path_db.path_list, 0);
    while (acceptor_path != NULL)
    {
        int8_t policy;

        if (acceptor_path->state == AUDIO_PATH_STATE_RUNNING)
        {
            T_AUDIO_PATH *ramp_path;

            ramp_path = NULL;
            policy = ramp_gain_policy_table[acceptor_path->category][initiator_path->category];
            if (policy > 0)
            {
                ramp_path = acceptor_path;
            }
            else if (policy < 0)
            {
                ramp_path = initiator_path;
            }

            if (ramp_path != NULL)
            {
                int16_t left_ramp_gain;
                int16_t right_ramp_gain;
                uint8_t ramp_gain_scaling;

                ramp_gain_scaling = audio_route_ramp_gain_scaling_get(ramp_path->category);
                left_ramp_gain = audio_path_gain_scale(ramp_path->dac_left_gain,
                                                       ramp_gain_scaling / 100.0f);
                right_ramp_gain = audio_path_gain_scale(ramp_path->dac_right_gain,
                                                        ramp_gain_scaling / 100.0f);

                dsp_mgr_decoder_ramp_gain_set(ramp_path->dsp_session,
                                              left_ramp_gain,
                                              right_ramp_gain,
                                              100);
            }
        }

        acceptor_path = LIST_NEXT_ENTRY(acceptor_path, state_list);
    }
}

static void audio_path_ramp_gain_up(T_AUDIO_PATH *initiator_path)
{
    T_AUDIO_PATH *acceptor_path;

    acceptor_path = os_queue_peek(&audio_path_db.path_list, 0);
    while (acceptor_path != NULL)
    {
        int8_t policy;

        if (acceptor_path->state == AUDIO_PATH_STATE_RUNNING)
        {
            policy = ramp_gain_policy_table[acceptor_path->category][initiator_path->category];
            if (policy > 0)
            {
                dsp_mgr_decoder_ramp_gain_set(acceptor_path->dsp_session,
                                              acceptor_path->dac_left_gain,
                                              acceptor_path->dac_right_gain,
                                              100);
            }
            else if (policy < 0)
            {
                dsp_mgr_decoder_ramp_gain_set(initiator_path->dsp_session,
                                              initiator_path->dac_left_gain,
                                              initiator_path->dac_right_gain,
                                              100);
            }
        }

        acceptor_path = LIST_NEXT_ENTRY(acceptor_path, state_list);
    }
}

static void audio_path_wait_running_handler(void *handle)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (path != NULL)
    {
        if ((path->category == AUDIO_CATEGORY_LLAPT) || (path->category == AUDIO_CATEGORY_ANC))
        {
            if (!sys_event_flag_check(audio_path_db.event_group, AUDIO_PATH_EVENT_FLAG_ANC_LLAPT_ENABLING))
            {
                sys_event_flag_post(audio_path_db.event_group, SYS_EVENT_FLAG_OPT_SET,
                                    AUDIO_PATH_EVENT_FLAG_ANC_LLAPT_ENABLING);

                sys_event_flag_wait(audio_path_db.event_group,
                                    anc_llapt_wait_enable_finish_cback,
                                    (void *)path,
                                    AUDIO_PATH_EVENT_FLAG_ANC_LLAPT_ENABLING,
                                    SYS_EVENT_FLAG_TYPE_CLEAR_AND);
#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
                anc_mgr_enable();
#endif
            }
        }
        else
        {
            audio_path_ramp_gain_down(path);
            audio_path_state_set(path, AUDIO_PATH_STATE_RUNNING);
            audio_path_schedule();
        }
    }
}

static void anc_llapt_wait_disable_finish_cback(void *handle)
{
    T_AUDIO_PATH *path;
    T_AUDIO_PLUGIN_PARAM plugin_param;

    plugin_param.dac_sample_rate = 48000;
    plugin_param.adc_sample_rate = 48000;

    path = (T_AUDIO_PATH *)handle;
    if (path != NULL)
    {
        /* post a dummy AUDIO_PLUGIN_OCCASION_DIGITAL_DOMAIN_OFF to make path into IDLE */
        audio_plugin_occasion_occur((T_AUDIO_PATH_HANDLE)path,
                                    path->category,
                                    AUDIO_PLUGIN_OCCASION_DIGITAL_DOMAIN_OFF,
                                    plugin_param);
    }
}

static void audio_path_dac_gain_set(T_AUDIO_PATH *path,
                                    int16_t       left_gain,
                                    int16_t       right_gain)
{
#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
    T_AUDIO_CATEGORY category = path->category;

    if ((category == AUDIO_CATEGORY_LLAPT) ||
        (category == AUDIO_CATEGORY_ANC))
    {
        anc_mgr_gain_set(left_gain, right_gain);
    }
    else
#endif
    {
        dsp_mgr_decoder_gain_set(path->dsp_session, left_gain, right_gain);
    }

    path->dac_left_gain = left_gain;
    path->dac_right_gain = right_gain;
}

static void audio_path_adc_gain_set(T_AUDIO_PATH *path,
                                    int16_t       left_gain,
                                    int16_t       right_gain)
{
    if (path->dsp_session != NULL)
    {
        dsp_mgr_encoder_gain_set(path->dsp_session, left_gain, right_gain);
    }
}

static void audio_path_codec_event_handler(uint32_t param)
{
    T_DSP_SCHED_MSG *dsp_msg;
    T_CODEC_MGR_EVENT_PARAM *event_param;
    T_AUDIO_PATH *session_path;
    T_AUDIO_PLUGIN_PARAM plugin_param;

    dsp_msg = (T_DSP_SCHED_MSG *)param;
    event_param = (T_CODEC_MGR_EVENT_PARAM *)(dsp_msg->p_data);
    session_path = (T_AUDIO_PATH *)(event_param->context);

    AUDIO_PRINT_TRACE2("audio_path_codec_event_handler: codec state %d, path %p",
                       event_param->state, session_path);

    if (session_path != NULL)
    {
        if (session_path->category == AUDIO_CATEGORY_ANC ||
            session_path->category == AUDIO_CATEGORY_LLAPT)
        {
            plugin_param.dac_sample_rate = 48000; /* fixed in 48K */
            plugin_param.adc_sample_rate = 48000; /* fixed in 48K */
        }
        else
        {
            plugin_param.dac_sample_rate = gateway_session_sample_rate_get(session_path->gateway_session,
                                                                           AUDIO_ROUTE_GATEWAY_SPORT,
                                                                           AUDIO_ROUTE_GATEWAY_0,
                                                                           AUDIO_ROUTE_GATEWAY_DIR_TX);
            plugin_param.adc_sample_rate = gateway_session_sample_rate_get(session_path->gateway_session,
                                                                           AUDIO_ROUTE_GATEWAY_SPORT,
                                                                           AUDIO_ROUTE_GATEWAY_0,
                                                                           AUDIO_ROUTE_GATEWAY_DIR_RX);
        }

        if (event_param->state == CODEC_MGR_SESSION_STATE_ENABLED)
        {
            sys_event_flag_post(session_path->event_group,
                                SYS_EVENT_FLAG_OPT_SET,
                                AUDIO_PATH_DOMAIN_CODEC);
            if (session_path->category == AUDIO_CATEGORY_LLAPT ||
                session_path->category == AUDIO_CATEGORY_ANC)
            {
                /* post the AUDIO_PLUGIN_OCCASION_ANALOG_DOMAIN_ON after codec on */
                audio_plugin_occasion_occur((T_AUDIO_PATH_HANDLE)session_path,
                                            session_path->category,
                                            AUDIO_PLUGIN_OCCASION_ANALOG_DOMAIN_ON,
                                            plugin_param);
            }
            else if ((session_path->category == AUDIO_CATEGORY_VAD) &&
                     (session_path->param.vad.type == VAD_TYPE_HW))
            {
                plugin_param.dac_sample_rate = 16000;
                plugin_param.adc_sample_rate = 16000;

                if (session_path->gateway_session != NULL)
                {
                    gateway_session_enable(session_path->gateway_session);
                }

                audio_plugin_occasion_occur((T_AUDIO_PATH_HANDLE)session_path,
                                            session_path->category,
                                            AUDIO_PLUGIN_OCCASION_ANALOG_DOMAIN_ON,
                                            plugin_param);
            }

        }
        else if (event_param->state == CODEC_MGR_SESSION_STATE_DISABLED)
        {
            sys_event_flag_post(session_path->event_group,
                                SYS_EVENT_FLAG_OPT_CLEAR,
                                AUDIO_PATH_DOMAIN_CODEC);
            audio_plugin_occasion_occur((T_AUDIO_PATH_HANDLE)session_path,
                                        session_path->category,
                                        AUDIO_PLUGIN_OCCASION_ANALOG_DOMAIN_OFF,
                                        plugin_param);
        }
    }
}

static T_AUDIO_PATH *audio_path_find_category(uint8_t          state,
                                              T_AUDIO_CATEGORY category)
{
    T_AUDIO_PATH *path;

    path = os_queue_peek(&audio_path_db.path_list, 0);
    while (path != NULL)
    {
        if ((path->category == category) && (path->state == state))
        {
            return path;
        }

        path = LIST_NEXT_ENTRY(path, state_list);
    }

    return NULL;
}

static bool audio_path_starting_check(void)
{
    T_AUDIO_PATH *path;

    path = os_queue_peek(&audio_path_db.path_list, 0);
    while (path != NULL)
    {
        if (path->state == AUDIO_PATH_STATE_READY)
        {
            return true;
        }

        path = LIST_NEXT_ENTRY(path, state_list);
    }

    return false;
}

static void audio_path_state_set(T_AUDIO_PATH       *path,
                                 T_AUDIO_PATH_STATE  state)
{
    T_AUDIO_PATH_EVENT event;

    AUDIO_PRINT_INFO4("audio_path_state_set: path %p, category 0x%02x, old state 0x%02x, new state 0x%02x",
                      path, path->category, path->state, state);

    path->state = state;

    switch (state)
    {
    case AUDIO_PATH_STATE_IDLE:
        {
            event = AUDIO_PATH_EVT_IDLE;
        }
        break;

    case AUDIO_PATH_STATE_READY:
        {
            // Once an audio path enters the ready state, set PATH_STARTING flag
            sys_event_flag_post(audio_path_db.event_group, SYS_EVENT_FLAG_OPT_SET,
                                AUDIO_PATH_EVENT_FLAG_PATH_STARTING);
            event = AUDIO_PATH_EVT_NONE;
        }
        break;

    case AUDIO_PATH_STATE_RUNNING:
        {
            event = AUDIO_PATH_EVT_RUNNING;
        }
        break;

    case AUDIO_PATH_STATE_SUSPENDING:
        {
            event = AUDIO_PATH_EVT_SUSPEND;
        }
        break;

    default:
        {
            event = AUDIO_PATH_EVT_NONE;
        }
        break;
    }

    if (event != AUDIO_PATH_EVT_NONE)
    {
        path->cback(path, event, 0);
    }
    if (event == AUDIO_PATH_EVT_RUNNING)
    {
        // When all paths are in running state, clear the PATH_STARTING flag
        if (audio_path_starting_check() == false)
        {
            sys_event_flag_post(audio_path_db.event_group, SYS_EVENT_FLAG_OPT_CLEAR,
                                AUDIO_PATH_EVENT_FLAG_PATH_STARTING);
        }
    }
}

static bool audio_path_mix_strategy(T_AUDIO_PATH *origin,
                                    T_AUDIO_PATH *mix)
{
    if ((mix_strategy_map[origin->category] & (1U << mix->category)) != 0)
    {
        return true;
    }

    return false;
}

static bool audio_path_is_mixable(T_AUDIO_PATH *mix)
{
    T_AUDIO_PATH *path;

    path = os_queue_peek(&audio_path_db.path_list, 0);
    while (path != NULL)
    {
        if (path->state == AUDIO_PATH_STATE_RUNNING)
        {
            if (audio_path_mix_strategy(path, mix) == false)
            {
                return false;
            }
        }

        path = LIST_NEXT_ENTRY(path, state_list);
    }

    return true;
}

static T_AUDIO_PATH *audio_path_schedule_priority(T_AUDIO_PATH *first_path,
                                                  T_AUDIO_PATH *second_path)
{
    T_AUDIO_PATH *path;

    if ((priority_strategy_map[first_path->category] & (1U << second_path->category)) != 0)
    {
        path = second_path;
    }
    else
    {
        path = first_path;
    }

    return path;
}

static T_AUDIO_PATH *audio_path_schedule_switch(void)
{
    T_AUDIO_PATH *path;
    T_AUDIO_PATH *select_path;

    path = os_queue_peek(&audio_path_db.path_list, 0);
    while (path != NULL)
    {
        if (path->state == AUDIO_PATH_STATE_PENDING)
        {
            break;
        }

        path = LIST_NEXT_ENTRY(path, state_list);
    }

    select_path = path;
    while (path != NULL)
    {
        if (path->state == AUDIO_PATH_STATE_PENDING)
        {
            select_path = audio_path_schedule_priority(select_path, path);
        }

        path = LIST_NEXT_ENTRY(path, state_list);
    }

    return select_path;
}

static void audio_path_plugin_msg_handle(T_AUDIO_PATH            *path,
                                         T_AUDIO_PLUGIN_OCCASION  occasion)
{
    AUDIO_PRINT_INFO2("audio_path_plugin_msg_handle: path 0x%08x, occasion %d", path, occasion);

    if (path != NULL)
    {
        switch (occasion)
        {
        case AUDIO_PLUGIN_OCCASION_ANALOG_DOMAIN_ON:
            {
                if (path->category == AUDIO_CATEGORY_LLAPT ||
                    path->category == AUDIO_CATEGORY_ANC)
                {
                    T_AUDIO_PLUGIN_PARAM param;

                    param.dac_sample_rate = 48000; /* fixed in 48K */
                    param.adc_sample_rate = 48000; /* fixed in 48K */
                    /* post a dummy AUDIO_PLUGIN_OCCASION_DIGITAL_DOMAIN_ON to make path running */
                    audio_plugin_occasion_occur(path,
                                                path->category,
                                                AUDIO_PLUGIN_OCCASION_DIGITAL_DOMAIN_ON,
                                                param);
                }
                else if ((path->category == AUDIO_CATEGORY_VAD) &&
                         (path->param.vad.type == VAD_TYPE_HW))
                {
                    T_AUDIO_PLUGIN_PARAM param;

                    param.dac_sample_rate = 16000;
                    param.adc_sample_rate = 16000;

                    audio_plugin_occasion_occur(path,
                                                path->category,
                                                AUDIO_PLUGIN_OCCASION_DIGITAL_DOMAIN_ON,
                                                param);
                }
                else
                {
                    dsp_mgr_session_run(path->dsp_session);
                }
            }
            break;

        case AUDIO_PLUGIN_OCCASION_ANALOG_DOMAIN_OFF:
            {
                if (path->state == AUDIO_PATH_STATE_STOPPING)
                {
                    audio_path_state_set(path, AUDIO_PATH_STATE_IDLE);
                }
                else if (path->state == AUDIO_PATH_STATE_SUSPENDING)
                {
                    audio_path_state_set(path, AUDIO_PATH_STATE_PENDING);
                }

                audio_path_schedule();
            }
            break;

        case AUDIO_PLUGIN_OCCASION_DIGITAL_DOMAIN_ON:
            {
                sys_event_flag_post(path->event_group,
                                    SYS_EVENT_FLAG_OPT_SET,
                                    AUDIO_PATH_DOMAIN_PLUGIN);
            }
            break;

        case AUDIO_PLUGIN_OCCASION_DIGITAL_DOMAIN_OFF:
            {
                sys_event_flag_post(path->event_group,
                                    SYS_EVENT_FLAG_OPT_CLEAR,
                                    AUDIO_PATH_DOMAIN_PLUGIN);

                if ((path->state == AUDIO_PATH_STATE_SUSPENDING) ||
                    (path->state == AUDIO_PATH_STATE_STOPPING))
                {
                    if (path->codec_session != NULL)
                    {
                        codec_mgr_session_disable(path->codec_session);
                    }

                    if (path->gateway_session != NULL)
                    {
                        gateway_session_disable(path->gateway_session);
                    }

                    if (path->vad_session != NULL)
                    {
                        vad_mgr_session_disable(path->vad_session);
                    }

                    if (path->codec_session == NULL)
                    {
                        T_AUDIO_PLUGIN_PARAM param;

                        param.dac_sample_rate = 0;
                        param.adc_sample_rate = 0;

                        audio_plugin_occasion_occur(path,
                                                    path->category,
                                                    AUDIO_PLUGIN_OCCASION_ANALOG_DOMAIN_OFF,
                                                    param);
                    }
                }
            }
            break;

        default:
            break;
        }
    }
}

void audio_path_msg_handler(void)
{
    T_AUDIO_PATH_MSG msg;

    if (os_msg_recv(hAudioPathMsgQueueHandle, &msg, 0) == true)
    {
        switch (msg.type)
        {
        case AUDIO_PATH_MSG_TYPE_PLUGIN:
            {
                audio_path_plugin_msg_handle((T_AUDIO_PATH *)msg.handle,
                                             (T_AUDIO_PLUGIN_OCCASION)msg.occasion);
            }
            break;

        case AUDIO_PATH_MSG_TYPE_CREATE:
            {
                msg.cback(msg.handle,
                          AUDIO_PATH_EVT_CREATE,
                          0);
            }
            break;

        case AUDIO_PATH_MSG_TYPE_DESTROY:
            {
                msg.cback(msg.handle,
                          AUDIO_PATH_EVT_RELEASE,
                          0);
            }
            break;

        default:
            break;
        }
    }
}

static uint32_t audio_path_domain_mask_get(T_AUDIO_PATH *path)
{
    uint32_t domain_mask = 0;

    switch (path->category)
    {
    case AUDIO_CATEGORY_AUDIO:
        {
            domain_mask = AUDIO_PATH_DOMAIN_DSP |
                          AUDIO_PATH_DOMAIN_GATEWAY |
                          AUDIO_PATH_DOMAIN_PLUGIN;

            if (path->device & (AUDIO_DEVICE_OUT_SPK | AUDIO_DEVICE_OUT_AUX))
            {
                domain_mask |= AUDIO_PATH_DOMAIN_CODEC;
            }
        }
        break;

    case AUDIO_CATEGORY_VOICE:
        {
            domain_mask = AUDIO_PATH_DOMAIN_DSP |
                          AUDIO_PATH_DOMAIN_GATEWAY |
                          AUDIO_PATH_DOMAIN_PLUGIN;

            if (path->device & (AUDIO_DEVICE_OUT_SPK | AUDIO_DEVICE_OUT_AUX |
                                AUDIO_DEVICE_IN_MIC | AUDIO_DEVICE_IN_AUX))
            {
                domain_mask |= AUDIO_PATH_DOMAIN_CODEC;
            }
        }
        break;

    case AUDIO_CATEGORY_RECORD:
        {
            domain_mask = AUDIO_PATH_DOMAIN_DSP |
                          AUDIO_PATH_DOMAIN_GATEWAY |
                          AUDIO_PATH_DOMAIN_PLUGIN;

            if (path->device & (AUDIO_DEVICE_IN_MIC | AUDIO_DEVICE_IN_AUX))
            {
                domain_mask |= AUDIO_PATH_DOMAIN_CODEC;
            }
        }
        break;

    case AUDIO_CATEGORY_LINE:
        {
            domain_mask = AUDIO_PATH_DOMAIN_DSP |
                          AUDIO_PATH_DOMAIN_GATEWAY |
                          AUDIO_PATH_DOMAIN_PLUGIN;

            if (path->device & (AUDIO_DEVICE_OUT_SPK | AUDIO_DEVICE_OUT_AUX |
                                AUDIO_DEVICE_IN_MIC | AUDIO_DEVICE_IN_AUX))
            {
                domain_mask |= AUDIO_PATH_DOMAIN_CODEC;
            }
        }
        break;

    case AUDIO_CATEGORY_TONE:
        {
            domain_mask = AUDIO_PATH_DOMAIN_DSP |
                          AUDIO_PATH_DOMAIN_GATEWAY |
                          AUDIO_PATH_DOMAIN_PLUGIN;

            if (path->device & (AUDIO_DEVICE_OUT_SPK | AUDIO_DEVICE_OUT_AUX))
            {
                domain_mask |= AUDIO_PATH_DOMAIN_CODEC;
            }
        }
        break;

    case AUDIO_CATEGORY_VP:
        {
            domain_mask = AUDIO_PATH_DOMAIN_DSP |
                          AUDIO_PATH_DOMAIN_GATEWAY |
                          AUDIO_PATH_DOMAIN_PLUGIN;

            if (path->device & (AUDIO_DEVICE_OUT_SPK | AUDIO_DEVICE_OUT_AUX))
            {
                domain_mask |= AUDIO_PATH_DOMAIN_CODEC;
            }
        }
        break;

    case AUDIO_CATEGORY_APT:
        {
            domain_mask = AUDIO_PATH_DOMAIN_CODEC |
                          AUDIO_PATH_DOMAIN_DSP |
                          AUDIO_PATH_DOMAIN_GATEWAY |
                          AUDIO_PATH_DOMAIN_PLUGIN;
        }
        break;

    case AUDIO_CATEGORY_LLAPT:
        {
            domain_mask = AUDIO_PATH_DOMAIN_CODEC |
                          AUDIO_PATH_DOMAIN_PLUGIN;
        }
        break;

    case AUDIO_CATEGORY_ANC:
        {
            domain_mask = AUDIO_PATH_DOMAIN_CODEC |
                          AUDIO_PATH_DOMAIN_PLUGIN;
        }
        break;

    case AUDIO_CATEGORY_VAD:
        {
            if (path->param.vad.type == VAD_TYPE_SW)
            {
                domain_mask = AUDIO_PATH_DOMAIN_CODEC |
                              AUDIO_PATH_DOMAIN_DSP |
                              AUDIO_PATH_DOMAIN_GATEWAY |
                              AUDIO_PATH_DOMAIN_PLUGIN;
            }
            else if (path->param.vad.type == VAD_TYPE_HW)
            {
                domain_mask = AUDIO_PATH_DOMAIN_CODEC |
                              AUDIO_PATH_DOMAIN_GATEWAY |
                              AUDIO_PATH_DOMAIN_PLUGIN;
            }
            else if (path->param.vad.type == VAD_TYPE_FUSION)
            {
                domain_mask = AUDIO_PATH_DOMAIN_CODEC |
                              AUDIO_PATH_DOMAIN_DSP |
                              AUDIO_PATH_DOMAIN_GATEWAY |
                              AUDIO_PATH_DOMAIN_PLUGIN |
                              AUDIO_PATH_DOMAIN_VAD;
            }
        }
        break;

    case AUDIO_CATEGORY_KWS:
        {
            domain_mask = AUDIO_PATH_DOMAIN_DSP |
                          AUDIO_PATH_DOMAIN_PLUGIN;
        }
        break;

    case AUDIO_CATEGORY_PIPE:
        {
            domain_mask = AUDIO_PATH_DOMAIN_DSP;
        }
        break;

    default:
        break;
    }

    return domain_mask;
}

static bool audio_path_is_stable(void)
{
    T_AUDIO_PATH *path;

    path = os_queue_peek(&audio_path_db.path_list, 0);
    while (path != NULL)
    {
        if ((path->state == AUDIO_PATH_STATE_READY) ||
            (path->state == AUDIO_PATH_STATE_STOPPING) ||
            (path->state == AUDIO_PATH_STATE_SUSPENDING))
        {
            return false;
        }

        path = LIST_NEXT_ENTRY(path, state_list);
    }

    return true;
}

static void audio_path_suspend(T_AUDIO_PATH *path)
{
    T_AUDIO_PATH *running_path;

    running_path = os_queue_peek(&audio_path_db.path_list, 0);
    while (running_path != NULL)
    {
        if ((running_path->state == AUDIO_PATH_STATE_RUNNING) &&
            (running_path->category != AUDIO_CATEGORY_ANC) &&
            (running_path->category != AUDIO_CATEGORY_LLAPT))
        {
            if (path == audio_path_schedule_priority(running_path, path))
            {
                dsp_mgr_session_disable(running_path->dsp_session);
                audio_path_state_set(running_path, AUDIO_PATH_STATE_SUSPENDING);
            }
        }

        running_path = LIST_NEXT_ENTRY(running_path, state_list);
    }
}

void audio_path_schedule(void)
{
    T_AUDIO_PATH *path;

    path = audio_path_schedule_switch();
    if (path != NULL)
    {
        if (audio_path_is_stable() == false)
        {
            return;
        }

        if (path->dsp_session != NULL)
        {
            if (!dsp_mgr_is_stable(path->dsp_session))
            {
                return;
            }
        }

        if (audio_path_is_mixable(path))
        {
            uint32_t domain_mask;

            audio_path_state_set(path, AUDIO_PATH_STATE_READY);

            domain_mask  = audio_path_domain_mask_get(path);
            domain_mask &= ~AUDIO_PATH_DOMAIN_GATEWAY;
            domain_mask &= ~AUDIO_PATH_DOMAIN_VAD;

            if (domain_mask & AUDIO_PATH_DOMAIN_CODEC)
            {
                codec_mgr_session_enable(path->codec_session);
            }

            if (domain_mask & AUDIO_PATH_DOMAIN_DSP)
            {
                dsp_mgr_session_enable(path->dsp_session);
            }

            sys_event_flag_wait(path->event_group,
                                audio_path_wait_running_handler,
                                path,
                                domain_mask,
                                SYS_EVENT_FLAG_TYPE_SET_AND);
        }
        else
        {
            audio_path_suspend(path);
        }
    }
}

static bool audio_path_dsp_cback(uint32_t  event,
                                 void     *msg)
{
    AUDIO_PRINT_TRACE2("audio_path_dsp_cback: event 0x%02x param %p", event, msg);

    switch (event)
    {
    case DSP_MGR_EVT_POWER_ON:
        {
            audio_path_schedule();
        }
        break;

    case DSP_MGR_EVT_CODEC_STATE:
        {
            audio_path_codec_event_handler((uint32_t)msg);
        }
        break;

    case DSP_MGR_EVT_POWER_OFF:
        {
            audio_path_schedule();
        }
        break;

    case DSP_MGR_EVT_PATH_SCHEDULE:
        {
            audio_path_schedule();
        }
        break;

    case DSP_MGR_EVT_DSP_INTER_MSG:
        {
            audio_path_db.cback(AUDIO_PATH_EVT_DSP_INTER_MSG, msg);
        }
        break;

    default:
        break;
    }

    return true;
}

#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
static bool audio_path_anc_cback(T_ANC_MGR_EVENT event)
{
    switch (event)
    {
    case ANC_MGR_EVENT_ENABLED:
        {
            if (sys_event_flag_check(audio_path_db.event_group, AUDIO_PATH_EVENT_FLAG_ANC_LLAPT_ENABLING))
            {
                sys_event_flag_post(audio_path_db.event_group, SYS_EVENT_FLAG_OPT_CLEAR,
                                    AUDIO_PATH_EVENT_FLAG_ANC_LLAPT_ENABLING);
            }
        }
        break;

    case ANC_MGR_EVENT_DISABLED:
        {
            if (sys_event_flag_check(audio_path_db.event_group, AUDIO_PATH_EVENT_FLAG_ANC_LLAPT_DISABLING))
            {
                sys_event_flag_post(audio_path_db.event_group, SYS_EVENT_FLAG_OPT_CLEAR,
                                    AUDIO_PATH_EVENT_FLAG_ANC_LLAPT_DISABLING);
            }
        }
        break;

    default:
        break;
    }

    return true;
}
#endif

bool audio_path_init(P_AUDIO_PATH_MGR_CBACK cback)
{
    int32_t ret = 0;

    if (audio_route_init() == false)
    {
        ret = 1;
        goto fail_init_audio_route;
    }

    if (bin_loader_init() == false)
    {
        ret = 2;
        goto fail_init_bin_loader;
    }

    if (dsp_mgr_init(audio_path_dsp_cback) == false)
    {
        ret = 3;
        goto fail_init_dsp_mgr;
    }

    if (gateway_init() == false)
    {
        ret = 4;
        goto fail_init_gateway;
    }

    if (codec_mgr_init() == false)
    {
        ret = 5;
        goto fail_init_codec_mgr;
    }
#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
    if (anc_mgr_init(audio_path_anc_cback) == false)
    {
        ret = 6;
        goto fail_init_anc_mgr;
    }
#endif
    os_queue_init(&audio_path_db.path_list);

    audio_path_db.cback  = cback;
    audio_path_db.event_group = sys_event_group_create(0);

    if (audio_plugin_init() == false)
    {
        ret = 7;
        goto fail_init_audio_plugin;
    }

    if (os_msg_queue_create(&hAudioPathMsgQueueHandle,
                            "pathQ",
                            8,
                            sizeof(T_AUDIO_PATH_MSG)) == false)
    {
        ret = 8;
        goto fail_create_msg_queue;
    }

    sys_mgr_event_register(EVENT_AUDIO_PATH_MSG, audio_path_msg_handler);

    return true;

fail_create_msg_queue:
    audio_plugin_deinit();
fail_init_audio_plugin:
#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
    anc_mgr_deinit();
fail_init_anc_mgr:
#endif
    codec_mgr_deinit();
fail_init_codec_mgr:
    gateway_deinit();
fail_init_gateway:
    dsp_mgr_deinit();
fail_init_dsp_mgr:
    bin_loader_deinit();
fail_init_bin_loader:
    audio_route_deinit();
fail_init_audio_route:
    AUDIO_PRINT_ERROR1("audio_path_init: failed %d", -ret);
    return false;
}

void audio_path_deinit(void)
{
    T_AUDIO_PATH *path;
    T_AUDIO_PATH *next;

    LIST_FOR_EACH_ENTRY_SAFE(path, next, &audio_path_db.path_list, state_list)
    {
        free(path);
    }
    os_queue_init(&audio_path_db.path_list);

    sys_event_group_delete(audio_path_db.event_group);

    codec_mgr_deinit();
    audio_route_deinit();
    gateway_deinit();
    dsp_mgr_deinit();
#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
    anc_mgr_deinit();
#endif
    audio_plugin_deinit();

    os_msg_queue_delete(hAudioPathMsgQueueHandle);
}

static void audio_path_dsp_session_cback(void           *handle,
                                         T_DSP_MGR_EVENT  event,
                                         uint32_t         param)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (path != NULL)
    {
        T_AUDIO_PLUGIN_PARAM plugin_param = {0};

        if (path->gateway_session != NULL)
        {
            plugin_param.dac_sample_rate = gateway_session_sample_rate_get(path->gateway_session,
                                                                           AUDIO_ROUTE_GATEWAY_SPORT,
                                                                           AUDIO_ROUTE_GATEWAY_0,
                                                                           AUDIO_ROUTE_GATEWAY_DIR_TX);
            plugin_param.adc_sample_rate = gateway_session_sample_rate_get(path->gateway_session,
                                                                           AUDIO_ROUTE_GATEWAY_SPORT,
                                                                           AUDIO_ROUTE_GATEWAY_0,
                                                                           AUDIO_ROUTE_GATEWAY_DIR_RX);
        }

        switch (event)
        {
        case DSP_MGR_EVT_EFFECT_REQ:
            {
                if (os_queue_search(&audio_path_db.path_list, path))
                {
                    if (path->state == AUDIO_PATH_STATE_READY)
                    {
                        path->cback(path, AUDIO_PATH_EVT_EFFECT_REQ, 0);
                    }
                }
            }
            break;

        case DSP_MGR_EVT_PREPARE_READY:
            {
                if (path->category != AUDIO_CATEGORY_PIPE)
                {
                    if (audio_path_wait_codec_enabled(path))
                    {
                        gateway_session_enable(path->gateway_session);

                        if (path->vad_session != NULL)
                        {
                            vad_mgr_session_enable(path->vad_session);
                        }

                        audio_plugin_occasion_occur(path,
                                                    path->category,
                                                    AUDIO_PLUGIN_OCCASION_ANALOG_DOMAIN_ON,
                                                    plugin_param);
                    }
                }
                else
                {
                    audio_plugin_occasion_occur(path,
                                                path->category,
                                                AUDIO_PLUGIN_OCCASION_ANALOG_DOMAIN_ON,
                                                plugin_param);
                }
            }
            break;

        case DSP_MGR_EVT_SESSION_STARTED:
            {
                sys_event_flag_post(path->event_group,
                                    SYS_EVENT_FLAG_OPT_SET,
                                    AUDIO_PATH_DOMAIN_DSP);
                audio_plugin_occasion_occur(path,
                                            path->category,
                                            AUDIO_PLUGIN_OCCASION_DIGITAL_DOMAIN_ON,
                                            plugin_param);
            }
            break;

        case DSP_MGR_EVT_SESSION_STOPPED:
            {
                sys_event_flag_post(path->event_group,
                                    SYS_EVENT_FLAG_OPT_CLEAR,
                                    AUDIO_PATH_DOMAIN_DSP);
                audio_plugin_occasion_occur(path,
                                            path->category,
                                            AUDIO_PLUGIN_OCCASION_DIGITAL_DOMAIN_OFF,
                                            plugin_param);
            }
            break;

        case DSP_MGR_EVT_DSP_SYNC_V2_SUCC:
            {
                path->cback(path, AUDIO_PATH_EVT_DSP_SYNC_LOCK, 0);
            }
            break;

        case DSP_MGR_EVT_DSP_UNSYNC:
            {
                path->cback(path, AUDIO_PATH_EXC_DSP_UNSYNC, 0);
            }
            break;

        case DSP_MGR_EVT_DSP_SYNC_UNLOCK:
            {
                path->cback(path, AUDIO_PATH_EVT_DSP_SYNC_UNLOCK, 0);
            }
            break;

        case DSP_MGR_EVT_DSP_SYNC_LOCK:
            {
                path->cback(path, AUDIO_PATH_EVT_DSP_SYNC_LOCK, 0);
            }
            break;

        case DSP_MGR_EVT_SYNC_EMPTY:
            {
                path->cback(path, AUDIO_PATH_EXC_DSP_SYNC_EMPTY, 0);
            }
            break;

        case DSP_MGR_EVT_SYNC_LOSE_TIMESTAMP:
            {
                path->cback(path, AUDIO_PATH_EXC_DSP_LOST_TIMESTAMP, 0);
            }
            break;

        case DSP_MGR_EVT_DSP_JOIN_INFO:
            {
                path->cback(path, AUDIO_PATH_EVT_RETURN_JOIN_INFO, param);
            }
            break;

        case DSP_MGR_EVT_DATA_REQ:
            {
                if (path->state == AUDIO_PATH_STATE_RUNNING)
                {
                    path->cback(path, AUDIO_PATH_EVT_DATA_REQ, 0);
                }
            }
            break;

        case DSP_MGR_EVT_DATA_COMPLETE:
            {
                path->cback(path, AUDIO_PATH_EVT_DATA_EMPTY, 0);
            }
            break;

        case DSP_MGR_EVT_DATA_IND:
            {
                if (path->state == AUDIO_PATH_STATE_RUNNING)
                {
                    path->cback((T_AUDIO_PATH_HANDLE)path, AUDIO_PATH_EVT_DATA_IND, 0);
                }
            }
            break;

        case DSP_MGR_EVT_DATA_EMPTY:
            {
                if (audio_path_find_category(AUDIO_PATH_STATE_RUNNING, AUDIO_CATEGORY_APT))
                {
                    dsp_ipc_set_force_dummy_pkt(DSP_IPC_DUMMY_PKT_START);
                }
                path->cback(path, AUDIO_PATH_EVT_DATA_EMPTY, 0);
            }
            break;

        case DSP_MGR_EVT_PLC_NOTIFY:
            {
                T_DSP_MGR_PLC_NOTIFY    *info;
                T_AUDIO_PATH_PLC_NOTIFY  plc;

                info = (T_DSP_MGR_PLC_NOTIFY *)param;
                plc.total_sample_num = info->total_sample_num;
                plc.plc_sample_num = info->plc_sample_num;

                path->cback(path, AUDIO_PATH_EVT_PLC_REPORT, (uint32_t)(&plc));
            }
            break;

        case DSP_MGR_EVT_SIGNAL_REFRESH:
            {
                T_DSP_MGR_SIGNAL_REFRESH *info = (T_DSP_MGR_SIGNAL_REFRESH *)param;
                T_AUDIO_PATH_SIGNAL_REFRESH signal_refresh;

                signal_refresh.direction = (T_AUDIO_PATH_SIGNAL_DIRECTION)info->direction;
                signal_refresh.freq_num = info->freq_num;
                signal_refresh.left_gain_table = info->left_gain_table;
                signal_refresh.right_gain_table = info->right_gain_table;
                path->cback(path, AUDIO_PATH_EVT_SIGNAL_REFRESH, (uint32_t)(&signal_refresh));
            }
            break;

        case DSP_MGR_EVT_FIFO_NOTIFY:
            {
                T_DSP_MGR_LATENCY_NOTIFY    *info;
                T_AUDIO_PATH_LATENCY_REPORT  latency;

                info = (T_DSP_MGR_LATENCY_NOTIFY *)param;
                latency.normal_packet_count = info->packet_number;
                latency.average_packet_latency = info->decoding_latency;
                latency.average_fifo_queuing = info->queuing_latency;

                path->cback(path, AUDIO_PATH_EVT_FIFO_REPORT, (uint32_t)(&latency));
            }
            break;

        case DSP_MGR_EVT_KWS_HIT:
            {
                T_AUDIO_PATH_KWS_HIT keyword;

                keyword.keyword_index = (uint8_t)param;

                path->cback(path, AUDIO_PATH_EVT_KWS_HIT, (uint32_t)&keyword);
            }
            break;

        case DSP_MGR_EVT_ERROR_REPORT:
            {
                path->cback(path, AUDIO_PATH_EVT_ERROR_REPORT, 0);
            }
            break;

        case DSP_MGR_EVT_CODEC_PIPE_MIX:
            {
                path->cback(path, AUDIO_PATH_EVT_PIPE_MIX, 0);
            }
            break;

        case DSP_MGR_EVT_CODEC_PIPE_DEMIX:
            {
                path->cback(path, AUDIO_PATH_EVT_PIPE_DEMIX, 0);
            }
            break;

        case DSP_MGR_EVT_VSE_RESPONSE:
            {
                T_DSP_MGR_VSE_RESPONSE    *info;
                T_AUDIO_PATH_VSE_RESPONSE  vse;

                info = (T_DSP_MGR_VSE_RESPONSE *)param;
                vse.company_id = info->company_id;
                vse.effect_id = info->effect_id;
                vse.seq_num = info->seq_num;
                vse.payload_len = info->payload_len;
                vse.payload = info->payload;

                path->cback(path, AUDIO_PATH_EVT_VSE_RESPONSE, (uint32_t)(&vse));
            }
            break;

        case DSP_MGR_EVT_VSE_INDICATE:
            {
                T_DSP_MGR_VSE_INDICATE    *info;
                T_AUDIO_PATH_VSE_INDICATE  vse;

                info = (T_DSP_MGR_VSE_INDICATE *)param;
                vse.company_id = info->company_id;
                vse.effect_id = info->effect_id;
                vse.seq_num = info->seq_num;
                vse.payload_len = info->payload_len;
                vse.payload = info->payload;

                path->cback(path, AUDIO_PATH_EVT_VSE_INDICATE, (uint32_t)(&vse));
            }
            break;

        case DSP_MGR_EVT_VSE_NOTIFY:
            {
                T_DSP_MGR_VSE_NOTIFY    *info;
                T_AUDIO_PATH_VSE_NOTIFY  vse;

                info = (T_DSP_MGR_VSE_NOTIFY *)param;
                vse.company_id = info->company_id;
                vse.effect_id = info->effect_id;
                vse.seq_num = info->seq_num;
                vse.payload_len = info->payload_len;
                vse.payload = info->payload;

                path->cback(path, AUDIO_PATH_EVT_VSE_NOTIFY, (uint32_t)(&vse));
            }
            break;

        default:
            break;
        }
    }
}

static void audio_path_gateway_cback(T_GATEWAY_EVENT event, void *context)
{
    T_AUDIO_PATH *path = (T_AUDIO_PATH *)context;

    switch (event)
    {
    case GATEWAY_EVENT_SUSPEND:
        {
            if (path->state == AUDIO_PATH_STATE_RUNNING)
            {
                dsp_mgr_session_suspend(path->dsp_session);
            }
        }
        break;

    case GATEWAY_EVENT_RESUME:
        {
            if (path->state == AUDIO_PATH_STATE_RUNNING)
            {
                dsp_mgr_session_resume(path->dsp_session);
            }
        }
        break;

    case GATEWAY_EVENT_DATA_IND:
        {
            path->cback(path, AUDIO_PATH_EVT_DATA_IND, 0);
        }
        break;

    default:
        break;
    }
}

static void audio_path_vad_cback(void            *handle,
                                 T_VAD_MGR_EVENT  event,
                                 uint32_t         param)
{

}

static bool audio_path_build(T_AUDIO_PATH       *path,
                             T_AUDIO_PATH_PARAM  param)
{
    uint32_t dac_sample_rate      = 0;
    uint32_t adc_sample_rate      = 0;
    uint8_t  dac_gain_level       = 0;
    uint8_t  adc_gain_level       = 0;
    uint8_t  aggressiveness_level = 0;
    uint16_t vad_frame_size  = 0;
    T_AUDIO_FORMAT_INFO *decoder_info = NULL;
    T_AUDIO_FORMAT_INFO *encoder_info = NULL;
    uint32_t domain_mask;

    switch (path->category)
    {
    case AUDIO_CATEGORY_AUDIO:
        {
            T_AUDIO_FORMAT_INFO *format_info = param.audio.decoder_info;
            T_AUDIO_ROUTE_DAC_GAIN route_gain;

            path->device    = param.audio.device;
            path->data_mode = param.audio.mode;
            decoder_info    = param.audio.decoder_info;
            dac_gain_level  = param.audio.dac_level;
            dac_sample_rate = audio_codec_sample_rate_get(format_info->type,
                                                          &format_info->attr);

            if (format_info->type == AUDIO_FORMAT_TYPE_CVSD)
            {
                dac_sample_rate = 16000;
            }

            if (audio_route_dac_gain_get(path->category, dac_gain_level, &route_gain) == true)
            {
                path->dac_left_gain  = route_gain.left_gain;
                path->dac_right_gain = route_gain.right_gain;
            }
        }
        break;

    case AUDIO_CATEGORY_VOICE:
        {
            T_AUDIO_FORMAT_INFO *format_info = param.voice.decoder_info;

            path->device    = param.voice.device;
            path->data_mode = param.voice.mode;
            decoder_info    = param.voice.decoder_info;
            encoder_info    = param.voice.encoder_info;
            dac_gain_level  = param.voice.dac_level;
            adc_gain_level  = param.voice.adc_level;
            dac_sample_rate = audio_codec_sample_rate_get(format_info->type,
                                                          &format_info->attr);
            adc_sample_rate = dac_sample_rate;
            if (format_info->type == AUDIO_FORMAT_TYPE_CVSD)
            {
                dac_sample_rate = 16000;
                adc_sample_rate = 16000;
            }
        }
        break;

    case AUDIO_CATEGORY_RECORD:
        {
            T_AUDIO_FORMAT_INFO *format_info = param.record.encoder_info;

            path->device    = param.record.device;
            path->data_mode = param.record.mode;
            encoder_info    = param.record.encoder_info;
            adc_gain_level  = param.record.adc_level;
            adc_sample_rate = audio_codec_sample_rate_get(format_info->type,
                                                          &format_info->attr);

            if (format_info->type == AUDIO_FORMAT_TYPE_CVSD)
            {
                adc_sample_rate = 16000;
            }
        }
        break;

    case AUDIO_CATEGORY_LINE:
        {
            path->device     = param.line.device;
            dac_sample_rate  = param.line.dac_sample_rate;
            adc_sample_rate  = param.line.adc_sample_rate;
            dac_gain_level   = param.line.dac_level;
            adc_gain_level   = param.line.adc_level;
        }
        break;

    case AUDIO_CATEGORY_TONE:
        {
            path->device     = param.ringtone.device;
            decoder_info     = (T_AUDIO_FORMAT_INFO *)param.ringtone.instance;
            dac_sample_rate  = 16000;
            dac_gain_level   = param.ringtone.dac_level;
        }
        break;

    case AUDIO_CATEGORY_VP:
        {
            T_AUDIO_FORMAT_INFO *format_info = param.vp.decoder_info;

            path->device    = param.vp.device;
            decoder_info    = param.vp.decoder_info;
            dac_gain_level  = param.vp.dac_level;
            dac_sample_rate = audio_codec_sample_rate_get(format_info->type,
                                                          &format_info->attr);

            if (format_info->type == AUDIO_FORMAT_TYPE_CVSD)
            {
                dac_sample_rate = 16000;
            }
        }
        break;

    case AUDIO_CATEGORY_APT:
        {
            path->device    = param.apt.device;
            dac_sample_rate = param.apt.dac_sample_rate;
            adc_sample_rate = param.apt.adc_sample_rate;
            dac_gain_level  = param.apt.dac_level;
            adc_gain_level  = param.apt.adc_level;
        }
        break;

    case AUDIO_CATEGORY_LLAPT:
        {
            path->device   = param.llapt.device;
            dac_gain_level = param.llapt.dac_level;
            adc_gain_level = param.llapt.adc_level;
        }
        break;

    case AUDIO_CATEGORY_ANC:
        {
            path->device   = param.anc.device;
            dac_gain_level = param.anc.dac_level;
            adc_gain_level = param.anc.adc_level;
        }
        break;

    case AUDIO_CATEGORY_VAD:
        {
            T_AUDIO_FORMAT_INFO *vad_format_info = param.vad.encoder_info;

            path->device         = param.vad.device;
            path->param.vad.type = param.vad.type;
            path->data_mode      = param.vad.mode;
            encoder_info         = param.vad.encoder_info;
            adc_gain_level       = param.anc.adc_level;
            aggressiveness_level = param.vad.aggressiveness_level;
            adc_sample_rate      = audio_codec_sample_rate_get(vad_format_info->type,
                                                               &vad_format_info->attr);

            if (vad_format_info->type == AUDIO_FORMAT_TYPE_CVSD)
            {
                adc_sample_rate = 16000;
            }

            if (param.vad.type != VAD_TYPE_SW)
            {
                vad_frame_size = audio_codec_frame_size_get(vad_format_info->type,
                                                            &vad_format_info->attr);
            }
        }
        break;

    case AUDIO_CATEGORY_KWS:
        {
            path->data_mode = param.kws.mode;
            decoder_info    = param.kws.decoder_info;
        }
        break;

    case AUDIO_CATEGORY_PIPE:
        {
            path->data_mode = param.pipe.mode;
            decoder_info    = param.pipe.decoder_info;
            encoder_info    = param.pipe.encoder_info;
        }
        break;

    default:
        {
            goto fail_invalid_category;
        }
    }

    domain_mask = audio_path_domain_mask_get(path);
    if (domain_mask & AUDIO_PATH_DOMAIN_CODEC)
    {
        path->codec_session = codec_mgr_session_create(path->category,
                                                       dac_sample_rate,
                                                       adc_sample_rate,
                                                       dac_gain_level,
                                                       adc_gain_level,
                                                       path->device,
                                                       path);
        if (path->codec_session == NULL)
        {
            goto fail_create_codec_session;
        }
    }

    if (domain_mask & AUDIO_PATH_DOMAIN_GATEWAY)
    {
        path->gateway_session = gateway_session_create(path->category,
                                                       dac_sample_rate,
                                                       adc_sample_rate,
                                                       path->device,
                                                       param.vad.type,
                                                       vad_frame_size,
                                                       audio_path_gateway_cback,
                                                       path);
        if (path->gateway_session == NULL)
        {
            goto fail_create_gateway_session;
        }
    }

    if (domain_mask & AUDIO_PATH_DOMAIN_DSP)
    {
        path->dsp_session = dsp_mgr_session_create(path->category,
                                                   path->data_mode,
                                                   dac_sample_rate,
                                                   adc_sample_rate,
                                                   decoder_info,
                                                   encoder_info,
                                                   audio_path_dsp_session_cback,
                                                   path->device,
                                                   path->param.vad.type,
                                                   aggressiveness_level,
                                                   path);
        if (path->dsp_session == NULL)
        {
            goto fail_create_dsp_session;
        }
    }

    if (domain_mask & AUDIO_PATH_DOMAIN_VAD)
    {
        path->vad_session = vad_mgr_session_create(path->param.vad.type,
                                                   adc_sample_rate,
                                                   audio_path_vad_cback,
                                                   path);
        if (path->vad_session == NULL)
        {
            goto fail_create_vad_session;
        }
    }

    return true;

fail_create_vad_session:
    if (path->dsp_session != NULL)
    {
        dsp_mgr_session_destroy(path->dsp_session);
    }
fail_create_dsp_session:
    if (path->gateway_session != NULL)
    {
        gateway_session_destroy(path->gateway_session);
    }
fail_create_gateway_session:
    if (path->codec_session != NULL)
    {
        codec_mgr_session_destroy(path->codec_session);
    }
fail_create_codec_session:
fail_invalid_category:
    return false;
}

T_AUDIO_PATH_HANDLE audio_path_create(T_AUDIO_CATEGORY   category,
                                      T_AUDIO_PATH_PARAM param,
                                      P_AUDIO_PATH_CBACK cback)
{
    T_AUDIO_PATH     *path;
    T_AUDIO_PATH_MSG  msg;
    int32_t           ret = 0;

    path = calloc(1, sizeof(T_AUDIO_PATH));
    if (path == NULL)
    {
        ret = 1;
        goto fail_alloc_path;
    }

    path->category    = category;
    path->cback       = cback;
    path->state       = AUDIO_PATH_STATE_IDLE;
    path->event_group = sys_event_group_create(0);

    if (audio_path_build(path,
                         param) == false)
    {
        ret = 2;
        goto fail_build_path;
    }

    os_queue_in(&audio_path_db.path_list, &path->state_list);

    msg.type     = AUDIO_PATH_MSG_TYPE_CREATE;
    msg.occasion = 0;
    msg.handle   = (T_AUDIO_PATH_HANDLE)path;
    msg.cback    = path->cback;
    audio_path_msg_send(&msg);
    return path;

fail_build_path:
    free(path);
fail_alloc_path:
    AUDIO_PRINT_ERROR3("audio_path_create: category %u, cback %p, ret %d",
                       category, cback, -ret);
    return NULL;
}

bool audio_path_destroy(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_PATH     *path;
    T_AUDIO_PATH_MSG  msg;
    int32_t           ret = 0;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (path->state != AUDIO_PATH_STATE_IDLE)
    {
        ret = 2;
        goto fail_check_state;
    }

    os_queue_delete(&audio_path_db.path_list, path);

    if (path->codec_session != NULL)
    {
        codec_mgr_session_destroy(path->codec_session);
    }

    if (path->gateway_session != NULL)
    {
        gateway_session_destroy(path->gateway_session);
    }

    if (path->vad_session != NULL)
    {
        vad_mgr_session_destroy(path->vad_session);
    }

    if (path->dsp_session != NULL)
    {
        dsp_mgr_session_destroy(path->dsp_session);
    }

    sys_event_group_delete(path->event_group);
    audio_route_path_unbind(path);

    msg.type     = AUDIO_PATH_MSG_TYPE_DESTROY;
    msg.occasion = 0;
    msg.handle   = (T_AUDIO_PATH_HANDLE)path;
    msg.cback    = path->cback;
    audio_path_msg_send(&msg);

    free(path);
    return true;

fail_check_state:
fail_check_handle:
    AUDIO_PRINT_ERROR1("audio_path_destroy: failed %d", -ret);
    return false;
}

bool audio_path_start(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_PATH *path;
    path = (T_AUDIO_PATH *)handle;

    if (path != NULL)
    {
        AUDIO_PRINT_INFO2("audio_path_start: handle %p, state 0x%02x", handle, path->state);

        if ((path->state == AUDIO_PATH_STATE_PENDING) ||
            (path->state == AUDIO_PATH_STATE_READY) ||
            (path->state == AUDIO_PATH_STATE_SUSPENDING))
        {
            return true;
        }
        else if (path->state == AUDIO_PATH_STATE_IDLE)
        {
            audio_path_state_set(path, AUDIO_PATH_STATE_PENDING);
            audio_path_schedule();
            return true;
        }
    }

    return false;
}

void audio_path_ready_stop_cback(void *handle)
{
    audio_path_stop(handle);
}

bool audio_path_stop(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_PATH *path;
    path = (T_AUDIO_PATH *)handle;

    if (path != NULL)
    {
        AUDIO_PRINT_INFO2("audio_path_stop: handle %p, state 0x%02x", handle, path->state);

        if (path->state == AUDIO_PATH_STATE_IDLE)
        {
            // Upper layers may need the AUDIO_PATH_EVT_IDLE event when they call audio_path_stop
            audio_path_state_set(path, AUDIO_PATH_STATE_IDLE);
        }
        ///TODO: The path to stop should only concern its own state.
        /// Currently the path to stop should wait all paths in ready state
        else if (audio_path_starting_check() == true)
        {
            sys_event_flag_wait(audio_path_db.event_group,
                                audio_path_ready_stop_cback,
                                (void *)path,
                                AUDIO_PATH_EVENT_FLAG_PATH_STARTING,
                                SYS_EVENT_FLAG_TYPE_CLEAR_AND);
        }
        else if (path->state == AUDIO_PATH_STATE_PENDING)
        {
            audio_path_state_set(path, AUDIO_PATH_STATE_IDLE);
        }
        else if (path->state == AUDIO_PATH_STATE_SUSPENDING)
        {
            audio_path_state_set(path, AUDIO_PATH_STATE_STOPPING);
        }
        else if (path->state == AUDIO_PATH_STATE_RUNNING)
        {
            audio_path_state_set(path, AUDIO_PATH_STATE_STOPPING);

            if ((path->category == AUDIO_CATEGORY_LLAPT) || (path->category == AUDIO_CATEGORY_ANC))
            {
                if (!sys_event_flag_check(audio_path_db.event_group, AUDIO_PATH_EVENT_FLAG_ANC_LLAPT_DISABLING))
                {
                    sys_event_flag_post(audio_path_db.event_group, SYS_EVENT_FLAG_OPT_SET,
                                        AUDIO_PATH_EVENT_FLAG_ANC_LLAPT_DISABLING);

                    sys_event_flag_wait(audio_path_db.event_group,
                                        anc_llapt_wait_disable_finish_cback,
                                        (void *)path,
                                        AUDIO_PATH_EVENT_FLAG_ANC_LLAPT_DISABLING,
                                        SYS_EVENT_FLAG_TYPE_CLEAR_AND);
#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
                    anc_mgr_disable();
#endif
                }
            }
            else if ((path->category == AUDIO_CATEGORY_VAD) &&
                     (path->param.vad.type == VAD_TYPE_HW))
            {
                T_AUDIO_PLUGIN_PARAM plugin_param;

                plugin_param.dac_sample_rate = 16000;
                plugin_param.adc_sample_rate = 16000;

                if (path->gateway_session != NULL)
                {
                    gateway_session_disable(path->gateway_session);
                }

                audio_plugin_occasion_occur((T_AUDIO_PATH_HANDLE)path,
                                            path->category,
                                            AUDIO_PLUGIN_OCCASION_DIGITAL_DOMAIN_OFF,
                                            plugin_param);
            }
            else
            {
                audio_path_ramp_gain_up(path);

                if (path->dsp_session != NULL)
                {
                    dsp_mgr_session_disable(path->dsp_session);
                }
            }
        }

        return true;
    }

    return false;
}

bool audio_path_dac_level_set(T_AUDIO_PATH_HANDLE handle,
                              uint8_t             level,
                              float               scale)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;

    if (path == NULL)
    {
        return false;
    }

    if (path->state == AUDIO_PATH_STATE_RUNNING)
    {
        T_AUDIO_ROUTE_DAC_GAIN route_gain;
        int16_t                left_gain;
        int16_t                right_gain;

        if (audio_route_dac_gain_get(path->category, level, &route_gain) == true)
        {
            if (scale == 0.0f)
            {
                left_gain  = route_gain.left_gain;
                right_gain = route_gain.right_gain;
            }
            else if (scale > 0.0f)
            {
                left_gain  = audio_path_gain_scale(route_gain.left_gain, 1.0f - scale);
                right_gain = route_gain.right_gain;
            }
            else
            {
                left_gain  = route_gain.left_gain;
                right_gain = audio_path_gain_scale(route_gain.right_gain, 1.0f + scale);
            }

            AUDIO_PRINT_TRACE5("audio_path_dac_level_set: category %u, level %u, scale %d/1000, left_gain 0x%04x, right_gain 0x%04x",
                               path->category, level, (int32_t)(scale * 1000), left_gain, right_gain);

            audio_path_dac_gain_set(path, left_gain, right_gain);
        }

        if (path->codec_session != NULL)
        {
            codec_mgr_session_dac_gain_set(path->codec_session);
        }
    }

    return true;
}

bool audio_path_dac_mute(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;

    if (path == NULL)
    {
        return false;
    }

    if (path->state == AUDIO_PATH_STATE_RUNNING)
    {
        audio_path_dac_gain_set(path, -128 * 128, -128 * 128);
    }

    return true;
}

bool audio_path_adc_level_set(T_AUDIO_PATH_HANDLE handle,
                              uint8_t             level,
                              float               scale)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;

    if (path == NULL)
    {
        return false;
    }

    if (path->state == AUDIO_PATH_STATE_RUNNING)
    {
        T_AUDIO_ROUTE_ADC_GAIN route_gain;
        int16_t                left_gain;
        int16_t                right_gain;

        if (audio_route_adc_gain_get(path->category, level, &route_gain) == true)
        {
            if (scale == 0.0f)
            {
                left_gain  = route_gain.left_gain;
                right_gain = route_gain.right_gain;
            }
            else if (scale > 0.0f)
            {
                left_gain  = audio_path_gain_scale(route_gain.left_gain, 1.0f - scale);
                right_gain = route_gain.right_gain;
            }
            else
            {
                left_gain  = route_gain.left_gain;
                right_gain = audio_path_gain_scale(route_gain.right_gain, 1.0f + scale);
            }

            AUDIO_PRINT_TRACE5("audio_path_adc_level_set: category %u, level %u, scale %d/1000, left_gain 0x%04x, right_gain 0x%04x",
                               path->category, level, (int32_t)(scale * 1000), left_gain, right_gain);

            audio_path_adc_gain_set(path, left_gain, right_gain);
        }

        if (path->codec_session != NULL)
        {
            codec_mgr_session_adc_gain_set(path->codec_session);
        }
    }

    return true;
}

bool audio_path_adc_mute(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;

    if (path == NULL)
    {
        return false;
    }

    if (path->state == AUDIO_PATH_STATE_RUNNING)
    {
        audio_path_adc_gain_set(path, -128 * 128, -128 * 128);

        if (path->codec_session != NULL)
        {
            codec_mgr_session_adc_gain_mute(path->codec_session);
        }
    }

    return true;
}

bool audio_path_power_off(void)
{
    codec_mgr_power_off();
    dsp_mgr_power_off();

    return true;
}

bool audio_path_power_on(void)
{
    return true;
}

bool audio_path_data_send(T_AUDIO_PATH_HANDLE    handle,
                          T_AUDIO_PATH_DATA_HDR *buf,
                          uint32_t               len)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;

    if (path == NULL)
    {
        return false;
    }

    if (path->state != AUDIO_PATH_STATE_RUNNING)
    {
        return false;
    }

    if ((path->category == AUDIO_CATEGORY_AUDIO) ||
        (path->category == AUDIO_CATEGORY_VOICE) ||
        (path->category == AUDIO_CATEGORY_VP)    ||
        (path->category == AUDIO_CATEGORY_PIPE)  ||
        (path->category == AUDIO_CATEGORY_KWS))
    {
        buf->sync_word = 0x3F3F3F3F;
        buf->session_id = (uint32_t)path->dsp_session;
        buf->tail = 0xffffffff;

        return dsp_mgr_data_send(path->dsp_session, (uint8_t *)buf, len);
    }

    return false;
}

uint16_t audio_path_data_recv(T_AUDIO_PATH_HANDLE    handle,
                              T_AUDIO_PATH_DATA_HDR *buf,
                              uint16_t               len)
{
    T_AUDIO_PATH *path = (T_AUDIO_PATH *)handle;

    if (path != NULL)
    {
        if ((path->category == AUDIO_CATEGORY_VAD) &&
            (path->param.vad.type == VAD_TYPE_HW))
        {
            return gateway_session_data_recv(path->gateway_session, (uint8_t *)buf, len);
        }
        else
        {
            return dsp_mgr_data_recv(path->dsp_session, (uint8_t *)buf, len);
        }
    }

    return 0;
}

uint16_t audio_path_data_peek(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (path != NULL)
    {
        if ((path->category == AUDIO_CATEGORY_VAD) &&
            (path->param.vad.type == VAD_TYPE_HW))
        {
            return gateway_session_data_len_peek(path->gateway_session);
        }
        else
        {
            return dsp_mgr_data_len_peek(path->dsp_session);
        }
    }

    return 0;
}

bool audio_path_timestamp_set(T_AUDIO_PATH_HANDLE handle,
                              uint8_t             clk_ref,
                              uint32_t            timestamp,
                              bool                sync_flag)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;

    dsp_mgr_signal_proc_start(path->dsp_session, timestamp, clk_ref, sync_flag);

    return true;
}

bool audio_path_synchronization_role_swap(T_AUDIO_PATH_HANDLE handle,
                                          uint8_t             role,
                                          bool                start)
{
    return dsp_ipc_set_handover_info(role, start);
}

bool audio_path_synchronization_data_send(T_AUDIO_PATH_HANDLE  handle,
                                          uint8_t             *buf,
                                          uint16_t             len)
{
    return dsp_ipc_synchronization_data_send(buf, len);
}

void audio_path_b2bmsg_interaction_timeout(void)
{
    dsp_ipc_set_b2bmsg_interaction_timeout(0);
}

bool audio_path_synchronization_join_set(T_AUDIO_PATH_HANDLE handle,
                                         uint8_t             role)
{
    if (role == 1)// Pri for join
    {
        dsp_rws_set_role(role, RWS_ROLE_SRC);
        dsp_rws_seamless(RWS_RESYNC_V2_MASTER);
    }
    else if (role == 2)// Sec for join
    {
        dsp_rws_seamless(RWS_RESYNC_V2_SLAVE);
    }
    else
    {
        dsp_rws_set_role(role, RWS_ROLE_NONE);
        dsp_rws_seamless(RWS_RESYNC_V2_OFF);

    }

    return true;
}

bool audio_path_is_running(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_PATH *audio_path;

    audio_path = (T_AUDIO_PATH *)handle;

    if (audio_path != NULL)
    {
        if (audio_path->state == AUDIO_PATH_STATE_RUNNING)
        {
            return true;
        }
    }

    return false;
}

void audio_path_fifo_report(T_AUDIO_PATH_HANDLE handle,
                            uint16_t            interval)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;

    if (path->state == AUDIO_PATH_STATE_RUNNING)
    {
        dsp_mgr_decoder_fifo_report(path->dsp_session, interval);
    }
}

bool audio_path_decoder_effect_control(T_AUDIO_PATH_HANDLE handle,
                                       uint8_t             action)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_decoder_effect_control(path->dsp_session, action);
    }

    return false;
}

bool audio_path_encoder_effect_control(T_AUDIO_PATH_HANDLE handle,
                                       uint8_t             action)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_encoder_effect_control(path->dsp_session, action);
    }

    return false;
}

bool audio_path_sw_sidetone_enable(T_AUDIO_PATH_HANDLE handle,
                                   int16_t             gain_step)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_sidetone_set(path->dsp_session, gain_step);
    }

    return false;
}

bool audio_path_sw_sidetone_disable(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_sidetone_clear(path->dsp_session);
    }

    return false;
}

bool audio_path_hw_sidetone_enable(int16_t gain,
                                   uint8_t level)
{
    return codec_mgr_sidetone_set(gain, level, 1);
}

bool audio_path_hw_sidetone_disable(void)
{
    return codec_mgr_sidetone_set(0, 0, 0);
}

bool audio_path_configure(T_AUDIO_PATH_HANDLE  handle,
                          void                *cfg)
{
#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;

    switch (path->category)
    {
    case AUDIO_CATEGORY_ANC:
    case AUDIO_CATEGORY_LLAPT:
        {
            T_ANC_LLAPT_CFG *config;

            config = (T_ANC_LLAPT_CFG *)cfg;

            anc_mgr_load_cfg_set(config->sub_type, config->scenario_id);
        }
        break;

    default:
        break;
    }
#endif
    return true;
}

void audio_path_lpm_set(bool enable)
{
    codec_mgr_lpm_set(enable);
    dsp_mgr_lpm_set(enable);
}

void audio_path_brightness_set(T_AUDIO_PATH_HANDLE handle,
                               float               strength)
{
#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
    T_AUDIO_PATH *audio_path;

    audio_path = (T_AUDIO_PATH *)handle;
    if (audio_path)
    {
        if (audio_path->state == AUDIO_PATH_STATE_RUNNING)
        {
            anc_mgr_eq_set(strength);
        }
    }
#endif
}

void audio_path_msg_send(T_AUDIO_PATH_MSG *msg)
{
    uint8_t evt;

    evt = EVENT_AUDIO_PATH_MSG;
    if (os_msg_send(hAudioPathMsgQueueHandle, msg, 0) == true)
    {
        (void)os_msg_send(hEventQueueHandleAu, &evt, 0);
    }
}

bool audio_path_signal_monitoring_set(T_AUDIO_PATH_HANDLE           handle,
                                      T_AUDIO_PATH_SIGNAL_DIRECTION direction,
                                      bool                          enable,
                                      uint16_t                      refresh_interval,
                                      uint8_t                       freq_num,
                                      uint32_t                      freq_table[])
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_signal_monitoring_set(path->dsp_session,
                                             (uint8_t)direction,
                                             enable,
                                             refresh_interval,
                                             freq_num,
                                             freq_table);
    }

    return false;
}

bool audio_path_channel_out_reorder(T_AUDIO_PATH_HANDLE handle,
                                    uint8_t             channel_num,
                                    uint32_t            channel_array[])
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_channel_out_reorder(path->dsp_session,
                                           channel_num,
                                           channel_array);
    }

    return false;
}

bool audio_path_channel_in_reorder(T_AUDIO_PATH_HANDLE handle,
                                   uint8_t             channel_num,
                                   uint32_t            channel_array[])
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_channel_in_reorder(path->dsp_session,
                                          channel_num,
                                          channel_array);
    }

    return false;
}

bool audio_path_asrc_set(T_AUDIO_PATH_HANDLE handle,
                         uint8_t             toggle,
                         int32_t             ratio,
                         uint32_t            timestamp)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_asrc_set(path->dsp_session, toggle, ratio, timestamp);
    }

    return false;
}

bool audio_path_decoder_plc_set(T_AUDIO_PATH_HANDLE handle,
                                bool                enable,
                                uint16_t            interval,
                                uint32_t            threshold)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;

    if (path)
    {
        return dsp_mgr_decoder_plc_set(path->dsp_session, enable, interval, threshold);
    }

    return false;
}

bool audio_path_eq_out_set(T_AUDIO_PATH_HANDLE  handle,
                           uint8_t             *info_buf,
                           uint16_t             info_len)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_eq_out_set(path->dsp_session, info_buf, info_len);
    }

    return false;
}

bool audio_path_eq_in_set(T_AUDIO_PATH_HANDLE  handle,
                          uint8_t             *info_buf,
                          uint16_t             info_len)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_eq_in_set(path->dsp_session, info_buf, info_len);
    }

    return false;
}

bool audio_path_eq_out_clear(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_eq_out_clear(path->dsp_session);
    }

    return false;
}

bool audio_path_eq_in_clear(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_eq_in_clear(path->dsp_session);
    }

    return false;
}

bool audio_path_nrec_set(T_AUDIO_PATH_HANDLE handle,
                         uint8_t             mode,
                         uint8_t             level)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_nrec_set(path->dsp_session, mode, level);
    }

    return false;
}

bool audio_path_nrec_clear(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_nrec_clear(path->dsp_session);
    }

    return false;
}

bool audio_path_ovp_set(T_AUDIO_PATH_HANDLE handle,
                        uint8_t             level)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_ovp_set(path->dsp_session, level);
    }

    return false;
}

bool audio_path_ovp_clear(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_ovp_clear(path->dsp_session);
    }

    return false;
}

bool audio_path_beamforming_set(T_AUDIO_PATH_HANDLE handle,
                                uint8_t             direction)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_beamforming_set(path->dsp_session, direction);
    }

    return false;
}

bool audio_path_beamforming_clear(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_beamforming_clear(path->dsp_session);
    }

    return false;
}

bool audio_path_kws_set(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_kws_set(path->dsp_session);
    }

    return false;
}

bool audio_path_kws_clear(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_kws_clear(path->dsp_session);
    }

    return false;
}

bool audio_path_vad_filter(T_AUDIO_PATH_HANDLE handle,
                           uint8_t             level)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_vad_filter(path->dsp_session, level);
    }

    return false;
}

bool audio_path_wdrc_set(T_AUDIO_PATH_HANDLE  handle,
                         uint8_t             *info_buf,
                         uint16_t             info_len)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_wdrc_set(path->dsp_session, info_buf, info_len);
    }

    return false;
}

bool audio_path_wdrc_clear(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_wdrc_clear(path->dsp_session);
    }

    return false;
}

bool audio_path_vse_req(T_AUDIO_PATH_HANDLE  handle,
                        uint16_t             company_id,
                        uint16_t             effect_id,
                        uint16_t             seq_num,
                        uint8_t             *info_buf,
                        uint16_t             info_len)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_vse_req(path->dsp_session,
                               company_id,
                               effect_id,
                               seq_num,
                               info_buf,
                               info_len);
    }

    return false;
}

bool audio_path_vse_cfm(T_AUDIO_PATH_HANDLE  handle,
                        uint16_t             company_id,
                        uint16_t             effect_id,
                        uint16_t             seq_num,
                        uint8_t             *info_buf,
                        uint16_t             info_len)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        return dsp_mgr_vse_cfm(path->dsp_session,
                               company_id,
                               effect_id,
                               seq_num,
                               info_buf,
                               info_len);
    }

    return false;
}

bool audio_path_pipe_gain_set(T_AUDIO_PATH_HANDLE handle,
                              int16_t             gain_left,
                              int16_t             gain_right)
{
    T_AUDIO_PATH *path;

    path = (T_AUDIO_PATH *)handle;
    if (os_queue_search(&audio_path_db.path_list, path))
    {
        audio_path_dac_gain_set(path, gain_left, gain_right);
        return true;
    }

    return false;
}

bool audio_path_pipe_pre_mix(T_AUDIO_PATH_HANDLE prime_handle,
                             T_AUDIO_PATH_HANDLE auxiliary_handle)
{
    T_AUDIO_PATH *prime_path;
    T_AUDIO_PATH *auxiliary_path;

    prime_path = (T_AUDIO_PATH *)prime_handle;
    auxiliary_path = (T_AUDIO_PATH *)auxiliary_handle;
    if (os_queue_search(&audio_path_db.path_list, prime_path) &&
        os_queue_search(&audio_path_db.path_list, auxiliary_path))
    {
        return dipc_codec_pipe_pre_mixer_add((uint32_t)prime_path->dsp_session,
                                             (uint32_t)auxiliary_path->dsp_session);
    }

    return false;
}

bool audio_path_pipe_post_mix(T_AUDIO_PATH_HANDLE prime_handle,
                              T_AUDIO_PATH_HANDLE auxiliary_handle)
{
    T_AUDIO_PATH *prime_path;
    T_AUDIO_PATH *auxiliary_path;

    prime_path = (T_AUDIO_PATH *)prime_handle;
    auxiliary_path = (T_AUDIO_PATH *)auxiliary_handle;
    if (os_queue_search(&audio_path_db.path_list, prime_path) &&
        os_queue_search(&audio_path_db.path_list, auxiliary_path))
    {
        return dipc_codec_pipe_post_mixer_add((uint32_t)prime_path->dsp_session,
                                              (uint32_t)auxiliary_path->dsp_session);
    }

    return false;
}

bool audio_path_pipe_demix(T_AUDIO_PATH_HANDLE prime_handle,
                           T_AUDIO_PATH_HANDLE auxiliary_handle)
{
    T_AUDIO_PATH *prime_path;
    T_AUDIO_PATH *auxiliary_path;

    prime_path = (T_AUDIO_PATH *)prime_handle;
    auxiliary_path = (T_AUDIO_PATH *)auxiliary_handle;
    if (os_queue_search(&audio_path_db.path_list, prime_path) &&
        os_queue_search(&audio_path_db.path_list, auxiliary_path))
    {
        return dipc_codec_pipe_mixer_remove((uint32_t)prime_path->dsp_session,
                                            (uint32_t)auxiliary_path->dsp_session);
    }

    return false;
}

