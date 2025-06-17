#include "app_flags.h"
#if F_APP_XIAOWEI_FEATURE_SUPPORT
#include "xiaowei_protocol.h"
#include <stdlib.h>
#include <string.h>
#include "trace.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_xiaowei.h"
#include "xiaowei_ble_service.h"
#include "app_xiaowei_device.h"
#include "app_xiaowei_record.h"
#include "app_multilink.h"
#include "xiaowei_bt.h"
#include "bt_avrcp.h"
#include "app_main.h"
#include "bt_hfp.h"
#include "app_a2dp.h"

T_XIAOWEI_FVALUE_STORE app_xiaowei_fvalues[XIAOWEI_FVALUE_MAX_NUMBER] = {0};

uint8_t app_exp_bit_rate = 0;/*Expected bit rate of app*/

/*xiaowei send response data secquence number, it is eaqual to the secquence number of received data*/
static uint8_t xiaowei_response_data_sn = 0;

static uint8_t protocol_version = 5;/*xiaowei protocol version 2,3,4,5*/

static uint8_t firmware_version[4] = {0, 0, 0, 0};/*0.0.0.0*/

static uint8_t sign[2] = {1, 1};/*TODO: 106 get sign ,how to generate sign????*/

static bool app_xiaowei_get_sn(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                               uint8_t *sn)
{
    T_XIAOWEI_LINK *p_link = app_xiaowei_find_link(type, bd_addr);
    if (p_link)
    {
        *sn = p_link->cmd_sn;
        return true;
    }
    else
    {
        //APP_PRINT_ERROR0("app_xiaowei_get_sn: p_link NULL");
    }
    return false;
}

static bool app_xiaowei_inc_sn(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr, uint8_t conn_id)
{
    T_XIAOWEI_LINK *p_link = app_xiaowei_find_link(type, bd_addr);
    if (p_link)
    {
        p_link->cmd_sn++;
        return true;
    }
    else
    {
        //APP_PRINT_ERROR0("app_xiaowei_inc_sn: p_link NULL");
    }
    return false;
}

static void app_xiaowei_send_request_data(T_XIAOWEI_CONNECTION_TYPE connection_type,
                                          uint8_t conn_id,
                                          uint8_t *bd_addr,
                                          T_XIAOWEI_CMD cmd, uint8_t sequence_number,
                                          void *p_payload)
{
    T_XIAOWEI_REQUEST xiaowei_request_data;
    T_XIAOWEI_DATA *p_xiaowei_data = NULL;
    T_XIAOWEI_DATA *p_payload_data = NULL;

    if (p_payload)
    {
        p_payload_data = xiaowei_payload_pack(cmd.command_id_index, cmd.command_type,
                                              p_payload);
    }

    if (cmd.command_type == XIAOWEI_CMD_TYPE_REQUEST)
    {
        xiaowei_request_data.command_id = cmd.command_id;
        xiaowei_request_data.command_type = cmd.command_type;
        xiaowei_request_data.sequence_number = sequence_number;/*0-255 increment cycle*/

        APP_PRINT_INFO3("xiaowei_send_request_data: conn_type %d, cmd_id %d, sn %d",
                        connection_type, cmd.command_id, xiaowei_request_data.sequence_number);

        if (p_payload_data)
        {
            xiaowei_request_data.payload_len = p_payload_data->length;
            xiaowei_request_data.p_payload = p_payload_data->p_buffer;
        }
        else
        {
            xiaowei_request_data.payload_len = 0;
            xiaowei_request_data.p_payload = NULL;
        }

        p_xiaowei_data = xiaowei_request_data_pack(&xiaowei_request_data);
    }

    if (connection_type == XIAOWEI_CONNECTION_TYPE_BLE)
    {
        if (!xiaowei_ble_service_tx_send_notify(conn_id, p_xiaowei_data->p_buffer, p_xiaowei_data->length))
        {
            // APP_PRINT_ERROR0("app_xiaowei_send_request_data: ble send notify fail");
        };

    }
    else if (connection_type == XIAOWEI_CONNECTION_TYPE_SPP)
    {
        bt_xiaowei_send_tx_notify(bd_addr, p_xiaowei_data->p_buffer, p_xiaowei_data->length);

    }
    else if (connection_type == XIAOWEI_CONNECTION_TYPE_IAP)
    {
        //TODO

    }

    if (p_payload_data)
    {
        xiaowei_data_free(p_payload_data);
        p_payload_data = NULL;
    }

    if (p_xiaowei_data)
    {
        xiaowei_data_free(p_xiaowei_data);
        p_xiaowei_data = NULL;
    }
}

