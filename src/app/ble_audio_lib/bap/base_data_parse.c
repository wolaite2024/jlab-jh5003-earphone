#include <string.h>
#include "trace.h"
#include "base_data_parse.h"
#include "ltv_util.h"
#include "codec_def.h"
#include "gap_le_types.h"
#include "ble_audio_def.h"
#include "ble_audio_mgr.h"

#if LE_AUDIO_BASE_DATA_PARSE
#if LE_AUDIO_DEINIT
typedef struct t_base_data_parse
{
    struct t_base_data_parse *p_next;
    T_BASE_DATA_MAPPING *p_mapping;
} T_BASE_DATA_PARSE;

T_OS_QUEUE base_data_queue;

T_BASE_DATA_PARSE *base_data_find_mapping(T_BASE_DATA_MAPPING *p_mapping)
{
    T_BASE_DATA_PARSE *p_db;
    for (uint8_t i = 0; i < base_data_queue.count; i++)
    {
        p_db = (T_BASE_DATA_PARSE *)os_queue_peek(&base_data_queue, i);
        if (p_db->p_mapping == p_mapping)
        {
            return p_db;
        }
    }
    return NULL;
}

void base_data_add_mapping(T_BASE_DATA_MAPPING *p_mapping)
{
    T_BASE_DATA_PARSE *p_db;
    p_db = ble_audio_mem_zalloc(sizeof(T_BASE_DATA_PARSE));
    if (p_db)
    {
        p_db->p_mapping = p_mapping;
        os_queue_in(&base_data_queue, p_db);
    }
}

void base_data_remove_mapping(T_BASE_DATA_MAPPING *p_mapping)
{
    T_BASE_DATA_PARSE *p_db = base_data_find_mapping(p_mapping);

    if (p_db)
    {
        os_queue_delete(&base_data_queue, p_db);
        ble_audio_mem_free(p_db);
    }
}

#endif

void base_data_free(T_BASE_DATA_MAPPING *p_mapping)
{
    T_BASE_DATA_SUBGROUP_PARAM *p_subgroup = NULL;
    if (p_mapping == NULL)
    {
        return;
    }
#if LE_AUDIO_DEINIT
    base_data_remove_mapping(p_mapping);
#endif
    if (p_mapping->p_subgroup)
    {
        for (uint8_t i = 0; i < p_mapping->num_subgroups; i++)
        {
            p_subgroup = &p_mapping->p_subgroup[i];
            if (p_subgroup->p_metadata)
            {
                ble_audio_mem_free(p_subgroup->p_metadata);
            }
            if (p_subgroup->p_bis_param)
            {
                ble_audio_mem_free(p_subgroup->p_bis_param);
            }
        }
        ble_audio_mem_free(p_mapping->p_subgroup);
    }

    ble_audio_mem_free(p_mapping);
}

bool base_data_get_bis_codec_cfg(T_BASE_DATA_MAPPING *p_mapping, uint8_t bis_idx,
                                 T_CODEC_CFG *p_cfg)
{
    uint8_t i = 0;
    uint8_t j = 0;
    T_BASE_DATA_SUBGROUP_PARAM *p_subgroup = NULL;
    T_BASE_DATA_BIS_PARAM *p_bis;
    if (p_mapping == NULL || p_cfg == NULL)
    {
        return false;
    }
    for (i = 0; i < p_mapping->num_subgroups; i++)
    {
        p_subgroup = &p_mapping->p_subgroup[i];
        for (j = 0; j < p_subgroup->num_bis; j++)
        {
            p_bis = &p_subgroup->p_bis_param[j];
            if (p_bis->bis_index == bis_idx)
            {
                memcpy(p_cfg, &p_bis->bis_codec_cfg, sizeof(T_CODEC_CFG));
                return true;
            }
        }
    }

    return false;
}

