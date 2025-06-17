/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_DURIAN_SUPPORT
#include <string.h>
#include <stdlib.h>
#include "trace.h"
#include "vad.h"
#include "gap_chann.h"
#include "gap_vendor.h"
#include "bt_att.h"
#include "app_durian_att.h"
#include "app_durian.h"
#include "app_main.h"
#include "app_roleswap_control.h"
#include "app_cfg.h"
#include "app_dsp_cfg.h"
#include "app_relay.h"
#include "app_hfp.h"
#include "app_a2dp.h"
#include "app_multilink.h"
#include "app_timer.h"
#include "app_audio_policy.h"
#include "app_report.h"

#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
#include "app_sensor_mems.h"
#endif

static T_AUDIO_TRACK_HANDLE avp_opus_handle = NULL;
static bool key_voice_reg_enable = false;
static uint8_t compactness_bd_addr[6] = {0};
uint8_t siri_seq = 0x00;

#define AVP_ATT_CONNECT_MS                1000
#define AVP_COMPACTNESS_MS                4000
#define AVP_VOICE_CALL_VP_DELAY_MS        200

const uint8_t all_bud_is_compacted[188] = { 0x04, 0x00, 0x04, 0x00, 0x21, 0x00, 0x01, 0x03, 0xB2, 0x00,
                                            0x00, 0x00, 0xD0, 0xE6, 0xD9, 0x41, 0x9B, 0x46, 0xAE, 0x3E,
                                            0xE0, 0x93, 0xC4, 0x3B, 0xFE, 0x9E, 0x2C, 0x3E, 0xF1, 0x87,
                                            0x11, 0x3F, 0xA5, 0x8D, 0x06, 0x3F, 0xA1, 0x28, 0x0F, 0x3F,
                                            0x3C, 0xD3, 0x1B, 0x3F, 0x1D, 0x17, 0xF2, 0x3E, 0xF1, 0x6F,
                                            0x25, 0x3F, 0xEF, 0xB9, 0x0B, 0x3F, 0xDA, 0x9C, 0x13, 0x3F,
                                            0xD5, 0xBD, 0x11, 0x3F, 0x8B, 0xC6, 0x0B, 0x3F, 0x10, 0x43,
                                            0x23, 0x3F, 0x75, 0xBA, 0x01, 0x3F, 0xF3, 0xC3, 0x02, 0x3F,
                                            0x68, 0xF8, 0xFF, 0x3E, 0xBE, 0x3B, 0x50, 0x3F, 0xDD, 0x52,
                                            0xC4, 0x3E, 0xB9, 0xCE, 0x05, 0x3F, 0xC8, 0x3F, 0xBE, 0x3E,
                                            0x8E, 0xCE, 0xB2, 0x41, 0x53, 0x02, 0x85, 0x3E, 0xC0, 0x80,
                                            0x87, 0x3C, 0xD0, 0x36, 0xE9, 0x3E, 0x32, 0x6F, 0x0C, 0x3F,
                                            0xBB, 0x35, 0x1A, 0x3F, 0xDC, 0xF6, 0x14, 0x3F, 0x57, 0xFB,
                                            0x1F, 0x3F, 0xD4, 0x75, 0x64, 0x3E, 0xB3, 0x3B, 0x30, 0x3F,
                                            0x24, 0x5C, 0xED, 0x3E, 0xDA, 0x57, 0xCD, 0x3E, 0x02, 0x34,
                                            0x2F, 0x3F, 0xED, 0x3E, 0xA5, 0x3E, 0x97, 0x18, 0xDC, 0x3E,
                                            0x2F, 0xB7, 0x1D, 0x3F, 0x3B, 0x58, 0x82, 0x3E, 0x59, 0x99,
                                            0xEC, 0x3E, 0x7D, 0xF1, 0x59, 0x3F, 0x71, 0xD5, 0xB7, 0x3E,
                                            0x1F, 0x33, 0x0A, 0x3F, 0xC5, 0xF5, 0x9C, 0x3E
                                          };

const uint8_t right_bud_is_compacted[188] = {0x04, 0x00, 0x04, 0x00, 0x21, 0x00, 0x01, 0x03, 0xB2, 0x00,
                                             0x00, 0x00, 0xC3, 0x45, 0x97, 0x41, 0x7C, 0xE2, 0x5E, 0x3D,
                                             0x40, 0x1E, 0x4F, 0x3C, 0xAB, 0x69, 0x8B, 0x3E, 0x8C, 0x9B,
                                             0x83, 0x3F, 0xB0, 0xC7, 0x94, 0x3F, 0x08, 0x00, 0xA5, 0x3F,
                                             0xE3, 0x5C, 0xA3, 0x3F, 0xFA, 0x2D, 0x91, 0x3F, 0x18, 0x98,
                                             0x8E, 0x3F, 0xA6, 0x8A, 0x39, 0x3F, 0xBB, 0x76, 0x57, 0x3F,
                                             0x75, 0x20, 0x0B, 0x3F, 0x59, 0xF7, 0x4B, 0x3F, 0x56, 0x47,
                                             0x45, 0x3F, 0x86, 0x4D, 0x26, 0x3F, 0x9F, 0x14, 0x32, 0x3F,
                                             0xBE, 0x36, 0x07, 0x3F, 0x25, 0x2D, 0xF2, 0x3E, 0xB0, 0x6D,
                                             0x0D, 0x3F, 0x02, 0x2A, 0x04, 0x3F, 0xE6, 0x78, 0xE0, 0x3E,
                                             0x7A, 0x31, 0x86, 0x41, 0xF7, 0xA6, 0xB8, 0x3D, 0xA0, 0xC2,
                                             0x35, 0x3C, 0xA1, 0x62, 0xDC, 0x3B, 0x04, 0x22, 0x65, 0x3D,
                                             0x60, 0xBF, 0x99, 0x3D, 0x32, 0x4C, 0xE0, 0x3D, 0xAF, 0x56,
                                             0x18, 0x3E, 0x00, 0xE8, 0x3F, 0x3E, 0x49, 0x92, 0x51, 0x3E,
                                             0xB6, 0x34, 0xB5, 0x3E, 0xC2, 0x83, 0xA5, 0x3E, 0xC0, 0xB3,
                                             0x2D, 0x3F, 0x9D, 0xBC, 0x95, 0x3E, 0xA0, 0x15, 0x1B, 0x3F,
                                             0x00, 0x69, 0xB6, 0x3E, 0x10, 0xB7, 0x08, 0x3F, 0x99, 0xCF,
                                             0x1F, 0x3F, 0x21, 0x16, 0x2B, 0x3F, 0x33, 0xC9, 0x90, 0x3F,
                                             0x6D, 0x85, 0xA7, 0x3F, 0x9D, 0x62, 0xD1, 0x3F
                                            };

