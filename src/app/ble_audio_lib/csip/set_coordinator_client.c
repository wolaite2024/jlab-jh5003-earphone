#include <string.h>
#include "trace.h"
#include "ble_audio_mgr.h"
#include "gap.h"
#include "ble_link_util.h"
#include "ble_audio.h"
#include "csis_client_int.h"
#include "csis_client.h"
#include "ble_audio_dm.h"
#include "csis_client_int.h"
#include "csis_sir.h"
#include "cap_int.h"
#include "sys_timer.h"
#include "ble_audio_group_int.h"
#include "os_queue.h"

#if LE_AUDIO_CSIS_CLIENT_SUPPORT
typedef struct t_set_mem_discov
{
    struct t_set_mem_discov *p_next;
    uint8_t addr_type;
    uint8_t bd_addr[6];
} T_SET_MEM_DISCOV;

typedef struct
{
    T_LE_AUDIO_GROUP     *set_coor;
    T_SYS_TIMER_HANDLE    timer_handle;
    uint32_t              timeout_ms;
    T_OS_QUEUE            discov_queue;
} T_SET_COOR_DISCOV_CB;

T_SET_COOR_DISCOV_CB set_coor_discov_cb;

static T_APP_RESULT set_coordinator_write_lock_result(uint16_t conn_handle,
                                                      T_GATT_CLIENT_EVENT type,
                                                      void *p_data);
static T_APP_RESULT set_coordinator_write_unlock_result(uint16_t conn_handle,
                                                        T_GATT_CLIENT_EVENT type,
                                                        void *p_data);
static bool set_coordinator_stop_discovery_timer(void);

T_CSIP_CB *set_coordinator_get_cb_int(const char *p_func_name,
                                      T_LE_AUDIO_GROUP *p_coor_set)
{
    if (p_coor_set != NULL)
    {
        if (os_queue_search(&audio_group_queue, p_coor_set) == true)
        {
            if (p_coor_set->p_csip_cb)
            {
                return p_coor_set->p_csip_cb;
            }
        }
    }
    PROTOCOL_PRINT_ERROR2("set_coordinator_get_cb:failed, %s, group_handle %p",
                          TRACE_STRING(p_func_name), p_coor_set);
    return NULL;
}

bool set_members_quick_sort_by_rank(T_LE_AUDIO_GROUP *p_audio_group)
{
    if (p_audio_group != NULL && !p_audio_group->p_csip_cb->rank_sorted)
    {
        T_OS_QUEUE temp_dev_queue;
        T_LE_AUDIO_DEV *p_dev;
        os_queue_init(&temp_dev_queue);
        while ((p_dev = (T_LE_AUDIO_DEV *)os_queue_out(&p_audio_group->dev_queue)) != NULL)
        {
            if (p_dev->is_used == false)
            {
                os_queue_insert(&temp_dev_queue, NULL, p_dev);
            }
            else
            {
                T_LE_AUDIO_DEV *p_prev_dev = NULL;
                T_LE_AUDIO_DEV *p_aft_dev = NULL;
                for (uint8_t i = 0; i < temp_dev_queue.count; i++)
                {
                    p_aft_dev = (T_LE_AUDIO_DEV *)os_queue_peek(&temp_dev_queue, i);
                    if (p_aft_dev != NULL && p_aft_dev->is_used == true && p_aft_dev->rank > p_dev->rank)
                    {
                        break;
                    }
                    p_prev_dev = p_aft_dev;
                }
                os_queue_insert(&temp_dev_queue, p_prev_dev, p_dev);
            }
        }
        memcpy(&p_audio_group->dev_queue, &temp_dev_queue, sizeof(T_OS_QUEUE));
        p_audio_group->p_csip_cb->rank_sorted = true;
        return true;
    }
    return false;
}

static T_SET_MEM_DISCOV *set_coordinator_find_or_alloc_set_mem(uint8_t *p_bd_addr,
                                                               uint8_t addr_type,
                                                               bool *p_is_new)
{
    T_SET_MEM_DISCOV *p_set_mem;
    *p_is_new = false;
    for (uint8_t i = 0; i < set_coor_discov_cb.discov_queue.count; i++)
    {
        p_set_mem = (T_SET_MEM_DISCOV *)os_queue_peek(&set_coor_discov_cb.discov_queue, i);
        if (p_set_mem->addr_type == addr_type &&
            memcmp(p_set_mem->bd_addr, p_bd_addr, 6) == 0)
        {
            return p_set_mem;
        }
    }
    p_set_mem = ble_audio_mem_zalloc(sizeof(T_SET_MEM_DISCOV));
    if (p_set_mem)
    {
        p_set_mem->addr_type = addr_type;
        memcpy(p_set_mem->bd_addr, p_bd_addr, 6);
        *p_is_new = true;
        os_queue_in(&set_coor_discov_cb.discov_queue, p_set_mem);
        return p_set_mem;
    }
    return NULL;
}

static void set_coordinator_free_set_mem_queue(void)
{
    T_SET_MEM_DISCOV *p_set_mem;
    while ((p_set_mem = (T_SET_MEM_DISCOV *)os_queue_out(&set_coor_discov_cb.discov_queue)) != NULL)
    {
        ble_audio_mem_free(p_set_mem);
    }
}

static void set_coordinator_discovery_search_done(bool is_timeout)
{
    if (set_coor_discov_cb.set_coor->p_csip_cb == NULL)
    {
        return;
    }
    if (is_timeout)
    {
        T_CSIS_CLIENT_SEARCH_TIMEOUT search_result;

        search_result.search_done = false;
        search_result.set_mem_size = set_coor_discov_cb.set_coor->p_csip_cb->size;
        search_result.group_handle = set_coor_discov_cb.set_coor;
        set_coordinator_free_set_mem_queue();
        set_coor_discov_cb.set_coor = NULL;
        ble_audio_mgr_dispatch(LE_AUDIO_MSG_CSIS_CLIENT_SEARCH_TIMEOUT, &search_result);
    }
    else
    {
        T_CSIS_CLIENT_SEARCH_DONE search_result;

        search_result.search_done = true;
        search_result.set_mem_size = set_coor_discov_cb.set_coor->p_csip_cb->size;
        search_result.group_handle = set_coor_discov_cb.set_coor;
        set_coordinator_stop_discovery_timer();
        set_coordinator_free_set_mem_queue();
        set_coor_discov_cb.set_coor = NULL;
        ble_audio_mgr_dispatch(LE_AUDIO_MSG_CSIS_CLIENT_SEARCH_DONE, &search_result);
    }
}

static void set_coordinator_discovery_timeout_cb(T_SYS_TIMER_HANDLE handle)
{
    PROTOCOL_PRINT_TRACE0("coordinator_discovery_timeout_cb");
    if (set_coor_discov_cb.set_coor == NULL)
    {
        return;
    }
    set_coordinator_discovery_search_done(true);
}

static bool set_coordinator_start_discovery_timer()
{
    if (set_coor_discov_cb.set_coor == NULL)
    {
        return false;
    }
    if (set_coor_discov_cb.timeout_ms != 0)
    {
        PROTOCOL_PRINT_TRACE0("coordinator_discovery_timer start");
        if (set_coor_discov_cb.timer_handle == NULL && set_coor_discov_cb.timeout_ms != 0)
        {
            set_coor_discov_cb.timer_handle = sys_timer_create("Coordinator discover",
                                                               SYS_TIMER_TYPE_LOW_PRECISION,
                                                               0,
                                                               set_coor_discov_cb.timeout_ms * 1000,
                                                               false,
                                                               set_coordinator_discovery_timeout_cb);
            if (set_coor_discov_cb.timer_handle)
            {
                return sys_timer_start(set_coor_discov_cb.timer_handle);
            }
        }
        else
        {
            return sys_timer_restart(set_coor_discov_cb.timer_handle, set_coor_discov_cb.timeout_ms * 1000);
        }
    }
    return false;
}

static bool set_coordinator_stop_discovery_timer(void)
{
    PROTOCOL_PRINT_TRACE0("set_coordinator_stop_discovery_timer");
    if (set_coor_discov_cb.timer_handle)
    {
        return sys_timer_stop(set_coor_discov_cb.timer_handle);
    }
    return false;
}

