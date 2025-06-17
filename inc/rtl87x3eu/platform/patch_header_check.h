/**
  ******************************************************************************
  * @file    patch_header_check.h
  * @author
  * @version V0.0.1
  * @date    2017-09-01
  * @brief   This file contains all the functions regarding patch header check.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef _PATCH_HEADER_CHECK_H_
#define _PATCH_HEADER_CHECK_H_
#include <stdbool.h>    //for bool
#include <stdint.h>   //for uint* type
#include "image_ext_header.h"
/** @addtogroup  FLASH_DEVICE    Flash Device
    * @{
    */
/*============================================================================*
  *                                   Constants
  *============================================================================*/
/** @defgroup FLASH_DEVICE_Exported_Constants Flash Device Exported Constants
  * @brief
  * @{
  */

/** @defgroup FLASH_IMAGE_SIGNATURE Flash Image Signature
  * @brief patch signature definition for each image
  * @{
  */


/** End of FLASH_IMAGE_SIGNATURE
  * @}
  */
#define DEFAULT_HEADER_SIZE                 0x400
#define EFUSE_HEADER_SIZE_UNIT              0x400

#define OUT_OF_CMAC_RANGE                   (offsetof(T_IMG_HEADER_FORMAT, auth) + offsetof(T_AUTH_HEADER_FORMAT, image_signature))
#define OUT_OF_SIGN_RANGE                   (offsetof(T_IMG_HEADER_FORMAT, auth) + offsetof(T_AUTH_HEADER_FORMAT, image_hash))
#define OUT_OF_HASH_RANGE                   (offsetof(T_IMG_HEADER_FORMAT, auth) + offsetof(T_AUTH_HEADER_FORMAT, PubKey))
#define SIGNATURE_BYTE_SIZE                 64
#define CMAC_BYTE_SIZE                      16
#define SHA256_BYTE_SIZE                    32
#define AUTH_HEADER_SIZE                    (SHA256_BYTE_SIZE + SIGNATURE_BYTE_SIZE + CMAC_BYTE_SIZE)

#define RSA_PUBLIC_KEY_BYTE_SIZE            (256)

#define ECDSA_PUBLIC_KEY_BYTE_SIZE          (32)

#define ECDSA_UNCOMPRESSED_PUBLIC_KEY_LEN   (2 * ECDSA_PUBLIC_KEY_BYTE_SIZE + 1)

#define UUID_SIZE                           16
#define SYS_CFG_SIGNATURE                   0x12345bb3

#define UPPERSTACK_PARTITION_NAME           "UPPER_STACK"
#define VP_PARTITION_NAME                   "VP"
#define TONE_PARTITION_NAME                 "TONE"

#define IMG_HDR_RSVD_SIZE        (348)
#define GOLDEN_PATTERN_WORD_LEN  (20)
#define GOLDEN_PATTERN_SIZE      (sizeof(uint32_t) * GOLDEN_PATTERN_WORD_LEN)
#define IMG_FEATURE_STR_LEN      (16)
#define IMG_INFO_SIZE            (sizeof(uint32_t) * (IMAGE_MAX - IMG_SBL) * 2)
#define VER_VAL_SIZE             (sizeof(uint32_t))
#define IMG_RELEASE_VER_SIZE     (sizeof(uint64_t))
#define IMG_RSVD0_SIZE           (IMG_HDR_RSVD_SIZE - VER_VAL_SIZE - IMG_INFO_SIZE - GOLDEN_PATTERN_SIZE - \
                                  IMG_FEATURE_STR_LEN - IMG_RELEASE_VER_SIZE)

#define DEV_ID_VALID_SIZE        (sizeof(uint8_t))
#define TOOL_RELEASE_VER_SIZE (6)
#define IMG_RSVD1_SIZE     (IMG_HDR_RSVD_SIZE - TOOL_RELEASE_VER_SIZE - DEV_ID_VALID_SIZE - IMG_FEATURE_STR_LEN - IMG_RELEASE_VER_SIZE)

#define NAND_IMG_INFO_SIZE       (sizeof(uint32_t) * (IMAGE_MAX - IMG_SBL))
#define IMG_RSVD2_SIZE           (IMG_HDR_RSVD_SIZE - VER_VAL_SIZE - IMG_INFO_SIZE - GOLDEN_PATTERN_SIZE - \
                                  NAND_IMG_INFO_SIZE - IMG_FEATURE_STR_LEN - IMG_RELEASE_VER_SIZE)

