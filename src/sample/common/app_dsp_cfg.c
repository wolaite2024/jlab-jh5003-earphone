/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <string.h>
#include <stdlib.h>
#include "os_mem.h"
#include "trace.h"
#include "patch_header_check.h"
#include "fmc_api.h"
#include "bt_types.h"
#include "eq_utils.h"
#include "app_dsp_cfg.h"

#if F_APP_BRIGHTNESS_SUPPORT
#include "app_audio_passthrough_brightness.h"
#endif

#if F_APP_EXT_MIC_SWITCH_SUPPORT
#include "app_main.h"
#include "app_cfg.h"
#include "audio_track.h"
#include "app_a2dp.h"
#include "app_hfp.h"
#include "app_audio_policy.h"
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
#include "app_dongle_record.h"
#endif

#if F_APP_USB_AUDIO_SUPPORT
#include "app_usb_audio.h"
#endif

#if F_APP_EXT_MIC_SWITCH_PHYSICAL_MIC_SUPPORT
#include "app_audio_route.h"
#endif

#define DSP_DATA_SYNC_WORD 0xAA55

typedef enum
{
    APP_DSP_EXTENSIBLE_PARAM_BRIGHTNESS  = 0x00,
} T_APP_DSP_EXTENSIBLE_PARAM_TYPE;

T_APP_DSP_CFG_VOLUME app_dsp_cfg_vol =    //load from dsp data
{
    .playback_volume_min = 0,
    .playback_volume_max = 15,
    .playback_volume_default = 8,
    .voice_out_volume_min = 0,
    .voice_out_volume_max = 15,
    .voice_out_volume_default = 8,
    .voice_volume_in_min = 0,
    .voice_volume_in_max = 15,
    .voice_volume_in_default = 8,
    .record_volume_min = 0,
    .record_volume_max = 15,
    .record_volume_default = 12,
    .voice_prompt_volume_min = 0,
    .voice_prompt_volume_max = 15,
    .voice_prompt_volume_default = 8,
    .ringtone_volume_min = 0,
    .ringtone_volume_max = 15,
    .ringtone_volume_default = 8,
    .tts_volume_max = 15,
    .tts_volume_min = 0,
    .tts_volume_default = 10,
    .line_in_volume_in_max = 15,
    .line_in_volume_in_min = 0,
    .line_in_volume_in_default = 10,
    .line_in_volume_out_max = 15,
    .line_in_volume_out_min = 0,
    .line_in_volume_out_default = 10,
};

T_APP_DSP_CFG_DATA *app_dsp_cfg_data;
T_APP_DSP_CFG_SIDETONE app_dsp_cfg_sidetone;

#if F_APP_EXT_MIC_SWITCH_SUPPORT

static void app_dsp_cfg_restart_audio_track(void)
{
    T_APP_BR_LINK *p_link = app_link_find_br_link(app_db.br_link[app_a2dp_get_active_idx()].bd_addr);
    T_APP_BR_LINK *p_link_hfp = app_link_find_br_link(app_db.br_link[app_hfp_get_active_idx()].bd_addr);

    if (p_link)
    {
        if (p_link->a2dp_track_handle)
        {
            audio_track_stop(p_link->a2dp_track_handle);
            audio_track_start(p_link->a2dp_track_handle);
        }
    }

    if (p_link_hfp)
    {
        if (p_link_hfp->sco_track_handle)
        {
            audio_track_stop(p_link_hfp->sco_track_handle);

            audio_track_volume_in_set(p_link_hfp->sco_track_handle, app_dsp_cfg_vol.voice_volume_in_default);

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY || app_audio_is_mic_mute())
            {
                audio_track_volume_in_mute(p_link_hfp->sco_track_handle);
            }

#if F_APP_VOICE_MIC_EQ_SUPPORT
            app_eq_index_set(MIC_SW_EQ, VOICE_MIC_MODE, 0);
#endif

            audio_track_start(p_link_hfp->sco_track_handle);
        }
    }

#if F_APP_GAMING_DONGLE_SUPPORT
    if (app_db.dongle_is_enable_mic)
    {
        app_dongle_record_restart_audio_track();
    }
#endif

#if F_APP_USB_AUDIO_SUPPORT
    if (app_usb_audio_get_us_track_state() == AUDIO_TRACK_STATE_STARTED)
    {
        app_usb_audio_stop(USB_AUDIO_SCENARIO_CAPTURE);
        app_usb_audio_start(USB_AUDIO_SCENARIO_CAPTURE);
    }
