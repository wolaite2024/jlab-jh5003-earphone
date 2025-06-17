#include <string.h>
#include "ascs_client.h"
#include "trace.h"
#include "ascs_client_cp.h"
#include "ase_handle_cp.h"
#include "gap_cig_mgr.h"
#include "ble_audio_mgr.h"
#include "ble_link_util.h"

#if LE_AUDIO_ASCS_CLIENT_SUPPORT
bool ase_cp_send_rec_start_ready(uint16_t conn_handle, uint8_t ase_id);
bool ase_cp_send_rec_stop_ready(uint16_t conn_handle, uint8_t ase_id);

bool ascs_client_setup_data_path(T_ASE_CHAR_ENTRY *p_entry)
{
    uint8_t path_direction;
    uint8_t codec_id[CODEC_ID_LEN] = {TRANSPARENT_CODEC_ID, 0, 0, 0, 0};
    if (p_entry->direction == SERVER_AUDIO_SINK)
    {
        path_direction = DATA_PATH_ADD_INPUT;
        p_entry->added_path_direction = DATA_PATH_INPUT_FLAG;
    }
    else
    {
        path_direction = DATA_PATH_ADD_OUTPUT;
        p_entry->added_path_direction = DATA_PATH_OUTPUT_FLAG;
    }

    if (cig_mgr_setup_data_path(p_entry->cis_conn_handle, path_direction,
                                DATA_PATH_ID_HCI, codec_id, 0,
                                0, NULL) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    p_entry->added_path_direction = 0;
    return false;
}

bool ascs_client_remove_data_path(T_ASE_CHAR_ENTRY *p_entry)
{
    uint8_t path_direction;
    if (p_entry->direction == SERVER_AUDIO_SINK)
    {
        path_direction = DATA_PATH_INPUT_FLAG;
    }
    else
    {
        path_direction = DATA_PATH_OUTPUT_FLAG;
    }
    if (cig_mgr_remove_data_path(p_entry->cis_conn_handle, path_direction) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    return false;
}

void ascs_client_check_state(uint16_t conn_handle, T_ASE_CHAR_ENTRY *p_entry)
{
    uint8_t data_path_flag;

    if (p_entry == NULL)
    {
        return;
    }
    if (p_entry->direction == SERVER_AUDIO_SINK)
    {
        data_path_flag = DATA_PATH_INPUT_FLAG;
    }
    else
    {
        data_path_flag = DATA_PATH_OUTPUT_FLAG;
    }
    T_ISOCH_INFO isoch_info = {(T_ISOCH_STATE)0};

    if (p_entry->cis_conn_handle != 0)
    {
        if (cig_mgr_get_isoch_info(p_entry->cis_conn_handle, &isoch_info) == false)
        {
            PROTOCOL_PRINT_ERROR1("ascs_client_check_state: iso info get fail!  id: %d",
                                  p_entry->ase_id);
            return;
        }
    }

    PROTOCOL_PRINT_INFO5("ascs_client_check_state: cis_conn_handle 0x%x, id: %d, ase_state: 0x%x, iso state: %d, data_path_adding: %d",
                         p_entry->cis_conn_handle, p_entry->ase_id,
                         p_entry->ase_state, isoch_info.state, isoch_info.data_path_adding);
    switch (p_entry->ase_state)
    {
    case ASE_STATE_IDLE:
        {
            p_entry->release_local = false;
            if (isoch_info.state == ISOCH_STATE_CONN && isoch_info.data_path_flags == 0)
            {
                cig_mgr_disconnect(p_entry->cis_conn_handle, HCI_ERR_REMOTE_USER_TERMINATE);
            }
        }
        break;

    case ASE_STATE_CODEC_CONFIGURED:
        {

        }
        break;

    case ASE_STATE_QOS_CONFIGURED:
        {
            if ((isoch_info.state == ISOCH_STATE_CONN)
                && ((isoch_info.data_path_flags & data_path_flag) != 0)
                && isoch_info.data_path_removing == false)
            {
                ascs_client_remove_data_path(p_entry);
            }
        }
        break;

    case ASE_STATE_ENABLING:
        {
            if (isoch_info.state == ISOCH_STATE_CONN)
            {
                if (p_entry->direction == SERVER_AUDIO_SOURCE &&
                    (p_entry->app_handshake == false ||
                     (p_entry->app_handshake == true && p_entry->app_handshake_rec == true)))
                {
                    if ((isoch_info.data_path_flags & data_path_flag) != 0 && p_entry->rec_start_ready_send == false)
                    {
                        if (ase_cp_send_rec_start_ready(conn_handle, p_entry->ase_id))
                        {
                            p_entry->rec_start_ready_send = true;
                            p_entry->app_handshake_rec = false;
                        }
                    }
                    if (isoch_info.data_path_adding == false)
                    {
                        if ((isoch_info.data_path_flags & data_path_flag) == 0)
                        {
                            ascs_client_setup_data_path(p_entry);
                        }
                    }
                }
            }
        }
        break;

    case ASE_STATE_STREAMING:
        {
            if ((isoch_info.state == ISOCH_STATE_CONN) &&
                ((isoch_info.data_path_flags & data_path_flag) == 0) &&
                isoch_info.data_path_adding == false)
            {
                ascs_client_setup_data_path(p_entry);
            }
        }
        break;

    case ASE_STATE_DISABLING:
        {
            if (isoch_info.state == ISOCH_STATE_CONN && isoch_info.data_path_removing == false)
            {
                if (p_entry->direction == SERVER_AUDIO_SOURCE &&
                    (p_entry->app_handshake == false ||
                     (p_entry->app_handshake == true && p_entry->app_handshake_rec == true)))
                {
                    if ((isoch_info.data_path_flags & data_path_flag) == 0 && p_entry->rec_stop_ready_send == false)
                    {
                        if (ase_cp_send_rec_stop_ready(conn_handle, p_entry->ase_id))
                        {
                            p_entry->rec_stop_ready_send = true;
                            p_entry->app_handshake_rec = false;
                        }
                    }
                    else
                    {
                        ascs_client_remove_data_path(p_entry);
                    }
                }
            }
            else
            {
                if (isoch_info.state == ISOCH_STATE_IDLE && p_entry->direction == SERVER_AUDIO_SOURCE &&
                    (p_entry->app_handshake == false ||
                     (p_entry->app_handshake == true && p_entry->app_handshake_rec == true)))
                {
                    if (p_entry->rec_stop_ready_send == false)
                    {
                        if (ase_cp_send_rec_stop_ready(conn_handle, p_entry->ase_id))
                        {
                            p_entry->rec_stop_ready_send = true;
                            p_entry->app_handshake_rec = false;
                        }
                    }
                }
            }
        }
        break;

    case ASE_STATE_RELEASING:
        {
            if (isoch_info.state == ISOCH_STATE_CONN && isoch_info.data_path_removing == false)
            {
                if ((isoch_info.data_path_flags & data_path_flag) != 0)
                {
                    ascs_client_remove_data_path(p_entry);
                }
                if (isoch_info.data_path_flags == 0 && p_entry->release_local)
                {
                    cig_mgr_disconnect(p_entry->cis_conn_handle, HCI_ERR_REMOTE_USER_TERMINATE);
                }
            }
        }
        break;

    default:
        break;
    }
}

void ascs_client_handle_cig_info(uint8_t conn_id, uint16_t cis_conn_handle, uint8_t cb_type,
                                 T_CIG_MGR_CB_DATA *p_data)
{
    uint8_t i;
    T_CIS_INFO cis_info;
    T_ASE_CHAR_ENTRY *p_entry = NULL;
    uint16_t conn_handle = le_get_conn_handle(conn_id);
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_id(conn_id);
    T_ASCS_CLIENT_DB *p_ascs_db = ascs_check_link(conn_handle);

    if (cig_mgr_get_cis_info(cis_conn_handle, &cis_info) == false)
    {
        return;
    }
    if (p_link != NULL && p_ascs_db != NULL)
    {
        for (i = 0; i < (p_ascs_db->snk_ase_num + p_ascs_db->src_ase_num); i++)
        {
            p_entry = &p_ascs_db->ase_data[i];
            if ((p_entry->used == true)
                && (p_entry->qos_state_data.cig_id == cis_info.cig_id))
            {
                if (p_entry->qos_state_data.cis_id == cis_info.cis_id)
                {
                    if (cb_type == MSG_CIG_MGR_CIS_ESTABLISHED_INFO)
                    {
                        T_ISOCH_INFO isoch_info;
                        if (cig_mgr_get_isoch_info(cis_conn_handle, &isoch_info))
                        {
                            if (isoch_info.data_path_flags != 0 && p_entry->added_path_direction == 0)
                            {
                                PROTOCOL_PRINT_INFO1("ascs_client_handle_cig_info: data path not remove, data_path_flags 0x%x",
                                                     isoch_info.data_path_flags);
                                if (p_entry->direction == SERVER_AUDIO_SINK)
                                {
                                    if (isoch_info.data_path_flags & DATA_PATH_INPUT_FLAG)
                                    {
                                        p_entry->added_path_direction = DATA_PATH_INPUT_FLAG;
                                    }
                                }
                                else
                                {
                                    if (isoch_info.data_path_flags & DATA_PATH_OUTPUT_FLAG)
                                    {
                                        p_entry->added_path_direction = DATA_PATH_OUTPUT_FLAG;
                                    }
                                }
                                if (p_entry->added_path_direction)
                                {
                                    T_ASCS_CLIENT_SETUP_DATA_PATH set_data_path;
                                    set_data_path.conn_handle = conn_handle;
                                    set_data_path.ase_id = p_entry->ase_id;
                                    set_data_path.path_direction = p_entry->added_path_direction;
                                    set_data_path.cis_conn_handle = cis_conn_handle;
                                    set_data_path.codec_cfg_len = p_entry->codec_spec_cfg_len;
                                    set_data_path.p_codec_cfg = p_entry->p_codec_spec_cfg;
                                    codec_cfg_parse(p_entry->codec_spec_cfg_len, p_entry->p_codec_spec_cfg,
                                                    &set_data_path.codec_parsed_data);
                                    ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CLIENT_SETUP_DATA_PATH, &set_data_path);
                                }
                            }

                        }
                    }
                    else if (cb_type == MSG_CIG_MGR_SETUP_DATA_PATH)
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
                        if (p_entry->added_path_direction == data_path_flags)
                        {
                            if (p_data->p_cig_mgr_setup_data_path_rsp->cause == GAP_SUCCESS)
                            {
                                T_ASCS_CLIENT_SETUP_DATA_PATH set_data_path;
                                set_data_path.conn_handle = conn_handle;
                                set_data_path.ase_id = p_entry->ase_id;
                                set_data_path.path_direction = p_entry->added_path_direction;
                                set_data_path.cis_conn_handle = p_data->p_cig_mgr_setup_data_path_rsp->cis_conn_handle;
                                set_data_path.codec_cfg_len = p_entry->codec_spec_cfg_len;
                                set_data_path.p_codec_cfg = p_entry->p_codec_spec_cfg;
                                codec_cfg_parse(p_entry->codec_spec_cfg_len, p_entry->p_codec_spec_cfg,
                                                &set_data_path.codec_parsed_data);
                                ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CLIENT_SETUP_DATA_PATH, &set_data_path);
                            }
                            else
                            {
                                p_entry->added_path_direction = 0;
                            }
                        }
                    }
                    else if (cb_type == MSG_CIG_MGR_REMOVE_DATA_PATH)
                    {
                        if (p_entry->added_path_direction == p_data->p_cig_mgr_remove_data_path_rsp->data_path_removing_path
                            &&
                            p_data->p_cig_mgr_remove_data_path_rsp->cause == GAP_SUCCESS)
                        {
                            T_ASCS_CLIENT_REMOVE_DATA_PATH remove_data_path;
                            remove_data_path.conn_handle = conn_handle;
                            remove_data_path.ase_id = p_entry->ase_id;
                            remove_data_path.path_direction = p_entry->added_path_direction;
                            remove_data_path.cause = 0;
                            remove_data_path.cis_conn_handle = p_data->p_cig_mgr_remove_data_path_rsp->cis_conn_handle;
                            ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CLIENT_REMOVE_DATA_PATH, &remove_data_path);
                            p_entry->added_path_direction = 0;
                        }
                    }
                    else if (cb_type == MSG_CIG_MGR_DISCONNECT_INFO)
                    {
                        if (p_entry->added_path_direction != 0)
                        {
                            T_ISOCH_INFO isoch_info;
                            T_ASCS_CLIENT_REMOVE_DATA_PATH remove_data_path;
                            if (cig_mgr_get_isoch_info(p_entry->cis_conn_handle, &isoch_info))
                            {
                                if (isoch_info.data_path_flags)
                                {
                                    cig_mgr_remove_data_path(p_entry->cis_conn_handle, isoch_info.data_path_flags);
                                }
                            }
                            remove_data_path.conn_handle = conn_handle;
                            remove_data_path.ase_id = p_entry->ase_id;
                            remove_data_path.path_direction = p_entry->added_path_direction;
                            remove_data_path.cis_conn_handle = p_data->p_cig_mgr_disconnect_info->cis_conn_handle;
                            remove_data_path.cause = p_data->p_cig_mgr_disconnect_info->cause;
                            ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CLIENT_REMOVE_DATA_PATH, &remove_data_path);
                            p_entry->added_path_direction = 0;
                        }
                        if (p_entry->ase_state == ASE_STATE_IDLE ||
                            p_entry->ase_state == ASE_STATE_CODEC_CONFIGURED)
                        {
                            p_entry->cis_conn_handle = 0;
                            memset(&p_entry->qos_state_data, 0, sizeof(T_ASE_QOS_CFG_STATE_DATA));
                        }
                    }
                    ascs_client_check_state(conn_handle, p_entry);
                }
            }
        }
    }
}

