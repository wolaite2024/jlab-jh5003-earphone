#if AMA_FEATURE_SUPPORT
#include "string.h"
#include "app_ama.h"
#include "app_ama_spp.h"
#include "spp_stream.h"


#include "app_main.h"
#include "bt_spp.h"
#include "btm.h"
#include "trace.h"
#include "app_cfg.h"
static const uint8_t ama_service_class_uuid128[16] =
{
    0x93, 0x1C, 0x7E, 0x8A, 0x54, 0x0F, 0x46, 0x86, 0xB7, 0x98, 0xE8, 0xDF, 0x0A, 0x2A, 0xD9, 0xF7
};


static void app_ama_spp_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    if (extend_app_cfg_const.ama_support)
    {
        T_BT_EVENT_PARAM *param = event_buf;
        T_APP_BR_LINK *p_link;
        bool handle = true;

        switch (event_type)
        {
        case BT_EVENT_SPP_CONN_CMPL:
            {
                if (param->spp_conn_cmpl.local_server_chann != RFC_SPP_AMA_CHANN_NUM)
                {
                    return;
                }

                T_APP_BR_LINK *p_link;
                p_link = app_link_find_br_link(param->spp_conn_cmpl.bd_addr);
                if (p_link != NULL)
                {
                    if (app_db.device_state == APP_DEVICE_STATE_OFF)//timing issue
                    {
                        //app_cancel_page_disconnect_all_link();
                    }
                    else
                    {
                        T_SPP_STREAM_EVENT_PARAM spp_stream_param;
                        spp_stream_param.server_chann = param->spp_conn_cmpl.local_server_chann;
                        spp_stream_param.connect_param.mtu            = param->spp_conn_cmpl.frame_size;
                        spp_stream_param.connect_param.remote_credits = param->spp_conn_cmpl.link_credit;
                        memcpy(spp_stream_param.connect_param.bd_addr, param->spp_conn_cmpl.bd_addr, 6);
                        spp_stream_event_process(SPP_STREAM_CONNECT_EVENT, &spp_stream_param);
                    }
                }
            }
            break;

        case BT_EVENT_SPP_CREDIT_RCVD:
            {
                if (param->spp_credit_rcvd.local_server_chann != RFC_SPP_AMA_CHANN_NUM)
                {
                    return;
                }

                p_link = app_link_find_br_link(param->spp_credit_rcvd.bd_addr);
                if (p_link == NULL)
                {
                    APP_PRINT_ERROR0("app_ama_dm_cback: no acl link found");
                    return;
                }

                {
                    T_SPP_STREAM_EVENT_PARAM spp_stream_param;
                    spp_stream_param.server_chann = param->spp_credit_rcvd.local_server_chann;
                    spp_stream_param.update_credit_param.remote_credits = param->spp_credit_rcvd.link_credit;
                    memcpy(spp_stream_param.update_credit_param.bd_addr, param->spp_credit_rcvd.bd_addr, 6);
                    spp_stream_event_process(SPP_STREAM_UPDATE_CREDIT_EVENT, &spp_stream_param);
                }
            }
            break;

        case BT_EVENT_SPP_DATA_IND:
            {
                if (param->spp_data_ind.local_server_chann != RFC_SPP_AMA_CHANN_NUM)
                {
                    return;
                }

                T_SPP_STREAM_EVENT_PARAM spp_stream_param;
                spp_stream_param.server_chann = param->spp_data_ind.local_server_chann;
                spp_stream_param.receive_data_param.data      = param->spp_data_ind.data;
                spp_stream_param.receive_data_param.length    = param->spp_data_ind.len;
                memcpy(spp_stream_param.receive_data_param.bd_addr, param->spp_data_ind.bd_addr, 6);
                spp_stream_event_process(SPP_STREAM_RECEIVE_DATA_EVENT, &spp_stream_param);
                bt_spp_credits_give(param->spp_data_ind.bd_addr, param->spp_data_ind.local_server_chann,
                                    1);

            }
            break;
        case BT_EVENT_SPP_CONN_IND:
            {
                if (param->spp_conn_ind.local_server_chann != RFC_SPP_AMA_CHANN_NUM)
                {
                    return;
                }

                bt_spp_connect_cfm(param->spp_conn_ind.bd_addr, param->spp_conn_ind.local_server_chann, true,
                                   param->spp_conn_ind.frame_size, 7);
            }
            break;

        case BT_EVENT_SPP_DISCONN_CMPL:
            {
                if (param->spp_disconn_cmpl.local_server_chann != RFC_SPP_AMA_CHANN_NUM)
                {
                    return;
                }

                p_link = app_link_find_br_link(param->spp_disconn_cmpl.bd_addr);
                if (p_link != NULL)
                {
                    //app_handle_profile_disconn(param->spp_disconn_cmpl.bd_addr, SPP_PROFILE_MASK,
                    //                           param->spp_disconn_cmpl.cause);
                    T_SPP_STREAM_EVENT_PARAM spp_stream_param;
                    spp_stream_param.server_chann = param->spp_disconn_cmpl.local_server_chann;
                    spp_stream_param.disconnect_param.cause = param->spp_disconn_cmpl.cause;
                    memcpy(spp_stream_param.disconnect_param.bd_addr, param->spp_disconn_cmpl.bd_addr, 6);
                    spp_stream_event_process(SPP_STREAM_DISCONNECT_EVENT, &spp_stream_param);
                }
            }
            break;

        default:
            {
                handle = false;
            }
            break;
        }

        if (handle == true)
        {
            APP_PRINT_INFO1("app_ama_dm_cback: event_type 0x%04x", event_type);
        }

    }

}



bool app_ama_spp_init(void)
{
    APP_PRINT_TRACE0("app_ama_spp_init");

    bt_spp_service_register((uint8_t *)ama_service_class_uuid128, RFC_SPP_AMA_CHANN_NUM);

    bt_mgr_cback_register(app_ama_spp_cback);

    return true;
}

#endif