#endif
}

static void app_dsp_cfg_reload(void)
{
    app_dsp_cfg_init(app_cfg_const.normal_apt_support);
    app_eq_init();
}

void app_dsp_cfg_apply(void)
{
    app_dsp_cfg_reload();
    app_dsp_cfg_restart_audio_track();

    if (app_db.apt_eq_instance != NULL)
    {
        app_eq_index_set(MIC_SW_EQ, APT_MODE, app_cfg_nv.apt_eq_idx);
    }

    APP_PRINT_TRACE1("app_dsp_cfg_reload: ext_mic_plugged %d", app_db.ext_mic_plugged);
}
#endif

static uint32_t app_dsp_cfg_get_dsp_config_addr(void)
{
#if F_APP_EXT_MIC_SWITCH_SUPPORT
    FLASH_IMG_ID dsp_image = FLASH_IMG_DSPCONFIG;

    if (app_db.ext_mic_plugged)
    {
#if CONFIG_SOC_SERIES_RTL8773D || TARGET_RTL8773DFL
        dsp_image = FLASH_IMG_EXT2;
#else
        dsp_image = FLASH_IMG_ADSP;
#endif
    }

    return flash_cur_bank_img_payload_addr_get(dsp_image);

#else
    return flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG);
#endif
}

void app_dsp_cfg_load_param_r_data(void *p_data, uint16_t offset, uint16_t size)
{
    fmc_flash_nor_read((app_dsp_cfg_get_dsp_config_addr() + offset), (uint8_t *)p_data, size);
}

static void app_dsp_cfg_load_header(void)
{
#if F_APP_EXT_MIC_SWITCH_SUPPORT
    if (app_dsp_cfg_data == NULL)
    {
        app_dsp_cfg_data = calloc(1, sizeof(T_APP_DSP_CFG_DATA));
    }
#else
    app_dsp_cfg_data = calloc(1, sizeof(T_APP_DSP_CFG_DATA));
#endif
    if (app_dsp_cfg_data != NULL)
    {
        fmc_flash_nor_read(app_dsp_cfg_get_dsp_config_addr(),
                           &app_dsp_cfg_data->dsp_cfg_header, sizeof(T_APP_DSP_CONFIG_HEADER));
    }
}

