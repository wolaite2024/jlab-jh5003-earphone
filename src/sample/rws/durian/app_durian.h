/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DURIAN_H_
#define _APP_DURIAN_H_

#include "sysm.h"
#include "bt_avp.h"
#include "trace.h"
#include "durian.h"
#include "durian_avp.h"
#include "durian_atti.h"
#include "durian_adv.h"
#include "app_durian_anc.h"
#include "app_durian_avp.h"
#include "app_durian_cfg.h"
#include "app_durian_adv.h"
#include "app_durian_link.h"
#include "app_durian_audio.h"
#include "app_durian_adp.h"
#include "app_durian_mmi.h"
#include "app_durian_sync.h"
#include "app_durian_cmd.h"
#include "app_durian_key.h"
#include "app_durian_loc.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    DURIAN_EVENT_START             = 0x00,
    DURIAN_EVENT_INIT              = 0x01,

} T_DURIAN_EVENT;

typedef enum
{
    APP_DURIAN_AVP_ANC_CLOSE                  = 0x01,
    APP_DURIAN_AVP_ANC_OPEN                   = 0x02,
    APP_DURIAN_AVP_ANC_TRANSPARENCY_MODE      = 0x03,
} T_APP_DURIAN_AVP_ANC;

typedef enum
{
    APP_DURIAN_AVP_BUD_UNKNOWN,
    APP_DURIAN_AVP_BUD_IN_EAR,
    APP_DURIAN_AVP_BUD_OUT_OF_CASE,
    APP_DURIAN_AVP_BUD_IN_CASE,
} T_APP_DURIAN_AVP_BUD_LOCATION;

typedef enum
{
    APP_DURIAN_AVP_CONTROL_NONE                 = 0x00,
    APP_DURIAN_AVP_CONTROL_SIRI                 = 0x01,
    APP_DURIAN_AVP_CONTROL_PLAY_PAUSE           = 0x02,
    APP_DURIAN_AVP_CONTROL_FORWARD              = 0x03,
    APP_DURIAN_AVP_CONTROL_BACKWARD             = 0x04,
} T_APP_DURIAN_AVP_CONTROL;

typedef enum
{
    APP_DURIAN_AVP_CONTROL_VOICE_RECOGNITION    = 0x00,
    APP_DURIAN_AVP_CONTROL_ANC                  = 0x01,
    APP_DURIAN_AVP_CONTROL_VOL_DOWN             = 0x02,
    APP_DURIAN_AVP_CONTROL_VOL_UP               = 0x03,
} T_APP_DURIAN_AVP_LONGPRESS_CONTROL;

typedef enum
{
    APP_DURIAN_AVP_MIC_AUTO                  = 0x00,
    APP_DURIAN_AVP_MIC_ALLWAYS_RIGHT         = 0x01,
    APP_DURIAN_AVP_MIC_ALLWAYS_LEFT          = 0x02,
} T_APP_DURIAN_AVP_MIC;

typedef enum
{
    APP_DURIAN_AVP_CLICK_SPEED_DEFAULT        = 0x00,
    APP_DURIAN_AVP_CLICK_SPEED_SLOW           = 0x01,
    APP_DURIAN_AVP_CLICK_SPEED_SLOWEST        = 0x02,
} T_APP_DURIAN_AVP_CLICK_SPEED;

typedef enum
{
    APP_DURIAN_AVP_LONG_PRESS_TIME_DEFAULT    = 0x00,
    APP_DURIAN_AVP_LONG_PRESS_TIME_SHORT      = 0x01,
    APP_DURIAN_AVP_LONG_PRESS_TIME_SHORTEST   = 0x02,
} T_APP_DURIAN_AVP_LONG_PRESS_TIME;

