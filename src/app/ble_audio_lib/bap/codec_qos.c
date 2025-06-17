#include <string.h>
#include "codec_qos.h"
#include "bt_types.h"
#include "codec_def.h"
#include "trace.h"
#include "ascs_def.h"

#if (LE_AUDIO_ASCS_CLIENT_SUPPORT || LE_AUDIO_BASE_DATA_GENERATE || LE_AUDIO_ASCS_SUPPORT || LE_AUDIO_PACS_CLIENT_SUPPORT)
const T_CODEC_CFG lc3_codec_cfg[] =
{
    {0x1B, FRAME_DURATION_CFG_7_5_MS, SAMPLING_FREQUENCY_CFG_8K,    1, 26,  AUDIO_LOCATION_MONO},//CODEC_CFG_ITEM_8_1
    {0x1B, FRAME_DURATION_CFG_10_MS,  SAMPLING_FREQUENCY_CFG_8K,    1, 30,  AUDIO_LOCATION_MONO},//CODEC_CFG_ITEM_8_2
    {0x1B, FRAME_DURATION_CFG_7_5_MS, SAMPLING_FREQUENCY_CFG_16K,   1, 30,  AUDIO_LOCATION_MONO},//CODEC_CFG_ITEM_16_1
    {0x1B, FRAME_DURATION_CFG_10_MS,  SAMPLING_FREQUENCY_CFG_16K,   1, 40,  AUDIO_LOCATION_MONO},//CODEC_CFG_ITEM_16_2
    {0x1B, FRAME_DURATION_CFG_7_5_MS, SAMPLING_FREQUENCY_CFG_24K,   1, 45,  AUDIO_LOCATION_MONO},//CODEC_CFG_ITEM_24_1
    {0x1B, FRAME_DURATION_CFG_10_MS,  SAMPLING_FREQUENCY_CFG_24K,   1, 60,  AUDIO_LOCATION_MONO},//CODEC_CFG_ITEM_24_2
    {0x1B, FRAME_DURATION_CFG_7_5_MS, SAMPLING_FREQUENCY_CFG_32K,   1, 60,  AUDIO_LOCATION_MONO},//CODEC_CFG_ITEM_32_1
    {0x1B, FRAME_DURATION_CFG_10_MS,  SAMPLING_FREQUENCY_CFG_32K,   1, 80,  AUDIO_LOCATION_MONO},//CODEC_CFG_ITEM_32_2
    {0x1B, FRAME_DURATION_CFG_7_5_MS, SAMPLING_FREQUENCY_CFG_44_1K, 1, 97,  AUDIO_LOCATION_MONO},//CODEC_CFG_ITEM_441_1
    {0x1B, FRAME_DURATION_CFG_10_MS,  SAMPLING_FREQUENCY_CFG_44_1K, 1, 130, AUDIO_LOCATION_MONO},//CODEC_CFG_ITEM_441_2
    {0x1B, FRAME_DURATION_CFG_7_5_MS, SAMPLING_FREQUENCY_CFG_48K,   1, 75,  AUDIO_LOCATION_MONO},//CODEC_CFG_ITEM_48_1
    {0x1B, FRAME_DURATION_CFG_10_MS,  SAMPLING_FREQUENCY_CFG_48K,   1, 100, AUDIO_LOCATION_MONO},//CODEC_CFG_ITEM_48_2
    {0x1B, FRAME_DURATION_CFG_7_5_MS, SAMPLING_FREQUENCY_CFG_48K,   1, 90,  AUDIO_LOCATION_MONO},//CODEC_CFG_ITEM_48_3
    {0x1B, FRAME_DURATION_CFG_10_MS,  SAMPLING_FREQUENCY_CFG_48K,   1, 120, AUDIO_LOCATION_MONO},//CODEC_CFG_ITEM_48_4
    {0x1B, FRAME_DURATION_CFG_7_5_MS, SAMPLING_FREQUENCY_CFG_48K,   1, 117, AUDIO_LOCATION_MONO},//CODEC_CFG_ITEM_48_5
    {0x1B, FRAME_DURATION_CFG_10_MS,  SAMPLING_FREQUENCY_CFG_48K,   1, 155, AUDIO_LOCATION_MONO},//CODEC_CFG_ITEM_48_6
};
#endif

