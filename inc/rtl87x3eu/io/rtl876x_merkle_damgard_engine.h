/**
*********************************************************************************************************
*               Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_merkle_damgard_engine.h
* @brief     this is the driver for sha2 and sm3 combo mode
* @details   only supports sha2-224, sha2-256 and sm3, and supports external DMA mode
* @author    kunhsuan wu
* @date      2023-07-18
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef __RTL876X_MERKLE_DAMGARD_ENGINE_H
#define __RTL876X_MERKLE_DAMGARD_ENGINE_H

#ifdef __cpluspuls
extern "C" {
#endif
    /* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "rtl876x.h"
#include "mde_config.h"

    /**
      * @brief reference to CTC hmac engine register table
      */
    typedef struct
    {
        /* the byte inversion example:
         *
         *                   addr   3    2    1    0
         *                    4   0x7a 0xaa 0x21 0x22
         *                    0   0xee 0xfe 0x97 0xad
         *
         *  if xxx_byte_inversion_en = 1
         *      the data input to the engine will be 0xad97feee2221aa7a
         *  if xxx_byte_inversion_en = 0
         *      the data input to the engine will be 0x7aaa2122eefe97ad
         */
        union
        {
            __IO uint32_t Control; // 0x00, control register
            struct
            {
                __IO uint32_t sha2_mode: 2;     // [1:0] 0: sha2-224, 1: sha2-256, 2: sha2-384, 3: sha2-512
                __IO uint32_t dma_mode: 1;      // [2] 0: read-only mode, 1: copy mode, only use read only mode
                __IO uint32_t sha1_mode: 1;     // [3] 0: non-sha1 mode, 1: sha1 mode if sm3_mode is 0
                __IO uint32_t hmac_mode: 1;     // [4] 0: hash mode, 1: hmac mode
                __IO uint32_t icg_enable: 1;    // [5]: enable auto clock gating
                __IO uint32_t slave_mode: 1;    /* [6] 0: internal dma mode, 1: slave(cpu) mode,
                                             *      hw is fixed 1 if configured to ext dma */
                __IO uint32_t sm3_mode: 1;      // [7] 0: non-sm3 mode, 1: sm3 mode
                __IO uint32_t hmac_key_size: 2; // [9:8] 0: 128 bits, 1: 192 bits, 2: 256 bits
                __IO uint32_t sha512_truncate_en: 1; // [10] 0: normal sha2-512, 1: truncated sha2-512
                __IO uint32_t sha512t256_mode: 1;   // [11] 0: sha2-512/224, 1: sha2-512/256
                __IO uint32_t payload_byte_inversion_en: 1; // [12] 0: high address as MSB, 1: low address as MSB
                __IO uint32_t key_byte_inversion_en: 1; // [13] 0: high address as MSB, 1: low address as MSB
                __IO uint32_t fifo_full_suspend: 1; // [14] 0: high address as MSB, 1: low address as MSB
                __I uint32_t rsvd0: 9;
                __IO uint32_t software_reset: 1; // [24]: trigger software reset, read this bit until 0
                __I uint32_t rsvd1: 7;
            } Control_fields;
        };
        union
        {
            __I uint32_t Status; // 0x04, status register
            struct
            {
                __I uint32_t fifo_left_size: 5; // [4:0]: record left size in hw fifo
                __I uint32_t rsvd0: 23;
                __I uint32_t sha_core_busy: 1;  // [28] 1: engine busy
                __I uint32_t rsvd: 3;
            } Status_fields;
        };
        union
        {
            __IO uint32_t BlockCtrl; // 0x08, BlockCtrl register
            struct
            {
                __IO uint32_t first_block: 1;    /* [0]: this operation is the first block,
                                                        if sha/sm3 mode, the digest will be initialized
                                                        if hmac mode, the i_pad will be executed first
                                                        this bit automatically cleared by hardware */
                __IO uint32_t last_block: 1;     /* [1]: this operation is the last block,
                                                        if sha/sm3 mode, the padding step will be executed at the end
                                                        if hmac mode, the o_pad will be executed at the end
                                                        this bit should be cleared by software */
                __IO uint32_t end_slave_input: 1;// [2]: if all input data is completed, set 1 to start padding
                __I uint32_t rsvd: 29;
            } BlockCtrl_fields;
        };
        __I uint32_t Reserved;  // 0x0C

        /* The IV/digest regsiters of SHA2-224/256, SM3 are 32-bit
         * H0 = SHA digest register 0
         * H1 = SHA digest register 2
         * H2 = SHA digest register 4
         * H3 = SHA digest register 6
         * H4 = SHA digest register 8
         * H5 = SHA digest register 10
         * H6 = SHA digest register 12
         * H7 = SHA digest register 14
         * since SHA2-224/256, SM3 use 32-bit, the digest register 1, 3, 5... should be set to 0 if use
            SW to setup initial hash value

         * The IV/digest regsiters of SHA2-384/512 are 64-bit
         * H0 = SHA digest register 0 || SHA digest register 1
         * H1 = SHA digest register 2 || SHA digest register 3
         * H2 = SHA digest register 4 || SHA digest register 5
         * H3 = SHA digest register 6 || SHA digest register 7
         * H4 = SHA digest register 8 || SHA digest register 9
         * H5 = SHA digest register 10 || SHA digest register 11
         * H6 = SHA digest register 12 || SHA digest register 13
         * H7 = SHA digest register 14 || SHA digest register 15
         *
         * The hash value is H0 || H1 || H2 || H3 || H4 || H5 || H6 || H7
         * || means concatenate

         * Software can reset digest registers by setting BlockCtrl[0] after setting SHA mode
         */
        union
        {
            __IO uint32_t digest[16]; // 0x10 ~ 0x4c, reset is SHA384 iv
            struct
            {
                __IO uint32_t SHA_DIGEST_0;     // 0x10, hash value 0 MSB
                __IO uint32_t SHA_DIGEST_1;     // 0x14, hash value 0 LSB
                __IO uint32_t SHA_DIGEST_2;     // 0x18, hash value 1 MSB
                __IO uint32_t SHA_DIGEST_3;     // 0x1c, hash value 1 LSB
                __IO uint32_t SHA_DIGEST_4;     // 0x20, hash value 2 MSB
                __IO uint32_t SHA_DIGEST_5;     // 0x24, hash value 2 LSB
                __IO uint32_t SHA_DIGEST_6;     // 0x28, hash value 3 MSB
                __IO uint32_t SHA_DIGEST_7;     // 0x2c, hash value 3 LSB
                __IO uint32_t SHA_DIGEST_8;     // 0x30, hash value 4 MSB
                __IO uint32_t SHA_DIGEST_9;     // 0x34, hash value 4 LSB
                __IO uint32_t SHA_DIGEST_10;    // 0x38, hash value 5 MSB
                __IO uint32_t SHA_DIGEST_11;    // 0x3c, hash value 5 LSB
                __IO uint32_t SHA_DIGEST_12;    // 0x40, hash value 6 MSB
                __IO uint32_t SHA_DIGEST_13;    // 0x44, hash value 6 LSB
                __IO uint32_t SHA_DIGEST_14;    // 0x48, hash value 7 MSB
                __IO uint32_t SHA_DIGEST_15;    // 0x4c, hash value 7 LSB
            } digest_fields;
        };
        __O uint32_t FIFO_DATA; // 0x50, fifo data register for slave mode

        __I uint32_t Reserved1[3];  // 0x54 ~ 0x5C

        union
        {
            __IO uint32_t hmac_key[8]; // 0x60 ~ 0x7c
            struct
            {
                __IO uint32_t HMAC_KEY_0;     // 0x60, hmac key[255:224]
                __IO uint32_t HMAC_KEY_1;     // 0x64, hmac key[223:192]
                __IO uint32_t HMAC_KEY_2;     // 0x68, hmac key[191:160]
                __IO uint32_t HMAC_KEY_3;     // 0x6c, hmac key[159:128]
                __IO uint32_t HMAC_KEY_4;     // 0x70, hmac key[127:96]
                __IO uint32_t HMAC_KEY_5;     // 0x74, hmac key[95:64]
                __IO uint32_t HMAC_KEY_6;     // 0x78, hmac key[63:32]
                __IO uint32_t HMAC_KEY_7;     // 0x7c, hmac key[31:0]
            } hmac_key_fields;
        };

        union
        {
            /* if hmac mode, fill the length with ipad and opad,
             * e.g,: 0x40 if sha2-224/256, 0x80 if sha2-384/512
             */
            __IO uint32_t hash_input_len[4]; // 0x80 ~ 0x8c, hash total byte len
            struct
            {
                __IO uint32_t HASH_input_len_0; // 0x80, hash input byte len [127:96]
                __IO uint32_t HASH_input_len_1; // 0x84, hash input byte len [95:64]
                __IO uint32_t HASH_input_len_2; // 0x88, hash input byte len [63:32]
                __IO uint32_t HASH_input_len_3; // 0x8c, hash input byte len [31:0]
            } hash_input_len_fields;
        };

        __I uint32_t Reserved2[28];  // 0x90 ~ 0xFC

        union
        {
            __IO uint32_t sha256_digest_alias[8]; // 0x100 ~ 0x11c
            struct
            {
                __IO uint32_t SHA256_DIGEST_ALIAS_0;     // 0x100, sha256 hash value 0
                __IO uint32_t SHA256_DIGEST_ALIAS_1;     // 0x104, sha256 hash value 1
                __IO uint32_t SHA256_DIGEST_ALIAS_2;     // 0x108, sha256 hash value 2
                __IO uint32_t SHA256_DIGEST_ALIAS_3;     // 0x10c, sha256 hash value 3
                __IO uint32_t SHA256_DIGEST_ALIAS_4;     // 0x110, sha256 hash value 4
                __IO uint32_t SHA256_DIGEST_ALIAS_5;     // 0x114, sha256 hash value 5
                __IO uint32_t SHA256_DIGEST_ALIAS_6;     // 0x118, sha256 hash value 6
                __IO uint32_t SHA256_DIGEST_ALIAS_7;     // 0x11c, sha256 hash value 7
            } sha256_digest_alias_fields;
        };

        union
        {
            __IO uint32_t DMA_HS_Config; // 0x120, DMA handshake config register
            struct
            {
                __IO uint32_t dma_hs_tr_width: 3;   /* [2:0]: The expected data width of each write transaction
                                                              from DMA. Since the engine used the APB protocol,
                                                              please set the field to 3'b010 */
                __IO uint32_t dma_hs_msize: 3;      /* [5:3]: The expected number of burst write transaction length
                                                              Please set this field to 3'b000 to avoid data loss */
                __I uint32_t rsvd: 26;
            } DMA_HS_Config_fields;
        };

        __IO uint32_t dma_hs_block_ts; // 0x124, Total data bytes for requesting DMA

        union
        {
            __IO uint32_t DMA_HS_buffer_byte; // 0x128, DMA handshake buffer byte register
            struct
            {
                __IO uint32_t dma_hs_buffer_byte: 8;    /* [7:0]: This register indicates the data bytes
                                                                  processed in engine. This number will not
                                                                  larger than block size of engine */
                __I uint32_t rsvd: 24;
            } DMA_HS_buffer_byte_fields;
        };

        union
        {
            __IO uint32_t DMA_HS_EN; // 0x12C, DMA handshake enable register
            struct
            {
                __IO uint32_t dma_hs_en: 1;    /* [0]: Once the register is enabled,
                                                       the engine starts to send the request to the DMA until
                                                       all the data being transferred, automatically cleared by hardware */
                __I uint32_t rsvd: 31;
            } DMA_HS_EN_fields;
        };

        __I uint32_t Reserved3[756];  // 0x130 ~ 0xCFC

        union
        {
            __IO uint32_t Mutex_status; // 0xD00, mutex status register
            struct
            {
                __IO uint32_t engine_mutex_status: 2;    /* [1:0]: if read 0, lock mutex,
                                                        if not 0, the engine is being used
                                                        0x1: secure domain lock mutex,
                                                        0x2: non secure domain lock mutex
                                                        write 0 to unlock mutex*/
                __I uint32_t rsvd: 30;
            } Mutex_status_fields;
        };

        union
        {
            __IO uint32_t Mutex_overwrite; // 0xD04, mutex overwrite register
            struct
            {
                __IO uint32_t engine_mutex_overwrite: 1;    /* [0]: allow secure domain overwrite mutex,
                                                           0: enable,
                                                           1: disable*/
                __I uint32_t rsvd: 31;
            } Mutex_overwrite_fields;
        };

        __I uint32_t Reserved4[186];  // 0xD08 ~ 0xFEC

        union
        {
            __I uint32_t Configuration; // 0xFF0, Configuration register
            struct
            {
                __I uint32_t sha2_core_en: 1;               /* [0] 0: disabled, 1: enabled */
                __I uint32_t support_sha512: 1;             /* [1] 0: disabled, 1: enabled */
                __I uint32_t support_truncated_sha512: 1;   /* [2] 0: disabled, 1: enabled */
                __I uint32_t support_hmac_mode: 1;          /* [3] 0: disabled, 1: enabled */
                __I uint32_t support_copy_mode: 1;          /* [4] 0: disabled, 1: enabled */
                __I uint32_t external_key_en: 1;            /* [5] 0: disabled, 1: enabled */
                __I uint32_t trustzone_en: 1;               /* [6] 0: disabled, 1: enabled */
                __I uint32_t sha1_core_en: 1;               /* [7] 0: disabled, 1: enabled */
                __I uint32_t sm3_core_en: 1;                /* [8] 0: disabled, 1: enabled */
                __I uint32_t rsvd0: 2;
                __I uint32_t disable_core_sha2: 1;          /* [12] 0: disabled, 1: enabled */
                __I uint32_t disable_core_sha1: 1;          /* [13] 0: disabled, 1: enabled */
                __I uint32_t disable_core_sm3: 1;           /* [14] 0: disabled, 1: enabled */
                __I uint32_t rsvd: 17;
            } Configuration_fields;
        };

        __I uint32_t Reserved5;  // 0xFF4

        union
        {
            __I uint32_t Version; // 0xFF8, Version register
            struct
            {
                __I uint32_t engine_version: 16;
                __I uint32_t svn_number: 16;
            } Version_fields;
        };

        union
        {
            __I uint32_t Date; // 0xFFC, Date register
            struct
            {
                __I uint32_t date: 16;
                __I uint32_t year: 16;
            } Date_fields;
        };
    } HW_MDE_CTRL_TypeDef;

