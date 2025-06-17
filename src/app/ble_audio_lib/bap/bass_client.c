#include <string.h>
#include "trace.h"
#include "ble_audio_mgr.h"
#include "bass_client.h"
#include "ble_link_util.h"
#include "ble_audio.h"
#include "bt_gatt_client.h"
#include "bass_def.h"
#include "ble_audio_dm.h"
#include "bap_int.h"
#include "gap_past_sender.h"

#if LE_AUDIO_BASS_CLIENT_SUPPORT

typedef struct
{
    uint8_t instance_id;
    T_BASS_BRS_DATA data;
} T_REMOTE_BRS_DATA;

typedef struct
{
    uint8_t brs_num;
    T_REMOTE_BRS_DATA brs_data[1];
} T_BASS_CLIENT_DB;

#define bass_check_link(conn_handle) bass_check_link_int(__func__, conn_handle)

T_BASS_CLIENT_DB *bass_check_link_int(const char *p_func_name, uint16_t conn_handle)
{
    T_BLE_AUDIO_LINK *p_link = NULL;

    p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL || p_link->p_bass_client == NULL)
    {
        PROTOCOL_PRINT_ERROR2("bass_check_link: %s failed, conn_handle 0x%x", TRACE_STRING(p_func_name),
                              conn_handle);
        return NULL;
    }
    return p_link->p_bass_client;
}

T_REMOTE_BRS_DATA *bass_find_brs_data(T_BASS_CLIENT_DB *p_db, uint8_t char_instance_id)
{
    if (p_db && char_instance_id < p_db->brs_num)
    {
        return &p_db->brs_data[char_instance_id];
    }
    return NULL;
}

bool bass_write_bas_cp(uint16_t conn_handle, bool is_req, uint16_t data_len, uint8_t *p_data)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    T_GATT_WRITE_TYPE write_type = GATT_WRITE_TYPE_REQ;
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL)
    {
        return false;
    }

    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_BASS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = BASS_UUID_CHAR_BROADCAST_AUDIO_SCAN_CP;

    if (!is_req)
    {
        if (data_len > p_link->mtu_size - 3)
        {
            write_type = GATT_WRITE_TYPE_REQ;
        }
        else
        {
            write_type = GATT_WRITE_TYPE_CMD;
        }
    }

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, write_type, handle, data_len, p_data, NULL);
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

void bass_enable_cccd(uint16_t conn_handle)
{
    T_ATTR_UUID srv_uuid;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_BASS;
    if (gatt_client_check_cccd_enabled(conn_handle, &srv_uuid, NULL) == false)
    {
        gatt_client_enable_srv_cccd(conn_handle, &srv_uuid, GATT_CLIENT_CONFIG_ALL);
    }
}
bool bass_read_brs_value(uint16_t conn_handle, uint8_t instance_id)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_BASS;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = instance_id;
    char_uuid.p.uuid16 = BASS_UUID_CHAR_BROADCAST_RECEIVE_STATE;
    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_read(conn_handle, handle, NULL);
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

bool bass_cp_remote_scan_stop(uint16_t conn_handle, bool is_req)
{
    uint8_t data[1];
    data[0] = BASS_CP_OP_REMOTE_SCAN_STOP;

    return bass_write_bas_cp(conn_handle, is_req, 1, data);
}

bool bass_cp_remote_scan_start(uint16_t conn_handle, bool is_req)
{
    uint8_t data[1];
    data[0] = BASS_CP_OP_REMOTE_SCAN_START;

    return bass_write_bas_cp(conn_handle, is_req, 1, data);
}

bool bass_cp_add_source(uint16_t conn_handle, T_BASS_CP_ADD_SOURCE *p_cp_data, bool is_req)
{
    uint8_t *p_data = NULL;
    uint16_t idx = 0;
    uint8_t i;
    bool ret = true;
    uint16_t mem_size = BASS_CP_OP_ADD_SOURCE_MIN_LEN + p_cp_data->bis_info_size;

    p_data = ble_audio_mem_zalloc(mem_size);

    if (p_data == NULL)
    {
        return false;
    }

    p_data[idx] = BASS_CP_OP_ADD_SOURCE;
    idx++;
    p_data[idx] = p_cp_data->advertiser_address_type;
    idx++;
    memcpy(&p_data[idx], p_cp_data->advertiser_address, GAP_BD_ADDR_LEN);
    idx += GAP_BD_ADDR_LEN;
    p_data[idx] = p_cp_data->advertiser_sid;
    idx++;
    memcpy(&p_data[idx], p_cp_data->broadcast_id, BROADCAST_ID_LEN);
    idx += BROADCAST_ID_LEN;
    p_data[idx] = p_cp_data->pa_sync;
    idx++;
    LE_UINT16_TO_ARRAY(&p_data[idx], p_cp_data->pa_interval);
    idx += 2;
    p_data[idx] = p_cp_data->num_subgroups;
    idx++;
    for (i = 0; i < p_cp_data->num_subgroups; i++)
    {
        LE_UINT32_TO_ARRAY(&p_data[idx], p_cp_data->p_cp_bis_info[i].bis_sync);
        idx += 4;
        p_data[idx] = p_cp_data->p_cp_bis_info[i].metadata_len;
        idx++;
        if ((idx + p_cp_data->p_cp_bis_info[i].metadata_len) > mem_size)
        {
            ret = false;
            break;
        }
        if (p_cp_data->p_cp_bis_info[i].metadata_len > 0)
        {
            memcpy(p_data + idx, p_cp_data->p_cp_bis_info[i].p_metadata,
                   p_cp_data->p_cp_bis_info[i].metadata_len);
            idx += p_cp_data->p_cp_bis_info[i].metadata_len;
        }
    }
    if (ret)
    {
        ret = bass_write_bas_cp(conn_handle, is_req, idx, p_data);
    }
    ble_audio_mem_free(p_data);
    return ret;
}

