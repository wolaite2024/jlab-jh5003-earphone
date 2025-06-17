/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if (CONFIG_REALTEK_AM_KWS_SUPPORT == 1)
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "os_queue.h"
#include "trace.h"
#include "audio_type.h"
#include "audio.h"
#include "audio_mgr.h"
#include "audio_path.h"
#include "audio_effect.h"
#include "kws.h"

#define KWS_VOLUME_ADC_LEVEL_MIN        (0)
#define KWS_VOLUME_ADC_LEVEL_MAX        (15)
#define KWS_VOLUME_ADC_LEVEL_DEFAULT    (10)

typedef struct t_kws_effect_instance
{
    T_AUDIO_EFFECT_INSTANCE   base;
    T_KWS_EFFECT_CONTENT_TYPE type;
    bool                      enabled;
} T_KWS_EFFECT_INSTANCE;

typedef enum t_kws_session_action
{
    KWS_SESSION_ACTION_NONE     = 0x00,
    KWS_SESSION_ACTION_CREATE   = 0x01,
    KWS_SESSION_ACTION_START    = 0x02,
    KWS_SESSION_ACTION_STOP     = 0x03,
    KWS_SESSION_ACTION_RELEASE  = 0x04,
} T_KWS_SESSION_ACTION;

typedef enum t_kws_session_state
{
    KWS_SESSION_STATE_RELEASED  = 0x00,
    KWS_SESSION_STATE_CREATING  = 0x01,
    KWS_SESSION_STATE_CREATED   = 0x02,
    KWS_SESSION_STATE_STARTING  = 0x03,
    KWS_SESSION_STATE_STARTED   = 0x04,
    KWS_SESSION_STATE_STOPPING  = 0x05,
    KWS_SESSION_STATE_STOPPED   = 0x06,
    KWS_SESSION_STATE_RELEASING = 0x07,
} T_KWS_SESSION_STATE;

typedef struct t_kws_session
{
    struct t_kws_session *next;
    T_AUDIO_PATH_HANDLE   handle;
    T_KWS_SESSION_STATE   state;
    T_KWS_SESSION_ACTION  action;
    T_AUDIO_FORMAT_INFO   format_info;
    uint16_t              frame_count;
    T_KWS_CBACK           cback;
} T_KWS_SESSION;

typedef struct t_kws_db
{
    T_OS_QUEUE            sessions;
} T_KWS_DB;

static T_KWS_DB *kws_db = NULL;

typedef bool (*T_KWS_SESSION_STATE_HANDLER)(T_KWS_SESSION       *kws,
                                            T_KWS_SESSION_STATE  state);

static bool kws_session_state_set(T_KWS_SESSION       *kws,
                                  T_KWS_SESSION_STATE  state);

static bool kws_path_cback(T_AUDIO_PATH_HANDLE handle,
                           T_AUDIO_PATH_EVENT  event,
                           uint32_t            param);

static bool kws_effect_xmit(T_AUDIO_EFFECT_INSTANCE   instance,
                            T_AUDIO_EFFECT_XMIT_TYPE  xmit_type,
                            void                     *context)
{
    T_KWS_EFFECT_INSTANCE *kws;
    T_AUDIO_PATH_HANDLE    handle;
    int32_t                ret = 0;
    kws = audio_effect_inheritor_get(instance);
    if (kws == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }
    if (kws->base != instance)
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
        if (audio_path_kws_set(handle) == false)
        {
            ret = 4;
            goto fail_set_kws;
        }
    }
    else if (xmit_type == AUDIO_EFFECT_XMIT_TYPE_DISABLE)
    {
        if (audio_path_kws_clear(handle) == false)
        {
            ret = 5;
            goto fail_clear_kws;
        }
    }

    return true;

fail_clear_kws:
fail_set_kws:
fail_get_owner:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR3("kws_effect_xmit: failed %d, instance %p, xmit_type %u",
                       -ret, instance, xmit_type);
    return false;
}

