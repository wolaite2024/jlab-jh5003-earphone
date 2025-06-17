#include <string.h>
#include "trace.h"
#include "csis_mgr.h"
#include "csis.h"
#include "bt_types.h"
#include "set_member.h"
#include "sys_timer.h"
#include "ble_audio_dm.h"
#include "ble_audio_mgr.h"
#include "ble_link_util.h"
#include "csis_sir.h"
#include "ble_audio_ual.h"

#if LE_AUDIO_CSIS_SUPPORT

#define CSIS_SIRK_NOTIFY_CCCD_FLAG 0x0001
#define CSIS_SIZE_NOTIFY_CCCD_FLAG 0x0002
#define CSIS_LOCK_NOTIFY_CCCD_FLAG 0x0004

bool csis_send_lock_change_notify(uint16_t conn_handle, T_SET_MEM *p_entry)
{
    uint8_t i;
    uint16_t cccd_handle;
    T_CSIS_LOCK_STATE lock_state = {0};

    for (i = 0; i < ble_audio_db.acl_link_num; i++)
    {
        if (ble_audio_db.le_link[i].used == true &&
            ble_audio_db.le_link[i].conn_handle != conn_handle)
        {
            T_LE_SRV_CFG *p_srv = ble_srv_find_by_srv_id(ble_audio_db.le_link[i].conn_handle,
                                                         p_entry->service_id);
            if (p_srv != NULL && (p_srv->cccd_cfg[0] & CSIS_LOCK_NOTIFY_CCCD_FLAG))
            {
                gatt_svc_send_data(ble_audio_db.le_link[i].conn_handle, L2C_FIXED_CID_ATT, p_entry->service_id,
                                   p_entry->lock_char_idx,
                                   &p_entry->lock, 1, GATT_PDU_TYPE_NOTIFICATION);
            }
        }
    }
    cccd_handle = server_get_start_handle(p_entry->service_id);
    if (cccd_handle != 0)
    {
        cccd_handle += (p_entry->lock_char_idx + 1);
        ble_audio_ual_set_pending_cccd(cccd_handle);
    }

    lock_state.service_id = p_entry->service_id;
    lock_state.lock_state = p_entry->lock;
    if (lock_state.lock_state == CSIS_LOCKED)
    {
        memcpy(lock_state.lock_address, p_entry->lock_address, GAP_BD_ADDR_LEN);
        lock_state.lock_address_type = p_entry->lock_address_type;
    }

    ble_audio_mgr_dispatch(LE_AUDIO_MSG_CSIS_LOCK_STATE, &lock_state);
    return true;
}

bool csis_get_sirk_char_data(T_SET_MEM *p_entry, uint8_t *p_data, T_BLE_AUDIO_LINK *p_link)
{
    if (p_entry->sirk_type == CSIS_SIRK_PLN)
    {
        p_data[0] = CSIS_SIRK_PLN;
        memcpy(&p_data[1], p_entry->sirk, CSIS_SIRK_LEN);
        return true;
    }
    else if (p_entry->sirk_type == CSIS_SIRK_ENC)
    {
        if (p_link)
        {
            uint8_t K[16];
            p_data[0] = CSIS_SIRK_ENC;
            if (csis_server_generate_le_k(p_link->remote_bd, p_link->remote_bd_type, p_link->conn_handle, K))
            {
                csis_crypto_sef(K, p_entry->sirk, &p_data[1]);
                return true;
            }
        }
    }
    return false;
}

bool csis_send_sirk_change_notify(T_SET_MEM *p_entry)
{
    uint8_t i;
    uint16_t cccd_handle;
    uint8_t data[17];

    for (i = 0; i < ble_audio_db.acl_link_num; i++)
    {
        if (ble_audio_db.le_link[i].used == true)
        {
            T_LE_SRV_CFG *p_srv = ble_srv_find_by_srv_id(ble_audio_db.le_link[i].conn_handle,
                                                         p_entry->service_id);
            if (p_srv != NULL && (p_srv->cccd_cfg[0] & CSIS_SIRK_NOTIFY_CCCD_FLAG))
            {
                if (csis_get_sirk_char_data(p_entry, data, &ble_audio_db.le_link[i]))
                {
                    gatt_svc_send_data(ble_audio_db.le_link[i].conn_handle, L2C_FIXED_CID_ATT, p_entry->service_id,
                                       p_entry->sirk_char_idx,
                                       data, 17, GATT_PDU_TYPE_NOTIFICATION);
                }
            }
        }
    }
    cccd_handle = server_get_start_handle(p_entry->service_id);
    if (cccd_handle != 0)
    {
        cccd_handle += (p_entry->sirk_char_idx + 1);
        ble_audio_ual_set_pending_cccd(cccd_handle);
    }
    return true;
}