T_APP_RESULT ascs_client_cig_cb(uint8_t cig_id, uint8_t cb_type, void *p_cb_data)
{
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_CIG_MGR_CB_DATA *p_data = (T_CIG_MGR_CB_DATA *)p_cb_data;

    PROTOCOL_PRINT_INFO1("ascs_client_cig_cb: cig_id %d", cig_id);

    switch (cb_type)
    {
    case MSG_CIG_MGR_START_SETTING:
        {
            PROTOCOL_PRINT_INFO2("[BAP][ISOCH] MSG_CIG_MGR_START_SETTING: cause 0x%x, cis_count 0x%x",
                                 p_data->p_cig_mgr_start_setting_rsp->cause,
                                 p_data->p_cig_mgr_start_setting_rsp->cis_count);
        }
        break;

    case MSG_CIG_MGR_REMOVE_CIG:
        {
            PROTOCOL_PRINT_INFO2("[BAP][ISOCH] MSG_CIG_MGR_REMOVE_CIG: cause 0x%x, cig_id %d",
                                 p_data->p_cig_mgr_remove_cig_rsp->cause,
                                 p_data->p_cig_mgr_remove_cig_rsp->cig_id);
        }
        break;

    case MSG_CIG_MGR_CREATE_CIS:
        {
            PROTOCOL_PRINT_INFO2("[BAP][ISOCH] MSG_CIG_MGR_CREATE_CIS: cause 0x%x, cis_count 0x%x",
                                 p_data->p_cig_mgr_create_cis_rsp->cause,
                                 p_data->p_cig_mgr_create_cis_rsp->cis_count);
            for (uint8_t i = 0; i < p_data->p_cig_mgr_create_cis_rsp->cis_count; i++)
            {
                PROTOCOL_PRINT_INFO4("[BAP][ISOCH] cis_info[%d]: cis_id 0x%x, state 0x%x, cis_conn_handle 0x%x", i,
                                     p_data->p_cig_mgr_create_cis_rsp->cis_info[i].cis_id,
                                     p_data->p_cig_mgr_create_cis_rsp->cis_info[i].state,
                                     p_data->p_cig_mgr_create_cis_rsp->cis_info[i].cis_conn_handle);
            }
        }
        break;

    case MSG_CIG_MGR_CIS_ESTABLISHED_INFO:
        {
            PROTOCOL_PRINT_INFO5("[BAP][ISOCH] MSG_CIG_MGR_CIS_ESTABLISHED_INFO: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_sync_delay 0x%x, cis_sync_delay 0x%x",
                                 p_data->p_cig_mgr_cis_established_info->cause,
                                 p_data->p_cig_mgr_cis_established_info->conn_id,
                                 p_data->p_cig_mgr_cis_established_info->cis_conn_handle,
                                 p_data->p_cig_mgr_cis_established_info->cig_sync_delay,
                                 p_data->p_cig_mgr_cis_established_info->cis_sync_delay);
            PROTOCOL_PRINT_INFO5("[BAP][ISOCH] MSG_CIG_MGR_CIS_ESTABLISHED_INFO: transport_latency_m_s 0x%x, transport_latency_s_m 0x%x, phy_m_s 0x%x, phy_s_m 0x%x, nse 0x%x",
                                 p_data->p_cig_mgr_cis_established_info->transport_latency_m_s,
                                 p_data->p_cig_mgr_cis_established_info->transport_latency_s_m,
                                 p_data->p_cig_mgr_cis_established_info->phy_m_s,
                                 p_data->p_cig_mgr_cis_established_info->phy_s_m,
                                 p_data->p_cig_mgr_cis_established_info->nse);
            PROTOCOL_PRINT_INFO7("[BAP][ISOCH] MSG_CIG_MGR_CIS_ESTABLISHED_INFO: bn_m_s 0x%x, bn_s_m 0x%x, ft_m_s 0x%x, ft_s_m 0x%x, max_pdu_m_s 0x%x, max_pdu_s_m 0x%x, iso_interval 0x%x",
                                 p_data->p_cig_mgr_cis_established_info->bn_m_s,
                                 p_data->p_cig_mgr_cis_established_info->bn_s_m,
                                 p_data->p_cig_mgr_cis_established_info->ft_m_s,
                                 p_data->p_cig_mgr_cis_established_info->ft_s_m,
                                 p_data->p_cig_mgr_cis_established_info->max_pdu_m_s,
                                 p_data->p_cig_mgr_cis_established_info->max_pdu_s_m,
                                 p_data->p_cig_mgr_cis_established_info->iso_interval);
            if (p_data->p_cig_mgr_cis_established_info->cause == GAP_SUCCESS)
            {
                ascs_client_handle_cig_info(p_data->p_cig_mgr_cis_established_info->conn_id,
                                            p_data->p_cig_mgr_cis_established_info->cis_conn_handle, cb_type, p_data);
            }
        }
        break;

    case MSG_CIG_MGR_SETUP_DATA_PATH:
        {
            PROTOCOL_PRINT_INFO3("[BAP][ISOCH] MSG_CIG_MGR_SETUP_DATA_PATH: cause 0x%x, cis_conn_handle 0x%x, conn_id %d",
                                 p_data->p_cig_mgr_setup_data_path_rsp->cause,
                                 p_data->p_cig_mgr_setup_data_path_rsp->cis_conn_handle,
                                 p_data->p_cig_mgr_setup_data_path_rsp->conn_id);
            if (p_data->p_cig_mgr_remove_data_path_rsp->cause == GAP_SUCCESS)
            {
                ascs_client_handle_cig_info(p_data->p_cig_mgr_setup_data_path_rsp->conn_id,
                                            p_data->p_cig_mgr_setup_data_path_rsp->cis_conn_handle, cb_type, p_data);
            }
        }
        break;

    case MSG_CIG_MGR_REMOVE_DATA_PATH:
        {
            PROTOCOL_PRINT_INFO3("[BAP][ISOCH] MSG_CIG_MGR_REMOVE_DATA_PATH: cause 0x%x, cis_conn_handle 0x%x, conn_id %d",
                                 p_data->p_cig_mgr_remove_data_path_rsp->cause,
                                 p_data->p_cig_mgr_remove_data_path_rsp->cis_conn_handle,
                                 p_data->p_cig_mgr_remove_data_path_rsp->conn_id);
            if (p_data->p_cig_mgr_remove_data_path_rsp->cause == GAP_SUCCESS)
            {
                ascs_client_handle_cig_info(p_data->p_cig_mgr_remove_data_path_rsp->conn_id,
                                            p_data->p_cig_mgr_remove_data_path_rsp->cis_conn_handle, cb_type, p_data);
            }
        }
        break;

    case MSG_CIG_MGR_DISCONNECT:
        {
            PROTOCOL_PRINT_INFO3("[BAP][ISOCH] MSG_CIG_MGR_DISCONNECT: conn_id %d, cis_conn_handle 0x%x, cause 0x%x",
                                 p_data->p_cig_mgr_disconnect_rsp->conn_id, p_data->p_cig_mgr_disconnect_rsp->cis_conn_handle,
                                 p_data->p_cig_mgr_disconnect_rsp->cause);
        }
        break;

    case MSG_CIG_MGR_DISCONNECT_INFO:
        {
            PROTOCOL_PRINT_INFO3("[BAP][ISOCH] MSG_CIG_MGR_DISCONNECT_INFO: conn_id %d, cis_conn_handle 0x%x, cause 0x%x",
                                 p_data->p_cig_mgr_disconnect_info->conn_id, p_data->p_cig_mgr_disconnect_info->cis_conn_handle,
                                 p_data->p_cig_mgr_disconnect_info->cause);
            ascs_client_handle_cig_info(p_data->p_cig_mgr_disconnect_info->conn_id,
                                        p_data->p_cig_mgr_disconnect_info->cis_conn_handle, cb_type, p_data);
        }
        break;

    default:
        break;
    }
    return result;
}

