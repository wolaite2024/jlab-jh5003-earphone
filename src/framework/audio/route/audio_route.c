/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "os_queue.h"
#include "trace.h"
#include "audio_route_cfg.h"
#include "audio_route.h"
#include "audio_path.h"
#include "codec_mgr.h"

typedef struct t_audio_route
{
    struct t_audio_route *next;
    void                 *owner;
    uint8_t               max_num;
    uint8_t               entry_num;
    T_AUDIO_ROUTE_ENTRY   entry[0];
} T_AUDIO_ROUTE;

typedef struct t_audio_route_db
{
    T_AUDIO_ROUTE_ADC_GAIN_CBACK adc_gain_cback[AUDIO_CATEGORY_NUM];
    T_AUDIO_ROUTE_DAC_GAIN_CBACK dac_gain_cback[AUDIO_CATEGORY_NUM];
    T_AUDIO_ROUTE_HW_EQ_CBACK    eq_cback;
    uint32_t                     device[AUDIO_CATEGORY_NUM];
    T_OS_QUEUE                   routes;
} T_AUDIO_ROUTE_DB;

static T_AUDIO_ROUTE_DB *audio_route_db = NULL;

static const uint32_t sample_rate_map[] =
{
    [AUDIO_ROUTE_GATEWAY_SR_8KHZ]       = 8000,
    [AUDIO_ROUTE_GATEWAY_SR_16KHZ]      = 16000,
    [AUDIO_ROUTE_GATEWAY_SR_32KHZ]      = 32000,
    [AUDIO_ROUTE_GATEWAY_SR_44P1KHZ]    = 44100,
    [AUDIO_ROUTE_GATEWAY_SR_48KHZ]      = 48000,
    [AUDIO_ROUTE_GATEWAY_SR_88P2KHZ]    = 88200,
    [AUDIO_ROUTE_GATEWAY_SR_96KHZ]      = 96000,
    [AUDIO_ROUTE_GATEWAY_SR_192KHZ]     = 192000,
    [AUDIO_ROUTE_GATEWAY_SR_12KHZ]      = 12000,
    [AUDIO_ROUTE_GATEWAY_SR_24KHZ]      = 24000,
    [AUDIO_ROUTE_GATEWAY_SR_11P025KHZ]  = 11025,
    [AUDIO_ROUTE_GATEWAY_SR_22P05KHZ]   = 22050,
    [AUDIO_ROUTE_GATEWAY_SR_64KHZ]      = 64000,
    [AUDIO_ROUTE_GATEWAY_SR_DYNAMIC]    = 0,
};

static void audio_route_gateway_db_get(T_AUDIO_ROUTE_GATEWAY_TYPE  type,
                                       T_AUDIO_ROUTE_GATEWAY_IDX   idx,
                                       T_AUDIO_ROUTE_GATEWAY_DIR   dir,
                                       T_AUDIO_ROUTE_GATEWAY_ATTR *attr)
{
    if (type == AUDIO_ROUTE_GATEWAY_SPORT)
    {
        switch (idx)
        {
        case AUDIO_ROUTE_GATEWAY_0:
            {
                attr->role    = (T_AUDIO_ROUTE_GATEWAY_ROLE)audio_route_cfg.sport0_role;
                attr->clk_src = (T_AUDIO_ROUTE_GATEWAY_CLK_SRC)audio_route_cfg.sport0_bclk_src;

                if (dir == AUDIO_ROUTE_GATEWAY_DIR_TX)
                {
                    attr->bridge      = (T_AUDIO_ROUTE_GATEWAY_BRIDGE)audio_route_cfg.sport0_tx_bridge;
                    attr->fifo_mode   = (T_AUDIO_ROUTE_GATEWAY_FIFO_MODE)audio_route_cfg.sport0_tx_fifo_mode;
                    attr->chann_mode  = (T_AUDIO_ROUTE_GATEWAY_CHANN_MODE)audio_route_cfg.sport0_tx_chann_mode;
                    attr->format      = (T_AUDIO_ROUTE_GATEWAY_FORMAT)audio_route_cfg.sport0_tx_format;
                    attr->data_len    = (T_AUDIO_ROUTE_GATEWAY_DATA_LEN)audio_route_cfg.sport0_tx_data_len;
                    attr->chann_len   = (T_AUDIO_ROUTE_GATEWAY_CHANN_LEN)audio_route_cfg.sport0_tx_chann_len;
                    attr->sample_rate = sample_rate_map[audio_route_cfg.sport0_tx_sample_rate];
                }
                else
                {
                    attr->bridge      = (T_AUDIO_ROUTE_GATEWAY_BRIDGE)audio_route_cfg.sport0_rx_bridge;
                    attr->fifo_mode   = (T_AUDIO_ROUTE_GATEWAY_FIFO_MODE)audio_route_cfg.sport0_rx_fifo_mode;
                    attr->chann_mode  = (T_AUDIO_ROUTE_GATEWAY_CHANN_MODE)audio_route_cfg.sport0_rx_chann_mode;
                    attr->format      = (T_AUDIO_ROUTE_GATEWAY_FORMAT)audio_route_cfg.sport0_rx_format;
                    attr->data_len    = (T_AUDIO_ROUTE_GATEWAY_DATA_LEN)audio_route_cfg.sport0_rx_data_len;
                    attr->chann_len   = (T_AUDIO_ROUTE_GATEWAY_CHANN_LEN)audio_route_cfg.sport0_rx_chann_len;
                    attr->sample_rate = sample_rate_map[audio_route_cfg.sport0_rx_sample_rate];
                }
            }
            break;

        case AUDIO_ROUTE_GATEWAY_1:
            {
                attr->role    = (T_AUDIO_ROUTE_GATEWAY_ROLE)audio_route_cfg.sport1_role;
                attr->clk_src = (T_AUDIO_ROUTE_GATEWAY_CLK_SRC)audio_route_cfg.sport1_bclk_src;

                if (dir == AUDIO_ROUTE_GATEWAY_DIR_TX)
                {
                    attr->bridge      = (T_AUDIO_ROUTE_GATEWAY_BRIDGE)audio_route_cfg.sport1_tx_bridge;
                    attr->fifo_mode   = (T_AUDIO_ROUTE_GATEWAY_FIFO_MODE)audio_route_cfg.sport1_tx_fifo_mode;
                    attr->chann_mode  = (T_AUDIO_ROUTE_GATEWAY_CHANN_MODE)audio_route_cfg.sport1_tx_chann_mode;
                    attr->format      = (T_AUDIO_ROUTE_GATEWAY_FORMAT)audio_route_cfg.sport1_tx_format;
                    attr->data_len    = (T_AUDIO_ROUTE_GATEWAY_DATA_LEN)audio_route_cfg.sport1_tx_data_len;
                    attr->chann_len   = (T_AUDIO_ROUTE_GATEWAY_CHANN_LEN)audio_route_cfg.sport1_tx_chann_len;
                    attr->sample_rate = sample_rate_map[audio_route_cfg.sport1_tx_sample_rate];
                }
                else
                {
                    attr->bridge      = (T_AUDIO_ROUTE_GATEWAY_BRIDGE)audio_route_cfg.sport1_rx_bridge;
                    attr->fifo_mode   = (T_AUDIO_ROUTE_GATEWAY_FIFO_MODE)audio_route_cfg.sport1_rx_fifo_mode;
                    attr->chann_mode  = (T_AUDIO_ROUTE_GATEWAY_CHANN_MODE)audio_route_cfg.sport1_rx_chann_mode;
                    attr->format      = (T_AUDIO_ROUTE_GATEWAY_FORMAT)audio_route_cfg.sport1_rx_format;
                    attr->data_len    = (T_AUDIO_ROUTE_GATEWAY_DATA_LEN)audio_route_cfg.sport1_rx_data_len;
                    attr->chann_len   = (T_AUDIO_ROUTE_GATEWAY_CHANN_LEN)audio_route_cfg.sport1_rx_chann_len;
                    attr->sample_rate = sample_rate_map[audio_route_cfg.sport1_rx_sample_rate];
                }
            }
            break;

        case AUDIO_ROUTE_GATEWAY_2:
            {
                attr->role    = (T_AUDIO_ROUTE_GATEWAY_ROLE)audio_route_cfg.sport2_role;
                attr->clk_src = (T_AUDIO_ROUTE_GATEWAY_CLK_SRC)audio_route_cfg.sport2_bclk_src;

                if (dir == AUDIO_ROUTE_GATEWAY_DIR_TX)
                {
                    attr->bridge      = (T_AUDIO_ROUTE_GATEWAY_BRIDGE)audio_route_cfg.sport2_tx_bridge;
                    attr->fifo_mode   = (T_AUDIO_ROUTE_GATEWAY_FIFO_MODE)audio_route_cfg.sport2_tx_fifo_mode;
                    attr->chann_mode  = (T_AUDIO_ROUTE_GATEWAY_CHANN_MODE)audio_route_cfg.sport2_tx_chann_mode;
                    attr->format      = (T_AUDIO_ROUTE_GATEWAY_FORMAT)audio_route_cfg.sport2_tx_format;
                    attr->data_len    = (T_AUDIO_ROUTE_GATEWAY_DATA_LEN)audio_route_cfg.sport2_tx_data_len;
                    attr->chann_len   = (T_AUDIO_ROUTE_GATEWAY_CHANN_LEN)audio_route_cfg.sport2_tx_chann_len;
                    attr->sample_rate = sample_rate_map[audio_route_cfg.sport2_tx_sample_rate];
                }
                else
                {
                    attr->bridge      = (T_AUDIO_ROUTE_GATEWAY_BRIDGE)audio_route_cfg.sport2_rx_bridge;
                    attr->fifo_mode   = (T_AUDIO_ROUTE_GATEWAY_FIFO_MODE)audio_route_cfg.sport2_rx_fifo_mode;
                    attr->chann_mode  = (T_AUDIO_ROUTE_GATEWAY_CHANN_MODE)audio_route_cfg.sport2_rx_chann_mode;
                    attr->format      = (T_AUDIO_ROUTE_GATEWAY_FORMAT)audio_route_cfg.sport2_rx_format;
                    attr->data_len    = (T_AUDIO_ROUTE_GATEWAY_DATA_LEN)audio_route_cfg.sport2_rx_data_len;
                    attr->chann_len   = (T_AUDIO_ROUTE_GATEWAY_CHANN_LEN)audio_route_cfg.sport2_rx_chann_len;
                    attr->sample_rate = sample_rate_map[audio_route_cfg.sport2_rx_sample_rate];
                }
            }
            break;

        case AUDIO_ROUTE_GATEWAY_3:
            {
                attr->role    = (T_AUDIO_ROUTE_GATEWAY_ROLE)audio_route_cfg.sport3_role;
                attr->clk_src = (T_AUDIO_ROUTE_GATEWAY_CLK_SRC)audio_route_cfg.sport3_bclk_src;

                if (dir == AUDIO_ROUTE_GATEWAY_DIR_TX)
                {
                    attr->bridge      = (T_AUDIO_ROUTE_GATEWAY_BRIDGE)audio_route_cfg.sport3_tx_bridge;
                    attr->fifo_mode   = (T_AUDIO_ROUTE_GATEWAY_FIFO_MODE)audio_route_cfg.sport3_tx_fifo_mode;
                    attr->chann_mode  = (T_AUDIO_ROUTE_GATEWAY_CHANN_MODE)audio_route_cfg.sport3_tx_chann_mode;
                    attr->format      = (T_AUDIO_ROUTE_GATEWAY_FORMAT)audio_route_cfg.sport3_tx_format;
                    attr->data_len    = (T_AUDIO_ROUTE_GATEWAY_DATA_LEN)audio_route_cfg.sport3_tx_data_len;
                    attr->chann_len   = (T_AUDIO_ROUTE_GATEWAY_CHANN_LEN)audio_route_cfg.sport3_tx_chann_len;
                    attr->sample_rate = sample_rate_map[audio_route_cfg.sport3_tx_sample_rate];
                }
                else
                {
                    attr->bridge      = (T_AUDIO_ROUTE_GATEWAY_BRIDGE)audio_route_cfg.sport3_rx_bridge;
                    attr->fifo_mode   = (T_AUDIO_ROUTE_GATEWAY_FIFO_MODE)audio_route_cfg.sport3_rx_fifo_mode;
                    attr->chann_mode  = (T_AUDIO_ROUTE_GATEWAY_CHANN_MODE)audio_route_cfg.sport3_rx_chann_mode;
                    attr->format      = (T_AUDIO_ROUTE_GATEWAY_FORMAT)audio_route_cfg.sport3_rx_format;
                    attr->data_len    = (T_AUDIO_ROUTE_GATEWAY_DATA_LEN)audio_route_cfg.sport3_rx_data_len;
                    attr->chann_len   = (T_AUDIO_ROUTE_GATEWAY_CHANN_LEN)audio_route_cfg.sport3_rx_chann_len;
                    attr->sample_rate = sample_rate_map[audio_route_cfg.sport3_rx_sample_rate];
                }
            }
            break;

        default:
            return ;
        }
    }
    else if (type == AUDIO_ROUTE_GATEWAY_SPDIF)
    {
        switch (idx)
        {
        case AUDIO_ROUTE_GATEWAY_0:
            {
                attr->role    = (T_AUDIO_ROUTE_GATEWAY_ROLE)audio_route_cfg.spdif0_role;
                attr->clk_src = AUDIO_ROUTE_GATEWAY_CLK_SRC_XTAL;

                if (dir == AUDIO_ROUTE_GATEWAY_DIR_TX)
                {
                    attr->bridge      = (T_AUDIO_ROUTE_GATEWAY_BRIDGE)audio_route_cfg.spdif0_tx_bridge;
                    attr->fifo_mode   = (T_AUDIO_ROUTE_GATEWAY_FIFO_MODE)audio_route_cfg.spdif0_tx_fifo_mode;
                    attr->chann_mode  = (T_AUDIO_ROUTE_GATEWAY_CHANN_MODE)audio_route_cfg.spdif0_tx_chann_mode;
                    attr->format      = (T_AUDIO_ROUTE_GATEWAY_FORMAT)audio_route_cfg.spdif0_tx_format;
                    attr->data_len    = (T_AUDIO_ROUTE_GATEWAY_DATA_LEN)audio_route_cfg.spdif0_tx_data_len;
                    attr->chann_len   = (T_AUDIO_ROUTE_GATEWAY_CHANN_LEN)audio_route_cfg.spdif0_tx_chann_len;
                    attr->sample_rate = sample_rate_map[audio_route_cfg.spdif0_tx_sample_rate];
                }
                else
                {
                    attr->bridge      = (T_AUDIO_ROUTE_GATEWAY_BRIDGE)audio_route_cfg.spdif0_rx_bridge;
                    attr->fifo_mode   = (T_AUDIO_ROUTE_GATEWAY_FIFO_MODE)audio_route_cfg.spdif0_rx_fifo_mode;
                    attr->chann_mode  = (T_AUDIO_ROUTE_GATEWAY_CHANN_MODE)audio_route_cfg.spdif0_rx_chann_mode;
                    attr->format      = (T_AUDIO_ROUTE_GATEWAY_FORMAT)audio_route_cfg.spdif0_rx_format;
                    attr->data_len    = (T_AUDIO_ROUTE_GATEWAY_DATA_LEN)audio_route_cfg.spdif0_rx_data_len;
                    attr->chann_len   = (T_AUDIO_ROUTE_GATEWAY_CHANN_LEN)audio_route_cfg.spdif0_rx_chann_len;
                    attr->sample_rate = sample_rate_map[audio_route_cfg.spdif0_rx_sample_rate];
                }
            }
            break;

        default:
            return ;
        }
    }
    else if (type == AUDIO_ROUTE_GATEWAY_VAD)
    {
        switch (idx)
        {
        case AUDIO_ROUTE_GATEWAY_0:
            {
                attr->role = AUDIO_ROUTE_GATEWAY_ROLE_MASTER;
                attr->clk_src = AUDIO_ROUTE_GATEWAY_CLK_SRC_XTAL;
                attr->bridge = AUDIO_ROUTE_GATEWAY_BRIDGE_INTERNAL;
                attr->fifo_mode = (T_AUDIO_ROUTE_GATEWAY_FIFO_MODE)audio_route_cfg.vad0_fifo_mode;
                attr->chann_mode = (T_AUDIO_ROUTE_GATEWAY_CHANN_MODE)audio_route_cfg.vad0_chann_mode;
                attr->format = AUDIO_ROUTE_GATEWAY_FORMAT_I2S_STANDARD;
                attr->data_len = (T_AUDIO_ROUTE_GATEWAY_DATA_LEN)audio_route_cfg.vad0_data_len;
                attr->chann_len = (T_AUDIO_ROUTE_GATEWAY_CHANN_LEN)audio_route_cfg.vad0_chann_len;
                attr->sample_rate = sample_rate_map[audio_route_cfg.vad0_sample_rate];
            }
            break;

        default:
            return ;
        }
    }
}

