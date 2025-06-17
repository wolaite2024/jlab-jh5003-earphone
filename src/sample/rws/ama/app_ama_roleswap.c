/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if AMA_FEATURE_SUPPORT

#include "app_ama.h"
#include "ama_service_api.h"
#include "app_ama_ble.h"
#include "app_ama_transport.h"
#include "app_ama_device.h"
#include "app_ama_accessory.h"
#include "app_ama_roleswap.h"

#include "remote.h"
#include <string.h>
#include "app_cfg.h"
#include "app_bt_sniffing.h"
#include "app_relay.h"
#include "trace.h"
#include "os_mem.h"


typedef enum
{
    SERVICE_INFO,
} T_APP_AMA_REMOTE_MSG;


typedef enum
{
    AMA_RELAY_ALL = 0,
    AMA_RELAY_MAX,
} AMA_RELAY_MSG_TYPE;


typedef struct
{
    uint32_t service_info_len;
    uint32_t ble_info_len;
    uint32_t stream_info_len;
    uint32_t device_info_len;
    uint32_t total_len;
} INFO_LENS;


static void app_ama_roleswap_calc_info_lens(INFO_LENS *p_lens)
{
    p_lens->total_len += p_lens->service_info_len = ama_get_service_info_len();
    p_lens->total_len += p_lens->ble_info_len = app_ama_ble_get_info_len();
    p_lens->total_len += p_lens->stream_info_len = ama_stream_get_info_len();
    p_lens->total_len += p_lens->device_info_len = app_ama_device_get_info_len();

    APP_PRINT_TRACE5("app_ama_roleswap calc_info_lens: service_info_len %d, ble_info_len %d, stream_info_len %d, device_info_len %d, total_len %d",
                     p_lens->service_info_len, p_lens->ble_info_len, p_lens->stream_info_len, p_lens->device_info_len,
                     p_lens->total_len);
}

#if F_APP_ERWS_SUPPORT
static void app_ama_roleswap_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                         T_REMOTE_RELAY_STATUS status)
{
    if (status != REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        return;
    }

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }

    APP_PRINT_TRACE1("app_ama_parse_cback: msg_type %d", msg_type);

    uint8_t *buf_temp = NULL;
    uint8_t *aligned_buf = NULL;

    APP_PRINT_TRACE2("ama_relay_cback:	data len %d, data %b", len, TRACE_BINARY(len, buf));

    if ((uint32_t)buf & 0b11)
    {
        APP_PRINT_WARN0("ama_relay_cback: buf for struct should be alligned");

        aligned_buf = ama_mem_alloc(AMA_ANY_MEMORY, len);
        if (aligned_buf == NULL)
        {
            return;
        }

        memcpy((uint8_t *)aligned_buf, (uint8_t *)buf, len);
        buf_temp = aligned_buf;
    }
    else
    {
        buf_temp = buf;
    }

    uint32_t data_len;
    data_len = ama_set_roleswap_service_process_info(buf_temp);
    APP_PRINT_TRACE2("app_ama_roleswap_parse_cback: data_len %d, service data %b", data_len,
                     TRACE_BINARY(data_len, buf_temp));
    buf_temp += data_len;

    data_len = app_ama_ble_set_info(buf_temp);
    APP_PRINT_TRACE2("app_ama_roleswap_parse_cback: data_len %d, ble data %b", data_len,
                     TRACE_BINARY(data_len, buf_temp));
    buf_temp += data_len;

    data_len = ama_stream_set_info(buf_temp);
    APP_PRINT_TRACE2("app_ama_roleswap_parse_cback: data_len %d, stream data %b", data_len,
                     TRACE_BINARY(data_len, buf_temp));
    buf_temp += data_len;

    data_len = app_ama_device_set_info(buf_temp);
    APP_PRINT_TRACE2("app_ama_roleswap_parse_cback: data_len %d, dev data %b", data_len,
                     TRACE_BINARY(data_len, buf_temp));
    buf_temp += data_len;


    if (aligned_buf)
    {
        free(aligned_buf);
    }
}
#endif

