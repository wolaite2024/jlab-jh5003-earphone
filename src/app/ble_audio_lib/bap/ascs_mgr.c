#include <string.h>
#include "trace.h"
#include "ascs_mgr.h"
#include "ascs.h"
#include "bt_types.h"
#include "ase_handle_cp.h"
#include "ble_audio_dm.h"
#include "ble_audio_mgr.h"
#include "gap_cig_mgr.h"
#include "ase_sm.h"
#include "ble_link_util.h"
#include "ble_audio_ual.h"
#include "calculate_util.h"

#if LE_AUDIO_ASCS_SUPPORT

bool ascs_action_cfg_codec(uint16_t conn_handle, uint8_t ase_id,
                           T_ASE_CP_CODEC_CFG_ARRAY_PARAM *p_param, uint8_t *p_codec_data)
{
    T_ASE_ENTRY *p_entry = NULL;
    p_entry = ase_find_by_ase_id(conn_handle, ase_id);
    if (p_entry == NULL)
    {
        goto error;
    }
    if (ase_action_cfg_codec(conn_handle, p_entry, p_param, p_codec_data, false) == false)
    {
        goto error;
    }
    return true;
error:
    PROTOCOL_PRINT_ERROR2("ascs_action_cfg_codec: failed, conn_handle 0x%x, ase_id %d", conn_handle,
                          ase_id);
    return false;
}

bool ascs_action_disable(uint16_t conn_handle, uint8_t ase_id)
{
    T_ASE_ENTRY *p_entry = NULL;
    p_entry = ase_find_by_ase_id(conn_handle, ase_id);
    if (p_entry == NULL)
    {
        goto error;
    }
    if (ase_action_disable(conn_handle, p_entry, false) == false)
    {
        goto error;
    }
    return true;
error:
    PROTOCOL_PRINT_ERROR2("ascs_action_disable: failed, conn_handle 0x%x, ase_id %d", conn_handle,
                          ase_id);
    return false;
}

bool ascs_action_release(uint16_t conn_handle, uint8_t ase_id)
{
    T_ASE_ENTRY *p_entry = NULL;
    p_entry = ase_find_by_ase_id(conn_handle, ase_id);
    T_ISOCH_INFO isoch_info;
    if (p_entry == NULL)
    {
        goto error;
    }

    if (cig_mgr_get_isoch_info(p_entry->cis_conn_handle, &isoch_info))
    {
        if (isoch_info.state == ISOCH_STATE_CONN)
        {
            uint8_t data_path_flag;
            if (p_entry->direction == SERVER_AUDIO_SINK)
            {
                data_path_flag = DATA_PATH_INPUT_FLAG;
            }
            else
            {
                data_path_flag = DATA_PATH_OUTPUT_FLAG;
            }
            if ((isoch_info.data_path_flags & data_path_flag) != 0)
            {
                goto error;
            }
        }
    }
    if (ase_action_release(conn_handle, p_entry, false) == false)
    {
        goto error;
    }
    else
    {
        if (isoch_info.state == ISOCH_STATE_CONN ||
            isoch_info.state == ISOCH_STATE_CONNECTING)
        {
            cig_mgr_disconnect(p_entry->cis_conn_handle, HCI_ERR_REMOTE_USER_TERMINATE);
        }
    }
    return true;
error:
    PROTOCOL_PRINT_ERROR2("ascs_action_release: failed, conn_handle 0x%x, ase_id %d", conn_handle,
                          ase_id);
    return false;
}

bool ascs_action_release_by_cig(uint16_t conn_handle, uint8_t cig_id)
{
    T_BLE_AUDIO_LINK *p_link = NULL;
    T_ASE_ENTRY *p_entry = NULL;
    bool ret = false;

    p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link != NULL && p_link->p_ase_entry != NULL)
    {
        uint8_t i;
        T_ASE_ENTRY *p_ase_table = p_link->p_ase_entry;
        for (i = 0; i < (ble_audio_db.snk_ase_num + ble_audio_db.src_ase_num); i++)
        {
            p_entry = &p_ase_table[i];
            if (p_entry->ase_state < ASE_STATE_QOS_CONFIGURED || p_entry->ase_state == ASE_STATE_RELEASING)
            {
                continue;
            }
            if (p_entry->qos_state_data.cig_id != cig_id)
            {
                continue;
            }
            if (ase_action_release(conn_handle, p_entry, false) == false)
            {
                goto error;
            }
            else
            {
                ret = true;
                T_ISOCH_INFO isoch_info;
                if (cig_mgr_get_isoch_info(p_entry->cis_conn_handle, &isoch_info))
                {
                    if (isoch_info.state == ISOCH_STATE_CONN ||
                        isoch_info.state == ISOCH_STATE_CONNECTING)
                    {
                        cig_mgr_disconnect(p_entry->cis_conn_handle, HCI_ERR_REMOTE_USER_TERMINATE);
                    }
                }

            }
        }
    }
    if (ret == false)
    {
        goto error;
    }
    return true;
error:
    PROTOCOL_PRINT_ERROR2("ascs_action_release_by_cig: failed, conn_handle 0x%x, cig_id %d",
                          conn_handle, cig_id);
    return false;
}

bool ascs_action_update_metadata(uint16_t conn_handle, uint8_t ase_id,
                                 uint8_t metadata_len, uint8_t *p_metadata)
{
    T_ASE_ENTRY *p_entry = NULL;
    p_entry = ase_find_by_ase_id(conn_handle, ase_id);
    if (p_entry == NULL)
    {
        goto error;
    }
    if (ase_action_update_metadata(conn_handle, p_entry, metadata_len, p_metadata, false) == false)
    {
        goto error;
    }
    return true;
error:
    PROTOCOL_PRINT_ERROR2("ascs_action_update_metadata: failed, conn_handle 0x%x, ase_id %d",
                          conn_handle,
                          ase_id);
    return false;
}

