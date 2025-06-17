
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if XM_XIAOAI_FEATURE_SUPPORT
#include <string.h>
#include <trace.h>

#include <os_mem.h>
#include <trace.h>

#include <gap.h>
#include <gap_conn_le.h>

#include <app_ble_gap.h>
#include <app_device.h>
#include <app_main.h>
#include <app_cfg.h>
#include "bt_types.h"
#include <xm_xiaoai_ble_service.h>
#include <xm_xiaoai_api.h>
#include <app_xiaoai_transport.h>
#include <app_xiaoai_ota.h>
#include <app_xm_xiaoai.h>
#include <app_xiaoai_rcsp.h>
#include <app_xiaoai_device.h>
#include <app_xiaoai_record.h>
#include <app_multilink.h>
#include "app_sniff_mode.h"

#define XM_XIAOAI_VERSION         0x1001
#define XM_XIAOAI_MAX_DEVICE_NAME_LENGTH    20

bool xm_xiaoai_get_opcode_sn(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                             uint8_t *opcode_sn);

bool xm_xiaoai_get_phone_type(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                              uint8_t *phone_type)
{
    T_XM_XIAOAI_LINK *p_link = app_xiaoai_find_link(type, bd_addr);
    if (p_link)
    {
        *phone_type = p_link->phone_type;
        return true;
    }
    return false;
}

bool app_xm_handle_cmd_get_device_info(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                       uint8_t conn_id,
                                       uint8_t opcode_sn, uint32_t mask)
{
    uint8_t length = 0;
    uint8_t i = 0;
    uint8_t pdu[35 + XM_XIAOAI_MAX_DEVICE_NAME_LENGTH];

    for (i = 0; i < 32; i++)
    {
        switch (mask & ((uint32_t)0x01 << i))
        {
        case XM_ATTR_TYPE_NAME:
            {
                uint8_t device_name_length = strlen((char *)&app_cfg_nv.device_name_legacy);
                if (device_name_length >= XM_XIAOAI_MAX_DEVICE_NAME_LENGTH)
                {
                    device_name_length = XM_XIAOAI_MAX_DEVICE_NAME_LENGTH;
                    APP_PRINT_ERROR2("XM_ATTR_TYPE_NAME: device_name_length cut from %d to %d",
                                     device_name_length, XM_XIAOAI_MAX_DEVICE_NAME_LENGTH);
                }
                pdu[length++] = device_name_length + 1;   //length

                pdu[length++] = i;    //AIVS_ATTR_TYPE

                uint8_t *p = &pdu[length];
                memcpy(p, app_cfg_nv.device_name_legacy, device_name_length);

                length += device_name_length;
            }
            break;

        case XM_ATTR_TYPE_VERSION:
            {
                pdu[length++] = 0x03;   //length

                pdu[length++] = i;    //AIVS_ATTR_TYPE

                pdu[length++] = (uint8_t)((XM_XIAOAI_VERSION & 0xFF00) >> 8);    //version
                pdu[length++] = (uint8_t)((XM_XIAOAI_VERSION & 0x00FF) >> 0);    //version
            }
            break;

        case XM_ATTR_TYPE_BATTERY:
            pdu[length++] = 0x02;   //length
            pdu[length++] = i;    //AIVS_ATTR_TYPE
            pdu[length++] = app_db.local_batt_level;   //battery level 100%
            break;

        case XM_ATTR_TYPE_VID_AND_PID:
            {
                pdu[length++] = 0x05;   //length
                pdu[length++] = i;    //AIVS_ATTR_TYPE

                uint8_t *p = &pdu[length];

                BE_UINT16_TO_ARRAY(p, extend_app_cfg_const.xiaoai_vid);
                length += 2;

                p = &pdu[length];
                BE_UINT16_TO_ARRAY(p, extend_app_cfg_const.xiaoai_pid);
                length += 2;
            }
            break;

        case XM_ATTR_TYPE_EDR_CONNECTION_STATUS:
            {
                uint8_t edr_status = T_XM_CMD_F2A_EDR_STATUS_DISCONNECTED;

                if (type == T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE)
                {
                    if (app_link_find_br_link(bd_addr) != NULL)
                    {
                        edr_status = T_XM_CMD_F2A_EDR_STATUS_CONNECTED;
                    }
                    else
                    {
                        APP_PRINT_ERROR1("XM_ATTR_TYPE_EDR_CONNECTION_STATUS: bd_addr not connected %s",
                                         TRACE_BDADDR(bd_addr));
                    }

                }
                else
                {
                    edr_status = T_XM_CMD_F2A_EDR_STATUS_CONNECTED;
                }

                APP_PRINT_INFO1("app_xm_handle_cmd_get_device_info: AIVS_ATTR_TYPE_EDR_CONNECTION_ON_STAT edr_status %d",
                                edr_status);
                pdu[length++] = 0x02;   //length
                pdu[length++] = i;    //AIVS_ATTR_TYPE
                pdu[length++] = edr_status;    //0:disconnect, 1:connected, 2:unpair
            }
            break;

        case XM_ATTR_TYPE_MANDATORY_UPGRADE_FLAG:
            {
                pdu[length++] = 0x02;   //length

                pdu[length++] = i;    //AIVS_ATTR_TYPE

                pdu[length++] = 0x0;    //0:normal system, 1:uboot system
            }
            break;

        case XM_ATTR_TYPE_UBOOT_VERSION:
            {
                pdu[length++] = 0x05;   //length

                pdu[length++] = i;    //AIVS_ATTR_TYPE

                pdu[length++] = 0x01;    //uboot version
                pdu[length++] = 0x01;    //uboot version
                pdu[length++] = 0x01;    //uboot version
                pdu[length++] = 0x01;    //uboot version
            }
            break;

        case XM_ARRT_TYPE_MULT_BATTERY:
            {
                pdu[length++] = 0x03;   //length

                pdu[length++] = i;    //AIVS_ATTR_TYPE

                if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT)
                {
                    pdu[length++] = app_db.local_batt_level;
                    pdu[length++] = app_db.remote_batt_level;
                }
                else
                {
                    pdu[length++] = app_db.remote_batt_level;
                    pdu[length++] = app_db.local_batt_level;
                }
            }
            break;

        case XM_ATTR_TYPE_CODEC_TYPE:
            {
                pdu[length++] = 0x02;   //length

                pdu[length++] = i;      //AIVS_ATTR_TYPE_CODEC_TYPE

                pdu[length++] = 0x01;   //0:speex(default), 1:opus
            }
            break;

        default:
            break;
        }
    }

    return xm_cmd_get_device_info_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS, opcode_sn, pdu,
                                      length);
}

