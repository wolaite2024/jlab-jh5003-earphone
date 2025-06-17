/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_REMOTE_H_
#define _AUDIO_REMOTE_H_

#include <stdint.h>
#include <stdbool.h>
#include "audio_type.h"
#include "audio_mgr.h"
#include "media_buffer.h"
#include "remote.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum t_audio_remote_state
{
    AUDIO_REMOTE_STATE_UNSYNC,
    AUDIO_REMOTE_STATE_ALIGNING,
    AUDIO_REMOTE_STATE_SEAMLESS_JOIN,
    AUDIO_REMOTE_STATE_SYNCED,
} T_AUDIO_REMOTE_STATE;

typedef enum t_audio_remote_evt
{
    AUDIO_REMOTE_EVENT_BUFFER_SYNC,
    AUDIO_REMOTE_EVENT_ALIGNED,
    AUDIO_REMOTE_EVENT_SYNC_PLAY_RSV,
    AUDIO_REMOTE_EVENT_SYNC_PLAY_START,
} T_AUDIO_REMOTE_EVT;

typedef struct t_media_packet_record
{
    struct t_media_packet_record *p_next;
    uint32_t                      timestamp;
    int32_t                       seq_offset;
    uint16_t                      upper_seq_num;
    uint16_t                      local_seq_num;
    uint16_t                      frame_counter;
    uint8_t                       frame_number;
} T_MEDIA_PACKET_RECORD;

typedef struct t_audio_latency_record
{
    bool     dynamic_latency_on;
    uint16_t plc_count;
    uint16_t latency;
    uint16_t latency_back_up;
    uint8_t  latency_override;
} T_AUDIO_REMOTE_LATENCY_INFO;

typedef struct t_audio_remote_buffer_sync_unit
{
    uint16_t    local_seq_num;
    uint16_t    frame_counter;
    uint16_t    upper_seq_num;
    uint16_t    rsv;
    uint32_t    timestamp;
} T_AUDIO_REMOTE_BUFFER_SYNC_UNIT;

typedef void *T_AUDIO_REMOTE_HANDLE;
typedef void (*T_AUDIO_REMOTE_EVENT_CBACK)(T_AUDIO_REMOTE_EVT  event,
                                           void               *param,
                                           void               *owner);
typedef void (*T_AUDIO_REMOTE_RELAY_CBACK)(uint16_t               event,
                                           void                  *param,
                                           T_REMOTE_RELAY_STATUS  status,
                                           void                  *owner);

bool audio_remote_init(void);
void audio_remote_deinit(void);

T_AUDIO_REMOTE_HANDLE audio_remote_register(T_AUDIO_STREAM_TYPE         type,
                                            void                       *buffer_ent,
                                            T_AUDIO_REMOTE_EVENT_CBACK  event_cback,
                                            T_AUDIO_REMOTE_RELAY_CBACK  relay_cback);
void audio_remote_unregister(T_AUDIO_REMOTE_HANDLE handle);
void audio_remote_reset(T_AUDIO_REMOTE_HANDLE handle);
void audio_remote_set_state(T_AUDIO_REMOTE_HANDLE handle,
                            T_AUDIO_REMOTE_STATE  state);
T_AUDIO_REMOTE_STATE audio_remote_get_state(T_AUDIO_REMOTE_HANDLE handle);

void audio_remote_seamless_join_set(bool enable, bool force_join);
bool audio_remote_seamless_join_seq_contiuity_check(T_AUDIO_REMOTE_HANDLE handle);
void audio_remote_dsp_seamless_join_handle(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                                           uint32_t               join_clk,
                                           uint32_t               join_frame,
                                           uint32_t               delta_frame);
bool audio_remote_find_join_pkt(T_AUDIO_REMOTE_HANDLE handle);
void audio_remote_seamless_join_retry(T_AUDIO_REMOTE_HANDLE handle,
                                      T_AUDIO_MGR_EXCEPTION exc);
