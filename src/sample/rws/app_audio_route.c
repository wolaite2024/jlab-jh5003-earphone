/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include <stdlib.h>
#include "trace.h"
#include "anc.h"
#include "audio_passthrough.h"
#include "app_dsp_cfg.h"
#include "app_cfg.h"
#include "app_audio_route.h"
#if F_APP_TEAMS_FEATURE_SUPPORT
#include "app_teams_cmd.h"
#endif

#if F_APP_DURIAN_SUPPORT
#include "app_durian.h"
#endif

void app_audio_route_path_bind(T_AUDIO_CATEGORY      category,
                               T_AUDIO_TRACK_HANDLE  handle,
                               T_AUDIO_ROUTE_PATH    path)
{
    if (category == AUDIO_CATEGORY_ANC)
    {
        anc_route_bind(path);
    }
    else if (category == AUDIO_CATEGORY_APT)
    {
        audio_passthrough_route_bind(AUDIO_PASSTHROUGH_MODE_NORMAL, path);
    }
    else if (category == AUDIO_CATEGORY_LLAPT)
    {
        audio_passthrough_route_bind(AUDIO_PASSTHROUGH_MODE_LOW_LATENCY, path);
    }
    else
    {
        audio_track_route_bind(handle, path);
    }
}

void app_audio_route_path_unbind(T_AUDIO_CATEGORY     category,
                                 T_AUDIO_TRACK_HANDLE handle)
{
    if (category == AUDIO_CATEGORY_ANC)
    {
        anc_route_unbind();
    }
    else if (category == AUDIO_CATEGORY_APT)
    {
        audio_passthrough_route_unbind(AUDIO_PASSTHROUGH_MODE_NORMAL);
    }
    else if (category == AUDIO_CATEGORY_LLAPT)
    {
        audio_passthrough_route_unbind(AUDIO_PASSTHROUGH_MODE_LOW_LATENCY);
    }
    else
    {
        audio_track_route_unbind(handle);
    }
}

bool app_audio_route_entry_get(T_AUDIO_CATEGORY       category,
                               uint32_t               device,
                               T_AUDIO_ROUTE_IO_TYPE  io_type,
                               T_AUDIO_ROUTE_ENTRY   *entry)
{
    T_AUDIO_ROUTE_PATH path;

    path = audio_route_path_take(category,
                                 device);

    if (path.entry != NULL)
    {
        uint8_t i;

        for (i = 0; i < path.entry_num; i++)
        {
            if (path.entry[i].io_type == io_type)
            {
                *entry = path.entry[i];
                break;
            }
        }

        audio_route_path_give(&path);

        return true;
    }

    return false;
}

void app_audio_route_entry_update(T_AUDIO_CATEGORY      category,
                                  uint32_t              device,
                                  T_AUDIO_TRACK_HANDLE  handle,
                                  uint8_t               entry_num,
                                  T_AUDIO_ROUTE_ENTRY   entry[])
{
    T_AUDIO_ROUTE_PATH path;
    uint8_t i, j;

    APP_PRINT_TRACE1("app_audio_route_entry_update: category 0x%x", category);

    app_audio_route_path_unbind(category, handle);

    path = audio_route_path_take(category,
                                 device);

    if (path.entry_num == 0)
    {
        return;
    }

    for (i = 0; i < path.entry_num; i++)
    {
        for (j = 0; j < entry_num; j++)
        {
            if (path.entry[i].io_type == entry[j].io_type)
            {
                if (category == AUDIO_CATEGORY_VOICE)
                {
                    path.entry[i].endpoint_attr.mic.id     = entry[j].endpoint_attr.mic.id;
                    path.entry[i].endpoint_attr.mic.type   = entry[j].endpoint_attr.mic.type;
                }
                else if (category == AUDIO_CATEGORY_ANC)
                {
                    // do not update adc_ch
                    T_AUDIO_ROUTE_ADC_CHANN adc_ch         = path.entry[i].endpoint_attr.mic.adc_ch;
                    path.entry[i].endpoint_attr.mic        = entry[j].endpoint_attr.mic;
                    path.entry[i].endpoint_attr.mic.adc_ch = adc_ch;
                }
                break;
            }
        }
    }

    app_audio_route_path_bind(category, handle, path);
    audio_route_path_give(&path);
}