bool app_xm_handle_cmd_dev_reboot(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                                  uint8_t opcode_sn,
                                  T_XM_CMD_DEV_REBOOT_TYPE reboot_type)
{
    uint8_t status = XM_CMD_STATUS_SUCCESS;

    switch (reboot_type)
    {
    case T_XM_CMD_DEV_REBOOT_TYPE_REBOOT:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                xm_cmd_dev_reboot_rsp(type, bd_addr, conn_id, status, opcode_sn);

                app_device_reboot(100, RESET_ALL);
            }
            else
            {
#if F_APP_ERWS_SUPPORT
                xiaoai_ota_p2s_send_device_reboot(type, bd_addr, conn_id, opcode_sn);
#endif
            }
        }
        break;
    case T_XM_CMD_DEV_REBOOT_TYPE_SHUTODWN:
        {
        }
        break;
    default:
        status = XM_CMD_STATUS_PARAM_ERROR;
        break;
    }
    return true;
}

bool app_xm_handle_cmd_notify_phone_info(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                         uint8_t conn_id,
                                         uint8_t opcode_sn,
                                         T_XM_CMD_PHONE_INFO_TYPE phone_type)
{
    bool ret = true;
    //xm_mgr->phone_type = phone_type;
    xm_cmd_notify_phone_info_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS, opcode_sn);

    if (phone_type == T_XM_CMD_PHONE_INFO_TYPE_IPHONE)
    {
        //TODO: reconnect BREDR

    }
    return ret;
}

bool app_xm_handle_cmd_set_pro_mtu(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                                   uint8_t opcode_sn,
                                   uint16_t mtu)
{
    //xm_mgr->mtu = mtu;
    return xm_cmd_set_pro_mtu_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS, opcode_sn);
}

bool app_xm_handle_cmd_a2f_disconnect_edr(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                          uint8_t conn_id,
                                          uint8_t opcode_sn)
{
    //TODO: disconnect BREDR
    return xm_cmd_a2f_disconnect_edr_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS, opcode_sn);
}

