#include <string.h>
#include "trace.h"
#include "base_data_generate.h"
#include "ltv_util.h"
#include "codec_def.h"
#include "gap_le_types.h"
#include "ble_audio_def.h"
#include "os_queue.h"
#include "ble_audio_mgr.h"

#if LE_AUDIO_BASE_DATA_GENERATE
#define BASE_DATA_MAX_LEN   256

typedef struct t_base_data_group
{
    struct t_base_data_group *p_next;
    //ctrl param
    uint8_t group_idx;
    uint8_t num_bis;
    uint8_t num_subgroups;
    T_OS_QUEUE subgroup_queue;
    //base param
    uint32_t presentation_delay;
} T_BASE_DATA_GROUP;

typedef struct t_base_data_subgroup
{
    struct t_base_data_subgroup *p_next;
    //ctrl param
    uint8_t subgroup_idx;
    //base param
    uint8_t codec_id[CODEC_ID_LEN];
    uint8_t num_bis;
    uint8_t codec_cfg_len;
    uint8_t metadata_len;
    uint8_t metadata_offset;
    uint8_t codec_metadata_len;
    uint8_t *p_codec_metadata;
    T_OS_QUEUE bis_queue;
} T_BASE_DATA_SUBGROUP;

typedef struct t_base_data_bis
{
    struct t_base_data_bis *p_next;
    uint8_t bis_index;
    uint8_t codec_cfg_len;
    uint8_t codec_cfg[];
} T_BASE_DATA_BIS;

T_OS_QUEUE base_group_queue;
uint8_t base_group_idx = 0;

T_BASE_DATA_GROUP *base_data_find_group(uint8_t group_idx)
{
    T_BASE_DATA_GROUP *p_db;
    for (uint8_t i = 0; i < base_group_queue.count; i++)
    {
        p_db = (T_BASE_DATA_GROUP *)os_queue_peek(&base_group_queue, i);
        if (p_db->group_idx == group_idx)
        {
            return p_db;
        }
    }
    return NULL;
}

T_BASE_DATA_SUBGROUP *base_data_find_subgroup(T_BASE_DATA_GROUP *p_group, uint8_t subgroup_idx)
{
    T_BASE_DATA_SUBGROUP *p_db;
    for (uint8_t i = 0; i < p_group->subgroup_queue.count; i++)
    {
        p_db = (T_BASE_DATA_SUBGROUP *)os_queue_peek(&p_group->subgroup_queue, i);
        if (p_db->subgroup_idx == subgroup_idx)
        {
            return p_db;
        }
    }
    return NULL;
}

T_BASE_DATA_BIS *base_data_find_bis(T_BASE_DATA_SUBGROUP *p_subgroup, uint8_t bis_index)
{
    T_BASE_DATA_BIS *p_db;
    for (uint8_t i = 0; i < p_subgroup->bis_queue.count; i++)
    {
        p_db = (T_BASE_DATA_BIS *)os_queue_peek(&p_subgroup->bis_queue, i);
        if (p_db->bis_index == bis_index)
        {
            return p_db;
        }
    }
    return NULL;
}

bool base_data_add_group(uint8_t *p_group_idx, uint32_t presentation_delay)
{
    uint8_t error_idx = 0;
    T_BASE_DATA_GROUP *p_base_group = NULL;
    if (p_group_idx == NULL)
    {
        error_idx = 2;
        goto error;
    }

    p_base_group = ble_audio_mem_zalloc(sizeof(T_BASE_DATA_GROUP));
    if (p_base_group == NULL)
    {
        error_idx = 4;
        goto error;
    }
    p_base_group->group_idx = base_group_idx;
    p_base_group->presentation_delay = presentation_delay;
    os_queue_in(&base_group_queue, p_base_group);

    *p_group_idx = base_group_idx;
    base_group_idx++;
    //PROTOCOL_PRINT_INFO1("base_data_add_group: base_group_idx %d", p_base_group->group_idx);
    return true;
error:
    if (p_base_group)
    {
        ble_audio_mem_free(p_base_group);
    }
    PROTOCOL_PRINT_ERROR1("base_data_add_group: failed, error_idx %d", error_idx);
    return false;
}

bool base_data_del_subgroup(T_BASE_DATA_GROUP *p_base_group, T_BASE_DATA_SUBGROUP *p_subgroup)
{
    T_BASE_DATA_BIS *p_bis = NULL;
    while ((p_bis = (T_BASE_DATA_BIS *)os_queue_out(&p_subgroup->bis_queue)) != NULL)
    {
        ble_audio_mem_free(p_bis);
    }
    if (p_subgroup->p_codec_metadata)
    {
        ble_audio_mem_free(p_subgroup->p_codec_metadata);
    }
    os_queue_delete(&p_base_group->subgroup_queue, p_subgroup);
    ble_audio_mem_free(p_subgroup);
    return true;
}