static void app_xiaowei_send_response_data(T_XIAOWEI_CONNECTION_TYPE connection_type,
                                           uint8_t conn_id,
                                           T_XIAOWEI_CMD cmd, uint8_t *bd_addr,
                                           T_XIAOWEI_RESPONSE_CODE response_code, void *p_payload)
{
    T_XIAOWEI_RESPONSE xiaowei_response_data;
    T_XIAOWEI_DATA *p_xiaowei_data = NULL;
    T_XIAOWEI_DATA *p_payload_data = NULL;

    if (p_payload)
    {
        p_payload_data = xiaowei_payload_pack(cmd.command_id_index, cmd.command_type, p_payload);
    }

    if (cmd.command_type == XIAOWEI_CMD_TYPE_RESPONSE)
    {
        xiaowei_response_data.command_id = cmd.command_id;
        xiaowei_response_data.command_type = cmd.command_type;
        xiaowei_response_data.sequence_number = xiaowei_response_data_sn;
        xiaowei_response_data.response_code = response_code;

        APP_PRINT_INFO3("xiaowei_send_response_data:command id %d, connection type %d, sn %d",
                        cmd.command_id, connection_type, xiaowei_response_data.sequence_number);

        if (p_payload_data)
        {
            xiaowei_response_data.payload_len = p_payload_data->length;
            xiaowei_response_data.p_payload = p_payload_data->p_buffer;
        }
        else
        {
            xiaowei_response_data.payload_len = 0;
            xiaowei_response_data.p_payload = NULL;
        }

        p_xiaowei_data = xiaowei_response_data_pack(&xiaowei_response_data);
    }

    if (connection_type == XIAOWEI_CONNECTION_TYPE_BLE)
    {
        if (!xiaowei_ble_service_tx_send_notify(conn_id, p_xiaowei_data->p_buffer, p_xiaowei_data->length))
        {
            //APP_PRINT_ERROR0("app_xiaowei_send_response_data: ble send notify fail");
        };
    }
    else if (connection_type == XIAOWEI_CONNECTION_TYPE_SPP)
    {
        bt_xiaowei_send_tx_notify(bd_addr, p_xiaowei_data->p_buffer, p_xiaowei_data->length);
    }
    else if (connection_type == XIAOWEI_CONNECTION_TYPE_IAP)
    {
        //TODO
    }

    if (p_payload_data)
    {
        xiaowei_data_free(p_payload_data);
        p_payload_data = NULL;
    }

    if (p_xiaowei_data)
    {
        xiaowei_data_free(p_xiaowei_data);
        p_xiaowei_data = NULL;
    }
}

void app_xiaowei_101_send_wakeup_request(T_XIAOWEI_CONNECTION_TYPE connection_type,
                                         uint8_t *bd_addr,
                                         uint8_t conn_id)
{
    T_XIAOWEI_CMD cmd;
    bool sn_err = false;
    cmd.command_id = XIAOWEI_CMD_ID_WAKEUP;
    cmd.command_id_index = XIAOWEI_CMD_IDEX_WAKEUP;
    cmd.command_type = XIAOWEI_CMD_TYPE_REQUEST;

    uint8_t sequence_number = 0;
    if (app_xiaowei_get_sn(connection_type, bd_addr, conn_id, &sequence_number))
    {
        app_xiaowei_send_request_data(connection_type, conn_id, bd_addr, cmd, sequence_number, NULL);
        app_xiaowei_inc_sn(connection_type, bd_addr, conn_id);
    }
    else
    {
        sn_err = true;
    };

    APP_PRINT_INFO2("app_xiaowei_101_send_wakeup_request:conn_type %d, sn_err %d", connection_type,
                    sn_err);
}

void app_xiaowei_handle_101_wakeup_response(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr,
                                            T_XIAOWEI_RESPONSE *p_response)
{
    T_XIAOWEI_RESPONSE_CODE cmd_result = (T_XIAOWEI_RESPONSE_CODE)p_response->response_code;

    if (cmd_result == XIAOWEI_RESPONSE_SUCCESS)
    {
        app_xiaowei_start_recording(bd_addr);
        app_xiaowei_device_set_va_state(bd_addr, XIAOWEI_VA_STATE_ACTIVE);
    }
    else
    {

    }

    APP_PRINT_INFO2("app_xiaowei_handle_101_wakeup_response: conn type %d, cmd result %d", type,
                    cmd_result);
}

void app_xiaowei_102_send_silence_request(T_XIAOWEI_CONNECTION_TYPE connection_type,
                                          uint8_t *bd_addr,
                                          uint8_t conn_id)
{
    bool sn_err = false;
    T_XIAOWEI_CMD cmd;
    cmd.command_id = XIAOWEI_CMD_ID_SILENCE;
    cmd.command_id_index = XIAOWEI_CMD_IDEX_SILENCE;
    cmd.command_type = XIAOWEI_CMD_TYPE_REQUEST;

    uint8_t sequence_number = 0;
    if (app_xiaowei_get_sn(connection_type, bd_addr, conn_id, &sequence_number))
    {
        app_xiaowei_send_request_data(connection_type, conn_id, bd_addr, cmd, sequence_number, NULL);
        app_xiaowei_inc_sn(connection_type, bd_addr, conn_id);
    }
    else
    {
        sn_err = true;
    };

    APP_PRINT_INFO2("xiaowei_102_send_silence_request:conn_type %d, sn_err %d", connection_type,
                    sn_err);
}

void app_xiaowei_handle_102_silence_response(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr,
                                             T_XIAOWEI_RESPONSE *p_response)
{
    T_XIAOWEI_RESPONSE_CODE cmd_result = (T_XIAOWEI_RESPONSE_CODE)p_response->response_code;
    if (cmd_result == XIAOWEI_RESPONSE_SUCCESS)
    {
        T_XIAOWEI_VA_STATE va_state = app_xiaowei_device_get_va_state(bd_addr);
        if (va_state != XIAOWEI_VA_STATE_IDLE)
        {
            uint8_t app_idx = app_a2dp_get_active_idx();
            bt_sniff_mode_enable(bd_addr, 784 + 32 * app_idx, 816 + 32 * app_idx, 0, 0);
            app_xiaowei_stop_recording(bd_addr);
            app_xiaowei_device_set_va_state(bd_addr, XIAOWEI_VA_STATE_IDLE);
        }
    }
    else
    {

    }

    APP_PRINT_INFO2("app_xiaowei_handle_102_silence_response: conn type %d, cmd result %d", type,
                    cmd_result);
}

