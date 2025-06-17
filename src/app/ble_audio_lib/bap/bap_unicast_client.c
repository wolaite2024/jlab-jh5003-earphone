#include <string.h>
#include "trace.h"
#include "ble_link_util.h"
#include "ble_audio.h"
#include "ascs_client.h"
#include "bap_unicast_client.h"
#include "ascs_client_cp.h"
#include "audio_stream_session_int.h"
#include "ble_audio_group_int.h"
#include "bap_int.h"
#include "sys_timer.h"

#if LE_AUDIO_ASCS_CLIENT_SUPPORT

typedef struct
{
    uint8_t sink_ase_num;
    uint8_t source_ase_num;
    uint8_t sink_channels;
    uint8_t sink_min_locations;
    uint8_t source_channels;
    uint8_t source_min_locations;
    uint8_t cis_num;
    uint8_t audio_stream_num;
} T_UNICAST_AUDIO_CFG_SERVER;

typedef struct
{
    uint8_t server_num;
    T_UNICAST_AUDIO_CFG_SERVER server_cfg[BAP_UNICAST_SERVERS_MAX_NUM];
} T_UNICAST_AUDIO_CFG;

const T_UNICAST_AUDIO_CFG UNICAST_CFG_TBL[UNICAST_AUDIO_CFG_MAX] =
{
    {0, .server_cfg[0] = {0, 0, 0, 0, 0, 0, 0, 0}, .server_cfg[1] = {0, 0, 0, 0, 0, 0, 0, 0}}, //UNICAST_AUDIO_CFG_UNKNOWN
    {1, .server_cfg[0] = {1, 0, 1, 0, 0, 0, 1, 1}, .server_cfg[1] = {0, 0, 0, 0, 0, 0, 0, 0}}, //UNICAST_AUDIO_CFG_1
    {1, .server_cfg[0] = {0, 1, 0, 0, 1, 1, 1, 1}, .server_cfg[1] = {0, 0, 0, 0, 0, 0, 0, 0}}, //UNICAST_AUDIO_CFG_2
    {1, .server_cfg[0] = {1, 1, 1, 1, 1, 1, 1, 2}, .server_cfg[1] = {0, 0, 0, 0, 0, 0, 0, 0}}, //UNICAST_AUDIO_CFG_3
    {1, .server_cfg[0] = {1, 0, 2, 2, 0, 0, 1, 1}, .server_cfg[1] = {0, 0, 0, 0, 0, 0, 0, 0}}, //UNICAST_AUDIO_CFG_4
    {1, .server_cfg[0] = {1, 1, 2, 0, 1, 0, 1, 2}, .server_cfg[1] = {0, 0, 0, 0, 0, 0, 0, 0}}, //UNICAST_AUDIO_CFG_5
    {1, .server_cfg[0] = {2, 0, 1, 2, 0, 0, 2, 2}, .server_cfg[1] = {0, 0, 0, 0, 0, 0, 0, 0}}, //UNICAST_AUDIO_CFG_6_I
    {2, .server_cfg[0] = {1, 0, 1, 1, 0, 0, 1, 1}, .server_cfg[1] = {1, 0, 1, 1, 0, 0, 1, 1}}, //UNICAST_AUDIO_CFG_6_II
    {1, .server_cfg[0] = {1, 1, 1, 0, 1, 0, 2, 2}, .server_cfg[1] = {0, 0, 0, 0, 0, 0, 0, 0}}, //UNICAST_AUDIO_CFG_7_I
    {2, .server_cfg[0] = {1, 0, 1, 0, 0, 0, 1, 1}, .server_cfg[1] = {0, 1, 0, 0, 1, 0, 1, 1}}, //UNICAST_AUDIO_CFG_7_II
    {1, .server_cfg[0] = {2, 1, 1, 2, 1, 0, 2, 3}, .server_cfg[1] = {0, 0, 0, 0, 0, 0, 0, 0}}, //UNICAST_AUDIO_CFG_8_I
    {2, .server_cfg[0] = {1, 0, 1, 1, 0, 0, 1, 1}, .server_cfg[1] = {1, 1, 1, 1, 1, 0, 1, 2}}, //UNICAST_AUDIO_CFG_8_II
    {1, .server_cfg[0] = {0, 2, 0, 0, 1, 2, 2, 2}, .server_cfg[1] = {0, 0, 0, 0, 0, 0, 0, 0}}, //UNICAST_AUDIO_CFG_9_I
    {2, .server_cfg[0] = {0, 1, 0, 0, 1, 1, 1, 1}, .server_cfg[1] = {0, 1, 0, 0, 1, 1, 1, 1}}, //UNICAST_AUDIO_CFG_9_II
    {1, .server_cfg[0] = {0, 1, 0, 0, 2, 2, 1, 1}, .server_cfg[1] = {0, 0, 0, 0, 0, 0, 0, 0}}, //UNICAST_AUDIO_CFG_10
    {1, .server_cfg[0] = {2, 2, 1, 2, 1, 2, 2, 4}, .server_cfg[1] = {0, 0, 0, 0, 0, 0, 0, 0}}, //UNICAST_AUDIO_CFG_11_I
    {2, .server_cfg[0] = {1, 1, 1, 1, 1, 1, 1, 2}, .server_cfg[1] = {1, 1, 1, 1, 1, 1, 1, 2}}  //UNICAST_AUDIO_CFG_11_II
};

bool bap_unicast_cp_config_qos(T_AUDIO_STREAM_SESSION *p_audio_session);
bool bap_unicast_cp_update_metadata(T_AUDIO_STREAM_SESSION *p_audio_session);
bool bap_unicast_audio_check_state(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                   T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                   T_AUDIO_STREAM_EVENT event, uint8_t *p_event_param);
void bap_unicast_stop_timer(T_AUDIO_STREAM_SESSION *p_audio_session);
T_AUDIO_STREAM_STATE bap_unicast_audio_get_new_state(T_AUDIO_STREAM_SESSION *p_audio_session,
                                                     T_BAP_UNICAST_RESULT result);
bool bap_unicast_get_presentation_delay(T_AUDIO_STREAM_SESSION *p_audio_session,
                                        T_AUDIO_DIRECTION direction,
                                        uint32_t *p_preferred_presentation_delay_min, uint32_t *p_preferred_presentation_delay_max,
                                        uint32_t *p_presentation_delay_min, uint32_t *p_presentation_delay_max);
bool bap_unicast_get_transport_latency(T_AUDIO_STREAM_SESSION *p_audio_session,
                                       uint16_t *p_sink_transport_latency_max,
                                       uint16_t *p_source_transport_latency_max);
#if LE_AUDIO_DEBUG
void bap_unicast_client_print(T_AUDIO_STREAM_SESSION_HANDLE handle)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    PROTOCOL_PRINT_INFO6("session: %p, group_handle 0x%x, state %d, prefer_state %d, cfg_type 0x%x, req_action %d",
                         p_audio_session,
                         p_audio_session->group_handle,
                         p_audio_session->state,
                         p_audio_session->prefer_state,
                         p_audio_session->cfg_type,
                         p_audio_session->req_action);
    PROTOCOL_PRINT_INFO2("session: cig_id %d, cig_state %d",
                         p_audio_session->cig_id,
                         p_audio_session->cig_state);
    PROTOCOL_PRINT_INFO7("session: cig param [0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x]",
                         p_audio_session->cig_param.sdu_interval_m_s,
                         p_audio_session->cig_param.sdu_interval_s_m,
                         p_audio_session->cig_param.sca,
                         p_audio_session->cig_param.packing,
                         p_audio_session->cig_param.framing,
                         p_audio_session->cig_param.latency_m_s,
                         p_audio_session->cig_param.latency_s_m);
    PROTOCOL_PRINT_INFO7("session: session_qos 1 [0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x]",
                         p_audio_session->session_qos.sca,
                         p_audio_session->session_qos.packing,
                         p_audio_session->session_qos.framing,
                         p_audio_session->session_qos.sdu_interval_m_s,
                         p_audio_session->session_qos.sdu_interval_s_m,
                         p_audio_session->session_qos.latency_m_s,
                         p_audio_session->session_qos.latency_s_m);
    PROTOCOL_PRINT_INFO2("session: session_qos 2 [0x%x 0x%x]",
                         p_audio_session->session_qos.sink_presentation_delay,
                         p_audio_session->session_qos.source_presentation_delay);

    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        T_AUDIO_STREAM_SERVER *p_srv = &p_audio_session->server[i];
        T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;
        PROTOCOL_PRINT_INFO4("session srv[%d]: dev_handle %p, cis_num %d, ase_num %d",
                             i,
                             p_srv->dev_handle,
                             p_srv->cis_num,
                             p_srv->ase_num);
        if (p_dev != NULL && p_dev->p_link != NULL)
        {
            PROTOCOL_PRINT_INFO2("dev: conn_id %d, conn_handle 0x%x", p_dev->p_link->conn_id,
                                 p_dev->p_link->conn_handle);
        }
        for (uint8_t k = 0; k < p_srv->cis_num; k++)
        {
            T_BAP_CIS_PARAM *p_cis_info = &p_srv->cis_info[k];
            PROTOCOL_PRINT_INFO6("cis_info[%d]: cis_id %d, sink_ase_idx %d, source_ase_idx %d, path_direction 0x%x, cis_conn_handle 0x%x",
                                 k,
                                 p_cis_info->cis_id,
                                 p_cis_info->sink_ase_idx,
                                 p_cis_info->source_ase_idx,
                                 p_cis_info->path_direction,
                                 p_cis_info->cis_conn_handle);
            PROTOCOL_PRINT_INFO6("cis_info: cis_param, max_sdu_m_s %d, max_sdu_s_m %d, phy_m_s 0x%x, phy_s_m 0x%x, rtn_m_s %d, rtn_s_m %d",
                                 p_cis_info->cis_param.max_sdu_m_s,
                                 p_cis_info->cis_param.max_sdu_s_m,
                                 p_cis_info->cis_param.phy_m_s,
                                 p_cis_info->cis_param.phy_s_m,
                                 p_cis_info->cis_param.rtn_m_s,
                                 p_cis_info->cis_param.rtn_s_m);
        }
        for (uint8_t j = 0; j < p_srv->ase_num; j++)
        {
            T_BAP_ASE_ENTRY *p_ase_entry = p_srv->ase_info[j].p_ase_entry;
            PROTOCOL_PRINT_INFO4("ase_info[%d]: cis_id %d, direction %d, channel_num %d",
                                 j,
                                 p_srv->ase_info[j].cis_id,
                                 p_srv->ase_info[j].direction,
                                 p_srv->ase_info[j].channel_num);

            PROTOCOL_PRINT_INFO8("ase_info: ase_codec_cfg, target_latency %d, target_phy %d, codec_id 0x%x, codec_cfg [0x%x 0x%x 0x%x 0x%x 0x%x]",
                                 p_srv->ase_info[j].ase_codec_cfg.target_latency,
                                 p_srv->ase_info[j].ase_codec_cfg.target_phy,
                                 p_srv->ase_info[j].ase_codec_cfg.codec_id[0],
                                 p_srv->ase_info[j].ase_codec_cfg.codec_cfg.frame_duration,
                                 p_srv->ase_info[j].ase_codec_cfg.codec_cfg.sample_frequency,
                                 p_srv->ase_info[j].ase_codec_cfg.codec_cfg.codec_frame_blocks_per_sdu,
                                 p_srv->ase_info[j].ase_codec_cfg.codec_cfg.octets_per_codec_frame,
                                 p_srv->ase_info[j].ase_codec_cfg.codec_cfg.audio_channel_allocation);
            if (p_ase_entry)
            {
                PROTOCOL_PRINT_INFO7("ase_info[%d]: ase_entry[used %d, ase_id %d, ase_state %d, handle 0x%x, pending_cp %d, wait_cp %d]",
                                     j,
                                     p_ase_entry->used,
                                     p_ase_entry->ase_id,
                                     p_ase_entry->ase_state,
                                     p_ase_entry->handle,
                                     p_ase_entry->pending_cp,
                                     p_ase_entry->wait_cp);
            }
            else
            {
                PROTOCOL_PRINT_INFO1("ase_info[%d]: p_ase_entry = NULL", j);
            }
        }
    }
}
#endif

T_BLE_AUDIO_DEV_HANDLE bap_unicast_get_dev_handle(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                                  uint16_t conn_handle)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    if (p_audio_session)
    {
        if (p_audio_session->group_handle)
        {
            return ble_audio_group_find_dev_by_conn_handle(p_audio_session->group_handle, conn_handle);
        }
    }
    return NULL;
}

bool bap_unicast_check_dev_state(T_LE_AUDIO_DEV *p_dev, bool check_ascs)
{
    T_GAP_CONN_INFO conn_info;
    conn_info.conn_state = GAP_CONN_STATE_DISCONNECTED;
    if (p_dev == NULL || p_dev->p_link == NULL)
    {
        PROTOCOL_PRINT_INFO1("bap_unicast_check_dev_state: failed, p_dev %p", p_dev);
        return false;
    }

    if (le_get_conn_info(p_dev->p_link->conn_id, &conn_info))
    {
        if (conn_info.conn_state == GAP_CONN_STATE_CONNECTED)
        {
            if (check_ascs)
            {
                T_BAP_LINK_CB *p_bap_cb;

                if (p_dev->p_link->p_bap_cb == NULL)
                {
                    PROTOCOL_PRINT_INFO1("bap_unicast_check_dev_state: conn_handle 0x%x, p_dev->p_link->p_bap_cb = NULL",
                                         p_dev->p_link->conn_handle);
                    return false;
                }
                p_bap_cb = p_dev->p_link->p_bap_cb;
                if (p_bap_cb->p_ascs_cb == NULL ||
                    ((p_bap_cb->disc_flags & BAP_DISC_ASCS) == 0))
                {
                    PROTOCOL_PRINT_INFO3("bap_unicast_check_dev_state: conn_handle 0x%x, p_ascs_cb %p, disc_flags 0x%x",
                                         p_dev->p_link->conn_handle, p_bap_cb->p_ascs_cb, p_bap_cb->disc_flags);
                    return false;
                }
            }

            return true;
        }
    }
    PROTOCOL_PRINT_INFO2("bap_unicast_check_dev_state: conn_handle 0x%x is not connect, conn_state %d",
                         p_dev->p_link->conn_handle, conn_info.conn_state);
    return false;
}

T_BAP_ASE_INFO *bap_unicast_get_ase_entry(T_AUDIO_STREAM_SESSION *p_audio_session,
                                          T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                          uint8_t ase_id)
{
    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        if (p_audio_session->server[i].dev_handle == dev_handle)
        {
            T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;
            if (p_dev == NULL || p_dev->p_link == NULL)
            {
                continue;
            }
            for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
            {
                T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
                if (p_ase_entry && p_ase_entry->ase_id == ase_id)
                {
                    return &p_audio_session->server[i].ase_info[j];
                }
            }
        }
    }
    return NULL;
}

uint8_t bap_unicast_get_idle_ase_num(T_BAP_ASCS_CB *p_ascs_cb, uint8_t direction)
{
    uint8_t i;
    uint8_t num = 0;
    if (direction == SERVER_AUDIO_SOURCE)
    {
        for (i = 0; i < p_ascs_cb->src_ase_num; i++)
        {
            uint8_t idx = i + p_ascs_cb->snk_ase_num;
            if (p_ascs_cb->ase_data[idx].used == false &&
                (p_ascs_cb->ase_data[idx].ase_state == ASE_STATE_IDLE ||
                 p_ascs_cb->ase_data[idx].ase_state == ASE_STATE_CODEC_CONFIGURED))
            {
                num++;
            }
        }
    }
    else if (direction == SERVER_AUDIO_SINK)
    {
        for (i = 0; i < p_ascs_cb->snk_ase_num; i++)
        {
            if (p_ascs_cb->ase_data[i].used == false &&
                (p_ascs_cb->ase_data[i].ase_state == ASE_STATE_IDLE ||
                 p_ascs_cb->ase_data[i].ase_state == ASE_STATE_CODEC_CONFIGURED))
            {
                num++;
            }
        }
    }
    return num;
}

bool bap_unicast_audio_get_cis_info(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                    T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                    T_AUDIO_STREAM_CIS_INFO *p_info)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    uint8_t err_idx = 0;
    T_AUDIO_CIS_INFO *p_cis_info;
    if (p_info == NULL)
    {
        err_idx = 1;
        goto failed;
    }
    if (audio_stream_session_check(handle) == false)
    {
        err_idx = 2;
        goto failed;
    }
    if (p_audio_session->state == AUDIO_STREAM_STATE_IDLE)
    {
        err_idx = 3;
        goto failed;
    }
    if (ble_audio_dev_handle_check(p_audio_session->group_handle, dev_handle) == false)
    {
        err_idx = 4;
        goto failed;
    }
    memset(p_info, 0, sizeof(T_AUDIO_STREAM_CIS_INFO));
    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        if (p_audio_session->server[i].dev_handle == dev_handle)
        {
            T_ISOCH_INFO isoch_info;
            T_AUDIO_STREAM_SERVER *p_srv = &p_audio_session->server[i];
            T_BAP_ASE_ENTRY *p_ase_entry;
            for (uint8_t j = 0; j < p_srv->cis_num; j++)
            {
                p_cis_info = &p_info->cis_info[p_info->cis_num];
                if (p_srv->cis_info[j].cis_id == 0)
                {
                    err_idx = 5;
                    goto failed;
                }
                p_cis_info->cis_id = p_srv->cis_info[j].cis_id;
                p_cis_info->cis_conn_handle = p_srv->cis_info[j].cis_conn_handle;

                if (cig_mgr_get_isoch_info(p_cis_info->cis_conn_handle, &isoch_info) == false)
                {
                    err_idx = 6;
                    goto failed;
                }
                p_cis_info->data_path_flags = isoch_info.data_path_flags;
                if (p_srv->cis_info[j].path_direction & DATA_PATH_INPUT_FLAG)
                {
                    if (p_srv->cis_info[j].sink_ase_idx < p_srv->ase_num)
                    {
                        p_ase_entry = p_srv->ase_info[p_srv->cis_info[j].sink_ase_idx].p_ase_entry;
                        if (p_ase_entry)
                        {
                            p_cis_info->sink_ase_id = p_ase_entry->ase_id;
                            p_cis_info->sink_ase_state = p_ase_entry->ase_state;
                        }
                        else
                        {
                            p_cis_info->sink_ase_id = 0;
                            p_cis_info->sink_ase_state = ASE_STATE_UNKOWN;
                        }
                    }
                }
                if (p_srv->cis_info[j].path_direction & DATA_PATH_OUTPUT_FLAG)
                {
                    if (p_srv->cis_info[j].source_ase_idx < p_srv->ase_num)
                    {
                        p_ase_entry = p_srv->ase_info[p_srv->cis_info[j].source_ase_idx].p_ase_entry;
                        if (p_ase_entry)
                        {
                            p_cis_info->source_ase_id = p_ase_entry->ase_id;
                            p_cis_info->source_ase_state = p_ase_entry->ase_state;
                        }
                        else
                        {
                            p_cis_info->source_ase_id = 0;
                            p_cis_info->source_ase_state = ASE_STATE_UNKOWN;
                        }
                    }
                }
                p_info->cis_num++;
            }
        }
    }

    return true;
failed:
    PROTOCOL_PRINT_ERROR1("bap_unicast_audio_get_cis_info: failed, err_idx %d", err_idx);
    return false;
}

bool bap_unicast_audio_get_session_info(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                        T_BAP_UNICAST_SESSION_INFO *p_info)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    uint8_t err_idx = 0;
    if (p_info == NULL)
    {
        err_idx = 1;
        goto failed;
    }
    if (audio_stream_session_check(handle) == false)
    {
        err_idx = 2;
        goto failed;
    }
    if (p_audio_session->server_num > BAP_UNICAST_SERVERS_MAX_NUM)
    {
        err_idx = 3;
        goto failed;
    }
    memset(p_info, 0, sizeof(T_BAP_UNICAST_SESSION_INFO));
    p_info->state = p_audio_session->state;
    p_info->cfg_type = p_audio_session->cfg_type;
    p_info->dev_num = p_audio_session->server_num;
    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;
        if (p_dev == NULL || p_dev->p_link == NULL)
        {
            p_info->dev_info[i].conn_state = GAP_CONN_STATE_DISCONNECTED;
        }
        else
        {
            p_info->dev_info[i].conn_state = p_dev->p_link->state;
            p_info->conn_dev_num++;
        }
        p_info->dev_info[i].dev_handle = p_audio_session->server[i].dev_handle;
        p_info->dev_info[i].ase_num = p_audio_session->server[i].ase_num;
        for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
        {
            T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
            if (p_ase_entry != NULL)
            {
                p_info->dev_info[i].ase_info[j].ase_id = p_ase_entry->ase_id;
                p_info->dev_info[i].ase_info[j].ase_state = p_ase_entry->ase_state;
            }
            else
            {
                p_info->dev_info[i].ase_info[j].ase_id = 0;
                p_info->dev_info[i].ase_info[j].ase_state = ASE_STATE_UNKOWN;
            }
            p_info->dev_info[i].ase_info[j].cfg_idx = j;
            p_info->dev_info[i].ase_info[j].direction = p_audio_session->server[i].ase_info[j].direction;
            p_info->dev_info[i].ase_info[j].channel_num = p_audio_session->server[i].ase_info[j].channel_num;
        }
    }
    return true;
failed:
    PROTOCOL_PRINT_ERROR1("bap_unicast_audio_get_session_info: failed, err_idx %d", err_idx);
    return false;
}