bool csis_send_size_change_notify(T_SET_MEM *p_entry)
{
    uint16_t cccd_handle;

    for (uint8_t i = 0; i < ble_audio_db.acl_link_num; i++)
    {
        if (ble_audio_db.le_link[i].used == true)
        {
            T_LE_SRV_CFG *p_srv = ble_srv_find_by_srv_id(ble_audio_db.le_link[i].conn_handle,
                                                         p_entry->service_id);
            if (p_srv != NULL && (p_srv->cccd_cfg[0] & CSIS_SIZE_NOTIFY_CCCD_FLAG))
            {
                gatt_svc_send_data(ble_audio_db.le_link[i].conn_handle, L2C_FIXED_CID_ATT, p_entry->service_id,
                                   p_entry->size_char_idx,
                                   &p_entry->size, 1, GATT_PDU_TYPE_NOTIFICATION);
            }
        }
    }
    cccd_handle = server_get_start_handle(p_entry->service_id);
    if (cccd_handle != 0)
    {
        cccd_handle += (p_entry->size_char_idx + 1);
        ble_audio_ual_set_pending_cccd(cccd_handle);
    }
    return true;
}

void csis_lock_tout_cback(T_SYS_TIMER_HANDLE handle)
{
    T_SET_MEM *p_entry;
    uint32_t service_id = sys_timer_id_get(handle);
    PROTOCOL_PRINT_TRACE1("csis_lock_tout_cback: service_id 0x%02x", service_id);

    p_entry = set_member_find_by_srv_id(service_id);

    if (p_entry != NULL)
    {
        if (p_entry->lock == CSIS_LOCKED)
        {
            p_entry->lock = CSIS_UNLOCKED;
            csis_send_lock_change_notify(0xff, p_entry);
        }
    }
}

bool csis_start_lock_timer(T_SET_MEM *p_entry)
{
    if (p_entry == NULL)
    {
        return false;
    }
    PROTOCOL_PRINT_TRACE1("csis_start_lock_timer: service_id %d", p_entry->service_id);
    if (p_entry->timer_handle == NULL)
    {
        p_entry->timer_handle = sys_timer_create("csis lock",
                                                 SYS_TIMER_TYPE_LOW_PRECISION,
                                                 p_entry->service_id,
                                                 CSIS_LOCK_DEFAULT_TIMEOUT * 1000,
                                                 false,
                                                 csis_lock_tout_cback);
        if (p_entry->timer_handle)
        {
            return sys_timer_start(p_entry->timer_handle);
        }
    }
    else
    {
        return sys_timer_restart(p_entry->timer_handle, CSIS_LOCK_DEFAULT_TIMEOUT * 1000);
    }
    return false;
}

bool csis_stop_lock_timer(T_SET_MEM *p_entry)
{
    if (p_entry == NULL)
    {
        return false;
    }
    PROTOCOL_PRINT_TRACE1("csis_stop_lock_timer: service_id %d", p_entry->service_id);
    if (p_entry->timer_handle == NULL)
    {
        return sys_timer_stop(p_entry->timer_handle);
    }
    return false;
}

bool csis_check_lock_address(uint16_t conn_handle, T_SET_MEM *p_mem)
{
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link != NULL)
    {
        if (ble_audio_ual_check_addr(p_link->remote_bd, p_link->remote_bd_type,
                                     p_mem->lock_address, p_mem->lock_address_type))
        {
            return true;
        }
    }
    return false;
}

