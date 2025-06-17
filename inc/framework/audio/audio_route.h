/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_ROUTE_H_
#define _AUDIO_ROUTE_H_

#include <stdint.h>
#include <stdbool.h>
#include "audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_ROUTE Audio Route
 *
 * \brief   Customize the Audio Path route configurations.
 */

/**
 * \brief Define the Audio Route Gateway index.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_gateway_idx
{
    AUDIO_ROUTE_GATEWAY_0,
    AUDIO_ROUTE_GATEWAY_1,
    AUDIO_ROUTE_GATEWAY_2,
    AUDIO_ROUTE_GATEWAY_3,
    AUDIO_ROUTE_GATEWAY_NUM,
} T_AUDIO_ROUTE_GATEWAY_IDX;

/**
 * \brief Define the Audio Route Gateway role.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_gateway_role
{
    AUDIO_ROUTE_GATEWAY_ROLE_MASTER,
    AUDIO_ROUTE_GATEWAY_ROLE_SLAVE,
    AUDIO_ROUTE_GATEWAY_ROLE_NUM,
} T_AUDIO_ROUTE_GATEWAY_ROLE;

/**
 * \brief Define the Audio Route Gateway bridge.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_gateway_bridge
{
    AUDIO_ROUTE_GATEWAY_BRIDGE_INTERNAL,
    AUDIO_ROUTE_GATEWAY_BRIDGE_EXTERNAL,
    AUDIO_ROUTE_GATEWAY_BRIDGE_NUM,
} T_AUDIO_ROUTE_GATEWAY_BRIDGE;

/**
 * \brief Define the Audio Route Gateway direction.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_gateway_dir
{
    AUDIO_ROUTE_GATEWAY_DIR_TX,
    AUDIO_ROUTE_GATEWAY_DIR_RX,
} T_AUDIO_ROUTE_GATEWAY_DIR;

/**
 * \brief Define the Audio Route Gateway FIFO mode.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_gateway_fifo_mode
{
    AUDIO_ROUTE_GATEWAY_FIFO_MODE_NON_INTERLACED,
    AUDIO_ROUTE_GATEWAY_FIFO_MODE_INTERLACED,
    AUDIO_ROUTE_GATEWAY_FIFO_MODE_NUM,
} T_AUDIO_ROUTE_GATEWAY_FIFO_MODE;

/**
 * \brief Define the Audio Route Gateway channel mode.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_gateway_chann_mode
{
    AUDIO_ROUTE_GATEWAY_CHANN_MODE_TDM2,
    AUDIO_ROUTE_GATEWAY_CHANN_MODE_TDM4,
    AUDIO_ROUTE_GATEWAY_CHANN_MODE_TDM6,
    AUDIO_ROUTE_GATEWAY_CHANN_MODE_TDM8,
    AUDIO_ROUTE_GATEWAY_CHANN_MODE_NUM,
} T_AUDIO_ROUTE_GATEWAY_CHANN_MODE;

/**
 * \brief Define the Audio Route Gateway format.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_gateway_format
{
    AUDIO_ROUTE_GATEWAY_FORMAT_I2S_STANDARD,
    AUDIO_ROUTE_GATEWAY_FORMAT_I2S_LEFT_JUSTIFIED,
    AUDIO_ROUTE_GATEWAY_FORMAT_I2S_PCM_A,
    AUDIO_ROUTE_GATEWAY_FORMAT_I2S_PCM_B,
    AUDIO_ROUTE_GATEWAY_FORMAT_NUM,
} T_AUDIO_ROUTE_GATEWAY_FORMAT;

/**
 * \brief Define the Audio Route Gateway data length.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_gateway_data_len
{
    AUDIO_ROUTE_GATEWAY_DATA_LEN_8BIT,
    AUDIO_ROUTE_GATEWAY_DATA_LEN_16BIT,
    AUDIO_ROUTE_GATEWAY_DATA_LEN_20BIT,
    AUDIO_ROUTE_GATEWAY_DATA_LEN_24BIT,
    AUDIO_ROUTE_GATEWAY_DATA_LEN_32BIT,
} T_AUDIO_ROUTE_GATEWAY_DATA_LEN;

/**
 * \brief Define the Audio Route Gateway channel length.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_gateway_chann_len
{
    AUDIO_ROUTE_GATEWAY_CHANN_LEN_8BIT,
    AUDIO_ROUTE_GATEWAY_CHANN_LEN_16BIT,
    AUDIO_ROUTE_GATEWAY_CHANN_LEN_20BIT,
    AUDIO_ROUTE_GATEWAY_CHANN_LEN_24BIT,
    AUDIO_ROUTE_GATEWAY_CHANN_LEN_32BIT,
} T_AUDIO_ROUTE_GATEWAY_CHANN_LEN;

/**
 * \brief Define the Audio Route Gateway channel index.
 *
 * \details At least one Gateway channel should be set. When the corresponding bit index of gateway
 *          channel is set, it indicates that the channel with the corresponding index is enabled.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_gateway_chann
{
    AUDIO_ROUTE_GATEWAY_CHANN0    = 0x00,
    AUDIO_ROUTE_GATEWAY_CHANN1    = 0x01,
    AUDIO_ROUTE_GATEWAY_CHANN2    = 0x02,
    AUDIO_ROUTE_GATEWAY_CHANN3    = 0x03,
    AUDIO_ROUTE_GATEWAY_CHANN4    = 0x04,
    AUDIO_ROUTE_GATEWAY_CHANN5    = 0x05,
    AUDIO_ROUTE_GATEWAY_CHANN6    = 0x06,
    AUDIO_ROUTE_GATEWAY_CHANN7    = 0x07,
    AUDIO_ROUTE_GATEWAY_CHANN_NUM = 0x08,
} T_AUDIO_ROUTE_GATEWAY_CHANN;

/**
 * \brief Define the Audio Route Gateway sample rate.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_gateway_sample_rate
{
    AUDIO_ROUTE_GATEWAY_SR_DYNAMIC   = 0x0F,
    AUDIO_ROUTE_GATEWAY_SR_8KHZ      = 0x00,
    AUDIO_ROUTE_GATEWAY_SR_16KHZ     = 0x01,
    AUDIO_ROUTE_GATEWAY_SR_32KHZ     = 0x02,
    AUDIO_ROUTE_GATEWAY_SR_44P1KHZ   = 0x03,
    AUDIO_ROUTE_GATEWAY_SR_48KHZ     = 0x04,
    AUDIO_ROUTE_GATEWAY_SR_88P2KHZ   = 0x05,
    AUDIO_ROUTE_GATEWAY_SR_96KHZ     = 0x06,
    AUDIO_ROUTE_GATEWAY_SR_192KHZ    = 0x07,
    AUDIO_ROUTE_GATEWAY_SR_12KHZ     = 0x08,
    AUDIO_ROUTE_GATEWAY_SR_24KHZ     = 0x09,
    AUDIO_ROUTE_GATEWAY_SR_11P025KHZ = 0x0a,
    AUDIO_ROUTE_GATEWAY_SR_22P05KHZ  = 0x0b,
    AUDIO_ROUTE_GATEWAY_SR_64KHZ     = 0x0c,
    AUDIO_ROUTE_GATEWAY_SR_NUM,
} T_AUDIO_ROUTE_GATEWAY_SAMPLE_RATE;

/**
 * \brief Define the Audio Route MCLK rate.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_mclk_rate
{
    AUDIO_ROUTE_MCLK_RATE_1P024MHZ,
    AUDIO_ROUTE_MCLK_RATE_1P4112MHZ,
    AUDIO_ROUTE_MCLK_RATE_2P048MHZ,
    AUDIO_ROUTE_MCLK_RATE_2P8224MHZ,
    AUDIO_ROUTE_MCLK_RATE_3P072MHZ,
    AUDIO_ROUTE_MCLK_RATE_4P096MHZ,
    AUDIO_ROUTE_MCLK_RATE_5P6448MHZ,
    AUDIO_ROUTE_MCLK_RATE_6P144MHZ,
    AUDIO_ROUTE_MCLK_RATE_8P192MHZ,
    AUDIO_ROUTE_MCLK_RATE_11P2896MHZ,
    AUDIO_ROUTE_MCLK_RATE_12P288MHZ,
    AUDIO_ROUTE_MCLK_RATE_16P384MHZ,
    AUDIO_ROUTE_MCLK_RATE_22P5792MHZ,
    AUDIO_ROUTE_MCLK_RATE_24P576MHZ,
    AUDIO_ROUTE_MCLK_RATE_32P768MHZ,
    AUDIO_ROUTE_MCLK_RATE_NUM,
} T_AUDIO_ROUTE_MCLK_RATE;

/**
 * \brief Define the Audio Route Gateway clock source.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_gateway_clk_src
{
    AUDIO_ROUTE_GATEWAY_CLK_SRC_XTAL,
    AUDIO_ROUTE_GATEWAY_CLK_SRC_PLL,
} T_AUDIO_ROUTE_GATEWAY_CLK_SRC;

/**
 * \brief Define the Audio Route Logical IO type.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_io_type
{
    AUDIO_ROUTE_IO_AUDIO_PRIMARY_OUT              = 0x00,
    AUDIO_ROUTE_IO_AUDIO_SECONDARY_OUT            = 0x01,
    AUDIO_ROUTE_IO_AUDIO_PRIMARY_REF_OUT          = 0x02,
    AUDIO_ROUTE_IO_AUDIO_SECONDARY_REF_OUT        = 0x03,

    AUDIO_ROUTE_IO_VOICE_PRIMARY_OUT              = 0x10,
    AUDIO_ROUTE_IO_VOICE_SECONDARY_OUT            = 0x11,
    AUDIO_ROUTE_IO_VOICE_PRIMARY_REF_OUT          = 0x12,
    AUDIO_ROUTE_IO_VOICE_SECONDARY_REF_OUT        = 0x13,
    AUDIO_ROUTE_IO_VOICE_PRIMARY_REF_IN           = 0x14,
    AUDIO_ROUTE_IO_VOICE_SECONDARY_REF_IN         = 0x15,
    AUDIO_ROUTE_IO_VOICE_PRIMARY_IN               = 0x16,
    AUDIO_ROUTE_IO_VOICE_SECONDARY_IN             = 0x17,
    AUDIO_ROUTE_IO_VOICE_FUSION_IN                = 0x18,
    AUDIO_ROUTE_IO_VOICE_BONE_IN                  = 0x19,

    AUDIO_ROUTE_IO_RECORD_PRIMARY_REF_IN          = 0x20,
    AUDIO_ROUTE_IO_RECORD_SECONDARY_REF_IN        = 0x21,
    AUDIO_ROUTE_IO_RECORD_PRIMARY_IN              = 0x22,
    AUDIO_ROUTE_IO_RECORD_SECONDARY_IN            = 0x23,
    AUDIO_ROUTE_IO_RECORD_FUSION_IN               = 0x24,
    AUDIO_ROUTE_IO_RECORD_BONE_IN                 = 0x25,

    AUDIO_ROUTE_IO_LINE_PRIMARY_OUT               = 0x30,
    AUDIO_ROUTE_IO_LINE_SECONDARY_OUT             = 0x31,
    AUDIO_ROUTE_IO_LINE_PRIMARY_REF_OUT           = 0x32,
    AUDIO_ROUTE_IO_LINE_SECONDARY_REF_OUT         = 0x33,
    AUDIO_ROUTE_IO_LINE_PRIMARY_REF_IN            = 0x34,
    AUDIO_ROUTE_IO_LINE_SECONDARY_REF_IN          = 0x35,
    AUDIO_ROUTE_IO_LINE_LEFT_IN                   = 0x36,
    AUDIO_ROUTE_IO_LINE_RIGHT_IN                  = 0x37,

    AUDIO_ROUTE_IO_RINGTONE_PRIMARY_OUT           = 0x40,
    AUDIO_ROUTE_IO_RINGTONE_SECONDARY_OUT         = 0x41,
    AUDIO_ROUTE_IO_RINGTONE_PRIMARY_REF_OUT       = 0x42,
    AUDIO_ROUTE_IO_RINGTONE_SECONDARY_REF_OUT     = 0x43,

    AUDIO_ROUTE_IO_VOICE_PROMPT_PRIMARY_OUT       = 0x50,
    AUDIO_ROUTE_IO_VOICE_PROMPT_SECONDARY_OUT     = 0x51,
    AUDIO_ROUTE_IO_VOICE_PROMPT_PRIMARY_REF_OUT   = 0x52,
    AUDIO_ROUTE_IO_VOICE_PROMPT_SECONDARY_REF_OUT = 0x53,

    AUDIO_ROUTE_IO_APT_PRIMARY_OUT                = 0x60,
    AUDIO_ROUTE_IO_APT_SECONDARY_OUT              = 0x61,
    AUDIO_ROUTE_IO_APT_PRIMARY_REF_OUT            = 0x62,
    AUDIO_ROUTE_IO_APT_SECONDARY_REF_OUT          = 0x63,
    AUDIO_ROUTE_IO_APT_PRIMARY_REF_IN             = 0x64,
    AUDIO_ROUTE_IO_APT_SECONDARY_REF_IN           = 0x65,
    AUDIO_ROUTE_IO_APT_FRONT_LEFT_IN              = 0x66,
    AUDIO_ROUTE_IO_APT_FRONT_RIGHT_IN             = 0x67,
    AUDIO_ROUTE_IO_APT_FF_LEFT_IN                 = 0x68,
    AUDIO_ROUTE_IO_APT_FF_RIGHT_IN                = 0x69,
    AUDIO_ROUTE_IO_APT_FB_LEFT_IN                 = 0x6A,
    AUDIO_ROUTE_IO_APT_FB_RIGHT_IN                = 0x6B,
    AUDIO_ROUTE_IO_APT_BONE_LEFT_IN               = 0x6C,
    AUDIO_ROUTE_IO_APT_BONE_RIGHT_IN              = 0x6D,


    AUDIO_ROUTE_IO_LLAPT_PRIMARY_OUT              = 0x70,
    AUDIO_ROUTE_IO_LLAPT_SECONDARY_OUT            = 0x71,
    AUDIO_ROUTE_IO_LLAPT_PRIMARY_REF_OUT          = 0x72,
    AUDIO_ROUTE_IO_LLAPT_SECONDARY_REF_OUT        = 0x73,
    AUDIO_ROUTE_IO_LLAPT_PRIMARY_REF_IN           = 0x74,
    AUDIO_ROUTE_IO_LLAPT_SECONDARY_REF_IN         = 0x75,
    AUDIO_ROUTE_IO_LLAPT_LEFT_IN                  = 0x76,
    AUDIO_ROUTE_IO_LLAPT_RIGHT_IN                 = 0x77,

    AUDIO_ROUTE_IO_ANC_PRIMARY_OUT                = 0x80,
    AUDIO_ROUTE_IO_ANC_SECONDARY_OUT              = 0x81,
    AUDIO_ROUTE_IO_ANC_PRIMARY_REF_OUT            = 0x82,
    AUDIO_ROUTE_IO_ANC_SECONDARY_REF_OUT          = 0x83,
    AUDIO_ROUTE_IO_ANC_PRIMARY_REF_IN             = 0x84,
    AUDIO_ROUTE_IO_ANC_SECONDARY_REF_IN           = 0x85,
    AUDIO_ROUTE_IO_ANC_FF_LEFT_IN                 = 0x86,
    AUDIO_ROUTE_IO_ANC_FF_RIGHT_IN                = 0x87,
    AUDIO_ROUTE_IO_ANC_FB_LEFT_IN                 = 0x88,
    AUDIO_ROUTE_IO_ANC_FB_RIGHT_IN                = 0x89,

    AUDIO_ROUTE_IO_VAD_PRIMARY_REF_IN             = 0x90,
    AUDIO_ROUTE_IO_VAD_SECONDARY_REF_IN           = 0x91,
    AUDIO_ROUTE_IO_VAD_PRIMARY_IN                 = 0x92,
    AUDIO_ROUTE_IO_VAD_SECONDARY_IN               = 0x93,
} T_AUDIO_ROUTE_IO_TYPE;

/**
 * \brief Define the Audio Route Endpoint type.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_endpoint_type
{
    AUDIO_ROUTE_ENDPOINT_MIC     = 0x00,
    AUDIO_ROUTE_ENDPOINT_SPK     = 0x01,
    AUDIO_ROUTE_ENDPOINT_AUX     = 0x02,
    AUDIO_ROUTE_ENDPOINT_SPDIF   = 0x03,
} T_AUDIO_ROUTE_ENDPOINT_TYPE;

/**
 * \brief Define the Audio Route ADC channel.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_adc_chann
{
    AUDIO_ROUTE_ADC_CHANN0,
    AUDIO_ROUTE_ADC_CHANN1,
    AUDIO_ROUTE_ADC_CHANN2,
    AUDIO_ROUTE_ADC_CHANN3,
    AUDIO_ROUTE_ADC_CHANN4,
    AUDIO_ROUTE_ADC_CHANN5,
    AUDIO_ROUTE_ADC_CHANN_NUM,
} T_AUDIO_ROUTE_ADC_CHANN;

/**
 * \brief Define the Audio Route DAC channel.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_dac_chann
{
    AUDIO_ROUTE_DAC_CHANN0,
    AUDIO_ROUTE_DAC_CHANN1,
    AUDIO_ROUTE_DAC_CHANN2,
    AUDIO_ROUTE_DAC_CHANN3,
    AUDIO_ROUTE_DAC_CHANN_NUM,
} T_AUDIO_ROUTE_DAC_CHANN;

/**
 * \brief Define the Audio Route DAC mix point.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_dac_mix_point
{
    AUDIO_ROUTE_DAC_MIX_POINT0,
    AUDIO_ROUTE_DAC_MIX_POINT1,
    AUDIO_ROUTE_DAC_MIX_POINT2,
    AUDIO_ROUTE_DAC_MIX_POINT3,
    AUDIO_ROUTE_DAC_MIX_POINT4,
    AUDIO_ROUTE_DAC_MIX_POINT5,
    AUDIO_ROUTE_DAC_MIX_POINT6,
    AUDIO_ROUTE_DAC_MIX_POINT7,
    AUDIO_ROUTE_DAC_MIX_POINT_NUM,
} T_AUDIO_ROUTE_DAC_MIX_POINT;

/**
 * \brief Define the Audio Route MIC identifier.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_mic_id
{
    AUDIO_ROUTE_DMIC1   = 0x00, /**< Digital microphone 1 */
    AUDIO_ROUTE_DMIC2   = 0x01, /**< Digital microphone 2 */
    AUDIO_ROUTE_AMIC1   = 0x02, /**< Analog  microphone 1 */
    AUDIO_ROUTE_AMIC2   = 0x03, /**< Analog  microphone 2 */
    AUDIO_ROUTE_AMIC3   = 0x04, /**< Analog  microphone 3 */
    AUDIO_ROUTE_AMIC4   = 0x05, /**< Analog  microphone 4 */
    AUDIO_ROUTE_DMIC3   = 0x06, /**< Digital microphone 3 */
    AUDIO_ROUTE_REF_MIC = 0x0E, /**< Internal microphone for reference signal in */
    AUDIO_ROUTE_EXT_MIC = 0x0F, /**< External microphone */
} T_AUDIO_ROUTE_MIC_ID;

