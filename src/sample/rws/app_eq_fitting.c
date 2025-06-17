#if F_APP_SAIYAN_EQ_FITTING
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <stdlib.h>
#include <string.h>
#include "stdlib_corecrt.h"
#include "dfu_api.h"
#include "fmc_api.h"
#include "flash_map.h"
#include "trace.h"
#include "audio_type.h"
#include "audio.h"
#include "audio_route.h"
#include "audio_track.h"
#include "audio_probe.h"
#include "app_eq_fitting.h"
#include "app_cmd.h"
#include "app_report.h"
#include "app_main.h"
#include "app_a2dp.h"
#include "app_hfp.h"
#include "app_cfg.h"

#if F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
#include "app_data_capture.h"
#endif

#if F_APP_APT_SUPPORT
#include "app_audio_passthrough.h"
#endif

#define LO_BYTE(x)  ((uint8_t)(x))
#define HI_BYTE(x)  ((uint8_t)((x & 0xFF00) >> 8))

#define HW_EQ_MAGIC_WORD             0x51455748
#define MAX_EQ_TOOL_CMD_LENGTH       500
#define MAX_EQ_FITTING_SIZE          1024
#define EQ_STAGE_LEN                 20
#define FLASH_NOR_SECTOR_SIZE        0x1000
#define IDX_UNKNOWN                  0xFF
#define IDX_MAX                      0x04

#define SAMPLE_RATE_16K              16000
#define SAMPLE_RATE_44K              44100
#define SAMPLE_RATE_48K              48000
#define SAMPLE_RATE_96K              96000

typedef enum t_hw_eq_device_type
{
    HW_EQ_DEVICE_PRIMARY_MIC     = 0x00,
    HW_EQ_DEVICE_SECONDARY_MIC   = 0x01,
    HW_EQ_DEVICE_INTERNAL_MIC    = 0x03,
    HW_EQ_DEVICE_PRIMARY_SPEAKER = 0x07,
} T_HW_EQ_DEVICE_TYPE;

typedef enum t_hw_eq_test_status
{
    HW_EQ_EXIT_TEST_MODE,
    HW_EQ_ENTER_TEST_MODE,
} T_HW_EQ_TEST_STATUS;

typedef enum t_hw_eq_write_status
{
    HW_EQ_WRITE_SUCCESS            = 0,
    HW_EQ_WRITE_MALLOC_FAIL        = 1,
    HW_EQ_WRITE_GET_BP_LV_FAIL     = 2,
    HW_EQ_WRITE_SET_BP_LV_FAIL     = 3,
    HW_EQ_WRITE_WRONG_LEN          = 4,
    HW_EQ_WRITE_READ_FAIL          = 5,
    HW_EQ_WRITE_ERASE_FAIL         = 6,
    HW_EQ_WRITE_FAIL               = 7,
    HW_EQ_WRITE_WRONG_TRACK_STATE  = 8,
} T_HW_EQ_WRITE_STATUS;

typedef struct __attribute__((__packed__)) t_hw_eq_para
{
    uint8_t device_type;
    uint8_t rsv;
    uint32_t sample_rate;
    uint8_t stage_number;
    uint8_t para[0];     /* para len is stage_number * 20 bytes */
} T_HW_EQ_PARA;

typedef struct __attribute__((__packed__)) t_hw_eq_data
{
    uint32_t magic_word;
    uint8_t calibrated;
    uint16_t total_len;  /* total len of all groups of  */
    uint16_t crc16;      /* CRC calc range: all groups of eq para */
    T_HW_EQ_PARA eq_para[0];
} T_HW_EQ_DATA;

typedef struct t_hw_eq_rcvd_data
{
    uint16_t buf_idx;
    uint8_t app_idx;
    uint8_t cmd_path;
    uint16_t cmd_id;
    uint16_t event_id;

    T_HW_EQ_DATA hw_eq;
} T_HW_EQ_RCVD_DATA;

typedef struct t_hw_eq_info
{
    uint32_t sample_rate;
    uint16_t offset;
    uint16_t len;
    struct t_hw_eq_info *next;
} T_HW_EQ_INFO;

typedef struct t_hw_eq_config_header
{
    uint16_t sync_word;
    uint16_t reserved1;
    uint32_t tool_version;
    uint16_t user_version;
    uint16_t bin_version;
    uint32_t reserved2;
    uint32_t eq_block_offset;
    uint32_t gain_table_block_offset;
    uint32_t algo_block_offset;
    uint32_t eq_extend_info_offset;
    uint32_t hw_eq_block_offset;
    uint32_t extensible_param_offset;
    uint32_t extensible_param2_offset;
    uint32_t package_features;
} T_HW_EQ_CONFIG_HEADER;

typedef struct t_hw_eq_param_header
{
    uint8_t  eq_type;
    uint8_t  eq_chann;
    uint16_t param_length;
    uint32_t param_offset;
    uint32_t sample_rate;
} T_HW_EQ_PARAM_HEADER;

typedef struct t_hw_eq_block
{
    uint16_t sync_word;
    uint16_t reserved1;
    uint32_t eq_block_len;
    uint16_t eq_num;
    uint16_t reserved2;
    T_HW_EQ_PARAM_HEADER *param_header;
} T_HW_EQ_BLOCK;

static T_HW_EQ_RCVD_DATA *rcvd_hw_eq = NULL;
static bool is_test_mode = false;
static bool is_disallow_playback = false;
static uint8_t *test_mode_tmp_eq = NULL;
static uint8_t *test_mode_tmp_eq_advanced = NULL;
static uint32_t eq_block_offset = 0;
static T_HW_EQ_INFO *hw_eq_info[IDX_MAX];
static T_HW_EQ_BLOCK *hw_eq_block = NULL;
extern T_AUDIO_TRACK_HANDLE dsp_capture_data_audio_record_handle;
extern T_APP_CFG_CONST app_cfg_const;

