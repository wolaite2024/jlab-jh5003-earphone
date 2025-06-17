/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "trace.h"
#include "sys_mgr.h"
#include "sys_ipc.h"
#include "audio.h"
#include "audio_mgr.h"
#include "audio_remote.h"
#include "notification.h"
#include "audio_path.h"
#include "media_buffer.h"
#include "jitter_buffer.h"
#include "audio_latency.h"

/* TODO Remove Start */
#include "bt_mgr.h"
#include "audio_interval_eval.h"
#include "media_pool.h"
/* TODO Remove End */

typedef struct t_audio_mgr_cback_item
{
    struct t_audio_mgr_cback_item *p_next;
    P_AUDIO_MGR_CBACK              cback;
} T_AUDIO_MGR_CBACK_ITEM;

typedef struct t_audio_db
{
    T_REMOTE_RELAY_HANDLE   relay_handle;
    T_OS_QUEUE              cback_list;
    T_SYS_IPC_HANDLE        sys_ipc_handle;
    T_SYS_IPC_HANDLE        bt_ipc_handle;
} T_AUDIO_DB;

static T_AUDIO_DB *audio_db = NULL;

T_AUDIO_TRACK_CAUSE convert_exception_to_restart_casuse(uint32_t exception)
{
    T_AUDIO_TRACK_CAUSE cause;
    T_AUDIO_MGR_EXCEPTION exc = (T_AUDIO_MGR_EXCEPTION)exception;
    switch (exc)
    {
    case MEDIA_BUFFER_PACKET_LOST:
        cause = AUDIO_TRACK_CAUSE_JOIN_PACKET_LOST;
        break;
    case DSP_DECODE_EMPTY:
        cause = AUDIO_TRACK_CAUSE_BUFFER_EMPTY;
        break;
    case MEDIA_BUFFER_SEAMLESS_MISS_PKT:
    case MEDIA_BUFFER_SEAMLESS_TIMEOUT:
        cause = AUDIO_TRACK_CAUSE_REMOTE_JOIN_FAILED;
        break;
    default:
        cause = AUDIO_TRACK_CAUSE_UNKNOWN_ERROR;
        break;
    }
    return cause;
}

void media_buffer_check_miss_ack(T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    if (buffer_ent->miss_ack_cnt)
    {
        if (audio_remote_is_buffer_unsync(buffer_ent->audio_remote_handle))
        {
            if (audio_remote_buffer_PLC(buffer_ent->audio_remote_handle))
            {
                buffer_ent->miss_ack_cnt--;
            }
        }
        else
        {
            if (media_buffer_downstream_dsp(buffer_ent))
            {
                buffer_ent->miss_ack_cnt--;
            }
        }
    }
}

bool bt_clk_compare(uint32_t clk_a,
                    uint32_t clk_b)
{
    clk_a = clk_a & 0X0FFFFFFF;
    clk_b = clk_b & 0X0FFFFFFF;

    if ((clk_a > clk_b  && clk_a - clk_b < 0X07FFFFFF) ||
        (clk_a < clk_b && clk_b - clk_a > 0X07FFFFFF))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool audio_mgr_cback_register(P_AUDIO_MGR_CBACK cback)
{
    T_AUDIO_MGR_CBACK_ITEM *p_item;

    p_item = (T_AUDIO_MGR_CBACK_ITEM *)audio_db->cback_list.p_first;
    while (p_item != NULL)
    {
        /* Sanity check if callback already registered */
        if (p_item->cback == cback)
        {
            return true;
        }

        p_item = p_item->p_next;
    }

    p_item = malloc(sizeof(T_AUDIO_MGR_CBACK_ITEM));
    if (p_item != NULL)
    {
        p_item->cback = cback;
        os_queue_in(&audio_db->cback_list, p_item);
        return true;
    }

    return false;
}

bool audio_mgr_cback_unregister(P_AUDIO_MGR_CBACK cback)
{
    T_AUDIO_MGR_CBACK_ITEM *p_item;
    bool ret = false;

    p_item = (T_AUDIO_MGR_CBACK_ITEM *)audio_db->cback_list.p_first;
    while (p_item != NULL)
    {
        if (p_item->cback == cback)
        {
            os_queue_delete(&audio_db->cback_list, p_item);
            free(p_item);
            ret = true;
            break;
        }

        p_item = p_item->p_next;
    }

    return ret;
}

bool audio_mgr_event_post(T_AUDIO_EVENT  event_type,
                          void          *event_buf,
                          uint16_t       buf_len)
{
    T_AUDIO_MGR_CBACK_ITEM *p_item;

    AUDIO_PRINT_INFO1("audio_mgr_event_post: event_type 0x%04x", event_type);

    p_item = (T_AUDIO_MGR_CBACK_ITEM *)audio_db->cback_list.p_first;
    while (p_item != NULL)
    {
        p_item->cback(event_type, event_buf, buf_len);
        p_item = p_item->p_next;
    }

    return true;
}

void audio_buffer_uninit_state(T_AUDIO_MSG            msg,
                               T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    if (buffer_ent == NULL)
    {
        return;
    }
    switch (msg)
    {
    case AUDIO_MSG_BUFFER_RESET:
        {
            audio_remote_reset(buffer_ent->audio_remote_handle);
        }
        break;

    case AUDIO_MSG_BUFFER_WRITE:
        {
            if (buffer_ent->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                if (buffer_ent->mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
                {
                    buffer_ent->max_latency_plc_count = media_buffer_cacu_frame_num(ULTRA_LOW_LATENCY_DYNAMIC_LATENCY_MS
                                                                                    -
                                                                                    buffer_ent->latency_back_up,
                                                                                    &(buffer_ent->format_info));
                }
                else if (buffer_ent->mode == AUDIO_STREAM_MODE_LOW_LATENCY)
                {
                    buffer_ent->max_latency_plc_count = media_buffer_cacu_frame_num(LOW_LATENCY_DYNAMIC_LATENCY_MS -
                                                                                    buffer_ent->latency_back_up,
                                                                                    &(buffer_ent->format_info));
                }

                if (buffer_ent->mode == AUDIO_STREAM_MODE_LOW_LATENCY ||
                    buffer_ent->mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
                {
                    AUDIO_PRINT_INFO4("media_buffer_init: gaming_mode start frame num %u latency %u fixed %u max_latency_plc_count %u",
                                      buffer_ent->start_decode_frame_cnt, buffer_ent->latency, buffer_ent->latency_fixed,
                                      buffer_ent->max_latency_plc_count);
                    media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_PREQUEUE);
                    audio_remote_reset(buffer_ent->audio_remote_handle);
                    if (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED &&
                        buffer_ent->usage != AUDIO_STREAM_USAGE_LOCAL)
                    {
                        if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
                        {
                            media_buffer_flush(buffer_ent, 1, BUFFER_DIR_DOWNSTREAM);
                            buffer_ent->sec_path_ready = 0;
                            media_buffer_query_sec_path_state(buffer_ent);
                        }
                    }
                    else
                    {
                        audio_mgr_dispatch(AUDIO_MSG_BUFFER_WRITE, buffer_ent);
                    }
                }
                else if (buffer_ent->mode == AUDIO_STREAM_MODE_DIRECT)
                {
                    media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_SYNC);
                    audio_remote_reset(buffer_ent->audio_remote_handle);
                    media_buffer_flush(buffer_ent, 1, BUFFER_DIR_DOWNSTREAM);

                }
                else
                {
                    AUDIO_PRINT_INFO3("media_buffer_init: start frame num %u latency %u max_plc %u",
                                      buffer_ent->start_decode_frame_cnt, buffer_ent->latency,
                                      media_buffer_get_max_plc_count());
                    media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_PREQUEUE);
                    audio_remote_reset(buffer_ent->audio_remote_handle);
                    if (buffer_ent->usage == AUDIO_STREAM_USAGE_SNOOP)
                    {
                        if (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED &&
                            remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
                        {
                            media_buffer_solo_play_timer_set(buffer_ent);
                        }
                    }
                }
            }
            else if (buffer_ent->stream_type == AUDIO_STREAM_TYPE_VOICE)
            {
                if (buffer_ent->mode == AUDIO_STREAM_MODE_DIRECT)
                {
                    media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_PLAY);
                    audio_remote_reset(buffer_ent->audio_remote_handle);
                    audio_path_timestamp_set(buffer_ent->path_handle, BT_CLK_NONE, 0xffffffff, false);
                    media_buffer_downstream_dsp(buffer_ent);
                }
                else
                {
                    media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_PREQUEUE);
                    audio_remote_reset(buffer_ent->audio_remote_handle);
                }
            }
        }
        break;

    default:
        break;
    }
}