void app_xiaowei_102_silence_response(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr,
                                      uint8_t conn_id, T_XIAOWEI_RESPONSE_CODE rsp_code)
{
    APP_PRINT_INFO1("app_xiaowei_102_silence_response:conn_type %d", type);
    T_XIAOWEI_CMD cmd;
    cmd.command_id = XIAOWEI_CMD_ID_SILENCE;
    cmd.command_id_index = XIAOWEI_CMD_IDEX_SILENCE;
    cmd.command_type = XIAOWEI_CMD_TYPE_RESPONSE;

    app_xiaowei_send_response_data(type, conn_id, cmd, bd_addr, rsp_code, NULL);

    T_XIAOWEI_VA_STATE va_state = app_xiaowei_device_get_va_state(bd_addr);
    if (va_state != XIAOWEI_VA_STATE_IDLE)
    {
        uint8_t app_idx = app_a2dp_get_active_idx();
        bt_sniff_mode_enable(bd_addr, 784 + 32 * app_idx, 816 + 32 * app_idx, 0, 0);
        app_xiaowei_stop_recording(bd_addr);
        app_xiaowei_device_set_va_state(bd_addr, XIAOWEI_VA_STATE_IDLE);
    }
};

void app_xiaowei_handle_102_silence_request(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr,
                                            uint8_t conn_id, T_XIAOWEI_REQUEST *p_request)
{
    APP_PRINT_INFO1("app_xiaowei_handle_102_silence_request: conn_type %d", type);
    xiaowei_response_data_sn = p_request->sequence_number;
    app_xiaowei_102_silence_response(type, bd_addr, conn_id, XIAOWEI_RESPONSE_SUCCESS);
};

void app_xiaowei_103_get_info_response(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr,
                                       uint8_t conn_id, T_XIAOWEI_RESPONSE_CODE rsp_code)
{
    T_XIAOWEI_CMD cmd;
    cmd.command_id = XIAOWEI_CMD_ID_GET_INFO;
    cmd.command_id_index = XIAOWEI_CMD_IDEX_GET_INFO;
    cmd.command_type = XIAOWEI_CMD_TYPE_RESPONSE;

    T_XIAOWEI_PAYLOAD_103_GET_INFO_RESPONSE response_payload;
    response_payload.product_id = extend_app_cfg_const.xiaowei_product_id;

    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        memcpy(response_payload.mac_addr, app_db.factory_addr, 6);
    }
    else
    {
        memcpy(response_payload.mac_addr, app_cfg_nv.bud_local_addr, 6);
    }

    uint8_t fvalue[8] = {0};
    xiaowei_fvalue_get(bd_addr, fvalue);
    memcpy(response_payload.fvalue, fvalue, 8);
    APP_PRINT_TRACE3("app_xiaowei_103_get_info_response: conn_type %d,fvalue %b,product_id 0x%x", type,
                     TRACE_BINARY(8,
                                  response_payload.fvalue), extend_app_cfg_const.xiaowei_product_id);

    //0:not connected, 1:QQ Music connected, 2:xiaowei connected, 3: like google assistant/ amazon alexa
    response_payload.connect_state = 0;
    response_payload.keep_conn = 0;

    app_xiaowei_send_response_data(type, conn_id, cmd, bd_addr, rsp_code, &response_payload);
};

void app_xiaowei_handle_103_get_info_request(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr,
                                             uint8_t conn_id, T_XIAOWEI_REQUEST *p_request)
{
    APP_PRINT_INFO1("app_xiaowei_handle_103_get_info_request: conn_type %d", type);
    xiaowei_response_data_sn = p_request->sequence_number;
    app_xiaowei_103_get_info_response(type, bd_addr, conn_id, XIAOWEI_RESPONSE_SUCCESS);
};

void app_xiaowei_105_get_config_response(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr,
                                         uint8_t conn_id, T_XIAOWEI_RESPONSE_CODE rsp_code)
{
    APP_PRINT_INFO2("app_xiaowei_105_get_config_response: conn_type %d, app_exp_bit_rate %d",
                    type, app_exp_bit_rate);
    T_XIAOWEI_CMD cmd;
    cmd.command_id = XIAOWEI_CMD_ID_GET_CONFIG;
    cmd.command_id_index = XIAOWEI_CMD_IDEX_GET_CONFIG;
    cmd.command_type = XIAOWEI_CMD_TYPE_RESPONSE;

    T_XIAOWEI_PAYLOAD_105_GET_CONFIG_RESPONSE response_payload;
    response_payload.connection_type =  type;
    response_payload.encode_type = XIAOWEI_ENCODE_TYPE_OPUS;
    if (app_exp_bit_rate == 32)
    {
        response_payload.compress_rate = 8;
    }
    else if (app_exp_bit_rate == 16)
    {
        response_payload.compress_rate = 16;
    }
    else
    {
//        APP_PRINT_ERROR0("app_xiaowei_105_get_config_response: err bit rate");
        return;
    }

    response_payload.sample_rate = XIAOWEI_SAMPLE_RATE_16000;
    response_payload.channel_count = 1;
    response_payload.bit_depth = XIAOWEI_BIT_DEPTH_16;
    response_payload.record_type = XIAOWEI_RECORD_TYPE_HOLD;//1:long press,2:short press

    memcpy(response_payload.firmware_version, firmware_version, 4);

    response_payload.model_name = extend_app_cfg_const.xiaowei_model_name;
    response_payload.model_name_length = sizeof(extend_app_cfg_const.xiaowei_model_name);

    response_payload.protocol_version = protocol_version;
    response_payload.extra_config.length = 0;
    response_payload.extra_config.p_buffer = NULL;

    app_xiaowei_send_response_data(type, conn_id, cmd, bd_addr, rsp_code, &response_payload);
};

