/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "stdlib_corecrt.h"
#include "console.h"
#include "gap.h"
#include "gap_br.h"
#include "trace.h"
#include "bt_types.h"
#include "bt_iap.h"
#include "app_main.h"
#include "app_cmd.h"
#include "app_cfg.h"
#include "app_report.h"
#include "app_data_transfer.h"
#include "app_transfer.h"
#include "app_ble_service.h"
#include "app_spp.h"

#if F_APP_LEA_SUPPORT
#include "app_dongle_common.h"
#include "app_dongle_data_ctrl.h"
#endif

#if F_APP_ONE_WIRE_UART_SUPPORT
#include "app_one_wire_uart.h"
#endif

#if F_APP_CHATGPT_SUPPORT
#include "app_chatgpt.h"
#endif

#if F_APP_IAP_RTK_SUPPORT
#include "app_iap_rtk.h"
#endif

// for CMD_LEGACY_DATA_TRANSFER and CMD_LE_DATA_TRANSFER
static uint8_t *uart_rx_dt_pkt_ptr = NULL;
static uint16_t uart_rx_dt_pkt_len = 0;

static void app_transfer_bt_data(uint8_t *cmd_ptr, uint8_t cmd_path, uint8_t app_idx,
                                 uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));
    uint16_t total_len = (cmd_ptr[4] | (cmd_ptr[5] << 8));
    uint16_t pkt_len = (cmd_ptr[6] | (cmd_ptr[7] << 8));
    uint8_t  idx = cmd_ptr[2];
    uint8_t pkt_type = cmd_ptr[3];
    uint8_t  *pkt_ptr = &cmd_ptr[8];
    bool skip_ack = false;

#if TARGET_LE_AUDIO_GAMING
    T_APP_LE_LINK *p_lea_link = app_dongle_get_le_audio_dongle_link();

    if (p_lea_link && (cmd_id == CMD_LE_DATA_TRANSFER))
    {
        /*
            Replace idx with app_idx because LE dongle would send data to primary and secondary simultaneously.
        */
        idx = app_idx;
    }
#endif

    if (pkt_len == 0)
    {
        ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
        goto bt_data_ack;
    }

    if (pkt_type > PKT_TYPE_END)
    {
        ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
        goto bt_data_ack;
    }

    if (cmd_path == CMD_PATH_UART)
    {
        if ((pkt_type == PKT_TYPE_SINGLE) || (pkt_type == PKT_TYPE_START))
        {
            if (uart_rx_dt_pkt_ptr)
            {
                free(uart_rx_dt_pkt_ptr);
            }

            uart_rx_dt_pkt_ptr = malloc(total_len);
            memcpy_s(uart_rx_dt_pkt_ptr, total_len, pkt_ptr, pkt_len);
            uart_rx_dt_pkt_len = pkt_len;
        }
        else
        {
            if (uart_rx_dt_pkt_ptr != NULL)
            {
                uint8_t *temp_ptr;

                temp_ptr = uart_rx_dt_pkt_ptr + uart_rx_dt_pkt_len;
                memcpy(temp_ptr, pkt_ptr, pkt_len);
                uart_rx_dt_pkt_len += pkt_len;
            }
        }

        if ((pkt_type == PKT_TYPE_SINGLE) || (pkt_type == PKT_TYPE_END))
        {
            if (ack_pkt[2] == CMD_SET_STATUS_COMPLETE)
            {
                uint8_t cmd_path = CMD_PATH_NONE;

                if (cmd_id == CMD_LEGACY_DATA_TRANSFER)
                {
                    if (app_db.br_link[idx].connected_profile & SPP_PROFILE_MASK)
                    {
                        cmd_path = CMD_PATH_SPP;
                    }
                    else if (app_db.br_link[idx].connected_profile & IAP_PROFILE_MASK)
                    {
                        cmd_path = CMD_PATH_IAP;
                    }
                }
                else if (cmd_id == CMD_LE_DATA_TRANSFER)
                {
                    cmd_path = CMD_PATH_LE;
                }

                if (!app_report_raw_data(cmd_path, app_idx, uart_rx_dt_pkt_ptr, uart_rx_dt_pkt_len))
                {
                    ack_pkt[2] = CMD_SET_STATUS_BUSY;
                }

                free(uart_rx_dt_pkt_ptr);
                uart_rx_dt_pkt_ptr = NULL;
            }
        }
    }
    else if (cmd_path == CMD_PATH_LE)
    {
#if TARGET_LE_AUDIO_GAMING
        if (p_lea_link)
        {
            app_dongle_handle_le_data(pkt_ptr, pkt_len);
            skip_ack = true;
        }
#endif
    }