bool bass_cp_add_source_by_sync_info(T_BLE_AUDIO_SYNC_HANDLE handle, uint16_t conn_handle,
                                     T_BASS_PA_SYNC pa_sync, uint32_t bis_array, bool is_req)
{
    uint8_t err_idx = 0;
    T_BLE_AUDIO_SYNC_INFO sync_info;
    T_BASS_CP_ADD_SOURCE cp_data = {0};
    uint16_t meta_data_len = 0;
    uint32_t bis_array_temp = 0;
    uint8_t i;
    T_BASS_CP_BIS_INFO *p_cp_bis_info = NULL;
    T_BASS_CP_BIS_INFO cp_bis_info;

    if (ble_audio_sync_get_info(handle, &sync_info) == false)
    {
        err_idx = 1;
        goto error;
    }

    cp_data.advertiser_address_type = sync_info.advertiser_address_type;
    memcpy(cp_data.advertiser_address, sync_info.advertiser_address, GAP_BD_ADDR_LEN);
    cp_data.advertiser_sid = sync_info.adv_sid;
    memcpy(cp_data.broadcast_id, sync_info.broadcast_id, BROADCAST_ID_LEN);
    cp_data.pa_sync = pa_sync;
    if (sync_info.pa_interval == 0)
    {
        cp_data.pa_interval = BASS_PA_INTERVAL_UNKNOWN;
    }
    else
    {
        cp_data.pa_interval = sync_info.pa_interval;
    }

    if (sync_info.p_base_mapping == NULL)
    {
        cp_bis_info.bis_sync = bis_array;
        cp_bis_info.metadata_len = 0;
        cp_bis_info.p_metadata = NULL;
        cp_data.p_cp_bis_info = &cp_bis_info;
        cp_data.num_subgroups = 1;
        cp_data.bis_info_size = cp_data.num_subgroups * sizeof(T_BASS_CP_BIS_INFO) + meta_data_len;
    }
    else
    {
        for (i = 0; i < sync_info.p_base_mapping->num_subgroups; i++)
        {
            if (sync_info.p_base_mapping->p_subgroup[i].bis_array & bis_array)
            {
                bis_array_temp |= (sync_info.p_base_mapping->p_subgroup[i].bis_array & bis_array);
            }
            meta_data_len += sync_info.p_base_mapping->p_subgroup[i].metadata_len;
        }
        if (bis_array != BASS_CP_BIS_SYNC_NO_PREFER && bis_array != bis_array_temp)
        {
            err_idx = 2;
            goto error;
        }
        cp_data.bis_info_size = sync_info.p_base_mapping->num_subgroups * sizeof(
                                    T_BASS_CP_BIS_INFO) + meta_data_len;
        p_cp_bis_info = ble_audio_mem_zalloc(cp_data.bis_info_size);
        if (p_cp_bis_info == NULL)
        {
            err_idx = 3;
            goto error;
        }

        uint8_t *p_meta_data = (uint8_t *)(p_cp_bis_info + sync_info.p_base_mapping->num_subgroups);
        for (i = 0; i < sync_info.p_base_mapping->num_subgroups; i++)
        {
            if (bis_array == BASS_CP_BIS_SYNC_NO_PREFER)
            {
                p_cp_bis_info[i].bis_sync = BASS_CP_BIS_SYNC_NO_PREFER;
            }
            else
            {
                p_cp_bis_info[i].bis_sync = (sync_info.p_base_mapping->p_subgroup[i].bis_array & bis_array);
            }
            p_cp_bis_info[i].metadata_len = sync_info.p_base_mapping->p_subgroup[i].metadata_len;
            if (p_cp_bis_info[i].metadata_len > 0)
            {
                memcpy(p_meta_data, sync_info.p_base_mapping->p_subgroup[i].p_metadata,
                       p_cp_bis_info[i].metadata_len);
                p_cp_bis_info[i].p_metadata = p_meta_data;
                p_meta_data += p_cp_bis_info->metadata_len;
            }
        }
        cp_data.p_cp_bis_info = p_cp_bis_info;
        cp_data.num_subgroups = sync_info.p_base_mapping->num_subgroups;
    }

    if (bass_cp_add_source(conn_handle, &cp_data, is_req) == false)
    {
        err_idx = 4;
        goto error;
    }
    if (p_cp_bis_info)
    {
        ble_audio_mem_free(p_cp_bis_info);
    }
    return true;
error:
    if (p_cp_bis_info)
    {
        ble_audio_mem_free(p_cp_bis_info);
    }
    PROTOCOL_PRINT_ERROR5("bass_cp_add_source_by_sync_info: failed, handle %p, conn_handle 0x%x, pa_sync %d, bis_array 0x%x, err_idx %d",
                          handle, conn_handle, pa_sync, bis_array, err_idx);
    return false;
}

