/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <string.h>
#include "trace.h"
#include "os_queue.h"
#include "stdlib.h"
#include "ble_ext_adv.h"
#include "ble_adv_data.h"
#include "os_timer.h"
#include "ble_mgr_int.h"

typedef struct t_adv_data_item
{
    struct t_adv_data_item *p_next;
    uint16_t adv_data_len;
    uint16_t scan_resp_len;
    uint8_t *p_adv_data;
    uint8_t *p_scan_resp;
} T_ADV_DATA_ITEM;

typedef struct
{
    bool update_scan_data;
    T_BLE_EXT_ADV_MGR_STATE perfer_state;
    T_BLE_EXT_ADV_MGR_STATE curr_state;
    T_OS_QUEUE  adv_data_queue;
    uint8_t     curr_idx;
} T_ADV_DATA_MGR;

static T_ADV_DATA_MGR adv_data_mgr;
static void *p_data_timer_handle;
static uint8_t ble_adv_handle = 0xFF;

void adv_data_timer_callback(void *p_handle)
{
    T_ADV_DATA_ITEM *p_item;

    BTM_PRINT_TRACE1("adv_data_timer_callback: curr_idx %d", adv_data_mgr.curr_idx);
    adv_data_mgr.curr_idx++;
    if (adv_data_mgr.curr_idx >= adv_data_mgr.adv_data_queue.count)
    {
        adv_data_mgr.curr_idx = 0;
    }
    p_item = os_queue_peek(&adv_data_mgr.adv_data_queue, adv_data_mgr.curr_idx);
    if (p_item != NULL)
    {
        if (adv_data_mgr.update_scan_data)
        {
            ble_ext_adv_mgr_set_scan_response_data(ble_adv_handle,
                                                   p_item->scan_resp_len, p_item->p_scan_resp);
        }
        ble_ext_adv_mgr_set_adv_data(ble_adv_handle,
                                     p_item->adv_data_len, p_item->p_adv_data);
    }
}

bool adv_data_timer_start(void)
{
    if (p_data_timer_handle != NULL)
    {
        return os_timer_start(&p_data_timer_handle);
    }
    return false;
}

bool adv_data_timer_stop(void)
{
    if (p_data_timer_handle != NULL)
    {
        return os_timer_stop(&p_data_timer_handle);
    }
    return false;
}

void adv_data_timer_create(void)
{
    uint8_t result = 0;
    uint32_t adv_interval;

    adv_interval = ble_ext_adv_mgr_get_adv_interval(ble_adv_handle);
    if (adv_interval == 0)
    {
        result = 1;
        goto error;
    }

    adv_interval = (adv_interval * 5) / 8;

    if (p_data_timer_handle == NULL)
    {
        os_timer_create(&p_data_timer_handle, "adv_data_timer", 0,
                        adv_interval, true, adv_data_timer_callback);
        if (p_data_timer_handle == NULL)
        {
            /*adv_data_timer_create: create timer failed*/
            result = 2;
            goto error;
        }
    }
    else
    {
        /*adv_data_timer_create: timer already exist*/
        result = 3;
        goto error;
    }
error:
    BTM_PRINT_INFO2("adv_data_timer_create: adv_interval %d, result %d", adv_interval, result);
}

