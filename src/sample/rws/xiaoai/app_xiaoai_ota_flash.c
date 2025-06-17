
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if XM_XIAOAI_FEATURE_SUPPORT

#include <string.h>
#include <stdint.h>
#include <trace.h>
#include <rtl876x.h>
#include "fmc_api.h"
#include "patch_header_check.h"
#include "dfu_api.h"
#include <app_xiaoai_ota_flash.h>
#include "xiaoai_mem.h"
#include "app_ota.h"
#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
#include "ota_ext.h"
#include "sha256.h"
#include "cache.h"
#else
#include "rom_ext.h"
#endif

#define READBACK_BUFFER_SIZE 64

#define XIAOAI_OTA_DEBUG_BYPASS_FLASH 0

#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
#define SHA256_LENGTH 32
#define SHA256_BUFFER_SIZE 128
bool xiaoai_ota_check_sha256(T_IMG_HEADER_FORMAT *addr)
{
#if XIAOAI_OTA_DEBUG_BYPASS_FLASH
    return true;
#else

    uint8_t sha256sum[SHA256_LENGTH];
    uint8_t sha256img[SHA256_LENGTH];
    uint32_t len;
    uint16_t i;
    uint16_t loop_cnt, remain_size;
    uint32_t p_data = (uint32_t)&addr->ctrl_header;

    len = sizeof(T_IMG_HEADER_FORMAT) - sizeof(T_AUTH_HEADER_FORMAT) + addr->ctrl_header.payload_len;

    uint8_t *buf = (uint8_t *)xiaoai_mem_zalloc(SHA256_BUFFER_SIZE);

    loop_cnt = len / SHA256_BUFFER_SIZE;
    remain_size = len % SHA256_BUFFER_SIZE;

    SHA256_CTX ctx = {0};
    SHA256_Init(&ctx);

    for (i = 0; i < loop_cnt; ++i)
    {
        fmc_flash_nor_read(p_data, buf, SHA256_BUFFER_SIZE);
        if (i == 0)
        {
            ((T_IMG_CTRL_HEADER_FORMAT *)buf)->ctrl_flag.not_ready = 0;
        }
        SHA256_Update(&ctx, buf, SHA256_BUFFER_SIZE);
        p_data += SHA256_BUFFER_SIZE;
    }
    if (remain_size)
    {
        fmc_flash_nor_read(p_data, buf, remain_size);
        if (loop_cnt == 0)
        {
            ((T_IMG_CTRL_HEADER_FORMAT *)buf)->ctrl_flag.not_ready = 0;
        }

        SHA256_Update(&ctx, buf, remain_size);
    }
    SHA256_Final(&ctx, sha256sum);

    xiaoai_mem_free(buf, SHA256_BUFFER_SIZE);

    fmc_flash_nor_read((uint32_t)&addr->auth.image_hash, sha256img, SHA256_LENGTH);

    bool ret = (memcmp(sha256img, sha256sum, SHA256_LENGTH) == 0);

    return ret;

#endif
}
#endif

#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 0)
bool xiaoai_ota_check_crc(void *p_header)
{
#if XIAOAI_OTA_DEBUG_BYPASS_FLASH
    return true;
#else

    bool with_semaphore = true;
    return dfu_check_crc((T_IMG_CTRL_HEADER_FORMAT *)p_header, with_semaphore);

#endif
}
#endif

bool xiaoai_ota_check_image(void *p_header)
{
#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
    return xiaoai_ota_check_sha256((T_IMG_HEADER_FORMAT *)p_header);
#else
    return xiaoai_ota_check_crc(p_header);
#endif
}

uint32_t xiaoai_ota_write_to_flash(uint32_t dfu_base_addr, uint32_t offset, uint32_t length,
                                   void *p_void, bool is_control_header)
{
#if XIAOAI_OTA_DEBUG_BYPASS_FLASH
    return 0;
#else
    uint8_t ret = 0;
    uint8_t readback_buffer[READBACK_BUFFER_SIZE];
    uint32_t read_back_len;
    uint32_t dest_addr;
    uint8_t *p_src = (uint8_t *)p_void;
    uint32_t remain_size = length;

    uint8_t bp_level;
#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
    fmc_flash_nor_get_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0),
                            &bp_level);
    fmc_flash_nor_set_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0),
                            0);
