/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "os_sync.h"
#include "trace.h"
#include "media_pool.h"

typedef struct t_media_pool_buffer
{
    struct t_media_pool_buffer *p_next; /* next free pool buffer */
    uint16_t                    size;   /* pool buffer size */
    uint8_t                     flags;  /* pool buffer flags */
} T_MEDIA_POOL_BUFFER;

typedef struct t_media_pool
{
    uint32_t             flags;             /* pool heap flags */
    T_MEDIA_POOL_BUFFER  pool_start;        /* pool buffer list head */
    T_MEDIA_POOL_BUFFER *p_pool_end;        /* pool buffer list tail */
    void                *heap_addr;         /* pool heap address */
    uint32_t             heap_size;         /* total available pool heap size */
    uint32_t             heap_size_unused;  /* current unused pool heap size */
    uint32_t             heap_size_mark;    /* low watermark for unused pool heap size */
    void                *next_free_block_after_get;     /* last get address */
} T_MEDIA_POOL;

//#define MEDIA_POOL_DEBUG

#define MEDIA_POOL_BUFFER_ALLOCATED    0x01
#define MEDIA_POOL_BUFFER_CALLBACK     0x02

#define POOL_HDR_SIZE           sizeof(T_MEDIA_POOL_BUFFER)

#define POOL_BUFFER_ADDR_MASK   (sizeof(void *) - 1)

#define POOL_BUFFER_ALIGN_SIZE  (1 << 3)
#define POOL_BUFFER_ALIGN_MASK  (POOL_BUFFER_ALIGN_SIZE - 1)

#define POOL_BUFFER_MINI_SIZE   (POOL_BUFFER_ALIGN_SIZE << 1)

/*
 * Media Pool 32 bit flags are divided into 4 sections.
 * bit  0 ~  7: set pool status.
 * bit  8 ~ 15: set pool memory type.
 * bit 16 ~ 23: reserved.
 * bit 24 ~ 31: set pool buffer alignment.
 */
#define MEDIA_POOL_CREATED        0x01

#define MEDIA_POOL_MEM_TYPE_SET(flag, type)   ( (flag) |= (((uint8_t)(type)) << 8)         )
#define MEDIA_POOL_MEM_TYPE_GET(flag, type)   ( (type) = (T_OS_MEM_TYPE)(((flag) >> 8) & 0xFF)  )

#define MEDIA_POOL_MEM_ALIGN_SET(flag, align) ( (flag) |= (((uint8_t)(align)) << 24)       )
#define MEDIA_POOL_MEM_ALIGN_GET(flag, align) ( (align) = (uint8_t)(((flag) >> 24) & 0xFF) )

bool media_pool_reset(T_MEDIA_POOL_HANDLE handle)
{
    T_MEDIA_POOL        *p_pool;
    uint8_t             *p_heap_start;
    uint8_t             *p_heap_end;
    uint32_t             heap_size;
    T_MEDIA_POOL_BUFFER *p_buf;
    int32_t              ret = 0;

    p_pool = (T_MEDIA_POOL *)handle;
    if (p_pool == NULL)
    {
        ret = 1;
        goto fail_invalid_pool_handle;
    }

    if (!(p_pool->flags & MEDIA_POOL_CREATED))
    {
        ret = 2;
        goto fail_invalid_pool_flag;
    }

    if (p_pool->heap_addr == NULL)
    {
        ret = 3;
        goto fail_invalid_pool_addr;
    }

    p_heap_start = p_pool->heap_addr;
    heap_size    = p_pool->heap_size;

    /* make pool heap start address alignment */
    if ((size_t)p_heap_start & POOL_BUFFER_ALIGN_MASK)
    {
        p_heap_start += POOL_BUFFER_ALIGN_SIZE - 1;
        p_heap_start  = (uint8_t *)((size_t)p_heap_start & ~POOL_BUFFER_ALIGN_MASK);
        heap_size    -= (size_t)p_heap_start - (size_t)p_pool->heap_addr;
    }

    p_pool->pool_start.p_next = (void *)p_heap_start;
    p_pool->pool_start.size   = 0;

    /* make pool heap end address alignment */
    p_heap_end  = p_heap_start + heap_size - POOL_HDR_SIZE;
    p_heap_end  = (uint8_t *)((size_t)p_heap_end & ~POOL_BUFFER_ALIGN_MASK);

    p_pool->p_pool_end          = (T_MEDIA_POOL_BUFFER *)p_heap_end;
    p_pool->p_pool_end->p_next  = NULL;
    p_pool->p_pool_end->size    = 0;

    p_buf           = (T_MEDIA_POOL_BUFFER *)p_heap_start;
    p_buf->p_next   = (T_MEDIA_POOL_BUFFER *)p_heap_end;
    p_buf->size     = (size_t)p_heap_end - (size_t)p_heap_start;

    p_pool->heap_size_unused    = p_buf->size;
    p_pool->heap_size_mark      = p_buf->size;
    p_pool->next_free_block_after_get       = NULL;

    AUDIO_PRINT_ERROR2("media_pool_reset: pool %p, size %u", p_pool, p_pool->heap_size);
    return true;

fail_invalid_pool_addr:
fail_invalid_pool_flag:
fail_invalid_pool_handle:
    AUDIO_PRINT_ERROR1("media_pool_reset: failed %d", -ret);
    return false;
}

