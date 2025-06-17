#ifndef __APP_DMA_RECORD_H
#define __APP_DMA_RECORD_H


#include "dma.pb-c.h"


void app_dma_record_init(void);


AudioFormat app_dma_record_get_format(void);


void app_dma_record_start(void);


void app_dma_record_stop(void);


#endif

