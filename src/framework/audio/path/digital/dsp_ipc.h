/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#ifndef _DSP_IPC_H_
#define _DSP_IPC_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/* DIPC Opcode Indicator Field (1 bit) */
#define DIPC_OIF_LEGACY             (0x00 << 15)
#define DIPC_OIF_CURRENT            (0x01 << 15)
#define DIPC_OIF_GET(opcode)        ((opcode) & 0x8000)

/* DIPC Opcode Domain Field (3-bit mask) */
#define DIPC_ODF_UNSPECIFIED        (0x00 << 12)
#define DIPC_ODF_ENTITY_1           (0x01 << 12)
#define DIPC_ODF_ENTITY_2           (0x02 << 12)
#define DIPC_ODF_ENTITY_3           (0x04 << 12)
#define DIPC_ODF_GET(opcode)        ((opcode) & 0x7000)

/* DIPC Opcode Group Field (4 bits) */
#define DIPC_OGF_INFORMATION        (0x00 << 8)
#define DIPC_OGF_CONFIGURATION      (0x01 << 8)
#define DIPC_OGF_TESTING            (0x02 << 8)
#define DIPC_OGF_SYNCHRONIZATION    (0x03 << 8)
#define DIPC_OGF_CODEC_PIPE         (0x04 << 8)
#define DIPC_OGF_CODER              (0x05 << 8)
#define DIPC_OGF_NOTIFICATION       (0x06 << 8)
#define DIPC_OGF_SIGNAL_PROCESS     (0x07 << 8)
#define DIPC_OGF_AUDIO_EFFECT       (0x08 << 8)
#define DIPC_OGF_VENDOR_SPECIFIC    (0x0F << 8)
#define DIPC_OGF_GET(opcode)        ((opcode) & 0x0F00)

/* DIPC Opcode Command Field (8 bits) */
#define DIPC_OCF_GET(opcode)        ((opcode) & 0x00FF)

/* DIPC H2D Information Command Codes */
#define DIPC_H2D_CONTROLLER_VERSION_READ        (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_INFORMATION | 0x00)
#define DIPC_H2D_CONTROLLER_CAPABILITIES_READ   (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_INFORMATION | 0x01)
#define DIPC_H2D_HOST_VERSION_REPLY             (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_INFORMATION | 0x02)

/* DIPC H2D Configuration Command Codes */
#define DIPC_H2D_GATE_SET                       (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CONFIGURATION | 0x00)
#define DIPC_H2D_GATE_CLEAR                     (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CONFIGURATION | 0x01)
#define DIPC_H2D_MCPS_REPLY                     (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CONFIGURATION | 0x04)
#define DIPC_H2D_POWER_CONTROL                  (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CONFIGURATION | 0x07)
#define DIPC_H2D_CONFIGURATION_LOAD             (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CONFIGURATION | 0x0A)
#define DIPC_H2D_CHANNEL_REORDER                (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CONFIGURATION | 0x10)

/* DIPC H2D Testing Command Codes */
/* TBD: DIPC Spec r03 */

/* DIPC H2D Synchronization Command Codes */
#define DIPC_H2D_TIMESTAMP_SET                  (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SYNCHRONIZATION | 0x00)
#define DIPC_H2D_SESSION_COUPLE                 (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SYNCHRONIZATION | 0x01)
#define DIPC_H2D_SESSION_DECOUPLE               (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SYNCHRONIZATION | 0x02)
#define DIPC_H2D_SESSION_ROLE_SET               (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SYNCHRONIZATION | 0x03)
#define DIPC_H2D_SESSION_JOIN                   (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SYNCHRONIZATION | 0x04)
#define DIPC_H2D_INFORMATION_RELAY_REPLY        (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SYNCHRONIZATION | 0x05)
#define DIPC_H2D_INFORMATION_RELAY_APPLY        (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SYNCHRONIZATION | 0x06)

/* DIPC H2D Codec Pipe Command Codes */
#define DIPC_H2D_CODEC_PIPE_CREATE              (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODEC_PIPE | 0x00)
#define DIPC_H2D_CODEC_PIPE_DESTROY             (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODEC_PIPE | 0x01)
#define DIPC_H2D_CODEC_PIPE_START               (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODEC_PIPE | 0x02)
#define DIPC_H2D_CODEC_PIPE_STOP                (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODEC_PIPE | 0x03)
#define DIPC_H2D_CODEC_PIPE_GAIN_SET            (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODEC_PIPE | 0x04)
#define DIPC_H2D_CODEC_PIPE_ASRC_SET            (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODEC_PIPE | 0x05)
#define DIPC_H2D_CODEC_PIPE_PRE_MIXER_ADD       (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODEC_PIPE | 0x10)
#define DIPC_H2D_CODEC_PIPE_POST_MIXER_ADD      (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODEC_PIPE | 0x11)
#define DIPC_H2D_CODEC_PIPE_MIXER_REMOVE        (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODEC_PIPE | 0x12)
#define DIPC_H2D_CODEC_PIPE_PRE_TEE_SPLIT       (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODEC_PIPE | 0x20)
#define DIPC_H2D_CODEC_PIPE_POST_TEE_SPLIT      (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODEC_PIPE | 0x21)
#define DIPC_H2D_CODEC_PIPE_TEE_MERGE           (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODEC_PIPE | 0x22)

/* DIPC H2D Coder (Decoder and Encoder) Command Codes */
#define DIPC_H2D_DECODER_CREATE                   (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODER | 0x00)
#define DIPC_H2D_DECODER_DESTROY                  (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODER | 0x01)
#define DIPC_H2D_DECODER_START                    (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODER | 0x02)
#define DIPC_H2D_DECODER_STOP                     (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODER | 0x03)
#define DIPC_H2D_DECODER_GAIN_SET                 (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODER | 0x04)
#define DIPC_H2D_DECODER_GAIN_RAMP                (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODER | 0x05)
#define DIPC_H2D_DECODER_ASRC_SET                 (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODER | 0x06)
#define DIPC_H2D_DECODER_PLC_SET                  (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODER | 0x09)
#define DIPC_H2D_DECODER_FIFO_REPORT              (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODER | 0x0A)
#define DIPC_H2D_ENCODER_CREATE                   (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODER | 0x80)
#define DIPC_H2D_ENCODER_DESTROY                  (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODER | 0x81)
#define DIPC_H2D_ENCODER_START                    (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODER | 0x82)
#define DIPC_H2D_ENCODER_STOP                     (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODER | 0x83)
#define DIPC_H2D_ENCODER_GAIN_SET                 (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_CODER | 0x84)

/* DIPC H2D Notification Command Codes */
#define DIPC_H2D_RINGTONE_CREATE                (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_NOTIFICATION | 0x00)
#define DIPC_H2D_RINGTONE_DESTROY               (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_NOTIFICATION | 0x01)
#define DIPC_H2D_RINGTONE_START                 (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_NOTIFICATION | 0x02)
#define DIPC_H2D_RINGTONE_STOP                  (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_NOTIFICATION | 0x03)
#define DIPC_H2D_RINGTONE_GAIN_SET              (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_NOTIFICATION | 0x04)
#define DIPC_H2D_VOICE_PROMPT_CREATE            (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_NOTIFICATION | 0x80)
#define DIPC_H2D_VOICE_PROMPT_DESTROY           (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_NOTIFICATION | 0x81)
#define DIPC_H2D_VOICE_PROMPT_START             (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_NOTIFICATION | 0x82)
#define DIPC_H2D_VOICE_PROMPT_STOP              (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_NOTIFICATION | 0x83)
#define DIPC_H2D_VOICE_PROMPT_GAIN_SET          (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_NOTIFICATION | 0x84)

/* DIPC H2D Signal Process Command Codes */
#define DIPC_H2D_LINE_CREATE                    (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x00)
#define DIPC_H2D_LINE_DESTROY                   (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x01)
#define DIPC_H2D_LINE_START                     (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x02)
#define DIPC_H2D_LINE_STOP                      (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x03)
#define DIPC_H2D_LINE_DAC_GAIN_SET              (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x04)
#define DIPC_H2D_LINE_ADC_GAIN_SET              (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x05)
#define DIPC_H2D_LINE_GAIN_RAMP                 (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x06)
#define DIPC_H2D_ANC_CREATE                     (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x20)
#define DIPC_H2D_ANC_DESTROY                    (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x21)
#define DIPC_H2D_ANC_START                      (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x22)
#define DIPC_H2D_ANC_STOP                       (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x23)
#define DIPC_H2D_ANC_DAC_GAIN_SET               (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x24)
#define DIPC_H2D_ANC_ADC_GAIN_SET               (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x25)
#define DIPC_H2D_ANC_GAIN_RAMP                  (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x26)
#define DIPC_H2D_VAD_CREATE                     (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x40)
#define DIPC_H2D_VAD_DESTROY                    (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x41)
#define DIPC_H2D_VAD_START                      (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x42)
#define DIPC_H2D_VAD_STOP                       (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x43)
#define DIPC_H2D_VAD_FILTER                     (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x44)
#define DIPC_H2D_KWS_CREATE                     (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x50)
#define DIPC_H2D_KWS_DESTROY                    (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x51)
#define DIPC_H2D_KWS_START                      (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x52)
#define DIPC_H2D_KWS_STOP                       (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x53)
#define DIPC_H2D_SIGNAL_MONITORING_START        (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x60)
#define DIPC_H2D_SIGNAL_MONITORING_STOP         (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_SIGNAL_PROCESS | 0x61)

