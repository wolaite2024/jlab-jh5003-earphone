/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file
* @brief
* @details
* @author    elliot chen ->justin
* @date      2015-05-08  ->2019-07-19
* @version   v1.0                ->V1.1
* *********************************************************************************************************
*/

#ifndef __GDMA_CHANNEL_MANAGER_H
#define __GDMA_CHANNEL_MANAGER_H



#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "vector_table.h"
#include <stdbool.h>
/** @addtogroup GDMA GDMA
  * @brief GDMA driver module
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/

typedef struct
{
    uint32_t lr;
} GDMA_ChInfoTypeDef;

#define DMA_CHANNEL_DSP_RESERVED (BIT2|BIT3|BIT4|BIT5)
#define DMA_CHANNEL_LOG_RESERVED (BIT8)

#define DMA_CH_IRQ(ChNum) ((IRQn_Type)((ChNum >= 9) ? ((ChNum >= 12) ? (GDMA0_Channel12_IRQn + ChNum - 12) : (GDMA0_Channel9_IRQn + ChNum - 9)) : (GDMA0_Channel0_IRQn + ChNum)))
#define DMA_MAX_VECTORN   IRQn_TO_VECTORn(GDMA0_Channel15_IRQn)
#define DMA_CH_VECTOR(ChNum)   IRQn_TO_VECTORn((ChNum >= 9) ? ((ChNum >= 12) ? (GDMA0_Channel12_IRQn + ChNum - 12) : (GDMA0_Channel9_IRQn + ChNum - 9)) : (GDMA0_Channel0_IRQn + ChNum))
#define DMA_VECTOR_TO_CH_NUM(vector) (dma_get_channel_number_by_vetorn(vector))
#define GDMA_TOTAL_CH_COUNT (16)

/** End of Group GDMA_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/

/** @defgroup GDMA_Exported_Constants GDMA Exported Constants
  * @{
  */


/** End of Group GDMA_Exported_Constant
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
  * @brief  request an unused DMA ch
  * @param  ch: requested channel number. if there's no unused channel, ch is set to 0xa5
  * @param  isr: requested channel's isr. if isr is NULL, ignore ram vector table update
  * @param  is_hp_ch: 1: request high performance channel, 0: normal DMA channel
  * @retval   true  status successful
  * @retval   false status fail
  */
extern bool (*GDMA_channel_request)(uint8_t *ch, void *isr, bool is_hp_ch);

/**
  * @brief  release an used DMA ch
  * @param  ch: released channel number.
  * @retval None
  */
void GDMA_channel_release(uint8_t ch);

/**
  * @brief  get assigned DMA channel mask
  * @retval mask of assigned DMA channel
  */
uint16_t GDMA_channel_get_active_mask(void);

/**
  * @brief  config mandatory allocation dma channel
  * @param  dma_fixed_mask: mask of assigned DMA channel.
  * @retval None
  */
void dma_channel_cfg(uint16_t dma_fixed_mask);

/**
  * @brief  config mandatory dsp allocation dma channel
  * @param  dsp_dma_channel_mask: mask of assigned DSP DMA channel.
    * @note   rom default set dsp dma channel 2\3\4\5, use dma_dsp_channel_set will
              reconfig dsp fixed channel, unused dma channel will release.
  * @retval None
  */
void dma_dsp_channel_set(uint32_t dsp_dma_channel_mask);

/**
  * @brief  config mandatory mcu allocation dma channel
  * @param  fixed_channel_mask: mask of assigned DMA channel.
    * @note   could called repeatedly
  * @retval refs to errno.h
  */
int32_t dma_fixed_channel_set(uint32_t fixed_channel_mask);

/**
  * @brief  Get gdma channel number by vector number
  * @param  v_num: vector number
  * @retval 0xff  wrong vector number
  * @retval num   gdma channel number
  */
uint8_t dma_get_channel_number_by_vetorn(uint32_t v_num);

#ifdef __cplusplus
}
#endif

#endif /*__GDMA_CHANNEL_MANAGER_H*/

/** @} */ /* End of group GDMA_Exported_Functions */
/** @} */ /* End of group GDMA */


/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/