bool app_xm_handle_cmd_set_device_info(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                       uint8_t conn_id,
                                       uint8_t opcode_sn, uint8_t *p_data,
                                       uint16_t len)
{
    // uint8_t data_len = p_data[0];
    uint8_t info_type = p_data[1];
    // uint8_t *p_value = p_data + 1 + (len - sizeof(info_type));
    switch (info_type)
    {
    case XM_ATTR_SET_DEVICE_POWER_MODE:
        {

        }
        break;

    case XM_ATTR_SET_DEVICE_FUNC_KEY:
        {

        }
        break;

    case XM_ATTR_SET_DEVICE_HOT_WORD:
        {

        }
        break;

    case XM_ATTR_SET_DEVICE_SUP_POWER_SAVE_NEW:
        {

        }
        break;

    case XM_ATTR_SET_DEVICE_ANC_MODE:
        {
            //TODO
        }
        break;

    case XM_ATTR_SET_DEVICE_GAME_MODE:
        {
            //TODO
        }
        break;

    case XM_ATTR_SET_DEVICE_AUTO_PLAY:
        {

        }
        break;

    default:
        break;
    }
    return xm_cmd_set_device_info_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS, opcode_sn);
}

bool app_xm_handle_cmd_get_device_run_info(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                           uint8_t conn_id,
                                           uint8_t opcode_sn, uint32_t mask)
{
    uint8_t length = 0;
    uint8_t i = 0;
    uint8_t pdu[50];

    for (i = 0; i < 32; i++)
    {
        switch (mask & ((uint32_t)0x01 << i))
        {
        case XM_ATTR_BRE_MAC:
            {
                pdu[length++] = 0x07;

                pdu[length++] = i;

                if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
                {
                    pdu[length++] = app_db.factory_addr[5];
                    pdu[length++] = app_db.factory_addr[4];
                    pdu[length++] = app_db.factory_addr[3];
                    pdu[length++] = app_db.factory_addr[2];
                    pdu[length++] = app_db.factory_addr[1];
                    pdu[length++] = app_db.factory_addr[0];
                }
                else
                {
                    pdu[length++] = app_cfg_nv.bud_local_addr[5];
                    pdu[length++] = app_cfg_nv.bud_local_addr[4];
                    pdu[length++] = app_cfg_nv.bud_local_addr[3];
                    pdu[length++] = app_cfg_nv.bud_local_addr[2];
                    pdu[length++] = app_cfg_nv.bud_local_addr[1];
                    pdu[length++] = app_cfg_nv.bud_local_addr[0];
                }
            }
            break;

        case XM_ATTR_BLE_MAC:
            {
                pdu[length++] = 0x07;

                pdu[length++] = i;

                if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
                {
                    pdu[length++] = app_db.factory_addr[5];
                    pdu[length++] = app_db.factory_addr[4];
                    pdu[length++] = app_db.factory_addr[3];
                    pdu[length++] = app_db.factory_addr[2];
                    pdu[length++] = app_db.factory_addr[1];
                    pdu[length++] = app_db.factory_addr[0];
                }
                else
                {
                    pdu[length++] = app_cfg_nv.bud_local_addr[5];
                    pdu[length++] = app_cfg_nv.bud_local_addr[4];
                    pdu[length++] = app_cfg_nv.bud_local_addr[3];
                    pdu[length++] = app_cfg_nv.bud_local_addr[2];
                    pdu[length++] = app_cfg_nv.bud_local_addr[1];
                    pdu[length++] = app_cfg_nv.bud_local_addr[0];
                }
            }
            break;

        case XM_ATTR_COMMUNICATION_MAX_MTU:
            {
                pdu[length++] = 0x03;

                pdu[length++] = i;

                pdu[length++] = 0x00;
                pdu[length++] = 0xF7;
            }
            break;

        case XM_ATTR_CLASSIC_BLUETOOTH_STATUS:
            {
                uint8_t edr_status = T_XM_CMD_F2A_EDR_STATUS_DISCONNECTED;

                if (type == T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE)
                {
                    if (app_link_find_br_link(bd_addr) != NULL)
                    {
                        edr_status = T_XM_CMD_F2A_EDR_STATUS_CONNECTED;
                    }
                    else
                    {
                        APP_PRINT_ERROR1("XM_ATTR_CLASSIC_BLUETOOTH_STATUS: bd_addr not connected %s",
                                         TRACE_BDADDR(bd_addr));
                    }

                }
                else
                {
                    edr_status = T_XM_CMD_F2A_EDR_STATUS_CONNECTED;
                }

                APP_PRINT_INFO1("app_xm_handle_cmd_get_device_run_info: XM_ATTR_CLASSIC_BLUETOOTH_STATUS edr_status %d",
                                edr_status);

                pdu[length++] = 0x02;

                pdu[length++] = i;

                pdu[length++] = edr_status;
            }
            break;

        case XM_ATTR_GET_POWER_MODE:
            {
                pdu[length++] = 0x02;

                pdu[length++] = i;

                pdu[length++] = 0x0;
            }
            break;

        case XM_ATTR_VENDOR_DATA:
            {
            }
            break;

        case XM_ATTR_TWS_STATUS:
            {
#if 0
                pdu[length++] = 0x02;

                pdu[length++] = i;

                pdu[length++] = 0x0;
#endif
            }
            break;

        case XM_ATTR_DEVICE_VIRTUAL_ADDRESS:
            {
            }
            break;

        case XM_ATTR_TYPE_DONGLE_STATUS:
            {
            }
            break;

        case XM_ATTR_TYPE_GET_ANC_STATUS:
            {
                //TODO
            }
            break;

        case XM_ATTR_DEVICE_AUTO_PLAY:
            {
            }
            break;

        case XM_ATTR_DEVICE_GAME_MODE:
            {
                //TODO
            }
            break;

        default:
            break;
        }
    }

    return xm_cmd_get_device_run_info_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS, opcode_sn, pdu,
                                          length);
}