bt_data_ack:
    if (!skip_ack)
    {
        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
    }
}

static T_DT_RESULT app_transfer_console_send(uint8_t app_idx, uint8_t *data, uint32_t len)
{
    return console_write(data, len) ? DT_OK : DT_SEND_FAIL;
}

static T_DT_MODE app_transfer_check_mode(uint8_t cmd_path, uint16_t event_id, bool broadcast)
{
    if (broadcast)
    {
        return DT_MODE_NO_NEED_ACK;
    }

    if ((event_id == EVENT_ACK)
#if F_APP_GAMING_DONGLE_SUPPORT
        || (app_db.remote_is_dongle)
#endif
       )
    {
        return DT_MODE_NO_NEED_ACK;
    }

    if (cmd_path == CMD_PATH_UART)
    {
        if (((event_id == EVENT_ACK) && (app_cfg_const.report_uart_event_only_once == 0)) ||
            app_cfg_const.report_uart_event_only_once
#if F_APP_ONE_WIRE_UART_SUPPORT
            || (app_cfg_const.one_wire_uart_support && one_wire_state == ONE_WIRE_STATE_IN_ONE_WIRE)
#endif
           )
        {
            return DT_MODE_NO_NEED_ACK;
        }
    }

#if F_APP_DATA_CAPTURE_SUPPORT
    if ((event_id == EVENT_DATA_CAPTURE_START_STOP) ||
        (event_id == EVENT_DATA_CAPTURE_DATA)       ||
        (event_id == EVENT_DATA_CAPTURE_ENTER_EXIT) ||
        (event_id == EVENT_ACK))
    {
        return DT_MODE_NO_NEED_ACK;
    }
#endif

#if F_APP_CHATGPT_SUPPORT
    if ((event_id == TRANS_CHAT_VOICE_ID) ||
        (event_id == TRANS_CHAT_BUFF_CTRL) || (event_id == TRANS_CHAT_ACK))
    {
        return DT_MODE_NO_NEED_ACK;
    }
#endif

    return DT_MODE_NEED_ACK;
}

bool app_transfer_push_data_queue(uint8_t cmd_path, uint8_t idx, uint16_t event_id, bool broadcast,
                                  uint8_t *data, uint16_t len)
{
    T_DT_MODE mode = app_transfer_check_mode(cmd_path, event_id, broadcast);
    return app_data_transfer_push_data_queue(cmd_path, idx, data, len, mode);
}

void app_transfer_switch(uint8_t cmd_path, uint16_t event_id, bool is_big_end)
{
    app_data_transfer_switch(cmd_path, event_id, is_big_end);
}

void app_transfer_queue_reset(uint8_t cmd_path)
{
    app_data_transfer_queue_reset(cmd_path);
}

void app_transfer_pop_data_queue(uint8_t cmd_path)
{
    app_data_transfer_pop_data_queue(cmd_path);
}

void app_transfer_handle_msg(T_IO_MSG *io_driver_msg_recv)
{
    app_data_transfer_handle_msg(io_driver_msg_recv);
}

