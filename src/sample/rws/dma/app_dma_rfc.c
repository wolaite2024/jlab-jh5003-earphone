#if DMA_FEATURE_SUPPORT
#include "app_dma_rfc.h"
#include "bt_rfc.h"
#include "stdlib.h"
#include "app_sdp.h"
#include "trace.h"


struct
{
    APP_DMA_TRANSPORT_CB cb;
} app_dma_rfc_mgr = {.cb = NULL};


static void app_dma_rfc_cback(uint8_t *bd_addr, T_BT_RFC_MSG_TYPE msg_type, void *msg_buf)
{
    APP_PRINT_INFO1("app_dma_rfc_cback: msg_type 0x%02x", msg_type);

    APP_DMA_TRANSPORT_PARAM param = {0};
    param.bd_addr = bd_addr;
    param.conn_id = DMA_DEFAULT_CONN_ID;

    switch (msg_type)
    {
    case BT_RFC_MSG_CONN_IND:
        {
            T_BT_RFC_CONN_IND *p_ind = (T_BT_RFC_CONN_IND *)msg_buf;

            bt_rfc_conn_cfm(bd_addr, RFC_SPP_DMA_CHANN_NUM, true, p_ind->frame_size, 7);
        }
        break;

    case BT_RFC_MSG_CONN_CMPL:
        {
            T_BT_RFC_CONN_CMPL *p_cmpl = (T_BT_RFC_CONN_CMPL *)msg_buf;
            param.conn_cmpl.mtu = p_cmpl->frame_size;
            app_dma_rfc_mgr.cb(APP_DMA_CONN_CMPL, &param);
        }
        break;

    case BT_RFC_MSG_DISCONN_CMPL:
        {
            app_dma_rfc_mgr.cb(APP_DMA_DISC_CMPL, &param);
        }
        break;

    case BT_RFC_MSG_DATA_IND:
        {
            T_BT_RFC_DATA_IND *p_ind = (T_BT_RFC_DATA_IND *)msg_buf;

            param.data_ind.data = p_ind->buf;
            param.data_ind.data_len = p_ind->length;
            app_dma_rfc_mgr.cb(APP_DMA_DATA_IND, &param);

            bt_rfc_credits_give(bd_addr, p_ind->local_server_chann, 1);
        }
        break;

    case BT_RFC_MSG_CREDIT_INFO:
        break;

    default:
        break;
    }
}


bool app_dma_rfc_write(uint8_t *bd_addr, uint8_t *data, uint32_t data_len)
{
    return bt_rfc_data_send(bd_addr, RFC_SPP_DMA_CHANN_NUM, data, data_len, false);
}


void app_dma_rfc_init(APP_DMA_TRANSPORT_CB cb)
{
    app_dma_rfc_mgr.cb = cb;

    bt_rfc_service_register(RFC_SPP_DMA_CHANN_NUM, app_dma_rfc_cback);
}

#endif