const uint8_t left_bud_is_compacted[188] = {0x04, 0x00, 0x04, 0x00, 0x21, 0x00, 0x01, 0x03, 0xB2, 0x00,
                                            0x00, 0x00, 0x84, 0x9F, 0x65, 0x41, 0xF8, 0xD3, 0xA0, 0x3D,
                                            0x00, 0x94, 0x3D, 0x3A, 0x37, 0x5B, 0x79, 0x3C, 0xE9, 0x57,
                                            0x0D, 0x3D, 0x0C, 0x64, 0x56, 0x3D, 0x1C, 0xFF, 0x84, 0x3D,
                                            0x1D, 0xAB, 0x82, 0x3D, 0x21, 0x9F, 0xF7, 0x3D, 0xD8, 0xD1,
                                            0xF6, 0x3D, 0x5D, 0x81, 0x37, 0x3E, 0x58, 0x37, 0x1A, 0x3E,
                                            0x0E, 0x1F, 0x8B, 0x3E, 0x7E, 0x87, 0x31, 0x3E, 0x8B, 0x16,
                                            0xB1, 0x3E, 0x3D, 0x61, 0x4D, 0x3E, 0x44, 0x04, 0xBA, 0x3E,
                                            0x4B, 0x3A, 0xB1, 0x3E, 0x78, 0xEF, 0xAB, 0x3E, 0x5A, 0xEF,
                                            0xF1, 0x3E, 0x81, 0x39, 0x27, 0x3F, 0xA8, 0xE1, 0x2F, 0x3F,
                                            0x53, 0x47, 0xA9, 0x41, 0x36, 0x37, 0x9F, 0x3D, 0x00, 0x0D,
                                            0x2B, 0x3B, 0x05, 0x47, 0x88, 0x3E, 0x70, 0x3E, 0x82, 0x3F,
                                            0x7A, 0x46, 0x8A, 0x3F, 0x0E, 0x2F, 0xA3, 0x3F, 0x78, 0xFC,
                                            0xAF, 0x3F, 0x59, 0x95, 0xAC, 0x3F, 0x10, 0x0E, 0xA8, 0x3F,
                                            0xBA, 0xC3, 0x8F, 0x3F, 0xAC, 0x4C, 0x9E, 0x3F, 0xB1, 0x71,
                                            0x55, 0x3F, 0x43, 0x08, 0x74, 0x3F, 0x23, 0xB4, 0x48, 0x3F,
                                            0xB0, 0x3F, 0x5E, 0x3F, 0x49, 0xB7, 0x29, 0x3F, 0x40, 0x2A,
                                            0x55, 0x3F, 0xDA, 0xFA, 0x13, 0x3F, 0xA8, 0x5E, 0x2E, 0x3F,
                                            0xB4, 0xAD, 0x16, 0x3F, 0x1B, 0x18, 0xC5, 0x3E
                                           };

const uint8_t all_bud_is_not_compacted[188] = { 0x04, 0x00, 0x04, 0x00, 0x21, 0x00, 0x01, 0x03, 0xB2, 0x00,
                                                0x00, 0x00, 0x2C, 0xC3, 0xAD, 0x41, 0x04, 0xCC, 0xC1, 0x3D,
                                                0x00, 0x54, 0xBF, 0x3B, 0xDB, 0x6B, 0xE8, 0x3B, 0x49, 0x29,
                                                0x7B, 0x3C, 0x93, 0x99, 0xAF, 0x3C, 0x68, 0x43, 0xF4, 0x3C,
                                                0x53, 0xF6, 0x06, 0x3D, 0x1C, 0x1F, 0x3B, 0x3D, 0x1F, 0x46,
                                                0x5D, 0x3D, 0x99, 0x39, 0x96, 0x3D, 0x1E, 0xEE, 0xAC, 0x3D,
                                                0xF9, 0x5C, 0x07, 0x3E, 0x9E, 0xDB, 0xFC, 0x3D, 0x95, 0x49,
                                                0x2C, 0x3E, 0x6E, 0xEF, 0x35, 0x3E, 0x69, 0x05, 0x3F, 0x3E,
                                                0x54, 0x1F, 0x8F, 0x3E, 0x55, 0xCA, 0xA9, 0x3E, 0x94, 0x51,
                                                0xD3, 0x3E, 0x91, 0x69, 0x08, 0x3F, 0x33, 0xD2, 0x18, 0x3F,
                                                0x4C, 0x57, 0xB5, 0x41, 0x73, 0x44, 0xD9, 0x3D, 0xC0, 0x91,
                                                0xE0, 0x3B, 0x2B, 0xAA, 0x1B, 0x3C, 0xAF, 0x64, 0x3D, 0x3D,
                                                0x7D, 0xCF, 0xB4, 0x3D, 0x60, 0x1B, 0xC6, 0x3D, 0x2A, 0xC6,
                                                0x0B, 0x3E, 0x91, 0x98, 0x17, 0x3E, 0xAD, 0xDD, 0x40, 0x3E,
                                                0x04, 0x02, 0xB7, 0x3E, 0xBA, 0xD2, 0x8E, 0x3E, 0xEE, 0x92,
                                                0x22, 0x3F, 0x1B, 0x45, 0x8B, 0x3E, 0x88, 0x43, 0x07, 0x3F,
                                                0xBA, 0x71, 0xC4, 0x3E, 0x6E, 0xD5, 0xFE, 0x3E, 0x08, 0xBC,
                                                0x3C, 0x3F, 0x56, 0xC9, 0x44, 0x3F, 0xEC, 0xED, 0x8A, 0x3F,
                                                0x75, 0x92, 0xA3, 0x3F, 0x87, 0x05, 0xAB, 0x3F
                                              };


typedef enum
{
    APP_TIMER_ATT_CONNECT,
    APP_TIMER_AVP_COMPACTNESS,
    APP_TIMER_VOICE_CALL_VP_DELAY,
} T_APP_AVP_TIMER;

static uint8_t app_durian_avp_timer_id = 0;

static uint8_t timer_idx_att_connnect = 0;
static uint8_t timer_idx_avp_compactness = 0;
static uint8_t timer_idx_voice_call_vp_delay = 0;

static bool app_durian_avp_track_async_read(T_AUDIO_TRACK_HANDLE   handle,
                                            uint32_t              *timestamp,
                                            uint16_t              *seq_num,
                                            T_AUDIO_STREAM_STATUS *status,
                                            uint8_t               *frame_num,
                                            void                  *buf,
                                            uint16_t               required_len,
                                            uint16_t              *actual_len)
{
    uint8_t app_idx = app_hfp_get_active_idx();

    if ((app_db.br_link[app_idx].connected_profile & AVP_PROFILE_MASK) &&
        (app_db.br_link[app_idx].audio_opus_status == AVP_VIOCE_RECOGNITION_ENCODE_START))
    {
        uint8_t sniffing_state = app_db.br_link[app_idx].bt_sniffing_state;

        if ((sniffing_state == APP_BT_SNIFFING_STATE_IDLE) ||
            (sniffing_state == APP_BT_SNIFFING_STATE_SNIFFING) ||
            (sniffing_state == APP_BT_SNIFFING_STATE_READY))
        {
            app_durian_avp_voice_recognition_data_send(app_db.br_link[app_idx].bd_addr, buf, required_len);
            *actual_len = required_len;
            return true;
        }
    }

    return false;
}

void app_durian_avp_voice_start(uint8_t app_idx)
{
    if (app_db.br_link[app_idx].connected_profile & AVP_PROFILE_MASK)
    {
        app_start_timer(&timer_idx_voice_call_vp_delay, "voice_call_vp_delay",
                        app_durian_avp_timer_id, APP_TIMER_VOICE_CALL_VP_DELAY, 0, false,
                        AVP_VOICE_CALL_VP_DELAY_MS);
        app_durian_avp_voice_recognition_enable_req(app_db.br_link[app_idx].bd_addr);
        key_voice_reg_enable = true;
    }
}

void app_durian_avp_voice_recognition_check(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        audio_track_release(avp_opus_handle);
        avp_opus_handle = NULL;
    }
    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        for (uint8_t app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
        {
            if (app_link_check_b2s_link_by_id(app_idx))
            {
                if ((app_db.br_link[app_idx].audio_opus_status != AVP_VIOCE_RECOGNITION_IDLE) &&
                    ((app_db.br_link[app_idx].connected_profile & AVP_PROFILE_MASK) != 0))
                {
                    T_AUDIO_FORMAT_INFO opus_info;

                    durian_avp_opus_info(&opus_info, AUDIO_FORMAT_TYPE_OPUS);

                    avp_opus_handle = audio_track_create(AUDIO_STREAM_TYPE_RECORD,
                                                         AUDIO_STREAM_MODE_NORMAL,
                                                         AUDIO_STREAM_USAGE_LOCAL,
                                                         opus_info,
                                                         0,
                                                         app_dsp_cfg_vol.record_volume_default,
                                                         AUDIO_DEVICE_IN_MIC,
                                                         NULL,
                                                         app_durian_avp_track_async_read);

                    if (app_db.br_link[app_idx].audio_opus_status == AVP_VIOCE_RECOGNITION_ENCODE_START)
                    {
                        if (app_cfg_const.enable_vad == 1)
                        {
                            vad_disable();
                        }

#if (F_APP_LISTENING_MODE_SUPPORT == 1)
                        app_durain_anc_siri_start_check();
#endif
                        audio_track_start(avp_opus_handle);
                    }
                }
            }
        }
    }
}

