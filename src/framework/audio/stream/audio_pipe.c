/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if (CONFIG_REALTEK_AM_AUDIO_PIPE_SUPPORT == 1)
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "os_queue.h"
#include "trace.h"
#include "audio_type.h"
#include "audio_path.h"
#include "audio_pipe.h"

/* TODO Remove Start */
#define MAX_MIXED_PIPE_NUM              (2)
/* TODO Remove End */

#define AUDIO_CODEC_DEBUG 0

typedef enum t_audio_pipe_state
{
    AUDIO_PIPE_STATE_RELEASED  = 0x00,
    AUDIO_PIPE_STATE_CREATING  = 0x01,
    AUDIO_PIPE_STATE_CREATED   = 0x02,
    AUDIO_PIPE_STATE_STARTING  = 0x03,
    AUDIO_PIPE_STATE_STARTED   = 0x04,
    AUDIO_PIPE_STATE_STOPPING  = 0x05,
    AUDIO_PIPE_STATE_STOPPED   = 0x06,
    AUDIO_PIPE_STATE_RELEASING = 0x07,
} T_AUDIO_PIPE_STATE;

typedef enum t_audio_pipe_action
{
    AUDIO_PIPE_ACTION_NONE    = 0x00,
    AUDIO_PIPE_ACTION_CREATE  = 0x01,
    AUDIO_PIPE_ACTION_START   = 0x02,
    AUDIO_PIPE_ACTION_STOP    = 0x03,
    AUDIO_PIPE_ACTION_RELEASE = 0x04,
} T_AUDIO_PIPE_ACTION;

typedef struct t_audio_pipe
{
    struct t_audio_pipe      *next;
    T_AUDIO_PATH_HANDLE       path_handle;
    T_AUDIO_PIPE_HANDLE       mixed_pipes[MAX_MIXED_PIPE_NUM];
    T_AUDIO_PIPE_CBACK        cback;
    T_AUDIO_FORMAT_INFO       src_info;
    T_AUDIO_FORMAT_INFO       snk_info;
    int16_t                   gain_left;
    int16_t                   gain_right;
    T_AUDIO_PIPE_STATE        state;
    T_AUDIO_PIPE_ACTION       action;
    uint16_t                  frame_count;
    uint8_t                   mixed_pipe_num;
    uint8_t                   credits;
    T_AUDIO_STREAM_MODE       mode;
    uint8_t                   asrc_toggle;
    uint8_t                   channel_out_num;
    uint32_t                 *channel_out_array;
} T_AUDIO_PIPE;

typedef struct t_audio_pipe_db
{
    T_OS_QUEUE               pipes;
} T_AUDIO_PIPE_DB;

typedef bool (*T_AUDIO_PIPE_STATE_HANDLER)(T_AUDIO_PIPE       *audio_pipe,
                                           T_AUDIO_PIPE_STATE  state);

static T_AUDIO_PIPE_DB audio_pipe_db;

static bool audio_pipe_state_set(T_AUDIO_PIPE       *audio_pipe,
                                 T_AUDIO_PIPE_STATE  state);

bool audio_pipe_init(void)
{
    os_queue_init(&audio_pipe_db.pipes);

    return true;
}

void audio_pipe_deinit(void)
{
    T_AUDIO_PIPE *audio_pipe;

    while ((audio_pipe = os_queue_out(&audio_pipe_db.pipes)) != NULL)
    {
        if (audio_pipe->channel_out_array != NULL)
        {
            free(audio_pipe->channel_out_array);
        }

        free(audio_pipe);
    }
}

static T_AUDIO_PIPE *audio_pipe_find(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_PIPE *audio_pipe;

    audio_pipe = os_queue_peek(&audio_pipe_db.pipes, 0);
    while (audio_pipe != NULL)
    {
        if (audio_pipe->path_handle != NULL &&
            audio_pipe->path_handle == handle)
        {
            return audio_pipe;
        }

        audio_pipe = audio_pipe->next;
    }

    return audio_pipe;
}

