/*
 * Copyright (c) 2020, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_TEAMS_BLE_POLICY
#include "string.h"
#include "stdlib.h"
#include "os_mem.h"
#include "trace.h"
#include "app_cfg.h"
#include "bt_bond.h"
#include "sysm.h"
#include "ftl.h"
#include "app_link_util.h"
#include "app_main.h"
#include "app_device.h"
#include "gap_conn_le.h"
#include "string.h"
#include "stdlib.h"
#include "app_teams_ble_policy.h"
#include "app_teams_ext_ftl.h"
#include "app_teams_rws.h"
#include "app_relay.h"
#include "app_bond.h"
#include "bt_sdp.h"
#include "btm.h"
#include "gap_le_types.h"
#include "ble_ext_adv.h"
#include "app_ble_gap.h"
#include "bas.h"
#include "app_timer.h"

static T_TEAMS_BLE_ADV_MGR teams_ble_mgr[MAX_BLE_LINK_NUM] = {0};
static uint8_t teams_ble_policy_timer_id = 0;
static T_TEAMS_BLE_DEVICE_INFO teams_ble_device_bond_info[APP_TEAMS_BLE_BOND_MAX_NUM];
uint8_t teams_bas_id = 0;

void app_teams_ble_policy_print_dev_bond_info(void)
{
    APP_PRINT_TRACE8("app_teams_ble_policy_print_dev_bond_info: bd_addr1 %s, device type %d, bd_addr2 %s, device type %d, bd_addr3 %s, device type %d, bd_addr4 %s, device type %d,",
                     TRACE_BDADDR(teams_ble_device_bond_info[0].bd_addr), teams_ble_device_bond_info[0].device_type,
                     TRACE_BDADDR(teams_ble_device_bond_info[1].bd_addr), teams_ble_device_bond_info[1].device_type,
                     TRACE_BDADDR(teams_ble_device_bond_info[2].bd_addr), teams_ble_device_bond_info[2].device_type,
                     TRACE_BDADDR(teams_ble_device_bond_info[3].bd_addr), teams_ble_device_bond_info[3].device_type);
    APP_PRINT_TRACE8("app_teams_ble_policy_print_dev_bond_info: bd_addr5 %s, device type %d, bd_addr6 %s, device type %d, bd_addr7 %s, device type %d, bd_addr8 %s, device type %d,",
                     TRACE_BDADDR(teams_ble_device_bond_info[4].bd_addr), teams_ble_device_bond_info[4].device_type,
                     TRACE_BDADDR(teams_ble_device_bond_info[5].bd_addr), teams_ble_device_bond_info[5].device_type,
                     TRACE_BDADDR(teams_ble_device_bond_info[6].bd_addr), teams_ble_device_bond_info[6].device_type,
                     TRACE_BDADDR(teams_ble_device_bond_info[7].bd_addr), teams_ble_device_bond_info[7].device_type);
}

bool app_teams_ble_policy_save_dev_bond_info(void)
{
    if (sizeof(teams_ble_device_bond_info) > APP_TEAMS_BLE_DEV_BOND_INFO_SIZE)
    {
        //save info size > ftl space
        return false;
    }

    app_teams_ble_policy_print_dev_bond_info();

#if (FTL_POOL_ENABLE == 1)
    if (app_teams_ext_save_ftl_data(APP_TEAMS_BLE_DEV_BOND_INFO_ADDR,
                                    (uint8_t *)teams_ble_device_bond_info,
                                    sizeof(teams_ble_device_bond_info)) != 0)
#else
    if (ftl_save_to_storage(teams_ble_device_bond_info, APP_TEAMS_BLE_DEV_BOND_INFO_ADDR,
                            sizeof(teams_ble_device_bond_info)) != 0)
#endif
    {
        return false;
    }

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        return app_relay_async_single(APP_MODULE_TYPE_TEAMS_APP_BLE_POLICY, TEAMS_APP_RWS_DEV_INFO_MSG);
    }
    return true;
}

bool app_teams_ble_policy_load_dev_bond_info(void)
{
    if (sizeof(teams_ble_device_bond_info) > APP_TEAMS_BLE_DEV_BOND_INFO_SIZE)
    {
        //save info size > ftl space
        return false;
    }

#if (FTL_POOL_ENABLE == 1)
    if (app_teams_ext_load_ftl_data(APP_TEAMS_BLE_DEV_BOND_INFO_ADDR,
                                    (uint8_t *)teams_ble_device_bond_info,
                                    sizeof(teams_ble_device_bond_info)) != 0)
#else
    if (ftl_load_from_storage(teams_ble_device_bond_info, APP_TEAMS_BLE_DEV_BOND_INFO_ADDR,
                              sizeof(teams_ble_device_bond_info)) == 0)
#endif
    {
        app_teams_ble_policy_print_dev_bond_info();
        return true;
    }
    else
    {
        return false;
    }
}

bool app_teams_ble_policy_add_dev_bond_info(uint8_t *bd_addr,
                                            T_APP_TEAMS_BLE_DEVICE_TYPE device_type)
{
    APP_PRINT_TRACE2("app_teams_ble_policy_add_dev_bond_info: bd_addr %s, device type %d",
                     TRACE_BDADDR(bd_addr), device_type);

    for (uint8_t i = 0; i < APP_TEAMS_BLE_BOND_MAX_NUM; i++)
    {
        if (teams_ble_device_bond_info[i].used &&
            (memcmp(teams_ble_device_bond_info[i].bd_addr, bd_addr, 6) == 0))
        {
            teams_ble_device_bond_info[i].device_type = device_type;
            app_teams_ble_policy_save_dev_bond_info();
            return true;
        }
    }

    for (uint8_t i = 0; i < APP_TEAMS_BLE_BOND_MAX_NUM; i++)
    {
        if (teams_ble_device_bond_info[i].used == false)
        {
            teams_ble_device_bond_info[i].used = true;
            memcpy(teams_ble_device_bond_info[i].bd_addr, bd_addr, 6);
            teams_ble_device_bond_info[i].device_type = device_type;
            app_teams_ble_policy_save_dev_bond_info();
            return true;
        }
    }
    return false;
}

void app_teams_ble_policy_del_dev_bond_info(uint8_t *bd_addr)
{
    APP_PRINT_TRACE1("app_teams_ble_policy_del_dev_bond_info: bd_addr %s",
                     TRACE_BDADDR(bd_addr));

    for (uint8_t i = 0; i < APP_TEAMS_BLE_BOND_MAX_NUM; i++)
    {
        if (!memcmp(teams_ble_device_bond_info[i].bd_addr, bd_addr, 6))
        {
            memset(&teams_ble_device_bond_info[i], 0, sizeof(T_TEAMS_BLE_DEVICE_INFO));
            app_teams_ble_policy_save_dev_bond_info();
        }
    }
}

void app_teams_ble_policy_clear_dev_bond_info(void)
{
    APP_PRINT_TRACE0("app_teams_ble_policy_clear_dev_bond_info");
    memset(teams_ble_device_bond_info, 0, sizeof(teams_ble_device_bond_info));
    app_teams_ble_policy_save_dev_bond_info();
}

T_TEAMS_BLE_DEVICE_INFO *app_teams_ble_policy_find_dev_bond_info(uint8_t *bd_addr)
{
    for (uint8_t i = 0; i < APP_TEAMS_BLE_BOND_MAX_NUM; i++)
    {
        if (!memcmp(teams_ble_device_bond_info[i].bd_addr, bd_addr, 6))
        {
            return &teams_ble_device_bond_info[i];
        }
    }

    return NULL;
}

void app_teams_ble_policy_set_dev_bond_info(uint8_t *bd_addr,
                                            T_APP_TEAMS_BLE_POLICY_BOND_INFO_SET_TYPE set_type, uint8_t data_length, uint8_t *p_data)
{
    T_TEAMS_BLE_DEVICE_INFO *p_app_teams_ble_bond_info = app_teams_ble_policy_find_dev_bond_info(
                                                             bd_addr);
    bool save_flag = false;

    APP_PRINT_INFO3("app_teams_ble_policy_set_dev_bond_info: bd_addr %s, set_type %d, data_length %d",
                    TRACE_BDADDR(bd_addr), set_type, data_length);

    if (p_app_teams_ble_bond_info && p_app_teams_ble_bond_info->used)
    {
        if (set_type == APP_TEAMS_BLE_POLICY_SET_DEVICE_TYPE)
        {
            if (data_length == sizeof(T_APP_TEAMS_BLE_DEVICE_TYPE))
            {
                p_app_teams_ble_bond_info->device_type = (T_APP_TEAMS_BLE_DEVICE_TYPE)(*p_data);
                save_flag = true;
            }
        }
        else if (set_type == APP_TEAMS_BLE_POLICY_SET_DEVICE_NAME)
        {
            memcpy(p_app_teams_ble_bond_info->device_name, p_data,
                   (data_length < APP_TEAMS_BLE_POLICY_BOND_INFO_NAME_MAX_LEN ? data_length :
                    APP_TEAMS_BLE_POLICY_BOND_INFO_NAME_MAX_LEN));
            save_flag = true;
        }
    }

    if (save_flag == true)
    {
        app_teams_ble_policy_save_dev_bond_info();
    }
}

T_TEAMS_BLE_ADV_MGR *app_teams_ble_policy_find_ble_adv_mgr_by_idx(uint8_t index)
{
    if (index >= MAX_BLE_LINK_NUM)
    {
        return NULL;
    }

    return &teams_ble_mgr[index];
}

T_TEAMS_BLE_ADV_MGR *app_teams_ble_policy_find_ble_adv_mgr_by_handle(uint8_t handle)
{
    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (teams_ble_mgr[i].handle == handle)
        {
            return &teams_ble_mgr[i];
        }
    }
    return NULL;
}

T_TEAMS_BLE_ADV_MGR *app_teams_ble_policy_find_ble_adv_mgr_by_bdaddr(uint8_t *bd_addr)
{
    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (!memcmp(teams_ble_mgr[i].bd_addr, bd_addr, 6))
        {
            return &teams_ble_mgr[i];
        }
    }
    return NULL;
}

T_TEAMS_BLE_ADV_MGR *app_teams_ble_policy_find_ble_adv_mgr_by_conn_id(uint8_t conn_id)
{
    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (teams_ble_mgr[i].conn_id == conn_id)
        {
            return &teams_ble_mgr[i];
        }
    }
    return NULL;
}

bool app_teams_ble_policy_adv_start(uint8_t adv_handle, uint16_t duration)
{
    T_TEAMS_BLE_ADV_MGR *ble_adv_mgr = app_teams_ble_policy_find_ble_adv_mgr_by_handle(adv_handle);
    if (ble_adv_mgr->adv_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
    {
        if (ble_ext_adv_mgr_enable(adv_handle, duration / 10) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        APP_PRINT_TRACE0("app_swift_pair_adv_start: Already started");
        return true;
    }
}

bool app_teams_ble_policy_adv_stop(uint8_t adv_handle, int8_t app_cause)
{
    if (ble_ext_adv_mgr_disable(adv_handle, app_cause) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void app_teams_ble_policy_handle_br_link_disconn(uint8_t *bd_addr)
{
    T_APP_LE_LINK *p_le_link = NULL;
    if (!bd_addr)
    {
        return;
    }
    p_le_link = app_link_find_le_link_by_addr(bd_addr);

    APP_PRINT_TRACE2("app_teams_ble_policy_handle_br_link_disconn: bd_addr %s, ble link %p",
                     TRACE_BDADDR(bd_addr), p_le_link);

    if (p_le_link)
    {
        le_disconnect(p_le_link->conn_id);
    }
    else
    {
        T_TEAMS_BLE_ADV_MGR *ble_adv_mgr = app_teams_ble_policy_find_ble_adv_mgr_by_bdaddr(bd_addr);
        if (ble_adv_mgr)
        {
            if (BLE_EXT_ADV_MGR_ADV_ENABLED == ble_ext_adv_mgr_get_adv_state(ble_adv_mgr->handle))
            {
                app_teams_ble_policy_adv_stop(ble_adv_mgr->handle, APP_STOP_ADV_CAUSE_TEAMS_BR_LINK_DISCONN);
            }
            else
            {
                app_stop_timer(&ble_adv_mgr->adv_period_timer_handle);
            }
        }
    }
}

void app_teams_ble_policy_handle_br_link_connected(uint8_t *bd_addr)
{
    T_TEAMS_BLE_ADV_MGR *p_teams_ble_adv_mgr = NULL;
    T_APP_BR_LINK *p_link = NULL;
    T_APP_LE_LINK *p_le_link = NULL;

    p_link = app_link_find_br_link(bd_addr);
    p_le_link = app_link_find_le_link_by_addr(bd_addr);
    //bredr link exists and ble link not exists and ble bond info exists
    APP_PRINT_TRACE3("app_teams_ble_policy_handle_br_link_connected: bd_addr %s, br link %p, ble link %p",
                     TRACE_BDADDR(bd_addr), p_link, p_le_link);
    if (p_link && !p_le_link && le_find_key_entry(bd_addr, GAP_REMOTE_ADDR_LE_PUBLIC))
    {
        p_teams_ble_adv_mgr = app_teams_ble_policy_find_ble_adv_mgr_by_idx(p_link->id);
        if (p_teams_ble_adv_mgr)
        {
            app_teams_ble_policy_set_adv_param(p_teams_ble_adv_mgr->handle, bd_addr);
            app_teams_ble_policy_adv_start(p_teams_ble_adv_mgr->handle, APP_TEAMS_BLE_POLICY_ADV_DURATION);
        }
    }
}

void app_teams_ble_policy_handle_le_link_disconn(uint8_t *bd_addr, uint8_t local_disc_cause,
                                                 uint16_t disc_cause)
{
    T_APP_BR_LINK *p_link = NULL;
    T_TEAMS_BLE_ADV_MGR *p_ble_adv_mgr = NULL;
    APP_PRINT_TRACE3("app_teams_ble_policy_handle_le_link_disconn: bd_addr %s, local disc cause %d, disc cause %x",
                     TRACE_BDADDR(bd_addr), local_disc_cause, disc_cause);

    //if (disc_cause == (HCI_ERR | HCI_ERR_CONN_TIMEOUT))
    {
        p_ble_adv_mgr = app_teams_ble_policy_find_ble_adv_mgr_by_bdaddr(bd_addr);
        if (p_ble_adv_mgr)
        {
            p_link = app_link_find_br_link(p_ble_adv_mgr->bd_addr);
            if (p_link)
            {
                app_teams_ble_policy_adv_start(p_ble_adv_mgr->handle, APP_TEAMS_BLE_POLICY_ADV_DURATION);
            }
        }
    }
}

static void app_teams_ble_policy_adv_callback(uint8_t cb_type, void *p_cb_data)
{
    T_BLE_EXT_ADV_CB_DATA cb_data;
    memcpy(&cb_data, p_cb_data, sizeof(T_BLE_EXT_ADV_CB_DATA));
    T_TEAMS_BLE_ADV_MGR *ble_adv_mgr = app_teams_ble_policy_find_ble_adv_mgr_by_handle(
                                           cb_data.p_ble_state_change->adv_handle);
    if (!ble_adv_mgr)
    {
        APP_PRINT_TRACE0("app_teams_ble_policy_adv_callback: ble adv mgr not find");
        return;
    }

    switch (cb_type)
    {
    case BLE_EXT_ADV_STATE_CHANGE:
        {
            ble_adv_mgr->adv_state = cb_data.p_ble_state_change->state;
            if (ble_adv_mgr->adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
            {
                APP_PRINT_TRACE1("app_teams_ble_policy_adv_callback: BLE_EXT_ADV_MGR_ADV_ENABLED, adv_handle %d",
                                 cb_data.p_ble_state_change->adv_handle);
            }
            else if (ble_adv_mgr->adv_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
            {
                APP_PRINT_TRACE1("app_teams_ble_policy_adv_callback: BLE_EXT_ADV_MGR_ADV_DISABLED, adv_handle %d",
                                 cb_data.p_ble_state_change->adv_handle);
                switch (cb_data.p_ble_state_change->stop_cause)
                {
                case BLE_EXT_ADV_STOP_CAUSE_APP:
                    APP_PRINT_TRACE1("app_teams_ble_policy_adv_callback: BLE_EXT_ADV_STOP_CAUSE_APP app_cause 0x%02x",
                                     cb_data.p_ble_state_change->app_cause);
                    break;

                case BLE_EXT_ADV_STOP_CAUSE_CONN:
                    app_stop_timer(&ble_adv_mgr->adv_period_timer_handle);
                    APP_PRINT_TRACE0("app_teams_ble_policy_adv_callback: BLE_EXT_ADV_STOP_CAUSE_CONN");
                    break;
                case BLE_EXT_ADV_STOP_CAUSE_TIMEOUT:
                    APP_PRINT_TRACE0("app_teams_ble_policy_adv_callback: BLE_EXT_ADV_STOP_CAUSE_TIMEOUT");
                    app_start_timer(&ble_adv_mgr->adv_period_timer_handle, "TEAMS_BLE_ADV_PERIOD",
                                    teams_ble_policy_timer_id, APP_TEAMS_BLE_POLICY_TIMER_ADV_PERIOD_TIMEOUT, ble_adv_mgr->handle,
                                    false,
                                    APP_TEAMS_BLE_POLICY_ADV_PERIOD);
                    break;
                default:
                    APP_PRINT_TRACE1("app_teams_ble_policy_adv_callback: stop_cause %d",
                                     cb_data.p_ble_state_change->stop_cause);
                    break;
                }
            }
        }
        break;

    case BLE_EXT_ADV_SET_CONN_INFO:
        APP_PRINT_TRACE1("app_teams_ble_policy_adv_callback: BLE_EXT_ADV_SET_CONN_INFO conn_id 0x%x",
                         cb_data.p_ble_conn_info->conn_id);
        ble_adv_mgr->adv_state = ble_ext_adv_mgr_get_adv_state(cb_data.p_ble_state_change->adv_handle);
        ble_adv_mgr->conn_id = cb_data.p_ble_conn_info->conn_id;
        break;

    default:
        break;
    }
    return;
}

void app_teams_ble_policy_adv_init(uint8_t *adv_handle)
{
    T_GAP_CAUSE ret ;
    if (!adv_handle)
    {
        return;
    }
    /* set adv parameter */
    T_LE_EXT_ADV_LEGACY_ADV_PROPERTY adv_event_prop = LE_EXT_ADV_LEGACY_ADV_CONN_LOW_DUTY_DIRECTED;
    uint16_t adv_interval_min = 0x20;
    uint16_t adv_interval_max = 0x20;
    T_GAP_LOCAL_ADDR_TYPE own_address_type = GAP_LOCAL_ADDR_LE_PUBLIC;
    T_GAP_REMOTE_ADDR_TYPE peer_address_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    T_GAP_ADV_FILTER_POLICY filter_policy = GAP_ADV_FILTER_ANY;
    uint8_t peer_address[6] = {0};

    if ((app_cfg_nv.le_single_random_addr[5] & 0xC0) != 0xC0)
    {
        le_gen_rand_addr(GAP_RAND_ADDR_STATIC, app_cfg_nv.le_single_random_addr);
    }

    /* build new adv*/
    ret = ble_ext_adv_mgr_init_adv_params(adv_handle, adv_event_prop, adv_interval_min,
                                          adv_interval_max, own_address_type, peer_address_type, peer_address,
                                          filter_policy, 0, NULL,
                                          0, NULL, app_cfg_nv.le_single_random_addr);
    APP_PRINT_TRACE2("app_teams_ble_policy_adv_init: adv_handle %d, ret %d", *adv_handle, ret);

    /* set adv event handle callback*/
    ble_ext_adv_mgr_register_callback(app_teams_ble_policy_adv_callback, *adv_handle);
}

