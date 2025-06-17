#include "trace.h"
#include "pacs_mgr.h"
#include "pac_mgr.h"
#include "pacs.h"
#include "ble_audio_mgr.h"
#include "ble_audio_dm.h"
#include "ble_link_util.h"
#include "ble_audio_ual.h"

#if LE_AUDIO_PACS_SUPPORT

#define PACS_SINK_AUDIO_LOCATIONS_CCCD_FLAG        0x1000
#define PACS_SOURCE_AUDIO_LOCATIONS_CCCD_FLAG      0x2000
#define PACS_AUDIO_AVAILABILITY_CONTEXTS_CCCD_FLAG 0x4000
#define PACS_SUPPORTED_AUDIO_CONTEXTS_CCCD_FLAG    0x8000
#define PACS_PAC_CCCD_FLAG_MASK                    0x0FFF

#define PACS_READ_DATA_LEN                        4

uint8_t pacs_read_data[PACS_READ_DATA_LEN];
T_PACS *p_pacs = NULL;
uint8_t pacs_sink_audio_locations_char_prop = GATT_CHAR_PROP_READ;
uint8_t pacs_source_audio_locations_char_prop = GATT_CHAR_PROP_READ;
uint8_t pacs_supported_audio_contexts_char_prop = GATT_CHAR_PROP_READ;

bool pacs_send_notify(uint16_t conn_handle, uint16_t attrib_idx, uint8_t *p_data, uint8_t data_len)
{
    PROTOCOL_PRINT_INFO3("pacs_send_notify: conn_handle 0x%x, attrib_idx %d, data_len %d",
                         conn_handle, attrib_idx, data_len);
    if (p_pacs == NULL)
    {
        return false;
    }
    return gatt_svc_send_data(conn_handle, L2C_FIXED_CID_ATT, p_pacs->service_id, attrib_idx,
                              p_data,
                              data_len, GATT_PDU_TYPE_NOTIFICATION);
}

bool pacs_send_all_notify(uint16_t update_cfg, uint8_t attrib_idx, uint8_t *p_data,
                          uint16_t data_len)
{
    uint8_t i;
    uint16_t cccd_handle;
    cccd_handle = server_get_start_handle(p_pacs->service_id);
    cccd_handle += (attrib_idx + 1);

    for (i = 0; i < ble_audio_db.acl_link_num; i++)
    {
        if ((ble_audio_db.le_link[i].used == true) &&
            (ble_audio_db.le_link[i].state == GAP_CONN_STATE_CONNECTED))
        {
            T_LE_SRV_CFG *p_srv = ble_srv_find_by_srv_id(ble_audio_db.le_link[i].conn_handle,
                                                         p_pacs->service_id);
            if (p_srv != NULL && (p_srv->cccd_cfg[0] & update_cfg))
            {
                pacs_send_notify(ble_audio_db.le_link[i].conn_handle, attrib_idx, p_data, data_len);
            }
        }
    }
    ble_audio_ual_set_pending_cccd(cccd_handle);
    return true;
}

void pacs_write_sink_audio_location_post_proc(uint16_t conn_handle, uint16_t cid,
                                              T_SERVER_ID service_id,
                                              uint16_t attrib_index, uint16_t length,
                                              uint8_t *p_value)
{
    /* check notify */
    if ((pacs_sink_audio_locations_char_prop & GATT_CHAR_PROP_NOTIFY) &&
        (ble_audio_db.dev_state.gap_init_state == GAP_INIT_STATE_STACK_READY) &&
        (p_pacs->sink_audio_loc_idx != 0))
    {
        uint8_t data[4];
        LE_UINT32_TO_ARRAY(data, ble_audio_db.sink_audio_location);
        pacs_send_all_notify(PACS_SINK_AUDIO_LOCATIONS_CCCD_FLAG, p_pacs->sink_audio_loc_idx,
                             data, 4);
    }
}

