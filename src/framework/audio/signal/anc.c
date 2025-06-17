/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
#include <stdbool.h>
#include <stdint.h>

#include "trace.h"
#include "audio_path.h"
#include "audio_mgr.h"
#include "anc.h"

/* TODO: Remove */
#include "anc_mgr.h"
/* TODO Remove End */

#define ANC_VOLUME_OUT_LEVEL_MIN        (0)
#define ANC_VOLUME_OUT_LEVEL_MAX        (15)
#define ANC_VOLUME_OUT_LEVEL_DEFAULT    (10)

#define ANC_VOLUME_IN_LEVEL_MIN         (0)
#define ANC_VOLUME_IN_LEVEL_MAX         (15)
#define ANC_VOLUME_IN_LEVEL_DEFAULT     (10)

typedef enum t_anc_action
{
    ANC_ACTION_NONE     = 0x00,
    ANC_ACTION_CREATE   = 0x01,
    ANC_ACTION_START    = 0x02,
    ANC_ACTION_STOP     = 0x03,
    ANC_ACTION_RELEASE  = 0x04,
} T_ANC_ACTION;

typedef enum t_anc_session_state
{
    ANC_SESSION_STATE_RELEASED  = 0x00,
    ANC_SESSION_STATE_CREATING  = 0x01,
    ANC_SESSION_STATE_CREATED   = 0x02,
    ANC_SESSION_STATE_STARTING  = 0x03,
    ANC_SESSION_STATE_STARTED   = 0x04,
    ANC_SESSION_STATE_STOPPING  = 0x05,
    ANC_SESSION_STATE_STOPPED   = 0x06,
    ANC_SESSION_STATE_RELEASING = 0x07,
} T_ANC_SESSION_STATE;

typedef struct t_anc_db
{
    T_AUDIO_PATH_HANDLE             path_handle;
    T_ANC_SESSION_STATE             state;
    T_ANC_ACTION                    action;
    uint8_t                         scenario_id;
    uint8_t                         volume_out;
    uint8_t                         volume_in;
} T_ANC_DB;

typedef bool (*T_ANC_STATE_HANDLER)(T_ANC_SESSION_STATE state);

static T_ANC_DB anc_db;

static bool anc_path_cback(T_AUDIO_PATH_HANDLE path_handle,
                           T_AUDIO_PATH_EVENT  event,
                           uint32_t            param);
static bool anc_state_set(T_ANC_SESSION_STATE state);