bool set_coordinator_cfg_discover(T_BLE_AUDIO_GROUP_HANDLE group_handle, bool discover,
                                  uint32_t timeout_ms)
{
    if (discover)
    {
        T_LE_AUDIO_GROUP *p_coor_set;
        if (group_handle == NULL)
        {
            goto failed;
        }
        if (ble_audio_group_handle_check(group_handle) == false)
        {
            goto failed;
        }
        p_coor_set = (T_LE_AUDIO_GROUP *)group_handle;
        if (p_coor_set->p_csip_cb == NULL)
        {
            goto failed;
        }
        if (set_coor_discov_cb.set_coor == NULL)
        {
            set_coor_discov_cb.set_coor = p_coor_set;
            set_coor_discov_cb.timeout_ms = timeout_ms;
            for (uint8_t i = 0; i < p_coor_set->dev_queue.count; i++)
            {
                T_LE_AUDIO_DEV *p_dev = (T_LE_AUDIO_DEV *)os_queue_peek(&p_coor_set->dev_queue, i);
                if (p_dev != NULL && p_dev->is_used == true)
                {
                    if (p_dev->p_link != NULL && p_dev->p_link->state == GAP_CONN_STATE_CONNECTED)
                    {
                        bool is_new;
                        set_coordinator_find_or_alloc_set_mem(p_dev->p_link->remote_bd,
                                                              p_dev->p_link->remote_bd_type, &is_new);
                    }
                }
            }
            set_coordinator_start_discovery_timer();
        }
        else if (set_coor_discov_cb.set_coor != p_coor_set)
        {
            goto failed;
        }
    }
    else
    {
        if (set_coor_discov_cb.set_coor != NULL)
        {
            set_coordinator_free_set_mem_queue();
            set_coordinator_stop_discovery_timer();
            set_coor_discov_cb.set_coor = NULL;
        }
    }
    PROTOCOL_PRINT_INFO3("[CSIS] set_coordinator_cfg_discover: discover %d, group_handle %p, timeout_ms %d",
                         discover, group_handle, timeout_ms);
    return true;
failed:
    PROTOCOL_PRINT_ERROR3("[CSIS] set_coordinator_cfg_discover: failed, discover %d, group_handle %p, timeout_ms %d",
                          discover, group_handle, timeout_ms);
    return false;
}

bool set_coordinator_check_adv_rsi(uint8_t report_data_len, uint8_t *p_report_data,
                                   uint8_t *p_bd_addr, uint8_t addr_type)
{
    uint8_t buffer[CSI_RSI_LEN];
    uint16_t pos = 0;
    T_CSIP_CB *p_csip_cb;
    if (set_coor_discov_cb.set_coor == NULL)
    {
        return false;
    }
    p_csip_cb = set_coor_discov_cb.set_coor->p_csip_cb;
    if (p_csip_cb == NULL)
    {
        return false;
    }

    while (pos < report_data_len)
    {
        /* Length of the AD structure. */
        uint16_t length = p_report_data[pos++];

        if (length == 0)
        {
            break;
        }

        if (length != (CSI_RSI_LEN + 1))
        {
            pos += length;
            continue;
        }

        if (p_report_data[pos] == GAP_ADTYPE_RSI)
        {
            /* Copy the AD Data to buffer. */
            memcpy(buffer, p_report_data + pos + 1, CSI_RSI_LEN);

            if (csis_check_rsi(p_csip_cb->sirk, buffer))
            {
                bool is_new = false;
                if (set_coordinator_find_or_alloc_set_mem(p_bd_addr, addr_type, &is_new) != NULL)
                {
                    PROTOCOL_PRINT_INFO2("[CSIS] set_coordinator_check_adv_rsi: p_bd_addr %s, is_new %d",
                                         TRACE_BDADDR(p_bd_addr), is_new);
                    if (is_new)
                    {
                        T_CSIS_CLIENT_SET_MEM_FOUND set_report;
                        memcpy(set_report.bd_addr, p_bd_addr, 6);
                        set_report.addr_type = addr_type;
                        memcpy(set_report.sirk, p_csip_cb->sirk, CSIS_SIRK_LEN);
                        set_report.srv_uuid = p_csip_cb->serv_uuid;
                        set_report.rank = CSIS_RANK_UNKNOWN;
                        set_report.set_mem_size = p_csip_cb->size;
                        set_report.group_handle = set_coor_discov_cb.set_coor;
                        set_report.dev_handle = ble_audio_group_find_dev(set_coor_discov_cb.set_coor,
                                                                         p_bd_addr, addr_type);
                        set_coordinator_start_discovery_timer();
                        ble_audio_mgr_dispatch(LE_AUDIO_MSG_CSIS_CLIENT_SET_MEM_FOUND, &set_report);
                        if (p_csip_cb->size == set_coor_discov_cb.discov_queue.count)
                        {
                            set_coordinator_discovery_search_done(false);
                            return true;
                        }
                    }
                }
                return true;
            }
        }

        pos += length;
    }
    return false;
}

bool set_coordinator_get_group_info(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                                    T_CSIS_GROUP_INFO *p_info)
{
    T_LE_AUDIO_GROUP *p_coor_set = (T_LE_AUDIO_GROUP *)group_handle;
    T_CSIP_CB *p_csip_cb = set_coordinator_get_cb(p_coor_set);
    if (p_csip_cb == NULL)
    {
        return false;
    }
    if (p_info != NULL)
    {
        p_info->srv_uuid = p_csip_cb->serv_uuid;
        p_info->char_exit = p_csip_cb->char_exit;
        p_info->set_mem_size = p_csip_cb->size;
        p_info->sirk_type = p_csip_cb->sirk_type;
        memcpy(p_info->sirk, p_csip_cb->sirk, CSIS_SIRK_LEN);

        return true;
    }
    return false;
}

