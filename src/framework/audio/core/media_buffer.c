/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "trace.h"
#include "jitter_buffer.h"
#include "media_pool.h"
#include "media_buffer.h"
#include "audio_codec.h"
#include "audio_remote.h"
#include "audio_latency.h"
#include "audio_interval_eval.h"

/* TODO Remove Start */
#include "audio_mgr.h"
#include "audio_path.h"
#include "bt_mgr.h"
#include "remote.h"

extern bool bt_clk_compare(uint32_t bt_clk_a, uint32_t bt_clk_b);

#define LOW_LATENCY_ASRC_ADJ_SPEED_PPM_LIM  500
#define LOW_LATENCY_ASRC_ADJ_DURATION_LIM   0x0fffffff/1000*(625/2)

/* TODO Remove End */

static const uint8_t default_uuid[8] = {0x44, 0x75, 0x6D, 0x6D, 0x79, 0x55, 0x49, 0x44}; /* DummyUID */

static const uint8_t preamble_count_table[] =
{
    2,      /* AUDIO_FORMAT_TYPE_PCM: 0 */
    3,      /* AUDIO_FORMAT_TYPE_CVSD: 1 */
    3,      /* AUDIO_FORMAT_TYPE_MSBC: 2 */
    4,      /* AUDIO_FORMAT_TYPE_SBC: 3 */
    2,      /* AUDIO_FORMAT_TYPE_AAC: 4 */
    2,      /* AUDIO_FORMAT_TYPE_OPUS: 5 */
    2,      /* AUDIO_FORMAT_TYPE_FLAC: 6 */
    3,      /* AUDIO_FORMAT_TYPE_MP3: 7 */
    2,      /* AUDIO_FORMAT_TYPE_LC3: 8 */
    4,      /* AUDIO_FORMAT_TYPE_LDAC: 9 */
    4,      /* AUDIO_FORMAT_TYPE_LHDC: 10 */
    0,      /* AUDIO_FORMAT_TYPE_G729: 11 */
    2,      /* AUDIO_FORMAT_TYPE_LC3PLUS: 12 */
};

typedef struct
{
    int32_t  final_asrc;
    uint32_t sync_clk;
    uint16_t event;
    uint8_t  clk_ref;
    uint8_t  tid;
} T_MEDIA_BUFFER_ASRC_SYNC_ADJUST;

typedef enum
{
    MEDIA_BUFFER_REMOTE_LOW_LAT_ADJUST,
    MEDIA_BUFFER_REMOTE_LOW_LAT_ADJUST_RESTORE,
    MEDIA_BUFFER_REMOTE_ASRC_SYNC_ADJUST,
    MEDIA_BUFFER_REMOTE_DYNAMIC_LATENCY_INC,
    MEDIA_BUFFER_REMOTE_DYNAMIC_LATENCY_DEC,
    MEDIA_BUFFER_REMOTE_DYNAMIC_LATENCY_INC_REQ,
    MEDIA_BUFFER_REMOTE_DYNAMIC_LATENCY_SET,
    MEDIA_BUFFER_REMOTE_DYNAMIC_LATENCY_RESUME,
    MEDIA_BUFFER_REMOTE_ULTRA_LOW_LATENCY_PLC_REQ,
    MEDIA_BUFFER_REMOTE_LOW_LATENCY_OVERRIDE,
    MEDIA_BUFFER_REMOTE_AUDIO_SYNCED,
    MEDIA_BUFFER_PRI_ONE_WAY_PLAY,
    MEDIA_BUFFER_REMOTE_RPT_EXCEPTION,
    MEDIA_BUFFER_REMOTE_QUERY_PATH_STATE,
    MEDIA_BUFFER_REMOTE_QUERY_PATH_STATE_RSP,
    MEDIA_BUFFER_REMOTE_PRI_RESET,
} T_MEDIA_BUFFER_REMOTE_MSG;

typedef struct t_media_buffer_db
{
    T_MEDIA_POOL_HANDLE playback_pool_handle;
    T_MEDIA_POOL_HANDLE voice_pool_handle;
    T_MEDIA_POOL_HANDLE record_pool_handle;

    T_OS_QUEUE          media_buffer_list;
    uint16_t            max_plc_count;
    uint8_t             latency_keeper_policy;
} T_MEDIA_BUFFER_DB;

static T_MEDIA_BUFFER_DB media_buffer_db = {0};

void media_buffer_audio_remote_evt_cback(T_AUDIO_REMOTE_EVT  event,
                                         void               *param,
                                         void               *owner);
void media_buffer_audio_remote_relay_cback(uint16_t               event,
                                           void                  *param,
                                           T_REMOTE_RELAY_STATUS  status,
                                           void                  *owner);
void media_buffer_jb_evt_cback(T_JITTER_BUFFER_EVT  event,
                               void                *param,
                               void                *owner);
void media_buffer_lat_evt_cback(T_AUDIO_LATENCY_EVT  event,
                                void                *param,
                                void                *owner);
void media_buffer_interval_eval_evt_cback(T_AUDIO_INTERVAL_EVAL_EVT  event,
                                          void                      *param,
                                          void                      *owner);

void media_buffer_backup_update(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                                T_MEDIA_DATA_HDR      *mediapacket)
{
    if (buffer_ent->last_packet_ptr != NULL)
    {
        media_buffer_put(media_buffer_db.playback_pool_handle, buffer_ent->last_packet_ptr);
        buffer_ent->last_packet_ptr = mediapacket;
    }
    else
    {
        buffer_ent->last_packet_ptr = mediapacket;
    }
}

bool media_buffer_flush(T_MEDIA_BUFFER_ENTITY    *buffer_ent,
                        uint16_t                  cnt,
                        T_MEDIA_BUFFER_DIRECTION  direction)
{
    uint16_t i;
    T_OS_QUEUE *p_queue;
    T_MEDIA_DATA_HDR *p_hdr;
    uint16_t total_frames = 0;
    int32_t ret = 0;

    if (buffer_ent == NULL)
    {
        ret = 1;
        goto fail_invalid_entity;
    }

    if (direction == BUFFER_DIR_DOWNSTREAM)
    {
        p_queue = &buffer_ent->down_queue;
    }
    else
    {
        p_queue = &buffer_ent->up_queue;
    }

    if (cnt != 0)
    {
        if (cnt > p_queue->count)
        {
            cnt = p_queue->count;
        }

        for (i = 0; i < cnt; i++)
        {
            p_hdr = os_queue_out(p_queue);
            if (p_hdr != NULL)
            {
                if (direction == BUFFER_DIR_DOWNSTREAM)
                {
                    buffer_ent->down_frame_num -= p_hdr->frame_number;
                    total_frames = buffer_ent->down_frame_num;
                }
                else
                {
                    buffer_ent->up_frame_num -= p_hdr->frame_number;
                    total_frames = buffer_ent->up_frame_num;
                }

                if (buffer_ent->mode == AUDIO_STREAM_MODE_LOW_LATENCY ||
                    buffer_ent->mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
                {
                    media_buffer_backup_update(buffer_ent, p_hdr);
                }
                else
                {
                    media_buffer_put(buffer_ent->pool_handle, p_hdr);
                }
            }
        }
    }

    AUDIO_PRINT_TRACE3("media_buffer_flush: cnt %u, direction %u, remain frames %u",
                       cnt, direction, total_frames);
    return true;

fail_invalid_entity:
    AUDIO_PRINT_ERROR1("media_buffer_flush: failed %d", -ret);
    return false;
}

void *media_buffer_peek(T_MEDIA_BUFFER_ENTITY    *buffer_ent,
                        int32_t                   index,
                        T_MEDIA_BUFFER_DIRECTION  direction)
{
    T_MEDIA_DATA_HDR *p_hdr;

    if (buffer_ent == NULL)
    {
        return NULL;
    }

    if (direction == BUFFER_DIR_DOWNSTREAM)
    {
        p_hdr = os_queue_peek(&buffer_ent->down_queue, index);
    }
    else
    {
        p_hdr = os_queue_peek(&buffer_ent->up_queue, index);
    }

    return p_hdr;
}

uint16_t media_buffer_shrink(T_MEDIA_BUFFER_ENTITY   *buffer_ent,
                             void                    *p_packet,
                             uint16_t                 len,
                             T_MEDIA_BUFFER_DIRECTION direction)
{
    uint16_t remain_len;
    uint16_t shrink_len;
    T_MEDIA_DATA_HDR *p_hdr = p_packet;
    T_AUDIO_PATH_DATA_HDR *p_h2d_hdr;
    T_OS_QUEUE *p_queue;

    if (buffer_ent == NULL || p_hdr == NULL)
    {
        return 0;
    }

    if (direction == BUFFER_DIR_DOWNSTREAM)
    {
        p_queue = &buffer_ent->down_queue;
    }
    else
    {
        p_queue = &buffer_ent->up_queue;
    }

    if (os_queue_search(p_queue, p_hdr) == false) //get upstream queue packet
    {
        return 0;
    }

    p_h2d_hdr = (T_AUDIO_PATH_DATA_HDR *)p_hdr->p_data;
    remain_len = p_h2d_hdr->payload_length - p_hdr->read_offset;
    shrink_len = len <= remain_len ? len : remain_len;
    p_hdr->read_offset += shrink_len;
    if (shrink_len == remain_len)
    {
        os_queue_delete(p_queue, p_hdr);
        media_buffer_put(buffer_ent->pool_handle, p_hdr);
    }

    return shrink_len;
}

bool media_buffer_write(T_MEDIA_BUFFER_ENTITY    *buffer_ent,
                        uint32_t                  timestamp,
                        uint16_t                  seq_number,
                        uint8_t                   frame_number,
                        uint8_t                   status,
                        T_MEDIA_BUFFER_DIRECTION  direction,
                        void                     *data,
                        uint16_t                  len,
                        uint16_t                 *written_len)
{
    T_MEDIA_DATA_HDR *p_packet;
    T_AUDIO_PATH_DATA_HDR *h2d_hdr;

    *written_len = 0;
    if (buffer_ent == NULL)
    {
        return false;
    }

    p_packet = media_buffer_get(buffer_ent->pool_handle,
                                sizeof(T_MEDIA_DATA_HDR) + sizeof(T_AUDIO_PATH_DATA_HDR) + len);
    if (p_packet == NULL)
    {
        return false;
    }

    p_packet->payload_length   = len + sizeof(T_AUDIO_PATH_DATA_HDR);
    p_packet->timestamp        = timestamp;
    p_packet->upper_seq_number = seq_number;
    p_packet->frame_number     = frame_number;
    p_packet->read_offset      = 0;

    h2d_hdr = (T_AUDIO_PATH_DATA_HDR *)p_packet->p_data;
    h2d_hdr->timestamp = p_packet->timestamp;
    h2d_hdr->seq_num = p_packet->local_seq_number;
    h2d_hdr->frame_number = p_packet->frame_number;
    h2d_hdr->payload_length = len;
    h2d_hdr->status = status;

    memcpy(p_packet->p_data + sizeof(T_AUDIO_PATH_DATA_HDR), data, len);
    if (direction == BUFFER_DIR_DOWNSTREAM)
    {
        os_queue_in(&buffer_ent->down_queue, p_packet);
        buffer_ent->down_frame_num += p_packet->frame_number;
        *written_len = len;
    }
    else
    {
        os_queue_in(&buffer_ent->up_queue, p_packet);
        buffer_ent->up_frame_num += p_packet->frame_number;
        *written_len = len;
    }

    return true;
}

void media_buffer_reset(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    if (buffer_ent == NULL)
    {
        return;
    }

    media_buffer_flush(buffer_ent, media_buffer_ds_pkt_cnt(buffer_ent),
                       BUFFER_DIR_DOWNSTREAM);
    media_buffer_flush(buffer_ent, media_buffer_us_pkt_cnt(buffer_ent),
                       BUFFER_DIR_UPSTREAM);
    buffer_ent->last_seq = 0;
    buffer_ent->last_seq_updated = false;
    buffer_ent->down_frame_num = 0;
    buffer_ent->up_frame_num   = 0;

    buffer_ent->state = MEDIA_BUFFER_STATE_IDLE;
    buffer_ent->miss_ack_cnt = 0;
    media_buffer_clear_downstream_info(buffer_ent);
    buffer_ent->ignore_latency_report = 0;
    buffer_ent->local_audio_sync = false;

    media_buffer_backup_update(buffer_ent, NULL);
    audio_latency_buffer_reset(buffer_ent->audio_latency_handle);
    jitter_buffer_buffer_reset(buffer_ent->jitter_buffer_handle);
    audio_remote_reset(buffer_ent->audio_remote_handle);
}

void media_buffer_dump(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{

}

T_MEDIA_BUFFER_ENTITY *media_buffer_construct(T_AUDIO_STREAM_TYPE  stream_type,
                                              T_AUDIO_STREAM_USAGE usage,
                                              T_AUDIO_STREAM_MODE  mode,
                                              T_AUDIO_FORMAT_INFO *format_info,
                                              void                *track_handle,
                                              void                *path_handle)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;

    buffer_ent = calloc(1, sizeof(T_MEDIA_BUFFER_ENTITY));
    if (buffer_ent != NULL)
    {
        buffer_ent->state = MEDIA_BUFFER_STATE_IDLE;
        os_queue_init(&buffer_ent->down_queue);
        os_queue_init(&buffer_ent->up_queue);

        buffer_ent->mode = mode;
        buffer_ent->track_handle = track_handle;
        buffer_ent->path_handle = path_handle;
        buffer_ent->usage = usage;
        buffer_ent->stream_type = stream_type;
        memcpy(&buffer_ent->format_info, format_info, sizeof(T_AUDIO_FORMAT_INFO));
        memcpy(buffer_ent->uuid, default_uuid, 8);
        os_queue_in(&media_buffer_db.media_buffer_list, buffer_ent);

        if (stream_type == AUDIO_STREAM_TYPE_RECORD)
        {
            buffer_ent->pool_handle = media_buffer_db.record_pool_handle;
        }
        else
        {
            if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                buffer_ent->pool_handle = media_buffer_db.playback_pool_handle;
                buffer_ent->jitter_buffer_handle = jitter_buffer_register(buffer_ent,
                                                                          media_buffer_jb_evt_cback);
                buffer_ent->audio_latency_handle = audio_latency_mgr_register(buffer_ent,
                                                                              media_buffer_lat_evt_cback);
                buffer_ent->audio_interval_eval_handle = audio_interval_evaluator_register(buffer_ent,
                                                                                           media_buffer_interval_eval_evt_cback);
            }
            else if (stream_type == AUDIO_STREAM_TYPE_VOICE)
            {
                buffer_ent->pool_handle = media_buffer_db.voice_pool_handle;
            }

            buffer_ent->audio_remote_handle = audio_remote_register(stream_type,
                                                                    buffer_ent,
                                                                    media_buffer_audio_remote_evt_cback,
                                                                    media_buffer_audio_remote_relay_cback);
        }
    }

    return buffer_ent;
}

