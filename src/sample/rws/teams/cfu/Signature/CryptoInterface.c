//****************************************************************************
// File:         CryptoInterface.c
// Description:  This module performs the following:
//               1) Verifies the SHA256/RSA 2048 digital signature on
//                  a firmware image.
//               2) Verifies the certificate chain of the signing certificate.
// Copyright (C) Mivcrosoft Corporation. All Rights Reserved.
//*****************************************************************************

//****************************************************************************
//
//                               INCLUDES
//
//****************************************************************************
#if F_APP_TEAMS_CFU_SUPPORT
#include <stdint.h>             // Standard types.
#include "CryptoInterface.h"    // Our header file.
#include "HashStructs.h"        // For Hash contexts
#include "CryptoLibECC.h"       // Lib functions
#include "CfuInfoStructs.h"     // For the linker-populated pointers we need
#include "trace.h"
#include "app_common_cfu.h"
//****************************************************************************
//
//                                  DEFINES
//
//****************************************************************************
#define BUILD_TYPE_FROM_FW_VERSION(X)   ((X >> 4) & 0x7)

//****************************************************************************
//
//                                  EXTERNS
//
//****************************************************************************
extern uint8_t pTestRoot;               // CA of the test signing cert.
extern uint8_t pProductionRoot;         // CA of the production signing cert.

//****************************************************************************
//
//                                  TYPEDEFS
//
//****************************************************************************

//****************************************************************************
//
//                              STATIC VARIABLES
//
//****************************************************************************
// Keys can have Extended Key Usages (EKUs) applied to them to indicate what
// they are authorized to do.
//
// Any cert that signs FW must carry one of these EKUs and be signed by
// the cert contained in the corresponding root .p7b file.
//
// Microsoft:
// {iso(1) identified-organization(3) dod(6) internet(1) private(4) enterprise(1) 311}
//
// Each team is assigned the last three numbers// in the EKU OIDs by MSFT PKI team.
// Eventually should be listed at http://support.microsoft.com/kb/287547

// Number of EKUs to try to find on a given leaf.  if any of the
// provided EKUs are found, the EKU check passes.
#define REQ_EKU_CNT             1 // Only 1 provided per check, prod or test

// Firmware ECDSA Production Signing EKU OID
static const uint8_t s_prodEKU[] = "1.3.6.1.4.1.311.76.9.21.1";
// Firmware ECDSA Test Signing EKU OID
static const uint8_t s_testEKU[] = "1.3.6.1.4.1.311.76.9.21.1";
// Firmware image build type
//static uint32_t s_buildType = FW_VER_BUILD_RELEASE_MSFT_SIGNED7;

//****************************************************************************
//
//                              GLOBAL VARIABLES
//
//****************************************************************************


//****************************************************************************
//
//                          PUBLIC FUNCTION DEFINITIONS
//
//****************************************************************************

//****************************************************************************
// CfuHashInit()
//  This function initializes a passed-in Hash Context.
// Input Parameters
//      pContext: Pointer to an opaque blob that keeps the current running hash.
//*****************************************************************************
void CfuHashInit(PHASH_CONTEXT pContext)
{
    HashInit(pContext);
    //s_buildType = BUILD_TYPE_FROM_FW_VERSION(FW_CFU_VERSION);
}

//****************************************************************************
//
// CfuHashUpdate()
//
//  This function updates a passed-in Hash Context with additional bytes
//
//
// Input Parameters
//      pContext: Pointer to an opaque blob that keeps the current running hash.
//      pPartIn:  Pointer to bytes to add to the current running hash.
//      partInLen: Number of bytes to add to the current running hash.
//
//*****************************************************************************
void CfuHashUpdate(PHASH_CONTEXT pContext, const uint8_t *pPartIn, uint32_t partInLen)
{
    HashUpdate(pContext, pPartIn, partInLen);
}

//****************************************************************************
// CfuHashFinalize()
//      This function finishes a current hash and returns the final hash array.
// Input Parameters
//      pContext: Pointer to an opaque blob that keeps the current running hash.
// Output Parameters:
//      digest: Byte array of the hash
//*****************************************************************************
void CfuHashFinalize(PHASH_CONTEXT pContext, uint8_t digest[HASH_LEN])
{
    HashFinalize(pContext, digest);
}

//****************************************************************************
// VerifyHash()
//      This function performs the following:
//      1) Attempts to verify using the Production root and EKU(s)
//      2) If that fails, and _ACCEPT_TEST_CERT is set, retries using the
//         Test root and EKU(s)
// Input Parameters
//      hashbuffer: a precomputed hash to verify against the hash in the signature.
//      hashbuffersize: size of  hashbuffer.  Must match the algorithm expectation.
//      pCfuHeader: a pointer to the top of a CFU image.
//*****************************************************************************
uint32_t VerifyHash(uint8_t *hashBuffer, uint32_t hashBufferSize, uint8_t *pSignature)
{
    // Note that CryptoLibECC_VerifyHash can be used to verify ANY ECDSA hash, such
    // as an unlock challenge, as long as you set the right EKU and you set the signature
    // pointer to the challenge response p7b. This is an example of how to do this
    // using the firmware image as the hashed item.
    CERTIFICATES_STATUS status = CERT_STATUS_SUCCESS;
    uint8_t *pSzEKUs[REQ_EKU_CNT] = {0};  // Array of required OID pointers

    // Make sure we have the correct size hash.
    if (hashBufferSize != HASH_LEN)
    {
        //AppTraceLog("VerifyHash error: CERT_STATUS_UNSUPPORTED_HASH_ALGORITHM!\r\n");
        return CERT_STATUS_UNSUPPORTED_HASH_ALGORITHM;
    }

    // Initialize the Hash Request fields that dont vary by cert.
    VERIFY_HASH_REQUEST verifyHashRequest =
    {
        .CipherAlgorithm   = PKI_CIPHER_ECDSA_P256,
        .HashAlgorithm     = PKI_HASH_SHA256,
        .pPKCS7Signature   = pSignature,
        .ppSzRequiredEKUs  = (uint8_t **)pSzEKUs,
        .cRequiredEKUs     = REQ_EKU_CNT,
        .Hash.pbData       = hashBuffer,
        .Hash.cbData       = hashBufferSize
    };

    // Run the hash with the Production cert.
    if (ms_cfu->signing == 1)
    {
        verifyHashRequest.pPKCS7RootofTrust = &pTestRoot;
        verifyHashRequest.ppSzRequiredEKUs[0] = (uint8_t *)&s_testEKU[0];
    }
    else if (ms_cfu->signing == 3)
    {
        verifyHashRequest.pPKCS7RootofTrust = &pProductionRoot;
        verifyHashRequest.ppSzRequiredEKUs[0] = (uint8_t *)&s_prodEKU[0];
    }

    status = CryptoLibECC_VerifyHash(&verifyHashRequest);

    return (uint32_t)status;
}
#endif
