#if (LE_AUDIO_BROADCAST_SINK_ROLE | LE_AUDIO_BROADCAST_ASSISTANT_ROLE)
#include <string.h>
#include "trace.h"
#include "os_queue.h"
#include "ble_audio_sync_int.h"
#include "gap_big_mgr.h"
#include "gap_callback_le.h"
#include "gap_pa_sync.h"
#include "ble_isoch_def.h"
#include "gap_conn_le.h"
#include "ble_audio_mgr.h"
#include "ble_audio_ual.h"
#if CONFIG_REALTEK_BLE_USE_UAL_API
#include "ble_pa_sync.h"
#define BLE_AUDIO_LIB_APP_ID    6
static void ble_audio_sync_callback(uint8_t adv_addr_type, uint8_t *adv_addr,
                                    uint8_t adv_sid, uint8_t *broadcast_id,
                                    T_PA_SYNC_STATE_CHANGE_INFO *p_data);
#endif

typedef struct t_bc_pa_data
{
    struct t_bc_pa_data *p_next;
    T_GAP_PERIODIC_ADV_REPORT_DATA_STATUS  data_status;
    uint8_t     data_len;
    uint8_t     data[];
} T_BC_PA_DATA;

typedef struct t_bc_sync_db
{
    struct t_bc_sync_db *p_next;
    P_FUN_BLE_AUDIO_SYNC_CB cb_pfn;
    uint8_t advertiser_address_type;
    uint8_t adv_sid;
    uint8_t advertiser_address[GAP_BD_ADDR_LEN];
    uint8_t broadcast_id[BROADCAST_ID_LEN];

//pa sync param
#if F_BT_LE_5_0_PA_SYNC_SCAN_SUPPORT
    uint8_t sync_id;
    uint8_t pa_sync_addr[GAP_BD_ADDR_LEN];
    uint8_t pa_sync_addr_type;
    T_GAP_PA_SYNC_STATE pa_state;
    T_BLE_AUDIO_PA_ACTION curr_pa_action;
    T_BLE_AUDIO_PA_ACTION pending_pa_action;
    T_BLE_AUDIO_ACTION_ROLE pa_action_role;
    bool    is_pa_cmp;
    uint8_t pa_cmp_idx;
    bool big_info_received;
    uint16_t sync_handle;
    uint16_t pa_interval;
    T_GAP_PA_SYNC_CREATE_SYNC_PARAM pa_sync_param;
    T_BASE_DATA_MAPPING *p_mapping;
    T_OS_QUEUE pa_data_queue;
    T_LE_BIGINFO_ADV_REPORT_INFO big_info;
#endif

//big sync param
#if F_BT_LE_5_2_ISOC_BIS_RECEIVER_SUPPORT
    uint8_t bis_num;
    uint8_t big_handle;
    uint8_t iso_data_path_set;
    T_GAP_BIG_SYNC_RECEIVER_SYNC_STATE big_state;
    T_BLE_AUDIO_BIG_ACTION curr_big_action;
    T_BLE_AUDIO_BIG_ACTION pending_big_action;
    T_BLE_AUDIO_ACTION_ROLE big_action_role;
    T_BIG_MGR_SYNC_RECEIVER_BIG_CREATE_SYNC_PARAM big_sync_param;
    T_BIG_MGR_BIS_CONN_HANDLE_INFO bis_conn_handle_info[GAP_BIG_MGR_MAX_BIS_NUM];
#endif
} T_BC_SYNC_DB;

T_OS_QUEUE bc_sync_list;
T_GAP_PA_SYNC_DEV_STATE bc_sync_pa_dev_state = {0, 0, 0};
T_BIG_MGR_SYNC_RECEIVER_DEV_STATE bc_sync_big_dev_state = {0, 0};
#if LE_AUDIO_BASS_SUPPORT
P_FUN_BLE_AUDIO_SYNC_CB bass_cb_pfn = NULL;
#endif

#define bc_sync_handle_check(handle) bc_sync_handle_check_int(__func__, handle)

bool bc_sync_handle_check_int(const char *p_func_name,
                              T_BLE_AUDIO_SYNC_HANDLE handle)
{
    if (handle != NULL)
    {
        if (os_queue_search(&bc_sync_list, handle) == true)
        {
            return true;
        }
    }
    PROTOCOL_PRINT_ERROR2("bc_sync_handle_check_int:failed, %s, handle %p",
                          TRACE_STRING(p_func_name), handle);
    return false;
}

T_BLE_AUDIO_SYNC_HANDLE ble_audio_sync_find(uint8_t adv_sid, uint8_t broadcast_id[BROADCAST_ID_LEN])
{
    T_BC_SYNC_DB *p_db;
    uint8_t all_zero[BROADCAST_ID_LEN] = {0, 0, 0};
    if (broadcast_id == NULL)
    {
        broadcast_id = all_zero;
    }
    for (uint8_t i = 0; i < bc_sync_list.count; i++)
    {
        p_db = (T_BC_SYNC_DB *)os_queue_peek(&bc_sync_list, i);

        if (p_db->adv_sid == adv_sid &&
            memcmp(p_db->broadcast_id, broadcast_id, BROADCAST_ID_LEN) == 0)
        {
            return p_db;
        }
    }
    return NULL;
}

bool ble_audio_sync_get_info(T_BLE_AUDIO_SYNC_HANDLE handle, T_BLE_AUDIO_SYNC_INFO *p_info)
{
    T_BC_SYNC_DB *p_db = (T_BC_SYNC_DB *)handle;
    if (bc_sync_handle_check(handle) == false)
    {
        return false;
    }
    if (p_info == NULL)
    {
        return false;
    }
    p_info->sync_id = p_db->sync_id;
    p_info->pa_state = p_db->pa_state;
    p_info->p_base_mapping = p_db->p_mapping;
    p_info->big_info_received = p_db->big_info_received;
    memcpy(&p_info->big_info, &p_db->big_info, sizeof(T_LE_BIGINFO_ADV_REPORT_INFO));
    p_info->pa_interval = p_db->pa_interval;
    p_info->pa_sync_addr_type = p_db->pa_sync_addr_type;
    memcpy(p_info->pa_sync_addr, p_db->pa_sync_addr, GAP_BD_ADDR_LEN);
    //BIG info
    p_info->big_state = p_db->big_state;
    p_info->advertiser_address_type = p_db->advertiser_address_type;
    memcpy(p_info->advertiser_address, p_db->advertiser_address, GAP_BD_ADDR_LEN);
    p_info->adv_sid = p_db->adv_sid;
    memcpy(p_info->broadcast_id, p_db->broadcast_id, BROADCAST_ID_LEN);
    return true;
}


bool ble_audio_sync_update_addr(T_BLE_AUDIO_SYNC_HANDLE handle,
                                uint8_t *advertiser_address)
{
    T_BC_SYNC_DB *p_db = (T_BC_SYNC_DB *)handle;
    if (bc_sync_handle_check(handle) == false)
    {
        return false;
    }
    if (p_db->advertiser_address_type != GAP_REMOTE_ADDR_LE_RANDOM)
    {
        return false;
    }
    if (memcmp(p_db->advertiser_address, advertiser_address, GAP_BD_ADDR_LEN) != 0)
    {
        memcpy(p_db->advertiser_address, advertiser_address, GAP_BD_ADDR_LEN);
        T_BLE_AUDIO_ADDR_UPDATE addr_update;
        T_BLE_AUDIO_SYNC_CB_DATA cb_data;
        addr_update.advertiser_address = p_db->advertiser_address;
        cb_data.p_addr_update = &addr_update;
        if (p_db->cb_pfn)
        {
            p_db->cb_pfn(p_db, MSG_BLE_AUDIO_ADDR_UPDATE, &cb_data);
        }
#if LE_AUDIO_BASS_SUPPORT
        if (bass_cb_pfn)
        {
            bass_cb_pfn(p_db, MSG_BLE_AUDIO_ADDR_UPDATE, &cb_data);
        }
#endif
    }
    return true;
}

