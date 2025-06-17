/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _GATEWAY_H_
#define _GATEWAY_H_

#include <stdint.h>
#include <stdbool.h>
#include "audio_type.h"
#include "audio_route.h"
#include "vad.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void *T_GATEWAY_SESSION_HANDLE;

typedef enum t_gateway_event
{
    GATEWAY_EVENT_SUSPEND  = 0x00,
    GATEWAY_EVENT_RESUME   = 0x01,
    GATEWAY_EVENT_DATA_IND = 0x02,
} T_GATEWAY_EVENT;

typedef void (*T_GATEWAY_CBACK)(T_GATEWAY_EVENT event, void *context);

bool gateway_init(void);

void gateway_deinit(void);

T_GATEWAY_SESSION_HANDLE gateway_session_create(T_AUDIO_CATEGORY  category,
                                                uint32_t          tx_sample_rate,
                                                uint32_t          rx_sample_rate,
                                                uint32_t          device,
                                                T_VAD_TYPE        vad_type,
                                                uint16_t          vad_frame_len,
                                                T_GATEWAY_CBACK   cback,
                                                void             *context);

bool gateway_session_destroy(T_GATEWAY_SESSION_HANDLE handle);

bool gateway_session_enable(T_GATEWAY_SESSION_HANDLE handle);

bool gateway_session_disable(T_GATEWAY_SESSION_HANDLE handle);

uint32_t gateway_session_sample_rate_get(T_GATEWAY_SESSION_HANDLE   handle,
                                         T_AUDIO_ROUTE_GATEWAY_TYPE type,
                                         T_AUDIO_ROUTE_GATEWAY_IDX  id,
                                         T_AUDIO_ROUTE_GATEWAY_DIR  dir);

uint16_t gateway_session_data_len_peek(T_GATEWAY_SESSION_HANDLE handle);

uint16_t gateway_session_data_recv(T_GATEWAY_SESSION_HANDLE  handle,
                                   uint8_t                  *buffer,
                                   uint16_t                  length);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _GATEWAY_H_ */
