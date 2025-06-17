/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _SYS_CFG_H_
#define _SYS_CFG_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define APP_DATA_SYNC_WORD      0xAA55
#define APP_DATA_SYNC_WORD_LEN  2
#define APP_DATA_TONE_NUM_LEN   2

#define DATA_SYNC_WORD          0xAA55AA55
#define DATA_SYNC_WORD_LEN      4

#define APP_CONFIG_OFFSET          (0)
#define APP_CONFIG_SIZE            1024
#define APP_LED_OFFSET             (APP_CONFIG_OFFSET + APP_CONFIG_SIZE)
#define APP_LED_SIZE               512

#define AUDIO_ROUTE_CONFIG_OFFSET  (APP_LED_OFFSET + APP_LED_SIZE)
#define AUDIO_ROUTE_CONFIG_SIZE    512

#define TONE_DATA_OFFSET           (4096)
#define TONE_DATA_SIZE             3072
#define VOICE_PROMPT_OFFSET        (TONE_DATA_OFFSET + TONE_DATA_SIZE)
#define VOICE_PROMPT_SIZE          137 * 1024

/** @brief  Read only configurations for inbox audio application. */
typedef struct
{
    //Header: 4 bytes
    uint32_t sync_word;

    uint16_t audio_secondary_ref_out_spk_enable: 1;
    uint16_t audio_secondary_ref_out_spk_endpoint_idx: 3;
    uint16_t audio_secondary_ref_out_spk_endpoint_type: 2;
    uint16_t audio_secondary_ref_out_spk_dac_ch: 3;
    uint16_t audio_secondary_ref_out_spk_gateway_idx: 3;
    uint16_t audio_secondary_ref_out_spk_rsvd: 4;

    uint16_t voice_secondary_ref_out_spk_enable: 1;
    uint16_t voice_secondary_ref_out_spk_endpoint_idx: 3;
    uint16_t voice_secondary_ref_out_spk_endpoint_type: 2;
    uint16_t voice_secondary_ref_out_spk_dac_ch: 3;
    uint16_t voice_secondary_ref_out_spk_gateway_idx: 3;
    uint16_t voice_secondary_ref_out_spk_rsvd: 4;

    uint16_t line_secondary_ref_out_spk_enable: 1;
    uint16_t line_secondary_ref_out_spk_endpoint_idx: 3;
    uint16_t line_secondary_ref_out_spk_endpoint_type: 2;
    uint16_t line_secondary_ref_out_spk_dac_ch: 3;
    uint16_t line_secondary_ref_out_spk_gateway_idx: 3;
    uint16_t line_secondary_ref_out_spk_rsvd: 4;

    uint16_t ringtone_secondary_ref_out_spk_enable: 1;
    uint16_t ringtone_secondary_ref_out_spk_endpoint_idx: 3;
    uint16_t ringtone_secondary_ref_out_spk_endpoint_type: 2;
    uint16_t ringtone_secondary_ref_out_spk_dac_ch: 3;
    uint16_t ringtone_secondary_ref_out_spk_gateway_idx: 3;
    uint16_t ringtone_secondary_ref_out_spk_rsvd: 4;

    uint16_t vp_secondary_ref_out_spk_enable: 1;
    uint16_t vp_secondary_ref_out_spk_endpoint_idx: 3;
    uint16_t vp_secondary_ref_out_spk_endpoint_type: 2;
    uint16_t vp_secondary_ref_out_spk_dac_ch: 3;
    uint16_t vp_secondary_ref_out_spk_gateway_idx: 3;
    uint16_t vp_secondary_ref_out_spk_rsvd: 4;

    uint16_t apt_secondary_ref_out_spk_enable: 1;
    uint16_t apt_secondary_ref_out_spk_endpoint_idx: 3;
    uint16_t apt_secondary_ref_out_spk_endpoint_type: 2;
    uint16_t apt_secondary_ref_out_spk_dac_ch: 3;
    uint16_t apt_secondary_ref_out_spk_gateway_idx: 3;
    uint16_t apt_secondary_ref_out_spk_rsvd: 4;

    uint16_t llapt_secondary_ref_out_spk_enable: 1;
    uint16_t llapt_secondary_ref_out_spk_endpoint_idx: 3;
    uint16_t llapt_secondary_ref_out_spk_endpoint_type: 2;
    uint16_t llapt_secondary_ref_out_spk_dac_ch: 3;
    uint16_t llapt_secondary_ref_out_spk_gateway_idx: 3;
    uint16_t llapt_secondary_ref_out_spk_rsvd: 4;

    uint16_t anc_secondary_ref_out_spk_enable: 1;
    uint16_t anc_secondary_ref_out_spk_endpoint_idx: 3;
    uint16_t anc_secondary_ref_out_spk_endpoint_type: 2;
    uint16_t anc_secondary_ref_out_spk_dac_ch: 3;
    uint16_t anc_secondary_ref_out_spk_gateway_idx: 3;
    uint16_t anc_secondary_ref_out_spk_rsvd: 4;

    uint16_t voice_secondary_ref_in_mic_enable: 1;
    uint16_t voice_secondary_ref_in_mic_endpoint_idx_l: 3;
    uint16_t voice_secondary_ref_in_mic_endpoint_type: 2;
    uint16_t voice_secondary_ref_in_mic_adc_ch: 3;
    uint16_t voice_secondary_ref_in_mic_gateway_idx: 3;
    uint16_t voice_secondary_ref_in_mic_rsvd: 3;
    uint16_t voice_secondary_ref_in_mic_endpoint_idx_h: 1;

    uint16_t record_secondary_ref_in_mic_enable: 1;
    uint16_t record_secondary_ref_in_mic_endpoint_idx_l: 3;
    uint16_t record_secondary_ref_in_mic_endpoint_type: 2;
    uint16_t record_secondary_ref_in_mic_adc_ch: 3;
    uint16_t record_secondary_ref_in_mic_gateway_idx: 3;
    uint16_t record_secondary_ref_in_mic_rsvd: 3;
    uint16_t record_secondary_ref_in_mic_endpoint_idx_h: 1;

    uint16_t line_secondary_ref_in_mic_enable: 1;
    uint16_t line_secondary_ref_in_mic_endpoint_idx_l: 3;
    uint16_t line_secondary_ref_in_mic_endpoint_type: 2;
    uint16_t line_secondary_ref_in_mic_adc_ch: 3;
    uint16_t line_secondary_ref_in_mic_gateway_idx: 3;
    uint16_t line_secondary_ref_in_mic_rsvd: 3;
    uint16_t line_secondary_ref_in_mic_endpoint_idx_h: 1;

    uint16_t apt_secondary_ref_in_mic_enable: 1;
    uint16_t apt_secondary_ref_in_mic_endpoint_idx_l: 3;
    uint16_t apt_secondary_ref_in_mic_endpoint_type: 2;
    uint16_t apt_secondary_ref_in_mic_adc_ch: 3;
    uint16_t apt_secondary_ref_in_mic_gateway_idx: 3;
    uint16_t apt_secondary_ref_in_mic_rsvd: 3;
    uint16_t apt_secondary_ref_in_mic_endpoint_idx_h: 1;

    uint16_t llapt_secondary_ref_in_mic_enable: 1;
    uint16_t llapt_secondary_ref_in_mic_endpoint_idx_l: 3;
    uint16_t llapt_secondary_ref_in_mic_endpoint_type: 2;
    uint16_t llapt_secondary_ref_in_mic_adc_ch: 3;
    uint16_t llapt_secondary_ref_in_mic_gateway_idx: 3;
    uint16_t llapt_secondary_ref_in_mic_rsvd: 3;
    uint16_t llapt_secondary_ref_in_mic_endpoint_idx_h: 1;

    uint16_t anc_secondary_ref_in_mic_enable: 1;
    uint16_t anc_secondary_ref_in_mic_endpoint_idx_l: 3;
    uint16_t anc_secondary_ref_in_mic_endpoint_type: 2;
    uint16_t anc_secondary_ref_in_mic_adc_ch: 3;
    uint16_t anc_secondary_ref_in_mic_gateway_idx: 3;
    uint16_t anc_secondary_ref_in_mic_rsvd: 3;
    uint16_t anc_secondary_ref_in_mic_endpoint_idx_h: 1;

    /*DAC_0 && DAC_1 mix point attribute*/
    uint8_t dac_0_mix_point;
    uint8_t dac_1_mix_point;
    uint8_t dac_2_mix_point;
    uint8_t dac_3_mix_point;

    //Audio HAL option1
    uint8_t dmic1_clk_pinmux;
    uint8_t dmic1_dat_pinmux;
    uint8_t dmic2_clk_pinmux;
    uint8_t dmic2_dat_pinmux;
    uint8_t dmic3_clk_pinmux;
    uint8_t dmic3_dat_pinmux;
    uint8_t micbias_pinmux;
    uint8_t micbias_gpio_pinmux;
    uint8_t spdif0_tx_pinmux;
    uint8_t sport0_tx_bridge   : 1;
    uint8_t sport0_rx_bridge   : 1;
    uint8_t sport1_tx_bridge   : 1;
    uint8_t sport1_rx_bridge   : 1;
    uint8_t sport2_tx_bridge   : 1;
    uint8_t sport2_rx_bridge   : 1;
    uint8_t sport3_tx_bridge   : 1;
    uint8_t sport3_rx_bridge   : 1;
    uint8_t spdif0_rx_pinmux;

    //Audio HAL option3
    uint8_t rsvd_spk_type : 2; //(0: single-end, 1: differential, 2: Capless)
    uint8_t rsvd_aux_in_channel : 1; //(0: Mono, 1: Stereo)
    uint8_t dsp_log_output_select : 2;
    uint8_t mic2_aux_co_pad  : 1;
    uint8_t dre_control_sel: 1;
    uint8_t rsvd_audio_hal_opt3: 1;

    //Audio SPK class type and Power Mode
    uint8_t spk1_class: 1;// 0: class D 1: class AB
    uint8_t spk2_class: 1;// 0: class D 1: class AB
    uint8_t amic1_class: 2;// 0: normal mode 1: low power mode, for AMIC1,2
    uint8_t amic2_class: 2;// 0: normal mode 1: low power mode, for AMIC1,2
    uint8_t amic3_class: 2;//0: normal mode 1: HSNR mode, for AMIC3,4
    uint8_t amic4_class: 2;//0: normal mode 1: HSNR mode, for AMIC3,4
    uint8_t aux_l_class: 2;// 0: normal mode 1: low power mode, for AUX_L,R
    uint8_t aux_r_class: 2;// 0: normal mode 1: low power mode, for AUX_L,R
    uint8_t dac0_filter_type: 1; // 0:linear phase 1:minimum phase
    uint8_t dac1_filter_type: 1; // 0:linear phase 1:minimum phase
    uint8_t dac2_filter_type: 1; // 0:linear phase 1:minimum phase
    uint8_t rsvd4: 7;
    uint8_t rsvd5: 4;
    uint8_t dsp_uart_enable : 2;
    uint8_t dsp2_log_output_select : 2;

    //Audio HAL option5
    uint32_t dmic1_clock: 3;
    uint32_t dmic2_clock: 3;
    uint32_t dmic3_clock: 3;
    uint32_t rsvd_amb_primary_mic_sel: 3;
    uint32_t rsvd_amb_primary_mic_type: 2;
    uint32_t rsvd_amb_secondary_mic_sel: 3;
    uint32_t rsvd_amb_secondary_mic_type: 2;
    uint32_t micbias_voltage_sel: 3;
    uint32_t micbias_ldo_ctr: 1;
    uint32_t avcc_drv_voltage_sel: 2;
    uint32_t rsvd_audio_hal_opt5: 7;

    //need to remove
    uint8_t voice_prompt_index_num;   // Maximum voice prompt number for a single language
    uint8_t apt_secondary_ref_in_mic_gateway_ch;
    uint8_t llapt_secondary_ref_in_mic_gateway_ch;
    uint8_t anc_secondary_ref_in_mic_gateway_ch;

    uint8_t audio_primary_spk_polarity: 1;
    uint8_t audio_secondary_spk_polarity: 1;
    uint8_t audio_primary_ref_spk_polarity: 1;
    uint8_t audio_secondary_ref_spk_polarity: 1;
    uint8_t voice_primary_spk_polarity: 1;
    uint8_t voice_secondary_spk_polarity: 1;
    uint8_t voice_primary_ref_spk_polarity: 1;
    uint8_t voice_secondary_ref_spk_polarity: 1;
    uint8_t voice_primary_ref_mic_polarity: 1;
    uint8_t voice_secondary_ref_mic_polarity: 1;
    uint8_t voice_primary_mic_polarity: 1;
    uint8_t voice_secondary_mic_polarity: 1;
    uint8_t voice_fusion_mic_polarity: 1;
    uint8_t voice_bone_mic_polarity: 1;
    uint8_t record_primary_ref_mic_polarity: 1;
    uint8_t record_secondary_ref_mic_polarity: 1;
    uint8_t record_primary_mic_polarity: 1;
    uint8_t record_secondary_mic_polarity: 1;
    uint8_t record_fusion_mic_polarity: 1;
    uint8_t record_bone_mic_polarity: 1;
    uint8_t record_aux_left_polarity: 1;
    uint8_t record_aux_right_polarity: 1;
    uint8_t line_primary_spk_polarity: 1;
    uint8_t line_secondary_spk_polarity: 1;
    uint8_t line_primary_ref_spk_polarity: 1;
    uint8_t line_secondary_ref_spk_polarity: 1;
    uint8_t line_primary_ref_mic_polarity: 1;
    uint8_t line_secondary_ref_mic_polarity: 1;
    uint8_t line_aux_left_polarity: 1;
    uint8_t line_aux_right_polarity: 1;
    uint8_t ringtone_primary_spk_polarity: 1;
    uint8_t ringtone_secondary_spk_polarity: 1;
    uint8_t ringtone_primary_ref_spk_polarity: 1;
    uint8_t ringtone_secondary_ref_spk_polarity: 1;
    uint8_t vp_primary_spk_polarity: 1;
    uint8_t vp_secondary_spk_polarity: 1;
    uint8_t vp_primary_ref_spk_polarity: 1;
    uint8_t vp_secondary_ref_spk_polarity: 1;
    uint8_t apt_primary_spk_polarity: 1;
    uint8_t apt_secondary_spk_polarity: 1;
    uint8_t apt_primary_ref_spk_polarity: 1;
    uint8_t apt_secondary_ref_spk_polarity: 1;
    uint8_t apt_primary_ref_mic_polarity: 1;
    uint8_t apt_secondary_ref_mic_polarity: 1;
    uint8_t apt_front_mic_left_polarity: 1;
    uint8_t apt_front_mic_right_polarity: 1;
    uint8_t apt_ff_mic_left_polarity: 1;
    uint8_t apt_ff_mic_right_polarity: 1;
    uint8_t llapt_primary_spk_polarity: 1;
    uint8_t llapt_secondary_spk_polarity: 1;
    uint8_t llapt_primary_ref_spk_polarity: 1;
    uint8_t llapt_secondary_ref_spk_polarity: 1;
    uint8_t llapt_primary_ref_mic_polarity: 1;
    uint8_t llapt_secondary_ref_mic_polarity: 1;
    uint8_t llapt_mic_left_polarity: 1;
    uint8_t llapt_mic_right_polarity: 1;
    uint8_t anc_primary_spk_polarity: 1;
    uint8_t anc_secondary_spk_polarity: 1;
    uint8_t anc_primary_ref_spk_polarity: 1;
    uint8_t anc_secondary_ref_spk_polarity: 1;
    uint8_t anc_primary_ref_mic_polarity: 1;
    uint8_t anc_secondary_ref_mic_polarity: 1;
    uint8_t anc_ff_mic_left_polarity: 1;
    uint8_t anc_ff_mic_right_polarity: 1;
    uint8_t anc_fb_mic_left_polarity: 1;
    uint8_t anc_fb_mic_right_polarity: 1;
    uint8_t vad_primary_ref_mic_polarity: 1;
    uint8_t vad_secondary_ref_mic_polarity: 1;
    uint8_t vad_primary_mic_polarity: 1;
    uint8_t vad_secondary_mic_polarity: 1;
    uint8_t apt_fb_mic_left_polarity: 1;
    uint8_t apt_fb_mic_right_polarity: 1;
    uint8_t apt_bone_mic_left_polarity: 1;
    uint8_t apt_bone_mic_right_polarity: 1;
    uint8_t line_mic_left_polarity: 1;
    uint8_t line_mic_right_polarity: 1;
    uint8_t polarity_rsvd2: 4;

    // category - audio
    uint16_t audio_primary_out_spk_enable: 1;
    uint16_t audio_primary_out_spk_endpoint_idx: 3;
    uint16_t audio_primary_out_spk_endpoint_type: 2;
    uint16_t audio_primary_out_spk_dac_ch: 3;
    uint16_t audio_primary_out_spk_gateway_idx: 3;
    uint16_t audio_primary_out_spk_rsvd: 4;

    uint16_t audio_secondary_out_spk_enable: 1;
    uint16_t audio_secondary_out_spk_endpoint_idx: 3;
    uint16_t audio_secondary_out_spk_endpoint_type: 2;
    uint16_t audio_secondary_out_spk_dac_ch: 3;
    uint16_t audio_secondary_out_spk_gateway_idx: 3;
    uint16_t audio_secondary_out_spk_rsvd: 4;

    // category - voice
    uint16_t voice_primary_out_spk_enable: 1;
    uint16_t voice_primary_out_spk_endpoint_idx: 3;
    uint16_t voice_primary_out_spk_endpoint_type: 2;
    uint16_t voice_primary_out_spk_dac_ch: 3;
    uint16_t voice_primary_out_spk_gateway_idx: 3;
    uint16_t voice_primary_out_spk_rsvd: 4;

    uint16_t voice_secondary_out_spk_enable: 1;
    uint16_t voice_secondary_out_spk_endpoint_idx: 3;
    uint16_t voice_secondary_out_spk_endpoint_type: 2;
    uint16_t voice_secondary_out_spk_dac_ch: 3;
    uint16_t voice_secondary_out_spk_gateway_idx: 3;
    uint16_t voice_secondary_out_spk_rsvd: 4;

    uint16_t voice_primary_ref_out_spk_enable: 1;
    uint16_t voice_primary_ref_out_spk_endpoint_idx: 3;
    uint16_t voice_primary_ref_out_spk_endpoint_type: 2;
    uint16_t voice_primary_ref_out_spk_dac_ch: 3;
    uint16_t voice_primary_ref_out_spk_gateway_idx: 3;
    uint16_t voice_primary_ref_out_spk_rsvd: 4;

    uint16_t voice_primary_ref_in_mic_enable: 1;
    uint16_t voice_primary_ref_in_mic_endpoint_idx_l: 3;
    uint16_t voice_primary_ref_in_mic_endpoint_type: 2;
    uint16_t voice_primary_ref_in_mic_adc_ch: 3;
    uint16_t voice_primary_ref_in_mic_gateway_idx: 3;
    uint16_t voice_primary_ref_in_mic_rsvd: 3;
    uint16_t voice_primary_ref_in_mic_endpoint_idx_h: 1;

    uint16_t voice_primary_in_mic_enable: 1;
    uint16_t voice_primary_in_mic_endpoint_idx_l: 3;
    uint16_t voice_primary_in_mic_endpoint_type: 2;
    uint16_t voice_primary_in_mic_adc_ch: 3;
    uint16_t voice_primary_in_mic_gateway_idx: 3;
    uint16_t voice_primary_in_mic_rsvd: 3;
    uint16_t voice_primary_in_mic_endpoint_idx_h: 1;

    uint16_t voice_secondary_in_mic_enable: 1;
    uint16_t voice_secondary_in_mic_endpoint_idx_l: 3;
    uint16_t voice_secondary_in_mic_endpoint_type: 2;
    uint16_t voice_secondary_in_mic_adc_ch: 3;
    uint16_t voice_secondary_in_mic_gateway_idx: 3;
    uint16_t voice_secondary_in_mic_rsvd: 3;
    uint16_t voice_secondary_in_mic_endpoint_idx_h: 1;

    uint16_t voice_fusion_in_mic_enable: 1;
    uint16_t voice_fusion_in_mic_endpoint_idx_l: 3;
    uint16_t voice_fusion_in_mic_endpoint_idx_type: 2;
    uint16_t voice_fusion_in_mic_adc_ch: 3;
    uint16_t voice_fusion_in_mic_gateway_idx: 3;
    uint16_t voice_fusion_in_mic_rsvd: 3;
    uint16_t voice_fusion_in_mic_endpoint_idx_h: 1;

    uint16_t voice_bone_in_mic_enable: 1;
    uint16_t voice_bone_in_mic_endpoint_idx_l: 3;
    uint16_t voice_bone_in_mic_endpoint_type: 2;
    uint16_t voice_bone_in_mic_adc_ch: 3;
    uint16_t voice_bone_in_mic_gateway_idx: 3;
    uint16_t voice_bone_in_mic_rsvd: 3;
    uint16_t voice_bone_in_mic_endpoint_idx_h: 1;

    // category - record
    uint16_t record_primary_in_mic_enable: 1;
    uint16_t record_primary_in_mic_endpoint_idx_l: 3;
    uint16_t record_primary_in_mic_endpoint_type: 2;
    uint16_t record_primary_in_mic_adc_ch: 3;
    uint16_t record_primary_in_mic_gateway_idx: 3;
    uint16_t record_primary_in_mic_rsvd: 3;
    uint16_t record_primary_in_mic_endpoint_idx_h: 1;

    uint16_t record_secondary_in_mic_enable: 1;
    uint16_t record_secondary_in_mic_endpoint_idx_l: 3;
    uint16_t record_secondary_in_mic_endpoint_type: 2;
    uint16_t record_secondary_in_mic_adc_ch: 3;
    uint16_t record_secondary_in_mic_gateway_idx: 3;
    uint16_t record_secondary_in_mic_rsvd: 3;
    uint16_t record_secondary_in_mic_endpoint_idx_h: 1;

    uint16_t record_primary_in_aux_enable: 1;
    uint16_t record_primary_in_aux_endpoint_idx_l: 3;
    uint16_t record_primary_in_aux_endpoint_type: 2;
    uint16_t record_primary_in_aux_adc_ch: 3;
    uint16_t record_primary_in_aux_gateway_idx: 3;
    uint16_t record_primary_in_aux_rsvd: 3;
    uint16_t record_primary_in_aux_endpoint_idx_h: 1;

    uint16_t record_secondary_in_aux_enable: 1;
    uint16_t record_secondary_in_aux_endpoint_idx_l: 3;
    uint16_t record_secondary_in_aux_endpoint_type: 2;
    uint16_t record_secondary_in_aux_adc_ch: 3;
    uint16_t record_secondary_in_aux_gateway_idx: 3;
    uint16_t record_secondary_in_aux_rsvd: 3;
    uint16_t record_secondary_in_aux_endpoint_idx_h: 1;

    // category - line
    uint16_t line_primary_out_spk_enable: 1;
    uint16_t line_primary_out_spk_endpoint_idx: 3;
    uint16_t line_primary_out_spk_endpoint_type: 2;
    uint16_t line_primary_out_spk_dac_ch: 3;
    uint16_t line_primary_out_spk_gateway_idx: 3;
    uint16_t line_primary_out_spk_rsvd: 4;

    uint16_t line_secondary_out_spk_enable: 1;
    uint16_t line_secondary_out_spk_endpoint_idx: 3;
    uint16_t line_secondary_out_spk_endpoint_type: 2;
    uint16_t line_secondary_out_spk_dac_ch: 3;
    uint16_t line_secondary_out_spk_gateway_idx: 3;
    uint16_t line_secondary_out_spk_rsvd: 4;

    uint16_t line_primary_ref_out_spk_enable: 1;
    uint16_t line_primary_ref_out_spk_endpoint_idx: 3;
    uint16_t line_primary_ref_out_spk_endpoint_type: 2;
    uint16_t line_primary_ref_out_spk_dac_ch: 3;
    uint16_t line_primary_ref_out_spk_gateway_idx: 3;
    uint16_t line_primary_ref_out_spk_rsvd: 4;

    uint16_t line_primary_ref_in_mic_enable: 1;
    uint16_t line_primary_ref_in_mic_endpoint_idx_l: 3;
    uint16_t line_primary_ref_in_mic_endpoint_type: 2;
    uint16_t line_primary_ref_in_mic_adc_ch: 3;
    uint16_t line_primary_ref_in_mic_gateway_idx: 3;
    uint16_t line_primary_ref_in_mic_rsvd: 3;
    uint16_t line_primary_ref_in_mic_endpoint_idx_h: 1;

    uint16_t line_left_in_aux_enable: 1;
    uint16_t line_left_in_aux_endpoint_idx_l: 3;
    uint16_t line_left_in_aux_endpoint_type: 2;
    uint16_t line_left_in_aux_adc_ch: 3;
    uint16_t line_left_in_aux_gateway_idx: 3;
    uint16_t line_left_in_aux_rsvd: 3;
    uint16_t line_left_in_aux_endpoint_idx_h: 1;

    uint16_t line_right_in_aux_enable: 1;
    uint16_t line_right_in_aux_endpoint_idx_l: 3;
    uint16_t line_right_in_aux_endpoint_type: 2;
    uint16_t line_right_in_aux_adc_ch: 3;
    uint16_t line_right_in_aux_gateway_idx: 3;
    uint16_t line_right_in_aux_rsvd: 3;
    uint16_t line_right_in_aux_endpoint_idx_h: 1;

    // category - ringtone
    uint16_t ringtone_primary_out_spk_enable: 1;
    uint16_t ringtone_primary_out_spk_endpoint_idx: 3;
    uint16_t ringtone_primary_out_spk_endpoint_type: 2;
    uint16_t ringtone_primary_out_spk_dac_ch: 3;
    uint16_t ringtone_primary_out_spk_gateway_idx: 3;
    uint16_t ringtone_primary_out_spk_rsvd: 4;

    uint16_t ringtone_secondary_out_spk_enable: 1;
    uint16_t ringtone_secondary_out_spk_endpoint_idx: 3;
    uint16_t ringtone_secondary_out_spk_endpoint_type: 2;
    uint16_t ringtone_secondary_out_spk_dac_ch: 3;
    uint16_t ringtone_secondary_out_spk_gateway_idx: 3;
    uint16_t ringtone_secondary_out_spk_rsvd: 4;

    // category - voice prompt
    uint16_t vp_primary_out_spk_enable: 1;
    uint16_t vp_primary_out_spk_endpoint_idx: 3;
    uint16_t vp_primary_out_spk_endpoint_type: 2;
    uint16_t vp_primary_out_spk_dac_ch: 3;
    uint16_t vp_primary_out_spk_gateway_idx: 3;
    uint16_t vp_primary_out_spk_rsvd: 4;

    uint16_t vp_secondary_out_spk_enable: 1;
    uint16_t vp_secondary_out_spk_endpoint_idx: 3;
    uint16_t vp_secondary_out_spk_endpoint_type: 2;
    uint16_t vp_secondary_out_spk_dac_ch: 3;
    uint16_t vp_secondary_out_spk_gateway_idx: 3;
    uint16_t vp_secondary_out_spk_rsvd: 4;

    // category - apt
    uint16_t apt_primary_out_spk_enable: 1;
    uint16_t apt_primary_out_spk_endpoint_idx: 3;
    uint16_t apt_primary_out_spk_endpoint_type: 2;
    uint16_t apt_primary_out_spk_dac_ch: 3;
    uint16_t apt_primary_out_spk_gateway_idx: 3;
    uint16_t apt_primary_out_spk_rsvd: 4;

    uint16_t apt_secondary_out_spk_enable: 1;
    uint16_t apt_secondary_out_spk_endpoint_idx: 3;
    uint16_t apt_secondary_out_spk_endpoint_type: 2;
    uint16_t apt_secondary_out_spk_dac_ch: 3;
    uint16_t apt_secondary_out_spk_gateway_idx: 3;
    uint16_t apt_secondary_out_spk_rsvd: 4;

    uint16_t apt_primary_ref_out_spk_enable: 1;
    uint16_t apt_primary_ref_out_spk_endpoint_idx: 3;
    uint16_t apt_primary_ref_out_spk_endpoint_type: 2;
    uint16_t apt_primary_ref_out_spk_dac_ch: 3;
    uint16_t apt_primary_ref_out_spk_gateway_idx: 3;
    uint16_t apt_primary_ref_out_spk_rsvd: 4;

    uint16_t apt_primary_ref_in_mic_enable: 1;
    uint16_t apt_primary_ref_in_mic_endpoint_idx_l: 3;
    uint16_t apt_primary_ref_in_mic_endpoint_type: 2;
    uint16_t apt_primary_ref_in_mic_adc_ch: 3;
    uint16_t apt_primary_ref_in_mic_gateway_idx: 3;
    uint16_t apt_primary_ref_in_mic_rsvd: 3;
    uint16_t apt_primary_ref_in_mic_endpoint_idx_h: 1;

    uint16_t apt_front_left_in_mic_enable: 1;
    uint16_t apt_front_left_in_mic_endpoint_idx_l: 3;
    uint16_t apt_front_left_in_mic_endpoint_type: 2;
    uint16_t apt_front_left_in_mic_adc_ch: 3;
    uint16_t apt_front_left_in_mic_gateway_idx: 3;
    uint16_t apt_front_left_in_mic_rsvd: 3;
    uint16_t apt_front_left_in_mic_endpoint_idx_h: 1;

    uint16_t apt_front_right_in_mic_enable: 1;
    uint16_t apt_front_right_in_mic_endpoint_idx_l: 3;
    uint16_t apt_front_right_in_mic_endpoint_type: 2;
    uint16_t apt_front_right_in_mic_adc_ch: 3;
    uint16_t apt_front_right_in_mic_gateway_idx: 3;
    uint16_t apt_front_right_in_mic_rsvd: 3;
    uint16_t apt_front_right_in_mic_endpoint_idx_h: 1;

    uint16_t apt_ff_left_in_mic_enable: 1;
    uint16_t apt_ff_left_in_mic_endpoint_idx_l: 3;
    uint16_t apt_ff_left_in_mic_endpoint_type: 2;
    uint16_t apt_ff_left_in_mic_adc_ch: 3;
    uint16_t apt_ff_left_in_mic_gateway_idx: 3;
    uint16_t apt_ff_left_in_mic_rsvd: 3;
    uint16_t apt_ff_left_in_mic_endpoint_idx_h: 1;

    uint16_t apt_ff_right_in_mic_enable: 1;
    uint16_t apt_ff_right_in_mic_endpoint_idx_l: 3;
    uint16_t apt_ff_right_in_mic_endpoint_type: 2;
    uint16_t apt_ff_right_in_mic_adc_ch: 3;
    uint16_t apt_ff_right_in_mic_gateway_idx: 3;
    uint16_t apt_ff_right_in_mic_rsvd: 3;
    uint16_t apt_ff_right_in_mic_endpoint_idx_h: 1;

    // category - llapt
    uint16_t llapt_primary_out_spk_enable: 1;
    uint16_t llapt_primary_out_spk_endpoint_idx: 3;
    uint16_t llapt_primary_out_spk_endpoint_type: 2;
    uint16_t llapt_primary_out_spk_dac_ch: 3;
    uint16_t llapt_primary_out_spk_gateway_idx: 3;
    uint16_t llapt_primary_out_spk_rsvd: 4;

    uint16_t llapt_secondary_out_spk_enable: 1;
    uint16_t llapt_secondary_out_spk_endpoint_idx: 3;
    uint16_t llapt_secondary_out_spk_endpoint_type: 2;
    uint16_t llapt_secondary_out_spk_dac_ch: 3;
    uint16_t llapt_secondary_out_spk_gateway_idx: 3;
    uint16_t llapt_secondary_out_spk_rsvd: 4;

    uint16_t llapt_primary_ref_out_spk_enable: 1;
    uint16_t llapt_primary_ref_out_spk_endpoint_idx: 3;
    uint16_t llapt_primary_ref_out_spk_endpoint_type: 2;
    uint16_t llapt_primary_ref_out_spk_dac_ch: 3;
    uint16_t llapt_primary_ref_out_spk_gateway_idx: 3;
    uint16_t llapt_primary_ref_out_spk_rsvd: 4;

    uint16_t llapt_primary_ref_in_mic_enable: 1;
    uint16_t llapt_primary_ref_in_mic_endpoint_idx_l: 3;
    uint16_t llapt_primary_ref_in_mic_endpoint_type: 2;
    uint16_t llapt_primary_ref_in_mic_adc_ch: 3;
    uint16_t llapt_primary_ref_in_mic_gateway_idx: 3;
    uint16_t llapt_primary_ref_in_mic_rsvd: 3;
    uint16_t llapt_primary_ref_in_mic_endpoint_idx_h: 1;

    uint16_t llapt_left_in_mic_enable: 1;
    uint16_t llapt_left_in_mic_endpoint_idx_l: 3;
    uint16_t llapt_left_in_mic_endpoint_type: 2;
    uint16_t llapt_left_in_mic_llapt_ch: 3;
    uint16_t llapt_left_in_mic_gateway_idx: 3;
    uint16_t llapt_left_in_mic_rsvd: 3;
    uint16_t llapt_left_in_mic_endpoint_idx_h: 1;

    uint16_t llapt_right_in_mic_enable: 1;
    uint16_t llapt_right_in_mic_endpoint_idx_l: 3;
    uint16_t llapt_right_in_mic_endpoint_type: 2;
    uint16_t llapt_right_in_mic_llapt_ch: 3;
    uint16_t llapt_right_in_mic_gateway_idx: 3;
    uint16_t llapt_right_in_mic_rsvd: 3;
    uint16_t llapt_right_in_mic_endpoint_idx_h: 1;

    // category - anc
    uint16_t anc_primary_out_spk_enable: 1;
    uint16_t anc_primary_out_spk_endpoint_idx: 3;
    uint16_t anc_primary_out_spk_endpoint_type: 2;
    uint16_t anc_primary_out_spk_dac_ch: 3;
    uint16_t anc_primary_out_spk_gateway_idx: 3;
    uint16_t anc_primary_out_spk_rsvd: 4;

    uint16_t anc_secondary_out_spk_enable: 1;
    uint16_t anc_secondary_out_spk_endpoint_idx: 3;
    uint16_t anc_secondary_out_spk_endpoint_type: 2;
    uint16_t anc_secondary_out_spk_dac_ch: 3;
    uint16_t anc_secondary_out_spk_gateway_idx: 3;
    uint16_t anc_secondary_out_spk_rsvd: 4;

    uint16_t anc_primary_ref_out_spk_enable: 1;
    uint16_t anc_primary_ref_out_spk_endpoint_idx: 3;
    uint16_t anc_primary_ref_out_spk_endpoint_type: 2;
    uint16_t anc_primary_ref_out_spk_dac_ch: 3;
    uint16_t anc_primary_ref_out_spk_gateway_idx: 3;
    uint16_t anc_primary_ref_out_spk_rsvd: 4;

    uint16_t anc_primary_ref_in_mic_enable: 1;
    uint16_t anc_primary_ref_in_mic_endpoint_idx_l: 3;
    uint16_t anc_primary_ref_in_mic_endpoint_type: 2;
    uint16_t anc_primary_ref_in_mic_adc_ch: 3;
    uint16_t anc_primary_ref_in_mic_gateway_idx: 3;
    uint16_t anc_primary_ref_in_mic_rsvd: 3;
    uint16_t anc_primary_ref_in_mic_endpoint_idx_h: 1;

    uint16_t anc_ff_left_in_mic_enable: 1;
    uint16_t anc_ff_left_in_mic_endpoint_idx_l: 3;
    uint16_t anc_ff_left_in_mic_endpoint_type: 2;
    uint16_t anc_ff_left_in_mic_anc_ch: 3;
    uint16_t anc_ff_left_in_mic_gateway_idx: 3;
    uint16_t anc_ff_left_in_mic_rsvd: 3;
    uint16_t anc_ff_left_in_mic_endpoint_idx_h: 1;

    uint16_t anc_ff_right_in_mic_enable: 1;
    uint16_t anc_ff_right_in_mic_endpoint_idx_l: 3;
    uint16_t anc_ff_right_in_mic_endpoint_type: 2;
    uint16_t anc_ff_right_in_mic_anc_ch: 3;
    uint16_t anc_ff_right_in_mic_gateway_idx: 3;
    uint16_t anc_ff_right_in_mic_rsvd: 3;
    uint16_t anc_ff_right_in_mic_endpoint_idx_h: 1;

    uint16_t anc_fb_left_in_mic_enable: 1;
    uint16_t anc_fb_left_in_mic_endpoint_idx_l: 3;
    uint16_t anc_fb_left_in_mic_endpoint_type: 2;
    uint16_t anc_fb_left_in_mic_anc_ch: 3;
    uint16_t anc_fb_left_in_mic_gateway_idx: 3;
    uint16_t anc_fb_left_in_mic_rsvd: 3;
    uint16_t anc_fb_left_in_mic_endpoint_idx_h: 1;

    uint16_t anc_fb_right_in_mic_enable: 1;
    uint16_t anc_fb_right_in_mic_endpoint_idx_l: 3;
    uint16_t anc_fb_right_in_mic_endpoint_type: 2;
    uint16_t anc_fb_right_in_mic_anc_ch: 3;
    uint16_t anc_fb_right_in_mic_gateway_idx: 3;
    uint16_t anc_fb_right_in_mic_rsvd: 3;
    uint16_t anc_fb_right_in_mic_endpoint_idx_h: 1;

    // category - vad
    uint16_t vad_primary_ref_in_mic_enable: 1;
    uint16_t vad_primary_ref_in_mic_endpoint_idx_l: 3;
    uint16_t vad_primary_ref_in_mic_endpoint_type: 2;
    uint16_t vad_primary_ref_in_mic_adc_ch: 3;
    uint16_t vad_primary_ref_in_mic_gateway_idx: 3;
    uint16_t vad_primary_ref_in_mic_rsvd: 3;
    uint16_t vad_primary_ref_in_mic_endpoint_idx_h: 1;

    uint16_t vad_secondary_ref_in_mic_enable: 1;
    uint16_t vad_secondary_ref_in_mic_endpoint_idx_l: 3;
    uint16_t vad_secondary_ref_in_mic_endpoint_type: 2;
    uint16_t vad_secondary_ref_in_mic_adc_ch: 3;
    uint16_t vad_secondary_ref_in_mic_gateway_idx: 3;
    uint16_t vad_secondary_ref_in_mic_rsvd: 3;
    uint16_t vad_secondary_ref_in_mic_endpoint_idx_h: 1;

    uint16_t vad_primary_in_mic_enable: 1;
    uint16_t vad_primary_in_mic_endpoint_idx_l: 3;
    uint16_t vad_primary_in_mic_endpoint_type: 2;
    uint16_t vad_primary_in_mic_adc_ch: 3;
    uint16_t vad_primary_in_mic_gateway_idx: 3;
    uint16_t vad_primary_in_mic_gateway_type: 2;
    uint16_t vad_primary_in_mic_rsvd: 1;
    uint16_t vad_primary_in_mic_endpoint_idx_h: 1;

    uint16_t vad_secondary_in_mic_enable: 1;
    uint16_t vad_secondary_in_mic_endpoint_idx_l: 3;
    uint16_t vad_secondary_in_mic_endpoint_type: 2;
    uint16_t vad_secondary_in_mic_adc_ch: 3;
    uint16_t vad_secondary_in_mic_gateway_idx: 3;
    uint16_t vad_secondary_in_mic_gateway_type: 2;
    uint16_t vad_secondary_in_mic_rsvd: 1;
    uint16_t vad_secondary_in_mic_endpoint_idx_h: 1;

    uint16_t audio_primary_ref_out_spk_enable: 1;
    uint16_t audio_primary_ref_out_spk_endpoint_idx: 3;
    uint16_t audio_primary_ref_out_spk_endpoint_type: 2;
    uint16_t audio_primary_ref_out_spk_dac_ch: 3;
    uint16_t audio_primary_ref_out_spk_gateway_idx: 3;
    uint16_t audio_primary_ref_out_spk_rsvd: 4;

    uint16_t record_primary_ref_in_mic_enable: 1;
    uint16_t record_primary_ref_in_mic_endpoint_idx_l: 3;
    uint16_t record_primary_ref_in_mic_endpoint_type: 2;
    uint16_t record_primary_ref_in_mic_adc_ch: 3;
    uint16_t record_primary_ref_in_mic_gateway_idx: 3;
    uint16_t record_primary_ref_in_mic_rsvd: 3;
    uint16_t record_primary_ref_in_mic_endpoint_idx_h: 1;

    uint16_t ringtone_primary_ref_out_spk_enable: 1;
    uint16_t ringtone_primary_ref_out_spk_endpoint_idx: 3;
    uint16_t ringtone_primary_ref_out_spk_endpoint_type: 2;
    uint16_t ringtone_primary_ref_out_spk_dac_ch: 3;
    uint16_t ringtone_primary_ref_out_spk_gateway_idx: 3;
    uint16_t ringtone_primary_ref_out_spk_rsvd: 4;

    uint16_t vp_primary_ref_out_spk_enable: 1;
    uint16_t vp_primary_ref_out_spk_endpoint_idx: 3;
    uint16_t vp_primary_ref_out_spk_endpoint_type: 2;
    uint16_t vp_primary_ref_out_spk_dac_ch: 3;
    uint16_t vp_primary_ref_out_spk_gateway_idx: 3;
    uint16_t vp_primary_ref_out_spk_rsvd: 4;

    uint8_t record_fusion_in_mic_gateway_ch;
    uint8_t record_bone_in_mic_gateway_ch;

    uint32_t sport0_enable                      : 1;
    uint32_t sport0_role                        : 1;
    uint32_t sport0_rx_fifo_mode                : 1;
    uint32_t sport0_rx_chann_mode               : 2;
    uint32_t sport0_tx_chann_mode               : 2;
    uint32_t sport0_rx_format                   : 2;
    uint32_t sport0_tx_format                   : 2;
    uint32_t sport0_rx_data_len                 : 3;
    uint32_t sport0_tx_data_len                 : 3;
    uint32_t sport0_rx_chann_len                : 3;
    uint32_t sport0_tx_chann_len                : 3;
    uint32_t sport0_rx_sample_rate              : 4;
    uint32_t sport0_tx_sample_rate              : 4;
    uint32_t sport0_tx_fifo_mode                : 1;

    uint32_t sport0_lrc_pinmux                  : 8;
    uint32_t sport0_bclk_pinmux                 : 8;
    uint32_t sport0_adc_dat_pinmux              : 8;
    uint32_t sport0_dac_dat_pinmux              : 8;

    uint32_t sport1_enable                      : 1;
    uint32_t sport1_role                        : 1;
    uint32_t sport1_rx_fifo_mode                : 1;
    uint32_t sport1_rx_chann_mode               : 2;
    uint32_t sport1_tx_chann_mode               : 2;
    uint32_t sport1_rx_format                   : 2;
    uint32_t sport1_tx_format                   : 2;
    uint32_t sport1_rx_data_len                 : 3;
    uint32_t sport1_tx_data_len                 : 3;
    uint32_t sport1_rx_chann_len                : 3;
    uint32_t sport1_tx_chann_len                : 3;
    uint32_t sport1_rx_sample_rate              : 4;
    uint32_t sport1_tx_sample_rate              : 4;
    uint32_t sport1_tx_fifo_mode                : 1;

    uint32_t sport1_lrc_pinmux                  : 8;
    uint32_t sport1_bclk_pinmux                 : 8;
    uint32_t sport1_adc_dat_pinmux              : 8;
    uint32_t sport1_dac_dat_pinmux              : 8;

    uint32_t sport2_enable                      : 1;
    uint32_t sport2_role                        : 1;
    uint32_t sport2_rx_fifo_mode                : 1;
    uint32_t sport2_rx_chann_mode               : 2;
    uint32_t sport2_tx_chann_mode               : 2;
    uint32_t sport2_rx_format                   : 2;
    uint32_t sport2_tx_format                   : 2;
    uint32_t sport2_rx_data_len                 : 3;
    uint32_t sport2_tx_data_len                 : 3;
    uint32_t sport2_rx_chann_len                : 3;
    uint32_t sport2_tx_chann_len                : 3;
    uint32_t sport2_rx_sample_rate              : 4;
    uint32_t sport2_tx_sample_rate              : 4;
    uint32_t sport2_tx_fifo_mode                : 1;

    uint32_t sport2_lrc_pinmux                  : 8;
    uint32_t sport2_bclk_pinmux                 : 8;
    uint32_t sport2_adc_dat_pinmux              : 8;
    uint32_t sport2_dac_dat_pinmux              : 8;

    uint32_t sport3_enable                      : 1;
    uint32_t sport3_role                        : 1;
    uint32_t sport3_rx_fifo_mode                : 1;
    uint32_t sport3_rx_chann_mode               : 2;
    uint32_t sport3_tx_chann_mode               : 2;
    uint32_t sport3_rx_format                   : 2;
    uint32_t sport3_tx_format                   : 2;
    uint32_t sport3_rx_data_len                 : 3;
    uint32_t sport3_tx_data_len                 : 3;
    uint32_t sport3_rx_chann_len                : 3;
    uint32_t sport3_tx_chann_len                : 3;
    uint32_t sport3_rx_sample_rate              : 4;
    uint32_t sport3_tx_sample_rate              : 4;
    uint32_t sport3_tx_fifo_mode                : 1;

    uint32_t sport3_lrc_pinmux                  : 8;
    uint32_t sport3_bclk_pinmux                 : 8;
    uint32_t sport3_adc_dat_pinmux              : 8;
    uint32_t sport3_dac_dat_pinmux              : 8;

    uint32_t sport_ext_mclk_enable              : 1;
    uint32_t sport_ext_mclk_rate                : 4;
    uint32_t sport_ext_mclk_pinmux              : 8;
    uint32_t sport0_bclk_src                    : 1;
    uint32_t sport1_bclk_src                    : 1;
    uint32_t sport2_bclk_src                    : 1;
    uint32_t sport3_bclk_src                    : 1;
    uint32_t vad0_enable                        : 1;
    uint32_t vad0_fifo_mode                     : 1;
    uint32_t vad0_chann_mode                    : 2;
    uint32_t vad0_sample_rate                   : 4;
    uint32_t vad0_data_len                      : 3;
    uint32_t vad0_chann_len                     : 3;
    uint32_t sport_ext_rsvd1                    : 1;

    // category - audio
    uint32_t audio_primary_out_spdif_gateway_ch : 8;
    uint32_t audio_ramp_gain_scaling            : 8;
    uint32_t audio_ramp_gain_duration           : 16;

    uint8_t voice_primary_out_spdif_gateway_ch;
    uint8_t voice_secondary_out_spdif_gateway_ch;
    uint8_t voice_primary_in_spdif_gateway_ch;
    uint8_t voice_secondary_in_spdif_gateway_ch;

    // category - voice
    uint32_t audio_secondary_out_spdif_gateway_ch : 8;
    uint32_t voice_ramp_gain_scaling              : 8;
    uint32_t voice_ramp_gain_duration             : 16;

    uint8_t line_primary_out_spdif_gateway_ch;
    uint8_t line_secondary_out_spdif_gateway_ch;
    uint8_t line_left_in_spdif_gateway_ch;
    uint8_t line_right_in_spdif_gateway_ch;

    // category - line
    uint32_t record_primary_in_spdif_gateway_ch : 8;
    uint32_t line_ramp_gain_scaling             : 8;
    uint32_t line_ramp_gain_duration            : 16;

    uint8_t ringtone_primary_out_spdif_gateway_ch;
    uint8_t ringtone_secondary_out_spdif_gateway_ch;
    uint8_t vp_primary_out_spdif_gateway_ch;
    uint8_t vp_secondary_out_spdif_gateway_ch;

    // category - apt
    uint32_t record_secondary_in_spdif_gateway_ch : 8;
    uint32_t apt_ramp_gain_scaling                : 8;
    uint32_t apt_ramp_gain_duration               : 16;

    uint8_t apt_fb_left_in_mic_gateway_ch;
    uint8_t apt_fb_right_in_mic_gateway_ch;
    uint8_t apt_bone_left_in_mic_gateway_ch;
    uint8_t apt_bone_right_in_mic_gateway_ch;

    // category - audio
    uint8_t audio_primary_spk_analog_gain;
    uint8_t audio_primary_spk_digital_gain;
    uint8_t audio_secondary_spk_analog_gain;
    uint8_t audio_secondary_spk_digital_gain;

    // category - voice
    uint8_t voice_primary_spk_analog_gain;
    uint8_t voice_primary_spk_digital_gain;
    uint8_t voice_secondary_spk_analog_gain;
    uint8_t voice_secondary_spk_digital_gain;
    uint8_t voice_primary_ref_spk_analog_gain;
    uint8_t voice_primary_ref_spk_digital_gain;
    uint8_t voice_primary_ref_mic_analog_gain;
    uint8_t voice_primary_ref_mic_digital_gain;
    uint8_t voice_primary_ref_mic_digital_boost_gain;
    uint8_t voice_primary_mic_analog_gain;
    uint8_t voice_primary_mic_digital_gain;
    uint8_t voice_primary_mic_digital_boost_gain;
    uint8_t voice_secondary_mic_analog_gain;
    uint8_t voice_secondary_mic_digital_gain;
    uint8_t voice_secondary_mic_digital_boost_gain;
    uint8_t voice_fusion_mic_analog_gain;
    uint8_t voice_fusion_mic_digital_gain;
    uint8_t voice_fusion_mic_digital_boost_gain;
    uint8_t voice_bone_mic_analog_gain;
    uint8_t voice_bone_mic_digital_gain;
    uint8_t voice_bone_mic_digital_boost_gain;

    // category - record
    uint8_t record_primary_mic_analog_gain;
    uint8_t record_primary_mic_digital_gain;
    uint8_t record_primary_mic_digital_boost_gain;
    uint8_t record_secondary_mic_analog_gain;
    uint8_t record_secondary_mic_digital_gain;
    uint8_t record_secondary_mic_digital_boost_gain;
    uint8_t record_aux_left_analog_gain;
    uint8_t record_aux_left_digital_gain;
    uint8_t record_aux_left_digital_boost_gain;
    uint8_t record_aux_right_analog_gain;
    uint8_t record_aux_right_digital_gain;
    uint8_t record_aux_right_digital_boost_gain;

    // category - line
    uint8_t line_primary_spk_analog_gain;
    uint8_t line_primary_spk_digital_gain;
    uint8_t line_secondary_spk_analog_gain;
    uint8_t line_secondary_spk_digital_gain;
    uint8_t line_primary_ref_spk_analog_gain;
    uint8_t line_primary_ref_spk_digital_gain;
    uint8_t line_primary_ref_mic_analog_gain;
    uint8_t line_primary_ref_mic_digital_gain;
    uint8_t line_primary_ref_mic_digital_boost_gain;
    uint8_t line_aux_left_analog_gain;
    uint8_t line_aux_left_digital_gain;
    uint8_t line_aux_left_digital_boost_gain;
    uint8_t line_aux_right_analog_gain;
    uint8_t line_aux_right_digital_gain;
    uint8_t line_aux_right_digital_boost_gain;

    // category - ringtone
    uint8_t ringtone_primary_spk_analog_gain;
    uint8_t ringtone_primary_spk_digital_gain;
    uint8_t ringtone_secondary_spk_analog_gain;
    uint8_t ringtone_secondary_spk_digital_gain;

    // category - voice prompt
    uint8_t vp_primary_spk_analog_gain;
    uint8_t vp_primary_spk_digital_gain;
    uint8_t vp_secondary_spk_analog_gain;
    uint8_t vp_secondary_spk_digital_gain;

    // category - apt
    uint8_t apt_primary_spk_analog_gain;
    uint8_t apt_primary_spk_digital_gain;
    uint8_t apt_secondary_spk_analog_gain;
    uint8_t apt_secondary_spk_digital_gain;
    uint8_t apt_primary_ref_spk_analog_gain;
    uint8_t apt_primary_ref_spk_digital_gain;
    uint8_t apt_primary_ref_mic_analog_gain;
    uint8_t apt_primary_ref_mic_digital_gain;
    uint8_t apt_primary_ref_mic_digital_boost_gain;
    uint8_t apt_front_mic_left_analog_gain;
    uint8_t apt_front_mic_left_digital_gain;
    uint8_t apt_front_mic_left_digital_boost_gain;
    uint8_t apt_front_mic_right_analog_gain;
    uint8_t apt_front_mic_right_digital_gain;
    uint8_t apt_front_mic_right_digital_boost_gain;
    uint8_t apt_ff_mic_left_analog_gain;
    uint8_t apt_ff_mic_left_digital_gain;
    uint8_t apt_ff_mic_left_digital_boost_gain;
    uint8_t apt_ff_mic_right_analog_gain;
    uint8_t apt_ff_mic_right_digital_gain;
    uint8_t apt_ff_mic_right_digital_boost_gain;

    // category - llapt
    uint8_t llapt_primary_spk_analog_gain;
    uint8_t llapt_primary_spk_digital_gain;
    uint8_t llapt_secondary_spk_analog_gain;
    uint8_t llapt_secondary_spk_digital_gain;
    uint8_t llapt_primary_ref_spk_analog_gain;
    uint8_t llapt_primary_ref_spk_digital_gain;
    uint8_t llapt_primary_ref_mic_analog_gain;
    uint8_t llapt_primary_ref_mic_digital_gain;
    uint8_t llapt_primary_ref_mic_digital_boost_gain;
    uint8_t llapt_mic_left_analog_gain;
    uint8_t llapt_mic_left_digital_gain;
    uint8_t llapt_mic_left_digital_boost_gain;
    uint8_t llapt_mic_right_analog_gain;
    uint8_t llapt_mic_right_digital_gain;
    uint8_t llapt_mic_right_digital_boost_gain;

    // category - anc
    uint8_t anc_primary_spk_analog_gain;
    uint8_t anc_primary_spk_digital_gain;
    uint8_t anc_secondary_spk_analog_gain;
    uint8_t anc_secondary_spk_digital_gain;
    uint8_t anc_primary_ref_spk_analog_gain;
    uint8_t anc_primary_ref_spk_digital_gain;
    uint8_t anc_primary_ref_mic_analog_gain;
    uint8_t anc_primary_ref_mic_digital_gain;
    uint8_t anc_primary_ref_mic_digital_boost_gain;
    uint8_t anc_ff_mic_left_analog_gain;
    uint8_t anc_ff_mic_left_digital_gain;
    uint8_t anc_ff_mic_left_digital_boost_gain;
    uint8_t anc_ff_mic_right_analog_gain;
    uint8_t anc_ff_mic_right_digital_gain;
    uint8_t anc_ff_mic_right_digital_boost_gain;
    uint8_t anc_fb_mic_left_analog_gain;
    uint8_t anc_fb_mic_left_digital_gain;
    uint8_t anc_fb_mic_left_digital_boost_gain;
    uint8_t anc_fb_mic_right_analog_gain;
    uint8_t anc_fb_mic_right_digital_gain;
    uint8_t anc_fb_mic_right_digital_boost_gain;

    // category - vad
    uint8_t vad_primary_ref_mic_analog_gain;
    uint8_t vad_primary_ref_mic_digital_gain;
    uint8_t vad_primary_ref_mic_digital_boost_gain;
    uint8_t vad_primary_mic_analog_gain;
    uint8_t vad_primary_mic_digital_gain;
    uint8_t vad_primary_mic_digital_boost_gain;
    uint8_t vad_secondary_mic_analog_gain;
    uint8_t vad_secondary_mic_digital_gain;
    uint8_t vad_secondary_mic_digital_boost_gain;

    uint8_t record_fusion_mic_analog_gain;
    uint8_t record_fusion_mic_digital_gain;
    uint8_t record_fusion_mic_digital_boost_gain;
    uint8_t record_bone_mic_analog_gain;
    uint8_t record_bone_mic_digital_gain;
    uint8_t record_bone_mic_digital_boost_gain;

    uint16_t record_fusion_in_mic_enable: 1;
    uint16_t record_fusion_in_mic_endpoint_idx_l: 3;
    uint16_t record_fusion_in_mic_endpoint_idx_type: 2;
    uint16_t record_fusion_in_mic_adc_ch: 3;
    uint16_t record_fusion_in_mic_gateway_idx: 3;
    uint16_t record_fusion_in_mic_rsvd: 3;
    uint16_t record_fusion_in_mic_endpoint_idx_h: 1;

    uint16_t record_bone_in_mic_enable: 1;
    uint16_t record_bone_in_mic_endpoint_idx_l: 3;
    uint16_t record_bone_in_mic_endpoint_type: 2;
    uint16_t record_bone_in_mic_adc_ch: 3;
    uint16_t record_bone_in_mic_gateway_idx: 3;
    uint16_t record_bone_in_mic_rsvd: 3;
    uint16_t record_bone_in_mic_endpoint_idx_h: 1;

    // Support SPDIF cfg info: SHALL refer to DIPC2.0 spec
    uint32_t spdif0_enable                      : 1;
    uint32_t spdif0_role                        : 1;
    uint32_t spdif0_rx_bridge                   : 1;
    uint32_t spdif0_tx_bridge                   : 1;
    uint32_t spdif0_rx_fifo_mode                : 1;
    uint32_t spdif0_tx_fifo_mode                : 1;
    uint32_t spdif0_rx_chann_mode               : 1;
    uint32_t spdif0_tx_chann_mode               : 1;
    uint32_t spdif0_rx_format                   : 2;
    uint32_t spdif0_tx_format                   : 2;
    uint32_t spdif0_rx_data_len                 : 3;
    uint32_t spdif0_tx_data_len                 : 3;
    uint32_t spdif0_rx_chann_len                : 3;
    uint32_t spdif0_tx_chann_len                : 3;
    uint32_t spdif0_rx_sample_rate              : 4;
    uint32_t spdif0_tx_sample_rate              : 4;

    // Support SPDIF logic IO
    // category - audio
    uint16_t audio_primary_out_spdif_enable: 1;
    uint16_t audio_primary_out_spdif_endpoint_idx: 4;
    uint16_t audio_primary_out_spdif_endpoint_type: 2;
    uint16_t audio_primary_out_spdif_gateway_idx: 2;
    uint16_t audio_primary_out_spdif_rsvd0: 3;
    uint16_t audio_primary_out_spdif_polarity: 1;
    uint16_t audio_primary_out_spdif_rsvd1: 3;

    uint16_t audio_secondary_out_spdif_enable: 1;
    uint16_t audio_secondary_out_spdif_endpoint_idx: 4;
    uint16_t audio_secondary_out_spdif_endpoint_type: 2;
    uint16_t audio_secondary_out_spdif_gateway_idx: 2;
    uint16_t audio_secondary_out_spdif_rsvd0: 3;
    uint16_t audio_secondary_out_spdif_polarity: 1;
    uint16_t audio_secondary_out_spdif_rsvd1: 3;

    // category - voice
    uint16_t voice_primary_out_spdif_enable: 1;
    uint16_t voice_primary_out_spdif_endpoint_idx: 4;
    uint16_t voice_primary_out_spdif_endpoint_type: 2;
    uint16_t voice_primary_out_spdif_gateway_idx: 2;
    uint16_t voice_primary_out_spdif_rsvd0: 3;
    uint16_t voice_primary_out_spdif_polarity: 1;
    uint16_t voice_primary_out_spdif_rsvd1: 3;

    uint16_t voice_secondary_out_spdif_enable: 1;
    uint16_t voice_secondary_out_spdif_endpoint_idx: 4;
    uint16_t voice_secondary_out_spdif_endpoint_type: 2;
    uint16_t voice_secondary_out_spdif_gateway_idx: 2;
    uint16_t voice_secondary_out_spdif_rsvd0: 3;
    uint16_t voice_secondary_out_spdif_polarity: 1;
    uint16_t voice_secondary_out_spdif_rsvd1: 3;

    uint16_t voice_primary_in_spdif_enable: 1;
    uint16_t voice_primary_in_spdif_endpoint_idx: 4;
    uint16_t voice_primary_in_spdif_endpoint_type: 2;
    uint16_t voice_primary_in_spdif_gateway_idx: 2;
    uint16_t voice_primary_in_spdif_rsvd0: 3;
    uint16_t voice_primary_in_spdif_polarity: 1;
    uint16_t voice_primary_in_spdif_rsvd1: 3;

    uint16_t voice_secondary_in_spdif_enable: 1;
    uint16_t voice_secondary_in_spdif_endpoint_idx: 4;
    uint16_t voice_secondary_in_spdif_endpoint_type: 2;
    uint16_t voice_secondary_in_spdif_gateway_idx: 2;
    uint16_t voice_secondary_in_spdif_rsvd0: 3;
    uint16_t voice_secondary_in_spdif_polarity: 1;
    uint16_t voice_secondary_in_spdif_rsvd1: 3;

    // category - record
    uint16_t record_primary_in_spdif_enable: 1;
    uint16_t record_primary_in_spdif_endpoint_idx: 4;
    uint16_t record_primary_in_spdif_endpoint_type: 2;
    uint16_t record_primary_in_spdif_gateway_idx: 2;
    uint16_t record_primary_in_spdif_rsvd0: 3;
    uint16_t record_primary_in_spdif_polarity: 1;
    uint16_t record_primary_in_spdif_rsvd1: 3;

    uint16_t record_secondary_in_spdif_enable: 1;
    uint16_t record_secondary_in_spdif_endpoint_idx: 4;
    uint16_t record_secondary_in_spdif_endpoint_type: 2;
    uint16_t record_secondary_in_spdif_gateway_idx: 2;
    uint16_t record_secondary_in_spdif_rsvd0: 3;
    uint16_t record_secondary_in_spdif_polarity: 1;
    uint16_t record_secondary_in_spdif_rsvd1: 3;

    // category - line
    uint16_t line_primary_out_spdif_enable: 1;
    uint16_t line_primary_out_spdif_endpoint_idx: 4;
    uint16_t line_primary_out_spdif_endpoint_type: 2;
    uint16_t line_primary_out_spdif_gateway_idx: 2;
    uint16_t line_primary_out_spdif_rsvd0: 3;
    uint16_t line_primary_out_spdif_polarity: 1;
    uint16_t line_primary_out_spdif_rsvd1: 3;

    uint16_t line_secondary_out_spdif_enable: 1;
    uint16_t line_secondary_out_spdif_endpoint_idx: 4;
    uint16_t line_secondary_out_spdif_endpoint_type: 2;
    uint16_t line_secondary_out_spdif_gateway_idx: 2;
    uint16_t line_secondary_out_spdif_rsvd0: 3;
    uint16_t line_secondary_out_spdif_polarity: 1;
    uint16_t line_secondary_out_spdif_rsvd1: 3;

    uint16_t line_left_in_spdif_enable: 1;
    uint16_t line_left_in_spdif_endpoint_idx: 4;
    uint16_t line_left_in_spdif_endpoint_type: 2;
    uint16_t line_left_in_spdif_gateway_idx: 2;
    uint16_t line_left_in_spdif_rsvd0: 3;
    uint16_t line_left_in_spdif_polarity: 1;
    uint16_t line_left_in_spdif_rsvd1: 3;

    uint16_t line_right_in_spdif_enable: 1;
    uint16_t line_right_in_spdif_endpoint_idx: 4;
    uint16_t line_right_in_spdif_endpoint_type: 2;
    uint16_t line_right_in_spdif_gateway_idx: 2;
    uint16_t line_right_in_spdif_rsvd0: 3;
    uint16_t line_right_in_spdif_polarity: 1;
    uint16_t line_right_in_spdif_rsvd1: 3;

    // category - ringtone
    uint16_t ringtone_primary_out_spdif_enable: 1;
    uint16_t ringtone_primary_out_spdif_endpoint_idx: 4;
    uint16_t ringtone_primary_out_spdif_endpoint_type: 2;
    uint16_t ringtone_primary_out_spdif_gateway_idx: 2;
    uint16_t ringtone_primary_out_spdif_rsvd0: 3;
    uint16_t ringtone_primary_out_spdif_polarity: 1;
    uint16_t ringtone_primary_out_spdif_rsvd1: 3;

    uint16_t ringtone_secondary_out_spdif_enable: 1;
    uint16_t ringtone_secondary_out_spdif_endpoint_idx: 4;
    uint16_t ringtone_secondary_out_spdif_endpoint_type: 2;
    uint16_t ringtone_secondary_out_spdif_gateway_idx: 2;
    uint16_t ringtone_secondary_out_spdif_rsvd0: 3;
    uint16_t ringtone_secondary_out_spdif_polarity: 1;
    uint16_t ringtone_secondary_out_spdif_rsvd1: 3;

    // category - vp
    uint16_t vp_primary_out_spdif_enable: 1;
    uint16_t vp_primary_out_spdif_endpoint_idx: 4;
    uint16_t vp_primary_out_spdif_endpoint_type: 2;
    uint16_t vp_primary_out_spdif_gateway_idx: 2;
    uint16_t vp_primary_out_spdif_rsvd0: 3;
    uint16_t vp_primary_out_spdif_polarity: 1;
    uint16_t vp_primary_out_spdif_rsvd1: 3;

    uint16_t vp_secondary_out_spdif_enable: 1;
    uint16_t vp_secondary_out_spdif_endpoint_idx: 4;
    uint16_t vp_secondary_out_spdif_endpoint_type: 2;
    uint16_t vp_secondary_out_spdif_gateway_idx: 2;
    uint16_t vp_secondary_out_spdif_rsvd0: 3;
    uint16_t vp_secondary_out_spdif_polarity: 1;
    uint16_t vp_secondary_out_spdif_rsvd1: 3;

    uint16_t apt_fb_left_in_mic_enable: 1;
    uint16_t apt_fb_left_in_mic_endpoint_idx_l: 3;
    uint16_t apt_fb_left_in_mic_endpoint_type: 2;
    uint16_t apt_fb_left_in_mic_adc_ch: 3;
    uint16_t apt_fb_left_in_mic_gateway_idx: 3;
    uint16_t apt_fb_left_in_mic_rsvd: 3;
    uint16_t apt_fb_left_in_mic_endpoint_idx_h: 1;

    uint16_t apt_fb_right_in_mic_enable: 1;
    uint16_t apt_fb_right_in_mic_endpoint_idx_l: 3;
    uint16_t apt_fb_right_in_mic_endpoint_type: 2;
    uint16_t apt_fb_right_in_mic_adc_ch: 3;
    uint16_t apt_fb_right_in_mic_gateway_idx: 3;
    uint16_t apt_fb_right_in_mic_rsvd: 3;
    uint16_t apt_fb_right_in_mic_endpoint_idx_h: 1;

    uint16_t apt_bone_left_in_mic_enable: 1;
    uint16_t apt_bone_left_in_mic_endpoint_idx_l: 3;
    uint16_t apt_bone_left_in_mic_endpoint_type: 2;
    uint16_t apt_bone_left_in_mic_adc_ch: 3;
    uint16_t apt_bone_left_in_mic_gateway_idx: 3;
    uint16_t apt_bone_left_in_mic_rsvd: 3;
    uint16_t apt_bone_left_in_mic_endpoint_idx_h: 1;

    uint16_t apt_bone_right_in_mic_enable: 1;
    uint16_t apt_bone_right_in_mic_endpoint_idx_l: 3;
    uint16_t apt_bone_right_in_mic_endpoint_type: 2;
    uint16_t apt_bone_right_in_mic_adc_ch: 3;
    uint16_t apt_bone_right_in_mic_gateway_idx: 3;
    uint16_t apt_bone_right_in_mic_rsvd: 3;
    uint16_t apt_bone_right_in_mic_endpoint_idx_h: 1;

    uint8_t apt_fb_mic_left_analog_gain;
    uint8_t apt_fb_mic_left_digital_gain;
    uint8_t apt_fb_mic_left_digital_boost_gain;
    uint8_t apt_fb_mic_right_analog_gain;
    uint8_t apt_fb_mic_right_digital_gain;
    uint8_t apt_fb_mic_right_digital_boost_gain;

    uint8_t apt_bone_mic_left_analog_gain;
    uint8_t apt_bone_mic_left_digital_gain;
    uint8_t apt_bone_mic_left_digital_boost_gain;
    uint8_t apt_bone_mic_right_analog_gain;
    uint8_t apt_bone_mic_right_digital_gain;
    uint8_t apt_bone_mic_right_digital_boost_gain;

    /*Support multi gateway channels*/
    // category - audio
    uint8_t audio_primary_out_spk_gateway_ch;
    uint8_t audio_secondary_out_spk_gateway_ch;

    // category - voice
    uint8_t voice_primary_out_spk_gateway_ch;
    uint8_t voice_secondary_out_spk_gateway_ch;
    uint8_t voice_primary_ref_out_spk_gateway_ch;
    uint8_t voice_primary_ref_in_mic_gateway_ch;
    uint8_t voice_primary_in_mic_gateway_ch;
    uint8_t voice_secondary_in_mic_gateway_ch;
    uint8_t voice_fusion_in_mic_gateway_ch;
    uint8_t voice_bone_in_mic_gateway_ch;

    // category - record
    uint8_t record_primary_in_mic_gateway_ch;
    uint8_t record_secondary_in_mic_gateway_ch;
    uint8_t record_primary_in_aux_gateway_ch;
    uint8_t record_secondary_in_aux_gateway_ch;

    // category - line
    uint8_t line_primary_out_spk_gateway_ch;
    uint8_t line_secondary_out_spk_gateway_ch;
    uint8_t line_primary_ref_out_spk_gateway_ch;
    uint8_t line_primary_ref_in_mic_gateway_ch;
    uint8_t line_left_in_aux_gateway_ch;
    uint8_t line_right_in_aux_gateway_ch;

    // category - ringtone
    uint8_t ringtone_primary_out_spk_gateway_ch;
    uint8_t ringtone_secondary_out_spk_gateway_ch;

    // category - voice prompt
    uint8_t vp_primary_out_spk_gateway_ch;
    uint8_t vp_secondary_out_spk_gateway_ch;

    // category - apt
    uint8_t apt_primary_out_spk_gateway_ch;
    uint8_t apt_secondary_out_spk_gateway_ch;
    uint8_t apt_primary_ref_out_spk_gateway_ch;
    uint8_t apt_primary_ref_in_mic_gateway_ch;
    uint8_t apt_front_left_in_mic_gateway_ch;
    uint8_t apt_front_right_in_mic_gateway_ch;
    uint8_t apt_ff_left_in_mic_gateway_ch;
    uint8_t apt_ff_right_in_mic_gateway_ch;

    // category - llapt
    uint8_t llapt_primary_out_spk_gateway_ch;
    uint8_t llapt_secondary_out_spk_gateway_ch;
    uint8_t llapt_primary_ref_out_spk_gateway_ch;
    uint8_t llapt_primary_ref_in_mic_gateway_ch;
    uint8_t llapt_left_in_mic_gateway_ch;
    uint8_t llapt_right_in_mic_gateway_ch;

    // category - anc
    uint8_t anc_primary_out_spk_gateway_ch;
    uint8_t anc_secondary_out_spk_gateway_ch;
    uint8_t anc_primary_ref_out_spk_gateway_ch;
    uint8_t anc_primary_ref_in_mic_gateway_ch;
    uint8_t anc_ff_left_in_mic_gateway_ch;
    uint8_t anc_ff_right_in_mic_gateway_ch;
    uint8_t anc_fb_left_in_mic_gateway_ch;
    uint8_t anc_fb_right_in_mic_gateway_ch;

    // category - vad
    uint8_t vad_primary_ref_in_mic_gateway_ch;
    uint8_t vad_secondary_ref_in_mic_gateway_ch;
    uint8_t vad_primary_in_mic_gateway_ch;
    uint8_t vad_secondary_in_mic_gateway_ch;

    uint8_t audio_primary_ref_out_spk_gateway_ch;
    uint8_t record_primary_ref_in_mic_gateway_ch;
    uint8_t ringtone_primary_ref_out_spk_gateway_ch;
    uint8_t vp_primary_ref_out_spk_gateway_ch;
    uint8_t audio_secondary_ref_out_spk_gateway_ch;
    uint8_t voice_secondary_ref_out_spk_gateway_ch;
    uint8_t line_secondary_ref_out_spk_gateway_ch;
    uint8_t ringtone_secondary_ref_out_spk_gateway_ch;
    uint8_t vp_secondary_ref_out_spk_gateway_ch;
    uint8_t apt_secondary_ref_out_spk_gateway_ch;
    uint8_t llapt_secondary_ref_out_spk_gateway_ch;
    uint8_t anc_secondary_ref_out_spk_gateway_ch;
    uint8_t voice_secondary_ref_in_mic_gateway_ch;
    uint8_t record_secondary_ref_in_mic_gateway_ch;
    uint8_t line_secondary_ref_in_mic_gateway_ch;

    uint8_t line_left_in_mic_gateway_ch;

    uint16_t line_left_in_mic_enable: 1;
    uint16_t line_left_in_mic_endpoint_idx_l: 3;
    uint16_t line_left_in_mic_endpoint_type: 2;
    uint16_t line_left_in_mic_adc_ch: 3;
    uint16_t line_left_in_mic_gateway_idx: 3;
    uint16_t line_left_in_mic_rsvd: 3;
    uint16_t line_left_in_mic_endpoint_idx_h: 1;

    uint16_t line_right_in_mic_enable: 1;
    uint16_t line_right_in_mic_endpoint_idx_l: 3;
    uint16_t line_right_in_mic_endpoint_type: 2;
    uint16_t line_right_in_mic_adc_ch: 3;
    uint16_t line_right_in_mic_gateway_idx: 3;
    uint16_t line_right_in_mic_rsvd: 3;
    uint16_t line_right_in_mic_endpoint_idx_h: 1;

    uint8_t line_right_in_mic_gateway_ch;
} T_AUDIO_ROUTE_CFG;

extern T_AUDIO_ROUTE_CFG audio_route_cfg;

void sys_cfg_load(void);

#ifdef __cplusplus
}
#endif

#endif /*_SYS_CFG_H_*/
