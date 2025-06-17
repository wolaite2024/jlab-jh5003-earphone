#ifndef __APP_DMA_DEVICE_H
#define __APP_DMA_DEVICE_H




#include "stdint.h"




void app_dma_device_init(void);

void app_dma_device_handle_b2s_connected(uint8_t *bd_addr);


#endif

