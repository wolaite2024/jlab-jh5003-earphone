/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "trace.h"
#include "os_queue.h"
#include "sys_ipc.h"
#include "sys_timer.h"
#include "remote.h"
#include "bt_rdtp.h"
#include "bt_mgr.h"

#define REMOTE_MSG_CREDITS              3
#define REMOTE_MSG_QUEUE_NUM            32
#define REMOTE_RELAY_PERIOD_MIN         30
#define REMOTE_RELAY_PERIOD_DEFAULT     60
#define REMOTE_RELAY_RETRANS_INTERVAL   10000 /* Microseconds */

typedef enum t_remote_relay_type
{
    REMOTE_RELAY_TYPE_ASYNC          = 0x00,
    REMOTE_RELAY_TYPE_ASYNC_LOOPBACK = 0x01,
    REMOTE_RELAY_TYPE_SYNC           = 0x02,
    REMOTE_RELAY_TYPE_SYNC_LOOPBACK  = 0x03,
} T_REMOTE_RELAY_TYPE;

typedef struct t_remote_relay_msg
{
    uint8_t             seq;
    uint8_t             cid;
    uint16_t            event;
    uint16_t            param_len;
    T_REMOTE_RELAY_TYPE relay_type;
    T_REMOTE_TIMER_TYPE timer_type;
    T_BT_CLK_REF        clk_ref;
    uint8_t             rsvd;
    uint16_t            time_duration;
    union
    {
        uint32_t        timer_period;
        uint32_t        sync_slot;
    } time_info;
    uint8_t             param[0];
} T_REMOTE_RELAY_MSG;

typedef struct t_remote_relay_item
{
    struct t_remote_relay_item *next;
    T_REMOTE_RELAY_MSG          msg;
} T_REMOTE_RELAY_ITEM;

typedef struct t_remote_timer
{
    struct t_remote_timer *next;
    void                  *timer_handle;
    T_REMOTE_TIMER_TYPE    timer_type;
    uint8_t                timer_index;
    uint16_t               event;
    uint8_t                rsvd[2];
    uint16_t               param_len;
    uint8_t                param[0];
} T_REMOTE_TIMER;

typedef struct t_remote_relay_chann
{
    struct t_remote_relay_chann *next;
    P_REMOTE_RELAY_CBACK         cback;
    uint8_t                      chann_id;
    uint8_t                      timer_index;
    T_OS_QUEUE                   timer_queue;
} T_REMOTE_RELAY_CHANN;

typedef struct t_remote_db
{
    T_REMOTE_SESSION_ROLE  session_role;
    T_REMOTE_SESSION_STATE session_state;
    uint8_t                local_addr[6];
    uint8_t                peer_addr[6];
    T_SYS_IPC_HANDLE       bt_ipc_handle;
    T_SYS_TIMER_HANDLE     retrans_timer_handle;
    uint8_t                tx_seq;
    uint8_t                rx_seq;
    uint8_t                credits;
    uint8_t                chann_id;       /* Next free relay channel id */
    T_OS_QUEUE             chann_list;     /* Relay channel list */
    T_OS_QUEUE             relay_queue;    /* Relay queue for pending messages */
    uint32_t               period;         /* Default relay timer period */
} T_REMOTE_DB;

static T_REMOTE_DB *remote_db = NULL;

bool remote_session_role_set(T_REMOTE_SESSION_ROLE role)
{
    if (remote_db != NULL)
    {
        REMOTE_PRINT_TRACE1("remote_session_role_set: role %u", role);

        remote_db->session_role = role;
        return true;
    }

    return false;
}

T_REMOTE_SESSION_ROLE remote_session_role_get(void)
{
    if (remote_db != NULL)
    {
        return remote_db->session_role;
    }

    return REMOTE_SESSION_ROLE_SINGLE;
}

bool remote_local_addr_set(uint8_t bd_addr[6])
{
    if (remote_db != NULL)
    {
        REMOTE_PRINT_TRACE1("remote_local_addr_set: bd_addr %s", TRACE_BDADDR(bd_addr));

        memcpy(remote_db->local_addr, bd_addr, 6);
        return true;
    }

    return false;
}

bool remote_local_addr_get(uint8_t bd_addr[6])
{
    if (remote_db != NULL)
    {
        memcpy(bd_addr, remote_db->local_addr, 6);
        return true;
    }

    return false;
}

bool remote_peer_addr_set(uint8_t bd_addr[6])
{
    if (remote_db != NULL)
    {
        REMOTE_PRINT_TRACE1("remote_peer_addr_set: bd_addr %s", TRACE_BDADDR(bd_addr));

        memcpy(remote_db->peer_addr, bd_addr, 6);
        return true;
    }

    return false;
}

bool remote_peer_addr_get(uint8_t bd_addr[6])
{
    if (remote_db != NULL)
    {
        memcpy(bd_addr, remote_db->peer_addr, 6);
        return true;
    }

    return false;
}

bool remote_session_open(uint8_t bd_addr[6])
{
    return bt_rdtp_connect_req(bd_addr);
}

bool remote_session_close(uint8_t bd_addr[6])
{
    return bt_rdtp_disconnect_req(bd_addr);
}

static bool remote_relay_chann_check(T_REMOTE_RELAY_CHANN *chann)
{
    if (chann != NULL)
    {
        return os_queue_search(&remote_db->chann_list, chann);
    }

    return false;
}

