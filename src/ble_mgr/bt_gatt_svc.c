#if CONFIG_REALTEK_BT_GATT_SVC_SUPPORT
#include <string.h>
#include "trace.h"
#include "stdlib.h"
#include "bt_gatt_svc.h"
#include "os_queue.h"
#include "gap_conn_le.h"
#include "profile_server.h"
#include "ble_mgr_int.h"

#define BT_GATT_SVC_USE_NORMAL_SERVER      1
#define BT_GATT_SVC_USE_EXT_SERVER         1
#define BT_GATT_SVC_SEND_DATA_COMPLETE   0x0000
#define BT_GATT_SVC_SEND_DATA_PD         0x0001
#define BT_GATT_SVC_SEND_INDICATE        0x01
#define BT_GATT_SVC_SEND_NOTIFY          0x02

#define GATT_SVC_GATT_SERVICE_CHANGE_INDEX    2

extern T_SERVER_ID builtin_gatt_service_id;

typedef struct t_gatt_svc_ind_data_queue_elem
{
    struct t_gatt_svc_ind_data_queue_elem *p_next;
    T_SERVER_ID  service_id;
    uint16_t attrib_idx;
    uint16_t data_len;
    uint8_t *p_svc_data;
} T_GATT_SVC_IND_DATA_QUEUE_ELEM;

typedef struct t_gatt_svc_link_ind
{
    struct t_gatt_svc_link_ind *p_next;
    T_OS_QUEUE srv_data_q;
    uint16_t conn_handle;
    uint8_t  conn_id;
    T_SERVER_ID service_id;
    uint16_t attrib_idx;
} T_GATT_SVC_SEND_IND_DATA;

typedef struct t_gatt_svc_notify_data
{
    struct t_gatt_svc_notify_data *p_next;
    uint16_t conn_handle;
    uint16_t cid;
    uint8_t  conn_id;
    T_SERVER_ID service_id;
    uint16_t attrib_idx;
    uint16_t data_len;
    uint8_t *p_svc_data;
} T_GATT_SVC_NOTIFY_DATA;

uint16_t gatt_svc_mode = GATT_SVC_DISABLE;
uint8_t gatt_svc_reg_num = 0;
T_OS_QUEUE gatt_svc_link_ind_list;
T_OS_QUEUE gatt_svc_notify_list;

P_FUNC_GATT_SVC_GENERAL_CB gatt_svc_general_cb = NULL;

#if BT_GATT_SVC_USE_EXT_SERVER
typedef struct t_gatt_svc_ext_cb
{
    struct t_gatt_svc_ext_cb *p_next;
    T_SERVER_ID service_id;
    P_FUN_GATT_EXT_SEND_DATA_CB p_srv_ext_send_data_cb;
} T_GATT_SVC_EXT_CB;

T_OS_QUEUE gatt_svc_send_data_cb_list;

static T_GATT_SVC_EXT_CB *gatt_svc_ext_svc_find(T_SERVER_ID service_id)
{
    for (uint8_t i = 0; i < gatt_svc_send_data_cb_list.count; i++)
    {
        T_GATT_SVC_EXT_CB *p_cb = (T_GATT_SVC_EXT_CB *)os_queue_peek(&gatt_svc_send_data_cb_list, i);
        if (p_cb->service_id == service_id)
        {
            return p_cb;
        }
    }
    return NULL;
}
#endif

#if BT_GATT_SVC_USE_NORMAL_SERVER
typedef struct t_gatt_svc_cb
{
    struct t_gatt_svc_cb *p_next;
    T_SERVER_ID service_id;
    const T_FUN_GATT_EXT_SERVICE_CBS *p_srv_ext_cbs;
    P_FUN_EXT_WRITE_IND_POST_PROC ext_write_ind_post_proc;
    P_FUN_GATT_EXT_SEND_DATA_CB p_srv_ext_send_data_cb;
} T_GATT_SVC_CB;

T_OS_QUEUE  gatt_svc_list;

static T_GATT_SVC_CB *gatt_svc_find(T_SERVER_ID service_id)
{
    for (uint8_t i = 0; i < gatt_svc_list.count; i++)
    {
        T_GATT_SVC_CB *p_cb = (T_GATT_SVC_CB *)os_queue_peek(&gatt_svc_list, i);
        if (p_cb->service_id == service_id)
        {
            return p_cb;
        }
    }
    return NULL;
}

static T_APP_RESULT gatt_svc_read_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                     uint16_t attrib_index,
                                     uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_APP_ERR;
    T_GATT_SVC_CB *p_cb = gatt_svc_find(service_id);
    if (p_cb)
    {
        if (p_cb->p_srv_ext_cbs && p_cb->p_srv_ext_cbs->read_attr_cb)
        {
            cause = p_cb->p_srv_ext_cbs->read_attr_cb(le_get_conn_handle(conn_id), L2C_FIXED_CID_ATT,
                                                      service_id, attrib_index, offset, p_length, pp_value);
        }
    }
    return cause;
}

static void gatt_svc_write_post(uint8_t conn_id, T_SERVER_ID service_id,
                                uint16_t attrib_index, uint16_t length,
                                uint8_t *p_value)
{
    T_GATT_SVC_CB *p_cb = gatt_svc_find(service_id);
    if (p_cb)
    {
        if (p_cb->ext_write_ind_post_proc)
        {
            p_cb->ext_write_ind_post_proc(le_get_conn_handle(conn_id), L2C_FIXED_CID_ATT,
                                          service_id, attrib_index, length, p_value);
            p_cb->ext_write_ind_post_proc = NULL;
        }
    }
}