static void app_dsp_cfg_gain_table_load(bool is_normal_apt)
{
    T_APP_DSP_GAIN_TABLE_BLOCK *block = NULL;
    int32_t ret = 0;

    block = calloc(1, sizeof(T_APP_DSP_GAIN_TABLE_BLOCK));
    if (block == NULL)
    {
        ret = 1;
        goto fail_calloc;
    }

    app_dsp_cfg_load_param_r_data(block,
                                  app_dsp_cfg_data->dsp_cfg_header.gain_table_block_offset,
                                  sizeof(T_APP_DSP_GAIN_TABLE_BLOCK));
    if (block->sync_word != DSP_DATA_SYNC_WORD)
    {
        ret = 2;
        goto fail_check;
    }

    //playback
    app_dsp_cfg_vol.playback_volume_max = block->audio_dac_gain_level_max;
    app_dsp_cfg_vol.playback_volume_min = block->audio_dac_gain_level_min;
    app_dsp_cfg_vol.playback_volume_default = block->audio_dac_gain_level_default;

    //voice
    app_dsp_cfg_vol.voice_out_volume_max = block->voice_dac_gain_level_max;
    app_dsp_cfg_vol.voice_out_volume_min = block->voice_dac_gain_level_min;
    app_dsp_cfg_vol.voice_out_volume_default = block->voice_dac_gain_level_default;
    app_dsp_cfg_vol.voice_volume_in_max = block->voice_adc_gain_level_max;
    app_dsp_cfg_vol.voice_volume_in_min = block->voice_adc_gain_level_min;
    app_dsp_cfg_vol.voice_volume_in_default = block->voice_adc_gain_level_default;

    //record
    app_dsp_cfg_vol.record_volume_max = block->record_adc_gain_level_max;
    app_dsp_cfg_vol.record_volume_min = block->record_adc_gain_level_min;
    app_dsp_cfg_vol.record_volume_default = block->record_adc_gain_level_default;

    //line in
    app_dsp_cfg_vol.line_in_volume_out_max = block->aux_dac_gain_level_max;
    app_dsp_cfg_vol.line_in_volume_out_min = block->aux_dac_gain_level_min;
    app_dsp_cfg_vol.line_in_volume_out_default = block->aux_dac_gain_level_default;
    app_dsp_cfg_vol.line_in_volume_in_max = block->aux_adc_gain_level_max;
    app_dsp_cfg_vol.line_in_volume_in_min = block->aux_adc_gain_level_min;
    app_dsp_cfg_vol.line_in_volume_in_default = block->aux_adc_gain_level_default;

    //ringtone
    app_dsp_cfg_vol.ringtone_volume_max = block->ringtone_dac_gain_level_max;
    app_dsp_cfg_vol.ringtone_volume_min = block->ringtone_dac_gain_level_min;
    app_dsp_cfg_vol.ringtone_volume_default = block->ringtone_dac_gain_level_default;

    //voice prompt
    app_dsp_cfg_vol.voice_prompt_volume_max = block->vp_dac_gain_level_max;
    app_dsp_cfg_vol.voice_prompt_volume_min = block->vp_dac_gain_level_min;
    app_dsp_cfg_vol.voice_prompt_volume_default = block->vp_dac_gain_level_default;

    //apt
    if (is_normal_apt)
    {
        app_dsp_cfg_vol.apt_volume_out_max = block->apt_dac_gain_level_max;
        app_dsp_cfg_vol.apt_volume_out_min = block->apt_dac_gain_level_min;
        app_dsp_cfg_vol.apt_volume_out_default = block->apt_dac_gain_level_default;
        app_dsp_cfg_vol.apt_volume_in_max = block->apt_adc_gain_level_max;
        app_dsp_cfg_vol.apt_volume_in_min = block->apt_adc_gain_level_min;
        app_dsp_cfg_vol.apt_volume_in_default = block->apt_adc_gain_level_default;
    }
    else
    {
        app_dsp_cfg_vol.apt_volume_out_max = block->llapt_dac_gain_level_max;
        app_dsp_cfg_vol.apt_volume_out_min = block->llapt_dac_gain_level_min;
        app_dsp_cfg_vol.apt_volume_out_default = block->llapt_dac_gain_level_default;
        app_dsp_cfg_vol.apt_volume_in_max = block->llapt_adc_gain_level_max;
        app_dsp_cfg_vol.apt_volume_in_min = block->llapt_adc_gain_level_min;
        app_dsp_cfg_vol.apt_volume_in_default = block->llapt_adc_gain_level_default;
    }

    memcpy(app_dsp_cfg_data->audio_dac_gain_table, block->audio_dac_gain_table,
           sizeof(block->audio_dac_gain_table));
    memcpy(app_dsp_cfg_data->voice_dac_gain_table, block->voice_dac_gain_table,
           sizeof(block->voice_dac_gain_table));
    memcpy(app_dsp_cfg_data->voice_adc_gain_table, block->voice_adc_gain_table,
           sizeof(block->voice_adc_gain_table));
    memcpy(app_dsp_cfg_data->record_adc_gain_table, block->record_adc_gain_table,
           sizeof(block->record_adc_gain_table));
    memcpy(app_dsp_cfg_data->aux_dac_gain_table, block->aux_dac_gain_table,
           sizeof(block->aux_dac_gain_table));
    memcpy(app_dsp_cfg_data->aux_adc_gain_table, block->aux_adc_gain_table,
           sizeof(block->aux_adc_gain_table));
    memcpy(app_dsp_cfg_data->ringtone_dac_gain_table, block->ringtone_dac_gain_table,
           sizeof(block->ringtone_dac_gain_table));
    memcpy(app_dsp_cfg_data->vp_dac_gain_table, block->vp_dac_gain_table,
           sizeof(block->vp_dac_gain_table));
    memcpy(app_dsp_cfg_data->apt_dac_gain_table, block->apt_dac_gain_table,
           sizeof(block->apt_dac_gain_table));
    memcpy(app_dsp_cfg_data->apt_adc_gain_table, block->apt_adc_gain_table,
           sizeof(block->apt_adc_gain_table));
    memcpy(app_dsp_cfg_data->llapt_dac_gain_table, block->llapt_dac_gain_table,
           sizeof(block->llapt_dac_gain_table));
    memcpy(app_dsp_cfg_data->llapt_adc_gain_table, block->llapt_adc_gain_table,
           sizeof(block->llapt_adc_gain_table));
    memcpy(app_dsp_cfg_data->anc_dac_gain_table, block->anc_dac_gain_table,
           sizeof(block->anc_dac_gain_table));
    memcpy(app_dsp_cfg_data->anc_adc_gain_table, block->anc_adc_gain_table,
           sizeof(block->anc_adc_gain_table));
    memcpy(app_dsp_cfg_data->vad_adc_gain_table, block->vad_adc_gain_table,
           sizeof(block->vad_adc_gain_table));

#if F_APP_SIDETONE_SUPPORT
    //sidetone
    app_dsp_cfg_sidetone.gain = block->hw_sidetone_digital_gain;
    app_dsp_cfg_sidetone.hw_enable = block->hw_sidetone_enable;
    app_dsp_cfg_sidetone.hpf_level = block->hw_sidetone_hpf_level;
#endif

    free(block);
    return;

fail_check:
    free(block);
fail_calloc:
    APP_PRINT_ERROR1("app_dsp_cfg_gain_table_load: failed %d", -ret);
}

