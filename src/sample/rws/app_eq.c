/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <stdlib.h>
#include <string.h>
#include "stdlib_corecrt.h"
#include "os_mem.h"
#include "trace.h"
#include "eq.h"
#include "bt_a2dp.h"
#include "eq_utils.h"
#include "app_main.h"
#include "app_link_util.h"
#include "app_cmd.h"
#include "app_report.h"
#include "app_eq.h"
#include "app_cfg.h"
#include "app_dsp_cfg.h"
#include "app_audio_policy.h"
#include "app_bt_policy_api.h"
#include "app_anc.h"
#include "app_a2dp.h"
#include "app_ipc.h"

#if F_APP_LEA_SUPPORT
#include "app_lea_unicast_audio.h"
#include "app_lea_ccp.h"
#endif

#if F_APP_LINEIN_SUPPORT
#include "app_line_in.h"
#endif

#if F_APP_USER_EQ_SUPPORT
#include "errno.h"
#endif

#if F_APP_USB_AUDIO_SUPPORT
#include "app_usb_audio.h"
#endif

#if F_APP_VOICE_SPK_EQ_SUPPORT
#include "app_hfp.h"
#include "eq_ext_ftl.h"
#endif

#if F_APP_APT_SUPPORT
#include "audio_passthrough.h"
#endif

#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
#include "app_multilink_customer.h"
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
#include "app_dongle_record.h"
#endif

// for EQ setting
#define BOTH_SIDE_ADJUST        0x02

#define LO_WORD(x)  ((uint8_t)(x))
#define HI_WORD(x)  ((uint8_t)((x & 0xFF00) >> 8))

#if F_APP_USER_EQ_SUPPORT
#define USER_EQ_VERSION             0x01
#define USER_EQ_NUM_2M_FLASH_SIZE   6
#define USER_EQ_NUM_4M_FLASH_SIZE   20
#define MAX_TOTLA_USER_EQ_NUM       20
#define MAX_RELAY_SIZE              250

#if F_APP_ERWS_SUPPORT
uint16_t spk_user_eq_idx_waiting_sync;

#if F_APP_APT_SUPPORT
uint16_t mic_user_eq_idx_waiting_sync;
#endif

#endif  /* F_APP_ERWS_SUPPORT */
#endif  /* F_APP_USER_EQ_SUPPORT */

typedef enum
{
    SAVE_EQ_FIELD_ONLY_APPLY      = 0x0,
    SAVE_EQ_FIELD_APPLY_AND_SAVE  = 0x1,
    SAVE_EQ_FIELD_ONLY_SAVE       = 0x2,
} T_APP_EQ_SAVE_EQ_FIELD;

bool app_eq_get_link_info(uint8_t cmd_path, uint8_t app_idx,
                          T_AUDIO_EQ_REPORT_DATA *eq_report_data);
bool app_eq_report_eq_frame(T_AUDIO_EQ_REPORT_DATA *eq_report_data);
static bool app_eq_report_get_eq_extend_info(T_AUDIO_EQ_INFO *p_eq_info);
static uint16_t app_eq_dsp_param_get(T_EQ_TYPE eq_type, T_EQ_STREAM_TYPE stream_type,
                                     uint8_t eq_mode, uint8_t index, uint8_t *data,
                                     bool is_eq_payload);

#if F_APP_USER_EQ_SUPPORT
uint16_t app_eq_get_user_eq_size(void);
void app_eq_check_user_eq(void);

#if F_APP_ERWS_SUPPORT
bool app_eq_relay_sync_user_eq_msg(T_EQ_TYPE eq_type, uint8_t eq_idx,
                                   T_EQ_SYNC_ACTION sync_eq_action, uint16_t offset, uint8_t *eq_data, uint32_t len);
#endif

#endif

static void app_eq_factory_reset_cback(uint32_t event, void *msg)
{
    switch (event)
    {
    case APP_DEVICE_IPC_EVT_FACTORY_RESET:
        {
#if F_APP_USER_EQ_SUPPORT
            app_eq_reset_all_user_eq();
#endif
        }
        break;

    default:
        break;
    }
}

void app_eq_init(void)
{
    uint16_t len = 0;
    app_db.max_eq_len = 0;

    if (eq_utils_init())
    {
        len = EQ_MAX_SIZE;  // 106

        app_db.max_eq_len = len;

#if F_APP_EXT_MIC_SWITCH_SUPPORT
        static bool eq_ipc_subscribe_flg = false;

        if (eq_ipc_subscribe_flg == false)
        {
            app_ipc_subscribe(APP_DEVICE_IPC_TOPIC, app_eq_factory_reset_cback);
            eq_ipc_subscribe_flg = true;
        }
#else
        app_ipc_subscribe(APP_DEVICE_IPC_TOPIC, app_eq_factory_reset_cback);
#endif

        app_eq_idx_check_accord_mode();

#if F_APP_USER_EQ_SUPPORT
        T_EQ_USER_EQ_HEADER eq_header;

        if (eq_utils_load_eq_from_ftl(0, (uint8_t *)&eq_header, sizeof(T_EQ_USER_EQ_HEADER)) == ENOF)
        {
            app_eq_reset_all_user_eq();
        }
        else
        {
            if (eq_header.user_eq_version != USER_EQ_VERSION)
            {
                app_eq_reset_all_user_eq();
            }
            else
            {
                app_eq_check_user_eq();
            }
        }
#endif
    }
}

void app_eq_deinit(void)
{
    app_db.max_eq_len = 0;
    eq_utils_deinit();
}

#if F_APP_LEA_SUPPORT
static void app_eq_media_eq_set(T_AUDIO_EFFECT_INSTANCE eq_instance, uint8_t *eq_buf,
                                uint16_t eq_len)
{
    if (mtc_get_btmode() == MULTI_PRO_BT_BREDR && eq_instance != NULL)
    {
        eq_set(eq_instance, eq_buf, eq_len);
    }
    else if (mtc_get_btmode() != MULTI_PRO_BT_BREDR && app_lea_uca_get_eq_instance() != NULL)
    {
        eq_set(app_lea_uca_get_eq_instance(), eq_buf, eq_len);
    }
}

void app_eq_media_eq_enable(T_EQ_ENABLE_INFO *enable_info)
{
    if (mtc_get_btmode() == MULTI_PRO_BT_BREDR && enable_info->instance != NULL)
    {
        app_eq_audio_eq_enable(&enable_info->instance, &enable_info->is_enable);
    }
    else if (mtc_get_btmode() != MULTI_PRO_BT_BREDR && app_lea_uca_get_eq_instance() != NULL)
    {
        app_eq_audio_eq_enable(app_lea_uca_p_eq_instance(), app_lea_uca_get_eq_abled());
    }
}
#endif

void app_eq_enable_info_get(uint8_t eq_mode, T_EQ_ENABLE_INFO *enable_info)
{
    T_APP_BR_LINK *link = NULL;

    memset(enable_info, 0, sizeof(T_EQ_ENABLE_INFO));

    if (app_db.sw_eq_type == SPK_SW_EQ)
    {
#if F_APP_VOICE_SPK_EQ_SUPPORT
        if (eq_mode == VOICE_SPK_MODE)
        {
            link = &app_db.br_link[app_hfp_get_active_idx()];
            enable_info->instance = link->voice_spk_eq_instance;
        }
        else
#endif
        {
            link = &app_db.br_link[app_a2dp_get_active_idx()];
            enable_info->instance = link->eq_instance;
            enable_info->is_enable = link->audio_eq_enabled;
        }

        enable_info->idx = app_cfg_nv.eq_idx;
    }
    else
    {
#if F_APP_VOICE_MIC_EQ_SUPPORT
        if (eq_mode == VOICE_MIC_MODE)
        {
            link = &app_db.br_link[app_hfp_get_active_idx()];
            enable_info->instance = link->voice_mic_eq_instance;
            enable_info->idx = app_cfg_nv.eq_idx;

#if F_APP_GAMING_DONGLE_SUPPORT
            if (enable_info->instance == NULL && app_dongle_get_record_state())
            {
                enable_info->instance = (T_AUDIO_EFFECT_INSTANCE)app_dongle_record_get_eq_instance();
            }
#endif
        }
        else
#endif
        {
#if F_APP_APT_SUPPORT
            enable_info->instance = app_db.apt_eq_instance;
            enable_info->idx = app_cfg_nv.apt_eq_idx;
#endif
        }
    }

    APP_PRINT_INFO3("app_eq_enable_info_get: instance %p, is_enable %d, idx %d", enable_info->instance,
                    enable_info->is_enable, enable_info->idx);
}

#if F_APP_GAMING_CHAT_MIXING_SUPPORT
void app_eq_set_by_multilink_source_ui(uint8_t idx, void *data, uint16_t len)
{
    if (0)
    {}
#if F_APP_USB_AUDIO_SUPPORT
    else if (idx == multilink_usb_idx)
    {
        if (app_usb_audio_get_ds_track_state() == AUDIO_TRACK_STATE_STARTED)
        {
            eq_set((T_AUDIO_EFFECT_INSTANCE) app_usb_audio_get_eq_instance(), data, len);
        }
    }
#endif
#if F_APP_LINEIN_SUPPORT
    else if (idx == multilink_line_in_idx)
    {
        if (app_line_in_plug_state_get())
        {
            eq_set(app_db.line_in_eq_instance, data, len);
        }
    }
#endif
    else
    {
        eq_set(app_db.br_link[idx].eq_instance, data, len);
    }
}

void app_eq_set_for_multi_audio_stream(void *data, uint16_t len)
{
    uint8_t active_music_idx = app_multilink_customer_get_active_music_link_index();
    uint8_t secondary_music_idx = app_multilink_customer_get_secondary_music_link_index();

    app_eq_set_by_multilink_source_ui(active_music_idx, data, len);

    if (active_music_idx != secondary_music_idx)
    {
        app_eq_set_by_multilink_source_ui(secondary_music_idx, data, len);
    }
}
#endif

