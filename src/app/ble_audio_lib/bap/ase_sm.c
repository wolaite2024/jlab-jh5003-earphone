#include <string.h>
#include "trace.h"
#include "ase_sm.h"
#include "ble_link_util.h"
#include "ascs.h"
#include "codec_qos.h"

#if LE_AUDIO_ASCS_SUPPORT
T_ASE_ENTRY *ase_find_by_ase_id(uint16_t conn_handle, uint8_t ase_id)
{
    T_BLE_AUDIO_LINK *p_link = NULL;
    T_ASE_ENTRY *p_entry = NULL;

    p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link != NULL && p_link->p_ase_entry != NULL)
    {
        uint8_t i;
        T_ASE_ENTRY *p_ase_table = p_link->p_ase_entry;
        for (i = 0; i < (ble_audio_db.snk_ase_num + ble_audio_db.src_ase_num); i++)
        {
            if (p_ase_table[i].ase_id == ase_id)
            {
                p_entry = &p_ase_table[i];
                break;
            }
        }
    }

    return p_entry;
}

T_ASE_ENTRY *ase_find_by_cig_cis(uint16_t conn_handle, T_AUDIO_DIRECTION direction, uint8_t cig_id,
                                 uint8_t cis_id)
{
    T_BLE_AUDIO_LINK *p_link = NULL;
    T_ASE_ENTRY *p_entry = NULL;


    p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link != NULL && p_link->p_ase_entry != NULL)
    {
        uint8_t i;
        T_ASE_ENTRY *p_ase_table = p_link->p_ase_entry;
        for (i = 0; i < (ble_audio_db.snk_ase_num + ble_audio_db.src_ase_num); i++)
        {
            if ((p_ase_table[i].ase_state == ASE_STATE_QOS_CONFIGURED ||
                 p_ase_table[i].ase_state == ASE_STATE_ENABLING ||
                 p_ase_table[i].ase_state == ASE_STATE_STREAMING ||
                 p_ase_table[i].ase_state == ASE_STATE_DISABLING) &&
                p_ase_table[i].direction == direction &&
                p_ase_table[i].qos_state_data.cig_id == cig_id &&
                p_ase_table[i].qos_state_data.cis_id == cis_id)
            {
                p_entry = &p_ase_table[i];
                break;
            }
        }
    }

    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR2("ase_find_by_cig_cis: not find, conn_handle 0x%x, cis_id 0x%x", conn_handle,
                              cis_id);
    }

    return p_entry;
}

bool ase_sm_link_init(uint16_t conn_handle)
{
    T_BLE_AUDIO_LINK *p_link = NULL;
    uint8_t        i;
    T_ASE_ENTRY *p_ase_table;

    p_link = ble_audio_link_find_by_conn_handle(conn_handle);

    if (p_link != NULL && p_link->p_ase_entry == NULL)
    {
        p_link->p_ase_entry = ble_audio_mem_zalloc((ble_audio_db.snk_ase_num + ble_audio_db.src_ase_num) *
                                                   sizeof(T_ASE_ENTRY));
        if (p_link->p_ase_entry == NULL)
        {
            PROTOCOL_PRINT_ERROR0("ase_sm_link_init: allocate mem failed");
            return false;
        }
    }
    if (p_link == NULL)
    {
        return false;
    }
    p_ase_table = p_link->p_ase_entry;
    for (i = 0; i < ble_audio_db.snk_ase_num; i++)
    {
        p_ase_table[i].ase_id = i + 1;
        p_ase_table[i].direction = SERVER_AUDIO_SINK;
        p_ase_table[i].conn_handle = conn_handle;
    }

    for (i = ble_audio_db.snk_ase_num; i < (ble_audio_db.snk_ase_num + ble_audio_db.src_ase_num); i++)
    {
        p_ase_table[i].ase_id = i + 1;
        p_ase_table[i].direction = SERVER_AUDIO_SOURCE;
        p_ase_table[i].conn_handle = conn_handle;
    }
    return true;
}

void ase_sm_link_free(uint16_t conn_handle)
{
    T_BLE_AUDIO_LINK *p_link = NULL;

    p_link = ble_audio_link_find_by_conn_handle(conn_handle);

    if (p_link != NULL && p_link->p_ase_entry != NULL)
    {
        uint8_t i;
        T_ASE_ENTRY *p_ase_table = p_link->p_ase_entry;
        for (i = 0; i < (ble_audio_db.snk_ase_num + ble_audio_db.src_ase_num); i++)
        {
            if (p_ase_table[i].p_codec_spec_cfg != NULL)
            {
                ble_audio_mem_free(p_ase_table[i].p_codec_spec_cfg);
            }
            if (p_ase_table[i].p_metadata != NULL)
            {
                ble_audio_mem_free(p_ase_table[i].p_metadata);
            }
        }
        ble_audio_mem_free(p_link->p_ase_entry);
        p_link->p_ase_entry = NULL;
    }
}