static bool audio_pipe_path_cback(T_AUDIO_PATH_HANDLE handle,
                                  T_AUDIO_PATH_EVENT  event,
                                  uint32_t            param)
{
    T_AUDIO_PIPE *audio_pipe;

    audio_pipe = audio_pipe_find(handle);

#if (AUDIO_CODEC_DEBUG == 0)
    if (event != AUDIO_PATH_EVT_DATA_REQ && event != AUDIO_PATH_EVT_DATA_IND)
#endif
    {
        AUDIO_PRINT_INFO4("audio_pipe_path_cback: handle %p, event 0x%02x, param 0x%08x, pipe %p",
                          handle, event, param, audio_pipe);
    }


    if (audio_pipe != NULL)
    {
        switch (event)
        {
        case AUDIO_PATH_EVT_RELEASE:
            {
                audio_pipe_state_set(audio_pipe, AUDIO_PIPE_STATE_RELEASED);
            }
            break;

        case AUDIO_PATH_EVT_CREATE:
            {
                audio_pipe_state_set(audio_pipe, AUDIO_PIPE_STATE_CREATED);
            }
            break;

        case AUDIO_PATH_EVT_IDLE:
            {
                audio_pipe_state_set(audio_pipe, AUDIO_PIPE_STATE_STOPPED);
            }
            break;

        case AUDIO_PATH_EVT_EFFECT_REQ:
            {
                audio_path_asrc_set(audio_pipe->path_handle,
                                    audio_pipe->asrc_toggle,
                                    0,
                                    0xFFFFFFFF);

                if (audio_pipe->channel_out_num != 0)
                {
                    audio_path_channel_out_reorder(audio_pipe->path_handle,
                                                   audio_pipe->channel_out_num,
                                                   audio_pipe->channel_out_array);
                }
            }
            break;

        case AUDIO_PATH_EVT_RUNNING:
            {
                audio_pipe_state_set(audio_pipe, AUDIO_PIPE_STATE_STARTED);
            }
            break;

        case AUDIO_PATH_EVT_DATA_REQ:
            if (audio_pipe->state == AUDIO_PIPE_STATE_STARTED)
            {
                audio_pipe->credits = 1;
                audio_pipe->cback(audio_pipe, AUDIO_PIPE_EVENT_DATA_FILLED, 0);
            }
            break;

        case AUDIO_PATH_EVT_DATA_IND:
            if (audio_pipe->state == AUDIO_PIPE_STATE_STARTED)
            {
                audio_pipe->cback(audio_pipe, AUDIO_PIPE_EVENT_DATA_IND, 0);
            }
            break;

        case AUDIO_PATH_EVT_PIPE_MIX:
            {
                audio_pipe->cback(audio_pipe, AUDIO_PIPE_EVENT_MIXED, 0);
            }
            break;

        case AUDIO_PATH_EVT_PIPE_DEMIX:
            {
                audio_pipe->cback(audio_pipe, AUDIO_PIPE_EVENT_DEMIXED, 0);
            }
            break;

        default:
            break;
        }

        return true;
    }

    return false;
}

T_AUDIO_PIPE_HANDLE audio_pipe_create(T_AUDIO_STREAM_MODE mode,
                                      T_AUDIO_FORMAT_INFO src_info,
                                      T_AUDIO_FORMAT_INFO snk_info,
                                      int16_t             gain,
                                      T_AUDIO_PIPE_CBACK  cback)
{
    T_AUDIO_PIPE *audio_pipe;
    int32_t       ret = 0;

    if (cback == NULL)
    {
        ret = 1;
        goto fail_check_cback;
    }

    audio_pipe = calloc(1, sizeof(T_AUDIO_PIPE));
    if (audio_pipe == NULL)
    {
        ret = 2;
        goto fail_alloc_pipe;
    }

    AUDIO_PRINT_INFO3("audio_pipe_create: handle %p, src type 0x%x, snk type 0x%x",
                      audio_pipe, src_info.type, snk_info.type);

    audio_pipe->cback              = cback;
    audio_pipe->state              = AUDIO_PIPE_STATE_RELEASED;
    audio_pipe->action             = AUDIO_PIPE_ACTION_NONE;
    audio_pipe->src_info           = src_info;
    audio_pipe->snk_info           = snk_info;
    audio_pipe->gain_left          = gain;
    audio_pipe->gain_right         = gain;
    audio_pipe->credits            = 1;
    audio_pipe->mode               = mode;
    audio_pipe->channel_out_num    = 0;
    audio_pipe->channel_out_array  = NULL;
    audio_pipe->asrc_toggle        = 0;

    if (audio_pipe_state_set(audio_pipe, AUDIO_PIPE_STATE_CREATING) == false)
    {
        ret = 3;
        goto fail_set_state;
    }

    return (T_AUDIO_PIPE_HANDLE)audio_pipe;

fail_set_state:
    free(audio_pipe);
fail_alloc_pipe:
fail_check_cback:
    AUDIO_PRINT_ERROR1("audio_pipe_create: failed %d", -ret);
    return NULL;
}