static T_APP_RESULT gatt_svc_write_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                      uint16_t attrib_index,
                                      T_WRITE_TYPE write_type,
                                      uint16_t length, uint8_t *p_value, P_FUN_WRITE_IND_POST_PROC *p_write_post_proc)
{
    T_APP_RESULT cause = APP_RESULT_APP_ERR;
    T_GATT_SVC_CB *p_cb = gatt_svc_find(service_id);
    if (p_cb)
    {
        if (p_cb->p_srv_ext_cbs && p_cb->p_srv_ext_cbs->write_attr_cb)
        {
            cause = p_cb->p_srv_ext_cbs->write_attr_cb(le_get_conn_handle(conn_id), L2C_FIXED_CID_ATT,
                                                       service_id, attrib_index, write_type, length, p_value, &p_cb->ext_write_ind_post_proc);
            if (p_cb->ext_write_ind_post_proc)
            {
                *p_write_post_proc = gatt_svc_write_post;
            }
        }
    }
    return cause;
}

static void gatt_svc_cccd_update_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                    uint16_t attrib_index,
                                    uint16_t ccc_bits)
{
    T_GATT_SVC_CB *p_cb = gatt_svc_find(service_id);
    if (p_cb)
    {
        if (p_cb->p_srv_ext_cbs && p_cb->p_srv_ext_cbs->cccd_update_cb)
        {
            p_cb->p_srv_ext_cbs->cccd_update_cb(le_get_conn_handle(conn_id), L2C_FIXED_CID_ATT,
                                                service_id, attrib_index, ccc_bits);
        }
    }
}

static const T_FUN_GATT_SERVICE_CBS gatt_svc_cbs =
{
    gatt_svc_read_cb,  // Read callback function pointer
    gatt_svc_write_cb, // Write callback function pointer
    gatt_svc_cccd_update_cb  // CCCD update callback function pointer
};
#endif

static void gatt_svc_send_data_cb(T_EXT_SEND_DATA_RESULT result)
{
    if (gatt_svc_mode == GATT_SVC_DISABLE)
    {
        return;
    }

#if BT_GATT_SVC_USE_NORMAL_SERVER
    if (gatt_svc_mode == GATT_SVC_USE_NORMAL_SERVER)
    {
        T_GATT_SVC_CB *p_cb = gatt_svc_find(result.service_id);
        if (p_cb)
        {
            if (p_cb->p_srv_ext_send_data_cb)
            {
                p_cb->p_srv_ext_send_data_cb(result);
            }
        }
    }
#endif
#if BT_GATT_SVC_USE_EXT_SERVER
    if (gatt_svc_mode == GATT_SVC_USE_EXT_SERVER)
    {
        T_GATT_SVC_EXT_CB *p_cb = gatt_svc_ext_svc_find(result.service_id);
        if (p_cb)
        {
            if (p_cb->p_srv_ext_send_data_cb)
            {
                p_cb->p_srv_ext_send_data_cb(result);
            }
        }
    }
#endif
}

uint8_t gatt_svc_get_num(void)
{
    return gatt_svc_reg_num;
}

bool gatt_svc_add(T_SERVER_ID *p_out_service_id, uint8_t *p_database, uint16_t length,
                  const T_FUN_GATT_EXT_SERVICE_CBS *p_srv_ext_cbs,
                  P_FUN_GATT_EXT_SEND_DATA_CB p_srv_ext_send_data_cb)
{
    if (gatt_svc_mode == GATT_SVC_DISABLE)
    {
        return false;
    }
#if BT_GATT_SVC_USE_EXT_SERVER
    if (gatt_svc_mode == GATT_SVC_USE_EXT_SERVER)
    {
        if (server_ext_add_service(p_out_service_id, p_database, length, p_srv_ext_cbs))
        {
            gatt_svc_reg_num++;
            if (p_srv_ext_send_data_cb != NULL)
            {
                T_GATT_SVC_EXT_CB *p_cb = calloc(1, sizeof(T_GATT_SVC_EXT_CB));
                if (p_cb)
                {
                    p_cb->service_id = *p_out_service_id;
                    p_cb->p_srv_ext_send_data_cb = p_srv_ext_send_data_cb;
                    os_queue_in(&gatt_svc_send_data_cb_list, p_cb);
                }
            }
            return true;
        }
    }
#endif
#if BT_GATT_SVC_USE_NORMAL_SERVER
    if (gatt_svc_mode == GATT_SVC_USE_NORMAL_SERVER)
    {
        if (server_add_service(p_out_service_id, p_database, length, gatt_svc_cbs))
        {
            gatt_svc_reg_num++;
            T_GATT_SVC_CB *p_cb = calloc(1, sizeof(T_GATT_SVC_CB));
            if (p_cb)
            {
                p_cb->service_id = *p_out_service_id;
                p_cb->p_srv_ext_cbs = p_srv_ext_cbs;
                p_cb->p_srv_ext_send_data_cb = p_srv_ext_send_data_cb;
                os_queue_in(&gatt_svc_list, p_cb);
            }
            return true;
        }
    }
#endif

    return false;
}

bool gatt_svc_read_confirm(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                           uint16_t attrib_index, uint8_t *p_data, uint16_t data_len,
                           uint16_t total_len, T_APP_RESULT cause)
{
    if (gatt_svc_mode == GATT_SVC_DISABLE)
    {
        return false;
    }
#if BT_GATT_SVC_USE_EXT_SERVER
    if (gatt_svc_mode == GATT_SVC_USE_EXT_SERVER)
    {
        return server_ext_attr_read_confirm(conn_handle, cid, service_id,
                                            attrib_index, p_data, data_len, total_len, cause);
    }
#endif
#if BT_GATT_SVC_USE_NORMAL_SERVER
    if (gatt_svc_mode == GATT_SVC_USE_NORMAL_SERVER)
    {
        uint8_t conn_id;
        if (le_get_conn_id_by_handle(conn_handle, &conn_id))
        {
            return server_attr_read_confirm(conn_id, service_id, attrib_index, p_data, data_len, cause);
        }
    }
#endif

    return false;
}

