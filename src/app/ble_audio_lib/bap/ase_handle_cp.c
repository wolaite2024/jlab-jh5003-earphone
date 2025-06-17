#include <string.h>
#include "trace.h"
#include "ase_handle_cp.h"
#include "ascs.h"
#include "codec_def.h"
#include "metadata_int.h"
#include "pac_mgr.h"
#include "ase_sm.h"
#include "ble_audio_mgr.h"
#include "metadata_def.h"

#if (LE_AUDIO_ASCS_SUPPORT | LE_AUDIO_ASCS_CLIENT_SUPPORT)

bool ase_cp_check_sm_state(uint8_t ase_state, uint8_t direction, T_ASE_CP_OP cp_op, bool is_client)
{
    bool result = false;

    switch (cp_op)
    {
    case ASE_CP_OP_CONFIG_CODEC:
        {
            if (ase_state == ASE_STATE_IDLE || ase_state == ASE_STATE_CODEC_CONFIGURED
                || ase_state == ASE_STATE_QOS_CONFIGURED)
            {
                result = true;
            }
        }
        break;

    case ASE_CP_OP_CONFIG_QOS:
        {
            if (is_client &&
                (ase_state == ASE_STATE_CODEC_CONFIGURED || ase_state == ASE_STATE_QOS_CONFIGURED))
            {
                result = true;
            }
        }
        break;

    case ASE_CP_OP_ENABLE:
        {
            if (is_client && ase_state == ASE_STATE_QOS_CONFIGURED)
            {
                result = true;
            }
        }
        break;

    case ASE_CP_OP_REC_START_READY:
        {
            if (ase_state == ASE_STATE_ENABLING &&
                (is_client && direction == SERVER_AUDIO_SOURCE)
                || (!is_client && direction == SERVER_AUDIO_SINK))
            {
                result = true;
            }
        }
        break;

    case ASE_CP_OP_REC_STOP_READY:
        {
            if (ase_state == ASE_STATE_DISABLING &&
                (is_client && direction == SERVER_AUDIO_SOURCE)
                || (!is_client && direction == SERVER_AUDIO_SINK))
            {
                result = true;
            }
        }
        break;

    case ASE_CP_OP_DISABLE:
        {
            if (ase_state == ASE_STATE_ENABLING || ase_state == ASE_STATE_STREAMING)
            {
                result = true;
            }
        }
        break;

    case ASE_CP_OP_UPDATE_METADATA:
        {
            if (ase_state == ASE_STATE_ENABLING || ase_state == ASE_STATE_STREAMING)
            {
                result = true;
            }
        }
        break;

    case ASE_CP_OP_RELEASE:
        {
            if (ase_state != ASE_STATE_IDLE && ase_state != ASE_STATE_RELEASING)
            {
                result = true;
            }
        }
        break;

    default:
        break;
    }

    if (result == false)
    {
        PROTOCOL_PRINT_ERROR2("[BAP][ASCS] ase_cp_check_sm_state: failed, cp_op %d, ase_state %d", cp_op,
                              ase_state);
    }
    return result;
}
#endif
#if LE_AUDIO_ASCS_SUPPORT
bool ase_cp_check_msg_len(T_ASE_CP_OP cp_op, uint8_t *p_data, uint16_t len)
{
    bool result = true;
    uint16_t idx = 0;
    uint16_t check_len = 0;
    uint8_t ase_num;
    if (len < ASE_CP_OP_MIN_LEN)
    {
        return false;
    }
    idx++;
    ase_num = p_data[idx];
    idx++;
    switch (cp_op)
    {
    case ASE_CP_OP_CONFIG_CODEC:
        {
            uint8_t codec_len = 0;
            for (uint16_t i = 0; i < ase_num; i++)
            {
                idx += 8;
                codec_len = p_data[idx];
                idx++;
                idx += codec_len;
            }
            if (idx != len)
            {
                result = false;
            }
        }
        break;

    case ASE_CP_OP_CONFIG_QOS:
        {
            check_len = 2 + ase_num * sizeof(T_ASE_CP_QOS_CFG_ARRAY_PARAM);
            if (len != check_len)
            {
                result = false;
            }
        }
        break;

    case ASE_CP_OP_REC_START_READY:
    case ASE_CP_OP_REC_STOP_READY:
    case ASE_CP_OP_RELEASE:
    case ASE_CP_OP_DISABLE:
        {
            check_len = 2 + ase_num;
            if (len != check_len)
            {
                result = false;
            }
        }
        break;

    case ASE_CP_OP_ENABLE:
    case ASE_CP_OP_UPDATE_METADATA:
        {
            uint8_t metadata_len = 0;
            for (uint16_t i = 0; i < ase_num; i++)
            {
                idx++;
                metadata_len = p_data[idx];
                idx++;
                idx += metadata_len;
            }
            if (idx != len)
            {
                result = false;
            }
        }
        break;

    default:
        result = false;
        break;
    }
    if (result == false)
    {
        PROTOCOL_PRINT_ERROR3("ase_cp_check_msg_len: failed, cp_op %d, len %d, idx %d", cp_op, len, idx);
    }
    return result;
}

