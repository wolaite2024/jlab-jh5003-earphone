/*
 * NOTICE
 *
 * 2017 Tile Inc.  All Rights Reserved.

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

/** @file tile_gap_driver.h
 ** @brief Tile GAP Driver interface. Provides TileLib Control over GAP functions, like connection,
 **        disconnection and connection parameters.
 */

#ifndef TILE_GAP_DRIVER_H_
#define TILE_GAP_DRIVER_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Connection parameters.
 */
struct tile_conn_params
{
    uint16_t conn_interval;
    uint16_t slave_latency;
    uint16_t conn_sup_timeout;
};


struct tile_gap_driver
{
    /**
     * Time in 10ms increments before Tile disconnects if no client has
     * authenticated. A value of 0 indicates that this feature is disabled.
     * The value may be updated at any time, but will not clear a timer which
     * is already running. The value is used after a connection is established.
     */
    uint16_t authentication_timer_delay;

    /**
     * Memory space for current connection parameters.
     */
    struct tile_conn_params conn_params;

    /**
    * Diagnostic info: counts the number of disconnections triggered by Auth Timer.
     */
    uint16_t *auth_disconnect_count;

    /**
     * Disconnect from the currently connected device.
     *
     * @return See @ref TILE_ERROR_CODES.
     */
    int (*gap_disconnect)(void);
};


/***************************************************************************************
 * @brief Register GAP Driver.
 *
 * @param[in]  driver GAP driver struct.
 *
 * @return See @ref TILE_ERROR_CODES.
 *
 ****************************************************************************************
 */
int tile_gap_register(struct tile_gap_driver *driver);


/***************************************************************************************
 * @brief Notify TileLib of a connection.
 *
 * @param[in]  conn_params Connection Parameters struct.
 *
 * @return See @ref TILE_ERROR_CODES.
 *
 ****************************************************************************************
 */
int tile_gap_connected(struct tile_conn_params *conn_params);

/***************************************************************************************
 * @brief Notify TileLib of a disconnection.
 *
 * @return See @ref TILE_ERROR_CODES.
 *
 ****************************************************************************************
 */
int tile_gap_disconnected(void);

/***************************************************************************************
 * @brief Notify TileLib of updated connection params.
 *
 * @param[in]  conn_params Connection Parameters struct.
 *
 * @return See @ref TILE_ERROR_CODES.
 *
 ****************************************************************************************
 */
int tile_gap_params_updated(struct tile_conn_params *conn_params);


#ifdef __cplusplus
}
#endif

#endif // TILE_GAP_DRIVER_H_