typedef enum
{
    APP_DURIAN_AVP_EVENT_SET_NAME                          = 0x02,
    APP_DURIAN_AVP_EVENT_CONTROL_SETTINGS                  = 0x03,
    APP_DURIAN_AVP_EVENT_IN_EAR_DETECTION                  = 0x04,
    APP_DURIAN_AVP_EVENT_MIC_SETTINGS                      = 0x05,
    APP_DURIAN_AVP_EVENT_ANC_SETTINGS                      = 0x06,
    APP_DURIAN_AVP_EVENT_CLICK_SETTINGS                    = 0x07,
    APP_DURIAN_AVP_EVENT_ANC_APT_CYCLE                     = 0x08,
    APP_DURIAN_AVP_EVENT_COMPACTNESS_TEST                  = 0x09,
    APP_DURIAN_AVP_EVENT_CLICK_SPEED                       = 0x0a,
    APP_DURIAN_AVP_EVENT_LONG_RESS_TIME                    = 0x0b,
    APP_DURIAN_AVP_EVENT_ONE_BUD_ANC                       = 0x0c,
    APP_DURIAN_AVP_EVENT_SPATIAL_AUDIO                     = 0x0d,
    APP_DURIAN_AVP_EVENT_VERSION_SYNC                      = 0x0e,
    APP_DURIAN_AVP_EVENT_DIGITAL_CROWN_SETTINGS            = 0x0f,
    APP_DURIAN_AVP_EVENT_AUDIO_SHARING                     = 0x10,
    APP_DURIAN_AVP_EVENT_VOLUME_CONTROL_SETTINGS           = 0x11,
    APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_START           = 0x82,
    APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_ENCODE_START    = 0x83,
    APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_ENCODE_STOP     = 0x84,
    APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_STOP            = 0x85,
    APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_ERROR           = 0x86,
    APP_DURIAN_AVP_EVENT_BALANCED_TONE_SLIGHT              = 0x87,
    APP_DURIAN_AVP_EVENT_BALANCED_TONE_MODERATE            = 0x88,
    APP_DURIAN_AVP_EVENT_BALANCED_TONE_STRONG              = 0x89,
    APP_DURIAN_AVP_EVENT_VOCAL_RANGE_SLIGHT                = 0x8a,
    APP_DURIAN_AVP_EVENT_VOCAL_RANGE_MODERATE              = 0x8b,
    APP_DURIAN_AVP_EVENT_VOCAL_RANGE_STRONG                = 0x8c,
    APP_DURIAN_AVP_EVENT_BRIGHTNESS_SLIGHT                 = 0x8d,
    APP_DURIAN_AVP_EVENT_BRIGHTNESS_MODERATE               = 0x8e,
    APP_DURIAN_AVP_EVENT_BRIGHTNESS_STRONG                 = 0x8f,
    APP_DURIAN_AVP_EVENT_APT_GAIN_SETTINGS                 = 0x90,
    APP_DURIAN_AVP_EVENT_APT_TONE_SETTINGS                 = 0x91,
    APP_DURIAN_AVP_EVENT_AUTO_APT_SETTINGS                 = 0x92,
} T_APP_DURIAN_AVP_EVENT;

typedef struct
{
    uint8_t  bd_addr[6];
    uint16_t len;
    uint8_t *data;
} T_APP_DURIAN_AVP_EVENT_SET_NAME;

typedef struct
{
    uint8_t          bd_addr[6];
    T_APP_DURIAN_AVP_CONTROL left_ear_control;
    T_APP_DURIAN_AVP_CONTROL right_ear_control;
} T_APP_DURIAN_AVP_EVENT_CONTROL_SETTINGS;

typedef struct
{
    uint8_t bd_addr[6];
    bool    open;
} T_APP_DURIAN_AVP_EVENT_IN_EAR_DETECTION;

typedef struct
{
    uint8_t      bd_addr[6];
    T_APP_DURIAN_AVP_MIC setting;
} T_APP_DURIAN_AVP_EVENT_MIC_SETTINGS;

typedef struct
{
    uint8_t      bd_addr[6];
    T_APP_DURIAN_AVP_ANC setting;
} T_APP_DURIAN_AVP_EVENT_ANC_SETTINGS;