#if (LE_AUDIO_ASCS_CLIENT_SUPPORT || LE_AUDIO_ASCS_SUPPORT)
const T_QOS_CFG_PREFERRED lc3_cis_low_latency_qos[] =
{
    {7500,  AUDIO_UNFRAMED, 26,  2, 8,  40000},//CODEC_CFG_ITEM_8_1
    {10000, AUDIO_UNFRAMED, 30,  2, 10, 40000},//CODEC_CFG_ITEM_8_2
    {7500,  AUDIO_UNFRAMED, 30,  2, 8,  40000},//CODEC_CFG_ITEM_16_1
    {10000, AUDIO_UNFRAMED, 40,  2, 10, 40000},//CODEC_CFG_ITEM_16_2
    {7500,  AUDIO_UNFRAMED, 45,  2, 8,  40000},//CODEC_CFG_ITEM_24_1
    {10000, AUDIO_UNFRAMED, 60,  2, 10, 40000},//CODEC_CFG_ITEM_24_2
    {7500,  AUDIO_UNFRAMED, 60,  2, 8,  40000},//CODEC_CFG_ITEM_32_1
    {10000, AUDIO_UNFRAMED, 80,  2, 10, 40000},//CODEC_CFG_ITEM_32_2
    {8163,  AUDIO_FRAMED,   97,  5, 24, 40000},//CODEC_CFG_ITEM_441_1
    {10884, AUDIO_FRAMED,   130, 5, 31, 40000},//CODEC_CFG_ITEM_441_2
    {7500,  AUDIO_UNFRAMED, 75,  5, 15, 40000},//CODEC_CFG_ITEM_48_1
    {10000, AUDIO_UNFRAMED, 100, 5, 20, 40000},//CODEC_CFG_ITEM_48_2
    {7500,  AUDIO_UNFRAMED, 90,  5, 15, 40000},//CODEC_CFG_ITEM_48_3
    {10000, AUDIO_UNFRAMED, 120, 5, 20, 40000},//CODEC_CFG_ITEM_48_4
    {7500,  AUDIO_UNFRAMED, 117, 5, 15, 40000},//CODEC_CFG_ITEM_48_5
    {10000, AUDIO_UNFRAMED, 155, 5, 20, 40000},//CODEC_CFG_ITEM_48_6
};

const T_QOS_CFG_PREFERRED lc3_cis_high_reliability_qos[] =
{
    {7500,  AUDIO_UNFRAMED, 26,  13, 75, 40000},//CODEC_CFG_ITEM_8_1
    {10000, AUDIO_UNFRAMED, 30,  13, 95, 40000},//CODEC_CFG_ITEM_8_2
    {7500,  AUDIO_UNFRAMED, 30,  13, 75, 40000},//CODEC_CFG_ITEM_16_1
    {10000, AUDIO_UNFRAMED, 40,  13, 95, 40000},//CODEC_CFG_ITEM_16_2
    {7500,  AUDIO_UNFRAMED, 45,  13, 75, 40000},//CODEC_CFG_ITEM_24_1
    {10000, AUDIO_UNFRAMED, 60,  13, 95, 40000},//CODEC_CFG_ITEM_24_2
    {7500,  AUDIO_UNFRAMED, 60,  13, 75, 40000},//CODEC_CFG_ITEM_32_1
    {10000, AUDIO_UNFRAMED, 80,  13, 95, 40000},//CODEC_CFG_ITEM_32_2
    {8163,  AUDIO_FRAMED,   97,  13, 80, 40000},//CODEC_CFG_ITEM_441_1
    {10884, AUDIO_FRAMED,   130, 13, 85, 40000},//CODEC_CFG_ITEM_441_2
    {7500,  AUDIO_UNFRAMED, 75,  13, 75, 40000},//CODEC_CFG_ITEM_48_1
    {10000, AUDIO_UNFRAMED, 100, 13, 95, 40000},//CODEC_CFG_ITEM_48_2
    {7500,  AUDIO_UNFRAMED, 90,  13, 75, 40000},//CODEC_CFG_ITEM_48_3
    {10000, AUDIO_UNFRAMED, 120, 13, 100, 40000},//CODEC_CFG_ITEM_48_4
    {7500,  AUDIO_UNFRAMED, 117, 13, 75, 40000},//CODEC_CFG_ITEM_48_5
    {10000, AUDIO_UNFRAMED, 155, 13, 100, 40000},//CODEC_CFG_ITEM_48_6
};
#endif