void app_xiaowei_handle_105_get_config_request(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr,
                                               uint8_t conn_id, T_XIAOWEI_CMD_IDX cmd_id_index, T_XIAOWEI_REQUEST *p_request)
{
    if ((p_request == NULL) || (bd_addr == NULL))
    {
        // APP_PRINT_ERROR0("app_xiaowei_handle_105_get_config_request: NULL");
        return;
    }

    xiaowei_response_data_sn = p_request->sequence_number;
    uint16_t payload_length = p_request->payload_len;

    if (payload_length)
    {
        T_XIAOWEI_PAYLOAD_105_GET_CONFIG_REQUEST *p_xiaowei_get_config_request_payload =
            xiaowei_payload_unpack(cmd_id_index, XIAOWEI_CMD_TYPE_REQUEST, p_request->p_payload,
                                   payload_length);

        if (p_xiaowei_get_config_request_payload)
        {
            app_exp_bit_rate = p_xiaowei_get_config_request_payload->expect_Bps;
            T_XIAOWEI_OS_TYPE os_system = p_xiaowei_get_config_request_payload->os_type;
            app_xiaowei_105_get_config_response(type, bd_addr, conn_id, XIAOWEI_RESPONSE_SUCCESS);

            uint8_t fvalue[8] = {0};
            xiaowei_fvalue_get(bd_addr, fvalue);
            if (memcmp(fvalue, p_xiaowei_get_config_request_payload->fvaule, 8))
            {
                xiaowei_fvalue_set(bd_addr, p_xiaowei_get_config_request_payload->fvaule, os_system);
            }

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                xiaowei_dev_send_fvalue_info_to_sec();
            }

            app_xiaowei_record_init(app_exp_bit_rate);
        }

        if (p_xiaowei_get_config_request_payload)
        {
            xiaowei_payload_unpack_free(cmd_id_index, XIAOWEI_CMD_TYPE_REQUEST,
                                        p_xiaowei_get_config_request_payload);
        }
    }
};

void app_xiaowei_106_get_sign_response(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr,
                                       uint8_t conn_id, T_XIAOWEI_RESPONSE_CODE rsp_code)
{
    APP_PRINT_INFO1("app_xiaowei_106_get_sign_response: conn_type %d", type);
    T_XIAOWEI_CMD cmd;
    cmd.command_id = XIAOWEI_CMD_ID_GET_SIGN;
    cmd.command_id_index = XIAOWEI_CMD_IDEX_GET_SIGN;
    cmd.command_type = XIAOWEI_CMD_TYPE_RESPONSE;

    T_XIAOWEI_PAYLOAD_106_GET_SIGN_RESPONSE response_paylaod;

    response_paylaod.sig.p_buffer = sign;
    response_paylaod.sig.length = sizeof(sign);

    app_xiaowei_send_response_data(type, conn_id, cmd, bd_addr, rsp_code, &response_paylaod);
};

void app_xiaowei_handle_106_get_sign_request(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr,
                                             uint8_t conn_id, T_XIAOWEI_CMD_IDX cmd_id_index, T_XIAOWEI_REQUEST *p_request)
{
    if ((bd_addr == NULL) || (p_request == NULL))
    {
        //APP_PRINT_ERROR0("app_xiaowei_handle_106_get_sign_request: NULL");
        return;
    }

    xiaowei_response_data_sn = p_request->sequence_number;
    uint16_t payload_length = p_request->payload_len;

    if (payload_length)
    {
        T_XIAOWEI_PAYLOAD_106_GET_SIGN_REQUEST *p_xiaowei_get_sign_request_payload = xiaowei_payload_unpack(
                cmd_id_index, XIAOWEI_CMD_TYPE_REQUEST, p_request->p_payload, payload_length);

        if (p_xiaowei_get_sign_request_payload)
        {
            APP_PRINT_TRACE1("app_xiaowei_handle_106_get_sign_request phone_id %b",
                             TRACE_BINARY(p_xiaowei_get_sign_request_payload->phone_id.length,
                                          p_xiaowei_get_sign_request_payload->phone_id.p_buffer));

        }

        app_xiaowei_106_get_sign_response(type, bd_addr, conn_id, XIAOWEI_RESPONSE_SUCCESS);

        if (p_xiaowei_get_sign_request_payload)
        {
            xiaowei_payload_unpack_free(cmd_id_index, XIAOWEI_CMD_TYPE_REQUEST,
                                        p_xiaowei_get_sign_request_payload);
        }
    }
};