#else
    app_flash_get_bp_lv((uint32_t)&bp_level);
    app_flash_set_bp_lv(0);
#endif

    if (is_control_header)
    {
        T_IMG_CTRL_HEADER_FORMAT *p_header = (T_IMG_CTRL_HEADER_FORMAT *)p_void;
        p_header->ctrl_flag.not_ready = 0x1;
    }

    if (((offset + dfu_base_addr) % 0x1000) == 0)
    {
        fmc_flash_nor_erase(dfu_base_addr + offset, FMC_FLASH_NOR_ERASE_SECTOR);
    }
    else
    {
        if (((offset + dfu_base_addr) / 0x1000) != ((offset + length + dfu_base_addr) / 0x1000))
        {
            if ((offset + length + dfu_base_addr) % 0x1000)
            {
                fmc_flash_nor_erase((dfu_base_addr + offset + length) & ~(0x1000 - 1), FMC_FLASH_NOR_ERASE_SECTOR);
            }
        }
    }

    APP_PRINT_INFO3("xiaoai_ota_write_to_flash: dfu_base_addr 0x%08x, offset %d, length %d, ",
                    dfu_base_addr, offset, length);

    if (!fmc_flash_nor_write(dfu_base_addr + offset, p_void, length))
    {
        ret = 2;
    }
#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
    cache_flush_by_addr((uint32_t *)(dfu_base_addr + offset), length);
    fmc_flash_nor_set_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0),
                            bp_level);
#else
    app_flash_set_bp_lv(bp_level);
#endif
    dest_addr = dfu_base_addr + offset;
    while (remain_size)
    {
        read_back_len = (remain_size >= READBACK_BUFFER_SIZE) ? READBACK_BUFFER_SIZE : remain_size;

        if (!fmc_flash_nor_read(dest_addr, readback_buffer, read_back_len))
        {
            ret = 3;
            break;
        }

        if (memcmp(readback_buffer, p_src, read_back_len) != 0)
        {
            ret = 4;
            break;
        }

        dest_addr += read_back_len;
        p_src += read_back_len;
        remain_size -= read_back_len;
    }

    if (ret)
    {
        APP_PRINT_ERROR1("xiaoai_ota_write_to_flash: ret %d", ret);
    }

    return ret;

#endif
}

void xiaoai_ota_clear_notready_flag(void)
{
#if XIAOAI_OTA_DEBUG_BYPASS_FLASH
    return;
#else

#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
    uint8_t bp_level;
    fmc_flash_nor_get_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0), &bp_level);
    fmc_flash_nor_set_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0), 0);
    app_ota_clear_notready_flag();
    fmc_flash_nor_set_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0), bp_level);
#else
    uint32_t base_addr;
    uint16_t ctrl_flag;
    T_IMG_HEADER_FORMAT *image_header;
    uint8_t bp_level;
    IMG_ID img_max;
    T_IMG_HEADER_FORMAT *ota_header_addr;

    app_flash_get_bp_lv((uint32_t)&bp_level);
    app_flash_set_bp_lv(0);
    img_max = IMG_MAX;

    ota_header_addr = (T_IMG_HEADER_FORMAT *)get_temp_ota_bank_addr_by_img_id(IMG_OTA);

    for (IMG_ID img_id = IMG_OTA; img_id < img_max; img_id++)
    {
        base_addr = get_temp_ota_bank_addr_by_img_id(img_id);
        image_header = (T_IMG_HEADER_FORMAT *)base_addr;

        if (img_id != IMG_OTA && !HAL_READ32((uint32_t)&ota_header_addr->fsbl_size, (img_id - IMG_SBL) * 8))
        {
            continue;
        }

        if (base_addr == 0 || img_id != image_header->ctrl_header.image_id)
        {
            continue;
        }

        fmc_flash_nor_read((uint32_t)&image_header->ctrl_header.ctrl_flag.value, (uint8_t *)&ctrl_flag,
                           sizeof(ctrl_flag));
        ctrl_flag &= ~0x80; //form 1 to 0
        fmc_flash_nor_write((uint32_t)&image_header->ctrl_header.ctrl_flag.value,
                            (uint8_t *)&ctrl_flag, sizeof(ctrl_flag));
    }

    app_flash_set_bp_lv(bp_level);
#endif

#endif
}
#endif