void media_buffer_destruct(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    if (buffer_ent != NULL)
    {
        media_buffer_reset(buffer_ent);
        if (buffer_ent->solo_play_timer)
        {
            sys_timer_delete(buffer_ent->solo_play_timer);
        }

        if (buffer_ent->audio_remote_handle)
        {
            audio_remote_unregister(buffer_ent->audio_remote_handle);
        }

        if (buffer_ent->jitter_buffer_handle)
        {
            jitter_buffer_unregister(buffer_ent->jitter_buffer_handle);
        }

        if (buffer_ent->audio_latency_handle)
        {
            audio_latency_mgr_unregister(buffer_ent->audio_latency_handle);
        }

        if (buffer_ent->audio_interval_eval_handle)
        {
            audio_interval_evaluator_unregister(buffer_ent->audio_interval_eval_handle);
        }

        os_queue_delete(&media_buffer_db.media_buffer_list, buffer_ent);
        free(buffer_ent);
    }
}

bool media_buffer_init(uint16_t playback_pool_size,
                       uint16_t voice_pool_size,
                       uint16_t record_pool_size)
{
    int32_t ret = 0;

    os_queue_init(&media_buffer_db.media_buffer_list);
    media_buffer_db.latency_keeper_policy = LATENCY_KEEPER_POLICY_BY_ASRC;
    media_buffer_db.max_plc_count         = DEFAULT_RESET_PLC_COUNT;

    if (playback_pool_size != 0)
    {
        media_buffer_db.playback_pool_handle = media_pool_create(playback_pool_size);
        if (media_buffer_db.playback_pool_handle == NULL)
        {
            ret = 1;
            goto fail_create_audio_pool;
        }
    }

    if (voice_pool_size != 0)
    {
        media_buffer_db.voice_pool_handle = media_pool_create(voice_pool_size);
        if (media_buffer_db.voice_pool_handle == NULL)
        {
            ret = 2;
            goto fail_create_voice_pool;
        }
    }

    if (record_pool_size != 0)
    {
        media_buffer_db.record_pool_handle = media_pool_create(record_pool_size);
        if (media_buffer_db.record_pool_handle == NULL)
        {
            ret = 3;
            goto fail_create_record_pool;
        }
    }

    if (jb_init() == false)
    {
        ret = 4;
        goto fail_init_jb;
    }

    return true;

fail_init_jb:
    media_pool_destroy(media_buffer_db.record_pool_handle);
    media_buffer_db.record_pool_handle = NULL;
fail_create_record_pool:
    media_pool_destroy(media_buffer_db.voice_pool_handle);
    media_buffer_db.voice_pool_handle = NULL;
fail_create_voice_pool:
    media_pool_destroy(media_buffer_db.playback_pool_handle);
    media_buffer_db.playback_pool_handle = NULL;
fail_create_audio_pool:
    AUDIO_PRINT_ERROR1("media_buffer_init: failed %d", -ret);
    return false;
}

void media_buffer_deinit(void)
{
    jb_deinit();

    if (media_buffer_db.playback_pool_handle != NULL)
    {
        media_pool_destroy(media_buffer_db.playback_pool_handle);
        media_buffer_db.playback_pool_handle = NULL;
    }

    if (media_buffer_db.voice_pool_handle != NULL)
    {
        media_pool_destroy(media_buffer_db.voice_pool_handle);
        media_buffer_db.voice_pool_handle = NULL;
    }

    if (media_buffer_db.record_pool_handle != NULL)
    {
        media_pool_destroy(media_buffer_db.record_pool_handle);
        media_buffer_db.record_pool_handle = NULL;
    }
}

uint16_t media_buffer_cacu_frame_num(uint16_t             time_ms,
                                     T_AUDIO_FORMAT_INFO *format_info)
{
    uint16_t frame_num = 0;
    uint32_t frame_duration;

    frame_duration = audio_codec_frame_duration_get(format_info->type, &format_info->attr);
    if (frame_duration != 0)
    {
        frame_num = (uint16_t)((uint32_t)time_ms * 1000 / frame_duration + 1);
    }

    return frame_num;
}

uint16_t media_buffer_cacu_frames_duration(uint16_t             frame_num,
                                           T_AUDIO_FORMAT_INFO *format_info)
{
    uint32_t frame_duration_us;
    uint16_t total_duration_ms;

    frame_duration_us = audio_codec_frame_duration_get(format_info->type, &format_info->attr);
    total_duration_ms = (uint16_t)(frame_duration_us * frame_num / 1000);
    return total_duration_ms;
}

bool media_buffer_get_downstream_info(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                                      uint16_t              *ds_frame_counter_last,
                                      uint16_t              *ds_local_seq_last)
{
    if (buffer_ent != NULL)
    {
        if (ds_frame_counter_last != NULL)
        {
            *ds_frame_counter_last = buffer_ent->ds_frame_counter_last;
        }

        if (ds_local_seq_last != NULL)
        {
            *ds_local_seq_last = buffer_ent->ds_local_seq_last;
        }

        return true;
    }

    return false;
}

bool media_buffer_clear_downstream_info(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    if (buffer_ent != NULL)
    {
        buffer_ent->ds_frame_counter_last = 0;
        buffer_ent->ds_local_seq_last = 0;
        return true;
    }

    return false;
}

bool media_buffer_set_downstream_info(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                                      uint16_t               ds_frame_counter_last,
                                      uint16_t               ds_local_seq_last)
{
    if (buffer_ent != NULL)
    {
        buffer_ent->ds_frame_counter_last = ds_frame_counter_last;
        buffer_ent->ds_local_seq_last = ds_local_seq_last;
        return true;
    }

    return false;
}

T_MEDIA_BUFFER_STATE media_buffer_state_get(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    T_MEDIA_BUFFER_STATE state = MEDIA_BUFFER_STATE_IDLE;
    if (buffer_ent != NULL)
    {
        state = buffer_ent->state;
    }

    return state;
}

void media_buffer_state_set(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                            T_MEDIA_BUFFER_STATE   state)
{
    if (buffer_ent != NULL)
    {
        AUDIO_PRINT_INFO2("media_buffer_state_set: stream_type %u, state %u",
                          buffer_ent->stream_type, state);
        buffer_ent->state = state;
    }
}

uint16_t media_buffer_ds_frame_cnt(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    if (buffer_ent != NULL)
    {
        return buffer_ent->down_frame_num;
    }

    return 0;
}

uint16_t media_buffer_ds_pkt_cnt(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    if (buffer_ent != NULL)
    {
        return buffer_ent->down_queue.count;
    }

    return 0;
}

uint16_t media_buffer_us_frame_cnt(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    if (buffer_ent != NULL)
    {
        return buffer_ent->up_frame_num;
    }

    return 0;
}

uint16_t media_buffer_us_pkt_cnt(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    if (buffer_ent != NULL)
    {
        return buffer_ent->up_queue.count;
    }

    return 0;
}

void media_buffer_proxy_latency_get(T_MEDIA_BUFFER_PROXY  proxy,
                                    uint16_t             *latency)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;

    buffer_ent = (T_MEDIA_BUFFER_ENTITY *)proxy;
    if (buffer_ent != NULL)
    {
        *latency = buffer_ent->latency;
    }
}

void media_buffer_proxy_latency_set(T_MEDIA_BUFFER_PROXY proxy,
                                    uint16_t             latency,
                                    bool                 fixed)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;

    buffer_ent = (T_MEDIA_BUFFER_ENTITY *)proxy;
    if (buffer_ent != NULL)
    {
        buffer_ent->latency_fixed = fixed;
        buffer_ent->latency = latency;
        buffer_ent->latency_back_up = latency;
        audio_latency_set_dynamic_latency_on(buffer_ent->audio_latency_handle, false);
        buffer_ent->plc_count = 0;
        buffer_ent->latency_override = false;

        buffer_ent->start_decode_frame_cnt = media_buffer_cacu_frame_num(buffer_ent->latency,
                                                                         &(buffer_ent->format_info));
        jitter_buffer_target_latency_update(buffer_ent->jitter_buffer_handle,
                                            buffer_ent->latency);
    }
}