T_MEDIA_POOL_HANDLE media_pool_create(uint16_t size)
{
    T_MEDIA_POOL        *p_pool;
    uint8_t             *p_heap_start;
    uint8_t             *p_heap_end;
    uint32_t             heap_size;
    T_MEDIA_POOL_BUFFER *p_buf;
    int32_t              ret = 0;

    p_pool = malloc(sizeof(T_MEDIA_POOL));
    if (p_pool == NULL)
    {
        ret = 1;
        goto fail_alloc_pool_header;
    }

    p_pool->heap_addr = malloc(size);
    if (p_pool->heap_addr == NULL)
    {
        ret = 2;
        goto fail_alloc_pool_heap;
    }

    p_pool->flags       = MEDIA_POOL_CREATED;
    p_pool->heap_size   = size;
    p_heap_start        = p_pool->heap_addr;
    heap_size           = p_pool->heap_size;

    /* make pool heap start address alignment */
    if ((size_t)p_heap_start & POOL_BUFFER_ALIGN_MASK)
    {
        p_heap_start += POOL_BUFFER_ALIGN_SIZE - 1;
        p_heap_start  = (uint8_t *)((size_t)p_heap_start & ~POOL_BUFFER_ALIGN_MASK);
        heap_size    -= (size_t)p_heap_start - (size_t)p_pool->heap_addr;
    }

    p_pool->pool_start.p_next = (void *)p_heap_start;
    p_pool->pool_start.size   = 0;

    /* make pool heap end address alignment */
    p_heap_end  = p_heap_start + heap_size - POOL_HDR_SIZE;
    p_heap_end  = (uint8_t *)((size_t)p_heap_end & ~POOL_BUFFER_ALIGN_MASK);

    p_pool->p_pool_end          = (T_MEDIA_POOL_BUFFER *)p_heap_end;
    p_pool->p_pool_end->p_next  = NULL;
    p_pool->p_pool_end->size    = 0;

    p_buf           = (T_MEDIA_POOL_BUFFER *)p_heap_start;
    p_buf->p_next   = (T_MEDIA_POOL_BUFFER *)p_heap_end;
    p_buf->size     = (size_t)p_heap_end - (size_t)p_heap_start;

    p_pool->heap_size_unused = p_buf->size;
    p_pool->heap_size_mark   = p_buf->size;
    p_pool->next_free_block_after_get    = NULL;

    return (T_MEDIA_POOL_HANDLE)p_pool;

fail_alloc_pool_heap:
    free(p_pool);
fail_alloc_pool_header:
    AUDIO_PRINT_ERROR1("media_pool_create: failed %d", -ret);
    return NULL;
}

void media_pool_destroy(T_MEDIA_POOL_HANDLE handle)
{
    T_MEDIA_POOL *p_pool;

    AUDIO_PRINT_ERROR1("media_pool_destroy: handle %p", handle);

    p_pool = (T_MEDIA_POOL *)handle;
    if (p_pool != NULL)
    {
        if (p_pool->heap_addr != NULL)
        {
            free(p_pool->heap_addr);
            p_pool->heap_addr = NULL;
        }

        free(p_pool);
    }
}