bool ble_audio_sync_update_cb(T_BLE_AUDIO_SYNC_HANDLE handle,
                              P_FUN_BLE_AUDIO_SYNC_CB cb_pfn)
{
    T_BC_SYNC_DB *p_db = (T_BC_SYNC_DB *)handle;
    if (bc_sync_handle_check(handle) == false)
    {
        return false;
    }
    p_db->cb_pfn = cb_pfn;
    return true;
}
#if LE_AUDIO_BASS_SUPPORT
void ble_audio_sync_register_bass_cb(P_FUN_BLE_AUDIO_SYNC_CB cb_pfn)
{
    bass_cb_pfn = cb_pfn;
}
#endif
T_BLE_AUDIO_SYNC_HANDLE ble_audio_sync_create_int(P_FUN_BLE_AUDIO_SYNC_CB cb_pfn,
                                                  uint8_t advertiser_address_type,
                                                  uint8_t *advertiser_address, uint8_t adv_sid,
                                                  uint8_t broadcast_id[BROADCAST_ID_LEN],
                                                  T_BLE_AUDIO_ACTION_ROLE role)
{
    T_BC_SYNC_DB *p_db = NULL;
    if (advertiser_address == NULL)
    {
        goto error;
    }
    p_db = ble_audio_sync_find(adv_sid, broadcast_id);
    if (p_db)
    {
        PROTOCOL_PRINT_ERROR0("ble_audio_sync_allocate: already exist");
        return NULL;
    }
#if LE_AUDIO_BASS_SUPPORT
    if (bass_cb_pfn && role == BLE_AUDIO_ACTION_ROLE_LOCAL_API)
    {
        if (bass_brs_get_empty_char_num() == 0)
        {
            PROTOCOL_PRINT_ERROR0("ble_audio_sync_allocate: no empty brs char");
            return NULL;
        }
    }
#endif
    p_db = ble_audio_mem_zalloc(sizeof(T_BC_SYNC_DB));
    if (p_db == NULL)
    {
        goto error;
    }
    p_db->cb_pfn = cb_pfn;
    p_db->advertiser_address_type = advertiser_address_type;
    p_db->adv_sid = adv_sid;
    p_db->sync_id = 0xFF;
    p_db->sync_handle = 0xFFFF;
    memcpy(p_db->advertiser_address, advertiser_address, GAP_BD_ADDR_LEN);
    p_db->pa_sync_addr_type = p_db->advertiser_address_type;
    memcpy(p_db->pa_sync_addr, p_db->advertiser_address, GAP_BD_ADDR_LEN);
    if (broadcast_id)
    {
        memcpy(p_db->broadcast_id, broadcast_id, BROADCAST_ID_LEN);
    }
    os_queue_in(&bc_sync_list, p_db);
    PROTOCOL_PRINT_INFO1("ble_audio_sync_allocate: handle %p", p_db);
#if LE_AUDIO_BASS_SUPPORT
    if (bass_cb_pfn && role == BLE_AUDIO_ACTION_ROLE_LOCAL_API)
    {
        bass_cb_pfn(p_db, MSG_BLE_AUDIO_SYNC_LOCAL_ADD, NULL);
    }
#endif
    return p_db;
error:
    PROTOCOL_PRINT_ERROR0("ble_audio_sync_allocate: failed");
    return NULL;
}

T_BLE_AUDIO_SYNC_HANDLE ble_audio_sync_allocate(P_FUN_BLE_AUDIO_SYNC_CB cb_pfn,
                                                uint8_t advertiser_address_type,
                                                uint8_t *advertiser_address, uint8_t adv_sid,
                                                uint8_t broadcast_id[BROADCAST_ID_LEN])
{
    return ble_audio_sync_create_int(cb_pfn, advertiser_address_type,
                                     advertiser_address, adv_sid,
                                     broadcast_id, BLE_AUDIO_ACTION_ROLE_LOCAL_API);
}

bool ble_audio_sync_release_int(T_BLE_AUDIO_SYNC_HANDLE *p_handle, T_BLE_AUDIO_ACTION_ROLE role)
{
    T_BC_SYNC_DB *p_db;
    if (p_handle == NULL)
    {
        return false;
    }
    p_db = (T_BC_SYNC_DB *)(*p_handle);
    if (bc_sync_handle_check(*p_handle) == false)
    {
        return false;
    }
    if (p_db && p_db->pa_state == GAP_PA_SYNC_STATE_TERMINATED &&
        p_db->big_state == BIG_SYNC_RECEIVER_SYNC_STATE_TERMINATED)
    {
        if (os_queue_delete(&bc_sync_list, p_db))
        {
            T_BC_PA_DATA *p;
            while ((p = os_queue_out(&p_db->pa_data_queue)) != NULL)
            {
                ble_audio_mem_free(p);
            }
            if (p_db->p_mapping)
            {
                base_data_free(p_db->p_mapping);
            }
            T_BLE_AUDIO_SYNC_CB_DATA cb_data;
            T_BLE_AUDIO_SYNC_HANDLE_RELEASED info;
            info.action_role = role;
            cb_data.p_sync_handle_released = &info;
            if (p_db->cb_pfn)
            {

                p_db->cb_pfn(p_db, MSG_BLE_AUDIO_SYNC_HANDLE_RELEASED, &cb_data);
            }
#if LE_AUDIO_BASS_SUPPORT
            if (bass_cb_pfn)
            {
                bass_cb_pfn(p_db, MSG_BLE_AUDIO_SYNC_HANDLE_RELEASED, &cb_data);
            }
#endif
            PROTOCOL_PRINT_INFO1("ble_audio_sync_release: handle %p", p_db);
            ble_audio_mem_free(p_db);
            *p_handle = NULL;
            return true;
        }
    }
    PROTOCOL_PRINT_ERROR0("ble_audio_sync_release: failed");
    return false;
}

bool ble_audio_sync_release(T_BLE_AUDIO_SYNC_HANDLE *p_handle)
{
    return ble_audio_sync_release_int(p_handle, BLE_AUDIO_ACTION_ROLE_LOCAL_API);
}

#if F_BT_LE_5_0_PA_SYNC_SCAN_SUPPORT
T_BC_SYNC_DB *ble_audio_sync_find_by_sync_id(uint8_t sync_id)
{
    T_BC_SYNC_DB *p_db;
    for (uint8_t i = 0; i < bc_sync_list.count; i++)
    {
        p_db = (T_BC_SYNC_DB *)os_queue_peek(&bc_sync_list, i);
        if (p_db->sync_id == sync_id)
        {
            return p_db;
        }
    }
    return NULL;
}

#if LE_AUDIO_BASS_SUPPORT
static T_BC_SYNC_DB *ble_audio_sync_find_by_adv_info(uint8_t *adv_addr, uint8_t adv_addr_type,
                                                     uint8_t adv_sid)
{
    T_BC_SYNC_DB *p_db;
    for (uint8_t i = 0; i < bc_sync_list.count; i++)
    {
        p_db = (T_BC_SYNC_DB *)os_queue_peek(&bc_sync_list, i);

        if ((p_db->adv_sid == adv_sid) &&
            (p_db->advertiser_address_type == adv_addr_type) &&
            !memcmp(p_db->advertiser_address, adv_addr, GAP_BD_ADDR_LEN))
        {
            return p_db;
        }
    }
    if (adv_addr_type ==  GAP_REMOTE_ADDR_LE_RANDOM &&
        ((adv_addr[5] & RANDOM_ADDR_MASK) == RANDOM_ADDR_MASK_RESOLVABLE))
    {
        uint8_t resolved_addr[GAP_BD_ADDR_LEN];
        T_GAP_IDENT_ADDR_TYPE resolved_addr_type;
        if (ble_audio_ual_resolve_rpa(adv_addr, resolved_addr, &resolved_addr_type))
        {
            for (uint8_t i = 0; i < bc_sync_list.count; i++)
            {
                p_db = (T_BC_SYNC_DB *)os_queue_peek(&bc_sync_list, i);

                if (p_db->adv_sid == adv_sid && p_db->advertiser_address_type == resolved_addr_type &&
                    memcmp(p_db->advertiser_address, resolved_addr, GAP_BD_ADDR_LEN) == 0)
                {
                    return p_db;
                }
            }
        }
    }
    PROTOCOL_PRINT_ERROR0("ble_audio_sync_find_by_adv_info: failed");
    return NULL;
}
#endif

void ble_audio_send_pa_sync_state(T_BC_SYNC_DB *p_db, T_GAP_PA_SYNC_STATE new_state,
                                  T_BLE_AUDIO_PA_ACTION action,
                                  uint16_t cause)
{
    if (p_db->pa_state != new_state || cause != 0)
    {
        PROTOCOL_PRINT_INFO5("ble_audio_send_pa_sync_state: handle %p, state %d -> %d, action %d, cause 0x%x",
                             p_db, p_db->pa_state, new_state, action, cause);
        p_db->pa_state = new_state;

        T_BLE_AUDIO_PA_SYNC_STATE state_change = {(T_GAP_PA_SYNC_STATE)0};
        T_BLE_AUDIO_SYNC_CB_DATA cb_data;

        state_change.sync_state = p_db->pa_state;
        state_change.action = action;
        state_change.cause = cause;
        state_change.action_role = p_db->pa_action_role;
        if (p_db->pa_state == GAP_PA_SYNC_STATE_SYNCHRONIZED)
        {
            T_GAP_PA_SYNC_COMMON_SYNC_PARAM sync_param;
            if (le_pa_sync_get_param(PA_SYNC_PARAM_SYNCHRONIZED_PARAM, &sync_param,
                                     p_db->sync_id) == GAP_CAUSE_SUCCESS)
            {
                p_db->pa_interval = sync_param.periodic_adv_interval;
            }

        }
        else
        {
            p_db->pa_interval = 0;
        }
        cb_data.p_pa_sync_state = &state_change;
        if (new_state == GAP_PA_SYNC_STATE_SYNCHRONIZED ||
            new_state == GAP_PA_SYNC_STATE_TERMINATED)
        {
            p_db->pa_action_role = BLE_AUDIO_ACTION_ROLE_IDLE;
            p_db->curr_pa_action = BLE_AUDIO_PA_IDLE;
        }
        if (p_db->cb_pfn)
        {
            p_db->cb_pfn(p_db, MSG_BLE_AUDIO_PA_SYNC_STATE, &cb_data);
        }
#if LE_AUDIO_BASS_SUPPORT
        if (bass_cb_pfn)
        {
            bass_cb_pfn(p_db, MSG_BLE_AUDIO_PA_SYNC_STATE, &cb_data);
        }
#endif
    }
}