static T_AUDIO_ROUTE_MIC_CLASS audio_route_mic_class_get(uint8_t mic_id)
{
    T_AUDIO_ROUTE_MIC_CLASS mic_power_class = AUDIO_ROUTE_MIC_NORMAL;

    switch (mic_id)
    {
    case AUDIO_ROUTE_AMIC1:
        {
            mic_power_class = (T_AUDIO_ROUTE_MIC_CLASS)audio_route_cfg.amic1_class;
        }
        break;
    case AUDIO_ROUTE_AMIC2:
        {
            mic_power_class = (T_AUDIO_ROUTE_MIC_CLASS)audio_route_cfg.amic2_class;
        }
        break;
    case AUDIO_ROUTE_AMIC3:
        {
            mic_power_class = (T_AUDIO_ROUTE_MIC_CLASS)audio_route_cfg.amic3_class;
        }
        break;
    case AUDIO_ROUTE_AMIC4:
        {
            mic_power_class = (T_AUDIO_ROUTE_MIC_CLASS)audio_route_cfg.amic4_class;
        }
        break;
    default:
        {
            mic_power_class = AUDIO_ROUTE_MIC_NORMAL;
        }
        break;
    }
    return mic_power_class;
}

static T_AUDIO_ROUTE_AUX_CLASS audio_route_aux_class_get(uint8_t aux_id)
{
    T_AUDIO_ROUTE_AUX_CLASS aux_power_class = AUDIO_ROUTE_AUX_NORMAL;

    switch (aux_id)
    {
    case AUDIO_ROUTE_AUX_L:
        {
            aux_power_class = (T_AUDIO_ROUTE_AUX_CLASS)audio_route_cfg.aux_l_class;
        }
        break;

    case AUDIO_ROUTE_AUX_R:
        {
            aux_power_class = (T_AUDIO_ROUTE_AUX_CLASS)audio_route_cfg.aux_r_class;
        }
        break;

    default:
        {
            aux_power_class = AUDIO_ROUTE_AUX_NORMAL;
        }
        break;
    }
    return aux_power_class;
}

static uint8_t audio_route_audio_entry_num_get(uint32_t device)
{
    uint8_t entry_num = 0;

    if (device & AUDIO_DEVICE_OUT_SPK)
    {
        entry_num += audio_route_cfg.audio_primary_out_spk_enable;
        entry_num += audio_route_cfg.audio_secondary_out_spk_enable;
        entry_num += audio_route_cfg.audio_primary_ref_out_spk_enable;
        entry_num += audio_route_cfg.audio_secondary_ref_out_spk_enable;
    }

    if (device & AUDIO_DEVICE_OUT_SPDIF)
    {
        entry_num += audio_route_cfg.audio_primary_out_spdif_enable;
        entry_num += audio_route_cfg.audio_secondary_out_spdif_enable;
    }

    return entry_num;
}

static void audio_route_audio_entry_get(T_AUDIO_ROUTE_ENTRY *entry,
                                        uint32_t             device)
{
    if (device & AUDIO_DEVICE_OUT_SPK)
    {
        if (audio_route_cfg.audio_primary_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_AUDIO_PRIMARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.audio_primary_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                              audio_route_cfg.audio_primary_out_spk_dac_ch;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.audio_primary_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.audio_primary_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk1_class;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.audio_primary_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.audio_primary_spk_digital_gain;
            entry->endpoint_attr.spk.mix_en = (T_AUDIO_ROUTE_DAC_MIX_POINT)
                                              audio_route_cfg.dac_0_mix_point;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.audio_primary_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.audio_primary_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.audio_secondary_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_AUDIO_SECONDARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.audio_secondary_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                              audio_route_cfg.audio_secondary_out_spk_dac_ch;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.audio_secondary_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.audio_secondary_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk2_class;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.audio_secondary_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.audio_secondary_spk_digital_gain;
            entry->endpoint_attr.spk.mix_en = (T_AUDIO_ROUTE_DAC_MIX_POINT)
                                              audio_route_cfg.dac_1_mix_point;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.audio_secondary_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.audio_secondary_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.audio_primary_ref_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_AUDIO_PRIMARY_REF_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.audio_primary_ref_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.audio_primary_ref_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.audio_primary_ref_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk1_class;
            entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                              audio_route_cfg.audio_primary_ref_out_spk_dac_ch;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.audio_primary_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.audio_primary_spk_digital_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.audio_primary_ref_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.audio_primary_ref_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.audio_secondary_ref_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_AUDIO_SECONDARY_REF_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.audio_secondary_ref_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.audio_secondary_ref_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.audio_secondary_ref_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk1_class;
            entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                              audio_route_cfg.audio_secondary_ref_out_spk_dac_ch;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.audio_secondary_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.audio_secondary_spk_digital_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.audio_secondary_ref_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.audio_secondary_ref_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }
    }

    if (device & AUDIO_DEVICE_OUT_SPDIF)
    {
        if (audio_route_cfg.audio_primary_out_spdif_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_AUDIO_PRIMARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.audio_primary_out_spdif_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPDIF;
            entry->endpoint_attr.spdif_out.id = (T_AUDIO_ROUTE_SPDIF_ID)
                                                audio_route_cfg.audio_primary_out_spdif_endpoint_idx;
            entry->endpoint_attr.spdif_out.type = (T_AUDIO_ROUTE_SPDIF_TYPE)
                                                  audio_route_cfg.audio_primary_out_spdif_endpoint_type;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPDIF;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.audio_primary_out_spdif_gateway_idx;
            entry->gateway_attr.channs = audio_route_cfg.audio_primary_out_spdif_gateway_ch;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.audio_secondary_out_spdif_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_AUDIO_SECONDARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.audio_secondary_out_spdif_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPDIF;
            entry->endpoint_attr.spdif_out.id = (T_AUDIO_ROUTE_SPDIF_ID)
                                                audio_route_cfg.audio_secondary_out_spdif_endpoint_idx;
            entry->endpoint_attr.spdif_out.type = (T_AUDIO_ROUTE_SPDIF_TYPE)
                                                  audio_route_cfg.audio_secondary_out_spdif_endpoint_type;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPDIF;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.audio_secondary_out_spdif_gateway_idx;
            entry->gateway_attr.channs = audio_route_cfg.audio_secondary_out_spdif_gateway_ch;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
        }
    }
}

static uint8_t audio_route_voice_entry_num_get(uint32_t device)
{
    uint8_t entry_num = 0;

    if (device & AUDIO_DEVICE_OUT_SPK)
    {
        entry_num += audio_route_cfg.voice_primary_out_spk_enable;
        entry_num += audio_route_cfg.voice_secondary_out_spk_enable;
        entry_num += audio_route_cfg.voice_primary_ref_out_spk_enable;
        entry_num += audio_route_cfg.voice_secondary_ref_out_spk_enable;
    }

    if (device & AUDIO_DEVICE_OUT_SPDIF)
    {
        entry_num += audio_route_cfg.voice_primary_out_spdif_enable;
        entry_num += audio_route_cfg.voice_secondary_out_spdif_enable;
    }

    if (device & AUDIO_DEVICE_IN_MIC)
    {
        entry_num += audio_route_cfg.voice_primary_in_mic_enable;
        entry_num += audio_route_cfg.voice_secondary_in_mic_enable;
        entry_num += audio_route_cfg.voice_fusion_in_mic_enable;
        entry_num += audio_route_cfg.voice_primary_ref_in_mic_enable;
        entry_num += audio_route_cfg.voice_secondary_ref_in_mic_enable;
        entry_num += audio_route_cfg.voice_bone_in_mic_enable;
    }

    if (device & AUDIO_DEVICE_IN_SPDIF)
    {
        entry_num += audio_route_cfg.voice_primary_in_spdif_enable;
        entry_num += audio_route_cfg.voice_secondary_in_spdif_enable;
    }

    return entry_num;
}

