/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "trace.h"
#include "gap_ext_adv.h"
#include "ble_ext_adv.h"
#include "stdlib.h"
#include "os_msg.h"
#include "gap_conn_le.h"
#include "bt_types.h"
#include "gap_bond_le.h"
#include "ble_mgr_int.h"

/*============================================================================*
 *                              Constants
 *============================================================================*/
/** @brief Idle advertising set */
#define APP_IDLE_ADV_SET 0xFF
/** @brief Idle advertising parameter update flags */
#define APP_IDLE_UPDATE_FLAGS    0xFF
/** @brief stop advertising just for update advertising parameters, when update success, advertsing will be enable again*/
#define APP_STOP_FOR_UPDATE 0xFF

extern bool le_get_conn_local_addr(uint16_t conn_handle, uint8_t *bd_addr, uint8_t *bd_type);
/*============================================================================*
 *                         Types
 *============================================================================*/
/** @brief  used to store Asynchronous action.*/
typedef struct
{
    uint8_t  adv_handle;
    uint8_t  expected_action;/**< expected_action: @ref T_BLE_EXT_ADV_ACTION.*/
    uint16_t param;/**< update_flags: @ref EXT_ADV_SET_ADV_PARAS, @ref EXT_ADV_SET_ADV_DATA, @ref EXT_ADV_SET_SCAN_RSP_DATA, @ref EXT_ADV_SET_RANDOM_ADDR*/
} T_EXTEND_ADV_EVENT;

/**
* @brief define the adv action for each adv set
* BLE_EXT_ADV_ACTION_IDLE:nothing todo
*
* BLE_EXT_ADV_ACTION_START: the application want to start adv and ble stack will handle this action now or later
* note: this action is  asynchronous
*
* BLE_EXT_ADV_ACTION_UPDATE: the application want to update adv params and ble stack will handle this action now or later
* note: this action is  asynchronous
*
* BLE_EXT_ADV_ACTION_STOP: the application want to stop adv and ble stack will handle this action now or later
* note: this action is  asynchronous
*/
typedef enum
{
    BLE_EXT_ADV_ACTION_IDLE,
    BLE_EXT_ADV_ACTION_START,
    BLE_EXT_ADV_ACTION_UPDATE,
    BLE_EXT_ADV_ACTION_STOP,
} T_BLE_EXT_ADV_ACTION;

/**
* @brief define pending adv action for each adv set, only asynchronous api has pending action,
* means the action is doing but not finished.
* BLE_EXT_ADV_PENDING_ACTION_IDLE:nothing to do
*
* BLE_EXT_ADV_PENDING_ACTION_START:the action of start adv now is doing but not finished
*
* BLE_EXT_ADV_PENDING_ACTION_STOP: the action of stop adv now is doing but not finished
*
* BLE_EXT_ADV_PENDING_ACTION_UPDATE: the action of update adv params now is doing but not finished
*/
typedef enum
{
    BLE_EXT_ADV_PENDING_ACTION_IDLE,
    BLE_EXT_ADV_PENDING_ACTION_START,
    BLE_EXT_ADV_PENDING_ACTION_UPDATE,
    BLE_EXT_ADV_PENDING_ACTION_STOP,
} T_BLE_EXT_ADV_PENDING_ACTION;

/**
* @brief define the data structure for each adv set
*/
typedef struct
{
    P_FUN_GAP_APP_CB         app_callback;
    bool                     is_used;
    uint8_t                  adv_handle;

    bool                     adv_param_set;
    uint8_t                  pending_start_stop_num;
    uint16_t                 pending_cause;
    uint8_t                  init_flags;
    T_BLE_EXT_ADV_MGR_STATE  ble_ext_adv_mgr_adv_state;
    uint8_t                  app_cause;
    uint8_t                  pending_action;

    T_GAP_EXT_ADV_STATE      ext_adv_state;

    //adv param
    uint16_t                 adv_event_prop;
    uint16_t                 adv_interval_min;
    uint16_t                 adv_interval_max;
    T_GAP_LOCAL_ADDR_TYPE    own_address_type;
    T_GAP_REMOTE_ADDR_TYPE   peer_address_type;
    uint8_t                  peer_address[6];
    T_GAP_ADV_FILTER_POLICY  filter_policy;
    uint16_t                 duration;
    uint8_t                  primary_adv_channel_map;
    T_GAP_PHYS_PRIM_ADV_TYPE primary_adv_phy;
    uint8_t                  secondary_adv_max_skip;
    T_GAP_PHYS_TYPE          secondary_adv_phy;
    uint8_t                  adv_sid;

    //adv param tx power Range: -127 to +20,  Units: dBm,  +127:Host has no preference
    int8_t                   adv_tx_power;

    //adv data
    uint16_t                 adv_data_len;
    uint8_t                  *adv_data;

    //scan response data
    uint16_t                 scan_response_data_len;
    uint8_t                  *scan_response_data;

    //random addr
    uint8_t                  random_addr[GAP_BD_ADDR_LEN];
} T_APP_EXT_ADV;

/*============================================================================*
 *                              Variables
 *============================================================================*/
/**< advertising set table */
static T_APP_EXT_ADV *ble_ext_adv_set_table;
/**< extend adv event queue handle, used to store Asynchronous action */
static void *extend_adv_event_handle;
/**< the maximum of Advertising set number */
static uint8_t ble_ext_adv_handle_num_max;

static T_APP_EXT_ADV app_ext_adv_defult_value =
{
    .adv_handle = APP_IDLE_ADV_SET,
    .init_flags = APP_IDLE_UPDATE_FLAGS,
};

T_APP_EXT_ADV *ble_ext_adv_mgr_find_by_handle(uint8_t adv_handle);
/*============================================================================*
 *                              Functions
 *============================================================================*/

/** @brief create a queue to store extend advertising event*/
void ble_ext_adv_queue_init()
{
    if (os_msg_queue_create(&extend_adv_event_handle, "ble_evtQ", MAX_NUM_OF_EXTEND_ADV_EVENT,
                            sizeof(T_EXTEND_ADV_EVENT)) != true)
    {
        BTM_PRINT_ERROR0("ble_extend_adv_queue_init: os_msg_queue_create fail");
    };
}

T_GAP_CAUSE ble_ext_adv_mgr_send_msg(uint8_t adv_handle, uint8_t action, uint16_t param)
{
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;
    T_EXTEND_ADV_EVENT extend_adv_event;
    extend_adv_event.adv_handle = adv_handle;
    extend_adv_event.expected_action = action;
    extend_adv_event.param = param;

    if (extend_adv_event_handle == NULL)
    {
        BTM_PRINT_ERROR0("ble_ext_adv_mgr_send_msg: extend_adv_event_handle NULL");
        return GAP_CAUSE_INVALID_STATE;
    }

    if (os_msg_send(extend_adv_event_handle, &extend_adv_event, 0) != true)
    {
        /*extend adv queue full*/
        uint32_t p_msg_num = 0;
        os_msg_queue_peek(extend_adv_event_handle, &p_msg_num);
        BTM_PRINT_ERROR1("ble_ext_adv_mgr_send_msg: fail extend adv queue msg num 0x%x", p_msg_num);
        result = GAP_CAUSE_NO_RESOURCE;;
    }
    else
    {
        T_APP_EXT_ADV *p_adv;
        p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);
        if (p_adv != NULL &&
            (action == BLE_EXT_ADV_ACTION_START || action == BLE_EXT_ADV_ACTION_STOP))
        {
            p_adv->pending_start_stop_num++;
        }
    }
    BTM_PRINT_INFO4("ble_ext_adv_mgr_send_msg: adv_handle %d, action %d, param 0x%x, result %d",
                    adv_handle, action, param, result);
    return result;
}

void ble_ext_adv_update_adv_state(T_APP_EXT_ADV *p_adv, T_BLE_EXT_ADV_STOP_CAUSE stop_cause,
                                  T_BLE_EXT_ADV_MGR_STATE new_state, uint8_t app_cause)
{
    if (p_adv->ble_ext_adv_mgr_adv_state != new_state)
    {
        p_adv->ble_ext_adv_mgr_adv_state = new_state;
        if (p_adv->app_callback)
        {
            T_BLE_EXT_ADV_CB_DATA cb_data;
            T_BLE_EXT_ADV_STATE_CHANGE adv_state;
            adv_state.state = p_adv->ble_ext_adv_mgr_adv_state;
            adv_state.stop_cause = stop_cause;
            adv_state.adv_handle = p_adv->adv_handle;
            adv_state.app_cause = app_cause;
            cb_data.p_ble_state_change = &adv_state;
            p_adv->app_callback(BLE_EXT_ADV_STATE_CHANGE, &cb_data);
        }
    }
}

