#include <string.h>
#include "trace.h"
#include "ble_audio_mgr.h"
#include "ble_audio_group_int.h"
#include "ble_audio_dm.h"
#include "ble_audio_ual.h"
#include "bap_int.h"
#include "gap_privacy.h"

#if LE_AUDIO_GROUP_SUPPORT
T_OS_QUEUE audio_group_queue;

typedef struct
{
    T_BLE_AUDIO_GROUP_HANDLE group_handle;
    uint8_t                  data[1];
} T_AUDIO_GROUP_MSG_DATA;

void ble_audio_group_send_msg(T_AUDIO_GROUP_MSG msg, T_BLE_AUDIO_GROUP_HANDLE group_handle,
                              uint8_t *p_data, uint8_t data_len, bool direct)
{
    T_LE_AUDIO_GROUP *p_audio_group = (T_LE_AUDIO_GROUP *)group_handle;

#if LE_AUDIO_DEINIT
    if (ble_audio_deinit_flow != 0)
    {
        return;
    }
#endif
    if (data_len != 0 && p_data == NULL)
    {
        goto failed;
    }

    if (ble_audio_group_handle_check(group_handle) == false)
    {
        goto failed;
    }

    if (data_len)
    {
        PROTOCOL_PRINT_INFO5("ble_audio_group_send_msg: group_handle %p, msg 0x%x, direct %d, data[%d] %b",
                             group_handle, msg, direct, data_len, TRACE_BINARY(data_len, p_data));
    }

    if (direct && p_audio_group->p_group_cb != NULL)
    {
        p_audio_group->p_group_cb(msg, group_handle, p_data);
    }
#if LE_AUDIO_IO_MESSAGE_HANDLER
    else
    {
        uint16_t subtype = msg;
        T_AUDIO_GROUP_MSG_DATA *p_msg = ble_audio_mem_zalloc(sizeof(T_AUDIO_GROUP_MSG_DATA) + data_len);
        if (p_msg == NULL)
        {
            goto failed;
        }
        subtype |= (BLE_AUDIO_GROUP << 8);
        p_msg->group_handle = group_handle;
        memcpy(p_msg->data, p_data, data_len);
        if (ble_audio_send_msg_to_app(subtype, p_msg) == false)
        {
            ble_audio_mem_free(p_msg);
        }
    }
#endif
    return;
failed:
    PROTOCOL_PRINT_ERROR1("ble_audio_group_send_msg: failed, msg %d", msg);
    return;
}

void ble_audio_group_hdl_event(void *p_msg, uint16_t event)
{
    T_AUDIO_GROUP_MSG_DATA *p_data = (T_AUDIO_GROUP_MSG_DATA *)p_msg;
    T_LE_AUDIO_GROUP *p_audio_group = (T_LE_AUDIO_GROUP *)p_data->group_handle;
    if (ble_audio_group_handle_check(p_data->group_handle) == false)
    {
        return;
    }
    PROTOCOL_PRINT_INFO1("ble_audio_group_hdl_event: event %d", event);
    if (p_audio_group->p_group_cb)
    {
        p_audio_group->p_group_cb((T_AUDIO_GROUP_MSG)event, p_data->group_handle, p_data->data);
    }
}

T_BLE_AUDIO_GROUP_HANDLE ble_audio_group_allocate(void)
{
    T_LE_AUDIO_GROUP *p_audio_group;
    p_audio_group = ble_audio_mem_zalloc(sizeof(T_LE_AUDIO_GROUP));

    if (p_audio_group == NULL)
    {
        PROTOCOL_PRINT_ERROR0("ble_audio_group_allocate: allocate mem failed");
        return NULL;
    }
    PROTOCOL_PRINT_INFO1("ble_audio_group_allocate: group_handle %p", p_audio_group);
    os_queue_in(&audio_group_queue, p_audio_group);
    os_queue_init(&p_audio_group->dev_queue);
    return p_audio_group;
}

