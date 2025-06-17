/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _XM_XIAOAI_RCSP_H_
#define _XM_XIAOAI_RCSP_H_

#include <stdint.h>
#include <stdbool.h>
#include <xm_xiaoai_api.h>

#ifdef __cplusplus
extern "C"  {
#endif

#define XM_PKT_PREAMBLE_SIZE        3
#define XM_PKT_TRAILER_SIZE         1
#define XM_PKT_TRAILER_HDR_SIZE     4

#define XM_PKT_PREAMBLE 0xfedcba
#define XM_PKT_TRAILER  0xef

typedef enum
{
    XM_CMD = 0,
    XM_CMD_R,
    XM_DATA,
    XM_DATA_R
} T_XM_PKT_TYPE;

typedef struct
{
    uint8_t type;
    uint8_t len;
    uint8_t data[1];
} T_XM_ATTR_DATA;

typedef struct
{
    uint8_t id;
    uint8_t need_asr_partial;
} T_XM_CMD_ASR_REQ_PARAM;

typedef struct
{
    uint8_t id;
    uint8_t status;
    uint8_t is_asr_partial;
    uint8_t result[1];
} T_XM_CMD_ASR_RESULT_PARAM;

typedef struct
{
    uint8_t id;
    uint8_t output_way;
    uint8_t compress_way;
    uint8_t package_size;
    uint8_t text[1];
} T_XM_CMD_TTS_REQ_PARAM;

typedef struct
{
    uint8_t id;
    uint8_t status;
} T_XM_CMD_TTS_RESULT_PARAM;

typedef struct
{
    uint8_t id;
    uint8_t text[1];
} T_XM_CMD_NLP_REQ_PARAM;

typedef struct
{
    uint8_t id;
    uint8_t status;
} T_XM_CMD_NLP_RESULT_PARAM;

typedef enum
{
    XM_CMD_ID_RSP = 0,
    XM_CMD_ID_CMD = 1
} T_XM_CMD_ID_TYPE;

typedef enum
{
    XM_REQ_RSP_TYPE_NO_RSP = 0,
    XM_REQ_RSP_TYPE_NEED_RSP = 1
} T_XM_REQ_RSP_TYPE;

typedef struct
{
    uint8_t opcode_sn;
    uint8_t data[1];
} __attribute__((packed)) T_XM_CMD_PAYLOAD;

typedef struct
{
    uint8_t status;
    uint8_t opcode_sn;
    uint8_t data[1];
} __attribute__((packed)) T_XM_CMD_R_PAYLOAD;

typedef struct
{
    uint8_t opcode_sn;
    uint8_t xm_cmd_opcode;
    uint8_t data[1];
} __attribute__((packed)) T_XM_DATA_PAYLOAD;

typedef struct
{
    uint8_t status;
    uint8_t opcode_sn;
    uint8_t xm_cmd_opcode;
    uint8_t data[1];
} __attribute__((packed)) T_XM_DATA_R_PAYLOAD;

typedef struct
{
    uint8_t preamble[3];
    uint8_t unused: 6;
    uint8_t req_rsp: 1;
    uint8_t cmd_id: 1;
    uint8_t opcode;
    uint16_t len;
    union
    {
        uint8_t padding[1];
        T_XM_CMD_PAYLOAD cmd;
        T_XM_CMD_R_PAYLOAD cmd_r;
        T_XM_DATA_PAYLOAD data;
        T_XM_DATA_R_PAYLOAD data_r;
    } payload;
} __attribute__((packed)) T_XM_PKT;

typedef enum
{
    T_XM_RCSP_ACCESS_AUTH_IDLE              = 0,
    T_XM_RCSP_ACCESS_AUTH_START             = 1,
    T_XM_RCSP_ACCESS_AUTH_RX_RAND           = 2,
    T_XM_RCSP_ACCESS_AUTH_TX_RAND_RESULT    = 3,
    T_XM_RCSP_ACCESS_AUTH_RX_AUTH_RESULT    = 4,
    T_XM_RCSP_ACCESS_AUTH_TX_RAND           = 5,
    T_XM_RCSP_ACCESS_AUTH_RX_RAND_RESULT    = 6,
    T_XM_RCSP_ACCESS_AUTH_TX_AUTH_RESULT    = 7,
    T_XM_RCSP_ACCESS_AUTH_COMPLETE          = 8
} __attribute__((packed)) T_XM_RCSP_ACCESS_AUTH_STATE;

typedef enum
{
    XM_ACCESS_AUTH_PKT_TYPE_RAND            = 0,
    XM_ACCESS_AUTH_PKT_TYPE_RAND_RESULT     = 1,
    XM_ACCESS_AUTH_PKT_TYPE_AUTH_RESULT     = 2,
} __attribute__((packed)) T_XM_ACCESS_AUTH_PKT_TYPE;

typedef struct
{
    T_XM_ACCESS_AUTH_PKT_TYPE pkt_type;
    uint8_t rand[16];
} __attribute__((packed)) T_XM_AUTH_PKT_RAND;

typedef struct
{
    T_XM_ACCESS_AUTH_PKT_TYPE pkt_type;
    uint8_t result[16];
} __attribute__((packed)) T_XM_AUTH_PKT_RAND_RESULT;

typedef struct
{
    T_XM_ACCESS_AUTH_PKT_TYPE pkt_type;
    uint8_t result[4];
} __attribute__((packed)) T_XM_AUTH_PKT_AUTH_RESULT;


void xm_rx_access_auth_pkt(T_XM_CMD_COMM_WAY_TYPE comm_way, uint8_t *bd_addr, uint8_t conn_id,
                           uint8_t *p_pkt,
                           uint16_t len);

#ifdef __cplusplus
}
#endif

#endif