void audio_buffer_prequeue_state(T_AUDIO_MSG            msg,
                                 T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    if (buffer_ent == NULL)
    {
        return;
    }

    switch (msg)
    {
    case AUDIO_MSG_BUFFER_RESET:
        {
            audio_remote_reset(buffer_ent->audio_remote_handle);
        }
        break;

    case AUDIO_MSG_BUFFER_WRITE:
        {
            if (buffer_ent->usage == AUDIO_STREAM_USAGE_LOCAL ||
                (buffer_ent->usage == AUDIO_STREAM_USAGE_SNOOP &&
                 remote_session_state_get() == REMOTE_SESSION_STATE_DISCONNECTED))
            {
                {
                    T_BT_CLK_REF clk_ref;
                    uint32_t bb_clock_slot;
                    uint32_t bb_clock_slot_sync;
                    uint16_t bb_clock_us;
                    uint8_t  clk_idx;
                    uint16_t latency = jitter_buffer_target_latency_get(buffer_ent->jitter_buffer_handle);

                    if (buffer_ent->mode == AUDIO_STREAM_MODE_LOW_LATENCY ||
                        buffer_ent->mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
                    {
                        clk_ref = bt_piconet_clk_get(BT_CLK_SNIFFING, &clk_idx, &bb_clock_slot, &bb_clock_us);
                        if (clk_ref != BT_CLK_NONE)
                        {
                            bb_clock_slot_sync = (bb_clock_slot + latency * 1000 * 2 / 625) & 0x0fffffff;
                            AUDIO_PRINT_INFO1("media_buffer_set_playing: gaming_mode latency %d", latency);
                            media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_PLAY);
                            audio_path_timestamp_set(buffer_ent->path_handle, clk_idx, bb_clock_slot_sync, false);
                            media_buffer_downstream_dsp(buffer_ent);
                        }
                        else
                        {
                            if (media_buffer_ds_frame_cnt(buffer_ent) >= buffer_ent->start_decode_frame_cnt)
                            {
                                AUDIO_PRINT_INFO1("media_buffer_set_playing: gaming_mode total_frames %d",
                                                  media_buffer_ds_frame_cnt(buffer_ent));
                                media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_PLAY);
                                audio_path_timestamp_set(buffer_ent->path_handle, clk_idx, 0xffffffff, false);
                                media_buffer_downstream_dsp(buffer_ent);
                                media_buffer_downstream_dsp(buffer_ent);
                            }
                        }
                    }
                    else
                    {
                        if (media_buffer_ds_frame_cnt(buffer_ent) >= buffer_ent->start_decode_frame_cnt / 2)
                        {
                            AUDIO_PRINT_INFO2("media_buffer_state: single play total_frames %d, goal %u",
                                              media_buffer_ds_frame_cnt(buffer_ent), buffer_ent->start_decode_frame_cnt);
                            media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_PLAY);
                            audio_remote_set_state(buffer_ent->audio_remote_handle, AUDIO_REMOTE_STATE_UNSYNC);
                            audio_path_synchronization_join_set(NULL, 0);

                            clk_ref = bt_piconet_clk_get(BT_CLK_SNIFFING, &clk_idx, &bb_clock_slot, &bb_clock_us);
                            if (clk_ref != BT_CLK_NONE)
                            {
                                uint16_t play_time = media_buffer_playtime_calculate(buffer_ent);

                                bb_clock_slot_sync = (bb_clock_slot +  play_time * 1000 * 2 / 625) & 0x0fffffff;
                            }
                            else
                            {
                                bb_clock_slot_sync = 0xffffffff;
                            }

                            audio_path_timestamp_set(buffer_ent->path_handle, clk_idx, bb_clock_slot_sync, false);
                            media_buffer_leading_downstream(buffer_ent);
                        }
                    }
                }
            }
            else
            {
                if (buffer_ent->mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY ||
                    buffer_ent->mode == AUDIO_STREAM_MODE_LOW_LATENCY)
                {
                    if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        if (buffer_ent->sec_path_ready)
                        {
                            bool dynamic_latency_on = false;
                            audio_latency_get_dynamic_latency_on(buffer_ent->audio_latency_handle,
                                                                 &dynamic_latency_on);
                            media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_SYNC);
                            if (buffer_ent->mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
                            {
                                audio_remote_ultra_low_latency_sync_play(buffer_ent->audio_remote_handle, dynamic_latency_on);
                            }
                            else
                            {
                                audio_remote_low_latency_sync_play(buffer_ent->audio_remote_handle, dynamic_latency_on);
                            }
                        }
                        else
                        {
                            if (media_buffer_ds_pkt_cnt(buffer_ent) > 5)
                            {
                                media_buffer_flush(buffer_ent, media_buffer_ds_pkt_cnt(buffer_ent),
                                                   BUFFER_DIR_DOWNSTREAM);
                                media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_IDLE);
                            }
                        }
                    }
                    else
                    {
                        if (audio_remote_get_state(buffer_ent->audio_remote_handle) == AUDIO_REMOTE_STATE_UNSYNC)
                        {
                            audio_remote_align_buffer(buffer_ent->audio_remote_handle);
                        }
                    }
                }
                else
                {
                    if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        if (media_buffer_ds_frame_cnt(buffer_ent) >= buffer_ent->start_decode_frame_cnt)
                        {
                            AUDIO_PRINT_WARN0("frames > start_decode_frame_cnt but fst sync rsp hasn't been received");
                        }
                        else
                        {
                            if (media_buffer_ds_pkt_cnt(buffer_ent) == MEDIA_SYNC_TRIGGER_CNT
                                && audio_remote_get_state(buffer_ent->audio_remote_handle) != AUDIO_REMOTE_STATE_SYNCED)
                            {
                                audio_remote_align_buffer(buffer_ent->audio_remote_handle);
                            }
                        }
                    }
                    else
                    {
                        if (media_buffer_ds_frame_cnt(buffer_ent) >= buffer_ent->start_decode_frame_cnt)
                        {
                            media_buffer_flush(buffer_ent, 1, BUFFER_DIR_DOWNSTREAM);
                        }

                        if (audio_remote_get_state(buffer_ent->audio_remote_handle) == AUDIO_REMOTE_STATE_UNSYNC)
                        {
                            if (buffer_ent->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
                            {
                                if (audio_remote_seamless_join_seq_contiuity_check(buffer_ent->audio_remote_handle))
                                {
                                    audio_remote_align_buffer(buffer_ent->audio_remote_handle);
                                }
                            }
                            else
                            {
                                audio_remote_align_buffer(buffer_ent->audio_remote_handle);
                            }
                        }
                        else if (audio_remote_get_state(buffer_ent->audio_remote_handle) ==
                                 AUDIO_REMOTE_STATE_SEAMLESS_JOIN)
                        {
                            //only sec runs here
                            audio_remote_find_join_pkt(buffer_ent->audio_remote_handle);
                        }
                    }
                }
            }

        }
        break;

    default:
        break;
    }
}