T_BLE_AUDIO_GROUP_HANDLE ble_audio_group_allocate_by_dev_num(uint8_t dev_num)
{
    T_LE_AUDIO_GROUP *p_audio_group = ble_audio_group_allocate();
    T_LE_AUDIO_DEV *p_dev;
    if (p_audio_group == NULL)
    {
        goto failed;
    }
    for (uint8_t i = 0; i < dev_num; i++)
    {
        p_dev = ble_audio_mem_zalloc(sizeof(T_LE_AUDIO_DEV));
        if (p_dev != NULL)
        {
            os_queue_in(&p_audio_group->dev_queue, p_dev);
        }
        else
        {
            goto failed;
        }
    }
    PROTOCOL_PRINT_INFO2("ble_audio_group_allocate_by_dev_num: group_handle %p, dev_num %d",
                         p_audio_group, dev_num);
    return p_audio_group;
failed:
    PROTOCOL_PRINT_ERROR0("ble_audio_group_allocate_by_dev_num: failed");
    if (p_audio_group)
    {
        ble_audio_group_release((T_BLE_AUDIO_GROUP_HANDLE *)&p_audio_group);
    }
    return NULL;
}

bool ble_audio_group_reg_cb(T_BLE_AUDIO_GROUP_HANDLE group_handle, P_FUN_AUDIO_GROUP_CB p_fun_cb)
{
    T_LE_AUDIO_GROUP *p_audio_group = (T_LE_AUDIO_GROUP *)group_handle;

    if (ble_audio_group_handle_check(group_handle) == false)
    {
        return false;
    }
    p_audio_group->p_group_cb = p_fun_cb;
    return true;
}

void ble_audio_group_update_conn_num(T_LE_AUDIO_GROUP *p_audio_group)
{
    uint8_t dev_num = 0;
    T_LE_AUDIO_DEV *p_dev;
    for (uint8_t i = 0; i < p_audio_group->dev_queue.count; i++)
    {
        p_dev = (T_LE_AUDIO_DEV *)os_queue_peek(&p_audio_group->dev_queue, i);
        if (p_dev != NULL && p_dev->is_used == true)
        {
            if (p_dev->p_link != NULL && p_dev->p_link->state == GAP_CONN_STATE_CONNECTED)
            {
                dev_num++;
            }
        }
    }
    if (p_audio_group->conn_dev_num != dev_num)
    {
        p_audio_group->conn_dev_num = dev_num;
        PROTOCOL_PRINT_INFO2("ble_audio_group_update_conn_num: group_handle %p, conn_dev_num %d",
                             p_audio_group, dev_num);
    }
}

void ble_audio_group_print(T_LE_AUDIO_GROUP *p_audio_group)
{
    uint8_t i;
    if (p_audio_group == NULL)
    {
        return;
    }
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
    if (p_audio_group->p_csip_cb != NULL)
    {
        T_CSIP_CB *p_csip_cb = p_audio_group->p_csip_cb;
        PROTOCOL_PRINT_INFO3("ble_audio_group_print: lock_state %d, size %d, sirk %b",
                             p_csip_cb->lock_state, p_csip_cb->size,
                             TRACE_BINARY(CSIS_SIRK_LEN, p_csip_cb->sirk));
    }
#endif
#if LE_AUDIO_ASCS_CLIENT_SUPPORT
    PROTOCOL_PRINT_INFO2("ble_audio_group_print: dev num %d, session_num %d",
                         p_audio_group->dev_queue.count,
                         p_audio_group->session_num);
#endif
    for (i = 0; i < p_audio_group->dev_queue.count; i++)
    {
        T_LE_AUDIO_DEV *p_dev = (T_LE_AUDIO_DEV *)os_queue_peek(&p_audio_group->dev_queue, i);
        if (p_dev != NULL)
        {
            if (p_dev->is_used)
            {
                PROTOCOL_PRINT_INFO5("Audio Device[%d]: handle %p, p_link %p, bd_addr %s, addr_type %d",
                                     i,  p_dev, p_dev->p_link,
                                     TRACE_BDADDR(p_dev->bd_addr),
                                     p_dev->addr_type
                                    );
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
                PROTOCOL_PRINT_INFO4("Audio Device[%d]: lock %d, rank %d, lock_obtained %d",
                                     i, p_dev->lock,
                                     p_dev->rank,
                                     p_dev->lock_obtained
                                    );
#endif
            }
            else
            {
                PROTOCOL_PRINT_INFO2("Audio Device[%d]: handle %p, not used", i, p_dev);
            }

        }
    }
}