T_AUDIO_EFFECT_INSTANCE kws_effect_create(T_KWS_EFFECT_CONTENT_TYPE type)
{
    T_KWS_EFFECT_INSTANCE *kws;
    int32_t                ret = 0;
    kws = malloc(sizeof(T_KWS_EFFECT_INSTANCE));
    if (kws == NULL)
    {
        ret = 1;
        goto fail_alloc_kws;
    }
    kws->enabled   = false;
    kws->type      = type;
    kws->base = audio_effect_create(kws_effect_xmit,
                                    NULL);
    if (kws->base == NULL)
    {
        ret = 2;
        goto fail_create_effect;
    }
    if (audio_effect_inheritor_set(kws->base, kws) == false)
    {
        ret = 3;
        goto fail_set_inheritor;
    }
    AUDIO_PRINT_TRACE2("kws_effect_create: type %d, instance %p",
                       kws->type, kws->base);

    return kws->base;

fail_set_inheritor:
    audio_effect_release(kws->base);
fail_create_effect:
    free(kws);
fail_alloc_kws:
    AUDIO_PRINT_ERROR1("kws_effect_create: failed %d", -ret);
    return NULL;
}

bool kws_effect_enable(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_KWS_EFFECT_INSTANCE *kws;
    int32_t                ret = 0;
    AUDIO_PRINT_TRACE1("kws_effect_enable: instance %p", instance);
    kws = audio_effect_inheritor_get(instance);
    if (kws == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }
    if (kws->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }
    if (kws->enabled == true)
    {
        ret = 3;
        goto fail_check_state;
    }
    if (audio_effect_enable(kws->base) == false)
    {
        ret = 4;
        goto fail_enable_effect;
    }
    kws->enabled = true;

    return true;

fail_enable_effect:
fail_check_state:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("kws_effect_enable: failed %d", -ret);
    return false;
}

bool kws_effect_disable(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_KWS_EFFECT_INSTANCE *kws;
    int32_t                ret = 0;
    AUDIO_PRINT_TRACE1("kws_effect_disable: instance %p", instance);
    kws = audio_effect_inheritor_get(instance);
    if (kws == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }
    if (kws->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }
    if (kws->enabled == false)
    {
        ret = 3;
        goto fail_check_state;
    }
    if (audio_effect_disable(kws->base) == false)
    {
        ret = 4;
        goto fail_disable_effect;
    }
    kws->enabled = false;

    return true;

fail_disable_effect:
fail_check_state:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("kws_effect_disable: failed %d", -ret);
    return false;
}

bool kws_effect_release(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_KWS_EFFECT_INSTANCE *kws;
    int32_t                ret = 0;
    AUDIO_PRINT_TRACE1("kws_effect_release: instance %p", instance);
    kws = audio_effect_inheritor_get(instance);
    if (kws == NULL)
    {
        ret = 1;
        goto fail_get_inheritor;
    }
    if (kws->base != instance)
    {
        ret = 2;
        goto fail_check_base;
    }
    if (audio_effect_release(kws->base) == false)
    {
        ret = 3;
        goto fail_release_effect;
    }
    free(kws);

    return true;

fail_release_effect:
fail_check_base:
fail_get_inheritor:
    AUDIO_PRINT_ERROR1("kws_effect_release: failed %d", -ret);
    return false;
}

static bool kws_session_released_handler(T_KWS_SESSION       *kws,
                                         T_KWS_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case KWS_SESSION_STATE_RELEASED:
    case KWS_SESSION_STATE_CREATED:
    case KWS_SESSION_STATE_STARTING:
    case KWS_SESSION_STATE_STARTED:
    case KWS_SESSION_STATE_STOPPING:
    case KWS_SESSION_STATE_STOPPED:
    case KWS_SESSION_STATE_RELEASING:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case KWS_SESSION_STATE_CREATING:
        {
            T_AUDIO_PATH_PARAM path_param;

            path_param.kws.mode         = AUDIO_STREAM_MODE_NORMAL;
            path_param.kws.decoder_info = &kws->format_info;

            kws->handle = audio_path_create(AUDIO_CATEGORY_KWS,
                                            path_param,
                                            kws_path_cback);
            if (kws->handle != NULL)
            {
                kws->state  = state;
                os_queue_in(&kws_db->sessions, kws);
                ret = true;
            }
            else
            {
                ret = false;
            }
        }
        break;
    }

    return ret;
}

