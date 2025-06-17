#include <string.h>
#include "trace.h"
#include "pac_mgr.h"
#include "bt_types.h"
#include "pacs_def.h"
#include "codec_def.h"
#include "codec_int.h"
#include "ble_audio_mgr.h"

#if LE_AUDIO_PACS_SUPPORT
T_PAC_RECORD_LIST *p_pac_list = NULL;
uint8_t pac_list_num = 0;
uint8_t sink_pac_num = 0;
uint8_t source_pac_num = 0;
uint8_t pac_char_attrib_num = 0;

T_PAC_RECORD *pac_find_by_id(uint8_t pac_id)
{
    T_PAC_RECORD_LIST *p_loop = p_pac_list;

    while (p_loop != NULL)
    {
        if (p_loop->pac_record.pac_id == pac_id)
        {
            return (&(p_loop->pac_record));
        }
        p_loop = p_loop->next;
    }

    return ((T_PAC_RECORD *)NULL);
}

T_PAC_RECORD *pac_find_by_index(uint16_t attrib_index, bool cccd)
{
    T_PAC_RECORD_LIST *p_loop = p_pac_list;
    uint16_t pac_attrib_idx;

    while (p_loop != NULL)
    {
        if (cccd)
        {
            pac_attrib_idx = p_loop->pac_record.attrib_cccd_idx;
        }
        else
        {
            pac_attrib_idx = p_loop->pac_record.attrib_idx;
        }

        if (pac_attrib_idx == attrib_index)
        {
            return (&(p_loop->pac_record));
        }
        p_loop = p_loop->next;
    }
    return ((T_PAC_RECORD *)NULL);
}

bool pac_find_record(const uint8_t *pac_data,  uint8_t pac_data_len, uint8_t *codec_id,
                     T_CODEC_CFG *p_cfg)
{
    uint8_t i = 0;
    uint8_t idx = 0;
    uint8_t codec_spec_cap_len = 0;
    uint8_t metadata_len = 0;
    uint8_t pac_record_num = 0;
    if (pac_data == NULL || pac_data_len < PAC_RECORD_MIN_LEN || p_cfg == NULL || codec_id == NULL)
    {
        return false;
    }
    pac_record_num = pac_data[idx++];
    for (i = 0; i < pac_record_num; i++)
    {
        if (memcmp(pac_data + idx, codec_id, CODEC_ID_LEN) == 0)
        {
            idx += CODEC_ID_LEN;
            codec_spec_cap_len = pac_data[idx++];
            if (codec_cap_check(codec_spec_cap_len, (uint8_t *)&pac_data[idx], p_cfg))
            {
                return true;
            }
            idx += codec_spec_cap_len;
        }
        else
        {
            idx += CODEC_ID_LEN;
            codec_spec_cap_len = pac_data[idx++];
            idx += codec_spec_cap_len;
        }
        metadata_len = pac_data[idx++];
        idx += metadata_len;
    }
    return false;
}

T_PAC_RECORD *pac_find_by_codec_data(uint8_t direction, uint8_t *codec_id, T_CODEC_CFG *p_cfg)
{
    T_PAC_RECORD_LIST *p_loop = p_pac_list;

    while (p_loop != NULL)
    {
        if (p_loop->pac_record.direction == direction)
        {
            if (pac_find_record(p_loop->pac_record.pac_data, p_loop->pac_record.pac_data_len, codec_id, p_cfg))
            {
                return (&(p_loop->pac_record));
            }
        }
        p_loop = p_loop->next;
    }
    PROTOCOL_PRINT_ERROR1("pac_find_by_codec_data: not find, direction %d", direction);
    return ((T_PAC_RECORD *)NULL);
}

