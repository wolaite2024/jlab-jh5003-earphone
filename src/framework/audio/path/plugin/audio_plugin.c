/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "stdlib.h"

#include "os_msg.h"
#include "os_queue.h"
#include "trace.h"
#include "audio_type.h"
#include "audio_path.h"
#include "audio_plugin.h"

/* TODO: Remove Start */
#include "app_msg.h"
#include "sys_mgr.h"
void *hAudioPluginQueueHandleAu = NULL;
extern void *hEventQueueHandleAu;
/* TODO: Remove Stop */

#if (CONFIG_REALTEK_AM_AUDIO_PLUGIN_SUPPORT == 1)

typedef struct t_audio_plugin_context
{
    T_AUDIO_PATH_HANDLE        path;
    T_AUDIO_PLUGIN_OCCASION    occasion;
} T_AUDIO_PLUGIN_CONTEXT;

typedef struct t_audio_plugin_instance
{
    const T_AUDIO_PLUGIN_POLICY     *policies;
    uint32_t                         policies_cnt;
} T_AUDIO_PLUGIN_INSTANCE;

typedef struct t_pending_occur_elem
{
    struct t_pending_occur_elem *p_next;
    T_AUDIO_PLUGIN_CONTEXT       context;
} T_PENDING_OCCUR_ELEM;

typedef struct t_audio_plugin_msg
{
    T_AUDIO_PLUGIN_HANDLE     handle;
    void                     *context;
} T_AUDIO_PLUGIN_MSG;

typedef struct t_audio_plugin_db
{
    T_AUDIO_PLUGIN_INSTANCE  *instance;
    T_OS_QUEUE                pending_occur_q;
} T_AUDIO_PLUGIN_DB;

static T_AUDIO_PLUGIN_DB *audio_plugin_db = NULL;

void audio_plugin_msg_handler(void)
{
    T_AUDIO_PLUGIN_MSG    msg;
    T_PENDING_OCCUR_ELEM *elem;
    T_AUDIO_PATH_MSG      path_msg;

    if (os_msg_recv(hAudioPluginQueueHandleAu, &msg, 0) == true)
    {
        elem = os_queue_peek(&audio_plugin_db->pending_occur_q, 0);
        while (elem != NULL)
        {
            if (memcmp(&elem->context, msg.context, sizeof(elem->context)) == 0)
            {
                /* notify audio path to continue its process */
                path_msg.type     = AUDIO_PATH_MSG_TYPE_PLUGIN;
                path_msg.handle   = elem->context.path;
                path_msg.occasion = elem->context.occasion;
                path_msg.cback    = NULL;
                audio_path_msg_send(&path_msg);
                os_queue_delete(&audio_plugin_db->pending_occur_q, elem);
                free(elem);
                break;
            }

            elem = elem->p_next;
        }
    }
}

bool audio_plugin_init(void)
{
    int32_t ret = 0;

    audio_plugin_db = calloc(1, sizeof(T_AUDIO_PLUGIN_DB));
    if (audio_plugin_db == NULL)
    {
        ret = 1;
        goto fail_alloc_db;
    }

    os_queue_init(&audio_plugin_db->pending_occur_q);
    if (os_msg_queue_create(&hAudioPluginQueueHandleAu,
                            "pluginQ",
                            8,
                            sizeof(T_AUDIO_PLUGIN_MSG)) == false)
    {
        ret = 2;
        goto fail_create_msg_queue;
    }

    sys_mgr_event_register(EVENT_AUDIO_PLUGIN_MSG, audio_plugin_msg_handler);

    return true;

fail_create_msg_queue:
    free(audio_plugin_db);
    audio_plugin_db = NULL;
fail_alloc_db:
    AUDIO_PRINT_ERROR1("audio_plugin_init: failed %d", -ret);
    return false;
}

void audio_plugin_deinit(void)
{
    if (audio_plugin_db != NULL)
    {
        T_PENDING_OCCUR_ELEM *elem;

        /* free instance */
        free(audio_plugin_db->instance);

        /* free pending occur elements */
        elem = os_queue_out(&audio_plugin_db->pending_occur_q);
        while (elem != NULL)
        {
            free(elem);
            elem = os_queue_out(&audio_plugin_db->pending_occur_q);
        }

        os_msg_queue_delete(hAudioPluginQueueHandleAu);
        free(audio_plugin_db);
        audio_plugin_db = NULL;
    }
}

T_AUDIO_PLUGIN_HANDLE audio_plugin_create(const T_AUDIO_PLUGIN_POLICY *policies,
                                          uint32_t                     count)
{
    T_AUDIO_PLUGIN_INSTANCE *instance;
    int32_t                  ret = 0;

    if (audio_plugin_db == NULL)
    {
        ret = 1;
        goto fail_invalid_db;
    }

    if (policies == NULL || count == 0)
    {
        ret = 2;
        goto fail_invalid_param;
    }

    instance = malloc(sizeof(T_AUDIO_PLUGIN_INSTANCE));
    if (instance == NULL)
    {
        ret = 3;
        goto fail_alloc_instance;
    }

    instance->policies = policies;
    instance->policies_cnt = count;
    audio_plugin_db->instance = instance;

    AUDIO_PRINT_INFO1("audio_plugin_create: handle %p", instance);
    return (T_AUDIO_PLUGIN_HANDLE)instance;

fail_alloc_instance:
fail_invalid_param:
fail_invalid_db:
    AUDIO_PRINT_ERROR1("audio_plugin_create: failed %d", -ret);
    return NULL;
}

