
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

#ifndef TILE_LIB_H_
#define TILE_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef uint32_t ret_code_t;

/**
 * @brief Tile Service UUIDs.
 * These are 16-bit UUIDs.
 */
#define TILE_SHIPPING_UUID    0xFEEC               /** Advertised by Tiles in Shipping Mode. */
#define TILE_ACTIVATED_UUID   0xFEED               /** Advertised by Tiles in Activated Mode. */

#define TILE_TOA_CMD_UUID     { 0xC0, 0x91, 0xC4, 0x8D, 0xBD, 0xE7, 0x60, 0xBA, 0xDD, 0xF4, 0xD6, 0x35, 0x18, 0x00, 0x41, 0x9D }
#define TILE_TOA_RSP_UUID     { 0xC0, 0x91, 0xC4, 0x8D, 0xBD, 0xE7, 0x60, 0xBA, 0xDD, 0xF4, 0xD6, 0x35, 0x19, 0x00, 0x41, 0x9D }

/**
 * TOA Command and Response characteristics lengths in octets.
 */
#define TILE_TOA_CMD_CHAR_LEN 20
#define TILE_TOA_RSP_CHAR_LEN 20

/**
 * Length, in bytes, of the Tile ID.
 */
#define TILE_ID_LEN                 8

/**
 * Length, in bytes, of the Tile authentication key.
 */
#define TILE_AUTH_KEY_LEN           16


/**
 * Length of the Tile firmware version string.
 */
#define TILE_FIRMWARE_VERSION_LEN   10

/**
 * Length of the Tile model number string.
 */
#define TILE_MODEL_NUMBER_LEN       10

/**
 * Length of the Tile hardware version string.
 */
#define TILE_HARDWARE_VERSION_LEN   5

/**
 * Length of the Tile BDADDR.
 */
#define TILE_BDADDR_LEN             6

/**
 * Length of the Tile Serial Number.
 */
#define TILE_SERIAL_NUM_LEN         (TOA_MPS-1)

/**
 * Error codes returned by Tile Lib functions
 */
enum TILE_ERROR_CODES
{
    TILE_ERROR_SUCCESS = 0,
    TILE_ERROR_NOT_INITIALIZED,
    TILE_ERROR_ILLEGAL_SERVICE,
    TILE_ERROR_ILLEGAL_PARAM,
    TILE_ERROR_ILLEGAL_OPERATION,
    TILE_ERROR_BUFFER_TOO_SMALL,
    TILE_ERROR_TERMINAL,
    TILE_ERROR_REENTRANCY,
    TILE_ERROR_NUM_TOA_CHANNELS,
};

#ifdef __cplusplus
}
#endif

#endif // TILE_LIB_H_
