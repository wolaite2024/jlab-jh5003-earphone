/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DONGLE_SOURCE_CTRL_H_
#define _APP_DONGLE_SOURCE_CTRL_H_

#include "app_hfp.h"

typedef enum
{
    ALLOWED_SOURCE_24G     = 0x00,
    ALLOWED_SOURCE_BT      = 0x01,
    ALLOWED_SOURCE_BT_24G  = 0x02,
    ALLOWED_SOURCE_MAX     = 0x03,
} T_ALLOWED_SOURCE;

typedef enum
{
    EVENT_SOURCE_SWITCH_TO_BT        = 0x01,
    EVENT_SOURCE_SWITCH_TO_DONGLE    = 0x02,
    EVENT_SOURCE_SWITCH_TO_BT_DONGLE = 0x03,
} T_SOURCE_CTRL_EVENT;

void app_dongle_source_ctrl_init(void);
void app_dongle_switch_allowed_source(void);
void app_dongle_source_ctrl_evt_handler(T_SOURCE_CTRL_EVENT event);
#if F_APP_LEA_SUPPORT
#if TARGET_LE_AUDIO_GAMING
void app_dongle_update_allowed_source(void);
#endif
void app_dongle_source_switch_to_dongle_le_audio_handle(uint8_t le_audio_num,
                                                        T_APP_LE_LINK *p_dongle_link, T_APP_LE_LINK *p_phone_link);
void app_dongle_source_switch_to_bt_le_audio_handle(uint8_t le_audio_num,
                                                    T_APP_LE_LINK *p_dongle_link, T_APP_LE_LINK *p_phone_link);
bool app_dongle_is_allowed_le_source(T_APP_LE_LINK *p_link);
#endif
void app_dongle_set_allowed_source(T_ALLOWED_SOURCE allowed_source);

#endif

