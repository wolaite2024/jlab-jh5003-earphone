/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_PATH_H_
#define _AUDIO_PATH_H_

#include <stdint.h>
#include <stdbool.h>
#include "audio_type.h"
#include "vad.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum t_audio_path_event
{
    AUDIO_PATH_EVT_NONE                 = 0x0000,
    AUDIO_PATH_EVT_CREATE               = 0x0001,
    AUDIO_PATH_EVT_IDLE                 = 0x0002,
    AUDIO_PATH_EVT_EFFECT_REQ           = 0x0004,
    AUDIO_PATH_EVT_RUNNING              = 0x0005,
    AUDIO_PATH_EVT_DATA_REQ             = 0x0007,
    AUDIO_PATH_EVT_RELEASE              = 0x0009,
    AUDIO_PATH_EVT_DATA_IND             = 0x000A,
    AUDIO_PATH_EVT_SUSPEND              = 0x000B,
    AUDIO_PATH_EVT_DATA_EMPTY           = 0x000C,
    AUDIO_PATH_EVT_PLC_REPORT           = 0x000D,
    AUDIO_PATH_EVT_DSP_SYNC_UNLOCK      = 0x000F,
    AUDIO_PATH_EVT_DSP_SYNC_LOCK        = 0x0010,
    AUDIO_PATH_EVT_RETURN_JOIN_INFO     = 0x0012,
    AUDIO_PATH_EVT_DSP_INTER_MSG        = 0x0013,
    AUDIO_PATH_EVT_FIFO_REPORT          = 0x0014,
    AUDIO_PATH_EVT_PIPE_MIX             = 0x0015,
    AUDIO_PATH_EVT_PIPE_DEMIX           = 0x0016,
    AUDIO_PATH_EVT_SIGNAL_REFRESH       = 0x0017,
    AUDIO_PATH_EVT_ERROR_REPORT         = 0x0019,
    AUDIO_PATH_EVT_VSE_RESPONSE         = 0x001B,
    AUDIO_PATH_EVT_VSE_INDICATE         = 0x001C,
    AUDIO_PATH_EVT_VSE_NOTIFY           = 0x001D,
    AUDIO_PATH_EVT_KWS_HIT              = 0x001E,
    AUDIO_PATH_EXC_DSP_UNSYNC           = 0x1001,
    AUDIO_PATH_EXC_DSP_SYNC_EMPTY       = 0x1002,
    AUDIO_PATH_EXC_DSP_LOST_TIMESTAMP   = 0x1003,
} T_AUDIO_PATH_EVENT;

typedef enum t_audio_path_msg_type
{
    AUDIO_PATH_MSG_TYPE_PLUGIN  = 0x00,
    AUDIO_PATH_MSG_TYPE_CREATE  = 0x01,
    AUDIO_PATH_MSG_TYPE_DESTROY = 0x02,
} T_AUDIO_PATH_MSG_TYPE;

typedef void *T_AUDIO_PATH_HANDLE;
typedef bool (*P_AUDIO_PATH_CBACK)(T_AUDIO_PATH_HANDLE handle,
                                   T_AUDIO_PATH_EVENT  event,
                                   uint32_t            param);

typedef struct t_audio_path_data_hdr
{
    uint32_t sync_word;
    uint32_t session_id;
    uint32_t timestamp;
    uint16_t seq_num;
    uint16_t frame_count;
    uint8_t  frame_number;
    uint8_t  status;
    uint16_t payload_length;
    uint32_t tail;
    uint8_t  payload[0];
} T_AUDIO_PATH_DATA_HDR;

typedef struct t_anc_llapt_cfg
{
    uint8_t sub_type;
    uint8_t scenario_id;
} T_ANC_LLAPT_CFG;

typedef struct t_audio_path_msg
{
    T_AUDIO_PATH_MSG_TYPE type;
    uint8_t               occasion;
    T_AUDIO_PATH_HANDLE   handle;
    P_AUDIO_PATH_CBACK    cback;
} T_AUDIO_PATH_MSG;

typedef struct t_audio_param
{
    uint32_t             device;
    T_AUDIO_FORMAT_INFO *decoder_info;
    T_AUDIO_STREAM_MODE  mode;
    uint8_t              dac_level;
} T_AUDIO_PARAM;