static T_REMOTE_RELAY_CHANN *remote_relay_chann_find(uint8_t chann_id)
{
    T_REMOTE_RELAY_CHANN *chann;

    chann = os_queue_peek(&remote_db->chann_list, 0);
    while (chann != NULL)
    {
        if (chann->chann_id == chann_id)
        {
            break;
        }

        chann = chann->next;
    }

    return chann;
}

void remote_relay_lp_timer_cback(void *handle)
{
    T_REMOTE_RELAY_CHANN *chann;
    T_REMOTE_TIMER       *timer;
    uint32_t              timer_id;
    uint8_t               chann_id;
    uint8_t               timer_index;
    uint16_t              msg_id;

    timer_id = sys_timer_id_get(handle);

    chann_id    = (uint8_t)(timer_id >> 24);
    timer_index = (uint8_t)(timer_id >> 16);
    msg_id      = (uint16_t)timer_id;

    chann = remote_relay_chann_find(chann_id);
    if (chann != NULL)
    {
        timer = os_queue_peek(&chann->timer_queue, 0);
        while (timer != NULL)
        {
            if (timer->timer_type == REMOTE_TIMER_LOW_PRECISION &&
                timer->timer_index == timer_index &&
                timer->event == msg_id)
            {
                break;
            }

            timer = timer->next;
        }

        if (timer != NULL)
        {
            sys_timer_delete(timer->timer_handle);
            os_queue_delete(&chann->timer_queue, timer);

            if (chann->cback != NULL)
            {
                chann->cback(timer->event,
                             REMOTE_RELAY_STATUS_SYNC_TOUT,
                             timer->param,
                             timer->param_len);
            }

            free(timer);
        }
    }
    else
    {
        REMOTE_PRINT_ERROR1("remote_relay_lp_timer_cback: invalid timer_id 0x%08x", timer_id);
    }
}

void remote_relay_hp_timer_cback(T_SYS_TIMER_HANDLE handle)
{
    T_REMOTE_RELAY_CHANN *chann;
    T_REMOTE_TIMER       *timer;
    uint32_t              timer_id;
    uint8_t               chann_id;
    uint8_t               timer_index;
    uint16_t              msg_id;

    timer_id = sys_timer_id_get(handle);

    chann_id    = (uint8_t)(timer_id >> 24);
    timer_index = (uint8_t)(timer_id >> 16);
    msg_id      = (uint16_t)timer_id;

    chann = remote_relay_chann_find(chann_id);
    if (chann != NULL)
    {
        timer = os_queue_peek(&chann->timer_queue, 0);
        while (timer != NULL)
        {
            if (timer->timer_type == REMOTE_TIMER_HIGH_PRECISION &&
                timer->timer_index == timer_index &&
                timer->event == msg_id)
            {
                break;
            }

            timer = timer->next;
        }

        if (timer != NULL)
        {
            sys_timer_delete(timer->timer_handle);
            os_queue_delete(&chann->timer_queue, timer);

            if (chann->cback != NULL)
            {
                chann->cback(timer->event,
                             REMOTE_RELAY_STATUS_SYNC_TOUT,
                             timer->param,
                             timer->param_len);
            }

            free(timer);
        }
    }
    else
    {
        REMOTE_PRINT_ERROR1("remote_relay_hp_timer_cback: invalid timer_id 0x%08x", timer_id);
    }
}

void remote_msg_queue_clear(void)
{
    T_REMOTE_RELAY_ITEM   *item;
    T_REMOTE_RELAY_MSG    *msg;
    T_REMOTE_RELAY_CHANN  *chann;
    T_REMOTE_RELAY_STATUS  relay_status;

    item = os_queue_out(&remote_db->relay_queue);
    while (item != NULL)
    {
        msg = &item->msg;
        chann = remote_relay_chann_find(msg->cid);

        if (msg->relay_type == REMOTE_RELAY_TYPE_SYNC)
        {
            relay_status = REMOTE_RELAY_STATUS_SYNC_SENT_FAILED;
        }
        else
        {
            relay_status = REMOTE_RELAY_STATUS_ASYNC_SENT_FAILED;
        }

        chann->cback(msg->event, relay_status, msg->param, msg->param_len);

        free(item);
        item = os_queue_out(&remote_db->relay_queue);
    }
}