T_ASE_CHAR_ENTRY *ase_cp_check_state(uint16_t conn_handle, T_ASE_CP_OP cp_op, uint8_t ase_id)
{
    T_ASE_CHAR_ENTRY *p_entry = ascs_find_ase_by_ase_id(conn_handle, ase_id);
    if (p_entry)
    {
        if (ase_cp_check_sm_state(p_entry->ase_state, p_entry->direction, cp_op, true))
        {
            return p_entry;
        }
    }
    return NULL;
}

bool ase_cp_config_codec(uint16_t conn_handle, uint8_t num_ases,
                         T_ASE_CP_CODEC_CFG_PARAM *p_codec)
{
    uint8_t *p_buf = NULL;
    uint16_t size;
    uint16_t idx = 0;
    uint8_t i;
    T_ASCS_CLIENT_DB *p_ascs_db = ascs_check_link(conn_handle);
    if (p_codec == NULL || p_ascs_db == NULL || num_ases == 0)
    {
        goto error;
    }
    for (i = 0; i < num_ases; i++)
    {
        if (ase_cp_check_state(conn_handle, ASE_CP_OP_CONFIG_CODEC, p_codec[i].ase_id) == NULL)
        {
            goto error;
        }
    }

    size = num_ases * sizeof(T_ASE_CP_CODEC_CFG_PARAM) + 2;
    p_buf = ble_audio_mem_zalloc(size);
    if (p_buf == NULL)
    {
        goto error;
    }
    p_buf[idx++] = ASE_CP_OP_CONFIG_CODEC;
    p_buf[idx++] = num_ases;
    for (uint8_t i = 0; i < num_ases; i++)
    {
        uint16_t block_size;
        block_size = p_codec[i].codec_spec_cfg_len + sizeof(T_ASE_CP_CODEC_CFG_ARRAY_PARAM);
        memcpy(p_buf + idx, &p_codec[i], block_size);
        idx += block_size;
    }
    if (ascs_send_ase_cp(conn_handle, p_buf, idx, false) == false)
    {
        goto error;
    }
    if (p_buf)
    {
        ble_audio_mem_free(p_buf);
    }
    return true;
error:
    if (p_buf)
    {
        ble_audio_mem_free(p_buf);
    }
    PROTOCOL_PRINT_ERROR0("ase_cp_config_codec: failed");
    return false;
}

