
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if XM_XIAOAI_FEATURE_SUPPORT
#include <string.h>
#include <trace.h>

#include <xiaoai_mem.h>
#include <trace.h>

#include "app_cfg.h"
#include "app_device.h"
#include "app_ota.h"
#include <app_xiaoai_ota.h>
#include <xm_xiaoai_ble_service.h>
#include <xm_xiaoai_api.h>
#include <xm_xiaoai_bt.h>
#include "app_xiaoai_transport.h"
#include "app_xiaoai_device.h"
#include "app_sniff_mode.h"
#include "app_relay.h"
#include "app_auto_power_off.h"

#include <gap.h>

#if F_APP_ERWS_SUPPORT
static T_XIAOAI_OTA_MGR xiaoai_ota_mgr;
#else
static T_XIAOAI_OTA_SINGLE_MGR xiaoai_ota_single_mgr;
#endif

typedef enum
{
    XIAOAI_OTA_P2S_MSG_IMAGE_HEADER_AND_BIN0_INFO = 0x00,
    XIAOAI_OTA_P2S_MSG_BIN1_INFO = 0x01,

    XIAOAI_OTA_P2S_NOTIFY_IMAGE_RX_COMPLETE = 0x10,
    XIAOAI_OTA_P2S_MSG_FIRMWARE_UPDATE_BLOCK,
    XIAOAI_OTA_P2S_MSG_DEVICE_REBOOT,
    XIAOAI_OTA_P2S_MSG_ACK,
    XIAOAI_OTA_P2S_MSG_OTA_START,

    XIAOAI_OTA_S2P_NOTIFY_IMAGE_RX_COMPLETE = 0x20,
    XIAOAI_OTA_S2P_MSG_FIRMWARE_UPDATE_BLOCK_RSP,
    XIAOAI_OTA_S2P_MSG_ACK,

    XIAOAI_OTA_REMOTE_MSG_NUM,
} T_XIAOAI_OTA_REMOTE_MSG_ID;

typedef struct
{
    uint16_t mtu;
    T_COMBINATION_OTA_IMAGE_HEADER combine_head;
    T_SUB_BIN_INFO sub_bin0;
} T_XIAOAI_OTA_P2S_MSG_IMAGE_HEADER_AND_BIN0_INFO;

typedef struct
{
    T_SUB_BIN_INFO sub_bin1;
} T_XIAOAI_OTA_P2S_MSG_IMAGE_BIN1_INFO;
typedef struct
{
    T_XM_CMD_COMM_WAY_TYPE type;
    uint8_t bd_addr[6];
    uint8_t conn_id;
    uint8_t status;
    uint8_t opcode_sn;
    uint8_t update_result;
} T_XIAOAI_OTA_P2S_MSG_NOTIFY_IMAGE_RX_COMPLETE;

typedef struct
{
    T_XM_CMD_COMM_WAY_TYPE type;
    uint8_t bd_addr[6];
    uint8_t conn_id;
    uint8_t opcode;
    bool req_rsp;
    uint16_t len;
    uint8_t data[0];
} T_XIAOAI_OTA_P2S_MSG_FIRMWARE_UPDATE_BLOCK;

typedef struct
{
    T_XM_CMD_COMM_WAY_TYPE type;
    uint8_t bd_addr[6];
    uint8_t conn_id;
    uint8_t opcode_sn;
} T_XIAOAI_OTA_P2S_MSG_DEVICE_REBOOT;

typedef struct
{
    T_XIAOAI_OTA_REMOTE_MSG_ID msg_id_acked;
} T_XIAOAI_OTA_P2S_MSG_ACK;

typedef struct
{
    T_XIAOAI_OTA_REMOTE_MSG_ID msg_id_ota_start;
} T_XIAOAI_OTA_P2S_MSG_OTA_START;

typedef struct
{
    T_XM_CMD_COMM_WAY_TYPE type;
    uint8_t bd_addr[6];
    uint8_t conn_id;
    uint8_t status;
    uint8_t opcode_sn;
    uint8_t update_result;
} T_XIAOAI_OTA_S2P_MSG_NOTIFY_IMAGE_RX_COMPLETE;

typedef struct
{
    T_XM_CMD_COMM_WAY_TYPE type;
    uint8_t bd_addr[6];
    uint8_t conn_id;
    uint8_t status;
    uint8_t opcode_sn;
    uint8_t update_result;
    uint32_t next_block_offset;
    uint16_t next_block_length;
    uint16_t delay_ms;
} T_XIAOAI_OTA_S2P_MSG_FIRMWARE_UPDATE_BLOCK_RSP;

typedef struct
{
    T_XIAOAI_OTA_REMOTE_MSG_ID msg_id_acked;
    union
    {
        T_XIAOAI_OTA_P2S_MSG_DEVICE_REBOOT dev_reboot;
    } msg;

} T_XIAOAI_OTA_S2P_MSG_ACK;

typedef union _T_XIAOAI_OTA_REMOTE_MSG
{
    T_XIAOAI_OTA_P2S_MSG_IMAGE_HEADER_AND_BIN0_INFO p2s_image_header_and_bin0_info;
    T_XIAOAI_OTA_P2S_MSG_IMAGE_BIN1_INFO            p2s_image_bin1_info;
    T_XIAOAI_OTA_P2S_MSG_NOTIFY_IMAGE_RX_COMPLETE   p2s_notify_rx_image_complete;
    T_XIAOAI_OTA_P2S_MSG_FIRMWARE_UPDATE_BLOCK      p2s_firmware_update_block;
    T_XIAOAI_OTA_P2S_MSG_DEVICE_REBOOT              p2s_device_reboot;
    T_XIAOAI_OTA_P2S_MSG_ACK                        p2s_ack;
    T_XIAOAI_OTA_P2S_MSG_OTA_START                  p2s_ota_start;

    T_XIAOAI_OTA_S2P_MSG_NOTIFY_IMAGE_RX_COMPLETE   s2p_notify_rx_image_complete;
    T_XIAOAI_OTA_S2P_MSG_FIRMWARE_UPDATE_BLOCK_RSP  s2p_firmware_update_block_rsp;
    T_XIAOAI_OTA_S2P_MSG_ACK                        s2p_ack;
} T_XIAOAI_OTA_REMOTE_MSG;

void xiaoai_ota_p2s_send_image_header_and_bin0_info(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                                    uint8_t conn_id);

void xiaoai_ota_p2s_send_bin1_info(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                   uint8_t conn_id);

void xiaoai_ota_s2p_send_notify_image_rx_complete(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                                  uint8_t conn_id,
                                                  uint8_t status, uint8_t opcode_sn, uint8_t update_result);

void xiaoai_ota_p2s_send_notify_image_rx_complete(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                                  uint8_t conn_id,
                                                  uint8_t status, uint8_t opcode_sn, uint8_t result);

void xiaoai_ota_p2s_send_firmware_update_block(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                               uint8_t conn_id,
                                               uint8_t opcode, bool req_rsp, uint8_t *p_data, uint16_t len);

void xiaoai_ota_s2p_send_firmware_update_block_rsp(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                                   uint8_t conn_id,
                                                   uint8_t status, uint8_t opcode_sn, uint8_t update_result,
                                                   uint32_t next_block_offset, uint16_t next_block_length, uint16_t delay_ms);

void xiaoai_ota_p2s_send_ota_start(T_XIAOAI_OTA_REMOTE_MSG_ID msg_id_ota_start);

static void xiaoai_ota_get_ota_mtu_size(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                        uint8_t conn_id,
                                        uint16_t *mtu)
{
    app_xiaoai_device_get_mtu(type, bd_addr, conn_id, mtu);

    if (*mtu + sizeof(T_XIAOAI_OTA_P2S_MSG_FIRMWARE_UPDATE_BLOCK) > APP_RELAY_MTU)
    {
        *mtu = APP_RELAY_MTU - sizeof(
                   T_XIAOAI_OTA_P2S_MSG_FIRMWARE_UPDATE_BLOCK); //final block sent to sec has a header
        *mtu = *mtu & 0xFFFC;
    }
}

static uint8_t count_one(uint8_t num)
{
    uint8_t count;
    for (count = 0; num != 0; count++)
    {
        num &= (num - 1);
    }
    return count;
}

static uint8_t xiaoai_ota_get_sub_image_header_count(uint8_t *p_data, uint8_t len)
{
    uint8_t file_count = 0;
    for (uint8_t i = 0; i < len; i++)
    {
        file_count += count_one(*(p_data + i));
    }

    return file_count;
}


static void xiaoai_ota_handle_sub_image_header(uint8_t sub_bin_index, uint8_t *p_data, uint8_t len)
{
    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
#if (F_APP_ERWS_SUPPORT == 0)
        for (uint8_t i = 0; i < xiaoai_ota_single_mgr.sub_bin.merged_header.sub_file_count; i++)
        {
            LE_STREAM_TO_UINT32(xiaoai_ota_single_mgr.sub_bin.sub_image_header[i].download_addr,
                                p_data);
            LE_STREAM_TO_UINT32(xiaoai_ota_single_mgr.sub_bin.sub_image_header[i].size, p_data);
            LE_STREAM_TO_UINT32(xiaoai_ota_single_mgr.sub_bin.sub_image_header[i].reserved, p_data);
            if (i == 0)
            {
                if (sub_bin_index == 0)
                {
                    xiaoai_ota_single_mgr.sub_bin.sub_image_header[i].start_file_offset =
                        OTA_MERGED_FILE_HEAD_SIZE +
                        OTA_MERGED_FILE_SUB_HEAD_SIZE * xiaoai_ota_single_mgr.sub_bin.merged_header.sub_file_count;
                }
            }
            else
            {
                xiaoai_ota_single_mgr.sub_bin.sub_image_header[i].start_file_offset =
                    xiaoai_ota_single_mgr.sub_bin.sub_image_header[i -
                                                                   1].start_file_offset + xiaoai_ota_single_mgr.sub_bin.sub_image_header[i - 1].size;
            }
            APP_PRINT_INFO3("xiaoai_ota_handle_sub_image_header: single, download_addr %p, size 0x%08x, start_file_offset 0x%08x",
                            xiaoai_ota_single_mgr.sub_bin.sub_image_header[i].download_addr,
                            xiaoai_ota_single_mgr.sub_bin.sub_image_header[i].size,
                            xiaoai_ota_single_mgr.sub_bin.sub_image_header[i].start_file_offset
                           );
        }
#endif
    }
    else
    {
#if F_APP_ERWS_SUPPORT
        for (uint8_t i = 0; i < xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.sub_file_count; i++)
        {
            LE_STREAM_TO_UINT32(xiaoai_ota_mgr.sub_bin[sub_bin_index].sub_image_header[i].download_addr,
                                p_data);
            LE_STREAM_TO_UINT32(xiaoai_ota_mgr.sub_bin[sub_bin_index].sub_image_header[i].size, p_data);
            LE_STREAM_TO_UINT32(xiaoai_ota_mgr.sub_bin[sub_bin_index].sub_image_header[i].reserved, p_data);
            if (i == 0)
            {
                if (sub_bin_index == 0)
                {
                    xiaoai_ota_mgr.sub_bin[sub_bin_index].sub_image_header[i].start_file_offset =
                        OTA_COMBINE_HEAD_SIZE + OTA_MERGED_FILE_HEAD_SIZE +
                        OTA_MERGED_FILE_SUB_HEAD_SIZE * xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.sub_file_count;

                }
                else
                {
                    xiaoai_ota_mgr.sub_bin[sub_bin_index].sub_image_header[i].start_file_offset =
                        OTA_COMBINE_HEAD_SIZE + xiaoai_ota_mgr.combine_head.bin_info[0].length + OTA_MERGED_FILE_HEAD_SIZE +
                        OTA_MERGED_FILE_SUB_HEAD_SIZE * xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.sub_file_count;
                }
            }
            else
            {
                xiaoai_ota_mgr.sub_bin[sub_bin_index].sub_image_header[i].start_file_offset =
                    xiaoai_ota_mgr.sub_bin[sub_bin_index].sub_image_header[i -
                                                                           1].start_file_offset + xiaoai_ota_mgr.sub_bin[sub_bin_index].sub_image_header[i - 1].size;
            }
            APP_PRINT_INFO4("xiaoai_ota_handle_sub_image_header: sub_bin_index %d, download_addr %p, size 0x%08x, start_file_offset 0x%08x",
                            sub_bin_index,
                            xiaoai_ota_mgr.sub_bin[sub_bin_index].sub_image_header[i].download_addr,
                            xiaoai_ota_mgr.sub_bin[sub_bin_index].sub_image_header[i].size,
                            xiaoai_ota_mgr.sub_bin[sub_bin_index].sub_image_header[i].start_file_offset
                           );
        }
#endif
    }
}

