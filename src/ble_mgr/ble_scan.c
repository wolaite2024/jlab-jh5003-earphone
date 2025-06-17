#include "ble_scan.h"
#include "gap_msg.h"
#include "trace.h"
#include "os_queue.h"
#include <stdlib.h>
#include <string.h>
#include "ble_mgr_int.h"

typedef struct t_ble_scan
{
    struct t_ble_scan     *p_next;
    BLE_SCAN_CB            cb;
    BLE_SCAN_PARAM         param;
    BLE_SCAN_FILTER        filter;
} T_BLE_SCAN;

typedef enum
{
    BLE_SCAN_ACTION_STOPED       = 0,
    BLE_SCAN_ACTION_STARTED      = 1,
    BLE_SCAN_ACTION_PAUSE        = 2,
} BLE_SCAN_ACTION;

typedef enum
{
    BLE_SCAN_PARAM_UPDATE_IDLE   = 0,
    BLE_SCAN_PARAM_UPDATE_REQ    = 1,
    BLE_SCAN_PARAM_UPDATE_STOP   = 2,
    BLE_SCAN_PARAM_UPDATE_START  = 3,
} BLE_SCAN_PARAM_UPDATE;

typedef struct
{
    BLE_SCAN_PARAM_UPDATE scan_update;
    BLE_SCAN_ACTION       scan_action;
    uint8_t               scan_state;
    T_OS_QUEUE            queue;
    BLE_SCAN_PARAM        param;
} T_BLE_SCAN_MGR;

T_BLE_SCAN_MGR *p_scan_mgr = NULL;

#define ble_scan_hdl_check(hdl) ble_scan_hdl_check_int(__func__, hdl)

static bool ble_scan_hdl_check_int(const char *p_func_name, BLE_SCAN_HDL hdl)
{
    if (p_scan_mgr != NULL && hdl != NULL)
    {
        if (os_queue_search(&p_scan_mgr->queue, hdl) == true)
        {
            return true;
        }
    }
    BTM_PRINT_ERROR2("ble_scan_hdl_check_int:failed, %s, hdl %p",
                     TRACE_STRING(p_func_name), hdl);
    return false;
}

static void go_through_cbs(BLE_SCAN_EVT evt, BLE_SCAN_EVT_DATA *data)
{
    T_BLE_SCAN *p_scan = (T_BLE_SCAN *)p_scan_mgr->queue.p_first;
    for (; p_scan != NULL; p_scan = p_scan->p_next)
    {
        p_scan->cb(evt, data);
    }
}

static bool match_filter(BLE_SCAN_FILTER *p_filter, T_LE_EXT_ADV_REPORT_INFO *p_report)
{
    if (p_filter->filter_flags == BLE_SCAN_FILTER_NONE)
    {
        return true;
    }
    if (p_filter->filter_flags & BLE_SCAN_FILTER_EVT_TYPE_BIT)
    {
        if (p_filter->evt_type != p_report->event_type)
        {
            return false;
        }

    }
    if (p_filter->filter_flags & BLE_SCAN_FILTER_REMOTE_ADDR_BIT)
    {
        bool addr_found = false;
        for (uint8_t i = 0; i < p_filter->addr_num; i++)
        {
            if (p_report->addr_type == p_filter->p_scan_addr[i].bd_type &&
                memcmp(p_report->bd_addr, p_filter->p_scan_addr[i].bd_addr, 6) == 0)
            {
                addr_found = true;
                break;
            }
        }
        if (addr_found == false)
        {
            return false;
        }
    }
    if (p_filter->filter_flags & BLE_SCAN_FILTER_ADV_DATA_BIT)
    {
        bool adv_data_found = false;
        uint8_t *adv_data = p_report->p_data;;
        uint32_t tmp_ad_len = 0;
        for (uint32_t pos = 0; pos < p_report->data_len; pos += tmp_ad_len)
        {
            tmp_ad_len = adv_data[pos] + 1;
            if (memcmp(&p_filter->ad_struct[0], &adv_data[pos + 1], p_filter->ad_len) == 0)
            {
                adv_data_found = true;
                break;
            }
        }
        if (adv_data_found == false)
        {
            return false;
        }
    }
    return true;
}

uint8_t ble_scan_get_cur_state(void)
{
    T_GAP_DEV_STATE dev_state;
    if (le_get_gap_param(GAP_PARAM_DEV_STATE, &dev_state) == GAP_CAUSE_SUCCESS)
    {
        return dev_state.gap_scan_state;
    }
    return GAP_SCAN_STATE_IDLE;
}

