/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_BLE_SWIFT_PAIR_SUPPORT
#include "os_mem.h"
#include "trace.h"
#include "gap_le_types.h"
#include "btm.h"
#include "ble_ext_adv.h"
#include "app_cfg.h"
#include "app_main.h"
#include "string.h"
#include "bt_types.h"
#include "app_timer.h"
#include "app_ble_swift_pair.h"
#include "app_adv_stop_cause.h"
#include "app_ble_rand_addr_mgr.h"

uint8_t swift_pair_adv_handle = 0xff;

static T_BLE_EXT_ADV_MGR_STATE swift_pair_adv_state = BLE_EXT_ADV_MGR_ADV_DISABLED;
static uint8_t swift_pair_adv_len = 7;
static uint8_t swift_pair_adv_data[31] = {0};
static T_APP_SWIFT_PAIR_MODE swift_pair_data_mode = APP_TIMER_SWIFT_PAIR_MODE_LE_BREDR;
static uint8_t app_ble_swift_pair_timer_id = 0;
static uint8_t timer_idx_swift_pair_change_adv_data = 0;
static uint8_t timer_idx_swift_pair_change_adv_interval = 0;
static uint16_t swift_pair_high_duty_interval = 0x30;
static uint16_t swift_pair_low_duty_interval = 0xA0;
T_LE_EXT_ADV_LEGACY_ADV_PROPERTY adv_event_prop = LE_EXT_ADV_LEGACY_ADV_CONN_SCAN_UNDIRECTED;

static uint8_t swift_pair_adv_no_vonder_section_data[3] =
{
    /* Flags */
    0x02,/* length */
    GAP_ADTYPE_FLAGS,
    GAP_ADTYPE_FLAGS_GENERAL | GAP_ADTYPE_FLAGS_SIMULTANEOUS_LE_BREDR_CONTROLLER | GAP_ADTYPE_FLAGS_SIMULTANEOUS_LE_BREDR_HOST,
};

static uint8_t swift_pair_adv_init_data[] =
{
    /* Flags */
    0x00,       /* length, when get all data, count length */
    GAP_ADTYPE_MANUFACTURER_SPECIFIC,
    0x06,/*mocrosoftVendor ID*/
    0x00,/*mocrosoftVendor ID*/
    MICROSOFT_BEACON_ID,/*mocrosoftVendor ID*/

    0x00,/*microsoft beacon sub scenario*/
    MICROSOFT_RESERVED_RSSI_BYTE,/*reserved rssi byte*/
};