void csis_read_sirk(uint16_t conn_handle, uint16_t cid, T_SET_MEM *p_entry)
{
    T_CSIS_READ_SIRK_IND read_sirk;
    uint8_t data[17];
    uint16_t cb_result = BLE_AUDIO_CB_RESULT_SUCCESS;

    read_sirk.conn_handle = conn_handle;
    read_sirk.sirk_type = p_entry->sirk_type;
    read_sirk.service_id = p_entry->service_id;;

    cb_result = ble_audio_mgr_dispatch(LE_AUDIO_MSG_CSIS_READ_SIRK_IND, &read_sirk);
    if (cb_result == BLE_AUDIO_CB_RESULT_SUCCESS)
    {
        T_BLE_AUDIO_LINK *p_link = NULL;
        if (p_entry->sirk_type == CSIS_SIRK_ENC)
        {
            p_link = ble_audio_link_find_by_conn_handle(conn_handle);
        }
        if (csis_get_sirk_char_data(p_entry, data, p_link) == false)
        {
            cb_result = BLE_AUDIO_CB_RESULT_APP_ERR;
        }
    }

    gatt_svc_read_confirm(conn_handle, cid, p_entry->service_id, p_entry->sirk_char_idx,
                          data, 17, 17, (T_APP_RESULT)cb_result);
}

bool csis_update_rand(uint8_t service_id, uint8_t rank)
{
    T_SET_MEM *p_set_mem = set_member_find_by_srv_id(service_id);
    if (p_set_mem == NULL || (p_set_mem->feature & SET_MEMBER_RANK_EXIST) == 0)
    {
        PROTOCOL_PRINT_ERROR1("[CSIS] csis_update_rand: failed, service_id %d",
                              service_id);
        return false;
    }
    p_set_mem->rank = rank;
    return true;
}

bool csis_update_sirk(uint8_t service_id, uint8_t *p_sirk, T_CSIS_SIRK_TYPE sirk_type)
{
    T_SET_MEM *p_set_mem = set_member_find_by_srv_id(service_id);
    if (p_set_mem == NULL || (p_set_mem->feature & SET_MEMBER_SIRK_NOTIFY_SUPPORT) == 0)
    {
        PROTOCOL_PRINT_ERROR1("[CSIS] csis_update_sirk: failed, service_id %d",
                              service_id);
        return false;
    }
    PROTOCOL_PRINT_INFO1("[CSIS] csis_update_sirk: service_id %d", service_id);
    memcpy(p_set_mem->sirk, p_sirk, CSIS_SIRK_LEN);
    p_set_mem->sirk_type = sirk_type;
    csis_send_sirk_change_notify(p_set_mem);
    return true;
}

bool csis_update_size(uint8_t service_id, uint8_t csis_size)
{
    T_SET_MEM *p_set_mem = set_member_find_by_srv_id(service_id);
    if (p_set_mem == NULL || (p_set_mem->feature & SET_MEMBER_SIZE_NOTIFY_SUPPORT) == 0)
    {
        PROTOCOL_PRINT_ERROR1("[CSIS] csis_update_size: failed, service_id %d",
                              service_id);
        return false;
    }
    if (p_set_mem->size == csis_size)
    {
        return true;
    }
    PROTOCOL_PRINT_INFO1("[CSIS] csis_update_size: service_id %d", service_id);
    p_set_mem->size = csis_size;
    csis_send_size_change_notify(p_set_mem);
    return true;
}