static void audio_route_voice_entry_get(T_AUDIO_ROUTE_ENTRY *entry,
                                        uint32_t             device)
{
    if (device & AUDIO_DEVICE_OUT_SPK)
    {
        if (audio_route_cfg.voice_primary_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_PRIMARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.voice_primary_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                              audio_route_cfg.voice_primary_out_spk_dac_ch;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.voice_primary_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.voice_primary_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk1_class;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.voice_primary_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.voice_primary_spk_digital_gain;
            entry->endpoint_attr.spk.mix_en = (T_AUDIO_ROUTE_DAC_MIX_POINT)
                                              audio_route_cfg.dac_0_mix_point;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.voice_primary_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.voice_primary_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.voice_secondary_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_SECONDARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.voice_secondary_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                              audio_route_cfg.voice_secondary_out_spk_dac_ch;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.voice_secondary_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.voice_secondary_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk2_class;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.voice_secondary_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.voice_secondary_spk_digital_gain;
            entry->endpoint_attr.spk.mix_en = (T_AUDIO_ROUTE_DAC_MIX_POINT)
                                              audio_route_cfg.dac_1_mix_point;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.voice_secondary_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.voice_secondary_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.voice_primary_ref_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_PRIMARY_REF_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.voice_primary_ref_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.voice_primary_ref_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.voice_primary_ref_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk1_class;
            entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                              audio_route_cfg.voice_primary_ref_out_spk_dac_ch;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.voice_primary_ref_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.voice_primary_ref_spk_digital_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.voice_primary_ref_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.voice_primary_ref_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);

            entry++;
        }

        if (audio_route_cfg.voice_secondary_ref_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_SECONDARY_REF_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.voice_secondary_ref_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.voice_secondary_ref_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.voice_secondary_ref_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk2_class;
            entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                              audio_route_cfg.voice_secondary_ref_out_spk_dac_ch;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.voice_primary_ref_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.voice_primary_ref_spk_digital_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.voice_secondary_ref_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.voice_secondary_ref_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }
    }

    if (device & AUDIO_DEVICE_OUT_SPDIF)
    {
        if (audio_route_cfg.voice_primary_out_spdif_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_PRIMARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.voice_primary_out_spdif_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPDIF;
            entry->endpoint_attr.spdif_out.id = (T_AUDIO_ROUTE_SPDIF_ID)
                                                audio_route_cfg.voice_primary_out_spdif_endpoint_idx;
            entry->endpoint_attr.spdif_out.type = (T_AUDIO_ROUTE_SPDIF_TYPE)
                                                  audio_route_cfg.voice_primary_out_spdif_endpoint_type;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPDIF;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.voice_primary_out_spdif_gateway_idx;
            entry->gateway_attr.channs = audio_route_cfg.voice_primary_out_spdif_gateway_ch;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.voice_secondary_out_spdif_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_SECONDARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.voice_secondary_out_spdif_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPDIF;
            entry->endpoint_attr.spdif_out.id = (T_AUDIO_ROUTE_SPDIF_ID)
                                                audio_route_cfg.voice_secondary_out_spdif_enable;
            entry->endpoint_attr.spdif_out.type = (T_AUDIO_ROUTE_SPDIF_TYPE)
                                                  audio_route_cfg.voice_secondary_out_spdif_endpoint_type;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPDIF;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.voice_secondary_out_spdif_gateway_idx;
            entry->gateway_attr.channs = audio_route_cfg.voice_secondary_out_spdif_gateway_ch;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }
    }

    if (device & AUDIO_DEVICE_IN_MIC)
    {
        if (audio_route_cfg.voice_primary_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_PRIMARY_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.voice_primary_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.voice_primary_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.voice_primary_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.voice_primary_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.voice_primary_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.voice_primary_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.voice_primary_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain = audio_route_cfg.voice_primary_mic_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.voice_primary_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.voice_primary_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.voice_secondary_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_SECONDARY_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.voice_secondary_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.voice_secondary_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.voice_secondary_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.voice_secondary_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.voice_secondary_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.voice_secondary_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.voice_secondary_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain =
                audio_route_cfg.voice_secondary_mic_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.voice_secondary_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.voice_secondary_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.voice_fusion_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_FUSION_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.voice_fusion_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.voice_fusion_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.voice_fusion_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.voice_fusion_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.voice_fusion_in_mic_endpoint_idx_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.voice_fusion_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.voice_fusion_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain = audio_route_cfg.voice_fusion_mic_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.voice_fusion_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.voice_fusion_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.voice_primary_ref_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_PRIMARY_REF_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.voice_primary_ref_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.voice_primary_ref_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.voice_primary_ref_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.voice_primary_ref_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.voice_primary_ref_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.voice_primary_ref_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.voice_primary_ref_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain =
                audio_route_cfg.voice_primary_ref_mic_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.voice_primary_ref_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.voice_primary_ref_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.voice_secondary_ref_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_SECONDARY_REF_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.voice_secondary_ref_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.voice_secondary_ref_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.voice_secondary_ref_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.voice_secondary_ref_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.voice_secondary_ref_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.voice_primary_ref_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.voice_primary_ref_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain =
                audio_route_cfg.voice_primary_ref_mic_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.voice_secondary_ref_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.voice_secondary_ref_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.voice_bone_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_BONE_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.voice_bone_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.voice_bone_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.voice_bone_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.voice_bone_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.voice_bone_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.voice_bone_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.voice_bone_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain = audio_route_cfg.voice_bone_mic_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.voice_bone_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.voice_bone_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }
    }

    if (device & AUDIO_DEVICE_IN_SPDIF)
    {
        if (audio_route_cfg.voice_primary_in_spdif_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_PRIMARY_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.voice_primary_in_spdif_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPDIF;
            entry->endpoint_attr.spdif_in.id = (T_AUDIO_ROUTE_SPDIF_ID)
                                               audio_route_cfg.voice_primary_in_spdif_endpoint_idx;
            entry->endpoint_attr.spdif_in.type = (T_AUDIO_ROUTE_SPDIF_TYPE)
                                                 audio_route_cfg.voice_primary_in_spdif_endpoint_type;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPDIF;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.voice_primary_in_spdif_gateway_idx;
            entry->gateway_attr.channs = audio_route_cfg.voice_primary_in_spdif_gateway_ch;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.voice_secondary_in_spdif_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_SECONDARY_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.voice_secondary_in_spdif_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPDIF;
            entry->endpoint_attr.spdif_in.id = (T_AUDIO_ROUTE_SPDIF_ID)
                                               audio_route_cfg.voice_secondary_in_spdif_enable;
            entry->endpoint_attr.spdif_in.type = (T_AUDIO_ROUTE_SPDIF_TYPE)
                                                 audio_route_cfg.voice_secondary_in_spdif_endpoint_type;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPDIF;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.voice_secondary_in_spdif_gateway_idx;
            entry->gateway_attr.channs = audio_route_cfg.voice_secondary_in_spdif_gateway_ch;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
        }
    }
}

static uint8_t audio_route_record_entry_num_get(uint32_t device)
{
    uint8_t entry_num = 0;

    if (device & AUDIO_DEVICE_IN_MIC)
    {
        entry_num += audio_route_cfg.record_primary_in_mic_enable;
        entry_num += audio_route_cfg.record_secondary_in_mic_enable;
        entry_num += audio_route_cfg.record_primary_ref_in_mic_enable;
        entry_num += audio_route_cfg.record_secondary_ref_in_mic_enable;
        entry_num += audio_route_cfg.record_fusion_in_mic_enable;
        entry_num += audio_route_cfg.record_bone_in_mic_enable;
    }

    if (device & AUDIO_DEVICE_IN_AUX)
    {
        entry_num += audio_route_cfg.record_primary_in_aux_enable;
        entry_num += audio_route_cfg.record_secondary_in_aux_enable;
    }

    if (device & AUDIO_DEVICE_IN_SPDIF)
    {
        entry_num += audio_route_cfg.record_primary_in_spdif_enable;
        entry_num += audio_route_cfg.record_secondary_in_spdif_enable;
    }

    return entry_num;
}

static void audio_route_record_entry_get(T_AUDIO_ROUTE_ENTRY *entry,
                                         uint32_t             device)
{
    if (device & AUDIO_DEVICE_IN_MIC)
    {
        if (audio_route_cfg.record_primary_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_RECORD_PRIMARY_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.record_primary_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.record_primary_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.record_primary_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.record_primary_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.record_primary_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.record_primary_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.record_primary_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain =
                audio_route_cfg.record_primary_mic_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.record_primary_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.record_primary_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.record_secondary_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_RECORD_SECONDARY_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.record_secondary_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.record_secondary_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.record_secondary_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.record_secondary_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.record_secondary_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.record_secondary_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.record_secondary_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain =
                audio_route_cfg.record_secondary_mic_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.record_secondary_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.record_secondary_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.record_primary_ref_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_RECORD_PRIMARY_REF_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.record_primary_ref_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.record_primary_ref_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.record_primary_ref_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.record_primary_ref_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.record_primary_ref_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.record_primary_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.record_primary_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain = audio_route_cfg.record_primary_mic_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.record_primary_ref_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.record_primary_ref_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.record_secondary_ref_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_RECORD_SECONDARY_REF_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.record_secondary_ref_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.record_secondary_ref_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.record_secondary_ref_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.record_secondary_ref_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.record_secondary_ref_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.record_secondary_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.record_secondary_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain = audio_route_cfg.record_secondary_mic_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.record_primary_ref_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.record_secondary_ref_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.record_fusion_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_RECORD_FUSION_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.record_fusion_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.record_fusion_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.record_fusion_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.record_fusion_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.record_fusion_in_mic_endpoint_idx_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.record_fusion_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.record_fusion_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain = audio_route_cfg.record_fusion_mic_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.record_fusion_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.record_fusion_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.record_bone_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_RECORD_BONE_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.record_bone_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.record_bone_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.record_bone_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.record_bone_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.record_bone_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.record_bone_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.record_bone_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain = audio_route_cfg.record_bone_mic_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.record_bone_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.record_bone_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }
    }

    if (device & AUDIO_DEVICE_IN_AUX)
    {
        if (audio_route_cfg.record_primary_in_aux_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_RECORD_PRIMARY_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.record_aux_left_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_AUX;
            entry->endpoint_attr.aux_in.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                                 audio_route_cfg.record_primary_in_aux_adc_ch;
            entry->endpoint_attr.aux_in.id = (T_AUDIO_ROUTE_AUX_ID)
                                             (audio_route_cfg.record_primary_in_aux_endpoint_idx_l +
                                              (audio_route_cfg.record_primary_in_aux_endpoint_idx_h << 3));
            entry->endpoint_attr.aux_in.class = audio_route_aux_class_get(entry->endpoint_attr.aux_in.id);
            entry->endpoint_attr.aux_in.ana_gain = audio_route_cfg.record_aux_left_analog_gain;
            entry->endpoint_attr.aux_in.dig_gain = audio_route_cfg.record_aux_left_digital_gain;
            entry->endpoint_attr.aux_in.dig_boost_gain =
                audio_route_cfg.record_aux_left_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.record_primary_in_aux_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.record_primary_in_aux_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.record_secondary_in_aux_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_RECORD_SECONDARY_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.record_aux_right_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_AUX;
            entry->endpoint_attr.aux_in.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                                 audio_route_cfg.record_secondary_in_aux_adc_ch;
            entry->endpoint_attr.aux_in.id = (T_AUDIO_ROUTE_AUX_ID)
                                             (audio_route_cfg.record_secondary_in_aux_endpoint_idx_l +
                                              (audio_route_cfg.record_secondary_in_aux_endpoint_idx_h << 3));
            entry->endpoint_attr.aux_in.class = audio_route_aux_class_get(entry->endpoint_attr.aux_in.id);
            entry->endpoint_attr.aux_in.ana_gain = audio_route_cfg.record_aux_right_analog_gain;
            entry->endpoint_attr.aux_in.dig_gain = audio_route_cfg.record_aux_right_digital_gain;
            entry->endpoint_attr.aux_in.dig_boost_gain =
                audio_route_cfg.record_aux_right_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.record_secondary_in_aux_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.record_secondary_in_aux_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }
    }

    if (device & AUDIO_DEVICE_IN_SPDIF)
    {
        if (audio_route_cfg.record_primary_in_spdif_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_RECORD_PRIMARY_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.record_primary_in_spdif_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPDIF;
            entry->endpoint_attr.spdif_in.id = (T_AUDIO_ROUTE_SPDIF_ID)
                                               audio_route_cfg.record_primary_in_spdif_endpoint_idx;
            entry->endpoint_attr.spdif_in.type = (T_AUDIO_ROUTE_SPDIF_TYPE)
                                                 audio_route_cfg.record_primary_in_spdif_endpoint_type;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPDIF;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.record_primary_in_spdif_gateway_idx;
            entry->gateway_attr.channs = audio_route_cfg.record_primary_in_spdif_gateway_ch;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.record_secondary_in_spdif_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_RECORD_SECONDARY_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.record_secondary_in_spdif_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPDIF;
            entry->endpoint_attr.spdif_in.id = (T_AUDIO_ROUTE_SPDIF_ID)
                                               audio_route_cfg.record_secondary_in_spdif_enable;
            entry->endpoint_attr.spdif_in.type = (T_AUDIO_ROUTE_SPDIF_TYPE)
                                                 audio_route_cfg.record_secondary_in_spdif_endpoint_type;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPDIF;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.record_secondary_in_spdif_gateway_idx;
            entry->gateway_attr.channs = audio_route_cfg.record_secondary_in_spdif_gateway_ch;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
        }
    }
}

