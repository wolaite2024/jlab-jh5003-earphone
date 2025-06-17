/***************************************************************************//**
 * \file HashStructs.h
 * Description:
 *
 * \copyright Copyright (c) Microsoft. All rights reserved.
 ******************************************************************************/
// These are the types and structs necessary to keep a hashing context.

#ifndef HASHSTRUCTS_H_
#define HASHSTRUCTS_H_

#include <stdint.h>

#define HASH_LEN    32

// Hash structs
typedef struct _HASH_DATA_BLOB
{
    uint8_t *pbData;
    uint32_t cbData;
} HASH_DATA, *PHASH_DATA;

typedef struct
{
    union
    {
        uint32_t state[8];
    } STATE;
    uint32_t count[2];
    uint8_t  buffer[HASH_LEN * 2];
} HASH_CONTEXT, *PHASH_CONTEXT;

#endif // HASHSTRUCTS_H_
