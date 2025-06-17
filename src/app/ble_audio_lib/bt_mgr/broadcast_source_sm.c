#if LE_AUDIO_BROADCAST_SOURCE_ROLE

#include <string.h>
#include "trace.h"
#include "os_queue.h"
#include "broadcast_source_sm.h"
#include "gap_pa_adv.h"
#include "ble_isoch_def.h"
#include "platform_utils.h"
#if CONFIG_REALTEK_BLE_USE_UAL_API
#include "ble_extend_adv.h"
#include "ual_adv.h"
#endif
#if CONFIG_REALTEK_LE_AUDIO_USE_BLE_EXT_ADV
#include "ble_ext_adv.h"
#else
#include "gap_ext_adv.h"
#endif
#include "ble_audio_mgr.h"

typedef enum
{
    BROADCAST_SOURCE_ACTION_IDLE            = 0x00,
    BROADCAST_SOURCE_ACTION_CONFIG          = 0x01,
    BROADCAST_SOURCE_ACTION_RECONFIG        = 0x02,
    BROADCAST_SOURCE_ACTION_METADATA_UPDATE = 0x03,
    BROADCAST_SOURCE_ACTION_ESTABLISH       = 0x04,
    BROADCAST_SOURCE_ACTION_DISABLE         = 0x05,
    BROADCAST_SOURCE_ACTION_RELEASE         = 0x06,
    BROADCAST_SOURCE_ACTION_BIG_SYNC_MODE   = 0x07,
} T_BROADCAST_SOURCE_ACTION;

typedef struct t_bsrc_sm_db
{
    struct t_bsrc_sm_db *p_next;
    P_FUN_BROADCAST_SOURCE_SM_CB cb_pfn;
    T_BROADCAST_SOURCE_STATE     state;
    T_BROADCAST_SOURCE_ACTION current_action;
    uint8_t adv_sid;
    uint8_t broadcast_id[BROADCAST_ID_LEN];

//EA
    uint8_t adv_handle;
    uint8_t ext_adv_state;
#if CONFIG_REALTEK_LE_AUDIO_USE_BLE_EXT_ADV
    T_BLE_EXT_ADV_MGR_STATE ble_ext_state;
#endif
    uint8_t ext_adv_setting;
    T_GAP_LOCAL_ADDR_TYPE own_address_type;
    uint8_t broadcast_data_len;
    uint8_t *p_broadcast_data;
//PA
    uint8_t *p_basic_data;
    uint8_t  basic_data_len;
    T_GAP_PA_ADV_STATE pa_state;
    uint8_t pa_setting;
    uint16_t periodic_adv_interval_min;
    uint16_t periodic_adv_interval_max;
    uint16_t periodic_adv_prop;
//BIG
    bool big_sync_mode;
    bool big_test_mode;
    uint8_t num_bis;
    uint8_t big_handle;
    T_GAP_BIG_ISOC_BROADCAST_STATE big_state;
    uint8_t terminate_reason;
    uint8_t iso_data_path_set;
    uint8_t big_bn;
    uint16_t big_iso_interval;
    uint32_t big_transport_latency;
    T_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_PARAM big_param;
    T_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_TEST_PARAM test_big_param;
    T_BIG_MGR_BIS_CONN_HANDLE_INFO bis_conn_handle_info[GAP_BIG_MGR_MAX_BIS_NUM];
} T_BSRC_SM_DB;

T_OS_QUEUE bsrc_sm_list;

#if CONFIG_REALTEK_LE_AUDIO_USE_BLE_EXT_ADV
extern T_GAP_CAUSE ble_ext_adv_mgr_set_adv_param(uint8_t adv_handle,
                                                 uint16_t adv_event_prop,
                                                 uint32_t primary_adv_interval_min, uint32_t primary_adv_interval_max,
                                                 uint8_t primary_adv_channel_map,
                                                 T_GAP_LOCAL_ADDR_TYPE own_address_type,
                                                 T_GAP_REMOTE_ADDR_TYPE peer_address_type, uint8_t *p_peer_address,
                                                 T_GAP_ADV_FILTER_POLICY filter_policy, int8_t tx_power,
                                                 T_GAP_PHYS_PRIM_ADV_TYPE primary_adv_phy, uint8_t secondary_adv_max_skip,
                                                 T_GAP_PHYS_TYPE secondary_adv_phy, uint8_t adv_sid,
                                                 bool scan_req_notification_enable);
#endif

#define bsrc_sm_handle_check(source_handle) bsrc_sm_handle_check_int(__func__, source_handle)

bool bsrc_sm_handle_check_int(const char *p_func_name,
                              T_BROADCAST_SOURCE_HANDLE source_handle)
{
    if (source_handle != NULL)
    {
        if (os_queue_search(&bsrc_sm_list, source_handle) == true)
        {
            return true;
        }
    }
    PROTOCOL_PRINT_ERROR2("bsrc_sm_handle_check:failed, %s, source_handle %p",
                          TRACE_STRING(p_func_name), source_handle);
    return false;
}

T_BSRC_SM_DB *broadcast_source_find_by_adv_handle(uint8_t adv_handle)
{
    T_BSRC_SM_DB *p_db;
    for (uint8_t i = 0; i < bsrc_sm_list.count; i++)
    {
        p_db = (T_BSRC_SM_DB *)os_queue_peek(&bsrc_sm_list, i);
        if (p_db->adv_handle == adv_handle)
        {
            return p_db;
        }
    }
    return NULL;
}

T_BSRC_SM_DB *broadcast_source_find_by_big_handle(uint8_t big_handle)
{
    T_BSRC_SM_DB *p_db;
    for (uint8_t i = 0; i < bsrc_sm_list.count; i++)
    {
        p_db = (T_BSRC_SM_DB *)os_queue_peek(&bsrc_sm_list, i);
        if (p_db->big_handle == big_handle)
        {
            return p_db;
        }
    }
    return NULL;
}

T_BROADCAST_SOURCE_HANDLE broadcast_source_find(uint8_t adv_sid,
                                                uint8_t broadcast_id[BROADCAST_ID_LEN])
{
    T_BSRC_SM_DB *p_db;
    if (broadcast_id == NULL)
    {
        return NULL;
    }
    for (uint8_t i = 0; i < bsrc_sm_list.count; i++)
    {
        p_db = (T_BSRC_SM_DB *)os_queue_peek(&bsrc_sm_list, i);
        if (p_db->adv_sid == adv_sid && memcmp(p_db->broadcast_id, broadcast_id, BROADCAST_ID_LEN) == 0)
        {
            return p_db;
        }
    }
    return NULL;
}

void broadcast_source_send_state_change(T_BSRC_SM_DB *p_db, T_BROADCAST_SOURCE_STATE new_state,
                                        uint16_t cause)
{
    if (p_db->state != new_state)
    {
        p_db->state = new_state;
        if (p_db->cb_pfn)
        {
            T_BROADCAST_SOURCE_STATE_CHANGE state_change;
            T_BROADCAST_SOURCE_SM_CB_DATA cb_data;
            state_change.state = p_db->state;
            state_change.cause = cause;
            cb_data.p_state_change = &state_change;
            p_db->cb_pfn(p_db, MSG_BROADCAST_SOURCE_STATE_CHANGE, &cb_data);
        }
    }
}

#if CONFIG_REALTEK_BLE_USE_UAL_API
static void broadcast_iso_callback(uint8_t big_handle, T_BIG_BC_MSG_TYPE cb_type,
                                   T_BIG_RES_DATA *p_cb_data)
{
    T_BSRC_SM_DB *p_db = broadcast_source_find_by_adv_handle(p_cb_data->big_alloc_info.adv_handle);
    PROTOCOL_PRINT_INFO1("broadcast_iso_callback: scb_type %d", cb_type);
    if (p_db == NULL)
    {
        return;
    }

    PROTOCOL_PRINT_INFO1("broadcast_iso_callback: current_action 0x%x", p_db->current_action);
    if (cb_type == BIG_MGR_ALLOC_INFO)
    {
        if (p_db->current_action == BROADCAST_SOURCE_ACTION_ESTABLISH)
        {
            if (p_cb_data->big_alloc_info.cause == BIG_CAUSE_ERROR)
            {
                p_db->current_action = BROADCAST_SOURCE_ACTION_IDLE;
                broadcast_source_send_state_change(p_db, BROADCAST_SOURCE_STATE_CONFIGURED, GAP_ERR_UNSPECIFIED);
            }
            else
            {
                p_db->big_handle = p_cb_data->big_alloc_info.big_handle;
                broadcast_source_send_state_change(p_db, BROADCAST_SOURCE_STATE_STREAMING_STARTING,
                                                   GAP_CAUSE_SUCCESS);
            }
        }
    }
}
#endif