void bap_unicast_send_bap_info(T_AUDIO_STREAM_SESSION *p_audio_session, T_AUDIO_STREAM_STATE  state,
                               T_BAP_UNICAST_RESULT result, uint16_t cause, T_AUDIO_GROUP_ADDL_INFO *p_addl_info)
{
    T_AUDIO_GROUP_BAP_STATE info = {0};
    if (p_audio_session->state != state)
    {
        T_AUDIO_STREAM_STATE old_state = p_audio_session->state;
        p_audio_session->state = state;
        info.handle = p_audio_session;
        info.state = state;
        info.curr_action = p_audio_session->req_action;
        info.result = result;
        info.cause = cause;
        if (p_addl_info)
        {
            memcpy(&info.addl_info, p_addl_info, sizeof(T_AUDIO_GROUP_ADDL_INFO));
        }
        if (p_audio_session->state != AUDIO_STREAM_STATE_STARTING &&
            p_audio_session->state != AUDIO_STREAM_STATE_STOPPING &&
            p_audio_session->state != AUDIO_STREAM_STATE_RELEASING)
        {
            if (p_audio_session->state == AUDIO_STREAM_STATE_CONFIGURED &&
                p_audio_session->req_action == BAP_UNICAST_ACTION_SERVER_RELEASE)
            {

            }
            else
            {
                p_audio_session->req_action = BAP_UNICAST_ACTION_IDLE;
            }

        }
        PROTOCOL_PRINT_INFO7("bap_unicast_send_bap_info: session handle %p, curr_action %d, req_action %d, (state %d -> %d), result %d, cause 0x%x",
                             p_audio_session, info.curr_action, p_audio_session->req_action,
                             old_state, info.state, info.result, info.cause);
        ble_audio_group_send_msg(AUDIO_GROUP_MSG_BAP_STATE, p_audio_session->group_handle,
                                 (uint8_t *)&info, sizeof(info), false);
        if (p_audio_session->state == AUDIO_STREAM_STATE_STREAMING ||
            p_audio_session->state == AUDIO_STREAM_STATE_PARTIAL_STREAMING)
        {
            if (p_audio_session->retry_update_metadata)
            {
                bap_unicast_cp_update_metadata(p_audio_session);
                p_audio_session->retry_update_metadata = false;
            }
        }
    }
}

bool bap_unicast_send_session_remove(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    T_AUDIO_GROUP_BAP_SESSION_REMOVE info;
    T_BLE_AUDIO_GROUP_HANDLE group_handle = p_audio_session->group_handle;
    info.handle = p_audio_session;
    PROTOCOL_PRINT_INFO1("bap_unicast_send_session_remove: session handle %p",
                         p_audio_session);
    if (audio_stream_session_release(p_audio_session) == false)
    {
        return false;
    }
    ble_audio_group_send_msg(AUDIO_GROUP_MSG_BAP_SESSION_REMOVE, group_handle,
                             (uint8_t *)&info, sizeof(info), false);
    return true;
}

void bap_unicast_send_start_qos_cfg(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    T_AUDIO_GROUP_BAP_START_QOS_CFG info = {0};

    info.handle = p_audio_session;
    bap_unicast_get_transport_latency(p_audio_session,
                                      &info.sink_transport_latency_max, &info.source_transport_latency_max);
    bap_unicast_get_presentation_delay(p_audio_session, SERVER_AUDIO_SINK,
                                       &info.sink_preferred_presentation_delay_min, &info.sink_preferred_presentation_delay_max,
                                       &info.sink_presentation_delay_min, &info.sink_presentation_delay_max);
    bap_unicast_get_presentation_delay(p_audio_session, SERVER_AUDIO_SOURCE,
                                       &info.source_preferred_presentation_delay_min, &info.source_preferred_presentation_delay_max,
                                       &info.source_presentation_delay_min, &info.source_presentation_delay_max);
    ble_audio_group_send_msg(AUDIO_GROUP_MSG_BAP_START_QOS_CFG, p_audio_session->group_handle,
                             (uint8_t *)&info, sizeof(info), true);
}

void bap_unicast_send_start_metadata_cfg(T_AUDIO_STREAM_SESSION *p_audio_session,
                                         T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                         uint8_t ase_id, uint8_t direction)
{
    T_AUDIO_GROUP_BAP_START_METADATA_CFG info = {0};

    info.handle = p_audio_session;
    info.dev_handle = dev_handle;
    info.ase_id = ase_id;
    info.direction = direction;
    ble_audio_group_send_msg(AUDIO_GROUP_MSG_BAP_START_METADATA_CFG, p_audio_session->group_handle,
                             (uint8_t *)&info, sizeof(info), true);
}

void bap_unicast_audio_cig_remove(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    p_audio_session->cig_state = T_CIG_SETTING_STATE_IDLE;
    memset(&p_audio_session->cig_param, 0, sizeof(T_CIG_CFG_PARAM));
    memset(&p_audio_session->session_qos, 0, sizeof(T_AUDIO_SESSION_QOS_CFG));
    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        memset(p_audio_session->server[i].cis_info, 0, AUDIO_STREAM_SRV_CIS_MAX * sizeof(T_BAP_CIS_PARAM));
        for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
        {
            T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
            if (p_ase_entry)
            {
                p_ase_entry->used = false;
                p_ase_entry->handle = NULL;
                p_audio_session->server[i].ase_info[j].p_ase_entry = NULL;
            }
            if (p_audio_session->server[i].ase_info[j].p_metadata)
            {
                ble_audio_mem_free(p_audio_session->server[i].ase_info[j].p_metadata);
                p_audio_session->server[i].ase_info[j].p_metadata = NULL;
                p_audio_session->server[i].ase_info[j].metadata_len = 0;
            }
            p_audio_session->server[i].ase_info[j].cis_id = 0;
        }
    }
}

void bap_unicast_audio_cfg_remove(T_AUDIO_STREAM_SESSION_HANDLE handle)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    p_audio_session->cfg_type = UNICAST_AUDIO_CFG_UNKNOWN;
    if (p_audio_session->server)
    {
        for (uint8_t i = 0; i < p_audio_session->server_num; i++)
        {
            for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
            {
                T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
                if (p_ase_entry)
                {
                    p_ase_entry->used = false;
                    p_ase_entry->handle = NULL;
                }
                if (p_audio_session->server[i].ase_info[j].p_metadata)
                {
                    ble_audio_mem_free(p_audio_session->server[i].ase_info[j].p_metadata);
                }
            }
            if (p_audio_session->server[i].ase_info)
            {
                ble_audio_mem_free(p_audio_session->server[i].ase_info);
            }
        }
        p_audio_session->server_num = 0;
        ble_audio_mem_free(p_audio_session->server);
        p_audio_session->server = NULL;
    }
    p_audio_session->cig_id = 0;
    p_audio_session->prefer_state = ASE_STATE_IDLE;
    p_audio_session->retry_num = 0;
    memset(&p_audio_session->cig_param, 0, sizeof(T_CIG_CFG_PARAM));
    if (p_audio_session->p_session_timer_handle)
    {
        sys_timer_delete(p_audio_session->p_session_timer_handle);
        p_audio_session->p_session_timer_handle = NULL;
    }
    bap_unicast_send_bap_info(p_audio_session, AUDIO_STREAM_STATE_IDLE,
                              BAP_UNICAST_RESULT_SUCCESS, GAP_SUCCESS, NULL);
}

void bap_unicast_handle_ase_data_notify(T_BAP_LINK_CB *p_bap_cb, uint8_t direction,
                                        uint8_t instance_id,
                                        T_ASE_CHAR_DATA *p_ase_data)
{
    bool metadata_update = false;
    uint8_t ase_idx = instance_id;
    T_BAP_ASE_ENTRY *p_ase_entry;
    T_AUDIO_STREAM_SESSION *p_audio_session = NULL;
    if (direction == SERVER_AUDIO_SOURCE)
    {
        ase_idx += p_bap_cb->p_ascs_cb->snk_ase_num;
    }
    p_ase_entry = &p_bap_cb->p_ascs_cb->ase_data[ase_idx];
    PROTOCOL_PRINT_TRACE3("bap_unicast_handle_ase_data_notify: ase_state 0x%x, pending_cp 0x%x, ase_idx %d",
                          p_ase_data->ase_state, p_ase_entry->pending_cp, ase_idx);
    if (p_ase_data->ase_state == ASE_STATE_CODEC_CONFIGURED &&
        p_ase_entry->pending_cp == ASE_CP_OP_CONFIG_CODEC)
    {
        p_ase_entry->pending_cp = ASE_CP_OP_NONE;
    }
    if (p_ase_data->ase_state == ASE_STATE_QOS_CONFIGURED &&
        p_ase_entry->pending_cp == ASE_CP_OP_CONFIG_QOS)
    {
        p_ase_entry->pending_cp = ASE_CP_OP_NONE;
    }
    if (p_ase_data->ase_state == ASE_STATE_ENABLING &&
        p_ase_entry->pending_cp == ASE_CP_OP_ENABLE)
    {
        p_ase_entry->pending_cp = ASE_CP_OP_NONE;
    }
    if ((p_ase_data->ase_state == ASE_STATE_ENABLING ||
         p_ase_data->ase_state == ASE_STATE_STREAMING) &&
        p_ase_entry->pending_cp == ASE_CP_OP_UPDATE_METADATA)
    {
        metadata_update = true;
        p_ase_entry->pending_cp = ASE_CP_OP_NONE;
    }

    if (audio_stream_session_check(p_ase_entry->handle))
    {
        p_audio_session = (T_AUDIO_STREAM_SESSION *)p_ase_entry->handle;
    }
    else
    {
        PROTOCOL_PRINT_ERROR1("bap_unicast_handle_ase_data_notify: ase_state 0x%x unkown state check to release",
                              p_ase_data->ase_state);
        if (p_ase_data->ase_state != ASE_STATE_IDLE &&
            p_ase_data->ase_state != ASE_STATE_CODEC_CONFIGURED &&
            p_ase_data->ase_state != ASE_STATE_RELEASING)
        {
            ase_cp_release(p_bap_cb->conn_handle, 1, &p_ase_data->ase_id);
        }
        p_ase_entry->ase_state = (T_ASE_STATE)p_ase_data->ase_state;
        return;
    }
    if (p_ase_entry->ase_state != p_ase_data->ase_state)
    {
        if (p_audio_session->req_action == BAP_UNICAST_ACTION_IDLE ||
            p_audio_session->req_action == BAP_UNICAST_ACTION_START ||
            p_audio_session->req_action == BAP_UNICAST_ACTION_UPDATE)
        {
            if (p_ase_data->ase_state == ASE_STATE_RELEASING ||
                (p_ase_entry->ase_state  == ASE_STATE_RELEASING
                 && (p_ase_data->ase_state == ASE_STATE_IDLE || p_ase_data->ase_state == ASE_STATE_CODEC_CONFIGURED))
               )
            {
                p_audio_session->req_action = BAP_UNICAST_ACTION_SERVER_RELEASE;
            }
            if (p_ase_data->ase_state == ASE_STATE_DISABLING ||
                (p_ase_data->ase_state == ASE_STATE_QOS_CONFIGURED &&
                 (p_ase_entry->ase_state == ASE_STATE_ENABLING || p_ase_entry->ase_state == ASE_STATE_STREAMING)))
            {
                p_audio_session->req_action = BAP_UNICAST_ACTION_SERVER_STOP;
            }
        }
        p_ase_entry->ase_state = (T_ASE_STATE)p_ase_data->ase_state;
        bap_unicast_audio_check_state(p_ase_entry->handle,
                                      bap_unicast_get_dev_handle(p_ase_entry->handle, p_bap_cb->conn_handle),
                                      AUDIO_STREAM_EVENT_ASE_STATE_CHANGE, NULL);
    }
    else
    {
        if (p_ase_entry->ase_state == ASE_STATE_ENABLING || p_ase_entry->ase_state == ASE_STATE_STREAMING
            || p_ase_entry->ase_state == ASE_STATE_CODEC_CONFIGURED)
        {
            if (p_ase_entry->ase_state == ASE_STATE_ENABLING || p_ase_entry->ase_state == ASE_STATE_STREAMING)
            {
                T_AUDIO_GROUP_MSG_BAP_METADATA_UPDATE info;
                info.handle = p_ase_entry->handle;
                info.dev_handle = bap_unicast_get_dev_handle(p_ase_entry->handle, p_bap_cb->conn_handle);
                info.ase_id = p_ase_data->ase_id;
                info.direction = p_ase_entry->direction;
                info.metadata_length = p_ase_data->param.enabling.metadata_length;
                info.p_metadata = p_ase_data->param.enabling.p_metadata;
                ble_audio_group_send_msg(AUDIO_GROUP_MSG_BAP_METADATA_UPDATE, p_audio_session->group_handle,
                                         (uint8_t *)&info, sizeof(info), false);
                if (metadata_update)
                {
                    bap_unicast_cp_update_metadata(p_audio_session);
                }
            }
            if (p_ase_entry->ase_state == ASE_STATE_CODEC_CONFIGURED)
            {
                bap_unicast_audio_check_state(p_ase_entry->handle,
                                              bap_unicast_get_dev_handle(p_ase_entry->handle, p_bap_cb->conn_handle),
                                              AUDIO_STREAM_EVENT_ASE_STATE_CHANGE, NULL);
            }
        }
    }
}

void bap_unicast_handle_ase_data_read(T_BAP_LINK_CB *p_bap_cb, uint8_t direction,
                                      uint8_t instance_id,
                                      T_ASE_CHAR_DATA *p_ase_data)
{
    uint8_t ase_idx = instance_id;
    T_BAP_ASE_ENTRY *p_ase_entry;
    T_AUDIO_STREAM_SESSION *p_audio_session = NULL;
    if (direction == SERVER_AUDIO_SOURCE)
    {
        ase_idx += p_bap_cb->p_ascs_cb->snk_ase_num;
    }
    p_ase_entry = &p_bap_cb->p_ascs_cb->ase_data[ase_idx];
    p_ase_entry->wait_cp = false;

    if (p_ase_entry->ase_state != p_ase_data->ase_state)
    {
        p_ase_entry->ase_state = (T_ASE_STATE)p_ase_data->ase_state;
    }
    if (audio_stream_session_check(p_ase_entry->handle))
    {
        p_audio_session = (T_AUDIO_STREAM_SESSION *)p_ase_entry->handle;
        for (uint8_t i = 0; i < p_audio_session->server_num; i++)
        {
            T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;
            if (p_dev == NULL || p_dev->p_link == NULL)
            {
                continue;
            }
            for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
            {
                T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
                if (p_ase_entry && p_ase_entry->wait_cp)
                {
                    return;
                }
            }
        }
    }
    else
    {
        return;
    }
    bap_unicast_audio_check_state(p_ase_entry->handle,
                                  NULL, AUDIO_STREAM_EVENT_READ_ASE_STATE, NULL);
}

void bap_unicast_client_check_cp_error(T_AUDIO_STREAM_SESSION *p_audio_session,
                                       T_ASCS_CLIENT_CP_NOTIFY *p_cp_notify)
{
    bool check_cp_error = false;
    if (p_cp_notify != NULL)
    {
        if (p_audio_session->state == AUDIO_STREAM_STATE_STARTING)
        {
            if (p_cp_notify->opcode == ASE_CP_OP_RELEASE || p_cp_notify->opcode == ASE_CP_OP_DISABLE ||
                p_cp_notify->opcode == ASE_CP_OP_REC_STOP_READY)
            {
                return;
            }
            else
            {
                check_cp_error = true;
            }
        }
        else if (p_audio_session->state == AUDIO_STREAM_STATE_STOPPING)
        {
            if (p_cp_notify->opcode == ASE_CP_OP_DISABLE ||
                p_cp_notify->opcode == ASE_CP_OP_REC_STOP_READY)
            {
                check_cp_error = true;
            }
        }
        else if (p_audio_session->state == AUDIO_STREAM_STATE_RELEASING)
        {
            if (p_cp_notify->opcode == ASE_CP_OP_RELEASE)
            {
                check_cp_error = true;
            }
        }
    }
    if (check_cp_error)
    {
        bap_unicast_audio_check_state(p_audio_session, NULL, AUDIO_STREAM_EVENT_ASE_CP_ERROR,
                                      (uint8_t *)p_cp_notify);
    }
}
uint16_t bap_unicast_client_handle_ascs_msg(uint16_t msg, void *buf, bool *p_handled)
{
    uint16_t cb_result = BLE_AUDIO_CB_RESULT_SUCCESS;
    T_BAP_LINK_CB *p_bap_cb = NULL;
    *p_handled = true;

    switch (msg)
    {
    case LE_AUDIO_MSG_ASCS_CLIENT_DIS_DONE:
        {
            T_ASCS_CLIENT_DIS_DONE *p_dis_done = (T_ASCS_CLIENT_DIS_DONE *)buf;
            if (p_dis_done == NULL)
            {
                break;
            }
            p_bap_cb =  bap_check_link(p_dis_done->conn_handle);
            if (p_bap_cb == NULL)
            {
                break;
            }
            if (p_dis_done->is_found)
            {
                if (bap_db.role_msk & (BAP_UNICAST_CLT_SRC_ROLE | BAP_UNICAST_CLT_SNK_ROLE))
                {
                    p_bap_cb->p_ascs_cb = ble_audio_mem_zalloc(sizeof(T_BAP_ASCS_CB) +
                                                               (p_dis_done->snk_ase_num + p_dis_done->src_ase_num) * sizeof(T_BAP_ASE_ENTRY));
                    if (p_bap_cb->p_ascs_cb == NULL)
                    {
                        PROTOCOL_PRINT_ERROR0("bap_unicast_client_handle_ascs_msg: alloc ascs cb fail");
                        break;
                    }
                    ascs_client_read_ase_all(p_dis_done->conn_handle);
                    ascs_client_enable_cccd(p_dis_done->conn_handle);
                    p_bap_cb->p_ascs_cb->snk_ase_num = p_dis_done->snk_ase_num;
                    p_bap_cb->p_ascs_cb->src_ase_num = p_dis_done->src_ase_num;
                    break;
                }
            }
            p_bap_cb->disc_flags |= BAP_DISC_ASCS;
            bap_send_discovery_result(p_bap_cb, GAP_SUCCESS);
        }
        break;
    case LE_AUDIO_MSG_ASCS_CLIENT_READ_ASE_DATA:
        {
            T_ASCS_CLIENT_ASE_DATA *p_ase_data = (T_ASCS_CLIENT_ASE_DATA *)buf;
            if (p_ase_data && p_ase_data->cause == GAP_SUCCESS)
            {
                p_bap_cb = bap_check_link(p_ase_data->conn_handle);
                if (p_bap_cb && p_bap_cb->p_ascs_cb)
                {
                    bap_unicast_handle_ase_data_read(p_bap_cb, p_ase_data->direction, p_ase_data->instance_id,
                                                     &p_ase_data->ase_data);
                }
            }
        }
        break;
    case LE_AUDIO_MSG_ASCS_CLIENT_ASE_DATA_NOTIFY:
        {
            T_ASCS_CLIENT_ASE_DATA *p_ase_data = (T_ASCS_CLIENT_ASE_DATA *)buf;
            if (p_ase_data)
            {
                p_bap_cb = bap_check_link(p_ase_data->conn_handle);
                if (p_bap_cb && p_bap_cb->p_ascs_cb)
                {
                    bap_unicast_handle_ase_data_notify(p_bap_cb, p_ase_data->direction, p_ase_data->instance_id,
                                                       &p_ase_data->ase_data);
                }

            }
        }
        break;
    case LE_AUDIO_MSG_ASCS_CLIENT_READ_ASE_ALL:
        {
            T_ASCS_CLIENT_READ_ASE_ALL *p_read_all = (T_ASCS_CLIENT_READ_ASE_ALL *)buf;
            if (p_read_all)
            {
                p_bap_cb =  bap_check_link(p_read_all->conn_handle);
                if (p_bap_cb == NULL || p_bap_cb->p_ascs_cb == NULL)
                {
                    break;
                }
                if (p_read_all->cause != GAP_SUCCESS)
                {
                    PROTOCOL_PRINT_ERROR0("bap_unicast_client_handle_ascs_msg: read all ase fail");
                    if (p_bap_cb->p_ascs_cb)
                    {
                        ble_audio_mem_free(p_bap_cb->p_ascs_cb);
                        p_bap_cb->p_ascs_cb = NULL;
                    }
                }
                else
                {
                    for (int i = 0; i < (p_bap_cb->p_ascs_cb->snk_ase_num + p_bap_cb->p_ascs_cb->src_ase_num); i++)
                    {
                        T_ASE_CHAR_DATA ase_data;
                        uint8_t direction;
                        uint8_t instance_id;
                        if (i < p_bap_cb->p_ascs_cb->snk_ase_num)
                        {
                            direction = SERVER_AUDIO_SINK;
                        }
                        else
                        {
                            direction = SERVER_AUDIO_SOURCE;
                        }
                        instance_id = (direction == SERVER_AUDIO_SINK ? i : (i - p_bap_cb->p_ascs_cb->snk_ase_num));
                        if (ascs_client_get_ase_data(p_bap_cb->conn_handle,
                                                     instance_id,
                                                     &ase_data, direction))
                        {
                            p_bap_cb->p_ascs_cb->ase_data[i].ase_id = ase_data.ase_id;
                            p_bap_cb->p_ascs_cb->ase_data[i].instance_id = instance_id;
                            p_bap_cb->p_ascs_cb->ase_data[i].direction = ase_data.direction;
                            p_bap_cb->p_ascs_cb->ase_data[i].ase_state = (T_ASE_STATE)ase_data.ase_state;
                        }
                    }
                }
                p_bap_cb->disc_flags |= BAP_DISC_ASCS;
                bap_send_discovery_result(p_bap_cb, p_read_all->cause);
            }
        }
        break;

    case LE_AUDIO_MSG_ASCS_CLIENT_CP_NOTIFY:
        {
            T_ASCS_CLIENT_CP_NOTIFY *p_cp_notify = (T_ASCS_CLIENT_CP_NOTIFY *)buf;
            T_BAP_ASE_ENTRY *p_entry;
            T_AUDIO_STREAM_SESSION_HANDLE session_handle = NULL;
            uint8_t response_code = ASE_CP_RESP_SUCCESS;
            if (p_cp_notify != NULL)
            {
                for (int i = 0; i < p_cp_notify->number_of_ase; i++)
                {
                    p_entry = bap_find_ase_by_ase_id(p_cp_notify->conn_handle, p_cp_notify->p_param[i].ase_id);
                    if (p_entry != NULL)
                    {
                        if (p_cp_notify->p_param[i].response_code != ASE_CP_RESP_SUCCESS)
                        {
                            response_code = p_cp_notify->p_param[i].response_code;
                            PROTOCOL_PRINT_ERROR2("bap_unicast_client_handle_ascs_msg: conn_handle 0x%x, ase_id %d",
                                                  p_cp_notify->conn_handle, p_cp_notify->p_param[i].ase_id);
                        }
                        p_entry->wait_cp = false;
                        if (p_entry->handle)
                        {
                            session_handle = p_entry->handle;
                        }
                    }
                }
            }
            if (audio_stream_session_check(session_handle))
            {
                T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)session_handle;

                if (response_code != ASE_CP_RESP_SUCCESS)
                {
                    bap_unicast_client_check_cp_error(p_audio_session, p_cp_notify);
                }
                for (uint8_t i = 0; i < p_audio_session->server_num; i++)
                {
                    for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
                    {
                        T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
                        if (p_ase_entry != NULL && p_ase_entry->wait_cp)
                        {
                            return cb_result;
                        }
                    }
                }
                bap_unicast_stop_timer(p_audio_session);

            }
        }
        break;

    case LE_AUDIO_MSG_ASCS_CLIENT_SETUP_DATA_PATH:
        {
            T_ASCS_CLIENT_SETUP_DATA_PATH *p_data = (T_ASCS_CLIENT_SETUP_DATA_PATH *)buf;
            T_BAP_ASE_ENTRY *p_entry;
            p_entry = bap_find_ase_by_ase_id(p_data->conn_handle, p_data->ase_id);
            if (p_entry && p_entry->handle)
            {
                if (audio_stream_session_check(p_entry->handle))
                {
                    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)p_entry->handle;
                    T_BLE_AUDIO_DEV_HANDLE dev_handle = ble_audio_group_find_dev_by_conn_handle(
                                                            p_audio_session->group_handle,
                                                            p_data->conn_handle);
                    if (dev_handle)
                    {
                        T_AUDIO_GROUP_MSG_BAP_SETUP_DATA_PATH info;
                        info.handle = p_entry->handle;
                        info.dev_handle = dev_handle;
                        info.ase_id = p_data->ase_id;
                        info.path_direction = p_data->path_direction;
                        info.cis_conn_handle = p_data->cis_conn_handle;
                        info.codec_cfg_len = p_data->codec_cfg_len;
                        info.p_codec_cfg = p_data->p_codec_cfg;
                        memcpy(&info.codec_parsed_data, &p_data->codec_parsed_data, sizeof(T_CODEC_CFG));
                        ble_audio_group_send_msg(AUDIO_GROUP_MSG_BAP_SETUP_DATA_PATH, p_audio_session->group_handle,
                                                 (uint8_t *)&info, sizeof(info), true);
                    }
                }
            }
        }
        break;

    case LE_AUDIO_MSG_ASCS_CLIENT_REMOVE_DATA_PATH:
        {
            T_ASCS_CLIENT_REMOVE_DATA_PATH *p_data = (T_ASCS_CLIENT_REMOVE_DATA_PATH *)buf;
            T_BAP_ASE_ENTRY *p_entry;
            p_entry = bap_find_ase_by_ase_id(p_data->conn_handle, p_data->ase_id);
            if (p_entry && p_entry->handle)
            {
                if (audio_stream_session_check(p_entry->handle))
                {
                    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)p_entry->handle;
                    T_BLE_AUDIO_DEV_HANDLE dev_handle = ble_audio_group_find_dev_by_conn_handle(
                                                            p_audio_session->group_handle,
                                                            p_data->conn_handle);
                    if (dev_handle)
                    {
                        T_AUDIO_GROUP_MSG_BAP_REMOVE_DATA_PATH info;
                        info.handle = p_entry->handle;
                        info.dev_handle = dev_handle;
                        info.ase_id = p_data->ase_id;
                        info.path_direction = p_data->path_direction;
                        info.cis_conn_handle = p_data->cis_conn_handle;
                        info.cause = p_data->cause;
                        ble_audio_group_send_msg(AUDIO_GROUP_MSG_BAP_REMOVE_DATA_PATH, p_audio_session->group_handle,
                                                 (uint8_t *)&info, sizeof(info), true);
                    }
                }
            }
        }
        break;
    default:
        break;
    }

    return cb_result;
}