bool gatt_svc_write_confirm(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                            uint16_t attrib_index, T_APP_RESULT cause)
{
    if (gatt_svc_mode == GATT_SVC_DISABLE)
    {
        return false;
    }
#if BT_GATT_SVC_USE_EXT_SERVER
    if (gatt_svc_mode == GATT_SVC_USE_EXT_SERVER)
    {
        return server_ext_attr_write_confirm(conn_handle, cid, service_id, attrib_index, cause);
    }
#endif
#if BT_GATT_SVC_USE_NORMAL_SERVER
    if (gatt_svc_mode == GATT_SVC_USE_NORMAL_SERVER)
    {
        uint8_t conn_id;
        if (le_get_conn_id_by_handle(conn_handle, &conn_id))
        {
            return server_attr_write_confirm(conn_id, service_id, attrib_index, cause);
        }
    }
#endif

    return false;
}

#define gatt_svc_check_link_ind(conn_handle) gatt_svc_check_link_ind_int(__func__, conn_handle)

T_GATT_SVC_SEND_IND_DATA *gatt_svc_check_link_ind_int(const char *p_func_name, uint16_t conn_handle)
{
    for (uint8_t i = 0; i < gatt_svc_link_ind_list.count; i++)
    {
        T_GATT_SVC_SEND_IND_DATA *p_cb = (T_GATT_SVC_SEND_IND_DATA *)os_queue_peek(&gatt_svc_link_ind_list,
                                                                                   i);
        if (p_cb->conn_handle == conn_handle)
        {
            return p_cb;
        }
    }
    return NULL;
}

bool gatt_svc_send_notify_ind_data(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                   uint16_t attrib_index,
                                   uint8_t *p_data, uint16_t data_len, T_GATT_PDU_TYPE type)
{
#if BT_GATT_SVC_USE_NORMAL_SERVER
    uint8_t conn_id;

    if (gatt_svc_mode == GATT_SVC_USE_NORMAL_SERVER)
    {
        if (!le_get_conn_id_by_handle(conn_handle, &conn_id))
        {
            BTM_PRINT_ERROR1("gatt_svc_send_notify_ind_data: conn_handle 0x%x not found link", conn_handle);
            return false;
        }
    }

    if (gatt_svc_mode == GATT_SVC_USE_NORMAL_SERVER)
    {
        return server_send_data(conn_id, service_id, attrib_index, p_data, data_len, type);
    }
#endif
#if BT_GATT_SVC_USE_EXT_SERVER
    if (gatt_svc_mode == GATT_SVC_USE_EXT_SERVER)
    {
        return server_ext_send_data(conn_handle, cid, service_id, attrib_index, p_data, data_len, type);
    }
#endif

    return true;
}

bool gatt_svc_handle_ind_queue(uint16_t conn_handle, uint16_t cid,
                               T_SERVER_ID service_id, uint16_t attrib_idx)
{
    T_GATT_SVC_SEND_IND_DATA *p_svc_send_data = NULL;
    T_GATT_SVC_IND_DATA_QUEUE_ELEM *p_svc_data_elem = NULL;


    p_svc_send_data = gatt_svc_check_link_ind(conn_handle);

    if (p_svc_send_data)
    {
        if ((p_svc_send_data->service_id == service_id &&
             p_svc_send_data->attrib_idx == attrib_idx) ||
            (p_svc_send_data->srv_data_q.flags == BT_GATT_SVC_SEND_DATA_COMPLETE &&
             p_svc_send_data->service_id == 0xff))
        {
            p_svc_send_data->srv_data_q.flags = BT_GATT_SVC_SEND_DATA_COMPLETE;
            p_svc_send_data->service_id = 0xff;
            p_svc_send_data->attrib_idx = 0;

            if (p_svc_send_data->srv_data_q.p_first != NULL)
            {
                p_svc_data_elem = os_queue_peek(&p_svc_send_data->srv_data_q, 0);
                uint8_t credit = 0;
                le_get_gap_param(GAP_PARAM_LE_REMAIN_CREDITS, &credit);

                if (credit != 0 && p_svc_data_elem)
                {
                    bool result = false;
                    result = gatt_svc_send_notify_ind_data(conn_handle, cid, p_svc_data_elem->service_id,
                                                           p_svc_data_elem->attrib_idx, p_svc_data_elem->p_svc_data,
                                                           p_svc_data_elem->data_len, GATT_PDU_TYPE_INDICATION);
                    if (result)
                    {
                        p_svc_send_data->srv_data_q.flags = BT_GATT_SVC_SEND_DATA_PD;
                        p_svc_send_data->service_id = p_svc_data_elem->service_id;
                        p_svc_send_data->attrib_idx = p_svc_data_elem->attrib_idx;

                        os_queue_delete(&p_svc_send_data->srv_data_q, p_svc_data_elem);
                        if (p_svc_data_elem->p_svc_data)
                        {
                            free(p_svc_data_elem->p_svc_data);
                        }

                        free(p_svc_data_elem);
                    }

                    BTM_PRINT_TRACE1("gatt_svc_handle_ind_queue: result %d", result);
                    return result;
                }
            }
        }
        return true;
    }

    BTM_PRINT_ERROR0("gatt_svc_handle_ind_queue: p_svc_send_data is NULL");
    return false;
}

bool gatt_svc_handle_notify_queue(void)
{
    T_GATT_SVC_NOTIFY_DATA *p_notify_data = NULL;
    uint8_t credit = 0;
    le_get_gap_param(GAP_PARAM_LE_REMAIN_CREDITS, &credit);
    bool result = true;

    while (credit != 0 && gatt_svc_notify_list.count != 0)
    {
        p_notify_data = os_queue_peek(&gatt_svc_notify_list, 0);

        result = gatt_svc_send_notify_ind_data(p_notify_data->conn_handle, p_notify_data->cid,
                                               p_notify_data->service_id, p_notify_data->attrib_idx,
                                               p_notify_data->p_svc_data, p_notify_data->data_len,
                                               GATT_PDU_TYPE_NOTIFICATION);
        if (result)
        {
            credit--;
            os_queue_delete(&gatt_svc_notify_list, p_notify_data);
            if (p_notify_data->p_svc_data)
            {
                free(p_notify_data->p_svc_data);
            }

            free(p_notify_data);
        }
        else
        {
            BTM_PRINT_ERROR2("gatt_svc_handle_notify_queue: conn_handle 0x%x, result %d",
                             p_notify_data->conn_handle, result);
            return result;
        }
    }

    return result;
}