/**
 * \brief Define the Audio Route MIC type.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_mic_type
{
    AUDIO_ROUTE_MIC_SINGLE_END   = 0x00,
    AUDIO_ROUTE_MIC_DIFFERENTIAL = 0x01,
    AUDIO_ROUTE_MIC_FALLING      = 0x02,
    AUDIO_ROUTE_MIC_RAISING      = 0x03,
} T_AUDIO_ROUTE_MIC_TYPE;

/**
 * \brief Define the Audio Route MIC class.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_mic_class
{
    AUDIO_ROUTE_MIC_NORMAL = 0x00,
    AUDIO_ROUTE_MIC_LP     = 0x01,
    AUDIO_ROUTE_MIC_SNR    = 0x02,
} T_AUDIO_ROUTE_MIC_CLASS;

/**
 * \brief Define the Audio Route device MIC.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct t_audio_route_device_mic
{
    T_AUDIO_ROUTE_MIC_ID      id;
    T_AUDIO_ROUTE_MIC_TYPE    type;
    T_AUDIO_ROUTE_MIC_CLASS   class;
    T_AUDIO_ROUTE_ADC_CHANN   adc_ch;
    uint8_t                   ana_gain;
    uint8_t                   dig_gain;
    uint8_t                   dig_boost_gain;
} T_AUDIO_ROUTE_DEVICE_MIC;

/**
 * \brief Define the Audio Route SPK identifier.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_spk_id
{
    AUDIO_ROUTE_SPK1    = 0x00, /**< Physical speaker 1 */
    AUDIO_ROUTE_SPK2    = 0x01, /**< Physical speaker 2 */
    AUDIO_ROUTE_REF_SPK = 0x06, /**< Internal speaker for reference signal out  */
    AUDIO_ROUTE_EXT_SPK = 0x07, /**< External speaker */
} T_AUDIO_ROUTE_SPK_ID;