static bool app_eq_fitting_is_media_buffer_idle(void)
{
    T_AUDIO_TRACK_STATE state = AUDIO_TRACK_STATE_RELEASED;
    T_APP_BR_LINK *p_link = app_link_find_br_link(app_db.br_link[app_a2dp_get_active_idx()].bd_addr);

    if (p_link == NULL || p_link->a2dp_track_handle == NULL)
    {
        return true;
    }

    audio_track_state_get(p_link->a2dp_track_handle, &state);

    if (state == AUDIO_TRACK_STATE_STARTED)
    {
        return false;
    }

    return true;
}

static void app_eq_fitting_stop_playback(void)
{
    T_APP_BR_LINK *p_link = app_link_find_br_link(app_db.br_link[app_a2dp_get_active_idx()].bd_addr);

    if (p_link && p_link->a2dp_track_handle)
    {
        audio_track_stop(p_link->a2dp_track_handle);
    }
}

static void app_eq_fitting_start_playback(void)
{
    T_APP_BR_LINK *p_link = app_link_find_br_link(app_db.br_link[app_a2dp_get_active_idx()].bd_addr);

    if (p_link && p_link->a2dp_track_handle && p_link->streaming_fg)
    {
        audio_track_start(p_link->a2dp_track_handle);
    }
}

static uint8_t app_eq_fitting_write_hw_eq(uint8_t *data, uint32_t len)
{
    uint8_t failed_cause = HW_EQ_WRITE_SUCCESS;
    bool resume_bp_lv = false;
    uint8_t old_bp_lv;
    uint8_t *flash_tem_buf = NULL;

    is_disallow_playback = true;

    if (!app_eq_fitting_is_media_buffer_idle())
    {
        failed_cause = HW_EQ_WRITE_WRONG_TRACK_STATE;
        goto exit;
    }

    /* need 4K for temporary storage, take it from media buffer */
    flash_tem_buf = (uint8_t *) audio_probe_media_buffer_malloc(FLASH_NOR_SECTOR_SIZE);
    if (!flash_tem_buf)
    {
        failed_cause = HW_EQ_WRITE_MALLOC_FAIL;
        goto exit;
    }

    if (!fmc_flash_nor_get_bp_lv(EQ_FITTING_ADDR, &old_bp_lv))
    {
        failed_cause = HW_EQ_WRITE_GET_BP_LV_FAIL;
        goto exit;
    }

    if (fmc_flash_nor_set_bp_lv(EQ_FITTING_ADDR, 0))
    {
        resume_bp_lv = true;
    }
    else
    {
        failed_cause = HW_EQ_WRITE_SET_BP_LV_FAIL;
        goto exit;
    }

    if (fmc_flash_nor_read(EQ_FITTING_ADDR, flash_tem_buf, FLASH_NOR_SECTOR_SIZE))
    {
        if (len <= EQ_FITTING_SIZE)
        {
            memcpy(flash_tem_buf, data, len);
        }
        else
        {
            failed_cause = HW_EQ_WRITE_WRONG_LEN;
            goto exit;
        }
    }
    else
    {
        failed_cause = HW_EQ_WRITE_READ_FAIL;
        goto exit;
    }

    if (!fmc_flash_nor_erase(EQ_FITTING_ADDR, FMC_FLASH_NOR_ERASE_SECTOR))
    {
        failed_cause = HW_EQ_WRITE_ERASE_FAIL;
        goto exit;
    }

    if (!fmc_flash_nor_write(EQ_FITTING_ADDR, flash_tem_buf, FLASH_NOR_SECTOR_SIZE))
    {
        failed_cause = HW_EQ_WRITE_FAIL;
        goto exit;
    }

exit:

    if (resume_bp_lv)
    {
        fmc_flash_nor_set_bp_lv(EQ_FITTING_ADDR, old_bp_lv);
    }

    if (flash_tem_buf)
    {
        audio_probe_media_buffer_free(flash_tem_buf);
    }

    if (failed_cause != HW_EQ_WRITE_WRONG_TRACK_STATE)
    {
        is_disallow_playback = false;
    }

    APP_PRINT_ERROR1("app_eq_fitting_write_hw_eq: cause %d", failed_cause);

    return failed_cause;
}

static bool app_eq_fitting_read_hw_eq(uint32_t offset, uint8_t *data, uint32_t len)
{
    if (!fmc_flash_nor_read(EQ_FITTING_ADDR + offset, data, len))
    {
        return false;
    }

    return true;
}

#if F_APP_APT_SUPPORT
static bool app_eq_fitting_is_dsp_apt_enabled(void)
{
    bool is_apt_on = (app_db.current_listening_state == ANC_OFF_NORMAL_APT_ON);

#if F_APP_SUPPORT_ANC_APT_COEXIST
    is_apt_on |= app_listening_is_anc_apt_on_state(app_db.current_listening_state);
#endif

    if (is_apt_on && !app_apt_is_busy())
    {
        return true;
    }
    else
    {
        return false;
    }
}
#endif

static bool app_eq_fitting_is_streaming(uint8_t stream_type)
{
    bool ret = false;
    void *track = NULL;

    if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
    {
        track = app_db.br_link[app_a2dp_get_active_idx()].a2dp_track_handle;
    }
    else if (stream_type == AUDIO_STREAM_TYPE_VOICE)
    {
        track = app_db.br_link[app_hfp_get_active_idx()].sco_track_handle;
    }
    else if (stream_type == AUDIO_STREAM_TYPE_RECORD)
    {
        track = dsp_capture_data_audio_record_handle;
    }

    if (track)
    {
        T_AUDIO_TRACK_STATE state;

        if (audio_track_state_get(track, &state))
        {
            if (state == AUDIO_TRACK_STATE_STARTED)
            {
                ret = true;
            }
        }
    }

    return ret;
}

