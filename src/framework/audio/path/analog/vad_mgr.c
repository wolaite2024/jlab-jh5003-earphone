/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "trace.h"
#include "audio_mgr.h"
#include "vad_mgr.h"
#include "vad_driver.h"

typedef enum t_vad_mgr_session_state
{
    VAD_MGR_SESSION_STATE_DISABLED    = 0x00,
    VAD_MGR_SESSION_STATE_ENABLING    = 0x01,
    VAD_MGR_SESSION_STATE_ENABLED     = 0x02,
    VAD_MGR_SESSION_STATE_DISABLING   = 0x03,
} T_VAD_MGR_SESSION_STATE;

typedef enum t_vad_mgr_session_event
{
    VAD_MGR_SESSION_EVENT_ENABLE_REQ  = 0x00,
    VAD_MGR_SESSION_EVENT_ENABLE_RSP  = 0x01,
    VAD_MGR_SESSION_EVENT_DISABLE_REQ = 0x02,
    VAD_MGR_SESSION_EVENT_DISABLE_RSP = 0x03,
} T_VAD_MGR_SESSION_EVENT;

typedef enum t_vad_mgr_state
{
    VAD_MGR_STATE_OFF       = 0x00,
    VAD_MGR_STATE_WAIT_OFF  = 0x01,
    VAD_MGR_STATE_ON        = 0x02,
    VAD_MGR_STATE_WAIT_ON   = 0x03,
} T_VAD_MGR_STATE;

typedef enum t_vad_mgr_fsm_event
{
    VAD_MGR_EVENT_ENABLE_REQ    = 0x00,
    VAD_MGR_EVENT_ENABLE_RSP    = 0x01,
    VAD_MGR_EVENT_DISABLE_REQ   = 0x02,
    VAD_MGR_EVENT_DISABLE_RSP   = 0x03,
} T_VAD_MGR_FSM_EVENT;

typedef struct t_vad_mgr_session
{
    T_VAD_MGR_SESSION_CBACK     cback;
    void                       *owner;
    void                       *context;
    uint32_t                    sample_rate;
    T_VAD_TYPE                  type;
    T_VAD_MGR_SESSION_STATE     state;
} T_VAD_MGR_SESSION;

typedef struct t_vad_mgr_db
{
    T_VAD_MGR_SESSION *session;
    T_VAD_MGR_STATE    state;
} T_VAD_MGR_DB;

static T_VAD_MGR_DB vad_mgr_db = {0};

static void vad_mgr_session_fsm(T_VAD_MGR_SESSION       *session,
                                T_VAD_MGR_SESSION_EVENT  event);

static void vad_mgr_hw_fsm(T_VAD_MGR_FSM_EVENT event)
{
    CODEC_PRINT_INFO2("vad_mgr_hw_fsm: state %d, event %d", vad_mgr_db.state, event);

    switch (vad_mgr_db.state)
    {
    case VAD_MGR_STATE_OFF:
        {
            if (event == VAD_MGR_EVENT_ENABLE_REQ)
            {
                vad_mgr_db.state = VAD_MGR_STATE_WAIT_ON;
                vad_drv_enable();
            }
        }
        break;

    case VAD_MGR_STATE_WAIT_ON:
        {
            if (event == VAD_MGR_EVENT_ENABLE_RSP)
            {
                vad_mgr_db.state = VAD_MGR_STATE_ON;
                vad_mgr_session_fsm(vad_mgr_db.session,
                                    VAD_MGR_SESSION_EVENT_ENABLE_RSP);
            }
        }
        break;

    case VAD_MGR_STATE_ON:
        {
            if (event == VAD_MGR_EVENT_DISABLE_REQ)
            {
                vad_mgr_db.state = VAD_MGR_STATE_WAIT_ON;
                vad_drv_disable();
            }
        }
        break;

    case VAD_MGR_STATE_WAIT_OFF:
        {
            if (event == VAD_MGR_EVENT_DISABLE_RSP)
            {
                vad_mgr_db.state = VAD_MGR_STATE_OFF;
                vad_mgr_session_fsm(vad_mgr_db.session,
                                    VAD_MGR_SESSION_EVENT_DISABLE_RSP);
            }
        }
        break;
    }
}

void vad_mgr_session_enable(T_VAD_MGR_SESSION_HANDLE handle)
{
    T_VAD_MGR_SESSION *session;

    CODEC_PRINT_INFO1("vad_mgr_session_enable: handle %p", handle);

    session = (T_VAD_MGR_SESSION *)handle;
    if (vad_mgr_db.session == session)
    {
        vad_mgr_session_fsm(session, VAD_MGR_SESSION_EVENT_ENABLE_REQ);
    }
}

void vad_mgr_session_disable(T_VAD_MGR_SESSION_HANDLE handle)
{
    T_VAD_MGR_SESSION *session;

    CODEC_PRINT_INFO1("vad_mgr_session_disable: handle %p", handle);

    session = (T_VAD_MGR_SESSION *)handle;
    if (vad_mgr_db.session == session)
    {
        vad_mgr_session_fsm(session, VAD_MGR_SESSION_EVENT_DISABLE_REQ);
    }
}

