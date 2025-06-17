
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
 * @file tile_features.c
 * @brief Support for features in Tile Lib
 */
#if C_APP_TILE_FEATURE_SUPPORT
#include <string.h>
#include <stdint.h>
#include <stdlib.h>  // for rand()

#include "tile_config.h"
#include "tile_features.h"
#include "tile_lib.h"
#include "drivers/tile_gap_driver.h"
#include "drivers/tile_timer_driver.h"
#include "drivers/tile_button_driver.h"
#include "drivers/tile_random_driver.h"
#include "modules/tile_tdi_module.h"
#include "modules/tile_toa_module.h"
#include "modules/tile_tmd_module.h"
#include "modules/tile_tdg_module.h"
#include "tile_song_module.h"

#include "modules/tile_trm_module.h"

#include "tile_storage/tile_storage.h"
#include "tile_ble_service.h"
#include "tile_service.h"

#include "trace.h"
#include "gap_adv.h"
#include "app_timer.h"
#include "app_ble_tile_client.h"
#include "app_cfg.h"
#include "app_audio_policy.h"
#include "remote.h"
#include "app_main.h"
#include "app_relay.h"
#include "ftl.h"
#include "data_storage.h"
#include "tile_storage.h"
#include "app_mmi.h"

/*******************************************************************************
 * Global variables
 ******************************************************************************/

tile_ble_env_t tile_ble_env;
T_TILE_AUTHORIZE_STATE tile_authorize_state = TILE_AUTHORIZE_STATE_NONE;

/*******************************************************************************
 * Local variables
 ******************************************************************************/
static toa_channel_t tile_toa_channels[NUM_TOA_CHANNELS];
static uint8_t toa_queue_buffer[TOA_QUEUE_BUFFER_SIZE];

static uint8_t tile_app_timer_id = 0;
static uint8_t timer_idx_tile_timeout[TILE_MAX_TIMERS] = {0};

/*******************************************************************************
 * Forward declarations
 ******************************************************************************/
void advertising_update(void);
void trigger_software_reset(void);

/* gap module*/
static int tile_disconnect(void);

/* TRM module */
int start_trm_rssi_meas(void);
int stop_trm_rssi_meas(void);

/* timer module*/
static int tile_timer_start(uint8_t timer_id, uint32_t duration);
static int tile_timer_cancel(uint8_t timer_id);

/* random module*/
static int tile_random_bytes(uint8_t *dst, uint8_t len);

/* toa module*/
static int tile_send_toa_response(uint8_t *data, uint16_t len);
static int tile_associate(uint8_t *tile_id, uint8_t *tile_auth_key, uint8_t *authorization_type);

/* tmd module*/
int  tile_mode_set(uint8_t mode);
static int  tile_mode_get(uint8_t *mode);

/* tdg module*/
static int tile_get_diagnostics_cb(void);

/* button driver */
int         tile_read_button_state(uint8_t *button_state);

uint8_t realtek_bdaddr[6] = { 0x00 };

/*******************************************************************************
 * Defines & types
 ******************************************************************************/

/*******************************************************************************
 * Tile configuration structures
 ******************************************************************************/

/* gap register struct */
static struct tile_gap_driver gap_driver =
{
    .gap_disconnect         = tile_disconnect,
};

/* timer driver struct */
struct tile_timer_driver timer_driver =
{
    .start  = tile_timer_start,
    .cancel = tile_timer_cancel,
};

/* random driver struct  */
static struct tile_random_driver random_driver =
{
    .random_bytes  = tile_random_bytes,
};

/* device information struct */
struct tile_tdi_module tdi_module =
{
    .tile_id                  = tile_stored_id,
    .model_number             = tile_model_number,
    .hardware_version         = tile_hw_version,
    .bdaddr                   = realtek_bdaddr, // RAM Variable, Not stored in Flash
    .firmware_version         = TILE_FW_VERSION,
};

/* tile over the air struct  */
struct tile_toa_module toa_module =
{
    .tile_id                  = tile_stored_id,
    .auth_key                 = tile_stored_auth_key,
    .channels                 = tile_toa_channels,
    .queue                    = toa_queue_buffer,
    .queue_size               = TOA_QUEUE_BUFFER_SIZE,
    .num_channels             = NUM_TOA_CHANNELS,
    .mic_failure_count        = NULL,
    .auth_failure_count       = NULL,
    .channel_open_count       = NULL,
    .authenticate_count       = NULL,
    .tka_closed_channel_count = NULL,
    .send_response            = tile_send_toa_response,
    .associate                = tile_associate
};

/* tile mode struct */
struct tile_tmd_module tmd_module =
{
    .get  = tile_mode_get,
    .set  = tile_mode_set,
};