bool app_xm_handle_cmd_notify_communication_way(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                                uint8_t conn_id,
                                                uint8_t opcode_sn, T_XM_CMD_COMM_WAY_TYPE com_way)
{
    return xm_cmd_notify_communication_way_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS,
                                               opcode_sn);
}

bool app_xm_handle_cmd_wakeup_classic_bluetooth(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                                uint8_t conn_id,
                                                uint8_t opcode_sn)
{
    return xm_cmd_wakeup_classic_bluetooth_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS,
                                               opcode_sn);
}

bool app_xm_handle_cmd_notify_phone_virtual_addr(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                                 uint8_t conn_id,
                                                 uint8_t opcode_sn, uint32_t virtual_addr)
{
    //xm_mgr->virtual_addr = virtual_addr;
    return xm_cmd_notify_phone_virtual_addr_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS,
                                                opcode_sn);
}

bool app_xm_handle_cmd_notify_unbound(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                      uint8_t conn_id,
                                      uint8_t opcode_sn)
{
    //TODO: Add remove bond, and enter pairing mode
    return xm_cmd_notify_unbound_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS, opcode_sn);
}

bool app_xm_handle_cmd_notify_device_run_info(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                              uint8_t conn_id,
                                              uint8_t opcode_sn)
{
    return xm_cmd_notify_device_run_info_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS, opcode_sn);
}

bool app_xm_handle_cmd_speech_start(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                                    uint8_t opcode_sn, uint8_t flag)
{
    bool ret = true;
    T_XM_CMD_STATUS status = XM_CMD_STATUS_SUCCESS;

    status = XM_CMD_STATUS_SUCCESS;
    ret = xm_cmd_speech_start_rsp(type, bd_addr, conn_id, status, opcode_sn);
    if (!ret)
    {
        APP_PRINT_ERROR2("app_xiaoai_device_va_start: xm_cmd_speech_start failed, ret %d, status %d", ret,
                         status);
    }

    return app_xiaoai_device_va_start(bd_addr, false, true);
}

bool app_xm_handle_cmd_speech_stop(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                                   uint8_t opcode_sn)
{
    return xm_cmd_speech_stop_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS, opcode_sn);
}

bool app_xm_handle_cmd_set_device_config(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                         uint8_t conn_id,
                                         uint8_t opcode_sn, uint8_t *p_data, uint8_t len)
{
    uint8_t param_length = len;
    uint8_t offset = 0;
    APP_PRINT_INFO2("app_xm_handle_cmd_set_device_config: len %d param2 %b", param_length,
                    TRACE_BINARY(param_length, p_data));
    while (offset < param_length)
    {
        T_XM_DEVICE_CONFIG_TLV config_tlv;

        config_tlv.length = *(p_data + offset);
        offset++;
        BE_ARRAY_TO_UINT16(config_tlv.type, p_data + offset);
        offset += 2;

        memcpy(config_tlv.value, p_data + offset, config_tlv.length - 2);
        APP_PRINT_INFO1("app_xm_handle_cmd_set_device_config: value %b", TRACE_BINARY(config_tlv.length - 2,
                                                                                      config_tlv.value));

        switch (config_tlv.type)
        {
        case XM_CONFIG_AUDIO_MODE:
            {

            }
            break;

        case XM_CONFIG_CUSTOM_KEY:
            {

            }
            break;

        case XM_CONFIG_AUTO_ANSWER:
            {

            }
            break;

        case XM_CONFIG_MULTILINK:
            {

            }
            break;

        case XM_CONFIG_EAR_FITTING_TEST:
            {

            }
            break;

        case XM_CONFIG_EAR_FITTING_RESULT:
            {

            }
            break;

        case XM_CONFIG_EQ_MODE:
            {

            }
            break;

        case XM_CONFIG_DEVICE_NAME:
            {
                uint8_t device_name_length = config_tlv.length - 2;
                if (device_name_length >= sizeof(config_tlv.value))
                {
                    device_name_length = sizeof(config_tlv.value) - 1;
                }
                config_tlv.value[device_name_length] = 0;
                device_name_length = strlen((char *)config_tlv.value);

                memset(app_cfg_nv.device_name_le, 0, GAP_DEVICE_NAME_LEN);
                memcpy(app_cfg_nv.device_name_le, config_tlv.value, device_name_length);
                app_cfg_nv.device_name_le[device_name_length] = 0;
                memset(app_cfg_nv.device_name_legacy, 0, GAP_DEVICE_NAME_LEN);
                memcpy(app_cfg_nv.device_name_legacy, config_tlv.value, device_name_length);
                app_cfg_nv.device_name_legacy[device_name_length] = 0;

                app_cfg_store(app_cfg_nv.device_name_legacy, 40);
                app_cfg_store(app_cfg_nv.device_name_le, 40);

                if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SINGLE)
                {
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DEVICE, APP_REMOTE_MSG_LE_NAME_SYNC,
                                                        (uint8_t *)config_tlv.value, device_name_length);
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DEVICE, APP_REMOTE_MSG_DEVICE_NAME_SYNC,
                                                        (uint8_t *)config_tlv.value, device_name_length);
                }
            }
            break;

        default:
            break;
        }
        offset += (config_tlv.length - 2);
    }
    return xm_cmd_set_device_config_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS, opcode_sn);
}

