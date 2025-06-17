/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <stdbool.h>
#include <stdint.h>

#include "audio_track.h"
#include "audio_line.h"
#include "audio_passthrough.h"
#include "tts.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup  AUDIO_MANAGER_AUDIO Audio
 *
 * \brief Audio module init, register/unregister callback and change volume.
 */

/**
 * \brief  Audio Manager event type for \ref P_AUDIO_MGR_CBACK.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef enum t_audio_event
{
    AUDIO_EVENT_UNKNOWN                             = 0x0000,

    /**
     * \brief Indicate the audio volume out changed \ref T_AUDIO_EVENT_PARAM_VOLUME_OUT_CHANGED.
     */
    AUDIO_EVENT_VOLUME_OUT_CHANGED                  = 0x0001,

    /**
     * \brief Indicate the audio volume out muted \ref T_AUDIO_EVENT_PARAM_VOLUME_OUT_MUTED.
     */
    AUDIO_EVENT_VOLUME_OUT_MUTED                    = 0x0002,

    /**
     * \brief Indicate the audio volume out unmuted \ref T_AUDIO_EVENT_PARAM_VOLUME_OUT_UNMUTED.
     */
    AUDIO_EVENT_VOLUME_OUT_UNMUTED                  = 0x0003,

    /**
     * \brief Indicate the audio volume in changed \ref T_AUDIO_EVENT_PARAM_VOLUME_IN_CHANGED.
     */
    AUDIO_EVENT_VOLUME_IN_CHANGED                   = 0x0004,

    /**
     * \brief Indicate the audio volume in muted \ref T_AUDIO_EVENT_PARAM_VOLUME_IN_MUTED.
     */
    AUDIO_EVENT_VOLUME_IN_MUTED                     = 0x0005,

    /**
     * \brief Indicate the audio volume in unmuted \ref T_AUDIO_EVENT_PARAM_VOLUME_IN_UNMUTED.
     */
    AUDIO_EVENT_VOLUME_IN_UNMUTED                   = 0x0006,

    /**
     * \brief Indicate the audio volume balance changed \ref T_AUDIO_EVENT_PARAM_VOLUME_BALANCE_CHANGED.
     */
    AUDIO_EVENT_VOLUME_BALANCE_CHANGED              = 0x0007,

    /**
     * \brief Indicate the TTS started \ref T_AUDIO_EVENT_PARAM_TTS_STARTED.
     */
    AUDIO_EVENT_TTS_STARTED                         = 0x0100,

    /**
     * \brief Indicate the TTS alerted \ref T_AUDIO_EVENT_PARAM_TTS_ALERTED.
     */
    AUDIO_EVENT_TTS_ALERTED                         = 0x0101,

    /**
     * \brief Indicate the TTS paused \ref T_AUDIO_EVENT_PARAM_TTS_PAUSED.
     */
    AUDIO_EVENT_TTS_PAUSED                          = 0x0102,

    /**
     * \brief Indicate the TTS resumed \ref T_AUDIO_EVENT_PARAM_TTS_RESUMED.
     */
    AUDIO_EVENT_TTS_RESUMED                         = 0x0103,

    /**
     * \brief Indicate the TTS stopped \ref T_AUDIO_EVENT_PARAM_TTS_STOPPED.
     */
    AUDIO_EVENT_TTS_STOPPED                         = 0x0104,

    /**
     * \brief Indicate the TTS exceeded \ref T_AUDIO_EVENT_PARAM_TTS_EXCEEDED.
     */
    AUDIO_EVENT_TTS_EXCEEDED                        = 0x0105,

    /**
     * \brief Indicate the TTS empty \ref T_AUDIO_EVENT_PARAM_TTS_EMPTY.
     */
    AUDIO_EVENT_TTS_EMPTY                           = 0x0106,

    /**
     * \brief Indicate the Ringtone started \ref T_AUDIO_EVENT_PARAM_RINGTONE_STARTED.
     */
    AUDIO_EVENT_RINGTONE_STARTED                    = 0x0110,

    /**
     * \brief Indicate the Ringtone stopped \ref T_AUDIO_EVENT_PARAM_RINGTONE_STOPPED.
     */
    AUDIO_EVENT_RINGTONE_STOPPED                    = 0x0111,

    /**
     * \brief Indicate the Voice Prompt started \ref T_AUDIO_EVENT_PARAM_VOICE_PROMPT_STARTED.
     */
    AUDIO_EVENT_VOICE_PROMPT_STARTED                = 0x0120,

    /**
     * \brief Indicate the Voice Prompt stopped \ref T_AUDIO_EVENT_PARAM_VOICE_PROMPT_STOPPED.
     */
    AUDIO_EVENT_VOICE_PROMPT_STOPPED                = 0x0121,

    /**
     * \brief Indicate the Audio Track session state changed \ref T_AUDIO_EVENT_PARAM_TRACK_STATE_CHANGED.
     */
    AUDIO_EVENT_TRACK_STATE_CHANGED                 = 0x0200,

    /**
     * \brief Indicate the Audio Track session volume out changed \ref T_AUDIO_EVENT_PARAM_TRACK_VOLUME_OUT_CHANGED.
     */
    AUDIO_EVENT_TRACK_VOLUME_OUT_CHANGED            = 0x0201,

    /**
     * \brief Indicate the Audio Track session volume out muted \ref T_AUDIO_EVENT_PARAM_TRACK_VOLUME_OUT_MUTED.
     */
    AUDIO_EVENT_TRACK_VOLUME_OUT_MUTED              = 0x0202,

    /**
     * \brief Indicate the Audio Track session volume out unmuted \ref T_AUDIO_EVENT_PARAM_TRACK_VOLUME_OUT_UNMUTED.
     */
    AUDIO_EVENT_TRACK_VOLUME_OUT_UNMUTED            = 0x0203,

    /**
     * \brief Indicate the Audio Track session volume in changed \ref T_AUDIO_EVENT_PARAM_TRACK_VOLUME_IN_CHANGED.
     */
    AUDIO_EVENT_TRACK_VOLUME_IN_CHANGED             = 0x0204,

    /**
     * \brief Indicate the Audio Track session volume in muted \ref T_AUDIO_EVENT_PARAM_TRACK_VOLUME_IN_MUTED.
     */
    AUDIO_EVENT_TRACK_VOLUME_IN_MUTED               = 0x0205,

    /**
     * \brief Indicate the Audio Track session volume in unmuted \ref T_AUDIO_EVENT_PARAM_TRACK_VOLUME_IN_UNMUTED.
     */
    AUDIO_EVENT_TRACK_VOLUME_IN_UNMUTED             = 0x0206,

    /**
     * \brief Indicate the Audio Track session data received \ref T_AUDIO_EVENT_PARAM_TRACK_DATA_IND.
     */
    AUDIO_EVENT_TRACK_DATA_IND                      = 0x0207,

    /**
     * \brief Indicate the Audio Track buffer low alerted \ref T_AUDIO_EVENT_PARAM_TRACK_BUFFER_LOW.
     */
    AUDIO_EVENT_TRACK_BUFFER_LOW                    = 0x0208,

    /**
     * \brief Indicate the Audio Track buffer high alerted \ref T_AUDIO_EVENT_PARAM_TRACK_BUFFER_HIGH.
     */
    AUDIO_EVENT_TRACK_BUFFER_HIGH                   = 0x0209,

    /**
     * \brief Indicate the Audio Track signal out strength in dB \ref T_AUDIO_EVENT_PARAM_TRACK_SIGNAL_OUT_REFRESHED.
     */
    AUDIO_EVENT_TRACK_SIGNAL_OUT_REFRESHED          = 0x020a,

    /**
     * \brief Indicate the Audio Track signal in strength in dB \ref T_AUDIO_EVENT_PARAM_TRACK_SIGNAL_IN_REFRESHED.
     */
    AUDIO_EVENT_TRACK_SIGNAL_IN_REFRESHED           = 0x020b,

    /**
     * \brief Report the Audio Track latency \ref T_AUDIO_EVENT_PARAM_TRACK_LATENCY_REPORTED.
     */
    AUDIO_EVENT_TRACK_LATENCY_REPORTED              = 0x020c,

    /**
     * \brief Indicate the Audio Track error \ref T_AUDIO_EVENT_PARAM_TRACK_ERROR_REPORT.
     */
    AUDIO_EVENT_TRACK_ERROR_REPORT                  = 0x020d,

    /**
     * \brief Report the Audio Track PLC info \ref T_AUDIO_MSG_PAYLOAD_TRACK_PLC_REPORT.
     */
    AUDIO_EVENT_TRACK_PLC_REPORT                  = 0x020e,

    /**
     * \brief Indicate the Audio Track decoding with no valid packet.
     */
    AUDIO_EVENT_TRACK_DECODE_EMPTY                  = 0x0210,


    /**
     * \brief Indicate the VAD enabled.
     */
    AUDIO_EVENT_VAD_ENABLED                         = 0x0300,

    /**
     * \brief Indicate the VAD disabled.
     */
    AUDIO_EVENT_VAD_DISABLED                        = 0x0301,

    /**
     * \brief Indicate the VAD data indicated \ref T_AUDIO_EVENT_PARAM_VAD_DATA_IND.
     */
    AUDIO_EVENT_VAD_DATA_IND                        = 0x0302,

    /**
     * \brief Indicate the KWS enabled.
     */
    AUDIO_EVENT_KWS_ENABLED                         = 0x0303,

    /**
     * \brief Indicate the KWS disabled.
     */
    AUDIO_EVENT_KWS_DISABLED                        = 0x0304,

    /**
     * \brief Indicate the KWS hit \ref T_AUDIO_EVENT_PARAM_KWS_HIT.
     */
    AUDIO_EVENT_KWS_HIT                             = 0x0305,

    /**
     * \brief Indicate the ANC enabled \ref T_AUDIO_EVENT_PARAM_ANC_ENABLED.
     */
    AUDIO_EVENT_ANC_ENABLED                         = 0x0306,

    /**
     * \brief Indicate the ANC disabled \ref T_AUDIO_EVENT_PARAM_ANC_DISABLED.
     */
    AUDIO_EVENT_ANC_DISABLED                        = 0x0307,

    /**
     * \brief Indicate Passthrough enabled \ref T_AUDIO_EVENT_PARAM_AUDIO_PASSTHROUGH_ENABLED.
     */
    AUDIO_EVENT_PASSTHROUGH_ENABLED                 = 0x0310,

    /**
     * \brief Indicate Passthrough disabled \ref T_AUDIO_EVENT_PARAM_AUDIO_PASSTHROUGH_DISABLED.
     */
    AUDIO_EVENT_PASSTHROUGH_DISABLED                = 0x0311,

    /**
     * \brief Indicate the Line started \ref T_AUDIO_EVENT_PARAM_LINE_STARTED.
     */
    AUDIO_EVENT_LINE_STARTED                        = 0x0400,

    /**
     * \brief Indicate the Line stopped \ref T_AUDIO_EVENT_PARAM_LINE_STOPPED.
     */
    AUDIO_EVENT_LINE_STOPPED                        = 0x0401,

    /**
     * \brief Indicate the Line volume out changed \ref T_AUDIO_EVENT_PARAM_LINE_VOLUME_OUT_CHANGED.
     */
    AUDIO_EVENT_LINE_VOLUME_OUT_CHANGED             = 0x0402,

    /**
     * \brief Indicate the Line volume in changed \ref T_AUDIO_EVENT_PARAM_LINE_VOLUME_IN_CHANGED.
     */
    AUDIO_EVENT_LINE_VOLUME_IN_CHANGED              = 0x0403,

    /**
     * \brief Indicate the Line volume balance changed \ref T_AUDIO_EVENT_PARAM_LINE_VOLUME_BALANCE_CHANGED.
     */
    AUDIO_EVENT_LINE_VOLUME_BALANCE_CHANGED         = 0x0404,

    /**
     * \brief Indicate the Line signal out strength in dB \ref T_AUDIO_EVENT_PARAM_LINE_SIGNAL_OUT_REFRESHED.
     */
    AUDIO_EVENT_LINE_SIGNAL_OUT_REFRESHED           = 0x0405,

    /**
     * \brief Indicate the Line signal in strength in dB \ref T_AUDIO_EVENT_PARAM_LINE_SIGNAL_IN_REFRESHED.
     */
    AUDIO_EVENT_LINE_SIGNAL_IN_REFRESHED            = 0x0406,

    /**
     * \brief Indicate the Line error \ref T_AUDIO_EVENT_PARAM_LINE_ERROR_REPORT.
     */
    AUDIO_EVENT_LINE_ERROR_REPORT                   = 0x0407,

    /**
     * \brief Indicate the buffer state turns to playing.
     */
    AUDIO_EVENT_BUFFER_STATE_PLAYING                = 0x0408,
} T_AUDIO_EVENT;