static uint32_t app_eq_fitting_get_sample_rate(T_AUDIO_CATEGORY category)
{
    uint32_t sample_rate = 0;

    switch (category)
    {
    case AUDIO_CATEGORY_AUDIO:
        {
            T_APP_BR_LINK *link = &app_db.br_link[app_a2dp_get_active_idx()];

            sample_rate = link->a2dp_codec_info.sampling_frequency;
        }
        break;

    case AUDIO_CATEGORY_VOICE:
        {
            sample_rate = SAMPLE_RATE_16K;
        }
        break;

    case AUDIO_CATEGORY_APT:
        {
            if (app_cfg_const.apt_sample_rate)
            {
                sample_rate = SAMPLE_RATE_96K;
            }
            else
            {
                sample_rate = SAMPLE_RATE_48K;
            }
        }
        break;

    case AUDIO_CATEGORY_RECORD:
        {
            sample_rate = SAMPLE_RATE_48K;
        }
        break;

    default:
        {
            sample_rate = 0;
        }
        break;
    }

    APP_PRINT_TRACE2("app_eq_fitting_get_sample_rate: category %x, sample_rate %d", category,
                     sample_rate);

    return sample_rate;
}

static uint8_t app_eq_fitting_get_eq_channel(T_AUDIO_CATEGORY category)
{
    T_AUDIO_ROUTE_PATH path;
    uint8_t i;
    uint8_t channel = 0;

    path = audio_route_path_take(category,
                                 AUDIO_DEVICE_OUT_SPK | AUDIO_DEVICE_IN_MIC);

    for (i = 0; i < path.entry_num; i++)
    {
        T_AUDIO_ROUTE_ENTRY *p_entry = &path.entry[i];
        T_AUDIO_ROUTE_IO_TYPE io_type = p_entry->io_type;
        T_AUDIO_ROUTE_ENDPOINT_TYPE endpoint_type = p_entry->endpoint_type;

        if ((endpoint_type == AUDIO_ROUTE_ENDPOINT_SPK) && (io_type == AUDIO_ROUTE_IO_AUDIO_PRIMARY_OUT))
        {
            channel = p_entry->endpoint_attr.spk.dac_ch;
            break;
        }
        else if ((endpoint_type == AUDIO_ROUTE_ENDPOINT_MIC) &&
                 (io_type == AUDIO_ROUTE_IO_VOICE_PRIMARY_IN ||
                  io_type == AUDIO_ROUTE_IO_VOICE_SECONDARY_IN))
        {
            channel = p_entry->endpoint_attr.mic.adc_ch;
            break;
        }
    }

    audio_route_path_give(&path);

    APP_PRINT_TRACE1("app_eq_fitting_get_eq_channel: channel %d", channel);

    return channel;
}

static uint8_t app_eq_fitting_get_io_idx(uint8_t io_type)
{
    uint8_t io_idx = IDX_UNKNOWN;

    switch (io_type)
    {
    case AUDIO_ROUTE_IO_RECORD_PRIMARY_IN:
    case AUDIO_ROUTE_IO_VOICE_PRIMARY_IN:
#if F_APP_APT_SUPPORT
    case AUDIO_ROUTE_IO_APT_FRONT_LEFT_IN:
#endif
        io_idx = 0;
        break;

    case AUDIO_ROUTE_IO_RECORD_SECONDARY_IN:
    case AUDIO_ROUTE_IO_VOICE_SECONDARY_IN:
#if F_APP_APT_SUPPORT
    case AUDIO_ROUTE_IO_APT_FF_LEFT_IN:
#endif
        io_idx = 1;
        break;

#if F_APP_APT_SUPPORT
    case AUDIO_ROUTE_IO_APT_FB_LEFT_IN:
        io_idx = 2;
        break;
#endif

    case AUDIO_ROUTE_IO_AUDIO_PRIMARY_OUT:
    case AUDIO_ROUTE_IO_VOICE_PRIMARY_OUT:
#if F_APP_APT_SUPPORT
    case AUDIO_ROUTE_IO_APT_PRIMARY_OUT:
#endif
        io_idx = 3;
        break;

    default:
        break;
    }

    APP_PRINT_TRACE2("app_eq_fitting_get_io_idx: io_type 0x%x, io_idx 0x%x", io_type,
                     io_idx);
    return io_idx;
}

static uint8_t app_eq_fitting_get_device_idx(uint8_t device_type)
{
    uint8_t device_idx = IDX_UNKNOWN;

    switch (device_type)
    {
    case HW_EQ_DEVICE_PRIMARY_MIC:
        device_idx = 0;
        break;

    case HW_EQ_DEVICE_SECONDARY_MIC:
        device_idx = 1;
        break;

    case HW_EQ_DEVICE_INTERNAL_MIC:
        device_idx = 2;
        break;

    case HW_EQ_DEVICE_PRIMARY_SPEAKER:
        device_idx = 3;
        break;

    default:
        break;
    }

    APP_PRINT_TRACE2("app_eq_fitting_get_device_idx: device_type 0x%x, device_idx 0x%x", device_type,
                     device_idx);
    return device_idx;
}