bool app_xm_handle_cmd_get_device_config(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                         uint8_t conn_id,
                                         uint8_t opcode_sn, uint8_t *p_data, uint8_t len)
{
    APP_PRINT_INFO0("app_xm_handle_cmd_get_device_config");
    uint8_t offset = 0;
    uint8_t rsp_length = 0;
    uint8_t pdu[50];
    while (offset < len)
    {
        uint16_t config_type;
        BE_ARRAY_TO_UINT16(config_type, &p_data[offset]);
        switch (config_type)
        {
        case XM_CONFIG_AUDIO_MODE:
            {
                pdu[rsp_length++] = 0x03;   //length

                pdu[rsp_length++] = (uint8_t)((config_type & 0xFF00) >> 8);    //config_type
                pdu[rsp_length++] = (uint8_t)((config_type & 0x00FF) >> 0);    //config_type

                pdu[rsp_length++] = 1; //TODO
            }
            break;

        case XM_CONFIG_CUSTOM_KEY:
            {
                pdu[rsp_length++] = 0x03; //length

                pdu[rsp_length++] = (uint8_t)((config_type & 0xFF00) >> 8);    //config_type
                pdu[rsp_length++] = (uint8_t)((config_type & 0x00FF) >> 0);    //config_type

                uint8_t gesture_kk = 00; //TODO
                uint8_t gesture_ll = 00;
                uint8_t gesture_rr = 00;
                pdu[rsp_length++] = gesture_kk;
                pdu[rsp_length++] = gesture_ll;
                pdu[rsp_length++] = gesture_rr;
            }
            break;

        case XM_CONFIG_AUTO_ANSWER:
            {
                pdu[rsp_length++] = 0x03; //length

                pdu[rsp_length++] = (uint8_t)((config_type & 0xFF00) >> 8);    //config_type
                pdu[rsp_length++] = (uint8_t)((config_type & 0x00FF) >> 0);    //config_type

                pdu[rsp_length++] = 1; //TODO
            }
            break;

        case XM_CONFIG_MULTILINK:
            {
                pdu[rsp_length++] = 0x03; //length

                pdu[rsp_length++] = (uint8_t)((config_type & 0xFF00) >> 8);    //config_type
                pdu[rsp_length++] = (uint8_t)((config_type & 0x00FF) >> 0);    //config_type

                pdu[rsp_length++] = app_cfg_const.enable_multi_link;
            }
            break;

        case XM_CONFIG_EAR_FITTING_TEST:
            {
                pdu[rsp_length++] = 0x03; //length

                pdu[rsp_length++] = (uint8_t)((config_type & 0xFF00) >> 8);    //config_type
                pdu[rsp_length++] = (uint8_t)((config_type & 0x00FF) >> 0);    //config_type

                pdu[rsp_length++] = 0x01;
            }
            break;

        case XM_CONFIG_EAR_FITTING_RESULT:
            {
                pdu[rsp_length++] = 0x03; //length

                pdu[rsp_length++] = (uint8_t)((config_type & 0xFF00) >> 8);    //config_type
                pdu[rsp_length++] = (uint8_t)((config_type & 0x00FF) >> 0);    //config_type

                pdu[rsp_length++] = 0x00; //TODO
            }
            break;

        case XM_CONFIG_EQ_MODE:
            {
                pdu[rsp_length++] = 0x03; //length

                pdu[rsp_length++] = (uint8_t)((config_type & 0xFF00) >> 8);    //config_type
                pdu[rsp_length++] = (uint8_t)((config_type & 0x00FF) >> 0);    //config_type

                pdu[rsp_length++] = 0x00; //TODO
            }
            break;

        case XM_CONFIG_DEVICE_NAME:
            {
                uint8_t device_name_len = strlen((const char *)app_cfg_nv.device_name_le);

                pdu[rsp_length++] = 2 + device_name_len; //length

                pdu[rsp_length++] = (uint8_t)((config_type & 0xFF00) >> 8);    //config_type
                pdu[rsp_length++] = (uint8_t)((config_type & 0x00FF) >> 0);    //config_type

                uint8_t *p_name = &pdu[rsp_length];
                memcpy(p_name, app_cfg_nv.device_name_le, device_name_len);

                rsp_length += device_name_len;
            }
            break;

        default:
            break;
        }
        offset += 2;
    }
    return xm_cmd_get_device_config_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS, opcode_sn, pdu,
                                        rsp_length);
}