bool audio_plugin_release(T_AUDIO_PLUGIN_HANDLE handle)
{
    T_AUDIO_PLUGIN_INSTANCE *instance;

    instance = (T_AUDIO_PLUGIN_INSTANCE *)handle;
    if (instance == audio_plugin_db->instance)
    {
        free(audio_plugin_db->instance);
        audio_plugin_db->instance = NULL;

        AUDIO_PRINT_TRACE1("audio_plugin_release: handle %p", handle);
        return true;
    }

    return false;
}

void audio_plugin_msg_send(T_AUDIO_PLUGIN_HANDLE  handle,
                           void                  *context)
{
    uint8_t            evt;
    T_AUDIO_PLUGIN_MSG plugin_msg;

    evt = EVENT_AUDIO_PLUGIN_MSG;
    plugin_msg.handle = handle;
    plugin_msg.context = context;
    if (os_msg_send(hAudioPluginQueueHandleAu, &plugin_msg, 0) == true)
    {
        (void)os_msg_send(hEventQueueHandleAu, &evt, 0);
    }
}

/* Audio path calls this under "occasion_type", then waits for msg from audio plugin to proceed its process. */
/* Within this API, audio plugin should look up all the policies and execute all required actions. */
/*
 * Note: After all actions have been done, the audio plugin should send a msg to audio_path through audio_path_msg_send.
         In order to achieve this, a pending occur queue is used. Each call of this API enqueues an elem
         node to the pending occur queue if some action is required. The elem node is dequeued in the msg handler
         when it detects that all required actions have been done.
 */
void audio_plugin_occasion_occur(T_AUDIO_PATH_HANDLE     path,
                                 T_AUDIO_CATEGORY        audio_category,
                                 T_AUDIO_PLUGIN_OCCASION occasion_type,
                                 T_AUDIO_PLUGIN_PARAM    param)
{
    T_PENDING_OCCUR_ELEM *elem;

    elem = malloc(sizeof(T_PENDING_OCCUR_ELEM));
    if (elem != NULL)
    {
        T_AUDIO_PLUGIN_INSTANCE *instance;
        bool                     match = false;
        uint32_t                 i;

        elem->context.path     = path;
        elem->context.occasion = occasion_type;

        instance = audio_plugin_db->instance;
        if (instance != NULL)
        {
            /* look up all policies under this instance */
            for (i = 0; i < instance->policies_cnt; i++)
            {
                if (occasion_type == instance->policies[i].occasion &&
                    audio_category == instance->policies[i].category &&
                    instance->policies[i].handler)
                {
                    instance->policies[i].handler((T_AUDIO_PLUGIN_HANDLE)instance, param, &elem->context);
                    match = true;
                    break;
                }
            }
        }

        if (match == false)
        {
            /* no plugin is required, notify audio path directly */
            T_AUDIO_PATH_MSG msg;

            msg.type     = AUDIO_PATH_MSG_TYPE_PLUGIN;
            msg.handle   = path;
            msg.occasion = occasion_type;
            msg.cback    = NULL;
            audio_path_msg_send(&msg);
            free(elem);
        }
        else
        {
            os_queue_in(&audio_plugin_db->pending_occur_q, elem);
        }
    }
}

#else /* CONFIG_REALTEK_AM_AUDIO_PLUGIN_SUPPORT */

bool audio_plugin_init(void)
{
    return true;
}

void audio_plugin_deinit(void)
{
    return;
}

T_AUDIO_PLUGIN_HANDLE audio_plugin_create(const T_AUDIO_PLUGIN_POLICY *policies,
                                          uint32_t                     count)
{
    return NULL;
}

bool audio_plugin_release(T_AUDIO_PLUGIN_HANDLE handle)
{
    return true;
}

void audio_plugin_msg_send(T_AUDIO_PLUGIN_HANDLE  handle,
                           void                  *context)
{
    return;
}

void audio_plugin_occasion_occur(T_AUDIO_PATH_HANDLE     path,
                                 T_AUDIO_CATEGORY        audio_type,
                                 T_AUDIO_PLUGIN_OCCASION occasion_type,
                                 T_AUDIO_PLUGIN_PARAM    param)
{
    T_AUDIO_PATH_MSG msg;

    /* fake msg to proceed audio path */
    msg.type     = AUDIO_PATH_MSG_TYPE_PLUGIN;
    msg.handle   = path;
    msg.occasion = occasion_type;
    msg.cback    = NULL;
    audio_path_msg_send(&msg);
    return;
}

#endif /* CONFIG_REALTEK_AM_AUDIO_PLUGIN_SUPPORT */
