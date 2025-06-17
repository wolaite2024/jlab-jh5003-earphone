/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "os_queue.h"
#include "trace.h"
#include "pm.h"
#include "dsp_shm.h"
#include "dsp_ipc.h"
#include "dsp_driver.h"
#if (CONFIG_SOC_SERIES_RTL8773D == 1)/*https://jira.realtek.com/browse/BBPRO3RD-718*/
#include "shm2_api.h"
#endif

#define DIPC_H2D_HEAD_LEN                   (4)
#define DIPC_H2D_LOAD_SUB_HEADER            (7)

/* DIPC Stream skip len */
#define DIPC_STREAM_SKIP_LEN(s, len)     {              \
        s += len;                                       \
    }

/* DIPC Stream to array */
#define DIPC_STREAM_TO_ARRAY(a, s, len)  {              \
        uint32_t ii;                                    \
        for (ii = 0; ii < len; ii++)                    \
        {                                               \
            *((uint8_t *)(a) + ii) = *s++;              \
        }                                               \
    }

/* DIPC Array to stream */
#define DIPC_ARRAY_TO_STREAM(s, a, len)  {              \
        uint32_t ii;                                    \
        for (ii = 0; ii < len; ii++)                    \
        {                                               \
            *s++ = *((uint8_t *)(a) + ii);              \
        }                                               \
    }

/* DIPC Little Endian stream to uint8 */
#define DIPC_LE_STREAM_TO_UINT8(u8, s)   {              \
        u8  = (uint8_t)(*s);                            \
        s  += 1;                                        \
    }

/* DIPC Little Endian stream to uint16 */
#define DIPC_LE_STREAM_TO_UINT16(u16, s) {              \
        u16  = ((uint16_t)(*(s + 0)) << 0) +            \
               ((uint16_t)(*(s + 1)) << 8);             \
        s   += 2;                                       \
    }

/* DIPC Little Endian stream to uint24 */
#define DIPC_LE_STREAM_TO_UINT24(u24, s) {              \
        u24  = ((uint32_t)(*(s + 0)) <<  0) +           \
               ((uint32_t)(*(s + 1)) <<  8) +           \
               ((uint32_t)(*(s + 2)) << 16);            \
        s   += 3;                                       \
    }

/* DIPC Little Endian stream to uint32 */
#define DIPC_LE_STREAM_TO_UINT32(u32, s) {              \
        u32  = ((uint32_t)(*(s + 0)) <<  0) +           \
               ((uint32_t)(*(s + 1)) <<  8) +           \
               ((uint32_t)(*(s + 2)) << 16) +           \
               ((uint32_t)(*(s + 3)) << 24);            \
        s   += 4;                                       \
    }

/* DIPC Little Endian uint8 to stream */
#define DIPC_LE_UINT8_TO_STREAM(s, u8)   {              \
        *s++ = (uint8_t)(u8);                           \
    }

/* DIPC Little Endian uint16 to stream */
#define DIPC_LE_UINT16_TO_STREAM(s, u16) {              \
        *s++ = (uint8_t)((u16) >> 0);                   \
        *s++ = (uint8_t)((u16) >> 8);                   \
    }

/* DIPC Little Endian uint24 to stream */
#define DIPC_LE_UINT24_TO_STREAM(s, u24) {              \
        *s++ = (uint8_t)((u24) >>  0);                  \
        *s++ = (uint8_t)((u24) >>  8);                  \
        *s++ = (uint8_t)((u24) >> 16);                  \
    }

/* DIPC Little Endian uint32 to stream */
#define DIPC_LE_UINT32_TO_STREAM(s, u32) {              \
        *s++ = (uint8_t)((u32) >>  0);                  \
        *s++ = (uint8_t)((u32) >>  8);                  \
        *s++ = (uint8_t)((u32) >> 16);                  \
        *s++ = (uint8_t)((u32) >> 24);                  \
    }

/* DIPC Little Endian array to uint8 */
#define DIPC_LE_ARRAY_TO_UINT8(u8, a)    {              \
        u8  = (uint8_t)(*(a + 0));                      \
    }

/* DIPC Little Endian array to uint16 */
#define DIPC_LE_ARRAY_TO_UINT16(u16, a)  {              \
        u16 = ((uint16_t)(*(a + 0)) << 0) +             \
              ((uint16_t)(*(a + 1)) << 8);              \
    }

/* DIPC Little Endian array to uint24 */
#define DIPC_LE_ARRAY_TO_UINT24(u24, a)  {              \
        u24 = ((uint32_t)(*(a + 0)) <<  0) +            \
              ((uint32_t)(*(a + 1)) <<  8) +            \
              ((uint32_t)(*(a + 2)) << 16);             \
    }

/* DIPC Little Endian array to uint32 */
#define DIPC_LE_ARRAY_TO_UINT32(u32, a) {               \
        u32 = ((uint32_t)(*(a + 0)) <<  0) +            \
              ((uint32_t)(*(a + 1)) <<  8) +            \
              ((uint32_t)(*(a + 2)) << 16) +            \
              ((uint32_t)(*(a + 3)) << 24);             \
    }

/* DIPC Little Endian uint8 to array */
#define DIPC_LE_UINT8_TO_ARRAY(a, u8)    {              \
        *((uint8_t *)(a) + 0) = (uint8_t)(u8);          \
    }

/* DIPC Little Endian uint16 to array */
#define DIPC_LE_UINT16_TO_ARRAY(a, u16)  {              \
        *((uint8_t *)(a) + 0) = (uint8_t)((u16) >> 0);  \
        *((uint8_t *)(a) + 1) = (uint8_t)((u16) >> 8);  \
    }

/* DIPC Little Endian uint24 to array */
#define DIPC_LE_UINT24_TO_ARRAY(a, u24) {               \
        *((uint8_t *)(a) + 0) = (uint8_t)((u24) >>  0); \
        *((uint8_t *)(a) + 1) = (uint8_t)((u24) >>  8); \
        *((uint8_t *)(a) + 2) = (uint8_t)((u24) >> 16); \
    }

/* DIPC Little Endian uint32 to array */
#define DIPC_LE_UINT32_TO_ARRAY(a, u32) {               \
        *((uint8_t *)(a) + 0) = (uint8_t)((u32) >>  0); \
        *((uint8_t *)(a) + 1) = (uint8_t)((u32) >>  8); \
        *((uint8_t *)(a) + 2) = (uint8_t)((u32) >> 16); \
        *((uint8_t *)(a) + 3) = (uint8_t)((u32) >> 24); \
    }

/* DIPC Big Endian stream to uint8 */
#define DIPC_BE_STREAM_TO_UINT8(u8, s)   {              \
        u8   = (uint8_t)(*(s + 0));                     \
        s   += 1;                                       \
    }

/* DIPC Big Endian stream to uint16 */
#define DIPC_BE_STREAM_TO_UINT16(u16, s) {              \
        u16  = ((uint16_t)(*(s + 0)) << 8) +            \
               ((uint16_t)(*(s + 1)) << 0);             \
        s   += 2;                                       \
    }

/* DIPC Big Endian stream to uint24 */
#define DIPC_BE_STREAM_TO_UINT24(u24, s) {              \
        u24  = ((uint32_t)(*(s + 0)) << 16) +           \
               ((uint32_t)(*(s + 1)) <<  8) +           \
               ((uint32_t)(*(s + 2)) <<  0);            \
        s   += 3;                                       \
    }

/* DIPC Big Endian stream to uint32 */
#define DIPC_BE_STREAM_TO_UINT32(u32, s) {              \
        u32  = ((uint32_t)(*(s + 0)) << 24) +           \
               ((uint32_t)(*(s + 1)) << 16) +           \
               ((uint32_t)(*(s + 2)) <<  8) +           \
               ((uint32_t)(*(s + 3)) <<  0);            \
        s   += 4;                                       \
    }

/* DIPC Big Endian uint8 to stream */
#define DIPC_BE_UINT8_TO_STREAM(s, u8)   {              \
        *s++ = (uint8_t)(u8);                           \
    }

/* DIPC Big Endian uint16 to stream */
#define DIPC_BE_UINT16_TO_STREAM(s, u16) {              \
        *s++ = (uint8_t)((u16) >> 8);                   \
        *s++ = (uint8_t)((u16) >> 0);                   \
    }

/* DIPC Big Endian uint24 to stream */
#define DIPC_BE_UINT24_TO_STREAM(s, u24) {              \
        *s++ = (uint8_t)((u24) >> 16);                  \
        *s++ = (uint8_t)((u24) >>  8);                  \
        *s++ = (uint8_t)((u24) >>  0);                  \
    }

/* DIPC Big Endian uint32 to stream */
#define DIPC_BE_UINT32_TO_STREAM(s, u32) {              \
        *s++ = (uint8_t)((u32) >> 24);                  \
        *s++ = (uint8_t)((u32) >> 16);                  \
        *s++ = (uint8_t)((u32) >>  8);                  \
        *s++ = (uint8_t)((u32) >>  0);                  \
    }

/* DIPC Big Endian array to uint8 */
#define DIPC_BE_ARRAY_TO_UINT8(u8, a)    {              \
        u8  = (uint8_t)(*(a + 0));                      \
    }

/* DIPC Big Endian array to uint16 */
#define DIPC_BE_ARRAY_TO_UINT16(u16, a)  {              \
        u16 = ((uint16_t)(*(a + 0)) << 8) +             \
              ((uint16_t)(*(a + 1)) << 0);              \
    }

/* DIPC Big Endian array to uint24 */
#define DIPC_BE_ARRAY_TO_UINT24(u24, a)  {              \
        u24 = ((uint32_t)(*(a + 0)) << 16) +            \
              ((uint32_t)(*(a + 1)) <<  8) +            \
              ((uint32_t)(*(a + 2)) <<  0);             \
    }

/* DIPC Big Endian array to uint32 */
#define DIPC_BE_ARRAY_TO_UINT32(u32, a)  {              \
        u32 = ((uint32_t)(*(a + 0)) << 24) +            \
              ((uint32_t)(*(a + 1)) << 16) +            \
              ((uint32_t)(*(a + 2)) <<  8) +            \
              ((uint32_t)(*(a + 3)) <<  0);             \
    }

/* DIPC Big Endian uint8 to array */
#define DIPC_BE_UINT8_TO_ARRAY(a, u8)    {              \
        *((uint8_t *)(a) + 0) = (uint8_t)(u8);          \
    }

/* DIPC Big Endian uint16 to array */
#define DIPC_BE_UINT16_TO_ARRAY(a, u16)  {              \
        *((uint8_t *)(a) + 0) = (uint8_t)((u16) >> 8);  \
        *((uint8_t *)(a) + 1) = (uint8_t)((u16) >> 0);  \
    }

/* DIPC Big Endian uint24 to array */
#define DIPC_BE_UINT24_TO_ARRAY(a, u24)  {              \
        *((uint8_t *)(a) + 0) = (uint8_t)((u24) >> 16); \
        *((uint8_t *)(a) + 1) = (uint8_t)((u24) >>  8); \
        *((uint8_t *)(a) + 2) = (uint8_t)((u24) >>  0); \
    }

/* DIPC Big Endian uint32 to array */
#define DIPC_BE_UINT32_TO_ARRAY(a, u32)  {              \
        *((uint8_t *)(a) + 0) = (uint8_t)((u32) >> 24); \
        *((uint8_t *)(a) + 1) = (uint8_t)((u32) >> 16); \
        *((uint8_t *)(a) + 2) = (uint8_t)((u32) >>  8); \
        *((uint8_t *)(a) + 3) = (uint8_t)((u32) >>  0); \
    }

#define SESSION_ID_MASK             (0xFFFFFFFE)

#define RWS_SPK2_AUDIO_SYNC_INITIAL         0x00
#define RWS_SPK2_AUDIO_SYNC_UNLOCK          0x01
#define RWS_SPK2_AUDIO_SYNC_LOCK            0x02
#define RWS_SPK2_AUDIO_SYNC_V2_SUCCESSFUL   0x03
#define RWS_SPK2_AUDIO_SYNC_EMPTY           0xFF
#define RWS_SPK2_AUDIO_SYNC_LOSE_TIMESTAMP  0xFE
#define RWS_SPK2_AUDIO_SYNC_FAIL            0xFD
#define RWS_START_BTCLK_EXPIRED             0xF2

typedef struct t_dsp_ipc_cback_elem
{
    struct t_dsp_ipc_cback_elem *next;
    P_DSP_IPC_CBACK              cback;
} T_DSP_IPC_CBACK_ELEM;

typedef struct t_dsp_ipc_db
{
    T_OS_QUEUE cback_list;
} T_DSP_IPC_DB;