bool app_eq_index_set(T_EQ_TYPE eq_type, uint8_t mode, uint8_t index)
{
    uint8_t eq_num;
    uint16_t eq_len;
    bool ret = false;
    uint8_t *dynamic_eq_buf = calloc(1, app_db.max_eq_len);
    T_EQ_STREAM_TYPE stream_type = EQ_STREAM_TYPE_AUDIO;
    T_EQ_ENABLE_INFO enable_info;

    app_db.sw_eq_type = eq_type;
    app_eq_enable_info_get(mode, &enable_info);

#if F_APP_VOICE_SPK_EQ_SUPPORT
    if (eq_type == SPK_SW_EQ && mode == VOICE_SPK_MODE)
    {
        stream_type = EQ_STREAM_TYPE_VOICE;
    }
#endif

#if F_APP_VOICE_MIC_EQ_SUPPORT
    if (eq_type == MIC_SW_EQ && mode == VOICE_MIC_MODE)
    {
        stream_type = EQ_STREAM_TYPE_VOICE;
    }
#endif

    if (dynamic_eq_buf != NULL)
    {
        eq_num = eq_utils_num_get(eq_type, mode);

        APP_PRINT_INFO4("app_eq_index_set: eq_type %d, mode %d, eq_num %d, index 0x%02x",
                        eq_type, mode, eq_num, index);

        if (eq_num == 0)
        {
            goto exit;
        }
        else
        {
            eq_len = app_eq_dsp_param_get(eq_type, stream_type, mode, index, dynamic_eq_buf,
                                          true);

            if (eq_len == 0)
            {
                goto exit;
            }
        }

        if (eq_type == SPK_SW_EQ)
        {
            if (0)
            {}
#if F_APP_GAMING_CHAT_MIXING_SUPPORT
            else if (mode != VOICE_SPK_MODE)
            {
                app_eq_set_for_multi_audio_stream(dynamic_eq_buf, eq_len);
            }
#endif
#if F_APP_LINEIN_SUPPORT
            else if (app_line_in_plug_state_get())
            {
                eq_set(app_db.line_in_eq_instance, dynamic_eq_buf, eq_len);
            }
#endif
#if F_APP_USB_AUDIO_SUPPORT
            else if (app_usb_audio_get_ds_track_state() == AUDIO_TRACK_STATE_STARTED)
            {
                eq_set((T_AUDIO_EFFECT_INSTANCE) app_usb_audio_get_eq_instance(), dynamic_eq_buf, eq_len);
            }
#endif
            else
            {
#if F_APP_LEA_SUPPORT
                app_eq_media_eq_set(enable_info.instance, dynamic_eq_buf, eq_len);
#else
                eq_set(enable_info.instance, dynamic_eq_buf, eq_len);
#endif
            }

            app_cfg_nv.eq_idx = index;
            ret = true;
        }
        else
        {
#if F_APP_APT_SUPPORT
            if (mode == APT_MODE)
            {
                if (index <= eq_num && eq_len != 0)
                {
                    eq_set(app_db.apt_eq_instance, dynamic_eq_buf, eq_len);
                    app_cfg_nv.apt_eq_idx = index ;
                    ret = true;
                }
            }
#endif

#if F_APP_VOICE_MIC_EQ_SUPPORT
            if (mode == VOICE_MIC_MODE)
            {
                eq_set(enable_info.instance, dynamic_eq_buf, eq_len);
            }
#endif
        }
    }
    else
    {
        APP_PRINT_ERROR0("app_eq_index_set: fail");
    }

exit:

    if (dynamic_eq_buf != NULL)
    {
        free(dynamic_eq_buf);
        dynamic_eq_buf = NULL;
    }

    return ret;
}

bool app_eq_param_set(uint8_t eq_mode, uint8_t index, void *data, uint16_t len)
{
    uint8_t *buf;
    T_EQ_ENABLE_INFO enable_info;

    app_eq_enable_info_get(eq_mode, &enable_info);

    APP_PRINT_TRACE4("app_eq_param_set: eq_mode %d, index %u, data %p, len 0x%04x", eq_mode, index,
                     data, len);

    buf = data;

    if ((buf != NULL) && (len != 0))
    {
        if (app_db.sw_eq_type == SPK_SW_EQ)
        {
            if (0)
            {}
#if F_APP_GAMING_CHAT_MIXING_SUPPORT
            else if (eq_mode != VOICE_SPK_MODE)
            {
                app_eq_set_for_multi_audio_stream(buf, len);
            }
#endif
#if F_APP_USB_AUDIO_SUPPORT
            else if (app_usb_audio_get_ds_track_state() == AUDIO_TRACK_STATE_STARTED)
            {
                eq_set((T_AUDIO_EFFECT_INSTANCE) app_usb_audio_get_eq_instance(), buf, len);
            }
#endif
            else
            {
#if F_APP_LEA_SUPPORT
                app_eq_media_eq_set(enable_info.instance, buf, len);
#else
                eq_set(enable_info.instance, buf, len);
#endif
            }

            app_cfg_nv.eq_idx = index;
        }
        else
        {
            if (enable_info.instance != NULL)
            {
                eq_set(enable_info.instance, buf, len);
            }

#if F_APP_VOICE_MIC_EQ_SUPPORT
            if (eq_mode == VOICE_MIC_MODE)
            {
                app_cfg_nv.eq_idx = index;
            }
            else
#endif
            {
                app_cfg_nv.apt_eq_idx = index;
            }
        }

        return true;
    }

    return false;
}

#if F_APP_USER_EQ_SUPPORT
uint16_t app_eq_set_bits(uint8_t n)
{
    uint16_t val = 0;

    for (uint8_t i = 0; i < n; i++)
    {
        val |= BIT(i);
    }

    return val;
}

uint16_t app_eq_get_user_eq_size(void)
{
    uint16_t eq_size = sizeof(T_EQ_USER_EQ_DATA);

    return eq_size;
}

uint16_t app_eq_get_user_eq_data_offset(T_EQ_TYPE eq_type, uint8_t mode, uint8_t index,
                                        uint8_t user_eq_spk_eq_num)
{
    uint16_t offset = 0;
    uint8_t eq_index = eq_utils_original_eq_index_get(eq_type, mode, index);
    uint16_t eq_size = app_eq_get_user_eq_size();

#if F_APP_VOICE_SPK_EQ_SUPPORT
    if (mode == VOICE_SPK_MODE)
    {
        if (eq_type == SPK_SW_EQ)
        {
            offset = AUDIO_AND_APT_EQ_SIZE;
        }
    }
    else
#endif
    {
        offset = sizeof(T_EQ_USER_EQ_HEADER) + (eq_index * eq_size);

        if (eq_type == MIC_SW_EQ)
        {
            offset += (user_eq_spk_eq_num * eq_size);
        }
    }

    return offset;
}

void app_eq_move_mic_user_eq(uint8_t mic_eq_idx, uint8_t old_spk_user_eq_num,
                             uint8_t new_spk_user_eq_num)
{
    uint16_t eq_size = app_eq_get_user_eq_size();
    uint16_t offset;
    uint8_t *eq_data = NULL;

    offset = app_eq_get_user_eq_data_offset(MIC_SW_EQ, EQ_MODE_NULL, mic_eq_idx, old_spk_user_eq_num);

    eq_data = calloc(eq_size, 1);
    if (eq_data == NULL)
    {
        goto exit;
    }

    if (eq_utils_load_eq_from_ftl(offset, eq_data, eq_size) == 0)
    {
        offset = app_eq_get_user_eq_data_offset(MIC_SW_EQ, EQ_MODE_NULL, mic_eq_idx, new_spk_user_eq_num);
        eq_utils_save_eq_to_ftl(offset, (uint8_t *)eq_data, eq_size);
    }

exit:
    if (eq_data)
    {
        free(eq_data);
    }
}

void app_eq_adjust_user_eq(uint8_t old_spk_user_eq_num, uint8_t old_mic_user_eq_num)
{
    uint8_t new_spk_user_eq_num = app_cfg_const.user_eq_spk_eq_num;
    uint8_t new_mic_user_eq_num = app_cfg_const.user_eq_mic_eq_num;
    uint8_t min_mic_num = (new_mic_user_eq_num > old_mic_user_eq_num) ? old_mic_user_eq_num :
                          new_mic_user_eq_num;
    uint8_t n;

    if (new_spk_user_eq_num > old_spk_user_eq_num)
    {
        for (n = min_mic_num; n > 0; n--)
        {
            app_eq_move_mic_user_eq(n - 1, old_spk_user_eq_num, new_spk_user_eq_num);
        }
    }
    else if (new_spk_user_eq_num < old_spk_user_eq_num)
    {
        for (n = 0; n < min_mic_num; n++)
        {
            app_eq_move_mic_user_eq(n, old_spk_user_eq_num, new_spk_user_eq_num);
        }
    }

    for (n = old_spk_user_eq_num; n < new_spk_user_eq_num; n++)
    {
        app_eq_reset_user_eq(SPK_SW_EQ, EQ_MODE_NULL, n);
    }

    for (n = old_mic_user_eq_num; n < new_mic_user_eq_num; n++)
    {
        app_eq_reset_user_eq(MIC_SW_EQ, EQ_MODE_NULL, n);
    }
}

void app_eq_check_user_eq(void)
{
    T_EQ_USER_EQ_HEADER eq_header;

    if (eq_utils_load_eq_from_ftl(0, (uint8_t *)&eq_header, sizeof(T_EQ_USER_EQ_HEADER)) == 0)
    {
        if (eq_header.spk_user_eq_num != app_cfg_const.user_eq_spk_eq_num ||
            eq_header.mic_user_eq_num != app_cfg_const.user_eq_mic_eq_num)
        {
            app_eq_adjust_user_eq(eq_header.spk_user_eq_num, eq_header.mic_user_eq_num);

            eq_header.spk_user_eq_num = app_cfg_const.user_eq_spk_eq_num;
            eq_header.mic_user_eq_num = app_cfg_const.user_eq_mic_eq_num;
            eq_utils_save_eq_to_ftl(0, (uint8_t *) &eq_header, sizeof(T_EQ_USER_EQ_HEADER));
        }
    }
}

bool app_eq_is_valid_user_eq_index(T_EQ_TYPE eq_type, uint8_t mode, uint8_t index)
{
    uint8_t eq_index = eq_utils_original_eq_index_get(eq_type, mode, index);
    uint8_t user_eq_num = 0;
    bool ret = true;

    user_eq_num = (eq_type == SPK_SW_EQ) ? app_cfg_const.user_eq_spk_eq_num :
                  app_cfg_const.user_eq_mic_eq_num;

    if (user_eq_num <= eq_index)
    {
        ret = false;
    }

#if F_APP_VOICE_SPK_EQ_SUPPORT
    if (eq_type == SPK_SW_EQ && mode == VOICE_SPK_MODE)
    {
        ret = true;
    }
#endif

    return ret;
}

bool app_eq_reset_user_eq(T_EQ_TYPE eq_type, uint8_t mode, uint8_t index)
{
    bool ret = true;
    uint8_t error = 0;
    uint32_t offset = 0;
    uint8_t eq_index = eq_utils_original_eq_index_get(eq_type, mode, index);
    T_EQ_USER_EQ_DATA eq_data = {0};
    uint8_t saved_eq_mode = EQ_MODE_NULL;

    if (!app_eq_is_valid_user_eq_index(eq_type, mode, index))
    {
        error = 1;
        ret = false;
        goto exit;
    }

    eq_data.eq_type = eq_type;
    eq_data.eq_idx = eq_index;
    eq_data.eq_data_len = 0;

    offset = app_eq_get_user_eq_data_offset(eq_type, mode, index, app_cfg_const.user_eq_spk_eq_num);
    eq_utils_save_eq_to_ftl(offset, (uint8_t *) &eq_data, sizeof(T_EQ_USER_EQ_DATA));

exit:

    APP_PRINT_TRACE4("app_eq_reset_user_eq: type %d, idx %d, ret %d, %d", eq_type, eq_index, ret,
                     error);
    return ret;
}