void set_coordinator_check_lock_state(T_LE_AUDIO_GROUP *p_coor_set, T_SET_COORDINATOR_EVENT event)
{
    T_SET_COORDINATOR_LOCK lock_state = p_coor_set->p_csip_cb->lock_state;
    uint8_t conn_dev_num = 0;
    uint8_t lock_granted_num = 0;
    uint8_t lock_non_granted_num = 0;
    uint8_t unlock_num = 0;
    uint8_t lock_unknown_num = 0;
    PROTOCOL_PRINT_INFO3("[CSIS] set_coordinator_check_lock_state: ++ group_handle %p, lock state %d, event %d",
                         p_coor_set, p_coor_set->p_csip_cb->lock_state, event);
    if (event == SET_COORDINATOR_EVENT_INIT)
    {
        if ((p_coor_set->p_csip_cb->char_exit & CSIS_LOCK_FLAG) == 0)
        {
            lock_state = SET_COORDINATOR_LOCK_NOT_SUPPORT;
            goto check_state;
        }
    }

    for (uint8_t i = 0; i < p_coor_set->dev_queue.count; i++)
    {
        T_LE_AUDIO_DEV *p_set_member = (T_LE_AUDIO_DEV *)os_queue_peek(&p_coor_set->dev_queue, i);
        if (p_set_member == NULL || p_set_member->is_used == false)
        {
            continue;
        }
        if (p_set_member->p_link && p_set_member->p_link->state == GAP_CONN_STATE_CONNECTED)
        {
            conn_dev_num++;
            if (p_set_member->lock == CSIS_UNLOCKED)
            {
                unlock_num++;
            }
            else if (p_set_member->lock == CSIS_LOCKED)
            {
                if (p_set_member->lock_obtained)
                {
                    lock_granted_num++;
                }
                else
                {
                    lock_non_granted_num++;
                }
            }
            else
            {
                lock_unknown_num++;
            }
        }
    }
    PROTOCOL_PRINT_INFO5("[CSIS] set_coordinator_check_lock_state: conn_dev_num %d, lock_granted_num %d, lock_non_granted_num %d, unlock_num %d, lock_unknown_num %d",
                         conn_dev_num, lock_granted_num,
                         lock_non_granted_num, unlock_num, lock_unknown_num);
    if (conn_dev_num == 0)
    {
        lock_state = SET_COORDINATOR_LOCK_NO_CONN;
    }
    else
    {
        if (conn_dev_num == unlock_num)
        {
            lock_state = SET_COORDINATOR_UNLOCK;
        }
        else if (conn_dev_num == lock_granted_num)
        {
            lock_state = SET_COORDINATOR_LOCK_GRANTED;
        }
        switch (event)
        {
        case SET_COORDINATOR_EVENT_LOCK_REQ:
            {
                if (lock_state != SET_COORDINATOR_LOCK_GRANTED)
                {
                    lock_state = SET_COORDINATOR_WAIT_FOR_LOCK;
                }
            }
            break;
        case SET_COORDINATOR_EVENT_UNLOCK_REQ:
            {
                if (lock_state != SET_COORDINATOR_UNLOCK)
                {
                    if (p_coor_set->p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_LOCK ||
                        p_coor_set->p_csip_cb->lock_state == SET_COORDINATOR_LOCK_GRANTED)
                    {
                        lock_state = SET_COORDINATOR_WAIT_FOR_UNLOCK;
                    }
                }
            }
            break;
        case SET_COORDINATOR_EVENT_MEM_ADD:
            {
                if (p_coor_set->p_csip_cb->lock_state == SET_COORDINATOR_LOCK_GRANTED)
                {
                    if (lock_state != SET_COORDINATOR_LOCK_GRANTED)
                    {
                        lock_state = SET_COORDINATOR_WAIT_FOR_LOCK;
                    }
                }
                else if (p_coor_set->p_csip_cb->lock_state == SET_COORDINATOR_UNLOCK)
                {
                    if (lock_non_granted_num != 0)
                    {
                        lock_state = SET_COORDINATOR_LOCK_UNKNOWN;
                    }
                }
            }
            break;

        case SET_COORDINATOR_EVENT_LOCK_NOTIFY:
            {
                if (p_coor_set->p_csip_cb->lock_state == SET_COORDINATOR_LOCK_GRANTED)
                {
                    if (lock_state != SET_COORDINATOR_LOCK_GRANTED &&
                        lock_state != SET_COORDINATOR_UNLOCK)
                    {
                        lock_state = SET_COORDINATOR_WAIT_FOR_UNLOCK;
                    }
                }
            }
            break;

        case SET_COORDINATOR_EVENT_LOCK_REQ_FAILED:
            {
                if (lock_granted_num)
                {
                    if (p_coor_set->p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_LOCK)
                    {
                        lock_state = SET_COORDINATOR_WAIT_FOR_UNLOCK;
                    }
                }
            }
            break;
        case SET_COORDINATOR_EVENT_UNLOCK_REQ_FAILED:
            {
                lock_state = SET_COORDINATOR_LOCK_UNKNOWN;
            }
            break;
        case SET_COORDINATOR_EVENT_LOCK_DENIED:
            {
                lock_state = SET_COORDINATOR_LOCK_DENIED;
            }
            break;
        default:
            break;
        }
    }
check_state:
    if (lock_state != p_coor_set->p_csip_cb->lock_state)
    {
        PROTOCOL_PRINT_INFO3("[CSIS] set_coordinator_check_lock_state: -- group_handle %p, lock state %d -> %d",
                             p_coor_set, p_coor_set->p_csip_cb->lock_state, lock_state);
        p_coor_set->p_csip_cb->lock_state = lock_state;
        if (event != SET_COORDINATOR_EVENT_INIT)
        {
            T_CSIS_CLIENT_LOCK_STATE lock_state;
            lock_state.group_handle = p_coor_set;
            lock_state.event = event;
            lock_state.lock_state = p_coor_set->p_csip_cb->lock_state;
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_CSIS_CLIENT_LOCK_STATE, &lock_state);
        }
    }
    return;
}

bool set_coordinator_unlock_req(T_LE_AUDIO_GROUP *p_coor_set)
{
    T_LE_AUDIO_DEV *p_set_member;
    int i;
    if (p_coor_set == NULL || p_coor_set->p_csip_cb == NULL)
    {
        return false;
    }
    if (p_coor_set->p_csip_cb->lock_req)
    {
        PROTOCOL_PRINT_INFO0("[CSIS] set_coordinator_unlock_req: lock_req is true");
        return true;
    }

    for (i = p_coor_set->dev_queue.count; i > 0; i--)
    {
        p_set_member = (T_LE_AUDIO_DEV *)os_queue_peek(&p_coor_set->dev_queue, i - 1);
        if (p_set_member == NULL || p_set_member->is_used == false)
        {
            continue;
        }
        if (p_set_member->lock_obtained && p_set_member->p_link)
        {
            p_set_member->lock_obtained = false;
            if (csis_send_lock_req(p_set_member->p_link->conn_handle, p_set_member->srv_instance_id,
                                   CSIS_UNLOCKED,
                                   set_coordinator_write_unlock_result))
            {
                PROTOCOL_PRINT_INFO1("[CSIS] set_coordinator_unlock_req: unlock request rank %d",
                                     p_set_member->rank);
                p_coor_set->p_csip_cb->lock_req = true;
                set_coordinator_check_lock_state(p_coor_set, SET_COORDINATOR_EVENT_UNLOCK_REQ);
                return true;
            }
            else
            {
                set_coordinator_check_lock_state(p_coor_set, SET_COORDINATOR_EVENT_UNLOCK_REQ_FAILED);
                PROTOCOL_PRINT_INFO1("[CSIS] set_coordinator_unlock_req: lock release fail, just skip it rank %d",
                                     p_set_member->rank);
                return false;
            }
        }
    }
    set_coordinator_check_lock_state(p_coor_set, SET_COORDINATOR_EVENT_UNLOCK_REQ);
    return true;
}

//Every time check whole set to make sure lock is obtained by set
bool set_coordinator_lock_req(T_LE_AUDIO_GROUP *p_coor_set)
{
    T_LE_AUDIO_DEV *p_set_member;
    int i;
    if (p_coor_set == NULL || p_coor_set->p_csip_cb == NULL)
    {
        return false;
    }
    if (p_coor_set->p_csip_cb->lock_req)
    {
        PROTOCOL_PRINT_INFO0("[CSIS] set_coordinator_lock_req: lock_req is true");
        return true;
    }

    for (i = 0; i < p_coor_set->dev_queue.count; i++)
    {
        p_set_member = (T_LE_AUDIO_DEV *)os_queue_peek(&p_coor_set->dev_queue, i);
        if (p_set_member == NULL || p_set_member->is_used == false)
        {
            continue;
        }

        if (p_set_member->lock_obtained)
        {
            continue;
        }
        else
        {
            if ((p_set_member->char_exit & CSIS_LOCK_FLAG) == 0)
            {
                PROTOCOL_PRINT_ERROR2("[CSIS] set_coordinator_lock_req member(%d) doesn't support lock character, char_exit 0x%x, just skip it",
                                      i, p_set_member->char_exit);
                continue;
            }
            else if (p_set_member->rank == 0)
            {
                PROTOCOL_PRINT_ERROR2("[CSIS] set_coordinator_lock_req member(%d) rank is 0, char_exit 0x%x, just skip it",
                                      i, p_set_member->char_exit);
                continue;
            }
            else if (p_set_member->p_link && p_set_member->p_link->state == GAP_CONN_STATE_CONNECTED)
            {
                if (csis_send_lock_req(p_set_member->p_link->conn_handle, p_set_member->srv_instance_id,
                                       CSIS_LOCKED,
                                       set_coordinator_write_lock_result))
                {
                    PROTOCOL_PRINT_INFO1("[CSIS] set_coordinator_lock_req: lock request rank %d", p_set_member->rank);
                    p_coor_set->p_csip_cb->lock_req = true;
                    set_coordinator_check_lock_state(p_coor_set, SET_COORDINATOR_EVENT_LOCK_REQ);
                    return true;
                }
                else
                {
                    PROTOCOL_PRINT_INFO1("[CSIS] set_coordinator_lock_req: lock request fail, rank %d",
                                         p_set_member->rank);
                    set_coordinator_check_lock_state(p_coor_set, SET_COORDINATOR_EVENT_LOCK_REQ_FAILED);
                    if (p_coor_set->p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_UNLOCK)
                    {
                        set_coordinator_unlock_req(p_coor_set);
                    }
                    return false;
                }
            }
        }
    }
    set_coordinator_check_lock_state(p_coor_set, SET_COORDINATOR_EVENT_LOCK_REQ);
    return true;
}

