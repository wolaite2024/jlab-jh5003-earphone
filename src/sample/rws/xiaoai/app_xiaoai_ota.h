/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_XIAOAI_OTA_H_
#define _APP_XIAOAI_OTA_H_

#include <stdint.h>
#include <stdbool.h>
#include "remote.h"
#include <xm_xiaoai_api.h>
#include <app_xiaoai_ota_flash.h>
#include <patch_header_check.h>

#ifdef __cplusplus
extern "C"  {
#endif

#define OTA_COMBINE_HEAD_SIZE               28

#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
#define OTA_MERGED_FILE_HEAD_SUB_FILE_INDICATOR_SIZE      32
#define OTA_SUB_BIN_COUNT                                 21
#else
#define OTA_MERGED_FILE_HEAD_SUB_FILE_INDICATOR_SIZE      4
#define OTA_SUB_BIN_COUNT                                 18
#endif

#define OTA_MERGED_FILE_HEAD_SIZE           40 + OTA_MERGED_FILE_HEAD_SUB_FILE_INDICATOR_SIZE
#define OTA_MERGED_FILE_SUB_HEAD_SIZE       12
#define OTA_SUB_IMAGE_MP_HEAD_SIZE          512
#if (XIAOAI_OTA_NEW_IMAGE_HEADER_STRUCTURE_SUPPORT == 1)
#define OTA_SUB_IMAGE_PAYLOAD_IMAGE_AUTH_HEAD_SIZE    304
#endif
#define OTA_SUB_IMAGE_PAYLOAD_IMAGE_CONTROL_HEAD_SIZE 12

typedef enum
{
    XIAOAI_OTA_STATE_RX_INIT = 0,
    XIAOAI_OTA_STATE_RX_COMBINE_AND_BIN0_MERGED_HEADER,
    XIAOAI_OTA_STATE_RX_BIN0_SUB_IMAGE_HEADER,
    XIAOAI_OTA_STATE_RX_BIN1_MERGED_HEADER,
    XIAOAI_OTA_STATE_RX_BIN1_SUB_IMAGE_HEADER,
    XIAOAI_OTA_STATE_RX_SUB_IMAGE_AUTH_HEADER, //auth header is moved before control header in new header struct
    XIAOAI_OTA_STATE_RX_SUB_IMAGE_HEADER, //control header
    XIAOAI_OTA_STATE_RX_SUB_IMAGE_PAYLOAD_OTA_DATA,
    XIAOAI_OTA_STATE_RX_SUB_IMAGE_SECONDARY,
    XIAOAI_OTA_STATE_RX_SUB_IMAGE_FINISH,
} T_XIAOAI_OTA_STATE;

typedef struct _T_SUB_IMAGE_HEADER
{
    IMG_ID image_id;
    uint32_t download_addr;
    uint32_t size;
    uint32_t reserved;
    uint32_t start_file_offset;
} T_SUB_IMAGE_HEADER;

typedef struct _T_MERGED_HEADER
{
    uint16_t signature;
    uint32_t size_of_merged_file;
    uint8_t  checksum[32];
    uint16_t extension;
    uint8_t  sub_file_indicator[OTA_MERGED_FILE_HEAD_SUB_FILE_INDICATOR_SIZE];
    uint8_t  sub_file_count;
} T_MERGED_HEADER;

typedef struct _T_BIN_INFO
{
    uint32_t length; // length of this bin file
    uint8_t default_bud_role : 1; // if 1, for primary ear; if 0, for secondary ear
    uint8_t rsv : 7;
    uint8_t rsv2[3];
} T_BIN_INFO;

typedef struct _T_COMBINATION_OTA_IMAGE
{
    uint32_t signature;// const signature: 0x9697b5c8
    uint32_t version;//version of combine spec
    uint32_t num; //number of bin
    T_BIN_INFO bin_info[2]; // actually has num of "struct BIN_INFO"
} T_COMBINATION_OTA_IMAGE_HEADER;


typedef struct _T_SUB_IMAGE_INFO
{
    T_MERGED_HEADER merged_header;
    T_SUB_IMAGE_HEADER sub_image_header[OTA_SUB_BIN_COUNT];
} T_SUB_BIN_INFO;

typedef struct _T_XIAOAI_OTA_MGR
{
    bool update_result_local;
    bool update_result_remote;
    bool update_complete_local;
    bool update_complete_remote;

    T_XIAOAI_OTA_STATE state;
    uint8_t     cur_sub_bin_index;
    uint8_t     cur_sub_image_index;
    uint8_t     end_sub_image_index;
    uint32_t    cur_sub_image_relative_offset;
    uint16_t    cur_expect_block_len;

    uint16_t mtu;
    T_COMBINATION_OTA_IMAGE_HEADER combine_head;
    T_SUB_BIN_INFO sub_bin[2];

} T_XIAOAI_OTA_MGR;

typedef struct _T_XIAOAI_OTA_SINGLE_MGR
{
    bool update_result_local;
    bool update_complete_local;

    T_XIAOAI_OTA_STATE state;
    uint8_t     cur_sub_bin_index;
    uint8_t     cur_sub_image_index;
    uint8_t     end_sub_image_index;
    uint32_t    cur_sub_image_relative_offset;
    uint16_t    cur_expect_block_len;

    uint16_t mtu;
    T_SUB_BIN_INFO sub_bin;

} T_XIAOAI_OTA_SINGLE_MGR;

void xiaoai_ota_handle_cmd(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                           uint8_t opcode,
                           bool req_rsp, uint8_t *p_data, uint16_t len);

void xiaoai_ota_single_handle_cmd(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                                  uint8_t opcode,
                                  bool req_rsp, uint8_t *p_data, uint16_t len);

bool xiaoai_ota_init(void);

void xiaoai_ota_p2s_send_device_reboot(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr,
                                       uint8_t conn_id,
                                       uint8_t opcode_sn);

#ifdef __cplusplus
}
#endif

#endif