void app_eq_reset_all_user_eq(void)
{
    uint8_t eq_index = 0;
    T_EQ_USER_EQ_HEADER eq_header = {0};

    eq_header.spk_user_eq_num = app_cfg_const.user_eq_spk_eq_num;
    eq_header.mic_user_eq_num = app_cfg_const.user_eq_mic_eq_num;
    eq_header.user_eq_version = USER_EQ_VERSION;

    eq_utils_save_eq_to_ftl(0, (uint8_t *) &eq_header, sizeof(T_EQ_USER_EQ_HEADER));

    for (eq_index = 0; eq_index < app_cfg_const.user_eq_spk_eq_num; eq_index++)
    {
        app_eq_reset_user_eq(SPK_SW_EQ, EQ_MODE_NULL, eq_index);
    }

#if F_APP_APT_SUPPORT
    for (eq_index = 0; eq_index < app_cfg_const.user_eq_mic_eq_num; eq_index++)
    {
        app_eq_reset_user_eq(MIC_SW_EQ, EQ_MODE_NULL, eq_index);
    }
#endif

#if F_APP_VOICE_SPK_EQ_SUPPORT
    app_eq_reset_user_eq(SPK_SW_EQ, VOICE_SPK_MODE, 0);
#endif


    APP_PRINT_TRACE0("app_eq_reset_all_user_eq: app_eq_reset_all_user_eq");
}

bool app_eq_save_user_eq_to_ftl(T_EQ_TYPE eq_type, uint8_t mode, uint8_t index,
                                uint8_t eq_adjust_side, uint8_t *eq_data,
                                uint16_t eq_data_len)
{
    uint8_t error = 0;
    bool ret = true;
    uint8_t eq_index = eq_utils_original_eq_index_get(eq_type, mode, index);
    uint16_t eq_data_offset = 0;
    T_EQ_USER_EQ_DATA user_eq_data = {0};

    if (!app_eq_is_valid_user_eq_index(eq_type, mode, index))
    {
        ret = false;
        error = 1;
        goto exit;
    }

    if (eq_data_len > sizeof(user_eq_data.eq_data))
    {
        ret = false;
        error = 2;
        goto exit;
    }

    user_eq_data.eq_type = eq_type;
    user_eq_data.eq_idx = eq_index;
    user_eq_data.eq_data_len = eq_data_len;
    memcpy(user_eq_data.eq_data, eq_data, eq_data_len);
    eq_data_offset = app_eq_get_user_eq_data_offset(eq_type, mode, index,
                                                    app_cfg_const.user_eq_spk_eq_num);

    if (eq_adjust_side == BOTH_SIDE_ADJUST || eq_adjust_side == app_cfg_const.bud_side)
    {
        eq_utils_save_eq_to_ftl(eq_data_offset, (uint8_t *)&user_eq_data, sizeof(T_EQ_USER_EQ_DATA));
    }

#if F_APP_ERWS_SUPPORT
    if (eq_adjust_side == BOTH_SIDE_ADJUST || eq_adjust_side != app_cfg_const.bud_side)
    {
        app_eq_relay_sync_user_eq_msg(eq_type, eq_index, EQ_SYNC_USER_EQ, eq_data_offset,
                                      (uint8_t *)&user_eq_data, sizeof(T_EQ_USER_EQ_DATA));
    }
#endif

exit:

    APP_PRINT_TRACE2("app_eq_save_user_eq_to_ftl: ret %d, %d", ret, error);
    return ret;
}

uint16_t app_eq_load_user_eq_from_ftl(T_EQ_TYPE eq_type, uint8_t mode, uint8_t index,
                                      uint8_t *p_data, bool is_eq_payload)
{
    uint8_t error = 0;
    uint32_t offset = 0;
    uint32_t tmp = 0;
    uint16_t eq_len = 0;
    T_EQ_USER_EQ_DATA eq_data = {0};
    T_EQ_PARAM *eq_param = NULL;

    if (!app_eq_is_valid_user_eq_index(eq_type, mode, index))
    {
        error = 1;
        goto exit;
    }

    offset = app_eq_get_user_eq_data_offset(eq_type, mode, index, app_cfg_const.user_eq_spk_eq_num);
    tmp = eq_utils_load_eq_from_ftl(offset, (uint8_t *)&eq_data, sizeof(T_EQ_USER_EQ_DATA));
    if (tmp != 0)
    {
        error = 2;
        goto exit;
    }

    if (eq_data.eq_data_len != 0)
    {
        if (is_eq_payload)
        {
            eq_param = (T_EQ_PARAM *)eq_data.eq_data;

            if ((T_EQ_PARAM_TYPE)eq_param->type == EQ_PARAM_TYPE_FINAL)
            {
                memcpy(p_data, eq_param->payload, eq_param->length);
                eq_len = eq_param->length;
            }
        }
        else
        {
            uint8_t i;
            uint8_t *eq_param_offset = eq_data.eq_data;
            uint8_t eq_param_len = 0;

            for (i = 0; i < EQ_PARAM_TYPE_MAX; i++)
            {
                eq_param = (T_EQ_PARAM *)eq_param_offset;

                eq_param_len = sizeof(T_EQ_PARAM) + eq_param->length;

                if ((T_EQ_PARAM_TYPE)eq_param->type == EQ_PARAM_TYPE_UI)
                {
                    memcpy(p_data, eq_param, eq_param_len);
                    eq_len += eq_param_len;
                }
                else if ((T_EQ_PARAM_TYPE)eq_param->type == EQ_PARAM_TYPE_COMPENSATION)
                {
                    memcpy(p_data + eq_len, eq_param, eq_param_len);
                    eq_len += eq_param_len;
                }

                eq_param_offset += eq_param_len;
            }
        }
    }

exit:
    APP_PRINT_TRACE2("app_eq_load_user_eq_from_ftl: len %d, %d", eq_len, error);

    return eq_len;
}

void app_eq_reset_unsaved_user_eq(void)
{
    if (app_eq_is_valid_user_eq_index(SPK_SW_EQ, app_db.spk_eq_mode, app_cfg_nv.eq_idx))
    {
#if F_APP_ERWS_SUPPORT
        if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
        {
            app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY,
                                               APP_REMOTE_MSG_SYNC_DEFAULT_EQ_INDEX, &app_cfg_nv.eq_idx, sizeof(uint8_t),
                                               REMOTE_TIMER_HIGH_PRECISION, 0,
                                               false);
        }
        else
#endif
        {
            app_eq_index_set(SPK_SW_EQ, app_db.spk_eq_mode, app_cfg_nv.eq_idx);
        }
    }

#if F_APP_APT_SUPPORT
    if (app_eq_is_valid_user_eq_index(MIC_SW_EQ, APT_MODE, app_cfg_nv.apt_eq_idx))
    {
#if F_APP_ERWS_SUPPORT
        if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
        {
            app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_APT, APP_REMOTE_MSG_APT_EQ_DEFAULT_INDEX_SYNC,
                                               &app_cfg_nv.apt_eq_idx, sizeof(uint8_t), REMOTE_TIMER_HIGH_PRECISION,
                                               0, false);
        }
        else
#endif
        {
            app_eq_index_set(MIC_SW_EQ, APT_MODE, app_cfg_nv.apt_eq_idx);
        }
    }
#endif

    APP_PRINT_TRACE0("app_eq_reset_unsaved_user_eq: app_eq_reset_unsaved_user_eq");
}

#if F_APP_ERWS_SUPPORT
bool app_eq_relay_sync_user_eq_msg(T_EQ_TYPE eq_type, uint8_t eq_idx,
                                   T_EQ_SYNC_ACTION sync_eq_action, uint16_t offset, uint8_t *eq_data, uint32_t len)
{
    uint8_t *buffer = NULL;
    bool ret = true;

    buffer = calloc(1, len + 5);
    if (buffer == NULL)
    {
        ret = false;
        goto exit;
    }

    buffer[0] = sync_eq_action;
    buffer[1] = eq_type;
    buffer[2] = eq_idx;
    buffer[3] = LO_WORD(offset);
    buffer[4] = HI_WORD(offset);

    memcpy(&buffer[5], eq_data, len);

    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_USER_EQ,
                                        buffer, len + 5);

    free(buffer);

exit:
    return ret;
}

void app_eq_process_sync_user_eq_when_b2b_connected(T_EQ_TYPE eq_type, uint8_t eq_idx,
                                                    uint16_t offset, uint8_t *eq_data, uint16_t eq_len)
{
    static uint8_t *final_data = NULL;
    uint16_t eq_size = app_eq_get_user_eq_size();
    uint16_t eq_offset = app_eq_get_user_eq_data_offset(eq_type, EQ_MODE_NULL, eq_idx,
                                                        app_cfg_const.user_eq_spk_eq_num);

    if (eq_offset == offset && eq_size == eq_len)
    {
        /* EQ data did not been split to multiple packet, save data directly */
        eq_utils_save_eq_to_ftl(offset, eq_data, eq_len);
    }
    else
    {
        if (final_data == NULL)
        {
            final_data = (uint8_t *) malloc(eq_size);
            if (!final_data)
            {
                return;
            }
        }

        memcpy_s(&final_data[offset - eq_offset], (eq_size - (offset - eq_offset)), eq_data, eq_len);

        if ((offset + eq_len) == (eq_offset + eq_size))
        {
            /* receiving final packet of this EQ data, save data */
            eq_utils_save_eq_to_ftl(eq_offset, final_data, eq_size);
            free(final_data);
            final_data = NULL;

#if F_APP_APT_SUPPORT
            if ((app_apt_is_normal_apt_on_state(app_db.current_listening_state) ||
                 app_listening_is_anc_apt_on_state(app_db.current_listening_state)))
            {
                if (eq_type == MIC_SW_EQ)
                {
                    uint8_t ori_idx = eq_utils_original_eq_index_get(MIC_SW_EQ, APT_MODE, app_cfg_nv.apt_eq_idx);

                    if (eq_idx == ori_idx)
                    {
                        app_eq_index_set(MIC_SW_EQ, APT_MODE, app_cfg_nv.apt_eq_idx);
                    }
                }
            }
#endif
        }
    }
}

