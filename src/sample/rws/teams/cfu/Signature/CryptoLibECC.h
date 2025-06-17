/*++

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
    THE POSSIBILITY OF SUCH DAMAGE.

    Copyright (C) Microsoft Corporation. All Rights Reserved.

Module Name:

    CryptoLibECC.h

Abstract:

    This file contains thepublic interface for the CryptoLibECC library for
    performing signature verification on ECC signatures for firmware and
    other cryptographic challenges.

Environment:

    Windows x86 for test vectors only.  Windows products should use Windows APIs for these
    functions, and to validate this library.

    Microcontrollers for product use that do not have access to other Crypto-Board-Approved
    ECC functions or hardware.  Architectures are x86 for Windows, ARM and ARC for MCUs.
    Toolchains are MSVC, IAR EWARM, KEIL MDK5 and Synopsis MetaWare

--*/
#ifndef _CRYPTOLIBECC_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "HashStructs.h"

#define _CRYPTOLIBECC_H_

#define MAX_CERTS_CNT           5
#define MAX_OID_BYTELEN         0x7F
#define MAX_CHAIN_DEPTH         5
#define MAX_EKU_CNT             5
#define MAX_EXT_CNT             30

// Enumerate every hash algorithm we have ever used.  Never remove an item.
typedef enum
{
    PKI_HASH_SHA256 = 1
} PKI_HASH;

// Enumerate every cipher we have ever used.  Never remove an item.
typedef enum
{
    PKI_CIPHER_NONE       = 0,      // Hash only.
    PKI_CIPHER_RSA_2K     = 1,      // RSA 2048-bit key
    PKI_CIPHER_ECDSA_P256 = 2       // ECDSA using P256 curve
} PKI_CIPHER;

typedef uint32_t                                          CERTIFICATES_STATUS;

#define CERT_STATUS_SUCCESS                             ((CERTIFICATES_STATUS)0x0)
#define CERT_STATUS_CERTIFICATE_VERIFICATION_FAILED     ((CERTIFICATES_STATUS)0x1)
#define CERT_STATUS_UNSUPPORTED_SIGNATURE_ALGORITHM     ((CERTIFICATES_STATUS)0x2)
#define CERT_STATUS_UNSUPPORTED_HASH_ALGORITHM          ((CERTIFICATES_STATUS)0x3)
#define CERT_STATUS_BAD_ASN1_PKCS7_CERTS                ((CERTIFICATES_STATUS)0x4)
#define CERT_STATUS_BAD_ASN1_PKCS7_TRUSTEDROOTS         ((CERTIFICATES_STATUS)0x5)
#define CERT_STATUS_SIGNATURE_VERIFICATION_FAILED       ((CERTIFICATES_STATUS)0x6)
#define CERT_STATUS_NO_MEMORY                           ((CERTIFICATES_STATUS)0x7)
#define CERT_STATUS_INVALID_ARGUMENT                    ((CERTIFICATES_STATUS)0x8)
#define CERT_STATUS_CERTIFICATE_EXPIRED                 ((CERTIFICATES_STATUS)0x9)
#define CERT_STATUS_INTERNAL_ERROR                      ((CERTIFICATES_STATUS)0xA)
#define CERT_STATUS_UNSUPPORTED_AUTH_ATTRIBUTES         ((CERTIFICATES_STATUS)0xB)

// Verify structs
typedef struct
{
    PKI_CIPHER CipherAlgorithm;
    PKI_HASH   HashAlgorithm;
    uint8_t   *pPKCS7RootofTrust;
    uint8_t  **pszRequiredEKUs;
    uint32_t   cRequiredEKUs;
    uint8_t   *pPKCS7Signature;
    void      *pSignedBinary;
    uint32_t   cbSignedBinarySize;
} VERIFY_IMAGE_REQUEST, *PVERIFY_IMAGE_REQUEST;

typedef struct
{
    PKI_CIPHER CipherAlgorithm;
    PKI_HASH   HashAlgorithm;
    uint8_t   *pPKCS7RootofTrust;
    uint8_t  **ppSzRequiredEKUs;
    uint32_t   cRequiredEKUs;
    uint8_t   *pPKCS7Signature;
    HASH_DATA  Hash;
} VERIFY_HASH_REQUEST, *PVERIFY_HASH_REQUEST;

// Utility functions
uint64_t CryptoLibVersion(void);

// Hash functions
void HashInit(PHASH_CONTEXT context);
void HashUpdate(PHASH_CONTEXT context, const uint8_t *partIn, uint32_t partInLen);
void HashFinalize(PHASH_CONTEXT context, uint8_t digest[HASH_LEN]);

// Verify functions
CERTIFICATES_STATUS CryptoLibECC_VerifyImage(PVERIFY_IMAGE_REQUEST pRequest);
CERTIFICATES_STATUS CryptoLibECC_VerifyHash(PVERIFY_HASH_REQUEST pHashedRequest);

#ifdef __cplusplus
}
#endif

#endif // _CRYPTOLIBECC_H_