static bool scan_start(void)
{
    BLE_SCAN_PARAM *p_param = &p_scan_mgr->param;

    /* Initialize extended scan parameters */
    le_ext_scan_set_param(GAP_PARAM_EXT_SCAN_LOCAL_ADDR_TYPE,
                          sizeof(p_param->own_addr_type), &p_param->own_addr_type);

    le_ext_scan_set_param(GAP_PARAM_EXT_SCAN_PHYS,
                          sizeof(p_param->phys), &p_param->phys);

    uint16_t ext_duration = 0;
    le_ext_scan_set_param(GAP_PARAM_EXT_SCAN_DURATION,
                          sizeof(ext_duration), &ext_duration);

    uint16_t ext_period = 0;
    le_ext_scan_set_param(GAP_PARAM_EXT_SCAN_PERIOD,
                          sizeof(ext_period), &ext_period);

    le_ext_scan_set_param(GAP_PARAM_EXT_SCAN_FILTER_POLICY,
                          sizeof(p_param->ext_filter_policy), &p_param->ext_filter_policy);

    le_ext_scan_set_param(GAP_PARAM_EXT_SCAN_FILTER_DUPLICATES,
                          sizeof(p_param->ext_filter_duplicate), &p_param->ext_filter_duplicate);
    if (p_param->phys & GAP_EXT_SCAN_PHYS_1M_BIT)
    {
        le_ext_scan_set_phy_param(LE_SCAN_PHY_LE_1M, &p_param->scan_param_1m);
    }
    if (p_param->phys & GAP_EXT_SCAN_PHYS_CODED_BIT)
    {
        le_ext_scan_set_phy_param(LE_SCAN_PHY_LE_CODED, &p_param->scan_param_coded);
    }

    if (le_ext_scan_start() != GAP_CAUSE_SUCCESS)
    {
        return false;
    }
    return true;
}

void ble_scan_check_state(void)
{
    uint8_t cur_scan_state = ble_scan_get_cur_state();
    BTM_PRINT_TRACE3("ble_scan_check_state: cur_scan_state %d ,scan_action %d, scan_update %d",
                     cur_scan_state,
                     p_scan_mgr->scan_action,
                     p_scan_mgr->scan_update);
    if (cur_scan_state == GAP_SCAN_STATE_SCANNING)
    {
        if (p_scan_mgr->scan_action == BLE_SCAN_ACTION_PAUSE ||
            p_scan_mgr->scan_action == BLE_SCAN_ACTION_STOPED)
        {
            le_ext_scan_stop();
        }
        else if (p_scan_mgr->scan_action == BLE_SCAN_ACTION_STARTED)
        {
            if (p_scan_mgr->scan_update == BLE_SCAN_PARAM_UPDATE_REQ)
            {
                if (le_ext_scan_stop() == GAP_CAUSE_SUCCESS)
                {
                    p_scan_mgr->scan_update = BLE_SCAN_PARAM_UPDATE_STOP;
                }
            }
            else if (p_scan_mgr->scan_update == BLE_SCAN_PARAM_UPDATE_START)
            {
                p_scan_mgr->scan_update = BLE_SCAN_PARAM_UPDATE_IDLE;
                go_through_cbs(BLE_SCAN_PARAM_CHANGES, NULL);
            }
        }
    }
    else if (cur_scan_state == GAP_SCAN_STATE_IDLE)
    {
        if (p_scan_mgr->scan_action == BLE_SCAN_ACTION_STARTED)
        {
            if (scan_start())
            {
                if (p_scan_mgr->scan_update == BLE_SCAN_PARAM_UPDATE_STOP)
                {
                    p_scan_mgr->scan_update = BLE_SCAN_PARAM_UPDATE_START;
                }
            }
        }
    }
}

