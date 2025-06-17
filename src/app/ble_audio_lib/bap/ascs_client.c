#include <string.h>
#include "ble_audio_mgr.h"
#include "ascs_client.h"
#include "trace.h"
#include "ble_audio_dm.h"
#include "ble_link_util.h"
#include "ascs_client_cp.h"
#include "bt_gatt_client.h"

#if LE_AUDIO_ASCS_CLIENT_SUPPORT

uint8_t ascs_init_cfg = 0;

T_ASCS_CLIENT_DB *ascs_check_link_int(const char *p_func_name, uint16_t conn_handle)
{
    T_BLE_AUDIO_LINK *p_link = NULL;

    p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL || p_link->p_ascs_client == NULL)
    {
        PROTOCOL_PRINT_ERROR2("ascs_check_link: %s failed, conn_handle 0x%x", TRACE_STRING(p_func_name),
                              conn_handle);
        return NULL;
    }
    return p_link->p_ascs_client;
}

T_ASE_CHAR_ENTRY *ascs_find_ase_entry(uint16_t conn_handle, uint8_t char_instance_id,
                                      uint8_t direction)
{
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    T_ASCS_CLIENT_DB *p_ascs_db = ascs_check_link(conn_handle);
    if (p_link != NULL && p_ascs_db != NULL)
    {
        if (direction == SERVER_AUDIO_SINK && char_instance_id < p_ascs_db->snk_ase_num)
        {
            return &p_ascs_db->ase_data[char_instance_id];
        }
        else if (direction == SERVER_AUDIO_SOURCE && char_instance_id < p_ascs_db->src_ase_num)
        {
            return &p_ascs_db->ase_data[char_instance_id + p_ascs_db->snk_ase_num];
        }
    }
    return NULL;
}

T_ASE_CHAR_ENTRY *ascs_find_ase_by_cig_cis_id(uint16_t conn_handle, uint8_t cig_id, uint8_t cis_id)
{
    uint8_t i;
    T_ASE_CHAR_ENTRY *p_entry = NULL;
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    T_ASCS_CLIENT_DB *p_ascs_db = ascs_check_link(conn_handle);
    if (p_link != NULL && p_ascs_db != NULL)
    {
        for (i = 0; i < (p_ascs_db->snk_ase_num + p_ascs_db->src_ase_num); i++)
        {
            p_entry = &p_ascs_db->ase_data[i];
            if ((p_entry->used == true)
                && (p_entry->ase_state != ASE_STATE_IDLE)
                && (p_entry->ase_state != ASE_STATE_CODEC_CONFIGURED)
                && (p_entry->qos_state_data.cig_id == cig_id)
                && (p_entry->qos_state_data.cis_id == cis_id))
            {
                return p_entry;
            }
        }
    }
    PROTOCOL_PRINT_ERROR3("ascs_find_ase_by_cig_cis_id: failed, conn_handle 0x%x, cig_id %d, cis_id %d",
                          conn_handle, cig_id, cis_id);
    return NULL;
}

T_ASE_CHAR_ENTRY *ascs_find_ase_by_ase_id(uint16_t conn_handle, uint8_t ase_id)
{
    uint8_t i;
    T_ASE_CHAR_ENTRY *p_entry = NULL;
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    T_ASCS_CLIENT_DB *p_ascs_db = ascs_check_link(conn_handle);
    if (p_link != NULL && p_ascs_db != NULL)
    {
        for (i = 0; i < (p_ascs_db->snk_ase_num + p_ascs_db->src_ase_num); i++)
        {
            p_entry = &p_ascs_db->ase_data[i];
            if ((p_entry->used == true)
                && (p_entry->ase_id != ASE_INVALID_ID)
                && (p_entry->ase_id == ase_id))
            {
                return p_entry;
            }
        }
    }
    PROTOCOL_PRINT_ERROR2("ascs_find_ase_by_ase_id: failed, conn_handle 0x%x, ase_id %d",
                          conn_handle, ase_id);
    return NULL;
}