T_GAP_CAUSE ble_ext_adv_action_start(T_APP_EXT_ADV *p_adv, uint16_t duration)
{
    T_GAP_CAUSE ret = GAP_CAUSE_SUCCESS;
    ret = le_ext_adv_set_adv_enable_param(p_adv->adv_handle, duration, 0);

    if (ret == GAP_CAUSE_SUCCESS)
    {
        ret = le_ext_adv_enable(1, &p_adv->adv_handle);
    }

    BTM_PRINT_INFO3("ble_ext_adv_action_start: adv_handle %d, duration %d, result %d",
                    p_adv->adv_handle, duration, ret);

    if (ret != GAP_CAUSE_SUCCESS)
    {
        if (p_adv->ble_ext_adv_mgr_adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
        {
            ble_ext_adv_update_adv_state(p_adv, BLE_EXT_ADV_STOP_CAUSE_ENABLE_FAILED,
                                         BLE_EXT_ADV_MGR_ADV_DISABLED, 0);
        }
    }
    else
    {
        p_adv->pending_action = BLE_EXT_ADV_PENDING_ACTION_START;
    }
    return ret;
}

T_GAP_CAUSE ble_ext_adv_action_stop(T_APP_EXT_ADV *p_adv, uint8_t cause)
{
    T_GAP_CAUSE ret = GAP_CAUSE_SUCCESS;
    ret = le_ext_adv_disable(1, &p_adv->adv_handle);

    BTM_PRINT_INFO2("ble_ext_adv_action_stop: adv_handle %d, result %d",
                    p_adv->adv_handle, ret);
    p_adv->app_cause = cause;

    if (ret == GAP_CAUSE_SUCCESS)
    {
        p_adv->pending_action = BLE_EXT_ADV_PENDING_ACTION_STOP;
    }
    return ret;
}

T_GAP_CAUSE ble_ext_adv_action_update(T_APP_EXT_ADV *p_adv, uint8_t update_flags)
{
    T_GAP_CAUSE ret = GAP_CAUSE_SUCCESS;
    if (p_adv->adv_param_set == false)
    {
        update_flags |= p_adv->init_flags;
        update_flags |= EXT_ADV_SET_ADV_PARAS;
    }
    ret = le_ext_adv_start_setting(p_adv->adv_handle, update_flags);

    BTM_PRINT_INFO4("ble_ext_adv_action_update: adv_handle %d, adv_param_set %d, update_flags 0x%x, result %d",
                    p_adv->adv_handle, p_adv->adv_param_set, update_flags, ret);

    if (ret == GAP_CAUSE_SUCCESS)
    {
        p_adv->pending_action = BLE_EXT_ADV_PENDING_ACTION_UPDATE;
        if (update_flags & EXT_ADV_SET_ADV_PARAS)
        {
            p_adv->adv_param_set = true;
            p_adv->init_flags = 0;
        }
    }
    return ret;
}

T_GAP_CAUSE ble_ext_adv_mgr_init(uint8_t adv_handle_num)
{
    bool use_extended = true;
    le_set_gap_param(GAP_PARAM_USE_EXTENDED_ADV, sizeof(use_extended), &use_extended);

    uint32_t size;
    ble_ext_adv_handle_num_max = adv_handle_num;
    size = ble_ext_adv_handle_num_max * sizeof(T_APP_EXT_ADV);

    ble_ext_adv_queue_init();
    le_bond_cfg_local_key_distribute(3, 1);

    if (le_ext_adv_init(adv_handle_num) == GAP_CAUSE_SUCCESS)
    {
        ble_ext_adv_set_table = (T_APP_EXT_ADV *)calloc(1, size);
    }
#if BLE_MGR_INIT_DEBUG
    BTM_PRINT_TRACE3("ble_ext_adv_mgr_init: version %d, support nums %d, table_alloc %d",
                     BLE_EXT_ADV_MGR_VERSION, adv_handle_num, ble_ext_adv_set_table);
#endif
    if (ble_ext_adv_set_table == NULL)
    {
        ble_ext_adv_handle_num_max = 0;
        return GAP_CAUSE_NO_RESOURCE;
    }
    else
    {
        /*set default value for ble_ext_adv_set_table*/
        for (int i = 0; i < ble_ext_adv_handle_num_max; i++)
        {
            ble_ext_adv_set_table[i] = app_ext_adv_defult_value;
        }
    }

    return GAP_CAUSE_SUCCESS;
}

T_APP_EXT_ADV *ble_ext_adv_mgr_allocate_adv_handle(uint8_t adv_handle)
{
    uint8_t i;
    for (i = 0; i < ble_ext_adv_handle_num_max; i++)
    {
        T_APP_EXT_ADV *p_adv = &ble_ext_adv_set_table[i];
        if (!p_adv->is_used)
        {
            p_adv->is_used = true;
            p_adv->adv_handle = adv_handle;
            BTM_PRINT_TRACE2("ble_ext_adv_mgr_allocate_adv_handle: success, idx %d, adv_handle %d",
                             i, p_adv->adv_handle);
            return p_adv;
        }
    }
    BTM_PRINT_ERROR0("ble_ext_adv_mgr_allocate_adv_handle: failed");

    return NULL;
}

T_GAP_CAUSE ble_ext_adv_mgr_init_adv_params_all(uint8_t *adv_handle, uint16_t adv_event_prop,
                                                uint8_t adv_sid, uint8_t tx_power,
                                                uint32_t primary_adv_interval_min, uint32_t primary_adv_interval_max,
                                                uint8_t primary_adv_channel_map, T_GAP_PHYS_PRIM_ADV_TYPE primary_adv_phy,
                                                uint8_t secondary_adv_max_skip, T_GAP_PHYS_TYPE secondary_adv_phy,
                                                T_GAP_LOCAL_ADDR_TYPE own_address_type, T_GAP_REMOTE_ADDR_TYPE peer_address_type,
                                                uint8_t *p_peer_address,
                                                T_GAP_ADV_FILTER_POLICY filter_policy, uint16_t adv_data_len, uint8_t *p_adv_data,
                                                uint16_t scan_data_len, uint8_t *p_scan_data, uint8_t *random_address)
{
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;
    (*adv_handle) = le_ext_adv_create_adv_handle();
    if ((*adv_handle) != APP_IDLE_ADV_SET)
    {
        T_APP_EXT_ADV *p_adv = ble_ext_adv_mgr_allocate_adv_handle(*adv_handle);

        if (p_adv != NULL)
        {
            /*adv param*/
            p_adv->adv_event_prop = adv_event_prop;
            p_adv->adv_interval_min = primary_adv_interval_min;
            p_adv->adv_interval_max = primary_adv_interval_max;
            p_adv->own_address_type = own_address_type;
            p_adv->peer_address_type = peer_address_type;
            p_adv->filter_policy = filter_policy;
            p_adv->adv_tx_power = tx_power;
            p_adv->primary_adv_phy = primary_adv_phy;
            p_adv->secondary_adv_phy = secondary_adv_phy;
            p_adv->primary_adv_channel_map = primary_adv_channel_map;
            p_adv->primary_adv_phy = primary_adv_phy;
            p_adv->secondary_adv_max_skip = secondary_adv_max_skip;
            p_adv->adv_sid = adv_sid;

            if (p_peer_address != NULL)
            {
                memcpy(p_adv->peer_address, p_peer_address, GAP_BD_ADDR_LEN);
            }

            /*update_flags */
            p_adv->init_flags = EXT_ADV_SET_ADV_PARAS;

            bool scan_req_notification_enable = false;
            le_ext_adv_set_adv_param((*adv_handle),
                                     adv_event_prop,
                                     primary_adv_interval_min,
                                     primary_adv_interval_max,
                                     primary_adv_channel_map,
                                     own_address_type,
                                     peer_address_type,
                                     p_peer_address,
                                     filter_policy,
                                     tx_power,
                                     primary_adv_phy,
                                     secondary_adv_max_skip,
                                     secondary_adv_phy,
                                     adv_sid,
                                     scan_req_notification_enable);

            /*random address*/
            if ((random_address != NULL) &&
                (own_address_type == GAP_LOCAL_ADDR_LE_RANDOM || own_address_type == GAP_LOCAL_ADDR_LE_RAP_OR_RAND))
            {
                memcpy(p_adv->random_addr, random_address, GAP_BD_ADDR_LEN);
                p_adv->init_flags |= EXT_ADV_SET_RANDOM_ADDR;
                le_ext_adv_set_random((*adv_handle), random_address);
            }

            if (p_adv_data != NULL &&
                !(adv_event_prop & GAP_EXT_ADV_EVT_PROP_DIRECTED_ADV))
            {
                p_adv->adv_data = p_adv_data;
                p_adv->adv_data_len = adv_data_len;

                p_adv->init_flags |= EXT_ADV_SET_ADV_DATA;
                le_ext_adv_set_adv_data((*adv_handle), adv_data_len,
                                        (uint8_t *)p_adv->adv_data);

            }

            if (p_scan_data != NULL &&
                (adv_event_prop & GAP_EXT_ADV_EVT_PROP_SCANNABLE_ADV))
            {
                p_adv->scan_response_data = p_scan_data;
                p_adv->scan_response_data_len = scan_data_len;

                p_adv->init_flags |= EXT_ADV_SET_SCAN_RSP_DATA;
                le_ext_adv_set_scan_response_data((*adv_handle), scan_data_len,
                                                  (uint8_t *)p_adv->scan_response_data);
            }
            result = GAP_CAUSE_SUCCESS;
        }
    }
    else
    {
        result = GAP_CAUSE_NO_RESOURCE;
    }
#if BLE_MGR_INIT_DEBUG
    BTM_PRINT_INFO5("ble_ext_adv_mgr_init_adv_params_all: adv_handle %d,adv_data_len %d,p_adv_data 0x%x,random_address 0x%x, result %d",
                    *adv_handle, adv_data_len, p_adv_data, random_address, result);
#endif
    return result;
}

T_GAP_CAUSE ble_ext_adv_mgr_init_adv_params(uint8_t *adv_handle, uint16_t adv_event_prop,
                                            uint32_t primary_adv_interval_min, uint32_t primary_adv_interval_max,
                                            T_GAP_LOCAL_ADDR_TYPE own_address_type, T_GAP_REMOTE_ADDR_TYPE peer_address_type,
                                            uint8_t *p_peer_address,
                                            T_GAP_ADV_FILTER_POLICY filter_policy, uint16_t adv_data_len, uint8_t *p_adv_data,
                                            uint16_t scan_data_len, uint8_t *p_scan_data, uint8_t *random_address)
{
    uint8_t  primary_adv_channel_map = GAP_ADVCHAN_ALL;
    uint8_t tx_power = 127;
    T_GAP_PHYS_PRIM_ADV_TYPE primary_adv_phy = GAP_PHYS_PRIM_ADV_1M;
    uint8_t secondary_adv_max_skip = 0;
    T_GAP_PHYS_TYPE secondary_adv_phy = GAP_PHYS_1M;
    uint8_t adv_sid = 0;
    return ble_ext_adv_mgr_init_adv_params_all(adv_handle, adv_event_prop, adv_sid, tx_power,
                                               primary_adv_interval_min, primary_adv_interval_max,
                                               primary_adv_channel_map, primary_adv_phy,
                                               secondary_adv_max_skip, secondary_adv_phy,
                                               own_address_type, peer_address_type,
                                               p_peer_address,
                                               filter_policy, adv_data_len, p_adv_data,
                                               scan_data_len, p_scan_data, random_address);
}

T_APP_EXT_ADV *ble_ext_adv_mgr_find_by_handle(uint8_t adv_handle)
{
    uint8_t i;
    for (i = 0; i < ble_ext_adv_handle_num_max; i++)
    {
        T_APP_EXT_ADV *p_adv = &ble_ext_adv_set_table[i];

        if (p_adv->is_used && p_adv->adv_handle == adv_handle)
        {
            return p_adv;
        }
    }

    BTM_PRINT_ERROR1("ble_ext_adv_mgr_find_by_handle: not found adv_handle %d", adv_handle);
    return NULL;
}

bool ble_ext_adv_mgr_find_pending_adv_action()
{
    uint8_t i;
    for (i = 0; i < ble_ext_adv_handle_num_max; i++)
    {
        T_APP_EXT_ADV *p_adv = &ble_ext_adv_set_table[i];

        if (p_adv->is_used && p_adv->pending_action != BLE_EXT_ADV_PENDING_ACTION_IDLE)
        {
            BTM_PRINT_INFO2("ble_ext_adv_mgr_find_pending_adv_action: has pending_action %d ,adv_handle %d",
                            p_adv->pending_action, p_adv->adv_handle);
            return true;
        }
    }

    return false;
}

T_GAP_CAUSE ble_ext_adv_mgr_start_set_adv_param(uint8_t adv_handle, T_APP_EXT_ADV *p_adv,
                                                uint8_t update_flags)
{
    T_GAP_DEV_STATE dev_state;
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;
    le_get_gap_param(GAP_PARAM_DEV_STATE, &dev_state);
    if (dev_state.gap_init_state == GAP_INIT_STATE_INIT)
    {
        return GAP_CAUSE_INVALID_STATE;
    }
    if (ble_ext_adv_mgr_find_pending_adv_action())
    {
        return ble_ext_adv_mgr_send_msg(adv_handle, BLE_EXT_ADV_ACTION_UPDATE, update_flags);
    }

    result = ble_ext_adv_action_update(p_adv, update_flags);

    return result;
}

T_GAP_CAUSE ble_ext_adv_mgr_set_adv_data(uint8_t adv_handle, uint16_t adv_data_len,
                                         uint8_t *p_adv_data)
{
    T_APP_EXT_ADV *p_adv;
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;

    p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);

    if (p_adv == NULL)
    {
        result = GAP_CAUSE_NOT_FIND;
        goto error;
    }

    if (p_adv_data == NULL)
    {
        result = GAP_CAUSE_INVALID_PARAM;
        goto error;
    }

    p_adv->adv_data = p_adv_data;
    p_adv->adv_data_len = adv_data_len;

    le_ext_adv_set_adv_data(adv_handle, p_adv->adv_data_len, (uint8_t *)p_adv->adv_data);
    result = ble_ext_adv_mgr_start_set_adv_param(adv_handle, p_adv, EXT_ADV_SET_ADV_DATA);

error:
    BTM_PRINT_INFO2("ble_ext_adv_mgr_set_adv_data: adv_handle %d, result %d", adv_handle, result);
    return result;
}