bool remote_raise_cmd_timer(T_REMOTE_RELAY_MSG *remote_cmd)
{
    T_REMOTE_RELAY_CHANN *chann;
    T_REMOTE_TIMER       *timer;
    uint32_t              bb_clock_slot; /* unit 312.5us */
    uint16_t              bb_clock_us;   /* unit 1us */
    uint32_t              sync_time_slot;
    uint32_t              time_offset;
    uint32_t              timer_id;
    uint8_t               clk_idx;
    int32_t               ret = 0;

    timer = calloc(1, sizeof(T_REMOTE_TIMER) + remote_cmd->param_len);
    if (timer == NULL)
    {
        return false;   /* NOTE leak */
    }
    memcpy(timer->param, remote_cmd->param, remote_cmd->param_len);
    timer->param_len = remote_cmd->param_len;
    timer->event = remote_cmd->event;
    chann = remote_relay_chann_find(remote_cmd->cid);
    timer->timer_index = chann->timer_index++;
    timer->timer_type = remote_cmd->timer_type;

    timer_id = (chann->chann_id << 24) | (timer->timer_index << 16) | remote_cmd->event;

    sync_time_slot = remote_cmd->time_info.sync_slot;

    if (bt_piconet_clk_get(remote_cmd->clk_ref,
                           &clk_idx,
                           &bb_clock_slot,
                           &bb_clock_us) != BT_CLK_NONE)
    {
        if ((sync_time_slot > bb_clock_slot &&
             ((sync_time_slot - bb_clock_slot) < 0x07ffffff))
            ||  /* in case overlap */
            (sync_time_slot <= bb_clock_slot  &&
             ((bb_clock_slot - sync_time_slot) > 0x07ffffff)))  /* consider wrap, 28 bit bt_clk /2 */
        {
            /* <<4 : bit26 is the MSB of BTclk, <<4 to shift bit 26 to bit 31 */
            time_offset = ((sync_time_slot << 4) - (bb_clock_slot << 4)) >> 4;  /* slots */
            time_offset = (time_offset * 625) >> 1;  /* us */
            if (time_offset < bb_clock_us)
            {
                ret = 1;
                time_offset = 0;
            }
            else
            {
                time_offset -= bb_clock_us;
            }
        }
        else
        {
            ret = 2;
            time_offset = 0;
        }
    }
    else
    {
        ret = 3;
        time_offset = 0;
    }

    if (ret)
    {
        REMOTE_PRINT_WARN1("remote_raise_cmd_timer: err %d", -ret);
    }

    if (timer->timer_type == REMOTE_TIMER_LOW_PRECISION)
    {
        timer->timer_handle = sys_timer_create("remote_lp_relay",
                                               SYS_TIMER_TYPE_LOW_PRECISION,
                                               timer_id,
                                               time_offset,
                                               false,
                                               remote_relay_lp_timer_cback);

    }
    else if (timer->timer_type == REMOTE_TIMER_HIGH_PRECISION)
    {
        timer->timer_handle = sys_timer_create("remote_hp_relay",
                                               SYS_TIMER_TYPE_HIGH_PRECISION,
                                               timer_id,
                                               time_offset,
                                               false,
                                               remote_relay_hp_timer_cback);
    }

    REMOTE_PRINT_TRACE5("remote_raise_cmd_timer: timer_type %u, cid 0x%02x, msg_id 0x%04x, interval %u, sync_time_slot 0x%x",
                        timer->timer_type, chann->chann_id, remote_cmd->event, time_offset, sync_time_slot);

    if (timer->timer_handle != NULL)
    {
        os_queue_in(&chann->timer_queue, timer);
        sys_timer_start(timer->timer_handle);
        return true;
    }
    else
    {
        free(timer);
        return false;
    }
}

static bool remote_msg_enqueue(T_REMOTE_RELAY_ITEM *item)
{
    if (remote_db->relay_queue.count < REMOTE_MSG_QUEUE_NUM)
    {
        os_queue_in(&remote_db->relay_queue, item);
        return true;
    }

    return false;
}

static void remote_msg_dequeue(void)
{
    T_REMOTE_RELAY_ITEM   *item;
    T_REMOTE_RELAY_MSG    *msg;
    T_REMOTE_SESSION_MODE  session_mode;
    bool                   ret;

    if (remote_session_mode_get(&session_mode) == false)
    {
        return;
    }

    if (session_mode != REMOTE_SESSION_MODE_ACTIVE)
    {
        remote_session_mode_set(REMOTE_SESSION_MODE_ACTIVE);
        return;
    }

    item = os_queue_peek(&remote_db->relay_queue, 0);
    while (item != NULL)
    {
        T_REMOTE_RELAY_STATUS relay_status;

        if (remote_db->credits == 0)
        {
            break;
        }

        msg = &item->msg;

        if (msg->relay_type == REMOTE_RELAY_TYPE_SYNC)
        {
            uint32_t     bb_clock_slot; /* unit 312.5us */
            uint16_t     bb_clock_us;   /* unit 1us */
            uint32_t     time_offset;
            T_BT_CLK_REF clk_ref;
            uint8_t      clk_idx;

            relay_status = REMOTE_RELAY_STATUS_SYNC_SENT_OUT;

            clk_ref = bt_piconet_clk_get(BT_CLK_NONE, &clk_idx, &bb_clock_slot, &bb_clock_us);
            if (msg->clk_ref != clk_ref)
            {
                msg->clk_ref = clk_ref;
                time_offset =  2 * REMOTE_RELAY_PERIOD_MIN * 1000 / 625;
            }
            else
            {
                time_offset = ((msg->time_info.sync_slot << 4) - (bb_clock_slot << 4)) >> 4;
                if ((time_offset < 2 * REMOTE_RELAY_PERIOD_MIN * 1000 / 625) ||
                    (time_offset > 0x7ffffff))
                {
                    time_offset = 2 * REMOTE_RELAY_PERIOD_MIN * 1000 / 625;
                }
                else if (time_offset > msg->time_duration)
                {
                    time_offset = msg->time_duration;
                }
            }

            msg->time_info.sync_slot = bb_clock_slot + time_offset;
            msg->time_duration = time_offset;
        }
        else
        {
            relay_status = REMOTE_RELAY_STATUS_ASYNC_SENT_OUT;
        }

        ret = bt_rdtp_data_send(remote_db->peer_addr,
                                msg,
                                sizeof(T_REMOTE_RELAY_MSG) + msg->param_len,
                                false);
        if (ret == true)
        {
            T_REMOTE_RELAY_CHANN *chann;

            remote_db->credits--;
            os_queue_delete(&remote_db->relay_queue, item);

            chann = remote_relay_chann_find(msg->cid);
            if (chann != NULL)
            {
                if (msg->relay_type == REMOTE_RELAY_TYPE_SYNC)
                {
                    remote_raise_cmd_timer(msg);
                }

                chann->cback(msg->event, relay_status, msg->param, msg->param_len);
            }

            free(item);
        }
        else
        {
            REMOTE_PRINT_WARN1("remote_msg_dequeue: send past queued cmd failed remain %d",
                               remote_db->relay_queue.count);
            sys_timer_start(remote_db->retrans_timer_handle);
            break;
        }

        item = os_queue_peek(&remote_db->relay_queue, 0);
    }
}