static bool dsp_ipc_event_parser(uint16_t  cmd_id,
                                 uint8_t  *param);
static void dipc_event_parser(uint16_t  event,
                              uint8_t  *payload,
                              uint16_t  payload_length);

static T_DSP_IPC_DB dsp_ipc_db = {0};

bool dsp_ipc_data_send(void     *addr,
                       uint8_t  *buffer,
                       uint16_t  length,
                       bool      flush)
{
    return dsp_shm_data_send(addr, buffer, length, flush);
}

uint16_t dsp_ipc_data_recv(void     *addr,
                           uint8_t  *buffer,
                           uint16_t  length)
{
    return dsp_shm_data_recv(addr, buffer, length);
}

uint16_t dsp_ipc_data_len_peek(void *addr)
{
    return dsp_shm_data_len_peek(addr);
}

uint16_t dsp_ipc_data_flush(void *addr)
{
    return dsp_shm_data_flush(addr);
}

bool dsp_ipc_set_fifo_len(T_DSP_FIFO_TRX_DIRECTION direction,
                          uint8_t                  lower_len)
{
    uint32_t cmd_length = 1;
    uint32_t cmd_buf_len = 8;
    uint8_t cmd_buf[8] = {0};

    cmd_buf[0] = (uint8_t)(H2D_FIFO_REQUEST);
    cmd_buf[1] = (uint8_t)(H2D_FIFO_REQUEST >> 8);
    cmd_buf[2] = (uint8_t)(cmd_length);
    cmd_buf[3] = (uint8_t)(cmd_length >> 8);
    cmd_buf[4] = direction;
    cmd_buf[5] = 0;
    cmd_buf[6] = lower_len;
    cmd_buf[7] = 0;

    return dsp_shm_cmd_send(cmd_buf, cmd_buf_len, true);
}

bool dsp_ipc_set_decode_emtpy(bool enable)
{
    uint32_t cmd_length = 1;
    uint32_t cmd_buf_len = 8;
    uint8_t cmd_buf[8] = {0};

    cmd_buf[0] = (uint8_t)(H2D_FIFO_EMPTY_REPORT_SET);
    cmd_buf[1] = (uint8_t)(H2D_FIFO_EMPTY_REPORT_SET >> 8);
    cmd_buf[2] = (uint8_t)(cmd_length);
    cmd_buf[3] = (uint8_t)(cmd_length >> 8);
    cmd_buf[4] = enable;

    return dsp_shm_cmd_send(cmd_buf, cmd_buf_len, true);
}

bool dsp_ipc_synchronization_data_send(uint8_t  *buf,
                                       uint16_t  len)
{
    uint32_t cmd_length = len / 4;
    uint32_t cmd_buf_len = (cmd_length + 1) * 4;
    uint8_t  cmd_buf[24] = {0};

    cmd_buf[0] = (uint8_t)(H2D_B2BMSG_INTERACTION_RECEIVED);
    cmd_buf[1] = (uint8_t)(H2D_B2BMSG_INTERACTION_RECEIVED >> 8);
    cmd_buf[2] = (uint8_t)(cmd_length);
    cmd_buf[3] = (uint8_t)(cmd_length >> 8);
    memcpy(&cmd_buf[4], buf, len);

    return dsp_shm_cmd_send(cmd_buf, cmd_buf_len, true);
}

bool dsp_ipc_set_b2bmsg_interaction_timeout(uint8_t param)
{
    uint32_t cmd_length = 0;
    uint32_t cmd_buf_len = 4;
    uint8_t cmd_buf[4];

    cmd_buf[0] = (uint8_t)(H2D_B2BMSG_INTERACTION_TIMEOUT);
    cmd_buf[1] = (uint8_t)(H2D_B2BMSG_INTERACTION_TIMEOUT >> 8);
    cmd_buf[2] = (uint8_t)(cmd_length);
    cmd_buf[3] = (uint8_t)(cmd_length >> 8);

    return dsp_shm_cmd_send(cmd_buf, cmd_buf_len, true);
}

bool dsp_ipc_set_rws_init(uint8_t  bt_clk_index,
                          uint32_t sample_rate,
                          uint8_t  type)
{
    uint32_t cmd_length = 2;
    uint32_t cmd_buf_len = 16;
    uint8_t cmd_buf[16] = {0};

    cmd_buf[0] = (uint8_t)(H2D_RWS_INIT);
    cmd_buf[1] = (uint8_t)(H2D_RWS_INIT >> 8);
    cmd_buf[2] = (uint8_t)(cmd_length);
    cmd_buf[3] = (uint8_t)(cmd_length >> 8);
    cmd_buf[4] = bt_clk_index;
    cmd_buf[8] = (uint8_t)sample_rate;
    cmd_buf[9] = (uint8_t)(sample_rate >> 8);
    cmd_buf[10] = (uint8_t)(sample_rate >> 16);
    cmd_buf[11] = (uint8_t)(sample_rate >> 24);
    cmd_buf[12] = type;

    return dsp_shm_cmd_send(cmd_buf, cmd_buf_len, true);
}

bool dsp_ipc_set_rws_seamless(uint8_t param)
{
    uint32_t cmd_length = 1;
    uint32_t cmd_buf_len = 8;
    uint8_t cmd_buf[8] = {0};

    cmd_buf[0] = (uint8_t)(H2D_RWS_SEAMLESS);
    cmd_buf[1] = (uint8_t)(H2D_RWS_SEAMLESS >> 8);
    cmd_buf[2] = (uint8_t)(cmd_length);
    cmd_buf[3] = (uint8_t)(cmd_length >> 8);
    cmd_buf[4] = param;

    return dsp_shm_cmd_send(cmd_buf, cmd_buf_len, true);
}

bool dsp_ipc_set_rws(uint8_t session_role,
                     uint8_t role)
{
    uint32_t cmd_length = 1;
    uint32_t cmd_buf_len = 8;
    uint8_t cmd_buf[8] = {0};

    cmd_buf[0] = (uint8_t)(H2D_RWS_SET);
    cmd_buf[1] = (uint8_t)(H2D_RWS_SET >> 8);
    cmd_buf[2] = (uint8_t)(cmd_length);
    cmd_buf[3] = (uint8_t)(cmd_length >> 8);
    cmd_buf[4] = session_role;
    cmd_buf[5] = role;

    return dsp_shm_cmd_send(cmd_buf, cmd_buf_len, true);
}

bool dsp_ipc_set_log_output_sel(uint8_t param)
{
    uint32_t cmd_length = 1;
    uint32_t cmd_buf_len = 8;
    uint8_t cmd_buf[8] = {0};

    cmd_buf[0] = (uint8_t)(H2D_CMD_LOG_OUTPUT_SEL);
    cmd_buf[1] = (uint8_t)(H2D_CMD_LOG_OUTPUT_SEL >> 8);
    cmd_buf[2] = (uint8_t)(cmd_length);
    cmd_buf[3] = (uint8_t)(cmd_length >> 8);
    cmd_buf[4] = param;

    return dsp_shm_cmd_send(cmd_buf, cmd_buf_len, true);
}

// special ipc for dsp2
bool dsp_ipc_set_dsp2_log_output_sel(uint8_t param)
{
#if (CONFIG_SOC_SERIES_RTL8773D == 1)/*https://jira.realtek.com/browse/BBPRO3RD-718*/
    uint32_t cmd_length = 1;
    uint32_t cmd_buf_len = 8;
    uint8_t cmd_buf[8] = {0};

    cmd_buf[0] = (uint8_t)(H2D_CMD_LOG_OUTPUT_SEL);
    cmd_buf[1] = (uint8_t)(H2D_CMD_LOG_OUTPUT_SEL >> 8);
    cmd_buf[2] = (uint8_t)(cmd_length);
    cmd_buf[3] = (uint8_t)(cmd_length >> 8);
    cmd_buf[4] = param;

    return shm2_cmd_send(cmd_buf, cmd_buf_len, true);
#else
    return true;
#endif
}

// special ipc for dsp1
bool dsp_ipc_set_dsp1_uart(uint8_t param)
{
    uint32_t cmd_length = 1;
    uint32_t cmd_buf_len = 8;
    uint8_t cmd_buf[8] = {0};

    cmd_buf[0] = (uint8_t)(H2D_CMD_DSP1_UART_CFG);
    cmd_buf[1] = (uint8_t)(H2D_CMD_DSP1_UART_CFG >> 8);
    cmd_buf[2] = (uint8_t)(cmd_length);
    cmd_buf[3] = (uint8_t)(cmd_length >> 8);
    cmd_buf[4] = param;

    return dsp_shm_cmd_send(cmd_buf, cmd_buf_len, true);
}

// special ipc for dsp2
bool dsp_ipc_set_dsp2_uart(uint8_t param)
{
#if (CONFIG_SOC_SERIES_RTL8773D == 1)/*https://jira.realtek.com/browse/BBPRO3RD-718*/
    uint32_t cmd_length = 1;
    uint32_t cmd_buf_len = 8;
    uint8_t cmd_buf[8] = {0};

    cmd_buf[0] = (uint8_t)(H2D_CMD_DSP2_UART_CFG);
    cmd_buf[1] = (uint8_t)(H2D_CMD_DSP2_UART_CFG >> 8);
    cmd_buf[2] = (uint8_t)(cmd_length);
    cmd_buf[3] = (uint8_t)(cmd_length >> 8);
    cmd_buf[4] = param;

    return shm2_cmd_send(cmd_buf, cmd_buf_len, true);
#else
    return true;
#endif
}

bool dsp_ipc_set_handover_info(uint8_t role,
                               bool    start)
{
    uint32_t cmd_length = 1;
    uint32_t cmd_buf_len = 8;
    uint8_t cmd_buf[8] = {0};

    cmd_buf[0] = (uint8_t)(H2D_HANDOVER_INFO);
    cmd_buf[1] = (uint8_t)(H2D_HANDOVER_INFO >> 8);
    cmd_buf[2] = (uint8_t)(cmd_length);
    cmd_buf[3] = (uint8_t)(cmd_length >> 8);
    cmd_buf[4] = (uint8_t)start;
    cmd_buf[5] = (uint8_t)role;

    return dsp_shm_cmd_send(cmd_buf, cmd_buf_len, true);
}

bool dsp_ipc_set_voice_trigger(uint32_t param)
{
    uint32_t cmd_length = 1;
    uint32_t cmd_buf_len = 8;
    uint8_t cmd_buf[8] = {0};

    cmd_buf[0] = (uint8_t)(H2D_DSP_VOICE_TRIGGER_SET);
    cmd_buf[1] = (uint8_t)(H2D_DSP_VOICE_TRIGGER_SET >> 8);
    cmd_buf[2] = (uint8_t)(cmd_length);
    cmd_buf[3] = (uint8_t)(cmd_length >> 8);
    cmd_buf[4] = (uint8_t)param;
    cmd_buf[5] = (uint8_t)(param >> 8);

    return dsp_shm_cmd_send(cmd_buf, cmd_buf_len, true);
}

bool dsp_ipc_set_force_dummy_pkt(uint32_t param)
{
    uint32_t cmd_length = 1;
    uint32_t cmd_buf_len = 8;
    uint8_t cmd_buf[8] = {0};

    cmd_buf[0] = (uint8_t)(H2D_FORCE_DUMMY_PKT);
    cmd_buf[1] = (uint8_t)(H2D_FORCE_DUMMY_PKT >> 8);
    cmd_buf[2] = (uint8_t)(cmd_length);
    cmd_buf[3] = (uint8_t)(cmd_length >> 8);
    cmd_buf[4] = (uint8_t)param;

    return dsp_shm_cmd_send(cmd_buf, cmd_buf_len, true);
}

static void dsp_ipc_drv_cback(uint32_t  event,
                              void     *msg)
{
    if (event == DSP_DRV_EVT_D2H)
    {
        uint16_t peek_len;
        peek_len = dsp_shm_evt_len_peek();
        if (peek_len != 0)
        {
            uint8_t *rcv_buffer;
            rcv_buffer = malloc(peek_len);
            if (rcv_buffer != NULL)
            {
                uint16_t rxlen = dsp_shm_evt_recv(rcv_buffer, peek_len);

                if (rxlen)
                {
                    uint16_t event;
                    uint16_t payload_length;
                    uint8_t *p;

                    p = rcv_buffer;

                    DIPC_LE_STREAM_TO_UINT16(event, p);
                    DIPC_LE_STREAM_TO_UINT16(payload_length, p);

                    if (DIPC_EIF_GET(event) == DIPC_EIF_CURRENT)
                    {
                        /* DIPC_PRINT_INFO2("dsp_ipc_drv_cback: cmd %b, rxlen %d", TRACE_BINARY(rxlen, rcv_buffer), rxlen); */
                        dipc_event_parser(event, p, payload_length);
                    }
                    else
                    {
                        dsp_ipc_event_parser(event, p);
                    }
                    dsp_send_msg(DSP_MSG_INTR_D2H_CMD, 0, NULL, 0);
                }
                free(rcv_buffer);
            }
        }
    }
}