bool ascs_client_get_ase_data_int(T_ASE_CHAR_ENTRY *p_entry, T_ASE_CHAR_DATA *p_ase_data)
{
    memset(p_ase_data, 0, sizeof(T_ASE_CHAR_DATA));
    p_ase_data->ase_id = p_entry->ase_id;
    p_ase_data->ase_state = p_entry->ase_state;
    p_ase_data->direction = p_entry->direction;
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

bool ascs_client_get_ase_data(uint16_t conn_handle, uint8_t char_instance_id,
                              T_ASE_CHAR_DATA *p_ase_data,
                              uint8_t direction)
{
    T_ASE_CHAR_ENTRY *p_entry = ascs_find_ase_entry(conn_handle, char_instance_id, direction);
    if (p_entry == NULL || p_ase_data == NULL || p_entry->used == false)
    {
        PROTOCOL_PRINT_ERROR3("ascs_client_get_ase_data: failed, conn_handle 0x%x, char_instance_id %d direction %d",
                              conn_handle, char_instance_id, direction);
        return false;
    }
    return ascs_client_get_ase_data_int(p_entry, p_ase_data);
}

bool ascs_client_get_ase_data_by_id(uint16_t conn_handle, uint8_t ase_id,
                                    T_ASE_CHAR_DATA *p_ase_data)
{
    T_ASE_CHAR_ENTRY *p_entry = ascs_find_ase_by_ase_id(conn_handle, ase_id);
    if (p_entry == NULL || p_ase_data == NULL || p_entry->used == false)
    {
        return false;
    }
    return ascs_client_get_ase_data_int(p_entry, p_ase_data);
}

bool ascs_client_get_codec_data(uint16_t conn_handle, uint8_t ase_id,
                                T_ASE_DATA_CODEC_CONFIGURED *p_codec_data)
{
    T_ASE_CHAR_ENTRY *p_entry = ascs_find_ase_by_ase_id(conn_handle, ase_id);
    if (p_entry == NULL || p_codec_data == NULL || p_entry->used == false)
    {
        return false;
    }
    if (p_entry->ase_state != ASE_STATE_IDLE &&
        p_entry->ase_state != ASE_STATE_RELEASING)
    {
        memcpy(&p_codec_data->data, &p_entry->codec_state_data,
               sizeof(T_ASE_CODEC_CFG_STATE_DATA));
        p_codec_data->p_codec_spec_cfg = p_entry->p_codec_spec_cfg;
        return true;
    }
    return false;
}

bool ascs_client_get_metadata(uint16_t conn_handle, uint8_t ase_id,
                              T_ASE_DATA_WITH_METADATA *p_metadata)
{
    T_ASE_CHAR_ENTRY *p_entry = ascs_find_ase_by_ase_id(conn_handle, ase_id);
    if (p_entry == NULL || p_metadata == NULL || p_entry->used == false)
    {
        return false;
    }
    if (p_entry->ase_state == ASE_STATE_ENABLING ||
        p_entry->ase_state == ASE_STATE_STREAMING ||
        p_entry->ase_state == ASE_STATE_DISABLING)
    {
        p_metadata->cig_id = p_entry->qos_state_data.cig_id;
        p_metadata->cis_id = p_entry->qos_state_data.cis_id;
        p_metadata->metadata_length = p_entry->metadata_len;
        p_metadata->p_metadata = p_entry->p_metadata;
        return true;
    }
    return false;
}

bool ascs_client_app_ctl_handshake(uint16_t conn_handle, uint8_t ase_id, bool app_handshake)
{
    T_ASE_CHAR_ENTRY *p_entry = ascs_find_ase_by_ase_id(conn_handle, ase_id);
    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR2("ascs_client_app_ctl_handshake: failed, conn_handle 0x%x, ase_id %d",
                              conn_handle,
                              ase_id);
        return false;
    }
    p_entry->app_handshake = app_handshake;
    return true;
}

void ascs_client_enable_cccd(uint16_t conn_handle)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_SUCCESS;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_ASCS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    T_ASCS_CLIENT_DB *p_ascs_db = ascs_check_link(conn_handle);
    if (p_ascs_db != 0)
    {
        char_uuid.p.uuid16 = ASCS_UUID_CHAR_ASE_CONTROL_POINT;
        if (gatt_client_check_cccd_enabled(conn_handle, &srv_uuid, &char_uuid) == false)
        {
            cause = gatt_client_enable_char_cccd(conn_handle, &srv_uuid, &char_uuid,
                                                 GATT_CLIENT_CONFIG_ALL);
        }
        if (cause == GAP_CAUSE_SUCCESS)
        {
            if (p_ascs_db->snk_ase_num != 0)
            {
                if (gatt_client_check_cccd_enabled(conn_handle, &srv_uuid, &char_uuid) == false)
                {
                    char_uuid.p.uuid16 = ASCS_UUID_CHAR_SNK_ASE;
                    gatt_client_enable_char_cccd(conn_handle, &srv_uuid, &char_uuid, GATT_CLIENT_CONFIG_ALL);
                }
            }
            if (p_ascs_db->src_ase_num != 0)
            {
                if (gatt_client_check_cccd_enabled(conn_handle, &srv_uuid, &char_uuid) == false)
                {
                    char_uuid.p.uuid16 = ASCS_UUID_CHAR_SRC_ASE;
                    gatt_client_enable_char_cccd(conn_handle, &srv_uuid, &char_uuid, GATT_CLIENT_CONFIG_ALL);
                }
            }
        }
    }
    return;
}

bool ascs_send_ase_cp(uint16_t conn_handle, uint8_t *p_data, uint16_t len, bool is_req)
{
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    T_ASCS_CLIENT_DB *p_ascs_db;
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL || p_link->p_ascs_client == NULL)
    {
        return false;
    }
    p_ascs_db = p_link->p_ascs_client;

    if (p_ascs_db->ase_cp_handle != 0)
    {
        T_GATT_WRITE_TYPE type = GATT_WRITE_TYPE_CMD;
        if (len > p_link->mtu_size - 3 || is_req == true)
        {
            type = GATT_WRITE_TYPE_REQ;
        }
        cause = gatt_client_write(conn_handle, type, p_ascs_db->ase_cp_handle, len, p_data, NULL);
    }
    if (cause == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ascs_client_read_ase(uint16_t conn_handle, uint8_t instance_id, uint8_t direction)
{
    T_ASCS_CLIENT_DB *p_ascs_db = ascs_check_link(conn_handle);
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    if (p_ascs_db == NULL || p_ascs_db->pending_action != ASE_PENDING_IDLE)
    {
        return false;
    }
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_ASCS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = instance_id;
    char_uuid.p.uuid16 = direction == SERVER_AUDIO_SINK ? ASCS_UUID_CHAR_SNK_ASE :
                         ASCS_UUID_CHAR_SRC_ASE;
    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_read(conn_handle, handle, NULL);
    }
    if (cause == GAP_CAUSE_SUCCESS)
    {
        p_ascs_db->pending_action = ASE_PENDING_READ_ASE;
        return true;
    }
    else
    {
        return false;
    }
}