void pacs_write_source_audio_location_post_proc(uint16_t conn_handle, uint16_t cid,
                                                T_SERVER_ID service_id,
                                                uint16_t attrib_index, uint16_t length,
                                                uint8_t *p_value)
{
    /* check notify */
    if ((pacs_source_audio_locations_char_prop & GATT_CHAR_PROP_NOTIFY) &&
        (ble_audio_db.dev_state.gap_init_state == GAP_INIT_STATE_STACK_READY) &&
        (p_pacs->source_audio_loc_idx != 0))
    {
        uint8_t data[4];
        LE_UINT32_TO_ARRAY(data, ble_audio_db.source_audio_location);
        pacs_send_all_notify(PACS_SOURCE_AUDIO_LOCATIONS_CCCD_FLAG, p_pacs->source_audio_loc_idx,
                             data, 4);
    }
}

bool pacs_pac_update(uint8_t pac_id, const uint8_t *pac_data, uint8_t pac_data_len)
{
    T_PAC_RECORD *p_record;
    p_record = pac_find_by_id(pac_id);
    if (p_record == NULL)
    {
        return false;
    }
    if (pac_update(pac_id, pac_data, pac_data_len))
    {
        uint16_t pac_cccd_flag = 0x0001;
        pac_cccd_flag = pac_cccd_flag << pac_id;
        return pacs_send_all_notify(pac_cccd_flag, p_record->attrib_idx,
                                    (uint8_t *)pac_data, pac_data_len);
    }
    return false;
}

bool pacs_update_sink_audio_locations(uint32_t sink_audio_location)
{
    if (ble_audio_db.sink_audio_location == sink_audio_location)
    {
        return true;
    }
    /* check notify */
    if ((pacs_sink_audio_locations_char_prop & GATT_CHAR_PROP_NOTIFY) &&
        (ble_audio_db.dev_state.gap_init_state == GAP_INIT_STATE_STACK_READY) &&
        (p_pacs->sink_audio_loc_idx != 0))
    {
        uint8_t data[4];
        ble_audio_db.sink_audio_location = sink_audio_location;
        LE_UINT32_TO_ARRAY(data, sink_audio_location);
        return pacs_send_all_notify(PACS_SINK_AUDIO_LOCATIONS_CCCD_FLAG, p_pacs->sink_audio_loc_idx,
                                    data, 4);
    }
    else
    {
        PROTOCOL_PRINT_ERROR2("[BAP][PACS] pacs_update_sink_audio_locations: invalid state, char_prop 0x%x, gap_init_state %d",
                              pacs_sink_audio_locations_char_prop,
                              ble_audio_db.dev_state.gap_init_state);
        return false;
    }
}
bool pacs_update_source_audio_locations(uint32_t source_audio_location)
{
    if (ble_audio_db.source_audio_location == source_audio_location)
    {
        return true;
    }
    /* check notify */
    if ((pacs_source_audio_locations_char_prop & GATT_CHAR_PROP_NOTIFY) &&
        (ble_audio_db.dev_state.gap_init_state == GAP_INIT_STATE_STACK_READY) &&
        (p_pacs->source_audio_loc_idx != 0))
    {
        uint8_t data[4];
        ble_audio_db.source_audio_location = source_audio_location;
        LE_UINT32_TO_ARRAY(data, source_audio_location);
        return pacs_send_all_notify(PACS_SOURCE_AUDIO_LOCATIONS_CCCD_FLAG, p_pacs->source_audio_loc_idx,
                                    data, 4);
    }
    else
    {
        PROTOCOL_PRINT_ERROR2("[BAP][PACS] pacs_update_source_audio_locations: invalid state, char_prop 0x%x, gap_init_state %d",
                              pacs_source_audio_locations_char_prop,
                              ble_audio_db.dev_state.gap_init_state);
        return false;
    }
}