T_APP_RESULT bap_unicast_client_cig_cb(uint8_t cig_id, uint8_t cb_type, void *p_cb_data)
{
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_CIG_MGR_CB_DATA *p_data = (T_CIG_MGR_CB_DATA *)p_cb_data;
    T_AUDIO_STREAM_SESSION_HANDLE handle;
    PROTOCOL_PRINT_INFO2("bap_unicast_client_cig_cb: cig_id %d, cb_type 0x%x", cig_id, cb_type);

    result = ascs_client_cig_cb(cig_id, cb_type, p_cb_data);

    handle = audio_stream_session_find_by_cig_id(cig_id);

    if (audio_stream_session_check(handle) == false)
    {
        PROTOCOL_PRINT_ERROR0("bap_unicast_client_cig_cb: stream session check fail");
        return result;
    }

    switch (cb_type)
    {
    case MSG_CIG_MGR_START_SETTING:
        {
            T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;

            if (p_data->p_cig_mgr_start_setting_rsp->cause == GAP_SUCCESS)
            {
                p_audio_session->cig_state = T_CIG_SETTING_STATE_CONFIGURED;
                for (uint8_t i = 0; i < p_audio_session->server_num; i++)
                {
                    T_AUDIO_STREAM_SERVER *p_srv = &p_audio_session->server[i];
                    for (uint8_t j = 0; j < p_srv->cis_num; j++)
                    {
                        if (p_srv->cis_info[j].cis_id != 0)
                        {
                            cig_mgr_get_cis_handle(p_srv->cis_info[j].cis_id, &p_srv->cis_info[j].cis_conn_handle);
                        }
                    }
                }
                if (p_audio_session->state == AUDIO_STREAM_STATE_STARTING)
                {
                    bap_unicast_cp_config_qos(p_audio_session);
                }
            }
            else
            {
                p_audio_session->cig_state = T_CIG_SETTING_STATE_IDLE;
                memset(&p_audio_session->cig_param, 0, sizeof(T_CIG_CFG_PARAM));
                for (uint8_t i = 0; i < p_audio_session->server_num; i++)
                {
                    memset(p_audio_session->server[i].cis_info, 0, AUDIO_STREAM_SRV_CIS_MAX * sizeof(T_BAP_CIS_PARAM));
                }
                bap_unicast_audio_check_state(handle, NULL,
                                              AUDIO_STREAM_EVENT_CIG_SET_ERROR, (uint8_t *)p_data->p_cig_mgr_start_setting_rsp);
            }
        }
        break;

    case MSG_CIG_MGR_REMOVE_CIG:
        {
            if (p_data->p_cig_mgr_remove_cig_rsp->cause == GAP_SUCCESS)
            {
                T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
                bap_unicast_audio_cig_remove(p_audio_session);
                bap_unicast_audio_check_state(handle, NULL,
                                              AUDIO_STREAM_EVENT_CIG_REMOVE, NULL);
            }
        }
        break;

    case MSG_CIG_MGR_CREATE_CIS:
        {
            if (p_data->p_cig_mgr_create_cis_rsp->cause != GAP_SUCCESS)
            {
                for (uint8_t i = 0; i < p_data->p_cig_mgr_create_cis_rsp->cis_count; i++)
                {
                    T_AUDIO_GROUP_BAP_CIS_DISCONN info = {0};
                    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
                    uint8_t conn_id;

                    info.handle = handle;
                    info.cause = p_data->p_cig_mgr_create_cis_rsp->cause;
                    info.cis_conn_handle = p_data->p_cig_mgr_create_cis_rsp->cis_info[i].cis_conn_handle;
                    if (cig_mgr_get_conn_id(info.cis_conn_handle, &conn_id))
                    {
                        info.conn_handle = le_get_conn_handle(conn_id);
                        if (info.conn_handle != 0xFFFF)
                        {
                            info.cig_id = p_data->p_cig_mgr_create_cis_rsp->cis_info[i].cig_id;
                            info.cis_id = p_data->p_cig_mgr_create_cis_rsp->cis_info[i].cis_id;
                            ble_audio_group_send_msg(AUDIO_GROUP_MSG_BAP_CIS_DISCONN, p_audio_session->group_handle,
                                                     (uint8_t *)&info, sizeof(info), true);
                        }
                    }
                }
                bap_unicast_audio_check_state(handle, NULL,
                                              AUDIO_STREAM_EVENT_CREATE_CIS, (uint8_t *)p_data->p_cig_mgr_create_cis_rsp);
            }
        }
        break;

    case MSG_CIG_MGR_CIS_ESTABLISHED_INFO:
        {
            if (p_data->p_cig_mgr_cis_established_info->cause != GAP_SUCCESS)
            {
                T_AUDIO_GROUP_BAP_CIS_DISCONN info = {0};
                T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;

                info.handle = handle;
                info.cause = p_data->p_cig_mgr_cis_established_info->cause;
                info.cis_conn_handle = p_data->p_cig_mgr_cis_established_info->cis_conn_handle;
                info.conn_handle = le_get_conn_handle(p_data->p_cig_mgr_cis_established_info->conn_id);
                if (info.conn_handle != 0xFFFF)
                {
                    info.cig_id = p_data->p_cig_mgr_cis_established_info->cig_id;
                    info.cis_id = p_data->p_cig_mgr_cis_established_info->cis_id;
                    ble_audio_group_send_msg(AUDIO_GROUP_MSG_BAP_CIS_DISCONN, p_audio_session->group_handle,
                                             (uint8_t *)&info, sizeof(info), true);
                }


                bap_unicast_audio_check_state(handle, NULL,
                                              AUDIO_STREAM_EVENT_READ_ASE_STATE, NULL);

            }
        }
        break;

    case MSG_CIG_MGR_SETUP_DATA_PATH:
        {
            if (p_data->p_cig_mgr_setup_data_path_rsp->cause == GAP_SUCCESS)
            {
                T_CIS_INFO cis_info;
                if (cig_mgr_get_cis_info(p_data->p_cig_mgr_setup_data_path_rsp->cis_conn_handle, &cis_info))
                {
                    bap_unicast_audio_check_state(handle, NULL,
                                                  AUDIO_STREAM_EVENT_DATA_PATH_ADD, NULL);

                }
            }
        }
        break;

    case MSG_CIG_MGR_REMOVE_DATA_PATH:
        {
            if (p_data->p_cig_mgr_remove_data_path_rsp->cause == GAP_SUCCESS)
            {
                T_CIS_INFO cis_info;
                if (cig_mgr_get_cis_info(p_data->p_cig_mgr_remove_data_path_rsp->cis_conn_handle, &cis_info))
                {
                    bap_unicast_audio_check_state(handle, NULL,
                                                  AUDIO_STREAM_EVENT_DATA_PATH_REMOVE, NULL);
                }
            }
        }
        break;

    case MSG_CIG_MGR_DISCONNECT:
        {
        }
        break;

    case MSG_CIG_MGR_DISCONNECT_INFO:
        {
            T_AUDIO_GROUP_BAP_CIS_DISCONN info = {0};
            T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;

            info.handle = handle;
            info.cause = p_data->p_cig_mgr_disconnect_info->cause;
            info.cis_conn_handle = p_data->p_cig_mgr_disconnect_info->cis_conn_handle;
            info.conn_handle = le_get_conn_handle(p_data->p_cig_mgr_disconnect_info->conn_id);
            if (info.conn_handle != 0xFFFF)
            {
                info.cig_id = p_data->p_cig_mgr_disconnect_info->cig_id;
                info.cis_id = p_data->p_cig_mgr_disconnect_info->cis_id;
                ble_audio_group_send_msg(AUDIO_GROUP_MSG_BAP_CIS_DISCONN, p_audio_session->group_handle,
                                         (uint8_t *)&info, sizeof(info), true);
            }

            bap_unicast_audio_check_state(handle, NULL,
                                          AUDIO_STREAM_EVENT_READ_ASE_STATE, NULL);
            if ((p_data->p_cig_mgr_disconnect_info->cause != (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE)) &&
                (p_data->p_cig_mgr_disconnect_info->cause != (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE)))
            {
                if (p_audio_session->state == AUDIO_STREAM_STATE_STARTING)
                {
                    bap_unicast_audio_check_state(handle, NULL,
                                                  AUDIO_STREAM_EVENT_CIS_DISCONN, (uint8_t *)p_data->p_cig_mgr_disconnect_info);
                }
            }
        }
        break;

    default:
        break;
    }
    return result;
}

bool bap_unicast_create_cis(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    uint16_t cis_conn_handle;
    uint16_t cis_handle_tbl[4];
    uint16_t cis_num = 0;
    uint16_t conn_handle_tbl[4];
    uint16_t dev_num = 0;
    T_GAP_CAUSE cause = GAP_CAUSE_SUCCESS;
    uint8_t err_idx = 0;
    T_ISOCH_INFO isoch_info;

    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        T_AUDIO_STREAM_SERVER *p_srv = &p_audio_session->server[i];
        T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;
        bool need_create_cis = false;

        if (bap_unicast_check_dev_state(p_dev, true) == false)
        {
            continue;
        }

        for (uint8_t j = 0; j < p_srv->cis_num; j++)
        {
            if (p_srv->cis_info[j].cis_id != 0)
            {
                if (cig_mgr_get_cis_handle(p_srv->cis_info[j].cis_id, &cis_conn_handle))
                {
                    if (cis_conn_handle != 0)
                    {
                        if (cig_mgr_get_isoch_info(cis_conn_handle, &isoch_info) == false)
                        {
                            err_idx = 1;
                            goto failed;
                        }
                    }
                    else
                    {
                        err_idx = 2;
                        goto failed;
                    }
                }
                else
                {
                    goto failed;
                }
                if (isoch_info.state == ISOCH_STATE_IDLE)
                {
                    cause = cig_mgr_set_cis_acl_link(p_srv->cis_info[j].cis_id, p_dev->p_link->conn_id);
                    if (cause != GAP_CAUSE_SUCCESS)
                    {
                        err_idx = 3;
                        goto failed;
                    }
                    else
                    {
                        cis_handle_tbl[cis_num] = cis_conn_handle;
                        cis_num++;
                        need_create_cis = true;
                    }
                }
            }
        }
        if (need_create_cis)
        {
            conn_handle_tbl[dev_num] = p_dev->p_link->conn_handle;
            dev_num++;
        }
    }
    if (cis_num)
    {
        cause = cig_mgr_create_cis_by_cis_conn_handle(p_audio_session->cig_id, cis_num,
                                                      cis_handle_tbl);
        if (cause == GAP_CAUSE_SUCCESS)
        {
            T_AUDIO_GROUP_BAP_CREATE_CIS info = {0};

            info.handle = p_audio_session;
            info.cig_id = p_audio_session->cig_id;
            info.dev_num = dev_num;
            for (uint8_t i = 0; i < dev_num; i++)
            {
                info.conn_handle_tbl[i] = conn_handle_tbl[i];
            }
            ble_audio_group_send_msg(AUDIO_GROUP_MSG_BAP_CREATE_CIS, p_audio_session->group_handle,
                                     (uint8_t *)&info, sizeof(info), true);
        }
    }
    else
    {
        cause = GAP_CAUSE_INVALID_STATE;
    }

    if (cause != GAP_CAUSE_SUCCESS)
    {
        err_idx = 4;
        goto failed;
    }
    return true;
failed:
    PROTOCOL_PRINT_ERROR2("bap_unicast_create_cis: failed, err_idx %d ,cause %d", err_idx, cause);
    return false;
}