bool set_coordinator_write_lock(T_BLE_AUDIO_GROUP_HANDLE group_handle)
{
    T_LE_AUDIO_GROUP *p_coor_set = (T_LE_AUDIO_GROUP *)group_handle;
    T_CSIP_CB *p_csip_cb = set_coordinator_get_cb(p_coor_set);
    if (p_csip_cb == NULL)
    {
        return false;
    }
    if (p_csip_cb->lock_state == SET_COORDINATOR_UNLOCK ||
        p_csip_cb->lock_state == SET_COORDINATOR_LOCK_UNKNOWN)
    {
        if (set_coordinator_lock_req(p_coor_set))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_LOCK ||
             p_csip_cb->lock_state == SET_COORDINATOR_LOCK_GRANTED)
    {
        return true;
    }
    else
    {
        PROTOCOL_PRINT_INFO2("[CSIS] set_coordinator_write_lock: group_handle %p, invalid lock_state %d",
                             group_handle, p_csip_cb->lock_state);
        return false;
    }
}

bool set_coordinator_write_unlock(T_BLE_AUDIO_GROUP_HANDLE group_handle)
{
    T_LE_AUDIO_GROUP *p_coor_set = (T_LE_AUDIO_GROUP *)group_handle;
    T_CSIP_CB *p_csip_cb = set_coordinator_get_cb(p_coor_set);
    if (p_csip_cb == NULL)
    {
        return false;
    }
    if (p_csip_cb->lock_state == SET_COORDINATOR_LOCK_GRANTED ||
        p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_LOCK)
    {
        if (set_coordinator_unlock_req(p_coor_set))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_UNLOCK ||
             p_csip_cb->lock_state == SET_COORDINATOR_UNLOCK ||
             p_csip_cb->lock_state == SET_COORDINATOR_LOCK_NO_CONN)
    {
        return true;
    }
    else
    {
        PROTOCOL_PRINT_INFO2("[CSIS] set_coordinator_write_unlock: group_handle %p, invalid lock_state %d",
                             group_handle, p_csip_cb->lock_state);
        return false;
    }
}

bool set_coordinator_get_lock_state(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                                    T_SET_COORDINATOR_LOCK *p_lock)
{
    T_LE_AUDIO_GROUP *p_coor_set = (T_LE_AUDIO_GROUP *)group_handle;
    if (set_coordinator_get_cb(p_coor_set) == false)
    {
        return false;
    }
    if (p_lock == NULL)
    {
        return false;
    }
    *p_lock = p_coor_set->p_csip_cb->lock_state;
    return true;
}

bool set_coordinator_find_mem(uint16_t conn_handle, uint8_t srv_instance_id,
                              T_LE_AUDIO_DEV **pp_set_member, T_LE_AUDIO_GROUP **pp_coor_set)
{
    T_LE_AUDIO_DEV *p_set_member = NULL;
    T_LE_AUDIO_GROUP *p_coor_set = NULL;
    for (uint8_t i = 0; i < audio_group_queue.count; i++)
    {
        T_LE_AUDIO_GROUP *temp;
        T_LE_AUDIO_DEV *p_temp_member = NULL;
        temp = (T_LE_AUDIO_GROUP *)os_queue_peek(&audio_group_queue, i);
        if (temp && temp->p_csip_cb)
        {
            for (uint8_t j = 0; j < temp->dev_queue.count; j++)
            {
                p_temp_member = (T_LE_AUDIO_DEV *)os_queue_peek(&temp->dev_queue, j);
                if (p_temp_member && p_temp_member->p_link &&
                    p_temp_member->p_link->state == GAP_CONN_STATE_CONNECTED &&
                    p_temp_member->p_link->conn_handle == conn_handle &&
                    p_temp_member->srv_instance_id == srv_instance_id)
                {
                    p_coor_set = temp;
                    p_set_member = p_temp_member;
                    break;
                }
            }
        }
    }
    if (p_coor_set == NULL || p_set_member == NULL)
    {
        return false;
    }
    else
    {
        *pp_set_member = p_set_member;
        *pp_coor_set = p_coor_set;
        return true;
    }
}

static T_APP_RESULT set_coordinator_write_lock_result(uint16_t conn_handle,
                                                      T_GATT_CLIENT_EVENT type,
                                                      void *p_data)
{
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;
    T_LE_AUDIO_DEV *p_set_member = NULL;
    T_LE_AUDIO_GROUP *p_coor_set = NULL;
    if (set_coordinator_find_mem(conn_handle, p_client_cb_data->write_result.srv_instance_id,
                                 &p_set_member, &p_coor_set) == false)
    {
        PROTOCOL_PRINT_ERROR2("[CSIS] set_coordinator_write_lock_result: not find mem, conn_handle 0x%x, srv_instance_id %d",
                              conn_handle,
                              p_client_cb_data->write_result.srv_instance_id);
        return APP_RESULT_SUCCESS;
    }
    p_coor_set->p_csip_cb->lock_req = false;
    if (p_client_cb_data->write_result.cause == GAP_SUCCESS ||
        p_client_cb_data->write_result.cause == ATT_ERR_CSIS_LOCK_ALREADY_GRANTED)
    {
        PROTOCOL_PRINT_INFO1("[CSIS] set_coordinator_write_lock_result: mem %p",
                             p_set_member);
        p_set_member->lock = CSIS_LOCKED;
        p_set_member->lock_obtained = true;
        if (p_coor_set->p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_LOCK)
        {
            set_coordinator_lock_req(p_coor_set);
        }
        else if (p_coor_set->p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_UNLOCK)
        {
            set_coordinator_unlock_req(p_coor_set);
        }
    }
    else
    {
        PROTOCOL_PRINT_ERROR2("[CSIS] set_coordinator_write_lock_result: lock request failed, conn_handle 0x%x, cause 0x%x",
                              conn_handle,
                              p_client_cb_data->write_result.cause);
        if (p_client_cb_data->write_result.cause == ATT_ERR_CSIS_LOCK_DENIED)
        {
            set_coordinator_check_lock_state(p_coor_set, SET_COORDINATOR_EVENT_LOCK_DENIED);
        }
        else
        {
            set_coordinator_check_lock_state(p_coor_set, SET_COORDINATOR_EVENT_LOCK_REQ_FAILED);
        }
        set_coordinator_unlock_req(p_coor_set);
    }

    return APP_RESULT_SUCCESS;
}

static T_APP_RESULT set_coordinator_write_unlock_result(uint16_t conn_handle,
                                                        T_GATT_CLIENT_EVENT type,
                                                        void *p_data)
{
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;
    T_LE_AUDIO_DEV *p_set_member = NULL;
    T_LE_AUDIO_GROUP *p_coor_set = NULL;
    if (set_coordinator_find_mem(conn_handle, p_client_cb_data->write_result.srv_instance_id,
                                 &p_set_member, &p_coor_set) == false)
    {
        PROTOCOL_PRINT_ERROR2("[CSIS] set_coordinator_write_unlock_result: not find mem, conn_handle 0x%x, srv_instance_id %d",
                              conn_handle,
                              p_client_cb_data->write_result.srv_instance_id);
        return APP_RESULT_SUCCESS;
    }
    p_coor_set->p_csip_cb->lock_req = false;
    p_set_member->lock_obtained = false;
    PROTOCOL_PRINT_INFO2("[CSIS] set_coordinator_write_unlock_result: cause 0x%x, lock_state %d",
                         p_client_cb_data->write_result.cause,
                         p_coor_set->p_csip_cb->lock_state);
    if (p_client_cb_data->write_result.cause == GAP_SUCCESS)
    {
        p_set_member->lock = CSIS_UNLOCKED;
        if (p_coor_set->p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_LOCK)
        {
            set_coordinator_lock_req(p_coor_set);
        }
        else if (p_coor_set->p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_UNLOCK ||
                 p_coor_set->p_csip_cb->lock_state == SET_COORDINATOR_LOCK_DENIED)
        {
            set_coordinator_unlock_req(p_coor_set);
        }
    }
    else
    {
        PROTOCOL_PRINT_ERROR2("[CSIS] set_coordinator_write_unlock_result: lock release failed, conn_handle 0x%x, cause 0x%x",
                              conn_handle,
                              p_client_cb_data->write_result.cause);
        set_coordinator_check_lock_state(p_coor_set, SET_COORDINATOR_EVENT_UNLOCK_REQ_FAILED);
    }
    return APP_RESULT_SUCCESS;
}

