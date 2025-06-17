#ifndef __APP_DMA_BLE_H
#define __APP_DMA_BLE_H


#include "app_dma_common.h"



void app_dma_ble_init(APP_DMA_TRANSPORT_CB cb);


void app_dma_ble_write(uint8_t conn_id, uint8_t *data, uint32_t data_len);


void app_dma_ble_adv_start(void);


void app_dma_ble_adv_stop(void);




#endif