/* DIPC H2D Audio Effect Command Codes */
#define DIPC_H2D_KWS_SET                        (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_AUDIO_EFFECT | 0x00)
#define DIPC_H2D_KWS_CLEAR                      (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_AUDIO_EFFECT | 0x01)
#define DIPC_H2D_EQ_SET                         (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_AUDIO_EFFECT | 0x08)
#define DIPC_H2D_EQ_CLEAR                       (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_AUDIO_EFFECT | 0x09)
#define DIPC_H2D_EFFECT_CONTROL                 (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_AUDIO_EFFECT | 0x0A)
#define DIPC_H2D_SIDETONE_SET                   (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_AUDIO_EFFECT | 0x10)
#define DIPC_H2D_SIDETONE_CLEAR                 (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_AUDIO_EFFECT | 0x11)
#define DIPC_H2D_NREC_SET                       (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_AUDIO_EFFECT | 0x20)
#define DIPC_H2D_NREC_CLEAR                     (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_AUDIO_EFFECT | 0x21)
#define DIPC_H2D_WDRC_SET                       (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_AUDIO_EFFECT | 0x40)
#define DIPC_H2D_WDRC_CLEAR                     (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_AUDIO_EFFECT | 0x41)
#define DIPC_H2D_ANC_OVP_SET                    (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_AUDIO_EFFECT | 0x50)
#define DIPC_H2D_ANC_OVP_CLEAR                  (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_AUDIO_EFFECT | 0x51)
#define DIPC_H2D_BEAMFORMING_SET                (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_AUDIO_EFFECT | 0x60)
#define DIPC_H2D_BEAMFORMING_CLEAR              (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_AUDIO_EFFECT | 0x61)

/* DIPC H2D Vendor Specific Command Codes */
#define DIPC_H2D_VENDOR_SPECIFIC_EFFECT_REQUEST   (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_VENDOR_SPECIFIC | 0x00)
#define DIPC_H2D_VENDOR_SPECIFIC_EFFECT_CONFIRM   (DIPC_OIF_CURRENT | DIPC_ODF_UNSPECIFIED | DIPC_OGF_VENDOR_SPECIFIC | 0x01)

/* DIPC Event Indicator Field (1 bit) */
#define DIPC_EIF_LEGACY             (0x00 << 15)
#define DIPC_EIF_CURRENT            (0x01 << 15)
#define DIPC_EIF_GET(opcode)        ((opcode) & 0x8000)

/* DIPC Event Domain Field (3-bit mask) */
#define DIPC_EDF_UNSPECIFIED        (0x00 << 12)
#define DIPC_EDF_ENTITY_1           (0x01 << 12)
#define DIPC_EDF_ENTITY_2           (0x02 << 12)
#define DIPC_EDF_ENTITY_3           (0x04 << 12)
#define DIPC_EDF_GET(opcode)        ((opcode) & 0x7000)

/* DIPC Event Group Field (4 bits) */
#define DIPC_EGF_INFORMATION        (0x00 << 8)
#define DIPC_EGF_CONFIGURATION      (0x01 << 8)
#define DIPC_EGF_DATA_PROCESS       (0x02 << 8)
#define DIPC_EGF_SYNCHRONIZATION    (0x03 << 8)
#define DIPC_EGF_CODEC_PIPE         (0x04 << 8)
#define DIPC_EGF_CODER              (0x05 << 8)
#define DIPC_EGF_NOTIFICATION       (0x06 << 8)
#define DIPC_EGF_SIGNAL_PROCESS     (0x07 << 8)
#define DIPC_EGF_AUDIO_EFFECT       (0x08 << 8)
#define DIPC_EGF_STATUS_REPORT      (0x09 << 8)
#define DIPC_EGF_VENDOR_SPECIFIC    (0x0F << 8)
#define DIPC_EGF_GET(opcode)        ((opcode) & 0x0F00)

/* DIPC Event Code Command Field (8 bits) */
#define DIPC_ECF_GET(opcode)        ((opcode) & 0x00FF)

/* DIPC D2H Information Event Codes */
#define DIPC_D2H_HOST_VERSION_REQUEST                    (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_INFORMATION | 0x02)

/* DIPC D2H Configuration Event Codes */
#define DIPC_D2H_MCPS_REQUEST                            (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_CONFIGURATION | 0x02)

/* DIPC D2H Data Process Event Codes */
#define DIPC_D2H_DATA_ACK                                (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_DATA_PROCESS | 0x00)
#define DIPC_D2H_DATA_COMPLETE                           (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_DATA_PROCESS | 0x01)
#define DIPC_D2H_DATA_INDICATE                           (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_DATA_PROCESS | 0x02)
#define DIPC_D2H_DATA_EMPTY_NOTIFY                       (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_DATA_PROCESS | 0x03)
#define DIPC_D2H_PLC_NOTIFY                              (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_DATA_PROCESS | 0x04)
#define DIPC_D2H_SIGNAL_REFRESH                          (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_DATA_PROCESS | 0x05)
#define DIPC_D2H_FIFO_NOTIFY                             (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_DATA_PROCESS | 0x06)
#define DIPC_D2H_KEYWORD_HIT                             (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_DATA_PROCESS | 0x07)

/* DIPC D2H Synchronization Event Codes */
#define DIPC_D2H_TIMESTAMP_LOSE                          (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_SYNCHRONIZATION | 0x02)
#define DIPC_D2H_SYNCHRONIZATION_LATCH                   (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_SYNCHRONIZATION | 0x03)
#define DIPC_D2H_SYNCHRONIZATION_UNLATCH                 (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_SYNCHRONIZATION | 0x04)
#define DIPC_D2H_JOIN_INFO_REPORT                        (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_SYNCHRONIZATION | 0x05)
#define DIPC_D2H_INFO_RELAY_REQUEST                      (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_SYNCHRONIZATION | 0x06)

/* DIPC D2H Status Report Event Codes */
#define DIPC_D2H_COMMAND_ACK                             (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_STATUS_REPORT | 0x00)
#define DIPC_D2H_COMMAND_COMPLETE                        (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_STATUS_REPORT | 0x01)
#define DIPC_D2H_ERROR_REPORT                            (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_STATUS_REPORT | 0x02)

/* DIPC D2H Vendor Specific Event Codes */
#define DIPC_D2H_VENDOR_SPECIFIC_COMMAND_ACK             (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_VENDOR_SPECIFIC | 0x00)
#define DIPC_D2H_VENDOR_SPECIFIC_COMMAND_COMPLETE        (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_VENDOR_SPECIFIC | 0x01)
#define DIPC_D2H_VENDOR_SPECIFIC_EFFECT_RESPONSE         (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_VENDOR_SPECIFIC | 0x02)
#define DIPC_D2H_VENDOR_SPECIFIC_EFFECT_INDICATE         (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_VENDOR_SPECIFIC | 0x03)
#define DIPC_D2H_VENDOR_SPECIFIC_EFFECT_NOTIFY           (DIPC_EIF_CURRENT | DIPC_EDF_UNSPECIFIED | DIPC_EGF_VENDOR_SPECIFIC | 0x04)

/* DIP3 D2H synchronization Command Codes*/
#define DIPC3_D2H_INFORMATION_EXCHANGE_COMMAND_BOOT_DONE  (DIPC_EIF_CURRENT | DIPC_EDF_ENTITY_3 | DIPC_EGF_INFORMATION | 0x00)
#define DIPC3_D2H_INFORMATION_EXCHANGE_COMMAND_ACK        (DIPC_EIF_CURRENT | DIPC_EDF_ENTITY_3 | DIPC_EGF_INFORMATION | 0x01)
#define DIPC3_D2H_INFORMATION_EXCHANGE_COMMAND_LOG        (DIPC_EIF_CURRENT | DIPC_EDF_ENTITY_3 | DIPC_EGF_INFORMATION | 0x02)

