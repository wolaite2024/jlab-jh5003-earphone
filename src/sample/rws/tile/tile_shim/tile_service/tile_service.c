/*
 * NOTICE
 *
 * 2017 Tile Inc.  All Rights Reserved.
 *
 * All code or other information included in the accompanying files (Tile Source Material)
 * is CONFIDENTIAL and PROPRIETARY information of Tile Inc. and your access and use is subject
 * to the terms of Tiles non-disclosure agreement as well as any other applicable agreement
 * with Tile.  The Tile Source Material may not be shared or disclosed outside your company,
 * nor distributed in or with any devices.  You may not use, copy or modify Tile Source
 * Material or any derivatives except for the purposes expressly agreed and approved by Tile.
 * All Tile Source Material is provided AS-IS without warranty of any kind.  Tile does not
 * warrant that the Tile Source Material will be error-free or fit for your purposes.
 * Tile will not be liable for any damages resulting from your use of or inability to use
 * the Tile Source Material.
 * You must include this Notice in any copies you make of the Tile Source Material.
 */

/**
 * @file tile_service.c
 * @brief Core functionality for Tile Lib
 */
#if C_APP_TILE_FEATURE_SUPPORT
#include "tile_config.h"
#include "tile_features/tile_features.h"

#include "tile_storage/tile_storage.h"

// TileLib includes
#include "tile_lib.h"
#include "tile_service.h"
#include "tile_ble_service.h"
#include "drivers/tile_gap_driver.h"
#include "drivers/tile_timer_driver.h"
#include "modules/tile_tmd_module.h"
//#include "modules/tile_trm_module.h"
#include "modules/tile_toa_module.h"

// Realtek Includes
#include "trace.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "app_main.h"

/*******************************************************************************
 * Functions
 ******************************************************************************/

/**
 * @brief Initialize Tile BLE service
 */
void tile_service_init(void)
{
    tile_storage_init(); // Initialize storage before initializing features

    tile_features_init();

    tile_ble_env.conn_handle = BLE_CONN_HANDLE_INVALID;
}

/**
 * @brief Handle Tile BLE events
 *
 * @param[in] p_evt    Event forwarded from BLE stack.
 */
void tile_on_ble_evt(tile_ble_events_t ble_event_id)
{
    switch (ble_event_id)
    {
    case TILE_BLE_GAP_EVT_CONNECTING:
        // This is to get toa initialized before notification is enabled
        tile_gap_connected(NULL);
        app_tile_set_mode(TILE_CONNECT);
        break;

    case TILE_BLE_GAP_EVT_CONNECTED:

        if (TILE_MODE_ACTIVATED != tile_stored_activation_mode)
        {
#if 0
            // when the Tile is not activated and InterimTileID is supported, the Interim TileID, Key is used.
            memcpy(tile_stored_id, interim_tile_id, TILE_ID_LEN);
            memcpy(tile_stored_auth_key, interim_tile_key, TILE_AUTH_KEY_LEN);
#endif
            if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT)
            {
                memcpy(tile_stored_id, go_air_left_tile_id, TILE_ID_LEN);
                memcpy(tile_stored_auth_key, go_air_left_tile_key, TILE_AUTH_KEY_LEN);
            }
            else
            {
                memcpy(tile_stored_id, go_air_right_tile_id, TILE_ID_LEN);
                memcpy(tile_stored_auth_key, go_air_right_tile_key, TILE_AUTH_KEY_LEN);
            }
        }

        break; // BLE_GAP_EVT_CONNECTED

    case TILE_BLE_GATTS_EVT_HVN_TX_COMPLETE:
        tile_toa_response_sent_ok();
        break;

//    case BLE_GAP_EVT_RSSI_CHANGED:
//    {
//
//      /* This is called everytime a change in RSSI is detected. The RSSI is read and sent to tile_lib
//        function tile_trm_rssi() */
//      const ble_gap_evt_rssi_changed_t* p_rssi  = &p_evt->evt.gap_evt.params.rssi_changed;

//      tile_trm_rssi(p_rssi->rssi);

//      /* To use RSSI getter function instead of a callback for RSSI changed, use
//         sd_ble_gap_rssi_get(). Example usage given below:
//      int8_t p_rssi = 0;
//      uint8_t p_ch = 0;
//      err_code = sd_ble_gap_rssi_get(tile_ble_env.conn_handle, &p_rssi, &p_ch);
//      NRF_LOG_INFO("Current RSSI levels from WRITE: %d Err Code: %d\n", p_rssi, err_code );
//      */
//      break;
//    }
//
    default:
        break;
    }
}

///*******************************************************************************
// * Local functions
// ******************************************************************************/


#endif