void app_xiaowei_108_send_stop_record_request(T_XIAOWEI_CONNECTION_TYPE connection_type,
                                              uint8_t *bd_addr, uint8_t conn_id)
{
    APP_PRINT_INFO1("xiaowei_108_send_stop_record_request:conn_type %d", connection_type);
    T_XIAOWEI_CMD cmd;
    cmd.command_id = XIAOWEI_CMD_ID_REQ_STOP_RECORD;
    cmd.command_id_index = XIAOWEI_CMD_IDEX_REQ_STOP_RECORD;
    cmd.command_type = XIAOWEI_CMD_TYPE_REQUEST;

    uint8_t sequence_number = 0;
    if (app_xiaowei_get_sn(connection_type, bd_addr, conn_id, &sequence_number))
    {
        app_xiaowei_send_request_data(connection_type, conn_id, bd_addr, cmd, sequence_number, NULL);
        app_xiaowei_inc_sn(connection_type, bd_addr, conn_id);
    }
    else
    {
        APP_PRINT_ERROR0("xiaowei_108_send_stop_record_request: sn err");
    };
}

void app_xiaowei_handle_108_stop_record_response(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr,
                                                 uint8_t conn_id, T_XIAOWEI_RESPONSE *p_response)
{
    T_XIAOWEI_RESPONSE_CODE cmd_result = (T_XIAOWEI_RESPONSE_CODE)p_response->response_code;
    APP_PRINT_INFO2("app_xiaowei_handle_108_stop_record_response: conn type %d, cmd_result %d", type,
                    cmd_result);
}

void app_xiaowei_110_clear_fvalue_response(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr,
                                           uint8_t conn_id, T_XIAOWEI_RESPONSE_CODE rsp_code)
{
    APP_PRINT_INFO1("app_xiaowei_110_clear_fvalue_response: conn_type %d", type);
    T_XIAOWEI_CMD cmd;
    cmd.command_id = XIAOWEI_CMD_ID_CLEAR_F_VALUE;
    cmd.command_id_index = XIAOWEI_CMD_IDEX_CLEAR_F_VALUE;
    cmd.command_type = XIAOWEI_CMD_TYPE_RESPONSE;

    app_xiaowei_send_response_data(type, conn_id, cmd, bd_addr, rsp_code, NULL);
};

void app_xiaowei_handle_110_clear_fvalue_request(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr,
                                                 uint8_t conn_id, T_XIAOWEI_CMD_IDX cmd_id_index, T_XIAOWEI_REQUEST *p_request)
{
    if ((p_request == NULL) || (bd_addr == NULL))
    {
        //APP_PRINT_ERROR0("app_xiaowei_handle_110_clear_fvalue_request: NULL");
        return;
    }

    xiaowei_response_data_sn = p_request->sequence_number;
    uint16_t payload_length = p_request->payload_len;

    if (payload_length)
    {
        T_XIAOWEI_PAYLOAD_110_CLEAR_FVALUE_REQUEST *p_xiaowei_get_config_request_payload =
            xiaowei_payload_unpack(cmd_id_index, XIAOWEI_CMD_TYPE_REQUEST, p_request->p_payload,
                                   payload_length);

        if (p_xiaowei_get_config_request_payload)
        {
            uint8_t clear_fvalue_flags = p_xiaowei_get_config_request_payload->clear_flag;
            if (clear_fvalue_flags)
            {
                xiaowei_fvalue_clear_all();
            }
        }

        app_xiaowei_110_clear_fvalue_response(type, bd_addr, conn_id, XIAOWEI_RESPONSE_SUCCESS);

        if (p_xiaowei_get_config_request_payload)
        {
            xiaowei_payload_unpack_free(cmd_id_index, XIAOWEI_CMD_TYPE_REQUEST,
                                        p_xiaowei_get_config_request_payload);
        }
    }
};

void app_xiaowei_111_play_control_response(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr,
                                           uint8_t conn_id, T_XIAOWEI_RESPONSE_CODE rsp_code)
{
    APP_PRINT_INFO1("app_xiaowei_111_play_control_response:conn_type %d", type);
    T_XIAOWEI_CMD cmd;
    cmd.command_id = XIAOWEI_CMD_ID_PLAY_CONTROL;
    cmd.command_id_index = XIAOWEI_CMD_IDEX_PLAY_CONTROL;
    cmd.command_type = XIAOWEI_CMD_TYPE_RESPONSE;

    app_xiaowei_send_response_data(type, conn_id, cmd, bd_addr, rsp_code, NULL);
};

