/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "os_sync.h"
#include "trace.h"
#include "audio_remote.h"
#include "jitter_buffer.h"
#include "media_buffer.h"

/* TODO Remove Start */
#include "audio_path.h"
#include "bt_mgr.h"
extern bool bt_clk_compare(uint32_t bt_clk_a, uint32_t bt_clk_b);
/* TODO Remove End */

#define AUDIO_REMOTE_RECORD_SIZE        20
#define AUDIO_REMOTE_JOINING_RETRY_CNT  5
#define AUDIO_REMOTE_SYNC_RELAY_DURATION 40

/*
 * These values are decided by sync timer timeout scheduling delay, audio path transmission time,
 * decoding time, system MCPS and signal processing time.
 *
 * 1. Timer timeout scheduling delay                  -- 2ms
 * 2. Audio path transmission time                    -- 1ms
 * 3. Decoding time                                   -- (0-4.5ms/channel) * channel_num
 * 4. Signal processing time                          -- 2ms
 */
static const uint8_t sync_play_margin_table[] =
{
    5,       /* AUDIO_FORMAT_TYPE_PCM: 0 */
    5,       /* AUDIO_FORMAT_TYPE_CVSD: 1 */
    5,       /* AUDIO_FORMAT_TYPE_MSBC: 2 */
    10,      /* AUDIO_FORMAT_TYPE_SBC: 3 */
    14,      /* AUDIO_FORMAT_TYPE_AAC: 4 */
    10,      /* AUDIO_FORMAT_TYPE_OPUS: 5 */
    14,      /* AUDIO_FORMAT_TYPE_FLAC: 6 */
    14,      /* AUDIO_FORMAT_TYPE_MP3: 7 */
    10,      /* AUDIO_FORMAT_TYPE_LC3: 8 */
    12,      /* AUDIO_FORMAT_TYPE_LDAC: 9 */
    12,      /* AUDIO_FORMAT_TYPE_LHDC: 10 */
    0,       /* AUDIO_FORMAT_TYPE_G729: 11 */
    10,      /* AUDIO_FORMAT_TYPE_LC3PLUS: 12 */
};

typedef struct
{
    uint16_t msg_id;
    uint16_t padding;
    uint8_t msg[0];
} T_AUDIO_REMOTE_BUFFER_MSG_RELAY;

typedef struct
{
    uint8_t              remote_endpoint;
    T_AUDIO_STREAM_TYPE  stream_type;
    uint16_t             reserved;
    uint8_t              uuid[8];
    uint8_t              msg[0];
} T_AUDIO_REMOTE_INTERNAL_MSG;

typedef struct t_audio_remote_buffer_first_sync_cmd
{
    uint8_t  tid;
    uint8_t  buffer_type;
    uint16_t count;
    uint32_t upper_seq_number[MEDIA_SYNC_TRIGGER_CNT];
    uint32_t timestamp[MEDIA_SYNC_TRIGGER_CNT];
} T_AUDIO_REMOTE_BUFFER_FIRST_SYNC_CMD;

typedef struct t_audio_remote_buffer_first_sync_rsp
{
    uint8_t  tid;
    uint8_t  buffer_type;
    uint8_t  found;
    uint8_t  cause;
    uint32_t match_id;
} T_AUDIO_REMOTE_BUFFER_FIRST_SYNC_RSP;

typedef enum
{
    AUDIO_REMOTE_BUFFER_SYNC_REQ                    = 0x0000,
    AUDIO_REMOTE_BUFFER_ALIGN_REQ                   = 0x0001,
    AUDIO_REMOTE_BUFFER_ALIGN_RSP                   = 0x0002,
    AUDIO_REMOTE_BUFFER_SYNC_PLAY                   = 0x0003,
    AUDIO_REMOTE_BUFFER_SEAMLESS_JOIN_INFO          = 0x0004,
    AUDIO_REMOTE_BUFFER_SEAMLESS_JOIN_PLAY          = 0x0005,
    AUDIO_REMOTE_BUFFER_NOT_SEAMLESS_JOIN           = 0x0006,
    AUDIO_REMOTE_BUFFER_SEC_ALIGNING                = 0x0007,
    AUDIO_REMOTE_BUFFER_LOW_LATENCY_SYNC_PLAY       = 0x0008,
    AUDIO_REMOTE_BUFFER_ULTRA_LOW_LATENCY_SYNC_PLAY = 0x0009,

    AUDIO_REMOTE_BUFFER_MSG_RELAY                   = 0xFFFF,
} T_AUDIO_REMOTE_BUFFER_RELAY_MSG;

typedef struct t_audio_remote_instance
{
    struct t_audio_remote_instance *p_next;
    void                           *buffer_ent;
    T_MEDIA_PACKET_RECORD          *record_buf;    /* record instances' pool */
    T_OS_QUEUE                      free_records;  /* unused record instances */
    T_OS_QUEUE                      used_records;  /* used record instances */
    void                           *join_abort_timer;
    void                           *seq_check_timer;
    T_AUDIO_REMOTE_EVENT_CBACK      event_cback;
    T_AUDIO_REMOTE_RELAY_CBACK      relay_cback;
    T_AUDIO_STREAM_TYPE             stream_type;
    T_AUDIO_REMOTE_STATE            state;
    uint16_t                        buffer_sync_trigger;
    uint32_t                        join_clk;
    uint32_t                        join_packet_id;
    uint16_t                        join_local_seq;
    uint16_t                        join_frame_counter;
    uint16_t                        delta_frame;
    uint16_t                        base_count;
    int32_t                         frame_diff;
    int32_t                         seq_diff;
    uint8_t                         endpoint;
    uint8_t                         seamless_join_retry_cnt;
    uint8_t                         seamless_join_lost_packet_cnt;
    uint8_t                         seamless_join_lost_packet_cnt_last;
    uint8_t                         slave_audio_sync;
    uint8_t                         sync_peroid_trigger_cnt;
    uint8_t                         sync_packet_cnt;
} T_AUDIO_REMOTE_INSTANCE;

typedef struct t_audio_remote_db
{
    T_REMOTE_RELAY_HANDLE   relay_handle;
    T_OS_QUEUE              remote_db_list;
    bool                    seamless_join;
    bool                    force_join;
    uint8_t                 endpoint_count;
} T_AUDIO_REMOTE_DB;

static T_AUDIO_REMOTE_DB *audio_remote_db;

void audio_remote_relay_cback(uint16_t               event,
                              T_REMOTE_RELAY_STATUS  status,
                              void                  *buf,
                              uint16_t               len);

static bool audio_remote_handle_check(T_AUDIO_REMOTE_INSTANCE *handle)
{
    if (handle != NULL)
    {
        return os_queue_search(&audio_remote_db->remote_db_list, handle);
    }

    return false;
}

static T_AUDIO_REMOTE_INSTANCE *audio_remote_handle_find(uint8_t endpoint)
{
    T_AUDIO_REMOTE_INSTANCE *handle;

    handle = os_queue_peek(&audio_remote_db->remote_db_list, 0);
    while (handle != NULL)
    {
        if (handle->endpoint == endpoint)
        {
            break;
        }

        handle = handle->p_next;
    }

    return handle;
}

static void audio_remote_handle_delete(T_AUDIO_REMOTE_INSTANCE *handle)
{
    if (handle != NULL)
    {
        if (handle->join_abort_timer != NULL)
        {
            sys_timer_delete(handle->join_abort_timer);
        }

        if (handle->seq_check_timer != NULL)
        {
            sys_timer_delete(handle->seq_check_timer);
        }

        os_queue_delete(&audio_remote_db->remote_db_list, handle);
        free(handle);
    }
}

int32_t cacu_diff_u16(uint16_t x, uint16_t y)
{
    int32_t seq_diff;
    if (x > y)
    {
        if (x - y > 0x7fff) //0xff-0x00
        {
            seq_diff = -((y + 0x10000) - x);
        }
        else
        {
            seq_diff = x - y;
        }
    }
    else
    {
        if (y - x > 0x7fff) //0xff-0x00
        {
            seq_diff = x + 0x10000 - y ;
        }
        else
        {
            seq_diff = x - y;
        }
    }
    return seq_diff;
}

void audio_remote_record_init(T_AUDIO_REMOTE_HANDLE handle,
                              uint16_t              size)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance;
    uint16_t                 i;

    remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;

    remote_instance->record_buf = NULL;
    os_queue_init(&remote_instance->free_records);
    os_queue_init(&remote_instance->used_records);

    if (size != 0)
    {
        remote_instance->record_buf = calloc(size, sizeof(T_MEDIA_PACKET_RECORD));
        if (remote_instance->record_buf != NULL)
        {
            for (i = 0; i < size; i++)
            {
                os_queue_in(&remote_instance->free_records, &remote_instance->record_buf[i]);
            }
        }
    }
}

void audio_remote_record_deinit(T_AUDIO_REMOTE_HANDLE handle)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance;

    remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;

    os_queue_init(&remote_instance->free_records);
    os_queue_init(&remote_instance->used_records);

    if (remote_instance->record_buf != NULL)
    {
        free(remote_instance->record_buf);
        remote_instance->record_buf = NULL;
    }
}

void audio_remote_record_clear(T_AUDIO_REMOTE_HANDLE handle)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance;
    T_MEDIA_PACKET_RECORD   *record;

    remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;

    record = os_queue_out(&remote_instance->used_records);
    while (record != NULL)
    {
        os_queue_in(&remote_instance->free_records, record);
        record = os_queue_out(&remote_instance->used_records);
    }
}

void audio_remote_record_push(T_AUDIO_REMOTE_HANDLE  handle,
                              void                  *p_packet)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance;
    T_MEDIA_PACKET_RECORD   *record;
    T_MEDIA_DATA_HDR        *pmedia_packet;

    remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;
    if (remote_instance == NULL)
    {
        return;
    }

    record = os_queue_out(&remote_instance->free_records);
    if (record == NULL)
    {
        /* dequeue the oldest record instance */
        record = os_queue_out(&remote_instance->used_records);
        if (record == NULL)
        {
            /* this shall not happen */
            return;
        }
    }

    pmedia_packet = p_packet;

    record->frame_counter = pmedia_packet->frame_counter;
    record->local_seq_num = pmedia_packet->local_seq_number;
    record->upper_seq_num = pmedia_packet->upper_seq_number;
    record->frame_number  = pmedia_packet->frame_number;
    record->timestamp     = pmedia_packet->timestamp;
    record->seq_offset    = 0;

    os_queue_in(&remote_instance->used_records, record);
}

T_MEDIA_PACKET_RECORD *audio_remote_record_search(T_AUDIO_REMOTE_HANDLE handle,
                                                  uint16_t              upper_seq)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance;
    T_MEDIA_PACKET_RECORD   *record;

    remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;

    record = os_queue_peek(&remote_instance->used_records, 0);
    while (record != NULL)
    {
        if (record->upper_seq_num == upper_seq)
        {
            break;
        }

        record = record->p_next;
    }

    return record;
}

int32_t audio_remote_record_update(T_AUDIO_REMOTE_HANDLE handle,
                                   uint16_t              local_seq,
                                   uint16_t              frame_counter,
                                   uint8_t               frame_number)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance;
    T_MEDIA_PACKET_RECORD   *record;
    bool                     found = false;
    int32_t                  frame_diff = 0;
    int32_t                  ret = 0;

    remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;
    if (remote_instance == NULL)
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    record = os_queue_peek(&remote_instance->used_records, 0);
    while (record != NULL)
    {
        if (found == false)
        {
            if (record->local_seq_num == local_seq)
            {
                found = true;
                frame_diff = frame_number - record->frame_number;

                AUDIO_PRINT_INFO5("audio_remote_record_update: local_seq %u, seq_offset %d, frame_number %u, frame_counter %u, frame_diff %d",
                                  local_seq, record->seq_offset, record->frame_number, record->frame_counter, frame_diff);

                record->frame_counter += frame_diff;
                record->frame_number  += frame_diff;

                if (frame_diff == 0)
                {
                    break;
                }
            }
        }
        else
        {
            /* update the following record instances's accumulated frame counter only */
            record->frame_counter += frame_diff;
        }

        record = record->p_next;
    }

    if (found == false)
    {
        ret = 2;
        goto fail_invalid_local_seq;
    }

    return frame_diff;

fail_invalid_local_seq:
fail_invalid_handle:
    AUDIO_PRINT_ERROR1("audio_remote_record_update: failed %d", -ret);
    return frame_diff;
}

void audio_remote_record_merge(T_AUDIO_REMOTE_HANDLE            handle,
                               T_MEDIA_PACKET_RECORD           *record,
                               T_AUDIO_REMOTE_BUFFER_SYNC_UNIT *p_unit)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance;
    int32_t                  seq_diff;
    int32_t                  frame_diff;

    remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;

    if (os_queue_search(&remote_instance->used_records, record) == true)
    {
        seq_diff   = cacu_diff_u16(p_unit->local_seq_num, record->local_seq_num + record->seq_offset);
        frame_diff = cacu_diff_u16(p_unit->frame_counter, record->frame_counter);

        while (record != NULL)
        {
            record->seq_offset    += seq_diff;
            record->frame_counter += frame_diff;

            AUDIO_PRINT_INFO4("audio_remote_record_merge: seq_diff %d, local_seq %u, seq_offset %d, frame_counter %u",
                              seq_diff, record->local_seq_num, record->seq_offset, record->frame_counter);

            record = record->p_next;
        }
    }
}

uint16_t audio_remote_record_pop(T_AUDIO_REMOTE_HANDLE            handle,
                                 T_AUDIO_REMOTE_BUFFER_SYNC_UNIT *p_data,
                                 uint16_t                         count)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance;
    T_MEDIA_PACKET_RECORD   *record;
    uint16_t                 i;

    remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;

    if (remote_instance->used_records.count < count)
    {
        count = remote_instance->used_records.count;
    }

    for (i = 0; i < count; i++)
    {
        record = os_queue_peek(&remote_instance->used_records, -1);
        if (record != NULL)
        {
            p_data[i].frame_counter = record->frame_counter;
            p_data[i].local_seq_num = record->local_seq_num;
            p_data[i].upper_seq_num = record->upper_seq_num;
            p_data[i].timestamp     = record->timestamp;

            os_queue_delete(&remote_instance->used_records, record);
            os_queue_in(&remote_instance->free_records, record);
        }
    }

    return count;
}