bool base_data_check_data(uint16_t len, uint8_t *p_data, uint16_t *p_offset)
{
    uint8_t error_idx = 0;
    uint16_t data_len = 0;
    uint16_t idx = 0;
    uint16_t length;
    bool found = false;
    uint16_t uuid;

    if (p_data == NULL || len == 0)
    {
        error_idx = 1;
        goto error;
    }
    for (; data_len < len;)
    {
        length = p_data[idx];
        if (p_data[idx + 1] == GAP_ADTYPE_SERVICE_DATA)
        {
            LE_ARRAY_TO_UINT16(uuid, p_data + idx + 2);
            if (uuid == BASIC_AUDIO_ANNOUNCEMENT_SRV_UUID)
            {
                *p_offset = idx;
                found = true;
            }
        }
        idx += (length + 1);
        data_len += (length + 1);
    }

    if (found == false)
    {
        error_idx = 2;
        goto error;
    }

    if (data_len != len)
    {
        error_idx = 3;
        goto error;
    }
    return true;
error:
    PROTOCOL_PRINT_ERROR1("base_data_check_data: failed, error_idx %d", error_idx);
    return false;
}

T_BASE_DATA_MAPPING *base_data_parse_data(uint16_t pa_data_len, uint8_t *p_pa_data)
{
    bool ret;
    uint8_t error_idx = 0;
    T_BASE_DATA_MAPPING *p_mapping = NULL;
    T_BASE_DATA_SUBGROUP_PARAM *p_subgroup = NULL;
    T_BASE_DATA_BIS_PARAM *p_bis;
    uint8_t codec_id_level2[CODEC_ID_LEN];
    T_CODEC_CFG    codec_cfg_level2;
    T_CODEC_CFG    codec_cfg_level3;
    uint8_t *p_codec_cfg;
    uint8_t codec_cfg_len;
    uint16_t len_offset;
    uint8_t i = 0;
    uint8_t j = 0;

    ret = base_data_check_data(pa_data_len, p_pa_data, &len_offset);
    if (ret == false)
    {
        error_idx = 1;
        goto error;
    }

    p_mapping = ble_audio_mem_zalloc(sizeof(T_BASE_DATA_MAPPING));
    if (p_mapping == NULL)
    {
        error_idx = 2;
        goto error;
    }

    STREAM_SKIP_LEN(p_pa_data, len_offset);
    STREAM_SKIP_LEN(p_pa_data, 4); //Skip len+type+uuid16

    LE_STREAM_TO_UINT24(p_mapping->presentation_delay, p_pa_data);
    LE_STREAM_TO_UINT8(p_mapping->num_subgroups, p_pa_data);

    p_mapping->p_subgroup = ble_audio_mem_zalloc(p_mapping->num_subgroups * sizeof(
                                                     T_BASE_DATA_SUBGROUP_PARAM));
    if (p_mapping->p_subgroup == NULL)
    {
        error_idx = 4;
        p_mapping->num_subgroups = 0;
        goto error;
    }

    for (i = 0; i < p_mapping->num_subgroups; i++)
    {
        uint8_t *p_temp_metadata = NULL;
        p_subgroup = &p_mapping->p_subgroup[i];
        p_subgroup->subgroup_idx = i;
        LE_STREAM_TO_UINT8(p_subgroup->num_bis, p_pa_data);
        memcpy(codec_id_level2, p_pa_data, CODEC_ID_LEN);
        STREAM_SKIP_LEN(p_pa_data, CODEC_ID_LEN);

        LE_STREAM_TO_UINT8(codec_cfg_len, p_pa_data);
        p_codec_cfg = p_pa_data;
        STREAM_SKIP_LEN(p_pa_data, codec_cfg_len);
        LE_STREAM_TO_UINT8(p_subgroup->metadata_len, p_pa_data);
        p_temp_metadata = p_pa_data;
        STREAM_SKIP_LEN(p_pa_data, p_subgroup->metadata_len);

        if (codec_cfg_len != 0)
        {
            ret = ltv_util_check_format(codec_cfg_len, p_codec_cfg);
            if (ret == false)
            {
                error_idx = 5;
                goto error;
            }
        }

        //parse the level 2 codec cfg
        memset((uint8_t *)&codec_cfg_level2, 0, sizeof(T_CODEC_CFG));
        if (codec_cfg_len)
        {
            codec_cfg_parse(codec_cfg_len, p_codec_cfg, &codec_cfg_level2);
        }

        if (p_subgroup->metadata_len != 0)
        {
            ret = ltv_util_check_format(p_subgroup->metadata_len, p_temp_metadata);
            if (ret == false)
            {
                error_idx = 6;
                goto error;
            }
        }

        //copy meta data
        if (p_subgroup->metadata_len != 0)
        {
            p_subgroup->p_metadata = ble_audio_mem_zalloc(p_subgroup->metadata_len);
            if (p_subgroup->p_metadata == NULL)
            {
                error_idx = 7;
                p_subgroup->metadata_len = 0;
                goto error;
            }
            memcpy(p_subgroup->p_metadata, p_temp_metadata, p_subgroup->metadata_len);
        }

        p_subgroup->p_bis_param = ble_audio_mem_zalloc(p_subgroup->num_bis * sizeof(T_BASE_DATA_BIS_PARAM));
        if (p_subgroup->p_bis_param == NULL)
        {
            error_idx = 8;
            p_subgroup->num_bis = 0;
            goto error;
        }

        for (j = 0; j < p_subgroup->num_bis; j++)
        {
            p_bis = &p_subgroup->p_bis_param[j];
            memcpy(p_bis->codec_id, codec_id_level2, CODEC_ID_LEN);
            p_mapping->num_bis++;
            p_bis->subgroup_idx = i;
            LE_STREAM_TO_UINT8(p_bis->bis_index, p_pa_data);
            p_subgroup->bis_array |= (1 << (p_bis->bis_index - 1));
            LE_STREAM_TO_UINT8(codec_cfg_len, p_pa_data);
            p_codec_cfg = p_pa_data;
            STREAM_SKIP_LEN(p_pa_data, codec_cfg_len);
            if (codec_cfg_len != 0)
            {
                ret = ltv_util_check_format(codec_cfg_len, p_codec_cfg);
                if (ret == false)
                {
                    error_idx = 9;
                    goto error;
                }
            }
            //parse the level 3 codec cfg
            memset((uint8_t *)&codec_cfg_level3, 0, sizeof(T_CODEC_CFG));
            if (codec_cfg_len)
            {
                codec_cfg_parse(codec_cfg_len, p_codec_cfg, &codec_cfg_level3);
            }

            //merge all the codec cfg
            p_bis->bis_codec_cfg.type_exist = codec_cfg_level2.type_exist |
                                              codec_cfg_level3.type_exist;

            if (codec_cfg_level3.type_exist & CODEC_CFG_SAMPLING_FREQUENCY_EXIST)
            {
                p_bis->bis_codec_cfg.sample_frequency = codec_cfg_level3.sample_frequency;
            }
            else if (codec_cfg_level2.type_exist & CODEC_CFG_SAMPLING_FREQUENCY_EXIST)
            {
                p_bis->bis_codec_cfg.sample_frequency = codec_cfg_level2.sample_frequency;
            }

            if (codec_cfg_level3.type_exist & CODEC_CFG_AUDIO_CHANNEL_ALLOCATION_EXIST)
            {
                p_bis->bis_codec_cfg.audio_channel_allocation = codec_cfg_level3.audio_channel_allocation;
            }
            else if (codec_cfg_level2.type_exist & CODEC_CFG_AUDIO_CHANNEL_ALLOCATION_EXIST)
            {
                p_bis->bis_codec_cfg.audio_channel_allocation = codec_cfg_level2.audio_channel_allocation;
            }

            if ((p_bis->bis_codec_cfg.type_exist & CODEC_CFG_TYPE_BLOCKS_PER_SDU_EXIST) == 0)
            {
                p_bis->bis_codec_cfg.type_exist |= CODEC_CFG_TYPE_BLOCKS_PER_SDU_EXIST;
                p_bis->bis_codec_cfg.codec_frame_blocks_per_sdu = 1;
            }
            else if (codec_cfg_level2.type_exist & CODEC_CFG_TYPE_BLOCKS_PER_SDU_EXIST)
            {
                p_bis->bis_codec_cfg.codec_frame_blocks_per_sdu = codec_cfg_level2.codec_frame_blocks_per_sdu;
            }
            else if (codec_cfg_level3.type_exist & CODEC_CFG_TYPE_BLOCKS_PER_SDU_EXIST)
            {
                p_bis->bis_codec_cfg.codec_frame_blocks_per_sdu = codec_cfg_level3.codec_frame_blocks_per_sdu;
            }

            if (codec_cfg_level2.type_exist & CODEC_CFG_OCTET_PER_CODEC_FRAME_EXIST)
            {
                p_bis->bis_codec_cfg.octets_per_codec_frame = codec_cfg_level2.octets_per_codec_frame;
            }
            else if (codec_cfg_level3.type_exist & CODEC_CFG_OCTET_PER_CODEC_FRAME_EXIST)
            {
                p_bis->bis_codec_cfg.octets_per_codec_frame = codec_cfg_level3.octets_per_codec_frame;
            }

            if (codec_cfg_level2.type_exist & CODEC_CFG_FRAME_DURATION_EXIST)
            {
                p_bis->bis_codec_cfg.frame_duration = codec_cfg_level2.frame_duration;
            }
            else if (codec_cfg_level3.type_exist & CODEC_CFG_FRAME_DURATION_EXIST)
            {
                p_bis->bis_codec_cfg.frame_duration = codec_cfg_level3.frame_duration;
            }
        }
    }
#if LE_AUDIO_DEINIT
    base_data_add_mapping(p_mapping);
#endif
    return p_mapping;
error:
    if (p_mapping)
    {
        base_data_free(p_mapping);
    }
    PROTOCOL_PRINT_ERROR1("base_data_parse_data: failed, error_idx %d", error_idx);
    return NULL;
}

