/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_TEAMS_CUSTOMIZED_CMD_H_
#define _APP_TEAMS_CUSTOMIZED_CMD_H_
#if F_APP_TEAMS_FEATURE_SUPPORT

#if F_APP_TEAMS_CUSTOMIZED_CMD_SUPPORT
#include <stdint.h>
#include <stdbool.h>
#include "app_teams_hid.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    TEAMS_CMD_GET_DEVICE_CAPABILITY = 0x02,
    TEAMS_CMD_GET_CHARGE_STATUS = 0x10,
    TEAMS_CMD_GET_SERIAL_NUMBER = 0x18,
    TEAMS_CMD_GET_BT_ADDR = 0x1B,
    TEAMS_CMD_GET_COLOR_NAME = 0x1C,
    TEAMS_CMD_GET_FIRMWARE_VERSION = 0x2F,
    TEAMS_CMD_SET_CURRENT_EQ = 0x32,
    TEAMS_CMD_GET_CURRENT_EQ = 0x33,
    TEAMS_CMD_SET_TONES_STATUS = 0x34,
    TEAMS_CMD_GET_TONES_STATUS = 0x35,
    TEAMS_CMD_SET_TONES_VOLUME = 0x36,
    TEAMS_CMD_GET_TONES_VOLUME = 0x37,
    TEAMS_CMD_SET_VOLUME_LIMITATION = 0x38,
    TEAMS_CMD_GET_VOLUME_LIMITATION = 0x39,
    TEAMS_CMD_SET_PTT_STATUS = 0x3A,
    TEAMS_CMD_GET_PTT_STATUS = 0x3B,
    TEAMS_CMD_GET_REGION_CODE = 0x3C,
    TEAMS_CMD_GET_PAIRED_HOST_INFO = 0x3D,
    TEAMS_CMD_REMOVE_PAIRED_HOST_INFO = 0x3E,
    TEAMS_CMD_GET_BT_CONNECTION_STATE = 0x3F,
    TEAMS_CMD_SET_BT_CONNECTION_STATE = 0x40,
    TEAMS_CMD_SET_DEVICE_NAME = 0x41,
    TEAMS_CMD_GET_DEVICE_NAME = 0x42,
    TEAMS_CMD_GET_LANGUAGE_LIST = 0x43,
    TEAMS_CMD_SET_ACTIVE_LANGUAGE = 0x44,
    TEAMS_CMD_USER_CONFIGURATION_RESET = 0x47,
    TEAMS_CMD_GET_PRODUCT_NAME = 0x48,
    TEAMS_CMD_GET_DEVICE_MODEL_CODE = 0x49,
    TEAMS_CMD_SET_CUSTOMIZED_EQ = 0x4A,
    TEAMS_CMD_GET_CUSTOMIZED_EQ = 0x4B,
    TEAMS_CMD_REMOVE_CUSTOMIZED_EQ = 0x4C,
    TEAMS_CMD_GET_ACTIVE_EQ_TYPE = 0x4D,
    TEAMS_CMD_PLAY_TONE = 0x4E,
    TEAMS_CMD_SET_TEST_MODE = 0x80,
    TEAMS_CMD_GET_TEST_MODE = 0x81,
    TEAMS_CMD_GET_COMPONENT_STATUS = 0x86,
    TEAMS_CMD_SET_LED_STATUS = 0x87,
    TEAMS_CMD_DEVICE_REBOOT = 0x8A,
    TEAMS_CMD_FACTORY_RESET = 0x8B,
    TEAMS_CMD_ENABLE_MIC_EQ = 0x92,
    TEAMS_CMD_ENABLE_SPEAKER_EQ = 0x93,
    TEAMS_CMD_DISABLE_MIC = 0x95,
    TEAMS_CMD_FW_FULL_RESET = 0x97,
    TEAMS_CMD_SET_BT_MODE = 0x98,
    TEAMS_CMD_GET_BT_MODE = 0x99,
    TEAMS_CMD_GET_LINK_KEY = 0xF0
} T_TEAMS_CUSTOMIZED_CMD_ID;

typedef enum
{
    T_TEAMS_BUTTON_STATUS = 0x01,
    T_TEAMS_BATTERY_INFO = 0x02,
    T_TEAMS_LED_STATUS = 0x03,
    T_TEAMS_MIC_STATUS = 0x04,
    T_TEAMS_ENCODER_STATUS = 0x05,
} T_TEAMS_COMPONENT_ID;

typedef enum
{
    TEAMS_MTE_MODE = 0x08,
} T_TEAMS_TEST_MODE;

/*============================================================================*
 *                              Functions
 *============================================================================*/
void app_teams_customized_cmd_handle_data(uint8_t *bd_addr, uint8_t report_id, uint8_t report_type,
                                          T_APP_TEAMS_HID_REPORT_ACTION_TYPE report_action,
                                          T_APP_TEAMS_HID_REPORT_SOURCE report_source,
                                          uint16_t len, uint8_t *data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

#endif

#endif
