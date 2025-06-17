/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _MEDIA_BUFFER_H_
#define _MEDIA_BUFFER_H_

#include <stdint.h>
#include <stdbool.h>
#include "media_pool.h"
#include "audio_type.h"

/* TODO Remove Start */
#include "os_queue.h"
/* TODO Remove End */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    BUFFER_DIR_DOWNSTREAM,
    BUFFER_DIR_UPSTREAM,
} T_MEDIA_BUFFER_DIRECTION;

typedef struct t_media_data_hdr
{
    struct t_media_data_hdr *p_next;
    uint32_t                 timestamp;
    uint16_t                 payload_length;
    uint16_t                 upper_seq_number;
    uint16_t                 read_offset;
    uint16_t                 frame_counter;
    uint16_t                 local_seq_number;
    uint8_t                  frame_number; //0 for useless, other for used
    uint8_t                  rsv[1];
    uint8_t                  p_data[0];
} T_MEDIA_DATA_HDR;

typedef enum t_media_buffer_state
{
    MEDIA_BUFFER_STATE_IDLE     = 0x00,
    MEDIA_BUFFER_STATE_PREQUEUE = 0x01,
    MEDIA_BUFFER_STATE_SYNC     = 0x02,
    MEDIA_BUFFER_STATE_PLAY     = 0x03,
} T_MEDIA_BUFFER_STATE;

typedef struct t_media_buffer_entity
{
    struct t_media_buffer_entity *p_next;
    void                         *audio_remote_handle;
    void                         *jitter_buffer_handle;
    void                         *audio_latency_handle;
    void                         *audio_interval_eval_handle;
    void                         *solo_play_timer;
    T_AUDIO_STREAM_TYPE           stream_type;
    T_AUDIO_STREAM_USAGE          usage;
    T_AUDIO_STREAM_MODE           mode;
    T_MEDIA_BUFFER_STATE          state;
    void                         *last_packet_ptr;
    uint32_t                      last_ts;
    uint32_t                      exception;
    uint16_t                      last_seq;
    uint16_t                      start_decode_frame_cnt;
    uint16_t                      max_latency_plc_count;
    uint16_t                      ds_frame_counter_last;
    uint16_t                      ds_local_seq_last;
    uint16_t                      buffer_sync_trigger_cnt;
    uint16_t                      plc_count;
    uint8_t                       miss_ack_cnt;
    bool                          last_seq_updated;
    bool                          local_audio_sync;
    uint8_t                       ignore_latency_report;
    uint8_t                       sec_path_ready;
    uint8_t                       media_buffer_fst_sync_tid;
    T_MEDIA_POOL_HANDLE           pool_handle;
    T_OS_QUEUE                    up_queue;
    T_OS_QUEUE                    down_queue;
    uint16_t                      up_frame_num;
    uint16_t                      down_frame_num;
    void                         *track_handle;
    void                         *path_handle;
    uint16_t                      lower_threshold_ms;
    uint16_t                      upper_threshold_ms;
    uint16_t                      latency;
    uint16_t                      latency_back_up;
    bool                          latency_fixed;
    uint8_t                       latency_override;
    uint8_t                       uuid[8];
    uint32_t                      restart_cause;
    T_AUDIO_FORMAT_INFO           format_info;
} T_MEDIA_BUFFER_ENTITY;

typedef void *T_MEDIA_BUFFER_PROXY;

bool media_buffer_init(uint16_t playback_pool_size,
                       uint16_t voice_pool_size,
                       uint16_t record_pool_size);
void media_buffer_deinit(void);

void media_buffer_proxy_uuid_set(T_MEDIA_BUFFER_PROXY proxy,
                                 uint8_t              uuid[8]);
void media_buffer_proxy_threshold_set(T_MEDIA_BUFFER_PROXY proxy,
                                      uint16_t             upper_threshold_ms,
                                      uint16_t             lower_threshold_ms);
void media_buffer_proxy_latency_set2(T_MEDIA_BUFFER_PROXY proxy,
                                     uint16_t             latency,
                                     bool                 fixed,
                                     float                var);
void media_buffer_proxy_latency_set(T_MEDIA_BUFFER_PROXY proxy,
                                    uint16_t             latency,
                                    bool                 fixed);
void media_buffer_proxy_latency_get(T_MEDIA_BUFFER_PROXY  proxy,
                                    uint16_t             *latency);
T_MEDIA_BUFFER_PROXY media_buffer_proxy_attach(T_AUDIO_STREAM_TYPE  stream_type,
                                               T_AUDIO_STREAM_USAGE usage,
                                               T_AUDIO_STREAM_MODE  mode,
                                               T_AUDIO_FORMAT_INFO *format_info,
                                               void                *track_handle,
                                               void                *path_handle);
void media_buffer_proxy_detach(T_MEDIA_BUFFER_PROXY proxy);
uint16_t media_buffer_proxy_us_write(T_MEDIA_BUFFER_PROXY  proxy,
                                     void                 *buf,
                                     uint16_t              len,
                                     uint32_t              timestamp,
                                     uint16_t              seq,
                                     T_AUDIO_STREAM_STATUS status,
                                     uint8_t               frame_num);
bool media_buffer_proxy_ds_write(T_MEDIA_BUFFER_PROXY  proxy,
                                 uint32_t              timestamp,
                                 uint16_t              seq,
                                 T_AUDIO_STREAM_STATUS status,
                                 uint8_t               frame_num,
                                 void                 *buf,
                                 uint16_t              len,
                                 uint16_t             *written_len);
uint16_t media_buffer_proxy_us_read(T_MEDIA_BUFFER_PROXY   proxy,
                                    void                  *buf,
                                    uint16_t               len,
                                    uint32_t              *timestamp,
                                    uint16_t              *seq_num,
                                    T_AUDIO_STREAM_STATUS *status,
                                    uint8_t               *frame_num);