bool base_data_del_group(uint8_t group_idx)
{
    uint8_t i;
    uint8_t error_idx = 0;
    T_BASE_DATA_GROUP *p_base_group = NULL;
    T_BASE_DATA_SUBGROUP *p_subgroup = NULL;
    p_base_group = base_data_find_group(group_idx);
    if (p_base_group == NULL)
    {
        error_idx = 1;
        goto error;
    }
    for (i = 0; i < p_base_group->num_subgroups; i++)
    {
        p_subgroup = base_data_find_subgroup(p_base_group, i);
        if (p_subgroup == NULL)
        {
            error_idx = 2;
            goto error;
        }
        if (base_data_del_subgroup(p_base_group, p_subgroup) == false)
        {
            error_idx = 3;
            goto error;
        }
    }

    os_queue_delete(&base_group_queue, p_base_group);
    ble_audio_mem_free(p_base_group);
    return true;
error:
    PROTOCOL_PRINT_ERROR2("base_data_del_group: failed, error_idx %d, group_idx %d",
                          error_idx, group_idx);
    return false;
}

bool base_data_add_subgroup(uint8_t *p_subgroup_idx, uint8_t group_idx, uint8_t *codec_id,
                            uint8_t codec_cfg_len, uint8_t *p_codec_cfg,
                            uint8_t metadata_len, uint8_t *p_metadata)
{
    bool ret;
    uint8_t error_idx = 0;
    T_BASE_DATA_GROUP *p_base_group = NULL;
    T_BASE_DATA_SUBGROUP *p_subgroup = NULL;
    uint8_t codec_metadata_len = 0;
    uint8_t idx = 0;
    if (codec_id == NULL || p_subgroup_idx == NULL)
    {
        error_idx = 1;
        goto error;
    }

    if (codec_cfg_len != 0)
    {
        ret = ltv_util_check_format(codec_cfg_len, p_codec_cfg);
        if (ret == false)
        {
            error_idx = 2;
            goto error;
        }
    }
    if (metadata_len != 0)
    {
        ret = ltv_util_check_format(metadata_len, p_metadata);
        if (ret == false)
        {
            error_idx = 3;
            goto error;
        }
    }

    p_base_group = base_data_find_group(group_idx);
    if (p_base_group == NULL)
    {
        error_idx = 4;
        goto error;
    }

    codec_metadata_len = codec_cfg_len + metadata_len + 2;
    p_subgroup = ble_audio_mem_zalloc(sizeof(T_BASE_DATA_SUBGROUP));
    if (p_subgroup == NULL)
    {
        error_idx = 6;
        goto error;
    }
    p_subgroup->p_codec_metadata = ble_audio_mem_zalloc(codec_metadata_len);
    if (p_subgroup->p_codec_metadata == NULL)
    {
        error_idx = 7;
        goto error;
    }
    p_subgroup->subgroup_idx = p_base_group->num_subgroups;
    memcpy(p_subgroup->codec_id, codec_id, CODEC_ID_LEN);
    p_subgroup->codec_cfg_len = codec_cfg_len;
    p_subgroup->metadata_len = metadata_len;
    p_subgroup->codec_metadata_len = codec_metadata_len;
    p_subgroup->p_codec_metadata[idx] = codec_cfg_len;
    idx++;
    if (codec_cfg_len != 0)
    {
        memcpy(&p_subgroup->p_codec_metadata[idx], p_codec_cfg, codec_cfg_len);
        idx += codec_cfg_len;
    }
    p_subgroup->p_codec_metadata[idx] = metadata_len;
    idx++;
    p_subgroup->metadata_offset = idx;
    if (metadata_len != 0)
    {
        memcpy(&p_subgroup->p_codec_metadata[idx], p_metadata, metadata_len);
        idx += metadata_len;
    }
    os_queue_in(&p_base_group->subgroup_queue, p_subgroup);
    p_base_group->num_subgroups++;
    *p_subgroup_idx = p_subgroup->subgroup_idx;
    //PROTOCOL_PRINT_INFO2("base_data_add_subgroup: group_idx %d, subgroup_idx %d",
    //group_idx, p_subgroup->subgroup_idx);
    return true;
error:
    if (p_subgroup)
    {
        if (p_subgroup->p_codec_metadata != NULL)
        {
            ble_audio_mem_free(p_subgroup->p_codec_metadata);
        }
        ble_audio_mem_free(p_subgroup);
    }
    PROTOCOL_PRINT_ERROR1("base_data_add_subgroup: failed, error_idx %d", error_idx);
    return false;
}

