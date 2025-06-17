/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _DSP_MGR_H_
#define _DSP_MGR_H_

#include <stdint.h>
#include <stdbool.h>

#include "audio_type.h"
#include "vad.h"
#include "sys_ipc.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define RWS_ROLE_NONE               0x00
#define RWS_ROLE_SRC                0x01
#define RWS_ROLE_SNK                0x10

#define RWS_RESYNC_V2_OFF       0x00
#define RWS_RESYNC_V2_MASTER    0x01
#define RWS_RESYNC_V2_SLAVE     0x02

typedef enum t_dsp_mgr_state
{
    DSP_MGR_STATE_OFF           = 0x00,
    DSP_MGR_STATE_WAIT_ON       = 0x01,
    DSP_MGR_STATE_ON            = 0x02,
    DSP_MGR_STATE_WAIT_READY    = 0x03,
    DSP_MGR_STATE_READY         = 0x04,
    DSP_MGR_STATE_WAIT_FROZEN   = 0x05,
    DSP_MGR_STATE_FROZEN        = 0x06,
    DSP_MGR_STATE_WAIT_OFF      = 0x07,
} T_DSP_MGR_STATE;

typedef enum
{
    VAD_SCENARIO_A2DP = 0x0,
    VAD_SCENARIO_SCO = 0x1,
    VAD_SCENARIO_LINE_IN = 0x2,
    VAD_SCENARIO_IDLE = 0x3,
} T_VAD_SCENARIO;

typedef enum t_dsp_mgr_event
{
    DSP_MGR_EVT_POWER_OFF               = 0x01,
    DSP_MGR_EVT_POWER_ON                = 0x02,
    DSP_MGR_EVT_DSP_EXCEPTION           = 0x03,
    DSP_MGR_EVT_EFFECT_REQ              = 0x04,
    DSP_MGR_EVT_PREPARE_READY           = 0x05,
    DSP_MGR_EVT_REQUEST_EFFECT          = 0x06,
    DSP_MGR_EVT_PATH_SCHEDULE           = 0x07,
    DSP_MGR_EVT_DSP_INTER_MSG           = 0x08,
    DSP_MGR_EVT_CODEC_STATE             = 0x0E,
    DSP_MGR_EVT_SESSION_STARTED         = 0x0F,
    DSP_MGR_EVT_SESSION_STOPPED         = 0x10,
    DSP_MGR_EVT_MAILBOX_DSP_DATA        = 0x11,
    DSP_MGR_EVT_DSP_SYNC_V2_SUCC        = 0x15,
    DSP_MGR_EVT_DSP_UNSYNC              = 0x16,
    DSP_MGR_EVT_DSP_SYNC_UNLOCK         = 0x17,
    DSP_MGR_EVT_DSP_SYNC_LOCK           = 0x18,
    DSP_MGR_EVT_SYNC_EMPTY              = 0x19,
    DSP_MGR_EVT_SYNC_LOSE_TIMESTAMP     = 0x1A,
    DSP_MGR_EVT_DSP_JOIN_INFO           = 0x1B,
    DSP_MGR_EVT_DATA_REQ                = 0x1F,
    DSP_MGR_EVT_DATA_IND                = 0x20,
    DSP_MGR_EVT_DATA_COMPLETE           = 0x21,
    DSP_MGR_EVT_DATA_EMPTY              = 0x22,
    DSP_MGR_EVT_PLC_NOTIFY              = 0x23,
    DSP_MGR_EVT_SIGNAL_REFRESH          = 0x24,
    DSP_MGR_EVT_FIFO_NOTIFY             = 0x25,
    DSP_MGR_EVT_KWS_HIT                 = 0x26,
    DSP_MGR_EVT_ERROR_REPORT            = 0x27,

    DSP_MGR_EVT_CODEC_PIPE_MIX          = 0x30,
    DSP_MGR_EVT_CODEC_PIPE_DEMIX        = 0x31,

    DSP_MGR_EVT_VSE_RESPONSE            = 0x40,
    DSP_MGR_EVT_VSE_INDICATE            = 0x41,
    DSP_MGR_EVT_VSE_NOTIFY              = 0x42,
} T_DSP_MGR_EVENT;

typedef struct t_dsp_mgr_latency_notify
{
    uint16_t packet_number;
    uint16_t decoding_latency;
    uint16_t queuing_latency;
} T_DSP_MGR_LATENCY_NOTIFY;

typedef struct t_dsp_mgr_signal_refresh
{
    uint8_t   direction;
    uint8_t   freq_num;
    uint16_t *left_gain_table;
    uint16_t *right_gain_table;
} T_DSP_MGR_SIGNAL_REFRESH;