static bool app_eq_fitting_send_hw_eq(T_AUDIO_CATEGORY category)
{
    T_AUDIO_ROUTE_PATH path;
    uint8_t i;
    uint32_t sample_rate = app_eq_fitting_get_sample_rate(category);

    path = audio_route_path_take(category,
                                 AUDIO_DEVICE_OUT_SPK | AUDIO_DEVICE_IN_MIC);

    for (i = 0; i < path.entry_num; i++)
    {
        T_AUDIO_ROUTE_ENTRY *p_entry = &path.entry[i];
        uint8_t io_idx = app_eq_fitting_get_io_idx(p_entry->io_type);
        T_AUDIO_ROUTE_ENDPOINT_TYPE endpoint_type = p_entry->endpoint_type;
        T_AUDIO_ROUTE_HW_EQ_TYPE eq_config_path = AUDIO_ROUTE_HW_EQ_NUM;
        uint8_t channel;

#if F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
        T_MIC_INDEX bit_mask = app_data_capture_mic_index_get();

        if ((bit_mask & BIT0) || (bit_mask & BIT3)) // FF_MIC or SECONDARY_VOICE_MIC
        {
            io_idx = 1;
        }
        else if (bit_mask & BIT1) // FB_MIC
        {
            io_idx = 2;
        }
        else if ((bit_mask & BIT2)) // VOICE_MIC
        {
            io_idx = 0;
        }
#endif

        if (endpoint_type == AUDIO_ROUTE_ENDPOINT_MIC)
        {
            eq_config_path = AUDIO_ROUTE_HW_EQ_MIC;
            channel = p_entry->endpoint_attr.mic.adc_ch;
#if F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
            if (bit_mask)
            {
                channel = 0; // adc0
            }
#endif
        }
        else if (endpoint_type == AUDIO_ROUTE_ENDPOINT_SPK)
        {
            eq_config_path = AUDIO_ROUTE_HW_EQ_SPK;
            channel = p_entry->endpoint_attr.spk.dac_ch;
        }

        if (eq_config_path != AUDIO_ROUTE_HW_EQ_NUM &&
            (io_idx < sizeof(hw_eq_info) / sizeof(T_HW_EQ_INFO *)))
        {
            T_HW_EQ_INFO *tmp = hw_eq_info[io_idx];

            while (tmp)
            {
                if (tmp->sample_rate == sample_rate)
                {
                    T_HW_EQ_PARA *buf = malloc(tmp->len);

                    if (buf)
                    {
                        if (app_eq_fitting_read_hw_eq(sizeof(T_HW_EQ_DATA) + tmp->offset, (uint8_t *)buf, tmp->len))
                        {
                            audio_route_hw_eq_apply(eq_config_path, channel, (uint8_t *)buf->para,
                                                    buf->stage_number * EQ_STAGE_LEN);
                        }
                        free(buf);
                    }

                    break;
                }
                tmp = tmp->next;
            }
        }
    }

    APP_PRINT_TRACE2("app_eq_fitting_send_hw_eq: category %x, path_num %d", category,
                     path.entry_num);

    audio_route_path_give(&path);

    return true;
}

static void app_eq_fitting_hw_eq_clear(void)
{
#if F_APP_APT_SUPPORT
    if (app_eq_fitting_is_dsp_apt_enabled())
    {
        audio_route_hw_eq_apply(AUDIO_ROUTE_HW_EQ_MIC, 0, NULL, 0);
        audio_route_hw_eq_apply(AUDIO_ROUTE_HW_EQ_MIC, 1, NULL, 0);
        audio_route_hw_eq_apply(AUDIO_ROUTE_HW_EQ_MIC, 2, NULL, 0);
    }
#endif
}

