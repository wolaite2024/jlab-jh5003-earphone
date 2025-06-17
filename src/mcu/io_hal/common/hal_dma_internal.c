/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     hal_dma_internal.c
* @brief    This file provides all the DMA hal functions.
* @details
* @author
* @date
* @version  v0.1
*********************************************************************************************************
*/

#include "rtl876x_gdma.h"
#include "vector_table.h"
#include "hal_dma_internal.h"

#if defined(IC_TYPE_RTL87x3EU)
#include "dma_channel.h"
#endif

#define IS_INVALID_DMA_CH_NUM(ch_num)       ((ch_num) >= HAL_DMA_CH_MAX)

static uint32_t user_data[HAL_DMA_CH_MAX];
P_HAL_DMA_CALLBACK dma_isr_cb[HAL_DMA_CH_MAX];

static void dma_isr(void)
{
    uint32_t ipsr = __get_IPSR();
    T_HAL_DMA_EVENT dma_isr_event = {.event = 0};

    if ((ipsr <=  DMA_MAX_VECTORN) && (ipsr >= GDMA0_Channel0_VECTORn))
    {
        uint8_t ch_num = DMA_VECTOR_TO_CH_NUM(ipsr);
        if (GDMA_GetTransferINTStatus(ch_num))
        {
            dma_isr_event.DMA_ISR_EVENT_TRANSFOR_DONE = 1;
        }
        GDMA_ClearAllTypeINT(ch_num);

        if (dma_isr_cb[ch_num])
        {
            dma_isr_cb[ch_num](dma_isr_event, user_data[ch_num]);
        }
    }
}

void hal_dma_update_isr_cb(T_DMA_CH_NUM ch_num, P_HAL_DMA_CALLBACK cb, uint32_t context)
{
    if (IS_INVALID_DMA_CH_NUM(ch_num))
    {
        return;
    }

    dma_isr_cb[ch_num] = cb;
    user_data[ch_num] = context;
    RamVectorTableUpdate(DMA_CH_VECTOR(ch_num), dma_isr);
}