void csis_handle_pending_cccd(T_BLE_AUDIO_LINK *p_link)
{
    T_LE_SRV_CFG *p_srv;
    uint8_t i;

    for (i = 0; i < set_mem_num; i++)
    {
        T_SET_MEM *p_entry;

        if (p_set_mem[i].used == true)
        {
            p_entry = &p_set_mem[i];
            p_srv = ble_srv_find_by_srv_id(p_link->conn_handle, p_entry->service_id);
            if (p_srv != NULL && p_srv->cccd_pending[0] != 0)
            {
                if ((p_srv->cccd_cfg[0] & CSIS_LOCK_NOTIFY_CCCD_FLAG)
                    && (p_srv->cccd_pending[0] & CSIS_LOCK_NOTIFY_CCCD_FLAG))
                {
                    if (gatt_svc_send_data(p_link->conn_handle, L2C_FIXED_CID_ATT, p_entry->service_id,
                                           p_entry->lock_char_idx,
                                           &p_entry->lock, 1, GATT_PDU_TYPE_NOTIFICATION))
                    {
                        uint16_t handle;
                        handle = server_get_start_handle(p_entry->service_id);
                        if (handle != 0)
                        {
                            handle += (p_entry->lock_char_idx + 1);
                            if (ble_audio_ual_clear_pending_cccd(p_link->conn_handle, handle))
                            {
                                p_srv->cccd_pending[0] &= ~CSIS_LOCK_NOTIFY_CCCD_FLAG;
                            }
                        }
                    }
                }
                if ((p_srv->cccd_cfg[0] & CSIS_SIRK_NOTIFY_CCCD_FLAG)
                    && (p_srv->cccd_pending[0] & CSIS_SIRK_NOTIFY_CCCD_FLAG))
                {
                    uint8_t data[17];
                    if (csis_get_sirk_char_data(p_entry, data, p_link))
                    {
                        if (gatt_svc_send_data(p_link->conn_handle, L2C_FIXED_CID_ATT, p_entry->service_id,
                                               p_entry->sirk_char_idx,
                                               data, 17, GATT_PDU_TYPE_NOTIFICATION))
                        {
                            uint16_t handle;
                            handle = server_get_start_handle(p_entry->service_id);
                            if (handle != 0)
                            {
                                handle += (p_entry->sirk_char_idx + 1);
                                if (ble_audio_ual_clear_pending_cccd(p_link->conn_handle, handle))
                                {
                                    p_srv->cccd_pending[0] &= ~CSIS_SIRK_NOTIFY_CCCD_FLAG;
                                }
                            }
                        }
                    }
                }
                if ((p_srv->cccd_cfg[0] & CSIS_SIZE_NOTIFY_CCCD_FLAG)
                    && (p_srv->cccd_pending[0] & CSIS_SIZE_NOTIFY_CCCD_FLAG))
                {
                    if (gatt_svc_send_data(p_link->conn_handle, L2C_FIXED_CID_ATT, p_entry->service_id,
                                           p_entry->size_char_idx,
                                           &p_entry->size, 1, GATT_PDU_TYPE_NOTIFICATION))
                    {
                        uint16_t handle;
                        handle = server_get_start_handle(p_entry->service_id);
                        if (handle != 0)
                        {
                            handle += (p_entry->size_char_idx + 1);
                            if (ble_audio_ual_clear_pending_cccd(p_link->conn_handle, handle))
                            {
                                p_srv->cccd_pending[0] &= ~CSIS_SIZE_NOTIFY_CCCD_FLAG;
                            }
                        }
                    }
                }
            }

        }
    }
}

static void csis_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BLE_DM_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BLE_DM_EVENT_CONN_STATE:
        {
            T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_id(param->conn_state.conn_id);
            if (p_link != NULL)
            {
                if (param->conn_state.state == GAP_CONN_STATE_DISCONNECTED)
                {
                    uint8_t  i;
                    if (ble_audio_ual_check_le_bond(p_link->conn_handle))
                    {
                        return;
                    }
                    if (p_set_mem == NULL)
                    {
                        return;
                    }

                    for (i = 0; i < set_mem_num; i++)
                    {
                        if (p_set_mem[i].used
                            && (p_set_mem[i].timer_handle != NULL))
                        {
                            T_SET_MEM *p_mem = &p_set_mem[i];
                            if (p_link->remote_bd_type == p_mem->lock_address_type
                                && memcmp(p_link->remote_bd, p_mem->lock_address, GAP_BD_ADDR_LEN) == 0)
                            {
                                if (p_mem->lock == CSIS_LOCKED)
                                {
                                    p_mem->lock = CSIS_UNLOCKED;
                                    csis_stop_lock_timer(p_mem);
                                    csis_send_lock_change_notify(p_link->conn_handle, p_mem);
                                }
                            }
                        }
                    }

                }
            }
        }
        break;

    case BLE_DM_EVENT_AUTHEN_INFO:
        {
            if (param->authen_info.new_state == GAP_AUTHEN_STATE_COMPLETE &&
                param->authen_info.cause == GAP_SUCCESS)
            {
                T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_id(param->authen_info.conn_id);
                if (p_link == NULL)
                {
                    return;
                }
                if (ble_audio_ual_check_le_bond(p_link->conn_handle) == false)
                {
                    return;
                }

                if (p_set_mem == NULL)
                {
                    return;
                }
                csis_handle_pending_cccd(p_link);
            }
        }
        break;
    default:
        break;
    }
}

bool csis_set_locked(T_SERVER_ID service_id)
{
    T_SET_MEM *p_entry = set_member_find_by_srv_id(service_id);
    if (p_entry != NULL)
    {
        p_entry->lock = CSIS_LOCKED;
        p_entry->lock_address_type = 0xff;
        csis_start_lock_timer(p_entry);
        return true;
    }
    return false;
}