static void app_xm_rx_handle_cmd(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                                 uint8_t opcode,
                                 bool req_rsp, uint8_t *p_data, uint16_t len)
{
    uint8_t opcode_sn = p_data[0];
    uint8_t *param2 = &p_data[1];

    switch (opcode)
    {
    case XM_OPCODE_GET_DEVICE_INFO:
        {

            uint32_t mask = 0;

            BE_ARRAY_TO_UINT32(mask, param2);
            app_xm_handle_cmd_get_device_info(type, bd_addr, conn_id, opcode_sn, mask);
        }
        break;

    case XM_OPCODE_DEV_REBOOT:
        {
            T_XM_CMD_DEV_REBOOT_TYPE reboot_type = (T_XM_CMD_DEV_REBOOT_TYPE)param2[0];
            app_xm_handle_cmd_dev_reboot(type, bd_addr, conn_id, opcode_sn, reboot_type);
        }
        break;

    case XM_OPCODE_NOTIFY_PHONE_INFO:
        {
            uint8_t phone_type = param2[0];

            if ((phone_type != T_XM_CMD_PHONE_INFO_TYPE_XIAOMI) &&
                (phone_type != T_XM_CMD_PHONE_INFO_TYPE_IPHONE))
            {
                phone_type = T_XM_CMD_PHONE_INFO_TYPE_ANDROID;
            }

            app_xm_handle_cmd_notify_phone_info(type, bd_addr, conn_id, opcode_sn,
                                                (T_XM_CMD_PHONE_INFO_TYPE)phone_type);
        }
        break;

    case XM_OPCODE_SET_PRO_MTU:
        {
            uint16_t mtu = 0;
            BE_ARRAY_TO_UINT16(mtu, param2);
            app_xm_handle_cmd_set_pro_mtu(type, bd_addr, conn_id, opcode_sn, mtu);
        }
        break;

    case XM_OPCODE_A2F_DISCONNECT_EDR:
        {
            app_xm_handle_cmd_a2f_disconnect_edr(type, bd_addr, conn_id, opcode_sn);
        }
        break;

    case XM_OPCODE_SET_DEVICE_INFO:
        {
            app_xm_handle_cmd_set_device_info(type, bd_addr, conn_id, opcode_sn, param2,
                                              len - sizeof(opcode_sn));
        }
        break;

    case XM_OPCODE_GET_DEVICE_RUN_INFO:
        {
            uint32_t mask = 0;
            BE_ARRAY_TO_UINT32(mask, param2);
            app_xm_handle_cmd_get_device_run_info(type, bd_addr, conn_id, opcode_sn, mask);

            T_APP_BR_LINK *p_link = app_link_find_br_link(bd_addr);
            if (p_link != NULL)
            {
                xm_cmd_f2a_edr_status(type, bd_addr, conn_id, T_XM_CMD_F2A_EDR_STATUS_CONNECTED);
            }
        }
        break;

    case XM_OPCODE_NOTIFY_COMMUNICATION_WAY:
        {
            T_XM_CMD_COMM_WAY_TYPE comm_way = (T_XM_CMD_COMM_WAY_TYPE)param2[0];
            app_xm_handle_cmd_notify_communication_way(type, bd_addr, conn_id, opcode_sn, comm_way);
        }
        break;

    case XM_OPCODE_WAKEUP_CLASSIC_BLUETOOTH:
        {
            uint8_t phone_type;
            app_xm_handle_cmd_wakeup_classic_bluetooth(type, bd_addr, conn_id, opcode_sn);

            if (xm_xiaoai_get_phone_type(type, bd_addr, conn_id, &phone_type))
            {
                if (phone_type == T_XM_CMD_PHONE_INFO_TYPE_IPHONE)
                {
                    xm_cmd_f2a_edr_status(type, bd_addr, conn_id, T_XM_CMD_F2A_EDR_STATUS_CONNECTED);
                }
                else
                {
                    xm_cmd_notify_f2a_bt_op(type, bd_addr, conn_id, T_XM_CMD_F2A_BT_OP_RECONNECT_CLASSIC);
                }
            }
        }
        break;

    case XM_OPCODE_NOTIFY_PHONE_VIRTUAL_ADDR:
        {
            uint32_t virtual_addr = 0;
            BE_ARRAY_TO_UINT32(virtual_addr, param2);
            app_xm_handle_cmd_notify_phone_virtual_addr(type, bd_addr, conn_id, opcode_sn, virtual_addr);
        }
        break;

    case XM_OPCODE_NOTIFY_UNBOUND:
        {
            app_xm_handle_cmd_notify_unbound(type, bd_addr, conn_id, opcode_sn);
        }
        break;

    case XM_OPCODE_NOTIFY_DEVICE_RUN_INFO:
        {
            app_xm_handle_cmd_notify_device_run_info(type, bd_addr, conn_id, opcode_sn);
        }
        break;

    case XM_OPCODE_SPEECH_START:
        {
            uint8_t flag = param2[0];
            app_xm_handle_cmd_speech_start(type, bd_addr, conn_id, opcode_sn, flag);
        }
        break;

    case XM_OPCODE_SPEECH_STOP:
        {
            APP_PRINT_INFO0("app_xm_rx_handle_cmd: remote stop speech");
            app_sniff_mode_b2s_enable(bd_addr, SNIFF_DISABLE_MASK_VA);
            app_xiaoai_stop_recording(bd_addr);
            app_xiaoai_device_set_va_state(bd_addr, VA_STATE_IDLE);
            app_xm_handle_cmd_speech_stop(type, bd_addr, conn_id, opcode_sn);
        }
        break;

    case XM_OPCODE_OTA_GET_DEVICE_UPDATE_FILEINFO_OFFSET:
    case XM_OPCODE_OTA_INQUIRE_DEVICE_IF_CAN_UPDATE:
    case XM_OPCODE_OTA_ENTER_UPDATE_MODE:
    case XM_OPCODE_OTA_EXIT_UPDATE_MODE:
    case XM_OPCODE_OTA_SEND_FIRMWARE_UPDATE_BLOCK:
    case XM_OPCODE_OTA_GET_DEVICE_REFRESH_FIRMWARE_STATUS:
    case XM_OPCODE_OTA_NOTIFY_UPDATE_MODE:
    case XM_OPCODE_OTA_RESERVED:
        {
            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
#if (F_APP_ERWS_SUPPORT == 0)
                xiaoai_ota_single_handle_cmd(type, bd_addr, conn_id, opcode, req_rsp, p_data, len);
#endif
            }
            else
            {
#if F_APP_ERWS_SUPPORT
                xiaoai_ota_handle_cmd(type, bd_addr, conn_id, opcode, req_rsp, p_data, len);
#endif
            }
        }
        break;

    case XM_OPCODE_SET_DEVICE_CONFIG:
        {
            uint8_t param_length = len - 1;
            APP_PRINT_INFO2("XM_OPCODE_SET_DEVICE_CONFIG: len %d param2 %b", param_length,
                            TRACE_BINARY(param_length, param2));
            app_xm_handle_cmd_set_device_config(type, bd_addr, conn_id, opcode_sn, param2, param_length);
        }
        break;

    case XM_OPCODE_GET_DEVICE_CONFIG:
        {
            uint8_t config_type_data_len = len - 1;
            app_xm_handle_cmd_get_device_config(type, bd_addr, conn_id, opcode_sn, param2,
                                                config_type_data_len);
        }
        break;

    default:
        {
            APP_PRINT_ERROR1("app_xm_rx_handle_cmd: unhandled opcode 0x%02x", opcode);
        }
        break;
    }
}