bool ase_cp_config_qos(uint16_t conn_handle, uint8_t num_ases,
                       T_ASE_CP_QOS_CFG_ARRAY_PARAM *p_qos)
{
    uint8_t *p_buf = NULL;
    uint16_t size;
    uint16_t idx = 0;
    uint8_t i;
    uint8_t ase_num = 0;
    T_ASCS_CLIENT_DB *p_ascs_db = ascs_check_link(conn_handle);
    if (p_qos == NULL || p_ascs_db == NULL || num_ases == 0)
    {
        goto error;
    }
    size = num_ases * sizeof(T_ASE_CP_QOS_CFG_ARRAY_PARAM) + 2;
    p_buf = ble_audio_mem_zalloc(size);

    if (p_buf == NULL)
    {
        goto error;
    }
    for (i = 0; i < num_ases; i++)
    {
        if (ase_cp_check_state(conn_handle, ASE_CP_OP_CONFIG_QOS, p_qos[i].ase_id) == NULL)
        {
            goto error;
        }
    }
    p_buf[idx++] = ASE_CP_OP_CONFIG_QOS;
    idx++;
    for (i = 0; i < num_ases; i++)
    {
        ase_num++;
        memcpy(p_buf + idx, &p_qos[i], sizeof(T_ASE_CP_QOS_CFG_ARRAY_PARAM));
        idx += sizeof(T_ASE_CP_QOS_CFG_ARRAY_PARAM);
    }
    if (ase_num)
    {
        p_buf[1] = ase_num;
        if (ascs_send_ase_cp(conn_handle, p_buf, idx, false) == false)
        {
            goto error;
        }
    }

    if (p_buf)
    {
        ble_audio_mem_free(p_buf);
    }
    return true;
error:
    if (p_buf)
    {
        ble_audio_mem_free(p_buf);
    }
    PROTOCOL_PRINT_ERROR0("ase_cp_config_qos: failed");
    return false;
}