void audio_remote_cancel_seamless_join(T_AUDIO_REMOTE_HANDLE handle);

void audio_remote_ultra_low_latency_sync_play(T_AUDIO_REMOTE_HANDLE handle,
                                              bool                  dynamic_latency_on);
void audio_remote_low_latency_sync_play(T_AUDIO_REMOTE_HANDLE handle,
                                        bool                  danamic_latency_on);
void audio_remote_appoint_playtime(T_AUDIO_REMOTE_HANDLE handle,
                                   uint32_t              set_timing_ms,
                                   bool                  dynamic_latency_on);
bool audio_remote_align_buffer(T_AUDIO_REMOTE_HANDLE handle);
bool audio_remote_period_sync_check(T_AUDIO_REMOTE_HANDLE handle,
                                    uint16_t              buffer_sync_trigger_cnt);
void audio_remote_buffer_sync_req(T_AUDIO_REMOTE_HANDLE handle);

void audio_remote_record_init(T_AUDIO_REMOTE_HANDLE handle,
                              uint16_t              size);
void audio_remote_record_deinit(T_AUDIO_REMOTE_HANDLE handle);
void audio_remote_record_clear(T_AUDIO_REMOTE_HANDLE handle);
void audio_remote_record_push(T_AUDIO_REMOTE_HANDLE  handle,
                              void                  *p_packet);
T_MEDIA_PACKET_RECORD *audio_remote_record_search(T_AUDIO_REMOTE_HANDLE handle,
                                                  uint16_t              upper_seq);
int32_t audio_remote_record_update(T_AUDIO_REMOTE_HANDLE handle,
                                   uint16_t              local_seq,
                                   uint16_t              frame_counter,
                                   uint8_t               frame_number);
void audio_remote_record_merge(T_AUDIO_REMOTE_HANDLE            handle,
                               T_MEDIA_PACKET_RECORD           *record,
                               T_AUDIO_REMOTE_BUFFER_SYNC_UNIT *p_unit);
uint16_t audio_remote_record_pop(T_AUDIO_REMOTE_HANDLE            handle,
                                 T_AUDIO_REMOTE_BUFFER_SYNC_UNIT *p_data,
                                 uint16_t                         count);
uint16_t audio_remote_record_peek(T_AUDIO_REMOTE_HANDLE            handle,
                                  T_AUDIO_REMOTE_BUFFER_SYNC_UNIT *p_data,
                                  uint16_t                         count);

bool audio_remote_is_buffer_unsync(T_AUDIO_REMOTE_HANDLE handle);
bool audio_remote_buffer_PLC(T_AUDIO_REMOTE_HANDLE handle);
void audio_remote_set_frame_diff(T_AUDIO_REMOTE_HANDLE handle,
                                 int32_t               frame_diff);
int32_t audio_remote_get_frame_diff(T_AUDIO_REMOTE_HANDLE handle);
void audio_remote_handle_lost_packet(T_AUDIO_REMOTE_HANDLE handle,
                                     uint8_t               count);
void audio_remote_sync_lock(T_AUDIO_REMOTE_HANDLE handle);
bool audio_remote_async_msg_relay(T_AUDIO_REMOTE_HANDLE  handle,
                                  uint16_t               msg_id,
                                  void                  *msg_buf,
                                  uint16_t               msg_len,
                                  bool                   loopback);
bool audio_remote_sync_msg_relay(T_AUDIO_REMOTE_HANDLE  handle,
                                 uint16_t               msg_id,
                                 void                  *msg_buf,
                                 uint16_t               msg_len,
                                 T_REMOTE_TIMER_TYPE    timer_type,
                                 uint32_t               timer_period,
                                 bool                   loopback);
bool audio_remote_get_seamless_join(void);
bool audio_remote_get_force_join(void);
uint16_t audio_remote_play_margin_get(T_AUDIO_FORMAT_TYPE type);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_REMOTE_H_ */