void xiaoai_ota_update_complete_check(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                      uint8_t conn_id,
                                      uint8_t status, uint8_t opcode_sn)
{
    APP_PRINT_INFO0("xiaoai_ota_update_complete_check");

    uint32_t next_block_offset = 0;
    uint16_t next_block_length = 0;
    uint16_t delay_ms = 0;
    uint8_t  update_result = 0;
#if (F_APP_ERWS_SUPPORT == 0)
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {

        if (xiaoai_ota_single_mgr.update_result_local == true)
        {
            update_result = 0;
            xiaoai_ota_clear_notready_flag();
            xiaoai_ota_single_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_FINISH;
            xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                      update_result, next_block_offset, next_block_length, delay_ms);
            app_auto_power_off_enable(AUTO_POWER_OFF_MASK_XIAOAI_OTA, app_cfg_const.timer_auto_power_off);
        }
    }
#else
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        if (xiaoai_ota_mgr.update_result_local == true && xiaoai_ota_mgr.update_result_remote == true)
        {
            update_result = 0;

            xiaoai_ota_clear_notready_flag();

            xiaoai_ota_s2p_send_notify_image_rx_complete(type, bd_addr, conn_id, status, opcode_sn,
                                                         update_result);
            app_auto_power_off_enable(AUTO_POWER_OFF_MASK_XIAOAI_OTA, app_cfg_const.timer_auto_power_off);
        }
    }
    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        if (xiaoai_ota_mgr.update_complete_local && xiaoai_ota_mgr.update_complete_remote)
        {
            if (xiaoai_ota_mgr.update_result_local == true && xiaoai_ota_mgr.update_result_remote == true)
            {
                update_result = 0;

                xiaoai_ota_clear_notready_flag();
            }
            else
            {
                update_result = 1;
            }
            xiaoai_ota_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_FINISH;
            xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                      update_result, next_block_offset, next_block_length, delay_ms);
            app_auto_power_off_enable(AUTO_POWER_OFF_MASK_XIAOAI_OTA, app_cfg_const.timer_auto_power_off);
        }
        else
        {
            if (xiaoai_ota_mgr.update_result_local == true)
            {
                update_result = 0;
                xiaoai_ota_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_SECONDARY;
                xiaoai_ota_p2s_send_image_header_and_bin0_info(type, bd_addr, conn_id);
                xiaoai_ota_p2s_send_bin1_info(type, bd_addr, conn_id);
                xiaoai_ota_p2s_send_notify_image_rx_complete(type, bd_addr, conn_id, status, opcode_sn,
                                                             update_result);
            }
            else
            {
                update_result = 1;
                xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                          update_result, next_block_offset, next_block_length, delay_ms);
            }

        }
    }
#endif
}

#if (F_APP_ERWS_SUPPORT == 0)
void xiaoai_ota_single_check_set_sub_bin_info(void)
{
    uint8_t active_bank = app_ota_get_active_bank();

    if (active_bank == IMAGE_LOCATION_BANK0)
    {
        xiaoai_ota_single_mgr.cur_sub_image_index =
            xiaoai_ota_single_mgr.sub_bin.merged_header.sub_file_count /
            2;
        xiaoai_ota_single_mgr.end_sub_image_index =
            xiaoai_ota_single_mgr.sub_bin.merged_header.sub_file_count;
    }
    else if (active_bank == IMAGE_LOCATION_BANK1)
    {
        xiaoai_ota_single_mgr.cur_sub_image_index = 0;
        xiaoai_ota_single_mgr.end_sub_image_index =
            xiaoai_ota_single_mgr.sub_bin.merged_header.sub_file_count /
            2;
#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
        xiaoai_ota_single_mgr.end_sub_image_index++; //common voice prompt data image
#endif
    }
    else
    {
        APP_PRINT_ERROR1("xiaoai_ota_single_check_set_sub_bin_info: active_bank %d", active_bank);
    }

    xiaoai_ota_single_mgr.cur_sub_image_relative_offset = 0;

    APP_PRINT_INFO6("xiaoai_ota_single_check_set_sub_bin_info: current bud_role %d, app_cfg_const.bud_role %d, active_bank %d, cur_sub_bin_index: %d, cur_sub_image_index %d, end_sub_image_index %d",
                    app_cfg_nv.bud_role, app_cfg_const.bud_role, active_bank, xiaoai_ota_single_mgr.cur_sub_bin_index,
                    xiaoai_ota_single_mgr.cur_sub_image_index,
                    xiaoai_ota_single_mgr.end_sub_image_index);

}
#endif

#if F_APP_ERWS_SUPPORT
void xiaoai_ota_check_set_cur_sub_bin_info(void)
{
    uint8_t default_bud_role = REMOTE_SESSION_ROLE_SINGLE;

    xiaoai_ota_mgr.cur_sub_bin_index = 0;

    for (uint8_t i = 0 ; i < xiaoai_ota_mgr.combine_head.num; i++)
    {
        if (xiaoai_ota_mgr.combine_head.bin_info[i].default_bud_role == 1)
        {
            default_bud_role = REMOTE_SESSION_ROLE_PRIMARY;
        }
        else
        {
            default_bud_role = REMOTE_SESSION_ROLE_SECONDARY;
        }

        if (app_cfg_const.bud_role == default_bud_role)
        {
            xiaoai_ota_mgr.cur_sub_bin_index = i;
            break;
        }
    }

    uint8_t active_bank = app_ota_get_active_bank();

    if (active_bank == IMAGE_LOCATION_BANK0)
    {
        xiaoai_ota_mgr.cur_sub_image_index = xiaoai_ota_mgr.sub_bin[0].merged_header.sub_file_count / 2;
        xiaoai_ota_mgr.end_sub_image_index = xiaoai_ota_mgr.sub_bin[0].merged_header.sub_file_count;
    }
    else if (active_bank == IMAGE_LOCATION_BANK1)
    {
        xiaoai_ota_mgr.cur_sub_image_index = 0;
        xiaoai_ota_mgr.end_sub_image_index = xiaoai_ota_mgr.sub_bin[0].merged_header.sub_file_count / 2;
#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
        xiaoai_ota_mgr.end_sub_image_index++; //common voice prompt data image
#endif
    }
    else
    {
        APP_PRINT_ERROR1("xiaoai_ota_check_set_cur_sub_bin_info: active_bank %d", active_bank);
    }

    xiaoai_ota_mgr.cur_sub_image_relative_offset = 0;

    APP_PRINT_INFO6("xiaoai_ota_check_set_cur_sub_bin_info: current bud_role %d, app_cfg_const.bud_role %d, active_bank %d, cur_sub_bin_index: %d, cur_sub_image_index %d, end_sub_image_index %d",
                    app_cfg_nv.bud_role, app_cfg_const.bud_role, active_bank, xiaoai_ota_mgr.cur_sub_bin_index,
                    xiaoai_ota_mgr.cur_sub_image_index,
                    xiaoai_ota_mgr.end_sub_image_index);

}
#endif

bool xiaoai_ota_check_sub_image_merged_header(uint8_t *p_data, uint16_t len)
{
    bool ret = true;

    return ret;
}

bool xiaoai_ota_check_sub_image_payload_image_control_header(uint8_t *p_data, uint16_t len)
{
    bool ret = true;
    T_IMG_CTRL_HEADER_FORMAT *p_img_header = (T_IMG_CTRL_HEADER_FORMAT *)p_data;

    APP_PRINT_INFO6("xiaoai_ota_check_sub_image_payload_image_control_header 1: "
                    " crc16 0x%04x, ic_type 0x%02x, secure_version 0x%02x, value 0x%04x, image_id 0x%04x, payload_len 0x%08x",
                    p_img_header->crc16, p_img_header->ic_type, p_img_header->secure_version,
                    p_img_header->ctrl_flag.value,
                    p_img_header->image_id, p_img_header->payload_len);

    APP_PRINT_INFO8("xiaoai_ota_check_sub_image_payload_image_control_header 2: "
                    "xip %d, enc %d, load_when_boot %d, enc_load %d, enc_key_select %d, "
                    "not_ready %d, not_obsolete %d, integrity_check_en_in_boot %d",
                    p_img_header->ctrl_flag.xip, p_img_header->ctrl_flag.enc, p_img_header->ctrl_flag.load_when_boot,
                    p_img_header->ctrl_flag.enc_load, p_img_header->ctrl_flag.enc_key_select,
                    p_img_header->ctrl_flag.not_ready,
                    p_img_header->ctrl_flag.not_obsolete, p_img_header->ctrl_flag.integrity_check_en_in_boot);

#if(XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
    APP_PRINT_INFO1("xiaoai_ota_check_sub_image_payload_image_control_header 3: header_size %d",
                    p_img_header->ctrl_flag.header_size);
#endif

    return ret;
}