bool ble_audio_group_release(T_BLE_AUDIO_GROUP_HANDLE *p_group_handle)
{
    T_LE_AUDIO_DEV *p_dev = NULL;
    T_LE_AUDIO_GROUP *p_audio_group;
    if (p_group_handle == NULL)
    {
        return false;
    }
    p_audio_group = (T_LE_AUDIO_GROUP *)(*p_group_handle);
    if (ble_audio_group_handle_check(*p_group_handle) == false)
    {
        return false;
    }
#if LE_AUDIO_ASCS_CLIENT_SUPPORT
    if (p_audio_group->session_num != 0)
    {
        PROTOCOL_PRINT_ERROR1("ble_audio_group_release: failed, session_num %d != 0",
                              p_audio_group->session_num);
        return false;
    }
#endif
    ble_audio_group_print(p_audio_group);
    PROTOCOL_PRINT_INFO1("ble_audio_group_release: group_handle %p", p_audio_group);
    while ((p_dev = (T_LE_AUDIO_DEV *)os_queue_out(&p_audio_group->dev_queue)) != NULL)
    {
        ble_audio_mem_free(p_dev);
    }
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
    if (p_audio_group->p_csip_cb)
    {
        ble_audio_mem_free(p_audio_group->p_csip_cb);
    }
#endif
    if (p_audio_group->p_cap)
    {
        ble_audio_mem_free(p_audio_group->p_cap);
    }
    os_queue_delete(&audio_group_queue, p_audio_group);
    ble_audio_mem_free(p_audio_group);
    *p_group_handle = NULL;
    return true;
}

#if LE_AUDIO_ASCS_CLIENT_SUPPORT
bool ble_audio_group_session(T_BLE_AUDIO_GROUP_HANDLE group_handle, bool is_add)
{
    T_LE_AUDIO_GROUP *p_audio_group = (T_LE_AUDIO_GROUP *)group_handle;
    if (ble_audio_group_handle_check(group_handle) == false)
    {
        return false;
    }
    if (is_add)
    {
        p_audio_group->session_num++;
    }
    else
    {
        p_audio_group->session_num--;
    }
    return true;
}
#endif

bool ble_audio_group_handle_check_int(const char *p_func_name,
                                      T_BLE_AUDIO_GROUP_HANDLE group_handle)
{
    if (group_handle != NULL)
    {
        if (os_queue_search(&audio_group_queue, group_handle) == true)
        {
            return true;
        }
    }
    PROTOCOL_PRINT_ERROR2("ble_audio_group_handle_check:failed, %s, group_handle %p",
                          TRACE_STRING(p_func_name), group_handle);
    return false;
}

bool ble_audio_dev_handle_check_int(const char *p_func_name, T_BLE_AUDIO_GROUP_HANDLE group_handle,
                                    T_BLE_AUDIO_DEV_HANDLE dev_handle)
{
    if (ble_audio_group_handle_check(group_handle))
    {
        T_LE_AUDIO_GROUP *p_audio_group = (T_LE_AUDIO_GROUP *)group_handle;
        if (dev_handle != NULL)
        {
            if (os_queue_search(&p_audio_group->dev_queue, dev_handle) == true)
            {
                return true;
            }
        }
    }
    PROTOCOL_PRINT_ERROR3("ble_audio_dev_handle_check: failed, %s, group_handle %p, dev_handle %p",
                          TRACE_STRING(p_func_name), group_handle, dev_handle);
    return false;
}

T_BLE_AUDIO_DEV_HANDLE ble_audio_group_find_unused_dev(T_LE_AUDIO_GROUP *p_audio_group)
{
    T_LE_AUDIO_DEV *p_dev;
    for (uint8_t i = 0; i < p_audio_group->dev_queue.count; i++)
    {
        p_dev = (T_LE_AUDIO_DEV *)os_queue_peek(&p_audio_group->dev_queue, i);
        if (p_dev != NULL && p_dev->is_used == false)
        {
            return p_dev;
        }
    }
    return NULL;
}

