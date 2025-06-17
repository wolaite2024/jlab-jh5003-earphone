/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_hw_rsa.h
* @brief
* @details
* @author    eason li
* @date      2020-03-05
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef __RTL876X_PUBLIC_KEY_ENGINE_H
#define __RTL876X_PUBLIC_KEY_ENGINE_H
// *INDENT-OFF*
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "pke_config.h"

typedef struct
{
    union /* 0x00 */
    {
        __IO uint32_t Control;  /* reset value: 0x210 */
        struct
        {
            __IO uint32_t cal_precompute: 1;            /* [0] when bit 27 is 0 and this bit is 1, pke will calculate R^2 mod n */  
            __IO uint32_t IMEM_read_en: 1;              /* [1] 0: IMEM can only be read when bit[30] = 1, 
                                                               1: enable IMEM read */   
            __IO uint32_t DPA_defense_en: 1;            /* [2] 1: enable, 0: disable */  
            __IO uint32_t timing_attack_defense_en: 1;  /* [3] 1: enable, 0: disable */  
            __IO uint32_t internal_LFSR: 1;             /* [4] 1: use internal LFSR */
            __IO uint32_t curve_sel: 1;                 /* [5] 1: Montgomery curves, 0: Edwards curves */
            __IO uint32_t mode_sel: 2;                  /* [7:6] 00: ECC prime field, 01: ECC binary field
                                                                 10: RSA, 11: Edwards or Montgomery Curves */
            __IO uint32_t length: 16;                   /* [23:8] precise RSA/ECC length */
            __IO uint32_t R2_mod_N_ready: 1;            /* [24] 1: R^2 mod N already stored by user */
            __IO uint32_t byte_swap_en: 1;              /* [25] only support in MMEM */
            __IO uint32_t word_swap_en: 1;              /* [26] only support in MMEM */
            __IO uint32_t other_func_en: 1;             /* [27] 1: enable other function (not RSA/ECC), 0: disable */
            __IO uint32_t reserved: 2;                  /* [29:28] */ 
            __IO uint32_t start_pke: 1;                 /* [30] cleared automatically by PKE */
            __IO uint32_t hash_value_ready: 1;          /* [31] hash value for simplified ecdsa has initialized*/
        } Control_field;
    };

    __I uint32_t Reserved;      /* 0x04 */

    union /* 0x08 */
    {
        __IO uint32_t Status;   /* reset value: 0x100 */
        struct
        {
            __IO uint32_t operation_finish: 1;          /* [0] ECC/RSA operation finish */
            __IO uint32_t status: 7;                    /* [7:1] pke error status */
            __IO uint32_t pke_idle: 1;                  /* [8] PKE idle */
            __IO uint32_t wait_hash_init: 1;            /* [9] wait_hash_init */
            __IO uint32_t rsvd: 22;
        } Status_field;
    };
    union /* 0x0C */
    {
        __IO uint32_t Interrupt_mask;
        struct
        {
            __IO uint32_t status_mask: 8;               /* [7:0] interrupt mask for status register[7:0]
                                                         *      8'h00: interrupt is masked
                                                         *      8'hff: interrupt is not masked
                                                         *      the valid value is only 8'h00 or 8'hff
                                                         */
            __IO uint32_t wait_hash_init_mask: 1;       /* [8] mask for wait hash value interrupt */
            __IO uint32_t rsvd: 23;
        } Interrupt_mask_field;
    };

    __I uint32_t Reserved2;     /* 0x10 */

    union /* 0x14 */
    {
        __IO uint32_t Swap_base;
        struct
        {
            __IO uint32_t rsvd0: 2;
            __IO uint32_t swap_base_addr: 9;            /* [10:2] word swap base address in word(32 bits) */  
            __IO uint32_t rsvd1: 5;
            __IO uint32_t key_port_en: 1;               /* [16] 0: not enable hw key port, 1: enable hw key port */
            __IO uint32_t key_num: 3;                   /* [19:17] key sel for key 0 ~ key 7 */
            __IO uint32_t rsvd2: 12;
        } Swap_base_field;
    };
    __IO uint32_t Reserved3;    /* 0x18 */
    __I uint32_t version;       /* 0x1c */
    __IO uint32_t Reserved4[3]; /* 0x20 ~ 0x28 */
    __I uint32_t seed;          /* 0x2c: seed of internal lfsr */
} PUBLIC_KEY_ENGINE_TypeDef;