void ascs_change_state(T_ASE_ENTRY *p_entry, uint8_t ase_state)
{
    if (p_entry == NULL || p_entry->ase_state == ase_state)
    {
        return;
    }
    switch (ase_state)
    {
    case ASE_STATE_IDLE:
        {
            if (p_entry->p_codec_spec_cfg)
            {
                ble_audio_mem_free(p_entry->p_codec_spec_cfg);
                p_entry->p_codec_spec_cfg = NULL;
            }
            p_entry->codec_spec_cfg_len = 0;
            if (p_entry->p_metadata)
            {
                ble_audio_mem_free(p_entry->p_metadata);
                p_entry->p_metadata = NULL;
            }
            p_entry->metadata_len = 0;
            memset(&p_entry->codec_state_data, 0, sizeof(T_ASE_CODEC_CFG_STATE_DATA));
            memset(&p_entry->qos_state_data, 0, sizeof(T_ASE_QOS_CFG_STATE_DATA));
            memset(&p_entry->codec_cfg, 0, sizeof(T_CODEC_CFG));
        }
        break;

    case ASE_STATE_CODEC_CONFIGURED:
        {
            if (p_entry->p_metadata)
            {
                ble_audio_mem_free(p_entry->p_metadata);
                p_entry->p_metadata = NULL;
            }
            p_entry->metadata_len = 0;
            memset(&p_entry->qos_state_data, 0, sizeof(T_ASE_QOS_CFG_STATE_DATA));
        }
        break;

    case ASE_STATE_QOS_CONFIGURED:
        {
            p_entry->rec_start_ready = false;
            p_entry->rec_stop_ready = false;
            p_entry->app_handshake_rec = false;
            if (p_entry->p_metadata)
            {
                ble_audio_mem_free(p_entry->p_metadata);
                p_entry->p_metadata = NULL;
                p_entry->metadata_len = 0;
            }
        }
        break;

    case ASE_STATE_STREAMING:
    case ASE_STATE_RELEASING:
        {
            p_entry->rec_start_ready = false;
            p_entry->rec_stop_ready = false;
            p_entry->app_handshake_rec = false;
        }
        break;

    default:
        break;
    }
    p_entry->ase_state = ase_state;
}

bool ase_gen_char_data(uint16_t conn_handle, uint8_t ase_id, uint8_t  **pp_data,
                       uint16_t *p_data_len)
{
    uint16_t idx = 0;
    T_ASE_ENTRY *p_entry = ase_find_by_ase_id(conn_handle, ase_id);
    if (p_entry != NULL)
    {
        uint8_t *p_data = NULL;
        uint16_t data_length = 0;
        data_length += 2;
        if (p_entry->ase_state == ASE_STATE_CODEC_CONFIGURED)
        {
            data_length += sizeof(T_ASE_CODEC_CFG_STATE_DATA);
            data_length += p_entry->codec_spec_cfg_len;
        }
        else if (p_entry->ase_state == ASE_STATE_ENABLING ||
                 p_entry->ase_state == ASE_STATE_STREAMING ||
                 p_entry->ase_state == ASE_STATE_DISABLING)
        {
            data_length += 3;
            data_length += p_entry->metadata_len;

        }
        else if (p_entry->ase_state == ASE_STATE_QOS_CONFIGURED)
        {
            data_length += sizeof(T_ASE_QOS_CFG_STATE_DATA);
        }
        else if (p_entry->ase_state == ASE_STATE_IDLE ||
                 p_entry->ase_state == ASE_STATE_RELEASING)
        {

        }
        else
        {
            goto failed;
        }
        p_data = ble_audio_mem_zalloc(data_length);
        if (p_data == NULL)
        {
            goto failed;
        }
        p_data[idx++] = p_entry->ase_id;
        if (p_entry->direction == SERVER_AUDIO_SINK &&
            p_entry->ase_state == ASE_STATE_DISABLING)
        {
            p_data[idx++] = ASE_STATE_STREAMING;
        }
        else
        {
            p_data[idx++] = p_entry->ase_state;
        }

        switch (p_entry->ase_state)
        {
        case ASE_STATE_CODEC_CONFIGURED:
            {
                memcpy(&p_data[idx], &p_entry->codec_state_data, sizeof(T_ASE_CODEC_CFG_STATE_DATA));
                idx += sizeof(T_ASE_CODEC_CFG_STATE_DATA);
                if (p_entry->codec_spec_cfg_len)
                {
                    memcpy(&p_data[idx], p_entry->p_codec_spec_cfg, p_entry->codec_spec_cfg_len);
                    idx += p_entry->codec_spec_cfg_len;
                }
            }
            break;

        case ASE_STATE_QOS_CONFIGURED:
            {
                memcpy(&p_data[idx], &p_entry->qos_state_data, sizeof(T_ASE_QOS_CFG_STATE_DATA));
                idx += sizeof(T_ASE_QOS_CFG_STATE_DATA);
            }
            break;

        case ASE_STATE_ENABLING:
        case ASE_STATE_STREAMING:
        case ASE_STATE_DISABLING:
            {
                p_data[idx++] = p_entry->qos_state_data.cig_id;
                p_data[idx++] = p_entry->qos_state_data.cis_id;
                p_data[idx++] = p_entry->metadata_len;
                if (p_entry->metadata_len)
                {
                    memcpy(&p_data[idx], p_entry->p_metadata, p_entry->metadata_len);
                    idx += p_entry->metadata_len;
                }
            }
            break;

        default:
            break;
        }
        *p_data_len = idx;
        *pp_data = p_data;
        PROTOCOL_PRINT_INFO4("ase_gen_char_data: ase_id %d, direction: %d, ase_state %d, len %d",
                             ase_id, p_entry->direction, p_entry->ase_state, idx);
        return true;
    }
failed:
    PROTOCOL_PRINT_ERROR1("ase_gen_char_data: not find entry, ase_id %d", ase_id);
    return false;
}