bool dsp_ipc_init(void)
{
    os_queue_init(&dsp_ipc_db.cback_list);
    return dsp_drv_register_cback(dsp_ipc_drv_cback);
}

void dsp_ipc_deinit(void)
{
    T_DSP_IPC_CBACK_ELEM *elem;

    elem = os_queue_out(&dsp_ipc_db.cback_list);
    while (elem != NULL)
    {
        free(elem);
        elem = os_queue_out(&dsp_ipc_db.cback_list);
    }

    dsp_drv_unregister_cback(dsp_ipc_drv_cback);
}

static bool dsp_ipc_event_parser(uint16_t  cmd_id,
                                 uint8_t  *param)
{
    T_DSP_IPC_EVENT event = DSP_IPC_EVT_NONE;
    uint32_t parameter = 0;
    uint32_t temp_param[3] = {0};

    switch (cmd_id)
    {
    case D2H_UPLINK_SYNCREF_REQUEST:
        {
            /* FIXME https://jira.realtek.com/browse/BB2RD-656 */
            event = DSP_IPC_EVT_PROBE_SYNC_REF_REQUEST;
            parameter = *((uint32_t *)param); /* session id */
        }
        break;

    case D2H_SILENCE_DETECT_REPORT:
        {
            event = DSP_IPC_EVT_DETECT_SILENCE;
            parameter = param[0];
        }
        break;

    case D2H_DSP_STATUS_IND:
        {
            uint8_t ind_type;
            uint8_t ind_status;

            ind_type = param[0];
            ind_status = param[1];

            if (ind_type == D2H_IND_TYPE_KEYWORD)
            {
                event = DSP_IPC_EVT_KEYWORD;
            }
            else if (ind_type == D2H_IND_TYPE_SEG_SEND)
            {
                if (ind_status == D2H_IND_STATUS_SEG_SEND_REQ_DATA)
                {
                    event = DSP_IPC_EVT_SEG_SEND_REQ_DATA;
                }
                else if (ind_status == D2H_IND_STATUS_SEG_SEND_ERROR)
                {
                    event = DSP_IPC_EVT_SEG_SEND_ERROR;
                }
            }
        }
        break;

    case D2H_RWS_SLAVE_SYNC_STATUS:
        {
            uint32_t *pt = (uint32_t *)&param[0];
            uint8_t sync_state;

            parameter = (uint8_t)pt[2];
            sync_state = (uint8_t)pt[2];

            if (sync_state == RWS_SPK2_AUDIO_SYNC_V2_SUCCESSFUL)
            {
                event = DSP_IPC_EVT_DSP_SYNC_V2_SUCC;
            }
            else if (sync_state == RWS_SPK2_AUDIO_SYNC_FAIL)
            {
                event = DSP_IPC_EVT_DSP_UNSYNC;
            }
            else if (sync_state == RWS_START_BTCLK_EXPIRED)
            {
                event = DSP_IPC_EVT_BTCLK_EXPIRED;
            }
            else if (sync_state == RWS_SPK2_AUDIO_SYNC_UNLOCK)
            {
                event = DSP_IPC_EVT_DSP_SYNC_UNLOCK;
            }
            else if (sync_state == RWS_SPK2_AUDIO_SYNC_LOCK)
            {
                event = DSP_IPC_EVT_DSP_SYNC_LOCK;
            }
            else if (sync_state == RWS_SPK2_AUDIO_SYNC_EMPTY)
            {
                event = DSP_IPC_EVT_SYNC_EMPTY;
            }
            else if (sync_state == RWS_SPK2_AUDIO_SYNC_LOSE_TIMESTAMP)
            {
                event = DSP_IPC_EVT_SYNC_LOSE_TIMESTAMP;
            }
        }
        break;

    case D2H_B2BMSG_INTERACTION_SEND:
        {
            event = DSP_IPC_EVT_B2BMSG;
            parameter = (uint32_t)param;
        }
        break;

    case D2H_RWS_SEAMLESS_RETURN_INFO:
        {
            event = DSP_IPC_EVT_JOIN_CLK;
            uint8_t i;
            for (i = 0; i < 3; i++)
            {
                temp_param[i] |= param[3 + 4 * i];
                temp_param[i] = temp_param[i] << 8;

                temp_param[i] |= param[2 + 4 * i];
                temp_param[i] = temp_param[i] << 8;

                temp_param[i] |= param[1 + 4 * i];
                temp_param[i] = temp_param[i] << 8;

                temp_param[i] |= param[0 + 4 * i];
            }
            parameter = (uint32_t)(&(temp_param[0]));
        }
        break;

    case D2H_SCENARIO_STATE:
        {
            event = DSP_IPC_EVT_PROBE;
            parameter = (uint32_t)param;
        }
        break;

    case D2H_HA_STATUS_INFO:
        {
            event = DSP_IPC_EVT_HA_STATUS_INFO;
            parameter = (uint32_t)param;
        }
        break;

    case D2H_OVP_TRAINING_REPORT:
        {
            event = DSP_IPC_EVT_OVP_TRAINING_INFO;
            parameter = (uint32_t)param;
        }
        break;

    default:
        break;
    }

    if (event != DSP_IPC_EVT_NONE)
    {
        T_DSP_IPC_CBACK_ELEM *elem;

        elem = os_queue_peek(&dsp_ipc_db.cback_list, 0);
        while (elem != NULL)
        {
            elem->cback(event, parameter);
            elem = elem->next;
        }
    }

    return true;
}

bool dsp_ipc_cback_register(P_DSP_IPC_CBACK cback)
{
    T_DSP_IPC_CBACK_ELEM *elem;

    elem = calloc(1, sizeof(T_DSP_IPC_CBACK_ELEM));
    if (elem != NULL)
    {
        elem->cback = cback;
        os_queue_in(&dsp_ipc_db.cback_list, elem);
        return true;
    }

    return false;
}

bool dsp_ipc_cback_unregister(P_DSP_IPC_CBACK cback)
{
    T_DSP_IPC_CBACK_ELEM *elem;

    elem = os_queue_peek(&dsp_ipc_db.cback_list, 0);
    while (elem != NULL)
    {
        if (elem->cback == cback)
        {
            return os_queue_delete(&dsp_ipc_db.cback_list, elem);
        }

        elem = elem->next;
    }

    return false;
}

static bool dsp_ipc_action_send(T_ACTION_CONFIG     action,
                                T_DSP_IPC_LOGIC_IO *logic_io,
                                uint16_t            cmd_id,
                                bool                flush)
{
    uint32_t cmd_length;
    uint32_t cmd_buf_len;
    uint8_t *cmd_buf;

    cmd_length  = logic_io->tlv_cnt + 2;
    cmd_buf_len = ((cmd_length + 1) * 4);
    cmd_buf = calloc(1, cmd_buf_len);
    if (cmd_buf == NULL)
    {
        return false;
    }

    cmd_buf[0] = (uint8_t)(cmd_id);
    cmd_buf[1] = (uint8_t)(cmd_id >> 8);
    cmd_buf[2] = (uint8_t)(cmd_length);
    cmd_buf[3] = (uint8_t)(cmd_length >> 8);
    memcpy(&cmd_buf[4], &action, sizeof(T_ACTION_CONFIG));
    memcpy(&cmd_buf[8], logic_io, sizeof(T_DSP_IPC_LOGIC_IO) - 4);
    memcpy(&cmd_buf[12], logic_io->tlv, sizeof(T_DSP_IPC_LOGIC_IO_TLV)*logic_io->tlv_cnt);

    if (dsp_shm_cmd_send(cmd_buf, cmd_buf_len, flush) == false)
    {
        free(cmd_buf);
        return false;
    }
    free(cmd_buf);
    return true;
}

bool dsp_ipc_vad_action(T_ACTION_CONFIG     action,
                        T_DSP_IPC_LOGIC_IO *logic_io)
{
    return dsp_ipc_action_send(action, logic_io, H2D_DSP_VOICE_TRIGGER_ACTION, true);
}

bool dsp_ipc_init_dsp_sdk(uint8_t scenario,
                          bool    flush)
{
    uint32_t cmd_length = 1;
    uint32_t cmd_buf_len = 8;
    uint8_t cmd_buf[8];

    cmd_buf[0] = (uint8_t)(H2D_DSP_SDK_INIT);
    cmd_buf[1] = (uint8_t)(H2D_DSP_SDK_INIT >> 8);
    cmd_buf[2] = (uint8_t)(cmd_length);
    cmd_buf[3] = (uint8_t)(cmd_length >> 8);
    cmd_buf[4] = scenario;
    cmd_buf[5] = 0;
    cmd_buf[6] = 0;
    cmd_buf[7] = 0;

    return dsp_shm_cmd_send(cmd_buf, cmd_buf_len, flush);
}

static bool dipc2_cmd_send(uint8_t  *cmd_buf,
                           uint32_t  cmd_buf_len,
                           bool      flush)
{
    bool ret;

    ret = dsp_shm_cmd_send(cmd_buf, cmd_buf_len, flush);

    DIPC_PRINT_INFO2("dipc2_cmd_send: %b, cmd_buf_len %d", TRACE_BINARY(cmd_buf_len, cmd_buf),
                     cmd_buf_len);

    return ret;
}

static void dipc_event_post(uint32_t  event,
                            void     *param)
{
    T_DSP_IPC_CBACK_ELEM *elem;

    if (event != DSP_IPC_EVT_NONE)
    {
        elem = os_queue_peek(&dsp_ipc_db.cback_list, 0);
        while (elem != NULL)
        {
            elem->cback((T_DSP_IPC_EVENT)event, (uint32_t)param);
            elem = elem->next;
        }
    }
}

