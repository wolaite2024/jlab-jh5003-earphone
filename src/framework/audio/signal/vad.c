/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if (CONFIG_REALTEK_AM_VAD_SUPPORT == 1)
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "os_queue.h"
#include "trace.h"
#include "audio_type.h"
#include "audio.h"
#include "audio_mgr.h"
#include "audio_path.h"
#include "audio_effect.h"
#include "vad.h"

#define VAD_VOLUME_ADC_LEVEL_MIN        (0)
#define VAD_VOLUME_ADC_LEVEL_MAX        (15)
#define VAD_VOLUME_ADC_LEVEL_DEFAULT    (10)

typedef enum t_vad_action
{
    VAD_ACTION_NONE     = 0x00,
    VAD_ACTION_CREATE   = 0x01,
    VAD_ACTION_START    = 0x02,
    VAD_ACTION_STOP     = 0x03,
    VAD_ACTION_RELEASE  = 0x04,
} T_VAD_ACTION;

typedef enum t_vad_session_state
{
    VAD_SESSION_STATE_RELEASED  = 0x00,
    VAD_SESSION_STATE_CREATING  = 0x01,
    VAD_SESSION_STATE_CREATED   = 0x02,
    VAD_SESSION_STATE_STARTING  = 0x03,
    VAD_SESSION_STATE_STARTED   = 0x04,
    VAD_SESSION_STATE_STOPPING  = 0x05,
    VAD_SESSION_STATE_STOPPED   = 0x06,
    VAD_SESSION_STATE_RELEASING = 0x07,
} T_VAD_SESSION_STATE;

typedef struct t_vad_db
{
    uint32_t                   device;
    T_VAD_SESSION_STATE        state;
    T_VAD_ACTION               action;
    T_AUDIO_PATH_HANDLE        path_handle;
    T_VAD_TYPE                 type;
    T_VAD_AGGRESSIVENESS_LEVEL aggressiveness_level;
    T_AUDIO_FORMAT_INFO        format_info;
    uint8_t                    volume_in;
    uint8_t                    volume_in_min;
    uint8_t                    volume_in_max;
    T_OS_QUEUE                 effects;
    bool                       effect_apply;
    uint8_t                    filter_level;
} T_VAD_DB;

typedef bool (*T_VAD_STATE_HANDLER)(T_VAD_SESSION_STATE state);

static T_VAD_DB vad_db;

static bool vad_effect_run(void);
static bool vad_effect_stop(void);
static bool vad_effect_clear(void);
static bool vad_path_cback(T_AUDIO_PATH_HANDLE path_handle,
                           T_AUDIO_PATH_EVENT  event,
                           uint32_t            param);
static bool vad_state_set(T_VAD_SESSION_STATE state);

static bool vad_released_handler(T_VAD_SESSION_STATE state)
{
    bool ret = true;

    switch (state)
    {
    case VAD_SESSION_STATE_RELEASED:
    case VAD_SESSION_STATE_CREATED:
    case VAD_SESSION_STATE_STARTING:
    case VAD_SESSION_STATE_STARTED:
    case VAD_SESSION_STATE_STOPPING:
    case VAD_SESSION_STATE_STOPPED:
    case VAD_SESSION_STATE_RELEASING:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case VAD_SESSION_STATE_CREATING:
        {
            T_AUDIO_PATH_PARAM path_param;

            path_param.vad.device               = vad_db.device;
            path_param.vad.mode                 = AUDIO_STREAM_MODE_NORMAL;
            path_param.vad.adc_level            = vad_db.volume_in;
            path_param.vad.encoder_info         = &vad_db.format_info;
            path_param.vad.aggressiveness_level = vad_db.aggressiveness_level;
            path_param.vad.type                 = vad_db.type;

            vad_db.path_handle = audio_path_create(AUDIO_CATEGORY_VAD,
                                                   path_param,
                                                   vad_path_cback);
            if (vad_db.path_handle != NULL)
            {
                vad_db.state = state;
                ret = true;
            }
            else
            {
                ret = false;
            }
        }
        break;
    }

    return ret;
}

