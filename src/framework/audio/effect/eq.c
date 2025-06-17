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
#include "eq.h"

typedef struct t_eq_instance
{
    T_AUDIO_EFFECT_INSTANCE  base;
    void                    *info_buf;
    uint16_t                 info_len;
    T_EQ_CONTENT_TYPE        type;
    bool                     enabled;
} T_EQ_INSTANCE;

static bool eq_effect_xmit(T_AUDIO_EFFECT_INSTANCE   instance,
                           T_AUDIO_EFFECT_XMIT_TYPE  xmit_type,
                           void                     *context)
{
    T_EQ_INSTANCE       *eq;
    T_AUDIO_PATH_HANDLE  handle;
    int32_t              ret = 0;

    eq = audio_effect_inheritor_get(instance);
    if (eq == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (eq->base != instance)
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

    if ((xmit_type == AUDIO_EFFECT_XMIT_TYPE_ENABLE) ||
        (xmit_type == AUDIO_EFFECT_XMIT_TYPE_FLUSH))
    {
        if (eq->info_buf != NULL &&
            eq->info_len != 0)
        {
            if ((eq->type == EQ_CONTENT_TYPE_AUDIO) ||
                (eq->type == EQ_CONTENT_TYPE_VOICE_OUT) ||
                (eq->type == EQ_CONTENT_TYPE_PASSTHROUGH) ||
                (eq->type == EQ_CONTENT_TYPE_LINE))
            {
                if (audio_path_eq_out_set(handle, eq->info_buf, eq->info_len) == false)
                {
                    ret = 4;
                    goto fail_set_eq_out;
                }
            }
            else if ((eq->type == EQ_CONTENT_TYPE_VOICE_IN) ||
                     (eq->type == EQ_CONTENT_TYPE_RECORD))
            {
                if (audio_path_eq_in_set(handle, eq->info_buf, eq->info_len) == false)
                {
                    ret = 5;
                    goto fail_set_eq_in;
                }
            }
        }
    }
    else if (xmit_type == AUDIO_EFFECT_XMIT_TYPE_DISABLE)
    {
        if ((eq->type == EQ_CONTENT_TYPE_AUDIO) ||
            (eq->type == EQ_CONTENT_TYPE_VOICE_OUT) ||
            (eq->type == EQ_CONTENT_TYPE_PASSTHROUGH) ||
            (eq->type == EQ_CONTENT_TYPE_LINE))
        {
            if (audio_path_eq_out_clear(handle) == false)
            {
                ret = 6;
                goto fail_clear_eq_out;
            }
        }
        else if ((eq->type == EQ_CONTENT_TYPE_VOICE_IN) ||
                 (eq->type == EQ_CONTENT_TYPE_RECORD))
        {
            if (audio_path_eq_in_clear(handle) == false)
            {
                ret = 7;
                goto fail_clear_eq_in;
            }
        }
    }

    return true;

fail_clear_eq_in:
fail_clear_eq_out:
fail_set_eq_in:
fail_set_eq_out:
fail_get_owner:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR3("eq_effect_xmit: failed %d, instance %p, xmit_type %u",
                       -ret, instance, xmit_type);
    return false;
}

T_AUDIO_EFFECT_INSTANCE eq_create(T_EQ_CONTENT_TYPE  type,
                                  void              *info_buf,
                                  uint16_t           info_len)
{
    T_EQ_INSTANCE *eq;
    int32_t        ret = 0;

    eq = malloc(sizeof(T_EQ_INSTANCE));
    if (eq == NULL)
    {
        ret = 1;
        goto fail_alloc_eq;
    }

    eq->enabled = false;
    eq->type    = type;

    eq->base = audio_effect_create(eq_effect_xmit,
                                   NULL);
    if (eq->base == NULL)
    {
        ret = 2;
        goto fail_create_effect;
    }

    if (audio_effect_inheritor_set(eq->base, eq) == false)
    {
        ret = 3;
        goto fail_set_inheritor;
    }

    eq->info_buf = NULL;
    eq->info_len = 0;

    if (info_buf != NULL &&
        info_len != 0)
    {
        eq->info_buf = malloc(info_len);
        if (eq->info_buf == NULL)
        {
            ret = 4;
            goto fail_alloc_info;
        }

        memcpy(eq->info_buf, info_buf, info_len);
        eq->info_len = info_len;
    }

    AUDIO_PRINT_INFO4("eq_create: instance %p, type %u, info_buf %p, info_len %u",
                      eq->base, type, info_buf, info_len);

    return eq->base;

fail_alloc_info:
    audio_effect_inheritor_clear(eq->base);
fail_set_inheritor:
    audio_effect_release(eq->base);
fail_create_effect:
    free(eq);
fail_alloc_eq:
    AUDIO_PRINT_ERROR1("eq_create: failed %d", -ret);
    return NULL;
}

bool eq_enable(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_EQ_INSTANCE *eq;
    int32_t        ret = 0;

    AUDIO_PRINT_INFO1("eq_enable: instance %p", instance);

    eq = audio_effect_inheritor_get(instance);
    if (eq == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (eq->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (eq->enabled == true)
    {
        ret = 3;
        goto fail_check_state;
    }

    if (audio_effect_enable(eq->base) == false)
    {
        ret = 4;
        goto fail_enable_effect;
    }

    eq->enabled = true;
    return true;

fail_enable_effect:
fail_check_state:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("eq_enable: failed %d", -ret);
    return false;
}

bool eq_disable(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_EQ_INSTANCE *eq;
    int32_t        ret = 0;

    AUDIO_PRINT_INFO1("eq_disable: instance %p", instance);

    eq = audio_effect_inheritor_get(instance);
    if (eq == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (eq->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (eq->enabled == false)
    {
        ret = 3;
        goto fail_check_state;
    }

    if (audio_effect_disable(eq->base) == false)
    {
        ret = 4;
        goto fail_disable_effect;
    }

    eq->enabled = false;
    return true;

fail_disable_effect:
fail_check_state:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("eq_disable: failed %d", -ret);
    return false;
}

bool eq_set(T_AUDIO_EFFECT_INSTANCE  instance,
            void                    *info_buf,
            uint16_t                 info_len)
{
    T_EQ_INSTANCE *eq;
    int32_t        ret = 0;

    AUDIO_PRINT_INFO3("eq_set: instance %p, info_buf %p, info_len %u",
                      instance, info_buf, info_len);

    eq = audio_effect_inheritor_get(instance);
    if (eq == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (eq->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (eq->info_buf != NULL &&
        info_buf != NULL &&
        eq->info_len == info_len &&
        !memcmp(eq->info_buf, info_buf, eq->info_len))
    {
        return true;
    }

    if (eq->info_buf != NULL)
    {
        free(eq->info_buf);
        eq->info_buf = NULL;
        eq->info_len = 0;
    }

    if (info_buf != NULL &&
        info_len != 0)
    {
        eq->info_buf = malloc(info_len);
        if (eq->info_buf == NULL)
        {
            ret = 3;
            goto fail_alloc_info;
        }

        memcpy(eq->info_buf, info_buf, info_len);
        eq->info_len = info_len;
    }

    if (eq->enabled == true)
    {
        if (audio_effect_update(eq->base, NULL) == false)
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
    AUDIO_PRINT_ERROR1("eq_set: failed %d", -ret);
    return false;
}

bool eq_release(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_EQ_INSTANCE *eq;
    int32_t        ret = 0;

    AUDIO_PRINT_INFO1("eq_release: instance %p", instance);

    eq = audio_effect_inheritor_get(instance);
    if (eq == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (eq->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (audio_effect_release(eq->base) == false)
    {
        ret = 3;
        goto fail_release_effect;
    }

    if (eq->info_buf != NULL)
    {
        free(eq->info_buf);
    }

    free(eq);
    return true;

fail_release_effect:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("eq_release: failed %d", -ret);
    return false;
}