bool bass_cp_modify_source(uint16_t conn_handle, T_BASS_CP_MODIFY_SOURCE *p_cp_data, bool is_req)
{
    uint8_t *p_data = NULL;
    uint16_t idx = 0;
    uint8_t i;
    bool ret = true;
    uint16_t mem_size = BASS_CP_OP_MODIFY_SOURCE_MIN_LEN + p_cp_data->bis_info_size;

    p_data = ble_audio_mem_zalloc(BASS_CP_OP_MODIFY_SOURCE_MIN_LEN + p_cp_data->bis_info_size);

    if (p_data == NULL)
    {
        PROTOCOL_PRINT_ERROR1("bass_cp_modify_source alloc buffer fail %d", mem_size);
        return false;
    }

    p_data[idx] = BASS_CP_OP_MODIFY_SOURCE;
    idx++;
    p_data[idx] = p_cp_data->source_id;
    idx++;

    p_data[idx] = p_cp_data->pa_sync;
    idx++;
    LE_UINT16_TO_ARRAY(&p_data[idx], p_cp_data->pa_interval);
    idx += 2;
    p_data[idx] = p_cp_data->num_subgroups;
    idx++;
    for (i = 0; i < p_cp_data->num_subgroups; i++)
    {
        LE_UINT32_TO_ARRAY(&p_data[idx], p_cp_data->p_cp_bis_info[i].bis_sync);
        idx += 4;
        p_data[idx] = p_cp_data->p_cp_bis_info[i].metadata_len;
        idx++;
        if ((idx + p_cp_data->p_cp_bis_info[i].metadata_len) > mem_size)
        {
            ret = false;
            PROTOCOL_PRINT_ERROR1("bass_cp_modify_source alloc mem_size too small %d", mem_size);
            break;
        }
        if (p_cp_data->p_cp_bis_info[i].metadata_len > 0)
        {
            memcpy(p_data + idx, p_cp_data->p_cp_bis_info[i].p_metadata,
                   p_cp_data->p_cp_bis_info[i].metadata_len);
            idx += p_cp_data->p_cp_bis_info[i].metadata_len;
        }
    }
    if (ret)
    {
        ret = bass_write_bas_cp(conn_handle, is_req, idx, p_data);
    }
    ble_audio_mem_free(p_data);
    return ret;
}

