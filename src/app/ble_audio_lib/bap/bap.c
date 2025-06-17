#include <string.h>
#include "trace.h"
#include "ble_audio_mgr.h"
#include "bap.h"
#include "bap_int.h"
#include "ble_link_util.h"
#include "ble_audio.h"
#include "bt_gatt_client.h"
#include "ble_audio_dm.h"
#include "pacs_mgr.h"
#include "os_queue.h"
#include "codec_qos.h"
#include "bass_client.h"
#include "gap_pa_adv.h"
#include "gap_pa_sync.h"
#include "gap_cig_mgr.h"
#include "gap_ext_adv.h"
#include "cap_int.h"
#include "ascs_client.h"
#include "pacs_client_int.h"

#if LE_AUDIO_BAP_SUPPORT
T_BAP_DB bap_db;

extern bool codec_cap_get_cfg_bits(uint32_t *p_cfg_bits, T_CODEC_CAP *p_cap);

#if (LE_AUDIO_BASS_CLIENT_SUPPORT || LE_AUDIO_PACS_CLIENT_SUPPORT || LE_AUDIO_ASCS_CLIENT_SUPPORT)
T_BAP_LINK_CB *bap_check_link_int(const char *p_func_name, uint16_t conn_handle)
{
    T_BLE_AUDIO_LINK *p_link = NULL;

    p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL || p_link->p_bap_cb == NULL)
    {
        PROTOCOL_PRINT_ERROR2("bap_check_link: %s failed, conn_handle 0x%x", TRACE_STRING(p_func_name),
                              conn_handle);
        return NULL;
    }
    return p_link->p_bap_cb;
}

static bool bap_link_free(T_BAP_LINK_CB *p_bap_cb)
{
    if (p_bap_cb != NULL)
    {
#if LE_AUDIO_PACS_CLIENT_SUPPORT
        T_BAP_PAC_ELEMENT *p_pac;
        while ((p_pac = os_queue_out(&p_bap_cb->pacs_cb.snk_pac_queue)) != NULL)
        {
            if (p_pac->pac_record.p_codec_cap)
            {
                ble_audio_mem_free(p_pac->pac_record.p_codec_cap);
            }
            if (p_pac->pac_record.p_metadata)
            {
                ble_audio_mem_free(p_pac->pac_record.p_metadata);
            }
            ble_audio_mem_free(p_pac);
        }
        while ((p_pac = os_queue_out(&p_bap_cb->pacs_cb.src_pac_queue)) != NULL)
        {
            if (p_pac->pac_record.p_codec_cap)
            {
                ble_audio_mem_free(p_pac->pac_record.p_codec_cap);
            }
            if (p_pac->pac_record.p_metadata)
            {
                ble_audio_mem_free(p_pac->pac_record.p_metadata);
            }
            ble_audio_mem_free(p_pac);
        }
#endif
#if LE_AUDIO_ASCS_CLIENT_SUPPORT
        if (p_bap_cb->p_ascs_cb)
        {
            ble_audio_mem_free(p_bap_cb->p_ascs_cb);
        }
#endif
        return true;
    }

    return false;
}

void bap_send_discovery_result(T_BAP_LINK_CB *p_bap_cb, uint16_t cause)
{
    if ((p_bap_cb->disc_flags == bap_db.disc_flags &&
         p_bap_cb->disc_done == false) || cause != GAP_SUCCESS)
    {
        T_BAP_DIS_ALL_DONE dis_info = {0};

        dis_info.conn_handle = p_bap_cb->conn_handle;
        if (cause == GAP_SUCCESS)
        {
#if LE_AUDIO_PACS_CLIENT_SUPPORT
            if (p_bap_cb->pacs_cb.value_exist)
            {
                dis_info.pacs_is_found = true;
                dis_info.sink_pac_num = p_bap_cb->pacs_cb.snk_pac_queue.count;
                dis_info.source_pac_num = p_bap_cb->pacs_cb.src_pac_queue.count;
            }
#endif
#if LE_AUDIO_ASCS_CLIENT_SUPPORT
            if (p_bap_cb->p_ascs_cb)
            {
                dis_info.ascs_is_found = true;
                dis_info.sink_ase_num = p_bap_cb->p_ascs_cb->snk_ase_num;
                dis_info.source_ase_num = p_bap_cb->p_ascs_cb->src_ase_num;
            }
#endif
#if LE_AUDIO_BASS_CLIENT_SUPPORT
            if (p_bap_cb->brs_char_num)
            {
                dis_info.bass_is_found = true;
                dis_info.brs_char_num = p_bap_cb->brs_char_num;
            }
#endif
            PROTOCOL_PRINT_INFO7("bap_send_discovery_result: conn_handle 0x%x, pacs_is_found %d, sink_pac_num %d, source_pac_num %d, ascs_is_found %d, sink_ase_num %d, source_ase_num %d",
                                 dis_info.conn_handle, dis_info.pacs_is_found,
                                 dis_info.sink_pac_num, dis_info.source_pac_num,
                                 dis_info.ascs_is_found, dis_info.sink_ase_num, dis_info.source_ase_num);
#if LE_AUDIO_ASCS_CLIENT_SUPPORT
            if (p_bap_cb->pacs_cb.value_exist && dis_info.ascs_is_found)
            {
                bap_unicast_client_check_session(dis_info.conn_handle);
            }
#endif
        }
        else
        {
            PROTOCOL_PRINT_ERROR2("bap_send_discovery_result: failed, conn_handle 0x%x, cause 0x%x",
                                  dis_info.conn_handle, cause);
        }

        p_bap_cb->disc_done = true;
        dis_info.cause = cause;
        ble_audio_mgr_dispatch(LE_AUDIO_MSG_BAP_DIS_ALL_DONE, &dis_info);
    }
}
#endif

