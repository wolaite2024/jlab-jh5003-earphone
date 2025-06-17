/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "os_queue.h"
#include "trace.h"
#include "media_buffer.h"
#include "audio_type.h"
#include "audio_mgr.h"
#include "audio_path.h"
#include "audio_effect.h"
#include "audio_track.h"

#define AUDIO_TRACK_PLAYBACK_DEFAULT_LATENCY  280
#define AUDIO_TRACK_VOICE_DEFAULT_LATENCY     100

typedef enum t_audio_session_state
{
    AUDIO_SESSION_STATE_RELEASED    = 0x00,
    AUDIO_SESSION_STATE_CREATING    = 0x01,
    AUDIO_SESSION_STATE_CREATED     = 0x02,
    AUDIO_SESSION_STATE_STARTING    = 0x03,
    AUDIO_SESSION_STATE_STARTED     = 0x04,
    AUDIO_SESSION_STATE_STOPPING    = 0x05,
    AUDIO_SESSION_STATE_STOPPED     = 0x06,
    AUDIO_SESSION_STATE_PAUSING     = 0x07,
    AUDIO_SESSION_STATE_PAUSED      = 0x08,
    AUDIO_SESSION_STATE_RESTARTING  = 0x09,
    AUDIO_SESSION_STATE_RELEASING   = 0x0a,
} T_AUDIO_SESSION_STATE;

typedef enum t_audio_track_action
{
    AUDIO_TRACK_ACTION_NONE     = 0x00,
    AUDIO_TRACK_ACTION_CREATE   = 0x01,
    AUDIO_TRACK_ACTION_START    = 0x02,
    AUDIO_TRACK_ACTION_STOP     = 0x03,
    AUDIO_TRACK_ACTION_PAUSE    = 0x04,
    AUDIO_TRACK_ACTION_RESTART  = 0x05,
    AUDIO_TRACK_ACTION_RELEASE  = 0x06,
} T_AUDIO_TRACK_ACTION;

typedef struct t_audio_track
{
    struct t_audio_track   *p_next;
    void                   *container;
    T_AUDIO_SESSION_STATE   state;
    T_AUDIO_TRACK_ACTION    action;
    T_AUDIO_STREAM_TYPE     stream_type;
    T_AUDIO_FORMAT_INFO     format_info;
    uint32_t                device;
    T_AUDIO_STREAM_MODE     mode;
    T_AUDIO_STREAM_USAGE    usage;
    uint16_t                latency;
    bool                    latency_fixed;
    uint8_t                 volume_out_max;
    uint8_t                 volume_out_min;
    bool                    volume_out_muted;
    uint8_t                 volume_out;
    uint8_t                 volume_in_max;
    uint8_t                 volume_in_min;
    bool                    volume_in_muted;
    uint8_t                 volume_in;
    P_AUDIO_TRACK_ASYNC_IO  async_write;
    P_AUDIO_TRACK_ASYNC_IO  async_read;
    T_AUDIO_PATH_HANDLE     path_handle;
    T_MEDIA_BUFFER_PROXY    buffer_proxy;
    T_OS_QUEUE              effects;
    bool                    effect_apply;
    uint8_t                 effect_bypass;
    bool                    signal_in;
    bool                    signal_out;
    uint16_t                signal_in_interval;
    uint16_t                signal_out_interval;
    uint32_t               *channel_in_array;
    uint32_t               *channel_out_array;
    uint8_t                 channel_in_num;
    uint8_t                 channel_out_num;
    uint8_t                 freq_in_num;
    uint8_t                 freq_out_num;
    uint32_t               *freq_in_table;
    uint32_t               *freq_out_table;
} T_AUDIO_TRACK;

typedef struct t_audio_track_db
{
    uint8_t        playback_volume_max;
    uint8_t        playback_volume_min;
    uint8_t        playback_volume_default;
    bool           playback_volume_muted;
    float          playback_volume_scale;
    uint8_t        voice_volume_out_max;
    uint8_t        voice_volume_out_min;
    uint8_t        voice_volume_out_default;
    bool           voice_volume_out_muted;
    float          voice_volume_out_scale;
    uint8_t        voice_volume_in_max;
    uint8_t        voice_volume_in_min;
    uint8_t        voice_volume_in_default;
    bool           voice_volume_in_muted;
    uint8_t        record_volume_max;
    uint8_t        record_volume_min;
    uint8_t        record_volume_default;
    bool           record_volume_muted;
    bool           multi_instance_enable;
    T_OS_QUEUE     idle_tracks;
    T_OS_QUEUE     playback_tracks;
    T_OS_QUEUE     voice_tracks;
    T_OS_QUEUE     record_tracks;
    T_AUDIO_TRACK *playback_buffer_owner;
    T_AUDIO_TRACK *voice_buffer_owner;
    T_AUDIO_TRACK *record_buffer_owner;
} T_AUDIO_TRACK_DB;

typedef bool (*T_AUDIO_TRACK_STATE_HANDLER)(T_AUDIO_TRACK         *track,
                                            T_AUDIO_SESSION_STATE  state);

static T_AUDIO_TRACK_DB audio_track_db;

static bool audio_session_state_set(T_AUDIO_TRACK         *track,
                                    T_AUDIO_SESSION_STATE  state);

static T_AUDIO_CATEGORY audio_track_path_category_convert(T_AUDIO_STREAM_TYPE stream_type)
{
    T_AUDIO_CATEGORY path_category;

    switch (stream_type)
    {
    case AUDIO_STREAM_TYPE_PLAYBACK:
        path_category = AUDIO_CATEGORY_AUDIO;
        break;

    case AUDIO_STREAM_TYPE_VOICE:
        path_category = AUDIO_CATEGORY_VOICE;
        break;

    case AUDIO_STREAM_TYPE_RECORD:
        path_category = AUDIO_CATEGORY_RECORD;
        break;
    }

    return path_category;
}

static bool audio_track_queue_add(T_OS_QUEUE    *queue,
                                  T_AUDIO_TRACK *track,
                                  bool           high_priority)
{
    if (track->container != NULL)
    {
        /* Delete the track from its original track queue. */
        if (os_queue_delete(track->container, track) == false)
        {
            /* Track is not in its container queue. This shall not happen, anyway. */
            return false;
        }
    }

    track->container = queue;

    /* Insert the track as the first element of the queue if it's high priority;
     * otherwise, append the track into the queue.
     */
    if (high_priority == true)
    {
        os_queue_insert(track->container, NULL, track);
    }
    else
    {
        os_queue_in(track->container, track);
    }

    return true;
}

static bool audio_track_queue_remove(T_AUDIO_TRACK *track)
{
    if (track->container != NULL)
    {
        if (os_queue_delete(track->container, track) == false)
        {
            /* Track is not in its container queue. This shall not happen, anyway. */
            return false;
        }

        track->container = NULL;
    }

    return true;
}

static bool audio_track_is_starting(T_AUDIO_STREAM_TYPE type)
{
    T_AUDIO_TRACK *track;

    switch (type)
    {
    case AUDIO_STREAM_TYPE_PLAYBACK:
        track = os_queue_peek(&audio_track_db.playback_tracks, 0);
        break;

    case AUDIO_STREAM_TYPE_VOICE:
        track = os_queue_peek(&audio_track_db.voice_tracks, 0);
        break;

    case AUDIO_STREAM_TYPE_RECORD:
        track = os_queue_peek(&audio_track_db.record_tracks, 0);
        break;
    }

    while (track != NULL)
    {
        if (track->state == AUDIO_SESSION_STATE_STARTING || track->state == AUDIO_SESSION_STATE_RESTARTING)
        {
            return true;
        }
        track = track->p_next;
    }

    return false;
}

static bool audio_track_is_started(T_AUDIO_STREAM_TYPE type)
{
    T_AUDIO_TRACK *track;

    switch (type)
    {
    case AUDIO_STREAM_TYPE_PLAYBACK:
        track = os_queue_peek(&audio_track_db.playback_tracks, 0);
        break;

    case AUDIO_STREAM_TYPE_VOICE:
        track = os_queue_peek(&audio_track_db.voice_tracks, 0);
        break;

    case AUDIO_STREAM_TYPE_RECORD:
        track = os_queue_peek(&audio_track_db.record_tracks, 0);
        break;
    }

    while (track != NULL)
    {
        if (track->state == AUDIO_SESSION_STATE_STARTED)
        {
            return true;
        }
        track = track->p_next;
    }

    return false;
}

void audio_track_reschedule(void)
{
    T_AUDIO_TRACK *tracks[4];
    T_AUDIO_TRACK *track;

    tracks[0] = os_queue_peek(&audio_track_db.playback_tracks, 0);
    tracks[1] = os_queue_peek(&audio_track_db.voice_tracks, 0);
    tracks[2] = os_queue_peek(&audio_track_db.record_tracks, 0);
    tracks[3] = os_queue_peek(&audio_track_db.idle_tracks, 0);

    for (uint8_t i = 0; i < 4; i++)
    {
        track = tracks[i];
        while (track != NULL)
        {
            switch (track->action)
            {
            case AUDIO_TRACK_ACTION_START:
                {
                    track->action = AUDIO_TRACK_ACTION_NONE;
                    audio_session_state_set(track, AUDIO_SESSION_STATE_STARTING);
                }
                break;

            case AUDIO_TRACK_ACTION_STOP:
                {
                    track->action = AUDIO_TRACK_ACTION_NONE;
                    audio_session_state_set(track, AUDIO_SESSION_STATE_STOPPING);
                }
                break;

            case AUDIO_TRACK_ACTION_PAUSE:
                {
                    track->action = AUDIO_TRACK_ACTION_NONE;
                    audio_session_state_set(track, AUDIO_SESSION_STATE_PAUSING);
                }
                break;

            case AUDIO_TRACK_ACTION_RESTART:
                {
                    track->action = AUDIO_TRACK_ACTION_NONE;
                    audio_session_state_set(track, AUDIO_SESSION_STATE_RESTARTING);
                }
                break;

            case AUDIO_TRACK_ACTION_RELEASE:
                {
                    track->action = AUDIO_TRACK_ACTION_NONE;
                    audio_session_state_set(track, AUDIO_SESSION_STATE_RELEASING);
                }
                break;

            default:
                break;
            }

            track = track->p_next;
        }
    }
}

static bool audio_track_queue_freeze(T_AUDIO_STREAM_TYPE type)
{
    T_AUDIO_TRACK *track;

    switch (type)
    {
    case AUDIO_STREAM_TYPE_PLAYBACK:
        track = os_queue_peek(&audio_track_db.playback_tracks, 0);
        break;

    case AUDIO_STREAM_TYPE_VOICE:
        track = os_queue_peek(&audio_track_db.voice_tracks, 0);
        break;

    case AUDIO_STREAM_TYPE_RECORD:
        track = os_queue_peek(&audio_track_db.record_tracks, 0);
        break;
    }

    while (track != NULL)
    {
        audio_track_stop(track);
        track = track->p_next;
    }

    return true;
}

static bool audio_track_handle_check(T_AUDIO_TRACK_HANDLE handle)
{
    T_AUDIO_TRACK *track;

    track = (T_AUDIO_TRACK *)handle;

    if (track == NULL)
    {
        return false;
    }

    if (os_queue_search(&audio_track_db.idle_tracks, track) == true)
    {
        return true;
    }

    if (os_queue_search(&audio_track_db.playback_tracks, track) == true)
    {
        return true;
    }

    if (os_queue_search(&audio_track_db.voice_tracks, track) == true)
    {
        return true;
    }

    if (os_queue_search(&audio_track_db.record_tracks, track) == true)
    {
        return true;
    }

    return false;
}

static bool audio_track_effect_check(T_AUDIO_TRACK           *track,
                                     T_AUDIO_EFFECT_INSTANCE  instance)
{
    if (instance == NULL)
    {
        return false;
    }

    if (os_queue_search(&track->effects, instance) == true)
    {
        return true;
    }

    return false;
}

static bool audio_track_effect_enqueue(T_AUDIO_TRACK           *track,
                                       T_AUDIO_EFFECT_INSTANCE  instance)
{
    if (instance == NULL)
    {
        return false;
    }

    if (os_queue_search(&track->effects, instance) == true)
    {
        return true;
    }

    os_queue_in(&track->effects, instance);

    return true;
}

static bool audio_track_effect_dequeue(T_AUDIO_TRACK           *track,
                                       T_AUDIO_EFFECT_INSTANCE  instance)
{
    if (instance == NULL)
    {
        return false;
    }

    return os_queue_delete(&track->effects, instance);
}

static void audio_track_effect_run(T_AUDIO_TRACK *track)
{
    T_AUDIO_EFFECT_INSTANCE instance;
    int32_t                 i = 0;

    while ((instance = os_queue_peek(&track->effects, i)) != NULL)
    {
        audio_effect_run(instance);
        i++;
    }
}