bool app_transfer_start_for_le(uint8_t le_link_id, uint16_t data_len, uint8_t *p_data)
{
    uint8_t idx = 0;
    uint16_t len = data_len + 8;
    uint16_t opcode = CMD_LE_DATA_TRANSFER;
    T_APP_LE_LINK *p_le_link = NULL;
    uint8_t *p_transmit_data = NULL;
    bool ret = false;

    if (p_data == NULL)
    {
        return false;
    }

#if F_APP_LEA_SUPPORT
    if (app_db.le_link[le_link_id].used)
    {
        p_le_link = &app_db.le_link[le_link_id];
    }
#endif
    if (p_le_link == NULL)
    {
        APP_PRINT_ERROR0("app_transfer_start_for_le: no le link exsit.");
        return false;
    }

    p_transmit_data = calloc(1, len + 4);
    if (p_transmit_data == NULL)
    {
        return false;
    }

    p_le_link->cmd.tx_seqn++;
    p_transmit_data[idx++] = CMD_SYNC_BYTE;
    p_transmit_data[idx++] = p_le_link->cmd.tx_seqn;
    p_transmit_data[idx++] = len;
    p_transmit_data[idx++] = len >> 8;
    p_transmit_data[idx++] = opcode;
    p_transmit_data[idx++] = opcode >> 8;
    p_transmit_data[idx++] = le_link_id;
    p_transmit_data[idx++] = PKT_TYPE_SINGLE;
    p_transmit_data[idx++] = data_len +
                             2; /*total_len: when type is single, total_len = payload_len + 2 */
    p_transmit_data[idx++] = (data_len + 2) >> 8;
    p_transmit_data[idx++] = data_len;     //payload_len
    p_transmit_data[idx++] = data_len >> 8;

    for (int i = 0; i < data_len; i++)
    {
        p_transmit_data[idx++] = p_data[i];
    }
    APP_PRINT_INFO1("app_transfer_start_for_le: p_transmit_data %s",
                    TRACE_BINARY(data_len + 12, p_transmit_data));

    ret = app_report_raw_data(CMD_PATH_LE, le_link_id, p_transmit_data, data_len + 12);
    free(p_transmit_data);

    return ret;
}

void app_transfer_handle_ble_disconnected(T_APP_LE_LINK *p_link, uint16_t disc_cause)
{
    if (app_cfg_const.enable_data_uart)
    {
        uint8_t event_buff[3];

        event_buff[0] = p_link->id;
        event_buff[1] = (uint8_t)(disc_cause);
        event_buff[2] = (uint8_t)(disc_cause >> 8);
        app_report_event(CMD_PATH_UART, EVENT_LE_DISCONNECTED, 0, &event_buff[0], 3);
    }

    app_transfer_queue_reset(CMD_PATH_LE);
}

void app_transfer_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                             uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));

    switch (cmd_id)
    {
    case CMD_LEGACY_DATA_TRANSFER:
    case CMD_LE_DATA_TRANSFER:
        {
            app_transfer_bt_data(&cmd_ptr[0], cmd_path, app_idx, &ack_pkt[0]);
        }
        break;

    default:
        {
            ack_pkt[2] = CMD_SET_STATUS_UNKNOW_CMD;
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;
    }
}

static void app_transfer_param_init(void)
{
    app_db.external_mcu_mtu = 256;

    data_trans.map[CMD_PATH_UART].send_instance = app_transfer_console_send;
    data_trans.map[CMD_PATH_LE].send_instance = app_ble_service_transfer;
    data_trans.map[CMD_PATH_SPP].send_instance = app_spp_transfer;
#if F_APP_IAP_RTK_SUPPORT
    data_trans.map[CMD_PATH_IAP].send_instance = app_iap_rtk_transfer;
#endif

#if F_APP_GATT_OVER_BREDR_SUPPORT
    data_trans.map[CMD_PATH_GATT_OVER_BREDR].send_instance = app_gatt_over_bredr_transfer;
#endif

    data_trans.enable = (app_cfg_const.enable_rtk_vendor_cmd != 0) ? true : false;
}

void app_transfer_init(void)
{
    app_transfer_param_init();
    app_data_transfer_init();
}