void media_buffer_proxy_latency_set2(T_MEDIA_BUFFER_PROXY proxy,
                                     uint16_t             latency,
                                     bool                 fixed,
                                     float                var)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;
    bool dynamic_latency_on = false;

    if (proxy == NULL)
    {
        return;
    }

    if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }

    buffer_ent = (T_MEDIA_BUFFER_ENTITY *)proxy;
    audio_latency_get_dynamic_latency_on(buffer_ent->audio_latency_handle,
                                         &dynamic_latency_on);

    if (buffer_ent->latency_override == false)
    {
        if (var > (latency / 2) * (latency / 2))
        {
            T_AUDIO_MSG_LOW_LATENCY_OVERRIDE cmd;
            AUDIO_PRINT_WARN4("the latency %ums is not suitable for this device, inc latency to %u current latency %u current plc cnt %u",
                              latency, latency * 2, buffer_ent->latency, buffer_ent->plc_count);

            buffer_ent->latency_fixed = fixed;
            buffer_ent->latency_back_up = latency * 2;
            buffer_ent->latency_override = true;

            if (!dynamic_latency_on)
            {
                buffer_ent->latency = latency * 2;
                buffer_ent->plc_count = 0;
            }

            cmd.dynamic_latency_on = dynamic_latency_on;
            cmd.plc_count = buffer_ent->plc_count;
            cmd.latency = buffer_ent->latency;
            cmd.latency_override = buffer_ent->latency_override;
            cmd.latency_backup = buffer_ent->latency_back_up;

            audio_remote_async_msg_relay(buffer_ent->audio_remote_handle,
                                         MEDIA_BUFFER_REMOTE_LOW_LATENCY_OVERRIDE,
                                         &cmd,
                                         sizeof(T_AUDIO_MSG_LOW_LATENCY_OVERRIDE),
                                         false);
        }
    }
    else
    {
        if (var < ((latency - 4) / 2) * ((latency - 4) / 2))
        {
            T_AUDIO_MSG_LOW_LATENCY_OVERRIDE cmd;

            AUDIO_PRINT_WARN4("restore the latency %ums to %ums current latency %u current plc cnt %u",
                              buffer_ent->latency_back_up, latency, buffer_ent->latency, buffer_ent->plc_count);

            buffer_ent->latency_fixed = fixed;
            buffer_ent->latency_back_up = latency;
            buffer_ent->latency_override = false;

            if (!dynamic_latency_on)
            {
                buffer_ent->latency = latency;
                buffer_ent->plc_count = 0;
            }

            cmd.dynamic_latency_on = dynamic_latency_on;
            cmd.plc_count = buffer_ent->plc_count;
            cmd.latency = buffer_ent->latency;
            cmd.latency_override = buffer_ent->latency_override;
            cmd.latency_backup = buffer_ent->latency_back_up;

            audio_remote_async_msg_relay(buffer_ent->audio_remote_handle,
                                         MEDIA_BUFFER_REMOTE_LOW_LATENCY_OVERRIDE,
                                         &cmd,
                                         sizeof(T_AUDIO_MSG_LOW_LATENCY_OVERRIDE),
                                         false);
        }
    }
}

bool media_buffer_proxy_level_get(T_MEDIA_BUFFER_PROXY  proxy,
                                  uint16_t             *level)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;

    buffer_ent = (T_MEDIA_BUFFER_ENTITY *)proxy;
    if (buffer_ent != NULL)
    {
        *level = media_buffer_cacu_frames_duration(media_buffer_ds_frame_cnt(buffer_ent),
                                                   &(buffer_ent->format_info));
        return true;
    }

    return false;
}

void media_buffer_proxy_uuid_set(T_MEDIA_BUFFER_PROXY proxy,
                                 uint8_t              uuid[8])
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;

    buffer_ent = (T_MEDIA_BUFFER_ENTITY *)proxy;
    if (buffer_ent != NULL)
    {
        memcpy(buffer_ent->uuid, uuid, 8);
    }
}
void media_buffer_proxy_threshold_set(T_MEDIA_BUFFER_PROXY proxy,
                                      uint16_t             upper_threshold_ms,
                                      uint16_t             lower_threshold_ms)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;

    buffer_ent = (T_MEDIA_BUFFER_ENTITY *)proxy;
    if (buffer_ent != NULL)
    {
        buffer_ent->lower_threshold_ms = lower_threshold_ms;
        buffer_ent->upper_threshold_ms = upper_threshold_ms;
    }
}

T_MEDIA_BUFFER_PROXY media_buffer_proxy_attach(T_AUDIO_STREAM_TYPE  stream_type,
                                               T_AUDIO_STREAM_USAGE usage,
                                               T_AUDIO_STREAM_MODE  mode,
                                               T_AUDIO_FORMAT_INFO *format_info,
                                               void                *track_handle,
                                               void                *path_handle)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;

    buffer_ent = media_buffer_construct(stream_type,
                                        usage,
                                        mode,
                                        format_info,
                                        track_handle,
                                        path_handle);

    return (T_MEDIA_BUFFER_PROXY)buffer_ent;
}

void media_buffer_proxy_detach(T_MEDIA_BUFFER_PROXY proxy)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;

    if (proxy != NULL)
    {
        buffer_ent = (T_MEDIA_BUFFER_ENTITY *)proxy;
        media_buffer_destruct(buffer_ent);
    }
}

uint16_t media_buffer_proxy_us_write(T_MEDIA_BUFFER_PROXY  proxy,
                                     void                 *buf,
                                     uint16_t              len,
                                     uint32_t              timestamp,
                                     uint16_t              seq,
                                     T_AUDIO_STREAM_STATUS status,
                                     uint8_t               frame_num)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;
    uint16_t written_len;
    int32_t ret = 0;

    if (proxy == NULL || buf == NULL)
    {
        ret = 1;
        goto fail_invalid_proxy;
    }

    buffer_ent = (T_MEDIA_BUFFER_ENTITY *)proxy;

    if (media_buffer_write(buffer_ent,
                           timestamp,
                           seq,
                           frame_num,
                           status,
                           BUFFER_DIR_UPSTREAM,
                           buf,
                           len,
                           &written_len) == false)
    {
        ret = 2;
        goto fail_write_buffer;
    }

    return written_len;

fail_write_buffer:
fail_invalid_proxy:
    AUDIO_PRINT_ERROR1("media_buffer_proxy_us_write: failed %d", -ret);
    return 0;
}

bool media_buffer_proxy_ds_write(T_MEDIA_BUFFER_PROXY  proxy,
                                 uint32_t              timestamp,
                                 uint16_t              seq,
                                 T_AUDIO_STREAM_STATUS status,
                                 uint8_t               frame_num,
                                 void                 *buf,
                                 uint16_t              len,
                                 uint16_t             *written_len)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;
    uint8_t dup_flag = 0;
    uint16_t buffer_level = 0;
    int32_t ret = 0;

    *written_len = 0;
    buffer_ent = (T_MEDIA_BUFFER_ENTITY *)proxy;
    if (buffer_ent == NULL)
    {
        ret = 1;
        goto fail_invalid_proxy;
    }

    if (buffer_ent->last_seq_updated == false)
    {
        buffer_ent->last_seq = seq;
        buffer_ent->last_ts = timestamp;
        buffer_ent->last_seq_updated = true;
    }
    else
    {
        if (seq == buffer_ent->last_seq)
        {
            ret = 2;
            goto fail_abort_dup_seq_pkt;
        }
        else
        {
            if ((uint16_t)(buffer_ent->last_seq + 1) != seq)
            {
                dup_flag = seq - (uint16_t)(buffer_ent->last_seq + 1);
                AUDIO_PRINT_INFO3("lost packet last %u, seq %u dup %u", buffer_ent->last_seq, seq, dup_flag);
                if (dup_flag >= media_buffer_db.max_plc_count + MAX_PLC_EXC_CNT &&
                    buffer_ent->mode != AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY &&
                    buffer_ent->mode != AUDIO_STREAM_MODE_DIRECT &&
                    buffer_ent->stream_type != AUDIO_STREAM_TYPE_VOICE)
                {
                    audio_mgr_exception(buffer_ent, MEDIA_BUFFER_PACKET_LOST, 0, 0);
                    ret = 3;
                    goto fail_packet_lost;
                }
                else
                {
                    audio_remote_handle_lost_packet(buffer_ent->audio_remote_handle, dup_flag);
                    if (dup_flag > media_buffer_db.max_plc_count)
                    {
                        dup_flag = media_buffer_db.max_plc_count;
                    }
                }
            }

            buffer_ent->last_seq = seq;
            buffer_ent->last_ts = timestamp;
        }
    }

    if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_PLAY)
    {
        uint16_t len1 = 0;

        media_buffer_proxy_level_get((T_MEDIA_BUFFER_PROXY)buffer_ent, &buffer_level);
        if (buffer_ent->mode != AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY &&
            buffer_ent->mode != AUDIO_STREAM_MODE_DIRECT)
        {
            while (dup_flag)
            {
                dup_flag--;
                media_buffer_write(buffer_ent,
                                   timestamp,
                                   seq,
                                   frame_num,
                                   AUDIO_STREAM_STATUS_LOST,
                                   BUFFER_DIR_DOWNSTREAM,
                                   buf,
                                   len,
                                   &len1);
            }
        }
    }

    if (media_buffer_write(buffer_ent,
                           timestamp,
                           seq,
                           frame_num,
                           status,
                           BUFFER_DIR_DOWNSTREAM,
                           buf,
                           len,
                           written_len) == false)
    {
        audio_mgr_exception(buffer_ent, MEDIA_BUFFER_FULL, 0, 0);
        ret = 4;
        goto fail_write_buffer;
    }

    AUDIO_PRINT_INFO8("media_buffer_write: role %u mode|state 0x%x frame %u total frames %u seq %u ts 0x%x lat %u plc_cnt %u",
                      remote_session_role_get(), (buffer_ent->mode << 8) | buffer_ent->state, frame_num,
                      media_buffer_ds_frame_cnt(buffer_ent),
                      seq, timestamp, buffer_ent->latency, buffer_ent->plc_count);

    if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_PLAY &&
        buffer_ent->mode == AUDIO_STREAM_MODE_NORMAL &&
        buffer_level < buffer_ent->upper_threshold_ms)
    {
        media_buffer_proxy_level_get((T_MEDIA_BUFFER_PROXY)buffer_ent, &buffer_level);
        if (buffer_level >= buffer_ent->upper_threshold_ms)
        {
            audio_mgr_dispatch(AUDIO_MSG_BUFFER_LEVEL_HIGH, buffer_ent);
        }
    }

    audio_mgr_dispatch(AUDIO_MSG_BUFFER_WRITE, buffer_ent);

    return true;

fail_write_buffer:
fail_packet_lost:
fail_abort_dup_seq_pkt:
fail_invalid_proxy:
    AUDIO_PRINT_ERROR1("media_buffer_proxy_ds_write: failed %d", -ret);
    return false;
}


uint16_t media_buffer_proxy_us_read(T_MEDIA_BUFFER_PROXY   proxy,
                                    void                  *buf,
                                    uint16_t               len,
                                    uint32_t              *timestamp,
                                    uint16_t              *seq_num,
                                    T_AUDIO_STREAM_STATUS *status,
                                    uint8_t               *frame_num)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;
    T_MEDIA_DATA_HDR *p_packet;
    T_AUDIO_PATH_DATA_HDR *p_h2d_hdr;
    uint16_t read_len;
    uint8_t *p_data;
    uint16_t data_len;
    int32_t ret = 0;

    if (proxy == NULL || buf == NULL)
    {
        ret = 1;
        goto fail_invalid_proxy;
    }

    buffer_ent = (T_MEDIA_BUFFER_ENTITY *)proxy;;
    p_packet = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_UPSTREAM);
    if (p_packet == NULL)
    {
        ret = 2;
        goto fail_null_pkt;
    }

    p_h2d_hdr = (T_AUDIO_PATH_DATA_HDR *)p_packet->p_data;
    p_data = p_h2d_hdr->payload;
    data_len = p_h2d_hdr->payload_length;
    if (len > (data_len - p_packet->read_offset))
    {
        read_len = data_len - p_packet->read_offset;
    }
    else
    {
        read_len = len;
    }

    memcpy(buf, p_data + p_packet->read_offset, read_len);
    *seq_num = p_packet->upper_seq_number;
    *timestamp = p_packet->timestamp;
    *frame_num = p_packet->frame_number;
    *status    = (T_AUDIO_STREAM_STATUS)p_h2d_hdr->status;

    media_buffer_shrink(buffer_ent, p_packet, read_len, BUFFER_DIR_UPSTREAM);
    return read_len;