typedef struct t_dsp_mgr_plc_notify
{
    uint32_t total_sample_num;
    uint32_t plc_sample_num;
} T_DSP_MGR_PLC_NOTIFY;

typedef struct t_dsp_mgr_vse_response
{
    uint16_t  company_id;
    uint16_t  effect_id;
    uint16_t  seq_num;
    uint16_t  payload_len;
    uint8_t  *payload;
} T_DSP_MGR_VSE_RESPONSE;

typedef struct t_dsp_mgr_vse_indicate
{
    uint16_t  company_id;
    uint16_t  effect_id;
    uint16_t  seq_num;
    uint16_t  payload_len;
    uint8_t  *payload;
} T_DSP_MGR_VSE_INDICATE;

typedef struct t_dsp_mgr_vse_notify
{
    uint16_t  company_id;
    uint16_t  effect_id;
    uint16_t  seq_num;
    uint16_t  payload_len;
    uint8_t  *payload;
} T_DSP_MGR_VSE_NOTIFY;

typedef void *T_DSP_MGR_SESSION_HANDLE;

typedef void (*P_DSP_MGR_SESSION_CBACK)(void            *handle,
                                        T_DSP_MGR_EVENT  event,
                                        uint32_t         param);

typedef bool (*P_DSP_MGR_CBACK)(uint32_t  event,
                                void     *msg);

bool dsp_mgr_init(P_DSP_MGR_CBACK cback);

void dsp_mgr_deinit(void);

void dsp_mgr_power_on(void);

void dsp_mgr_power_off(void);

T_SYS_IPC_HANDLE dsp_mgr_register_cback(P_SYS_IPC_CBACK cback);

void dsp_mgr_unregister_cback(T_SYS_IPC_HANDLE handle);

T_DSP_MGR_SESSION_HANDLE dsp_mgr_session_create(T_AUDIO_CATEGORY          category,
                                                uint8_t                   data_mode,
                                                uint32_t                  dac_sample_rate,
                                                uint32_t                  adc_sample_rate,
                                                T_AUDIO_FORMAT_INFO      *decoder_info,
                                                T_AUDIO_FORMAT_INFO      *encoder_info,
                                                P_DSP_MGR_SESSION_CBACK   callback,
                                                uint32_t                  device,
                                                T_VAD_TYPE                vad_type,
                                                uint8_t                   aggressiveness_level,
                                                void                     *context);

bool dsp_mgr_session_destroy(T_DSP_MGR_SESSION_HANDLE handle);

bool dsp_mgr_session_enable(T_DSP_MGR_SESSION_HANDLE handle);

void dsp_mgr_session_run(T_DSP_MGR_SESSION_HANDLE handle);

bool dsp_mgr_session_disable(T_DSP_MGR_SESSION_HANDLE handle);

bool dsp_mgr_session_suspend(T_DSP_MGR_SESSION_HANDLE handle);

bool dsp_mgr_session_resume(T_DSP_MGR_SESSION_HANDLE handle);

bool dsp_mgr_decoder_gain_set(T_DSP_MGR_SESSION_HANDLE handle,
                              uint16_t                 gain_step_left,
                              uint16_t                 gain_step_right);

void dsp_mgr_decoder_ramp_gain_set(T_DSP_MGR_SESSION_HANDLE handle,
                                   uint16_t                 gain_step_left,
                                   uint16_t                 gain_step_right,
                                   uint16_t                 duration);

bool dsp_mgr_encoder_gain_set(T_DSP_MGR_SESSION_HANDLE handle,
                              uint16_t                 gain_step_left,
                              uint16_t                 gain_step_right);

bool dsp_mgr_signal_monitoring_set(T_DSP_MGR_SESSION_HANDLE handle,
                                   uint8_t                  direction,
                                   bool                     enable,
                                   uint16_t                 refresh_interval,
                                   uint8_t                  freq_num,
                                   uint32_t                 freq_table[]);

bool dsp_mgr_data_send(T_DSP_MGR_SESSION_HANDLE  handle,
                       uint8_t                  *buffer,
                       uint16_t                  length);

uint16_t dsp_mgr_data_len_peek(T_DSP_MGR_SESSION_HANDLE handle);

uint16_t dsp_mgr_data_recv(T_DSP_MGR_SESSION_HANDLE  handle,
                           uint8_t                  *buffer,
                           uint16_t                  length);

void dsp_mgr_signal_proc_start(T_DSP_MGR_SESSION_HANDLE handle,
                               uint32_t                 timestamp,
                               uint8_t                  clk_ref,
                               bool                     synsc_flag);

