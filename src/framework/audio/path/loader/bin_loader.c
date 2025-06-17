/*
 * Copyright (c) 2021, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "os_msg.h"
#include "os_queue.h"
#include "trace.h"
#include "section.h"
#include "bin_loader.h"
#include "bin_loader_driver.h"

/* TODO Remove Start */
#include "app_msg.h"
#include "sys_mgr.h"

extern void *hEventQueueHandleAu;
/* TODO Remove End */

#define BIN_LOADER_MSG_MAX_NUM     (0x08)

typedef enum t_bin_loader_state
{
    BIN_LOADER_STATE_IDLE,
    BIN_LOADER_STATE_BUSY,
} T_BIN_LOADER_STATE;

typedef struct t_bin_loader_session
{
    struct t_bin_loader_session *p_next;
    P_BIN_LOADER_CBACK           cback;
} T_BIN_LOADER_SESSION;

typedef struct t_bin_loader_token
{
    struct t_bin_loader_token *p_next;
    T_BIN_LOADER_SESSION      *session;
    uint32_t                   id;
    void                      *context;
} T_BIN_LOADER_TOKEN;

typedef enum t_bin_loader_msg_type
{
    BIN_LOADER_MSG_CONTINUE = 0x00,
    BIN_LOADER_MSG_COMPLETE = 0x01,
    BIN_LOADER_MSG_FAIL     = 0x02,
} T_BIN_LOADER_MSG_TYPE;

typedef struct t_bin_loader_msg
{
    T_BIN_LOADER_MSG_TYPE   type;
    uint16_t                data_len;
    void                   *p_data;
} T_BIN_LOADER_MSG;

typedef struct t_bin_loader_db
{
    T_OS_QUEUE          session_list;
    T_OS_QUEUE          token_list;
    T_BIN_LOADER_STATE  state;
    void               *queue_handle;
} T_BIN_LOADER_DB;

static T_BIN_LOADER_DB bin_loader_db = {0};

RAM_TEXT_SECTION
void bin_loader_msg_send(T_BIN_LOADER_MSG_TYPE  type,
                         void                  *p_data,
                         uint16_t               len)
{
    T_BIN_LOADER_MSG msg;
    uint8_t          event = EVENT_BIN_LOADER_MSG;
    int32_t          ret = 0;

    msg.type     = type;
    msg.p_data   = p_data;
    msg.data_len = len;

    if (hEventQueueHandleAu == NULL)
    {
        ret = 1;
        goto send_fail;
    }

    if (os_msg_send(bin_loader_db.queue_handle, &msg, 0) == false)
    {
        ret = 2;
        goto send_fail;
    }

    if (os_msg_send(hEventQueueHandleAu, &event, 0) == false)
    {
        ret = 3;
        goto send_fail;
    }

    return;

send_fail:
    LOADER_PRINT_ERROR4("bin_loader_msg_send: failed %d, type 0x%02x, p_data %p, len %u",
                        -ret, type, p_data, len);
}

bool bin_loader_session_check(T_BIN_LOADER_SESSION *session)
{
    return os_queue_search(&bin_loader_db.session_list, session);
}

T_BIN_LOADER_SESSION_HANDLE bin_loader_session_create(P_BIN_LOADER_CBACK cback)
{
    T_BIN_LOADER_SESSION *session;

    session = malloc(sizeof(T_BIN_LOADER_SESSION));
    if (session != NULL)
    {
        session->cback = cback;
        os_queue_in(&bin_loader_db.session_list, session);
    }

    return (T_BIN_LOADER_SESSION_HANDLE)session;
}

bool bin_loader_session_destroy(T_BIN_LOADER_SESSION_HANDLE handle)
{
    T_BIN_LOADER_SESSION *session;

    session = (T_BIN_LOADER_SESSION *)handle;
    if (bin_loader_session_check(session) == true)
    {
        T_BIN_LOADER_TOKEN *curr_token;
        T_BIN_LOADER_TOKEN *next_token;

        curr_token = os_queue_peek(&bin_loader_db.token_list, 0);
        while (curr_token != NULL)
        {
            next_token = curr_token->p_next;

            if (curr_token->session == session)
            {
                os_queue_delete(&bin_loader_db.token_list, curr_token);
            }

            curr_token = next_token;
        }

        os_queue_delete(&bin_loader_db.session_list, session);
        free(session);
        return true;
    }

    return false;
}

bool bin_loader_token_issue(T_BIN_LOADER_SESSION_HANDLE  handle,
                            uint32_t                     id,
                            void                        *context)
{
    T_BIN_LOADER_SESSION *session;
    T_BIN_LOADER_TOKEN   *token;
    int32_t               ret = 0;

    session = (T_BIN_LOADER_SESSION *)handle;

    if (bin_loader_session_check(session) == false)
    {
        ret = 1;
        goto fail_check_session;
    }

    token = malloc(sizeof(T_BIN_LOADER_TOKEN));
    if (token == NULL)
    {
        ret = 2;
        goto fail_alloc_token;
    }

    token->session = session;
    token->id      = id;
    token->context = context;
    os_queue_in(&bin_loader_db.token_list, token);
    LOADER_PRINT_TRACE5("bin_loader_token_issue: handle %p, id 0x%08x, context %p, token 0x%08x, state %d",
                        handle, id, context, token, bin_loader_db.state);

    if (bin_loader_db.state == BIN_LOADER_STATE_IDLE)
    {
        bin_loader_db.state = BIN_LOADER_STATE_BUSY;
        bin_loader_drv_image_load(id);
        session->cback(session, BIN_LOADER_EVENT_EXECUTED, id, context);
    }

    return true;

fail_alloc_token:
fail_check_session:
    LOADER_PRINT_TRACE2("bin_loader_token_issue: handle %p, failed %d", handle, -ret);
    return false;
}