#if LE_AUDIO_PACS_CLIENT_SUPPORT
static void bap_pac_record_remove(T_BAP_LINK_CB *p_bap_cb, uint16_t handle, uint8_t role)
{
    T_OS_QUEUE temp_queue;
    T_BAP_PAC_ELEMENT *p_pac;
    os_queue_init(&temp_queue);

    if (role == PACS_SINK_PAC)
    {
        while ((p_pac = os_queue_out(&p_bap_cb->pacs_cb.snk_pac_queue)) != NULL)
        {
            if (p_pac->pac_record.pac_handle == handle)
            {
                if (p_pac->pac_record.p_codec_cap)
                {
                    ble_audio_mem_free(p_pac->pac_record.p_codec_cap);
                }

                if (p_pac->pac_record.p_metadata)
                {
                    ble_audio_mem_free(p_pac->pac_record.p_metadata);
                }
                ble_audio_mem_free(p_pac);
            }
            else
            {
                os_queue_in(&temp_queue, p_pac);
            }
        }
        memcpy(&p_bap_cb->pacs_cb.snk_pac_queue, &temp_queue, sizeof(temp_queue));
    }
    else
    {
        while ((p_pac = os_queue_out(&p_bap_cb->pacs_cb.src_pac_queue)) != NULL)
        {
            if (p_pac->pac_record.pac_handle == handle)
            {
                if (p_pac->pac_record.p_codec_cap)
                {
                    ble_audio_mem_free(p_pac->pac_record.p_codec_cap);
                }

                if (p_pac->pac_record.p_metadata)
                {
                    ble_audio_mem_free(p_pac->pac_record.p_metadata);
                }
                ble_audio_mem_free(p_pac);
            }
            else
            {
                os_queue_in(&temp_queue, p_pac);
            }
        }
        memcpy(&p_bap_cb->pacs_cb.src_pac_queue, &temp_queue, sizeof(temp_queue));
    }
}
static void bap_pac_record_parse(T_BAP_LINK_CB *p_bap_cb, T_PAC_CHAR_DATA *pac_data, uint8_t role)
{
    uint8_t error_idx = 0;
    T_BAP_PAC_ELEMENT *p_pac = NULL;
    T_BAP_PAC_RECORD  *p_pac_record = NULL;
    uint8_t *p_record = pac_data->p_record;
    uint16_t pac_record_len = pac_data->pac_record_len;
    if (p_record != NULL && pac_record_len != 0)
    {
        uint8_t i = 0;
        uint16_t idx = 0;
        uint8_t codec_spec_cap_len = 0;
        uint8_t metadata_len = 0;
        uint8_t *p_codec_id;
        uint8_t len = 0;
        uint8_t data_len = 0;
        uint8_t type;
        uint8_t pac_record_num = p_record[0];
        idx++;
        for (i = 0; i < pac_record_num; i++)
        {
            bool unknown_metadata = false;
            uint8_t *p_metadata = NULL;
            p_codec_id = &p_record[idx];
            idx += CODEC_ID_LEN;
            codec_spec_cap_len = p_record[idx++];

            p_pac = ble_audio_mem_zalloc(sizeof(T_BAP_PAC_ELEMENT));
            if (p_pac == NULL)
            {
                error_idx = 1;
                goto error;
            }
            p_pac_record = &p_pac->pac_record;
            memcpy(p_pac_record->codec_id, p_codec_id, CODEC_ID_LEN);
            if (codec_spec_cap_len != 0)
            {
                if (codec_cap_parse(codec_spec_cap_len, &p_record[idx], &p_pac_record->codec_cap))
                {
                    if (p_pac_record->codec_cap.type_exist & CODEC_CAP_VENDOR_INFO_EXIST)
                    {
                        p_pac_record->p_codec_cap = ble_audio_mem_zalloc(codec_spec_cap_len);
                        if (p_pac_record->p_codec_cap == NULL)
                        {
                            error_idx = 9;
                            goto error;
                        }
                        p_pac_record->codec_cap_length = codec_spec_cap_len;
                        memcpy(p_pac_record->p_codec_cap, &p_record[idx], codec_spec_cap_len);
                    }
                    idx += codec_spec_cap_len;
                }
                else
                {
                    error_idx = 2;
                    goto error;
                }
            }

            metadata_len = p_record[idx++];
            data_len = 0;
            p_metadata = &p_record[idx];
            for (; data_len < metadata_len;)
            {
                len = p_record[idx++];
                type = p_record[idx++];
                if (type == METADATA_TYPE_PREFERRED_AUDIO_CONTEXTS)
                {
                    if (len != 3)
                    {
                        error_idx = 3;
                        goto error;
                    }
                    LE_ARRAY_TO_UINT16(p_pac_record->pref_audio_contexts, p_record + idx);
                }
                else
                {
                    unknown_metadata = true;
                }
                idx += (len - 1);
                data_len += (len + 1);
            }
            if (data_len != metadata_len)
            {
                error_idx = 5;
                goto error;
            }
            if (idx > pac_record_len)
            {
                error_idx = 6;
                goto error;
            }
            p_pac_record->pac_handle = pac_data->handle;
            if (p_pac_record->codec_id[0] == LC3_CODEC_ID)
            {
                codec_cap_get_cfg_bits(&p_pac_record->lc3_sup_cfg_bits, &p_pac_record->codec_cap);
            }
            if (unknown_metadata)
            {
                p_pac_record->p_metadata = ble_audio_mem_zalloc(metadata_len);
                if (p_pac_record->p_metadata == NULL)
                {
                    error_idx = 7;
                    goto error;
                }
                p_pac_record->metadata_length = metadata_len;
                memcpy(p_pac_record->p_metadata, p_metadata, metadata_len);
            }
            if (role == PACS_SINK_PAC)
            {
                os_queue_in(&p_bap_cb->pacs_cb.snk_pac_queue, (void *)p_pac);
            }
            else
            {
                os_queue_in(&p_bap_cb->pacs_cb.src_pac_queue, (void *)p_pac);
            }
        }
    }
    return;
error:
    if (p_pac != NULL)
    {
        if (p_pac->pac_record.p_metadata)
        {
            ble_audio_mem_free(p_pac->pac_record.p_metadata);
        }
        ble_audio_mem_free(p_pac);
    }
    PROTOCOL_PRINT_ERROR1("bap_pac_record_parse: invalid format error_idx %d",
                          error_idx);
}