bool ascs_get_codec_cfg(uint16_t conn_handle, uint8_t ase_id, T_ASE_DATA_CODEC_CONFIGURED *p_cfg)
{
    T_ASE_ENTRY *p_entry = ase_find_by_ase_id(conn_handle, ase_id);
    if (p_entry == NULL || p_cfg == NULL)
    {
        return false;
    }
    if (p_entry->ase_state ==  ASE_STATE_RELEASING || p_entry->ase_state ==  ASE_STATE_IDLE)
    {
        return false;
    }
    memcpy(&p_cfg->data, &p_entry->codec_state_data, sizeof(T_ASE_CODEC_CFG_STATE_DATA));
    p_cfg->p_codec_spec_cfg = p_entry->p_codec_spec_cfg;
    return true;
}

bool ascs_get_qos_cfg(uint16_t conn_handle, uint8_t ase_id, T_ASE_QOS_CFG_STATE_DATA *p_cfg)
{
    T_ASE_ENTRY *p_entry = ase_find_by_ase_id(conn_handle, ase_id);
    if (p_entry == NULL || p_cfg == NULL)
    {
        return false;
    }
    if (p_entry->ase_state ==  ASE_STATE_RELEASING || p_entry->ase_state ==  ASE_STATE_IDLE ||
        p_entry->ase_state ==  ASE_STATE_CODEC_CONFIGURED)
    {
        return false;
    }
    memcpy(p_cfg, &p_entry->qos_state_data, sizeof(T_ASE_QOS_CFG_STATE_DATA));
    return true;
}

bool ascs_get_ase_data(uint16_t conn_handle, uint8_t ase_id, T_ASE_CHAR_DATA *p_ase_data)
{
    T_ASE_ENTRY *p_entry = ase_find_by_ase_id(conn_handle, ase_id);
    if (p_entry == NULL || p_ase_data == NULL)
    {
        return false;
    }
    memset(p_ase_data, 0, sizeof(T_ASE_CHAR_DATA));
    p_ase_data->ase_id = p_entry->ase_id;
    p_ase_data->direction = p_entry->direction;
    p_ase_data->ase_state = p_entry->ase_state;
    switch (p_entry->ase_state)
    {
    case ASE_STATE_CODEC_CONFIGURED:
        {
            memcpy(&p_ase_data->param.codec_configured.data, &p_entry->codec_state_data,
                   sizeof(T_ASE_CODEC_CFG_STATE_DATA));
            p_ase_data->param.codec_configured.p_codec_spec_cfg = p_entry->p_codec_spec_cfg;
        }
        break;

    case ASE_STATE_QOS_CONFIGURED:
        {
            memcpy(&p_ase_data->param.qos_configured, &p_entry->qos_state_data,
                   sizeof(T_ASE_QOS_CFG_STATE_DATA));
        }
        break;

    case ASE_STATE_ENABLING:
    case ASE_STATE_STREAMING:
    case ASE_STATE_DISABLING:
        {
            p_ase_data->param.enabling.cig_id = p_entry->qos_state_data.cig_id;
            p_ase_data->param.enabling.cis_id = p_entry->qos_state_data.cis_id;
            p_ase_data->param.enabling.metadata_length = p_entry->metadata_len;
            p_ase_data->param.enabling.p_metadata = p_entry->p_metadata;
        }
        break;

    default:
        break;
    }
    return true;
}