static uint8_t audio_route_ringtone_entry_num_get(uint32_t device)
{
    uint8_t entry_num = 0;

    if (device & AUDIO_DEVICE_OUT_SPK)
    {
        entry_num += audio_route_cfg.ringtone_primary_out_spk_enable;
        entry_num += audio_route_cfg.ringtone_secondary_out_spk_enable;
        entry_num += audio_route_cfg.ringtone_primary_ref_out_spk_enable;
        entry_num += audio_route_cfg.ringtone_secondary_ref_out_spk_enable;
    }

    if (device & AUDIO_DEVICE_OUT_SPDIF)
    {
        entry_num += audio_route_cfg.ringtone_primary_out_spdif_enable;
        entry_num += audio_route_cfg.ringtone_secondary_out_spdif_enable;
    }

    return entry_num;
}

static void audio_route_ringtone_entry_get(T_AUDIO_ROUTE_ENTRY *entry,
                                           uint32_t             device)
{
    if (device & AUDIO_DEVICE_OUT_SPK)
    {
        if (audio_route_cfg.ringtone_primary_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_RINGTONE_PRIMARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.ringtone_primary_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                              audio_route_cfg.ringtone_primary_out_spk_dac_ch;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.ringtone_primary_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.ringtone_primary_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk1_class;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.ringtone_primary_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.ringtone_primary_spk_digital_gain;
            entry->endpoint_attr.spk.mix_en = (T_AUDIO_ROUTE_DAC_MIX_POINT)
                                              audio_route_cfg.dac_0_mix_point;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.ringtone_primary_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.ringtone_primary_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.ringtone_secondary_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_RINGTONE_SECONDARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.ringtone_secondary_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                              audio_route_cfg.ringtone_secondary_out_spk_dac_ch;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.ringtone_secondary_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.ringtone_secondary_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk2_class;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.ringtone_secondary_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.ringtone_secondary_spk_digital_gain;
            entry->endpoint_attr.spk.mix_en = (T_AUDIO_ROUTE_DAC_MIX_POINT)
                                              audio_route_cfg.dac_1_mix_point;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.ringtone_secondary_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.ringtone_secondary_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.ringtone_primary_ref_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_RINGTONE_PRIMARY_REF_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.ringtone_primary_ref_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.ringtone_primary_ref_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.ringtone_primary_ref_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk1_class;
            entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                              audio_route_cfg.ringtone_primary_ref_out_spk_dac_ch;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.ringtone_primary_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.ringtone_primary_spk_digital_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.ringtone_primary_ref_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.ringtone_primary_ref_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.ringtone_secondary_ref_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_RINGTONE_SECONDARY_REF_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.ringtone_secondary_ref_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.ringtone_secondary_ref_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.ringtone_secondary_ref_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk2_class;
            entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                              audio_route_cfg.ringtone_secondary_ref_out_spk_dac_ch;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.ringtone_primary_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.ringtone_primary_spk_digital_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.ringtone_secondary_ref_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.ringtone_secondary_ref_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }
    }

    if (device & AUDIO_DEVICE_OUT_SPDIF)
    {
        if (audio_route_cfg.ringtone_primary_out_spdif_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_RINGTONE_PRIMARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.ringtone_primary_out_spdif_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPDIF;
            entry->endpoint_attr.spdif_out.id = (T_AUDIO_ROUTE_SPDIF_ID)
                                                audio_route_cfg.ringtone_primary_out_spdif_endpoint_idx;
            entry->endpoint_attr.spdif_out.type = (T_AUDIO_ROUTE_SPDIF_TYPE)
                                                  audio_route_cfg.ringtone_primary_out_spdif_endpoint_type;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPDIF;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.ringtone_primary_out_spdif_gateway_idx;
            entry->gateway_attr.channs = audio_route_cfg.ringtone_primary_out_spdif_gateway_ch;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.ringtone_secondary_out_spdif_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_RINGTONE_SECONDARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.ringtone_secondary_out_spdif_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPDIF;
            entry->endpoint_attr.spdif_out.id = (T_AUDIO_ROUTE_SPDIF_ID)
                                                audio_route_cfg.ringtone_secondary_out_spdif_enable;
            entry->endpoint_attr.spdif_out.type = (T_AUDIO_ROUTE_SPDIF_TYPE)
                                                  audio_route_cfg.ringtone_secondary_out_spdif_endpoint_type;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPDIF;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.ringtone_secondary_out_spdif_gateway_idx;
            entry->gateway_attr.channs = audio_route_cfg.ringtone_secondary_out_spdif_gateway_ch;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
        }
    }
}

static uint8_t audio_route_vp_entry_num_get(uint32_t device)
{
    uint8_t entry_num = 0;

    if (device & AUDIO_DEVICE_OUT_SPK)
    {
        entry_num += audio_route_cfg.vp_primary_out_spk_enable;
        entry_num += audio_route_cfg.vp_secondary_out_spk_enable;
        entry_num += audio_route_cfg.vp_primary_ref_out_spk_enable;
        entry_num += audio_route_cfg.vp_secondary_ref_out_spk_enable;
    }

    if (device & AUDIO_DEVICE_OUT_SPDIF)
    {
        entry_num += audio_route_cfg.vp_primary_out_spdif_enable;
        entry_num += audio_route_cfg.vp_secondary_out_spdif_enable;
    }

    return entry_num;
}

static void audio_route_vp_entry_get(T_AUDIO_ROUTE_ENTRY *entry,
                                     uint32_t             device)
{
    if (device & AUDIO_DEVICE_OUT_SPK)
    {
        if (audio_route_cfg.vp_primary_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_PROMPT_PRIMARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.vp_primary_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                              audio_route_cfg.vp_primary_out_spk_dac_ch;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.vp_primary_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.vp_primary_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk1_class;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.vp_primary_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.vp_primary_spk_digital_gain;
            entry->endpoint_attr.spk.mix_en = (T_AUDIO_ROUTE_DAC_MIX_POINT)audio_route_cfg.dac_0_mix_point;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.vp_primary_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.vp_primary_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.vp_secondary_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_PROMPT_SECONDARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.vp_secondary_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                              audio_route_cfg.vp_secondary_out_spk_dac_ch;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.vp_secondary_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.vp_secondary_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk2_class;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.vp_secondary_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.vp_secondary_spk_digital_gain;
            entry->endpoint_attr.spk.mix_en = (T_AUDIO_ROUTE_DAC_MIX_POINT)audio_route_cfg.dac_1_mix_point;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.vp_secondary_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.vp_secondary_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.vp_primary_ref_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_PROMPT_PRIMARY_REF_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.vp_primary_ref_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.vp_primary_ref_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.vp_primary_ref_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk1_class;
            entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                              audio_route_cfg.vp_primary_ref_out_spk_dac_ch;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.vp_primary_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.vp_primary_spk_digital_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.vp_primary_ref_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.vp_primary_ref_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.vp_secondary_ref_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_PROMPT_SECONDARY_REF_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.vp_secondary_ref_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.vp_secondary_ref_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.vp_secondary_ref_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk2_class;
            entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                              audio_route_cfg.vp_secondary_ref_out_spk_dac_ch;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.vp_primary_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.vp_primary_spk_digital_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.vp_secondary_ref_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.vp_secondary_ref_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }
    }

    if (device & AUDIO_DEVICE_OUT_SPDIF)
    {
        if (audio_route_cfg.vp_primary_out_spdif_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_PROMPT_PRIMARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.vp_primary_out_spdif_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPDIF;
            entry->endpoint_attr.spdif_out.id = (T_AUDIO_ROUTE_SPDIF_ID)
                                                audio_route_cfg.vp_primary_out_spdif_endpoint_idx;
            entry->endpoint_attr.spdif_out.type = (T_AUDIO_ROUTE_SPDIF_TYPE)
                                                  audio_route_cfg.vp_primary_out_spdif_endpoint_type;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPDIF;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.vp_primary_out_spdif_gateway_idx;
            entry->gateway_attr.channs = audio_route_cfg.vp_primary_out_spdif_gateway_ch;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.vp_secondary_out_spdif_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VOICE_PROMPT_SECONDARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.vp_secondary_out_spdif_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPDIF;
            entry->endpoint_attr.spdif_out.id = (T_AUDIO_ROUTE_SPDIF_ID)
                                                audio_route_cfg.vp_secondary_out_spdif_enable;
            entry->endpoint_attr.spdif_out.type = (T_AUDIO_ROUTE_SPDIF_TYPE)
                                                  audio_route_cfg.vp_secondary_out_spdif_endpoint_type;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPDIF;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.vp_secondary_out_spdif_gateway_idx;
            entry->gateway_attr.channs = audio_route_cfg.vp_secondary_out_spdif_gateway_ch;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
        }
    }
}

static uint8_t audio_route_apt_entry_num_get(uint32_t device)
{
    uint8_t entry_num = 0;

    entry_num += audio_route_cfg.apt_primary_out_spk_enable;
    entry_num += audio_route_cfg.apt_secondary_out_spk_enable;
    entry_num += audio_route_cfg.apt_primary_ref_out_spk_enable;
    entry_num += audio_route_cfg.apt_secondary_ref_out_spk_enable;
    entry_num += audio_route_cfg.apt_front_left_in_mic_enable;
    entry_num += audio_route_cfg.apt_front_right_in_mic_enable;
    entry_num += audio_route_cfg.apt_ff_left_in_mic_enable;
    entry_num += audio_route_cfg.apt_ff_right_in_mic_enable;
    entry_num += audio_route_cfg.apt_fb_left_in_mic_enable;
    entry_num += audio_route_cfg.apt_fb_right_in_mic_enable;
    entry_num += audio_route_cfg.apt_bone_left_in_mic_enable;
    entry_num += audio_route_cfg.apt_bone_right_in_mic_enable;
    entry_num += audio_route_cfg.apt_primary_ref_in_mic_enable;
    entry_num += audio_route_cfg.apt_secondary_ref_in_mic_enable;

    return entry_num;
}