T_GAP_CAUSE ble_ext_adv_mgr_set_scan_response_data(uint8_t adv_handle, uint16_t scan_data_len,
                                                   uint8_t *p_scan_data)
{
    T_APP_EXT_ADV *p_adv;
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;

    p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);

    if (p_adv == NULL)
    {
        result = GAP_CAUSE_NOT_FIND;
        goto error;
    }

    if (p_scan_data != NULL)
    {
        p_adv->scan_response_data = p_scan_data;
        p_adv->scan_response_data_len = scan_data_len;

        le_ext_adv_set_scan_response_data(adv_handle, p_adv->scan_response_data_len,
                                          (uint8_t *)p_adv->scan_response_data);

        result = ble_ext_adv_mgr_start_set_adv_param(adv_handle, p_adv, EXT_ADV_SET_SCAN_RSP_DATA);
    }

error:
    BTM_PRINT_INFO2("ble_ext_adv_mgr_set_scan_response_data: adv_handle %d, result %d", adv_handle,
                    result);
    return result;
}

T_GAP_CAUSE ble_ext_adv_mgr_enable(uint8_t adv_handle, uint16_t duration_10ms)
{
    T_APP_EXT_ADV *p_adv;
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;
    p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);

    if (p_adv == NULL)
    {
        result = GAP_CAUSE_NOT_FIND;
        goto error;
    }

    if (p_adv->ble_ext_adv_mgr_adv_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
    {
        if (p_adv->adv_param_set == false)
        {
            ble_ext_adv_mgr_start_set_adv_param(adv_handle, p_adv, p_adv->init_flags);
        }

        if (ble_ext_adv_mgr_find_pending_adv_action())
        {
            result = ble_ext_adv_mgr_send_msg(adv_handle, BLE_EXT_ADV_ACTION_START, duration_10ms);
        }
        else
        {
            result = ble_ext_adv_action_start(p_adv, duration_10ms);
        }
    }
    else
    {
        result = GAP_CAUSE_INVALID_STATE;
        goto error;
    }

    if (result == GAP_CAUSE_SUCCESS)
    {
        p_adv->duration = duration_10ms;
        ble_ext_adv_update_adv_state(p_adv, (T_BLE_EXT_ADV_STOP_CAUSE)0, BLE_EXT_ADV_MGR_ADV_ENABLED, 0);
    }

error:
    BTM_PRINT_INFO5("ble_ext_adv_mgr_enable: adv_handle %d, duration_10ms %d, init_flags 0x%x, adv_param_set %d, result %d",
                    adv_handle, duration_10ms, ((p_adv == NULL) ? 0 : p_adv->init_flags),
                    ((p_adv == NULL) ? 0 : p_adv->adv_param_set), result);
    return result;
}