T_AVP_VIOCE_RECOGNITION_STATUS app_durian_avp_get_opus_status(void)
{
    uint8_t app_idx = app_a2dp_get_active_idx();

    return (T_AVP_VIOCE_RECOGNITION_STATUS)app_db.br_link[app_idx].audio_opus_status;
}

bool app_durian_avp_couple_battery_level_report(uint8_t *bd_addr, uint8_t right_ear_level,
                                                uint8_t right_ear_charging,
                                                uint8_t left_ear_level, uint8_t left_ear_charging,
                                                uint8_t case_level, uint8_t case_status)
{
    uint8_t buf[22] = {0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x03, 0x02, 0x01, 0x33,/*batt_level*/
                       0x01, 0x01, 0x04, 0x01, 0x44,/*batt_level*/ 0x02, 0x01, 0x08, 0x01, 0x00,/*case_batt_level*/
                       0x04/*do not display*/, 0x01
                      };

    buf[9] = right_ear_level;
    buf[10] = right_ear_charging;
    buf[14] = left_ear_level;
    buf[15] = left_ear_charging;
    buf[19] = case_level;
    buf[20] = case_status;

    return bt_avp_data_send(bd_addr, buf, sizeof(buf), false);
}

bool app_durian_avp_single_battery_level_report(uint8_t *bd_addr, uint8_t level, uint8_t charging)
{
    uint8_t buf[12] = {0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x01, 0x01, 0x01, 0x33, 0x02, 0x01};

    buf[9] = level;
    buf[10] = charging;

    return bt_avp_data_send(bd_addr, buf, sizeof(buf), false);
}

bool app_durian_avp_anc_setting_report(uint8_t *bd_addr, T_APP_DURIAN_AVP_ANC anc_setting)
{
    if (anc_setting == APP_DURIAN_AVP_ANC_CLOSE)
    {
        uint8_t buf[11] = {0x04, 0x00, 0x04, 0x00, 0x09, 0x00, 0x0D, 0x01, 0x00, 0x00, 0x00};
        return bt_avp_data_send(bd_addr, buf, sizeof(buf), false);
    }
    else if (anc_setting == APP_DURIAN_AVP_ANC_OPEN)
    {
        uint8_t buf[11] = {0x04, 0x00, 0x04, 0x00, 0x09, 0x00, 0x0D, 0x02, 0x00, 0x00, 0x00};
        return bt_avp_data_send(bd_addr, buf, sizeof(buf), false);
    }
    else if (anc_setting == APP_DURIAN_AVP_ANC_TRANSPARENCY_MODE)
    {
        uint8_t buf[11] = {0x04, 0x00, 0x04, 0x00, 0x09, 0x00, 0x0D, 0x03, 0x00, 0x00, 0x00};
        return bt_avp_data_send(bd_addr, buf, sizeof(buf), false);
    }

    return true;
}

void app_durian_avp_bud_location_report(uint8_t *bd_addr,
                                        T_APP_DURIAN_AVP_BUD_LOCATION pri_ear_location,
                                        T_APP_DURIAN_AVP_BUD_LOCATION sec_ear_location, bool is_right)
{
    uint8_t buf1[8] = {0x04, 0x00, 0x04, 0x00, 0x06, 0x00, 0x02, 0x02};
    uint8_t buf2[10] = {0x04, 0x00, 0x04, 0x00, 0x08, 0x00, 0x02, 0x02, 0x01, 0x00};

    if (pri_ear_location == APP_DURIAN_AVP_BUD_IN_EAR)
    {
        buf1[6] = 0x00;
    }
    else if (pri_ear_location == APP_DURIAN_AVP_BUD_OUT_OF_CASE)
    {
        buf1[6] = 0x01;
    }
    else if (pri_ear_location == APP_DURIAN_AVP_BUD_IN_CASE)
    {
        buf1[6] = 0x02;
    }
    else if (pri_ear_location == APP_DURIAN_AVP_BUD_UNKNOWN)
    {
        buf1[6] = 0x03;
    }

    if (sec_ear_location == APP_DURIAN_AVP_BUD_IN_EAR)
    {
        buf1[7] = 0x00;
    }
    else if (sec_ear_location == APP_DURIAN_AVP_BUD_OUT_OF_CASE)
    {
        buf1[7] = 0x01;
    }
    else if (sec_ear_location == APP_DURIAN_AVP_BUD_IN_CASE)
    {
        buf1[7] = 0x02;
    }
    else if (sec_ear_location == APP_DURIAN_AVP_BUD_UNKNOWN)
    {
        buf1[7] = 0x03;
    }

    if (is_right)
    {
        buf2[6] = 0x02;
    }
    else
    {
        buf2[6] = 0x01;
    }
    bt_avp_data_send(bd_addr, buf1, sizeof(buf1), false);
    bt_avp_data_send(bd_addr, buf2, sizeof(buf2), false);
}

static bool app_durian_avp_compact_test_report(uint8_t *bd_addr, bool right_ear_result,
                                               bool left_ear_result)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);

    if (p_link != NULL)
    {
        if (right_ear_result == true)
        {
            if (left_ear_result == true)
            {
                return bt_avp_data_send(bd_addr, (uint8_t *)all_bud_is_compacted,
                                        sizeof(all_bud_is_compacted), false);
            }
            else
            {
                return bt_avp_data_send(bd_addr, (uint8_t *)right_bud_is_compacted,
                                        sizeof(right_bud_is_compacted), false);
            }
        }
        else
        {
            if (left_ear_result == true)
            {
                return bt_avp_data_send(bd_addr, (uint8_t *)left_bud_is_compacted,
                                        sizeof(left_bud_is_compacted), false);
            }
            else
            {
                return bt_avp_data_send(bd_addr, (uint8_t *)all_bud_is_not_compacted,
                                        sizeof(all_bud_is_not_compacted), false);
            }
        }
    }

    return false;
}

bool app_durian_avp_voice_recognition_enable_req(uint8_t *bd_addr)
{
    uint16_t conn_handle;
    uint16_t cid;
    uint8_t cid_num;
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        uint8_t buf[5] = {0x01, 0x00, 0x01, 0x01, 0x02};

        gap_chann_get_handle(bd_addr, 0x10/*BTIF_REMOTE_ADDR_CLASSIC*/, &conn_handle);
        gap_chann_get_cid(conn_handle, 1, &cid, &cid_num);

        return avp_att_service_send_notify3(conn_handle, cid, buf, sizeof(buf));
    }

    return false;
}

bool app_durian_avp_voice_recognition_disable_req(uint8_t *bd_addr)
{
    uint16_t conn_handle;
    uint16_t cid;
    uint8_t cid_num;
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        uint8_t buf[4] = {0x01, 0x00, 0x05, 0x00};

        gap_chann_get_handle(bd_addr, 0x10/*BTIF_REMOTE_ADDR_CLASSIC*/, &conn_handle);
        gap_chann_get_cid(conn_handle, 1, &cid, &cid_num);

        return avp_att_service_send_notify3(conn_handle, cid, buf, sizeof(buf));
    }

    return false;
}


bool app_durian_avp_voice_recognition_encode_start(uint8_t *bd_addr)
{
    uint16_t conn_handle;
    uint16_t cid;
    uint8_t cid_num;
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        uint8_t buf[2] = {0x01, 0x00};
        gap_chann_get_handle(bd_addr, 0x10/*BTIF_REMOTE_ADDR_CLASSIC*/, &conn_handle);
        gap_chann_get_cid(conn_handle, 1, &cid, &cid_num);

        return avp_att_service_send_notify2(conn_handle, cid, buf, sizeof(buf));
    }

    return false;
}

bool app_durian_avp_voice_recognition_encode_stop(uint8_t *bd_addr)
{
    uint16_t conn_handle;
    uint16_t cid;
    uint8_t cid_num;
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        uint8_t buf[4] = {0x01, 0x00, 0x03, 0x00};
        gap_chann_get_handle(bd_addr, 0x10/*BTIF_REMOTE_ADDR_CLASSIC*/, &conn_handle);
        gap_chann_get_cid(conn_handle, 1, &cid, &cid_num);

        return avp_att_service_send_notify3(conn_handle, cid, buf, sizeof(buf));
    }

    return false;
}