#if CONFIG_REALTEK_LE_AUDIO_USE_BLE_EXT_ADV
static void broadcast_source_adv_callback(uint8_t cb_type, void *p_cb_data)
{
    T_BLE_EXT_ADV_CB_DATA cb_data;
    memcpy(&cb_data, p_cb_data, sizeof(T_BLE_EXT_ADV_CB_DATA));
    switch (cb_type)
    {
    case BLE_EXT_ADV_STATE_CHANGE:
        {
            T_BSRC_SM_DB *p_db = broadcast_source_find_by_adv_handle(cb_data.p_ble_state_change->adv_handle);
            PROTOCOL_PRINT_INFO1("broadcast_source_adv_callback: BLE_EXT_ADV_STATE_CHANGE, state %d",
                                 cb_data.p_ble_state_change->state);
            if (p_db)
            {
                p_db->ble_ext_state = cb_data.p_ble_state_change->state;
            }
        }
        break;

    default:
        break;
    }
}
#endif

T_BROADCAST_SOURCE_HANDLE broadcast_source_add(P_FUN_BROADCAST_SOURCE_SM_CB cb_pfn)
{
    T_BSRC_SM_DB *p_db = NULL;
    p_db = ble_audio_mem_zalloc(sizeof(T_BSRC_SM_DB));
    if (p_db == NULL)
    {
        goto error;
    }
#if CONFIG_REALTEK_BLE_USE_UAL_API
    p_db->adv_handle = ble_ext_adv_mgr_create_adv(NULL);
    if (p_db->adv_handle == 0xff)
    {
        goto error;
    }
#elif CONFIG_REALTEK_LE_AUDIO_USE_BLE_EXT_ADV
    p_db->adv_handle = 0xff;
#else
    p_db->adv_handle = le_ext_adv_create_adv_handle();
    if (p_db->adv_handle == 0xff)
    {
        goto error;
    }
#endif
    p_db->ext_adv_setting |= EXT_ADV_SET_ADV_PARAS;
    p_db->broadcast_id[0] = (uint8_t)platform_random(0xFFFFFFFF);
    p_db->broadcast_id[1] = (uint8_t)(platform_random(0xFFFFFFFF) >> 8);
    p_db->broadcast_id[2] = (uint8_t)(platform_random(0xFFFFFFFF) >> 16);

    p_db->cb_pfn = cb_pfn;
    os_queue_in(&bsrc_sm_list, p_db);
    PROTOCOL_PRINT_INFO1("broadcast_source_add: success, handle %p", p_db);
    return p_db;
error:
    PROTOCOL_PRINT_ERROR0("broadcast_source_add: failed");
    if (p_db)
    {
        ble_audio_mem_free(p_db);
    }
    return NULL;
}

bool broadcast_source_update_broadcast_id(T_BROADCAST_SOURCE_HANDLE handle,
                                          uint8_t broadcast_id[BROADCAST_ID_LEN])
{
    T_BSRC_SM_DB *p_db = (T_BSRC_SM_DB *)handle;
    if (bsrc_sm_handle_check(handle) == false)
    {
        return false;
    }
    if (p_db->state != BROADCAST_SOURCE_STATE_IDLE)
    {
        PROTOCOL_PRINT_ERROR0("broadcast_source_update_broadcast_id: invalid state");
        return false;
    }
    p_db->broadcast_id[0] = broadcast_id[0];
    p_db->broadcast_id[1] = broadcast_id[1];
    p_db->broadcast_id[2] = broadcast_id[2];
    return true;
}

bool broadcast_source_set_eadv_param(T_BROADCAST_SOURCE_HANDLE handle, uint8_t adv_sid,
                                     uint32_t primary_adv_interval_min,
                                     uint32_t primary_adv_interval_max, uint8_t primary_adv_channel_map,
                                     T_GAP_LOCAL_ADDR_TYPE own_address_type, uint8_t *p_local_rand_addr,
                                     T_GAP_ADV_FILTER_POLICY filter_policy, uint8_t tx_power,
                                     T_GAP_PHYS_PRIM_ADV_TYPE primary_adv_phy, uint8_t secondary_adv_max_skip,
                                     T_GAP_PHYS_TYPE secondary_adv_phy,
                                     uint8_t broadcast_data_len, uint8_t *p_broadcast_data)
{
    T_GAP_CAUSE cause;
#if CONFIG_REALTEK_BLE_USE_UAL_API
    T_ADV_PARAM_CONF conf_msk = 0;
    T_ADV_PARAMS param;
#endif
    if (bsrc_sm_handle_check(handle) == false)
    {
        return false;
    }
    if ((broadcast_data_len > 0 && p_broadcast_data == NULL) || adv_sid > 0x0F)
    {
        PROTOCOL_PRINT_ERROR0("broadcast_source_set_eadv_param: invalid param");
        return false;
    }

    T_BSRC_SM_DB *p_db = (T_BSRC_SM_DB *)handle;
    if (p_db->state != BROADCAST_SOURCE_STATE_IDLE)
    {
        PROTOCOL_PRINT_ERROR0("broadcast_source_set_eadv_param: invalid state");
        return false;
    }
    PROTOCOL_PRINT_INFO1("broadcast_source_set_eadv_param: handle %p", handle);
    if (p_db->p_broadcast_data != NULL)
    {
        ble_audio_mem_free(p_db->p_broadcast_data);
        p_db->p_broadcast_data = NULL;
    }
    if (broadcast_data_len)
    {
        p_db->p_broadcast_data = ble_audio_mem_zalloc(broadcast_data_len);
        if (p_db->p_broadcast_data == NULL)
        {
            goto error;
        }
        p_db->broadcast_data_len = broadcast_data_len;
        memcpy(p_db->p_broadcast_data, p_broadcast_data, p_db->broadcast_data_len);
    }
    p_db->own_address_type = own_address_type;
#if CONFIG_REALTEK_BLE_USE_UAL_API
    conf_msk = ADV_EVENT_PROP_CONF | ADV_INTERVAL_RANGE_CONF | PRIM_ADV_CHANNEL_MAP_CONF |
               LOCAL_BD_TYPE_CONF | FILTER_POLICY_CONF | ADV_TX_POWER_CONF |
               PRIM_ADV_PHY_CONF | SEC_ADV_MAX_SKIP_CONF | SEC_ADV_PHY_CONF | ADV_SID_CONF |
               SCAN_REQ_NOTIFY_CONF;

    param.primary_adv_channel_map = primary_adv_channel_map;
    param.filter_policy = (T_ADV_FILTER_POLICY)filter_policy;
    param.primary_adv_phy = (T_PHYS_PRIM_ADV_TYPE)primary_adv_phy;
    param.secondary_adv_max_skip = secondary_adv_max_skip;
    param.secondary_adv_phy = (T_ADV_PHYS_TYPE)secondary_adv_phy;
    param.adv_sid = adv_sid;
    p_db->adv_sid = adv_sid;
    param.scan_req_notify_enable = false;
    param.tx_power = tx_power;
    param.local_bd_type = (T_BLE_BD_TYPE)own_address_type;
    param.peer_bd_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    param.adv_event_prop = LE_EXT_ADV_EXTENDED_ADV_NON_SCAN_NON_CONN_UNDIRECTED;
    param.adv_interval_min = primary_adv_interval_min;
    param.adv_interval_max = primary_adv_interval_max;

    cause = ble_ext_adv_mgr_create_adv_param(p_db->adv_handle, conf_msk, &param,
                                             p_db->broadcast_data_len,
                                             p_db->p_broadcast_data,
                                             0, NULL);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        goto error;
    }
    else
    {
        p_db->ext_adv_setting |= (EXT_ADV_SET_ADV_PARAS | EXT_ADV_SET_ADV_DATA);
    }