void bap_handle_pacs_char_value(T_BAP_LINK_CB *p_bap_cb, bool is_notify, T_PACS_TYPE type,
                                T_PACS_DATA *p_pacs_data)
{
    switch (type)
    {
    case PACS_AUDIO_SUPPORTED_CONTEXTS:
        p_bap_cb->pacs_cb.snk_sup_context = p_pacs_data->contexts_data.sink_contexts;
        p_bap_cb->pacs_cb.src_sup_context = p_pacs_data->contexts_data.source_contexts;
        p_bap_cb->pacs_cb.value_exist |= PACS_AUDIO_SUPPORTED_CONTEXTS_EXIST;
        break;
    case PACS_AUDIO_AVAILABLE_CONTEXTS:
        p_bap_cb->pacs_cb.snk_avail_context = p_pacs_data->contexts_data.sink_contexts;
        p_bap_cb->pacs_cb.src_avail_context = p_pacs_data->contexts_data.source_contexts;
        p_bap_cb->pacs_cb.value_exist |= PACS_AUDIO_AVAILABLE_CONTEXTS_EXIST;
        break;
    case PACS_SINK_AUDIO_LOC:
        p_bap_cb->pacs_cb.snk_audio_loc = p_pacs_data->audio_locations;
        p_bap_cb->pacs_cb.value_exist |= PACS_SINK_AUDIO_LOC_EXIST;
        break;
    case PACS_SOURCE_AUDIO_LOC:
        p_bap_cb->pacs_cb.src_audio_loc = p_pacs_data->audio_locations;
        p_bap_cb->pacs_cb.value_exist |= PACS_SOURCE_AUDIO_LOC_EXIST;
        break;

    case PACS_SINK_PAC:
        if (is_notify == false)
        {
            if (p_pacs_data->pac_data.is_complete)
            {
                PROTOCOL_PRINT_INFO0("bap_msg_cb, read pacs sink pac complete");
                p_bap_cb->pacs_cb.value_exist |= PACS_SINK_PAC_EXIST;
            }
            bap_pac_record_parse(p_bap_cb, &p_pacs_data->pac_data, PACS_SINK_PAC);
        }
        else
        {
            bap_pac_record_remove(p_bap_cb, p_pacs_data->pac_data.handle, PACS_SINK_PAC);
            bap_pac_record_parse(p_bap_cb, &p_pacs_data->pac_data, PACS_SINK_PAC);
        }
        break;

    case PACS_SOURCE_PAC:
        if (is_notify == false)
        {
            if (p_pacs_data->pac_data.is_complete)
            {
                PROTOCOL_PRINT_INFO0("bap_msg_cb, read pacs source pac complete");
                p_bap_cb->pacs_cb.value_exist |= PACS_SOURCE_PAC_EXIST;
            }
            bap_pac_record_parse(p_bap_cb, &p_pacs_data->pac_data, PACS_SOURCE_PAC);
        }
        else
        {
            bap_pac_record_remove(p_bap_cb, p_pacs_data->pac_data.handle, PACS_SOURCE_PAC);
            bap_pac_record_parse(p_bap_cb, &p_pacs_data->pac_data, PACS_SOURCE_PAC);
        }
        break;

    default:
        break;
    }
    if (is_notify == false)
    {
        if (p_bap_cb->pacs_cb.read_req == p_bap_cb->pacs_cb.value_exist)
        {
            p_bap_cb->pacs_cb.read_req = 0;
            p_bap_cb->disc_flags |= BAP_DISC_PACS;
            bap_send_discovery_result(p_bap_cb, GAP_SUCCESS);
        }
    }
    else
    {
        T_BAP_PACS_NOTIFY pacs_notify;
        pacs_notify.pacs_type = type;
        pacs_notify.conn_handle = p_bap_cb->conn_handle;
        if (type == PACS_SINK_PAC || type == PACS_SOURCE_PAC)
        {
            pacs_notify.pac_handle = p_pacs_data->pac_data.handle;
        }
        else
        {
            pacs_notify.pac_handle = 0;
        }
        ble_audio_mgr_dispatch(LE_AUDIO_MSG_BAP_PACS_NOTIFY, &pacs_notify);
    }
}

bool bap_pacs_get_info(uint16_t conn_handle, T_BAP_PACS_INFO *p_pacs_info)
{
    T_BAP_LINK_CB *p_bap_cb = bap_check_link(conn_handle);
    if (p_bap_cb == NULL || p_pacs_info == NULL)
    {
        PROTOCOL_PRINT_ERROR0("bap_pacs_get_info: not find the link");
        return false;
    }
    memset(p_pacs_info, 0, sizeof(T_BAP_PACS_INFO));
    p_pacs_info->value_exist = p_bap_cb->pacs_cb.value_exist;
    p_pacs_info->sink_pac_num = p_bap_cb->pacs_cb.snk_pac_queue.count;
    p_pacs_info->source_pac_num = p_bap_cb->pacs_cb.src_pac_queue.count;
    p_pacs_info->snk_audio_loc = p_bap_cb->pacs_cb.snk_audio_loc;
    p_pacs_info->src_audio_loc = p_bap_cb->pacs_cb.src_audio_loc;
    p_pacs_info->snk_sup_context = p_bap_cb->pacs_cb.snk_sup_context;
    p_pacs_info->src_sup_context = p_bap_cb->pacs_cb.src_sup_context;
    p_pacs_info->snk_avail_context = p_bap_cb->pacs_cb.snk_avail_context;
    p_pacs_info->src_avail_context = p_bap_cb->pacs_cb.src_avail_context;
    return true;
}

bool bap_pacs_get_pac_record(uint16_t conn_handle, T_AUDIO_DIRECTION direction,
                             uint8_t *p_pac_num, T_BAP_PAC_RECORD *p_pac_tbl)
{
    T_BAP_LINK_CB *p_bap_cb = bap_check_link(conn_handle);
    if (p_bap_cb == NULL || p_pac_num == NULL || p_pac_tbl == NULL)
    {
        goto failed;
    }
    if (direction == SERVER_AUDIO_SINK)
    {
        if (*p_pac_num < p_bap_cb->pacs_cb.snk_pac_queue.count)
        {
            goto failed;
        }
        for (int i = 0; i < p_bap_cb->pacs_cb.snk_pac_queue.count; i++)
        {
            T_BAP_PAC_ELEMENT *p_pac = os_queue_peek(&p_bap_cb->pacs_cb.snk_pac_queue, i);
            if (p_pac == NULL)
            {
                goto failed;
            }
            memcpy(&p_pac_tbl[i], &p_pac->pac_record, sizeof(T_BAP_PAC_RECORD));
        }
        *p_pac_num = p_bap_cb->pacs_cb.snk_pac_queue.count;
    }
    else
    {
        if (*p_pac_num < p_bap_cb->pacs_cb.src_pac_queue.count)
        {
            goto failed;
        }
        for (int i = 0; i < p_bap_cb->pacs_cb.src_pac_queue.count; i++)
        {
            T_BAP_PAC_ELEMENT *p_pac = os_queue_peek(&p_bap_cb->pacs_cb.src_pac_queue, i);
            if (p_pac == NULL)
            {
                goto failed;
            }
            memcpy(&p_pac_tbl[i], &p_pac->pac_record, sizeof(T_BAP_PAC_RECORD));
        }
        *p_pac_num = p_bap_cb->pacs_cb.src_pac_queue.count;
    }
    return true;
failed:
    PROTOCOL_PRINT_ERROR0("bap_pacs_get_pac_record: error");
    if (p_pac_num)
    {
        *p_pac_num = 0;
    }
    return false;
}