bool bap_unicast_get_transport_latency(T_AUDIO_STREAM_SESSION *p_audio_session,
                                       uint16_t *p_sink_transport_latency_max,
                                       uint16_t *p_source_transport_latency_max)
{
    T_ASE_DATA_CODEC_CONFIGURED ase_codec_data;
    uint16_t  transport_latency_max = 0;
    *p_sink_transport_latency_max = 0;
    *p_source_transport_latency_max = 0;
    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        T_AUDIO_STREAM_SERVER *p_srv = &p_audio_session->server[i];
        T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;
        if (bap_unicast_check_dev_state(p_dev, true) == false)
        {
            continue;
        }
        for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
        {
            T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
            if (p_ase_entry != NULL && p_ase_entry->ase_state != ASE_STATE_IDLE &&
                p_ase_entry->ase_state != ASE_STATE_RELEASING)
            {
                if (ascs_client_get_codec_data(p_dev->p_link->conn_handle, p_ase_entry->ase_id,
                                               &ase_codec_data) == false)
                {
                    return false;
                }
                LE_ARRAY_TO_UINT16(transport_latency_max, ase_codec_data.data.max_transport_latency);
                if (p_srv->ase_info[j].direction == SERVER_AUDIO_SINK)
                {
                    if (*p_sink_transport_latency_max == 0)
                    {
                        *p_sink_transport_latency_max = transport_latency_max;
                    }
                    else
                    {
                        if (transport_latency_max < *p_sink_transport_latency_max)
                        {
                            *p_sink_transport_latency_max = transport_latency_max;
                        }
                    }
                }
                else
                {
                    if (*p_source_transport_latency_max == 0)
                    {
                        *p_source_transport_latency_max = transport_latency_max;
                    }
                    else
                    {
                        if (transport_latency_max < *p_source_transport_latency_max)
                        {
                            *p_source_transport_latency_max = transport_latency_max;
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool bap_unicast_get_presentation_delay(T_AUDIO_STREAM_SESSION *p_audio_session,
                                        T_AUDIO_DIRECTION direction,
                                        uint32_t *p_preferred_presentation_delay_min, uint32_t *p_preferred_presentation_delay_max,
                                        uint32_t *p_presentation_delay_min, uint32_t *p_presentation_delay_max)
{
    T_ASE_DATA_CODEC_CONFIGURED ase_codec_data;
    uint32_t delay_min = 0, delay_max = 0;
    uint32_t prefer_delay_min = 0, prefer_delay_max = 0;
    *p_presentation_delay_min = 0;
    *p_presentation_delay_max = 0;
    *p_preferred_presentation_delay_min = 0;
    *p_preferred_presentation_delay_max = 0;
    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        T_AUDIO_STREAM_SERVER *p_srv = &p_audio_session->server[i];
        T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;


        if (bap_unicast_check_dev_state(p_dev, true) == false)
        {
            continue;
        }

        for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
        {
            uint32_t presentation_delay_min = 0, presentation_delay_max = 0;
            uint32_t prefer_presentation_delay_min = 0, prefer_presentation_delay_max = 0;
            T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
            if (p_ase_entry != NULL && p_ase_entry->ase_state != ASE_STATE_IDLE &&
                p_ase_entry->ase_state != ASE_STATE_RELEASING)
            {
                if (p_srv->ase_info[j].direction == direction)
                {
                    if (ascs_client_get_codec_data(p_dev->p_link->conn_handle, p_ase_entry->ase_id,
                                                   &ase_codec_data) == false)
                    {
                        return false;
                    }
                    LE_ARRAY_TO_UINT24(presentation_delay_min, ase_codec_data.data.presentation_delay_min);
                    LE_ARRAY_TO_UINT24(presentation_delay_max, ase_codec_data.data.presentation_delay_max);
                    LE_ARRAY_TO_UINT24(prefer_presentation_delay_min,
                                       ase_codec_data.data.preferred_presentation_delay_min);
                    LE_ARRAY_TO_UINT24(prefer_presentation_delay_max,
                                       ase_codec_data.data.preferred_presentation_delay_max);
                    if (delay_min < presentation_delay_min)
                    {
                        delay_min = presentation_delay_min;
                    }
                    if (delay_max == 0)
                    {
                        delay_max = presentation_delay_max;
                    }
                    else
                    {
                        if (delay_max > presentation_delay_max)
                        {
                            delay_max = presentation_delay_max;
                        }
                    }

                    if (prefer_delay_min < prefer_presentation_delay_min)
                    {
                        prefer_delay_min = prefer_presentation_delay_min;
                    }
                    if (prefer_delay_max == 0)
                    {
                        prefer_delay_max = prefer_presentation_delay_max;
                    }
                    else
                    {
                        if (prefer_delay_max > prefer_presentation_delay_max)
                        {
                            prefer_delay_max = prefer_presentation_delay_max;
                        }
                    }
                }
            }
        }
    }

    PROTOCOL_PRINT_INFO5("[BAP][ASCS] bap_unicast_get_presentation_delay: direction %d, delay (0x%x -> 0x%x), prefer delay(0x%x -> 0x%x)",
                         direction, delay_min, delay_max,
                         prefer_delay_min, prefer_delay_max);

    if (prefer_delay_min <= prefer_delay_max)
    {
        *p_preferred_presentation_delay_min = prefer_delay_min;
        *p_preferred_presentation_delay_max = prefer_delay_max;
    }

    if (delay_min <= delay_max && delay_max != 0)
    {
        *p_presentation_delay_min = delay_min;
        *p_presentation_delay_max = delay_max;
        return true;
    }
    return false;
}

bool bap_unicast_gen_qos_param(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    T_ASE_DATA_CODEC_CONFIGURED ase_codec_data = {0};
    uint16_t max_sdu_size;
    uint32_t presentation_delay_min = 0, presentation_delay_max = 0;
    uint32_t preferred_presentation_delay_min = 0, preferred_presentation_delay_max = 0;
    uint32_t sink_presentation_delay = 0;
    uint32_t source_presentation_delay = 0;
    uint8_t  framing = 0xff;
    uint32_t sdu_interval;/*3bytes*/
    uint8_t cfg_dev_num = 0;
    uint8_t err_idx = 0;
    uint8_t conn_dev_num = 0;

    if (bap_unicast_get_presentation_delay(p_audio_session, SERVER_AUDIO_SINK,
                                           &preferred_presentation_delay_min, &preferred_presentation_delay_max,
                                           &presentation_delay_min, &presentation_delay_max))
    {
        if (preferred_presentation_delay_max != 0)
        {
            sink_presentation_delay = preferred_presentation_delay_max;
        }
        else
        {
            if (AUDIO_DEFAULT_PRESENTATION_DELAY < presentation_delay_min ||
                AUDIO_DEFAULT_PRESENTATION_DELAY > presentation_delay_max)
            {
                sink_presentation_delay = presentation_delay_max;
            }
            else
            {
                sink_presentation_delay = AUDIO_DEFAULT_PRESENTATION_DELAY;
            }
        }
    }
    if (bap_unicast_get_presentation_delay(p_audio_session, SERVER_AUDIO_SOURCE,
                                           &preferred_presentation_delay_min, &preferred_presentation_delay_max,
                                           &presentation_delay_min, &presentation_delay_max))
    {
        if (preferred_presentation_delay_max != 0)
        {
            source_presentation_delay = preferred_presentation_delay_max;
        }
        else
        {
            if (AUDIO_DEFAULT_PRESENTATION_DELAY < presentation_delay_min ||
                AUDIO_DEFAULT_PRESENTATION_DELAY > presentation_delay_max)
            {
                source_presentation_delay = presentation_delay_max;
            }
            else
            {
                source_presentation_delay = AUDIO_DEFAULT_PRESENTATION_DELAY;
            }
        }
    }
    bap_unicast_get_transport_latency(p_audio_session,
                                      &p_audio_session->session_qos.latency_m_s, &p_audio_session->session_qos.latency_s_m);
    p_audio_session->session_qos.sink_presentation_delay = sink_presentation_delay;
    p_audio_session->session_qos.source_presentation_delay = source_presentation_delay;
    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        bool handled = false;
        T_AUDIO_STREAM_SERVER *p_srv = &p_audio_session->server[i];
        T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;
        p_srv->cfg_flags = 0;

        if (bap_unicast_check_dev_state(p_dev, true) == false)
        {
            continue;
        }
        conn_dev_num++;

        for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
        {
            T_BAP_ASE_INFO *p_ase_info = &p_srv->ase_info[j];
            if (p_ase_info->p_ase_entry)
            {
                handled = true;
                if (p_ase_info->p_ase_entry->ase_state != ASE_STATE_CODEC_CONFIGURED)
                {
                    continue;
                }
                memset(&p_ase_info->qos_cfg, 0, sizeof(T_AUDIO_ASE_QOS_CFG));

                if (ascs_client_get_codec_data(p_dev->p_link->conn_handle, p_ase_info->p_ase_entry->ase_id,
                                               &ase_codec_data) == false)
                {
                    err_idx = 1;
                    goto failed;
                }
                if (p_ase_info->ase_codec_cfg.codec_cfg.sample_frequency != SAMPLING_FREQUENCY_CFG_44_1K &&
                    ase_codec_data.data.supported_framing == AUDIO_UNFRAMED_NOT_SUPPORTED)
                {
                    err_idx = 2;
                    goto failed;
                }
                if (p_ase_info->ase_codec_cfg.codec_cfg.sample_frequency == SAMPLING_FREQUENCY_CFG_44_1K)
                {
                    p_audio_session->session_qos.framing = AUDIO_FRAMED;
                }
                else
                {
                    p_audio_session->session_qos.framing = AUDIO_UNFRAMED;
                }
                if (framing == 0xff)
                {
                    framing = p_audio_session->session_qos.framing;
                }
                else
                {
                    if (framing != p_audio_session->session_qos.framing)
                    {
                        err_idx = 3;
                        goto failed;
                    }
                }
                if (codec_max_sdu_len_get(&p_ase_info->ase_codec_cfg.codec_cfg, &max_sdu_size) == false)
                {
                    if ((p_ase_info->ase_codec_cfg.codec_cfg.type_exist & CODEC_CFG_VENDOR_INFO_EXIST) == 0)
                    {
                        err_idx = 4;
                        goto failed;
                    }
                }

                if (codec_sdu_interval_get(&p_ase_info->ase_codec_cfg.codec_cfg, &sdu_interval) == false)
                {
                    if ((p_ase_info->ase_codec_cfg.codec_cfg.type_exist & CODEC_CFG_VENDOR_INFO_EXIST) == 0)
                    {
                        err_idx = 5;
                        goto failed;
                    }
                }
                if (p_srv->ase_info[j].direction == SERVER_AUDIO_SOURCE)
                {
                    if (p_audio_session->session_qos.sdu_interval_s_m == 0)
                    {
                        p_audio_session->session_qos.sdu_interval_s_m = sdu_interval;
                    }
                    else
                    {
                        if (sdu_interval != p_audio_session->session_qos.sdu_interval_s_m)
                        {
                            err_idx = 6;
                            goto failed;
                        }
                    }
                }
                else
                {
                    if (p_audio_session->session_qos.sdu_interval_m_s == 0)
                    {
                        p_audio_session->session_qos.sdu_interval_m_s = sdu_interval;
                    }
                    else
                    {
                        if (sdu_interval != p_audio_session->session_qos.sdu_interval_m_s)
                        {
                            err_idx = 7;
                            goto failed;
                        }
                    }
                }
                p_ase_info->qos_cfg.phy = ase_codec_data.data.preferred_phy;
                p_ase_info->qos_cfg.max_sdu = max_sdu_size;
                p_ase_info->qos_cfg.retransmission_number = ase_codec_data.data.preferred_retrans_number;
            }
        }
        if (handled)
        {
            p_srv->cfg_flags |= AUDIO_STREAM_SERVER_CFG_QOS;
            cfg_dev_num++;
        }
    }
    if (conn_dev_num == 0)
    {
        err_idx = 10;
        goto failed;
    }

    if (cfg_dev_num == 0)
    {
        err_idx = 11;
        goto failed;
    }
    bap_unicast_send_start_qos_cfg(p_audio_session);

    if (p_audio_session->server_num != cfg_dev_num)
    {
        T_AUDIO_ASE_QOS_CFG  sink_qos_cfg = {0};
        T_AUDIO_ASE_QOS_CFG  source_qos_cfg = {0};
        PROTOCOL_PRINT_INFO2("bap_unicast_gen_qos_param: server_num %d, cfg_dev_num %d",
                             p_audio_session->server_num, cfg_dev_num);
        for (uint8_t i = 0; i < p_audio_session->server_num; i++)
        {
            T_AUDIO_STREAM_SERVER *p_srv = &p_audio_session->server[i];
            if (p_srv->cfg_flags & AUDIO_STREAM_SERVER_CFG_QOS)
            {
                for (uint8_t j = 0; j < p_srv->ase_num; j++)
                {
                    T_BAP_ASE_INFO *p_ase_info = &p_srv->ase_info[j];
                    if (p_ase_info->direction == SERVER_AUDIO_SINK)
                    {
                        memcpy(&sink_qos_cfg, &p_ase_info->qos_cfg, sizeof(T_AUDIO_ASE_QOS_CFG));
                    }
                    else if (p_ase_info->direction == SERVER_AUDIO_SOURCE)
                    {
                        memcpy(&source_qos_cfg, &p_ase_info->qos_cfg, sizeof(T_AUDIO_ASE_QOS_CFG));
                    }
                }
            }
        }
        for (uint8_t i = 0; i < p_audio_session->server_num; i++)
        {
            T_AUDIO_STREAM_SERVER *p_srv = &p_audio_session->server[i];
            if ((p_srv->cfg_flags & AUDIO_STREAM_SERVER_CFG_QOS) == 0)
            {
                for (uint8_t j = 0; j < p_srv->ase_num; j++)
                {
                    T_BAP_ASE_INFO *p_ase_info = &p_srv->ase_info[j];
                    if (p_ase_info->p_ase_entry == NULL)
                    {
                        memset(&p_ase_info->qos_cfg, 0, sizeof(T_AUDIO_ASE_QOS_CFG));
                        if (codec_max_sdu_len_get(&p_ase_info->ase_codec_cfg.codec_cfg, &max_sdu_size) == false)
                        {
                            if ((p_ase_info->ase_codec_cfg.codec_cfg.type_exist & CODEC_CFG_VENDOR_INFO_EXIST) == 0)
                            {
                                err_idx = 8;
                                goto failed;
                            }
                        }
                        p_ase_info->qos_cfg.max_sdu = max_sdu_size;
                        p_ase_info->qos_cfg.phy = ase_codec_data.data.preferred_phy;
                        p_ase_info->qos_cfg.retransmission_number = ase_codec_data.data.preferred_retrans_number;
                        if (p_ase_info->direction == SERVER_AUDIO_SINK && sink_qos_cfg.phy != 0)
                        {
                            p_ase_info->qos_cfg.phy = sink_qos_cfg.phy;
                            p_ase_info->qos_cfg.retransmission_number = sink_qos_cfg.retransmission_number;
                        }
                        else if (p_ase_info->direction == SERVER_AUDIO_SOURCE && source_qos_cfg.phy != 0)
                        {
                            p_ase_info->qos_cfg.phy = source_qos_cfg.phy;
                            p_ase_info->qos_cfg.retransmission_number = source_qos_cfg.retransmission_number;
                        }
                    }
                }
            }
        }
    }

    return true;
failed:
    PROTOCOL_PRINT_ERROR1("bap_unicast_gen_qos_param: failed error index %d", err_idx);
    return false;
}

bool bap_unicast_config_cig(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    T_GAP_CAUSE cause;
    const T_UNICAST_AUDIO_CFG *p_cfg = &UNICAST_CFG_TBL[p_audio_session->cfg_type];
    cause = cig_mgr_reg_cig(p_audio_session->cig_id, bap_unicast_client_cig_cb);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        goto failed;
    }
    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        uint8_t sink_ase_num = 0;
        uint8_t source_ase_num = 0;
        T_AUDIO_STREAM_SERVER *p_srv = &p_audio_session->server[i];
        const T_UNICAST_AUDIO_CFG_SERVER *p_ser_cfg;
        if (p_audio_session->server_num > BAP_UNICAST_SERVERS_MAX_NUM)
        {
            p_ser_cfg = &p_cfg->server_cfg[0];
        }
        else
        {
            p_ser_cfg = &p_cfg->server_cfg[i];
        }
        for (uint8_t k = 0; k < p_srv->cis_num; k++)
        {
            T_BAP_CIS_PARAM *p_cis_info = &p_srv->cis_info[k];
            for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
            {
                T_BAP_ASE_INFO *p_ase_info = &p_srv->ase_info[j];
                if (p_srv->ase_info[j].cis_id != 0)
                {
                    continue;
                }
                if (p_ser_cfg->sink_ase_num == sink_ase_num &&
                    p_ser_cfg->source_ase_num == source_ase_num)
                {
                    break;
                }

                if (p_cis_info->cis_id == 0)
                {
                    do
                    {
                        p_cis_info->cis_id = audio_stream_get_free_cis_id();
                        cause = cig_mgr_add_cis(p_audio_session->cig_id, p_cis_info->cis_id);
                    }
                    while (cause == GAP_CAUSE_ALREADY_IN_REQ);

                    if (cause != GAP_CAUSE_SUCCESS)
                    {
                        goto release;
                    }
                }
                if (p_ase_info->qos_cfg.phy == 0)
                {
                    p_ase_info->qos_cfg.phy = ASE_LE_2M_PHY_BIT;
                }
                if (p_srv->ase_info[j].direction == SERVER_AUDIO_SOURCE &&
                    ((p_cis_info->path_direction & DATA_PATH_OUTPUT_FLAG) == 0))
                {
                    p_cis_info->path_direction |= DATA_PATH_OUTPUT_FLAG;
                    p_cis_info->source_ase_idx = j;
                    p_cis_info->cis_param.max_sdu_s_m = p_ase_info->qos_cfg.max_sdu;
                    p_cis_info->cis_param.phy_s_m = p_ase_info->qos_cfg.phy;
                    p_cis_info->cis_param.rtn_s_m = p_ase_info->qos_cfg.retransmission_number;
                    if (p_cis_info->cis_param.phy_m_s == 0)
                    {
                        p_cis_info->cis_param.phy_m_s = p_ase_info->qos_cfg.phy;
                    }
                    p_srv->ase_info[j].cis_id = p_cis_info->cis_id;
                    source_ase_num++;
                }
                if (p_srv->ase_info[j].direction == SERVER_AUDIO_SINK &&
                    ((p_cis_info->path_direction & DATA_PATH_INPUT_FLAG) == 0))
                {
                    p_cis_info->path_direction |= DATA_PATH_INPUT_FLAG;
                    p_cis_info->sink_ase_idx = j;
                    p_cis_info->cis_param.max_sdu_m_s = p_ase_info->qos_cfg.max_sdu;
                    p_cis_info->cis_param.phy_m_s = p_ase_info->qos_cfg.phy;
                    p_cis_info->cis_param.rtn_m_s = p_ase_info->qos_cfg.retransmission_number;
                    if (p_cis_info->cis_param.phy_s_m == 0)
                    {
                        p_cis_info->cis_param.phy_s_m = p_ase_info->qos_cfg.phy;
                    }
                    p_srv->ase_info[j].cis_id = p_cis_info->cis_id;
                    sink_ase_num++;
                }
                if (p_audio_session->cfg_type == UNICAST_AUDIO_CFG_7_I)
                {
                    break;
                }
                if (p_audio_session->cfg_type == UNICAST_AUDIO_CFG_8_I && k == 0)
                {
                    break;
                }
                if (p_cis_info->path_direction == (DATA_PATH_INPUT_FLAG & DATA_PATH_OUTPUT_FLAG))
                {
                    break;
                }
            }
            PROTOCOL_PRINT_INFO5("bap_unicast_config_cig: cis_id %d, sink_ase_idx %d, source_ase_idx %d, path_direction 0x%x, cis_conn_handle 0x%x",
                                 p_cis_info->cis_id,
                                 p_cis_info->sink_ase_idx,
                                 p_cis_info->source_ase_idx,
                                 p_cis_info->path_direction,
                                 p_cis_info->cis_conn_handle);
            if (p_cis_info->cis_id != 0)
            {
                if (cig_mgr_set_cis_param(p_cis_info->cis_id, &p_cis_info->cis_param) != GAP_CAUSE_SUCCESS)
                {
                    goto release;
                }
            }
        }
    }
    if (p_audio_session->session_qos.sdu_interval_m_s)
    {
        p_audio_session->cig_param.sdu_interval_m_s = p_audio_session->session_qos.sdu_interval_m_s;
        p_audio_session->cig_param.latency_m_s = p_audio_session->session_qos.latency_m_s;
    }
    else
    {
        p_audio_session->cig_param.sdu_interval_m_s = p_audio_session->session_qos.sdu_interval_s_m;
        p_audio_session->cig_param.latency_m_s = p_audio_session->session_qos.latency_s_m;
    }
    if (p_audio_session->session_qos.sdu_interval_s_m)
    {
        p_audio_session->cig_param.sdu_interval_s_m = p_audio_session->session_qos.sdu_interval_s_m;
        p_audio_session->cig_param.latency_s_m = p_audio_session->session_qos.latency_s_m;
    }
    else
    {
        p_audio_session->cig_param.sdu_interval_s_m = p_audio_session->session_qos.sdu_interval_m_s;
        p_audio_session->cig_param.latency_s_m = p_audio_session->session_qos.latency_m_s;
    }
    p_audio_session->cig_param.sca = p_audio_session->session_qos.sca;
    p_audio_session->cig_param.packing = p_audio_session->session_qos.packing;
    p_audio_session->cig_param.framing = p_audio_session->session_qos.framing;

    cause = cig_mgr_set_cig_param(p_audio_session->cig_id, &p_audio_session->cig_param);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        goto release;
    }

    cause = cig_mgr_start_setting(p_audio_session->cig_id);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        goto release;
    }
    p_audio_session->cig_state = T_CIG_SETTING_STATE_STARTING;
    //bap_unicast_client_print(p_audio_session);
    return true;
failed:
    PROTOCOL_PRINT_ERROR0("bap_unicast_config_cig: failed");
    return false;
release:
    PROTOCOL_PRINT_ERROR0("bap_unicast_config_cig: failed, release cig");
    cig_mgr_remove_cig(p_audio_session->cig_id);
    memset(&p_audio_session->cig_param, 0, sizeof(T_CIG_CFG_PARAM));
    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        memset(p_audio_session->server[i].cis_info, 0, AUDIO_STREAM_SRV_CIS_MAX * sizeof(T_BAP_CIS_PARAM));
        for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
        {
            p_audio_session->server[i].ase_info[j].cis_id = 0;
        }
    }
    return false;
}

bool bap_unicast_check_audio_cfg(T_UNICAST_AUDIO_CFG_TYPE cfg_type,
                                 uint8_t server_num, T_BLE_AUDIO_DEV_HANDLE *p_dev_tbl)
{
    uint8_t err_idx = 0;
    if (cfg_type < UNICAST_AUDIO_CFG_MAX)
    {
        T_BAP_LINK_CB *p_temp_bap_cb = NULL;
        const T_UNICAST_AUDIO_CFG *p_cfg = &UNICAST_CFG_TBL[cfg_type];
        uint8_t i = 0;
        if (p_cfg->server_num != server_num)
        {
            return false;
        }
        //found connected device
        for (i = 0; i < server_num; i++)
        {
            T_LE_AUDIO_DEV *p_dev = (T_LE_AUDIO_DEV *)p_dev_tbl[i];
            if (p_dev->p_link != NULL && p_dev->p_link->p_bap_cb != NULL)
            {
                p_temp_bap_cb = p_dev->p_link->p_bap_cb;
                break;
            }
        }
        if (p_temp_bap_cb == NULL)
        {
            err_idx = 2;
            goto failed;
        }
        for (i = 0; i < server_num; i++)
        {
            T_LE_AUDIO_DEV *p_dev = (T_LE_AUDIO_DEV *)p_dev_tbl[i];
            T_BAP_LINK_CB *p_bap_cb;
            const T_UNICAST_AUDIO_CFG_SERVER *p_ser_cfg;
            if (server_num > BAP_UNICAST_SERVERS_MAX_NUM)
            {
                p_ser_cfg = &p_cfg->server_cfg[0];
            }
            else
            {
                p_ser_cfg = &p_cfg->server_cfg[i];
            }
            if (p_dev->p_link != NULL && p_dev->p_link->p_bap_cb != NULL)
            {
                p_bap_cb = p_dev->p_link->p_bap_cb;
            }
            else
            {
                p_bap_cb = p_temp_bap_cb;
            }
            if (p_bap_cb->p_ascs_cb == NULL)
            {
                err_idx = 3;
                goto failed;
            }
#if LE_AUDIO_PACS_CLIENT_SUPPORT
            if (p_ser_cfg->sink_min_locations > 1)
            {
                if (p_ser_cfg->sink_min_locations > __builtin_popcount(p_bap_cb->pacs_cb.snk_audio_loc))
                {
                    err_idx = 4;
                    goto failed;
                }
            }
            if (p_ser_cfg->source_min_locations > 1)
            {
                if (p_ser_cfg->source_min_locations > __builtin_popcount(p_bap_cb->pacs_cb.src_audio_loc))
                {
                    err_idx = 5;
                    goto failed;
                }
            }
#endif
            if (p_ser_cfg->sink_ase_num > bap_unicast_get_idle_ase_num(p_bap_cb->p_ascs_cb,
                                                                       SERVER_AUDIO_SINK))
            {
                err_idx = 6;
                goto failed;
            }
            if (p_ser_cfg->source_ase_num > bap_unicast_get_idle_ase_num(p_bap_cb->p_ascs_cb,
                                                                         SERVER_AUDIO_SOURCE))
            {
                err_idx = 7;
                goto failed;
            }
        }
    }
    else
    {
        err_idx = 8;
        goto failed;
    }
    return true;
failed:
    PROTOCOL_PRINT_ERROR1("bap_unicast_check_audio_cfg: failed, err_idx %d", err_idx);
    return false;
}

bool bap_unicast_audio_assign_ase(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    bool dev_connected = false;
    uint8_t err_idx = 0;
    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        uint8_t ase_idx = 0;
        T_LE_AUDIO_DEV *p_dev = (T_LE_AUDIO_DEV *)p_audio_session->server[i].dev_handle;
        T_BAP_ASCS_CB *p_ascs_cb;
        T_BAP_ASE_INFO *p_ase_info;
        T_BAP_LINK_CB *p_bap_cb;

        if (bap_unicast_check_dev_state(p_dev, true) == false)
        {
            continue;
        }

        p_bap_cb = p_dev->p_link->p_bap_cb;
        p_ascs_cb = p_bap_cb->p_ascs_cb;
        dev_connected = true;
        for (ase_idx = 0; ase_idx < p_audio_session->server[i].ase_num; ase_idx++)
        {
            p_ase_info = &p_audio_session->server[i].ase_info[ase_idx];
            if (p_ase_info->p_ase_entry == NULL)
            {
                if (p_ase_info->direction == SERVER_AUDIO_SINK)
                {
                    for (uint8_t j = 0; j < p_ascs_cb->snk_ase_num; j++)
                    {
                        if (p_ascs_cb->ase_data[j].used == false &&
                            p_ascs_cb->ase_data[j].ase_id != 0 &&
                            (p_ascs_cb->ase_data[j].ase_state == ASE_STATE_IDLE ||
                             p_ascs_cb->ase_data[j].ase_state == ASE_STATE_CODEC_CONFIGURED))
                        {
                            p_ascs_cb->ase_data[j].used = true;
                            p_ascs_cb->ase_data[j].handle = p_audio_session;
                            p_ase_info->p_ase_entry = &p_ascs_cb->ase_data[j];
                            break;
                        }
                    }
                    if (p_ase_info->p_ase_entry == NULL)
                    {
                        err_idx = 1;
                        goto failed;
                    }
                }
                else if (p_ase_info->direction == SERVER_AUDIO_SOURCE)
                {
                    for (uint8_t j = 0; j < p_ascs_cb->src_ase_num; j++)
                    {
                        uint8_t idx = j + p_ascs_cb->snk_ase_num;
                        if (p_ascs_cb->ase_data[idx].used == false &&
                            p_ascs_cb->ase_data[idx].ase_id != 0 &&
                            (p_ascs_cb->ase_data[idx].ase_state == ASE_STATE_IDLE ||
                             p_ascs_cb->ase_data[idx].ase_state == ASE_STATE_CODEC_CONFIGURED))
                        {
                            p_ascs_cb->ase_data[idx].used = true;
                            p_ascs_cb->ase_data[idx].handle = p_audio_session;
                            p_ase_info->p_ase_entry = &p_ascs_cb->ase_data[idx];
                            break;
                        }
                    }
                    if (p_ase_info->p_ase_entry == NULL)
                    {
                        err_idx = 2;
                        goto failed;
                    }
                }
            }
        }
    }
    if (dev_connected == false)
    {
        PROTOCOL_PRINT_ERROR1("bap_unicast_audio_assign_ase: no connected dev, handle %p",
                              p_audio_session);
        return false;
    }
    //PROTOCOL_PRINT_INFO0("bap_unicast_audio_assign_ase");
    //bap_unicast_client_print(p_audio_session);
    return true;
