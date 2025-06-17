/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if XM_XIAOAI_FEATURE_SUPPORT
#include "app_xiaoai_at.h"
#include "string.h"
#include "stdio.h"
#include "bt_hfp.h"
#include "app_cfg.h"
#include "app_xm_xiaoai_ble_adv.h"
#include "trace.h"

/**
 * \brief conn AT commad
 */
typedef struct _XIAOAI_XM_CONN_AT_CMD
{
    uint8_t headset_state;
    uint8_t battery_state[3];
    uint8_t adv_cnt;
} T_XIAOAI_XM_CONN_AT_CMD;

T_XIAOAI_XM_CONN_AT_CMD xiaoai_xm_conn_at_cmd;

#ifdef XM_XIAOAI_EXTEND_AT_CMD
/**
 * \brief extend AT commad
 */
typedef struct _XIAOAI_XM_EXTEND_AT_CMD
{
    //LTV PART 1
    uint8_t ltv_length1;                            //byte 0

    uint8_t ltv_type1;                              //byte 1

    uint8_t upgrade_support: 1;                     //byte 2
    uint8_t find_headset_support: 1;
    uint8_t reserved: 5;
    uint8_t next_flag_1: 1;

    uint8_t denoise_support: 1;                     //byte 3
    uint8_t double_click_headset_support_l: 1;
    uint8_t double_click_headset_support_r: 1;
    uint8_t voiceprint_recognition_support: 1;
    uint8_t game_mode_support_1: 1;
    uint8_t voice_command_training_support: 1;
    uint8_t equalizer_support: 1;
    uint8_t game_mode_support_2: 1;

    //LTV PART 2
    uint8_t ltv_length2;                            //byte 4
    uint8_t ltv_type2;
    uint8_t version;
    uint8_t adv_profile_type;
    uint8_t major_id;
    uint8_t minor_id;
    uint8_t rom_version[4];                         //byte 10~13
    uint8_t denoise;                                //byte 14
    uint8_t double_click_headset_l;
    uint8_t double_click_headset_r;
    uint8_t voice_wake_up;
    uint8_t voiceprint_recognition;

    uint8_t voice_cmd_training_xiaoai: 1;           //byte 19
    uint8_t voice_cmd_training_last_song: 1;
    uint8_t voice_cmd_training_next_song: 1;
    uint8_t voice_cmd_training_volume_up: 1;
    uint8_t voice_cmd_training_volume_down: 1;
    uint8_t voice_cmd_training_hang_up: 1;
    uint8_t voice_cmd_training_call_answer: 1;
    uint8_t voice_cmd_training_reserve: 1;

    uint8_t voice_cmd_training_2;

    uint8_t equalizer;
    uint8_t game_mode;                              //byte 22
} T_XIAOAI_XM_EXTEND_AT_CMD;

T_XIAOAI_XM_EXTEND_AT_CMD xiaoai_xm_extend_at_cmd;