/* DIPC Event Error Codes (8 bits) */
#define DIPC_ERROR_SUCCESS                  0x00
#define DIPC_ERROR_HARDWARE_FAILURE         0x01
#define DIPC_ERROR_UNKNOWN_COMMAND          0x02
#define DIPC_ERROR_UNKNOWN_SESSION_ID       0x03
#define DIPC_ERROR_INVALID_COMMAND_PARAM    0x04
#define DIPC_ERROR_COMMAND_DISALLOWED       0x05
#define DIPC_ERROR_MEM_CAPACITY_EXCEEDED    0x06
#define DIPC_ERROR_UNSUPPORTED_FEATURE      0x07
#define DIPC_ERROR_CONTROLLER_BUSY          0x08
#define DIPC_ERROR_INVALID_OPERATION_MODE   0x09
#define DIPC_ERROR_UNSPECIFIED_REASON       0xFF

/* DIPC Audio Category (8 bits) */
#define DIPC_AUDIO_CATEGORY_AUDIO           0x00
#define DIPC_AUDIO_CATEGORY_VOICE           0x01
#define DIPC_AUDIO_CATEGORY_RECORD          0x02
#define DIPC_AUDIO_CATEGORY_LINE            0x03
#define DIPC_AUDIO_CATEGORY_RINGTONE        0x04
#define DIPC_AUDIO_CATEGORY_VOICE_PROMPT    0x05
#define DIPC_AUDIO_CATEGORY_APT             0x06
#define DIPC_AUDIO_CATEGORY_LLAPT           0x07
#define DIPC_AUDIO_CATEGORY_ANC             0x08
#define DIPC_AUDIO_CATEGORY_VAD             0x09
#define DIPC_AUDIO_CATEGORY_KWS             0x0A

/* DIPC Coder ID (8 bits) */
#define DIPC_CODER_ID_PCM                   0x00
#define DIPC_CODER_ID_CVSD                  0x01
#define DIPC_CODER_ID_MSBC                  0x02
#define DIPC_CODER_ID_SBC                   0x03
#define DIPC_CODER_ID_AAC                   0x04
#define DIPC_CODER_ID_OPUS                  0x05
#define DIPC_CODER_ID_FLAC                  0x06
#define DIPC_CODER_ID_MP3                   0x07
#define DIPC_CODER_ID_LC3                   0x08
#define DIPC_CODER_ID_LDAC                  0x09
#define DIPC_CODER_ID_LHDC                  0x0A
#define DIPC_CODER_ID_G729                  0x0B
#define DIPC_CODER_ID_LC3PLUS               0x0C

/* DIPC Audio Route Logical IO (8 bits) */
#define DIPC_LOGICAL_IO_AUDIO_PRIMARY_OUT           0x00
#define DIPC_LOGICAL_IO_AUDIO_SECONDARY_OUT         0x01
#define DIPC_LOGICAL_IO_AUDIO_PRIMARY_REF_OUT       0x02
#define DIPC_LOGICAL_IO_AUDIO_SECONDARY_REF_OUT     0x03

#define DIPC_LOGICAL_IO_VOICE_PRIMARY_OUT           0x10
#define DIPC_LOGICAL_IO_VOICE_SECONDARY_OUT         0x11
#define DIPC_LOGICAL_IO_VOICE_PRIMARY_REF_OUT       0x12
#define DIPC_LOGICAL_IO_VOICE_SECONDARY_REF_OUT     0x13
#define DIPC_LOGICAL_IO_VOICE_PRIMARY_REF_IN        0x14
#define DIPC_LOGICAL_IO_VOICE_SECONDARY_REF_IN      0x15
#define DIPC_LOGICAL_IO_VOICE_PRIMARY_IN            0x16
#define DIPC_LOGICAL_IO_VOICE_SECONDARY_IN          0x17
#define DIPC_LOGICAL_IO_VOICE_FUSION_IN             0x18
#define DIPC_LOGICAL_IO_VOICE_BONE_IN               0x19

#define DIPC_LOGICAL_IO_RECORD_PRIMARY_REF_IN           0x20
#define DIPC_LOGICAL_IO_RECORD_SECONDARY_REF_IN         0x21
#define DIPC_LOGICAL_IO_RECORD_PRIMARY_IN               0x22
#define DIPC_LOGICAL_IO_RECORD_SECONDARY_IN             0x23
#define DIPC_LOGICAL_IO_RECORD_FUSION_IN                0x24
#define DIPC_LOGICAL_IO_RECORD_BONE_IN                  0x25

#define DIPC_LOGICAL_IO_LINE_PRIMARY_OUT                0x30
#define DIPC_LOGICAL_IO_LINE_SECONDARY_OUT              0x31
#define DIPC_LOGICAL_IO_LINE_PRIMARY_REF_OUT            0x32
#define DIPC_LOGICAL_IO_LINE_SECONDARY_REF_OUT          0x33
#define DIPC_LOGICAL_IO_LINE_PRIMARY_REF_IN             0x34
#define DIPC_LOGICAL_IO_LINE_SECONDARY_REF_IN           0x35
#define DIPC_LOGICAL_IO_LINE_LEFT_IN                    0x36
#define DIPC_LOGICAL_IO_LINE_RIGHT_IN                   0x37

#define DIPC_LOGICAL_IO_RINGTONE_PRIMARY_OUT            0x40
#define DIPC_LOGICAL_IO_RINGTONE_SECONDARY_OUT          0x41
#define DIPC_LOGICAL_IO_RINGTONE_PRIMARY_REF_OUT        0x42
#define DIPC_LOGICAL_IO_RINGTONE_SECONDARY_REF_OUT      0x43

#define DIPC_LOGICAL_IO_VOICE_PROMPT_PRIMARY_OUT        0x50
#define DIPC_LOGICAL_IO_VOICE_PROMPT_SECONDARY_OUT      0x51
#define DIPC_LOGICAL_IO_VOICE_PROMPT_PRIMARY_REF_OUT    0x52
#define DIPC_LOGICAL_IO_VOICE_PROMPT_SECONDARY_REF_OUT  0x53

#define DIPC_LOGICAL_IO_APT_PRIMARY_OUT                 0x60
#define DIPC_LOGICAL_IO_APT_SECONDARY_OUT               0x61
#define DIPC_LOGICAL_IO_APT_PRIMARY_REF_OUT             0x62
#define DIPC_LOGICAL_IO_APT_SECONDARY_REF_OUT           0x63
#define DIPC_LOGICAL_IO_APT_PRIMARY_REF_IN              0x64
#define DIPC_LOGICAL_IO_APT_SECONDARY_REF_IN            0x65
#define DIPC_LOGICAL_IO_APT_PRIMARY_LEFT_IN             0x66
#define DIPC_LOGICAL_IO_APT_PRIMARY_RIGHT_IN            0x67
#define DIPC_LOGICAL_IO_APT_SECONDARY_LEFT_IN           0x68
#define DIPC_LOGICAL_IO_APT_SECONDARY_RIGHT_IN          0x69

#define DIPC_LOGICAL_IO_LLAPT_PRIMARY_OUT               0x70
#define DIPC_LOGICAL_IO_LLAPT_SECONDARY_OUT             0x71
#define DIPC_LOGICAL_IO_LLAPT_PRIMARY_REF_OUT           0x72
#define DIPC_LOGICAL_IO_LLAPT_SECONDARY_REF_OUT         0x73
#define DIPC_LOGICAL_IO_LLAPT_PRIMARY_REF_IN            0x74
#define DIPC_LOGICAL_IO_LLAPT_SECONDARY_REF_IN          0x75
#define DIPC_LOGICAL_IO_LLAPT_LEFT_IN                   0x76
#define DIPC_LOGICAL_IO_LLAPT_RIGHT_IN                  0x77

#define DIPC_LOGICAL_IO_ANC_PRIMARY_OUT                 0x80
#define DIPC_LOGICAL_IO_ANC_SECONDARY_OUT               0x81
#define DIPC_LOGICAL_IO_ANC_PRIMARY_REF_OUT             0x82
#define DIPC_LOGICAL_IO_ANC_SECONDARY_REF_OUT           0x83
#define DIPC_LOGICAL_IO_ANC_PRIMARY_REF_IN              0x84
#define DIPC_LOGICAL_IO_ANC_SECONDARY_REF_IN            0x85
#define DIPC_LOGICAL_IO_ANC_FF_LEFT_IN                  0x86
#define DIPC_LOGICAL_IO_ANC_FF_RIGHT_IN                 0x87
#define DIPC_LOGICAL_IO_ANC_FB_LEFT_IN                  0x88
#define DIPC_LOGICAL_IO_ANC_FB_RIGHT_IN                 0x89