void app_eq_continue_sync_user_eq_when_connected(bool is_first_time, T_EQ_TYPE last_eq_type,
                                                 uint8_t last_eq_idx, uint16_t last_offset, uint16_t last_len)
{
    uint16_t eq_size = app_eq_get_user_eq_size();
    T_EQ_TYPE eq_type = EQ_TYPE_MAX;
    uint8_t eq_idx = last_eq_idx;
    uint16_t eq_offset = app_eq_get_user_eq_data_offset(last_eq_type, EQ_MODE_NULL, last_eq_idx,
                                                        app_cfg_const.user_eq_spk_eq_num);
    uint16_t new_offset = last_offset + last_len;
    uint16_t len;

    if (!is_first_time && ((new_offset - eq_offset) < eq_size)) /* sync the remaining data of this eq */
    {
        if (eq_offset + eq_size - new_offset >= MAX_RELAY_SIZE)
        {
            len = MAX_RELAY_SIZE;
        }
        else
        {
            len = eq_offset + eq_size - new_offset;
        }

        eq_type = last_eq_type;
    }
    else /* sync next eq */
    {
        if (!is_first_time)
        {
            if (last_eq_type == SPK_SW_EQ)
            {
                spk_user_eq_idx_waiting_sync &= ~(BIT(last_eq_idx));
            }
#if F_APP_APT_SUPPORT
            else if (last_eq_type == MIC_SW_EQ)
            {
                mic_user_eq_idx_waiting_sync &= ~(BIT(last_eq_idx));
            }
#endif
        }

        eq_idx = eq_utils_original_eq_index_get(SPK_SW_EQ, app_db.spk_eq_mode, app_cfg_nv.eq_idx);
        if (spk_user_eq_idx_waiting_sync & BIT(eq_idx)) /* sync current spk eq first */
        {
            eq_type = SPK_SW_EQ;
        }
        else
        {
#if F_APP_APT_SUPPORT
            eq_idx = eq_utils_original_eq_index_get(MIC_SW_EQ, APT_MODE, app_cfg_nv.apt_eq_idx);

            if (mic_user_eq_idx_waiting_sync & BIT(eq_idx)) /* sync current mic eq first */
            {
                eq_type = MIC_SW_EQ;
            }
            else
#endif
            {
                if (spk_user_eq_idx_waiting_sync != 0)
                {
                    eq_idx = __builtin_ffs(spk_user_eq_idx_waiting_sync) - 1;
                    eq_type = SPK_SW_EQ;
                }
#if F_APP_APT_SUPPORT
                else if (mic_user_eq_idx_waiting_sync != 0)
                {
                    eq_idx = __builtin_ffs(mic_user_eq_idx_waiting_sync) - 1;
                    eq_type = MIC_SW_EQ;
                }
#endif
            }
        }

        if (eq_type != EQ_TYPE_MAX)
        {
            len = (eq_size >= MAX_RELAY_SIZE) ? MAX_RELAY_SIZE : eq_size;
            new_offset = app_eq_get_user_eq_data_offset(eq_type, EQ_MODE_NULL, eq_idx,
                                                        app_cfg_const.user_eq_spk_eq_num);
        }
    }

    if (eq_type != EQ_TYPE_MAX)
    {
        uint16_t offset;
        uint8_t eq_data[MAX_RELAY_SIZE];

        if (eq_utils_load_eq_from_ftl(new_offset, eq_data, len) == 0)
        {
            app_eq_relay_sync_user_eq_msg(eq_type, eq_idx, EQ_SYNC_USER_EQ_WHEN_B2B_CONNECTED, new_offset,
                                          eq_data, len);
        }
    }
}

void app_eq_sync_user_eq_when_connected(void)
{
#if F_APP_AUDIO_VOICE_SPK_EQ_INDEPENDENT_CFG
    spk_user_eq_idx_waiting_sync = 0;
#else
    spk_user_eq_idx_waiting_sync = app_eq_set_bits(app_cfg_const.user_eq_spk_eq_num);
#endif

#if F_APP_APT_SUPPORT
    mic_user_eq_idx_waiting_sync = 0;

    if (app_cfg_const.normal_apt_support && !app_cfg_const.rws_apt_eq_adjust_separate)
    {
        if (eq_utils_num_get(MIC_SW_EQ, APT_MODE) != 0)
        {
            mic_user_eq_idx_waiting_sync = app_eq_set_bits(app_cfg_const.user_eq_mic_eq_num);
        }
    }
#endif

    app_eq_continue_sync_user_eq_when_connected(true, EQ_TYPE_MAX, 0, 0, 0);
}

#if (F_APP_SEPARATE_ADJUST_APT_EQ_SUPPORT == 1) || (F_APP_AUDIO_VOICE_SPK_EQ_INDEPENDENT_CFG == 1)
void app_eq_report_sec_eq_to_src(uint8_t cmd_path, uint8_t app_idx, uint16_t eq_len,
                                 uint8_t *eq_data)
{
    T_AUDIO_EQ_REPORT_DATA eq_report_data;

    if (app_eq_get_link_info(cmd_path, app_idx, &eq_report_data))
    {
        T_AUDIO_EQ_INFO *eq_info = eq_report_data.eq_info;

        eq_info->eq_data_len = eq_len;

        if (eq_info->eq_data_buf != NULL)
        {
            free(eq_info->eq_data_buf);
        }

        eq_info->eq_data_buf = malloc(app_db.max_eq_len * EQ_PARAM_TYPE_MAX);
        if (eq_info->eq_data_buf)
        {
            memcpy_s(eq_info->eq_data_buf, (app_db.max_eq_len * EQ_PARAM_TYPE_MAX), eq_data,
                     eq_info->eq_data_len);
            app_eq_report_eq_frame(&eq_report_data);
        }
    }
}

void app_eq_report_sec_eq_to_pri(uint8_t eq_type, uint8_t eq_mode, uint8_t eq_idx, uint8_t cmd_path,
                                 uint8_t app_idx)
{
    T_AUDIO_EQ_INFO eq_info;

    memset(&eq_info, 0, sizeof(eq_info));

    eq_info.eq_mode = eq_mode;
    eq_info.eq_idx = eq_idx;
    eq_info.sw_eq_type = eq_type;
    eq_info.eq_data_buf = NULL;

    if (app_eq_report_get_eq_extend_info(&eq_info))
    {
        uint8_t *buffer = malloc(eq_info.eq_data_len + 7);

        if (buffer)
        {
            buffer[0] = EQ_SYNC_REPORT_SECONDARY_EQ_INFO;
            buffer[1] = eq_type;
            buffer[2] = eq_idx;
            buffer[3] = cmd_path;
            buffer[4] = app_idx;
            buffer[5] = LO_WORD(eq_info.eq_data_len);
            buffer[6] = HI_WORD(eq_info.eq_data_len);

            memcpy(&buffer[7], eq_info.eq_data_buf, eq_info.eq_data_len);
            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_USER_EQ,
                                                buffer, eq_info.eq_data_len + 7);
            free(buffer);
        }
    }

    if (eq_info.eq_data_buf)
    {
        free(eq_info.eq_data_buf);
    }
}
#endif  /* F_APP_SEPARATE_ADJUST_APT_EQ_SUPPORT */
#endif  /* F_APP_ERWS_SUPPORT */
#endif  /* F_APP_USER_EQ_SUPPORT */

uint8_t app_eq_get_default_idx(T_EQ_TYPE eq_type, uint8_t mode)
{
    if (eq_type == SPK_SW_EQ)
    {
        return app_dsp_cfg_data->eq_param.header->eq_spk_application_header[mode].default_spk_eq_idx;
    }
    else
    {
        return app_dsp_cfg_data->eq_param.header->eq_mic_application_header[mode].default_mic_eq_idx;
    }
}

static uint16_t app_eq_dsp_param_get(T_EQ_TYPE eq_type, T_EQ_STREAM_TYPE stream_type,
                                     uint8_t eq_mode, uint8_t index, uint8_t *data,
                                     bool is_eq_payload)
{
    uint16_t eq_len = 0;

#if F_APP_USER_EQ_SUPPORT
    if ((stream_type == EQ_STREAM_TYPE_AUDIO) || ((stream_type == EQ_STREAM_TYPE_VOICE) &&
                                                  (eq_type == SPK_SW_EQ)))
    {
        eq_len = app_eq_load_user_eq_from_ftl(eq_type, eq_mode, index, data, is_eq_payload);
    }

    if (eq_len == 0)
#endif
    {
        uint8_t eq_idx_ori = eq_utils_original_eq_index_get(eq_type, eq_mode, index);

        if ((eq_idx_ori <= EQ_MAX_INDEX) && (data != NULL))
        {
            for (uint8_t i = 0; i < app_dsp_cfg_data->eq_param.header->eq_num; i++)
            {
                uint8_t type = app_dsp_cfg_data->eq_param.header->sub_header[i].eq_type;
                uint8_t eq_idx = app_dsp_cfg_data->eq_param.header->sub_header[i].eq_idx;
                uint8_t scenario = app_dsp_cfg_data->eq_param.header->sub_header[i].stream_type;

                if (type == eq_type && eq_idx == eq_idx_ori && scenario == stream_type)
                {
                    uint16_t param_len = app_dsp_cfg_data->eq_param.header->sub_header[i].param_length;

                    if (param_len != 0)
                    {
                        uint32_t eq_offset = app_dsp_cfg_data->dsp_cfg_header.eq_block_offset +
                                             app_dsp_cfg_data->eq_param.header->sub_header[i].offset;

                        if (is_eq_payload)
                        {
                            app_dsp_cfg_load_param_r_data(data, eq_offset, param_len);
                            eq_len = param_len;
                        }
                        else
                        {
                            data[0] = EQ_PARAM_TYPE_UI;
                            data[1] = param_len;
                            app_dsp_cfg_load_param_r_data(data + 2, eq_offset, param_len);
                            eq_len = param_len + 2;
                        }
                    }
                    break;
                }

            }
        }
        else
        {
            APP_PRINT_ERROR1("app_eq_dsp_param_get: error idx %d", eq_idx_ori);
        }
    }

    APP_PRINT_INFO6("app_eq_dsp_param_get: eq_type %d, dsp cfg index %d, stream_type %d, data %p, is_payload %d, len %d",
                    eq_type, index, stream_type, data, is_eq_payload, eq_len);

    return eq_len;
}

void app_eq_audio_eq_enable(T_AUDIO_EFFECT_INSTANCE *eq_instance, bool *audio_eq_enabled)
{
    if (eq_instance && audio_eq_enabled)
    {
        if (!(*audio_eq_enabled))
        {
            eq_enable(*eq_instance);
            *audio_eq_enabled = true;
        }
    }
}

static bool app_eq_report_get_eq_extend_info(T_AUDIO_EQ_INFO *p_eq_info)
{
    bool ret = true;
    int err = 0;

    uint16_t eq_len = 0;
    uint8_t *eq_data_temp = calloc(app_db.max_eq_len * EQ_PARAM_TYPE_MAX, sizeof(uint8_t));
    T_EQ_STREAM_TYPE stream_type = EQ_STREAM_TYPE_AUDIO;

#if F_APP_VOICE_SPK_EQ_SUPPORT
    if (p_eq_info->eq_mode == VOICE_SPK_MODE)
    {
        stream_type = EQ_STREAM_TYPE_VOICE;
    }
#endif

    if (!eq_data_temp)
    {
        err = 1;
        ret = false;
        goto exit;
    }

    eq_len = app_eq_dsp_param_get((T_EQ_TYPE)p_eq_info->sw_eq_type, stream_type, p_eq_info->eq_mode,
                                  p_eq_info->eq_idx, eq_data_temp, false);

    if (eq_len == 0)
    {
        err = 2;
        ret = false;
        goto exit;
    }

    p_eq_info->eq_data_len = eq_len;

    if (p_eq_info->eq_data_buf != NULL)
    {
        free(p_eq_info->eq_data_buf);
    }

    p_eq_info->eq_data_buf = malloc(p_eq_info->eq_data_len);
    if (p_eq_info->eq_data_buf == NULL)
    {
        err = 3;
        ret = false;
        goto exit;
    }

    memcpy(&(p_eq_info->eq_data_buf)[0], &eq_data_temp[0], eq_len);

exit:
    if (eq_data_temp)
    {
        free(eq_data_temp);
    }

    APP_PRINT_TRACE2("app_eq_generate_eq_param_for_report: ret %d, err %d", ret, err);

    return ret;
}

