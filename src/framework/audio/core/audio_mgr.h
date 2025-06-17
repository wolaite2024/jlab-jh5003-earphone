/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_MGR_H_
#define _AUDIO_MGR_H_

#include <stdint.h>
#include <stdbool.h>

#include "media_pool.h"
#include "media_buffer.h"
#include "audio_route.h"
#include "audio_track.h"
#include "audio_volume.h"
#include "tts.h"
#include "audio.h"
#include "audio_type.h"
#include "audio_path.h"
#include "audio_plugin.h"
#include "audio_line.h"
#include "audio_passthrough.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VOLUME_PLAYBACK_MAX_LEVEL       (15)
#define VOLUME_PLAYBACK_MIN_LEVEL       (0)
#define VOLUME_PLAYBACK_DEFAULT_LEVEL   (10)

#define VOLUME_VOICE_OUT_MAX_LEVEL      (15)
#define VOLUME_VOICE_OUT_MIN_LEVEL      (0)
#define VOLUME_VOICE_OUT_DEFAULT_LEVEL  (10)

#define VOLUME_VOICE_IN_MAX_LEVEL       (15)
#define VOLUME_VOICE_IN_MIN_LEVEL       (0)
#define VOLUME_VOICE_IN_DEFAULT_LEVEL   (10)

#define VOLUME_RECORD_MAX_LEVEL         (15)
#define VOLUME_RECORD_MIN_LEVEL         (0)
#define VOLUME_RECORD_DEFAULT_LEVEL     (10)

#define SOLO_PLAY_TIMER_TIMEOUT             2000000 //1// 4s
#define DYNAMIC_CHECK_TIME                  300000 //1// 300ms
#define RWS_TIME_TO_ABORT_SEAMLESS_JOIN     1000000 //1// 1s

#define DYNAMIC_DEC_CHECK_TIME              300000000UL //1// 5min
#define DYNAMIC_DEC_CHECK_TIME2             300000000UL //1// 5min
#define SEQUENCE_CHECK_TIME                 500000

#define MEDIA_SYNC_TRIGGER_CNT              3
#define MEDIA_SYNC_PEROID_TRIGGER_CNT       30
#define MEDIA_SYNC_PAKCET_CNT               MEDIA_SYNC_PEROID_TRIGGER_CNT/3
#define SYNC_PLAY_NOTI_PROTECT_MARGIN       10
#define DYNAMIC_LATENCY_MS                  100

#define MEDIA_SYNC_PEROID_TRIGGER_CNT_RTK       50
#define MEDIA_SYNC_PAKCET_CNT_RTK               MEDIA_SYNC_PEROID_TRIGGER_CNT_RTK/4
#define MAX_PLC_EXC_CNT                         1
#define MAX_SEAMLESS_JOIN_CHECK_LOST_COUNT      6
#define NORMAL_MODE_DYNAMIC_LATENCY_RATIO       0.15
#define LOW_LATENCY_DYNAMIC_LATENCY_MS          140
#define ULTRA_LOW_LATENCY_DYNAMIC_LATENCY_MS    60
#define LOW_LATENCY_ASRC_PROTECTION_THRESHOLD   4
#define DEFAULT_RESET_PLC_COUNT                 4
#define LATENCY_REPORT_PERIOD_MS                1000

typedef enum
{
    LATENCY_KEEPER_POLICY_BY_ASRC,
    LATENCY_KEEPER_POLICY_BY_PLC,
    LATENCY_KEEPER_POLICY_BY_BOTH,
} T_LATENCY_KEEPER_POLICY;