static T_APP_RESULT set_coordinator_discover_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                                void *p_data)
{
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    T_CSIS_CLIENT_DB *p_csis_db = NULL;
    T_LE_AUDIO_GROUP *p_coor_set = NULL;
    T_CSIS_CLIENT_READ_RESULT read_result = {0};
    if (p_link)
    {
        p_csis_db = (T_CSIS_CLIENT_DB *)p_link->p_csis_client;
    }
    if (p_csis_db == NULL)
    {
        return APP_RESULT_APP_ERR;
    }

    if (p_client_cb_data->read_result.cause == GAP_SUCCESS)
    {
        if (p_client_cb_data->read_result.char_uuid.p.uuid16 == CSIS_UUID_CHAR_SIRK)
        {

            uint8_t sirk[16];
            if (p_client_cb_data->read_result.value_size != 17)
            {
                goto read_failed;
            }
            if (p_client_cb_data->read_result.p_value[0] == CSIS_SIRK_ENC)
            {
                uint8_t K[16];
                if (csis_client_generate_le_k(p_link->remote_bd, p_link->remote_bd_type, p_link->conn_handle, K))
                {
                    csis_crypto_sdf(K, &p_client_cb_data->read_result.p_value[1], sirk);
                }
                else
                {
                    goto read_failed;
                }
                PROTOCOL_PRINT_INFO1("[CSIS] SIRK decryption success %b",
                                     TRACE_BINARY(CSIS_SIRK_LEN, sirk));
            }
            else
            {
                memcpy(sirk, &p_client_cb_data->read_result.p_value[1], CSIS_SIRK_LEN);
            }
            p_csis_db->sirk_type = (T_CSIS_SIRK_TYPE)p_client_cb_data->read_result.p_value[0];
            memcpy(p_csis_db->cur_sirk, sirk, CSIS_SIRK_LEN);
            p_csis_db->cur_read_flags |= CSIS_SIRK_FLAG;
            if (csis_read_char_value(conn_handle, p_client_cb_data->read_result.srv_instance_id,
                                     CSIS_CHAR_SIZE, set_coordinator_discover_cb))
            {
                p_csis_db->cur_read_req_flags |= CSIS_SIZE_FLAG;
            }
            if (csis_read_char_value(conn_handle, p_client_cb_data->read_result.srv_instance_id,
                                     CSIS_CHAR_LOCK, set_coordinator_discover_cb))
            {
                p_csis_db->cur_read_req_flags |= CSIS_LOCK_FLAG;
            }
            if (csis_read_char_value(conn_handle, p_client_cb_data->read_result.srv_instance_id,
                                     CSIS_CHAR_RANK, set_coordinator_discover_cb))
            {
                p_csis_db->cur_read_req_flags |= CSIS_RANK_FLAG;
            }
        }
        else if (p_client_cb_data->read_result.char_uuid.p.uuid16 == CSIS_UUID_CHAR_SIZE)
        {
            p_csis_db->cur_size = p_client_cb_data->read_result.p_value[0];
            p_csis_db->cur_read_flags |= CSIS_SIZE_FLAG;
        }
        else if (p_client_cb_data->read_result.char_uuid.p.uuid16 == CSIS_UUID_CHAR_LOCK)
        {
            p_csis_db->cur_lock = (T_CSIS_LOCK)p_client_cb_data->read_result.p_value[0];
            p_csis_db->cur_read_flags |= CSIS_LOCK_FLAG;
        }
        else if (p_client_cb_data->read_result.char_uuid.p.uuid16 == CSIS_UUID_CHAR_RANK)
        {
            p_csis_db->cur_rank = p_client_cb_data->read_result.p_value[0];
            p_csis_db->cur_read_flags |= CSIS_RANK_FLAG;
        }
    }
    else
    {
        goto read_failed;
    }
    if (p_csis_db->cur_read_req_flags == p_csis_db->cur_read_flags)
    {
        T_LE_AUDIO_DEV *p_member = NULL;
        uint16_t serv_uuid = GATT_UUID_CSIS;
        T_ATTR_UUID srv_uuid;
        srv_uuid.is_uuid16 = true;
        srv_uuid.instance_id = p_csis_db->cur_srv_idx;
        srv_uuid.p.uuid16 = GATT_UUID_CSIS;
        if (gatt_client_check_cccd_enabled(conn_handle, &srv_uuid, NULL) == false)
        {
            gatt_client_enable_srv_cccd(conn_handle, &srv_uuid, GATT_CLIENT_CONFIG_ALL);
        }

        p_coor_set = set_coordinator_find_by_sirk(p_csis_db->cur_sirk);
        if (p_coor_set == NULL)
        {
            PROTOCOL_PRINT_INFO0("[CSIS] read_csis_chars_callback: not found coordinate set elem, alloc one");
            T_ATTR_UUID srv_uuid;
            T_ATTR_UUID include_srv;
            srv_uuid.is_uuid16 = true;
            srv_uuid.p.uuid16 = GATT_UUID_CSIS;
            srv_uuid.instance_id = p_client_cb_data->read_result.srv_instance_id;
            if (gatt_client_find_primary_srv_by_include(conn_handle, &srv_uuid,
                                                        &include_srv))
            {
                PROTOCOL_PRINT_INFO2("[CSIS] csis[%d] included by srv uuid: 0x%x",
                                     p_client_cb_data->read_result.srv_instance_id, include_srv.p.uuid16);
                serv_uuid = include_srv.p.uuid16;
            }
        }
        else
        {
            serv_uuid = p_coor_set->p_csip_cb->serv_uuid;
            p_member = ble_audio_group_find_dev(p_coor_set, p_link->remote_bd,
                                                p_link->remote_bd_type);
        }

        memcpy(read_result.mem_info.bd_addr, p_link->remote_bd, 6);
        read_result.mem_info.addr_type = p_link->remote_bd_type;
        memcpy(read_result.mem_info.sirk, p_csis_db->cur_sirk, CSIS_SIRK_LEN);
        read_result.mem_info.srv_instance_id = p_csis_db->cur_srv_idx;
        read_result.mem_info.srv_uuid = serv_uuid;
        read_result.mem_info.rank = p_csis_db->cur_rank;
        read_result.mem_info.set_mem_size = p_csis_db->cur_size;
        read_result.mem_info.lock = p_csis_db->cur_lock;
        read_result.group_handle = p_coor_set;
        read_result.dev_handle = p_member;
        read_result.mem_info.char_exit = p_csis_db->cur_read_flags;
        read_result.conn_handle = conn_handle;
        read_result.mem_info.sirk_type = p_csis_db->sirk_type;
        if (p_member)
        {
            p_member->lock = p_csis_db->cur_lock;
        }
        if (p_member && p_member->rank != 0 && !p_coor_set->p_csip_cb->rank_sorted)
        {
            set_members_quick_sort_by_rank(p_coor_set);
        }
        if (p_link->p_csis_client != NULL)
        {
            ble_audio_mem_free(p_link->p_csis_client);
            p_link->p_csis_client = NULL;
        }
        if (p_member)
        {
            set_coordinator_check_lock_state(p_coor_set, SET_COORDINATOR_EVENT_MEM_ADD);
            if (p_coor_set->p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_LOCK)
            {
                set_coordinator_lock_req(p_coor_set);
            }
        }
        ble_audio_mgr_dispatch(LE_AUDIO_MSG_CSIS_CLIENT_READ_RESULT, &read_result);
    }
    return APP_RESULT_SUCCESS;
read_failed:
    read_result.conn_handle = conn_handle;
    if (p_client_cb_data->read_result.cause == GAP_SUCCESS)
    {
        p_client_cb_data->read_result.cause = GATT_ERR | GATT_ERR_PROC_FAIL;
    }
    read_result.cause = p_client_cb_data->read_result.cause;
    if (p_link->p_csis_client != NULL)
    {
        ble_audio_mem_free(p_link->p_csis_client);
        p_link->p_csis_client = NULL;
    }
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_CSIS_CLIENT_READ_RESULT, &read_result);
    return APP_RESULT_APP_ERR;
}