bool bap_pacs_get_pac_record_by_handle(uint16_t conn_handle, uint16_t pac_handle,
                                       uint8_t *p_pac_num, T_BAP_PAC_RECORD *p_pac_tbl)
{
    T_BAP_LINK_CB *p_bap_cb = bap_check_link(conn_handle);
    T_BAP_PAC_ELEMENT *p_pac;
    uint8_t idx = 0;

    if (p_bap_cb == NULL || p_pac_num == NULL || p_pac_tbl == NULL || *p_pac_num == 0)
    {
        goto failed;
    }
    for (int i = 0; i < p_bap_cb->pacs_cb.snk_pac_queue.count; i++)
    {
        p_pac = os_queue_peek(&p_bap_cb->pacs_cb.snk_pac_queue, i);
        if (p_pac && p_pac->pac_record.pac_handle == pac_handle)
        {
            memcpy(&p_pac_tbl[idx], &p_pac->pac_record, sizeof(T_BAP_PAC_RECORD));
            idx++;
            if (idx > *p_pac_num)
            {
                goto failed;
            }
        }
    }
    if (idx == 0)
    {
        for (int i = 0; i < p_bap_cb->pacs_cb.src_pac_queue.count; i++)
        {
            p_pac = os_queue_peek(&p_bap_cb->pacs_cb.src_pac_queue, i);
            if (p_pac && p_pac->pac_record.pac_handle == pac_handle)
            {
                memcpy(&p_pac_tbl[idx], &p_pac->pac_record, sizeof(T_BAP_PAC_RECORD));
                idx++;
                if (idx > *p_pac_num)
                {
                    goto failed;
                }
            }
        }
    }
    if (idx == 0)
    {
        goto failed;
    }
    *p_pac_num = idx;
    return true;
failed:
    PROTOCOL_PRINT_ERROR0("bap_pacs_get_pac_record_by_handle: error");
    if (p_pac_num)
    {
        *p_pac_num = 0;
    }
    return false;
}

uint32_t bap_pacs_get_lc3_snk_table_msk(uint16_t conn_handle, uint16_t pref_audio_contexts,
                                        uint8_t channel_count,
                                        uint8_t block_num)
{
    uint32_t table_msk = 0;
    T_BAP_LINK_CB *p_bap_cb = bap_check_link(conn_handle);
    uint8_t audio_channel_counts = 0;
    uint8_t max_frame_num;

    if (p_bap_cb == NULL)
    {
        PROTOCOL_PRINT_ERROR0("bap_pacs_get_lc3_snk_table_msk: not find the link");
        return 0;
    }
    if (pref_audio_contexts != 0 &&
        (pref_audio_contexts & p_bap_cb->pacs_cb.snk_avail_context) == 0)
    {
        PROTOCOL_PRINT_ERROR3("bap_pacs_get_lc3_snk_table_msk: audio context not available, conn_handle 0x%x, pref_audio_contexts 0x%x, snk_avail_context 0x%x",
                              conn_handle, pref_audio_contexts, p_bap_cb->pacs_cb.snk_avail_context);
        return 0;
    }
    max_frame_num = channel_count * block_num;
    audio_channel_counts = (1 << (channel_count - 1));

    for (int i = 0; i < p_bap_cb->pacs_cb.snk_pac_queue.count; i++)
    {
        T_BAP_PAC_ELEMENT *p_pac = os_queue_peek(&p_bap_cb->pacs_cb.snk_pac_queue, i);
        T_BAP_PAC_RECORD  *p_pac_record;
        if (p_pac == NULL)
        {
            continue;
        }
        p_pac_record = &p_pac->pac_record;

        if ((audio_channel_counts & p_pac_record->codec_cap.audio_channel_counts) == 0)
        {
            continue;
        }
        if (p_pac_record->codec_cap.max_supported_codec_frames_per_sdu != 0 &&
            max_frame_num > p_pac_record->codec_cap.max_supported_codec_frames_per_sdu)
        {
            continue;
        }

        if (pref_audio_contexts == AUDIO_CONTEXT_UNSPECIFIED || pref_audio_contexts == 0)
        {
            table_msk |= p_pac_record->lc3_sup_cfg_bits;
        }
        else if (pref_audio_contexts & p_pac_record->pref_audio_contexts)
        {
            table_msk |= p_pac_record->lc3_sup_cfg_bits;
        }
    }
    PROTOCOL_PRINT_INFO5("bap_pacs_get_lc3_snk_table_msk: conn_handle 0x%x, pref_audio_contexts 0x%x, channel_count %d, block_num %d, table_msk 0x%x",
                         conn_handle, pref_audio_contexts, channel_count, block_num, table_msk);
    return table_msk;
}