static uint8_t dipc_event_status_report_complete_handler(uint8_t  *payload,
                                                         uint16_t  payload_length)
{
    uint8_t  *p;
    uint8_t   status;
    uint16_t  opcode;

    p = payload;

    DIPC_LE_STREAM_TO_UINT16(opcode, p);
    DIPC_LE_STREAM_TO_UINT8(status, p);

    if (status != DIPC_ERROR_SUCCESS)
    {
        DIPC_PRINT_INFO2("dipc_event_status_report_complete_handler: opcode 0x%04x, status %d",
                         opcode, status);

        return status;
    }

    switch (opcode)
    {
    case DIPC_H2D_GATE_CLEAR:
        {
            T_DIPC_GATEWAY_STATUS gateway_status;

            DIPC_LE_STREAM_TO_UINT8(gateway_status.type, p);
            DIPC_LE_STREAM_TO_UINT8(gateway_status.id, p);
            DIPC_LE_STREAM_TO_UINT8(gateway_status.dir, p);
            dipc_event_post(DSP_IPC_EVT_GATEWAY_CLEAR, &gateway_status);
        }
        break;

    case DIPC_H2D_POWER_CONTROL:
        {
            T_DIPC_POWER_CONTROL param;

            DIPC_LE_STREAM_TO_UINT8(param.entity_id, p);
            DIPC_LE_STREAM_TO_UINT8(param.mode, p);

            dipc_event_post(DSP_IPC_EVT_POWER_CONTROL_ACK, (void *)&param);
        }
        break;

    case DIPC_H2D_CODEC_PIPE_CREATE:
        {
            T_DIPC_DATA_QUEUE_INFO info;

            info.status = status;
            DIPC_LE_STREAM_TO_UINT32(info.session_id, p);
            DIPC_LE_STREAM_TO_UINT32(info.src_transport_address, p);
            DIPC_LE_STREAM_TO_UINT32(info.src_transport_size, p);
            DIPC_LE_STREAM_TO_UINT32(info.snk_transport_address, p);
            DIPC_LE_STREAM_TO_UINT32(info.snk_transport_size, p);

            dipc_event_post(DSP_IPC_EVT_CODEC_PIPE_CREATED, &info);
        }
        break;

    case DIPC_H2D_CODEC_PIPE_START:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_event_post(DSP_IPC_EVT_CODEC_PIPE_STARTED, (void *)session_id);
        }
        break;

    case DIPC_H2D_CODEC_PIPE_STOP:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_codec_pipe_destroy(session_id);
        }
        break;

    case DIPC_H2D_CODEC_PIPE_DESTROY:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_event_post(DSP_IPC_EVT_CODEC_PIPE_STOPPED, (void *)session_id);
        }
        break;

    case DIPC_H2D_CODEC_PIPE_PRE_MIXER_ADD:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_event_post(DSP_IPC_EVT_CODEC_PIPE_PRE_MIXER_ADD, (void *)session_id);
        }
        break;

    case DIPC_H2D_CODEC_PIPE_POST_MIXER_ADD:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_event_post(DSP_IPC_EVT_CODEC_PIPE_POST_MIXER_ADD, (void *)session_id);
        }
        break;

    case DIPC_H2D_CODEC_PIPE_MIXER_REMOVE:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_event_post(DSP_IPC_EVT_CODEC_PIPE_MIXER_REMOVE, (void *)session_id);
        }
        break;

    case DIPC_H2D_DECODER_CREATE:
        {
            T_DIPC_DATA_QUEUE_INFO info;

            info.status = status;
            DIPC_LE_STREAM_TO_UINT32(info.session_id, p);
            DIPC_LE_STREAM_TO_UINT32(info.src_transport_address, p);
            DIPC_LE_STREAM_TO_UINT32(info.src_transport_size, p);
            info.snk_transport_address = 0;
            info.snk_transport_size = 0;

            dipc_event_post(DSP_IPC_EVT_DECODER_CREATED, (void *)&info);
        }
        break;

    case DIPC_H2D_DECODER_START:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_event_post(DSP_IPC_EVT_DECODER_STARTED, (void *)session_id);
        }
        break;

    case DIPC_H2D_DECODER_STOP:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_decoder_destroy(session_id);
        }
        break;

    case DIPC_H2D_DECODER_DESTROY:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_event_post(DSP_IPC_EVT_DECODER_STOPPED, (void *)session_id);
        }
        break;

    case DIPC_H2D_ENCODER_CREATE:
        {
            T_DIPC_DATA_QUEUE_INFO info;

            info.status = status;
            DIPC_LE_STREAM_TO_UINT32(info.session_id, p);
            info.session_id = info.session_id & SESSION_ID_MASK;
            DIPC_LE_STREAM_TO_UINT32(info.snk_transport_address, p);
            DIPC_LE_STREAM_TO_UINT32(info.snk_transport_size, p);
            info.src_transport_address = 0;
            info.src_transport_size = 0;

            dipc_event_post(DSP_IPC_EVT_ENCODER_CREATED, (void *)&info);
        }
        break;

    case DIPC_H2D_ENCODER_START:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            session_id = session_id & SESSION_ID_MASK;
            dipc_event_post(DSP_IPC_EVT_ENCODER_STARTED, (void *)session_id);
        }
        break;

    case DIPC_H2D_ENCODER_STOP:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_encoder_destroy(session_id);
        }
        break;

    case DIPC_H2D_ENCODER_DESTROY:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            session_id = session_id & SESSION_ID_MASK;
            dipc_event_post(DSP_IPC_EVT_ENCODER_STOPPED, (void *)session_id);
        }
        break;

    case DIPC_H2D_CONFIGURATION_LOAD:
        {
            T_DIPC_CONFIGURATION_LOAD param;

            param.status = status;
            DIPC_LE_STREAM_TO_UINT8(param.entity_id, p);
            DIPC_LE_STREAM_TO_UINT32(param.session_id, p);
            param.session_id = param.session_id & SESSION_ID_MASK;

            dipc_event_post(DSP_IPC_EVT_CONFIGURATION_LOADED, &param);
        }
        break;

    case DIPC_H2D_RINGTONE_CREATE:
        {
            T_DIPC_DATA_QUEUE_INFO info;

            info.status = status;
            DIPC_LE_STREAM_TO_UINT32(info.session_id, p);
            info.src_transport_address = 0;
            info.src_transport_size = 0;
            info.snk_transport_address = 0;
            info.snk_transport_size = 0;

            dipc_event_post(DSP_IPC_EVT_TONE_CREATED, &info);
        }
        break;

    case DIPC_H2D_RINGTONE_START:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_event_post(DSP_IPC_EVT_TONE_STARTED, (void *)session_id);
        }
        break;

    case DIPC_H2D_RINGTONE_STOP:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_ringtone_destroy(session_id);
        }
        break;

    case DIPC_H2D_RINGTONE_DESTROY:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_event_post(DSP_IPC_EVT_TONE_STOPPED, (void *)session_id);
        }
        break;

    case DIPC_H2D_VOICE_PROMPT_CREATE:
        {
            T_DIPC_DATA_QUEUE_INFO info;

            info.status = status;
            DIPC_LE_STREAM_TO_UINT32(info.session_id, p);
            DIPC_LE_STREAM_TO_UINT32(info.src_transport_address, p);
            DIPC_LE_STREAM_TO_UINT32(info.src_transport_size, p);
            info.snk_transport_address = 0;
            info.snk_transport_size = 0;

            dipc_event_post(DSP_IPC_EVT_VP_CREATED, (void *)&info);
        }
        break;

    case DIPC_H2D_VOICE_PROMPT_START:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_event_post(DSP_IPC_EVT_VP_STARTED, (void *)session_id);
        }
        break;

    case DIPC_H2D_VOICE_PROMPT_STOP:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_voice_prompt_destroy(session_id);
        }
        break;

    case DIPC_H2D_VOICE_PROMPT_DESTROY:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_event_post(DSP_IPC_EVT_VP_STOPPED, (void *)session_id);
        }
        break;

    case DIPC_H2D_LINE_CREATE:
        {
            T_DIPC_DATA_QUEUE_INFO info;

            info.status = status;
            DIPC_LE_STREAM_TO_UINT32(info.session_id, p);
            info.src_transport_address = 0;
            info.src_transport_size = 0;
            info.snk_transport_address = 0;
            info.snk_transport_size = 0;

            dipc_event_post(DSP_IPC_EVT_LINE_CREATED, &info);
        }
        break;

    case DIPC_H2D_LINE_START:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_event_post(DSP_IPC_EVT_LINE_STARTED, (void *)session_id);
        }
        break;

    case DIPC_H2D_LINE_STOP:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_line_destroy(session_id);
        }
        break;

    case DIPC_H2D_LINE_DESTROY:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_event_post(DSP_IPC_EVT_LINE_STOPPED, (void *)session_id);
        }
        break;

    case DIPC_H2D_ANC_CREATE:
        {
            T_DIPC_DATA_QUEUE_INFO info;

            info.status = status;
            DIPC_LE_STREAM_TO_UINT32(info.session_id, p);
            info.src_transport_address = 0;
            info.src_transport_size = 0;
            info.snk_transport_address = 0;
            info.snk_transport_size = 0;

            dipc_event_post(DSP_IPC_EVT_APT_CREATED, &info);
        }
        break;

    case DIPC_H2D_ANC_START:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_event_post(DSP_IPC_EVT_APT_STARTED, (void *)(session_id));
        }
        break;

    case DIPC_H2D_ANC_STOP:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_apt_destroy(session_id);
        }
        break;

    case DIPC_H2D_ANC_DESTROY:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_event_post(DSP_IPC_EVT_APT_STOPPED, (void *)(session_id));
        }
        break;

    case DIPC_H2D_VAD_CREATE:
        {
            T_DIPC_DATA_QUEUE_INFO info;

            info.status = status;
            DIPC_LE_STREAM_TO_UINT32(info.session_id, p);
            info.session_id = info.session_id & SESSION_ID_MASK;
            DIPC_LE_STREAM_TO_UINT32(info.snk_transport_address, p);
            DIPC_LE_STREAM_TO_UINT32(info.snk_transport_size, p);
            info.src_transport_address = 0;
            info.src_transport_size = 0;

            dipc_event_post(DSP_IPC_EVT_VAD_CREATED, &info);
        }
        break;

    case DIPC_H2D_VAD_START:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            session_id = session_id & SESSION_ID_MASK;
            dipc_event_post(DSP_IPC_EVT_VAD_STARTED, (void *)session_id);
        }
        break;

    case DIPC_H2D_VAD_STOP:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            session_id = session_id & SESSION_ID_MASK;
            dipc_vad_destroy(session_id);
        }
        break;

    case DIPC_H2D_VAD_DESTROY:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            session_id = session_id & SESSION_ID_MASK;
            dipc_event_post(DSP_IPC_EVT_VAD_STOPPED, (void *)session_id);
        }
        break;

    case DIPC_H2D_KWS_CREATE:
        {
            T_DIPC_DATA_QUEUE_INFO info;

            info.status = status;
            DIPC_LE_STREAM_TO_UINT32(info.session_id, p);
            DIPC_LE_STREAM_TO_UINT32(info.src_transport_address, p);
            DIPC_LE_STREAM_TO_UINT32(info.src_transport_size, p);
            info.snk_transport_address = 0;
            info.snk_transport_size = 0;

            dipc_event_post(DSP_IPC_EVT_KWS_CREATED, &info);
        }
        break;

    case DIPC_H2D_KWS_START:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_event_post(DSP_IPC_EVT_KWS_STARTED, (void *)session_id);
        }
        break;

    case DIPC_H2D_KWS_STOP:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_kws_destroy(session_id);
        }
        break;

    case DIPC_H2D_KWS_DESTROY:
        {
            uint32_t session_id;

            DIPC_LE_STREAM_TO_UINT32(session_id, p);
            dipc_event_post(DSP_IPC_EVT_KWS_STOPPED, (void *)session_id);
        }
        break;

    default:
        break;
    }

    return DIPC_ERROR_SUCCESS;
}

static uint8_t dipc_event_error_handler(uint8_t  *payload,
                                        uint16_t  payload_length)
{
    uint8_t             *p;
    T_DIPC_ERROR_REPORT  info;

    p = payload;
    DIPC_LE_STREAM_TO_UINT32(info.session_id, p);
    DIPC_LE_STREAM_TO_UINT8(info.error_code, p);

    info.session_id = info.session_id & SESSION_ID_MASK;

    dipc_event_post(DSP_IPC_EVT_ERROR_REPORT, &info);

    return info.error_code;
}

static uint8_t dipc_event_mcps_request_handler(uint8_t  *payload,
                                               uint16_t  payload_length)
{
    uint8_t *p;
    uint8_t  status;
    uint8_t  entity_id;
    uint16_t mcps;
    uint32_t serial_num;
    uint32_t actual_mcps = 0;


    p = payload;
    DIPC_LE_STREAM_TO_UINT32(serial_num, p);
    DIPC_LE_STREAM_TO_UINT8(entity_id, p);
    DIPC_LE_STREAM_TO_UINT16(mcps, p);

    switch (entity_id)
    {
    case DIPC_ENTITY_1:
        status = DIPC_ERROR_SUCCESS;
        (void)pm_dsp1_freq_set((uint32_t)mcps, &actual_mcps);
        dipc_mcps_reply(serial_num, entity_id, (uint16_t)actual_mcps);
        break;

    case DIPC_ENTITY_2:
        status = DIPC_ERROR_SUCCESS;
        (void)pm_dsp2_freq_set((uint32_t)mcps, &actual_mcps);
        dipc_mcps_reply(serial_num, entity_id, (uint16_t)actual_mcps);
        break;

    default:
        status = DIPC_ERROR_INVALID_COMMAND_PARAM;
        break;
    }

    return status;
}

static uint8_t dipc_event_data_ack_handler(uint8_t  *payload,
                                           uint16_t  payload_length)
{
    uint8_t *p;
    uint8_t  status;

    p = payload;
    DIPC_LE_STREAM_TO_UINT8(status, p);

    if (status == DIPC_ERROR_SUCCESS)
    {
        T_DIPC_DATA_ACK ack_info;

        DIPC_LE_STREAM_TO_UINT32(ack_info.session_id, p);
        DIPC_LE_STREAM_TO_UINT16(ack_info.seq_num, p);

        dipc_event_post(DSP_IPC_EVT_DATA_ACK, &ack_info);
    }

    return status;
}

static uint8_t dipc_event_data_complete_handler(uint8_t  *payload,
                                                uint16_t  payload_length)
{
    uint8_t  *p;
    uint8_t   status;
    uint32_t  session_id;

    p = payload;
    DIPC_LE_STREAM_TO_UINT8(status, p);
    DIPC_LE_STREAM_TO_UINT32(session_id, p);

    if (status == DIPC_ERROR_SUCCESS)
    {
        dipc_event_post(DSP_IPC_EVT_DATA_COMPLETE, (void *)session_id);
    }

    return status;
}

static uint8_t dipc_event_data_indicate_handler(uint8_t  *payload,
                                                uint16_t  payload_length)
{
    uint8_t              *p;
    uint8_t               status;
    T_DIPC_DATA_INDICATE  indicate_info;

    p = payload;
    DIPC_LE_STREAM_TO_UINT8(status, p);

    if (status == DIPC_ERROR_SUCCESS)
    {
        DIPC_LE_STREAM_TO_UINT32(indicate_info.session_id, p);
        indicate_info.session_id = indicate_info.session_id & SESSION_ID_MASK;
        DIPC_LE_STREAM_TO_UINT16(indicate_info.seq_num, p);
        DIPC_LE_STREAM_TO_UINT16(indicate_info.data_len, p);

        dipc_event_post(DSP_IPC_EVT_DATA_IND, &indicate_info);
    }

    return status;
}