void ble_audio_group_update_identity_addr(T_LE_AUDIO_DEV *p_dev)
{
    if (p_dev->addr_type == GAP_REMOTE_ADDR_LE_RANDOM)
    {
        if ((p_dev->bd_addr[5] & 0xC0) == RANDOM_ADDR_MASK_RESOLVABLE)
        {
            p_dev->is_rpa = true;
            if (ble_audio_ual_resolve_rpa(p_dev->bd_addr, p_dev->identity_addr,
                                          &p_dev->identity_addr_type) == false)
            {
                p_dev->identity_addr_type = 0xff;
            }
        }
        else if ((p_dev->bd_addr[5] & 0xC0) == RANDOM_ADDR_MASK_STATIC)
        {
            p_dev->identity_addr_type = GAP_IDENT_ADDR_RAND;
            p_dev->is_rpa = true;
            memcpy(p_dev->identity_addr, p_dev->bd_addr, GAP_BD_ADDR_LEN);
        }
    }
    else if (p_dev->addr_type == GAP_REMOTE_ADDR_LE_PUBLIC_IDENTITY ||
             p_dev->addr_type == GAP_REMOTE_ADDR_LE_RANDOM_IDENTITY)
    {
        p_dev->identity_addr_type = le_privacy_convert_addr_type((T_GAP_REMOTE_ADDR_TYPE)p_dev->addr_type);
        p_dev->is_rpa = true;
        memcpy(p_dev->identity_addr, p_dev->bd_addr, GAP_BD_ADDR_LEN);
    }
    else if (p_dev->addr_type == GAP_REMOTE_ADDR_LE_PUBLIC)
    {
        p_dev->identity_addr_type = GAP_IDENT_ADDR_PUBLIC;
        p_dev->is_rpa = true;
        memcpy(p_dev->identity_addr, p_dev->bd_addr, GAP_BD_ADDR_LEN);
    }
    PROTOCOL_PRINT_INFO4("ble_audio_group_update_identity_addr: group_handle %p, bd_addr %s, addr_type %d",
                         p_dev, p_dev->is_rpa, TRACE_BDADDR(p_dev->identity_addr), p_dev->identity_addr_type);
}

void ble_audio_group_update_link_info(T_LE_AUDIO_DEV *p_dev, T_BLE_AUDIO_LINK *p_link, bool is_add)
{
    p_dev->p_link = p_link;
    if (p_link)
    {
        if (p_link->is_rpa && p_link->identity_addr_type != 0xff)
        {
            p_dev->is_rpa = p_link->is_rpa;
            p_dev->identity_addr_type = p_link->identity_addr_type;
            memcpy(p_dev->identity_addr, p_link->identity_addr, GAP_BD_ADDR_LEN);
        }
    }
    else
    {
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
        p_dev->lock_obtained = false;
        p_dev->lock = CSIS_NONE_LOCK;
#endif
        if (is_add)
        {
            ble_audio_group_update_identity_addr(p_dev);
        }
    }
}

T_BLE_AUDIO_DEV_HANDLE ble_audio_group_add_dev(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                                               uint8_t *p_bd_addr, uint8_t addr_type)
{
    T_LE_AUDIO_DEV *p_dev;
    T_LE_AUDIO_GROUP *p_audio_group = (T_LE_AUDIO_GROUP *)group_handle;
    T_BLE_AUDIO_LINK *p_link;
    bool new_dev = false;
    if (ble_audio_group_handle_check(group_handle) == false)
    {
        PROTOCOL_PRINT_ERROR0("ble_audio_group_add_dev: check failed");
        return NULL;
    }
    if (p_bd_addr == NULL)
    {
        PROTOCOL_PRINT_ERROR0("ble_audio_group_add_dev: address is NULL");
        return NULL;
    }
    p_dev = ble_audio_group_find_dev(group_handle, p_bd_addr, addr_type);
    p_link = ble_audio_link_find_by_addr(p_bd_addr, addr_type);

    if (p_dev)
    {
        if (p_dev->p_link != p_link)
        {
            p_dev->p_link = p_link;
            ble_audio_group_update_conn_num(p_audio_group);
        }
        return p_dev;
    }
    p_dev = ble_audio_group_find_unused_dev(p_audio_group);
    if (p_dev == NULL)
    {
        p_dev = ble_audio_mem_zalloc(sizeof(T_LE_AUDIO_DEV));
        new_dev = true;
    }
    if (p_dev != NULL)
    {
        p_dev->addr_type = (T_GAP_REMOTE_ADDR_TYPE)addr_type;
        memcpy(p_dev->bd_addr, p_bd_addr, GAP_BD_ADDR_LEN);

        ble_audio_group_update_link_info(p_dev, p_link, true);
        PROTOCOL_PRINT_INFO4("ble_audio_group_add_dev: group_handle %p, dev_handle %p, bd_addr %s, addr_type %d",
                             group_handle, p_dev, TRACE_BDADDR(p_bd_addr), addr_type);
        if (new_dev)
        {
            os_queue_in(&p_audio_group->dev_queue, p_dev);
        }
        if (p_dev->is_used == false)
        {
            p_dev->is_used = true;
#if LE_AUDIO_ASCS_CLIENT_SUPPORT
            if (p_audio_group->session_num && p_link != NULL)
            {
                bap_unicast_client_check_dev_add(p_audio_group, p_dev);
            }
#endif
        }
        ble_audio_group_update_conn_num(p_audio_group);
        return p_dev;
    }

    PROTOCOL_PRINT_ERROR2("ble_audio_group_add_dev: failed, bd_addr %s, addr_type %d",
                          TRACE_BDADDR(p_bd_addr),
                          addr_type);
    return NULL;
}