void xiaoai_xm_extend_at_cmd_init(uint8_t *rom_version)
{
    //LTV PART 1
    xiaoai_xm_extend_at_cmd.ltv_length1 = 3;
    xiaoai_xm_extend_at_cmd.ltv_type1 = 1;
    xiaoai_xm_extend_at_cmd.upgrade_support = 0;
    xiaoai_xm_extend_at_cmd.find_headset_support = 0;
    xiaoai_xm_extend_at_cmd.reserved = 0;
    xiaoai_xm_extend_at_cmd.next_flag_1 = 0;

    xiaoai_xm_extend_at_cmd.denoise_support = 0;
    xiaoai_xm_extend_at_cmd.double_click_headset_support_l = 1;
    xiaoai_xm_extend_at_cmd.double_click_headset_support_r = 0;
    xiaoai_xm_extend_at_cmd.voiceprint_recognition_support = 0;
    xiaoai_xm_extend_at_cmd.game_mode_support_1 = 0;
    xiaoai_xm_extend_at_cmd.voice_command_training_support = 0;
    xiaoai_xm_extend_at_cmd.equalizer_support = 0;
    xiaoai_xm_extend_at_cmd.game_mode_support_2 = 0;

    //LTV PART 2
    xiaoai_xm_extend_at_cmd.ltv_length2 = 18; //unchangable
    xiaoai_xm_extend_at_cmd.ltv_type2 = 2; //2: basic feature type
    xiaoai_xm_extend_at_cmd.version = 1;
    xiaoai_xm_extend_at_cmd.adv_profile_type = FAST_CONNECT_PROFILE;
    xiaoai_xm_extend_at_cmd.major_id = extend_app_cfg_const.xiaoai_major_id;
    xiaoai_xm_extend_at_cmd.minor_id = extend_app_cfg_const.xiaoai_minor_id;
    memcpy(xiaoai_xm_extend_at_cmd.rom_version, rom_version, 4);
    xiaoai_xm_extend_at_cmd.denoise = DENOISE_OFF;
    xiaoai_xm_extend_at_cmd.double_click_headset_l = VOICE_ASSISTANT;
    xiaoai_xm_extend_at_cmd.double_click_headset_r = VOICE_ASSISTANT;
    xiaoai_xm_extend_at_cmd.voice_wake_up = 0;
    xiaoai_xm_extend_at_cmd.voiceprint_recognition = 0;

    xiaoai_xm_extend_at_cmd.voice_cmd_training_xiaoai = 0;
    xiaoai_xm_extend_at_cmd.voice_cmd_training_last_song = 0;
    xiaoai_xm_extend_at_cmd.voice_cmd_training_next_song = 0;
    xiaoai_xm_extend_at_cmd.voice_cmd_training_volume_up = 0;
    xiaoai_xm_extend_at_cmd.voice_cmd_training_volume_down = 0;
    xiaoai_xm_extend_at_cmd.voice_cmd_training_hang_up = 0;
    xiaoai_xm_extend_at_cmd.voice_cmd_training_call_answer = 0;
    xiaoai_xm_extend_at_cmd.voice_cmd_training_reserve = 0;

    xiaoai_xm_extend_at_cmd.voice_cmd_training_2 = 0;
    xiaoai_xm_extend_at_cmd.equalizer = 0;
    xiaoai_xm_extend_at_cmd.game_mode = 0;
}

bool xiaoai_send_extend_at_cmd(uint8_t *bd_addr)
{
    APP_PRINT_INFO0("xiaoai_send_extend_at_cmd");
    uint8_t rom_version[4] = {1, 3, 5, 7};
    xiaoai_xm_extend_at_cmd_init(rom_version);
    uint8_t *res = (uint8_t *)&xiaoai_xm_extend_at_cmd;

    char str_dest[70] = "AT+XIAOMI=1,2,1,1";

    for (int i = 0; i < sizeof(xiaoai_xm_extend_at_cmd); i++)
    {
        char buf[5];
        sprintf(buf, ",%d", *(res + i));
        strcat(str_dest, buf);
    }
    return bt_hfp_send_vnd_at_cmd_req(bd_addr, str_dest);
}
#endif // XM_XIAOAI_EXTEND_AT_CMD

void xiaoai_xm_conn_at_cmd_init()
{
    xiaoai_xm_conn_at_cmd.headset_state = app_xm_xiaoai_get_headset_state();
    app_xm_xiaoai_get_battery_state(xiaoai_xm_conn_at_cmd.battery_state);
    xiaoai_xm_conn_at_cmd.adv_cnt = app_xm_xiaoai_get_adv_count();
}

bool xiaoai_send_conn_at_cmd(uint8_t *bd_addr)
{
    APP_PRINT_INFO0("xiaoai_send_conn_at_cmd");
    xiaoai_xm_conn_at_cmd_init();

    uint8_t *res = (uint8_t *)&xiaoai_xm_conn_at_cmd;

    char str_dest[35] = "AT+XIAOMI=1,1";
    for (int i = 0; i < sizeof(xiaoai_xm_conn_at_cmd); i++)
    {
        char buf[5];
        sprintf(buf, ",%d", *(res + i));
        strcat(str_dest, buf);
    }
    return bt_hfp_send_vnd_at_cmd_req(bd_addr, str_dest);
}
#endif