bool audio_pipe_release(T_AUDIO_PIPE_HANDLE handle)
{
    T_AUDIO_PIPE *audio_pipe;
    T_AUDIO_PIPE *item;
    uint8_t       i;
    int32_t       ret = 0;

    audio_pipe = (T_AUDIO_PIPE *)handle;
    if (os_queue_search(&audio_pipe_db.pipes, audio_pipe) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    AUDIO_PRINT_TRACE3("audio_pipe_release: handle %p, state %d, action %d",
                       audio_pipe, audio_pipe->state, audio_pipe->action);

    if (audio_pipe->mixed_pipe_num > 0)
    {
        ret = 2;
        goto fail_check_mixed_pipe;
    }

    item = os_queue_peek(&audio_pipe_db.pipes, 0);
    while (item)
    {
        for (i = 0; i < MAX_MIXED_PIPE_NUM; i++)
        {
            if (item->mixed_pipes[i] == handle)
            {
                item->mixed_pipe_num--;
                item->mixed_pipes[i] = 0;
                break;
            }
        }

        item = item->next;
    }

    if (audio_pipe_state_set(audio_pipe, AUDIO_PIPE_STATE_RELEASING) == false)
    {
        ret = 3;
        goto fail_set_state;
    }

    return true;

fail_set_state:
fail_check_mixed_pipe:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_pipe_release: handle %p, failed %d", audio_pipe, -ret);
    return false;
}

bool audio_pipe_start(T_AUDIO_PIPE_HANDLE handle)
{
    T_AUDIO_PIPE *audio_pipe;
    int32_t       ret = 0;

    audio_pipe = (T_AUDIO_PIPE *)handle;
    if (os_queue_search(&audio_pipe_db.pipes, audio_pipe) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (audio_pipe_state_set(audio_pipe, AUDIO_PIPE_STATE_STARTING) == false)
    {
        ret = 2;
        goto fail_set_state;
    }

    return true;

fail_set_state:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_pipe_start: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_pipe_stop(T_AUDIO_PIPE_HANDLE handle)
{
    T_AUDIO_PIPE *audio_pipe;
    int32_t       ret = 0;

    audio_pipe = (T_AUDIO_PIPE *)handle;
    if (os_queue_search(&audio_pipe_db.pipes, audio_pipe) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (audio_pipe_state_set(audio_pipe, AUDIO_PIPE_STATE_STOPPING) == false)
    {
        ret = 2;
        goto fail_set_state;
    }

    return true;

fail_set_state:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_pipe_stop: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_pipe_format_info_get(T_AUDIO_PIPE_HANDLE  handle,
                                T_AUDIO_FORMAT_INFO *src_info,
                                T_AUDIO_FORMAT_INFO *snk_info)
{
    T_AUDIO_PIPE *audio_pipe = (T_AUDIO_PIPE *)handle;

    if (os_queue_search(&audio_pipe_db.pipes, audio_pipe) == false)
    {
        return false;
    }

    *src_info = audio_pipe->src_info;
    *snk_info = audio_pipe->snk_info;

    return true;
}

bool audio_pipe_gain_get(T_AUDIO_PIPE_HANDLE  handle,
                         int16_t             *gain_left,
                         int16_t             *gain_right)
{
    T_AUDIO_PIPE *audio_pipe = (T_AUDIO_PIPE *)handle;

    if (os_queue_search(&audio_pipe_db.pipes, audio_pipe) == false)
    {
        return false;
    }

    *gain_left = audio_pipe->gain_left;
    *gain_right = audio_pipe->gain_right;

    return true;
}

bool audio_pipe_gain_set(T_AUDIO_PIPE_HANDLE handle,
                         int16_t             gain_left,
                         int16_t             gain_right)
{
    T_AUDIO_PIPE *audio_pipe = (T_AUDIO_PIPE *)handle;
    int32_t       ret = 0;

    if (os_queue_search(&audio_pipe_db.pipes, audio_pipe) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (audio_pipe->state == AUDIO_PIPE_STATE_STARTED)
    {
        if (audio_path_pipe_gain_set(audio_pipe->path_handle,
                                     gain_left,
                                     gain_right) == false)
        {
            ret = 2;
            goto fail_set_gain;
        }
    }

    audio_pipe->gain_left = gain_left;
    audio_pipe->gain_right = gain_right;
    return true;

fail_set_gain:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_pipe_gain_set: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_pipe_channel_out_set(T_AUDIO_PIPE_HANDLE handle,
                                uint8_t             channel_num,
                                uint32_t            channel_array[])
{
    T_AUDIO_PIPE *audio_pipe = (T_AUDIO_PIPE *)handle;
    int32_t       ret = 0;

    if (os_queue_search(&audio_pipe_db.pipes, audio_pipe) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (channel_num == 0)
    {
        ret = 2;
        goto fail_check_num;
    }

    if (audio_pipe->state == AUDIO_PIPE_STATE_STARTED)
    {
        if (audio_path_channel_out_reorder(audio_pipe->path_handle,
                                           channel_num,
                                           channel_array) == false)
        {
            ret = 3;
            goto fail_reorder_channel;
        }
    }

    audio_pipe->channel_out_num   = channel_num;
    audio_pipe->channel_out_array = malloc(4 * audio_pipe->channel_out_num);
    if (audio_pipe->channel_out_array == NULL)
    {
        ret = 4;
        goto fail_alloc_array;
    }

    memcpy(audio_pipe->channel_out_array, channel_array, 4 * channel_num);
    return true;

fail_alloc_array:
fail_reorder_channel:
fail_check_num:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_pipe_channel_out_set: handle %p, failed %d",
                       handle, -ret);
    return false;
}

bool audio_pipe_fill(T_AUDIO_PIPE_HANDLE    handle,
                     uint32_t               timestamp,
                     uint16_t               seq_num,
                     T_AUDIO_STREAM_STATUS  status,
                     uint8_t                frame_num,
                     void                  *buf,
                     uint16_t               len)
{
    T_AUDIO_PIPE          *audio_pipe = (T_AUDIO_PIPE *)handle;
    T_AUDIO_PATH_DATA_HDR *header;
    int32_t                ret = 0;

    if (os_queue_search(&audio_pipe_db.pipes, audio_pipe) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (audio_pipe->state != AUDIO_PIPE_STATE_STARTED)
    {
        ret = 2;
        goto fail_invalid_state;
    }

    if ((audio_pipe->mode != AUDIO_STREAM_MODE_DIRECT) &&
        (audio_pipe->mode != AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY))
    {
        if (audio_pipe->credits == 0)
        {
            ret = 3;
            goto fail_no_credits;
        }
    }

    header = malloc(len + sizeof(T_AUDIO_PATH_DATA_HDR));
    if (header == NULL)
    {
        ret = 4;
        goto fail_alloc_mem;
    }

    header->timestamp = timestamp;
    header->seq_num = seq_num;
    header->frame_count = audio_pipe->frame_count + frame_num;
    header->frame_number = frame_num;
    header->status = status;
    header->payload_length = len;

    if (buf != NULL)
    {
        memcpy(header->payload, buf, len);
    }

    if (audio_path_data_send(audio_pipe->path_handle,
                             header,
                             len + sizeof(T_AUDIO_PATH_DATA_HDR)) == false)
    {
        ret = 5;
        goto fail_fill_data;
    }

#if (AUDIO_CODEC_DEBUG == 1)
    AUDIO_PRINT_INFO3("audio_pipe_fill: pipe_session %p, seq_num 0x%x, len 0x%x",
                      audio_pipe, seq_num, len);
#endif

    audio_pipe->frame_count += frame_num;
    audio_pipe->credits = 0;
    free(header);

    return true;

fail_fill_data:
    free(header);
fail_alloc_mem:
fail_no_credits:
fail_invalid_state:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_pipe_fill: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_pipe_drain(T_AUDIO_PIPE_HANDLE    handle,
                      uint32_t              *timestamp,
                      uint16_t              *seq_num,
                      T_AUDIO_STREAM_STATUS *status,
                      uint8_t               *frame_num,
                      void                  *buf,
                      uint16_t              *len)
{
    T_AUDIO_PIPE          *audio_pipe = (T_AUDIO_PIPE *)handle;
    T_AUDIO_PATH_DATA_HDR *peek_buf;
    uint16_t               peek_len;
    uint16_t               recv_len;
    int32_t                ret = 0;

    if (os_queue_search(&audio_pipe_db.pipes, audio_pipe) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }

    if (audio_pipe->state != AUDIO_PIPE_STATE_STARTED)
    {
        ret = 2;
        goto fail_invalid_state;
    }

    peek_len = audio_path_data_peek(audio_pipe->path_handle);
    peek_buf = malloc(peek_len);
    if (peek_buf == NULL)
    {
        ret = 3;
        goto fail_alloc_mem;
    }

    recv_len = audio_path_data_recv(audio_pipe->path_handle, peek_buf, peek_len);
    if (recv_len == 0)
    {
        ret = 4;
        goto fail_drain_data;
    }

    recv_len = recv_len - sizeof(T_AUDIO_PATH_DATA_HDR);
    *timestamp = peek_buf->timestamp;
    *seq_num = peek_buf->seq_num;
    *status = (T_AUDIO_STREAM_STATUS)peek_buf->status;
    *frame_num = peek_buf->frame_number;
    *len = recv_len;
    memcpy(buf, peek_buf->payload, recv_len);
    free(peek_buf);

#if (AUDIO_CODEC_DEBUG == 1)
    AUDIO_PRINT_INFO3("audio_pipe_drain: pipe_session %p, recv_len 0x%x, seq_num 0x%x",
                      audio_pipe, recv_len, peek_buf->seq_num);
#endif

    return true;

fail_drain_data:
    free(peek_buf);
fail_alloc_mem:
fail_invalid_state:
fail_check_handle:
    AUDIO_PRINT_ERROR2("audio_pipe_drain: handle %p, failed %d", handle, -ret);
    return false;
}

bool audio_pipe_pre_mix(T_AUDIO_PIPE_HANDLE prime_handle,
                        T_AUDIO_PIPE_HANDLE auxiliary_handle)
{
    uint8_t       i;
    T_AUDIO_PIPE *prime_pipe = (T_AUDIO_PIPE *)prime_handle;
    T_AUDIO_PIPE *auxiliary_pipe = (T_AUDIO_PIPE *)auxiliary_handle;

    if (os_queue_search(&audio_pipe_db.pipes, prime_pipe) == false)
    {
        return false;
    }

    if (os_queue_search(&audio_pipe_db.pipes, auxiliary_pipe) == false)
    {
        return false;
    }

    if (prime_pipe->mixed_pipe_num < MAX_MIXED_PIPE_NUM)
    {
        for (i = 0; i < MAX_MIXED_PIPE_NUM; i++)
        {
            if (prime_pipe->mixed_pipes[i] == 0)
            {
                prime_pipe->mixed_pipe_num++;
                prime_pipe->mixed_pipes[i] = auxiliary_handle;
                break;
            }
        }

        AUDIO_PRINT_INFO2("audio_pipe_pre_mix: prime %p, auxiliary %p", prime_pipe, auxiliary_pipe);

        return audio_path_pipe_pre_mix(prime_pipe->path_handle,
                                       auxiliary_pipe->path_handle);
    }

    return false;
}

bool audio_pipe_post_mix(T_AUDIO_PIPE_HANDLE prime_handle,
                         T_AUDIO_PIPE_HANDLE auxiliary_handle)
{
    uint8_t       i;
    T_AUDIO_PIPE *prime_pipe = (T_AUDIO_PIPE *)prime_handle;
    T_AUDIO_PIPE *auxiliary_pipe = (T_AUDIO_PIPE *)auxiliary_handle;

    if (os_queue_search(&audio_pipe_db.pipes, prime_pipe) == false)
    {
        return false;
    }

    if (os_queue_search(&audio_pipe_db.pipes, auxiliary_pipe) == false)
    {
        return false;
    }

    if (prime_pipe->mixed_pipe_num < MAX_MIXED_PIPE_NUM)
    {
        for (i = 0; i < MAX_MIXED_PIPE_NUM; i++)
        {
            if (prime_pipe->mixed_pipes[i] == 0)
            {
                prime_pipe->mixed_pipe_num++;
                prime_pipe->mixed_pipes[i] = auxiliary_handle;
                break;
            }
        }

        AUDIO_PRINT_INFO2("audio_pipe_post_mix: prime %p, auxiliary %p", prime_pipe, auxiliary_pipe);

        return audio_path_pipe_post_mix(prime_pipe->path_handle,
                                        auxiliary_pipe->path_handle);
    }

    return false;
}

bool audio_pipe_demix(T_AUDIO_PIPE_HANDLE prime_handle,
                      T_AUDIO_PIPE_HANDLE auxiliary_handle)
{
    uint8_t       i;
    T_AUDIO_PIPE *prime_pipe = (T_AUDIO_PIPE *)prime_handle;
    T_AUDIO_PIPE *auxiliary_pipe = (T_AUDIO_PIPE *)auxiliary_handle;

    if (os_queue_search(&audio_pipe_db.pipes, prime_pipe) == false)
    {
        return false;
    }

    if (os_queue_search(&audio_pipe_db.pipes, auxiliary_pipe) == false)
    {
        return false;
    }

    for (i = 0; i < MAX_MIXED_PIPE_NUM; i++)
    {
        if (prime_pipe->mixed_pipes[i] == auxiliary_handle)
        {
            prime_pipe->mixed_pipe_num--;
            prime_pipe->mixed_pipes[i] = 0;
            break;
        }
    }

    AUDIO_PRINT_INFO2("audio_pipe_demix: prime %p, auxiliary %p", prime_pipe, auxiliary_pipe);

    return audio_path_pipe_demix(prime_pipe->path_handle,
                                 auxiliary_pipe->path_handle);
}

bool audio_pipe_asrc_toggle(T_AUDIO_PIPE_HANDLE handle,
                            uint8_t             toggle)
{
    T_AUDIO_PIPE *pipe = (T_AUDIO_PIPE *)handle;

    if (os_queue_search(&audio_pipe_db.pipes, pipe) == false)
    {
        return false;
    }

    pipe->asrc_toggle = toggle;
    return true;
}

bool audio_pipe_asrc_set(T_AUDIO_PIPE_HANDLE handle,
                         int32_t             ratio)
{
    T_AUDIO_PIPE *pipe = (T_AUDIO_PIPE *)handle;
    int32_t       ret = 0;

    if (os_queue_search(&audio_pipe_db.pipes, pipe) == false)
    {
        ret = 1;
        goto fail_check_pipe;
    }

    if ((pipe->state != AUDIO_PIPE_STATE_STARTED) ||
        (pipe->asrc_toggle == 0))
    {
        ret = 2;
        goto fail_check_state;
    }

    if (audio_path_asrc_set(pipe->path_handle,
                            pipe->asrc_toggle,
                            ratio,
                            0xFFFFFFFF) == false)
    {
        ret = 3;
        goto fail_set_asrc;
    }

    return true;

fail_set_asrc:
fail_check_state:
fail_check_pipe:
    AUDIO_PRINT_ERROR1("audio_pipe_asrc_set: failed %d", -ret);
    return false;
}

static bool audio_pipe_released_handler(T_AUDIO_PIPE       *audio_pipe,
                                        T_AUDIO_PIPE_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_PIPE_STATE_RELEASED:
    case AUDIO_PIPE_STATE_CREATED:
    case AUDIO_PIPE_STATE_STARTING:
    case AUDIO_PIPE_STATE_STARTED:
    case AUDIO_PIPE_STATE_STOPPING:
    case AUDIO_PIPE_STATE_STOPPED:
    case AUDIO_PIPE_STATE_RELEASING:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_PIPE_STATE_CREATING:
        {
            T_AUDIO_PATH_PARAM path_param;

            path_param.pipe.mode = audio_pipe->mode;
            path_param.pipe.decoder_info = &audio_pipe->src_info;
            path_param.pipe.encoder_info = &audio_pipe->snk_info;

            audio_pipe->path_handle = audio_path_create(AUDIO_CATEGORY_PIPE,
                                                        path_param,
                                                        audio_pipe_path_cback);
            if (audio_pipe->path_handle != NULL)
            {
                /* Update pipe session state first, as audio path callback
                 * will invoke audio_pipe_start.
                 */
                audio_pipe->state = state;

                /* Enqueue pipe before audio path created, as audio path callback
                 * may manipulate the idle pipe first.
                 */
                os_queue_in(&audio_pipe_db.pipes, audio_pipe);
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

static bool audio_pipe_creating_handler(T_AUDIO_PIPE       *audio_pipe,
                                        T_AUDIO_PIPE_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_PIPE_STATE_RELEASED:
    case AUDIO_PIPE_STATE_CREATING:
    case AUDIO_PIPE_STATE_STARTED:
    case AUDIO_PIPE_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_PIPE_STATE_CREATED:
        {
            audio_pipe->state = state;
            audio_pipe->cback(audio_pipe, AUDIO_PIPE_EVENT_CREATED, 0);

            if (audio_pipe->action == AUDIO_PIPE_ACTION_NONE)
            {
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating audio pipe is impossible.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_START)
            {
                audio_pipe->action = AUDIO_PIPE_ACTION_NONE;
                ret = audio_pipe_state_set(audio_pipe, AUDIO_PIPE_STATE_STARTING);
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_RELEASE)
            {
                audio_pipe->action = AUDIO_PIPE_ACTION_NONE;
                ret = audio_pipe_state_set(audio_pipe, AUDIO_PIPE_STATE_RELEASING);
            }
        }
        break;

    case AUDIO_PIPE_STATE_STARTING:
        {
            if (audio_pipe->action == AUDIO_PIPE_ACTION_NONE)
            {
                /* Start audio pipe immediately after audio_pipe_create invoked,
                 * and the audio path created event has not been received, so this
                 * action should be pending.
                 */
                audio_pipe->action = AUDIO_PIPE_ACTION_START;
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating audio pipe is impossible.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_START)
            {
                /* Drop repeating audio_pipe_start invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_RELEASE)
            {
                /* When the creating audio pipe is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_PIPE_STATE_STOPPING:
        {
            if (audio_pipe->action == AUDIO_PIPE_ACTION_NONE)
            {
                /* Stopping the creating audio pipe is forbidden. */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating audio pipe is impossible.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_START)
            {
                /* Cancel all pending actions when the creating audio pipe is
                 * pending for start action, but receives the stop action later.
                 */
                audio_pipe->action = AUDIO_PIPE_ACTION_NONE;
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_RELEASE)
            {
                /* When the creating audio pipe is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_PIPE_STATE_RELEASING:
        {
            if (audio_pipe->action == AUDIO_PIPE_ACTION_NONE)
            {
                audio_pipe->action = AUDIO_PIPE_ACTION_RELEASE;
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating audio pipe is impossible.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_START)
            {
                /* Override the pending start action for the creating audio pipe. */
                audio_pipe->action = AUDIO_PIPE_ACTION_RELEASE;
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_RELEASE)
            {
                /* Drop repeating audio_pipe_release invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
        }
        break;
    }

    return ret;
}

static bool audio_pipe_created_handler(T_AUDIO_PIPE       *audio_pipe,
                                       T_AUDIO_PIPE_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_PIPE_STATE_RELEASED:
    case AUDIO_PIPE_STATE_CREATING:
    case AUDIO_PIPE_STATE_CREATED:
    case AUDIO_PIPE_STATE_STARTED:
    case AUDIO_PIPE_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_PIPE_STATE_STARTING:
        {
            audio_pipe->state = state;

            ret = audio_path_start(audio_pipe->path_handle);
            if (ret == false)
            {
                /* Restore the pipe state. */
                audio_pipe->state = AUDIO_PIPE_STATE_CREATED;
            }
        }
        break;

    case AUDIO_PIPE_STATE_STOPPING:
        {
            if (audio_pipe->action == AUDIO_PIPE_ACTION_START)
            {
                audio_pipe->action = AUDIO_PIPE_ACTION_NONE;
            }
            /* Stopping the created audio pipe is forbidden. */
            ret = false;
        }
        break;

    case AUDIO_PIPE_STATE_RELEASING:
        {
            audio_pipe->state = state;

            ret = audio_path_destroy(audio_pipe->path_handle);
            if (ret == false)
            {
                /* Restore the pipe state. */
                audio_pipe->state = AUDIO_PIPE_STATE_CREATED;
            }
        }
        break;
    }

    return ret;
}

static bool audio_pipe_starting_handler(T_AUDIO_PIPE       *audio_pipe,
                                        T_AUDIO_PIPE_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_PIPE_STATE_RELEASED:
    case AUDIO_PIPE_STATE_CREATING:
    case AUDIO_PIPE_STATE_CREATED:
    case AUDIO_PIPE_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_PIPE_STATE_STARTING:
        {
            if (audio_pipe->action == AUDIO_PIPE_ACTION_NONE)
            {
                /* Drop repeating audio_pipe_start invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting audio pipe is impossible.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_STOP)
            {
                /* Cancel all pending actions when the starting audio pipe is
                 * pending for stop action, but receives the start action later.
                 */
                audio_pipe->action = AUDIO_PIPE_ACTION_NONE;
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_RELEASE)
            {
                /* When the starting audio pipe is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_PIPE_STATE_STARTED:
        {
            audio_pipe->frame_count = 0;
            audio_pipe->state = state;
            audio_path_pipe_gain_set(audio_pipe->path_handle,
                                     audio_pipe->gain_left,
                                     audio_pipe->gain_right);

            audio_pipe->cback(audio_pipe, AUDIO_PIPE_EVENT_STARTED, 0);

            if (audio_pipe->action == AUDIO_PIPE_ACTION_NONE)
            {
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting audio pipe is impossible.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_STOP)
            {
                audio_pipe->action = AUDIO_PIPE_ACTION_NONE;
                ret = audio_pipe_state_set(audio_pipe, AUDIO_PIPE_STATE_STOPPING);
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_RELEASE)
            {
                audio_pipe->action = AUDIO_PIPE_ACTION_NONE;
                ret = audio_pipe_state_set(audio_pipe, AUDIO_PIPE_STATE_RELEASING);
            }
        }
        break;

    case AUDIO_PIPE_STATE_STOPPING:
        {
            if (audio_pipe->action == AUDIO_PIPE_ACTION_NONE)
            {
                audio_pipe->action = AUDIO_PIPE_ACTION_STOP;
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting audio pipe is impossible.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_STOP)
            {
                /* Drop repeating audio_pipe_stop invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_RELEASE)
            {
                /* When the starting audio pipe is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_PIPE_STATE_RELEASING:
        {
            if (audio_pipe->action == AUDIO_PIPE_ACTION_NONE)
            {
                audio_pipe->action = AUDIO_PIPE_ACTION_RELEASE;
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting audio pipe is impossible.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_STOP)
            {
                /* Override the pending stop action for the starting audio pipe. */
                audio_pipe->action = AUDIO_PIPE_ACTION_RELEASE;
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_RELEASE)
            {
                /* Drop repeating audio_pipe_release invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
        }
        break;
    }

    return ret;
}

static bool audio_pipe_started_handler(T_AUDIO_PIPE       *audio_pipe,
                                       T_AUDIO_PIPE_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_PIPE_STATE_RELEASED:
    case AUDIO_PIPE_STATE_CREATING:
    case AUDIO_PIPE_STATE_CREATED:
    case AUDIO_PIPE_STATE_STARTING:
    case AUDIO_PIPE_STATE_STARTED:
    case AUDIO_PIPE_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_PIPE_STATE_STOPPING:
        {
            audio_pipe->state = state;

            ret = audio_path_stop(audio_pipe->path_handle);
            if (ret == false)
            {
                /* Restore the pipe state. */
                audio_pipe->state = AUDIO_PIPE_STATE_STARTED;
            }
        }
        break;

    case AUDIO_PIPE_STATE_RELEASING:
        {
            /* Started audio pipe cannot be released directly, so it should be
             * stopped first before released.
             */
            audio_pipe->state  = AUDIO_PIPE_STATE_STOPPING;
            audio_pipe->action = AUDIO_PIPE_ACTION_RELEASE;

            ret = audio_path_stop(audio_pipe->path_handle);
            if (ret == false)
            {
                /* Restore the pipe state. */
                audio_pipe->state  = AUDIO_PIPE_STATE_STARTED;
                audio_pipe->action = AUDIO_PIPE_ACTION_NONE;
            }
        }
        break;
    }

    return ret;
}

static bool audio_pipe_stopping_handler(T_AUDIO_PIPE       *audio_pipe,
                                        T_AUDIO_PIPE_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_PIPE_STATE_RELEASED:
    case AUDIO_PIPE_STATE_CREATING:
    case AUDIO_PIPE_STATE_CREATED:
    case AUDIO_PIPE_STATE_STARTED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_PIPE_STATE_STARTING:
        {
            if (audio_pipe->action == AUDIO_PIPE_ACTION_NONE)
            {
                audio_pipe->action = AUDIO_PIPE_ACTION_START;
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping audio pipe is impossible.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_START)
            {
                /* Drop repeating audio_pipe_start invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_RELEASE)
            {
                /* When the stopping audio pipe is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_PIPE_STATE_STOPPING:
        {
            if (audio_pipe->action == AUDIO_PIPE_ACTION_NONE)
            {
                /* Drop repeating audio_pipe_stop invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping audio pipe is impossible.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_START)
            {
                /* Cancel all pending actions when the stopping audio pipe is
                 * pending for start action, but receives the stop action later.
                 */
                audio_pipe->action = AUDIO_PIPE_ACTION_NONE;
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_RELEASE)
            {
                /* When the stopping audio pipe is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case AUDIO_PIPE_STATE_STOPPED:
        {
            audio_pipe->state = state;
            audio_pipe->cback(audio_pipe, AUDIO_PIPE_EVENT_STOPPED, 0);

            if (audio_pipe->action == AUDIO_PIPE_ACTION_NONE)
            {
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping audio pipe is impossible.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_START)
            {
                audio_pipe->action = AUDIO_PIPE_ACTION_NONE;
                ret = audio_pipe_state_set(audio_pipe, AUDIO_PIPE_STATE_STARTING);
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_RELEASE)
            {
                audio_pipe->action = AUDIO_PIPE_ACTION_NONE;
                ret = audio_pipe_state_set(audio_pipe, AUDIO_PIPE_STATE_RELEASING);
            }
        }
        break;

    case AUDIO_PIPE_STATE_RELEASING:
        {
            if (audio_pipe->action == AUDIO_PIPE_ACTION_NONE)
            {
                audio_pipe->action = AUDIO_PIPE_ACTION_RELEASE;
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping audio pipe is impossible.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_START)
            {
                /* Override the pending start action for the stopping audio pipe. */
                audio_pipe->action = AUDIO_PIPE_ACTION_RELEASE;
                ret = true;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (audio_pipe->action == AUDIO_PIPE_ACTION_RELEASE)
            {
                /* Drop repeating audio_pipe_release invocation. But this case
                 * is permitted.
                 */
                ret = true;
            }
        }
        break;
    }

    return ret;
}

static bool audio_pipe_stopped_handler(T_AUDIO_PIPE       *audio_pipe,
                                       T_AUDIO_PIPE_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_PIPE_STATE_RELEASED:
    case AUDIO_PIPE_STATE_CREATING:
    case AUDIO_PIPE_STATE_CREATED:
    case AUDIO_PIPE_STATE_STARTED:
    case AUDIO_PIPE_STATE_STOPPING:
    case AUDIO_PIPE_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_PIPE_STATE_STARTING:
        {
            audio_pipe->state = state;

            ret = audio_path_start(audio_pipe->path_handle);
            if (ret == false)
            {
                /* Restore the pipe state. */
                audio_pipe->state = AUDIO_PIPE_STATE_STOPPED;
            }
        }
        break;

    case AUDIO_PIPE_STATE_RELEASING:
        {
            audio_pipe->state = state;

            ret = audio_path_destroy(audio_pipe->path_handle);
            if (ret == false)
            {
                /* Restore the pipe state. */
                audio_pipe->state = AUDIO_PIPE_STATE_STOPPED;
            }
        }
        break;
    }

    return ret;
}

static bool audio_pipe_releasing_handler(T_AUDIO_PIPE       *audio_pipe,
                                         T_AUDIO_PIPE_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case AUDIO_PIPE_STATE_RELEASED:
        {
            audio_pipe->state  = state;
            audio_pipe->action = AUDIO_PIPE_ACTION_NONE;
            os_queue_delete(&audio_pipe_db.pipes, audio_pipe);

            if (audio_pipe->channel_out_array != NULL)
            {
                free(audio_pipe->channel_out_array);
            }

            audio_pipe->cback(audio_pipe, AUDIO_PIPE_EVENT_RELEASED, 0);
            free(audio_pipe);
        }
        break;

    case AUDIO_PIPE_STATE_CREATING:
    case AUDIO_PIPE_STATE_CREATED:
    case AUDIO_PIPE_STATE_STARTING:
    case AUDIO_PIPE_STATE_STARTED:
    case AUDIO_PIPE_STATE_STOPPING:
    case AUDIO_PIPE_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case AUDIO_PIPE_STATE_RELEASING:
        {
            /* Drop repeating audio_pipe_release invocation. But this case
             * is permitted.
             */
            ret = true;
        }
        break;
    }

    return ret;
}

static const T_AUDIO_PIPE_STATE_HANDLER audio_pipe_state_handler[] =
{
    audio_pipe_released_handler,
    audio_pipe_creating_handler,
    audio_pipe_created_handler,
    audio_pipe_starting_handler,
    audio_pipe_started_handler,
    audio_pipe_stopping_handler,
    audio_pipe_stopped_handler,
    audio_pipe_releasing_handler,
};

static bool audio_pipe_state_set(T_AUDIO_PIPE       *audio_pipe,
                                 T_AUDIO_PIPE_STATE  state)
{
    AUDIO_PRINT_INFO4("audio_pipe_state_set: handle %p, curr_state 0x%02x, next_state 0x%02x, action 0x%02x",
                      audio_pipe, audio_pipe->state, state, audio_pipe->action);

    return audio_pipe_state_handler[audio_pipe->state](audio_pipe, state);
}
#else
#include <stdint.h>
#include <stddef.h>
#include "audio_pipe.h"
bool audio_pipe_init(void)
{
    return true;
}

void audio_pipe_deinit(void)
{

}

T_AUDIO_PIPE_HANDLE audio_pipe_create(T_AUDIO_STREAM_MODE mode,
                                      T_AUDIO_FORMAT_INFO src_info,
                                      T_AUDIO_FORMAT_INFO snk_info,
                                      int16_t             gain,
                                      T_AUDIO_PIPE_CBACK  cback)
{
    return NULL;
}

bool audio_pipe_release(T_AUDIO_PIPE_HANDLE handle)
{
    return false;
}

bool audio_pipe_start(T_AUDIO_PIPE_HANDLE handle)
{
    return false;
}

bool audio_pipe_stop(T_AUDIO_PIPE_HANDLE handle)
{
    return false;
}

bool audio_pipe_format_info_get(T_AUDIO_PIPE_HANDLE  handle,
                                T_AUDIO_FORMAT_INFO *src_info,
                                T_AUDIO_FORMAT_INFO *snk_info)
{
    return false;
}

bool audio_pipe_gain_get(T_AUDIO_PIPE_HANDLE  handle,
                         int16_t             *gain_left,
                         int16_t             *gain_right)
{
    return false;
}

bool audio_pipe_gain_set(T_AUDIO_PIPE_HANDLE handle,
                         int16_t             gain_left,
                         int16_t             gain_right)
{
    return false;
}

bool audio_pipe_channel_out_set(T_AUDIO_PIPE_HANDLE handle,
                                uint8_t             channel_num,
                                uint32_t            channel_array[])
{
    return false;
}

bool audio_pipe_fill(T_AUDIO_PIPE_HANDLE    handle,
                     uint32_t               timestamp,
                     uint16_t               seq_num,
                     T_AUDIO_STREAM_STATUS  status,
                     uint8_t                frame_num,
                     void                  *buf,
                     uint16_t               len)
{
    return false;
}

bool audio_pipe_drain(T_AUDIO_PIPE_HANDLE    handle,
                      uint32_t              *timestamp,
                      uint16_t              *seq_num,
                      T_AUDIO_STREAM_STATUS *status,
                      uint8_t               *frame_num,
                      void                  *buf,
                      uint16_t              *len)
{
    return false;
}

bool audio_pipe_pre_mix(T_AUDIO_PIPE_HANDLE prime_handle,
                        T_AUDIO_PIPE_HANDLE auxiliary_handle)
{
    return false;
}

bool audio_pipe_post_mix(T_AUDIO_PIPE_HANDLE prime_handle,
                         T_AUDIO_PIPE_HANDLE auxiliary_handle)
{
    return false;
}

bool audio_pipe_demix(T_AUDIO_PIPE_HANDLE prime_handle,
                      T_AUDIO_PIPE_HANDLE auxiliary_handle)
{
    return false;
}

bool audio_pipe_asrc_toggle(T_AUDIO_PIPE_HANDLE handle,
                            uint8_t             toggle)
{
    return false;
}

bool audio_pipe_asrc_set(T_AUDIO_PIPE_HANDLE handle,
                         int32_t             ratio)
{
    return false;
}
#endif