static bool kws_session_creating_handler(T_KWS_SESSION       *kws,
                                         T_KWS_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case KWS_SESSION_STATE_RELEASED:
    case KWS_SESSION_STATE_CREATING:
    case KWS_SESSION_STATE_STARTED:
    case KWS_SESSION_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case KWS_SESSION_STATE_CREATED:
        {
            kws->state = state;
            kws->cback(kws, KWS_EVENT_CREATED, 0);
            if (kws->action == KWS_SESSION_ACTION_NONE)
            {
                ret = true;
            }
            else if (kws->action == KWS_SESSION_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating path is impossible.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_START)
            {
                kws->action = KWS_SESSION_ACTION_NONE;
                ret = kws_session_state_set(kws, KWS_SESSION_STATE_STARTING);
            }
            else if (kws->action == KWS_SESSION_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_RELEASE)
            {
                kws->action = KWS_SESSION_ACTION_NONE;
                ret = kws_session_state_set(kws, KWS_SESSION_STATE_RELEASING);
            }
        }
        break;

    case KWS_SESSION_STATE_STARTING:
        {
            if (kws->action == KWS_SESSION_ACTION_NONE)
            {
                /* Start the path immediately before the path created event
                 * received, so this action should be pending.
                 */
                kws->action = KWS_SESSION_ACTION_START;
            }
            else if (kws->action == KWS_SESSION_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating path is impossible.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_START)
            {
                /* Drop the repeating path start action. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (kws->action == KWS_SESSION_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_RELEASE)
            {
                /* When the creating path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case KWS_SESSION_STATE_STOPPING:
        {
            if (kws->action == KWS_SESSION_ACTION_NONE)
            {
                /* Stopping the creating path is forbidden. */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating path is impossible.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_START)
            {
                /* Cancel all pending actions when the creating path is pending
                 * for start action, but receives the stop action later.
                 */
                kws->action = KWS_SESSION_ACTION_NONE;
            }
            else if (kws->action == KWS_SESSION_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_RELEASE)
            {
                /* When the creating path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case KWS_SESSION_STATE_RELEASING:
        {
            if (kws->action == KWS_SESSION_ACTION_NONE)
            {
                kws->action = KWS_SESSION_ACTION_RELEASE;
            }
            else if (kws->action == KWS_SESSION_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating path is impossible.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_START)
            {
                /* Override the pending start action for the creating path. */
                kws->action = KWS_SESSION_ACTION_RELEASE;
            }
            else if (kws->action == KWS_SESSION_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_RELEASE)
            {
                /* Drop the repeating release action. But this case
                 * is permitted.
                 */
                ret = true;
            }
        }
        break;
    }

    return ret;
}

static bool kws_session_created_handler(T_KWS_SESSION       *kws,
                                        T_KWS_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case KWS_SESSION_STATE_RELEASED:
    case KWS_SESSION_STATE_CREATING:
    case KWS_SESSION_STATE_CREATED:
    case KWS_SESSION_STATE_STARTED:
    case KWS_SESSION_STATE_STOPPING:
    case KWS_SESSION_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case KWS_SESSION_STATE_STARTING:
        {
            kws->state = state;
            ret = audio_path_start(kws->handle);
            if (ret == false)
            {
                kws->state = KWS_SESSION_STATE_CREATED;
            }
        }
        break;

    case KWS_SESSION_STATE_RELEASING:
        {
            kws->state = state;
            ret = audio_path_destroy(kws->handle);
            if (ret == false)
            {
                kws->state = KWS_SESSION_STATE_CREATED;
            }
        }
        break;
    }

    return ret;
}