uint16_t audio_remote_record_peek(T_AUDIO_REMOTE_HANDLE            handle,
                                  T_AUDIO_REMOTE_BUFFER_SYNC_UNIT *p_data,
                                  uint16_t                         count)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance;
    T_MEDIA_PACKET_RECORD   *record;
    uint16_t                 i;

    remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;

    if (remote_instance->used_records.count < count)
    {
        count = remote_instance->used_records.count;
    }

    for (i = 0; i < count; i++)
    {
        record = os_queue_peek(&remote_instance->used_records, -(i + 1));
        if (record != NULL)
        {
            p_data[i].frame_counter = record->frame_counter;
            p_data[i].local_seq_num = record->local_seq_num;
            p_data[i].upper_seq_num = record->upper_seq_num;
            p_data[i].timestamp     = record->timestamp;
        }
    }

    return count;
}

bool audio_remote_init(void)
{
    int32_t ret = 0;

    audio_remote_db = malloc(sizeof(T_AUDIO_REMOTE_DB));
    if (audio_remote_db == NULL)
    {
        ret = 1;
        goto fail_alloc_db;
    }

    audio_remote_db->relay_handle = remote_relay_register(audio_remote_relay_cback);
    if (audio_remote_db->relay_handle == NULL)
    {
        ret = 2;
        goto fail_register_cback;
    }

    audio_remote_db->seamless_join = true;
    audio_remote_db->force_join = false;
    audio_remote_db->endpoint_count = 0;
    os_queue_init(&audio_remote_db->remote_db_list);
    return true;

fail_register_cback:
    free(audio_remote_db);
    audio_remote_db = NULL;
fail_alloc_db:
    AUDIO_PRINT_ERROR1("audio_remote_init: failed %d", -ret);
    return false;
}

void audio_remote_deinit(void)
{
    if (audio_remote_db != NULL)
    {
        T_AUDIO_REMOTE_INSTANCE *handle;

        handle = os_queue_out(&audio_remote_db->remote_db_list);
        while (handle != NULL)
        {
            if (handle->join_abort_timer != NULL)
            {
                sys_timer_delete(handle->join_abort_timer);
            }

            if (handle->seq_check_timer != NULL)
            {
                sys_timer_delete(handle->seq_check_timer);
            }

            free(handle);
            handle = os_queue_out(&audio_remote_db->remote_db_list);
        }

        remote_relay_unregister(audio_remote_db->relay_handle);
        free(audio_remote_db);
    }
}

T_AUDIO_REMOTE_HANDLE audio_remote_register(T_AUDIO_STREAM_TYPE         type,
                                            void                       *buffer_ent,
                                            T_AUDIO_REMOTE_EVENT_CBACK  event_cback,
                                            T_AUDIO_REMOTE_RELAY_CBACK  relay_cback)
{
    T_AUDIO_REMOTE_INSTANCE *handle;

    handle = calloc(1, sizeof(T_AUDIO_REMOTE_INSTANCE));
    if (handle == NULL)
    {
        return NULL;
    }

    handle->stream_type = type;
    handle->buffer_ent = buffer_ent;
    if (type == AUDIO_STREAM_TYPE_PLAYBACK)
    {
        audio_remote_record_init(handle, AUDIO_REMOTE_RECORD_SIZE);
    }
    else
    {
        audio_remote_record_init(handle, 0);
    }

    handle->event_cback = event_cback;
    handle->relay_cback = relay_cback;
    handle->endpoint = audio_remote_db->endpoint_count++;
    os_queue_in(&audio_remote_db->remote_db_list, handle);
    return handle;
}

void audio_remote_unregister(T_AUDIO_REMOTE_HANDLE handle)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;

    if (audio_remote_handle_check(remote_instance) == true)
    {
        audio_remote_record_deinit(handle);
        audio_remote_handle_delete(remote_instance);
    }
}

void audio_remote_join_abort_timeout_cback(T_SYS_TIMER_HANDLE handle)
{
    uint32_t timer_id;
    T_AUDIO_REMOTE_INSTANCE *remote_instance;

    timer_id = sys_timer_id_get(handle);

    remote_instance = audio_remote_handle_find(timer_id);
    if (remote_instance != NULL)
    {
        AUDIO_PRINT_TRACE2("audio_remote_join_abort_timeout_cback: timer_id 0x%08x, handle %p",
                           timer_id, handle);
        audio_remote_seamless_join_retry(remote_instance, MEDIA_BUFFER_SEAMLESS_TIMEOUT);
    }
}

void audio_remote_seq_check_timeout_cback(T_SYS_TIMER_HANDLE handle)
{
    uint32_t timer_id;
    T_AUDIO_REMOTE_INSTANCE *remote_instance;

    timer_id = sys_timer_id_get(handle);

    remote_instance = audio_remote_handle_find(timer_id);
    if (remote_instance != NULL)
    {
        AUDIO_PRINT_TRACE2("audio_remote_seq_check_timeout_cback: timer_id 0x%08x, handle %p",
                           timer_id, handle);
        remote_instance->seamless_join_lost_packet_cnt_last =
            remote_instance->seamless_join_lost_packet_cnt;
        remote_instance->seamless_join_lost_packet_cnt = 0;
        sys_timer_start(remote_instance->seq_check_timer);
    }
}

static bool audio_remote_internal_async_msg_relay(T_AUDIO_REMOTE_INSTANCE *remote_instance,
                                                  uint16_t                 msg_id,
                                                  void                    *msg_buf,
                                                  uint16_t                 msg_len,
                                                  bool                     loopback)
{
    bool ret;
    T_AUDIO_REMOTE_INTERNAL_MSG *async_msg;
    T_MEDIA_BUFFER_ENTITY *buffer_ent;
    uint16_t malloc_len = msg_len + sizeof(T_AUDIO_REMOTE_INTERNAL_MSG);

    if (audio_remote_handle_check(remote_instance) == false)
    {
        return false;
    }

    async_msg = malloc(malloc_len);
    if (async_msg == NULL)
    {
        return false;
    }

    async_msg->remote_endpoint = remote_instance->endpoint;
    buffer_ent = remote_instance->buffer_ent;
    if (buffer_ent != NULL)
    {
        memcpy(async_msg->uuid, buffer_ent->uuid, 8);
        async_msg->stream_type = buffer_ent->stream_type;
    }
    memcpy(async_msg->msg, msg_buf, msg_len);

    ret = remote_async_msg_relay(audio_remote_db->relay_handle,
                                 msg_id,
                                 async_msg,
                                 malloc_len,
                                 loopback);
    free(async_msg);
    return ret;
}

static bool audio_remote_internal_sync_msg_relay(T_AUDIO_REMOTE_INSTANCE *remote_instance,
                                                 uint16_t                 msg_id,
                                                 void                    *msg_buf,
                                                 uint16_t                 msg_len,
                                                 T_REMOTE_TIMER_TYPE      timer_type,
                                                 uint32_t                 timer_period,
                                                 bool                     loopback)
{
    bool ret;
    T_AUDIO_REMOTE_INTERNAL_MSG *sync_msg;
    T_MEDIA_BUFFER_ENTITY *buffer_ent;
    uint16_t malloc_len = msg_len + sizeof(T_AUDIO_REMOTE_INTERNAL_MSG);

    if (audio_remote_handle_check(remote_instance) == false)
    {
        return false;
    }

    sync_msg = malloc(malloc_len);
    if (sync_msg == NULL)
    {
        return false;
    }

    sync_msg->remote_endpoint = remote_instance->endpoint;
    buffer_ent = remote_instance->buffer_ent;
    if (buffer_ent != NULL)
    {
        memcpy(sync_msg->uuid, buffer_ent->uuid, 8);
        sync_msg->stream_type = buffer_ent->stream_type;
    }
    memcpy(sync_msg->msg, msg_buf, msg_len);

    ret = remote_sync_msg_relay(audio_remote_db->relay_handle,
                                msg_id,
                                sync_msg,
                                malloc_len,
                                timer_type,
                                timer_period,
                                loopback);
    free(sync_msg);
    return ret;
}

void audio_remote_cancel_seamless_join(T_AUDIO_REMOTE_HANDLE handle)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;

    if (handle == NULL)
    {
        return;
    }

    audio_remote_internal_async_msg_relay(remote_instance,
                                          AUDIO_REMOTE_BUFFER_NOT_SEAMLESS_JOIN,
                                          &remote_instance->stream_type,
                                          1,
                                          false);
    audio_remote_reset(handle);
}

void audio_remote_seamless_join_info_req(void)
{
    if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
    {
        audio_path_synchronization_join_set(NULL, 1);
    }
}

void audio_remote_dsp_seamless_join_handle(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                                           uint32_t               join_clk,
                                           uint32_t               join_frame,
                                           uint32_t               delta_frame)
{
    uint16_t framecounter_last;
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)
                                               buffer_ent->audio_remote_handle;

    if (remote_instance == NULL)
    {
        return;
    }

    media_buffer_get_downstream_info(buffer_ent, &framecounter_last, NULL);
    remote_instance->join_clk = join_clk;

    if (join_frame - framecounter_last < delta_frame)
    {
        remote_instance->delta_frame = join_frame - framecounter_last;
    }
    else
    {
        remote_instance->delta_frame = join_frame - (framecounter_last + 0x10000);
    }
    remote_instance->base_count = framecounter_last;
    AUDIO_PRINT_INFO4("audio_remote_dsp_seamless_join_handle: join_clk 0x%x join_frame %u, delta_frame %u->%u",
                      remote_instance->join_clk, join_frame, delta_frame, remote_instance->delta_frame);
    audio_remote_set_state(remote_instance, AUDIO_REMOTE_STATE_SEAMLESS_JOIN);
}

bool audio_remote_buffer_sync_req_handle(T_AUDIO_REMOTE_INSTANCE *remote_instance,
                                         uint8_t                 *pdata,
                                         uint8_t                  len)
{
    T_AUDIO_REMOTE_BUFFER_SYNC_UNIT *p_sync_units = (T_AUDIO_REMOTE_BUFFER_SYNC_UNIT *)pdata;
    uint8_t sync_pkt_cnt = len / sizeof(T_AUDIO_REMOTE_BUFFER_SYNC_UNIT);
    T_MEDIA_PACKET_RECORD *p_rec;
    uint8_t i;
    bool rec_found = false;

    for (i = 0; i < sync_pkt_cnt; i++)
    {
        p_rec = audio_remote_record_search(remote_instance, p_sync_units[i].upper_seq_num);
        if (p_rec != NULL)
        {
            if ((uint16_t)(p_rec->local_seq_num + p_rec->seq_offset) != p_sync_units[i].local_seq_num ||
                p_rec->frame_counter != p_sync_units[i].frame_counter)
            {
                remote_instance->frame_diff += cacu_diff_u16(p_sync_units[i].frame_counter, p_rec->frame_counter);
                remote_instance->seq_diff += cacu_diff_u16(p_sync_units[i].local_seq_num,
                                                           p_rec->local_seq_num + p_rec->seq_offset);
                audio_remote_record_merge(remote_instance, p_rec, &p_sync_units[i]);
                AUDIO_PRINT_INFO3("audio_remote_buffer_sync_req_handle: mismatch seq_diff %d, frame_diff %d, upper_seq %u",
                                  remote_instance->seq_diff, remote_instance->frame_diff, p_rec->upper_seq_num);
                rec_found = true;
                break;
            }
            else
            {
                remote_instance->frame_diff = 0;
                remote_instance->seq_diff = 0;
                AUDIO_PRINT_INFO4("audio_remote_buffer_sync_req_handle: match local_seq %u(%d), upper_seq %u, frame_count %u",
                                  p_rec->local_seq_num, p_rec->seq_offset, p_rec->upper_seq_num, p_rec->frame_counter);
                rec_found = true;
                break;
            }
        }
    }

    if (rec_found == false)
    {
        AUDIO_PRINT_TRACE0("audio_remote_buffer_sync_req_handle: sec can't find match record");

        p_rec = os_queue_peek(&remote_instance->used_records, -1);
        if (p_rec == NULL)
        {
            return false;
        }
        if (cacu_diff_u16(p_sync_units[sync_pkt_cnt - 1].upper_seq_num, p_rec->upper_seq_num) > 0)
        {
            AUDIO_PRINT_TRACE0("audio_remote_buffer_sync_req_handle: sec may dup too many packets");
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return true;
    }
}

bool audio_remote_period_sync_check(T_AUDIO_REMOTE_HANDLE handle,
                                    uint16_t              buffer_sync_trigger_cnt)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;

    if (audio_remote_handle_check(remote_instance) == false)
    {
        return false;
    }

    if (buffer_sync_trigger_cnt >= remote_instance->sync_peroid_trigger_cnt)
    {
        return true;
    }
    return false;
}

void audio_remote_buffer_sync_req(T_AUDIO_REMOTE_HANDLE handle)
{
    T_AUDIO_REMOTE_BUFFER_SYNC_UNIT *p_sync_units;
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;
    uint16_t pkt_cnt;

    if (handle == NULL)
    {
        return;
    }

    if (audio_remote_get_state(handle) != AUDIO_REMOTE_STATE_SYNCED ||
        remote_instance->stream_type != AUDIO_STREAM_TYPE_PLAYBACK)
    {
        return;
    }

    pkt_cnt = remote_instance->used_records.count;
    if (pkt_cnt == 0)
    {
        AUDIO_PRINT_TRACE0("audio_remote_buffer_sync_req: no data in history");
        return;
    }
    pkt_cnt = pkt_cnt > remote_instance->sync_packet_cnt ? remote_instance->sync_packet_cnt : pkt_cnt;
    p_sync_units = calloc(pkt_cnt, sizeof(T_AUDIO_REMOTE_BUFFER_SYNC_UNIT));
    if (p_sync_units != NULL)
    {
        audio_remote_record_pop(handle, p_sync_units, pkt_cnt);
        audio_remote_internal_async_msg_relay(remote_instance,
                                              AUDIO_REMOTE_BUFFER_SYNC_REQ,
                                              p_sync_units,
                                              sizeof(T_AUDIO_REMOTE_BUFFER_SYNC_UNIT)*pkt_cnt,
                                              false);
        AUDIO_PRINT_TRACE3("audio_remote_buffer_sync_req: cnt %u, upper_seq %u--%u", pkt_cnt,
                           p_sync_units->upper_seq_num,
                           p_sync_units[pkt_cnt - 1].upper_seq_num);
        free(p_sync_units);
    }
}