bool app_durian_avp_voice_recognition_data_send(uint8_t *bd_addr, uint8_t *p_data,
                                                uint16_t data_len)
{
    T_APP_BR_LINK *p_link;
    uint16_t conn_handle;
    uint16_t cid;
    uint8_t cid_num;
    bool ret = false;
    uint8_t *p_buf;
    uint8_t *p;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        p_buf = malloc(data_len + 10);
        if (p_buf)
        {
            p = p_buf;
            LE_UINT8_TO_STREAM(p, 0x01);
            LE_UINT8_TO_STREAM(p, 0x00);
            LE_UINT8_TO_STREAM(p, siri_seq);
            LE_UINT16_TO_STREAM(p, data_len + 5);
            LE_UINT16_TO_STREAM(p, 0x0000);
            LE_UINT8_TO_STREAM(p, siri_seq);
            LE_UINT8_TO_STREAM(p, 0x00);
            LE_UINT8_TO_STREAM(p, (uint8_t)data_len);
            memcpy(p, p_data, data_len);

            gap_chann_get_handle(bd_addr, 0x10/*BTIF_REMOTE_ADDR_CLASSIC*/, &conn_handle);
            gap_chann_get_cid(conn_handle, 1, &cid, &cid_num);
            ret = avp_att_service_send_notify1(conn_handle,  cid, p_buf, data_len + 10);

            free(p_buf);

            if (siri_seq == 0xff)
            {
                siri_seq = 0;
            }
            else
            {
                siri_seq++;
            }
        }
    }

    return ret;
}

void app_durian_avp_compactness_state_machine(bool force_pass)
{
    bool right_compactness, left_compactness;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED &&
        durian_db.remote_compactness.compactness_result == AVP_EARFIT_RESULT_UNKOWN &&
        !force_pass)
    {
        APP_PRINT_TRACE0("app_durian_avp_compactness_state_machine: wait for sec-bud testing result");
        return;
    }

    if (app_cfg_const.bud_side == DEVICE_ROLE_LEFT)
    {
        left_compactness = (durian_db.local_compactness.compactness_result == AVP_EARFIT_RESULT_GOOD) ?
                           true :
                           false;
        right_compactness = (durian_db.remote_compactness.compactness_result == AVP_EARFIT_RESULT_GOOD) ?
                            true : false;
    }
    else
    {
        right_compactness = (durian_db.local_compactness.compactness_result == AVP_EARFIT_RESULT_GOOD) ?
                            true :
                            false;
        left_compactness = (durian_db.remote_compactness.compactness_result == AVP_EARFIT_RESULT_GOOD) ?
                           true :
                           false;
    }
    if (force_pass)
    {
        left_compactness = true;
        right_compactness = true;
        if (app_db.remote_session_state != REMOTE_SESSION_STATE_CONNECTED)
        {
            if (app_cfg_const.bud_side == DEVICE_ROLE_LEFT)
            {
                right_compactness = false;
            }
            else
            {
                left_compactness = false;
            }
        }
    }
    app_durian_avp_compact_test_report(compactness_bd_addr, right_compactness,
                                       left_compactness);

    //CONTROL_ANC_APT
    if (!app_cfg_const.disable_fake_earfit_verification)
    {
        return;
    }
    //app_avp_compactness_anc_apt_ctrl(false);
}

#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
void app_durian_avp_atti_start(uint8_t app_idx)
{
    if (app_db.br_link[app_idx].mems_is_start)
    {
        return;
    }

#if (MEMS_EVB == 0)
    bool is_left = (app_cfg_const.bud_side == DEVICE_ROLE_LEFT) ? true : false;
#if TARGET_RTL8753EFE_VS_AVP&DURIAN_FOUR
    durian_atti_pos_set(PROD_ID_AIRPODS3, (gsensor_pos_e)app_cfg_const.mems_pos, is_left);
#endif
#if (TARGET_RTL8763ESE_AVP&DURIAN_PRO)|(TARGET_RTL8763ESE_AVP&DURIAN_PRO2)
    durian_atti_pos_set(PROD_ID_AIRPODS_PRO, (gsensor_pos_e)app_cfg_const.mems_pos, is_left);
#endif
#if TARGET_RTL8763ESE_AVP&DURIAN_BEATS_FIT
    app_cfg_const.mems_pos = GS_Z_UP_X_LEFT;
    durian_atti_pos_set(PROD_ID_BEATS_FIT_PRO, (gsensor_pos_e)app_cfg_const.mems_pos, is_left);
#endif
#endif

}
#endif

void app_durian_avp_handle_event(T_APP_DURIAN_AVP_EVENT event_type, void *event_buf,
                                 uint16_t buf_len)
{
    T_APP_DURIAN_AVP_EVENT_PARAM *param = event_buf;
    uint8_t active_hf_idx = app_hfp_get_active_idx();

    switch (event_type)
    {
    case APP_DURIAN_AVP_EVENT_SET_NAME:
        {
            app_durian_cfg_set_name(param->set_name.data, param->set_name.len);
            app_durian_adp_batt_report();
            app_durian_sync_report_event_broadcast(EVENT_AVP_NAME_SET, app_cfg_nv.device_name_legacy, 40);
        }
        break;

#if DURIAN_TWO
    case APP_DURIAN_AVP_EVENT_CONTROL_SETTINGS: //pod1&2
        {
            app_durian_key_control_set(param->control_settings.left_ear_control,
                                       param->control_settings.right_ear_control);


            uint8_t ear_control[2];
            ear_control[0] = param->control_settings.left_ear_control;
            ear_control[1] = param->control_settings.right_ear_control;
            app_durian_sync_report_event_broadcast(EVENT_AVP_CONTROL_SET, ear_control, 2);
        }
        break;
#endif

#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
    case APP_DURIAN_AVP_EVENT_CLICK_SETTINGS: //pro
        {
            uint8_t left_right_action;
            APP_PRINT_INFO2("app_avp_bt_cback: left_ear_longpress_control 0x%2x,right_ear_longpress_control 0x%2x",
                            param->click_settings.left_ear_longpress_control,
                            param->click_settings.right_ear_longpress_control);

            left_right_action = param->click_settings.left_ear_longpress_control << 4;
            left_right_action |= param->click_settings.right_ear_longpress_control;
            /*If action is anc switch:
              0x03: 1&3
              0x05: 2&3
              0x06: 1&2
              0x07: 1&2&3
            1:anc
            2.parent
            3.close;*/
            app_durian_key_click_set(left_right_action);
            app_durian_sync_report_event_broadcast(EVENT_AVP_CLICK_SET, &left_right_action, 1);
        }
        break;
#endif

#if (F_APP_LISTENING_MODE_SUPPORT == 1)
    case APP_DURIAN_AVP_EVENT_ANC_APT_CYCLE:
        {
            uint8_t cycle_set = param->cycle_settings.setting;
            app_durian_anc_cylce_set(cycle_set);
            app_durian_sync_report_event_broadcast(EVENT_AVP_ANC_APT_CYCLE, &cycle_set, 1);
        }
        break;

#if DURIAN_PRO2
    case APP_DURIAN_AVP_EVENT_AUTO_APT_SETTINGS:
        {
            uint8_t auto_apt_en = (uint8_t)param->auto_apt_settings.enable;
            app_durian_anc_auto_apt_cfg(auto_apt_en);
        }
        break;

    case APP_DURIAN_AVP_EVENT_VOLUME_CONTROL_SETTINGS:
        {
            uint8_t vol_control_en = (uint8_t)param->volume_control_settings.is_enable;
            app_durian_audio_vol_control(vol_control_en);
        }
        break;
#endif

    case APP_DURIAN_AVP_EVENT_ANC_SETTINGS:
        {
            app_durian_anc_set(param->anc_settings.setting);
        }
        break;

    case APP_DURIAN_AVP_EVENT_ONE_BUD_ANC:
        {
            APP_PRINT_INFO1("app_avp_bt_cback: one bud anc is open %d", param->one_bud_anc.is_open);
            durian_db.anc_one_bud_enabled = param->one_bud_anc.is_open;
            app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_ANC_ONE_BUD_ENABLE);
        }
        break;
#endif

    case APP_DURIAN_AVP_EVENT_IN_EAR_DETECTION:
        {
            uint8_t ear_detect_en = (uint8_t)param->in_ear_detection.open;
            app_durian_loc_in_ear_detect(ear_detect_en);
            app_durian_sync_report_event_broadcast(EVENT_EAR_DETECTION_STATUS, &ear_detect_en, 1);
        }
        break;

#if (F_APP_ERWS_SUPPORT == 1)
    case APP_DURIAN_AVP_EVENT_MIC_SETTINGS:
        {
            APP_PRINT_INFO1("app_avp_bt_cback: avp_mic_select %d", param->mic_settings.setting);
            durian_db.mic_setting = param->mic_settings.setting;

            app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_MIC_SETTING);
            app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_DURIAN_MIC_CHANGED);
        }
        break;