static bool kws_session_starting_handler(T_KWS_SESSION       *kws,
                                         T_KWS_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case KWS_SESSION_STATE_RELEASED:
    case KWS_SESSION_STATE_CREATING:
    case KWS_SESSION_STATE_CREATED:
    case KWS_SESSION_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case KWS_SESSION_STATE_STARTING:
        {
            if (kws->action == KWS_SESSION_ACTION_NONE)
            {
                /* Drop the repeating path start action. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (kws->action == KWS_SESSION_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting path is impossible.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_STOP)
            {
                /* Cancel all pending actions when the starting path is pending
                 * for stop action, but receives the start action later.
                 */
                kws->action = KWS_SESSION_ACTION_NONE;
            }
            else if (kws->action == KWS_SESSION_ACTION_RELEASE)
            {
                /* When the starting path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case KWS_SESSION_STATE_STARTED:
        {
            kws->state = state;
            audio_mgr_dispatch(AUDIO_MSG_KWS_ENABLE, NULL);

            if (kws->action == KWS_SESSION_ACTION_NONE)
            {
                ret = true;
            }
            else if (kws->action == KWS_SESSION_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting path is impossible.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_STOP)
            {
                kws->action = KWS_SESSION_ACTION_NONE;
                ret = kws_session_state_set(kws, KWS_SESSION_STATE_STOPPING);
            }
            else if (kws->action == KWS_SESSION_ACTION_RELEASE)
            {
                kws->action = KWS_SESSION_ACTION_NONE;
                ret = kws_session_state_set(kws, KWS_SESSION_STATE_RELEASING);
            }
        }
        break;

    case KWS_SESSION_STATE_STOPPING:
        {
            if (kws->action == KWS_SESSION_ACTION_NONE)
            {
                kws->action = KWS_SESSION_ACTION_STOP;
            }
            else if (kws->action == KWS_SESSION_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting path is impossible.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_STOP)
            {
                /* Drop the repeating stop action. But this case is permitted. */
                ret = true;
            }
            else if (kws->action == KWS_SESSION_ACTION_RELEASE)
            {
                /* When the starting path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case KWS_SESSION_STATE_RELEASING:
        {
            if (kws->action == KWS_SESSION_ACTION_NONE)
            {
                kws->action = KWS_SESSION_ACTION_RELEASE;
            }
            else if (kws->action == KWS_SESSION_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting path is impossible.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_STOP)
            {
                /* Override the pending stop action for the starting path. */
                kws->action = KWS_SESSION_ACTION_RELEASE;
            }
            else if (kws->action == KWS_SESSION_ACTION_RELEASE)
            {
                /* Drop the repeating release action. But this case is permitted. */
                ret = true;
            }
        }
        break;
    }

    return ret;
}

static bool kws_session_started_handler(T_KWS_SESSION       *kws,
                                        T_KWS_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case KWS_SESSION_STATE_RELEASED:
    case KWS_SESSION_STATE_CREATING:
    case KWS_SESSION_STATE_CREATED:
    case KWS_SESSION_STATE_STARTING:
    case KWS_SESSION_STATE_STARTED:
    case KWS_SESSION_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case KWS_SESSION_STATE_STOPPING:
        {
            kws->state = state;
            ret = audio_path_stop(kws->handle);
            if (ret == false)
            {
                kws->state = KWS_SESSION_STATE_STARTED;
            }
        }
        break;

    case KWS_SESSION_STATE_RELEASING:
        {
            /* Started path cannot be released directly, so it should be
             * stopped first before released.
             */
            kws->state  = KWS_SESSION_STATE_STOPPING;
            kws->action = KWS_SESSION_ACTION_RELEASE;
            ret = audio_path_stop(kws->handle);
            if (ret == false)
            {
                kws->state  = KWS_SESSION_STATE_STARTED;
                kws->action = KWS_SESSION_ACTION_NONE;
            }
        }
        break;
    }

    return ret;
}