void *media_buffer_get(T_MEDIA_POOL_HANDLE handle,
                       uint16_t            buf_size)
{
    T_MEDIA_POOL        *p_pool;
    void                *p_buf;
    T_MEDIA_POOL_BUFFER *p_prev_hdr;
    T_MEDIA_POOL_BUFFER *p_curr_hdr;
    uint32_t             s;
    int32_t              ret = 0;

    p_pool = (T_MEDIA_POOL *)handle;
    if (p_pool == NULL)
    {
        ret = 1;
        goto fail_invalid_pool_handle;
    }

    s = os_lock();

    if (!(p_pool->flags & MEDIA_POOL_CREATED))
    {
        os_unlock(s);
        ret = 2;
        goto fail_invalid_pool_flag;
    }

    if (buf_size > 0)
    {
        buf_size += POOL_HDR_SIZE;

        if (buf_size & POOL_BUFFER_ALIGN_MASK)
        {
            buf_size = (buf_size + POOL_BUFFER_ALIGN_SIZE - 1) & ~POOL_BUFFER_ALIGN_MASK;
        }
    }

    if (buf_size == 0 || buf_size > p_pool->heap_size_unused)
    {
        os_unlock(s);
        ret = 3;
        goto fail_invalid_buf_size;
    }

    p_curr_hdr = NULL;
    p_prev_hdr = NULL;
    if (p_pool->next_free_block_after_get != NULL &&
        p_pool->next_free_block_after_get != p_pool->p_pool_end)
    {
        /*check the ptr*/
        for (p_curr_hdr = p_pool->pool_start.p_next, p_prev_hdr = &p_pool->pool_start;
             p_curr_hdr != p_pool->p_pool_end;
             p_curr_hdr = p_curr_hdr->p_next)
        {
            if (p_curr_hdr == p_pool->next_free_block_after_get)
            {
                if (p_curr_hdr->size < buf_size)
                {
                    p_curr_hdr = NULL;
                    p_prev_hdr = NULL;
                    p_pool->next_free_block_after_get = NULL;
                }
                break;
            }
            p_prev_hdr = p_curr_hdr;
        }
        if (p_curr_hdr == p_pool->p_pool_end)
        {
            p_curr_hdr = NULL;
            p_prev_hdr = NULL;
            p_pool->next_free_block_after_get = NULL;
        }
    }

    if (p_curr_hdr == NULL)
    {
        p_prev_hdr = &p_pool->pool_start;
        p_curr_hdr = p_pool->pool_start.p_next;

        while (p_curr_hdr->size < buf_size && p_curr_hdr->p_next != NULL)
        {
            p_prev_hdr = p_curr_hdr;
            p_curr_hdr = p_curr_hdr->p_next;
        }
    }

    if (p_curr_hdr == p_pool->p_pool_end)
    {
        os_unlock(s);
        ret = 4;
        goto fail_no_buf_size;
    }

    p_buf = (uint8_t *)p_prev_hdr->p_next + POOL_HDR_SIZE;

    if (p_curr_hdr->size - buf_size >= POOL_BUFFER_MINI_SIZE)
    {
        T_MEDIA_POOL_BUFFER *p_tmp_hdr;

        p_tmp_hdr = (T_MEDIA_POOL_BUFFER *)((uint8_t *)p_prev_hdr->p_next + buf_size);
        p_tmp_hdr->p_next = p_curr_hdr->p_next;
        p_tmp_hdr->size   = p_curr_hdr->size - buf_size;
        p_tmp_hdr->flags  = 0;

        p_prev_hdr->p_next = p_tmp_hdr;
        p_curr_hdr->size   = buf_size;

        p_pool->heap_size_unused -= buf_size;
    }
    else
    {
        p_prev_hdr->p_next = p_curr_hdr->p_next;
        p_pool->heap_size_unused -= p_curr_hdr->size;
    }

    p_curr_hdr->p_next  = NULL;
    p_curr_hdr->flags   = MEDIA_POOL_BUFFER_ALLOCATED;
    if (p_pool->heap_size_unused < p_pool->heap_size_mark)
    {
        p_pool->heap_size_mark = p_pool->heap_size_unused;
    }

    p_pool->next_free_block_after_get = p_prev_hdr->p_next;
    os_unlock(s);

#ifdef MEDIA_POOL_DEBUG
    AUDIO_PRINT_TRACE4("media_buffer_get: dump p_pool %p, size %u, buf %p, next_free_after_get %p",
                       p_pool, buf_size, p_buf, p_pool->next_free_block_after_get);
    media_pool_dump(p_pool);
#endif

    return p_buf;

fail_no_buf_size:
fail_invalid_buf_size:
fail_invalid_pool_flag:
fail_invalid_pool_handle:
    AUDIO_PRINT_ERROR1("media_buffer_get: failed %d", -ret);
    return NULL;
}

