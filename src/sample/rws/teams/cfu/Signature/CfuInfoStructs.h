/***************************************************************************//**
 * \file CfuInfoStructs.h
 * Description:
 *
 * \copyright Copyright (c) Microsoft. All rights reserved.
 ******************************************************************************/

#ifndef CFUINFOSTRUCTS_H_
#define CFUINFOSTRUCTS_H_

#include <stdint.h>

// GENERAL NOTES in PKI V4
// Segments are deprecated.  All downloads are now streamed - hashes and CRC are calculated as the image arrives.
// PrepFunction is deprecated.
// An Extension field is added for minor future-proofing.
// HW Build IDs are renamed Milestones.
// Pointers to roots are not needed in the CFU structs. The linker may place them anywhare and code accesses them directly.

//****************************************************************************
//
//                                  DEFINES
//
//****************************************************************************
// CFU protocol version tracking:
#define CFU_VERS_4              0x4U    // version 4 of the pkiInfo block

//#define PKI_OPTIONS_MCU       (0U)    // ***DEPRECATED*** bit 0 clear indicates MCU image
//#define PKI_OPTIONS_CFU       (1U)    // ***DEPRECATED*** bit 0 set indicates subcomponent image
#define CFU_OPTIONS_NORESET     (0U)    // Do not reset target component after update
#define CFU_OPTIONS_RESET       (1U)    // Reset target component after update
#define CFU_OPTIONS_AUTH_CRC16  (0U)    // Image carries a CRC16
#define CFU_OPTIONS_AUTH_CRC32  (1U)    // Image carries a CRC32
#define CFU_OPTIONS_AUTH_PKI    (2U)    // Image is signed, use CFU_PKI info
#define CFU_OPTIONS_FILL_NONE   (0U)    // Hashed/CRC data may NOT have gaps. Gaps will fail at validation or sooner.
#define CFU_OPTIONS_FILL_SKIP   (1U)    // Hashed/CRC data may have gaps.  Do not fill. Hash/CRC only the bytes received.
#define CFU_OPTIONS_FILL_00     (2U)    // Hashed/CRC data may have gaps.  Assume gaps have 00 filler.
#define CFU_OPTIONS_FILL_FF     (3U)    // Hashed/CRC data may have gaps.  Assume gaps have  FF filler.

// Struct IDs are used to identify each of the structs below, which when combined with struct versioning permits
// minor updates to the standard without reversioning the entire architecture.  With these attributes, along with
// per-struct versioning, unknown structs can be skipped and backward compatibility can be maintained.

#define CFU_STRUCT_HEADER       (1U)
#define CFU_STRUCT_INFO         (2U)
#define CFU_STRUCT_HW_COMPAT    (3U)
#define CFU_STRUCT_PKI          (4U)

//****************************************************************************
//
//                                  TYPEDEFS
//
//****************************************************************************

// This struct is not signed.  All values must be sanitized before use.
#define CFU_STRUCT_HEADER_VER   (1U)
typedef struct
{
    uint32_t size;                              // Size of this struct
    uint32_t structID;                          // ID of this struct == CFU_STRUCT_HEADER
    uint32_t version;                           // Version of this struct
    uint32_t cfuInfoOffset;                     // Offset of the CFU Info struct, also start of protected data block.
    uint32_t cfuLen;                            // Length of the CFU metadata, effectively the start of the target data block.
} CFU_HEADER, *PCFU_HEADER;