#define BOOT_PATCH_IGNORE_COMMON_IMG   1
#define CLR_BITMAP(val, mask)          ((val)  &= ((uint32_t)0xFFFFFFFF) << (mask))
/*IC Type refer to WIKI: https://wiki.realtek.com/display/Bee1/BT+SOC+IC+Type*/
#define IC_TYPE                    0x13

#define STACK_PATCH_IMG_ID             IMG_STACKPATCH
#define STACK_PATCH_IMG_ID_OFFSET      (IMG_STACKPATCH - IMG_SBL)

#define get_image_payload_length(ptr)  (ptr->ctrl_header.payload_len)
/** End of FLASH_DEVICE_Exported_Constants
  * @}
  */
/*============================================================================*
  *                                   Types
  *============================================================================*/
/** @defgroup FLASH_DEVICE_Exported_Types Flash Device Exported Types
  * @brief
  * @{
  */
typedef enum _IMG_ID
{
    IMG_OCCD        = 0x278E,
    IMG_BOOTPATCH   = 0x278F,
    IMG_OTA         = 0x2790, /* OTA header */
    IMG_SBL         = 0x2791,
    IMG_MCUPATCH    = 0x2792,
    IMG_MCUAPP      = 0x2793,
    IMG_DSPSYSTEM    = 0x2794,
    IMG_DSPAPP      = 0x2795,
    IMG_MCUCONFIG     = 0x2796,
    IMG_DSPCONFIG     = 0x2797,
    IMG_ANC         = 0x2798,
    IMG_EXT1        = 0x2799,
    IMG_EXT2        = 0x279A,
    IMG_EXT3        = 0x279B,
    IMG_ADSP        = 0x279B,
    IMG_SYSPATCH    = 0x279C,
    IMG_STACKPATCH  = 0x279D,
    IMG_UPPERSTACK  = 0x279E,
    IMG_FRAMEWORK   = 0x279F,
    IMG_SYSDATA     = 0x27A0,
    IMAGE_MAX
} IMG_ID;


typedef enum _PRE_IMG_ID
{
    PRE_IMG_SYSPATCH         = IMAGE_MAX,
    PRE_IMG_STACKPATCH       = 0x27A2,
    PRE_IMG_UPPERSTACK       = 0x27A3,
    PRE_IMG_VP               = 0x27A4,
    PRE_IMAGE_MAX
} PRE_IMG_ID;

typedef enum _USER_IMG_ID
{
    IMG_USER_DATA8      = 0xFFF7,
    IMG_USER_DATA_FIRST = IMG_USER_DATA8,
    IMG_USER_DATA7      = 0xFFF8,
    IMG_USER_DATA6      = 0xFFF9,
    IMG_USER_DATA5      = 0xFFFA,
    IMG_USER_DATA4      = 0xFFFB,
    IMG_USER_DATA3      = 0xFFFC,
    IMG_USER_DATA2      = 0xFFFD,
    IMG_USER_DATA1      = 0xFFFE,
    IMG_USER_DATA_MAX   = 0xFFFF,
} USER_IMG_ID;

typedef enum
{
    IMG_CHECK_PASS,
    IMG_CHECK_ERR_HEADER_ADDR,
    IMG_CHECK_ERR_NOT_READY,
    IMG_CHECK_ERR_ROM_UUID,
    IMG_CHECK_ERR_MAGIC_PATTERN,
    IMG_CHECK_ERR_SIZE,
    IMG_CHECK_ERR_ID,
    IMG_CHECK_ERR_ENTRY_RETURN,
    IMG_CHECK_ERR_VERIFY,
    IMG_CHECK_ERR_EXE_BASE,
    IMG_CHECK_ERR_MAX,
} IMG_CHECK_ERR_TYPE;

typedef enum _ENC_KEY_SELECT
{
    ENC_KEY_SCEK = 0,
    ENC_KEY_SCEK_WITH_RTKCONST,
    ENC_KEY_OCEK,
    ENC_KEY_OCEK_WITH_OEMCONST,
    ENC_KEY_ON_FLASH,
    ENC_KEY_MAX,
} ENC_KEY_SELECT;