bool gatt_svc_handle_profile_data_cmpl(uint16_t conn_handle, uint16_t cid,
                                       T_SERVER_ID service_id, uint16_t attrib_idx,
                                       uint16_t credits, uint16_t cause)
{
    T_EXT_SEND_DATA_RESULT send_data_result = {0};

    if (gatt_svc_mode == GATT_SVC_DISABLE)
    {
        return true;
    }

    send_data_result.conn_handle = conn_handle;
    send_data_result.cid         = cid;
    send_data_result.service_id  = service_id;
    send_data_result.attrib_idx  = attrib_idx;
    send_data_result.credits     = credits;
    send_data_result.cause       = cause;

    gatt_svc_send_data_cb(send_data_result);

    bool result_ind = true;
    bool result_not = true;

    result_ind = gatt_svc_handle_ind_queue(conn_handle, cid, service_id, attrib_idx);

    result_not = gatt_svc_handle_notify_queue();

    if (!(result_ind && result_not))
    {
        BTM_PRINT_ERROR2("gatt_svc_handle_profile_data_cmpl: failed, result_ind %d, result_not %d",
                         result_ind, result_not);
    }

    return (result_ind && result_not);

}

static void gatt_svc_clear_svc_data_queue(uint16_t conn_handle)
{
    T_GATT_SVC_SEND_IND_DATA *p_svc_send_data = gatt_svc_check_link_ind(conn_handle);
    T_GATT_SVC_IND_DATA_QUEUE_ELEM *p_svc_data_elem = NULL;

    BTM_PRINT_INFO1("gatt_svc_clear_svc_data_queue: conn_handle 0x%x", conn_handle);

    if (p_svc_send_data)
    {
        while ((p_svc_data_elem = os_queue_out(&p_svc_send_data->srv_data_q)) != NULL)
        {
            if (p_svc_data_elem->p_svc_data)
            {
                free(p_svc_data_elem->p_svc_data);
            }

            free(p_svc_data_elem);
        }

        os_queue_delete(&gatt_svc_link_ind_list, p_svc_send_data);
        free(p_svc_send_data);
    }

    T_GATT_SVC_NOTIFY_DATA *p_notify_data = NULL;
    uint8_t i = 0;
    while ((p_notify_data = os_queue_peek(&gatt_svc_notify_list, i)) != NULL)
    {
        if (p_notify_data->conn_handle == conn_handle)
        {
            if (p_notify_data->p_svc_data)
            {
                free(p_notify_data->p_svc_data);
            }
            os_queue_delete(&gatt_svc_notify_list, p_notify_data);
            free(p_notify_data);
        }
        else
        {
            i++;
        }
    }
}

static bool gatt_svc_check_ind_prop(uint16_t conn_handle, T_SERVER_ID service_id,
                                    uint16_t attrib_idx, T_GATT_PDU_TYPE type)
{
    bool ret = false;
    bool result = false;
    uint16_t ccc_bits = GATT_CLIENT_CHAR_CONFIG_DEFAULT;

#if BT_GATT_SVC_USE_NORMAL_SERVER
    if (gatt_svc_mode == GATT_SVC_USE_NORMAL_SERVER)
    {
        uint8_t conn_id;
        le_get_conn_id_by_handle(conn_handle, &conn_id);
        result = server_get_cccd_info(conn_id, service_id, attrib_idx + 1, &ccc_bits);
    }
#endif
#if BT_GATT_SVC_USE_EXT_SERVER
    if (gatt_svc_mode == GATT_SVC_USE_EXT_SERVER)
    {
        result = server_ext_get_cccd_info(conn_handle, service_id, attrib_idx + 1, &ccc_bits);
    }
#endif

    if (result)
    {
        if (ccc_bits == GATT_CLIENT_CHAR_CONFIG_NOTIFY_INDICATE)
        {
            if (type == GATT_PDU_TYPE_INDICATION)
            {
                ret = true;
            }
        }
        else if (ccc_bits == GATT_CLIENT_CHAR_CONFIG_INDICATE)
        {
            if (type != GATT_PDU_TYPE_NOTIFICATION)
            {
                ret = true;
            }
        }
    }

    BTM_PRINT_TRACE4("gatt_svc_check_ind_prop: conn_handle 0x%x, service_id %d, attrib_idx %d, ret %d",
                     conn_handle, service_id, attrib_idx, ret);
    return ret;
}

