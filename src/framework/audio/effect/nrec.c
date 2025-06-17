/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "trace.h"
#include "audio_path.h"
#include "audio_effect.h"
#include "nrec.h"

#define NREC_LEVEL_MIN     0
#define NREC_LEVEL_MAX     15
#define NREC_LEVEL_DEFAULT 10

typedef struct t_nrec_instance
{
    T_AUDIO_EFFECT_INSTANCE base;
    T_NREC_CONTENT_TYPE     type;
    T_NREC_MODE             mode;
    uint8_t                 level;
    bool                    enabled;
} T_NREC_INSTANCE;

static bool nrec_effect_xmit(T_AUDIO_EFFECT_INSTANCE   instance,
                             T_AUDIO_EFFECT_XMIT_TYPE  xmit_type,
                             void                     *context)
{
    T_NREC_INSTANCE     *nrec;
    T_AUDIO_PATH_HANDLE  handle;
    int32_t              ret = 0;

    nrec = audio_effect_inheritor_get(instance);
    if (nrec == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (nrec->base != instance)
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
        if (audio_path_nrec_set(handle, nrec->mode, nrec->level) == false)
        {
            ret = 4;
            goto fail_set_nrec;
        }
    }
    else if (xmit_type == AUDIO_EFFECT_XMIT_TYPE_DISABLE)
    {
        if (audio_path_nrec_clear(handle) == false)
        {
            ret = 5;
            goto fail_clear_nrec;
        }
    }

    return true;

fail_clear_nrec:
fail_set_nrec:
fail_get_owner:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR3("nrec_effect_xmit: failed %d, instance %p, xmit_type %u",
                       -ret, instance, xmit_type);
    return false;
}

T_AUDIO_EFFECT_INSTANCE nrec_create(T_NREC_CONTENT_TYPE type,
                                    T_NREC_MODE         mode,
                                    uint8_t             level)
{
    T_NREC_INSTANCE *nrec;
    int32_t          ret = 0;

    nrec = malloc(sizeof(T_NREC_INSTANCE));
    if (nrec == NULL)
    {
        ret = 1;
        goto fail_alloc_nrec;
    }

    nrec->enabled = false;
    nrec->type    = type;
    nrec->mode    = mode;
    nrec->level   = level;

    nrec->base = audio_effect_create(nrec_effect_xmit,
                                     NULL);
    if (nrec->base == NULL)
    {
        ret = 2;
        goto fail_create_effect;
    }

    if (audio_effect_inheritor_set(nrec->base, nrec) == false)
    {
        ret = 3;
        goto fail_set_inheritor;
    }

    AUDIO_PRINT_TRACE2("nrec_create: type %d, instance %p", nrec->type, nrec->base);

    return nrec->base;

fail_set_inheritor:
    audio_effect_release(nrec->base);
fail_create_effect:
    free(nrec);
fail_alloc_nrec:
    AUDIO_PRINT_ERROR1("nrec_create: failed %d", -ret);
    return NULL;
}

bool nrec_enable(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_NREC_INSTANCE *nrec;
    int32_t          ret = 0;

    AUDIO_PRINT_TRACE1("nrec_enable: instance %p", instance);

    nrec = audio_effect_inheritor_get(instance);
    if (nrec == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (nrec->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (nrec->enabled == true)
    {
        ret = 3;
        goto fail_check_state;
    }

    if (audio_effect_enable(nrec->base) == false)
    {
        ret = 4;
        goto fail_enable_effect;
    }

    nrec->enabled = true;
    return true;

fail_enable_effect:
fail_check_state:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("nrec_enable: failed %d", -ret);
    return false;
}

bool nrec_disable(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_NREC_INSTANCE *nrec;
    int32_t          ret = 0;

    AUDIO_PRINT_TRACE1("nrec_disable: instance %p", instance);

    nrec = audio_effect_inheritor_get(instance);
    if (nrec == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (nrec->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (nrec->enabled == false)
    {
        ret = 3;
        goto fail_check_state;
    }

    if (audio_effect_disable(nrec->base) == false)
    {
        ret = 4;
        goto fail_disable_effect;
    }

    nrec->enabled = false;
    return true;

fail_disable_effect:
fail_check_state:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("nrec_disable: failed %d", -ret);
    return false;
}

bool nrec_release(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_NREC_INSTANCE *nrec;
    int32_t          ret = 0;

    AUDIO_PRINT_TRACE1("nrec_release: instance %p", instance);

    nrec = audio_effect_inheritor_get(instance);
    if (nrec == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (nrec->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (audio_effect_release(nrec->base) == false)
    {
        ret = 3;
        goto fail_release_effect;
    }

    free(nrec);
    return true;

fail_release_effect:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("nrec_release: failed %d", -ret);
    return false;
}