uint32_t bap_pacs_get_lc3_src_table_msk(uint16_t conn_handle, uint16_t pref_audio_contexts,
                                        uint8_t channel_count,
                                        uint8_t block_num)
{
    uint32_t table_msk = 0;
    T_BAP_LINK_CB *p_bap_cb = bap_check_link(conn_handle);
    uint8_t audio_channel_counts = 0;
    uint8_t max_frame_num;

    if (p_bap_cb == NULL)
    {
        PROTOCOL_PRINT_ERROR0("bap_pacs_get_lc3_src_table_msk: not find the link");
        return 0;
    }
    if (pref_audio_contexts != 0 &&
        (pref_audio_contexts & p_bap_cb->pacs_cb.src_avail_context) == 0)
    {
        PROTOCOL_PRINT_ERROR3("bap_pacs_get_lc3_src_table_msk: audio context not available, conn_handle 0x%x, pref_audio_contexts 0x%x, src_avail_context 0x%x",
                              conn_handle, pref_audio_contexts, p_bap_cb->pacs_cb.src_avail_context);
        return 0;
    }
    max_frame_num = channel_count * block_num;
    audio_channel_counts = (1 << (channel_count - 1));

    for (int i = 0; i < p_bap_cb->pacs_cb.src_pac_queue.count; i++)
    {
        T_BAP_PAC_ELEMENT *p_pac = os_queue_peek(&p_bap_cb->pacs_cb.src_pac_queue, i);
        T_BAP_PAC_RECORD  *p_pac_record;
        if (p_pac == NULL)
        {
            continue;
        }
        p_pac_record = &p_pac->pac_record;
        if ((audio_channel_counts & p_pac_record->codec_cap.audio_channel_counts) == 0)
        {
            continue;
        }
        if (p_pac_record->codec_cap.max_supported_codec_frames_per_sdu != 0 &&
            max_frame_num > p_pac_record->codec_cap.max_supported_codec_frames_per_sdu)
        {
            continue;
        }

        if (pref_audio_contexts == AUDIO_CONTEXT_UNSPECIFIED || pref_audio_contexts == 0)
        {
            table_msk |= p_pac_record->lc3_sup_cfg_bits;
        }
        else if (pref_audio_contexts & p_pac_record->pref_audio_contexts)
        {
            table_msk |= p_pac_record->lc3_sup_cfg_bits;
        }
    }
    PROTOCOL_PRINT_INFO5("bap_pacs_get_lc3_src_table_msk: conn_handle 0x%x, pref_audio_contexts 0x%x, channel_count %d, block_num %d, table_msk 0x%x",
                         conn_handle, pref_audio_contexts, channel_count, block_num, table_msk);
    return table_msk;
}

#if (LE_AUDIO_BROADCAST_ASSISTANT_ROLE && LE_AUDIO_PACS_CLIENT_SUPPORT)
uint32_t bap_pacs_check_cfg_by_base_data(T_BASE_DATA_MAPPING *p_base_mapping, uint16_t conn_handle,
                                         uint8_t filter)
{
    uint8_t err_idx = 0;
    uint32_t bis_array = 0;
    T_BAP_PAC_RECORD  *p_pac_record;
    T_BASE_DATA_BIS_PARAM *p_bis_param;
    T_BAP_LINK_CB *p_bap_cb = bap_check_link(conn_handle);
    if (p_bap_cb == NULL || (p_bap_cb->disc_flags & BAP_DISC_PACS) == 0)
    {
        err_idx = 1;
        goto error;
    }
    if (p_bap_cb->pacs_cb.snk_pac_queue.count == 0)
    {
        err_idx = 2;
        goto error;
    }
    if (p_base_mapping == NULL)
    {
        err_idx = 4;
        goto error;
    }

    for (uint8_t j = 0; j < p_base_mapping->num_subgroups; j++)
    {
        for (uint8_t k = 0; k < p_base_mapping->p_subgroup[j].num_bis; k++)
        {
            p_bis_param = &p_base_mapping->p_subgroup[j].p_bis_param[k];
            if (filter & BA_PACS_CHECK_FILTER_ALLOCATION)
            {
                if (p_bis_param->bis_codec_cfg.type_exist & CODEC_CFG_AUDIO_CHANNEL_ALLOCATION_EXIST)
                {
                    if ((p_bis_param->bis_codec_cfg.audio_channel_allocation & p_bap_cb->pacs_cb.snk_audio_loc) !=
                        p_bis_param->bis_codec_cfg.audio_channel_allocation)
                    {
                        PROTOCOL_PRINT_INFO3("bis[%d, %d]: allocation 0x%x not match",
                                             j, k, p_bis_param->bis_codec_cfg.audio_channel_allocation);
                        continue;
                    }
                }
            }
            for (uint8_t i = 0; i < p_bap_cb->pacs_cb.snk_pac_queue.count; i++)
            {
                T_BAP_PAC_ELEMENT *p_pac = os_queue_peek(&p_bap_cb->pacs_cb.snk_pac_queue, i);
                if (p_pac == NULL)
                {
                    err_idx = 5;
                    goto error;
                }
                p_pac_record = &p_pac->pac_record;
                if (memcmp(p_pac_record->codec_id, p_bis_param->codec_id, CODEC_ID_LEN) == 0)
                {
                    if (codec_cfg_check_cap(&p_pac_record->codec_cap, &p_bis_param->bis_codec_cfg))
                    {
                        bis_array |= (1 << (p_bis_param->bis_index - 1));
                        break;
                    }
                }
            }
        }
    }
    if (bis_array == 0)
    {
        err_idx = 6;
        goto error;
    }
    PROTOCOL_PRINT_INFO3("bap_pacs_check_cfg_by_base_data: conn_handle 0x%x, bis_array 0x%x, filter 0x%x",
                         conn_handle, bis_array, filter);
    return bis_array;
error:
    PROTOCOL_PRINT_ERROR2("bap_pacs_check_cfg_by_base_data: failed, conn_handle 0x%x, err_idx %d",
                          conn_handle, err_idx);
    return 0;
}

uint32_t bap_pacs_check_cfg_by_sync_info(T_BLE_AUDIO_SYNC_HANDLE handle, uint16_t conn_handle,
                                         uint8_t filter)
{
    T_BLE_AUDIO_SYNC_INFO sync_info;
    if (ble_audio_sync_get_info(handle, &sync_info))
    {
        if (sync_info.p_base_mapping)
        {
            return bap_pacs_check_cfg_by_base_data(sync_info.p_base_mapping, conn_handle, filter);
        }
    }
    return 0;
}
#endif
#endif
#if LE_AUDIO_ASCS_CLIENT_SUPPORT
T_BAP_ASE_ENTRY *bap_find_ase_by_ase_id(uint16_t conn_handle, uint8_t ase_id)
{
    T_BAP_LINK_CB *p_bap_cb = bap_check_link(conn_handle);
    if (p_bap_cb == NULL || p_bap_cb->p_ascs_cb == NULL)
    {
        PROTOCOL_PRINT_ERROR0("bap_find_ase_by_ase_id fail");
        return NULL;
    }
    for (int i = 0; i < (p_bap_cb->p_ascs_cb->snk_ase_num + p_bap_cb->p_ascs_cb->src_ase_num); i++)
    {
        if (p_bap_cb->p_ascs_cb->ase_data[i].used == true &&
            p_bap_cb->p_ascs_cb->ase_data[i].ase_id == ase_id)
        {
            return &p_bap_cb->p_ascs_cb->ase_data[i];
        }
    }
    return NULL;
}
#endif