/**
 * \brief  Audio Manager event parameter audio volume out changed.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_volume_out_changed
{
    T_AUDIO_STREAM_TYPE type;
    uint8_t             prev_volume;
    uint8_t             curr_volume;
} T_AUDIO_EVENT_PARAM_VOLUME_OUT_CHANGED;

/**
 * \brief  Audio Manager event parameter audio volume out muted.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_volume_out_muted
{
    T_AUDIO_STREAM_TYPE type;
    uint8_t             volume;
} T_AUDIO_EVENT_PARAM_VOLUME_OUT_MUTED;

/**
 * \brief  Audio Manager event parameter audio volume out unmuted.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_volume_out_unmuted
{
    T_AUDIO_STREAM_TYPE type;
    uint8_t             volume;
} T_AUDIO_EVENT_PARAM_VOLUME_OUT_UNMUTED;

/**
 * \brief  Audio Manager event parameter audio volume in changed.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_volume_in_changed
{
    T_AUDIO_STREAM_TYPE type;
    uint8_t             prev_volume;
    uint8_t             curr_volume;
} T_AUDIO_EVENT_PARAM_VOLUME_IN_CHANGED;

/**
 * \brief  Audio Manager event parameter audio volume in muted.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_volume_in_muted
{
    T_AUDIO_STREAM_TYPE type;
    uint8_t             volume;
} T_AUDIO_EVENT_PARAM_VOLUME_IN_MUTED;

/**
 * \brief  Audio Manager event parameter audio volume in unmuted.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_volume_in_unmuted
{
    T_AUDIO_STREAM_TYPE type;
    uint8_t             volume;
} T_AUDIO_EVENT_PARAM_VOLUME_IN_UNMUTED;

/**
 * \brief  Audio Manager event parameter audio volume balance changed.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_volume_balance_changed
{
    T_AUDIO_STREAM_TYPE type;
    float               prev_scale;
    float               curr_scale;
} T_AUDIO_EVENT_PARAM_VOLUME_BALANCE_CHANGED;

/**
 * \brief  Audio Manager event parameter TTS started.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_tts_started
{
    T_TTS_HANDLE handle;
} T_AUDIO_EVENT_PARAM_TTS_STARTED;

/**
 * \brief  Audio Manager event parameter TTS alerted.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_tts_alerted
{
    T_TTS_HANDLE handle;
} T_AUDIO_EVENT_PARAM_TTS_ALERTED;

/**
 * \brief  Audio Manager event parameter TTS paused.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_tts_paused
{
    T_TTS_HANDLE handle;
} T_AUDIO_EVENT_PARAM_TTS_PAUSED;

/**
 * \brief  Audio Manager event parameter TTS resumed.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_tts_resumed
{
    T_TTS_HANDLE handle;
} T_AUDIO_EVENT_PARAM_TTS_RESUMED;

/**
 * \brief  Audio Manager event parameter TTS stopped.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_tts_stopped
{
    T_TTS_HANDLE handle;
} T_AUDIO_EVENT_PARAM_TTS_STOPPED;

/**
 * \brief  Audio Manager event parameter TTS exceeded.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_tts_exceeded
{
    T_TTS_HANDLE handle;
} T_AUDIO_EVENT_PARAM_TTS_EXCEEDED;

/**
 * \brief  Audio Manager event parameter TTS empty.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_tts_empty
{
    T_TTS_HANDLE handle;
} T_AUDIO_EVENT_PARAM_TTS_EMPTY;

/**
 * \brief  Audio Manager event parameter Ringtone started.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_ringtone_started
{
    uint8_t     index;
} T_AUDIO_EVENT_PARAM_RINGTONE_STARTED;

/**
 * \brief  Audio Manager event parameter Ringtone stopped.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_ringtone_stopped
{
    uint8_t     index;
} T_AUDIO_EVENT_PARAM_RINGTONE_STOPPED;

/**
 * \brief  Audio Manager event parameter Voice Prompt started.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_voice_prompt_started
{
    uint8_t     index;
} T_AUDIO_EVENT_PARAM_VOICE_PROMPT_STARTED;

/**
 * \brief  Audio Manager event parameter Voice Prompt stopped.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_voice_prompt_stopped
{
    uint8_t     index;
} T_AUDIO_EVENT_PARAM_VOICE_PROMPT_STOPPED;

/**
 * \brief  Audio Manager event parameter Passthrough started.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_passthrough_started
{
    T_AUDIO_PASSTHROUGH_MODE     mode;
} T_AUDIO_EVENT_PARAM_PASSTHROUGH_STARTED;

/**
 * \brief  Audio Manager event parameter Passthrough stopped.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_passthrough_stopped
{
    T_AUDIO_PASSTHROUGH_MODE     mode;
} T_AUDIO_EVENT_PARAM_PASSTHROUGH_STOPPED;

/**
 * \brief   Audio Track state changed event parameter for \ref AUDIO_EVENT_TRACK_STATE_CHANGED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_state_changed
{
    T_AUDIO_TRACK_HANDLE handle;
    T_AUDIO_TRACK_STATE  state;
    uint8_t              cause;
} T_AUDIO_EVENT_PARAM_TRACK_STATE_CHANGED;

/**
 * \brief   Audio Track volume out changed event parameter for \ref AUDIO_EVENT_TRACK_VOLUME_OUT_CHANGED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_volume_out_changed
{
    T_AUDIO_TRACK_HANDLE handle;
    uint8_t              prev_volume;
    uint8_t              curr_volume;
} T_AUDIO_EVENT_PARAM_TRACK_VOLUME_OUT_CHANGED;

/**
 * \brief   Audio Track volume out muted event parameter for \ref AUDIO_EVENT_TRACK_VOLUME_OUT_MUTED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_volume_out_muted
{
    T_AUDIO_TRACK_HANDLE handle;
    uint8_t              volume;
} T_AUDIO_EVENT_PARAM_TRACK_VOLUME_OUT_MUTED;

/**
 * \brief   Audio Track volume out unmuted event parameter for \ref AUDIO_EVENT_TRACK_VOLUME_OUT_UNMUTED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_volume_out_unmuted
{
    T_AUDIO_TRACK_HANDLE handle;
    uint8_t              volume;
} T_AUDIO_EVENT_PARAM_TRACK_VOLUME_OUT_UNMUTED;

/**
 * \brief   Audio Track volume in changed event parameter for \ref AUDIO_EVENT_TRACK_VOLUME_IN_CHANGED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_volume_in_changed
{
    T_AUDIO_TRACK_HANDLE handle;
    uint8_t              prev_volume;
    uint8_t              curr_volume;
} T_AUDIO_EVENT_PARAM_TRACK_VOLUME_IN_CHANGED;

/**
 * \brief   Audio Track volume in muted event parameter for \ref AUDIO_EVENT_TRACK_VOLUME_IN_MUTED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_volume_in_muted
{
    T_AUDIO_TRACK_HANDLE handle;
    uint8_t              volume;
} T_AUDIO_EVENT_PARAM_TRACK_VOLUME_IN_MUTED;

/**
 * \brief   Audio Track volume in unmuted event parameter for \ref AUDIO_EVENT_TRACK_VOLUME_IN_UNMUTED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_volume_in_unmuted
{
    T_AUDIO_TRACK_HANDLE handle;
    uint8_t              volume;
} T_AUDIO_EVENT_PARAM_TRACK_VOLUME_IN_UNMUTED;

/**
 * \brief   Audio Track buffer low event parameter for \ref AUDIO_EVENT_TRACK_BUFFER_LOW.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_buffer_low
{
    T_AUDIO_TRACK_HANDLE handle;
    uint16_t  buffer_level_ms;
} T_AUDIO_EVENT_PARAM_TRACK_BUFFER_LOW;

/**
 * \brief   Audio Track buffer high event parameter for \ref AUDIO_EVENT_TRACK_BUFFER_HIGH.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_buffer_high
{
    T_AUDIO_TRACK_HANDLE handle;
    uint16_t  buffer_level_ms;
} T_AUDIO_EVENT_PARAM_TRACK_BUFFER_HIGH;

/**
 * \brief   Audio Track data indicated event parameter for \ref AUDIO_EVENT_TRACK_DATA_IND.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_data_ind
{
    T_AUDIO_TRACK_HANDLE handle;
    uint16_t             len;
} T_AUDIO_EVENT_PARAM_TRACK_DATA_IND;

/**
 * \brief   Audio Track data indicated event parameter for \ref AUDIO_EVENT_TRACK_SIGNAL_OUT_REFRESHED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_signal_out_refreshed
{
    T_AUDIO_TRACK_HANDLE handle;
    uint8_t              freq_num;
    uint16_t            *left_gain_table;
    uint16_t            *right_gain_table;
} T_AUDIO_EVENT_PARAM_TRACK_SIGNAL_OUT_REFRESHED;

/**
 * \brief   Audio Track data indicated event parameter for \ref AUDIO_EVENT_TRACK_SIGNAL_IN_REFRESHED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_signal_in_refreshed
{
    T_AUDIO_TRACK_HANDLE handle;
    uint8_t              freq_num;
    uint16_t            *left_gain_table;
    uint16_t            *right_gain_table;
} T_AUDIO_EVENT_PARAM_TRACK_SIGNAL_IN_REFRESHED;

/**
 * \brief   Audio Track data indicated event parameter for \ref T_AUDIO_EVENT_PARAM_TRACK_LATENCY_REPORTED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_latency_reported
{
    T_AUDIO_TRACK_HANDLE handle;
    /**
     * Audio Track preset latency in milliseconds.
     */
    uint16_t             preset_latency;
    /**
     * Audio Track stream's instant latency from reception to rendering.
     */
    uint16_t             instant_latency;
    /**
     * Audio Track stream's de-jitter compensation based on preset latency.
     */
    int32_t              dejitter_compensation;
} T_AUDIO_EVENT_PARAM_TRACK_LATENCY_REPORTED;

