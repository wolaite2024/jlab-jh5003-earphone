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
 * @file tile_storage.c
 * @brief Tile storage system
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#if C_APP_TILE_FEATURE_SUPPORT
#include "tile_storage.h"

#include "tile_lib.h"
#include "tile_config.h"
#include "modules/tile_tmd_module.h"
#include "tile_features/tile_features.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "data_storage.h"
#include "trace.h"
#include "ftl.h"
#include "remote.h"
#include "app_main.h"
#include "app_relay.h"
#include "tile_features.h"

/*******************************************************************************
 * Global variables
 ******************************************************************************/

extern struct tile_song_module song_module;
extern uint8_t g_FindActivate_SongPlayed;

/*******************************************************************************
 * Local variables
 ******************************************************************************/

static volatile bool write_in_progress   = false;
static volatile bool write_one_more_time = false;

char tile_hw_version[]    = "01.01";
char tile_model_number[]  = "RLTK 01.01";

uint16_t        tile_persist_signature;
uint8_t         tile_stored_activation_mode;
uint8_t         tile_stored_id[TILE_ID_LEN];
uint8_t         tile_stored_auth_key[TILE_AUTH_KEY_LEN];

const uint8_t interim_tile_id[8]    = INTERIM_TILE_ID;
const uint8_t interim_tile_key[16]  = INTERIM_AUTH_KEY;

const uint8_t go_air_left_tile_id[8]    = GO_AIR_LEFT_TILE_ID;
const uint8_t go_air_left_tile_key[16]  = GO_AIR_LEFT_TILE_KEY;

const uint8_t go_air_right_tile_id[8]    = GO_AIR_RIGHT_TILE_ID;
const uint8_t go_air_right_tile_key[16]  = GO_AIR_RIGHT_TILE_KEY;

/*******************************************************************************
 * Forward declarations
 ******************************************************************************/
uint16_t tile_get_adv_uuid(void);

void trigger_software_reset(void);
bool SongPlaying(void);

//*******************************************************************************
uint8_t temp_tile_persist[TILE_STORAGE_SIZE];
uint8_t temp_tile_addr[TILE_ADDR_SIZE];

void tile_storage_init(void)
{
    uint32_t return_code;

    memset(temp_tile_persist, 0, TILE_STORAGE_SIZE);

    //return_code = ftl_load(temp_tile_persist, TILE_APPDATA_BANK_A_ADDR, sizeof(temp_tile_persist));
    return_code = ftl_load_from_storage(temp_tile_persist, TILE_APPDATA_BANK_A_ADDR,
                                        sizeof(temp_tile_persist));

    if ((temp_tile_persist[SIGNATURE_OFFSET] == (PERSIST_SIGNATURE & 0x00FF)) &&
        (temp_tile_persist[SIGNATURE_OFFSET + 1] == ((PERSIST_SIGNATURE & 0xFF00) >> 8)))
    {
        // Data Valid
        tile_stored_activation_mode = temp_tile_persist[MODE_OFFSET];
        memcpy(tile_stored_id, &temp_tile_persist[TILE_ID_OFFSET], TILE_ID_SIZE);
        memcpy(tile_stored_auth_key, &temp_tile_persist[TILE_AUTH_KEY_OFFSET], TILE_AUTH_KEY_SIZE);
    }
    else
    {
        //APP_PRINT_INFO1("*********Tile Flash INVALID, return code %u", return_code);
        tile_stored_activation_mode   = TILE_MODE_SHIPPING;
    }

    APP_PRINT_INFO1("tile_storage_init: return_code %d", return_code);
}

uint8_t tile_get_current_mode(void)
{
    if (tile_stored_activation_mode == TILE_MODE_MANUFACTURING)
    {
        uint8_t tile_app_data[4];
        ftl_load_from_storage(tile_app_data, TILE_APPDATA_BANK_A_ADDR, 4);
        tile_stored_activation_mode = tile_app_data[MODE_OFFSET];
    }

    return tile_stored_activation_mode;
}

void tile_set_current_mode(uint8_t action)
{
    APP_PRINT_INFO2("tile_set_current_mode = %d,%d",
                    action, tile_stored_activation_mode);
    tile_stored_activation_mode = action;
}

//****************************************************************************************
/**
 * @brief Save tile_persist to flash
 *        Purpose of this Function is to Store updated AppData part of persist memory
 */
void tile_store_app_data(void)
{
    uint32_t return_code;

    tile_persist_signature = PERSIST_SIGNATURE;
    memcpy(&temp_tile_persist[SIGNATURE_OFFSET], &tile_persist_signature, SIGNATURE_SIZE);
    temp_tile_persist[MODE_OFFSET] = tile_stored_activation_mode;
    memcpy(&temp_tile_persist[TILE_ID_OFFSET], tile_stored_id, TILE_ID_SIZE);
    memcpy(&temp_tile_persist[TILE_AUTH_KEY_OFFSET], tile_stored_auth_key, TILE_AUTH_KEY_SIZE);

    //return_code = ftl_save(temp_tile_persist, TILE_APPDATA_BANK_A_ADDR, sizeof(temp_tile_persist));
    return_code = ftl_save_to_storage(temp_tile_persist, TILE_APPDATA_BANK_A_ADDR,
                                      sizeof(temp_tile_persist));

    APP_PRINT_INFO1("tile_store_app_data: return_code %d", return_code);
    //APP_PRINT_INFO1("*********Tile return code, %u", return_code);

//    for (uint16_t index = 0; index < TILE_STORAGE_SIZE; index++)
//    {
//        DBG_DIRECT("0x%x", temp_tile_persist[index]);
//    }
}
void tile_store_addr(void)
{
    uint32_t err_code;
    memcpy(&temp_tile_addr[0], realtek_bdaddr, 6);

    err_code = ftl_save_to_storage(temp_tile_addr, TILE_LINK_BUD_ADDR,
                                   sizeof(temp_tile_addr));
    APP_PRINT_INFO1("tile_store_addr: err_code %d", err_code);
}
#endif