bool ascs_app_ctl_handshake(uint16_t conn_handle, uint8_t ase_id, bool app_handshake)
{
    T_ASE_ENTRY *p_entry = NULL;
    p_entry = ase_find_by_ase_id(conn_handle, ase_id);
    if (p_entry == NULL)
    {
        return false;
    }
    p_entry->app_handshake = app_handshake;
    return true;
}

bool ascs_action_rec_start_ready(uint16_t conn_handle, uint8_t ase_id)
{
    T_ASE_ENTRY *p_entry = NULL;
    p_entry = ase_find_by_ase_id(conn_handle, ase_id);
    if (p_entry == NULL)
    {
        goto error;
    }
    if (ase_cp_check_sm_state(p_entry->ase_state, p_entry->direction,
                              ASE_CP_OP_REC_START_READY, false) == false)
    {
        goto error;
    }
    if (p_entry->app_handshake)
    {
        p_entry->app_handshake_rec = true;
        if (ase_sm_check_iosch_status(ASE_SM_CHECK_ENABLE, p_entry))
        {
            ase_switch_to_streaming(conn_handle, p_entry);
        }
        else
        {
            ascs_check_current_state(conn_handle, p_entry);
        }
        return true;
    }
    else
    {
        goto error;
    }
error:
    return false;
}

bool ascs_action_rec_stop_ready(uint16_t conn_handle, uint8_t ase_id)
{
    T_ASE_ENTRY *p_entry = NULL;
    p_entry = ase_find_by_ase_id(conn_handle, ase_id);
    if (p_entry == NULL)
    {
        goto error;
    }
    if (ase_cp_check_sm_state(p_entry->ase_state, p_entry->direction,
                              ASE_CP_OP_REC_STOP_READY, false) == false)
    {
        goto error;
    }
    if (p_entry->app_handshake)
    {
        p_entry->app_handshake_rec = true;
        if (ase_sm_check_iosch_status(ASE_SM_CHECK_DISABLE, p_entry))
        {
            ase_switch_to_qos_cfg(conn_handle, p_entry);
        }
        else
        {
            ascs_check_current_state(conn_handle, p_entry);
        }
        return true;
    }
    else
    {
        goto error;
    }
error:
    return false;
}

void ase_handle_remote_cp(uint16_t conn_handle, T_ASCS_ASE_CP *p_ase_cp)
{
    uint16_t idx = 0;
    uint8_t ase_id;
    T_ASE_ENTRY *p_entry = NULL;

    switch (p_ase_cp->op)
    {
    case ASE_CP_OP_CONFIG_CODEC:
        {
            T_ASE_CP_CONFIG_CODEC_DATA *p_codec_data;
            uint16_t min_transport_latency = 0;
            uint32_t sdu_interval_m_s = 0;
            uint32_t sdu_interval_s_m = 0;

            for (idx = 0; idx < p_ase_cp->number_of_ase; idx++)
            {
                T_CODEC_CFG codec_cfg = {0};
                uint32_t sdu_interval_temp = 0;
                p_codec_data = &p_ase_cp->data.config_codec.param[idx];
                p_entry = ase_find_by_ase_id(conn_handle, p_codec_data->data.ase_id);
                if (p_entry)
                {
                    if (codec_cfg_parse(p_codec_data->data.codec_spec_cfg_len, p_codec_data->p_codec_spec_cfg,
                                        &codec_cfg))
                    {
                        if (codec_sdu_interval_get(&codec_cfg, &sdu_interval_temp))
                        {
                            if (p_entry->direction == SERVER_AUDIO_SINK)
                            {
                                if (sdu_interval_m_s < sdu_interval_temp)
                                {
                                    sdu_interval_m_s = sdu_interval_temp;
                                }
                            }
                            else
                            {
                                if (sdu_interval_s_m < sdu_interval_temp)
                                {
                                    sdu_interval_s_m = sdu_interval_temp;
                                }
                            }
                        }
                    }
                }
            }

            if (sdu_interval_m_s != 0 && sdu_interval_s_m != 0)
            {
                /*Unframed PDUs shall only be used when the ISO_Interval is equal to or an integer multiple of the SDU_Interval.
                 The latency shall larger than ISO_Interval.*/
                uint32_t value =  least_common_multiple(sdu_interval_m_s, sdu_interval_s_m);
                min_transport_latency = value / 1000;
                if (value % 1000 != 0)
                {
                    min_transport_latency++;
                }
                PROTOCOL_PRINT_INFO3("ase_handle_remote_cp: ASE_CP_OP_CONFIG_CODEC, sdu_interval_m_s %d, sdu_interval_s_m %d, min_transport_latency %d",
                                     sdu_interval_m_s, sdu_interval_s_m, min_transport_latency);
            }

            for (idx = 0; idx < p_ase_cp->number_of_ase; idx++)
            {
                p_codec_data = &p_ase_cp->data.config_codec.param[idx];
                ase_id = p_codec_data->data.ase_id;
                p_entry = ase_find_by_ase_id(conn_handle, ase_id);
                if (p_entry == NULL)
                {
                    goto error;
                }
                p_entry->min_transport_latency = min_transport_latency;
                ase_action_cfg_codec(conn_handle, p_entry, &p_codec_data->data, p_codec_data->p_codec_spec_cfg,
                                     true);
            }
        }
        break;

    case ASE_CP_OP_CONFIG_QOS:
        {
            for (idx = 0; idx < p_ase_cp->number_of_ase; idx++)
            {
                T_ASE_CP_QOS_CFG_ARRAY_PARAM *p_qos_cfg = &p_ase_cp->data.config_qos.param[idx];
                ase_id = p_qos_cfg->ase_id;
                p_entry = ase_find_by_ase_id(conn_handle, ase_id);
                if (p_entry == NULL)
                {
                    goto error;
                }
                ase_action_cfg_qos(conn_handle, p_entry, p_qos_cfg);
            }
        }
        break;

    case ASE_CP_OP_ENABLE:
        {
            for (idx = 0; idx < p_ase_cp->number_of_ase; idx++)
            {
                T_ASE_CP_METADATA_DATA *p_data = &p_ase_cp->data.enable.param[idx];
                ase_id = p_data->ase_id;
                p_entry = ase_find_by_ase_id(conn_handle, ase_id);
                if (p_entry == NULL)
                {
                    goto error;
                }
                ase_action_enable(conn_handle, p_entry, p_data->metadata_length, p_data->p_metadata);
            }
        }
        break;

    case ASE_CP_OP_UPDATE_METADATA:
        {
            for (idx = 0; idx < p_ase_cp->number_of_ase; idx++)
            {
                T_ASE_CP_METADATA_DATA *p_data = &p_ase_cp->data.enable.param[idx];
                ase_id = p_data->ase_id;
                p_entry = ase_find_by_ase_id(conn_handle, ase_id);
                if (p_entry == NULL)
                {
                    goto error;
                }
                ase_action_update_metadata(conn_handle, p_entry, p_data->metadata_length, p_data->p_metadata, true);
            }
        }
        break;

    case ASE_CP_OP_REC_START_READY:
        {
            for (idx = 0; idx < p_ase_cp->number_of_ase; idx++)
            {
                ase_id = p_ase_cp->data.ase_id_data.ase_id[idx];
                p_entry = ase_find_by_ase_id(conn_handle, ase_id);
                if (p_entry == NULL)
                {
                    goto error;
                }
                ase_action_rec_start_ready(conn_handle, p_entry);
            }
        }
        break;

    case ASE_CP_OP_REC_STOP_READY:
        {
            for (idx = 0; idx < p_ase_cp->number_of_ase; idx++)
            {
                ase_id = p_ase_cp->data.ase_id_data.ase_id[idx];
                p_entry = ase_find_by_ase_id(conn_handle, ase_id);
                if (p_entry == NULL)
                {
                    goto error;
                }
                ase_action_rec_stop_ready(conn_handle, p_entry);
            }
        }
        break;

    case ASE_CP_OP_DISABLE:
        {
            for (idx = 0; idx < p_ase_cp->number_of_ase; idx++)
            {
                ase_id = p_ase_cp->data.ase_id_data.ase_id[idx];
                p_entry = ase_find_by_ase_id(conn_handle, ase_id);
                if (p_entry == NULL)
                {
                    goto error;
                }
                ase_action_disable(conn_handle, p_entry, true);
            }
        }
        break;

    case ASE_CP_OP_RELEASE:
        {
            for (idx = 0; idx < p_ase_cp->number_of_ase; idx++)
            {
                ase_id = p_ase_cp->data.ase_id_data.ase_id[idx];
                p_entry = ase_find_by_ase_id(conn_handle, ase_id);
                if (p_entry == NULL)
                {
                    goto error;
                }
                ase_action_release(conn_handle, p_entry, true);
            }
        }
        break;

    default:
        break;
    }

    return;
error:
    return;
}