static void app_eq_fitting_hw_eq_clear_advanced(T_AUDIO_STREAM_TYPE stream_type)
{
    if (stream_type == AUDIO_STREAM_TYPE_VOICE ||
        stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
    {
        if (app_eq_fitting_is_streaming(stream_type))
        {
            audio_route_hw_eq_apply(AUDIO_ROUTE_HW_EQ_SPK, 0, NULL, 0);
        }
    }

    if (stream_type == AUDIO_STREAM_TYPE_VOICE ||
        stream_type == AUDIO_STREAM_TYPE_RECORD)
    {
        if (app_eq_fitting_is_streaming(stream_type))
        {
            audio_route_hw_eq_apply(AUDIO_ROUTE_HW_EQ_MIC, 0, NULL, 0);
            audio_route_hw_eq_apply(AUDIO_ROUTE_HW_EQ_MIC, 1, NULL, 0);
            audio_route_hw_eq_apply(AUDIO_ROUTE_HW_EQ_MIC, 2, NULL, 0);
        }
    }
}

static void app_eq_fitting_free_eq_info(void)
{
    uint8_t i;

    for (i = 0; i < sizeof(hw_eq_info) / sizeof(T_HW_EQ_INFO *); i++)
    {
        if (hw_eq_info[i])
        {
            T_HW_EQ_INFO *next, *curr;

            curr = hw_eq_info[i];

            while (curr)
            {
                next = curr->next;
                free(curr);
                curr = next;
            }

            hw_eq_info[i] = NULL;
        }
    }
}

static void app_eq_fitting_init_eq_info(void)
{
    T_HW_EQ_DATA hw_eq_data;
    uint8_t buf[MAX_EQ_FITTING_SIZE];
    uint16_t offset = 0;
    uint16_t read_len;
    uint16_t eq_len;
    uint16_t remain_size;
    T_HW_EQ_INFO *tmp;
    T_HW_EQ_PARA *eq_para;
    uint8_t device_idx;

    memset(&hw_eq_info, 0, sizeof(hw_eq_info));

    if (app_eq_fitting_read_hw_eq(0, (uint8_t *)&hw_eq_data, sizeof(hw_eq_data)))
    {
        if ((hw_eq_data.magic_word != HW_EQ_MAGIC_WORD) || (hw_eq_data.calibrated != true) ||
            (hw_eq_data.total_len == 0))
        {
            /* not hw eq */
            return;
        }

        while (offset < hw_eq_data.total_len)
        {
            read_len = (sizeof(buf) > (hw_eq_data.total_len - offset)) ? (hw_eq_data.total_len - offset) :
                       sizeof(buf);

            if (app_eq_fitting_read_hw_eq(sizeof(T_HW_EQ_DATA) + offset, buf, read_len))
            {
                remain_size = read_len;

                while (remain_size >= sizeof(T_HW_EQ_PARA))
                {
                    eq_para = (T_HW_EQ_PARA *) &buf[read_len - remain_size];
                    eq_len = sizeof(T_HW_EQ_PARA) + eq_para->stage_number * EQ_STAGE_LEN;

                    device_idx = app_eq_fitting_get_device_idx(eq_para->device_type);
                    if (device_idx < sizeof(hw_eq_info) / sizeof(T_HW_EQ_INFO *))
                    {
                        tmp = (T_HW_EQ_INFO *)malloc(sizeof(T_HW_EQ_INFO));
                        if (tmp)
                        {
                            tmp->sample_rate = eq_para->sample_rate;
                            tmp->offset = offset;
                            tmp->len = eq_len;
                            tmp->next = hw_eq_info[device_idx];
                            hw_eq_info[device_idx] = tmp;
                        }
                    }

                    remain_size -= eq_len;
                    offset += eq_len;
                }
            }
            else
            {
                /* read fail */
                break;
            }
        }
    }
}

static void app_eq_fitting_set_test_mode_tmp_eq(void)
{
    if (test_mode_tmp_eq)
    {
        uint16_t eq_len = test_mode_tmp_eq[2] | (test_mode_tmp_eq[3] << 8);
        uint32_t sample_rate = app_eq_fitting_get_sample_rate(AUDIO_CATEGORY_AUDIO);

        /* tmp eq only support 48K audio */
        if (app_eq_fitting_is_streaming(AUDIO_STREAM_TYPE_PLAYBACK) &&
            (sample_rate == SAMPLE_RATE_48K))
        {
            audio_route_hw_eq_apply((T_AUDIO_ROUTE_HW_EQ_TYPE)test_mode_tmp_eq[0],
                                    test_mode_tmp_eq[1],
                                    &test_mode_tmp_eq[4],
                                    eq_len);
        }
    }
}

static void app_eq_fitting_set_test_mode_tmp_eq_advanced(void)
{
    if (test_mode_tmp_eq_advanced)
    {
        uint8_t  i;
        uint8_t  eq_num = test_mode_tmp_eq_advanced[0];
        uint8_t *eq_para_offset = &test_mode_tmp_eq_advanced[1];
        uint8_t  eq_channel;
        uint16_t para_len = 0;
        uint32_t sample_rate;
        T_HW_EQ_PARA             *eq_para = NULL;
        T_AUDIO_ROUTE_HW_EQ_TYPE  eq_type = AUDIO_ROUTE_HW_EQ_NUM;

        for (i = 0; i < eq_num; i++)
        {
            eq_para = (T_HW_EQ_PARA *)eq_para_offset;

            para_len = eq_para->stage_number * EQ_STAGE_LEN;

            if (eq_para->device_type == HW_EQ_DEVICE_PRIMARY_SPEAKER)
            {
                eq_type = AUDIO_ROUTE_HW_EQ_SPK;
                if (app_eq_fitting_is_streaming(AUDIO_STREAM_TYPE_PLAYBACK))
                {
                    eq_channel = app_eq_fitting_get_eq_channel(AUDIO_CATEGORY_AUDIO);
                    sample_rate = app_eq_fitting_get_sample_rate(AUDIO_CATEGORY_AUDIO);
                    if (sample_rate == eq_para->sample_rate)
                    {
                        audio_route_hw_eq_apply(eq_type, eq_channel, eq_para->para, para_len);
                    }
                }
            }
            else if (eq_para->device_type == HW_EQ_DEVICE_PRIMARY_MIC ||
                     eq_para->device_type == HW_EQ_DEVICE_SECONDARY_MIC ||
                     eq_para->device_type == HW_EQ_DEVICE_INTERNAL_MIC)
            {
                eq_type = AUDIO_ROUTE_HW_EQ_MIC;
                if (app_eq_fitting_is_streaming(AUDIO_STREAM_TYPE_VOICE))
                {
                    eq_channel = app_eq_fitting_get_eq_channel(AUDIO_CATEGORY_VOICE);
                    sample_rate = app_eq_fitting_get_sample_rate(AUDIO_CATEGORY_VOICE);
                    if (sample_rate == eq_para->sample_rate)
                    {
                        audio_route_hw_eq_apply(eq_type, eq_channel, eq_para->para, para_len);
                    }
                }
                else if (app_eq_fitting_is_streaming(AUDIO_STREAM_TYPE_RECORD))
                {
                    eq_channel = 0;
                    sample_rate = app_eq_fitting_get_sample_rate(AUDIO_CATEGORY_RECORD);
                    if (sample_rate == eq_para->sample_rate)
                    {
                        audio_route_hw_eq_apply(eq_type, eq_channel, eq_para->para, para_len);
                    }
                }
            }

            eq_para_offset += (sizeof(T_HW_EQ_PARA) + para_len);
        }
    }
}

bool app_eq_fitting_is_busy(void)
{
    return is_disallow_playback;
}

void app_eq_fitting_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                               uint8_t app_idx, uint8_t *ack_pkt)
{
    uint16_t cmd_id = cmd_ptr[0] | (cmd_ptr[1] << 8);

    ack_pkt[2] = CMD_SET_STATUS_COMPLETE;

    switch (cmd_id)
    {
    case CMD_HW_EQ_TEST_MODE:
        {
            uint8_t test_mode_status = cmd_ptr[2];

            if (test_mode_status == HW_EQ_ENTER_TEST_MODE)
            {
                is_test_mode = true;
                app_eq_fitting_hw_eq_clear();
            }
            else if (test_mode_status == HW_EQ_EXIT_TEST_MODE)
            {
                is_test_mode = false;
                if (test_mode_tmp_eq)
                {
                    free(test_mode_tmp_eq);
                    test_mode_tmp_eq = NULL;
                }
                if (test_mode_tmp_eq_advanced)
                {
                    free(test_mode_tmp_eq_advanced);
                    test_mode_tmp_eq_advanced = NULL;
                }

                if (app_eq_fitting_is_streaming(AUDIO_STREAM_TYPE_VOICE))
                {
                    audio_track_restart(app_db.br_link[app_hfp_get_active_idx()].sco_track_handle);
                }

                if (app_eq_fitting_is_streaming(AUDIO_STREAM_TYPE_PLAYBACK))
                {
                    audio_track_restart(app_db.br_link[app_a2dp_get_active_idx()].a2dp_track_handle);
                }

                if (app_eq_fitting_is_streaming(AUDIO_STREAM_TYPE_RECORD))
                {
                    audio_track_restart(dsp_capture_data_audio_record_handle);
                }

#if F_APP_APT_SUPPORT
                if (app_eq_fitting_is_dsp_apt_enabled())
                {
                    app_listening_state_machine(EVENT_ANC_APT_OFF, false, false);
                    app_listening_state_machine(EVENT_NORMAL_APT_ON, false, false);
                }
#endif
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            if (ack_pkt[2] == CMD_SET_STATUS_COMPLETE)
            {
                app_report_event(cmd_path, EVENT_HW_EQ_TEST_MODE, app_idx, &ack_pkt[2], sizeof(uint8_t));
            }
        }
        break;

    case CMD_HW_EQ_START_SET:
        {
            uint16_t total_eq_len = cmd_ptr[2] | (cmd_ptr[3] << 8);
            uint8_t buf[3];

            if (rcvd_hw_eq != NULL)
            {
                free(rcvd_hw_eq);
            }

            rcvd_hw_eq = calloc(1, sizeof(T_HW_EQ_RCVD_DATA) + total_eq_len);
            if (rcvd_hw_eq)
            {
                rcvd_hw_eq->hw_eq.total_len = total_eq_len;
                rcvd_hw_eq->hw_eq.crc16 = (uint16_t)(cmd_ptr[4] | (cmd_ptr[5] << 8));
                rcvd_hw_eq->buf_idx = 0;
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            if (ack_pkt[2] == CMD_SET_STATUS_COMPLETE)
            {
                buf[0] = CMD_SET_STATUS_COMPLETE;
                buf[1] = LO_BYTE(MAX_EQ_TOOL_CMD_LENGTH);
                buf[2] = HI_BYTE(MAX_EQ_TOOL_CMD_LENGTH);

                app_report_event(cmd_path, EVENT_HW_EQ_START_SET, app_idx, buf, sizeof(buf));
            }
        }
        break;

    case CMD_HW_EQ_CONTINUE_SET:
        {
            uint8_t total_pkt_num = cmd_ptr[2];
            uint8_t current_pkt_idx = cmd_ptr[3];
            uint8_t write_hw_eq_result = HW_EQ_WRITE_SUCCESS;

            if (rcvd_hw_eq && ((rcvd_hw_eq->buf_idx + cmd_len - 4) <= rcvd_hw_eq->hw_eq.total_len))
            {
                T_HW_EQ_DATA *hw_eq_data = &rcvd_hw_eq->hw_eq;

                memcpy_s(((uint8_t *)hw_eq_data->eq_para + rcvd_hw_eq->buf_idx),
                         hw_eq_data->total_len - rcvd_hw_eq->buf_idx, &cmd_ptr[4], cmd_len - 4);

                rcvd_hw_eq->buf_idx += (cmd_len - 4);

                if (current_pkt_idx == (total_pkt_num - 1))
                {
                    if (dfu_checkbufcrc((uint8_t *)&hw_eq_data->eq_para[0], hw_eq_data->total_len,
                                        hw_eq_data->crc16))
                    {
                        /* crc check fail */
                        ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                    }
                    else
                    {
                        hw_eq_data->calibrated = true;
                        hw_eq_data->magic_word = HW_EQ_MAGIC_WORD;

                        write_hw_eq_result = app_eq_fitting_write_hw_eq((uint8_t *)hw_eq_data,
                                                                        sizeof(T_HW_EQ_DATA) + hw_eq_data->total_len);

                        if (write_hw_eq_result == HW_EQ_WRITE_SUCCESS)
                        {
                            app_eq_fitting_free_eq_info();
                            app_eq_fitting_init_eq_info();
                        }
                        else if (write_hw_eq_result == HW_EQ_WRITE_WRONG_TRACK_STATE)
                        {
                            /* guarantee audio track state is not at started */
                            app_eq_fitting_stop_playback();

                            rcvd_hw_eq->cmd_path = cmd_path;
                            rcvd_hw_eq->app_idx = app_idx;
                            rcvd_hw_eq->cmd_id = cmd_id;
                            rcvd_hw_eq->event_id = EVENT_HW_EQ_CONTINUE_SET;
                        }
                        else
                        {
                            ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                        }
                    }

                    if (write_hw_eq_result != HW_EQ_WRITE_WRONG_TRACK_STATE)
                    {
                        free(rcvd_hw_eq);
                        rcvd_hw_eq = NULL;
                    }
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            if (write_hw_eq_result != HW_EQ_WRITE_WRONG_TRACK_STATE)
            {
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                if (ack_pkt[2] == CMD_SET_STATUS_COMPLETE)
                {
                    app_report_event(cmd_path, EVENT_HW_EQ_CONTINUE_SET, app_idx, &ack_pkt[2], sizeof(uint8_t));
                }
            }
        }
        break;

    case CMD_HW_EQ_CLEAR_CALIBRATE_FLAG:
        {
            T_HW_EQ_DATA *cleared_eq;
            uint8_t write_hw_eq_result = HW_EQ_WRITE_SUCCESS;

            if (rcvd_hw_eq != NULL)
            {
                free(rcvd_hw_eq);
            }

            rcvd_hw_eq = calloc(1, sizeof(T_HW_EQ_RCVD_DATA));
            if (rcvd_hw_eq)
            {
                cleared_eq = &rcvd_hw_eq->hw_eq;

                cleared_eq->magic_word = HW_EQ_MAGIC_WORD;
                cleared_eq->calibrated = false;
                cleared_eq->total_len = 0;

                write_hw_eq_result = app_eq_fitting_write_hw_eq((uint8_t *)cleared_eq, sizeof(T_HW_EQ_DATA));
                if (write_hw_eq_result == HW_EQ_WRITE_SUCCESS)
                {
                    app_eq_fitting_free_eq_info();
                }
                else if (write_hw_eq_result == HW_EQ_WRITE_WRONG_TRACK_STATE)
                {
                    /* guarantee audio track state is not at started */
                    app_eq_fitting_stop_playback();

                    rcvd_hw_eq->cmd_path = cmd_path;
                    rcvd_hw_eq->app_idx = app_idx;
                    rcvd_hw_eq->cmd_id = cmd_id;
                    rcvd_hw_eq->event_id = EVENT_HW_EQ_CLEAR_CALIBRATE_FLAG;
                }
                else
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }

                if (write_hw_eq_result != HW_EQ_WRITE_WRONG_TRACK_STATE)
                {
                    free(rcvd_hw_eq);
                    rcvd_hw_eq = NULL;
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
            }

            if (write_hw_eq_result != HW_EQ_WRITE_WRONG_TRACK_STATE)
            {
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                if (ack_pkt[2] == CMD_SET_STATUS_COMPLETE)
                {
                    app_report_event(cmd_path, EVENT_HW_EQ_CLEAR_CALIBRATE_FLAG, app_idx, &ack_pkt[2], sizeof(uint8_t));
                }
            }
        }
        break;

    case CMD_HW_EQ_SET_TEST_MODE_TMP_EQ:
        {
            uint8_t eq_type = cmd_ptr[3];
            uint8_t eq_channel = cmd_ptr[4];
            uint16_t eq_len;

            LE_ARRAY_TO_UINT16(eq_len, &cmd_ptr[9]);

            if (test_mode_tmp_eq)
            {
                free(test_mode_tmp_eq);
            }

            test_mode_tmp_eq = (uint8_t *)malloc(eq_len + 4);

            if (test_mode_tmp_eq)
            {
                test_mode_tmp_eq[0] = eq_type;
                test_mode_tmp_eq[1] = eq_channel;
                test_mode_tmp_eq[2] = eq_len & 0xFF;
                test_mode_tmp_eq[3] = (eq_len >> 8) & 0xFF;
                memcpy(&test_mode_tmp_eq[4], &cmd_ptr[11], eq_len);

                app_eq_fitting_set_test_mode_tmp_eq();
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            if (ack_pkt[2] == CMD_SET_STATUS_COMPLETE)
            {
                app_report_event(cmd_path, EVENT_HW_EQ_SET_TEST_MODE_TMP_EQ, app_idx, &ack_pkt[2], sizeof(uint8_t));
            }
        }
        break;

    case CMD_HW_EQ_SET_TEST_MODE_TMP_EQ_ADVANCED:
        {
            uint8_t eq_num = cmd_ptr[2];
            uint8_t *eq_para_offset = &cmd_ptr[3];
            T_HW_EQ_PARA *eq_para = NULL;
            uint16_t eq_para_len = 1;
            uint16_t para_len = 0;
            uint8_t i;

            if (test_mode_tmp_eq_advanced)
            {
                free(test_mode_tmp_eq_advanced);
            }

            if (eq_num <= 3)
            {
                // calculate total cmd para len
                for (i = 0; i < eq_num; i++)
                {
                    eq_para = (T_HW_EQ_PARA *)eq_para_offset;

                    para_len = eq_para->stage_number * EQ_STAGE_LEN;

                    eq_para_len += (sizeof(T_HW_EQ_PARA) + para_len);

                    eq_para_offset += (sizeof(T_HW_EQ_PARA) + para_len);
                }

                // malloc tmp_eq_advanced to storage cmd para
                test_mode_tmp_eq_advanced = (uint8_t *)malloc(eq_para_len);

                if (test_mode_tmp_eq_advanced && (cmd_len == 2 + eq_para_len))
                {
                    memcpy(test_mode_tmp_eq_advanced, &cmd_ptr[2], eq_para_len);
                    app_eq_fitting_set_test_mode_tmp_eq_advanced();
                }
                else
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            if (ack_pkt[2] == CMD_SET_STATUS_COMPLETE)
            {
                app_report_event(cmd_path, EVENT_HW_EQ_SET_TEST_MODE_TMP_EQ_ADVANCED, app_idx, &ack_pkt[2],
                                 sizeof(uint8_t));
            }
        }
        break;

    default:
        break;
    }
}

static bool app_eq_fitting_eq_load(void)
{
    T_HW_EQ_CONFIG_HEADER *eq_cfg;
    T_HW_EQ_BLOCK         *block;
    uint32_t               data_offset;
    int32_t                ret = 0;

    eq_cfg = malloc(sizeof(T_HW_EQ_CONFIG_HEADER));
    if (eq_cfg == NULL)
    {
        ret = 1;
        goto fail_alloc_config;
    }

    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG),
                       eq_cfg, sizeof(T_HW_EQ_CONFIG_HEADER));
    if (eq_cfg->sync_word != APP_DATA_SYNC_WORD)
    {
        ret = 2;
        goto fail_load_config;
    }

    eq_block_offset = eq_cfg->hw_eq_block_offset;
    data_offset = eq_block_offset;
    block = hw_eq_block;

    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + data_offset,
                       block, sizeof(T_HW_EQ_BLOCK) - sizeof(T_HW_EQ_PARAM_HEADER *));
    if (block->sync_word != APP_DATA_SYNC_WORD)
    {
        ret = 3;
        goto fail_load_block;
    }

    block->param_header = malloc(block->eq_num * sizeof(T_HW_EQ_PARAM_HEADER));
    if (block->param_header == NULL)
    {
        ret = 4;
        goto fail_alloc_header;
    }

    data_offset += sizeof(T_HW_EQ_BLOCK) - sizeof(T_HW_EQ_PARAM_HEADER *);
    if (fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + data_offset,
                           block->param_header, block->eq_num * sizeof(T_HW_EQ_PARAM_HEADER)) == false)
    {
        ret = 5;
        goto fail_load_header;
    }

    APP_PRINT_TRACE2("app_eq_fitting_eq_load: eq_block_len %d, eq_num %d",
                     block->eq_block_len, block->eq_num);

    free(eq_cfg);
    return true;

fail_load_header:
    free(block->param_header);
    block->param_header = NULL;
fail_alloc_header:
fail_load_block:
fail_load_config:
    free(eq_cfg);
fail_alloc_config:
    APP_PRINT_ERROR1("app_eq_fitting_eq_load: failed %d", -ret);
    return false;
}

static bool app_eq_fitting_hw_eq_cback(T_AUDIO_ROUTE_HW_EQ_TYPE eq_type,
                                       uint8_t                  eq_chann,
                                       uint32_t                 sample_rate)
{
    uint16_t i;

    for (i = 0; i < hw_eq_block->eq_num; i++)
    {
        if (hw_eq_block->param_header[i].eq_type == eq_type &&
            hw_eq_block->param_header[i].eq_chann == eq_chann &&
            hw_eq_block->param_header[i].sample_rate == sample_rate)
        {
            uint32_t  param_offset;
            uint16_t  param_length;
            uint8_t  *param_buffer;

            APP_PRINT_TRACE3("app_eq_fitting_hw_eq_cback: eq_type 0x%02x, eq_chann 0x%02x, sample_rate %d",
                             hw_eq_block->param_header[i].eq_type,
                             hw_eq_block->param_header[i].eq_chann,
                             hw_eq_block->param_header[i].sample_rate);

            param_offset = eq_block_offset + hw_eq_block->param_header[i].param_offset;
            param_length = hw_eq_block->param_header[i].param_length;

            param_buffer = malloc(param_length);
            if (param_buffer != NULL)
            {
                fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + param_offset,
                                   param_buffer, param_length);

                audio_route_hw_eq_apply(eq_type, eq_chann, param_buffer, param_length);
                free(param_buffer);
            }

            break;
        }
    }

    return true;
}

static void app_eq_fitting_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;

    switch (event_type)
    {
#if F_APP_APT_SUPPORT
    case AUDIO_EVENT_PASSTHROUGH_ENABLED:
        {
            T_AUDIO_EVENT_PARAM_PASSTHROUGH_STARTED *param = (T_AUDIO_EVENT_PARAM_PASSTHROUGH_STARTED *)
                                                             event_buf;

            if (param->mode == AUDIO_PASSTHROUGH_MODE_NORMAL)
            {
                if (is_test_mode)
                {
                    app_eq_fitting_hw_eq_clear();
                    app_eq_fitting_set_test_mode_tmp_eq();
                    app_eq_fitting_set_test_mode_tmp_eq_advanced();
                }
                else
                {
                    app_eq_fitting_send_hw_eq(AUDIO_CATEGORY_APT);
                }
            }
        }
        break;
#endif

    case AUDIO_EVENT_TRACK_STATE_CHANGED:
        {
            T_AUDIO_STREAM_TYPE stream_type;
            T_AUDIO_EVENT_PARAM *param = event_buf;
            T_AUDIO_TRACK_STATE state = param->track_state_changed.state;

            if (audio_track_stream_type_get(param->track_state_changed.handle, &stream_type))
            {
                if ((state == AUDIO_TRACK_STATE_STARTED) || (state == AUDIO_TRACK_STATE_RESTARTED))
                {
                    if (is_test_mode)
                    {
                        app_eq_fitting_hw_eq_clear_advanced(stream_type);
                        app_eq_fitting_set_test_mode_tmp_eq();
                        app_eq_fitting_set_test_mode_tmp_eq_advanced();
                    }
                    else
                    {
                        if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
                        {
                            app_eq_fitting_send_hw_eq(AUDIO_CATEGORY_AUDIO);
                        }
                        else if (stream_type == AUDIO_STREAM_TYPE_VOICE)
                        {
                            app_eq_fitting_send_hw_eq(AUDIO_CATEGORY_VOICE);
                        }
                        else if (stream_type == AUDIO_STREAM_TYPE_RECORD)
                        {
                            app_eq_fitting_send_hw_eq(AUDIO_CATEGORY_RECORD);
                        }
                    }
                }
                else if (state == AUDIO_TRACK_STATE_STOPPED)
                {
                    if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK && is_disallow_playback && rcvd_hw_eq)
                    {
                        T_HW_EQ_DATA *hw_eq_data = &rcvd_hw_eq->hw_eq;
                        uint8_t write_hw_eq_result = HW_EQ_WRITE_SUCCESS;
                        uint8_t ack_pkt[3];

                        write_hw_eq_result = app_eq_fitting_write_hw_eq((uint8_t *)hw_eq_data,
                                                                        sizeof(T_HW_EQ_DATA) + hw_eq_data->total_len);

                        ack_pkt[0] = LO_BYTE(rcvd_hw_eq->cmd_id);
                        ack_pkt[1] = HI_BYTE(rcvd_hw_eq->cmd_id);

                        if (write_hw_eq_result == HW_EQ_WRITE_SUCCESS)
                        {
                            ack_pkt[2] = CMD_SET_STATUS_COMPLETE;

                            app_eq_fitting_free_eq_info();
                            app_eq_fitting_init_eq_info();
                        }
                        else
                        {
                            ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                        }

                        app_report_event(rcvd_hw_eq->cmd_path, EVENT_ACK, rcvd_hw_eq->app_idx, ack_pkt,
                                         sizeof(ack_pkt));
                        if (ack_pkt[2] == CMD_SET_STATUS_COMPLETE)
                        {
                            app_report_event(rcvd_hw_eq->cmd_path, rcvd_hw_eq->event_id,
                                             rcvd_hw_eq->app_idx, &ack_pkt[2], sizeof(uint8_t));
                        }

                        free(rcvd_hw_eq);
                        rcvd_hw_eq = NULL;

                        app_eq_fitting_start_playback();
                    }
                }
            }
        }
        break;

    default:
        {
            handle = false;
        }
        break;
    }

    if (handle)
    {
        APP_PRINT_TRACE2("app_eq_fitting_cback: event 0x%04x, is_test_mode %d", event_type, is_test_mode);
    }
}