#endif

#if DURIAN_AIRMAX
    case APP_DURIAN_AVP_EVENT_DIGITAL_CROWN_SETTINGS:
        {
            APP_PRINT_INFO1("app_avp_bt_cback: is_clockwise %d", param->digital_crown_settings.is_clockwise);
            app_cfg_nv.is_clockwise = param->digital_crown_settings.is_clockwise;
        }
        break;
#endif

    case APP_DURIAN_AVP_EVENT_COMPACTNESS_TEST:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->compactness_test.bd_addr);
            if ((p_link != NULL) && (timer_idx_avp_compactness == 0))
            {
                memcpy(compactness_bd_addr, p_link->bd_addr, 6);
                if (app_cfg_const.disable_fake_earfit_verification)
                {
                    //Wait next version update, replace below flow
                    app_start_timer(&timer_idx_avp_compactness, "avp_compactness",
                                    app_durian_avp_timer_id, APP_TIMER_AVP_COMPACTNESS, 0, false,
                                    AVP_COMPACTNESS_MS);
                }
                else
                {
                    app_start_timer(&timer_idx_avp_compactness, "avp_compactness",
                                    app_durian_avp_timer_id, APP_TIMER_AVP_COMPACTNESS, 0, false,
                                    AVP_COMPACTNESS_MS);
                }

            }
        }
        break;

    case APP_DURIAN_AVP_EVENT_CLICK_SPEED:
        {
            durian_db.click_speed = param->click_speed_settings.speed;
            app_cfg_const.key_multi_click_interval = app_durian_key_multiclick_get(durian_db.click_speed);
            durian_db.click_speed_rec = true;
            app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_KEY_CLICK_SPEED);
        }
        break;

    case APP_DURIAN_AVP_EVENT_LONG_RESS_TIME:
        {
            durian_db.long_press_time = param->long_press_time_settings.time;
            app_cfg_const.key_long_press_interval = app_durian_key_long_press_get(durian_db.long_press_time);

            durian_db.long_press_time_rec = true;
            app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_KEY_LONG_PRESS_TIME);
        }
        break;

#if F_APP_TWO_GAIN_TABLE
    case APP_DURIAN_AVP_EVENT_AUDIO_SHARING:
        {
            T_APP_BR_LINK *p_link = app_link_find_br_link(param->audio_sharing.bd_addr);

            if (p_link != NULL)
            {
                p_link->audio_sharing = param->audio_sharing.is_enable;
                app_durian_sync_amplify_gain(p_link->bd_addr);
            }
        }
        break;
#endif

#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
    case APP_DURIAN_AVP_EVENT_SPATIAL_AUDIO:
        {
            if (app_cfg_const.mems_support)
            {
                T_APP_BR_LINK *p_link = app_link_find_br_link(param->spatial_audio.bd_addr);
                if (p_link != NULL)
                {
                    uint8_t attitude_ret = durian_atti_hdl(param->spatial_audio.bd_addr, param->spatial_audio.data,
                                                           param->spatial_audio.len);

                    APP_PRINT_INFO1("app_avp_bt_cback: attitude_ret %d", attitude_ret);

                    if (attitude_ret == 1)
                    {
                        app_sensor_mems_spatial_start(p_link->id);
                    }
                    else if (attitude_ret == 2)
                    {
                        app_sensor_mems_spatial_stop(p_link->id);
                    }
                }
            }
        }
        break;
#endif

    case APP_DURIAN_AVP_EVENT_VERSION_SYNC:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->version_sync.bd_addr);
            if (p_link != NULL)
            {
                T_VER_SYNC ver_sync = {0};
                ver_sync.bd_addr = param->version_sync.bd_addr;
                ver_sync.version = p_link->remote_device_vendor_version;
                durian_avp_version_sync(&ver_sync);
                app_durian_adp_batt_report();
            }
        }
        break;

    case APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_START:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->voice_recognition_start.bd_addr);
            if (p_link != NULL)
            {
                if (p_link->call_status == APP_CALL_IDLE)
                {
                    p_link->call_status = APP_VOICE_ACTIVATION_ONGOING;
                }

                app_hfp_update_call_status();

                if (p_link->id != active_hf_idx)
                {
                    break;
                }

                app_multi_a2dp_active_link_set(app_db.br_link[p_link->id].bd_addr);
                app_multi_pause_inactive_a2dp_link_stream(p_link->id, false);
                app_bond_set_priority(app_db.br_link[p_link->id].bd_addr);

                if (key_voice_reg_enable)
                {
                    p_link->audio_opus_status = AVP_VIOCE_RECOGNITION_ENCODE_START;

#if (F_APP_LISTENING_MODE_SUPPORT == 1)
                    app_durain_anc_siri_start_check();
#endif

                    audio_track_start(avp_opus_handle);
                    app_durian_avp_voice_recognition_encode_start(p_link->bd_addr);
                    key_voice_reg_enable = false;
                }
            }
        }
        break;

    case APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_ENCODE_START:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->voice_recognition_encode_start.bd_addr);
            if (p_link != NULL)
            {
                if (p_link->call_status == APP_CALL_IDLE)
                {
                    p_link->call_status = APP_VOICE_ACTIVATION_ONGOING;
                }

                app_hfp_update_call_status();

                if (p_link->id != active_hf_idx)
                {
                    break;
                }

                app_multi_a2dp_active_link_set(app_db.br_link[p_link->id].bd_addr);
                app_multi_pause_inactive_a2dp_link_stream(p_link->id, false);
                app_bond_set_priority(app_db.br_link[p_link->id].bd_addr);

                if (app_cfg_const.enable_vad == 1 && app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                {
                    vad_disable();
                }
                p_link->audio_opus_status = AVP_VIOCE_RECOGNITION_ENCODE_START;

#if (F_APP_LISTENING_MODE_SUPPORT == 1)
                app_durain_anc_siri_start_check();
#endif

                audio_track_start(avp_opus_handle);
            }
        }
        break;

    case APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_ENCODE_STOP:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->voice_recognition_encode_stop.bd_addr);
            if (p_link != NULL)
            {
                if (p_link->call_status == APP_VOICE_ACTIVATION_ONGOING)
                {
                    p_link->call_status = APP_CALL_IDLE;
                }

                app_hfp_update_call_status();

                if (p_link->id != active_hf_idx)
                {
                    break;
                }

                audio_track_stop(avp_opus_handle);
                p_link->audio_opus_status = AVP_VIOCE_RECOGNITION_ENCODE_STOP;
                app_durian_avp_voice_recognition_encode_stop(p_link->bd_addr);
            }

            if (app_cfg_const.enable_vad == 1 && app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                T_AUDIO_FORMAT_INFO        format_info;
                T_VAD_AGGRESSIVENESS_LEVEL aggressiveness_level;

                aggressiveness_level = VAD_AGGRESSIVENESS_LEVEL_LOW;
                format_info.type = AUDIO_FORMAT_TYPE_SBC;
                format_info.frame_num = 1;
                format_info.attr.sbc.sample_rate = 48000;
                format_info.attr.sbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_JOINT_STEREO;
                format_info.attr.sbc.block_length = 16;
                format_info.attr.sbc.subband_num = 8;
                format_info.attr.sbc.allocation_method = 0;
                format_info.attr.sbc.bitpool = 0x22;
                format_info.attr.sbc.chann_loaction = AUDIO_CHANNEL_LOCATION_FL | AUDIO_CHANNEL_LOCATION_FR;

                vad_enable(aggressiveness_level, format_info);
            }
        }
        break;

    case APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_STOP:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->voice_recognition_stop.bd_addr);
            if (p_link != NULL)
            {
                if (p_link->call_status == APP_VOICE_ACTIVATION_ONGOING)
                {
                    p_link->call_status = APP_CALL_IDLE;
                    app_hfp_update_call_status();
                }

                if (p_link->id != active_hf_idx)
                {
                    break;
                }

                if (p_link->audio_opus_status == AVP_VIOCE_RECOGNITION_ENCODE_START)
                {
                    audio_track_stop(avp_opus_handle);
                    app_durian_avp_voice_recognition_encode_stop(p_link->bd_addr);
                }

                p_link->audio_opus_status = AVP_VIOCE_RECOGNITION_STOP;

#if (F_APP_LISTENING_MODE_SUPPORT == 1)
                app_durain_anc_siri_stop_check();
#endif

            }
        }
        break;

    default:
        break;
    }

    APP_PRINT_INFO1("app_durian_avp_handle_event: event_type 0x%04x", event_type);
}

