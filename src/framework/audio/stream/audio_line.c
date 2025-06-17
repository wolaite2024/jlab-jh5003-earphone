/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "os_queue.h"
#include "trace.h"
#include "audio_type.h"
#include "audio_mgr.h"
#include "audio_effect.h"
#include "audio_path.h"
#include "audio_line.h"

#define AUDIO_LINE_DAC_LEVEL_MIN       (0)
#define AUDIO_LINE_DAC_LEVEL_MAX       (15)
#define AUDIO_LINE_DAC_LEVEL_DEFAULT   (10)

#define AUDIO_LINE_ADC_LEVEL_MIN       (0)
#define AUDIO_LINE_ADC_LEVEL_MAX       (15)
#define AUDIO_LINE_ADC_LEVEL_DEFAULT   (10)

typedef enum t_audio_line_session_state
{
    AUDIO_LINE_SESSION_STATE_RELEASED  = 0x00,
    AUDIO_LINE_SESSION_STATE_CREATING  = 0x01,
    AUDIO_LINE_SESSION_STATE_CREATED   = 0x02,
    AUDIO_LINE_SESSION_STATE_STARTING  = 0x03,
    AUDIO_LINE_SESSION_STATE_STARTED   = 0x04,
    AUDIO_LINE_SESSION_STATE_STOPPING  = 0x05,
    AUDIO_LINE_SESSION_STATE_STOPPED   = 0x06,
    AUDIO_LINE_SESSION_STATE_PAUSING   = 0x07,
    AUDIO_LINE_SESSION_STATE_PAUSED    = 0x08,
    AUDIO_LINE_SESSION_STATE_RELEASING = 0x09,
} T_AUDIO_LINE_SESSION_STATE;

typedef enum t_audio_line_action
{
    AUDIO_LINE_ACTION_NONE     = 0x00,
    AUDIO_LINE_ACTION_CREATE   = 0x01,
    AUDIO_LINE_ACTION_START    = 0x02,
    AUDIO_LINE_ACTION_STOP     = 0x03,
    AUDIO_LINE_ACTION_PAUSE    = 0x04,
    AUDIO_LINE_ACTION_RELEASE  = 0x05,
} T_AUDIO_LINE_ACTION;

typedef struct t_audio_line
{
    struct t_audio_line        *next;
    void                       *container;
    T_AUDIO_PATH_HANDLE         path_handle;
    T_AUDIO_LINE_SESSION_STATE  state;
    T_AUDIO_LINE_ACTION         action;
    uint8_t                     volume_out_min;
    uint8_t                     volume_out_max;
    uint8_t                     volume_out;
    uint8_t                     volume_in_min;
    uint8_t                     volume_in_max;
    uint8_t                     volume_in;
    bool                        signal_in;
    bool                        signal_out;
    uint16_t                    signal_in_interval;
    uint16_t                    signal_out_interval;
    bool                        effect_apply;
    T_OS_QUEUE                  effects;
    uint32_t                    device;
    uint32_t                    sample_rate_in;
    uint32_t                    sample_rate_out;
    uint32_t                   *channel_out_array;
    uint8_t                     channel_out_num;
    uint8_t                     freq_in_num;
    uint8_t                     freq_out_num;
    uint32_t                   *freq_in_table;
    uint32_t                   *freq_out_table;
} T_AUDIO_LINE;

typedef struct t_audio_line_db
{
    uint8_t          volume_out_min;
    uint8_t          volume_out_max;
    uint8_t          volume_in_min;
    uint8_t          volume_in_max;
    float            volume_out_scale;
    T_OS_QUEUE       lines;
} T_AUDIO_LINE_DB;

typedef bool (*T_AUDIO_LINE_STATE_HANDLER)(T_AUDIO_LINE               *audio_line,
                                           T_AUDIO_LINE_SESSION_STATE  state);

#if (CONFIG_REALTEK_AM_AUDIO_LINE_SUPPORT == 1)
static T_AUDIO_LINE_DB audio_line_db;

static void audio_line_volume_apply(T_AUDIO_LINE *audio_line);
static bool audio_line_effect_run(T_AUDIO_LINE *audio_line);
static bool audio_line_effect_stop(T_AUDIO_LINE *audio_line);
static bool audio_line_effect_clear(T_AUDIO_LINE *audio_line);
static bool audio_line_path_cback(T_AUDIO_PATH_HANDLE path_handle,
                                  T_AUDIO_PATH_EVENT  event,
                                  uint32_t            param);
static bool audio_line_state_set(T_AUDIO_LINE               *audio_line,
                                 T_AUDIO_LINE_SESSION_STATE  state);

static bool audio_line_queue_add(T_OS_QUEUE   *queue,
                                 T_AUDIO_LINE *audio_line)
{
    if (audio_line->container != NULL)
    {
        /* Delete the audio line from its original audio line queue. */
        if (os_queue_delete(audio_line->container, audio_line) == false)
        {
            /* line is not in its container queue. This shall not happen, anyway. */
            return false;
        }
    }

    audio_line->container = queue;
    os_queue_in(audio_line->container, audio_line);
    return true;
}

static bool audio_line_queue_remove(T_AUDIO_LINE *audio_line)
{
    if (audio_line->container != NULL)
    {
        if (os_queue_delete(audio_line->container, audio_line) == false)
        {
            /* Audio line is not in its container queue. This shall not happen, anyway. */
            return false;
        }

        audio_line->container = NULL;
    }

    return true;
}

static bool audio_line_handle_check(T_AUDIO_LINE_HANDLE handle)
{
    T_AUDIO_LINE *audio_line;

    audio_line = (T_AUDIO_LINE *)handle;

    if (audio_line == NULL)
    {
        return false;
    }

    if (os_queue_search(&audio_line_db.lines, audio_line) == true)
    {
        return true;
    }

    return false;
}