static uint8_t dipc_event_data_empty_handler(uint8_t  *payload,
                                             uint16_t  payload_length)
{
    uint8_t *p;
    uint32_t session_id;

    p = payload;

    DIPC_LE_STREAM_TO_UINT32(session_id, p);

    dipc_event_post(DSP_IPC_EVT_DATA_EMPTY, (void *)(session_id & SESSION_ID_MASK));

    return DIPC_ERROR_SUCCESS;
}

static uint8_t dipc_event_plc_notify_handler(uint8_t  *payload,
                                             uint16_t  payload_length)
{
    uint8_t           *p;
    T_DIPC_PLC_NOTIFY  plc_notify;

    p = payload;
    DIPC_LE_STREAM_TO_UINT32(plc_notify.session_id, p);
    plc_notify.session_id = plc_notify.session_id & SESSION_ID_MASK;
    DIPC_LE_STREAM_TO_UINT32(plc_notify.total_sample_num, p);
    DIPC_LE_STREAM_TO_UINT32(plc_notify.plc_sample_num, p);

    dipc_event_post(DSP_IPC_EVT_PLC_NOTIFY, &plc_notify);

    return DIPC_ERROR_SUCCESS;
}

static uint8_t dipc_event_signal_refresh_handler(uint8_t  *payload,
                                                 uint16_t  payload_length)
{
    uint8_t *p;
    uint8_t  i;
    T_DIPC_SIGNAL_REFRESH signal_refresh;

    p = payload;

    DIPC_LE_STREAM_TO_UINT32(signal_refresh.session_id, p);
    signal_refresh.session_id = signal_refresh.session_id & SESSION_ID_MASK;
    DIPC_LE_STREAM_TO_UINT8(signal_refresh.direction, p);
    DIPC_LE_STREAM_TO_UINT8(signal_refresh.freq_num, p);

    signal_refresh.left_gain_table = malloc(signal_refresh.freq_num * 2);
    if (signal_refresh.left_gain_table == NULL)
    {
        return DIPC_ERROR_MEM_CAPACITY_EXCEEDED;
    }

    signal_refresh.right_gain_table = malloc(signal_refresh.freq_num * 2);
    if (signal_refresh.right_gain_table == NULL)
    {
        free(signal_refresh.left_gain_table);
        return DIPC_ERROR_MEM_CAPACITY_EXCEEDED;
    }

    for (i = 0; i < signal_refresh.freq_num; ++i)
    {
        DIPC_LE_STREAM_TO_UINT16(signal_refresh.left_gain_table[i], p);
        DIPC_LE_STREAM_TO_UINT16(signal_refresh.right_gain_table[i], p);
    }

    dipc_event_post(DSP_IPC_EVT_SIGNAL_REFRESH, &signal_refresh);

    free(signal_refresh.left_gain_table);
    free(signal_refresh.right_gain_table);

    return DIPC_ERROR_SUCCESS;
}

static uint8_t dipc_event_fifo_notify_handler(uint8_t  *payload,
                                              uint16_t  payload_length)
{
    uint8_t            *p;
    T_DIPC_FIFO_NOTIFY  info;

    p = payload;
    DIPC_LE_STREAM_TO_UINT32(info.session_id, p);
    info.session_id = info.session_id & SESSION_ID_MASK;
    DIPC_LE_STREAM_TO_UINT16(info.packet_number, p);
    DIPC_LE_STREAM_TO_UINT16(info.decoding_latency, p);
    DIPC_LE_STREAM_TO_UINT16(info.queuing_latency, p);

    dipc_event_post(DSP_IPC_EVT_FIFO_NOTIFY, &info);

    return DIPC_ERROR_SUCCESS;
}

static uint8_t dipc_event_kws_hit_handler(uint8_t  *payload,
                                          uint16_t  payload_length)
{
    uint8_t        *p;
    T_DIPC_KWS_HIT  keyword;

    p = payload;
    DIPC_LE_STREAM_TO_UINT32(keyword.session_id, p);
    keyword.session_id = keyword.session_id & SESSION_ID_MASK;
    DIPC_LE_STREAM_TO_UINT8(keyword.keyword_index, p);

    dipc_event_post(DSP_IPC_EVT_KWS_HIT, &keyword);

    return DIPC_ERROR_SUCCESS;
}

static uint8_t dipc_event_vendor_specific_effect_rsp_handler(uint8_t  *payload,
                                                             uint16_t  payload_length)
{
    uint8_t             *p;
    T_DIPC_VSE_RESPONSE  rsp_info;

    p = payload;
    DIPC_LE_STREAM_TO_UINT32(rsp_info.session_id, p);
    DIPC_LE_STREAM_TO_UINT16(rsp_info.company_id, p);
    DIPC_LE_STREAM_TO_UINT16(rsp_info.effect_id, p);
    DIPC_LE_STREAM_TO_UINT16(rsp_info.seq_num, p);
    DIPC_LE_STREAM_TO_UINT16(rsp_info.payload_len, p);

    rsp_info.session_id = rsp_info.session_id & SESSION_ID_MASK;
    rsp_info.payload = NULL;
    if (rsp_info.payload_len)
    {
        rsp_info.payload = malloc(rsp_info.payload_len);
        if (rsp_info.payload != NULL)
        {
            DIPC_STREAM_TO_ARRAY(rsp_info.payload, p, rsp_info.payload_len);
        }
        else
        {
            return DIPC_ERROR_MEM_CAPACITY_EXCEEDED;
        }
    }

    dipc_event_post(DSP_IPC_EVT_VSE_RESPONSE, &rsp_info);
    if (rsp_info.payload != NULL)
    {
        free(rsp_info.payload);
    }

    return DIPC_ERROR_SUCCESS;
}

static uint8_t dipc_event_vendor_specific_effect_ind_handler(uint8_t  *payload,
                                                             uint16_t  payload_length)
{
    uint8_t             *p;
    T_DIPC_VSE_INDICATE  ind_info;

    p = payload;
    DIPC_LE_STREAM_TO_UINT32(ind_info.session_id, p);
    DIPC_LE_STREAM_TO_UINT16(ind_info.company_id, p);
    DIPC_LE_STREAM_TO_UINT16(ind_info.effect_id, p);
    DIPC_LE_STREAM_TO_UINT16(ind_info.seq_num, p);
    DIPC_LE_STREAM_TO_UINT16(ind_info.payload_len, p);

    ind_info.session_id = ind_info.session_id & SESSION_ID_MASK;
    ind_info.payload = NULL;
    if (ind_info.payload_len)
    {
        ind_info.payload = malloc(ind_info.payload_len);
        if (ind_info.payload != NULL)
        {
            DIPC_STREAM_TO_ARRAY(ind_info.payload, p, ind_info.payload_len);
        }
        else
        {
            return DIPC_ERROR_MEM_CAPACITY_EXCEEDED;
        }
    }

    dipc_event_post(DSP_IPC_EVT_VSE_INDICATE, &ind_info);
    if (ind_info.payload != NULL)
    {
        free(ind_info.payload);
    }

    return DIPC_ERROR_SUCCESS;
}

static uint8_t dipc_event_vendor_specific_effect_notify_handler(uint8_t  *payload,
                                                                uint16_t  payload_length)
{
    uint8_t           *p;
    T_DIPC_VSE_NOTIFY  notify_info;

    p = payload;
    DIPC_LE_STREAM_TO_UINT32(notify_info.session_id, p);
    DIPC_LE_STREAM_TO_UINT16(notify_info.company_id, p);
    DIPC_LE_STREAM_TO_UINT16(notify_info.effect_id, p);
    DIPC_LE_STREAM_TO_UINT16(notify_info.seq_num, p);
    DIPC_LE_STREAM_TO_UINT16(notify_info.payload_len, p);

    notify_info.session_id = notify_info.session_id & SESSION_ID_MASK;
    notify_info.payload = NULL;
    if (notify_info.payload_len)
    {
        notify_info.payload = malloc(notify_info.payload_len);
        if (notify_info.payload != NULL)
        {
            DIPC_STREAM_TO_ARRAY(notify_info.payload, p, notify_info.payload_len);
        }
        else
        {
            return DIPC_ERROR_MEM_CAPACITY_EXCEEDED;
        }
    }

    dipc_event_post(DSP_IPC_EVT_VSE_NOTIFY, &notify_info);
    if (notify_info.payload != NULL)
    {
        free(notify_info.payload);
    }

    return DIPC_ERROR_SUCCESS;
}

