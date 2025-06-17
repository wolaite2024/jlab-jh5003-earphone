/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "os_queue.h"
#include "audio_effect.h"

#define EFFECT_CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr)-(size_t)(&((type *)0)->member)))

typedef struct t_audio_effect
{
    struct t_audio_effect *p_next;
    T_OS_QUEUE_ELEM        list;
    void                  *owner;
    void                  *inheritor;
    P_AUDIO_EFFECT_CBACK   cback;
    bool                   enabled;
    P_AUDIO_EFFECT_XMIT    xmit;
    P_AUDIO_EFFECT_RECV    recv;
} T_AUDIO_EFFECT;

typedef struct t_audio_effect_db
{
    T_OS_QUEUE             instances;
} T_AUDIO_EFFECT_DB;

static T_AUDIO_EFFECT_DB audio_effect_db;

bool audio_effect_init(void)
{
    os_queue_init(&audio_effect_db.instances);
    return true;
}

void audio_effect_deinit(void)
{
    T_OS_QUEUE_ELEM *item;

    item = os_queue_out(&audio_effect_db.instances);
    while (item != NULL)
    {
        T_AUDIO_EFFECT *effect;

        effect = EFFECT_CONTAINER_OF(item, T_AUDIO_EFFECT, list);
        item = item->p_next;
        /* TODO release base */
        free(effect);
    }
}

T_AUDIO_EFFECT_INSTANCE audio_effect_create(P_AUDIO_EFFECT_XMIT xmit,
                                            P_AUDIO_EFFECT_RECV recv)
{
    T_AUDIO_EFFECT *effect;

    effect = malloc(sizeof(T_AUDIO_EFFECT));
    if (effect != NULL)
    {
        effect->owner     = NULL;
        effect->inheritor = NULL;
        effect->cback     = NULL;
        effect->enabled   = false;
        effect->xmit      = xmit;
        effect->recv      = recv;

        os_queue_in(&audio_effect_db.instances, &effect->list);
    }

    return effect;
}

bool audio_effect_release(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_AUDIO_EFFECT *effect;

    effect = (T_AUDIO_EFFECT *)instance;
    if (os_queue_search(&audio_effect_db.instances, &effect->list) == true)
    {
        if (effect->cback != NULL)
        {
            effect->cback(instance, AUDIO_EFFECT_EVENT_RELEASED, NULL);
        }

        os_queue_delete(&audio_effect_db.instances, &effect->list);
        free(effect);
        return true;
    }

    return false;
}

bool audio_effect_owner_set(T_AUDIO_EFFECT_INSTANCE  instance,
                            void                    *owner,
                            P_AUDIO_EFFECT_CBACK     cback)
{
    T_AUDIO_EFFECT *effect;

    effect = (T_AUDIO_EFFECT *)instance;
    if (os_queue_search(&audio_effect_db.instances, &effect->list) == true)
    {
        if (effect->owner == NULL &&
            owner         != NULL &&
            cback         != NULL)
        {
            effect->owner = owner;
            effect->cback = cback;

            effect->cback(instance, AUDIO_EFFECT_EVENT_CREATED, NULL);
            return true;
        }
    }

    return false;
}

void *audio_effect_owner_get(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_AUDIO_EFFECT *effect;

    effect = (T_AUDIO_EFFECT *)instance;
    if (os_queue_search(&audio_effect_db.instances, &effect->list) == true)
    {
        return effect->owner;
    }

    return NULL;
}

bool audio_effect_owner_clear(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_AUDIO_EFFECT *effect;

    effect = (T_AUDIO_EFFECT *)instance;
    if (os_queue_search(&audio_effect_db.instances, &effect->list) == true)
    {
        effect->owner = NULL;
        effect->cback = NULL;
        return true;
    }

    return false;
}

bool audio_effect_inheritor_set(T_AUDIO_EFFECT_INSTANCE  instance,
                                void                    *inheritor)
{
    T_AUDIO_EFFECT *effect;

    effect = (T_AUDIO_EFFECT *)instance;
    if (os_queue_search(&audio_effect_db.instances, &effect->list) == true)
    {
        if (effect->inheritor == NULL &&
            inheritor != NULL)
        {
            effect->inheritor = inheritor;
            return true;
        }
    }

    return false;
}

void *audio_effect_inheritor_get(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_AUDIO_EFFECT *effect;

    effect = (T_AUDIO_EFFECT *)instance;
    if (os_queue_search(&audio_effect_db.instances, &effect->list) == true)
    {
        return effect->inheritor;
    }

    return NULL;
}

bool audio_effect_inheritor_clear(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_AUDIO_EFFECT *effect;

    effect = (T_AUDIO_EFFECT *)instance;
    if (os_queue_search(&audio_effect_db.instances, &effect->list) == true)
    {
        effect->inheritor = NULL;
        return true;
    }

    return false;
}

bool audio_effect_enable(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_AUDIO_EFFECT *effect;

    effect = (T_AUDIO_EFFECT *)instance;
    if (os_queue_search(&audio_effect_db.instances, &effect->list) == true)
    {
        if (effect->enabled == false)
        {
            effect->enabled = true;

            if (effect->cback != NULL)
            {
                effect->cback(instance, AUDIO_EFFECT_EVENT_ENABLED, NULL);
            }
        }

        return true;
    }

    return false;
}