#if (LE_AUDIO_BASS_CLIENT_SUPPORT || LE_AUDIO_PACS_CLIENT_SUPPORT || LE_AUDIO_ASCS_CLIENT_SUPPORT)
uint16_t bap_msg_cb(uint16_t msg, void *buf, bool *p_handled)
{
    uint16_t cb_result = BLE_AUDIO_CB_RESULT_SUCCESS;
    T_BAP_LINK_CB *p_bap_cb = NULL;

    switch (msg)
    {
#if LE_AUDIO_BASS_CLIENT_SUPPORT
    case LE_AUDIO_MSG_BASS_CLIENT_DIS_DONE:
        if (bap_db.disc_flags & BAP_DISC_BASS)
        {
            T_BASS_CLIENT_DIS_DONE *p_data = (T_BASS_CLIENT_DIS_DONE *)buf;

            if (p_data == NULL)
            {
                break;
            }
            p_bap_cb = bap_check_link(p_data->conn_handle);
            if (p_bap_cb == NULL)
            {
                break;
            }
            if (p_data->is_found)
            {
                p_bap_cb->brs_char_num = p_data->brs_char_num;
                if (p_data->load_from_ftl == false)
                {
                    bass_enable_cccd(p_data->conn_handle);
                }
                for (uint8_t i = 0; i < p_data->brs_char_num; i++)
                {
                    bass_read_brs_value(p_data->conn_handle, i);
                }
            }
            else
            {
                p_bap_cb->disc_flags |= BAP_DISC_BASS;
                bap_send_discovery_result(p_bap_cb, GAP_SUCCESS);
            }
        }
        break;

    case LE_AUDIO_MSG_BASS_CLIENT_BRS_DATA:
        if (bap_db.disc_flags & BAP_DISC_BASS)
        {
            T_BASS_CLIENT_BRS_DATA *p_data = (T_BASS_CLIENT_BRS_DATA *)buf;
            if (p_data == NULL)
            {
                break;
            }
            p_bap_cb = bap_check_link(p_data->conn_handle);
            if (p_bap_cb == NULL)
            {
                break;
            }
            if ((p_data->char_instance_id + 1) == p_bap_cb->brs_char_num)
            {
                p_bap_cb->disc_flags |= BAP_DISC_BASS;
                bap_send_discovery_result(p_bap_cb, GAP_SUCCESS);
            }
        }
        break;
#endif
#if LE_AUDIO_PACS_CLIENT_SUPPORT
    case LE_AUDIO_MSG_PACS_CLIENT_DIS_DONE:
        if (bap_db.disc_flags & BAP_DISC_PACS)
        {
            T_PACS_CLIENT_DIS_DONE *dis_done = (T_PACS_CLIENT_DIS_DONE *)buf;
            if (dis_done == NULL)
            {
                break;
            }
            p_bap_cb = bap_check_link(dis_done->conn_handle);
            if (p_bap_cb == NULL)
            {
                break;
            }
            if (dis_done->is_found)
            {
                p_bap_cb->pacs_cb.read_req = 0;
                pacs_enable_cccd(dis_done->conn_handle, PACS_OP_ALL);
                if (pacs_read_char_value(dis_done->conn_handle, PACS_AUDIO_SUPPORTED_CONTEXTS))
                {
                    p_bap_cb->pacs_cb.read_req |= PACS_AUDIO_SUPPORTED_CONTEXTS_EXIST;
                }
                if (pacs_read_char_value(dis_done->conn_handle, PACS_AUDIO_AVAILABLE_CONTEXTS))
                {
                    p_bap_cb->pacs_cb.read_req |= PACS_AUDIO_AVAILABLE_CONTEXTS_EXIST;
                }
                if (bap_db.role_msk & (BAP_BROADCAST_ASSISTANT_ROLE | BAP_UNICAST_CLT_SRC_ROLE))
                {
                    if (dis_done->sink_pac_num)
                    {
                        if (pacs_read_char_value(dis_done->conn_handle, PACS_SINK_PAC))
                        {
                            p_bap_cb->pacs_cb.read_req |= PACS_SINK_PAC_EXIST;
                        }
                        if (pacs_read_char_value(dis_done->conn_handle, PACS_SINK_AUDIO_LOC))
                        {
                            p_bap_cb->pacs_cb.read_req |= PACS_SINK_AUDIO_LOC_EXIST;
                        }
                    }
                }
                if (bap_db.role_msk & (BAP_UNICAST_CLT_SNK_ROLE))
                {
                    if (dis_done->source_pac_num)
                    {
                        if (pacs_read_char_value(dis_done->conn_handle, PACS_SOURCE_PAC))
                        {
                            p_bap_cb->pacs_cb.read_req |= PACS_SOURCE_PAC_EXIST;
                        }
                        if (pacs_read_char_value(dis_done->conn_handle, PACS_SOURCE_AUDIO_LOC))
                        {
                            p_bap_cb->pacs_cb.read_req |= PACS_SOURCE_AUDIO_LOC_EXIST;
                        }
                    }
                }
            }
            else
            {
                p_bap_cb->disc_flags |= BAP_DISC_PACS;
                bap_send_discovery_result(p_bap_cb, GAP_SUCCESS);
            }
        }
        break;

    case LE_AUDIO_MSG_PACS_CLIENT_READ_RESULT:
        if (bap_db.disc_flags & BAP_DISC_PACS)
        {
            T_PACS_CLIENT_READ_RESULT *p_read_result = (T_PACS_CLIENT_READ_RESULT *)buf;
            if (p_read_result != NULL)
            {
                p_bap_cb = bap_check_link(p_read_result->conn_handle);
                if (p_bap_cb == NULL || p_read_result->cause != GAP_SUCCESS)
                {
                    if (p_bap_cb && p_bap_cb->pacs_cb.read_req != 0)
                    {
                        p_bap_cb->pacs_cb.read_req = 0;
                        p_bap_cb->disc_flags |= BAP_DISC_PACS;
                        bap_send_discovery_result(p_bap_cb, GAP_SUCCESS);
                    }
                    break;
                }
                bap_handle_pacs_char_value(p_bap_cb, false, p_read_result->type, &p_read_result->data);
            }
        }
        break;
    case LE_AUDIO_MSG_PACS_CLIENT_NOTIFY:
        if (bap_db.disc_flags & BAP_DISC_PACS)
        {
            T_PACS_CLIENT_NOTIFY *p_notify_data = (T_PACS_CLIENT_NOTIFY *)buf;
            if (p_notify_data != NULL)
            {
                p_bap_cb =  bap_check_link(p_notify_data->conn_handle);
                if (p_bap_cb == NULL)
                {
                    break;
                }
                bap_handle_pacs_char_value(p_bap_cb, true, p_notify_data->type, &p_notify_data->data);
            }
        }
        break;
#endif
#if LE_AUDIO_ASCS_CLIENT_SUPPORT
    case LE_AUDIO_MSG_ASCS_CLIENT_DIS_DONE:
    case LE_AUDIO_MSG_ASCS_CLIENT_READ_ASE_DATA:
    case LE_AUDIO_MSG_ASCS_CLIENT_ASE_DATA_NOTIFY:
    case LE_AUDIO_MSG_ASCS_CLIENT_READ_ASE_ALL:
    case LE_AUDIO_MSG_ASCS_CLIENT_CP_NOTIFY:
    case LE_AUDIO_MSG_ASCS_CLIENT_SETUP_DATA_PATH:
    case LE_AUDIO_MSG_ASCS_CLIENT_REMOVE_DATA_PATH:
        if (bap_db.disc_flags & BAP_DISC_ASCS)
        {
            cb_result = bap_unicast_client_handle_ascs_msg(msg, buf, p_handled);
        }
        break;
#endif
    default:
        break;
    }
    return cb_result;
}
#endif