typedef enum
{
    MEDIA_BUFFER_EXC_GROUP          = 0x0100,
    MEDIA_BUFFER_EMPTY              = 0x0101,
    MEDIA_BUFFER_STATE_ERR          = 0x0102,
    MEDIA_BUFFER_ASRC_ADJ_OVERTIME  = 0x0103,
    MEDIA_BUFFER_JBC_CLK_ERR        = 0x0104,
    MEDIA_BUFFER_FST_PKT_ERR        = 0x0105,
    MEDIA_BUFFER_PLAY_OVERTIME      = 0x0106,
    MEDIA_BUFFER_PACKET_LOSS        = 0x0107,
    MEDIA_BUFFER_FULL               = 0x0108,
    MEDIA_BUFFER_MISS_PLAYTIME      = 0x0109,
    MEDIA_BUFFER_PATH_NOT_READY     = 0x010a,
    MEDIA_BUFFER_FST_SYNC_ERR       = 0x010b,
    MEDIA_BUFFER_SYNC_ERR           = 0x010c,
    MEDIA_BUFFER_PLAY_WO_SYNC       = 0x010d,
    MEDIA_BUFFER_PLAY_ERR_TID       = 0x010e,
    MEDIA_BUFFER_ERR_CLK            = 0x010f,
    MEDIA_BUFFER_ERR_TID            = 0x0110,
    MEDIA_BUFFER_SYNC_CLK_MISS      = 0x0111,
    MEDIA_BUFFER_DSP_PLC_ERR        = 0x0112,
    MEDIA_BUFFER_DSP_PLC_TOO_LATE   = 0x0113,
    MEDIA_BUFFER_SEAMLESS_MISS_PKT  = 0x0114,
    MEDIA_BUFFER_REMOTE_STATE_ERR   = 0x0115,
    MEDIA_BUFFER_MISS_SPEC_PKT      = 0x0116,
    MEDIA_BUFFER_ERR_SNIFF_CLK      = 0x0117,
    MEDIA_BUFFER_DSP_PLC_TOO_MUCH   = 0x0118,
    MEDIA_BUFFER_PRI_RESET_BUFFER   = 0x0119,
    MEDIA_BUFFER_SEAMLESS_TIMEOUT   = 0x011a,
    MEDIA_BUFFER_PACKET_LOST        = 0x011b,
    MEDIA_BUFFER_SEAMLESS_BFE_SYNC  = 0x011c,
    MEDIA_BUFFER_SEAMLESS_REFUSE    = 0x011f,

    DSP_EXC_GROUP                   = 0x0200,
    DSP_DECODE_EMPTY                = 0x0203,
    DSP_UNSYNC                      = 0x0206,
    DSP_SYNC_EMPTY                  = 0x0207,
    DSP_LOST_TIMESTAMP              = 0x0208,
} T_AUDIO_MGR_EXCEPTION;

typedef enum
{
    FST_SYNC_STATE_NONE,
    FST_SYNC_STATE_ERROR,
    FST_SYNC_DATA_EXPIRED,
    FST_SYNC_DATA_FLUSHED,
    FST_SYNC_DATA_NOT_FOUND,
    FST_SYNC_BUFFET_INACTIVE,
} T_FST_SYNC_ERR_REASON;

typedef struct
{
    T_AUDIO_MGR_EXCEPTION exc;
    uint8_t stream_type;
    uint32_t param;
} T_AUDIO_EXC_CMD;