static bool vad_creating_handler(T_VAD_SESSION_STATE state)
{
    bool ret = true;

    switch (state)
    {
    case VAD_SESSION_STATE_RELEASED:
    case VAD_SESSION_STATE_CREATING:
    case VAD_SESSION_STATE_STARTED:
    case VAD_SESSION_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case VAD_SESSION_STATE_CREATED:
        {
            vad_db.state = state;

            if (vad_db.action == VAD_ACTION_NONE)
            {
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating path is impossible.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_START)
            {
                vad_db.action = VAD_ACTION_NONE;
                ret = vad_state_set(VAD_SESSION_STATE_STARTING);
            }
            else if (vad_db.action == VAD_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_RELEASE)
            {
                vad_db.action = VAD_ACTION_NONE;
                ret = vad_state_set(VAD_SESSION_STATE_RELEASING);
            }
        }
        break;

    case VAD_SESSION_STATE_STARTING:
        {
            if (vad_db.action == VAD_ACTION_NONE)
            {
                /* Start the path immediately before the path created event
                 * received, so this action should be pending.
                 */
                vad_db.action = VAD_ACTION_START;
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating path is impossible.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_START)
            {
                /* Drop the repeating path start action. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_RELEASE)
            {
                /* When the creating path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case VAD_SESSION_STATE_STOPPING:
        {
            if (vad_db.action == VAD_ACTION_NONE)
            {
                /* Stopping the creating path is forbidden. */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating path is impossible.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_START)
            {
                /* Cancel all pending actions when the creating path is pending
                 * for start action, but receives the stop action later.
                 */
                vad_db.action = VAD_ACTION_NONE;
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_RELEASE)
            {
                /* When the creating path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case VAD_SESSION_STATE_RELEASING:
        {
            if (vad_db.action == VAD_ACTION_NONE)
            {
                vad_db.action = VAD_ACTION_RELEASE;
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating path is impossible.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_START)
            {
                /* Override the pending start action for the creating path. */
                vad_db.action = VAD_ACTION_RELEASE;
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_RELEASE)
            {
                /* Drop the repeating release action. But this case
                 * is permitted.
                 */
                ret = true;
            }
        }
        break;
    }

    return ret;
}

static bool vad_created_handler(T_VAD_SESSION_STATE state)
{
    bool ret = true;

    switch (state)
    {
    case VAD_SESSION_STATE_RELEASED:
    case VAD_SESSION_STATE_CREATING:
    case VAD_SESSION_STATE_CREATED:
    case VAD_SESSION_STATE_STARTED:
    case VAD_SESSION_STATE_STOPPING:
    case VAD_SESSION_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case VAD_SESSION_STATE_STARTING:
        {
            vad_db.state = state;
            ret = audio_path_start(vad_db.path_handle);
            if (ret == false)
            {
                vad_db.state = VAD_SESSION_STATE_CREATED;
            }
        }
        break;

    case VAD_SESSION_STATE_RELEASING:
        {
            vad_db.state = state;
            ret = audio_path_destroy(vad_db.path_handle);
            if (ret == false)
            {
                vad_db.state = VAD_SESSION_STATE_CREATED;
            }
        }
        break;
    }

    return ret;
}

static bool vad_starting_handler(T_VAD_SESSION_STATE state)
{
    bool ret = true;

    switch (state)
    {
    case VAD_SESSION_STATE_RELEASED:
    case VAD_SESSION_STATE_CREATING:
    case VAD_SESSION_STATE_CREATED:
    case VAD_SESSION_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case VAD_SESSION_STATE_STARTING:
        {
            if (vad_db.action == VAD_ACTION_NONE)
            {
                /* Drop the repeating path start action. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting path is impossible.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_STOP)
            {
                /* Cancel all pending actions when the starting path is pending
                 * for stop action, but receives the start action later.
                 */
                vad_db.action = VAD_ACTION_NONE;
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_RELEASE)
            {
                /* When the starting path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case VAD_SESSION_STATE_STARTED:
        {
            vad_db.state = state;
            vad_db.effect_apply = false;

            audio_path_vad_filter(vad_db.path_handle, vad_db.filter_level);
            audio_mgr_event_post(AUDIO_EVENT_VAD_ENABLED, NULL, 0);

            if (vad_db.action == VAD_ACTION_NONE)
            {
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting path is impossible.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_STOP)
            {
                vad_db.action = VAD_ACTION_NONE;
                ret = vad_state_set(VAD_SESSION_STATE_STOPPING);
            }
            else if (vad_db.action == VAD_ACTION_RELEASE)
            {
                vad_db.action = VAD_ACTION_NONE;
                ret = vad_state_set(VAD_SESSION_STATE_RELEASING);
            }
        }
        break;

    case VAD_SESSION_STATE_STOPPING:
        {
            if (vad_db.action == VAD_ACTION_NONE)
            {
                vad_db.action = VAD_ACTION_STOP;
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting path is impossible.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_STOP)
            {
                /* Drop the repeating stop action. But this case is permitted. */
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_RELEASE)
            {
                /* When the starting path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case VAD_SESSION_STATE_RELEASING:
        {
            if (vad_db.action == VAD_ACTION_NONE)
            {
                vad_db.action = VAD_ACTION_RELEASE;
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting path is impossible.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_STOP)
            {
                /* Override the pending stop action for the starting path. */
                vad_db.action = VAD_ACTION_RELEASE;
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_RELEASE)
            {
                /* Drop the repeating release action. But this case is permitted. */
                ret = true;
            }
        }
        break;
    }

    return ret;
}