typedef struct t_voice_param
{
    uint32_t             device;
    T_AUDIO_FORMAT_INFO *decoder_info;
    T_AUDIO_FORMAT_INFO *encoder_info;
    T_AUDIO_STREAM_MODE  mode;
    uint8_t              dac_level;
    uint8_t              adc_level;
} T_VOICE_PARAM;

typedef struct t_record_param
{
    uint32_t             device;
    T_AUDIO_FORMAT_INFO *encoder_info;
    T_AUDIO_STREAM_MODE  mode;
    uint8_t              adc_level;
} T_RECORD_PARAM;

typedef struct t_line_param
{
    uint32_t             device;
    uint32_t             dac_sample_rate;
    uint32_t             adc_sample_rate;
    uint8_t              dac_level;
    uint8_t              adc_level;
} T_LINE_PARAM;

typedef struct t_ringtone_instance
{
    void     *coeff;
    uint16_t  size;
} T_RINGTONE_INSTANCE;

typedef struct t_ringtone_param
{
    uint32_t             device;
    T_RINGTONE_INSTANCE *instance;
    uint8_t              dac_level;
} T_RINGTONE_PARAM;

typedef struct t_voice_prompt_param
{
    uint32_t             device;
    T_AUDIO_FORMAT_INFO *decoder_info;
    uint8_t              dac_level;
} T_VOICE_PROMPT_PARAM;

typedef struct t_apt_param
{
    uint32_t             device;
    uint32_t             dac_sample_rate;
    uint32_t             adc_sample_rate;
    uint8_t              dac_level;
    uint8_t              adc_level;
} T_APT_PARAM;

typedef struct t_llapt_param
{
    uint32_t             device;
    uint8_t              dac_level;
    uint8_t              adc_level;
} T_LLAPT_PARAM;

typedef struct t_anc_param
{
    uint32_t             device;
    uint8_t              dac_level;
    uint8_t              adc_level;
} T_ANC_PARAM;

typedef struct t_vad_param
{
    uint32_t             device;
    T_AUDIO_FORMAT_INFO *encoder_info;
    T_VAD_TYPE           type;
    T_AUDIO_STREAM_MODE  mode;
    uint8_t              adc_level;
    uint8_t              aggressiveness_level;
} T_VAD_PARAM;

typedef struct t_kws_param
{
    T_AUDIO_STREAM_MODE  mode;
    T_AUDIO_FORMAT_INFO *decoder_info;
} T_KWS_PARAM;

typedef struct t_pipe_param
{
    T_AUDIO_FORMAT_INFO *decoder_info;
    T_AUDIO_FORMAT_INFO *encoder_info;
    T_AUDIO_STREAM_MODE  mode;
} T_PIPE_PARAM;

typedef union t_audio_path_param
{
    T_AUDIO_PARAM        audio;
    T_VOICE_PARAM        voice;
    T_RECORD_PARAM       record;
    T_LINE_PARAM         line;
    T_RINGTONE_PARAM     ringtone;
    T_VOICE_PROMPT_PARAM vp;
    T_APT_PARAM          apt;
    T_LLAPT_PARAM        llapt;
    T_ANC_PARAM          anc;
    T_VAD_PARAM          vad;
    T_KWS_PARAM          kws;
    T_PIPE_PARAM         pipe;
} T_AUDIO_PATH_PARAM;

typedef struct t_audio_path_latency_report
{
    uint16_t    normal_packet_count;
    uint16_t    average_packet_latency;
    uint16_t    average_fifo_queuing;
} T_AUDIO_PATH_LATENCY_REPORT;

typedef struct t_audio_path_plc_notify
{
    uint32_t total_sample_num;
    uint32_t plc_sample_num;
} T_AUDIO_PATH_PLC_NOTIFY;

typedef enum t_audio_path_signal_direction
{
    AUDIO_PATH_DIR_TX,
    AUDIO_PATH_DIR_RX,
} T_AUDIO_PATH_SIGNAL_DIRECTION;

typedef struct t_audio_path_signal_refresh
{
    T_AUDIO_PATH_SIGNAL_DIRECTION  direction;
    uint8_t                        freq_num;
    uint16_t                      *left_gain_table;
    uint16_t                      *right_gain_table;
} T_AUDIO_PATH_SIGNAL_REFRESH;