#elif CONFIG_REALTEK_LE_AUDIO_USE_BLE_EXT_ADV
    if (p_db->adv_handle == 0xff)
    {
        cause = ble_ext_adv_mgr_init_adv_params_all(&p_db->adv_handle,
                                                    LE_EXT_ADV_EXTENDED_ADV_NON_SCAN_NON_CONN_UNDIRECTED,
                                                    adv_sid, tx_power,
                                                    primary_adv_interval_min, primary_adv_interval_max,
                                                    primary_adv_channel_map, primary_adv_phy,
                                                    secondary_adv_max_skip, secondary_adv_phy,
                                                    own_address_type, GAP_REMOTE_ADDR_LE_PUBLIC,
                                                    NULL,
                                                    filter_policy, p_db->broadcast_data_len, p_db->p_broadcast_data,
                                                    0, NULL, p_local_rand_addr);
        if (cause != GAP_CAUSE_SUCCESS)
        {
            goto error;
        }
        ble_ext_adv_mgr_register_callback(broadcast_source_adv_callback, p_db->adv_handle);
    }
    else
    {
        cause = ble_ext_adv_mgr_set_adv_param(p_db->adv_handle,
                                              LE_EXT_ADV_EXTENDED_ADV_NON_SCAN_NON_CONN_UNDIRECTED,
                                              primary_adv_interval_min, primary_adv_interval_max,
                                              primary_adv_channel_map,
                                              own_address_type,
                                              GAP_REMOTE_ADDR_LE_PUBLIC, NULL,
                                              filter_policy, tx_power,
                                              primary_adv_phy, secondary_adv_max_skip,
                                              secondary_adv_phy, adv_sid,
                                              false);
        if (cause != GAP_CAUSE_SUCCESS)
        {
            goto error;
        }
        cause = ble_ext_adv_mgr_set_adv_data(p_db->adv_handle, p_db->broadcast_data_len,
                                             p_db->p_broadcast_data);
        if (cause != GAP_CAUSE_SUCCESS)
        {
            goto error;
        }
    }
#else
    cause = le_ext_adv_set_adv_param(p_db->adv_handle,
                                     LE_EXT_ADV_EXTENDED_ADV_NON_SCAN_NON_CONN_UNDIRECTED,
                                     primary_adv_interval_min, primary_adv_interval_max,
                                     primary_adv_channel_map, own_address_type,
                                     GAP_REMOTE_ADDR_LE_PUBLIC, NULL,
                                     filter_policy, tx_power,
                                     primary_adv_phy, secondary_adv_max_skip,
                                     secondary_adv_phy, adv_sid, false);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        goto error;
    }
    else
    {
        p_db->ext_adv_setting |= EXT_ADV_SET_ADV_PARAS;
    }
    cause = le_ext_adv_set_adv_data(p_db->adv_handle, p_db->broadcast_data_len, p_db->p_broadcast_data);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        goto error;
    }
    else
    {
        p_db->ext_adv_setting |= EXT_ADV_SET_ADV_DATA;
    }

    if (own_address_type == GAP_LOCAL_ADDR_LE_RANDOM)
    {
        cause = le_ext_adv_set_random(p_db->adv_handle, p_local_rand_addr);
        if (cause != GAP_CAUSE_SUCCESS)
        {
            goto error;
        }
        else
        {
            p_db->ext_adv_setting |= EXT_ADV_SET_RANDOM_ADDR;
        }
    }
#endif
    return true;
error:
    PROTOCOL_PRINT_ERROR1("broadcast_source_set_eadv_param: failed, handle %p", handle);
    return false;
}

bool broadcast_source_set_pa_param(T_BROADCAST_SOURCE_HANDLE handle,
                                   uint16_t periodic_adv_interval_min,
                                   uint16_t periodic_adv_interval_max, uint16_t periodic_adv_prop,
                                   uint8_t basic_data_len, uint8_t *p_basic_data)
{
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint8_t err_idx = 0;
    if (bsrc_sm_handle_check(handle) == false)
    {
        return false;
    }
    if ((basic_data_len > 0 && p_basic_data == NULL))
    {
        PROTOCOL_PRINT_ERROR0("broadcast_source_set_pa_param: invalid param");
        return false;
    }
    T_BSRC_SM_DB *p_db = (T_BSRC_SM_DB *)handle;
    if (p_db->state != BROADCAST_SOURCE_STATE_IDLE)
    {
        PROTOCOL_PRINT_ERROR0("broadcast_source_set_pa_param: invalid state");
        return false;
    }
    PROTOCOL_PRINT_INFO1("broadcast_source_set_pa_param: handle %p", handle);
    if (p_db->p_basic_data != NULL)
    {
        ble_audio_mem_free(p_db->p_basic_data);
        p_db->p_basic_data = NULL;
    }
    if (basic_data_len)
    {
        p_db->p_basic_data = ble_audio_mem_zalloc(basic_data_len);
        if (p_db->p_basic_data == NULL)
        {
            err_idx = 1;
            goto error;
        }
        p_db->basic_data_len = basic_data_len;
        memcpy(p_db->p_basic_data, p_basic_data, p_db->basic_data_len);
    }

#if CONFIG_REALTEK_BLE_USE_UAL_API
    cause = ble_ext_adv_mgr_create_pa(p_db->adv_handle, periodic_adv_interval_min,
                                      periodic_adv_interval_max, periodic_adv_prop,
                                      p_db->basic_data_len, p_db->p_basic_data,
                                      false);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        err_idx = 2;
        goto error;
    }
    else
    {
        p_db->pa_setting |= (PA_ADV_SET_PERIODIC_ADV_PARAS | PA_ADV_SET_PERIODIC_ADV_DATA);
    }
#else
    cause = le_pa_adv_set_periodic_adv_param(p_db->adv_handle,
                                             periodic_adv_interval_min, periodic_adv_interval_max,
                                             periodic_adv_prop);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        err_idx = 3;
        goto error;
    }
    else
    {
        p_db->pa_setting |= PA_ADV_SET_PERIODIC_ADV_PARAS;
    }
    cause = le_pa_adv_set_periodic_adv_data(p_db->adv_handle, p_db->basic_data_len, p_db->p_basic_data,
                                            false);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        err_idx = 4;
        goto error;
    }
    else
    {
        p_db->pa_setting |= PA_ADV_SET_PERIODIC_ADV_DATA;
    }
#endif
    return true;
error:
    PROTOCOL_PRINT_ERROR3("broadcast_source_set_pa_param: failed, handle %p, err_idx %d, cause %d",
                          handle, err_idx, cause);
    return false;
}

bool broadcast_source_check_state(T_BSRC_SM_DB *p_db, T_BROADCAST_SOURCE_ACTION action)
{
    if (p_db->current_action != BROADCAST_SOURCE_ACTION_IDLE)
    {
        PROTOCOL_PRINT_ERROR2("broadcast_source_check_state: handle %p, pending action %d", p_db,
                              p_db->current_action);
        return false;
    }
    switch (action)
    {
    case BROADCAST_SOURCE_ACTION_CONFIG:
        if (p_db->state != BROADCAST_SOURCE_STATE_IDLE)
        {
            goto error;
        }
        break;

    case BROADCAST_SOURCE_ACTION_RECONFIG:
        if (p_db->state != BROADCAST_SOURCE_STATE_CONFIGURED)
        {
            goto error;
        }
        break;

    case BROADCAST_SOURCE_ACTION_METADATA_UPDATE:
        if ((p_db->state != BROADCAST_SOURCE_STATE_CONFIGURED) &&
            (p_db->state != BROADCAST_SOURCE_STATE_STREAMING))
        {
            goto error;
        }
        break;

    case BROADCAST_SOURCE_ACTION_ESTABLISH:
        if (p_db->state != BROADCAST_SOURCE_STATE_CONFIGURED)
        {
            goto error;
        }
        break;

    case BROADCAST_SOURCE_ACTION_DISABLE:
        if (p_db->state != BROADCAST_SOURCE_STATE_STREAMING)
        {
            goto error;
        }
        break;

    case BROADCAST_SOURCE_ACTION_RELEASE:
        if (p_db->state != BROADCAST_SOURCE_STATE_CONFIGURED)
        {
            goto error;
        }
        break;
    default:
        break;
    }
    return true;
error:
    PROTOCOL_PRINT_ERROR2("broadcast_source_check_state: failed, state %d, action %d",
                          p_db->state, action);
    return false;
}