bool pacs_update_supported_contexts(uint16_t sink_supported_contexts,
                                    uint16_t source_supported_contexts)
{
    if ((ble_audio_db.sink_supported_contexts == sink_supported_contexts) &&
        (ble_audio_db.source_supported_contexts == source_supported_contexts))
    {
        return  true;
    }
    /* check notify */
    if ((pacs_supported_audio_contexts_char_prop & GATT_CHAR_PROP_NOTIFY) &&
        (ble_audio_db.dev_state.gap_init_state == GAP_INIT_STATE_STACK_READY))
    {
        uint8_t data[4];
        ble_audio_db.sink_supported_contexts = sink_supported_contexts;
        ble_audio_db.source_supported_contexts = source_supported_contexts;
        LE_UINT16_TO_ARRAY(data, sink_supported_contexts);
        LE_UINT16_TO_ARRAY(data + 2, source_supported_contexts);
        return pacs_send_all_notify(PACS_SUPPORTED_AUDIO_CONTEXTS_CCCD_FLAG,
                                    PACS_CHAR_SUPPORTED_AUDIO_CONTEXTS_INDEX,
                                    data, 4);
    }
    else
    {
        PROTOCOL_PRINT_ERROR2("[BAP][PACS] pacs_update_supported_contexts: invalid state, char_prop 0x%x, gap_init_state %d",
                              pacs_supported_audio_contexts_char_prop,
                              ble_audio_db.dev_state.gap_init_state);
        return false;
    }
}

bool pacs_available_contexts_read_cfm(uint16_t conn_handle, uint16_t cid,
                                      uint16_t sink_available_contexts,
                                      uint16_t source_available_contexts)
{
    uint8_t data[4];
    LE_UINT16_TO_ARRAY(data, sink_available_contexts);
    LE_UINT16_TO_ARRAY(data + 2, source_available_contexts);
    return gatt_svc_read_confirm(conn_handle, cid,
                                 p_pacs->service_id,
                                 PACS_CHAR_AUDIO_CONTEXTS_AVA_INDEX,
                                 data, 4, 4, APP_RESULT_SUCCESS);
}

bool pacs_update_available_contexts(uint16_t conn_handle, uint16_t sink_available_contexts,
                                    uint16_t source_available_contexts)
{
    uint8_t data[4];
    LE_UINT16_TO_ARRAY(data, sink_available_contexts);
    LE_UINT16_TO_ARRAY(data + 2, source_available_contexts);
    return pacs_send_notify(conn_handle, PACS_CHAR_AUDIO_CONTEXTS_AVA_INDEX, data, 4);
}