static void app_swift_pair_adv_callback(uint8_t cb_type, void *p_cb_data)
{
    T_BLE_EXT_ADV_CB_DATA cb_data;
    memcpy(&cb_data, p_cb_data, sizeof(T_BLE_EXT_ADV_CB_DATA));
    APP_PRINT_INFO4("app_swift_pair_adv_callback: type %d, adv_handle %d, stop_cause %d, app_cause %d",
                    cb_type, cb_data.p_ble_state_change->adv_handle,
                    cb_data.p_ble_state_change->stop_cause,
                    cb_data.p_ble_state_change->app_cause);
    switch (cb_type)
    {
    case BLE_EXT_ADV_STATE_CHANGE:
        {
            swift_pair_adv_state = cb_data.p_ble_state_change->state;
            if (swift_pair_adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
            {

            }
            else if (swift_pair_adv_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
            {
                switch (cb_data.p_ble_state_change->stop_cause)
                {
                case BLE_EXT_ADV_STOP_CAUSE_APP:
                    break;
                case BLE_EXT_ADV_STOP_CAUSE_CONN:
                    break;
                case BLE_EXT_ADV_STOP_CAUSE_TIMEOUT:
                    break;
                default:
                    break;
                }
            }
        }
        break;

    case BLE_EXT_ADV_SET_CONN_INFO:
        swift_pair_adv_state = ble_ext_adv_mgr_get_adv_state(swift_pair_adv_handle);
        break;

    default:
        break;
    }
    return;
}

//start action must be after app_swift_pair_handle_power_on
bool app_swift_pair_adv_start(uint16_t duration_10ms)
{
    APP_PRINT_TRACE1("app_swift_pair_adv_start: swift_pair_adv_state %d", swift_pair_adv_state);

    if (swift_pair_adv_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
    {
        if (ble_ext_adv_mgr_enable(swift_pair_adv_handle, duration_10ms) == GAP_CAUSE_SUCCESS)
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
        APP_PRINT_WARN0("app_swift_pair_adv_start: Already started");
        return true;
    }
}

bool app_swift_pair_adv_stop(int8_t app_cause)
{
    if (ble_ext_adv_mgr_disable(swift_pair_adv_handle, app_cause) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*only update local adv data, user need to set adv data to controller manually */
void app_swift_pair_select_mode(T_APP_SWIFT_PAIR_MODE mode)
{
    uint32_t appearance;
    uint8_t name_len;
    //reset adv data
    memset(swift_pair_adv_data, 0, sizeof(swift_pair_adv_data));

    if (mode == APP_TIMER_SWIFT_PAIR_MODE_NO_VENDOR_SECTION)
    {
        memcpy(swift_pair_adv_data, swift_pair_adv_no_vonder_section_data,
               sizeof(swift_pair_adv_no_vonder_section_data));
        swift_pair_adv_len = sizeof(swift_pair_adv_no_vonder_section_data);
        return;
    }

    //set adv init data
    memcpy(swift_pair_adv_data, swift_pair_adv_init_data, sizeof(swift_pair_adv_init_data));
    swift_pair_adv_len = sizeof(swift_pair_adv_init_data);

    //set sub scenario field
    swift_pair_adv_data[5] = (MICROSOFT_BEACON_SUB_SCENARIO + mode);

    if (mode == APP_TIMER_SWIFT_PAIR_MODE_BREDR_ONLY)
    {
        /* set bd_addr in adv data*/
        if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE ||
            app_cfg_const.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            memcpy(&swift_pair_adv_data[swift_pair_adv_len], app_db.factory_addr, 6);
        }
        else
        {
            memcpy(&swift_pair_adv_data[swift_pair_adv_len], app_cfg_nv.bud_peer_factory_addr, 6);
        }
        swift_pair_adv_len += 6;

        /* set adv prop*/
        adv_event_prop = LE_EXT_ADV_LEGACY_ADV_NON_SCAN_NON_CONN_UNDIRECTED;
    }

    if (mode == APP_TIMER_SWIFT_PAIR_MODE_LE_BREDR ||
        mode == APP_TIMER_SWIFT_PAIR_MODE_BREDR_ONLY)
    {
        /* set appearance in adv data*/
        appearance = (MAJOR_DEVICE_CLASS_AUDIO | MINOR_DEVICE_CLASS_HEADSET);
        memcpy(&swift_pair_adv_data[swift_pair_adv_len], &appearance, 3);
        swift_pair_adv_len += 3;
    }

    /* set device legacy name in adv data*/
    name_len = strlen((const char *)app_cfg_nv.device_name_legacy);
    if (name_len > (sizeof(swift_pair_adv_data) - swift_pair_adv_len))
    {
        name_len = (sizeof(swift_pair_adv_data) - swift_pair_adv_len);
    }
    memcpy(&swift_pair_adv_data[swift_pair_adv_len], app_cfg_nv.device_name_legacy, name_len);
    swift_pair_adv_len += name_len;

    //set adv length
    swift_pair_adv_data[0] = swift_pair_adv_len - 1;

    return ;
}

void app_swift_pair_adv_init(void)
{
    /* set adv parameter */
    T_GAP_LOCAL_ADDR_TYPE own_address_type = GAP_LOCAL_ADDR_LE_RANDOM;
    T_GAP_REMOTE_ADDR_TYPE peer_address_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t  peer_address[6] = {0, 0, 0, 0, 0, 0};
    T_GAP_ADV_FILTER_POLICY filter_policy = GAP_ADV_FILTER_ANY;

    /* set adv data*/
    app_swift_pair_select_mode(swift_pair_data_mode);

    uint8_t random_addr[6] = {0};
    app_ble_rand_addr_get(random_addr);

    /* build new adv*/
    ble_ext_adv_mgr_init_adv_params(&swift_pair_adv_handle, adv_event_prop,
                                    swift_pair_high_duty_interval,
                                    swift_pair_high_duty_interval, own_address_type, peer_address_type, peer_address,
                                    filter_policy, swift_pair_adv_len, swift_pair_adv_data,
                                    0, NULL, random_addr);

    /* set adv event handle callback*/
    ble_ext_adv_mgr_register_callback(app_swift_pair_adv_callback, swift_pair_adv_handle);
}

void app_swift_pair_start_adv_data_change_timer(uint16_t timeout)
{
    app_start_timer(&timer_idx_swift_pair_change_adv_data, "swift_pair_change_adv_data",
                    app_ble_swift_pair_timer_id, APP_TIMER_SWIFT_PAIR_CHANGE_ADV_DATA, 0, false,
                    timeout * 1000);
}

void app_swift_pair_stop_adv_data_change_timer(void)
{
    app_stop_timer(&timer_idx_swift_pair_change_adv_data);
}

void app_swift_pair_start_adv_interval_change_timer(uint16_t timeout)
{
    app_start_timer(&timer_idx_swift_pair_change_adv_interval,
                    "swift_pair_change_adv_interval", app_ble_swift_pair_timer_id,
                    APP_TIMER_SWIFT_PAIR_CHANGE_ADV_INTERVAL, 0, false,
                    timeout * 1000);
}

void app_swift_pair_stop_adv_interval_change_timer(void)
{
    app_stop_timer(&timer_idx_swift_pair_change_adv_interval);
}

void app_swift_pair_handle_power_on(void)
{
    app_swift_pair_adv_init();
}

void app_swift_pair_handle_power_off(void)
{
    app_swift_pair_stop_adv_data_change_timer();
    app_swift_pair_stop_adv_interval_change_timer();
    app_swift_pair_adv_stop(APP_STOP_ADV_CAUSE_POWER_OFF);
}

void app_swift_pair_handle_radio_mode_change(T_BT_DEVICE_MODE radio_mode)
{
    uint16_t swift_adv_keep_vendor_section_timeout;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }

    if (app_cfg_const.timer_pairing_timeout == 0)
    {
        swift_adv_keep_vendor_section_timeout = 0;
    }
    else if (app_cfg_const.timer_pairing_timeout <=
             APP_SWIFT_PAIR_DEFAULT_ADV_DATA_CHANGE_PRIOR_TIMEOUT)
    {
        swift_adv_keep_vendor_section_timeout = APP_SWIFT_PAIR_ADV_KEEP_VENDOR_SECTUON_MIN_TIMEOUT;
    }
    else
    {
        swift_adv_keep_vendor_section_timeout = app_cfg_const.timer_pairing_timeout -
                                                APP_SWIFT_PAIR_DEFAULT_ADV_DATA_CHANGE_PRIOR_TIMEOUT;
    }

    if (radio_mode == BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE)
    {
        if (swift_adv_keep_vendor_section_timeout)
        {
            app_swift_pair_start_adv_data_change_timer(swift_adv_keep_vendor_section_timeout);
        }

        if ((app_cfg_const.timer_pairing_timeout == 0) ||
            (app_cfg_const.timer_pairing_timeout > APP_SWIFT_PAIR_DEFAULT_ADV_HIGH_DUTY_DURATION))
        {
            app_swift_pair_start_adv_interval_change_timer(APP_SWIFT_PAIR_DEFAULT_ADV_HIGH_DUTY_DURATION);
        }
        ble_ext_adv_mgr_change_adv_interval(swift_pair_adv_handle, swift_pair_high_duty_interval);
        app_swift_pair_select_mode(swift_pair_data_mode);
        ble_ext_adv_mgr_set_adv_data(swift_pair_adv_handle, swift_pair_adv_len, swift_pair_adv_data);
        app_swift_pair_adv_start(app_cfg_const.timer_pairing_timeout * 100);
    }
    else
    {
        app_swift_pair_stop_adv_data_change_timer();
        app_swift_pair_stop_adv_interval_change_timer();
        app_swift_pair_adv_stop(APP_STOP_ADV_CAUSE_EXIT_PAIRING_MODE);
    }
}

static void app_swift_pair_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_INFO2("app_swift_pair_timeout_cb: timer_evt %d, param %d",
                    timer_evt, param);

    switch (timer_evt)
    {
    case APP_TIMER_SWIFT_PAIR_CHANGE_ADV_DATA:
        {
            app_swift_pair_select_mode(APP_TIMER_SWIFT_PAIR_MODE_NO_VENDOR_SECTION);
            ble_ext_adv_mgr_set_adv_data(swift_pair_adv_handle, swift_pair_adv_len, swift_pair_adv_data);
        }
        break;

    case APP_TIMER_SWIFT_PAIR_CHANGE_ADV_INTERVAL:
        {
            ble_ext_adv_mgr_change_adv_interval(swift_pair_adv_handle, swift_pair_low_duty_interval);
        }
        break;

    default:
        break;
    }
}

void app_swift_pair_init(void)
{
    app_timer_reg_cb(app_swift_pair_timeout_cb, &app_ble_swift_pair_timer_id);
}

#endif