bool set_coordinator_add_group(T_BLE_AUDIO_GROUP_HANDLE *p_group_handle,
                               P_FUN_AUDIO_GROUP_CB p_fun_cb,
                               T_BLE_AUDIO_DEV_HANDLE *p_dev_handle, T_CSIS_SET_MEM_INFO *p_mem_info)
{
    T_LE_AUDIO_GROUP *p_coor_set = NULL;
    T_LE_AUDIO_DEV *p_member = NULL;
    if (p_group_handle == NULL || p_dev_handle == NULL || p_mem_info == NULL)
    {
        goto failed;
    }
    if (p_mem_info->char_exit == 0)
    {
        goto failed;
    }
    p_coor_set = ble_audio_group_allocate_by_dev_num(p_mem_info->set_mem_size);
    if (p_coor_set == NULL)
    {
        goto failed;
    }
    p_coor_set->p_csip_cb = ble_audio_mem_zalloc(sizeof(T_CSIP_CB));
    if (p_coor_set->p_csip_cb == NULL)
    {
        goto failed;
    }
    p_member = ble_audio_group_add_dev(p_coor_set, p_mem_info->bd_addr,
                                       p_mem_info->addr_type);
    if (p_member == NULL)
    {
        goto failed;
    }
    p_coor_set->p_csip_cb->size = p_mem_info->set_mem_size;
    p_coor_set->p_csip_cb->serv_uuid = p_mem_info->srv_uuid;
    p_coor_set->p_csip_cb->char_exit = p_mem_info->char_exit;
    p_coor_set->p_csip_cb->sirk_type = p_mem_info->sirk_type;
    memcpy(p_coor_set->p_csip_cb->sirk, p_mem_info->sirk, CSIS_SIRK_LEN);
    p_member->srv_instance_id = p_mem_info->srv_instance_id;
    p_member->lock = p_mem_info->lock;
    p_member->rank = p_mem_info->rank;
    p_member->char_exit = p_mem_info->char_exit;
    if (p_fun_cb)
    {
        ble_audio_group_reg_cb(p_coor_set, p_fun_cb);
    }
    set_coordinator_check_lock_state(p_coor_set, SET_COORDINATOR_EVENT_INIT);
    *p_group_handle = p_coor_set;
    *p_dev_handle = p_member;
    T_CAP_SET_MEM_ADD mem_add;
    mem_add.new_group = true;
    mem_add.group_handle = p_coor_set;
    mem_add.dev_handle = p_member;
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_CAP_SET_MEM_ADD, &mem_add);
    return true;
failed:
    if (p_coor_set)
    {
        ble_audio_group_release((T_BLE_AUDIO_GROUP_HANDLE *)&p_coor_set);
        goto failed;
    }
    PROTOCOL_PRINT_ERROR0("[CSIS] set_coordinator_add_group: fail");
    return false;
}
bool set_coordinator_add_dev(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                             T_BLE_AUDIO_DEV_HANDLE *p_dev_handle,
                             T_CSIS_SET_MEM_INFO *p_mem_info)
{
    T_LE_AUDIO_GROUP *p_coor_set = (T_LE_AUDIO_GROUP *)group_handle;
    T_LE_AUDIO_DEV *p_member = NULL;
    T_CSIP_CB *p_csip_cb = set_coordinator_get_cb(p_coor_set);
    if (p_csip_cb == NULL)
    {
        goto failed;
    }
    if (p_dev_handle == NULL || p_mem_info == NULL)
    {
        goto failed;
    }
    if (p_mem_info->char_exit == 0)
    {
        goto failed;
    }
    if (memcmp(p_coor_set->p_csip_cb->sirk, p_mem_info->sirk, CSIS_SIRK_LEN) != 0)
    {
        goto failed;
    }
    p_member = ble_audio_group_add_dev(p_coor_set, p_mem_info->bd_addr,
                                       p_mem_info->addr_type);
    if (p_member == NULL)
    {
        goto failed;
    }
    p_member->srv_instance_id = p_mem_info->srv_instance_id;
    p_member->lock = p_mem_info->lock;
    p_member->rank = p_mem_info->rank;
    p_member->char_exit = p_mem_info->char_exit;
    if (p_member->rank != 0)
    {
        p_coor_set->p_csip_cb->rank_sorted = false;
        set_members_quick_sort_by_rank(p_coor_set);
    }
    set_coordinator_check_lock_state(p_coor_set, SET_COORDINATOR_EVENT_MEM_ADD);
    if (p_coor_set->p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_LOCK)
    {
        set_coordinator_lock_req(p_coor_set);
    }
    T_CAP_SET_MEM_ADD mem_add;
    mem_add.new_group = false;
    mem_add.group_handle = group_handle;
    mem_add.dev_handle = p_member;
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_CAP_SET_MEM_ADD, &mem_add);
    *p_dev_handle = p_member;
    return true;
failed:
    PROTOCOL_PRINT_ERROR0("[CSIS] set_coordinator_add_dev: fail");
    return false;
}

bool set_coordinator_read_chars(uint16_t conn_handle, uint8_t srv_instance_id)
{
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link && p_link->p_csis_client == NULL)
    {
        T_LE_AUDIO_DEV *p_set_member = NULL;
        T_LE_AUDIO_GROUP *p_coor_set;
        T_CSIS_CLIENT_DB *p_csis_db = ble_audio_mem_zalloc(sizeof(T_CSIS_CLIENT_DB));
        if (p_csis_db == NULL)
        {
            goto failed;
        }
        p_link->p_csis_client = p_csis_db;
        for (uint8_t i = 0; i < audio_group_queue.count; i++)
        {
            p_coor_set = (T_LE_AUDIO_GROUP *)os_queue_peek(&audio_group_queue, i);
            if (p_coor_set != NULL && p_coor_set->p_csip_cb != NULL)
            {
                T_LE_AUDIO_DEV *p_temp_set_member = ble_audio_group_find_dev(p_coor_set, p_link->remote_bd,
                                                                             p_link->remote_bd_type);
                if (p_temp_set_member != NULL &&
                    p_temp_set_member->srv_instance_id == srv_instance_id)
                {
                    p_set_member = p_temp_set_member;
                    break;
                }
            }
        }
        if (p_set_member)
        {
            if (p_set_member->char_exit)
            {
                p_csis_db->cur_srv_idx = p_set_member->srv_instance_id;
                p_csis_db->cur_size = p_coor_set->p_csip_cb->size;
                p_csis_db->cur_lock = CSIS_NONE_LOCK;
                p_csis_db->cur_rank = p_set_member->rank;
                p_csis_db->cur_read_flags = p_set_member->char_exit;
                p_csis_db->sirk_type = p_coor_set->p_csip_cb->sirk_type;
                memcpy(p_csis_db->cur_sirk, p_coor_set->p_csip_cb->sirk, CSIS_SIRK_LEN);
                p_csis_db->cur_read_req_flags = p_set_member->char_exit;
                if (csis_read_char_value(conn_handle, srv_instance_id, CSIS_CHAR_LOCK, set_coordinator_discover_cb))
                {
                    p_csis_db->cur_read_req_flags |= CSIS_LOCK_FLAG;
                    return true;
                }
            }
        }
        if (csis_read_char_value(conn_handle, srv_instance_id, CSIS_CHAR_SIRK, set_coordinator_discover_cb))
        {
            p_csis_db->cur_read_req_flags |= CSIS_SIRK_FLAG;
            return true;
        }
        else
        {
            p_link->p_csis_client = NULL;
            ble_audio_mem_free(p_csis_db);
        }
    }