#if MDE_SUPPORT_INT_DMA_EN == 1
    typedef struct
    {
        __IO uint32_t SAR0;             // 0x00, source address
        __IO uint32_t Reserved0;        // 0x04
        __IO uint32_t DAR0;             // 0x08, destination address
        __IO uint32_t Reserved1[3];     // 0x0C~0x14

        union
        {
            __IO uint32_t CTRL0;        // 0x18, low part of control register, default: 0x4801
            struct
            {
                __IO uint32_t int_en: 1;        // [0]: interrupt enable
                __IO uint32_t dst_tr_width: 3;  // [3:1]: destination transfer width, should be 0x0(8-bit)
                __IO uint32_t src_tr_width: 3;  // [6:4]: source transfer width, should be 0x0(8-bit)
                __IO uint32_t DINC: 2;          /* [8:7]: destination address increment,
                                                      should be 0x0 for increment mode */
                __IO uint32_t SINC: 2;          /* [10:9]: source address increment,
                                                       should be 0x0 for increment mode */
                __IO uint32_t DEST_MSIZE: 3;    /* [13:11]: destination burst length,
                                                        for ro mode: don't care
                                                        for rw mode: must be 0x0(single) */
                __IO uint32_t SRC_MSIZE: 3;     /* [16:14]: source burst length,
                                                        for sha224/256: should be 0x2(burst 8)
                                                        for sha384/512: should be 0x3(burst 16) */
                __IO uint32_t rsvd: 15;
            } CTRL0_fields;
        };

        union
        {
            __O uint32_t CTRL0_block_ts;   // 0x1C, high part of control register
            struct
            {
                __O uint32_t block_ts: 19;     /* [18:0]: block transfer size (block count * src msize),
                                                      should be multiple SHA2 blocks
                                                      maximum block transfer size is according to
                                                      MDE_MAX_DMA_BUF_BLOCK_SIZE */
                __O uint32_t rsvd: 13;
            } CTRL0_block_ts_fields;
        };

        __IO uint32_t Reserved2[9];     // 0x20~0x40

        union
        {
            __IO uint32_t CFG0;         // 0x44, high part of config register, defualt: 0x8
            struct
            {
                __IO uint32_t rsvd0: 3;
                __IO uint32_t PROTCTL: 1;       /* [3]: security control (AxPROT[1])
                                                    0: secure DMA transfer, 1: non-secure DMA transfer */
                __IO uint32_t rsvd1: 28;
            } CFG0_fields;
        };

        __IO uint32_t Reserved3[168];   // 0x48~0x2E4

        union
        {
            __IO uint32_t StatusTfr;      // 0x2E8
            struct
            {
                __I uint32_t status_tfr: 8;     // [7:0]: transfer status of each channel
                __IO uint32_t rsvd: 24;
            } StatusTfr_fields;
        };

        __IO uint32_t Reserved4[9];   // 0x2EC~0x30C

        union
        {
            __IO uint32_t MaskTfr;      // 0x310
            struct
            {
                __IO uint32_t INT_MASK: 1;      // [0]: interrupt mask, 0: masked, 1: unmasked
                __IO uint32_t rsvd0: 7;
                __O uint32_t INT_MASK_WE: 1;    // [8]: interrupt mask write enable
                __IO uint32_t rsvd1: 23;
            } MaskTfr_fields;
        };

        __IO uint32_t Reserved5[9];     // 0x314~0x334

        union
        {
            __O uint32_t ClearTfr;      // 0x338
            struct
            {
                __O uint32_t ClearTfr: 1;      // [0]: write 1 to clear transfer complete interrupt (TFR)
                __O uint32_t rsvd0: 31;
            } ClearTfr_fields;
        };

        __IO uint32_t Reserved6[9];     // 0x33C~0x35C

        union
        {
            __I uint32_t StatusInt;     // 0x360
            struct
            {
                __I uint32_t TFR: 1;        // [0]: DMA transfer complete
                __I uint32_t BLOCK: 1;      // [1]: block transfer complete (masked, unused)
                __I uint32_t SRCT: 1;       // [2]: SCR transfer complete (masked, unused)
                __I uint32_t DSTT: 1;       // [3]: DST transfer complete (masked, unused)
                __I uint32_t ERR: 1;        // [4]: bus transfer error
                __I uint32_t rsvd0: 27;
            } StatusInt_fields;
        };

        __IO uint32_t Reserved7[13];    // 0x364~0x394

        union
        {
            __IO uint32_t DmaCfgReg;    // 0x398
            struct
            {
                __IO uint32_t DMA_EN: 1;        // [0] 0: DMA disabled, 1: DMA enabled
                __IO uint32_t rsvd0: 31;
            } DmaCfgReg_fields;
        };

        __IO uint32_t Reserved8;        // 0x39C

        union
        {
            __IO uint32_t ChEnReg;      // 0x3A0
            struct
            {
                __IO uint32_t CH_EN: 1;         /* [0]: enable dma channel, 0: disable, 1: enable
                                                    this bit is automatically cleared by hw to
                                                    disable the channel after DMA transfer completed
                                                    sw can poll this bit to determine when this
                                                    channel is done */
                __IO uint32_t rsvd0: 7;
                __O uint32_t CH_EN_WE: 1;       // [8]: channel enable write enable
                __IO uint32_t rsvd1: 23;
            } ChEnReg_fields;
        };
    } HW_MDE_DMA_TypeDef;

