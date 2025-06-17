/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     ots_coc.c
* @brief    Object transfer service source file.
* @details  Interfaces to access Media Control service.
* @author   cheng_cai
* @date
* @version  v1.0
*********************************************************************************************************
*/
#include "ots_coc.h"
#include "ots_comm.h"
#include "trace.h"
#include "ble_audio_mgr.h"

#if (LE_AUDIO_OTS_CLIENT_SUPPORT|LE_AUDIO_OTS_SERV_SUPPORT)
static T_COC_CB *p_coc_cb = NULL;

static T_COC_LINK_CB *find_free_coc_by_conn_id(uint8_t conn_id)
{
    int i;
    for (i = 0; i < p_coc_cb->coc_queue.count; i++)
    {
        T_COC_LINK_CB *coc_cb = (T_COC_LINK_CB *)os_queue_peek(&p_coc_cb->coc_queue, i);
        if (coc_cb->conn_id == conn_id &&
            coc_cb->app_callback == NULL)
        {
            return coc_cb;
        }
    }
    return NULL;
}

static T_COC_LINK_CB *find_coc_by_ids(uint8_t conn_id, uint16_t cid)
{
    int i;
    for (i = 0; i < p_coc_cb->coc_queue.count; i++)
    {
        T_COC_LINK_CB *coc_cb = (T_COC_LINK_CB *)os_queue_peek(&p_coc_cb->coc_queue, i);
        if (coc_cb->conn_id == conn_id &&
            coc_cb->cid == cid)
        {
            return coc_cb;
        }
    }
    return NULL;
}

static T_COC_LINK_CB *alloc_coc_cb_by_conn_id(uint8_t conn_id)
{
    T_COC_LINK_CB *coc_cb = (T_COC_LINK_CB *)ble_audio_mem_zalloc(sizeof(T_COC_LINK_CB));
    if (coc_cb == NULL)
    {
        PROTOCOL_PRINT_ERROR0("alloc_coc_cb_by_conn_id alloc fail");
        return NULL;
    }
    coc_cb->conn_id = conn_id;
    coc_cb->id = 0xFF;
    os_queue_in(&p_coc_cb->coc_queue, (void *)coc_cb);
    return coc_cb;
}

static void delete_coc_cb_by_id(uint16_t cid, uint8_t conn_id)
{
    T_COC_LINK_CB *coc_cb = find_coc_by_ids(conn_id, cid);

    if (coc_cb && coc_cb->cid == cid)
    {
        os_queue_delete(&p_coc_cb->coc_queue, (void *)coc_cb);
        ble_audio_mem_free((void *) coc_cb);
    }
}

static T_APP_RESULT ots_coc_callback(uint8_t coc_type, void *p_coc_data)
{
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_LE_COC_DATA *p_data = (T_LE_COC_DATA *)p_coc_data;
    PROTOCOL_PRINT_TRACE1("[OTS COC]ots_coc_callback: coc_type = %d", coc_type);
    T_COC_LINK_CB *coc_cb;
    switch (coc_type)
    {
    case GAP_COC_MSG_LE_CHANN_STATE:
        {
            PROTOCOL_PRINT_TRACE4("[OTS COC]ots_coc_callback: coc_type = %d, conn_state = %d, cause = 0x%x , cid = 0x%x",
                                  coc_type, p_data->p_le_chann_state->conn_state, p_data->p_le_chann_state->cause,
                                  p_data->p_le_chann_state->cid);
            if (p_data->p_le_chann_state->conn_state == GAP_CHANN_STATE_CONNECTING)
            {
                //IF not 0, remote start coc connect
                if (p_data->p_le_chann_state->cid != 0)
                {
                    coc_cb = find_coc_by_ids(p_data->p_le_chann_state->conn_id, p_data->p_le_chann_state->cid);
                    if (coc_cb == NULL)
                    {
                        coc_cb = alloc_coc_cb_by_conn_id(p_data->p_le_chann_state->conn_id);
                        //FIX TODO confirm it is remote connect, so the local is server
                        coc_cb->is_serv = true;
                        coc_cb->cid = p_data->p_le_chann_state->cid;
                    }
                }
            }
            else if (p_data->p_le_chann_state->conn_state == GAP_CHANN_STATE_CONNECTED)
            {
                coc_cb = find_coc_by_ids(p_data->p_le_chann_state->conn_id, p_data->p_le_chann_state->cid);

                if (coc_cb == NULL)
                {
                    //try to find it is client start coc connect
                    coc_cb = find_coc_by_ids(p_data->p_le_chann_state->conn_id, 0);
                }

                if (coc_cb != NULL)
                {
                    coc_cb->conn_id = p_data->p_le_chann_state->conn_id;
                    coc_cb->cid = p_data->p_le_chann_state->cid;
                    if (coc_cb->app_callback != NULL)
                    {
                        coc_cb->app_callback(coc_type, p_coc_data, coc_cb->id);
                    }
                }
            }
            else if (p_data->p_le_chann_state->conn_state == GAP_CHANN_STATE_DISCONNECTED)
            {
                coc_cb = find_coc_by_ids(p_data->p_le_chann_state->conn_id, p_data->p_le_chann_state->cid);
                //FIX TODO suppose it is client start coc connect, but failed
                if (coc_cb == NULL)
                {
                    coc_cb = find_coc_by_ids(p_data->p_le_chann_state->conn_id, 0);
                }
                if (coc_cb != NULL)
                {
                    if (coc_cb->app_callback != NULL)
                    {
                        coc_cb->app_callback(coc_type, p_coc_data, coc_cb->id);
                    }
                    delete_coc_cb_by_id(p_data->p_le_chann_state->cid, p_data->p_le_chann_state->conn_id);
                }
            }
        }
        break;

    case GAP_COC_MSG_LE_REG_PSM:
        break;

    case GAP_COC_MSG_LE_SET_PSM_SECURITY:
        break;

    case GAP_COC_MSG_LE_SEND_DATA:
        coc_cb = find_coc_by_ids(p_data->p_le_send_data->conn_id, p_data->p_le_send_data->cid);
        if (coc_cb != NULL && coc_cb->app_callback != NULL)
        {
            coc_cb->app_callback(coc_type, p_coc_data, coc_cb->id);
        }
        break;

    case GAP_COC_MSG_LE_RECEIVE_DATA:
        coc_cb = find_coc_by_ids(p_data->p_le_receive_data->conn_id, p_data->p_le_receive_data->cid);
        if (coc_cb != NULL && coc_cb->app_callback != NULL)
        {
            coc_cb->app_callback(coc_type, p_coc_data, coc_cb->id);
        }

        break;

    default:
        break;
    }
    return result;
}