static void app_durian_avp_bt_cback(T_BT_AVP_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_AVP_EVENT_PARAM *param = event_buf;
    T_APP_BR_LINK *p_link;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        APP_PRINT_TRACE0("app_durian_avp_bt_cback: role is sec");
        return;
    }

    switch (event_type)
    {
    case BT_AVP_EVENT_CONN_CMPL:
        {
            uint8_t avp_is_conn = 0;

            p_link = app_link_find_br_link(param->conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                app_bt_policy_msg_prof_conn(p_link->bd_addr, AVP_PROFILE_MASK);
                app_start_timer(&timer_idx_att_connnect, "att_connect",
                                app_durian_avp_timer_id, APP_TIMER_ATT_CONNECT, 0, false,
                                AVP_ATT_CONNECT_MS);
                durian_db.click_speed_rec = false;
                durian_db.long_press_time_rec = false;
                durian_avp_id_set();
                avp_is_conn = 1;
                app_durian_sync_report_event_broadcast(EVENT_AVP_PROFILE_CONNECT, &avp_is_conn, 1);
            }
        }
        break;

    case BT_AVP_EVENT_DISCONN_CMPL:
        {
            p_link = app_link_find_br_link(param->disconn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                uint8_t avp_is_conn = 0;
                app_bt_policy_msg_prof_disconn(p_link->bd_addr, AVP_PROFILE_MASK, 0);
                app_durian_sync_report_event_broadcast(EVENT_AVP_PROFILE_CONNECT, &avp_is_conn, 1);
            }
        }
        break;

    case BT_AVP_EVENT_DATA_IND:
        {
            p_link = app_link_find_br_link(param->data_ind.bd_addr);
            if (p_link != NULL)
            {
                T_APP_DURIAN_AVP_EVENT_PARAM event_param;
                uint8_t   *data = param->data_ind.data;
                uint16_t   len = param->data_ind.len;

                /*sync packet*/
                if ((data[0] == 0x00) && (data[1] == 0x00) &&
                    (data[2] == 0x04) && (data[3] == 0x00))
                {
                    uint8_t durian_pro[18] = {0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00,
                                              0x02, 0x00, 0x05, 0x00, 0x49, 0x4E, 0x05, 0x00,
                                              0xA5, 0x4F
                                             };

                    bt_avp_data_send(p_link->bd_addr, durian_pro, sizeof(durian_pro), false);

                    memcpy(event_param.version_sync.bd_addr, p_link->bd_addr, 6);
                    app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_VERSION_SYNC, &event_param, sizeof(event_param));
                }
                /*iphone set name*/
                else if ((data[0] == 0x04) && (data[1] == 0x00) &&
                         (data[2] == 0x04) && (data[3] == 0x00) &&
                         (data[4] == 0x1A) && (data[5] == 0x00) &&
                         (data[6] == 0x01))
                {
                    event_param.set_name.len = data[7] + (data[8] << 8);
                    event_param.set_name.data = &data[9];
                    memcpy(event_param.set_name.bd_addr, p_link->bd_addr, 6);
                    app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_SET_NAME, &event_param, sizeof(event_param));
                }
                /*pod1&2 click settings*/
                else if ((data[0] == 0x04) && (data[1] == 0x00) &&
                         (data[2] == 0x04) && (data[3] == 0x00) &&
                         (data[4] == 0x09) && (data[5] == 0x00) &&
                         (data[6] == 0x0C))
                {
                    memcpy(event_param.control_settings.bd_addr, p_link->bd_addr, 6);
                    event_param.control_settings.right_ear_control = (T_APP_DURIAN_AVP_CONTROL)data[7];
                    event_param.control_settings.left_ear_control = (T_APP_DURIAN_AVP_CONTROL)data[8];
                    app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_CONTROL_SETTINGS, &event_param,
                                                sizeof(event_param));
                }
                /*in_ear detection*/
                else if ((data[0] == 0x04) && (data[1] == 0x00) &&
                         (data[2] == 0x04) && (data[3] == 0x00) &&
                         (data[4] == 0x09) && (data[5] == 0x00) &&
                         (data[6] == 0x0A))
                {
                    memcpy(event_param.in_ear_detection.bd_addr, p_link->bd_addr, 6);

                    if (data[7] == 0x01)
                    {
                        event_param.in_ear_detection.open = false;
                    }
                    else if (data[7] == 0x02)
                    {
                        event_param.in_ear_detection.open = true;
                    }

                    app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_IN_EAR_DETECTION, &event_param,
                                                sizeof(event_param));
                }
                /*mic settings*/
                else if ((data[0] == 0x04) && (data[1] == 0x00) &&
                         (data[2] == 0x04) && (data[3] == 0x00) &&
                         (data[4] == 0x09) && (data[5] == 0x00) &&
                         (data[6] == 0x01))
                {
                    memcpy(event_param.mic_settings.bd_addr, p_link->bd_addr, 6);
                    event_param.mic_settings.setting = (T_APP_DURIAN_AVP_MIC)data[7];
                    app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_MIC_SETTINGS, &event_param, sizeof(event_param));
                }
                /*anc settings*/
                else if ((data[0] == 0x04) && (data[1] == 0x00) &&
                         (data[2] == 0x04) && (data[3] == 0x00) &&
                         (data[4] == 0x09) && (data[5] == 0x00) &&
                         (data[6] == 0x0D))
                {
                    memcpy(event_param.anc_settings.bd_addr, p_link->bd_addr, 6);
                    if (data[7] == 0x01)
                    {
                        event_param.anc_settings.setting = APP_DURIAN_AVP_ANC_CLOSE;
                    }
                    else if (data[7] == 0x02)
                    {
                        event_param.anc_settings.setting = APP_DURIAN_AVP_ANC_OPEN;
                    }
                    else if (data[7] == 0x03)
                    {
                        event_param.anc_settings.setting = APP_DURIAN_AVP_ANC_TRANSPARENCY_MODE;
                    }

                    app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_ANC_SETTINGS, &event_param, sizeof(event_param));
                }

                /*pro click settings: anc or voice recg*/
                else if ((data[0] == 0x04) && (data[1] == 0x00) &&
                         (data[2] == 0x04) && (data[3] == 0x00) &&
                         (data[4] == 0x09) && (data[5] == 0x00) &&
                         (data[6] == 0x16))
                {
                    if (data[7] == 0x05)
                    {
                        event_param.click_settings.right_ear_longpress_control = APP_DURIAN_AVP_CONTROL_ANC;
                    }
                    else if (data[7] == 0x01)
                    {
                        event_param.click_settings.right_ear_longpress_control = APP_DURIAN_AVP_CONTROL_VOICE_RECOGNITION;
                    }
                    else if (data[7] == 0x06)
                    {
                        event_param.click_settings.right_ear_longpress_control = APP_DURIAN_AVP_CONTROL_VOL_UP;
                    }
                    else if (data[7] == 0x07)
                    {
                        event_param.click_settings.right_ear_longpress_control = APP_DURIAN_AVP_CONTROL_VOL_DOWN;
                    }

                    if (data[8] == 0x05)
                    {
                        event_param.click_settings.left_ear_longpress_control = APP_DURIAN_AVP_CONTROL_ANC;
                    }
                    else if (data[8] == 0x01)
                    {
                        event_param.click_settings.left_ear_longpress_control = APP_DURIAN_AVP_CONTROL_VOICE_RECOGNITION;
                    }
                    else if (data[8] == 0x06)
                    {
                        event_param.click_settings.left_ear_longpress_control = APP_DURIAN_AVP_CONTROL_VOL_UP;
                    }
                    else if (data[8] == 0x07)
                    {
                        event_param.click_settings.left_ear_longpress_control = APP_DURIAN_AVP_CONTROL_VOL_DOWN;
                    }

                    memcpy(event_param.click_settings.bd_addr, p_link->bd_addr, 6);
                    app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_CLICK_SETTINGS, &event_param, sizeof(event_param));
                }
                /*pro click settings: anc apt cycle*/
                else if ((data[0] == 0x04) && (data[1] == 0x00) &&
                         (data[2] == 0x04) && (data[3] == 0x00) &&
                         (data[4] == 0x09) && (data[5] == 0x00) &&
                         (data[6] == 0x1A))
                {
                    event_param.cycle_settings.setting = data[7];

                    memcpy(event_param.cycle_settings.bd_addr, p_link->bd_addr, 6);
                    app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_ANC_APT_CYCLE, &event_param, sizeof(event_param));
                }
                /*compactness test*/
                else if ((data[0] == 0x04) && (data[1] == 0x00) &&
                         (data[2] == 0x04) && (data[3] == 0x00) &&
                         (data[4] == 0x21) && (data[5] == 0x00))
                {
                    memcpy(event_param.compactness_test.bd_addr, p_link->bd_addr, 6);
                    app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_COMPACTNESS_TEST, &event_param,
                                                sizeof(event_param));
                }
                /*click speed*/
                else if ((data[0] == 0x04) && (data[1] == 0x00) &&
                         (data[2] == 0x04) && (data[3] == 0x00) &&
                         (data[4] == 0x09) && (data[5] == 0x00) &&
                         (data[6] == 0x17))
                {
                    memcpy(event_param.click_speed_settings.bd_addr, p_link->bd_addr, 6);
                    event_param.click_speed_settings.speed = (T_APP_DURIAN_AVP_CLICK_SPEED)data[7];
                    app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_CLICK_SPEED, &event_param, sizeof(event_param));
                }
                /*long press time*/
                else if ((data[0] == 0x04) && (data[1] == 0x00) &&
                         (data[2] == 0x04) && (data[3] == 0x00) &&
                         (data[4] == 0x09) && (data[5] == 0x00) &&
                         (data[6] == 0x18))
                {
                    memcpy(event_param.long_press_time_settings.bd_addr, p_link->bd_addr, 6);
                    event_param.long_press_time_settings.time = (T_APP_DURIAN_AVP_LONG_PRESS_TIME)data[7];
                    app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_LONG_RESS_TIME, &event_param, sizeof(event_param));
                }
                /*one bud in ear to open anc*/
                else if ((data[0] == 0x04) && (data[1] == 0x00) &&
                         (data[2] == 0x04) && (data[3] == 0x00) &&
                         (data[4] == 0x09) && (data[5] == 0x00) &&
                         (data[6] == 0x1B))
                {
                    memcpy(event_param.one_bud_anc.bd_addr, p_link->bd_addr, 6);

                    if (data[7] == 0x01)
                    {
                        event_param.one_bud_anc.is_open = true;
                    }
                    else
                    {
                        event_param.one_bud_anc.is_open = false;
                    }

                    app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_ONE_BUD_ANC, &event_param, sizeof(event_param));
                }
                /*air max*/
                else if ((data[0] == 0x04) && (data[1] == 0x00) &&
                         (data[2] == 0x04) && (data[3] == 0x00) &&
                         (data[4] == 0x09) && (data[5] == 0x00) &&
                         (data[6] == 0x1C))
                {
                    memcpy(event_param.digital_crown_settings.bd_addr, p_link->bd_addr, 6);

                    if (data[7] == 0x01)
                    {
                        event_param.digital_crown_settings.is_clockwise = true;
                    }
                    else if (data[7] == 0x02)
                    {
                        event_param.digital_crown_settings.is_clockwise = false;
                    }

                    app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_DIGITAL_CROWN_SETTINGS, &event_param,
                                                sizeof(event_param));
                }
                /*share audio*/
                else if ((data[0] == 0x04) && (data[1] == 0x00) &&
                         (data[2] == 0x04) && (data[3] == 0x00) &&
                         (data[4] == 0x1f) && (data[5] == 0x00))
                {
                    memcpy(event_param.audio_sharing.bd_addr, p_link->bd_addr, 6);

                    if (data[6] == data[7])
                    {
                        event_param.audio_sharing.is_enable = true;
                    }
                    else
                    {
                        event_param.audio_sharing.is_enable = false;
                    }

                    app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_AUDIO_SHARING, &event_param, sizeof(event_param));
                }
                /*spatial audio*/
                else if ((data[0] == 0x04) && (data[1] == 0x00) &&
                         (data[2] == 0x04) && (data[3] == 0x00) &&
                         (data[4] == 0x17) && (data[5] == 0x00))
                {
                    memcpy(event_param.spatial_audio.bd_addr, p_link->bd_addr, 6);
                    event_param.spatial_audio.data = &data[0];
                    event_param.spatial_audio.len = len;
                    app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_SPATIAL_AUDIO, &event_param, sizeof(event_param));
                }
                /*volume control setting*/
                else if ((data[0] == 0x04) && (data[1] == 0x00) &&
                         (data[2] == 0x04) && (data[3] == 0x00) &&
                         (data[4] == 0x09) && (data[5] == 0x00) &&
                         (data[6] == 0x25))
                {
                    memcpy(event_param.volume_control_settings.bd_addr, p_link->bd_addr, 6);

                    if (data[7] == 0x01)
                    {
                        event_param.volume_control_settings.is_enable = true;
                    }
                    else if (data[7] == 0x02)
                    {
                        event_param.volume_control_settings.is_enable = false;
                    }

                    app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_VOLUME_CONTROL_SETTINGS, &event_param,
                                                sizeof(event_param));
                }
                /*discon event*/
                else if ((data[0] == 0x02) && (data[1] == 0x00) &&
                         (data[2] == 0x04) && (data[3] == 0x00) &&
                         (data[4] == 0x00) && (data[5] == 0x00))
                {
                    uint8_t disconn_ack[6] = {0x03, 0x00, 0x04, 0x00, 0x00, 0x00};
                    bt_avp_data_send(p_link->bd_addr, disconn_ack, sizeof(disconn_ack), false);
                }
            }
        }
        break;

    default:
        break;
    }
}