/**
*@brief    used to update adv param
*this api shall be used after @ref ble_ext_adv_mgr_init_adv_params_all() or @ref ble_ext_adv_mgr_init_adv_params().
*@note
*@param[in] adv_handle ble adv handle, each adv set has a adv handle
*@param[in] adv_event_prop @ref T_LE_EXT_ADV_LEGACY_ADV_PROPERTY
*@param[in] primary_adv_interval_min
*@param[in] primary_adv_interval_max
*@param[in] primary_adv_channel_map  @ref GAP_ADVCHAN_ALL
*@param[in] own_address_type @ref T_GAP_LOCAL_ADDR_TYPE
*@param[in] peer_address_type @ref T_GAP_REMOTE_ADDR_TYPE
*@param[in] p_peer_address  if not used, set default value NULL
*@param[in] filter_policy @ref T_GAP_ADV_FILTER_POLICY
*@param[in] tx_power Range: -127 to +20, Units: dBm, 0x7F Host has no preference, if not used, set default value 127
*@param[in] primary_adv_phy @ref T_GAP_PHYS_PRIM_ADV_TYPE  the default value is GAP_PHYS_PRIM_ADV_1M
*@param[in] secondary_adv_max_skip the default value is 0
*@param[in] secondary_adv_phy  @ref T_GAP_PHYS_TYPE
*@param[in] adv_sid the default value is 0
*@param[in] scan_req_notification_enable the default value is false
*@return    T_GAP_CAUSE
*@retval GAP_CAUSE_SUCCESS  Operation success.
*@retval GAP_CAUSE_NOT_FIND Operartion failure, the advertising handle is not found.
*/
T_GAP_CAUSE ble_ext_adv_mgr_set_adv_param(uint8_t adv_handle,
                                          uint16_t adv_event_prop,
                                          uint32_t primary_adv_interval_min, uint32_t primary_adv_interval_max,
                                          uint8_t primary_adv_channel_map,
                                          T_GAP_LOCAL_ADDR_TYPE own_address_type,
                                          T_GAP_REMOTE_ADDR_TYPE peer_address_type, uint8_t *p_peer_address,
                                          T_GAP_ADV_FILTER_POLICY filter_policy, int8_t tx_power,
                                          T_GAP_PHYS_PRIM_ADV_TYPE primary_adv_phy, uint8_t secondary_adv_max_skip,
                                          T_GAP_PHYS_TYPE secondary_adv_phy, uint8_t adv_sid,
                                          bool scan_req_notification_enable)
{
    T_APP_EXT_ADV *p_adv;
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;

    p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);

    if (p_adv == NULL)
    {
        result = GAP_CAUSE_NOT_FIND;
        goto error;
    }

    p_adv->adv_event_prop = adv_event_prop;
    p_adv->adv_interval_min = primary_adv_interval_min;
    p_adv->adv_interval_max = primary_adv_interval_max;
    p_adv->own_address_type = own_address_type;
    p_adv->peer_address_type = peer_address_type;
    p_adv->filter_policy = filter_policy;
    p_adv->adv_tx_power = tx_power;
    p_adv->primary_adv_phy = primary_adv_phy;
    p_adv->secondary_adv_phy = secondary_adv_phy;
    p_adv->adv_sid = adv_sid;
    p_adv->primary_adv_channel_map = primary_adv_channel_map;
    p_adv->secondary_adv_max_skip = secondary_adv_max_skip;

    if (p_peer_address != NULL)
    {
        memcpy(p_adv->peer_address, p_peer_address, GAP_BD_ADDR_LEN);
    }

    le_ext_adv_set_adv_param(adv_handle, adv_event_prop,
                             primary_adv_interval_min, primary_adv_interval_max,
                             primary_adv_channel_map, own_address_type,
                             peer_address_type, p_peer_address,
                             filter_policy, tx_power,
                             primary_adv_phy, secondary_adv_max_skip,
                             secondary_adv_phy, adv_sid,
                             scan_req_notification_enable);

    if (p_adv->ble_ext_adv_mgr_adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
    {
        ble_ext_adv_mgr_disable(adv_handle, APP_STOP_FOR_UPDATE);
        ble_ext_adv_mgr_start_set_adv_param(adv_handle, p_adv, EXT_ADV_SET_ADV_PARAS);
        ble_ext_adv_mgr_send_msg(adv_handle, BLE_EXT_ADV_ACTION_START, p_adv->duration);
    }
    else
    {
        ble_ext_adv_mgr_start_set_adv_param(adv_handle, p_adv, EXT_ADV_SET_ADV_PARAS);
    }

error:
    if (result != GAP_CAUSE_SUCCESS)
    {
        BTM_PRINT_INFO4("ble_ext_adv_mgr_set_adv_param: adv_handle %d, stack adv state %d, ble mgr lib adv state %d, result %d",
                        ((p_adv == NULL) ? 0 : p_adv->adv_handle),
                        ((p_adv == NULL) ? 0 : p_adv->ext_adv_state),
                        ((p_adv == NULL) ? 0 : p_adv->ble_ext_adv_mgr_adv_state), result);
    }
    return result;
}

T_GAP_CAUSE ble_ext_adv_mgr_change_adv_interval(uint8_t adv_handle, uint16_t adv_interval)
{
    T_APP_EXT_ADV *p_adv;
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;
    bool scan_req_notification_enable = false;
    p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);
    if (p_adv == NULL)
    {
        result = GAP_CAUSE_NOT_FIND;
        goto error;
    }

    if ((adv_interval >= p_adv->adv_interval_min) && (adv_interval <= p_adv->adv_interval_max))
    {
        result = GAP_CAUSE_SUCCESS;
        goto error;
    }

    result = ble_ext_adv_mgr_set_adv_param(p_adv->adv_handle,
                                           p_adv->adv_event_prop,
                                           adv_interval, adv_interval,
                                           p_adv->primary_adv_channel_map,
                                           p_adv->own_address_type,
                                           p_adv->peer_address_type, p_adv->peer_address,
                                           p_adv->filter_policy, p_adv->adv_tx_power,
                                           p_adv->primary_adv_phy, p_adv->secondary_adv_max_skip,
                                           p_adv->secondary_adv_phy, p_adv->adv_sid, scan_req_notification_enable);

error:
    BTM_PRINT_INFO5("ble_ext_adv_mgr_change_adv_interval: adv_handle %d, new_interval 0x%x,old_interval_min 0x%x,old_interval_max 0x%x,result %d ",
                    adv_handle, adv_interval,
                    ((p_adv == NULL) ? 0 : p_adv->adv_interval_min),
                    ((p_adv == NULL) ? 0 : p_adv->adv_interval_max), result);
    return result;
}

T_GAP_CAUSE ble_ext_adv_mgr_change_peer_addr(uint8_t adv_handle,
                                             T_GAP_REMOTE_ADDR_TYPE peer_addr_type,
                                             uint8_t *peer_addr)
{
    T_APP_EXT_ADV *p_adv;
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;
    bool scan_req_notification_enable = false;
    p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);
    if (p_adv == NULL || peer_addr == NULL)
    {
        result = GAP_CAUSE_NOT_FIND;
        goto error;
    }

    if ((p_adv->peer_address_type == peer_addr_type) &&
        (memcmp(p_adv->peer_address, peer_addr, 6) == 0))
    {
        result = GAP_CAUSE_SUCCESS;
        goto error;
    }

    result = ble_ext_adv_mgr_set_adv_param(p_adv->adv_handle,
                                           p_adv->adv_event_prop,
                                           p_adv->adv_interval_min, p_adv->adv_interval_max,
                                           p_adv->primary_adv_channel_map,
                                           p_adv->own_address_type,
                                           peer_addr_type, peer_addr,
                                           p_adv->filter_policy, p_adv->adv_tx_power,
                                           p_adv->primary_adv_phy, p_adv->secondary_adv_max_skip,
                                           p_adv->secondary_adv_phy, p_adv->adv_sid, scan_req_notification_enable);
error:
    BTM_PRINT_INFO4("ble_ext_adv_mgr_change_peer_addr: adv_handle %d, peer_addr %s, peer_addr_type %d, result %d",
                    adv_handle, TRACE_BDADDR(peer_addr), peer_addr_type, result);
    return result;
}

