/*
 * NOTICE
 *
 * 2016 Tile Inc.  All Rights Reserved.
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
 * @file tile_features.h
 * @brief Support for features in Tile Lib
 */

#ifndef TILE_FEATURES_H_
#define TILE_FEATURES_H_

//#include "app_timer.h"
#include "drivers/tile_timer_driver.h"

extern uint8_t realtek_bdaddr[6];

typedef struct
{
    uint16_t conn_handle;
} tile_ble_env_t;

typedef enum
{
    TILE_CHECK_STATE_CHANGE   = 0x00,
    TILE_CONNECT              = 0x01,
    TILE_DISCONNECT           = 0x02,
} T_TILE_BUD_TO_SRC_STATE;

typedef enum
{
    TILE_AUTHORIZE_STATE_NONE   = 0x00,
    TILE_AUTHORIZE_STATE_ERROR  = 0x01,
    TILE_AUTHORIZE_STATE_OK     = 0x02,
} T_TILE_AUTHORIZE_STATE;

extern tile_ble_env_t tile_ble_env;
extern T_TILE_AUTHORIZE_STATE tile_authorize_state;

void tile_create_timers(void);
void tile_features_init(void);
int  tile_mode_set(uint8_t mode);
void tile_update_tdi_module_info(void);
#endif