fail_null_pkt:
fail_invalid_proxy:
    AUDIO_PRINT_ERROR1("media_buffer_proxy_us_read: failed %d", -ret);
    return 0;
}

void media_buffer_proxy_reset(T_MEDIA_BUFFER_PROXY proxy)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;

    if (proxy != NULL)
    {
        buffer_ent = (T_MEDIA_BUFFER_ENTITY *)proxy;
        audio_latency_set_dynamic_latency_on(buffer_ent->audio_latency_handle, false);
        buffer_ent->plc_count = 0;
        buffer_ent->latency = buffer_ent->latency_back_up;
        audio_latency_reset(buffer_ent->audio_latency_handle);
        media_buffer_reset(buffer_ent);
    }
}

bool media_buffer_backup_downstream(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    T_MEDIA_DATA_HDR *p_packet;
    T_AUDIO_PATH_DATA_HDR *p_h2d_hdr;
    int32_t ret = 0;

    if (buffer_ent == NULL)
    {
        ret = 1;
        goto fail_invalid_entity;
    }

    p_packet = (T_MEDIA_DATA_HDR *)buffer_ent->last_packet_ptr;
    if (p_packet == NULL)
    {
        ret = 2;
        goto fail_null_packet;
    }

    p_h2d_hdr = (T_AUDIO_PATH_DATA_HDR *)(p_packet->p_data);
    p_packet->frame_counter = buffer_ent->ds_frame_counter_last + p_packet->frame_number;
    p_packet->local_seq_number = buffer_ent->ds_local_seq_last + 1;
    p_h2d_hdr->seq_num = buffer_ent->ds_local_seq_last + 1;
    p_h2d_hdr->frame_count = p_packet->frame_counter;

    if (audio_path_data_send(buffer_ent->path_handle,
                             (T_AUDIO_PATH_DATA_HDR *)p_packet->p_data,
                             p_packet->payload_length))
    {
        buffer_ent->ds_local_seq_last++;
        buffer_ent->ds_frame_counter_last = buffer_ent->ds_frame_counter_last + p_packet->frame_number;
        audio_remote_record_push(buffer_ent->audio_remote_handle, p_packet);

        AUDIO_PRINT_INFO7("media_buffer_backup_downstream: seq %u, local_seq %u, packet_cnt %u, bt_clock 0x%x, frame_couter %u, frame_num %d, codec %u, miss ack %u",
                          p_packet->upper_seq_number, p_packet->local_seq_number,
                          media_buffer_ds_pkt_cnt(buffer_ent), p_packet->timestamp, p_packet->frame_counter,
                          p_packet->frame_number, buffer_ent->miss_ack_cnt);

        return true;
    }
    else
    {
        ret = 3;
        goto fail_path_data_send;
    }

fail_path_data_send:
fail_null_packet:
fail_invalid_entity:
    AUDIO_PRINT_ERROR1("media_buffer_backup_downstream: failed %d", -ret);
    return false;
}

bool media_buffer_downstream_dsp(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    T_MEDIA_DATA_HDR *p_packet;
    T_AUDIO_PATH_DATA_HDR *p_h2d_hdr;
    uint16_t buffer_level = 0;
    int32_t ret = 0;

    if (buffer_ent == NULL)
    {
        ret = 1;
        goto fail_invalid_entity;
    }

    p_packet = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);
    if (p_packet == NULL)
    {
        return false;
    }

    p_h2d_hdr = (T_AUDIO_PATH_DATA_HDR *)(p_packet->p_data);

    p_packet->frame_counter = buffer_ent->ds_frame_counter_last + p_packet->frame_number;
    p_packet->local_seq_number = buffer_ent->ds_local_seq_last + 1;

    if (buffer_ent->mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY ||
        buffer_ent->mode == AUDIO_STREAM_MODE_DIRECT ||
        buffer_ent->stream_type == AUDIO_STREAM_TYPE_VOICE)
    {
        p_h2d_hdr->seq_num = p_packet->upper_seq_number;
    }
    else
    {
        p_h2d_hdr->seq_num = buffer_ent->ds_local_seq_last + 1;
    }
    p_h2d_hdr->frame_count = p_packet->frame_counter;

    if (audio_path_data_send(buffer_ent->path_handle,
                             (T_AUDIO_PATH_DATA_HDR *)p_packet->p_data,
                             p_packet->payload_length) == false)
    {
        ret = 2;
        goto fail_data_send;
    }

    media_buffer_proxy_level_get((T_MEDIA_BUFFER_PROXY)buffer_ent, &buffer_level);

    buffer_ent->ds_local_seq_last++;
    buffer_ent->ds_frame_counter_last = buffer_ent->ds_frame_counter_last + p_packet->frame_number;
    audio_remote_record_push(buffer_ent->audio_remote_handle, p_packet);

    AUDIO_PRINT_TRACE7("media_buffer_downstream: seq %u, local_seq %u, packet_cnt %u, bt_clock 0x%x, frame_couter %u, frame_num %d, miss ack %u",
                       p_packet->upper_seq_number, p_packet->local_seq_number,
                       media_buffer_ds_pkt_cnt(buffer_ent), p_packet->timestamp, p_packet->frame_counter,
                       p_packet->frame_number, buffer_ent->miss_ack_cnt);
    media_buffer_flush(buffer_ent, 1, BUFFER_DIR_DOWNSTREAM);

    if (buffer_ent->lower_threshold_ms != 0 &&
        buffer_ent->mode == AUDIO_STREAM_MODE_NORMAL &&
        buffer_level > buffer_ent->lower_threshold_ms)
    {
        media_buffer_proxy_level_get((T_MEDIA_BUFFER_PROXY)buffer_ent, &buffer_level);
        if (buffer_level <= buffer_ent->lower_threshold_ms)
        {
            audio_mgr_dispatch(AUDIO_MSG_BUFFER_LEVEL_LOW, buffer_ent);
        }
    }

    return true;

fail_data_send:
fail_invalid_entity:
    AUDIO_PRINT_ERROR1("media_buffer_downstream: failed %d", -ret);
    return false;
}

bool media_buffer_get_stream_type(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                                  T_AUDIO_STREAM_TYPE   *type)
{

    if (buffer_ent == NULL)
    {
        return false;
    }

    *type = buffer_ent->stream_type;
    return true;
}

bool media_buffer_get_mode(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                           T_AUDIO_STREAM_MODE   *mode)
{

    if (buffer_ent == NULL)
    {
        return false;
    }

    *mode = buffer_ent->mode;
    return true;
}

bool media_buffer_get_format_type(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                                  T_AUDIO_FORMAT_TYPE   *type)
{
    if (buffer_ent == NULL)
    {
        return false;
    }

    *type = buffer_ent->format_info.type;
    return true;
}

void media_buffer_leading_downstream(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    uint8_t preamble_count;
    uint8_t ds_num;

    preamble_count = preamble_count_table[buffer_ent->format_info.type];
    ds_num = media_buffer_ds_pkt_cnt(buffer_ent);
    if (ds_num > preamble_count)
    {
        ds_num = preamble_count;
    }

    for (uint8_t i = 0; i < ds_num; i++)
    {
        if (media_buffer_downstream_dsp(buffer_ent) == false)
        {
            buffer_ent->miss_ack_cnt++;
        }
    }
}

bool media_buffer_sync_lost(void)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;

    buffer_ent = os_queue_peek(&media_buffer_db.media_buffer_list, 0);
    while (buffer_ent != NULL)
    {
        if (buffer_ent->stream_type == AUDIO_STREAM_TYPE_PLAYBACK &&
            media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_PLAY)
        {
            audio_remote_reset(buffer_ent->audio_remote_handle);
            jitter_buffer_asrc_pid_unblock(buffer_ent->jitter_buffer_handle);
        }

        buffer_ent = buffer_ent->p_next;
    }

    return true;
}

T_MEDIA_BUFFER_ENTITY *media_buffer_find_buffer_by_uuid(T_AUDIO_STREAM_TYPE stream_type,
                                                        uint8_t             uuid[8])
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;

    buffer_ent = os_queue_peek(&media_buffer_db.media_buffer_list, 0);
    while (buffer_ent != NULL)
    {
        if (stream_type == buffer_ent->stream_type &&
            !memcmp(buffer_ent->uuid, uuid, 8))
        {
            break;
        }

        buffer_ent = buffer_ent->p_next;
    }

    return buffer_ent;
}

void media_buffer_timeout_cback(T_SYS_TIMER_HANDLE handle)
{
    uint32_t timer_id;
    T_MEDIA_BUFFER_ENTITY *buffer_ent;

    timer_id = sys_timer_id_get(handle);
    AUDIO_PRINT_TRACE2("media_buffer_timeout_cback: timer_id 0x%02X handle %p",
                       timer_id, handle);

    buffer_ent = (T_MEDIA_BUFFER_ENTITY *)timer_id;
    if (buffer_ent != NULL)
    {
        sys_timer_delete(buffer_ent->solo_play_timer);
        buffer_ent->solo_play_timer = NULL;

        if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
        {
            if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_PREQUEUE)
            {
                uint16_t play_time;
                uint32_t bb_clock_slot;
                uint32_t bb_clock_slot_sync;
                uint16_t bb_clock_us;
                uint8_t  clk_idx;
                T_BT_CLK_REF clk_ref;

                audio_remote_reset(buffer_ent->audio_remote_handle);

                while (media_buffer_ds_frame_cnt(buffer_ent) > buffer_ent->start_decode_frame_cnt / 2)
                {
                    media_buffer_flush(buffer_ent, 1, BUFFER_DIR_DOWNSTREAM);
                }

                play_time = media_buffer_playtime_calculate(buffer_ent);
                audio_path_synchronization_join_set(NULL, 0);

                clk_ref = bt_piconet_clk_get(BT_CLK_SNIFFING, &clk_idx, &bb_clock_slot, &bb_clock_us);
                if (clk_ref != BT_CLK_NONE)
                {
                    bb_clock_slot_sync = (bb_clock_slot + play_time * 1000 * 2 / 625) & 0x0fffffff;
                    media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_PLAY);
                    audio_remote_set_state(buffer_ent->audio_remote_handle, AUDIO_REMOTE_STATE_UNSYNC);
                    audio_path_timestamp_set(buffer_ent->path_handle, clk_idx, bb_clock_slot_sync, false);
                }
                else
                {
                    media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_PLAY);
                    audio_remote_set_state(buffer_ent->audio_remote_handle, AUDIO_REMOTE_STATE_UNSYNC);
                    audio_path_timestamp_set(buffer_ent->path_handle, clk_idx, 0xffffffff, false);
                }

                media_buffer_leading_downstream(buffer_ent);

                if (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
                {
                    audio_remote_async_msg_relay(buffer_ent->audio_remote_handle,
                                                 MEDIA_BUFFER_PRI_ONE_WAY_PLAY,
                                                 NULL,
                                                 0,
                                                 false);
                }

            }
        }
    }
}