bool pacs_check_codec_cfg(uint8_t direction, uint8_t *codec_id, T_CODEC_CFG *p_cfg)
{
    if (pac_find_by_codec_data(direction, codec_id, p_cfg) == NULL)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool pac_check_record(const uint8_t *pac_data,  uint8_t pac_data_len)
{
    uint8_t i = 0;
    uint8_t idx = 0;
    uint8_t len = 0;
    uint8_t error_idx = 0;
    uint8_t data_len = 0;
    uint8_t codec_spec_cap_len = 0;
    uint8_t metadata_len = 0;
    uint8_t pac_record_num = 0;
    if (pac_data == NULL || pac_data_len < PAC_RECORD_MIN_LEN)
    {
        error_idx = 1;
        goto error;
    }
    pac_record_num = pac_data[idx++];
    for (i = 0; i < pac_record_num; i++)
    {
        idx += CODEC_ID_LEN;
        codec_spec_cap_len = pac_data[idx++];

        for (data_len = 0; data_len < codec_spec_cap_len;)
        {
            len = pac_data[idx];
            idx += (len + 1);
            data_len += (len + 1);
        }

        if (data_len != codec_spec_cap_len)
        {
            error_idx = 2;
            goto error;
        }

        metadata_len = pac_data[idx++];
        data_len = 0;
        for (; data_len < metadata_len;)
        {
            len = pac_data[idx];
            idx += (len + 1);
            data_len += (len + 1);
        }
        if (data_len != metadata_len)
        {
            error_idx = 3;
            goto error;
        }
        if (idx > pac_data_len)
        {
            error_idx = 4;
            goto error;
        }
    }

    if (idx != pac_data_len)
    {
        error_idx = 5;
        goto error;
    }
    return true;
error:
    PROTOCOL_PRINT_ERROR1("pac_check_record: invalid format error_idx %d", error_idx);
    return false;
}

bool pacs_gen_pac_records(uint16_t attrib_index, uint8_t **pp_data,
                          uint16_t *p_data_len)
{
    T_PAC_RECORD_LIST *p_loop = p_pac_list;

    while (p_loop != NULL)
    {
        if (p_loop->pac_record.attrib_idx == attrib_index)
        {
            *pp_data = p_loop->pac_record.pac_data;
            *p_data_len = p_loop->pac_record.pac_data_len;
            return true;
        }
        p_loop = p_loop->next;
    }
    return false;
}

uint8_t pacs_pac_add(T_AUDIO_DIRECTION direction, const uint8_t *pac_data,
                     uint8_t pac_data_len, bool notify)
{
    T_PAC_RECORD_LIST  *p_new_item;
    T_PAC_RECORD pac_record = {0};

    if (p_pacs != NULL)
    {
        PROTOCOL_PRINT_ERROR0("pacs_pac_add: failed, srv already registered");
        return PAC_RECORD_INVALID_ID;
    }

    if (pac_check_record(pac_data, pac_data_len) == false)
    {
        PROTOCOL_PRINT_ERROR0("pacs_pac_add: parse record failed");
        return PAC_RECORD_INVALID_ID;
    }

    p_new_item = ble_audio_mem_zalloc(sizeof(T_PAC_RECORD_LIST));
    if (p_new_item == NULL)
    {
        PROTOCOL_PRINT_ERROR0("pacs_pac_add: allocate men failed");
        return PAC_RECORD_INVALID_ID;
    }
    p_new_item->next = NULL;
    pac_record.pac_id = pac_list_num;
    pac_record.notify = notify;
    pac_record.direction = direction;
    pac_record.pac_data = (uint8_t *)pac_data;
    pac_record.pac_data_len = pac_data_len;
    memcpy(&(p_new_item->pac_record), &pac_record, sizeof(T_PAC_RECORD));
    if (direction == SERVER_AUDIO_SOURCE)
    {
        source_pac_num++;
    }
    else if (direction == SERVER_AUDIO_SINK)
    {
        sink_pac_num++;
    }
    if (notify)
    {
        pac_char_attrib_num += 3;
    }
    else
    {
        pac_char_attrib_num += 2;
    }

    // Find spot in list
    if (p_pac_list == NULL)
    {
        // First item in list
        p_pac_list = p_new_item;
    }
    else
    {
        T_PAC_RECORD_LIST *p_loop = p_pac_list;
        while (p_loop->next != NULL)
        {
            p_loop = p_loop->next;
        }
        p_loop->next = p_new_item;
    }

    pac_list_num++;
    return p_new_item->pac_record.pac_id;
}

bool pac_update(uint8_t pac_id, const uint8_t *pac_data, uint8_t pac_data_len)
{
    T_PAC_RECORD *p_record;
    uint8_t error_type = 0;

    p_record = pac_find_by_id(pac_id);
    if (p_record == NULL || pac_data == NULL)
    {
        error_type = 1;
        goto error;
    }
    if (p_record->notify == false)
    {
        error_type = 2;
        goto error;
    }
    if (pac_check_record(pac_data, pac_data_len))
    {
        p_record->pac_data = (uint8_t *)pac_data;
        p_record->pac_data_len = pac_data_len;
    }
    else
    {
        error_type = 3;
        goto error;
    }
    PROTOCOL_PRINT_INFO1("[BAP][PACS] pac_update: pac_id %d", pac_id);
    return true;
error:
    PROTOCOL_PRINT_ERROR1("[BAP][PACS] pac_update: failed, error_type %d", error_type);
    return false;
}

#if LE_AUDIO_DEBUG
void pac_print_info(void)
{
    T_PAC_RECORD_LIST *p_loop = p_pac_list;

    while (p_loop != NULL)
    {
        PROTOCOL_PRINT_TRACE3("[BAP][PACS] pac_print_info[%d]: %p, next %p",
                              p_loop->pac_record.pac_id,
                              p_loop,
                              p_loop->next);
        PROTOCOL_PRINT_TRACE6("[BAP][PACS] notify %d, direction %d, pac_data[%d] %b, attrib_idx %d, attrib_cccd_idx %d",
                              p_loop->pac_record.notify,
                              p_loop->pac_record.direction,
                              p_loop->pac_record.pac_data_len,
                              TRACE_BINARY(p_loop->pac_record.pac_data_len, p_loop->pac_record.pac_data),
                              p_loop->pac_record.attrib_idx,
                              p_loop->pac_record.attrib_cccd_idx);
        p_loop = p_loop->next;
    }
}
#endif
#endif