#define DIPC_LOGICAL_IO_VAD_PRIMARY_REF_IN              0x90
#define DIPC_LOGICAL_IO_VAD_SECONDARY_REF_IN            0x91
#define DIPC_LOGICAL_IO_VAD_PRIMARY_IN                  0x92
#define DIPC_LOGICAL_IO_VAD_SECONDARY_IN                0x93

/* DIPC Header Sync Word (32 bits) */
#define DIPC_SYNC_WORD              0x3F3F3F3F

/* DIPC Header Tail (32 bits) */
#define DIPC_TAIL                   0xFFFFFFFF

/* DIPC Entity ID */
#define DIPC_ENTITY_UNSPECIFIED     (0x0)
#define DIPC_ENTITY_1               (0x1)
#define DIPC_ENTITY_2               (0x2)
#define DIPC_ENTITY_3               (0x3)

/* DIPC Session ID Type (2 bits) */
#define DIPC_SESSION_TYPE_DECODER           0x00
#define DIPC_SESSION_TYPE_ENCODER           0x01
#define DIPC_SESSION_TYPE_CODEC_PIPE        0x02
#define DIPC_SESSION_TYPE_SIGNAL_PROCESS    0x03

/* DIPC Data Status (8 bits) */
#define DIPC_DATA_STATUS_CORRECT            0x00
#define DIPC_DATA_STATUS_ERROR              0x01
#define DIPC_DATA_STATUS_LOST               0x02
#define DIPC_DATA_STATUS_DUMMY              0x03

/* DIPC Data Mode (8 bits) */
#define DIPC_DATA_MODE_NORMAL       0x00
#define DIPC_DATA_MODE_DIRECT       0x01

/* DIPC GATEWAY TYPE */
#define DIPC_GATEWAY_TYPE_I2S       (0x0)
#define DIPC_GATEWAY_TYPE_SPDIF     (0x1)
#define DIPC_GATEWAY_TYPE_VAD       (0x2)

/* DIPC GATEWAY DIRECTION */
#define DIPC_DIRECTION_TX           (0x0)
#define DIPC_DIRECTION_RX           (0x1)
#define DIPC_DIRECTION_MAX          (0x2)

/* DIPC GATEWAY ID */
#define DIPC_GATE_ID0               (0x0)
#define DIPC_GATE_ID1               (0x1)
#define DIPC_GATE_ID2               (0x2)
#define DIPC_GATE_ID3               (0x3)
#define DIPC_GATEWAY_MAX            (0x4)

/* DIPC GATEWAY Endpoint */
#define DIPC_GATEWAY_ENDPOINT_MIC      (0x0)
#define DIPC_GATEWAY_ENDPOINT_SPK      (0x1)
#define DIPC_GATEWAY_ENDPOINT_AUX      (0x2)
#define DIPC_GATEWAY_ENDPOINT_SPDIF    (0x3)
#define DIPC_GATEWAY_ENDPOINT_USB      (0x4)

/* DIPC FADE Enable */
#define DIPC_FADE_ENABLE            (0x1)
#define DIPC_FADE_DISABLE           (0x0)

/* Power Control Mode */
#define DIPC_POWER_CONTROL_MODE_ON       (0x00)
#define DIPC_POWER_CONTROL_MODE_OFF      (0x01)
#define DIPC_POWER_CONTROL_MODE_SUSPEND  (0x02)
#define DIPC_POWER_CONTROL_MODE_RESUME   (0x03)

typedef struct t_dipc_gateway_status
{
    uint8_t type;
    uint8_t id;
    uint8_t dir;
} T_DIPC_GATEWAY_STATUS;

typedef struct t_dipc_configuration_load
{
    uint8_t  status;
    uint8_t  entity_id;
    uint32_t session_id;
} T_DIPC_CONFIGURATION_LOAD;

typedef struct t_dipc_pcm_coder_format
{
    uint32_t sample_rate;
    uint16_t frame_size;
    uint8_t  chann_num;
    uint8_t  bit_width;
    uint32_t chann_location;
} T_DIPC_PCM_CODER_FORMAT;

typedef struct t_dipc_cvsd_coder_format
{
    uint32_t sample_rate;
    uint16_t frame_size;
    uint8_t  chann_num;
    uint8_t  bit_width;
    uint32_t chann_location;
} T_DIPC_CVSD_CODER_FORMAT;

typedef struct t_dipc_msbc_coder_format
{
    uint32_t sample_rate;
    uint16_t frame_size;
    uint8_t  chann_num;
    uint8_t  bit_width;
    uint8_t  chann_mode;
    uint8_t  block_length;
    uint8_t  subband_num;
    uint8_t  allocation_method;
    uint8_t  bitpool;
    uint8_t  paddings[3];
    uint32_t chann_location;
} T_DIPC_MSBC_CODER_FORMAT;

typedef struct t_dipc_sbc_coder_format
{
    uint32_t sample_rate;
    uint16_t frame_size;
    uint8_t  chann_num;
    uint8_t  bit_width;
    uint8_t  chann_mode;
    uint8_t  block_length;
    uint8_t  subband_num;
    uint8_t  allocation_method;
    uint8_t  bitpool;
    uint8_t  paddings[3];
    uint32_t chann_location;
} T_DIPC_SBC_CODER_FORMAT;

typedef struct t_dipc_aac_coder_format
{
    uint32_t sample_rate;
    uint16_t frame_size;
    uint8_t  chann_num;
    uint8_t  bit_width;
    uint8_t  transport_format;
    uint8_t  object_type;
    uint8_t  vbr;
    uint8_t  paddings;
    uint32_t bitrate;
    uint32_t chann_location;
} T_DIPC_AAC_CODER_FORMAT;

typedef struct t_dipc_opus_coder_format
{
    uint32_t sample_rate;
    uint16_t frame_size;
    uint8_t  chann_num;
    uint8_t  bit_width;
    uint8_t  cbr;
    uint8_t  cvbr;
    uint8_t  mode;
    uint8_t  complexity;
    uint32_t bitrate;
    uint32_t chann_location;
    uint8_t  entropy;
    uint8_t  paddings[3];
} T_DIPC_OPUS_CODER_FORMAT;

typedef struct t_dipc_mp3_coder_format
{
    uint32_t sample_rate;
    uint16_t frame_size;
    uint8_t  chann_num;
    uint8_t  bit_width;
    uint8_t  chann_mode;
    uint8_t  version;
    uint8_t  layer;
    uint8_t  paddings;
    uint32_t bitrate;
    uint32_t chann_location;
} T_DIPC_MP3_CODER_FORMAT;

typedef struct t_dipc_lc3_coder_format
{
    uint32_t sample_rate;
    uint16_t frame_size;
    uint8_t  chann_num;
    uint8_t  bit_width;
    uint16_t frame_length;
    uint16_t paddings;
    uint32_t chann_location;
    uint32_t presentation_delay;
} T_DIPC_LC3_CODER_FORMAT;

typedef struct t_dipc_ldac_coder_format
{
    uint32_t sample_rate;
    uint16_t frame_size;
    uint8_t  chann_num;
    uint8_t  bit_width;
    uint8_t  chann_mode;
    uint8_t  paddings[3];
    uint32_t chann_location;
} T_DIPC_LDAC_CODER_FORMAT;

typedef struct t_dipc_flac_coder_format
{
    uint32_t paddings;
} T_DIPC_FLAC_CODER_FORMAT;

typedef struct t_dipc_lhdc_coder_format
{
    uint32_t sample_rate;
    uint16_t frame_size;
    uint8_t  chann_num;
    uint8_t  bit_width;
    uint32_t chann_location;
} T_DIPC_LHDC_CODER_FORMAT;

typedef struct t_dipc_g729_coder_format
{
    uint32_t paddings;
} T_DIPC_G729_CODER_FORMAT;

typedef struct t_dipc_lc3plus_coder_format
{
    uint32_t sample_rate;
    uint16_t frame_size;
    uint8_t  chann_num;
    uint8_t  bit_width;
    uint16_t frame_length;
    uint8_t  mode;
    uint8_t  paddings;
    uint32_t chann_location;
    uint32_t presentation_delay;
} T_DIPC_LC3PLUS_CODER_FORMAT;

typedef struct t_dipc_data_queue_info
{
    uint8_t  status;
    uint32_t session_id;
    uint32_t src_transport_address;
    uint32_t src_transport_size;
    uint32_t snk_transport_address;
    uint32_t snk_transport_size;
} T_DIPC_DATA_QUEUE_INFO;