bool ascs_get_ase_info(uint16_t conn_handle, T_ASCS_INFO *p_info)
{
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);

    if (p_link != NULL && p_link->p_ase_entry != NULL && p_info != NULL)
    {
        uint8_t i;
        T_ASE_ENTRY *p_ase_table = p_link->p_ase_entry;
        memset(p_info, 0, sizeof(T_ASCS_INFO));
        for (i = 0; i < (ble_audio_db.snk_ase_num + ble_audio_db.src_ase_num); i++)
        {
            if (p_ase_table[i].direction == SERVER_AUDIO_SINK)
            {
                p_info->snk_info[p_info->snk_ase_num].ase_id = p_ase_table[i].ase_id;
                p_info->snk_info[p_info->snk_ase_num].ase_state = p_ase_table[i].ase_state;
                p_info->snk_ase_num++;
            }
            else
            {
                p_info->src_info[p_info->src_ase_num].ase_id = p_ase_table[i].ase_id;
                p_info->src_info[p_info->src_ase_num].ase_state = p_ase_table[i].ase_state;
                p_info->src_ase_num++;
            }
        }
        return true;
    }
    return false;
}

bool ascs_get_ase_prefer_qos(uint16_t conn_handle, uint8_t ase_id, T_ASCS_PREFER_QOS_DATA *p_qos)
{
    T_ASE_ENTRY *p_entry = ase_find_by_ase_id(conn_handle, ase_id);
    T_ASE_CODEC_CFG_STATE_DATA *p_cfg_data;
    if (p_entry == NULL || p_qos == NULL)
    {
        return false;
    }
    if (p_entry->ase_state != ASE_STATE_CODEC_CONFIGURED)
    {
        return false;
    }
    p_cfg_data = &p_entry->codec_state_data;

    p_qos->supported_framing = p_cfg_data->supported_framing;
    p_qos->preferred_retrans_number = p_cfg_data->preferred_retrans_number;
    p_qos->preferred_phy = p_cfg_data->preferred_phy;
    LE_ARRAY_TO_UINT16(p_qos->max_transport_latency, p_cfg_data->max_transport_latency);
    LE_ARRAY_TO_UINT24(p_qos->presentation_delay_min, p_cfg_data->presentation_delay_min);
    LE_ARRAY_TO_UINT24(p_qos->presentation_delay_max, p_cfg_data->presentation_delay_max);
    LE_ARRAY_TO_UINT24(p_qos->preferred_presentation_delay_min,
                       p_cfg_data->preferred_presentation_delay_min);
    LE_ARRAY_TO_UINT24(p_qos->preferred_presentation_delay_max,
                       p_cfg_data->preferred_presentation_delay_max);
    PROTOCOL_PRINT_INFO6("[BAP][ASCS] ascs_get_ase_prefer_qos: conn_handle 0x%x, ase_id %d, supported_framing 0x%x, preferred_phy %d, preferred_retrans_number %d, max_transport_latency 0x%x",
                         conn_handle, p_entry->ase_id, p_qos->supported_framing, p_qos->preferred_phy,
                         p_qos->preferred_retrans_number, p_qos->max_transport_latency);
    PROTOCOL_PRINT_INFO4("[BAP][ASCS] ascs_get_ase_prefer_qos: presentation_delay(0x%x -> 0x%x), preferred_presentation_delay(0x%x -> 0x%x)",
                         p_qos->presentation_delay_min, p_qos->presentation_delay_max,
                         p_qos->preferred_presentation_delay_min, p_qos->preferred_presentation_delay_max);
    return true;
}