bool ascs_client_read_ase_all_int(uint16_t conn_handle, T_ASCS_CLIENT_DB *p_ascs_db, bool req)
{
    T_ASCS_CLIENT_READ_ASE_ALL read_result;
    T_ASE_CHAR_ENTRY *p_entry = NULL;
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    uint8_t i;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_ASCS;
    char_uuid.is_uuid16 = true;

    if (p_ascs_db == NULL)
    {
        return false;
    }
    if (req == true && p_ascs_db->pending_action != ASE_PENDING_IDLE)
    {
        return false;
    }
    for (i = 0; i < (p_ascs_db->snk_ase_num + p_ascs_db->src_ase_num); i++)
    {
        p_entry = &p_ascs_db->ase_data[i];
        if (p_entry->used == false)
        {
            char_uuid.p.uuid16 = p_entry->direction == SERVER_AUDIO_SINK ? ASCS_UUID_CHAR_SNK_ASE :
                                 ASCS_UUID_CHAR_SRC_ASE;
            char_uuid.instance_id = p_entry->direction == SERVER_AUDIO_SINK ? i : (i - p_ascs_db->snk_ase_num);
            if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
            {
                cause = gatt_client_read(conn_handle, handle, NULL);
                if (cause != GAP_CAUSE_SUCCESS)
                {
                    goto error;
                }
                else
                {
                    goto ret;
                }
            }
            else
            {
                goto error;
            }
        }
    }
    read_result.conn_handle = conn_handle;
    read_result.snk_ase_num = p_ascs_db->snk_ase_num;
    read_result.src_ase_num = p_ascs_db->src_ase_num;
    read_result.cause = 0;
    p_ascs_db->pending_action = ASE_PENDING_IDLE;
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CLIENT_READ_ASE_ALL, &read_result);
    return true;
ret:
    p_ascs_db->pending_action = ASE_PENDING_READ_ASE_ALL;
    return true;
error:
    p_ascs_db->pending_action = ASE_PENDING_IDLE;
    return false;
}

bool ascs_client_read_ase_all(uint16_t conn_handle)
{
    T_ASCS_CLIENT_DB *p_ascs_db = ascs_check_link(conn_handle);
    return ascs_client_read_ase_all_int(conn_handle, p_ascs_db, true);
}

void ascs_client_handle_cp_notify(uint16_t conn_handle, uint8_t *p_data, uint16_t data_len)
{
    T_ASE_CP_NOTIFY_DATA *p_parsed_data = (T_ASE_CP_NOTIFY_DATA *)p_data;
    T_ASCS_CLIENT_CP_NOTIFY cp_notify;
    if (p_parsed_data->number_of_ase == 0xFF)
    {
        if (data_len != sizeof(T_ASE_CP_NOTIFY_DATA))
        {
            goto error;
        }
    }
    else
    {
        if (data_len != (sizeof(T_ASE_CP_NOTIFY_DATA) + (p_parsed_data->number_of_ase - 1)*sizeof(
                             T_ASE_CP_NOTIFY_ARRAY_PARAM)))
        {
            goto error;
        }
    }

    PROTOCOL_PRINT_INFO3("[BAP][ASCS] ascs_client_handle_cp_notify: conn_handle 0x%x opcode 0x%x, number_of_ase 0x%x",
                         conn_handle, p_parsed_data->opcode,  p_parsed_data->number_of_ase);
    if (p_parsed_data->number_of_ase == 0xFF)
    {
        PROTOCOL_PRINT_INFO3("[BAP][ASCS] ascs_client_handle_cp_notify: ase_id 0x%x, response_code 0x%x, reason 0x%x",
                             p_parsed_data->param[0].ase_id, p_parsed_data->param[0].response_code,
                             p_parsed_data->param[0].reason);

    }
    else
    {
        bool is_success = true;
        for (uint8_t i = 0; i < p_parsed_data->number_of_ase; i++)
        {
            PROTOCOL_PRINT_INFO4("[BAP][ASCS] ASE_IDs[%d]: ase_id 0x%x, response_code 0x%x, reason 0x%x",
                                 i, p_parsed_data->param[i].ase_id, p_parsed_data->param[i].response_code,
                                 p_parsed_data->param[i].reason);
            if (p_parsed_data->param[i].response_code != ASE_CP_RESP_SUCCESS)
            {
                is_success = false;
            }
        }
        if (is_success && p_parsed_data->opcode == ASE_CP_OP_RELEASE)
        {
            for (uint8_t i = 0; i < p_parsed_data->number_of_ase; i++)
            {
                T_ASE_CHAR_ENTRY *p_entry = ascs_find_ase_by_ase_id(conn_handle, p_parsed_data->param[i].ase_id);
                if (p_entry)
                {
                    p_entry->release_local = true;
                }
            }

        }

    }
    cp_notify.conn_handle = conn_handle;
    cp_notify.opcode = p_parsed_data->opcode;
    cp_notify.number_of_ase = p_parsed_data->number_of_ase;
    cp_notify.p_param = p_parsed_data->param;
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CLIENT_CP_NOTIFY, &cp_notify);
    return;
error:
    PROTOCOL_PRINT_ERROR0("ascs_client_handle_cp_notify: invalid format");
    return;
}