failed:
    PROTOCOL_PRINT_ERROR0("[CSIS] set_coordinator_read_chars: failed");
    return false;
}

T_BLE_AUDIO_GROUP_HANDLE set_coordinator_find_by_addr(uint8_t *bd_addr, uint8_t addr_type,
                                                      uint16_t srv_uuid, T_BLE_AUDIO_DEV_HANDLE *p_dev_handle)
{
    uint8_t i;
    T_LE_AUDIO_GROUP *p_coor_set;
    T_LE_AUDIO_DEV *p_set_member;

    for (i = 0; i < audio_group_queue.count; i++)
    {
        p_coor_set = (T_LE_AUDIO_GROUP *)os_queue_peek(&audio_group_queue, i);
        if (p_coor_set != NULL && p_coor_set->p_csip_cb != NULL &&
            p_coor_set->p_csip_cb->serv_uuid == srv_uuid)
        {
            p_set_member = ble_audio_group_find_dev(p_coor_set, bd_addr, addr_type);
            if (p_set_member != NULL)
            {
                if (p_dev_handle)
                {
                    *p_dev_handle = (T_BLE_AUDIO_DEV_HANDLE)p_set_member;
                }
                return (T_BLE_AUDIO_GROUP_HANDLE)p_coor_set;
            }
        }
    }
    return NULL;
}

T_BLE_AUDIO_GROUP_HANDLE set_coordinator_find_by_rsi(uint8_t *p_rsi)
{
    uint8_t i;
    T_LE_AUDIO_GROUP *p_coor_set;

    for (i = 0; i < audio_group_queue.count; i++)
    {
        p_coor_set = (T_LE_AUDIO_GROUP *)os_queue_peek(&audio_group_queue, i);
        if (p_coor_set != NULL && p_coor_set->p_csip_cb != NULL)
        {
            if (csis_check_rsi(p_coor_set->p_csip_cb->sirk, p_rsi))
            {
                return (T_BLE_AUDIO_GROUP_HANDLE)p_coor_set;
            }
        }
    }

    return NULL;
}

T_BLE_AUDIO_GROUP_HANDLE set_coordinator_find_by_sirk(uint8_t *p_sirk)
{
    uint8_t i;
    T_LE_AUDIO_GROUP *p_coor_set;

    for (i = 0; i < audio_group_queue.count; i++)
    {
        p_coor_set = (T_LE_AUDIO_GROUP *)os_queue_peek(&audio_group_queue, i);
        if (p_coor_set != NULL && p_coor_set->p_csip_cb != NULL)
        {
            if (memcmp(p_coor_set->p_csip_cb->sirk, p_sirk, CSIS_SIRK_LEN) == 0)
            {
                return (T_BLE_AUDIO_GROUP_HANDLE)p_coor_set;
            }
        }
    }

    return NULL;
}

bool set_coordinator_get_mem_info(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                                  T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                  T_CSIS_SET_MEM_INFO *p_info)
{
    uint8_t i;
    T_LE_AUDIO_DEV *p_set_member;
    T_LE_AUDIO_GROUP *p_coor_set = (T_LE_AUDIO_GROUP *)group_handle;
    T_CSIP_CB *p_csip_cb = set_coordinator_get_cb(p_coor_set);
    if (p_info == NULL)
    {
        return false;
    }
    if (p_csip_cb == NULL)
    {
        return false;
    }

    for (i = 0; i < p_coor_set->dev_queue.count; i++)
    {
        p_set_member = (T_LE_AUDIO_DEV *)os_queue_peek(&p_coor_set->dev_queue, i);
        if (p_set_member != NULL && p_set_member->is_used == true)
        {
            if (p_set_member == (T_LE_AUDIO_DEV *)dev_handle)
            {
                p_info->addr_type = p_set_member->addr_type;
                p_info->rank = p_set_member->rank;
                p_info->set_mem_size = p_csip_cb->size;
                p_info->srv_uuid = p_csip_cb->serv_uuid;
                p_info->srv_instance_id = p_set_member->srv_instance_id;
                p_info->sirk_type = p_csip_cb->sirk_type;
                p_info->lock = p_set_member->lock;
                p_info->char_exit = p_set_member->char_exit;
                memcpy(p_info->bd_addr, p_set_member->bd_addr, GAP_BD_ADDR_LEN);
                memcpy(p_info->sirk, p_csip_cb->sirk, CSIS_SIRK_LEN);
                return true;
            }
        }
    }
    return false;
}