bool ase_check_cfg_codec_param(T_ASE_ENTRY *p_entry, T_ASE_CP_CODEC_CFG_ARRAY_PARAM *p_codec_cfg,
                               uint8_t *p_codec_data, uint8_t *p_resp_code, uint8_t *p_reason,  T_CODEC_CFG *p_parsed_cfg)
{
    if (p_resp_code == NULL || p_reason == NULL)
    {
        goto error;
    }
    if (p_entry == NULL || p_codec_cfg == NULL)
    {
        *p_resp_code = ASE_CP_RESP_INVALID_PARAM;
        goto error;
    }
    *p_resp_code = 0;
    *p_reason = 0;

    if (p_codec_cfg->target_latency > ASE_TARGET_HIGHER_RELIABILITY)
    {
        *p_resp_code = ASE_CP_RESP_INVALID_PARAM;
        *p_reason = ASE_CP_REASON_MAX_TRANSPORT_LATENCY;
        goto error;
    }
    if (p_codec_cfg->target_phy > ASE_TARGET_PHY_CODED)
    {
        *p_resp_code = ASE_CP_RESP_INVALID_PARAM;
        *p_reason = ASE_CP_REASON_PHY;
        goto error;
    }
    if (p_codec_cfg->codec_id[0] != LC3_CODEC_ID && p_codec_cfg->codec_id[0] != VENDOR_CODEC_ID)
    {
        *p_resp_code = ASE_CP_RESP_INVALID_PARAM;
        *p_reason = ASE_CP_REASON_CODEC_ID;
        goto error;
    }
    if (p_codec_cfg->codec_id[0] == LC3_CODEC_ID)
    {
        uint8_t lc3_codec[CODEC_ID_LEN] = {LC3_CODEC_ID, 0, 0, 0, 0};
        if (memcmp(p_codec_cfg->codec_id, lc3_codec, CODEC_ID_LEN))
        {
            *p_resp_code = ASE_CP_RESP_INVALID_PARAM;
            *p_reason = ASE_CP_REASON_CODEC_ID;
            goto error;
        }
    }
#if LE_AUDIO_PACS_SUPPORT
    if (codec_cfg_parse(p_codec_cfg->codec_spec_cfg_len, p_codec_data, p_parsed_cfg))
    {
        if (p_codec_cfg->codec_id[0] == LC3_CODEC_ID)
        {
            if (pac_find_by_codec_data(p_entry->direction, p_codec_cfg->codec_id, p_parsed_cfg) == NULL)
            {
                *p_resp_code = ASE_CP_RESP_UNSUPPORTED_AUDIO_CAP;
                *p_reason = ASE_CP_REASON_CODEC_CFG;
                goto error;
            }
        }
    }
    else
    {
        *p_resp_code = ASE_CP_RESP_INVALID_PARAM;
        *p_reason = ASE_CP_REASON_CODEC_CFG;
        goto error;
    }
#endif
    return true;
error:
    if (p_resp_code != NULL && p_reason != NULL)
    {
        PROTOCOL_PRINT_ERROR2("ase_check_cfg_codec_param: failed, resp_code 0x%x, reason 0x%x",
                              *p_resp_code, *p_reason);
    }
    return false;
}