static bool remote_msg_send(T_REMOTE_RELAY_MSG *msg_header,
                            uint8_t            *buf,
                            uint16_t            len)
{
    T_REMOTE_SESSION_MODE  session_mode;
    T_REMOTE_RELAY_ITEM   *item;
    T_REMOTE_RELAY_MSG    *msg;
    int32_t                ret = 0;

    if (remote_session_mode_get(&session_mode) == false)
    {
        ret = 1;
        goto fail_get_mode;
    }

    item = malloc(sizeof(T_REMOTE_RELAY_ITEM) + len);
    if (item == NULL)
    {
        ret = 2;
        goto fail_alloc_item;
    }

    msg = &item->msg;
    memcpy(msg, msg_header, sizeof(T_REMOTE_RELAY_MSG));
    memcpy(msg->param, buf, len);

    if (remote_msg_enqueue(item) == false)
    {
        ret = 3;
        goto fail_enqueue_msg;
    }

    remote_msg_dequeue();

    return true;

fail_enqueue_msg:
    free(item);
fail_alloc_item:
fail_get_mode:
    REMOTE_PRINT_ERROR1("remote_msg_send: failed %d", -ret);
    return false;
}

bool remote_async_msg_relay(T_REMOTE_RELAY_HANDLE  handle,
                            uint16_t               msg_id,
                            void                  *msg_buf,
                            uint16_t               msg_len,
                            bool                   loopback)
{
    T_REMOTE_RELAY_MSG    relay_msg;
    T_REMOTE_RELAY_CHANN *chann;
    int32_t               ret = 0;

    if (remote_db == NULL)
    {
        ret = 1;
        goto fail_invalid_db;
    }

    if (remote_db->session_state != REMOTE_SESSION_STATE_CONNECTED)
    {
        ret = 2;
        goto fail_invalid_state;
    }

    chann = (T_REMOTE_RELAY_CHANN *)handle;
    if (remote_relay_chann_check(chann) == false)
    {
        ret = 3;
        goto fail_check_chann;
    }

    relay_msg.seq           = ++remote_db->tx_seq;
    relay_msg.cid           = chann->chann_id;
    relay_msg.rsvd          = 0;
    relay_msg.time_duration = 0;
    relay_msg.event         = msg_id;
    relay_msg.param_len     = msg_len;
    relay_msg.timer_type    = REMOTE_TIMER_DEFAULT;
    relay_msg.clk_ref       = BT_CLK_NONE;
    relay_msg.time_info.timer_period = 0;

    if (loopback == true)
    {
        relay_msg.relay_type = REMOTE_RELAY_TYPE_ASYNC_LOOPBACK;
    }
    else
    {
        relay_msg.relay_type = REMOTE_RELAY_TYPE_ASYNC;
    }

    REMOTE_PRINT_TRACE4("remote_async_msg_relay: handle %p, seq 0x%02x, msg_id 0x%04x, loopback %u",
                        handle, remote_db->tx_seq, msg_id, loopback);

    if (remote_msg_send(&relay_msg, msg_buf, msg_len) == false)
    {
        ret = 4;
        goto fail_send_msg;
    }

    return true;

fail_send_msg:
fail_check_chann:
fail_invalid_state:
fail_invalid_db:
    REMOTE_PRINT_ERROR4("remote_async_msg_relay: handle %p, msg_id 0x%04x, loopback %u, ret %d",
                        handle, msg_id, loopback, -ret);
    return false;
}