void media_buffer_audio_remote_relay_cback(uint16_t               event,
                                           void                  *param,
                                           T_REMOTE_RELAY_STATUS  status,
                                           void                  *owner)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent = (T_MEDIA_BUFFER_ENTITY *)owner;

    AUDIO_PRINT_INFO3("media_buffer_audio_remote_relay_cback: event 0x%04x status %d owner %p",
                      event, status, owner);

    if (buffer_ent == NULL)
    {
        return;
    }

    switch (event)
    {
    case MEDIA_BUFFER_REMOTE_LOW_LAT_ADJUST_RESTORE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT || status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                T_BT_CLK_REF clk_ref;
                uint8_t  clk_idx;
                uint32_t bb_clock_slot;
                uint32_t bb_clock_rcv = *(uint32_t *)param;
                uint16_t bb_clock_us;

                clk_ref = bt_piconet_clk_get(BT_CLK_SNIFFING, &clk_idx, &bb_clock_slot, &bb_clock_us);
                if (clk_ref != BT_CLK_NONE &&
                    bt_clk_compare(bb_clock_rcv, bb_clock_slot + 5 * 2 * 1000 / 625))
                {
                    jitter_buffer_low_latency_asrc_restore(buffer_ent->jitter_buffer_handle, bb_clock_rcv);
                }
                else
                {
                    audio_mgr_exception(buffer_ent, MEDIA_BUFFER_ASRC_ADJ_OVERTIME, 0, 0);
                }
            }
        }
        break;

    case MEDIA_BUFFER_REMOTE_PRI_RESET:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                audio_mgr_exception(buffer_ent, MEDIA_BUFFER_PRI_RESET_BUFFER, 1, 0);
            }
        }
        break;

    case MEDIA_BUFFER_REMOTE_QUERY_PATH_STATE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (audio_path_is_running(buffer_ent->path_handle))
                {
                    audio_remote_async_msg_relay(buffer_ent->audio_remote_handle,
                                                 MEDIA_BUFFER_REMOTE_QUERY_PATH_STATE_RSP,
                                                 NULL,
                                                 0,
                                                 false);
                }
            }
        }
        break;

    case MEDIA_BUFFER_REMOTE_QUERY_PATH_STATE_RSP:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                buffer_ent->sec_path_ready = 1;
                media_buffer_flush(buffer_ent, media_buffer_ds_pkt_cnt(buffer_ent),
                                   BUFFER_DIR_DOWNSTREAM);
            }
        }
        break;

    case MEDIA_BUFFER_REMOTE_RPT_EXCEPTION:
        {
            T_AUDIO_EXC_CMD *p_cmd;
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                p_cmd = (T_AUDIO_EXC_CMD *)param;
                if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY &&
                    remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
                {
                    audio_mgr_exception(buffer_ent, p_cmd->exc, 1, p_cmd->param);
                }
            }
        }
        break;

    case MEDIA_BUFFER_PRI_ONE_WAY_PLAY:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_PLAY)
                {
                    audio_track_restart(buffer_ent->track_handle);
                }
                media_buffer_reset(buffer_ent);
            }
        }
        break;

    case MEDIA_BUFFER_REMOTE_AUDIO_SYNCED:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                audio_remote_sync_lock(buffer_ent->audio_remote_handle);
                jitter_buffer_asrc_pid_unblock(buffer_ent->jitter_buffer_handle);
            }
        }
        break;

    case MEDIA_BUFFER_REMOTE_LOW_LATENCY_OVERRIDE:
        {
            T_AUDIO_MSG_LOW_LATENCY_OVERRIDE *p_cmd = (T_AUDIO_MSG_LOW_LATENCY_OVERRIDE *)param;

            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                audio_latency_set_dynamic_latency_on(buffer_ent->audio_latency_handle,
                                                     p_cmd->dynamic_latency_on);
                buffer_ent->plc_count = p_cmd->plc_count;
                buffer_ent->latency = p_cmd->latency;
                buffer_ent->latency_override = p_cmd->latency_override;
                buffer_ent->latency_back_up = p_cmd->latency_backup;
            }
        }
        break;

    case MEDIA_BUFFER_REMOTE_DYNAMIC_LATENCY_RESUME:
        {
            audio_latency_resume(buffer_ent->audio_latency_handle);
        }
        break;

    case MEDIA_BUFFER_REMOTE_DYNAMIC_LATENCY_SET:
        {
            T_DYNAMIC_LAT_SET *p_cmd = (T_DYNAMIC_LAT_SET *)param;

            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                audio_latency_dynamic_set(buffer_ent->audio_latency_handle, p_cmd->target_lat_ms,
                                          p_cmd->auto_resume);
            }
        }
        break;

    case MEDIA_BUFFER_REMOTE_DYNAMIC_LATENCY_INC_REQ:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    T_DYNAMIC_LAT_INC_DEC *p_cmd = (T_DYNAMIC_LAT_INC_DEC *)param;

                    if (buffer_ent->mode == AUDIO_STREAM_MODE_LOW_LATENCY ||
                        buffer_ent->mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
                    {
                        if (buffer_ent->latency_fixed == false &&
                            media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_PLAY)
                        {
                            if (buffer_ent->plc_count < p_cmd->previous_total_plc_count + p_cmd->plc_count)
                            {
                                T_DYNAMIC_LAT_INC_DEC cmd;
                                cmd.previous_total_plc_count = buffer_ent->plc_count;
                                cmd.plc_count = p_cmd->previous_total_plc_count + p_cmd->plc_count - buffer_ent->plc_count;
                                cmd.auto_dec = true;
                                if (audio_remote_async_msg_relay(buffer_ent->audio_remote_handle,
                                                                 MEDIA_BUFFER_REMOTE_DYNAMIC_LATENCY_INC,
                                                                 &cmd,
                                                                 sizeof(T_DYNAMIC_LAT_INC_DEC),
                                                                 false) == false)
                                {
                                    audio_latency_low_lat_dynamic_increase(buffer_ent->audio_latency_handle, cmd.plc_count,
                                                                           cmd.auto_dec);
                                }
                            }
                        }
                    }
                }
            }
        }
        break;

    case MEDIA_BUFFER_REMOTE_DYNAMIC_LATENCY_INC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                T_DYNAMIC_LAT_INC_DEC *p_cmd = (T_DYNAMIC_LAT_INC_DEC *)param;

                if (buffer_ent->latency_fixed == false)
                {
                    buffer_ent->plc_count = p_cmd->previous_total_plc_count;
                    audio_latency_low_lat_dynamic_increase(buffer_ent->audio_latency_handle, p_cmd->plc_count,
                                                           p_cmd->auto_dec);
                }
            }
        }
        break;

    case MEDIA_BUFFER_REMOTE_DYNAMIC_LATENCY_DEC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                T_DYNAMIC_LAT_INC_DEC *p_cmd = (T_DYNAMIC_LAT_INC_DEC *)param;

                if (buffer_ent->latency_fixed == false)
                {
                    buffer_ent->plc_count = p_cmd->previous_total_plc_count;
                    audio_latency_low_lat_dynamic_decrease(buffer_ent->audio_latency_handle, p_cmd->plc_count);
                }
            }
        }
        break;

    case MEDIA_BUFFER_REMOTE_ASRC_SYNC_ADJUST:
        {
            T_MEDIA_BUFFER_ASRC_SYNC_ADJUST *p_msg = (T_MEDIA_BUFFER_ASRC_SYNC_ADJUST *)param;

            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT || status == REMOTE_RELAY_STATUS_SYNC_EXPIRED)
            {
                if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_PLAY)
                {
                    T_BT_CLK_REF clk_ref;
                    uint32_t bb_clock_slot;
                    uint32_t bb_clock_rcv;
                    uint16_t bb_clock_us;
                    uint8_t  clk_idx;

                    bb_clock_rcv = p_msg->sync_clk;
                    clk_ref = bt_piconet_clk_get((T_BT_CLK_REF)(p_msg->clk_ref), &clk_idx, &bb_clock_slot,
                                                 &bb_clock_us);

                    if (p_msg->event != JITTER_BUFFER_EVT_ASRC_PID_ADJ_CANCEL)
                    {
                        if (jitter_buffer_asrc_pid_is_block(buffer_ent->jitter_buffer_handle))
                        {
                            AUDIO_PRINT_WARN0("jitter buffer adjust blocks");
                            break;
                        }
                    }

                    if (p_msg->tid != buffer_ent->media_buffer_fst_sync_tid)
                    {
                        AUDIO_PRINT_WARN2("jitter buffer adjust error tid %d %d", p_msg->tid,
                                          buffer_ent->media_buffer_fst_sync_tid);
                        break;
                    }

                    if (clk_ref != BT_CLK_NONE &&
                        bt_clk_compare(bb_clock_rcv, bb_clock_slot + 2 * 2 * 1000 / 625))
                    {
                        jitter_buffer_asrc_ratio_adjust(buffer_ent->jitter_buffer_handle, bb_clock_rcv, p_msg->final_asrc);
                    }
                    else
                    {
                        AUDIO_PRINT_WARN4("JB_clk_err: clk ref %u sync clk %u local %u + margin %u", clk_ref, bb_clock_rcv,
                                          bb_clock_slot, 2 * 2 * 1000 / 625);
                        if (remote_session_role_get() != REMOTE_SESSION_ROLE_PRIMARY ||
                            remote_session_state_get() != REMOTE_SESSION_STATE_DISCONNECTED)
                        {
                            audio_mgr_exception(buffer_ent, MEDIA_BUFFER_JBC_CLK_ERR, 0, 0);
                        }

                    }
                }
                else if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_SYNC ||
                         audio_remote_get_state(buffer_ent->audio_remote_handle) == AUDIO_REMOTE_STATE_SEAMLESS_JOIN)
                {
                    audio_mgr_exception(buffer_ent, MEDIA_BUFFER_JBC_CLK_ERR, 0, 0);
                }
            }
            else if (status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED)
            {
                audio_mgr_exception(buffer_ent, MEDIA_BUFFER_ASRC_ADJ_OVERTIME, 0, 0);
            }
        }
        break;

    case MEDIA_BUFFER_REMOTE_LOW_LAT_ADJUST:
        {
            T_JITTER_BUFFER_EVT_LOW_LAT_ADJ *p_msg = (T_JITTER_BUFFER_EVT_LOW_LAT_ADJ *)param;

            if (status == REMOTE_RELAY_STATUS_SYNC_RCVD)
            {
                AUDIO_PRINT_TRACE5("gaming mode asrc: type %d restore clk 0x%x sync clk 0x%x acc %d dec_num %d",
                                   p_msg->policy, p_msg->restore_clk,
                                   p_msg->sync_adj_clk, p_msg->accelerate, p_msg->dec_frame_num);
            }
            else if (status == REMOTE_RELAY_STATUS_SYNC_TOUT || status == REMOTE_RELAY_STATUS_SYNC_EXPIRED)
            {
                T_BT_CLK_REF clk_ref;
                uint32_t bb_clock_slot;
                uint32_t bb_clock_rcv;
                uint16_t bb_clock_us;
                uint8_t  clk_idx;

                if (
                    media_buffer_state_get(buffer_ent) != MEDIA_BUFFER_STATE_PLAY ||
                    (buffer_ent->mode != AUDIO_STREAM_MODE_LOW_LATENCY &&
                     buffer_ent->mode != AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
                )
                {
                    break;
                }

                if (p_msg->policy == LATENCY_KEEPER_POLICY_BY_ASRC ||
                    p_msg->policy == LATENCY_KEEPER_POLICY_BY_BOTH)
                {
                    bb_clock_rcv = p_msg->sync_adj_clk;
                    clk_ref = bt_piconet_clk_get(BT_CLK_SNIFFING, &clk_idx, &bb_clock_slot, &bb_clock_us);

                    if (clk_ref != BT_CLK_NONE &&
                        bt_clk_compare(bb_clock_rcv, bb_clock_slot + 5 * 2 * 1000 / 625))
                    {
                        uint32_t restore_us;

                        restore_us = (((p_msg->restore_clk << 4) - (bb_clock_slot << 4)) >> 4) * 625 / 2;

                        if (restore_us < 100000)
                        {
                            AUDIO_PRINT_WARN4("JB_clk_err2: gaming mode asrc clk ref %u sync clk %u local %u + margin %u",
                                              clk_ref, bb_clock_rcv,
                                              bb_clock_slot, 5 * 2 * 1000 / 625);
                            audio_mgr_exception(buffer_ent, MEDIA_BUFFER_ASRC_ADJ_OVERTIME, 0, 0);
                            break;
                        }
                        jitter_buffer_low_latency_adjust_latency(buffer_ent->jitter_buffer_handle, p_msg);
                    }
                    else
                    {
                        AUDIO_PRINT_WARN4("JB_clk_err: gaming mode asrc clk ref %u sync clk %u local %u + margin %u",
                                          clk_ref, bb_clock_rcv,
                                          bb_clock_slot, 5 * 2 * 1000 / 625);
                        audio_mgr_exception(buffer_ent, MEDIA_BUFFER_ASRC_ADJ_OVERTIME, 0, 0);
                    }
                }
            }
            else if (status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED)
            {
                audio_mgr_exception(buffer_ent, MEDIA_BUFFER_JBC_CLK_ERR, 0, 0);

            }
        }
        break;

    case MEDIA_BUFFER_REMOTE_ULTRA_LOW_LATENCY_PLC_REQ:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                T_MEDIA_DATA_HDR *p_media_packet;

                if (media_buffer_backup_downstream(buffer_ent))
                {
                    p_media_packet = buffer_ent->last_packet_ptr;
                    audio_latency_ultra_low_latency_plc_req(buffer_ent->audio_latency_handle,
                                                            p_media_packet->frame_number);
                    buffer_ent->buffer_sync_trigger_cnt = 0;
                    audio_remote_buffer_sync_req(buffer_ent->audio_remote_handle);
                }
            }
        }
        break;

    default:
        break;
    }

}