bool ase_sm_check_iosch_status(T_ASE_SM_CHECK_TYPE type, T_ASE_ENTRY *p_entry)
{
    T_ISOCH_INFO isoch_info;
    uint8_t data_path_flag;
    bool ret = cig_mgr_get_isoch_info(p_entry->cis_conn_handle, &isoch_info);
    if (p_entry->direction == SERVER_AUDIO_SINK)
    {
        data_path_flag = DATA_PATH_OUTPUT_FLAG;
    }
    else
    {
        data_path_flag = DATA_PATH_INPUT_FLAG;
    }
    if (type == ASE_SM_CHECK_ENABLE)
    {
        if (ret)
        {
            if ((isoch_info.state == ISOCH_STATE_CONN)
                && ((isoch_info.data_path_flags & data_path_flag) != 0))
            {
                return true;
            }
        }
    }
    else if (type == ASE_SM_CHECK_DISABLE)
    {
        if (ret)
        {
            if ((isoch_info.state == ISOCH_STATE_IDLE)
                || ((isoch_info.state == ISOCH_STATE_CONN)
                    && ((isoch_info.data_path_flags & data_path_flag) == 0)))
            {
                return true;
            }
            PROTOCOL_PRINT_ERROR2("ASE_SM_CHECK_DISABLE: failed, state %d, data_path_flags 0x%x",
                                  isoch_info.state, isoch_info.data_path_flags);

        }
        else
        {
            return true;
        }
    }
    else if (type == ASE_SM_CHECK_RELEASE)
    {
        if (ret)
        {
            if (isoch_info.state == ISOCH_STATE_IDLE)
            {
                return true;
            }
        }
        else
        {
            return true;
        }
    }
    PROTOCOL_PRINT_ERROR1("ase_sm_check_iosch_status: failed, check type %d", type);
    return false;
}

bool ascs_setup_data_path(T_ASE_ENTRY *p_entry)
{
    T_GAP_CAUSE cause;
    uint8_t path_direction;
    uint8_t codec_id[CODEC_ID_LEN] = {TRANSPARENT_CODEC_ID, 0, 0, 0, 0};
    if (p_entry->direction == SERVER_AUDIO_SINK)
    {
        path_direction = DATA_PATH_ADD_OUTPUT;
        p_entry->added_path_direction = DATA_PATH_OUTPUT_FLAG;
    }
    else
    {
        path_direction = DATA_PATH_ADD_INPUT;
        p_entry->added_path_direction = DATA_PATH_INPUT_FLAG;
    }

    if ((cause = cig_mgr_setup_data_path(p_entry->cis_conn_handle, path_direction,
                                         DATA_PATH_ID_HCI, codec_id, 0,
                                         0, NULL)) != GAP_CAUSE_SUCCESS)
    {
        p_entry->added_path_direction = 0;
        PROTOCOL_PRINT_INFO1("[BAP][ISOCH] ascs_setup_data_path fail cause 0x%x", cause);
        return false;
    }

    return true;
}