void app_xiaowei_handle_111_play_control_request(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr,
                                                 uint8_t conn_id, T_XIAOWEI_CMD_IDX cmd_id_index, T_XIAOWEI_REQUEST *p_request)
{
    xiaowei_response_data_sn = p_request->sequence_number;

    if (p_request->payload_len)
    {
        T_XIAOWEI_PAYLOAD_111_PLAY_CONTROL_REQUEST *p_xiaowei_play_control_request_payload =
            xiaowei_payload_unpack(cmd_id_index, XIAOWEI_CMD_TYPE_REQUEST, p_request->p_payload,
                                   p_request->payload_len);

        if (p_xiaowei_play_control_request_payload)
        {
            APP_PRINT_INFO2("app_xiaowei_handle_111_play_control_request: conn_type %d,value %d", type,
                            p_xiaowei_play_control_request_payload->play_control_value);
            if (p_xiaowei_play_control_request_payload->play_control_value & XIAOWEI_PLAY_CONTROL_TTS_END)
            {
                //TODO
            }

            if (p_xiaowei_play_control_request_payload->play_control_value & XIAOWEI_PLAY_CONTROL_AVRCP_PAUSE)
            {
                //avrcp pause
                T_APP_BR_LINK *p_link = NULL;
                p_link = app_link_find_br_link(bd_addr);
                if (p_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
                {
                    bt_avrcp_pause(bd_addr);
                }
            }

            if (p_xiaowei_play_control_request_payload->play_control_value & XIAOWEI_PLAY_CONTROL_AVRCP_RESUME)
            {
                //avrcp resume
                T_APP_BR_LINK *p_link = NULL;
                p_link = app_link_find_br_link(bd_addr);
                if (p_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PAUSED)
                {
                    bt_avrcp_play(bd_addr);
                }
            }
        }

        app_xiaowei_111_play_control_response(type, bd_addr, conn_id, XIAOWEI_RESPONSE_SUCCESS);

        if (p_xiaowei_play_control_request_payload)
        {
            xiaowei_payload_unpack_free(cmd_id_index, XIAOWEI_CMD_TYPE_REQUEST,
                                        p_xiaowei_play_control_request_payload);
        }
    }
};

void app_xiaowei_112_heart_beat_response(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr,
                                         uint8_t conn_id, T_XIAOWEI_RESPONSE_CODE rsp_code)
{
    APP_PRINT_INFO1("app_xiaowei_112_heart_beat_response:conn_type %d", type);
    T_XIAOWEI_CMD cmd;
    cmd.command_id = XIAOWEI_CMD_ID_HEART_BEAT;
    cmd.command_id_index = XIAOWEI_CMD_IDEX_HEART_BEAT;
    cmd.command_type = XIAOWEI_CMD_TYPE_RESPONSE;

    app_xiaowei_send_response_data(type, conn_id, cmd, bd_addr, rsp_code, NULL);
};

void app_xiaowei_handle_112_heart_beat_request(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr,
                                               uint8_t conn_id, T_XIAOWEI_REQUEST *p_request)
{
    APP_PRINT_INFO1("app_xiaowei_handle_112_heart_beat_request: conn_type %d", type);
    xiaowei_response_data_sn = p_request->sequence_number;
    app_xiaowei_112_heart_beat_response(type, bd_addr, conn_id, XIAOWEI_RESPONSE_SUCCESS);
};

void app_xiaowei_255_send_voice_data_request(T_XIAOWEI_CONNECTION_TYPE connection_type,
                                             uint8_t *bd_addr, uint8_t conn_id, uint8_t *p_buffer, uint16_t length)
{
    APP_PRINT_INFO1("app_xiaowei_255_send_voice_data_request:conn_type %d", connection_type);
    T_XIAOWEI_CMD cmd;
    cmd.command_id = XIAOWEI_CMD_ID_VOICE_DATA;
    cmd.command_id_index = XIAOWEI_CMD_IDEX_VOICE_DATA;
    cmd.command_type = XIAOWEI_CMD_TYPE_REQUEST;

    T_XIAOWEI_PAYLOAD_255_VOICE_DATA_REQUEST payload;
    payload.length = length;
    payload.p_buffer = p_buffer;

    uint8_t sequence_number = 0;
    if (app_xiaowei_get_sn(connection_type, bd_addr, conn_id, &sequence_number))
    {
        app_xiaowei_send_request_data(connection_type, conn_id, bd_addr, cmd, sequence_number, &payload);
        app_xiaowei_inc_sn(connection_type, bd_addr, conn_id);
    }
    else
    {
        APP_PRINT_ERROR0("app_xiaowei_255_send_voice_data_request: sn err");
    };
}

void app_xiaowei_handle_255_voice_data_response(T_XIAOWEI_CONNECTION_TYPE type, uint8_t *bd_addr,
                                                T_XIAOWEI_RESPONSE *p_response)
{
    T_XIAOWEI_RESPONSE_CODE cmd_result = (T_XIAOWEI_RESPONSE_CODE)p_response->response_code;
    APP_PRINT_INFO2("app_xiaowei_handle_255_voice_data_response: conn type %d, cmd_result", type,
                    cmd_result);
}

void app_xiaowei_receive_data(T_XIAOWEI_CONNECTION_TYPE connection_type, uint8_t *bd_addr,
                              uint8_t conn_id, uint8_t *p_buffer, uint16_t length)
{
    if (p_buffer == NULL || bd_addr == NULL)
    {
        //APP_PRINT_ERROR0("app_xiaowei_receive_data:NULL");
        return;
    }

    T_XIAOWEI_CMD_ID cmd_id = (T_XIAOWEI_CMD_ID)p_buffer[0];
    T_XIAOWEI_CMD_TYPE cmd_type = (T_XIAOWEI_CMD_TYPE)p_buffer[1];
    T_XIAOWEI_REQUEST *p_request = NULL;
    T_XIAOWEI_RESPONSE *p_response  = NULL;
    APP_PRINT_INFO3("app_xiaowei_receive_data:conn_type %d,cmd_id %d,cmd_type %d",
                    connection_type, cmd_id, cmd_type);

    if (cmd_type == XIAOWEI_CMD_TYPE_REQUEST)
    {
        p_request = xiaowei_request_data_unpack(p_buffer, length);

        if (p_request == NULL)
        {
            // APP_PRINT_ERROR0("app_xiaowei_receive_data: p_request is NULL");
            return;
        }
    }
    else if (cmd_type == XIAOWEI_CMD_TYPE_RESPONSE)
    {
        p_response = xiaowei_response_data_unpack(p_buffer, length);

        if (p_response == NULL)
        {
            //APP_PRINT_ERROR0("app_xiaowei_receive_data: p_response is NULL");
            return;
        }
    }
    else
    {
        //APP_PRINT_ERROR0("app_xiaowei_receive_data: invalid cmd type");
        return;
    }

    T_XIAOWEI_CMD_IDX cmd_id_index = xiaowei_find_cmd_id_index(cmd_id);

    switch (cmd_id)
    {
    case XIAOWEI_CMD_ID_UNKNOWN:
        {
            //APP_PRINT_INFO0("app_xiaowei_receive_data: unknown cmd id");
        }
        break;

    case XIAOWEI_CMD_ID_WAKEUP:
        {
            if (cmd_type == XIAOWEI_CMD_TYPE_RESPONSE)
            {
                app_xiaowei_handle_101_wakeup_response(connection_type, bd_addr, p_response);
            }
        }
        break;

    case XIAOWEI_CMD_ID_SILENCE:
        {
            if (cmd_type == XIAOWEI_CMD_TYPE_REQUEST)
            {
                app_xiaowei_handle_102_silence_request(connection_type, bd_addr, conn_id, p_request);
            }

            if (cmd_type == XIAOWEI_CMD_TYPE_RESPONSE)
            {
                app_xiaowei_handle_102_silence_response(connection_type, bd_addr, p_response);
            }
        }
        break;

    case XIAOWEI_CMD_ID_GET_INFO:
        {
            if (cmd_type == XIAOWEI_CMD_TYPE_REQUEST)
            {
                app_xiaowei_handle_103_get_info_request(connection_type, bd_addr, conn_id, p_request);
            }
        }
        break;

    case XIAOWEI_CMD_ID_START_RECORD:
        {
            //not used any more
        }
        break;

    case XIAOWEI_CMD_ID_GET_CONFIG:
        {
            if (cmd_type == XIAOWEI_CMD_TYPE_REQUEST)
            {
                app_xiaowei_handle_105_get_config_request(connection_type, bd_addr, conn_id, cmd_id_index,
                                                          p_request);
            }
        }
        break;

    case XIAOWEI_CMD_ID_GET_SIGN:
        {
            if (cmd_type == XIAOWEI_CMD_TYPE_REQUEST)
            {
                app_xiaowei_handle_106_get_sign_request(connection_type, bd_addr, conn_id, cmd_id_index,
                                                        p_request);
            }
        }
        break;

    case XIAOWEI_CMD_ID_STOP_PLAY:
        {
            if (cmd_type == XIAOWEI_CMD_TYPE_RESPONSE)
            {

            }
        }
        break;

    case XIAOWEI_CMD_ID_REQ_STOP_RECORD:
        {
            if (cmd_type == XIAOWEI_CMD_TYPE_RESPONSE)
            {
                app_xiaowei_handle_108_stop_record_response(connection_type, bd_addr, conn_id, p_response);
            }
        }
        break;

    case XIAOWEI_CMD_ID_SET_KEY_FUNCTION:
        {
            if (cmd_type == XIAOWEI_CMD_TYPE_REQUEST)
            {

            }

            if (cmd_type == XIAOWEI_CMD_TYPE_RESPONSE)
            {

            }
        }
        break;

    case XIAOWEI_CMD_ID_CLEAR_F_VALUE:
        {
            if (cmd_type == XIAOWEI_CMD_TYPE_REQUEST)
            {
                app_xiaowei_handle_110_clear_fvalue_request(connection_type, bd_addr, conn_id, cmd_id_index,
                                                            p_request);
            }
        }
        break;

    case XIAOWEI_CMD_ID_PLAY_CONTROL:
        {
            if (cmd_type == XIAOWEI_CMD_TYPE_REQUEST)
            {
                app_xiaowei_handle_111_play_control_request(connection_type, bd_addr, conn_id, cmd_id_index,
                                                            p_request);
            }
        }
        break;

    case XIAOWEI_CMD_ID_HEART_BEAT:
        {
            if (cmd_type == XIAOWEI_CMD_TYPE_REQUEST)
            {
                app_xiaowei_handle_112_heart_beat_request(connection_type, bd_addr, conn_id, p_request);
            }
        }
        break;

    case XIAOWEI_CMD_ID_CONNECT_CONTROL:
        {
            if (cmd_type == XIAOWEI_CMD_TYPE_REQUEST)
            {

            }

            if (cmd_type == XIAOWEI_CMD_TYPE_RESPONSE)
            {

            }
        }
        break;

    case XIAOWEI_CMD_ID_GENERAL_CONFIG:
        {
            if (cmd_type == XIAOWEI_CMD_TYPE_REQUEST)
            {

            }

            if (cmd_type == XIAOWEI_CMD_TYPE_RESPONSE)
            {

            }
        }
        break;

    case XIAOWEI_CMD_ID_CUSTOM_SKILL:
        {
            if (cmd_type == XIAOWEI_CMD_TYPE_REQUEST)
            {

            }

            if (cmd_type == XIAOWEI_CMD_TYPE_RESPONSE)
            {

            }
        }
        break;

    case XIAOWEI_CMD_ID_VOICE_DATA:
        {
            if (cmd_type == XIAOWEI_CMD_TYPE_RESPONSE)
            {
                app_xiaowei_handle_255_voice_data_response(connection_type, bd_addr, p_response);
            }
        }
        break;

    default:
        {
//            APP_PRINT_ERROR0("app_xiaowei_receive_data: err cmd id");
        }
        break;
    }

    if (p_request)
    {
        xiaowei_request_data_unpack_free(p_request);
        p_request = NULL;
    }

    if (p_response)
    {
        xiaowei_response_data_unpack_free(p_response);
        p_response = NULL;
    }
}

void app_xiaowei_handle_factory_reset()
{
    APP_PRINT_INFO0("app_xiaowei_handle_factory_reset");
    xiaowei_fvalue_clear_all();
}

/**
  * @brief send fvalue to remote
  */
bool app_xiaowei_remote_fvalue(T_XIAOWEI_FVALUE_STORE *fvalue, uint8_t length)
{
    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_XIAOWEI_FVALUE,
                                        APP_REMOTE_MSG_XIAOWEI_FVALUE,
                                        (uint8_t *)fvalue, length);
    return true;
}