bool ascs_cfg_ase_prefer_qos(uint16_t conn_handle, uint8_t ase_id, T_ASCS_PREFER_QOS_DATA *p_qos)
{
    T_ASE_ENTRY *p_entry = ase_find_by_ase_id(conn_handle, ase_id);
    T_ASE_CODEC_CFG_STATE_DATA *p_cfg_data;
    if (p_entry == NULL || p_qos == NULL)
    {
        return false;
    }
    if (p_entry->ase_state != ASE_STATE_CODEC_CONFIGURED)
    {
        return false;
    }
    p_cfg_data = &p_entry->codec_state_data;
    p_cfg_data->supported_framing = p_qos->supported_framing;
    p_cfg_data->preferred_retrans_number = p_qos->preferred_retrans_number;
    p_cfg_data->preferred_phy = p_qos->preferred_phy;
    LE_UINT16_TO_ARRAY(p_cfg_data->max_transport_latency, p_qos->max_transport_latency);
    LE_UINT24_TO_ARRAY(p_cfg_data->presentation_delay_min, p_qos->presentation_delay_min);
    LE_UINT24_TO_ARRAY(p_cfg_data->presentation_delay_max, p_qos->presentation_delay_max);
    LE_UINT24_TO_ARRAY(p_cfg_data->preferred_presentation_delay_min,
                       p_qos->preferred_presentation_delay_min);
    LE_UINT24_TO_ARRAY(p_cfg_data->preferred_presentation_delay_max,
                       p_qos->preferred_presentation_delay_max);
    PROTOCOL_PRINT_INFO6("[BAP][ASCS] ascs_cfg_ase_prefer_qos: conn_handle 0x%x, ase_id %d, supported_framing 0x%x, preferred_phy %d, preferred_retrans_number %d, max_transport_latency 0x%x",
                         conn_handle, p_entry->ase_id, p_qos->supported_framing, p_qos->preferred_phy,
                         p_qos->preferred_retrans_number, p_qos->max_transport_latency);
    PROTOCOL_PRINT_INFO4("[BAP][ASCS] ascs_cfg_ase_prefer_qos: presentation_delay(0x%x -> 0x%x), preferred_presentation_delay(0x%x -> 0x%x)",
                         p_qos->presentation_delay_min, p_qos->presentation_delay_max,
                         p_qos->preferred_presentation_delay_min, p_qos->preferred_presentation_delay_max);
    return true;
}

bool ase_sm_send_notify(uint16_t conn_handle, T_ASE_ENTRY *p_entry)
{
    uint8_t *p_data = NULL;
    uint16_t len;
    T_ASCS_ASE_STATE ase_state;

    if (p_entry == NULL)
    {
        return false;
    }

    if (ase_gen_char_data(conn_handle, p_entry->ase_id, &p_data, &len))
    {
        ascs_send_ase_notify(conn_handle, L2C_FIXED_CID_ATT, p_entry->ase_id, p_data, len);
        ble_audio_mem_free(p_data);
    }
    ase_state.conn_handle = conn_handle;
    if (ascs_get_ase_data(conn_handle, p_entry->ase_id, &ase_state.ase_data))
    {
        ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_ASE_STATE, &ase_state);
    }
    return false;
}

bool ase_action_cfg_codec(uint16_t conn_handle, T_ASE_ENTRY *p_entry,
                          T_ASE_CP_CODEC_CFG_ARRAY_PARAM *p_param, uint8_t *p_codec_data,
                          bool is_client)
{
    T_ASE_CODEC_CFG_STATE_DATA *p_cfg_data = &p_entry->codec_state_data;

    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR0("ase_action_cfg_codec: p_entry == NULL");
        return false;
    }
    PROTOCOL_PRINT_INFO3("[BAP][ASCS] ase_action_cfg_codec: ase_id %d, start state %d, is_client %d",
                         p_entry->ase_id, p_entry->ase_state, is_client);
    PROTOCOL_PRINT_INFO4("[BAP][ASCS] ase_action_cfg_codec: direction %d, target_latency %d, target_phy %d, codec_id %b",
                         p_entry->direction, p_param->target_latency, p_param->target_phy, TRACE_BINARY(5,
                                 p_param->codec_id));
    if (p_param->codec_spec_cfg_len)
    {
        PROTOCOL_PRINT_INFO2("[BAP][ASCS] ase_action_cfg_codec: codec[%d] %b",
                             p_param->codec_spec_cfg_len, TRACE_BINARY(p_param->codec_spec_cfg_len, p_codec_data));
    }

    if (p_entry->ase_state == ASE_STATE_IDLE || p_entry->ase_state == ASE_STATE_CODEC_CONFIGURED ||
        p_entry->ase_state == ASE_STATE_QOS_CONFIGURED)
    {
        ascs_change_state(p_entry, ASE_STATE_CODEC_CONFIGURED);
    }
    else
    {
        return false;
    }

    if (codec_cfg_parse(p_param->codec_spec_cfg_len, p_codec_data, &p_entry->codec_cfg) == false)
    {
        return false;
    }

    p_entry->codec_state_data.codec_spec_cfg_len = p_param->codec_spec_cfg_len;
    memcpy(p_entry->codec_state_data.codec_id, p_param->codec_id, 5);

    if (p_entry->p_codec_spec_cfg)
    {
        ble_audio_mem_free(p_entry->p_codec_spec_cfg);
        p_entry->p_codec_spec_cfg = NULL;
    }
    p_entry->codec_spec_cfg_len = p_param->codec_spec_cfg_len;
    if (p_entry->codec_spec_cfg_len != 0)
    {
        p_entry->p_codec_spec_cfg = ble_audio_mem_zalloc(p_entry->codec_spec_cfg_len);
        if (p_entry->p_codec_spec_cfg == NULL)
        {
            return false;
        }
        memcpy(p_entry->p_codec_spec_cfg, p_codec_data, p_entry->codec_spec_cfg_len);
    }

    if (p_param->codec_id[0] == LC3_CODEC_ID)
    {
        T_QOS_CFG_PREFERRED qos_data;
        if (qos_preferred_cfg_get_by_codec(&p_entry->codec_cfg, p_param->target_latency, &qos_data))
        {
            p_cfg_data->supported_framing = AUDIO_UNFRAMED_SUPPORTED;
            p_cfg_data->preferred_retrans_number = qos_data.retransmission_number;
            p_cfg_data->preferred_phy = (1 << (p_param->target_phy - 1)) |
                                        ASE_LE_2M_PHY_BIT;
            if (p_entry->min_transport_latency != 0 &&
                qos_data.max_transport_latency < p_entry->min_transport_latency)
            {
                PROTOCOL_PRINT_INFO1("[BAP][ASCS] ase_action_cfg_codec: max_transport_latency %d < p_entry->min_transport_latency",
                                     qos_data.max_transport_latency);
                qos_data.max_transport_latency = p_entry->min_transport_latency;
            }
            LE_UINT16_TO_ARRAY(p_cfg_data->max_transport_latency, qos_data.max_transport_latency);
            LE_UINT24_TO_ARRAY(p_cfg_data->presentation_delay_min, 0);
            LE_UINT24_TO_ARRAY(p_cfg_data->presentation_delay_max, (qos_data.presentation_delay + 10000));
            LE_UINT24_TO_ARRAY(p_cfg_data->preferred_presentation_delay_min, 0);
            LE_UINT24_TO_ARRAY(p_cfg_data->preferred_presentation_delay_max, 0);
        }
    }

    T_ASCS_GET_PREFER_QOS prefer_qos;
    prefer_qos.conn_handle = conn_handle;
    prefer_qos.ase_id = p_entry->ase_id;
    prefer_qos.direction = p_entry->direction;
    memcpy(&prefer_qos.codec_cfg, &p_entry->codec_cfg, sizeof(T_CODEC_CFG));
    prefer_qos.target_latency = (T_ASE_TARGET_LATENCY)p_param->target_latency;
    prefer_qos.target_phy = (T_ASE_TARGET_PHY)p_param->target_phy;
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_GET_PREFER_QOS, &prefer_qos);

    ase_sm_send_notify(conn_handle, p_entry);
    ascs_check_current_state(conn_handle, p_entry);

    return true;
}