bool audio_effect_disable(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_AUDIO_EFFECT *effect;

    effect = (T_AUDIO_EFFECT *)instance;
    if (os_queue_search(&audio_effect_db.instances, &effect->list) == true)
    {
        if (effect->enabled == true)
        {
            if (effect->cback != NULL)
            {
                effect->cback(instance, AUDIO_EFFECT_EVENT_DISABLED, NULL);
            }

            effect->enabled = false;
        }

        return true;
    }

    return false;
}

bool audio_effect_update(T_AUDIO_EFFECT_INSTANCE instance, void *context)
{
    T_AUDIO_EFFECT *effect;

    effect = (T_AUDIO_EFFECT *)instance;
    if (os_queue_search(&audio_effect_db.instances, &effect->list) == true)
    {
        if (effect->enabled == true)
        {
            if (effect->cback != NULL)
            {
                effect->cback(instance, AUDIO_EFFECT_EVENT_UPDATED, context);
            }
        }

        return true;
    }

    return false;
}

bool audio_effect_run(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_AUDIO_EFFECT *effect;

    effect = (T_AUDIO_EFFECT *)instance;
    if (os_queue_search(&audio_effect_db.instances, &effect->list) == true)
    {
        if (effect->enabled == true)
        {
            if (effect->xmit != NULL)
            {
                return effect->xmit(instance, AUDIO_EFFECT_XMIT_TYPE_ENABLE, NULL);
            }
        }

        return true;
    }

    return false;
}

bool audio_effect_stop(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_AUDIO_EFFECT *effect;

    effect = (T_AUDIO_EFFECT *)instance;
    if (os_queue_search(&audio_effect_db.instances, &effect->list) == true)
    {
        if (effect->enabled == true)
        {
            if (effect->xmit != NULL)
            {
                return effect->xmit(instance, AUDIO_EFFECT_XMIT_TYPE_DISABLE, NULL);
            }
        }

        return true;
    }

    return false;
}

bool audio_effect_flush(T_AUDIO_EFFECT_INSTANCE  instance,
                        void                    *context)
{
    T_AUDIO_EFFECT *effect;

    effect = (T_AUDIO_EFFECT *)instance;
    if (os_queue_search(&audio_effect_db.instances, &effect->list) == true)
    {
        if (effect->enabled == true)
        {
            if (effect->xmit != NULL)
            {
                return effect->xmit(instance, AUDIO_EFFECT_XMIT_TYPE_FLUSH, context);
            }
        }

        return true;
    }

    return false;
}

bool audio_effect_response(T_AUDIO_EFFECT_INSTANCE  instance,
                           uint16_t                 company_id,
                           uint16_t                 effect_id,
                           uint16_t                 seq_num,
                           uint16_t                 payload_len,
                           uint8_t                 *payload)
{
    T_AUDIO_EFFECT *effect;

    effect = (T_AUDIO_EFFECT *)instance;
    if (os_queue_search(&audio_effect_db.instances, &effect->list) == true)
    {
        if (effect->enabled == true)
        {
            if (effect->recv != NULL)
            {
                return effect->recv(instance,
                                    AUDIO_EFFECT_RECV_TYPE_RESPONSE,
                                    company_id,
                                    effect_id,
                                    seq_num,
                                    payload_len,
                                    payload);
            }
        }

        return true;
    }

    return false;
}

bool audio_effect_indicate(T_AUDIO_EFFECT_INSTANCE  instance,
                           uint16_t                 company_id,
                           uint16_t                 effect_id,
                           uint16_t                 seq_num,
                           uint16_t                 payload_len,
                           uint8_t                 *payload)
{
    T_AUDIO_EFFECT *effect;

    effect = (T_AUDIO_EFFECT *)instance;
    if (os_queue_search(&audio_effect_db.instances, &effect->list) == true)
    {
        if (effect->enabled == true)
        {
            if (effect->recv != NULL)
            {
                return effect->recv(instance,
                                    AUDIO_EFFECT_RECV_TYPE_INDICATE,
                                    company_id,
                                    effect_id,
                                    seq_num,
                                    payload_len,
                                    payload);
            }
        }

        return true;
    }

    return false;
}

bool audio_effect_notify(T_AUDIO_EFFECT_INSTANCE  instance,
                         uint16_t                 company_id,
                         uint16_t                 effect_id,
                         uint16_t                 seq_num,
                         uint16_t                 payload_len,
                         uint8_t                 *payload)
{
    T_AUDIO_EFFECT *effect;

    effect = (T_AUDIO_EFFECT *)instance;
    if (os_queue_search(&audio_effect_db.instances, &effect->list) == true)
    {
        if (effect->enabled == true)
        {
            if (effect->recv != NULL)
            {
                return effect->recv(instance,
                                    AUDIO_EFFECT_RECV_TYPE_NOTIFY,
                                    company_id,
                                    effect_id,
                                    seq_num,
                                    payload_len,
                                    payload);
            }
        }

        return true;
    }

    return false;
}