T_VAD_MGR_SESSION_HANDLE vad_mgr_session_create(T_VAD_TYPE               type,
                                                uint32_t                 sample_rate,
                                                T_VAD_MGR_SESSION_CBACK  cback,
                                                void                    *context)
{
    T_VAD_MGR_SESSION *session;

    session = calloc(1, sizeof(T_VAD_MGR_SESSION));
    if (session != NULL)
    {
        session->state       = VAD_MGR_SESSION_STATE_DISABLED;
        session->cback       = cback;
        session->context     = context;
        session->sample_rate = sample_rate;
        session->type        = type;
        vad_mgr_db.session   = session;
    }

    return session;
}

void vad_mgr_session_destroy(T_VAD_MGR_SESSION_HANDLE handle)
{
    T_VAD_MGR_SESSION *session = (T_VAD_MGR_SESSION *)handle;

    if (vad_mgr_db.session == session)
    {
        free(session);
        vad_mgr_db.session = NULL;
    }
}

static void vad_mgr_session_fsm(T_VAD_MGR_SESSION       *session,
                                T_VAD_MGR_SESSION_EVENT  event)
{
    CODEC_PRINT_INFO3("vad_mgr_session_fsm: session %p, state %d, event %d",
                      session,  session->state, event);

    switch (session->state)
    {
    case VAD_MGR_SESSION_STATE_DISABLED:
        {
            if (event == VAD_MGR_SESSION_EVENT_ENABLE_REQ)
            {
                if (session->type != VAD_TYPE_SW)
                {
                    T_AUDIO_ROUTE_PATH  path;

                    session->state = VAD_MGR_SESSION_STATE_ENABLING;

                    path = audio_route_path_get(session->owner,
                                                AUDIO_CATEGORY_VAD,
                                                AUDIO_DEVICE_IN_MIC);
                    if (path.entry_num != 0)
                    {
                        for (uint8_t i = 0; i < path.entry_num; i++)
                        {
                            if (path.entry[i].io_type == AUDIO_ROUTE_IO_VAD_PRIMARY_IN)
                            {
                                vad_drv_param_config((T_VAD_DRV_ADC_CHANNEL_SEL)path.entry[i].endpoint_attr.mic.adc_ch,
                                                     session->sample_rate);
                                break;
                            }
                        }

                        audio_route_path_give(&path);
                    }

                    vad_mgr_hw_fsm(VAD_MGR_EVENT_ENABLE_REQ);
                }
            }
        }
        break;

    case VAD_MGR_SESSION_STATE_ENABLING:
        {
            if (event == VAD_MGR_SESSION_EVENT_ENABLE_RSP)
            {
                session->state = VAD_MGR_SESSION_STATE_ENABLED;

                session->cback(session,
                               VAD_MGR_EVENT_SESSION_ENABLED,
                               (uint32_t)session->context);
            }
        }
        break;

    case VAD_MGR_SESSION_STATE_ENABLED:
        {
            if (event == VAD_MGR_SESSION_EVENT_DISABLE_REQ)
            {
                session->state = VAD_MGR_SESSION_STATE_DISABLING;

                vad_mgr_session_fsm(session,
                                    VAD_MGR_SESSION_EVENT_DISABLE_RSP);
            }
        }
        break;

    case VAD_MGR_SESSION_STATE_DISABLING:
        {
            if (event == VAD_MGR_SESSION_EVENT_DISABLE_RSP)
            {
                session->state = VAD_MGR_SESSION_STATE_DISABLED;

                session->cback(session,
                               VAD_MGR_EVENT_SESSION_DISABLED,
                               (uint32_t)session->context);
            }
        }
        break;
    }
}

static void vad_mgr_drv_cback(T_VAD_DRV_EVENT event, void *param)
{
    switch (event)
    {
    case VAD_DRV_EVENT_DISABLED :
        {
            vad_mgr_hw_fsm(VAD_MGR_EVENT_DISABLE_RSP);
        }
        break;

    case VAD_DRV_EVENT_ENABLED :
        {
            vad_mgr_hw_fsm(VAD_MGR_EVENT_ENABLE_RSP);
        }
        break;
    }
}

bool vad_mgr_init(void)
{
    vad_mgr_db.state   = VAD_MGR_STATE_OFF;
    vad_mgr_db.session = NULL;

    return vad_drv_init(vad_mgr_drv_cback);
}

void vad_mgr_deinit(void)
{
    if (vad_mgr_db.session != NULL)
    {
        free(vad_mgr_db.session);
        vad_mgr_db.session = NULL;
    }
}

bool vad_mgr_path_bind(T_VAD_MGR_SESSION_HANDLE  handle,
                       void                     *owner)
{
    T_VAD_MGR_SESSION *session;

    session = (T_VAD_MGR_SESSION *)handle;
    if (vad_mgr_db.session == session)
    {
        session->owner = owner;
        return true;
    }

    return false;
}

bool vad_mgr_path_unbind(T_VAD_MGR_SESSION_HANDLE  handle,
                         void                     *owner)
{
    T_VAD_MGR_SESSION *session;

    session = (T_VAD_MGR_SESSION *)handle;
    if (vad_mgr_db.session == session)
    {
        if (session->owner == owner)
        {
            session->owner = NULL;
            return true;
        }
    }

    return false;
}