void pacs_handle_pending_cccd(uint16_t conn_handle, T_LE_SRV_CFG *p_srv)
{
    uint16_t cccd_handle;

    PROTOCOL_PRINT_TRACE2("[BAP][PACS] pacs_handle_pending_cccd: start, conn_handle 0x%x, cccd_pending 0x%x",
                          conn_handle, p_srv->cccd_pending);
    if (p_srv->cccd_pending[0] & PACS_SINK_AUDIO_LOCATIONS_CCCD_FLAG)
    {
        uint8_t data[4];
        LE_UINT32_TO_ARRAY(data, ble_audio_db.sink_audio_location);
        if (pacs_send_notify(conn_handle, p_pacs->sink_audio_loc_idx, data, 4))
        {
            cccd_handle = server_get_start_handle(p_pacs->service_id);
            cccd_handle += (p_pacs->sink_audio_loc_idx + 1);
            if (ble_audio_ual_clear_pending_cccd(conn_handle, cccd_handle))
            {
                p_srv->cccd_pending[0] &= ~PACS_SINK_AUDIO_LOCATIONS_CCCD_FLAG;
            }
        }
    }
    if (p_srv->cccd_pending[0] & PACS_SOURCE_AUDIO_LOCATIONS_CCCD_FLAG)
    {
        uint8_t data[4];
        LE_UINT32_TO_ARRAY(data, ble_audio_db.source_audio_location);
        if (pacs_send_notify(conn_handle, p_pacs->source_audio_loc_idx, data, 4))
        {
            cccd_handle = server_get_start_handle(p_pacs->service_id);
            cccd_handle += (p_pacs->source_audio_loc_idx + 1);
            if (ble_audio_ual_clear_pending_cccd(conn_handle, cccd_handle))
            {
                p_srv->cccd_pending[0] &= ~PACS_SOURCE_AUDIO_LOCATIONS_CCCD_FLAG;
            }
        }
    }
    if (p_srv->cccd_pending[0] & PACS_SUPPORTED_AUDIO_CONTEXTS_CCCD_FLAG)
    {
        uint8_t data[4];
        LE_UINT16_TO_ARRAY(data, ble_audio_db.sink_supported_contexts);
        LE_UINT16_TO_ARRAY(data + 2, ble_audio_db.source_supported_contexts);
        if (pacs_send_notify(conn_handle, PACS_CHAR_SUPPORTED_AUDIO_CONTEXTS_INDEX, data, 4))
        {
            cccd_handle = server_get_start_handle(p_pacs->service_id);
            cccd_handle += (PACS_CHAR_SUPPORTED_AUDIO_CONTEXTS_INDEX + 1);
            if (ble_audio_ual_clear_pending_cccd(conn_handle, cccd_handle))
            {
                p_srv->cccd_pending[0] &= ~PACS_SUPPORTED_AUDIO_CONTEXTS_CCCD_FLAG;
            }
        }
    }
    if (p_srv->cccd_pending[0] & PACS_PAC_CCCD_FLAG_MASK)
    {
        uint8_t pac_id = 0;
        uint16_t pac_cccd_flag;

        for (; pac_id < pac_list_num; pac_id++)
        {
            pac_cccd_flag = 0x0001 << pac_id;
            if (p_srv->cccd_pending[0] & pac_cccd_flag)
            {
                T_PAC_RECORD *p_record;
                p_record = pac_find_by_id(pac_id);
                if (p_record != NULL)
                {
                    if (pacs_send_notify(conn_handle, p_record->attrib_idx, p_record->pac_data, p_record->pac_data_len))
                    {
                        cccd_handle = server_get_start_handle(p_pacs->service_id);
                        cccd_handle += (p_record->attrib_idx + 1);
                        if (ble_audio_ual_clear_pending_cccd(conn_handle, cccd_handle))
                        {
                            p_srv->cccd_pending[0] &= ~pac_cccd_flag;
                        }
                    }
                }
            }
        }
    }
    PROTOCOL_PRINT_TRACE2("[BAP][PACS] pacs_handle_pending_cccd: end, conn_handle 0x%x, cccd_pending 0x%x",
                          conn_handle, p_srv->cccd_pending);
}

static void pacs_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BLE_DM_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
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
                    p_srv = ble_srv_find_by_srv_id(p_link->conn_handle, p_pacs->service_id);
                    if (p_srv != NULL && p_srv->cccd_pending[0] != 0)
                    {
                        pacs_handle_pending_cccd(p_link->conn_handle, p_srv);
                    }
                }
            }
        }
        break;
    default:
        break;
    }
}

T_APP_RESULT pacs_attr_read_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                               uint16_t attrib_index,
                               uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;

    PROTOCOL_PRINT_INFO2("pacs_attr_read_cb: attrib_index %d, offset %d", attrib_index, offset);
    if (attrib_index == PACS_CHAR_AUDIO_CONTEXTS_AVA_INDEX)
    {
        T_PACS_READ_AVAILABLE_CONTEXTS_IND read_ind;
        read_ind.conn_handle = conn_handle;
        read_ind.cid = cid;
        ble_audio_mgr_dispatch(LE_AUDIO_MSG_PACS_READ_AVAILABLE_CONTEXTS_IND, &read_ind);
        cause = APP_RESULT_PENDING;
    }
    else if (attrib_index == PACS_CHAR_SUPPORTED_AUDIO_CONTEXTS_INDEX)
    {
        LE_UINT16_TO_ARRAY(pacs_read_data, ble_audio_db.sink_supported_contexts);
        LE_UINT16_TO_ARRAY(pacs_read_data + 2, ble_audio_db.source_supported_contexts);
        *pp_value = pacs_read_data;
        *p_length = 4;
    }
    else if (attrib_index == p_pacs->sink_audio_loc_idx)
    {
        LE_UINT32_TO_ARRAY(pacs_read_data, ble_audio_db.sink_audio_location);
        *pp_value = pacs_read_data;
        *p_length = 4;
    }
    else if (attrib_index == p_pacs->source_audio_loc_idx)
    {
        LE_UINT32_TO_ARRAY(pacs_read_data, ble_audio_db.source_audio_location);
        *pp_value = pacs_read_data;
        *p_length = 4;
    }
    else if ((attrib_index >= p_pacs->sink_pac_start_idx) && (attrib_index <= p_pacs->sink_pac_end_idx))
    {
        if (pacs_gen_pac_records(attrib_index,
                                 pp_value, p_length) == false)
        {
            return APP_RESULT_ATTR_NOT_FOUND;
        }
    }
    else if ((attrib_index >= p_pacs->source_pac_start_idx) &&
             (attrib_index <= p_pacs->source_pac_end_idx))
    {
        if (pacs_gen_pac_records(attrib_index,
                                 pp_value, p_length) == false)
        {
            return APP_RESULT_ATTR_NOT_FOUND;
        }
    }
    else
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    return (cause);
}