bool audio_remote_find_join_pkt(T_AUDIO_REMOTE_HANDLE handle)
{
    if (handle == NULL)
    {
        return false;
    }

    if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
    {
        T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;
        T_MEDIA_BUFFER_ENTITY *buffer_ent = remote_instance->buffer_ent;
        uint16_t local_seq;
        T_MEDIA_DATA_HDR *p_packet;
        uint16_t frame_a = 0;
        uint16_t frame_b = 0;
        uint16_t framecounter_last;
        bool found_packet = false;

        media_buffer_get_downstream_info(buffer_ent, &framecounter_last, &local_seq);

        p_packet = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);

        AUDIO_PRINT_TRACE3("base %u delta %u last %u", remote_instance->base_count,
                           remote_instance->delta_frame, framecounter_last);

        remote_instance->delta_frame -= (uint16_t)(framecounter_last -
                                                   remote_instance->base_count);
        remote_instance->base_count = framecounter_last;

        while (p_packet != NULL)
        {
            frame_a = frame_b;
            frame_b += p_packet->frame_number;
            local_seq++;
            if (remote_instance->delta_frame > frame_a &&
                remote_instance->delta_frame <= frame_b)
            {
                p_packet->local_seq_number = local_seq;
                p_packet->frame_counter = frame_b + remote_instance->base_count;
                AUDIO_PRINT_INFO5("audio_remote_find_join_pkt: upper_seq %u, local_seq %u, frame_counter %u, base_count %u, delta_frame %u",
                                  p_packet->upper_seq_number, p_packet->local_seq_number, p_packet->frame_counter,
                                  remote_instance->base_count, remote_instance->delta_frame);
                found_packet = true;
                break;
            }
            else
            {
                p_packet = p_packet->p_next;
            }
        }

        if (found_packet)
        {
            T_AUDIO_MSG_JOIN_INFO cmd;

            cmd.join_clk = remote_instance->join_clk;
            cmd.packet_local_seq = p_packet->local_seq_number;
            cmd.packet_frame_counter = p_packet->frame_counter;
            cmd.stream_type = remote_instance->stream_type;
            cmd.tid = buffer_ent->media_buffer_fst_sync_tid;
            cmd.packet_id = p_packet->upper_seq_number;

            audio_remote_internal_async_msg_relay(remote_instance,
                                                  AUDIO_REMOTE_BUFFER_SEAMLESS_JOIN_INFO,
                                                  &cmd,
                                                  sizeof(T_AUDIO_MSG_JOIN_INFO),
                                                  false);
            audio_remote_set_state(remote_instance, AUDIO_REMOTE_STATE_UNSYNC);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;
        T_MEDIA_BUFFER_ENTITY *buffer_ent = remote_instance->buffer_ent;
        T_MEDIA_DATA_HDR *p_packet = NULL;
        T_BT_CLK_REF clk_ref;
        uint32_t bb_clock_slot;
        uint16_t bb_clock_us;
        uint8_t  clk_idx;
        uint32_t time_us;
        T_AUDIO_MSG_JOIN_INFO cmd;

        clk_ref = bt_piconet_clk_get(BT_CLK_SNIFFING, &clk_idx, &bb_clock_slot, &bb_clock_us);

        if (clk_ref != BT_CLK_NONE &&
            bt_clk_compare(remote_instance->join_clk, bb_clock_slot + 20 * 2 * 1000 / 625))
        {
            p_packet = media_buffer_peek(buffer_ent, -1, BUFFER_DIR_DOWNSTREAM);
            if (p_packet->upper_seq_number != (uint16_t)remote_instance->join_packet_id)
            {
                if (cacu_diff_u16(p_packet->upper_seq_number, ((uint16_t)remote_instance->join_packet_id)) > 0)
                {
                    audio_remote_seamless_join_retry(remote_instance, MEDIA_BUFFER_SEAMLESS_MISS_PKT);
                }
                return false;
            }
            else
            {
                cmd.packet_id = p_packet->upper_seq_number;
            }

            media_buffer_flush(buffer_ent, media_buffer_ds_pkt_cnt(buffer_ent) - 1,
                               BUFFER_DIR_DOWNSTREAM);
            time_us = (((remote_instance->join_clk << 4) - ((bb_clock_slot + 20 * 2 * 1000 / 625) << 4)) >> 4)
                      *
                      625 / 2;
            time_us = (time_us > 50000) ? 50000 : time_us;
            cmd.join_clk = remote_instance->join_clk;
            cmd.packet_local_seq = remote_instance->join_local_seq;
            cmd.packet_frame_counter = remote_instance->join_frame_counter;
            cmd.stream_type = remote_instance->stream_type;
            cmd.tid = buffer_ent->media_buffer_fst_sync_tid;
            audio_remote_internal_sync_msg_relay(remote_instance,
                                                 AUDIO_REMOTE_BUFFER_SEAMLESS_JOIN_PLAY,
                                                 &cmd,
                                                 sizeof(T_AUDIO_MSG_JOIN_INFO),
                                                 REMOTE_TIMER_HIGH_PRECISION,
                                                 time_us / 1000,
                                                 false);
            media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_SYNC);
            return true;

        }
        else
        {
            audio_remote_seamless_join_retry(remote_instance, MEDIA_BUFFER_ERR_CLK);
            return false;
        }
    }
}

T_AUDIO_REMOTE_STATE audio_remote_get_state(T_AUDIO_REMOTE_HANDLE handle)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;
    return remote_instance->state;
}

void audio_remote_set_state(T_AUDIO_REMOTE_HANDLE handle,
                            T_AUDIO_REMOTE_STATE  state)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;
    if (handle == NULL)
    {
        return;
    }
    AUDIO_PRINT_INFO1("audio_remote_set_state: %u", state);
    remote_instance->state = state;
}

void audio_remote_seamless_join_retry(T_AUDIO_REMOTE_HANDLE handle,
                                      T_AUDIO_MGR_EXCEPTION exc)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;

    if (handle == NULL)
    {
        return;
    }

    remote_instance->seamless_join_retry_cnt++;
    if (remote_instance->seamless_join_retry_cnt >= AUDIO_REMOTE_JOINING_RETRY_CNT &&
        audio_remote_db->force_join == false)
    {
        audio_mgr_exception(remote_instance->buffer_ent, exc, 0, 0);
    }
    else
    {
        audio_remote_reset(remote_instance);
    }
}

void audio_remote_reset(T_AUDIO_REMOTE_HANDLE handle)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;
    T_AUDIO_STREAM_MODE mode;

    if (handle == NULL)
    {
        return;
    }
    AUDIO_PRINT_INFO0("audio_remote_reset");

    remote_instance->state = AUDIO_REMOTE_STATE_UNSYNC;
    remote_instance->seq_diff = 0;
    remote_instance->frame_diff = 0;
    remote_instance->buffer_sync_trigger = 0;
    remote_instance->seamless_join_retry_cnt = 0;
    remote_instance->slave_audio_sync = 0;
    audio_remote_record_clear(handle);

    media_buffer_get_mode(remote_instance->buffer_ent, &mode);
    if (mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
    {
        remote_instance->sync_peroid_trigger_cnt = MEDIA_SYNC_PEROID_TRIGGER_CNT_RTK;
        remote_instance->sync_packet_cnt = MEDIA_SYNC_PAKCET_CNT_RTK;
    }
    else
    {
        remote_instance->sync_peroid_trigger_cnt = MEDIA_SYNC_PEROID_TRIGGER_CNT;
        remote_instance->sync_packet_cnt = MEDIA_SYNC_PAKCET_CNT;
    }
}

bool audio_remote_align_buffer(T_AUDIO_REMOTE_HANDLE handle)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;
    T_MEDIA_BUFFER_ENTITY *buffer_ent;
    T_AUDIO_REMOTE_BUFFER_FIRST_SYNC_CMD *psync_cmd;
    bool ret = true;
    T_MEDIA_DATA_HDR *p_local_media_head;

    if (handle == NULL)
    {
        return false;
    }

    buffer_ent = remote_instance->buffer_ent;
    if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
    {
        uint16_t media_pkt_cnt;

        media_pkt_cnt = media_buffer_ds_pkt_cnt(buffer_ent);
        if (media_pkt_cnt > MEDIA_SYNC_TRIGGER_CNT)
        {
            media_pkt_cnt = MEDIA_SYNC_TRIGGER_CNT;
        }

        if (media_pkt_cnt == 0 ||
            media_buffer_ds_frame_cnt(buffer_ent) == 0)
        {
            return false;
        }

        psync_cmd = calloc(1, sizeof(T_AUDIO_REMOTE_BUFFER_FIRST_SYNC_CMD));
        if (psync_cmd == NULL)
        {
            return false;
        }

        buffer_ent->media_buffer_fst_sync_tid++;
        psync_cmd->tid = buffer_ent->media_buffer_fst_sync_tid;
        psync_cmd->count = media_pkt_cnt;
        p_local_media_head = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);

        for (uint16_t i = 0; i < media_pkt_cnt; i++)
        {
            psync_cmd->upper_seq_number[i] = p_local_media_head->upper_seq_number;
            psync_cmd->timestamp[i] = p_local_media_head->timestamp;
            p_local_media_head = p_local_media_head->p_next;
        }

        psync_cmd->buffer_type = remote_instance->stream_type;

        if (remote_instance->stream_type != AUDIO_STREAM_TYPE_RECORD)
        {
            AUDIO_PRINT_TRACE6("audio_mgr_align_buffer: pkt cnt %d tid %d, upper_seq_number: %d->%d timestamp: %d->%d",
                               psync_cmd->count, psync_cmd->tid, psync_cmd->upper_seq_number[0],
                               psync_cmd->upper_seq_number[media_pkt_cnt - 1], psync_cmd->timestamp[0],
                               psync_cmd->timestamp[media_pkt_cnt - 1]);
        }

        if (audio_remote_internal_async_msg_relay(remote_instance,
                                                  AUDIO_REMOTE_BUFFER_ALIGN_REQ,
                                                  psync_cmd,
                                                  sizeof(T_AUDIO_REMOTE_BUFFER_FIRST_SYNC_CMD),
                                                  false) == false)
        {
            ret = false;
        }

        free(psync_cmd);
        if (ret)
        {
            audio_remote_set_state(handle, AUDIO_REMOTE_STATE_ALIGNING);
        }
    }
    else
    {
        T_AUDIO_STREAM_MODE mode;
        audio_remote_set_state(handle, AUDIO_REMOTE_STATE_ALIGNING);

        media_buffer_get_mode(buffer_ent, &mode);

        if (mode != AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY && mode != AUDIO_STREAM_MODE_LOW_LATENCY)
        {
            audio_remote_internal_async_msg_relay(remote_instance,
                                                  AUDIO_REMOTE_BUFFER_SEC_ALIGNING,
                                                  &remote_instance->stream_type,
                                                  1,
                                                  false);
            if (remote_instance->join_abort_timer != NULL)
            {
                sys_timer_delete(remote_instance->join_abort_timer);
            }
            remote_instance->join_abort_timer = sys_timer_create("join_abort_timer",
                                                                 SYS_TIMER_TYPE_LOW_PRECISION,
                                                                 remote_instance->endpoint,
                                                                 RWS_TIME_TO_ABORT_SEAMLESS_JOIN,
                                                                 false,
                                                                 audio_remote_join_abort_timeout_cback);
            if (remote_instance->join_abort_timer != NULL)
            {
                sys_timer_start(remote_instance->join_abort_timer);
            }
        }
        ret = true;
    }

    return ret;
}