bool remote_sync_msg_relay(T_REMOTE_RELAY_HANDLE  handle,
                           uint16_t               msg_id,
                           void                  *msg_buf,
                           uint16_t               msg_len,
                           T_REMOTE_TIMER_TYPE    timer_type,
                           uint32_t               timer_period,
                           bool                   loopback)
{
    T_REMOTE_RELAY_MSG    relay_msg;
    T_REMOTE_RELAY_CHANN *chann;
    uint32_t              bb_clock_slot; /* unit 312.5us */
    uint16_t              bb_clock_us; /* unit 1us */
    uint32_t              sync_time_slot;
    int32_t               ret = 0;

    if (remote_db == NULL)
    {
        ret = 1;
        goto fail_invalid_db;
    }

    if (remote_db->session_state != REMOTE_SESSION_STATE_CONNECTED)
    {
        ret = 2;
        goto fail_invalid_state;
    }

    chann = (T_REMOTE_RELAY_CHANN *)handle;

    if (remote_relay_chann_check(chann) == false)
    {
        ret = 3;
        goto fail_check_chann;
    }

    if (timer_period == 0)
    {
        timer_period = remote_db->period;
    }

    if (loopback == true)
    {
        relay_msg.time_info.timer_period = timer_period;
        relay_msg.relay_type = REMOTE_RELAY_TYPE_SYNC_LOOPBACK;
        relay_msg.clk_ref = BT_CLK_NONE;
        relay_msg.time_duration = 0;
    }
    else
    {
        uint8_t clk_idx;

        relay_msg.clk_ref = bt_piconet_clk_get(BT_CLK_NONE, &clk_idx, &bb_clock_slot, &bb_clock_us);
        if (relay_msg.clk_ref == BT_CLK_NONE)
        {
            ret = 4;
            goto fail_get_clk;
        }

        sync_time_slot = bb_clock_slot + (2 * timer_period * 1000 / 625);
        sync_time_slot &= 0x0FFFFFFF;
        relay_msg.time_info.sync_slot = sync_time_slot;
        relay_msg.relay_type = REMOTE_RELAY_TYPE_SYNC;
        relay_msg.time_duration = 2 * timer_period * 1000 / 625;  /* slots */
    }

    relay_msg.seq       = ++remote_db->tx_seq;
    relay_msg.event     = msg_id;
    relay_msg.cid       = chann->chann_id;
    relay_msg.param_len = msg_len;
    relay_msg.rsvd      = 0;

    /* select low precision timer as the default timer type */
    if (timer_type == REMOTE_TIMER_DEFAULT)
    {
        relay_msg.timer_type = REMOTE_TIMER_LOW_PRECISION;
    }
    else
    {
        relay_msg.timer_type = timer_type;
    }

    REMOTE_PRINT_TRACE5("remote_sync_msg_relay: handle %p, seq 0x%02x, msg_id 0x%04x, timer_type %u, loopback %u",
                        handle, remote_db->tx_seq, msg_id, timer_type, loopback);

    if (remote_msg_send(&relay_msg, msg_buf, msg_len) == false)
    {
        ret = 5;
        goto fail_send_msg;
    }

    return true;

fail_send_msg:
fail_get_clk:
fail_check_chann:
fail_invalid_state:
fail_invalid_db:
    REMOTE_PRINT_ERROR5("remote_sync_msg_relay: handle %p, msg_id 0x%04x, timer_type %u, loopback %u, ret %d",
                        handle, msg_id, timer_type, loopback, -ret);
    return false;
}