void audio_buffer_wait_play_state(T_AUDIO_MSG            msg,
                                  T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    if (buffer_ent == NULL)
    {
        return;
    }
    switch (msg)
    {
    case AUDIO_MSG_BUFFER_RESET:
        {
            media_buffer_reset(buffer_ent);
        }
        break;

    case AUDIO_MSG_BUFFER_WRITE:
        {
            if (buffer_ent->mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
            {
                media_buffer_flush(buffer_ent, media_buffer_ds_pkt_cnt(buffer_ent) - 1,
                                   BUFFER_DIR_DOWNSTREAM);
            }
            else if (buffer_ent->mode == AUDIO_STREAM_MODE_DIRECT)
            {
                media_buffer_state_set(buffer_ent, MEDIA_BUFFER_STATE_PLAY);
                audio_path_timestamp_set(buffer_ent->path_handle, BT_CLK_NONE, 0xffffffff, false);
                media_buffer_downstream_dsp(buffer_ent);
            }
        }
        break;

    default:
        break;
    }

}

void audio_buffer_playing_state(T_AUDIO_MSG            msg,
                                T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    if (buffer_ent == NULL)
    {
        return;
    }
    switch (msg)
    {
    case AUDIO_MSG_BUFFER_RESET:
        {
            T_AUDIO_TRACK_STATE  state;

            media_buffer_reset(buffer_ent);
            if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY &&
                remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
            {
                media_buffer_reset_relay(buffer_ent);
            }

            buffer_ent->restart_cause = convert_exception_to_restart_casuse(buffer_ent->exception);
            audio_track_state_get(buffer_ent->track_handle, &state);
            if (state == AUDIO_TRACK_STATE_STARTED)
            {
                audio_track_restart(buffer_ent->track_handle);
            }
        }
        break;

    case AUDIO_MSG_BUFFER_WRITE:
        {
            if (audio_remote_get_state(buffer_ent->audio_remote_handle) == AUDIO_REMOTE_STATE_SEAMLESS_JOIN)
            {
                //only pri runs here
                audio_remote_find_join_pkt(buffer_ent->audio_remote_handle);
            }

            if (buffer_ent->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
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
                else if (buffer_ent->mode == AUDIO_STREAM_MODE_DIRECT)
                {
                    while (media_buffer_downstream_dsp(buffer_ent));
                }
            }

            if (buffer_ent->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    buffer_ent->buffer_sync_trigger_cnt++;
                    if (audio_remote_period_sync_check(buffer_ent->audio_remote_handle,
                                                       buffer_ent->buffer_sync_trigger_cnt))
                    {
                        buffer_ent->buffer_sync_trigger_cnt = 0;
                        audio_remote_buffer_sync_req(buffer_ent->audio_remote_handle);
                    }
                }
                media_buffer_check_miss_ack(buffer_ent);

            }
            else if (buffer_ent->stream_type == AUDIO_STREAM_TYPE_VOICE)
            {
                media_buffer_downstream_dsp(buffer_ent);
            }
        }
        break;

    default:
        break;
    }
}

bool audio_mgr_path_cback(uint32_t  id,
                          void     *msg)
{
    bool ret = false;

    switch (id)
    {
    case AUDIO_PATH_EVT_DSP_INTER_MSG:
        {
            ret = audio_mgr_dispatch(AUDIO_MSG_A2DP_DSP_INTER_MSG, msg);
            if (ret == false)
            {
                audio_path_b2bmsg_interaction_timeout();
            }
        }
        break;
    }

    return ret;
}

void audio_mgr_buffer_event_handle(T_AUDIO_MSG            msg,
                                   T_MEDIA_BUFFER_ENTITY *buffer_ent)
{
    switch (media_buffer_state_get(buffer_ent))
    {
    case MEDIA_BUFFER_STATE_IDLE:
        {
            audio_buffer_uninit_state(msg, buffer_ent);
        }
        break;

    case MEDIA_BUFFER_STATE_PREQUEUE:
        {
            audio_buffer_prequeue_state(msg, buffer_ent);
        }
        break;

    case MEDIA_BUFFER_STATE_SYNC:
        {
            audio_buffer_wait_play_state(msg, buffer_ent);
        }
        break;

    case MEDIA_BUFFER_STATE_PLAY:
        {
            audio_buffer_playing_state(msg, buffer_ent);
        }
        break;
    }
}