static void app_xiaowei_remote_handle_favlue(uint8_t *info_data, uint8_t info_len)
{
    memcpy(app_xiaowei_fvalues, xiaowei_fvalue_get_all(),
           sizeof(T_XIAOWEI_FVALUE_STORE)*XIAOWEI_FVALUE_MAX_NUMBER);

    if (memcmp((uint8_t *)app_xiaowei_fvalues, info_data, info_len))
    {
        xiaowei_fvalue_set_all((T_XIAOWEI_FVALUE_STORE *)info_data);
    }
    else
    {
        // APP_PRINT_INFO0("app_xiaowei_remote_handle_favlue: has in flash");
    }
}

#if F_APP_ERWS_SUPPORT
static void app_xiaowei_fvalue_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                           T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE2("app_xiaowei_fvalue_parse_cback: msg_type 0x%04x, status %d", msg_type, status);

    switch (msg_type)
    {
    case APP_REMOTE_MSG_XIAOWEI_FVALUE:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (extend_app_cfg_const.xiaowei_support && buf != NULL && len != 0)
            {
                app_xiaowei_remote_handle_favlue(buf, len);
            }
        }
        break;

    default:
        break;
    }
}

uint16_t app_xiaowei_fvalue_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;
    APP_PRINT_INFO1("app_xiaowei_fvalue_relay_cback: msg_type %d", msg_type);

    switch (msg_type)
    {
    case APP_REMOTE_MSG_XIAOWEI_FVALUE:
        {
            memcpy(app_xiaowei_fvalues, xiaowei_fvalue_get_all(),
                   sizeof(T_XIAOWEI_FVALUE_STORE)*XIAOWEI_FVALUE_MAX_NUMBER);
            payload_len = sizeof(T_XIAOWEI_FVALUE_STORE) * XIAOWEI_FVALUE_MAX_NUMBER;
            msg_ptr = (uint8_t *)app_xiaowei_fvalues;
            skip = false;
        }
        break;

    default:
        break;
    }

    uint16_t msg_len = payload_len + 4;

    if ((total == true) && (skip == true))
    {
        msg_len = 0;
    }

    if (buf != NULL)
    {
        if (((total == true) && (skip == false)) || (total == false))
        {
            buf[0] = (uint8_t)(msg_len & 0xFF);
            buf[1] = (uint8_t)(msg_len >> 8);
            buf[2] = APP_MODULE_TYPE_XIAOWEI_FVALUE;
            buf[3] = msg_type;
            if (payload_len != 0 && msg_ptr != NULL)
            {
                memcpy(&buf[4], msg_ptr, payload_len);
            }
        }
        APP_PRINT_INFO1("app_xiaowei_fvalue_relay_cback: buf %b", TRACE_BINARY(msg_len, buf));
    }

    return msg_len;
}
#endif