#if LE_AUDIO_DEBUG
void ascs_print_state(T_ASE_CHAR_ENTRY *p_entry)
{
    if (p_entry == NULL)
    {
        return;
    }

    switch (p_entry->ase_state)
    {
    case ASE_STATE_IDLE:
        PROTOCOL_PRINT_INFO1("[BAP][ASCS] ASE_STATE_IDLE: ase_id %d", p_entry->ase_id);
        break;

    case ASE_STATE_RELEASING:
        PROTOCOL_PRINT_INFO1("[BAP][ASCS] ASE_STATE_RELEASING: ase_id %d", p_entry->ase_id);
        break;

    case ASE_STATE_CODEC_CONFIGURED:
        {
            uint32_t presentation_delay_min;
            uint32_t presentation_delay_max;
            uint16_t max_transport_latency;
            LE_ARRAY_TO_UINT24(presentation_delay_min, p_entry->codec_state_data.presentation_delay_min);
            LE_ARRAY_TO_UINT24(presentation_delay_max, p_entry->codec_state_data.presentation_delay_max);
            LE_ARRAY_TO_UINT16(max_transport_latency, p_entry->codec_state_data.max_transport_latency);

            PROTOCOL_PRINT_INFO6("[BAP][ASCS] ASE_STATE_CODEC_CONFIGURED: ase_id %d, direction 0x%01x, supported_framing 0x%01x, preferred_phy 0x%01x, preferred_retrans_number 0x%01x",
                                 p_entry->ase_id,
                                 p_entry->direction,
                                 p_entry->codec_state_data.supported_framing,
                                 p_entry->codec_state_data.preferred_phy,
                                 p_entry->codec_state_data.preferred_retrans_number,
                                 p_entry->codec_state_data.preferred_retrans_number);
            PROTOCOL_PRINT_INFO3("[BAP][ASCS] ASE_STATE_CODEC_CONFIGURED: max_transport_latency 0x%02x, presentation_delay_min 0x%03x, presentation_delay_max 0x%03x",
                                 max_transport_latency,
                                 presentation_delay_min, presentation_delay_max);
            PROTOCOL_PRINT_INFO2("[BAP][ASCS] ASE_STATE_CODEC_CONFIGURED: codec_id %b, codec_spec_cfg_len %d",
                                 TRACE_BINARY(5, p_entry->codec_state_data.codec_id),
                                 p_entry->codec_state_data.codec_spec_cfg_len);
            if (p_entry->codec_state_data.codec_spec_cfg_len)
            {
                PROTOCOL_PRINT_INFO1("[BAP][ASCS] codec_spec_cfg %b",
                                     TRACE_BINARY(p_entry->codec_spec_cfg_len, p_entry->p_codec_spec_cfg));
            }
        }
        break;

    case ASE_STATE_QOS_CONFIGURED:
        {
            uint16_t max_sdu_size;
            uint16_t max_transport_latency;
            uint32_t sdu_interval;
            uint32_t presentation_delay;
            LE_ARRAY_TO_UINT24(sdu_interval, p_entry->qos_state_data.sdu_interval);
            LE_ARRAY_TO_UINT24(presentation_delay, p_entry->qos_state_data.presentation_delay);
            LE_ARRAY_TO_UINT16(max_sdu_size, p_entry->qos_state_data.max_sdu);
            LE_ARRAY_TO_UINT16(max_transport_latency, p_entry->qos_state_data.max_transport_latency);
            PROTOCOL_PRINT_INFO6("[BAP][ASCS] ASE_STATE_QOS_CONFIGURED: ase_id %d, cig_id %d, cis_id %d, sdu_interval 0x%03x, framing 0x%01x, phy 0x%x",
                                 p_entry->ase_id,
                                 p_entry->qos_state_data.cig_id,
                                 p_entry->qos_state_data.cis_id, sdu_interval,
                                 p_entry->qos_state_data.framing,
                                 p_entry->qos_state_data.phy);
            PROTOCOL_PRINT_INFO4("[BAP][ASCS] ASE_STATE_QOS_CONFIGURED: max_sdu 0x%02x, retransmission_number %d, max_transport_latency 0x%02x, presentation_delay 0x%03x",
                                 max_sdu_size, p_entry->qos_state_data.retransmission_number,
                                 max_transport_latency, presentation_delay);
        }
        break;

    case ASE_STATE_ENABLING:
        PROTOCOL_PRINT_INFO2("[BAP][ASCS] ASE_STATE_ENABLING: ase_id %d, metadata_len %d",
                             p_entry->ase_id,
                             p_entry->metadata_len);
        if (p_entry->metadata_len)
        {
            PROTOCOL_PRINT_INFO1("[BAP][ASCS] metadata %b",
                                 TRACE_BINARY(p_entry->metadata_len, p_entry->p_metadata));
        }
        break;

    case ASE_STATE_STREAMING:
        PROTOCOL_PRINT_INFO2("[BAP][ASCS] ASE_STATE_STREAMING: ase_id %d, metadata_len %d",
                             p_entry->ase_id,
                             p_entry->metadata_len);
        if (p_entry->metadata_len)
        {
            PROTOCOL_PRINT_INFO1("[BAP][ASCS] metadata %b",
                                 TRACE_BINARY(p_entry->metadata_len, p_entry->p_metadata));
        }
        break;

    case ASE_STATE_DISABLING:
        PROTOCOL_PRINT_INFO2("[BAP][ASCS] ASE_STATE_DISABLING: ase_id %d, metadata_len %d",
                             p_entry->ase_id,
                             p_entry->metadata_len);
        if (p_entry->metadata_len)
        {
            PROTOCOL_PRINT_INFO1("[BAP][ASCS] metadata %b",
                                 TRACE_BINARY(p_entry->metadata_len, p_entry->p_metadata));
        }
        break;

    default:
        break;
    }
}
#endif

