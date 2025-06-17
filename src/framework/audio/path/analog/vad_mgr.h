/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _VAD_MGR_H_
#define _VAD_MGR_H_

#include <stdint.h>
#include <stdbool.h>
#include "vad.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum t_vad_mgr_event
{
    VAD_MGR_EVENT_SESSION_ENABLED  = 0x00,
    VAD_MGR_EVENT_SESSION_DISABLED = 0x01,
} T_VAD_MGR_EVENT;

typedef void *T_VAD_MGR_SESSION_HANDLE;

typedef void (*T_VAD_MGR_SESSION_CBACK)(void            *handle,
                                        T_VAD_MGR_EVENT  event,
                                        uint32_t         param);

bool vad_mgr_init(void);

void vad_mgr_deinit(void);

T_VAD_MGR_SESSION_HANDLE vad_mgr_session_create(T_VAD_TYPE               type,
                                                uint32_t                 sample_rate,
                                                T_VAD_MGR_SESSION_CBACK  cback,
                                                void                    *context);

void vad_mgr_session_destroy(T_VAD_MGR_SESSION_HANDLE handle);

void vad_mgr_session_enable(T_VAD_MGR_SESSION_HANDLE handle);

void vad_mgr_session_disable(T_VAD_MGR_SESSION_HANDLE handle);

bool vad_mgr_path_bind(T_VAD_MGR_SESSION_HANDLE  handle,
                       void                     *owner);

bool vad_mgr_path_unbind(T_VAD_MGR_SESSION_HANDLE  handle,
                         void                     *owner);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _VAD_MGR_H_ */