#if F_APP_ERWS_SUPPORT
void xiaoai_ota_handle_cmd(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                           uint8_t opcode,
                           bool req_rsp, uint8_t *p_data, uint16_t len)
{
    uint8_t opcode_sn = p_data[0];
    uint8_t *param2 = &p_data[1];

    APP_PRINT_INFO3("xiaoai_ota_handle_cmd: opcode 0x%x, len %d, p_data %b", opcode, len,
                    TRACE_BINARY(len, p_data));

    switch (opcode)
    {
    case XM_OPCODE_OTA_GET_DEVICE_UPDATE_FILEINFO_OFFSET:
        {
            app_auto_power_off_disable(AUTO_POWER_OFF_MASK_XIAOAI_OTA);
            xiaoai_ota_p2s_send_ota_start(XIAOAI_OTA_P2S_MSG_OTA_START);

            uint16_t mtu = 20;

            T_XM_CMD_OTA_GET_FILE_OFFSET file_offset;
            file_offset.offset = 0;
            file_offset.len = OTA_COMBINE_HEAD_SIZE + OTA_MERGED_FILE_HEAD_SIZE;

            xm_cmd_ota_get_device_update_fileinfo_offset_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS,
                                                             opcode_sn, file_offset);

            xiaoai_ota_get_ota_mtu_size(type, bd_addr, conn_id, &mtu);
            xiaoai_ota_mgr.mtu = mtu;
        }
        break;

    case XM_OPCODE_OTA_INQUIRE_DEVICE_IF_CAN_UPDATE:
        {
            uint8_t *p = param2;
            if (len - sizeof(opcode_sn) != OTA_COMBINE_HEAD_SIZE + OTA_MERGED_FILE_HEAD_SIZE)
            {
                APP_PRINT_ERROR0("xiaoai_ota_handle_cmd: XM_OPCODE_OTA_INQUIRE_DEVICE_IF_CAN_UPDATE len incorrect 1");
            }

            LE_STREAM_TO_UINT32(xiaoai_ota_mgr.combine_head.signature, p);
            LE_STREAM_TO_UINT32(xiaoai_ota_mgr.combine_head.version, p);
            LE_STREAM_TO_UINT32(xiaoai_ota_mgr.combine_head.num, p);
            LE_STREAM_TO_UINT32(xiaoai_ota_mgr.combine_head.bin_info[0].length, p);
            LE_STREAM_TO_UINT32(xiaoai_ota_mgr.combine_head.bin_info[0].default_bud_role, p);
            LE_STREAM_TO_UINT32(xiaoai_ota_mgr.combine_head.bin_info[1].length, p);
            LE_STREAM_TO_UINT32(xiaoai_ota_mgr.combine_head.bin_info[1].default_bud_role, p);

            APP_PRINT_INFO7("xiaoai_ota_handle_cmd: XM_OPCODE_OTA_INQUIRE_DEVICE_IF_CAN_UPDATE combine_head signature 0x%08x, version 0x%08x, num %d, "
                            "bin_info[0].length 0x%08x, bin_info[0].default_bud_role %d, bin_info[1].length 0x%08x, bin_info[1].default_bud_role %d",
                            xiaoai_ota_mgr.combine_head.signature,
                            xiaoai_ota_mgr.combine_head.version,
                            xiaoai_ota_mgr.combine_head.num,
                            xiaoai_ota_mgr.combine_head.bin_info[0].length,
                            xiaoai_ota_mgr.combine_head.bin_info[0].default_bud_role,
                            xiaoai_ota_mgr.combine_head.bin_info[1].length,
                            xiaoai_ota_mgr.combine_head.bin_info[1].default_bud_role);

            LE_STREAM_TO_UINT16(xiaoai_ota_mgr.sub_bin[0].merged_header.signature, p);
            LE_STREAM_TO_UINT32(xiaoai_ota_mgr.sub_bin[0].merged_header.size_of_merged_file, p);
            memcpy(xiaoai_ota_mgr.sub_bin[0].merged_header.checksum, p,
                   sizeof(xiaoai_ota_mgr.sub_bin[0].merged_header.checksum));
            p += sizeof(xiaoai_ota_mgr.sub_bin[0].merged_header.checksum);

            LE_STREAM_TO_UINT16(xiaoai_ota_mgr.sub_bin[0].merged_header.extension, p);

            memcpy(xiaoai_ota_mgr.sub_bin[0].merged_header.sub_file_indicator, p,
                   sizeof(xiaoai_ota_mgr.sub_bin[0].merged_header.sub_file_indicator));
            p += sizeof(xiaoai_ota_mgr.sub_bin[0].merged_header.sub_file_indicator);
            xiaoai_ota_mgr.sub_bin[0].merged_header.sub_file_count =
                xiaoai_ota_get_sub_image_header_count(xiaoai_ota_mgr.sub_bin[0].merged_header.sub_file_indicator,
                                                      sizeof(xiaoai_ota_mgr.sub_bin[0].merged_header.sub_file_indicator));

            APP_PRINT_INFO6("xiaoai_ota_handle_cmd: XM_OPCODE_OTA_INQUIRE_DEVICE_IF_CAN_UPDATE sub_bin_index 0, sub_image_header signature 0x%04x, size_of_merged_file 0x%08x, checksum %b, extension %x, sub_file_indicator 0x%b, sub_file_count %d",
                            xiaoai_ota_mgr.sub_bin[0].merged_header.signature,
                            xiaoai_ota_mgr.sub_bin[0].merged_header.size_of_merged_file,
                            TRACE_BINARY(sizeof(xiaoai_ota_mgr.sub_bin[0].merged_header.checksum),
                                         xiaoai_ota_mgr.sub_bin[0].merged_header.checksum),
                            xiaoai_ota_mgr.sub_bin[0].merged_header.extension,
                            TRACE_BINARY(sizeof(xiaoai_ota_mgr.sub_bin[0].merged_header.sub_file_indicator),
                                         xiaoai_ota_mgr.sub_bin[0].merged_header.sub_file_indicator),
                            xiaoai_ota_mgr.sub_bin[0].merged_header.sub_file_count);

            T_XM_OTA_STATUS update_available = XM_OTA_STATUS_AVAILABLE_DUAL;
            xm_cmd_ota_inquire_device_if_can_update_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS,
                                                        opcode_sn, update_available);

        }
        break;
    case XM_OPCODE_OTA_ENTER_UPDATE_MODE:
        {
            uint8_t in_ota_mode = 0;
            uint32_t first_block_offset = OTA_COMBINE_HEAD_SIZE + OTA_MERGED_FILE_HEAD_SIZE;
            uint16_t len = xiaoai_ota_mgr.sub_bin[0].merged_header.sub_file_count *
                           OTA_MERGED_FILE_SUB_HEAD_SIZE;
            uint8_t crc_en = false;

            app_sniff_mode_b2s_disable(bd_addr, SNIFF_DISABLE_MASK_OTA);

            xm_cmd_ota_enter_update_mode_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS, opcode_sn,
                                             in_ota_mode, first_block_offset, len, crc_en);

            xiaoai_ota_mgr.cur_expect_block_len = len;
            xiaoai_ota_mgr.state = XIAOAI_OTA_STATE_RX_BIN0_SUB_IMAGE_HEADER;
        }
        break;
    case XM_OPCODE_OTA_EXIT_UPDATE_MODE:
        {
            xm_cmd_ota_exit_update_mode_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS, opcode_sn, 0);
        }
        break;
    case XM_OPCODE_OTA_SEND_FIRMWARE_UPDATE_BLOCK:
        {
            APP_PRINT_INFO1("xiaoai_ota_handle_cmd: XM_OPCODE_OTA_SEND_FIRMWARE_UPDATE_BLOCK state %d",
                            xiaoai_ota_mgr.state);

            if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                (xiaoai_ota_mgr.state == XIAOAI_OTA_STATE_RX_SUB_IMAGE_SECONDARY))
            {
                xiaoai_ota_p2s_send_firmware_update_block(type, bd_addr, conn_id, opcode, req_rsp, p_data, len);
                break;
            }

            uint8_t status = XM_CMD_STATUS_SUCCESS;
            uint8_t update_result = 0;
            uint32_t next_block_offset = 0;
            uint16_t next_block_length = 0;
            uint16_t delay_ms = 0;

            if ((len - 1) != xiaoai_ota_mgr.cur_expect_block_len)
            {
                status = XM_CMD_STATUS_PARAM_ERROR;
                APP_PRINT_ERROR1("xiaoai_ota_handle_cmd: XM_OPCODE_OTA_SEND_FIRMWARE_UPDATE_BLOCK len incorrect 2 cur_expect_block_len %d",
                                 xiaoai_ota_mgr.cur_expect_block_len);

                xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                          update_result, next_block_offset, next_block_length, delay_ms);
                break;
            }

            if (xiaoai_ota_mgr.state == XIAOAI_OTA_STATE_RX_BIN0_SUB_IMAGE_HEADER)
            {
                xiaoai_ota_handle_sub_image_header(0, param2, len - 1);

                next_block_offset = OTA_COMBINE_HEAD_SIZE + xiaoai_ota_mgr.combine_head.bin_info[0].length;
                next_block_length = OTA_MERGED_FILE_HEAD_SIZE;

                xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                          update_result, next_block_offset, next_block_length, delay_ms);

                xiaoai_ota_mgr.cur_expect_block_len = next_block_length;
                xiaoai_ota_mgr.state = XIAOAI_OTA_STATE_RX_BIN1_MERGED_HEADER;
            }
            else if (xiaoai_ota_mgr.state == XIAOAI_OTA_STATE_RX_BIN1_MERGED_HEADER)
            {
                uint8_t sub_bin_index = 1;

                uint8_t *p = param2;
                LE_STREAM_TO_UINT16(xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.signature, p);
                LE_STREAM_TO_UINT32(xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.size_of_merged_file, p);
                memcpy(xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.checksum, p,
                       sizeof(xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.checksum));
                p += sizeof(xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.checksum);

                LE_STREAM_TO_UINT16(xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.extension, p);

                memcpy(xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.sub_file_indicator, p,
                       sizeof(xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.sub_file_indicator));
                p += sizeof(xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.sub_file_indicator);
                xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.sub_file_count =
                    xiaoai_ota_get_sub_image_header_count(
                        xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.sub_file_indicator,
                        sizeof(xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.sub_file_indicator));

                APP_PRINT_INFO7("xiaoai_ota_handle_cmd: XM_OPCODE_OTA_SEND_FIRMWARE_UPDATE_BLOCK sub_bin_index %d, sub_image_header signature 0x%04x, size_of_merged_file 0x%08x, checksum %b, extension %x, sub_file_indicator 0x%b, sub_file_count %d",
                                sub_bin_index,
                                xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.signature,
                                xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.size_of_merged_file,
                                TRACE_BINARY(sizeof(xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.checksum),
                                             xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.checksum),
                                xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.extension,
                                TRACE_BINARY(sizeof(xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.sub_file_indicator),
                                             xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.sub_file_indicator),
                                xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.sub_file_count);

                next_block_offset = OTA_COMBINE_HEAD_SIZE + xiaoai_ota_mgr.combine_head.bin_info[0].length +
                                    OTA_MERGED_FILE_HEAD_SIZE;
                next_block_length = xiaoai_ota_mgr.sub_bin[sub_bin_index].merged_header.sub_file_count *
                                    OTA_MERGED_FILE_SUB_HEAD_SIZE;

                xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                          update_result, next_block_offset, next_block_length, delay_ms);

                xiaoai_ota_mgr.cur_expect_block_len = next_block_length;
                xiaoai_ota_mgr.state = XIAOAI_OTA_STATE_RX_BIN1_SUB_IMAGE_HEADER;
            }
            else if (xiaoai_ota_mgr.state == XIAOAI_OTA_STATE_RX_BIN1_SUB_IMAGE_HEADER) //state 4
            {
                xiaoai_ota_handle_sub_image_header(1, param2, len - 1);

                xiaoai_ota_check_set_cur_sub_bin_info();

                //app_flash_set_bp_lv(0);

                //request 1st image, skip mp header OTA_SUB_IMAGE_MP_HEAD_SIZE = 512 bytes
                xiaoai_ota_mgr.cur_sub_image_relative_offset += OTA_SUB_IMAGE_MP_HEAD_SIZE;

                next_block_offset = xiaoai_ota_mgr.cur_sub_image_relative_offset +
                                    xiaoai_ota_mgr.sub_bin[xiaoai_ota_mgr.cur_sub_bin_index].sub_image_header[xiaoai_ota_mgr.cur_sub_image_index].start_file_offset;
                next_block_length = OTA_SUB_IMAGE_PAYLOAD_IMAGE_CONTROL_HEAD_SIZE;
#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
                next_block_length = xiaoai_ota_mgr.mtu;
                if (xiaoai_ota_mgr.cur_sub_image_relative_offset + next_block_length >
                    OTA_SUB_IMAGE_MP_HEAD_SIZE + OTA_SUB_IMAGE_PAYLOAD_IMAGE_AUTH_HEAD_SIZE)
                {
                    next_block_length =
                        OTA_SUB_IMAGE_MP_HEAD_SIZE + OTA_SUB_IMAGE_PAYLOAD_IMAGE_AUTH_HEAD_SIZE //512+304 - relative
                        -
                        xiaoai_ota_mgr.cur_sub_image_relative_offset;
                }
                APP_PRINT_INFO1("xiaoai_final_next_block_length 0x%04x", next_block_length);
#endif
                xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                          update_result, next_block_offset, next_block_length, delay_ms);

                xiaoai_ota_mgr.cur_expect_block_len = next_block_length;