T_APP_RESULT pacs_attr_write_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                uint16_t attrib_index, T_WRITE_TYPE write_type,
                                uint16_t length, uint8_t *p_value, P_FUN_EXT_WRITE_IND_POST_PROC *p_write_post_proc)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    uint32_t locations = 0;

    PROTOCOL_PRINT_INFO2("pacs_attr_write_cb: attrib_index %d, length %x", attrib_index, length);
    if (attrib_index == p_pacs->sink_audio_loc_idx)
    {
        if (length != 4)
        {
            return (T_APP_RESULT)(ATT_ERR | ATT_ERR_WRITE_REQUEST_REJECTED);
        }
        LE_ARRAY_TO_UINT32(locations, p_value);
        PROTOCOL_PRINT_INFO1("[BAP][PACS] PACS_WRITE_SINK_AUDIO_LOCATIONS: locations 0x%x",
                             locations);
        if (locations == 0 || (locations | AUDIO_LOCATION_MASK) != AUDIO_LOCATION_MASK)
        {
            return (T_APP_RESULT)(ATT_ERR | ATT_ERR_WRITE_REQUEST_REJECTED);
        }
        if (ble_audio_db.sink_audio_location != locations)
        {
            T_PACS_WRITE_SINK_AUDIO_LOC_IND sink_loc;
            sink_loc.conn_handle = conn_handle;
            sink_loc.sink_audio_locations = locations;
            cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_PACS_WRITE_SINK_AUDIO_LOC_IND,
                                                         &sink_loc);
            if (APP_RESULT_SUCCESS == cause)
            {
                ble_audio_db.sink_audio_location = locations;
                *p_write_post_proc = pacs_write_sink_audio_location_post_proc;
            }
        }
    }
    else if (attrib_index == p_pacs->source_audio_loc_idx)
    {
        if (length != 4)
        {
            return (T_APP_RESULT)(ATT_ERR | ATT_ERR_WRITE_REQUEST_REJECTED);
        }
        LE_ARRAY_TO_UINT32(locations, p_value);

        PROTOCOL_PRINT_INFO1("[BAP][PACS] PACS_WRITE_SOURCE_AUDIO_LOCATIONS: locations 0x%x",
                             locations);
        if (locations == 0 || (locations | AUDIO_LOCATION_MASK) != AUDIO_LOCATION_MASK)
        {
            return (T_APP_RESULT)(ATT_ERR | ATT_ERR_WRITE_REQUEST_REJECTED);
        }
        if (ble_audio_db.source_audio_location != locations)
        {
            T_PACS_WRITE_SOURCE_AUDIO_LOC_IND source_loc;

            source_loc.conn_handle = conn_handle;
            source_loc.source_audio_locations = locations;
            cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_PACS_WRITE_SOURCE_AUDIO_LOC_IND,
                                                         &source_loc);
            if (APP_RESULT_SUCCESS == cause)
            {
                ble_audio_db.source_audio_location = locations;
                *p_write_post_proc = pacs_write_source_audio_location_post_proc;
            }
        }
    }
    else
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    return (cause);
}