bool bass_cp_modify_source_by_sync_info(T_BLE_AUDIO_SYNC_HANDLE handle, uint16_t conn_handle,
                                        uint8_t source_id,
                                        T_BASS_PA_SYNC pa_sync, uint32_t bis_array, bool is_req)
{
    uint8_t err_idx = 0;
    T_BLE_AUDIO_SYNC_INFO sync_info;
    T_BASS_CP_MODIFY_SOURCE cp_data = {0};
    uint16_t meta_data_len = 0;
    uint32_t bis_array_temp = 0;
    uint8_t i;
    T_BASS_CP_BIS_INFO *p_cp_bis_info = NULL;
    T_BASS_CP_BIS_INFO cp_bis_info;

    if (ble_audio_sync_get_info(handle, &sync_info) == false)
    {
        err_idx = 1;
        goto error;
    }

    cp_data.source_id = source_id;
    cp_data.pa_sync = pa_sync;
    if (sync_info.pa_interval == 0)
    {
        cp_data.pa_interval = BASS_PA_INTERVAL_UNKNOWN;
    }
    else
    {
        cp_data.pa_interval = sync_info.pa_interval;
    }

    if (sync_info.p_base_mapping == NULL)
    {
        cp_bis_info.bis_sync = bis_array;
        cp_bis_info.metadata_len = 0;
        cp_bis_info.p_metadata = NULL;
        cp_data.p_cp_bis_info = &cp_bis_info;
        cp_data.num_subgroups = 1;
        cp_data.bis_info_size = cp_data.num_subgroups * sizeof(T_BASS_CP_BIS_INFO) + meta_data_len;
    }
    else
    {
        for (i = 0; i < sync_info.p_base_mapping->num_subgroups; i++)
        {
            if (sync_info.p_base_mapping->p_subgroup[i].bis_array & bis_array)
            {
                bis_array_temp |= (sync_info.p_base_mapping->p_subgroup[i].bis_array & bis_array);
            }
            meta_data_len += sync_info.p_base_mapping->p_subgroup[i].metadata_len;
        }
        if (bis_array != BASS_CP_BIS_SYNC_NO_PREFER && bis_array != bis_array_temp)
        {
            err_idx = 2;
            goto error;
        }
        cp_data.bis_info_size = sync_info.p_base_mapping->num_subgroups * sizeof(
                                    T_BASS_CP_BIS_INFO) + meta_data_len;
        p_cp_bis_info = ble_audio_mem_zalloc(cp_data.bis_info_size);
        if (p_cp_bis_info == NULL)
        {
            err_idx = 3;
            goto error;
        }

        uint8_t *p_meta_data = (uint8_t *)(p_cp_bis_info + sync_info.p_base_mapping->num_subgroups);
        for (i = 0; i < sync_info.p_base_mapping->num_subgroups; i++)
        {
            if (bis_array == BASS_CP_BIS_SYNC_NO_PREFER)
            {
                p_cp_bis_info[i].bis_sync = BASS_CP_BIS_SYNC_NO_PREFER;
            }
            else
            {
                p_cp_bis_info[i].bis_sync = (sync_info.p_base_mapping->p_subgroup[i].bis_array & bis_array);
            }
            p_cp_bis_info[i].metadata_len = sync_info.p_base_mapping->p_subgroup[i].metadata_len;
            if (p_cp_bis_info[i].metadata_len > 0)
            {
                memcpy(p_meta_data, sync_info.p_base_mapping->p_subgroup[i].p_metadata,
                       p_cp_bis_info[i].metadata_len);
                p_cp_bis_info[i].p_metadata = p_meta_data;
                p_meta_data += p_cp_bis_info->metadata_len;
            }
        }
        cp_data.p_cp_bis_info = p_cp_bis_info;
        cp_data.num_subgroups = sync_info.p_base_mapping->num_subgroups;
    }

    if (bass_cp_modify_source(conn_handle, &cp_data, is_req) == false)
    {
        err_idx = 4;
        goto error;
    }
    if (p_cp_bis_info)
    {
        ble_audio_mem_free(p_cp_bis_info);
    }
    return true;
error:
    if (p_cp_bis_info)
    {
        ble_audio_mem_free(p_cp_bis_info);
    }
    PROTOCOL_PRINT_ERROR5("bass_cp_modify_source_by_sync_info: failed, handle %p, conn_handle 0x%x, pa_sync %d, bis_array 0x%x, err_idx %d",
                          handle, conn_handle, pa_sync, bis_array, err_idx);
    return false;
}

bool bass_cp_set_broadcast_code(uint16_t conn_handle, T_BASS_CP_SET_BROADCAST_CODE *p_cp_data,
                                bool is_req)
{
    uint8_t data[BASS_CP_OP_SET_BROADCAST_CODE_LEN];
    uint8_t idx = 0;

    data[idx] = BASS_CP_OP_SET_BROADCAST_CODE;
    idx++;
    data[idx] = p_cp_data->source_id;
    idx++;
    memcpy(&data[idx], p_cp_data->broadcast_code, BROADCAST_CODE_LEN);
    idx += BROADCAST_CODE_LEN;
    return bass_write_bas_cp(conn_handle, is_req, idx, data);
}