failed:
    PROTOCOL_PRINT_ERROR2("bap_unicast_audio_assign_ase: failed, handle %p, err_idx %d",
                          p_audio_session, err_idx);
    if (p_audio_session->state == AUDIO_STREAM_STATE_IDLE_CONFIGURED &&
        p_audio_session->cig_state == T_CIG_SETTING_STATE_IDLE)
    {
        bap_unicast_audio_cig_remove(p_audio_session);
    }
    return false;
}

uint32_t bap_unicast_get_audio_cfg_mask(T_AUDIO_STREAM_SESSION_HANDLE handle, uint32_t prefer_cfg,
                                        uint8_t dev_num, T_BLE_AUDIO_DEV_HANDLE *p_dev_tbl)
{
    uint32_t mask = prefer_cfg & UNICAST_AUDIO_CFG_MASK;
    uint32_t supported_mask = 0;
    uint16_t index = 0;
    uint8_t err_idx = 0;
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    if (dev_num == 0 || p_dev_tbl == NULL)
    {
        err_idx = 1;
        goto failed;
    }
    if (audio_stream_session_check(handle) == false)
    {
        err_idx = 2;
        goto failed;
    }
    if (p_audio_session->state != AUDIO_STREAM_STATE_IDLE)
    {
        err_idx = 3;
        goto failed;
    }
    for (uint8_t i = 0; i < dev_num; i++)
    {
        if (ble_audio_dev_handle_check(p_audio_session->group_handle, p_dev_tbl[i]) == false)
        {
            err_idx = 4;
            goto failed;
        }
    }
    while (mask)
    {
        if (mask & 0x01)
        {
            if (bap_unicast_check_audio_cfg((T_UNICAST_AUDIO_CFG_TYPE)index, dev_num, p_dev_tbl))
            {
                supported_mask |= (1 << index);
            }
        }
        mask >>= 1;
        index++;
    }
    PROTOCOL_PRINT_INFO2("bap_unicast_get_audio_cfg_mask: handle %p, supported_mask 0x%x", handle,
                         supported_mask);
    return supported_mask;
failed:
    PROTOCOL_PRINT_ERROR2("bap_unicast_get_audio_cfg_mask: failed, handle %p, err_idx %d", handle,
                          err_idx);
    return 0;
}

bool bap_unicast_audio_start_cfg(T_AUDIO_STREAM_SESSION *p_audio_session,
                                 T_UNICAST_AUDIO_CFG_TYPE cfg_type,
                                 uint8_t dev_num, T_BLE_AUDIO_DEV_HANDLE *p_dev_tbl)
{
    const T_UNICAST_AUDIO_CFG *p_cfg = &UNICAST_CFG_TBL[cfg_type];
    if (p_audio_session->server)
    {
        goto failed;
    }
    p_audio_session->server = ble_audio_mem_zalloc(dev_num * sizeof(T_AUDIO_STREAM_SERVER));
    if (p_audio_session->server == NULL)
    {
        goto failed;
    }
    p_audio_session->cfg_type = cfg_type;
    p_audio_session->server_num = dev_num;
    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        uint8_t ase_idx = 0;
        const T_UNICAST_AUDIO_CFG_SERVER *p_ser_cfg;
        uint8_t ase_num;
        if (p_audio_session->server_num > BAP_UNICAST_SERVERS_MAX_NUM)
        {
            p_ser_cfg = &p_cfg->server_cfg[0];
        }
        else
        {
            p_ser_cfg = &p_cfg->server_cfg[i];
        }
        ase_num = p_ser_cfg->sink_ase_num + p_ser_cfg->source_ase_num;
        p_audio_session->server[i].ase_info = ble_audio_mem_zalloc(ase_num * sizeof(T_BAP_ASE_INFO));
        if (p_audio_session->server[i].ase_info == NULL)
        {
            goto failed;
        }
        if (p_audio_session->cfg_type == UNICAST_AUDIO_CFG_11_I)
        {
            if (ase_num != 4)
            {
                goto failed;
            }
            p_audio_session->server[i].ase_info[0].direction = SERVER_AUDIO_SINK;
            p_audio_session->server[i].ase_info[0].channel_num = p_ser_cfg->sink_channels;
            p_audio_session->server[i].ase_info[1].direction = SERVER_AUDIO_SOURCE;
            p_audio_session->server[i].ase_info[1].channel_num = p_ser_cfg->source_channels;
            p_audio_session->server[i].ase_info[2].direction = SERVER_AUDIO_SINK;
            p_audio_session->server[i].ase_info[2].channel_num = p_ser_cfg->sink_channels;
            p_audio_session->server[i].ase_info[3].direction = SERVER_AUDIO_SOURCE;
            p_audio_session->server[i].ase_info[3].channel_num = p_ser_cfg->source_channels;
        }
        else
        {
            for (uint8_t j = 0; j < p_ser_cfg->sink_ase_num; j++)
            {
                p_audio_session->server[i].ase_info[ase_idx].direction = SERVER_AUDIO_SINK;
                p_audio_session->server[i].ase_info[ase_idx].channel_num = p_ser_cfg->sink_channels;
                ase_idx++;
            }
            for (uint8_t j = 0; j < p_ser_cfg->source_ase_num; j++)
            {
                p_audio_session->server[i].ase_info[ase_idx].direction = SERVER_AUDIO_SOURCE;
                p_audio_session->server[i].ase_info[ase_idx].channel_num = p_ser_cfg->source_channels;
                ase_idx++;
            }
        }
        p_audio_session->server[i].ase_num = ase_num;
        p_audio_session->server[i].dev_handle = p_dev_tbl[i];
        p_audio_session->server[i].cis_num = p_ser_cfg->cis_num;
    }
    return true;
failed:
    if (p_audio_session->server)
    {
        for (uint8_t i = 0; i < p_audio_session->server_num; i++)
        {
            if (p_audio_session->server[i].ase_info)
            {
                ble_audio_mem_free(p_audio_session->server[i].ase_info);
            }
        }
        ble_audio_mem_free(p_audio_session->server);
        p_audio_session->server = NULL;
        p_audio_session->server_num = 0;
    }
    PROTOCOL_PRINT_ERROR1("bap_unicast_audio_start_cfg: failed, session %p", p_audio_session);
    return false;
}

bool bap_unicast_audio_cfg(T_AUDIO_STREAM_SESSION_HANDLE handle, T_UNICAST_AUDIO_CFG_TYPE cfg_type,
                           uint8_t dev_num, T_BLE_AUDIO_DEV_HANDLE *p_dev_tbl)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    uint8_t err_idx = 0;
    if (audio_stream_session_check(handle) == false)
    {
        err_idx = 1;
        goto failed;
    }
    if (p_audio_session->state != AUDIO_STREAM_STATE_IDLE)
    {
        err_idx = 2;
        goto failed;
    }
    for (uint8_t i = 0; i < dev_num; i++)
    {
        if (ble_audio_dev_handle_check(p_audio_session->group_handle, p_dev_tbl[i]) == false)
        {
            err_idx = 3;
            goto failed;
        }
    }
    if (bap_unicast_check_audio_cfg(cfg_type, dev_num, p_dev_tbl) == false)
    {
        err_idx = 4;
        goto failed;
    }
    if (bap_unicast_audio_start_cfg(p_audio_session, cfg_type, dev_num, p_dev_tbl) == false)
    {
        err_idx = 5;
        goto failed;
    }
    PROTOCOL_PRINT_INFO3("bap_unicast_audio_cfg: handle %p, cfg_type %d, dev_num %d",
                         handle, cfg_type, dev_num);
    for (uint8_t i = 0; i < dev_num; i++)
    {
        PROTOCOL_PRINT_INFO1("bap_unicast_audio_cfg: p_dev_tbl %p",
                             p_dev_tbl[i]);
    }
    bap_unicast_send_bap_info(p_audio_session, AUDIO_STREAM_STATE_IDLE_CONFIGURED,
                              BAP_UNICAST_RESULT_SUCCESS, GAP_SUCCESS, NULL);
    return true;
failed:
    PROTOCOL_PRINT_ERROR2("bap_unicast_audio_cfg: failed, handle %p, err_idx %d", handle, err_idx);
    return false;
}

bool bap_unicast_audio_cfg_ase_codec(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                     T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                     uint8_t cfg_idx, T_AUDIO_ASE_CODEC_CFG *p_cfg)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    uint8_t err_idx = 0;
    if (audio_stream_session_check(handle) == false)
    {
        err_idx = 1;
        goto failed;
    }
    if (p_audio_session->state != AUDIO_STREAM_STATE_IDLE_CONFIGURED)
    {
        err_idx = 2;
        goto failed;
    }
    if (p_cfg == NULL)
    {
        err_idx = 3;
        goto failed;
    }

    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        if (p_audio_session->server[i].dev_handle == dev_handle)
        {
            if (cfg_idx >= p_audio_session->server[i].ase_num)
            {
                err_idx = 4;
                goto failed;
            }
            memcpy(&p_audio_session->server[i].ase_info[cfg_idx].ase_codec_cfg, p_cfg,
                   sizeof(T_AUDIO_ASE_CODEC_CFG));
            return true;
        }
    }
failed:
    PROTOCOL_PRINT_ERROR2("bap_unicast_audio_cfg_ase_codec: failed, handle %p, error idex %d", handle,
                          err_idx);
    return false;
}

bool bap_unicast_audio_cfg_ase_qos(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                   T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                   uint8_t ase_id, T_AUDIO_ASE_QOS_CFG *p_cfg)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    T_BAP_ASE_INFO *p_ase_entry;
    if (audio_stream_session_check(handle) == false)
    {
        goto failed;
    }
    if (p_cfg == NULL)
    {
        goto failed;
    }
    if (p_audio_session->cig_state != T_CIG_SETTING_STATE_IDLE)
    {
        goto failed;
    }
    if ((p_cfg->phy | ASE_LE_PHY_CFG_MASK) != ASE_LE_PHY_CFG_MASK)
    {
        goto failed;
    }
    if (p_cfg->max_sdu > QOS_MAX_SDU_MAX)
    {
        goto failed;
    }
    p_ase_entry = bap_unicast_get_ase_entry(p_audio_session, dev_handle, ase_id);
    if (p_ase_entry == NULL)
    {
        goto failed;
    }
    memcpy(&p_ase_entry->qos_cfg, p_cfg, sizeof(T_AUDIO_ASE_QOS_CFG));
    PROTOCOL_PRINT_INFO2("bap_unicast_audio_cfg_ase_qos: handle %p, ase_id %d",
                         handle, ase_id);
    return true;
failed:
    PROTOCOL_PRINT_ERROR1("bap_unicast_audio_cfg_ase_qos: failed, handle %p", handle);
    return false;
}

bool bap_unicast_audio_cfg_ase_metadata(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                        T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                        uint8_t ase_id, uint8_t metadata_len, uint8_t *p_metadata)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    T_BAP_ASE_INFO *p_ase_info = NULL;
    if (audio_stream_session_check(handle) == false)
    {
        goto failed;
    }
    if (metadata_len != 0 && p_metadata == NULL)
    {
        goto failed;
    }
    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        if (p_audio_session->server[i].dev_handle == dev_handle || dev_handle == NULL)
        {
            for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
            {
                p_ase_info = &p_audio_session->server[i].ase_info[j];
                if (ase_id == 0 || (p_ase_info->p_ase_entry && p_ase_info->p_ase_entry->ase_id == ase_id))
                {
                    if (p_ase_info->p_metadata)
                    {
                        ble_audio_mem_free(p_ase_info->p_metadata);
                        p_ase_info->metadata_len = 0;
                        p_ase_info->p_metadata = NULL;
                    }
                    if (metadata_len)
                    {
                        p_ase_info->p_metadata = ble_audio_mem_zalloc(metadata_len);
                        if (p_ase_info->p_metadata == NULL)
                        {
                            goto failed;
                        }
                        memcpy(p_ase_info->p_metadata, p_metadata, metadata_len);
                        p_ase_info->metadata_len = metadata_len;
                    }
                }
            }
        }

    }

    PROTOCOL_PRINT_INFO2("bap_unicast_audio_cfg_ase_metadata: dev_handle %p, ase_id %d",
                         dev_handle, ase_id);
    if (metadata_len != 0)
    {
        PROTOCOL_PRINT_INFO2("bap_unicast_audio_cfg_ase_metadata: metadata[%d] %b",
                             metadata_len, TRACE_BINARY(metadata_len, p_metadata));
    }
    return true;
failed:
    PROTOCOL_PRINT_ERROR1("bap_unicast_audio_cfg_ase_metadata: failed, handle %p", handle);
    return false;
}

bool bap_unicast_audio_cfg_session_qos(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                       T_AUDIO_SESSION_QOS_CFG *p_cfg)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    if (audio_stream_session_check(handle) == false)
    {
        goto failed;
    }
    if (p_cfg == NULL)
    {
        goto failed;
    }
    if (p_audio_session->cig_state != T_CIG_SETTING_STATE_IDLE)
    {
        goto failed;
    }
    if (p_cfg->sca > 0x07 || p_cfg->packing > 0x01 || p_cfg->framing > 0x01)
    {
        goto failed;
    }
    if ((p_cfg->sdu_interval_m_s != 0 && p_cfg->sdu_interval_m_s < QOS_SDU_INTERVAL_MIN) ||
        p_cfg->sdu_interval_m_s > QOS_SDU_INTERVAL_MAX ||
        (p_cfg->sdu_interval_s_m != 0 && p_cfg->sdu_interval_s_m < QOS_SDU_INTERVAL_MIN) ||
        p_cfg->sdu_interval_s_m > QOS_SDU_INTERVAL_MAX)
    {
        goto failed;
    }
    if (p_cfg->latency_m_s > QOS_TRANSPORT_LATENCY_MAX ||
        p_cfg->latency_s_m > QOS_TRANSPORT_LATENCY_MAX)
    {
        goto failed;
    }
    memcpy(&p_audio_session->session_qos, p_cfg, sizeof(T_AUDIO_SESSION_QOS_CFG));
    PROTOCOL_PRINT_INFO1("bap_unicast_audio_cfg_session_qos: handle %p",
                         handle);
    return true;
failed:
    PROTOCOL_PRINT_ERROR1("bap_unicast_audio_cfg_session_qos: failed, handle %p", handle);
    return false;
}

bool bap_unicast_audio_get_session_qos(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                       T_AUDIO_SESSION_QOS_CFG *p_cfg)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    if (audio_stream_session_check(handle) == false)
    {
        goto failed;
    }
    if (p_cfg == NULL)
    {
        goto failed;
    }
    memcpy(p_cfg, &p_audio_session->session_qos, sizeof(T_AUDIO_SESSION_QOS_CFG));
    PROTOCOL_PRINT_INFO1("bap_unicast_audio_get_session_qos: handle %p",
                         handle);
    return true;
failed:
    PROTOCOL_PRINT_ERROR1("bap_unicast_audio_get_session_qos: failed, handle %p", handle);
    return false;
}

bool bap_unicast_audio_get_ase_qos(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                   T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                   uint8_t ase_id, T_AUDIO_ASE_QOS_CFG *p_cfg)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    T_BAP_ASE_INFO *p_ase_entry;
    if (audio_stream_session_check(handle) == false)
    {
        goto failed;
    }
    if (p_cfg == NULL)
    {
        goto failed;
    }
    p_ase_entry = bap_unicast_get_ase_entry(p_audio_session, dev_handle, ase_id);
    if (p_ase_entry == NULL)
    {
        goto failed;
    }
    memcpy(p_cfg, &p_ase_entry->qos_cfg, sizeof(T_AUDIO_ASE_QOS_CFG));
    PROTOCOL_PRINT_INFO2("bap_unicast_audio_get_ase_qos: handle %p, ase_id %d",
                         handle, ase_id);
    return true;
failed:
    PROTOCOL_PRINT_ERROR1("bap_unicast_audio_get_ase_qos: failed, handle %p", handle);
    return false;
}

static void bap_unicast_cp_timeout(T_SYS_TIMER_HANDLE handle)
{
    uint32_t timer_id = sys_timer_id_get(handle);
    T_AUDIO_STREAM_SESSION *p_audio_session;
    p_audio_session = (T_AUDIO_STREAM_SESSION *)timer_id;
    PROTOCOL_PRINT_INFO1("bap_unicast_cp_timeout: p_audio_session %p", p_audio_session);
    if (audio_stream_session_check(p_audio_session))
    {
        for (uint8_t i = 0; i < p_audio_session->server_num; i++)
        {
            T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;

            if (bap_unicast_check_dev_state(p_dev, true) == false)
            {
                continue;
            }

            for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
            {
                T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
                if (p_ase_entry && p_ase_entry->wait_cp)
                {
                    ascs_client_read_ase(p_dev->p_link->conn_handle, p_ase_entry->instance_id,
                                         p_audio_session->server[i].ase_info[j].direction);
                }
            }
        }
    }
}

void bap_unicast_start_timer(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    PROTOCOL_PRINT_INFO0("bap_unicast_start_timer");
    if (p_audio_session->p_session_timer_handle == NULL)
    {
        p_audio_session->p_session_timer_handle = sys_timer_create("ase_op_timer",
                                                                   SYS_TIMER_TYPE_LOW_PRECISION,
                                                                   (uint32_t)p_audio_session,
                                                                   1000000, false, bap_unicast_cp_timeout);
        if (p_audio_session->p_session_timer_handle)
        {
            sys_timer_start(p_audio_session->p_session_timer_handle);
        }
    }
    else
    {
        sys_timer_restart(p_audio_session->p_session_timer_handle, 1000000);
    }
}

void bap_unicast_stop_timer(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    PROTOCOL_PRINT_INFO0("bap_unicast_stop_timer");
    if (p_audio_session->p_session_timer_handle)
    {
        sys_timer_stop(p_audio_session->p_session_timer_handle);
    }
}

void bap_unicast_del_cis_disc_timer(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    if (p_audio_session->p_cis_disc_timer_handle)
    {
        sys_timer_delete(p_audio_session->p_cis_disc_timer_handle);
        p_audio_session->p_cis_disc_timer_handle = NULL;
    }
}

static void bap_unicast_cis_disc_timeout(T_SYS_TIMER_HANDLE handle)
{
    uint32_t timer_id = sys_timer_id_get(handle);
    T_AUDIO_STREAM_SESSION *p_audio_session;
    p_audio_session = (T_AUDIO_STREAM_SESSION *)timer_id;
    PROTOCOL_PRINT_INFO1("bap_unicast_cis_disc_timeout: p_audio_session %p", p_audio_session);
    if (audio_stream_session_check(p_audio_session))
    {
        if (p_audio_session->state != AUDIO_STREAM_STATE_CONFIGURED)
        {
            return;
        }
        for (uint8_t i = 0; i < p_audio_session->server_num; i++)
        {
            T_ISOCH_INFO isoch_info;
            T_AUDIO_STREAM_SERVER *p_srv = &p_audio_session->server[i];
            for (uint8_t j = 0; j < p_srv->cis_num; j++)
            {
                if (p_srv->cis_info[j].cis_id == 0)
                {
                    continue;
                }
                if (cig_mgr_get_isoch_info(p_srv->cis_info[j].cis_conn_handle, &isoch_info) == false)
                {
                    continue;
                }
                else
                {
                    if (isoch_info.state == ISOCH_STATE_CONN)
                    {
                        cig_mgr_disconnect(p_srv->cis_info[j].cis_conn_handle, HCI_ERR_REMOTE_USER_TERMINATE);
                    }
                }
            }
        }
        bap_unicast_del_cis_disc_timer(p_audio_session);
    }
}

void bap_unicast_start_cis_disc_timer(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    if (p_audio_session->p_cis_disc_timer_handle == NULL)
    {
        p_audio_session->p_cis_disc_timer_handle = sys_timer_create("cis_disc_timer",
                                                                    SYS_TIMER_TYPE_LOW_PRECISION,
                                                                    (uint32_t)p_audio_session,
                                                                    p_audio_session->cis_timeout_ms * 1000, false, bap_unicast_cis_disc_timeout);
        if (p_audio_session->p_cis_disc_timer_handle)
        {
            sys_timer_start(p_audio_session->p_cis_disc_timer_handle);
        }
    }
    else
    {
        sys_timer_restart(p_audio_session->p_cis_disc_timer_handle, p_audio_session->cis_timeout_ms * 1000);
    }
}

