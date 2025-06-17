/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _NOTIFICATION_H_
#define _NOTIFICATION_H_

#include <stdbool.h>
#include <stdint.h>

#include "tts.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define NOTIFICATION_INVALID_INDEX  0xFF

#define NOTIFICATION_TYPE_NONE      0x00
#define NOTIFICATION_TYPE_RINGTONE  0x01
#define NOTIFICATION_TYPE_VP        0x02
#define NOTIFICATION_TYPE_TTS       0x03

typedef enum t_notification_state
{
    NOTIFICATION_STATE_IDLE     = 0x00,
    NOTIFICATION_STATE_RINGTONE = 0x01,
    NOTIFICATION_STATE_VP       = 0x02,
    NOTIFICATION_STATE_TTS      = 0x03,
    NOTIFICATION_STATE_RELAY    = 0x04,
} T_NOTIFICATION_STATE;

bool notification_init(uint16_t pool_size);

void notification_deinit(void);

uint8_t notification_mode_get(uint8_t type);

bool notification_mode_set(uint8_t type,
                           uint8_t mode);

uint8_t notification_volume_max_get(uint8_t type);

bool notification_volume_max_set(uint8_t type,
                                 uint8_t volume);

uint8_t notification_volume_min_get(uint8_t type);

bool notification_volume_min_set(uint8_t type,
                                 uint8_t volume);

uint8_t notification_volume_get(uint8_t type);

bool notification_volume_set(uint8_t type,
                             uint8_t volume);

bool notification_volume_mute(uint8_t type);

bool notification_volume_unmute(uint8_t type);

float notification_volume_balance_get(uint8_t type);

bool notification_volume_balance_set(uint8_t type,
                                     float   scale);

bool notification_push(uint8_t      type,
                       uint8_t      index,
                       uint8_t      language,
                       T_TTS_HANDLE handle,
                       bool         relay);

bool notification_stop(uint8_t      type,
                       T_TTS_HANDLE handle);

bool notification_write(uint8_t       type,
                        T_TTS_HANDLE  handle,
                        uint8_t      *buf,
                        uint16_t      len);

bool notification_flush(uint8_t type,
                        bool    relay);

bool notification_cancel(uint8_t type,
                         uint8_t index,
                         bool    relay);

T_TTS_HANDLE notification_tts_alloc(uint32_t length,
                                    uint32_t cfg);

bool notification_tts_free(T_TTS_HANDLE handle);

T_NOTIFICATION_STATE notification_get_state(void);

uint8_t notification_get_pending_elem_num(void);

uint8_t notification_remaining_count_get(uint8_t type);

void notification_sync_lost(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _NOTIFICATION_H_ */