static void audio_track_effect_response(T_AUDIO_TRACK *track,
                                        uint16_t       company_id,
                                        uint16_t       effect_id,
                                        uint16_t       seq_num,
                                        uint16_t       payload_len,
                                        uint8_t       *payload)
{
    T_AUDIO_EFFECT_INSTANCE instance;
    int32_t                 i = 0;

    while ((instance = os_queue_peek(&track->effects, i)) != NULL)
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

static void audio_track_effect_indicate(T_AUDIO_TRACK *track,
                                        uint16_t       company_id,
                                        uint16_t       effect_id,
                                        uint16_t       seq_num,
                                        uint16_t       payload_len,
                                        uint8_t       *payload)
{
    T_AUDIO_EFFECT_INSTANCE instance;
    int32_t                 i = 0;

    while ((instance = os_queue_peek(&track->effects, i)) != NULL)
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

static void audio_track_effect_notify(T_AUDIO_TRACK *track,
                                      uint16_t       company_id,
                                      uint16_t       effect_id,
                                      uint16_t       seq_num,
                                      uint16_t       payload_len,
                                      uint8_t       *payload)
{
    T_AUDIO_EFFECT_INSTANCE instance;
    int32_t                 i = 0;

    while ((instance = os_queue_peek(&track->effects, i)) != NULL)
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

static void audio_track_effect_control_apply(T_AUDIO_TRACK *track,
                                             uint8_t        action)
{
    switch (track->stream_type)
    {
    case AUDIO_STREAM_TYPE_PLAYBACK:
        {
            audio_path_decoder_effect_control(track->path_handle, action);
        }
        break;

    case AUDIO_STREAM_TYPE_RECORD:
        {
            audio_path_encoder_effect_control(track->path_handle, action);
        }
        break;

    case AUDIO_STREAM_TYPE_VOICE:
        {
            audio_path_decoder_effect_control(track->path_handle, action);
            audio_path_encoder_effect_control(track->path_handle, action);
        }
        break;

    default:
        break;
    }
}

static bool audio_track_effect_stop(T_AUDIO_TRACK *track)
{
#if 0
    T_AUDIO_EFFECT_INSTANCE instance;
    int32_t                 i = 0;

    while ((instance = os_queue_peek(&track->effects, i)) != NULL)
    {
        audio_effect_stop(instance);
        i++;
    }
#endif
    return true;
}

static bool audio_track_effect_clear(T_AUDIO_TRACK *track)
{
    T_AUDIO_EFFECT_INSTANCE instance;

    while ((instance = os_queue_out(&track->effects)) != NULL)
    {
        audio_effect_owner_clear(instance);
    }

    return true;
}

void audio_track_volume_out_mute_all(T_AUDIO_STREAM_TYPE type)
{
    T_OS_QUEUE    *queue = NULL;
    T_AUDIO_TRACK *track;

    switch (type)
    {
    case AUDIO_STREAM_TYPE_PLAYBACK:
        if (audio_track_db.playback_volume_muted == false)
        {
            queue = &audio_track_db.playback_tracks;
            audio_track_db.playback_volume_muted = true;
        }
        break;

    case AUDIO_STREAM_TYPE_VOICE:
        if (audio_track_db.voice_volume_out_muted == false)
        {
            queue = &audio_track_db.voice_tracks;
            audio_track_db.voice_volume_out_muted = true;
        }
        break;

    case AUDIO_STREAM_TYPE_RECORD:
        break;
    }

    if (queue != NULL)
    {
        track = os_queue_peek(queue, 0);
        while (track != NULL)
        {
            if (track->volume_out_muted == false)
            {
                audio_path_dac_mute(track->path_handle);
            }

            track = track->p_next;
        }
    }
}

void audio_track_volume_out_unmute_all(T_AUDIO_STREAM_TYPE type)
{
    T_OS_QUEUE    *queue = NULL;
    T_AUDIO_TRACK *track;
    float          scale;

    switch (type)
    {
    case AUDIO_STREAM_TYPE_PLAYBACK:
        if (audio_track_db.playback_volume_muted == true)
        {
            queue = &audio_track_db.playback_tracks;
            audio_track_db.playback_volume_muted = false;
            scale = audio_track_db.playback_volume_scale;
        }
        break;

    case AUDIO_STREAM_TYPE_VOICE:
        if (audio_track_db.voice_volume_out_muted == true)
        {
            queue = &audio_track_db.voice_tracks;
            audio_track_db.voice_volume_out_muted = false;
            scale = audio_track_db.voice_volume_out_scale;
        }
        break;

    case AUDIO_STREAM_TYPE_RECORD:
        {
            queue = NULL;
            scale = 0.0f;
        }
        break;
    }

    if (queue != NULL)
    {
        track = os_queue_peek(queue, 0);
        while (track != NULL)
        {
            if (track->volume_out_muted == false)
            {
                audio_path_dac_level_set(track->path_handle, track->volume_out, scale);
            }

            track = track->p_next;
        }
    }
}

void audio_track_volume_out_set_all(T_AUDIO_STREAM_TYPE type,
                                    uint8_t             volume)
{
    T_OS_QUEUE    *queue = NULL;
    T_AUDIO_TRACK *track;

    switch (type)
    {
    case AUDIO_STREAM_TYPE_PLAYBACK:
        queue = &audio_track_db.playback_tracks;
        break;

    case AUDIO_STREAM_TYPE_VOICE:
        queue = &audio_track_db.voice_tracks;
        break;

    case AUDIO_STREAM_TYPE_RECORD:
        break;
    }

    if (queue != NULL)
    {
        track = os_queue_peek(queue, 0);
        while (track != NULL)
        {
            audio_track_volume_out_set(track, volume);
            track = track->p_next;
        }

        track = os_queue_peek(&audio_track_db.idle_tracks, 0);
        while (track != NULL)
        {
            if (track->stream_type == type)
            {
                audio_track_volume_out_set(track, volume);
            }

            track = track->p_next;
        }
    }
}

void audio_track_volume_in_mute_all(T_AUDIO_STREAM_TYPE type)
{
    T_OS_QUEUE    *queue = NULL;
    T_AUDIO_TRACK *track;

    switch (type)
    {
    case AUDIO_STREAM_TYPE_PLAYBACK:
        break;

    case AUDIO_STREAM_TYPE_VOICE:
        if (audio_track_db.voice_volume_in_muted == false)
        {
            queue = &audio_track_db.voice_tracks;
            audio_track_db.voice_volume_in_muted = true;
        }
        break;

    case AUDIO_STREAM_TYPE_RECORD:
        if (audio_track_db.record_volume_muted == false)
        {
            queue = &audio_track_db.record_tracks;
            audio_track_db.record_volume_muted = true;
        }
        break;
    }

    if (queue != NULL)
    {
        track = os_queue_peek(queue, 0);
        while (track != NULL)
        {
            if (track->volume_in_muted == false)
            {
                audio_path_adc_mute(track->path_handle);
            }

            track = track->p_next;
        }
    }
}

void audio_track_volume_in_unmute_all(T_AUDIO_STREAM_TYPE type)
{
    T_OS_QUEUE    *queue = NULL;
    T_AUDIO_TRACK *track;

    switch (type)
    {
    case AUDIO_STREAM_TYPE_PLAYBACK:
        break;

    case AUDIO_STREAM_TYPE_VOICE:
        if (audio_track_db.voice_volume_in_muted == true)
        {
            queue = &audio_track_db.voice_tracks;
            audio_track_db.voice_volume_in_muted = false;
        }
        break;

    case AUDIO_STREAM_TYPE_RECORD:
        if (audio_track_db.record_volume_muted == true)
        {
            queue = &audio_track_db.record_tracks;
            audio_track_db.record_volume_muted = false;
        }
        break;
    }

    if (queue != NULL)
    {
        track = os_queue_peek(queue, 0);
        while (track != NULL)
        {
            if (track->volume_in_muted == false)
            {
                audio_path_adc_level_set(track->path_handle, track->volume_in, 0.0f);
            }

            track = track->p_next;
        }
    }
}

void audio_track_volume_in_set_all(T_AUDIO_STREAM_TYPE type,
                                   uint8_t             volume)
{
    T_OS_QUEUE    *queue = NULL;
    T_AUDIO_TRACK *track;

    switch (type)
    {
    case AUDIO_STREAM_TYPE_PLAYBACK:
        break;

    case AUDIO_STREAM_TYPE_VOICE:
        queue = &audio_track_db.voice_tracks;
        break;

    case AUDIO_STREAM_TYPE_RECORD:
        queue = &audio_track_db.record_tracks;
        break;
    }

    if (queue != NULL)
    {
        track = os_queue_peek(queue, 0);
        while (track != NULL)
        {
            audio_track_volume_in_set(track, volume);
            track = track->p_next;
        }

        track = os_queue_peek(&audio_track_db.idle_tracks, 0);
        while (track != NULL)
        {
            if (track->stream_type == type)
            {
                audio_track_volume_in_set(track, volume);
            }

            track = track->p_next;
        }
    }
}

static void audio_track_volume_apply(T_AUDIO_TRACK *track)
{
    switch (track->stream_type)
    {
    case AUDIO_STREAM_TYPE_PLAYBACK:
        if ((audio_track_db.playback_volume_muted == true) ||
            (track->volume_out_muted == true))
        {
            audio_path_dac_mute(track->path_handle);
        }
        else
        {
            audio_path_dac_level_set(track->path_handle, track->volume_out,
                                     audio_track_db.playback_volume_scale);
        }
        break;

    case AUDIO_STREAM_TYPE_VOICE:
        if ((audio_track_db.voice_volume_out_muted == true) ||
            (track->volume_out_muted == true))
        {
            audio_path_dac_mute(track->path_handle);
        }
        else
        {
            audio_path_dac_level_set(track->path_handle, track->volume_out,
                                     audio_track_db.voice_volume_out_scale);
        }

        if ((audio_track_db.voice_volume_in_muted == true) ||
            (track->volume_in_muted == true))
        {
            audio_path_adc_mute(track->path_handle);
        }
        else
        {
            audio_path_adc_level_set(track->path_handle, track->volume_in, 0.0f);
        }
        break;

    case AUDIO_STREAM_TYPE_RECORD:
        if ((audio_track_db.record_volume_muted == true) ||
            (track->volume_in_muted == true))
        {
            audio_path_adc_mute(track->path_handle);
        }
        else
        {
            audio_path_adc_level_set(track->path_handle, track->volume_in, 0.0f);
        }
        break;
    }
}

bool audio_track_volume_balance_set(T_AUDIO_STREAM_TYPE stream_type,
                                    float               scale)
{
    T_OS_QUEUE    *queue;
    T_AUDIO_TRACK *track;

    switch (stream_type)
    {
    case AUDIO_STREAM_TYPE_PLAYBACK:
        queue = &audio_track_db.playback_tracks;
        audio_track_db.playback_volume_scale = scale;
        break;

    case AUDIO_STREAM_TYPE_VOICE:
        queue = &audio_track_db.voice_tracks;
        audio_track_db.voice_volume_out_scale = scale;
        break;

    case AUDIO_STREAM_TYPE_RECORD:
        queue = NULL;
        break;
    }

    if (queue != NULL)
    {
        track = os_queue_peek(queue, 0);
        while (track != NULL)
        {
            audio_track_volume_out_set(track, track->volume_out);
            track = track->p_next;
        }

        return true;
    }

    return false;
}

static T_AUDIO_TRACK *audio_track_find_by_path(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_TRACK *track;

    track = os_queue_peek(&audio_track_db.idle_tracks, 0);
    while (track != NULL)
    {
        if (track->path_handle != NULL &&
            track->path_handle == handle)
        {
            return track;
        }

        track = track->p_next;
    }

    track = os_queue_peek(&audio_track_db.playback_tracks, 0);
    while (track != NULL)
    {
        if (track->path_handle != NULL &&
            track->path_handle == handle)
        {
            return track;
        }

        track = track->p_next;
    }

    track = os_queue_peek(&audio_track_db.voice_tracks, 0);
    while (track != NULL)
    {
        if (track->path_handle != NULL &&
            track->path_handle == handle)
        {
            return track;
        }

        track = track->p_next;
    }

    track = os_queue_peek(&audio_track_db.record_tracks, 0);
    while (track != NULL)
    {
        if (track->path_handle != NULL &&
            track->path_handle == handle)
        {
            return track;
        }

        track = track->p_next;
    }

    return track;
}

static bool audio_track_buffer_attach(T_AUDIO_TRACK *track)
{
    track->buffer_proxy = media_buffer_proxy_attach(track->stream_type, track->usage, track->mode,
                                                    &track->format_info, track, track->path_handle);
    media_buffer_proxy_latency_set(track->buffer_proxy, track->latency, track->latency_fixed);
    return true;
}

static bool audio_track_buffer_detach(T_AUDIO_TRACK *track)
{
    media_buffer_proxy_detach(track->buffer_proxy);
    track->buffer_proxy = NULL;

    return true;
}

static bool audio_track_buffer_reset(T_AUDIO_TRACK *track)
{
    media_buffer_proxy_reset(track->buffer_proxy);

    return true;
}

static bool audio_track_path_cback(T_AUDIO_PATH_HANDLE handle,
                                   T_AUDIO_PATH_EVENT  event,
                                   uint32_t            param)
{
    T_AUDIO_TRACK *track;

    track = audio_track_find_by_path(handle);

    AUDIO_PRINT_TRACE4("audio_track_path_cback: handle %p, event 0x%02x, param 0x%08x, track %p",
                       handle, event, param, track);

    if (track != NULL)
    {
        switch (event)
        {
        case AUDIO_PATH_EVT_RELEASE:
            {
                audio_session_state_set(track, AUDIO_SESSION_STATE_RELEASED);
            }
            break;

        case AUDIO_PATH_EVT_CREATE:
            {
                audio_session_state_set(track, AUDIO_SESSION_STATE_CREATED);
            }
            break;

        case AUDIO_PATH_EVT_IDLE:
            {
                audio_session_state_set(track, AUDIO_SESSION_STATE_STOPPED);
            }
            break;

        case AUDIO_PATH_EVT_EFFECT_REQ:
            {
                track->effect_apply = true;

                audio_track_effect_control_apply(track, track->effect_bypass);
                audio_track_effect_run(track);

                if (track->channel_out_num != 0)
                {
                    if (track->stream_type == AUDIO_STREAM_TYPE_PLAYBACK ||
                        track->stream_type == AUDIO_STREAM_TYPE_VOICE)
                    {
                        audio_path_channel_out_reorder(track->path_handle,
                                                       track->channel_out_num,
                                                       track->channel_out_array);
                    }
                }

                if (track->channel_in_num != 0)
                {
                    if (track->stream_type == AUDIO_STREAM_TYPE_RECORD ||
                        track->stream_type == AUDIO_STREAM_TYPE_VOICE)
                    {
                        audio_path_channel_in_reorder(track->path_handle,
                                                      track->channel_in_num,
                                                      track->channel_in_array);
                    }
                }
            }
            break;

        case AUDIO_PATH_EVT_RUNNING:
            {
                audio_session_state_set(track, AUDIO_SESSION_STATE_STARTED);
            }
            break;

        case AUDIO_PATH_EVT_SUSPEND:
            {
                audio_session_state_set(track, AUDIO_SESSION_STATE_PAUSED);
            }
            break;

        case AUDIO_PATH_EVT_DATA_REQ:
            audio_mgr_dispatch(AUDIO_MSG_TRACK_DATA_XMIT, track->buffer_proxy);
            break;

        case AUDIO_PATH_EVT_KWS_HIT:
            {
                T_AUDIO_PATH_KWS_HIT        *kws_hit = (T_AUDIO_PATH_KWS_HIT *)param;
                T_AUDIO_MSG_PAYLOAD_KWS_HIT  payload;

                payload.keyword_index = kws_hit->keyword_index;

                audio_mgr_dispatch(AUDIO_MSG_KWS_HIT, &payload);
            }
            break;

        case AUDIO_PATH_EVT_DATA_IND:
            {
                T_AUDIO_PATH_DATA_HDR *peek_buf;
                uint16_t               peek_len;
                uint16_t               len;
                uint16_t               actual_len;
                T_AUDIO_STREAM_STATUS  status;
                uint8_t                frame_num;
                uint32_t               timestamp;
                uint16_t               seq_num;
                uint8_t               *payload;
                uint16_t               payload_len;

                peek_len = audio_path_data_peek(track->path_handle);
                peek_buf = malloc(peek_len);
                if (peek_buf == NULL)
                {
                    break;
                }

                len = audio_path_data_recv(track->path_handle, peek_buf, peek_len);
                if (len == 0)
                {
                    AUDIO_PRINT_ERROR2("AUDIO_PATH_EVT_DATA_IND: peek_len %u, len %u", peek_len, len);
                    free(peek_buf);
                    break;
                }

                status            = (T_AUDIO_STREAM_STATUS)peek_buf->status;
                frame_num         = peek_buf->frame_number;
                timestamp         = peek_buf->timestamp;
                payload           = peek_buf->payload;
                payload_len       = peek_buf->payload_length;
                seq_num           = peek_buf->seq_num;

                AUDIO_PRINT_INFO5("AUDIO_PATH_EVT_DATA_IND: status %u, payload_len %u, timestamp 0x%08x, seq_num %u, frame_num %u",
                                  status, payload_len, timestamp, seq_num, frame_num);

                if (track->async_read != NULL)
                {
                    if (track->async_read(track,
                                          &timestamp,
                                          &seq_num,
                                          &status,
                                          &frame_num,
                                          payload,
                                          payload_len,
                                          &actual_len))
                    {
                        if (actual_len != payload_len)
                        {
                            media_buffer_proxy_us_write(track->buffer_proxy,
                                                        payload + actual_len,
                                                        payload_len - actual_len,
                                                        timestamp,
                                                        seq_num,
                                                        status,
                                                        frame_num);
                        }
                    }
                    else
                    {
                        media_buffer_proxy_us_write(track->buffer_proxy,
                                                    payload,
                                                    payload_len,
                                                    timestamp,
                                                    seq_num,
                                                    status,
                                                    frame_num);
                    }
                }
                else
                {
                    uint16_t written_len;
                    T_AUDIO_MSG_PAYLOAD_TRACK_DATA_IND msg;

                    written_len = media_buffer_proxy_us_write(track->buffer_proxy,
                                                              payload,
                                                              payload_len,
                                                              timestamp,
                                                              seq_num,
                                                              status,
                                                              frame_num);
                    if (written_len != 0)
                    {
                        msg.handle = track;
                        msg.len    = written_len;
                        audio_mgr_dispatch(AUDIO_MSG_TRACK_DATA_IND, &msg);
                    }
                }
                free(peek_buf);
            }
            break;

        case AUDIO_PATH_EVT_DATA_EMPTY:
            {
                audio_mgr_dispatch(AUDIO_MSG_DSP_DECODE_EMPTY, track->buffer_proxy);
            }
            break;

        case AUDIO_PATH_EVT_SIGNAL_REFRESH:
            {
                T_AUDIO_PATH_SIGNAL_REFRESH *p_msg = (T_AUDIO_PATH_SIGNAL_REFRESH *)param;

                if (p_msg->direction == AUDIO_PATH_DIR_TX)
                {
                    T_AUDIO_MSG_PAYLOAD_TRACK_SIGNAL_OUT_REFRESH msg;

                    msg.handle = track;
                    msg.freq_num = p_msg->freq_num;
                    msg.left_gain_table = p_msg->left_gain_table;
                    msg.right_gain_table = p_msg->right_gain_table;
                    audio_mgr_dispatch(AUDIO_MSG_TRACK_SIGNAL_OUT_REFRESH, &msg);
                }
                else if (p_msg->direction == AUDIO_PATH_DIR_RX)
                {
                    T_AUDIO_MSG_PAYLOAD_TRACK_SIGNAL_IN_REFRESH msg;

                    msg.handle = track;
                    msg.freq_num = p_msg->freq_num;
                    msg.left_gain_table = p_msg->left_gain_table;
                    msg.right_gain_table = p_msg->right_gain_table;
                    audio_mgr_dispatch(AUDIO_MSG_TRACK_SIGNAL_IN_REFRESH, &msg);
                }
            }
            break;

        case AUDIO_PATH_EVT_FIFO_REPORT:
            {
                T_AUDIO_PATH_LATENCY_REPORT *p_msg = (T_AUDIO_PATH_LATENCY_REPORT *)param;
                T_AUDIO_MSG_PAYLOAD_STREAM_LATENCY_REPORT msg;

                msg.buffer_proxy = track->buffer_proxy;
                msg.average_fifo_queuing = p_msg->average_fifo_queuing;
                msg.average_packet_latency = p_msg->average_packet_latency;
                msg.normal_packet_count = p_msg->normal_packet_count;

                audio_mgr_dispatch(AUDIO_MSG_STREAM_LATENCY_REPORT, &msg);
            }
            break;

        case AUDIO_PATH_EVT_PLC_REPORT:
            {
                T_AUDIO_PATH_PLC_NOTIFY *p_msg = (T_AUDIO_PATH_PLC_NOTIFY *)param;
                T_AUDIO_MSG_PAYLOAD_TRACK_PLC_REPORT msg;

                msg.buffer_proxy = track->buffer_proxy;
                msg.total_sample_num = p_msg->total_sample_num;
                msg.plc_sample_num = p_msg->plc_sample_num;

                audio_mgr_dispatch(AUDIO_MSG_STREAM_PLC_REPORT, &msg);
            }
            break;

        case AUDIO_PATH_EVT_VSE_RESPONSE:
            {
                T_AUDIO_PATH_VSE_RESPONSE *rsp = (T_AUDIO_PATH_VSE_RESPONSE *)param;

                audio_track_effect_response(track,
                                            rsp->company_id,
                                            rsp->effect_id,
                                            rsp->seq_num,
                                            rsp->payload_len,
                                            rsp->payload);
            }
            break;

        case AUDIO_PATH_EVT_VSE_INDICATE:
            {
                T_AUDIO_PATH_VSE_INDICATE *ind = (T_AUDIO_PATH_VSE_INDICATE *)param;

                audio_track_effect_indicate(track,
                                            ind->company_id,
                                            ind->effect_id,
                                            ind->seq_num,
                                            ind->payload_len,
                                            ind->payload);
            }
            break;

        case AUDIO_PATH_EVT_VSE_NOTIFY:
            {
                T_AUDIO_PATH_VSE_NOTIFY *notify = (T_AUDIO_PATH_VSE_NOTIFY *)param;

                audio_track_effect_notify(track,
                                          notify->company_id,
                                          notify->effect_id,
                                          notify->seq_num,
                                          notify->payload_len,
                                          notify->payload);
            }
            break;

        case AUDIO_PATH_EVT_DSP_SYNC_UNLOCK:
            {
                if ((track->stream_type == AUDIO_STREAM_TYPE_VOICE) ||
                    (track->stream_type == AUDIO_STREAM_TYPE_PLAYBACK))
                {
                    audio_mgr_dispatch(AUDIO_MSG_DSP_SYNC_UNLOCK, track->buffer_proxy);
                }
            }
            break;

        case AUDIO_PATH_EVT_DSP_SYNC_LOCK:
            {
                if ((track->stream_type == AUDIO_STREAM_TYPE_VOICE) ||
                    track->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
                {
                    audio_mgr_dispatch(AUDIO_MSG_A2DP_DSP_SYNC_LOCK, track->buffer_proxy);
                }
            }
            break;

        case AUDIO_PATH_EVT_RETURN_JOIN_INFO:
            {
                T_AUDIO_MSG_RETURN_JOIN_INFO *p_msg = (T_AUDIO_MSG_RETURN_JOIN_INFO *)param;
                T_AUDIO_MSG_PAYLOAD_TRACK_RETURN_JOIN_INFO msg;

                msg.buffer_proxy = track->buffer_proxy;
                msg.join_clk = p_msg->join_clk;
                msg.join_frame = p_msg->join_frame;
                msg.delta_frame = p_msg->delta_frame;

                audio_mgr_dispatch(AUDIO_MSG_RETURN_JOIN_INFO, &msg);
            }
            break;

        case AUDIO_PATH_EXC_DSP_UNSYNC:
            {
                if ((track->stream_type == AUDIO_STREAM_TYPE_VOICE) ||
                    (track->stream_type == AUDIO_STREAM_TYPE_PLAYBACK))
                {
                    audio_mgr_dispatch(AUDIO_MSG_DSP_UNSYNC, track->buffer_proxy);
                }
            }
            break;

        case AUDIO_PATH_EXC_DSP_SYNC_EMPTY:
            {
                audio_mgr_dispatch(AUDIO_MSG_DSP_SYNC_EMPTY, track->buffer_proxy);
            }
            break;

        case AUDIO_PATH_EXC_DSP_LOST_TIMESTAMP:
            {
                audio_mgr_dispatch(AUDIO_MSG_DSP_LOST_TIMESTAMP, track->buffer_proxy);
            }
            break;

        case AUDIO_PATH_EVT_ERROR_REPORT:
            {
                T_AUDIO_MSG_PAYLOAD_TRACK_ERROR_REPORT msg;

                msg.handle = track;
                audio_mgr_dispatch(AUDIO_MSG_TRACK_ERROR_REPORT, &msg);
            }
            break;

        default:
            break;
        }

        return true;
    }

    return false;
}

static bool audio_track_released_handler(T_AUDIO_TRACK         *track,
                                         T_AUDIO_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_SESSION_STATE_RELEASED:
    case AUDIO_SESSION_STATE_CREATED:
    case AUDIO_SESSION_STATE_STARTING:
    case AUDIO_SESSION_STATE_STARTED:
    case AUDIO_SESSION_STATE_STOPPING:
    case AUDIO_SESSION_STATE_STOPPED:
    case AUDIO_SESSION_STATE_PAUSING:
    case AUDIO_SESSION_STATE_PAUSED:
    case AUDIO_SESSION_STATE_RESTARTING:
    case AUDIO_SESSION_STATE_RELEASING:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_SESSION_STATE_CREATING:
        {
            T_AUDIO_CATEGORY   path_category;
            T_AUDIO_PATH_PARAM path_param;

            path_category  = audio_track_path_category_convert(track->stream_type);
            if (path_category == AUDIO_CATEGORY_AUDIO)
            {
                path_param.audio.device       = track->device;
                path_param.audio.decoder_info = &track->format_info;
                path_param.audio.mode         = track->mode;
                path_param.audio.dac_level    = track->volume_out;
            }
            else if (path_category == AUDIO_CATEGORY_VOICE)
            {
                path_param.voice.device       = track->device;
                path_param.voice.decoder_info = &track->format_info;
                path_param.voice.encoder_info = &track->format_info;
                path_param.voice.mode         = track->mode;
                path_param.voice.dac_level    = track->volume_out;
                path_param.voice.adc_level    = track->volume_in;
            }
            else
            {
                path_param.record.device       = track->device;
                path_param.record.encoder_info = &track->format_info;
                path_param.record.mode         = track->mode;
                path_param.record.adc_level    = track->volume_in;
            }

            track->path_handle = audio_path_create(path_category,
                                                   path_param,
                                                   audio_track_path_cback);
            if (track->path_handle != NULL)
            {
                /* Update track session state first, as audio path callback
                 * will invoke audio_track_start.
                 */
                track->state = state;

                /* Enqueue track before audio path created, as audio path callback
                 * may manipulate the idle track first.
                 */
                audio_track_queue_add(&audio_track_db.idle_tracks, track, false);

                /* Attach track to the matched media buffer. */
                audio_track_buffer_attach(track);

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

static bool audio_track_creating_handler(T_AUDIO_TRACK         *track,
                                         T_AUDIO_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_SESSION_STATE_RELEASED:
    case AUDIO_SESSION_STATE_CREATING:
    case AUDIO_SESSION_STATE_STARTED:
    case AUDIO_SESSION_STATE_STOPPED:
    case AUDIO_SESSION_STATE_PAUSED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_SESSION_STATE_CREATED:
        {
            T_AUDIO_MSG_PAYLOAD_TRACK_CREATE payload;

            track->state        = state;

            payload.handle      = track;
            payload.stream_type = track->stream_type;

            audio_mgr_dispatch(AUDIO_MSG_TRACK_CREATE, &payload);

            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = audio_session_state_set(track, AUDIO_SESSION_STATE_STARTING);
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = audio_session_state_set(track, AUDIO_SESSION_STATE_RESTARTING);
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = audio_session_state_set(track, AUDIO_SESSION_STATE_RELEASING);
            }
        }
        break;

    case AUDIO_SESSION_STATE_STARTING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                /* Start audio track immediately after audio_track_create invoked,
                 * and the audio path created event has not been received, so this
                 * action should be pending.
                 */
                track->action = AUDIO_TRACK_ACTION_START;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* Drop repeating audio_track_start invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* Override the pending restart action for the creating audio track. */
                track->action = AUDIO_TRACK_ACTION_START;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the creating audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_STOPPING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                /* Stopping the creating audio track is forbidden. */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* Cancel all pending actions when the creating audio track is
                 * pending for start action, but receives the stop action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* Cancel all pending actions when the creating audio track is
                 * pending for restart action, but receives the stop action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the creating audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_PAUSING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                /* Pausing the creating audio track is forbidden. */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* Cancel all pending actions when the creating audio track is
                 * pending for start action, but receives the pause action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* Cancel all pending actions when the creating audio track is
                 * pending for restart action, but receives the pause action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the creating audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_RESTARTING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                /* Restart audio track immediately after audio_track_create invoked,
                 * and the audio path created event has not been received, so this
                 * action should be pending.
                 */
                track->action = AUDIO_TRACK_ACTION_RESTART;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* Override the pending start action for the creating audio track. */
                track->action = AUDIO_TRACK_ACTION_RESTART;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* Drop repeating audio_track_restart invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the creating audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_RELEASING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                track->action = AUDIO_TRACK_ACTION_RELEASE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* Override the pending start action for the creating audio track. */
                track->action = AUDIO_TRACK_ACTION_RELEASE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* Override the pending restart action for the creating audio track. */
                track->action = AUDIO_TRACK_ACTION_RELEASE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* Drop repeating audio_track_release invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
        }
        break;
    }

    return ret;
}

static bool audio_track_created_handler(T_AUDIO_TRACK         *track,
                                        T_AUDIO_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_SESSION_STATE_RELEASED:
    case AUDIO_SESSION_STATE_CREATING:
    case AUDIO_SESSION_STATE_CREATED:
    case AUDIO_SESSION_STATE_STARTED:
    case AUDIO_SESSION_STATE_STOPPED:
    case AUDIO_SESSION_STATE_PAUSED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_SESSION_STATE_STARTING:
        {
            if (audio_track_db.multi_instance_enable == false)
            {
                if (audio_track_is_starting(track->stream_type))
                {
                    track->action = AUDIO_TRACK_ACTION_START;
                    break;
                }
            }

            if (audio_track_db.multi_instance_enable == false)
            {
                /* Only one of the tracks with the same stream type can be playing. */
                audio_track_queue_freeze(track->stream_type);

                /* Suppress all audio playback tracks in the voice scenario. */
                if (track->stream_type == AUDIO_STREAM_TYPE_VOICE)
                {
                    audio_track_queue_freeze(AUDIO_STREAM_TYPE_PLAYBACK);
                }
                else if (track->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
                {
                    if (audio_track_is_started(AUDIO_STREAM_TYPE_VOICE))
                    {
                        track->action = AUDIO_TRACK_ACTION_START;
                        break;
                    }
                }
            }

            track->state = state;

            ret = audio_track_queue_remove(track);
            if (ret == false)
            {
                /* Restore the track state. */
                track->state = AUDIO_SESSION_STATE_CREATED;
                break;
            }

            /* The latest started track has the highest priority. */
            if (track->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                audio_track_queue_add(&audio_track_db.playback_tracks, track, true);
            }
            else if (track->stream_type == AUDIO_STREAM_TYPE_VOICE)
            {
                audio_track_queue_add(&audio_track_db.voice_tracks, track, true);
            }
            else if (track->stream_type == AUDIO_STREAM_TYPE_RECORD)
            {
                audio_track_queue_add(&audio_track_db.record_tracks, track, true);
            }

            ret = audio_path_start(track->path_handle);
            if (ret == false)
            {
                /* TODO resume other tracks. */

                /* Restore the track state. */
                track->state = AUDIO_SESSION_STATE_CREATED;
                audio_track_queue_add(&audio_track_db.idle_tracks, track, false);
                break;
            }
        }
        break;

    case AUDIO_SESSION_STATE_STOPPING:
        {
            if (track->action == AUDIO_TRACK_ACTION_START || track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                track->action = AUDIO_TRACK_ACTION_NONE;
            }
            /* Stopping the created audio track is forbidden. */
            ret = false;
        }
        break;

    case AUDIO_SESSION_STATE_PAUSING:
        {
            if (track->action == AUDIO_TRACK_ACTION_START || track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                track->action = AUDIO_TRACK_ACTION_NONE;
            }
            /* Pausing the created audio track is forbidden. */
            ret = false;
        }
        break;

    case AUDIO_SESSION_STATE_RESTARTING:
        {
            if (audio_track_db.multi_instance_enable == false)
            {
                if (audio_track_is_starting(track->stream_type))
                {
                    track->action = AUDIO_TRACK_ACTION_RESTART;
                    break;
                }
            }

            if (audio_track_db.multi_instance_enable == false)
            {
                /* Only one of the tracks with the same stream type can be playing. */
                audio_track_queue_freeze(track->stream_type);

                /* Suppress all audio playback tracks in the voice scenario. */
                if (track->stream_type == AUDIO_STREAM_TYPE_VOICE)
                {
                    audio_track_queue_freeze(AUDIO_STREAM_TYPE_PLAYBACK);
                }
                else if (track->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
                {
                    if (audio_track_is_started(AUDIO_STREAM_TYPE_VOICE))
                    {
                        track->action = AUDIO_TRACK_ACTION_RESTART;
                        break;
                    }
                }
            }

            track->state = state;

            ret = audio_track_queue_remove(track);
            if (ret == false)
            {
                /* Restore the track state. */
                track->state = AUDIO_SESSION_STATE_CREATED;
                break;
            }

            /* The latest started track has the highest priority. */
            if (track->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                audio_track_queue_add(&audio_track_db.playback_tracks, track, true);
            }
            else if (track->stream_type == AUDIO_STREAM_TYPE_VOICE)
            {
                audio_track_queue_add(&audio_track_db.voice_tracks, track, true);
            }
            else if (track->stream_type == AUDIO_STREAM_TYPE_RECORD)
            {
                audio_track_queue_add(&audio_track_db.record_tracks, track, true);
            }

            ret = audio_path_start(track->path_handle);
            if (ret == false)
            {
                /* TODO resume other tracks. */

                /* Restore the track state. */
                track->state = AUDIO_SESSION_STATE_CREATED;
                audio_track_queue_add(&audio_track_db.idle_tracks, track, false);
                break;
            }
        }
        break;

    case AUDIO_SESSION_STATE_RELEASING:
        {
            track->state = AUDIO_SESSION_STATE_RELEASING;

            /* Detach track's media buffer. */
            audio_track_buffer_detach(track);

            ret = audio_path_destroy(track->path_handle);
            if (ret == false)
            {
                /* Restore the track state. */
                track->state = AUDIO_SESSION_STATE_CREATED;
                audio_track_buffer_attach(track);
                break;
            }
        }
        break;
    }

    return ret;
}

static bool audio_track_starting_handler(T_AUDIO_TRACK         *track,
                                         T_AUDIO_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_SESSION_STATE_RELEASED:
    case AUDIO_SESSION_STATE_CREATING:
    case AUDIO_SESSION_STATE_CREATED:
    case AUDIO_SESSION_STATE_STOPPED:
    case AUDIO_SESSION_STATE_PAUSED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_SESSION_STATE_STARTING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                /* Drop repeating audio_track_start invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* Cancel all pending actions when the starting audio track is
                 * pending for stop action, but receives the start action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* Cancel all pending actions when the starting audio track is
                 * pending for pause action, but receives the start action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* This case will not happen in reality, as the pending restart
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the starting audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_STARTED:
        {
            T_AUDIO_MSG_PAYLOAD_TRACK_START payload;

            track->state        = state;
            track->effect_apply = false;

            audio_track_volume_apply(track);
            media_buffer_proxy_fifo_report_config(track->buffer_proxy);

            if (track->signal_out)
            {
                if (track->stream_type == AUDIO_STREAM_TYPE_PLAYBACK ||
                    track->stream_type == AUDIO_STREAM_TYPE_VOICE)
                {
                    audio_path_signal_monitoring_set(track->path_handle,
                                                     AUDIO_PATH_DIR_TX,
                                                     true,
                                                     track->signal_out_interval,
                                                     track->freq_out_num,
                                                     track->freq_out_table);
                }
            }

            if (track->signal_in)
            {
                if (track->stream_type == AUDIO_STREAM_TYPE_VOICE ||
                    track->stream_type == AUDIO_STREAM_TYPE_RECORD)
                {
                    audio_path_signal_monitoring_set(track->path_handle,
                                                     AUDIO_PATH_DIR_RX,
                                                     true,
                                                     track->signal_in_interval,
                                                     track->freq_in_num,
                                                     track->freq_in_table);
                }
            }

            payload.handle       = track;
            payload.stream_type  = track->stream_type;
            payload.buffer_proxy = track->buffer_proxy;

            audio_mgr_dispatch(AUDIO_MSG_TRACK_START, &payload);

            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                audio_track_reschedule();
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = audio_session_state_set(track, AUDIO_SESSION_STATE_STOPPING);
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = audio_session_state_set(track, AUDIO_SESSION_STATE_PAUSING);
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = audio_session_state_set(track, AUDIO_SESSION_STATE_RESTARTING);
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = audio_session_state_set(track, AUDIO_SESSION_STATE_RELEASING);
            }
        }
        break;

    case AUDIO_SESSION_STATE_STOPPING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                track->action = AUDIO_TRACK_ACTION_STOP;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* Drop repeating audio_track_stop invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* Override the pending pause action for the starting audio track. */
                track->action = AUDIO_TRACK_ACTION_STOP;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* This case will not happen in reality, as the pending restart
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the starting audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_PAUSING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                track->action = AUDIO_TRACK_ACTION_PAUSE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* Drop the pause action for the starting audio track, as the
                 * pause action cannot override the pending stop action.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* Drop repeating audio_track_pause invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* This case will not happen in reality, as the pending restart
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the starting audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_RESTARTING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                /* Drop audio_track_restart invocation. But this case
                 * is permitted.
                 */
                track->action = AUDIO_TRACK_ACTION_RESTART;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                track->action = AUDIO_TRACK_ACTION_RESTART;
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* Cancel all pending actions when the starting audio track is
                 * pending for stop action, but receives the restart action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* Cancel all pending actions when the starting audio track is
                 * pending for pause action, but receives the restart action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* This case will not happen in reality, as the pending restart
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the starting audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_RELEASING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                track->action = AUDIO_TRACK_ACTION_RELEASE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* Override the pending stop action for the starting audio track. */
                track->action = AUDIO_TRACK_ACTION_RELEASE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* Override the pending pause action for the starting audio track. */
                track->action = AUDIO_TRACK_ACTION_RELEASE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* This case will not happen in reality, as the pending restart
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* Drop repeating audio_track_release invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
        }
        break;
    }

    return ret;
}

static bool audio_track_started_handler(T_AUDIO_TRACK         *track,
                                        T_AUDIO_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_SESSION_STATE_RELEASED:
    case AUDIO_SESSION_STATE_CREATING:
    case AUDIO_SESSION_STATE_CREATED:
    case AUDIO_SESSION_STATE_STARTING:
    case AUDIO_SESSION_STATE_STARTED:
    case AUDIO_SESSION_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_SESSION_STATE_STOPPING:
        {
            track->state = state;
            audio_track_effect_stop(track);

            ret = audio_path_stop(track->path_handle);
            if (ret == false)
            {
                /* Restore the track state. */
                track->state = AUDIO_SESSION_STATE_STARTED;
                audio_track_effect_run(track);
                break;
            }
        }
        break;

    case AUDIO_SESSION_STATE_PAUSING:
        {
            track->state = state;
            audio_track_effect_stop(track);

            /* Currently, there is no audio path pause interface. */
            ret = audio_path_stop(track->path_handle);
            if (ret == false)
            {
                /* Restore the track state. */
                track->state = AUDIO_SESSION_STATE_STARTED;
                audio_track_effect_run(track);
                break;
            }
        }
        break;

    case AUDIO_SESSION_STATE_PAUSED:
        {
            T_AUDIO_MSG_PAYLOAD_TRACK_PAUSE payload;

            track->state        = state;

            payload.handle      = track;
            payload.stream_type = track->stream_type;
            payload.buffer_proxy = track->buffer_proxy;

            audio_mgr_dispatch(AUDIO_MSG_TRACK_SUSPEND, &payload);
        }
        break;

    case AUDIO_SESSION_STATE_RESTARTING:
        {
            track->state = state;
            audio_track_effect_stop(track);

            ret = audio_path_stop(track->path_handle);
            if (ret == false)
            {
                /* Restore the track state. */
                track->state = AUDIO_SESSION_STATE_STARTED;
                audio_track_effect_run(track);
                break;
            }
        }
        break;

    case AUDIO_SESSION_STATE_RELEASING:
        {
            /* Started audio track cannot be released directly, so it should be
             * stopped first before released.
             */
            track->state  = AUDIO_SESSION_STATE_STOPPING;
            track->action = AUDIO_TRACK_ACTION_RELEASE;
            audio_track_effect_stop(track);

            ret = audio_path_stop(track->path_handle);
            if (ret == false)
            {
                /* Restore the track state. */
                track->state  = AUDIO_SESSION_STATE_STARTED;
                track->action = AUDIO_TRACK_ACTION_NONE;
                audio_track_effect_run(track);
                break;
            }
        }
        break;
    }

    return ret;
}

static bool audio_track_stopping_handler(T_AUDIO_TRACK         *track,
                                         T_AUDIO_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_SESSION_STATE_RELEASED:
    case AUDIO_SESSION_STATE_CREATING:
    case AUDIO_SESSION_STATE_CREATED:
    case AUDIO_SESSION_STATE_STARTED:
    case AUDIO_SESSION_STATE_PAUSED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_SESSION_STATE_STARTING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                track->action = AUDIO_TRACK_ACTION_START;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* Drop repeating audio_track_start invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* Override the pending restart action for the stopping audio track. */
                track->action = AUDIO_TRACK_ACTION_START;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the stopping audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_STOPPING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                /* Drop repeating audio_track_stop invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* Cancel all pending actions when the stopping audio track is
                 * pending for start action, but receives the stop action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* Cancel all pending actions when the stopping audio track is
                 * pending for restart action, but receives the stop action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the stopping audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_STOPPED:
        {
            T_AUDIO_MSG_PAYLOAD_TRACK_STOP payload;

            track->state        = state;

            /* Reset the track's bound media buffer. */
            audio_track_buffer_reset(track);

            payload.handle      = track;
            payload.stream_type = track->stream_type;

            audio_mgr_dispatch(AUDIO_MSG_TRACK_STOP, &payload);

            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                audio_track_reschedule();
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = audio_session_state_set(track, AUDIO_SESSION_STATE_STARTING);
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = audio_session_state_set(track, AUDIO_SESSION_STATE_RESTARTING);
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = audio_session_state_set(track, AUDIO_SESSION_STATE_RELEASING);
            }
        }
        break;

    case AUDIO_SESSION_STATE_PAUSING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                /* Drop the pause action for the stopping audio track, as the
                 * pause action cannot override the stop action.
                 */
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* Cancel all pending actions when the stopping audio track is
                 * pending for start action, but receives the pause action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* Cancel all pending actions when the stopping audio track is
                 * pending for restart action, but receives the pause action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the stopping audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_RESTARTING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                track->action = AUDIO_TRACK_ACTION_RESTART;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* Override the pending start action for the stopping audio track. */
                track->action = AUDIO_TRACK_ACTION_RESTART;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* Drop repeating audio_track_start invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the stopping audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_RELEASING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                track->action = AUDIO_TRACK_ACTION_RELEASE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* Override the pending start action for the stopping audio track. */
                track->action = AUDIO_TRACK_ACTION_RELEASE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* Override the pending restart action for the stopping audio track. */
                track->action = AUDIO_TRACK_ACTION_RELEASE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* Drop repeating audio_track_release invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
        }
        break;
    }

    return ret;
}