T_APP_RESULT csis_attr_read_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                               uint16_t attrib_index,
                               uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    T_SET_MEM *p_entry;

    p_entry = set_member_find_by_srv_id(service_id);
    if (p_entry == NULL)
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }
    *p_length = 0;
    PROTOCOL_PRINT_INFO2("csis_attr_read_cb: attrib_index %d, offset %x", attrib_index, offset);

    if (attrib_index == p_entry->sirk_char_idx)
    {
        csis_read_sirk(conn_handle, cid, p_entry);
        cause = APP_RESULT_PENDING;
    }
    else if (attrib_index == p_entry->size_char_idx)
    {
        *pp_value = &p_entry->size;
        *p_length = 1;
    }
    else if (attrib_index == p_entry->rank_char_idx)
    {
        *pp_value = &p_entry->rank;
        *p_length = 1;
    }
    else if (attrib_index == p_entry->lock_char_idx)
    {
        *pp_value = &p_entry->lock;
        *p_length = 1;
    }
    else
    {
        cause  = APP_RESULT_ATTR_NOT_FOUND;
    }

    return (cause);
}

T_APP_RESULT csis_attr_write_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                uint16_t attrib_index,
                                T_WRITE_TYPE write_type,
                                uint16_t length, uint8_t *p_value, P_FUN_EXT_WRITE_IND_POST_PROC *p_write_post_proc)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    T_SET_MEM *p_entry;

    p_entry = set_member_find_by_srv_id(service_id);
    if (p_entry == NULL)
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    PROTOCOL_PRINT_INFO3("csis_attr_write_cb: attrib_index %d, length %x, curr lock %d", attrib_index,
                         length, p_entry->lock);

    if (attrib_index == p_entry->lock_char_idx)
    {
        uint8_t lock_req;
        if (length != 1)
        {
            return APP_RESULT_INVALID_VALUE_SIZE;
        }
        lock_req = *p_value;
        if (lock_req == 0 || lock_req > CSIS_LOCKED)
        {
            PROTOCOL_PRINT_ERROR1("[CSIS] csis_attr_write_cb: invalid lock value, lock_req %d", lock_req);
            return (T_APP_RESULT)ATT_ERR_CSIS_INVALID_LOCK_VALUE;
        }
        if (p_entry->lock == CSIS_UNLOCKED)
        {
            if (lock_req == CSIS_UNLOCKED)
            {
                return cause;
            }
        }
        else
        {
            if (lock_req == CSIS_LOCKED)
            {
                T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
                if (p_link != NULL)
                {
                    if (ble_audio_ual_check_addr(p_entry->lock_address, p_entry->lock_address_type,
                                                 p_link->remote_bd, p_link->remote_bd_type))
                    {
                        return (T_APP_RESULT)ATT_ERR_CSIS_LOCK_ALREADY_GRANTED;
                    }

                }
                PROTOCOL_PRINT_ERROR0("[CSIS] csis_attr_write_cb: invalid lock denied");
                return (T_APP_RESULT)ATT_ERR_CSIS_LOCK_DENIED;
            }
        }

        if (lock_req == CSIS_UNLOCKED)
        {
            if (csis_check_lock_address(conn_handle, p_entry))
            {
                csis_stop_lock_timer(p_entry);
                p_entry->lock = CSIS_UNLOCKED;
                csis_send_lock_change_notify(conn_handle, p_entry);
            }
            else
            {
                PROTOCOL_PRINT_ERROR0("[CSIS] csis_attr_write_cb: release not allowed");
                cause = (T_APP_RESULT)ATT_ERR_CSIS_LOCK_RELEASE_NOT_ALLOWED;
            }
        }
        else if (lock_req == CSIS_LOCKED)
        {
            T_BLE_AUDIO_LINK *p_link = NULL;
            csis_start_lock_timer(p_entry);
            p_entry->lock = CSIS_LOCKED;
            p_link = ble_audio_link_find_by_conn_handle(conn_handle);
            if (p_link)
            {
                memcpy(p_entry->lock_address, p_link->remote_bd, 6);
                p_entry->lock_address_type = p_link->remote_bd_type;
                csis_send_lock_change_notify(conn_handle, p_entry);
            }
        }
    }
    else
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }
    return (cause);
}