typedef enum
{
    AUDIO_MSG_TRACK_CREATE                = 0x0300,
    AUDIO_MSG_TRACK_START                 = 0x0301,
    AUDIO_MSG_TRACK_STOP                  = 0x0302,
    AUDIO_MSG_TRACK_PAUSE                 = 0x0303,
    AUDIO_MSG_TRACK_RESTART               = 0x0304,
    AUDIO_MSG_TRACK_RELEASE               = 0x0305,
    AUDIO_MSG_TRACK_DATA_IND              = 0x0306,
    AUDIO_MSG_TRACK_DATA_XMIT             = 0x0307,
    AUDIO_MSG_TRACK_VOLUME_OUT_CHANGE     = 0x0308,
    AUDIO_MSG_TRACK_VOLUME_OUT_MUTE       = 0x0309,
    AUDIO_MSG_TRACK_VOLUME_OUT_UNMUTE     = 0x030A,
    AUDIO_MSG_TRACK_VOLUME_IN_CHANGE      = 0x030B,
    AUDIO_MSG_TRACK_VOLUME_IN_MUTE        = 0x030C,
    AUDIO_MSG_TRACK_VOLUME_IN_UNMUTE      = 0x030D,
    AUDIO_MSG_TRACK_SUSPEND               = 0x030E,
    AUDIO_MSG_TRACK_SUSPEND_RESUME        = 0x030F,
    AUDIO_MSG_TRACK_SIGNAL_OUT_REFRESH    = 0x0310,
    AUDIO_MSG_TRACK_SIGNAL_IN_REFRESH     = 0x0311,
    AUDIO_MSG_TRACK_ERROR_REPORT          = 0x0312,

    AUDIO_MSG_A2DP_DSP_SYNC_LOCK        = 0x0410,
    AUDIO_MSG_A2DP_DSP_INTER_MSG        = 0x0411,
    AUDIO_MSG_RETURN_JOIN_INFO          = 0x0414,

    AUDIO_MSG_VOLUME_OUT_MAX_SET        = 0x0600,
    AUDIO_MSG_VOLUME_OUT_MIN_SET        = 0x0601,
    AUDIO_MSG_VOLUME_OUT_CHANGE         = 0x0602,
    AUDIO_MSG_VOLUME_OUT_MUTE           = 0x0603,
    AUDIO_MSG_VOLUME_OUT_UNMUTE         = 0x0604,
    AUDIO_MSG_VOLUME_IN_MAX_SET         = 0x0606,
    AUDIO_MSG_VOLUME_IN_MIN_SET         = 0x0607,
    AUDIO_MSG_VOLUME_IN_CHANGE          = 0x0608,
    AUDIO_MSG_VOLUME_IN_MUTED           = 0x0609,
    AUDIO_MSG_VOLUME_IN_UNMUTE          = 0x060A,
    AUDIO_MSG_VOLUME_BALANCE_CHANGE     = 0x060C,

    AUDIO_MSG_KWS_ENABLE                 = 0x0706,
    AUDIO_MSG_KWS_DISABLE                = 0x0707,
    AUDIO_MSG_KWS_HIT                    = 0x0708,
    AUDIO_MSG_PASSTHROUGH_ENABLE         = 0x0709,
    AUDIO_MSG_PASSTHROUGH_DISABLE        = 0x070A,

    AUDIO_MSG_BUFFER_WRITE              = 0x0800,
    AUDIO_MSG_STREAM_PLC_REPORT         = 0x0801,
    AUDIO_MSG_BUFFER_RESET              = 0x0804,
    AUDIO_MSG_BUFFER_LEVEL_LOW          = 0x0805,
    AUDIO_MSG_BUFFER_LEVEL_HIGH         = 0x0806,
    AUDIO_MSG_DSP_DECODE_EMPTY          = 0x0807,
    AUDIO_MSG_BUFFER_STATE_PLAYING      = 0x080a,
    AUDIO_MSG_DSP_UNSYNC                = 0x080c,
    AUDIO_MSG_DSP_SYNC_UNLOCK           = 0x080d,
    AUDIO_MSG_DSP_SYNC_EMPTY            = 0x080e,
    AUDIO_MSG_DSP_LOST_TIMESTAMP        = 0x080f,
    AUDIO_MSG_STREAM_LATENCY_REPORT     = 0x0810,
} T_AUDIO_MSG;

typedef struct t_audio_msg_payload_stream_latency_report
{
    T_MEDIA_BUFFER_PROXY    buffer_proxy;
    uint16_t                normal_packet_count;
    uint16_t                average_packet_latency;
    uint16_t                average_fifo_queuing;
} T_AUDIO_MSG_PAYLOAD_STREAM_LATENCY_REPORT;

typedef struct t_audio_msg_payload_passthrough_enable
{
    T_AUDIO_PASSTHROUGH_MODE mode;
} T_AUDIO_MSG_PAYLOAD_PASSTHROUGH_ENABLE;

typedef struct t_audio_msg_payload_passthrough_disable
{
    T_AUDIO_PASSTHROUGH_MODE mode;
} T_AUDIO_MSG_PAYLOAD_PASSTHROUGH_DISABLE;

typedef struct t_audio_msg_payload_track_create
{
    T_AUDIO_TRACK_HANDLE    handle;
    T_AUDIO_STREAM_TYPE     stream_type;
} T_AUDIO_MSG_PAYLOAD_TRACK_CREATE;

typedef struct t_audio_msg_payload_track_start
{
    T_AUDIO_TRACK_HANDLE    handle;
    T_AUDIO_STREAM_TYPE     stream_type;
    T_MEDIA_BUFFER_PROXY    buffer_proxy;
} T_AUDIO_MSG_PAYLOAD_TRACK_START;

typedef struct t_audio_msg_payload_track_stop
{
    T_AUDIO_TRACK_HANDLE    handle;
    T_AUDIO_STREAM_TYPE     stream_type;
} T_AUDIO_MSG_PAYLOAD_TRACK_STOP;

typedef struct t_audio_msg_payload_track_pause
{
    T_AUDIO_TRACK_HANDLE    handle;
    T_AUDIO_STREAM_TYPE     stream_type;
    T_MEDIA_BUFFER_PROXY    buffer_proxy;
} T_AUDIO_MSG_PAYLOAD_TRACK_PAUSE;