bool bap_unicast_check_codec_cfg(T_ASE_DATA_CODEC_CONFIGURED *p_codec_data, T_CODEC_CFG *p_cfg)
{
    T_CODEC_CFG ase_codec_cfg;

    if (codec_cfg_parse(p_codec_data->data.codec_spec_cfg_len, p_codec_data->p_codec_spec_cfg,
                        &ase_codec_cfg))
    {
        if ((p_cfg->type_exist & CODEC_CFG_FRAME_DURATION_EXIST) &&
            (ase_codec_cfg.frame_duration != p_cfg->frame_duration))
        {
            return true;
        }

        if (ase_codec_cfg.sample_frequency != p_cfg->sample_frequency)
        {
            return true;
        }

        if ((p_cfg->type_exist & CODEC_CFG_TYPE_BLOCKS_PER_SDU_EXIST) &&
            (ase_codec_cfg.codec_frame_blocks_per_sdu != p_cfg->codec_frame_blocks_per_sdu))
        {
            return true;
        }

        if ((p_cfg->type_exist & CODEC_CFG_OCTET_PER_CODEC_FRAME_EXIST) &&
            (ase_codec_cfg.octets_per_codec_frame != p_cfg->octets_per_codec_frame))
        {
            return true;
        }

        if ((p_cfg->type_exist & CODEC_CFG_AUDIO_CHANNEL_ALLOCATION_EXIST) &&
            (ase_codec_cfg.audio_channel_allocation != p_cfg->audio_channel_allocation))
        {
            return true;
        }
    }
    return false;
}

bool bap_unicast_check_codec_cfg_cmpl(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    T_ASE_DATA_CODEC_CONFIGURED codec_data;
    bool need_update = false;

    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;


        if (bap_unicast_check_dev_state(p_dev, true) == false)
        {
            continue;
        }

        for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
        {
            T_BAP_ASE_INFO *p_ase_info = &p_audio_session->server[i].ase_info[j];

            if (p_ase_info->p_ase_entry == NULL)
            {
                continue;
            }

            if (p_ase_info->p_ase_entry->ase_state == ASE_STATE_CODEC_CONFIGURED)
            {
                if (ascs_client_get_codec_data(p_dev->p_link->conn_handle, p_ase_info->p_ase_entry->ase_id,
                                               &codec_data))
                {
                    if (memcmp(p_ase_info->ase_codec_cfg.codec_id, codec_data.data.codec_id, CODEC_ID_LEN) != 0)
                    {
                        need_update = true;
                        p_ase_info->codec_update = true;
                        continue;
                    }

                    if (bap_unicast_check_codec_cfg(&codec_data, &p_ase_info->ase_codec_cfg.codec_cfg))
                    {
                        need_update = true;
                        p_ase_info->codec_update = true;
                        continue;
                    }
                }
                p_ase_info->codec_update = false;
            }
        }
    }

    PROTOCOL_PRINT_INFO1("bap_unicast_check_codec_cfg_cmpl: need_update %d", need_update);

    return need_update;
}

bool bap_unicast_cp_config_codec(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    T_ASE_CP_CODEC_CFG_PARAM *p_codec = NULL;
    uint8_t err_idx = 0;
    uint8_t idx = 0;
    bool send_cp = false;
    T_BAP_ASE_ENTRY *ase_entry[AUDIO_STREAM_SRV_ASE_MAX];
    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;
        idx = 0;
        if (bap_unicast_check_dev_state(p_dev, true) == false)
        {
            continue;
        }
        p_codec = ble_audio_mem_zalloc(sizeof(T_ASE_CP_CODEC_CFG_PARAM) *
                                       p_audio_session->server[i].ase_num);
        if (p_codec == NULL)
        {
            err_idx = 1;
            goto failed;
        }

        for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
        {
            T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
            if (p_ase_entry == NULL)
            {
                err_idx = 2;
                goto failed;
            }
            PROTOCOL_PRINT_INFO5("bap_unicast_cp_config_codec: conn_handle 0x%x, ase_id %d, ase_state %d, pending_cp %d, codec_update %d",
                                 p_dev->p_link->conn_handle, p_ase_entry->ase_id, p_ase_entry->ase_state,
                                 p_ase_entry->pending_cp,
                                 p_audio_session->server[i].ase_info[j].codec_update);
            if (p_ase_entry->pending_cp == ASE_CP_OP_CONFIG_CODEC)
            {
                continue;
            }
            if (p_ase_entry->ase_state == ASE_STATE_IDLE ||
                (p_ase_entry->ase_state == ASE_STATE_CODEC_CONFIGURED &&
                 p_audio_session->server[i].ase_info[j].codec_update == true))
            {
                T_AUDIO_ASE_CODEC_CFG *p_codec_cfg;
                p_codec[idx].ase_id = p_ase_entry->ase_id;
                if (p_audio_session->server[i].ase_info[j].ase_codec_cfg.codec_cfg.type_exist)
                {
                    p_codec_cfg = &p_audio_session->server[i].ase_info[j].ase_codec_cfg;
                }
                else
                {
                    err_idx = 3;
                    goto failed;
                }
                p_codec[idx].target_latency = p_codec_cfg->target_latency;
                p_codec[idx].target_phy = p_codec_cfg->target_phy;
                memcpy(p_codec[idx].codec_id, p_codec_cfg->codec_id, CODEC_ID_LEN);
                if (codec_cfg_gen(&p_codec[idx].codec_spec_cfg_len, p_codec[idx].codec_spec_cfg,
                                  &p_codec_cfg->codec_cfg))
                {
                    if (p_codec_cfg->vendor_codec_cfg_len != 0 &&
                        (p_codec[idx].codec_spec_cfg_len + p_codec_cfg->vendor_codec_cfg_len) <= CODEC_DATA_MAX_LEN)
                    {
                        memcpy(&p_codec[idx].codec_spec_cfg[p_codec[idx].codec_spec_cfg_len],
                               p_codec_cfg->vendor_codec_cfg, p_codec_cfg->vendor_codec_cfg_len);
                        p_codec[idx].codec_spec_cfg_len += p_codec_cfg->vendor_codec_cfg_len;
                    }
                    ase_entry[idx] = p_ase_entry;
                    idx++;
                    continue;
                }
                err_idx = 4;
                goto failed;
            }
        }
        if (idx)
        {
            if (ase_cp_config_codec(p_dev->p_link->conn_handle, idx, p_codec) == false)
            {
                err_idx = 5;
                goto failed;
            }
            else
            {
                send_cp = true;
                for (uint8_t k = 0; k < idx; k++)
                {
                    ase_entry[k]->pending_cp = ASE_CP_OP_CONFIG_CODEC;
                    ase_entry[k]->wait_cp = true;
                }
            }
        }
        if (p_codec)
        {
            ble_audio_mem_free(p_codec);
            p_codec = NULL;
        }
    }
    p_audio_session->prefer_state = ASE_STATE_CODEC_CONFIGURED;
    if (send_cp)
    {
        bap_unicast_start_timer(p_audio_session);
    }
    return true;
failed:
    if (p_codec)
    {
        ble_audio_mem_free(p_codec);
    }
    PROTOCOL_PRINT_ERROR1("bap_unicast_cp_config_codec: failed, err_idx %d", err_idx);
    return false;
}

T_CIS_CFG_PARAM *bap_unicast_find_cis_param(T_AUDIO_STREAM_SERVER *p_srv, uint8_t cis_id)
{
    for (uint8_t i = 0; i < p_srv->cis_num; i++)
    {
        if (p_srv->cis_info[i].cis_id == cis_id && cis_id != 0)
        {
            return &p_srv->cis_info[i].cis_param;
        }
    }
    return NULL;
}

bool bap_unicast_cp_config_qos(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    T_ASE_CP_QOS_CFG_ARRAY_PARAM *p_param = NULL;
    uint8_t idx = 0;
    uint8_t err_idx = 0;
    bool send_cp = false;
    T_BAP_ASE_ENTRY *ase_entry[AUDIO_STREAM_SRV_ASE_MAX];
    PROTOCOL_PRINT_TRACE1("bap_unicast_cp_config_qos: server_num %d", p_audio_session->server_num);
    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        T_AUDIO_STREAM_SERVER *p_srv = &p_audio_session->server[i];
        T_CIS_CFG_PARAM *p_cis_param;
        T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;
        idx = 0;
        if (bap_unicast_check_dev_state(p_dev, true) == false)
        {
            continue;
        }
        p_param = ble_audio_mem_zalloc(sizeof(T_ASE_CP_QOS_CFG_ARRAY_PARAM) *
                                       p_audio_session->server[i].ase_num);
        if (p_param == NULL)
        {
            err_idx = 1;
            goto failed;
        }
        for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
        {
            T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
            if (p_ase_entry == NULL)
            {
                PROTOCOL_PRINT_ERROR0("bap_unicast_cp_config_qos: p_ase_entry == NULL");
                goto failed;
            }
            if (p_ase_entry->ase_state == ASE_STATE_CODEC_CONFIGURED &&
                p_ase_entry->pending_cp != ASE_CP_OP_CONFIG_QOS)
            {
                p_param[idx].ase_id = p_ase_entry->ase_id;
                p_param[idx].cig_id = p_audio_session->cig_id;
                p_cis_param = bap_unicast_find_cis_param(p_srv, p_srv->ase_info[j].cis_id);
                if (p_cis_param == NULL)
                {
                    err_idx = 2;
                    goto failed;
                }
                p_param[idx].cis_id = p_audio_session->server[i].ase_info[j].cis_id;
                p_param[idx].framing = p_audio_session->cig_param.framing;

                if (p_srv->ase_info[j].direction == SERVER_AUDIO_SINK)
                {
                    LE_UINT24_TO_ARRAY(p_param[idx].sdu_interval, p_audio_session->cig_param.sdu_interval_m_s);
                    LE_UINT16_TO_ARRAY(p_param[idx].max_transport_latency, p_audio_session->cig_param.latency_m_s);
                    p_param[idx].phy = p_cis_param->phy_m_s;
                    LE_UINT16_TO_ARRAY(p_param[idx].max_sdu, p_cis_param->max_sdu_m_s);
                    p_param[idx].retransmission_number = p_cis_param->rtn_m_s;
                    if (p_audio_session->session_qos.sink_presentation_delay)
                    {
                        LE_UINT24_TO_ARRAY(p_param[idx].presentation_delay,
                                           p_audio_session->session_qos.sink_presentation_delay);
                    }
                    else
                    {
                        LE_UINT24_TO_ARRAY(p_param[idx].presentation_delay,
                                           p_audio_session->session_qos.source_presentation_delay);
                    }
                }
                else
                {
                    LE_UINT24_TO_ARRAY(p_param[idx].sdu_interval, p_audio_session->cig_param.sdu_interval_s_m);
                    LE_UINT16_TO_ARRAY(p_param[idx].max_transport_latency, p_audio_session->cig_param.latency_s_m);
                    p_param[idx].phy = p_cis_param->phy_s_m;
                    LE_UINT16_TO_ARRAY(p_param[idx].max_sdu, p_cis_param->max_sdu_s_m);
                    p_param[idx].retransmission_number = p_cis_param->rtn_s_m;
                    if (p_audio_session->session_qos.source_presentation_delay)
                    {
                        LE_UINT24_TO_ARRAY(p_param[idx].presentation_delay,
                                           p_audio_session->session_qos.source_presentation_delay);
                    }
                    else
                    {
                        LE_UINT24_TO_ARRAY(p_param[idx].presentation_delay,
                                           p_audio_session->session_qos.sink_presentation_delay);
                    }
                }
                ase_entry[idx] = p_ase_entry;
                idx++;
            }
        }
        if (idx)
        {
            if (ase_cp_config_qos(p_dev->p_link->conn_handle, idx, p_param) == false)
            {
                err_idx = 3;
                goto failed;
            }
            else
            {
                send_cp = true;
                for (uint8_t k = 0; k < idx; k++)
                {
                    ase_entry[k]->pending_cp = ASE_CP_OP_CONFIG_QOS;
                    ase_entry[k]->wait_cp = true;
                }
            }
        }
        if (p_param)
        {
            ble_audio_mem_free(p_param);
            p_param = NULL;
        }
    }
    if (send_cp)
    {
        p_audio_session->prefer_state = ASE_STATE_QOS_CONFIGURED;
        bap_unicast_start_timer(p_audio_session);
    }
    return true;
failed:
    if (p_param)
    {
        ble_audio_mem_free(p_param);
        p_param = NULL;
    }
    PROTOCOL_PRINT_ERROR1("bap_unicast_cp_config_qos: failed, err_idx %d", err_idx);
    return false;
}

bool bap_unicast_cp_enable(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    T_ASE_CP_METADATA_PARAM *p_param = NULL;
    uint8_t idx = 0;
    bool send_cp = false;
    T_BAP_ASE_ENTRY *ase_entry[AUDIO_STREAM_SRV_ASE_MAX];
    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;
        idx = 0;
        if (bap_unicast_check_dev_state(p_dev, true) == false)
        {
            continue;
        }
        p_param = ble_audio_mem_zalloc(sizeof(T_ASE_CP_METADATA_PARAM) *
                                       p_audio_session->server[i].ase_num);
        if (p_param == NULL)
        {
            goto failed;
        }
        for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
        {
            T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
            if (p_ase_entry == NULL)
            {
                PROTOCOL_PRINT_ERROR0("bap_unicast_cp_enable: p_ase_entry == NULL");
                goto failed;
            }

            if (p_ase_entry->ase_state == ASE_STATE_QOS_CONFIGURED &&
                p_ase_entry->pending_cp != ASE_CP_OP_ENABLE)
            {
                bap_unicast_send_start_metadata_cfg(p_audio_session, p_audio_session->server[i].dev_handle,
                                                    p_ase_entry->ase_id, p_ase_entry->direction);
                p_param[idx].ase_id = p_ase_entry->ase_id;
                p_param[idx].metadata_len = p_audio_session->server[i].ase_info[j].metadata_len;
                p_param[idx].p_data = p_audio_session->server[i].ase_info[j].p_metadata;
                ase_entry[idx] = p_ase_entry;
                idx++;
            }
        }
        if (idx)
        {
            if (ase_cp_enable(p_dev->p_link->conn_handle, idx, p_param) == false)
            {
                goto failed;
            }
            else
            {
                send_cp = true;
                for (uint8_t k = 0; k < idx; k++)
                {
                    ase_entry[k]->pending_cp = ASE_CP_OP_ENABLE;
                    ase_entry[k]->wait_cp = true;
                }
            }
        }
        if (p_param)
        {
            ble_audio_mem_free(p_param);
            p_param = NULL;
        }
    }
    if (send_cp)
    {
        p_audio_session->prefer_state = ASE_STATE_ENABLING;
        bap_unicast_start_timer(p_audio_session);
    }
    return true;
failed:
    if (p_param)
    {
        ble_audio_mem_free(p_param);
        p_param = NULL;
    }
    PROTOCOL_PRINT_ERROR0("bap_unicast_cp_enable: failed");
    return false;
}

bool bap_unicast_cp_disable(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    uint8_t ase_id[10];
    uint8_t num_ases = 0;
    bool send_cp = false;
    T_BAP_ASE_ENTRY *ase_entry[AUDIO_STREAM_SRV_ASE_MAX];
    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;
        num_ases = 0;
        if (bap_unicast_check_dev_state(p_dev, true) == false)
        {
            continue;
        }
        for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
        {
            T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
            if (p_ase_entry->ase_state == ASE_STATE_ENABLING ||
                p_ase_entry->ase_state == ASE_STATE_STREAMING)
            {
                ase_id[num_ases] = p_ase_entry->ase_id;
                ase_entry[num_ases] = p_ase_entry;
                num_ases++;
            }
        }
        if (num_ases)
        {
            if (ase_cp_disable(p_dev->p_link->conn_handle, num_ases, ase_id) == false)
            {
                goto failed;
            }
            else
            {
                send_cp = true;
                for (uint8_t k = 0; k < num_ases; k++)
                {
                    ase_entry[k]->pending_cp = ASE_CP_OP_DISABLE;
                    ase_entry[k]->wait_cp = true;
                }
            }
        }
    }
    if (send_cp)
    {
        p_audio_session->prefer_state = ASE_STATE_QOS_CONFIGURED;
        bap_unicast_start_timer(p_audio_session);
    }
    return true;
failed:
    PROTOCOL_PRINT_ERROR0("bap_unicast_cp_disable: failed");
    return false;
}

bool bap_unicast_cp_release(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    uint8_t ase_id[10];
    uint8_t num_ases = 0;
    bool send_cp = false;
    T_BAP_ASE_ENTRY *ase_entry[AUDIO_STREAM_SRV_ASE_MAX];
    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;
        num_ases = 0;
        if (bap_unicast_check_dev_state(p_dev, true) == false)
        {
            continue;
        }
        for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
        {
            T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
            if (p_ase_entry->ase_state != ASE_STATE_IDLE &&
                p_ase_entry->ase_state != ASE_STATE_RELEASING)
            {
                ase_id[num_ases] = p_ase_entry->ase_id;
                ase_entry[num_ases] = p_ase_entry;
                num_ases++;
            }
        }
        if (num_ases)
        {
            if (ase_cp_release(p_dev->p_link->conn_handle, num_ases, ase_id) == false)
            {
                goto failed;
            }
            else
            {
                send_cp = true;
                for (uint8_t k = 0; k < num_ases; k++)
                {
                    ase_entry[k]->pending_cp = ASE_CP_OP_RELEASE;
                    ase_entry[k]->wait_cp = true;
                }
            }
        }
    }
    if (send_cp)
    {
        p_audio_session->prefer_state = ASE_STATE_RELEASING;
        bap_unicast_start_timer(p_audio_session);
    }
    return true;
failed:
    PROTOCOL_PRINT_ERROR0("bap_unicast_cp_release: failed");
    return false;
}

bool bap_unicast_cp_update_metadata(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    T_ASE_CP_METADATA_PARAM *p_param = NULL;
    uint8_t idx = 0;
    bool send_cp = false;
    T_BAP_ASE_ENTRY *ase_entry[AUDIO_STREAM_SRV_ASE_MAX];
    T_ASE_DATA_WITH_METADATA ase_metadata;

    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;
        idx = 0;
        if (bap_unicast_check_dev_state(p_dev, true) == false)
        {
            continue;
        }
        p_param = ble_audio_mem_zalloc(sizeof(T_ASE_CP_METADATA_PARAM) *
                                       p_audio_session->server[i].ase_num);
        if (p_param == NULL)
        {
            goto failed;
        }
        for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
        {
            T_BAP_ASE_INFO *p_ase_info = &p_audio_session->server[i].ase_info[j];
            if (p_ase_info->p_ase_entry == NULL)
            {
                goto failed;
            }
            PROTOCOL_PRINT_INFO3("bap_unicast_cp_update_metadata: conn_handle 0x%x, ase_state %d, pending_cp %d",
                                 p_dev->p_link->conn_handle, p_ase_info->p_ase_entry->ase_state,
                                 p_ase_info->p_ase_entry->pending_cp);
            if ((p_ase_info->p_ase_entry->ase_state == ASE_STATE_ENABLING ||
                 p_ase_info->p_ase_entry->ase_state == ASE_STATE_STREAMING) &&
                p_ase_info->p_ase_entry->pending_cp != ASE_CP_OP_UPDATE_METADATA)
            {
                if (ascs_client_get_metadata(p_dev->p_link->conn_handle, p_ase_info->p_ase_entry->ase_id,
                                             &ase_metadata) == false)
                {
                    goto failed;
                }
                if (p_ase_info->metadata_len == ase_metadata.metadata_length)
                {
                    if (p_ase_info->metadata_len == 0)
                    {
                        continue;
                    }
                    if (memcmp(p_ase_info->p_metadata, ase_metadata.p_metadata, p_ase_info->metadata_len) == 0)
                    {
                        continue;
                    }
                }
                p_param[idx].ase_id = p_ase_info->p_ase_entry->ase_id;
                p_param[idx].metadata_len = p_ase_info->metadata_len;
                p_param[idx].p_data = p_ase_info->p_metadata;
                ase_entry[idx] = p_ase_info->p_ase_entry;
                PROTOCOL_PRINT_INFO1("bap_unicast_cp_update_metadata: ase_id %d",
                                     p_ase_info->p_ase_entry->ase_id);
                idx++;
            }
            else if (p_ase_info->p_ase_entry->ase_state == ASE_STATE_QOS_CONFIGURED &&
                     p_ase_info->p_ase_entry->pending_cp == ASE_CP_OP_ENABLE)
            {
                p_audio_session->retry_update_metadata = true;
            }
        }
        if (idx)
        {
            if (ase_cp_update_metadata(p_dev->p_link->conn_handle, idx, p_param) == false)
            {
                goto failed;
            }
            else
            {
                send_cp = true;
                for (uint8_t k = 0; k < idx; k++)
                {
                    ase_entry[k]->pending_cp = ASE_CP_OP_UPDATE_METADATA;
                    ase_entry[k]->wait_cp = true;
                }
            }
        }
        if (p_param)
        {
            ble_audio_mem_free(p_param);
            p_param = NULL;
        }
    }
    if (send_cp)
    {
        bap_unicast_start_timer(p_audio_session);
    }
    return true;
failed:
    if (p_param)
    {
        ble_audio_mem_free(p_param);
        p_param = NULL;
    }
    PROTOCOL_PRINT_ERROR0("bap_unicast_cp_update_metadata: failed");
    return false;
}

