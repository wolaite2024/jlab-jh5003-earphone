/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_hw_aes.h
* @brief
* @details
* @author    eason li
* @date      2016-01-04
* @version   v0.1
* *********************************************************************************************************
*/


#ifndef __RTL876X_HW_AES_H
#define __RTL876X_HW_AES_H

#ifdef __cpluspuls
extern "C" {
#endif
    /* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <string.h>
#include "rtl876x.h"
#include "aes_engine_config.h"

    /**
      * @brief reference to hw aes register table
      */
    typedef struct
    {
        union
        {
            __IO uint32_t CTL;
            struct
            {
                __IO uint32_t enc_en:          1; /* aes encryption enable */
                __IO uint32_t dec_en:          1; /* aes decryption enable */
                __IO uint32_t ase256_en:       1; /* aes 256-bit mode enable */
                __IO uint32_t aes_mode_sel:    3; /* aes mode select, see HW_AES_MODE below */
                __IO uint32_t access_mode:     1; /* 0: CPU mode, 1: DMA mode */
                __IO uint32_t scram_en:        1; /* scramble function, 0:disable, 1:enable */
                __IO uint32_t use_hidden_key:  1; /* secure related */
                __IO uint32_t copy_hidden_key: 1; /* secure related */
                __IO uint32_t hidden_256:      1; /* secure related */
                __IO uint32_t poly_en:         1; /* XOR polynomial, 0:disable, 1:enable */
                __IO uint32_t aes_key_sel:     3; /* 1: key2, 2: key3, 3: key4, 4: key5, others: IRK */
                __IO uint32_t ctr_mode:        2;
                __IO uint32_t rsvd:            14;
                __IO uint32_t dout_rdy:        1; /* aes data output ready signal used in CPU mode.
                                                 when data output is ready, dout_rdy = 1.
                                                 if dout_rdy == 1, "CPU reads enc_dout or dec_dout
                                                 for 4 times (128 bits)" will clear dout_rdy to 0.
                                               */
            } CTL_BITS;
        };
        __IO uint32_t data_in;
        __I  uint32_t enc_dout;
        __I  uint32_t dec_dout;
        /* if 256-bit key:
            IRK[0]: key[159:128]
            IRK[1]: key[191:160]
            IRK[2]: key[223:192]
            IRK[3]: key[255:224]
            IRK[4]: key[31:0]
            IRK[5]: key[63:32]
            IRK[6]: key[95:64]
            IRK[7]: key[127:96]
           if 128-bit key:
            IRK[0]: key[31:0]
            IRK[1]: key[63:32]
            IRK[2]: key[95:64]
            IRK[3]: key[127:96]
           it is handled by hw_aes.c
        */
        __IO uint32_t IRK[8];
        __IO uint32_t iv[4];
        union
        {
            __IO uint32_t reg_0x40;           /* 0x40 */
            struct
            {
                __IO uint32_t reg_aes_hide_dec_out: 1;
                __IO uint32_t rsvd: 31;
            } REG_0x40;
        };
        union
        {
            __IO uint32_t reg_0x44;           /* 0x44 */
            struct
            {
                __IO uint32_t reg_w1o_scram_en_lock: 1;
                __IO uint32_t reg_aes_hide_dec_out_lock: 1;
                __IO uint32_t rsvd: 30;
            } REG_0x44;
        };
        __IO uint32_t reg_0x48;
        __IO uint32_t reg_0x4c;
        __IO uint32_t reg_0x50;
        __IO uint32_t reg_0x54;
        __IO uint32_t reg_0x58;
        __IO uint32_t reg_0x5c;
        __IO uint32_t reg_0x60;
        __IO uint32_t reg_0x64;
        __IO uint32_t reg_0x68;
        __IO uint32_t reg_0x6c;
        __IO uint32_t reg_0x70;
        union
        {
            __IO uint32_t secure_status;      /* 0x74 */
            struct
            {
                __IO uint32_t cnt:            4;
                __IO uint32_t S:              1;
                __IO uint32_t irk_cnt:        3;
                __IO uint32_t rsvd:           8;
                __IO uint32_t hash_:          16;
            } SEC_STS;
        };
        __O  uint32_t reg_0x78;
        __IO uint32_t reg_0x7c;
        union
        {
            __IO uint32_t reg_0x80;
            struct
            {
                __IO uint32_t cmac_blk: 32;
            } REG_0x80;
        };
        union
        {
            __IO uint32_t reg_0x84;
            struct
            {
                __IO uint32_t cmac_key_sel: 1;
                __IO uint32_t rsvd: 31;
            } REG_0x84;
        };
        __IO uint32_t reg_0x88;
        __IO uint32_t reg_0x8c;
        __IO uint32_t key2[4];                /* 0x90 ~ 0x9c */
        __IO uint32_t key3[4];                /* 0xa0 ~ 0xac */
        __IO uint32_t key4[4];                /* 0xb0 ~ 0xbc */
        __IO uint32_t key5[4];                /* 0xc0 ~ 0xcc */
        union
        {
            __IO uint32_t reg_0xd0;
            struct
            {
                __IO uint32_t key2_lock: 1;
                __IO uint32_t key3_lock: 1;
                __IO uint32_t key4_lock: 1;
                __IO uint32_t key5_lock: 1;
                __IO uint32_t rsvd: 28;
            } REG_0xd0;
        };
    } HW_AES_TypeDef;

#define HWAES           ((HW_AES_TypeDef *) HWAES_BASE_ADDR)

#define HWAES_CTL       0x0
#define HWAES_DATAIN    0x4
#define HWAES_ENC_DOUT  0x8
#define HWAES_DEC_DOUT  0xC
#define HWAES_IRK       0x10
#define HWAES_IV        0x30
#define HWAES_SEC_REG   0x40
#define HWAES_SEC_STS   0x74
#define HWAES_AUTHEN    0x78

    typedef enum
    {
        AES_NONE,
        AES_CBC = 0x1,                  // 6'b00_0001
        AES_ECB = 0x2,                  // 6'b00_0010
        AES_CFB = 0x3,                  // 6'b00_0011
        AES_OFB = 0x4,                  // 6'b00_0100
        AES_CTR_IV_64BITS_MSB = 0x5,    // 6'b00_0101
        AES_CTR_IV_96BITS_MSB = 0x15,   // 6'b01_0101
        AES_CTR_IV_64BITS_LSB = 0x25,   // 6'b10_0101
        AES_CTR_IV_96BITS_LSB = 0x35,   // 6'b11_0101
        AES_CIPHER_MAC = 0x6            // 6'b00_0110
    } HW_AES_WORK_MODE;

    typedef enum
    {
        HW_AES_CPU_MODE,
        HW_AES_DMA_MODE
    } HW_AES_ACCESS_MODE;

    typedef enum
    {
        CTR_IV_64BITS_MSB = 0,
        CTR_IV_96BITS_MSB = 1,
        CTR_IV_64BITS_LSB = 2,
        CTR_IV_96BITS_LSB = 3,
    } HW_AES_CTR_MODE;

    typedef enum
    {
        CMAC_KEY1,
        CMAC_KEY2,
    } CMAC_KEY_SEL;

    typedef enum
    {
        AES_KEY_IRK = 0,
        AES_KEY2    = 1,
        AES_KEY3    = 2,
        AES_KEY4    = 3,
        AES_KEY5    = 4,
        AES_HIDDEN_KEY,
        AES_KEY_MAX,
    } AES_KEY_SEL;

    typedef enum
    {
        AES_KEY_BITS_128 = 128,
        AES_KEY_BITS_256 = 256,
    } AES_KEY_BITS_SEL;

// *INDENT-OFF*
#define HW_AES_SET_ENC_EN(isEnable)         (HWAES->CTL_BITS.enc_en = (isEnable))
#define HW_AES_SET_DEC_EN(isEnable)         (HWAES->CTL_BITS.dec_en = (isEnable))
#define HW_AES_SET_256_EN(isEnable)         (HWAES->CTL_BITS.ase256_en = (isEnable))
#define HW_AES_GET_256_EN                   (HWAES->CTL_BITS.ase256_en)
#define HW_AES_SET_AES_MODE(mode)           (HWAES->CTL_BITS.aes_mode_sel = (mode & 0x7))
#define HW_AES_GET_AES_MODE()               (HWAES->CTL_BITS.aes_mode_sel)
#define HW_AES_SET_WORK_MODE(mode)          (HWAES->CTL_BITS.access_mode = (mode & BIT0))
#define HW_AES_IS_DATA_OUT_READY            (HWAES->CTL_BITS.dout_rdy)
#define HW_AES_SET_CTR_MODE(mode)           (HWAES->CTL_BITS.ctr_mode = mode)
#define HW_AES_SET_INPUT_DATA(data)         (HWAES->data_in = (uint32_t)(data))
#define HW_AES_READ_ENC_OUTPUT(Out)         (Out = HWAES->enc_dout)
#define HW_AES_READ_DEC_OUTPUT(Out)         (Out = HWAES->dec_dout)
#define HW_AES_SET_SCRAMBLE_EN(isEnable)    (HWAES->CTL_BITS.scram_en = (isEnable))
#define HW_AES_SET_HIDE_DEC_OUT(isEnable)   (HWAES->REG_0x40.reg_aes_hide_dec_out = (isEnable))
#define HW_AES_USE_HIDDEN_KEY(isEnable)     (HWAES->CTL_BITS.use_hidden_key = (isEnable))
#define HW_AES_COPY_HIDDEN_KEY(isEnable)    (HWAES->CTL_BITS.copy_hidden_key = (isEnable))
#define HW_AES_SET_HIDDEN_256(isEnable)     (HWAES->CTL_BITS.hidden_256 = (isEnable))
#define HW_AES_LOCK_KEY2(isEnable)          (HWAES->REG_0xd0.key2_lock = (isEnable))
#define HW_AES_LOCK_KEY3(isEnable)          (HWAES->REG_0xd0.key3_lock = (isEnable))
#define HW_AES_LOCK_KEY4(isEnable)          (HWAES->REG_0xd0.key4_lock = (isEnable))
#define HW_AES_LOCK_KEY5(isEnable)          (HWAES->REG_0xd0.key5_lock = (isEnable))
#define HW_AES_SET_CMAC_BLK(blk_cnt)        (HWAES->REG_0x80.cmac_blk = blk_cnt)
#define HW_AES_SEL_CMAC_KEY(key_sel)        (HWAES->REG_0x84.cmac_key_sel = key_sel)
#define HW_AES_SEL_KEY(key_sel)             (HWAES->CTL_BITS.aes_key_sel = key_sel)
// *INDENT-ON*

#define HWAES_PAD_FIRST         (0X00000080)
#define HWCMAC_BLOCK_BIT_SIZE   (128)
#define HWCMAC_BLOCK_BYTE_SIZE  (16)

#define AES_DEBUG_LOG 0
#if (AES_DEBUG_LOG == 1)
#define AES_LOG(...) DBG_DIRECT(__VA_ARGS__)
#else
#define AES_LOG(...)
#endif

    typedef void (*AES_DMA_CB)(void *);
    typedef void (*AES_DMA_ISR)(void);

    typedef struct
    {
        uint32_t *key;
        AES_KEY_BITS_SEL key_bits;
        AES_KEY_SEL key_sel;
    } HW_AES_KEY_CFG;

    typedef struct
    {
        uint8_t dma_ch_num;
        AES_DMA_ISR dma_isr;
        AES_DMA_CB cb;
        void *cb_param;
    } HW_AES_DMA_CFG;

    typedef struct
    {
        uint8_t *in;
        uint32_t byte_len;
        HW_AES_ACCESS_MODE mode;
    } HW_AES_CMAC_CFG;

    __STATIC_INLINE void hw_aes_clear(void)
    {
        HWAES->CTL = 0;
        HWAES->reg_0x40 = 0;
    }

    __STATIC_INLINE void hw_aes_set_irk(const uint32_t *key, uint8_t cnt)
    {
        if (key == NULL)
        {
            return;
        }

        uint32_t ptr[8];

        memcpy((uint8_t *)ptr, (uint8_t *)key, cnt * 4);

        for (uint8_t i = 0; i < cnt; i ++)
        {
            HWAES->IRK[(cnt - 1) - i] = ptr[i];
        }
    }

    __STATIC_INLINE void hw_aes_get_irk(uint32_t *key, uint8_t cnt)
    {
        uint32_t ptr[8] = {};

        for (uint8_t i = 0; i < cnt; i ++)
        {
            ptr[i] = HWAES->IRK[(cnt - 1) - i];
        }

        memcpy((uint8_t *)key, (uint8_t *)ptr, cnt * 4);
    }

    __STATIC_INLINE void hw_aes_set_iv(const uint32_t *iv)
    {
        uint32_t ptr[4];

        memcpy((uint8_t *)ptr, (uint8_t *)iv, 16);

        for (uint8_t i = 0; i < 4; i ++)
        {
            HWAES->iv[3 - i] = ptr[i];
        }
    }

    __STATIC_INLINE void hw_aes_get_iv(uint32_t *iv)
    {
        uint32_t ptr[4];

        for (uint8_t i = 0; i < 4; i ++)
        {
            ptr[i] = HWAES->iv[3 - i];
        }

        memcpy((uint8_t *)iv, (uint8_t *)ptr, 16);
    }

    __STATIC_INLINE void hw_aes_set_key2(const uint32_t *key)
    {
        uint32_t ptr[4];

        memcpy((uint8_t *)ptr, (uint8_t *)key, 16);

        for (uint8_t i = 0; i < 4; i ++)
        {
            HWAES->key2[3 - i] = ptr[i];
        }
    }

    __STATIC_INLINE void hw_aes_set_key3(const uint32_t *key)
    {
        uint32_t ptr[4];

        memcpy((uint8_t *)ptr, (uint8_t *)key, 16);

        for (uint8_t i = 0; i < 4; i ++)
        {
            HWAES->key3[3 - i] = ptr[i];
        }
    }

    __STATIC_INLINE void hw_aes_set_key4(const uint32_t *key)
    {
        uint32_t ptr[4];

        memcpy((uint8_t *)ptr, (uint8_t *)key, 16);

        for (uint8_t i = 0; i < 4; i ++)
        {
            HWAES->key4[3 - i] = ptr[i];
        }
    }

    __STATIC_INLINE void hw_aes_set_key5(const uint32_t *key)
    {
        uint32_t ptr[4];

        memcpy((uint8_t *)ptr, (uint8_t *)key, 16);

        for (uint8_t i = 0; i < 4; i ++)
        {
            HWAES->key5[3 - i] = ptr[i];
        }
    }

    extern bool (*hw_aes_cpu_operate)(const uint32_t *in, uint32_t *out, uint32_t word_len,
                                      bool isEncrypt);

    void hw_aes_init(HW_AES_WORK_MODE aes_mode, const uint32_t *iv, HW_AES_KEY_CFG *key_cfg);

#if AES_SUPPORT_DMA_MODE == 1
    extern void (*hw_aes_dma_channel_init)(HW_AES_DMA_CFG *rx_cfg, HW_AES_DMA_CFG *tx_cfg);
    extern bool (*hw_aes_dma_operate)(const uint32_t *in, uint32_t *out, uint32_t word_len,
                                      bool isEncrypt);

    void hw_aes_set_dma_rx_done(bool isDone);
    bool hw_aes_is_dma_rx_done(void);
    void hw_aes_set_dma_tx_done(bool isDone);
    bool hw_aes_is_dma_tx_done(void);
    bool hw_aes_dma_done(void);
    void hw_aes_set_dma_move_src(uint32_t src);
    void hw_aes_set_dma_move_dst(uint32_t dst);
    void hw_aes_set_dma_carry_size(uint32_t size);
#endif

#if AES_SUPPORT_CMAC_MODE == 1
    extern bool (*hw_cmac_operate)(uint8_t *in, uint32_t *out, uint32_t byte_len,
                                   HW_AES_ACCESS_MODE access_mode);

    bool hw_cmac(HW_AES_CMAC_CFG *cmac_cfg, HW_AES_KEY_CFG *key_cfg, HW_AES_DMA_CFG *rx_cfg,
                 uint32_t *out);

    __STATIC_INLINE bool hw_cmac_cpu_operate(uint8_t *in, uint32_t *out, uint32_t byte_len)
    {
        return hw_cmac_operate(in, out, byte_len, HW_AES_CPU_MODE);
    }

#if AES_SUPPORT_DMA_MODE == 1
    __STATIC_INLINE bool hw_cmac_dma_operate(uint8_t *in, uint32_t *out, uint32_t byte_len)
    {
        return hw_cmac_operate(in, out, byte_len, HW_AES_DMA_MODE);
    }
#endif // AES_SUPPORT_DMA_MODE
#endif // AES_SUPPORT_CMAC_MODE

#ifdef __cplusplus
}
#endif
#endif /*__RTL8762X_GDMA_H*/