#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
                xiaoai_ota_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_AUTH_HEADER;
#else
                xiaoai_ota_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_HEADER;
#endif
            }
#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
            else if (xiaoai_ota_mgr.state == XIAOAI_OTA_STATE_RX_SUB_IMAGE_AUTH_HEADER)
            {
                APP_PRINT_INFO0("XIAOAI_OTA_STATE_RX_SUB_IMAGE_AUTH_HEADER");
                uint32_t dfu_base_addr =
                    xiaoai_ota_mgr.sub_bin[xiaoai_ota_mgr.cur_sub_bin_index].sub_image_header[xiaoai_ota_mgr.cur_sub_image_index].download_addr;
                uint32_t offset = xiaoai_ota_mgr.cur_sub_image_relative_offset - OTA_SUB_IMAGE_MP_HEAD_SIZE;
                uint32_t length = xiaoai_ota_mgr.cur_expect_block_len; //android 304, ios mtu_size (<304)
                uint8_t *p_data = param2;

                uint32_t write_result = xiaoai_ota_write_to_flash(dfu_base_addr, offset, length, p_data, 0);
                if (write_result != 0)
                {
                    status = XM_CMD_STATUS_FAIL;
                    next_block_offset = 0;
                    next_block_length = 0;
                    update_result = 1;
                }
                else
                {
                    next_block_length = xiaoai_ota_mgr.mtu;
                    ///update cur_sub_image_relative_offset
                    xiaoai_ota_mgr.cur_sub_image_relative_offset +=
                        xiaoai_ota_mgr.cur_expect_block_len; //512+304 or 512 + mtu
                    if (xiaoai_ota_mgr.cur_sub_image_relative_offset == OTA_SUB_IMAGE_MP_HEAD_SIZE +
                        OTA_SUB_IMAGE_PAYLOAD_IMAGE_AUTH_HEAD_SIZE)
                    {
                        //go to next state
                        next_block_offset = xiaoai_ota_mgr.cur_sub_image_relative_offset +
                                            xiaoai_ota_mgr.sub_bin[xiaoai_ota_mgr.cur_sub_bin_index].sub_image_header[xiaoai_ota_mgr.cur_sub_image_index].start_file_offset;
                        next_block_length = OTA_SUB_IMAGE_PAYLOAD_IMAGE_CONTROL_HEAD_SIZE;

                        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                            app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
                        {
                            xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                                      update_result, next_block_offset, next_block_length, delay_ms);
                        }
                        else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                        {
                            xiaoai_ota_s2p_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                                          update_result, next_block_offset, next_block_length, delay_ms);

                        }
                        xiaoai_ota_mgr.cur_expect_block_len = next_block_length;
                        xiaoai_ota_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_HEADER; //control header
                    }
                    else
                    {
                        if (xiaoai_ota_mgr.cur_sub_image_relative_offset + next_block_length >
                            OTA_SUB_IMAGE_MP_HEAD_SIZE + OTA_SUB_IMAGE_PAYLOAD_IMAGE_AUTH_HEAD_SIZE)
                        {
                            next_block_length = OTA_SUB_IMAGE_MP_HEAD_SIZE +
                                                OTA_SUB_IMAGE_PAYLOAD_IMAGE_AUTH_HEAD_SIZE //512+304 - relative
                                                -
                                                xiaoai_ota_mgr.cur_sub_image_relative_offset;
                        }
                        //prepare next block
                        next_block_offset = xiaoai_ota_mgr.cur_sub_image_relative_offset +
                                            xiaoai_ota_mgr.sub_bin[xiaoai_ota_mgr.cur_sub_bin_index].sub_image_header[xiaoai_ota_mgr.cur_sub_image_index].start_file_offset;

                        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                            app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
                        {
                            xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                                      update_result, next_block_offset, next_block_length, delay_ms);
                        }
                        else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                        {
                            xiaoai_ota_s2p_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                                          update_result, next_block_offset, next_block_length, delay_ms);

                        }
                        xiaoai_ota_mgr.cur_expect_block_len = next_block_length;
                        xiaoai_ota_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_AUTH_HEADER;
                    }
                }
            }