#define MDE_DMA                         ((HW_MDE_DMA_TypeDef *) MDE_DMA_BASE_ADDR)

#define MDE_DMA_CH_EN_WE                (BIT8)
#define MDE_DMA_CH_EN                   (BIT0)
#endif

#define MDE_CTRL                        ((HW_MDE_CTRL_TypeDef *) MDE_CTRL_BASE_ADDR)
#define MDE_FIFO_OFFSET                 (0x50)
#define MDE_ENGINE_FIFO_INPUT           (MDE_CTRL_BASE_ADDR + MDE_FIFO_OFFSET)

#define MDE_PAD_FIRST                   (0x80)
#define MDE_HASH_BYTE_LEN_REG_LEN       4

#define SHA1_BLOCK_BYTE_SIZE            (64)
#define SHA1_BLOCK_BIT_SIZE             (512)
#define SHA2_224_BLOCK_BYTE_SIZE        (64)
#define SHA2_224_BLOCK_BIT_SIZE         (512)
#define SHA2_256_BLOCK_BYTE_SIZE        (64)
#define SHA2_256_BLOCK_BIT_SIZE         (512)
#define SHA2_384_BLOCK_BYTE_SIZE        (128)
#define SHA2_384_BLOCK_BIT_SIZE         (1024)
#define SHA2_512_BLOCK_BYTE_SIZE        (128)
#define SHA2_512_BLOCK_BIT_SIZE         (1024)
#define SM3_BLOCK_BYTE_SIZE             (64)
#define SM3_BLOCK_BIT_SIZE              (512)

