#if BISTO_FEATURE_SUPPORT

#include <stdint.h>

#include "app_bisto_bt.h"
#include "app_bisto_ble.h"
#include "app_sdp.h"
#include "app_link_util.h"
#include "btm.h"
#include "bisto_api.h"
#include "bisto_bt_uuid.h"
#include "bt_spp.h"
#include "spp_stream.h"
#include "gap_br.h"
#include "trace.h"


static struct
{
    BISTO_BT *p_bt;
    uint8_t bd_addr[6];
} app_bisto_bt = {.p_bt = NULL};


static bool app_bisto_bt_check_server_chann(T_BT_EVENT event_type, T_BT_EVENT_PARAM *param)
{
    uint32_t server_chann = 0;

    switch (event_type)
    {
    case BT_EVENT_SPP_CONN_CMPL:
        server_chann = param->spp_conn_cmpl.local_server_chann;
        break;

    case BT_EVENT_SPP_CREDIT_RCVD:
        server_chann = param->spp_credit_rcvd.local_server_chann;
        break;

    case BT_EVENT_SPP_DATA_IND:
        server_chann = param->spp_data_ind.local_server_chann;
        break;

    case BT_EVENT_SPP_CONN_IND:
        server_chann = param->spp_conn_ind.local_server_chann;
        break;

    case BT_EVENT_SPP_DISCONN_CMPL:
        server_chann = param->spp_disconn_cmpl.local_server_chann;
        break;

    default:
        return false;
    }

    APP_PRINT_TRACE2("[bisto] app_bisto_bt_check_server_chann: event_type %x, server_chann %d",
                     event_type,
                     server_chann);

    if (server_chann == RFC_SPP_BISTO_CTRL_CHANN_NUM || server_chann == RFC_SPP_BISTO_AU_CHANN_NUM)
    {
        return true;
    }

    return false;
}


static void app_bisto_bt_spp_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    T_SPP_STREAM_EVENT_PARAM spp_stream_param;

    if (app_bisto_bt_check_server_chann(event_type, param) == false)
    {
        return;
    }

    switch (event_type)
    {
    case BT_EVENT_SPP_CONN_CMPL:
        spp_stream_param.server_chann = param->spp_conn_cmpl.local_server_chann;
        spp_stream_param.connect_param.remote_credits = param->spp_conn_cmpl.link_credit;
        spp_stream_param.connect_param.mtu = param->spp_conn_cmpl.frame_size;
        memcpy(spp_stream_param.connect_param.bd_addr, param->spp_conn_cmpl.bd_addr, 6);
        spp_stream_event_process(SPP_STREAM_CONNECT_EVENT, &spp_stream_param);
        break;

    case BT_EVENT_SPP_CREDIT_RCVD:
        spp_stream_param.server_chann = param->spp_credit_rcvd.local_server_chann;
        spp_stream_param.update_credit_param.remote_credits = param->spp_credit_rcvd.link_credit;
        memcpy(spp_stream_param.update_credit_param.bd_addr, param->spp_credit_rcvd.bd_addr, 6);
        spp_stream_event_process(SPP_STREAM_UPDATE_CREDIT_EVENT, &spp_stream_param);
        break;

    case BT_EVENT_SPP_DATA_IND:
        spp_stream_param.server_chann = param->spp_data_ind.local_server_chann;
        spp_stream_param.receive_data_param.data      = param->spp_data_ind.data;
        spp_stream_param.receive_data_param.length    = param->spp_data_ind.len;
        memcpy(spp_stream_param.receive_data_param.bd_addr, param->spp_data_ind.bd_addr, 6);
        spp_stream_event_process(SPP_STREAM_RECEIVE_DATA_EVENT, &spp_stream_param);

        bt_spp_credits_give(param->spp_data_ind.bd_addr, param->spp_data_ind.local_server_chann,
                            1);
        break;

    case BT_EVENT_SPP_CONN_IND:
        bisto_bt_spp_connect_ind(bisto_bt_get_instance(), param->spp_conn_ind.local_server_chann,
                                 param->spp_conn_ind.bd_addr);
        bt_spp_connect_cfm(param->spp_conn_ind.bd_addr, param->spp_conn_ind.local_server_chann, true,
                           param->spp_conn_ind.frame_size, 7);
        break;

    case BT_EVENT_SPP_DISCONN_CMPL:
        spp_stream_param.server_chann = param->spp_disconn_cmpl.local_server_chann;
        memcpy(spp_stream_param.disconnect_param.bd_addr, param->spp_disconn_cmpl.bd_addr, 6);
        spp_stream_param.disconnect_param.cause = param->spp_disconn_cmpl.cause;
        spp_stream_event_process(SPP_STREAM_DISCONNECT_EVENT, &spp_stream_param);
        break;

    default:
        break;
    }
}


static bool app_bisto_bt_spp_init(void)
{
    if (bt_spp_service_register((uint8_t *)bisto_control_service_class_uuid128,
                                RFC_SPP_BISTO_CTRL_CHANN_NUM) == false)
    {
        APP_PRINT_ERROR0("app_bisto_bt_spp_init: bisto control spp reg failed");
        return false;
    };

    if (bt_spp_service_register((uint8_t *)bisto_audio_service_class_uuid128,
                                RFC_SPP_BISTO_AU_CHANN_NUM) == false)
    {
        APP_PRINT_ERROR0("app_bisto_bt_spp_init: bisto audio spp reg failed");
        return false;
    };

    bt_mgr_cback_register(app_bisto_bt_spp_cback);

    return true;
}


void app_bisto_bt_set_name(void)
{
    gap_br_set_param(GAP_BR_PARAM_NAME, sizeof(BISTO_BT_NAME), BISTO_BT_NAME);
}


int32_t app_bisto_bt_init(void)
{
    app_bisto_bt.p_bt = bisto_bt_init(RFC_SPP_BISTO_CTRL_CHANN_NUM, RFC_SPP_BISTO_AU_CHANN_NUM);

    app_bisto_bt_spp_init();

    return 0;
}


void app_bisto_bt_handle_b2s_connected(uint8_t *bd_addr)
{
    bisto_bt_link_connect(bisto_bt_get_instance(), bd_addr);
    bisto_bt_channs_create(app_bisto_bt.p_bt, bd_addr);
    memcpy(app_bisto_bt.bd_addr, bd_addr, 6);
}


void app_bisto_bt_handle_b2s_disconnected(uint8_t *bd_addr)
{
    bisto_bt_link_disconnect(bisto_bt_get_instance(), bd_addr);
    bisto_bt_channs_delete(app_bisto_bt.p_bt, bd_addr);
    memset(app_bisto_bt.bd_addr, 0, 6);
}

#endif