bool app_audio_route_dac_gain_set(T_AUDIO_CATEGORY category, uint8_t level, uint16_t gain)
{
    switch (category)
    {
    case AUDIO_CATEGORY_AUDIO:
        {
            if (level <= app_dsp_cfg_vol.playback_volume_max)
            {
                app_dsp_cfg_data->audio_dac_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_VOICE:
        {
            if (level <= app_dsp_cfg_vol.voice_out_volume_max)
            {
                app_dsp_cfg_data->voice_dac_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_LINE:
        {
            if (level <= app_dsp_cfg_vol.line_in_volume_out_max)
            {
                app_dsp_cfg_data->aux_dac_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_TONE:
        {
            if (level <= app_dsp_cfg_vol.ringtone_volume_max)
            {
                app_dsp_cfg_data->ringtone_dac_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_VP:
        {
            if (level <= app_dsp_cfg_vol.voice_prompt_volume_max)
            {
                app_dsp_cfg_data->vp_dac_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_APT:
        {
            if (level <= app_dsp_cfg_vol.apt_volume_out_max)
            {
                app_dsp_cfg_data->apt_dac_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_LLAPT:
        {
            if (level <= app_dsp_cfg_vol.apt_volume_out_max)
            {
                app_dsp_cfg_data->llapt_dac_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    default:
        return false;
    }

    return true;
}

bool app_audio_route_adc_gain_set(T_AUDIO_CATEGORY category, uint8_t level, uint16_t gain)
{
    switch (category)
    {
    case AUDIO_CATEGORY_VOICE:
        {
            if (level <= app_dsp_cfg_vol.voice_volume_in_max)
            {
                app_dsp_cfg_data->voice_adc_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_RECORD:
        {
            if (level <= app_dsp_cfg_vol.record_volume_max)
            {
                app_dsp_cfg_data->record_adc_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_LINE:
        {
            if (level <= app_dsp_cfg_vol.line_in_volume_in_max)
            {
                app_dsp_cfg_data->aux_adc_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_APT:
        {
            if (level <= app_dsp_cfg_vol.apt_volume_in_max)
            {
                app_dsp_cfg_data->apt_adc_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_LLAPT:
        {
            if (level <= app_dsp_cfg_vol.apt_volume_in_max)
            {
                app_dsp_cfg_data->llapt_adc_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_VAD:
        {
            if (level <= app_dsp_cfg_vol.vad_volume_max)
            {
                app_dsp_cfg_data->vad_adc_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    default:
        return false;
    }

    return true;
}

static bool app_audio_route_dac_gain_get_cback(T_AUDIO_CATEGORY category, uint32_t level,
                                               T_AUDIO_ROUTE_DAC_GAIN *gain)
{
    switch (category)
    {
    case AUDIO_CATEGORY_AUDIO:
        {
#if F_APP_DURIAN_SUPPORT
            bool dac_set = false;
            dac_set = app_durian_audio_dac_gain_set(level, gain);

            if (!dac_set)
            {
                return false;
            }
#else
            if (level <= app_dsp_cfg_vol.playback_volume_max)
            {
#if F_APP_TEAMS_FEATURE_SUPPORT
                if ((level < app_dsp_cfg_vol.playback_volume_max) || (teams_dac_gain == 0))
                {
                    gain->left_gain = app_dsp_cfg_data->audio_dac_gain_table[level] + teams_dac_gain;
                    gain->right_gain = app_dsp_cfg_data->audio_dac_gain_table[level] + teams_dac_gain;
                }
                else
                {
                    gain->left_gain = app_dsp_cfg_data->audio_dac_gain_table[level - 1] + teams_dac_gain + 384;
                    gain->right_gain = app_dsp_cfg_data->audio_dac_gain_table[level - 1] + teams_dac_gain + 384;
                }
#else
                gain->left_gain = app_dsp_cfg_data->audio_dac_gain_table[level];
                gain->right_gain = app_dsp_cfg_data->audio_dac_gain_table[level];
#endif
            }
            else
            {
                return false;
            }
#endif
        }
        break;

    case AUDIO_CATEGORY_VOICE:
        {
            if (level <= app_dsp_cfg_vol.voice_out_volume_max)
            {
#if F_APP_TEAMS_FEATURE_SUPPORT
                if ((level < app_dsp_cfg_vol.voice_out_volume_max) || (teams_dac_gain == 0))
                {
                    gain->left_gain = app_dsp_cfg_data->voice_dac_gain_table[level] + teams_dac_gain;
                    gain->right_gain = app_dsp_cfg_data->voice_dac_gain_table[level] + teams_dac_gain;
                }
                else
                {
                    gain->left_gain = app_dsp_cfg_data->voice_dac_gain_table[level - 1] + teams_dac_gain + 384;
                    gain->right_gain = app_dsp_cfg_data->voice_dac_gain_table[level - 1] + teams_dac_gain + 384;
                }
#else
                gain->left_gain = app_dsp_cfg_data->voice_dac_gain_table[level];
                gain->right_gain = app_dsp_cfg_data->voice_dac_gain_table[level];
#endif
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_LINE:
        {
            if (level <= app_dsp_cfg_vol.line_in_volume_out_max)
            {
                gain->left_gain = app_dsp_cfg_data->aux_dac_gain_table[level];
                gain->right_gain = app_dsp_cfg_data->aux_dac_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_TONE:
        {
            if (level <= app_dsp_cfg_vol.ringtone_volume_max)
            {
                gain->left_gain = app_dsp_cfg_data->ringtone_dac_gain_table[level];
                gain->right_gain = app_dsp_cfg_data->ringtone_dac_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_VP:
        {
            if (level <= app_dsp_cfg_vol.voice_prompt_volume_max)
            {
                gain->left_gain = app_dsp_cfg_data->vp_dac_gain_table[level];
                gain->right_gain = app_dsp_cfg_data->vp_dac_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_APT:
        {
            if (level <= app_dsp_cfg_vol.apt_volume_out_max)
            {
                gain->left_gain = app_dsp_cfg_data->apt_dac_gain_table[level];
                gain->right_gain = app_dsp_cfg_data->apt_dac_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_LLAPT:
        {
            if (level <= app_dsp_cfg_vol.apt_volume_out_max)
            {
                gain->left_gain = app_dsp_cfg_data->llapt_dac_gain_table[level];
                gain->right_gain = app_dsp_cfg_data->llapt_dac_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    default:
        return false;
    }
    return true;
}

static bool app_audio_route_adc_gain_get_cback(T_AUDIO_CATEGORY category, uint32_t level,
                                               T_AUDIO_ROUTE_ADC_GAIN *gain)
{
    switch (category)
    {
    case AUDIO_CATEGORY_VOICE:
        {
            if (level <= app_dsp_cfg_vol.voice_volume_in_max)
            {
                gain->left_gain = app_dsp_cfg_data->voice_adc_gain_table[level];
                gain->right_gain = app_dsp_cfg_data->voice_adc_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_RECORD:
        {
            if (level <= app_dsp_cfg_vol.record_volume_max)
            {
                gain->left_gain = app_dsp_cfg_data->record_adc_gain_table[level];
                gain->right_gain = app_dsp_cfg_data->record_adc_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_LINE:
        {
            if (level <= app_dsp_cfg_vol.line_in_volume_in_max)
            {
                gain->left_gain = app_dsp_cfg_data->aux_adc_gain_table[level];
                gain->right_gain = app_dsp_cfg_data->aux_adc_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_APT:
        {
            if (level <= app_dsp_cfg_vol.apt_volume_in_max)
            {
                gain->left_gain = app_dsp_cfg_data->apt_adc_gain_table[level];
                gain->right_gain = app_dsp_cfg_data->apt_adc_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_LLAPT:
        {
            if (level <= app_dsp_cfg_vol.apt_volume_in_max)
            {
                gain->left_gain = app_dsp_cfg_data->llapt_adc_gain_table[level];
                gain->right_gain = app_dsp_cfg_data->llapt_adc_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_VAD:
        {
            if (level <= app_dsp_cfg_vol.vad_volume_max)
            {
                gain->left_gain = app_dsp_cfg_data->vad_adc_gain_table[level];
                gain->right_gain = app_dsp_cfg_data->vad_adc_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    default:
        return false;
    }

    return true;
}

void app_audio_route_gain_init(void)
{
    uint8_t index;

    for (index = 0; index < AUDIO_CATEGORY_NUM; index++)
    {
        audio_route_gain_register((T_AUDIO_CATEGORY)index, app_audio_route_dac_gain_get_cback,
                                  app_audio_route_adc_gain_get_cback);
    }
}

void app_audio_route_anc_llapt_coexist_register(void)
{
    T_AUDIO_ROUTE_PATH  llapt_path;
    T_AUDIO_ROUTE_PATH  anc_path;
    T_AUDIO_ROUTE_PATH  update_path;

    llapt_path = audio_route_path_take(AUDIO_CATEGORY_LLAPT,
                                       AUDIO_DEVICE_OUT_SPK | AUDIO_DEVICE_IN_MIC);
    if (llapt_path.entry_num == 0)
    {
        return ;
    }

    anc_path = audio_route_path_take(AUDIO_CATEGORY_ANC,
                                     AUDIO_DEVICE_OUT_SPK | AUDIO_DEVICE_IN_MIC);
    if (anc_path.entry_num == 0)
    {
        audio_route_path_give(&llapt_path);
        return ;
    }

    update_path.entry_num = llapt_path.entry_num;

    for (uint8_t i = 0; i < anc_path.entry_num; ++i)
    {
        if ((anc_path.entry[i].io_type == AUDIO_ROUTE_IO_ANC_FF_LEFT_IN) ||
            (anc_path.entry[i].io_type == AUDIO_ROUTE_IO_ANC_FF_RIGHT_IN) ||
            (anc_path.entry[i].io_type == AUDIO_ROUTE_IO_ANC_FB_LEFT_IN) ||
            (anc_path.entry[i].io_type == AUDIO_ROUTE_IO_ANC_FB_RIGHT_IN))
        {
            ++update_path.entry_num;
        }
    }

    update_path.entry = calloc(1, sizeof(T_AUDIO_ROUTE_ENTRY) * update_path.entry_num);
    if (update_path.entry != NULL)
    {
        uint8_t count = 0;

        memcpy(update_path.entry,
               llapt_path.entry,
               sizeof(T_AUDIO_ROUTE_ENTRY) * llapt_path.entry_num);

        for (uint8_t i = 0; i < anc_path.entry_num; ++i)
        {
            if ((anc_path.entry[i].io_type == AUDIO_ROUTE_IO_ANC_FF_LEFT_IN) ||
                (anc_path.entry[i].io_type == AUDIO_ROUTE_IO_ANC_FF_RIGHT_IN) ||
                (anc_path.entry[i].io_type == AUDIO_ROUTE_IO_ANC_FB_LEFT_IN) ||
                (anc_path.entry[i].io_type == AUDIO_ROUTE_IO_ANC_FB_RIGHT_IN))
            {
                memcpy(&(update_path.entry[llapt_path.entry_num + count]),
                       &(anc_path.entry[i]),
                       sizeof(T_AUDIO_ROUTE_ENTRY));

                ++count;
            }
        }

        app_audio_route_path_bind(AUDIO_CATEGORY_LLAPT, NULL, update_path);

        free(update_path.entry);
    }

    audio_route_path_give(&llapt_path);
    audio_route_path_give(&anc_path);
}

void app_audio_route_anc_llapt_coexist_unregister()
{
    app_audio_route_path_unbind(AUDIO_CATEGORY_LLAPT, NULL);
}