bool ascs_remove_data_path(T_ASE_ENTRY *p_entry)
{
    uint8_t path_direction;
    if (p_entry->direction == SERVER_AUDIO_SINK)
    {
        path_direction = DATA_PATH_OUTPUT_FLAG;
    }
    else
    {
        path_direction = DATA_PATH_INPUT_FLAG;
    }
    if (cig_mgr_remove_data_path(p_entry->cis_conn_handle, path_direction) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    return false;
}

void ascs_check_current_state(uint16_t conn_handle, T_ASE_ENTRY *p_entry)
{
    T_ISOCH_INFO isoch_info;
    uint8_t data_path_flag;
    bool isoch_exist = cig_mgr_get_isoch_info(p_entry->cis_conn_handle, &isoch_info);
    if (p_entry->direction == SERVER_AUDIO_SINK)
    {
        data_path_flag = DATA_PATH_OUTPUT_FLAG;
    }
    else
    {
        data_path_flag = DATA_PATH_INPUT_FLAG;
    }
    PROTOCOL_PRINT_INFO3("[BAP][ISOCH] ascs_check_current_state: ase_state %d, isoch_exist %d, data_path_flag 0x%x",
                         p_entry->ase_state, isoch_exist, data_path_flag);
    switch (p_entry->ase_state)
    {
    case ASE_STATE_QOS_CONFIGURED:
        {
            if ((isoch_info.state == ISOCH_STATE_CONN)
                && ((isoch_info.data_path_flags & data_path_flag) != 0)
                && isoch_info.data_path_removing == false)
            {
                ascs_remove_data_path(p_entry);
            }
        }
        break;

    case ASE_STATE_ENABLING:
        {
            if (isoch_exist && isoch_info.state == ISOCH_STATE_CONN &&
                isoch_info.data_path_adding == false)
            {
                if (p_entry->direction == SERVER_AUDIO_SOURCE)
                {
                    if ((isoch_info.data_path_flags & data_path_flag) == 0 && p_entry->rec_start_ready == true)
                    {
                        ascs_setup_data_path(p_entry);
                    }
                }
                else
                {
                    if (p_entry->app_handshake == false ||
                        (p_entry->app_handshake == true && p_entry->app_handshake_rec == true))
                    {
                        ascs_setup_data_path(p_entry);
                    }

                }
            }
        }
        break;

    case ASE_STATE_STREAMING:
        {
            if (isoch_exist && (isoch_info.state == ISOCH_STATE_CONN) &&
                ((isoch_info.data_path_flags & data_path_flag) == 0) &&
                isoch_info.data_path_adding == false)
            {
                ascs_setup_data_path(p_entry);
            }
        }
        break;

    case ASE_STATE_DISABLING:
        {
            if (isoch_exist && isoch_info.state == ISOCH_STATE_CONN && isoch_info.data_path_removing == false)
            {
                if (p_entry->direction == SERVER_AUDIO_SOURCE)
                {
                    if ((isoch_info.data_path_flags & data_path_flag) != 0 && p_entry->rec_stop_ready == true)
                    {
                        ascs_remove_data_path(p_entry);
                    }
                }
            }
        }
        break;

    case ASE_STATE_RELEASING:
        {
            if (isoch_exist && isoch_info.state == ISOCH_STATE_CONN && isoch_info.data_path_removing == false)
            {
                if ((isoch_info.data_path_flags & data_path_flag) != 0)
                {
                    ascs_remove_data_path(p_entry);
                }
            }
            if ((isoch_exist &&  isoch_info.state == ISOCH_STATE_IDLE)
                || isoch_exist == false)
            {
                ase_action_released(conn_handle, p_entry);
            }
        }
        break;

    default:
        break;
    }
}

T_APP_RESULT ascs_handle_cig_mgr_msg(uint8_t cb_type, void *p_cb_data)
{
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_CIG_MGR_CB_DATA *p_data = (T_CIG_MGR_CB_DATA *)p_cb_data;

    PROTOCOL_PRINT_INFO1("[BAP][ISOCH] ascs_handle_cig_mgr_msg: cb_type 0x%x", cb_type);

    switch (cb_type)
    {
    case MSG_CIG_MGR_CIS_ESTABLISHED_INFO:
        {
            if (p_data->p_cig_mgr_cis_established_info->cause == GAP_SUCCESS)
            {
                uint8_t conn_id;
                uint16_t conn_handle;
                T_CIS_INFO cis_info;
                T_ASE_ENTRY *p_entry_sink;
                T_ASE_ENTRY *p_entry_source;

                cig_mgr_get_conn_id(p_data->p_cig_mgr_cis_established_info->cis_conn_handle, &conn_id);
                cig_mgr_get_cis_info(p_data->p_cig_mgr_cis_established_info->cis_conn_handle, &cis_info);
                conn_handle = le_get_conn_handle(conn_id);
                p_entry_sink = ase_find_by_cig_cis(conn_handle, SERVER_AUDIO_SINK, cis_info.cig_id,
                                                   cis_info.cis_id);
                p_entry_source = ase_find_by_cig_cis(conn_handle, SERVER_AUDIO_SOURCE, cis_info.cig_id,
                                                     cis_info.cis_id);

                if (p_entry_sink)
                {
                    p_entry_sink->cis_conn_handle = p_data->p_cig_mgr_cis_established_info->cis_conn_handle;
                    ascs_check_current_state(conn_handle, p_entry_sink);
                }
                if (p_entry_source)
                {
                    p_entry_source->cis_conn_handle = p_data->p_cig_mgr_cis_established_info->cis_conn_handle;
                    ascs_check_current_state(conn_handle, p_entry_source);
                }

                if (p_entry_sink == NULL && p_entry_source == NULL)
                {
                    break;
                }

                T_ASCS_CIS_CONN_INFO cis_conn_info;
                cis_conn_info.conn_handle = conn_handle;
                cis_conn_info.cis_conn_handle = p_data->p_cig_mgr_cis_established_info->cis_conn_handle;
                cis_conn_info.cig_id = p_data->p_cig_mgr_cis_established_info->cig_id;
                cis_conn_info.cis_id = p_data->p_cig_mgr_cis_established_info->cis_id;
                ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CIS_CONN_INFO, &cis_conn_info);
            }
        }
        break;

    case MSG_CIG_MGR_SETUP_DATA_PATH:
        {
            T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_id(
                                           p_data->p_cig_mgr_setup_data_path_rsp->conn_id);
            if (p_link != NULL && p_link->p_ase_entry != NULL)
            {
                uint8_t i;
                T_ASE_ENTRY *p_ase_table = p_link->p_ase_entry;
                for (i = 0; i < (ble_audio_db.snk_ase_num + ble_audio_db.src_ase_num); i++)
                {
                    if (p_ase_table[i].cis_conn_handle == p_data->p_cig_mgr_setup_data_path_rsp->cis_conn_handle)
                    {
                        uint8_t data_path_flags = 0;
                        if (p_data->p_cig_mgr_setup_data_path_rsp->data_path_adding_path == DATA_PATH_ADD_INPUT)
                        {
                            data_path_flags |= DATA_PATH_INPUT_FLAG;
                        }
                        else if (p_data->p_cig_mgr_setup_data_path_rsp->data_path_adding_path == DATA_PATH_ADD_OUTPUT)
                        {
                            data_path_flags |= DATA_PATH_OUTPUT_FLAG;
                        }
                        if (p_ase_table[i].added_path_direction == data_path_flags)
                        {
                            if (p_data->p_cig_mgr_setup_data_path_rsp->cause == GAP_SUCCESS)
                            {
                                T_ASCS_SETUP_DATA_PATH set_data_path;
                                set_data_path.conn_handle = p_link->conn_handle;
                                set_data_path.ase_id = p_ase_table[i].ase_id;
                                set_data_path.path_direction = p_ase_table[i].added_path_direction;
                                set_data_path.cis_conn_handle = p_data->p_cig_mgr_setup_data_path_rsp->cis_conn_handle;
                                memcpy(&set_data_path.codec_parsed_data, &p_ase_table[i].codec_cfg, sizeof(T_CODEC_CFG));
                                ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_SETUP_DATA_PATH, &set_data_path);
                                if (p_ase_table[i].ase_state == ASE_STATE_ENABLING)
                                {
                                    if (ase_sm_check_iosch_status(ASE_SM_CHECK_ENABLE, &p_ase_table[i]))
                                    {
                                        ase_switch_to_streaming(p_link->conn_handle, &p_ase_table[i]);
                                    }
                                }
                            }
                            else
                            {
                                p_ase_table[i].added_path_direction = 0;
                            }
                        }
                        else
                        {
                            if (p_data->p_cig_mgr_setup_data_path_rsp->cause == GAP_SUCCESS)
                            {
                                if (p_ase_table[i].ase_state == ASE_STATE_ENABLING &&
                                    p_ase_table[i].direction == SERVER_AUDIO_SOURCE &&
                                    p_ase_table[i].rec_start_ready == true
                                   )
                                {
                                    ascs_check_current_state(p_link->conn_handle,  &p_ase_table[i]);
                                }
                            }
                        }
                    }
                }
            }
        }
        break;

    case MSG_CIG_MGR_REMOVE_DATA_PATH:
        {
            if (p_data->p_cig_mgr_remove_data_path_rsp->cause == GAP_SUCCESS)
            {
                T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_id(
                                               p_data->p_cig_mgr_remove_data_path_rsp->conn_id);
                if (p_link != NULL && p_link->p_ase_entry != NULL)
                {
                    uint8_t i;
                    T_ASE_ENTRY *p_ase_table = p_link->p_ase_entry;
                    for (i = 0; i < (ble_audio_db.snk_ase_num + ble_audio_db.src_ase_num); i++)
                    {
                        if (p_ase_table[i].cis_conn_handle == p_data->p_cig_mgr_remove_data_path_rsp->cis_conn_handle)
                        {
                            if (p_ase_table[i].added_path_direction ==
                                p_data->p_cig_mgr_remove_data_path_rsp->data_path_removing_path)
                            {
                                T_ASCS_REMOVE_DATA_PATH remove_data_path;
                                remove_data_path.conn_handle = p_link->conn_handle;
                                remove_data_path.ase_id = p_ase_table[i].ase_id;
                                remove_data_path.path_direction = p_ase_table[i].added_path_direction;
                                remove_data_path.cis_conn_handle = p_data->p_cig_mgr_remove_data_path_rsp->cis_conn_handle;
                                ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_REMOVE_DATA_PATH, &remove_data_path);
                                p_ase_table[i].added_path_direction = 0;
                            }
                            else
                            {
                                if (p_ase_table[i].ase_state == ASE_STATE_RELEASING)
                                {
                                    T_ISOCH_INFO isoch_info;
                                    if (cig_mgr_get_isoch_info(p_ase_table[i].cis_conn_handle, &isoch_info))
                                    {
                                        if (isoch_info.state == ISOCH_STATE_CONN &&  isoch_info.data_path_flags != 0 &&
                                            isoch_info.data_path_removing == false)
                                        {
                                            uint8_t data_path_flag;
                                            if (p_ase_table[i].direction == SERVER_AUDIO_SINK)
                                            {
                                                data_path_flag = DATA_PATH_OUTPUT_FLAG;
                                            }
                                            else
                                            {
                                                data_path_flag = DATA_PATH_INPUT_FLAG;
                                            }
                                            if ((isoch_info.data_path_flags & data_path_flag) != 0)
                                            {
                                                ascs_remove_data_path(&p_ase_table[i]);
                                            }
                                        }
                                    }
                                }
                            }
                            if (p_ase_table[i].ase_state == ASE_STATE_DISABLING)
                            {
                                if (ase_sm_check_iosch_status(ASE_SM_CHECK_DISABLE, &p_ase_table[i]))
                                {
                                    ase_switch_to_qos_cfg(p_link->conn_handle, &p_ase_table[i]);
                                }
                            }
                        }
                    }
                }
            }
        }
        break;

    case MSG_CIG_MGR_DISCONNECT: //fixme, add operation if cause is not success
        {

        }
        break;

    case MSG_CIG_MGR_DISCONNECT_INFO:
        {
            T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_id(
                                           p_data->p_cig_mgr_disconnect_info->conn_id);
            if (p_link != NULL && p_link->p_ase_entry != NULL)
            {
                uint8_t i;
                bool send_msg = false;
                T_ASE_ENTRY *p_ase_table = p_link->p_ase_entry;
                for (i = 0; i < (ble_audio_db.snk_ase_num + ble_audio_db.src_ase_num); i++)
                {
                    if (p_ase_table[i].cis_conn_handle == p_data->p_cig_mgr_disconnect_info->cis_conn_handle)
                    {
                        send_msg = true;
                        p_ase_table[i].cis_conn_handle = 0;
                        if (p_ase_table[i].added_path_direction != 0)
                        {
                            T_ASCS_REMOVE_DATA_PATH remove_data_path;
                            remove_data_path.conn_handle = p_link->conn_handle;
                            remove_data_path.ase_id = p_ase_table[i].ase_id;
                            remove_data_path.path_direction = p_ase_table[i].added_path_direction;
                            remove_data_path.cis_conn_handle = p_data->p_cig_mgr_disconnect_info->cis_conn_handle;
                            ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_REMOVE_DATA_PATH, &remove_data_path);
                            p_ase_table[i].added_path_direction = 0;
                        }
                        if (p_ase_table[i].ase_state == ASE_STATE_RELEASING)
                        {
                            ase_action_released(p_link->conn_handle, &p_ase_table[i]);
                        }
                        else if (p_ase_table[i].ase_state == ASE_STATE_STREAMING ||
                                 p_ase_table[i].ase_state == ASE_STATE_DISABLING)
                        {
                            ascs_change_state(&p_ase_table[i], ASE_STATE_QOS_CONFIGURED);
                            ase_sm_send_notify(p_link->conn_handle, &p_ase_table[i]);
                            ascs_check_current_state(p_link->conn_handle, &p_ase_table[i]);
                        }
                    }
                }

                if (send_msg)
                {
                    T_ASCS_CIS_DISCONN_INFO cis_info;
                    cis_info.conn_handle = p_link->conn_handle;
                    cis_info.cis_conn_handle = p_data->p_cig_mgr_disconnect_info->cis_conn_handle;
                    cis_info.cause = p_data->p_cig_mgr_disconnect_info->cause;
                    cis_info.cig_id = p_data->p_cig_mgr_disconnect_info->cig_id;
                    cis_info.cis_id = p_data->p_cig_mgr_disconnect_info->cis_id;
                    ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CIS_DISCONN_INFO, &cis_info);
                }
            }
        }
        break;

    case MSG_CIG_MGR_CIS_REQUEST_IND:
        {
            uint16_t cb_result;
            T_ASCS_CIS_REQUEST_IND req_ind = {0};
            T_ASE_ENTRY *p_entry_sink;
            T_ASE_ENTRY *p_entry_source;
            uint16_t conn_handle = le_get_conn_handle(p_data->p_cig_mgr_cis_request_ind->conn_id);

            req_ind.cis_conn_handle = p_data->p_cig_mgr_cis_request_ind->cis_conn_handle;
            req_ind.conn_handle = conn_handle;

            p_entry_sink = ase_find_by_cig_cis(conn_handle,
                                               SERVER_AUDIO_SINK, p_data->p_cig_mgr_cis_established_info->cig_id,
                                               p_data->p_cig_mgr_cis_established_info->cis_id);
            p_entry_source = ase_find_by_cig_cis(conn_handle,
                                                 SERVER_AUDIO_SOURCE,
                                                 p_data->p_cig_mgr_cis_established_info->cig_id,
                                                 p_data->p_cig_mgr_cis_established_info->cis_id);

            if (p_entry_sink)
            {
                req_ind.snk_ase_id = p_entry_sink->ase_id;
                req_ind.snk_ase_state = p_entry_sink->ase_state;
            }

            if (p_entry_source)
            {
                req_ind.src_ase_id = p_entry_source->ase_id;
                req_ind.src_ase_state = p_entry_source->ase_state;
            }

            if (p_entry_sink == NULL && p_entry_source == NULL)
            {
                break;
            }
            cb_result = ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CIS_REQUEST_IND, &req_ind);
            //APP_RESULT_ACCEPT, APP_RESULT_PENDING, HCI cause
            if (cb_result == BLE_AUDIO_CB_RESULT_SUCCESS)
            {
                result = APP_RESULT_ACCEPT;
            }
            else
            {
                result = (T_APP_RESULT)cb_result;
            }
        }
        break;

    }
    return result;
}

