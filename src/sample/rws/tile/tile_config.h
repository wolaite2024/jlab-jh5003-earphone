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

/** @file tile_config.h
 ** @brief Configuration for Tile functionality
 */

//#include "boards.h"

#ifndef TILE_CONFIG_H_
#define TILE_CONFIG_H_

#define TILE_FW_VERSION "38.03.01.0"

// Iterim pair for platform example, assigned to RTL8763BO
#define INTERIM_TILE_ID      {0x07, 0xd7, 0xfe, 0x3e, 0xf2, 0xb4, 0xd9, 0x97}
#define INTERIM_AUTH_KEY     {0xef, 0x70, 0x69, 0xa6, 0xfe, 0xf0, 0x62, 0xe5, 0x20, 0x84, 0xee, 0x70, 0xed, 0x8a, 0xea, 0x56}

#define GO_AIR_LEFT_TILE_ID  {0x67, 0x4b, 0xbe, 0x4b, 0x81, 0xe3, 0x5e, 0xe0}
#define GO_AIR_LEFT_TILE_KEY {0x77, 0x68, 0xbe, 0x7a, 0x79, 0x22, 0x3d, 0x40, 0xd3, 0x18, 0x7a, 0xf9, 0x11, 0x4e, 0xfa, 0x84}

#define GO_AIR_RIGHT_TILE_ID  {0x3a, 0xd5, 0xa1, 0xb8, 0x52, 0xae, 0xde, 0x4d}
#define GO_AIR_RIGHT_TILE_KEY {0xbc, 0x82, 0x3e, 0xa5, 0xdf, 0x25, 0x12, 0x8a, 0xb2, 0xd1, 0x6e, 0x94, 0xc8, 0x9a, 0xf2, 0x88}

/**
 * @brief Number of TOA channels supported
 */
#define NUM_TOA_CHANNELS             3

/**
 * @brief Diagnostic Version
 */
#define DIAGNOSTIC_VERSION           82

/**
 * @brief Size of the TOA message buffer
 */
#define TOA_QUEUE_BUFFER_SIZE        (100 + 40 * (NUM_TOA_CHANNELS - 1))

#endif