bool ase_cp_enable(uint16_t conn_handle, uint8_t num_ases,
                   T_ASE_CP_METADATA_PARAM *p_metadata)
{
    uint8_t *p_buf = NULL;
    uint16_t size = 0;
    uint16_t idx = 0;
    uint8_t i;
    if (p_metadata == NULL || num_ases == 0)
    {
        goto error;
    }
    for (i = 0; i < num_ases; i++)
    {
        if (ase_cp_check_state(conn_handle, ASE_CP_OP_ENABLE, p_metadata[i].ase_id) == NULL)
        {
            goto error;
        }
        size += (p_metadata[i].metadata_len + 2);
    }
    size += 2;
    p_buf = ble_audio_mem_zalloc(size);
    if (p_buf == NULL)
    {
        goto error;
    }
    p_buf[idx++] = ASE_CP_OP_ENABLE;
    p_buf[idx++] = num_ases;
    for (uint8_t i = 0; i < num_ases; i++)
    {
        p_buf[idx++] = p_metadata[i].ase_id;
        p_buf[idx++] = p_metadata[i].metadata_len;
        memcpy(p_buf + idx, p_metadata[i].p_data, p_metadata[i].metadata_len);
        idx += p_metadata[i].metadata_len;
    }
    if (ascs_send_ase_cp(conn_handle, p_buf, idx, false) == false)
    {
        goto error;
    }
    if (p_buf)
    {
        ble_audio_mem_free(p_buf);
    }
    return true;
error:
    if (p_buf)
    {
        ble_audio_mem_free(p_buf);
    }
    PROTOCOL_PRINT_ERROR0("ase_cp_enable: failed");
    return false;
}