bool ascs_cis_request_cfm(uint16_t cis_conn_handle, bool accept, uint8_t reject_reason)
{
    T_GAP_CAUSE cause;
    if (accept)
    {
        cause = cig_mgr_acceptor_accept_cis(cis_conn_handle);
    }
    else
    {
        cause = cig_mgr_acceptor_reject_cis(cis_conn_handle, reject_reason);
    }
    if (cause == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    return false;
}

void ascs_check_ase_state_when_disconnect(T_BLE_AUDIO_LINK *p_link)
{
    if (p_link->p_ase_entry != NULL)
    {
        uint16_t cccd_handle;
        T_ASE_ENTRY *p_ase_table = p_link->p_ase_entry;
        uint16_t start_handle = server_get_start_handle(ascs_service_id);

        for (uint8_t i = 0; i < (ble_audio_db.snk_ase_num + ble_audio_db.src_ase_num); i++)
        {
            if (p_ase_table[i].ase_state != ASE_STATE_IDLE)
            {
                PROTOCOL_PRINT_TRACE2("[BAP][ASCS] ascs_check_ase_state_when_disconnect: i %d, ase_state %d",
                                      i, p_ase_table[i].ase_state);
                cccd_handle = start_handle + ASCS_CHAR_ASE_CCCD_INDEX + 3 * i;
                ble_audio_ual_set_pending_link_cccd(p_link, cccd_handle);
            }
        }
    }
}

void ascs_handle_pending_cccd(T_BLE_AUDIO_LINK *p_link, T_LE_SRV_CFG *p_srv)
{
    PROTOCOL_PRINT_TRACE2("[BAP][ASCS] ascs_handle_pending_cccd: start, conn_handle 0x%x, cccd_pending 0x%x",
                          p_link->conn_handle, p_srv->cccd_pending[0]);
    if (p_link->p_ase_entry != NULL)
    {
        uint16_t cccd_handle;
        uint16_t start_handle = server_get_start_handle(ascs_service_id);
        T_ASE_ENTRY *p_ase_table = p_link->p_ase_entry;

        for (uint8_t i = 0; i < (ble_audio_db.snk_ase_num + ble_audio_db.src_ase_num); i++)
        {
            uint16_t pending_flag = 0;

            cccd_handle = start_handle + ASCS_CHAR_ASE_CCCD_INDEX + 3 * i;
            pending_flag = (1 << i);
            if (p_srv->cccd_pending[0] & pending_flag)
            {
                uint8_t *p_data = NULL;
                uint16_t len;

                if (ase_gen_char_data(p_link->conn_handle, p_ase_table[i].ase_id, &p_data, &len))
                {
                    if (ascs_send_ase_notify(p_link->conn_handle, L2C_FIXED_CID_ATT, p_ase_table[i].ase_id, p_data,
                                             len))
                    {
                        if (ble_audio_ual_clear_pending_cccd(p_link->conn_handle, cccd_handle))
                        {
                            p_srv->cccd_pending[0] &= ~pending_flag;
                        }
                    }
                    ble_audio_mem_free(p_data);
                }

            }
        }
    }
}

static void ascs_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BLE_DM_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BLE_DM_EVENT_CONN_STATE:
        if (param->conn_state.state == GAP_CONN_STATE_CONNECTED)
        {
            PROTOCOL_PRINT_INFO0("ascs_dm_cback: GAP_CONN_STATE_CONNECTED");
            ase_sm_link_init(le_get_conn_handle(param->conn_state.conn_id));
        }
        else if (param->conn_state.state == GAP_CONN_STATE_DISCONNECTED)
        {
            T_BLE_AUDIO_LINK *p_link = NULL;
            PROTOCOL_PRINT_INFO0("ascs_dm_cback: GAP_CONN_STATE_DISCONNECTED");

            p_link = ble_audio_link_find_by_conn_id(param->conn_state.conn_id);

            if (p_link == NULL)
            {
                return;
            }
            ascs_check_ase_state_when_disconnect(p_link);
            if (p_link->p_ase_entry != NULL)
            {
                uint8_t i;
                T_ASCS_ASE_STATE ase_state;
                T_ASE_ENTRY *p_ase_table = p_link->p_ase_entry;
                ase_state.conn_handle = p_link->conn_handle;
                for (i = 0; i < (ble_audio_db.snk_ase_num + ble_audio_db.src_ase_num); i++)
                {
                    if (p_ase_table[i].ase_state != ASE_STATE_IDLE &&
                        p_ase_table[i].ase_state != ASE_STATE_RELEASING)
                    {
                        ascs_change_state(&p_ase_table[i], ASE_STATE_RELEASING);
                        if (ascs_get_ase_data(p_link->conn_handle, p_ase_table[i].ase_id, &ase_state.ase_data))
                        {
                            ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_ASE_STATE, &ase_state);
                        }
                    }
                    if (p_ase_table[i].ase_state != ASE_STATE_IDLE)
                    {
                        ascs_change_state(&p_ase_table[i], ASE_STATE_IDLE);
                        if (ascs_get_ase_data(p_link->conn_handle, p_ase_table[i].ase_id, &ase_state.ase_data))
                        {
                            ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_ASE_STATE, &ase_state);
                        }
                    }
                }
            }
            ase_sm_link_free(p_link->conn_handle);
        }
        break;

    case BLE_DM_EVENT_AUTHEN_INFO:
        {
            if (param->authen_info.new_state == GAP_AUTHEN_STATE_COMPLETE &&
                param->authen_info.cause == GAP_SUCCESS)
            {
                T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_id(param->authen_info.conn_id);
                if (p_link == NULL)
                {
                    return;
                }
                if (ble_audio_ual_check_le_bond(p_link->conn_handle))
                {
                    T_LE_SRV_CFG *p_srv;
                    p_srv = ble_srv_find_by_srv_id(p_link->conn_handle, ascs_service_id);
                    if (p_srv != NULL && p_srv->cccd_pending[0] != 0)
                    {
                        ascs_handle_pending_cccd(p_link, p_srv);
                    }
                }
            }
        }
        break;
    default:
        break;
    }
}