#endif
            else if (xiaoai_ota_mgr.state == XIAOAI_OTA_STATE_RX_SUB_IMAGE_HEADER) //Control header
            {
                APP_PRINT_INFO0("xiaoai_ota_control_header");
                //xiaoai_ota_check_sub_image_merged_header(param2, OTA_SUB_IMAGE_MP_HEAD_SIZE);
                xiaoai_ota_check_sub_image_payload_image_control_header(param2,
                                                                        OTA_SUB_IMAGE_PAYLOAD_IMAGE_CONTROL_HEAD_SIZE);

                uint32_t dfu_base_addr =
                    xiaoai_ota_mgr.sub_bin[xiaoai_ota_mgr.cur_sub_bin_index].sub_image_header[xiaoai_ota_mgr.cur_sub_image_index].download_addr;
                uint32_t offset = xiaoai_ota_mgr.cur_sub_image_relative_offset - OTA_SUB_IMAGE_MP_HEAD_SIZE;
                uint32_t length = OTA_SUB_IMAGE_PAYLOAD_IMAGE_CONTROL_HEAD_SIZE;
                uint8_t *p_data = param2;

                APP_PRINT_INFO1("xiaoai_ota_control_header: offset %d", offset);

                uint32_t write_result = xiaoai_ota_write_to_flash(dfu_base_addr, offset, length, p_data,
                                                                  1);//write control header

                if (write_result != 0)
                {
                    status = XM_CMD_STATUS_FAIL;
                    next_block_offset = 0;
                    next_block_length = 0;
                    update_result = 1;
                }
                else
                {
                    xiaoai_ota_mgr.cur_sub_image_relative_offset += xiaoai_ota_mgr.cur_expect_block_len;
                    //prepare next block
                    next_block_offset = xiaoai_ota_mgr.cur_sub_image_relative_offset +
                                        xiaoai_ota_mgr.sub_bin[xiaoai_ota_mgr.cur_sub_bin_index].sub_image_header[xiaoai_ota_mgr.cur_sub_image_index].start_file_offset;

                    next_block_length = xiaoai_ota_mgr.mtu;
                    //Judge if it will exceed the size of bin
                    if (xiaoai_ota_mgr.cur_sub_image_relative_offset + next_block_length >
                        xiaoai_ota_mgr.sub_bin[xiaoai_ota_mgr.cur_sub_bin_index].sub_image_header[xiaoai_ota_mgr.cur_sub_image_index].size)
                    {
                        next_block_length =
                            xiaoai_ota_mgr.sub_bin[xiaoai_ota_mgr.cur_sub_bin_index].sub_image_header[xiaoai_ota_mgr.cur_sub_image_index].size
                            -
                            xiaoai_ota_mgr.cur_sub_image_relative_offset;
                    }
                    APP_PRINT_INFO1("xiaoai_final_next_block_length 0x%04x", next_block_length);
                }

                if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                {
                    xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                              update_result, next_block_offset, next_block_length, delay_ms);
                }
                else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    xiaoai_ota_s2p_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                                  update_result, next_block_offset, next_block_length, delay_ms);

                }

                xiaoai_ota_mgr.cur_expect_block_len = next_block_length;
                xiaoai_ota_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_PAYLOAD_OTA_DATA;
            }
            else if (xiaoai_ota_mgr.state == XIAOAI_OTA_STATE_RX_SUB_IMAGE_PAYLOAD_OTA_DATA)
            {
                //xm_ota_handle_sub_image_payload_ota_data(param2, len - 1);
                //write to flash
                uint32_t dfu_base_addr =
                    xiaoai_ota_mgr.sub_bin[xiaoai_ota_mgr.cur_sub_bin_index].sub_image_header[xiaoai_ota_mgr.cur_sub_image_index].download_addr;
                uint32_t offset = xiaoai_ota_mgr.cur_sub_image_relative_offset - OTA_SUB_IMAGE_MP_HEAD_SIZE;
                uint32_t length = xiaoai_ota_mgr.cur_expect_block_len;
                uint8_t *p_data = param2;

                uint32_t write_result = xiaoai_ota_write_to_flash(dfu_base_addr, offset, length, p_data, 0);
                if (write_result != 0)
                {
                    status = XM_CMD_STATUS_FAIL;
                    next_block_offset = 0;
                    next_block_length = 0;
                    update_result = 1;
                    xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                              update_result, next_block_offset, next_block_length, delay_ms);

                    break;
                }

                //last pkt of one sub image
                if (xiaoai_ota_mgr.cur_sub_image_relative_offset + xiaoai_ota_mgr.cur_expect_block_len ==
                    xiaoai_ota_mgr.sub_bin[xiaoai_ota_mgr.cur_sub_bin_index].sub_image_header[xiaoai_ota_mgr.cur_sub_image_index].size)
                {
                    void *p_image_header = (void *)
                                           xiaoai_ota_mgr.sub_bin[xiaoai_ota_mgr.cur_sub_bin_index].sub_image_header[xiaoai_ota_mgr.cur_sub_image_index].download_addr;

                    bool check_image = xiaoai_ota_check_image(p_image_header);
                    APP_PRINT_INFO1("xiaoai_ota_check_image: check_image %d", check_image);

                    if (check_image)
                    {
                        //update cur_sub_image_relative_offset
                        xiaoai_ota_mgr.cur_sub_image_relative_offset = 0;
                        xiaoai_ota_mgr.cur_sub_image_index++;

                        //if it is last sub image
                        if (xiaoai_ota_mgr.cur_sub_image_index == xiaoai_ota_mgr.end_sub_image_index)
                        {
                            //OTA complete
                            xiaoai_ota_mgr.update_result_local = true;
                            xiaoai_ota_mgr.update_complete_local = true;
                            xiaoai_ota_mgr.cur_sub_image_relative_offset = 0;
                            xiaoai_ota_mgr.cur_sub_image_index = 0;

                            xiaoai_ota_update_complete_check(type, bd_addr, conn_id, status, opcode_sn);
                        }
                        else
                        {
                            //request Nth(next) sub image
                            APP_PRINT_INFO1("xiaoai_ota_handle_cmd: XM_OPCODE_OTA_SEND_FIRMWARE_UPDATE_BLOCK start update %d sub image",
                                            xiaoai_ota_mgr.cur_sub_image_index);

                            //skip mp header OTA_SUB_IMAGE_MP_HEAD_SIZE = 512 bytes
                            xiaoai_ota_mgr.cur_sub_image_relative_offset += OTA_SUB_IMAGE_MP_HEAD_SIZE;

                            next_block_offset = xiaoai_ota_mgr.cur_sub_image_relative_offset +
                                                xiaoai_ota_mgr.sub_bin[xiaoai_ota_mgr.cur_sub_bin_index].sub_image_header[xiaoai_ota_mgr.cur_sub_image_index].start_file_offset;
                            next_block_length = OTA_SUB_IMAGE_PAYLOAD_IMAGE_CONTROL_HEAD_SIZE;
#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
                            next_block_length = xiaoai_ota_mgr.mtu;
                            if (xiaoai_ota_mgr.cur_sub_image_relative_offset + next_block_length >
                                OTA_SUB_IMAGE_MP_HEAD_SIZE + OTA_SUB_IMAGE_PAYLOAD_IMAGE_AUTH_HEAD_SIZE)
                            {
                                next_block_length =
                                    OTA_SUB_IMAGE_MP_HEAD_SIZE + OTA_SUB_IMAGE_PAYLOAD_IMAGE_AUTH_HEAD_SIZE //512+304 - relative
                                    -
                                    xiaoai_ota_mgr.cur_sub_image_relative_offset;
                            }
                            APP_PRINT_INFO1("xiaoai_final_next_block_length 0x%04x", next_block_length);
#endif


                            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                                app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
                            {
                                xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                                          update_result, next_block_offset, next_block_length, delay_ms);
                            }
                            else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                            {
                                xiaoai_ota_s2p_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                                              update_result, next_block_offset, next_block_length, delay_ms);

                            }

                            xiaoai_ota_mgr.cur_expect_block_len = next_block_length;

#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
                            xiaoai_ota_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_AUTH_HEADER;
#else
                            xiaoai_ota_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_HEADER;
#endif
                        }
                    }
                    else
                    {
                        APP_PRINT_ERROR0("xiaoai_ota_check_image failed");
                        status = XM_CMD_STATUS_CRC_ERROR;
                        next_block_offset = 0;
                        next_block_length = 0;
                        update_result = 1;
                        xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                                  update_result, next_block_offset, next_block_length, delay_ms);
                    }
                }
                else
                {
                    //update cur_sub_image_relative_offset
                    xiaoai_ota_mgr.cur_sub_image_relative_offset += xiaoai_ota_mgr.cur_expect_block_len;
                    //prepare next block

                    next_block_offset = xiaoai_ota_mgr.cur_sub_image_relative_offset +
                                        xiaoai_ota_mgr.sub_bin[xiaoai_ota_mgr.cur_sub_bin_index].sub_image_header[xiaoai_ota_mgr.cur_sub_image_index].start_file_offset;

                    next_block_length = xiaoai_ota_mgr.mtu;
                    if (xiaoai_ota_mgr.cur_sub_image_relative_offset + next_block_length >
                        xiaoai_ota_mgr.sub_bin[xiaoai_ota_mgr.cur_sub_bin_index].sub_image_header[xiaoai_ota_mgr.cur_sub_image_index].size)
                    {
                        next_block_length =
                            xiaoai_ota_mgr.sub_bin[xiaoai_ota_mgr.cur_sub_bin_index].sub_image_header[xiaoai_ota_mgr.cur_sub_image_index].size
                            -
                            xiaoai_ota_mgr.cur_sub_image_relative_offset;
                    }
                    APP_PRINT_INFO1("xiaoai_final_next_block_length 0x%04x", next_block_length);
                    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                    {
                        xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                                  update_result, next_block_offset, next_block_length, delay_ms);
                    }
                    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                    {
                        xiaoai_ota_s2p_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                                      update_result, next_block_offset, next_block_length, delay_ms);

                    }

                    xiaoai_ota_mgr.cur_expect_block_len = next_block_length;
                    xiaoai_ota_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_PAYLOAD_OTA_DATA;
                }
            }
        }
        break;
    case XM_OPCODE_OTA_GET_DEVICE_REFRESH_FIRMWARE_STATUS:
        {
            T_XM_OTA_UPDATE_RESULT update_result = XM_OTA_UPDATE_RESULT_SUCCESS;
            if (xiaoai_ota_mgr.update_result_local && xiaoai_ota_mgr.update_result_remote)
            {
                update_result = XM_OTA_UPDATE_RESULT_SUCCESS;
            }
            else
            {
                update_result = XM_OTA_UPDATE_RESULT_FAIL;
            }

            xm_cmd_ota_get_device_refresh_firmware_status_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS,
                                                              opcode_sn, update_result);
        }
        break;
    case XM_OPCODE_OTA_NOTIFY_UPDATE_MODE:
        {
            xm_cmd_ota_notify_update_mode_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS, opcode_sn);
        }
        break;
    case XM_OPCODE_OTA_RESERVED:
        {

        }
        break;

    default:
        break;
    }
}
#else
void xiaoai_ota_single_handle_cmd(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                                  uint8_t opcode,
                                  bool req_rsp, uint8_t *p_data, uint16_t len)
{
    uint8_t opcode_sn = p_data[0];
    uint8_t *param2 = &p_data[1];

    APP_PRINT_INFO3("xiaoai_ota_single_handle_cmd: opcode 0x%x, len %d, p_data %b", opcode, len,
                    TRACE_BINARY(len, p_data));

    switch (opcode)
    {
    case XM_OPCODE_OTA_GET_DEVICE_UPDATE_FILEINFO_OFFSET:
        {
            app_auto_power_off_disable(AUTO_POWER_OFF_MASK_XIAOAI_OTA);
            uint16_t mtu = 20;

            T_XM_CMD_OTA_GET_FILE_OFFSET file_offset;
            file_offset.offset = 0;
            file_offset.len = OTA_MERGED_FILE_HEAD_SIZE; //44

            xm_cmd_ota_get_device_update_fileinfo_offset_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS,
                                                             opcode_sn, file_offset);

            xiaoai_ota_get_ota_mtu_size(type, bd_addr, conn_id, &mtu);
            xiaoai_ota_single_mgr.mtu = mtu;
        }
        break;

    case XM_OPCODE_OTA_INQUIRE_DEVICE_IF_CAN_UPDATE:
        {
            uint8_t *p = param2;
            if (len - sizeof(opcode_sn) != OTA_MERGED_FILE_HEAD_SIZE)
            {
                APP_PRINT_ERROR0("xiaoai_ota_single_handle_cmd: XM_OPCODE_OTA_INQUIRE_DEVICE_IF_CAN_UPDATE len incorrect 1");
            }

            LE_STREAM_TO_UINT16(xiaoai_ota_single_mgr.sub_bin.merged_header.signature, p);
            LE_STREAM_TO_UINT32(xiaoai_ota_single_mgr.sub_bin.merged_header.size_of_merged_file, p);
            memcpy(xiaoai_ota_single_mgr.sub_bin.merged_header.checksum, p,
                   sizeof(xiaoai_ota_single_mgr.sub_bin.merged_header.checksum));
            p += sizeof(xiaoai_ota_single_mgr.sub_bin.merged_header.checksum);

            LE_STREAM_TO_UINT16(xiaoai_ota_single_mgr.sub_bin.merged_header.extension, p);

            memcpy(xiaoai_ota_single_mgr.sub_bin.merged_header.sub_file_indicator, p,
                   sizeof(xiaoai_ota_single_mgr.sub_bin.merged_header.sub_file_indicator));
            p += sizeof(xiaoai_ota_single_mgr.sub_bin.merged_header.sub_file_indicator);
            xiaoai_ota_single_mgr.sub_bin.merged_header.sub_file_count =
                xiaoai_ota_get_sub_image_header_count(
                    xiaoai_ota_single_mgr.sub_bin.merged_header.sub_file_indicator,
                    sizeof(xiaoai_ota_single_mgr.sub_bin.merged_header.sub_file_indicator));

            APP_PRINT_INFO7("xiaoai_ota_single_handle_cmd: XM_OPCODE_OTA_INQUIRE_DEVICE_IF_CAN_UPDATE sub_image_header signature 0x%04x, size_of_merged_file 0x%08x, checksum %b, extension %x, sub_file_indicator 0x%b, sub_file_count %d",
                            xiaoai_ota_single_mgr.sub_bin.merged_header.signature,
                            xiaoai_ota_single_mgr.sub_bin.merged_header.size_of_merged_file,
                            TRACE_BINARY(sizeof(xiaoai_ota_single_mgr.sub_bin.merged_header.checksum),
                                         xiaoai_ota_single_mgr.sub_bin.merged_header.checksum),
                            xiaoai_ota_single_mgr.sub_bin.merged_header.extension,
                            TRACE_BINARY(sizeof(xiaoai_ota_single_mgr.sub_bin.merged_header.sub_file_indicator),
                                         xiaoai_ota_single_mgr.sub_bin.merged_header.sub_file_indicator),
                            xiaoai_ota_single_mgr.sub_bin.merged_header.sub_file_indicator,
                            xiaoai_ota_single_mgr.sub_bin.merged_header.sub_file_count);

            T_XM_OTA_STATUS update_available = XM_OTA_STATUS_AVAILABLE_DUAL;
            xm_cmd_ota_inquire_device_if_can_update_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS,
                                                        opcode_sn, update_available);

        }
        break;
    case XM_OPCODE_OTA_ENTER_UPDATE_MODE:
        {
            uint8_t in_ota_mode = 0;
            uint32_t first_block_offset = OTA_MERGED_FILE_HEAD_SIZE;
            uint16_t len = xiaoai_ota_single_mgr.sub_bin.merged_header.sub_file_count *
                           OTA_MERGED_FILE_SUB_HEAD_SIZE;
            uint8_t crc_en = false;

            app_sniff_mode_b2s_disable(bd_addr, SNIFF_DISABLE_MASK_OTA);

            xm_cmd_ota_enter_update_mode_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS, opcode_sn,
                                             in_ota_mode, first_block_offset, len, crc_en);

            xiaoai_ota_single_mgr.cur_expect_block_len = len;
            xiaoai_ota_single_mgr.state = XIAOAI_OTA_STATE_RX_BIN0_SUB_IMAGE_HEADER;
        }
        break;
    case XM_OPCODE_OTA_EXIT_UPDATE_MODE:
        {
            xm_cmd_ota_exit_update_mode_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS, opcode_sn, 0);
        }
        break;
    case XM_OPCODE_OTA_SEND_FIRMWARE_UPDATE_BLOCK: //E5
        {
            APP_PRINT_INFO1("xiaoai_ota_single_handle_cmd: XM_OPCODE_OTA_SEND_FIRMWARE_UPDATE_BLOCK state %d",
                            xiaoai_ota_single_mgr.state);

            uint8_t status = XM_CMD_STATUS_SUCCESS;
            uint8_t update_result = 0;
            uint32_t next_block_offset = 0;
            uint16_t next_block_length = 0;
            uint16_t delay_ms = 0;

            if ((len - 1) != xiaoai_ota_single_mgr.cur_expect_block_len)
            {
                status = XM_CMD_STATUS_PARAM_ERROR;
                APP_PRINT_ERROR1("xiaoai_ota_single_handle_cmd: XM_OPCODE_OTA_SEND_FIRMWARE_UPDATE_BLOCK len incorrect 2 cur_expect_block_len %d",
                                 xiaoai_ota_single_mgr.cur_expect_block_len);

                xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                          update_result, next_block_offset, next_block_length, delay_ms);
                break;
            }

            if (xiaoai_ota_single_mgr.state == XIAOAI_OTA_STATE_RX_BIN0_SUB_IMAGE_HEADER)
            {
                xiaoai_ota_handle_sub_image_header(0, param2, len - 1);

                xiaoai_ota_single_check_set_sub_bin_info();

                //request 1st image, skip mp header OTA_SUB_IMAGE_MP_HEAD_SIZE = 512 bytes
                xiaoai_ota_single_mgr.cur_sub_image_relative_offset += OTA_SUB_IMAGE_MP_HEAD_SIZE;

                next_block_offset = xiaoai_ota_single_mgr.cur_sub_image_relative_offset +
                                    xiaoai_ota_single_mgr.sub_bin.sub_image_header[xiaoai_ota_single_mgr.cur_sub_image_index].start_file_offset;
                next_block_length = OTA_SUB_IMAGE_PAYLOAD_IMAGE_CONTROL_HEAD_SIZE;

#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
                next_block_length = xiaoai_ota_single_mgr.mtu;
                if (xiaoai_ota_single_mgr.cur_sub_image_relative_offset + next_block_length >
                    OTA_SUB_IMAGE_MP_HEAD_SIZE + OTA_SUB_IMAGE_PAYLOAD_IMAGE_AUTH_HEAD_SIZE)
                {
                    next_block_length =
                        OTA_SUB_IMAGE_MP_HEAD_SIZE + OTA_SUB_IMAGE_PAYLOAD_IMAGE_AUTH_HEAD_SIZE //512+304 - relative
                        -
                        xiaoai_ota_single_mgr.cur_sub_image_relative_offset;
                }
                APP_PRINT_INFO1("xiaoai_final_next_block_length 0x%04x", next_block_length);
#endif

                xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                          update_result, next_block_offset, next_block_length, delay_ms);

                xiaoai_ota_single_mgr.cur_expect_block_len = next_block_length;