static bool anc_released_handler(T_ANC_SESSION_STATE state)
{
    bool ret = true;

    switch (state)
    {
    case ANC_SESSION_STATE_RELEASED:
    case ANC_SESSION_STATE_CREATED:
    case ANC_SESSION_STATE_STARTING:
    case ANC_SESSION_STATE_STARTED:
    case ANC_SESSION_STATE_STOPPING:
    case ANC_SESSION_STATE_STOPPED:
    case ANC_SESSION_STATE_RELEASING:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case ANC_SESSION_STATE_CREATING:
        {
            T_AUDIO_PATH_PARAM path_param;

            path_param.anc.device    = AUDIO_DEVICE_OUT_SPK | AUDIO_DEVICE_IN_MIC;
            path_param.anc.dac_level = anc_db.volume_out;
            path_param.anc.adc_level = anc_db.volume_in;

            anc_db.path_handle = audio_path_create(AUDIO_CATEGORY_ANC,
                                                   path_param,
                                                   anc_path_cback);
            if (anc_db.path_handle != NULL)
            {
                anc_db.state = state;
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

static bool anc_creating_handler(T_ANC_SESSION_STATE state)
{
    bool ret = true;

    switch (state)
    {
    case ANC_SESSION_STATE_RELEASED:
    case ANC_SESSION_STATE_CREATING:
    case ANC_SESSION_STATE_STARTED:
    case ANC_SESSION_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case ANC_SESSION_STATE_CREATED:
        {
            anc_db.state = state;

            if (anc_db.action == ANC_ACTION_NONE)
            {
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating path is impossible.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_START)
            {
                anc_db.action = ANC_ACTION_NONE;
                ret = anc_state_set(ANC_SESSION_STATE_STARTING);
            }
            else if (anc_db.action == ANC_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_RELEASE)
            {
                anc_db.action = ANC_ACTION_NONE;
                ret = anc_state_set(ANC_SESSION_STATE_RELEASING);
            }
        }
        break;

    case ANC_SESSION_STATE_STARTING:
        {
            if (anc_db.action == ANC_ACTION_NONE)
            {
                /* Start the path immediately before the path created event
                 * received, so this action should be pending.
                 */
                anc_db.action = ANC_ACTION_START;
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating path is impossible.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_START)
            {
                /* Drop the repeating path start action. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_RELEASE)
            {
                /* When the creating path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case ANC_SESSION_STATE_STOPPING:
        {
            if (anc_db.action == ANC_ACTION_NONE)
            {
                /* Stopping the creating path is forbidden. */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating path is impossible.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_START)
            {
                /* Cancel all pending actions when the creating path is pending
                 * for start action, but receives the stop action later.
                 */
                anc_db.action = ANC_ACTION_NONE;
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_RELEASE)
            {
                /* When the creating path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case ANC_SESSION_STATE_RELEASING:
        {
            if (anc_db.action == ANC_ACTION_NONE)
            {
                anc_db.action = ANC_ACTION_RELEASE;
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same creating path is impossible.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_START)
            {
                /* Override the pending start action for the creating path. */
                anc_db.action = ANC_ACTION_RELEASE;
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in creating state.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_RELEASE)
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

static bool anc_created_handler(T_ANC_SESSION_STATE state)
{
    bool ret = true;

    switch (state)
    {
    case ANC_SESSION_STATE_RELEASED:
    case ANC_SESSION_STATE_CREATING:
    case ANC_SESSION_STATE_CREATED:
    case ANC_SESSION_STATE_STARTED:
    case ANC_SESSION_STATE_STOPPING:
    case ANC_SESSION_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case ANC_SESSION_STATE_STARTING:
        {
            T_ANC_LLAPT_CFG config;

            anc_db.state = state;

            config.sub_type = ANC_IMAGE_SUB_TYPE_ANC_COEF;
            config.scenario_id = anc_db.scenario_id;
            audio_path_configure(anc_db.path_handle, &config);

            ret = audio_path_start(anc_db.path_handle);
            if (ret == false)
            {
                anc_db.state = ANC_SESSION_STATE_CREATED;
            }
        }
        break;

    case ANC_SESSION_STATE_RELEASING:
        {
            anc_db.state = state;

            ret = audio_path_destroy(anc_db.path_handle);
            if (ret == false)
            {
                anc_db.state = ANC_SESSION_STATE_CREATED;
            }
        }
        break;
    }

    return ret;
}

static bool anc_starting_handler(T_ANC_SESSION_STATE state)
{
    bool ret = true;

    switch (state)
    {
    case ANC_SESSION_STATE_RELEASED:
    case ANC_SESSION_STATE_CREATING:
    case ANC_SESSION_STATE_CREATED:
    case ANC_SESSION_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case ANC_SESSION_STATE_STARTING:
        {
            if (anc_db.action == ANC_ACTION_NONE)
            {
                /* Drop the repeating path start action. But this case
                 * is permitted.
                 */
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting path is impossible.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_STOP)
            {
                /* Cancel all pending actions when the starting path is pending
                 * for stop action, but receives the start action later.
                 */
                anc_db.action = ANC_ACTION_NONE;
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_RELEASE)
            {
                /* When the starting path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case ANC_SESSION_STATE_STARTED:
        {
            anc_db.state = state;

            audio_mgr_event_post(AUDIO_EVENT_ANC_ENABLED, NULL, 0);

            if (anc_db.action == ANC_ACTION_NONE)
            {
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_STOP)
            {
                anc_db.action = ANC_ACTION_NONE;
                ret = anc_state_set(ANC_SESSION_STATE_STOPPING);
            }
            else if (anc_db.action == ANC_ACTION_RELEASE)
            {
                anc_db.action = ANC_ACTION_NONE;
                ret = anc_state_set(ANC_SESSION_STATE_RELEASING);
            }
        }
        break;

    case ANC_SESSION_STATE_STOPPING:
        {
            if (anc_db.action == ANC_ACTION_NONE)
            {
                anc_db.action = ANC_ACTION_STOP;
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting path is impossible.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_STOP)
            {
                /* Drop the repeating stop action. But this case is permitted. */
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_RELEASE)
            {
                /* When the starting path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case ANC_SESSION_STATE_RELEASING:
        {
            if (anc_db.action == ANC_ACTION_NONE)
            {
                anc_db.action = ANC_ACTION_RELEASE;
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same starting path is impossible.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_START)
            {
                /* This case will not happen in reality, as the pending start
                 * action will be dropped directly in starting state.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_STOP)
            {
                /* Override the pending stop action for the starting path. */
                anc_db.action = ANC_ACTION_RELEASE;
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_RELEASE)
            {
                /* Drop the repeating release action. But this case is permitted. */
                ret = true;
            }
        }
        break;
    }

    return ret;
}

static bool anc_started_handler(T_ANC_SESSION_STATE state)
{
    bool ret = true;

    switch (state)
    {
    case ANC_SESSION_STATE_RELEASED:
    case ANC_SESSION_STATE_CREATING:
    case ANC_SESSION_STATE_CREATED:
    case ANC_SESSION_STATE_STARTING:
    case ANC_SESSION_STATE_STARTED:
    case ANC_SESSION_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case ANC_SESSION_STATE_STOPPING:
        {
            anc_db.state = state;

            ret = audio_path_stop(anc_db.path_handle);
            if (ret == false)
            {
                anc_db.state = ANC_SESSION_STATE_STARTED;
            }
        }
        break;

    case ANC_SESSION_STATE_RELEASING:
        {
            /* Started path cannot be released directly, so it should be
             * stopped first before released.
             */
            anc_db.state  = ANC_SESSION_STATE_STOPPING;
            anc_db.action = ANC_ACTION_RELEASE;

            ret = audio_path_stop(anc_db.path_handle);
            if (ret == false)
            {
                anc_db.state  = ANC_SESSION_STATE_STARTED;
                anc_db.action = ANC_ACTION_NONE;
            }
        }
        break;
    }

    return ret;
}


static bool anc_stopping_handler(T_ANC_SESSION_STATE state)
{
    bool ret = true;

    switch (state)
    {
    case ANC_SESSION_STATE_RELEASED:
    case ANC_SESSION_STATE_CREATING:
    case ANC_SESSION_STATE_CREATED:
    case ANC_SESSION_STATE_STARTED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case ANC_SESSION_STATE_STARTING:
        {
            if (anc_db.action == ANC_ACTION_NONE)
            {
                anc_db.action = ANC_ACTION_START;
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping path is impossible.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_START)
            {
                /* Drop the repeating start action. But this case is permitted. */
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_RELEASE)
            {
                /* When the stopping path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case ANC_SESSION_STATE_STOPPING:
        {
            if (anc_db.action == ANC_ACTION_NONE)
            {
                /* Drop the repeating stop action. But this case is permitted. */
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping path is impossible.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_START)
            {
                /* Cancel all pending actions when the stopping path is pending
                 * for start action, but receives the stop action later.
                 */
                anc_db.action = ANC_ACTION_NONE;
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_RELEASE)
            {
                /* When the stopping path is pending for releasing, any
                 * following actions should be dropped.
                 */
                ret = false;
            }
        }
        break;

    case ANC_SESSION_STATE_STOPPED:
        {
            anc_db.state = state;

            audio_mgr_event_post(AUDIO_EVENT_ANC_DISABLED, NULL, 0);

            if (anc_db.action == ANC_ACTION_NONE)
            {
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_START)
            {
                anc_db.action = ANC_ACTION_NONE;
                ret = anc_state_set(ANC_SESSION_STATE_STARTING);
            }
            else if (anc_db.action == ANC_ACTION_RELEASE)
            {
                anc_db.action = ANC_ACTION_NONE;
                ret = anc_state_set(ANC_SESSION_STATE_RELEASING);
            }
        }
        break;

    case ANC_SESSION_STATE_RELEASING:
        {
            if (anc_db.action == ANC_ACTION_NONE)
            {
                anc_db.action = ANC_ACTION_RELEASE;
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_CREATE)
            {
                /* This case will not happen in reality, as the pending create
                 * action of the same stopping path is impossible.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_START)
            {
                /* Override the pending start action for the stopping path. */
                anc_db.action = ANC_ACTION_RELEASE;
                ret = true;
            }
            else if (anc_db.action == ANC_ACTION_STOP)
            {
                /* This case will not happen in reality, as the pending stop
                 * action will be dropped directly in stopping state.
                 */
                ret = false;
            }
            else if (anc_db.action == ANC_ACTION_RELEASE)
            {
                /* Drop the repeating release action. But this case is permitted. */
                ret = true;
            }
        }
        break;
    }

    return ret;
}

static bool anc_stopped_handler(T_ANC_SESSION_STATE  state)
{
    bool ret = true;

    switch (state)
    {
    case ANC_SESSION_STATE_RELEASED:
    case ANC_SESSION_STATE_CREATING:
    case ANC_SESSION_STATE_CREATED:
    case ANC_SESSION_STATE_STARTED:
    case ANC_SESSION_STATE_STOPPING:
    case ANC_SESSION_STATE_STOPPED:
        {
            /* This case will not happen in reality. */
            ret = false;
        }
        break;

    case ANC_SESSION_STATE_STARTING:
        {
            T_ANC_LLAPT_CFG config;

            anc_db.state = state;

            config.sub_type = ANC_IMAGE_SUB_TYPE_ANC_COEF;
            config.scenario_id = anc_db.scenario_id;
            audio_path_configure(anc_db.path_handle, &config);

            ret = audio_path_start(anc_db.path_handle);
            if (ret == false)
            {
                anc_db.state = ANC_SESSION_STATE_STOPPED;
            }
        }
        break;

    case ANC_SESSION_STATE_RELEASING:
        {
            anc_db.state = state;

            ret = audio_path_destroy(anc_db.path_handle);
            if (ret == false)
            {
                anc_db.state = ANC_SESSION_STATE_STOPPED;
            }
        }
        break;
    }

    return ret;
}


static bool anc_releasing_handler(T_ANC_SESSION_STATE state)
{
    bool ret = true;

    switch (state)
    {
    case ANC_SESSION_STATE_RELEASED:
        {
            anc_db.state  = state;
            anc_db.action = ANC_ACTION_NONE;
            anc_db.path_handle = NULL;
            ret = true;
        }
        break;

    case ANC_SESSION_STATE_CREATING:
    case ANC_SESSION_STATE_CREATED:
    case ANC_SESSION_STATE_STARTING:
    case ANC_SESSION_STATE_STARTED:
    case ANC_SESSION_STATE_STOPPING:
    case ANC_SESSION_STATE_STOPPED:
        {
            ret = false;
        }
        break;

    case ANC_SESSION_STATE_RELEASING:
        {
            /* Drop the repeating release action. But this case is permitted. */
            ret = true;
        }
        break;
    }

    return ret;
}

static const T_ANC_STATE_HANDLER anc_state_handler[] =
{
    anc_released_handler,
    anc_creating_handler,
    anc_created_handler,
    anc_starting_handler,
    anc_started_handler,
    anc_stopping_handler,
    anc_stopped_handler,
    anc_releasing_handler,
};

static bool anc_state_set(T_ANC_SESSION_STATE state)
{
    AUDIO_PRINT_INFO4("anc_state_set: path_handle %p, curr state %u, next state %u, action %u",
                      anc_db.path_handle, anc_db.state, state, anc_db.action);

    return anc_state_handler[anc_db.state](state);
}

static bool anc_path_cback(T_AUDIO_PATH_HANDLE path_handle,
                           T_AUDIO_PATH_EVENT  event,
                           uint32_t            param)
{
    bool ret = false;

    AUDIO_PRINT_TRACE3("anc_path_cback: handle %p, event 0x%02x, param 0x%08x",
                       path_handle, event, param);

    switch (event)
    {
    case AUDIO_PATH_EVT_RELEASE:
        {
            ret = anc_state_set(ANC_SESSION_STATE_RELEASED);
        }
        break;

    case AUDIO_PATH_EVT_CREATE:
        {
            ret = anc_state_set(ANC_SESSION_STATE_CREATED);
        }
        break;

    case AUDIO_PATH_EVT_RUNNING:
        {
            ret = anc_state_set(ANC_SESSION_STATE_STARTED);
        }
        break;

    case AUDIO_PATH_EVT_IDLE:
        {
            ret = anc_state_set(ANC_SESSION_STATE_STOPPED);
        }
        break;

    default:
        break;
    }

    return ret;
}

bool anc_init(void)
{
    int32_t ret = 0;

    anc_db.state                   = ANC_SESSION_STATE_RELEASED;
    anc_db.action                  = ANC_ACTION_NONE;
    anc_db.scenario_id             = 0;
    anc_db.volume_out              = ANC_VOLUME_OUT_LEVEL_DEFAULT;
    anc_db.volume_in               = ANC_VOLUME_IN_LEVEL_DEFAULT;

    if (anc_state_set(ANC_SESSION_STATE_CREATING) == false)
    {
        ret = 1;
        goto fail_set_state;
    }

    return true;

fail_set_state:
    AUDIO_PRINT_ERROR1("anc_init: failed %d", -ret);
    return false;
}

void anc_deinit(void)
{
    if (anc_db.path_handle != NULL)
    {
        anc_state_set(ANC_SESSION_STATE_RELEASING);
    }
}

bool anc_enable(uint8_t scenario_id)
{
    anc_db.scenario_id = scenario_id;
    return anc_state_set(ANC_SESSION_STATE_STARTING);
}

bool anc_disable(void)
{
    return anc_state_set(ANC_SESSION_STATE_STOPPING);
}

T_ANC_STATE anc_state_get(void)
{
    T_ANC_STATE state;

    switch (anc_db.state)
    {
    case ANC_SESSION_STATE_RELEASED:
    case ANC_SESSION_STATE_RELEASING:
    case ANC_SESSION_STATE_STOPPED:
        state = ANC_STATE_STOPPED;
        break;

    case ANC_SESSION_STATE_CREATING:
    case ANC_SESSION_STATE_CREATED:
    case ANC_SESSION_STATE_STARTING:
        state = ANC_STATE_STARTING;
        break;

    case ANC_SESSION_STATE_STARTED:
        state = ANC_STATE_STARTED;
        break;

    case ANC_SESSION_STATE_STOPPING:
        state = ANC_STATE_STOPPING;
        break;

    default:
        state = ANC_STATE_STOPPED;
        break;
    }

    return state;
}

bool anc_route_path_take(uint32_t            device,
                         T_AUDIO_ROUTE_PATH *path)
{
    int32_t ret = 0;

    *path = audio_route_path_get(anc_db.path_handle,
                                 AUDIO_CATEGORY_ANC,
                                 device);
    if (path->entry == NULL)
    {
        ret = 1;
        goto fail_get_route;
    }

    return true;

fail_get_route:
    AUDIO_PRINT_ERROR2("anc_route_path_take: failed %d, device 0x%08x",
                       -ret, device);
    return false;
}

void anc_route_path_give(T_AUDIO_ROUTE_PATH *path)
{
    audio_route_path_give(path);
}

bool anc_route_bind(T_AUDIO_ROUTE_PATH path)
{
    return audio_route_path_bind(anc_db.path_handle,
                                 path);
}

bool anc_route_unbind(void)
{
    return audio_route_path_unbind(anc_db.path_handle);
}

bool anc_route_add(T_AUDIO_ROUTE_ENTRY *entry)
{
    return audio_route_path_add(anc_db.path_handle,
                                entry);
}

bool anc_route_remove(T_AUDIO_ROUTE_IO_TYPE io_type)
{
    return audio_route_path_remove(anc_db.path_handle,
                                   io_type);
}

#else   /* CONFIG_REALTEK_AM_ANC_SUPPORT */

#include <stdbool.h>
#include <stdint.h>
#include "anc.h"

bool anc_init(void)
{
    return true;
}

void anc_deinit(void)
{
    return;
}

bool anc_enable(uint8_t scenario_id)
{
    return false;
}

bool anc_disable(void)
{
    return false;
}

T_ANC_STATE anc_state_get(void)
{
    return ANC_STATE_STOPPED;
}

bool anc_route_path_take(uint32_t            device,
                         T_AUDIO_ROUTE_PATH *path)
{
    return false;
}

void anc_route_path_give(T_AUDIO_ROUTE_PATH *path)
{
    return;
}

bool anc_route_bind(T_AUDIO_ROUTE_PATH path)
{
    return false;
}

bool anc_route_unbind(void)
{
    return false;
}

bool anc_route_add(T_AUDIO_ROUTE_ENTRY *entry)
{
    return false;
}

bool anc_route_remove(T_AUDIO_ROUTE_IO_TYPE io_type)
{
    return false;
}

#endif  /* CONFIG_REALTEK_AM_ANC_SUPPORT */