typedef struct t_dipc_logical_io_entry
{
    uint8_t  logical_io;
    uint8_t  endpoint;
    uint8_t  gateway_type;
    uint8_t  gateway_id;
    uint8_t  gateway_channels;
    uint8_t  gateway_polarity;
    uint16_t rsvd;
} T_DIPC_LOGICAL_IO_ENTRY;

typedef struct t_dipc_logical_io_tbl_head
{
    uint8_t     version;
    uint8_t     entry_count;
    uint16_t    reserved;
} T_DIPC_LOGICAL_IO_TBL_HEAD;

typedef struct t_dipc_data_ack
{
    uint32_t session_id;
    uint16_t seq_num;
} T_DIPC_DATA_ACK;

typedef struct t_dipc_data_indicate
{
    uint32_t session_id;
    uint16_t seq_num;
    uint16_t data_len;
} T_DIPC_DATA_INDICATE;

typedef struct t_dipc_kws_hit
{
    uint32_t session_id;
    uint8_t  keyword_index;
} T_DIPC_KWS_HIT;

typedef struct t_dipc_signal_refresh
{
    uint32_t  session_id;
    uint8_t   direction;
    uint8_t   freq_num;
    uint16_t *left_gain_table;
    uint16_t *right_gain_table;
} T_DIPC_SIGNAL_REFRESH;

typedef struct t_dipc_plc_notify
{
    uint32_t session_id;
    uint32_t total_sample_num;
    uint32_t plc_sample_num;
} T_DIPC_PLC_NOTIFY;

typedef struct t_dipc_error_report
{
    uint32_t session_id;
    uint8_t  error_code;
} T_DIPC_ERROR_REPORT;

typedef struct t_dipc_vse_response
{
    uint32_t  session_id;
    uint16_t  company_id;
    uint16_t  effect_id;
    uint16_t  seq_num;
    uint16_t  payload_len;
    uint8_t  *payload;
} T_DIPC_VSE_RESPONSE;

typedef struct t_dipc_vse_indicate
{
    uint32_t  session_id;
    uint16_t  company_id;
    uint16_t  effect_id;
    uint16_t  seq_num;
    uint16_t  payload_len;
    uint8_t  *payload;
} T_DIPC_VSE_INDICATE;

typedef struct t_dipc_vse_notify
{
    uint32_t  session_id;
    uint16_t  company_id;
    uint16_t  effect_id;
    uint16_t  seq_num;
    uint16_t  payload_len;
    uint8_t  *payload;
} T_DIPC_VSE_NOTIFY;

typedef struct t_dipc_power_control
{
    uint8_t entity_id;
    uint8_t mode;
} T_DIPC_POWER_CONTROL;

bool dipc_codec_pipe_create(uint32_t  session_id,
                            uint8_t   data_mode,
                            uint8_t   src_coder_id,
                            uint8_t   src_frame_num,
                            uint16_t  src_coder_format_size,
                            uint8_t  *src_coder_format,
                            uint8_t   snk_coder_id,
                            uint8_t   snk_frame_num,
                            uint16_t  snk_coder_format_size,
                            uint8_t  *snk_coder_format);
bool dipc_codec_pipe_destroy(uint32_t session_id);
bool dipc_codec_pipe_start(uint32_t session_id);
bool dipc_codec_pipe_stop(uint32_t session_id);
bool dipc_codec_pipe_gain_set(uint32_t session_id,
                              uint16_t gain_step_left,
                              uint16_t gain_step_right);
bool dipc_codec_pipe_pre_mixer_add(uint32_t prime_session_id,
                                   uint32_t auxiliary_session_id);
bool dipc_codec_pipe_post_mixer_add(uint32_t prime_session_id,
                                    uint32_t auxiliary_session_id);
bool dipc_codec_pipe_mixer_remove(uint32_t prime_session_id,
                                  uint32_t auxiliary_session_id);
bool dipc_codec_pipe_asrc_set(uint32_t session_id,
                              uint8_t  toggle,
                              int32_t  ratio);

//for D2H_DSP_STATUS_IND type
#define D2H_IND_TYPE_KEYWORD                6
#define D2H_IND_TYPE_SEG_SEND               10

//for D2H_IND_TYPE_SEG_SEND
#define D2H_IND_STATUS_SEG_SEND_REQ_DATA    0
#define D2H_IND_STATUS_SEG_SEND_ERROR       1

#define DIPC_CH_MODE_NON_INTERLACED         (0)
#define DIPC_CH_MODE_INTERLACED             (1)

/** @brief  DSP log output path.*/
typedef enum
{
    DSP_OUTPUT_LOG_NONE = 0x0,          //!< no DSP log.
    DSP_OUTPUT_LOG_BY_UART = 0x1,       //!< DSP log by uart directly.
    DSP_OUTPUT_LOG_BY_MCU = 0x2,        //!< DSP log by MCU.
} T_DSP_OUTPUT_LOG;

typedef enum
{
    DSP_IPC_DUMMY_PKT_STOP,
    DSP_IPC_DUMMY_PKT_START,
} T_DUMMY_PKT_CMD;

typedef struct t_dsp_ipc_action_ack
{
    uint32_t h2d_id: 16;
    uint32_t result: 8;
    uint32_t rsvd : 8;
} T_DSP_IPC_ACTION_ACK;

typedef enum
{
    DSP_IPC_ACTION_STOP   = 0x0,
    DSP_IPC_ACTION_START  = 0x1,
} T_DSP_IPC_ACTION;

typedef enum
{
    //D2H mailbox
    MAILBOX_D2H_SHARE_QUEUE_ADDRESS     = 0x01,
    MAILBOX_D2H_DSP_LOG_BUFFER          = 0x02,
    MAILBOX_D2H_DSP_ADCDAC_DATA0        = 0x04,
    MAILBOX_D2H_DSP_ADCDAC_DATA1        = 0X05,

    MAILBOX_D2H_WATCHDOG_TIMEOUT        = 0xFE,
    MAILBOX_D2H_DSP_EXCEPTION           = 0xFF
} T_SHM_MAILBOX_D2H;

typedef enum
{
    ALGORITHM_PROPRIETARY_VOICE,    //00
    ALGORITHM_G711_ALAW,            //01
    ALGORITHM_CVSD,                 //02
    ALGORITHM_MSBC,                 //03
    ALGORITHM_OPUS_VOICE,           //04
    ALGORITHM_LC3_VOICE,            //05
    ALGORITHM_USB_SPEECH,           //06
    ALGORITHM_SBC,                  //07
    ALGORITHM_AAC,                  //08
    ALGORITHM_PROPRIETARY_AUDIO1,   //09
    ALGORITHM_PROPRIETARY_AUDIO2,   //10
    ALGORITHM_MP3,                  //11
    ALGORITHM_USB_AUDIO,            //12
    ALGORITHM_LC3,                  //13
    ALGORITHM_LDAC,                 //14 SONY
    ALGORITHM_UHQ,                  //15 Samsung
    ALGORITHM_LHDC,                 //16 Savitech
    ALGORITHM_OPUS_AUDIO,           //17
    ALGORITHM_FLAC,                 //18
    ALGORITHM_PURE_STREAM,          //19, ALGORITHM_PROPRIETARY_AUDIO4
    ALGORITHM_LINE_IN,              //20, MUST before ALGORITHM_END
    ALGORITHM_END                   //21
} T_ALGORITHM_TYPE;

typedef enum
{
    DSP_FIFO_TX_DIRECTION,
    DSP_FIFO_RX_DIRECTION,
} T_DSP_FIFO_TRX_DIRECTION;

typedef struct
{
    uint16_t    data_len;
    uint8_t     *p_data;
} T_DSP_MAILBOX_DATA;

typedef struct
{
    uint32_t version        : 4;
    uint32_t action         : 2;
    uint32_t fade_action    : 2;
    uint32_t vad_hw         : 1;
    uint32_t vad_kws        : 1;
    uint32_t rsvd           : 22;
} T_ACTION_CONFIG;

typedef struct t_dsp_ipc_logic_io_tlv
{
    uint32_t category    : 4;
    uint32_t logic_io_id : 8;
    uint32_t location    : 4;
    uint32_t gateway_id  : 4;
    uint32_t rtx         : 4;
    uint32_t channel     : 4;
    uint32_t polarity    : 1;
    uint32_t rsvd        : 3;
} T_DSP_IPC_LOGIC_IO_TLV;

typedef struct t_dsp_ipc_logic_io
{
    uint32_t version : 4;
    uint32_t tlv_cnt : 8;
    uint32_t rsvd    : 20;
    T_DSP_IPC_LOGIC_IO_TLV *tlv;
} T_DSP_IPC_LOGIC_IO;