void remote_relay_data_ind(uint8_t  *buf,
                           uint16_t  len)
{
    T_REMOTE_RELAY_MSG   *msg;
    T_REMOTE_RELAY_CHANN *chann;
    T_REMOTE_RELAY_TYPE   relay_type;
    uint8_t               seq;
    uint8_t               cid;
    uint16_t              event;
    int32_t               ret;

    msg   = (T_REMOTE_RELAY_MSG *)buf;

    seq        = msg->seq;
    cid        = msg->cid;
    event      = msg->event;
    relay_type = msg->relay_type;

    if (seq == remote_db->rx_seq)
    {
        ret = 1;
        goto fail_duplicate_seq;
    }

    if (seq != (uint8_t)(remote_db->rx_seq + 1))
    {
        REMOTE_PRINT_WARN2("remote_relay_data_ind: received seq 0x%02x, expected seq 0x%02x",
                           seq, (uint8_t)(remote_db->rx_seq + 1));
    }

    remote_db->rx_seq = seq;

    chann = remote_relay_chann_find(cid);
    if (chann == NULL)
    {
        ret = 2;
        goto fail_invalid_chann;
    }

    REMOTE_PRINT_TRACE3("remote_relay_data_ind: seq 0x%02x, event 0x%04x, cid %u",
                        seq, event, cid);

    if (relay_type == REMOTE_RELAY_TYPE_ASYNC)
    {
        chann->cback(event, REMOTE_RELAY_STATUS_ASYNC_RCVD, msg->param, msg->param_len);
    }
    else if (relay_type == REMOTE_RELAY_TYPE_ASYNC_LOOPBACK)
    {
        chann->cback(event, REMOTE_RELAY_STATUS_ASYNC_LOOPBACK, msg->param, msg->param_len);
        remote_async_msg_relay(chann,
                               event,
                               msg->param,
                               msg->param_len,
                               false);
    }
    else if (relay_type == REMOTE_RELAY_TYPE_SYNC)
    {
        uint32_t        bb_clock_slot;    /* 312.5us */
        uint16_t        bb_clock_us;      /* 1us */
        uint32_t        sync_time_slot;
        uint32_t        timer_id;
        uint32_t        time_offset;
        uint8_t         clk_idx;
        T_BT_CLK_REF    clk_ref;
        T_REMOTE_TIMER *timer;

        chann->cback(event, REMOTE_RELAY_STATUS_SYNC_RCVD, msg->param, msg->param_len);
        clk_ref = bt_piconet_clk_get(msg->clk_ref, &clk_idx, &bb_clock_slot, &bb_clock_us);
        if (clk_ref == BT_CLK_NONE || clk_ref != msg->clk_ref)
        {
            REMOTE_PRINT_WARN2("remote_relay_data_ind: clock ref changed remote %u local %u",
                               msg->clk_ref, clk_ref);
            if (msg->clk_ref == BT_CLK_SNIFFING)
            {
                clk_ref = bt_piconet_clk_get(BT_CLK_CTRL, &clk_idx, &bb_clock_slot, &bb_clock_us);
                if (clk_ref != BT_CLK_NONE)
                {
                    sync_time_slot = msg->time_info.sync_slot;
                    if (((sync_time_slot << 4) - (bb_clock_slot << 4)) < (msg->time_duration << 4))
                    {
                        msg->clk_ref = BT_CLK_CTRL;
                    }
                    else
                    {
                        chann->cback(event, REMOTE_RELAY_STATUS_SYNC_REF_CHANGED, msg->param, msg->param_len);
                        return;
                    }
                }
                else
                {
                    chann->cback(event, REMOTE_RELAY_STATUS_SYNC_REF_CHANGED, msg->param, msg->param_len);
                    return;
                }
            }
            else
            {
                chann->cback(event, REMOTE_RELAY_STATUS_SYNC_REF_CHANGED, msg->param, msg->param_len);
                return;
            }
        }
        sync_time_slot = msg->time_info.sync_slot;

        REMOTE_PRINT_INFO4("remote_relay_data_ind: sync_slot: %x, time_duration %x, bb_clk_slot %x, bb_clk_us %x",
                           sync_time_slot, msg->time_duration, bb_clock_slot, bb_clock_us);

        if ((sync_time_slot > bb_clock_slot &&
             (sync_time_slot - bb_clock_slot) < 0x07ffffff)
            ||  /* in case overlap */
            (sync_time_slot <= bb_clock_slot  &&
             (bb_clock_slot - sync_time_slot) > 0x07ffffff)) /* consider wrap, 28 bit bt_clk /2 */
        {
            /* <<4 : bit26 is the MSB of BTclk, <<4 to shift bit 26 to bit 31 */
            time_offset = ((sync_time_slot << 4) - (bb_clock_slot << 4)) >> 4; /* slots */
            if (time_offset > msg->time_duration)
            {
                chann->cback(event, REMOTE_RELAY_STATUS_SYNC_EXPIRED, msg->param, msg->param_len);
                return;
            }
            else
            {
                time_offset = (time_offset * 625) >> 1; /* us */
                if (time_offset < bb_clock_us)
                {
                    chann->cback(event, REMOTE_RELAY_STATUS_SYNC_EXPIRED, msg->param, msg->param_len);
                    return;
                }
                else
                {
                    time_offset -= bb_clock_us;
                }
            }
        }
        else
        {
            chann->cback(event, REMOTE_RELAY_STATUS_SYNC_EXPIRED, msg->param, msg->param_len);
            return;
        }

        timer = malloc(sizeof(T_REMOTE_TIMER) + msg->param_len);
        if (timer == NULL)
        {
            return; /* NOTE leak */
        }

        memcpy(timer->param, msg->param, msg->param_len);
        timer->param_len = msg->param_len;
        timer->event = event;
        timer->timer_index = chann->timer_index++;
        timer->timer_type = msg->timer_type;
        timer_id = (chann->chann_id << 24) | (timer->timer_index << 16) | event;

        if (msg->timer_type == REMOTE_TIMER_LOW_PRECISION)
        {
            timer->timer_handle = sys_timer_create("remote_lp_receive",
                                                   SYS_TIMER_TYPE_LOW_PRECISION,
                                                   timer_id,
                                                   time_offset,
                                                   false,
                                                   remote_relay_lp_timer_cback);
            if (timer->timer_handle != NULL)
            {
                os_queue_in(&chann->timer_queue, timer);
                sys_timer_start(timer->timer_handle);
            }
            else
            {
                free(timer);
            }
        }
        else if (msg->timer_type == REMOTE_TIMER_HIGH_PRECISION)
        {

            timer->timer_handle = sys_timer_create("remote_hp_receive",
                                                   SYS_TIMER_TYPE_HIGH_PRECISION,
                                                   timer_id,
                                                   time_offset,
                                                   false,
                                                   remote_relay_hp_timer_cback);
            if (timer->timer_handle != NULL)
            {
                os_queue_in(&chann->timer_queue, timer);
                sys_timer_start(timer->timer_handle);
            }
            else
            {
                free(timer);
            }
            REMOTE_PRINT_TRACE1("remote_relay_data_ind: rcv sync clk 0x%x", msg->time_info.sync_slot);
        }
        else
        {
            free(timer);
        }
    }
    else if (relay_type == REMOTE_RELAY_TYPE_SYNC_LOOPBACK)
    {
        chann->cback(event, REMOTE_RELAY_STATUS_SYNC_LOOPBACK, msg->param, msg->param_len);
        remote_sync_msg_relay(chann,
                              event,
                              msg->param,
                              msg->param_len,
                              msg->timer_type,
                              msg->time_info.timer_period,
                              false);
    }

    return;

fail_invalid_chann:
fail_duplicate_seq:
    REMOTE_PRINT_WARN1("remote_relay_data_ind: err %d", -ret);
}

void remote_relay_data_rsp(void)
{
    if (remote_db != NULL)
    {
        if (remote_db->credits < REMOTE_MSG_CREDITS)
        {
            remote_db->credits++;
        }

        remote_msg_dequeue();
    }
}

