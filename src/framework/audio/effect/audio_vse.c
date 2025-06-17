/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "trace.h"
#include "audio_path.h"
#include "audio_effect.h"
#include "audio_vse.h"

#define AUDIO_VSE_CONTEXT_REQ    0x00
#define AUDIO_VSE_CONTEXT_CFM    0x01

typedef struct t_audio_vse_context
{
    uint8_t     action;
    uint16_t    seq_num;
    uint16_t    payload_len;
    uint8_t    *payload;
} T_AUDIO_VSE_CONTEXT;

typedef struct t_audio_vse_instance
{
    T_AUDIO_EFFECT_INSTANCE base;
    uint16_t                company_id;
    uint16_t                effect_id;
    P_AUDIO_VSE_CBACK       cback;
} T_AUDIO_VSE_INSTANCE;

static bool vse_effect_xmit(T_AUDIO_EFFECT_INSTANCE   instance,
                            T_AUDIO_EFFECT_XMIT_TYPE  xmit_type,
                            void                     *context)
{
    T_AUDIO_VSE_INSTANCE *vse;
    T_AUDIO_PATH_HANDLE   handle;
    int32_t               ret = 0;

    vse = audio_effect_inheritor_get(instance);
    if (vse == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (vse->base != instance)
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

    if (xmit_type == AUDIO_EFFECT_XMIT_TYPE_ENABLE)
    {
        if (vse->cback != NULL)
        {
            vse->cback(instance,
                       AUDIO_VSE_EVENT_ENABLE,
                       0,
                       NULL,
                       0);
        }
    }
    else if (xmit_type == AUDIO_EFFECT_XMIT_TYPE_DISABLE)
    {
        if (vse->cback != NULL)
        {
            vse->cback(instance,
                       AUDIO_VSE_EVENT_DISABLE,
                       0,
                       NULL,
                       0);
        }
    }
    else if (xmit_type == AUDIO_EFFECT_XMIT_TYPE_FLUSH)
    {
        T_AUDIO_VSE_CONTEXT *flush_context;

        flush_context = context;
        if (flush_context->action == AUDIO_VSE_CONTEXT_REQ)
        {
            if (audio_path_vse_req(handle,
                                   vse->company_id,
                                   vse->effect_id,
                                   flush_context->seq_num,
                                   flush_context->payload,
                                   flush_context->payload_len) == false)
            {
                ret = 4;
                goto fail_req_vse;
            }
        }
        else if (flush_context->action == AUDIO_VSE_CONTEXT_CFM)
        {
            if (audio_path_vse_cfm(handle,
                                   vse->company_id,
                                   vse->effect_id,
                                   flush_context->seq_num,
                                   flush_context->payload,
                                   flush_context->payload_len) == false)
            {
                ret = 5;
                goto fail_cfm_vse;
            }
        }
    }

    return true;

fail_cfm_vse:
fail_req_vse:
fail_get_owner:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR3("vse_effect_xmit: failed %d, instance %p, xmit_type %u",
                       -ret, instance, xmit_type);
    return false;
}

static bool vse_effect_recv(T_AUDIO_EFFECT_INSTANCE   instance,
                            T_AUDIO_EFFECT_RECV_TYPE  recv_type,
                            uint16_t                  company_id,
                            uint16_t                  effect_id,
                            uint16_t                  seq_num,
                            uint16_t                  payload_len,
                            uint8_t                  *payload)
{
    T_AUDIO_VSE_INSTANCE *vse;
    T_AUDIO_PATH_HANDLE   handle;
    int32_t               ret = 0;

    vse = audio_effect_inheritor_get(instance);
    if (vse == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (vse->base != instance)
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

    if (vse->company_id == company_id &&
        vse->effect_id  == effect_id)
    {
        if (vse->cback != NULL)
        {
            T_AUDIO_VSE_EVENT event;

            if (recv_type == AUDIO_EFFECT_RECV_TYPE_RESPONSE)
            {
                event = AUDIO_VSE_EVENT_TRANS_RSP;
            }
            else if (recv_type == AUDIO_EFFECT_RECV_TYPE_INDICATE)
            {
                event = AUDIO_VSE_EVENT_TRANS_IND;
            }
            else if (recv_type == AUDIO_EFFECT_RECV_TYPE_NOTIFY)
            {
                event = AUDIO_VSE_EVENT_TRANS_NOTIFY;
            }

            vse->cback(instance,
                       event,
                       seq_num,
                       payload,
                       payload_len);
        }
    }

    return true;

fail_get_owner:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR3("vse_effect_recv: failed %d, instance %p, recv_type %u",
                       -ret, instance, recv_type);
    return false;
}

T_AUDIO_EFFECT_INSTANCE audio_vse_create(uint16_t          company_id,
                                         uint16_t          effect_id,
                                         P_AUDIO_VSE_CBACK cback)
{
    T_AUDIO_VSE_INSTANCE *vse;
    int32_t               ret = 0;

    vse = malloc(sizeof(T_AUDIO_VSE_INSTANCE));
    if (vse == NULL)
    {
        ret = 1;
        goto fail_alloc_vse;
    }

    vse->company_id = company_id;
    vse->effect_id  = effect_id;
    vse->cback      = cback;

    vse->base = audio_effect_create(vse_effect_xmit,
                                    vse_effect_recv);
    if (vse->base == NULL)
    {
        ret = 2;
        goto fail_create_effect;
    }

    if (audio_effect_inheritor_set(vse->base, vse) == false)
    {
        ret = 3;
        goto fail_set_inheritor;
    }

    if (audio_effect_enable(vse->base) == false)
    {
        ret = 4;
        goto fail_enable_effect;
    }

    AUDIO_PRINT_INFO3("audio_vse_create: instance %p, company_id 0x%04x, effect_id 0x%04x",
                      vse->base, company_id, effect_id);

    return vse->base;

fail_enable_effect:
fail_set_inheritor:
    (void)audio_effect_release(vse->base);
fail_create_effect:
    free(vse);
fail_alloc_vse:
    AUDIO_PRINT_ERROR1("audio_vse_create: failed %d", -ret);
    return NULL;
}

bool audio_vse_apply(T_AUDIO_EFFECT_INSTANCE  instance,
                     uint16_t                 seq_num,
                     void                    *ltv_buf,
                     uint16_t                 ltv_len)
{
    T_AUDIO_VSE_INSTANCE *vse;
    T_AUDIO_VSE_CONTEXT   context;
    int32_t               ret = 0;

    AUDIO_PRINT_INFO4("audio_vse_apply: instance %p, seq_num %u, ltv_buf %p, ltv_len %u",
                      instance, seq_num, ltv_buf, ltv_len);

    vse = audio_effect_inheritor_get(instance);
    if (vse == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (vse->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (ltv_buf == NULL ||
        ltv_len == 0)
    {
        ret = 3;
        goto fail_invalid_param;
    }

    context.action      = AUDIO_VSE_CONTEXT_REQ;
    context.seq_num     = seq_num;
    context.payload_len = ltv_len;
    context.payload     = ltv_buf;
    if (audio_effect_update(vse->base,
                            &context) == false)
    {
        ret = 4;
        goto fail_update_effect;
    }

    return true;

fail_update_effect:
fail_invalid_param:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR2("audio_vse_apply: failed %d, instance %p", -ret, instance);
    return false;
}

bool audio_vse_cfm(T_AUDIO_EFFECT_INSTANCE  instance,
                   uint16_t                 seq_num,
                   void                    *ltv_buf,
                   uint16_t                 ltv_len)
{
    T_AUDIO_VSE_INSTANCE *vse;
    T_AUDIO_VSE_CONTEXT   context;
    int32_t               ret = 0;

    AUDIO_PRINT_INFO4("audio_vse_cfm: instance %p, seq_num %u, ltv_buf %p, ltv_len %u",
                      instance, seq_num, ltv_buf, ltv_len);

    vse = audio_effect_inheritor_get(instance);
    if (vse == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (vse->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (ltv_buf == NULL ||
        ltv_len == 0)
    {
        ret = 3;
        goto fail_invalid_param;
    }

    context.action      = AUDIO_VSE_CONTEXT_CFM;
    context.seq_num     = seq_num;
    context.payload_len = ltv_len;
    context.payload     = ltv_buf;
    if (audio_effect_update(vse->base,
                            &context) == false)
    {
        ret = 4;
        goto fail_update_effect;
    }

    return true;

fail_update_effect:
fail_invalid_param:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR2("audio_vse_cfm: failed %d, instance %p", -ret, instance);
    return false;
}

bool audio_vse_release(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_AUDIO_VSE_INSTANCE *vse;
    int32_t               ret = 0;

    AUDIO_PRINT_INFO1("audio_vse_release: instance %p", instance);

    vse = audio_effect_inheritor_get(instance);
    if (vse == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }

    if (vse->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }

    if (audio_effect_disable(vse->base) == false)
    {
        ret = 3;
        goto fail_disable_effect;
    }

    if (audio_effect_release(vse->base) == false)
    {
        ret = 4;
        goto fail_release_effect;
    }

    free(vse);
    return true;

fail_release_effect:
    (void)audio_effect_enable(vse->base);
fail_disable_effect:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR2("audio_vse_release: failed %d, instance %p", -ret, instance);
    return false;
}