T_APP_RESULT ascs_attr_read_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                               uint16_t attrib_index,
                               uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    uint8_t *p_data = NULL;
    uint16_t len;
    uint8_t ase_id;

    PROTOCOL_PRINT_INFO2("ascs_attr_read_cb: attrib_index %d, offset %x", attrib_index, offset);

    ase_id = ascs_get_ase_id(attrib_index, false);
    if (ase_id == 0)
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    if (ase_gen_char_data(conn_handle, ase_id, &p_data, &len))
    {
        if (offset > len)
        {
            cause = APP_RESULT_INVALID_OFFSET;
        }
        else if (gatt_svc_read_confirm(conn_handle, cid, ascs_service_id, attrib_index,
                                       p_data + offset,
                                       len - offset,
                                       len,
                                       APP_RESULT_SUCCESS))
        {
            cause = APP_RESULT_PENDING;
        }
        ble_audio_mem_free(p_data);
    }

    return (cause);
}

T_APP_RESULT ascs_attr_write_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                uint16_t attrib_index,
                                T_WRITE_TYPE write_type,
                                uint16_t length, uint8_t *p_value, P_FUN_EXT_WRITE_IND_POST_PROC *p_write_post_proc)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;

    PROTOCOL_PRINT_INFO2("ascs_attr_write_cb: attrib_index %d, length %x", attrib_index, length);

    if (attrib_index == ASCS_CHAR_ASE_CP_INDEX)
    {
        if (p_value != NULL)
        {
            T_ASCS_ASE_CP *p_ase_cp;
            PROTOCOL_PRINT_INFO2("ascs_callback: control point data[%d]: %b",
                                 length, TRACE_BINARY(length, p_value));
            p_ase_cp = ase_check_cp_data(conn_handle, cid,
                                         p_value, length);
            if (p_ase_cp)
            {
                ase_handle_remote_cp(conn_handle, p_ase_cp);
                ble_audio_mem_free(p_ase_cp);
            }
        }
    }
    else
    {
        cause = APP_RESULT_ATTR_NOT_FOUND;
    }

    return (cause);
}