static bool audio_track_stopped_handler(T_AUDIO_TRACK         *track,
                                        T_AUDIO_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_SESSION_STATE_RELEASED:
    case AUDIO_SESSION_STATE_CREATING:
    case AUDIO_SESSION_STATE_CREATED:
    case AUDIO_SESSION_STATE_STARTED:
    case AUDIO_SESSION_STATE_STOPPING:
    case AUDIO_SESSION_STATE_STOPPED:
    case AUDIO_SESSION_STATE_PAUSING:
    case AUDIO_SESSION_STATE_PAUSED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_SESSION_STATE_STARTING:
        {
            if (audio_track_db.multi_instance_enable == false)
            {
                /* Only one of the tracks with the same stream type can be playing. */
                audio_track_queue_freeze(track->stream_type);

                /* Suppress all audio playback tracks in the voice scenario. */
                if (track->stream_type == AUDIO_STREAM_TYPE_VOICE)
                {
                    audio_track_queue_freeze(AUDIO_STREAM_TYPE_PLAYBACK);
                }
                else if (track->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
                {
                    if (audio_track_is_started(AUDIO_STREAM_TYPE_VOICE))
                    {
                        track->action = AUDIO_TRACK_ACTION_START;
                        break;
                    }
                }
            }

            track->state = state;

            ret = audio_track_queue_remove(track);
            if (ret == false)
            {
                /* Restore the track state. */
                track->state = AUDIO_SESSION_STATE_STOPPED;
                break;
            }

            /* TODO record the latest track; check if it is, do not reset (pause) the bound media buffer. */
            /* Currently, reset the media buffer. */

            /* The latest started track has the highest priority. */
            if (track->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                audio_track_queue_add(&audio_track_db.playback_tracks, track, true);
            }
            else if (track->stream_type == AUDIO_STREAM_TYPE_VOICE)
            {
                audio_track_queue_add(&audio_track_db.voice_tracks, track, true);
            }
            else if (track->stream_type == AUDIO_STREAM_TYPE_RECORD)
            {
                audio_track_queue_add(&audio_track_db.record_tracks, track, true);
            }

            ret = audio_path_start(track->path_handle);
            if (ret == false)
            {
                /* TODO resume other tracks. */

                /* Restore the track state. */
                track->state = AUDIO_SESSION_STATE_STOPPED;
                break;
            }
        }
        break;

    case AUDIO_SESSION_STATE_RESTARTING:
        {
            if (audio_track_db.multi_instance_enable == false)
            {
                /* Only one of the tracks with the same stream type can be playing. */
                audio_track_queue_freeze(track->stream_type);

                /* Suppress all audio playback tracks in the voice scenario. */
                if (track->stream_type == AUDIO_STREAM_TYPE_VOICE)
                {
                    audio_track_queue_freeze(AUDIO_STREAM_TYPE_PLAYBACK);
                }
                else if (track->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
                {
                    if (audio_track_is_started(AUDIO_STREAM_TYPE_VOICE))
                    {
                        track->action = AUDIO_TRACK_ACTION_RESTART;
                        break;
                    }
                }
            }

            track->state = state;

            ret = audio_track_queue_remove(track);
            if (ret == false)
            {
                /* Restore the track state. */
                track->state = AUDIO_SESSION_STATE_STOPPED;
                break;
            }

            /* TODO record the latest track; check if it is, do not reset (pause) the bound media buffer. */
            /* Currently, reset the media buffer. */

            /* The latest started track has the highest priority. */
            if (track->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                audio_track_queue_add(&audio_track_db.playback_tracks, track, true);
            }
            else if (track->stream_type == AUDIO_STREAM_TYPE_VOICE)
            {
                audio_track_queue_add(&audio_track_db.voice_tracks, track, true);
            }
            else if (track->stream_type == AUDIO_STREAM_TYPE_RECORD)
            {
                audio_track_queue_add(&audio_track_db.record_tracks, track, true);
            }

            ret = audio_path_start(track->path_handle);
            if (ret == false)
            {
                /* TODO resume other tracks. */

                /* Restore the track state. */
                track->state = AUDIO_SESSION_STATE_STOPPED;
                break;
            }
        }
        break;

    case AUDIO_SESSION_STATE_RELEASING:
        {
            track->state = AUDIO_SESSION_STATE_RELEASING;

            /* Detach track's media buffer. */
            audio_track_buffer_detach(track);

            ret = audio_path_destroy(track->path_handle);
            if (ret == false)
            {
                /* Restore the track state. */
                track->state = AUDIO_SESSION_STATE_STOPPED;
                audio_track_buffer_attach(track);
                break;
            }
        }
        break;
    }

    return ret;
}

static bool audio_track_pausing_handler(T_AUDIO_TRACK         *track,
                                        T_AUDIO_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_SESSION_STATE_RELEASED:
    case AUDIO_SESSION_STATE_CREATING:
    case AUDIO_SESSION_STATE_CREATED:
    case AUDIO_SESSION_STATE_STARTED:
    case AUDIO_SESSION_STATE_PAUSED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_SESSION_STATE_STARTING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                track->action = AUDIO_TRACK_ACTION_START;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same pausing audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* Drop repeating audio_track_start invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will override the pausing state directly.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in pausing state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* Override the pending restart action for the pausing audio track. */
                track->action = AUDIO_TRACK_ACTION_START;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the pausing audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_STOPPING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                track->action = AUDIO_TRACK_ACTION_STOP;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same pausing audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* Cancel all pending actions when the pausing audio track is
                 * pending for start action, but receives the stop action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will override the pausing state directly.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in pausing state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* Cancel all pending actions when the pausing audio track is
                 * pending for restart action, but receives the stop action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the starting audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_STOPPED:
        {
            T_AUDIO_MSG_PAYLOAD_TRACK_PAUSE payload;

            track->state        = AUDIO_SESSION_STATE_PAUSED;

            payload.handle      = track;
            payload.stream_type = track->stream_type;
            payload.buffer_proxy = track->buffer_proxy;

            audio_mgr_dispatch(AUDIO_MSG_TRACK_PAUSE, &payload);

            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                audio_track_reschedule();
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same pausing audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = audio_session_state_set(track, AUDIO_SESSION_STATE_STARTING);
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will override the pausing state directly.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in pausing state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = audio_session_state_set(track, AUDIO_SESSION_STATE_RESTARTING);
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = audio_session_state_set(track, AUDIO_SESSION_STATE_RELEASING);
            }
        }
        break;

    case AUDIO_SESSION_STATE_PAUSING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                /* Drop repeating audio_track_pause invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same pausing audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* Cancel all pending actions when the pausing audio track is
                 * pending for start action, but receives the pause action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will override the pausing state directly.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in pausing state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* Cancel all pending actions when the pausing audio track is
                 * pending for restart action, but receives the pause action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the pausing audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_RESTARTING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                track->action = AUDIO_TRACK_ACTION_RESTART;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same pausing audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* Override the pending start action for the pausing audio track. */
                track->action = AUDIO_TRACK_ACTION_RESTART;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will override the pausing state directly.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in pausing state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* Drop repeating audio_track_restart invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the pausing audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_RELEASING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                track->action = AUDIO_TRACK_ACTION_RELEASE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same pausing audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* Override the pending start action for the pausing audio track. */
                track->action = AUDIO_TRACK_ACTION_RELEASE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* Override the pending start action for the pausing audio track. */
                track->action = AUDIO_TRACK_ACTION_RELEASE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* This case will not happen in reality, as the pending pause
                 * action will be dropped directly in pausing state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* Override the pending restart action for the pausing audio track. */
                track->action = AUDIO_TRACK_ACTION_RELEASE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* Drop repeating audio_track_release invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
        }
        break;
    }

    return ret;
}