bool ble_audio_group_del_dev(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                             T_BLE_AUDIO_DEV_HANDLE *p_dev_handle)
{
    T_LE_AUDIO_GROUP *p_audio_group = (T_LE_AUDIO_GROUP *)group_handle;
    T_LE_AUDIO_DEV *p_dev;
    if (p_dev_handle == NULL)
    {
        return false;
    }
    p_dev = (T_LE_AUDIO_DEV *)(*p_dev_handle);
    if (ble_audio_dev_handle_check(group_handle, *p_dev_handle) == false)
    {
        return false;
    }
#if LE_AUDIO_ASCS_CLIENT_SUPPORT
    if (p_audio_group->session_num != 0)
    {
        PROTOCOL_PRINT_ERROR1("ble_audio_group_del_dev: failed, session_num %d != 0",
                              p_audio_group->session_num);
        return false;
    }
#endif
    PROTOCOL_PRINT_INFO2("ble_audio_group_del_dev: group_handle %p, dev_handle %p", p_audio_group,
                         p_dev);
    memset(&p_dev->is_used, 0, sizeof(T_LE_AUDIO_DEV) - 4);
    *p_dev_handle = NULL;
    ble_audio_group_update_conn_num(p_audio_group);
    if (ble_audio_group_get_used_dev_num(group_handle, false) == 0)
    {
        ble_audio_group_send_msg(AUDIO_GROUP_MSG_DEV_EMPTY, p_audio_group, NULL, 0, true);
    }
    return true;
}