typedef struct
{
    uint8_t bd_addr[6];
    T_APP_DURIAN_AVP_LONGPRESS_CONTROL left_ear_longpress_control;
    T_APP_DURIAN_AVP_LONGPRESS_CONTROL right_ear_longpress_control;
} T_APP_DURIAN_AVP_EVENT_CLICK_SETTINGS;

typedef struct
{
    uint8_t bd_addr[6];
    uint8_t setting;
} T_APP_DURIAN_AVP_EVENT_ANC_APT_CYCLE;

typedef struct
{
    uint8_t bd_addr[6];
} T_APP_DURIAN_AVP_EVENT_COMPACTNESS_TEST;

typedef struct
{
    uint8_t              bd_addr[6];
    T_APP_DURIAN_AVP_CLICK_SPEED speed;
} T_APP_DURIAN_AVP_EVENT_CLICK_SPEED_SETTINGS;

typedef struct
{
    uint8_t                  bd_addr[6];
    T_APP_DURIAN_AVP_LONG_PRESS_TIME time;
} T_APP_DURIAN_AVP_EVENT_LONG_PRESS_TIME_SETTINGS;

typedef struct
{
    uint8_t bd_addr[6];
    bool    is_open;
} T_APP_DURIAN_AVP_EVENT_ONE_BUD_ANC;

typedef struct
{
    uint8_t  bd_addr[6];
    uint8_t  len;
    uint8_t *data;
} T_APP_DURIAN_AVP_EVENT_SPATIAL_AUDIO;

typedef struct
{
    uint8_t bd_addr[6];
} T_APP_DURIAN_AVP_EVENT_VERSION_SYNC;

typedef struct
{
    uint8_t bd_addr[6];
    bool    is_clockwise;
} T_APP_DURIAN_AVP_EVENT_DIGITAL_CROWN_SETTINGS;

typedef struct
{
    uint8_t bd_addr[6];
    bool    is_enable;
} T_APP_DURIAN_AVP_EVENT_AUDIO_SHARING;

typedef struct
{
    uint8_t bd_addr[6];
    bool    is_enable;
} T_APP_DURIAN_AVP_EVENT_VOLUME_CONTROL_SETTINGS;

typedef struct
{
    uint8_t bd_addr[6];
} T_APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_START;

typedef struct
{
    uint8_t bd_addr[6];
} T_APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_ENCODE_START;

typedef struct
{
    uint8_t bd_addr[6];
} T_APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_ENCODE_STOP;

typedef struct
{
    uint8_t bd_addr[6];
} T_APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_STOP;

typedef struct
{
    uint8_t bd_addr[6];
} T_APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_ERROR;

typedef struct
{
    uint8_t bd_addr[6];
} T_APP_DURIAN_AVP_EVENT_BALANCED_TONE_SLIGHT;

typedef struct
{
    uint8_t bd_addr[6];
} T_APP_DURIAN_AVP_EVENT_BALANCED_TONE_MODERATE;

typedef struct
{
    uint8_t bd_addr[6];
} T_APP_DURIAN_AVP_EVENT_BALANCED_TONE_STRONG;

typedef struct
{
    uint8_t bd_addr[6];
} T_APP_DURIAN_AVP_EVENT_VOCAL_RANGE_SLIGHT;

typedef struct
{
    uint8_t bd_addr[6];
} T_APP_DURIAN_AVP_EVENT_VOCAL_RANGE_MODERATE;

typedef struct
{
    uint8_t bd_addr[6];
} T_APP_DURIAN_AVP_EVENT_VOCAL_RANGE_STRONG;

typedef struct
{
    uint8_t bd_addr[6];
} T_APP_DURIAN_AVP_EVENT_BRIGHTNESS_SLIGHT;

typedef struct
{
    uint8_t bd_addr[6];
} T_APP_DURIAN_AVP_EVENT_BRIGHTNESS_MODERATE;

typedef struct
{
    uint8_t bd_addr[6];
} T_APP_DURIAN_AVP_EVENT_BRIGHTNESS_STRONG;