#if LE_AUDIO_BASE_DATA_GENERATE
const T_QOS_CFG_PREFERRED lc3_bis_low_latency_qos[] =
{
    {7500,  AUDIO_UNFRAMED, 26,  2, 8,  40000},//CODEC_CFG_ITEM_8_1
    {10000, AUDIO_UNFRAMED, 30,  2, 10, 40000},//CODEC_CFG_ITEM_8_2
    {7500,  AUDIO_UNFRAMED, 30,  2, 8,  40000},//CODEC_CFG_ITEM_16_1
    {10000, AUDIO_UNFRAMED, 40,  2, 10, 40000},//CODEC_CFG_ITEM_16_2
    {7500,  AUDIO_UNFRAMED, 45,  2, 8,  40000},//CODEC_CFG_ITEM_24_1
    {10000, AUDIO_UNFRAMED, 60,  2, 10, 40000},//CODEC_CFG_ITEM_24_2
    {7500,  AUDIO_UNFRAMED, 60,  2, 8,  40000},//CODEC_CFG_ITEM_32_1
    {10000, AUDIO_UNFRAMED, 80,  2, 10, 40000},//CODEC_CFG_ITEM_32_2
    {8163,  AUDIO_FRAMED,   97,  4, 24, 40000},//CODEC_CFG_ITEM_441_1
    {10884, AUDIO_FRAMED,   130, 4, 31, 40000},//CODEC_CFG_ITEM_441_2
    {7500,  AUDIO_UNFRAMED, 75,  4, 15, 40000},//CODEC_CFG_ITEM_48_1
    {10000, AUDIO_UNFRAMED, 100, 4, 20, 40000},//CODEC_CFG_ITEM_48_2
    {7500,  AUDIO_UNFRAMED, 90,  4, 15, 40000},//CODEC_CFG_ITEM_48_3
    {10000, AUDIO_UNFRAMED, 120, 4, 20, 40000},//CODEC_CFG_ITEM_48_4
    {7500,  AUDIO_UNFRAMED, 117, 4, 15, 40000},//CODEC_CFG_ITEM_48_5
    {10000, AUDIO_UNFRAMED, 155, 4, 20, 40000},//CODEC_CFG_ITEM_48_6
};

const T_QOS_CFG_PREFERRED lc3_bis_high_reliability_qos[] =
{
    {7500,  AUDIO_UNFRAMED, 26,  4, 45, 40000},//CODEC_CFG_ITEM_8_1
    {10000, AUDIO_UNFRAMED, 30,  4, 60, 40000},//CODEC_CFG_ITEM_8_2
    {7500,  AUDIO_UNFRAMED, 30,  4, 45, 40000},//CODEC_CFG_ITEM_16_1
    {10000, AUDIO_UNFRAMED, 40,  4, 60, 40000},//CODEC_CFG_ITEM_16_2
    {7500,  AUDIO_UNFRAMED, 45,  4, 45, 40000},//CODEC_CFG_ITEM_24_1
    {10000, AUDIO_UNFRAMED, 60,  4, 60, 40000},//CODEC_CFG_ITEM_24_2
    {7500,  AUDIO_UNFRAMED, 60,  4, 45, 40000},//CODEC_CFG_ITEM_32_1
    {10000, AUDIO_UNFRAMED, 80,  4, 60, 40000},//CODEC_CFG_ITEM_32_2
    {8163,  AUDIO_FRAMED,   97,  4, 54, 40000},//CODEC_CFG_ITEM_441_1
    {10884, AUDIO_FRAMED,   130, 4, 60, 40000},//CODEC_CFG_ITEM_441_2
    {7500,  AUDIO_UNFRAMED, 75,  4, 50, 40000},//CODEC_CFG_ITEM_48_1
    {10000, AUDIO_UNFRAMED, 100, 4, 65, 40000},//CODEC_CFG_ITEM_48_2
    {7500,  AUDIO_UNFRAMED, 90,  4, 50, 40000},//CODEC_CFG_ITEM_48_3
    {10000, AUDIO_UNFRAMED, 120, 4, 65, 40000},//CODEC_CFG_ITEM_48_4
    {7500,  AUDIO_UNFRAMED, 117, 4, 50, 40000},//CODEC_CFG_ITEM_48_5
    {10000, AUDIO_UNFRAMED, 155, 4, 65, 40000},//CODEC_CFG_ITEM_48_6
};
#endif