bool gatt_svc_save_data_in_queue(uint16_t conn_handle, uint16_t cid, uint8_t conn_id,
                                 T_SERVER_ID service_id, uint16_t attrib_idx,
                                 uint8_t *p_data, uint16_t data_len, uint8_t flag)
{
    if (flag == BT_GATT_SVC_SEND_NOTIFY)
    {
        T_GATT_SVC_NOTIFY_DATA *p_notify_data = calloc(1, sizeof(T_GATT_SVC_NOTIFY_DATA));
        if (p_notify_data)
        {
            p_notify_data->conn_handle = conn_handle;
            p_notify_data->conn_id = conn_id;
            p_notify_data->cid = cid;
            p_notify_data->service_id = service_id;
            p_notify_data->attrib_idx = attrib_idx;
            if (p_data && (data_len != 0))
            {
                uint8_t *p_svc_data = calloc(1, data_len);

                if (p_svc_data)
                {
                    memcpy(p_svc_data, p_data, data_len);
                    p_notify_data->p_svc_data = p_svc_data;
                    p_notify_data->data_len = data_len;
                }
                else
                {
                    free(p_notify_data);
                    goto Failed;
                }
            }
            else
            {
                p_notify_data->p_svc_data = NULL;
                p_notify_data->data_len = 0;
            }

            os_queue_in(&gatt_svc_notify_list, p_notify_data);
            return true;
        }
    }
    else if (flag == BT_GATT_SVC_SEND_INDICATE)
    {
        T_GATT_SVC_SEND_IND_DATA *p_svc_send_data = gatt_svc_check_link_ind(conn_handle);

        if (!p_svc_send_data)
        {
            goto Failed;
        }

        T_GATT_SVC_IND_DATA_QUEUE_ELEM *p_svc_data_elem = calloc(1, sizeof(T_GATT_SVC_IND_DATA_QUEUE_ELEM));
        if (p_svc_data_elem != NULL)
        {
            p_svc_data_elem->service_id = service_id;
            p_svc_data_elem->attrib_idx = attrib_idx;
            if ((p_data != NULL) && (data_len != 0))
            {
                uint8_t *p_svc_data = calloc(1, data_len);

                if (p_svc_data != NULL)
                {
                    memcpy(p_svc_data, p_data, data_len);
                    p_svc_data_elem->p_svc_data = p_svc_data;
                    p_svc_data_elem->data_len = data_len;
                }
                else
                {
                    free(p_svc_data_elem);
                    goto Failed;
                }
            }
            else
            {
                p_svc_data_elem->p_svc_data = NULL;
                p_svc_data_elem->data_len = 0;
            }

            os_queue_in(&p_svc_send_data->srv_data_q, p_svc_data_elem);
            return true;
        }
    }

Failed:
    BTM_PRINT_ERROR4("gatt_svc_save_data_in_queue: conn_handle 0x%x, flag %d, service_id %d, attrib_idx %d",
                     conn_handle, flag, service_id, attrib_idx);
    return false;
}

bool gatt_svc_send_data(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                        uint16_t attrib_index,
                        uint8_t *p_data, uint16_t data_len, T_GATT_PDU_TYPE type)
{
    uint8_t conn_id = 0xff;
    T_GAP_CHANN_INFO chann_info;

    if (gatt_svc_mode == GATT_SVC_DISABLE)
    {
        return false;
    }

    if (gap_chann_get_info(conn_handle, cid, &chann_info))
    {
        if ((chann_info.chann_type == GAP_CHANN_TYPE_LE_ATT) ||
            (chann_info.chann_type == GAP_CHANN_TYPE_LE_ECFC))
        {
            if (le_get_conn_id_by_handle(conn_handle, &conn_id) == false)
            {
                BTM_PRINT_ERROR1("gatt_svc_send_data: server_send_data failed, conn_handle 0x%x, invalid state",
                                 conn_handle);
                return false;
            }
        }
    }
    else
    {
        BTM_PRINT_ERROR1("gatt_svc_send_data: server_send_data failed, conn_handle 0x%x, invalid state 2",
                         conn_handle);
        return false;
    }

    T_GATT_SVC_SEND_IND_DATA *p_svc_send_data = gatt_svc_check_link_ind(conn_handle);

    if (p_svc_send_data == NULL)
    {
        p_svc_send_data = calloc(1, sizeof(T_GATT_SVC_SEND_IND_DATA));

        if (p_svc_send_data != NULL)
        {
            os_queue_init(&p_svc_send_data->srv_data_q);
            p_svc_send_data->service_id = 0xff;
            p_svc_send_data->attrib_idx = 0;
            p_svc_send_data->conn_handle = conn_handle;
            p_svc_send_data->conn_id = conn_id;
            p_svc_send_data->srv_data_q.flags = BT_GATT_SVC_SEND_DATA_COMPLETE;
            os_queue_in(&gatt_svc_link_ind_list, p_svc_send_data);
        }
        else
        {
            return false;
        }
    }

    uint8_t credit = 0;
    le_get_gap_param(GAP_PARAM_LE_REMAIN_CREDITS, &credit);

    if (gatt_svc_check_ind_prop(conn_handle, service_id, attrib_index, type))
    {
        if ((credit != 0) && (p_svc_send_data->srv_data_q.p_first == NULL) &&
            (p_svc_send_data->srv_data_q.flags == BT_GATT_SVC_SEND_DATA_COMPLETE))
        {
            bool result;
            result = gatt_svc_send_notify_ind_data(conn_handle, cid, service_id, attrib_index,
                                                   p_data, data_len, GATT_PDU_TYPE_INDICATION);
            if (result)
            {
                p_svc_send_data->srv_data_q.flags = BT_GATT_SVC_SEND_DATA_PD;
                p_svc_send_data->service_id = service_id;
                p_svc_send_data->attrib_idx = attrib_index;
            }
            else
            {
                BTM_PRINT_ERROR2("gatt_svc_send_data: server_send_data failed, conn_handle 0x%x, attrib_index %d",
                                 conn_handle, attrib_index);
                p_svc_send_data->srv_data_q.flags = BT_GATT_SVC_SEND_DATA_COMPLETE;
            }

            return result;
        }
        else
        {
            BTM_PRINT_TRACE3("gatt_svc_send_data: send ind in queue, conn_handle 0x%x, service_id %d, attrib_idx %d",
                             conn_handle, service_id, attrib_index);
            return gatt_svc_save_data_in_queue(conn_handle, cid, conn_id, service_id, attrib_index,
                                               p_data, data_len, BT_GATT_SVC_SEND_INDICATE);
        }
    }
    else
    {
        if (credit != 0 && gatt_svc_notify_list.count == 0)
        {
            return gatt_svc_send_notify_ind_data(conn_handle, cid, service_id, attrib_index,
                                                 p_data, data_len, GATT_PDU_TYPE_NOTIFICATION);
        }
        else
        {
            BTM_PRINT_TRACE3("gatt_svc_send_data: send notify in queue, conn_handle 0x%x, service_id %d, attrib_idx %d",
                             conn_handle, service_id, attrib_index);
            gatt_svc_save_data_in_queue(conn_handle, cid, conn_id, service_id, attrib_index,
                                        p_data, data_len, BT_GATT_SVC_SEND_NOTIFY);

            return gatt_svc_handle_notify_queue();
        }
    }
}