/**
 * \brief Define the Audio Route SPK type.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_spk_type
{
    AUDIO_ROUTE_SPK_SINGLE_END   = 0x00,
    AUDIO_ROUTE_SPK_DIFFERENTIAL = 0x01,
    AUDIO_ROUTE_SPK_CAPLESS      = 0x02,
} T_AUDIO_ROUTE_SPK_TYPE;

/**
 * \brief Define the Audio Route SPK Class.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_spk_class
{
    AUDIO_ROUTE_SPK_CLASS_D  = 0x00,
    AUDIO_ROUTE_SPK_CLASS_AB = 0x01,
} T_AUDIO_ROUTE_SPK_CLASS;

/**
 * \brief Define the Audio Route SPDIF identifier.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_spdif_id
{
    AUDIO_ROUTE_SPDIF_0 = 0,           /**< Physical endpoint: spdif 0 */
} T_AUDIO_ROUTE_SPDIF_ID;

/**
 * \brief Define the Audio Route SPDIF type.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_spdif_type
{
    AUDIO_ROUTE_SPDIF_DEFAULT = 0,
} T_AUDIO_ROUTE_SPDIF_TYPE;

/**
 * \brief Define the Audio Route Gateway type.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_gateway_type
{
    AUDIO_ROUTE_GATEWAY_SPORT = 0,
    AUDIO_ROUTE_GATEWAY_SPDIF = 1,
    AUDIO_ROUTE_GATEWAY_VAD   = 2,
} T_AUDIO_ROUTE_GATEWAY_TYPE;

/**
 * \brief Define the Audio Route device SPK.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct t_audio_route_device_spk
{
    T_AUDIO_ROUTE_SPK_ID        id;
    T_AUDIO_ROUTE_SPK_TYPE      type;
    T_AUDIO_ROUTE_SPK_CLASS     class;
    T_AUDIO_ROUTE_DAC_CHANN     dac_ch;
    uint8_t                     ana_gain;
    uint8_t                     dig_gain;
    uint8_t                     mix_en;
} T_AUDIO_ROUTE_DEVICE_SPK;

/**
 * \brief Define the Audio Route AUX identifier.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_aux_id
{
    AUDIO_ROUTE_AUX_L = 0x00,        /**< AUX-IN L */
    AUDIO_ROUTE_AUX_R = 0x01,        /**< AUX-IN R */
} T_AUDIO_ROUTE_AUX_ID;

