/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_EFFECT_H_
#define _AUDIO_EFFECT_H_

#include <stdbool.h>
#include <stdint.h>
#include "audio_type.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum t_audio_effect_xmit_type
{
    AUDIO_EFFECT_XMIT_TYPE_ENABLE  = 0x00,
    AUDIO_EFFECT_XMIT_TYPE_DISABLE = 0x01,
    AUDIO_EFFECT_XMIT_TYPE_FLUSH   = 0x02,
} T_AUDIO_EFFECT_XMIT_TYPE;

typedef enum t_audio_effect_recv_type
{
    AUDIO_EFFECT_RECV_TYPE_RESPONSE = 0x00,
    AUDIO_EFFECT_RECV_TYPE_INDICATE = 0x01,
    AUDIO_EFFECT_RECV_TYPE_NOTIFY   = 0x02,
} T_AUDIO_EFFECT_RECV_TYPE;

typedef enum t_audio_effect_event
{
    AUDIO_EFFECT_EVENT_CREATED  = 0x00,
    AUDIO_EFFECT_EVENT_ENABLED  = 0x01,
    AUDIO_EFFECT_EVENT_DISABLED = 0x02,
    AUDIO_EFFECT_EVENT_UPDATED  = 0x03,
    AUDIO_EFFECT_EVENT_RELEASED = 0x04,
} T_AUDIO_EFFECT_EVENT;

typedef bool (*P_AUDIO_EFFECT_XMIT)(T_AUDIO_EFFECT_INSTANCE   instance,
                                    T_AUDIO_EFFECT_XMIT_TYPE  xmit_type,
                                    void                     *context);

typedef bool (*P_AUDIO_EFFECT_RECV)(T_AUDIO_EFFECT_INSTANCE   instance,
                                    T_AUDIO_EFFECT_RECV_TYPE  recv_type,
                                    uint16_t                  company_id,
                                    uint16_t                  effect_id,
                                    uint16_t                  seq_num,
                                    uint16_t                  payload_len,
                                    uint8_t                  *payload);

typedef void (*P_AUDIO_EFFECT_CBACK)(T_AUDIO_EFFECT_INSTANCE  instance,
                                     T_AUDIO_EFFECT_EVENT     event,
                                     void                    *context);

T_AUDIO_EFFECT_INSTANCE audio_effect_create(P_AUDIO_EFFECT_XMIT xmit,
                                            P_AUDIO_EFFECT_RECV recv);

bool audio_effect_release(T_AUDIO_EFFECT_INSTANCE instance);

bool audio_effect_owner_set(T_AUDIO_EFFECT_INSTANCE  instance,
                            void                    *owner,
                            P_AUDIO_EFFECT_CBACK     cback);

void *audio_effect_owner_get(T_AUDIO_EFFECT_INSTANCE instance);

bool audio_effect_owner_clear(T_AUDIO_EFFECT_INSTANCE instance);

bool audio_effect_inheritor_set(T_AUDIO_EFFECT_INSTANCE  instance,
                                void                    *inheritor);

void *audio_effect_inheritor_get(T_AUDIO_EFFECT_INSTANCE instance);

bool audio_effect_inheritor_clear(T_AUDIO_EFFECT_INSTANCE instance);

bool audio_effect_enable(T_AUDIO_EFFECT_INSTANCE instance);

bool audio_effect_disable(T_AUDIO_EFFECT_INSTANCE instance);

bool audio_effect_update(T_AUDIO_EFFECT_INSTANCE  instance,
                         void                    *context);

bool audio_effect_run(T_AUDIO_EFFECT_INSTANCE instance);

bool audio_effect_stop(T_AUDIO_EFFECT_INSTANCE instance);

bool audio_effect_flush(T_AUDIO_EFFECT_INSTANCE  instance,
                        void                    *context);

bool audio_effect_response(T_AUDIO_EFFECT_INSTANCE  instance,
                           uint16_t                 company_id,
                           uint16_t                 effect_id,
                           uint16_t                 seq_num,
                           uint16_t                 payload_len,
                           uint8_t                 *payload);

bool audio_effect_indicate(T_AUDIO_EFFECT_INSTANCE  instance,
                           uint16_t                 company_id,
                           uint16_t                 effect_id,
                           uint16_t                 seq_num,
                           uint16_t                 payload_len,
                           uint8_t                 *payload);

bool audio_effect_notify(T_AUDIO_EFFECT_INSTANCE  instance,
                         uint16_t                 company_id,
                         uint16_t                 effect_id,
                         uint16_t                 seq_num,
                         uint16_t                 payload_len,
                         uint8_t                 *payload);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_EFFECT_H_ */