static void dipc_event_parser(uint16_t  event,
                              uint8_t  *payload,
                              uint16_t  payload_length)
{
    uint8_t ret;

    ret = 0;

    switch (event)
    {
    case DIPC_D2H_HOST_VERSION_REQUEST:
        {

        }
        break;

    case DIPC_D2H_MCPS_REQUEST:
        {
            ret = dipc_event_mcps_request_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_DATA_ACK:
        {
            ret = dipc_event_data_ack_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_DATA_COMPLETE:
        {
            ret = dipc_event_data_complete_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_DATA_INDICATE:
        {
            ret = dipc_event_data_indicate_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_DATA_EMPTY_NOTIFY:
        {
            ret = dipc_event_data_empty_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_PLC_NOTIFY:
        {
            ret = dipc_event_plc_notify_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_SIGNAL_REFRESH:
        {
            ret = dipc_event_signal_refresh_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_FIFO_NOTIFY:
        {
            ret = dipc_event_fifo_notify_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_KEYWORD_HIT:
        {
            ret = dipc_event_kws_hit_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_TIMESTAMP_LOSE:
        {
            //ret = dipc_event_timestamp_lose_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_SYNCHRONIZATION_LATCH:
        {
            //ret = dipc_event_sync_latch_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_SYNCHRONIZATION_UNLATCH:
        {
            //ret = dipc_event_sync_unlatch_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_JOIN_INFO_REPORT:
        {
            //ret = dipc_event_info_report_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_INFO_RELAY_REQUEST:
        {
            //ret = dipc_event_relay_request_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_COMMAND_ACK:
        {
            //ret = dipc_event_status_report_ack_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_COMMAND_COMPLETE:
        {
            ret = dipc_event_status_report_complete_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_ERROR_REPORT:
        {
            ret = dipc_event_error_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_VENDOR_SPECIFIC_EFFECT_RESPONSE:
        {
            ret = dipc_event_vendor_specific_effect_rsp_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_VENDOR_SPECIFIC_EFFECT_INDICATE:
        {
            ret = dipc_event_vendor_specific_effect_ind_handler(payload, payload_length);
        }
        break;

    case DIPC_D2H_VENDOR_SPECIFIC_EFFECT_NOTIFY:
        {
            ret = dipc_event_vendor_specific_effect_notify_handler(payload, payload_length);
        }
        break;

    default :
        {}
        break;
    }

    if (ret != DIPC_ERROR_SUCCESS)
    {
        DIPC_PRINT_TRACE2("dipc_event_parser: event 0x%04x, ret %d", event, ret);
    }
}

bool dipc_gateway_set(uint8_t  type,
                      uint8_t  id,
                      uint8_t  direction,
                      uint8_t  role,
                      uint8_t  bridge,
                      uint32_t sample_rate,
                      uint8_t  fifo_mode,
                      uint8_t  channel_mode,
                      uint8_t  channel_format,
                      uint8_t  channel_length,
                      uint8_t  data_length)
{
    uint8_t  cmd_buf[18];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_GATE_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 14);
    DIPC_LE_UINT8_TO_STREAM(p,  type);
    DIPC_LE_UINT8_TO_STREAM(p,  id);
    DIPC_LE_UINT8_TO_STREAM(p,  direction);
    DIPC_LE_UINT8_TO_STREAM(p,  role);
    DIPC_LE_UINT8_TO_STREAM(p,  bridge);
    DIPC_LE_UINT32_TO_STREAM(p, sample_rate);
    DIPC_LE_UINT8_TO_STREAM(p,  fifo_mode);
    DIPC_LE_UINT8_TO_STREAM(p,  channel_mode);
    DIPC_LE_UINT8_TO_STREAM(p,  channel_format);
    DIPC_LE_UINT8_TO_STREAM(p,  channel_length);
    DIPC_LE_UINT8_TO_STREAM(p,  data_length);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_gateway_clear(uint8_t type,
                        uint8_t id,
                        uint8_t direction)
{
    uint8_t cmd_buf[7];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_GATE_CLEAR);
    DIPC_LE_UINT16_TO_STREAM(p, 3);
    DIPC_LE_UINT8_TO_STREAM(p,  type);
    DIPC_LE_UINT8_TO_STREAM(p,  id);
    DIPC_LE_UINT8_TO_STREAM(p,  direction);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_mcps_reply(uint32_t serial_num,
                     uint8_t  entity_id,
                     uint16_t mcps)
{
    uint8_t cmd_buf[11];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_MCPS_REPLY);
    DIPC_LE_UINT16_TO_STREAM(p, 7);
    DIPC_LE_UINT32_TO_STREAM(p, serial_num);
    DIPC_LE_UINT8_TO_STREAM(p, entity_id);
    DIPC_LE_UINT16_TO_STREAM(p, mcps);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_power_control(uint8_t entity_id, uint8_t mode)
{
    uint8_t  cmd_buf[6];
    uint8_t *p;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_POWER_CONTROL);
    DIPC_LE_UINT16_TO_STREAM(p, 2);
    DIPC_LE_UINT8_TO_STREAM(p, entity_id);
    DIPC_LE_UINT8_TO_STREAM(p, mode);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_channel_out_reorder(uint32_t  session_id,
                              uint8_t   channel_num,
                              uint32_t  channel_array[])
{
    uint8_t  *cmd_buf;
    uint16_t  cmd_size;

    cmd_size = DIPC_H2D_HEAD_LEN + 5 + channel_num * 4;
    cmd_buf  = malloc(cmd_size);
    if (cmd_buf != NULL)
    {
        uint8_t *p;
        bool     ret;

        p = cmd_buf;
        DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_CHANNEL_REORDER);
        DIPC_LE_UINT16_TO_STREAM(p, cmd_size - DIPC_H2D_HEAD_LEN);
        DIPC_LE_UINT32_TO_STREAM(p, session_id);
        DIPC_LE_UINT8_TO_STREAM(p,  channel_num);
        DIPC_ARRAY_TO_STREAM(p, channel_array, channel_num * 4);

        ret = dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
        free(cmd_buf);
        return ret;
    }

    return false;
}

bool dipc_channel_in_reorder(uint32_t  session_id,
                             uint8_t   channel_num,
                             uint32_t  channel_array[])
{
    uint8_t  *cmd_buf;
    uint16_t  cmd_size;

    cmd_size = DIPC_H2D_HEAD_LEN + 5 + channel_num * 4;
    cmd_buf  = malloc(cmd_size);
    if (cmd_buf != NULL)
    {
        uint8_t *p;
        bool     ret;

        session_id |= DIPC_SESSION_TYPE_ENCODER;

        p = cmd_buf;
        DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_CHANNEL_REORDER);
        DIPC_LE_UINT16_TO_STREAM(p, cmd_size - DIPC_H2D_HEAD_LEN);
        DIPC_LE_UINT32_TO_STREAM(p, session_id);
        DIPC_LE_UINT8_TO_STREAM(p,  channel_num);
        DIPC_ARRAY_TO_STREAM(p, channel_array, channel_num * 4);

        ret = dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
        free(cmd_buf);
        return ret;
    }

    return false;
}

uint8_t *dipc_configuration_alloc(uint16_t len)
{
    uint8_t *p = NULL;

    if (len > 0)
    {
        p = malloc(DIPC_H2D_HEAD_LEN + DIPC_H2D_LOAD_SUB_HEADER + len);
        if (p != NULL)
        {
            p += DIPC_H2D_HEAD_LEN + DIPC_H2D_LOAD_SUB_HEADER;
        }
    }

    return p;
}

void dipc_configuration_free(uint8_t *buf)
{
    if (buf != NULL)
    {
        free(buf - DIPC_H2D_HEAD_LEN - DIPC_H2D_LOAD_SUB_HEADER);
    }
}

bool dipc_configuration_load(uint8_t   entity_id,
                             uint32_t  session_id,
                             uint8_t  *buf,
                             uint16_t  len)
{
    uint8_t *cmd_buf;
    uint8_t *p;

    cmd_buf = buf - DIPC_H2D_HEAD_LEN - DIPC_H2D_LOAD_SUB_HEADER;
    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_CONFIGURATION_LOAD);
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_LOAD_SUB_HEADER + len);
    DIPC_LE_UINT8_TO_STREAM(p, entity_id);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, len);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf + len, true);
}

bool dipc_codec_pipe_create(uint32_t  session_id,
                            uint8_t   data_mode,
                            uint8_t   src_coder_id,
                            uint8_t   src_frame_num,
                            uint16_t  src_coder_format_size,
                            uint8_t  *src_coder_format,
                            uint8_t   snk_coder_id,
                            uint8_t   snk_frame_num,
                            uint16_t  snk_coder_format_size,
                            uint8_t  *snk_coder_format)
{
    uint8_t *cmd_buf;
    uint16_t cmd_size;
    uint8_t *p;
    bool ret;

    cmd_size = DIPC_H2D_HEAD_LEN + 9 + src_coder_format_size + 4 + snk_coder_format_size;

    cmd_buf = calloc(1, cmd_size);
    if (cmd_buf == NULL)
    {
        return false;
    }

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_CODEC_PIPE_CREATE);
    DIPC_LE_UINT16_TO_STREAM(p, cmd_size - DIPC_H2D_HEAD_LEN);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p,  data_mode);
    DIPC_LE_UINT8_TO_STREAM(p,  src_coder_id);
    DIPC_LE_UINT8_TO_STREAM(p,  src_frame_num);
    DIPC_LE_UINT16_TO_STREAM(p, src_coder_format_size);

    memcpy(p, src_coder_format, src_coder_format_size);
    p += src_coder_format_size;

    DIPC_LE_UINT8_TO_STREAM(p,  snk_coder_id);
    DIPC_LE_UINT8_TO_STREAM(p,  snk_frame_num);
    DIPC_LE_UINT16_TO_STREAM(p, snk_coder_format_size);

    memcpy(p, snk_coder_format, snk_coder_format_size);
    p += snk_coder_format_size;

    ret = dipc2_cmd_send(cmd_buf, p - cmd_buf, true);

    free(cmd_buf);

    return ret;
}

bool dipc_codec_pipe_destroy(uint32_t session_id)
{
    uint8_t cmd_buf[8];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_CODEC_PIPE_DESTROY);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_codec_pipe_start(uint32_t session_id)
{
    uint8_t cmd_buf[8];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_CODEC_PIPE_START);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_codec_pipe_stop(uint32_t session_id)
{
    uint8_t cmd_buf[8];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_CODEC_PIPE_STOP);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_codec_pipe_gain_set(uint32_t session_id,
                              uint16_t gain_step_left,
                              uint16_t gain_step_right)
{
    uint8_t cmd_buf[12];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_CODEC_PIPE_GAIN_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 8);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_left);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_right);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_codec_pipe_pre_mixer_add(uint32_t prime_session_id,
                                   uint32_t auxiliary_session_id)
{
    uint8_t cmd_buf[12];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_CODEC_PIPE_PRE_MIXER_ADD);
    DIPC_LE_UINT16_TO_STREAM(p, 8);
    DIPC_LE_UINT32_TO_STREAM(p, prime_session_id);
    DIPC_LE_UINT32_TO_STREAM(p, auxiliary_session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_codec_pipe_post_mixer_add(uint32_t prime_session_id,
                                    uint32_t auxiliary_session_id)
{
    uint8_t cmd_buf[12];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_CODEC_PIPE_POST_MIXER_ADD);
    DIPC_LE_UINT16_TO_STREAM(p, 8);
    DIPC_LE_UINT32_TO_STREAM(p, prime_session_id);
    DIPC_LE_UINT32_TO_STREAM(p, auxiliary_session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_codec_pipe_mixer_remove(uint32_t prime_session_id,
                                  uint32_t auxiliary_session_id)
{
    uint8_t cmd_buf[12];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_CODEC_PIPE_MIXER_REMOVE);
    DIPC_LE_UINT16_TO_STREAM(p, 8);
    DIPC_LE_UINT32_TO_STREAM(p, prime_session_id);
    DIPC_LE_UINT32_TO_STREAM(p, auxiliary_session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_codec_pipe_asrc_set(uint32_t session_id,
                              uint8_t  toggle,
                              int32_t  ratio)
{
    uint8_t  cmd_buf[13];
    uint8_t *p;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_CODEC_PIPE_ASRC_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 9);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p, toggle);
    DIPC_LE_UINT32_TO_STREAM(p, ratio);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_decoder_create(uint32_t  session_id,
                         uint8_t   category,
                         uint8_t   data_mode,
                         uint8_t   frame_num,
                         uint8_t   coder_id,
                         uint16_t  coder_format_size,
                         uint8_t  *coder_format,
                         uint16_t  io_table_size,
                         uint8_t  *io_table)
{
    uint8_t *cmd_buf;
    uint16_t cmd_size;
    uint8_t *p;
    bool ret;

    cmd_size = DIPC_H2D_HEAD_LEN + 8 + coder_format_size + 4 + io_table_size;

    cmd_buf = calloc(1, cmd_size);
    if (cmd_buf == NULL)
    {
        return false;
    }

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_DECODER_CREATE);
    DIPC_LE_UINT16_TO_STREAM(p, cmd_size - DIPC_H2D_HEAD_LEN);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p,  category);
    DIPC_LE_UINT8_TO_STREAM(p,  coder_id);
    DIPC_LE_UINT16_TO_STREAM(p, coder_format_size);
    DIPC_ARRAY_TO_STREAM(p, coder_format, coder_format_size);
    DIPC_LE_UINT8_TO_STREAM(p, data_mode);
    DIPC_LE_UINT8_TO_STREAM(p, frame_num);
    DIPC_LE_UINT16_TO_STREAM(p, io_table_size);
    DIPC_ARRAY_TO_STREAM(p, io_table, io_table_size);

    DIPC_PRINT_TRACE6("dipc_decoder_create: session_id 0x%08x, coder_id %d, "
                      "category %d, format_size %d, frame_num %d, tbl_size %d", session_id, coder_id,
                      category, coder_format_size, frame_num, io_table_size);
    /* DIPC_PRINT_TRACE1("dipc_decoder_create: coder_format %b", TRACE_BINARY(format_size, coder_format)); */

    ret = dipc2_cmd_send(cmd_buf, p - cmd_buf, true);

    free(cmd_buf);

    return ret;
}

bool dipc_decoder_destroy(uint32_t session_id)
{
    uint8_t cmd_buf[8];
    uint8_t *p;
    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_DECODER_DESTROY);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_decoder_start(uint32_t session_id,
                        uint8_t  fade)
{
    uint8_t cmd_buf[9];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_DECODER_START);
    DIPC_LE_UINT16_TO_STREAM(p, 5);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p,  fade);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_decoder_stop(uint32_t session_id,
                       uint8_t  fade)
{
    uint8_t cmd_buf[9];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_DECODER_STOP);
    DIPC_LE_UINT16_TO_STREAM(p, 5);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p,  fade);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_decoder_gain_set(uint32_t session_id,
                           uint16_t gain_step_left,
                           uint16_t gain_step_right)
{
    uint8_t cmd_buf[12];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_DECODER_GAIN_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 8);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_left);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_right);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_decoder_ramp_gain_set(uint32_t session_id,
                                uint16_t gain_step_left,
                                uint16_t gain_step_right,
                                uint16_t duration)
{
    uint8_t cmd_buf[14];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_DECODER_GAIN_RAMP);
    DIPC_LE_UINT16_TO_STREAM(p, 10);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_left);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_right);
    DIPC_LE_UINT16_TO_STREAM(p, duration);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_decoder_asrc_set(uint32_t session_id,
                           int32_t  ratio,
                           uint32_t timestamp)
{
    uint8_t cmd_buf[16];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_DECODER_ASRC_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 12);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT32_TO_STREAM(p, ratio);
    DIPC_LE_UINT32_TO_STREAM(p, timestamp);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_signal_monitoring_start(uint32_t session_id,
                                  uint8_t  direction,
                                  uint16_t refresh_interval,
                                  uint8_t  freq_num,
                                  uint32_t freq_table[])
{
    uint8_t *cmd_buf;
    uint16_t cmd_size;
    uint8_t *p;
    bool     ret;

    cmd_size = 12 + freq_num * 4;
    cmd_buf = calloc(1, cmd_size);
    if (cmd_buf == NULL)
    {
        return false;
    }

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_SIGNAL_MONITORING_START);
    DIPC_LE_UINT16_TO_STREAM(p, cmd_size - DIPC_H2D_HEAD_LEN);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p, direction);
    DIPC_LE_UINT16_TO_STREAM(p, refresh_interval);
    DIPC_LE_UINT8_TO_STREAM(p, freq_num);
    DIPC_ARRAY_TO_STREAM(p, freq_table, freq_num * 4);

    ret = dipc2_cmd_send(cmd_buf, p - cmd_buf, true);

    free(cmd_buf);

    return ret;
}