typedef struct t_audio_path_vse_response
{
    uint16_t    company_id;
    uint16_t    effect_id;
    uint16_t    seq_num;
    uint16_t    payload_len;
    uint8_t    *payload;
} T_AUDIO_PATH_VSE_RESPONSE;

typedef struct t_audio_path_vse_indicate
{
    uint16_t    company_id;
    uint16_t    effect_id;
    uint16_t    seq_num;
    uint16_t    payload_len;
    uint8_t    *payload;
} T_AUDIO_PATH_VSE_INDICATE;

typedef struct t_audio_path_vse_notify
{
    uint16_t    company_id;
    uint16_t    effect_id;
    uint16_t    seq_num;
    uint16_t    payload_len;
    uint8_t    *payload;
} T_AUDIO_PATH_VSE_NOTIFY;

typedef struct t_audio_path_kws_hit
{
    uint8_t     keyword_index;
} T_AUDIO_PATH_KWS_HIT;

typedef bool (*P_AUDIO_PATH_MGR_CBACK)(uint32_t  id,
                                       void     *msg);

bool audio_path_init(P_AUDIO_PATH_MGR_CBACK cback);

void audio_path_deinit(void);

T_AUDIO_PATH_HANDLE audio_path_create(T_AUDIO_CATEGORY   category,
                                      T_AUDIO_PATH_PARAM param,
                                      P_AUDIO_PATH_CBACK cback);

bool audio_path_destroy(T_AUDIO_PATH_HANDLE handle);

bool audio_path_start(T_AUDIO_PATH_HANDLE handle);

bool audio_path_stop(T_AUDIO_PATH_HANDLE handle);

bool audio_path_data_send(T_AUDIO_PATH_HANDLE    handle,
                          T_AUDIO_PATH_DATA_HDR *buf,
                          uint32_t               len);

uint16_t audio_path_data_recv(T_AUDIO_PATH_HANDLE    handle,
                              T_AUDIO_PATH_DATA_HDR *buf,
                              uint16_t               len);

uint16_t audio_path_data_peek(T_AUDIO_PATH_HANDLE handle);

bool audio_path_dac_level_set(T_AUDIO_PATH_HANDLE handle,
                              uint8_t             level,
                              float               scale);

bool audio_path_dac_mute(T_AUDIO_PATH_HANDLE handle);

bool audio_path_adc_level_set(T_AUDIO_PATH_HANDLE handle,
                              uint8_t             level,
                              float               scale);

bool audio_path_adc_mute(T_AUDIO_PATH_HANDLE handle);

bool audio_path_power_off(void);

bool audio_path_power_on(void);

bool audio_path_is_running(T_AUDIO_PATH_HANDLE handle);

bool audio_path_sw_sidetone_enable(T_AUDIO_PATH_HANDLE handle,
                                   int16_t             gain_step);

bool audio_path_sw_sidetone_disable(T_AUDIO_PATH_HANDLE handle);

void audio_path_fifo_report(T_AUDIO_PATH_HANDLE handle,
                            uint16_t            interval);

bool audio_path_decoder_effect_control(T_AUDIO_PATH_HANDLE handle,
                                       uint8_t             action);

bool audio_path_encoder_effect_control(T_AUDIO_PATH_HANDLE handle,
                                       uint8_t             action);

bool audio_path_hw_sidetone_enable(int16_t gain,
                                   uint8_t level);

bool audio_path_hw_sidetone_disable(void);

/* dsp rws2.0 */
bool audio_path_timestamp_set(T_AUDIO_PATH_HANDLE handle,
                              uint8_t             clk_ref,
                              uint32_t            timestamp,
                              bool                sync_flag);

bool audio_path_synchronization_role_swap(T_AUDIO_PATH_HANDLE handle,
                                          uint8_t             role,
                                          bool                start);

bool audio_path_synchronization_data_send(T_AUDIO_PATH_HANDLE  handle,
                                          uint8_t             *buf,
                                          uint16_t             len);

void audio_path_b2bmsg_interaction_timeout(void);

bool audio_path_synchronization_join_set(T_AUDIO_PATH_HANDLE handle,
                                         uint8_t             role);