static bool vad_started_handler(T_VAD_SESSION_STATE state)
{
    bool ret = true;

    switch (state)
    {
    case VAD_SESSION_STATE_RELEASED:
    case VAD_SESSION_STATE_CREATING:
    case VAD_SESSION_STATE_CREATED:
    case VAD_SESSION_STATE_STARTING:
    case VAD_SESSION_STATE_STARTED:
    case VAD_SESSION_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case VAD_SESSION_STATE_STOPPING:
        {
            vad_db.state = state;
            vad_effect_stop();

            ret = audio_path_stop(vad_db.path_handle);
            if (ret == false)
            {
                vad_db.state = VAD_SESSION_STATE_STARTED;
                vad_effect_run();
            }
        }
        break;

    case VAD_SESSION_STATE_RELEASING:
        {
            /* Started path cannot be released directly, so it should be
             * stopped first before released.
             */
            vad_db.state  = VAD_SESSION_STATE_STOPPING;
            vad_db.action = VAD_ACTION_RELEASE;
            vad_effect_stop();

            ret = audio_path_stop(vad_db.path_handle);
            if (ret == false)
            {
                vad_db.state  = VAD_SESSION_STATE_STARTED;
                vad_db.action = VAD_ACTION_NONE;
                vad_effect_run();
            }
        }
        break;
    }

    return ret;
}