static bool audio_track_paused_handler(T_AUDIO_TRACK         *track,
                                       T_AUDIO_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_SESSION_STATE_RELEASED:
    case AUDIO_SESSION_STATE_CREATING:
    case AUDIO_SESSION_STATE_CREATED:
    case AUDIO_SESSION_STATE_STOPPED:
    case AUDIO_SESSION_STATE_PAUSING:
    case AUDIO_SESSION_STATE_PAUSED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_SESSION_STATE_STARTING:
        {
            track->state = state;

            if (track->stream_type == AUDIO_STREAM_TYPE_VOICE)
            {
                audio_track_queue_freeze(AUDIO_STREAM_TYPE_PLAYBACK);
            }

            ret = audio_path_start(track->path_handle);

            if (ret == false)
            {
                track->state = AUDIO_SESSION_STATE_PAUSED;
            }
        }
        break;

    case AUDIO_SESSION_STATE_STARTED:
        {
            T_AUDIO_MSG_PAYLOAD_TRACK_START payload;

            track->state        = state;
            track->effect_apply = false;

            audio_track_volume_apply(track);

            media_buffer_proxy_fifo_report_config(track->buffer_proxy);

            payload.handle       = track;
            payload.stream_type  = track->stream_type;
            payload.buffer_proxy = track->buffer_proxy;

            /* This state changing event is triggered from audio path. */
            audio_mgr_dispatch(AUDIO_MSG_TRACK_SUSPEND_RESUME, &payload);
            audio_track_reschedule();
        }
        break;

    case AUDIO_SESSION_STATE_STOPPING:
        {
            track->state = state;

            ret = audio_path_stop(track->path_handle);
            if (ret == false)
            {
                /* Restore the track state. */
                track->state = AUDIO_SESSION_STATE_STARTED;
                break;
            }

        }
        break;

    case AUDIO_SESSION_STATE_RESTARTING:
        {
            track->state = state;

            /* Suppress all audio playback tracks in the voice scenario. */
            if (track->stream_type == AUDIO_STREAM_TYPE_VOICE)
            {
                audio_track_queue_freeze(AUDIO_STREAM_TYPE_PLAYBACK);
            }
            ret = audio_path_start(track->path_handle);
            if (ret == false)
            {
                track->state = AUDIO_SESSION_STATE_PAUSED;
            }
        }
        break;

    case AUDIO_SESSION_STATE_RELEASING:
        {
            track->state = AUDIO_SESSION_STATE_STOPPING;
            track->action = AUDIO_TRACK_ACTION_RELEASE;

            /* Reset the bound media buffer. */
            audio_track_buffer_reset(track);

            audio_path_stop(track->path_handle);
        }
        break;
    }

    return ret;
}