bool app_teams_ble_policy_set_adv_param(uint8_t adv_handle, uint8_t *peer_address)
{
    T_TEAMS_BLE_ADV_MGR *ble_adv_mgr = app_teams_ble_policy_find_ble_adv_mgr_by_handle(adv_handle);
    T_GAP_CAUSE ret;
    if (ble_adv_mgr)
    {
        ble_adv_mgr->conn_id = 0xff;
        app_stop_timer(&ble_adv_mgr->adv_period_timer_handle);
        memcpy(ble_adv_mgr->bd_addr, peer_address, 6);
        T_GAP_REMOTE_ADDR_TYPE peer_addr_type = GAP_REMOTE_ADDR_LE_PUBLIC;
        ret = ble_ext_adv_mgr_change_peer_addr(adv_handle, peer_addr_type, peer_address);
        APP_PRINT_INFO1("app_teams_ble_policy_set_adv_param: ret %d", ret);
        if (ret == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

void app_teams_ble_policy_handle_power_on(void)
{
    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        app_teams_ble_policy_adv_init(&teams_ble_mgr[i].handle);
    }
}

void app_teams_ble_policy_handle_power_off(void)
{
    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        app_teams_ble_policy_adv_stop(teams_ble_mgr[i].handle, APP_STOP_ADV_CAUSE_POWER_OFF);
        app_stop_timer(&teams_ble_mgr[i].adv_period_timer_handle);
    }
}

void app_teams_ble_policy_handle_battery_change(uint8_t battery_level)
{
    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (app_db.le_link[i].used)
        {
            bas_battery_level_value_notify(app_db.le_link[i].conn_id, teams_bas_id, battery_level);
        }
    }
}

