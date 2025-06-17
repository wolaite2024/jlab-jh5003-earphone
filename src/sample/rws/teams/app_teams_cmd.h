/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_TEAMS_CMD_H_
#define _APP_TEAMS_CMD_H_
#if F_APP_TEAMS_FEATURE_SUPPORT

#include <stdint.h>
#include <stdbool.h>
#include "app_teams_hid.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct
{
    uint32_t teams_tones_status;
    uint32_t teams_voice_prompt_status;

    uint8_t teams_limitation_mode;
    uint8_t teams_limitation_mode_max_volume;
    uint8_t teams_ptt_status;
    uint8_t teams_test_mode;
} T_TEAMS_CMD_DATA;

#define APP_COMMAND_MAX_FOR_USB   (63)  //count of usb command response
extern int16_t teams_dac_gain;
extern T_TEAMS_CMD_DATA app_teams_cmd_data;
extern uint8_t *app_command_for_usb;

typedef enum
{
    TEAMS_CMD_DEMO = 0x01,
} T_TEAMS_CMD_ID;

typedef enum
{
    TEAMS_RSP_STATUS_OK = 0x00,/*Request are all correct and accepted*/
    TEAMS_RSP_STATUS_INVALID_PARAMETER = 0x02,/*Some parameter in request are not correct*/
    TEAMS_RSP_STATUS_FAIL = 0x03,/*Requests are correct but operation failed*/
    TEAMS_RSP_STATUS_DATA_NOT_READY = 0x04/*Response data in processing*/
} T_TEAMS_RSP_STATUS;

typedef enum
{
    MIC_MUTE_KEY_MASK = 0x40,/*key6*/
    MIC_KEY_LONG_PRESS_INTERVAL = 0x0A,
} T_TEAMS_KEY_MASK;

typedef struct
{
    uint8_t device_capability[16];
    uint8_t device_model[4];
    uint8_t serial_number[16];
    uint8_t region_code[2];
    uint8_t color_code[16];
    uint8_t bundle_id[4];
} T_DEVICE_CONSTANT_CONFIGURATION;

/*============================================================================*
 *                              Functions
 *============================================================================*/
bool app_teams_cmd_get_send_vgm_flag(void);

void app_teams_cmd_set_send_vgm_flag(bool status);

void app_teams_cmd_handle_data(uint8_t *bd_addr, uint8_t report_id, uint8_t report_type,
                               T_APP_TEAMS_HID_REPORT_ACTION_TYPE report_action,
                               T_APP_TEAMS_HID_REPORT_SOURCE report_source,
                               uint16_t len, uint8_t *data);

void app_teams_cmd_send_response_data(uint8_t *bd_addr, uint8_t report_id, uint8_t report_type,
                                      T_APP_TEAMS_HID_REPORT_ACTION_TYPE report_action,
                                      T_APP_TEAMS_HID_REPORT_SOURCE report_source,
                                      uint16_t len, uint8_t *data);

uint8_t *app_teams_cmd_get_usb_response(void);

#if F_APP_TEAMS_CUSTOMIZED_CMD_SUPPORT
bool app_teams_cmd_get_device_capability(const void **p_buff, uint16_t *buff_len);

bool app_teams_cmd_get_device_model(const void **p_buff, uint16_t *buff_len);

bool app_teams_cmd_get_device_serial_number(const void **p_buff, uint16_t *buff_len);

bool app_teams_cmd_get_color_code(const void **p_buff, uint16_t *buff_len);

bool app_teams_cmd_get_region_code(const void **p_buff, uint16_t *buff_len);

bool app_teams_cmd_load_info(void *p_data);

bool app_teams_cmd_save_info(void *p_data);

uint32_t app_teams_cmd_info_reset(void);

void app_teams_cmd_info_init(void);

bool app_teams_cmd_user_config_reset(void);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

#endif