void bin_loader_msg_handler(void)
{
    T_BIN_LOADER_MSG msg;

    if (os_msg_recv(bin_loader_db.queue_handle, &msg, 0) == true)
    {
        T_BIN_LOADER_TOKEN *token;

        LOADER_PRINT_TRACE2("bin_loader_msg_handler: type 0x%02x, state 0x%02x",
                            msg.type, bin_loader_db.state);

        switch (msg.type)
        {
        case BIN_LOADER_MSG_CONTINUE:
            {
                token = os_queue_peek(&bin_loader_db.token_list, 0);
                if (token != NULL)
                {
                    bin_loader_db.state = BIN_LOADER_STATE_IDLE;
                    token->session->cback(token->session,
                                          BIN_LOADER_EVENT_CONTINUED,
                                          token->id,
                                          token->context);
                }
            }
            break;

        case BIN_LOADER_MSG_COMPLETE:
        case BIN_LOADER_MSG_FAIL:
            {
                token = os_queue_out(&bin_loader_db.token_list);
                LOADER_PRINT_TRACE1("bin_loader_msg_handler: token out: token 0x%08x",
                                    token);

                if (token != NULL)
                {
                    bin_loader_db.state = BIN_LOADER_STATE_IDLE;

                    if (msg.type == BIN_LOADER_MSG_COMPLETE)
                    {
                        token->session->cback(token->session,
                                              BIN_LOADER_EVENT_COMPLETED,
                                              token->id,
                                              token->context);
                    }
                    else
                    {
                        token->session->cback(token->session,
                                              BIN_LOADER_EVENT_FAILED,
                                              token->id,
                                              token->context);
                    }
                    free(token);

                    if (bin_loader_db.state == BIN_LOADER_STATE_IDLE)
                    {
                        token = os_queue_peek(&bin_loader_db.token_list, 0);
                        LOADER_PRINT_TRACE1("bin_loader_msg_handler: token peek: token 0x%08x",
                                            token);

                        if (token != NULL)
                        {
                            bin_loader_db.state = BIN_LOADER_STATE_BUSY;
                            bin_loader_drv_image_load(token->id);
                            token->session->cback(token->session,
                                                  BIN_LOADER_EVENT_EXECUTED,
                                                  token->id,
                                                  token->context);
                        }
                    }
                }
            }
            break;

        default:
            break;
        }
    }
}

RAM_TEXT_SECTION static void bin_loader_driver_cback(T_BIN_LOADER_DRV_EVENT event)
{
    switch (event)
    {
    case BIN_LOADER_DRV_EVENT_CONTINUED:
        bin_loader_msg_send(BIN_LOADER_MSG_CONTINUE, NULL, 0);
        break;

    case BIN_LOADER_DRV_EVENT_COMPLETED:
        bin_loader_msg_send(BIN_LOADER_MSG_COMPLETE, NULL, 0);
        break;

    case BIN_LOADER_DRV_EVENT_FAILED:
        bin_loader_msg_send(BIN_LOADER_MSG_FAIL, NULL, 0);
        break;
    }
}

bool bin_loader_init(void)
{
    int32_t ret = 0;

    bin_loader_db.state = BIN_LOADER_STATE_IDLE;
    os_queue_init(&bin_loader_db.session_list);
    os_queue_init(&bin_loader_db.token_list);

    if (bin_loader_drv_init(bin_loader_driver_cback) == false)
    {
        ret = 1;
        goto fail_init_drv;
    }

    if (os_msg_queue_create(&bin_loader_db.queue_handle,
                            "bin loader",
                            BIN_LOADER_MSG_MAX_NUM,
                            sizeof(T_BIN_LOADER_MSG)) == false)
    {
        ret = 2;
        goto fail_create_queue;
    }

    sys_mgr_event_register(EVENT_BIN_LOADER_MSG, bin_loader_msg_handler);
    return true;

fail_create_queue:
    bin_loader_drv_deinit();
fail_init_drv:
    LOADER_PRINT_ERROR1("bin_loader_init: failed %d", -ret);
    return false;
}

void bin_loader_deinit(void)
{
    T_BIN_LOADER_SESSION *session;

    bin_loader_db.state = BIN_LOADER_STATE_IDLE;

    session = os_queue_out(&bin_loader_db.session_list);
    while (session != NULL)
    {
        bin_loader_session_destroy(session);
        session = os_queue_out(&bin_loader_db.session_list);
    }

    if (bin_loader_db.queue_handle != NULL)
    {
        os_msg_queue_delete(bin_loader_db.queue_handle);
    }

    bin_loader_drv_deinit();
}