/**
 * \brief Define the Audio Route AUX class.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_aux_class
{
    AUDIO_ROUTE_AUX_NORMAL = 0x00,
    AUDIO_ROUTE_AUX_LP     = 0x01,
} T_AUDIO_ROUTE_AUX_CLASS;

/**
 * \brief Define the Audio Route device AUX in.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct  t_audio_route_device_aux_in
{
    T_AUDIO_ROUTE_AUX_ID      id;
    T_AUDIO_ROUTE_AUX_CLASS   class;
    T_AUDIO_ROUTE_ADC_CHANN   adc_ch;
    bool                      equalizer_en;
    uint8_t                   ana_gain;
    uint8_t                   dig_gain;
    uint8_t                   dig_boost_gain;
} T_AUDIO_ROUTE_DEVICE_AUX_IN;

/**
 * \brief Define the Audio Route device SPDIF in.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct  t_audio_route_device_spdif_in
{
    T_AUDIO_ROUTE_SPDIF_ID      id;
    T_AUDIO_ROUTE_SPDIF_TYPE    type;
} T_AUDIO_ROUTE_DEVICE_SPDIF_IN;

/**
 * \brief Define the Audio Route device SPDIF out.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct  t_audio_route_device_spdif_out
{
    T_AUDIO_ROUTE_SPDIF_ID      id;
    T_AUDIO_ROUTE_SPDIF_TYPE    type;
} T_AUDIO_ROUTE_DEVICE_SPDIF_OUT;

/**
 * \brief Define the Audio Route ADC digital gain.
 *
 * \details ADC digital gain in (dB * 128) step unit is defined as adc_gain = gain_db * 128,
 *          where the gain_db ranges from -128.0 dB to + 127.0 dB.
 *
 * \note    As some platforms do not support floating point arithmetic, Audio Route inputs
 *          the converted dB in signed integer for the underlying modules.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct t_audio_route_adc_gain
{
    int16_t left_gain;
    int16_t right_gain;
} T_AUDIO_ROUTE_ADC_GAIN;

/**
 * \brief Define the Audio Route DAC digital gain.
 *
 * \details DAC digital gain in (dB * 128) step unit is defined as dac_gain = gain_db * 128,
 *          where the gain_db ranges from -128.0 dB to + 127.0 dB.
 *
 * \note    As some platforms do not support floating point arithmetic, Audio Route inputs
 *          the converted dB in signed integer for the underlying modules.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct t_audio_route_dac_gain
{
    int16_t left_gain;
    int16_t right_gain;
} T_AUDIO_ROUTE_DAC_GAIN;

/**
 * \brief Define the Audio Route IO polarity.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_io_polarity
{
    AUDIO_ROUTE_IO_POLARITY_FORWARD = 0x00,
    AUDIO_ROUTE_IO_POLARITY_REVERSE = 0x01,
} T_AUDIO_ROUTE_IO_POLARITY;

/**
 * \brief Define the Audio Route HW Equalizer (EQ) type.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_hw_eq_type
{
    AUDIO_ROUTE_HW_EQ_SPK = 0x00,
    AUDIO_ROUTE_HW_EQ_MIC = 0x01,
    AUDIO_ROUTE_HW_EQ_AUX = 0x02,
    AUDIO_ROUTE_HW_EQ_NUM,
} T_AUDIO_ROUTE_HW_EQ_TYPE;

/**
 * \brief Define the Audio Route Gateway database.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct t_audio_route_gateway_db
{
    T_AUDIO_ROUTE_GATEWAY_ROLE        role;
    T_AUDIO_ROUTE_GATEWAY_BRIDGE      bridge;
    T_AUDIO_ROUTE_GATEWAY_FIFO_MODE   fifo_mode;
    T_AUDIO_ROUTE_GATEWAY_CHANN_MODE  chann_mode;
    T_AUDIO_ROUTE_GATEWAY_FORMAT      format;
    T_AUDIO_ROUTE_GATEWAY_DATA_LEN    data_len;
    T_AUDIO_ROUTE_GATEWAY_CHANN_LEN   chann_len;
    uint32_t                          sample_rate;
} T_AUDIO_ROUTE_GATEWAY_DB;

/**
 * \brief Define the Audio Route Endpoint attribute.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef union t_audio_route_endpoint_attr
{
    T_AUDIO_ROUTE_DEVICE_MIC       mic;
    T_AUDIO_ROUTE_DEVICE_SPK       spk;
    T_AUDIO_ROUTE_DEVICE_AUX_IN    aux_in;
    T_AUDIO_ROUTE_DEVICE_SPDIF_IN  spdif_in;
    T_AUDIO_ROUTE_DEVICE_SPDIF_OUT spdif_out;
} T_AUDIO_ROUTE_ENDPOINT_ATTR;

/**
 * \brief Define the Audio Route Gateway attribute.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct t_audio_route_gateway_attr
{
    T_AUDIO_ROUTE_GATEWAY_TYPE        type;
    T_AUDIO_ROUTE_GATEWAY_IDX         idx;
    T_AUDIO_ROUTE_GATEWAY_DIR         dir;
    uint8_t                           channs;
    T_AUDIO_ROUTE_GATEWAY_ROLE        role;
    T_AUDIO_ROUTE_GATEWAY_CLK_SRC     clk_src;
    T_AUDIO_ROUTE_GATEWAY_BRIDGE      bridge;
    T_AUDIO_ROUTE_GATEWAY_FIFO_MODE   fifo_mode;
    T_AUDIO_ROUTE_GATEWAY_CHANN_MODE  chann_mode;
    T_AUDIO_ROUTE_GATEWAY_FORMAT      format;
    T_AUDIO_ROUTE_GATEWAY_DATA_LEN    data_len;
    T_AUDIO_ROUTE_GATEWAY_CHANN_LEN   chann_len;
    uint32_t                          sample_rate;
} T_AUDIO_ROUTE_GATEWAY_ATTR;

/**
 * \brief Define the Audio Route entry.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct t_audio_route_entry
{
    T_AUDIO_ROUTE_IO_TYPE           io_type;
    T_AUDIO_ROUTE_IO_POLARITY       io_polarity;
    T_AUDIO_ROUTE_ENDPOINT_TYPE     endpoint_type;
    T_AUDIO_ROUTE_ENDPOINT_ATTR     endpoint_attr;
    T_AUDIO_ROUTE_GATEWAY_ATTR      gateway_attr;
} T_AUDIO_ROUTE_ENTRY;

/**
 * \brief Define the Audio Route path.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct t_audio_route_path
{
    uint8_t                      entry_num;
    T_AUDIO_ROUTE_ENTRY         *entry;
} T_AUDIO_ROUTE_PATH;

/**
 * \brief Define the Audio Route DAC digital gain callback prototype.
 *
 * \details Applications use \ref audio_route_gain_register to register the DAC digital
 *          gain adjustment callback. Audio Route will request the DAC digital gain value
 *          at the specified level actively at runtime.
 *
 * \param[in]     category  The Audio Category \ref T_AUDIO_CATEGORY.
 * \param[in]     level     The requested DAC digital gain level.
 * \param[out]    gain      The corresponding DAC digital gain value \ref T_AUDIO_ROUTE_DAC_GAIN.
 *
 * \return  The status of getting the DAC digital gain value.
 * \retval  true    The DAC digital gain value was got successfully.
 * \retval  false   The DAC digital gain value was failed to get.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef bool (*T_AUDIO_ROUTE_DAC_GAIN_CBACK)(T_AUDIO_CATEGORY        category,
                                             uint32_t                level,
                                             T_AUDIO_ROUTE_DAC_GAIN *gain);

/**
 * \brief Define the Audio Route ADC digital gain callback prototype.
 *
 * \details Applications use \ref audio_route_gain_register to register the ADC digital
 *          gain adjustment callback. Audio Route will request the ADC digital gain value
 *          at the specified level actively at runtime.
 *
 * \param[in]     category  The Audio Category \ref T_AUDIO_CATEGORY.
 * \param[in]     level     The requested ADC digital gain level.
 * \param[out]    gain      The corresponding ADC digital gain value \ref T_AUDIO_ROUTE_ADC_GAIN.
 *
 * \return  The status of getting the ADC digital gain value.
 * \retval  true    The ADC digital gain value was got successfully.
 * \retval  false   The ADC digital gain value was failed to get.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef bool (*T_AUDIO_ROUTE_ADC_GAIN_CBACK)(T_AUDIO_CATEGORY        category,
                                             uint32_t                level,
                                             T_AUDIO_ROUTE_ADC_GAIN *gain);

/**
 * \brief Define the Audio Route hardware EQ callback prototype.
 *
 * \details Applications use \ref audio_route_hw_eq_register to register the hardware EQ callback.
 *          Audio Route will request the hardware EQ coefficients actively at runtime and the
 *          applications shall use \ref audio_route_hw_eq_apply to apply the specific hardware EQ
 *          coefficients if needed.
 *
 * \param[in] type         Hardware EQ type \ref T_AUDIO_ROUTE_HW_EQ_TYPE.
 * \param[in] chann        The specified DAC channel \ref T_AUDIO_ROUTE_DAC_CHANN or ADC channel
 *                         \ref T_AUDIO_ROUTE_ADC_CHANN where the required hardware EQ is applied.
 * \param[in] sample_rate  Hardware EQ sample rate.
 *
 * \return  The result of the callback function.
 * \retval  true    The hardware EQ coefficients were got successfully.
 * \retval  false   The hardware EQ coefficients were failed to get.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef bool (*T_AUDIO_ROUTE_HW_EQ_CBACK)(T_AUDIO_ROUTE_HW_EQ_TYPE type,
                                          uint8_t                  chann,
                                          uint32_t                 sample_rate);

/**
 * \brief   Register the Audio Route ADC and/or DAC digital gain callback.
 *
 * \param[in] category    The Audio Category \ref T_AUDIO_CATEGORY.
 * \param[in] dac_cback   The Audio Route DAC gain callback \ref T_AUDIO_ROUTE_DAC_GAIN_CBACK.
 * \param[in] adc_cback   The Audio Route ADC gain callback \ref T_AUDIO_ROUTE_ADC_GAIN_CBACK.
 *
 * \return  The status of registering the ADC and DAC digital gain callback.
 * \retval  true    The Audio Route digital gain callback was registered successfully.
 * \retval  false   The Audio Route digital gain callback was failed to register.
 *
 * \note    The ADC or DAC digital gain callback shall be set to NULL if not supported.
 *
 * \ingroup AUDIO_ROUTE
 */