typedef struct t_audio_msg_payload_track_release
{
    T_AUDIO_TRACK_HANDLE    handle;
    T_AUDIO_STREAM_TYPE     stream_type;
} T_AUDIO_MSG_PAYLOAD_TRACK_RELEASE;

typedef struct t_audio_msg_payload_track_restart
{
    T_AUDIO_TRACK_HANDLE    handle;
    T_AUDIO_STREAM_TYPE     stream_type;
    T_MEDIA_BUFFER_PROXY    buffer_proxy;
} T_AUDIO_MSG_PAYLOAD_TRACK_RESTART;

typedef struct t_audio_msg_payload_track_volume_out_change
{
    T_AUDIO_TRACK_HANDLE    handle;
    uint8_t                 curr_volume;
    uint8_t                 prev_volume;
} T_AUDIO_MSG_PAYLOAD_TRACK_VOLUME_OUT_CHANGE;

typedef struct t_audio_msg_payload_track_volume_out_mute
{
    T_AUDIO_TRACK_HANDLE    handle;
    uint8_t                 volume;
} T_AUDIO_MSG_PAYLOAD_TRACK_VOLUME_OUT_MUTE;

typedef struct t_audio_msg_payload_track_volume_out_unmute
{
    T_AUDIO_TRACK_HANDLE    handle;
    uint8_t                 volume;
} T_AUDIO_MSG_PAYLOAD_TRACK_VOLUME_OUT_UNMUTE;

typedef struct t_audio_msg_payload_track_volume_in_change
{
    T_AUDIO_TRACK_HANDLE    handle;
    uint8_t                 curr_volume;
    uint8_t                 prev_volume;
} T_AUDIO_MSG_PAYLOAD_TRACK_VOLUME_IN_CHANGE;

typedef struct t_audio_msg_payload_track_volume_in_mute
{
    T_AUDIO_TRACK_HANDLE    handle;
    uint8_t                 volume;
} T_AUDIO_MSG_PAYLOAD_TRACK_VOLUME_IN_MUTE;

typedef struct t_audio_msg_payload_track_volume_in_unmute
{
    T_AUDIO_TRACK_HANDLE    handle;
    uint8_t                 volume;
} T_AUDIO_MSG_PAYLOAD_TRACK_VOLUME_IN_UNMUTE;

typedef struct t_audio_msg_payload_track_data_ind
{
    T_AUDIO_TRACK_HANDLE    handle;
    uint16_t                len;
} T_AUDIO_MSG_PAYLOAD_TRACK_DATA_IND;

typedef struct t_audio_msg_payload_kws_hit
{
    uint8_t  keyword_index;
} T_AUDIO_MSG_PAYLOAD_KWS_HIT;

typedef struct t_audio_msg_payload_track_signal_out_refresh
{
    T_AUDIO_TRACK_HANDLE    handle;
    uint8_t                 freq_num;
    uint16_t               *left_gain_table;
    uint16_t               *right_gain_table;
} T_AUDIO_MSG_PAYLOAD_TRACK_SIGNAL_OUT_REFRESH;

typedef struct t_audio_msg_payload_track_signal_in_refresh
{
    T_AUDIO_TRACK_HANDLE    handle;
    uint8_t                 freq_num;
    uint16_t               *left_gain_table;
    uint16_t               *right_gain_table;
} T_AUDIO_MSG_PAYLOAD_TRACK_SIGNAL_IN_REFRESH;

typedef struct t_audio_msg_payload_track_error_report
{
    T_AUDIO_TRACK_HANDLE    handle;
} T_AUDIO_MSG_PAYLOAD_TRACK_ERROR_REPORT;

typedef struct t_audio_msg_payload_volume_out_max_set
{
    T_AUDIO_STREAM_TYPE     type;
    uint8_t                 volume;
} T_AUDIO_MSG_PAYLOAD_VOLUME_OUT_MAX_SET;

typedef struct t_audio_msg_payload_volume_out_min_set
{
    T_AUDIO_STREAM_TYPE     type;
    uint8_t                 volume;
} T_AUDIO_MSG_PAYLOAD_VOLUME_OUT_MIN_SET;

typedef struct t_audio_msg_payload_volume_out_change
{
    T_AUDIO_STREAM_TYPE     type;
    uint8_t                 prev_volume;
    uint8_t                 curr_volume;
} T_AUDIO_MSG_PAYLOAD_VOLUME_OUT_CHANGE;