static void app_teams_ble_policy_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;

    switch (event_type)
    {
    case SYS_EVENT_POWER_ON:
        {
            app_teams_ble_policy_handle_power_on();
        }
        break;

    case SYS_EVENT_POWER_OFF:
        {
            app_teams_ble_policy_handle_power_off();
        }
        break;
    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_teams_ble_policy_dm_cback: event_type 0x%04x", event_type);
    }
}

static void app_teams_ble_policy_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BT_EVENT_ACL_CONN_BR_SC_CMPL:
        {
            APP_PRINT_INFO2("app_teams_ble_policy_bt_cback: event_type 0x%04x, cause %d", event_type,
                            param->acl_conn_br_sc_cmpl.cause);
            if (param->acl_conn_br_sc_cmpl.cause == 0)
            {
                app_teams_ble_policy_handle_br_link_connected(param->acl_conn_br_sc_cmpl.bd_addr);
            }
        }
        break;

    default:
        break;

    }
}

static void app_teams_ble_policy_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_teams_ble_policy_timeout_cb: timer_evt %d, param %d",
                     timer_evt, param);
    uint8_t adv_handle = param;
    T_TEAMS_BLE_ADV_MGR *ble_adv_mgr = app_teams_ble_policy_find_ble_adv_mgr_by_handle(adv_handle);

    switch (timer_evt)
    {
    case APP_TEAMS_BLE_POLICY_TIMER_ADV_PERIOD_TIMEOUT:
        {
            if (ble_adv_mgr)
            {
                app_teams_ble_policy_adv_start(adv_handle, APP_TEAMS_BLE_POLICY_ADV_DURATION);
            }
        }
        break;
    default:
        break;
    }
}

