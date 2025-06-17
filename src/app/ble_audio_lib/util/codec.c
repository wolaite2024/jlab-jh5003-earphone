#include <string.h>
#include "trace.h"
#include "codec_def.h"
#include "bt_types.h"
#include "ble_audio_def.h"

#if (LE_AUDIO_PACS_CLIENT_SUPPORT || LE_AUDIO_PACS_SUPPORT)
bool codec_cap_parse(uint8_t value_len, uint8_t *p_value, T_CODEC_CAP *p_cap)
{
    uint8_t err_idx = 0;
    uint16_t idx = 0;
    uint16_t length;
    uint8_t type;
    if (p_value == NULL || value_len == 0 || p_cap == NULL)
    {
        err_idx = 1;
        goto error;
    }
    memset(p_cap, 0, sizeof(T_CODEC_CAP));
    p_cap->max_supported_codec_frames_per_sdu = 1;
    for (; idx < value_len;)
    {
        length = p_value[idx];
        idx++;
        type = p_value[idx];

        switch (type)
        {
        case CODEC_CAP_TYPE_SUPPORTED_SAMPLING_FREQUENCIES:
            {
                if (length != 3)
                {
                    err_idx = 2;
                    goto error;
                }
                LE_ARRAY_TO_UINT16(p_cap->supported_sampling_frequencies, &p_value[idx + 1]);
                if ((p_cap->supported_sampling_frequencies | SAMPLING_FREQUENCY_MASK) != SAMPLING_FREQUENCY_MASK)
                {
                    err_idx = 3;
                    goto error;
                }
                p_cap->type_exist |= CODEC_CAP_SUPPORTED_SAMPLING_FREQUENCIES_EXIST;
            }
            break;

        case CODEC_CAP_TYPE_SUPPORTED_FRAME_DURATIONS:
            {
                if (length != 2)
                {
                    err_idx = 4;
                    goto error;
                }
                p_cap->supported_frame_durations = p_value[idx + 1];
                p_cap->type_exist |= CODEC_CAP_SUPPORTED_FRAME_DURATIONS_EXIST;
            }
            break;

        case CODEC_CAP_TYPE_AUDIO_CHANNEL_COUNTS:
            {
                if (length != 2)
                {
                    err_idx = 6;
                    goto error;
                }
                p_cap->audio_channel_counts = p_value[idx + 1];
                p_cap->type_exist |= CODEC_CAP_AUDIO_CHANNEL_COUNTS_EXIST;
            }
            break;

        case CODEC_CAP_TYPE_SUPPORTED_OCTETS_PER_CODEC_FRAME:
            {
                if (length != 5)
                {
                    err_idx = 8;
                    goto error;
                }
                LE_ARRAY_TO_UINT16(p_cap->min_octets_per_codec_frame, &p_value[idx + 1]);
                LE_ARRAY_TO_UINT16(p_cap->max_octets_per_codec_frame, &p_value[idx + 3]);
                p_cap->type_exist |= CODEC_CAP_SUPPORTED_OCTETS_PER_CODEC_FRAME_EXIST;
            }
            break;

        case CODEC_CAP_TYPE_MAX_SUPPORTED_FRAMES_PER_SDU:
            {
                if (length != 2)
                {
                    err_idx = 9;
                    goto error;
                }
                p_cap->max_supported_codec_frames_per_sdu = p_value[idx + 1];
                if (p_cap->max_supported_codec_frames_per_sdu == 0)
                {
                    err_idx = 10;
                    goto error;
                }
                p_cap->type_exist |= CODEC_CAP_MAX_SUPPORTED_FRAMES_PER_SDU_EXIST;
            }
            break;

        default:
            {
                p_cap->type_exist |= CODEC_CAP_VENDOR_INFO_EXIST;
                PROTOCOL_PRINT_INFO2("codec_cap_parse: vendor info, type %d, length %d", type, length);
            }
            break;
        }
        idx += length;
    }

    if (idx != value_len)
    {
        err_idx = 12;
        goto error;
    }

    return true;
error:
    PROTOCOL_PRINT_ERROR1("codec_cap_parse: failed, err_idx %d", err_idx);
    return false;
}
#endif