/* tile mode struct */
static struct tile_tdg_module tdg_module =
{
    .get_diagnostics = tile_get_diagnostics_cb,
};

/* Song module */
int rtk_play_indication(uint8_t number, uint8_t strength, uint8_t duration);
int rtk_play_indication_stop(void);

struct tile_song_module song_module =
{
    .play                 = rtk_play_indication, //PlaySong,
    .stop                 = rtk_play_indication_stop, //StopSong,
};

struct tile_trm_module trm_module =
{
    .start = start_trm_rssi_meas,
    .stop = stop_trm_rssi_meas,
};

/*******************************************************************************
 * Functions
 ******************************************************************************/

/***************************** TRM start/stop functions *******************************/
/**
 * @brief start TRM rssi measurements
 */

int start_trm_rssi_meas(void)
{
    ret_code_t err_code = 0;

    return err_code;
}

/**
 * @brief stop TRM rssi measurements
 */
int stop_trm_rssi_meas(void)
{
    ret_code_t err_code = 0;

    return err_code;
}

void tile_update_tdi_module_info(void)
{
    // This is for updating the bluetooth addresss after the stack is initialized.
    tile_tdi_register(&tdi_module);
    APP_PRINT_INFO0("tile_update_tdi_module_info");

    if (tile_get_current_mode() == TILE_MODE_ACTIVATED)
    {
        memset(temp_tile_addr, 0, TILE_ADDR_SIZE);
        ftl_load_from_storage(temp_tile_addr, TILE_LINK_BUD_ADDR,
                              sizeof(temp_tile_addr));
    }
}

void tile_features_init(void)
{
    APP_PRINT_INFO0("tile_features_init");

    /****************************************************************/
    /**** Minimum Features required for TileLib Interoperability ****/
    /****************************************************************/
    /* Initialize GAP driver */
    tile_gap_register(&gap_driver);

    /* Initialize timer driver */
    tile_timer_register(&timer_driver);

    /* Initialize random driver */
    tile_random_register(&random_driver);

    /* Initialize device information module */
    // Note that the device Bluetooth address is not available yet.
    // Send it to the tdi_module after GAP_INIT_STATE_STACK_READY.
    tile_tdi_register(&tdi_module);

    /* Initialize tile over the air module */
    tile_toa_register(&toa_module);

    /* Initialize tile mode module */
    tile_tmd_register(&tmd_module);

    /****************************************************************/
    /**** Additional Features ****/
    /****************************************************************/

    /* Initialize tile diagnbostics module */
    tile_tdg_register(&tdg_module);

    /* Initialize trm module */
    tile_trm_register(&trm_module);

    /* Initialize song module */
    tile_song_register(&song_module);

}


/*******************************************************************************
 * Callback functions for Tile Lib
 ******************************************************************************/

/***************************** gap module *******************************/
/**
 * @brief Disconnect current connection
 */
static int tile_disconnect(void)
{
    return TILE_ERROR_SUCCESS;
}

/************************************************************************/

/***************************** timer module *****************************/

/**
 * @brief Timer handler for Tile timers
 */
static void tile_timer_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    tile_timer_expired(timer_evt);
}

/**
 * @brief Start a Tile timer
 *
 * @param[in] timer_id   ID for timer, as specified by Tile Lib
 * @param[in] duration   Duration (in 1ms increments) for the timer
 */
static int tile_timer_start(uint8_t start_timer_id, uint32_t duration)
{
    if (duration < 1)
    {
        duration++;
    }

    // Start software timer
    /* The new timer takes priority, so stop any existing timer */
    app_stop_timer(&timer_idx_tile_timeout[start_timer_id]);

    app_start_timer(&timer_idx_tile_timeout[start_timer_id], "tile_timeout",
                    tile_app_timer_id, start_timer_id, 0, false,
                    duration);

    return TILE_ERROR_SUCCESS;
}

/**
 * @brief Cancel a Tile timer
 *
 * @param[in] timer_id    ID for timer, as specified by Tile Lib
 */
static int tile_timer_cancel(uint8_t stop_timer_id)
{
    app_stop_timer(&timer_idx_tile_timeout[stop_timer_id]);
    return TILE_ERROR_SUCCESS;
}

void tile_create_timers(void)
{
    app_timer_reg_cb(tile_timer_timeout_cb, &tile_app_timer_id);
}

/************************************************************************/

/****************************random module *******************************/
/**
 * @brief Generate some random bytes
 *
 * @param[out] dst    Destination address for the random bytes
 * @param[in]  len    Number of bytes requested
 */
static int tile_random_bytes(uint8_t *dst, uint8_t len)
{
    uint8_t num;

    for (num = 0; num < len; num++)
    {
        uint32_t tempval = rand();
        dst[num] = tempval & 0x000000FF;
    }

    return TILE_ERROR_SUCCESS;
}