void app_xm_handle_cmd_rsp_f2a_edr_status(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                          uint8_t conn_id)
{

}

void app_xm_handle_cmd_rsp_notify_f2a_bt_op(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                            uint8_t conn_id,
                                            T_XM_CMD_F2A_BT_OP_TYPE bt_op)
{
    switch (bt_op)
    {
    case T_XM_CMD_F2A_BT_OP_DISCONNECT_A2DP_HFP:
        break;

    case T_XM_CMD_F2A_BT_OP_RECONNECT_CLASSIC:
        break;

    default:
        break;
    }

}

void app_xm_handle_cmd_rsp_report_device_status(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                                uint8_t conn_id)
{

}

void app_xm_handle_cmd_rsp_speech_start(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t flag)
{
    app_xiaoai_start_recording(bd_addr);
    app_xiaoai_device_set_va_state(bd_addr, VA_STATE_ACTIVE);
}

void app_xm_handle_cmd_rsp_speech_stop(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr)
{
    app_xiaoai_stop_recording(bd_addr);
    app_xiaoai_device_set_va_state(bd_addr, VA_STATE_IDLE);
}

void app_xm_handle_cmd_rsp_speech_cancel(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr)
{

}

void app_xm_handle_cmd_rsp_speech_long_hold(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr)
{

}