typedef struct t_dipc_fifo_notify
{
    uint32_t session_id;
    uint16_t packet_number;
    uint16_t decoding_latency;
    uint16_t queuing_latency;
} T_DIPC_FIFO_NOTIFY;

/**
  * @brief D2H command IDs
*/
typedef enum
{
    D2H_RESERVED = 0x00, //Command id: 0x00

    D2H_DSP_STATUS_IND = 0x02, //Command id: 0x02
    /*
    bit[7:0] : Indication Type
    0: Ringtone
    1: Fade in
    2: Fade out
    3: Decode empty
    4: Voice prompt

    bit[15:8] : Indication Status
    */

    D2H_SILENCE_DETECT_REPORT = 0x03, //Command id: 0x03
    /*
     * Assume initial state not_silence
     * DSP report silence status when condition is changed
     * bit 0: 1 for Spk path is silence, 0 for Spk path is not silence
     *
     */

    D2H_RWS_SLAVE_SYNC_STATUS = 0x07, //Command id: 0x07
    /*
     *  Word 0: [31:0] Loop_n
        Word 1: [31:0] error of samples (Q4)
        Word 2: [31:0] :
                (-3)RWS  reSync
                (-2)Spk1 timestamp lose
                (-1)I2S FIFO empty
                (0) initial condition
                (1) rws sync. unlock
                (2) rws sync. Lock
                (3) rws RWSV2 Seamless OK
     */
    D2H_DSP_FW_ROM_VERSION_ACK = 0x08, //Command id: 0x08
    /*
     * 2 words for version report
     * Word1: Bit[31:8]: IC part number
     * Word1: Bit[7:0]: FW ROM version
     * Word2: Bit{31:0]: 0
     */

    D2H_DSP_FW_RAM_VERSION_ACK = 0x09, //Command id: 0x09
    /*
     * 2 words for version report
     * Word1: Bit[31:8]: IC part number
     * Word1: Bit[7:0]: FW ROM version
     * Word2: Bit[31:16]: FW RAM version
     */

    D2H_DSP_FW_PATCH_VERSION_ACK = 0x0A, //Command id: 0x0A
    D2H_DSP_FW_SDK_VERSION_ACK = 0x0B, //Command id: 0x0B
    /*
     * 1 word for version report
     * Word0: Bit[31:0]: SDK Version number
     */

    D2H_DSP_SET_DAC_GAIN = 0x11, //Command id:0x11
    /*
     * Byte 0: Ch0 DAC gain
     * Byte 1: Ch1 DAC gain
     * Byte 2: Reserved (Ch2 DAC gain)
     * Byte 3: Reserved (Ch3 DAC gain)
     */

    D2H_DSP_SET_ADC_GAIN = 0x12, //Command id:0x12
    /*
     * Byte 0: Ch0 ADC gain
     * Byte 1: Ch1 ADC gain
     * Byte 2: Reserved (Ch2 ADC gain)
     * Byte 3: Reserved (Ch3 ADC gain)
     * Byte 4: Reserved (Ch4 ADC gain)
     * Byte 5: Reserved (Ch5 ADC gain)
     * Byte 6: Reserved (Ch6 ADC gain)
     * Byte 7: Reserved (Ch7 ADC gain)
     */

    D2H_B2BMSG_INTERACTION_SEND = 0x14, //Command id: 0x14

    D2H_RWS_SEAMLESS_RETURN_INFO = 0x17,  //Command id: 0x17
    /*
     * Word0:Secondary Next I2S Start Btclock (Unit:312.5us)
     * Word1:Secondary Next I2S Start Seq 0~65535. StartSeq = CurrentSeq + Interval
     * Word2:Secondary Current Seq 0~255
     */

    D2H_UPLINK_SYNCREF_REQUEST = 0x31, //request Sync Reference depend on session id
    /*
     *   word0: session id, let mcu to find correspond conn_handle
     */

    D2H_HA_STATUS_INFO = 0xB0,

    D2H_DSP_SDK_GENERAL_CMD = 0xC0,
    /* Word 0: Command payload pointer
    */

    D2H_SPORT_VERIFY_RESULT = 0xD0,

    D2H_SCENARIO_STATE = 0xE0,

    D2H_DSP_DBG = 0xF2,
    BID_SHM_LOOPBACK_TEST_ACK = 0xF3,
    BID_SHM_LOOPBACK_MPC_TEST_ACK = 0xF4,
    D2H_FT_TEST_REPORT = 0xF5,
    D2H_TEST_RESP = 0xF6,
    D2H_OVP_TRAINING_REPORT = 0xF7,
} T_SHM_CMD_D2H;

/**
  * @brief H2D command IDs
*/
typedef enum
{
    H2D_CMD_LOG_OUTPUT_SEL = 0x1B, //Command id: 0x1B
    /*
    Bit[7:0]: DSP Log output selection
            0: DSP Log disabled
            1: reserved
            2: DSP Log is forward to MCU to output

    Bit[15:8]: valid if Bit[7:0] == 0x0
            0: DSP send log to SHM, then use DMA output the log
            1: DSP send log to Uart Register directly

    Bit[23:16]: valid if Bit[15:8] == 0x1
            0: DSP use DMA to output log
            1: DSP write uart Register directly
    */

    H2D_CMD_DSP1_UART_CFG = 0x1C,
    /*Bit[7:0]: DSP1 UART Config
            0: DSP1 UART disabled
            1: DSP1 UART tx pin enable
            2: DSP1 UART rx pin enable
            3: DSP1 UART tx/rx pin enable
    */

    H2D_RWS_INIT = 0x26, //Command id: 0x26
    /*
     * Reset RWS state and set Stamp index
        Word0:
        [3:0]: (0) Native
               (1) Piconet0
               (2) Piconet1
               (3) Piconet2
               (4) Piconet3
        Word1: [31:0] :I2S Sampling Rate idx(0~6)
           (0)8000,  (1)16000, (2)32000, (3)44100,  (4)48000, (5)88200, (6)96000,
     *
     */

    H2D_RWS_SET = 0x29, //Command: 0x29
    /*
     * Set device is Master Speaker or Slave Speaker
        Word0:
        [0]: (1) Master Speaker enable; (0)Disable
        [4]: (1) Slave  Speaker enable; (0)Disable
    */

    H2D_RWS_SEAMLESS = 0x2F, //Command id: 0x2F
    /*   Word0: [31:0] (0) disable Seamless
     *                   (1) enable  Seamless  Master
     *                   (2) enable  Seamless  Slave
     */

    H2D_B2BMSG_INTERACTION_RECEIVED = 0x3F,  //command id: 0x3F
    H2D_B2BMSG_INTERACTION_TIMEOUT = 0x40,    //command id: 0x40

    H2D_DSP_VOICE_TRIGGER_SET = 0x49, //command id: 0x49
    /*
     * 2 byte parameters
     * bit[0]    : 1: HW VAD enable, 0: disable
     * bit[1]    : 1: stereo TX (Neckband type), 0: mono TX (RWS type)
     * bit[2~7]  : reserved
     * bit[8]    : 1: KWS enable, 0:disable
     * bit[9~15] : reserved
     */

    H2D_DSP_VOICE_TRIGGER_ACTION = 0x4A,       //command id: 0x4A
    /*
     * 1 byte parameters
     * bit[0]    : 1: Voice Trigger continued, 0: Voice Trigger suspended
     * bit[1~7]  : reserved
     */

    H2D_HANDOVER_INFO = 0x4B, //command id: 0x4B
    /*
    * 2 byte parameters
    * bit[0~7]    : 2: role sec, 1:  role pri
    * bit[7~15]   : 1: handover start , 0: handover stop
    */

    H2D_FORCE_DUMMY_PKT = 0x4E,                        //command id: 0x4E
    /*
     * MCU request DSP to start put dummy packet output
     *
     * Byte[0]: 0:Stop   1:Start
     */

    H2D_DSP_SDK_INIT = 0x58,

    H2D_FIFO_REQUEST = 0x81,
    /*
     *   uint16_t[0]
     *     0: FIFO_TYPE_TX_FIFO
     *     1: FIFO_TYPE_RX_FIFO
     *
     *   uint16_t[1]
     *     0: FIFO_FEATURE_LEN_NORMAL
     *     1: FIFO_FEATURE_LEN_LOW
     *
     */

    H2D_FIFO_EMPTY_REPORT_SET = 0x82,
    /*
     *   uint8_t[0]
     *     0: disable empty report
     *     1: enalbe empty report
     *
     */

    H2D_SPPCAPTURE_SET = 0x0F01,
} T_SHM_CMD_H2D;