bool audio_route_gain_register(T_AUDIO_CATEGORY             category,
                               T_AUDIO_ROUTE_DAC_GAIN_CBACK dac_cback,
                               T_AUDIO_ROUTE_ADC_GAIN_CBACK adc_cback);

/**
 * \brief   Unregister the Audio Route ADC and DAC digital callback.
 *
 * \param[in] category    The Audio Category \ref T_AUDIO_CATEGORY.
 *
 * \return  void.
 *
 * \ingroup AUDIO_ROUTE
 */
void audio_route_gain_unregister(T_AUDIO_CATEGORY category);

/**
 * \brief   Get the Audio Route ADC gain.
 *
 * \param[in]  category The Audio Category \ref T_AUDIO_CATEGORY.
 * \param[in]  level    The ADC digital gain level.
 * \param[out] gain     The ADC digital gain value \ref T_AUDIO_ROUTE_ADC_GAIN.
 *
 * \return  The status of getting ADC gain.
 *
 * \ingroup AUDIO_ROUTE
 */
bool audio_route_adc_gain_get(T_AUDIO_CATEGORY        category,
                              uint32_t                level,
                              T_AUDIO_ROUTE_ADC_GAIN *gain);

/**
 * \brief   Get the Audio Route DAC gain.
 *
 * \param[in]  category The Audio Category \ref T_AUDIO_CATEGORY.
 * \param[in]  level    The DAC digital gain level.
 * \param[out] gain     The DAC digital gain value \ref T_AUDIO_ROUTE_DAC_GAIN.
 *
 * \return  The status of getting DAC gain.
 *
 * \ingroup AUDIO_ROUTE
 */