bool ase_action_cfg_qos(uint16_t conn_handle, T_ASE_ENTRY *p_entry,
                        T_ASE_CP_QOS_CFG_ARRAY_PARAM *p_param)
{
    PROTOCOL_PRINT_INFO2("[BAP][ASCS] ase_action_cfg_qos: ase_id %d, start state %d",
                         p_entry->ase_id, p_entry->ase_state);
    PROTOCOL_PRINT_INFO7("[BAP][ASCS] ase_action_cfg_qos: cig_id %d, cis_id %d, sdu_interval %x %x %x, framing %d, phy 0x%x",
                         p_param->cig_id, p_param->cis_id,
                         p_param->sdu_interval[0], p_param->sdu_interval[1],
                         p_param->sdu_interval[2],
                         p_param->framing, p_param->phy);
    PROTOCOL_PRINT_INFO8("[BAP][ASCS] ase_action_cfg_qos: max_sdu  %x %x, retransmission_number %d, max_transport_latency %x %x, presentation_delay  %x %x %x",
                         p_param->max_sdu[0], p_param->max_sdu[1],
                         p_param->retransmission_number,
                         p_param->max_transport_latency[0],
                         p_param->max_transport_latency[1],
                         p_param->presentation_delay[0],
                         p_param->presentation_delay[1],
                         p_param->presentation_delay[2]);
    if (p_entry->ase_state == ASE_STATE_CODEC_CONFIGURED ||
        p_entry->ase_state == ASE_STATE_QOS_CONFIGURED)
    {
        ascs_change_state(p_entry, ASE_STATE_QOS_CONFIGURED);
    }
    else
    {
        return false;
    }
    memcpy(&p_entry->qos_state_data, &p_param->cig_id, sizeof(T_ASE_QOS_CFG_STATE_DATA));
    ase_sm_send_notify(conn_handle, p_entry);
    ascs_check_current_state(conn_handle, p_entry);

    return true;
}