void media_buffer_audio_remote_evt_cback(T_AUDIO_REMOTE_EVT  event,
                                         void               *param,
                                         void               *owner)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent = (T_MEDIA_BUFFER_ENTITY *)owner;

    if (event != AUDIO_REMOTE_EVENT_BUFFER_SYNC)
    {
        AUDIO_PRINT_INFO1("media_buffer_audio_remote_evt_cback: event 0x%04x", event);
    }

    switch (event)
    {
    case AUDIO_REMOTE_EVENT_BUFFER_SYNC:
        {
            bool rec_found = *(bool *)param;
            if (rec_found)
            {
                if (buffer_ent->mode == AUDIO_STREAM_MODE_LOW_LATENCY ||
                    buffer_ent->mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
                {
                    if (audio_remote_is_buffer_unsync(buffer_ent->audio_remote_handle))
                    {
                        audio_remote_buffer_PLC(buffer_ent->audio_remote_handle);
                    }
                    media_buffer_downstream_dsp(buffer_ent);
                }
                else
                {
                    media_buffer_check_miss_ack(buffer_ent);
                }
            }
            else
            {
                if (audio_remote_get_force_join() == false)
                {
                    audio_mgr_exception(buffer_ent, MEDIA_BUFFER_SYNC_ERR, 0, 0);
                }
            }
        }
        break;

    case AUDIO_REMOTE_EVENT_ALIGNED:
        {
            if (media_buffer_ds_pkt_cnt(buffer_ent) == 0)
            {
                AUDIO_PRINT_ERROR0("audio_buffer_prequeue_state: no data, unknow fatal error");
                break;
            }
            if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
            {
                uint16_t play_time;
                bool dynamic_latency_on = false;

                play_time = media_buffer_playtime_calculate(buffer_ent);
                audio_latency_get_dynamic_latency_on(buffer_ent->audio_latency_handle,
                                                     &dynamic_latency_on);
                audio_remote_appoint_playtime(buffer_ent->audio_remote_handle, play_time, dynamic_latency_on);
                media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_SYNC);
            }
            else if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
            {
                //sys_timer_stop(&audio_db->one_way_play_a2dp_timer_handle);
            }
        }
        break;

    case AUDIO_REMOTE_EVENT_SYNC_PLAY_START:
        {
            if (param != NULL)
            {
                T_AUDIO_REMOTE_LATENCY_INFO *msg = (T_AUDIO_REMOTE_LATENCY_INFO *) param;

                audio_latency_set_dynamic_latency_on(buffer_ent->audio_latency_handle,
                                                     msg->dynamic_latency_on);
                buffer_ent->plc_count = msg->plc_count;
                buffer_ent->latency = msg->latency;
                buffer_ent->latency_override = msg->latency_override;
                buffer_ent->latency_back_up = msg->latency_back_up;
            }

            if (buffer_ent->mode == AUDIO_STREAM_MODE_LOW_LATENCY)
            {
                for (uint8_t i = media_buffer_ds_pkt_cnt(buffer_ent) ; i > 0; i--)
                {
                    media_buffer_downstream_dsp(buffer_ent);
                }
            }
            else if (buffer_ent->mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
            {
                media_buffer_downstream_dsp(buffer_ent);
            }
            else
            {
                media_buffer_leading_downstream(buffer_ent);
            }

            media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_PLAY);
            jitter_buffer_asrc_pid_block(buffer_ent->jitter_buffer_handle);//wait for slave_sync flag
            audio_mgr_dispatch(AUDIO_MSG_BUFFER_STATE_PLAYING, NULL);

        }
        break;

    case AUDIO_REMOTE_EVENT_SYNC_PLAY_RSV:
        {
            uint8_t tid = *(uint8_t *)param;

            if (buffer_ent->media_buffer_fst_sync_tid == tid)
            {
                if (buffer_ent->solo_play_timer != NULL)
                {
                    sys_timer_delete(buffer_ent->solo_play_timer);
                    buffer_ent->solo_play_timer = NULL;
                }
                media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_SYNC);
            }
        }
        break;

    default:
        break;
    }
}

void media_buffer_lat_evt_cback(T_AUDIO_LATENCY_EVT  event,
                                void                *param,
                                void                *owner)
{
    AUDIO_PRINT_TRACE1("media_buffer_lat_evt_cback: event 0x%04x", event);
    T_MEDIA_BUFFER_ENTITY *buffer_ent = (T_MEDIA_BUFFER_ENTITY *)owner;

    switch (event)
    {
    case AUDIO_LATENCY_EVT_ULTRA_LOW_LAT_PLC_REQ:
        {
            if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
            {
                audio_remote_async_msg_relay(buffer_ent->audio_remote_handle,
                                             MEDIA_BUFFER_REMOTE_ULTRA_LOW_LATENCY_PLC_REQ,
                                             NULL,
                                             0,
                                             false);
            }
            else
            {
                T_MEDIA_DATA_HDR *p_media_packet;

                if (media_buffer_backup_downstream(buffer_ent))
                {
                    p_media_packet = buffer_ent->last_packet_ptr;
                    audio_latency_ultra_low_latency_plc_req(buffer_ent->audio_latency_handle,
                                                            p_media_packet->frame_number);
                    buffer_ent->buffer_sync_trigger_cnt = 0;
                    audio_remote_buffer_sync_req(buffer_ent->audio_remote_handle);
                }
            }
        }
        break;

    case AUDIO_LATENCY_EVT_PLC_FIX:
        {
            uint8_t plc_frame_num = *(uint8_t *)param;
            if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_PLAY)
            {
                audio_remote_set_frame_diff(buffer_ent->audio_remote_handle,
                                            audio_remote_get_frame_diff(buffer_ent->audio_remote_handle) - plc_frame_num);
                audio_remote_buffer_PLC(buffer_ent->audio_remote_handle);
            }
        }
        break;

    case AUDIO_LATENCY_EVT_MAX_LAT_PLC:
        {
            audio_mgr_exception(buffer_ent, MEDIA_BUFFER_DSP_PLC_TOO_MUCH, 0, 0);
        }
        break;

    case AUDIO_LATENCY_EVT_LOW_LAT_DECREASE_REQ:
        {
            T_DYNAMIC_LAT_INC_DEC cmd;
            uint8_t plc_count = *(int8_t *)param;

            cmd.previous_total_plc_count = buffer_ent->plc_count;
            cmd.plc_count = plc_count;
            if (remote_session_role_get() != REMOTE_SESSION_ROLE_SECONDARY)
            {
                if (audio_remote_async_msg_relay(buffer_ent->audio_remote_handle,
                                                 MEDIA_BUFFER_REMOTE_DYNAMIC_LATENCY_DEC,
                                                 &cmd,
                                                 sizeof(T_DYNAMIC_LAT_INC_DEC),
                                                 false) == false)
                {
                    audio_latency_low_lat_dynamic_decrease(buffer_ent->audio_latency_handle, plc_count);
                }
            }
        }
        break;

    case AUDIO_LATENCY_EVT_LOW_LAT_INCREASE_REQ:
        {
            T_DYNAMIC_LAT_INC_DEC cmd;
            uint8_t plc_count = *(int8_t *)param;

            cmd.previous_total_plc_count = buffer_ent->plc_count;
            cmd.plc_count = plc_count;
            cmd.auto_dec = false;
            if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
            {
                if (audio_remote_async_msg_relay(buffer_ent->audio_remote_handle,
                                                 MEDIA_BUFFER_REMOTE_DYNAMIC_LATENCY_INC,
                                                 &cmd,
                                                 sizeof(T_DYNAMIC_LAT_INC_DEC),
                                                 false) == false)
                {
                    audio_latency_low_lat_dynamic_increase(buffer_ent->audio_latency_handle, plc_count, cmd.auto_dec);
                }
            }
            else
            {
                audio_remote_async_msg_relay(buffer_ent->audio_remote_handle,
                                             MEDIA_BUFFER_REMOTE_DYNAMIC_LATENCY_INC_REQ,
                                             &cmd,
                                             sizeof(T_DYNAMIC_LAT_INC_DEC),
                                             false);
            }
        }
        break;

    case AUDIO_LATENCY_EVT_LAT_DECREASED:
        {
            uint8_t plc_frame_num = *(uint8_t *)param;
            if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_PLAY)
            {
                if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    buffer_ent->ignore_latency_report++;
                }
                audio_remote_set_frame_diff(buffer_ent->audio_remote_handle,
                                            audio_remote_get_frame_diff(buffer_ent->audio_remote_handle) - plc_frame_num);
                audio_remote_buffer_PLC(buffer_ent->audio_remote_handle);
            }
        }

    case AUDIO_LATENCY_EVT_LAT_INCREASED:
        {
            if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_PLAY)
            {
                if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    buffer_ent->ignore_latency_report++;
                    buffer_ent->buffer_sync_trigger_cnt = 0;
                    audio_remote_buffer_sync_req(buffer_ent->audio_remote_handle);
                }
            }
            else
            {
                if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    buffer_ent->start_decode_frame_cnt = media_buffer_cacu_frame_num(buffer_ent->latency,
                                                                                     &(buffer_ent->format_info));
                }
                else
                {
                    media_buffer_reset(buffer_ent);
                }
            }
        }
        break;

    default:
        break;
    }
}

