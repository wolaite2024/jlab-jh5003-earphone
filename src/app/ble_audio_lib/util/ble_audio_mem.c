#include "ble_audio_mem.h"
#include "ble_audio_mgr.h"
#include "trace.h"
#if LE_AUDIO_DEINIT
#include <string.h>
#endif

#if LE_AUDIO_MEM_DUMP

#define BLE_AUDIO_MEM_MAX_NUM 100
typedef struct
{
    uint32_t size;
    void *p_buffer;
} T_BLE_AUDIO_MEM;

uint32_t ble_audio_alloc_count = 0;
uint32_t ble_audio_used_size = 0;
T_BLE_AUDIO_MEM ble_audio_mem_tbl[BLE_AUDIO_MEM_MAX_NUM];

void ble_audio_mem_add(void *p_buffer, size_t size)
{
    uint8_t i;
    for (i = 0; i < BLE_AUDIO_MEM_MAX_NUM; i++)
    {
        if (ble_audio_mem_tbl[i].p_buffer == NULL)
        {
            ble_audio_mem_tbl[i].p_buffer = p_buffer;
            ble_audio_mem_tbl[i].size = size;
            break;
        }
    }
    if (i == BLE_AUDIO_MEM_MAX_NUM)
    {
        PROTOCOL_PRINT_ERROR0("ble_audio_mem_add: failed");
        return;
    }
    ble_audio_alloc_count++;
    ble_audio_used_size += size;
}

void ble_audio_mem_del(void *p_buffer)
{
    uint8_t i;
    for (i = 0; i < BLE_AUDIO_MEM_MAX_NUM; i++)
    {
        if (ble_audio_mem_tbl[i].p_buffer == p_buffer)
        {
            ble_audio_mem_tbl[i].p_buffer = NULL;
            ble_audio_used_size -= ble_audio_mem_tbl[i].size;
            ble_audio_mem_tbl[i].size = 0;
            break;
        }
    }
    if (i == BLE_AUDIO_MEM_MAX_NUM)
    {
        PROTOCOL_PRINT_ERROR1("ble_audio_mem_del: not find mem %p", p_buffer);
        return;
    }
    ble_audio_alloc_count--;
}

void ble_audio_mem_print(void)
{
    uint8_t i;

    PROTOCOL_PRINT_ERROR2("ble_audio_mem_print: ble_audio_alloc_count %d, ble_audio_used_size %d",
                          ble_audio_alloc_count, ble_audio_used_size);
    for (i = 0; i < BLE_AUDIO_MEM_MAX_NUM; i++)
    {
        if (ble_audio_mem_tbl[i].p_buffer != NULL)
        {
            PROTOCOL_PRINT_ERROR2("ble_audio_mem_print: not free mem %p, size %d",
                                  ble_audio_mem_tbl[i].p_buffer,
                                  ble_audio_mem_tbl[i].size);
        }
    }
}

void *ble_audio_mem_zalloc_intern(size_t size, const char *p_func, uint32_t file_line)
{
    void *p;

    p = calloc(1, size);
    if (p)
    {
        ble_audio_mem_add(p, size);
        PROTOCOL_PRINT_ERROR6("ble_audio_mem_a: %s<%u> required size %u, mem %p, count %u, ble_audio_used_size %d",
                              TRACE_STRING(p_func), file_line, size, p,
                              ble_audio_alloc_count, ble_audio_used_size);
    }
    else
    {
        PROTOCOL_PRINT_ERROR2("ble_audio_mem_a: %s<%u> failed",
                              TRACE_STRING(p_func), file_line);
    }

    return p;
}

void ble_audio_mem_free_intern(void *p_block, const char *p_func, uint32_t file_line)
{
    free(p_block);
    ble_audio_mem_del(p_block);
    PROTOCOL_PRINT_ERROR5("ble_audio_mem_f: %s<%u> , mem %p, count %u, , ble_audio_used_size %d",
                          TRACE_STRING(p_func), file_line, p_block,
                          ble_audio_alloc_count, ble_audio_used_size);
}

#if LE_AUDIO_DEINIT
void ble_audio_mem_deinit(void)
{
    ble_audio_mem_print();
    ble_audio_alloc_count = 0;
    ble_audio_used_size = 0;
    memset(ble_audio_mem_tbl, 0, sizeof(ble_audio_mem_tbl));
}
#endif
#endif