static void audio_route_apt_entry_get(T_AUDIO_ROUTE_ENTRY *entry,
                                      uint32_t             device)
{
    if (audio_route_cfg.apt_primary_out_spk_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_APT_PRIMARY_OUT;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.apt_primary_spk_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
        entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                          audio_route_cfg.apt_primary_out_spk_dac_ch;
        entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                      audio_route_cfg.apt_primary_out_spk_endpoint_idx;
        entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                        audio_route_cfg.apt_primary_out_spk_endpoint_type;
        entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk1_class;
        entry->endpoint_attr.spk.ana_gain = audio_route_cfg.apt_primary_spk_analog_gain;
        entry->endpoint_attr.spk.dig_gain = audio_route_cfg.apt_primary_spk_digital_gain;
        entry->endpoint_attr.spk.mix_en = (T_AUDIO_ROUTE_DAC_MIX_POINT)
                                          audio_route_cfg.dac_0_mix_point;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.apt_primary_out_spk_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
        entry->gateway_attr.channs = audio_route_cfg.apt_primary_out_spk_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.apt_secondary_out_spk_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_APT_SECONDARY_OUT;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.apt_secondary_spk_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
        entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                          audio_route_cfg.apt_secondary_out_spk_dac_ch;
        entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                      audio_route_cfg.apt_secondary_out_spk_endpoint_idx;
        entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                        audio_route_cfg.apt_secondary_out_spk_endpoint_type;
        entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk2_class;
        entry->endpoint_attr.spk.ana_gain = audio_route_cfg.apt_secondary_spk_analog_gain;
        entry->endpoint_attr.spk.dig_gain = audio_route_cfg.apt_secondary_spk_digital_gain;
        entry->endpoint_attr.spk.mix_en = (T_AUDIO_ROUTE_DAC_MIX_POINT)
                                          audio_route_cfg.dac_1_mix_point;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.apt_secondary_out_spk_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
        entry->gateway_attr.channs = audio_route_cfg.apt_secondary_out_spk_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.apt_primary_ref_out_spk_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_APT_PRIMARY_REF_OUT;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.apt_primary_ref_spk_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
        entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                      audio_route_cfg.apt_primary_ref_out_spk_endpoint_idx;
        entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                        audio_route_cfg.apt_primary_ref_out_spk_endpoint_type;
        entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk1_class;
        entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                          audio_route_cfg.apt_primary_ref_out_spk_dac_ch;
        entry->endpoint_attr.spk.ana_gain = audio_route_cfg.apt_primary_ref_spk_analog_gain;
        entry->endpoint_attr.spk.dig_gain = audio_route_cfg.apt_primary_ref_spk_digital_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.apt_primary_ref_out_spk_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
        entry->gateway_attr.channs = audio_route_cfg.apt_primary_ref_out_spk_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.apt_secondary_ref_out_spk_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_APT_SECONDARY_REF_OUT;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.apt_secondary_ref_spk_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
        entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                      audio_route_cfg.apt_secondary_ref_out_spk_endpoint_idx;
        entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                        audio_route_cfg.apt_secondary_ref_out_spk_endpoint_type;
        entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk2_class;
        entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                          audio_route_cfg.apt_secondary_ref_out_spk_dac_ch;
        entry->endpoint_attr.spk.ana_gain = audio_route_cfg.apt_primary_ref_spk_analog_gain;
        entry->endpoint_attr.spk.dig_gain = audio_route_cfg.apt_primary_ref_spk_digital_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.apt_secondary_ref_out_spk_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
        entry->gateway_attr.channs = audio_route_cfg.apt_secondary_ref_out_spk_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.apt_front_left_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_APT_FRONT_LEFT_IN;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.apt_front_mic_left_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.apt_front_left_in_mic_adc_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.apt_front_left_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.apt_front_left_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.apt_front_left_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.apt_front_mic_left_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.apt_front_mic_left_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain =
            audio_route_cfg.apt_front_mic_left_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.apt_front_left_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.apt_front_left_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.apt_front_right_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_APT_FRONT_RIGHT_IN;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.apt_front_mic_right_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.apt_front_right_in_mic_adc_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.apt_front_right_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.apt_front_right_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.apt_front_right_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.apt_front_mic_right_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.apt_front_mic_right_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain =
            audio_route_cfg.apt_front_mic_right_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.apt_front_right_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.apt_front_right_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.apt_ff_left_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_APT_FF_LEFT_IN;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.apt_ff_mic_left_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.apt_ff_left_in_mic_adc_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.apt_ff_left_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.apt_ff_left_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.apt_ff_left_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.apt_ff_mic_left_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.apt_ff_mic_left_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain =
            audio_route_cfg.apt_ff_mic_left_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.apt_ff_left_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.apt_ff_left_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.apt_ff_right_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_APT_FF_RIGHT_IN;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.apt_ff_mic_right_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.apt_ff_right_in_mic_adc_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.apt_ff_right_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.apt_ff_right_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.apt_ff_right_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.apt_ff_mic_right_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.apt_ff_mic_right_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain =
            audio_route_cfg.apt_ff_mic_right_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.apt_ff_right_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.apt_ff_right_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.apt_fb_left_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_APT_FB_LEFT_IN;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.apt_fb_mic_left_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.apt_fb_left_in_mic_adc_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.apt_fb_left_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.apt_fb_left_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.apt_fb_left_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.apt_fb_mic_left_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.apt_fb_mic_left_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain =
            audio_route_cfg.apt_fb_mic_left_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.apt_fb_left_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.apt_fb_left_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.apt_fb_right_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_APT_FB_RIGHT_IN;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.apt_fb_mic_right_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.apt_fb_right_in_mic_adc_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.apt_fb_right_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.apt_fb_right_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.apt_fb_right_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.apt_fb_mic_right_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.apt_fb_mic_right_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain =
            audio_route_cfg.apt_fb_mic_right_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.apt_fb_right_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.apt_fb_right_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.apt_bone_left_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_APT_BONE_LEFT_IN;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.apt_bone_mic_left_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.apt_bone_left_in_mic_adc_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.apt_bone_left_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.apt_bone_left_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.apt_bone_left_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.apt_bone_mic_left_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.apt_bone_mic_left_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain =
            audio_route_cfg.apt_bone_mic_left_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.apt_bone_left_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.apt_bone_left_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.apt_bone_right_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_APT_BONE_RIGHT_IN;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.apt_bone_mic_right_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.apt_bone_right_in_mic_adc_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.apt_bone_right_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.apt_bone_right_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.apt_bone_right_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.apt_bone_mic_right_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.apt_bone_mic_right_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain =
            audio_route_cfg.apt_bone_mic_right_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.apt_bone_right_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.apt_bone_right_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.apt_primary_ref_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_APT_PRIMARY_REF_IN;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.apt_primary_ref_mic_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.apt_primary_ref_in_mic_adc_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.apt_primary_ref_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.apt_primary_ref_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.apt_primary_ref_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.apt_primary_ref_mic_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.apt_primary_ref_mic_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain =
            audio_route_cfg.apt_primary_ref_mic_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.apt_primary_ref_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.apt_primary_ref_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.apt_secondary_ref_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_APT_SECONDARY_REF_IN;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.apt_secondary_ref_mic_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.apt_secondary_ref_in_mic_adc_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.apt_secondary_ref_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.apt_secondary_ref_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.apt_secondary_ref_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.apt_primary_ref_mic_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.apt_primary_ref_mic_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain =
            audio_route_cfg.apt_primary_ref_mic_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.apt_secondary_ref_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.apt_secondary_ref_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
    }
}
#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
static uint8_t audio_route_anc_entry_num_get(uint32_t device)
{
    uint8_t entry_num = 0;

    entry_num += audio_route_cfg.anc_primary_out_spk_enable;
    entry_num += audio_route_cfg.anc_secondary_out_spk_enable;
    entry_num += audio_route_cfg.anc_primary_ref_out_spk_enable;
    entry_num += audio_route_cfg.anc_secondary_ref_out_spk_enable;
    entry_num += audio_route_cfg.anc_fb_left_in_mic_enable;
    entry_num += audio_route_cfg.anc_fb_right_in_mic_enable;
    entry_num += audio_route_cfg.anc_ff_left_in_mic_enable;
    entry_num += audio_route_cfg.anc_ff_right_in_mic_enable;
    entry_num += audio_route_cfg.anc_primary_ref_in_mic_enable;
    entry_num += audio_route_cfg.anc_secondary_ref_in_mic_enable;

    return entry_num;
}

static void audio_route_anc_entry_get(T_AUDIO_ROUTE_ENTRY *entry,
                                      uint32_t             device)
{
    if (audio_route_cfg.anc_primary_out_spk_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_ANC_PRIMARY_OUT;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.anc_primary_spk_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
        entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                          audio_route_cfg.anc_primary_out_spk_dac_ch;
        entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                      audio_route_cfg.anc_primary_out_spk_endpoint_idx;
        entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                        audio_route_cfg.anc_primary_out_spk_endpoint_type;
        entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk1_class;
        entry->endpoint_attr.spk.ana_gain = audio_route_cfg.anc_primary_spk_analog_gain;
        entry->endpoint_attr.spk.dig_gain = audio_route_cfg.anc_primary_spk_digital_gain;
        entry->endpoint_attr.spk.mix_en = (T_AUDIO_ROUTE_DAC_MIX_POINT)
                                          audio_route_cfg.dac_0_mix_point;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.anc_primary_out_spk_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
        entry->gateway_attr.channs = audio_route_cfg.anc_primary_out_spk_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.anc_secondary_out_spk_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_ANC_SECONDARY_OUT;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.anc_secondary_spk_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
        entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                          audio_route_cfg.anc_secondary_out_spk_dac_ch;
        entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                      audio_route_cfg.anc_secondary_out_spk_endpoint_idx;
        entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                        audio_route_cfg.anc_secondary_out_spk_endpoint_type;
        entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk2_class;
        entry->endpoint_attr.spk.ana_gain = audio_route_cfg.anc_secondary_spk_analog_gain;
        entry->endpoint_attr.spk.dig_gain = audio_route_cfg.anc_secondary_spk_digital_gain;
        entry->endpoint_attr.spk.mix_en = (T_AUDIO_ROUTE_DAC_MIX_POINT)
                                          audio_route_cfg.dac_1_mix_point;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.anc_secondary_out_spk_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
        entry->gateway_attr.channs = audio_route_cfg.anc_secondary_out_spk_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.anc_primary_ref_out_spk_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_ANC_PRIMARY_REF_OUT;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.anc_primary_ref_spk_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
        entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                      audio_route_cfg.anc_primary_ref_out_spk_endpoint_idx;
        entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                        audio_route_cfg.anc_primary_ref_out_spk_endpoint_type;
        entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk1_class;
        entry->endpoint_attr.spk.ana_gain = audio_route_cfg.anc_primary_ref_spk_analog_gain;
        entry->endpoint_attr.spk.dig_gain = audio_route_cfg.anc_primary_ref_spk_digital_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.anc_primary_ref_out_spk_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
        entry->gateway_attr.channs = audio_route_cfg.anc_primary_ref_out_spk_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.anc_secondary_ref_out_spk_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_ANC_SECONDARY_REF_OUT;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.anc_secondary_ref_spk_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
        entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                      audio_route_cfg.anc_secondary_ref_out_spk_endpoint_idx;
        entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                        audio_route_cfg.anc_secondary_ref_out_spk_endpoint_type;
        entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk2_class;
        entry->endpoint_attr.spk.ana_gain = audio_route_cfg.anc_primary_ref_spk_analog_gain;
        entry->endpoint_attr.spk.dig_gain = audio_route_cfg.anc_primary_ref_spk_digital_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.anc_secondary_ref_out_spk_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
        entry->gateway_attr.channs = audio_route_cfg.anc_secondary_ref_out_spk_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.anc_fb_left_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_ANC_FB_LEFT_IN;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.anc_fb_mic_left_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.anc_fb_left_in_mic_anc_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.anc_fb_left_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.anc_fb_left_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.anc_fb_left_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.anc_fb_mic_left_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.anc_fb_mic_left_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain = audio_route_cfg.anc_fb_mic_left_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.anc_fb_left_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.anc_fb_left_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.anc_fb_right_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_ANC_FB_RIGHT_IN;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.anc_fb_mic_right_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.anc_fb_right_in_mic_anc_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.anc_fb_right_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.anc_fb_right_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.anc_fb_right_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.anc_fb_mic_right_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.anc_fb_mic_right_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain = audio_route_cfg.anc_fb_mic_right_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.anc_fb_right_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.anc_fb_right_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.anc_ff_left_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_ANC_FF_LEFT_IN;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.anc_ff_mic_left_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.anc_ff_left_in_mic_anc_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.anc_ff_left_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.anc_ff_left_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.anc_ff_left_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.anc_ff_mic_left_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.anc_ff_mic_left_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain = audio_route_cfg.anc_ff_mic_left_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.anc_ff_left_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.anc_ff_left_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.anc_ff_right_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_ANC_FF_RIGHT_IN;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.anc_ff_mic_right_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.anc_ff_right_in_mic_anc_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.anc_ff_right_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.anc_ff_right_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.anc_ff_right_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.anc_ff_mic_right_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.anc_ff_mic_right_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain = audio_route_cfg.anc_ff_mic_right_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.anc_ff_right_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.anc_ff_right_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.anc_primary_ref_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_ANC_PRIMARY_REF_IN;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.anc_primary_ref_mic_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.anc_primary_ref_in_mic_adc_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.anc_primary_ref_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.anc_primary_ref_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.anc_primary_ref_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.anc_primary_ref_mic_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.anc_primary_ref_mic_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain =
            audio_route_cfg.anc_primary_ref_mic_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.anc_primary_ref_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.anc_primary_ref_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.anc_secondary_ref_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_ANC_SECONDARY_REF_IN;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.anc_secondary_ref_mic_polarity;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.anc_secondary_ref_in_mic_adc_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.anc_secondary_ref_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.anc_secondary_ref_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.anc_secondary_ref_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.anc_primary_ref_mic_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.anc_primary_ref_mic_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain =
            audio_route_cfg.anc_primary_ref_mic_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.anc_secondary_ref_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.anc_secondary_ref_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
    }
}
#endif

