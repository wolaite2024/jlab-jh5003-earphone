/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "trace.h"
#include "audio_route.h"
#include "audio_mgr.h"
#include "audio_effect.h"
#include "audio_path.h"
#include "audio_passthrough.h"

/* TODO Remove Start */
#include "anc_mgr.h"
/* TODO Remove End */

#define AUDIO_PASSTHROUGH_DAC_LEVEL_MIN     (0)
#define AUDIO_PASSTHROUGH_DAC_LEVEL_MAX     (15)
#define AUDIO_PASSTHROUGH_DAC_LEVEL_DEFAULT (10)

#define AUDIO_PASSTHROUGH_ADC_LEVEL_MIN     (0)
#define AUDIO_PASSTHROUGH_ADC_LEVEL_MAX     (15)
#define AUDIO_PASSTHROUGH_ADC_LEVEL_DEFAULT (10)

#define AUDIO_PASSTHROUGH_BRIGHTNESS_STRENGTH_MIN     (0.0f)
#define AUDIO_PASSTHROUGH_BRIGHTNESS_STRENGTH_MAX     (1.0f)
#define AUDIO_PASSTHROUGH_BRIGHTNESS_STRENGTH_DEFAULT (0.5f)

#define AUDIO_PASSTHROUGH_OVP_LEVEL_MIN     (0)
#define AUDIO_PASSTHROUGH_OVP_LEVEL_MAX     (15)
#define AUDIO_PASSTHROUGH_OVP_LEVEL_DEFAULT (7)

typedef enum t_audio_passthrough_session_state
{
    AUDIO_PASSTHROUGH_SESSION_STATE_RELEASED,
    AUDIO_PASSTHROUGH_SESSION_STATE_CREATED,
    AUDIO_PASSTHROUGH_SESSION_STATE_STARTING,
    AUDIO_PASSTHROUGH_SESSION_STATE_STARTED,
    AUDIO_PASSTHROUGH_SESSION_STATE_STOPPING,
    AUDIO_PASSTHROUGH_SESSION_STATE_STOPPED,
    AUDIO_PASSTHROUGH_SESSION_STATE_RELEASING,
} T_AUDIO_PASSTHROUGH_SESSION_STATE;

typedef enum t_audio_passthrough_session_action
{
    AUDIO_PASSTHROUGH_SESSION_ACTION_NONE     = 0x00,
    AUDIO_PASSTHROUGH_SESSION_ACTION_START    = 0x01,
    AUDIO_PASSTHROUGH_SESSION_ACTION_STOP     = 0x02,
    AUDIO_PASSTHROUGH_SESSION_ACTION_RELEASE  = 0x03,
} T_AUDIO_PASSTHROUGH_SESSION_ACTION;

typedef struct t_audio_passthrough_session
{
    T_AUDIO_PATH_HANDLE                path_handle;
    T_AUDIO_PASSTHROUGH_SESSION_STATE  state;
    T_AUDIO_PASSTHROUGH_SESSION_ACTION action;
} T_AUDIO_PASSTHROUGH_SESSION;

typedef struct t_audio_passthrough_db
{
    T_AUDIO_PASSTHROUGH_SESSION       *apt;
    T_AUDIO_PASSTHROUGH_SESSION       *llapt;
    T_AUDIO_PASSTHROUGH_MODE           mode;
    uint8_t                            llapt_scenario_id;
    uint8_t                            volume_out_level_min;
    uint8_t                            volume_out_level_max;
    bool                               volume_out_muted;
    uint8_t                            volume_out_level;
    float                              volume_out_scale;
    uint8_t                            volume_in_level_min;
    uint8_t                            volume_in_level_max;
    bool                               volume_in_muted;
    uint8_t                            volume_in_level;
    bool                               ovp_enable;
    uint8_t                            ovp_level;
    bool                               effect_apply;
    T_OS_QUEUE                         effects;
    float                              brightness_strength_min;
    float                              brightness_strength_max;
    float                              brightness_strength;
} T_AUDIO_PASSTHROUGH_DB;

static T_AUDIO_PASSTHROUGH_DB audio_passthrough_db;

static bool audio_passthrough_effect_run(void);
static bool audio_passthrough_effect_stop(void);
static bool audio_passthrough_effect_clear(void);

static void audio_passthrough_volume_apply(T_AUDIO_PASSTHROUGH_SESSION *session)
{
    if (audio_passthrough_db.volume_out_muted == false)
    {
        audio_path_dac_level_set(session->path_handle,
                                 audio_passthrough_db.volume_out_level,
                                 audio_passthrough_db.volume_out_scale);
    }
    else
    {
        audio_path_dac_mute(session->path_handle);
    }

    if (audio_passthrough_db.mode == AUDIO_PASSTHROUGH_MODE_NORMAL)
    {
        if (audio_passthrough_db.volume_in_muted == false)
        {
            audio_path_adc_level_set(session->path_handle,
                                     audio_passthrough_db.volume_in_level,
                                     0.0f);
        }
        else
        {
            audio_path_adc_mute(session->path_handle);
        }
    }
}