#define MMEM_OFFSET                         0x10000
#define TMEM_OFFSET                         0x20000
#define IMEM_OFFSET                         0x30000
#define MMEM_BYTE_SIZE                      0x800
#if PKE_SIMPLIFY_SUPPORT == 0
#define TMEM_BYTE_SIZE                      0x200
#else
#define TMEM_BYTE_SIZE                      0x800
#endif

#define PUBLIC_KEY_ENGINE                   ((PUBLIC_KEY_ENGINE_TypeDef *) PKE_BASE_ADDR)
#define MMEM(offset)                        (uint32_t)(PKE_BASE_ADDR + MMEM_OFFSET + offset)
#define TMEM(offset)                        (uint32_t)(PKE_BASE_ADDR + TMEM_OFFSET + offset)

#define IS_PKE_REGISTER(addr, len)          ((addr >= PKE_BASE_ADDR) && ((addr + len) < (PKE_BASE_ADDR + IMEM_OFFSET)))

#define RR_MOD_N_PRIME_OFFSET               MMEM(0x0)

/* pke function entry */
#define PKE_FUNCTION_ID_OFFSET              TMEM(0x0)
#define RTK_PUBKEY_ECC_MUL_ENTRY            (0x1)     // for ecc
#define RTK_PUBKEY_MOD_EXP_ENTRY            (0x1)     // for rsa

#define RTK_PUBKEY_MOD_MUL_ENTRY            (0x2)     // (a * b) mod n
#define RTK_PUBKEY_MOD_ADD_ENTRY            (0x3)     // (a + b) mod n
#define RTK_PUBKEY_ECDSA_MOD_PRIVATE_ENTRY  (0x4)     // the function is same as 0x2
#define RTK_PUBKEY_K_INV_ENTRY              (0x7)     // (a^-1) mod n
#define RTK_PUBKEY_ECC_ADD_POINT_ENTRY      (0x8)     // ECC point a add ECC point b
#define RTK_PUBKEY_SET_A_FROM_P_ENTRY       (0xa)     // ECC param A = n - 3
#define RTK_PUBKEY_MOD_MUL_BIN_ENTRY        (0xb)     // (a * b) mod n (binary field)
#define RTK_PUBKEY_MOD_XOR_ENTRY            (0xc)     // (a xor b) mod n
#define RTK_PUBKEY_X_MOD_N_ENTRY            (0xe)     // a mod n
#define RTK_PUBKEY_MOD_SUB_ENTRY            (0xf)     // (a - b) mod n
#define RTK_PUBKEY_WITNESS_ENTRY            (0x10)    // witness
#define RTK_PUBKEY_BIG_NUM_MUL_ENTRY        (0x12)    // a * b
#define RTK_PUBKEY_SIMPLIFY_ECDSA_SIGN      (0x19)
#define RTK_PUBKEY_SIMPLIFY_ECDSA_VERIFY    (0x1a)
#define RTK_PUBKEY_SIMPLIFY_EDDSA_GEN       (0x1b)
#define RTK_PUBKEY_SIMPLIFY_EDDSA_SIGN_R    (0x1c)
#define RTK_PUBKEY_SIMPLIFY_EDDSA_SIGN_S    (0x1d)
#define RTK_PUBKEY_SIMPLIFY_EDDSA_VERIFY    (0x1e)
#define RTK_PUBKEY_SIMPLIFY_SM2_SIGN        (0x1f)
#define RTK_PUBKEY_SIMPLIFY_SM2_VERIFY      (0x20)

#define RSA_OPERAND_MAX_BIT_LEN             (96*32)
#define ECC_OPERAND_MAX_BIT_LEN             (16*32)
#define ECC_OPERAND_MAX_BYTE_LEN            (ECC_OPERAND_MAX_BIT_LEN >> 3)

