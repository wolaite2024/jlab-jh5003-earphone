/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if C_APP_TILE_FEATURE_SUPPORT
#include "trace.h"
#include "app_ble_tile_client.h"
#include "os_mem.h"
#include "app_main.h"
#include "app_report.h"
#include "app_cmd.h"
#include "ble_ext_adv.h"
#include "string.h"
#include "tile_lib.h"
#include "gatt.h"
#include "tile_storage.h"
#include "app_bt_policy_api.h"
#include "ftl.h"
#include "data_storage.h"
#include "tile_features.h"
#include "app_device.h"
#include "app_relay.h"
#include "tile_ble_service.h"
#include "tile_gap_driver.h"
#include "gap_conn_le.h"

static uint8_t tile_func_adv_handle;

void app_tile_shpping_adv_enable(void)
{
    app_tile_update_adv_content(TILE_MODE_SHIPPING);
}

void app_tile_active_adv_enable(void)
{
    app_tile_update_adv_content(TILE_MODE_ACTIVATED);
}

void app_tile_disable(void)
{
    if (app_db.tile_state != 0)
    {
        APP_PRINT_TRACE0("app_tile_disable");

        if (tile_ble_env.conn_handle != BLE_CONN_HANDLE_INVALID)
        {
            le_disconnect(tile_ble_env.conn_handle);
            tile_gap_disconnected();
        }
        ble_ext_adv_mgr_disable(tile_func_adv_handle, 0);
        app_db.tile_state = 0;
    }
}

void app_tile_stop_adv(void)
{
    if (app_db.tile_state != 0)
    {
        APP_PRINT_TRACE0("app_tile_stop_adv");
        ble_ext_adv_mgr_disable(tile_func_adv_handle, 0);
        app_db.tile_state = 0;
    }
}

void app_tile_change_adv_interval(uint16_t adv_interval)
{
    APP_PRINT_TRACE1("app_tile_change_adv_interval: change adv_interval to 0x%x", adv_interval);

    app_tile_stop_adv();

    ble_ext_adv_mgr_change_adv_interval(tile_func_adv_handle, adv_interval);

    app_tile_set_mode(TILE_CHECK_STATE_CHANGE);
}

void app_tile_set_mode(T_TILE_BUD_TO_SRC_STATE mode)
{
    uint8_t tile_current = tile_get_current_mode();

    uint8_t ever_tile_conn_addr[6];
    memcpy(ever_tile_conn_addr, temp_tile_addr, 6);

    APP_PRINT_TRACE6("app_tile_set_mode: mode %d, tile_current %d, tile_state %d, conn_handle %d, realtek_bdaddr %s, ever_tile_conn_addr %s",
                     mode, tile_current, app_db.tile_state, tile_ble_env.conn_handle, TRACE_BDADDR(realtek_bdaddr),
                     TRACE_BDADDR(ever_tile_conn_addr));

    if (mode == TILE_DISCONNECT)
    {
        if (tile_ble_env.conn_handle == BLE_CONN_HANDLE_INVALID)
        {
            if (tile_current == TILE_MODE_SHIPPING)
            {
                app_tile_shpping_adv_enable();
            }
            else
            {
                app_tile_active_adv_enable();
            }
        }
    }
    else if (mode == TILE_CONNECT)
    {
        if (app_db.tile_state == TILE_MODE_ACTIVATED)
        {
            app_tile_active_adv_enable();
        }
    }
    else if (mode == TILE_CHECK_STATE_CHANGE)
    {
        if ((memcmp(ever_tile_conn_addr, realtek_bdaddr, 6) == 0) && (tile_current == TILE_MODE_ACTIVATED))
        {
            if (app_db.tile_state != TILE_MODE_ACTIVATED)
            {
                app_tile_active_adv_enable();
                app_db.tile_state = TILE_MODE_ACTIVATED;
            }
        }
        else
        {
            if (app_db.tile_state != TILE_MODE_SHIPPING)
            {
                tile_set_current_mode(TILE_MODE_SHIPPING);
                app_tile_shpping_adv_enable();
                if (app_cfg_nv.app_is_power_on == 0)
                {
                    /* power off don't enable tile shipping mode adv */
                }
                else
                {
                    app_db.tile_state = TILE_MODE_SHIPPING;
                }
            }
        }
    }
}

const uint8_t adv_data_tile_shipping[] =
{
    /* Flags */
    0x02,             /* length */
    GAP_ADTYPE_FLAGS, /* type="Flags" */
    GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
    /* Service */
    0x03,             /* length of 16 bit complete section*/
    GAP_ADTYPE_16BIT_COMPLETE,
    LO_WORD(TILE_SHIPPING_UUID),
    HI_WORD(TILE_SHIPPING_UUID),
    /* Local name */
    0x0F,             /* length */
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    'B', 'L', 'E', '_', 'P', 'E', 'R', 'I', 'P', 'H', 'E', 'R', 'A', 'L',
};

/** @brief  GAP - Advertisement data (max size = 31 bytes, best kept short to conserve power) */
const uint8_t adv_data_tile_activated[] =
{
    /* Flags */
    0x02,             /* length */
    GAP_ADTYPE_FLAGS, /* type="Flags" */
    GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
    /* Service */
    0x03,             /* length of 16 bit complete section*/
    GAP_ADTYPE_16BIT_COMPLETE,
    LO_WORD(TILE_ACTIVATED_UUID),
    HI_WORD(TILE_ACTIVATED_UUID),
    /* Local name */
    0x0F,             /* length */
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    'B', 'L', 'E', '_', 'P', 'E', 'R', 'I', 'P', 'H', 'E', 'R', 'A', 'L',
};