bool bass_cp_remove_source(uint16_t conn_handle, T_BASS_CP_REMOVE_SOURCE *p_cp_data, bool is_req)
{
    uint8_t data[BASS_CP_OP_REMOVE_SOURCE_LEN];
    uint8_t idx = 0;

    data[idx] = BASS_CP_OP_REMOVE_SOURCE;
    idx++;
    data[idx] = p_cp_data->source_id;
    idx++;
    return bass_write_bas_cp(conn_handle, is_req, idx, data);

}
#if F_BT_LE_5_1_PAST_SENDER_SYNC_SUPPORT
bool bass_past_by_remote_src(T_BLE_AUDIO_SYNC_HANDLE handle, uint16_t conn_handle,
                             T_BASS_PAST_SRV_DATA srv_data)
{
    T_BLE_AUDIO_SYNC_INFO sync_info;

    if (ble_audio_sync_get_info(handle, &sync_info))
    {
        uint16_t service_data = 0;
        uint8_t conn_id;
        le_get_conn_id_by_handle(conn_handle, &conn_id);
        service_data = srv_data.source_id << 8;
        service_data |= srv_data.adv_a_match_ext_adv;
        service_data |= (srv_data.adv_a_match_src << 1);
        if (le_past_sender_periodic_adv_sync_transfer(conn_id, service_data,
                                                      sync_info.sync_id) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
    }
    PROTOCOL_PRINT_ERROR2("bass_past_by_remote_src: failed, handle %p, conn_handle 0x%x",
                          handle, conn_handle);
    return false;
}
#endif
#if F_BT_LE_5_1_PAST_SENDER_ADV_SUPPORT
#if LE_AUDIO_BROADCAST_SOURCE_ROLE
bool bass_past_by_local_src(T_BROADCAST_SOURCE_HANDLE handle, uint16_t conn_handle,
                            T_BASS_PAST_SRV_DATA srv_data)
{
    T_BROADCAST_SOURCE_INFO src_info;

    if (broadcast_source_get_info(handle, &src_info))
    {
        uint16_t service_data = 0;
        uint8_t conn_id;
        le_get_conn_id_by_handle(conn_handle, &conn_id);
        service_data = srv_data.source_id << 8;
        service_data |= srv_data.adv_a_match_ext_adv;
        service_data |= (srv_data.adv_a_match_src << 1);
        if (le_past_sender_periodic_adv_set_info_transfer(conn_id, service_data,
                                                          src_info.adv_handle) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
    }
    PROTOCOL_PRINT_ERROR2("bass_past_by_local_src: failed, source handle %p, conn_handle 0x%x",
                          handle, conn_handle);
    return false;
}
#endif
#endif
bool bass_client_prase_brs(T_REMOTE_BRS_DATA *p_brs_data, uint8_t *p_data, uint16_t data_len)
{
    uint16_t metadata_len;
    uint16_t idx = 0;
    T_BASS_BRS_DATA brs_data = {0};
    uint8_t i;
    uint16_t size;
    uint8_t *p_meta_data;

    if (data_len == 0)
    {
        if (p_brs_data->data.p_cp_bis_info)
        {
            ble_audio_mem_free(p_brs_data->data.p_cp_bis_info);
            memset(&p_brs_data->data, 0, sizeof(T_BASS_BRS_DATA));
        }
        return true;
    }
    if (p_data == NULL || data_len < BASS_BRS_DATA_MIN_LEN)
    {
        goto error;
    }

    brs_data.brs_is_used = true;
    brs_data.source_id = p_data[idx++];
    brs_data.source_address_type = p_data[idx++];
    memcpy(brs_data.source_address, &p_data[idx], GAP_BD_ADDR_LEN);
    idx += GAP_BD_ADDR_LEN;
    brs_data.source_adv_sid = p_data[idx++];
    memcpy(brs_data.broadcast_id, &p_data[idx], BROADCAST_ID_LEN);
    idx += BROADCAST_ID_LEN;
    brs_data.pa_sync_state = (T_PA_SYNC_STATE)p_data[idx++];
    brs_data.big_encryption = (T_BIG_ENCRYPTION_STATE)p_data[idx++];
    if (brs_data.big_encryption == BIG_BAD_CODE)
    {
        if (data_len < (BASS_BRS_DATA_MIN_LEN + BROADCAST_CODE_LEN))
        {
            goto error;
        }
        memcpy(brs_data.bad_code, p_data + idx, BROADCAST_CODE_LEN);
        idx += BROADCAST_CODE_LEN;
    }
    brs_data.num_subgroups = p_data[idx++];
    size = 0;
    metadata_len = 0;
    for (i = 0; i < brs_data.num_subgroups; i++)
    {
        size += 5;
        if (data_len < (idx + size))
        {
            goto error;
        }
        metadata_len += p_data[idx + size - 1];
        size += p_data[idx + size - 1];
    }
    if (data_len < (idx + size))
    {
        goto error;
    }

    size = brs_data.num_subgroups * sizeof(T_BASS_CP_BIS_INFO) + metadata_len;
    if (size > 0)
    {
        brs_data.p_cp_bis_info = ble_audio_mem_zalloc(size);

        if (brs_data.p_cp_bis_info == NULL)
        {
            goto error;
        }
        brs_data.bis_info_size = size;
        p_meta_data = (uint8_t *)(brs_data.p_cp_bis_info + brs_data.num_subgroups);
        for (i = 0; i < brs_data.num_subgroups; i++)
        {
            LE_ARRAY_TO_UINT32(brs_data.p_cp_bis_info[i].bis_sync, &p_data[idx]);
            if (brs_data.p_cp_bis_info[i].bis_sync != BASS_FAILED_TO_SYNC_TO_BIG)
            {
                brs_data.bis_sync_state |= brs_data.p_cp_bis_info[i].bis_sync;
            }
            else
            {
                brs_data.bis_sync_state = BASS_FAILED_TO_SYNC_TO_BIG;
            }
            idx += 4;
            brs_data.p_cp_bis_info[i].metadata_len = p_data[idx++];
            if (brs_data.p_cp_bis_info[i].metadata_len > 0)
            {
                memcpy(p_meta_data, p_data + idx, brs_data.p_cp_bis_info[i].metadata_len);
                brs_data.p_cp_bis_info[i].p_metadata = p_meta_data;
                idx += brs_data.p_cp_bis_info[i].metadata_len;
                p_meta_data += brs_data.p_cp_bis_info[i].metadata_len;
                PROTOCOL_PRINT_INFO3("[BAP][BASS] bass_client_prase_brs: %d metadata[%d] %b", i,
                                     brs_data.p_cp_bis_info[i].metadata_len,
                                     TRACE_BINARY(brs_data.p_cp_bis_info[i].metadata_len,
                                                  brs_data.p_cp_bis_info[i].p_metadata));
            }
        }
    }

    PROTOCOL_PRINT_INFO8("[BAP][BASS] bass_client_prase_brs: source_id %d, source_address_type 0x%x, source_address %s, boradcast_id %s, source_adv_sid %d,  pa_sync_state %d, bis_sync_state 0x%x, big_encryption %d",
                         brs_data.source_id,
                         brs_data.source_address_type,
                         TRACE_BDADDR(brs_data.source_address),
                         TRACE_BINARY(BROADCAST_ID_LEN, brs_data.broadcast_id),
                         brs_data.source_adv_sid,
                         brs_data.pa_sync_state,
                         brs_data.bis_sync_state,
                         brs_data.big_encryption);
    if (p_brs_data->data.p_cp_bis_info)
    {
        ble_audio_mem_free(p_brs_data->data.p_cp_bis_info);
    }
    memcpy(&p_brs_data->data, &brs_data, sizeof(brs_data));
    return true;
error:
    PROTOCOL_PRINT_ERROR0("[BAP][BASS] bass_client_prase_brs: failed");
    if (brs_data.p_cp_bis_info)
    {
        ble_audio_mem_free(brs_data.p_cp_bis_info);
    }
    return false;
}

T_BASS_BRS_DATA *bass_get_brs_data(uint16_t conn_handle, uint8_t char_instance_id)
{
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    T_BASS_CLIENT_DB *p_bass_db = bass_check_link(conn_handle);
    if (p_link != NULL && p_bass_db != NULL)
    {
        T_REMOTE_BRS_DATA *p_brs_data = bass_find_brs_data(p_bass_db, char_instance_id);
        return &p_brs_data->data;
    }
    return NULL;
}


T_APP_RESULT bass_client_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;
    T_BASS_CLIENT_DB *p_bass_db = bass_check_link(conn_handle);
    if (p_bass_db == NULL && type != GATT_CLIENT_EVENT_DIS_DONE)
    {
        goto error;
    }
    switch (type)
    {
    case GATT_CLIENT_EVENT_DIS_DONE:
        {
            uint8_t brs_num = 0;
            T_BASS_CLIENT_DIS_DONE dis_done = {0};
            PROTOCOL_PRINT_INFO2("[BAP][BASS] GATT_CLIENT_EVENT_DIS_DONE: is_found %d, srv_instance_num %d",
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
                srv_uuid.p.uuid16 = GATT_UUID_BASS;
                char_uuid.is_uuid16 = true;
                char_uuid.instance_id = 0;
                char_uuid.p.uuid16 = BASS_UUID_CHAR_BROADCAST_RECEIVE_STATE;
                brs_num = gatt_client_get_char_num(conn_handle, &srv_uuid, &char_uuid);
                p_link = ble_audio_link_find_by_conn_handle(conn_handle);
                if (p_link == NULL || brs_num == 0)
                {
                    PROTOCOL_PRINT_ERROR1("bass_client_cb, link disconnected or brs_num = %d!", brs_num);
                    goto error;
                }

                if (p_link->p_bass_client)
                {
                    ble_audio_mem_free(p_link->p_bass_client);
                }
                p_link->p_bass_client = ble_audio_mem_zalloc(sizeof(T_BASS_CLIENT_DB) + (brs_num - 1) * sizeof(
                                                                 T_REMOTE_BRS_DATA));

                if (p_link->p_bass_client == NULL)
                {
                    PROTOCOL_PRINT_ERROR0("bass_client_cb, alloc p_bass_client fail!");
                    goto error;
                }
                p_bass_db = p_link->p_bass_client;
                p_bass_db->brs_num = brs_num;
                for (i = 0; i < p_bass_db->brs_num; i++)
                {
                    p_bass_db->brs_data[i].instance_id = i;
                }

            }
            dis_done.conn_handle = conn_handle;
            dis_done.is_found = p_client_cb_data->dis_done.is_found;
            dis_done.brs_char_num = brs_num;
            dis_done.load_from_ftl = p_client_cb_data->dis_done.load_from_ftl;
            ble_audio_mgr_dispatch((T_LE_AUDIO_MSG)LE_AUDIO_MSG_BASS_CLIENT_DIS_DONE, &dis_done);
        }
        break;

    case GATT_CLIENT_EVENT_READ_RESULT:
        {
            T_BASS_CLIENT_BRS_DATA brs_data = {0};
            PROTOCOL_PRINT_INFO7("[BAP][BASS] GATT_CLIENT_EVENT_READ_RESULT: conn_handle 0x%x, cause 0x%x, char_type %d, srv_instance_id %d, uuid16[%d] 0x%x, handle 0x%x",
                                 conn_handle, p_client_cb_data->read_result.cause,
                                 p_client_cb_data->read_result.char_type,
                                 p_client_cb_data->read_result.srv_instance_id,
                                 p_client_cb_data->read_result.char_uuid.instance_id,
                                 p_client_cb_data->read_result.char_uuid.p.uuid16,
                                 p_client_cb_data->read_result.handle);
            PROTOCOL_PRINT_INFO2("[BAP][BASS] GATT_CLIENT_EVENT_READ_RESULT: value[%d] %b",
                                 p_client_cb_data->read_result.value_size,
                                 TRACE_BINARY(p_client_cb_data->read_result.value_size,
                                              p_client_cb_data->read_result.p_value));
            brs_data.conn_handle = conn_handle;
            brs_data.notify = false;
            brs_data.read_cause = p_client_cb_data->read_result.cause;
            brs_data.char_instance_id = p_client_cb_data->read_result.char_uuid.instance_id;
            if (p_client_cb_data->read_result.cause == GAP_SUCCESS)
            {
                T_REMOTE_BRS_DATA *p_brs_data = bass_find_brs_data(p_bass_db,
                                                                   p_client_cb_data->read_result.char_uuid.instance_id);
                if (p_brs_data == NULL)
                {
                    goto error;
                }
                if (bass_client_prase_brs(p_brs_data, p_client_cb_data->read_result.p_value,
                                          p_client_cb_data->read_result.value_size))
                {
                    brs_data.p_brs_data = &p_brs_data->data;
                }
            }
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_BASS_CLIENT_BRS_DATA, &brs_data);
        }
        break;

    case GATT_CLIENT_EVENT_NOTIFY_IND:
        {
            T_PA_SYNC_STATE pa_sync_state;
            PROTOCOL_PRINT_INFO7("[BAP][BASS] GATT_CLIENT_EVENT_NOTIFY_IND: conn_handle 0x%x, srv_instance_id %d, uuid16[%d] 0x%x, handle 0x%x, data[%d] %b",
                                 conn_handle,
                                 p_client_cb_data->notify_ind.srv_instance_id,
                                 p_client_cb_data->notify_ind.char_uuid.instance_id,
                                 p_client_cb_data->notify_ind.char_uuid.p.uuid16,
                                 p_client_cb_data->notify_ind.handle,
                                 p_client_cb_data->notify_ind.value_size,
                                 TRACE_BINARY(p_client_cb_data->notify_ind.value_size,
                                              p_client_cb_data->notify_ind.p_value));
            T_REMOTE_BRS_DATA *p_brs_data = bass_find_brs_data(p_bass_db,
                                                               p_client_cb_data->notify_ind.char_uuid.instance_id);
            if (p_brs_data == NULL)
            {
                goto error;
            }
            pa_sync_state = p_brs_data->data.pa_sync_state;
            if (bass_client_prase_brs(p_brs_data, p_client_cb_data->notify_ind.p_value,
                                      p_client_cb_data->notify_ind.value_size))
            {
                T_BASS_CLIENT_BRS_DATA brs_data;
                brs_data.conn_handle = conn_handle;
                brs_data.notify = true;
                brs_data.read_cause = 0;
                brs_data.char_instance_id = p_client_cb_data->notify_ind.char_uuid.instance_id;
                brs_data.p_brs_data = &p_brs_data->data;
                ble_audio_mgr_dispatch(LE_AUDIO_MSG_BASS_CLIENT_BRS_DATA, &brs_data);
                if (p_brs_data->data.pa_sync_state != pa_sync_state &&
                    p_brs_data->data.pa_sync_state == PA_SYNC_STATE_SYNC_INFO_REQ)
                {
                    T_BASS_CLIENT_SYNC_INFO_REQ sync_req_data;
                    sync_req_data.conn_handle = conn_handle;
                    sync_req_data.char_instance_id = p_client_cb_data->notify_ind.char_uuid.instance_id;
                    sync_req_data.p_brs_data = &p_brs_data->data;
                    ble_audio_mgr_dispatch(LE_AUDIO_MSG_BASS_CLIENT_SYNC_INFO_REQ, &sync_req_data);
                }
            }
            else
            {
                T_BLE_AUDIO_LINK *p_link = NULL;
                p_link = ble_audio_link_find_by_conn_handle(conn_handle);
                if (p_link && p_client_cb_data->notify_ind.value_size == (p_link->mtu_size - 3))
                {
                    bass_read_brs_value(conn_handle, p_client_cb_data->notify_ind.char_uuid.instance_id);
                }
            }
        }
        break;

    case GATT_CLIENT_EVENT_WRITE_RESULT:
        {
            PROTOCOL_PRINT_INFO8("[BAP][BASS] GATT_CLIENT_EVENT_WRITE_RESULT: conn_handle 0x%x, cause 0x%x, type %d, char_type %d, srv_instance_id %d, uuid16[%d] 0x%x, handle 0x%x",
                                 conn_handle, p_client_cb_data->write_result.cause,
                                 p_client_cb_data->write_result.type,
                                 p_client_cb_data->write_result.char_type,
                                 p_client_cb_data->write_result.srv_instance_id,
                                 p_client_cb_data->write_result.char_uuid.instance_id,
                                 p_client_cb_data->write_result.char_uuid.p.uuid16,
                                 p_client_cb_data->write_result.handle);
            if (p_client_cb_data->write_result.char_type == GATT_CHAR_VALUE &&
                p_client_cb_data->write_result.char_uuid.p.uuid16 == BASS_UUID_CHAR_BROADCAST_AUDIO_SCAN_CP)
            {
                T_BASS_CLIENT_CP_RESULT cp_result;
                cp_result.conn_handle = conn_handle;
                cp_result.cause = p_client_cb_data->write_result.cause;
                ble_audio_mgr_dispatch(LE_AUDIO_MSG_BASS_CLIENT_CP_RESULT, &cp_result);
            }
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
    return APP_RESULT_APP_ERR;
}

static void bass_client_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BLE_DM_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BLE_DM_EVENT_CONN_STATE:
        {
            T_BLE_AUDIO_LINK *p_link = NULL;
            if (param->conn_state.state == GAP_CONN_STATE_DISCONNECTED)
            {
                p_link = ble_audio_link_find_by_conn_id(param->conn_state.conn_id);
                if (p_link == NULL)
                {
                    PROTOCOL_PRINT_ERROR1("bass_client_dm_cback: not find the link, conn_id %d",
                                          param->conn_state.conn_id);
                    break;
                }
                if (p_link->p_bass_client != NULL)
                {
                    T_BASS_CLIENT_DB *p_db = (T_BASS_CLIENT_DB *)p_link->p_bass_client;

                    for (uint8_t i = 0; i < p_db->brs_num; i++)
                    {
                        if (p_db->brs_data[i].data.p_cp_bis_info)
                        {
                            ble_audio_mem_free(p_db->brs_data[i].data.p_cp_bis_info);
                        }
                    }
                    ble_audio_mem_free(p_link->p_bass_client);
                    p_link->p_bass_client = NULL;
                }
            }
        }
        break;

    default:
        break;
    }
}

bool bass_client_init(void)
{
    T_ATTR_UUID srv_uuid = {0};
    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = GATT_UUID_BASS;
    gatt_client_spec_register(&srv_uuid, bass_client_cb);
    ble_dm_cback_register(bass_client_dm_cback);
    return true;
}

#if LE_AUDIO_DEINIT
void bass_client_deinit(void)
{
    for (uint8_t k = 0; k < ble_audio_db.acl_link_num; k++)
    {
        if (ble_audio_db.le_link[k].p_bass_client != NULL)
        {
            T_BASS_CLIENT_DB *p_db = (T_BASS_CLIENT_DB *)ble_audio_db.le_link[k].p_bass_client;

            for (uint8_t i = 0; i < p_db->brs_num; i++)
            {
                if (p_db->brs_data[i].data.p_cp_bis_info)
                {
                    ble_audio_mem_free(p_db->brs_data[i].data.p_cp_bis_info);
                }
            }
            ble_audio_mem_free(ble_audio_db.le_link[k].p_bass_client);
            ble_audio_db.le_link[k].p_bass_client = NULL;
        }
    }
}
#endif

#endif