bool ble_scan_param_update(void)
{
    BLE_SCAN_PARAM scan_param;
    memset(&scan_param, 0, sizeof(BLE_SCAN_PARAM));
    for (uint8_t i = 0; i < p_scan_mgr->queue.count; i++)
    {
        T_BLE_SCAN *p_scan = (T_BLE_SCAN *)os_queue_peek(&p_scan_mgr->queue, i);
        if (p_scan)
        {
            if (i == 0)
            {
                memcpy(&scan_param, &p_scan->param, sizeof(BLE_SCAN_PARAM));
            }
            else
            {
                if (scan_param.own_addr_type != p_scan->param.own_addr_type)
                {
                    scan_param.own_addr_type = GAP_LOCAL_ADDR_LE_PUBLIC;
                }
                if (p_scan->param.ext_filter_duplicate == GAP_SCAN_FILTER_DUPLICATE_DISABLE)
                {
                    scan_param.ext_filter_duplicate = GAP_SCAN_FILTER_DUPLICATE_DISABLE;
                }
                if (p_scan->param.ext_filter_policy == GAP_SCAN_FILTER_ANY)
                {
                    scan_param.ext_filter_policy = GAP_SCAN_FILTER_ANY;
                }
                if (p_scan->param.phys & GAP_EXT_SCAN_PHYS_1M_BIT)
                {
                    if ((scan_param.phys & GAP_EXT_SCAN_PHYS_1M_BIT) == 0)
                    {
                        memcpy(&scan_param.scan_param_1m, &p_scan->param.scan_param_1m, sizeof(T_GAP_LE_EXT_SCAN_PARAM));
                    }
                    else
                    {
                        if (p_scan->param.scan_param_1m.scan_type == GAP_SCAN_MODE_ACTIVE)
                        {
                            scan_param.scan_param_1m.scan_type = GAP_SCAN_MODE_ACTIVE;
                        }
                        if (p_scan->param.scan_param_1m.scan_interval < scan_param.scan_param_1m.scan_interval)
                        {
                            scan_param.scan_param_1m.scan_interval = p_scan->param.scan_param_1m.scan_interval;
                            scan_param.scan_param_1m.scan_window = p_scan->param.scan_param_1m.scan_window;
                        }
                    }
                }
                if (p_scan->param.phys & GAP_EXT_SCAN_PHYS_CODED_BIT)
                {
                    if ((scan_param.phys & GAP_EXT_SCAN_PHYS_CODED_BIT) == 0)
                    {
                        memcpy(&scan_param.scan_param_coded, &p_scan->param.scan_param_coded,
                               sizeof(T_GAP_LE_EXT_SCAN_PARAM));
                    }
                    else
                    {
                        if (p_scan->param.scan_param_coded.scan_type == GAP_SCAN_MODE_ACTIVE)
                        {
                            scan_param.scan_param_coded.scan_type = GAP_SCAN_MODE_ACTIVE;
                        }
                        if (p_scan->param.scan_param_coded.scan_interval < scan_param.scan_param_coded.scan_interval)
                        {
                            scan_param.scan_param_coded.scan_interval = p_scan->param.scan_param_coded.scan_interval;
                            scan_param.scan_param_coded.scan_window = p_scan->param.scan_param_coded.scan_window;
                        }
                    }
                }
                scan_param.phys |= p_scan->param.phys;
            }
        }
    }
    if (memcmp(&p_scan_mgr->param, &scan_param, sizeof(BLE_SCAN_PARAM)) == 0)
    {
        return false;
    }
    else
    {
        memcpy(&p_scan_mgr->param, &scan_param, sizeof(BLE_SCAN_PARAM));
        if (p_scan_mgr->scan_action == BLE_SCAN_ACTION_STARTED)
        {
            uint8_t cur_scan_state = ble_scan_get_cur_state();
            if (cur_scan_state == GAP_SCAN_STATE_SCANNING ||
                cur_scan_state == GAP_SCAN_STATE_START)
            {
                p_scan_mgr->scan_update = BLE_SCAN_PARAM_UPDATE_REQ;
            }
        }
        return true;
    }
}

bool ble_scan_set_param_by_hdl(BLE_SCAN_HDL hdl, BLE_SCAN_PARAM *p_scan_param)
{
    if (ble_scan_hdl_check(hdl) == false || p_scan_param == NULL)
    {
        return false;
    }
    T_BLE_SCAN *p_ble_scan = (T_BLE_SCAN *)hdl;
    memcpy(&p_ble_scan->param, p_scan_param, sizeof(BLE_SCAN_PARAM));
    ble_scan_param_update();
    ble_scan_check_state();
    return true;
}

bool ble_scan_get_param_by_hdl(BLE_SCAN_HDL hdl, BLE_SCAN_PARAM *p_scan_param)
{
    if (ble_scan_hdl_check(hdl) == false)
    {
        return false;
    }
    if (p_scan_param)
    {
        T_BLE_SCAN *p_ble_scan = (T_BLE_SCAN *)hdl;
        memcpy(p_scan_param, &p_ble_scan->param, sizeof(BLE_SCAN_PARAM));
        return true;
    }
    return false;
}