bool dipc_signal_monitoring_stop(uint32_t session_id,
                                 uint8_t  direction)
{
    uint8_t  cmd_buf[9];
    uint8_t *p;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_SIGNAL_MONITORING_STOP);
    DIPC_LE_UINT16_TO_STREAM(p, 5);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p, direction);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_decoder_plc_set(uint32_t session_id,
                          bool     enable,
                          uint16_t interval,
                          uint32_t threshold)
{
    uint8_t  cmd_buf[15];
    uint8_t *p;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_DECODER_PLC_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 11);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p, (uint8_t)enable);
    DIPC_LE_UINT16_TO_STREAM(p, interval);
    DIPC_LE_UINT32_TO_STREAM(p, threshold);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_decoder_fifo_report(uint32_t session_id,
                              uint16_t interval)
{
    uint8_t cmd_buf[10];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_DECODER_FIFO_REPORT);
    DIPC_LE_UINT16_TO_STREAM(p, 6);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, interval);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_decoder_effect_control(uint32_t session_id,
                                 uint8_t  action)
{
    uint8_t cmd_buf[9];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_EFFECT_CONTROL);
    DIPC_LE_UINT16_TO_STREAM(p, 5);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p, action);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_encoder_create(uint32_t  session_id,
                         uint8_t   category,
                         uint8_t   data_mode,
                         uint8_t   frame_num,
                         uint8_t   coder_id,
                         uint16_t  coder_format_size,
                         uint8_t  *coder_format,
                         uint16_t  io_table_size,
                         uint8_t  *io_table)
{
    uint8_t *cmd_buf;
    uint16_t cmd_size;
    uint8_t *p;
    bool ret;

    cmd_size = DIPC_H2D_HEAD_LEN + 8 + coder_format_size + 4 + io_table_size;

    cmd_buf = calloc(1, cmd_size);
    if (cmd_buf == NULL)
    {
        return false;
    }

    p = cmd_buf;
    session_id |= DIPC_SESSION_TYPE_ENCODER;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_ENCODER_CREATE);
    DIPC_LE_UINT16_TO_STREAM(p, cmd_size - DIPC_H2D_HEAD_LEN);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p,  category);
    DIPC_LE_UINT8_TO_STREAM(p,  coder_id);
    DIPC_LE_UINT16_TO_STREAM(p, coder_format_size);
    DIPC_ARRAY_TO_STREAM(p, coder_format, coder_format_size);
    DIPC_LE_UINT8_TO_STREAM(p, data_mode);
    DIPC_LE_UINT8_TO_STREAM(p, frame_num);
    DIPC_LE_UINT16_TO_STREAM(p, io_table_size);
    DIPC_ARRAY_TO_STREAM(p, io_table, io_table_size);

    DIPC_PRINT_TRACE6("dipc_encoder_create: session_id 0x%08x, coder_id %d, "
                      "category %d, format_size %d, frame_num %d, tbl_size %d", session_id, coder_id,
                      category, coder_format_size, frame_num, io_table_size);
    /* DIPC_PRINT_TRACE1("dipc_encoder_create: coder_format %b", TRACE_BINARY(format_size, coder_format)); */

    ret = dipc2_cmd_send(cmd_buf, p - cmd_buf, true);

    free(cmd_buf);

    return ret;
}

bool dipc_encoder_destroy(uint32_t session_id)
{
    uint8_t cmd_buf[8];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_ENCODER_DESTROY);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_encoder_start(uint32_t session_id,
                        uint8_t  fade)
{
    uint8_t cmd_buf[9];
    uint8_t *p;

    p = cmd_buf;
    session_id |= DIPC_SESSION_TYPE_ENCODER;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_ENCODER_START);
    DIPC_LE_UINT16_TO_STREAM(p, 5);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p,  fade);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_encoder_stop(uint32_t session_id,
                       uint8_t  fade)
{
    uint8_t cmd_buf[9];
    uint8_t *p;

    p = cmd_buf;
    session_id |= DIPC_SESSION_TYPE_ENCODER;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_ENCODER_STOP);
    DIPC_LE_UINT16_TO_STREAM(p, 5);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p,  fade);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_encoder_gain_set(uint32_t session_id,
                           uint16_t gain_step_left,
                           uint16_t gain_step_right)
{
    uint8_t cmd_buf[12];
    uint8_t *p;

    p = cmd_buf;
    session_id |= DIPC_SESSION_TYPE_ENCODER;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_ENCODER_GAIN_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 8);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_left);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_right);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_timestamp_set(uint32_t session_id,
                        uint32_t clk_source,
                        uint32_t timestamp)
{
    uint8_t cmd_buf[16];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_TIMESTAMP_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 12);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT32_TO_STREAM(p, clk_source);
    DIPC_LE_UINT32_TO_STREAM(p, timestamp);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_encoder_effect_control(uint32_t session_id,
                                 uint8_t  action)
{
    uint8_t cmd_buf[9];
    uint8_t *p;

    p = cmd_buf;
    session_id |= DIPC_SESSION_TYPE_ENCODER;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_EFFECT_CONTROL);
    DIPC_LE_UINT16_TO_STREAM(p, 5);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p, action);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_line_create(uint32_t  session_id,
                      uint16_t  io_table_size,
                      uint8_t  *io_table)
{
    uint8_t *cmd_buf;
    uint16_t cmd_size;
    uint8_t *p;
    bool ret;

    cmd_size = DIPC_H2D_HEAD_LEN + 8 + io_table_size;

    cmd_buf = calloc(1, cmd_size);
    if (cmd_buf == NULL)
    {
        return false;
    }

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_LINE_CREATE);
    DIPC_LE_UINT16_TO_STREAM(p, cmd_size - DIPC_H2D_HEAD_LEN);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, io_table_size);
    DIPC_LE_UINT16_TO_STREAM(p, 0x0000);
    DIPC_ARRAY_TO_STREAM(p, io_table, io_table_size);

    ret = dipc2_cmd_send(cmd_buf, p - cmd_buf, true);

    free(cmd_buf);

    return ret;
}

bool dipc_line_destroy(uint32_t session_id)
{
    uint8_t cmd_buf[8];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_LINE_DESTROY);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_line_start(uint32_t session_id,
                     uint8_t  fade)
{
    uint8_t cmd_buf[9];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_LINE_START);
    DIPC_LE_UINT16_TO_STREAM(p, 5);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p,  fade);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_line_stop(uint32_t session_id,
                    uint8_t  fade)
{
    uint8_t cmd_buf[9];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_LINE_STOP);
    DIPC_LE_UINT16_TO_STREAM(p, 5);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p,  fade);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_line_dac_gain_set(uint32_t session_id,
                            uint16_t gain_step_left,
                            uint16_t gain_step_right)
{
    uint8_t cmd_buf[12];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_LINE_DAC_GAIN_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 8);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_left);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_right);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_line_adc_gain_set(uint32_t session_id,
                            uint16_t gain_step_left,
                            uint16_t gain_step_right)
{
    uint8_t cmd_buf[12];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_LINE_ADC_GAIN_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 8);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_left);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_right);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_line_gain_ramp(uint32_t session_id,
                         uint16_t gain_step_left,
                         uint16_t gain_step_right,
                         uint16_t duration)
{
    uint8_t cmd_buf[14];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_LINE_GAIN_RAMP);
    DIPC_LE_UINT16_TO_STREAM(p, 10);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_left);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_right);
    DIPC_LE_UINT16_TO_STREAM(p, duration);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_voice_prompt_create(uint32_t  session_id,
                              uint8_t   coder_id,
                              uint8_t   frame_num,
                              uint16_t  coder_format_size,
                              uint8_t  *coder_format,
                              uint16_t  io_table_size,
                              uint8_t  *io_table)
{
    uint8_t  *cmd_buf;
    uint16_t  cmd_size;

    cmd_size = DIPC_H2D_HEAD_LEN + 12 + coder_format_size + io_table_size;
    cmd_buf  = malloc(cmd_size);
    if (cmd_buf != NULL)
    {
        uint8_t *p;
        bool     ret;

        p = cmd_buf;
        DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_VOICE_PROMPT_CREATE);
        DIPC_LE_UINT16_TO_STREAM(p, cmd_size - DIPC_H2D_HEAD_LEN);
        DIPC_LE_UINT32_TO_STREAM(p, session_id);
        DIPC_LE_UINT16_TO_STREAM(p, io_table_size);
        DIPC_LE_UINT16_TO_STREAM(p, 0x0000);
        DIPC_ARRAY_TO_STREAM(p, io_table, io_table_size);
        DIPC_LE_UINT8_TO_STREAM(p, coder_id);
        DIPC_LE_UINT8_TO_STREAM(p, frame_num);
        DIPC_LE_UINT16_TO_STREAM(p, coder_format_size);
        DIPC_ARRAY_TO_STREAM(p, coder_format, coder_format_size);

        ret = dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
        free(cmd_buf);
        return ret;
    }

    return false;
}

bool dipc_voice_prompt_destroy(uint32_t session_id)
{
    uint8_t cmd_buf[8];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_VOICE_PROMPT_DESTROY);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_voice_prompt_start(uint32_t session_id,
                             uint8_t  fade)
{
    uint8_t cmd_buf[9];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_VOICE_PROMPT_START);
    DIPC_LE_UINT16_TO_STREAM(p, 5);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p,  fade);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_voice_prompt_stop(uint32_t session_id,
                            uint8_t  fade)
{
    uint8_t cmd_buf[9];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_VOICE_PROMPT_STOP);
    DIPC_LE_UINT16_TO_STREAM(p, 5);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p,  fade);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_voice_prompt_gain_set(uint32_t session_id,
                                uint16_t gain_step_left,
                                uint16_t gain_step_right)
{
    uint8_t cmd_buf[12];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_VOICE_PROMPT_GAIN_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 8);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_left);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_right);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_ringtone_create(uint32_t  session_id,
                          uint16_t  tbl_size,
                          uint8_t  *tbl,
                          uint16_t  coefficient_vector_size,
                          uint8_t  *coefficient_vector)
{
    uint8_t *cmd_buf;
    uint16_t cmd_length;
    uint8_t *p;
    bool ret;

    cmd_length = 12 + tbl_size + 2 + coefficient_vector_size;

    cmd_buf = malloc(cmd_length);
    if (cmd_buf == NULL)
    {
        return false;
    }

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_RINGTONE_CREATE);
    DIPC_LE_UINT16_TO_STREAM(p, cmd_length - DIPC_H2D_HEAD_LEN);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, tbl_size);
    DIPC_LE_UINT16_TO_STREAM(p, 0x000000);
    DIPC_ARRAY_TO_STREAM(p, tbl, tbl_size);
    DIPC_LE_UINT16_TO_STREAM(p, coefficient_vector_size);
    DIPC_ARRAY_TO_STREAM(p, coefficient_vector, coefficient_vector_size);

    ret = dipc2_cmd_send(cmd_buf, p - cmd_buf, true);

    free(cmd_buf);

    return ret;
}

bool dipc_ringtone_start(uint32_t session_id,
                         uint8_t  fade)
{
    uint8_t cmd_buf[9];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_RINGTONE_START);
    DIPC_LE_UINT16_TO_STREAM(p, 5);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p,  fade);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_ringtone_stop(uint32_t session_id,
                        uint8_t  fade)
{
    uint8_t cmd_buf[9];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_RINGTONE_STOP);
    DIPC_LE_UINT16_TO_STREAM(p, 5);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p,  fade);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_ringtone_destroy(uint32_t session_id)
{
    uint8_t cmd_buf[8];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_RINGTONE_DESTROY);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_ringtone_gain_set(uint32_t session_id,
                            uint16_t gain_step_left,
                            uint16_t gain_step_right)
{
    uint8_t cmd_buf[12];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_RINGTONE_GAIN_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 8);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_left);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_right);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_apt_create(uint32_t  session_id,
                     uint16_t  io_table_size,
                     uint8_t  *io_table)
{
    uint8_t *cmd_buf;
    uint16_t cmd_size;
    uint8_t *p;
    bool ret;

    cmd_size = DIPC_H2D_HEAD_LEN + 8 + io_table_size;

    cmd_buf = malloc(cmd_size);
    if (cmd_buf == NULL)
    {
        return false;
    }

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_ANC_CREATE);
    DIPC_LE_UINT16_TO_STREAM(p, cmd_size - DIPC_H2D_HEAD_LEN);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, io_table_size);
    DIPC_LE_UINT16_TO_STREAM(p, 0x0000);
    DIPC_ARRAY_TO_STREAM(p, io_table, io_table_size);

    ret = dipc2_cmd_send(cmd_buf, p - cmd_buf, true);

    free(cmd_buf);

    return ret;
}