T_BLE_AUDIO_DEV_HANDLE ble_audio_group_find_dev(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                                                uint8_t *bd_addr, uint8_t addr_type)
{
    uint8_t i;
    T_LE_AUDIO_GROUP *p_audio_group = (T_LE_AUDIO_GROUP *)group_handle;
    T_LE_AUDIO_DEV *p_dev;

    if (ble_audio_group_handle_check(group_handle) == false)
    {
        return NULL;
    }
    if (bd_addr == NULL)
    {
        return NULL;
    }

    for (i = 0; i < p_audio_group->dev_queue.count; i++)
    {
        p_dev = (T_LE_AUDIO_DEV *)os_queue_peek(&p_audio_group->dev_queue, i);
        if (p_dev != NULL && p_dev->is_used == true)
        {
            T_GAP_IDENT_ADDR_TYPE resolved_addr_type = le_privacy_convert_addr_type((
                                                                                        T_GAP_REMOTE_ADDR_TYPE)addr_type);
            if (((p_dev->addr_type == addr_type) ||
                 (p_dev->addr_type == (T_GAP_REMOTE_ADDR_TYPE)resolved_addr_type)) &&
                memcmp(p_dev->bd_addr, bd_addr, GAP_BD_ADDR_LEN) == 0)
            {
                return p_dev;
            }
        }
    }
    if (addr_type == GAP_REMOTE_ADDR_LE_RANDOM && (bd_addr[5] & 0xC0) == 0x40)
    {
        uint8_t resolved_addr[GAP_BD_ADDR_LEN];
        T_GAP_IDENT_ADDR_TYPE resolved_addr_type;
        if (ble_audio_ual_resolve_rpa(bd_addr, resolved_addr, &resolved_addr_type))
        {
            for (i = 0; i < p_audio_group->dev_queue.count; i++)
            {
                p_dev = (T_LE_AUDIO_DEV *)os_queue_peek(&p_audio_group->dev_queue, i);
                if (p_dev != NULL && p_dev->is_used == true)
                {
                    if (p_dev->is_rpa && p_dev->identity_addr_type != 0xff &&
                        p_dev->identity_addr_type == resolved_addr_type &&
                        memcmp(p_dev->identity_addr, resolved_addr, GAP_BD_ADDR_LEN) == 0)
                    {
                        return p_dev;
                    }
                }
            }
        }
    }
    else if (addr_type == GAP_REMOTE_ADDR_LE_PUBLIC_IDENTITY ||
             addr_type == GAP_REMOTE_ADDR_LE_RANDOM_IDENTITY ||
             addr_type == GAP_REMOTE_ADDR_LE_PUBLIC ||
             (addr_type == GAP_REMOTE_ADDR_LE_RANDOM && (bd_addr[5] & 0xC0) == RANDOM_ADDR_MASK_STATIC))
    {
        T_GAP_IDENT_ADDR_TYPE resolved_addr_type = le_privacy_convert_addr_type((
                                                                                    T_GAP_REMOTE_ADDR_TYPE)addr_type);
        for (i = 0; i < p_audio_group->dev_queue.count; i++)
        {
            p_dev = (T_LE_AUDIO_DEV *)os_queue_peek(&p_audio_group->dev_queue, i);
            if (p_dev != NULL && p_dev->is_used == true)
            {
                if (p_dev->is_rpa && p_dev->identity_addr_type != 0xff &&
                    p_dev->identity_addr_type == resolved_addr_type &&
                    memcmp(p_dev->identity_addr, bd_addr, GAP_BD_ADDR_LEN) == 0)
                {
                    return p_dev;
                }
            }
        }
    }

    return NULL;
}

T_BLE_AUDIO_DEV_HANDLE ble_audio_group_find_dev_by_conn_handle(T_BLE_AUDIO_GROUP_HANDLE
                                                               group_handle,
                                                               uint16_t conn_handle)
{
    uint8_t i;
    T_LE_AUDIO_GROUP *p_audio_group = (T_LE_AUDIO_GROUP *)group_handle;
    T_LE_AUDIO_DEV *p_dev;

    if (ble_audio_group_handle_check(group_handle) == false)
    {
        return NULL;
    }

    for (i = 0; i < p_audio_group->dev_queue.count; i++)
    {
        p_dev = (T_LE_AUDIO_DEV *)os_queue_peek(&p_audio_group->dev_queue, i);
        if (p_dev != NULL && p_dev->is_used == true)
        {
            if (p_dev->p_link != NULL && p_dev->p_link->used == true &&
                p_dev->p_link->conn_handle == conn_handle)
            {
                return p_dev;
            }
        }
    }
    return NULL;
}

bool ble_audio_group_get_dev_info(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                                  T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                  T_AUDIO_DEV_INFO *p_info)
{
    T_LE_AUDIO_DEV *p_dev;
    if (ble_audio_dev_handle_check(group_handle, dev_handle) == false)
    {
        goto failed;
    }
    if (p_info == NULL)
    {
        goto failed;
    }
    p_dev = (T_LE_AUDIO_DEV *)dev_handle;
    memset(p_info, 0, sizeof(T_AUDIO_DEV_INFO));
    p_info->dev_handle = dev_handle;
    if (p_dev->is_used)
    {
        memcpy(p_info->bd_addr, p_dev->bd_addr, 6);
        if (p_dev->p_link)
        {
            p_info->conn_state = p_dev->p_link->state;
            p_info->conn_handle = p_dev->p_link->conn_handle;
        }
        p_info->addr_type = p_dev->addr_type;
    }
    p_info->is_used = p_dev->is_used;
    return true;
failed:
    return false;
}