static bool vad_stopping_handler(T_VAD_SESSION_STATE state)
{
    bool ret = true;

    switch (state)
    {
    case VAD_SESSION_STATE_RELEASED:
    case VAD_SESSION_STATE_CREATING:
    case VAD_SESSION_STATE_CREATED:
    case VAD_SESSION_STATE_STARTED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case VAD_SESSION_STATE_STARTING:
        {
            if (vad_db.action == VAD_ACTION_NONE)
            {
                vad_db.action = VAD_ACTION_START;
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping path is impossible.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_START)
            {
                /* Drop the repeating start action. But this case is permitted. */
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_RELEASE)
            {
                /* When the stopping path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case VAD_SESSION_STATE_STOPPING:
        {
            if (vad_db.action == VAD_ACTION_NONE)
            {
                /* Drop the repeating stop action. But this case is permitted. */
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping path is impossible.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_START)
            {
                /* Cancel all pending actions when the stopping path is pending
                 * for start action, but receives the stop action later.
                 */
                vad_db.action = VAD_ACTION_NONE;
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_RELEASE)
            {
                /* When the stopping path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case VAD_SESSION_STATE_STOPPED:
        {
            vad_db.state = state;

            if (vad_db.action == VAD_ACTION_NONE)
            {
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping path is impossible.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_START)
            {
                vad_db.action = VAD_ACTION_NONE;
                ret = vad_state_set(VAD_SESSION_STATE_STARTING);
            }
            else if (vad_db.action == VAD_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_RELEASE)
            {
                vad_db.action = VAD_ACTION_NONE;
                ret = vad_state_set(VAD_SESSION_STATE_RELEASING);
            }
        }
        break;

    case VAD_SESSION_STATE_RELEASING:
        {
            if (vad_db.action == VAD_ACTION_NONE)
            {
                vad_db.action = VAD_ACTION_RELEASE;
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping path is impossible.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_START)
            {
                /* Override the pending start action for the stopping path. */
                vad_db.action = VAD_ACTION_RELEASE;
                ret = true;
            }
            else if (vad_db.action == VAD_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (vad_db.action == VAD_ACTION_RELEASE)
            {
                /* Drop the repeating release action. But this case is permitted. */
                ret = true;
            }
        }
        break;
    }

    return ret;
}

static bool vad_stopped_handler(T_VAD_SESSION_STATE state)
{
    bool ret = true;

    switch (state)
    {
    case VAD_SESSION_STATE_RELEASED:
    case VAD_SESSION_STATE_CREATING:
    case VAD_SESSION_STATE_CREATED:
    case VAD_SESSION_STATE_STARTED:
    case VAD_SESSION_STATE_STOPPING:
    case VAD_SESSION_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case VAD_SESSION_STATE_STARTING:
        {
            vad_db.state = state;
            ret = audio_path_start(vad_db.path_handle);
            if (ret == false)
            {
                vad_db.state = VAD_SESSION_STATE_STOPPED;
            }
        }
        break;

    case VAD_SESSION_STATE_RELEASING:
        {
            vad_db.state = state;
            ret = audio_path_destroy(vad_db.path_handle);
            if (ret == false)
            {
                vad_db.state = VAD_SESSION_STATE_STOPPED;
            }
        }
        break;
    }

    return ret;
}

static bool vad_releasing_handler(T_VAD_SESSION_STATE state)
{
    bool ret = true;

    switch (state)
    {
    case VAD_SESSION_STATE_RELEASED:
        {
            vad_db.state  = state;
            vad_db.action = VAD_ACTION_NONE;
            vad_db.path_handle = NULL;
            vad_effect_clear();
            audio_mgr_event_post(AUDIO_EVENT_VAD_DISABLED, NULL, 0);
        }
        break;

    case VAD_SESSION_STATE_CREATING:
    case VAD_SESSION_STATE_CREATED:
    case VAD_SESSION_STATE_STARTING:
    case VAD_SESSION_STATE_STARTED:
    case VAD_SESSION_STATE_STOPPING:
    case VAD_SESSION_STATE_STOPPED:
        {
            ret = false;
        }
        break;

    case VAD_SESSION_STATE_RELEASING:
        {
            /* Drop the repeating release action. But this case is permitted. */
            ret = true;
        }
        break;
    }

    return ret;
}

static const T_VAD_STATE_HANDLER vad_state_handler[] =
{
    vad_released_handler,
    vad_creating_handler,
    vad_created_handler,
    vad_starting_handler,
    vad_started_handler,
    vad_stopping_handler,
    vad_stopped_handler,
    vad_releasing_handler,
};

static bool vad_state_set(T_VAD_SESSION_STATE state)
{
    AUDIO_PRINT_INFO4("vad_state_set: path_handle %p, curr state %u, next state %u, action %u",
                      vad_db.path_handle, vad_db.state, state, vad_db.action);

    return vad_state_handler[vad_db.state](state);
}

static bool vad_path_cback(T_AUDIO_PATH_HANDLE path_handle,
                           T_AUDIO_PATH_EVENT  event,
                           uint32_t            param)
{
    AUDIO_PRINT_TRACE3("vad_path_cback: handle %p, event 0x%02x, param 0x%08x",
                       path_handle, event, param);

    switch (event)
    {
    case AUDIO_PATH_EVT_RELEASE:
        {
            vad_state_set(VAD_SESSION_STATE_RELEASED);
        }
        break;

    case AUDIO_PATH_EVT_CREATE:
        {
            vad_state_set(VAD_SESSION_STATE_CREATED);
        }
        break;

    case AUDIO_PATH_EVT_RUNNING:
        {
            vad_state_set(VAD_SESSION_STATE_STARTED);
        }
        break;

    case AUDIO_PATH_EVT_IDLE:
        {
            vad_state_set(VAD_SESSION_STATE_STOPPED);
        }
        break;

    case AUDIO_PATH_EVT_KWS_HIT:
        {
            T_AUDIO_PATH_KWS_HIT        *kws_hit = (T_AUDIO_PATH_KWS_HIT *)param;
            T_AUDIO_MSG_PAYLOAD_KWS_HIT  payload;

            payload.keyword_index = kws_hit->keyword_index;

            audio_mgr_dispatch(AUDIO_MSG_KWS_HIT, &payload);
        }
        break;

    case AUDIO_PATH_EVT_EFFECT_REQ:
        {
            vad_db.effect_apply = true;
            vad_effect_run();
        }
        break;

    case AUDIO_PATH_EVT_DATA_IND:
        {
            T_AUDIO_PATH_DATA_HDR *peek_buf;
            uint16_t               peek_len;
            uint16_t               len;
            T_AUDIO_EVENT_PARAM    param;

            peek_len = audio_path_data_peek(vad_db.path_handle);
            if (peek_len != 0)
            {
                peek_buf = malloc(peek_len);
                if (peek_buf != NULL)
                {
                    len = audio_path_data_recv(vad_db.path_handle, peek_buf, peek_len);
                    if (len != 0)
                    {
                        param.vad_data_ind.status         = peek_buf->status;
                        param.vad_data_ind.frame_num      = peek_buf->frame_number;
                        param.vad_data_ind.timestamp      = peek_buf->timestamp;
                        param.vad_data_ind.seq_num        = peek_buf->seq_num;
                        param.vad_data_ind.payload_length = peek_buf->payload_length;
                        param.vad_data_ind.payload        = peek_buf->payload;

                        audio_mgr_event_post(AUDIO_EVENT_VAD_DATA_IND, &param, sizeof(param));
                    }

                    free(peek_buf);
                }
            }
        }
        break;

    default:
        break;
    }

    return true;
}

bool vad_init(void)
{
    vad_db.device               = AUDIO_DEVICE_IN_MIC;
    vad_db.state                = VAD_SESSION_STATE_RELEASED;
    vad_db.action               = VAD_ACTION_NONE;
    vad_db.aggressiveness_level = VAD_AGGRESSIVENESS_LEVEL_LOW;
    vad_db.volume_in            = VAD_VOLUME_ADC_LEVEL_DEFAULT;
    vad_db.volume_in_min        = VAD_VOLUME_ADC_LEVEL_MIN;
    vad_db.volume_in_max        = VAD_VOLUME_ADC_LEVEL_MAX;
    vad_db.effect_apply         = false;
    vad_db.filter_level         = VAD_FILTER_LEVEL_DEFAULT;
    memset(&vad_db.format_info, 0, sizeof(T_AUDIO_FORMAT_INFO));
    os_queue_init(&vad_db.effects);

    return true;
}

void vad_deinit(void)
{
    T_AUDIO_EFFECT_INSTANCE instance;

    instance = os_queue_out(&vad_db.effects);
    while (instance != NULL)
    {
        free(instance);
        instance = os_queue_out(&vad_db.effects);
    }
}

bool vad_enable(T_VAD_TYPE                 type,
                T_VAD_AGGRESSIVENESS_LEVEL aggressiveness_level,
                T_AUDIO_FORMAT_INFO        format_info)
{
    if (vad_db.state == VAD_SESSION_STATE_RELEASED)
    {
        vad_db.type                 = type;
        vad_db.aggressiveness_level = aggressiveness_level;
        vad_db.format_info          = format_info;

        if (vad_state_set(VAD_SESSION_STATE_CREATING))
        {
            return vad_state_set(VAD_SESSION_STATE_STARTING);
        }
    }

    return true;
}

bool vad_filter(T_VAD_FILTER_LEVEL level)
{
    vad_db.filter_level = level;

    if (vad_db.state == VAD_SESSION_STATE_STARTED)
    {
        audio_path_vad_filter(vad_db.path_handle, level);
    }

    return true;
}

bool vad_disable(void)
{
    if (vad_state_set(VAD_SESSION_STATE_STOPPING))
    {
        return vad_state_set(VAD_SESSION_STATE_RELEASING);
    }

    return true;
}

static bool vad_effect_enqueue(T_AUDIO_EFFECT_INSTANCE instance)
{
    if (os_queue_search(&vad_db.effects, instance) == false)
    {
        os_queue_in(&vad_db.effects, instance);
    }

    return true;
}

static bool vad_effect_dequeue(T_AUDIO_EFFECT_INSTANCE instance)
{
    return os_queue_delete(&vad_db.effects, instance);
}

static bool vad_effect_run(void)
{
    T_AUDIO_EFFECT_INSTANCE instance;
    int32_t                 i = 0;

    while ((instance = os_queue_peek(&vad_db.effects, i)) != NULL)
    {
        audio_effect_run(instance);
        i++;
    }

    return true;
}

static bool vad_effect_stop(void)
{
    T_AUDIO_EFFECT_INSTANCE instance;
    int32_t                 i = 0;

    while ((instance = os_queue_peek(&vad_db.effects, i)) != NULL)
    {
        audio_effect_stop(instance);
        i++;
    }

    return true;
}

static bool vad_effect_clear(void)
{
    T_AUDIO_EFFECT_INSTANCE instance;

    while ((instance = os_queue_out(&vad_db.effects)) != NULL)
    {
        audio_effect_owner_clear(instance);
    }

    return true;
}

static void vad_effect_cback(T_AUDIO_EFFECT_INSTANCE  instance,
                             T_AUDIO_EFFECT_EVENT     event,
                             void                    *context)
{
    T_AUDIO_PATH_HANDLE path_handle;

    path_handle = audio_effect_owner_get(instance);

    AUDIO_PRINT_TRACE3("vad_effect_cback: instance %p, path_handle %p, event 0x%02x",
                       instance, path_handle, event);

    if (vad_db.path_handle == path_handle)
    {
        switch (event)
        {
        case AUDIO_EFFECT_EVENT_CREATED:
            break;

        case AUDIO_EFFECT_EVENT_ENABLED:
            if (vad_db.state == VAD_SESSION_STATE_STARTED)
            {
                audio_effect_run(instance);
            }
            else if (vad_db.state == VAD_SESSION_STATE_STARTING)
            {
                if (vad_db.effect_apply == true)
                {
                    audio_effect_run(instance);
                }
            }
            break;

        case AUDIO_EFFECT_EVENT_DISABLED:
            if (vad_db.state == VAD_SESSION_STATE_STARTED)
            {
                audio_effect_stop(instance);
            }
            else if (vad_db.state == VAD_SESSION_STATE_STARTING)
            {
                if (vad_db.effect_apply == true)
                {
                    audio_effect_stop(instance);
                }
            }
            break;

        case AUDIO_EFFECT_EVENT_UPDATED:
            if (vad_db.state == VAD_SESSION_STATE_STARTED)
            {
                audio_effect_flush(instance, context);
            }
            else if (vad_db.state == VAD_SESSION_STATE_STARTING)
            {
                if (vad_db.effect_apply == true)
                {
                    audio_effect_flush(instance, context);
                }
            }
            break;

        case AUDIO_EFFECT_EVENT_RELEASED:
            if (vad_db.state == VAD_SESSION_STATE_STARTED)
            {
                audio_effect_stop(instance);
            }
            else if (vad_db.state == VAD_SESSION_STATE_STARTING)
            {
                if (vad_db.effect_apply == true)
                {
                    audio_effect_stop(instance);
                }
            }

            vad_effect_dequeue(instance);
            audio_effect_owner_clear(instance);
            break;
        }
    }
}

bool vad_effect_attach(T_AUDIO_EFFECT_INSTANCE instance)
{
    int32_t ret = 0;

    if (vad_effect_enqueue(instance) == false)
    {
        ret = 1;
        goto fail_enqueue_effect;
    }

    if (audio_effect_owner_set(instance,
                               vad_db.path_handle,
                               vad_effect_cback) == false)
    {
        ret = 2;
        goto fail_set_owner;
    }

    if (vad_db.state == VAD_SESSION_STATE_STARTED)
    {
        audio_effect_run(instance);
    }

    return true;

fail_set_owner:
    vad_effect_dequeue(instance);
fail_enqueue_effect:
    AUDIO_PRINT_ERROR2("vad_effect_attach: instance %p, failed %d",
                       instance, -ret);
    return false;
}

bool vad_effect_detach(T_AUDIO_EFFECT_INSTANCE instance)
{
    int32_t ret = 0;

    if (os_queue_search(&vad_db.effects, instance) == false)
    {
        ret = 1;
        goto fail_check_instance;
    }

    if (vad_db.state == VAD_SESSION_STATE_STARTED)
    {
        audio_effect_stop(instance);
    }

    if (vad_effect_dequeue(instance) == false)
    {
        ret = 2;
        goto fail_dequeue_effect;
    }

    if (audio_effect_owner_clear(instance) == false)
    {
        ret = 3;
        goto fail_clear_owner;
    }

    return true;

fail_clear_owner:
    vad_effect_enqueue(instance);
fail_dequeue_effect:
fail_check_instance:
    AUDIO_PRINT_ERROR2("vad_effect_detach: instance %p, failed %d",
                       instance, -ret);
    return false;
}
#else
#include <stdbool.h>
#include "vad.h"

bool vad_init(void)
{
    /* Let VAD initialization pass. */
    return true;
}

void vad_deinit(void)
{

}

bool vad_enable(T_VAD_TYPE                 type,
                T_VAD_AGGRESSIVENESS_LEVEL aggressiveness_level,
                T_AUDIO_FORMAT_INFO        format_info)
{
    return false;
}

bool vad_disable(void)
{
    return false;
}

bool vad_filter(T_VAD_FILTER_LEVEL level)
{
    return false;
}

bool vad_effect_attach(T_AUDIO_EFFECT_INSTANCE instance)
{
    return false;
}

bool vad_effect_detach(T_AUDIO_EFFECT_INSTANCE instance)
{
    return false;
}
#endif /* AM_VAD_SUPPORT */