bool ase_cp_update_metadata(uint16_t conn_handle, uint8_t num_ases,
                            T_ASE_CP_METADATA_PARAM *p_metadata)
{
    uint8_t *p_buf = NULL;
    uint16_t size = 0;
    uint16_t idx = 0;
    uint8_t i;
    if (p_metadata == NULL || num_ases == 0)
    {
        goto error;
    }
    for (i = 0; i < num_ases; i++)
    {
        if (ase_cp_check_state(conn_handle, ASE_CP_OP_UPDATE_METADATA, p_metadata[i].ase_id) == NULL)
        {
            goto error;
        }
        size += (p_metadata[i].metadata_len + 2);
    }
    size += 2;
    p_buf = ble_audio_mem_zalloc(size);
    if (p_buf == NULL)
    {
        goto error;
    }
    p_buf[idx++] = ASE_CP_OP_UPDATE_METADATA;
    p_buf[idx++] = num_ases;
    for (uint8_t i = 0; i < num_ases; i++)
    {
        p_buf[idx++] = p_metadata[i].ase_id;
        p_buf[idx++] = p_metadata[i].metadata_len;
        memcpy(p_buf + idx, p_metadata[i].p_data, p_metadata[i].metadata_len);
        idx += p_metadata[i].metadata_len;
    }
    if (ascs_send_ase_cp(conn_handle, p_buf, idx, false) == false)
    {
        goto error;
    }
    if (p_buf)
    {
        ble_audio_mem_free(p_buf);
    }
    return true;
error:
    if (p_buf)
    {
        ble_audio_mem_free(p_buf);
    }
    PROTOCOL_PRINT_ERROR0("ase_cp_update_metadata: failed");
    return false;
}