void media_buffer_interval_eval_evt_cback(T_AUDIO_INTERVAL_EVAL_EVT  event,
                                          void                      *param,
                                          void                      *owner)
{
    AUDIO_PRINT_TRACE1("media_buffer_interval_eval_evt_cback: event 0x%04x", event);
    T_MEDIA_BUFFER_ENTITY *buffer_ent = (T_MEDIA_BUFFER_ENTITY *)owner;

    switch (event)
    {
    case AUDIO_INTERVAL_EVAL_EVT_UNSTABLE:
        {
            if (remote_session_role_get() != REMOTE_SESSION_ROLE_SECONDARY)
            {
                audio_latency_handle_inverval_unstable_evt(buffer_ent->audio_latency_handle);
            }
        }
        break;

    case AUDIO_INTERVAL_EVAL_EVT_STABLE:
        {
            if (remote_session_role_get() != REMOTE_SESSION_ROLE_SECONDARY)
            {
                if (buffer_ent != NULL)
                {
                    audio_latency_handle_inverval_stable_evt(buffer_ent->audio_latency_handle,
                                                             buffer_ent->format_info.type);
                }
            }
        }
        break;

    default:
        break;
    }
}

void media_buffer_jb_evt_cback(T_JITTER_BUFFER_EVT  event,
                               void                *param,
                               void                *owner)
{
    AUDIO_PRINT_TRACE1("media_buffer_jb_evt_cback: event 0x%04x", event);
    T_MEDIA_BUFFER_ENTITY *buffer_ent = (T_MEDIA_BUFFER_ENTITY *)owner;

    switch (event)
    {
    case JITTER_BUFFER_EVT_LOW_LAT_ASRC_RESTORE_CANCEL:
        {
            T_BT_CLK_REF clk_ref;
            uint8_t  clk_idx;
            uint32_t bb_clock_slot;
            uint16_t bb_clock_us;

            if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
            {
                break;
            }

            if (buffer_ent->mode == AUDIO_STREAM_MODE_LOW_LATENCY ||
                buffer_ent->mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
            {
                if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_PLAY)
                {
                    clk_ref = bt_piconet_clk_get(BT_CLK_SNIFFING, &clk_idx, &bb_clock_slot, &bb_clock_us);

                    if (clk_ref == BT_CLK_NONE)
                    {
                        break;
                    }

                    bb_clock_slot += (250 * 2 * 1000 / 625); // ms to bt clk
                    bb_clock_slot &= 0x0fffffff;

                    if (audio_remote_async_msg_relay(buffer_ent->audio_remote_handle,
                                                     MEDIA_BUFFER_REMOTE_LOW_LAT_ADJUST_RESTORE,
                                                     &bb_clock_slot,
                                                     sizeof(uint32_t),
                                                     false) == false)
                    {
                        jitter_buffer_low_latency_asrc_restore(buffer_ent->jitter_buffer_handle, 0xffffffff);
                    }
                }
            }
        }
        break;

    case JITTER_BUFFER_EVT_LOW_LAT_ASRC_RESTORE_REQ:
        {
            uint32_t restore_clk = *(uint32_t *)param;
            T_BT_CLK_REF clk_ref;
            uint32_t bb_clock_slot;
            uint16_t bb_clock_us;
            uint8_t  clk_idx;

            if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
            {
                break;
            }

            if (buffer_ent->mode == AUDIO_STREAM_MODE_LOW_LATENCY ||
                buffer_ent->mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
            {
                if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_PLAY)
                {
                    clk_ref = bt_piconet_clk_get(BT_CLK_SNIFFING, &clk_idx, &bb_clock_slot, &bb_clock_us);
                    if (clk_ref != BT_CLK_NONE &&
                        bt_clk_compare(restore_clk, bb_clock_slot + 5 * 2 * 1000 / 625))
                    {
                        if (audio_remote_async_msg_relay(buffer_ent->audio_remote_handle,
                                                         MEDIA_BUFFER_REMOTE_LOW_LAT_ADJUST_RESTORE,
                                                         &restore_clk,
                                                         sizeof(uint32_t),
                                                         false) == false)
                        {
                            jitter_buffer_low_latency_asrc_restore(buffer_ent->jitter_buffer_handle, 0xffffffff);
                        }
                    }
                    else
                    {
                        AUDIO_PRINT_WARN3("asrc_restore_error: gaming mode asrc clk ref %u sync clk %u local %u",
                                          clk_ref, restore_clk, bb_clock_slot);
                        audio_mgr_exception(buffer_ent, MEDIA_BUFFER_ASRC_ADJ_OVERTIME, 0, 0);
                    }
                }
            }
        }
        break;

    case JITTER_BUFFER_EVT_LAT_ADJ_PLC_REQ:
        {
            uint8_t dec_frame_num = *(uint8_t *)param;
            audio_remote_set_frame_diff(buffer_ent->audio_remote_handle,
                                        audio_remote_get_frame_diff(buffer_ent->audio_remote_handle) - dec_frame_num);
            audio_remote_buffer_PLC(buffer_ent->audio_remote_handle);
        }
        break;

    case JITTER_BUFFER_EVT_ASRC_ADJ_REQ:
        {
            T_JITTER_BUFFER_EVT_LOW_LAT_ADJ_REQ *msg = (T_JITTER_BUFFER_EVT_LOW_LAT_ADJ_REQ *)param;
            T_JITTER_BUFFER_EVT_LOW_LAT_ADJ cmd;
            T_BT_CLK_REF clk_ref;
            uint8_t  clk_idx;
            uint32_t bb_clock_slot;
            uint16_t bb_clock_us;
            uint32_t duration = 0;

            cmd.policy = media_buffer_db.latency_keeper_policy;
            cmd.dec_frame_num = 0;
            cmd.restore_clk = 0;
            cmd.sync_adj_clk = 0;
            cmd.duration = 0;
            cmd.accelerate = msg->accelerate;

            if (media_buffer_db.latency_keeper_policy == LATENCY_KEEPER_POLICY_BY_ASRC)
            {
                clk_ref = bt_piconet_clk_get(BT_CLK_SNIFFING, &clk_idx, &bb_clock_slot, &bb_clock_us);

                if (clk_ref == BT_CLK_NONE)
                {
                    break;
                }

                bb_clock_slot += (250 * 2 * 1000 / 625); // ms to bt clk
                bb_clock_slot &= 0x0fffffff;

                cmd.sync_adj_clk = bb_clock_slot;

                cmd.ratio = msg->diff_ms * 50;
                if (cmd.ratio > LOW_LATENCY_ASRC_ADJ_SPEED_PPM_LIM)
                {
                    cmd.ratio = LOW_LATENCY_ASRC_ADJ_SPEED_PPM_LIM;
                }

                duration = msg->diff_ms * 1000 / cmd.ratio * 1000;
                if (duration > LOW_LATENCY_ASRC_ADJ_DURATION_LIM)
                {
                    duration = LOW_LATENCY_ASRC_ADJ_DURATION_LIM;
                }
                bb_clock_slot += (duration * 2 * 1000 / 625);
                bb_clock_slot &= 0x0fffffff;
                cmd.restore_clk = bb_clock_slot;
                cmd.duration = duration;

            }
            else if (media_buffer_db.latency_keeper_policy == LATENCY_KEEPER_POLICY_BY_PLC)
            {
                if (msg->accelerate)
                {
                    uint16_t frame_duration = media_buffer_cacu_frames_duration(1, &buffer_ent->format_info);
                    if (frame_duration != 0)
                    {
                        cmd.dec_frame_num = msg->diff_ms / frame_duration;
                    }
                    if (cmd.dec_frame_num == 0)
                    {
                        break;
                    }
                }
            }
            else
            {
                uint16_t frame_duration = media_buffer_cacu_frames_duration(1, &buffer_ent->format_info);
                uint16_t diff_ms;

                if (frame_duration != 0)
                {
                    if (msg->accelerate)
                    {
                        if (msg->diff_ms > 20)
                        {
                            cmd.dec_frame_num = (msg->diff_ms - 20) / frame_duration;
                        }
                    }
                }

                diff_ms = msg->diff_ms - cmd.dec_frame_num * frame_duration;

                clk_ref = bt_piconet_clk_get(BT_CLK_SNIFFING, &clk_idx, &bb_clock_slot, &bb_clock_us);

                if (clk_ref == BT_CLK_NONE)
                {
                    return;
                }

                bb_clock_slot += (250 * 2 * 1000 / 625); // ms to bt clk
                bb_clock_slot &= 0x0fffffff;

                cmd.sync_adj_clk = bb_clock_slot;

                cmd.ratio = diff_ms * 50;
                if (cmd.ratio > LOW_LATENCY_ASRC_ADJ_SPEED_PPM_LIM)
                {
                    cmd.ratio = LOW_LATENCY_ASRC_ADJ_SPEED_PPM_LIM;
                }

                duration = diff_ms * 1000 / cmd.ratio * 1000;
                if (duration > LOW_LATENCY_ASRC_ADJ_DURATION_LIM)
                {
                    duration = LOW_LATENCY_ASRC_ADJ_DURATION_LIM;
                }
                bb_clock_slot += (duration * 2 * 1000 / 625);
                bb_clock_slot &= 0x0fffffff;
                cmd.restore_clk = bb_clock_slot;
                cmd.duration = duration;

            }
            AUDIO_PRINT_TRACE7("gaming mode asrc: policy %d diff %d restore clk 0x%x sync clk 0x%x duration %d acc %d dec_num %d",
                               media_buffer_db.latency_keeper_policy, msg->diff_ms, cmd.restore_clk, cmd.sync_adj_clk,
                               duration, msg->accelerate, cmd.dec_frame_num);


            if (audio_remote_sync_msg_relay(buffer_ent->audio_remote_handle,
                                            MEDIA_BUFFER_REMOTE_LOW_LAT_ADJUST,
                                            &cmd,
                                            sizeof(T_JITTER_BUFFER_EVT_LOW_LAT_ADJ),
                                            REMOTE_TIMER_HIGH_PRECISION,
                                            200,
                                            false) == false)
            {
                jitter_buffer_low_latency_adjust_latency(buffer_ent->jitter_buffer_handle, &cmd);
            }
        }
        break;

    case JITTER_BUFFER_EVT_ASRC_PID_ADJ_OPEN:
    case JITTER_BUFFER_EVT_ASRC_PID_ADJ_CLOSE:
    case JITTER_BUFFER_EVT_ASRC_PID_ADJ_CANCEL:
    case JITTER_BUFFER_EVT_ASRC_PID_ADJ:
    case JITTER_BUFFER_EVT_ASRC_LONGTERM_ADJ_REQ:
        {
            T_JITTER_BUFFER_ASRC_ADJ *msg = (T_JITTER_BUFFER_ASRC_ADJ *)param;
            T_MEDIA_BUFFER_ASRC_SYNC_ADJUST cmd;
            T_BT_CLK_REF clk_ref;
            uint32_t bb_clock_slot;
            uint16_t bb_clock_us;
            uint8_t  clk_idx;

            if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
            {
                break;
            }

            buffer_ent->ignore_latency_report++;

            if (remote_session_state_get() != REMOTE_SESSION_STATE_CONNECTED)
            {
                jitter_buffer_asrc_ratio_adjust(buffer_ent->jitter_buffer_handle, 0xffffffff, msg->asrc_ppm);
                break;
            }
            if (audio_remote_get_state(buffer_ent->audio_remote_handle) != AUDIO_REMOTE_STATE_SYNCED)
            {
                jitter_buffer_asrc_ratio_adjust(buffer_ent->jitter_buffer_handle, 0xffffffff, msg->asrc_ppm);
                break;
            }

            clk_ref = bt_piconet_clk_get(BT_CLK_NONE, &clk_idx, &bb_clock_slot, &bb_clock_us);

            if (clk_ref == BT_CLK_NONE)
            {
                bb_clock_slot = 0xffffffff;
            }
            else
            {
                bb_clock_slot += (250 * 2 * 1000 / 625); // ms to bt clk
                bb_clock_slot &= 0x0fffffff;
            }

            cmd.sync_clk = bb_clock_slot;
            cmd.clk_ref = clk_ref;
            cmd.final_asrc = msg->asrc_ppm;
            cmd.tid = buffer_ent->media_buffer_fst_sync_tid;
            cmd.event = event;
            if (audio_remote_sync_msg_relay(buffer_ent->audio_remote_handle,
                                            MEDIA_BUFFER_REMOTE_ASRC_SYNC_ADJUST,
                                            &cmd,
                                            sizeof(T_MEDIA_BUFFER_ASRC_SYNC_ADJUST),
                                            REMOTE_TIMER_HIGH_PRECISION,
                                            150,
                                            false) == false)
            {
                jitter_buffer_asrc_ratio_adjust(buffer_ent->jitter_buffer_handle, 0xffffffff, msg->asrc_ppm);
            }
        }
        break;

    case JITTER_BUFFER_EVT_ASRC_ADJ:
        {
            T_JITTER_BUFFER_ASRC_ADJ *msg = (T_JITTER_BUFFER_ASRC_ADJ *)param;
            audio_path_asrc_set(buffer_ent->path_handle, 1, msg->asrc_ppm, msg->timestamp);
        }
        break;

    default:
        break;
    }
}