static bool kws_session_stopping_handler(T_KWS_SESSION       *kws,
                                         T_KWS_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case KWS_SESSION_STATE_RELEASED:
    case KWS_SESSION_STATE_CREATING:
    case KWS_SESSION_STATE_CREATED:
    case KWS_SESSION_STATE_STARTED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case KWS_SESSION_STATE_STARTING:
        {
            if (kws->action == KWS_SESSION_ACTION_NONE)
            {
                kws->action = KWS_SESSION_ACTION_START;
            }
            else if (kws->action == KWS_SESSION_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping path is impossible.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_START)
            {
                /* Drop the repeating start action. But this case is permitted. */
                ret = true;
            }
            else if (kws->action == KWS_SESSION_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_RELEASE)
            {
                /* When the stopping path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case KWS_SESSION_STATE_STOPPING:
        {
            if (kws->action == KWS_SESSION_ACTION_NONE)
            {
                /* Drop the repeating stop action. But this case is permitted. */
                ret = true;
            }
            else if (kws->action == KWS_SESSION_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping path is impossible.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_START)
            {
                /* Cancel all pending actions when the stopping path is pending
                 * for start action, but receives the stop action later.
                 */
                kws->action = KWS_SESSION_ACTION_NONE;
            }
            else if (kws->action == KWS_SESSION_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_RELEASE)
            {
                /* When the stopping path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case KWS_SESSION_STATE_STOPPED:
        {
            kws->state = state;
            kws->cback(kws, KWS_EVENT_STOPPED, 0);
            if (kws->action == KWS_SESSION_ACTION_NONE)
            {
                ret = true;
            }
            else if (kws->action == KWS_SESSION_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping path is impossible.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_START)
            {
                kws->action = KWS_SESSION_ACTION_NONE;
                ret = kws_session_state_set(kws, KWS_SESSION_STATE_STARTING);
            }
            else if (kws->action == KWS_SESSION_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_RELEASE)
            {
                kws->action = KWS_SESSION_ACTION_NONE;
                ret = kws_session_state_set(kws, KWS_SESSION_STATE_RELEASING);
            }
        }
        break;

    case KWS_SESSION_STATE_RELEASING:
        {
            if (kws->action == KWS_SESSION_ACTION_NONE)
            {
                kws->action = KWS_SESSION_ACTION_RELEASE;
            }
            else if (kws->action == KWS_SESSION_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping path is impossible.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_START)
            {
                /* Override the pending start action for the stopping path. */
                kws->action = KWS_SESSION_ACTION_RELEASE;
            }
            else if (kws->action == KWS_SESSION_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (kws->action == KWS_SESSION_ACTION_RELEASE)
            {
                /* Drop the repeating release action. But this case is permitted. */
                ret = true;
            }
        }
        break;
    }

    return ret;
}

static bool kws_session_stopped_handler(T_KWS_SESSION       *kws,
                                        T_KWS_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case KWS_SESSION_STATE_RELEASED:
    case KWS_SESSION_STATE_CREATING:
    case KWS_SESSION_STATE_CREATED:
    case KWS_SESSION_STATE_STARTED:
    case KWS_SESSION_STATE_STOPPING:
    case KWS_SESSION_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case KWS_SESSION_STATE_STARTING:
        {
            kws->state = state;
            ret = audio_path_start(kws->handle);
            if (ret == false)
            {
                kws->state = KWS_SESSION_STATE_STOPPED;
            }
        }
        break;

    case KWS_SESSION_STATE_RELEASING:
        {
            kws->state = state;
            ret = audio_path_destroy(kws->handle);
            if (ret == false)
            {
                kws->state = KWS_SESSION_STATE_STOPPED;
            }
        }
        break;
    }

    return ret;
}

static bool kws_session_releasing_handler(T_KWS_SESSION       *kws,
                                          T_KWS_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case KWS_SESSION_STATE_RELEASED:
        {
            kws->state  = state;
            kws->action = KWS_SESSION_ACTION_NONE;

            audio_mgr_dispatch(AUDIO_MSG_KWS_DISABLE, NULL);
            os_queue_delete(&kws_db->sessions, kws);
            kws->cback(kws, KWS_EVENT_RELEASED, 0);
            free(kws);
        }
        break;

    case KWS_SESSION_STATE_CREATING:
    case KWS_SESSION_STATE_CREATED:
    case KWS_SESSION_STATE_STARTING:
    case KWS_SESSION_STATE_STARTED:
    case KWS_SESSION_STATE_STOPPING:
    case KWS_SESSION_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case KWS_SESSION_STATE_RELEASING:
        {
            /* Drop the repeating release action. But this case is permitted. */
            ret = true;
        }
        break;
    }

    return ret;
}

static const T_KWS_SESSION_STATE_HANDLER kws_session_state_handler[] =
{
    kws_session_released_handler,
    kws_session_creating_handler,
    kws_session_created_handler,
    kws_session_starting_handler,
    kws_session_started_handler,
    kws_session_stopping_handler,
    kws_session_stopped_handler,
    kws_session_releasing_handler,
};

static bool kws_session_state_set(T_KWS_SESSION       *kws,
                                  T_KWS_SESSION_STATE  state)
{
    AUDIO_PRINT_TRACE4("kws_session_state_set: handle %p, curr state %u, next state %u, action %u",
                       kws, kws->state, state, kws->action);

    return kws_session_state_handler[kws->state](kws, state);
}