typedef enum _IMG_ENC_TYPE
{
    IMG_ENC_TYPE_NORMAL_AES       = 1,
    IMG_ENC_TYPE_ON_THE_FLY       = 2,
    IMG_ENC_TYPE_MAX
} IMG_ENC_TYPE;

typedef union
{
    uint8_t d8[ECDSA_UNCOMPRESSED_PUBLIC_KEY_LEN];
    struct
    {
        uint8_t flag; /*04 for uncompressed public key*/
        uint8_t x[ECDSA_PUBLIC_KEY_BYTE_SIZE];
        uint8_t y[ECDSA_PUBLIC_KEY_BYTE_SIZE];
    } ecdsa_pb_key;
} PUBLIC_KEY;

typedef struct _IMG_CTRL_HEADER_FORMAT
{
    union
    {
        uint32_t value;
        struct
        {
            uint32_t header_version: 8;
uint32_t device_type:
            4; //nor :0; nand :1: , sd : 2, emmc :3 ,  eflash: 4, nand\sd\emmc: 14, whatever: 15
            uint32_t integrity_check_en_in_boot: 1; // enable image integrity check in boot flow
            uint32_t load_when_boot: 1; // load image when boot
            uint32_t not_ready: 1; //for copy image in ota
            uint32_t not_obsolete: 1; //for copy image in ota
            uint32_t compressed_not_ready: 1;
            uint32_t compressed_not_obsolete: 1;
            uint32_t compressed_image_type: 3;
            uint32_t ctrl_flag_reserved: 11;
        };
    } ctrl_flag;
    uint8_t ic_type;
    uint8_t secure_version;
    uint16_t image_id;
    uint32_t header_len;
    uint32_t payload_len;
} T_IMG_CTRL_HEADER_FORMAT;

typedef struct
{
    union
    {
        uint64_t version;
        struct
        {
            uint64_t _version_reserve: 32;   //!< reserved
            uint64_t _version_revision: 16; //!< revision version
            uint64_t _version_minor: 8;     //!< minor version
            uint64_t _version_major: 8;     //!< major version
        } sub_version;
    };
    uint32_t _version_commitid;     //!< git commit id
    uint8_t _customer_name[8];      //!< branch name for customer patch
    uint8_t version_reserved[4];
} T_VERSION_FORMAT;

//typedef union
//{
//    uint8_t bytes[24];
//    struct
//    {
//        uint32_t itcm_heap_addr;
//        uint32_t app_ram_data_size;
//        uint32_t app_heap_data_on_size;
//        uint32_t app_heap_data_off_size;
//    } app_ram_info;
//} T_EXTRA_INFO_FORMAT;
#define IMG_SHA_START_OFFSET (sizeof(T_AUTH_HEADER_FORMAT) - sizeof(PUBLIC_KEY) - 3)
typedef struct _AUTH_HEADER_FORMAT
{
    uint8_t img_string[6];// "IMGHDR"
    uint16_t auth_length;
    uint8_t auth_type;
    uint8_t auth_reserved[7]; //for cmac 16bytes align
    uint8_t image_mac[CMAC_BYTE_SIZE];
    uint8_t image_signature[SIGNATURE_BYTE_SIZE];
    uint8_t image_hash[SHA256_BYTE_SIZE];
    PUBLIC_KEY PubKey;
    uint8_t auth_reserved1[3];
} T_AUTH_HEADER_FORMAT; // 196 bytes

typedef struct
{
    union
    {
        uint32_t value;
        struct
        {
            uint32_t enc: 1;
            uint32_t enc_type: 3; // 1: normal AES 2: on the fly
            uint32_t mode: 4; // gcm mode:0b00, ctr mode:0b01, mix mode:0b10
            uint32_t key_select: 4;
            uint32_t is_encrypted: 1;
            uint32_t rsvd: 19;
        };
    } ctrl_flag;
    uint32_t encryption_load_addr; // load base  nor: flash address nand/sd/emmc: psram
    uint32_t encryption_size;  // load size
    uint32_t encryption_exe_addr;  // exe base  nor: ram address nand/sd/emmc: psram
    uint8_t dec_key[16];
    union
    {
        uint8_t iv[16];
        struct
        {
            uint8_t  iv_high[4];
            uint8_t  iv_low[4];
            uint8_t  iv_reserved[8];
        };
    };

} T_ENCRYPT_FORMAT; //48bytes

