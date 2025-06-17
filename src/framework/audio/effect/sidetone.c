/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "trace.h"
#include "audio_path.h"
#include "audio_effect.h"
#include "sidetone.h"

typedef struct t_sidetone_instance
{
    T_AUDIO_EFFECT_INSTANCE base;
    int16_t                 gain;
    T_SIDETONE_TYPE         type;
    uint8_t                 level;
    bool                    enabled;
} T_SIDETONE_INSTANCE;

static bool sidetone_effect_xmit(T_AUDIO_EFFECT_INSTANCE   instance,
                                 T_AUDIO_EFFECT_XMIT_TYPE  xmit_type,
                                 void                     *context)
{
    T_SIDETONE_INSTANCE *sidetone;
    T_AUDIO_PATH_HANDLE  handle;
    int32_t              ret = 0;

    sidetone = audio_effect_inheritor_get(instance);
    if (sidetone == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (sidetone->base != instance)
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

    AUDIO_PRINT_TRACE5("sidetone_effect_xmit: instance %p, xmit_type %u, type %d, gain %d, hpf_level %d",
                       instance, xmit_type, sidetone->type, sidetone->gain, sidetone->level);

    if (xmit_type == AUDIO_EFFECT_XMIT_TYPE_ENABLE ||
        xmit_type == AUDIO_EFFECT_XMIT_TYPE_FLUSH)
    {
        if (sidetone->type == SIDETONE_TYPE_SW)
        {
            if (audio_path_sw_sidetone_enable(handle, sidetone->gain) == false)
            {
                ret = 4;
                goto fail_set_sw_sidetone;
            }
        }
        else if (sidetone->type == SIDETONE_TYPE_HW)
        {
            if (audio_path_hw_sidetone_enable(sidetone->gain, sidetone->level) == false)
            {
                ret = 5;
                goto fail_set_hw_sidetone;
            }
        }
    }
    else if (xmit_type == AUDIO_EFFECT_XMIT_TYPE_DISABLE)
    {
        if (sidetone->type == SIDETONE_TYPE_SW)
        {
            if (audio_path_sw_sidetone_disable(handle) == false)
            {
                ret = 6;
                goto fail_clear_sw_sidetone;
            }
        }
        else if (sidetone->type == SIDETONE_TYPE_HW)
        {
            if (audio_path_hw_sidetone_disable() == false)
            {
                ret = 7;
                goto fail_clear_hw_sidetone;
            }
        }
    }

    return true;

fail_clear_hw_sidetone:
fail_clear_sw_sidetone:
fail_set_hw_sidetone:
fail_set_sw_sidetone:
fail_get_owner:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR3("sidetone_effect_xmit: failed %d, instance %p, xmit_type %u",
                       -ret, instance, xmit_type);
    return false;
}

T_AUDIO_EFFECT_INSTANCE sidetone_create(T_SIDETONE_TYPE type,
                                        int16_t         gain,
                                        uint8_t         level)
{
    T_SIDETONE_INSTANCE *sidetone;
    int32_t              ret = 0;

    AUDIO_PRINT_TRACE3("sidetone_create: type %d, gain %d, hpf_level %u",
                       type, gain, level);

    sidetone = malloc(sizeof(T_SIDETONE_INSTANCE));
    if (sidetone == NULL)
    {
        ret = 1;
        goto fail_alloc_sidetone;
    }

    sidetone->base = audio_effect_create(sidetone_effect_xmit,
                                         NULL);
    if (sidetone->base == NULL)
    {
        ret = 2;
        goto fail_create_effect;
    }

    if (audio_effect_inheritor_set(sidetone->base, sidetone) == false)
    {
        ret = 3;
        goto fail_set_inheritor;
    }

    sidetone->enabled    = false;
    sidetone->type       = type;
    sidetone->gain       = gain;
    sidetone->level      = level;

    return sidetone->base;

fail_set_inheritor:
    audio_effect_release(sidetone->base);
fail_create_effect:
    free(sidetone);
fail_alloc_sidetone:
    AUDIO_PRINT_ERROR1("sidetone_create: failed %d", -ret);
    return NULL;
}

bool sidetone_enable(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_SIDETONE_INSTANCE *sidetone;
    int32_t              ret = 0;

    AUDIO_PRINT_TRACE1("sidetone_enable: instance %p", instance);

    sidetone = audio_effect_inheritor_get(instance);
    if (sidetone == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (sidetone->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (sidetone->enabled == true)
    {
        ret = 3;
        goto fail_check_state;
    }

    if (audio_effect_enable(sidetone->base) == false)
    {
        ret = 4;
        goto fail_enable_effect;
    }

    sidetone->enabled = true;
    return true;

fail_enable_effect:
fail_check_state:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("sidetone_enable: failed %d", -ret);
    return false;
}

bool sidetone_disable(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_SIDETONE_INSTANCE *sidetone;
    int32_t              ret = 0;

    AUDIO_PRINT_TRACE1("sidetone_disable: instance %p", instance);

    sidetone = audio_effect_inheritor_get(instance);
    if (sidetone == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (sidetone->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (sidetone->enabled == false)
    {
        ret = 3;
        goto fail_check_state;
    }

    if (audio_effect_disable(sidetone->base) == false)
    {
        ret = 4;
        goto fail_disable_effect;
    }

    sidetone->enabled = false;
    return true;

fail_disable_effect:
fail_check_state:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("sidetone_disable: failed %d", -ret);
    return false;
}

bool sidetone_release(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_SIDETONE_INSTANCE *sidetone;
    int32_t              ret = 0;

    AUDIO_PRINT_TRACE1("sidetone_release: instance %p", instance);

    sidetone = audio_effect_inheritor_get(instance);
    if (sidetone == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (sidetone->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (audio_effect_release(sidetone->base) == false)
    {
        ret = 3;
        goto fail_release_effect;
    }

    free(sidetone);
    return true;

fail_release_effect:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("sidetone_release: failed %d", -ret);
    return false;
}

bool sidetone_gain_set(T_AUDIO_EFFECT_INSTANCE instance,
                       int16_t                 gain)
{
    T_SIDETONE_INSTANCE *sidetone;
    int32_t ret = 0;

    AUDIO_PRINT_TRACE2("sidetone_gain_set: instance %p, gain %d", instance, gain);

    sidetone = audio_effect_inheritor_get(instance);
    if (sidetone == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (sidetone->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    sidetone->gain = gain;

    if (sidetone->enabled == true)
    {
        if (audio_effect_update(sidetone->base, NULL) == false)
        {
            ret = 3;
            goto fail_update_effect;
        }
    }

    return true;

fail_update_effect:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("sidetone_gain_set: failed %d", -ret);
    return false;
}
