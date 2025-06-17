/**
 * @file   app_asp_device_rfc.h
 * @brief  struct and interface about asp api for app
 * @author leon
 * @date   2020.9.3
 * @version 1.0
 * @par Copyright (c):
         Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_ASP_DEVICE_H_
#define _APP_ASP_DEVICE_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if F_APP_TEAMS_FEATURE_SUPPORT
#include "asp_device_api.h"
#include "teams_util.h"
#include "asp_device_link.h"
/** @defgroup RWS_APP RWS APP
  * @brief Provides rws app interfaces.
  * @{
  */

/** @defgroup APP_ASP_DEVICE APP ASP DEVICE module init
* @brief Provides App Asp Device profile interfaces.
* @{
*/

#define APP_ASP_PENDING_TIMEOUT (10*1000) /*10s*/
#define APP_ASP_PER_REPORT_PERIOD  (60*1000) /*60s*/

/**
*  @brief enum for audio codec type
*/
typedef enum
{
    T_APP_ASP_DEVICE_ASP_DEAUTH,
    T_APP_ASP_DEVICE_ASP_AUTH,
    T_APP_ASP_DEVICE_AUDIO_ACTIVE_IDX_CHANGED,
    T_APP_ASP_DEVICE_HOLD_STATUS_CHANGE
} T_APP_ASP_DEVICE_ASP_EVENT_PYTE;

typedef enum
{
    T_APP_ASP_DEVICE_TELEMETRY_AUDIO_CODEC_NARROWBAND_TYPE = 1,
    T_APP_ASP_DEVICE_TELEMETRY_AUDIO_CODEC_WIDEBAND_TYPE
} T_APP_ASP_DEVICE_TELEMETRY_AUDIO_CODEC_TYPE;

typedef enum
{
    T_APP_ASP_DEVICE_TELEMETRY_BATTERY_LEVEL_OFF = 1,
    T_APP_ASP_DEVICE_TELEMETRY_BATTERY_LEVEL_LOW,
    T_APP_ASP_DEVICE_TELEMETRY_BATTERY_LEVEL_MED,
    T_APP_ASP_DEVICE_TELEMETRY_BATTERY_LEVEL_HI
} T_APP_ASP_DEVICE_TELEMETRY_BATTERY_LEVEL_TYPE;

typedef enum
{
    T_APP_ASP_DEVICE_TELEMETRY_RADIO_LINK_QUALITY_OFF = 1,
    T_APP_ASP_DEVICE_TELEMETRY_RADIO_LINK_QUALITY_LOW,
    T_APP_ASP_DEVICE_TELEMETRY_RADIO_LINK_QUALITY_HI
} T_APP_ASP_DEVICE_TELEMETRY_RADIO_LINK_QUALITY_TYPE;

typedef enum
{
    APP_ASP_LINK_PENDING
} T_APP_ASP_TIMER;

typedef struct
{
    uint8_t bd_addr[6];
    bool pending_flag;
    bool used;
    uint8_t idx;

    T_APP_ASP_DEVICE_TELEMETRY_RADIO_LINK_QUALITY_TYPE radio_quality;
    T_ASP_DEVICE_LINK_STATE link_state;
    uint8_t resv[1];
    uint32_t pending_dt_mask;
    uint8_t app_asp_timer_handle_pending;
    uint32_t asp_link_auth_timestamp;
    uint32_t asp_link_active_timestamp;
} T_APP_ASP_LINK_INFO;

typedef struct
{
    uint8_t bd_addr[6];
    bool pending_flag;
    uint8_t used;
    uint32_t pending_dt_mask;
    uint32_t asp_link_auth_timestamp;
    uint32_t asp_link_active_timestamp;
} T_APP_ASP_LINK_RELAY_INFO; //20 bytes

typedef struct
{
    T_APP_ASP_LINK_RELAY_INFO app_asp_link_relay_info[TEAMS_LINK_NUM];
    uint8_t button_info[3];
    bool mute_lock;
    uint8_t conference_num;
    uint8_t active_asp_idx[6];
} T_APP_ASP_RELAY_INFO; //70 bytes


