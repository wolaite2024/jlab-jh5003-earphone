/*
 * Copyright (c) 2020, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_TEAMS_FEATURE_SUPPORT
#include "trace.h"
#include <stdint.h>
#include "ftl_ext.h"
#include "ftl.h"
#include "storage.h"
#include "flash_map.h"
#include "bt_ext_ftl.h"
#define APP_TEAMS_EXT_FTL_PARTITION_NAME "TEAMS_EXT_FTL"

#if (FTL_POOL_ENABLE == 1)
#define TEAMS_SIZE     (0x440)
#else
uint16_t app_teams_ext_ftl_start_offset = 0;
uint16_t app_teams_ext_ftl_max_offset = 0;
#endif

uint32_t app_teams_ext_save_ftl_data(uint32_t offset, uint8_t *data, uint32_t len)
{
    return storage_write(APP_TEAMS_EXT_FTL_PARTITION_NAME, offset, len, data, NULL, NULL);
}

uint32_t app_teams_ext_load_ftl_data(uint32_t offset, uint8_t *data, uint32_t len)
{
    return storage_read(APP_TEAMS_EXT_FTL_PARTITION_NAME, offset, len, data, NULL, NULL);
}

int32_t app_teams_ext_ftl_partition_init(const T_STORAGE_PARTITION_INFO *info)
{
#if (FTL_POOL_ENABLE == 1)
    return ftl_init_module(APP_TEAMS_EXT_FTL_PARTITION_NAME, TEAMS_SIZE, 4);
#else
    return ftl_partition_init(info->address, TEAMS_EXT_FTL_SIZE / 0x1000, 4,
                              &app_teams_ext_ftl_start_offset,
                              &app_teams_ext_ftl_max_offset);
#endif
}

int32_t app_teams_ext_ftl_partition_write(const T_STORAGE_PARTITION_INFO *info, uint32_t offset,
                                          uint32_t len,
                                          void *buf)
{
#if (FTL_POOL_ENABLE == 1)
    return ftl_save_to_module(APP_TEAMS_EXT_FTL_PARTITION_NAME, (void *)buf, offset, len);
#else
    return ftl_save_to_storage((void *)buf, offset + app_teams_ext_ftl_start_offset, len);
#endif
}

int32_t app_teams_ext_ftl_partition_read(const T_STORAGE_PARTITION_INFO *info, uint32_t offset,
                                         uint32_t len,
                                         void *buf)
{
#if (FTL_POOL_ENABLE == 1)
    return ftl_load_from_module(APP_TEAMS_EXT_FTL_PARTITION_NAME, (void *)buf, offset, len);
#else
    return ftl_load_from_storage((void *)buf, offset + app_teams_ext_ftl_start_offset, len);
#endif
}

static const T_STORAGE_PARTITION_INFO  app_teams_ext_partitions =
{
    .name = APP_TEAMS_EXT_FTL_PARTITION_NAME,
#if (FTL_POOL_ENABLE == 1)
    .address = NULL,
    .size = NULL,
#elif (defined(TEAMS_EXT_FTL_ADDR))
    .address = TEAMS_EXT_FTL_ADDR,
    .size = TEAMS_EXT_FTL_SIZE,
#endif
    .perm = STORAGE_PERMISSION_READ | STORAGE_PERMISSION_WRITE,
    .image_id = 0,
    .media_type = STORAGE_MEDIA_TYPE_NOR,
    .content_type = STORAGE_CONTENT_TYPE_RW_DATA,
    .init = app_teams_ext_ftl_partition_init,
    .read = app_teams_ext_ftl_partition_read,
    .write = app_teams_ext_ftl_partition_write,
    .erase = NULL,
    .async_read = NULL,
    .async_write = NULL,
    .async_erase = NULL,
};
bool app_teams_ext_ftl_init(void)
{
#if ((defined(TEAMS_EXT_FTL_ADDR)) || (FTL_POOL_ENABLE == 1))
    if (storage_partition_init(&app_teams_ext_partitions, 1) == 0)
    {
        return true;
    }
#endif
    APP_PRINT_ERROR0("app_teams_ext_ftl_init:failed");
    return false;
}


#endif