T_ASE_CHAR_ENTRY *ascs_client_handle_ase_data(uint16_t conn_handle, uint8_t instance_id,
                                              uint8_t *p_data,
                                              uint16_t data_len, uint8_t direction, bool is_notify)
{
    T_ASE_CHAR_ENTRY *p_entry = ascs_find_ase_entry(conn_handle, instance_id, direction);
    uint8_t ase_old_state;
    uint8_t total_len = 0;
    uint8_t idx = 0;
    bool result = false;
    if (p_entry == NULL || p_data == NULL || data_len < ASE_DATA_HEADER_LEN)
    {
        PROTOCOL_PRINT_INFO0("[BAP][ASCS] ascs_client_handle_ase_data: Invalid param");
        return NULL;
    }
    ase_old_state = p_entry->ase_state;
    PROTOCOL_PRINT_TRACE4("[BAP][ASCS] ascs_client_handle_ase_data: conn_handle 0x%x, direction %d, old state 0x%x, new state 0x%x",
                          conn_handle, direction, ase_old_state, p_data[1]);
    switch (p_data[1])
    {
    case ASE_STATE_IDLE:
    case ASE_STATE_RELEASING:
        if (data_len == ASE_DATA_HEADER_LEN)
        {
            p_entry->used = true;
            p_entry->ase_id = p_data[0];
            p_entry->ase_state = p_data[1];
            result = true;
        }
        break;

    case ASE_STATE_CODEC_CONFIGURED:

        if (data_len >= ASE_DATA_CODEC_CONFIGURED_STATE_MIN_LEN)
        {
            total_len = ASE_DATA_CODEC_CONFIGURED_STATE_MIN_LEN + p_data[ASE_DATA_CODEC_LEN_OFFSET];
            if (total_len == data_len)
            {
                p_entry->used = true;
                p_entry->ase_id = p_data[idx++];
                p_entry->ase_state = p_data[idx++];
                memcpy(&p_entry->codec_state_data, p_data + idx, sizeof(T_ASE_CODEC_CFG_STATE_DATA));
                idx += sizeof(T_ASE_CODEC_CFG_STATE_DATA);
                p_entry->codec_spec_cfg_len = p_entry->codec_state_data.codec_spec_cfg_len;
                if (p_entry->p_codec_spec_cfg)
                {
                    ble_audio_mem_free(p_entry->p_codec_spec_cfg);
                    p_entry->p_codec_spec_cfg = NULL;
                }
                if (p_entry->codec_spec_cfg_len)
                {
                    p_entry->p_codec_spec_cfg = ble_audio_mem_zalloc(p_entry->codec_spec_cfg_len);
                    if (p_entry->p_codec_spec_cfg == NULL)
                    {
                        result = false;
                        break;
                    }
                    memcpy(p_entry->p_codec_spec_cfg, p_data + idx, p_entry->codec_spec_cfg_len);
                }
                p_entry->cis_conn_handle = 0;
                result = true;
            }
        }
        break;

    case ASE_STATE_QOS_CONFIGURED:
        if (data_len == ASE_DATA_QOS_CONFIGURED_STATE_LEN)
        {
            p_entry->used = true;
            p_entry->ase_id = p_data[0];
            p_entry->ase_state = p_data[1];
            memcpy(&p_entry->qos_state_data, (p_data + 2), sizeof(T_ASE_QOS_CFG_STATE_DATA));
            cig_mgr_get_cis_handle(p_entry->qos_state_data.cis_id, &p_entry->cis_conn_handle);
            result = true;
        }
        break;

    case ASE_STATE_ENABLING:
    case ASE_STATE_STREAMING:
    case ASE_STATE_DISABLING:
        if (data_len >= ASE_DATA_WITH_METADATA_MIN_LEN)
        {
            total_len = ASE_DATA_WITH_METADATA_MIN_LEN + p_data[ASE_DATA_WITH_METADATA_LEN_OFFSET];
            if (total_len == data_len)
            {
                p_entry->used = true;
                p_entry->ase_id = p_data[idx++];
                p_entry->ase_state = p_data[idx++];
                idx += 2;
                p_entry->metadata_len = p_data[idx++];
                if (p_entry->p_metadata)
                {
                    ble_audio_mem_free(p_entry->p_metadata);
                    p_entry->p_metadata = NULL;
                }
                if (p_entry->metadata_len)
                {
                    p_entry->p_metadata = ble_audio_mem_zalloc(p_entry->metadata_len);
                    if (p_entry->p_metadata == NULL)
                    {
                        result = false;
                        break;
                    }
                    memcpy(p_entry->p_metadata, p_data + idx, p_entry->metadata_len);
                }
                result = true;
            }
        }
        break;

    default:
        break;
    }
    if (result)
    {

        PROTOCOL_PRINT_INFO4("[BAP][ASCS] ASE char: conn_handle 0x%x, ase_id 0x%x, ase_state (0x%x -> 0x%x)",
                             conn_handle,
                             p_entry->ase_id,
                             ase_old_state,
                             p_entry->ase_state);
        if (p_entry->ase_state != ASE_STATE_DISABLING)
        {
            p_entry->rec_stop_ready_send = false;
        }
        if (p_entry->ase_state != ASE_STATE_ENABLING)
        {
            p_entry->rec_start_ready_send = false;
        }
        if (is_notify)
        {
            T_ASCS_CLIENT_ASE_DATA ase_char_data;
            ase_char_data.conn_handle = conn_handle;
            ase_char_data.instance_id = instance_id;
            ase_char_data.cause = 0;
            ase_char_data.direction = p_entry->direction;
            if (ascs_client_get_ase_data(conn_handle, instance_id, &ase_char_data.ase_data, p_entry->direction))
            {
                ase_char_data.notify = true;
                ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CLIENT_ASE_DATA_NOTIFY, &ase_char_data);
            }
        }
#if LE_AUDIO_DEBUG
        ascs_print_state(p_entry);
#endif
        return p_entry;
    }
    else
    {
        PROTOCOL_PRINT_INFO2("[BAP][ASCS] ascs_client_handle_ase_data: failed, ase_state %d, data_len %d",
                             p_entry->ase_state, data_len);
        return NULL;
    }
}