T_REMOTE_RELAY_HANDLE remote_relay_register(P_REMOTE_RELAY_CBACK cback)
{
    T_REMOTE_RELAY_CHANN *chann;

    chann = malloc(sizeof(T_REMOTE_RELAY_CHANN));
    if (chann != NULL)
    {
        chann->cback       = cback;
        chann->chann_id    = remote_db->chann_id++;
        chann->timer_index = 0;
        os_queue_init(&chann->timer_queue);
        os_queue_in(&remote_db->chann_list, chann);
    }

    return (T_REMOTE_RELAY_HANDLE)chann;
}

bool remote_relay_unregister(T_REMOTE_RELAY_HANDLE handle)
{
    T_REMOTE_RELAY_CHANN *chann;
    T_REMOTE_TIMER       *timer;

    chann = (T_REMOTE_RELAY_CHANN *)handle;
    if (remote_relay_chann_check(chann) == true)
    {
        timer = os_queue_out(&chann->timer_queue);
        while (timer != NULL)
        {
            sys_timer_delete(timer->timer_handle);
            free(timer);
            timer = os_queue_out(&chann->timer_queue);
        }

        os_queue_delete(&remote_db->chann_list, chann);
        free(chann);
        return true;
    }

    return false;
}

void remote_retrans_timer_cback(T_SYS_TIMER_HANDLE handle)
{
    REMOTE_PRINT_TRACE1("remote_retrans_timer_cback: msg count %u",
                        remote_db->relay_queue.count);

    remote_msg_dequeue();

    if (remote_db->relay_queue.count != 0)
    {
        sys_timer_start(remote_db->retrans_timer_handle);
    }
}

bool remote_session_mode_req(T_REMOTE_SESSION_MODE mode)
{
    if (remote_db != NULL)
    {
        if (mode == REMOTE_SESSION_MODE_SLEEP)
        {
            if (remote_db->relay_queue.count != 0)
            {
                return false;
            }
        }

        return true;
    }

    return false;
}

bool remote_session_mode_get(T_REMOTE_SESSION_MODE *mode)
{
    T_BT_LINK_PM_STATE  pm_state;
    int32_t             ret = 0;

    if (remote_db == NULL)
    {
        ret = 1;
        goto fail_db;
    }

    if (bt_pm_state_get(remote_db->peer_addr, &pm_state) == false)
    {
        ret = 2;
        goto fail_pm_state;
    }

    switch (pm_state)
    {
    case BT_LINK_PM_STATE_ACTIVE:
        *mode = REMOTE_SESSION_MODE_ACTIVE;
        break;

    case BT_LINK_PM_STATE_SNIFF_PENDING:
        *mode = REMOTE_SESSION_MODE_SLEEP_PENDING;
        break;

    case BT_LINK_PM_STATE_SNIFF:
        *mode = REMOTE_SESSION_MODE_SLEEP;
        break;

    case BT_LINK_PM_STATE_ACTIVE_PENDING:
        *mode = REMOTE_SESSION_MODE_ACTIVE_PENDING;
        break;
    }

    return true;

fail_pm_state:
fail_db:
    REMOTE_PRINT_ERROR1("remote_session_mode_get: failed %d", -ret);
    return false;
}

bool remote_session_mode_set(T_REMOTE_SESSION_MODE mode)
{
    int32_t ret = 0;

    if (remote_db == NULL)
    {
        ret = 1;
        goto fail_db;
    }

    if (mode == REMOTE_SESSION_MODE_SLEEP)
    {
        bt_pm_sm(remote_db->peer_addr, BT_PM_EVENT_SNIFF_ENTER_REQ);
    }
    else if (mode == REMOTE_SESSION_MODE_ACTIVE)
    {
        bt_pm_sm(remote_db->peer_addr, BT_PM_EVENT_SNIFF_EXIT_REQ);
    }
    else
    {
        ret = 2;
        goto fail_invalid_mode;
    }

    return true;

fail_invalid_mode:
fail_db:
    REMOTE_PRINT_ERROR1("remote_session_mode_set: failed %d", -ret);
    return false;
}

bool remote_bt_pm_cback(uint8_t       bd_addr[6],
                        T_BT_PM_EVENT event)
{
    bool ret = true;

    switch (event)
    {
    case BT_PM_EVENT_LINK_CONNECTED:
        break;

    case BT_PM_EVENT_LINK_DISCONNECTED:
        break;

    case BT_PM_EVENT_SNIFF_ENTER_SUCCESS:
        if (remote_db->relay_queue.count != 0)
        {
            remote_session_mode_set(REMOTE_SESSION_MODE_ACTIVE);
        }
        break;

    case BT_PM_EVENT_SNIFF_ENTER_FAIL:
        remote_msg_dequeue();
        break;

    case BT_PM_EVENT_SNIFF_ENTER_REQ:
        ret = remote_session_mode_req(REMOTE_SESSION_MODE_SLEEP);
        break;

    case BT_PM_EVENT_SNIFF_EXIT_SUCCESS:
        remote_msg_dequeue();
        break;

    case BT_PM_EVENT_SNIFF_EXIT_FAIL:
        if (remote_db->relay_queue.count != 0)
        {
            remote_session_mode_set(REMOTE_SESSION_MODE_ACTIVE);
        }
        break;

    case BT_PM_EVENT_SNIFF_EXIT_REQ:
        ret = remote_session_mode_req(REMOTE_SESSION_MODE_ACTIVE);
        break;
    }

    return ret;
}

