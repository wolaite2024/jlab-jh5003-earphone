#ifndef _BLE_AUDIO_MEM_H_
#define _BLE_AUDIO_MEM_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"

#define LE_AUDIO_MEM_DUMP 0

#if LE_AUDIO_MEM_DUMP

void *ble_audio_mem_zalloc_intern(size_t size,
                                  const char *p_func, uint32_t file_line);
void ble_audio_mem_free_intern(void *p_block, const char *p_func, uint32_t file_line);
#define ble_audio_mem_zalloc(size)   \
    ble_audio_mem_zalloc_intern(size, __func__, __LINE__)

#define ble_audio_mem_free(p_block)   \
    ble_audio_mem_free_intern(p_block, __func__, __LINE__)
#else
#define ble_audio_mem_zalloc(size) calloc(1, size)
#define ble_audio_mem_free(p_block) free(p_block)
#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