#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
                xiaoai_ota_single_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_AUTH_HEADER;
#else
                xiaoai_ota_single_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_HEADER;
#endif
            }
#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
            else if (xiaoai_ota_single_mgr.state == XIAOAI_OTA_STATE_RX_SUB_IMAGE_AUTH_HEADER)
            {
                APP_PRINT_INFO0("xiaoai_ota_auth_header");
                uint32_t dfu_base_addr =
                    xiaoai_ota_single_mgr.sub_bin.sub_image_header[xiaoai_ota_single_mgr.cur_sub_image_index].download_addr;
                uint32_t offset = xiaoai_ota_single_mgr.cur_sub_image_relative_offset - OTA_SUB_IMAGE_MP_HEAD_SIZE;
                uint32_t length = OTA_SUB_IMAGE_PAYLOAD_IMAGE_AUTH_HEAD_SIZE;
                uint8_t *p_data = param2;

                uint32_t write_result = xiaoai_ota_write_to_flash(dfu_base_addr, offset, length, p_data, 0);
                if (write_result != 0)
                {
                    status = XM_CMD_STATUS_FAIL;
                    next_block_offset = 0;
                    next_block_length = 0;
                    update_result = 1;
                }
                else
                {
                    next_block_length = xiaoai_ota_single_mgr.mtu;
                    ///update cur_sub_image_relative_offset
                    xiaoai_ota_single_mgr.cur_sub_image_relative_offset += xiaoai_ota_single_mgr.cur_expect_block_len;
                    if (xiaoai_ota_single_mgr.cur_sub_image_relative_offset == OTA_SUB_IMAGE_MP_HEAD_SIZE +
                        OTA_SUB_IMAGE_PAYLOAD_IMAGE_AUTH_HEAD_SIZE)
                    {
                        //go to next state
                        next_block_offset = xiaoai_ota_single_mgr.cur_sub_image_relative_offset +
                                            xiaoai_ota_single_mgr.sub_bin.sub_image_header[xiaoai_ota_single_mgr.cur_sub_image_index].start_file_offset;
                        next_block_length = OTA_SUB_IMAGE_PAYLOAD_IMAGE_CONTROL_HEAD_SIZE;

                        xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                                  update_result, next_block_offset, next_block_length, delay_ms);

                        xiaoai_ota_single_mgr.cur_expect_block_len = next_block_length;
                        xiaoai_ota_single_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_HEADER; //control header
                    }
                    else
                    {
                        if (xiaoai_ota_single_mgr.cur_sub_image_relative_offset + next_block_length >
                            OTA_SUB_IMAGE_MP_HEAD_SIZE + OTA_SUB_IMAGE_PAYLOAD_IMAGE_AUTH_HEAD_SIZE)
                        {
                            next_block_length = OTA_SUB_IMAGE_MP_HEAD_SIZE +
                                                OTA_SUB_IMAGE_PAYLOAD_IMAGE_AUTH_HEAD_SIZE //512+304 - relative
                                                -
                                                xiaoai_ota_single_mgr.cur_sub_image_relative_offset;
                        }
                        next_block_offset = xiaoai_ota_single_mgr.cur_sub_image_relative_offset +
                                            xiaoai_ota_single_mgr.sub_bin.sub_image_header[xiaoai_ota_single_mgr.cur_sub_image_index].start_file_offset;
                        xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                                  update_result, next_block_offset, next_block_length, delay_ms);

                        xiaoai_ota_single_mgr.cur_expect_block_len = next_block_length;
                        xiaoai_ota_single_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_AUTH_HEADER;
                    }
                }
            }
#endif
            else if (xiaoai_ota_single_mgr.state == XIAOAI_OTA_STATE_RX_SUB_IMAGE_HEADER)
            {
                //xiaoai_ota_check_sub_image_merged_header(param2, OTA_SUB_IMAGE_MP_HEAD_SIZE);
                xiaoai_ota_check_sub_image_payload_image_control_header(param2,
                                                                        OTA_SUB_IMAGE_PAYLOAD_IMAGE_CONTROL_HEAD_SIZE);

                //write to flash
                uint32_t dfu_base_addr =
                    xiaoai_ota_single_mgr.sub_bin.sub_image_header[xiaoai_ota_single_mgr.cur_sub_image_index].download_addr;
                uint32_t offset = xiaoai_ota_single_mgr.cur_sub_image_relative_offset - OTA_SUB_IMAGE_MP_HEAD_SIZE;
                uint32_t length = OTA_SUB_IMAGE_PAYLOAD_IMAGE_CONTROL_HEAD_SIZE;
                uint8_t *p_data = param2;

                uint32_t write_result = xiaoai_ota_write_to_flash(dfu_base_addr, offset, length, p_data, 1);
                if (write_result != 0)
                {
                    status = XM_CMD_STATUS_FAIL;
                    next_block_offset = 0;
                    next_block_length = 0;
                    update_result = 1;
                }
                else
                {
                    ///update cur_sub_image_relative_offset
                    xiaoai_ota_single_mgr.cur_sub_image_relative_offset += xiaoai_ota_single_mgr.cur_expect_block_len;
                    //prepare next block
                    next_block_offset = xiaoai_ota_single_mgr.cur_sub_image_relative_offset +
                                        xiaoai_ota_single_mgr.sub_bin.sub_image_header[xiaoai_ota_single_mgr.cur_sub_image_index].start_file_offset;

                    next_block_length = xiaoai_ota_single_mgr.mtu;
                    //Judge if it will exceed the size of bin
                    if (xiaoai_ota_single_mgr.cur_sub_image_relative_offset + next_block_length >
                        xiaoai_ota_single_mgr.sub_bin.sub_image_header[xiaoai_ota_single_mgr.cur_sub_image_index].size)
                    {
                        next_block_length =
                            xiaoai_ota_single_mgr.sub_bin.sub_image_header[xiaoai_ota_single_mgr.cur_sub_image_index].size
                            -
                            xiaoai_ota_single_mgr.cur_sub_image_relative_offset;
                    }
                    APP_PRINT_INFO1("xiaoai_final_next_block_length 0x%04x", next_block_length);
                }

                xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                          update_result, next_block_offset, next_block_length, delay_ms);

                xiaoai_ota_single_mgr.cur_expect_block_len = next_block_length;
                xiaoai_ota_single_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_PAYLOAD_OTA_DATA;
            }
            else if (xiaoai_ota_single_mgr.state == XIAOAI_OTA_STATE_RX_SUB_IMAGE_PAYLOAD_OTA_DATA)
            {
                //xm_ota_handle_sub_image_payload_ota_data(param2, len - 1);
                //write to flash
                uint32_t dfu_base_addr =
                    xiaoai_ota_single_mgr.sub_bin.sub_image_header[xiaoai_ota_single_mgr.cur_sub_image_index].download_addr;
                uint32_t offset = xiaoai_ota_single_mgr.cur_sub_image_relative_offset - OTA_SUB_IMAGE_MP_HEAD_SIZE;
                uint32_t length = xiaoai_ota_single_mgr.cur_expect_block_len;
                uint8_t *p_data = param2;

                uint32_t write_result = xiaoai_ota_write_to_flash(dfu_base_addr, offset, length, p_data, 0);
                if (write_result != 0)
                {
                    status = XM_CMD_STATUS_FAIL;
                    next_block_offset = 0;
                    next_block_length = 0;
                    update_result = 1;
                    xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                              update_result, next_block_offset, next_block_length, delay_ms);

                    break;
                }

                //last pkt of one sub image
                if (xiaoai_ota_single_mgr.cur_sub_image_relative_offset + xiaoai_ota_single_mgr.cur_expect_block_len
                    ==
                    xiaoai_ota_single_mgr.sub_bin.sub_image_header[xiaoai_ota_single_mgr.cur_sub_image_index].size)
                {
                    void *p_image_header = (void *)
                                           xiaoai_ota_single_mgr.sub_bin.sub_image_header[xiaoai_ota_single_mgr.cur_sub_image_index].download_addr;


                    bool check_image = xiaoai_ota_check_image(p_image_header);
                    APP_PRINT_INFO1("xiaoai_ota_check_image: check_image %d", check_image);

                    if (check_image)
                    {
                        //update cur_sub_image_relative_offset
                        xiaoai_ota_single_mgr.cur_sub_image_relative_offset = 0;
                        xiaoai_ota_single_mgr.cur_sub_image_index++;

                        //if it is last sub image
                        if (xiaoai_ota_single_mgr.cur_sub_image_index == xiaoai_ota_single_mgr.end_sub_image_index)
                        {
                            //OTA complete
                            xiaoai_ota_single_mgr.update_result_local = true;
                            xiaoai_ota_single_mgr.update_complete_local = true;
                            xiaoai_ota_single_mgr.cur_sub_image_relative_offset = 0;
                            xiaoai_ota_single_mgr.cur_sub_image_index = 0;

                            xiaoai_ota_update_complete_check(type, bd_addr, conn_id, status, opcode_sn);
                        }
                        else
                        {
                            //request Nth sub image
                            APP_PRINT_INFO1("xiaoai_ota_single_handle_cmd: XM_OPCODE_OTA_SEND_FIRMWARE_UPDATE_BLOCK start update %d sub image",
                                            xiaoai_ota_single_mgr.cur_sub_image_index);

                            //skip mp header OTA_SUB_IMAGE_MP_HEAD_SIZE = 512 bytes
                            xiaoai_ota_single_mgr.cur_sub_image_relative_offset += OTA_SUB_IMAGE_MP_HEAD_SIZE;

                            next_block_offset = xiaoai_ota_single_mgr.cur_sub_image_relative_offset +
                                                xiaoai_ota_single_mgr.sub_bin.sub_image_header[xiaoai_ota_single_mgr.cur_sub_image_index].start_file_offset;
#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
                            next_block_length = OTA_SUB_IMAGE_PAYLOAD_IMAGE_AUTH_HEAD_SIZE;
#else
                            next_block_length = OTA_SUB_IMAGE_PAYLOAD_IMAGE_CONTROL_HEAD_SIZE;
#endif
                            xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                                      update_result, next_block_offset, next_block_length, delay_ms);

                            xiaoai_ota_single_mgr.cur_expect_block_len = next_block_length;
#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
                            xiaoai_ota_single_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_AUTH_HEADER;
#else
                            xiaoai_ota_single_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_HEADER;
#endif
                        }
                    }
                    else
                    {
                        APP_PRINT_ERROR0("xiaoai_ota_check_image failed");
                        status = XM_CMD_STATUS_CRC_ERROR;
                        next_block_offset = 0;
                        next_block_length = 0;
                        update_result = 1;
                        xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                                  update_result, next_block_offset, next_block_length, delay_ms);
                    }
                }
                else
                {
                    //update cur_sub_image_relative_offset
                    xiaoai_ota_single_mgr.cur_sub_image_relative_offset += xiaoai_ota_single_mgr.cur_expect_block_len;
                    //prepare next block

                    next_block_offset = xiaoai_ota_single_mgr.cur_sub_image_relative_offset +
                                        xiaoai_ota_single_mgr.sub_bin.sub_image_header[xiaoai_ota_single_mgr.cur_sub_image_index].start_file_offset;

                    next_block_length = xiaoai_ota_single_mgr.mtu;

                    if (xiaoai_ota_single_mgr.cur_sub_image_relative_offset + next_block_length >
                        xiaoai_ota_single_mgr.sub_bin.sub_image_header[xiaoai_ota_single_mgr.cur_sub_image_index].size)
                    {
                        next_block_length =
                            xiaoai_ota_single_mgr.sub_bin.sub_image_header[xiaoai_ota_single_mgr.cur_sub_image_index].size
                            -
                            xiaoai_ota_single_mgr.cur_sub_image_relative_offset;
                    }
                    xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                              update_result, next_block_offset, next_block_length, delay_ms);

                    xiaoai_ota_single_mgr.cur_expect_block_len = next_block_length;
                    xiaoai_ota_single_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_PAYLOAD_OTA_DATA;
                }
            }
        }
        break;

    case XM_OPCODE_OTA_GET_DEVICE_REFRESH_FIRMWARE_STATUS:
        {
            T_XM_OTA_UPDATE_RESULT update_result = XM_OTA_UPDATE_RESULT_SUCCESS;
            if (xiaoai_ota_single_mgr.update_result_local)
            {
                update_result = XM_OTA_UPDATE_RESULT_SUCCESS;
            }
            else
            {
                update_result = XM_OTA_UPDATE_RESULT_FAIL;
            }

            xm_cmd_ota_get_device_refresh_firmware_status_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS,
                                                              opcode_sn, update_result);
        }
        break;
    case XM_OPCODE_OTA_NOTIFY_UPDATE_MODE:
        {
            xm_cmd_ota_notify_update_mode_rsp(type, bd_addr, conn_id, XM_CMD_STATUS_SUCCESS, opcode_sn);
        }
        break;
    case XM_OPCODE_OTA_RESERVED:
        {

        }
        break;

    default:
        break;
    }
}
#endif

