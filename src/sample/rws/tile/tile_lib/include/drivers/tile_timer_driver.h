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

/** @file tile_timer_driver.h
 ** @brief Tile Timer Driver interface. Provides TileLib an interface to use timers.
 */

#ifndef TILE_TIMER_DRIVER_H_
#define TILE_TIMER_DRIVER_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Coversion factor for seconds. Delays are specified in seconds. So multiply by 1000
 * To get milliseconds, for starting Realtek timers
 */
#define TILE_TICKS_PER_SEC ((uint32_t)1000)

/**
 * IDs to associate with each Tile timer.
 */
enum TILE_TIMER_IDS
{
    TILE_AUTHENTICATION_TIMER,
    TILE_TKA_TIMER1,
    TILE_TKA_TIMER2,
    TILE_TKA_TIMER3,
    TILE_MAX_TIMERS /* < Number of timers used by Tile Lib. */
};

struct tile_timer_driver
{
    /**
     * Start a timer. duration is in 10ms increments.
     */
    int (*start)(uint8_t timer_id, uint32_t duration);

    /**
     * Cancel a timer.
     */
    int (*cancel)(uint8_t timer_id);
};


/**
 * Timer registration function.
 */
int tile_timer_register(struct tile_timer_driver *driver);


/**
 * Call when a Tile timer has expired.
 */
int tile_timer_expired(uint8_t timer_id);

#ifdef __cplusplus
}
#endif

#endif // TILE_TIMER_DRIVER_H_