void app_eq_fitting_init(void)
{
    rcvd_hw_eq = NULL;
    is_test_mode = false;
    is_disallow_playback = false;
    test_mode_tmp_eq = NULL;
    test_mode_tmp_eq_advanced = NULL;

    app_eq_fitting_init_eq_info();
    audio_mgr_cback_register(app_eq_fitting_cback);

    if (app_eq_fitting_eq_load() == false)
    {
        return;
    }

    if (audio_route_hw_eq_register(app_eq_fitting_hw_eq_cback) == false)
    {
        return;
    }
}

void app_eq_fitting_deinit(void)
{
    if (rcvd_hw_eq)
    {
        free(rcvd_hw_eq);
        rcvd_hw_eq = NULL;
    }

    if (test_mode_tmp_eq)
    {
        free(test_mode_tmp_eq);
        test_mode_tmp_eq = NULL;
    }

    if (test_mode_tmp_eq_advanced)
    {
        free(test_mode_tmp_eq_advanced);
        test_mode_tmp_eq_advanced = NULL;
    }

    app_eq_fitting_free_eq_info();
    is_test_mode = false;
    is_disallow_playback = false;

    audio_mgr_cback_unregister(app_eq_fitting_cback);
    audio_route_hw_eq_unregister();
}

#endif