void csis_cccd_update_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                         uint16_t attrib_index,
                         uint16_t ccc_bits)
{
    T_SET_MEM *p_entry;

    p_entry = set_member_find_by_srv_id(service_id);
    if (p_entry == NULL)
    {
        return;
    }
    T_LE_SRV_CFG *p_srv = ble_srv_alloc_by_srv_id(conn_handle, service_id);
    if (p_srv == NULL)
    {
        return;
    }
    PROTOCOL_PRINT_INFO3("csis_cccd_update_cb: service_id %d, attrib_index %d, ccc_bits 0x%x",
                         service_id, attrib_index, ccc_bits);
    attrib_index--;

    if (attrib_index == p_entry->sirk_char_idx)
    {
        ble_srv_set_cccd_flags(p_srv, ccc_bits, 0,
                               CSIS_SIRK_NOTIFY_CCCD_FLAG, true);
    }
    else if (attrib_index == p_entry->size_char_idx)
    {
        ble_srv_set_cccd_flags(p_srv, ccc_bits, 0,
                               CSIS_SIZE_NOTIFY_CCCD_FLAG, true);
    }
    else if (attrib_index == p_entry->lock_char_idx)
    {
        ble_srv_set_cccd_flags(p_srv, ccc_bits, 0,
                               CSIS_LOCK_NOTIFY_CCCD_FLAG, true);
    }
    return;
}

const T_FUN_GATT_EXT_SERVICE_CBS csis_cbs =
{
    csis_attr_read_cb,  // Read callback function pointer
    csis_attr_write_cb, // Write callback function pointer
    csis_cccd_update_cb  // CCCD update callback function pointer
};

T_SERVER_ID csis_reg_srv(uint8_t *sirk, T_CSIS_SIRK_TYPE sirk_type, uint8_t size, uint8_t rank,
                         uint8_t feature, T_ATTR_UUID *p_inc_srv_uuid)
{
    T_SET_MEM *p_entry;
    p_entry = set_member_find_by_uuid(p_inc_srv_uuid);
    if (p_entry)
    {
        return p_entry->service_id;
    }
    if (feature & SET_MEMBER_LOCK_EXIST)
    {
        feature |= SET_MEMBER_RANK_EXIST;
        if (rank == 0)
        {
            PROTOCOL_PRINT_ERROR0("csis_reg_srv: lock required but rank is 0");
        }
    }

    p_entry = set_member_allocate_entry(sirk, sirk_type, size, rank, feature, p_inc_srv_uuid);
    if (p_entry)
    {
        return csis_add_service(&csis_cbs, p_entry);
    }
    return 0xFF;
}

bool csis_get_srv_id(uint8_t *p_service_id, T_ATTR_UUID *p_inc_srv_uuid)
{
    T_SET_MEM *p_entry;
    p_entry = set_member_find_by_uuid(p_inc_srv_uuid);
    if (p_entry)
    {
        *p_service_id = p_entry->service_id;
        return true;
    }
    return false;
}

bool csis_get_srv_tbl(uint8_t service_id, T_ATTRIB_APPL **pp_srv_tbl)
{
    T_SET_MEM *p_entry;
    p_entry = set_member_find_by_srv_id(service_id);
    if (p_entry)
    {
        *pp_srv_tbl = p_entry->p_attr_tbl;
        return true;
    }
    return false;
}

bool csis_init(uint8_t set_mem_num)
{
#if LE_AUDIO_INIT_DEBUG
    PROTOCOL_PRINT_INFO0("csis_init");
#endif
    if (set_member_init(set_mem_num))
    {
        ble_dm_cback_register(csis_dm_cback);
        return true;
    }
    return false;
}

#if LE_AUDIO_DEINIT
void csis_deinit(void)
{
    if (p_set_mem)
    {
        for (uint8_t i = 0; i < set_mem_num; i++)
        {
            if (p_set_mem[i].timer_handle)
            {
                sys_timer_delete(p_set_mem[i].timer_handle);
            }
            if (p_set_mem[i].p_attr_tbl)
            {
                ble_audio_mem_free(p_set_mem[i].p_attr_tbl);
            }
        }
        set_mem_num = 0;
        ble_audio_mem_free(p_set_mem);
        p_set_mem = NULL;
    }
}
#endif
#endif