bool bap_unicast_check_data_path(T_BAP_ASE_INFO *p_ase_info)
{
    uint16_t cis_conn_handle;
    if (cig_mgr_get_cis_handle(p_ase_info->cis_id, &cis_conn_handle))
    {
        T_ISOCH_INFO isoch_info;
        if (cig_mgr_get_isoch_info(cis_conn_handle, &isoch_info))
        {
            if (p_ase_info->direction == SERVER_AUDIO_SOURCE)
            {
                if (isoch_info.data_path_flags & DATA_PATH_OUTPUT_FLAG)
                {
                    return true;
                }
            }
            else
            {
                if (isoch_info.data_path_flags & DATA_PATH_INPUT_FLAG)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

T_AUDIO_STREAM_STATE bap_unicast_audio_get_new_state(T_AUDIO_STREAM_SESSION *p_audio_session,
                                                     T_BAP_UNICAST_RESULT result)
{
    T_AUDIO_STREAM_STATE new_state = AUDIO_STREAM_STATE_IDLE;
    uint8_t conn_dev = 0;
    T_ASE_STATE max_ase_state = ASE_STATE_UNKOWN;
    T_ASE_STATE min_ase_state = ASE_STATE_UNKOWN;
    for (uint8_t i = 0; i < p_audio_session->server_num; i++)
    {
        T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;

        if (bap_unicast_check_dev_state(p_dev, true) == false)
        {
            continue;
        }
        conn_dev++;
        for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
        {
            T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
            if (p_ase_entry->ase_state == ASE_STATE_DISABLING)
            {
                new_state = AUDIO_STREAM_STATE_STOPPING;
                break;
            }
            else if (p_ase_entry->ase_state == ASE_STATE_RELEASING)
            {
                new_state = AUDIO_STREAM_STATE_RELEASING;
                break;
            }
            else if (p_ase_entry->ase_state == ASE_STATE_ENABLING)
            {
                new_state = AUDIO_STREAM_STATE_STARTING;
                break;
            }
            if (max_ase_state == ASE_STATE_UNKOWN || max_ase_state < p_ase_entry->ase_state)
            {
                max_ase_state = p_ase_entry->ase_state;
            }
            if (min_ase_state == ASE_STATE_UNKOWN || min_ase_state > p_ase_entry->ase_state)
            {
                min_ase_state = p_ase_entry->ase_state;
            }
        }
        if (new_state != AUDIO_STREAM_STATE_IDLE)
        {
            PROTOCOL_PRINT_INFO1("bap_unicast_audio_get_new_state: temp new_state %d ", new_state);
            break;
        }
    }
    if (new_state == AUDIO_STREAM_STATE_IDLE)
    {
        if (max_ase_state == ASE_STATE_STREAMING)
        {
            if (min_ase_state == ASE_STATE_STREAMING)
            {
                if (conn_dev < p_audio_session->server_num)
                {
                    new_state = AUDIO_STREAM_STATE_PARTIAL_STREAMING;
                }
                else
                {
                    new_state = AUDIO_STREAM_STATE_STREAMING;
                }
            }
            else
            {
                new_state = AUDIO_STREAM_STATE_PARTIAL_STREAMING;
            }
        }
        else
        {
            if (max_ase_state == ASE_STATE_QOS_CONFIGURED)
            {
                new_state = AUDIO_STREAM_STATE_CONFIGURED;
            }
            else
            {
                if (p_audio_session->cig_state == T_CIG_SETTING_STATE_IDLE)
                {
                    if (p_audio_session->cfg_type == UNICAST_AUDIO_CFG_UNKNOWN)
                    {
                        new_state = AUDIO_STREAM_STATE_IDLE;
                    }
                    else
                    {
                        new_state = AUDIO_STREAM_STATE_IDLE_CONFIGURED;
                    }
                }
                else
                {
                    new_state = AUDIO_STREAM_STATE_CONFIGURED;
                }
            }
        }
    }
    if (result == BAP_UNICAST_RESULT_SUCCESS)
    {
        if (p_audio_session->state == AUDIO_STREAM_STATE_RELEASING)
        {
            if (new_state != AUDIO_STREAM_STATE_IDLE &&
                new_state != AUDIO_STREAM_STATE_IDLE_CONFIGURED)
            {
                new_state = AUDIO_STREAM_STATE_RELEASING;
            }
        }
        else if (p_audio_session->state == AUDIO_STREAM_STATE_STARTING)
        {
            if (new_state != AUDIO_STREAM_STATE_PARTIAL_STREAMING &&
                new_state != AUDIO_STREAM_STATE_RELEASING &&
                new_state != AUDIO_STREAM_STATE_STOPPING &&
                new_state != AUDIO_STREAM_STATE_STREAMING)
            {
                new_state = AUDIO_STREAM_STATE_STARTING;
            }
        }
        else if (p_audio_session->state == AUDIO_STREAM_STATE_STOPPING)
        {
            if (new_state > AUDIO_STREAM_STATE_CONFIGURED)
            {
                new_state = AUDIO_STREAM_STATE_STOPPING;
            }
        }
    }
    else
    {
        if (p_audio_session->state == AUDIO_STREAM_STATE_STARTING)
        {
            if (conn_dev != 0)
            {
                for (uint8_t i = 0; i < p_audio_session->server_num; i++)
                {
                    T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;

                    if (bap_unicast_check_dev_state(p_dev, true) == false)
                    {
                        continue;
                    }
                    for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
                    {
                        T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
                        if (p_ase_entry->ase_state == ASE_STATE_ENABLING ||
                            p_ase_entry->ase_state == ASE_STATE_STREAMING)
                        {
                            PROTOCOL_PRINT_ERROR0("bap_unicast_audio_get_new_state: invalid state, need disable");
                            bap_unicast_cp_disable(p_audio_session);
                            break;
                        }
                    }
                }
            }
            if (p_audio_session->cig_state == T_CIG_SETTING_STATE_IDLE)
            {
                if (p_audio_session->cfg_type == UNICAST_AUDIO_CFG_UNKNOWN)
                {
                    new_state = AUDIO_STREAM_STATE_IDLE;
                }
                else
                {
                    new_state = AUDIO_STREAM_STATE_IDLE_CONFIGURED;
                }
            }
            else
            {
                new_state = AUDIO_STREAM_STATE_CONFIGURED;
            }
        }
    }

    PROTOCOL_PRINT_INFO5("bap_unicast_audio_get_new_state: result %d, new_state (%d ->%d), min_ase_state %d, max_ase_state %d",
                         result, p_audio_session->state, new_state, min_ase_state, max_ase_state);
    return new_state;
}

void bap_unicast_handle_cp_error(T_AUDIO_STREAM_SESSION *p_audio_session, uint8_t *p_event_param)
{
    T_AUDIO_GROUP_ADDL_INFO addl_info = {0};
    T_AUDIO_STREAM_STATE new_state = bap_unicast_audio_get_new_state(p_audio_session,
                                                                     BAP_UNICAST_RESULT_ASE_CP_ERR);

    p_audio_session->retry_num = 0;
    if (p_event_param != NULL)
    {
        T_ASCS_CLIENT_CP_NOTIFY *p_cp_notify = (T_ASCS_CLIENT_CP_NOTIFY *)p_event_param;
        addl_info.cp_error.conn_handle = p_cp_notify->conn_handle;
        addl_info.cp_error.opcode = p_cp_notify->opcode;
        if (p_cp_notify->p_param)
        {
            for (uint8_t i = 0; i < p_cp_notify->number_of_ase; i++)
            {
                if (p_cp_notify->p_param[i].response_code != ASE_CP_RESP_SUCCESS)
                {
                    addl_info.cp_error.ase_id = p_cp_notify->p_param[i].ase_id;
                    addl_info.cp_error.response_code = p_cp_notify->p_param[i].response_code;
                    addl_info.cp_error.reason = p_cp_notify->p_param[i].reason;
                    break;
                }
            }
        }
    }
    p_audio_session->retry_num = 0;
    bap_unicast_send_bap_info(p_audio_session, new_state, BAP_UNICAST_RESULT_ASE_CP_ERR, 0,
                              &addl_info);
}

bool bap_unicast_audio_handle_starting(T_AUDIO_STREAM_SESSION *p_audio_session,
                                       T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                       T_AUDIO_STREAM_EVENT event, uint8_t *p_event_param)
{
    T_BAP_UNICAST_RESULT result = BAP_UNICAST_RESULT_SUCCESS;
    uint16_t cause = 0;
    bool ret = true;
    PROTOCOL_PRINT_INFO3("bap_unicast_audio_handle_starting: event %d, prefer_state %d, cig_state %d",
                         event,
                         p_audio_session->prefer_state,
                         p_audio_session->cig_state);
    switch (event)
    {
    case AUDIO_STREAM_EVENT_START_ACTION:
        {
            T_ASE_STATE curr_ase_state = ASE_STATE_UNKOWN;
            for (uint8_t i = 0; i < p_audio_session->server_num; i++)
            {
                T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;
                if (bap_unicast_check_dev_state(p_dev, true) == false)
                {
                    continue;
                }

                for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
                {
                    T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
                    if (p_ase_entry == NULL)
                    {
                        ret = false;
                        break;
                    }
                    if (p_ase_entry->ase_state > ASE_STATE_STREAMING)
                    {
                        ret = false;
                        break;
                    }
                    if (curr_ase_state == ASE_STATE_UNKOWN || curr_ase_state > p_ase_entry->ase_state)
                    {
                        curr_ase_state = p_ase_entry->ase_state;
                    }
                }
            }

            if (curr_ase_state != ASE_STATE_UNKOWN && ret)
            {
                PROTOCOL_PRINT_INFO1("bap_unicast_audio_handle_starting: start action, curr_ase_state %d",
                                     curr_ase_state);
                if (curr_ase_state == ASE_STATE_IDLE)
                {
                    bap_unicast_check_codec_cfg_cmpl(p_audio_session);
                    ret = bap_unicast_cp_config_codec(p_audio_session);
                }
                else if (curr_ase_state == ASE_STATE_CODEC_CONFIGURED)
                {
                    if (bap_unicast_check_codec_cfg_cmpl(p_audio_session) == false)
                    {
                        if (p_audio_session->cig_state == T_CIG_SETTING_STATE_IDLE)
                        {
                            if (bap_unicast_gen_qos_param(p_audio_session) == false)
                            {
                                result = BAP_UNICAST_RESULT_FAILED;
                                goto failed;
                            }

                            ret = bap_unicast_config_cig(p_audio_session);
                        }
                        else if (p_audio_session->cig_state == T_CIG_SETTING_STATE_CONFIGURED)
                        {
                            ret = bap_unicast_cp_config_qos(p_audio_session);
                        }
                    }
                    else
                    {
                        ret = bap_unicast_cp_config_codec(p_audio_session);
                    }
                }
                else if (curr_ase_state == ASE_STATE_QOS_CONFIGURED)
                {
                    ret = bap_unicast_cp_enable(p_audio_session);
                }
                if (ret)
                {
                    bap_unicast_send_bap_info(p_audio_session, AUDIO_STREAM_STATE_STARTING,
                                              BAP_UNICAST_RESULT_SUCCESS, GAP_SUCCESS, NULL);
                }
            }
        }
        break;

    case AUDIO_STREAM_EVENT_CIS_DISCONN:
        {
            T_CIG_MGR_DISCONNECT_INFO *p_info = (T_CIG_MGR_DISCONNECT_INFO *)p_event_param;
            if (p_info)
            {
                PROTOCOL_PRINT_ERROR1("bap_unicast_audio_handle_starting: AUDIO_STREAM_EVENT_CIS_DISCONN, cause 0x%x",
                                      p_info->cause);
                if (p_info->cause == (HCI_ERR | HCI_ERR_UNKNOWN_CONN_ID))
                {
                    if (p_audio_session->retry_num < AUDIO_ACTION_RETRY_MAX_NUM)
                    {
                        if (bap_unicast_create_cis(p_audio_session))
                        {
                            p_audio_session->retry_num++;
                            p_audio_session->prefer_state = ASE_STATE_STREAMING;
                            break;
                        }
                    }
                }
                cause = p_info->cause;
                result = BAP_UNICAST_RESULT_CIS_DISCONN;
                goto failed;
            }
        }
        break;

    case AUDIO_STREAM_EVENT_ASE_STATE_CHANGE:
    case AUDIO_STREAM_EVENT_DATA_PATH_ADD:
    case AUDIO_STREAM_EVENT_READ_ASE_STATE:
        {
            bool is_complete = true;
            bool disc_conn_dev = false;
            uint8_t conn_dev_num = 0;

            for (uint8_t i = 0; i < p_audio_session->server_num; i++)
            {
                T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;
                if (bap_unicast_check_dev_state(p_dev, true) == false)
                {
                    disc_conn_dev = true;
                    continue;
                }
                conn_dev_num++;

                for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
                {
                    T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
                    if (p_ase_entry == NULL)
                    {
                        ret = false;
                        break;
                    }
                    if (p_ase_entry->ase_state > ASE_STATE_CODEC_CONFIGURED &&
                        p_audio_session->server[i].ase_info[j].cis_id == 0)
                    {
                        ret = false;
                        break;
                    }
                    if (p_ase_entry->ase_state == ASE_STATE_DISABLING ||
                        p_ase_entry->ase_state == ASE_STATE_RELEASING)
                    {
                        result = BAP_UNICAST_RESULT_ASE_INVALID_STATE;
                        goto failed;
                    }
                    if (p_ase_entry->ase_state < p_audio_session->prefer_state)
                    {
                        is_complete = false;
                        break;
                    }
                    else
                    {
                        if (p_ase_entry->ase_state == ASE_STATE_STREAMING)
                        {
                            if (bap_unicast_check_data_path(&p_audio_session->server[i].ase_info[j]) == false)
                            {
                                is_complete = false;
                                break;
                            }
                        }
                    }
                }
            }

            if (conn_dev_num == 0)
            {
                PROTOCOL_PRINT_ERROR1("bap_unicast_audio_handle_starting: event %d, no link",
                                      event);
                result = BAP_UNICAST_RESULT_ACL_DISCONN;
                goto failed;
            }

            if (is_complete)
            {
                if (p_audio_session->prefer_state == ASE_STATE_STREAMING)
                {
                    if (ret == true)
                    {
                        p_audio_session->retry_num = 0;
                        if (disc_conn_dev)
                        {
                            bap_unicast_send_bap_info(p_audio_session, AUDIO_STREAM_STATE_PARTIAL_STREAMING,
                                                      BAP_UNICAST_RESULT_SUCCESS, GAP_SUCCESS, NULL);
                        }
                        else
                        {
                            bap_unicast_send_bap_info(p_audio_session, AUDIO_STREAM_STATE_STREAMING,
                                                      BAP_UNICAST_RESULT_SUCCESS, GAP_SUCCESS, NULL);
                        }
                    }
                }
                else if (p_audio_session->prefer_state == ASE_STATE_CODEC_CONFIGURED)
                {
                    if (bap_unicast_check_codec_cfg_cmpl(p_audio_session) == false)
                    {
                        if (p_audio_session->cig_state == T_CIG_SETTING_STATE_IDLE)
                        {
                            if (bap_unicast_gen_qos_param(p_audio_session) == false)
                            {
                                result = BAP_UNICAST_RESULT_FAILED;
                                goto failed;
                            }
                            bap_unicast_config_cig(p_audio_session);
                        }
                        else if (p_audio_session->cig_state == T_CIG_SETTING_STATE_CONFIGURED)
                        {
                            bap_unicast_cp_config_qos(p_audio_session);
                        }
                    }
                    else
                    {
                        ret = bap_unicast_cp_config_codec(p_audio_session);
                    }
                }
                else if (p_audio_session->prefer_state == ASE_STATE_QOS_CONFIGURED)
                {
                    bap_unicast_cp_enable(p_audio_session);
                }
                else if (p_audio_session->prefer_state == ASE_STATE_ENABLING)
                {
                    if (bap_unicast_create_cis(p_audio_session))
                    {
                        p_audio_session->prefer_state = ASE_STATE_STREAMING;
                    }
                }
            }
            else
            {
                if (p_audio_session->prefer_state == ASE_STATE_CODEC_CONFIGURED)
                {
                    bap_unicast_check_codec_cfg_cmpl(p_audio_session);
                    ret = bap_unicast_cp_config_codec(p_audio_session);
                }
                else if (p_audio_session->prefer_state == ASE_STATE_QOS_CONFIGURED)
                {
                    if (p_audio_session->cig_state == T_CIG_SETTING_STATE_IDLE)
                    {
                        if (bap_unicast_gen_qos_param(p_audio_session) == false)
                        {
                            result = BAP_UNICAST_RESULT_FAILED;
                            goto failed;
                        }
                        ret = bap_unicast_config_cig(p_audio_session);
                    }
                    else if (p_audio_session->cig_state == T_CIG_SETTING_STATE_CONFIGURED)
                    {
                        ret = bap_unicast_cp_config_qos(p_audio_session);
                    }
                }
                else if (p_audio_session->prefer_state == ASE_STATE_ENABLING)
                {
                    ret = bap_unicast_cp_enable(p_audio_session);
                }
            }
        }
        break;
    case AUDIO_STREAM_EVENT_CREATE_CIS:
        {
            T_CIG_MGR_CREATE_CIS_RSP *p_rsp = (T_CIG_MGR_CREATE_CIS_RSP *)p_event_param;
            if (p_rsp)
            {
                if (p_rsp->cause == (HCI_ERR | HCI_ERR_FAIL_TO_ESTABLISH_CONN))
                {
                    if (p_audio_session->retry_num < AUDIO_ACTION_RETRY_MAX_NUM)
                    {
                        if (bap_unicast_create_cis(p_audio_session))
                        {
                            p_audio_session->retry_num++;
                            break;
                        }
                    }
                    else
                    {
                    }
                }
                result = BAP_UNICAST_RESULT_CIS_ERR;
                cause = p_rsp->cause;
                goto failed;
            }
        }
        break;
    case AUDIO_STREAM_EVENT_ASE_CP_ERROR:
        {
            bap_unicast_handle_cp_error(p_audio_session, p_event_param);
            return false;
        }
    case AUDIO_STREAM_EVENT_CIG_SET_ERROR:
        {
            T_CIG_MGR_START_SETTING_RSP *p_rsp = (T_CIG_MGR_START_SETTING_RSP *)p_event_param;
            result = BAP_UNICAST_RESULT_CIG_SET_ERR;
            cause = p_rsp->cause;
            goto failed;
        }

    default:
        break;
    }
    return ret;
failed:
    {
        T_AUDIO_STREAM_STATE new_state = bap_unicast_audio_get_new_state(p_audio_session, result);
        p_audio_session->retry_num = 0;
        bap_unicast_send_bap_info(p_audio_session, new_state, result, cause, NULL);
    }
    return false;
}

bool bap_unicast_audio_handle_stopping(T_AUDIO_STREAM_SESSION *p_audio_session,
                                       T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                       T_AUDIO_STREAM_EVENT event, uint8_t *p_event_param)
{
    bool ret = true;
    switch (event)
    {
    case AUDIO_STREAM_EVENT_STOP_ACTION:
        {
            T_ASE_STATE curr_ase_state = ASE_STATE_UNKOWN;
            for (uint8_t i = 0; i < p_audio_session->server_num; i++)
            {
                T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;
                if (bap_unicast_check_dev_state(p_dev, true) == false)
                {
                    if (curr_ase_state == ASE_STATE_UNKOWN)
                    {
                        curr_ase_state = ASE_STATE_IDLE;
                    }
                    continue;
                }
                for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
                {
                    T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
                    if (p_ase_entry == NULL)
                    {
                        ret = false;
                        break;
                    }
                    if (p_ase_entry->ase_state == ASE_STATE_RELEASING ||
                        p_ase_entry->ase_state == ASE_STATE_DISABLING)
                    {
                        continue;
                    }
                    if (curr_ase_state == ASE_STATE_UNKOWN || curr_ase_state < p_ase_entry->ase_state)
                    {
                        curr_ase_state = p_ase_entry->ase_state;
                    }
                }
            }
            if (ret)
            {
                PROTOCOL_PRINT_INFO1("bap_unicast_audio_handle_stopping: start action, curr_ase_state %d",
                                     curr_ase_state);
                if (curr_ase_state == ASE_STATE_STREAMING ||
                    curr_ase_state == ASE_STATE_ENABLING)
                {
                    ret = bap_unicast_cp_disable(p_audio_session);
                }
                if (ret)
                {
                    bap_unicast_send_bap_info(p_audio_session, AUDIO_STREAM_STATE_STOPPING,
                                              BAP_UNICAST_RESULT_SUCCESS, GAP_SUCCESS, NULL);
                }
            }
        }
        break;
    case AUDIO_STREAM_EVENT_ASE_STATE_CHANGE:
    case AUDIO_STREAM_EVENT_READ_ASE_STATE:
        {
            bool is_complete = true;
            for (uint8_t i = 0; i < p_audio_session->server_num; i++)
            {
                T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;

                if (bap_unicast_check_dev_state(p_dev, true) == false)
                {
                    continue;
                }
                for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
                {
                    T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
                    if (p_ase_entry == NULL)
                    {
                        ret = false;
                        break;
                    }
                    if (p_ase_entry->ase_state > ASE_STATE_QOS_CONFIGURED)
                    {
                        is_complete = false;
                        break;
                    }
                }
            }

            if (is_complete)
            {
                if (p_audio_session->cis_timeout_ms)
                {
                    bap_unicast_start_cis_disc_timer(p_audio_session);
                }
                p_audio_session->retry_num = 0;
                bap_unicast_send_bap_info(p_audio_session, AUDIO_STREAM_STATE_CONFIGURED,
                                          BAP_UNICAST_RESULT_SUCCESS, GAP_SUCCESS, NULL);
            }
        }
        break;

    case AUDIO_STREAM_EVENT_ASE_CP_ERROR:
        {
            bap_unicast_handle_cp_error(p_audio_session, p_event_param);
        }
        break;
    default:
        break;
    }
    return ret;
}

bool bap_unicast_audio_handle_releasing(T_AUDIO_STREAM_SESSION *p_audio_session,
                                        T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                        T_AUDIO_STREAM_EVENT event, uint8_t *p_event_param)
{
    bool ret = true;
    switch (event)
    {
    case AUDIO_STREAM_EVENT_RELEASE_ACTION:
        {
            T_ASE_STATE curr_ase_state = ASE_STATE_UNKOWN;
            T_ASE_CP_OP pending_cp = ASE_CP_OP_NONE;
            for (uint8_t i = 0; i < p_audio_session->server_num; i++)
            {
                T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;

                if (bap_unicast_check_dev_state(p_dev, true) == false)
                {
                    if (curr_ase_state == ASE_STATE_UNKOWN)
                    {
                        curr_ase_state = ASE_STATE_IDLE;
                    }
                    continue;
                }
                for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
                {
                    T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
                    if (p_ase_entry == NULL)
                    {
                        if (curr_ase_state == ASE_STATE_UNKOWN)
                        {
                            curr_ase_state = ASE_STATE_IDLE;
                        }
                        continue;
                    }
                    if (p_ase_entry->ase_state == ASE_STATE_RELEASING ||
                        p_ase_entry->ase_state == ASE_STATE_DISABLING)
                    {
                        continue;
                    }

                    if (p_ase_entry->pending_cp == ASE_CP_OP_CONFIG_QOS)
                    {
                        pending_cp = ASE_CP_OP_CONFIG_QOS;
                    }
                    if (curr_ase_state == ASE_STATE_UNKOWN || curr_ase_state < p_ase_entry->ase_state)
                    {
                        curr_ase_state = p_ase_entry->ase_state;
                    }
                }
            }
            if (ret)
            {
                PROTOCOL_PRINT_INFO2("bap_unicast_audio_handle_releasing: start action, curr_ase_state %d, pending_cp %d",
                                     curr_ase_state, pending_cp);
                if ((curr_ase_state == ASE_STATE_IDLE ||
                     curr_ase_state == ASE_STATE_CODEC_CONFIGURED) && pending_cp != ASE_CP_OP_CONFIG_QOS)
                {
                    if (p_audio_session->cig_state == T_CIG_SETTING_STATE_CONFIGURED ||
                        p_audio_session->cig_state == T_CIG_SETTING_STATE_STARTING)
                    {
                        cig_mgr_remove_cig(p_audio_session->cig_id);
                    }
                    else
                    {
                        goto released;
                    }
                }
                else
                {
                    ret = bap_unicast_cp_release(p_audio_session);
                }
                if (ret)
                {
                    bap_unicast_send_bap_info(p_audio_session, AUDIO_STREAM_STATE_RELEASING,
                                              BAP_UNICAST_RESULT_SUCCESS, GAP_SUCCESS, NULL);
                }
            }
        }
        break;
    case AUDIO_STREAM_EVENT_ASE_STATE_CHANGE:
    case AUDIO_STREAM_EVENT_READ_ASE_STATE:
    case AUDIO_STREAM_EVENT_CREATE_CIS://Create cis failed
        {
            bool is_complete = true;
            for (uint8_t i = 0; i < p_audio_session->server_num; i++)
            {
                T_LE_AUDIO_DEV *p_dev = p_audio_session->server[i].dev_handle;

                if (bap_unicast_check_dev_state(p_dev, true) == false)
                {
                    continue;
                }
                for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
                {
                    T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
                    if (p_ase_entry == NULL)
                    {
                        ret = false;
                        break;
                    }
                    if (p_ase_entry->ase_state > ASE_STATE_CODEC_CONFIGURED)
                    {
                        is_complete = false;
                        break;
                    }
                }
            }

            PROTOCOL_PRINT_INFO2("bap_unicast_audio_handle_releasing: event %d, is_complete %d", event,
                                 is_complete);

            if (is_complete)
            {
                if (p_audio_session->cig_state == T_CIG_SETTING_STATE_CONFIGURED)
                {
                    cig_mgr_remove_cig(p_audio_session->cig_id);
                }
                else
                {
                    goto released;
                }
            }
        }
        break;

    case AUDIO_STREAM_EVENT_ASE_CP_ERROR:
        {
            bap_unicast_handle_cp_error(p_audio_session, p_event_param);
        }
        break;

    case AUDIO_STREAM_EVENT_CIG_REMOVE:
        {
            goto released;
        }
    default:
        break;
    }
    return ret;
released:
    p_audio_session->retry_num = 0;
    if (p_audio_session->req_action == BAP_UNICAST_ACTION_REMOVE_SESSION)
    {
        bap_unicast_send_bap_info(p_audio_session, AUDIO_STREAM_STATE_IDLE_CONFIGURED,
                                  BAP_UNICAST_RESULT_SUCCESS, GAP_SUCCESS, NULL);
        bap_unicast_send_session_remove(p_audio_session);
    }
    else
    {
        bap_unicast_send_bap_info(p_audio_session, AUDIO_STREAM_STATE_IDLE_CONFIGURED,
                                  BAP_UNICAST_RESULT_SUCCESS, GAP_SUCCESS, NULL);
    }
    return ret;
}

bool bap_unicast_audio_check_state(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                   T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                   T_AUDIO_STREAM_EVENT event, uint8_t *p_event_param)
{
    bool ret = true;
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;

    PROTOCOL_PRINT_INFO5("bap_unicast_audio_check_state: session %p, dev_handle %p, event %d, req_action %d, state 0x%x",
                         handle, dev_handle, event, p_audio_session->req_action, p_audio_session->state);
    if (p_audio_session->req_action == BAP_UNICAST_ACTION_SERVER_STOP)
    {
        if (event == AUDIO_STREAM_EVENT_ASE_STATE_CHANGE)
        {
            if (p_audio_session->state != AUDIO_STREAM_STATE_STOPPING)
            {
                T_AUDIO_STREAM_STATE new_state = bap_unicast_audio_get_new_state(p_audio_session,
                                                                                 BAP_UNICAST_RESULT_SUCCESS);
                bap_unicast_send_bap_info(p_audio_session, new_state, BAP_UNICAST_RESULT_SUCCESS, 0, NULL);
            }
        }
    }
    if (p_audio_session->req_action == BAP_UNICAST_ACTION_SERVER_RELEASE)
    {
        if (event == AUDIO_STREAM_EVENT_ASE_STATE_CHANGE)
        {
            if (p_audio_session->state != AUDIO_STREAM_STATE_RELEASING)
            {
                T_AUDIO_STREAM_STATE new_state = bap_unicast_audio_get_new_state(p_audio_session,
                                                                                 BAP_UNICAST_RESULT_SUCCESS);
                bap_unicast_send_bap_info(p_audio_session, new_state, BAP_UNICAST_RESULT_SUCCESS, 0, NULL);
                if (new_state == AUDIO_STREAM_STATE_RELEASING)
                {
                    event = AUDIO_STREAM_EVENT_RELEASE_ACTION;
                }
            }
        }
    }
    switch (p_audio_session->state)
    {
    case AUDIO_STREAM_STATE_IDLE_CONFIGURED:
    case AUDIO_STREAM_STATE_CONFIGURED:
        {
            if (event == AUDIO_STREAM_EVENT_START_ACTION)
            {
                ret = bap_unicast_audio_handle_starting(p_audio_session, dev_handle, event, p_event_param);
            }
            else if (event == AUDIO_STREAM_EVENT_RELEASE_ACTION)
            {
                ret = bap_unicast_audio_handle_releasing(p_audio_session, dev_handle, event, p_event_param);
            }
        }
        break;

    case AUDIO_STREAM_STATE_STARTING:
        {
            if (event == AUDIO_STREAM_EVENT_STOP_ACTION)
            {
                ret = bap_unicast_audio_handle_stopping(p_audio_session, dev_handle, event, p_event_param);

            }
            else  if (event == AUDIO_STREAM_EVENT_RELEASE_ACTION)
            {
                ret = bap_unicast_audio_handle_releasing(p_audio_session, dev_handle, event, p_event_param);
            }
            else
            {
                ret = bap_unicast_audio_handle_starting(p_audio_session, dev_handle, event, p_event_param);
            }
        }
        break;

    case AUDIO_STREAM_STATE_STREAMING:
        {
            if (event == AUDIO_STREAM_EVENT_STOP_ACTION)
            {
                ret = bap_unicast_audio_handle_stopping(p_audio_session, dev_handle, event, p_event_param);
            }
            else if (event == AUDIO_STREAM_EVENT_RELEASE_ACTION)
            {
                ret = bap_unicast_audio_handle_releasing(p_audio_session, dev_handle, event, p_event_param);
            }
        }
        break;

    case AUDIO_STREAM_STATE_PARTIAL_STREAMING:
        {
            if (event == AUDIO_STREAM_EVENT_START_ACTION)
            {
                ret = bap_unicast_audio_handle_starting(p_audio_session, dev_handle, event, p_event_param);
            }
            else if (event == AUDIO_STREAM_EVENT_STOP_ACTION)
            {
                ret = bap_unicast_audio_handle_stopping(p_audio_session, dev_handle, event, p_event_param);
            }
            else if (event == AUDIO_STREAM_EVENT_RELEASE_ACTION)
            {
                ret = bap_unicast_audio_handle_releasing(p_audio_session, dev_handle, event, p_event_param);
            }
        }
        break;

    case AUDIO_STREAM_STATE_STOPPING:
        {
            if (event == AUDIO_STREAM_EVENT_RELEASE_ACTION)
            {
                ret = bap_unicast_audio_handle_releasing(p_audio_session, NULL, event, p_event_param);
            }
            else
            {
                ret = bap_unicast_audio_handle_stopping(p_audio_session, NULL, event, p_event_param);
            }
        }
        break;

    case AUDIO_STREAM_STATE_RELEASING:
        {
            ret = bap_unicast_audio_handle_releasing(p_audio_session, NULL, event, p_event_param);
        }
        break;

    default:
        break;
    }
    return ret;
}

bool bap_unicast_audio_start(T_AUDIO_STREAM_SESSION_HANDLE handle)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    uint8_t err_idx = 0;
    T_BAP_UNICAST_ACTION old_action;

    PROTOCOL_PRINT_INFO1("bap_unicast_audio_start: handle %p", handle);
    if (audio_stream_session_check(handle) == false)
    {
        err_idx = 1;
        goto failed;
    }
    if (p_audio_session->state != AUDIO_STREAM_STATE_CONFIGURED &&
        p_audio_session->state != AUDIO_STREAM_STATE_IDLE_CONFIGURED &&
        p_audio_session->state != AUDIO_STREAM_STATE_PARTIAL_STREAMING)
    {
        err_idx = 2;
        goto failed;
    }
    if (bap_unicast_audio_assign_ase(p_audio_session) == false)
    {
        err_idx = 3;
        goto failed;
    }
    old_action = p_audio_session->req_action;
    p_audio_session->req_action = BAP_UNICAST_ACTION_START;

    if (bap_unicast_audio_check_state(handle, NULL, AUDIO_STREAM_EVENT_START_ACTION, NULL) == false)
    {
        p_audio_session->req_action = old_action;
        err_idx = 4;
        goto failed;
    }
    bap_unicast_del_cis_disc_timer(p_audio_session);
    return true;
failed:
    PROTOCOL_PRINT_ERROR2("bap_unicast_audio_start: failed, handle %p error index %d", handle, err_idx);
    //bap_unicast_client_print(p_audio_session);
    return false;
}
bool bap_unicast_audio_stop(T_AUDIO_STREAM_SESSION_HANDLE handle, uint32_t cis_timeout_ms)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    T_BAP_UNICAST_ACTION old_action;

    PROTOCOL_PRINT_INFO2("bap_unicast_audio_stop: handle %p, cis_timeout_ms %d", handle,
                         cis_timeout_ms);
    if (audio_stream_session_check(handle) == false)
    {
        goto failed;
    }
    if (p_audio_session->state != AUDIO_STREAM_STATE_STARTING &&
        p_audio_session->state != AUDIO_STREAM_STATE_STREAMING &&
        p_audio_session->state != AUDIO_STREAM_STATE_PARTIAL_STREAMING)
    {
        goto failed;
    }
    old_action = p_audio_session->req_action;
    p_audio_session->req_action = BAP_UNICAST_ACTION_STOP;
    p_audio_session->cis_timeout_ms = cis_timeout_ms;
    if (bap_unicast_audio_check_state(handle, NULL, AUDIO_STREAM_EVENT_STOP_ACTION, NULL) == false)
    {
        p_audio_session->req_action = old_action;
        goto failed;
    }

    return true;
failed:
    PROTOCOL_PRINT_ERROR1("bap_unicast_audio_stop: failed, handle %p", handle);
    return false;
}

bool bap_unicast_audio_release(T_AUDIO_STREAM_SESSION_HANDLE handle)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    T_BAP_UNICAST_ACTION old_action;

    PROTOCOL_PRINT_INFO1("bap_unicast_audio_release: handle %p", handle);
    if (audio_stream_session_check(handle) == false)
    {
        goto failed;
    }

    PROTOCOL_PRINT_INFO1("bap_unicast_audio_release: state 0x%x", p_audio_session->state);
    if (p_audio_session->state == AUDIO_STREAM_STATE_IDLE ||
        p_audio_session->state == AUDIO_STREAM_STATE_IDLE_CONFIGURED ||
        p_audio_session->state == AUDIO_STREAM_STATE_RELEASING)
    {
        goto failed;
    }
    old_action = p_audio_session->req_action;
    p_audio_session->req_action = BAP_UNICAST_ACTION_RELEASE;
    if (bap_unicast_audio_check_state(handle, NULL, AUDIO_STREAM_EVENT_RELEASE_ACTION, NULL) == false)
    {
        p_audio_session->req_action = old_action;
        goto failed;
    }

    return true;
failed:
    PROTOCOL_PRINT_ERROR1("bap_unicast_audio_release: failed, handle %p", handle);
    return false;
}

bool bap_unicast_audio_update(T_AUDIO_STREAM_SESSION_HANDLE handle)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    PROTOCOL_PRINT_INFO1("bap_unicast_audio_update: handle %p", handle);
    if (audio_stream_session_check(handle) == false)
    {
        goto failed;
    }
    if (p_audio_session->state != AUDIO_STREAM_STATE_STREAMING &&
        p_audio_session->state != AUDIO_STREAM_STATE_PARTIAL_STREAMING &&
        p_audio_session->state != AUDIO_STREAM_STATE_STARTING)
    {
        goto failed;
    }
    if (bap_unicast_cp_update_metadata(p_audio_session) == false)
    {
        goto failed;
    }

    return true;
failed:
    PROTOCOL_PRINT_ERROR1("bap_unicast_audio_update: failed, handle %p", handle);
    return false;
}