static uint8_t audio_route_vad_entry_num_get(uint32_t device)
{
    uint8_t entry_num = 0;

    if (device & AUDIO_DEVICE_IN_MIC)
    {
        entry_num += audio_route_cfg.vad_primary_in_mic_enable;
        entry_num += audio_route_cfg.vad_primary_ref_in_mic_enable;
        entry_num += audio_route_cfg.vad_secondary_ref_in_mic_enable;
    }

    return entry_num;
}

static void audio_route_vad_entry_get(T_AUDIO_ROUTE_ENTRY *entry,
                                      uint32_t             device)
{
    if (device & AUDIO_DEVICE_IN_MIC)
    {
        if (audio_route_cfg.vad_primary_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VAD_PRIMARY_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.vad_primary_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.vad_primary_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.vad_primary_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.vad_primary_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.vad_primary_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.vad_primary_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.vad_primary_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain =
                audio_route_cfg.vad_primary_mic_digital_boost_gain;
            entry->gateway_attr.type = (T_AUDIO_ROUTE_GATEWAY_TYPE)
                                       audio_route_cfg.vad_primary_in_mic_gateway_type;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.vad_primary_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.vad_primary_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.vad_secondary_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VAD_SECONDARY_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.vad_secondary_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.vad_secondary_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.vad_secondary_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.vad_secondary_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.vad_secondary_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.vad_secondary_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.vad_secondary_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain =
                audio_route_cfg.vad_secondary_mic_digital_boost_gain;
            entry->gateway_attr.type = (T_AUDIO_ROUTE_GATEWAY_TYPE)
                                       audio_route_cfg.vad_secondary_in_mic_gateway_type;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.vad_secondary_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.vad_secondary_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.vad_primary_ref_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VAD_PRIMARY_REF_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.vad_primary_ref_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.vad_primary_ref_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.vad_primary_ref_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.vad_primary_ref_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.vad_primary_ref_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.vad_primary_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.vad_primary_mic_digital_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.vad_primary_ref_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.vad_primary_ref_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.vad_secondary_ref_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_VAD_SECONDARY_REF_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.vad_secondary_ref_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.vad_secondary_ref_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.vad_secondary_ref_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.vad_secondary_ref_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.vad_secondary_ref_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.vad_primary_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.vad_primary_mic_digital_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.vad_primary_ref_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.vad_secondary_ref_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
        }
    }
}

static uint8_t audio_route_line_entry_num_get(uint32_t device)
{
    uint8_t entry_num = 0;

    if (device & AUDIO_DEVICE_OUT_SPK)
    {
        entry_num += audio_route_cfg.line_primary_out_spk_enable;
        entry_num += audio_route_cfg.line_secondary_out_spk_enable;
        entry_num += audio_route_cfg.line_primary_ref_out_spk_enable;
        entry_num += audio_route_cfg.line_secondary_ref_out_spk_enable;
    }

    if (device & AUDIO_DEVICE_OUT_SPDIF)
    {
        entry_num += audio_route_cfg.line_primary_out_spdif_enable;
        entry_num += audio_route_cfg.line_secondary_out_spdif_enable;
    }

    if (device & AUDIO_DEVICE_IN_AUX)
    {
        entry_num += audio_route_cfg.line_left_in_aux_enable;
        entry_num += audio_route_cfg.line_right_in_aux_enable;
        entry_num += audio_route_cfg.line_primary_ref_in_mic_enable;
        entry_num += audio_route_cfg.line_secondary_ref_in_mic_enable;
    }

    if (device & AUDIO_DEVICE_IN_MIC)
    {
        entry_num += audio_route_cfg.line_left_in_mic_enable;
        entry_num += audio_route_cfg.line_right_in_mic_enable;
        entry_num += audio_route_cfg.line_primary_ref_in_mic_enable;
        entry_num += audio_route_cfg.line_secondary_ref_in_mic_enable;
    }

    if (device & AUDIO_DEVICE_IN_SPDIF)
    {
        entry_num += audio_route_cfg.line_left_in_spdif_enable;
        entry_num += audio_route_cfg.line_right_in_spdif_enable;
    }

    return entry_num;
}

static void audio_route_line_entry_get(T_AUDIO_ROUTE_ENTRY *entry,
                                       uint32_t             device)
{
    if (device & AUDIO_DEVICE_OUT_SPK)
    {
        if (audio_route_cfg.line_primary_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_LINE_PRIMARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.line_primary_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                              audio_route_cfg.line_primary_out_spk_dac_ch;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.line_primary_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.line_primary_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk1_class;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.line_primary_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.line_primary_spk_digital_gain;
            entry->endpoint_attr.spk.mix_en = (T_AUDIO_ROUTE_DAC_MIX_POINT)
                                              audio_route_cfg.dac_0_mix_point;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.line_primary_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.line_primary_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.line_secondary_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_LINE_SECONDARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.line_secondary_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                              audio_route_cfg.line_secondary_out_spk_dac_ch;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.line_secondary_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.line_secondary_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk2_class;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.line_secondary_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.line_secondary_spk_digital_gain;
            entry->endpoint_attr.spk.mix_en = (T_AUDIO_ROUTE_DAC_MIX_POINT)
                                              audio_route_cfg.dac_1_mix_point;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.line_secondary_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.line_secondary_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.line_primary_ref_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_LINE_PRIMARY_REF_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.line_primary_ref_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.line_primary_ref_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.line_primary_ref_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk1_class;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.line_primary_ref_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.line_primary_ref_spk_digital_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.line_primary_ref_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.line_primary_ref_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.line_secondary_ref_out_spk_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_LINE_SECONDARY_REF_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.line_secondary_ref_spk_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
            entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                          audio_route_cfg.line_secondary_ref_out_spk_endpoint_idx;
            entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                            audio_route_cfg.line_secondary_ref_out_spk_endpoint_type;
            entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk2_class;
            entry->endpoint_attr.spk.ana_gain = audio_route_cfg.line_primary_ref_spk_analog_gain;
            entry->endpoint_attr.spk.dig_gain = audio_route_cfg.line_primary_ref_spk_digital_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.line_secondary_ref_out_spk_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            entry->gateway_attr.channs = audio_route_cfg.line_secondary_ref_out_spk_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }
    }

    if (device & AUDIO_DEVICE_OUT_SPDIF)
    {
        if (audio_route_cfg.line_primary_out_spdif_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_LINE_PRIMARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.line_primary_out_spdif_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPDIF;
            entry->endpoint_attr.spdif_out.id = (T_AUDIO_ROUTE_SPDIF_ID)
                                                audio_route_cfg.line_primary_out_spdif_endpoint_idx;
            entry->endpoint_attr.spdif_out.type = (T_AUDIO_ROUTE_SPDIF_TYPE)
                                                  audio_route_cfg.line_primary_out_spdif_endpoint_type;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPDIF;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.line_primary_out_spdif_gateway_idx;
            entry->gateway_attr.channs = audio_route_cfg.line_primary_out_spdif_gateway_ch;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.line_secondary_out_spdif_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_LINE_SECONDARY_OUT;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.line_secondary_out_spdif_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPDIF;
            entry->endpoint_attr.spdif_out.id = (T_AUDIO_ROUTE_SPDIF_ID)
                                                audio_route_cfg.line_secondary_out_spdif_enable;
            entry->endpoint_attr.spdif_out.type = (T_AUDIO_ROUTE_SPDIF_TYPE)
                                                  audio_route_cfg.line_secondary_out_spdif_endpoint_type;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPDIF;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.line_secondary_out_spdif_gateway_idx;
            entry->gateway_attr.channs = audio_route_cfg.line_secondary_out_spdif_gateway_ch;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }
    }

    if (device & AUDIO_DEVICE_IN_AUX)
    {
        if (audio_route_cfg.line_left_in_aux_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_LINE_LEFT_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.line_aux_left_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_AUX;
            entry->endpoint_attr.aux_in.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                                 audio_route_cfg.line_left_in_aux_adc_ch;
            entry->endpoint_attr.aux_in.id = (T_AUDIO_ROUTE_AUX_ID)
                                             (audio_route_cfg.line_left_in_aux_endpoint_idx_l +
                                              (audio_route_cfg.line_left_in_aux_endpoint_idx_h << 3));
            entry->endpoint_attr.aux_in.class = audio_route_aux_class_get(
                                                    entry->endpoint_attr.aux_in.id);
            entry->endpoint_attr.aux_in.equalizer_en = true;
            entry->endpoint_attr.aux_in.ana_gain = audio_route_cfg.line_aux_left_analog_gain;
            entry->endpoint_attr.aux_in.dig_gain = audio_route_cfg.line_aux_left_digital_gain;
            entry->endpoint_attr.aux_in.dig_boost_gain = audio_route_cfg.line_aux_left_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.line_left_in_aux_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.line_left_in_aux_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.line_right_in_aux_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_LINE_RIGHT_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.line_aux_right_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_AUX;
            entry->endpoint_attr.aux_in.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                                 audio_route_cfg.line_right_in_aux_adc_ch;
            entry->endpoint_attr.aux_in.id = (T_AUDIO_ROUTE_AUX_ID)
                                             (audio_route_cfg.line_right_in_aux_endpoint_idx_l +
                                              (audio_route_cfg.line_right_in_aux_endpoint_idx_h << 3));
            entry->endpoint_attr.aux_in.class = audio_route_aux_class_get(
                                                    entry->endpoint_attr.aux_in.id);
            entry->endpoint_attr.aux_in.equalizer_en = true;
            entry->endpoint_attr.aux_in.ana_gain = audio_route_cfg.line_aux_right_analog_gain;
            entry->endpoint_attr.aux_in.dig_gain = audio_route_cfg.line_aux_right_digital_gain;
            entry->endpoint_attr.aux_in.dig_boost_gain = audio_route_cfg.line_aux_right_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.line_right_in_aux_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.line_right_in_aux_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.line_primary_ref_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_LINE_PRIMARY_REF_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.line_primary_ref_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.line_primary_ref_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.line_primary_ref_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.line_primary_ref_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.line_primary_ref_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.line_primary_ref_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.line_primary_ref_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain = audio_route_cfg.line_primary_ref_mic_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.line_primary_ref_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.line_primary_ref_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.line_secondary_ref_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_LINE_SECONDARY_REF_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.line_secondary_ref_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.line_secondary_ref_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.line_secondary_ref_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.line_secondary_ref_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.line_secondary_ref_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.line_primary_ref_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.line_primary_ref_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain = audio_route_cfg.line_primary_ref_mic_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.line_secondary_ref_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.line_secondary_ref_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }
    }

    if (device & AUDIO_DEVICE_IN_MIC)
    {
        if (audio_route_cfg.line_left_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_LINE_LEFT_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.line_mic_left_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.line_left_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.line_left_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.line_left_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.line_left_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.record_primary_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.record_primary_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain =
                audio_route_cfg.record_primary_mic_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.line_left_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.line_left_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.line_right_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_LINE_RIGHT_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.line_mic_right_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.line_right_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.line_right_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.line_right_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.line_right_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.record_secondary_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.record_secondary_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain =
                audio_route_cfg.record_secondary_mic_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.line_right_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.line_right_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.line_primary_ref_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_LINE_PRIMARY_REF_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.line_primary_ref_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.line_primary_ref_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.line_primary_ref_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.line_primary_ref_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.line_primary_ref_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.line_primary_ref_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.line_primary_ref_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain = audio_route_cfg.line_primary_ref_mic_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.line_primary_ref_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.line_primary_ref_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.line_secondary_ref_in_mic_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_LINE_SECONDARY_REF_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.line_secondary_ref_mic_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
            entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                              audio_route_cfg.line_secondary_ref_in_mic_adc_ch;
            entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                          (audio_route_cfg.line_secondary_ref_in_mic_endpoint_idx_l +
                                           (audio_route_cfg.line_secondary_ref_in_mic_endpoint_idx_h << 3));
            entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                            audio_route_cfg.line_secondary_ref_in_mic_endpoint_type;
            entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
            entry->endpoint_attr.mic.ana_gain = audio_route_cfg.line_primary_ref_mic_analog_gain;
            entry->endpoint_attr.mic.dig_gain = audio_route_cfg.line_primary_ref_mic_digital_gain;
            entry->endpoint_attr.mic.dig_boost_gain = audio_route_cfg.line_primary_ref_mic_digital_boost_gain;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.line_secondary_ref_in_mic_gateway_idx;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            entry->gateway_attr.channs = audio_route_cfg.line_secondary_ref_in_mic_gateway_ch;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }
    }

    if (device & AUDIO_DEVICE_IN_SPDIF)
    {
        if (audio_route_cfg.line_left_in_spdif_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_LINE_LEFT_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.line_left_in_spdif_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPDIF;
            entry->endpoint_attr.spdif_in.id = (T_AUDIO_ROUTE_SPDIF_ID)
                                               audio_route_cfg.line_left_in_spdif_endpoint_idx;
            entry->endpoint_attr.spdif_in.type = (T_AUDIO_ROUTE_SPDIF_TYPE)
                                                 audio_route_cfg.line_left_in_spdif_endpoint_type;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPDIF;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.line_left_in_spdif_gateway_idx;
            entry->gateway_attr.channs = audio_route_cfg.line_left_in_spdif_gateway_ch;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
            entry++;
        }

        if (audio_route_cfg.line_right_in_spdif_enable)
        {
            entry->io_type = AUDIO_ROUTE_IO_LINE_RIGHT_IN;
            entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                                 audio_route_cfg.line_right_in_spdif_polarity;
            entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPDIF;
            entry->endpoint_attr.spdif_in.id = (T_AUDIO_ROUTE_SPDIF_ID)
                                               audio_route_cfg.line_right_in_spdif_enable;
            entry->endpoint_attr.spdif_in.type = (T_AUDIO_ROUTE_SPDIF_TYPE)
                                                 audio_route_cfg.line_right_in_spdif_endpoint_type;
            entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPDIF;
            entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                      audio_route_cfg.line_right_in_spdif_gateway_idx;
            entry->gateway_attr.channs = audio_route_cfg.line_right_in_spdif_gateway_ch;
            entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
            audio_route_gateway_db_get(entry->gateway_attr.type,
                                       entry->gateway_attr.idx,
                                       entry->gateway_attr.dir,
                                       &entry->gateway_attr);
        }
    }
}