bool base_data_add_bis(uint8_t *p_bis_idx, uint8_t group_idx, uint8_t subgroup_idx,
                       uint8_t codec_cfg_len, uint8_t *p_codec_cfg)
{
    bool ret;
    uint8_t error_idx = 0;
    T_BASE_DATA_GROUP *p_base_group = NULL;
    T_BASE_DATA_SUBGROUP *p_subgroup = NULL;
    T_BASE_DATA_BIS *p_bis = NULL;
    if (p_bis_idx == NULL)
    {
        error_idx = 1;
        goto error;
    }

    if (codec_cfg_len != 0)
    {
        ret = ltv_util_check_format(codec_cfg_len, p_codec_cfg);
        if (ret == false)
        {
            error_idx = 2;
            goto error;
        }
    }

    p_base_group = base_data_find_group(group_idx);
    if (p_base_group == NULL)
    {
        error_idx = 3;
        goto error;
    }
    p_subgroup = base_data_find_subgroup(p_base_group, subgroup_idx);
    if (p_subgroup == NULL)
    {
        error_idx = 4;
        goto error;
    }
    p_bis = ble_audio_mem_zalloc((sizeof(T_BASE_DATA_BIS) + codec_cfg_len));
    if (p_bis == NULL)
    {
        error_idx = 5;
        goto error;
    }

    p_bis->bis_index = p_base_group->num_bis + 1;
    p_bis->codec_cfg_len = codec_cfg_len;
    if (codec_cfg_len != 0)
    {
        memcpy(p_bis->codec_cfg, p_codec_cfg, codec_cfg_len);
    }
    os_queue_in(&p_subgroup->bis_queue, p_bis);
    p_base_group->num_bis++;
    p_subgroup->num_bis++;
    *p_bis_idx = p_bis->bis_index;
    //PROTOCOL_PRINT_INFO3("base_data_add_bis: group_idx %d, subgroup_idx %d, bis_idx %d",
    // group_idx, subgroup_idx, p_bis->bis_index);
    return true;
error:
    if (p_bis)
    {
        ble_audio_mem_free(p_bis);
    }
    PROTOCOL_PRINT_ERROR1("base_data_add_bis: failed, error_idx %d", error_idx);
    return false;
}

bool base_data_gen_pa_data(uint8_t group_idx, uint16_t *p_pa_len, uint8_t **pp_pa_data)
{
    uint8_t error_idx = 0;
    T_BASE_DATA_GROUP *p_base_group = NULL;
    T_BASE_DATA_SUBGROUP *p_subgroup = NULL;
    T_BASE_DATA_BIS *p_bis = NULL;
    uint8_t i;
    uint8_t *p_data;
    uint16_t idx = 0;

    p_data = calloc(1, BASE_DATA_MAX_LEN);
    if (p_data == NULL)
    {
        error_idx = 1;
        goto error;
    }

    p_base_group = base_data_find_group(group_idx);
    if (p_base_group == NULL)
    {
        error_idx = 2;
        goto error;
    }
    //Service Data AD data type
    idx++; //skip total length
    p_data[idx] = GAP_ADTYPE_SERVICE_DATA;
    idx++;
    p_data[idx] = (uint8_t)(BASIC_AUDIO_ANNOUNCEMENT_SRV_UUID);
    idx++;
    p_data[idx] = (uint8_t)(BASIC_AUDIO_ANNOUNCEMENT_SRV_UUID >> 8);
    idx++;
    //Level 1
    LE_UINT24_TO_ARRAY(&p_data[idx], p_base_group->presentation_delay);
    idx += 3;
    if (p_base_group->num_subgroups == 0)
    {
        error_idx = 3;
        goto error;
    }
    p_data[idx] = p_base_group->num_subgroups;
    idx++;

    for (i = 0; i < p_base_group->num_subgroups; i++)
    {
        p_subgroup = base_data_find_subgroup(p_base_group, i);
        if (p_subgroup == NULL)
        {
            error_idx = 4;
            goto error;
        }
        if (p_subgroup->num_bis == 0)
        {
            error_idx = 5;
            goto error;
        }
        if ((idx + 8 + p_subgroup->codec_metadata_len) > BASE_DATA_MAX_LEN)
        {
            error_idx = 6;
            goto error;
        }
        //Level 2
        p_data[idx] = p_subgroup->num_bis;
        idx++;
        memcpy(&p_data[idx], p_subgroup->codec_id, CODEC_ID_LEN);
        idx += CODEC_ID_LEN;
        memcpy(&p_data[idx], p_subgroup->p_codec_metadata, p_subgroup->codec_metadata_len);
        idx += p_subgroup->codec_metadata_len;

        for (uint8_t i = 0; i < p_subgroup->bis_queue.count; i++)
        {
            p_bis = (T_BASE_DATA_BIS *)os_queue_peek(&p_subgroup->bis_queue, i);
            if (p_bis)
            {
                if ((idx + 2 + p_bis->codec_cfg_len) > BASE_DATA_MAX_LEN)
                {
                    error_idx = 8;
                    goto error;
                }
                //Level 3
                p_data[idx] = p_bis->bis_index;   //this should be got from cig param, as stack also alloc
                //bis index from 1 to ... in order. So we set bis index in order too.
                idx++;
                p_data[idx] = p_bis->codec_cfg_len;
                idx++;
                memcpy(&p_data[idx], p_bis->codec_cfg, p_bis->codec_cfg_len);
                idx += p_bis->codec_cfg_len;
            }
        }
    }
    p_data[0] = idx - 1;
    *p_pa_len = idx;
    *pp_pa_data = p_data;
    return true;
error:
    if (p_data)
    {
        free(p_data);
    }
    PROTOCOL_PRINT_ERROR1("base_data_gen_pa_data: failed, error_idx %d", error_idx);
    return false;
}