typedef enum t_dsp_ipc_event
{
    DSP_IPC_EVT_NONE                      = 0x00,
    DSP_IPC_EVT_POWER_CONTROL_ACK         = 0x01,
    DSP_IPC_EVT_DETECT_SILENCE            = 0x09,
    DSP_IPC_EVT_VOL_UP                    = 0x0D,
    DSP_IPC_EVT_VOL_DOWN                  = 0x0E,
    DSP_IPC_EVT_VERSION                   = 0x0F,
    DSP_IPC_EVT_B2BMSG                    = 0x11,
    DSP_IPC_EVT_JOIN_CLK                  = 0x12,
    DSP_IPC_EVT_KEYWORD                   = 0x13,
    DSP_IPC_EVT_SET_DAC_GAIN              = 0x14,
    DSP_IPC_EVT_SET_ADC_GAIN              = 0x15,
    DSP_IPC_EVT_PROBE                     = 0x19,/*reservation of probe class */
    DSP_IPC_EVT_HA_STATUS_INFO            = 0x1D,
    DSP_IPC_EVT_PROBE_SYNC_REF_REQUEST    = 0x1E, /* FIXME https://jira.realtek.com/browse/BB2RD-656 */
    DSP_IPC_EVT_DSP_SYNC_V2_SUCC          = 0x26,
    DSP_IPC_EVT_DSP_UNSYNC                = 0x27,
    DSP_IPC_EVT_DSP_SYNC_UNLOCK           = 0x28,
    DSP_IPC_EVT_DSP_SYNC_LOCK             = 0x29,
    DSP_IPC_EVT_SYNC_EMPTY                = 0x2A,
    DSP_IPC_EVT_SYNC_LOSE_TIMESTAMP       = 0x2B,
    DSP_IPC_EVT_BTCLK_EXPIRED             = 0x2C,
    DSP_IPC_EVT_SEG_SEND_REQ_DATA         = 0x31,
    DSP_IPC_EVT_SEG_SEND_ERROR            = 0x32,
    DSP_IPC_EVT_GATEWAY_CLEAR             = 0x40,
    DSP_IPC_EVT_OVP_TRAINING_INFO         = 0x41,

    DSP_IPC_EVT_CONFIGURATION_LOADED      = 0x0101,
    DSP_IPC_EVT_DATA_ACK                  = 0x0200,
    DSP_IPC_EVT_DATA_COMPLETE             = 0x0201,
    DSP_IPC_EVT_DATA_IND                  = 0x0202,
    DSP_IPC_EVT_DATA_EMPTY                = 0x0203,
    DSP_IPC_EVT_PLC_NOTIFY                = 0x0204,
    DSP_IPC_EVT_SIGNAL_REFRESH            = 0x0205,
    DSP_IPC_EVT_FIFO_NOTIFY               = 0x0206,

    DSP_IPC_EVT_CODEC_PIPE_CREATED        = 0x0400,
    DSP_IPC_EVT_CODEC_PIPE_STARTED        = 0x0401,
    DSP_IPC_EVT_CODEC_PIPE_STOPPED        = 0x0402,
    DSP_IPC_EVT_CODEC_PIPE_PRE_MIXER_ADD  = 0x0405,
    DSP_IPC_EVT_CODEC_PIPE_POST_MIXER_ADD = 0x0406,
    DSP_IPC_EVT_CODEC_PIPE_MIXER_REMOVE   = 0x0407,

    DSP_IPC_EVT_ENCODER_DECODER           = 0x0500,
    DSP_IPC_EVT_DECODER_CREATED           = 0x0501,
    DSP_IPC_EVT_DECODER_STARTED           = 0x0502,
    DSP_IPC_EVT_DECODER_STOPPED           = 0x0503,
    DSP_IPC_EVT_ENCODER_CREATED           = 0x0510,
    DSP_IPC_EVT_ENCODER_STARTED           = 0x0511,
    DSP_IPC_EVT_ENCODER_STOPPED           = 0x0512,
    DSP_IPC_EVT_TONE_CREATED              = 0x0600,
    DSP_IPC_EVT_TONE_STARTED              = 0x0601,
    DSP_IPC_EVT_TONE_STOPPED              = 0x0602,
    DSP_IPC_EVT_VP_CREATED                = 0x0603,
    DSP_IPC_EVT_VP_STARTED                = 0x0604,
    DSP_IPC_EVT_VP_STOPPED                = 0x0605,

    DSP_IPC_EVT_LINE_CREATED              = 0x0700,
    DSP_IPC_EVT_LINE_STARTED              = 0x0701,
    DSP_IPC_EVT_LINE_STOPPED              = 0x0702,
    DSP_IPC_EVT_APT_CREATED               = 0x0703,
    DSP_IPC_EVT_APT_STARTED               = 0x0704,
    DSP_IPC_EVT_APT_STOPPED               = 0x0705,
    DSP_IPC_EVT_VAD_CREATED               = 0x0706,
    DSP_IPC_EVT_VAD_STARTED               = 0x0707,
    DSP_IPC_EVT_VAD_STOPPED               = 0x0708,
    DSP_IPC_EVT_KWS_CREATED               = 0x0710,
    DSP_IPC_EVT_KWS_STARTED               = 0x0711,
    DSP_IPC_EVT_KWS_STOPPED               = 0x0712,
    DSP_IPC_EVT_KWS_HIT                   = 0x0713,

    DSP_IPC_EVT_VENDOR_SPECIFIC           = 0x0800,
    DSP_IPC_EVT_VSE_RESPONSE              = 0x0801,
    DSP_IPC_EVT_VSE_INDICATE              = 0x0802,
    DSP_IPC_EVT_VSE_NOTIFY                = 0x0803,

    DSP_IPC_EVT_ERROR_REPORT              = 0x0900,
} T_DSP_IPC_EVENT;

/**
 * \brief define for \ref H2D_DSP_VOICE_TRIGGER_SET_SIZE
 *
 */
typedef struct t_dsp_voice_triger_set
{
    uint8_t vad_setting;
    uint8_t kws_setting;
} T_DSP_VOICE_TRIGER_SET;

typedef bool (*P_DSP_IPC_CBACK)(T_DSP_IPC_EVENT event,
                                uint32_t        param);

bool dsp_ipc_init(void);
void dsp_ipc_deinit(void);
bool dsp_ipc_cback_register(P_DSP_IPC_CBACK cback);
bool dsp_ipc_cback_unregister(P_DSP_IPC_CBACK cback);
bool dsp_ipc_data_send(void     *addr,
                       uint8_t  *buffer,
                       uint16_t  length,
                       bool      flush);
uint16_t dsp_ipc_data_recv(void     *addr,
                           uint8_t  *buffer,
                           uint16_t  length);
uint16_t dsp_ipc_data_len_peek(void *addr);
uint16_t dsp_ipc_data_flush(void *addr);
bool dsp_ipc_set_fifo_len(T_DSP_FIFO_TRX_DIRECTION direction,
                          uint8_t                  lower_len);
bool dsp_ipc_set_decode_emtpy(bool enable);
bool dsp_ipc_synchronization_data_send(uint8_t  *buf,
                                       uint16_t  len);
bool dsp_ipc_set_b2bmsg_interaction_timeout(uint8_t param);
bool dsp_ipc_set_rws_init(uint8_t  bt_clk_index,
                          uint32_t sample_rate,
                          uint8_t  type);
bool dsp_ipc_set_rws_seamless(uint8_t param);
bool dsp_ipc_set_rws(uint8_t session_role,
                     uint8_t role);
bool dsp_ipc_set_log_output_sel(uint8_t param);
// to do remove start
bool dsp_ipc_set_dsp2_log_output_sel(uint8_t param);
bool dsp_ipc_set_dsp1_uart(uint8_t param);
bool dsp_ipc_set_dsp2_uart(uint8_t param);
// to do remove end
bool dsp_ipc_set_handover_info(uint8_t role,
                               bool    start);
bool dsp_ipc_set_voice_trigger(uint32_t param);
bool dsp_ipc_set_force_dummy_pkt(uint32_t param);

bool dsp_ipc_vad_action(T_ACTION_CONFIG     action,
                        T_DSP_IPC_LOGIC_IO *logic_io);

bool dsp_ipc_init_dsp_sdk(uint8_t scenario,
                          bool    flush);

bool dipc_gateway_set(uint8_t  type,
                      uint8_t  id,
                      uint8_t  direction,
                      uint8_t  role,
                      uint8_t  bridge,
                      uint32_t sample_rate,
                      uint8_t  fifo_mode,
                      uint8_t  channel_mode,
                      uint8_t  channel_format,
                      uint8_t  channel_length,
                      uint8_t  data_length);

bool dipc_gateway_clear(uint8_t type,
                        uint8_t id,
                        uint8_t direction);