#if (LE_AUDIO_BASS_CLIENT_SUPPORT || LE_AUDIO_PACS_CLIENT_SUPPORT || LE_AUDIO_ASCS_CLIENT_SUPPORT)
static void bap_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BLE_DM_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BLE_DM_EVENT_CONN_STATE:
        {
            T_BLE_AUDIO_LINK *p_link = NULL;
            p_link = ble_audio_link_find_by_conn_id(param->conn_state.conn_id);
            if (p_link == NULL)
            {
                PROTOCOL_PRINT_ERROR1("bap_dm_cback: not find the link, conn_id %d",
                                      param->conn_state.conn_id);
                break;
            }
            if (param->conn_state.state == GAP_CONN_STATE_CONNECTED)
            {
                if (p_link->p_bap_cb == NULL)
                {
                    p_link->p_bap_cb = ble_audio_mem_zalloc(sizeof(T_BAP_LINK_CB));
                    if (p_link->p_bap_cb)
                    {
                        T_BAP_LINK_CB *p_bap_cb = (T_BAP_LINK_CB *)p_link->p_bap_cb;
                        p_bap_cb->conn_handle = p_link->conn_handle;
                    }
                }
            }
            else if (param->conn_state.state == GAP_CONN_STATE_DISCONNECTED)
            {
#if LE_AUDIO_ASCS_CLIENT_SUPPORT
                bap_unicast_client_handle_disconn_msg(p_link, param->conn_state.disc_cause);
#endif
                if (p_link->p_bap_cb != NULL)
                {
                    bap_link_free(p_link->p_bap_cb);
                    ble_audio_mem_free(p_link->p_bap_cb);
                    p_link->p_bap_cb = NULL;
                }
            }
        }
        break;

    default:
        break;
    }
}
#endif