bool ase_check_cfg_qos_param(T_ASE_ENTRY *p_entry, T_ASE_CP_QOS_CFG_ARRAY_PARAM *p_qos_cfg,
                             uint8_t *p_resp_code, uint8_t *p_reason)
{
    uint32_t sdu_interval = 0;
    uint16_t max_sdu_size = 0;
    uint16_t transport_latency = 0;
    uint32_t presentation_delay = 0;
    uint32_t presentation_delay_min = 0;
    uint32_t presentation_delay_max = 0;

    if (p_resp_code == NULL || p_reason == NULL)
    {
        goto error;
    }
    if (p_entry == NULL || p_qos_cfg == NULL)
    {
        *p_resp_code = ASE_CP_RESP_INVALID_PARAM;
        goto error;
    }
    *p_resp_code = 0;
    *p_reason = 0;
    LE_ARRAY_TO_UINT24(sdu_interval, p_qos_cfg->sdu_interval);
    LE_ARRAY_TO_UINT16(max_sdu_size, p_qos_cfg->max_sdu);
    LE_ARRAY_TO_UINT16(transport_latency, p_qos_cfg->max_transport_latency);
    LE_ARRAY_TO_UINT24(presentation_delay, p_qos_cfg->presentation_delay);
    LE_ARRAY_TO_UINT24(presentation_delay_min, p_entry->codec_state_data.presentation_delay_min);
    LE_ARRAY_TO_UINT24(presentation_delay_max, p_entry->codec_state_data.presentation_delay_max);

    if (sdu_interval < QOS_SDU_INTERVAL_MIN || sdu_interval > QOS_SDU_INTERVAL_MAX)
    {
        *p_resp_code = ASE_CP_RESP_INVALID_PARAM;
        *p_reason = ASE_CP_REASON_SDU_INTERVAL;
        goto error;
    }

    if (max_sdu_size > QOS_MAX_SDU_MAX)
    {
        *p_resp_code = ASE_CP_RESP_INVALID_PARAM;
        *p_reason = ASE_CP_REASON_MAX_SDU_SIZE;
        goto error;
    }
    if (transport_latency < QOS_TRANSPORT_LATENCY_MIN || transport_latency > QOS_TRANSPORT_LATENCY_MAX)
    {
        if (p_entry->codec_state_data.codec_id[0] == LC3_CODEC_ID ||
            (p_entry->codec_state_data.codec_id[0] != LC3_CODEC_ID &&
             transport_latency > QOS_TRANSPORT_LATENCY_MAX))
        {
            *p_resp_code = ASE_CP_RESP_INVALID_PARAM;
            *p_reason = ASE_CP_REASON_MAX_TRANSPORT_LATENCY;
            goto error;
        }
    }
    if (presentation_delay < presentation_delay_min || presentation_delay > presentation_delay_max)
    {
        *p_resp_code = ASE_CP_RESP_REJECTED_PARAM;
        *p_reason = ASE_CP_REASON_PRESENTATION_DELAY;
        goto error;
    }
    if ((p_qos_cfg->phy | ASE_LE_PHY_CFG_MASK) != ASE_LE_PHY_CFG_MASK)
    {
        *p_resp_code = ASE_CP_RESP_INVALID_PARAM;
        *p_reason = ASE_CP_REASON_PHY;
        goto error;
    }
    if (p_qos_cfg->framing > AUDIO_FRAMED)
    {
        *p_resp_code = ASE_CP_RESP_INVALID_PARAM;
        *p_reason = ASE_CP_REASON_FRAMING;
        goto error;
    }
    if (p_entry->codec_state_data.supported_framing == AUDIO_UNFRAMED_NOT_SUPPORTED &&
        p_qos_cfg->framing == AUDIO_UNFRAMED)
    {
        *p_resp_code = ASE_CP_RESP_REJECTED_PARAM;
        *p_reason = ASE_CP_REASON_FRAMING;
        goto error;
    }
    return true;
error:
    if (p_resp_code != NULL && p_reason != NULL)
    {
        PROTOCOL_PRINT_ERROR2("ase_check_cfg_qos_param: failed, resp_code 0x%x, reason 0x%x",
                              *p_resp_code, *p_reason);
    }

    return false;
}