static bool app_xiaowei_relay_init(void)
{
    APP_PRINT_INFO0("app_xiaowei_relay_init");
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_xiaowei_fvalue_relay_cback, app_xiaowei_fvalue_parse_cback,
                             APP_MODULE_TYPE_XIAOWEI_FVALUE, APP_REMOTE_MSG_XIAOWEI_FVALUE_MAX_MSG_NUM);
#endif

    return true;
}

static void app_xiaowei_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_HFP_CALL_STATUS:
        if (param->hfp_call_status.curr_status != BT_HFP_CALL_IDLE)
        {
            T_XIAOWEI_VA_STATE va_state = app_xiaowei_device_get_va_state(param->hfp_call_status.bd_addr);

            if (va_state != XIAOWEI_VA_STATE_IDLE)
            {
                app_xiaowei_stop_recording(param->hfp_call_status.bd_addr);
                app_xiaowei_device_set_va_state(param->hfp_call_status.bd_addr, XIAOWEI_VA_STATE_IDLE);
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_xiaowei_bt_cback: event_type 0x%04x", event_type);
    }
}

void app_xiaowei_bt_mgr_cback_register(void)
{
    bt_mgr_cback_register(app_xiaowei_bt_cback);
}

void app_xiaowei_init(void)
{
    xiaowei_protocol_init();
    app_xiaowei_device_init();
    app_xiaowei_transport_init();
    app_xiaowei_relay_init();
    app_xiaowei_bt_mgr_cback_register();
}
#endif