void audio_remote_align_buffer_req_handle(T_AUDIO_REMOTE_HANDLE  handle,
                                          uint8_t               *pdata,
                                          uint8_t                len)
{
    T_AUDIO_REMOTE_BUFFER_FIRST_SYNC_CMD *psync_cmd = (T_AUDIO_REMOTE_BUFFER_FIRST_SYNC_CMD *)pdata;
    uint16_t media_pkt_cnt = psync_cmd->count;
    T_AUDIO_REMOTE_INSTANCE *remote_instance;
    uint32_t *IDs = NULL;
    T_AUDIO_REMOTE_BUFFER_FIRST_SYNC_RSP rsp;
    T_MEDIA_DATA_HDR *p_local_media_head;
    uint8_t pkt_cnt;
    uint8_t istart = 0;
    uint8_t i = 0;
    T_MEDIA_BUFFER_ENTITY *buffer_ent = NULL;
    uint32_t id = 0;
    bool ret = true;

    rsp.found = false;
    rsp.tid = psync_cmd->tid;
    rsp.buffer_type = psync_cmd->buffer_type;
    rsp.cause = 0;

    remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;
    buffer_ent = remote_instance->buffer_ent;

    if (buffer_ent == NULL)
    {
        return;
    }

    if (buffer_ent->state == MEDIA_BUFFER_STATE_IDLE)
    {
        rsp.cause = FST_SYNC_BUFFET_INACTIVE;
        audio_remote_internal_async_msg_relay(remote_instance,
                                              AUDIO_REMOTE_BUFFER_ALIGN_RSP,
                                              &rsp,
                                              sizeof(T_AUDIO_REMOTE_BUFFER_FIRST_SYNC_RSP),
                                              false);
        return;
    }

    buffer_ent->media_buffer_fst_sync_tid = psync_cmd->tid;
    if (remote_instance->state != AUDIO_REMOTE_STATE_ALIGNING)
    {
        uint32_t cause = FST_SYNC_STATE_ERROR;
        audio_mgr_exception(buffer_ent, MEDIA_BUFFER_FST_SYNC_ERR, 0,
                            (cause << 16) | (remote_instance->state << 8) |
                            buffer_ent->media_buffer_fst_sync_tid);
        return;
    }

    pkt_cnt = media_buffer_ds_pkt_cnt(buffer_ent);
    AUDIO_PRINT_TRACE2("audio_remote_align_buffer_req_handle: media_buffer_sync rev cnt %d, local cnt %d",
                       media_pkt_cnt, pkt_cnt);
    if (pkt_cnt == 0)
    {
        rsp.found = false;
        rsp.cause = FST_SYNC_DATA_NOT_FOUND;
        audio_remote_internal_async_msg_relay(remote_instance,
                                              AUDIO_REMOTE_BUFFER_ALIGN_RSP,
                                              &rsp,
                                              sizeof(T_AUDIO_REMOTE_BUFFER_FIRST_SYNC_RSP),
                                              false);
        return;
    }

    p_local_media_head = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);

    id = p_local_media_head->upper_seq_number;
    IDs = psync_cmd->upper_seq_number;

    if (cacu_diff_u16(id, IDs[media_pkt_cnt - 1]) > 0)
    {
        rsp.found = false;
        rsp.cause = FST_SYNC_DATA_EXPIRED;
        rsp.match_id = id;
        ret = false;
        audio_remote_internal_async_msg_relay(remote_instance,
                                              AUDIO_REMOTE_BUFFER_ALIGN_RSP,
                                              &rsp,
                                              sizeof(T_AUDIO_REMOTE_BUFFER_FIRST_SYNC_RSP),
                                              false);
    }

    if (ret == false)
    {
        AUDIO_PRINT_INFO2("audio_remote_align_buffer_req_handle: media_buffer_sync rev latest id %d < local oldest id %d",
                          IDs[media_pkt_cnt - 1], id);
        return;
    }

    p_local_media_head = media_buffer_peek(buffer_ent, -1, BUFFER_DIR_DOWNSTREAM);
    id = p_local_media_head->upper_seq_number;

    if (bt_clk_compare(IDs[0], id) > 0)
    {
        rsp.found = false;
        rsp.cause = FST_SYNC_DATA_EXPIRED;
        ret = false;
        audio_remote_internal_async_msg_relay(remote_instance,
                                              AUDIO_REMOTE_BUFFER_ALIGN_RSP,
                                              &rsp,
                                              sizeof(T_AUDIO_REMOTE_BUFFER_FIRST_SYNC_RSP),
                                              false);
    }

    if (ret == false)
    {
        AUDIO_PRINT_INFO2("audio_remote_align_buffer_req_handle: media_buffer_sync rev oldest id %d > local latest id %d",
                          IDs[0], id);
        media_buffer_flush(buffer_ent, media_buffer_ds_pkt_cnt(buffer_ent),
                           BUFFER_DIR_DOWNSTREAM);
        return;
    }

    p_local_media_head = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);
    while (p_local_media_head != NULL && rsp.found == false)
    {
        id = p_local_media_head->upper_seq_number;
        for (i = istart; i < media_pkt_cnt; i++)
        {
            if (IDs[i] == id)
            {
                rsp.match_id = id;
                rsp.found = true;
                break;
            }
        }
        p_local_media_head = p_local_media_head->p_next;
    }

    if (rsp.found == true)
    {
        p_local_media_head = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);
        while (p_local_media_head != NULL)
        {
            if (p_local_media_head->upper_seq_number != rsp.match_id)
            {
                media_buffer_flush(buffer_ent, 1, BUFFER_DIR_DOWNSTREAM);
                p_local_media_head = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);
            }
            else
            {
                break;
            }
        }
    }

    audio_remote_internal_async_msg_relay(remote_instance,
                                          AUDIO_REMOTE_BUFFER_ALIGN_RSP,
                                          &rsp,
                                          sizeof(T_AUDIO_REMOTE_BUFFER_FIRST_SYNC_RSP),
                                          false);

    if (rsp.found == true)
    {
        AUDIO_PRINT_INFO1("audio_remote_align_buffer_req_handle: spk2 has find a match pkt. 0x%x",
                          rsp.match_id);
        audio_remote_set_state(remote_instance, AUDIO_REMOTE_STATE_SYNCED);
        remote_instance->event_cback(AUDIO_REMOTE_EVENT_ALIGNED, NULL, remote_instance->buffer_ent);
    }
    else
    {
        AUDIO_PRINT_TRACE0("audio_remote_align_buffer_req_handle: spk2 can't find a match pkt");
    }
}

void audio_remote_align_buffer_rsp_handle(T_AUDIO_REMOTE_HANDLE  handle,
                                          uint8_t               *pdata,
                                          uint8_t                len)
{
    T_AUDIO_REMOTE_BUFFER_FIRST_SYNC_RSP *p_rsp = (T_AUDIO_REMOTE_BUFFER_FIRST_SYNC_RSP *)pdata;
    uint32_t match_id = p_rsp->match_id;
    T_MEDIA_DATA_HDR *p_local_media_head;
    T_MEDIA_BUFFER_ENTITY *buffer_ent = NULL;
    T_AUDIO_REMOTE_INSTANCE *remote_instance;
    int32_t ret = 0;

    remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;
    buffer_ent = remote_instance->buffer_ent;

    if (buffer_ent == NULL)
    {
        ret = 1;
        goto fail_buffer_ent_empty;
    }

    if (buffer_ent->media_buffer_fst_sync_tid != p_rsp->tid)
    {
        ret = 2;
        goto fail_tid_not_match;
    }

    if ((p_rsp->found) == true)
    {
        p_local_media_head = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);

        if (p_local_media_head == NULL)
        {
            ret = 3;
            goto fail_media_buffer_no_data;
        }

        AUDIO_PRINT_INFO3("audio_remote_align_buffer_rsp_handle: Sec rsp upper_seq %d, Pri upper_seq %d, Pri timestamp 0x%x",
                          match_id, p_local_media_head->upper_seq_number, p_local_media_head->timestamp);

        while (p_local_media_head != NULL)
        {
            if (p_local_media_head->upper_seq_number == match_id)
            {
                break;
            }
            else
            {
                p_local_media_head = p_local_media_head->p_next;
                media_buffer_flush(buffer_ent, 1, BUFFER_DIR_DOWNSTREAM);
            }
        }

        if (p_local_media_head == NULL)
        {
            uint32_t cause = FST_SYNC_DATA_FLUSHED;
            audio_mgr_exception(buffer_ent, MEDIA_BUFFER_FST_SYNC_ERR, 0, cause << 16);
        }
        else
        {
            audio_remote_set_state(remote_instance, AUDIO_REMOTE_STATE_SYNCED);
            remote_instance->event_cback(AUDIO_REMOTE_EVENT_ALIGNED, NULL, remote_instance->buffer_ent);
        }
    }
    else
    {
        AUDIO_PRINT_INFO1("audio_remote_align_buffer_rsp_handle: media_buffer_sync spk2 can't find a match seq cause %u",
                          p_rsp->cause);
        if (remote_instance->state != AUDIO_REMOTE_STATE_UNSYNC)
        {
            uint32_t cause;
            if (p_rsp->cause == FST_SYNC_DATA_EXPIRED)
            {
                cause = FST_SYNC_DATA_EXPIRED;
                audio_mgr_exception(buffer_ent, MEDIA_BUFFER_FST_SYNC_ERR, 0, cause << 16);
            }
            else if (p_rsp->cause == FST_SYNC_BUFFET_INACTIVE)
            {
                if (buffer_ent->media_buffer_fst_sync_tid == p_rsp->tid &&
                    media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_PREQUEUE)
                {
                    media_buffer_flush(buffer_ent, media_buffer_ds_pkt_cnt(buffer_ent) - MEDIA_SYNC_TRIGGER_CNT,
                                       BUFFER_DIR_DOWNSTREAM);
                    if (media_buffer_ds_pkt_cnt(buffer_ent) >= MEDIA_SYNC_TRIGGER_CNT)
                    {
                        audio_remote_align_buffer(remote_instance);
                    }
                }
            }
            else
            {
                cause = FST_SYNC_DATA_NOT_FOUND;
                audio_mgr_exception(buffer_ent, MEDIA_BUFFER_FST_SYNC_ERR, 0, cause << 16);
            }
        }
        else
        {
            media_buffer_flush(buffer_ent,
                               media_buffer_ds_pkt_cnt(buffer_ent) - (MEDIA_SYNC_TRIGGER_CNT - 1), BUFFER_DIR_DOWNSTREAM);
        }
    }

    return;

fail_media_buffer_no_data:
fail_tid_not_match:
fail_buffer_ent_empty:
    AUDIO_PRINT_ERROR1("audio_remote_align_buffer_rsp_handle: failed %d", -ret);
    return;
}


void audio_remote_appoint_playtime(T_AUDIO_REMOTE_HANDLE handle,
                                   uint32_t              set_timing_ms,
                                   bool                  dynamic_latency_on)
{
    T_SYNC_PLAY_MSG msg;
    T_MEDIA_BUFFER_ENTITY *buffer_ent;
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;

    if (handle == NULL)
    {
        return;
    }

    buffer_ent = remote_instance->buffer_ent;
    if (buffer_ent == NULL)
    {
        return;
    }

    msg.time_period = set_timing_ms;
    msg.sync_type = remote_instance->stream_type;
    msg.tid = buffer_ent->media_buffer_fst_sync_tid;

    T_BT_CLK_REF clk_ref;
    uint8_t  clk_idx;
    uint32_t bb_clock_slot;
    uint16_t bb_clock_us;
    clk_ref = bt_piconet_clk_get(BT_CLK_NONE, &clk_idx, &bb_clock_slot, &bb_clock_us);

    if (clk_ref == BT_CLK_NONE)
    {
        bb_clock_slot = 0xffffffff;
    }
    else
    {
        bb_clock_slot += (set_timing_ms * 2 * 1000 / 625); // ms to bt clk
        bb_clock_slot &= 0x0fffffff;
    }
    msg.sync_clk = bb_clock_slot;
    msg.clk_ref = clk_ref;
    AUDIO_PRINT_INFO3("audio_remote_appoint_playtime: buffer_sync clk %u time ms %u, buffer_type %d",
                      bb_clock_slot, set_timing_ms, msg.sync_type);
    audio_remote_internal_sync_msg_relay(remote_instance,
                                         AUDIO_REMOTE_BUFFER_SYNC_PLAY,
                                         &msg,
                                         sizeof(T_SYNC_PLAY_MSG),
                                         REMOTE_TIMER_HIGH_PRECISION,
                                         AUDIO_REMOTE_SYNC_RELAY_DURATION,
                                         false);
}