static bool audio_track_restarting_handler(T_AUDIO_TRACK         *track,
                                           T_AUDIO_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_SESSION_STATE_RELEASED:
    case AUDIO_SESSION_STATE_CREATING:
    case AUDIO_SESSION_STATE_CREATED:
    case AUDIO_SESSION_STATE_PAUSED:

        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_SESSION_STATE_STARTING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                /* Drop audio_track_start invocation. But this case is permitted. */
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same restarting audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in restarting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* Cancel all pending actions when the restarting audio track is
                 * pending for stop action, but receives the start action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* Cancel all pending actions when the restarting audio track is
                 * pending for pause action, but receives the start action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* This case will not happen in reality, as the pending restart
                 * action will be dropped directly in restarting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the restarting audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_STARTED:
        {
            T_AUDIO_MSG_PAYLOAD_TRACK_RESTART payload;

            track->state         = state;
            track->effect_apply  = false;
            payload.handle       = track;
            payload.stream_type  = track->stream_type;
            payload.buffer_proxy = track->buffer_proxy;

            audio_track_volume_apply(track);
            media_buffer_proxy_fifo_report_config(track->buffer_proxy);

            if (track->signal_out)
            {
                if (track->stream_type == AUDIO_STREAM_TYPE_PLAYBACK ||
                    track->stream_type == AUDIO_STREAM_TYPE_VOICE)
                {
                    audio_path_signal_monitoring_set(track->path_handle,
                                                     AUDIO_PATH_DIR_TX,
                                                     true,
                                                     track->signal_out_interval,
                                                     track->freq_out_num,
                                                     track->freq_out_table);
                }
            }

            if (track->signal_in)
            {
                if (track->stream_type == AUDIO_STREAM_TYPE_VOICE ||
                    track->stream_type == AUDIO_STREAM_TYPE_RECORD)
                {
                    audio_path_signal_monitoring_set(track->path_handle,
                                                     AUDIO_PATH_DIR_RX,
                                                     true,
                                                     track->signal_in_interval,
                                                     track->freq_in_num,
                                                     track->freq_in_table);
                }
            }

            audio_mgr_dispatch(AUDIO_MSG_TRACK_RESTART, &payload);

            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                audio_track_reschedule();
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same restarting audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in restarting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = audio_session_state_set(track, AUDIO_SESSION_STATE_STOPPING);
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = audio_session_state_set(track, AUDIO_SESSION_STATE_PAUSING);
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* This case will not happen in reality, as the pending restart
                 * action will be dropped directly in restarting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = audio_session_state_set(track, AUDIO_SESSION_STATE_RELEASING);
            }
        }
        break;


    case AUDIO_SESSION_STATE_STOPPING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                track->action = AUDIO_TRACK_ACTION_STOP;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same restarting audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in restarting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* Drop repeating audio_track_stop invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* Override the pending pause action for the restarting audio track. */
                track->action = AUDIO_TRACK_ACTION_STOP;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* This case will not happen in reality, as the pending restart
                 * action will be dropped directly in restarting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the restarting audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_STOPPED:
        {
            /* Reset the track's bound media buffer. */
            audio_track_buffer_reset(track);

            ret = audio_path_start(track->path_handle);
            if (ret == false)
            {
                track->state = state;
            }
        }
        break;

    case AUDIO_SESSION_STATE_PAUSING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                track->action = AUDIO_TRACK_ACTION_PAUSE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same restarting audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action of the same restarting audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* Override the pending stop action for the restarting audio track. */
                track->action = AUDIO_TRACK_ACTION_PAUSE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* This case will not happen in reality, as the pending restart
                 * action will be dropped directly in restarting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the restarting audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_RESTARTING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                /* Drop repeating audio_track_restart invocation. But this case is permitted. */
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same restarting audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in restarting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* Cancel all pending actions when the restarting audio track is
                 * pending for stop action, but receives the restart action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* Cancel all pending actions when the restarting audio track is
                 * pending for pause action, but receives the restart action later.
                 */
                track->action = AUDIO_TRACK_ACTION_NONE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* This case will not happen in reality, as the pending restart
                 * action will be dropped directly in restarting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* When the restarting audio track is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_SESSION_STATE_RELEASING:
        {
            if (track->action == AUDIO_TRACK_ACTION_NONE)
            {
                track->action = AUDIO_TRACK_ACTION_RELEASE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same restarting audio track is impossible.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in restarting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_STOP)
            {
                /* Override the pending stop action for the restarting audio track. */
                track->action = AUDIO_TRACK_ACTION_RELEASE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_PAUSE)
            {
                /* Override the pending pause action for the restarting audio track. */
                track->action = AUDIO_TRACK_ACTION_RELEASE;
                ret = true;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RESTART)
            {
                /* This case will not happen in reality, as the pending restart
                 * action will be dropped directly in restarting state.
                 */
                ret = false;
            }
            else if (track->action == AUDIO_TRACK_ACTION_RELEASE)
            {
                /* Drop repeating audio_track_release invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
        }
        break;
    }

    return ret;
}


static bool audio_track_releasing_handler(T_AUDIO_TRACK         *track,
                                          T_AUDIO_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_SESSION_STATE_CREATING:
    case AUDIO_SESSION_STATE_CREATED:
    case AUDIO_SESSION_STATE_STARTING:
    case AUDIO_SESSION_STATE_STARTED:
    case AUDIO_SESSION_STATE_STOPPING:
    case AUDIO_SESSION_STATE_STOPPED:
    case AUDIO_SESSION_STATE_PAUSING:
    case AUDIO_SESSION_STATE_PAUSED:
    case AUDIO_SESSION_STATE_RESTARTING:
    case AUDIO_SESSION_STATE_RELEASING:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_SESSION_STATE_RELEASED:
        {
            T_AUDIO_MSG_PAYLOAD_TRACK_RELEASE payload;

            track->state        = state;
            track->action       = AUDIO_TRACK_ACTION_NONE;

            payload.handle      = track;
            payload.stream_type = track->stream_type;

            audio_track_effect_clear(track);
            (void)audio_track_queue_remove(track);

            if (track->freq_in_table != NULL)
            {
                free(track->freq_in_table);
            }

            if (track->freq_out_table != NULL)
            {
                free(track->freq_out_table);
            }

            if (track->channel_in_array != NULL)
            {
                free(track->channel_in_array);
            }

            if (track->channel_out_array != NULL)
            {
                free(track->channel_out_array);
            }

            audio_mgr_dispatch(AUDIO_MSG_TRACK_RELEASE, &payload);
            free(track);
            audio_track_reschedule();
        }
        break;
    }

    return ret;
}

static const T_AUDIO_TRACK_STATE_HANDLER audio_track_state_handler[] =
{
    audio_track_released_handler,
    audio_track_creating_handler,
    audio_track_created_handler,
    audio_track_starting_handler,
    audio_track_started_handler,
    audio_track_stopping_handler,
    audio_track_stopped_handler,
    audio_track_pausing_handler,
    audio_track_paused_handler,
    audio_track_restarting_handler,
    audio_track_releasing_handler,
};

static bool audio_session_state_set(T_AUDIO_TRACK         *track,
                                    T_AUDIO_SESSION_STATE  state)
{
    AUDIO_PRINT_TRACE5("audio_session_state_set: track %p, curr state %u, next state %u, action %u, codec %u",
                       track, track->state, state, track->action, track->format_info.type);

    return audio_track_state_handler[track->state](track, state);
}

bool audio_track_init(uint16_t playback_pool_size,
                      uint16_t voice_pool_size,
                      uint16_t record_pool_size)
{
    os_queue_init(&audio_track_db.idle_tracks);
    os_queue_init(&audio_track_db.playback_tracks);
    os_queue_init(&audio_track_db.voice_tracks);
    os_queue_init(&audio_track_db.record_tracks);

    audio_track_db.playback_buffer_owner = NULL;
    audio_track_db.voice_buffer_owner    = NULL;
    audio_track_db.record_buffer_owner   = NULL;

    audio_track_db.playback_volume_max      = VOLUME_PLAYBACK_MAX_LEVEL;
    audio_track_db.playback_volume_min      = VOLUME_PLAYBACK_MIN_LEVEL;
    audio_track_db.playback_volume_default  = VOLUME_PLAYBACK_DEFAULT_LEVEL;
    audio_track_db.playback_volume_muted    = false;
    audio_track_db.playback_volume_scale    = 0.0f;

    audio_track_db.voice_volume_out_max     = VOLUME_VOICE_OUT_MAX_LEVEL;
    audio_track_db.voice_volume_out_min     = VOLUME_VOICE_OUT_MIN_LEVEL;
    audio_track_db.voice_volume_out_default = VOLUME_VOICE_OUT_DEFAULT_LEVEL;
    audio_track_db.voice_volume_out_muted   = false;
    audio_track_db.voice_volume_out_scale   = 0.0f;

    audio_track_db.voice_volume_in_max      = VOLUME_VOICE_IN_MAX_LEVEL;
    audio_track_db.voice_volume_in_min      = VOLUME_VOICE_IN_MIN_LEVEL;
    audio_track_db.voice_volume_in_default  = VOLUME_VOICE_IN_DEFAULT_LEVEL;
    audio_track_db.voice_volume_in_muted    = false;

    audio_track_db.record_volume_max        = VOLUME_RECORD_MAX_LEVEL;
    audio_track_db.record_volume_min        = VOLUME_RECORD_MIN_LEVEL;
    audio_track_db.record_volume_default    = VOLUME_RECORD_DEFAULT_LEVEL;
    audio_track_db.record_volume_muted      = false;

    audio_track_db.multi_instance_enable    = false;

    return media_buffer_init(playback_pool_size, voice_pool_size, record_pool_size);
}

void audio_track_deinit(void)
{
    T_AUDIO_TRACK *track;

    media_buffer_deinit();

    track = os_queue_out(&audio_track_db.idle_tracks);
    while (track != NULL)
    {
        if (track->freq_in_table != NULL)
        {
            free(track->freq_in_table);
        }

        if (track->freq_out_table != NULL)
        {
            free(track->freq_out_table);
        }

        if (track->channel_in_array != NULL)
        {
            free(track->channel_in_array);
        }

        if (track->channel_out_array != NULL)
        {
            free(track->channel_out_array);
        }

        free(track);
        track = os_queue_out(&audio_track_db.idle_tracks);
    }

    track = os_queue_out(&audio_track_db.playback_tracks);
    while (track != NULL)
    {
        if (track->freq_in_table != NULL)
        {
            free(track->freq_in_table);
        }

        if (track->freq_out_table != NULL)
        {
            free(track->freq_out_table);
        }

        if (track->channel_in_array != NULL)
        {
            free(track->channel_in_array);
        }

        if (track->channel_out_array != NULL)
        {
            free(track->channel_out_array);
        }

        free(track);
        track = os_queue_out(&audio_track_db.playback_tracks);
    }

    track = os_queue_out(&audio_track_db.voice_tracks);
    while (track != NULL)
    {
        if (track->freq_in_table != NULL)
        {
            free(track->freq_in_table);
        }

        if (track->freq_out_table != NULL)
        {
            free(track->freq_out_table);
        }

        if (track->channel_in_array != NULL)
        {
            free(track->channel_in_array);
        }

        if (track->channel_out_array != NULL)
        {
            free(track->channel_out_array);
        }

        free(track);
        track = os_queue_out(&audio_track_db.voice_tracks);
    }

    track = os_queue_out(&audio_track_db.record_tracks);
    while (track != NULL)
    {
        if (track->freq_in_table != NULL)
        {
            free(track->freq_in_table);
        }

        if (track->freq_out_table != NULL)
        {
            free(track->freq_out_table);
        }

        if (track->channel_in_array != NULL)
        {
            free(track->channel_in_array);
        }

        if (track->channel_out_array != NULL)
        {
            free(track->channel_out_array);
        }

        free(track);
        track = os_queue_out(&audio_track_db.record_tracks);
    }
}

bool audio_track_policy_set(T_AUDIO_TRACK_POLICY policy)
{
    if (policy == AUDIO_TRACK_POLICY_SINGLE_STREAM)
    {
        audio_track_db.multi_instance_enable = false;
    }
    else if (policy == AUDIO_TRACK_POLICY_MULTI_STREAM)
    {
        audio_track_db.multi_instance_enable = true;
    }

    return true;
}

T_AUDIO_TRACK_HANDLE audio_track_create(T_AUDIO_STREAM_TYPE    stream_type,
                                        T_AUDIO_STREAM_MODE    mode,
                                        T_AUDIO_STREAM_USAGE   usage,
                                        T_AUDIO_FORMAT_INFO    format_info,
                                        uint8_t                volume_out,
                                        uint8_t                volume_in,
                                        uint32_t               device,
                                        P_AUDIO_TRACK_ASYNC_IO async_write,
                                        P_AUDIO_TRACK_ASYNC_IO async_read)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = calloc(1, sizeof(T_AUDIO_TRACK));
    if (track == NULL)
    {
        ret = 1;
        goto fail_alloc_track;
    }

    AUDIO_PRINT_INFO7("audio_track_create: handle %p, stream_type %u, mode %u, usage %u, "
                      "device 0x%08x, async_write %p, async_read %p",
                      track, stream_type, mode, usage, device, async_write, async_read);

    track->container            = NULL;
    track->state                = AUDIO_SESSION_STATE_RELEASED;
    track->action               = AUDIO_TRACK_ACTION_NONE;
    track->stream_type          = stream_type;
    track->mode                 = mode;
    track->usage                = usage;
    track->format_info          = format_info;
    track->device               = device;
    track->latency_fixed        = false;
    track->buffer_proxy         = NULL;
    track->async_write          = async_write;
    track->async_read           = async_read;
    track->path_handle          = NULL;
    track->volume_out_muted     = false;
    track->volume_in_muted      = false;
    track->signal_in            = false;
    track->signal_out           = false;
    track->freq_in_num          = 0;
    track->freq_out_num         = 0;
    track->freq_in_table        = NULL;
    track->freq_out_table       = NULL;
    track->signal_in_interval   = 1000;
    track->signal_out_interval  = 1000;
    track->channel_in_num       = 0;
    track->channel_out_num      = 0;
    track->channel_in_array     = NULL;
    track->channel_out_array    = NULL;

    if (track->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
    {
        track->latency          = AUDIO_TRACK_PLAYBACK_DEFAULT_LATENCY;
        track->volume_out_max   = audio_track_db.playback_volume_max;
        track->volume_out_min   = audio_track_db.playback_volume_min;
        track->volume_out       = audio_track_db.playback_volume_default;

        if (volume_out >= track->volume_out_min &&
            volume_out <= track->volume_out_max)
        {
            track->volume_out = volume_out;
        }
    }
    else if (track->stream_type == AUDIO_STREAM_TYPE_VOICE)
    {
        track->latency          = AUDIO_TRACK_VOICE_DEFAULT_LATENCY;
        track->volume_out_max   = audio_track_db.voice_volume_out_max;
        track->volume_out_min   = audio_track_db.voice_volume_out_min;
        track->volume_out       = audio_track_db.voice_volume_out_default;
        track->volume_in_max    = audio_track_db.voice_volume_in_max;
        track->volume_in_min    = audio_track_db.voice_volume_in_min;
        track->volume_in        = audio_track_db.voice_volume_in_default;

        if (volume_out >= track->volume_out_min &&
            volume_out <= track->volume_out_max)
        {
            track->volume_out = volume_out;
        }

        if (volume_in >= track->volume_in_min &&
            volume_in <= track->volume_in_max)
        {
            track->volume_in = volume_in;
        }
    }
    else
    {
        track->volume_in_max    = audio_track_db.record_volume_max;
        track->volume_in_min    = audio_track_db.record_volume_min;
        track->volume_in        = audio_track_db.record_volume_default;

        if (volume_in >= track->volume_in_min &&
            volume_in <= track->volume_in_max)
        {
            track->volume_in = volume_in;
        }
    }

    track->effect_apply  = false;
    track->effect_bypass = 0;
    os_queue_init(&track->effects);

    if (audio_session_state_set(track, AUDIO_SESSION_STATE_CREATING) == false)
    {
        ret = 2;
        goto fail_set_state;
    }

    return (T_AUDIO_TRACK_HANDLE)track;

fail_set_state:
    free(track);
fail_alloc_track:
    AUDIO_PRINT_ERROR1("audio_track_create: failed %d", -ret);
    return NULL;
}

bool audio_track_start(T_AUDIO_TRACK_HANDLE handle)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    AUDIO_PRINT_INFO3("audio_track_start: handle %p, state %u, action %u",
                      handle, track->state, track->action);

    if (audio_session_state_set(track, AUDIO_SESSION_STATE_STARTING) == false)
    {
        ret = 2;
        goto fail_set_state;
    }
    return true;

fail_set_state:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_start: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_track_restart(T_AUDIO_TRACK_HANDLE handle)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    AUDIO_PRINT_INFO3("audio_track_restart: handle %p, state %u, action %u",
                      handle, track->state, track->action);

    if (audio_session_state_set(track, AUDIO_SESSION_STATE_RESTARTING) == false)
    {
        ret = 2;
        goto fail_set_state;
    }
    return true;

fail_set_state:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_restart: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_track_pause(T_AUDIO_TRACK_HANDLE handle)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    AUDIO_PRINT_INFO3("audio_track_pause: handle %p, state %u, action %u",
                      handle, track->state, track->action);

    if (audio_session_state_set(track, AUDIO_SESSION_STATE_PAUSING) == false)
    {
        ret = 2;
        goto fail_set_state;
    }

    return true;

fail_set_state:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_pause: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_track_stop(T_AUDIO_TRACK_HANDLE handle)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    AUDIO_PRINT_INFO3("audio_track_stop: handle %p, state %u, action %u",
                      handle, track->state, track->action);

    if (audio_session_state_set(track, AUDIO_SESSION_STATE_STOPPING) == false)
    {
        ret = 2;
        goto fail_set_state;
    }

    return true;

fail_set_state:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_stop: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_track_flush(T_AUDIO_TRACK_HANDLE handle)
{
    /* TODO */
    return false;
}

bool audio_track_release(T_AUDIO_TRACK_HANDLE handle)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    AUDIO_PRINT_INFO3("audio_track_release: handle %p, state %u, action %u",
                      handle, track->state, track->action);

    if (audio_session_state_set(track, AUDIO_SESSION_STATE_RELEASING) == false)
    {
        ret = 2;
        goto fail_set_state;
    }

    return true;

fail_set_state:
fail_invalid_handle:
    AUDIO_PRINT_ERROR2("audio_track_release: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_track_write(T_AUDIO_TRACK_HANDLE   handle,
                       uint32_t               timestamp,
                       uint16_t               seq_num,
                       T_AUDIO_STREAM_STATUS  status,
                       uint8_t                frame_num,
                       void                  *buf,
                       uint16_t               len,
                       uint16_t              *written_len)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (track->stream_type == AUDIO_STREAM_TYPE_PLAYBACK &&
        track->latency_fixed == false &&
        track->mode == AUDIO_STREAM_MODE_LOW_LATENCY)
    {
        float var;
        media_buffer_proxy_inteval_evaluator_update(track->buffer_proxy, seq_num, timestamp);
        if (media_buffer_proxy_inteval_evaluator_get_var(track->buffer_proxy, &var))
        {
            media_buffer_proxy_latency_set2(track->buffer_proxy, track->latency, track->latency_fixed, var);
        }
    }

    if (track->async_write != NULL)
    {
        ret = 2;
        goto fail_invalid_write;
    }

    if (track->state != AUDIO_SESSION_STATE_STARTED)
    {
        ret = 3;
        goto fail_invalid_state;
    }

    if (media_buffer_proxy_ds_write(track->buffer_proxy,
                                    timestamp,
                                    seq_num,
                                    status,
                                    frame_num,
                                    buf,
                                    len,
                                    written_len) == false)
    {
        ret = 4;
        goto fail_write_buffer;
    }

    return true;

fail_write_buffer:
fail_invalid_state:
fail_invalid_write:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_write: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_track_read(T_AUDIO_TRACK_HANDLE   handle,
                      uint32_t              *timestamp,
                      uint16_t              *seq_num,
                      T_AUDIO_STREAM_STATUS *status,
                      uint8_t               *frame_num,
                      void                  *buf,
                      uint16_t               len,
                      uint16_t              *read_len)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (track->async_read != NULL)
    {
        ret = 2;
        goto fail_invalid_read;
    }

    if (track->state != AUDIO_SESSION_STATE_STARTED)
    {
        ret = 3;
        goto fail_invalid_state;
    }

    *read_len = media_buffer_proxy_us_read(track->buffer_proxy,
                                           buf,
                                           len,
                                           timestamp,
                                           seq_num,
                                           status,
                                           frame_num);
    return true;

fail_invalid_state:
fail_invalid_read:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_read: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_track_uuid_set(T_AUDIO_TRACK_HANDLE handle,
                          uint8_t              uuid[8])
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    AUDIO_PRINT_INFO2("audio_track_uuid_set: handle %p, uuid %b", handle, TRACE_BINARY(8, uuid));

    media_buffer_proxy_uuid_set(track->buffer_proxy, uuid);
    return true;

fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_uuid_set: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_track_threshold_set(T_AUDIO_TRACK_HANDLE handle,
                               uint16_t             upper_threshold,
                               uint16_t             lower_threshold)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    if (track->stream_type != AUDIO_STREAM_TYPE_PLAYBACK)
    {
        ret = 2;
        goto fail_invalid_type;
    }

    AUDIO_PRINT_INFO3("audio_track_threshold_set: handle %p, upper_threshold %u, lower_threshold %u",
                      handle, upper_threshold, lower_threshold);

    media_buffer_proxy_threshold_set(track->buffer_proxy, upper_threshold, lower_threshold);
    return true;

fail_invalid_type:
fail_invalid_handle:
    AUDIO_PRINT_ERROR2("audio_track_threshold_set: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_track_state_get(T_AUDIO_TRACK_HANDLE  handle,
                           T_AUDIO_TRACK_STATE  *state)
{
    T_AUDIO_TRACK *track;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == true)
    {
        switch (track->state)
        {
        case AUDIO_SESSION_STATE_RELEASED:
        case AUDIO_SESSION_STATE_RELEASING:
            *state = AUDIO_TRACK_STATE_RELEASED;
            break;

        case AUDIO_SESSION_STATE_CREATING:
        case AUDIO_SESSION_STATE_CREATED:
            *state = AUDIO_TRACK_STATE_CREATED;
            break;

        case AUDIO_SESSION_STATE_STARTING:
        case AUDIO_SESSION_STATE_STARTED:
        case AUDIO_SESSION_STATE_RESTARTING:
            *state = AUDIO_TRACK_STATE_STARTED;
            break;

        case AUDIO_SESSION_STATE_STOPPING:
        case AUDIO_SESSION_STATE_STOPPED:
            *state = AUDIO_TRACK_STATE_STOPPED;
            break;

        case AUDIO_SESSION_STATE_PAUSING:
        case AUDIO_SESSION_STATE_PAUSED:
            *state = AUDIO_TRACK_STATE_PAUSED;
            break;
        }

        return true;
    }

    return false;
}

