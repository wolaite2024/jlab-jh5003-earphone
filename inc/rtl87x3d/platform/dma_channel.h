/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file
* @brief
* @details
* @author    qinyuan_hu
* @date      2024-10-11
* @version   v1.2
* *********************************************************************************************************
*/

#ifndef __GDMA_CHANNEL_MANAGER_H
#define __GDMA_CHANNEL_MANAGER_H



#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include <stdbool.h>
/** @addtogroup 87x3d_PLATFORM_GDMA DMA Channel
  * @brief DMA channel driver module.
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup 87x3d_PLATFORM_GDMA_Exported_Types DMA Channel Exported Types
  * @{
  */
typedef struct
{
    uint32_t lr;            //!< Record the function address that successfully applied for a DMA channel.
} GDMA_ChInfoTypeDef;

/** End of Group 87x3d_PLATFORM_GDMA_Exported_Types
  * @}
  */

/** @defgroup 87x3d_PLATFORM_GDMA_Exported_Constants DMA Channel Exported Constants
  * @{
  */
#define DMA_CHANNEL_DSP_RESERVED (BIT4|BIT5)        //!< Indicates the default dma channel for dsp. APP do not modify it.
#define DMA_CHANNEL_LOG_RESERVED (BIT15)            //!< Indicates the default dma channel for log. APP do not modify it.

/** End of Group 87x3d_PLATFORM_GDMA_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup 87x3d_PLATFORM_GDMA_Exported_Functions DMA Channel Exported Functions
  * @{
  */
/**
  * @brief  Request an unused DMA channel.
  * @param  ch: Requested channel number. If there's no unused channel, ch is set to 0xa5.
  * @param  isr: Requested channel's isr. If isr is NULL, ignore ram vector table update.
  * @param  is_hp_ch: 1: request high performance channel, 0: normal DMA channel.
  * @retval   true  Status successful.
  * @retval   false Status fail.
  */
bool dma_channel_request(uint8_t *ch, void *isr, bool is_hp_ch);
#define GDMA_channel_request dma_channel_request        //!< Request an unused DMA channel.
/**
  * @brief  Release an used DMA channel.
  * @param  ch: Released channel number.
  */
void dma_channel_release(uint8_t ch);
#define GDMA_channel_release dma_channel_release        //!< Release an used DMA channel.
/**
  * @brief  Get assigned DMA channel mask.
  * @return Mask of assigned DMA channel.
  */
uint16_t dma_channel_get_active_mask(void);
#define GDMA_channel_get_active_mask dma_channel_get_active_mask    //!< Get assigned DMA channel mask.

/**
  * @brief  Config mandatory DSP allocation DMA channel.
  * @param  dsp_dma_channel_mask: Mask of assigned DSP DMA channel.
  * @note   Rom default set DSP DMA channel 2\3\4\5\8, use it will
              reconfig DSP fixed channel, unused DMA channel will release.
  */
void dma_dsp_channel_set(uint32_t dsp_dma_channel_mask);

/**
  * @brief  Config mandatory MCU allocation DMA channel.
  * @param  fixed_channel_mask: Mask of assigned DMA channel.
  * @note   Could called repeatedly.
  * @return Refer to errno.h.
  */
int32_t dma_fixed_channel_set(uint32_t fixed_channel_mask);

#ifdef __cplusplus
}
#endif

#endif /*__GDMA_CHANNEL_MANAGER_H*/

/** @} */ /* End of group 87x3d_PLATFORM_GDMA_Exported_Functions */
/** @} */ /* End of group 87x3d_PLATFORM_GDMA */


/******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/