void audio_remote_relay_cback(uint16_t               event,
                              T_REMOTE_RELAY_STATUS  status,
                              void                  *buf,
                              uint16_t               len)
{
    T_AUDIO_REMOTE_INTERNAL_MSG *inter_msg;
    T_AUDIO_REMOTE_INSTANCE *remote_instance;
    T_MEDIA_BUFFER_ENTITY *buffer_ent;

    inter_msg = (T_AUDIO_REMOTE_INTERNAL_MSG *)buf;
    buffer_ent = media_buffer_find_buffer_by_uuid(inter_msg->stream_type, inter_msg->uuid);
    if (buffer_ent == NULL)
    {
        return;
    }

    remote_instance = (T_AUDIO_REMOTE_INSTANCE *)buffer_ent->audio_remote_handle;
    if (remote_instance == NULL)
    {
        return;
    }

    if (event != AUDIO_REMOTE_BUFFER_SYNC_REQ &&
        event != AUDIO_REMOTE_BUFFER_MSG_RELAY)
    {
        AUDIO_PRINT_INFO3("audio_remote_relay_cback: event 0x%04x, status %u ep %d", event, status,
                          remote_instance->endpoint);
    }

    buf = inter_msg->msg;
    len -= sizeof(T_AUDIO_REMOTE_INTERNAL_MSG);

    switch (event)
    {
    case AUDIO_REMOTE_BUFFER_MSG_RELAY:
        {
            T_AUDIO_REMOTE_BUFFER_MSG_RELAY *msg = (T_AUDIO_REMOTE_BUFFER_MSG_RELAY *)buf;
            remote_instance->relay_cback(msg->msg_id, msg->msg, status, buffer_ent);
        }
        break;

    case AUDIO_REMOTE_BUFFER_SEAMLESS_JOIN_INFO:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                T_BT_CLK_REF clk_ref;
                uint32_t bb_clock_slot;
                uint16_t bb_clock_us;
                uint8_t  clk_idx;
                T_AUDIO_MSG_JOIN_INFO *p_cmd = (T_AUDIO_MSG_JOIN_INFO *)buf;
                T_MEDIA_DATA_HDR *p_packet;

                if (remote_session_role_get() != REMOTE_SESSION_ROLE_SECONDARY)
                {
                    break;
                }

                sys_timer_delete(remote_instance->join_abort_timer);
                remote_instance->join_abort_timer = NULL;
                buffer_ent->media_buffer_fst_sync_tid = p_cmd->tid;

                p_packet = media_buffer_peek(buffer_ent, -1, BUFFER_DIR_DOWNSTREAM);
                if (p_packet != NULL)
                {
                    if (cacu_diff_u16((uint16_t)p_cmd->packet_id, p_packet->upper_seq_number) > 0)
                    {
                        AUDIO_PRINT_INFO1("AUDIO_REMOTE_EVT_SEAMLESS_JOIN_INFO: sec has not receives 0x%x",
                                          p_cmd->packet_id);
                        audio_remote_set_state(remote_instance, AUDIO_REMOTE_STATE_SEAMLESS_JOIN);
                        jitter_buffer_asrc_pid_block(buffer_ent->jitter_buffer_handle);
                        remote_instance->join_clk = p_cmd->join_clk;
                        remote_instance->join_packet_id = p_cmd->packet_id;
                        remote_instance->join_local_seq = p_cmd->packet_local_seq;
                        remote_instance->join_frame_counter = p_cmd->packet_frame_counter;
                        break;
                    }
                }

                p_packet = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);

                while (p_packet != NULL)
                {
                    if (p_cmd->packet_id == p_packet->upper_seq_number)
                    {
                        break;
                    }

                    media_buffer_flush(buffer_ent, 1, BUFFER_DIR_DOWNSTREAM);
                    p_packet = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);
                }

                if (p_packet == NULL)
                {
                    audio_remote_seamless_join_retry(remote_instance, MEDIA_BUFFER_SEAMLESS_MISS_PKT);
                    break;
                }

                clk_ref = bt_piconet_clk_get(BT_CLK_SNIFFING, &clk_idx, &bb_clock_slot, &bb_clock_us);
                AUDIO_PRINT_INFO3("AUDIO_REMOTE_EVT_SEAMLESS_JOIN_INFO: seamless_join_info pkt_id %u, join_clk 0x%x, type %u",
                                  p_cmd->packet_id, p_cmd->join_clk, p_cmd->stream_type);

                if (clk_ref != BT_CLK_NONE &&
                    bt_clk_compare(p_cmd->join_clk, bb_clock_slot + 20 * 2 * 1000 / 625))
                {
                    uint32_t time_us;

                    time_us = (((p_cmd->join_clk << 4) - ((bb_clock_slot + 20 * 2 * 1000 / 625) << 4)) >> 4) * 625 / 2;
                    time_us = (time_us > 50000) ? 50000 : time_us;
                    audio_remote_internal_sync_msg_relay(remote_instance,
                                                         AUDIO_REMOTE_BUFFER_SEAMLESS_JOIN_PLAY,
                                                         p_cmd,
                                                         sizeof(T_AUDIO_MSG_JOIN_INFO),
                                                         REMOTE_TIMER_HIGH_PRECISION,
                                                         time_us / 1000,
                                                         false);
                    media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_SYNC);
                    audio_remote_set_state(remote_instance, AUDIO_REMOTE_STATE_SEAMLESS_JOIN);
                    jitter_buffer_asrc_pid_block(buffer_ent->jitter_buffer_handle);
                }
                else
                {
                    audio_remote_seamless_join_retry(remote_instance, MEDIA_BUFFER_ERR_CLK);
                    break;
                }
            }
        }
        break;

    case AUDIO_REMOTE_BUFFER_SYNC_REQ:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (audio_remote_get_state(remote_instance) == AUDIO_REMOTE_STATE_SYNCED)
                {
                    bool rec_found;
                    rec_found = audio_remote_buffer_sync_req_handle(remote_instance, (uint8_t *)buf, len);
                    remote_instance->event_cback(AUDIO_REMOTE_EVENT_BUFFER_SYNC, &rec_found,
                                                 remote_instance->buffer_ent);
                }
            }
        }
        break;

    case AUDIO_REMOTE_BUFFER_NOT_SEAMLESS_JOIN:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                sys_timer_delete(remote_instance->join_abort_timer);
                remote_instance->join_abort_timer = NULL;
            }
        }
        break;

    case AUDIO_REMOTE_BUFFER_SEC_ALIGNING:
        {
            T_AUDIO_STREAM_TYPE sync_type = *(T_AUDIO_STREAM_TYPE *)buf;
            T_AUDIO_STREAM_MODE mode;
            T_AUDIO_FORMAT_TYPE type;

            media_buffer_get_mode(buffer_ent, &mode);
            media_buffer_get_format_type(buffer_ent, &type);

            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_PLAY)
                {
                    if (mode == AUDIO_STREAM_MODE_LOW_LATENCY ||
                        mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
                    {
                        audio_mgr_dispatch(AUDIO_MSG_BUFFER_RESET, buffer_ent);
                    }
                    else
                    {
                        if (sync_type == AUDIO_STREAM_TYPE_PLAYBACK ||
                            sync_type == AUDIO_STREAM_TYPE_VOICE)
                        {
                            if (audio_remote_db->seamless_join)
                            {
                                if (buffer_ent->local_audio_sync)
                                {
                                    jitter_buffer_asrc_pid_block(buffer_ent->jitter_buffer_handle);
                                    audio_remote_seamless_join_info_req();
                                }
                                else
                                {
                                    if (audio_remote_get_state(remote_instance) == AUDIO_REMOTE_STATE_SYNCED)
                                    {
                                        audio_mgr_exception(buffer_ent, MEDIA_BUFFER_SEAMLESS_BFE_SYNC, 0, 0);
                                        audio_remote_internal_async_msg_relay(remote_instance,
                                                                              AUDIO_REMOTE_BUFFER_NOT_SEAMLESS_JOIN,
                                                                              &(buffer_ent->stream_type),
                                                                              1,
                                                                              false);
                                    }
                                    else
                                    {
                                        remote_instance->seamless_join_retry_cnt++;
                                        if (remote_instance->seamless_join_retry_cnt > AUDIO_REMOTE_JOINING_RETRY_CNT)
                                        {
                                            audio_mgr_exception(buffer_ent, MEDIA_BUFFER_SEAMLESS_BFE_SYNC, 0,
                                                                remote_instance->seamless_join_retry_cnt);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if (audio_remote_db->force_join == false)
                                {
                                    remote_instance->seamless_join_retry_cnt++;
                                    if (remote_instance->seamless_join_retry_cnt > AUDIO_REMOTE_JOINING_RETRY_CNT)
                                    {
                                        audio_mgr_exception(buffer_ent, MEDIA_BUFFER_SEAMLESS_REFUSE, 0, 0);
                                        audio_remote_internal_async_msg_relay(remote_instance,
                                                                              AUDIO_REMOTE_BUFFER_NOT_SEAMLESS_JOIN,
                                                                              &sync_type,
                                                                              1,
                                                                              false);
                                    }
                                }
                            }
                        }
                        else
                        {
                            audio_mgr_exception(buffer_ent, MEDIA_BUFFER_SEAMLESS_REFUSE, 0, 0);
                            audio_remote_internal_async_msg_relay(remote_instance,
                                                                  AUDIO_REMOTE_BUFFER_NOT_SEAMLESS_JOIN,
                                                                  &sync_type,
                                                                  1,
                                                                  false);
                        }
                    }
                }
                else if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_SYNC)
                {
                    audio_mgr_dispatch(AUDIO_MSG_BUFFER_RESET, buffer_ent);
                    audio_remote_internal_async_msg_relay(remote_instance,
                                                          AUDIO_REMOTE_BUFFER_NOT_SEAMLESS_JOIN,
                                                          &sync_type,
                                                          1,
                                                          false);
                }
                else
                {
                    audio_remote_internal_async_msg_relay(remote_instance,
                                                          AUDIO_REMOTE_BUFFER_NOT_SEAMLESS_JOIN,
                                                          &sync_type,
                                                          1,
                                                          false);
                }
            }
        }
        break;

    case AUDIO_REMOTE_BUFFER_SEAMLESS_JOIN_PLAY:
        {
            T_AUDIO_MSG_JOIN_INFO *p_cmd = (T_AUDIO_MSG_JOIN_INFO *)buf;

            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT || status == REMOTE_RELAY_STATUS_SYNC_EXPIRED)
            {
                if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    if (audio_remote_get_state(remote_instance) ==  AUDIO_REMOTE_STATE_SEAMLESS_JOIN)
                    {
                        audio_remote_reset(remote_instance);
                    }
                    if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_PLAY &&
                        audio_remote_get_state(remote_instance) ==  AUDIO_REMOTE_STATE_UNSYNC)
                    {
                        audio_remote_set_state(remote_instance, AUDIO_REMOTE_STATE_SYNCED);
                    }
                    else
                    {
                        audio_mgr_exception(buffer_ent, MEDIA_BUFFER_REMOTE_STATE_ERR, 0,
                                            (remote_instance->state << 16) | media_buffer_state_get(buffer_ent));
                    }

                }
                else if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    T_BT_CLK_REF clk_ref;
                    uint8_t  clk_idx;
                    uint32_t bb_clock_slot;
                    uint16_t bb_clock_us;
                    T_MEDIA_DATA_HDR *p_packet;

                    if (audio_remote_get_state(remote_instance) != AUDIO_REMOTE_STATE_SEAMLESS_JOIN)
                    {
                        audio_mgr_exception(buffer_ent, MEDIA_BUFFER_REMOTE_STATE_ERR, 0, 0);
                        return;
                    }

                    p_packet = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);

                    if (p_cmd->packet_id != p_packet->upper_seq_number)
                    {
                        audio_mgr_exception(buffer_ent, MEDIA_BUFFER_SEAMLESS_MISS_PKT, 0, 0);
                        return;
                    }

                    clk_ref = bt_piconet_clk_get(BT_CLK_SNIFFING, &clk_idx, &bb_clock_slot, &bb_clock_us);

                    if (clk_ref != BT_CLK_NONE &&
                        bt_clk_compare(p_cmd->join_clk,
                                       bb_clock_slot + sync_play_margin_table[buffer_ent->format_info.type] * 2 * 1000 / 625))
                    {
                        audio_path_timestamp_set(buffer_ent->path_handle, clk_idx, p_cmd->join_clk,
                                                 true);
                        audio_path_synchronization_join_set(NULL, 2);
                        media_buffer_set_downstream_info(buffer_ent, p_cmd->packet_frame_counter - p_packet->frame_number,
                                                         p_cmd->packet_local_seq - 1);
                        audio_remote_set_state(remote_instance, AUDIO_REMOTE_STATE_SYNCED);

                        remote_instance->seamless_join_retry_cnt = 0;
                        remote_instance->event_cback(AUDIO_REMOTE_EVENT_SYNC_PLAY_START, NULL, remote_instance->buffer_ent);
                    }
                    else
                    {
                        audio_mgr_exception(buffer_ent, MEDIA_BUFFER_MISS_PLAYTIME, 0, 0);
                    }
                }
            }
            else if (status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED)
            {
                audio_mgr_exception(buffer_ent, MEDIA_BUFFER_MISS_PLAYTIME, 0, 0);
            }
        }
        break;

    case AUDIO_REMOTE_BUFFER_ALIGN_REQ:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                audio_remote_align_buffer_req_handle(remote_instance, (uint8_t *)buf, len);
            }
        }
        break;

    case AUDIO_REMOTE_BUFFER_ALIGN_RSP:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                audio_remote_align_buffer_rsp_handle(remote_instance, (uint8_t *)buf, len);
            }
        }
        break;

    case AUDIO_REMOTE_BUFFER_SYNC_PLAY:
        {
            T_SYNC_PLAY_MSG *msg = (T_SYNC_PLAY_MSG *)buf;

            if (status == REMOTE_RELAY_STATUS_SYNC_RCVD)
            {
                if (remote_instance->state != AUDIO_REMOTE_STATE_SYNCED)
                {
                    audio_mgr_exception(buffer_ent, MEDIA_BUFFER_PLAY_WO_SYNC, 0, 0);
                }
                else
                {
                    remote_instance->event_cback(AUDIO_REMOTE_EVENT_SYNC_PLAY_RSV, &msg->tid,
                                                 remote_instance->buffer_ent);
                }
            }
            else if (status == REMOTE_RELAY_STATUS_SYNC_TOUT || status == REMOTE_RELAY_STATUS_SYNC_EXPIRED)
            {
                T_BT_CLK_REF clk_ref;
                uint32_t bb_clock_slot;
                uint16_t bb_clock_us;
                uint8_t  clk_idx;
                uint32_t bb_clock_rcv;

                if (status == REMOTE_RELAY_STATUS_SYNC_EXPIRED)
                {
                    remote_instance->event_cback(AUDIO_REMOTE_EVENT_SYNC_PLAY_RSV, &msg->tid,
                                                 remote_instance->buffer_ent);
                }

                bb_clock_rcv = msg->sync_clk;
                clk_ref = bt_piconet_clk_get((T_BT_CLK_REF)(msg->clk_ref), &clk_idx, &bb_clock_slot, &bb_clock_us);

                if (clk_ref != BT_CLK_NONE &&
                    bt_clk_compare(bb_clock_rcv,
                                   bb_clock_slot + sync_play_margin_table[buffer_ent->format_info.type] * 2 * 1000 / 625))
                {
                    AUDIO_PRINT_TRACE1("audio_remote_relay_cback: media_buffer_sync play tout state %d",
                                       media_buffer_state_get(buffer_ent));
                    if (audio_path_is_running(buffer_ent->path_handle))
                    {
                        if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_SYNC)
                        {
                            if (msg->tid == buffer_ent->media_buffer_fst_sync_tid)
                            {
                                media_buffer_clear_downstream_info(buffer_ent);
                                audio_path_timestamp_set(buffer_ent->path_handle, clk_idx, bb_clock_rcv, true);
                                remote_instance->event_cback(AUDIO_REMOTE_EVENT_SYNC_PLAY_START, NULL, remote_instance->buffer_ent);
                            }
                            else
                            {
                                AUDIO_PRINT_WARN2("tid is error when wait to play %u %u", msg->tid,
                                                  buffer_ent->media_buffer_fst_sync_tid);
                            }
                        }
                    }
                    else
                    {
                        audio_mgr_exception(buffer_ent, MEDIA_BUFFER_PATH_NOT_READY, 0, 0);
                    }
                }
                else
                {
                    audio_mgr_exception(buffer_ent, MEDIA_BUFFER_PLAY_OVERTIME, 0, bb_clock_rcv);
                }
            }
            else if (status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED)
            {
                audio_mgr_exception(buffer_ent, MEDIA_BUFFER_PLAY_OVERTIME, 0, 0);
            }
        }
        break;

    case AUDIO_REMOTE_BUFFER_LOW_LATENCY_SYNC_PLAY:
        {
            T_AUDIO_MSG_LOW_LATENCY_SYNC_PLAY *p_cmd = (T_AUDIO_MSG_LOW_LATENCY_SYNC_PLAY *)buf;
            T_AUDIO_REMOTE_LATENCY_INFO msg;

            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                AUDIO_PRINT_INFO4("AUDIO_REMOTE_LOW_LATENCY_SYNC_PLAY: tid %u, upper_seq %u, sync_clk 0x%0x, dynamic_latency_on %u",
                                  p_cmd->tid, p_cmd->upper_seq, p_cmd->sync_clk, p_cmd->dynamic_latency_on);
                T_MEDIA_DATA_HDR *p_local_media_head;

                if (audio_path_is_running(buffer_ent->path_handle) == false)
                {
                    audio_mgr_exception(buffer_ent, MEDIA_BUFFER_PATH_NOT_READY, 0, 0);
                    break;
                }

                if (p_cmd->dynamic_latency_on == false)
                {
                    msg.dynamic_latency_on = false;
                    msg.plc_count = 0;
                    msg.latency = p_cmd->latency;
                    msg.latency_override = p_cmd->latency_override;
                    msg.latency_back_up = p_cmd->latency_backup;
                }
                else
                {
                    msg.dynamic_latency_on = true;
                    msg.plc_count = p_cmd->plc_count;
                    msg.latency = p_cmd->latency;
                    msg.latency_override = p_cmd->latency_override;
                    msg.latency_back_up = p_cmd->latency_backup;
                }

                p_local_media_head = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);

                while (p_local_media_head != NULL)
                {
                    if (p_local_media_head->upper_seq_number == p_cmd->upper_seq)
                    {
                        break;
                    }
                    p_local_media_head = p_local_media_head->p_next;
                }
                if (p_local_media_head == NULL)
                {
                    p_local_media_head = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);
                    if ((uint16_t)(p_cmd->upper_seq + 1) == p_local_media_head->upper_seq_number)
                    {
                        media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_SYNC);
                        buffer_ent->media_buffer_fst_sync_tid = p_cmd->tid;
                        audio_remote_set_state(buffer_ent->audio_remote_handle, AUDIO_REMOTE_STATE_SYNCED);
                    }
                    else
                    {
                        p_local_media_head = media_buffer_peek(buffer_ent, -1, BUFFER_DIR_DOWNSTREAM);
                        if (p_cmd->upper_seq == (uint16_t)(p_local_media_head->upper_seq_number + 1))
                        {
                            media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_SYNC);
                            buffer_ent->media_buffer_fst_sync_tid = p_cmd->tid;
                            audio_remote_set_state(buffer_ent->audio_remote_handle, AUDIO_REMOTE_STATE_SYNCED);
                            media_buffer_flush(buffer_ent, media_buffer_ds_pkt_cnt(buffer_ent),
                                               BUFFER_DIR_DOWNSTREAM);
                        }
                        else
                        {
                            audio_mgr_exception(buffer_ent, MEDIA_BUFFER_MISS_SPEC_PKT, 0, 0);
                            break;
                        }
                    }
                }
                else
                {
                    p_local_media_head = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);
                    while (p_local_media_head != NULL)
                    {
                        if (p_local_media_head->upper_seq_number == p_cmd->upper_seq)
                        {
                            break;
                        }
                        media_buffer_flush(buffer_ent, 1, BUFFER_DIR_DOWNSTREAM);
                        p_local_media_head = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);
                    }
                    media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_SYNC);
                    buffer_ent->media_buffer_fst_sync_tid = p_cmd->tid;
                    audio_remote_set_state(buffer_ent->audio_remote_handle, AUDIO_REMOTE_STATE_SYNCED);
                }

                if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_SYNC)
                {
                    T_BT_CLK_REF clk_ref;
                    uint8_t  clk_idx;
                    uint32_t bb_clock_slot;
                    uint16_t bb_clock_us;
                    uint32_t bb_clock_rcv;
                    uint8_t  protect_ms;

                    bb_clock_rcv = p_cmd->sync_clk;
                    clk_ref = bt_piconet_clk_get((T_BT_CLK_REF)(p_cmd->clk_ref), &clk_idx, &bb_clock_slot,
                                                 &bb_clock_us);

                    if (buffer_ent->format_info.type == AUDIO_FORMAT_TYPE_SBC)
                    {
                        protect_ms = 3;
                    }
                    else
                    {
                        protect_ms = 10;
                    }

                    if (clk_ref != BT_CLK_NONE &&
                        bt_clk_compare(bb_clock_rcv, bb_clock_slot + protect_ms * 2 * 1000 / 625))
                    {
                        AUDIO_PRINT_TRACE1("audio_remote_relay_cback: gaming_mode media_buffer_sync play tout state %d",
                                           media_buffer_state_get(buffer_ent));

                        audio_path_timestamp_set(buffer_ent->path_handle, clk_idx, bb_clock_rcv, true);
                        media_buffer_clear_downstream_info(buffer_ent);
                        remote_instance->seq_diff = 0;
                        remote_instance->frame_diff = 0;
                        remote_instance->event_cback(AUDIO_REMOTE_EVENT_SYNC_PLAY_START, &msg, remote_instance->buffer_ent);
                    }
                    else
                    {
                        audio_mgr_exception(buffer_ent, MEDIA_BUFFER_PLAY_OVERTIME, 0, bb_clock_rcv);
                    }
                }
            }
        }
        break;

    case AUDIO_REMOTE_BUFFER_ULTRA_LOW_LATENCY_SYNC_PLAY:
        {
            T_AUDIO_MSG_LOW_LATENCY_SYNC_PLAY *p_cmd = (T_AUDIO_MSG_LOW_LATENCY_SYNC_PLAY *)buf;
            T_AUDIO_REMOTE_LATENCY_INFO msg;

            if (status == REMOTE_RELAY_STATUS_SYNC_RCVD)
            {
                buffer_ent->media_buffer_fst_sync_tid = p_cmd->tid;

                if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_PREQUEUE)
                {
                    media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_SYNC);
                }
                else
                {
                    audio_mgr_exception(buffer_ent, MEDIA_BUFFER_STATE_ERR, 0, 0);
                }
            }
            else if (status == REMOTE_RELAY_STATUS_SYNC_TOUT || status == REMOTE_RELAY_STATUS_SYNC_EXPIRED)
            {
                if (audio_path_is_running(buffer_ent->path_handle) == false)
                {
                    audio_mgr_exception(buffer_ent, MEDIA_BUFFER_PATH_NOT_READY, 0, 0);
                    break;
                }

                if (p_cmd->dynamic_latency_on == false)
                {
                    msg.dynamic_latency_on = false;
                    msg.plc_count = 0;
                    msg.latency = p_cmd->latency;
                    msg.latency_override = p_cmd->latency_override;
                    msg.latency_back_up = p_cmd->latency_backup;
                }
                else
                {
                    msg.dynamic_latency_on = true;
                    msg.plc_count = p_cmd->plc_count;
                    msg.latency = p_cmd->latency;
                    msg.latency_override = p_cmd->latency_override;
                    msg.latency_back_up = p_cmd->latency_backup;
                }

                media_buffer_flush(buffer_ent, media_buffer_ds_pkt_cnt(buffer_ent) - 1,
                                   BUFFER_DIR_DOWNSTREAM);

                if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_SYNC)
                {
                    T_BT_CLK_REF clk_ref;
                    uint32_t bb_clock_slot;
                    uint16_t bb_clock_us;
                    uint8_t  clk_idx;
                    uint32_t bb_clock_rcv;
                    uint8_t  protect_ms;

                    bb_clock_rcv = p_cmd->sync_clk;
                    clk_ref = bt_piconet_clk_get((T_BT_CLK_REF)(p_cmd->clk_ref), &clk_idx, &bb_clock_slot,
                                                 &bb_clock_us);

                    if (buffer_ent->format_info.type == AUDIO_FORMAT_TYPE_SBC)
                    {
                        protect_ms = 3;
                    }
                    else
                    {
                        protect_ms = 10;
                    }

                    if (clk_ref != BT_CLK_NONE &&
                        bt_clk_compare(bb_clock_rcv, bb_clock_slot + protect_ms * 2 * 1000 / 625))
                    {
                        audio_path_timestamp_set(buffer_ent->path_handle, clk_idx, bb_clock_rcv, true);
                        media_buffer_clear_downstream_info(buffer_ent);
                        remote_instance->seq_diff = 0;
                        remote_instance->frame_diff = 0;
                        audio_remote_set_state(buffer_ent->audio_remote_handle, AUDIO_REMOTE_STATE_SYNCED);
                        remote_instance->event_cback(AUDIO_REMOTE_EVENT_SYNC_PLAY_START, &msg, remote_instance->buffer_ent);
                    }
                    else
                    {
                        audio_mgr_exception(buffer_ent, MEDIA_BUFFER_PLAY_OVERTIME, 0, bb_clock_rcv);
                    }
                }
            }
            else if (status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED)
            {
                audio_mgr_exception(buffer_ent, MEDIA_BUFFER_MISS_PLAYTIME, 0, 0);
            }
        }
        break;

    default:
        break;
    }
}