bool audio_track_stream_type_get(T_AUDIO_TRACK_HANDLE  handle,
                                 T_AUDIO_STREAM_TYPE  *stream_type)
{
    T_AUDIO_TRACK *track;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == true)
    {
        *stream_type = track->stream_type;
        return true;
    }

    return false;
}

bool audio_track_format_info_get(T_AUDIO_TRACK_HANDLE  handle,
                                 T_AUDIO_FORMAT_INFO  *format_info)
{
    T_AUDIO_TRACK *track;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == true)
    {
        *format_info = track->format_info;
        return true;
    }

    return false;
}

bool audio_track_device_get(T_AUDIO_TRACK_HANDLE  handle,
                            uint32_t             *device)
{
    T_AUDIO_TRACK *track;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == true)
    {
        *device = track->device;
        return true;
    }

    return false;
}

bool audio_track_mode_get(T_AUDIO_TRACK_HANDLE  handle,
                          T_AUDIO_STREAM_MODE  *mode)
{
    T_AUDIO_TRACK *track;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == true)
    {
        *mode = track->mode;
        return true;
    }

    return false;
}

bool audio_track_usage_get(T_AUDIO_TRACK_HANDLE  handle,
                           T_AUDIO_STREAM_USAGE *usage)

{
    T_AUDIO_TRACK *track;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == true)
    {
        *usage = track->usage;
        return true;
    }

    return false;
}

bool audio_track_latency_max_get(T_AUDIO_TRACK_HANDLE  handle,
                                 uint16_t             *latency)
{
    /* TODO */
    return false;
}

bool audio_track_latency_min_get(T_AUDIO_TRACK_HANDLE  handle,
                                 uint16_t             *latency)
{
    /* TODO */
    return false;
}

bool audio_track_latency_get(T_AUDIO_TRACK_HANDLE  handle,
                             uint16_t             *latency)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    *latency = track->latency;
    return true;

fail_invalid_handle:
    AUDIO_PRINT_ERROR2("audio_track_latency_get: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_track_latency_set(T_AUDIO_TRACK_HANDLE handle,
                             uint16_t             latency,
                             bool                 fixed)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    AUDIO_PRINT_INFO3("audio_track_latency_set: handle %p, latency %u fixed %u",
                      handle, latency, fixed);

    track->latency_fixed = fixed;
    track->latency = latency;
    media_buffer_proxy_latency_set(track->buffer_proxy, latency, fixed);
    return true;

fail_invalid_handle:
    AUDIO_PRINT_ERROR2("audio_track_latency_set: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_track_plc_notify_set(T_AUDIO_TRACK_HANDLE handle,
                                bool                 enable,
                                uint16_t             interval,
                                uint32_t             threshold)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    audio_path_decoder_plc_set(track->path_handle, enable, interval, threshold);
    return true;

fail_invalid_handle:
    AUDIO_PRINT_ERROR2("audio_track_plc_notify_set: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_track_buffer_level_get(T_AUDIO_TRACK_HANDLE  handle,
                                  uint16_t             *level)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    if (media_buffer_proxy_level_get(track->buffer_proxy, level) == false)
    {
        ret = 2;
        goto fail_invalid_level;
    }

    return true;

fail_invalid_level:
fail_invalid_handle:
    AUDIO_PRINT_ERROR2("audio_track_buffer_level_get: handle %p, failed %d", handle, -ret);
    return false;
}

static uint8_t audio_track_volume_scale(uint8_t volume,
                                        uint8_t src_volume,
                                        uint8_t dst_volume)
{
    return (volume * dst_volume + src_volume / 2) / src_volume;
}