bool ble_scan_get_cur_param(BLE_SCAN_PARAM *p_scan_param)
{
    if (p_scan_mgr != NULL && p_scan_param != NULL)
    {
        memcpy(p_scan_param, &p_scan_mgr->param, sizeof(BLE_SCAN_PARAM));
        return true;
    }
    return false;
}

bool ble_scan_start(BLE_SCAN_HDL *p_hdl, BLE_SCAN_CB cb,
                    BLE_SCAN_PARAM *p_scan_param, BLE_SCAN_FILTER *p_filter)
{
    T_BLE_SCAN *p_scan = NULL;
    bool ad_struct_alloced = false;
    if (p_hdl == NULL || p_scan_mgr == NULL)
    {
        goto ERR_HANDLE;
    }

    if (*p_hdl != NULL)
    {
        return ble_scan_hdl_check(*p_hdl);
    }

    p_scan = calloc(1, sizeof(*p_scan));
    if (p_scan == NULL)
    {
        goto ERR_HANDLE;
    }

    if (p_scan_param)
    {
        memcpy(&p_scan->param, p_scan_param, sizeof(BLE_SCAN_PARAM));
    }

    if (p_filter)
    {
        memcpy(&p_scan->filter, p_filter,  sizeof(p_scan->filter));
        if ((p_filter->filter_flags & BLE_SCAN_FILTER_ADV_DATA_BIT) &&
            p_filter->ad_struct != NULL && p_filter->ad_len != 0)
        {
            p_scan->filter.ad_struct = calloc(1, p_filter->ad_len);

            if (p_scan->filter.ad_struct == NULL)
            {
                goto ERR_HANDLE;
            }
            ad_struct_alloced = true;
            memcpy(p_scan->filter.ad_struct, p_filter->ad_struct, p_filter->ad_len);
        }
        else
        {
            p_filter->filter_flags &= ~BLE_SCAN_FILTER_ADV_DATA_BIT;
            p_scan->filter.ad_struct = NULL;
            p_filter->ad_len = 0;
        }

        if ((p_filter->filter_flags & BLE_SCAN_FILTER_REMOTE_ADDR_BIT) &&
            p_filter->p_scan_addr != NULL && p_filter->addr_num != 0)
        {
            p_scan->filter.p_scan_addr = calloc(1, p_filter->addr_num * sizeof(BLE_SCAN_ADDR));

            if (p_scan->filter.p_scan_addr == NULL)
            {
                goto ERR_HANDLE;
            }
            memcpy(p_scan->filter.p_scan_addr, p_filter->p_scan_addr,
                   p_filter->addr_num * sizeof(BLE_SCAN_ADDR));
        }
        else
        {
            p_filter->filter_flags &= ~BLE_SCAN_FILTER_REMOTE_ADDR_BIT;
            p_scan->filter.p_scan_addr = NULL;
            p_filter->addr_num = 0;
        }
    }

    if (cb && (p_scan->cb == NULL))
    {
        p_scan->cb = cb;
    }

    os_queue_in(&p_scan_mgr->queue, p_scan);

    if (p_scan_mgr->scan_action != BLE_SCAN_ACTION_PAUSE)
    {
        p_scan_mgr->scan_action = BLE_SCAN_ACTION_STARTED;
    }
    BTM_PRINT_TRACE1("ble_scan_start: hdl %p", p_scan);
    ble_scan_param_update();
    ble_scan_check_state();
    *p_hdl = p_scan;
    return true;

ERR_HANDLE:
    if (ad_struct_alloced)
    {
        free(p_scan->filter.ad_struct);
        p_scan->filter.ad_struct = NULL;
    }

    if (p_scan)
    {
        free(p_scan);
    }
    BTM_PRINT_ERROR0("ble_scan_start: handle error");
    return false;
}


bool ble_scan_stop(BLE_SCAN_HDL *p_hdl)
{
    BLE_SCAN_HDL hdl;
    if (p_hdl == NULL)
    {
        return false;
    }

    hdl = *p_hdl;
    if (hdl == NULL)
    {
        return true;
    }

    if (ble_scan_hdl_check(hdl) == false)
    {
        return false;
    }
    BTM_PRINT_TRACE1("ble_scan_stop: hdl %p", hdl);
    os_queue_delete(&p_scan_mgr->queue, hdl);
    T_BLE_SCAN *p_ble_scan = (T_BLE_SCAN *)hdl;
    if (p_ble_scan->filter.ad_struct)
    {
        free(p_ble_scan->filter.ad_struct);
    }
    if (p_ble_scan->filter.p_scan_addr)
    {
        free(p_ble_scan->filter.p_scan_addr);
    }
    free(hdl);
    *p_hdl = NULL;
    if (p_scan_mgr->queue.count == 0)
    {
        if (p_scan_mgr->scan_action != BLE_SCAN_ACTION_PAUSE)
        {
            p_scan_mgr->scan_action = BLE_SCAN_ACTION_STOPED;
        }
    }
    else
    {
        ble_scan_param_update();
    }
    ble_scan_check_state();
    return true;
}