void media_buffer_proxy_fifo_report_config(T_MEDIA_BUFFER_PROXY proxy)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;

    if (proxy == NULL)
    {
        return;
    }

    buffer_ent = (T_MEDIA_BUFFER_ENTITY *)proxy;
    if (buffer_ent->stream_type == AUDIO_STREAM_TYPE_PLAYBACK &&
        buffer_ent->mode != AUDIO_STREAM_MODE_DIRECT)
    {
        audio_path_fifo_report(buffer_ent->path_handle, LATENCY_REPORT_PERIOD_MS);
    }

}

uint16_t media_buffer_playtime_calculate(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    uint16_t play_time_ms;
    uint16_t total_frames;
    uint16_t play_margin_ms;

    play_margin_ms = audio_remote_play_margin_get(buffer_ent->format_info.type);
    total_frames = media_buffer_ds_frame_cnt(buffer_ent);

    if (buffer_ent->start_decode_frame_cnt >= total_frames)
    {
        uint16_t diff;

        diff = buffer_ent->start_decode_frame_cnt - total_frames;
        play_time_ms = diff * buffer_ent->latency / buffer_ent->start_decode_frame_cnt;

        if (play_time_ms <= play_margin_ms)
        {
            play_time_ms = play_margin_ms;
        }
    }
    else
    {
        play_time_ms = play_margin_ms;
    }

    AUDIO_PRINT_INFO4("media_buffer_playtime_calculate: start frames %u, total frames %u, lantency %u, play time %u",
                      buffer_ent->start_decode_frame_cnt, total_frames, buffer_ent->latency, play_time_ms);
    return play_time_ms;
}

void media_buffer_dynamic_latency_set(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                                      uint16_t               latency,
                                      bool                   auto_resume)
{
    T_DYNAMIC_LAT_SET cmd;

    if (buffer_ent == NULL)
    {
        return;
    }

    cmd.target_lat_ms = latency;
    cmd.previous_lat_ms = buffer_ent->latency_back_up;
    cmd.auto_resume = auto_resume;

    audio_remote_async_msg_relay(buffer_ent->audio_remote_handle,
                                 MEDIA_BUFFER_REMOTE_DYNAMIC_LATENCY_SET,
                                 &cmd,
                                 sizeof(T_DYNAMIC_LAT_SET),
                                 false);

    audio_latency_dynamic_set(buffer_ent->audio_latency_handle, latency, auto_resume);
}

void media_buffer_dynamic_latency_resume(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    if (buffer_ent == NULL)
    {
        return;
    }

    audio_remote_async_msg_relay(buffer_ent->audio_remote_handle,
                                 MEDIA_BUFFER_REMOTE_DYNAMIC_LATENCY_RESUME,
                                 NULL,
                                 0,
                                 false);
    audio_latency_resume(buffer_ent->audio_latency_handle);
}

void media_buffer_handle_plc_event(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                                   uint16_t               plc_total_frames,
                                   uint16_t               plc_frame_counter,
                                   uint16_t               plc_local_seq,
                                   uint8_t                plc_frame_num)
{
    uint16_t current_plc_count = buffer_ent->plc_count;
    uint16_t ds_local_seq_last;

    ds_local_seq_last = buffer_ent->ds_local_seq_last;
    if (media_buffer_cacu_frames_duration(plc_total_frames, &buffer_ent->format_info) > 90)
    {
        if (!buffer_ent->latency_fixed)
        {
            if (remote_session_role_get() != REMOTE_SESSION_ROLE_SECONDARY &&
                current_plc_count <= buffer_ent->max_latency_plc_count)
            {
                T_DYNAMIC_LAT_INC_DEC cmd;
                cmd.previous_total_plc_count = current_plc_count;
                cmd.plc_count = plc_frame_num;
                cmd.auto_dec = true;
                if (audio_remote_async_msg_relay(buffer_ent->audio_remote_handle,
                                                 MEDIA_BUFFER_REMOTE_DYNAMIC_LATENCY_INC,
                                                 &cmd,
                                                 sizeof(T_DYNAMIC_LAT_INC_DEC),
                                                 false) == false)
                {
                    audio_latency_low_lat_dynamic_increase(buffer_ent->audio_latency_handle, plc_frame_num,
                                                           cmd.auto_dec);
                }
            }
        }

        audio_mgr_exception(buffer_ent, MEDIA_BUFFER_DSP_PLC_TOO_MUCH, 0, 0);
        return;
    }

    if ((uint16_t)(buffer_ent->ds_local_seq_last + 1) == plc_local_seq)
    {

        T_MEDIA_DATA_HDR media_packet;
        T_MEDIA_DATA_HDR *p_media_packet;

        media_packet.frame_counter = plc_frame_counter;
        media_packet.local_seq_number = plc_local_seq;
        media_packet.frame_number = plc_frame_num;

        if (buffer_ent->last_packet_ptr == NULL)
        {
            return;
        }
        else
        {
            p_media_packet = buffer_ent->last_packet_ptr;
        }

        media_packet.upper_seq_number = p_media_packet->upper_seq_number;
        buffer_ent->ds_local_seq_last++;
        buffer_ent->ds_frame_counter_last = media_packet.frame_counter;
        audio_remote_record_push(buffer_ent->audio_remote_handle, &media_packet);

    }
    else if (buffer_ent->ds_local_seq_last == plc_local_seq)
    {
        int32_t diff;

        diff = audio_remote_record_update(buffer_ent->audio_remote_handle,
                                          plc_local_seq,
                                          plc_frame_counter,
                                          plc_frame_num);

        buffer_ent->ds_frame_counter_last += diff;
        if (buffer_ent->latency_fixed)
        {
            audio_remote_set_frame_diff(buffer_ent->audio_remote_handle,
                                        audio_remote_get_frame_diff(buffer_ent->audio_remote_handle) - diff);
        }
    }
    else
    {
        int32_t diff;

        diff = audio_remote_record_update(buffer_ent->audio_remote_handle,
                                          plc_local_seq,
                                          plc_frame_counter,
                                          plc_frame_num);
        if (diff != 0)
        {
            audio_mgr_exception(buffer_ent, MEDIA_BUFFER_DSP_PLC_TOO_LATE, 0, diff << 16);
            return;
        }
    }

    audio_latency_handle_plc_event(buffer_ent->audio_latency_handle, ds_local_seq_last,
                                   plc_total_frames,
                                   plc_frame_counter, plc_local_seq, plc_frame_num);
}

void media_buffer_handle_sync_lock(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
    {
        audio_remote_async_msg_relay(buffer_ent->audio_remote_handle,
                                     MEDIA_BUFFER_REMOTE_AUDIO_SYNCED,
                                     NULL,
                                     0,
                                     false);
        audio_remote_sync_lock(buffer_ent->audio_remote_handle);
        jitter_buffer_asrc_pid_unblock(buffer_ent->jitter_buffer_handle);
    }

    buffer_ent->local_audio_sync = true;
}

void media_buffer_solo_play_timer_set(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    if (buffer_ent->solo_play_timer != NULL)
    {
        sys_timer_delete(buffer_ent->solo_play_timer);
        buffer_ent->solo_play_timer = NULL;
    }

    buffer_ent->solo_play_timer = sys_timer_create("solo_play_timer",
                                                   SYS_TIMER_TYPE_LOW_PRECISION,
                                                   (uint32_t)buffer_ent,
                                                   SOLO_PLAY_TIMER_TIMEOUT,
                                                   false,
                                                   media_buffer_timeout_cback);
    if (buffer_ent->solo_play_timer != NULL)
    {
        sys_timer_start(buffer_ent->solo_play_timer);
    }
}

void media_buffer_relay_exception(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                                  uint8_t               *cmd)
{
    audio_remote_async_msg_relay(buffer_ent->audio_remote_handle,
                                 MEDIA_BUFFER_REMOTE_RPT_EXCEPTION,
                                 cmd,
                                 sizeof(T_AUDIO_EXC_CMD),
                                 false);
}

void media_buffer_query_sec_path_state(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
    {
        audio_remote_async_msg_relay(buffer_ent->audio_remote_handle,
                                     MEDIA_BUFFER_REMOTE_QUERY_PATH_STATE,
                                     NULL,
                                     0,
                                     false);
    }
}

void media_buffer_reset_relay(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    audio_remote_async_msg_relay(buffer_ent->audio_remote_handle,
                                 MEDIA_BUFFER_REMOTE_PRI_RESET,
                                 NULL,
                                 0,
                                 false);
}

void media_buffer_proxy_inteval_evaluator_update(T_MEDIA_BUFFER_PROXY proxy,
                                                 uint16_t             seq,
                                                 uint32_t             clk)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;
    buffer_ent = (T_MEDIA_BUFFER_ENTITY *)proxy;
    if (buffer_ent->audio_interval_eval_handle)
    {
        audio_inteval_evaluator_update(buffer_ent->audio_interval_eval_handle, seq, clk);
    }
}

bool media_buffer_proxy_inteval_evaluator_get_var(T_MEDIA_BUFFER_PROXY  proxy,
                                                  float                *var)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;
    buffer_ent = (T_MEDIA_BUFFER_ENTITY *)proxy;
    if (buffer_ent->audio_interval_eval_handle)
    {
        return audio_inteval_evaluator_get_var(buffer_ent->audio_interval_eval_handle, var);
    }
    else
    {
        return false;
    }
}

void media_buffer_set_max_plc_count(uint8_t plc_count)
{
    if (plc_count >= 1 && plc_count <= 7)
    {
        media_buffer_db.max_plc_count = plc_count;
    }
}

uint8_t media_buffer_get_max_plc_count(void)
{
    return media_buffer_db.max_plc_count;
}

void *media_buffer_malloc(uint16_t buf_size)
{
    return media_buffer_get(media_buffer_db.playback_pool_handle, buf_size);
}

bool media_buffer_free(void *p_buf)
{
    return media_buffer_put(media_buffer_db.playback_pool_handle, p_buf);
}