bool broadcast_source_handle_config_action(T_BSRC_SM_DB *p_db, uint16_t cause)
{
    T_GAP_CAUSE gap_cause = GAP_CAUSE_SUCCESS;
    uint8_t err_idx = 0;
    if (cause == GAP_SUCCESS)
    {
        if (p_db->ext_adv_state == EXT_ADV_STATE_ADVERTISING && p_db->pa_state == PA_ADV_STATE_ADVERTISING)
        {
            p_db->current_action = BROADCAST_SOURCE_ACTION_IDLE;
            broadcast_source_send_state_change(p_db, BROADCAST_SOURCE_STATE_CONFIGURED, cause);
            return true;
        }
        if (p_db->ext_adv_state == EXT_ADV_STATE_IDLE)
        {
            if (p_db->ext_adv_setting != 0)
            {
#if CONFIG_REALTEK_BLE_USE_UAL_API
                //just wait for all the settings complete
                return true;
#elif CONFIG_REALTEK_LE_AUDIO_USE_BLE_EXT_ADV
                //just for start setting
                gap_cause = ble_ext_adv_mgr_set_adv_data(p_db->adv_handle, p_db->broadcast_data_len,
                                                         p_db->p_broadcast_data);
                err_idx = 1;
                goto check_result;
#else

                gap_cause = le_ext_adv_start_setting(p_db->adv_handle, p_db->ext_adv_setting);
                err_idx = 1;
                goto check_result;
#endif
            }
        }
        if (p_db->pa_state == PA_ADV_STATE_IDLE)
        {
            if (p_db->pa_setting != 0)
            {
#if CONFIG_REALTEK_BLE_USE_UAL_API
                gap_cause = ble_ext_adv_mgr_start_pa_setting(p_db->adv_handle, p_db->pa_setting);
#else
                gap_cause = le_pa_adv_start_setting(p_db->adv_handle, p_db->pa_setting);
#endif
                if (gap_cause == GAP_CAUSE_ALREADY_IN_REQ)
                {
                    gap_cause = GAP_CAUSE_SUCCESS;
                }
                err_idx = 3;
                goto check_result;
            }
            else
            {
#if CONFIG_REALTEK_BLE_USE_UAL_API
                gap_cause = ble_enable_pa(p_db->adv_handle);
#else
                gap_cause = le_pa_adv_set_periodic_adv_enable(p_db->adv_handle, true);
#endif
                err_idx = 4;
                goto check_result;
            }
        }

        if (p_db->ext_adv_state == EXT_ADV_STATE_IDLE)
        {
            if (p_db->ext_adv_setting == 0)
            {
#if CONFIG_REALTEK_BLE_USE_UAL_API
                gap_cause = ble_enable_ext_adv(p_db->adv_handle, 0);
#elif CONFIG_REALTEK_LE_AUDIO_USE_BLE_EXT_ADV
                if (p_db->ble_ext_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
                {
                    gap_cause = ble_ext_adv_mgr_enable(p_db->adv_handle, 0);
                }
#else
                gap_cause = le_ext_adv_enable(1, &p_db->adv_handle);
#endif
                err_idx = 2;
                goto check_result;
            }
        }
    }
    else
    {
        goto error;
    }
check_result:
    if (gap_cause != GAP_CAUSE_SUCCESS)
    {
        PROTOCOL_PRINT_ERROR4("broadcast_source_handle_config_action: gap_cause 0x%x, err_idx %d, p_db->ext_adv_state %d, p_db->pa_state %d",
                              gap_cause, err_idx, p_db->ext_adv_state, p_db->pa_state);
        cause = GAP_ERR_UNSPECIFIED;
        goto error;
    }
    broadcast_source_send_state_change(p_db, BROADCAST_SOURCE_STATE_CONFIGURED_STARTING, cause);
    return true;
error:
    p_db->current_action = BROADCAST_SOURCE_ACTION_IDLE;
    broadcast_source_send_state_change(p_db, BROADCAST_SOURCE_STATE_IDLE, cause);
    return false;
}

bool broadcast_source_handle_establish_action(T_BSRC_SM_DB *p_db, uint16_t cause, bool req)
{
    T_GAP_CAUSE gap_cause = GAP_CAUSE_SUCCESS;
    if (req)
    {
#if CONFIG_REALTEK_BLE_USE_UAL_API
        if (p_db->big_test_mode)
        {
            gap_cause = le_create_test_big(p_db->adv_handle, (T_BIG_TEST_PARAM *)&p_db->test_big_param,
                                           broadcast_iso_callback);
        }
        else
        {
            gap_cause = le_create_big(p_db->adv_handle, (T_BIG_PARAM *)&p_db->big_param,
                                      broadcast_iso_callback);
        }
        if (gap_cause != GAP_CAUSE_SUCCESS)
        {
            cause = GAP_ERR_UNSPECIFIED;
            goto error;
        }
        else
        {
            goto check_result;
        }
#else
        if (p_db->big_test_mode)
        {
            gap_cause = gap_big_mgr_isoc_broadcaster_create_big_test(p_db->adv_handle,
                                                                     &p_db->test_big_param, &p_db->big_handle);
        }
        else
        {
            gap_cause = gap_big_mgr_isoc_broadcaster_create_big(p_db->adv_handle,
                                                                &p_db->big_param, &p_db->big_handle);
        }
        goto check_result;
#endif

    }
    else
    {
        if (cause == GAP_SUCCESS)
        {
            if (p_db->big_state == BIG_ISOC_BROADCAST_STATE_BROADCASTING)
            {
                p_db->current_action = BROADCAST_SOURCE_ACTION_IDLE;
                broadcast_source_send_state_change(p_db, BROADCAST_SOURCE_STATE_STREAMING, cause);
                return true;
            }
        }
        else
        {
            goto error;
        }
    }

check_result:
    if (gap_cause != GAP_CAUSE_SUCCESS)
    {
        cause = GAP_ERR_UNSPECIFIED;
        goto error;
    }
#if CONFIG_REALTEK_BLE_USE_UAL_API

#else
    broadcast_source_send_state_change(p_db, BROADCAST_SOURCE_STATE_STREAMING_STARTING, cause);
#endif
    return true;
error:
    p_db->current_action = BROADCAST_SOURCE_ACTION_IDLE;
    broadcast_source_send_state_change(p_db, BROADCAST_SOURCE_STATE_CONFIGURED, cause);
    return false;
}

bool broadcast_source_handle_disable_action(T_BSRC_SM_DB *p_db, uint16_t cause, bool req)
{
    T_GAP_CAUSE gap_cause = GAP_CAUSE_SUCCESS;
    if (req)
    {
#if CONFIG_REALTEK_BLE_USE_UAL_API
        gap_cause = le_terminate_big(p_db->big_handle, p_db->terminate_reason);
#else
        gap_cause = gap_big_mgr_isoc_broadcaster_terminate_big(p_db->big_handle, p_db->terminate_reason);
#endif
        goto check_result;
    }
    else
    {
        if ((cause == GAP_SUCCESS) || (cause == (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE)))
        {
            if (p_db->big_state == BIG_ISOC_BROADCAST_STATE_IDLE)
            {
                p_db->current_action = BROADCAST_SOURCE_ACTION_IDLE;
                if (p_db->big_sync_mode == false)
                {
                    broadcast_source_send_state_change(p_db, BROADCAST_SOURCE_STATE_IDLE, 0);
                }
                else
                {
                    broadcast_source_send_state_change(p_db, BROADCAST_SOURCE_STATE_CONFIGURED, cause);
                }
                return true;
            }
        }
        else
        {
            goto error;
        }
    }

check_result:
    if ((gap_cause != GAP_CAUSE_SUCCESS) || ((cause != GAP_SUCCESS) &&
                                             (cause != (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE))))
    {
        cause = GAP_ERR_UNSPECIFIED;
        goto error;
    }
    broadcast_source_send_state_change(p_db, BROADCAST_SOURCE_STATE_STREAMING_STOPPING, cause);
    return true;
error:
    p_db->current_action = BROADCAST_SOURCE_ACTION_IDLE;
    broadcast_source_send_state_change(p_db, BROADCAST_SOURCE_STATE_STREAMING, cause);
    return false;
}

bool broadcast_source_handle_release_action(T_BSRC_SM_DB *p_db, uint16_t cause)
{
    T_GAP_CAUSE gap_cause = GAP_CAUSE_SUCCESS;
    if (p_db->ext_adv_state == EXT_ADV_STATE_IDLE && p_db->pa_state == PA_ADV_STATE_IDLE)
    {
        p_db->current_action = BROADCAST_SOURCE_ACTION_IDLE;
        cause = 0;
        broadcast_source_send_state_change(p_db, BROADCAST_SOURCE_STATE_IDLE, cause);
        return true;
    }
    if (p_db->ext_adv_state == EXT_ADV_STATE_ADVERTISING)
    {
#if CONFIG_REALTEK_BLE_USE_UAL_API
        gap_cause = ble_disable_ext_adv(p_db->adv_handle, BLE_ADV_STOP_CAUSE_APP);
#elif CONFIG_REALTEK_LE_AUDIO_USE_BLE_EXT_ADV
        if (p_db->ble_ext_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
        {
            gap_cause = ble_ext_adv_mgr_disable(p_db->adv_handle, 0);
        }
#else
        gap_cause = le_ext_adv_disable(1, &p_db->adv_handle);
#endif
        goto check_result;
    }
    if (p_db->pa_state == PA_ADV_STATE_ADVERTISING)
    {
#if CONFIG_REALTEK_BLE_USE_UAL_API
        gap_cause = ble_disable_pa(p_db->adv_handle, BLE_ADV_STOP_CAUSE_APP);
#else
        gap_cause = le_pa_adv_set_periodic_adv_enable(p_db->adv_handle, false);
#endif
        goto check_result;
    }

check_result:
    if (gap_cause != GAP_CAUSE_SUCCESS)
    {
        cause = GAP_ERR_UNSPECIFIED;
        goto error;
    }
    broadcast_source_send_state_change(p_db, BROADCAST_SOURCE_STATE_CONFIGURED_STOPPING, cause);
    return true;
error:
    p_db->current_action = BROADCAST_SOURCE_ACTION_IDLE;
    broadcast_source_send_state_change(p_db, BROADCAST_SOURCE_STATE_CONFIGURED, cause);
    return false;
}

bool broadcast_source_handle_big_sync_mode_action(T_BSRC_SM_DB *p_db, uint16_t cause)
{
    T_GAP_CAUSE gap_cause = GAP_CAUSE_SUCCESS;
    bool ret = true;

    if (p_db->big_sync_mode == false)
    {
        if (p_db->ext_adv_state == EXT_ADV_STATE_IDLE && p_db->pa_state == PA_ADV_STATE_IDLE)
        {
            cause = 0;
            goto send_result;
        }
        if (p_db->ext_adv_state == EXT_ADV_STATE_ADVERTISING)
        {
#if CONFIG_REALTEK_BLE_USE_UAL_API
            gap_cause = ble_disable_ext_adv(p_db->adv_handle, BLE_ADV_STOP_CAUSE_APP);
#elif CONFIG_REALTEK_LE_AUDIO_USE_BLE_EXT_ADV
            if (p_db->ble_ext_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
            {
                gap_cause = ble_ext_adv_mgr_disable(p_db->adv_handle, 0);
            }
#else
            gap_cause = le_ext_adv_disable(1, &p_db->adv_handle);
#endif
            goto check_result;
        }
        if (p_db->pa_state == PA_ADV_STATE_ADVERTISING)
        {
#if CONFIG_REALTEK_BLE_USE_UAL_API
            gap_cause = ble_disable_pa(p_db->adv_handle, BLE_ADV_STOP_CAUSE_APP);
#else
            gap_cause = le_pa_adv_set_periodic_adv_enable(p_db->adv_handle, false);
#endif
            goto check_result;
        }
    }
    else
    {
        if (p_db->ext_adv_state == EXT_ADV_STATE_ADVERTISING && p_db->pa_state == PA_ADV_STATE_ADVERTISING)
        {
            cause = 0;
            goto send_result;
        }
        if (p_db->pa_state == PA_ADV_STATE_IDLE)
        {
#if CONFIG_REALTEK_BLE_USE_UAL_API
            gap_cause = ble_enable_pa(p_db->adv_handle);
#else
            gap_cause = le_pa_adv_set_periodic_adv_enable(p_db->adv_handle, true);
#endif
            goto check_result;
        }

        if (p_db->ext_adv_state == EXT_ADV_STATE_IDLE)
        {
#if CONFIG_REALTEK_BLE_USE_UAL_API
            gap_cause = ble_enable_ext_adv(p_db->adv_handle, 0);
#elif defined CONFIG_REALTEK_LE_AUDIO_USE_BLE_EXT_ADV
            if (p_db->ble_ext_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
            {
                gap_cause = ble_ext_adv_mgr_enable(p_db->adv_handle, 0);
            }
#else
            gap_cause = le_ext_adv_enable(1, &p_db->adv_handle);
#endif
            goto check_result;
        }
    }

check_result:
    if (gap_cause != GAP_CAUSE_SUCCESS)
    {
        cause = GAP_ERR_UNSPECIFIED;
        ret = false;
        goto send_result;
    }
    return true;
send_result:
    {
        T_BROADCAST_SOURCE_SM_CB_DATA cb_data;
        T_BROADCAST_SOURCE_BIG_SYNC_MODE sync_mode;
        uint8_t cb_type;
        sync_mode.cause = cause;
        sync_mode.big_sync_mode = p_db->big_sync_mode;
        cb_data.p_big_sync_mode = &sync_mode;
        cb_type = MSG_BROADCAST_SOURCE_BIG_SYNC_MODE;
        p_db->current_action = BROADCAST_SOURCE_ACTION_IDLE;
        p_db->cb_pfn(p_db, cb_type, &cb_data);
    }
    return ret;
}

bool broadcast_source_check_action(T_BSRC_SM_DB *p_db, uint8_t msg_type, uint16_t cause, bool req)
{
    T_BROADCAST_SOURCE_ACTION action = p_db->current_action;
    PROTOCOL_PRINT_INFO5("broadcast_source_check_action: handle 0x%x, current_action %d, msg_type 0x%x, cause 0x%x, req %d",
                         p_db, p_db->current_action, msg_type, cause, req);
    switch (p_db->current_action)
    {
    case BROADCAST_SOURCE_ACTION_CONFIG:
        if (broadcast_source_handle_config_action(p_db, cause) == false)
        {
            goto error;
        }
        break;

    case BROADCAST_SOURCE_ACTION_RECONFIG:
    case BROADCAST_SOURCE_ACTION_METADATA_UPDATE:
        if (req)
        {
#if CONFIG_REALTEK_BLE_USE_UAL_API
            //skip , just wait
            break;
#else
            T_GAP_CAUSE gap_cause = GAP_CAUSE_SUCCESS;
            gap_cause = le_pa_adv_start_setting(p_db->adv_handle, PA_ADV_SET_PERIODIC_ADV_DATA);
            if (gap_cause != GAP_CAUSE_SUCCESS)
            {
                cause = GAP_ERR_UNSPECIFIED;
            }
            else
            {
                break;
            }
#endif
        }
        else
        {
            if (msg_type != GAP_MSG_LE_PA_ADV_START_SETTING)
            {
                break;
            }
        }

        if (p_db->cb_pfn)
        {
            T_BROADCAST_SOURCE_SM_CB_DATA cb_data;
            uint8_t cb_type;
            cb_data.cause = cause;
            if (p_db->current_action == BROADCAST_SOURCE_ACTION_RECONFIG)
            {
                cb_type = MSG_BROADCAST_SOURCE_RECONFIG;
            }
            else
            {
                cb_type = MSG_BROADCAST_SOURCE_METADATA_UPDATE;
            }
            p_db->current_action = BROADCAST_SOURCE_ACTION_IDLE;
            p_db->cb_pfn(p_db, cb_type, &cb_data);
        }
        break;

    case BROADCAST_SOURCE_ACTION_ESTABLISH:
        if (broadcast_source_handle_establish_action(p_db, cause, req) == false)
        {
            goto error;
        }
        break;

    case BROADCAST_SOURCE_ACTION_DISABLE:
        if (broadcast_source_handle_disable_action(p_db, cause, req) == false)
        {
            goto error;
        }
        break;

    case BROADCAST_SOURCE_ACTION_RELEASE:
        if (broadcast_source_handle_release_action(p_db, cause) == false)
        {
            goto error;
        }
        break;

    case BROADCAST_SOURCE_ACTION_BIG_SYNC_MODE:
        if (broadcast_source_handle_big_sync_mode_action(p_db, cause) == false)
        {
            goto error;
        }
        break;
    default:
        break;
    }
    return true;
error:
    PROTOCOL_PRINT_ERROR4("broadcast_source_check_action: failed, handle 0x%x, pending action %d, current_action %d, cause 0x%x",
                          p_db, action, p_db->current_action, cause);
    return false;
}

bool broadcast_source_get_info(T_BROADCAST_SOURCE_HANDLE handle, T_BROADCAST_SOURCE_INFO *p_info)
{
    T_BSRC_SM_DB *p_db = (T_BSRC_SM_DB *)handle;
    if (bsrc_sm_handle_check(handle) == false)
    {
        return false;
    }
    if (p_info)
    {
        p_info->state = p_db->state;
        p_info->adv_sid = p_db->adv_sid;
        memcpy(p_info->broadcast_id, p_db->broadcast_id, BROADCAST_ID_LEN);
        p_info->adv_handle = p_db->adv_handle;
        p_info->big_handle = p_db->big_handle;
        p_info->ext_adv_state = (T_BLE_EXT_ADV_STATE)p_db->ext_adv_state;
        p_info->pa_state = p_db->pa_state;
        p_info->big_state = p_db->big_state;
        p_info->own_address_type = p_db->own_address_type;
        p_info->big_bn = p_db->big_bn;
        p_info->big_iso_interval = p_db->big_iso_interval;
        p_info->big_transport_latency = p_db->big_transport_latency;
        return true;
    }
    return false;
}

bool broadcast_source_config(T_BROADCAST_SOURCE_HANDLE handle)
{
    T_BSRC_SM_DB *p_db = (T_BSRC_SM_DB *)handle;
    if (bsrc_sm_handle_check(handle) == false)
    {
        return false;
    }
    if (p_db)
    {
        PROTOCOL_PRINT_INFO1("broadcast_source_config: handle %p", handle);
        if (broadcast_source_check_state(p_db, BROADCAST_SOURCE_ACTION_CONFIG))
        {
            p_db->current_action = BROADCAST_SOURCE_ACTION_CONFIG;
            return broadcast_source_check_action(p_db, 0, GAP_SUCCESS, true);
        }
    }
    return false;
}

bool broadcast_source_update_base_data(T_BSRC_SM_DB *p_db, uint8_t basic_data_len,
                                       uint8_t *p_basic_data)
{
    T_GAP_CAUSE gap_cause = GAP_CAUSE_SUCCESS;
    if (p_db->p_basic_data != NULL)
    {
        ble_audio_mem_free(p_db->p_basic_data);
        p_db->p_basic_data = NULL;
    }
    if (basic_data_len)
    {
        p_db->p_basic_data = ble_audio_mem_zalloc(basic_data_len);
        if (p_db->p_basic_data == NULL)
        {
            return false;
        }
        p_db->basic_data_len = basic_data_len;
        memcpy(p_db->p_basic_data, p_basic_data, p_db->basic_data_len);
    }

#if CONFIG_REALTEK_BLE_USE_UAL_API
    gap_cause = ble_ext_adv_mgr_set_pa_data(p_db->adv_handle, p_db->basic_data_len,
                                            p_db->p_basic_data, false);
#else
    gap_cause = le_pa_adv_set_periodic_adv_data(p_db->adv_handle, p_db->basic_data_len,
                                                p_db->p_basic_data,
                                                false);
#endif
    if (gap_cause != GAP_CAUSE_SUCCESS)
    {
        return false;
    }
    return true;
}

bool broadcast_source_reconfig(T_BROADCAST_SOURCE_HANDLE handle, uint8_t basic_data_len,
                               uint8_t *p_basic_data)
{
    T_BSRC_SM_DB *p_db = (T_BSRC_SM_DB *)handle;
    if (bsrc_sm_handle_check(handle) == false)
    {
        return false;
    }
    if (p_db)
    {
        PROTOCOL_PRINT_INFO1("broadcast_source_reconfig: handle %p", handle);
        if (broadcast_source_check_state(p_db, BROADCAST_SOURCE_ACTION_RECONFIG))
        {
            if (p_db->basic_data_len == basic_data_len)
            {
                if (memcmp(p_db->p_basic_data, p_basic_data, p_db->basic_data_len) == 0)
                {
                    return true;
                }
            }

            if (broadcast_source_update_base_data(p_db, basic_data_len, p_basic_data) == false)
            {
                return false;
            }
            p_db->current_action = BROADCAST_SOURCE_ACTION_RECONFIG;
            return broadcast_source_check_action(p_db, 0, GAP_SUCCESS, true);
        }
    }
    return false;
}

bool broadcast_source_pa_update(T_BROADCAST_SOURCE_HANDLE handle, uint8_t pa_data_len,
                                uint8_t *p_pa_data)
{
    T_BSRC_SM_DB *p_db = (T_BSRC_SM_DB *)handle;
    if (bsrc_sm_handle_check(handle) == false)
    {
        return false;
    }
    if (pa_data_len > 0 && p_pa_data == NULL)
    {
        PROTOCOL_PRINT_ERROR0("broadcast_source_pa_update: invalid param");
        return false;
    }
    if (p_db)
    {
        PROTOCOL_PRINT_INFO1("broadcast_source_pa_update: handle %p", handle);
        if (broadcast_source_check_state(p_db, BROADCAST_SOURCE_ACTION_METADATA_UPDATE))
        {
            if (p_db->basic_data_len == pa_data_len)
            {
                if (memcmp(p_db->p_basic_data, p_pa_data, p_db->basic_data_len) == 0)
                {
                    return true;
                }
            }
            if (broadcast_source_update_base_data(p_db, pa_data_len, p_pa_data) == false)
            {
                return false;
            }
            p_db->current_action = BROADCAST_SOURCE_ACTION_METADATA_UPDATE;
            return broadcast_source_check_action(p_db, 0, GAP_SUCCESS, true);
        }
    }
    return false;
}

bool broadcast_source_establish(T_BROADCAST_SOURCE_HANDLE handle,
                                T_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_PARAM create_big_param)
{
    T_BSRC_SM_DB *p_db = (T_BSRC_SM_DB *)handle;
    if (bsrc_sm_handle_check(handle) == false)
    {
        return false;
    }
    if (p_db)
    {
        PROTOCOL_PRINT_INFO1("broadcast_source_establish: handle %p", handle);
        if (broadcast_source_check_state(p_db, BROADCAST_SOURCE_ACTION_ESTABLISH))
        {
            p_db->big_sync_mode = true;
            p_db->big_test_mode = false;
            p_db->num_bis = create_big_param.num_bis;
            memcpy(&p_db->big_param, &create_big_param, sizeof(T_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_PARAM));
            p_db->current_action = BROADCAST_SOURCE_ACTION_ESTABLISH;
            return broadcast_source_check_action(p_db, 0, GAP_SUCCESS, true);
        }
    }
    return false;
}

bool broadcast_source_establish_test_mode(T_BROADCAST_SOURCE_HANDLE handle,
                                          T_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_TEST_PARAM test_big_param)
{
    T_BSRC_SM_DB *p_db = (T_BSRC_SM_DB *)handle;
    if (bsrc_sm_handle_check(handle) == false)
    {
        return false;
    }
    if (p_db)
    {
        PROTOCOL_PRINT_INFO1("broadcast_source_establish_test_mode: handle %p", handle);
        if (broadcast_source_check_state(p_db, BROADCAST_SOURCE_ACTION_ESTABLISH))
        {
            p_db->big_sync_mode = true;
            p_db->big_test_mode = true;
            p_db->num_bis = test_big_param.num_bis;
            memcpy(&p_db->test_big_param, &test_big_param,
                   sizeof(T_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_TEST_PARAM));
            p_db->current_action = BROADCAST_SOURCE_ACTION_ESTABLISH;
            return broadcast_source_check_action(p_db, 0, GAP_SUCCESS, true);
        }
    }
    return false;
}

bool broadcast_source_disable(T_BROADCAST_SOURCE_HANDLE handle, uint8_t reason)
{
    T_BSRC_SM_DB *p_db = (T_BSRC_SM_DB *)handle;
    if (bsrc_sm_handle_check(handle) == false)
    {
        return false;
    }
    if (p_db)
    {
        PROTOCOL_PRINT_INFO1("broadcast_source_disable: handle %p", handle);
        if (broadcast_source_check_state(p_db, BROADCAST_SOURCE_ACTION_DISABLE))
        {
            p_db->terminate_reason = reason;
            p_db->current_action = BROADCAST_SOURCE_ACTION_DISABLE;
            return broadcast_source_check_action(p_db, 0, GAP_SUCCESS, true);
        }
    }
    return false;
}

bool broadcast_source_release(T_BROADCAST_SOURCE_HANDLE handle)
{
    T_BSRC_SM_DB *p_db = (T_BSRC_SM_DB *)handle;
    if (bsrc_sm_handle_check(handle) == false)
    {
        return false;
    }
    if (p_db)
    {
        PROTOCOL_PRINT_INFO1("broadcast_source_release: handle %p", handle);
        if (broadcast_source_check_state(p_db, BROADCAST_SOURCE_ACTION_RELEASE))
        {
            p_db->current_action = BROADCAST_SOURCE_ACTION_RELEASE;
            return broadcast_source_check_action(p_db, 0, GAP_SUCCESS, true);
        }
    }
    return false;
}

bool broadcast_source_big_sync_mode(T_BROADCAST_SOURCE_HANDLE handle, bool big_sync_mode)
{
    T_BSRC_SM_DB *p_db = (T_BSRC_SM_DB *)handle;
    if (bsrc_sm_handle_check(handle) == false)
    {
        return false;
    }
    if (p_db)
    {
        if (p_db->state != BROADCAST_SOURCE_STATE_STREAMING || p_db->big_sync_mode == big_sync_mode)
        {
            PROTOCOL_PRINT_ERROR1("broadcast_source_big_sync_mode: failed, invalid state %d", p_db->state);
            return false;
        }
        PROTOCOL_PRINT_INFO2("broadcast_source_big_sync_mode: handle %p, big_sync_mode %d", handle,
                             big_sync_mode);
        p_db->big_sync_mode = big_sync_mode;
        p_db->current_action = BROADCAST_SOURCE_ACTION_BIG_SYNC_MODE;
        return broadcast_source_check_action(p_db, 0, GAP_SUCCESS, true);
    }
    return false;
}

bool broadcast_source_get_bis_conn_handle(T_BROADCAST_SOURCE_HANDLE handle, uint8_t bis_idx,
                                          uint16_t *p_bis_conn_handle)
{
    T_BSRC_SM_DB *p_db = (T_BSRC_SM_DB *)handle;
    if (bsrc_sm_handle_check(handle) == false)
    {
        return false;
    }
    if (p_bis_conn_handle == NULL)
    {
        return false;
    }
    if (p_db && p_db->state == BROADCAST_SOURCE_STATE_STREAMING)
    {
        uint8_t i;
        for (i = 0; i < p_db->num_bis; i++)
        {
            if (p_db->bis_conn_handle_info[i].bis_idx == bis_idx)
            {
                *p_bis_conn_handle = p_db->bis_conn_handle_info[i].bis_conn_handle;
                return true;
            }
        }
    }
    return  false;
}

bool broadcast_source_setup_data_path(T_BROADCAST_SOURCE_HANDLE handle, uint8_t bis_idx,
                                      uint8_t codec_id[5], uint32_t controller_delay,
                                      uint8_t codec_len, uint8_t *p_codec_data)
{
    T_GAP_CAUSE cause;
    T_BSRC_SM_DB *p_db = (T_BSRC_SM_DB *)handle;
    if (bsrc_sm_handle_check(handle) == false)
    {
        return false;
    }
    if (p_db && p_db->state == BROADCAST_SOURCE_STATE_STREAMING)
    {
        uint8_t i;
        for (i = 0; i < p_db->num_bis; i++)
        {
            if (p_db->bis_conn_handle_info[i].bis_idx == bis_idx)
            {
                break;
            }
        }
        if (i == p_db->num_bis)
        {
            goto error;
        }
        cause = gap_big_mgr_setup_data_path(p_db->bis_conn_handle_info[i].bis_conn_handle,
                                            DATA_PATH_ADD_INPUT,
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
    PROTOCOL_PRINT_ERROR2("broadcast_source_setup_data_path: failed, handle %p, bis_idx %d", handle,
                          bis_idx);
    return false;
}

bool broadcast_source_remove_data_path(T_BROADCAST_SOURCE_HANDLE handle, uint8_t bis_idx)
{
    T_GAP_CAUSE cause;
    T_BSRC_SM_DB *p_db = (T_BSRC_SM_DB *)handle;
    if (bsrc_sm_handle_check(handle) == false)
    {
        return false;
    }
    if (p_db && p_db->state == BROADCAST_SOURCE_STATE_STREAMING)
    {
        uint8_t i;
        for (i = 0; i < p_db->num_bis; i++)
        {
            if (p_db->bis_conn_handle_info[i].bis_idx == bis_idx)
            {
                break;
            }
        }
        if (i == p_db->num_bis)
        {
            goto error;
        }
        cause = gap_big_mgr_remove_data_path(p_db->bis_conn_handle_info[i].bis_conn_handle,
                                             DATA_PATH_INPUT_FLAG);
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
    PROTOCOL_PRINT_ERROR2("broadcast_source_remove_data_path: failed, handle %p, bis_idx %d", handle,
                          bis_idx);
    return false;
}

T_BIG_MGR_BIS_CONN_HANDLE_INFO *broadcast_source_get_bis_info(T_BROADCAST_SOURCE_HANDLE handle,
                                                              uint8_t *p_bis_num)
{
    T_BSRC_SM_DB *p_db = (T_BSRC_SM_DB *)handle;
    if (bsrc_sm_handle_check(handle) == false)
    {
        return NULL;
    }
    if (p_bis_num == NULL)
    {
        PROTOCOL_PRINT_ERROR0("broadcast_source_get_bis_info invalid parameter");
        return NULL;
    }
    if (p_db && p_db->state == BROADCAST_SOURCE_STATE_STREAMING)
    {
        *p_bis_num = p_db->num_bis;
        return p_db->bis_conn_handle_info;
    }
    return NULL;
}

void broadcast_source_handle_gap_cb(uint8_t cb_type, T_LE_CB_DATA *p_data)
{
    T_BSRC_SM_DB *p_db;
    switch (cb_type)
    {
    case GAP_MSG_LE_EXT_ADV_STATE_CHANGE_INFO:
        {
            p_db = broadcast_source_find_by_adv_handle(p_data->p_le_ext_adv_state_change_info->adv_handle);
            if (p_db == NULL)
            {
                return;
            }
            p_db->ext_adv_state = (T_GAP_EXT_ADV_STATE)p_data->p_le_ext_adv_state_change_info->state;
            if ((p_db->ext_adv_state == EXT_ADV_STATE_IDLE) ||
                (p_db->ext_adv_state == EXT_ADV_STATE_ADVERTISING))
            {
                broadcast_source_check_action(p_db, cb_type, p_data->p_le_ext_adv_state_change_info->cause, false);
            }
        }
        break;

    case GAP_MSG_LE_EXT_ADV_START_SETTING:
        {
            p_db = broadcast_source_find_by_adv_handle(p_data->p_le_ext_adv_start_setting_rsp->adv_handle);
            if (p_db == NULL)
            {
                return;
            }
            if (p_data->p_le_ext_adv_start_setting_rsp->cause == GAP_SUCCESS)
            {
                p_db->ext_adv_setting = 0;
            }
            broadcast_source_check_action(p_db, cb_type, p_data->p_le_ext_adv_start_setting_rsp->cause, false);
        }
        break;

    case GAP_MSG_LE_PA_ADV_STATE_CHANGE_INFO:
        {
            p_db = broadcast_source_find_by_adv_handle(p_data->p_le_pa_adv_state_change_info->adv_handle);
            if (p_db == NULL)
            {
                return;
            }
            p_db->pa_state = (T_GAP_PA_ADV_STATE)p_data->p_le_pa_adv_state_change_info->state;

            if ((p_db->pa_state == PA_ADV_STATE_IDLE) ||
                (p_db->pa_state == PA_ADV_STATE_WAIT_EXT_ADV_STATE_ADVERTISING) ||
                (p_db->pa_state == PA_ADV_STATE_ADVERTISING))
            {
                broadcast_source_check_action(p_db, cb_type, p_data->p_le_pa_adv_state_change_info->cause, false);
            }
        }
        break;

    case GAP_MSG_LE_PA_ADV_START_SETTING:
        {
            p_db = broadcast_source_find_by_adv_handle(p_data->p_le_pa_adv_start_setting_rsp->adv_handle);
            if (p_db == NULL)
            {
                return;
            }
            if (p_data->p_le_pa_adv_start_setting_rsp->cause == GAP_SUCCESS)
            {
                p_db->pa_setting = 0;

            }
            broadcast_source_check_action(p_db, cb_type, p_data->p_le_pa_adv_start_setting_rsp->cause, false);
        }
        break;

    case GAP_MSG_LE_PA_ADV_SET_PERIODIC_ADV_ENABLE:
        {
            p_db = broadcast_source_find_by_adv_handle(
                       p_data->p_le_pa_adv_set_periodic_adv_enable_rsp->adv_handle);
            if (p_db == NULL)
            {
                return;
            }
            if (p_data->p_le_pa_adv_set_periodic_adv_enable_rsp->cause != GAP_SUCCESS)
            {
                broadcast_source_check_action(p_db, cb_type, p_data->p_le_pa_adv_set_periodic_adv_enable_rsp->cause,
                                              false);
            }
        }
        break;
    case GAP_MSG_LE_EXT_ADV_REMOVE_SET:
        {
        }
        break;
    default:
        break;
    }
}

T_APP_RESULT broadcast_source_isoc_broadcaster_cb(uint8_t big_handle, uint8_t cb_type,
                                                  void *p_cb_data)
{
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_BIG_MGR_CB_DATA *p_data = (T_BIG_MGR_CB_DATA *)p_cb_data;
    T_BSRC_SM_DB *p_db;

    switch (cb_type)
    {
    case MSG_BIG_MGR_SETUP_DATA_PATH:
        {
            uint8_t i;
            p_db = broadcast_source_find_by_big_handle(p_data->p_big_mgr_setup_data_path_rsp->big_handle);
            if (p_db == NULL || p_db->big_state != BIG_ISOC_BROADCAST_STATE_BROADCASTING)
            {
                return result;
            }
            PROTOCOL_PRINT_INFO3("MSG_BIG_MGR_SETUP_DATA_PATH: cause 0x%x, big_handle 0x%x, bis_conn_handle 0x%x",
                                 p_data->p_big_mgr_setup_data_path_rsp->cause,
                                 p_data->p_big_mgr_setup_data_path_rsp->big_handle,
                                 p_data->p_big_mgr_setup_data_path_rsp->bis_conn_handle);
            for (i = 0; i < p_db->num_bis; i++)
            {
                if (p_db->bis_conn_handle_info[i].bis_conn_handle ==
                    p_data->p_big_mgr_setup_data_path_rsp->bis_conn_handle)
                {
                    break;
                }
            }
            if (i == p_db->num_bis)
            {
                return result;
            }
            p_db->iso_data_path_set |= (1 << i);
            if (p_db->cb_pfn)
            {
                T_BROADCAST_SOURCE_SM_CB_DATA cb_data;
                T_BROADCAST_SOURCE_SETUP_DATA_PATH set_data_path;
                set_data_path.cause = p_data->p_big_mgr_setup_data_path_rsp->cause;
                set_data_path.bis_idx = p_db->bis_conn_handle_info[i].bis_idx;
                set_data_path.bis_conn_handle = p_db->bis_conn_handle_info[i].bis_conn_handle;
                cb_data.p_setup_data_path = &set_data_path;
                p_db->cb_pfn(p_db, MSG_BROADCAST_SOURCE_SETUP_DATA_PATH, &cb_data);
            }
        }
        break;

    case MSG_BIG_MGR_REMOVE_DATA_PATH:
        {
            uint8_t i;
            PROTOCOL_PRINT_INFO3("MSG_BIG_MGR_REMOVE_DATA_PATH: cause 0x%x, big_handle 0x%x, bis_conn_handle 0x%x",
                                 p_data->p_big_mgr_remove_data_path_rsp->cause,
                                 p_data->p_big_mgr_remove_data_path_rsp->big_handle,
                                 p_data->p_big_mgr_remove_data_path_rsp->bis_conn_handle);
            p_db = broadcast_source_find_by_big_handle(p_data->p_big_mgr_remove_data_path_rsp->big_handle);
            if (p_db == NULL)
            {
                return result;
            }

            for (i = 0; i < p_db->num_bis; i++)
            {
                if (p_db->bis_conn_handle_info[i].bis_conn_handle ==
                    p_data->p_big_mgr_remove_data_path_rsp->bis_conn_handle)
                {
                    break;
                }
            }

            if (i == p_db->num_bis)
            {
                return result;
            }
            p_db->iso_data_path_set &= ~(1 << i);

            if (p_db->cb_pfn)
            {
                T_BROADCAST_SOURCE_SM_CB_DATA cb_data;
                T_BROADCAST_SOURCE_REMOVE_DATA_PATH rmv_data_path;
                rmv_data_path.cause = p_data->p_big_mgr_remove_data_path_rsp->cause;
                rmv_data_path.bis_conn_handle = p_data->p_big_mgr_remove_data_path_rsp->bis_conn_handle;
                cb_data.p_remove_data_path = &rmv_data_path;
                p_db->cb_pfn(p_db, MSG_BROADCAST_SOURCE_REMOVE_DATA_PATH, &cb_data);
            }
        }
        break;

    case MSG_BIG_MGR_ISOC_BROADCAST_STATE_CHANGE_INFO:
        {
            PROTOCOL_PRINT_INFO4("broadcast_source_isoc_broadcaster_cb BIG_BROADCAST_STATE_CHANGE_INFO: cause 0x%x, big_handle 0x%x, adv_handle 0x%x, new_state 0x%x",
                                 p_data->p_big_mgr_isoc_broadcast_state_change_info->cause,
                                 p_data->p_big_mgr_isoc_broadcast_state_change_info->big_handle,
                                 p_data->p_big_mgr_isoc_broadcast_state_change_info->adv_handle,
                                 p_data->p_big_mgr_isoc_broadcast_state_change_info->new_state);
            p_db = broadcast_source_find_by_adv_handle(
                       p_data->p_big_mgr_isoc_broadcast_state_change_info->adv_handle);
            if (p_db == NULL)
            {
                p_db = broadcast_source_find_by_big_handle(
                           p_data->p_big_mgr_isoc_broadcast_state_change_info->big_handle);
                if (p_db == NULL)
                {
                    PROTOCOL_PRINT_ERROR0("broadcast_source_isoc_broadcaster_cb BIG_BROADCAST_STATE_CHANGE_INFO: db is NULL");
                    return result;
                }
            }
            p_db->big_state = (T_GAP_BIG_ISOC_BROADCAST_STATE)
                              p_data->p_big_mgr_isoc_broadcast_state_change_info->new_state;
            if ((p_db->big_state == BIG_ISOC_BROADCAST_STATE_IDLE) ||
                (p_db->big_state == BIG_ISOC_BROADCAST_STATE_WAIT_EXT_ADV_STATE_PA_ADV_STATE_ADVERTISING) ||
                (p_db->big_state == BIG_ISOC_BROADCAST_STATE_WAIT_PA_ADV_STATE_ADVERTISING) ||
                (p_db->big_state == BIG_ISOC_BROADCAST_STATE_WAIT_EXT_ADV_STATE_ADVERTISING) ||
                (p_db->big_state == BIG_ISOC_BROADCAST_STATE_BROADCASTING))
            {
                broadcast_source_check_action(p_db, 0, p_data->p_big_mgr_isoc_broadcast_state_change_info->cause,
                                              false);
            }

            if (p_db->big_state == BIG_ISOC_BROADCAST_STATE_IDLE)
            {
                p_db->big_handle = 0;
                p_db->big_bn = 0;
                p_db->big_iso_interval = 0;
                p_db->big_transport_latency = 0;
            }

            if (p_db->big_state == BIG_ISOC_BROADCAST_STATE_TERMINATING &&
                p_db->iso_data_path_set != 0)
            {
                uint8_t idx = 0;
                while (p_db->iso_data_path_set != 0)
                {
                    if (p_db->iso_data_path_set & (1 << idx))
                    {
                        if (p_db->cb_pfn)
                        {
                            T_BROADCAST_SOURCE_SM_CB_DATA cb_data;
                            T_BROADCAST_SOURCE_REMOVE_DATA_PATH rmv_data_path;
                            rmv_data_path.cause = 0;
                            rmv_data_path.bis_conn_handle = p_db->bis_conn_handle_info[idx].bis_conn_handle;
                            cb_data.p_remove_data_path = &rmv_data_path;
                            p_db->cb_pfn(p_db, MSG_BROADCAST_SOURCE_REMOVE_DATA_PATH, &cb_data);
                        }
                        p_db->iso_data_path_set &= ~(1 << idx);
                    }
                    idx++;
                }
            }
        }
        break;

    case MSG_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_CMPL_INFO:
        PROTOCOL_PRINT_INFO7("MSG_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_CMPL_INFO: big_handle 0x%x, adv_handle 0x%x, cause 0x%x, big_sync_delay 0x%x, transport_latency_big 0x%x, phy %d, nse %d",
                             p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->big_handle,
                             p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->adv_handle,
                             p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->cause,
                             p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->big_sync_delay,
                             p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->transport_latency_big,
                             p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->phy,
                             p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->nse);
        PROTOCOL_PRINT_INFO6("MSG_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_CMPL_INFO: bn %d, pto %d, irc %d, max_pdu %d, iso_interval 0x%x, num_bis %d",
                             p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->bn,
                             p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->pto,
                             p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->irc,
                             p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->max_pdu,
                             p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->iso_interval,
                             p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->num_bis);
        for (uint8_t i = 0; i < p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->num_bis; i++)
        {
            PROTOCOL_PRINT_INFO2("MSG_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_CMPL_INFO: bis index %d, bis_conn_handle 0x%x",
                                 p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->bis_conn_handle_info[i].bis_idx,
                                 p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->bis_conn_handle_info[i].bis_conn_handle);
        }
        p_db = broadcast_source_find_by_adv_handle(
                   p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->adv_handle);
        if (p_db == NULL)
        {
            return result;
        }
        p_db->big_bn = p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->bn;
        p_db->big_iso_interval = p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->iso_interval;
        p_db->big_transport_latency = p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->transport_latency_big;
        p_db->iso_data_path_set = 0;
        if (p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->cause == GAP_SUCCESS)
        {
            memcpy(p_db->bis_conn_handle_info,
                   p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->bis_conn_handle_info,
                   GAP_BIG_MGR_MAX_BIS_NUM * sizeof(T_BIG_MGR_BIS_CONN_HANDLE_INFO));
        }
        broadcast_source_check_action(p_db, 0, p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->cause,
                                      false);
        break;

    default:
        break;
    }
    return result;
}

#if LE_AUDIO_DEINIT
void broadcast_source_deinit(void)
{
    T_BSRC_SM_DB *p_db;
    while ((p_db = os_queue_out(&bsrc_sm_list)) != NULL)
    {
        if (p_db->p_broadcast_data != NULL)
        {
            ble_audio_mem_free(p_db->p_broadcast_data);
        }

        if (p_db->p_basic_data != NULL)
        {
            ble_audio_mem_free(p_db->p_basic_data);
        }
        ble_audio_mem_free(p_db);
    }
}
#endif
#endif
