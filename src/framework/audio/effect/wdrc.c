/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "trace.h"
#include "audio_path.h"
#include "audio_effect.h"
#include "wdrc.h"

typedef struct t_wdrc_instance
{
    T_AUDIO_EFFECT_INSTANCE  base;
    void                    *info_buf;
    uint16_t                 info_len;
    T_WDRC_CONTENT_TYPE      type;
    bool                     enabled;
} T_WDRC_INSTANCE;

static bool wdrc_effect_xmit(T_AUDIO_EFFECT_INSTANCE   instance,
                             T_AUDIO_EFFECT_XMIT_TYPE  xmit_type,
                             void                     *context)
{
    T_WDRC_INSTANCE     *wdrc;
    T_AUDIO_PATH_HANDLE  handle;
    int32_t              ret = 0;

    wdrc = audio_effect_inheritor_get(instance);
    if (wdrc == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (wdrc->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    handle = audio_effect_owner_get(instance);
    if (handle == NULL)
    {
        ret = 3;
        goto fail_get_owner;
    }

    if (xmit_type == AUDIO_EFFECT_XMIT_TYPE_ENABLE ||
        xmit_type == AUDIO_EFFECT_XMIT_TYPE_FLUSH)
    {
        if (wdrc->info_buf != NULL &&
            wdrc->info_len != 0)
        {
            if (audio_path_wdrc_set(handle, wdrc->info_buf, wdrc->info_len) == false)
            {
                ret = 4;
                goto fail_set_wdrc;
            }
        }
    }
    else if (xmit_type == AUDIO_EFFECT_XMIT_TYPE_DISABLE)
    {
        if (audio_path_wdrc_clear(handle) == false)
        {
            ret = 5;
            goto fail_clear_wdrc;
        }
    }

    return true;

fail_clear_wdrc:
fail_set_wdrc:
fail_get_owner:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR3("wdrc_effect_xmit: failed %d, instance %p, xmit_type %u",
                       -ret, instance, xmit_type);
    return false;
}

T_AUDIO_EFFECT_INSTANCE wdrc_create(T_WDRC_CONTENT_TYPE  type,
                                    void                *info_buf,
                                    uint16_t             info_len)
{
    T_WDRC_INSTANCE *wdrc;
    int32_t          ret = 0;

    AUDIO_PRINT_TRACE3("wdrc_create: type %d info_buf %p, info_len %u",
                       type, info_buf, info_len);

    wdrc = malloc(sizeof(T_WDRC_INSTANCE));
    if (wdrc == NULL)
    {
        ret = 1;
        goto fail_alloc_wdrc;
    }

    wdrc->enabled = false;
    wdrc->type    = type;

    wdrc->base = audio_effect_create(wdrc_effect_xmit,
                                     NULL);
    if (wdrc->base == NULL)
    {
        ret = 2;
        goto fail_create_effect;
    }

    if (audio_effect_inheritor_set(wdrc->base, wdrc) == false)
    {
        ret = 3;
        goto fail_set_inheritor;
    }

    wdrc->info_buf = NULL;
    wdrc->info_len = 0;

    if (info_buf != NULL &&
        info_len != 0)
    {
        wdrc->info_buf = malloc(info_len);
        if (wdrc->info_buf == NULL)
        {
            ret = 4;
            goto fail_alloc_info;
        }

        memcpy(wdrc->info_buf, info_buf, info_len);
        wdrc->info_len = info_len;
    }

    return wdrc->base;

fail_alloc_info:
    audio_effect_inheritor_clear(wdrc->base);
fail_set_inheritor:
    audio_effect_release(wdrc->base);
fail_create_effect:
    free(wdrc);
fail_alloc_wdrc:
    AUDIO_PRINT_ERROR1("wdrc_create: failed %d", -ret);
    return NULL;
}

bool wdrc_enable(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_WDRC_INSTANCE *wdrc;
    int32_t          ret = 0;

    AUDIO_PRINT_TRACE1("wdrc_enable: instance %p", instance);

    wdrc = audio_effect_inheritor_get(instance);
    if (wdrc == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (wdrc->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (wdrc->enabled == true)
    {
        ret = 3;
        goto fail_check_state;
    }

    if (audio_effect_enable(wdrc->base) == false)
    {
        ret = 4;
        goto fail_enable_effect;
    }

    wdrc->enabled = true;
    return true;

fail_enable_effect:
fail_check_state:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("wdrc_enable: failed %d", -ret);
    return false;
}

bool wdrc_disable(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_WDRC_INSTANCE *wdrc;
    int32_t          ret = 0;

    AUDIO_PRINT_TRACE1("wdrc_disable: instance %p", instance);

    wdrc = audio_effect_inheritor_get(instance);
    if (wdrc == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (wdrc->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (wdrc->enabled == false)
    {
        ret = 3;
        goto fail_check_state;
    }

    if (audio_effect_disable(wdrc->base) == false)
    {
        ret = 4;
        goto fail_disable_effect;
    }

    wdrc->enabled = false;
    return true;

fail_disable_effect:
fail_check_state:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("wdrc_disable: failed %d", -ret);
    return false;
}

bool wdrc_set(T_AUDIO_EFFECT_INSTANCE  instance,
              void                    *info_buf,
              uint16_t                 info_len)
{
    T_WDRC_INSTANCE *wdrc;
    int32_t          ret = 0;

    AUDIO_PRINT_TRACE3("wdrc_set: instance %p, info_buf %p, info_len %u",
                       instance, info_buf, info_len);

    wdrc = audio_effect_inheritor_get(instance);
    if (wdrc == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (wdrc->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (wdrc->info_buf != NULL)
    {
        free(wdrc->info_buf);
        wdrc->info_buf = NULL;
        wdrc->info_len = 0;
    }

    if (info_buf != NULL &&
        info_len != 0)
    {
        wdrc->info_buf = malloc(info_len);
        if (wdrc->info_buf == NULL)
        {
            ret = 3;
            goto fail_alloc_info;
        }

        memcpy(wdrc->info_buf, info_buf, info_len);
        wdrc->info_len = info_len;
    }

    if (wdrc->enabled == true)
    {
        if (audio_effect_update(wdrc->base, NULL) == false)
        {
            ret = 4;
            goto fail_update_effect;
        }
    }

    return true;

fail_update_effect:
fail_alloc_info:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("wdrc_set: failed %d", -ret);
    return false;
}

bool wdrc_release(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_WDRC_INSTANCE *wdrc;
    int32_t        ret = 0;

    AUDIO_PRINT_TRACE1("wdrc_release: instance %p", instance);

    wdrc = audio_effect_inheritor_get(instance);
    if (wdrc == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (wdrc->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (audio_effect_release(wdrc->base) == false)
    {
        ret = 3;
        goto fail_release_effect;
    }

    if (wdrc->info_buf != NULL)
    {
        free(wdrc->info_buf);
    }

    free(wdrc);
    return true;

fail_release_effect:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("wdrc_release: failed %d", -ret);
    return false;
}
