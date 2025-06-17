/*
 * Copyright (c) 2020, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_TEAMS_HID_H_
#define _APP_TEAMS_HID_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "app_link_util.h"
/** @defgroup RWS_APP RWS APP
  * @brief Provides rws app interfaces.
  * @{
  */


/** @defgroup APP_TEAMS_HID APP TEAMS HID
  * @brief App Teams Hid
  * @{
  */

extern uint8_t *teams_rsp_cache;
extern uint16_t teams_rsp_len;

typedef enum
{
    APP_TEAMS_HID_REPORT_ACTION_NONE,
    APP_TEAMS_HID_REPORT_ACTION_GET,
    APP_TEAMS_HID_REPORT_ACTION_SET,
} T_APP_TEAMS_HID_REPORT_ACTION_TYPE;

typedef enum
{
    APP_TEAMS_HID_REPORT_SOURCE_USB,
    APP_TEAMS_HID_REPORT_SOURCE_BLE,
    APP_TEAMS_HID_REPORT_SOURCE_SPP,
    APP_TEAMS_HID_REPORT_SOURCE_CLASSIC_HID,
} T_APP_TEAMS_HID_REPORT_SOURCE;

typedef struct
{
    uint16_t data_len;
    uint8_t *p_data;
} T_APP_TEAMS_HID_USB_DATA;

typedef struct
{
    uint8_t conn_id;
    uint8_t report_type;
    uint16_t data_len;
    uint8_t *p_data;
} T_APP_TEAMS_HID_BLE_DATA;

typedef struct
{
    uint8_t bd_addr[6];
    uint16_t data_len;
    uint8_t *p_data;
} T_APP_TEAMS_HID_SPP_DATA;

typedef struct
{
    uint8_t bd_addr[6];
    uint8_t report_id;
    uint8_t report_type;
    uint16_t report_size;
    T_APP_TEAMS_HID_REPORT_ACTION_TYPE report_action;
    uint16_t data_len;
    uint8_t *p_data;
} T_APP_TEAMS_HID_CLASSIC_DATA;

typedef union
{
    T_APP_TEAMS_HID_USB_DATA teams_usb_data;
    T_APP_TEAMS_HID_BLE_DATA teams_ble_data;
    T_APP_TEAMS_HID_SPP_DATA teams_spp_data;
    T_APP_TEAMS_HID_CLASSIC_DATA teams_classic_hid_data;
} T_APP_TEAMS_HID_DATA;

/**
    * @brief  teams hid module init
    * @param  void
    * @return void
    */
bool app_teams_hid_init(void);

bool app_teams_hid_send_config_report(uint8_t report_id, T_APP_TEAMS_HID_REPORT_SOURCE source_type,
                                      T_APP_TEAMS_HID_DATA *hid_msg);

/**
 * @brief api to send wl report
 *
 * @param report_id
 * @param source_type
 * @param hid_msg
 * @return true
 * @return false
 * usb channel:
 *  T_APP_TEAMS_HID_DATA teams_hid_data;
    uint8_t report_data[8] = {0x39,1,2,3,4,5,6,7};
    teams_hid_data.teams_usb_data.data_len = sizeof(report_data);
    teams_hid_data.teams_usb_data.p_data = report_data;

    app_teams_hid_send_wl_report(0x39, APP_TEAMS_HID_REPORT_SOURCE_USB, &teams_hid_data);
 * spp channel:
 *  T_APP_TEAMS_HID_DATA teams_hid_data;
    uint8_t report_data[8] = {0x39,1,2,3,4,5,6,7};

    teams_hid_data.teams_spp_data.data_len = sizeof(report_data)-1;
    teams_hid_data.teams_spp_data.p_data = &report_data[1];
    memcpy(teams_hid_data.teams_spp_data.bd_addr, bd_addr, 6);
    app_teams_hid_send_wl_report(0x39, APP_TEAMS_HID_REPORT_SOURCE_SPP, &teams_hid_data);
 * classic hid channel:
    T_APP_TEAMS_HID_DATA teams_hid_data;
    uint8_t report_data[8] = {0x39,1,2,3,4,5,6,7};

    teams_hid_data.teams_classic_hid_data.data_len = sizeof(report_data)-1;
    teams_hid_data.teams_classic_hid_data.p_data = &report_data[1];
    teams_hid_data.teams_classic_hid_data.report_id = 0x39;
    teams_hid_data.teams_classic_hid_data.report_type = BT_HID_DEVICE_REPORT_TYPE_FEATURE;
    teams_hid_data.teams_classic_hid_data.report_action = APP_TEAMS_HID_REPORT_ACTION_SET;
    memcpy(teams_hid_data.teams_classic_hid_data.bd_addr, app_db.br_link[app_idx].bd_addr, 6);
    app_teams_hid_send_wl_report(0x39, APP_TEAMS_HID_REPORT_SOURCE_CLASSIC_HID, &teams_hid_data);
 */
bool app_teams_hid_send_classic_hid_data(uint8_t *bd_addr, uint8_t report_id, uint8_t report_type,
                                         T_APP_TEAMS_HID_REPORT_ACTION_TYPE report_action, uint16_t len,
                                         uint8_t *data);

bool app_teams_hid_send_wl_report(uint8_t report_id, T_APP_TEAMS_HID_REPORT_SOURCE source_type,
                                  T_APP_TEAMS_HID_DATA *hid_msg);

void app_teams_hid_handle_config_report(uint8_t report_id,
                                        T_APP_TEAMS_HID_REPORT_SOURCE source_type,
                                        T_APP_TEAMS_HID_DATA *p_hid_data);

void app_teams_hid_handle_wl_report(uint8_t report_id, T_APP_TEAMS_HID_REPORT_SOURCE source_type,
                                    T_APP_TEAMS_HID_DATA *p_hid_data);

bool app_teams_hid_vp_update_is_process_check(T_APP_BR_LINK **p_link);

/** End of APP_TEAMS_HID
* @}
*/

/** End of RWS_APP
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