bool media_buffer_put(T_MEDIA_POOL_HANDLE  handle,
                      void                *p_buf)
{
    T_MEDIA_POOL        *p_pool;
    T_MEDIA_POOL_BUFFER *p_hdr;
    T_MEDIA_POOL_BUFFER *p_curr_hdr;
    uint32_t             s;
    int32_t              ret = 0;

    p_pool = (T_MEDIA_POOL *)handle;
    if (p_pool == NULL)
    {
        ret = 1;
        goto fail_invalid_pool_handle;
    }

    s = os_lock();

    if (!(p_pool->flags & MEDIA_POOL_CREATED))
    {
        os_unlock(s);
        ret = 2;
        goto fail_invalid_pool_flag;
    }

    if ((p_buf == NULL) || ((size_t)p_buf & POOL_BUFFER_ADDR_MASK))
    {
        os_unlock(s);
        ret = 3;
        goto fail_invalid_pool_buf;
    }

    p_hdr = (void *)((uint8_t *)p_buf - POOL_HDR_SIZE);

#ifdef MEDIA_POOL_DEBUG
    AUDIO_PRINT_TRACE4("media_buffer_put: pool %p, buf %p, size %u, next_free_after_get %p",
                       p_pool, p_buf, p_hdr->size, p_pool->next_free_block_after_get);
#endif

    if (!(p_hdr->flags & MEDIA_POOL_BUFFER_ALLOCATED))
    {
        os_unlock(s);
        ret = 4;
        goto fail_double_free_pool;
    }

    p_hdr->flags = 0;
    p_pool->heap_size_unused += p_hdr->size;

    p_curr_hdr = &p_pool->pool_start;
    while (p_curr_hdr->p_next < p_hdr)
    {
        p_curr_hdr = p_curr_hdr->p_next;
    }

    /* merge left contiguous buffers */
    if ((uint8_t *)p_curr_hdr + p_curr_hdr->size == (uint8_t *)p_hdr)
    {
        p_curr_hdr->size += p_hdr->size;
        p_hdr = p_curr_hdr;
    }

    /* merge right contiguous buffers */
    if ((uint8_t *)p_hdr + p_hdr->size == (uint8_t *)p_curr_hdr->p_next)
    {
        if (p_curr_hdr->p_next != p_pool->p_pool_end)
        {
            p_hdr->size   += p_curr_hdr->p_next->size;
            p_hdr->p_next  = p_curr_hdr->p_next->p_next;
        }
        else
        {
            p_hdr->p_next = p_pool->p_pool_end;
        }
        /* update next_free_block_after_get ptr */
        if (p_pool->next_free_block_after_get == p_curr_hdr->p_next)
        {
            p_pool->next_free_block_after_get = p_hdr;
        }
    }
    else
    {
        p_hdr->p_next = p_curr_hdr->p_next;
    }

    /* no left merge */
    if (p_curr_hdr != p_hdr)
    {
        p_curr_hdr->p_next = p_hdr;
    }

    os_unlock(s);

#ifdef MEDIA_POOL_DEBUG
    media_pool_dump(p_pool);
#endif

    return true;

fail_double_free_pool:
fail_invalid_pool_buf:
fail_invalid_pool_flag:
fail_invalid_pool_handle:
    AUDIO_PRINT_ERROR1("media_buffer_put: failed %d", -ret);
    return false;
}

void media_pool_dump(T_MEDIA_POOL_HANDLE handle)
{
    T_MEDIA_POOL        *p_pool;
    T_MEDIA_POOL_BUFFER *p_hdr;
    uint32_t             s;

    p_pool = (T_MEDIA_POOL *)handle;

    s = os_lock();

    if (p_pool != NULL)
    {
        p_hdr = p_pool->pool_start.p_next;
        while (p_hdr != NULL)
        {
            AUDIO_PRINT_ERROR4("Pool heap free list: hdr %p, next %p, size %u, flags 0x%01x",
                               p_hdr, p_hdr->p_next, p_hdr->size, p_hdr->flags);

            p_hdr = p_hdr->p_next;
        }
    }

    os_unlock(s);
}