T_GAP_CAUSE ble_ext_adv_mgr_change_adv_tx_power(uint8_t adv_handle, int8_t adv_tx_power)
{
    T_APP_EXT_ADV *p_adv;
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;
    bool scan_req_notification_enable = false;
    p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);

    if (p_adv == NULL)
    {
        result = GAP_CAUSE_NOT_FIND;
        goto error;
    }

    if (p_adv->adv_tx_power == adv_tx_power)
    {
        result = GAP_CAUSE_SUCCESS;
        goto error;
    }

    if ((adv_tx_power < -127) || (adv_tx_power > 20 && adv_tx_power != 127))
    {
        result = GAP_CAUSE_INVALID_PARAM;
        goto error;
    }

    result = ble_ext_adv_mgr_set_adv_param(p_adv->adv_handle,
                                           p_adv->adv_event_prop,
                                           p_adv->adv_interval_min, p_adv->adv_interval_max,
                                           p_adv->primary_adv_channel_map,
                                           p_adv->own_address_type,
                                           p_adv->peer_address_type, p_adv->peer_address,
                                           p_adv->filter_policy, adv_tx_power,
                                           p_adv->primary_adv_phy, p_adv->secondary_adv_max_skip,
                                           p_adv->secondary_adv_phy, p_adv->adv_sid, scan_req_notification_enable);
error:
    BTM_PRINT_INFO4("ble_ext_adv_mgr_change_adv_tx_power: adv_handle %d, new_tx_power %d, old_tx_power %d, result %d",
                    adv_handle, adv_tx_power,
                    ((p_adv == NULL) ? 0 : p_adv->adv_tx_power), result);
    return result;
}

T_GAP_CAUSE ble_ext_adv_mgr_change_adv_event_prop(uint8_t adv_handle, uint16_t adv_event_prop)
{
    T_APP_EXT_ADV *p_adv;
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;
    bool scan_req_notification_enable = false;
    p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);

    if (p_adv == NULL)
    {
        result = GAP_CAUSE_NOT_FIND;
        goto error;
    }

    if (p_adv->adv_event_prop == adv_event_prop)
    {
        result = GAP_CAUSE_SUCCESS;
        goto error;
    }

    result = ble_ext_adv_mgr_set_adv_param(p_adv->adv_handle,
                                           adv_event_prop,
                                           p_adv->adv_interval_min, p_adv->adv_interval_max,
                                           p_adv->primary_adv_channel_map,
                                           p_adv->own_address_type,
                                           p_adv->peer_address_type, p_adv->peer_address,
                                           p_adv->filter_policy, p_adv->adv_tx_power,
                                           p_adv->primary_adv_phy, p_adv->secondary_adv_max_skip,
                                           p_adv->secondary_adv_phy, p_adv->adv_sid, scan_req_notification_enable);
error:
    BTM_PRINT_INFO4("ble_ext_adv_mgr_change_adv_event_prop: adv_handle %d, new_event_prop %d, old_event_prop %d, result %d",
                    adv_handle, adv_event_prop,
                    ((p_adv == NULL) ? 0 : p_adv->adv_event_prop), result);
    return result;
}

T_GAP_CAUSE ble_ext_adv_mgr_change_adv_phy(uint8_t adv_handle,
                                           T_GAP_PHYS_PRIM_ADV_TYPE primary_adv_phy,
                                           T_GAP_PHYS_TYPE secondary_adv_phy)
{
    T_APP_EXT_ADV *p_adv;
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;
    bool scan_req_notification_enable = false;
    p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);

    if (p_adv == NULL)
    {
        result = GAP_CAUSE_NOT_FIND;
        goto error;
    }

    if ((p_adv->primary_adv_phy == primary_adv_phy) && (p_adv->secondary_adv_phy == secondary_adv_phy))
    {
        result = GAP_CAUSE_SUCCESS;
        goto error;
    }

    if ((primary_adv_phy != GAP_PHYS_PRIM_ADV_1M) && (primary_adv_phy != GAP_PHYS_PRIM_ADV_CODED))
    {
        result = GAP_CAUSE_INVALID_PARAM;
        goto error;
    }

    result = ble_ext_adv_mgr_set_adv_param(p_adv->adv_handle,
                                           p_adv->adv_event_prop,
                                           p_adv->adv_interval_min, p_adv->adv_interval_max,
                                           p_adv->primary_adv_channel_map,
                                           p_adv->own_address_type,
                                           p_adv->peer_address_type, p_adv->peer_address,
                                           p_adv->filter_policy, p_adv->adv_tx_power,
                                           primary_adv_phy, p_adv->secondary_adv_max_skip,
                                           secondary_adv_phy, p_adv->adv_sid, scan_req_notification_enable);
error:
    if (result != GAP_CAUSE_SUCCESS)
    {
        BTM_PRINT_INFO4("ble_ext_adv_mgr_change_adv_phy: adv_handle %d, primary_adv_phy %d, secondary_adv_phy %d, result %d",
                        adv_handle, primary_adv_phy, secondary_adv_phy, result);
    }

    return result;
}

/**
 * @brief ble_ext_adv_mgr_change_channel_map
 * used to update primary_adv_channel_map
 * this api shall be used after @ref ble_ext_adv_mgr_init_adv_params_all() or @ref ble_ext_adv_mgr_init_adv_params().
 *@note
 *\xrefitem Added_API_2_12_1_0 "Added Since 2.12.1.0" "Added API"
 * @param adv_handle ble adv handle, each adv set has a adv handle.
 * @param primary_adv_channel_map @ref GAP_ADVCHAN_ALL or @ref GAP_ADVCHAN_37 or @ref GAP_ADVCHAN_38 or @ref GAP_ADVCHAN_39
 * @return T_GAP_CAUSE
 */
T_GAP_CAUSE ble_ext_adv_mgr_change_channel_map(uint8_t adv_handle, uint8_t primary_adv_channel_map)
{
    T_APP_EXT_ADV *p_adv;
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;
    bool scan_req_notification_enable = false;
    p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);

    if (p_adv == NULL)
    {
        result = GAP_CAUSE_NOT_FIND;
        goto error;
    }

    if (p_adv->primary_adv_channel_map == primary_adv_channel_map)
    {
        result = GAP_CAUSE_SUCCESS;
        goto error;
    }

    result = ble_ext_adv_mgr_set_adv_param(p_adv->adv_handle,
                                           p_adv->adv_event_prop,
                                           p_adv->adv_interval_min, p_adv->adv_interval_max,
                                           primary_adv_channel_map,
                                           p_adv->own_address_type,
                                           p_adv->peer_address_type, p_adv->peer_address,
                                           p_adv->filter_policy, p_adv->adv_tx_power,
                                           p_adv->primary_adv_phy, p_adv->secondary_adv_max_skip,
                                           p_adv->secondary_adv_phy, p_adv->adv_sid, scan_req_notification_enable);

error:
    BTM_PRINT_INFO3("ble_ext_adv_mgr_change_channel_map: adv_handle %d, primary_adv_channel_map %d, result %d",
                    adv_handle, primary_adv_channel_map, result);
    return result;
}

/**
 * @brief ble_ext_adv_mgr_change_filter_policy
 * used to update filter policy
 * this api shall be used after @ref ble_ext_adv_mgr_init_adv_params_all() or @ref ble_ext_adv_mgr_init_adv_params().
 *@note
 *\xrefitem Added_API_2_12_1_0 "Added Since 2.12.1.0" "Added API"
 * @param adv_handle ble adv handle, each adv set has a adv handle.
 * @param filter_policy @ref T_GAP_ADV_FILTER_POLICY
 * @return T_GAP_CAUSE
 */
T_GAP_CAUSE ble_ext_adv_mgr_change_filter_policy(uint8_t adv_handle,
                                                 T_GAP_ADV_FILTER_POLICY filter_policy)
{
    T_APP_EXT_ADV *p_adv;
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;
    bool scan_req_notification_enable = false;
    p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);

    if (p_adv == NULL)
    {
        result = GAP_CAUSE_NOT_FIND;
        goto error;
    }

    if (p_adv->filter_policy == filter_policy)
    {
        result = GAP_CAUSE_SUCCESS;
        goto error;
    }

    result = ble_ext_adv_mgr_set_adv_param(p_adv->adv_handle,
                                           p_adv->adv_event_prop,
                                           p_adv->adv_interval_min, p_adv->adv_interval_max,
                                           p_adv->primary_adv_channel_map,
                                           p_adv->own_address_type,
                                           p_adv->peer_address_type, p_adv->peer_address,
                                           filter_policy, p_adv->adv_tx_power,
                                           p_adv->primary_adv_phy, p_adv->secondary_adv_max_skip,
                                           p_adv->secondary_adv_phy, p_adv->adv_sid, scan_req_notification_enable);
