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
 * @file tile_storage.h
 * @brief Tile storage system
 */


#ifndef TILE_STORAGE_H_
#define TILE_STORAGE_H_


#include <stdint.h>
#include "data_storage.h"
#include "tile_lib.h"

#define SIGNATURE_SIZE                2
#define MODE_SIZE                     1
#define TILE_ID_SIZE                  TILE_ID_LEN
#define TILE_AUTH_KEY_SIZE            TILE_AUTH_KEY_LEN

#define MODE_OFFSET                   0
#define TILE_ID_OFFSET                (MODE_OFFSET + MODE_SIZE)
#define TILE_AUTH_KEY_OFFSET          (TILE_ID_OFFSET + TILE_ID_SIZE)
#define SIGNATURE_OFFSET              (TILE_AUTH_KEY_OFFSET + TILE_AUTH_KEY_SIZE)

#define PERSIST_SIGNATURE             0xA5A6

extern uint8_t         tile_stored_activation_mode;
extern uint8_t         tile_stored_id[TILE_ID_LEN];
extern uint8_t         tile_stored_auth_key[TILE_AUTH_KEY_LEN];

extern char tile_hw_version[];
extern char tile_model_number[];

extern const uint8_t interim_tile_id[];
extern const uint8_t interim_tile_key[];

extern const uint8_t go_air_left_tile_id[];
extern const uint8_t go_air_left_tile_key[];

extern const uint8_t go_air_right_tile_id[];
extern const uint8_t go_air_right_tile_key[];

extern uint8_t temp_tile_persist[TILE_STORAGE_SIZE];
extern uint8_t temp_tile_addr[TILE_ADDR_SIZE];

void tile_storage_init(void);
void tile_store_app_data(void);
void tile_store_addr(void);
uint8_t tile_get_current_mode(void);
void tile_set_current_mode(uint8_t action);
#endif