void app_eq_report_abort_frame(T_AUDIO_EQ_REPORT_DATA *eq_report_data)
{
    uint8_t abort_frame[10];
    uint16_t frame_len = 0x02;

    abort_frame[0] = eq_report_data->eq_info->eq_idx;
    abort_frame[1] = eq_report_data->eq_info->adjust_side;
    abort_frame[2] = eq_report_data->eq_info->sw_eq_type;
    abort_frame[3] = eq_report_data->eq_info->eq_mode;
    abort_frame[4] = AUDIO_EQ_FRAME_ABORT;
    abort_frame[5] = eq_report_data->eq_info->eq_seq;
    abort_frame[6] = (uint8_t)(frame_len & 0xFF);
    abort_frame[7] = (uint8_t)((frame_len >> 8) & 0xFF);
    abort_frame[8] = (uint8_t)(CMD_AUDIO_EQ_PARAM_GET & 0xFF);
    abort_frame[9] = (uint8_t)((CMD_AUDIO_EQ_PARAM_GET >> 8) & 0xFF);

    app_report_event(eq_report_data->cmd_path, EVENT_AUDIO_EQ_PARAM_REPORT, eq_report_data->id,
                     abort_frame, frame_len + EQ_ABORT_FRAME_HEADER_LEN);

    if (eq_report_data->eq_info->eq_data_buf != NULL)
    {
        free(eq_report_data->eq_info->eq_data_buf);
        eq_report_data->eq_info->eq_data_buf = NULL;
    }
}

bool app_eq_get_link_info(uint8_t cmd_path, uint8_t app_idx, T_AUDIO_EQ_REPORT_DATA *eq_report_data)
{
    bool ret = false;

    eq_report_data->cmd_path = cmd_path;

    if (cmd_path == CMD_PATH_SPP || cmd_path == CMD_PATH_IAP)
    {
        T_APP_BR_LINK *p_br_link = (T_APP_BR_LINK *) &app_db.br_link[app_idx];

        if (p_br_link)
        {
            eq_report_data->eq_info = &p_br_link->audio_get_eq_info;
            eq_report_data->id = p_br_link->id;

            ret = true;
        }
    }
    else if (cmd_path == CMD_PATH_LE)
    {
        T_APP_LE_LINK *p_le_link = (T_APP_LE_LINK *) &app_db.le_link[app_idx];

        if (p_le_link)
        {
            eq_report_data->eq_info = &p_le_link->audio_get_eq_info;

            /* EQ: 3 is att header,  6 is trasmint service header */
            eq_report_data->max_frame_len = p_le_link->mtu_size - 3 - EQ_START_FRAME_HEADER_LEN - 6;
            eq_report_data->id = p_le_link->id;

            ret = true;
        }
    }
    else if (cmd_path == CMD_PATH_GATT_OVER_BREDR)
    {
        T_APP_BR_LINK *p_br_link = (T_APP_BR_LINK *) &app_db.br_link[app_idx];

        if (p_br_link)
        {
            eq_report_data->eq_info = &p_br_link->audio_get_eq_info;

            /* EQ: 3 is att header,  6 is trasmint service header */
            eq_report_data->max_frame_len = p_br_link->mtu_size - 3 - EQ_START_FRAME_HEADER_LEN - 6;
            eq_report_data->id = p_br_link->id;

            ret = true;
        }
    }

    return ret;
}

bool app_eq_report_eq_frame(T_AUDIO_EQ_REPORT_DATA *eq_report_data)
{
    T_AUDIO_EQ_INFO *p_eq_info = eq_report_data->eq_info;
    uint16_t frame_len = p_eq_info->eq_data_len;
    T_AUDIO_EQ_FRAME_TYPE eq_frame_type;
    uint8_t *frame;
    uint8_t frame_idx = 0;
    uint16_t total_frame_len = 0;

    if (eq_report_data->cmd_path == CMD_PATH_LE ||
        eq_report_data->cmd_path == CMD_PATH_GATT_OVER_BREDR)
    {
        if (p_eq_info->eq_data_offset == 0)
        {
            if (p_eq_info->eq_data_len > eq_report_data->max_frame_len)
            {
                eq_frame_type = AUDIO_EQ_FRAME_START;
                frame_len = eq_report_data->max_frame_len;
            }
            else
            {
                eq_frame_type = AUDIO_EQ_FRAME_SINGLE;
            }
        }
        else
        {
            if (p_eq_info->eq_data_len > eq_report_data->max_frame_len)
            {
                eq_frame_type = AUDIO_EQ_FRAME_CONTINUE;
                frame_len = eq_report_data->max_frame_len;
            }
            else
            {
                eq_frame_type = AUDIO_EQ_FRAME_END;
            }
        }
    }
    else
    {
        eq_frame_type = AUDIO_EQ_FRAME_SINGLE;
    }

    if (eq_frame_type == AUDIO_EQ_FRAME_START)
    {
        total_frame_len = frame_len + EQ_START_FRAME_HEADER_LEN;
    }
    else
    {
        total_frame_len = frame_len + EQ_SINGLE_FRAME_HEADER_LEN;
    }

    frame = malloc(total_frame_len);

    if (frame == NULL)
    {
        app_eq_report_abort_frame(eq_report_data);
        return false;
    }

    frame[frame_idx++] = p_eq_info->eq_idx;
    frame[frame_idx++] = p_eq_info->adjust_side;
    frame[frame_idx++] = p_eq_info->sw_eq_type;
    frame[frame_idx++] = p_eq_info->eq_mode;
    frame[frame_idx++] = eq_frame_type;
    frame[frame_idx++] = p_eq_info->eq_seq;

    if (eq_frame_type == AUDIO_EQ_FRAME_START)
    {
        frame[frame_idx++] = (uint8_t)(p_eq_info->eq_data_len & 0xFF);/*the low byte of frame_len*/
        frame[frame_idx++] = (uint8_t)((p_eq_info->eq_data_len >> 8) & 0xFF);/*the high byte of frame_len*/
    }

    frame[frame_idx++] = (uint8_t)(frame_len & 0xFF);/*the low byte of frame_len*/
    frame[frame_idx++] = (uint8_t)((frame_len >> 8) & 0xFF);/*the high byte of frame_len*/

    memcpy(&frame[frame_idx], p_eq_info->eq_data_buf, frame_len);

    app_report_event(eq_report_data->cmd_path, EVENT_AUDIO_EQ_PARAM_REPORT, eq_report_data->id,
                     frame, total_frame_len);

    p_eq_info->eq_data_len -= frame_len;

    if (frame[3] == AUDIO_EQ_FRAME_START || frame[3] == AUDIO_EQ_FRAME_CONTINUE)
    {
        p_eq_info->eq_data_offset += frame_len;
        (p_eq_info->eq_seq)++;
    }
    else
    {
        if (p_eq_info->eq_data_buf != NULL)
        {
            free(p_eq_info->eq_data_buf);
            p_eq_info->eq_data_buf = NULL;
        }
    }

    free(frame);

    return true;
}

void app_eq_report_eq_param(uint8_t cmd_path, uint8_t app_idx)
{
    int8_t err = 0;
    T_AUDIO_EQ_REPORT_DATA eq_report_data;

    if (!app_eq_get_link_info(cmd_path, app_idx, &eq_report_data))
    {
        err = 1;
        goto exit;
    }

    if (eq_report_data.eq_info->eq_data_len > 0)
    {
        if (!app_eq_report_eq_frame(&eq_report_data))
        {
            err = 2;
            goto exit;
        }
    }

exit:

    APP_PRINT_TRACE3("app_eq_report_eq_param: cmd_path %d, len %d, ret %d", cmd_path,
                     eq_report_data.eq_info->eq_data_len, err);
}

void app_eq_report_terminate_param_report(uint8_t cmd_path, uint8_t app_idx)
{
    T_AUDIO_EQ_REPORT_DATA eq_report_data;

    int8_t err = 0;

    if (!app_eq_get_link_info(cmd_path, app_idx, &eq_report_data))
    {
        err = 1;
        goto exit;
    }

    app_eq_report_abort_frame(&eq_report_data);

exit:
    APP_PRINT_TRACE1("app_eq_report_abort: err %d", err);
}

void app_eq_idx_check_accord_mode(void)
{
#if F_APP_LINEIN_SUPPORT
    if ((app_db.spk_eq_mode == LINE_IN_MODE) &&
        (eq_utils_num_get(SPK_SW_EQ, app_db.spk_eq_mode) != 0))
    {
        app_cfg_nv.eq_idx = app_cfg_nv.eq_idx_line_in_mode_record;
    }
    else
#endif
    {
        if ((app_db.spk_eq_mode == GAMING_MODE) &&
            (eq_utils_num_get(SPK_SW_EQ, app_db.spk_eq_mode) != 0))
        {
            app_cfg_nv.eq_idx = app_cfg_nv.eq_idx_gaming_mode_record;
        }
#if F_APP_ANC_SUPPORT
        else if ((app_db.spk_eq_mode == ANC_MODE) &&
                 (eq_utils_num_get(SPK_SW_EQ, app_db.spk_eq_mode) != 0))
        {
            app_cfg_nv.eq_idx = app_cfg_nv.eq_idx_anc_mode_record;
        }
#endif
#if F_APP_VOICE_SPK_EQ_SUPPORT
        else if ((app_db.spk_eq_mode == VOICE_SPK_MODE) &&
                 (eq_utils_num_get(SPK_SW_EQ, app_db.spk_eq_mode) != 0))
        {
            app_cfg_nv.eq_idx = 0;
        }
#endif
        else if ((eq_utils_num_get(SPK_SW_EQ, NORMAL_MODE) != 0))
        {
            app_cfg_nv.eq_idx = app_cfg_nv.eq_idx_normal_mode_record;
        }
    }
}

void app_eq_sync_idx_accord_eq_mode(uint8_t eq_mode, uint8_t index)
{
    if (eq_mode == GAMING_MODE)
    {
        app_cfg_nv.eq_idx_gaming_mode_record = index;

#if F_APP_ERWS_SUPPORT
        app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_GAMING_RECORD_EQ_INDEX);
#endif
    }
    else if (eq_mode == NORMAL_MODE)
    {
        app_cfg_nv.eq_idx_normal_mode_record = index;

#if F_APP_ERWS_SUPPORT
        app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_NORMAL_RECORD_EQ_INDEX);
#endif
    }

#if F_APP_ANC_SUPPORT
    else if (eq_mode == ANC_MODE)
    {
        app_cfg_nv.eq_idx_anc_mode_record = index;

#if F_APP_ERWS_SUPPORT
        app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_ANC_RECORD_EQ_INDEX);
#endif
    }
#endif