#if (LE_AUDIO_ASCS_CLIENT_SUPPORT || LE_AUDIO_BASE_DATA_GENERATE || LE_AUDIO_ASCS_SUPPORT)
bool codec_preferred_cfg_get(T_CODEC_CFG_ITEM item, T_CODEC_CFG *p_cfg)
{
    if (p_cfg == NULL)
    {
        return false;
    }
    memcpy(p_cfg, &lc3_codec_cfg[item], sizeof(T_CODEC_CFG));
    return true;
}

bool qos_preferred_cfg_get(T_CODEC_CFG_ITEM item, T_QOS_CFG_TYPE type, T_QOS_CFG_PREFERRED *p_qos)
{
    const T_QOS_CFG_PREFERRED *p_qos_table;
    if (p_qos == NULL)
    {
        return false;
    }

    switch (type)
    {
#if (LE_AUDIO_ASCS_CLIENT_SUPPORT || LE_AUDIO_ASCS_SUPPORT)
    case QOS_CFG_CIS_LOW_LATENCY:
        {
            p_qos_table = lc3_cis_low_latency_qos;
        }
        break;

    case QOS_CFG_CIS_HIG_RELIABILITY:
        {
            p_qos_table = lc3_cis_high_reliability_qos;
        }
        break;
#endif

#if LE_AUDIO_BASE_DATA_GENERATE
    case QOS_CFG_BIS_LOW_LATENCY:
        {
            p_qos_table = lc3_bis_low_latency_qos;
        }
        break;

    case QOS_CFG_BIS_HIG_RELIABILITY:
        {
            p_qos_table = lc3_bis_high_reliability_qos;
        }
        break;
#endif
    default:
        return false;
    }

    memcpy(p_qos, &p_qos_table[item], sizeof(T_QOS_CFG_PREFERRED));
    return true;
}
#endif

#if LE_AUDIO_ASCS_SUPPORT
bool qos_preferred_cfg_get_by_codec(T_CODEC_CFG *p_cfg, uint8_t target_latency,
                                    T_QOS_CFG_PREFERRED *p_qos)
{
    uint8_t i = 0;
    T_CODEC_CFG *p_temp_cfg;
    if (p_cfg == NULL || p_qos == NULL)
    {
        return false;
    }
    for (; i <= CODEC_CFG_ITEM_48_6; i++)
    {
        p_temp_cfg = (T_CODEC_CFG *)&lc3_codec_cfg[i];
        if (p_temp_cfg->frame_duration == p_cfg->frame_duration &&
            p_temp_cfg->sample_frequency == p_cfg->sample_frequency &&
            p_temp_cfg->octets_per_codec_frame == p_cfg->octets_per_codec_frame)
        {
            if (target_latency == ASE_TARGET_HIGHER_RELIABILITY)
            {
                memcpy(p_qos, &lc3_cis_high_reliability_qos[i], sizeof(T_QOS_CFG_PREFERRED));
            }
            else
            {
                memcpy(p_qos, &lc3_cis_low_latency_qos[i], sizeof(T_QOS_CFG_PREFERRED));
            }
            return true;
        }
    }
    return true;
}
#endif

#if (LE_AUDIO_ASCS_CLIENT_SUPPORT || LE_AUDIO_BASE_DATA_GENERATE || LE_AUDIO_ASCS_SUPPORT)
bool codec_max_sdu_len_get(T_CODEC_CFG *p_cfg, uint16_t *p_max_len)
{
    uint8_t blocks_num = 1;
    uint8_t chnl_num = 1;
    if (p_max_len == NULL || p_cfg == NULL)
    {
        return false;
    }

    if (!(p_cfg->type_exist & CODEC_CFG_OCTET_PER_CODEC_FRAME_EXIST))
    {
        return false;
    }

    if (p_cfg->type_exist & CODEC_CFG_TYPE_BLOCKS_PER_SDU_EXIST)
    {
        blocks_num = p_cfg->codec_frame_blocks_per_sdu;
    }
    if (p_cfg->type_exist & CODEC_CFG_AUDIO_CHANNEL_ALLOCATION_EXIST)
    {
        chnl_num = p_cfg->audio_channel_allocation == AUDIO_LOCATION_MONO ?
                   1 : __builtin_popcount(p_cfg->audio_channel_allocation);
    }

    *p_max_len = blocks_num * chnl_num * p_cfg->octets_per_codec_frame;
    return true;
}

