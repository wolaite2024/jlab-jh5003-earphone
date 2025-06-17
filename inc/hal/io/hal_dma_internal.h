/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      hal_dma_internal.h
* @brief
* @details
* @author
* @date
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _HAL_DMA_
#define _HAL_DMA_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "hal_def.h"

/** @addtogroup HAL_DMA HAL DMA
  * @brief HAL DMA driver module.
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/


/** @defgroup HAL_DMA_Exported_Types HAL DMA Exported Types
  * @{
  */

/**
  * @brief  DMA channel number.
  */
typedef enum
{
    HAL_DMA_CH0,                    //!< Hal dma channel0.
    HAL_DMA_CH1,                    //!< Hal dma channel1.
    HAL_DMA_CH2,                    //!< Hal dma channel2.
    HAL_DMA_CH3,                    //!< Hal dma channel3.
    HAL_DMA_CH4,                    //!< Hal dma channel4.
    HAL_DMA_CH5,                    //!< Hal dma channel5.
    HAL_DMA_CH6,                    //!< Hal dma channel6.
    HAL_DMA_CH7,                    //!< Hal dma channel7.
#if (CHIP_DMA_CHANNEL_NUM > 8)
    HAL_DMA_CH8,                    //!< Hal dma channel8.
#endif
#if (CHIP_DMA_CHANNEL_NUM > 15)
    HAL_DMA_CH9,                    //!< Hal dma channel9.
    HAL_DMA_CH10,                   //!< Hal dma channel10.
    HAL_DMA_CH11,                   //!< Hal dma channel11.
    HAL_DMA_CH12,                   //!< Hal dma channel12.
    HAL_DMA_CH13,                   //!< Hal dma channel13.
    HAL_DMA_CH14,                   //!< Hal dma channel14.
    HAL_DMA_CH15,                   //!< Hal dma channel15.
#endif
    HAL_DMA_CH_MAX
} T_DMA_CH_NUM;

/**
  * @brief  DMA ISR event definition.
  */
typedef union
{
    struct
    {
        uint32_t DMA_ISR_EVENT_TRANSFOR_DONE   : 1;         //!< Hal dma isr event bit for transfor done.
        uint32_t DMA_ISR_EVENT_BLOCK           : 1;         //!< Hal dma isr event bit for block finish.
        uint32_t DMA_ISR_EVENT_ERROR           : 1;         //!< Hal dma isr event bit for error.
        uint32_t DMA_ISR_EVENT_RESERVED        : 29;        //!< Hal dma isr event bit reserved.
    };
    uint32_t event;     //!< Hal dma isr event.
} T_HAL_DMA_EVENT;

/**
  * @brief  Hal DMA callback.
  */
typedef void (*P_HAL_DMA_CALLBACK)(T_HAL_DMA_EVENT event, uint32_t context);

/** End of group HAL_DMA_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/



/** @defgroup HAL_DMA_Exported_Functions HAL DMA Exported Functions
  * @{
  */

/**
 * hal_dma_internal.h
 *
 * \brief   Update the DMA interrupt callback for the specific channel number.
 *
 * \param[in]  ch_num        The DMA channel number, please refer to @ref T_DMA_CH_NUM.
 * \param[in]  cb            The callback to be called when the specific interrupt happened. @ref P_HAL_DMA_CALLBACK
 * \param[in]  context       The user data when callback is called.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void dma_test(void)
 * {
 *     int8_t gdma_channel_num;
 *     uint32_t context;
 *     void hal_uart_tx_dma_handler(T_HAL_DMA_EVENT event, uint32_t context)
 *     {
 *        //add user code.
 *     }
 *     hal_dma_update_isr_cb((T_DMA_CH_NUM)gdma_channel_num, hal_uart_tx_dma_handler, context);
 * }
 * \endcode
 *
 * \ingroup  HAL_DMA_Exported_Functions
 */
void hal_dma_update_isr_cb(T_DMA_CH_NUM ch_num, P_HAL_DMA_CALLBACK cb, uint32_t context);

#ifdef __cplusplus
}
#endif

#endif /* _HAL_DMA_ */

/** @} */ /* End of group HAL_DMA_Exported_Functions */
/** @} */ /* End of group HAL_DMA */

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/