bool ase_action_enable(uint16_t conn_handle, T_ASE_ENTRY *p_entry,
                       uint8_t metadata_len, uint8_t *p_metadata)
{
    PROTOCOL_PRINT_INFO3("[BAP][ASCS] ase_action_enable: ase_id %d, start state %d, metadata_len %d",
                         p_entry->ase_id, p_entry->ase_state, metadata_len);
    if (metadata_len)
    {
        PROTOCOL_PRINT_INFO1("[BAP][ASCS] ase_action_enable: metadata %b",
                             TRACE_BINARY(metadata_len, p_metadata));
    }
    if (p_entry->ase_state == ASE_STATE_QOS_CONFIGURED)
    {
        ascs_change_state(p_entry, ASE_STATE_ENABLING);
    }
    else
    {
        return false;
    }

    if (p_entry->p_metadata)
    {
        ble_audio_mem_free(p_entry->p_metadata);
        p_entry->p_metadata = NULL;
    }
    p_entry->metadata_len = metadata_len;
    if (p_entry->metadata_len != 0)
    {
        p_entry->p_metadata = ble_audio_mem_zalloc(p_entry->metadata_len);
        if (p_entry->p_metadata == NULL)
        {
            return false;
        }
        memcpy(p_entry->p_metadata, p_metadata, p_entry->metadata_len);
    }

    if (p_entry->direction == SERVER_AUDIO_SINK)
    {
        if (ase_sm_check_iosch_status(ASE_SM_CHECK_ENABLE, p_entry))
        {
            if (p_entry->app_handshake == false ||
                (p_entry->app_handshake == true && p_entry->app_handshake_rec == true))
            {
                ascs_change_state(p_entry, ASE_STATE_STREAMING);
            }

        }
    }
    ase_sm_send_notify(conn_handle, p_entry);
    ascs_check_current_state(conn_handle, p_entry);
    return true;
}

bool ase_action_rec_start_ready(uint16_t conn_handle, T_ASE_ENTRY *p_entry)
{
    uint8_t old_ase_state;
    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR0("ase_action_rec_start_ready: p_entry == NULL");
        return false;
    }
    old_ase_state = p_entry->ase_state;
    PROTOCOL_PRINT_INFO2("[BAP][ASCS] ase_action_rec_start_ready: ase_id %d, start state %d",
                         p_entry->ase_id, p_entry->ase_state);

    if (p_entry->ase_state == ASE_STATE_ENABLING &&
        p_entry->direction == SERVER_AUDIO_SOURCE)
    {
        p_entry->rec_start_ready = true;
        if (ase_sm_check_iosch_status(ASE_SM_CHECK_ENABLE, p_entry))
        {
            ascs_change_state(p_entry, ASE_STATE_STREAMING);
        }
    }
    else
    {
        return false;
    }
    if (old_ase_state != p_entry->ase_state)
    {
        ase_sm_send_notify(conn_handle, p_entry);
    }

    ascs_check_current_state(conn_handle, p_entry);
    return true;
}

bool ase_action_rec_stop_ready(uint16_t conn_handle, T_ASE_ENTRY *p_entry)
{
    uint8_t old_ase_state;
    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR0("ase_action_rec_stop_ready: p_entry == NULL");
        return false;
    }
    old_ase_state = p_entry->ase_state;
    PROTOCOL_PRINT_INFO2("[BAP][ASCS] ase_action_rec_stop_ready: ase_id %d, start state %d",
                         p_entry->ase_id, p_entry->ase_state);

    if (p_entry->ase_state == ASE_STATE_DISABLING &&
        p_entry->direction == SERVER_AUDIO_SOURCE)
    {
        p_entry->rec_stop_ready = true;
        if (ase_sm_check_iosch_status(ASE_SM_CHECK_DISABLE, p_entry))
        {
            ascs_change_state(p_entry, ASE_STATE_QOS_CONFIGURED);
        }
    }
    else
    {
        return false;
    }
    if (old_ase_state != p_entry->ase_state)
    {
        ase_sm_send_notify(conn_handle, p_entry);
    }
    ascs_check_current_state(conn_handle, p_entry);
    return true;
}

bool ase_switch_to_streaming(uint16_t conn_handle, T_ASE_ENTRY *p_entry)
{
    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR0("ase_switch_to_streaming: p_entry == NULL");
        return false;
    }
    PROTOCOL_PRINT_INFO2("[BAP][ASCS] ase_switch_to_streaming: ase_id %d, start state %d",
                         p_entry->ase_id, p_entry->ase_state);
    if (p_entry->ase_state == ASE_STATE_ENABLING)
    {
        ascs_change_state(p_entry, ASE_STATE_STREAMING);
    }
    else
    {
        return false;
    }
    ase_sm_send_notify(conn_handle, p_entry);
    ascs_check_current_state(conn_handle, p_entry);
    return true;
}