bool audio_mgr_dispatch(T_AUDIO_MSG  msg,
                        void        *buf)
{
    bool ret = false;

    switch (msg)
    {
    case AUDIO_MSG_BUFFER_RESET:
    case AUDIO_MSG_BUFFER_WRITE:
        {
            T_MEDIA_BUFFER_ENTITY *buffer_ent = (T_MEDIA_BUFFER_ENTITY *)buf;
            audio_mgr_buffer_event_handle(msg, buffer_ent);
        }
        break;

    case AUDIO_MSG_BUFFER_LEVEL_LOW:
        {
            T_MEDIA_BUFFER_ENTITY *buffer_ent = (T_MEDIA_BUFFER_ENTITY *)buf;
            uint16_t buffer_level = 0;
            T_AUDIO_EVENT_PARAM_TRACK_BUFFER_LOW buffer_low_param;

            media_buffer_proxy_level_get((T_MEDIA_BUFFER_PROXY)buffer_ent, &buffer_level);
            buffer_low_param.buffer_level_ms = buffer_level;
            buffer_low_param.handle = buffer_ent->track_handle;
            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_BUFFER_LOW, &buffer_low_param,
                                       sizeof(T_AUDIO_EVENT_PARAM_TRACK_BUFFER_LOW));
        }
        break;

    case AUDIO_MSG_BUFFER_LEVEL_HIGH:
        {
            T_MEDIA_BUFFER_ENTITY *buffer_ent = (T_MEDIA_BUFFER_ENTITY *)buf;
            uint16_t buffer_level = 0;
            T_AUDIO_EVENT_PARAM_TRACK_BUFFER_HIGH buffer_high_param;

            media_buffer_proxy_level_get((T_MEDIA_BUFFER_PROXY)buffer_ent, &buffer_level);
            buffer_high_param.buffer_level_ms = buffer_level;
            buffer_high_param.handle = buffer_ent->track_handle;
            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_BUFFER_HIGH, &buffer_high_param,
                                       sizeof(T_AUDIO_EVENT_PARAM_TRACK_BUFFER_HIGH));
        }
        break;

    case AUDIO_MSG_BUFFER_STATE_PLAYING:
        {
            ret = audio_mgr_event_post(AUDIO_EVENT_BUFFER_STATE_PLAYING, NULL, 0);
        }
        break;

    case AUDIO_MSG_DSP_DECODE_EMPTY:
        {
            T_MEDIA_BUFFER_ENTITY *buffer_ent = (T_MEDIA_BUFFER_ENTITY *)buf;
            if (buffer_ent->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                audio_mgr_exception(buffer_ent, DSP_DECODE_EMPTY, 0, 0);
            }
        }
        break;

    case AUDIO_MSG_DSP_SYNC_UNLOCK:
        {
            T_MEDIA_BUFFER_ENTITY *buffer_ent = (T_MEDIA_BUFFER_ENTITY *)buf;
            if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
            {
                audio_mgr_exception(buffer_ent, DSP_UNSYNC, 0, 0);
            }
        }
        break;

    case AUDIO_MSG_A2DP_DSP_SYNC_LOCK:
        {
            T_MEDIA_BUFFER_ENTITY *buffer_ent = (T_MEDIA_BUFFER_ENTITY *)buf;
            media_buffer_handle_sync_lock(buffer_ent);
        }
        break;

    case AUDIO_MSG_DSP_UNSYNC:
        {
            T_MEDIA_BUFFER_ENTITY *buffer_ent = (T_MEDIA_BUFFER_ENTITY *)buf;
            if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
            {
                audio_mgr_exception(buffer_ent, DSP_UNSYNC, 0, 0);
            }
        }
        break;

    case AUDIO_MSG_DSP_SYNC_EMPTY:
        {
            T_MEDIA_BUFFER_ENTITY *buffer_ent = (T_MEDIA_BUFFER_ENTITY *)buf;
            if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
            {
                audio_mgr_exception(buffer_ent, DSP_SYNC_EMPTY, 0, 0);
            }
        }
        break;

    case AUDIO_MSG_DSP_LOST_TIMESTAMP:
        {
            T_MEDIA_BUFFER_ENTITY *buffer_ent = (T_MEDIA_BUFFER_ENTITY *)buf;
            if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
            {
                audio_mgr_exception(buffer_ent, DSP_LOST_TIMESTAMP, 0, 0);
            }
        }
        break;

    case AUDIO_MSG_A2DP_DSP_INTER_MSG:
        {
            uint8_t *p_buf = (uint8_t *)buf;
            ret = remote_async_msg_relay(audio_db->relay_handle, AUDIO_REMOTE_DSP_INTER_MSG, buf,
                                         p_buf[0], false);

        }
        break;

    case AUDIO_MSG_TRACK_DATA_IND:
        {
            T_AUDIO_MSG_PAYLOAD_TRACK_DATA_IND *msg = buf;
            T_AUDIO_EVENT_PARAM param;

            param.track_data_ind.handle = msg->handle;
            param.track_data_ind.len    = msg->len;

            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_DATA_IND, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_TRACK_DATA_XMIT:
        {
            T_MEDIA_BUFFER_ENTITY *buffer_ent;

            buffer_ent = (T_MEDIA_BUFFER_ENTITY *)buf;
            if (media_buffer_state_get(buffer_ent) != MEDIA_BUFFER_STATE_PLAY)
            {
                break;
            }

            if (audio_path_is_running(buffer_ent->path_handle) == false)
            {
                break;
            }

            if (buffer_ent->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
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
                    if (audio_remote_is_buffer_unsync(buffer_ent->audio_remote_handle))
                    {
                        if (audio_remote_buffer_PLC(buffer_ent->audio_remote_handle) == false)
                        {
                            buffer_ent->miss_ack_cnt++;
                        }
                    }
                    else
                    {
                        if (media_buffer_downstream_dsp(buffer_ent) == false)
                        {
                            buffer_ent->miss_ack_cnt++;
                        }
                    }
                }
            }
            else if (buffer_ent->stream_type == AUDIO_STREAM_TYPE_VOICE)
            {
                if (media_buffer_downstream_dsp(buffer_ent) == false)
                {
                    buffer_ent->miss_ack_cnt++;
                }
            }
        }
        break;

    case AUDIO_MSG_TRACK_CREATE:
        {
            T_AUDIO_EVENT_PARAM param;
            T_AUDIO_MSG_PAYLOAD_TRACK_CREATE *payload;

            payload = buf;

            /* TODO bind to media buffer according to stream type. */

            param.track_state_changed.handle = payload->handle;
            param.track_state_changed.state  = AUDIO_TRACK_STATE_CREATED;
            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_STATE_CHANGED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_TRACK_START:
        {
            T_AUDIO_EVENT_PARAM param;
            T_AUDIO_MSG_PAYLOAD_TRACK_START *payload;
            T_MEDIA_BUFFER_ENTITY *buffer_ent;

            payload = buf;
            buffer_ent = (T_MEDIA_BUFFER_ENTITY *)payload->buffer_proxy;
            if (buffer_ent == NULL)
            {
                break;
            }

            param.track_state_changed.handle = payload->handle;
            param.track_state_changed.state  = AUDIO_TRACK_STATE_STARTED;
            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_STATE_CHANGED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_TRACK_RESTART:
        {
            T_AUDIO_MSG_PAYLOAD_TRACK_RESTART *payload;
            T_AUDIO_EVENT_PARAM param;
            T_MEDIA_BUFFER_ENTITY *buffer_ent;

            payload = buf;
            buffer_ent = (T_MEDIA_BUFFER_ENTITY *)payload->buffer_proxy;
            if (buffer_ent == NULL)
            {
                break;
            }

            param.track_state_changed.handle = payload->handle;
            param.track_state_changed.state  = AUDIO_TRACK_STATE_RESTARTED;
            param.track_state_changed.cause = buffer_ent->restart_cause;
            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_STATE_CHANGED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_TRACK_SUSPEND_RESUME:
        {

        }
        break;

    case AUDIO_MSG_TRACK_STOP:
        {
            T_AUDIO_EVENT_PARAM param;
            T_AUDIO_MSG_PAYLOAD_TRACK_STOP *payload;

            payload = buf;

            /* TODO deactive the media buffer according to stream type. */

            param.track_state_changed.handle = payload->handle;
            param.track_state_changed.state  = AUDIO_TRACK_STATE_STOPPED;
            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_STATE_CHANGED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_TRACK_SUSPEND:
        {
            T_AUDIO_MSG_PAYLOAD_TRACK_PAUSE *payload = buf;
            T_MEDIA_BUFFER_ENTITY *buffer_ent;

            buffer_ent = (T_MEDIA_BUFFER_ENTITY *)payload->buffer_proxy;
            if (buffer_ent == NULL)
            {
                break;
            }

            if (AUDIO_STREAM_TYPE_PLAYBACK == payload->stream_type)
            {
                if (media_buffer_state_get(buffer_ent) == MEDIA_BUFFER_STATE_SYNC)
                {
                    if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        media_buffer_reset_relay(buffer_ent);
                    }
                }
                media_buffer_reset(buffer_ent);
            }
            else if (AUDIO_STREAM_TYPE_VOICE == payload->stream_type)
            {
                media_buffer_reset(buffer_ent);
            }
        }
        break;

    case AUDIO_MSG_TRACK_PAUSE:
        {
            T_AUDIO_EVENT_PARAM param;
            T_AUDIO_MSG_PAYLOAD_TRACK_PAUSE *payload = buf;
            T_MEDIA_BUFFER_ENTITY *buffer_ent;

            buffer_ent = (T_MEDIA_BUFFER_ENTITY *)payload->buffer_proxy;
            if (buffer_ent == NULL)
            {
                break;
            }

            /* TODO deactive the media buffer according to stream type. */

            param.track_state_changed.handle = payload->handle;
            param.track_state_changed.state  = AUDIO_TRACK_STATE_PAUSED;
            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_STATE_CHANGED, &param, sizeof(param));

            if (AUDIO_STREAM_TYPE_PLAYBACK == payload->stream_type ||
                AUDIO_STREAM_TYPE_VOICE == payload->stream_type)
            {
                media_buffer_reset(buffer_ent);
            }
        }
        break;

    case AUDIO_MSG_TRACK_RELEASE:
        {
            T_AUDIO_EVENT_PARAM param;
            T_AUDIO_MSG_PAYLOAD_TRACK_RELEASE *payload;

            payload = buf;

            /* TODO detach the media buffer according to stream type. */

            param.track_state_changed.handle = payload->handle;
            param.track_state_changed.state  = AUDIO_TRACK_STATE_RELEASED;
            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_STATE_CHANGED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_TRACK_VOLUME_OUT_CHANGE:
        {
            T_AUDIO_EVENT_PARAM param;
            T_AUDIO_MSG_PAYLOAD_TRACK_VOLUME_OUT_CHANGE *payload;

            payload = buf;

            param.track_volume_out_changed.handle = payload->handle;
            param.track_volume_out_changed.prev_volume = payload->prev_volume;
            param.track_volume_out_changed.curr_volume = payload->curr_volume;
            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_VOLUME_OUT_CHANGED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_TRACK_VOLUME_OUT_MUTE:
        {
            T_AUDIO_EVENT_PARAM param;
            T_AUDIO_MSG_PAYLOAD_TRACK_VOLUME_OUT_MUTE *payload;

            payload = buf;

            param.track_volume_out_muted.handle = payload->handle;
            param.track_volume_out_muted.volume = payload->volume;
            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_VOLUME_OUT_MUTED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_TRACK_VOLUME_OUT_UNMUTE:
        {
            T_AUDIO_EVENT_PARAM param;
            T_AUDIO_MSG_PAYLOAD_TRACK_VOLUME_OUT_UNMUTE *payload;

            payload = buf;

            param.track_volume_out_unmuted.handle = payload->handle;
            param.track_volume_out_unmuted.volume = payload->volume;
            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_VOLUME_OUT_UNMUTED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_TRACK_VOLUME_IN_CHANGE:
        {
            T_AUDIO_EVENT_PARAM param;
            T_AUDIO_MSG_PAYLOAD_TRACK_VOLUME_IN_CHANGE *payload;

            payload = buf;

            param.track_volume_in_changed.handle = payload->handle;
            param.track_volume_in_changed.prev_volume = payload->prev_volume;
            param.track_volume_in_changed.curr_volume = payload->curr_volume;
            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_VOLUME_IN_CHANGED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_TRACK_VOLUME_IN_MUTE:
        {
            T_AUDIO_EVENT_PARAM param;
            T_AUDIO_MSG_PAYLOAD_TRACK_VOLUME_IN_MUTE *payload;

            payload = buf;

            param.track_volume_in_muted.handle = payload->handle;
            param.track_volume_in_muted.volume = payload->volume;
            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_VOLUME_IN_MUTED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_TRACK_VOLUME_IN_UNMUTE:
        {
            T_AUDIO_EVENT_PARAM param;
            T_AUDIO_MSG_PAYLOAD_TRACK_VOLUME_IN_UNMUTE *payload;

            payload = buf;

            param.track_volume_in_unmuted.handle = payload->handle;
            param.track_volume_in_unmuted.volume = payload->volume;
            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_VOLUME_IN_UNMUTED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_VOLUME_OUT_MAX_SET:
        {
            T_AUDIO_MSG_PAYLOAD_VOLUME_OUT_MAX_SET *payload;

            payload = buf;
            ret = audio_track_volume_out_max_set(payload->type, payload->volume);
        }
        break;

    case AUDIO_MSG_VOLUME_OUT_MIN_SET:
        {
            T_AUDIO_MSG_PAYLOAD_VOLUME_OUT_MIN_SET *payload;

            payload = buf;
            ret = audio_track_volume_out_min_set(payload->type, payload->volume);
        }
        break;

    case AUDIO_MSG_VOLUME_OUT_CHANGE:
        {
            T_AUDIO_EVENT_PARAM param;
            T_AUDIO_MSG_PAYLOAD_VOLUME_OUT_CHANGE *payload;

            payload = buf;
            param.volume_out_changed.type        = payload->type;
            param.volume_out_changed.prev_volume = payload->prev_volume;
            param.volume_out_changed.curr_volume = payload->curr_volume;

            audio_track_volume_out_set_all(payload->type, payload->curr_volume);
            ret = audio_mgr_event_post(AUDIO_EVENT_VOLUME_OUT_CHANGED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_VOLUME_OUT_MUTE:
        {
            T_AUDIO_EVENT_PARAM param;
            T_AUDIO_MSG_PAYLOAD_VOLUME_OUT_MUTE *payload;

            payload = buf;
            param.volume_out_muted.type         = payload->type;
            param.volume_out_muted.volume       = payload->volume;

            audio_track_volume_out_mute_all(payload->type);
            ret = audio_mgr_event_post(AUDIO_EVENT_VOLUME_OUT_MUTED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_VOLUME_OUT_UNMUTE:
        {
            T_AUDIO_EVENT_PARAM param;
            T_AUDIO_MSG_PAYLOAD_VOLUME_OUT_UNMUTE *payload;

            payload = buf;
            param.volume_out_unmuted.type       = payload->type;
            param.volume_out_unmuted.volume     = payload->volume;

            audio_track_volume_out_unmute_all(payload->type);
            ret = audio_mgr_event_post(AUDIO_EVENT_VOLUME_OUT_UNMUTED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_VOLUME_IN_MAX_SET:
        {
            T_AUDIO_MSG_PAYLOAD_VOLUME_IN_MAX_SET *payload;

            payload = buf;
            ret = audio_track_volume_in_max_set(payload->type, payload->volume);
        }
        break;

    case AUDIO_MSG_VOLUME_IN_MIN_SET:
        {
            T_AUDIO_MSG_PAYLOAD_VOLUME_IN_MIN_SET *payload;

            payload = buf;
            ret = audio_track_volume_in_min_set(payload->type, payload->volume);
        }
        break;

    case AUDIO_MSG_VOLUME_IN_CHANGE:
        {
            T_AUDIO_MSG_PAYLOAD_VOLUME_IN_CHANGE *payload;
            T_AUDIO_EVENT_PARAM param;

            payload = buf;
            param.volume_in_changed.type        = payload->type;
            param.volume_in_changed.prev_volume = payload->prev_volume;
            param.volume_in_changed.curr_volume = payload->curr_volume;

            audio_track_volume_in_set_all(payload->type, payload->curr_volume);
            ret = audio_mgr_event_post(AUDIO_EVENT_VOLUME_IN_CHANGED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_VOLUME_IN_MUTED:
        {
            T_AUDIO_MSG_PAYLOAD_VOLUME_IN_MUTE *payload;
            T_AUDIO_EVENT_PARAM param;

            payload = buf;
            param.volume_in_muted.type      = payload->type;
            param.volume_in_muted.volume    = payload->volume;

            audio_track_volume_in_mute_all(payload->type);
            ret = audio_mgr_event_post(AUDIO_EVENT_VOLUME_IN_MUTED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_VOLUME_IN_UNMUTE:
        {
            T_AUDIO_MSG_PAYLOAD_VOLUME_IN_UNMUTE *payload;
            T_AUDIO_EVENT_PARAM param;

            payload = buf;
            param.volume_in_unmuted.type    = payload->type;
            param.volume_in_unmuted.volume  = payload->volume;

            audio_track_volume_in_unmute_all(payload->type);
            ret = audio_mgr_event_post(AUDIO_EVENT_VOLUME_IN_UNMUTED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_VOLUME_BALANCE_CHANGE:
        {
            T_AUDIO_MSG_PAYLOAD_VOLUME_BALANCE_CHANGE *payload;
            T_AUDIO_EVENT_PARAM param;

            payload = buf;
            param.volume_balance_changed.type       = payload->type;
            param.volume_balance_changed.prev_scale = payload->prev_scale;
            param.volume_balance_changed.curr_scale = payload->curr_scale;

            audio_track_volume_balance_set(payload->type, payload->curr_scale);
            ret = audio_mgr_event_post(AUDIO_EVENT_VOLUME_BALANCE_CHANGED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_KWS_HIT:
        {
            T_AUDIO_MSG_PAYLOAD_KWS_HIT *payload;
            T_AUDIO_EVENT_PARAM param;

            payload = (T_AUDIO_MSG_PAYLOAD_KWS_HIT *)buf;
            param.kws_hit.keyword_index = payload->keyword_index;

            ret = audio_mgr_event_post(AUDIO_EVENT_KWS_HIT, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_RETURN_JOIN_INFO:
        {
            T_MEDIA_BUFFER_ENTITY *buffer_ent;
            T_AUDIO_MSG_PAYLOAD_TRACK_RETURN_JOIN_INFO *param = (T_AUDIO_MSG_PAYLOAD_TRACK_RETURN_JOIN_INFO *)
                                                                buf;

            buffer_ent = (T_MEDIA_BUFFER_ENTITY *)(param->buffer_proxy);
            if (buffer_ent)
            {
                audio_remote_dsp_seamless_join_handle(buffer_ent,
                                                      param->join_clk,
                                                      param->join_frame,
                                                      param->delta_frame);
            }
        }
        break;

    case AUDIO_MSG_TRACK_SIGNAL_OUT_REFRESH:
        {
            T_AUDIO_MSG_PAYLOAD_TRACK_SIGNAL_OUT_REFRESH *param;
            T_AUDIO_EVENT_PARAM payload;

            param = buf;
            payload.track_signal_out_refreshed.handle = param->handle;
            payload.track_signal_out_refreshed.freq_num = param->freq_num;
            payload.track_signal_out_refreshed.left_gain_table = param->left_gain_table;
            payload.track_signal_out_refreshed.right_gain_table = param->right_gain_table;

            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_SIGNAL_OUT_REFRESHED, &payload, sizeof(payload));
        }
        break;

    case AUDIO_MSG_TRACK_SIGNAL_IN_REFRESH:
        {
            T_AUDIO_MSG_PAYLOAD_TRACK_SIGNAL_IN_REFRESH *param;
            T_AUDIO_EVENT_PARAM payload;

            param = buf;
            payload.track_signal_in_refreshed.handle = param->handle;
            payload.track_signal_in_refreshed.freq_num = param->freq_num;
            payload.track_signal_in_refreshed.left_gain_table = param->left_gain_table;
            payload.track_signal_in_refreshed.right_gain_table = param->right_gain_table;

            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_SIGNAL_IN_REFRESHED, &payload, sizeof(payload));
        }
        break;

    case AUDIO_MSG_TRACK_ERROR_REPORT:
        {
            T_AUDIO_EVENT_PARAM payload;
            T_AUDIO_MSG_PAYLOAD_TRACK_ERROR_REPORT *param;

            param = buf;
            payload.track_error_report.handle = param->handle;

            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_ERROR_REPORT, &payload, sizeof(payload));
        }
        break;

    case AUDIO_MSG_STREAM_LATENCY_REPORT:
        {
            T_MEDIA_BUFFER_ENTITY *buffer_ent;
            T_AUDIO_MSG_PAYLOAD_STREAM_LATENCY_REPORT *param = (T_AUDIO_MSG_PAYLOAD_STREAM_LATENCY_REPORT *)buf;
            T_AUDIO_REMOTE_BUFFER_SYNC_UNIT sync_unit[2] = {0};
            uint16_t average_clk_diff = param->average_packet_latency;
            uint16_t average_fifo_queuing = param->average_fifo_queuing;
            uint16_t average_latency_ms = average_clk_diff + average_fifo_queuing;
            T_AUDIO_EVENT_PARAM payload;

            buffer_ent = (T_MEDIA_BUFFER_ENTITY *)(param->buffer_proxy);
            if (buffer_ent == NULL)
            {
                break;
            }

            payload.track_latency_reported.handle = buffer_ent->track_handle;
            payload.track_latency_reported.preset_latency = buffer_ent->latency;
            payload.track_latency_reported.instant_latency = average_latency_ms;
            payload.track_latency_reported.dejitter_compensation = jitter_buffer_compensation_get(
                                                                       buffer_ent->jitter_buffer_handle);

            ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_LATENCY_REPORTED, &payload, sizeof(payload));

            AUDIO_PRINT_TRACE4("latency_report: count %u avg %ums %u %u", param->normal_packet_count,
                               average_clk_diff, average_fifo_queuing, average_latency_ms);

            audio_remote_record_peek(buffer_ent->audio_remote_handle, sync_unit, 2);

            if (buffer_ent->usage != AUDIO_STREAM_USAGE_LOCAL)
            {
                if (average_fifo_queuing == 0 ||
                    (sync_unit[0].timestamp == 0 && sync_unit[1].timestamp == 0) ||
                    (sync_unit[0].timestamp == 0xffffffff && sync_unit[1].timestamp == 0xffffffff) ||
                    average_clk_diff > 750 || average_clk_diff == 0)
                {
                    break;
                }
            }

            audio_latency_record_update(buffer_ent->audio_latency_handle, average_latency_ms);

            if (buffer_ent->ignore_latency_report)
            {
                buffer_ent->ignore_latency_report--;
                break;
            }

            if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
            {
                break;
            }

            if (buffer_ent->mode == AUDIO_STREAM_MODE_LOW_LATENCY)
            {
                if (remote_session_state_get() == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    jitter_buffer_asrc_pid_adjust(buffer_ent->jitter_buffer_handle, average_latency_ms);
                }
                else
                {
                    jitter_buffer_low_latency_keeper(buffer_ent->jitter_buffer_handle, average_latency_ms);
                }
            }
            else
            {
                if (jitter_buffer_asrc_pid_is_block(buffer_ent->jitter_buffer_handle))
                {
                    break;
                }

                jitter_buffer_asrc_pid_adjust(buffer_ent->jitter_buffer_handle, average_latency_ms);
            }
        }
        break;

    case AUDIO_MSG_STREAM_PLC_REPORT:
        {
            T_AUDIO_MSG_PAYLOAD_TRACK_PLC_REPORT *plc = (T_AUDIO_MSG_PAYLOAD_TRACK_PLC_REPORT *)buf;
            T_MEDIA_BUFFER_ENTITY *buffer_ent;

            buffer_ent = (T_MEDIA_BUFFER_ENTITY *)(plc->buffer_proxy);
            if (buffer_ent != NULL)
            {
                T_AUDIO_EVENT_PARAM payload;

                payload.track_plc_report.handle = buffer_ent->track_handle;
                payload.track_plc_report.plc_sample_num = plc->plc_sample_num;
                payload.track_plc_report.total_sample_num = plc->total_sample_num;

                ret = audio_mgr_event_post(AUDIO_EVENT_TRACK_PLC_REPORT, &payload, sizeof(payload));

                AUDIO_PRINT_INFO3("dsp_plc_notify: handle 0x%x plc sample num %u total sample num %u",
                                  buffer_ent->track_handle, plc->plc_sample_num, plc->total_sample_num);
            }
        }
        break;

    case AUDIO_MSG_PASSTHROUGH_ENABLE:
        {
            T_AUDIO_EVENT_PARAM param;
            T_AUDIO_MSG_PAYLOAD_PASSTHROUGH_ENABLE *payload;

            payload = buf;
            param.passthrough_started.mode = payload->mode;

            ret = audio_mgr_event_post(AUDIO_EVENT_PASSTHROUGH_ENABLED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_PASSTHROUGH_DISABLE:
        {
            T_AUDIO_EVENT_PARAM param;
            T_AUDIO_MSG_PAYLOAD_PASSTHROUGH_DISABLE *payload;

            payload = buf;
            param.passthrough_stopped.mode = payload->mode;

            ret = audio_mgr_event_post(AUDIO_EVENT_PASSTHROUGH_DISABLED, &param, sizeof(param));
        }
        break;

    case AUDIO_MSG_KWS_ENABLE:
        {
            ret = audio_mgr_event_post(AUDIO_EVENT_KWS_ENABLED, NULL, 0);
        }
        break;

    case AUDIO_MSG_KWS_DISABLE:
        {
            ret = audio_mgr_event_post(AUDIO_EVENT_KWS_DISABLED, NULL, 0);
        }
        break;

    default:
        break;
    }

    return ret;
}

void audio_mgr_relay_cback(uint16_t               event,
                           T_REMOTE_RELAY_STATUS  status,
                           void                  *buf,
                           uint16_t               len)
{
    AUDIO_PRINT_TRACE2("audio_mgr_relay_cback: event 0x%04x, status %u", event, status);

    switch (event)
    {
    case AUDIO_REMOTE_SYNC_PARAM:
        {
            T_AUDIO_SYNC_PARAMS *p_msg = (T_AUDIO_SYNC_PARAMS *)buf;
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                audio_remote_seamless_join_set(p_msg->seamless_join, p_msg->force_join);
            }
        }
        break;

    case AUDIO_REMOTE_SEAMLESS_JOIN_SET:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                T_AUDIO_REMOTE_SEAMLESS_JOIN_SET *p_msg = (T_AUDIO_REMOTE_SEAMLESS_JOIN_SET *)buf;
                audio_remote_seamless_join_set(p_msg->enable, p_msg->force_join);
            }
        }
        break;

    case AUDIO_REMOTE_DSP_INTER_MSG:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                audio_path_synchronization_data_send(NULL, (uint8_t *)buf, ((uint8_t *)buf)[0]);
            }
        }
        break;

    default:
        break;
    }
}

bool audio_bt_ipc_cback(uint32_t  id,
                        void     *msg)
{
    switch (id)
    {
    case BT_IPC_REMOTE_CONNECTED:
        {
            T_AUDIO_SYNC_PARAMS msg;
            if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
            {
                msg.seamless_join = audio_remote_get_seamless_join();
                msg.force_join = audio_remote_get_force_join();
                remote_async_msg_relay(audio_db->relay_handle,
                                       AUDIO_REMOTE_SYNC_PARAM,
                                       &msg,
                                       sizeof(T_AUDIO_SYNC_PARAMS),
                                       false);
            }
        }
        break;

    case BT_IPC_REMOTE_SWAP_START:
        {
            uint16_t role = remote_session_role_get();
            audio_path_synchronization_role_swap(NULL, role, true);
        }
        break;

    case BT_IPC_REMOTE_SWAP_STOP:
        {
            uint16_t role = remote_session_role_get();
            audio_path_synchronization_role_swap(NULL, role, false);
        }
        break;

    case BT_IPC_REMOTE_DISCONNECTED:
        {
            notification_sync_lost();
            media_buffer_sync_lost();
        }

        break;

    default:
        break;
    }

    return true;
}

bool audio_sys_ipc_cback(uint32_t  id,
                         void     *msg)
{
    switch (id)
    {
    case SYS_IPC_POWER_ON:
        {

        }
        break;

    case SYS_IPC_POWER_OFF:
        {
            audio_path_power_off();
        }
        break;

    default:
        break;
    }

    return true;
}

bool audio_mgr_init(uint16_t playback_pool_size,
                    uint16_t voice_pool_size,
                    uint16_t record_pool_size,
                    uint16_t notification_pool_size)
{
    int32_t ret = 0;

    audio_db = calloc(1, sizeof(T_AUDIO_DB));
    if (audio_db == NULL)
    {
        ret = 1;
        goto fail_alloc_db;
    }

    if (audio_path_init(audio_mgr_path_cback) == false)
    {
        ret = 2;
        goto fail_init_audio_path;
    }

    if (audio_remote_init() == false)
    {
        ret = 3;
        goto fail_init_audio_remote;
    }

    if (audio_volume_init() == false)
    {
        ret = 4;
        goto fail_init_audio_volume;
    }

    if (audio_track_init(playback_pool_size, voice_pool_size, record_pool_size) == false)
    {
        ret = 5;
        goto fail_init_audio_track;
    }

    if (audio_line_init() == false)
    {
        ret = 6;
        goto fail_init_audio_line;
    }

    if (audio_effect_init() == false)
    {
        ret = 7;
        goto fail_init_audio_effect;
    }

    if (notification_init(notification_pool_size) == false)
    {
        ret = 8;
        goto fail_init_notification;
    }
#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
    if (anc_init() == false)
    {
        ret = 9;
        goto fail_init_anc;
    }
#endif
    if (audio_passthrough_init() == false)
    {
        ret = 10;
        goto fail_init_audio_passthrough;
    }

    if (vad_init() == false)
    {
        ret = 11;
        goto fail_init_vad;
    }

    if (kws_init() == false)
    {
        ret = 12;
        goto fail_init_kws;
    }

    audio_db->relay_handle = remote_relay_register(audio_mgr_relay_cback);
    if (audio_db->relay_handle == NULL)
    {
        ret = 13;
        goto fail_register_remote_relay;
    }

    audio_db->sys_ipc_handle = sys_ipc_subscribe(SYS_IPC_TOPIC, audio_sys_ipc_cback);
    if (audio_db->sys_ipc_handle == NULL)
    {
        ret = 14;
        goto fail_subscribe_sys_ipc_cback;
    }

    audio_db->bt_ipc_handle = sys_ipc_subscribe(BT_IPC_TOPIC, audio_bt_ipc_cback);
    if (audio_db->bt_ipc_handle == NULL)
    {
        ret = 15;
        goto fail_subscribe_bt_ipc_cback;
    }

    if (audio_latency_mgr_init() == false)
    {
        ret = 16;
        goto fail_init_audio_latency;
    }

    if (audio_interval_eval_init() == false)
    {
        ret = 17;
        goto fail_init_audio_interval_eval;
    }

    if (audio_pipe_init() == false)
    {
        ret = 18;
        goto fail_init_audio_pipe;
    }

    return true;

fail_init_audio_pipe:
    audio_interval_eval_deinit();
fail_init_audio_interval_eval:
    audio_latency_mgr_deinit();
fail_init_audio_latency:
    sys_ipc_unsubscribe(audio_db->bt_ipc_handle);
fail_subscribe_bt_ipc_cback:
    sys_ipc_unsubscribe(audio_db->sys_ipc_handle);
fail_subscribe_sys_ipc_cback:
    remote_relay_unregister(audio_db->relay_handle);
fail_register_remote_relay:
    kws_deinit();
fail_init_kws:
    vad_deinit();
fail_init_vad:
    audio_passthrough_deinit();
fail_init_audio_passthrough:
#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
    anc_deinit();
fail_init_anc:
#endif
    notification_deinit();
fail_init_notification:
    audio_effect_deinit();
fail_init_audio_effect:
    audio_line_deinit();
fail_init_audio_line:
    audio_track_deinit();
fail_init_audio_track:
    audio_volume_deinit();
fail_init_audio_volume:
    audio_remote_deinit();
fail_init_audio_remote:
    audio_path_deinit();
fail_init_audio_path:
    free(audio_db);
    audio_db = NULL;
fail_alloc_db:
    AUDIO_PRINT_ERROR1("audio_mgr_init: failed %d", -ret);
    return false;
}

void audio_mgr_set_max_plc_count(uint8_t plc_count)
{
    media_buffer_set_max_plc_count(plc_count);
}


void audio_mgr_exception(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                         T_AUDIO_MGR_EXCEPTION  exc,
                         uint8_t                from_remote,
                         uint32_t               param)
{
    T_MEDIA_BUFFER_STATE state = media_buffer_state_get(buffer_ent);

    AUDIO_PRINT_INFO4("audio_mgr_exception: exception 0x%04x, from_remote %d, param 0x%08x, state %u",
                      exc, from_remote, param, state);

    if (buffer_ent == NULL)
    {
        return;
    }

    buffer_ent->exception = exc;
    if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
    {
        T_AUDIO_EXC_CMD cmd;
        cmd.exc = exc;
        cmd.stream_type = buffer_ent->stream_type;
        cmd.param = param;

        if (exc != DSP_SYNC_EMPTY &&
            exc != DSP_DECODE_EMPTY &&
            exc != MEDIA_BUFFER_PRI_RESET_BUFFER &&
            exc != MEDIA_BUFFER_PACKET_LOST &&
            exc != MEDIA_BUFFER_JBC_CLK_ERR &&
            exc != MEDIA_BUFFER_REMOTE_STATE_ERR)
        {
            if (state != MEDIA_BUFFER_STATE_IDLE)
            {
                media_buffer_relay_exception(buffer_ent, (uint8_t *)&cmd);
            }
        }
    }
    else
    {
        if (state == MEDIA_BUFFER_STATE_PLAY)
        {
            if (audio_remote_get_state(buffer_ent->audio_remote_handle) == AUDIO_REMOTE_STATE_SEAMLESS_JOIN)
            {
                audio_remote_cancel_seamless_join(buffer_ent->audio_remote_handle);
            }

            if (audio_remote_get_force_join() && from_remote)
            {
                if (exc == MEDIA_BUFFER_FULL || exc == DSP_UNSYNC)
                {
                    AUDIO_PRINT_INFO1("audio_mgr_exc: pri ignores sec exceptions 0x%x", exc);
                    return;
                }
            }
        }
    }

    if (state != MEDIA_BUFFER_STATE_IDLE)
    {
        switch (exc)
        {
        case MEDIA_BUFFER_FULL:
            {
                if (state == MEDIA_BUFFER_STATE_PLAY && from_remote == 0)
                {
                    if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        if (buffer_ent->mode != AUDIO_STREAM_MODE_LOW_LATENCY &&
                            buffer_ent->mode != AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
                        {
                            bool dynamic_latency_on = false;
                            audio_latency_get_dynamic_latency_on(buffer_ent->audio_latency_handle,
                                                                 &dynamic_latency_on);
                            if (dynamic_latency_on)
                            {
                                media_buffer_dynamic_latency_resume(buffer_ent);
                            }
                        }
                    }
                }
                media_buffer_flush(buffer_ent, media_buffer_ds_pkt_cnt(buffer_ent),
                                   BUFFER_DIR_DOWNSTREAM);
                audio_mgr_dispatch(AUDIO_MSG_BUFFER_RESET, buffer_ent);
            }
            break;

        case MEDIA_BUFFER_FST_SYNC_ERR:
            {
                if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    if (from_remote == 1)
                    {
                        if (buffer_ent->media_buffer_fst_sync_tid == (uint8_t)(param & 0x00ff) &&
                            (param >> 16) == FST_SYNC_STATE_ERROR)
                        {
                            media_buffer_flush(buffer_ent,
                                               media_buffer_ds_pkt_cnt(buffer_ent) - MEDIA_SYNC_TRIGGER_CNT, BUFFER_DIR_DOWNSTREAM);
                            if (media_buffer_ds_pkt_cnt(buffer_ent) >= MEDIA_SYNC_TRIGGER_CNT)
                            {
                                audio_remote_align_buffer(buffer_ent->audio_remote_handle);
                            }
                        }
                    }
                    else
                    {
                        if ((param >> 16) == FST_SYNC_DATA_EXPIRED)
                        {
                            if (media_buffer_ds_pkt_cnt(buffer_ent) > MEDIA_SYNC_TRIGGER_CNT)
                            {
                                media_buffer_flush(buffer_ent,
                                                   media_buffer_ds_pkt_cnt(buffer_ent) - MEDIA_SYNC_TRIGGER_CNT, BUFFER_DIR_DOWNSTREAM);
                            }
                            audio_remote_align_buffer(buffer_ent->audio_remote_handle);
                        }
                        else
                        {
                            if (state == MEDIA_BUFFER_STATE_PREQUEUE)
                            {
                                if (media_buffer_ds_pkt_cnt(buffer_ent) - MEDIA_SYNC_TRIGGER_CNT > 0)
                                {
                                    media_buffer_flush(buffer_ent,
                                                       media_buffer_ds_pkt_cnt(buffer_ent) - MEDIA_SYNC_TRIGGER_CNT, BUFFER_DIR_DOWNSTREAM);
                                }
                                audio_remote_align_buffer(buffer_ent->audio_remote_handle);

                            }
                            else
                            {
                                audio_mgr_dispatch(AUDIO_MSG_BUFFER_RESET, buffer_ent);
                            }
                        }
                    }
                }
                else
                {
                    audio_mgr_dispatch(AUDIO_MSG_BUFFER_RESET, buffer_ent);
                }
            }
            break;

        case DSP_DECODE_EMPTY:
            {
                T_AUDIO_EVENT_PARAM param;

                if (state == MEDIA_BUFFER_STATE_PLAY && from_remote == 0)
                {
                    if (buffer_ent->mode != AUDIO_STREAM_MODE_LOW_LATENCY &&
                        buffer_ent->mode != AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
                    {
                        bool dynamic_latency_on = false;
                        audio_latency_get_dynamic_latency_on(buffer_ent->audio_latency_handle,
                                                             &dynamic_latency_on);
                        if (buffer_ent->latency_fixed == false && dynamic_latency_on == false)
                        {
                            media_buffer_dynamic_latency_set(buffer_ent,
                                                             buffer_ent->latency_back_up * (1 + NORMAL_MODE_DYNAMIC_LATENCY_RATIO), true);
                        }
                    }
                }

                param.decode_empty.handle = buffer_ent->track_handle;
                audio_mgr_event_post(AUDIO_EVENT_TRACK_DECODE_EMPTY, &param, sizeof(param));
            }
            break;

        case MEDIA_BUFFER_PACKET_LOST:
            {
                if (state == MEDIA_BUFFER_STATE_PREQUEUE)
                {
                    media_buffer_flush(buffer_ent, media_buffer_ds_pkt_cnt(buffer_ent),
                                       BUFFER_DIR_DOWNSTREAM);
                    buffer_ent->last_seq_updated = false;
                }

                audio_mgr_dispatch(AUDIO_MSG_BUFFER_RESET, buffer_ent);
            }
            break;

        default:
            {
                audio_mgr_dispatch(AUDIO_MSG_BUFFER_RESET, buffer_ent);
            }
            break;
        }
    }
    else
    {
        if (exc == MEDIA_BUFFER_FULL)
        {
            media_buffer_flush(buffer_ent, media_buffer_ds_pkt_cnt(buffer_ent),
                               BUFFER_DIR_DOWNSTREAM);
            audio_mgr_dispatch(AUDIO_MSG_BUFFER_RESET, buffer_ent);
        }
    }
}

bool audio_remote_join_set(bool enable,
                           bool force_join)
{
    T_AUDIO_REMOTE_SEAMLESS_JOIN_SET msg;

    msg.enable = enable;
    msg.force_join = force_join;

    audio_remote_seamless_join_set(enable, force_join);
    remote_async_msg_relay(audio_db->relay_handle,
                           AUDIO_REMOTE_SEAMLESS_JOIN_SET,
                           &msg,
                           sizeof(T_AUDIO_REMOTE_SEAMLESS_JOIN_SET),
                           false);
    return true;
}