void app_tile_adv_init(void)
{
    T_LE_EXT_ADV_LEGACY_ADV_PROPERTY adv_event_prop =
        LE_EXT_ADV_LEGACY_ADV_CONN_SCAN_UNDIRECTED;
    uint16_t adv_interval_min = TILE_ADV_INTERVAL_LONG;
    uint16_t adv_interval_max = TILE_ADV_INTERVAL_LONG;
    T_GAP_LOCAL_ADDR_TYPE own_address_type = GAP_LOCAL_ADDR_LE_RANDOM;
    T_GAP_REMOTE_ADDR_TYPE peer_address_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t  peer_address[6] = {0, 0, 0, 0, 0, 0};
    T_GAP_ADV_FILTER_POLICY filter_policy = GAP_ADV_FILTER_ANY;

    uint8_t tile_stored_le_rand_addr[6] = {0, 0, 0, 0, 0, 0};
    ftl_load_from_storage(tile_stored_le_rand_addr, TILE_LINK_BUD_LE_ADDR, TILE_ADDR_SIZE);

    if ((tile_stored_le_rand_addr[5] & 0xC0) != 0xC0)
    {
        //le_gen_rand_addr(GAP_RAND_ADDR_STATIC, tile_stored_le_rand_addr);
        memcpy(tile_stored_le_rand_addr, realtek_bdaddr, 6);

        tile_stored_le_rand_addr[5] |= 0xC0;

        if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            tile_stored_le_rand_addr[0] += 0xAA;
            tile_stored_le_rand_addr[1] += 0xAA;
            tile_stored_le_rand_addr[2] += 0xAA;
        }
        else
        {
            tile_stored_le_rand_addr[0] += 0xBB;
            tile_stored_le_rand_addr[1] += 0xBB;
            tile_stored_le_rand_addr[2] += 0xBB;
        }

        uint8_t bd_all_zeros[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0xC0};
        uint8_t bd_all_ones[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        if ((memcmp(tile_stored_le_rand_addr, bd_all_zeros, 6) == 0) ||
            (memcmp(tile_stored_le_rand_addr, bd_all_ones, 6) == 0))
        {
            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                tile_stored_le_rand_addr[0] = 0x66;
            }
            else
            {
                tile_stored_le_rand_addr[0] = 0x77;
            }
        }

        ftl_save_to_storage(tile_stored_le_rand_addr, TILE_LINK_BUD_LE_ADDR, TILE_ADDR_SIZE);
    }

    ble_ext_adv_mgr_init_adv_params(&tile_func_adv_handle,
                                    adv_event_prop,
                                    adv_interval_min,
                                    adv_interval_max,
                                    own_address_type,
                                    peer_address_type,
                                    peer_address,
                                    filter_policy,
                                    0, NULL, 0, NULL, tile_stored_le_rand_addr);

    ble_ext_adv_mgr_register_callback(app_tile_adv_callback,
                                      tile_func_adv_handle);

    APP_PRINT_TRACE2("app_tile_adv_init: tile_stored_le_rand_addr %s, tile_func_adv_handle %d",
                     TRACE_BDADDR(tile_stored_le_rand_addr), tile_func_adv_handle);
}

static void app_tile_adv_callback(uint8_t cb_type, void *p_cb_data)
{
    T_BLE_EXT_ADV_CB_DATA cb_data;

    memcpy(&cb_data, p_cb_data, sizeof(T_BLE_EXT_ADV_CB_DATA));
    switch (cb_type)
    {
    case BLE_EXT_ADV_STATE_CHANGE:
        APP_PRINT_TRACE4("app_tile_adv_callback: BLE_EXT_ADV_STATE_CHANGE, adv_handle %d, state %d, stop_cause %d, app_cause %d",
                         cb_data.p_ble_state_change->adv_handle,
                         cb_data.p_ble_state_change->state,
                         cb_data.p_ble_state_change->stop_cause,
                         cb_data.p_ble_state_change->app_cause);
        break;

    default:
        break;
    }
}

void app_tile_update_adv_content(uint8_t tile_mode_input)
{
    APP_PRINT_INFO3("app_tile_update_adv_content %d, %d, app_is_power_on %d",
                    tile_mode_input,
                    tile_get_current_mode(), app_cfg_nv.app_is_power_on);

    if (TILE_MODE_ACTIVATED == tile_mode_input)
    {
        APP_PRINT_INFO0("adv_data_tile_activated");

        ble_ext_adv_mgr_set_adv_data(tile_func_adv_handle,
                                     sizeof(adv_data_tile_activated),
                                     (uint8_t *)&adv_data_tile_activated);
    }
    else
    {
        if (app_cfg_nv.app_is_power_on == 0)
        {
            /* power off don't enable tile shipping mode adv */
            return;
        }

        APP_PRINT_INFO0("adv_data_tile_shipping");

        ble_ext_adv_mgr_set_adv_data(tile_func_adv_handle,
                                     sizeof(adv_data_tile_shipping),
                                     (uint8_t *)&adv_data_tile_shipping);
    }
    ble_ext_adv_mgr_enable(tile_func_adv_handle, 0);
}

void app_tile_handle_role_swap(T_BT_EVENT_PARAM_REMOTE_ROLESWAP_STATUS *p_role_swap_status)
{
    if (p_role_swap_status->status == BTM_ROLESWAP_STATUS_START_IND)
    {
        app_tile_stop_adv();
    }
    else if (p_role_swap_status->status == BTM_ROLESWAP_STATUS_SUCCESS)
    {
        app_tile_set_mode(TILE_CHECK_STATE_CHANGE);
    }
    else if (p_role_swap_status->status == BTM_ROLESWAP_STATUS_TERMINATED ||
             p_role_swap_status->status == BTM_ROLESWAP_STATUS_FAIL)
    {
        app_tile_set_mode(TILE_CHECK_STATE_CHANGE);
    }
}

#endif
