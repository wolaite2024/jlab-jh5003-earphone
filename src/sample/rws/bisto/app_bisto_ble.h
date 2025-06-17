#ifndef __APP_BISTO_BLE_H__
#define __APP_BISTO_BLE_H__


#include <stdint.h>
#include <stdbool.h>
#include "app_bisto_bt.h"

#if BISTO_FEATURE_SUPPORT
#define BISTO_GATT_SERVICE_NUM     (4)
#else
#define BISTO_GATT_SERVICE_NUM     (0)
#endif


#if BISTO_FEATURE_SUPPORT
#define BISTO_GATT_CLIENT_NUM     (2)
#else
#define BISTO_GATT_CLIENT_NUM     (0)
#endif


#define BISTO_BT_NAME_TO_LE_NAME(BT_NAME)   BT_NAME "-LE"
#define BISTO_LE_NAME BISTO_BT_NAME_TO_LE_NAME(BISTO_BT_NAME)


typedef struct
{
    bool is_adving;
} APP_BISTO_BLE_REMOTE_INFO;



void app_bisto_ble_init(void);

void app_bisto_ble_disconnect(void);

bool app_bisto_ble_adv_start(void);
bool app_bisto_ble_adv_stop(void);

void app_bisto_ble_set_name(void);


void app_bisto_ble_gatt_client_start_discovery(uint8_t conn_id);


void app_bisto_ble_set_remote_info(APP_BISTO_BLE_REMOTE_INFO *p_info);
void app_bisto_ble_get_remote_info(APP_BISTO_BLE_REMOTE_INFO *p_info);


#endif



