bool remote_bt_ipc_cback(uint32_t  id,
                         void     *msg)
{
    switch (id)
    {
    case BT_IPC_REMOTE_CONNECTED:
        {
            remote_db->session_state = REMOTE_SESSION_STATE_CONNECTED;
            remote_db->credits = REMOTE_MSG_CREDITS;

            bt_pm_cback_register(remote_db->peer_addr, remote_bt_pm_cback);
        }
        break;

    case BT_IPC_REMOTE_DISCONNECTED:
        {
            remote_db->session_state = REMOTE_SESSION_STATE_DISCONNECTED;
            remote_msg_queue_clear();
            remote_db->tx_seq       = 0;
            remote_db->rx_seq       = 0;

            bt_pm_cback_unregister(remote_db->peer_addr, remote_bt_pm_cback);
        }
        break;

    case BT_IPC_REMOTE_DATA_IND:
        {
            T_BT_IPC_MSG *p_msg = (T_BT_IPC_MSG *)msg;

            remote_relay_data_ind(p_msg->bt_remote_data_ind.buf, p_msg->bt_remote_data_ind.len);
        }
        break;

    case BT_IPC_REMOTE_DATA_RSP:
        {
            remote_relay_data_rsp();
        }
        break;

    case BT_IPC_REMOTE_SESSION_ACTIVE:
        break;

    case BT_IPC_REMOTE_SESSION_SLEEP:
        break;

    default:
        break;
    }

    return true;
}

bool remote_mgr_init(T_REMOTE_SESSION_ROLE role)
{
    int32_t ret = 0;

    remote_db = calloc(1, sizeof(T_REMOTE_DB));
    if (remote_db == NULL)
    {
        ret = 1;
        goto fail_alloc_db;
    }

    remote_db->bt_ipc_handle = sys_ipc_subscribe(BT_IPC_TOPIC, remote_bt_ipc_cback);
    if (remote_db->bt_ipc_handle == NULL)
    {
        ret = 2;
        goto fail_subscribe_ipc_cback;
    }

    remote_db->retrans_timer_handle = sys_timer_create("remote_retrans",
                                                       SYS_TIMER_TYPE_LOW_PRECISION,
                                                       0,
                                                       REMOTE_RELAY_RETRANS_INTERVAL,
                                                       false,
                                                       remote_retrans_timer_cback);
    if (remote_db->retrans_timer_handle == NULL)
    {
        ret = 3;
        goto fail_create_timer;
    }

    remote_db->session_role = role;
    remote_db->tx_seq       = 0;
    remote_db->rx_seq       = 0;
    remote_db->chann_id     = 0;
    remote_db->period       = REMOTE_RELAY_PERIOD_DEFAULT;
    os_queue_init(&remote_db->relay_queue);
    os_queue_init(&remote_db->chann_list);

    return true;

fail_create_timer:
    sys_ipc_unsubscribe(remote_db->bt_ipc_handle);
fail_subscribe_ipc_cback:
    free(remote_db);
    remote_db = NULL;
fail_alloc_db:
    REMOTE_PRINT_ERROR1("remote_mgr_init: failed %d", -ret);
    return false;
}

void remote_mgr_deinit(void)
{
    if (remote_db != NULL)
    {
        T_REMOTE_RELAY_ITEM  *relay_item;
        T_REMOTE_RELAY_CHANN *chann;
        T_REMOTE_TIMER       *timer;

        relay_item = os_queue_out(&remote_db->relay_queue);
        while (relay_item != NULL)
        {
            free(relay_item);
            relay_item = os_queue_out(&remote_db->relay_queue);
        }

        chann = os_queue_out(&remote_db->chann_list);
        while (chann != NULL)
        {
            timer = os_queue_out(&chann->timer_queue);
            while (timer != NULL)
            {
                sys_timer_delete(timer->timer_handle);
                free(timer);
                timer = os_queue_out(&chann->timer_queue);
            }
            free(chann);
            chann = os_queue_out(&remote_db->chann_list);
        }

        if (remote_db->retrans_timer_handle)
        {
            sys_timer_delete(remote_db->retrans_timer_handle);
        }

        sys_ipc_unsubscribe(remote_db->bt_ipc_handle);
        free(remote_db);
        remote_db = NULL;
    }
}

T_REMOTE_SESSION_STATE remote_session_state_get(void)
{
    if (remote_db != NULL)
    {
        return remote_db->session_state;
    }

    return REMOTE_SESSION_STATE_DISCONNECTED;
}

bool remote_roleswap_start(uint8_t                      bd_addr[6],
                           uint8_t                      context,
                           bool                         stop_after_shadow,
                           P_REMOTE_ROLESWAP_SYNC_CBACK cback)
{
    return bt_roleswap_start(bd_addr, context, stop_after_shadow, cback);
}

bool remote_roleswap_cfm(bool    accept,
                         uint8_t context)
{
    return bt_roleswap_cfm(accept, context);
}

bool remote_roleswap_stop(uint8_t bd_addr[6])
{
    return bt_roleswap_stop(bd_addr);
}