void pacs_cccd_update_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                         uint16_t attrib_index,
                         uint16_t ccc_bits)
{
    T_PAC_RECORD *p_pac = NULL;
    T_LE_SRV_CFG *p_srv = ble_srv_alloc_by_srv_id(conn_handle, service_id);
    if (p_srv == NULL)
    {
        return;
    }

    PROTOCOL_PRINT_INFO2("pacs_cccd_update_cb: attrib_index %d, ccc_bits 0x%x", attrib_index, ccc_bits);
    if (attrib_index == PACS_CHAR_AUDIO_CONTEXTS_AVA_CCCD_INDEX)
    {
        PROTOCOL_PRINT_INFO1("[BAP][PACS] PACS_AVA_AUDIO_CONTEXTS_CCCD: ccc_bits 0x%x",
                             ccc_bits);
        ble_srv_set_cccd_flags(p_srv, ccc_bits, 0,
                               PACS_AUDIO_AVAILABILITY_CONTEXTS_CCCD_FLAG, true);
    }
    else if (attrib_index == PACS_CHAR_SUPPORTED_AUDIO_CONTEXTS_CCCD_INDEX)
    {
        PROTOCOL_PRINT_INFO1("[BAP][PACS] PACS_SUPPORTED_AUDIO_CONTEXTS_CCCD: ccc_bits 0x%x",
                             ccc_bits);
        ble_srv_set_cccd_flags(p_srv, ccc_bits, 0,
                               PACS_SUPPORTED_AUDIO_CONTEXTS_CCCD_FLAG, true);
    }
    else if (attrib_index == p_pacs->sink_audio_loc_cccd_idx)
    {
        PROTOCOL_PRINT_INFO1("[BAP][PACS] PACS_SINK_AUDIO_LOCATIONS_CCCD: ccc_bits 0x%x",
                             ccc_bits);
        ble_srv_set_cccd_flags(p_srv, ccc_bits, 0,
                               PACS_SINK_AUDIO_LOCATIONS_CCCD_FLAG, true);
    }
    else if (attrib_index == p_pacs->source_audio_loc_cccd_idx)
    {
        PROTOCOL_PRINT_INFO1("[BAP][PACS] PACS_SOURCE_AUDIO_LOCATIONS_CCCD: ccc_bits 0x%x",
                             ccc_bits);
        ble_srv_set_cccd_flags(p_srv, ccc_bits, 0,
                               PACS_SOURCE_AUDIO_LOCATIONS_CCCD_FLAG, true);
    }
    else if ((attrib_index > p_pacs->sink_pac_start_idx) && (attrib_index <= p_pacs->sink_pac_end_idx))
    {
        p_pac = pac_find_by_index(attrib_index, true);
        if (p_pac == NULL)
        {
            return;
        }
    }
    else if ((attrib_index > p_pacs->source_pac_start_idx) &&
             (attrib_index <= p_pacs->source_pac_end_idx))
    {
        p_pac = pac_find_by_index(attrib_index, true);
        if (p_pac == NULL)
        {
            return;
        }
    }
    if (p_pac)
    {
        uint16_t pac_cccd_flag = 0x0001;
        pac_cccd_flag = pac_cccd_flag <<  p_pac->pac_id;
        PROTOCOL_PRINT_INFO3("[BAP][PACS] PACS PAC CCCD: pac_id %d,ccc_bits 0x%x, pac_cccd_flag 0x%x",
                             p_pac->pac_id,
                             ccc_bits,
                             pac_cccd_flag);
        ble_srv_set_cccd_flags(p_srv, ccc_bits, 0, pac_cccd_flag, true);
    }
    ble_audio_mgr_send_server_cccd_info(conn_handle, p_pacs->p_pacs_attr_tbl, service_id, attrib_index,
                                        ccc_bits, 0, p_pacs->attr_num);
    return;
}

const T_FUN_GATT_EXT_SERVICE_CBS pacs_cbs =
{
    pacs_attr_read_cb,  // Read callback function pointer
    pacs_attr_write_cb, // Write callback function pointer
    pacs_cccd_update_cb  // CCCD update callback function pointer
};