/************************************************************************/

/***************************** toa module *******************************/

/**
 * @brief Send notification on a characteristic in TOA_RSP
 *
 * @param[in] data       Data to set attribute to.
 * @param[in] len        Length of data.
 */
static int tile_send_toa_response(uint8_t *data, uint16_t len)
{
    APP_PRINT_INFO1("tile_send_toa_response: tile_ble_env.conn_handle 0x%x", tile_ble_env.conn_handle);

    if (BLE_CONN_HANDLE_INVALID == tile_ble_env.conn_handle)
    {
        return TILE_ERROR_ILLEGAL_OPERATION;
    }

    tile_ble_service_send_v3_notify(tile_ble_env.conn_handle, data, len);

    return TILE_ERROR_SUCCESS;
};

/**
 * @brief Set the new Tile Id/Auth Key during Commissioning Process if in Manu/Shipping Modes
 *        Return SUCCESS for Non-Commissioning Cases
 */
static int tile_associate(uint8_t *tile_id, uint8_t *tile_auth_key, uint8_t *authorization_type)
{
    int retcode = TOA_RSP_ERROR_AUTHORIZATION;

    APP_PRINT_INFO2("tile_associate: tile_stored_activation_mode %d, tile_authorize_state %d",
                    tile_stored_activation_mode, tile_authorize_state);

    if (TILE_MODE_SHIPPING == tile_stored_activation_mode)
    {
        if (tile_authorize_state == TILE_AUTHORIZE_STATE_OK)
        {
            memcpy(tile_stored_id, tile_id,  sizeof(tile_stored_id));
            memcpy(tile_stored_auth_key, tile_auth_key, sizeof(tile_stored_auth_key));

            tile_authorize_state = TILE_AUTHORIZE_STATE_NONE;
            retcode = TOA_ERROR_OK;
        }
        else
        {
            tile_authorize_state = TILE_AUTHORIZE_STATE_ERROR;
            retcode = TOA_RSP_ERROR_AUTHORIZATION;
        }
    }
    else // Do not allow to set new Tile id/Auth key for an already Activated Tile. We should never come here ideally
    {
        retcode = TOA_RSP_SERVICE_UNAVAILABLE;
    }
    return retcode;
}

/************************************************************************/


/***************************** mode module *******************************/

/**
 * @brief Set the mode of the device.
 *
 * @param[in] mode  Mode, as specified by the TMD module.
 */
int tile_mode_set(uint8_t mode)
{
    APP_PRINT_TRACE2("tile_mode_set: mode %d, tile_stored_activation_mode %d", mode,
                     tile_stored_activation_mode);

    if (TILE_MODE_ACTIVATED != mode)
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

    tile_set_current_mode(mode);
    tile_store_app_data();
    tile_store_addr();

    //  update_advertising_info();
    app_tile_update_adv_content(mode);

    return TILE_ERROR_SUCCESS;
}

/**
 * @brief Get the current mode of the device.
 *
 * @param[out] mode  Mode, as specified by the TMD module.
 */
static int tile_mode_get(uint8_t *mode)
{
    *mode = tile_stored_activation_mode;

    return TILE_ERROR_SUCCESS;
}

/***************************** tdg module *******************************/
static int tile_get_diagnostics_cb(void)
{
    uint8_t version = DIAGNOSTIC_VERSION;

    tdg_add_data(&version, 1);
    tdg_add_data(&tile_stored_activation_mode, 1);

    // Reset Counters


    tdg_finish();

    return TILE_ERROR_SUCCESS;
}

static int rtk_play_indication(uint8_t number, uint8_t strength, uint8_t duration)
{
    APP_PRINT_INFO3("rtk_play_indication %d,%d,%d", number,
                    strength,
                    duration);
    if (number == TILE_SONG_FIND)
    {
        if (app_db.avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_mmi_handle_action(MMI_AV_PLAY_PAUSE);
            }
            else
            {
                uint8_t action = MMI_AV_PLAY_PAUSE;
                remote_async_msg_relay(app_db.relay_handle, APP_REMOTE_MSG_MMI_SYNC, &action, 1, false);
            }
        }
        app_audio_tone_play(app_cfg_const.tone_gfps_findme, false, false);
    }
    return NULL;
}

static int rtk_play_indication_stop(void)
{
    APP_PRINT_INFO0("rtk_play_indication_stop");
    return NULL;
}

void app_tile_init(void)
{
    APP_PRINT_TRACE0("app_tile_init");

    tile_service_init();

    gap_get_param(GAP_PARAM_BD_ADDR, realtek_bdaddr);

    tile_update_tdi_module_info();

    app_tile_adv_init();
}
#endif