bool ble_audio_pa_sync_establish_int(T_BLE_AUDIO_SYNC_HANDLE handle, uint8_t options,
                                     uint8_t sync_cte_type,
                                     uint16_t skip, uint16_t sync_timeout,
                                     T_BLE_AUDIO_ACTION_ROLE role)
{
    T_BC_SYNC_DB *p_db = (T_BC_SYNC_DB *)handle;
    if (bc_sync_handle_check(handle) == false)
    {
        return false;
    }

#if CONFIG_REALTEK_BLE_USE_UAL_API
    if (p_db && p_db->pa_state == GAP_PA_SYNC_STATE_TERMINATED)
    {
        ble_register_pa_sync_callback(BLE_AUDIO_LIB_APP_ID, ble_audio_sync_callback);
        if (ble_pa_sync(BLE_AUDIO_LIB_APP_ID, p_db->advertiser_address,
                        p_db->advertiser_address_type, p_db->adv_sid,
                        p_db->broadcast_id, false))
        {
            p_db->pa_action_role = role;
            p_db->pending_pa_action = BLE_AUDIO_PA_IDLE;
            p_db->curr_pa_action = BLE_AUDIO_PA_SYNC;
            p_db->sync_id = 0xFF;
            p_db->sync_handle = 0xFFFF;
            return true;
        }
    }
#else
    if (p_db && p_db->pa_state == GAP_PA_SYNC_STATE_TERMINATED)
    {
        T_GAP_CAUSE cause;
        T_GAP_PA_SYNC_CREATE_SYNC_PARAM sync_param;
        sync_param.options = options;
        sync_param.sync_cte_type = sync_cte_type;
        sync_param.adv_sid = p_db->adv_sid;
        sync_param.adv_addr_type = (T_GAP_PA_SYNC_ADV_ADDR_TYPE)p_db->advertiser_address_type;
        memcpy(sync_param.adv_addr, p_db->advertiser_address, GAP_BD_ADDR_LEN);
        sync_param.skip = skip;
        sync_param.sync_timeout = sync_timeout;
        if (bc_sync_pa_dev_state.gap_create_sync_state == GAP_PA_CREATE_SYNC_DEV_STATE_SYNCHRONIZING)
        {
            memcpy(&p_db->pa_sync_param, &sync_param, sizeof(T_GAP_PA_SYNC_CREATE_SYNC_PARAM));
            p_db->pending_pa_action = BLE_AUDIO_PA_SYNC;
            p_db->pa_action_role = role;
            return true;
        }
        T_BLE_AUDIO_ACTION_ROLE old_pa_action_role = p_db->pa_action_role;
        T_BLE_AUDIO_PA_ACTION old_curr_pa_action = p_db->curr_pa_action;
        p_db->pa_action_role = role;
        p_db->curr_pa_action = BLE_AUDIO_PA_SYNC;
        cause = le_pa_sync_create_sync(&sync_param, &p_db->sync_id);
        p_db->pending_pa_action = BLE_AUDIO_PA_IDLE;
        if (cause == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
        else
        {
            p_db->pa_action_role = old_pa_action_role;
            p_db->curr_pa_action = old_curr_pa_action;
        }
    }
#endif
    return false;
}

bool ble_audio_pa_sync_establish(T_BLE_AUDIO_SYNC_HANDLE handle, uint8_t options,
                                 uint8_t sync_cte_type,
                                 uint16_t skip, uint16_t sync_timeout)
{
    return ble_audio_pa_sync_establish_int(handle, options, sync_cte_type, skip, sync_timeout,
                                           BLE_AUDIO_ACTION_ROLE_LOCAL_API);
}

bool ble_audio_pa_terminate_int(T_BLE_AUDIO_SYNC_HANDLE handle, T_BLE_AUDIO_ACTION_ROLE role)
{
    T_BC_SYNC_DB *p_db = (T_BC_SYNC_DB *)handle;
    if (bc_sync_handle_check(handle) == false)
    {
        return false;
    }
#if CONFIG_REALTEK_BLE_USE_UAL_API
    if (p_db &&
        (p_db->pa_state != GAP_PA_SYNC_STATE_TERMINATED) &&
        (p_db->pa_state != GAP_PA_SYNC_STATE_TERMINATING))
    {
        if (ble_terminate_pa_sync_by_sync_id(BLE_AUDIO_LIB_APP_ID, p_db->sync_id, false))
        {
            p_db->pending_pa_action = BLE_AUDIO_PA_IDLE;
            p_db->pa_action_role = role;
            p_db->curr_pa_action = BLE_AUDIO_PA_TERMINATE;
            return true;
        }
    }
#else
    T_GAP_CAUSE cause;
    if (p_db &&
        (p_db->pa_state != GAP_PA_SYNC_STATE_TERMINATED) &&
        (p_db->pa_state != GAP_PA_SYNC_STATE_TERMINATING))
    {
        if (bc_sync_pa_dev_state.gap_terminate_sync_state == GAP_PA_TERMINATE_SYNC_DEV_STATE_TERMINATING)
        {
            p_db->pending_pa_action = BLE_AUDIO_PA_TERMINATE;
            p_db->pa_action_role = role;
            return true;
        }
        T_BLE_AUDIO_ACTION_ROLE old_pa_action_role = p_db->pa_action_role;
        T_BLE_AUDIO_PA_ACTION old_curr_pa_action = p_db->curr_pa_action;
        p_db->pa_action_role = role;
        p_db->curr_pa_action = BLE_AUDIO_PA_TERMINATE;
        cause = le_pa_sync_terminate_sync(p_db->sync_id);
        p_db->pending_pa_action = BLE_AUDIO_PA_IDLE;
        if (cause == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
        else
        {
            p_db->pa_action_role = old_pa_action_role;
            p_db->curr_pa_action = old_curr_pa_action;
        }
    }
#endif
    return false;
}

bool ble_audio_pa_terminate(T_BLE_AUDIO_SYNC_HANDLE handle)
{
    return ble_audio_pa_terminate_int(handle, BLE_AUDIO_ACTION_ROLE_LOCAL_API);
}
#if F_BT_LE_5_1_PAST_RECIPIENT_SUPPORT
bool ble_audio_set_past_recipient_param(uint16_t conn_handle,
                                        T_GAP_PAST_RECIPIENT_PERIODIC_ADV_SYNC_TRANSFER_PARAM *p_param)
{
    if (p_param)
    {
        uint8_t conn_id;
        if (le_get_conn_id_by_handle(conn_handle, &conn_id))
        {
            if (le_past_recipient_set_periodic_adv_sync_transfer_params(conn_id, p_param) == GAP_CAUSE_SUCCESS)
            {
                return true;
            }
        }
    }
    return false;
}
#endif
void ble_audio_sync_handle_pa_dev_state_evt(T_GAP_PA_SYNC_DEV_STATE new_state, uint16_t cause)
{
    T_GAP_PA_SYNC_DEV_STATE old_state = bc_sync_pa_dev_state;
    bc_sync_pa_dev_state = new_state;
    if (old_state.gap_terminate_sync_state != new_state.gap_terminate_sync_state)
    {
        if (new_state.gap_terminate_sync_state == GAP_PA_TERMINATE_SYNC_DEV_STATE_IDLE)
        {
            T_BC_SYNC_DB *p_db;
            PROTOCOL_PRINT_INFO1("GAP_PA_TERMINATE_SYNC_DEV_STATE_IDLE: cause 0x%x", cause);
            for (uint8_t i = 0; i < bc_sync_list.count; i++)
            {
                p_db = (T_BC_SYNC_DB *)os_queue_peek(&bc_sync_list, i);
                if (p_db->pending_pa_action == BLE_AUDIO_PA_TERMINATE)
                {
                    if (ble_audio_pa_terminate(p_db) == false)
                    {
                        ble_audio_send_pa_sync_state(p_db, p_db->pa_state, BLE_AUDIO_PA_TERMINATE,
                                                     GAP_ERR_REQ_FAILED);
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        else if (new_state.gap_terminate_sync_state == GAP_PA_TERMINATE_SYNC_DEV_STATE_TERMINATING)
        {
            PROTOCOL_PRINT_INFO1("GAP_PA_TERMINATE_SYNC_DEV_STATE_TERMINATING: cause 0x%x", cause);
        }
    }

#if CONFIG_REALTEK_BLE_USE_UAL_API

#else
    if (old_state.gap_create_sync_state != new_state.gap_create_sync_state)
    {
        if (new_state.gap_create_sync_state == GAP_PA_CREATE_SYNC_DEV_STATE_IDLE)
        {
            T_BC_SYNC_DB *p_db;
            PROTOCOL_PRINT_INFO1("GAP_PA_CREATE_SYNC_DEV_STATE_IDLE: cause 0x%x", cause);
            for (uint8_t i = 0; i < bc_sync_list.count; i++)
            {
                p_db = (T_BC_SYNC_DB *)os_queue_peek(&bc_sync_list, i);
                if (p_db->pending_pa_action == BLE_AUDIO_PA_SYNC)
                {
                    if (ble_audio_pa_sync_establish(p_db, p_db->pa_sync_param.options,
                                                    p_db->pa_sync_param.sync_cte_type, p_db->pa_sync_param.skip,
                                                    p_db->pa_sync_param.sync_timeout) == false)
                    {
                        ble_audio_send_pa_sync_state(p_db, p_db->pa_state, BLE_AUDIO_PA_SYNC,
                                                     GAP_ERR_REQ_FAILED);
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        else if (new_state.gap_create_sync_state == GAP_PA_CREATE_SYNC_DEV_STATE_SYNCHRONIZING)
        {
            PROTOCOL_PRINT_INFO1("GAP_PA_CREATE_SYNC_DEV_STATE_SYNCHRONIZING: cause 0x%x", cause);
        }
    }
#endif
}
#endif

void ble_audio_sync_pa_info_list_free(T_BC_SYNC_DB *p_db, bool map_release)
{
    T_BC_PA_DATA *p;
    while ((p = os_queue_out(&p_db->pa_data_queue)) != NULL)
    {
        ble_audio_mem_free(p);
    }
    p_db->is_pa_cmp = false;
    p_db->pa_cmp_idx = 0;
    if (map_release)
    {
        if (p_db->p_mapping)
        {
            base_data_free(p_db->p_mapping);
            p_db->p_mapping = NULL;
        }
    }
}

void ble_audio_sync_cmp_pa_mapping(T_BC_SYNC_DB *p_db)
{
    bool map_modify = false;
    uint8_t *p_buf = NULL;
    uint16_t buf_len = 0;
    uint16_t offset = 0;
    T_BC_PA_DATA *pa_data;
    T_BASE_DATA_MAPPING *p_mapping = NULL;

    PROTOCOL_PRINT_INFO2("ble_audio_sync_cmp_pa_mapping: p_db->pa_data_queue.count %d, p_db->p_mapping %p",
                         p_db->pa_data_queue.count, p_db->p_mapping);

    for (uint8_t i = 0; i < p_db->pa_data_queue.count; i++)
    {
        pa_data = (T_BC_PA_DATA *)os_queue_peek(&p_db->pa_data_queue, i);
        buf_len += pa_data->data_len;
    }
    p_buf = ble_audio_mem_zalloc(buf_len);
    if (p_buf == NULL)
    {
        return;
    }
    for (uint8_t i = 0; i < p_db->pa_data_queue.count; i++)
    {
        pa_data = (T_BC_PA_DATA *)os_queue_peek(&p_db->pa_data_queue, i);
        memcpy(&p_buf[offset], pa_data->data, pa_data->data_len);
        offset += pa_data->data_len;
    }
    p_mapping = base_data_parse_data(offset, p_buf);
    if (p_mapping)
    {
        if (p_db->p_mapping)
        {
            if (!base_data_cmp(p_db->p_mapping, p_mapping))
            {
                base_data_free(p_db->p_mapping);
                p_db->p_mapping = p_mapping;
                map_modify = true;
            }
            else
            {
                base_data_free(p_mapping);
            }
        }
        else
        {
            p_db->p_mapping = p_mapping;
            map_modify = true;
        }
    }
    else
    {
        ble_audio_sync_pa_info_list_free(p_db, false);
    }
    if (p_buf)
    {
        ble_audio_mem_free(p_buf);
    }
    if (map_modify)
    {
        T_BLE_AUDIO_SYNC_CB_DATA cb_data;
        T_BLE_AUDIO_BASE_DATA_MODIFY_INFO modify_info;
        modify_info.p_base_mapping = p_db->p_mapping;
        cb_data.p_base_data_modify_info = &modify_info;
        if (p_db->cb_pfn)
        {
            p_db->cb_pfn(p_db, MSG_BLE_AUDIO_BASE_DATA_MODIFY_INFO, &cb_data);
        }
#if LE_AUDIO_BASS_SUPPORT
        if (bass_cb_pfn)
        {
            bass_cb_pfn(p_db, MSG_BLE_AUDIO_BASE_DATA_MODIFY_INFO, &cb_data);
        }
#endif
    }
}

void ble_audio_sync_handle_pa_info(T_BC_SYNC_DB *p_db, T_LE_PERIODIC_ADV_REPORT_INFO *p_info)
{
    if (p_db->pa_data_queue.count == 0)
    {
        PROTOCOL_PRINT_INFO1("ble_audio_sync_handle_pa_info: count = 0, p_db->is_pa_cmp %d",
                             p_db->is_pa_cmp);
        if (p_db->is_pa_cmp)
        {
            if (p_info->data_status == GAP_PERIODIC_ADV_REPORT_DATA_STATUS_COMPLETE)
            {
                p_db->is_pa_cmp = false;
            }
        }
        else
        {
            if (p_info->data_status != GAP_PERIODIC_ADV_REPORT_DATA_STATUS_TRUNCATED)
            {
                goto new_report;
            }
        }
    }
    else
    {
        if (p_db->is_pa_cmp)
        {
            if (p_info->data_status == GAP_PERIODIC_ADV_REPORT_DATA_STATUS_TRUNCATED)
            {
                p_db->pa_cmp_idx = 0;
            }
            else
            {
                T_BC_PA_DATA *p_cmp_data;
                p_cmp_data = os_queue_peek(&p_db->pa_data_queue, p_db->pa_cmp_idx);
                if (p_cmp_data)
                {
                    if (p_cmp_data->data_len == p_info->data_len &&
                        p_cmp_data->data_status == p_info->data_status &&
                        memcmp(p_info->p_data, p_cmp_data->data, p_info->data_len) == 0)
                    {
                        p_db->pa_cmp_idx++;
                    }
                    else
                    {
                        PROTOCOL_PRINT_INFO2("ble_audio_sync_handle_pa_info: not match, p_db->pa_cmp_idx %d, data_status %d",
                                             p_db->pa_cmp_idx, p_info->data_status);
                        ble_audio_sync_pa_info_list_free(p_db, false);
                        if (p_info->data_status != GAP_PERIODIC_ADV_REPORT_DATA_STATUS_COMPLETE)
                        {
                            p_db->is_pa_cmp = true;
                        }
                    }

                }
                else
                {
                    PROTOCOL_PRINT_INFO1("ble_audio_sync_handle_pa_info: error, p_db->pa_cmp_idx %d",
                                         p_db->pa_cmp_idx);
                    p_db->pa_cmp_idx = 0;
                    return;
                }
                if (p_info->data_status == GAP_PERIODIC_ADV_REPORT_DATA_STATUS_COMPLETE)
                {
                    p_db->pa_cmp_idx = 0;
                }
            }
        }
        else
        {
            if (p_info->data_status == GAP_PERIODIC_ADV_REPORT_DATA_STATUS_TRUNCATED)
            {
                ble_audio_sync_pa_info_list_free(p_db, false);
            }
            else
            {
                goto new_report;
            }
        }
    }
    return;
new_report:
    {
        T_BC_PA_DATA *pa_data = ble_audio_mem_zalloc(sizeof(T_BC_PA_DATA) + p_info->data_len);
        if (pa_data == NULL)
        {
            return;
        }
        pa_data->data_status = p_info->data_status;
        pa_data->data_len = p_info->data_len;
        memcpy(pa_data->data, p_info->p_data, p_info->data_len);
        os_queue_in(&p_db->pa_data_queue, pa_data);

        if (p_info->data_status == GAP_PERIODIC_ADV_REPORT_DATA_STATUS_COMPLETE)
        {
            p_db->is_pa_cmp = true;
            ble_audio_sync_cmp_pa_mapping(p_db);
        }
        else
        {
            p_db->is_pa_cmp = false;
        }
    }
    return;
}

void ble_audio_sync_handle_gap_cb(uint8_t cb_type, T_LE_CB_DATA *p_data)
{
    T_BC_SYNC_DB *p_db;
    switch (cb_type)
    {
#if F_BT_LE_5_0_PA_SYNC_SCAN_SUPPORT
    case GAP_MSG_LE_PA_SYNC_MODIFY_PERIODIC_ADV_LIST:
        {
            PROTOCOL_PRINT_INFO2("GAP_MSG_LE_PA_SYNC_MODIFY_PERIODIC_ADV_LIST: operation %d, cause 0x%x",
                                 p_data->p_le_pa_sync_modify_periodic_adv_list_rsp->operation,
                                 p_data->p_le_pa_sync_modify_periodic_adv_list_rsp->cause);
        }
        break;

    case GAP_MSG_LE_PA_SYNC_DEV_STATE_CHANGE_INFO:
        {
            ble_audio_sync_handle_pa_dev_state_evt(p_data->p_le_pa_sync_dev_state_change_info->state,
                                                   p_data->p_le_pa_sync_dev_state_change_info->cause);
        }
        break;

    case GAP_MSG_LE_PA_SYNC_STATE_CHANGE_INFO:
        {
            T_BLE_AUDIO_PA_ACTION pa_action;
            p_db = ble_audio_sync_find_by_sync_id(p_data->p_le_pa_sync_state_change_info->sync_id);
            if (p_db == NULL)
            {
                return;
            }
            PROTOCOL_PRINT_INFO4("GAP_MSG_LE_PA_SYNC_STATE_CHANGE_INFO: sync_id %d, sync_handle 0x%x, state %d , terminate_cause 0x%x",
                                 p_data->p_le_pa_sync_state_change_info->sync_id,
                                 p_data->p_le_pa_sync_state_change_info->sync_handle,
                                 (T_GAP_PA_SYNC_STATE)p_data->p_le_pa_sync_state_change_info->state,
                                 p_data->p_le_pa_sync_state_change_info->cause);
            p_db->sync_handle = p_data->p_le_pa_sync_state_change_info->sync_handle;
            pa_action = p_db->curr_pa_action;
            if (p_data->p_le_pa_sync_state_change_info->cause == (HCI_ERR | HCI_ERR_CONN_TIMEOUT))
            {
                pa_action = BLE_AUDIO_PA_LOST;
            }
            if (p_data->p_le_pa_sync_state_change_info->state == GAP_PA_SYNC_STATE_TERMINATED)
            {
                ble_audio_sync_pa_info_list_free(p_db, true);
                p_db->big_info_received = false;
                memset(&p_db->big_info, 0, sizeof(T_LE_BIGINFO_ADV_REPORT_INFO));
            }

            if (p_db->pa_state == GAP_PA_SYNC_STATE_TERMINATING &&
                p_data->p_le_pa_sync_state_change_info->state == GAP_PA_SYNC_STATE_SYNCHRONIZED &&
                p_db->curr_pa_action == BLE_AUDIO_PA_TERMINATE)
            {
                PROTOCOL_PRINT_INFO0("GAP_MSG_LE_PA_SYNC_STATE_CHANGE_INFO: cancel failed, pa terminate req");
                //Fix BB2BUG-5769, PA SYNC cancel failed, cause = 0x107
                if (le_pa_sync_terminate_sync(p_db->sync_id) == GAP_CAUSE_SUCCESS)
                {
                    break;
                }
            }
            if (p_data->p_le_pa_sync_state_change_info->state == GAP_PA_SYNC_STATE_TERMINATED)
            {
                p_db->sync_id = 0xFF;
                p_db->sync_handle = 0xFFFF;
            }
            ble_audio_send_pa_sync_state(p_db,
                                         (T_GAP_PA_SYNC_STATE)p_data->p_le_pa_sync_state_change_info->state,
                                         pa_action,  p_data->p_le_pa_sync_state_change_info->cause);
        }
        break;

    case GAP_MSG_LE_PERIODIC_ADV_REPORT_INFO:
        {
            T_BC_SYNC_DB *p_db = ble_audio_sync_find_by_sync_id(p_data->p_le_periodic_adv_report_info->sync_id);
            if (p_db == NULL)
            {
                return;
            }
#if 0
            PROTOCOL_PRINT_INFO7("GAP_MSG_LE_PERIODIC_ADV_REPORT_INFO: sync_id %d, sync_handle 0x%x, tx_power %d, rssi %d, cte_type %d, data_status 0x%x, data_len %d",
                                 p_data->p_le_periodic_adv_report_info->sync_id,
                                 p_data->p_le_periodic_adv_report_info->sync_handle,
                                 p_data->p_le_periodic_adv_report_info->tx_power,
                                 p_data->p_le_periodic_adv_report_info->rssi,
                                 p_data->p_le_periodic_adv_report_info->cte_type,
                                 p_data->p_le_periodic_adv_report_info->data_status,
                                 p_data->p_le_periodic_adv_report_info->data_len);

            if (p_data->p_le_periodic_adv_report_info->data_len)
            {
                PROTOCOL_PRINT_INFO2("GAP_MSG_LE_PERIODIC_ADV_REPORT_INFO: data_len %d, data %s",
                                     p_data->p_le_periodic_adv_report_info->data_len,
                                     TRACE_BINARY(p_data->p_le_periodic_adv_report_info->data_len,
                                                  p_data->p_le_periodic_adv_report_info->p_data));
            }
#endif
            ble_audio_sync_handle_pa_info(p_db, p_data->p_le_periodic_adv_report_info);
            if (p_db->cb_pfn)
            {
                T_BLE_AUDIO_SYNC_CB_DATA cb_data;
                cb_data.p_le_periodic_adv_report_info = p_data->p_le_periodic_adv_report_info;
                p_db->cb_pfn(p_db, MSG_BLE_AUDIO_PA_REPORT_INFO, &cb_data);
            }
        }
        break;
#if F_BT_LE_5_2_SUPPORT
    case GAP_MSG_LE_BIGINFO_ADV_REPORT_INFO:
        {
            p_db = ble_audio_sync_find_by_sync_id(p_data->p_le_biginfo_adv_report_info->sync_id);
            if (p_db == NULL)
            {
                return;
            }
            p_db->big_info_received = true;
            memcpy(&p_db->big_info, p_data->p_le_biginfo_adv_report_info, sizeof(T_LE_BIGINFO_ADV_REPORT_INFO));
#if 0
            PROTOCOL_PRINT_INFO8("GAP_MSG_LE_BIGINFO_ADV_REPORT_INFO: sync_id %d, sync_handle 0x%x, num_bis %d, nse %d, iso_interval 0x%x, bn %d, pto %d, irc %d",
                                 p_data->p_le_biginfo_adv_report_info->sync_id,
                                 p_data->p_le_biginfo_adv_report_info->sync_handle,
                                 p_data->p_le_biginfo_adv_report_info->num_bis,
                                 p_data->p_le_biginfo_adv_report_info->nse,
                                 p_data->p_le_biginfo_adv_report_info->iso_interval,
                                 p_data->p_le_biginfo_adv_report_info->bn,
                                 p_data->p_le_biginfo_adv_report_info->pto,
                                 p_data->p_le_biginfo_adv_report_info->irc);
            PROTOCOL_PRINT_INFO8("GAP_MSG_LE_BIGINFO_ADV_REPORT_INFO: sync_id %d, sync_handle 0x%x, max_pdu %d, sdu_interval 0x%x, max_sdu %d, phy %d, framing %d, encryption %d",
                                 p_data->p_le_biginfo_adv_report_info->sync_id,
                                 p_data->p_le_biginfo_adv_report_info->sync_handle,
                                 p_data->p_le_biginfo_adv_report_info->max_pdu,
                                 p_data->p_le_biginfo_adv_report_info->sdu_interval,
                                 p_data->p_le_biginfo_adv_report_info->max_sdu,
                                 p_data->p_le_biginfo_adv_report_info->phy,
                                 p_data->p_le_biginfo_adv_report_info->framing,
                                 p_data->p_le_biginfo_adv_report_info->encryption);
#endif
            T_BLE_AUDIO_SYNC_CB_DATA cb_data;
            cb_data.p_le_biginfo_adv_report_info = p_data->p_le_biginfo_adv_report_info;
            if (p_db->cb_pfn)
            {

                p_db->cb_pfn(p_db, MSG_BLE_AUDIO_PA_BIGINFO, &cb_data);
            }
#if LE_AUDIO_BASS_SUPPORT
            if (bass_cb_pfn)
            {
                bass_cb_pfn(p_db, MSG_BLE_AUDIO_PA_BIGINFO, &cb_data);
            }
#endif
        }
        break;
#endif
#endif
#if F_BT_LE_5_1_PAST_RECIPIENT_SUPPORT
    case GAP_MSG_LE_PAST_RECIPIENT_SET_DEFAULT_PERIODIC_ADV_SYNC_TRANSFER_PARAMS:
        PROTOCOL_PRINT_INFO1("GAP_MSG_LE_PAST_RECIPIENT_SET_DEFAULT_PERIODIC_ADV_SYNC_TRANSFER_PARAMS: cause 0x%x",
                             p_data->p_le_past_recipient_set_default_periodic_adv_sync_transfer_params_rsp->cause);
        break;

    case GAP_MSG_LE_PAST_RECIPIENT_SET_PERIODIC_ADV_SYNC_TRANSFER_PARAMS:
        PROTOCOL_PRINT_INFO2("GAP_MSG_LE_PAST_RECIPIENT_SET_PERIODIC_ADV_SYNC_TRANSFER_PARAMS: cause 0x%x, conn_id 0x%x",
                             p_data->p_le_past_recipient_set_periodic_adv_sync_transfer_params_rsp->cause,
                             p_data->p_le_past_recipient_set_periodic_adv_sync_transfer_params_rsp->conn_id);
        break;
    case GAP_MSG_LE_PAST_RECIPIENT_PERIODIC_ADV_SYNC_TRANSFER_RECEIVED_INFO:
        PROTOCOL_PRINT_INFO0("GAP_MSG_LE_PAST_RECIPIENT_PERIODIC_ADV_SYNC_TRANSFER_RECEIVED_INFO");
#if LE_AUDIO_BASS_SUPPORT
        if (p_data->p_le_past_recipient_periodic_adv_sync_transfer_received_info->cause == GAP_SUCCESS)
        {
            p_db = ble_audio_sync_find_by_adv_info(
                       p_data->p_le_past_recipient_periodic_adv_sync_transfer_received_info->adv_addr,
                       p_data->p_le_past_recipient_periodic_adv_sync_transfer_received_info->adv_addr_type,
                       p_data->p_le_past_recipient_periodic_adv_sync_transfer_received_info->adv_sid);
            if (p_db == NULL)
            {
                (void)le_pa_sync_terminate_sync(
                    p_data->p_le_past_recipient_periodic_adv_sync_transfer_received_info->sync_id);
                return;
            }
            p_db->sync_id = p_data->p_le_past_recipient_periodic_adv_sync_transfer_received_info->sync_id;
            p_db->sync_handle =
                p_data->p_le_past_recipient_periodic_adv_sync_transfer_received_info->sync_handle;
            p_db->pa_sync_addr_type =
                p_data->p_le_past_recipient_periodic_adv_sync_transfer_received_info->adv_addr_type;
            memcpy(p_db->pa_sync_addr,
                   p_data->p_le_past_recipient_periodic_adv_sync_transfer_received_info->adv_addr, GAP_BD_ADDR_LEN);

            if (bass_cb_pfn)
            {
                bass_cb_pfn(p_db, MSG_BLE_AUDIO_SYNC_PAST_SRV_DATA,
                            p_data->p_le_past_recipient_periodic_adv_sync_transfer_received_info);
            }
        }
#endif
        break;
#endif
    default:
        break;
    }
}
#if LE_AUDIO_BROADCAST_SINK_ROLE
#if F_BT_LE_5_2_ISOC_BIS_RECEIVER_SUPPORT
T_BC_SYNC_DB *ble_audio_sync_find_by_big_handle(uint8_t big_handle)
{
    T_BC_SYNC_DB *p_db;
    for (uint8_t i = 0; i < bc_sync_list.count; i++)
    {
        p_db = (T_BC_SYNC_DB *)os_queue_peek(&bc_sync_list, i);
        if (p_db->big_handle == big_handle)
        {
            return p_db;
        }
    }
    return NULL;
}

void ble_audio_send_big_sync_state(T_BC_SYNC_DB *p_db, T_GAP_BIG_SYNC_RECEIVER_SYNC_STATE new_state,
                                   T_BLE_AUDIO_BIG_ACTION action,
                                   uint16_t cause)
{
    if (p_db->big_state != new_state || cause != 0)
    {
        PROTOCOL_PRINT_INFO5("ble_audio_send_big_sync_state: handle %p, state %d -> %d, action %d, cause 0x%x",
                             p_db, p_db->big_state, new_state, action, cause);
        p_db->big_state = new_state;
        T_BLE_AUDIO_BIG_SYNC_STATE state_change;
        T_BLE_AUDIO_SYNC_CB_DATA cb_data;
        state_change.sync_state = p_db->big_state;
        state_change.action = action;
        state_change.action_role = p_db->big_action_role;
        state_change.cause = cause;
        state_change.encryption = p_db->big_sync_param.encryption;
        cb_data.p_big_sync_state = &state_change;

        if (new_state == BIG_SYNC_RECEIVER_SYNC_STATE_TERMINATED ||
            new_state == BIG_SYNC_RECEIVER_SYNC_STATE_SYNCHRONIZED)
        {
            p_db->curr_big_action = BLE_AUDIO_BIG_IDLE;
            p_db->big_action_role = BLE_AUDIO_ACTION_ROLE_IDLE;
        }

        if (p_db->cb_pfn)
        {
            p_db->cb_pfn(p_db, MSG_BLE_AUDIO_BIG_SYNC_STATE, &cb_data);
        }
#if LE_AUDIO_BASS_SUPPORT
        if (bass_cb_pfn)
        {
            bass_cb_pfn(p_db, MSG_BLE_AUDIO_BIG_SYNC_STATE, &cb_data);
        }
#endif
    }
}

bool ble_audio_big_get_sync_info(T_BLE_AUDIO_SYNC_HANDLE handle,
                                 T_BIG_MGR_SYNC_RECEIVER_BIG_CREATE_SYNC_PARAM *p_sync_param)
{
    T_BC_SYNC_DB *p_db = (T_BC_SYNC_DB *)handle;
    if (bc_sync_handle_check(handle) == false)
    {
        return false;
    }
    memcpy(p_sync_param, &p_db->big_sync_param, sizeof(T_BIG_MGR_SYNC_RECEIVER_BIG_CREATE_SYNC_PARAM));
    return true;
}

bool ble_audio_big_sync_establish_int(T_BLE_AUDIO_SYNC_HANDLE handle,
                                      T_BIG_MGR_SYNC_RECEIVER_BIG_CREATE_SYNC_PARAM *p_sync_param,
                                      T_BLE_AUDIO_ACTION_ROLE role)
{
    T_GAP_CAUSE cause;
    T_BC_SYNC_DB *p_db = (T_BC_SYNC_DB *)handle;
    if (bc_sync_handle_check(handle) == false)
    {
        return false;
    }
    if (p_db && p_db->big_state == BIG_SYNC_RECEIVER_SYNC_STATE_TERMINATED)
    {
        memcpy(&p_db->big_sync_param, p_sync_param, sizeof(T_BIG_MGR_SYNC_RECEIVER_BIG_CREATE_SYNC_PARAM));
        if (bc_sync_big_dev_state.gap_big_create_sync_state ==
            GAP_BIG_MGR_SYNC_RECEIVER_CREATE_SYNC_DEV_STATE_SYNCHRONIZING)
        {
            p_db->pending_big_action = BLE_AUDIO_BIG_SYNC;
            p_db->big_action_role = role;
            return true;
        }
        cause = gap_big_mgr_sync_receiver_big_create_sync(p_db->sync_handle, p_sync_param,
                                                          &p_db->big_handle);
        p_db->pending_big_action = BLE_AUDIO_BIG_IDLE;
        if (cause == GAP_CAUSE_SUCCESS)
        {
            p_db->curr_big_action = BLE_AUDIO_BIG_SYNC;
            p_db->big_action_role = role;
            return true;
        }
        PROTOCOL_PRINT_ERROR1("ble_audio_big_sync_establish: failed, cause 0x%x", cause);
    }
    return false;
}

bool ble_audio_big_sync_establish(T_BLE_AUDIO_SYNC_HANDLE handle,
                                  T_BIG_MGR_SYNC_RECEIVER_BIG_CREATE_SYNC_PARAM *p_param)
{
    return ble_audio_big_sync_establish_int(handle, p_param, BLE_AUDIO_ACTION_ROLE_LOCAL_API);
}

bool ble_audio_big_terminate_int(T_BLE_AUDIO_SYNC_HANDLE handle, T_BLE_AUDIO_ACTION_ROLE role)
{
    T_GAP_CAUSE cause;
    T_BC_SYNC_DB *p_db = (T_BC_SYNC_DB *)handle;
    if (bc_sync_handle_check(handle) == false)
    {
        return false;
    }
    if (p_db &&
        (p_db->big_state != BIG_SYNC_RECEIVER_SYNC_STATE_TERMINATED) &&
        (p_db->big_state != BIG_SYNC_RECEIVER_SYNC_STATE_TERMINATING))
    {
        cause = gap_big_mgr_sync_receiver_big_terminate_sync(p_db->big_handle);
        if (cause == GAP_CAUSE_SUCCESS)
        {
            p_db->curr_big_action = BLE_AUDIO_BIG_TERMINATE;
            p_db->big_action_role = role;
            return true;
        }
    }
    return false;
}

bool ble_audio_big_terminate(T_BLE_AUDIO_SYNC_HANDLE handle)
{
    return ble_audio_big_terminate_int(handle, BLE_AUDIO_ACTION_ROLE_LOCAL_API);
}

bool ble_audio_get_bis_sync_state(T_BLE_AUDIO_SYNC_HANDLE handle, uint32_t *p_sync_state)
{
    uint32_t bis_sync_state = 0;
    T_BC_SYNC_DB *p_db = (T_BC_SYNC_DB *)handle;
    if (bc_sync_handle_check(handle) == false)
    {
        return false;
    }
    if (p_db && (p_db->big_state == BIG_SYNC_RECEIVER_SYNC_STATE_SYNCHRONIZED))
    {
        uint8_t i;
        for (i = 0; i < p_db->bis_num; i++)
        {
            if (p_db->bis_conn_handle_info[i].bis_idx < 1)
            {
                return false;
            }
            bis_sync_state |= (0x01 << (p_db->bis_conn_handle_info[i].bis_idx - 1));
        }
        *p_sync_state = bis_sync_state;
        return true;
    }
    return false;
}

bool ble_audio_get_bis_sync_info(T_BLE_AUDIO_SYNC_HANDLE handle, T_BLE_AUDIO_BIS_INFO *p_info)
{
    T_BC_SYNC_DB *p_db = (T_BC_SYNC_DB *)handle;
    if (bc_sync_handle_check(handle) == false || p_info == NULL)
    {
        return false;
    }
    if (p_db && (p_db->big_state == BIG_SYNC_RECEIVER_SYNC_STATE_SYNCHRONIZED))
    {
        p_info->bis_num = p_db->bis_num;
        memcpy(p_info->bis_info, p_db->bis_conn_handle_info,
               sizeof(T_BIG_MGR_BIS_CONN_HANDLE_INFO)*GAP_BIG_MGR_MAX_BIS_NUM);
        return true;
    }
    return false;
}

bool ble_audio_bis_setup_data_path(T_BLE_AUDIO_SYNC_HANDLE handle, uint8_t bis_idx,
                                   uint8_t codec_id[5], uint32_t controller_delay,
                                   uint8_t codec_len, uint8_t *p_codec_data)
{
    T_GAP_CAUSE cause;
    T_BC_SYNC_DB *p_db = (T_BC_SYNC_DB *)handle;
    if (bc_sync_handle_check(handle) == false)
    {
        return false;
    }
    if (p_db && p_db->big_state == BIG_SYNC_RECEIVER_SYNC_STATE_SYNCHRONIZED)
    {
        uint8_t i;
        for (i = 0; i < p_db->bis_num; i++)
        {
            if (p_db->bis_conn_handle_info[i].bis_idx == bis_idx)
            {
                break;
            }
        }
        if (i == p_db->bis_num)
        {
            goto error;
        }
        cause = gap_big_mgr_setup_data_path(p_db->bis_conn_handle_info[i].bis_conn_handle,
                                            DATA_PATH_ADD_OUTPUT,
                                            0, codec_id, controller_delay,
                                            codec_len, p_codec_data);
        if (cause != GAP_CAUSE_SUCCESS)
        {
            goto error;
        }
    }
    else
    {
        goto error;
    }

    return true;
error:
    PROTOCOL_PRINT_ERROR2("ble_audio_bis_setup_data_path: failed, handle %p, bis_idx %d", handle,
                          bis_idx);
    return false;
}

bool ble_audio_bis_remove_data_path(T_BLE_AUDIO_SYNC_HANDLE handle, uint8_t bis_idx)
{
    T_GAP_CAUSE cause;
    T_BC_SYNC_DB *p_db = (T_BC_SYNC_DB *)handle;
    if (bc_sync_handle_check(handle) == false)
    {
        return false;
    }
    if (p_db && p_db->big_state == BIG_SYNC_RECEIVER_SYNC_STATE_SYNCHRONIZED)
    {
        uint8_t i;
        for (i = 0; i < p_db->bis_num; i++)
        {
            if (p_db->bis_conn_handle_info[i].bis_idx == bis_idx)
            {
                break;
            }
        }
        if (i == p_db->bis_num)
        {
            goto error;
        }
        cause = gap_big_mgr_remove_data_path(p_db->bis_conn_handle_info[i].bis_conn_handle,
                                             DATA_PATH_OUTPUT_FLAG);
        if (cause != GAP_CAUSE_SUCCESS)
        {
            goto error;
        }
    }
    else
    {
        goto error;
    }

    return true;
error:
    PROTOCOL_PRINT_ERROR2("ble_audio_bis_remove_data_path: failed, handle %p, bis_idx %d", handle,
                          bis_idx);
    return false;
}


void ble_audio_sync_handle_big_dev_state_evt(uint8_t big_handle,
                                             T_BIG_MGR_SYNC_RECEIVER_DEV_STATE new_state, uint16_t cause)
{
    T_BIG_MGR_SYNC_RECEIVER_DEV_STATE old_state = bc_sync_big_dev_state;
    bc_sync_big_dev_state = new_state;
    PROTOCOL_PRINT_INFO4("ble_audio_sync_handle_big_dev_state_evt: big_handle 0x%x, gap_big_create_sync_state %d, cause 0x%x",
                         big_handle, new_state.gap_big_create_sync_state, new_state.gap_big_create_sync_state, cause);

    if (old_state.gap_big_create_sync_state != new_state.gap_big_create_sync_state)
    {
        if (new_state.gap_big_create_sync_state == GAP_BIG_MGR_SYNC_RECEIVER_CREATE_SYNC_DEV_STATE_IDLE)
        {
            T_BC_SYNC_DB *p_db;
            PROTOCOL_PRINT_INFO1("GAP_BIG_MGR_SYNC_RECEIVER_CREATE_SYNC_DEV_STATE_IDLE: cause 0x%x", cause);
            for (uint8_t i = 0; i < bc_sync_list.count; i++)
            {
                p_db = (T_BC_SYNC_DB *)os_queue_peek(&bc_sync_list, i);
                if (p_db->pending_big_action == BLE_AUDIO_BIG_SYNC)
                {
                    if (ble_audio_big_sync_establish(p_db, &p_db->big_sync_param) == false)
                    {
                        ble_audio_send_big_sync_state(p_db, p_db->big_state, BLE_AUDIO_BIG_SYNC,
                                                      GAP_ERR_REQ_FAILED);
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        else if (new_state.gap_big_create_sync_state ==
                 GAP_BIG_MGR_SYNC_RECEIVER_CREATE_SYNC_DEV_STATE_SYNCHRONIZING)
        {
            PROTOCOL_PRINT_INFO2("GAP_BIG_MGR_SYNC_RECEIVER_CREATE_SYNC_DEV_STATE_SYNCHRONIZING: big_handle 0x%x, cause 0x%x",
                                 big_handle, cause);

        }
    }
}

T_APP_RESULT ble_audio_big_sync_receiver_cb(uint8_t big_handle, uint8_t cb_type,
                                            void *p_cb_data)
{
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_BIG_MGR_CB_DATA *p_data = (T_BIG_MGR_CB_DATA *)p_cb_data;
    T_BC_SYNC_DB *p_db;

    switch (cb_type)
    {
    case MSG_BIG_MGR_SETUP_DATA_PATH:
        {
            uint8_t i;
            p_db = ble_audio_sync_find_by_big_handle(p_data->p_big_mgr_setup_data_path_rsp->big_handle);
            if (p_db == NULL || p_db->big_state != BIG_SYNC_RECEIVER_SYNC_STATE_SYNCHRONIZED)
            {
                return result;
            }
            PROTOCOL_PRINT_INFO3("MSG_BIG_MGR_SETUP_DATA_PATH: cause 0x%x, big_handle 0x%x, bis_conn_handle 0x%x",
                                 p_data->p_big_mgr_setup_data_path_rsp->cause,
                                 p_data->p_big_mgr_setup_data_path_rsp->big_handle,
                                 p_data->p_big_mgr_setup_data_path_rsp->bis_conn_handle);
            for (i = 0; i < p_db->bis_num; i++)
            {
                if (p_db->bis_conn_handle_info[i].bis_conn_handle ==
                    p_data->p_big_mgr_setup_data_path_rsp->bis_conn_handle)
                {
                    p_db->iso_data_path_set |= (1 << i);
                    break;
                }
            }
            if (i == p_db->bis_num)
            {
                return result;
            }

            if (p_db->cb_pfn)
            {
                T_BLE_AUDIO_SYNC_CB_DATA cb_data;
                T_BLE_AUDIO_BIG_SETUP_DATA_PATH set_data_path;
                set_data_path.cause = p_data->p_big_mgr_setup_data_path_rsp->cause;
                set_data_path.bis_idx = p_db->bis_conn_handle_info[i].bis_idx;
                set_data_path.bis_conn_handle = p_db->bis_conn_handle_info[i].bis_conn_handle;
                cb_data.p_setup_data_path = &set_data_path;
                p_db->cb_pfn(p_db, MSG_BLE_AUDIO_BIG_SETUP_DATA_PATH, &cb_data);
            }
        }
        break;

    case MSG_BIG_MGR_REMOVE_DATA_PATH:
        {
            uint8_t i;
            p_db = ble_audio_sync_find_by_big_handle(p_data->p_big_mgr_remove_data_path_rsp->big_handle);
            if (p_db == NULL || p_db->big_state != BIG_SYNC_RECEIVER_SYNC_STATE_SYNCHRONIZED)
            {
                return result;
            }
            PROTOCOL_PRINT_INFO3("MSG_BIG_MGR_REMOVE_DATA_PATH: cause 0x%x, big_handle 0x%x, bis_conn_handle 0x%x",
                                 p_data->p_big_mgr_remove_data_path_rsp->cause,
                                 p_data->p_big_mgr_remove_data_path_rsp->big_handle,
                                 p_data->p_big_mgr_remove_data_path_rsp->bis_conn_handle);
            for (i = 0; i < p_db->bis_num; i++)
            {
                if (p_db->bis_conn_handle_info[i].bis_conn_handle ==
                    p_data->p_big_mgr_remove_data_path_rsp->bis_conn_handle)
                {
                    p_db->iso_data_path_set &= ~(1 << i);
                    break;
                }
            }
            if (i == p_db->bis_num)
            {
                return result;
            }

            if (p_db->cb_pfn)
            {
                T_BLE_AUDIO_SYNC_CB_DATA cb_data;
                T_BLE_AUDIO_BIG_REMOVE_DATA_PATH set_data_path;
                set_data_path.cause = p_data->p_big_mgr_remove_data_path_rsp->cause;
                set_data_path.bis_idx = p_db->bis_conn_handle_info[i].bis_idx;
                set_data_path.bis_conn_handle = p_db->bis_conn_handle_info[i].bis_conn_handle;
                cb_data.p_remove_data_path = &set_data_path;
                p_db->cb_pfn(p_db, MSG_BLE_AUDIO_BIG_REMOVE_DATA_PATH, &cb_data);
            }
        }
        break;

    case MSG_BIG_MGR_SYNC_RECEIVER_BIG_SYNC_ESTABLISHED_INFO:
        {
            p_db = ble_audio_sync_find_by_sync_id(p_data->p_big_mgr_sync_rx_big_sync_established_info->sync_id);
            if (p_db == NULL)
            {
                return result;
            }
            if (p_data->p_big_mgr_sync_rx_big_sync_established_info->cause == GAP_SUCCESS)
            {
                p_db->bis_num = p_data->p_big_mgr_sync_rx_big_sync_established_info->num_bis;
                memcpy(p_db->bis_conn_handle_info,
                       p_data->p_big_mgr_sync_rx_big_sync_established_info->bis_conn_handle_info,
                       p_db->bis_num * sizeof(T_BIG_MGR_BIS_CONN_HANDLE_INFO));
                p_db->iso_data_path_set = 0;
            }

            PROTOCOL_PRINT_INFO6("MSG_BIG_MGR_SYNC_RECEIVER_BIG_SYNC_ESTABLISHED_INFO: big_handle 0x%x, sync_handle 0x%x, sync_id %d, cause 0x%x, transport_latency_big 0x%x, nse %d",
                                 p_data->p_big_mgr_sync_rx_big_sync_established_info->big_handle,
                                 p_data->p_big_mgr_sync_rx_big_sync_established_info->sync_handle,
                                 p_data->p_big_mgr_sync_rx_big_sync_established_info->sync_id,
                                 p_data->p_big_mgr_sync_rx_big_sync_established_info->cause,
                                 p_data->p_big_mgr_sync_rx_big_sync_established_info->transport_latency_big,
                                 p_data->p_big_mgr_sync_rx_big_sync_established_info->nse);
            PROTOCOL_PRINT_INFO8("MSG_BIG_MGR_SYNC_RECEIVER_BIG_SYNC_ESTABLISHED_INFO: big_handle 0x%x, sync_handle 0x%x, bn %d, pto %d, irc %d, max_pdu %d, iso_interval 0x%x, num_bis %d",
                                 p_data->p_big_mgr_sync_rx_big_sync_established_info->big_handle,
                                 p_data->p_big_mgr_sync_rx_big_sync_established_info->sync_handle,
                                 p_data->p_big_mgr_sync_rx_big_sync_established_info->bn,
                                 p_data->p_big_mgr_sync_rx_big_sync_established_info->pto,
                                 p_data->p_big_mgr_sync_rx_big_sync_established_info->irc,
                                 p_data->p_big_mgr_sync_rx_big_sync_established_info->max_pdu,
                                 p_data->p_big_mgr_sync_rx_big_sync_established_info->iso_interval,
                                 p_data->p_big_mgr_sync_rx_big_sync_established_info->num_bis);
            for (uint8_t i = 0; i < p_data->p_big_mgr_sync_rx_big_sync_established_info->num_bis; i++)
            {
                PROTOCOL_PRINT_INFO2("MSG_BIG_MGR_SYNC_RECEIVER_BIG_SYNC_ESTABLISHED_INFO: bis index %d, bis_conn_handle 0x%x",
                                     p_data->p_big_mgr_sync_rx_big_sync_established_info->bis_conn_handle_info[i].bis_idx,
                                     p_data->p_big_mgr_sync_rx_big_sync_established_info->bis_conn_handle_info[i].bis_conn_handle);
            }
        }
        break;

    case MSG_BIG_MGR_SYNC_RECEIVER_DEV_STATE_CHANGE_INFO:
        {
            ble_audio_sync_handle_big_dev_state_evt(big_handle,
                                                    p_data->p_big_mgr_sync_receiver_dev_state_change_info->state,
                                                    p_data->p_big_mgr_sync_receiver_dev_state_change_info->cause);
        }
        break;

    case MSG_BIG_MGR_SYNC_RECEIVER_SYNC_STATE_CHANGE_INFO:
        {
            T_BLE_AUDIO_BIG_ACTION big_action;
            p_db = ble_audio_sync_find_by_big_handle(
                       p_data->p_big_mgr_sync_receiver_sync_state_change_info->big_handle);
            if (p_db == NULL)
            {
                return result;
            }
            PROTOCOL_PRINT_INFO5("MSG_BIG_MGR_SYNC_RECEIVER_SYNC_STATE_CHANGE_INFO: big_handle 0x%x, sync_handle 0x%x, sync_id %d, state %d, cause 0x%x",
                                 p_data->p_big_mgr_sync_receiver_sync_state_change_info->big_handle,
                                 p_data->p_big_mgr_sync_receiver_sync_state_change_info->sync_handle,
                                 p_data->p_big_mgr_sync_receiver_sync_state_change_info->sync_id,
                                 p_data->p_big_mgr_sync_receiver_sync_state_change_info->state,
                                 p_data->p_big_mgr_sync_receiver_sync_state_change_info->cause);
            big_action = p_db->curr_big_action;
            if (p_data->p_big_mgr_sync_receiver_sync_state_change_info->state ==
                BIG_SYNC_RECEIVER_SYNC_STATE_TERMINATED)
            {
                uint8_t idx = 0;
                p_db->big_handle = 0;
                while (p_db->iso_data_path_set != 0)
                {
                    if (p_db->iso_data_path_set & (1 << idx))
                    {
                        if (p_db->cb_pfn)
                        {
                            T_BLE_AUDIO_SYNC_CB_DATA cb_data;
                            T_BLE_AUDIO_BIG_REMOVE_DATA_PATH set_data_path;
                            set_data_path.cause = 0;
                            set_data_path.bis_idx = p_db->bis_conn_handle_info[idx].bis_idx;
                            set_data_path.bis_conn_handle = p_db->bis_conn_handle_info[idx].bis_conn_handle;
                            cb_data.p_remove_data_path = &set_data_path;
                            p_db->cb_pfn(p_db, MSG_BLE_AUDIO_BIG_REMOVE_DATA_PATH, &cb_data);
                        }
                        p_db->iso_data_path_set &= ~(1 << idx);
                    }
                    idx++;
                }
            }
            ble_audio_send_big_sync_state(p_db,
                                          (T_GAP_BIG_SYNC_RECEIVER_SYNC_STATE)p_data->p_big_mgr_sync_receiver_sync_state_change_info->state,
                                          big_action,  p_data->p_big_mgr_sync_receiver_sync_state_change_info->cause);
        }
        break;

    default:
        break;
    }
    return result;
}
#endif
#endif

#if CONFIG_REALTEK_BLE_USE_UAL_API
static void ble_audio_sync_callback(uint8_t adv_addr_type, uint8_t *adv_addr, uint8_t adv_sid,
                                    uint8_t *broadcast_id, T_PA_SYNC_STATE_CHANGE_INFO *p_data)
{
    PROTOCOL_PRINT_INFO3("ble_audio_sync_callback state 0x%x, sync_id %d, sync_handle 0x%x",
                         p_data->state, p_data->sync_id, p_data->sync_handle);
    T_BC_SYNC_DB *p_db = (T_BC_SYNC_DB *)ble_audio_sync_find(adv_sid, broadcast_id);

    if (p_db == NULL)
    {
        PROTOCOL_PRINT_ERROR0("ble_audio_sync_callback: can't find sync db");
        return;
    }
    if (p_data->state == GAP_PA_SYNC_STATE_SYNCHRONIZED)
    {
        p_db->sync_id = p_data->sync_id;
        p_db->sync_handle = p_data->sync_handle;
    }
    else if (p_data->state == GAP_PA_SYNC_STATE_SYNCHRONIZING ||
             (p_data->state == GAP_PA_SYNC_STATE_TERMINATED &&
              p_data->cause == 0xFFFF))
    {
        T_BLE_AUDIO_PA_ACTION pa_action = p_db->curr_pa_action;
        ble_audio_send_pa_sync_state(p_db,
                                     (T_GAP_PA_SYNC_STATE)p_data->state,
                                     pa_action,  p_data->cause);
    }
}

void ble_audio_sync_init()
{
#if LE_AUDIO_BROADCAST_SINK_ROLE
#if F_BT_LE_5_2_ISOC_BIS_RECEIVER_SUPPORT
    ble_big_sync_register_cb_int(ble_audio_big_sync_receiver_cb);
#endif
#endif
}
#endif

#if LE_AUDIO_DEINIT
void ble_audio_sync_deinit(void)
{
    T_BC_SYNC_DB *p_db;
    while ((p_db = os_queue_out(&bc_sync_list)) != NULL)
    {
        T_BC_PA_DATA *p;
        while ((p = os_queue_out(&p_db->pa_data_queue)) != NULL)
        {
            ble_audio_mem_free(p);
        }
        if (p_db->p_mapping)
        {
            base_data_free(p_db->p_mapping);
        }
        ble_audio_mem_free(p_db);
    }
    memset(&bc_sync_pa_dev_state, 0, sizeof(bc_sync_pa_dev_state));
    memset(&bc_sync_big_dev_state, 0, sizeof(bc_sync_big_dev_state));
#if LE_AUDIO_BASS_SUPPORT
    bass_cb_pfn = NULL;
#endif
}
#endif

#endif