bool bap_unicast_audio_remove_cfg(T_AUDIO_STREAM_SESSION_HANDLE handle)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    PROTOCOL_PRINT_INFO1("bap_unicast_audio_remove_cfg: handle %p", handle);
    if (audio_stream_session_check(handle) == false)
    {
        goto failed;
    }
    if (p_audio_session->state == AUDIO_STREAM_STATE_IDLE)
    {
        return true;
    }
    if (p_audio_session->state != AUDIO_STREAM_STATE_IDLE_CONFIGURED)
    {
        goto failed;
    }
    if (p_audio_session->cig_state != T_CIG_SETTING_STATE_IDLE)
    {
        goto failed;
    }
    p_audio_session->req_action = BAP_UNICAST_ACTION_REMOVE_CFG;
    bap_unicast_audio_cfg_remove(handle);

    return true;
failed:
    PROTOCOL_PRINT_ERROR1("bap_unicast_audio_remove_cfg: failed, handle %p", handle);
    return false;
}

bool bap_unicast_audio_remove_session(T_AUDIO_STREAM_SESSION_HANDLE handle)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    PROTOCOL_PRINT_INFO1("bap_unicast_audio_remove_session: handle %p", handle);
    if (audio_stream_session_check(handle) == false)
    {
        goto failed;
    }
    if (p_audio_session->state == AUDIO_STREAM_STATE_IDLE ||
        p_audio_session->state == AUDIO_STREAM_STATE_IDLE_CONFIGURED)
    {
        if (bap_unicast_send_session_remove(p_audio_session))
        {
            return true;
        }
        else
        {
            goto failed;
        }
    }
    p_audio_session->req_action = BAP_UNICAST_ACTION_REMOVE_SESSION;
    if (bap_unicast_audio_check_state(handle, NULL, AUDIO_STREAM_EVENT_RELEASE_ACTION, NULL) == false)
    {
        goto failed;
    }

    return true;
failed:
    PROTOCOL_PRINT_ERROR1("bap_unicast_audio_remove_session: failed, handle %p", handle);
    return false;
}

void bap_unicast_client_start_int(T_AUDIO_STREAM_SESSION *p_audio_session)
{
    uint8_t err_idx = 0;
    PROTOCOL_PRINT_INFO1("bap_unicast_client_start_int: handle %p", p_audio_session);
    if (bap_unicast_audio_assign_ase(p_audio_session) == false)
    {
        err_idx = 1;
        goto failed;
    }
    p_audio_session->req_action = BAP_UNICAST_ACTION_START;

    if (bap_unicast_audio_check_state(p_audio_session, NULL, AUDIO_STREAM_EVENT_START_ACTION,
                                      NULL) == false)
    {
        err_idx = 2;
        goto failed;
    }
    return;
failed:
    PROTOCOL_PRINT_ERROR2("bap_unicast_client_start_int: failed, p_audio_session %p error index %d",
                          p_audio_session, err_idx);
}

void bap_unicast_client_check_session(uint16_t conn_handle)
{
    uint8_t i;
    T_AUDIO_STREAM_SESSION *p_audio_session;
    T_BLE_AUDIO_DEV_HANDLE dev_handle;
    for (i = 0; i < audio_session_queue.count; i++)
    {
        p_audio_session = (T_AUDIO_STREAM_SESSION *)os_queue_peek(&audio_session_queue, i);
        if (p_audio_session != NULL)
        {
            dev_handle = ble_audio_group_find_dev_by_conn_handle(p_audio_session->group_handle, conn_handle);
            if (dev_handle)
            {
                for (uint8_t k = 0; k < p_audio_session->server_num; k++)
                {
                    if (p_audio_session->server[k].dev_handle == dev_handle)
                    {
                        PROTOCOL_PRINT_INFO2("bap_unicast_client_check_session: p_audio_session %p, state %d",
                                             p_audio_session, p_audio_session->state);
                        if (p_audio_session->state == AUDIO_STREAM_STATE_STARTING ||
                            p_audio_session->state == AUDIO_STREAM_STATE_PARTIAL_STREAMING)
                        {
                            bap_unicast_client_start_int(p_audio_session);
                        }
                        break;
                    }
                }
            }
        }
    }
}

void bap_unicast_client_check_dev_add(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                                      T_BLE_AUDIO_DEV_HANDLE dev_handle)
{
    T_LE_AUDIO_DEV *p_dev = (T_LE_AUDIO_DEV *)dev_handle;
    T_BAP_LINK_CB *p_bap_cb = p_dev->p_link->p_bap_cb;
    uint8_t i;
    if (p_bap_cb != NULL)
    {
        if (p_bap_cb->disc_flags == bap_db.disc_flags &&
#if LE_AUDIO_PACS_CLIENT_SUPPORT
            p_bap_cb->pacs_cb.value_exist != 0 &&
#endif
            p_bap_cb->p_ascs_cb != NULL)
        {
            T_AUDIO_STREAM_SESSION *p_audio_session;
            for (i = 0; i < audio_session_queue.count; i++)
            {
                p_audio_session = (T_AUDIO_STREAM_SESSION *)os_queue_peek(&audio_session_queue, i);
                if (p_audio_session != NULL)
                {
                    if (p_audio_session->group_handle == group_handle)
                    {
                        for (uint8_t k = 0; k < p_audio_session->server_num; k++)
                        {
                            if (p_audio_session->server[k].dev_handle == p_dev)
                            {
                                PROTOCOL_PRINT_INFO2("bap_unicast_client_check_dev_add: p_audio_session %p, state %d",
                                                     p_audio_session, p_audio_session->state);
                                if (p_audio_session->state == AUDIO_STREAM_STATE_STARTING ||
                                    p_audio_session->state == AUDIO_STREAM_STATE_PARTIAL_STREAMING)
                                {
                                    bap_unicast_client_start_int(p_audio_session);
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void bap_unicast_client_handle_disconn_msg(T_BLE_AUDIO_LINK *p_link, uint16_t cause)
{
    uint8_t i;
    T_AUDIO_STREAM_SESSION *p_audio_session;
    T_BLE_AUDIO_DEV_HANDLE dev_handle;
    for (i = 0; i < audio_session_queue.count; i++)
    {
        p_audio_session = (T_AUDIO_STREAM_SESSION *)os_queue_peek(&audio_session_queue, i);
        if (p_audio_session != NULL)
        {
            dev_handle = ble_audio_group_find_dev(p_audio_session->group_handle,
                                                  p_link->remote_bd, p_link->remote_bd_type);
            if (dev_handle)
            {
                for (uint8_t k = 0; k < p_audio_session->server_num; k++)
                {
                    if (p_audio_session->server[k].dev_handle == dev_handle)
                    {
                        for (uint8_t j = 0; j < p_audio_session->server[k].ase_num; j++)
                        {
                            T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[k].ase_info[j].p_ase_entry;
                            if (p_ase_entry)
                            {
                                p_ase_entry->used = false;
                                p_ase_entry->handle = NULL;
                                p_audio_session->server[k].ase_info[j].p_ase_entry = NULL;
                            }
                            if (p_audio_session->server[k].ase_info[j].p_metadata)
                            {
                                ble_audio_mem_free(p_audio_session->server[k].ase_info[j].p_metadata);
                                p_audio_session->server[k].ase_info[j].p_metadata = 0;
                                p_audio_session->server[k].ase_info[j].metadata_len = 0;
                            }
                        }
                    }
                }
            }
            T_AUDIO_STREAM_STATE new_state = bap_unicast_audio_get_new_state(p_audio_session,
                                                                             BAP_UNICAST_RESULT_SUCCESS);
            p_audio_session->retry_num = 0;
            bap_unicast_send_bap_info(p_audio_session, new_state,
                                      BAP_UNICAST_RESULT_ACL_DISCONN, cause, NULL);
            bap_unicast_audio_check_state(p_audio_session, NULL, AUDIO_STREAM_EVENT_READ_ASE_STATE, NULL);
        }
    }
}
#endif