void dsp_rws_set_role(uint8_t session_role,
                      uint8_t role);
void dsp_rws_seamless(uint8_t role);

bool dsp_mgr_channel_out_reorder(T_DSP_MGR_SESSION_HANDLE handle,
                                 uint8_t                  channel_num,
                                 uint32_t                 channel_array[]);

bool dsp_mgr_channel_in_reorder(T_DSP_MGR_SESSION_HANDLE handle,
                                uint8_t                  channel_num,
                                uint32_t                 channel_array[]);

T_DSP_MGR_STATE dsp_mgr_state_get(void);

bool dsp_mgr_is_stable(T_DSP_MGR_SESSION_HANDLE handle);
bool dsp_mgr_power_on_check(void);

/*dsp2*/
uint8_t dsp_mgr_dsp2_ref_get(void);
void dsp_mgr_dsp2_ref_increment(void);
void dsp_mgr_dsp2_ref_decrement(void);

void dsp_mgr_decoder_fifo_report(T_DSP_MGR_SESSION_HANDLE handle,
                                 uint16_t                 interval);

bool dsp_mgr_decoder_effect_control(T_DSP_MGR_SESSION_HANDLE handle,
                                    uint8_t                  action);

bool dsp_mgr_encoder_effect_control(T_DSP_MGR_SESSION_HANDLE handle,
                                    uint8_t                  action);

bool dsp_mgr_asrc_set(T_DSP_MGR_SESSION_HANDLE handle,
                      uint8_t                  toggle,
                      int32_t                  ratio,
                      uint32_t                 timestamp);

bool dsp_mgr_decoder_plc_set(T_DSP_MGR_SESSION_HANDLE handle,
                             bool                     enable,
                             uint16_t                 interval,
                             uint32_t                 threshold);

bool dsp_mgr_kws_set(T_DSP_MGR_SESSION_HANDLE handle);

bool dsp_mgr_kws_clear(T_DSP_MGR_SESSION_HANDLE handle);

bool dsp_mgr_vad_filter(T_DSP_MGR_SESSION_HANDLE handle,
                        uint8_t                  level);

bool dsp_mgr_eq_out_set(T_DSP_MGR_SESSION_HANDLE  handle,
                        uint8_t                  *info_buf,
                        uint16_t                  info_len);

bool dsp_mgr_eq_in_set(T_DSP_MGR_SESSION_HANDLE  handle,
                       uint8_t                  *info_buf,
                       uint16_t                  info_len);

bool dsp_mgr_eq_out_clear(T_DSP_MGR_SESSION_HANDLE handle);

bool dsp_mgr_eq_in_clear(T_DSP_MGR_SESSION_HANDLE handle);

bool dsp_mgr_nrec_set(T_DSP_MGR_SESSION_HANDLE handle,
                      uint8_t                  mode,
                      uint8_t                  level);

bool dsp_mgr_nrec_clear(T_DSP_MGR_SESSION_HANDLE handle);

bool dsp_mgr_sidetone_set(T_DSP_MGR_SESSION_HANDLE handle,
                          uint16_t                 gain_step);

bool dsp_mgr_sidetone_clear(T_DSP_MGR_SESSION_HANDLE handle);

bool dsp_mgr_ovp_set(T_DSP_MGR_SESSION_HANDLE handle,
                     uint8_t                  level);

bool dsp_mgr_ovp_clear(T_DSP_MGR_SESSION_HANDLE handle);

bool dsp_mgr_beamforming_set(T_DSP_MGR_SESSION_HANDLE handle,
                             uint8_t                  direction);

bool dsp_mgr_beamforming_clear(T_DSP_MGR_SESSION_HANDLE handle);

bool dsp_mgr_wdrc_set(T_DSP_MGR_SESSION_HANDLE  handle,
                      uint8_t                  *info_buf,
                      uint16_t                  info_len);

bool dsp_mgr_wdrc_clear(T_DSP_MGR_SESSION_HANDLE handle);

bool dsp_mgr_vse_req(T_DSP_MGR_SESSION_HANDLE  handle,
                     uint16_t                  company_id,
                     uint16_t                  effect_id,
                     uint16_t                  seq_num,
                     uint8_t                  *info_buf,
                     uint16_t                  info_len);

bool dsp_mgr_vse_cfm(T_DSP_MGR_SESSION_HANDLE  handle,
                     uint16_t                  company_id,
                     uint16_t                  effect_id,
                     uint16_t                  seq_num,
                     uint8_t                  *info_buf,
                     uint16_t                  info_len);

void dsp_mgr_lpm_set(bool enable);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DSP_MGR_H_ */