bool ble_scan_resume(void)
{
    if (p_scan_mgr == NULL)
    {
        return false;
    }

    BTM_PRINT_TRACE1("ble_scan_resume: current state %d", p_scan_mgr->scan_action);

    if (p_scan_mgr->scan_action == BLE_SCAN_ACTION_PAUSE)
    {
        if (p_scan_mgr->queue.count == 0)
        {
            p_scan_mgr->scan_action = BLE_SCAN_ACTION_STOPED;
        }
        else
        {
            p_scan_mgr->scan_action = BLE_SCAN_ACTION_STARTED;
        }
        ble_scan_check_state();
    }
    return true;
}

bool ble_scan_pause(void)
{
    if (p_scan_mgr == NULL)
    {
        return false;
    }

    BTM_PRINT_TRACE1("ble_scan_pause: current state %d", p_scan_mgr->scan_action);

    if (p_scan_mgr->scan_action != BLE_SCAN_ACTION_PAUSE)
    {
        p_scan_mgr->scan_action = BLE_SCAN_ACTION_PAUSE;
        ble_scan_check_state();
    }
    return true;
}

void ble_scan_handle_gap_cb(uint8_t cb_type, T_LE_CB_DATA *cb_data)
{
    if (p_scan_mgr == NULL)
    {
        return;
    }
#if 0
    BTM_PRINT_TRACE2("ble_scan_handle_gap_cb: cb_type 0x%x, evt_type %d",
                     cb_type, cb_data->p_le_ext_adv_report_info->event_type);

    BTM_PRINT_TRACE2("ble_scan_handle_gap_cb: len %d, data %b",
                     cb_data->p_le_ext_adv_report_info->data_len,
                     TRACE_BINARY(8, cb_data->p_le_ext_adv_report_info->p_data));
#endif
    switch (cb_type)
    {
    case GAP_MSG_LE_EXT_ADV_REPORT_INFO:
        {
            for (uint8_t i = 0; i < p_scan_mgr->queue.count; i++)
            {
                T_BLE_SCAN *p_scan = os_queue_peek(&p_scan_mgr->queue, i);
                if (p_scan)
                {
                    if (match_filter(&p_scan->filter, cb_data->p_le_ext_adv_report_info))
                    {
                        BLE_SCAN_EVT_DATA evt_data = {0};
                        evt_data.report = cb_data->p_le_ext_adv_report_info;
                        p_scan->cb(BLE_SCAN_REPORT, &evt_data);
                    }
                }
            }
        }
        break;

    default:
        break;
    }
}

void ble_scan_handle_state(uint8_t gap_scan_state, uint16_t cause)
{
    if (p_scan_mgr == NULL)
    {
        return;
    }
    if (p_scan_mgr->scan_state == gap_scan_state)
    {
        return;
    }
    BTM_PRINT_TRACE3("ble_scan_handle_state: scan_state %d -> %d, cause 0x%x",
                     p_scan_mgr->scan_state, gap_scan_state, cause);
    p_scan_mgr->scan_state = gap_scan_state;
    ble_scan_check_state();
}

void ble_scan_init(void)
{
    if (p_scan_mgr)
    {
        return;
    }
    p_scan_mgr = calloc(1, sizeof(T_BLE_SCAN_MGR));
    if (p_scan_mgr == NULL)
    {
        return;
    }
    os_queue_init(&p_scan_mgr->queue);
}

#if BLE_MGR_DEINIT
void ble_scan_deinit(void)
{
    if (p_scan_mgr)
    {
        T_BLE_SCAN *p_ble_scan;
        while ((p_ble_scan = os_queue_out(&p_scan_mgr->queue)) != NULL)
        {
            if (p_ble_scan->filter.ad_struct)
            {
                free(p_ble_scan->filter.ad_struct);
            }

            if (p_ble_scan->filter.p_scan_addr)
            {
                free(p_ble_scan->filter.p_scan_addr);
            }
            free(p_ble_scan);
        }
        free(p_scan_mgr);
        p_scan_mgr = NULL;
    }
}
#endif

