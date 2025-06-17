/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <string.h>
#include "stdlib.h"
#include "ble_conn.h"
#include "trace.h"
#include "gap_msg.h"
#include "gap_conn_le.h"
#include "ble_mgr_int.h"

typedef struct
{
    uint16_t  conn_interval_min;
    uint16_t  conn_interval_max;
    uint16_t  conn_latency;
    uint16_t  supervision_timeout;
} T_BLE_CONN_PARAM;

/**
 * @brief Ble conn update state
 * BLE_UPDATE_STATE_REQ_TO_IDLE:
 * this state use scenes is when app received GAP_CONN_PARAM_UPDATE_STATUS_SUCCESS and p_link->state is BLE_UPDATE_STATE_REQ,
 * we need set p_link->state = BLE_UPDATE_STATE_REQ_TO_IDLE. and set p_link->state = BLE_UPDATE_STATE_IDLE when app received GAP_CONN_PARAM_UPDATE_STATUS_PENDING
 */
typedef enum
{
    BLE_UPDATE_STATE_IDLE,
    BLE_UPDATE_STATE_REQ,
    BLE_UPDATE_STATE_PENDING,
    BLE_UPDATE_STATE_REQ_TO_IDLE,
} T_BLE_UPDATE_STATE;

typedef struct
{
    uint8_t conn_id;
    bool prefer_param_set;
    T_GAP_CONN_STATE conn_state;
    T_BLE_UPDATE_STATE state;
    T_BLE_CONN_PARAM curr_param;
    T_BLE_CONN_PARAM prefer_param;
} T_BLE_CONN_UPDATE_LINK;

static T_BLE_CONN_UPDATE_LINK *ble_update_table;
static uint8_t ble_conn_update_link_num;

bool ble_compare_conn_update_param(T_BLE_CONN_UPDATE_LINK *p_link)
{
    bool is_update = true;
    if (p_link->prefer_param_set == false)
    {
        return false;
    }
    if ((p_link->curr_param.conn_latency == p_link->prefer_param.conn_latency) &&
        (p_link->curr_param.supervision_timeout == p_link->prefer_param.supervision_timeout))
    {
        uint16_t  conn_interval_min = p_link->prefer_param.conn_interval_min;
        uint16_t  conn_interval_max = p_link->prefer_param.conn_interval_max;
        if (conn_interval_max < 0x18)
        {
            conn_interval_max = 0x18;//modify for ios device
        }
        if ((p_link->curr_param.conn_interval_min >=  conn_interval_min) &&
            (p_link->curr_param.conn_interval_min <=  conn_interval_max))
        {
            is_update = false;
        }
    }
    return is_update;
}

bool ble_check_conn_update_param(T_BLE_CONN_UPDATE_LINK *p_link)
{
    bool ret = false;
    bool is_update = false;
    T_GAP_CAUSE cause = GAP_CAUSE_ERROR_UNKNOWN;

    if (p_link->state == BLE_UPDATE_STATE_IDLE)
    {
        is_update = ble_compare_conn_update_param(p_link);
        if (is_update)
        {
            uint16_t ce_length_min = 2 * (p_link->prefer_param.conn_interval_min - 1);
            uint16_t ce_length_max = 2 * (p_link->prefer_param.conn_interval_max - 1);
            cause = le_update_conn_param(p_link->conn_id,  p_link->prefer_param.conn_interval_min,
                                         p_link->prefer_param.conn_interval_max, p_link->prefer_param.conn_latency,
                                         p_link->prefer_param.supervision_timeout, ce_length_min, ce_length_max);
            if (cause == GAP_CAUSE_SUCCESS)
            {
                ret = true;
                p_link->state = BLE_UPDATE_STATE_REQ;
            }
        }
    }
    BTM_PRINT_INFO4("ble_check_conn_update_param: conn_id %d, is_update %d, p_link->state %d, cause 0x%02x",
                    p_link->conn_id, is_update, p_link->state, cause);
    return ret;
}