typedef struct
{
    uint32_t  image_load_base;
    uint32_t  image_load_size;
    uint32_t  image_exe_base;
    uint32_t  image_exe_size;
} T_IMAGE_INFO;

typedef union
{
    uint8_t  common_reserved[64];
    struct
    {
        uint8_t  common_reserved1[5];
        uint8_t  dev_id_valid;
        uint16_t dev_id;
        uint8_t  image_feature[IMG_FEATURE_STR_LEN];     // offset: 1008,used for image feature string
        uint64_t image_release_version;                   //offset: 1000, used for image release version
    };
} T_COMMON_EXTRA_INFO_FORMAT;

typedef union _IMG_HEADER_FORMAT
{
    uint8_t bytes[DEFAULT_HEADER_SIZE];
    struct
    {
        T_AUTH_HEADER_FORMAT auth;  // 196 bytes
        T_IMG_CTRL_HEADER_FORMAT ctrl_header;  //16bytes
        T_ENCRYPT_FORMAT encrypt_header; //48bytes
        uint8_t uuid[16];
        uint32_t magic_pattern;
        T_VERSION_FORMAT git_ver;  //24bytes
        T_COMMON_EXTRA_INFO_FORMAT common_extra_info;
        union //656bytes
        {
            struct  // data 8bytes
            {
                uint16_t reserved[1];
                uint16_t tool_version;
                uint32_t timestamp;
                uint8_t  data_reserved[648];
            };

            struct // image_header
            {
                uint32_t exe_entry;
                uint32_t image_base; //image begin: nor:flash address   nand/sd/emmc: psram address
                uint32_t ram_load_src; // ram code load base
                uint32_t ram_load_len; // ram code load size
                uint32_t ram_load_dst; // ram code exe base
                union
                {
                    uint8_t  exe_image_reserved[636]; //T_EXTRA_INFO_FORMAT ex_info; //uint8_t comiple_stamp[16];
                    struct // boot patch image_header 148
                    {
                        uint32_t resv_golden_pattern[GOLDEN_PATTERN_WORD_LEN];
                        uint32_t pre_image_num;
                        T_IMAGE_INFO ext_image_info[(PRE_IMAGE_MAX - IMAGE_MAX)]; //4x4
                        uint8_t boot_patch_header_reserved[488];
                    };
                    struct // image_header 16
                    {
                        T_EXTRA_INFO_FORMAT ex_info;
                    };
                };
            };
            struct   // ota header 264
            {
                uint32_t layout_size; // K
                uint32_t ota_image_num;
                T_IMAGE_INFO image_info[(IMAGE_MAX - IMG_SBL)];  //16x4
                uint8_t  ota_header_reserved[328];
            };
        };
    };
} T_IMG_HEADER_FORMAT;

typedef struct _ROM_HEADER_FORMAT
{
    uint8_t uuid[16];
    void *init_ptr;
    void *entry_ptr;
} T_ROM_HEADER_FORMAT;

//static __inline uint32_t *get_image_addr_in_bank(uint32_t ota_addr, IMG_ID image_id)
//{
//    T_IMG_HEADER_FORMAT *ota = (T_IMG_HEADER_FORMAT *)ota_addr;
//    return &ota->image_info[(image_id - IMG_SBL)].image_load_base;
//}

//static __inline uint32_t *get_image_size_in_bank(uint32_t ota_addr, IMG_ID image_id)
//{
//    T_IMG_HEADER_FORMAT *ota = (T_IMG_HEADER_FORMAT *)ota_addr;
//    return &ota->image_info[(image_id - IMG_SBL)].image_load_size;
//}

static __inline uint32_t *get_image_addr_in_bootpatch(uint32_t bootpatch_addr, PRE_IMG_ID image_id)
{
    T_IMG_HEADER_FORMAT *bootpatch = (T_IMG_HEADER_FORMAT *)bootpatch_addr;
    return &bootpatch->ext_image_info[(image_id - PRE_IMG_SYSPATCH)].image_load_base;
}