#if F_APP_ERWS_SUPPORT
static void xiaoai_ota_send_remote_msg(T_XIAOAI_OTA_REMOTE_MSG_ID msg_id,
                                       T_XIAOAI_OTA_REMOTE_MSG *p_remote_info,
                                       uint16_t msg_len)
{
    APP_PRINT_TRACE1("xiaoai_ota_send_remote_msg: msg_id 0x%x", msg_id);

    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_XIAOAI_OTA, msg_id,
                                        (uint8_t *)p_remote_info, msg_len);
}

void xiaoai_ota_p2s_send_image_header_and_bin0_info(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                                    uint8_t conn_id)
{
    T_XIAOAI_OTA_REMOTE_MSG_ID msg_id = XIAOAI_OTA_P2S_MSG_IMAGE_HEADER_AND_BIN0_INFO;
    T_XIAOAI_OTA_REMOTE_MSG remote_info;
    uint16_t mtu = 20;

    xiaoai_ota_get_ota_mtu_size(type, bd_addr, conn_id, &mtu);

    remote_info.p2s_image_header_and_bin0_info.mtu = mtu;
    memcpy(&remote_info.p2s_image_header_and_bin0_info.combine_head, &xiaoai_ota_mgr.combine_head,
           sizeof(xiaoai_ota_mgr.combine_head));

    memcpy(&remote_info.p2s_image_header_and_bin0_info.sub_bin0, &xiaoai_ota_mgr.sub_bin[0],
           sizeof(xiaoai_ota_mgr.sub_bin[0]));

    xiaoai_ota_send_remote_msg(msg_id, &remote_info,
                               sizeof(remote_info.p2s_image_header_and_bin0_info));
}

void xiaoai_ota_p2s_send_bin1_info(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id)
{
    T_XIAOAI_OTA_REMOTE_MSG_ID msg_id = XIAOAI_OTA_P2S_MSG_BIN1_INFO;
    T_XIAOAI_OTA_REMOTE_MSG remote_info;

    memcpy(&remote_info.p2s_image_bin1_info.sub_bin1, &xiaoai_ota_mgr.sub_bin[1],
           sizeof(xiaoai_ota_mgr.sub_bin[1]));

    xiaoai_ota_send_remote_msg(msg_id, &remote_info,
                               sizeof(remote_info.p2s_image_header_and_bin0_info));
}

void xiaoai_ota_p2s_send_notify_image_rx_complete(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                                  uint8_t conn_id,
                                                  uint8_t status, uint8_t opcode_sn, uint8_t result)
{
    T_XIAOAI_OTA_REMOTE_MSG_ID msg_id = XIAOAI_OTA_P2S_NOTIFY_IMAGE_RX_COMPLETE;
    T_XIAOAI_OTA_REMOTE_MSG remote_info;

    remote_info.p2s_notify_rx_image_complete.type = type;
    memcpy(remote_info.p2s_notify_rx_image_complete.bd_addr, bd_addr, 6);
    remote_info.p2s_notify_rx_image_complete.conn_id = conn_id;
    remote_info.p2s_notify_rx_image_complete.status = status;
    remote_info.p2s_notify_rx_image_complete.opcode_sn = opcode_sn;
    remote_info.p2s_notify_rx_image_complete.update_result = result;

    xiaoai_ota_send_remote_msg(msg_id, &remote_info, sizeof(remote_info.p2s_notify_rx_image_complete));
}

void xiaoai_ota_p2s_send_firmware_update_block(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                               uint8_t conn_id,
                                               uint8_t opcode, bool req_rsp, uint8_t *p_data, uint16_t len)
{
    T_XIAOAI_OTA_REMOTE_MSG_ID msg_id = XIAOAI_OTA_P2S_MSG_FIRMWARE_UPDATE_BLOCK;
    T_XIAOAI_OTA_REMOTE_MSG *p_remote_info = (T_XIAOAI_OTA_REMOTE_MSG *)xiaoai_mem_zalloc(
                                                 sizeof(T_XIAOAI_OTA_P2S_MSG_FIRMWARE_UPDATE_BLOCK) + len);

    if (p_remote_info)
    {
        p_remote_info->p2s_firmware_update_block.type = type;
        memcpy(p_remote_info->p2s_firmware_update_block.bd_addr, bd_addr, 6);
        p_remote_info->p2s_firmware_update_block.conn_id = conn_id;
        p_remote_info->p2s_firmware_update_block.opcode = opcode;
        p_remote_info->p2s_firmware_update_block.req_rsp = req_rsp;
        p_remote_info->p2s_firmware_update_block.len = len;
        memcpy(p_remote_info->p2s_firmware_update_block.data, p_data, len);

        xiaoai_ota_send_remote_msg(msg_id, p_remote_info,
                                   sizeof(p_remote_info->p2s_firmware_update_block) + len);
        xiaoai_mem_free(p_remote_info, sizeof(T_XIAOAI_OTA_REMOTE_MSG) + len);
    }
}

void xiaoai_ota_p2s_send_device_reboot(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                       uint8_t conn_id,
                                       uint8_t opcode_sn)
{
    T_XIAOAI_OTA_REMOTE_MSG_ID msg_id = XIAOAI_OTA_P2S_MSG_DEVICE_REBOOT;
    T_XIAOAI_OTA_REMOTE_MSG remote_info;

    remote_info.p2s_device_reboot.type = type;
    memcpy(remote_info.p2s_device_reboot.bd_addr, bd_addr, 6);
    remote_info.p2s_device_reboot.opcode_sn = opcode_sn;

    xiaoai_ota_send_remote_msg(msg_id, &remote_info, sizeof(remote_info.p2s_device_reboot));
}

void xiaoai_ota_send_p2s_ack(T_XIAOAI_OTA_REMOTE_MSG_ID msg_id_acked)
{
    T_XIAOAI_OTA_REMOTE_MSG_ID msg_id = XIAOAI_OTA_P2S_MSG_ACK;
    T_XIAOAI_OTA_REMOTE_MSG remote_info;
    remote_info.p2s_ack.msg_id_acked = msg_id_acked;

    xiaoai_ota_send_remote_msg(msg_id, &remote_info, sizeof(remote_info.p2s_ack));
}

void xiaoai_ota_p2s_send_ota_start(T_XIAOAI_OTA_REMOTE_MSG_ID msg_id_ota_start)
{
    T_XIAOAI_OTA_REMOTE_MSG_ID msg_id = XIAOAI_OTA_P2S_MSG_OTA_START;
    T_XIAOAI_OTA_REMOTE_MSG remote_info;
    remote_info.p2s_ota_start.msg_id_ota_start = msg_id_ota_start;

    xiaoai_ota_send_remote_msg(msg_id, &remote_info, sizeof(remote_info.p2s_ota_start));
}

void xiaoai_ota_s2p_send_notify_image_rx_complete(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                                  uint8_t conn_id,
                                                  uint8_t status, uint8_t opcode_sn, uint8_t update_result)
{
    T_XIAOAI_OTA_REMOTE_MSG_ID msg_id = XIAOAI_OTA_S2P_NOTIFY_IMAGE_RX_COMPLETE;
    T_XIAOAI_OTA_REMOTE_MSG remote_info;

    remote_info.s2p_notify_rx_image_complete.type = type;
    memcpy(remote_info.s2p_notify_rx_image_complete.bd_addr, bd_addr, 6);
    remote_info.s2p_notify_rx_image_complete.conn_id = conn_id;
    remote_info.s2p_notify_rx_image_complete.status = status;
    remote_info.s2p_notify_rx_image_complete.opcode_sn = opcode_sn;
    remote_info.s2p_notify_rx_image_complete.update_result = update_result;

    xiaoai_ota_send_remote_msg(msg_id, &remote_info, sizeof(remote_info.s2p_notify_rx_image_complete));
}

void xiaoai_ota_s2p_send_firmware_update_block_rsp(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                                   uint8_t conn_id,
                                                   uint8_t status, uint8_t opcode_sn, uint8_t update_result,
                                                   uint32_t next_block_offset, uint16_t next_block_length, uint16_t delay_ms)
{
    T_XIAOAI_OTA_REMOTE_MSG_ID msg_id = XIAOAI_OTA_S2P_MSG_FIRMWARE_UPDATE_BLOCK_RSP;
    T_XIAOAI_OTA_REMOTE_MSG remote_info;

    remote_info.s2p_firmware_update_block_rsp.type = type;
    memcpy(remote_info.s2p_firmware_update_block_rsp.bd_addr, bd_addr, 6);
    remote_info.s2p_firmware_update_block_rsp.conn_id = conn_id;
    remote_info.s2p_firmware_update_block_rsp.status = status;
    remote_info.s2p_firmware_update_block_rsp.opcode_sn = opcode_sn;
    remote_info.s2p_firmware_update_block_rsp.update_result = update_result;
    remote_info.s2p_firmware_update_block_rsp.next_block_offset = next_block_offset;
    remote_info.s2p_firmware_update_block_rsp.next_block_length = next_block_length;
    remote_info.s2p_firmware_update_block_rsp.delay_ms = delay_ms;

    APP_PRINT_INFO3("xiaoai_ota_s2p_send_firmware_update_block_rsp: next_block_offset 0x%08x, next_block_length %d, status %d",
                    next_block_offset, next_block_length, status);

    xiaoai_ota_send_remote_msg(msg_id, &remote_info, sizeof(remote_info.s2p_firmware_update_block_rsp));
}