void ble_handle_conn_update_info(T_GAP_CONN_PARAM_UPDATE update_info)
{
    T_BLE_CONN_UPDATE_LINK *p_link;
    if (update_info.conn_id >= ble_conn_update_link_num)
    {
        BTM_PRINT_ERROR1("ble_handle_conn_update_info: invalid conn_id %d", update_info.conn_id);
        return;
    }
    p_link = &ble_update_table[update_info.conn_id];
    T_BLE_UPDATE_STATE old_link_state = p_link->state;
    switch (update_info.status)
    {
    case GAP_CONN_PARAM_UPDATE_STATUS_SUCCESS:
        {
            uint16_t conn_interval;
            uint16_t conn_slave_latency;
            uint16_t conn_supervision_timeout;

            le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval, update_info.conn_id);
            le_get_conn_param(GAP_PARAM_CONN_LATENCY, &conn_slave_latency, update_info.conn_id);
            le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &conn_supervision_timeout, update_info.conn_id);

            p_link->curr_param.conn_interval_max = conn_interval;
            p_link->curr_param.conn_interval_min = conn_interval;
            p_link->curr_param.conn_latency = conn_slave_latency;
            p_link->curr_param.supervision_timeout = conn_supervision_timeout;

            BTM_PRINT_INFO4("ble_handle_conn_update_info update success: conn_id %d, conn_interval 0x%04x, conn_slave_latency 0x%04x, conn_supervision_timeout 0x%04x",
                            update_info.conn_id, conn_interval, conn_slave_latency, conn_supervision_timeout);
            if (old_link_state == BLE_UPDATE_STATE_PENDING)
            {
                p_link->state = BLE_UPDATE_STATE_IDLE;
            }
            else if (old_link_state == BLE_UPDATE_STATE_REQ)
            {
                p_link->state = BLE_UPDATE_STATE_REQ_TO_IDLE;
            }
            ble_check_conn_update_param(p_link);
        }
        break;

    case GAP_CONN_PARAM_UPDATE_STATUS_FAIL:
        {
            if (old_link_state == BLE_UPDATE_STATE_PENDING)
            {
                p_link->state = BLE_UPDATE_STATE_IDLE;
            }
            else if (old_link_state == BLE_UPDATE_STATE_REQ)
            {
                if (update_info.cause == (L2C_ERR | L2C_ERR_LE_CONN_PARAM_UPDATE_REJECT))
                {
                    /*remote reject*/
                    p_link->prefer_param_set = false;
                }
                p_link->state = BLE_UPDATE_STATE_IDLE;
            }
        }
        break;

    case GAP_CONN_PARAM_UPDATE_STATUS_PENDING:
        {
            if (old_link_state == BLE_UPDATE_STATE_REQ)
            {
                p_link->state = BLE_UPDATE_STATE_PENDING;
            }
            else if (old_link_state == BLE_UPDATE_STATE_REQ_TO_IDLE)
            {
                p_link->state = BLE_UPDATE_STATE_IDLE;
            }
            ble_check_conn_update_param(p_link);
        }
        break;
    default:
        break;
    }
    BTM_PRINT_TRACE5("ble_handle_conn_update_info: conn_id %d, update status %d, cause 0x%04x, p_link->state %d -> %d",
                     update_info.conn_id, update_info.status, update_info.cause, old_link_state, p_link->state);
}

void ble_handle_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state)
{
    T_BLE_CONN_UPDATE_LINK *p_link;
    if (conn_id >= ble_conn_update_link_num)
    {
        BTM_PRINT_ERROR1("ble_handle_conn_state_evt: invalid conn_id %d", conn_id);
        return;
    }
    p_link = &ble_update_table[conn_id];
    switch (new_state)
    {
    case GAP_CONN_STATE_DISCONNECTED:
        {
            memset(p_link, 0, sizeof(T_BLE_CONN_UPDATE_LINK));
        }
        break;

    case GAP_CONN_STATE_CONNECTED:
        {
            uint16_t conn_interval;
            uint16_t conn_slave_latency;
            uint16_t conn_supervision_timeout;

            le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_LATENCY, &conn_slave_latency, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &conn_supervision_timeout, conn_id);

            p_link->curr_param.conn_interval_max = conn_interval;
            p_link->curr_param.conn_interval_min = conn_interval;
            p_link->curr_param.conn_latency = conn_slave_latency;
            p_link->curr_param.supervision_timeout = conn_supervision_timeout;
        }
        break;

    default:
        break;

    }
    p_link->conn_state = new_state;
    p_link->conn_id = conn_id;
}

bool ble_set_prefer_conn_param(uint8_t conn_id, uint16_t conn_interval_min,
                               uint16_t conn_interval_max, uint16_t conn_latency,
                               uint16_t supervision_timeout)
{
    uint8_t ret = 0;
    T_BLE_CONN_UPDATE_LINK *p_link;

    if (conn_id >= ble_conn_update_link_num)
    {
        ret = 1;
        goto error;
    }
    p_link = &ble_update_table[conn_id];
    if (p_link->conn_state == GAP_CONN_STATE_CONNECTED)
    {
        p_link->prefer_param_set = true;
        p_link->prefer_param.conn_interval_min = conn_interval_min;
        p_link->prefer_param.conn_interval_max = conn_interval_max;
        p_link->prefer_param.conn_latency = conn_latency;
        p_link->prefer_param.supervision_timeout = supervision_timeout;

        if (ble_check_conn_update_param(p_link) == false)
        {
            /*1.current connection paramter has already meet the conditions, do not need to update. or
              2.current link state is busy, may be update after link state change into idle*/
            ret = 2;
            goto error;
        }
    }
    else
    {
        ret = 3;
        goto error;
    }

error:
    BTM_PRINT_TRACE6("ble_set_prefer_conn_param: ret %d, conn_id %d, conn_interval_min 0x%04x, conn_interval_max 0x%04x, conn_latency 0x%04x, supervision_timeout 0x%04x",
                     ret, conn_id, conn_interval_min, conn_interval_max,
                     conn_latency, supervision_timeout);

    return (((ret == 0) || (ret == 2)) ? true : false);
}


bool ble_conn_update_init(uint8_t link_num)
{
    uint16_t size;
    bool ret = true;
    ble_conn_update_link_num = link_num;
    size = ble_conn_update_link_num * sizeof(T_BLE_CONN_UPDATE_LINK);
    ble_update_table = calloc(1, size);
    if ((link_num != 0) && (ble_update_table == NULL))
    {
        /*allocate ram failed*/
        ret = false;
    }
#if BLE_MGR_INIT_DEBUG
    BTM_PRINT_TRACE2("ble_conn_update_init: link_num %d, ret %d", link_num, ret);
#endif
    return ret;
}

#if BLE_MGR_DEINIT
void ble_conn_update_deinit(void)
{
    if (ble_update_table)
    {
        ble_conn_update_link_num = 0;
        free(ble_update_table);
        ble_update_table = NULL;
    }
}
#endif