typedef struct
{
    uint8_t  bd_addr[6];
    uint32_t left_ear_gian;
    uint32_t right_ear_gian;
} T_APP_DURIAN_AVP_EVENT_APT_GAIN_SETTINGS;

typedef struct
{
    uint8_t  bd_addr[6];
    uint32_t left_ear_tone;
    uint32_t right_ear_tone;
} T_APP_DURIAN_AVP_EVENT_APT_TONE_SETTINGS;

typedef struct
{
    uint8_t  bd_addr[6];
    bool     enable;
} T_APP_DURIAN_AVP_EVENT_AUTO_APT_SETTINGS;

typedef union
{
    T_APP_DURIAN_AVP_EVENT_SET_NAME                         set_name;
    T_APP_DURIAN_AVP_EVENT_CONTROL_SETTINGS                 control_settings;
    T_APP_DURIAN_AVP_EVENT_IN_EAR_DETECTION                 in_ear_detection;
    T_APP_DURIAN_AVP_EVENT_MIC_SETTINGS                     mic_settings;
    T_APP_DURIAN_AVP_EVENT_ANC_SETTINGS                     anc_settings;
    T_APP_DURIAN_AVP_EVENT_CLICK_SETTINGS                   click_settings;
    T_APP_DURIAN_AVP_EVENT_ANC_APT_CYCLE                    cycle_settings;
    T_APP_DURIAN_AVP_EVENT_COMPACTNESS_TEST                 compactness_test;
    T_APP_DURIAN_AVP_EVENT_CLICK_SPEED_SETTINGS             click_speed_settings;
    T_APP_DURIAN_AVP_EVENT_LONG_PRESS_TIME_SETTINGS         long_press_time_settings;
    T_APP_DURIAN_AVP_EVENT_ONE_BUD_ANC                      one_bud_anc;
    T_APP_DURIAN_AVP_EVENT_SPATIAL_AUDIO                    spatial_audio;
    T_APP_DURIAN_AVP_EVENT_VERSION_SYNC                     version_sync;
    T_APP_DURIAN_AVP_EVENT_DIGITAL_CROWN_SETTINGS           digital_crown_settings;
    T_APP_DURIAN_AVP_EVENT_AUDIO_SHARING                    audio_sharing;
    T_APP_DURIAN_AVP_EVENT_VOLUME_CONTROL_SETTINGS          volume_control_settings;
    T_APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_START          voice_recognition_start;
    T_APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_ENCODE_START   voice_recognition_encode_start;
    T_APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_ENCODE_STOP    voice_recognition_encode_stop;
    T_APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_STOP           voice_recognition_stop;
    T_APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_ERROR          voice_recognition_error;
    T_APP_DURIAN_AVP_EVENT_BALANCED_TONE_SLIGHT             balanced_tone_slight;
    T_APP_DURIAN_AVP_EVENT_BALANCED_TONE_MODERATE           balanced_tone_moderate;
    T_APP_DURIAN_AVP_EVENT_BALANCED_TONE_STRONG             balanced_tone_strong;
    T_APP_DURIAN_AVP_EVENT_VOCAL_RANGE_SLIGHT               vocal_range_slight;
    T_APP_DURIAN_AVP_EVENT_VOCAL_RANGE_MODERATE             vocal_range_moderate;
    T_APP_DURIAN_AVP_EVENT_VOCAL_RANGE_STRONG               vocal_range_strong;
    T_APP_DURIAN_AVP_EVENT_BRIGHTNESS_SLIGHT                brightness_slight;
    T_APP_DURIAN_AVP_EVENT_BRIGHTNESS_MODERATE              brightness_moderate;
    T_APP_DURIAN_AVP_EVENT_BRIGHTNESS_STRONG                brightness_strong;
    T_APP_DURIAN_AVP_EVENT_APT_GAIN_SETTINGS                apt_gain_settings;
    T_APP_DURIAN_AVP_EVENT_APT_TONE_SETTINGS                apt_tone_settings;
    T_APP_DURIAN_AVP_EVENT_AUTO_APT_SETTINGS                auto_apt_settings;
} T_APP_DURIAN_AVP_EVENT_PARAM;