bool base_data_update_metadata(uint8_t group_idx, uint8_t subgroup_idx,
                               uint8_t metadata_len, uint8_t *p_metadata)
{
    bool ret;
    uint8_t error_idx = 0;
    T_BASE_DATA_GROUP *p_base_group = NULL;
    T_BASE_DATA_SUBGROUP *p_subgroup = NULL;
    uint8_t codec_metadata_len = 0;
    uint8_t *p_codec_metadata = NULL;
    uint8_t idx = 0;

    if (metadata_len != 0)
    {
        ret = ltv_util_check_format(metadata_len, p_metadata);
        if (ret == false)
        {
            error_idx = 2;
            goto error;
        }
    }

    p_base_group = base_data_find_group(group_idx);
    if (p_base_group == NULL)
    {
        error_idx = 3;
        goto error;
    }

    p_subgroup = base_data_find_subgroup(p_base_group, subgroup_idx);
    if (p_subgroup == NULL)
    {
        error_idx = 4;
        goto error;
    }
    codec_metadata_len = metadata_len + p_subgroup->codec_cfg_len + 2;
    p_codec_metadata = ble_audio_mem_zalloc(codec_metadata_len);
    if (p_codec_metadata == NULL)
    {
        error_idx = 5;
        goto error;
    }
    p_codec_metadata[idx] = p_subgroup->codec_cfg_len;
    idx++;
    if (p_subgroup->codec_cfg_len != 0)
    {
        memcpy(&p_codec_metadata[idx], &p_subgroup->p_codec_metadata[1], p_subgroup->codec_cfg_len);
        idx += p_subgroup->codec_cfg_len;
    }
    p_codec_metadata[idx] = metadata_len;
    idx++;
    p_subgroup->metadata_offset = idx;

    if (metadata_len != 0)
    {
        memcpy(&p_codec_metadata[idx], p_metadata, metadata_len);
        idx += metadata_len;
    }
    p_subgroup->metadata_len = metadata_len;
    p_subgroup->codec_metadata_len = codec_metadata_len;
    if (p_subgroup->p_codec_metadata)
    {
        ble_audio_mem_free(p_subgroup->p_codec_metadata);
    }
    p_subgroup->p_codec_metadata = p_codec_metadata;

    PROTOCOL_PRINT_INFO2("base_data_update_metadata: group_idx %d, idx %d",
                         group_idx, idx);
    return true;
error:
    PROTOCOL_PRINT_ERROR1("base_data_update_metadata: failed, error_idx %d", error_idx);
    return false;
}

bool base_data_get_bis_num(uint8_t group_idx, uint8_t *p_bis_num)
{
    T_BASE_DATA_GROUP *p_base_group = NULL;

    p_base_group = base_data_find_group(group_idx);
    if (p_base_group == NULL)
    {
        return false;
    }
    if (p_bis_num)
    {
        *p_bis_num =  p_base_group->num_bis;
    }
    return true;
}

#if LE_AUDIO_DEINIT
void base_data_gen_deinit(void)
{
    T_BASE_DATA_GROUP *p_base_group;
    T_BASE_DATA_SUBGROUP *p_subgroup;

    while ((p_base_group = os_queue_out(&base_group_queue)) != NULL)
    {
        while ((p_subgroup = os_queue_out(&p_base_group->subgroup_queue)) != NULL)
        {
            base_data_del_subgroup(p_base_group, p_subgroup);
        }
        ble_audio_mem_free(p_base_group);
    }
    base_group_idx = 0;
}
#endif
#endif