#if F_APP_LINEIN_SUPPORT
    else if (eq_mode == LINE_IN_MODE)
    {
        /* do not need to relay due to only stereo support line-in */
        app_cfg_nv.eq_idx_line_in_mode_record = index;
    }
#endif
}

void app_eq_play_audio_eq_tone(void)
{
#if F_APP_TEAMS_FEATURE_SUPPORT
    return;
#else

    uint8_t eq_index = eq_utils_original_eq_index_get(SPK_SW_EQ, app_db.spk_eq_mode, app_cfg_nv.eq_idx);

    app_audio_tone_type_play((T_APP_AUDIO_TONE_TYPE)(TONE_AUDIO_EQ_0 + eq_index),
                             false, true);
#endif
}

void app_eq_play_apt_eq_tone(void)
{
    uint8_t eq_index = eq_utils_original_eq_index_get(MIC_SW_EQ, APT_MODE, app_cfg_nv.apt_eq_idx);

#if F_APP_TEAMS_FEATURE_SUPPORT
#else
    app_audio_tone_type_play((T_APP_AUDIO_TONE_TYPE)(TONE_APT_EQ_0 + eq_index),
                             false, true);
#endif
}

T_AUDIO_EFFECT_INSTANCE app_eq_create(T_EQ_CONTENT_TYPE eq_content_type,
                                      T_EQ_STREAM_TYPE stream_type, T_EQ_TYPE eq_type, uint8_t eq_mode, uint8_t eq_index)
{
    T_AUDIO_EFFECT_INSTANCE eq_instance = NULL;
    uint8_t eq_num = eq_utils_num_get(eq_type, eq_mode);
    uint8_t err = 0;

    if (eq_num != 0)
    {
        uint8_t *eq_buf = calloc(1, app_db.max_eq_len);

        if (eq_buf != NULL)
        {
            uint16_t eq_len = app_eq_dsp_param_get(eq_type, stream_type, eq_mode, eq_index, eq_buf,
                                                   true);

            if (eq_len != 0)
            {
                eq_instance = eq_create(eq_content_type, eq_buf, eq_len);
            }
            else
            {
                err = 1;
            }

            free(eq_buf);
            eq_buf = NULL;
        }
        else
        {
            err = 2;
        }
    }
    else
    {
        err = 3;
    }

    if (err != 0)
    {
        APP_PRINT_ERROR1("app_eq_create: fail %d", err);
    }

    return eq_instance;
}

bool app_eq_cmd_operate(uint8_t eq_mode, uint8_t eq_adjust_side, uint8_t is_play_eq_tone,
                        uint8_t eq_idx, uint8_t eq_len_to_dsp, uint8_t *cmd_ptr)
{
    bool ret = true;
    T_EQ_ENABLE_INFO enable_info;

    app_eq_enable_info_get(eq_mode, &enable_info);

    APP_PRINT_TRACE6("app_eq_cmd_operate: type:%d, mode:%d, side:%d, tone:%d, idx:%d, len:%d",
                     app_db.sw_eq_type, eq_mode, eq_adjust_side, is_play_eq_tone, eq_idx, eq_len_to_dsp);

#if F_APP_ERWS_SUPPORT
    app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_SW_EQ_TYPE);
#endif

    if (app_db.sw_eq_type == SPK_SW_EQ)
    {
        if (is_play_eq_tone)
        {
            app_cfg_nv.eq_idx = eq_idx;
            app_eq_play_audio_eq_tone();
        }

        if (eq_adjust_side == BOTH_SIDE_ADJUST)
        {
#if F_APP_ERWS_SUPPORT
            if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                uint16_t relay_data_len = eq_len_to_dsp + 1;
                uint8_t *relay_data = (uint8_t *)malloc(relay_data_len);

                relay_data[0] = eq_mode;
                memcpy(&relay_data[1], &cmd_ptr[0], eq_len_to_dsp);

                app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_EQ_DATA,
                                                   relay_data, relay_data_len, REMOTE_TIMER_HIGH_PRECISION, 0, false);
                free(relay_data);
            }
            else
#endif
            {
                app_eq_param_set(eq_mode, eq_idx, &cmd_ptr[0], eq_len_to_dsp);

#if F_APP_LEA_SUPPORT
                app_eq_media_eq_enable(&enable_info);
#else
                app_eq_audio_eq_enable(&enable_info.instance, &enable_info.is_enable);
#endif
            }
        }
        else if (eq_adjust_side == app_cfg_const.bud_side)
        {
            app_eq_param_set(eq_mode, eq_idx, &cmd_ptr[0], eq_len_to_dsp);

#if F_APP_LEA_SUPPORT
            app_eq_media_eq_enable(&enable_info);
#else
            app_eq_audio_eq_enable(&enable_info.instance, &enable_info.is_enable);
#endif
        }
        else if (eq_adjust_side ^ app_cfg_const.bud_side)
        {
            uint16_t relay_data_len = eq_len_to_dsp + 1;
            uint8_t *relay_data = (uint8_t *)malloc(relay_data_len);

            relay_data[0] = eq_mode;
            memcpy(&relay_data[1], &cmd_ptr[0], eq_len_to_dsp);

            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_EQ_DATA,
                                                relay_data, relay_data_len);
            free(relay_data);
        }
    }
    else if (app_db.sw_eq_type == MIC_SW_EQ)
    {
#if F_APP_APT_SUPPORT
        if (eq_mode == APT_MODE)
        {
            if (app_cfg_nv.apt_eq_idx != eq_idx)
            {
                app_cfg_nv.apt_eq_idx = eq_idx;

#if F_APP_ERWS_SUPPORT
                app_relay_async_single(APP_MODULE_TYPE_APT, APP_REMOTE_MSG_APT_EQ_INDEX_SYNC);
#endif
            }

            if (is_play_eq_tone)
            {
                app_eq_play_apt_eq_tone();
            }
        }
#endif

        if (eq_adjust_side == BOTH_SIDE_ADJUST)
        {
#if F_APP_ERWS_SUPPORT
            if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                uint16_t relay_data_len = eq_len_to_dsp + 1;
                uint8_t *relay_data = (uint8_t *)malloc(relay_data_len);

                relay_data[0] = eq_mode;
                memcpy(&relay_data[1], &cmd_ptr[0], eq_len_to_dsp);

                app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_EQ_DATA,
                                                   relay_data, relay_data_len, REMOTE_TIMER_HIGH_PRECISION, 0, false);
                free(relay_data);
            }
            else
#endif
            {
                app_eq_param_set(eq_mode, eq_idx, &cmd_ptr[0], eq_len_to_dsp);
                eq_enable(enable_info.instance);
            }
        }
        else if (eq_adjust_side == app_cfg_const.bud_side)
        {
            app_eq_param_set(eq_mode, eq_idx, &cmd_ptr[0], eq_len_to_dsp);
            eq_enable(enable_info.instance);
        }
        else if (eq_adjust_side ^ app_cfg_const.bud_side)
        {
            uint16_t relay_data_len = eq_len_to_dsp + 1;
            uint8_t *relay_data = (uint8_t *)malloc(relay_data_len);

            relay_data[0] = eq_mode;
            memcpy(&relay_data[1], &cmd_ptr[0], eq_len_to_dsp);

            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_EQ_DATA,
                                                relay_data, relay_data_len);
            free(relay_data);
        }
    }
    else
    {
        ret = false;
    }

exit:
    return ret;
}

static uint8_t app_eq_judge_mic_eq_mode(uint8_t stream_type)
{
    uint8_t new_eq_mode = MIC_EQ_MODE_MAX;

#if F_APP_APT_SUPPORT
    bool is_normal_apt_on = app_apt_is_normal_apt_on_state(app_db.current_listening_state);

#if F_APP_SUPPORT_ANC_APT_COEXIST
    is_normal_apt_on |= app_listening_is_anc_apt_on_state(app_db.current_listening_state);
#endif
#endif

    T_APP_CALL_STATUS lea_call_status = APP_CALL_IDLE;

#if F_APP_LEA_SUPPORT
    lea_call_status = app_lea_ccp_get_call_status();
#endif

    if (stream_type == EQ_STREAM_TYPE_AUDIO)
    {
#if F_APP_APT_SUPPORT
        if (is_normal_apt_on && (eq_utils_num_get(MIC_SW_EQ, APT_MODE) != 0))
        {
            new_eq_mode = APT_MODE;
        }
#endif
    }
    else if (stream_type == EQ_STREAM_TYPE_VOICE)
    {
#if F_APP_VOICE_MIC_EQ_SUPPORT
        if ((app_hfp_sco_is_connected() || lea_call_status != APP_CALL_IDLE) &&
            (eq_utils_num_get(MIC_SW_EQ, VOICE_MIC_MODE) != 0))
        {
            new_eq_mode = VOICE_MIC_MODE;
        }
#endif
    }

    return new_eq_mode;
}

uint8_t app_eq_judge_audio_eq_mode(void)
{
    uint8_t new_eq_mode = SPK_EQ_MODE_MAX;

#if F_APP_ANC_SUPPORT
    bool is_anc_on = app_anc_is_anc_on_state(app_db.current_listening_state);

#if F_APP_SUPPORT_ANC_APT_COEXIST
    is_anc_on |= app_listening_is_anc_apt_on_state(app_db.current_listening_state);
#endif
#endif

    T_APP_CALL_STATUS lea_call_status = APP_CALL_IDLE;

#if F_APP_LEA_SUPPORT
    lea_call_status = app_lea_ccp_get_call_status();
#endif

    if (0)
    {
    }
#if F_APP_LINEIN_SUPPORT
    else if (app_line_in_plug_state_get() && (eq_utils_num_get(SPK_SW_EQ, LINE_IN_MODE) != 0))
    {
        new_eq_mode = LINE_IN_MODE;
    }
#endif
#if F_APP_VOICE_SPK_EQ_SUPPORT
    else if ((app_link_get_sco_conn_num() || lea_call_status != APP_CALL_IDLE) &&
             (eq_utils_num_get(SPK_SW_EQ, VOICE_SPK_MODE) != 0))
    {
        new_eq_mode = VOICE_SPK_MODE;
    }
#endif
    else if (app_db.gaming_mode && (eq_utils_num_get(SPK_SW_EQ, GAMING_MODE) != 0))
    {
        new_eq_mode = GAMING_MODE;
    }
#if F_APP_ANC_SUPPORT
    else if (is_anc_on && (eq_utils_num_get(SPK_SW_EQ, ANC_MODE) != 0))
    {
        new_eq_mode = ANC_MODE;
    }
#endif
    else
    {
        new_eq_mode = NORMAL_MODE;
    }

    return new_eq_mode;
}

uint8_t app_eq_mode_get(uint8_t eq_type, uint8_t stream_type)
{
    uint8_t eq_mode = MIC_EQ_MODE_MAX;

    if (eq_type == MIC_SW_EQ)
    {
        eq_mode = app_eq_judge_mic_eq_mode(stream_type);
    }
    else
    {
        eq_mode = app_eq_judge_audio_eq_mode();
    }

    APP_PRINT_TRACE2("app_eq_mode_get: eq_type %d, eq_mode %d", eq_type, eq_mode);
    return eq_mode;
}