void ble_adv_data_check_timer(void)
{
    bool timer_open = false;
    if (adv_data_mgr.adv_data_queue.count == 0)
    {
        if (adv_data_mgr.curr_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
        {
            ble_ext_adv_mgr_disable(ble_adv_handle, 0);
        }
    }
    else if (adv_data_mgr.adv_data_queue.count == 1)
    {
        T_ADV_DATA_ITEM *p_item = (T_ADV_DATA_ITEM *)adv_data_mgr.adv_data_queue.p_first;
        if (adv_data_mgr.update_scan_data)
        {
            ble_ext_adv_mgr_set_scan_response_data(ble_adv_handle,
                                                   p_item->scan_resp_len, p_item->p_scan_resp);
        }
        ble_ext_adv_mgr_set_adv_data(ble_adv_handle,
                                     p_item->adv_data_len, p_item->p_adv_data);
        if (adv_data_mgr.perfer_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
        {
            if (adv_data_mgr.curr_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
            {
                ble_ext_adv_mgr_enable(ble_adv_handle, 0);
            }
        }
    }
    else
    {
        if (adv_data_mgr.perfer_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
        {
            if (adv_data_mgr.curr_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
            {
                ble_ext_adv_mgr_enable(ble_adv_handle, 0);
            }
            timer_open = true;
        }
    }

    if (timer_open)
    {
        adv_data_timer_start();
    }
    else
    {
        adv_data_timer_stop();
    }
}

void ble_adv_data_check_state(void)
{
    BTM_PRINT_TRACE3("ble_adv_data_check_state: perfer_state %d, curr_state %d, count %d",
                     adv_data_mgr.perfer_state,
                     adv_data_mgr.curr_state,
                     adv_data_mgr.adv_data_queue.count);
    if (adv_data_mgr.perfer_state != adv_data_mgr.curr_state)
    {
        if (adv_data_mgr.perfer_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
        {
            if (adv_data_mgr.adv_data_queue.count != 0)
            {
                ble_ext_adv_mgr_enable(ble_adv_handle, 0);
                if (adv_data_mgr.adv_data_queue.count != 1)
                {
                    adv_data_timer_start();
                }
            }
        }
        else
        {
            ble_ext_adv_mgr_disable(ble_adv_handle, 0);
            adv_data_timer_stop();
        }
    }
}

bool ble_adv_data_add(void **pp_handle, uint16_t adv_data_len, uint8_t *p_adv_data,
                      uint16_t scan_resp_len, uint8_t *p_scan_resp)
{

    T_ADV_DATA_ITEM *p_loop;
    T_ADV_DATA_ITEM *p_item = NULL;
    uint8_t result = 0;

    if (ble_adv_handle == 0xff)
    {
        result = 3;
        goto error;
    }

    if (pp_handle == NULL || p_adv_data == NULL)
    {
        result = 1;
        goto error;
    }

    *pp_handle = NULL;
    p_loop = (T_ADV_DATA_ITEM *)adv_data_mgr.adv_data_queue.p_first;

    while (p_loop != NULL)
    {
        if (p_loop->adv_data_len == adv_data_len
            && p_loop->p_adv_data == p_adv_data)
        {
            /*ble_adv_data_add: this adv data already existed in queue*/
            *pp_handle = p_loop;
            goto error;
        }
        p_loop = p_loop->p_next;
    }

    p_item = calloc(1, sizeof(T_ADV_DATA_ITEM));
    if (p_item != NULL)
    {
        p_item->adv_data_len = adv_data_len;
        p_item->p_adv_data = p_adv_data;
        p_item->scan_resp_len = scan_resp_len;
        p_item->p_scan_resp = p_scan_resp;
        /*ble_adv_data_add new adv data add into queue*/
        os_queue_in(&adv_data_mgr.adv_data_queue, p_item);
        ble_adv_data_check_timer();
        *pp_handle = (void *)p_item;
    }
    else
    {
        result = 2;
        goto error;
    }

error:
    BTM_PRINT_TRACE2("ble_adv_data_add: p_handle %p, result %d", p_item, result);
    return ((result == 0) ? true : false);
}

bool ble_adv_data_del(void *p_handle)
{
    if (p_handle != NULL)
    {
        BTM_PRINT_TRACE1("ble_adv_data_del: p_handle %p", ble_adv_data_del);
        if (os_queue_delete(&adv_data_mgr.adv_data_queue, p_handle))
        {
            free(p_handle);
            ble_adv_data_check_timer();
            return true;
        }
        return false;
    }
    return false;
}

bool ble_adv_data_enable(void)
{
    if (ble_adv_handle == 0xff)
    {
        return false;
    }
    BTM_PRINT_TRACE0("ble_adv_data_enable");
    adv_data_mgr.perfer_state = BLE_EXT_ADV_MGR_ADV_ENABLED;
    ble_adv_data_check_state();
    return true;
}

bool ble_adv_data_disable(void)
{
    if (ble_adv_handle == 0xff)
    {
        return false;
    }
    BTM_PRINT_TRACE0("ble_adv_data_disable");
    adv_data_mgr.perfer_state = BLE_EXT_ADV_MGR_ADV_DISABLED;
    ble_adv_data_check_state();
    return true;
}

static void ble_adv_data_callback(uint8_t cb_type, void *p_cb_data)
{
    T_BLE_EXT_ADV_CB_DATA cb_data;
    memcpy(&cb_data, p_cb_data, sizeof(T_BLE_EXT_ADV_CB_DATA));
    switch (cb_type)
    {
    case BLE_EXT_ADV_STATE_CHANGE:
        {
            BTM_PRINT_TRACE4("ble_adv_data_callback: BLE_EXT_ADV_STATE_CHANGE, adv_handle %d, state %d, stop_cause %d, app_cause %d",
                             cb_data.p_ble_state_change->adv_handle,
                             cb_data.p_ble_state_change->state,
                             cb_data.p_ble_state_change->stop_cause,
                             cb_data.p_ble_state_change->app_cause);
            adv_data_mgr.curr_state = cb_data.p_ble_state_change->state;
        }
        break;

    case BLE_EXT_ADV_SET_CONN_INFO:
        BTM_PRINT_TRACE1("ble_adv_data_callback: BLE_EXT_ADV_SET_CONN_INFO conn_id 0x%x",
                         cb_data.p_ble_conn_info->conn_id);
        ble_adv_data_check_state();
        break;
    default:
        break;
    }
    return;
}

bool ble_adv_data_init(bool update_scan_data, T_GAP_LOCAL_ADDR_TYPE own_address_type,
                       T_GAP_REMOTE_ADDR_TYPE peer_address_type,
                       uint32_t adv_interval, uint32_t scan_rsp_len, uint8_t *scan_rsp_data)
{
    T_LE_EXT_ADV_LEGACY_ADV_PROPERTY adv_event_prop = LE_EXT_ADV_LEGACY_ADV_CONN_SCAN_UNDIRECTED;
    uint8_t  peer_address[6] = {0, 0, 0, 0, 0, 0};
    T_GAP_ADV_FILTER_POLICY filter_policy = GAP_ADV_FILTER_ANY;

    ble_ext_adv_mgr_init_adv_params(&ble_adv_handle, adv_event_prop, adv_interval,
                                    adv_interval, own_address_type, peer_address_type, peer_address,
                                    filter_policy, 0, NULL,
                                    scan_rsp_len, scan_rsp_data, NULL);
#if BLE_MGR_INIT_DEBUG
    BTM_PRINT_TRACE1("ble_adv_data_init: adv_handle %d", ble_adv_handle);
#endif
    os_queue_init(&adv_data_mgr.adv_data_queue);
    adv_data_mgr.update_scan_data = update_scan_data;

    if (ble_ext_adv_mgr_register_callback(ble_adv_data_callback, ble_adv_handle) == GAP_CAUSE_SUCCESS)
    {
        adv_data_timer_create();
        return true;
    }

    ble_adv_handle = 0xff;
    return false;
}

void ble_adv_data_handle_conn_state(uint8_t conn_id, T_GAP_CONN_STATE new_state,
                                    uint16_t disc_cause)
{
    if (new_state == GAP_CONN_STATE_DISCONNECTED)
    {
        ble_adv_data_check_state();
    }
}

#if BLE_MGR_DEINIT
void ble_adv_data_deinit(void)
{
    T_ADV_DATA_ITEM *p_item;
    while ((p_item = os_queue_out(&adv_data_mgr.adv_data_queue)) != NULL)
    {
        free(p_item);
    }

    if (p_data_timer_handle != NULL)
    {
        os_timer_delete(&p_data_timer_handle);
        p_data_timer_handle = NULL;
    }
    ble_adv_handle = 0xff;
    memset(&adv_data_mgr, 0, sizeof(T_ADV_DATA_MGR));
}
#endif