// This is the main info block.  It, along with HW Compat and PKI (if signed) must precede the signed data so that the
// stream(s) can be properly identified and routed.
#define CFU_STRUCT_INFO_VER             (1U)
typedef struct
{
    uint32_t size;                              // Size of this struct
    uint32_t structID;                          // ID of this struct == CFU_STRUCT_INFO
    uint32_t version;                           // Version of this struct
    uint32_t cfuVersion;                        // Version of the CFU Protocol
    uint32_t platformID;                        // Platform ID of the update
    uint32_t componentID;                       // Component ID of the update
    uint32_t fwVersion;                         // Standard firmware version
    uint32_t privFWVer;                         // Private firmware version, component-specific, (optional, null if unused)
    uint32_t hwCompatOffset;                    // Array of hw milestones and variants supported, in the address space of the payload.
    struct
    {
uint32_t reserved           :
        1;       // Formerly identified MCU vs Subcomponent updates. Deprecated
uint32_t resetAfterFwUpdate :
        1;       // Set this bit to indicate that a reset is required after FW update is complete
        uint32_t auth               :  2;       // How to authenticate the image.
        uint32_t fill               :  2;       // How to handle gaps in the image.
        uint32_t padding            : 26;       // For future use...
    } OPTIONS;                                  // Bitmap of options.
    uint32_t extension;                         // For future use... (optional, 0 if unused)
    union
    {
        uint32_t cfuCRCOffset;                  // Offset of the CRC, in the address space of the payload.
        uint32_t cfuHashOffset;                 // Offset of a raw hash, in the address space of the payload.
        uint32_t cfuPKIOffset;                  // Offset of the CFU_PKI struct, in the address space of the payload.
    } AUTH;
} CFU_INFO, *PCFU_INFO;

// This describes which milestone/variant a particular range of the incoming stream is targeted for, where in the stream it is, and
// where it belongs on the target.  Each entry requries a line in the offer file.  The MCU must read this array before the main
// stream begins to know which part of the stream to forward, and to where in the component.
// Each instance of a component is a specific hardware milestone and variant.  It is required that an update uniquely match only one
// milestone/variant for a component, i.e., a specific component must only match one line in the offer list.

// Because most addresses referenced are offsets into the image and not into any acutal processor address space, they are given as
// integers.  They should be cast when the structures are resident in memory after adding the address of the top of the payload.

// The size, version and struct ID of a milestone are tied to the CFU_HW_COMPAT structure.
typedef struct
{
    uint32_t hwMilestone;                       // Identifies a HW milestone build that is supported. Single value 0x0-0xF.
    uint32_t hwVariantsMask;                    // Bitmap of variants this image supports;  bit 0 corresponds to Variant 0. 0xFFFF FFFF if unused.
    uint32_t sourceOffset;                      // Offset of the data to be delivered in the address space of the payload.
    uint32_t destAddress;                       // Address of the data to be delivered in the address space of the component.
    uint32_t length;                            // Length of the data to be delivered to the component at the destAddress.
} CFU_MILESTONE, *PCFU_MILESTONE;

// This contains the array of Milestone structs.  A single signed package may contain one image for several milestone/varaiants, or separate images
// for different milestone/variants.  If the latter, the entire package must still be downloaded to authenticate, but parts of the stream
// intended for a different milestone/variant can be discarded after updating the hash/CRC.
// Each milestone/variant has a corresponding line in the offer file.
#define CFU_STRUCT_HW_COMPAT_VER                (1U)
typedef struct
{
    uint32_t      size;                         // Size of this struct
    uint32_t      structID;                     // ID of this struct CFU_STRUCT_HW_COMPAT
    uint32_t      version;                      // Version of this struct
    uint32_t      count;                        // = CFU_MILESTONE_COUNT // Number of milestones
    CFU_MILESTONE
    milestone[2];                 // Details of the particular milestone supported, size defined in CryptoInterface
} CFU_HW_COMPAT, *PCFU_HW_COMPAT;

// The PKI struct is only present if the download is signed.
// We dont need the old Root pointers, they are not used on incoming inspections.  They can be linked in anywhere in a verifying MCU and used
// like any other code.
#define CFU_STRUCT_PKI_VER                      (1U)
typedef struct
{
    uint32_t size;                              // Size of this struct
    uint32_t structID;                          // ID of this struct == CFU_STRUCT_PKI
    uint32_t version;                           // Version of this struct
    uint32_t hashAlgorithm;                     // ID of hash algorithm used to sign
    uint32_t signingCipher;                     // ID of cipher used to sign
    uint32_t signatureOffset;                   // If .signingCipher = PKI_CIPHER_NONE, signatureOffset points to the raw hash.
    // For all other values of .signingCipher, signatureOffset points to the PKCS#7 Detached Signature in the CFU file.
} CFU_PKI, *PCFU_PKI;

#endif /* CFUINFOSTRUCTS_H_ */