void app_durian_att_bt_cback(uint8_t *bd_addr, T_BT_ATT_MSG_TYPE msg_type, void *p_msg)
{
    T_APP_BR_LINK *p_link;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }

    switch (msg_type)
    {
    case BT_ATT_MSG_CONN_CMPL:
        {
            p_link = app_link_find_br_link(bd_addr);
            if (p_link != NULL)
            {
                app_bt_policy_msg_prof_conn(p_link->bd_addr, GATT_PROFILE_MASK);
                p_link->audio_opus_status = AVP_VIOCE_RECOGNITION_CREATE;  //opus audio_path created

                if (avp_opus_handle == NULL)
                {
                    T_AUDIO_FORMAT_INFO opus_info;

                    durian_avp_opus_info(&opus_info, AUDIO_FORMAT_TYPE_OPUS);

                    avp_opus_handle = audio_track_create(AUDIO_STREAM_TYPE_RECORD,
                                                         AUDIO_STREAM_MODE_NORMAL,
                                                         AUDIO_STREAM_USAGE_LOCAL,
                                                         opus_info,
                                                         0,
                                                         app_dsp_cfg_vol.record_volume_default,
                                                         AUDIO_DEVICE_IN_MIC,
                                                         NULL,
                                                         app_durian_avp_track_async_read);
                }
            }
        }
        break;

    case BT_ATT_MSG_DISCONN_CMPL:
        {
            p_link = app_link_find_br_link(bd_addr);
            if (p_link != NULL)
            {
                if (app_link_get_b2s_link_num_with_profile(GATT_PROFILE_MASK) == 1)
                {
                    audio_track_release(avp_opus_handle);
                    avp_opus_handle = NULL;
                }

                p_link->audio_opus_status = AVP_VIOCE_RECOGNITION_IDLE; //opus audio_path released
                app_bt_policy_msg_prof_disconn(p_link->bd_addr, GATT_PROFILE_MASK, 0);
                app_start_timer(&timer_idx_att_connnect, "att_connect",
                                app_durian_avp_timer_id, APP_TIMER_ATT_CONNECT, 0, false,
                                AVP_ATT_CONNECT_MS);
            }
        }
        break;

    default:
        break;
    }
}