T_APP_RESULT ascs_client_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;
    T_ASCS_CLIENT_DB *p_ascs_db = ascs_check_link(conn_handle);
    if (p_ascs_db == NULL && type != GATT_CLIENT_EVENT_DIS_DONE)
    {
        goto error;
    }
    switch (type)
    {
    case GATT_CLIENT_EVENT_DIS_DONE:
        {
            uint8_t snk_ase_num = 0;
            uint8_t src_ase_num = 0;
            T_ASCS_CLIENT_DIS_DONE dis_done = {0};
            PROTOCOL_PRINT_INFO2("[BAP][ASCS] GATT_CLIENT_EVENT_DIS_DONE: is_found %d, srv_instance_num %d",
                                 p_client_cb_data->dis_done.is_found,
                                 p_client_cb_data->dis_done.srv_instance_num);
            if (p_client_cb_data->dis_done.is_found)
            {
                uint8_t i;
                T_BLE_AUDIO_LINK *p_link = NULL;
                T_ATTR_UUID srv_uuid;
                T_ATTR_UUID char_uuid;
                srv_uuid.is_uuid16 = true;
                srv_uuid.instance_id = 0;
                srv_uuid.p.uuid16 = GATT_UUID_ASCS;
                char_uuid.is_uuid16 = true;
                char_uuid.instance_id = 0;
                if (ascs_init_cfg & UNICAST_CLT_SRC_ROLE)
                {
                    char_uuid.p.uuid16 = ASCS_UUID_CHAR_SNK_ASE;
                    snk_ase_num = gatt_client_get_char_num(conn_handle, &srv_uuid, &char_uuid);
                }
                if (ascs_init_cfg & UNICAST_CLT_SNK_ROLE)
                {
                    char_uuid.p.uuid16 = ASCS_UUID_CHAR_SRC_ASE;
                    src_ase_num = gatt_client_get_char_num(conn_handle, &srv_uuid, &char_uuid);
                }
                p_link = ble_audio_link_find_by_conn_handle(conn_handle);
                if (p_link == NULL || (snk_ase_num + src_ase_num) == 0)
                {
                    goto error;
                }

                if (p_link->p_ascs_client)
                {
                    ble_audio_mem_free(p_link->p_ascs_client);
                }

                p_link->p_ascs_client = ble_audio_mem_zalloc(sizeof(T_ASCS_CLIENT_DB) +
                                                             (snk_ase_num + src_ase_num - 1) * sizeof(T_ASE_CHAR_ENTRY));

                if (p_link->p_ascs_client == NULL)
                {
                    goto error;
                }
                p_ascs_db = p_link->p_ascs_client;
                p_ascs_db->snk_ase_num = snk_ase_num;
                p_ascs_db->src_ase_num = src_ase_num;
                dis_done.snk_ase_num = snk_ase_num;
                dis_done.src_ase_num = src_ase_num;
                for (i = 0; i < p_ascs_db->snk_ase_num; i++)
                {
                    p_ascs_db->ase_data[i].instance_id = i;
                    p_ascs_db->ase_data[i].direction = SERVER_AUDIO_SINK;
                }
                for (i = 0; i <  p_ascs_db->src_ase_num; i++)
                {
                    p_ascs_db->ase_data[i + p_ascs_db->snk_ase_num].instance_id = i;
                    p_ascs_db->ase_data[i + p_ascs_db->snk_ase_num].direction = SERVER_AUDIO_SOURCE;
                }
                char_uuid.p.uuid16 = ASCS_UUID_CHAR_ASE_CONTROL_POINT;
                if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid,
                                                 &p_ascs_db->ase_cp_handle) == false)
                {
                    goto error;
                }
            }
            dis_done.conn_handle = conn_handle;
            dis_done.is_found = p_client_cb_data->dis_done.is_found;
            dis_done.load_from_ftl = p_client_cb_data->dis_done.load_from_ftl;
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CLIENT_DIS_DONE, &dis_done);
        }
        break;

    case GATT_CLIENT_EVENT_READ_RESULT:
        {
            PROTOCOL_PRINT_INFO7("[BAP][ASCS] GATT_CLIENT_EVENT_READ_RESULT: conn_handle 0x%x, cause 0x%x, char_type %d, srv_instance_id %d, uuid16[%d] 0x%x, handle 0x%x",
                                 conn_handle, p_client_cb_data->read_result.cause,
                                 p_client_cb_data->read_result.char_type,
                                 p_client_cb_data->read_result.srv_instance_id,
                                 p_client_cb_data->read_result.char_uuid.instance_id,
                                 p_client_cb_data->read_result.char_uuid.p.uuid16,
                                 p_client_cb_data->read_result.handle);
            PROTOCOL_PRINT_INFO2("[BAP][ASCS] GATT_CLIENT_EVENT_READ_RESULT: value[%d] %b",
                                 p_client_cb_data->read_result.value_size,
                                 TRACE_BINARY(p_client_cb_data->read_result.value_size,
                                              p_client_cb_data->read_result.p_value));
            if (p_client_cb_data->read_result.char_uuid.p.uuid16 == ASCS_UUID_CHAR_SNK_ASE ||
                p_client_cb_data->read_result.char_uuid.p.uuid16 == ASCS_UUID_CHAR_SRC_ASE)
            {
                uint8_t direction;
                if (p_client_cb_data->read_result.char_uuid.p.uuid16 == ASCS_UUID_CHAR_SNK_ASE)
                {
                    direction = SERVER_AUDIO_SINK;
                }
                else
                {
                    direction = SERVER_AUDIO_SOURCE;
                }
                if (p_client_cb_data->read_result.cause == GAP_SUCCESS)
                {
                    T_ASE_CHAR_ENTRY *p_entry = ascs_client_handle_ase_data(conn_handle,
                                                                            p_client_cb_data->read_result.char_uuid.instance_id,
                                                                            p_client_cb_data->read_result.p_value,
                                                                            p_client_cb_data->read_result.value_size,
                                                                            direction, false);
                    if (p_entry == NULL)
                    {
                        p_client_cb_data->read_result.cause = GAP_ERR_INVALID_PARAM;
                    }
                }

                if (p_client_cb_data->read_result.cause == GAP_SUCCESS)
                {
                    if (p_ascs_db->pending_action == ASE_PENDING_READ_ASE)
                    {
                        T_ASCS_CLIENT_ASE_DATA ase_char_data;
                        if (ascs_client_get_ase_data(conn_handle, p_client_cb_data->read_result.char_uuid.instance_id,
                                                     &ase_char_data.ase_data, direction))
                        {
                            ase_char_data.conn_handle = conn_handle;
                            ase_char_data.instance_id = p_client_cb_data->read_result.char_uuid.instance_id;
                            ase_char_data.direction = direction;
                            ase_char_data.cause = GAP_SUCCESS;
                            ase_char_data.notify = false;
                            p_ascs_db->pending_action = ASE_PENDING_IDLE;
                            ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CLIENT_READ_ASE_DATA, &ase_char_data);
                        }
                    }
                    else if (p_ascs_db->pending_action == ASE_PENDING_READ_ASE_ALL)
                    {
                        if (ascs_client_read_ase_all_int(conn_handle, p_ascs_db, false) == false)
                        {
                            p_client_cb_data->read_result.cause = GAP_ERR_REQ_FAILED;
                        }
                    }
                }
                if (p_client_cb_data->read_result.cause != GAP_SUCCESS)
                {
                    if (p_ascs_db->pending_action == ASE_PENDING_READ_ASE)
                    {
                        T_ASCS_CLIENT_ASE_DATA read_result = {0};

                        read_result.conn_handle = conn_handle;
                        read_result.notify = false;
                        read_result.instance_id = p_client_cb_data->read_result.srv_instance_id;
                        read_result.direction = direction;
                        read_result.cause = p_client_cb_data->read_result.cause;
                        p_ascs_db->pending_action = ASE_PENDING_IDLE;
                        ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CLIENT_READ_ASE_DATA, &read_result);
                    }
                    else if (p_ascs_db->pending_action == ASE_PENDING_READ_ASE_ALL)
                    {
                        T_ASCS_CLIENT_READ_ASE_ALL read_result = {0};

                        read_result.conn_handle = conn_handle;
                        read_result.snk_ase_num = p_ascs_db->snk_ase_num;
                        read_result.src_ase_num = p_ascs_db->src_ase_num;
                        read_result.cause = p_client_cb_data->read_result.cause;
                        p_ascs_db->pending_action = ASE_PENDING_IDLE;
                        ble_audio_mgr_dispatch(LE_AUDIO_MSG_ASCS_CLIENT_READ_ASE_ALL, &read_result);
                    }
                }
            }
        }
        break;

    case GATT_CLIENT_EVENT_NOTIFY_IND:
        {
            PROTOCOL_PRINT_INFO7("[BAP][ASCS] GATT_CLIENT_EVENT_NOTIFY_IND: conn_handle 0x%x, srv_instance_id %d, uuid16[%d] 0x%x, handle 0x%x, data[%d] %b",
                                 conn_handle,
                                 p_client_cb_data->notify_ind.srv_instance_id,
                                 p_client_cb_data->notify_ind.char_uuid.instance_id,
                                 p_client_cb_data->notify_ind.char_uuid.p.uuid16,
                                 p_client_cb_data->notify_ind.handle,
                                 p_client_cb_data->notify_ind.value_size,
                                 TRACE_BINARY(p_client_cb_data->notify_ind.value_size,
                                              p_client_cb_data->notify_ind.p_value));
            if (p_client_cb_data->notify_ind.char_uuid.p.uuid16 == ASCS_UUID_CHAR_ASE_CONTROL_POINT)
            {
                ascs_client_handle_cp_notify(conn_handle, p_client_cb_data->notify_ind.p_value,
                                             p_client_cb_data->notify_ind.value_size);
            }
            else if (p_client_cb_data->notify_ind.char_uuid.p.uuid16 == ASCS_UUID_CHAR_SNK_ASE ||
                     p_client_cb_data->notify_ind.char_uuid.p.uuid16 == ASCS_UUID_CHAR_SRC_ASE)
            {
                uint8_t direction;
                if (p_client_cb_data->notify_ind.char_uuid.p.uuid16 == ASCS_UUID_CHAR_SNK_ASE)
                {
                    direction = SERVER_AUDIO_SINK;
                }
                else
                {
                    direction = SERVER_AUDIO_SOURCE;
                }
                T_ASE_CHAR_ENTRY *p_entry = ascs_client_handle_ase_data(conn_handle,
                                                                        p_client_cb_data->notify_ind.char_uuid.instance_id,
                                                                        p_client_cb_data->notify_ind.p_value,
                                                                        p_client_cb_data->notify_ind.value_size, direction, true);
                if (p_entry)
                {
                    ascs_client_check_state(conn_handle, p_entry);
                }
            }
        }

        break;

    case GATT_CLIENT_EVENT_WRITE_RESULT:
        {
            PROTOCOL_PRINT_INFO8("[BAP][ASCS] GATT_CLIENT_EVENT_WRITE_RESULT: conn_handle 0x%x, cause 0x%x, type %d, char_type %d, srv_instance_id %d, uuid16[%d] 0x%x, handle 0x%x",
                                 conn_handle, p_client_cb_data->write_result.cause,
                                 p_client_cb_data->write_result.type,
                                 p_client_cb_data->write_result.char_type,
                                 p_client_cb_data->write_result.srv_instance_id,
                                 p_client_cb_data->write_result.char_uuid.instance_id,
                                 p_client_cb_data->write_result.char_uuid.p.uuid16,
                                 p_client_cb_data->write_result.handle);
        }
        break;

    case GATT_CLIENT_EVENT_CCCD_CFG:
        {
            ble_audio_mgr_dispatch_client_attr_cccd_info(conn_handle, &p_client_cb_data->cccd_cfg);
        }
        break;

    default:
        break;
    }

    return result;