static T_KWS_SESSION *kws_find(T_AUDIO_PATH_HANDLE handle)
{
    T_KWS_SESSION *kws;
    kws = os_queue_peek(&kws_db->sessions, 0);
    while (kws != NULL)
    {
        if (kws->handle != NULL &&
            kws->handle == handle)
        {
            return kws;
        }
        kws = kws->next;
    }

    return kws;
}

static bool kws_path_cback(T_AUDIO_PATH_HANDLE handle,
                           T_AUDIO_PATH_EVENT  event,
                           uint32_t            param)
{
    T_KWS_SESSION *kws;
    kws = kws_find(handle);
    AUDIO_PRINT_TRACE3("kws_path_cback: handle %p, event 0x%02x, param 0x%08x",
                       handle, event, param);
    if (kws != NULL)
    {
        switch (event)
        {
        case AUDIO_PATH_EVT_RELEASE:
            {
                kws_session_state_set(kws, KWS_SESSION_STATE_RELEASED);
                kws->cback(kws, KWS_EVENT_RELEASED, 0);
            }
            break;
        case AUDIO_PATH_EVT_CREATE:
            {
                kws_session_state_set(kws, KWS_SESSION_STATE_CREATED);
                kws->cback(kws, KWS_EVENT_CREATED, 0);
            }
            break;
        case AUDIO_PATH_EVT_RUNNING:
            {
                kws_session_state_set(kws, KWS_SESSION_STATE_STARTED);
                kws->cback(kws, KWS_EVENT_STARTED, 0);
            }
            break;
        case AUDIO_PATH_EVT_IDLE:
            {
                kws_session_state_set(kws, KWS_SESSION_STATE_STOPPED);
                kws->cback(kws, KWS_EVENT_STOPPED, 0);
            }
            break;
        case AUDIO_PATH_EVT_DATA_REQ:
            {
                if (kws->state == KWS_SESSION_STATE_STARTED)
                {
                    kws->cback(kws, KWS_EVENT_DATA_FILLED, 0);
                }
            }
            break;
        case AUDIO_PATH_EVT_KWS_HIT:
            {
                T_AUDIO_PATH_KWS_HIT        *kws_hit = (T_AUDIO_PATH_KWS_HIT *)param;
                T_AUDIO_MSG_PAYLOAD_KWS_HIT  payload;
                payload.keyword_index = kws_hit->keyword_index;
                audio_mgr_dispatch(AUDIO_MSG_KWS_HIT, &payload);
            }
            break;
        default:
            break;
        }

        return true;
    }

    return false;
}

bool kws_fill(T_KWS_HANDLE           handle,
              uint32_t               timestamp,
              uint16_t               seq_num,
              T_AUDIO_STREAM_STATUS  status,
              uint8_t                frame_num,
              void                  *buf,
              uint16_t               len)
{
    T_AUDIO_PATH_DATA_HDR *header;
    T_KWS_SESSION         *kws = (T_KWS_SESSION *)handle;
    if (kws->state == KWS_SESSION_STATE_STARTED)
    {
        header = malloc(len + sizeof(T_AUDIO_PATH_DATA_HDR));
        if (header != NULL)
        {
            header->timestamp = timestamp;
            header->seq_num = seq_num;
            header->frame_count = kws->frame_count + frame_num;
            header->frame_number = frame_num;
            header->status = status;
            header->payload_length = len;
            if (buf != NULL)
            {
                memcpy(header->payload, buf, len);
            }
            audio_path_data_send(kws->handle,
                                 header,
                                 len + sizeof(T_AUDIO_PATH_DATA_HDR));
            AUDIO_PRINT_INFO3("kws_fill: kws_session %p, seq_num 0x%x, len 0x%x",
                              kws->handle, seq_num, len);
            kws->frame_count += frame_num;
            free(header);

            return true;
        }
    }

    return false;
}

bool kws_init(void)
{
    kws_db = malloc(sizeof(T_KWS_DB));
    if (kws_db != NULL)
    {
        os_queue_init(&kws_db->sessions);
        return true;
    }

    return false;
}

void kws_deinit(void)
{
    if (kws_db != NULL)
    {
        T_KWS_SESSION *kws;
        while ((kws = os_queue_out(&kws_db->sessions)) != NULL)
        {
            free(kws);
        }
        free(kws_db);
        kws_db = NULL;
    }
}