static bool audio_line_released_handler(T_AUDIO_LINE               *audio_line,
                                        T_AUDIO_LINE_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_LINE_SESSION_STATE_RELEASED:
    case AUDIO_LINE_SESSION_STATE_CREATED:
    case AUDIO_LINE_SESSION_STATE_STARTING:
    case AUDIO_LINE_SESSION_STATE_STARTED:
    case AUDIO_LINE_SESSION_STATE_STOPPING:
    case AUDIO_LINE_SESSION_STATE_STOPPED:
    case AUDIO_LINE_SESSION_STATE_PAUSING:
    case AUDIO_LINE_SESSION_STATE_PAUSED:
    case AUDIO_LINE_SESSION_STATE_RELEASING:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_LINE_SESSION_STATE_CREATING:
        {
            T_AUDIO_PATH_PARAM path_param;

            path_param.line.device = audio_line->device;
            path_param.line.dac_level = audio_line->volume_out;
            path_param.line.adc_level = audio_line->volume_in;
            path_param.line.dac_sample_rate = audio_line->sample_rate_out;
            path_param.line.adc_sample_rate = audio_line->sample_rate_in;

            audio_line->path_handle = audio_path_create(AUDIO_CATEGORY_LINE,
                                                        path_param,
                                                        audio_line_path_cback);
            if (audio_line->path_handle != NULL)
            {
                audio_line->state = state;

                audio_line_queue_add(&audio_line_db.lines, audio_line);
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

static bool audio_line_creating_handler(T_AUDIO_LINE               *audio_line,
                                        T_AUDIO_LINE_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_LINE_SESSION_STATE_RELEASED:
    case AUDIO_LINE_SESSION_STATE_CREATING:
    case AUDIO_LINE_SESSION_STATE_STARTED:
    case AUDIO_LINE_SESSION_STATE_STOPPED:
    case AUDIO_LINE_SESSION_STATE_PAUSED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_LINE_SESSION_STATE_CREATED:
        {
            audio_line->state = state;

            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                audio_line->action = AUDIO_LINE_ACTION_NONE;
                ret = audio_line_state_set(audio_line, AUDIO_LINE_SESSION_STATE_STARTING);
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
            {
                audio_line->action = AUDIO_LINE_ACTION_NONE;
                ret = audio_line_state_set(audio_line, AUDIO_LINE_SESSION_STATE_RELEASING);
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_STARTING:
        {
            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                /* Start the path immediately before the path created event
                 * received, so this action should be pending.
                 */
                audio_line->action = AUDIO_LINE_ACTION_START;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                /* Drop the repeating path start action. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
            {
                /* When the creating path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_STOPPING:
        {
            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                /* Stopping the creating path is forbidden. */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                /* Cancel all pending actions when the creating path is pending
                 * for start action, but receives the stop action later.
                 */
                audio_line->action = AUDIO_LINE_ACTION_NONE;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
            {
                /* When the creating path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_PAUSING:
        {
            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                /* Pausing the creating path is forbidden. */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                /* Cancel all pending actions when the creating path is pending
                 * for start action, but receives the pause action later.
                 */
                audio_line->action = AUDIO_LINE_ACTION_NONE;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
            {
                /* When the creating path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_RELEASING:
        {
            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                audio_line->action = AUDIO_LINE_ACTION_RELEASE;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                /* Override the pending start action for the creating path. */
                audio_line->action = AUDIO_LINE_ACTION_RELEASE;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
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

static bool audio_line_created_handler(T_AUDIO_LINE               *audio_line,
                                       T_AUDIO_LINE_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_LINE_SESSION_STATE_RELEASED:
    case AUDIO_LINE_SESSION_STATE_CREATING:
    case AUDIO_LINE_SESSION_STATE_CREATED:
    case AUDIO_LINE_SESSION_STATE_STARTED:
    case AUDIO_LINE_SESSION_STATE_STOPPING:
    case AUDIO_LINE_SESSION_STATE_STOPPED:
    case AUDIO_LINE_SESSION_STATE_PAUSING:
    case AUDIO_LINE_SESSION_STATE_PAUSED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_LINE_SESSION_STATE_STARTING:
        {
            audio_line->state = state;
            ret = audio_path_start(audio_line->path_handle);
            if (ret == false)
            {
                audio_line->state = AUDIO_LINE_SESSION_STATE_CREATED;
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_RELEASING:
        {
            audio_line->state = state;
            ret = audio_path_destroy(audio_line->path_handle);
            if (ret == false)
            {
                audio_line->state = AUDIO_LINE_SESSION_STATE_CREATED;
            }
        }
        break;
    }

    return ret;
}

static bool audio_line_starting_handler(T_AUDIO_LINE               *audio_line,
                                        T_AUDIO_LINE_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_LINE_SESSION_STATE_RELEASED:
    case AUDIO_LINE_SESSION_STATE_CREATING:
    case AUDIO_LINE_SESSION_STATE_CREATED:
    case AUDIO_LINE_SESSION_STATE_STOPPED:
    case AUDIO_LINE_SESSION_STATE_PAUSED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_LINE_SESSION_STATE_STARTING:
        {
            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                /* Drop the repeating path start action. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* Cancel all pending actions when the starting path is pending
                 * for stop action, but receives the start action later.
                 */
                audio_line->action = AUDIO_LINE_ACTION_NONE;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
            {
                /* When the starting path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_STARTED:
        {
            audio_line->state = state;
            audio_line->effect_apply = false;

            audio_line_volume_apply(audio_line);

            if (audio_line->signal_out)
            {
                audio_path_signal_monitoring_set(audio_line->path_handle,
                                                 AUDIO_PATH_DIR_TX,
                                                 true,
                                                 audio_line->signal_out_interval,
                                                 audio_line->freq_out_num,
                                                 audio_line->freq_out_table);
            }

            if (audio_line->signal_in)
            {
                audio_path_signal_monitoring_set(audio_line->path_handle,
                                                 AUDIO_PATH_DIR_RX,
                                                 true,
                                                 audio_line->signal_in_interval,
                                                 audio_line->freq_in_num,
                                                 audio_line->freq_in_table);
            }

            T_AUDIO_EVENT_PARAM param;

            param.line_started.handle = audio_line;
            audio_mgr_event_post(AUDIO_EVENT_LINE_STARTED, &param, sizeof(param));

            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                audio_line->action = AUDIO_LINE_ACTION_NONE;
                ret = audio_line_state_set(audio_line, AUDIO_LINE_SESSION_STATE_STOPPING);
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_PAUSE)
            {
                audio_line->action = AUDIO_LINE_ACTION_NONE;
                ret = audio_line_state_set(audio_line, AUDIO_LINE_SESSION_STATE_PAUSING);
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
            {
                audio_line->action = AUDIO_LINE_ACTION_NONE;
                ret = audio_line_state_set(audio_line, AUDIO_LINE_SESSION_STATE_RELEASING);
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_STOPPING:
        {
            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                audio_line->action = AUDIO_LINE_ACTION_STOP;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* Drop the repeating stop action. But this case is permitted. */
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_PAUSE)
            {
                /* Override the pending pause action for the starting path. */
                audio_line->action = AUDIO_LINE_ACTION_STOP;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
            {
                /* When the starting path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_PAUSING:
        {
            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                audio_line->action = AUDIO_LINE_ACTION_PAUSE;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* Drop the pause action for the starting path, as the pause
                 * action cannot override the pending stop action.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_PAUSE)
            {
                /* Drop the repeating pause action. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
            {
                /* When the starting path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_RELEASING:
        {
            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                audio_line->action = AUDIO_LINE_ACTION_RELEASE;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* Override the pending stop action for the starting path. */
                audio_line->action = AUDIO_LINE_ACTION_RELEASE;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
            {
                /* Drop the repeating release action. But this case is permitted. */
                ret = true;
            }
        }
        break;
    }

    return ret;
}

static bool audio_line_started_handler(T_AUDIO_LINE               *audio_line,
                                       T_AUDIO_LINE_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_LINE_SESSION_STATE_RELEASED:
    case AUDIO_LINE_SESSION_STATE_CREATING:
    case AUDIO_LINE_SESSION_STATE_CREATED:
    case AUDIO_LINE_SESSION_STATE_STARTING:
    case AUDIO_LINE_SESSION_STATE_STARTED:
    case AUDIO_LINE_SESSION_STATE_STOPPED:
    case AUDIO_LINE_SESSION_STATE_PAUSED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_LINE_SESSION_STATE_STOPPING:
        {
            audio_line->state = state;
            audio_line_effect_stop(audio_line);

            ret = audio_path_stop(audio_line->path_handle);
            if (ret == false)
            {
                audio_line->state = AUDIO_LINE_SESSION_STATE_STARTED;
                audio_line_effect_run(audio_line);
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_PAUSING:
        {
            audio_line->state = state;
            audio_line_effect_stop(audio_line);

            /* Currently, there is no audio path pause interface. */
            ret = audio_path_stop(audio_line->path_handle);
            if (ret == false)
            {
                /* Restore the audio_line state. */
                audio_line->state = AUDIO_LINE_SESSION_STATE_STARTED;
                audio_line_effect_run(audio_line);
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_RELEASING:
        {
            /* Started path cannot be released directly, so it should be
             * stopped first before released.
             */
            audio_line->state  = AUDIO_LINE_SESSION_STATE_STOPPING;
            audio_line->action = AUDIO_LINE_ACTION_RELEASE;
            audio_line_effect_stop(audio_line);

            ret = audio_path_stop(audio_line->path_handle);
            if (ret == false)
            {
                audio_line->state  = AUDIO_LINE_SESSION_STATE_STARTED;
                audio_line->action = AUDIO_LINE_ACTION_NONE;
                audio_line_effect_run(audio_line);
            }
        }
        break;
    }

    return ret;
}

static bool audio_line_stopping_handler(T_AUDIO_LINE               *audio_line,
                                        T_AUDIO_LINE_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_LINE_SESSION_STATE_RELEASED:
    case AUDIO_LINE_SESSION_STATE_CREATING:
    case AUDIO_LINE_SESSION_STATE_CREATED:
    case AUDIO_LINE_SESSION_STATE_STARTED:
    case AUDIO_LINE_SESSION_STATE_PAUSED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_LINE_SESSION_STATE_STARTING:
        {
            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                audio_line->action = AUDIO_LINE_ACTION_START;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                /* Drop the repeating start action. But this case is permitted. */
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
            {
                /* When the stopping path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_STOPPING:
        {
            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                /* Drop the repeating stop action. But this case is permitted. */
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                /* Cancel all pending actions when the stopping path is pending
                 * for start action, but receives the stop action later.
                 */
                audio_line->action = AUDIO_LINE_ACTION_NONE;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
            {
                /* When the stopping path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_STOPPED:
        {
            T_AUDIO_EVENT_PARAM param;

            audio_line->state = state;
            param.line_stopped.handle = audio_line;
            audio_mgr_event_post(AUDIO_EVENT_LINE_STOPPED, &param, sizeof(param));

            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                audio_line->action = AUDIO_LINE_ACTION_NONE;
                ret = audio_line_state_set(audio_line, AUDIO_LINE_SESSION_STATE_STARTING);
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
            {
                audio_line->action = AUDIO_LINE_ACTION_NONE;
                ret = audio_line_state_set(audio_line, AUDIO_LINE_SESSION_STATE_RELEASING);
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_PAUSING:
        {
            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                /* Drop the pause action for the stopping path, as the
                 * pause action cannot override the stop action.
                 */
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                /* Cancel all pending actions when the stopping path is pending
                 * for start action, but receives the pause action later.
                 */
                audio_line->action = AUDIO_LINE_ACTION_NONE;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
            {
                /* When the stopping path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_RELEASING:
        {
            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                audio_line->action = AUDIO_LINE_ACTION_RELEASE;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                /* Override the pending start action for the stopping path. */
                audio_line->action = AUDIO_LINE_ACTION_RELEASE;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
            {
                /* Drop the repeating release action. But this case is permitted. */
                ret = true;
            }
        }
        break;
    }

    return ret;
}

static bool audio_line_stopped_handler(T_AUDIO_LINE               *audio_line,
                                       T_AUDIO_LINE_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_LINE_SESSION_STATE_RELEASED:
    case AUDIO_LINE_SESSION_STATE_CREATING:
    case AUDIO_LINE_SESSION_STATE_CREATED:
    case AUDIO_LINE_SESSION_STATE_STARTED:
    case AUDIO_LINE_SESSION_STATE_STOPPING:
    case AUDIO_LINE_SESSION_STATE_STOPPED:
    case AUDIO_LINE_SESSION_STATE_PAUSING:
    case AUDIO_LINE_SESSION_STATE_PAUSED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_LINE_SESSION_STATE_STARTING:
        {
            audio_line->state = state;
            ret = audio_path_start(audio_line->path_handle);
            if (ret == false)
            {
                audio_line->state = AUDIO_LINE_SESSION_STATE_STOPPED;
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_RELEASING:
        {
            audio_line->state = state;
            ret = audio_path_destroy(audio_line->path_handle);
            if (ret == false)
            {
                audio_line->state = AUDIO_LINE_SESSION_STATE_STOPPED;
            }
        }
        break;
    }

    return ret;
}

static bool audio_line_pausing_handler(T_AUDIO_LINE               *audio_line,
                                       T_AUDIO_LINE_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_LINE_SESSION_STATE_RELEASED:
    case AUDIO_LINE_SESSION_STATE_CREATING:
    case AUDIO_LINE_SESSION_STATE_CREATED:
    case AUDIO_LINE_SESSION_STATE_STARTED:
    case AUDIO_LINE_SESSION_STATE_PAUSED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_LINE_SESSION_STATE_STARTING:
        {
            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                audio_line->action = AUDIO_LINE_ACTION_START;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same pausing path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                /* Drop the repeating start action. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will override the pausing state directly.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in pausing state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
            {
                /* When the pausing path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_STOPPING:
        {
            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                /* Override the pausing state. */
                audio_line->state = state;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same pausing path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                /* Cancel all pending actions when the pausing path is pending
                 * for start action, but receives the stop action later.
                 */
                audio_line->action = AUDIO_LINE_ACTION_NONE;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will override the pausing state directly.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in pausing state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
            {
                /* When the pausing path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_STOPPED:
        {
            audio_line->state = AUDIO_LINE_SESSION_STATE_PAUSED;

            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same pausing path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                audio_line->action = AUDIO_LINE_ACTION_NONE;
                ret = audio_line_state_set(audio_line, AUDIO_LINE_SESSION_STATE_STARTING);
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will override the pausing state directly.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in pausing state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
            {
                audio_line->action = AUDIO_LINE_ACTION_NONE;
                ret = audio_line_state_set(audio_line, AUDIO_LINE_SESSION_STATE_RELEASING);
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_PAUSING:
        {
            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                /* Drop the repeating pause action. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same pausing path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                /* Cancel all pending actions when the pausing path is pending
                 * for start action, but receives the pause action later.
                 */
                audio_line->action = AUDIO_LINE_ACTION_NONE;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will override the pausing state directly.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in pausing state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
            {
                /* When the pausing path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_RELEASING:
        {
            if (audio_line->action == AUDIO_LINE_ACTION_NONE)
            {
                audio_line->action = AUDIO_LINE_ACTION_RELEASE;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same pausing path is impossible.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_START)
            {
                /* Override the pending start action for the pausing path. */
                audio_line->action = AUDIO_LINE_ACTION_RELEASE;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_STOP)
            {
                /* Override the pending start action for the pausing path. */
                audio_line->action = AUDIO_LINE_ACTION_RELEASE;
                ret = true;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in pausing state.
                 */
                ret = false;
            }
            else if (audio_line->action == AUDIO_LINE_ACTION_RELEASE)
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

static bool audio_line_paused_handler(T_AUDIO_LINE               *audio_line,
                                      T_AUDIO_LINE_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_LINE_SESSION_STATE_RELEASED:
    case AUDIO_LINE_SESSION_STATE_CREATING:
    case AUDIO_LINE_SESSION_STATE_CREATED:
    case AUDIO_LINE_SESSION_STATE_STOPPED:
    case AUDIO_LINE_SESSION_STATE_PAUSING:
    case AUDIO_LINE_SESSION_STATE_PAUSED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_LINE_SESSION_STATE_STARTING:
        {
            audio_line->state = state;
            ret = audio_path_start(audio_line->path_handle);
            if (ret == false)
            {
                audio_line->state = AUDIO_LINE_SESSION_STATE_PAUSED;
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_STARTED:
        {
            audio_line->state = state;
            audio_line->effect_apply = false;

            audio_line_volume_apply(audio_line);

            if (audio_line->signal_out)
            {
                audio_path_signal_monitoring_set(audio_line->path_handle,
                                                 AUDIO_PATH_DIR_TX,
                                                 true,
                                                 audio_line->signal_out_interval,
                                                 audio_line->freq_out_num,
                                                 audio_line->freq_out_table);
            }

            if (audio_line->signal_in)
            {
                audio_path_signal_monitoring_set(audio_line->path_handle,
                                                 AUDIO_PATH_DIR_RX,
                                                 true,
                                                 audio_line->signal_in_interval,
                                                 audio_line->freq_in_num,
                                                 audio_line->freq_in_table);
            }

            ret = true;
        }
        break;

    case AUDIO_LINE_SESSION_STATE_STOPPING:
        {
            audio_line->state = state;

            ret = audio_path_stop(audio_line->path_handle);
            if (ret == false)
            {
                /* Restore the path state. */
                audio_line->state = AUDIO_LINE_SESSION_STATE_PAUSED;
            }
        }
        break;

    case AUDIO_LINE_SESSION_STATE_RELEASING:
        {
            audio_line->state  = AUDIO_LINE_SESSION_STATE_STOPPING;
            audio_line->action = AUDIO_LINE_ACTION_RELEASE;

            ret = audio_path_stop(audio_line->path_handle);
            if (ret == false)
            {
                /* Restore the path state. */
                audio_line->state = AUDIO_LINE_SESSION_STATE_PAUSED;
                audio_line->action = AUDIO_LINE_ACTION_NONE;
            }
        }
        break;
    }

    return ret;
}

static bool audio_line_releasing_handler(T_AUDIO_LINE               *audio_line,
                                         T_AUDIO_LINE_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_LINE_SESSION_STATE_RELEASED:
        {
            audio_line->state  = state;
            audio_line->action = AUDIO_LINE_ACTION_NONE;

            audio_line_effect_clear(audio_line);
            audio_line_queue_remove(audio_line);

            if (audio_line->freq_in_table != NULL)
            {
                free(audio_line->freq_in_table);
            }

            if (audio_line->freq_out_table != NULL)
            {
                free(audio_line->freq_out_table);
            }

            if (audio_line->channel_out_array != NULL)
            {
                free(audio_line->channel_out_array);
            }

            free(audio_line);
        }
        break;

    case AUDIO_LINE_SESSION_STATE_CREATING:
    case AUDIO_LINE_SESSION_STATE_CREATED:
    case AUDIO_LINE_SESSION_STATE_STARTING:
    case AUDIO_LINE_SESSION_STATE_STARTED:
    case AUDIO_LINE_SESSION_STATE_STOPPING:
    case AUDIO_LINE_SESSION_STATE_STOPPED:
    case AUDIO_LINE_SESSION_STATE_PAUSING:
    case AUDIO_LINE_SESSION_STATE_PAUSED:
        {
            ret = false;
        }
        break;

    case AUDIO_LINE_SESSION_STATE_RELEASING:
        {
            /* Drop the repeating release action. But this case is permitted. */
            ret = true;
        }
        break;
    }

    return ret;
}

static const T_AUDIO_LINE_STATE_HANDLER audio_line_state_handler[] =
{
    audio_line_released_handler,
    audio_line_creating_handler,
    audio_line_created_handler,
    audio_line_starting_handler,
    audio_line_started_handler,
    audio_line_stopping_handler,
    audio_line_stopped_handler,
    audio_line_pausing_handler,
    audio_line_paused_handler,
    audio_line_releasing_handler,
};

static bool audio_line_state_set(T_AUDIO_LINE               *audio_line,
                                 T_AUDIO_LINE_SESSION_STATE  state)
{
    AUDIO_PRINT_INFO4("audio_line_state_set: audio_line %p, curr state %u, next state %u, action %u",
                      audio_line, audio_line->state, state, audio_line->action);

    return audio_line_state_handler[audio_line->state](audio_line, state);
}

static T_AUDIO_LINE *audio_line_find_by_path(T_AUDIO_PATH_HANDLE path_handle)
{
    T_AUDIO_LINE *audio_line;

    audio_line = os_queue_peek(&audio_line_db.lines, 0);
    while (audio_line != NULL)
    {
        if (audio_line->path_handle != NULL &&
            audio_line->path_handle == path_handle)
        {
            return audio_line;
        }

        audio_line = audio_line->next;
    }

    return audio_line;
}

static bool audio_line_path_cback(T_AUDIO_PATH_HANDLE path_handle,
                                  T_AUDIO_PATH_EVENT  event,
                                  uint32_t            param)
{
    T_AUDIO_LINE *audio_line;

    audio_line = audio_line_find_by_path(path_handle);

    AUDIO_PRINT_TRACE3("audio_line_path_cback: handle %p, event 0x%02x, param 0x%08x",
                       path_handle, event, param);

    if (audio_line != NULL)
    {
        switch (event)
        {
        case AUDIO_PATH_EVT_CREATE:
            {
                audio_line_state_set(audio_line, AUDIO_LINE_SESSION_STATE_CREATED);
            }
            break;

        case AUDIO_PATH_EVT_IDLE:
            {
                audio_line_state_set(audio_line, AUDIO_LINE_SESSION_STATE_STOPPED);
            }
            break;

        case AUDIO_PATH_EVT_EFFECT_REQ:
            {
                audio_line->effect_apply = true;
                audio_line_effect_run(audio_line);

                if (audio_line->channel_out_num != 0)
                {
                    audio_path_channel_out_reorder(audio_line->path_handle,
                                                   audio_line->channel_out_num,
                                                   audio_line->channel_out_array);
                }
            }
            break;

        case AUDIO_PATH_EVT_RUNNING:
            {
                audio_line_state_set(audio_line, AUDIO_LINE_SESSION_STATE_STARTED);
            }
            break;

        case AUDIO_PATH_EVT_RELEASE:
            {
                audio_line_state_set(audio_line, AUDIO_LINE_SESSION_STATE_RELEASED);
            }
            break;

        case AUDIO_PATH_EVT_SIGNAL_REFRESH:
            {
                T_AUDIO_EVENT_PARAM msg;
                T_AUDIO_PATH_SIGNAL_REFRESH *p_msg = (T_AUDIO_PATH_SIGNAL_REFRESH *)param;

                if (p_msg->direction == AUDIO_PATH_DIR_TX)
                {
                    msg.line_signal_out_refreshed.handle = audio_line;
                    msg.line_signal_out_refreshed.freq_num = p_msg->freq_num;
                    msg.line_signal_out_refreshed.left_gain_table = p_msg->left_gain_table;
                    msg.line_signal_out_refreshed.right_gain_table = p_msg->right_gain_table;
                    audio_mgr_event_post(AUDIO_EVENT_LINE_SIGNAL_OUT_REFRESHED, &msg, sizeof(msg));
                }
                else if (p_msg->direction == AUDIO_PATH_DIR_RX)
                {
                    msg.line_signal_in_refreshed.handle = audio_line;
                    msg.line_signal_in_refreshed.freq_num = p_msg->freq_num;
                    msg.line_signal_in_refreshed.left_gain_table = p_msg->left_gain_table;
                    msg.line_signal_in_refreshed.right_gain_table = p_msg->right_gain_table;
                    audio_mgr_event_post(AUDIO_EVENT_LINE_SIGNAL_IN_REFRESHED, &msg, sizeof(msg));
                }
            }
            break;

        case AUDIO_PATH_EVT_ERROR_REPORT:
            {
                T_AUDIO_EVENT_PARAM msg;

                msg.line_error_report.handle = audio_line;
                audio_mgr_event_post(AUDIO_EVENT_LINE_ERROR_REPORT, &msg, sizeof(msg));
            }
            break;

        default:
            break;
        }

        return true;
    }

    return false;
}

bool audio_line_init(void)
{
    audio_line_db.volume_out_min        = AUDIO_LINE_DAC_LEVEL_MIN;
    audio_line_db.volume_out_max        = AUDIO_LINE_DAC_LEVEL_MAX;
    audio_line_db.volume_in_min         = AUDIO_LINE_ADC_LEVEL_MIN;
    audio_line_db.volume_in_max         = AUDIO_LINE_ADC_LEVEL_MAX;
    audio_line_db.volume_out_scale      = 0.0f;
    os_queue_init(&audio_line_db.lines);

    return true;
}

void audio_line_deinit(void)
{
    T_AUDIO_LINE *audio_line;

    while ((audio_line = os_queue_out(&audio_line_db.lines)) != NULL)
    {
        T_AUDIO_EFFECT_INSTANCE instance;

        while ((instance = os_queue_out(&audio_line->effects)) != NULL)
        {
            audio_effect_owner_clear(instance);
        }

        if (audio_line->path_handle != NULL)
        {
            audio_path_destroy(audio_line->path_handle);
            audio_line->path_handle = NULL;
        }

        if (audio_line->freq_in_table != NULL)
        {
            free(audio_line->freq_in_table);
        }

        if (audio_line->freq_out_table != NULL)
        {
            free(audio_line->freq_out_table);
        }

        if (audio_line->channel_out_array != NULL)
        {
            free(audio_line->channel_out_array);
        }

        free(audio_line);
    }
}

T_AUDIO_LINE_HANDLE audio_line_create(uint32_t device,
                                      uint32_t sample_rate_in,
                                      uint32_t sample_rate_out)
{
    T_AUDIO_LINE *audio_line;
    int32_t       ret = 0;

    audio_line = calloc(1, sizeof(T_AUDIO_LINE));
    if (audio_line == NULL)
    {
        ret = 1;
        goto fail_alloc_audio_line;
    }

    audio_line->state               = AUDIO_LINE_SESSION_STATE_RELEASED;
    audio_line->action              = AUDIO_LINE_ACTION_NONE;
    audio_line->volume_out_min      = audio_line_db.volume_out_min;
    audio_line->volume_out_max      = audio_line_db.volume_out_max;
    audio_line->volume_out          = AUDIO_LINE_DAC_LEVEL_DEFAULT;
    audio_line->volume_in_min       = audio_line_db.volume_in_min;
    audio_line->volume_in_max       = audio_line_db.volume_in_max;
    audio_line->volume_in           = AUDIO_LINE_ADC_LEVEL_DEFAULT;
    audio_line->signal_in           = false;
    audio_line->signal_out          = false;
    audio_line->signal_in_interval  = 0;
    audio_line->signal_out_interval = 0;
    audio_line->freq_in_num         = 0;
    audio_line->freq_out_num        = 0;
    audio_line->freq_in_table       = NULL;
    audio_line->freq_out_table      = NULL;
    audio_line->effect_apply        = false;
    audio_line->path_handle         = NULL;
    audio_line->device              = device;
    audio_line->sample_rate_in      = sample_rate_in;
    audio_line->sample_rate_out     = sample_rate_out;
    audio_line->channel_out_num     = 0;
    audio_line->channel_out_array   = NULL;
    os_queue_init(&audio_line->effects);

    if (audio_line_state_set(audio_line, AUDIO_LINE_SESSION_STATE_CREATING) == false)
    {
        ret = 2;
        goto fail_set_state;
    }

    return (T_AUDIO_LINE_HANDLE)audio_line;

fail_set_state:
    free(audio_line);
fail_alloc_audio_line:
    AUDIO_PRINT_ERROR1("audio_line_create: failed %d", -ret);
    return NULL;
}

bool audio_line_release(T_AUDIO_LINE_HANDLE handle)
{
    T_AUDIO_LINE *audio_line;
    int32_t       ret = 0;

    audio_line = (T_AUDIO_LINE *)handle;
    if (audio_line_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    if (audio_line_state_set(audio_line, AUDIO_LINE_SESSION_STATE_RELEASING) == false)
    {
        ret = 2;
        goto fail_set_state;
    }

    return true;

fail_set_state:
fail_invalid_handle:
    AUDIO_PRINT_ERROR2("audio_line_release: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_line_start(T_AUDIO_LINE_HANDLE handle)
{
    T_AUDIO_LINE *audio_line;
    int32_t       ret = 0;

    audio_line = (T_AUDIO_LINE *)handle;;
    if (audio_line_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    if (audio_line_state_set(audio_line, AUDIO_LINE_SESSION_STATE_STARTING) == false)
    {
        ret = 2;
        goto fail_set_state;
    }

    return true;

fail_set_state:
fail_invalid_handle:
    AUDIO_PRINT_ERROR2("audio_line_start: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_line_stop(T_AUDIO_LINE_HANDLE handle)
{
    T_AUDIO_LINE *audio_line;
    int32_t       ret = 0;

    audio_line = (T_AUDIO_LINE *)handle;
    if (audio_line_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    if (audio_line_state_set(audio_line, AUDIO_LINE_SESSION_STATE_STOPPING) == false)
    {
        ret = 2;
        goto fail_set_state;
    }

    return true;

fail_set_state:
fail_invalid_handle:
    AUDIO_PRINT_ERROR2("audio_line_stop: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_line_state_get(T_AUDIO_LINE_HANDLE  handle,
                          T_AUDIO_LINE_STATE  *state)
{
    T_AUDIO_LINE *audio_line;

    audio_line = (T_AUDIO_LINE *)handle;
    if (audio_line_handle_check(handle) == true)
    {
        switch (audio_line->state)
        {
        case AUDIO_LINE_SESSION_STATE_RELEASED:
        case AUDIO_LINE_SESSION_STATE_RELEASING:
            *state = AUDIO_LINE_STATE_RELEASED;
            break;

        case AUDIO_LINE_SESSION_STATE_CREATING:
        case AUDIO_LINE_SESSION_STATE_CREATED:
            *state = AUDIO_LINE_STATE_CREATED;
            break;

        case AUDIO_LINE_SESSION_STATE_STARTING:
        case AUDIO_LINE_SESSION_STATE_STARTED:
            *state = AUDIO_LINE_STATE_STARTED;
            break;

        case AUDIO_LINE_SESSION_STATE_STOPPING:
        case AUDIO_LINE_SESSION_STATE_STOPPED:
            *state = AUDIO_LINE_STATE_STOPPED;
            break;

        case AUDIO_LINE_SESSION_STATE_PAUSING:
        case AUDIO_LINE_SESSION_STATE_PAUSED:
            *state = AUDIO_LINE_STATE_PAUSED;
            break;
        }

        return true;
    }

    return false;
}

static void audio_line_volume_apply(T_AUDIO_LINE *audio_line)
{
    audio_path_dac_level_set(audio_line->path_handle,
                             audio_line->volume_out,
                             audio_line_db.volume_out_scale);
    audio_path_adc_level_set(audio_line->path_handle,
                             audio_line->volume_in,
                             0.0f);
}

uint8_t audio_line_volume_out_max_get(void)
{
    return audio_line_db.volume_out_max;
}

bool audio_line_volume_out_max_set(uint8_t level)
{
    audio_line_db.volume_out_max = level;
    return true;
}

uint8_t audio_line_volume_out_min_get(void)
{
    return audio_line_db.volume_out_min;
}

bool audio_line_volume_out_min_set(uint8_t volume)
{
    audio_line_db.volume_out_min = volume;
    return true;
}

bool audio_line_volume_out_get(T_AUDIO_LINE_HANDLE  handle,
                               uint8_t             *volume)
{
    T_AUDIO_LINE *audio_line;

    audio_line = (T_AUDIO_LINE *)handle;
    if (audio_line != NULL)
    {
        *volume = audio_line->volume_out;
        return true;
    }

    return false;
}

bool audio_line_volume_out_set(T_AUDIO_LINE_HANDLE handle,
                               uint8_t             volume)
{
    T_AUDIO_LINE *audio_line;
    T_AUDIO_EVENT_PARAM param;
    int32_t ret = 0;

    audio_line = (T_AUDIO_LINE *)handle;
    if (audio_line_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    if (volume < audio_line->volume_out_min ||
        volume > audio_line->volume_out_max)
    {
        ret = 2;
        goto fail_check_volume;
    }

    if (audio_path_dac_level_set(audio_line->path_handle, volume,
                                 audio_line_db.volume_out_scale) == false)
    {
        ret = 3;
        goto fail_set_volume;
    }

    param.line_volume_out_changed.handle      = audio_line;
    param.line_volume_out_changed.prev_volume = audio_line->volume_out;
    param.line_volume_out_changed.curr_volume = volume;

    audio_line->volume_out = volume;

    return audio_mgr_event_post(AUDIO_EVENT_LINE_VOLUME_OUT_CHANGED, &param, sizeof(param));

fail_set_volume:
fail_check_volume:
fail_invalid_handle:
    AUDIO_PRINT_ERROR3("audio_line_volume_out_set: handle %p, volume %u, failed %d",
                       handle, volume, -ret);
    return false;
}

uint8_t audio_line_volume_in_max_get(void)
{
    return audio_line_db.volume_in_max;
}

bool audio_line_volume_in_max_set(uint8_t volume)
{
    audio_line_db.volume_in_max = volume;
    return true;
}

uint8_t audio_line_volume_in_min_get(void)
{
    return audio_line_db.volume_in_min;
}

bool audio_line_volume_in_min_set(uint8_t volume)
{
    audio_line_db.volume_in_min = volume;
    return true;
}

bool audio_line_volume_in_get(T_AUDIO_LINE_HANDLE  handle,
                              uint8_t             *volume)
{
    T_AUDIO_LINE *audio_line;

    audio_line = (T_AUDIO_LINE *)handle;
    if (audio_line != NULL)
    {
        *volume = audio_line->volume_in;
        return true;
    }

    return false;
}

bool audio_line_volume_in_set(T_AUDIO_LINE_HANDLE handle,
                              uint8_t             volume)
{
    T_AUDIO_LINE *audio_line;
    T_AUDIO_EVENT_PARAM param;
    int32_t ret = 0;

    audio_line = (T_AUDIO_LINE *)handle;
    if (audio_line_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    if (volume < audio_line_db.volume_in_min ||
        volume > audio_line_db.volume_in_max)
    {
        ret = 2;
        goto fail_check_volume;
    }

    if (audio_path_adc_level_set(audio_line->path_handle, volume, 0.0f) == false)
    {
        ret = 3;
        goto fail_set_volume;
    }

    param.line_volume_in_changed.handle      = audio_line;
    param.line_volume_in_changed.prev_volume = audio_line->volume_in;
    param.line_volume_in_changed.curr_volume = volume;

    audio_line->volume_in = volume;

    return audio_mgr_event_post(AUDIO_EVENT_LINE_VOLUME_IN_CHANGED, &param, sizeof(param));

fail_set_volume:
fail_check_volume:
fail_invalid_handle:
    AUDIO_PRINT_ERROR3("audio_line_volume_in_set: handle %p, volume %u, failed %d",
                       handle, volume, -ret);
    return false;
}

bool audio_line_channel_out_set(T_AUDIO_LINE_HANDLE handle,
                                uint8_t             channel_num,
                                uint32_t            channel_array[])
{
    T_AUDIO_LINE *audio_line;
    int32_t       ret = 0;

    audio_line = (T_AUDIO_LINE *)handle;
    if (audio_line_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    if (channel_num == 0)
    {
        ret = 2;
        goto fail_check_num;
    }

    if (audio_line->state == AUDIO_LINE_SESSION_STATE_STARTED)
    {
        if (audio_path_channel_out_reorder(audio_line->path_handle,
                                           channel_num,
                                           channel_array) == false)
        {
            ret = 3;
            goto fail_reorder_channel;
        }
    }

    audio_line->channel_out_num   = channel_num;
    audio_line->channel_out_array = malloc(4 * audio_line->channel_out_num);
    if (audio_line->channel_out_array == NULL)
    {
        ret = 4;
        goto fail_alloc_array;
    }

    memcpy(audio_line->channel_out_array, channel_array, 4 * channel_num);
    return true;

fail_alloc_array:
fail_reorder_channel:
fail_check_num:
fail_invalid_handle:
    AUDIO_PRINT_ERROR2("audio_line_channel_out_set: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_line_signal_out_monitoring_start(T_AUDIO_LINE_HANDLE handle,
                                            uint16_t            refresh_interval,
                                            uint8_t             freq_num,
                                            uint32_t            freq_table[])
{
    T_AUDIO_LINE *audio_line;
    int32_t       ret = 0;

    audio_line = (T_AUDIO_LINE *)handle;
    if (audio_line_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    if (audio_line->signal_out == true)
    {
        ret = 2;
        goto fail_check_signal_out;
    }

    if (audio_line->state == AUDIO_LINE_SESSION_STATE_STARTED)
    {
        if (audio_path_signal_monitoring_set(audio_line->path_handle,
                                             AUDIO_PATH_DIR_TX,
                                             true,
                                             refresh_interval,
                                             freq_num,
                                             freq_table) == false)
        {
            ret = 3;
            goto fail_set_signal;
        }
    }

    audio_line->signal_out          = true;
    audio_line->signal_out_interval = refresh_interval;
    audio_line->freq_out_num        = freq_num;
    audio_line->freq_out_table      = malloc(audio_line->freq_out_num * 4);
    if (audio_line->freq_out_table == NULL)
    {
        ret = 4;
        goto fail_alloc_freq_table;
    }

    memcpy(audio_line->freq_out_table, freq_table, 4 * freq_num);

    return true;

fail_alloc_freq_table:
fail_set_signal:
fail_check_signal_out:
fail_invalid_handle:
    AUDIO_PRINT_ERROR2("audio_line_signal_out_monitoring_start: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_line_signal_out_monitoring_stop(T_AUDIO_LINE_HANDLE handle)
{
    T_AUDIO_LINE *audio_line;
    int32_t       ret = 0;

    audio_line = (T_AUDIO_LINE *)handle;
    if (audio_line_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    if (audio_line->signal_out == false)
    {
        ret = 2;
        goto fail_check_signal_out;
    }

    if (audio_path_signal_monitoring_set(audio_line->path_handle,
                                         AUDIO_PATH_DIR_TX,
                                         false,
                                         0,
                                         0,
                                         NULL) == false)
    {
        ret = 3;
        goto fail_set_signal;
    }

    audio_line->signal_out          = false;
    audio_line->signal_out_interval = 0;
    audio_line->freq_out_num        = 0;

    if (audio_line->freq_out_table != NULL)
    {
        free(audio_line->freq_out_table);
        audio_line->freq_out_table = NULL;
    }

    return true;

fail_set_signal:
fail_check_signal_out:
fail_invalid_handle:
    AUDIO_PRINT_ERROR2("audio_line_signal_out_monitoring_stop: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_line_signal_in_monitoring_start(T_AUDIO_LINE_HANDLE handle,
                                           uint16_t            refresh_interval,
                                           uint8_t             freq_num,
                                           uint32_t            freq_table[])
{
    T_AUDIO_LINE *audio_line;
    int32_t       ret = 0;

    audio_line = (T_AUDIO_LINE *)handle;
    if (audio_line_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    if (audio_line->signal_in == true)
    {
        ret = 2;
        goto fail_check_signal_in;
    }

    if (audio_line->state == AUDIO_LINE_SESSION_STATE_STARTED)
    {
        if (audio_path_signal_monitoring_set(audio_line->path_handle,
                                             AUDIO_PATH_DIR_RX,
                                             true,
                                             refresh_interval,
                                             freq_num,
                                             freq_table) == false)
        {
            ret = 3;
            goto fail_set_signal;
        }
    }

    audio_line->signal_in          = true;
    audio_line->signal_in_interval = refresh_interval;
    audio_line->freq_in_num        = freq_num;
    audio_line->freq_in_table      = malloc(audio_line->freq_in_num * 4);
    if (audio_line->freq_in_table == NULL)
    {
        ret = 4;
        goto fail_alloc_freq_table;
    }

    memcpy(audio_line->freq_in_table, freq_table, 4 * freq_num);

    return true;

fail_alloc_freq_table:
fail_set_signal:
fail_check_signal_in:
fail_invalid_handle:
    AUDIO_PRINT_ERROR2("audio_line_signal_in_monitoring_start: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_line_signal_in_monitoring_stop(T_AUDIO_LINE_HANDLE handle)
{
    T_AUDIO_LINE *audio_line;
    int32_t       ret = 0;

    audio_line = (T_AUDIO_LINE *)handle;
    if (audio_line_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    if (audio_line->signal_in == false)
    {
        ret = 2;
        goto fail_check_signal_in;
    }

    if (audio_path_signal_monitoring_set(audio_line->path_handle,
                                         AUDIO_PATH_DIR_RX,
                                         false,
                                         0,
                                         0,
                                         NULL) == false)
    {
        ret = 3;
        goto fail_set_signal;
    }

    audio_line->signal_in          = false;
    audio_line->signal_in_interval = 0;
    audio_line->freq_in_num        = 0;

    if (audio_line->freq_in_table != NULL)
    {
        free(audio_line->freq_in_table);
        audio_line->freq_in_table = NULL;
    }

    return true;

fail_set_signal:
fail_check_signal_in:
fail_invalid_handle:
    AUDIO_PRINT_ERROR2("audio_line_signal_in_monitoring_stop: handle %p, failed %d",
                       handle, -ret);
    return false;
}

float audio_line_volume_balance_get(void)
{
    return audio_line_db.volume_out_scale;
}

bool audio_line_volume_balance_set(float scale)
{
    T_OS_QUEUE    *queue;
    T_AUDIO_LINE  *audio_line;
    T_AUDIO_EVENT_PARAM param;
    int32_t ret = 0;

    if (scale < -1.0f ||
        scale > 1.0f)
    {
        ret = 1;
        goto fail_check_scale;
    }

    queue = &audio_line_db.lines;
    if (queue != NULL)
    {
        audio_line = os_queue_peek(queue, 0);
        while (audio_line != NULL)
        {
            if (audio_path_dac_level_set(audio_line->path_handle, audio_line->volume_out, scale) == false)
            {
                ret = 2;
                goto fail_set_scale;
            }
            audio_line = audio_line->next;
        }
    }

    param.line_volume_balance_changed.prev_scale = audio_line_db.volume_out_scale;
    param.line_volume_balance_changed.curr_scale = scale;

    audio_line_db.volume_out_scale = scale;

    return audio_mgr_event_post(AUDIO_EVENT_LINE_VOLUME_BALANCE_CHANGED, &param, sizeof(param));

fail_set_scale:
fail_check_scale:
    AUDIO_PRINT_ERROR2("audio_line_volume_balance_set: scale %d/1000, failed %d",
                       (int32_t)(scale * 1000), -ret);
    return false;
}

static bool audio_line_effect_check(T_AUDIO_LINE            *audio_line,
                                    T_AUDIO_EFFECT_INSTANCE  instance)
{
    if (instance != NULL)
    {
        return os_queue_search(&audio_line->effects, instance);
    }

    return false;
}

static bool audio_line_effect_enqueue(T_AUDIO_LINE            *audio_line,
                                      T_AUDIO_EFFECT_INSTANCE  instance)
{
    if (instance != NULL)
    {
        if (os_queue_search(&audio_line->effects, instance) == false)
        {
            os_queue_in(&audio_line->effects, instance);
        }

        return true;
    }

    return false;
}

static bool audio_line_effect_dequeue(T_AUDIO_LINE            *audio_line,
                                      T_AUDIO_EFFECT_INSTANCE  instance)
{
    if (instance != NULL)
    {
        return os_queue_delete(&audio_line->effects, instance);
    }

    return false;
}

static bool audio_line_effect_run(T_AUDIO_LINE *audio_line)
{
    T_AUDIO_EFFECT_INSTANCE instance;
    int32_t                 i = 0;

    while ((instance = os_queue_peek(&audio_line->effects, i)) != NULL)
    {
        audio_effect_run(instance);
        i++;
    }

    return true;
}

static bool audio_line_effect_stop(T_AUDIO_LINE *audio_line)
{
#if 0
    T_AUDIO_EFFECT_INSTANCE instance;
    int32_t                 i = 0;

    while ((instance = os_queue_peek(&audio_line->effects, i)) != NULL)
    {
        audio_effect_stop(instance);
        i++;
    }
#endif
    return true;
}

static bool audio_line_effect_clear(T_AUDIO_LINE *audio_line)
{
    T_AUDIO_EFFECT_INSTANCE instance;

    while ((instance = os_queue_out(&audio_line->effects)) != NULL)
    {
        audio_effect_owner_clear(instance);
    }

    return true;
}

static void audio_line_effect_cback(T_AUDIO_EFFECT_INSTANCE  instance,
                                    T_AUDIO_EFFECT_EVENT     event,
                                    void                    *context)
{
    T_AUDIO_PATH_HANDLE handle;
    T_AUDIO_LINE       *audio_line;

    handle = audio_effect_owner_get(instance);
    audio_line = audio_line_find_by_path(handle);

    AUDIO_PRINT_TRACE3("audio_line_effect_cback: instance %p, handle %p, event 0x%02x",
                       instance, handle, event);

    if (audio_line_handle_check(audio_line) == true)
    {
        switch (event)
        {
        case AUDIO_EFFECT_EVENT_CREATED:
            break;

        case AUDIO_EFFECT_EVENT_ENABLED:
            if (audio_line->state == AUDIO_LINE_SESSION_STATE_STARTED)
            {
                audio_effect_run(instance);
            }
            else if (audio_line->state == AUDIO_LINE_SESSION_STATE_STARTING)
            {
                if (audio_line->effect_apply == true)
                {
                    audio_effect_run(instance);
                }
            }
            break;

        case AUDIO_EFFECT_EVENT_DISABLED:
            if (audio_line->state == AUDIO_LINE_SESSION_STATE_STARTED)
            {
                audio_effect_stop(instance);
            }
            else if (audio_line->state == AUDIO_LINE_SESSION_STATE_STARTING ||
                     audio_line->state == AUDIO_LINE_SESSION_STATE_PAUSED)
            {
                if (audio_line->effect_apply == true)
                {
                    audio_effect_stop(instance);
                }
            }
            break;

        case AUDIO_EFFECT_EVENT_UPDATED:
            if (audio_line->state == AUDIO_LINE_SESSION_STATE_STARTED)
            {
                audio_effect_flush(instance, context);
            }
            else if (audio_line->state == AUDIO_LINE_SESSION_STATE_STARTING)
            {
                if (audio_line->effect_apply == true)
                {
                    audio_effect_flush(instance, context);
                }
            }
            break;

        case AUDIO_EFFECT_EVENT_RELEASED:
            if (audio_line->state == AUDIO_LINE_SESSION_STATE_STARTED)
            {
                audio_effect_stop(instance);
            }
            else if (audio_line->state == AUDIO_LINE_SESSION_STATE_STARTING)
            {
                if (audio_line->effect_apply == true)
                {
                    audio_effect_stop(instance);
                }
            }

            audio_line_effect_dequeue(audio_line, instance);
            audio_effect_owner_clear(instance);
            break;
        }
    }
}

bool audio_line_effect_attach(T_AUDIO_LINE_HANDLE     handle,
                              T_AUDIO_EFFECT_INSTANCE instance)
{
    T_AUDIO_LINE *audio_line;
    int32_t       ret = 0;

    audio_line = (T_AUDIO_LINE *)handle;
    if (audio_line_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    if (audio_line_effect_enqueue(audio_line, instance) == false)
    {
        ret = 2;
        goto fail_enqueue_effect;
    }

    if (audio_effect_owner_set(instance,
                               audio_line->path_handle,
                               audio_line_effect_cback) == false)
    {
        ret = 3;
        goto fail_set_owner;
    }

    if (audio_line->state == AUDIO_LINE_SESSION_STATE_STARTED)
    {
        audio_effect_run(instance);
    }

    return true;

fail_set_owner:
    audio_line_effect_dequeue(audio_line, instance);
fail_enqueue_effect:
fail_invalid_handle:
    AUDIO_PRINT_ERROR2("audio_line_effect_attach: instance %p, failed %d",
                       instance, -ret);
    return false;
}

bool audio_line_effect_detach(T_AUDIO_LINE_HANDLE     handle,
                              T_AUDIO_EFFECT_INSTANCE instance)
{
    T_AUDIO_LINE *audio_line;
    int32_t       ret = 0;

    audio_line = (T_AUDIO_LINE *)handle;
    if (audio_line_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    if (audio_line_effect_check(audio_line, instance) == false)
    {
        ret = 2;
        goto fail_check_instance;
    }

    if (audio_line->state == AUDIO_LINE_SESSION_STATE_STARTED)
    {
        audio_effect_stop(instance);
    }

    if (audio_line_effect_dequeue(audio_line, instance) == false)
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
    audio_line_effect_enqueue(audio_line, instance);
fail_dequeue_effect:
fail_check_instance:
fail_invalid_handle:
    AUDIO_PRINT_ERROR2("audio_line_effect_detach: instance %p, failed %d",
                       instance, -ret);
    return false;
}

bool audio_line_route_path_take(T_AUDIO_LINE_HANDLE  handle,
                                uint32_t             device,
                                T_AUDIO_ROUTE_PATH  *path)
{
    T_AUDIO_LINE *audio_line;
    int32_t       ret = 0;

    audio_line = (T_AUDIO_LINE *)handle;

    if (audio_line_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    *path = audio_route_path_get(audio_line->path_handle,
                                 AUDIO_CATEGORY_LINE,
                                 device);
    if (path->entry == NULL)
    {
        ret = 2;
        goto fail_get_route;
    }

    return true;

fail_get_route:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_line_route_path_take: handle %p, failed %d",
                       handle, -ret);
    return false;
}

void audio_line_route_path_give(T_AUDIO_ROUTE_PATH *path)
{
    audio_route_path_give(path);
}

bool audio_line_route_bind(T_AUDIO_LINE_HANDLE handle,
                           T_AUDIO_ROUTE_PATH  path)
{
    T_AUDIO_LINE *line;
    int32_t       ret = 0;

    line = (T_AUDIO_LINE *)handle;

    if (audio_line_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (audio_route_path_bind(line->path_handle,
                              path) == false)
    {
        ret = 2;
        goto fail_bind_path;
    }

    return true;

fail_bind_path:
fail_check_handle:
    AUDIO_PRINT_ERROR3("audio_line_route_bind: handle %p, entry_num %d, failed %d",
                       handle, path.entry_num, -ret);
    return false;
}

bool audio_line_route_unbind(T_AUDIO_LINE_HANDLE handle)
{
    T_AUDIO_LINE *line;
    int32_t       ret = 0;

    line = (T_AUDIO_LINE *)handle;

    if (audio_line_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (audio_route_path_unbind(line->path_handle) == false)
    {
        ret = 2;
        goto fail_clear_owner;
    }

    return true;

fail_clear_owner:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_line_route_unbind: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_line_route_add(T_AUDIO_LINE_HANDLE  handle,
                          T_AUDIO_ROUTE_ENTRY *entry)
{
    T_AUDIO_LINE *line;
    int32_t       ret = 0;

    line = (T_AUDIO_LINE *)handle;

    if (audio_line_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (audio_route_path_add(line->path_handle,
                             entry) == false)
    {
        ret = 2;
        goto fail_add_entry;
    }

    return true;

fail_add_entry:
fail_check_handle:
    AUDIO_PRINT_ERROR3("audio_line_route_add: track_handle %p, entry %p, failed %d",
                       handle, entry, -ret);
    return false;
}

bool audio_line_route_remove(T_AUDIO_LINE_HANDLE   handle,
                             T_AUDIO_ROUTE_IO_TYPE io_type)
{
    T_AUDIO_LINE *line;
    int32_t       ret = 0;

    line = (T_AUDIO_LINE *)handle;

    if (audio_line_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (audio_route_path_remove(line->path_handle,
                                io_type) == false)
    {
        ret = 2;
        goto fail_remove_entry;
    }

    return true;

fail_remove_entry:
fail_check_handle:
    AUDIO_PRINT_ERROR3("audio_line_route_remove: track_handle %p, io_type 0x%02x, failed %d",
                       handle, io_type, -ret);
    return false;
}

#else
bool audio_line_init(void)
{
    /* Let Audio Line initialization pass. */
    return true;
}

void audio_line_deinit(void)
{

}

T_AUDIO_LINE_HANDLE audio_line_create(uint32_t device,
                                      uint32_t sample_rate_in,
                                      uint32_t sample_rate_out)
{
    return NULL;
}


bool audio_line_release(T_AUDIO_LINE_HANDLE handle)
{
    return false;
}

bool audio_line_start(T_AUDIO_LINE_HANDLE handle)
{
    return false;
}

bool audio_line_stop(T_AUDIO_LINE_HANDLE handle)
{
    return false;
}

bool audio_line_state_get(T_AUDIO_LINE_HANDLE  handle,
                          T_AUDIO_LINE_STATE  *state)
{
    return false;
}

uint8_t audio_line_volume_out_max_get(void)
{
    return 0;
}

bool audio_line_volume_out_max_set(uint8_t level)
{
    return false;
}

uint8_t audio_line_volume_out_min_get(void)
{
    return 0;
}

bool audio_line_volume_out_min_set(uint8_t volume)
{
    return false;
}

bool audio_line_volume_out_get(T_AUDIO_LINE_HANDLE  handle,
                               uint8_t             *volume)
{
    return false;
}

bool audio_line_volume_out_set(T_AUDIO_LINE_HANDLE handle,
                               uint8_t             volume)
{
    return false;
}

uint8_t audio_line_volume_in_max_get(void)
{
    return 0;
}

bool audio_line_volume_in_max_set(uint8_t volume)
{
    return false;
}

uint8_t audio_line_volume_in_min_get(void)
{
    return 0;
}

bool audio_line_volume_in_min_set(uint8_t volume)
{
    return false;
}

bool audio_line_volume_in_get(T_AUDIO_LINE_HANDLE  handle,
                              uint8_t             *volume)
{
    return false;
}

bool audio_line_volume_in_set(T_AUDIO_LINE_HANDLE handle,
                              uint8_t             volume)
{
    return false;
}

bool audio_line_channel_out_set(T_AUDIO_LINE_HANDLE handle,
                                uint8_t             channel_num,
                                uint32_t            channel_array[])
{
    return false;
}

bool audio_line_signal_out_monitoring_start(T_AUDIO_LINE_HANDLE handle,
                                            uint16_t            refresh_interval,
                                            uint8_t             freq_num,
                                            uint32_t            freq_table[])
{
    return false;
}

bool audio_line_signal_out_monitoring_stop(T_AUDIO_LINE_HANDLE handle)
{
    return false;
}

bool audio_line_signal_in_monitoring_start(T_AUDIO_LINE_HANDLE handle,
                                           uint16_t            refresh_interval,
                                           uint8_t             freq_num,
                                           uint32_t            freq_table[])
{
    return false;
}

bool audio_line_signal_in_monitoring_stop(T_AUDIO_LINE_HANDLE handle)
{
    return false;
}

float audio_line_volume_balance_get(void)
{
    return 0.0f;
}

bool audio_line_volume_balance_set(float scale)
{
    return false;
}

bool audio_line_effect_attach(T_AUDIO_LINE_HANDLE     handle,
                              T_AUDIO_EFFECT_INSTANCE instance)
{
    return false;
}

bool audio_line_effect_detach(T_AUDIO_LINE_HANDLE     handle,
                              T_AUDIO_EFFECT_INSTANCE instance)
{
    return false;
}

bool audio_line_route_path_take(T_AUDIO_TRACK_HANDLE  handle,
                                uint32_t              device,
                                T_AUDIO_ROUTE_PATH   *path)
{
    return false;
}

void audio_line_route_path_give(T_AUDIO_ROUTE_PATH *path)
{
    return;
}

bool audio_line_route_bind(T_AUDIO_LINE_HANDLE handle,
                           T_AUDIO_ROUTE_PATH  path)
{
    return false;
}

bool audio_line_route_unbind(T_AUDIO_LINE_HANDLE handle)
{
    return false;
}

bool audio_line_route_add(T_AUDIO_LINE_HANDLE  handle,
                          T_AUDIO_ROUTE_ENTRY *entry)
{
    return false;
}

bool audio_line_route_remove(T_AUDIO_LINE_HANDLE   handle,
                             T_AUDIO_ROUTE_IO_TYPE io_type)
{
    return false;
}

#endif /* CONFIG_REALTEK_AM_AUDIO_LINE_SUPPORT */