#if (LE_AUDIO_ASCS_SUPPORT || LE_AUDIO_ASCS_CLIENT_SUPPORT || LE_AUDIO_BASE_DATA_PARSE)
bool codec_cfg_parse(uint8_t value_len, uint8_t *p_value, T_CODEC_CFG *p_cfg)
{
    uint8_t err_idx = 0;
    uint16_t idx = 0;
    uint16_t length;
    uint8_t type;
    if (p_value == NULL || value_len == 0 || p_cfg == NULL)
    {
        err_idx = 1;
        goto error;
    }
    memset(p_cfg, 0, sizeof(T_CODEC_CFG));
    p_cfg->codec_frame_blocks_per_sdu = 1;
    for (; idx < value_len;)
    {
        length = p_value[idx];
        idx++;
        type = p_value[idx];

        switch (type)
        {
        case CODEC_CFG_TYPE_SAMPLING_FREQUENCY:
            {
                if (length != 2)
                {
                    err_idx = 2;
                    goto error;
                }
                p_cfg->sample_frequency = p_value[idx + 1];
                if (p_cfg->sample_frequency > SAMPLING_FREQUENCY_CFG_96K ||
                    p_cfg->sample_frequency < SAMPLING_FREQUENCY_CFG_8K)
                {
                    err_idx = 3;
                    goto error;
                }
                p_cfg->type_exist |= CODEC_CFG_SAMPLING_FREQUENCY_EXIST;
            }
            break;

        case CODEC_CFG_TYPE_FRAME_DURATION:
            {
                if (length != 2)
                {
                    err_idx = 4;
                    goto error;
                }
                p_cfg->frame_duration = p_value[idx + 1];
                if (p_cfg->frame_duration > FRAME_DURATION_CFG_MAX)
                {
                    err_idx = 5;
                    goto error;
                }
                p_cfg->type_exist |= CODEC_CFG_FRAME_DURATION_EXIST;
            }
            break;

        case CODEC_CFG_TYPE_AUDIO_CHANNEL_ALLOCATION:
            {
                if (length != 5)
                {
                    err_idx = 6;
                    goto error;
                }
                LE_ARRAY_TO_UINT32(p_cfg->audio_channel_allocation, &p_value[idx + 1]);
                if ((p_cfg->audio_channel_allocation | AUDIO_LOCATION_MASK) != AUDIO_LOCATION_MASK)
                {
                    err_idx = 7;
                    goto error;
                }
                p_cfg->type_exist |= CODEC_CFG_AUDIO_CHANNEL_ALLOCATION_EXIST;
            }
            break;

        case CODEC_CFG_TYPE_OCTET_PER_CODEC_FRAME:
            {
                if (length != 3)
                {
                    err_idx = 8;
                    goto error;
                }
                LE_ARRAY_TO_UINT16(p_cfg->octets_per_codec_frame, &p_value[idx + 1]);
                p_cfg->type_exist |= CODEC_CFG_OCTET_PER_CODEC_FRAME_EXIST;
            }
            break;

        case CODEC_CFG_TYPE_BLOCKS_PER_SDU:
            {
                if (length != 2)
                {
                    err_idx = 9;
                    goto error;
                }
                p_cfg->codec_frame_blocks_per_sdu = p_value[idx + 1];
                if (p_cfg->codec_frame_blocks_per_sdu == 0)
                {
                    err_idx = 10;
                    goto error;
                }
                p_cfg->type_exist |= CODEC_CFG_TYPE_BLOCKS_PER_SDU_EXIST;
            }
            break;

        default:
            {
                PROTOCOL_PRINT_INFO2("codec_cfg_parse: vendor info, type %d, length %d", type, length);
                p_cfg->type_exist |= CODEC_CFG_VENDOR_INFO_EXIST;
            }
            break;
        }
        idx += length;
    }

    if (idx != value_len)
    {
        err_idx = 12;
        goto error;
    }
    PROTOCOL_PRINT_INFO6("codec_cfg_parse: type_exist 0x%x, frame_duration %d, sample_frequency %d, codec_frame_blocks_per_sdu %d, octets_per_codec_frame %d, audio_channel_allocation 0x%x",
                         p_cfg->type_exist,
                         p_cfg->frame_duration,
                         p_cfg->sample_frequency,
                         p_cfg->codec_frame_blocks_per_sdu,
                         p_cfg->octets_per_codec_frame,
                         p_cfg->audio_channel_allocation);
    return true;
error:
    PROTOCOL_PRINT_ERROR1("codec_cfg_parse: failed, err_idx %d", err_idx);
    if (err_idx != 1)
    {
        PROTOCOL_PRINT_ERROR1("codec_cfg_parse: failed, error data %b", TRACE_BINARY(value_len, p_value));
    }
    return false;
}
#endif