bool ase_switch_to_qos_cfg(uint16_t conn_handle, T_ASE_ENTRY *p_entry)
{
    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR0("ase_switch_to_qos_cfg: p_entry == NULL");
        return false;
    }
    PROTOCOL_PRINT_INFO2("[BAP][ASCS] ase_switch_to_qos_cfg: ase_id %d, start state %d",
                         p_entry->ase_id, p_entry->ase_state);
    if (p_entry->ase_state == ASE_STATE_DISABLING)
    {
        ascs_change_state(p_entry, ASE_STATE_QOS_CONFIGURED);
    }
    else
    {
        return false;
    }
    ase_sm_send_notify(conn_handle, p_entry);
    ascs_check_current_state(conn_handle, p_entry);
    return true;
}

bool ase_action_disable(uint16_t conn_handle, T_ASE_ENTRY *p_entry, bool is_client)
{
    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR0("ase_action_disable: p_entry == NULL");
        return false;
    }
    PROTOCOL_PRINT_INFO3("[BAP][ASCS] ase_action_disable: ase_id %d, start state %d, is_client %d",
                         p_entry->ase_id, p_entry->ase_state, is_client);
    if (p_entry->ase_state == ASE_STATE_ENABLING || p_entry->ase_state == ASE_STATE_STREAMING)
    {

        if (p_entry->direction == SERVER_AUDIO_SINK)
        {
            ascs_change_state(p_entry, ASE_STATE_QOS_CONFIGURED);
        }
        else
        {
            ascs_change_state(p_entry, ASE_STATE_DISABLING);
        }

        ase_sm_send_notify(conn_handle, p_entry);
        ascs_check_current_state(conn_handle, p_entry);
    }
    else
    {
        return false;
    }


    return true;
}

bool ase_action_update_metadata(uint16_t conn_handle, T_ASE_ENTRY *p_entry,
                                uint8_t metadata_len, uint8_t *p_metadata, bool is_client)
{
    PROTOCOL_PRINT_INFO2("[BAP][ASCS] ase_action_update_metadata: ase_id %d, start state %d",
                         p_entry->ase_id, p_entry->ase_state);
    if (metadata_len)
    {
        PROTOCOL_PRINT_INFO2("[BAP][ASCS] ase_action_update_metadata: metadata[%d] %b",
                             metadata_len, TRACE_BINARY(metadata_len, p_metadata));
    }
    if (p_entry->ase_state != ASE_STATE_ENABLING && p_entry->ase_state != ASE_STATE_STREAMING)
    {
        return false;
    }

    if (p_entry->p_metadata)
    {
        ble_audio_mem_free(p_entry->p_metadata);
        p_entry->p_metadata = NULL;
    }
    p_entry->metadata_len = metadata_len;
    if (p_entry->metadata_len != 0)
    {
        p_entry->p_metadata = ble_audio_mem_zalloc(p_entry->metadata_len);
        if (p_entry->p_metadata == NULL)
        {
            return false;
        }
        memcpy(p_entry->p_metadata, p_metadata, p_entry->metadata_len);
    }
    ase_sm_send_notify(conn_handle, p_entry);
    ascs_check_current_state(conn_handle, p_entry);

    return true;

}

bool ase_action_release(uint16_t conn_handle, T_ASE_ENTRY *p_entry, bool is_client)
{
    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR0("ase_action_release: p_entry == NULL");
        return false;
    }
    PROTOCOL_PRINT_INFO3("[BAP][ASCS] ase_action_release: ase_id %d, start state %d, is_client %d",
                         p_entry->ase_id, p_entry->ase_state, is_client);
    if (p_entry->ase_state != ASE_STATE_IDLE &&
        p_entry->ase_state != ASE_STATE_RELEASING)
    {
        ascs_change_state(p_entry, ASE_STATE_RELEASING);
    }
    else
    {
        return false;
    }
    ase_sm_send_notify(conn_handle, p_entry);
    ascs_check_current_state(conn_handle, p_entry);

    return true;
}

bool ase_action_released(uint16_t conn_handle, T_ASE_ENTRY *p_entry)
{
    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR0("ase_action_released: p_entry == NULL");
        return false;
    }
    PROTOCOL_PRINT_INFO2("[BAP][ASCS] ase_action_released: ase_id %d, start state %d",
                         p_entry->ase_id, p_entry->ase_state);
    if (ase_sm_check_iosch_status(ASE_SM_CHECK_RELEASE, p_entry) == false)
    {
        return false;
    }
    if (p_entry->ase_state == ASE_STATE_RELEASING)
    {
        ascs_change_state(p_entry, ASE_STATE_IDLE);
    }
    else
    {
        return false;
    }
    ase_sm_send_notify(conn_handle, p_entry);
    ascs_check_current_state(conn_handle, p_entry);
    return true;
}

#endif
