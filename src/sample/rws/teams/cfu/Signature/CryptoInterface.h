//****************************************************************************
//
// File:         CryptoInterface.h
//
// Description:  This module sets up the call to the ECC Crypto Library
//
// This module is customized according to the memory model of the processor in
// order to get the linker to properly populate the PKI Info block.
//
// Copyright (C) Microsoft Corporation. All Rights Reserved.
//
//*****************************************************************************

#ifndef __CRYPTO_INTERFACE_H__
#define __CRYPTO_INTERFACE_H__

#ifdef __cplusplus
extern "C"
{
#endif

// Milestones that a component can support. It's a reasonable number but can be changed if needed, even per-build.
// It is a balance between array size and project requirements. Can be set to 1 if milestones are
// not supported, but at least one is required.
#define CFU_MILESTONE_COUNT      2          // Support 2 component milestones.

#define SIGNATURE_TYPE          1// 0: unsigned 1: test, 2: attestation 3: production

#include "CfuInfoStructs.h"   // CFU block definition
#include "HashStructs.h"      // Hash context
//*****************************************************************************
//
//                   PRODUCT CUSTOMIZATIONS
//
//*****************************************************************************

//****************************************************************************
//
//                          PUBLIC FUNCTION PROTOTYPES
//
//****************************************************************************
// Hash functions
void CfuHashInit(PHASH_CONTEXT pContext);
void CfuHashUpdate(PHASH_CONTEXT pContext, const uint8_t *pPartIn, uint32_t partInLen);
void CfuHashFinalize(PHASH_CONTEXT pContext, uint8_t digest[HASH_LEN]);

// Verifies a precomputed hash.
uint32_t VerifyHash(uint8_t *hashBuffer, uint32_t hashBufferSize, uint8_t *pSignature);

#ifdef __cplusplus
}
#endif

#endif // __CRYPTO_INTERFACE_H__