static bool app_dsp_cfg_eq_param_load(void)
{
    uint32_t                   data_offset;
    int32_t                    ret = 0;
    T_APP_DSP_EQ_PARAM_HEADER  *header = NULL;
    T_APP_DSP_EQ_SPK_HEADER    *spk_header = NULL;
    T_APP_DSP_EQ_MIC_HEADER    *mic_header = NULL;
    T_APP_DSP_EQ_SPK_HEADER    *voice_eq_header = NULL;
    uint32_t eq_spk_application_num;
    uint32_t eq_mic_application_num;
    uint8_t                     mode;

#if F_APP_EXT_MIC_SWITCH_SUPPORT
    static T_APP_DSP_EQ_PARAM_HEADER *prev_eq_param_header = NULL;

    if (prev_eq_param_header == NULL)
    {
        app_dsp_cfg_data->eq_param.header = (T_APP_DSP_EQ_PARAM_HEADER *)calloc(1,
                                                                                sizeof(T_APP_DSP_EQ_PARAM_HEADER));
        prev_eq_param_header = app_dsp_cfg_data->eq_param.header;
    }
    else
    {
        app_dsp_cfg_data->eq_param.header = prev_eq_param_header;
    }
#else
    app_dsp_cfg_data->eq_param.header = calloc(1, sizeof(T_APP_DSP_EQ_PARAM_HEADER));
#endif

    header = app_dsp_cfg_data->eq_param.header;
    if (header == NULL)
    {
        ret = 1;
        goto fail_alloc_eq_param_head;
    }

    data_offset = app_dsp_cfg_data->dsp_cfg_header.eq_block_offset;
    fmc_flash_nor_read(app_dsp_cfg_get_dsp_config_addr() + data_offset,
                       header, sizeof(T_APP_DSP_EQ_PARAM_HEADER) - 16);

    if (header->sync_word != DSP_DATA_SYNC_WORD)
    {
        ret = 2;
        goto fail_load_eq_header;
    }

    data_offset += sizeof(T_APP_DSP_EQ_PARAM_HEADER) - 16;

    eq_spk_application_num = header->eq_spk_application_num;

#if F_APP_EXT_MIC_SWITCH_SUPPORT
    static T_APP_DSP_EQ_SPK_HEADER *prev_eq_spk_application_header = NULL;

    if (prev_eq_spk_application_header == NULL)
    {
        header->eq_spk_application_header = (T_APP_DSP_EQ_SPK_HEADER *)calloc(1,
                                                                              eq_spk_application_num * sizeof(T_APP_DSP_EQ_SPK_HEADER));
        prev_eq_spk_application_header = header->eq_spk_application_header;
    }
    else
    {
        header->eq_spk_application_header = prev_eq_spk_application_header;
    }
#else
    header->eq_spk_application_header = calloc(1,
                                               eq_spk_application_num * sizeof(T_APP_DSP_EQ_SPK_HEADER));
#endif
    spk_header = header->eq_spk_application_header;
    if (spk_header == NULL)
    {
        ret = 3;
        goto fail_alloc_spk_info;
    }

    fmc_flash_nor_read(app_dsp_cfg_get_dsp_config_addr() + data_offset,
                       spk_header, header->eq_spk_application_num * sizeof(T_APP_DSP_EQ_SPK_HEADER));

    data_offset += header->eq_spk_application_num * sizeof(T_APP_DSP_EQ_SPK_HEADER);

    eq_mic_application_num = header->eq_mic_application_num;

#if F_APP_EXT_MIC_SWITCH_SUPPORT
    static T_APP_DSP_EQ_MIC_HEADER *prev_eq_mic_application_header = NULL;

    if (prev_eq_mic_application_header == NULL)
    {
        header->eq_mic_application_header = (T_APP_DSP_EQ_MIC_HEADER *)calloc(1,
                                                                              eq_mic_application_num * sizeof(T_APP_DSP_EQ_MIC_HEADER));
        prev_eq_mic_application_header = header->eq_mic_application_header;
    }
    else
    {
        header->eq_mic_application_header = prev_eq_mic_application_header;
    }
#else
    header->eq_mic_application_header = calloc(1,
                                               eq_mic_application_num * sizeof(T_APP_DSP_EQ_MIC_HEADER));
#endif

    mic_header = header->eq_mic_application_header;
    if (mic_header == NULL)
    {
        ret = 4;
        goto fail_alloc_mic_info;
    }
    fmc_flash_nor_read(app_dsp_cfg_get_dsp_config_addr() + data_offset,
                       mic_header, header->eq_mic_application_num * sizeof(T_APP_DSP_EQ_MIC_HEADER));

    data_offset += header->eq_mic_application_num * sizeof(T_APP_DSP_EQ_MIC_HEADER);

    if (header->voice_eq_applications_num != 0xff)
    {
#if F_APP_EXT_MIC_SWITCH_SUPPORT
        static T_APP_DSP_EQ_SPK_HEADER *prev_voice_eq_application_header = NULL;

        if (prev_voice_eq_application_header == NULL)
        {
            header->voice_eq_application_header = (T_APP_DSP_EQ_SPK_HEADER *)calloc(1,
                                                                                    header->voice_eq_applications_num * sizeof(T_APP_DSP_EQ_SPK_HEADER));
            prev_voice_eq_application_header = header->voice_eq_application_header;
        }
        else
        {
            header->voice_eq_application_header = prev_voice_eq_application_header;
        }
#else
        header->voice_eq_application_header = calloc(1,
                                                     header->voice_eq_applications_num * sizeof(T_APP_DSP_EQ_SPK_HEADER));

#endif
        voice_eq_header = header->voice_eq_application_header;
        if (voice_eq_header == NULL)
        {
            ret = 6;
            goto fail_alloc_voice_eq_info;
        }
        fmc_flash_nor_read(app_dsp_cfg_get_dsp_config_addr() + data_offset,
                           voice_eq_header, header->voice_eq_applications_num * sizeof(T_APP_DSP_EQ_SPK_HEADER));

        data_offset += header->voice_eq_applications_num * sizeof(T_APP_DSP_EQ_SPK_HEADER);

        eq_utils_bitmask_init(SPK_SW_EQ, VOICE_SPK_MODE, voice_eq_header->spk_eq_idx_bitmask);
        eq_utils_bitmask_init(MIC_SW_EQ, VOICE_MIC_MODE,
                              voice_eq_header->spk_eq_idx_bitmask); // spk and mic share
    }

#if F_APP_EXT_MIC_SWITCH_SUPPORT
    static T_APP_DSP_EQ_SUB_PARAM_HEADER *prev_sub_header = NULL;

    if (prev_sub_header == NULL)
    {
        header->sub_header = (T_APP_DSP_EQ_SUB_PARAM_HEADER *)calloc(1,
                                                                     header->eq_num * sizeof(T_APP_DSP_EQ_SUB_PARAM_HEADER));
        prev_sub_header = header->sub_header;
    }
    else
    {
        header->sub_header = prev_sub_header;
    }
#else
    header->sub_header = calloc(1, header->eq_num * sizeof(T_APP_DSP_EQ_SUB_PARAM_HEADER));
#endif

    if (header->sub_header == NULL)
    {
        ret = 5;
        goto fail_alloc_sub_header_info;
    }
    fmc_flash_nor_read(app_dsp_cfg_get_dsp_config_addr() + data_offset,
                       header->sub_header, header->eq_num * sizeof(T_APP_DSP_EQ_SUB_PARAM_HEADER));

    for (mode = 0; mode < SPK_EQ_MODE_MAX; mode++)
    {
        if (mode != VOICE_SPK_MODE)
        {
            eq_utils_bitmask_init(SPK_SW_EQ, mode, header->eq_spk_application_header[mode].spk_eq_idx_bitmask);
        }
    }

    for (mode = 0; mode < MIC_EQ_MODE_MAX; mode++)
    {
        if (mode != VOICE_MIC_MODE)
        {
            eq_utils_bitmask_init(MIC_SW_EQ, mode, header->eq_mic_application_header[mode].mic_eq_idx_bitmask);
        }
    }

    return true;

fail_alloc_sub_header_info:
    free(voice_eq_header);
fail_alloc_voice_eq_info:
    free(mic_header);
fail_alloc_mic_info:
    free(spk_header);
fail_alloc_spk_info:
fail_load_eq_header:
    free(header);
fail_alloc_eq_param_head:
    APP_PRINT_ERROR1("app_dsp_cfg_eq_param_load: fail %d", -ret);
    return false;
}