void gatt_svc_handle_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state)
{
    if (new_state == GAP_CONN_STATE_DISCONNECTED)
    {
        T_GATT_SVC_SEND_IND_DATA *p_svc_send_data = NULL;
        T_GATT_SVC_IND_DATA_QUEUE_ELEM *p_svc_data_elem = NULL;

        for (uint8_t i = 0; i < gatt_svc_link_ind_list.count; i++)
        {
            T_GATT_SVC_SEND_IND_DATA *p_cb = (T_GATT_SVC_SEND_IND_DATA *)os_queue_peek(&gatt_svc_link_ind_list,
                                                                                       i);
            if (p_cb->conn_id == conn_id)
            {
                p_svc_send_data = p_cb;
                break;
            }
        }

        BTM_PRINT_INFO1("gatt_svc_handle_conn_state_evt: disconnected, conn_id 0x%x", conn_id);

        if (p_svc_send_data)
        {
            while ((p_svc_data_elem = os_queue_out(&p_svc_send_data->srv_data_q)) != NULL)
            {
                if (p_svc_data_elem->p_svc_data)
                {
                    free(p_svc_data_elem->p_svc_data);
                }

                free(p_svc_data_elem);
            }

            os_queue_delete(&gatt_svc_link_ind_list, p_svc_send_data);
            free(p_svc_send_data);
        }

        T_GATT_SVC_NOTIFY_DATA *p_notify_data = NULL;
        uint8_t i = 0;
        while ((p_notify_data = os_queue_peek(&gatt_svc_notify_list, i)) != NULL)
        {
            if (p_notify_data->conn_id == conn_id)
            {
                if (p_notify_data->p_svc_data)
                {
                    free(p_notify_data->p_svc_data);
                }
                os_queue_delete(&gatt_svc_notify_list, p_notify_data);
                free(p_notify_data);
            }
            else
            {
                i++;
            }
        }
    }
}

void gatt_svc_handle_gap_common_cb(uint8_t cb_type, void *p_cb_data)
{
    T_GAP_CB_DATA cb_data;
    memcpy(&cb_data, p_cb_data, sizeof(T_GAP_CB_DATA));

    if (cb_type == GAP_MSG_GAP_CHANN_DEL)
    {
        if (gap_chann_get_num(cb_data.p_gap_chann_del->conn_handle) == 0 &&
            cb_data.p_gap_chann_del->cid != L2C_FIXED_CID_ATT)
        {
            BTM_PRINT_INFO2("gatt_svc_handle_gap_common_cb: GAP_MSG_GAP_CHANN_DEL, conn_handle 0x%x, cid 0x%x",
                            cb_data.p_gap_chann_del->conn_handle,
                            cb_data.p_gap_chann_del->cid);
            gatt_svc_clear_svc_data_queue(cb_data.p_gap_chann_del->conn_handle);
        }
    }
}

void gatt_svc_send_reg_result(T_SERVER_ID service_id, uint8_t type, void *p_data)
{
    if (type == PROFILE_EVT_SRV_REG_COMPLETE)
    {
        T_SERVER_RESULT *p_result = (T_SERVER_RESULT *)p_data;
        T_GATT_SVC_REG_RESULT reg_result;
        memset(&reg_result, 0, sizeof(T_GATT_SVC_REG_RESULT));

        reg_result.result = (T_GATT_SVC_RESULT) * p_result;
        if (gatt_svc_general_cb)
        {
            (*gatt_svc_general_cb)(GATT_SVC_EVENT_REG_RESULT, (void *)&reg_result);
        }
    }
    else if (type == PROFILE_EVT_SRV_REG_AFTER_INIT_COMPLETE)
    {
        T_SERVER_REG_AFTER_INIT_RESULT *p_result = (T_SERVER_REG_AFTER_INIT_RESULT *)p_data;
        T_GATT_SVC_REG_AFTER_INIT_RESULT reg_result;
        memset(&reg_result, 0, sizeof(T_GATT_SVC_REG_AFTER_INIT_RESULT));

        reg_result.cause = p_result->cause;
        reg_result.service_id = p_result->service_id;
        if (gatt_svc_general_cb)
        {
            (*gatt_svc_general_cb)(GATT_SVC_EVENT_REG_AFTER_INIT_RESULT, (void *)&reg_result);
        }
    }
}

#if BT_GATT_SVC_USE_NORMAL_SERVER
T_APP_RESULT gatt_svc_profile_general_cb(T_SERVER_ID service_id, void *p_data)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;

    if (service_id == SERVICE_PROFILE_GENERAL_ID)
    {
        T_SERVER_APP_CB_DATA *p_param = (T_SERVER_APP_CB_DATA *)p_data;
        uint8_t conn_id = p_param->event_data.send_data_result.conn_id;

        switch (p_param->eventId)
        {
        case PROFILE_EVT_SRV_REG_COMPLETE:
            {
                BTM_PRINT_INFO1("gatt_svc_general_cb: PROFILE_EVT_SRV_REG_COMPLETE: result %d",
                                p_param->event_data.service_reg_result);
                gatt_svc_send_reg_result(service_id, PROFILE_EVT_SRV_REG_COMPLETE,
                                         &p_param->event_data.service_reg_result);
            }
            break;

        case PROFILE_EVT_SRV_REG_AFTER_INIT_COMPLETE:
            {
                BTM_PRINT_ERROR3("gatt_svc_general_cb: PROFILE_EVT_SRV_REG_AFTER_INIT_COMPLETE:service_id %d, result %d, cuase 0x%x",
                                 p_param->event_data.server_reg_after_init_result.service_id,
                                 p_param->event_data.server_reg_after_init_result.result,
                                 p_param->event_data.server_reg_after_init_result.cause);
                gatt_svc_send_reg_result(service_id, PROFILE_EVT_SRV_REG_AFTER_INIT_COMPLETE,
                                         &p_param->event_data.server_reg_after_init_result);
            }
            break;

        case PROFILE_EVT_SEND_DATA_COMPLETE:
            {
                if (p_param->event_data.send_data_result.cause == GAP_SUCCESS)
                {
                    BTM_PRINT_INFO0("gatt_svc_general_cb: PROFILE_EVT_SEND_DATA_COMPLETE success");
                }
                else
                {
                    BTM_PRINT_ERROR0("gatt_svc_general_cb: PROFILE_EVT_SEND_DATA_COMPLETE failed");
                }

                uint16_t conn_handle = le_get_conn_handle(conn_id);
                if (!gatt_svc_handle_profile_data_cmpl(conn_handle, L2C_FIXED_CID_ATT,
                                                       p_param->event_data.send_data_result.service_id,
                                                       p_param->event_data.send_data_result.attrib_idx,
                                                       p_param->event_data.send_data_result.credits,
                                                       p_param->event_data.send_data_result.cause))
                {
                    BTM_PRINT_ERROR0("gatt_svc_general_cb: gatt_svc_handle_profile_data_cmpl failed");
                }
            }
            break;

        default:
            break;
        }
    }

    return app_result;
}
#endif