bool le_coc_register_cback(uint8_t conn_id, P_FUN_LE_COC_OTS_CB app_callback, uint8_t id,
                           bool is_server)
{
    T_COC_LINK_CB *coc_cb = find_free_coc_by_conn_id(conn_id);
    if (coc_cb != NULL)
    {
        coc_cb->app_callback = app_callback;
        coc_cb->is_serv = is_server;
        coc_cb->id = id;
        T_LE_COC_DATA coc_data;
        T_LE_COC_CHANN_STATE chann_state;
        chann_state.conn_id = conn_id;
        chann_state.cid = coc_cb->cid;
        chann_state.conn_state = GAP_CHANN_STATE_CONNECTED;
        chann_state.cause = 0;
        coc_data.p_le_chann_state = &chann_state;
        app_callback(GAP_COC_MSG_LE_CHANN_STATE, (void *)&coc_data, id);
    }
    else
    {
        //The credit channel should be ready before server register
        if (is_server)
        {
            return false;
        }
        else
        {
            coc_cb = alloc_coc_cb_by_conn_id(conn_id);
            coc_cb->app_callback = app_callback;
            coc_cb->is_serv = is_server;
            coc_cb->id = id;
            if (le_coc_create(conn_id, OTS_PSM) != GAP_CAUSE_SUCCESS)
            {
                return false;
            }
        }
    }
    return true;
}

void le_coc_disconnect(int16_t cid)
{
    le_coc_disc(cid);
}

void le_coc_enable(void)
{
    if (p_coc_cb != NULL)
    {
        return;
    }

    uint8_t credit_num = OTS_COC_CREDIT_NUM;
    p_coc_cb = ble_audio_mem_zalloc(sizeof(T_COC_CB));
    le_coc_init(OTS_LE_COC_CHAN_MAX_NUM);
    le_coc_register_app_cb(ots_coc_callback);
    le_coc_set_param(COC_PARAM_CREDITS_THRESHOLD, 1, &credit_num);
    //le_coc_set_param(COC_PARAM_LOCAL_MTU, 2, &mtu_size);
    le_coc_reg_psm(OTS_PSM, true);
    os_queue_init(&p_coc_cb->coc_queue);
}
#if LE_AUDIO_DEINIT
void le_coc_deinit(void)
{
    if (p_coc_cb)
    {
        T_COC_LINK_CB *coc_cb;
        while ((coc_cb = os_queue_out(&p_coc_cb->coc_queue)) != NULL)
        {
            ble_audio_mem_free(coc_cb);
        }
        ble_audio_mem_free(p_coc_cb);
        p_coc_cb = NULL;
    }
}
#endif
#endif