bool base_data_cmp(T_BASE_DATA_MAPPING *p_mapping_a, T_BASE_DATA_MAPPING *p_mapping_b)
{
    uint8_t i, j;
    if (p_mapping_a == NULL && p_mapping_b == NULL)
    {
        return true;
    }
    else if (p_mapping_a == NULL || p_mapping_b == NULL)
    {
        return false;
    }

    if (p_mapping_a->num_subgroups != p_mapping_b->num_subgroups ||
        p_mapping_a->num_bis != p_mapping_b->num_bis)
    {
        return false;
    }
    for (i = 0; i < p_mapping_a->num_subgroups; i++)
    {
        if (p_mapping_a->p_subgroup[i].num_bis != p_mapping_b->p_subgroup[i].num_bis ||
            p_mapping_a->p_subgroup[i].bis_array != p_mapping_b->p_subgroup[i].bis_array ||
            p_mapping_a->p_subgroup[i].metadata_len != p_mapping_b->p_subgroup[i].metadata_len ||
            memcmp(p_mapping_a->p_subgroup[i].p_metadata, p_mapping_b->p_subgroup[i].p_metadata,
                   p_mapping_a->p_subgroup[i].metadata_len) != 0)
        {
            return false;
        }
        for (j = 0; j < p_mapping_a->p_subgroup[i].num_bis; j++)
        {
            if (memcmp(p_mapping_a->p_subgroup[i].p_bis_param + j, p_mapping_b->p_subgroup[i].p_bis_param + j,
                       sizeof(T_BASE_DATA_BIS_PARAM)) != 0)
            {
                return false;
            }
        }
    }

    return true;
}

