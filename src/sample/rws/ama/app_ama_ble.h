#ifndef _APP_AMA_BLE_H_
#define _APP_AMA_BLE_H_


#include <stdbool.h>
#include "ama_ble_service.h"

T_SERVER_ID app_ama_ble_init(void);


bool app_ama_ble_adv_start(void);
bool app_ama_ble_adv_stop(void);
bool app_ama_ble_adv_on(void);


int32_t app_ama_ble_get_info_len(void);
void app_ama_ble_get_info(uint8_t *buf);
void app_ama_ble_set_mtu(uint8_t conn_id, uint16_t mtu);
int32_t app_ama_ble_set_info(uint8_t *buf);

#endif