void audio_remote_ultra_low_latency_sync_play(T_AUDIO_REMOTE_HANDLE handle,
                                              bool                  dynamic_latency_on)
{
    T_BT_CLK_REF clk_ref;
    uint8_t  clk_idx;
    uint32_t bb_clock_slot;
    uint32_t bb_clock_slot_sync;
    uint16_t bb_clock_us;
    T_AUDIO_MSG_LOW_LATENCY_SYNC_PLAY cmd;
    int32_t latency_offset_ms;
    T_MEDIA_BUFFER_ENTITY *buffer_ent;
    T_AUDIO_REMOTE_INSTANCE *remote_instance;

    if (handle == NULL)
    {
        return;
    }

    remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;
    buffer_ent = remote_instance->buffer_ent;

    if (buffer_ent == NULL)
    {
        return;
    }

    latency_offset_ms = buffer_ent->latency_back_up + media_buffer_cacu_frames_duration(
                            buffer_ent->plc_count,
                            &buffer_ent->format_info);

    clk_ref = bt_piconet_clk_get(BT_CLK_SNIFFING, &clk_idx, &bb_clock_slot, &bb_clock_us);
    if (clk_ref == BT_CLK_NONE)
    {
        media_buffer_flush(buffer_ent, media_buffer_ds_pkt_cnt(buffer_ent),
                           BUFFER_DIR_DOWNSTREAM);
        audio_mgr_exception(buffer_ent, MEDIA_BUFFER_ERR_SNIFF_CLK, 0, 0);
        return;
    }

    bb_clock_slot_sync = (bb_clock_slot + (latency_offset_ms + 100) * 1000 * 2 / 625) & 0x0fffffff;

    AUDIO_PRINT_INFO4("audio_remote_ultra_low_latency_sync_play: sync_clk %u local_clk %u latency offset ms %d dynamic lat %u",
                      bb_clock_slot_sync,
                      bb_clock_slot, latency_offset_ms, dynamic_latency_on);
    cmd.sync_clk = bb_clock_slot_sync;

    buffer_ent->media_buffer_fst_sync_tid++;
    cmd.tid = buffer_ent->media_buffer_fst_sync_tid;
    cmd.clk_ref = BT_CLK_SNIFFING;
    cmd.dynamic_latency_on = dynamic_latency_on;
    cmd.plc_count = buffer_ent->plc_count;
    cmd.latency = buffer_ent->latency;
    cmd.latency_override = buffer_ent->latency_override;
    cmd.latency_backup  = buffer_ent->latency_back_up;

    audio_remote_internal_sync_msg_relay(remote_instance,
                                         AUDIO_REMOTE_BUFFER_ULTRA_LOW_LATENCY_SYNC_PLAY,
                                         &cmd,
                                         sizeof(T_AUDIO_MSG_LOW_LATENCY_SYNC_PLAY),
                                         REMOTE_TIMER_HIGH_PRECISION,
                                         100,
                                         false);
}

void audio_remote_low_latency_sync_play(T_AUDIO_REMOTE_HANDLE handle,
                                        bool                  dynamic_latency_on)
{
    T_BT_CLK_REF clk_ref;
    uint32_t bb_clock_slot;
    uint32_t bb_clock_slot_sync;
    uint16_t bb_clock_us;
    uint8_t  clk_idx;
    T_AUDIO_MSG_LOW_LATENCY_SYNC_PLAY cmd;
    int32_t latency_offset_ms;
    T_MEDIA_DATA_HDR *p_media_packet;
    T_MEDIA_BUFFER_ENTITY *buffer_ent;
    T_AUDIO_REMOTE_INSTANCE *remote_instance;

    if (handle == NULL)
    {
        return;
    }

    remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;
    buffer_ent = remote_instance->buffer_ent;

    if (buffer_ent == NULL)
    {
        return;
    }
    latency_offset_ms = buffer_ent->latency;
    clk_ref = bt_piconet_clk_get(BT_CLK_SNIFFING, &clk_idx, &bb_clock_slot, &bb_clock_us);

    if (clk_ref == BT_CLK_NONE)
    {
        media_buffer_flush(buffer_ent, media_buffer_ds_pkt_cnt(buffer_ent),
                           BUFFER_DIR_DOWNSTREAM);
        audio_mgr_exception(buffer_ent, MEDIA_BUFFER_ERR_SNIFF_CLK, 0, 0);
        return;
    }

    bb_clock_slot_sync = (buffer_ent->last_ts + latency_offset_ms * 1000 * 2 / 625) & 0x0fffffff;

    AUDIO_PRINT_INFO5("audio_remote_low_latency_sync_play: sync_clk %u local_clk %u latency offset ms %d dynamic lat %u latency_override %u",
                      bb_clock_slot_sync,
                      bb_clock_slot, latency_offset_ms, dynamic_latency_on,
                      buffer_ent->latency_override);
    cmd.sync_clk = bb_clock_slot_sync;

    media_buffer_flush(buffer_ent, media_buffer_ds_pkt_cnt(buffer_ent) - 1,
                       BUFFER_DIR_DOWNSTREAM);

    p_media_packet = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);
    if (p_media_packet == NULL)
    {
        audio_mgr_exception(buffer_ent, MEDIA_BUFFER_STATE_ERR, 0, 0);
        return;
    }

    buffer_ent->media_buffer_fst_sync_tid++;
    cmd.upper_seq = p_media_packet->upper_seq_number;
    cmd.tid = buffer_ent->media_buffer_fst_sync_tid;
    cmd.clk_ref = BT_CLK_SNIFFING;
    cmd.dynamic_latency_on = dynamic_latency_on;
    cmd.latency_override = buffer_ent->latency_override;
    cmd.latency_backup  = buffer_ent->latency_back_up;
    cmd.plc_count = buffer_ent->plc_count;
    cmd.latency = buffer_ent->latency;

    if (audio_remote_internal_async_msg_relay(remote_instance,
                                              AUDIO_REMOTE_BUFFER_LOW_LATENCY_SYNC_PLAY,
                                              &cmd,
                                              sizeof(T_AUDIO_MSG_LOW_LATENCY_SYNC_PLAY),
                                              false))
    {
        audio_path_timestamp_set(buffer_ent->path_handle, clk_idx, bb_clock_slot_sync, true);
        media_buffer_clear_downstream_info(buffer_ent);
        remote_instance->seq_diff = 0;
        remote_instance->frame_diff = 0;
        audio_remote_set_state(buffer_ent->audio_remote_handle, AUDIO_REMOTE_STATE_SYNCED);
        remote_instance->event_cback(AUDIO_REMOTE_EVENT_SYNC_PLAY_START, NULL, remote_instance->buffer_ent);
    }
}