static void app_ama_roleswap_fill_roleswap_info(uint8_t *buf, const INFO_LENS *const p_lens)
{
    uint8_t *aligned_buf = calloc(1, p_lens->total_len);
    uint8_t *buf_temp = aligned_buf;


    ama_get_roleswap_service_process_info(buf_temp);
    APP_PRINT_TRACE2("app_ama_roleswap fill_roleswap_info: len %d, service data %b",
                     p_lens->service_info_len,
                     TRACE_BINARY(p_lens->service_info_len, buf_temp));
    buf_temp += p_lens->service_info_len;


    app_ama_ble_get_info(buf_temp);
    APP_PRINT_TRACE2("app_ama_roleswap fill_roleswap_info: len %d, ble data %b", p_lens->ble_info_len,
                     TRACE_BINARY(p_lens->ble_info_len, buf_temp));
    buf_temp += p_lens->ble_info_len;

    ama_stream_get_info(buf_temp);
    APP_PRINT_TRACE2("app_ama_roleswap fill_roleswap_info: len %d, stream data %b",
                     p_lens->stream_info_len,
                     TRACE_BINARY(p_lens->stream_info_len, buf_temp));
    buf_temp += p_lens->stream_info_len;

    app_ama_device_get_info(buf_temp);
    APP_PRINT_TRACE2("app_ama_roleswap fill_roleswap_info: len %d, dev data %b",
                     p_lens->device_info_len,
                     TRACE_BINARY(p_lens->device_info_len, buf_temp));
    buf_temp += p_lens->device_info_len;

    APP_PRINT_TRACE2("app_ama_roleswap fill_roleswap_info: total_len %d, data %b", p_lens->total_len,
                     TRACE_BINARY(p_lens->total_len, buf));


    memcpy(buf, aligned_buf, p_lens->total_len);
    free(aligned_buf);
}


static void app_ama_roleswap_send_roleswap_info(void)
{
    uint8_t *buf = NULL;
    INFO_LENS info_lens = {0};

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_PRIMARY)
    {
        return;
    }

    app_ama_roleswap_calc_info_lens(&info_lens);

    buf = ama_mem_alloc(AMA_NONVOLATILE_MEMORY, info_lens.total_len);
    if (buf == NULL)
    {
        return;
    }

    app_ama_roleswap_fill_roleswap_info(buf, &info_lens);

    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AMA_ROLESWAP, AMA_RELAY_ALL, buf,
                                        info_lens.total_len);

    ama_mem_free(buf);
}

#if F_APP_ERWS_SUPPORT
uint16_t app_ama_roleswap_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    INFO_LENS info_lens = {0};

    APP_PRINT_INFO1("app_ama_roleswap_relay_cback: msg_type %d", msg_type);

    app_ama_roleswap_calc_info_lens(&info_lens);
    uint16_t msg_len = info_lens.total_len + 4;

    if (buf != NULL)
    {
        buf[0] = (uint8_t)(msg_len & 0xFF);
        buf[1] = (uint8_t)(msg_len >> 8);
        buf[2] = APP_MODULE_TYPE_AMA_ROLESWAP;
        buf[3] = msg_type;

        app_ama_roleswap_fill_roleswap_info(&buf[4], &info_lens);
    }

    return msg_len;
}
#endif

static void app_ama_roleswap_adv_start_for_multilink(void)
{
    uint32_t connected_dev_num = 0;
    connected_dev_num = app_ama_device_get_connected_device_num();
    if (app_cfg_const.enable_multi_link)
    {
        if (connected_dev_num < 2)
        {
            app_ama_ble_adv_start();
            return;
        }
    }
    else
    {
        if (connected_dev_num == 0)
        {
            app_ama_ble_adv_start();
            return;
        }
    }

    app_ama_ble_adv_stop();
}


static void app_ama_roleswap_handle_role_decided(void)
{
    app_ama_accessory_role_update((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_ama_roleswap_adv_start_for_multilink();
    }
    else
    {
        app_ama_ble_adv_stop();
    }
}


void app_ama_roleswap_handle_engage_role_decided(void)
{
    app_ama_roleswap_handle_role_decided();
}


void app_ama_roleswap_handle_roleswap_role_decided(void)
{
    app_ama_roleswap_handle_role_decided();
}


void app_ama_roleswap_handle_role_swap_success(T_REMOTE_SESSION_ROLE device_role)
{
    //ble should linkback by phone again, different from bredr
    if (device_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        app_ama_device_ble_disconnect_all();
    }

    app_ama_roleswap_handle_roleswap_role_decided();
}


void app_ama_roleswap_handle_role_swap_fail(T_REMOTE_SESSION_ROLE device_role)
{
//    app_ama_accessory_role_update((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
//    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
//    {
//        app_ama_roleswap_adv_start_for_multilink();
//    }
}


void app_ama_roleswap_handle_remote_conn_cmpl(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        app_ama_roleswap_send_roleswap_info();
    }
}


void app_ama_roleswap_handle_remote_disconn_cmpl(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_ama_device_ble_disconnect_all();
    }
}


inline static void app_ama_roleswap_start(void)
{
    app_ama_roleswap_send_roleswap_info();
}


void app_ama_roleswap_init(void)
{
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_ama_roleswap_relay_cback, app_ama_roleswap_parse_cback,
                             APP_MODULE_TYPE_AMA_ROLESWAP,
                             AMA_RELAY_MAX);
#endif
    // app_bt_sniffing_roleswap_cb_reg(app_ama_roleswap_start);
}


#endif