void audio_path_lpm_set(bool enable);

bool audio_path_configure(T_AUDIO_PATH_HANDLE  handle,
                          void                *cfg);

void audio_path_brightness_set(T_AUDIO_PATH_HANDLE handle,
                               float               strength);

void audio_path_msg_send(T_AUDIO_PATH_MSG *msg);

bool audio_path_signal_monitoring_set(T_AUDIO_PATH_HANDLE           handle,
                                      T_AUDIO_PATH_SIGNAL_DIRECTION direction,
                                      bool                          enable,
                                      uint16_t                      refresh_interval,
                                      uint8_t                       freq_num,
                                      uint32_t                      freq_table[]);

bool audio_path_channel_out_reorder(T_AUDIO_PATH_HANDLE handle,
                                    uint8_t             channel_num,
                                    uint32_t            channel_array[]);

bool audio_path_channel_in_reorder(T_AUDIO_PATH_HANDLE handle,
                                   uint8_t             channel_num,
                                   uint32_t            channel_array[]);

bool audio_path_asrc_set(T_AUDIO_PATH_HANDLE handle,
                         uint8_t             toggle,
                         int32_t             ratio,
                         uint32_t            timestamp);

bool audio_path_decoder_plc_set(T_AUDIO_PATH_HANDLE handle,
                                bool                enable,
                                uint16_t            interval,
                                uint32_t            threshold);

bool audio_path_kws_set(T_AUDIO_PATH_HANDLE handle);

bool audio_path_kws_clear(T_AUDIO_PATH_HANDLE handle);

bool audio_path_vad_filter(T_AUDIO_PATH_HANDLE handle,
                           uint8_t             level);

bool audio_path_eq_out_set(T_AUDIO_PATH_HANDLE  handle,
                           uint8_t             *info_buf,
                           uint16_t             info_len);

bool audio_path_eq_in_set(T_AUDIO_PATH_HANDLE  handle,
                          uint8_t             *info_buf,
                          uint16_t             info_len);

bool audio_path_eq_out_clear(T_AUDIO_PATH_HANDLE handle);

bool audio_path_eq_in_clear(T_AUDIO_PATH_HANDLE handle);

bool audio_path_nrec_set(T_AUDIO_PATH_HANDLE handle,
                         uint8_t             mode,
                         uint8_t             level);

bool audio_path_nrec_clear(T_AUDIO_PATH_HANDLE handle);

bool audio_path_ovp_set(T_AUDIO_PATH_HANDLE handle,
                        uint8_t             level);

bool audio_path_ovp_clear(T_AUDIO_PATH_HANDLE handle);

bool audio_path_beamforming_set(T_AUDIO_PATH_HANDLE handle,
                                uint8_t             direction);

bool audio_path_beamforming_clear(T_AUDIO_PATH_HANDLE handle);

bool audio_path_vse_req(T_AUDIO_PATH_HANDLE  handle,
                        uint16_t             company_id,
                        uint16_t             effect_id,
                        uint16_t             seq_num,
                        uint8_t             *info_buf,
                        uint16_t             info_len);

bool audio_path_vse_cfm(T_AUDIO_PATH_HANDLE  handle,
                        uint16_t             company_id,
                        uint16_t             effect_id,
                        uint16_t             seq_num,
                        uint8_t             *info_buf,
                        uint16_t             info_len);

bool audio_path_pipe_gain_set(T_AUDIO_PATH_HANDLE handle,
                              int16_t             gain_left,
                              int16_t             gain_right);

bool audio_path_pipe_pre_mix(T_AUDIO_PATH_HANDLE prime_handle,
                             T_AUDIO_PATH_HANDLE auxiliary_handle);

bool audio_path_pipe_post_mix(T_AUDIO_PATH_HANDLE prime_handle,
                              T_AUDIO_PATH_HANDLE auxiliary_handle);

bool audio_path_pipe_demix(T_AUDIO_PATH_HANDLE prime_handle,
                           T_AUDIO_PATH_HANDLE auxiliary_handle);
bool audio_path_wdrc_set(T_AUDIO_PATH_HANDLE handle, uint8_t *info_buf, uint16_t info_len);

bool audio_path_wdrc_clear(T_AUDIO_PATH_HANDLE handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_PATH_H_ */
