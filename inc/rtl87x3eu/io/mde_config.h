#ifndef __MDE_CONFIG_H
#define __MDE_CONFIG_H

/* config according to hmac engine config form */

// Using internal DMA or not
// 0: This engine doses not include DMA. Data needs to be pushed by CPU(or DMA on system)
// 1: This engine includes a DMA. The internal DMA can achieves better performance
#define MDE_SUPPORT_INT_DMA_EN          0

// Support HMAC mode or not
// 0: only support hash function
// 1: support hash and hmac function
#define MDE_SUPPORT_HMAC_MODE           0

// Support SHA2 or not
// If the user wants to enable this function, please set the enable to 1
#define MDE_SHA2_CORE_EN                1

// Only available when MDE_SHA2_CORE_EN = 1
// Support SHA384 or SHA512 mode or not
// 0: SHA2 supports SHA224, SHA256
// 1: SHA2 supports SHA224, SHA256, SHA384, SHA512
#define MDE_SUPPORT_SHA512              0

// Only available when MDE_SHA2_CORE_EN = 1 and MDE_SUPPORT_SHA512 = 1
// Support SHA512/224 and SHA512/256 mode or not
// 0: only support SHA224, SHA256, SHA384, SHA512
// 1: add additional initial IV values for SHA512/224 and SHA512/256
#define MDE_SUPPORT_TRUNCATED_SHA512_EN 0

// Support SHA1 or not
// If the user wants to enable this function, please set the enable to 1
#define MDE_SHA1_CORE_EN                0

// Support SM3 or not
// If the user wants to enable this function, please set the enable to 1
#define MDE_SM3_CORE_EN                 1

// Only available when SUPPORT_HMAC_MODE = 1
// Use key manager to input key
// Key manager provides APB setup key and external source key
// The user sets the number and trigger the key manager to deliver the key to the engine
// If the user wants to enable this function, please set the enable to 1
#define KEY_MANAGER_EN                  0

#if !defined(MDE_HW_PADDING_SUPPORT) || \
    (defined(MDE_SUPPORT_HMAC_MODE) && MDE_SUPPORT_HMAC_MODE == 1)
#define MDE_HW_PADDING_SUPPORT          1
#endif

/* config to decide driver behavior */

// Using DMA mode or not
// 0: disable dma mode driver
// 1: enable dma mode driver
#define MDE_SUPPORT_DMA_MODE            1

#if MDE_SUPPORT_INT_DMA_EN == 0

#define MDE_DMA_RX_CH_NUM               6
#define MDE_MAX_DMA_BUF_BYTE_SIZE       (0xFC0)
#define MDE_DMA_HS_MSIZE                0x0
#define MDE_DMA_HS_TR_WIDTH             0x2
#define MDE_GET_SRC_BLOCK_TS(byte)      (byte)
#define MDE_EXT_DMA_HANDSHAKE_PORT      (GDMA_Handshake_SM3)// need to config with dma handshake port 

#define MDE_APBPeriph                   (APBPeriph_GDMA)
#define MDE_APBPeriph_Clock             (APBPeriph_GDMA_CLOCK)

#else
// src
#define MDE_DMA_SRC_TR_WIDTH_8BITS      (0x0)
#define MDE_DMA_SRC_ADDRESS_INCREMENT   (0x0)
#define MDE_DMA_SRC_MSIZE8              (0x2)
#define MDE_DMA_SRC_MSIZE16             (0x3)
//dst
#define MDE_DMA_DST_TR_WIDTH_8BITS      (0x0)
#define MDE_DMA_DST_ADDRESS_INCREMENT   (0x0)
#define MDE_DMA_DST_MSIZE1              (0x0)
// channel num don't care
#define MDE_DMA_RX_CH_NUM               (0x0)
#define MDE_MAX_DMA_BUF_BLOCK_SIZE      (0x1FFFFFFF)
#define MDE_MAX_DMA_BUF_BYTE_SIZE       (MDE_MAX_DMA_BUF_BLOCK_SIZE << MDE_DMA_SRC_TR_WIDTH_8BITS)
#define MDE_GET_SRC_BLOCK_TS(byte)      (byte >> MDE_DMA_SRC_TR_WIDTH_8BITS)

#define MDE_DMA_BASE_ADDR               (SHA2_DMA_BASE)
#define MDE_IRQN                        (SHA2_IRQn)

#endif

#define MDE_CTRL_BASE_ADDR              (LALU_HMAC_BASE)

__STATIC_INLINE void hw_mde_set_clock(bool is_on)
{
    /* turn on hw sha2 clock */
    SYSBLKCTRL->u_234.BITS_234.BIT_CKE_SM3 = is_on;

    /* enable hw sha2 */
    SYSBLKCTRL->u_218.BITS_218.BIT_PERI_SM3_EN = is_on;
}

#endif /*__MDE_CONFIG_H*/