bool audio_remote_is_buffer_unsync(T_AUDIO_REMOTE_HANDLE handle)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;

    if (handle == NULL)
    {
        return false;
    }

    if (remote_instance->seq_diff != 0 || remote_instance->frame_diff != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

uint16_t audio_remote_cacu_frame_len(uint16_t payload_length,
                                     uint8_t  frame_number)
{
    return (payload_length - sizeof(T_AUDIO_PATH_DATA_HDR)) / frame_number;
}

static bool audio_remote_buffer_PLC_policy_1(T_AUDIO_REMOTE_INSTANCE *remote_instance)
{
    T_MEDIA_DATA_HDR *p_media_packet;
    T_AUDIO_PATH_DATA_HDR *p_stream_head;
    uint16_t download_frames = 0;
    uint16_t download_seq = 0;
    uint16_t download_frames_counter = 0;
    uint16_t download_frames_length = 0;
    uint16_t frame_len = 0;
    bool ret = true;
    T_MEDIA_BUFFER_ENTITY *buffer_ent = remote_instance->buffer_ent;

    if (buffer_ent->last_packet_ptr == NULL)
    {
        p_media_packet = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);
        if (p_media_packet == NULL)
        {
            AUDIO_PRINT_ERROR0("media_buffer_plc: policy 1, no more packets");
            return false;
        }
    }
    else
    {
        p_media_packet = buffer_ent->last_packet_ptr;
    }

    p_stream_head = (T_AUDIO_PATH_DATA_HDR *)(p_media_packet->p_data);

    if (p_stream_head->status != AUDIO_STREAM_STATUS_CORRECT)
    {
        return false;
    }

    frame_len = audio_remote_cacu_frame_len(p_media_packet->payload_length,
                                            p_media_packet->frame_number);

    download_frames =    remote_instance->frame_diff / remote_instance->seq_diff;
    download_seq = buffer_ent->ds_local_seq_last + 1;
    download_frames_counter = buffer_ent->ds_frame_counter_last + download_frames;
    download_frames_length = frame_len * download_frames;

    if (buffer_ent->format_info.type == AUDIO_FORMAT_TYPE_SBC)
    {
        T_MEDIA_DATA_HDR *p_make_up ;
        T_AUDIO_PATH_DATA_HDR *p_stream_head_fab;

        uint16_t size = sizeof(T_MEDIA_DATA_HDR) + sizeof(T_AUDIO_PATH_DATA_HDR) + download_frames_length;
        uint16_t media_data_len = p_stream_head->payload_length;
        uint8_t *media_data_ptr = p_stream_head->payload;

        p_make_up = media_buffer_get(buffer_ent->pool_handle, size);
        if (p_make_up == NULL)
        {
            return false;
        }
        p_make_up->payload_length = download_frames_length + sizeof(T_AUDIO_PATH_DATA_HDR);
        p_make_up->frame_counter = download_frames_counter;
        p_make_up->frame_number = download_frames ;
        p_make_up->local_seq_number = download_seq;
        p_make_up->upper_seq_number = p_media_packet->upper_seq_number;

        p_stream_head_fab = (T_AUDIO_PATH_DATA_HDR *)(p_make_up->p_data);

        p_stream_head_fab->timestamp = p_media_packet->timestamp;
        p_stream_head_fab->status = AUDIO_STREAM_STATUS_LOST;
        p_stream_head_fab->seq_num = download_seq;
        p_stream_head_fab->frame_number = download_frames;
        p_stream_head_fab->frame_count = download_frames_counter;
        p_stream_head_fab->payload_length = download_frames_length;

        if (download_frames > p_media_packet->frame_number)
        {
            //Make up more frames
            uint8_t make_up_frame_num = download_frames - p_media_packet->frame_number;
            uint8_t *p_more_payload;
            uint8_t temp_frame_num = 0;
            uint16_t temp_frame_len = 0;

            memcpy(p_stream_head_fab->payload, media_data_ptr, media_data_len);
            p_more_payload = &p_stream_head_fab->payload[media_data_len];
            while (make_up_frame_num)
            {
                temp_frame_num = (make_up_frame_num < p_media_packet->frame_number) ? make_up_frame_num :
                                 p_media_packet->frame_number; // min(p_media_packet->frame_number,make_up_frame_num)
                temp_frame_len = frame_len * temp_frame_num;
                memcpy((void *)p_more_payload, media_data_ptr, temp_frame_len); // get it from first frame

                p_more_payload += temp_frame_len;
                make_up_frame_num -= temp_frame_num;
            }

            if (audio_path_data_send(buffer_ent->path_handle,
                                     (T_AUDIO_PATH_DATA_HDR *)p_make_up->p_data,
                                     p_make_up->payload_length))
            {
                AUDIO_PRINT_WARN5("media_buffer_plc: download_frames %d, h2d_seq %u, packet_len %d, frames_counter %d | %d",
                                  download_frames, p_stream_head_fab->seq_num, p_make_up->payload_length,
                                  buffer_ent->ds_frame_counter_last, download_frames_counter);
                buffer_ent->ds_local_seq_last++;
                buffer_ent->ds_frame_counter_last = download_frames_counter;
                audio_remote_record_push(buffer_ent->audio_remote_handle, p_make_up);
                remote_instance->frame_diff -= download_frames;
                remote_instance->seq_diff -= 1;
            }
            else
            {
                ret = false;
            }
        }
        else
        {
            memcpy(p_stream_head_fab->payload, media_data_ptr, download_frames_length);

            if (audio_path_data_send(buffer_ent->path_handle,
                                     (T_AUDIO_PATH_DATA_HDR *)p_make_up->p_data,
                                     p_make_up->payload_length))
            {
                AUDIO_PRINT_WARN5("media_buffer_plc: download_length %d, h2d_seq %d, packet_len %d, frames_counter %d | %d",
                                  p_make_up->payload_length, p_stream_head_fab->seq_num, p_media_packet->payload_length,
                                  buffer_ent->ds_frame_counter_last, download_frames_counter);
                buffer_ent->ds_local_seq_last++;
                buffer_ent->ds_frame_counter_last = download_frames_counter;
                audio_remote_record_push(buffer_ent->audio_remote_handle, p_make_up);
                remote_instance->frame_diff -= download_frames;
                remote_instance->seq_diff -= 1;
            }
            else
            {
                ret = false;
            }
        }
        media_buffer_put(buffer_ent->pool_handle, p_make_up);
    }
    else if (buffer_ent->format_info.type == AUDIO_FORMAT_TYPE_AAC)
    {
        T_MEDIA_DATA_HDR packet_head;
        T_AUDIO_PATH_DATA_HDR *p_stream_head_fab;
        T_AUDIO_PATH_DATA_HDR *p_stream_head = (T_AUDIO_PATH_DATA_HDR *)p_media_packet->p_data;
        uint8_t             *p_data;
        uint16_t media_data_len;
        uint8_t *media_data_ptr;

        media_data_len = p_stream_head->payload_length;
        media_data_ptr = p_stream_head->payload;

        packet_head.upper_seq_number = p_media_packet->upper_seq_number;
        packet_head.local_seq_number = download_seq;
        packet_head.frame_counter = download_frames_counter;
        packet_head.frame_number = 1;
        p_data = media_buffer_get(buffer_ent->pool_handle,
                                  p_stream_head->payload_length + sizeof(T_AUDIO_PATH_DATA_HDR));

        if (p_data != NULL)
        {
            p_stream_head_fab = (T_AUDIO_PATH_DATA_HDR *)p_data;

            p_stream_head_fab->timestamp = p_media_packet->timestamp;
            p_stream_head_fab->status = AUDIO_STREAM_STATUS_LOST;
            p_stream_head_fab->seq_num = download_seq;
            p_stream_head_fab->frame_number = 1;
            p_stream_head_fab->frame_count = download_frames_counter;
            p_stream_head_fab->payload_length = media_data_len;

            memcpy(p_stream_head_fab->payload, media_data_ptr, media_data_len);

            if (audio_path_data_send(buffer_ent->path_handle,
                                     (T_AUDIO_PATH_DATA_HDR *)p_data,
                                     p_media_packet->payload_length))
            {
                AUDIO_PRINT_WARN3("media_buffer_plc: AAC, h2d_seq %u, frames_counter %d | %d",
                                  p_stream_head_fab->seq_num, p_media_packet->frame_counter, download_frames_counter);
                buffer_ent->ds_local_seq_last++;
                buffer_ent->ds_frame_counter_last = download_frames_counter;
                audio_remote_record_push(buffer_ent->audio_remote_handle, &packet_head);
                remote_instance->frame_diff -= download_frames;
                remote_instance->seq_diff -= 1;
            }
            else
            {
                ret = false;
            }
            media_buffer_put(buffer_ent->pool_handle, p_data);
        }
    }
    else
    {
        audio_mgr_exception(buffer_ent, MEDIA_BUFFER_PACKET_LOSS, 0, remote_instance->frame_diff);
        return false;
    }

    AUDIO_PRINT_INFO2("media_buffer_plc: remain seq %d frames %d",
                      remote_instance->seq_diff,
                      remote_instance->frame_diff);
    return ret;
}

static bool audio_remote_buffer_PLC_policy_2(T_AUDIO_REMOTE_INSTANCE *remote_instance)
{
    uint16_t size;
    uint16_t download_frames_length;
    uint16_t frame_len;
    T_MEDIA_BUFFER_ENTITY *buffer_ent = remote_instance->buffer_ent;
    T_MEDIA_DATA_HDR *p_media_packet;
    T_MEDIA_DATA_HDR *p_make_up ;
    T_AUDIO_PATH_DATA_HDR *p_head;
    bool ret = true;

    AUDIO_PRINT_INFO1("audio_remote_buffer_PLC_policy_2: seq diff 0 frame diff %u",
                      remote_instance->frame_diff);

    p_media_packet = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);
    if (p_media_packet == NULL)
    {
        AUDIO_PRINT_ERROR0("audio_remote_buffer_PLC_policy_2: no more packets");
        return false;
    }

    frame_len = audio_remote_cacu_frame_len(p_media_packet->payload_length,
                                            p_media_packet->frame_number);
    download_frames_length = frame_len * (p_media_packet->frame_number + 1);
    size = sizeof(T_MEDIA_DATA_HDR) + sizeof(T_AUDIO_PATH_DATA_HDR) + download_frames_length;
    p_make_up = media_buffer_get(buffer_ent->pool_handle, size);
    if (p_make_up == NULL)
    {
        return false;
    }

    memcpy(p_make_up,
           p_media_packet,
           sizeof(T_MEDIA_DATA_HDR) + p_media_packet->payload_length); //remain 1 frame

    memcpy(p_make_up->p_data + p_media_packet->payload_length,
           p_media_packet->p_data + p_media_packet->payload_length - frame_len,
           frame_len);//copy 1 frame

    p_head = (T_AUDIO_PATH_DATA_HDR *)p_make_up->p_data;

    p_make_up->payload_length = sizeof(T_AUDIO_PATH_DATA_HDR) + download_frames_length;
    p_make_up->frame_number = p_make_up->frame_number + 1;
    p_make_up->frame_counter = p_make_up->frame_number + buffer_ent->ds_frame_counter_last;
    p_make_up->local_seq_number = buffer_ent->ds_local_seq_last + 1;

    p_head->timestamp = p_media_packet->timestamp;
    p_head->status = AUDIO_STREAM_STATUS_CORRECT;
    p_head->seq_num = buffer_ent->ds_local_seq_last + 1;
    p_head->frame_number = p_make_up->frame_number;
    p_head->frame_count = p_make_up->frame_counter;
    p_head->payload_length = download_frames_length;

    if (audio_path_data_send(buffer_ent->path_handle,
                             (T_AUDIO_PATH_DATA_HDR *)p_make_up->p_data,
                             p_make_up->payload_length))
    {
        AUDIO_PRINT_WARN5("audio_remote_buffer_PLC_policy_2: download_frames %d, h2d_seq %u, packet_len %d, frames_counter %d | %d",
                          p_make_up->frame_number, p_make_up->local_seq_number, p_make_up->payload_length,
                          buffer_ent->ds_frame_counter_last, p_make_up->frame_counter);
        buffer_ent->ds_local_seq_last++;
        buffer_ent->ds_frame_counter_last = p_make_up->frame_counter;
        remote_instance->frame_diff -= 1;
        audio_remote_record_push(buffer_ent->audio_remote_handle, p_make_up);
        media_buffer_flush(buffer_ent, 1, BUFFER_DIR_DOWNSTREAM);
    }
    else
    {
        ret = false;
    }
    media_buffer_put(buffer_ent->pool_handle, p_make_up);

    return ret;
}