void ascs_cccd_update_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                         uint16_t attrib_index,
                         uint16_t ccc_bits)
{
    T_LE_SRV_CFG *p_srv = ble_srv_alloc_by_srv_id(conn_handle, service_id);
    if (p_srv == NULL)
    {
        return;
    }

    if (attrib_index == ASCS_CHAR_ASE_CP_CCCD_INDEX)
    {
        PROTOCOL_PRINT_INFO1("[BAP][ASCS] ASE CP Char: ccc_bits 0x%x", ccc_bits);
    }
    else
    {
        uint8_t ase_id = ascs_get_ase_id(attrib_index, true);
        if (ase_id != 0)
        {
            PROTOCOL_PRINT_INFO2("[BAP][ASCS] ASE Char: ase_id %d, ccc_bits 0x%x",
                                 ase_id, ccc_bits);
            if (ccc_bits & GATT_CLIENT_CHAR_NOTIF_IND_DATA_PENGDING)
            {
                uint16_t pending_flag = 0;
                uint8_t idx = ase_id - 1;
                pending_flag = (1 << idx);
                ble_srv_set_cccd_flags(p_srv, ccc_bits, 0, pending_flag, true);
            }
        }
    }
    ble_audio_mgr_send_server_cccd_info(conn_handle, p_ascs_attr_tbl, service_id, attrib_index,
                                        ccc_bits, 0, ascs_attr_num);
    return;
}