typedef struct
{
    bool left_click_anc_en;
    bool right_click_anc_en;
    T_AVP_CYCLE_SETTING_ANC anc_cycle_setting;

    union
    {
        uint8_t local_remote_double_action;

        struct
        {
            uint8_t remote_double_click_action: 4;
            uint8_t local_double_click_action: 4;
        };
    };

    union
    {
        uint8_t local_remote_long_action;

        struct
        {
            uint8_t remote_long_action: 4;
            uint8_t local_long_action: 4;
        };
    };

    uint8_t ear_detect_en;

    T_APP_DURIAN_AVP_MIC mic_setting;

#if F_APP_LISTENING_MODE_SUPPORT
    uint8_t anc_one_bud_enabled;
    T_AVP_ANC_SETTINGS anc_cur_setting;
    T_AVP_ANC_SETTINGS anc_pre_setting;
    T_AVP_ANC_SETTINGS anc_one_setting;
    T_AVP_ANC_SETTINGS anc_both_setting;

    bool anc_apt_need_tone;

#if DURIAN_PRO2
    uint8_t auto_apt_en;
#endif

    bool apt_off_cause_siri;
    bool apt_off_cause_call;
#endif

#if DURIAN_PRO2
    uint8_t vol_ctl_en;
#endif

#if F_APP_TWO_GAIN_TABLE
    bool amplify_gain;
#endif

    uint8_t local_loc;
    uint8_t remote_loc;

    bool both_in_ear_src_lost;
    bool local_loc_changed;
    bool remote_loc_changed;

    bool click_speed_rec;
    uint8_t click_speed;
    uint8_t click_speed_origin;

    bool long_press_time_rec;
    uint8_t long_press_time;
    uint8_t long_press_origin;

    bool power_on_by_cmd;
    bool power_off_by_cmd;

    bool power_on_from_factory_reset;

    T_AVP_COMPACTNESS_DB local_compactness;
    T_AVP_COMPACTNESS_DB remote_compactness;

    uint8_t both_in_ear;

    uint8_t adv_purpose;
    uint8_t adv_purpose_last;
    uint8_t adv_serial;

    bool b2b_synced;

    uint8_t local_batt;
    uint8_t remote_batt;

    uint8_t adv_disallow_update_batt: 1;
    uint8_t role_decided: 1;
    uint8_t need_open_case_adv: 1;
    uint8_t fast_pair_connected: 1;
    uint8_t avp_db_rsv: 4;

    uint8_t id_is_display;
    bool remote_loc_received;
} T_DURIAN_DB;

extern T_DURIAN_DB durian_db;

void app_durian_init(void);
bool app_durian_avp_couple_battery_level_report(uint8_t *bd_addr, uint8_t right_ear_level,
                                                uint8_t right_ear_charging,
                                                uint8_t left_ear_level, uint8_t left_ear_charging,
                                                uint8_t case_level, uint8_t case_status);
bool app_durian_avp_single_battery_level_report(uint8_t *bd_addr, uint8_t level, uint8_t charging);
bool app_durian_avp_anc_setting_report(uint8_t *bd_addr, T_APP_DURIAN_AVP_ANC anc_setting);
void app_durian_avp_bud_location_report(uint8_t *bd_addr,
                                        T_APP_DURIAN_AVP_BUD_LOCATION pri_ear_location,
                                        T_APP_DURIAN_AVP_BUD_LOCATION sec_ear_location, bool is_right);
bool app_durian_avp_voice_recognition_enable_req(uint8_t *bd_addr);
bool app_durian_avp_voice_recognition_disable_req(uint8_t *bd_addr);
bool app_durian_avp_voice_recognition_encode_start(uint8_t *bd_addr);
bool app_durian_avp_voice_recognition_encode_stop(uint8_t *bd_addr);
bool app_durian_avp_voice_recognition_data_send(uint8_t *bd_addr, uint8_t *p_data,
                                                uint16_t data_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