static void app_xm_rx_handle_cmd_rsp(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                                     uint8_t opcode,
                                     bool req_rsp,
                                     uint8_t *p_data, uint16_t len)
{

    uint8_t status = p_data[0];
    uint8_t opcode_sn = p_data[1];
    uint8_t cmd_opcode_sn = 0;

    xm_xiaoai_get_opcode_sn(type, bd_addr, conn_id, &cmd_opcode_sn);

    if (status != XM_CMD_STATUS_SUCCESS || opcode_sn != cmd_opcode_sn)
    {
        APP_PRINT_ERROR3("app_xm_rx_handle_cmd_rsp: status %d, opcode_sn %d, cmd_opcode_sn %d", status,
                         opcode_sn, cmd_opcode_sn);
    }

    APP_PRINT_INFO1("app_xm_rx_handle_cmd_rsp: opcode 0x%02x", opcode);

    switch (opcode)
    {
    case XM_OPCODE_F2A_EDR_STATUS:
        {
            app_xm_handle_cmd_rsp_f2a_edr_status(type, bd_addr, conn_id);
        }
        break;

    case XM_OPCODE_NOTIFY_F2A_BT_OP:
        {
            T_XM_CMD_F2A_BT_OP_TYPE bt_op = (T_XM_CMD_F2A_BT_OP_TYPE)p_data[2];
            app_xm_handle_cmd_rsp_notify_f2a_bt_op(type, bd_addr, conn_id, bt_op);
        }
        break;

    case XM_OPCODE_REPORT_DEVICE_STATUS:
        {
            app_xm_handle_cmd_rsp_report_device_status(type, bd_addr, conn_id);
        }
        break;

    case XM_OPCODE_SPEECH_START:
        {
            app_xm_handle_cmd_rsp_speech_start(type, bd_addr, 0);
        }
        break;

    case XM_OPCODE_SPEECH_STOP:
        {
            app_xm_handle_cmd_rsp_speech_stop(type, bd_addr);
        }
        break;

    case XM_OPCODE_SPEECH_CANCEL:
        {
            app_xm_handle_cmd_rsp_speech_cancel(type, bd_addr);
        }
        break;

    case XM_OPCODE_SPEECH_LONG_HOLD:
        {
            app_xm_handle_cmd_rsp_speech_long_hold(type, bd_addr);
        }
        break;

    default:
        {
            APP_PRINT_ERROR1("app_xm_rx_handle_cmd_rsp: unhandled opcode 0x%02x", opcode);
        }
        break;
    }
}

void app_xm_xiaoai_init(void)
{
    xm_xiaoai_init(app_xm_rx_handle_cmd, app_xm_rx_handle_cmd_rsp, xm_rx_access_auth_pkt, 2, 2);

    app_xiaoai_device_init();
    app_xiaoai_transport_init();
    app_xiaoai_record_init();
    xiaoai_ota_init();
}

void app_xm_xiaoai_handle_conn_state(uint8_t conn_id, T_GAP_CONN_STATE new_state)
{
    switch (new_state)
    {
    case GAP_CONN_STATE_DISCONNECTED:
        {
            uint8_t bd_addr[6];
            uint8_t bd_type;
            le_get_conn_addr(conn_id, bd_addr, &bd_type);
            app_xiaoai_transport_delete(T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE, bd_addr);
        }
        break;
    case GAP_CONN_STATE_CONNECTED:
        {
#if 0
            uint8_t bd_addr[6];
            uint8_t bd_type;
            le_get_conn_addr(conn_id, bd_addr, &bd_type);
            app_xiaoai_handle_b2s_ble_connected(bd_addr, conn_id);
#endif
        }
        break;
    default:
        break;
    }
}
#endif
