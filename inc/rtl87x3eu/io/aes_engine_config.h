#ifndef __AES_ENGINE_CONFIG_H
#define __AES_ENGINE_CONFIG_H

/* config to decide driver behavior */

// Using DMA mode or not
// 0: disable dma mode driver
// 1: enable dma mode driver
#define AES_SUPPORT_DMA_MODE           1

// Sipport cmac mode or not
// 0: not support cmac mode driver
// 1: support cmac mode driver
#define AES_SUPPORT_CMAC_MODE          1

#if AES_SUPPORT_DMA_MODE == 1

#define HWAES_DMA_RX_CH_NUM             (6) // need to config with dma channel
#define HWAES_DMA_TX_CH_NUM             (7) // need to config with dma channel 
#define HWAES_MAX_DMA_BUF_BYTE_SIZE     (0xFC0)
#define HWAES_DMA_RX_HANDSHAKE_PORT     (GDMA_Handshake_AES_RX) // need to config with dma handshake port
#define HWAES_DMA_TX_HANDSHAKE_PORT     (GDMA_Handshake_AES_TX) // need to config with dma handshake port
#define HWAES_APBPeriph                 (APBPeriph_GDMA)
#define HWAES_APBPeriph_Clock           (APBPeriph_GDMA_CLOCK)

#endif

#define HWAES_BASE_ADDR                 (AES_ENGINE_BASE)

__STATIC_INLINE void hw_aes_set_clk(bool is_enable)
{
    /* turn on hw aes clock */
    SYSBLKCTRL->u_238.BITS_238.BIT_SOC_ACTCK_AES_EN = is_enable;
    SYSBLKCTRL->u_238.BITS_238.BIT_SOC_SLPCK_AES_EN = is_enable;

    /* enable hw aes */
    SYSBLKCTRL->u_218.BITS_218.BIT_PERI_AES_EN = is_enable;
}

#endif /*__AES_ENGINE_CONFIG_H*/