uint8_t ble_audio_group_get_dev_num(T_BLE_AUDIO_GROUP_HANDLE group_handle)
{

    T_LE_AUDIO_GROUP *p_audio_group = (T_LE_AUDIO_GROUP *)group_handle;
    if (ble_audio_group_handle_check(group_handle) == false)
    {
        goto failed;
    }
    return p_audio_group->dev_queue.count;
failed:
    return 0;
}

uint8_t ble_audio_group_get_used_dev_num(T_BLE_AUDIO_GROUP_HANDLE group_handle, bool check_conn)
{
    T_LE_AUDIO_GROUP *p_audio_group = (T_LE_AUDIO_GROUP *)group_handle;
    T_LE_AUDIO_DEV *p_dev;
    uint8_t dev_num = 0;
    if (ble_audio_group_handle_check(group_handle) == false)
    {
        goto failed;
    }
    for (uint8_t i = 0; i < p_audio_group->dev_queue.count; i++)
    {
        p_dev = (T_LE_AUDIO_DEV *)os_queue_peek(&p_audio_group->dev_queue, i);
        if (p_dev != NULL && p_dev->is_used == true)
        {
            if (check_conn)
            {
                if (p_dev->p_link != NULL && p_dev->p_link->state == GAP_CONN_STATE_CONNECTED)
                {
                    dev_num++;
                }
            }
            else
            {
                dev_num++;
            }
        }
    }
    return dev_num;
failed:
    return 0;
}

bool ble_audio_group_get_info(T_BLE_AUDIO_GROUP_HANDLE group_handle, uint8_t *p_dev_num,
                              T_AUDIO_DEV_INFO *p_dev_tbl)
{
    T_LE_AUDIO_GROUP *p_audio_group = (T_LE_AUDIO_GROUP *)group_handle;
    if (p_dev_num == NULL || p_dev_tbl == NULL)
    {
        goto failed;
    }
    if (ble_audio_group_handle_check(group_handle) == false)
    {
        *p_dev_num = 0;
        goto failed;
    }

    if (*p_dev_num < p_audio_group->dev_queue.count)
    {
        *p_dev_num = 0;
        goto failed;
    }
    for (uint8_t i = 0; i < p_audio_group->dev_queue.count; i++)
    {
        T_LE_AUDIO_DEV *p_dev = (T_LE_AUDIO_DEV *)os_queue_peek(&p_audio_group->dev_queue, i);
        if (p_dev != NULL)
        {
            if (p_dev->is_used == true)
            {
                memcpy(p_dev_tbl[i].bd_addr, p_dev->bd_addr, 6);
                if (p_dev->p_link)
                {
                    p_dev_tbl[i].conn_state = p_dev->p_link->state;
                    p_dev_tbl[i].conn_handle = p_dev->p_link->conn_handle;
                }
                else
                {
                    p_dev_tbl[i].conn_state = GAP_CONN_STATE_DISCONNECTED;
                    p_dev_tbl[i].conn_handle = 0;
                }
                p_dev_tbl[i].addr_type = p_dev->addr_type;

            }
            else
            {
                memset(&p_dev_tbl[i], 0, sizeof(T_AUDIO_DEV_INFO));
            }
            p_dev_tbl[i].dev_handle = p_dev;
            p_dev_tbl[i].is_used = p_dev->is_used;
        }
    }
    *p_dev_num = p_audio_group->dev_queue.count;
    return true;
failed:
    return false;
}

