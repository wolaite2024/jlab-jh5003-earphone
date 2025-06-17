/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include "trace.h"
#include "wdg.h"
#include "audio_route_cfg.h"
#include "fmc_api.h"

T_AUDIO_ROUTE_CFG audio_route_cfg;

void sys_cfg_load(void)
{
    uint32_t sync_word = 0;

    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_MCUCONFIG) +
                       AUDIO_ROUTE_CONFIG_OFFSET,
                       &sync_word, DATA_SYNC_WORD_LEN);

    if (sync_word == DATA_SYNC_WORD)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_MCUCONFIG) +
                           AUDIO_ROUTE_CONFIG_OFFSET,
                           &audio_route_cfg, sizeof(T_AUDIO_ROUTE_CFG));
    }
    else
    {
        DBG_DIRECT("sys_cfg_load: failed");
        chip_reset(RESET_ALL);
    }
}