typedef struct t_audio_msg_payload_volume_out_mute
{
    T_AUDIO_STREAM_TYPE     type;
    uint8_t                 volume;
} T_AUDIO_MSG_PAYLOAD_VOLUME_OUT_MUTE;

typedef struct t_audio_msg_payload_volume_out_unmute
{
    T_AUDIO_STREAM_TYPE     type;
    uint8_t                 volume;
} T_AUDIO_MSG_PAYLOAD_VOLUME_OUT_UNMUTE;

typedef struct t_audio_msg_payload_volume_in_max_set
{
    T_AUDIO_STREAM_TYPE     type;
    uint8_t                 volume;
} T_AUDIO_MSG_PAYLOAD_VOLUME_IN_MAX_SET;

typedef struct t_audio_msg_payload_volume_in_min_set
{
    T_AUDIO_STREAM_TYPE     type;
    uint8_t                 volume;
} T_AUDIO_MSG_PAYLOAD_VOLUME_IN_MIN_SET;

typedef struct t_audio_msg_payload_volume_in_change
{
    T_AUDIO_STREAM_TYPE     type;
    uint8_t                 prev_volume;
    uint8_t                 curr_volume;
} T_AUDIO_MSG_PAYLOAD_VOLUME_IN_CHANGE;

typedef struct t_audio_msg_payload_volume_in_mute
{
    T_AUDIO_STREAM_TYPE     type;
    uint8_t                 volume;
} T_AUDIO_MSG_PAYLOAD_VOLUME_IN_MUTE;

typedef struct t_audio_msg_payload_volume_in_unmute
{
    T_AUDIO_STREAM_TYPE     type;
    uint8_t                 volume;
} T_AUDIO_MSG_PAYLOAD_VOLUME_IN_UNMUTE;

typedef struct t_audio_msg_payload_volume_balance_change
{
    T_AUDIO_STREAM_TYPE     type;
    float                   prev_scale;
    float                   curr_scale;
} T_AUDIO_MSG_PAYLOAD_VOLUME_BALANCE_CHANGE;

typedef struct t_audio_msg_payload_track_return_join_info
{
    T_MEDIA_BUFFER_PROXY    buffer_proxy;
    uint32_t                join_clk;
    uint32_t                join_frame;
    uint32_t                delta_frame;
} T_AUDIO_MSG_PAYLOAD_TRACK_RETURN_JOIN_INFO;

typedef struct t_audio_msg_payload_track_plc_report
{
    T_MEDIA_BUFFER_PROXY    buffer_proxy;
    uint32_t                total_sample_num;
    uint32_t                plc_sample_num;
} T_AUDIO_MSG_PAYLOAD_TRACK_PLC_REPORT;

typedef struct
{
    uint32_t join_clk;
    uint32_t join_frame;
    uint32_t delta_frame;
} T_AUDIO_MSG_RETURN_JOIN_INFO;

typedef struct
{
    uint32_t sync_clk;
    uint16_t upper_seq;
    uint8_t  tid;
    uint8_t  clk_ref;
    bool     dynamic_latency_on;
    bool     latency_override;
    uint16_t plc_count;
    uint16_t latency;
    uint16_t latency_backup;
} T_AUDIO_MSG_LOW_LATENCY_SYNC_PLAY;

typedef struct
{
    bool     dynamic_latency_on;
    bool     latency_override;
    uint16_t plc_count;
    uint16_t latency;
    uint16_t latency_backup;
} T_AUDIO_MSG_LOW_LATENCY_OVERRIDE;

typedef struct
{
    uint32_t join_clk;
    uint32_t packet_id;
    uint16_t packet_local_seq;
    uint16_t packet_frame_counter;
    uint8_t  stream_type;
    uint8_t  tid;
} T_AUDIO_MSG_JOIN_INFO;

typedef struct
{
    uint32_t    time_period;
    uint32_t    sync_clk;
    uint8_t     sync_type;
    uint8_t     tid;
    uint8_t     clk_ref;
} T_SYNC_PLAY_MSG;

typedef struct
{
    uint16_t previous_total_plc_count;
    uint16_t plc_count;
    uint8_t  auto_dec;
} T_DYNAMIC_LAT_INC_DEC;

typedef struct
{
    uint16_t target_lat_ms;
    uint16_t previous_lat_ms;
    uint8_t  auto_resume;
} T_DYNAMIC_LAT_SET;