bool dipc_apt_destroy(uint32_t session_id)
{
    uint8_t cmd_buf[8];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_ANC_DESTROY);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_apt_start(uint32_t session_id,
                    uint8_t  fade)
{
    uint8_t cmd_buf[9];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_ANC_START);
    DIPC_LE_UINT16_TO_STREAM(p, 5);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p,  fade);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_apt_stop(uint32_t session_id,
                   uint8_t  fade)
{
    uint8_t cmd_buf[9];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_ANC_STOP);
    DIPC_LE_UINT16_TO_STREAM(p, 5);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p,  fade);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_vad_create(uint32_t  session_id,
                     uint8_t   aggressiveness_level,
                     uint8_t   coder_id,
                     uint16_t  coder_format_size,
                     uint8_t  *coder_format,
                     uint8_t   data_mode,
                     uint8_t   frame_num,
                     uint16_t  io_table_size,
                     uint8_t  *io_table)
{
    uint8_t  *cmd_buf;

    cmd_buf  = malloc(DIPC_H2D_HEAD_LEN + 12 + coder_format_size + io_table_size);
    if (cmd_buf != NULL)
    {
        uint8_t *p;
        bool     ret;

        session_id |= DIPC_SESSION_TYPE_ENCODER;

        p = cmd_buf;
        DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_VAD_CREATE);
        DIPC_LE_UINT16_TO_STREAM(p, 12 + coder_format_size + io_table_size);
        DIPC_LE_UINT32_TO_STREAM(p, session_id);
        DIPC_LE_UINT8_TO_STREAM(p, aggressiveness_level);
        DIPC_LE_UINT8_TO_STREAM(p, coder_id);
        DIPC_LE_UINT16_TO_STREAM(p, coder_format_size);
        DIPC_ARRAY_TO_STREAM(p, coder_format, coder_format_size);
        DIPC_LE_UINT8_TO_STREAM(p, data_mode);
        DIPC_LE_UINT8_TO_STREAM(p, frame_num);
        DIPC_LE_UINT16_TO_STREAM(p, io_table_size);
        DIPC_ARRAY_TO_STREAM(p, io_table, io_table_size);

        ret = dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
        free(cmd_buf);
        return ret;
    }

    return false;
}

bool dipc_vad_destroy(uint32_t session_id)
{
    uint8_t  cmd_buf[8];
    uint8_t *p;

    session_id |= DIPC_SESSION_TYPE_ENCODER;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_VAD_DESTROY);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_vad_start(uint32_t session_id)
{
    uint8_t  cmd_buf[8];
    uint8_t *p;

    session_id |= DIPC_SESSION_TYPE_ENCODER;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_VAD_START);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_vad_stop(uint32_t session_id)
{
    uint8_t  cmd_buf[8];
    uint8_t *p;

    session_id |= DIPC_SESSION_TYPE_ENCODER;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_VAD_STOP);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_vad_filter(uint32_t session_id,
                     uint8_t  level)
{
    uint8_t  cmd_buf[9];
    uint8_t *p;

    session_id |= DIPC_SESSION_TYPE_ENCODER;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_VAD_FILTER);
    DIPC_LE_UINT16_TO_STREAM(p, 5);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p, level);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_kws_create(uint32_t  session_id,
                     uint8_t   coder_id,
                     uint8_t   frame_num,
                     uint16_t  coder_format_size,
                     uint8_t  *coder_format)
{
    uint8_t  *cmd_buf;

    cmd_buf  = malloc(DIPC_H2D_HEAD_LEN + 8 + coder_format_size);
    if (cmd_buf != NULL)
    {
        uint8_t *p;
        bool     ret;

        p = cmd_buf;
        DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_KWS_CREATE);
        DIPC_LE_UINT16_TO_STREAM(p, 8 + coder_format_size);
        DIPC_LE_UINT32_TO_STREAM(p, session_id);
        DIPC_LE_UINT8_TO_STREAM(p, coder_id);
        DIPC_LE_UINT8_TO_STREAM(p, frame_num);
        DIPC_LE_UINT16_TO_STREAM(p, coder_format_size);
        DIPC_ARRAY_TO_STREAM(p, coder_format, coder_format_size);

        ret = dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
        free(cmd_buf);
        return ret;
    }

    return false;
}

bool dipc_kws_destroy(uint32_t session_id)
{
    uint8_t  cmd_buf[8];
    uint8_t *p;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_KWS_DESTROY);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_kws_start(uint32_t session_id)
{
    uint8_t  cmd_buf[8];
    uint8_t *p;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_KWS_START);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_kws_stop(uint32_t session_id)
{
    uint8_t  cmd_buf[8];
    uint8_t *p;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_KWS_STOP);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_apt_dac_gain_set(uint32_t session_id,
                           uint16_t gain_step_left,
                           uint16_t gain_step_right)
{
    uint8_t cmd_buf[12];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_ANC_DAC_GAIN_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 8);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_left);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_right);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_apt_adc_gain_set(uint32_t session_id,
                           uint16_t gain_step_left,
                           uint16_t gain_step_right)
{
    uint8_t cmd_buf[12];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_ANC_ADC_GAIN_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 8);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_left);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_right);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_apt_gain_ramp(uint32_t session_id,
                        uint16_t gain_step_left,
                        uint16_t gain_step_right,
                        uint16_t duration)
{
    uint8_t cmd_buf[14];
    uint8_t *p;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_ANC_GAIN_RAMP);
    DIPC_LE_UINT16_TO_STREAM(p, 10);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_left);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step_right);
    DIPC_LE_UINT16_TO_STREAM(p, duration);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_kws_set(uint32_t session_id)
{
    uint8_t  cmd_buf[8];
    uint8_t *p;

    session_id |= DIPC_SESSION_TYPE_ENCODER;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_KWS_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_kws_clear(uint32_t session_id)
{
    uint8_t  cmd_buf[8];
    uint8_t *p;

    session_id |= DIPC_SESSION_TYPE_ENCODER;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_KWS_CLEAR);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_eq_set(uint32_t  session_id,
                 uint8_t  *info_buf,
                 uint16_t  info_len)
{
    uint8_t *cmd_buf;
    uint8_t *p;
    bool ret;

    cmd_buf = calloc(1, DIPC_H2D_HEAD_LEN + 6 + info_len);
    if (cmd_buf == NULL)
    {
        return false;
    }

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_EQ_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 6 + info_len);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, info_len);
    DIPC_ARRAY_TO_STREAM(p, info_buf, info_len);

    ret = dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
    free(cmd_buf);

    return ret;
}

bool dipc_eq_clear(uint32_t session_id)
{
    uint8_t cmd_buf[8];
    uint8_t *p;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_EQ_CLEAR);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_nrec_set(uint32_t session_id,
                   uint8_t  mode,
                   uint8_t  level)
{
    uint8_t  cmd_buf[10];
    uint8_t *p;

    session_id |= DIPC_SESSION_TYPE_ENCODER;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_NREC_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 6);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p, mode);
    DIPC_LE_UINT8_TO_STREAM(p, level);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_nrec_clear(uint32_t session_id)
{
    uint8_t cmd_buf[8];
    uint8_t *p;

    session_id |= DIPC_SESSION_TYPE_ENCODER;

    p = cmd_buf;

    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_NREC_CLEAR);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_voice_sidetone_set(uint32_t session_id,
                             uint16_t gain_step)
{
    uint8_t cmd_buf[10];
    uint8_t *p;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_SIDETONE_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 6);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT16_TO_STREAM(p, gain_step);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_voice_sidetone_clear(uint32_t session_id)
{
    uint8_t cmd_buf[8];
    uint8_t *p;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_SIDETONE_CLEAR);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_apt_ovp_set(uint32_t session_id,
                      uint8_t  level)
{
    uint8_t cmd_buf[9];
    uint8_t *p;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_ANC_OVP_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 5);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p, level);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_apt_ovp_clear(uint32_t session_id)
{
    uint8_t cmd_buf[8];
    uint8_t *p;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_ANC_OVP_CLEAR);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_apt_beamforming_set(uint32_t session_id,
                              uint8_t  direction)
{
    uint8_t cmd_buf[9];
    uint8_t *p;

    session_id |= DIPC_SESSION_TYPE_ENCODER;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_BEAMFORMING_SET);
    DIPC_LE_UINT16_TO_STREAM(p, 5);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);
    DIPC_LE_UINT8_TO_STREAM(p, direction);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_apt_beamforming_clear(uint32_t session_id)
{
    uint8_t cmd_buf[8];
    uint8_t *p;

    session_id |= DIPC_SESSION_TYPE_ENCODER;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_BEAMFORMING_CLEAR);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_wdrc_set(uint32_t  session_id,
                   uint8_t  *info_buf,
                   uint16_t  info_len)
{
    uint8_t  *cmd_buf;
    uint16_t  cmd_len;

    cmd_len = 8 + info_len;
    cmd_buf = malloc(cmd_len);
    if (cmd_buf != NULL)
    {
        uint8_t  *p;
        bool      ret;

        p = cmd_buf;
        DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_WDRC_SET);
        DIPC_LE_UINT16_TO_STREAM(p, cmd_len - DIPC_H2D_HEAD_LEN);
        DIPC_LE_UINT32_TO_STREAM(p, session_id);
        DIPC_ARRAY_TO_STREAM(p, info_buf, info_len);

        ret = dipc2_cmd_send(cmd_buf, cmd_len, true);
        free(cmd_buf);
        return ret;
    }

    return false;
}

bool dipc_wdrc_clear(uint32_t session_id)
{
    uint8_t  cmd_buf[8];
    uint8_t *p;

    p = cmd_buf;
    DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_WDRC_CLEAR);
    DIPC_LE_UINT16_TO_STREAM(p, 4);
    DIPC_LE_UINT32_TO_STREAM(p, session_id);

    return dipc2_cmd_send(cmd_buf, p - cmd_buf, true);
}

bool dipc_vse_req(uint32_t  session_id,
                  uint16_t  company_id,
                  uint16_t  effect_id,
                  uint16_t  seq_num,
                  uint8_t  *info_buf,
                  uint16_t  info_len)
{
    uint8_t  *cmd_buf;
    uint16_t  cmd_len;

    cmd_len = 16 + info_len;
    cmd_buf = malloc(cmd_len);
    if (cmd_buf != NULL)
    {
        uint8_t *p;
        bool     ret;

        p = cmd_buf;
        DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_VENDOR_SPECIFIC_EFFECT_REQUEST);
        DIPC_LE_UINT16_TO_STREAM(p, cmd_len - DIPC_H2D_HEAD_LEN);
        DIPC_LE_UINT32_TO_STREAM(p, session_id);
        DIPC_LE_UINT16_TO_STREAM(p, company_id);
        DIPC_LE_UINT16_TO_STREAM(p, effect_id);
        DIPC_LE_UINT16_TO_STREAM(p, seq_num);
        DIPC_LE_UINT16_TO_STREAM(p, info_len);
        DIPC_ARRAY_TO_STREAM(p, info_buf, info_len);

        ret = dipc2_cmd_send(cmd_buf, cmd_len, true);
        free(cmd_buf);
        return ret;
    }

    return false;
}

bool dipc_vse_cfm(uint32_t  session_id,
                  uint16_t  company_id,
                  uint16_t  effect_id,
                  uint16_t  seq_num,
                  uint8_t  *info_buf,
                  uint16_t  info_len)
{
    uint8_t  *cmd_buf;
    uint16_t  cmd_len;

    cmd_len = 16 + info_len;
    cmd_buf = malloc(cmd_len);
    if (cmd_buf != NULL)
    {
        uint8_t *p;
        bool     ret;

        p = cmd_buf;
        DIPC_LE_UINT16_TO_STREAM(p, DIPC_H2D_VENDOR_SPECIFIC_EFFECT_CONFIRM);
        DIPC_LE_UINT16_TO_STREAM(p, cmd_len - DIPC_H2D_HEAD_LEN);
        DIPC_LE_UINT32_TO_STREAM(p, session_id);
        DIPC_LE_UINT16_TO_STREAM(p, company_id);
        DIPC_LE_UINT16_TO_STREAM(p, effect_id);
        DIPC_LE_UINT16_TO_STREAM(p, seq_num);
        DIPC_LE_UINT16_TO_STREAM(p, info_len);
        DIPC_ARRAY_TO_STREAM(p, info_buf, info_len);

        ret = dipc2_cmd_send(cmd_buf, cmd_len, true);
        free(cmd_buf);
        return ret;
    }

    return false;
}