void app_eq_enable_effect(uint8_t eq_mode, uint16_t eq_len)
{
    T_APP_BR_LINK *link = NULL;
    uint8_t *buf = calloc(1, app_db.max_eq_len);

    if (buf != NULL)
    {
        if (app_db.sw_eq_type == SPK_SW_EQ)
        {
#if F_APP_VOICE_SPK_EQ_SUPPORT
            if (eq_mode == VOICE_SPK_MODE)
            {
                link = &app_db.br_link[app_hfp_get_active_idx()];
                link->voice_spk_eq_instance = eq_create(EQ_CONTENT_TYPE_VOICE_OUT, buf, eq_len);

                if (link->voice_spk_eq_instance != NULL)
                {
                    eq_enable(link->voice_spk_eq_instance);
                    audio_track_effect_attach(link->sco_track_handle, link->voice_spk_eq_instance);
                }
            }
            else
#endif
            {
                link = &app_db.br_link[app_a2dp_get_active_idx()];
                link->eq_instance = eq_create(EQ_CONTENT_TYPE_AUDIO, buf, eq_len);

                if (link->eq_instance != NULL)
                {
                    app_eq_audio_eq_enable(&link->eq_instance, &link->audio_eq_enabled);
                    audio_track_effect_attach(link->a2dp_track_handle, link->eq_instance);
                }
            }
        }
        else
        {
#if F_APP_VOICE_MIC_EQ_SUPPORT
            if (eq_mode == VOICE_MIC_MODE)
            {
                link = &app_db.br_link[app_hfp_get_active_idx()];
                link->voice_mic_eq_instance = eq_create(EQ_CONTENT_TYPE_VOICE_IN, buf, eq_len);

                if (link->voice_mic_eq_instance != NULL)
                {
                    eq_enable(link->voice_mic_eq_instance);
                    audio_track_effect_attach(link->sco_track_handle, link->voice_mic_eq_instance);
                }
            }
            else
#endif
            {
#if F_APP_APT_SUPPORT
                app_db.apt_eq_instance = eq_create(EQ_CONTENT_TYPE_PASSTHROUGH, buf, eq_len);

                if (app_db.apt_eq_instance != NULL)
                {
                    eq_enable(app_db.apt_eq_instance);
                    audio_passthrough_effect_attach(app_db.apt_eq_instance);
                }
#endif
            }
        }
        free(buf);
        buf = NULL;
    }
    else
    {
        APP_PRINT_ERROR0("app_eq_enable_effect: fail");
    }
}

void app_eq_change_audio_eq_mode(bool report_when_eq_mode_change)
{
    uint8_t new_eq_mode;
    uint8_t eq_idx;

    new_eq_mode = app_eq_judge_audio_eq_mode();

    if (app_db.spk_eq_mode != new_eq_mode)
    {
        APP_PRINT_TRACE2("app_eq_change_audio_eq_mode: eq_mode %d -> %d", app_db.spk_eq_mode,
                         new_eq_mode);
        app_db.spk_eq_mode = new_eq_mode;
        app_eq_idx_check_accord_mode();
        eq_idx = app_cfg_nv.eq_idx;

#if F_APP_USER_EQ_SUPPORT
        if (app_eq_is_valid_user_eq_index(SPK_SW_EQ, new_eq_mode, eq_idx))
        {
            app_eq_index_set(SPK_SW_EQ, new_eq_mode, eq_idx);
        }
        else
#endif
        {
            if (!app_db.eq_ctrl_by_src)
            {
                app_eq_index_set(SPK_SW_EQ, new_eq_mode, eq_idx);
            }
        }

#if F_APP_ERWS_SUPPORT
        if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
#endif
        {
            if (report_when_eq_mode_change)
            {
                app_report_eq_idx(EQ_INDEX_REPORT_BY_CHANGE_MODE);
            }
        }
    }
}