T_ASCS_ASE_CP *ase_check_cp_data(uint16_t conn_handle, uint16_t cid, uint8_t *p_data, uint16_t len)
{
    T_ASE_CP_OP op;
    T_ASE_CP_NOTIFY_DATA *p_notify_data = NULL;
    T_ASE_CP_NOTIFY_ARRAY_PARAM *p_resp = NULL;
    T_ASE_CP_NOTIFY_DATA cp_resp;
    T_ASCS_ASE_CP *p_parsed_data = NULL;
    uint8_t response_code = 0;
    uint8_t ase_num;
    uint8_t ase_idx = 0;
    uint16_t idx = 0;
    uint8_t ase_id;
    T_ASE_ENTRY *p_entry = NULL;
    uint16_t cb_result = BLE_AUDIO_CB_RESULT_SUCCESS;

    if (p_data == NULL)
    {
        return NULL;
    }
    op = (T_ASE_CP_OP)p_data[idx++];
    if (op > ASE_CP_OP_RELEASE || op == ASE_CP_OP_NONE)
    {
        response_code = ASE_CP_RESP_UNSUPPORTED_OPCODE;
        goto failed1;
    }
    if (ase_cp_check_msg_len(op, p_data, len) == false)
    {
        response_code = ASE_CP_RESP_INVALID_LENGTH;
        goto failed1;
    }
    ase_num = p_data[idx];
    idx++;
    if (ase_num == 0 && idx != len)
    {
        response_code = ASE_CP_RESP_INVALID_LENGTH;
        goto failed1;
    }
    p_notify_data = ble_audio_mem_zalloc(sizeof(T_ASE_CP_NOTIFY_DATA) + (ase_num - 1) * sizeof(
                                             T_ASE_CP_NOTIFY_ARRAY_PARAM));
    if (p_notify_data == NULL)
    {
        return NULL;
    }
    p_parsed_data = ble_audio_mem_zalloc(sizeof(T_ASCS_ASE_CP));
    if (p_parsed_data == NULL)
    {
        ble_audio_mem_free(p_notify_data);
        return NULL;
    }
    p_notify_data->opcode = op;
    p_notify_data->number_of_ase = ase_num;
    p_parsed_data->op = op;
    p_parsed_data->number_of_ase = ase_num;

    for (; idx < len;)
    {
        p_resp = &p_notify_data->param[ase_idx];
        ase_id = p_data[idx];
        PROTOCOL_PRINT_INFO4("ase_check_cp_data: start, idx %d, len %d, op %d, ase_id %d", idx, len, op,
                             ase_id);
        p_entry = ase_find_by_ase_id(conn_handle, ase_id);
        if (p_entry != NULL)
        {
            if ((op == ASE_CP_OP_REC_START_READY || op == ASE_CP_OP_REC_STOP_READY) &&
                p_entry->direction == SERVER_AUDIO_SINK)
            {
                p_resp->ase_id = ase_id;
                p_resp->response_code = ASE_CP_RESP_INVALID_ASE_DIRECTION;
                p_resp->reason = 0;
                goto failed2;
            }
            else if (ase_cp_check_sm_state(p_entry->ase_state, p_entry->direction, op,
                                           true) == false)
            {
                p_resp->ase_id = ase_id;
                p_resp->response_code = ASE_CP_RESP_INVALID_ASE_SM_TRANSITION;
                p_resp->reason = 0;
                goto failed2;
            }
        }
        else
        {
            p_resp->ase_id = ase_id;
            p_resp->response_code = ASE_CP_RESP_INVALID_ASE_ID;
            p_resp->reason = 0;
            goto failed2;
        }

        if (op == ASE_CP_OP_CONFIG_CODEC)
        {
            T_ASE_CP_CODEC_CFG_ARRAY_PARAM *p_codec_cfg;
            p_codec_cfg = (T_ASE_CP_CODEC_CFG_ARRAY_PARAM *)&p_data[idx];
            idx += sizeof(T_ASE_CP_CODEC_CFG_ARRAY_PARAM);

            if (ase_check_cfg_codec_param(p_entry, p_codec_cfg, &p_data[idx],  &p_resp->response_code,
                                          &p_resp->reason, &p_parsed_data->data.config_codec.param[ase_idx].codec_parsed_data) == false)
            {
                p_resp->ase_id = ase_id;
                goto failed2;
            }
            p_parsed_data->data.config_codec.conn_handle = conn_handle;
            p_parsed_data->data.config_codec.number_of_ase = ase_num;
            memcpy(&p_parsed_data->data.config_codec.param[ase_idx].data, p_codec_cfg,
                   sizeof(T_ASE_CP_CODEC_CFG_ARRAY_PARAM));
            p_parsed_data->data.config_codec.param[ase_idx].p_codec_spec_cfg = &p_data[idx];
            if (p_codec_cfg->codec_spec_cfg_len)
            {
                idx += p_codec_cfg->codec_spec_cfg_len;
            }
        }
        else if (op == ASE_CP_OP_CONFIG_QOS)
        {
            T_ASE_ENTRY *p_entry_check = NULL;
            T_ASE_CP_QOS_CFG_ARRAY_PARAM *p_qos_cfg;
            p_qos_cfg = (T_ASE_CP_QOS_CFG_ARRAY_PARAM *)&p_data[idx];
            idx += sizeof(T_ASE_CP_QOS_CFG_ARRAY_PARAM);
            if (ase_check_cfg_qos_param(p_entry, p_qos_cfg, &p_resp->response_code, &p_resp->reason) == false)
            {
                p_resp->ase_id = ase_id;
                goto failed2;
            }
            p_parsed_data->data.config_qos.conn_handle = conn_handle;
            p_parsed_data->data.config_qos.number_of_ase = ase_num;
            memcpy(&p_parsed_data->data.config_qos.param[ase_idx], p_qos_cfg,
                   sizeof(T_ASE_CP_QOS_CFG_ARRAY_PARAM));
            p_entry_check = ase_find_by_cig_cis(conn_handle, (T_AUDIO_DIRECTION)p_entry->direction,
                                                p_qos_cfg->cig_id, p_qos_cfg->cis_id);
            if (p_entry_check != NULL && p_entry_check != p_entry)
            {
                p_resp->ase_id = ase_id;
                p_resp->response_code = ASE_CP_RESP_INVALID_PARAM;
                p_resp->reason = ASE_CP_REASON_INVALID_ASE_CIS_MAPPING;
                goto failed2;
            }
        }
        else if (op == ASE_CP_OP_ENABLE || op == ASE_CP_OP_UPDATE_METADATA)
        {
            uint8_t metadata_len;
            uint8_t *p_metadata;
            uint16_t available_contexts = 0xffff;
            idx++;
            metadata_len = p_data[idx];
            idx++;
            p_metadata = &p_data[idx];
            idx += metadata_len;
            if (metadata_len != 0)
            {
                if (metadata_check_format(metadata_len, p_metadata, &p_resp->response_code,
                                          &p_resp->reason, &available_contexts) == false)
                {
                    p_resp->ase_id = ase_id;
                    goto failed2;
                }
                if (available_contexts != 0xffff)
                {
                    T_ASCS_AUDIO_CONTEXTS_CHECK_IND context_check;
                    context_check.conn_handle = conn_handle;
                    context_check.ase_id = ase_id;
                    context_check.direction = p_entry->direction;
                    if (op == ASE_CP_OP_ENABLE)
                    {
                        context_check.is_update_metadata = false;
                    }
                    else
                    {
                        context_check.is_update_metadata = true;
                    }
                    context_check.available_contexts = available_contexts;
                    if (ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_AUDIO_CONTEXTS_CHECK_IND,
                                               &context_check) == BLE_AUDIO_CB_RESULT_REJECT)
                    {
                        p_resp->response_code = ASE_CP_RESP_REJECTED_METADATA;
                        p_resp->reason = METADATA_TYPE_STREAMING_AUDIO_CONTEXTS;
                        p_resp->ase_id = ase_id;
                        goto failed2;
                    }
                }
            }
            if (op == ASE_CP_OP_ENABLE)
            {
                p_parsed_data->data.enable.conn_handle = conn_handle;
                p_parsed_data->data.enable.number_of_ase = ase_num;
                p_parsed_data->data.enable.param[ase_idx].ase_id = ase_id;
                p_parsed_data->data.enable.param[ase_idx].metadata_length = metadata_len;
                p_parsed_data->data.enable.param[ase_idx].p_metadata = p_metadata;
            }
            else
            {
                p_parsed_data->data.update_metadata.conn_handle = conn_handle;
                p_parsed_data->data.update_metadata.number_of_ase = ase_num;
                p_parsed_data->data.update_metadata.param[ase_idx].ase_id = ase_id;
                p_parsed_data->data.update_metadata.param[ase_idx].metadata_length = metadata_len;
                p_parsed_data->data.update_metadata.param[ase_idx].p_metadata = p_metadata;
            }
        }
        else if (op == ASE_CP_OP_REC_START_READY || op == ASE_CP_OP_REC_STOP_READY ||
                 op == ASE_CP_OP_RELEASE || op == ASE_CP_OP_DISABLE)
        {
            idx += 1;
            p_parsed_data->data.ase_id_data.conn_handle = conn_handle;
            p_parsed_data->data.ase_id_data.number_of_ase = ase_num;
            p_parsed_data->data.ase_id_data.ase_id[ase_idx] = ase_id;
        }

        p_resp->ase_id = ase_id;
        p_resp->response_code = 0;
        p_resp->reason = 0;
        ase_idx++;
    }

    switch (op)
    {
    case ASE_CP_OP_CONFIG_CODEC:
        {
            cb_result = ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CP_CONFIG_CODEC_IND,
                                               &p_parsed_data->data.config_codec);
        }
        break;

    case ASE_CP_OP_CONFIG_QOS:
        {
            cb_result = ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CP_CONFIG_QOS_IND,
                                               &p_parsed_data->data.config_qos);
        }
        break;
    case ASE_CP_OP_ENABLE:
        {
            cb_result = ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CP_ENABLE_IND, &p_parsed_data->data.enable);
        }
        break;

    case ASE_CP_OP_DISABLE:
        {
            cb_result = ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CP_DISABLE_IND, &p_parsed_data->data.disable);
        }
        break;

    case ASE_CP_OP_UPDATE_METADATA:
        {
            cb_result = ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CP_UPDATE_METADATA_IND,
                                               &p_parsed_data->data.update_metadata);
        }
        break;

    case ASE_CP_OP_RELEASE:
        {
            cb_result = ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CP_RELEASE_IND,
                                               &p_parsed_data->data.ase_id_data);
        }
        break;
    default:
        break;
    }
    if (cb_result == BLE_AUDIO_CB_RESULT_SUCCESS)
    {
        ascs_send_ase_cp_notify(conn_handle, cid, (uint8_t *)p_notify_data,
                                sizeof(T_ASE_CP_NOTIFY_DATA) + (ase_num - 1) * sizeof(T_ASE_CP_NOTIFY_ARRAY_PARAM));
    }
    else
    {
        PROTOCOL_PRINT_ERROR2("ase_check_cp_data: app reject, cb_result 0x%x , ase_idx %d", cb_result,
                              ase_idx);
        if (cb_result == ASE_CP_RESP_INSUFFICIENT_RESOURCE || cb_result == ASE_CP_RESP_UNSPECIFIED_ERROR)
        {
            for (uint8_t i = 0; i < ase_idx; i++)
            {
                p_resp = &p_notify_data->param[i];
                p_resp->response_code = (uint8_t)cb_result;
            }
        }
        ascs_send_ase_cp_notify(conn_handle, cid, (uint8_t *)p_notify_data,
                                sizeof(T_ASE_CP_NOTIFY_DATA) + (ase_num - 1) * sizeof(T_ASE_CP_NOTIFY_ARRAY_PARAM));
        if (p_parsed_data)
        {
            ble_audio_mem_free(p_parsed_data);
        }
        p_parsed_data = NULL;
    }
    ble_audio_mem_free(p_notify_data);
    return p_parsed_data;
failed1:
    cp_resp.opcode = op;
    cp_resp.number_of_ase = 0xFF;
    cp_resp.param[0].ase_id = 0;
    cp_resp.param[0].response_code = response_code;
    cp_resp.param[0].reason = 0;
    ascs_send_ase_cp_notify(conn_handle, cid, (uint8_t *)&cp_resp, sizeof(cp_resp));
    return NULL;
failed2:
    if (p_parsed_data)
    {
        ble_audio_mem_free(p_parsed_data);
    }
    p_notify_data->number_of_ase = ase_idx + 1;
    ascs_send_ase_cp_notify(conn_handle, cid, (uint8_t *)p_notify_data,
                            sizeof(T_ASE_CP_NOTIFY_DATA) + ase_idx * sizeof(T_ASE_CP_NOTIFY_ARRAY_PARAM));
    ble_audio_mem_free(p_notify_data);
    return NULL;
}
#endif