#if (LE_AUDIO_ASCS_CLIENT_SUPPORT || LE_AUDIO_BASE_DATA_GENERATE || LE_AUDIO_ASCS_SUPPORT)
bool codec_cfg_gen(uint8_t *p_value_len, uint8_t *p_value, T_CODEC_CFG *p_cfg)
{
    uint8_t idx = 0;
    if (p_value == NULL || p_value_len == NULL || p_cfg == NULL)
    {
        goto error;
    }
    if (p_cfg->type_exist & CODEC_CFG_SAMPLING_FREQUENCY_EXIST)
    {
        p_value[idx++] = 0x02;
        p_value[idx++] = CODEC_CFG_TYPE_SAMPLING_FREQUENCY;
        p_value[idx++] = p_cfg->sample_frequency;

    }
    if (p_cfg->type_exist & CODEC_CFG_FRAME_DURATION_EXIST)
    {
        p_value[idx++] = 0x02;
        p_value[idx++] = CODEC_CFG_TYPE_FRAME_DURATION;
        p_value[idx++] = p_cfg->frame_duration;
    }
    if ((p_cfg->type_exist & CODEC_CFG_AUDIO_CHANNEL_ALLOCATION_EXIST) &&
        p_cfg->audio_channel_allocation != AUDIO_LOCATION_MONO)
    {
        p_value[idx++] = 0x05;
        p_value[idx++] = CODEC_CFG_TYPE_AUDIO_CHANNEL_ALLOCATION;
        LE_UINT32_TO_ARRAY(&p_value[idx], p_cfg->audio_channel_allocation);
        idx += 4;
    }
    if (p_cfg->type_exist & CODEC_CFG_OCTET_PER_CODEC_FRAME_EXIST)
    {
        p_value[idx++] = 0x03;
        p_value[idx++] = CODEC_CFG_TYPE_OCTET_PER_CODEC_FRAME;
        LE_UINT16_TO_ARRAY(&p_value[idx], p_cfg->octets_per_codec_frame);
        idx += 2;
    }
    if (p_cfg->type_exist & CODEC_CFG_TYPE_BLOCKS_PER_SDU_EXIST)
    {
        p_value[idx++] = 0x02;
        p_value[idx++] = CODEC_CFG_TYPE_BLOCKS_PER_SDU;
        p_value[idx++] = p_cfg->codec_frame_blocks_per_sdu;
    }
    *p_value_len = idx;
    return true;
error:
    PROTOCOL_PRINT_ERROR0("codec_cfg_gen: failed");
    return false;
}
#endif