/**
*  @brief dsp effect
*/
#define TELEMETRY_DSP_EFFECT_ACOUSTIC_ECHO_CANCELLATION  (0x00000001)
#define TELEMETRY_DSP_EFFECT_AUTOMATIC_GAIN_CONTROL      (0x00000002)
#define TELEMETRY_DSP_EFFECT_NOISE_SUPPRESSION           (0x00000004)
#define TELEMETRY_DSP_EFFECT_SPEAKER_PROTECTION          (0x00000008)
#define TELEMETRY_DSP_EFFECT_BEAMFORMING                 (0x00000010)
#define TELEMETRY_DSP_EFFECT_CONSTANT_TONE_REMOVAL       (0x00000020)
#define TELEMETRY_DSP_EFFECT_EQUALIZER                   (0x00000040)
#define TELEMETRY_DSP_EFFECT_LOUDNESS_EQUALIZER          (0x00000080)
#define TELEMETRY_DSP_EFFECT_BASS_BOOST                  (0x00000100)
#define TELEMETRY_DSP_EFFECT_VIRTUAL_SURROUND            (0x00000200)
#define TELEMETRY_DSP_EFFECT_VIRTUAL_HEADPHONES          (0x00000400)
#define TELEMETRY_DSP_EFFECT_SPEAKER_FILL                (0x00000800)
#define TELEMETRY_DSP_EFFECT_ROOM_CORRECTION             (0x00001000)
#define TELEMETRY_DSP_EFFECT_BASS_MANAGERMENT            (0x00002000)
#define TELEMETRY_DSP_EFFECT_ENVIRONMENTAL_EFFECTS       (0x00004000)
#define TELEMETRY_DSP_EFFECT_SPEAKER_COMPENSATION        (0x00008000)
#define TELEMETRY_DSP_EFFECT_DYNAMIC_RANGE_COMPRESSION   (0x00010000)
#define TELEMETRY_DSP_EFFECT_OTHER                       (0x00020000)



/**
*  @brief function app asp module init
*  @note  task init call this function, the resource of need for asp module would init, including callback register
*/
bool app_asp_device_init(void);

/**
*  @brief interface for app to send telemetry update data msg
*  @param[in] br_addr  get from api asp_device_api_get_active_link_addr
*  @param[in] mask telemetry update data mask
*  @note   if mask have multiple data, their element id shall follow spec
*  @return bool
*  @retval true success
*  @retval false failed
*/
bool app_asp_device_send_teams_telemetry_update_msg(uint8_t *br_addr, uint32_t mask);

/**
*  @brief interface for app to send teams button invoked msg
*  @param[in] br_addr  get from api asp_device_api_get_active_link_addr
*  @param[in] mask telemetry update data mask
*  @note   if mask have multiple data, their element id shall follow spec
*  @return bool
*  @retval true success
*  @retval false failed
*/
bool app_asp_device_send_teams_button_invoked_msg(uint8_t *br_addr,
                                                  T_ASP_DEVICE_SOURCE_ID_TYPE source_id);

/**
 *  @brief function send api for ble about asp protocol
 *  @note  task init call this function, the rfcomm channel would init completly
 */
bool app_asp_device_send_pkt_by_ble(uint8_t conn_id, uint8_t *p_data, uint16_t data_len);

void app_asp_device_send_hook_button_status(bool is_off_hook);

void app_asp_device_send_mute_button_status(bool is_mute);

void app_asp_device_handle_battery(uint8_t battery_level_from_charger, bool is_first_power_on);

T_APP_ASP_LINK_INFO *app_asp_device_find_link_by_addr(uint8_t *bd_addr);

void app_asp_device_handle_device_state(bool device_state);

void app_asp_device_handle_per(uint8_t *bd_addr, uint8_t per);

void app_asp_device_handle_conference_change(uint8_t conference_num);

void app_asp_device_build_relay_info(T_APP_ASP_RELAY_INFO *info);

void app_asp_device_pre_handle_relay_info(void);

void app_asp_device_handle_relay_info(T_APP_ASP_RELAY_INFO *info, uint16_t data_len);

T_APP_ASP_LINK_INFO *app_asp_device_find_link_by_index(uint8_t index);

void app_asp_device_alloc_link(uint8_t *bd_addr);

void app_asp_device_delete_link(uint8_t *bd_addr);

void app_asp_device_set_active_index(uint8_t *bd_addr);

void app_asp_device_handle_asp_event(uint8_t *bd_addr, T_APP_ASP_DEVICE_ASP_EVENT_PYTE event_type);

bool app_teams_no_launch(void);

bool app_teams_is_launch(void);

bool app_teams_is_close(void);

bool app_teams_notify(void);

bool app_teams_clear_notification(void);

void app_teams_set_launch_status(uint8_t status);

void app_teams_set_notify_status(uint8_t status);

void app_teams_led_handle_notification(uint8_t *bd_addr);

/** End of APP_ASP_DEVICE
* @}
*/

/** End of RWS_APP
* @}
*/

#endif
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_ASP_DEVICE_H_ */