static bool audio_remote_buffer_PLC_policy_3(T_AUDIO_REMOTE_INSTANCE *remote_instance)
{
    uint16_t download_frames;
    uint16_t download_counter;
    uint16_t download_length;
    uint16_t frame_len;
    T_MEDIA_BUFFER_ENTITY *buffer_ent = remote_instance->buffer_ent;
    T_MEDIA_DATA_HDR *p_media_packet;
    T_AUDIO_PATH_DATA_HDR *p_head;
    bool ret = true;

    AUDIO_PRINT_INFO1("audio_remote_buffer_PLC_policy_3: seq diff 0 frame diff %u",
                      remote_instance->frame_diff);
    p_media_packet = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);
    if (p_media_packet == NULL)
    {
        AUDIO_PRINT_ERROR0("audio_remote_buffer_PLC_policy_3: no more packets");
        return false;
    }

    while (p_media_packet->frame_number + remote_instance->frame_diff <= 0)
    {
        remote_instance->frame_diff += p_media_packet->frame_number;
        media_buffer_flush(buffer_ent, 1, BUFFER_DIR_DOWNSTREAM);
        p_media_packet = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);
        if (p_media_packet == NULL)
        {
            AUDIO_PRINT_INFO1("audio_remote_buffer_PLC_policy_3: no more data remain diff frame %d",
                              remote_instance->frame_diff);
            return false;
        }
    }

    frame_len = audio_remote_cacu_frame_len(p_media_packet->payload_length,
                                            p_media_packet->frame_number);

    if (remote_instance->frame_diff == 0)
    {
        AUDIO_PRINT_INFO2("audio_remote_buffer_PLC_policy_3: remain seq %d frames %d",
                          remote_instance->seq_diff,
                          remote_instance->frame_diff);
        return media_buffer_downstream_dsp(buffer_ent);
    }
    else
    {
        download_frames = p_media_packet->frame_number + remote_instance->frame_diff;
        download_counter = download_frames + buffer_ent->ds_frame_counter_last;
        download_length = sizeof(T_AUDIO_PATH_DATA_HDR) + download_frames * frame_len;

        p_head = (T_AUDIO_PATH_DATA_HDR *)p_media_packet->p_data;
        p_media_packet->local_seq_number = buffer_ent->ds_local_seq_last + 1;

        p_head->timestamp = p_media_packet->timestamp;
        p_head->status = AUDIO_STREAM_STATUS_CORRECT;
        p_head->seq_num = buffer_ent->ds_local_seq_last + 1;
        p_head->frame_number = download_frames;
        p_head->frame_count = download_counter;
        p_head->payload_length = download_frames * frame_len;

        if (audio_path_data_send(buffer_ent->path_handle,
                                 p_head,
                                 download_length))
        {
            p_media_packet->frame_number = download_frames;
            p_media_packet->frame_counter = download_counter;
            AUDIO_PRINT_WARN5("audio_remote_buffer_PLC_policy_3: download_frames %d, h2d_seq %u, packet_len %d, frames_counter %d | %d",
                              p_media_packet->frame_number, p_media_packet->local_seq_number, download_length,
                              buffer_ent->ds_frame_counter_last, p_media_packet->frame_counter);
            buffer_ent->ds_local_seq_last++;
            buffer_ent->ds_frame_counter_last = download_counter;
            remote_instance->frame_diff = 0;
            audio_remote_record_push(buffer_ent->audio_remote_handle, p_media_packet);
            media_buffer_flush(buffer_ent, 1, BUFFER_DIR_DOWNSTREAM);
        }
        else
        {
            ret = false;
        }
        return ret;
    }
}

bool audio_remote_buffer_PLC(T_AUDIO_REMOTE_HANDLE handle)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;
    T_MEDIA_BUFFER_ENTITY *buffer_ent;
    T_MEDIA_DATA_HDR *p_media_packet;
    bool ret = false;

    if (audio_remote_handle_check(remote_instance) == false)
    {
        return false;
    }

    buffer_ent = remote_instance->buffer_ent;

    if (buffer_ent == NULL)
    {
        return false;
    }

    if (remote_instance->seq_diff > 0 && remote_instance->frame_diff > 0)
    {
        ret = audio_remote_buffer_PLC_policy_1(handle);
    }
    else if (remote_instance->seq_diff > 0 && remote_instance->frame_diff <= 0)
    {

        while (remote_instance->frame_diff <= 0)
        {
            p_media_packet = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);
            if (p_media_packet == NULL)
            {
                goto fail_no_more_packet;
            }
            remote_instance->frame_diff += p_media_packet->frame_number;
            media_buffer_flush(buffer_ent, 1, BUFFER_DIR_DOWNSTREAM);
            remote_instance->seq_diff = remote_instance->seq_diff + 1;
        }

        ret = audio_remote_buffer_PLC_policy_1(remote_instance);
    }
    else
    {
        while (remote_instance->seq_diff < 0)
        {
            p_media_packet = media_buffer_peek(buffer_ent, 0, BUFFER_DIR_DOWNSTREAM);
            if (p_media_packet == NULL)
            {
                goto fail_no_more_packet;
            }
            remote_instance->frame_diff += p_media_packet->frame_number;
            media_buffer_flush(buffer_ent, 1, BUFFER_DIR_DOWNSTREAM);
            remote_instance->seq_diff = remote_instance->seq_diff + 1;
        }

        if (remote_instance->frame_diff == 0)
        {
            AUDIO_PRINT_INFO2("media_buffer_plc: remain seq %d frames %d",
                              remote_instance->seq_diff,
                              remote_instance->frame_diff);
            ret = media_buffer_downstream_dsp(buffer_ent);
        }
        else
        {
            if (buffer_ent->format_info.type == AUDIO_FORMAT_TYPE_SBC)
            {
                if (remote_instance->frame_diff > 0)
                {
                    ret = audio_remote_buffer_PLC_policy_2(remote_instance);
                }
                else
                {
                    ret = audio_remote_buffer_PLC_policy_3(remote_instance);
                }
            }
            else if (buffer_ent->format_info.type == AUDIO_FORMAT_TYPE_AAC)
            {
                if (remote_instance->frame_diff < 0)
                {
                    if (media_buffer_ds_pkt_cnt(buffer_ent) != 0)
                    {
                        remote_instance->frame_diff += 1;
                        media_buffer_flush(buffer_ent, 1, BUFFER_DIR_DOWNSTREAM);
                        ret = true;
                    }
                    else
                    {
                        ret = false;
                    }
                }
                else
                {
                    audio_mgr_exception(buffer_ent, MEDIA_BUFFER_PACKET_LOSS, 0, remote_instance->frame_diff);
                    ret = false;
                }
            }
            else
            {
                audio_mgr_exception(buffer_ent, MEDIA_BUFFER_PACKET_LOSS, 0, remote_instance->frame_diff);
                ret = false;
            }
        }
    }

    return ret;

fail_no_more_packet:
    AUDIO_PRINT_ERROR0("media_buffer_plc: no more packets");
    return false;
}

int32_t audio_remote_get_frame_diff(T_AUDIO_REMOTE_HANDLE handle)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;
    if (audio_remote_handle_check(remote_instance))
    {
        return remote_instance->frame_diff;
    }
    else
    {
        return false;
    }
}

void audio_remote_set_frame_diff(T_AUDIO_REMOTE_HANDLE handle,
                                 int32_t               frame_diff)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;
    if (audio_remote_handle_check(remote_instance))
    {
        remote_instance->frame_diff = frame_diff;
    }
}


bool audio_remote_seamless_join_seq_contiuity_check(T_AUDIO_REMOTE_HANDLE handle)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;

    if (audio_remote_handle_check(remote_instance) == false)
    {
        return false;
    }

    if (remote_instance->stream_type != AUDIO_STREAM_TYPE_PLAYBACK)
    {
        return false;
    }

    if (audio_remote_db->force_join)
    {
        if (remote_instance->seq_check_timer == NULL)
        {
            remote_instance->seq_check_timer = sys_timer_create("seq_check_timer",
                                                                SYS_TIMER_TYPE_LOW_PRECISION,
                                                                remote_instance->endpoint,
                                                                SEQUENCE_CHECK_TIME,
                                                                false,
                                                                audio_remote_seq_check_timeout_cback);
            if (remote_instance->seq_check_timer != NULL)
            {
                sys_timer_start(remote_instance->seq_check_timer);
            }
            remote_instance->seamless_join_lost_packet_cnt = 0;
            remote_instance->seamless_join_lost_packet_cnt_last = MAX_SEAMLESS_JOIN_CHECK_LOST_COUNT;
        }

        if (remote_instance->seamless_join_lost_packet_cnt_last < MAX_SEAMLESS_JOIN_CHECK_LOST_COUNT)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (remote_instance->seq_check_timer)
        {
            sys_timer_delete(remote_instance->seq_check_timer);
            remote_instance->seq_check_timer = NULL;
            remote_instance->seamless_join_lost_packet_cnt = 0;
            remote_instance->seamless_join_lost_packet_cnt_last = 0;
        }
        return true;
    }
}

void audio_remote_seamless_join_set(bool enable,
                                    bool force_join)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance;

    AUDIO_PRINT_INFO2("audio_remote_seamless_join_set: enable %d force %d", enable, force_join);
    audio_remote_db->force_join = force_join;
    audio_remote_db->seamless_join = enable;
    if (force_join)
    {
        remote_instance = os_queue_peek(&audio_remote_db->remote_db_list, 0);
        while (remote_instance != NULL)
        {
            if (remote_instance->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                if (remote_instance->seq_check_timer == NULL)
                {
                    remote_instance->seq_check_timer = sys_timer_create("seq_check_timer",
                                                                        SYS_TIMER_TYPE_LOW_PRECISION,
                                                                        remote_instance->endpoint,
                                                                        SEQUENCE_CHECK_TIME,
                                                                        false,
                                                                        audio_remote_seq_check_timeout_cback);
                    if (remote_instance->seq_check_timer != NULL)
                    {
                        sys_timer_start(remote_instance->seq_check_timer);
                    }
                    remote_instance->seamless_join_lost_packet_cnt = 0;
                    remote_instance->seamless_join_lost_packet_cnt_last = 0;
                }
            }

            remote_instance = remote_instance->p_next;
        }
    }
    else
    {
        remote_instance = os_queue_peek(&audio_remote_db->remote_db_list, 0);
        while (remote_instance != NULL)
        {
            if (remote_instance->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                sys_timer_delete(remote_instance->seq_check_timer);
                remote_instance->seq_check_timer = NULL;
                remote_instance->seamless_join_lost_packet_cnt = 0;
                remote_instance->seamless_join_lost_packet_cnt_last = 0;
            }
            remote_instance = remote_instance->p_next;
        }
    }
}

void audio_remote_handle_lost_packet(T_AUDIO_REMOTE_HANDLE handle,
                                     uint8_t               count)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;

    if (audio_remote_handle_check(remote_instance) == false)
    {
        return;
    }

    if (remote_instance->seq_check_timer)
    {
        remote_instance->seamless_join_lost_packet_cnt += count;
    }
}

bool audio_remote_async_msg_relay(T_AUDIO_REMOTE_HANDLE  handle,
                                  uint16_t               msg_id,
                                  void                  *msg_buf,
                                  uint16_t               msg_len,
                                  bool                   loopback)
{
    bool ret;
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;
    T_MEDIA_BUFFER_ENTITY *buffer_ent;
    T_AUDIO_REMOTE_INTERNAL_MSG *async_msg;
    T_AUDIO_REMOTE_BUFFER_MSG_RELAY *relay_msg;
    uint16_t malloc_len = msg_len + sizeof(T_AUDIO_REMOTE_BUFFER_MSG_RELAY) + sizeof(
                              T_AUDIO_REMOTE_INTERNAL_MSG);

    if (audio_remote_handle_check(remote_instance) == false)
    {
        return false;
    }
    async_msg = (T_AUDIO_REMOTE_INTERNAL_MSG *)calloc(1, malloc_len);
    if (async_msg == NULL)
    {
        return false;
    }
    relay_msg = (T_AUDIO_REMOTE_BUFFER_MSG_RELAY *)async_msg->msg;

    async_msg->remote_endpoint = remote_instance->endpoint;
    buffer_ent = remote_instance->buffer_ent;
    if (buffer_ent != NULL)
    {
        memcpy(async_msg->uuid, buffer_ent->uuid, 8);
        async_msg->stream_type = buffer_ent->stream_type;
    }
    relay_msg->msg_id = msg_id;
    memcpy(relay_msg->msg, msg_buf, msg_len);

    ret = remote_async_msg_relay(audio_remote_db->relay_handle,
                                 AUDIO_REMOTE_BUFFER_MSG_RELAY,
                                 async_msg,
                                 malloc_len,
                                 loopback);

    free(async_msg);
    return ret;
}

bool audio_remote_sync_msg_relay(T_AUDIO_REMOTE_HANDLE  handle,
                                 uint16_t               msg_id,
                                 void                  *msg_buf,
                                 uint16_t               msg_len,
                                 T_REMOTE_TIMER_TYPE    timer_type,
                                 uint32_t               timer_period,
                                 bool                   loopback)
{
    bool ret;
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;
    T_MEDIA_BUFFER_ENTITY *buffer_ent;
    T_AUDIO_REMOTE_INTERNAL_MSG *sync_msg;
    T_AUDIO_REMOTE_BUFFER_MSG_RELAY *relay_msg;
    uint16_t malloc_len = msg_len + sizeof(T_AUDIO_REMOTE_BUFFER_MSG_RELAY) + sizeof(
                              T_AUDIO_REMOTE_INTERNAL_MSG);

    if (audio_remote_handle_check(remote_instance) == false)
    {
        return false;
    }

    sync_msg = (T_AUDIO_REMOTE_INTERNAL_MSG *)calloc(1, malloc_len);
    if (sync_msg == NULL)
    {
        return false;
    }

    relay_msg = (T_AUDIO_REMOTE_BUFFER_MSG_RELAY *)sync_msg->msg;
    sync_msg->remote_endpoint = remote_instance->endpoint;
    buffer_ent = remote_instance->buffer_ent;
    if (buffer_ent != NULL)
    {
        memcpy(sync_msg->uuid, buffer_ent->uuid, 8);
        sync_msg->stream_type = buffer_ent->stream_type;
    }
    relay_msg->msg_id = msg_id;
    memcpy(relay_msg->msg, msg_buf, msg_len);

    ret = remote_sync_msg_relay(audio_remote_db->relay_handle,
                                AUDIO_REMOTE_BUFFER_MSG_RELAY,
                                sync_msg,
                                malloc_len,
                                timer_type,
                                timer_period,
                                loopback);
    free(sync_msg);
    return ret;
}

void audio_remote_sync_lock(T_AUDIO_REMOTE_HANDLE handle)
{
    T_AUDIO_REMOTE_INSTANCE *remote_instance = (T_AUDIO_REMOTE_INSTANCE *)handle;

    if (audio_remote_handle_check(handle))
    {
        remote_instance->slave_audio_sync = true;
    }
}

bool audio_remote_get_seamless_join(void)
{
    return audio_remote_db->seamless_join;
}

bool audio_remote_get_force_join(void)
{
    return audio_remote_db->force_join;
}

uint16_t audio_remote_play_margin_get(T_AUDIO_FORMAT_TYPE type)
{
    return sync_play_margin_table[type] + AUDIO_REMOTE_SYNC_RELAY_DURATION;
}
