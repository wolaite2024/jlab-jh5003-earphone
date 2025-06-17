
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if XM_XIAOAI_FEATURE_SUPPORT

#include <string.h>
#include <trace.h>

#include <xiaoai_mem.h>
#include <trace.h>

#include <xm_auth.h>


void *xm_malloc(size_t __size)
{
    return xiaoai_mem_zalloc(__size);
}

void xm_free(void *p)
{
    xiaoai_mem_free(p, 0);
}

int xm_rand(void)
{
    return rand();
}

void xm_srand(void)
{
    srand(0);
}

void   *xm_memcpy(void *str1, const void *str2, size_t n)
{
    return memcpy(str1, str2, n);
}

void   *xm_memset(void *str, int c, size_t n)
{
    return memset(str, c, n);
}

int xm_log(const char *format, ...)
{
    return 0;
}
#endif