T_MEDIA_BUFFER_STATE media_buffer_state_get(T_MEDIA_BUFFER_ENTITY *buffer_ent);
void media_buffer_state_set(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                            T_MEDIA_BUFFER_STATE   state);
bool media_buffer_downstream_dsp(T_MEDIA_BUFFER_ENTITY *buffer);
uint16_t media_buffer_ds_frame_cnt(T_MEDIA_BUFFER_ENTITY *buffer_ent);
uint16_t media_buffer_ds_pkt_cnt(T_MEDIA_BUFFER_ENTITY *buffer_ent);
uint16_t media_buffer_us_frame_cnt(T_MEDIA_BUFFER_ENTITY *buffer_ent);
uint16_t media_buffer_us_pkt_cnt(T_MEDIA_BUFFER_ENTITY *buffer_ent);
void media_buffer_proxy_reset(T_MEDIA_BUFFER_PROXY proxy);
uint16_t media_buffer_cacu_frame_num(uint16_t             time_ms,
                                     T_AUDIO_FORMAT_INFO *format_info);
uint16_t media_buffer_cacu_frames_duration(uint16_t             frame_num,
                                           T_AUDIO_FORMAT_INFO *format_info);
bool media_buffer_backup_downstream(T_MEDIA_BUFFER_ENTITY *buffer_ent);
bool media_buffer_proxy_level_get(T_MEDIA_BUFFER_PROXY  proxy,
                                  uint16_t             *level);
void media_buffer_proxy_fifo_report_config(T_MEDIA_BUFFER_PROXY proxy);
bool media_buffer_clear_downstream_info(T_MEDIA_BUFFER_ENTITY *buffer_ent);
bool media_buffer_set_downstream_info(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                                      uint16_t               ds_frame_counter_last,
                                      uint16_t               ds_local_seq_last);
bool media_buffer_get_downstream_info(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                                      uint16_t              *ds_frame_counter_last,
                                      uint16_t              *ds_local_seq_last);
bool media_buffer_get_stream_type(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                                  T_AUDIO_STREAM_TYPE   *type);
bool media_buffer_get_mode(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                           T_AUDIO_STREAM_MODE   *mode);
bool media_buffer_get_format_type(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                                  T_AUDIO_FORMAT_TYPE   *type);
void media_buffer_check_miss_ack(T_MEDIA_BUFFER_ENTITY *buffer_ent);
void media_buffer_leading_downstream(T_MEDIA_BUFFER_ENTITY *buffer_ent);
uint16_t media_buffer_playtime_calculate(T_MEDIA_BUFFER_ENTITY *buffer_ent);
void media_buffer_handle_plc_event(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                                   uint16_t               plc_total_frames,
                                   uint16_t               plc_frame_counter,
                                   uint16_t               plc_local_seq,
                                   uint8_t                plc_frame_num);
void media_buffer_dynamic_latency_resume(T_MEDIA_BUFFER_ENTITY *buffer_ent);
void media_buffer_dynamic_latency_set(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                                      uint16_t               latency,
                                      bool                   auto_resume);
void media_buffer_handle_sync_lock(T_MEDIA_BUFFER_ENTITY *buffer_ent);
void media_buffer_solo_play_timer_set(T_MEDIA_BUFFER_ENTITY *buffer_ent);
void media_buffer_relay_exception(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                                  uint8_t               *cmd);
void media_buffer_query_sec_path_state(T_MEDIA_BUFFER_ENTITY *buffer_ent);
void media_buffer_reset_relay(T_MEDIA_BUFFER_ENTITY *buffer_ent);
void media_buffer_proxy_inteval_evaluator_update(T_MEDIA_BUFFER_PROXY proxy,
                                                 uint16_t             seq,
                                                 uint32_t             clk);
bool media_buffer_proxy_inteval_evaluator_get_var(T_MEDIA_BUFFER_PROXY  proxy,
                                                  float                *var);

void *media_buffer_peek(T_MEDIA_BUFFER_ENTITY    *buffer_ent,
                        int32_t                   index,
                        T_MEDIA_BUFFER_DIRECTION  direction);
uint16_t media_buffer_shrink(T_MEDIA_BUFFER_ENTITY   *buffer_ent,
                             void                    *p_packet,
                             uint16_t                 len,
                             T_MEDIA_BUFFER_DIRECTION direction);
bool media_buffer_write(T_MEDIA_BUFFER_ENTITY    *buffer_ent,
                        uint32_t                  timestamp,
                        uint16_t                  seq_number,
                        uint8_t                   frame_number,
                        uint8_t                   status,
                        T_MEDIA_BUFFER_DIRECTION  direction,
                        void                     *data,
                        uint16_t                  len,
                        uint16_t                 *written_len);
bool media_buffer_flush(T_MEDIA_BUFFER_ENTITY    *buffer_ent,
                        uint16_t                  cnt,
                        T_MEDIA_BUFFER_DIRECTION  direction);
void media_buffer_reset(T_MEDIA_BUFFER_ENTITY *buffer_ent);
void media_buffer_dump(T_MEDIA_BUFFER_ENTITY *buffer_ent);
bool media_buffer_sync_lost(void);
T_MEDIA_BUFFER_ENTITY *media_buffer_find_buffer_by_uuid(T_AUDIO_STREAM_TYPE stream_type,
                                                        uint8_t             uuid[8]);
void media_buffer_set_max_plc_count(uint8_t plc_count);
uint8_t media_buffer_get_max_plc_count(void);
void *media_buffer_malloc(uint16_t buf_size);
bool media_buffer_free(void *p_buf);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MEDIA_BUFFER_H_ */