void xiaoai_ota_send_s2p_ack(T_XIAOAI_OTA_S2P_MSG_ACK msg_ack)
{
    T_XIAOAI_OTA_REMOTE_MSG_ID msg_id = XIAOAI_OTA_S2P_MSG_ACK;
    T_XIAOAI_OTA_REMOTE_MSG remote_info;
    remote_info.s2p_ack = msg_ack;

    xiaoai_ota_send_remote_msg(msg_id, &remote_info, sizeof(remote_info.s2p_ack));
}

void xiaoai_ota_secondary_handle_notify_rx_image_complete(T_XM_CMD_COMM_WAY_TYPE type,
                                                          uint8_t *bd_addr, uint8_t conn_id, uint8_t status, uint8_t opcode_sn, uint8_t update_result)
{
    uint16_t delay_ms = 0;
    uint32_t next_block_offset = 0;
    uint16_t next_block_length = 0;

    APP_PRINT_INFO1("xiaoai_ota_secondary_handle_notify_rx_image_complete: start update %d sub image",
                    xiaoai_ota_mgr.cur_sub_image_index);

    //request 1st image, skip mp header OTA_SUB_IMAGE_MP_HEAD_SIZE = 512 bytes
    xiaoai_ota_mgr.cur_sub_image_relative_offset += OTA_SUB_IMAGE_MP_HEAD_SIZE;

    next_block_offset = xiaoai_ota_mgr.cur_sub_image_relative_offset +
                        xiaoai_ota_mgr.sub_bin[xiaoai_ota_mgr.cur_sub_bin_index].sub_image_header[xiaoai_ota_mgr.cur_sub_image_index].start_file_offset;
    next_block_length = OTA_SUB_IMAGE_PAYLOAD_IMAGE_CONTROL_HEAD_SIZE;
#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
    next_block_length = xiaoai_ota_mgr.mtu;
    if (xiaoai_ota_mgr.cur_sub_image_relative_offset + next_block_length >
        OTA_SUB_IMAGE_MP_HEAD_SIZE + OTA_SUB_IMAGE_PAYLOAD_IMAGE_AUTH_HEAD_SIZE)
    {
        next_block_length =
            OTA_SUB_IMAGE_MP_HEAD_SIZE + OTA_SUB_IMAGE_PAYLOAD_IMAGE_AUTH_HEAD_SIZE //512+304 - relative
            -
            xiaoai_ota_mgr.cur_sub_image_relative_offset;
    }
    APP_PRINT_INFO1("xiaoai_final_next_block_length 0x%04x", next_block_length);
#endif
    xiaoai_ota_s2p_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                  update_result, next_block_offset, next_block_length, delay_ms);

    xiaoai_ota_mgr.cur_expect_block_len = next_block_length;

#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
    xiaoai_ota_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_AUTH_HEADER;
#else
    xiaoai_ota_mgr.state = XIAOAI_OTA_STATE_RX_SUB_IMAGE_HEADER;
#endif
}

#if F_APP_ERWS_SUPPORT
static void app_xiaoai_ota_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                       T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE1("xiaoai_ota_relay_cback: msg_id 0x%04x", msg_type);

    if (status != REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        APP_PRINT_ERROR1("xiaoai_ota_relay_cback: invalid status %d", status);
        return;
    }
    T_XIAOAI_OTA_REMOTE_MSG *p_info = (T_XIAOAI_OTA_REMOTE_MSG *)buf;
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        switch (msg_type)
        {
        case XIAOAI_OTA_P2S_MSG_IMAGE_HEADER_AND_BIN0_INFO:
            {
                xiaoai_ota_mgr.mtu = p_info->p2s_image_header_and_bin0_info.mtu;
                memcpy(&xiaoai_ota_mgr.combine_head, &p_info->p2s_image_header_and_bin0_info.combine_head,
                       sizeof(xiaoai_ota_mgr.combine_head));
                memcpy(&xiaoai_ota_mgr.sub_bin[0], &p_info->p2s_image_header_and_bin0_info.sub_bin0,
                       sizeof(xiaoai_ota_mgr.sub_bin[0]));
            }
            break;

        case XIAOAI_OTA_P2S_MSG_BIN1_INFO:
            {
                memcpy(&xiaoai_ota_mgr.sub_bin[1], &p_info->p2s_image_bin1_info.sub_bin1,
                       sizeof(xiaoai_ota_mgr.sub_bin[1]));
            }
            break;

        case XIAOAI_OTA_P2S_NOTIFY_IMAGE_RX_COMPLETE:
            {
                T_XM_CMD_COMM_WAY_TYPE type = p_info->p2s_notify_rx_image_complete.type;
                uint8_t *bd_addr = p_info->p2s_notify_rx_image_complete.bd_addr;
                uint8_t conn_id = p_info->p2s_notify_rx_image_complete.conn_id;
                uint8_t status = p_info->p2s_notify_rx_image_complete.status;
                uint8_t opcode_sn = p_info->p2s_notify_rx_image_complete.opcode_sn;
                uint8_t update_result = p_info->p2s_notify_rx_image_complete.update_result;

                xiaoai_ota_mgr.update_complete_remote = true;
                xiaoai_ota_mgr.update_result_remote = true;

                xiaoai_ota_check_set_cur_sub_bin_info();

                //app_flash_set_bp_lv(0);

                xiaoai_ota_secondary_handle_notify_rx_image_complete(type, bd_addr, conn_id, status, opcode_sn,
                                                                     update_result);
            }
            break;

        case XIAOAI_OTA_P2S_MSG_FIRMWARE_UPDATE_BLOCK:
            {
                T_XM_CMD_COMM_WAY_TYPE type = p_info->p2s_firmware_update_block.type;
                uint8_t *bd_addr = p_info->p2s_firmware_update_block.bd_addr;
                uint8_t conn_id = p_info->p2s_firmware_update_block.conn_id;
                uint8_t opcode = p_info->p2s_firmware_update_block.opcode;
                bool req_rsp = p_info->p2s_firmware_update_block.req_rsp;
                uint16_t len = p_info->p2s_firmware_update_block.len;
                uint8_t *p_data = p_info->p2s_firmware_update_block.data;

                xiaoai_ota_handle_cmd(type, bd_addr, conn_id, opcode, req_rsp, p_data, len);

            }
            break;

        case XIAOAI_OTA_P2S_MSG_DEVICE_REBOOT:
            {
                T_XIAOAI_OTA_S2P_MSG_ACK msg_ack;

                msg_ack.msg_id_acked = XIAOAI_OTA_P2S_MSG_DEVICE_REBOOT;
                msg_ack.msg.dev_reboot.type = p_info->p2s_device_reboot.type;
                memcpy(msg_ack.msg.dev_reboot.bd_addr, p_info->p2s_device_reboot.bd_addr, 6);
                msg_ack.msg.dev_reboot.conn_id = p_info->p2s_device_reboot.conn_id;
                msg_ack.msg.dev_reboot.opcode_sn = p_info->p2s_device_reboot.opcode_sn;

                xiaoai_ota_send_s2p_ack(msg_ack);

                app_device_reboot(100, RESET_ALL);
            }
            break;

        case XIAOAI_OTA_P2S_MSG_ACK:
            {

            }
            break;

        case XIAOAI_OTA_P2S_MSG_OTA_START:
            {
                app_auto_power_off_disable(AUTO_POWER_OFF_MASK_XIAOAI_OTA);
            }
            break;

        default:
            break;
        }
    }
    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        switch (msg_type)
        {
        case XIAOAI_OTA_S2P_NOTIFY_IMAGE_RX_COMPLETE:
            {
                T_XM_CMD_COMM_WAY_TYPE type = p_info->s2p_notify_rx_image_complete.type;
                uint8_t *bd_addr = p_info->s2p_notify_rx_image_complete.bd_addr;
                uint8_t conn_id = p_info->s2p_notify_rx_image_complete.conn_id;
                uint8_t status = p_info->s2p_notify_rx_image_complete.status;
                uint8_t opcode_sn = p_info->s2p_notify_rx_image_complete.opcode_sn;
                uint8_t update_result = p_info->s2p_notify_rx_image_complete.update_result;

                xiaoai_ota_mgr.update_complete_remote = true;

                if (update_result == 0)
                {
                    xiaoai_ota_mgr.update_result_remote = true;
                }
                else
                {
                    xiaoai_ota_mgr.update_result_remote = false;
                }

                xiaoai_ota_update_complete_check(type, bd_addr, conn_id, status, opcode_sn);

            }
            break;

        case XIAOAI_OTA_S2P_MSG_FIRMWARE_UPDATE_BLOCK_RSP:
            {
                T_XM_CMD_COMM_WAY_TYPE type = p_info->s2p_firmware_update_block_rsp.type;
                uint8_t *bd_addr = p_info->s2p_firmware_update_block_rsp.bd_addr;
                uint8_t status = p_info->s2p_firmware_update_block_rsp.status;
                uint8_t conn_id = p_info->s2p_firmware_update_block_rsp.conn_id;
                uint8_t opcode_sn = p_info->s2p_firmware_update_block_rsp.opcode_sn;
                uint8_t update_result = p_info->s2p_firmware_update_block_rsp.update_result;
                uint32_t next_block_offset = p_info->s2p_firmware_update_block_rsp.next_block_offset;
                uint16_t next_block_length = p_info->s2p_firmware_update_block_rsp.next_block_length;
                uint16_t delay_ms = p_info->s2p_firmware_update_block_rsp.delay_ms;

                xm_cmd_ota_send_firmware_update_block_rsp(type, bd_addr, conn_id, status, opcode_sn,
                                                          update_result, next_block_offset, next_block_length, delay_ms);
            }
            break;

        case XIAOAI_OTA_S2P_MSG_ACK:
            {
                uint16_t msg_id_acked = p_info->s2p_ack.msg_id_acked;
                if (msg_id_acked == XIAOAI_OTA_P2S_MSG_DEVICE_REBOOT)
                {
                    T_XM_CMD_COMM_WAY_TYPE type = p_info->s2p_ack.msg.dev_reboot.type;
                    uint8_t *bd_addr = p_info->s2p_ack.msg.dev_reboot.bd_addr;
                    uint8_t status = XM_CMD_STATUS_SUCCESS;
                    uint8_t opcode_sn = p_info->s2p_ack.msg.dev_reboot.opcode_sn;
                    uint8_t conn_id = p_info->s2p_ack.msg.dev_reboot.conn_id;
                    xm_cmd_dev_reboot_rsp(type, bd_addr, conn_id, status, opcode_sn);

                    app_device_reboot(100, RESET_ALL);
                }
            }
            break;

        default:
            break;
        }
    }
}
#endif
#endif

bool xiaoai_ota_init(void)
{
    bool ret = true;
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(NULL, app_xiaoai_ota_parse_cback,
                             APP_MODULE_TYPE_XIAOAI_OTA, XIAOAI_OTA_REMOTE_MSG_NUM);
#endif
    return ret;
}
#endif