static bool app_dsp_cfg_extensible_param_load(void)
{
    uint32_t data_offset;
    uint16_t index = 0;
    int32_t  ret = 0;
    T_APP_DSP_EXTENSIBLE_PARAM *header = NULL;

    header = (T_APP_DSP_EXTENSIBLE_PARAM *)(&app_dsp_cfg_data->dsp_extensible_param);
    header->sync_word = 0;

    if (app_dsp_cfg_data->dsp_cfg_header.extensible_param_offset == 0)
    {
        //for DSP configuration bin compatibility
        ret = 1;
        goto extensible_invalid_address;
    }

    data_offset = app_dsp_cfg_data->dsp_cfg_header.extensible_param_offset;
    fmc_flash_nor_read((app_dsp_cfg_get_dsp_config_addr() +
                        data_offset),
                       (uint8_t *)header,
                       sizeof(header->sync_word) + sizeof(header->extensible_len));

    if (header->sync_word != DSP_DATA_SYNC_WORD || header->extensible_len == 0)
    {
        ret = 2;
        goto extensible_param_header_fail;
    }

    data_offset += sizeof(header->sync_word);
    data_offset += sizeof(header->extensible_len);

    header->raw_data = (uint8_t *)calloc(1, header->extensible_len);

    if (header->raw_data == NULL)
    {
        ret = 3;
        goto extensible_raw_data_alloc_fail;
    }

    fmc_flash_nor_read((app_dsp_cfg_get_dsp_config_addr() +
                        data_offset),
                       header->raw_data, header->extensible_len);

    while (index < header->extensible_len)
    {
        uint16_t type;
        uint16_t length;

        LE_ARRAY_TO_UINT16(type, &header->raw_data[index]);
        index += 2;

        LE_ARRAY_TO_UINT16(length, &header->raw_data[index]);
        index += 2;

        switch (type)
        {
#if F_APP_BRIGHTNESS_SUPPORT
        case APP_DSP_EXTENSIBLE_PARAM_BRIGHTNESS:
            {
                T_APP_DSP_EXTENSIBLE_BRIGHTNESS *ext_data = (T_APP_DSP_EXTENSIBLE_BRIGHTNESS *)
                                                            &header->raw_data[index];

                dsp_config_support_brightness = true;
                brightness_level_max = ext_data->brightness_level_max;
                brightness_level_min = 0;
                brightness_level_default = ext_data->brightness_level_default;
                memcpy(brightness_gain_table, &ext_data->brightness_gain_table[0],
                       sizeof(ext_data->brightness_gain_table));
            }
            break;
#endif

        default:
            break;
        }

        index += length;
    }

    free(header->raw_data);

    return true;

extensible_invalid_address:
extensible_raw_data_alloc_fail:
extensible_param_header_fail:

    APP_PRINT_ERROR1("app_dsp_cfg_extensible_param_load: fail %d", -ret);
    return false;
}

void app_dsp_cfg_init(bool is_normal_apt)
{
    app_dsp_cfg_load_header();
    app_dsp_cfg_eq_param_load();
    app_dsp_cfg_extensible_param_load();
    app_dsp_cfg_gain_table_load(is_normal_apt);
}