error:
    BTM_PRINT_INFO3("ble_ext_adv_mgr_change_filter_policy: adv_handle %d, filter_policy %d, result %d",
                    adv_handle, filter_policy, result);
    return result;
}

/**
 * @brief ble_ext_adv_mgr_change_own_address_type
 * used to update own address type, this api shall be used after gap stack init ready.
 *@note
 *\xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 * @param adv_handle ble adv handle, each adv set has a adv handle.
 * @param own_addr_type @ref T_GAP_LOCAL_ADDR_TYPE
 * @return T_GAP_CAUSE
 */
T_GAP_CAUSE ble_ext_adv_mgr_change_own_address_type(uint8_t adv_handle,
                                                    T_GAP_LOCAL_ADDR_TYPE own_addr_type)
{
    T_APP_EXT_ADV *p_adv;
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;
    bool scan_req_notification_enable = false;
    p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);

    if (p_adv == NULL)
    {
        result = GAP_CAUSE_NOT_FIND;
        goto error;
    }

    if (p_adv->own_address_type == own_addr_type)
    {
        result = GAP_CAUSE_SUCCESS;
        goto error;
    }

    result = ble_ext_adv_mgr_set_adv_param(p_adv->adv_handle,
                                           p_adv->adv_event_prop,
                                           p_adv->adv_interval_min, p_adv->adv_interval_max,
                                           p_adv->primary_adv_channel_map,
                                           own_addr_type,
                                           p_adv->peer_address_type, p_adv->peer_address,
                                           p_adv->filter_policy, p_adv->adv_tx_power,
                                           p_adv->primary_adv_phy, p_adv->secondary_adv_max_skip,
                                           p_adv->secondary_adv_phy, p_adv->adv_sid, scan_req_notification_enable);
error:
    BTM_PRINT_INFO3("ble_ext_adv_mgr_change_own_address_type: adv_handle %d, own_addr_type %d, result %d",
                    adv_handle, own_addr_type, result);
    return result;
}

T_GAP_CAUSE ble_ext_adv_mgr_set_random(uint8_t adv_handle, uint8_t *random_address)
{
    T_APP_EXT_ADV *p_adv;
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;

    p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);

    if (p_adv == NULL || random_address == NULL)
    {
        result = GAP_CAUSE_NOT_FIND;
        goto error;
    }
    if (p_adv->own_address_type == GAP_LOCAL_ADDR_LE_PUBLIC ||
        p_adv->own_address_type == GAP_LOCAL_ADDR_LE_RAP_OR_PUBLIC)
    {
        result = GAP_CAUSE_INVALID_PARAM;
        goto error;
    }

    memcpy(p_adv->random_addr, random_address, GAP_BD_ADDR_LEN);

    le_ext_adv_set_random(adv_handle, random_address);

    if (p_adv->ble_ext_adv_mgr_adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
    {
        ble_ext_adv_mgr_disable(adv_handle, APP_STOP_FOR_UPDATE);
        ble_ext_adv_mgr_start_set_adv_param(adv_handle, p_adv, EXT_ADV_SET_RANDOM_ADDR);
        ble_ext_adv_mgr_send_msg(adv_handle, BLE_EXT_ADV_ACTION_START, p_adv->duration);
    }
    else
    {
        ble_ext_adv_mgr_start_set_adv_param(adv_handle, p_adv, EXT_ADV_SET_RANDOM_ADDR);
    }

error:
    BTM_PRINT_INFO3("ble_ext_adv_mgr_set_random: adv_handle %d, random_address %b, result %d",
                    adv_handle, TRACE_BDADDR(random_address), result);
    return result;
}

T_GAP_CAUSE ble_ext_adv_mgr_set_multi_param(uint8_t adv_handle, uint8_t *random_address,
                                            uint16_t adv_interval,
                                            uint16_t adv_data_len, uint8_t *p_adv_data, uint16_t scan_data_len,
                                            uint8_t *p_scan_data)
{
    T_APP_EXT_ADV *p_adv;
    uint8_t set_flag = 0;
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;

    p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);

    if (p_adv == NULL)
    {
        result = GAP_CAUSE_NOT_FIND;
        goto error;
    }

    set_flag = p_adv->init_flags;

    if (random_address != NULL &&
        (p_adv->own_address_type == GAP_LOCAL_ADDR_LE_RANDOM ||
         p_adv->own_address_type == GAP_LOCAL_ADDR_LE_RAP_OR_RAND))
    {
        set_flag |= EXT_ADV_SET_RANDOM_ADDR;
        memcpy(p_adv->random_addr, random_address, GAP_BD_ADDR_LEN);
        le_ext_adv_set_random(adv_handle, random_address);
    }
    if ((adv_interval <= p_adv->adv_interval_min) || (adv_interval >= p_adv->adv_interval_max))
    {
        bool scan_req_notification_enable = false;

        set_flag |= EXT_ADV_SET_ADV_PARAS;
        le_ext_adv_set_adv_param(adv_handle, p_adv->adv_event_prop,
                                 adv_interval, adv_interval,
                                 p_adv->primary_adv_channel_map, p_adv->own_address_type,
                                 p_adv->peer_address_type, p_adv->peer_address,
                                 p_adv->filter_policy, p_adv->adv_tx_power,
                                 p_adv->primary_adv_phy, p_adv->secondary_adv_max_skip,
                                 p_adv->secondary_adv_phy, p_adv->adv_sid,
                                 scan_req_notification_enable);

    }
    if (p_adv_data != NULL)
    {
        p_adv->adv_data_len = adv_data_len;
        p_adv->adv_data = p_adv_data;

        set_flag |= EXT_ADV_SET_ADV_DATA;

        le_ext_adv_set_adv_data(adv_handle, p_adv->adv_data_len, (uint8_t *)p_adv->adv_data);
    }

    if (p_scan_data != NULL)
    {

        p_adv->scan_response_data_len = scan_data_len;
        p_adv->scan_response_data = p_scan_data;

        set_flag |= EXT_ADV_SET_SCAN_RSP_DATA;

        le_ext_adv_set_scan_response_data(adv_handle, p_adv->scan_response_data_len,
                                          (uint8_t *)p_adv->scan_response_data);
    }

    if (set_flag == 0)
    {
        result = GAP_CAUSE_INVALID_PARAM;
        goto error;
    }

    if (set_flag & (EXT_ADV_SET_RANDOM_ADDR | EXT_ADV_SET_ADV_PARAS))
    {
        if (p_adv->ble_ext_adv_mgr_adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
        {
            ble_ext_adv_mgr_disable(adv_handle, APP_STOP_FOR_UPDATE);
            ble_ext_adv_mgr_start_set_adv_param(adv_handle, p_adv, set_flag);
            result = ble_ext_adv_mgr_send_msg(adv_handle, BLE_EXT_ADV_ACTION_START, p_adv->duration);
        }
        else
        {
            result = ble_ext_adv_mgr_start_set_adv_param(adv_handle, p_adv, set_flag);
        }
    }
    else
    {
        result = ble_ext_adv_mgr_start_set_adv_param(adv_handle, p_adv, set_flag);
    }

    if (result == GAP_CAUSE_SUCCESS)
    {
        if (set_flag & EXT_ADV_SET_ADV_PARAS)
        {
            p_adv->adv_param_set = true;
        }
    }

error:
    BTM_PRINT_INFO3("ble_ext_adv_mgr_set_multi_param: adv_handle %d, set_flag 0x%x, result %d",
                    adv_handle, set_flag, result);
    return result;
}

T_GAP_CAUSE ble_ext_adv_mgr_disable(uint8_t adv_handle, uint8_t app_cause)
{
    T_APP_EXT_ADV *p_adv;
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;
    p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);

    if (p_adv == NULL)
    {
        /*adv_handle not created*/
        result = GAP_CAUSE_NOT_FIND;
        goto error;
    }

    if (p_adv->ble_ext_adv_mgr_adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
    {
        if (ble_ext_adv_mgr_find_pending_adv_action())
        {
            result = ble_ext_adv_mgr_send_msg(adv_handle, BLE_EXT_ADV_ACTION_STOP, app_cause);
        }
        else
        {
            result = ble_ext_adv_action_stop(p_adv, app_cause);
        }
    }
    else
    {
        result = GAP_CAUSE_INVALID_STATE;
        goto error;
    }
    if (result == GAP_CAUSE_SUCCESS && app_cause != APP_STOP_FOR_UPDATE)
    {
        ble_ext_adv_update_adv_state(p_adv, BLE_EXT_ADV_STOP_CAUSE_APP, BLE_EXT_ADV_MGR_ADV_DISABLED,
                                     app_cause);
    }

error:
    BTM_PRINT_INFO3("ble_ext_adv_mgr_disable: adv_handle %d, app_cause 0x%x, result %d", adv_handle,
                    app_cause, result);
    return result;
}