const T_FUN_GATT_EXT_SERVICE_CBS ascs_cbs =
{
    ascs_attr_read_cb,  // Read callback function pointer
    ascs_attr_write_cb, // Write callback function pointer
    ascs_cccd_update_cb  // CCCD update callback function pointer
};

void ascs_init(uint8_t snk_ase_num, uint8_t src_ase_num, bool init_gap)
{
#if LE_AUDIO_INIT_DEBUG
    PROTOCOL_PRINT_INFO2("ascs_init: snk_ase_num %d, src_ase_num %d",
                         snk_ase_num, src_ase_num);
#endif
    ble_audio_db.snk_ase_num = snk_ase_num;
    ble_audio_db.src_ase_num = src_ase_num;
    if (init_gap)
    {
        cig_mgr_reg_acceptor_cb(ascs_handle_cig_mgr_msg);
    }

    ble_dm_cback_register(ascs_dm_cback);
    ascs_service_id = ascs_add_service(&ascs_cbs);
}

#if LE_AUDIO_DEINIT
void ascs_deinit(void)
{
    for (uint8_t k = 0; k < ble_audio_db.acl_link_num; k++)
    {
        if (ble_audio_db.le_link[k].p_ase_entry != NULL)
        {
            uint8_t i;
            T_ASE_ENTRY *p_ase_table = ble_audio_db.le_link[k].p_ase_entry;
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
            ble_audio_mem_free(ble_audio_db.le_link[k].p_ase_entry);
            ble_audio_db.le_link[k].p_ase_entry = NULL;
        }
    }
    if (p_ascs_attr_tbl)
    {
        ble_audio_mem_free(p_ascs_attr_tbl);
        p_ascs_attr_tbl = NULL;
    }
    ble_audio_db.src_ase_num = 0;
    ble_audio_db.snk_ase_num = 0;
    ascs_service_id = 0xff;
}
#endif
#endif
