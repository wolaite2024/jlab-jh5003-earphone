#ifndef __PKE_CONFIG_H
#define __PKE_CONFIG_H

#include "rtl876x.h"
#include "pcc_reg.h"

/* config according to public key engine config form */

// Using RSA or not, would be also used in primality test and eddsa
#define PKE_RSA_MODE_SUPPORT        1

// choose 3072 or 4096 if PKE_RSA_MODE_SUPPORT is 1
#define PKE_RSA_MAX_WIDTH           (3072)

// Need to support ecc prime field or not
#define PKE_ECC_PRIME_SUPPORT       1

// Need to support ecc binary field or not
#define PKE_ECC_BIN_SUPPORT         1

// Need to support simplified ecdsa/eddsa/sm2 flow or not
// PKE_RSA_MODE_SUPPORT, PKE_ECC_PRIME_SUPPORT and PKE_ECC_BIN_SUPPORT should be 1
#define PKE_SIMPLIFY_SUPPORT        1

// Need to use the internal circuit for rng or not
// Used in Countermeasures of Side Channel Attacks
#define PKE_INTERNAL_LFSR_SUPPORT   1

// Need to support hardware key port or not
// 0: needn't to support hardware key
// 1~8: the number of hardware key
// PKE_SIMPLIFY_SUPPORT should be 1 if need hardware key port
#define PKE_HW_KEY_PORT_NUM         0

#if (PKE_SIMPLIFY_SUPPORT == 1)
#if (PKE_RSA_MODE_SUPPORT == 0) || (PKE_ECC_PRIME_SUPPORT == 0) || (PKE_ECC_BIN_SUPPORT == 0)
#error "error configure: all of PKE_RSA_MODE_SUPPORT, PKE_ECC_PRIME_SUPPORT and PKE_ECC_BIN_SUPPORT should be 1"
#endif
#endif

#if (PKE_SIMPLIFY_SUPPORT == 0) && (PKE_HW_KEY_PORT_NUM > 0)
#error "error configure: PKE_SIMPLIFY_SUPPORT and PKE_HW_KEY_PORT_NUM"
#endif

/* config to decide driver behavior */

// if user desires to use one of ecdsa/eddsa/sm2, and PKE_SIMPLIFY_SUPPORT is 0
// enable this option to support the required big number operation,
// otherwise user and comment this option to reduced some code size
#define PKE_BIGNUM_OPERATION_SUPPORT    1

#define PKE_BASE_ADDR                   (LALU_PKE_BASE)

__STATIC_INLINE void hw_pke_clock(bool enable)
{
    /* turn on hw pke clock */
    SYSBLKCTRL->u_234.BITS_234.BIT_SOC_ACTCK_PKE = enable;
    SYSBLKCTRL->u_234.BITS_234.BIT_SOC_SLPCK_PKE = enable;

    /* enable hw pke */
    SYSBLKCTRL->u_210.BITS_210.BIT_SOC_PKE_EN = enable;
}

#endif /*__PKE_CONFIG_H*/