#if ((LE_AUDIO_BROADCAST_ASSISTANT_ROLE && LE_AUDIO_PACS_CLIENT_SUPPORT) || LE_AUDIO_PACS_SUPPORT)
bool codec_cfg_check_cap(T_CODEC_CAP *p_cap, T_CODEC_CFG *p_cfg)
{
    uint8_t err_idx = 0;
    uint8_t channel_num = 0;
    if (p_cap == NULL || p_cfg == NULL)
    {
        err_idx = 1;
        goto error;
    }
    if (p_cfg->type_exist & CODEC_CFG_SAMPLING_FREQUENCY_EXIST)
    {
        if (p_cap->type_exist & CODEC_CAP_SUPPORTED_SAMPLING_FREQUENCIES_EXIST)
        {
            uint16_t sample_frequency = 0x0001 << (p_cfg->sample_frequency - 1);
            if ((p_cap->supported_sampling_frequencies & sample_frequency) == 0)
            {
                err_idx = 2;
                goto error;
            }
        }
        else
        {
            err_idx = 3;
            goto error;
        }
    }
    if (p_cfg->type_exist & CODEC_CFG_FRAME_DURATION_EXIST)
    {
        if (p_cap->type_exist & CODEC_CAP_SUPPORTED_FRAME_DURATIONS_EXIST)
        {
            if (p_cfg->frame_duration == FRAME_DURATION_CFG_7_5_MS)
            {
                if ((p_cap->supported_frame_durations & FRAME_DURATION_7_5_MS_BIT) == 0)
                {
                    err_idx = 4;
                    goto error;
                }
            }
            if (p_cfg->frame_duration == FRAME_DURATION_CFG_10_MS)
            {
                if ((p_cap->supported_frame_durations & FRAME_DURATION_10_MS_BIT) == 0)
                {
                    err_idx = 5;
                    goto error;
                }
            }
            if (p_cfg->frame_duration == FRAME_DURATION_CFG_5_MS)
            {
                if ((p_cap->supported_frame_durations & FRAME_DURATION_5_MS_BIT) == 0)
                {
                    err_idx = 12;
                    goto error;
                }
            }
            if (p_cfg->frame_duration == FRAME_DURATION_CFG_2_5_MS)
            {
                if ((p_cap->supported_frame_durations & FRAME_DURATION_2_5_MS_BIT) == 0)
                {
                    err_idx = 13;
                    goto error;
                }
            }
        }
        else
        {
            err_idx = 6;
            goto error;
        }
    }
    if (p_cfg->type_exist & CODEC_CFG_AUDIO_CHANNEL_ALLOCATION_EXIST)
    {
        channel_num = __builtin_popcount(p_cfg->audio_channel_allocation);
        if (channel_num == 0)
        {
            channel_num = 1;
        }

        if (p_cap->type_exist & CODEC_CAP_AUDIO_CHANNEL_COUNTS_EXIST)
        {
            uint8_t channel_bit = 0x01 << (channel_num - 1);
            if ((p_cap->audio_channel_counts & channel_bit) == 0)
            {
                err_idx = 7;
                goto error;
            }
        }
        else
        {
            err_idx = 8;
            goto error;
        }
    }
    if (p_cfg->type_exist & CODEC_CFG_OCTET_PER_CODEC_FRAME_EXIST)
    {
        if (p_cap->type_exist & CODEC_CAP_SUPPORTED_OCTETS_PER_CODEC_FRAME_EXIST)
        {
            if (p_cfg->octets_per_codec_frame < p_cap->min_octets_per_codec_frame ||
                p_cfg->octets_per_codec_frame > p_cap->max_octets_per_codec_frame)
            {
                err_idx = 9;
                goto error;
            }
        }
    }
    if (p_cfg->type_exist & CODEC_CFG_TYPE_BLOCKS_PER_SDU_EXIST)
    {
        if (p_cap->type_exist & CODEC_CAP_MAX_SUPPORTED_FRAMES_PER_SDU_EXIST)
        {
            uint8_t frame_num = channel_num * p_cfg->codec_frame_blocks_per_sdu;
            if (frame_num > p_cap->max_supported_codec_frames_per_sdu)
            {
                err_idx = 10;
                goto error;
            }
        }
        else
        {
            if (p_cfg->codec_frame_blocks_per_sdu > 1)
            {
                err_idx = 11;
                goto error;
            }
        }
    }
    return true;
error:
    PROTOCOL_PRINT_INFO1("codec_cfg_check_cap: not find, cause %d", err_idx);
    return false;
}
#endif

#if LE_AUDIO_PACS_SUPPORT
bool codec_cap_check(uint8_t len, uint8_t *p_data, T_CODEC_CFG *p_cfg)
{
    T_CODEC_CAP codec_cap;
    if (codec_cap_parse(len, p_data, &codec_cap))
    {
        if (codec_cfg_check_cap(&codec_cap, p_cfg))
        {
            return true;
        }
    }
    return false;
}
#endif