void app_teams_ble_policy_build_relay_adv_info(T_APP_TEAMS_RELAY_BLE_POLICY_ADV_INFO *relay_info)
{
    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        relay_info->teams_ble_link_relay_info[i].adv_state = teams_ble_mgr[i].adv_state;
        if (teams_ble_mgr[i].adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
        {
            app_teams_ble_policy_adv_stop(teams_ble_mgr[i].handle, APP_STOP_ADV_CAUSE_ROLE_SWAP);
        }
        if (teams_ble_mgr[i].adv_period_timer_handle)
        {
            relay_info->teams_ble_link_relay_info[i].adv_period_timer_start_flag = true;
            app_stop_timer(&teams_ble_mgr[i].adv_period_timer_handle);
        }
        memcpy(relay_info->teams_ble_link_relay_info[i].bd_addr, teams_ble_mgr[i].bd_addr, 6);
        relay_info->teams_ble_link_relay_info[i].conn_id = teams_ble_mgr[i].conn_id;
    }
}

void app_teams_ble_policy_handle_adv_relay_info(T_APP_TEAMS_RELAY_BLE_POLICY_ADV_INFO *relay_info,
                                                uint16_t data_size)
{
    if (data_size != sizeof(T_APP_TEAMS_RELAY_BLE_POLICY_ADV_INFO))
    {
        return;
    }

    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        teams_ble_mgr[i].adv_state = relay_info->teams_ble_link_relay_info[i].adv_state;
        if (teams_ble_mgr[i].adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
        {
            app_teams_ble_policy_adv_start(teams_ble_mgr[i].handle, APP_TEAMS_BLE_POLICY_ADV_DURATION);
        }
        else if (relay_info->teams_ble_link_relay_info[i].adv_period_timer_start_flag)
        {
            app_start_timer(&teams_ble_mgr[i].adv_period_timer_handle, "TEAMS_BLE_ADV_PERIOD",
                            teams_ble_policy_timer_id, APP_TEAMS_BLE_POLICY_TIMER_ADV_PERIOD_TIMEOUT, teams_ble_mgr[i].handle,
                            false,
                            APP_TEAMS_BLE_POLICY_ADV_PERIOD);
        }
        memcpy(teams_ble_mgr[i].bd_addr, relay_info->teams_ble_link_relay_info[i].bd_addr, 6);
        teams_ble_mgr[i].conn_id = relay_info->teams_ble_link_relay_info[i].conn_id;
    }
}