bool ase_cp_send_rec_start_ready(uint16_t conn_handle, uint8_t ase_id)
{
    uint8_t data[4];
    uint16_t idx = 0;

    data[idx++] = ASE_CP_OP_REC_START_READY;
    data[idx++] = 1;
    data[idx++] = ase_id;
    if (ascs_send_ase_cp(conn_handle, data, idx, false) == false)
    {
        goto error;
    }
    return true;
error:
    PROTOCOL_PRINT_ERROR0("ase_cp_send_rec_start_ready: failed");
    return false;
}

bool ase_cp_rec_start_ready(uint16_t conn_handle, uint8_t ase_id)
{
    T_ASE_CHAR_ENTRY *p_entry = ascs_find_ase_by_ase_id(conn_handle, ase_id);
    if (p_entry == NULL)
    {
        goto error;
    }
    if (ase_cp_check_state(conn_handle, ASE_CP_OP_REC_START_READY, ase_id) == NULL)
    {
        goto error;
    }

    if (p_entry->app_handshake)
    {
        p_entry->app_handshake_rec = true;
        ascs_client_check_state(conn_handle, p_entry);
    }
    else
    {
        goto error;
    }

    return true;
error:
    PROTOCOL_PRINT_ERROR0("ase_cp_rec_start_ready: failed");
    return false;
}