T_APP_RESULT set_coordinator_csis_callback(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                           void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    T_LE_AUDIO_DEV *p_set_member = NULL;
    uint8_t i;
    T_LE_AUDIO_GROUP *p_coor_set;
    if (p_link == NULL)
    {
        PROTOCOL_PRINT_ERROR0("[CSIS] set_coordinator_csis_callback: link is null");
        return APP_RESULT_APP_ERR;
    }
    switch (type)
    {
    case GATT_CLIENT_EVENT_DIS_DONE:
        {
            uint8_t srv_num = 0;
            T_CSIS_CLIENT_DIS_DONE dis_done;
            dis_done.conn_handle = conn_handle;
            dis_done.is_found = p_client_cb_data->dis_done.is_found;
            dis_done.srv_num = p_client_cb_data->dis_done.srv_instance_num;
            dis_done.load_from_ftl = p_client_cb_data->dis_done.load_from_ftl;
            if (p_client_cb_data->dis_done.is_found)
            {
                srv_num = p_client_cb_data->dis_done.srv_instance_num;
                if (p_link == NULL || srv_num == 0)
                {
                    return APP_RESULT_APP_ERR;
                }
                PROTOCOL_PRINT_INFO1("[CSIS] GATT_CLIENT_EVENT_DIS_DONE: srv_num %d", srv_num);
            }
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_CSIS_CLIENT_DIS_DONE, &dis_done);
        }
        break;

    case GATT_CLIENT_EVENT_NOTIFY_IND:
        {
            PROTOCOL_PRINT_INFO2("[CSIS] GATT_CLIENT_EVENT_NOTIFY_IND: conn_handle 0x%x, srv_instance_id %d",
                                 conn_handle, p_client_cb_data->notify_ind.srv_instance_id);
            if (p_client_cb_data->notify_ind.char_uuid.p.uuid16 == CSIS_UUID_CHAR_SIRK)
            {
                uint8_t sirk[16];
                PROTOCOL_PRINT_INFO1("[CSIS] Notify CSIS_CHAR_SIRK: type %d",
                                     p_client_cb_data->notify_ind.p_value[0]);
                if (p_client_cb_data->notify_ind.value_size != 17)
                {
                    return APP_RESULT_INVALID_VALUE_SIZE;
                }
                if (p_client_cb_data->notify_ind.p_value[0] == CSIS_SIRK_ENC)
                {
                    uint8_t K[16];
                    if (csis_client_generate_le_k(p_link->remote_bd, p_link->remote_bd_type, p_link->conn_handle, K))
                    {
                        csis_crypto_sdf(K, &p_client_cb_data->notify_ind.p_value[1], sirk);
                    }
                    else
                    {
                        return APP_RESULT_APP_ERR;
                    }
                }
                else
                {
                    memcpy(sirk, &p_client_cb_data->notify_ind.p_value[1], CSIS_SIRK_LEN);
                }
                for (i = 0; i < audio_group_queue.count; i++)
                {
                    p_coor_set = (T_LE_AUDIO_GROUP *)os_queue_peek(&audio_group_queue, i);
                    if (p_coor_set != NULL && p_coor_set->p_csip_cb != NULL)
                    {
                        p_set_member = ble_audio_group_find_dev(p_coor_set, p_link->remote_bd,
                                                                p_link->remote_bd_type);
                        if (p_set_member != NULL &&
                            p_set_member->srv_instance_id == p_client_cb_data->notify_ind.srv_instance_id &&
                            memcmp(p_coor_set->p_csip_cb->sirk, sirk, CSIS_SIRK_LEN) != 0)
                        {
                            T_CSIS_CLIENT_SIRK_NOTIFY srk_change;
                            memcpy(p_coor_set->p_csip_cb->sirk, sirk, CSIS_SIRK_LEN);
                            p_coor_set->p_csip_cb->sirk_type = (T_CSIS_SIRK_TYPE)p_client_cb_data->notify_ind.p_value[0];
                            memcpy(srk_change.sirk, sirk, CSIS_SIRK_LEN);
                            srk_change.group_handle = p_coor_set;
                            srk_change.sirk_type = p_coor_set->p_csip_cb->sirk_type;
                            ble_audio_mgr_dispatch(LE_AUDIO_MSG_CSIS_CLIENT_SIRK_NOTIFY, &srk_change);
                            break;
                        }
                    }
                }

                if (p_link)
                {
                    T_CSIS_CLIENT_DB *p_csis_db = (T_CSIS_CLIENT_DB *)p_link->p_csis_client;
                    if (p_csis_db)
                    {
                        PROTOCOL_PRINT_INFO1("[CSIS] Notify CSIS_CHAR_SIRK: update SIRK, cur_read_flags 0x%x",
                                             p_csis_db->cur_read_flags);
                        if (p_csis_db->cur_read_flags & CSIS_SIRK_FLAG)
                        {
                            p_csis_db->sirk_type = (T_CSIS_SIRK_TYPE)p_client_cb_data->notify_ind.p_value[0];
                            memcpy(p_csis_db->cur_sirk, sirk, CSIS_SIRK_LEN);
                        }
                    }
                }
            }
            else if (p_client_cb_data->notify_ind.char_uuid.p.uuid16 == CSIS_UUID_CHAR_SIZE)
            {
                PROTOCOL_PRINT_INFO1("[CSIS] Notify CSIS_CHAR_SIZE: size %d",
                                     p_client_cb_data->notify_ind.p_value[0]);
                for (i = 0; i < audio_group_queue.count; i++)
                {
                    p_coor_set = (T_LE_AUDIO_GROUP *)os_queue_peek(&audio_group_queue, i);
                    if (p_coor_set != NULL && p_coor_set->p_csip_cb != NULL)
                    {
                        p_set_member = ble_audio_group_find_dev(p_coor_set, p_link->remote_bd,
                                                                p_link->remote_bd_type);
                        if (p_set_member != NULL &&
                            p_set_member->srv_instance_id == p_client_cb_data->notify_ind.srv_instance_id)
                        {
                            if (p_coor_set->p_csip_cb->size != p_client_cb_data->notify_ind.p_value[0])
                            {
                                T_CSIS_CLIENT_SIZE_NOTIFY size_change;
                                size_change.group_handle = p_coor_set;
                                size_change.old_size = p_coor_set->p_csip_cb->size;
                                p_coor_set->p_csip_cb->size = p_client_cb_data->notify_ind.p_value[0];
                                size_change.new_size = p_coor_set->p_csip_cb->size;
                                ble_audio_mgr_dispatch(LE_AUDIO_MSG_CSIS_CLIENT_SIZE_NOTIFY, &size_change);
                            }
                            break;
                        }
                    }
                }
            }
            else if (p_client_cb_data->notify_ind.char_uuid.p.uuid16 == CSIS_UUID_CHAR_LOCK)
            {
                PROTOCOL_PRINT_INFO1("[CSIS] Notify CSIS_CHAR_LOCK: lock %d",
                                     p_client_cb_data->notify_ind.p_value[0]);
                for (i = 0; i < audio_group_queue.count; i++)
                {
                    p_coor_set = (T_LE_AUDIO_GROUP *)os_queue_peek(&audio_group_queue, i);
                    if (p_coor_set != NULL && p_coor_set->p_csip_cb != NULL)
                    {
                        p_set_member = ble_audio_group_find_dev(p_coor_set, p_link->remote_bd,
                                                                p_link->remote_bd_type);
                        if (p_set_member != NULL &&
                            p_set_member->srv_instance_id == p_client_cb_data->notify_ind.srv_instance_id)
                        {
                            PROTOCOL_PRINT_INFO1("[CSIS] GATT_CLIENT_EVENT_NOTIFY_IND: found srv inde: %d",
                                                 p_client_cb_data->notify_ind.srv_instance_id);
                            if (p_set_member->lock != (T_CSIS_LOCK)p_client_cb_data->notify_ind.p_value[0])
                            {
                                if (p_set_member->lock_obtained && p_set_member->lock == CSIS_LOCKED)
                                {
                                    p_set_member->lock_obtained = false;
                                }
                                p_set_member->lock = (T_CSIS_LOCK)p_client_cb_data->notify_ind.p_value[0];
                                set_coordinator_check_lock_state(p_coor_set, SET_COORDINATOR_EVENT_LOCK_NOTIFY);
                                if (p_coor_set->p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_LOCK)
                                {
                                    set_coordinator_lock_req(p_coor_set);
                                }
                            }
                            break;
                        }
                    }
                }

            }
        }
        break;

    case GATT_CLIENT_EVENT_CCCD_CFG:
        {
            ble_audio_mgr_dispatch_client_attr_cccd_info(conn_handle, &p_client_cb_data->cccd_cfg);
        }
        break;

    default:
        break;
    }

    return result;
}

static void set_coordinator_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BLE_DM_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BLE_DM_EVENT_CONN_STATE:
        {
            T_BLE_AUDIO_LINK *p_link = NULL;
            p_link = ble_audio_link_find_by_conn_id(param->conn_state.conn_id);
            if (p_link == NULL)
            {
                PROTOCOL_PRINT_ERROR1("set_coordinator_dm_cback: not find the link, conn_id %d",
                                      param->conn_state.conn_id);
                break;
            }

            if (param->conn_state.state == GAP_CONN_STATE_DISCONNECTED)
            {
                uint8_t i;
                T_LE_AUDIO_GROUP *p_coor_set;
                T_LE_AUDIO_DEV *p_set_member;
                if (set_coor_discov_cb.set_coor != NULL && set_coor_discov_cb.discov_queue.count != 0)
                {
                    T_SET_MEM_DISCOV *p_set_mem;
                    for (uint8_t i = 0; i < set_coor_discov_cb.discov_queue.count; i++)
                    {
                        p_set_mem = (T_SET_MEM_DISCOV *)os_queue_peek(&set_coor_discov_cb.discov_queue, i);
                        if (p_set_mem->addr_type == p_link->remote_bd_type &&
                            memcmp(p_set_mem->bd_addr, p_link->remote_bd, 6) == 0)
                        {
                            os_queue_delete(&set_coor_discov_cb.discov_queue, p_set_mem);
                            ble_audio_mem_free(p_set_mem);
                        }
                    }
                }
                for (i = 0; i < audio_group_queue.count; i++)
                {
                    p_coor_set = (T_LE_AUDIO_GROUP *)os_queue_peek(&audio_group_queue, i);
                    if (p_coor_set != NULL && p_coor_set->p_csip_cb != NULL)
                    {
                        p_set_member = ble_audio_group_find_dev(p_coor_set, p_link->remote_bd, p_link->remote_bd_type);
                        if (p_set_member != NULL)
                        {
                            p_set_member->lock = CSIS_NONE_LOCK;
                            p_set_member->lock_obtained = false;
                            set_coordinator_check_lock_state(p_coor_set, SET_COORDINATOR_EVENT_MEM_DISCONN);
                        }
                    }
                }
            }
        }
        break;
    default:
        break;
    }
}

bool set_coordinator_init(void)
{
    //set_coordinator_dm_cback register must be before csis_client_init
    ble_dm_cback_register(set_coordinator_dm_cback);
    csis_client_init(set_coordinator_csis_callback);
    return true;
}
#if LE_AUDIO_DEINIT
void set_coordinator_deinit(void)
{
    T_SET_MEM_DISCOV *p_set_mem;
    while ((p_set_mem = os_queue_out(&set_coor_discov_cb.discov_queue)) != NULL)
    {
        ble_audio_mem_free(p_set_mem);
    }
    if (set_coor_discov_cb.timer_handle)
    {
        sys_timer_delete(set_coor_discov_cb.timer_handle);
        set_coor_discov_cb.timer_handle = NULL;
    }
    memset(&set_coor_discov_cb, 0, sizeof(set_coor_discov_cb));
}
#endif
#endif