typedef struct
{
    bool    seamless_join;
    bool    force_join;
} T_AUDIO_SYNC_PARAMS;

typedef struct
{
    bool enable;
    bool force_join;
} T_AUDIO_REMOTE_SEAMLESS_JOIN_SET;

typedef enum
{
    AUDIO_REMOTE_DSP_INTER_MSG              = 0x0019,

    AUDIO_REMOTE_SEAMLESS_JOIN_SET          = 0x070a,
    AUDIO_REMOTE_SYNC_PARAM                 = 0x070b,

} T_AUDIO_REMOTE_MSG;

void audio_mgr_exception(T_MEDIA_BUFFER_ENTITY *buffer_ent,
                         T_AUDIO_MGR_EXCEPTION  exc,
                         uint8_t                from_remote,
                         uint32_t               param);
bool audio_mgr_dispatch(T_AUDIO_MSG  msg,
                        void        *buf);
bool audio_mgr_event_post(T_AUDIO_EVENT  event_type,
                          void          *event_buf,
                          uint16_t       buf_len);
bool audio_volume_init(void);
void audio_volume_deinit(void);
bool audio_track_init(uint16_t playback_pool_size,
                      uint16_t voice_pool_size,
                      uint16_t record_pool_size);
void audio_track_deinit(void);
bool audio_track_volume_out_max_set(T_AUDIO_STREAM_TYPE type,
                                    uint8_t             volume);
bool audio_track_volume_out_min_set(T_AUDIO_STREAM_TYPE type,
                                    uint8_t             volume);
void audio_track_volume_out_set_all(T_AUDIO_STREAM_TYPE type,
                                    uint8_t             volume);
void audio_track_volume_out_mute_all(T_AUDIO_STREAM_TYPE type);
void audio_track_volume_out_unmute_all(T_AUDIO_STREAM_TYPE type);
bool audio_track_volume_in_max_set(T_AUDIO_STREAM_TYPE type,
                                   uint8_t             volume);
bool audio_track_volume_in_min_set(T_AUDIO_STREAM_TYPE type,
                                   uint8_t             volume);
void audio_track_volume_in_set_all(T_AUDIO_STREAM_TYPE type,
                                   uint8_t             volume);
void audio_track_volume_in_mute_all(T_AUDIO_STREAM_TYPE type);
void audio_track_volume_in_unmute_all(T_AUDIO_STREAM_TYPE type);
bool audio_track_volume_balance_set(T_AUDIO_STREAM_TYPE stream_type,
                                    float               scale);

bool audio_route_init(void);
void audio_route_deinit(void);
T_AUDIO_ROUTE_PATH audio_route_path_get(T_AUDIO_PATH_HANDLE handle,
                                        T_AUDIO_CATEGORY    category,
                                        uint32_t            device);
bool audio_route_path_bind(T_AUDIO_PATH_HANDLE handle,
                           T_AUDIO_ROUTE_PATH  path);
bool audio_route_path_unbind(T_AUDIO_PATH_HANDLE handle);
bool audio_route_path_add(T_AUDIO_PATH_HANDLE  handle,
                          T_AUDIO_ROUTE_ENTRY *entry);
bool audio_route_path_remove(T_AUDIO_PATH_HANDLE   handle,
                             T_AUDIO_ROUTE_IO_TYPE io_type);
uint8_t audio_route_ramp_gain_scaling_get(T_AUDIO_CATEGORY category);
bool audio_route_hw_eq_set(T_AUDIO_ROUTE_HW_EQ_TYPE type,
                           uint8_t                  chann,
                           uint32_t                 sample_rate);

bool audio_effect_init(void);
void audio_effect_deinit(void);

bool vad_init(void);
void vad_deinit(void);

bool kws_init(void);
void kws_deinit(void);

bool anc_init(void);
void anc_deinit(void);
bool audio_passthrough_init(void);
void audio_passthrough_deinit(void);

bool audio_line_init(void);
void audio_line_deinit(void);

bool audio_pipe_init(void);
void audio_pipe_deinit(void);

bool audio_plugin_init(void);
void audio_plugin_deinit(void);
void audio_plugin_occasion_occur(T_AUDIO_PATH_HANDLE path,
                                 T_AUDIO_CATEGORY audio_category,
                                 T_AUDIO_PLUGIN_OCCASION occasion_type,
                                 T_AUDIO_PLUGIN_PARAM param);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_MGR_H_ */