bool ase_cp_send_rec_stop_ready(uint16_t conn_handle, uint8_t ase_id)
{
    uint8_t data[4];
    uint16_t idx = 0;

    data[idx++] = ASE_CP_OP_REC_STOP_READY;
    data[idx++] = 1;
    data[idx++] = ase_id;
    if (ascs_send_ase_cp(conn_handle, data, idx, false) == false)
    {
        goto error;
    }
    return true;
error:
    PROTOCOL_PRINT_ERROR0("ase_cp_send_rec_stop_ready: failed");
    return false;
}

bool ase_cp_rec_stop_ready(uint16_t conn_handle, uint8_t ase_id)
{
    T_ASE_CHAR_ENTRY *p_entry = ascs_find_ase_by_ase_id(conn_handle, ase_id);
    if (p_entry == NULL)
    {
        goto error;
    }
    if (ase_cp_check_state(conn_handle, ASE_CP_OP_REC_STOP_READY, ase_id) == NULL)
    {
        goto error;
    }

    if (p_entry->app_handshake)
    {
        p_entry->app_handshake_rec = true;
        ascs_client_check_state(conn_handle, p_entry);
    }
    else
    {
        goto error;
    }

    return true;
error:
    PROTOCOL_PRINT_ERROR0("ase_cp_rec_stop_ready: failed");
    return false;
}

bool ase_cp_disable(uint16_t conn_handle, uint8_t num_ases, uint8_t *p_ase_id)
{
    uint8_t *p_buf = NULL;
    uint16_t size;
    uint16_t idx = 0;
    uint8_t i;
    if (p_ase_id == NULL || num_ases == 0)
    {
        goto error;
    }
    for (i = 0; i < num_ases; i++)
    {
        if (ase_cp_check_state(conn_handle, ASE_CP_OP_DISABLE, p_ase_id[i]) == NULL)
        {
            goto error;
        }
    }
    size = num_ases + 2;
    p_buf = ble_audio_mem_zalloc(size);
    if (p_buf == NULL)
    {
        goto error;
    }
    p_buf[idx++] = ASE_CP_OP_DISABLE;
    p_buf[idx++] = num_ases;
    memcpy(p_buf + idx, p_ase_id, num_ases);
    idx += num_ases;
    if (ascs_send_ase_cp(conn_handle, p_buf, idx, false) == false)
    {
        goto error;
    }
    ble_audio_mem_free(p_buf);
    return true;
error:
    if (p_buf)
    {
        ble_audio_mem_free(p_buf);
    }
    PROTOCOL_PRINT_ERROR0("ase_cp_disable: failed");
    return false;
}

bool ase_cp_release(uint16_t conn_handle, uint8_t num_ases, uint8_t *p_ase_id)
{
    uint8_t *p_buf = NULL;
    uint16_t size;
    uint16_t idx = 0;
    uint8_t i;
    if (p_ase_id == NULL || num_ases == 0)
    {
        goto error;
    }
    for (i = 0; i < num_ases; i++)
    {
        if (ase_cp_check_state(conn_handle, ASE_CP_OP_RELEASE, p_ase_id[i]) == NULL)
        {
            goto error;
        }
    }
    size = num_ases + 2;
    p_buf = ble_audio_mem_zalloc(size);
    if (p_buf == NULL)
    {
        goto error;
    }
    p_buf[idx++] = ASE_CP_OP_RELEASE;
    p_buf[idx++] = num_ases;
    memcpy(p_buf + idx, p_ase_id, num_ases);
    idx += num_ases;
    if (ascs_send_ase_cp(conn_handle, p_buf, idx, false) == false)
    {
        goto error;
    }
    ble_audio_mem_free(p_buf);
    return true;
error:
    if (p_buf)
    {
        ble_audio_mem_free(p_buf);
    }
    PROTOCOL_PRINT_ERROR0("ase_cp_release: failed");
    return false;
}
#endif