T_GAP_CAUSE ble_ext_adv_mgr_disable_all(uint8_t app_cause)
{
    T_APP_EXT_ADV *p_adv;

    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;

    for (uint8_t i = 0; i < ble_ext_adv_handle_num_max; i++)
    {
        p_adv = &ble_ext_adv_set_table[i];

        if (p_adv != NULL && p_adv->is_used &&
            p_adv->ble_ext_adv_mgr_adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
        {
            result = ble_ext_adv_mgr_disable(p_adv->adv_handle, app_cause);

            BTM_PRINT_INFO3("ble_ext_adv_mgr_disable_all: adv_handle %d, app_cause %d, result 0x%x",
                            p_adv->adv_handle, app_cause, result);
        }
    }
    return result;
}

void ble_ext_adv_mgr_check_state(T_APP_EXT_ADV *p_adv, uint16_t cause, bool state_change)
{
    T_BLE_EXT_ADV_STOP_CAUSE stop_cause = BLE_EXT_ADV_STOP_CAUSE_UNKNOWN;
    T_BLE_EXT_ADV_MGR_STATE new_adv_state = p_adv->ble_ext_adv_mgr_adv_state;

    if (state_change)
    {
        p_adv->pending_cause = cause;
    }
    cause = p_adv->pending_cause;

    if (p_adv->pending_start_stop_num == 0)
    {
        if (p_adv->ext_adv_state == EXT_ADV_STATE_IDLE)
        {
            if (cause == (HCI_ERR | HCI_ERR_OPERATION_CANCELLED_BY_HOST))
            {
                /*EXT_ADV_STATE_IDLE because app stop*/
                stop_cause = BLE_EXT_ADV_STOP_CAUSE_APP;
                if (p_adv->app_cause != APP_STOP_FOR_UPDATE)
                {
                    new_adv_state = BLE_EXT_ADV_MGR_ADV_DISABLED;
                }
            }
            else if (cause == (HCI_ERR | HCI_ERR_DIRECTED_ADV_TIMEOUT))
            {
                /*EXT_ADV_STATE_IDLE because timeout*/
                stop_cause = BLE_EXT_ADV_STOP_CAUSE_TIMEOUT;
                new_adv_state = BLE_EXT_ADV_MGR_ADV_DISABLED;
            }
            else if (cause == 0)
            {
                /*EXT_ADV_STATE_IDLE because conn*/
                stop_cause = BLE_EXT_ADV_STOP_CAUSE_CONN;
                new_adv_state = BLE_EXT_ADV_MGR_ADV_DISABLED;
            }
            else
            {
                stop_cause = BLE_EXT_ADV_STOP_CAUSE_UNKNOWN;
                if (p_adv->ble_ext_adv_mgr_adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
                {
                    stop_cause = BLE_EXT_ADV_STOP_CAUSE_ENABLE_FAILED;
                }
                new_adv_state = BLE_EXT_ADV_MGR_ADV_DISABLED;
            }
        }
    }
    else
    {
        if (p_adv->ext_adv_state == EXT_ADV_STATE_IDLE && cause == 0)
        {
            stop_cause = BLE_EXT_ADV_STOP_CAUSE_CONN;
            new_adv_state = BLE_EXT_ADV_MGR_ADV_DISABLED;
            if (p_adv->ble_ext_adv_mgr_adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
            {
                BTM_PRINT_ERROR1("ble_ext_adv_mgr_check_state: p_adv->pending_start_stop_num %d",
                                 p_adv->pending_start_stop_num);
                //ble_ext_adv_mgr_disable(p_adv->adv_handle, 0);
            }
        }
    }
    BTM_PRINT_INFO4("ble_ext_adv_mgr_check_state: adv_handle %d, old adv state %d, new adv state %d, T_BLE_EXT_ADV_STOP_CAUSE 0x%x",
                    p_adv->adv_handle, p_adv->ble_ext_adv_mgr_adv_state, new_adv_state, stop_cause);
    ble_ext_adv_update_adv_state(p_adv, stop_cause, new_adv_state, p_adv->app_cause);
}

/**
 * @brief ble_ext_adv_mgr_delete_pending_actions
 * delete pending actions for specified adv_handle
 * @param adv_handle specified adv_handle
 */
void ble_ext_adv_mgr_delete_pending_actions(uint8_t adv_handle)
{
    uint32_t loop = 0;
    uint32_t msg_num = 0;
    T_EXTEND_ADV_EVENT extend_adv_event;

    if (extend_adv_event_handle == NULL)
    {
        BTM_PRINT_ERROR0("ble_ext_adv_mgr_delete_pending_actions: extend_adv_event_handle NULL");
        return;
    }

    os_msg_queue_peek(extend_adv_event_handle, &msg_num);

    /*pop queue element*/
    while ((loop < msg_num) && (os_msg_recv(extend_adv_event_handle, &extend_adv_event, 0) == true))
    {
        BTM_PRINT_INFO2("ble_ext_adv_mgr_delete_pending_actions: adv handle in queue %d, expect deleted adv handle %d",
                        extend_adv_event.adv_handle, adv_handle);

        if (extend_adv_event.adv_handle != adv_handle)
        {
            if (os_msg_send(extend_adv_event_handle, &extend_adv_event, 0) != true)
            {
                BTM_PRINT_ERROR0("ble_ext_adv_mgr_delete_pending_actions: extend adv queue full");
            }
        }
        loop++;
    }
}

void ble_ext_adv_mgr_check_next_step(void)
{
    T_APP_EXT_ADV *p_adv;
    T_EXTEND_ADV_EVENT extend_adv_event;
    T_GAP_CAUSE ret = GAP_CAUSE_SUCCESS;

    if (extend_adv_event_handle == NULL)
    {
        BTM_PRINT_ERROR0("ble_ext_adv_mgr_check_next_step: extend_adv_event_handle NULL");
        return;
    }

    /*pop queue element*/
    while (os_msg_recv(extend_adv_event_handle, &extend_adv_event, 0) == true)
    {
        /*find p_adv by adv_handle in queue*/
        p_adv = ble_ext_adv_mgr_find_by_handle(extend_adv_event.adv_handle);

        if (p_adv == NULL)
        {
            ret = GAP_CAUSE_NOT_FIND;
            goto error;
        }
        else
        {
            /*handle expected_action*/
            switch (extend_adv_event.expected_action)
            {
            case BLE_EXT_ADV_ACTION_START:
                p_adv->pending_start_stop_num--;
                if (p_adv->ext_adv_state == EXT_ADV_STATE_IDLE)
                {
                    ret = ble_ext_adv_action_start(p_adv, extend_adv_event.param);
                }
                else
                {
                    ret = GAP_CAUSE_INVALID_STATE;
                    goto error;
                }
                break;

            case BLE_EXT_ADV_ACTION_STOP:
                p_adv->pending_start_stop_num--;
                if (p_adv->ext_adv_state == EXT_ADV_STATE_ADVERTISING)
                {
                    ret = ble_ext_adv_action_stop(p_adv, extend_adv_event.param);
                }
                else
                {
                    ret = GAP_CAUSE_INVALID_PARAM;
                    goto error;
                }
                break;

            case BLE_EXT_ADV_ACTION_UPDATE:
                {
                    ret = ble_ext_adv_action_update(p_adv, extend_adv_event.param);
                }
                break;
            }

            if (p_adv->pending_start_stop_num == 0 && ret != GAP_CAUSE_SUCCESS)
            {
                if (extend_adv_event.expected_action == BLE_EXT_ADV_ACTION_START ||
                    extend_adv_event.expected_action == BLE_EXT_ADV_ACTION_STOP)
                {
                    ble_ext_adv_mgr_check_state(p_adv, 0, false);
                }
            }
        }
error:
        BTM_PRINT_INFO3("ble_ext_adv_mgr_check_next_step: adv_handle %d, expected_action %d, ret %d",
                        extend_adv_event.adv_handle,
                        extend_adv_event.expected_action, ret);
        if (ret == GAP_CAUSE_SUCCESS)
        {
            break;
        }
    }
}

void ble_ext_adv_mgr_handle_adv_state(uint8_t adv_handle, T_GAP_EXT_ADV_STATE new_state,
                                      uint16_t cause)
{
    T_APP_EXT_ADV *p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);

    if (p_adv != NULL)
    {
        uint8_t old_adv_state = p_adv->ext_adv_state;
        p_adv->ext_adv_state = new_state;
        BTM_PRINT_INFO3("ble_ext_adv_mgr_handle_adv_state: adv_handle %d, adv_state %d -> %d",
                        adv_handle, old_adv_state, p_adv->ext_adv_state);
        ble_ext_adv_mgr_check_state(p_adv, cause, true);

        if (old_adv_state == EXT_ADV_STATE_START)
        {
            //enable procedure
            if (p_adv->pending_action == BLE_EXT_ADV_PENDING_ACTION_START)
            {
                p_adv->pending_action = BLE_EXT_ADV_PENDING_ACTION_IDLE;
            }

            ble_ext_adv_mgr_check_next_step();
        }
        else if (old_adv_state == EXT_ADV_STATE_STOP)
        {
            //disable procedure
            if (p_adv->pending_action == BLE_EXT_ADV_PENDING_ACTION_STOP)
            {
                p_adv->pending_action = BLE_EXT_ADV_PENDING_ACTION_IDLE;
            }

            ble_ext_adv_mgr_check_next_step();
        }
    }
}

void ble_ext_adv_mgr_handle_conn_state(uint8_t conn_id, T_GAP_CONN_STATE new_state,
                                       uint16_t disc_cause)
{
    uint8_t adv_handle = 0xFF;

    switch (new_state)
    {
    case GAP_CONN_STATE_CONNECTED:
        adv_handle = le_ext_adv_get_adv_handle_by_conn_id(conn_id);

        if (adv_handle != 0xFF)
        {
            T_APP_EXT_ADV *p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);

            if (p_adv != NULL)
            {
                /*BB2BUG-5998:
                After the connection is successful, directly delete the actions related to this adv_handle in the pending queue.
                If the APP still wants to modify this adv_handle's advertising parameters or enable advertising,
                the APP needs to perform related actions again after receiving BLE_EXT_ADV_SET_CONN_INFO*/
                ble_ext_adv_mgr_delete_pending_actions(adv_handle);

                uint8_t local_address[6] = {0};
                uint8_t local_address_type = 0xFF;
                uint16_t conn_handle = 0xFF;

                conn_handle = le_get_conn_handle(conn_id);
                bool ret = le_get_conn_local_addr(conn_handle, local_address, &local_address_type);

                BTM_PRINT_INFO6("ble_ext_adv_mgr_handle_conn_state: GAP_CONN_STATE_CONNECTED adv_handle %d, conn_id 0x%x,"
                                "conn_handle 0x%x, local_address %b, local_address_type 0x%x, ret %d",
                                adv_handle, conn_id, conn_handle, TRACE_BDADDR(local_address), local_address_type, ret);

                for (uint8_t i = 0; i < ble_ext_adv_handle_num_max; i++)
                {
                    T_APP_EXT_ADV *p_adv_temp = &ble_ext_adv_set_table[i];

                    if (p_adv_temp->is_used && (p_adv_temp->app_callback != NULL))
                    {
                        T_BLE_EXT_ADV_CB_DATA cb_data;
                        T_BLE_EXT_ADV_SET_CONN_INFO adv_conn_info;
                        adv_conn_info.adv_handle = p_adv->adv_handle;
                        adv_conn_info.conn_id = conn_id;
                        adv_conn_info.local_addr_type = local_address_type;
                        memcpy(adv_conn_info.local_addr, local_address, 6);

                        cb_data.p_ble_conn_info = &adv_conn_info;
                        p_adv_temp->app_callback(BLE_EXT_ADV_SET_CONN_INFO, &cb_data);
                    }
                }
            }
        }
        break;

    default:
        break;
    }
}

void ble_ext_adv_mgr_handle_gap_cb(uint8_t cb_type, T_LE_CB_DATA *p_data)
{
    if (cb_type == GAP_MSG_LE_EXT_ADV_START_SETTING)
    {
        T_APP_EXT_ADV *p_adv;
        BTM_PRINT_INFO3("GAP_MSG_LE_EXT_ADV_START_SETTING: cause 0x%x, flag 0x%x, adv_handle %d",
                        p_data->p_le_ext_adv_start_setting_rsp->cause, p_data->p_le_ext_adv_start_setting_rsp->flag,
                        p_data->p_le_ext_adv_start_setting_rsp->adv_handle);
        p_adv = ble_ext_adv_mgr_find_by_handle(
                    p_data->p_le_ext_adv_start_setting_rsp->adv_handle);

        if (p_adv != NULL)
        {
            if (p_adv->pending_action == BLE_EXT_ADV_PENDING_ACTION_UPDATE)
            {
                p_adv->pending_action = BLE_EXT_ADV_PENDING_ACTION_IDLE;
            }
        }
        if (p_data->p_le_ext_adv_start_setting_rsp->cause != GAP_CAUSE_SUCCESS)
        {
            BTM_PRINT_ERROR1("GAP_MSG_LE_EXT_ADV_START_SETTING: failed, cause 0x%x",
                             p_data->p_le_ext_adv_start_setting_rsp->cause);
        }
        ble_ext_adv_mgr_check_next_step();
    }
}

T_GAP_CAUSE ble_ext_adv_mgr_register_callback(P_FUN_GAP_APP_CB app_callback, uint8_t adv_handle)
{
    T_APP_EXT_ADV *p_adv;
    p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);

    if (p_adv == NULL)
    {
        return GAP_CAUSE_INVALID_PARAM;
    }

    p_adv->app_callback = app_callback;
    return GAP_CAUSE_SUCCESS;
}

