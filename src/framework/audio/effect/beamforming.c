/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "trace.h"
#include "audio_path.h"
#include "audio_effect.h"
#include "beamforming.h"

typedef struct t_beamforming_instance
{
    T_AUDIO_EFFECT_INSTANCE        base;
    T_BEAMFORMING_CONTENT_TYPE     type;
    T_BEAMFORMING_DIRECTION        direction;
    bool                           enabled;
} T_BEAMFORMING_INSTANCE;

static bool beamforming_effect_xmit(T_AUDIO_EFFECT_INSTANCE   instance,
                                    T_AUDIO_EFFECT_XMIT_TYPE  xmit_type,
                                    void                     *context)
{
    T_BEAMFORMING_INSTANCE *beamforming;
    T_AUDIO_PATH_HANDLE     handle;
    int32_t                 ret = 0;

    beamforming = audio_effect_inheritor_get(instance);
    if (beamforming == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (beamforming->base != instance)
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
        if (audio_path_beamforming_set(handle, beamforming->direction) == false)
        {
            ret = 4;
            goto fail_set_beamforming;
        }
    }
    else if (xmit_type == AUDIO_EFFECT_XMIT_TYPE_DISABLE)
    {
        if (audio_path_beamforming_clear(handle) == false)
        {
            ret = 5;
            goto fail_clear_beamforming;
        }
    }

    return true;

fail_clear_beamforming:
fail_set_beamforming:
fail_get_owner:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR3("beamforming_effect_xmit: failed %d, instance %p, xmit_type %u",
                       -ret, instance, xmit_type);
    return false;
}

T_AUDIO_EFFECT_INSTANCE beamforming_create(T_BEAMFORMING_CONTENT_TYPE type,
                                           T_BEAMFORMING_DIRECTION    direction)
{
    T_BEAMFORMING_INSTANCE *beamforming;
    int32_t                 ret = 0;

    beamforming = malloc(sizeof(T_BEAMFORMING_INSTANCE));
    if (beamforming == NULL)
    {
        ret = 1;
        goto fail_alloc_beamforming;
    }

    beamforming->enabled   = false;
    beamforming->type      = type;
    beamforming->direction = direction;

    beamforming->base = audio_effect_create(beamforming_effect_xmit,
                                            NULL);
    if (beamforming->base == NULL)
    {
        ret = 2;
        goto fail_create_effect;
    }

    if (audio_effect_inheritor_set(beamforming->base, beamforming) == false)
    {
        ret = 3;
        goto fail_set_inheritor;
    }

    AUDIO_PRINT_TRACE2("beamforming_create: type %d, instance %p",
                       beamforming->type, beamforming->base);

    return beamforming->base;

fail_set_inheritor:
    audio_effect_release(beamforming->base);
fail_create_effect:
    free(beamforming);
fail_alloc_beamforming:
    AUDIO_PRINT_ERROR1("beamforming_create: failed %d", -ret);
    return NULL;
}

bool beamforming_enable(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_BEAMFORMING_INSTANCE *beamforming;
    int32_t                 ret = 0;

    AUDIO_PRINT_TRACE1("beamforming_enable: instance %p", instance);

    beamforming = audio_effect_inheritor_get(instance);
    if (beamforming == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (beamforming->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (beamforming->enabled == true)
    {
        ret = 3;
        goto fail_check_state;
    }

    if (audio_effect_enable(beamforming->base) == false)
    {
        ret = 4;
        goto fail_enable_effect;
    }

    beamforming->enabled = true;
    return true;

fail_enable_effect:
fail_check_state:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("beamforming_enable: failed %d", -ret);
    return false;
}

bool beamforming_disable(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_BEAMFORMING_INSTANCE *beamforming;
    int32_t                 ret = 0;

    AUDIO_PRINT_TRACE1("beamforming_disable: instance %p", instance);

    beamforming = audio_effect_inheritor_get(instance);
    if (beamforming == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (beamforming->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (beamforming->enabled == false)
    {
        ret = 3;
        goto fail_check_state;
    }

    if (audio_effect_disable(beamforming->base) == false)
    {
        ret = 4;
        goto fail_disable_effect;
    }

    beamforming->enabled = false;
    return true;

fail_disable_effect:
fail_check_state:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("beamforming_disable: failed %d", -ret);
    return false;
}

bool beamforming_release(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_BEAMFORMING_INSTANCE *beamforming;
    int32_t                 ret = 0;

    AUDIO_PRINT_TRACE1("beamforming_release: instance %p", instance);

    beamforming = audio_effect_inheritor_get(instance);
    if (beamforming == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (beamforming->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (audio_effect_release(beamforming->base) == false)
    {
        ret = 3;
        goto fail_release_effect;
    }

    free(beamforming);
    return true;

fail_release_effect:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("beamforming_release: failed %d", -ret);
    return false;
}