static bool audio_passthrough_session_state_set(T_AUDIO_PASSTHROUGH_SESSION       *session,
                                                T_AUDIO_PASSTHROUGH_SESSION_STATE  state)
{
    bool ret = false;

    AUDIO_PRINT_INFO4("audio_passthrough_session_state_set: session %p, curr state %u, next state %u, action %u",
                      session, session->state, state, session->action);

    switch (session->state)
    {
    case AUDIO_PASSTHROUGH_SESSION_STATE_CREATED:
        if (state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTING)
        {
            T_ANC_LLAPT_CFG config;

            session->state = state;

            config.sub_type = ANC_IMAGE_SUB_TYPE_APT_COEF;
            config.scenario_id = audio_passthrough_db.llapt_scenario_id;
            audio_path_configure(session->path_handle, &config);

            ret = audio_path_start(session->path_handle);
            if (ret == false)
            {
                session->state = AUDIO_PASSTHROUGH_SESSION_STATE_CREATED;
            }
        }
        else if (state == AUDIO_PASSTHROUGH_SESSION_STATE_RELEASING)
        {
            session->state = state;

            ret = audio_path_destroy(session->path_handle);
            if (ret == false)
            {
                session->state = AUDIO_PASSTHROUGH_SESSION_STATE_CREATED;
            }
        }
        break;

    case AUDIO_PASSTHROUGH_SESSION_STATE_STARTING:
        if (state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTING)
        {
            if (session->action == AUDIO_PASSTHROUGH_SESSION_ACTION_NONE)
            {
                /* Drop the repeating path start action. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (session->action == AUDIO_PASSTHROUGH_SESSION_ACTION_STOP)
            {
                /* Cancel all pending actions when the starting path is pending
                 * for stop action, but receives the start action later.
                 */
                session->action = AUDIO_PASSTHROUGH_SESSION_ACTION_NONE;
                ret = true;
            }
        }
        else if (state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
        {
            T_AUDIO_MSG_PAYLOAD_PASSTHROUGH_ENABLE payload;

            session->state = state;

            payload.mode = audio_passthrough_db.mode;

            audio_passthrough_volume_apply(session);

            if (audio_passthrough_db.mode == AUDIO_PASSTHROUGH_MODE_NORMAL)
            {
                audio_passthrough_db.effect_apply = false;

                if (audio_passthrough_db.ovp_enable == true)
                {
                    audio_path_ovp_set(session->path_handle, audio_passthrough_db.ovp_level);
                }
            }
            else if (audio_passthrough_db.mode == AUDIO_PASSTHROUGH_MODE_LOW_LATENCY)
            {
                audio_path_brightness_set(session->path_handle,
                                          audio_passthrough_db.brightness_strength);
            }

            audio_mgr_dispatch(AUDIO_MSG_PASSTHROUGH_ENABLE, &payload);

            if (session->action == AUDIO_PASSTHROUGH_SESSION_ACTION_NONE)
            {
                ret = true;
            }
            else if (session->action == AUDIO_PASSTHROUGH_SESSION_ACTION_STOP)
            {
                session->action = AUDIO_PASSTHROUGH_SESSION_ACTION_NONE;
                ret = audio_passthrough_session_state_set(session, AUDIO_PASSTHROUGH_SESSION_STATE_STOPPING);
            }
            else if (session->action == AUDIO_PASSTHROUGH_SESSION_ACTION_RELEASE)
            {
                session->action = AUDIO_PASSTHROUGH_SESSION_ACTION_NONE;
                ret = audio_passthrough_session_state_set(session, AUDIO_PASSTHROUGH_SESSION_STATE_RELEASING);
            }
        }
        else if (state == AUDIO_PASSTHROUGH_SESSION_STATE_STOPPING)
        {
            if (session->action == AUDIO_PASSTHROUGH_SESSION_ACTION_NONE)
            {
                session->action = AUDIO_PASSTHROUGH_SESSION_ACTION_STOP;
                ret = true;
            }
            else if (session->action == AUDIO_PASSTHROUGH_SESSION_ACTION_STOP)
            {
                /* Drop the repeating stop action. But this case is permitted. */
                ret = true;
            }
        }
        else if (state == AUDIO_PASSTHROUGH_SESSION_STATE_RELEASING)
        {
            session->action = AUDIO_PASSTHROUGH_SESSION_ACTION_RELEASE;
            ret = true;
        }
        break;

    case AUDIO_PASSTHROUGH_SESSION_STATE_STARTED:
        if (state == AUDIO_PASSTHROUGH_SESSION_STATE_STOPPING)
        {
            session->state = state;
            audio_passthrough_effect_stop();

            ret = audio_path_stop(session->path_handle);
            if (ret == false)
            {
                session->state = AUDIO_PASSTHROUGH_SESSION_STATE_STARTED;
                audio_passthrough_effect_run();
            }
        }
        else if (state == AUDIO_PASSTHROUGH_SESSION_STATE_RELEASING)
        {
            /* Started path cannot be released directly, so it should be
             * stopped first before released.
             */
            session->state  = AUDIO_PASSTHROUGH_SESSION_STATE_STOPPING;
            session->action = AUDIO_PASSTHROUGH_SESSION_ACTION_RELEASE;
            audio_passthrough_effect_stop();

            ret = audio_path_stop(session->path_handle);
            if (ret == false)
            {
                session->state  = AUDIO_PASSTHROUGH_SESSION_STATE_STARTED;
                session->action = AUDIO_PASSTHROUGH_SESSION_ACTION_NONE;
                audio_passthrough_effect_run();
            }
        }
        break;

    case AUDIO_PASSTHROUGH_SESSION_STATE_STOPPING:
        if (state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTING)
        {
            if (session->action == AUDIO_PASSTHROUGH_SESSION_ACTION_NONE)
            {
                session->action = AUDIO_PASSTHROUGH_SESSION_ACTION_START;
                ret = true;
            }
            else if (session->action == AUDIO_PASSTHROUGH_SESSION_ACTION_START)
            {
                /* Drop the repeating start action. But this case is permitted. */
                ret = true;
            }
        }
        else if (state == AUDIO_PASSTHROUGH_SESSION_STATE_STOPPING)
        {
            if (session->action == AUDIO_PASSTHROUGH_SESSION_ACTION_NONE)
            {
                /* Drop the repeating stop action. But this case is permitted. */
                ret = true;
            }
            else if (session->action == AUDIO_PASSTHROUGH_SESSION_ACTION_START)
            {
                /* Cancel all pending actions when the stopping path is pending
                 * for start action, but receives the stop action later.
                 */
                session->action = AUDIO_PASSTHROUGH_SESSION_ACTION_NONE;
                ret = true;
            }
        }
        else if (state == AUDIO_PASSTHROUGH_SESSION_STATE_STOPPED)
        {
            T_AUDIO_MSG_PAYLOAD_PASSTHROUGH_DISABLE payload;

            session->state = state;

            payload.mode = audio_passthrough_db.mode;
            audio_mgr_dispatch(AUDIO_MSG_PASSTHROUGH_DISABLE, &payload);

            if (session->action == AUDIO_PASSTHROUGH_SESSION_ACTION_NONE)
            {
                ret = true;
            }
            else if (session->action == AUDIO_PASSTHROUGH_SESSION_ACTION_START)
            {
                session->action = AUDIO_PASSTHROUGH_SESSION_ACTION_NONE;
                ret = audio_passthrough_session_state_set(session, AUDIO_PASSTHROUGH_SESSION_STATE_STARTING);
            }
            else if (session->action == AUDIO_PASSTHROUGH_SESSION_ACTION_RELEASE)
            {
                session->action = AUDIO_PASSTHROUGH_SESSION_ACTION_NONE;
                ret = audio_passthrough_session_state_set(session, AUDIO_PASSTHROUGH_SESSION_STATE_RELEASING);
            }
        }
        else if (state == AUDIO_PASSTHROUGH_SESSION_STATE_RELEASING)
        {
            session->action = AUDIO_PASSTHROUGH_SESSION_ACTION_RELEASE;
            ret = true;
        }
        break;

    case AUDIO_PASSTHROUGH_SESSION_STATE_STOPPED:
        if (state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTING)
        {
            T_ANC_LLAPT_CFG config;

            session->state = state;

            config.sub_type = ANC_IMAGE_SUB_TYPE_APT_COEF;
            config.scenario_id = audio_passthrough_db.llapt_scenario_id;
            audio_path_configure(session->path_handle, &config);

            ret = audio_path_start(session->path_handle);
            if (ret == false)
            {
                session->state = AUDIO_PASSTHROUGH_SESSION_STATE_STOPPED;
            }
        }
        else if (state == AUDIO_PASSTHROUGH_SESSION_STATE_RELEASING)
        {
            session->state = state;

            ret = audio_path_destroy(session->path_handle);
            if (ret == false)
            {
                session->state = AUDIO_PASSTHROUGH_SESSION_STATE_STOPPED;
            }
        }
        break;

    case AUDIO_PASSTHROUGH_SESSION_STATE_RELEASING:
        if (state == AUDIO_PASSTHROUGH_SESSION_STATE_RELEASED)
        {
            session->state  = state;
            session->action = AUDIO_PASSTHROUGH_SESSION_ACTION_NONE;
            session->path_handle = NULL;

            audio_passthrough_effect_clear();

            if (session == audio_passthrough_db.apt)
            {
                audio_passthrough_db.apt = NULL;
            }
            else if (session == audio_passthrough_db.llapt)
            {
                audio_passthrough_db.llapt = NULL;
            }
            free(session);

            ret = true;
        }
        else if (state == AUDIO_PASSTHROUGH_SESSION_STATE_RELEASING)
        {
            /* Drop the repeating release action. But this case is permitted. */
            ret = true;
        }
        break;

    default:
        break;
    }

    return ret;
}

static void audio_passthrough_effect_response(uint16_t  company_id,
                                              uint16_t  effect_id,
                                              uint16_t  seq_num,
                                              uint16_t  payload_len,
                                              uint8_t  *payload)
{
    T_AUDIO_EFFECT_INSTANCE instance;
    int32_t                 i = 0;

    while ((instance = os_queue_peek(&audio_passthrough_db.effects, i)) != NULL)
    {
        audio_effect_response(instance,
                              company_id,
                              effect_id,
                              seq_num,
                              payload_len,
                              payload);
        i++;
    }
}

static void audio_passthrough_effect_indicate(uint16_t  company_id,
                                              uint16_t  effect_id,
                                              uint16_t  seq_num,
                                              uint16_t  payload_len,
                                              uint8_t  *payload)
{
    T_AUDIO_EFFECT_INSTANCE instance;
    int32_t                 i = 0;

    while ((instance = os_queue_peek(&audio_passthrough_db.effects, i)) != NULL)
    {
        audio_effect_indicate(instance,
                              company_id,
                              effect_id,
                              seq_num,
                              payload_len,
                              payload);
        i++;
    }
}

static void audio_passthrough_effect_notify(uint16_t  company_id,
                                            uint16_t  effect_id,
                                            uint16_t  seq_num,
                                            uint16_t  payload_len,
                                            uint8_t  *payload)
{
    T_AUDIO_EFFECT_INSTANCE instance;
    int32_t                 i = 0;

    while ((instance = os_queue_peek(&audio_passthrough_db.effects, i)) != NULL)
    {
        audio_effect_notify(instance,
                            company_id,
                            effect_id,
                            seq_num,
                            payload_len,
                            payload);
        i++;
    }
}

static bool audio_passthrough_path_cback(T_AUDIO_PATH_HANDLE handle,
                                         T_AUDIO_PATH_EVENT  event,
                                         uint32_t            param)
{
    T_AUDIO_PASSTHROUGH_SESSION *session = NULL;
    bool                         ret     = false;

    AUDIO_PRINT_TRACE3("audio_passthrough_path_cback: handle %p, event 0x%02x, param 0x%08x",
                       handle, event, param);

    if (audio_passthrough_db.apt != NULL)
    {
        if (audio_passthrough_db.apt->path_handle == handle)
        {
            session = audio_passthrough_db.apt;
        }
    }

    if (audio_passthrough_db.llapt != NULL)
    {
        if (audio_passthrough_db.llapt->path_handle == handle)
        {
            session = audio_passthrough_db.llapt;
        }
    }

    if (session != NULL)
    {
        switch (event)
        {
        case AUDIO_PATH_EVT_RELEASE:
            {
                ret = audio_passthrough_session_state_set(session, AUDIO_PASSTHROUGH_SESSION_STATE_RELEASED);
            }
            break;

        case AUDIO_PATH_EVT_RUNNING:
            {
                ret = audio_passthrough_session_state_set(session, AUDIO_PASSTHROUGH_SESSION_STATE_STARTED);
            }
            break;

        case AUDIO_PATH_EVT_IDLE:
            {
                ret = audio_passthrough_session_state_set(session, AUDIO_PASSTHROUGH_SESSION_STATE_STOPPED);
            }
            break;

        case AUDIO_PATH_EVT_EFFECT_REQ:
            {
                if (audio_passthrough_db.mode == AUDIO_PASSTHROUGH_MODE_NORMAL)
                {
                    audio_passthrough_db.effect_apply = true;
                    audio_passthrough_effect_run();
                    ret = true;
                }
            }
            break;

        case AUDIO_PATH_EVT_VSE_RESPONSE:
            {
                if (audio_passthrough_db.mode == AUDIO_PASSTHROUGH_MODE_NORMAL)
                {
                    T_AUDIO_PATH_VSE_RESPONSE *rsp = (T_AUDIO_PATH_VSE_RESPONSE *)param;

                    audio_passthrough_effect_response(rsp->company_id,
                                                      rsp->effect_id,
                                                      rsp->seq_num,
                                                      rsp->payload_len,
                                                      rsp->payload);
                    ret = true;
                }
            }
            break;

        case AUDIO_PATH_EVT_VSE_INDICATE:
            {
                if (audio_passthrough_db.mode == AUDIO_PASSTHROUGH_MODE_NORMAL)
                {
                    T_AUDIO_PATH_VSE_INDICATE *ind = (T_AUDIO_PATH_VSE_INDICATE *)param;

                    audio_passthrough_effect_indicate(ind->company_id,
                                                      ind->effect_id,
                                                      ind->seq_num,
                                                      ind->payload_len,
                                                      ind->payload);
                    ret = true;
                }
            }
            break;

        case AUDIO_PATH_EVT_VSE_NOTIFY:
            {
                if (audio_passthrough_db.mode == AUDIO_PASSTHROUGH_MODE_NORMAL)
                {
                    T_AUDIO_PATH_VSE_NOTIFY *notify = (T_AUDIO_PATH_VSE_NOTIFY *)param;

                    audio_passthrough_effect_notify(notify->company_id,
                                                    notify->effect_id,
                                                    notify->seq_num,
                                                    notify->payload_len,
                                                    notify->payload);
                    ret = true;
                }
            }
            break;

        default:
            break;
        }
    }

    return ret;
}

bool audio_passthrough_init(void)
{
    audio_passthrough_db.apt      = NULL;
    audio_passthrough_db.llapt    = NULL;
    audio_passthrough_db.mode     = AUDIO_PASSTHROUGH_MODE_NORMAL;
    audio_passthrough_db.volume_out_level_min = AUDIO_PASSTHROUGH_DAC_LEVEL_MIN;
    audio_passthrough_db.volume_out_level_max = AUDIO_PASSTHROUGH_DAC_LEVEL_MAX;
    audio_passthrough_db.volume_out_muted     = false;
    audio_passthrough_db.volume_out_level     = AUDIO_PASSTHROUGH_DAC_LEVEL_DEFAULT;
    audio_passthrough_db.volume_out_scale     = 0.0f;
    audio_passthrough_db.volume_in_level_min  = AUDIO_PASSTHROUGH_ADC_LEVEL_MIN;
    audio_passthrough_db.volume_in_level_max  = AUDIO_PASSTHROUGH_ADC_LEVEL_MAX;
    audio_passthrough_db.volume_in_muted      = false;
    audio_passthrough_db.volume_in_level      = AUDIO_PASSTHROUGH_ADC_LEVEL_DEFAULT;
    audio_passthrough_db.brightness_strength_min  = AUDIO_PASSTHROUGH_BRIGHTNESS_STRENGTH_MIN;
    audio_passthrough_db.brightness_strength_max  = AUDIO_PASSTHROUGH_BRIGHTNESS_STRENGTH_MAX;
    audio_passthrough_db.brightness_strength      = AUDIO_PASSTHROUGH_BRIGHTNESS_STRENGTH_DEFAULT;
    audio_passthrough_db.ovp_enable           = false;
    audio_passthrough_db.ovp_level            = AUDIO_PASSTHROUGH_OVP_LEVEL_DEFAULT;
    audio_passthrough_db.llapt_scenario_id    = 0;
    audio_passthrough_db.effect_apply         = false;
    os_queue_init(&audio_passthrough_db.effects);

    return true;
}

void audio_passthrough_deinit(void)
{
    T_AUDIO_EFFECT_INSTANCE instance;

    while ((instance = os_queue_out(&audio_passthrough_db.effects)) != NULL)
    {
        audio_effect_owner_clear(instance);
    }

    audio_passthrough_destroy();
}

bool audio_passthrough_create(uint32_t device,
                              uint32_t sample_rate)
{
    T_AUDIO_PATH_PARAM apt_param;
    T_AUDIO_PATH_PARAM llapt_param;
    int32_t            ret = 0;

    if (audio_passthrough_db.apt != NULL)
    {
        ret = 1;
        goto fail_apt_exist;
    }

    audio_passthrough_db.apt = calloc(1, sizeof(T_AUDIO_PASSTHROUGH_SESSION));
    if (audio_passthrough_db.apt == NULL)
    {
        ret = 2;
        goto fail_alloc_apt;
    }

    apt_param.apt.device          = device;
    apt_param.apt.dac_level       = audio_passthrough_db.volume_out_level;
    apt_param.apt.adc_level       = audio_passthrough_db.volume_in_level;
    apt_param.apt.dac_sample_rate = sample_rate;
    apt_param.apt.adc_sample_rate = sample_rate;

    audio_passthrough_db.apt->path_handle = audio_path_create(AUDIO_CATEGORY_APT,
                                                              apt_param,
                                                              audio_passthrough_path_cback);
    if (audio_passthrough_db.apt->path_handle == NULL)
    {
        ret = 3;
        goto fail_create_apt_path;
    }

    audio_passthrough_db.apt->state  = AUDIO_PASSTHROUGH_SESSION_STATE_CREATED;
    audio_passthrough_db.apt->action = AUDIO_PASSTHROUGH_SESSION_ACTION_NONE;

    if (audio_passthrough_db.llapt != NULL)
    {
        ret = 4;
        goto fail_llapt_exist;
    }

    audio_passthrough_db.llapt = calloc(1, sizeof(T_AUDIO_PASSTHROUGH_SESSION));
    if (audio_passthrough_db.llapt == NULL)
    {
        ret = 5;
        goto fail_alloc_llapt;
    }

    llapt_param.llapt.device    = AUDIO_DEVICE_OUT_SPK | AUDIO_DEVICE_IN_MIC;
    llapt_param.llapt.dac_level = audio_passthrough_db.volume_out_level;
    llapt_param.llapt.adc_level = audio_passthrough_db.volume_in_level;

    audio_passthrough_db.llapt->path_handle = audio_path_create(AUDIO_CATEGORY_LLAPT,
                                                                llapt_param,
                                                                audio_passthrough_path_cback);
    if (audio_passthrough_db.llapt->path_handle == NULL)
    {
        ret = 6;
        goto fail_create_llapt_path;
    }

    audio_passthrough_db.llapt->state  = AUDIO_PASSTHROUGH_SESSION_STATE_CREATED;
    audio_passthrough_db.llapt->action = AUDIO_PASSTHROUGH_SESSION_ACTION_NONE;

    return true;

fail_create_llapt_path:
    free(audio_passthrough_db.llapt);
    audio_passthrough_db.llapt = NULL;
fail_alloc_llapt:
fail_llapt_exist:
    audio_path_destroy(audio_passthrough_db.apt->path_handle);
fail_create_apt_path:
    free(audio_passthrough_db.apt);
    audio_passthrough_db.apt = NULL;
fail_alloc_apt:
fail_apt_exist:
    AUDIO_PRINT_ERROR1("audio_passthrough_create: failed %d ", -ret);
    return false;
}

bool audio_passthrough_destroy(void)
{
    int32_t ret = 0;

    if (audio_passthrough_db.apt != NULL)
    {
        if (audio_passthrough_session_state_set(audio_passthrough_db.apt,
                                                AUDIO_PASSTHROUGH_SESSION_STATE_RELEASING) == false)
        {
            ret = 1;
            goto fail_destroy_normal_apt;
        }
    }

    if (audio_passthrough_db.llapt != NULL)
    {
        if (audio_passthrough_session_state_set(audio_passthrough_db.llapt,
                                                AUDIO_PASSTHROUGH_SESSION_STATE_RELEASING) == false)
        {
            ret = 2;
            goto fail_destroy_llapt;
        }
    }

    return true;

fail_destroy_llapt:
fail_destroy_normal_apt:
    AUDIO_PRINT_ERROR1("audio_passthrough_destroy: failed %d ", -ret);
    return false;
}

bool audio_passthrough_enable(T_AUDIO_PASSTHROUGH_MODE mode,
                              uint8_t                  llapt_scenario_id)
{
    T_AUDIO_PASSTHROUGH_SESSION *session = NULL;
    bool                         ret = false;

    AUDIO_PRINT_INFO2("audio_passthrough_enable: mode %d, llapt scenario %d",
                      mode, llapt_scenario_id);

    audio_passthrough_db.mode              = mode;
    audio_passthrough_db.llapt_scenario_id = llapt_scenario_id;

    if (mode == AUDIO_PASSTHROUGH_MODE_NORMAL)
    {
        session = audio_passthrough_db.apt;
    }
    else if (mode == AUDIO_PASSTHROUGH_MODE_LOW_LATENCY)
    {
        session = audio_passthrough_db.llapt;
    }

    if (session != NULL)
    {
        ret = audio_passthrough_session_state_set(session,
                                                  AUDIO_PASSTHROUGH_SESSION_STATE_STARTING);
    }

    return ret;
}

bool audio_passthrough_disable(void)
{
    bool ret = false;

    if (audio_passthrough_db.mode == AUDIO_PASSTHROUGH_MODE_NORMAL)
    {
        if (audio_passthrough_db.apt != NULL)
        {
            ret = audio_passthrough_session_state_set(audio_passthrough_db.apt,
                                                      AUDIO_PASSTHROUGH_SESSION_STATE_STOPPING);
        }
    }
    else if (audio_passthrough_db.mode == AUDIO_PASSTHROUGH_MODE_LOW_LATENCY)
    {
        if (audio_passthrough_db.llapt != NULL)
        {
            ret = audio_passthrough_session_state_set(audio_passthrough_db.llapt,
                                                      AUDIO_PASSTHROUGH_SESSION_STATE_STOPPING);
        }
    }

    return ret;
}

T_AUDIO_PASSTHROUGH_STATE audio_passthrough_state_get()
{
    T_AUDIO_PASSTHROUGH_STATE    state   = AUDIO_PASSTHROUGH_STATE_RELEASED;
    T_AUDIO_PASSTHROUGH_SESSION *session = NULL;

    if (audio_passthrough_db.mode == AUDIO_PASSTHROUGH_MODE_NORMAL)
    {
        session = audio_passthrough_db.apt;
    }
    else if (audio_passthrough_db.mode == AUDIO_PASSTHROUGH_MODE_LOW_LATENCY)
    {
        session = audio_passthrough_db.llapt;
    }

    if (session != NULL)
    {
        switch (session->state)
        {
        case AUDIO_PASSTHROUGH_SESSION_STATE_RELEASED:
            state = AUDIO_PASSTHROUGH_STATE_RELEASED;
            break;

        case AUDIO_PASSTHROUGH_SESSION_STATE_CREATED:
            state = AUDIO_PASSTHROUGH_STATE_CREATED;
            break;

        case AUDIO_PASSTHROUGH_SESSION_STATE_STARTING:
            state = AUDIO_PASSTHROUGH_STATE_STARTING;
            break;

        case AUDIO_PASSTHROUGH_SESSION_STATE_STARTED:
            state = AUDIO_PASSTHROUGH_STATE_STARTED;
            break;

        case AUDIO_PASSTHROUGH_SESSION_STATE_STOPPING:
            state = AUDIO_PASSTHROUGH_STATE_STOPPING;
            break;

        case AUDIO_PASSTHROUGH_SESSION_STATE_STOPPED:
            state = AUDIO_PASSTHROUGH_STATE_STOPPED;
            break;

        case AUDIO_PASSTHROUGH_SESSION_STATE_RELEASING:
            state = AUDIO_PASSTHROUGH_STATE_RELEASING;
            break;

        default:
            break;
        }
    }

    return state;
}

uint8_t audio_passthrough_volume_out_max_get(void)
{
    return audio_passthrough_db.volume_out_level_max;
}

bool audio_passthrough_volume_out_max_set(uint8_t level)
{
    audio_passthrough_db.volume_out_level_max = level;
    return true;
}

uint8_t audio_passthrough_volume_out_min_get(void)
{
    return audio_passthrough_db.volume_out_level_min;
}

bool audio_passthrough_volume_out_min_set(uint8_t level)
{
    audio_passthrough_db.volume_out_level_min = level;
    return true;
}

uint8_t audio_passthrough_volume_out_get(void)
{
    return audio_passthrough_db.volume_out_level;
}

bool audio_passthrough_volume_out_set(uint8_t level)
{
    T_AUDIO_PASSTHROUGH_SESSION *session = NULL;
    int32_t ret = 0;

    if (level < audio_passthrough_db.volume_out_level_min ||
        level > audio_passthrough_db.volume_out_level_max)
    {
        ret = 1;
        goto fail_check_volume;
    }

    session = audio_passthrough_db.apt;
    if (session != NULL)
    {
        if (session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
        {
            if (audio_passthrough_db.volume_out_muted == false)
            {
                if (audio_path_dac_level_set(session->path_handle,
                                             level,
                                             audio_passthrough_db.volume_out_scale) == false)
                {
                    ret = 2;
                    goto fail_set_apt_volume;
                }
            }
        }
    }

    session = audio_passthrough_db.llapt;
    if (session != NULL)
    {
        if (session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
        {
            if (audio_passthrough_db.volume_out_muted == false)
            {
                if (audio_path_dac_level_set(session->path_handle,
                                             level,
                                             audio_passthrough_db.volume_out_scale) == false)
                {
                    ret = 3;
                    goto fail_set_llapt_volume;
                }
            }
        }
    }

    audio_passthrough_db.volume_out_level = level;

    return true;

fail_set_llapt_volume:
fail_set_apt_volume:
fail_check_volume:
    AUDIO_PRINT_ERROR2("audio_passthrough_volume_out_set: handle %p, failed %d",
                       session, -ret);
    return false;
}

bool audio_passthrough_volume_out_mute(void)
{
    T_AUDIO_PASSTHROUGH_SESSION *session = NULL;
    int32_t ret = 0;

    if (audio_passthrough_db.volume_out_muted == true)
    {
        ret = 1;
        goto fail_check_muted;
    }

    session = audio_passthrough_db.apt;
    if (session != NULL)
    {
        if (session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
        {
            if (audio_path_dac_mute(session->path_handle) == false)
            {
                ret = 2;
                goto fail_mute_apt_volume;
            }
        }
    }

    session = audio_passthrough_db.llapt;
    if (session != NULL)
    {
        if (session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
        {
            if (audio_path_dac_mute(session->path_handle) == false)
            {
                ret = 3;
                goto fail_mute_llapt_volume;
            }
        }
    }

    audio_passthrough_db.volume_out_muted = true;

    return true;

fail_mute_llapt_volume:
fail_mute_apt_volume:
fail_check_muted:
    AUDIO_PRINT_ERROR2("audio_passthrough_volume_out_mute: handle %p, failed %d",
                       session, -ret);
    return false;
}

bool audio_passthrough_volume_out_unmute(void)
{
    T_AUDIO_PASSTHROUGH_SESSION *session = NULL;
    int32_t ret = 0;

    if (audio_passthrough_db.volume_out_muted == false)
    {
        ret = 1;
        goto fail_check_muted;
    }

    session = audio_passthrough_db.apt;
    if (session != NULL)
    {
        if (session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
        {
            if (audio_path_dac_level_set(session->path_handle,
                                         audio_passthrough_db.volume_out_level,
                                         audio_passthrough_db.volume_out_scale) == false)
            {
                ret = 2;
                goto fail_unmute_apt_volume;
            }
        }
    }

    session = audio_passthrough_db.llapt;
    if (session != NULL)
    {
        if (session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
        {
            if (audio_path_dac_level_set(session->path_handle,
                                         audio_passthrough_db.volume_out_level,
                                         audio_passthrough_db.volume_out_scale) == false)
            {
                ret = 3;
                goto fail_unmute_llapt_volume;
            }
        }
    }

    audio_passthrough_db.volume_out_muted = false;

    return true;

fail_unmute_llapt_volume:
fail_unmute_apt_volume:
fail_check_muted:
    AUDIO_PRINT_ERROR2("audio_passthrough_volume_out_mute: handle %p, failed %d",
                       session, -ret);
    return false;
}

bool audio_passthrough_volume_out_is_muted(void)
{
    return audio_passthrough_db.volume_out_muted;
}

uint8_t audio_passthrough_volume_in_max_get(void)
{
    return audio_passthrough_db.volume_in_level_max;
}

bool audio_passthrough_volume_in_max_set(uint8_t level)
{
    audio_passthrough_db.volume_in_level_max = level;
    return true;
}

uint8_t audio_passthrough_volume_in_min_get(void)
{
    return audio_passthrough_db.volume_in_level_min;
}

bool audio_passthrough_volume_in_min_set(uint8_t level)
{
    audio_passthrough_db.volume_in_level_min = level;
    return true;
}

uint8_t audio_passthrough_volume_in_get(void)
{
    return audio_passthrough_db.volume_in_level;
}

bool audio_passthrough_volume_in_set(uint8_t level)
{
    T_AUDIO_PASSTHROUGH_SESSION *session = NULL;
    int32_t ret = 0;

    if (level < audio_passthrough_db.volume_in_level_min ||
        level > audio_passthrough_db.volume_in_level_max)
    {
        ret = 1;
        goto fail_check_volume;
    }

    session = audio_passthrough_db.apt;
    if (session != NULL)
    {
        if (session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
        {
            if (audio_passthrough_db.volume_in_muted == false)
            {
                if (audio_path_adc_level_set(session->path_handle,
                                             level,
                                             0.0f) == false)
                {
                    ret = 2;
                    goto fail_set_apt_volume;
                }
            }
        }
    }

    session = audio_passthrough_db.llapt;
    if (session != NULL)
    {
        if (session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
        {
            if (audio_passthrough_db.volume_in_muted == false)
            {
                if (audio_path_adc_level_set(session->path_handle,
                                             level,
                                             0.0f) == false)
                {
                    ret = 3;
                    goto fail_set_llapt_volume;
                }
            }
        }
    }

    audio_passthrough_db.volume_in_level = level;

    return true;

fail_set_llapt_volume:
fail_set_apt_volume:
fail_check_volume:
    AUDIO_PRINT_ERROR2("audio_passthrough_volume_in_set: handle %p, failed %d",
                       session, -ret);
    return false;
}

bool audio_passthrough_volume_in_mute(void)
{
    T_AUDIO_PASSTHROUGH_SESSION *session = NULL;
    int32_t ret = 0;

    if (audio_passthrough_db.volume_in_muted == true)
    {
        ret = 1;
        goto fail_check_muted;
    }

    session = audio_passthrough_db.apt;
    if (session != NULL)
    {
        if (session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
        {
            if (audio_path_adc_mute(session->path_handle) == false)
            {
                ret = 2;
                goto fail_mute_apt_volume;
            }
        }
    }

    session = audio_passthrough_db.llapt;
    if (session != NULL)
    {
        if (session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
        {
            if (audio_path_adc_mute(session->path_handle) == false)
            {
                ret = 3;
                goto fail_mute_llapt_volume;
            }
        }
    }

    audio_passthrough_db.volume_in_muted = true;

    return true;

fail_mute_llapt_volume:
fail_mute_apt_volume:
fail_check_muted:
    AUDIO_PRINT_ERROR2("audio_passthrough_volume_in_mute: handle %p, failed %d",
                       session, -ret);
    return false;
}

bool audio_passthrough_volume_in_unmute(void)
{
    T_AUDIO_PASSTHROUGH_SESSION *session = NULL;
    int32_t ret = 0;

    if (audio_passthrough_db.volume_in_muted == false)
    {
        ret = 1;
        goto fail_check_muted;
    }

    session = audio_passthrough_db.apt;
    if (session != NULL)
    {
        if (session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
        {
            if (audio_path_adc_level_set(session->path_handle,
                                         audio_passthrough_db.volume_in_level,
                                         0.0f) == false)
            {
                ret = 2;
                goto fail_unmute_apt_volume;
            }
        }
    }

    session = audio_passthrough_db.llapt;
    if (session != NULL)
    {
        if (session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
        {
            if (audio_path_adc_level_set(session->path_handle,
                                         audio_passthrough_db.volume_in_level,
                                         0.0f) == false)
            {
                ret = 2;
                goto fail_unmute_llapt_volume;
            }
        }
    }

    audio_passthrough_db.volume_in_muted = false;

    return true;

fail_unmute_llapt_volume:
fail_unmute_apt_volume:
fail_check_muted:
    AUDIO_PRINT_ERROR2("audio_passthrough_volume_in_unmute: handle %p, failed %d",
                       session, -ret);
    return false;
}

bool audio_passthrough_volume_in_is_muted(void)
{
    return audio_passthrough_db.volume_in_muted;
}

float audio_passthrough_volume_balance_get(void)
{
    return audio_passthrough_db.volume_out_scale;
}

bool audio_passthrough_volume_balance_set(float scale)
{
    T_AUDIO_PASSTHROUGH_SESSION *session = NULL;
    int32_t ret = 0;

    if (scale < -1.0f || scale > 1.0f)
    {
        ret = 1;
        goto fail_check_scale;
    }

    session = audio_passthrough_db.apt;
    if (session != NULL)
    {
        if (session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
        {
            if (audio_path_dac_level_set(session->path_handle,
                                         audio_passthrough_db.volume_out_level,
                                         scale) == false)
            {
                ret = 2;
                goto fail_set_apt_scale;
            }
        }
    }

    session = audio_passthrough_db.llapt;
    if (session != NULL)
    {
        if (session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
        {
            if (audio_path_dac_level_set(session->path_handle,
                                         audio_passthrough_db.volume_out_level,
                                         scale) == false)
            {
                ret = 3;
                goto fail_set_llapt_scale;
            }
        }
    }

    audio_passthrough_db.volume_out_scale = scale;

    return true;

fail_set_llapt_scale:
fail_set_apt_scale:
fail_check_scale:
    AUDIO_PRINT_ERROR3("audio_passthrough_volume_balance_set: handle %p, scale %d/1000, failed %d",
                       session, (int32_t)(scale * 1000), -ret);
    return false;
}

bool audio_passthrough_brightness_set(float strength)
{
    T_AUDIO_PASSTHROUGH_SESSION *session;

    if (strength >= audio_passthrough_db.brightness_strength_min &&
        strength <= audio_passthrough_db.brightness_strength_max)
    {
        audio_passthrough_db.brightness_strength = strength;

        session = audio_passthrough_db.llapt;
        if (session != NULL)
        {
            if (session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
            {
                audio_path_brightness_set(session->path_handle, strength);
            }
        }
        return true;
    }
    return false;
}

bool audio_passthrough_ovp_enable(uint8_t level)
{
    T_AUDIO_PASSTHROUGH_SESSION *session = audio_passthrough_db.apt;
    int32_t                      ret = 0;

    if (audio_passthrough_db.ovp_enable == true)
    {
        ret = 1;
        goto fail_check_ovp;
    }

    if (level > AUDIO_PASSTHROUGH_OVP_LEVEL_MAX)
    {
        ret = 2;
        goto fail_check_level;
    }

    audio_passthrough_db.ovp_enable = true;
    audio_passthrough_db.ovp_level  = level;

    if (session != NULL)
    {
        if (session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
        {
            audio_path_ovp_set(session->path_handle, level);
        }
    }
    return true;

fail_check_level:
fail_check_ovp:
    AUDIO_PRINT_ERROR2("audio_passthrough_ovp_enable: handle %p, failed %d",
                       session, -ret);
    return false;
}

bool audio_passthrough_ovp_set(uint8_t level)
{
    T_AUDIO_PASSTHROUGH_SESSION *session = audio_passthrough_db.apt;

    if (audio_passthrough_db.ovp_level <= AUDIO_PASSTHROUGH_OVP_LEVEL_MAX)
    {
        audio_passthrough_db.ovp_level  = level;

        if (audio_passthrough_db.ovp_enable == true)
        {
            if (session != NULL)
            {
                if (session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
                {
                    audio_path_ovp_set(session->path_handle, level);
                }
            }
        }

        return true;
    }

    return false;
}

bool audio_passthrough_ovp_disable(void)
{
    T_AUDIO_PASSTHROUGH_SESSION *session = audio_passthrough_db.apt;

    if (audio_passthrough_db.ovp_enable == true)
    {
        audio_passthrough_db.ovp_enable = false;
        audio_passthrough_db.ovp_level  = AUDIO_PASSTHROUGH_OVP_LEVEL_DEFAULT;

        if (session != NULL)
        {
            if (session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
            {
                audio_path_ovp_clear(session->path_handle);
            }
        }

        return true;
    }

    return false;
}

static bool audio_passthrough_effect_check(T_AUDIO_EFFECT_INSTANCE instance)
{
    if (instance == NULL)
    {
        return false;
    }

    if (os_queue_search(&audio_passthrough_db.effects, instance) == true)
    {
        return true;
    }

    return false;
}

static bool audio_passthrough_effect_enqueue(T_AUDIO_EFFECT_INSTANCE instance)
{
    if (instance == NULL)
    {
        return false;
    }

    if (os_queue_search(&audio_passthrough_db.effects, instance) == true)
    {
        return true;
    }

    os_queue_in(&audio_passthrough_db.effects, instance);
    return true;
}

static bool audio_passthrough_effect_dequeue(T_AUDIO_EFFECT_INSTANCE instance)
{
    if (instance == NULL)
    {
        return false;
    }

    return os_queue_delete(&audio_passthrough_db.effects, instance);
}

static bool audio_passthrough_effect_run(void)
{
    T_AUDIO_EFFECT_INSTANCE instance;
    int32_t                 i = 0;

    while ((instance = os_queue_peek(&audio_passthrough_db.effects, i)) != NULL)
    {
        audio_effect_run(instance);
        i++;
    }

    return true;
}

static bool audio_passthrough_effect_stop(void)
{
#if 0
    T_AUDIO_EFFECT_INSTANCE instance;
    int32_t                 i = 0;

    while ((instance = os_queue_peek(&audio_passthrough_db.effects, i)) != NULL)
    {
        audio_effect_stop(instance);
        i++;
    }
#endif
    return true;
}

static bool audio_passthrough_effect_clear(void)
{
    T_AUDIO_EFFECT_INSTANCE instance;

    while ((instance = os_queue_out(&audio_passthrough_db.effects)) != NULL)
    {
        audio_effect_owner_clear(instance);
    }

    return true;
}

static void audio_passthrough_effect_cback(T_AUDIO_EFFECT_INSTANCE  instance,
                                           T_AUDIO_EFFECT_EVENT     event,
                                           void                    *context)
{
    T_AUDIO_PATH_HANDLE          handle;
    T_AUDIO_PASSTHROUGH_SESSION *apt_session;

    handle = audio_effect_owner_get(instance);
    apt_session = audio_passthrough_db.apt;

    AUDIO_PRINT_TRACE4("audio_passthrough_effect_cback: instance %p, handle %p, event 0x%02x, apt_session %p",
                       instance, handle, event, apt_session);

    if (apt_session != NULL)
    {
        if (apt_session->path_handle == handle)
        {
            switch (event)
            {
            case AUDIO_EFFECT_EVENT_CREATED:
                break;

            case AUDIO_EFFECT_EVENT_ENABLED:
                if (apt_session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
                {
                    audio_effect_run(instance);
                }
                else if (apt_session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTING)
                {
                    if (audio_passthrough_db.effect_apply == true)
                    {
                        audio_effect_run(instance);
                    }
                }
                break;

            case AUDIO_EFFECT_EVENT_DISABLED:
                if (apt_session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
                {
                    audio_effect_stop(instance);
                }
                else if (apt_session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTING)
                {
                    if (audio_passthrough_db.effect_apply == true)
                    {
                        audio_effect_stop(instance);
                    }
                }
                break;

            case AUDIO_EFFECT_EVENT_UPDATED:
                if (apt_session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
                {
                    audio_effect_flush(instance, context);
                }
                else if (apt_session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTING)
                {
                    if (audio_passthrough_db.effect_apply == true)
                    {
                        audio_effect_flush(instance, context);
                    }
                }
                break;

            case AUDIO_EFFECT_EVENT_RELEASED:
                if (apt_session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
                {
                    audio_effect_stop(instance);
                }
                else if (apt_session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTING)
                {
                    if (audio_passthrough_db.effect_apply == true)
                    {
                        audio_effect_stop(instance);
                    }
                }

                audio_passthrough_effect_dequeue(instance);
                audio_effect_owner_clear(instance);
                break;
            }
        }
    }
}

bool audio_passthrough_effect_attach(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_AUDIO_PASSTHROUGH_SESSION *apt_session = audio_passthrough_db.apt;
    int32_t                      ret = 0;

    if (apt_session == NULL)
    {
        ret = 1;
        goto fail_check_apt_session;
    }

    if (audio_passthrough_effect_enqueue(instance) == false)
    {
        ret = 2;
        goto fail_enqueue_effect;
    }

    if (audio_effect_owner_set(instance,
                               apt_session->path_handle,
                               audio_passthrough_effect_cback) == false)
    {
        ret = 2;
        goto fail_set_owner;
    }

    if (apt_session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
    {
        if (audio_passthrough_db.mode == AUDIO_PASSTHROUGH_MODE_NORMAL)
        {
            audio_effect_run(instance);
        }
    }

    return true;

fail_set_owner:
    audio_passthrough_effect_dequeue(instance);
fail_enqueue_effect:
fail_check_apt_session:
    AUDIO_PRINT_ERROR2("audio_passthrough_effect_attach: instance %p, failed %d",
                       instance, -ret);
    return false;
}

bool audio_passthrough_effect_detach(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_AUDIO_PASSTHROUGH_SESSION *apt_session = audio_passthrough_db.apt;
    int32_t                      ret = 0;

    if (apt_session == NULL)
    {
        ret = 1;
        goto fail_check_apt_session;
    }

    if (audio_passthrough_effect_check(instance) == false)
    {
        ret = 2;
        goto fail_check_instance;
    }

    if (apt_session->state == AUDIO_PASSTHROUGH_SESSION_STATE_STARTED)
    {
        audio_effect_stop(instance);
    }

    if (audio_passthrough_effect_dequeue(instance) == false)
    {
        ret = 3;
        goto fail_dequeue_effect;
    }

    if (audio_effect_owner_clear(instance) == false)
    {
        ret = 4;
        goto fail_clear_owner;
    }

    return true;

fail_clear_owner:
    audio_passthrough_effect_enqueue(instance);
fail_dequeue_effect:
fail_check_instance:
fail_check_apt_session:
    AUDIO_PRINT_ERROR2("audio_passthrough_effect_detach: instance %p, failed %d",
                       instance, -ret);
    return false;
}

bool audio_passthrough_route_path_take(T_AUDIO_PASSTHROUGH_MODE  mode,
                                       uint32_t                  device,
                                       T_AUDIO_ROUTE_PATH       *path)
{
    int32_t ret = 0;

    if (mode == AUDIO_PASSTHROUGH_MODE_NORMAL)
    {
        if (audio_passthrough_db.apt != NULL)
        {
            *path = audio_route_path_get(audio_passthrough_db.apt->path_handle,
                                         AUDIO_CATEGORY_APT,
                                         device);
            if (path->entry == NULL)
            {
                ret = 1;
                goto fail_get_route;
            }
        }
    }
    else if (mode == AUDIO_PASSTHROUGH_MODE_LOW_LATENCY)
    {
        if (audio_passthrough_db.llapt != NULL)
        {
            *path = audio_route_path_get(audio_passthrough_db.llapt->path_handle,
                                         AUDIO_CATEGORY_LLAPT,
                                         device);
            if (path->entry == NULL)
            {
                ret = 2;
                goto fail_get_route;
            }
        }
    }

    return true;

fail_get_route:
    AUDIO_PRINT_ERROR3("audio_passthrough_route_path_take: failed %d, mode %d, device 0x%08x",
                       -ret, mode, device);
    return false;
}

void audio_passthrough_route_path_give(T_AUDIO_ROUTE_PATH *path)
{
    audio_route_path_give(path);
}

bool audio_passthrough_route_bind(T_AUDIO_PASSTHROUGH_MODE mode,
                                  T_AUDIO_ROUTE_PATH       path)
{
    int32_t ret = 0;

    if (mode == AUDIO_PASSTHROUGH_MODE_NORMAL)
    {
        if (audio_passthrough_db.apt != NULL)
        {
            if (audio_route_path_bind(audio_passthrough_db.apt->path_handle,
                                      path) == false)
            {
                ret = 1;
                goto fail_bind_path;
            }
        }
    }
    else if (mode == AUDIO_PASSTHROUGH_MODE_LOW_LATENCY)
    {
        if (audio_passthrough_db.llapt != NULL)
        {
            if (audio_route_path_bind(audio_passthrough_db.llapt->path_handle,
                                      path) == false)
            {
                ret = 2;
                goto fail_bind_path;
            }
        }
    }

    return true;

fail_bind_path:
    AUDIO_PRINT_ERROR2("audio_passthrough_route_bind: mode %d, failed %d",
                       mode, -ret);
    return false;
}

bool audio_passthrough_route_unbind(T_AUDIO_PASSTHROUGH_MODE mode)
{
    bool ret = false;

    if (mode == AUDIO_PASSTHROUGH_MODE_NORMAL)
    {
        if (audio_passthrough_db.apt != NULL)
        {
            ret = audio_route_path_unbind(audio_passthrough_db.apt->path_handle);
        }
    }
    else if (mode == AUDIO_PASSTHROUGH_MODE_LOW_LATENCY)
    {
        if (audio_passthrough_db.llapt != NULL)
        {
            ret = audio_route_path_unbind(audio_passthrough_db.llapt->path_handle);
        }
    }

    return ret;
}

bool audio_passthrough_route_add(T_AUDIO_PASSTHROUGH_MODE  mode,
                                 T_AUDIO_ROUTE_ENTRY      *entry)
{
    bool ret = false;

    if (mode == AUDIO_PASSTHROUGH_MODE_NORMAL)
    {
        if (audio_passthrough_db.apt != NULL)
        {
            ret = audio_route_path_add(audio_passthrough_db.apt->path_handle,
                                       entry);
        }
    }
    else if (mode == AUDIO_PASSTHROUGH_MODE_LOW_LATENCY)
    {
        if (audio_passthrough_db.llapt != NULL)
        {
            ret = audio_route_path_add(audio_passthrough_db.llapt->path_handle,
                                       entry);
        }
    }

    return ret;
}

bool audio_passthrough_route_remove(T_AUDIO_PASSTHROUGH_MODE mode,
                                    T_AUDIO_ROUTE_IO_TYPE    io_type)
{
    bool ret = false;

    if (mode == AUDIO_PASSTHROUGH_MODE_NORMAL)
    {
        if (audio_passthrough_db.apt != NULL)
        {
            ret = audio_route_path_remove(audio_passthrough_db.apt->path_handle,
                                          io_type);
        }
    }
    else if (mode == AUDIO_PASSTHROUGH_MODE_LOW_LATENCY)
    {
        if (audio_passthrough_db.llapt != NULL)
        {
            ret = audio_route_path_remove(audio_passthrough_db.llapt->path_handle,
                                          io_type);
        }
    }

    return ret;
}