bool codec_sdu_interval_get(T_CODEC_CFG *p_cfg, uint32_t *p_sdu_int)
{
    uint8_t blocks_num = 1;
    uint32_t frame_duration = 10000;
    if (p_sdu_int == NULL || p_cfg == NULL)
    {
        return false;
    }

    if (!(p_cfg->type_exist & CODEC_CFG_FRAME_DURATION_EXIST))
    {
        return false;
    }

    if (p_cfg->type_exist & CODEC_CFG_TYPE_BLOCKS_PER_SDU_EXIST)
    {
        blocks_num = p_cfg->codec_frame_blocks_per_sdu;
    }
    if (p_cfg->frame_duration == FRAME_DURATION_CFG_7_5_MS)
    {
        frame_duration = 7500;
    }
    else if (p_cfg->frame_duration == FRAME_DURATION_CFG_10_MS)
    {
        frame_duration = 10000;
    }
    else if (p_cfg->frame_duration == FRAME_DURATION_CFG_5_MS)
    {
        frame_duration = 5000;
    }
    else if (p_cfg->frame_duration == FRAME_DURATION_CFG_2_5_MS)
    {
        frame_duration = 2500;
    }
    else
    {
        return false;
    }

    *p_sdu_int = blocks_num * frame_duration;
    return true;
}
#endif

#if LE_AUDIO_PACS_CLIENT_SUPPORT
bool codec_cap_get_cfg_bits(uint32_t *p_cfg_bits, T_CODEC_CAP *p_cap)
{
    uint32_t sample_sup_idx = 0;
    uint32_t frame_duration_idx = 0;
    uint32_t mask = 0;
    uint32_t supported_bits = 0;
    uint16_t index = 0;
    if (p_cfg_bits == NULL || p_cap == NULL)
    {
        return false;
    }
    *p_cfg_bits = 0;
    if (p_cap->type_exist & CODEC_CAP_SUPPORTED_SAMPLING_FREQUENCIES_EXIST)
    {
        if (p_cap->supported_sampling_frequencies & SAMPLING_FREQUENCY_8K)
        {
            sample_sup_idx |= SAMPLE_FREQ_8K_TABLE_MASK;
        }
        if (p_cap->supported_sampling_frequencies & SAMPLING_FREQUENCY_16K)
        {
            sample_sup_idx |= SAMPLE_FREQ_16K_TABLE_MASK;
        }
        if (p_cap->supported_sampling_frequencies & SAMPLING_FREQUENCY_24K)
        {
            sample_sup_idx |= SAMPLE_FREQ_24K_TABLE_MASK;
        }
        if (p_cap->supported_sampling_frequencies & SAMPLING_FREQUENCY_32K)
        {
            sample_sup_idx |= SAMPLE_FREQ_32K_TABLE_MASK;
        }
        if (p_cap->supported_sampling_frequencies & SAMPLING_FREQUENCY_44_1K)
        {
            sample_sup_idx |= SAMPLE_FREQ_441K_TABLE_MASK;
        }
        if (p_cap->supported_sampling_frequencies & SAMPLING_FREQUENCY_48K)
        {
            sample_sup_idx |= SAMPLE_FREQ_48K_TABLE_MASK;
        }
    }
    if (p_cap->type_exist & CODEC_CAP_SUPPORTED_FRAME_DURATIONS_EXIST)
    {
        if (p_cap->supported_frame_durations & FRAME_DURATION_7_5_MS_BIT)
        {
            frame_duration_idx |= FRAME_DURATION_7_5M_TABLE_MASK;
        }
        if (p_cap->supported_frame_durations & FRAME_DURATION_10_MS_BIT)
        {
            frame_duration_idx |= FRAME_DURATION_10M_TABLE_MASK;
        }
    }
    mask = sample_sup_idx & frame_duration_idx;
    while (mask)
    {
        if (mask & 0x01)
        {
            if (p_cap->min_octets_per_codec_frame <= lc3_codec_cfg[index].octets_per_codec_frame &&
                p_cap->max_octets_per_codec_frame >= lc3_codec_cfg[index].octets_per_codec_frame)
            {
                supported_bits |= (1 << index);
            }
        }
        mask >>= 1;
        index++;
    }
    *p_cfg_bits = supported_bits;
    return true;
}
#endif