bool dipc_mcps_reply(uint32_t serial_num,
                     uint8_t  entity_id,
                     uint16_t mcps);

bool dipc_power_control(uint8_t entity_id, uint8_t mode);

uint8_t *dipc_configuration_alloc(uint16_t len);

void dipc_configuration_free(uint8_t *buf);

bool dipc_configuration_load(uint8_t   entity_id,
                             uint32_t  session_id,
                             uint8_t  *buf,
                             uint16_t  len);

bool dipc_channel_out_reorder(uint32_t  session_id,
                              uint8_t   channel_num,
                              uint32_t  channel_array[]);

bool dipc_channel_in_reorder(uint32_t  session_id,
                             uint8_t   channel_num,
                             uint32_t  channel_array[]);

bool dipc_decoder_create(uint32_t  session_id,
                         uint8_t   category,
                         uint8_t   data_mode,
                         uint8_t   frame_num,
                         uint8_t   coder_id,
                         uint16_t  coder_format_size,
                         uint8_t  *coder_format,
                         uint16_t  io_table_size,
                         uint8_t  *io_table);

bool dipc_decoder_destroy(uint32_t session_id);

bool dipc_decoder_start(uint32_t session_id,
                        uint8_t  fade);

bool dipc_decoder_stop(uint32_t session_id,
                       uint8_t  fade);

bool dipc_decoder_gain_set(uint32_t session_id,
                           uint16_t gain_step_left,
                           uint16_t gain_step_right);

bool dipc_decoder_ramp_gain_set(uint32_t session_id,
                                uint16_t gain_step_left,
                                uint16_t gain_step_right,
                                uint16_t duration);

bool dipc_decoder_asrc_set(uint32_t session_id,
                           int32_t  ratio,
                           uint32_t timestamp);

bool dipc_signal_monitoring_start(uint32_t session_id,
                                  uint8_t  direction,
                                  uint16_t refresh_interval,
                                  uint8_t  freq_num,
                                  uint32_t freq_table[]);

bool dipc_signal_monitoring_stop(uint32_t session_id,
                                 uint8_t  direction);

bool dipc_decoder_plc_set(uint32_t session_id,
                          bool     enable,
                          uint16_t interval,
                          uint32_t threshold);

bool dipc_decoder_fifo_report(uint32_t session_id,
                              uint16_t interval);

bool dipc_decoder_effect_control(uint32_t session_id,
                                 uint8_t  action);

bool dipc_encoder_create(uint32_t  session_id,
                         uint8_t   category,
                         uint8_t   data_mode,
                         uint8_t   frame_num,
                         uint8_t   coder_id,
                         uint16_t  coder_format_size,
                         uint8_t  *coder_format,
                         uint16_t  io_table_size,
                         uint8_t  *io_table);

bool dipc_encoder_destroy(uint32_t session_id);

bool dipc_encoder_start(uint32_t session_id,
                        uint8_t  fade);

bool dipc_encoder_stop(uint32_t session_id,
                       uint8_t  fade);

bool dipc_encoder_gain_set(uint32_t session_id,
                           uint16_t gain_step_left,
                           uint16_t gain_step_right);

bool dipc_encoder_effect_control(uint32_t session_id,
                                 uint8_t  action);

bool dipc_line_create(uint32_t  session_id,
                      uint16_t  io_table_size,
                      uint8_t  *io_table);

bool dipc_line_destroy(uint32_t session_id);

bool dipc_line_start(uint32_t session_id,
                     uint8_t  fade);

bool dipc_line_stop(uint32_t session_id,
                    uint8_t  fade);

bool dipc_line_dac_gain_set(uint32_t session_id,
                            uint16_t gain_step_left,
                            uint16_t gain_step_right);

bool dipc_line_adc_gain_set(uint32_t session_id,
                            uint16_t gain_step_left,
                            uint16_t gain_step_right);

bool dipc_line_gain_ramp(uint32_t session_id,
                         uint16_t gain_step_left,
                         uint16_t gain_step_right,
                         uint16_t duration);

bool dipc_timestamp_set(uint32_t session_id,
                        uint32_t clk_source,
                        uint32_t timestamp);

bool dipc_voice_prompt_create(uint32_t  session_id,
                              uint8_t   coder_id,
                              uint8_t   frame_num,
                              uint16_t  coder_format_size,
                              uint8_t  *coder_format,
                              uint16_t  io_table_size,
                              uint8_t  *io_table);

bool dipc_voice_prompt_start(uint32_t session_id,
                             uint8_t  fade);

bool dipc_voice_prompt_destroy(uint32_t session_id);

bool dipc_voice_prompt_stop(uint32_t session_id,
                            uint8_t  fade);

bool dipc_voice_prompt_gain_set(uint32_t session_id,
                                uint16_t gain_step_left,
                                uint16_t gain_step_right);

bool dipc_ringtone_create(uint32_t  session_id,
                          uint16_t  tbl_size,
                          uint8_t  *tbl,
                          uint16_t  coefficient_vector_size,
                          uint8_t  *coefficient_vector);

bool dipc_ringtone_start(uint32_t session_id,
                         uint8_t  fade);

bool dipc_ringtone_stop(uint32_t session_id,
                        uint8_t  fade);

bool dipc_ringtone_destroy(uint32_t session_id);

bool dipc_ringtone_gain_set(uint32_t session_id,
                            uint16_t gain_step_left,
                            uint16_t gain_step_right);

bool dipc_apt_create(uint32_t  session_id,
                     uint16_t  io_table_size,
                     uint8_t  *io_table);

bool dipc_apt_destroy(uint32_t session_id);

bool dipc_apt_start(uint32_t session_id,
                    uint8_t  fade);

bool dipc_apt_stop(uint32_t session_id,
                   uint8_t  fade);

bool dipc_vad_create(uint32_t  session_id,
                     uint8_t   aggressiveness_level,
                     uint8_t   coder_id,
                     uint16_t  coder_format_size,
                     uint8_t  *coder_format,
                     uint8_t   data_mode,
                     uint8_t   frame_num,
                     uint16_t  io_table_size,
                     uint8_t  *io_table);

bool dipc_vad_destroy(uint32_t session_id);

bool dipc_vad_start(uint32_t session_id);

bool dipc_vad_stop(uint32_t session_id);

bool dipc_vad_filter(uint32_t session_id,
                     uint8_t  level);

bool dipc_kws_create(uint32_t  session_id,
                     uint8_t   coder_id,
                     uint8_t   frame_num,
                     uint16_t  coder_format_size,
                     uint8_t  *coder_format);

bool dipc_kws_destroy(uint32_t session_id);

bool dipc_kws_start(uint32_t session_id);

bool dipc_kws_stop(uint32_t session_id);

bool dipc_kws_set(uint32_t session_id);

bool dipc_kws_clear(uint32_t session_id);

bool dipc_apt_dac_gain_set(uint32_t session_id,
                           uint16_t gain_step_left,
                           uint16_t gain_step_right);

bool dipc_apt_adc_gain_set(uint32_t session_id,
                           uint16_t gain_step_left,
                           uint16_t gain_step_right);

bool dipc_apt_gain_ramp(uint32_t session_id,
                        uint16_t gain_step_left,
                        uint16_t gain_step_right,
                        uint16_t duration);

bool dipc_eq_set(uint32_t  session_id,
                 uint8_t  *info_buf,
                 uint16_t  info_len);

bool dipc_eq_clear(uint32_t session_id);

bool dipc_nrec_set(uint32_t session_id,
                   uint8_t  mode,
                   uint8_t  level);

bool dipc_nrec_clear(uint32_t session_id);

bool dipc_voice_sidetone_set(uint32_t session_id,
                             uint16_t gain_step);

bool dipc_voice_sidetone_clear(uint32_t session_id);

bool dipc_apt_ovp_set(uint32_t session_id,
                      uint8_t  level);

bool dipc_apt_ovp_clear(uint32_t session_id);

bool dipc_apt_beamforming_set(uint32_t session_id,
                              uint8_t  direction);

bool dipc_apt_beamforming_clear(uint32_t session_id);

bool dipc_wdrc_set(uint32_t session_id, uint8_t *info_buf, uint16_t info_len);

bool dipc_wdrc_clear(uint32_t session_id);

bool dipc_vse_req(uint32_t  session_id,
                  uint16_t  company_id,
                  uint16_t  effect_id,
                  uint16_t  seq_num,
                  uint8_t  *info_buf,
                  uint16_t  info_len);

bool dipc_vse_cfm(uint32_t  session_id,
                  uint16_t  company_id,
                  uint16_t  effect_id,
                  uint16_t  seq_num,
                  uint8_t  *info_buf,
                  uint16_t  info_len);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif   /* _DSP_IPC_H_ */