#define SHA1_DIGEST_BYTE_SIZE           20
#define SHA2_224_DIGEST_BYTE_SIZE       28
#define SHA2_256_DIGEST_BYTE_SIZE       32
#define SHA2_384_DIGEST_BYTE_SIZE       48
#define SHA2_512_DIGEST_BYTE_SIZE       64
#define SM3_DIGEST_BYTE_SIZE            32

#define MDE_DEBUG_LOG 0
#if (MDE_DEBUG_LOG == 1)
#define MDE_LOG(...) DBG_DIRECT(__VA_ARGS__)
#else
#define MDE_LOG(...)
#endif

    typedef enum
    {
        HW_MDE_DMA_MODE = 0,
        HW_MDE_CPU_MODE = 1
    } HW_MDE_ACCESS_MODE;

    typedef enum
    {
        HW_MDE_HASH_ALGO_SM3          = 0x0,
        HW_MDE_HASH_ALGO_SHA1         = 0x1,
        HW_MDE_HASH_ALGO_SHA2_224     = 0x2,
        HW_MDE_HASH_ALGO_SHA2_256     = 0x3,
        HW_MDE_HASH_ALGO_SHA2_384     = 0x4,
        HW_MDE_HASH_ALGO_SHA2_512     = 0x5,
        HW_MDE_HASH_ALGO_SHA2_512t224 = 0x6,
        HW_MDE_HASH_ALGO_SHA2_512t256 = 0x7,

        HW_MDE_HASH_ALGO_MAX          = 0x8,

        HW_MDE_HMAC_ALGO_SM3          = 0x100,
        HW_MDE_HMAC_ALGO_SHA1         = 0x101,
        HW_MDE_HMAC_ALGO_SHA2_224     = 0x102,
        HW_MDE_HMAC_ALGO_SHA2_256     = 0x103,
        HW_MDE_HMAC_ALGO_SHA2_384     = 0x104,
        HW_MDE_HMAC_ALGO_SHA2_512     = 0x105,
        HW_MDE_HMAC_ALGO_SHA2_512t224 = 0x106,
        HW_MDE_HMAC_ALGO_SHA2_512t256 = 0x107,

    } HW_MDE_ALGO;

    typedef enum
    {
        HW_MDE_KEY_128 = 128,
        HW_MDE_KEY_192 = 192,
        HW_MDE_KEY_256 = 256,
    } HW_MDE_KEY_SIZE_SEL;

    typedef union
    {
        uint64_t bit_len[2];
        uint8_t bit_len_arr[16];
    } Bit_len_TypeDef;

    typedef struct
    {
        uint64_t total[2];           /*!< The number of Bytes processed.  */
        uint32_t state[16];          /*!< The intermediate digest state. */
        uint8_t buffer[128];         /*!< The data block being processed. */
        HW_MDE_ALGO algo;            /*!< hash algorithm */
    } MDE_CTX;

    __STATIC_INLINE uint32_t hw_mde_get_iv_len(HW_MDE_ALGO algo)
    {
        HW_MDE_ALGO hash_algo = (algo & 0xff);

        uint32_t iv_byte_len_array[HW_MDE_HASH_ALGO_MAX] =
        {
            SM3_DIGEST_BYTE_SIZE,
            SHA1_DIGEST_BYTE_SIZE,
            SHA2_256_DIGEST_BYTE_SIZE,  // sha2 224 is also 32 byte iv
            SHA2_256_DIGEST_BYTE_SIZE,
            SHA2_512_DIGEST_BYTE_SIZE,  // sha2 384 is also 64 byte iv
            SHA2_512_DIGEST_BYTE_SIZE,
            SHA2_512_DIGEST_BYTE_SIZE,  // sha2 512t224 is also 64 byte iv
            SHA2_512_DIGEST_BYTE_SIZE,  // sha2 512t256 is also 64 byte iv
        };

        return iv_byte_len_array[hash_algo];
    }

    __STATIC_INLINE uint32_t hw_mde_get_digest_len(HW_MDE_ALGO algo)
    {
        HW_MDE_ALGO hash_algo = (algo & 0xff);

        uint32_t digest_byte_len_array[HW_MDE_HASH_ALGO_MAX] =
        {
            SM3_DIGEST_BYTE_SIZE,
            SHA1_DIGEST_BYTE_SIZE,
            SHA2_224_DIGEST_BYTE_SIZE,
            SHA2_256_DIGEST_BYTE_SIZE,
            SHA2_384_DIGEST_BYTE_SIZE,
            SHA2_512_DIGEST_BYTE_SIZE,
            SHA2_224_DIGEST_BYTE_SIZE,
            SHA2_256_DIGEST_BYTE_SIZE,
        };

        return digest_byte_len_array[hash_algo];
    }

    __STATIC_INLINE uint32_t hw_mde_get_block_byte_len(HW_MDE_ALGO algo)
    {
        HW_MDE_ALGO hash_algo = (algo & 0xff);

        uint32_t block_byte_len_array[HW_MDE_HASH_ALGO_MAX] =
        {
            SM3_BLOCK_BYTE_SIZE,
            SHA1_BLOCK_BYTE_SIZE,
            SHA2_224_BLOCK_BYTE_SIZE,
            SHA2_256_BLOCK_BYTE_SIZE,
            SHA2_384_BLOCK_BYTE_SIZE,
            SHA2_512_BLOCK_BYTE_SIZE,
            SHA2_512_BLOCK_BYTE_SIZE,
            SHA2_512_BLOCK_BYTE_SIZE,
        };

        return block_byte_len_array[hash_algo];
    }

    extern void (*hw_mde_iv_init)(HW_MDE_ALGO algo, uint32_t *iv);
    extern void (*hw_mde_start)(MDE_CTX *ctx, HW_MDE_ALGO algo);
    extern bool (*hw_mde_update)(MDE_CTX *ctx, const uint8_t *input, uint32_t byte_len,
                                 HW_MDE_ACCESS_MODE access_mode);
    extern bool (*hw_mde_finish)(MDE_CTX *ctx, uint32_t *result, HW_MDE_ACCESS_MODE access_mode);

    void hw_mde_get_digest(HW_MDE_ALGO algo, uint32_t *output);
    void hw_mde_init(void);

#if MDE_SUPPORT_DMA_MODE == 1
    extern void (*hw_mde_dma_channel_init)(uint8_t dma_rx_ch_num);

    void hw_mde_set_dma_move_src(uint32_t src);
    void hw_mde_set_dma_current_update_remain_len(uint32_t size);
    bool hw_mde_dma_done(void);
#endif

#if MDE_SUPPORT_HMAC_MODE == 1
    void hw_mde_set_hmac_key(const uint8_t *key, uint32_t key_bits, uint8_t key_sel);
#endif

#ifdef __cplusplus
}
#endif
#endif /*__RTL876X_MERKLE_DAMGARD_ENGINE_H*/

