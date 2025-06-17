/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_FINDMY_BLE_ADV_H_
#define _APP_FINDMY_BLE_ADV_H_

#include "fmna_constants.h"
#include "fmna_adv_platform.h"
#include "remote.h"

#define APP_FINDMY_FAST_ADV_TIMEOUT  300
#define APP_FINDMY_SLOW_ADV_TIMEOUT  0

bool app_findmy_adv_start(uint16_t timeout_sec);

bool app_findmy_adv_stop(uint8_t app_cause);

void app_findmy_enter_pair_mode(void);

void app_findmy_handle_role_swap_success(T_REMOTE_SESSION_ROLE device_role);

void app_findmy_handle_role_swap_fail(T_REMOTE_SESSION_ROLE device_role);

uint8_t app_findmy_adv_get_adv_handle(void);

void app_findmy_adv_init(void);

#endif /* app_findmy_ble_adv_h */
