#ifndef __APP_BISTO_BT_H
#define __APP_BISTO_BT_H


#include <stdint.h>
#include <stdbool.h>

#if BISTO_FEATURE_SUPPORT
#define BISTO_SPP_CHANN_NUM             (2)
#else
#define BISTO_SPP_CHANN_NUM             (0)
#endif


#define BISTO_BT_NAME "Bisto Demo"


int32_t app_bisto_bt_init(void);


void app_bisto_bt_set_name(void);


void app_bisto_bt_handle_b2s_connected(uint8_t *bd_addr);


void app_bisto_bt_handle_b2s_disconnected(uint8_t *bd_addr);


#endif