#define PKE_DEBUG_LOG 0
#if (PKE_DEBUG_LOG == 1)
#define PKE_LOG(...) DBG_DIRECT(__VA_ARGS__)
#else
#define PKE_LOG(...)
#endif

typedef enum
{
    ERR_NONE                = 0x0,
    ERR_PRIME               = 0x1,
    ERR_R2_MOD_N            = 0x2,
    ERR_ECC_ODD_POINT       = 0x4,
    ERR_RSA_NOT_SUPPORT     = 0x5,
    ERR_CHECK_VERSION       = 0x6,
    ERR_NO_SUCH_FUNCTION    = 0x7,
    ERR_ECC_Z               = 0x8,
    ERR_EDDSA_DECODE_FAILED = 0x9,
    ERR_MODULAR_INV         = 0x10,
    ERR_RSA_N_ST_INPUT      = 0x20,
    ERR_NO_VALID_EXP        = 0x40,
    ERR_INVALID_INPUT       = 0x80,
    ERR_INVALID_SIGNATURE   = 0xFF,
} ERR_CODE;

typedef enum
{
    ECC_PRIME_MODE       = 0,   // 3'b000
    ECC_BINARY_MODE      = 1,   // 3'b001
    RSA_MODE             = 2,   // 3'b010
    ECC_EDWARDS_CURVE    = 3,   // 3'b011
    ECC_MONTGOMERY_CURVE = 7,   // 3'b111
}PKE_MODE;

typedef struct 
{
    uint8_t input[64];
    uint32_t bit_len;
}PKE_OPERAND;

typedef struct
{
    uint8_t *modular;       /* prime number */
    uint8_t *exponent;      /* private / public key */
    uint8_t *message;
    uint32_t key_bits;      /* also equal to modular bit length */
    uint32_t exp_bit_size;
    uint32_t message_bit_size;
}EXP_MOD_CTX;

extern uint32_t hw_pke_key_bit_len;

__STATIC_INLINE void hw_pke_set_byte_swap(bool enable)
{
    PUBLIC_KEY_ENGINE->Control_field.byte_swap_en = enable;
}

__STATIC_INLINE void hw_pke_set_word_swap(bool enable, uint32_t swap_base)
{
    PUBLIC_KEY_ENGINE->Control_field.word_swap_en = enable;
    if (enable)
    {
        PUBLIC_KEY_ENGINE->Swap_base = swap_base;
    }
}

__STATIC_INLINE void hw_pke_set_cal_precompute(bool enable)
{
    PUBLIC_KEY_ENGINE->Control_field.cal_precompute = enable;
}

__STATIC_INLINE void hw_pke_set_RR_mod_N_ready(bool ready)
{
    PUBLIC_KEY_ENGINE->Control_field.R2_mod_N_ready = ready;
}

__STATIC_INLINE void hw_set_key_size(uint32_t key_bits)
{
    hw_pke_key_bit_len = key_bits & 0xFFFF;
    PUBLIC_KEY_ENGINE->Control_field.length = key_bits & 0xFFFF;
}

__STATIC_INLINE void hw_pke_set_hash_ready(void)
{
    PUBLIC_KEY_ENGINE->Control_field.hash_value_ready = 1;
}

extern void (*hw_pke_set_single_operand)(uint32_t operand_addr, const uint8_t *operands, uint32_t byte_len);
extern ERR_CODE (*hw_pke_compute)(uint16_t func_id);
extern bool (*hw_pke_get_output)(uint32_t output_addr, uint8_t *result);

void hw_pke_init(uint32_t key_bits, PKE_MODE mode);
void hw_pke_set_function_id(uint16_t func_id);
ERR_CODE hw_pke_precompute_RR_mod_N(uint8_t *prime, uint32_t prime_byte_cnt);
ERR_CODE hw_pke_check_status(void);


#ifdef __cplusplus
}
#endif
#endif /*__RTL876X_PUBLIC_KEY_ENGINE_H*/