static uint8_t audio_route_llapt_entry_num_get(uint32_t device)
{
    uint8_t entry_num = 0;

    entry_num += audio_route_cfg.llapt_primary_out_spk_enable;
    entry_num += audio_route_cfg.llapt_secondary_out_spk_enable;
    entry_num += audio_route_cfg.llapt_primary_ref_out_spk_enable;
    entry_num += audio_route_cfg.llapt_secondary_ref_out_spk_enable;
    entry_num += audio_route_cfg.llapt_left_in_mic_enable;
    entry_num += audio_route_cfg.llapt_right_in_mic_enable;
    entry_num += audio_route_cfg.llapt_primary_ref_in_mic_enable;
    entry_num += audio_route_cfg.llapt_secondary_ref_in_mic_enable;

    return entry_num;
}

static void audio_route_llapt_entry_get(T_AUDIO_ROUTE_ENTRY *entry,
                                        uint32_t             device)
{
    if (audio_route_cfg.llapt_primary_out_spk_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_LLAPT_PRIMARY_OUT;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.llapt_primary_spk_polarity;
        entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                          audio_route_cfg.llapt_primary_out_spk_dac_ch;
        entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                      audio_route_cfg.llapt_primary_out_spk_endpoint_idx;
        entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                        audio_route_cfg.llapt_primary_out_spk_endpoint_type;
        entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk1_class;
        entry->endpoint_attr.spk.ana_gain = audio_route_cfg.llapt_primary_spk_analog_gain;
        entry->endpoint_attr.spk.dig_gain = audio_route_cfg.llapt_primary_spk_digital_gain;
        entry->endpoint_attr.spk.mix_en = (T_AUDIO_ROUTE_DAC_MIX_POINT)
                                          audio_route_cfg.dac_0_mix_point;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.llapt_primary_out_spk_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
        entry->gateway_attr.channs = audio_route_cfg.llapt_primary_out_spk_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.llapt_secondary_out_spk_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_LLAPT_SECONDARY_OUT;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.llapt_secondary_spk_polarity;
        entry->endpoint_attr.spk.dac_ch = (T_AUDIO_ROUTE_DAC_CHANN)
                                          audio_route_cfg.llapt_secondary_out_spk_dac_ch;
        entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                      audio_route_cfg.llapt_secondary_out_spk_endpoint_idx;
        entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                        audio_route_cfg.llapt_secondary_out_spk_endpoint_type;
        entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk2_class;
        entry->endpoint_attr.spk.ana_gain = audio_route_cfg.llapt_secondary_spk_analog_gain;
        entry->endpoint_attr.spk.dig_gain = audio_route_cfg.llapt_secondary_spk_digital_gain;
        entry->endpoint_attr.spk.mix_en = (T_AUDIO_ROUTE_DAC_MIX_POINT)
                                          audio_route_cfg.dac_1_mix_point;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.llapt_secondary_out_spk_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
        entry->gateway_attr.channs = audio_route_cfg.llapt_secondary_out_spk_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.llapt_primary_ref_out_spk_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_LLAPT_PRIMARY_REF_OUT;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.llapt_primary_ref_spk_polarity;
        entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                      audio_route_cfg.llapt_primary_ref_out_spk_endpoint_idx;
        entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                        audio_route_cfg.llapt_primary_ref_out_spk_endpoint_type;
        entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk1_class;
        entry->endpoint_attr.spk.ana_gain = audio_route_cfg.llapt_primary_ref_spk_analog_gain;
        entry->endpoint_attr.spk.dig_gain = audio_route_cfg.llapt_primary_ref_spk_digital_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.llapt_primary_ref_out_spk_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
        entry->gateway_attr.channs = audio_route_cfg.llapt_primary_ref_out_spk_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.llapt_secondary_ref_out_spk_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_LLAPT_SECONDARY_REF_OUT;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_SPK;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.llapt_secondary_ref_spk_polarity;
        entry->endpoint_attr.spk.id = (T_AUDIO_ROUTE_SPK_ID)
                                      audio_route_cfg.llapt_secondary_ref_out_spk_endpoint_idx;
        entry->endpoint_attr.spk.type = (T_AUDIO_ROUTE_SPK_TYPE)
                                        audio_route_cfg.llapt_secondary_ref_out_spk_endpoint_type;
        entry->endpoint_attr.spk.class = (T_AUDIO_ROUTE_SPK_CLASS)audio_route_cfg.spk2_class;
        entry->endpoint_attr.spk.ana_gain = audio_route_cfg.llapt_primary_ref_spk_analog_gain;
        entry->endpoint_attr.spk.dig_gain = audio_route_cfg.llapt_primary_ref_spk_digital_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.llapt_secondary_ref_out_spk_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_TX;
        entry->gateway_attr.channs = audio_route_cfg.llapt_secondary_ref_out_spk_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.llapt_left_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_LLAPT_LEFT_IN;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.llapt_mic_left_polarity;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.llapt_left_in_mic_llapt_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.llapt_left_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.llapt_left_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.llapt_left_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.llapt_mic_left_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.llapt_mic_left_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain = audio_route_cfg.llapt_mic_left_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.llapt_left_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.llapt_left_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.llapt_right_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_LLAPT_RIGHT_IN;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)audio_route_cfg.llapt_mic_right_polarity;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.llapt_right_in_mic_llapt_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.llapt_right_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.llapt_right_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.llapt_right_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.llapt_mic_right_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.llapt_mic_right_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain = audio_route_cfg.llapt_mic_right_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.llapt_right_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.llapt_right_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.llapt_primary_ref_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_LLAPT_PRIMARY_REF_IN;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.llapt_primary_ref_mic_polarity;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.llapt_primary_ref_in_mic_adc_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.llapt_primary_ref_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.llapt_primary_ref_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.llapt_primary_ref_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.llapt_primary_ref_mic_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.llapt_primary_ref_mic_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain =
            audio_route_cfg.llapt_primary_ref_mic_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.llapt_primary_ref_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.llapt_primary_ref_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
        entry++;
    }

    if (audio_route_cfg.llapt_secondary_ref_in_mic_enable)
    {
        entry->io_type = AUDIO_ROUTE_IO_LLAPT_SECONDARY_REF_IN;
        entry->endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;
        entry->io_polarity = (T_AUDIO_ROUTE_IO_POLARITY)
                             audio_route_cfg.llapt_secondary_ref_mic_polarity;
        entry->endpoint_attr.mic.adc_ch = (T_AUDIO_ROUTE_ADC_CHANN)
                                          audio_route_cfg.llapt_secondary_ref_in_mic_adc_ch;
        entry->endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)
                                      (audio_route_cfg.llapt_secondary_ref_in_mic_endpoint_idx_l +
                                       (audio_route_cfg.llapt_secondary_ref_in_mic_endpoint_idx_h << 3));
        entry->endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)
                                        audio_route_cfg.llapt_secondary_ref_in_mic_endpoint_type;
        entry->endpoint_attr.mic.class = audio_route_mic_class_get(entry->endpoint_attr.mic.id);
        entry->endpoint_attr.mic.ana_gain = audio_route_cfg.llapt_primary_ref_mic_analog_gain;
        entry->endpoint_attr.mic.dig_gain = audio_route_cfg.llapt_primary_ref_mic_digital_gain;
        entry->endpoint_attr.mic.dig_boost_gain =
            audio_route_cfg.llapt_primary_ref_mic_digital_boost_gain;
        entry->gateway_attr.type = AUDIO_ROUTE_GATEWAY_SPORT;
        entry->gateway_attr.idx = (T_AUDIO_ROUTE_GATEWAY_IDX)
                                  audio_route_cfg.llapt_secondary_ref_in_mic_gateway_idx;
        entry->gateway_attr.dir = AUDIO_ROUTE_GATEWAY_DIR_RX;
        entry->gateway_attr.channs = audio_route_cfg.llapt_secondary_ref_in_mic_gateway_ch;
        audio_route_gateway_db_get(entry->gateway_attr.type,
                                   entry->gateway_attr.idx,
                                   entry->gateway_attr.dir,
                                   &entry->gateway_attr);
    }
}

uint8_t audio_route_entry_num_get(T_AUDIO_CATEGORY category,
                                  uint32_t         device)
{
    uint8_t entry_num = 0;

    switch (category)
    {
    case AUDIO_CATEGORY_AUDIO:
        {
            entry_num = audio_route_audio_entry_num_get(device);
        }
        break;

    case AUDIO_CATEGORY_VOICE:
        {
            entry_num = audio_route_voice_entry_num_get(device);
        }
        break;

    case AUDIO_CATEGORY_RECORD:
        {
            entry_num = audio_route_record_entry_num_get(device);
        }
        break;

    case AUDIO_CATEGORY_TONE:
        {
            entry_num = audio_route_ringtone_entry_num_get(device);
        }
        break;

    case AUDIO_CATEGORY_VP:
        {
            entry_num = audio_route_vp_entry_num_get(device);
        }
        break;

    case AUDIO_CATEGORY_APT:
        {
            entry_num = audio_route_apt_entry_num_get(device);
        }
        break;
#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
    case AUDIO_CATEGORY_ANC:
        {
            entry_num = audio_route_anc_entry_num_get(device);
        }
        break;
#endif
    case AUDIO_CATEGORY_VAD:
        {
            entry_num = audio_route_vad_entry_num_get(device);
        }
        break;

    case AUDIO_CATEGORY_LINE:
        {
            entry_num = audio_route_line_entry_num_get(device);
        }
        break;

    case AUDIO_CATEGORY_LLAPT:
        {
            entry_num = audio_route_llapt_entry_num_get(device);
        }
        break;

    default:
        break;
    }

    return entry_num;
}