void base_data_print(T_BASE_DATA_MAPPING *p_mapping)
{
    T_BASE_DATA_SUBGROUP_PARAM *p_subgroup;
    T_BASE_DATA_BIS_PARAM *p_bis;
    uint8_t i = 0;
    uint8_t j = 0;

    if (p_mapping == NULL || p_mapping->p_subgroup == NULL)
    {
        return;
    }
    PROTOCOL_PRINT_INFO3("Level 1 Group: num_subgroups %d, num_bis %d, presentation_delay 0x%x",
                         p_mapping->num_subgroups, p_mapping->num_bis, p_mapping->presentation_delay);

    for (i = 0; i < p_mapping->num_subgroups; i++)
    {
        p_subgroup = &p_mapping->p_subgroup[i];
        PROTOCOL_PRINT_INFO3("Level 2 Subgroup[%d]: num_bis %d, bis_array 0x%x", p_subgroup->subgroup_idx,
                             p_subgroup->num_bis, p_subgroup->bis_array);
        if (p_subgroup->metadata_len != 0)
        {
            PROTOCOL_PRINT_INFO3("Level 2 Subgroup[%d]: metadata[%d] %b",
                                 p_subgroup->subgroup_idx,
                                 p_subgroup->metadata_len, TRACE_BINARY(p_subgroup->metadata_len, p_subgroup->p_metadata));
        }
        if (p_subgroup->p_bis_param == NULL)
        {
            continue;
        }
        for (j = 0; j < p_subgroup->num_bis; j++)
        {
            p_bis = &p_subgroup->p_bis_param[j];
            PROTOCOL_PRINT_INFO3("Level 3 BIS[%d]: bis_index %d, code id: %b",
                                 p_bis->subgroup_idx, p_bis->bis_index,
                                 TRACE_BINARY(CODEC_ID_LEN, p_bis->codec_id));
            PROTOCOL_PRINT_INFO3("Level 3 bis codec, frame_duration: 0x%x, sample_frequency: 0x%x , codec_frame_blocks_per_sdu: 0x%x",
                                 p_bis->bis_codec_cfg.frame_duration, p_bis->bis_codec_cfg.sample_frequency,
                                 p_bis->bis_codec_cfg.codec_frame_blocks_per_sdu);
            PROTOCOL_PRINT_INFO2("Level 3 bis codec, octets_per_codec_frame: 0x%x, audio_channel_allocation: 0x%x",
                                 p_bis->bis_codec_cfg.octets_per_codec_frame, p_bis->bis_codec_cfg.audio_channel_allocation);
        }
    }
}

#if LE_AUDIO_DEINIT
void base_data_parse_deinit(void)
{
    T_BASE_DATA_PARSE *p_db;

    while ((p_db = os_queue_out(&base_data_queue)) != NULL)
    {
        T_BASE_DATA_SUBGROUP_PARAM *p_subgroup = NULL;
        if (p_db->p_mapping == NULL)
        {
            ble_audio_mem_free(p_db);
            continue;
        }

        if (p_db->p_mapping->p_subgroup)
        {
            for (uint8_t i = 0; i < p_db->p_mapping->num_subgroups; i++)
            {
                p_subgroup = &p_db->p_mapping->p_subgroup[i];
                if (p_subgroup->p_metadata)
                {
                    ble_audio_mem_free(p_subgroup->p_metadata);
                }
                if (p_subgroup->p_bis_param)
                {
                    ble_audio_mem_free(p_subgroup->p_bis_param);
                }
            }
            ble_audio_mem_free(p_db->p_mapping->p_subgroup);
        }

        ble_audio_mem_free(p_db->p_mapping);
        ble_audio_mem_free(p_db);
    }
}
#endif
#endif
