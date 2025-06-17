/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_XIAOAI_OTA_FLASH_H_
#define _APP_XIAOAI_OTA_FLASH_H_

#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C"  {
#endif

#define XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT   1 //do not change it

uint32_t xiaoai_ota_write_to_flash(uint32_t dfu_base_addr, uint32_t offset, uint32_t length,
                                   void *p_void, bool is_control_header);
bool xiaoai_ota_check_image(void *p_header);
void xiaoai_ota_clear_notready_flag(void);

#ifdef __cplusplus
}
#endif

#endif