bool app_durian_gatt_cback(uint16_t conn_handle, uint8_t service_id, uint16_t cid, uint8_t *data,
                           uint16_t length)
{
    uint8_t bd_addr[6];
    T_APP_BR_LINK *p_link = NULL;
    uint8_t addr_type;
    T_APP_DURIAN_AVP_EVENT_PARAM event_param;

    if (!gap_chann_get_addr(conn_handle, bd_addr, &addr_type))
    {
        return false;
    }

    p_link = app_link_find_br_link(bd_addr);

    if (p_link != NULL)
    {
        if (length == 0x05)
        {
            //voice recognition start
            if ((data[0] == 0x01) &&
                (data[1] == 0x00) && (data[2] == 0x04) &&
                (data[3] == 0x01) && (data[4] == 0x07))
            {
                memcpy(event_param.voice_recognition_start.bd_addr, bd_addr, 6);
                app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_START, &event_param,
                                            sizeof(event_param));
            }
            //voice recognition stop
            else if ((data[0] == 0x01) &&
                     (data[1] == 0x00) && (data[2] == 0x04) &&
                     (data[3] == 0x01) && (data[4] == 0x00))
            {
                memcpy(event_param.voice_recognition_stop.bd_addr, bd_addr, 6);
                app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_STOP, &event_param,
                                            sizeof(event_param));
            }
        }
        else if (length == 0x02)
        {
            //encode start
            if ((data[0] == 0x01) && (data[1] == 0x00))
            {
                memcpy(event_param.voice_recognition_encode_start.bd_addr, bd_addr, 6);
                app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_ENCODE_START, &event_param,
                                            sizeof(event_param));
            }
        }
        else if (length == 0x03)
        {
            //encode stop
            if ((data[0] == 0x01) &&
                (data[1] == 0x00) && (data[2] == 0x00))
            {
                siri_seq = 0;

                memcpy(event_param.voice_recognition_encode_stop.bd_addr, bd_addr, 6);
                app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_VOICE_RECOGNITION_ENCODE_STOP, &event_param,
                                            sizeof(event_param));
            }
        }
        else if (length == 0x01)
        {
            if (data[0] == 0)
            {
                memcpy(event_param.auto_apt_settings.bd_addr, bd_addr, 6);
                event_param.auto_apt_settings.enable = false;
                app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_AUTO_APT_SETTINGS, &event_param,
                                            sizeof(event_param));
            }
            else if (data[0] == 0x01)
            {
                memcpy(event_param.auto_apt_settings.bd_addr, bd_addr, 6);
                event_param.auto_apt_settings.enable = true;
                app_durian_avp_handle_event(APP_DURIAN_AVP_EVENT_AUTO_APT_SETTINGS, &event_param,
                                            sizeof(event_param));
            }
        }

        return true;
    }

    return false;
}

static void app_durian_avp_bt_mgr_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_A2DP_DISCONN_CMPL:
        {
#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
            T_APP_BR_LINK *p_link;
            p_link = app_link_find_br_link(param->a2dp_disconn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                app_sensor_mems_spatial_stop(p_link->id);
            }
#endif
        }
        break;

    case BT_EVENT_HFP_CALL_STATUS:
        {
#if (F_APP_LISTENING_MODE_SUPPORT == 1)
            if (param->hfp_call_status.curr_status == BT_HFP_CALL_IDLE)
            {
                app_durian_anc_call_stop_check();
            }
            else
            {
                app_durian_anc_call_start_check();
            }
#endif
        }
        break;

    case BT_EVENT_A2DP_STREAM_STOP:
        {
            T_APP_BR_LINK *p_link;
            p_link = app_link_find_br_link(param->a2dp_stream_stop.bd_addr);

            if (p_link != NULL)
            {
                durian_avp_stream_stop(param->a2dp_stream_stop.bd_addr);
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_START_IND:
        {
            T_APP_BR_LINK *p_link;
            p_link = app_link_find_br_link(param->a2dp_stream_start_ind.bd_addr);

            if (p_link != NULL)
            {
                durian_avp_stream_start(param->a2dp_stream_start_ind.bd_addr);
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_DATA_IND:
        {
#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
            if (app_cfg_const.mems_support)
            {
                T_APP_BR_LINK *p_link;
                p_link = app_link_find_br_link(param->a2dp_stream_data_ind.bd_addr);

                if (p_link != NULL)
                {
                    if (p_link->mems_is_start)
                    {
                        durian_atti_a2dp_ts(param->a2dp_stream_data_ind.timestamp);
                    }
                }
            }
#endif
        }
        break;

    case BT_EVENT_DID_ATTR_INFO:
        {
            T_APP_BR_LINK *p_link = NULL;

            p_link = app_link_find_br_link(param->did_attr_info.bd_addr);

            if (p_link != NULL)
            {
                if (param->did_attr_info.vendor_id == IOS_VENDOR_ID)
                {
                    p_link->remote_device_vendor_id = APP_REMOTE_DEVICE_IOS;
                    p_link->remote_device_vendor_version = param->did_attr_info.version;
                }
                else
                {
                    p_link->remote_device_vendor_id = APP_REMOTE_DEVICE_OTHERS;
                }

                T_DID_SYNC did_sync = {0};
                did_sync.version = param->did_attr_info.version;
                did_sync.bd_addr = param->did_attr_info.bd_addr;

                durian_avp_did_sync(&did_sync);
            }

        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        if (event_type != BT_EVENT_A2DP_STREAM_DATA_IND)
        {
            APP_PRINT_INFO1("app_avp_bt_mgr_cback: event_type 0x%04x", event_type);
        }
    }
}

static void app_durian_avp_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_avp_timeout_cb: timer_evt 0x%02x, param %d", timer_evt, param);

    switch (timer_evt)
    {
    case APP_TIMER_ATT_CONNECT:
        {
            app_stop_timer(&timer_idx_att_connnect);
            //connect att no matter opus status
            {
                for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
                {
                    if (app_db.br_link[i].connected_profile & AVP_PROFILE_MASK)
                    {
                        APP_PRINT_TRACE0("app_avp_timeout_cb: bt_att_connect_req");
                        bt_att_connect_req(app_db.br_link[i].bd_addr);
                    }
                }
            }

#if(F_APP_LISTENING_MODE_SUPPORT == 1)
            app_durian_anc_report(durian_db.anc_cur_setting);
#endif
        }
        break;

    case APP_TIMER_AVP_COMPACTNESS:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_durian_avp_compactness_state_machine(true);
            }
            app_stop_timer(&timer_idx_avp_compactness);

        }
        break;

    case APP_TIMER_VOICE_CALL_VP_DELAY:
        {
            app_stop_timer(&timer_idx_voice_call_vp_delay);
            app_audio_tone_type_play(TONE_HF_CALL_VOICE_DIAL, false, true);
        }
        break;

    default:
        break;
    }
}

void app_durian_avp_init(void)
{
    if (durian_cfg.one_trig_need)
    {
        bt_avp_init(app_durian_avp_bt_cback);
        bt_att_init(app_durian_att_bt_cback);
        avp_att_add_service(app_durian_gatt_cback);
    }
    bt_mgr_cback_register(app_durian_avp_bt_mgr_cback);
    app_timer_reg_cb(app_durian_avp_timeout_cb, &app_durian_avp_timer_id);
}
#endif