#if BT_GATT_SVC_USE_EXT_SERVER
T_APP_RESULT gatt_svc_ext_profile_general_cb(T_SERVER_ID service_id, void *p_data)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;

    if (service_id == SERVICE_PROFILE_GENERAL_ID)
    {
        T_SERVER_EXT_APP_CB_DATA *p_param = (T_SERVER_EXT_APP_CB_DATA *)p_data;

        switch (p_param->eventId)
        {
        case PROFILE_EVT_SRV_REG_COMPLETE:
            {
                BTM_PRINT_INFO1("gatt_svc_ext_general_cb: PROFILE_EVT_SRV_REG_COMPLETE: result %d",
                                p_param->event_data.service_reg_result);
                gatt_svc_send_reg_result(service_id, PROFILE_EVT_SRV_REG_COMPLETE,
                                         &p_param->event_data.service_reg_result);
            }
            break;

        case PROFILE_EVT_SRV_REG_AFTER_INIT_COMPLETE:
            {
                BTM_PRINT_ERROR3("gatt_svc_ext_general_cb: PROFILE_EVT_SRV_REG_AFTER_INIT_COMPLETE:service_id %d, result %d, cuase 0x%x",
                                 p_param->event_data.server_reg_after_init_result.service_id,
                                 p_param->event_data.server_reg_after_init_result.result,
                                 p_param->event_data.server_reg_after_init_result.cause);
                gatt_svc_send_reg_result(service_id, PROFILE_EVT_SRV_REG_AFTER_INIT_COMPLETE,
                                         &p_param->event_data.server_reg_after_init_result);
            }
            break;

        case PROFILE_EVT_SEND_DATA_COMPLETE:
            {
                if (p_param->event_data.send_data_result.cause == GAP_SUCCESS)
                {
                    BTM_PRINT_INFO0("gatt_svc_ext_general_cb: PROFILE_EVT_SEND_DATA_COMPLETE success");
                }
                else
                {
                    BTM_PRINT_ERROR0("gatt_svc_ext_general_cb: PROFILE_EVT_SEND_DATA_COMPLETE failed");
                }

                if (!gatt_svc_handle_profile_data_cmpl(p_param->event_data.send_data_result.conn_handle,
                                                       p_param->event_data.send_data_result.cid,
                                                       p_param->event_data.send_data_result.service_id,
                                                       p_param->event_data.send_data_result.attrib_idx,
                                                       p_param->event_data.send_data_result.credits,
                                                       p_param->event_data.send_data_result.cause))
                {
                    BTM_PRINT_ERROR0("gatt_svc_ext_general_cb: gatt_svc_handle_profile_data_cmpl failed");
                }
            }
            break;

        default:
            break;
        }
    }

    return app_result;
}
#endif

bool gatt_svc_init(uint16_t mode, uint8_t svc_num)
{
    if (gatt_svc_mode == GATT_SVC_DISABLE)
    {
        if (mode == GATT_SVC_USE_NORMAL_SERVER)
        {
#if BT_GATT_SVC_USE_NORMAL_SERVER
            gatt_svc_mode = GATT_SVC_USE_NORMAL_SERVER;
            if (svc_num != 0)
            {
                server_register_app_cb(gatt_svc_profile_general_cb);
                server_init(svc_num);
            }
#else
            gatt_svc_mode = GATT_SVC_DISABLE;
            BTM_PRINT_ERROR0("gatt_svc_init: GATT_SVC_USE_NORMAL_SERVER not support");
            return false;
#endif
        }
        else if (mode == GATT_SVC_USE_EXT_SERVER)
        {
#if BT_GATT_SVC_USE_EXT_SERVER
            gatt_svc_mode = GATT_SVC_USE_EXT_SERVER;
            if (svc_num != 0)
            {
                server_cfg_use_ext_api(true);
                server_ext_register_app_cb(gatt_svc_ext_profile_general_cb);
                server_init(svc_num);
            }
#else
            gatt_svc_mode = GATT_SVC_DISABLE;
            BTM_PRINT_ERROR0("gatt_svc_init: GATT_SVC_USE_EXT_SERVER not support");
            return false;
#endif
        }

        return true;
    }
    else
    {
        BTM_PRINT_ERROR0("gatt_svc_init: gatt svc already init");
        return false;
    }
}

void gatt_svc_register_general_cb(P_FUNC_GATT_SVC_GENERAL_CB svc_cb)
{
    gatt_svc_general_cb = svc_cb;
}

