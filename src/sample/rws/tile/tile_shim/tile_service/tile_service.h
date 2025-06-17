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
 * @file tile_service.h"
 * @brief Core functionality for Tile Lib
 */


#ifndef TILE_SERVICE_H_
#define TILE_SERVICE_H_

#include <stdint.h>
#include "tile_lib.h"

typedef enum
{
    TILE_BLE_GAP_EVT_DISCONNECTED,
    TILE_BLE_GAP_EVT_CONNECTED,
    TILE_BLE_GAP_EVT_CONNECTING,
    TILE_BLE_GATTS_EVT_HVN_TX_COMPLETE
} tile_ble_events_t;

void tile_service_init(void);
void tile_on_ble_evt(tile_ble_events_t ble_event_id);
#endif