error:
    PROTOCOL_PRINT_ERROR1("[BAP][ASCS] ascs_client_cb: error, type %d", type);
    return APP_RESULT_APP_ERR;
}

static void ascs_client_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
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
                PROTOCOL_PRINT_ERROR1("ascs_client_dm_cback: not find the link, conn_id %d",
                                      param->conn_state.conn_id);
                break;
            }
            if (param->conn_state.state == GAP_CONN_STATE_DISCONNECTED)
            {
                PROTOCOL_PRINT_INFO0("ascs_client_dm_cback: GAP_CONN_STATE_DISCONNECTED");
                if (p_link->p_ascs_client != NULL)
                {
                    T_ASCS_CLIENT_DB *p_ascs_db = (T_ASCS_CLIENT_DB *)p_link->p_ascs_client;
                    uint8_t i;
                    T_ASE_CHAR_ENTRY *p_entry = NULL;
                    for (i = 0; i < (p_ascs_db->snk_ase_num + p_ascs_db->src_ase_num); i++)
                    {
                        p_entry = &p_ascs_db->ase_data[i];
                        if (p_entry->p_metadata)
                        {
                            ble_audio_mem_free(p_entry->p_metadata);
                        }
                        if (p_entry->p_codec_spec_cfg)
                        {
                            ble_audio_mem_free(p_entry->p_codec_spec_cfg);
                        }
                    }
                    ble_audio_mem_free(p_link->p_ascs_client);
                    p_link->p_ascs_client = NULL;
                }
            }
        }
        break;

    default:
        break;
    }
}