void app_eq_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                       uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));
    ack_pkt[2] = CMD_SET_STATUS_COMPLETE;

    switch (cmd_id)
    {
    case CMD_AUDIO_EQ_QUERY:
        {
            uint8_t query_type;
            uint8_t buf[11];
            uint8_t eq_reply_len = 0;

            query_type = cmd_ptr[2];
            buf[0] = query_type;

            if (query_type == AUDIO_EQ_QUERY_STATE)
            {
                buf[1] = 1;
                eq_reply_len = 2;
            }
            else if (query_type == AUDIO_EQ_QUERY_NUM)
            {
                buf[1] = eq_utils_num_get(SPK_SW_EQ, NORMAL_MODE);
                buf[2] = eq_utils_num_get(SPK_SW_EQ, GAMING_MODE);

#if F_APP_APT_SUPPORT
                buf[3] = eq_utils_num_get(MIC_SW_EQ, APT_MODE);
#endif

#if F_APP_ANC_SUPPORT
                buf[4] = eq_utils_num_get(SPK_SW_EQ, ANC_MODE);
#endif
                eq_reply_len = 5;
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                break;
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_AUDIO_EQ_REPLY, app_idx, buf, eq_reply_len);
        }
        break;

    case CMD_AUDIO_EQ_QUERY_PARAM:
        {
            uint8_t eq_mode = cmd_ptr[2];
            uint8_t buf[23] = {0};
            uint32_t capacity;
            uint16_t support_sample_rate = 0;

            /* EQ state */
            buf[0] = 1;

#if F_APP_USER_EQ_SUPPORT
            /* EQ mapping */
            buf[1] = app_cfg_const.user_eq_spk_eq_num;
            buf[2] = app_cfg_const.user_eq_mic_eq_num;
#endif

            capacity = eq_utils_get_capacity(SPK_SW_EQ, NORMAL_MODE);
            buf[3] = LO_WORD(capacity);
            buf[4] = HI_WORD(capacity);

            capacity = eq_utils_get_capacity(SPK_SW_EQ, GAMING_MODE);
            buf[5] = LO_WORD(capacity);
            buf[6] = HI_WORD(capacity);

#if F_APP_APT_SUPPORT
            capacity = eq_utils_get_capacity(MIC_SW_EQ, APT_MODE);
            buf[7] = LO_WORD(capacity);
            buf[8] = HI_WORD(capacity);
#endif

#if F_APP_ANC_SUPPORT
            capacity = eq_utils_get_capacity(SPK_SW_EQ, ANC_MODE);
            buf[9] = LO_WORD(capacity);
            buf[10] = HI_WORD(capacity);
#endif

            /* sample rate */
            buf[11] = 0;
            buf[12] = LO_WORD(support_sample_rate);
            buf[13] = HI_WORD(support_sample_rate);

            /* spk EQ mode & index*/
            buf[14] = app_db.spk_eq_mode;
            buf[15] = app_cfg_nv.eq_idx_normal_mode_record;
            buf[16] = app_cfg_nv.eq_idx_gaming_mode_record;

#if F_APP_ANC_SUPPORT
            buf[17] = app_cfg_nv.eq_idx_anc_mode_record;
#endif

#if F_APP_APT_SUPPORT
            /* mic EQ mode & index */
            buf[18] = APT_MODE;
            buf[19] = app_cfg_nv.apt_eq_idx;
#endif

            capacity = eq_utils_get_capacity(SPK_SW_EQ, VOICE_SPK_MODE);
            buf[20] = LO_WORD(capacity);
            buf[21] = HI_WORD(capacity);
            buf[22] = 0;

            app_report_event(cmd_path, EVENT_AUDIO_EQ_REPLY_PARAM, app_idx, buf, sizeof(buf));
        }
        break;

    case CMD_AUDIO_EQ_PARAM_SET:
        {
            uint8_t eq_idx = cmd_ptr[2];
            uint8_t eq_adjust_side = cmd_ptr[3];
            uint8_t eq_mode = cmd_ptr[5];
            uint8_t save_eq = cmd_ptr[6];
            uint8_t type = cmd_ptr[7];
            uint8_t seq = cmd_ptr[8];

            bool is_play_eq_tone = false;
            uint8_t eq_len_to_dsp = 0;
            T_AUDIO_EQ_INFO *p_audio_eq_info = NULL;
            uint8_t *eq_data = NULL;
            T_EQ_PARAM *eq_param = NULL;

            app_db.sw_eq_type = cmd_ptr[4];

            if ((app_db.sw_eq_type == SPK_SW_EQ) && (app_cfg_nv.eq_idx != eq_idx) ||
                (app_db.sw_eq_type == MIC_SW_EQ) && (app_cfg_nv.apt_eq_idx != eq_idx))
            {
                /* change eq, it needs to play eq tone */
                is_play_eq_tone = true;
            }

            if ((app_db.sw_eq_type == SPK_SW_EQ) && (app_db.spk_eq_mode != eq_mode))
            {
                if (save_eq != SAVE_EQ_FIELD_ONLY_SAVE)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    break;
                }
            }

            if (cmd_path == CMD_PATH_SPP ||
                cmd_path == CMD_PATH_IAP ||
                cmd_path == CMD_PATH_GATT_OVER_BREDR)
            {
                p_audio_eq_info = &app_db.br_link[app_idx].audio_set_eq_info;
            }
            else if (cmd_path == CMD_PATH_LE)
            {
                p_audio_eq_info = &app_db.le_link[app_idx].audio_set_eq_info;
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                break;
            }

            if (type == AUDIO_EQ_FRAME_SINGLE || type == AUDIO_EQ_FRAME_START)
            {
                p_audio_eq_info->eq_seq = EQ_INIT_SEQ;
                p_audio_eq_info->eq_mode = cmd_ptr[5];
                p_audio_eq_info->eq_data_len = (uint16_t)(cmd_ptr[9] | cmd_ptr[10] << 8);
                p_audio_eq_info->eq_data_offset = 0;

                if (type == AUDIO_EQ_FRAME_START)
                {
                    if (p_audio_eq_info->eq_data_buf != NULL)
                    {
                        free(p_audio_eq_info->eq_data_buf);
                    }

                    p_audio_eq_info->eq_data_buf = malloc(p_audio_eq_info->eq_data_len);

                    if (p_audio_eq_info->eq_data_buf == NULL)
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                        break;
                    }
                }
            }

            if (seq != p_audio_eq_info->eq_seq)
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                break;
            }

            if (type == AUDIO_EQ_FRAME_SINGLE)
            {
                eq_data = &cmd_ptr[11];
            }
            else
            {
                uint16_t frame_len = 0;
                uint8_t cmd_idx;

                if (type == AUDIO_EQ_FRAME_START)
                {
                    frame_len = (uint16_t)(cmd_ptr[11] | cmd_ptr[12] << 8);
                    cmd_idx = 13;
                }
                else
                {
                    frame_len = (uint16_t)(cmd_ptr[9] | cmd_ptr[10] << 8);
                    cmd_idx = 11;
                }

                p_audio_eq_info->eq_seq = p_audio_eq_info->eq_seq + 1;
                memcpy(p_audio_eq_info->eq_data_buf + p_audio_eq_info->eq_data_offset, &cmd_ptr[cmd_idx],
                       frame_len);
                p_audio_eq_info->eq_data_offset += frame_len;
                eq_data = p_audio_eq_info->eq_data_buf;
            }

            if (type == AUDIO_EQ_FRAME_SINGLE || type == AUDIO_EQ_FRAME_END)
            {
                if (app_db.sw_eq_type == SPK_SW_EQ)
                {
                    app_eq_sync_idx_accord_eq_mode(eq_mode, eq_idx);
                }

                // calculate eq_len_to_dsp
                eq_param = (T_EQ_PARAM *)eq_data;

                if ((T_EQ_PARAM_TYPE)eq_param->type == EQ_PARAM_TYPE_FINAL)
                {
                    eq_len_to_dsp = eq_param->length;

                    if (save_eq != SAVE_EQ_FIELD_ONLY_SAVE)
                    {
                        if (!app_eq_cmd_operate(eq_mode, eq_adjust_side, is_play_eq_tone, eq_idx, eq_len_to_dsp,
                                                eq_param->payload))
                        {
                            ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                        }
                    }
                }

#if F_APP_USER_EQ_SUPPORT
                if (save_eq != SAVE_EQ_FIELD_ONLY_APPLY)
                {
                    app_eq_save_user_eq_to_ftl((T_EQ_TYPE)app_db.sw_eq_type, eq_mode, eq_idx,
                                               eq_adjust_side,
                                               &eq_data[0],
                                               p_audio_eq_info->eq_data_len);
                }
#endif

                if (p_audio_eq_info->eq_data_buf != NULL)
                {
                    free(p_audio_eq_info->eq_data_buf);
                    p_audio_eq_info->eq_data_buf = NULL;
                }
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_AUDIO_EQ_PARAM_GET:
        {
            if (cmd_len < 5)
            {
                /* if the length of cmd less than 5, the EQ version is 1.1 */
                T_SRC_SUPPORT_VER_FORMAT *version = app_cmd_get_src_version(cmd_path, app_idx);

                version->eq_spec_ver_major = 1;
                version->eq_spec_ver_minor = 1;
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_cmd_update_eq_ctrl(false, true);
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                break;
            }

            T_AUDIO_EQ_REPORT_DATA eq_report_data;

            if (app_eq_get_link_info(cmd_path, app_idx, &eq_report_data))
            {
                T_AUDIO_EQ_INFO *p_eq_info = eq_report_data.eq_info;

                p_eq_info->eq_idx = cmd_ptr[2];
                p_eq_info->adjust_side = cmd_ptr[3];
                p_eq_info->sw_eq_type = cmd_ptr[4];
                p_eq_info->eq_mode = cmd_ptr[5];
                p_eq_info->eq_seq = EQ_INIT_SEQ;
                p_eq_info->eq_data_offset = 0;

                if ((p_eq_info->adjust_side == app_cfg_const.bud_side) ||
                    (p_eq_info->adjust_side == BOTH_SIDE_ADJUST))
                {
                    if (app_eq_report_get_eq_extend_info(p_eq_info))
                    {
                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                        app_eq_report_eq_frame(&eq_report_data);
                    }
                    else
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    }
                }
                else
                {
#if ((F_APP_USER_EQ_SUPPORT && F_APP_SEPARATE_ADJUST_APT_EQ_SUPPORT) || (F_APP_AUDIO_VOICE_SPK_EQ_INDEPENDENT_CFG == 1))
                    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                    {
                        uint8_t buf[6] = {0};

                        buf[0] = EQ_SYNC_GET_SECONDARY_EQ_INFO;
                        buf[1] = p_eq_info->sw_eq_type;
                        buf[2] = p_eq_info->eq_idx;
                        buf[3] = p_eq_info->eq_mode;
                        buf[4] = cmd_path;
                        buf[5] = app_idx;

                        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_USER_EQ,
                                                            buf, sizeof(buf));
                    }
                    else
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                    }

                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
#endif
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;

    case CMD_AUDIO_EQ_INDEX_SET:
        {
            uint8_t eq_idx;
            uint8_t eq_mode;
            bool is_play_eq_tone = false;

            eq_idx = cmd_ptr[2];
            eq_mode = cmd_ptr[3];

            if (app_cfg_nv.eq_idx != eq_idx)
            {
                is_play_eq_tone = true;
            }

            if (app_db.spk_eq_mode != eq_mode)
            {
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
            else
            {
                app_eq_sync_idx_accord_eq_mode(eq_mode, eq_idx);
                app_cfg_nv.eq_idx = eq_idx;

#if F_APP_ERWS_SUPPORT
                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                {
                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY,
                                                       APP_REMOTE_MSG_SYNC_DEFAULT_EQ_INDEX, &eq_idx, sizeof(uint8_t), REMOTE_TIMER_HIGH_PRECISION, 0,
                                                       false);
                }
                else
#endif
                {
                    app_eq_index_set(SPK_SW_EQ, eq_mode, eq_idx);
                    T_EQ_ENABLE_INFO enable_info;

                    app_eq_enable_info_get(eq_mode, &enable_info);

#if F_APP_LEA_SUPPORT
                    app_eq_media_eq_enable(&enable_info);
#else
                    app_eq_audio_eq_enable(&enable_info.instance, &enable_info.is_enable);
#endif
                }

                if (is_play_eq_tone)
                {
                    app_eq_play_audio_eq_tone();
                }

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;

    case CMD_AUDIO_EQ_INDEX_GET:
        {
            uint8_t eq_mode = cmd_ptr[2];

            if (app_db.spk_eq_mode != eq_mode)
            {
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                break;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_eq_idx(EQ_INDEX_REPORT_BY_GET_EQ_INDEX);
        }
        break;

#if F_APP_APT_SUPPORT
    case CMD_APT_EQ_INDEX_SET:
        {
            uint8_t event_data = cmd_ptr[2];
            uint8_t eq_num;
            bool is_play_eq_tone = false;
            eq_num = eq_utils_num_get(MIC_SW_EQ, APT_MODE);

            if (app_cfg_nv.apt_eq_idx != event_data)
            {
                is_play_eq_tone = true;
            }

            if (eq_num != 0)
            {
                app_cfg_nv.apt_eq_idx = event_data;

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                    app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    if (is_play_eq_tone)
                    {
                        app_eq_play_apt_eq_tone();
                    }

#if F_APP_ERWS_SUPPORT
                    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                    {
                        app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_APT, APP_REMOTE_MSG_APT_EQ_DEFAULT_INDEX_SYNC,
                                                           &event_data, sizeof(uint8_t), REMOTE_TIMER_HIGH_PRECISION,
                                                           0, false);
                    }
                    else
#endif
                    {
                        app_eq_index_set(MIC_SW_EQ, APT_MODE, app_cfg_nv.apt_eq_idx);
                        eq_enable(app_db.apt_eq_instance);
                    }
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_APT_EQ_INDEX_GET:
        {
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_apt_eq_idx(EQ_INDEX_REPORT_BY_GET_APT_EQ_INDEX);
        }
        break;
#endif

#if F_APP_USER_EQ_SUPPORT
    case CMD_RESET_EQ_DATA:
        {
            T_AUDIO_EQ_REPORT_DATA eq_report_data;

            uint8_t eq_type = cmd_ptr[2];
            uint8_t eq_mode = cmd_ptr[3];
            uint8_t eq_idx = cmd_ptr[4];
            uint8_t adjust_side = cmd_ptr[5];
            uint8_t buf[4] = {EQ_SYNC_RESET_EQ, eq_type, eq_idx, eq_mode};
            bool is_apply_default_eq = true;

#if F_APP_ERWS_SUPPORT
            T_APP_MODULE_TYPE module_type = APP_MODULE_TYPE_AUDIO_POLICY;
            uint8_t msg_type = APP_REMOTE_MSG_SYNC_DEFAULT_EQ_INDEX;

#if F_APP_APT_SUPPORT
            if (eq_type == MIC_SW_EQ)
            {
                module_type = APP_MODULE_TYPE_APT;
                msg_type = APP_REMOTE_MSG_APT_EQ_DEFAULT_INDEX_SYNC;
            }
#endif

            if (eq_type == SPK_SW_EQ)
            {
                if ((app_db.spk_eq_mode != eq_mode) || (eq_idx != app_cfg_nv.eq_idx))
                {
                    is_apply_default_eq = false;
                }
            }

            if (adjust_side == BOTH_SIDE_ADJUST &&
                app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                app_eq_reset_user_eq((T_EQ_TYPE)eq_type, eq_mode, eq_idx);
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_USER_EQ, buf,
                                                    sizeof(buf));

                if (is_apply_default_eq)
                {
                    app_relay_sync_single_with_raw_msg(module_type, msg_type, &eq_idx, sizeof(uint8_t),
                                                       REMOTE_TIMER_HIGH_PRECISION, 0, false);
                }
            }
#if (F_APP_SEPARATE_ADJUST_APT_EQ_SUPPORT == 1) || (F_APP_AUDIO_VOICE_SPK_EQ_INDEPENDENT_CFG == 1)
            else if (adjust_side != app_cfg_const.bud_side && adjust_side != BOTH_SIDE_ADJUST)
            {
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_USER_EQ, buf,
                                                    sizeof(buf));

                if (is_apply_default_eq)
                {
                    app_relay_async_single_with_raw_msg(module_type, msg_type, &eq_idx, sizeof(uint8_t));
                }
            }
#endif
            else
#endif  /* F_APP_ERWS_SUPPORT */
            {
                app_eq_reset_user_eq((T_EQ_TYPE) eq_type, eq_mode, eq_idx);

                if (is_apply_default_eq)
                {
                    app_eq_index_set((T_EQ_TYPE) eq_type, eq_mode, eq_idx);
                }
            }

            if (app_eq_get_link_info(cmd_path, app_idx, &eq_report_data))
            {
                T_AUDIO_EQ_INFO *p_eq_info = eq_report_data.eq_info;

                p_eq_info->sw_eq_type = eq_type;
                p_eq_info->eq_mode = eq_mode;
                p_eq_info->eq_idx = eq_idx;
                p_eq_info->adjust_side = adjust_side;
                p_eq_info->eq_seq = EQ_INIT_SEQ;
                p_eq_info->eq_data_offset = 0;

#if (F_APP_SEPARATE_ADJUST_APT_EQ_SUPPORT == 1) || (F_APP_AUDIO_VOICE_SPK_EQ_INDEPENDENT_CFG == 1)
                if (adjust_side != BOTH_SIDE_ADJUST && adjust_side != app_cfg_const.bud_side)
                {
                    uint8_t buffer[6] = {EQ_SYNC_GET_SECONDARY_EQ_INFO, eq_type, eq_idx, eq_mode, cmd_path, app_idx};

                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_USER_EQ,
                                                        buffer, sizeof(buffer));

                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                }
                else
#endif
                {
                    if (app_eq_report_get_eq_extend_info(p_eq_info))
                    {
                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                        app_eq_report_eq_frame(&eq_report_data);
                    }
                    else
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    }
                }
            }
        }
        break;
#endif  /* F_APP_USER_EQ_SUPPORT */

    default:
        break;
    }
}