bool audio_route_dac_gain_get(T_AUDIO_CATEGORY        category,
                              uint32_t                level,
                              T_AUDIO_ROUTE_DAC_GAIN *gain);

/**
 * \brief   Register the Audio Route hardware EQ callback.
 *
 * \param[in] cback   The Audio Route hardware EQ callback \ref T_AUDIO_ROUTE_HW_EQ_CBACK.
 *
 * \return  The status of registering the hardware EQ callback.
 * \retval  true    The Audio Route hardware EQ callback was registered successfully.
 * \retval  false   The Audio Route hardware EQ callback was failed to register.
 *
 * \note    The hardware EQ callback shall be set to NULL if not supported.
 *
 * \ingroup AUDIO_ROUTE
 */
bool audio_route_hw_eq_register(T_AUDIO_ROUTE_HW_EQ_CBACK cback);

/**
 * \brief   Unregister the Audio Route hardware EQ callback.
 *
 * \return  void.
 *
 * \ingroup AUDIO_ROUTE
 */
void audio_route_hw_eq_unregister(void);

/**
 * \brief   Apply the Audio Route hardware EQ coefficients.
 *
 * \param[in] type   Hardware EQ type \ref T_AUDIO_ROUTE_HW_EQ_TYPE.
 * \param[in] chann  The specified DAC channel \ref T_AUDIO_ROUTE_DAC_CHANN or ADC channel
 *                   \ref T_AUDIO_ROUTE_ADC_CHANN where the required hardware EQ is applied.
 * \param[in] buf    Hardware EQ band coefficients buffer.
 * \param[in] len    Hardware EQ band coefficients length.
 *
 * \return          The status of applying hardware EQ coefficients.
 * \retval true     Hardware EQ coefficients was applied successfully.
 * \retval false    Hardware EQ coefficients was failed to apply.
 *
 * \ingroup AUDIO_ROUTE
 */
bool audio_route_hw_eq_apply(T_AUDIO_ROUTE_HW_EQ_TYPE  type,
                             uint8_t                   chann,
                             uint8_t                  *buf,
                             uint16_t                  len);

/**
 * \brief   Get the built-in Audio Route path.
 *
 * \param[in] category    Audio Category \ref T_AUDIO_CATEGORY.
 * \param[in] device      Audio Route device \ref AUDIO_DEVICE_BITMASK.
 *
 * \return The Audio Route path.
 *
 * \ingroup AUDIO_ROUTE
 */
T_AUDIO_ROUTE_PATH audio_route_path_take(T_AUDIO_CATEGORY category,
                                         uint32_t         device);

/**
 * \brief   Free the Audio Route path.
 *
 * \param[in] path    Audio Route path \ref T_AUDIO_ROUTE_PATH.
 *
 * \ingroup AUDIO_ROUTE
 */
void audio_route_path_give(T_AUDIO_ROUTE_PATH *path);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_ROUTE_H_ */