T_KWS_HANDLE kws_create(T_AUDIO_FORMAT_INFO format_info,
                        T_KWS_CBACK         cback)
{
    T_KWS_SESSION *kws;
    int32_t        ret = 0;
    if (cback == NULL)
    {
        ret = 1;
        goto fail_check_cback;
    }
    kws = calloc(1, sizeof(T_KWS_SESSION));
    if (kws == NULL)
    {
        ret = 2;
        goto fail_alloc_kws;
    }
    AUDIO_PRINT_INFO2("kws_create: handle %p, format type 0x%x",
                      kws, format_info.type);
    kws->handle      = NULL;
    kws->state       = KWS_SESSION_STATE_RELEASED;
    kws->action      = KWS_SESSION_ACTION_NONE;
    kws->frame_count = 0;
    kws->format_info = format_info;
    kws->cback       = cback;
    if (kws_session_state_set(kws, KWS_SESSION_STATE_CREATING) == false)
    {
        ret = 3;
        goto fail_set_state;
    }

    return (T_KWS_HANDLE)kws;

fail_set_state:
    free(kws);
fail_alloc_kws:
fail_check_cback:
    AUDIO_PRINT_ERROR1("kws_create: failed %d", -ret);
    return NULL;
}

bool kws_start(T_KWS_HANDLE handle)
{
    T_KWS_SESSION *kws;
    int32_t        ret = 0;
    kws = (T_KWS_SESSION *)handle;
    if (os_queue_search(&kws_db->sessions, kws) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }
    if (kws_session_state_set(kws, KWS_SESSION_STATE_STARTING) == false)
    {
        ret = 2;
        goto fail_set_state;
    }

    return true;

fail_set_state:
fail_check_handle:
    AUDIO_PRINT_ERROR2("kws_start: handle %p, failed %d", handle, -ret);
    return false;
}

bool kws_stop(T_KWS_HANDLE handle)
{
    T_KWS_SESSION *kws;
    int32_t        ret = 0;
    kws = (T_KWS_SESSION *)handle;
    if (os_queue_search(&kws_db->sessions, kws) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }
    if (kws_session_state_set(kws, KWS_SESSION_STATE_STOPPING) == false)
    {
        ret = 2;
        goto fail_set_state;
    }

    return true;

fail_set_state:
fail_check_handle:
    AUDIO_PRINT_ERROR2("kws_stop: handle %p, failed %d", handle, -ret);
    return false;
}

bool kws_release(T_KWS_HANDLE handle)
{
    T_KWS_SESSION *kws;
    int32_t        ret = 0;
    kws = (T_KWS_SESSION *)handle;
    if (os_queue_search(&kws_db->sessions, kws) == false)
    {
        ret = 1;
        goto fail_check_handle;
    }
    if (kws_session_state_set(kws, KWS_SESSION_STATE_RELEASING) == false)
    {
        ret = 2;
        goto fail_set_state;
    }

    return true;

fail_set_state:
fail_check_handle:
    AUDIO_PRINT_ERROR2("kws_release: handle %p, failed %d", handle, -ret);
    return false;
}
#else
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "kws.h"


bool kws_init(void)
{
    /* Let KWS initialization pass. */
    return true;
}

void kws_deinit(void)
{
}

T_AUDIO_EFFECT_INSTANCE kws_effect_create(T_KWS_EFFECT_CONTENT_TYPE type)
{
    return NULL;
}

bool kws_effect_enable(T_AUDIO_EFFECT_INSTANCE instance)
{
    return false;
}
bool kws_effect_disable(T_AUDIO_EFFECT_INSTANCE instance)
{
    return false;
}

bool kws_effect_release(T_AUDIO_EFFECT_INSTANCE instance)
{
    return false;
}

T_KWS_HANDLE kws_create(T_AUDIO_FORMAT_INFO format_info,
                        T_KWS_CBACK         cback)
{
    return NULL;
}

bool kws_start(T_KWS_HANDLE handle)
{
    return false;
}

bool kws_stop(T_KWS_HANDLE handle)
{
    return false;
}

bool kws_fill(T_KWS_HANDLE           handle,
              uint32_t               timestamp,
              uint16_t               seq_num,
              T_AUDIO_STREAM_STATUS  status,
              uint8_t                frame_num,
              void                  *buf,
              uint16_t               len)
{
    return false;
}

#endif /* CONFIG_REALTEK_AM_KWS_SUPPORT */