bool pacs_init(T_PACS_PARAMS *p_param)
{
    if (p_param == NULL || p_pacs != NULL)
    {
        goto failed;
    }
#if LE_AUDIO_INIT_DEBUG
    PROTOCOL_PRINT_INFO4("pacs_init: sink_locations, is_exist %d, is_notify %d, is_write %d, sink_audio_location 0x%x",
                         p_param->sink_locations.is_exist,
                         p_param->sink_locations.is_notify,
                         p_param->sink_locations.is_write,
                         p_param->sink_locations.sink_audio_location);
    PROTOCOL_PRINT_INFO4("pacs_init: source_locations, is_exist %d, is_notify %d, is_write %d, source_audio_location 0x%x",
                         p_param->source_locations.is_exist,
                         p_param->source_locations.is_notify,
                         p_param->source_locations.is_write,
                         p_param->source_locations.source_audio_location);
    PROTOCOL_PRINT_INFO3("pacs_init: supported_contexts, is_notify %d, sink_supported_contexts 0x%x, source_supported_contexts 0x%x",
                         p_param->supported_contexts.is_notify,
                         p_param->supported_contexts.sink_supported_contexts,
                         p_param->supported_contexts.source_supported_contexts);
#endif
    p_pacs = ble_audio_mem_zalloc(sizeof(T_PACS));
    if (p_pacs == NULL)
    {
        goto failed;
    }
    if (p_param->sink_locations.is_exist == false)
    {
        pacs_sink_audio_locations_char_prop = 0;
    }
    else
    {
        if (p_param->sink_locations.is_notify)
        {
            pacs_sink_audio_locations_char_prop |= GATT_CHAR_PROP_NOTIFY;
        }
        if (p_param->sink_locations.is_write)
        {
            pacs_sink_audio_locations_char_prop |= GATT_CHAR_PROP_WRITE;
        }
        ble_audio_db.sink_audio_location = p_param->sink_locations.sink_audio_location;
    }
    if (p_param->source_locations.is_exist == false)
    {
        pacs_source_audio_locations_char_prop = 0;
    }
    else
    {
        if (p_param->source_locations.is_notify)
        {
            pacs_source_audio_locations_char_prop |= GATT_CHAR_PROP_NOTIFY;
        }
        if (p_param->source_locations.is_write)
        {
            pacs_source_audio_locations_char_prop |= GATT_CHAR_PROP_WRITE;
        }
        ble_audio_db.source_audio_location = p_param->source_locations.source_audio_location;
    }
    if (p_param->supported_contexts.is_notify)
    {
        pacs_supported_audio_contexts_char_prop = (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY);
    }
    else
    {
        pacs_supported_audio_contexts_char_prop = GATT_CHAR_PROP_READ;
    }
    ble_audio_db.sink_supported_contexts = p_param->supported_contexts.sink_supported_contexts;
    ble_audio_db.source_supported_contexts = p_param->supported_contexts.source_supported_contexts;

    if (!pacs_add_service(&pacs_cbs))
    {
        goto failed;
    }

    ble_dm_cback_register(pacs_dm_cback);
#if LE_AUDIO_INIT_DEBUG
#if LE_AUDIO_DEBUG
    pac_print_info();
#endif
#endif
    return true;
failed:
    PROTOCOL_PRINT_ERROR0("[BAP][PACS] pacs_init: failed");
    return false;
}

#if LE_AUDIO_DEINIT
void pacs_deinit(void)
{
    T_PAC_RECORD_LIST *p_loop = p_pac_list;

    pac_list_num = 0;
    sink_pac_num = 0;
    source_pac_num = 0;
    pac_char_attrib_num = 0;

    while (p_loop != NULL)
    {
        T_PAC_RECORD_LIST *p_temp_loop = p_loop;
        p_loop = p_loop->next;
        ble_audio_mem_free(p_temp_loop);
    }
    p_pac_list = NULL;

    if (p_pacs)
    {
        if (p_pacs->p_pacs_attr_tbl)
        {
            ble_audio_mem_free(p_pacs->p_pacs_attr_tbl);
        }
        ble_audio_mem_free(p_pacs);
        p_pacs = NULL;
        pacs_sink_audio_locations_char_prop = GATT_CHAR_PROP_READ;
        pacs_source_audio_locations_char_prop = GATT_CHAR_PROP_READ;
        pacs_supported_audio_contexts_char_prop = GATT_CHAR_PROP_READ;
    }
}
#endif
#endif