void audio_route_entry_get(T_AUDIO_CATEGORY     category,
                           T_AUDIO_ROUTE_ENTRY *entry,
                           uint32_t             device)
{
    switch (category)
    {
    case AUDIO_CATEGORY_AUDIO:
        {
            audio_route_audio_entry_get(entry, device);
        }
        break;

    case AUDIO_CATEGORY_VOICE:
        {
            audio_route_voice_entry_get(entry, device);
        }
        break;

    case AUDIO_CATEGORY_RECORD:
        {
            audio_route_record_entry_get(entry, device);
        }
        break;

    case AUDIO_CATEGORY_TONE:
        {
            audio_route_ringtone_entry_get(entry, device);
        }
        break;

    case AUDIO_CATEGORY_VP:
        {
            audio_route_vp_entry_get(entry, device);
        }
        break;

    case AUDIO_CATEGORY_APT:
        {
            audio_route_apt_entry_get(entry, device);
        }
        break;
#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
    case AUDIO_CATEGORY_ANC:
        {
            audio_route_anc_entry_get(entry, device);
        }
        break;
#endif
    case AUDIO_CATEGORY_VAD:
        {
            audio_route_vad_entry_get(entry, device);
        }
        break;

    case AUDIO_CATEGORY_LINE:
        {
            audio_route_line_entry_get(entry, device);
        }
        break;

    case AUDIO_CATEGORY_LLAPT:
        {
            audio_route_llapt_entry_get(entry, device);
        }
        break;

    default:
        break;
    }
}

T_AUDIO_ROUTE *audio_route_path_find(T_AUDIO_PATH_HANDLE handle)
{
    if (handle != NULL)
    {
        T_AUDIO_ROUTE *route;

        route = os_queue_peek(&audio_route_db->routes, 0);
        while (route != NULL)
        {
            if (route->owner == handle)
            {
                return route;
            }

            route = route->next;
        }
    }

    return NULL;
}

static bool audio_route_is_created(T_AUDIO_ROUTE_PATH *path)
{
    T_AUDIO_ROUTE *route;

    route = os_queue_peek(&audio_route_db->routes, 0);
    while (route != NULL)
    {
        if (route->entry == path->entry)
        {
            return true;
        }

        route = route->next;
    }

    return false;
}

T_AUDIO_ROUTE_PATH audio_route_path_take(T_AUDIO_CATEGORY category,
                                         uint32_t         device)
{
    T_AUDIO_ROUTE_PATH path;

    path.entry     = NULL;
    path.entry_num = audio_route_entry_num_get(category,
                                               device);
    if (path.entry_num != 0)
    {
        path.entry = calloc(path.entry_num,
                            sizeof(T_AUDIO_ROUTE_ENTRY));
        if (path.entry != NULL)
        {
            audio_route_entry_get(category,
                                  path.entry,
                                  device);
        }
    }

    return path;
}

T_AUDIO_ROUTE_PATH audio_route_path_get(T_AUDIO_PATH_HANDLE handle,
                                        T_AUDIO_CATEGORY    category,
                                        uint32_t            device)
{
    T_AUDIO_ROUTE_PATH  path;
    T_AUDIO_ROUTE      *route;

    path.entry     = NULL;
    path.entry_num = 0;

    route = audio_route_path_find(handle);
    if (route != NULL)
    {
        path.entry_num = route->entry_num;
        path.entry     = route->entry;
    }
    else
    {
        path.entry_num = audio_route_entry_num_get(category,
                                                   device);
        if (path.entry_num != 0)
        {
            path.entry = calloc(path.entry_num,
                                sizeof(T_AUDIO_ROUTE_ENTRY));
            if (path.entry != NULL)
            {
                audio_route_entry_get(category,
                                      path.entry,
                                      device);
            }
        }
    }

    return path;
}

void audio_route_path_give(T_AUDIO_ROUTE_PATH *path)
{
    if (!audio_route_is_created(path))
    {
        if ((path->entry_num != 0) &&
            (path->entry != NULL))
        {
            free(path->entry);
        }
    }
}

bool audio_route_init(void)
{
    audio_route_db = calloc(1, sizeof(T_AUDIO_ROUTE_DB));
    if (audio_route_db != NULL)
    {
        os_queue_init(&audio_route_db->routes);
        sys_cfg_load();
        return true;
    }

    return false;
}

void audio_route_deinit(void)
{
    if (audio_route_db != NULL)
    {
        T_AUDIO_ROUTE *route;

        route = os_queue_out(&audio_route_db->routes);
        while (route != NULL)
        {
            free(route);
            route = os_queue_out(&audio_route_db->routes);
        }

        free(audio_route_db);
        audio_route_db = NULL;
    }
}

bool audio_route_gain_register(T_AUDIO_CATEGORY             category,
                               T_AUDIO_ROUTE_DAC_GAIN_CBACK dac_cback,
                               T_AUDIO_ROUTE_ADC_GAIN_CBACK adc_cback)
{
    if ((adc_cback != NULL) || (dac_cback != NULL))
    {
        if (adc_cback != NULL)
        {
            audio_route_db->adc_gain_cback[category] = adc_cback;
        }

        if (dac_cback != NULL)
        {
            audio_route_db->dac_gain_cback[category] = dac_cback;
        }

        return true;
    }

    return false;
}

void audio_route_gain_unregister(T_AUDIO_CATEGORY category)
{
    audio_route_db->adc_gain_cback[category] = NULL;
    audio_route_db->dac_gain_cback[category] = NULL;
}

void audio_route_device_set(T_AUDIO_CATEGORY category,
                            uint32_t         device)
{
    audio_route_db->device[category] = device;
}

uint32_t audio_route_device_get(T_AUDIO_CATEGORY category)
{
    return audio_route_db->device[category];
}

bool audio_route_adc_gain_get(T_AUDIO_CATEGORY        category,
                              uint32_t                level,
                              T_AUDIO_ROUTE_ADC_GAIN *gain)
{
    if (audio_route_db->adc_gain_cback[category] != NULL)
    {
        return audio_route_db->adc_gain_cback[category](category, level, gain);
    }

    return false;
}

bool audio_route_dac_gain_get(T_AUDIO_CATEGORY        category,
                              uint32_t                level,
                              T_AUDIO_ROUTE_DAC_GAIN *gain)
{
    if (audio_route_db->dac_gain_cback[category] != NULL)
    {
        return audio_route_db->dac_gain_cback[category](category, level, gain);
    }

    return false;
}

bool audio_route_hw_eq_register(T_AUDIO_ROUTE_HW_EQ_CBACK cback)
{
    if (cback != NULL)
    {
        if (audio_route_db->eq_cback != NULL)
        {
            audio_route_db->eq_cback = cback;
            return true;
        }
    }

    return false;
}

void audio_route_hw_eq_unregister(void)
{
    audio_route_db->eq_cback = NULL;
}

bool audio_route_hw_eq_apply(T_AUDIO_ROUTE_HW_EQ_TYPE  type,
                             uint8_t                   chann,
                             uint8_t                  *buf,
                             uint16_t                  len)
{
    return codec_mgr_hw_eq_apply(type, chann, buf, len);
}

bool audio_route_hw_eq_set(T_AUDIO_ROUTE_HW_EQ_TYPE type,
                           uint8_t                  chann,
                           uint32_t                 sample_rate)
{
    if (audio_route_db->eq_cback != NULL)
    {
        return audio_route_db->eq_cback(type, chann, sample_rate);
    }

    return false;
}

bool audio_route_path_add(T_AUDIO_PATH_HANDLE  handle,
                          T_AUDIO_ROUTE_ENTRY *entry)
{
    T_AUDIO_ROUTE *route;

    route = audio_route_path_find(handle);
    if (route != NULL)
    {
        if (route->entry_num < route->max_num)
        {
            uint8_t i;

            for (i = 0; i < route->entry_num; i++)
            {
                if (route->entry[i].io_type == entry->io_type)
                {
                    route->entry[i] = *entry;
                    return true;
                }
            }

            route->entry[route->entry_num] = *entry;
            route->entry_num++;
            return true;
        }
    }

    return false;
}

bool audio_route_path_bind(T_AUDIO_PATH_HANDLE handle,
                           T_AUDIO_ROUTE_PATH  path)
{
    T_AUDIO_ROUTE *route;
    uint8_t        i;
    int32_t        ret = 0;

    if (audio_route_db == NULL)
    {
        ret = 1;
        goto invalid_audio_route_db;
    }

    if (handle == NULL)
    {
        ret = 2;
        goto invalid_handle;
    }

    if (path.entry_num == 0)
    {
        ret = 3;
        goto invalid_entry_num;
    }

    route = calloc(1, sizeof(T_AUDIO_ROUTE) + sizeof(T_AUDIO_ROUTE_ENTRY) * path.entry_num);
    if (route == NULL)
    {
        ret = 4;
        goto fail_alloc_route;
    }

    route->owner     = handle;
    route->max_num   = path.entry_num;
    route->entry_num = 0;
    os_queue_in(&audio_route_db->routes, route);

    for (i = 0; i < path.entry_num; i++)
    {
        if (audio_route_path_add(route->owner,
                                 &path.entry[i]) == false)
        {
            ret = 5;
            goto fail_add_entry;
        }
    }

    return true;

fail_add_entry:
fail_alloc_route:
invalid_entry_num:
invalid_handle:
invalid_audio_route_db:
    AUDIO_PRINT_ERROR1("audio_route_path_bind: failed %d", -ret);
    return false;
}

bool audio_route_path_unbind(T_AUDIO_PATH_HANDLE handle)
{
    T_AUDIO_ROUTE *route;
    int32_t        ret = 0;

    if (audio_route_db == NULL)
    {
        ret = 1;
        goto invalid_audio_route_db;
    }

    route = audio_route_path_find(handle);
    if (os_queue_delete(&audio_route_db->routes, route) == false)
    {
        ret = 2;
        goto invalid_instance;
    }

    free(route);
    return true;

invalid_instance:
invalid_audio_route_db:
    AUDIO_PRINT_ERROR1("audio_route_path_unbind: failed %d", -ret);
    return false;
}

bool audio_route_path_remove(T_AUDIO_PATH_HANDLE   handle,
                             T_AUDIO_ROUTE_IO_TYPE io_type)
{
    T_AUDIO_ROUTE *route;

    route = audio_route_path_find(handle);
    if (route != NULL)
    {
        uint8_t i;
        uint8_t j;

        for (i = 0; i < route->entry_num; i++)
        {
            if (route->entry[i].io_type == io_type)
            {
                for (j = i; j < route->entry_num - 1; j++)
                {
                    route->entry[j] = route->entry[j + 1];
                }

                route->entry_num--;
                return true;
            }
        }
    }

    return false;
}

uint8_t audio_route_ramp_gain_scaling_get(T_AUDIO_CATEGORY category)
{
    int16_t ramp_gain;

    switch (category)
    {
    case AUDIO_CATEGORY_AUDIO:
        {
            ramp_gain = (int16_t)audio_route_cfg.audio_ramp_gain_scaling;
        }
        break;

    case AUDIO_CATEGORY_VOICE:
        {
            ramp_gain = (int16_t)audio_route_cfg.voice_ramp_gain_scaling;
        }
        break;

    case AUDIO_CATEGORY_LINE:
        {
            ramp_gain = (int16_t)audio_route_cfg.line_ramp_gain_scaling;
        }
        break;

    case AUDIO_CATEGORY_APT:
        {
            ramp_gain = (int16_t)audio_route_cfg.apt_ramp_gain_scaling;
        }
        break;

    default:
        {
            ramp_gain = (int16_t)audio_route_cfg.audio_ramp_gain_scaling;
        }
        break;
    }

    return ramp_gain;
}