static __inline uint32_t *get_image_size_in_bootpatch(uint32_t bootpatch_addr, PRE_IMG_ID image_id)
{
    T_IMG_HEADER_FORMAT *bootpatch = (T_IMG_HEADER_FORMAT *)bootpatch_addr;
    return &bootpatch->ext_image_info[(image_id - PRE_IMG_SYSPATCH)].image_load_size;
}

static __inline uint32_t *get_image_load_addr_in_bank(uint32_t ota_addr, IMG_ID image_id)
{
    T_IMG_HEADER_FORMAT *ota = (T_IMG_HEADER_FORMAT *)ota_addr;
    return &ota->image_info[image_id - IMG_SBL].image_load_base;
}

static __inline uint32_t *get_image_load_size_in_bank(uint32_t ota_addr, IMG_ID image_id)
{
    T_IMG_HEADER_FORMAT *ota = (T_IMG_HEADER_FORMAT *)ota_addr;
    return &ota->image_info[image_id - IMG_SBL].image_load_size;
}

static __inline uint32_t *get_image_exe_addr_in_bank(uint32_t ota_addr, IMG_ID image_id)
{
    T_IMG_HEADER_FORMAT *ota = (T_IMG_HEADER_FORMAT *)ota_addr;
    return &ota->image_info[(image_id - IMG_SBL)].image_exe_base;
}

static __inline uint32_t *get_image_exe_size_in_bank(uint32_t ota_addr, IMG_ID image_id)
{
    T_IMG_HEADER_FORMAT *ota = (T_IMG_HEADER_FORMAT *)ota_addr;
    return &ota->image_info[(image_id - IMG_SBL)].image_exe_size;
}

extern uint32_t ota_header_addr_rom;
static __inline uint32_t get_active_ota_bank_addr(void)
{
    return ota_header_addr_rom;
}

extern uint32_t boot_patch_addr_rom;
static __inline uint32_t get_active_boot_patch_addr(void)
{
    return boot_patch_addr_rom;
}



/** End of FLASH_DEVICE_Exported_Types
  * @}
  */
/*************************************************************************************************
*                          Constants
*************************************************************************************************/


/*************************************************************************************************
*                          functions
*************************************************************************************************/
/** @defgroup FLASH_DEVICE_Exported_Functions Flash Device Exported Functions
    * @brief
    * @{
    */
/**
 * @brief check specified signature image start address
 * @param header image header info of the given image.
 * @param signature signature to specify which image
 * @return image address
*/
uint32_t check_image_header(T_IMG_HEADER_FORMAT *header, uint16_t image_id);

IMG_CHECK_ERR_TYPE check_header_valid(uint32_t header_addr, IMG_ID image_id);
IMG_CHECK_ERR_TYPE image_entry(uint16_t image_id, uint32_t image_addr);
bool image_integrity_check(uint32_t img_addr, uint16_t image_id);
bool image_loading(uint32_t img_addr);
uint8_t get_stack_patch_valid(void);
uint32_t get_stack_patch_rom_header_addr(void);
IMG_CHECK_ERR_TYPE image_entry_check(T_ROM_HEADER_FORMAT *rom_header,
                                     T_ROM_HEADER_FORMAT *patch_header);
IMG_CHECK_ERR_TYPE stack_patch_check_entry(void);

uint32_t get_header_addr_by_img_id(IMG_ID image_id);
uint32_t get_temp_ota_bank_addr_by_img_id(IMG_ID image_id);
uint32_t get_img_header_format_size(void);
T_EXTRA_INFO_FORMAT *get_extra_info(void);
extern bool (*is_ota_support_bank_switch)(void);
extern bool (*image_verify)(uint32_t img_addr, IMG_ID img_id);
extern bool (*image_authentication)(uint32_t img_addr, IMG_ID img_id);
extern bool (*image_decryption)(T_IMG_HEADER_FORMAT *header);
extern bool (*is_secure_boot)(void);
extern bool (*ram_image_authentication)(uint32_t addr, uint32_t size, IMG_ID img_id);

/** @} */ /* End of group FLASH_DEVICE_Exported_Functions */
/** @} */ /* End of group FLASH_DEVICE */

#endif // _PATCH_HEADER_CHECK_H_