void app_teams_ble_policy_build_relay_dev_bond_info(T_APP_TEAMS_RELAY_BLE_POLICY_DEV_BOND_INFO
                                                    *relay_info)
{
    memcpy((uint8_t *)relay_info->teams_ble_dev_bond_relay_info, (uint8_t *)teams_ble_device_bond_info,
           sizeof(T_APP_TEAMS_RELAY_BLE_POLICY_DEV_BOND_INFO));
}

void app_teams_ble_policy_handle_dev_bond_relay_info(T_APP_TEAMS_RELAY_BLE_POLICY_DEV_BOND_INFO
                                                     *relay_info,
                                                     uint16_t data_size)
{
    if (data_size != sizeof(teams_ble_device_bond_info))
    {
        return;
    }

    memcpy(teams_ble_device_bond_info, relay_info->teams_ble_dev_bond_relay_info,
           sizeof(teams_ble_device_bond_info));
    app_teams_ble_policy_save_dev_bond_info();
}

void app_teams_ble_policy_init(void)
{
    app_teams_ble_policy_load_dev_bond_info();
    sys_mgr_cback_register(app_teams_ble_policy_dm_cback);
    app_timer_reg_cb(app_teams_ble_policy_timeout_cb, &teams_ble_policy_timer_id);
    bt_mgr_cback_register(app_teams_ble_policy_bt_cback);
}

#endif