T_BLE_EXT_ADV_MGR_STATE ble_ext_adv_mgr_get_adv_state(uint8_t adv_handle)
{
    T_APP_EXT_ADV *p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);

    if (p_adv != NULL)
    {
        return p_adv->ble_ext_adv_mgr_adv_state;
    }

    return BLE_EXT_ADV_MGR_ADV_DISABLED;
}

uint16_t ble_ext_adv_mgr_get_adv_interval(uint8_t adv_handle)
{
    T_APP_EXT_ADV *p_adv = ble_ext_adv_mgr_find_by_handle(adv_handle);

    if (p_adv != NULL)
    {
        return p_adv->adv_interval_max;
    }

    return 0;
}

void ble_ext_adv_print_info(void)
{
    uint32_t msg_num;

    if (extend_adv_event_handle == NULL)
    {
        BTM_PRINT_ERROR0("ble_ext_adv_mgr_check_next_step: extend_adv_event_handle NULL");
        return;
    }

    if (os_msg_queue_peek(extend_adv_event_handle, &msg_num))
    {
        BTM_PRINT_INFO1("ble_ext_adv_print_info: pending msg_num %d", msg_num);
    }

    for (int i = 0; i < ble_ext_adv_handle_num_max; i++)
    {
        if (ble_ext_adv_set_table[i].is_used)
        {
            BTM_PRINT_INFO4("ble_ext_adv_print_info: adv_handle %d, ble_ext_adv_mgr_adv_state %d, ext_adv_state %d, pending_action %d",
                            ble_ext_adv_set_table[i].adv_handle,
                            ble_ext_adv_set_table[i].ble_ext_adv_mgr_adv_state,
                            ble_ext_adv_set_table[i].ext_adv_state,
                            ble_ext_adv_set_table[i].pending_action);

        }
    }
}

bool ble_ext_adv_is_ongoing(void)
{
    for (int i = 0; i < ble_ext_adv_handle_num_max; i++)
    {
        if (BLE_EXT_ADV_MGR_ADV_ENABLED == ble_ext_adv_mgr_get_adv_state(i))
        {
            return true ;
        }
    }
    return false;
}

#if BLE_MGR_DEINIT
void ble_ext_adv_mgr_deinit(void)
{
    if (ble_ext_adv_set_table)
    {
        ble_ext_adv_handle_num_max = 0;
        free(ble_ext_adv_set_table);
        ble_ext_adv_set_table = NULL;
    }

    if (extend_adv_event_handle)
    {
        os_msg_queue_delete(extend_adv_event_handle);
        extend_adv_event_handle = NULL;
    }
}
#endif