/**
 * \brief   Audio Track error report event parameter for \ref AUDIO_EVENT_TRACK_ERROR_REPORT.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_error_report
{
    T_AUDIO_TRACK_HANDLE handle;
} T_AUDIO_EVENT_PARAM_TRACK_ERROR_REPORT;

/**
 * \brief   Audio Track plc report event parameter for \ref AUDIO_EVENT_TRACK_PLC_REPORT.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_plc_report
{
    T_AUDIO_TRACK_HANDLE handle;

    /**
     * The total processed sample number, including the PLC sample number, is calculated during the
     * interval specified by \ref audio_track_plc_notify_set.
     */
    uint32_t             total_sample_num;

    /**
     * The PLC sample number is calculated during the interval specified by \ref audio_track_plc_notify_set.
     */
    uint32_t             plc_sample_num;
} T_AUDIO_EVENT_PARAM_TRACK_PLC_REPORT;

/**
 * \brief   Audio Track decode empty event parameter for \ref AUDIO_EVENT_TRACK_DECODE_EMPTY.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_decode_empty
{
    T_AUDIO_TRACK_HANDLE handle;
} T_AUDIO_EVENT_PARAM_TRACK_DECODE_EMPTY;

/**
 * \brief   Audio Line start event parameter for \ref AUDIO_EVENT_LINE_STARTED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_line_started
{
    T_AUDIO_LINE_HANDLE handle;
} T_AUDIO_EVENT_PARAM_LINE_STARTED;

/**
 * \brief   Audio Line stop event parameter for \ref AUDIO_EVENT_LINE_STOPPED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_line_stopped
{
    T_AUDIO_LINE_HANDLE handle;
} T_AUDIO_EVENT_PARAM_LINE_STOPPED;

/**
 * \brief  Audio Line volume out changed event parameter for \ref AUDIO_EVENT_LINE_VOLUME_OUT_CHANGED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_line_volume_out_changed
{
    T_AUDIO_LINE_HANDLE  handle;
    uint8_t              prev_volume;
    uint8_t              curr_volume;
} T_AUDIO_EVENT_PARAM_LINE_VOLUME_OUT_CHANGED;

/**
 * \brief  Audio Line volume in changed event parameter for \ref AUDIO_EVENT_LINE_VOLUME_IN_CHANGED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_line_volume_in_changed
{
    T_AUDIO_LINE_HANDLE handle;
    uint8_t             prev_volume;
    uint8_t             curr_volume;
} T_AUDIO_EVENT_PARAM_LINE_VOLUME_IN_CHANGED;

/**
 * \brief  Audio Line volume balance changed event parameter for \ref AUDIO_EVENT_LINE_VOLUME_BALANCE_CHANGED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_line_volume_balance_changed
{
    float               prev_scale;
    float               curr_scale;
} T_AUDIO_EVENT_PARAM_LINE_VOLUME_BALANCE_CHANGED;

/**
 * \brief   Audio Line signal out refreshed event parameter for \ref AUDIO_EVENT_LINE_SIGNAL_OUT_REFRESHED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_line_signal_out_refreshed
{
    T_AUDIO_LINE_HANDLE  handle;
    uint8_t              freq_num;
    uint16_t            *left_gain_table;
    uint16_t            *right_gain_table;
} T_AUDIO_EVENT_PARAM_LINE_SIGNAL_OUT_REFRESHED;

/**
 * \brief   Audio Line signal in refreshed event parameter for \ref AUDIO_EVENT_LINE_SIGNAL_IN_REFRESHED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_line_signal_in_refreshed
{
    T_AUDIO_LINE_HANDLE  handle;
    uint8_t              freq_num;
    uint16_t            *left_gain_table;
    uint16_t            *right_gain_table;
} T_AUDIO_EVENT_PARAM_LINE_SIGNAL_IN_REFRESHED;

/**
 * \brief   Audio Line error report event parameter for \ref AUDIO_EVENT_LINE_ERROR_REPORT.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_line_error_report
{
    T_AUDIO_LINE_HANDLE  handle;
} T_AUDIO_EVENT_PARAM_LINE_ERROR_REPORT;

/**
 * \brief   VAD data indicated event parameter for \ref AUDIO_EVENT_VAD_DATA_IND.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_vad_data_ind
{
    uint8_t   status;
    uint8_t   frame_num;
    uint32_t  timestamp;
    uint16_t  seq_num;
    uint16_t  payload_length;
    uint8_t  *payload;
} T_AUDIO_EVENT_PARAM_VAD_DATA_IND;

/**
 * \brief   KWS hit event parameter for \ref AUDIO_EVENT_KWS_HIT.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_kws_hit
{
    uint8_t  keyword_index;
} T_AUDIO_EVENT_PARAM_KWS_HIT;

/**
 * \brief   Define Audio Manager event parameter for \ref T_AUDIO_EVENT.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef union t_audio_event_param
{
    T_AUDIO_EVENT_PARAM_VOLUME_OUT_CHANGED          volume_out_changed;
    T_AUDIO_EVENT_PARAM_VOLUME_OUT_MUTED            volume_out_muted;
    T_AUDIO_EVENT_PARAM_VOLUME_OUT_UNMUTED          volume_out_unmuted;
    T_AUDIO_EVENT_PARAM_VOLUME_IN_CHANGED           volume_in_changed;
    T_AUDIO_EVENT_PARAM_VOLUME_IN_MUTED             volume_in_muted;
    T_AUDIO_EVENT_PARAM_VOLUME_IN_UNMUTED           volume_in_unmuted;
    T_AUDIO_EVENT_PARAM_VOLUME_BALANCE_CHANGED      volume_balance_changed;

    T_AUDIO_EVENT_PARAM_TTS_STARTED                 tts_started;
    T_AUDIO_EVENT_PARAM_TTS_ALERTED                 tts_alerted;
    T_AUDIO_EVENT_PARAM_TTS_PAUSED                  tts_paused;
    T_AUDIO_EVENT_PARAM_TTS_RESUMED                 tts_resumed;
    T_AUDIO_EVENT_PARAM_TTS_STOPPED                 tts_stopped;
    T_AUDIO_EVENT_PARAM_TTS_EXCEEDED                tts_exceeded;
    T_AUDIO_EVENT_PARAM_TTS_EMPTY                   tts_empty;

    T_AUDIO_EVENT_PARAM_RINGTONE_STARTED            ringtone_started;
    T_AUDIO_EVENT_PARAM_RINGTONE_STOPPED            ringtone_stopped;

    T_AUDIO_EVENT_PARAM_VOICE_PROMPT_STARTED        voice_prompt_started;
    T_AUDIO_EVENT_PARAM_VOICE_PROMPT_STOPPED        voice_prompt_stopped;

    T_AUDIO_EVENT_PARAM_PASSTHROUGH_STARTED         passthrough_started;
    T_AUDIO_EVENT_PARAM_PASSTHROUGH_STOPPED         passthrough_stopped;

    T_AUDIO_EVENT_PARAM_TRACK_STATE_CHANGED         track_state_changed;
    T_AUDIO_EVENT_PARAM_TRACK_VOLUME_OUT_CHANGED    track_volume_out_changed;
    T_AUDIO_EVENT_PARAM_TRACK_VOLUME_OUT_MUTED      track_volume_out_muted;
    T_AUDIO_EVENT_PARAM_TRACK_VOLUME_OUT_UNMUTED    track_volume_out_unmuted;
    T_AUDIO_EVENT_PARAM_TRACK_VOLUME_IN_CHANGED     track_volume_in_changed;
    T_AUDIO_EVENT_PARAM_TRACK_VOLUME_IN_MUTED       track_volume_in_muted;
    T_AUDIO_EVENT_PARAM_TRACK_VOLUME_IN_UNMUTED     track_volume_in_unmuted;
    T_AUDIO_EVENT_PARAM_TRACK_DATA_IND              track_data_ind;
    T_AUDIO_EVENT_PARAM_TRACK_SIGNAL_OUT_REFRESHED  track_signal_out_refreshed;
    T_AUDIO_EVENT_PARAM_TRACK_SIGNAL_IN_REFRESHED   track_signal_in_refreshed;
    T_AUDIO_EVENT_PARAM_TRACK_LATENCY_REPORTED      track_latency_reported;
    T_AUDIO_EVENT_PARAM_TRACK_ERROR_REPORT          track_error_report;
    T_AUDIO_EVENT_PARAM_TRACK_PLC_REPORT            track_plc_report;
    T_AUDIO_EVENT_PARAM_TRACK_DECODE_EMPTY          decode_empty;

    T_AUDIO_EVENT_PARAM_LINE_STARTED                line_started;
    T_AUDIO_EVENT_PARAM_LINE_STOPPED                line_stopped;
    T_AUDIO_EVENT_PARAM_LINE_VOLUME_OUT_CHANGED     line_volume_out_changed;
    T_AUDIO_EVENT_PARAM_LINE_VOLUME_IN_CHANGED      line_volume_in_changed;
    T_AUDIO_EVENT_PARAM_LINE_VOLUME_BALANCE_CHANGED line_volume_balance_changed;
    T_AUDIO_EVENT_PARAM_LINE_SIGNAL_OUT_REFRESHED   line_signal_out_refreshed;
    T_AUDIO_EVENT_PARAM_LINE_SIGNAL_IN_REFRESHED    line_signal_in_refreshed;
    T_AUDIO_EVENT_PARAM_LINE_ERROR_REPORT           line_error_report;

    T_AUDIO_EVENT_PARAM_VAD_DATA_IND                vad_data_ind;
    T_AUDIO_EVENT_PARAM_KWS_HIT                     kws_hit;
} T_AUDIO_EVENT_PARAM;

/**
 * \brief  Audio Manager event callback prototype.
 *
 * \param[in] event_type    Audio Manager event type.
 * \param[in] event_buf     Event buffer address.
 * \param[in] buf_len       Event buffer length.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef void (*P_AUDIO_MGR_CBACK)(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len);

/**
 * \brief  Audio Manager event callback register.
 *
 * \param[in] cback    Callback function \ref P_AUDIO_MGR_CBACK.
 *
 * \return         The status of Audio Manager event callback register.
 * \retval true    Audio Manager event callback was registered successfully.
 * \retval false   Audio Manager event callback was failed to register.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
bool audio_mgr_cback_register(P_AUDIO_MGR_CBACK cback);

/**
 * \brief  Audio Manager event callback unregister.
 *
 * \param[in] cback    Callback function \ref P_AUDIO_MGR_CBACK.
 *
 * \return         The status of Audio Manager event callback unregister.
 * \retval true    Audio Manager event callback was unregistered successfully.
 * \retval false   Audio Manager event callback was failed to unregister.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
bool audio_mgr_cback_unregister(P_AUDIO_MGR_CBACK cback);

/**
 * \brief  Initialize Audio Manager.
 *
 * \param[in] playback_pool_size        playback pool size in bytes.
 * \param[in] voice_pool_size           voice pool size in bytes.
 * \param[in] record_pool_size          record pool size in bytes.
 * \param[in] notification_pool_size    notification pool size in bytes.
 *
 * \return         The status of initializing Audio Manager.
 * \retval true    Audio Manager was initialized successfully.
 * \retval false   Audio Manager was failed to initialize.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
bool audio_mgr_init(uint16_t playback_pool_size,
                    uint16_t voice_pool_size,
                    uint16_t record_pool_size,
                    uint16_t notification_pool_size);

void audio_mgr_set_max_plc_count(uint8_t plc_count);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_H_ */