bool gatt_svc_service_changed_indicate(uint16_t conn_handle, uint16_t cid, uint16_t start_handle,
                                       uint16_t end_handle)
{
    uint8_t data[4];
    LE_UINT16_TO_ARRAY(data, start_handle);
    LE_UINT16_TO_ARRAY(&data[2], end_handle);
    return gatt_svc_send_data(conn_handle, cid, builtin_gatt_service_id,
                              GATT_SVC_GATT_SERVICE_CHANGE_INDEX, data,
                              4, GATT_PDU_TYPE_ANY);
}

T_CHAR_UUID gatt_svc_find_char_uuid_by_index(const T_ATTRIB_APPL *p_srv, uint16_t index,
                                             uint16_t attr_num)
{
    T_CHAR_UUID char_uuid;
    memset(&char_uuid, 0, sizeof(T_CHAR_UUID));
    if (p_srv == NULL)
    {
        BTM_PRINT_ERROR0("gatt_svc_find_char_uuid_by_index: failed, service table is NULL");
        return char_uuid;
    }

    if (index >= attr_num)
    {
        BTM_PRINT_ERROR0("gatt_svc_find_char_uuid_by_index: failed, index error");
        return char_uuid;
    }

    //FIX TODO we suppose if UUID size is 128bit is service or charac and no 32 bit uuid
    if (p_srv[index].flags & ATTRIB_FLAG_UUID_128BIT)
    {
        memcpy(char_uuid.uu.char_uuid128, p_srv[index].type_value, UUID_128BIT_SIZE);
        char_uuid.uuid_size = UUID_128BIT_SIZE;
    }
    else
    {
        char_uuid.uu.char_uuid16 = (p_srv[index].type_value[0]) | (p_srv[index].type_value[1] << 8);
        char_uuid.uuid_size = UUID_16BIT_SIZE;
        //If it is cccd, we should find charac uuid
        if (char_uuid.uu.char_uuid16 == GATT_UUID_CHAR_CLIENT_CONFIG)
        {
            index--;
            while (index > 0)
            {
                char_uuid.uu.char_uuid16 = (p_srv[index].type_value[0]) | (p_srv[index].type_value[1] << 8);
                if (char_uuid.uu.char_uuid16 == GATT_UUID_CHARACTERISTIC)
                {
                    index++;
                    if (p_srv[index].flags & ATTRIB_FLAG_UUID_128BIT)
                    {
                        memcpy(char_uuid.uu.char_uuid128, p_srv[index].type_value, UUID_128BIT_SIZE);
                        char_uuid.uuid_size = UUID_128BIT_SIZE;
                    }
                    else
                    {
                        char_uuid.uu.char_uuid16 = (p_srv[index].type_value[0]) | (p_srv[index].type_value[1] << 8);
                    }
                    break;
                }
                index--;
            }
        }
    }

    char_uuid.index = index;
    return char_uuid;
}


/*********************************************************************
***  attr_num: the table total attr_num

***   char_uuid:
      char_uuid.index: the which num-th character to be found

***   return char index in the table
**********************************************************************/
bool gatt_svc_find_char_index_by_uuid(const T_ATTRIB_APPL *p_srv, T_CHAR_UUID char_uuid,
                                      uint16_t attr_num, uint16_t *index)
{
    uint16_t idx = 0;
    uint8_t char_num = 1;
    if (p_srv != NULL && index != NULL)
    {
        while (idx < attr_num)
        {
            if ((char_uuid.uuid_size == UUID_128BIT_SIZE && (p_srv[idx].flags & ATTRIB_FLAG_UUID_128BIT)) ||
                (char_uuid.uuid_size == UUID_16BIT_SIZE && !(p_srv[idx].flags & ATTRIB_FLAG_UUID_128BIT)))
            {
                //use char_uuid128 array to compare
                if (memcmp(char_uuid.uu.char_uuid128, p_srv[idx].type_value, char_uuid.uuid_size) == 0)
                {
                    if (char_uuid.index == char_num)
                    {
                        *index = idx;
                        return true;
                    }
                    char_num++;
                }
            }
            idx++;
        }
    }
    BTM_PRINT_ERROR0("gatt_svc_find_char_index_by_uuid: failed");
    return false;
}

/*********************************************************************
***  attr_num: the table total attr_num

***   char_uuid:
      char_uuid.index: the which num-th character to be found

***   return char index in the table
**********************************************************************/
uint16_t gatt_svc_find_char_index_by_uuid16(const T_ATTRIB_APPL *p_srv,
                                            uint16_t char_uuid16, uint16_t attr_num)
{
    T_CHAR_UUID char_uuid;
    uint16_t attrib_idx = 0;
    char_uuid.index = 1;
    char_uuid.uuid_size = UUID_16BIT_SIZE;
    char_uuid.uu.char_uuid16 = char_uuid16;

    if (!gatt_svc_find_char_index_by_uuid(p_srv, char_uuid, attr_num, &attrib_idx))
    {
        return 0;
    }

    return attrib_idx;
}

#if BLE_MGR_DEINIT
void gatt_svc_deinit(void)
{
    gatt_svc_mode = GATT_SVC_DISABLE;
#if BT_GATT_SVC_USE_EXT_SERVER
    T_GATT_SVC_EXT_CB *p_item;
    while ((p_item = os_queue_out(&gatt_svc_send_data_cb_list)) != NULL)
    {
        free(p_item);
    }
#endif
#if BT_GATT_SVC_USE_NORMAL_SERVER
    T_GATT_SVC_CB *p_cb;
    while ((p_cb = os_queue_out(&gatt_svc_list)) != NULL)
    {
        free(p_cb);
    }
#endif
    T_GATT_SVC_SEND_IND_DATA *p_svc_send_data;
    while ((p_svc_send_data = os_queue_peek(&gatt_svc_link_ind_list)) != NULL)
    {
        gatt_svc_clear_svc_data_queue(p_svc_send_data->conn_handle);
    }
    gatt_svc_reg_num = 0;
    gatt_svc_general_cb = NULL;
}
#endif
#endif