bool bap_role_init(T_BAP_ROLE_INFO *p_role_info)
{
    uint8_t err_idx = 0;
    memset((uint8_t *)&bap_db, 0, sizeof(T_BAP_DB));

    if (p_role_info == NULL)
    {
        err_idx = 1;
        goto failed;
    }

    if (p_role_info->init_gap)
    {
#if F_BT_LE_5_2_ISOC_BIS_SUPPORT
        if ((p_role_info->isoc_big_broadcaster_num + p_role_info->isoc_big_receiver_num) != 0)
        {
            if (gap_big_mgr_init(4, GAP_BIG_MGR_MAX_BIS_NUM) != GAP_CAUSE_SUCCESS)
            {
                err_idx = 11;
                goto failed;
            }
        }
#endif
#if LE_AUDIO_BROADCAST_SOURCE_ROLE
        if (p_role_info->role_mask & BAP_BROADCAST_SOURCE_ROLE)
        {
            if (p_role_info->pa_adv_num == 0 || p_role_info->isoc_big_broadcaster_num == 0 ||
                p_role_info->isoc_bis_broadcaster_num == 0)
            {
                err_idx = 4;
                goto failed;
            }
#if F_BT_LE_5_2_ISOC_BIS_BROADCASTER_SUPPORT
            if (gap_big_mgr_isoc_broadcaster_init(p_role_info->isoc_big_broadcaster_num,
                                                  p_role_info->isoc_bis_broadcaster_num,
                                                  broadcast_source_isoc_broadcaster_cb) != GAP_CAUSE_SUCCESS)
            {
                err_idx = 5;
                goto failed;
            }
#endif
#if F_BT_LE_5_0_PA_ADV_SUPPORT
            if (le_pa_adv_init(p_role_info->pa_adv_num) != GAP_CAUSE_SUCCESS)
            {
                err_idx = 6;
                goto failed;
            }
#endif
        }
        else
        {
            p_role_info->pa_adv_num = 0;
            p_role_info->isoc_big_broadcaster_num = 0;
            p_role_info->isoc_bis_broadcaster_num = 0;
        }
#endif
#if (LE_AUDIO_BROADCAST_ASSISTANT_ROLE | LE_AUDIO_BROADCAST_SINK_ROLE)
        if (p_role_info->role_mask & (BAP_BROADCAST_ASSISTANT_ROLE | BAP_BROADCAST_SINK_ROLE))
        {
            if (p_role_info->pa_sync_num != 0)
            {
#if F_BT_LE_5_0_PA_SYNC_SUPPORT
                if (le_pa_sync_init(p_role_info->pa_sync_num) != GAP_CAUSE_SUCCESS)
                {
                    err_idx = 8;
                    goto failed;
                }
#endif
            }
        }
        else
        {
            p_role_info->pa_sync_num = 0;
        }
#endif

#if LE_AUDIO_BROADCAST_SINK_ROLE
        if (p_role_info->role_mask & BAP_BROADCAST_SINK_ROLE)
        {
            if (p_role_info->isoc_big_receiver_num == 0 ||
                p_role_info->isoc_bis_receiver_num == 0)
            {
                err_idx = 9;
                goto failed;
            }
#if F_BT_LE_5_2_ISOC_BIS_BROADCASTER_SUPPORT
            if (gap_big_mgr_sync_receiver_init(p_role_info->isoc_big_receiver_num,
                                               p_role_info->isoc_bis_receiver_num,
                                               ble_audio_big_sync_receiver_cb) != GAP_CAUSE_SUCCESS)
            {
                err_idx = 10;
                goto failed;
            }
#endif
        }
        else
        {
            p_role_info->isoc_big_receiver_num = 0;
            p_role_info->isoc_bis_receiver_num = 0;
        }
#endif

        if (p_role_info->role_mask & (BAP_UNICAST_CLT_SRC_ROLE | BAP_UNICAST_CLT_SRC_ROLE |
                                      BAP_UNICAST_SRV_SRC_ROLE | BAP_UNICAST_SRV_SNK_ROLE))
        {
            if (p_role_info->isoc_cis_max_num == 0)
            {
                err_idx = 12;
                goto failed;
            }
            if (p_role_info->role_mask & (BAP_UNICAST_CLT_SRC_ROLE | BAP_UNICAST_CLT_SNK_ROLE))
            {
                if (p_role_info->isoc_cig_max_num == 0)
                {
                    err_idx = 13;
                    goto failed;
                }
            }
            bool cis_flag = true;
            le_set_gap_param(GAP_PARAM_CIS_HOST_SUPPORT, sizeof(cis_flag), &cis_flag);
            cig_mgr_init(p_role_info->isoc_cig_max_num, p_role_info->isoc_cis_max_num);
        }
#if F_BT_LE_GAP_MSG_INFO_WAY
        le_ext_adv_gap_msg_info_way(false);
#endif
    }

#if (LE_AUDIO_BROADCAST_ASSISTANT_ROLE & LE_AUDIO_BASS_CLIENT_SUPPORT)
    if (p_role_info->role_mask & BAP_BROADCAST_ASSISTANT_ROLE)
    {
        bap_db.disc_flags |= BAP_DISC_BASS;
        bass_client_init();
    }
#endif

#if LE_AUDIO_SCAN_DELEGATOR_ROLE
    if (p_role_info->role_mask & BAP_SCAN_DELEGATOR_ROLE)
    {
        bass_init(p_role_info->brs_num);
    }
#endif

#if LE_AUDIO_ASCS_SUPPORT
    if (p_role_info->role_mask & (BAP_UNICAST_SRV_SRC_ROLE | BAP_UNICAST_SRV_SNK_ROLE))
    {
        if (p_role_info->snk_ase_num != 0 || p_role_info->src_ase_num != 0)
        {
            ascs_init(p_role_info->snk_ase_num, p_role_info->src_ase_num, p_role_info->init_gap);
        }
    }
#endif

#if(LE_AUDIO_ASCS_CLIENT_SUPPORT)
    if (p_role_info->role_mask & (BAP_UNICAST_CLT_SRC_ROLE | BAP_UNICAST_CLT_SNK_ROLE))
    {
        bap_db.disc_flags |= BAP_DISC_ASCS;
        ascs_client_init(p_role_info->role_mask & UNICAST_CLT_ROLE_MASK);
    }
#endif


#if(LE_AUDIO_PACS_CLIENT_SUPPORT)
    if (p_role_info->role_mask & (BAP_UNICAST_CLT_SRC_ROLE | BAP_BROADCAST_ASSISTANT_ROLE |
                                  BAP_UNICAST_CLT_SNK_ROLE))
    {
        bap_db.disc_flags |= BAP_DISC_PACS;
        pacs_client_init();
    }
#endif
#if (LE_AUDIO_BASS_CLIENT_SUPPORT || LE_AUDIO_PACS_CLIENT_SUPPORT || LE_AUDIO_ASCS_CLIENT_SUPPORT||LE_AUDIO_CSIS_CLIENT_SUPPORT ||LE_AUDIO_VCS_CLIENT_SUPPORT ||LE_AUDIO_MICS_CLIENT_SUPPORT)
    if (p_role_info->role_mask & (BAP_UNICAST_CLT_SRC_ROLE | BAP_BROADCAST_ASSISTANT_ROLE |
                                  BAP_UNICAST_CLT_SNK_ROLE))
    {
        ble_audio_reg_cap_cb(cap_msg_cb);
    }
#endif
    bap_db.role_msk = p_role_info->role_mask;
#if (LE_AUDIO_BASS_CLIENT_SUPPORT || LE_AUDIO_PACS_CLIENT_SUPPORT || LE_AUDIO_ASCS_CLIENT_SUPPORT)
    ble_dm_cback_register(bap_dm_cback);
#endif
#if LE_AUDIO_INIT_DEBUG
    PROTOCOL_PRINT_INFO2("[BAP]bap_role_init: role_msk 0x%x, disc_flags 0x%x",
                         bap_db.role_msk, bap_db.disc_flags);
#endif
    return true;
failed:
    PROTOCOL_PRINT_ERROR1("[BAP]bap_role_init: failed, err_idx %d", err_idx);
    return false;
}

#if LE_AUDIO_DEINIT
void bap_role_deinit(void)
{
    for (uint8_t k = 0; k < ble_audio_db.acl_link_num; k++)
    {
        if (ble_audio_db.le_link[k].p_bap_cb != NULL)
        {
#if (LE_AUDIO_BASS_CLIENT_SUPPORT || LE_AUDIO_PACS_CLIENT_SUPPORT || LE_AUDIO_ASCS_CLIENT_SUPPORT)
            bap_link_free(ble_audio_db.le_link[k].p_bap_cb);
#endif
            ble_audio_mem_free(ble_audio_db.le_link[k].p_bap_cb);
            ble_audio_db.le_link[k].p_bap_cb = NULL;
        }
    }
    memset((uint8_t *)&bap_db, 0, sizeof(bap_db));
}
#endif
#endif