static void ble_audio_group_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BLE_DM_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BLE_DM_EVENT_CONN_STATE:
        {
            uint8_t i, j;
            T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_id(param->conn_state.conn_id);
            T_LE_AUDIO_GROUP *p_audio_group;
            T_LE_AUDIO_DEV *p_dev;

            if (p_link != NULL)
            {
                for (i = 0; i < audio_group_queue.count; i++)
                {
                    p_audio_group = os_queue_peek(&audio_group_queue, i);
                    for (j = 0; j < p_audio_group->dev_queue.count; j++)
                    {
                        p_dev = (T_LE_AUDIO_DEV *)os_queue_peek(&p_audio_group->dev_queue, j);
                        if (p_dev != NULL && p_dev->is_used == true)
                        {
                            if (param->conn_state.state == GAP_CONN_STATE_DISCONNECTED)
                            {
                                if (p_dev->p_link == p_link)
                                {
                                    T_AUDIO_GROUP_MSG_DEV_DISCONN dev_info;
                                    ble_audio_group_update_link_info(p_dev, NULL, false);
                                    ble_audio_group_update_conn_num(p_audio_group);
                                    dev_info.dev_handle = p_dev;
                                    dev_info.cause = param->conn_state.disc_cause;
                                    ble_audio_group_send_msg(AUDIO_GROUP_MSG_DEV_DISCONN, p_audio_group,
                                                             (uint8_t *)&dev_info, sizeof(dev_info), false);
                                }
                            }
                        }
                    }
                    if (param->conn_state.state == GAP_CONN_STATE_CONNECTED)
                    {
                        p_dev = ble_audio_group_find_dev(p_audio_group, p_link->remote_bd, p_link->remote_bd_type);
                        if (p_dev)
                        {
                            T_AUDIO_GROUP_MSG_DEV_CONN dev_info;
                            p_dev->p_link = p_link;
                            ble_audio_group_update_conn_num(p_audio_group);
                            dev_info.dev_handle = p_dev;
                            ble_audio_group_send_msg(AUDIO_GROUP_MSG_DEV_CONN, p_audio_group,
                                                     (uint8_t *)&dev_info, sizeof(dev_info), false);
                        }
                    }
                }
            }
        }
        break;

    case BLE_DM_EVENT_BOND_MODIFY:
        {
            uint8_t i, j;
            T_LE_AUDIO_GROUP *p_audio_group;
            T_LE_AUDIO_DEV *p_dev;

            for (i = 0; i < audio_group_queue.count; i++)
            {
                p_audio_group = os_queue_peek(&audio_group_queue, i);
                if (param->bond_modify.type == BT_BOND_ADD)
                {
                    for (j = 0; j < p_audio_group->dev_queue.count; j++)
                    {
                        p_dev = (T_LE_AUDIO_DEV *)os_queue_peek(&p_audio_group->dev_queue, j);

                        if (p_dev != NULL  && p_dev->is_used == true &&
                            p_dev->identity_addr_type == 0xff)
                        {
                            ble_audio_group_update_identity_addr(p_dev);
                        }
                    }
                }
                else if (param->bond_modify.type == BT_BOND_DELETE)
                {
                    p_dev = ble_audio_group_find_dev(p_audio_group, param->bond_modify.remote_addr,
                                                     param->bond_modify.remote_bd_type);
                    if (p_dev != NULL)
                    {
                        T_AUDIO_GROUP_MSG_DEV_BOND_REMOVE dev_info;
                        dev_info.dev_handle = p_dev;
                        ble_audio_group_send_msg(AUDIO_GROUP_MSG_DEV_BOND_REMOVE, p_audio_group,
                                                 (uint8_t *)&dev_info, sizeof(dev_info), false);
                    }
                }
                else if (param->bond_modify.type == BT_BOND_CLEAR)
                {
                    ble_audio_group_send_msg(AUDIO_GROUP_MSG_DEV_BOND_CLEAR, p_audio_group,
                                             NULL, 0, false);
                }
            }
        }
        break;
    default:
        break;
    }
}


void ble_audio_group_init(void)
{
    os_queue_init(&audio_group_queue);
    ble_dm_cback_register(ble_audio_group_dm_cback);
    ble_audio_handle_register(BLE_AUDIO_GROUP, ble_audio_group_hdl_event);
}

#if LE_AUDIO_DEINIT
void ble_audio_group_deinit(void)
{
    T_LE_AUDIO_GROUP *p_audio_group;
    T_LE_AUDIO_DEV *p_dev;

    while ((p_audio_group = os_queue_out(&audio_group_queue)) != NULL)
    {
        while ((p_dev = os_queue_out(&p_audio_group->dev_queue)) != NULL)
        {
            ble_audio_mem_free(p_dev);
        }
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
        if (p_audio_group->p_csip_cb)
        {
            ble_audio_mem_free(p_audio_group->p_csip_cb);
        }
#endif
#if LE_AUDIO_CAP_SUPPORT
        if (p_audio_group->p_cap)
        {
            ble_audio_mem_free(p_audio_group->p_cap);
        }
#endif
        ble_audio_mem_free(p_audio_group);
    }
}
#endif
#endif

