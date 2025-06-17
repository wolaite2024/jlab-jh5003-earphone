#ifndef __APP_BISTO_OTA_H
#define __APP_BISTO_OTA_H


#include "bisto_api.h"


typedef struct
{
    uint32_t len;
    uint8_t *data;
} APP_BISTO_OTA_UPDATE_DATA;


void app_bisto_ota_init(void);

bool app_bisto_ota_data_update(APP_BISTO_OTA_UPDATE_DATA *p_update_data);

BISTO_ERR app_bisto_ota_begin(char *ota_version, uint32_t image_len);


#endif