bool ascs_client_init(uint8_t ascs_cfg)
{
    T_ATTR_UUID srv_uuid = {0};
    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = GATT_UUID_ASCS;
    ascs_init_cfg = ascs_cfg;
    if (gatt_client_spec_register(&srv_uuid, ascs_client_cb) == GAP_CAUSE_SUCCESS)
    {
        ble_dm_cback_register(ascs_client_dm_cback);
        return true;
    }
    return false;
}

#if LE_AUDIO_DEINIT
void ascs_client_deinit(void)
{
    ascs_init_cfg = 0;

    for (uint8_t i = 0; i < ble_audio_db.acl_link_num; i++)
    {
        if (ble_audio_db.le_link[i].p_ascs_client != NULL)
        {
            T_ASCS_CLIENT_DB *p_ascs_db = (T_ASCS_CLIENT_DB *)ble_audio_db.le_link[i].p_ascs_client;
            uint8_t j;
            T_ASE_CHAR_ENTRY *p_entry = NULL;
            for (j = 0; j < (p_ascs_db->snk_ase_num + p_ascs_db->src_ase_num); j++)
            {
                p_entry = &p_ascs_db->ase_data[j];
                if (p_entry->p_metadata)
                {
                    ble_audio_mem_free(p_entry->p_metadata);
                }

                if (p_entry->p_codec_spec_cfg)
                {
                    ble_audio_mem_free(p_entry->p_codec_spec_cfg);
                }
            }
            ble_audio_mem_free(ble_audio_db.le_link[i].p_ascs_client);
            ble_audio_db.le_link[i].p_ascs_client = NULL;
        }
    }

}
#endif
#endif