bool audio_track_volume_out_max_get(T_AUDIO_TRACK_HANDLE  handle,
                                    uint8_t              *volume)
{
    T_AUDIO_TRACK *track;
    int32_t ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (track->stream_type != AUDIO_STREAM_TYPE_PLAYBACK &&
        track->stream_type != AUDIO_STREAM_TYPE_VOICE)
    {
        ret = 2;
        goto fail_check_type;
    }

    *volume = track->volume_out_max;
    return true;

fail_check_type:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_volume_out_max_get: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_volume_out_max_set(T_AUDIO_STREAM_TYPE type,
                                    uint8_t             volume)
{
    T_OS_QUEUE    *queue;
    T_AUDIO_TRACK *track;
    uint8_t        volume_max = 0;
    uint8_t        volume_min = 0;
    uint8_t        volume_default = 0;

    switch (type)
    {
    case AUDIO_STREAM_TYPE_PLAYBACK:
        volume_max      = audio_track_db.playback_volume_max;
        volume_min      = audio_track_db.playback_volume_min;
        volume_default  = audio_track_db.playback_volume_default;

        audio_track_db.playback_volume_min = audio_track_volume_scale(volume_min,
                                                                      volume_max,
                                                                      volume);
        audio_track_db.playback_volume_default = audio_track_volume_scale(volume_default,
                                                                          volume_max,
                                                                          volume);
        audio_track_db.playback_volume_max = volume;

        queue = &audio_track_db.playback_tracks;
        break;

    case AUDIO_STREAM_TYPE_VOICE:
        volume_max      = audio_track_db.voice_volume_out_max;
        volume_min      = audio_track_db.voice_volume_out_min;
        volume_default  = audio_track_db.voice_volume_out_default;

        audio_track_db.voice_volume_out_min = audio_track_volume_scale(volume_min,
                                                                       volume_max,
                                                                       volume);
        audio_track_db.voice_volume_out_default = audio_track_volume_scale(volume_default,
                                                                           volume_max,
                                                                           volume);
        audio_track_db.voice_volume_out_max = volume;

        queue = &audio_track_db.voice_tracks;
        break;

    case AUDIO_STREAM_TYPE_RECORD:
        return false;
    }

    if (volume != volume_max)
    {
        track = os_queue_peek(queue, 0);
        while (track != NULL)
        {
            track->volume_out_min   = audio_track_volume_scale(track->volume_out_min,
                                                               track->volume_out_max,
                                                               volume);
            track->volume_out       = audio_track_volume_scale(track->volume_out,
                                                               track->volume_out_max,
                                                               volume);
            track->volume_out_max   = volume;
            track                   = track->p_next;
        }

        track = os_queue_peek(&audio_track_db.idle_tracks, 0);
        while (track != NULL)
        {
            if (track->stream_type == type)
            {
                track->volume_out_min   = audio_track_volume_scale(track->volume_out_min,
                                                                   track->volume_out_max,
                                                                   volume);
                track->volume_out       = audio_track_volume_scale(track->volume_out,
                                                                   track->volume_out_max,
                                                                   volume);
                track->volume_out_max   = volume;
            }

            track = track->p_next;
        }
    }

    return true;
}

bool audio_track_volume_out_min_get(T_AUDIO_TRACK_HANDLE  handle,
                                    uint8_t              *volume)
{
    T_AUDIO_TRACK *track;
    int32_t ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (track->stream_type != AUDIO_STREAM_TYPE_PLAYBACK &&
        track->stream_type != AUDIO_STREAM_TYPE_VOICE)
    {
        ret = 2;
        goto fail_check_type;
    }

    *volume = track->volume_out_min;
    return true;

fail_check_type:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_volume_out_min_get: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_volume_out_min_set(T_AUDIO_STREAM_TYPE type,
                                    uint8_t             volume)
{
    T_OS_QUEUE    *queue;
    T_AUDIO_TRACK *track;
    uint8_t        volume_min = 0;

    switch (type)
    {
    case AUDIO_STREAM_TYPE_PLAYBACK:
        volume_min = audio_track_db.playback_volume_min;
        audio_track_db.playback_volume_min = volume;
        queue = &audio_track_db.playback_tracks;
        break;

    case AUDIO_STREAM_TYPE_VOICE:
        volume_min = audio_track_db.voice_volume_out_min;
        audio_track_db.voice_volume_out_min = volume;
        queue = &audio_track_db.voice_tracks;
        break;

    case AUDIO_STREAM_TYPE_RECORD:
        return false;
    }

    if (volume != volume_min)
    {
        track = os_queue_peek(queue, 0);
        while (track != NULL)
        {
            track->volume_out_min = volume;
            track                 = track->p_next;
        }

        track = os_queue_peek(&audio_track_db.idle_tracks, 0);
        while (track != NULL)
        {
            if (track->stream_type == type)
            {
                track->volume_out_min = volume;
            }

            track = track->p_next;
        }
    }

    return true;
}

bool audio_track_volume_out_get(T_AUDIO_TRACK_HANDLE  handle,
                                uint8_t              *volume)
{
    T_AUDIO_TRACK *track;
    int32_t ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (track->stream_type != AUDIO_STREAM_TYPE_PLAYBACK &&
        track->stream_type != AUDIO_STREAM_TYPE_VOICE)
    {
        ret = 2;
        goto fail_check_type;
    }

    *volume = track->volume_out;
    return true;

fail_check_type:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_volume_out_get: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_volume_out_set(T_AUDIO_TRACK_HANDLE handle,
                                uint8_t              volume)
{
    T_AUDIO_TRACK *track;
    T_AUDIO_MSG_PAYLOAD_TRACK_VOLUME_OUT_CHANGE payload;
    int32_t ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    AUDIO_PRINT_INFO2("audio_track_volume_out_set: volume %u, volume_out_muted %u",
                      volume, track->volume_out_muted);

    if (volume < track->volume_out_min ||
        volume > track->volume_out_max)
    {
        ret = 2;
        goto fail_check_volume;
    }

    if (track->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
    {
        if ((audio_track_db.playback_volume_muted == false) &&
            (track->volume_out_muted == false))
        {
            audio_path_dac_level_set(track->path_handle,
                                     volume,
                                     audio_track_db.playback_volume_scale);
        }
    }
    else if (track->stream_type == AUDIO_STREAM_TYPE_VOICE)
    {
        if ((audio_track_db.voice_volume_out_muted == false) &&
            (track->volume_out_muted == false))
        {
            audio_path_dac_level_set(track->path_handle,
                                     volume,
                                     audio_track_db.voice_volume_out_scale);
        }
    }
    else
    {
        ret = 3;
        goto fail_check_type;
    }

    payload.handle          = track;
    payload.prev_volume     = track->volume_out;
    payload.curr_volume     = volume;

    track->volume_out       = volume;

    return audio_mgr_dispatch(AUDIO_MSG_TRACK_VOLUME_OUT_CHANGE, &payload);

fail_check_type:
fail_check_volume:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_volume_out_set: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_volume_out_mute(T_AUDIO_TRACK_HANDLE handle)
{
    T_AUDIO_TRACK *track;
    T_AUDIO_MSG_PAYLOAD_TRACK_VOLUME_OUT_MUTE payload;
    int32_t ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (track->stream_type != AUDIO_STREAM_TYPE_PLAYBACK &&
        track->stream_type != AUDIO_STREAM_TYPE_VOICE)
    {
        ret = 2;
        goto fail_check_type;
    }

    if (track->volume_out_muted == true)
    {
        ret = 3;
        goto fail_check_muted;
    }

    audio_path_dac_mute(track->path_handle);

    track->volume_out_muted = true;

    payload.handle  = track;
    payload.volume  = track->volume_out;

    audio_mgr_dispatch(AUDIO_MSG_TRACK_VOLUME_OUT_MUTE, &payload);
    return true;

fail_check_muted:
fail_check_type:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_volume_out_mute: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_volume_out_unmute(T_AUDIO_TRACK_HANDLE handle)
{
    T_AUDIO_TRACK *track;
    T_AUDIO_MSG_PAYLOAD_TRACK_VOLUME_OUT_UNMUTE payload;
    int32_t ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (track->stream_type != AUDIO_STREAM_TYPE_PLAYBACK &&
        track->stream_type != AUDIO_STREAM_TYPE_VOICE)
    {
        ret = 2;
        goto fail_check_type;
    }

    if (track->volume_out_muted == false)
    {
        ret = 3;
        goto fail_check_muted;
    }

    if ((track->stream_type == AUDIO_STREAM_TYPE_PLAYBACK) &&
        (audio_track_db.playback_volume_muted == false))
    {
        audio_path_dac_level_set(track->path_handle,
                                 track->volume_out,
                                 audio_track_db.playback_volume_scale);
    }
    else if ((track->stream_type == AUDIO_STREAM_TYPE_VOICE) &&
             (audio_track_db.voice_volume_out_muted == false))
    {
        audio_path_dac_level_set(track->path_handle,
                                 track->volume_out,
                                 audio_track_db.voice_volume_out_scale);
    }

    track->volume_out_muted = false;

    payload.handle  = track;
    payload.volume  = track->volume_out;

    audio_mgr_dispatch(AUDIO_MSG_TRACK_VOLUME_OUT_UNMUTE, &payload);
    return true;

fail_check_muted:
fail_check_type:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_volume_out_unmute: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_volume_out_is_muted(T_AUDIO_TRACK_HANDLE  handle,
                                     bool                 *is_muted)
{
    T_AUDIO_TRACK *track;
    int32_t ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    *is_muted = track->volume_out_muted;
    return true;

fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_volume_out_is_muted: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_channel_out_set(T_AUDIO_TRACK_HANDLE handle,
                                 uint8_t              channel_num,
                                 uint32_t             channel_array[])
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (track->stream_type != AUDIO_STREAM_TYPE_PLAYBACK &&
        track->stream_type != AUDIO_STREAM_TYPE_VOICE)
    {
        ret = 2;
        goto fail_check_type;
    }

    if (channel_num == 0)
    {
        ret = 3;
        goto fail_check_num;
    }

    if (track->state == AUDIO_SESSION_STATE_STARTED)
    {
        if (audio_path_channel_out_reorder(track->path_handle,
                                           channel_num,
                                           channel_array) == false)
        {
            ret = 4;
            goto fail_reorder_channel;
        }
    }

    track->channel_out_num   = channel_num;
    track->channel_out_array = malloc(4 * track->channel_out_num);
    if (track->channel_out_array == NULL)
    {
        ret = 5;
        goto fail_alloc_array;
    }

    memcpy(track->channel_out_array, channel_array, 4 * channel_num);
    return true;

fail_alloc_array:
fail_reorder_channel:
fail_check_num:
fail_check_type:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_channel_out_set: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_channel_in_set(T_AUDIO_TRACK_HANDLE handle,
                                uint8_t              channel_num,
                                uint32_t             channel_array[])
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;
    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (track->stream_type != AUDIO_STREAM_TYPE_RECORD &&
        track->stream_type != AUDIO_STREAM_TYPE_VOICE)
    {
        ret = 2;
        goto fail_check_type;
    }

    if (channel_num == 0)
    {
        ret = 3;
        goto fail_check_num;
    }

    if (track->state == AUDIO_SESSION_STATE_STARTED)
    {
        if (audio_path_channel_in_reorder(track->path_handle,
                                          channel_num,
                                          channel_array) == false)
        {
            ret = 4;
            goto fail_reorder_channel;
        }
    }

    track->channel_in_num   = channel_num;
    track->channel_in_array = malloc(4 * track->channel_in_num);
    if (track->channel_in_array == NULL)
    {
        ret = 5;
        goto fail_alloc_array;
    }

    memcpy(track->channel_in_array, channel_array, 4 * channel_num);
    return true;

fail_alloc_array:
fail_reorder_channel:
fail_check_num:
fail_check_type:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_channel_in_set: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_signal_out_monitoring_start(T_AUDIO_TRACK_HANDLE handle,
                                             uint16_t             refresh_interval,
                                             uint8_t              freq_num,
                                             uint32_t             freq_table[])
{
    T_AUDIO_TRACK *track;
    int32_t ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (track->stream_type != AUDIO_STREAM_TYPE_PLAYBACK &&
        track->stream_type != AUDIO_STREAM_TYPE_VOICE)
    {
        ret = 2;
        goto fail_check_type;
    }

    if (track->signal_out == true)
    {
        ret = 3;
        goto fail_check_signal_out;
    }

    if (track->state == AUDIO_SESSION_STATE_STARTED)
    {
        if (audio_path_signal_monitoring_set(track->path_handle,
                                             AUDIO_PATH_DIR_TX,
                                             true,
                                             refresh_interval,
                                             freq_num,
                                             freq_table) == false)
        {
            ret = 4;
            goto fail_set_signal;
        }
    }

    track->signal_out          = true;
    track->signal_out_interval = refresh_interval;
    track->freq_out_num        = freq_num;
    track->freq_out_table      = malloc(track->freq_out_num * 4);
    if (track->freq_out_table == NULL)
    {
        ret = 5;
        goto fail_alloc_freq_table;
    }

    memcpy(track->freq_out_table, freq_table, 4 * freq_num);

    return true;

fail_alloc_freq_table:
fail_set_signal:
fail_check_signal_out:
fail_check_type:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_signal_out_monitoring_start: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_signal_out_monitoring_stop(T_AUDIO_TRACK_HANDLE handle)
{
    T_AUDIO_TRACK *track;
    int32_t ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (track->stream_type != AUDIO_STREAM_TYPE_PLAYBACK &&
        track->stream_type != AUDIO_STREAM_TYPE_VOICE)
    {
        ret = 2;
        goto fail_check_type;
    }

    if (track->signal_out == false)
    {
        ret = 3;
        goto fail_check_signal_out;
    }

    if (audio_path_signal_monitoring_set(track->path_handle,
                                         AUDIO_PATH_DIR_TX,
                                         false,
                                         0,
                                         0,
                                         NULL) == false)
    {
        ret = 4;
        goto fail_set_signal;
    }

    track->signal_out          = false;
    track->signal_out_interval = 0;
    track->freq_out_num        = 0;

    if (track->freq_out_table != NULL)
    {
        free(track->freq_out_table);
        track->freq_out_table = NULL;
    }

    return true;

fail_set_signal:
fail_check_signal_out:
fail_check_type:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_signal_out_monitoring_stop: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_signal_in_monitoring_start(T_AUDIO_TRACK_HANDLE handle,
                                            uint16_t             refresh_interval,
                                            uint8_t              freq_num,
                                            uint32_t             freq_table[])
{
    T_AUDIO_TRACK *track;
    int32_t ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (track->stream_type != AUDIO_STREAM_TYPE_RECORD &&
        track->stream_type != AUDIO_STREAM_TYPE_VOICE)
    {
        ret = 2;
        goto fail_check_type;
    }

    if (track->signal_in == true)
    {
        ret = 3;
        goto fail_check_signal_in;
    }

    if (track->state == AUDIO_SESSION_STATE_STARTED)
    {
        if (audio_path_signal_monitoring_set(track->path_handle,
                                             AUDIO_PATH_DIR_RX,
                                             true,
                                             refresh_interval,
                                             freq_num,
                                             freq_table) == false)
        {
            ret = 4;
            goto fail_set_signal;
        }
    }

    track->signal_in          = true;
    track->signal_in_interval = refresh_interval;
    track->freq_in_num        = freq_num;
    track->freq_in_table      = malloc(track->freq_in_num * 4);
    if (track->freq_in_table == NULL)
    {
        ret = 5;
        goto fail_alloc_freq_table;
    }

    memcpy(track->freq_in_table, freq_table, 4 * freq_num);

    return true;

fail_alloc_freq_table:
fail_set_signal:
fail_check_signal_in:
fail_check_type:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_signal_in_monitoring_start: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_signal_in_monitoring_stop(T_AUDIO_TRACK_HANDLE handle)
{
    T_AUDIO_TRACK *track;
    int32_t ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (track->stream_type != AUDIO_STREAM_TYPE_RECORD &&
        track->stream_type != AUDIO_STREAM_TYPE_VOICE)
    {
        ret = 2;
        goto fail_check_type;
    }

    if (track->signal_in == false)
    {
        ret = 3;
        goto fail_check_signal_in;
    }

    if (audio_path_signal_monitoring_set(track->path_handle,
                                         AUDIO_PATH_DIR_RX,
                                         false,
                                         0,
                                         0,
                                         NULL) == false)
    {
        ret = 4;
        goto fail_set_signal;
    }

    track->signal_in          = false;
    track->signal_in_interval = 0;
    track->freq_in_num        = 0;

    if (track->freq_in_table != NULL)
    {
        free(track->freq_in_table);
        track->freq_in_table = NULL;
    }

    return true;

fail_set_signal:
fail_check_signal_in:
fail_check_type:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_signal_in_monitoring_stop: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_volume_in_max_get(T_AUDIO_TRACK_HANDLE  handle,
                                   uint8_t              *volume)
{
    T_AUDIO_TRACK *track;
    int32_t ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (track->stream_type != AUDIO_STREAM_TYPE_VOICE &&
        track->stream_type != AUDIO_STREAM_TYPE_RECORD)
    {
        ret = 2;
        goto fail_check_type;
    }

    *volume = track->volume_in_max;
    return true;

fail_check_type:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_volume_in_max_get: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_volume_in_max_set(T_AUDIO_STREAM_TYPE type,
                                   uint8_t             volume)
{
    T_OS_QUEUE    *queue;
    T_AUDIO_TRACK *track;
    uint8_t        volume_max = 0;
    uint8_t        volume_min = 0;
    uint8_t        volume_default = 0;

    switch (type)
    {
    case AUDIO_STREAM_TYPE_PLAYBACK:
        return false;

    case AUDIO_STREAM_TYPE_VOICE:
        volume_max      = audio_track_db.voice_volume_in_max;
        volume_min      = audio_track_db.voice_volume_in_min;
        volume_default  = audio_track_db.voice_volume_in_default;

        audio_track_db.voice_volume_in_min = audio_track_volume_scale(volume_min,
                                                                      volume_max,
                                                                      volume);
        audio_track_db.voice_volume_in_default = audio_track_volume_scale(volume_default,
                                                                          volume_max,
                                                                          volume);
        audio_track_db.voice_volume_in_max = volume;

        queue = &audio_track_db.voice_tracks;
        break;

    case AUDIO_STREAM_TYPE_RECORD:
        volume_max      = audio_track_db.record_volume_max;
        volume_min      = audio_track_db.record_volume_min;
        volume_default  = audio_track_db.record_volume_default;

        audio_track_db.record_volume_min = audio_track_volume_scale(volume_min,
                                                                    volume_max,
                                                                    volume);
        audio_track_db.record_volume_default = audio_track_volume_scale(volume_default,
                                                                        volume_max,
                                                                        volume);
        audio_track_db.record_volume_max = volume;

        queue = &audio_track_db.record_tracks;
        break;
    }

    if (volume != volume_max)
    {
        track = os_queue_peek(queue, 0);
        while (track != NULL)
        {
            track->volume_in_min   = audio_track_volume_scale(track->volume_in_min,
                                                              track->volume_in_max,
                                                              volume);
            track->volume_in       = audio_track_volume_scale(track->volume_in,
                                                              track->volume_in_max,
                                                              volume);
            track->volume_in_max   = volume;
            track                  = track->p_next;
        }

        track = os_queue_peek(&audio_track_db.idle_tracks, 0);
        while (track != NULL)
        {
            if (track->stream_type == type)
            {
                track->volume_in_min   = audio_track_volume_scale(track->volume_in_min,
                                                                  track->volume_in_max,
                                                                  volume);
                track->volume_in       = audio_track_volume_scale(track->volume_in,
                                                                  track->volume_in_max,
                                                                  volume);
                track->volume_in_max   = volume;
            }

            track = track->p_next;
        }
    }

    return true;
}

bool audio_track_volume_in_min_get(T_AUDIO_TRACK_HANDLE  handle,
                                   uint8_t              *volume)
{
    T_AUDIO_TRACK *track;
    int32_t ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (track->stream_type != AUDIO_STREAM_TYPE_VOICE &&
        track->stream_type != AUDIO_STREAM_TYPE_RECORD)
    {
        ret = 2;
        goto fail_check_type;
    }

    *volume = track->volume_out_min;
    return true;

fail_check_type:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_volume_in_min_get: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_volume_in_min_set(T_AUDIO_STREAM_TYPE type,
                                   uint8_t             volume)
{
    T_OS_QUEUE    *queue;
    T_AUDIO_TRACK *track;
    uint8_t        volume_min = 0;

    switch (type)
    {
    case AUDIO_STREAM_TYPE_PLAYBACK:
        return false;

    case AUDIO_STREAM_TYPE_VOICE:
        volume_min = audio_track_db.voice_volume_in_min;
        audio_track_db.voice_volume_in_min = volume;
        queue = &audio_track_db.voice_tracks;
        break;

    case AUDIO_STREAM_TYPE_RECORD:
        volume_min = audio_track_db.record_volume_min;
        audio_track_db.record_volume_min = volume;
        queue = &audio_track_db.record_tracks;
        break;
    }

    if (volume != volume_min)
    {
        track = os_queue_peek(queue, 0);
        while (track != NULL)
        {
            track->volume_in_min = volume;
            track                = track->p_next;
        }

        track = os_queue_peek(&audio_track_db.idle_tracks, 0);
        while (track != NULL)
        {
            if (track->stream_type == type)
            {
                track->volume_in_min = volume;
            }

            track = track->p_next;
        }
    }

    return true;
}

bool audio_track_volume_in_get(T_AUDIO_TRACK_HANDLE  handle,
                               uint8_t              *volume)
{
    T_AUDIO_TRACK *track;
    int32_t ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (track->stream_type != AUDIO_STREAM_TYPE_VOICE &&
        track->stream_type != AUDIO_STREAM_TYPE_RECORD)
    {
        ret = 2;
        goto fail_check_type;
    }

    *volume = track->volume_in;
    return true;

fail_check_type:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_volume_in_get: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_volume_in_set(T_AUDIO_TRACK_HANDLE handle,
                               uint8_t              volume)
{
    T_AUDIO_TRACK *track;
    T_AUDIO_MSG_PAYLOAD_TRACK_VOLUME_IN_CHANGE payload;
    int32_t ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    AUDIO_PRINT_INFO2("audio_track_volume_in_set: volume %u, volume_in_muted %u",
                      volume, track->volume_in_muted);

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (volume < track->volume_in_min ||
        volume > track->volume_in_max)
    {
        ret = 2;
        goto fail_check_volume;
    }

    if (track->stream_type == AUDIO_STREAM_TYPE_VOICE)
    {
        if ((audio_track_db.voice_volume_in_muted == false) &&
            (track->volume_in_muted == false))
        {
            audio_path_adc_level_set(track->path_handle, volume, 0.0f);
        }
    }
    else if (track->stream_type == AUDIO_STREAM_TYPE_RECORD)
    {
        if ((audio_track_db.record_volume_muted == false) &&
            (track->volume_in_muted == false))
        {
            audio_path_adc_level_set(track->path_handle, volume, 0.0f);
        }
    }
    else
    {
        ret = 3;
        goto fail_check_type;
    }

    payload.handle          = track;
    payload.prev_volume     = track->volume_in;
    payload.curr_volume     = volume;

    track->volume_in        = volume;

    audio_mgr_dispatch(AUDIO_MSG_TRACK_VOLUME_IN_CHANGE, &payload);
    return true;

fail_check_type:
fail_check_volume:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_volume_in_set: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_volume_in_mute(T_AUDIO_TRACK_HANDLE handle)
{
    T_AUDIO_TRACK *track;
    T_AUDIO_MSG_PAYLOAD_TRACK_VOLUME_IN_MUTE payload;
    int32_t ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (track->stream_type != AUDIO_STREAM_TYPE_VOICE &&
        track->stream_type != AUDIO_STREAM_TYPE_RECORD)
    {
        ret = 2;
        goto fail_check_type;
    }

    if (track->volume_in_muted == true)
    {
        ret = 3;
        goto fail_check_mute;
    }

    audio_path_adc_mute(track->path_handle);

    track->volume_in_muted = true;

    payload.handle  = track;
    payload.volume  = track->volume_in;

    audio_mgr_dispatch(AUDIO_MSG_TRACK_VOLUME_IN_MUTE, &payload);
    return true;

fail_check_mute:
fail_check_type:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_volume_in_mute: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_volume_in_unmute(T_AUDIO_TRACK_HANDLE handle)
{
    T_AUDIO_TRACK *track;
    T_AUDIO_MSG_PAYLOAD_TRACK_VOLUME_IN_UNMUTE payload;
    int32_t ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (track->stream_type != AUDIO_STREAM_TYPE_VOICE &&
        track->stream_type != AUDIO_STREAM_TYPE_RECORD)
    {
        ret = 2;
        goto fail_check_type;
    }

    if (track->volume_in_muted == false)
    {
        ret = 3;
        goto fail_check_muted;
    }

    if ((track->stream_type == AUDIO_STREAM_TYPE_VOICE) &&
        (audio_track_db.voice_volume_in_muted == false))
    {
        audio_path_adc_level_set(track->path_handle, track->volume_in, 0.0f);
    }
    else if ((track->stream_type == AUDIO_STREAM_TYPE_RECORD) &&
             (audio_track_db.record_volume_muted == false))
    {
        audio_path_adc_level_set(track->path_handle, track->volume_in, 0.0f);
    }

    track->volume_in_muted = false;

    payload.handle  = track;
    payload.volume  = track->volume_in;

    audio_mgr_dispatch(AUDIO_MSG_TRACK_VOLUME_IN_UNMUTE, &payload);
    return true;

fail_check_muted:
fail_check_type:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_volume_in_unmute: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_volume_in_is_muted(T_AUDIO_TRACK_HANDLE  handle,
                                    bool                 *is_muted)
{
    T_AUDIO_TRACK *track;
    int32_t ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    *is_muted = track->volume_in_muted;
    return true;

fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_volume_in_is_muted: handle %p, failed %d",
                       handle, -ret);
    return false;
}

static void audio_track_effect_cback(T_AUDIO_EFFECT_INSTANCE  instance,
                                     T_AUDIO_EFFECT_EVENT     event,
                                     void                    *context)
{
    T_AUDIO_PATH_HANDLE *path;
    T_AUDIO_TRACK       *track;

    path  = audio_effect_owner_get(instance);
    track = audio_track_find_by_path(path);

    AUDIO_PRINT_INFO3("audio_track_effect_cback: instance %p, track %p, event 0x%02x",
                      instance, track, event);

    if (audio_track_handle_check(track) == true)
    {
        switch (event)
        {
        case AUDIO_EFFECT_EVENT_CREATED:
            break;

        case AUDIO_EFFECT_EVENT_ENABLED:
            if (track->state == AUDIO_SESSION_STATE_STARTED)
            {
                audio_effect_run(instance);
            }
            else if (track->state == AUDIO_SESSION_STATE_STARTING ||
                     track->state == AUDIO_SESSION_STATE_PAUSED ||
                     track->state == AUDIO_SESSION_STATE_RESTARTING)
            {
                if (track->effect_apply == true)
                {
                    audio_effect_run(instance);
                }
            }
            break;

        case AUDIO_EFFECT_EVENT_DISABLED:
            if (track->state == AUDIO_SESSION_STATE_STARTED)
            {
                audio_effect_stop(instance);
            }
            else if (track->state == AUDIO_SESSION_STATE_STARTING ||
                     track->state == AUDIO_SESSION_STATE_PAUSED ||
                     track->state == AUDIO_SESSION_STATE_RESTARTING)
            {
                if (track->effect_apply == true)
                {
                    audio_effect_stop(instance);
                }
            }
            break;

        case AUDIO_EFFECT_EVENT_UPDATED:
            if (track->state == AUDIO_SESSION_STATE_STARTED)
            {
                audio_effect_flush(instance, context);
            }
            else if (track->state == AUDIO_SESSION_STATE_STARTING ||
                     track->state == AUDIO_SESSION_STATE_PAUSED ||
                     track->state == AUDIO_SESSION_STATE_RESTARTING)
            {
                if (track->effect_apply == true)
                {
                    audio_effect_flush(instance, context);
                }
            }
            break;

        case AUDIO_EFFECT_EVENT_RELEASED:
            if (track->state == AUDIO_SESSION_STATE_STARTED)
            {
                audio_effect_stop(instance);
            }
            else if (track->state == AUDIO_SESSION_STATE_STARTING ||
                     track->state == AUDIO_SESSION_STATE_PAUSED ||
                     track->state == AUDIO_SESSION_STATE_RESTARTING)
            {
                if (track->effect_apply == true)
                {
                    audio_effect_stop(instance);
                }
            }

            audio_track_effect_dequeue(track, instance);
            audio_effect_owner_clear(instance);
            break;
        }
    }
}

bool audio_track_effect_control(T_AUDIO_TRACK_HANDLE handle,
                                uint8_t              action)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    track->effect_bypass = action;

    if (track->state == AUDIO_SESSION_STATE_STARTED)
    {
        audio_track_effect_control_apply(track, action);
    }

    return true;

fail_check_handle:
    AUDIO_PRINT_ERROR3("audio_track_effect_control: handle %p, action %d, failed %d",
                       handle, action, -ret);
    return false;
}

bool audio_track_effect_attach(T_AUDIO_TRACK_HANDLE    handle,
                               T_AUDIO_EFFECT_INSTANCE instance)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (audio_track_effect_enqueue(track, instance) == false)
    {
        ret = 2;
        goto fail_enqueue_effect;
    }

    if (audio_effect_owner_set(instance,
                               track->path_handle,
                               audio_track_effect_cback) == false)
    {
        ret = 3;
        goto fail_set_owner;
    }

    if (track->state == AUDIO_SESSION_STATE_STARTED)
    {
        audio_effect_run(instance);
    }

    return true;

fail_set_owner:
    audio_track_effect_dequeue(track, instance);
fail_enqueue_effect:
fail_check_handle:
    AUDIO_PRINT_ERROR3("audio_track_effect_attach: handle %p, instance %p, failed %d",
                       handle, instance, -ret);
    return false;
}

bool audio_track_effect_detach(T_AUDIO_TRACK_HANDLE    handle,
                               T_AUDIO_EFFECT_INSTANCE instance)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (audio_track_effect_check(track, instance) == false)
    {
        ret = 2;
        goto fail_check_instance;
    }

    if (track->state == AUDIO_SESSION_STATE_STARTED)
    {
        audio_effect_stop(instance);
    }

    if (audio_track_effect_dequeue(track, instance) == false)
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
    audio_track_effect_enqueue(track, instance);
fail_dequeue_effect:
fail_check_instance:
fail_check_handle:
    AUDIO_PRINT_ERROR3("audio_track_effect_detach: handle %p, instance %p, failed %d",
                       handle, instance, -ret);
    return false;
}

bool audio_track_route_path_take(T_AUDIO_TRACK_HANDLE  handle,
                                 uint32_t              device,
                                 T_AUDIO_ROUTE_PATH   *path)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    *path = audio_route_path_get(track->path_handle,
                                 audio_track_path_category_convert(track->stream_type),
                                 device);
    if (path->entry == NULL)
    {
        ret = 2;
        goto fail_get_route;
    }

    return true;

fail_get_route:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_route_path_take: handle %p, failed %d",
                       handle, -ret);
    return false;
}

void audio_track_route_path_give(T_AUDIO_ROUTE_PATH *path)
{
    audio_route_path_give(path);
}

bool audio_track_route_bind(T_AUDIO_TRACK_HANDLE handle,
                            T_AUDIO_ROUTE_PATH   path)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (audio_route_path_bind(track->path_handle,
                              path) == false)
    {
        ret = 2;
        goto fail_bind_path;
    }

    return true;

fail_bind_path:
fail_check_handle:
    AUDIO_PRINT_ERROR3("audio_track_route_bind: handle %p, entry_num %d, failed %d",
                       handle, path.entry_num, -ret);
    return false;
}

bool audio_track_route_unbind(T_AUDIO_TRACK_HANDLE handle)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (audio_route_path_unbind(track->path_handle) == false)
    {
        ret = 2;
        goto fail_clear_owner;
    }

    return true;

fail_clear_owner:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_track_route_unbind: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_track_route_add(T_AUDIO_TRACK_HANDLE  handle,
                           T_AUDIO_ROUTE_ENTRY  *entry)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (audio_route_path_add(track->path_handle,
                             entry) == false)
    {
        ret = 2;
        goto fail_add_entry;
    }

    return true;

fail_add_entry:
fail_check_handle:
    AUDIO_PRINT_ERROR3("audio_track_route_add: handle %p, entry %p, failed %d",
                       handle, entry, -ret);
    return false;
}

bool audio_track_route_remove(T_AUDIO_TRACK_HANDLE  handle,
                              T_AUDIO_ROUTE_IO_TYPE io_type)
{
    T_AUDIO_TRACK *track;
    int32_t        ret = 0;

    track = (T_AUDIO_TRACK *)handle;

    if (audio_track_handle_check(handle) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (audio_route_path_remove(track->path_handle,
                                io_type) == false)
    {
        ret = 2;
        goto fail_remove_entry;
    }

    return true;

fail_remove_entry:
fail_check_handle:
    AUDIO_PRINT_ERROR3("audio_track_route_remove: handle %p, io_type 0x%02x, failed %d",
                       handle, io_type, -ret);
    return false;
}
