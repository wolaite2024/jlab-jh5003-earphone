#if F_APP_EXT_MIC_SWITCH_SUPPORT
#include "trace.h"
#include "app_main.h"
#include "string.h"
#include "app_dlps.h"
#include "app_cfg.h"
#include "app_ext_mic_switch.h"
#include "app_dsp_cfg.h"
#include "app_io_msg.h"
#include "section.h"
#include "hw_tim.h"
#include "fmc_api.h"
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "hal_gpio_int.h"
#include "vector_table.h"
#include "app_timer.h"
#include "hal_gpio.h"
#include "audio.h"
#include "audio_vse.h"

#if F_APP_EXT_MIC_SWITCH_PHYSICAL_MIC_SUPPORT
#include "audio_route.h"
#include "audio_type.h"
#endif

#if F_APP_EXT_MIC_SWITCH_IC_SUPPORT
#define MIC_SEL_PIN             P6_2
//#define DIO_MUTE_PIN            P6_3
#endif

#define VSE_EFFECT_ID           0x00

typedef enum t_app_ext_mic_ltv_type
{
    LTV_TYPE_COMFORT_NOISE    = 0x09,
    LTV_TYPE_FADE_IN_OUT      = 0x07,
    LTV_TYPE_SPEECH_BAND_GAIN = 0x06, /* 32kHZ Process */
    LTV_TYPE_MBDRC            = 0x03, /* MB-DRC/AGC */
    LTV_TYPE_AGC              = 0x10, /* MB-DRC/AGC */
    LTV_TYPE_DIGITAL_FILTER   = 0x04, /* HPF & DC Remover*/
    LTV_TYPE_NEAR_END_NR      = 0x0E, /* NR/AES */
    LTV_TYPE_AES              = 0x0D, /* NR/AES */
    LTV_TYPE_DMNR             = 0x0B, /* Multi-Mic */
    LTV_TYPE_MIC_FUSION       = 0x0F, /* Multi-Mic */
    LTV_TYPE_AEC              = 0x11,
    LTV_TYPE_NEAR_END_GAIN    = 0x0C, /* DSP Gain */
} T_APP_EXT_MIC_LTV_TYPE;

typedef struct t_app_ext_mic_ltv_metadata
{
    uint8_t  len;
    uint8_t  type;
    uint8_t  version;
    uint8_t  control;
    uint8_t  payload[0];
} T_APP_EXT_MIC_LTV_METADATA;

typedef struct t_app_ext_mic_start_ltv_metadata
{
    uint8_t  len;
    uint8_t  type;
    uint8_t  version;
    uint8_t  control;
    uint16_t total_payload_len;
    uint8_t  payload[0];
} T_APP_EXT_MIC_START_LTV_METADATA;

typedef struct t_app_ext_mic_dsp_algo_entry
{
    uint8_t  category;
    uint8_t  bulk_num;
    uint16_t total_len;
    uint32_t offset;
} T_APP_EXT_MIC_DSP_ALGO_ENTRY;

typedef struct t_app_ext_mic_dsp_algo_block
{
    uint16_t         sync_word;
    uint16_t         reserved;
    uint32_t         block_len;
    uint8_t          version;
    uint8_t          category_num;
    uint16_t         bulk_size;
    T_APP_EXT_MIC_DSP_ALGO_ENTRY entry[AUDIO_CATEGORY_NUM];
} T_APP_EXT_MIC_DSP_ALGO_BLOCK;

typedef struct t_app_ext_mic_dsp_algo_data
{
    uint32_t          offset;
    T_APP_EXT_MIC_DSP_ALGO_BLOCK  algo_block;
} T_APP_EXT_MIC_DSP_ALGO_DATA;

typedef struct t_app_ext_mic_dsp_config_header
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
} T_APP_EXT_MIC_DSP_CONFIG_HEADER;

typedef struct t_app_ext_mic_vse_data
{
    struct t_app_ext_mic_vse_data *next;
    uint16_t seq_num;
    T_AUDIO_TRACK_HANDLE audio_track_handle;
    T_AUDIO_EFFECT_INSTANCE vse_instance;
} T_APP_EXT_MIC_VSE_DATA;

#if F_APP_EXT_MIC_PLUG_IN_GPIO_DETECT
#define BOOM_IN_DETECT_PIN      P3_5//P6_0

static uint8_t app_gpio_ext_mic_timer_id = 0;
static uint8_t timer_idx_ext_mic_io_debounce = 0;

typedef enum
{
    APP_TIMER_EXT_MIC_DEBOUNCE,
} T_APP_EXT_MIC_TIMER;
#endif

typedef enum
{
    LTV_METADATA_CONTROL_TYPE_SINGLE,
    LTV_METADATA_CONTROL_TYPE_START,
    LTV_METADATA_CONTROL_TYPE_CONTINUE,
    LTV_METADATA_CONTROL_TYPE_END,
} T_APP_EXT_MIC_LTV_METADATA_CONTROL_TYPE;

T_OS_QUEUE vse_queue;
static T_APP_EXT_MIC_DSP_ALGO_DATA dsp_algo_data = {0};

#if F_APP_EXT_MIC_SWITCH_IC_SUPPORT
//DIO32276 mute: 1, unmute: 0
void app_ext_mic_switch_ic_mute(bool mute)
{
#ifdef DIO_MUTE_PIN
    hal_gpio_set_level(DIO_MUTE_PIN, (T_GPIO_LEVEL)mute);
#endif
}

//DIO32276 mic select internal mic: 1, boom mic: 0
void app_ext_mic_switch_ic_mic_sel(bool internal_mic)
{
    hal_gpio_set_level(MIC_SEL_PIN, (T_GPIO_LEVEL)internal_mic);
}
#endif

bool app_ext_mic_load_dsp_algo_data(void)
{
    T_APP_EXT_MIC_DSP_CONFIG_HEADER header;
    uint32_t            offset;
    int32_t             ret = 0;
    FLASH_IMG_ID        dsp_image = FLASH_IMG_DSPCONFIG;

    if (app_db.ext_mic_plugged)
    {
#if CONFIG_SOC_SERIES_RTL8773D || TARGET_RTL8773DFL
        dsp_image = FLASH_IMG_EXT2;
#else
        dsp_image = FLASH_IMG_ADSP;
#endif
    }

    offset = flash_cur_bank_img_payload_addr_get(dsp_image);
    fmc_flash_nor_read(offset, &header, sizeof(T_APP_EXT_MIC_DSP_CONFIG_HEADER));
    if (header.sync_word != APP_DATA_SYNC_WORD)
    {
        ret = 1;
        goto fail_load_header;
    }

    offset += header.algo_block_offset;
    dsp_algo_data.offset = offset;
    fmc_flash_nor_read(offset, &dsp_algo_data.algo_block, sizeof(T_APP_EXT_MIC_DSP_ALGO_BLOCK));
    if (dsp_algo_data.algo_block.sync_word != APP_DATA_SYNC_WORD)
    {
        ret = 2;
        goto fail_load_block;
    }

    APP_PRINT_TRACE1("app_ext_mic_load_dsp_algo_data: dsp_image %d", dsp_image);

    return true;

fail_load_block:
fail_load_header:
    APP_PRINT_TRACE1("app_ext_mic_load_dsp_algo_data: failed %d", -ret);
    return false;
}

T_APP_EXT_MIC_VSE_DATA *app_ext_mic_find_vse(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_APP_EXT_MIC_VSE_DATA *vse_data = NULL;
    uint8_t i;

    for (i = 0; i < vse_queue.count; i++)
    {
        vse_data = os_queue_peek(&vse_queue, i);

        if (vse_data != NULL && vse_data->vse_instance == instance)
        {
            return vse_data;
        }
    }

    return NULL;
}

T_APP_EXT_MIC_VSE_DATA *app_ext_mic_find_vse_by_audio_track(T_AUDIO_TRACK_HANDLE handle)
{
    T_APP_EXT_MIC_VSE_DATA *vse_data = NULL;
    uint8_t i;

    for (i = 0; i < vse_queue.count; i++)
    {
        vse_data = os_queue_peek(&vse_queue, i);

        if (vse_data != NULL && vse_data->audio_track_handle == handle)
        {
            return vse_data;
        }
    }

    return NULL;
}

bool app_ext_mic_check_ltv_upstream_type(uint8_t type)
{
    uint8_t i;
    uint8_t upstream_type[] =
    {
        LTV_TYPE_COMFORT_NOISE,
        LTV_TYPE_FADE_IN_OUT,
        LTV_TYPE_SPEECH_BAND_GAIN,
        LTV_TYPE_MBDRC,
        LTV_TYPE_AGC,
        LTV_TYPE_DIGITAL_FILTER,
        LTV_TYPE_NEAR_END_NR,
        LTV_TYPE_AES,
        LTV_TYPE_DMNR,
        LTV_TYPE_MIC_FUSION,
        LTV_TYPE_AEC,
        LTV_TYPE_NEAR_END_GAIN,
    };

    for (i = 0; i < sizeof(upstream_type); i++)
    {
        if (type == upstream_type[i])
        {
            return true;
        }
    }

    return false;
}

static void app_ext_mic_send_dsp_algo(T_AUDIO_EFFECT_INSTANCE instance)
{
    T_APP_EXT_MIC_VSE_DATA *vse_data = app_ext_mic_find_vse(instance);
    T_AUDIO_STREAM_TYPE stream_type;
    T_AUDIO_CATEGORY category;
    uint8_t *load_buf = NULL;
    uint8_t *tmp_buf = NULL;
    T_APP_EXT_MIC_LTV_METADATA *tmp_ltv_data = NULL;
    uint16_t bulk_size = dsp_algo_data.algo_block.bulk_size;
    uint16_t total_len = 0;
    uint16_t remained_len = 0;
    uint32_t offset = 0;
    uint8_t cause = 0;
    uint16_t tmp_data_index = 0;
    uint8_t i = 0;

    if (!vse_data)
    {
        cause = 1;
        goto exit;
    }

    if (!audio_track_stream_type_get(vse_data->audio_track_handle, &stream_type))
    {
        cause = 2;
        goto exit;
    }
    else
    {
        if (stream_type == AUDIO_STREAM_TYPE_VOICE)
        {
            category = AUDIO_CATEGORY_VOICE;
        }
        else if (stream_type == AUDIO_CATEGORY_RECORD)
        {
            category = AUDIO_CATEGORY_RECORD;
        }
        else
        {
            cause = 3;
            goto exit;
        }
    }

    offset       = dsp_algo_data.offset + dsp_algo_data.algo_block.entry[category].offset;
    total_len    = dsp_algo_data.algo_block.entry[category].total_len;
    remained_len = total_len;

    if (total_len == 0)
    {
        cause = 4;
        goto exit;
    }

    load_buf = (uint8_t *)calloc(bulk_size, 1);
    tmp_buf = (uint8_t *)calloc(bulk_size, 1);

    if (load_buf == NULL || tmp_buf == NULL)
    {
        cause = 5;
        goto exit;
    }

    tmp_ltv_data = (T_APP_EXT_MIC_LTV_METADATA *) tmp_buf;

    for (i = 0; i < dsp_algo_data.algo_block.entry[category].bulk_num; i++)
    {
        uint16_t index = 0;
        uint16_t load_len = (remained_len < bulk_size) ? remained_len : bulk_size;

        fmc_flash_nor_read(offset + i * bulk_size, load_buf, load_len);
        remained_len -= load_len;

        while (index < load_len && load_buf[index] != 0)
        {
            T_APP_EXT_MIC_LTV_METADATA *ltv_metadata = (T_APP_EXT_MIC_LTV_METADATA *)&load_buf[index];

            if (app_ext_mic_check_ltv_upstream_type(ltv_metadata->type))
            {
                if (ltv_metadata->control == LTV_METADATA_CONTROL_TYPE_SINGLE)
                {
                    audio_vse_apply(instance, vse_data->seq_num++, ltv_metadata, ltv_metadata->len + 1);
                }
                else if (ltv_metadata->control == LTV_METADATA_CONTROL_TYPE_START)
                {
                    T_APP_EXT_MIC_START_LTV_METADATA *start_ltv_metadata =
                        (T_APP_EXT_MIC_START_LTV_METADATA *)&load_buf[index];

                    tmp_ltv_data->len = start_ltv_metadata->total_payload_len + 3;
                    tmp_ltv_data->type = start_ltv_metadata->type;
                    tmp_ltv_data->version = start_ltv_metadata->version;
                    tmp_ltv_data->control = LTV_METADATA_CONTROL_TYPE_SINGLE;
                    memcpy(tmp_ltv_data->payload, start_ltv_metadata->payload, start_ltv_metadata->len - 5);
                    tmp_data_index = (start_ltv_metadata->len - 5);
                }
                else
                {
                    if (tmp_ltv_data->type == ltv_metadata->type)
                    {
                        memcpy(&tmp_ltv_data->payload[tmp_data_index], ltv_metadata->payload, ltv_metadata->len - 3);
                        audio_vse_apply(instance, vse_data->seq_num++, tmp_ltv_data, tmp_ltv_data->len + 1);
                    }
                }
            }

            index += (ltv_metadata->len + 1);
        }
    }

exit:
    if (load_buf)
    {
        free(load_buf);
    }

    if (tmp_buf)
    {
        free(tmp_buf);
    }

    if (cause != 0)
    {
        APP_PRINT_INFO1("app_ext_mic_send_dsp_algo: cause %d", cause);
    }
}

static void app_ext_mic_vse_cback(T_AUDIO_EFFECT_INSTANCE  instance,
                                  T_AUDIO_VSE_EVENT        event_type,
                                  uint16_t                 seq_num,
                                  void                     *event_buf,
                                  uint16_t                 buf_len)
{
    bool handle = true;

    switch (event_type)
    {
    case AUDIO_VSE_EVENT_ENABLE:
        {
            app_ext_mic_send_dsp_algo(instance);
        }
        break;

    case AUDIO_VSE_EVENT_DISABLE:
        {
        }
        break;

    case AUDIO_VSE_EVENT_TRANS_RSP:
        {
        }
        break;

    case AUDIO_VSE_EVENT_TRANS_IND:
        {
        }
        break;

    case AUDIO_VSE_EVENT_TRANS_NOTIFY:
        {
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
        APP_PRINT_INFO1("app_ext_mic_vse_cback: event_type 0x%04x", event_type);
    }
}

void app_ext_mic_add_vse(T_AUDIO_TRACK_HANDLE handle)
{
    T_APP_EXT_MIC_VSE_DATA *vse_data = NULL;
    T_AUDIO_STREAM_TYPE stream_type;
    uint8_t cause = 0;

    if (!audio_track_stream_type_get(handle, &stream_type))
    {
        cause = 1;
        goto exit;
    }
    else
    {
        if (stream_type != AUDIO_STREAM_TYPE_VOICE && stream_type != AUDIO_CATEGORY_RECORD)
        {
            goto exit;
        }
    }

    if (app_ext_mic_find_vse_by_audio_track(handle))
    {
        cause = 2;
        goto exit;
    }

    vse_data = calloc(1, sizeof(T_APP_EXT_MIC_VSE_DATA));

    if (vse_data)
    {
        vse_data->vse_instance = audio_vse_create(AUDIO_VSE_COMPANY_ID_REALTEK, VSE_EFFECT_ID,
                                                  app_ext_mic_vse_cback);

        if (vse_data->vse_instance)
        {
            vse_data->audio_track_handle = handle;

            audio_track_effect_attach(handle, vse_data->vse_instance);
            os_queue_in(&vse_queue, vse_data);
        }
        else
        {
            free(vse_data);
            cause = 4;
            goto exit;
        }
    }
    else
    {
        cause = 3;
        goto exit;
    }

exit:
    if (cause != 0)
    {
        APP_PRINT_INFO1("app_ext_mic_add_vse: cause %d", cause);
    }
}

void app_ext_mic_remove_vse(T_AUDIO_TRACK_HANDLE handle)
{
    T_APP_EXT_MIC_VSE_DATA *vse_data = app_ext_mic_find_vse_by_audio_track(handle);

    if (vse_data)
    {
        os_queue_delete(&vse_queue, vse_data);
        audio_vse_release(vse_data->vse_instance);
        free(vse_data);
    }
}

void app_ext_mic_clear_seq_num(T_AUDIO_TRACK_HANDLE handle)
{
    T_APP_EXT_MIC_VSE_DATA *vse_data = app_ext_mic_find_vse_by_audio_track(handle);

    if (vse_data)
    {
        vse_data->seq_num = 0;
    }
}

#if F_APP_EXT_MIC_SWITCH_PHYSICAL_MIC_SUPPORT
void app_ext_mic_switch_physical_mic(T_AUDIO_TRACK_HANDLE handle)
{
    T_AUDIO_STREAM_TYPE stream_type;
    T_AUDIO_ROUTE_PATH path;
    T_AUDIO_CATEGORY category;
    T_AUDIO_ROUTE_IO_TYPE primary_in;
    T_AUDIO_ROUTE_IO_TYPE secondary_in;
    T_AUDIO_ROUTE_ENTRY secondary_in_entry;
    bool is_secondary_in_enabled = false;
    uint8_t i, j;

    if (audio_track_stream_type_get(handle, &stream_type) == false)
    {
        return;
    }
    else
    {
        if (stream_type == AUDIO_STREAM_TYPE_VOICE)
        {
            category = AUDIO_CATEGORY_VOICE;
            primary_in = AUDIO_ROUTE_IO_VOICE_PRIMARY_IN;
            secondary_in = AUDIO_ROUTE_IO_VOICE_SECONDARY_IN;
        }
        else if (stream_type == AUDIO_STREAM_TYPE_RECORD)
        {
            category = AUDIO_CATEGORY_RECORD;
            primary_in = AUDIO_ROUTE_IO_RECORD_PRIMARY_IN;
            secondary_in = AUDIO_ROUTE_IO_RECORD_SECONDARY_IN;
        }
        else
        {
            return;
        }
    }

    audio_track_route_unbind(handle);
    path = audio_route_path_take(category, AUDIO_DEVICE_IN_MIC | AUDIO_DEVICE_OUT_SPK);

    for (i = 0; i < path.entry_num; i++)
    {
        if (path.entry[i].io_type == primary_in)
        {
            if (app_db.ext_mic_plugged)
            {
                path.entry[i].endpoint_attr.mic.id = AUDIO_ROUTE_AMIC1;
            }
            else
            {
                path.entry[i].endpoint_attr.mic.id = AUDIO_ROUTE_AMIC2;
            }

            memcpy(&secondary_in_entry, &path.entry[i], sizeof(T_AUDIO_ROUTE_ENTRY));
        }
        else if (path.entry[i].io_type == secondary_in)
        {
            path.entry[i].endpoint_attr.mic.id = AUDIO_ROUTE_AMIC3;

            is_secondary_in_enabled = true;
        }
    }

    audio_track_route_bind(handle, path);

    if (is_secondary_in_enabled && app_db.ext_mic_plugged)
    {
        audio_track_route_remove(handle, secondary_in);
    }
    else if (!is_secondary_in_enabled && !app_db.ext_mic_plugged)
    {
        secondary_in_entry.endpoint_attr.mic.id = AUDIO_ROUTE_AMIC3;
        audio_track_route_add(handle, &secondary_in_entry);
    }

    audio_route_path_give(&path);

    APP_PRINT_TRACE4("app_ext_mic_switch_physical_mic: category %d, entry_num %d, secondary_in_enabled %d, ext_mic_plugged %d",
                     category, path.entry_num, is_secondary_in_enabled, app_db.ext_mic_plugged);
}
#endif

static void app_ext_mic_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_AUDIO_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case AUDIO_EVENT_TRACK_STATE_CHANGED:
        {
            switch (param->track_state_changed.state)
            {
            case AUDIO_TRACK_STATE_CREATED:
                {
#if F_APP_EXT_MIC_SWITCH_PHYSICAL_MIC_SUPPORT
                    app_ext_mic_switch_physical_mic(param->track_state_changed.handle);
#endif

                    app_ext_mic_add_vse(param->track_state_changed.handle);
                }
                break;

            case AUDIO_TRACK_STATE_RELEASED:
                {
                    app_ext_mic_remove_vse(param->track_state_changed.handle);
                }
                break;

            case AUDIO_TRACK_STATE_STOPPED:
            case AUDIO_TRACK_STATE_PAUSED:
                {
#if F_APP_EXT_MIC_SWITCH_PHYSICAL_MIC_SUPPORT
                    app_ext_mic_switch_physical_mic(param->track_state_changed.handle);
#endif

                    app_ext_mic_clear_seq_num(param->track_state_changed.handle);
                }
                break;
            }
        }
        break;
    }

}

#if F_APP_EXT_MIC_PLUG_IN_GPIO_DETECT
static uint8_t app_ext_mic_gpio_detect_switch()
{
    //Read BOOM_IN status to set MIC_SEL to High : Internal Mic
    //                                      Low  : Boom Mic
    uint8_t boom_in_level;
    uint8_t mic_plugged = false;

    boom_in_level = !hal_gpio_get_input_level(BOOM_IN_DETECT_PIN);

#if F_APP_EXT_MIC_SWITCH_IC_SUPPORT
    app_ext_mic_switch_ic_mute(APP_EXT_MIC_IC_MUTE);
#endif

    if (boom_in_level)
    {
#if F_APP_EXT_MIC_SWITCH_IC_SUPPORT
        //MIC_SEL_PIN High Level : Boom Mic is not plugged
        app_ext_mic_switch_ic_mic_sel(APP_EXT_MIC_IC_INTERNAL_MIC);
#endif
        mic_plugged = false;
    }
    else
    {
#if F_APP_EXT_MIC_SWITCH_IC_SUPPORT
        //MIC_SEL_PIN Low Level : Boom Mic is Plugged
        app_ext_mic_switch_ic_mic_sel(APP_EXT_MIC_IC_BOOM_MIC);
#endif
        mic_plugged = true;
    }

    //switch dsp config image
    if (app_db.ext_mic_plugged != mic_plugged)
    {
        app_db.ext_mic_plugged = mic_plugged;

        app_ext_mic_load_dsp_algo_data();
        app_dsp_cfg_apply();
    }

#if F_APP_EXT_MIC_SWITCH_IC_SUPPORT
    app_ext_mic_switch_ic_mute(APP_EXT_MIC_IC_UNMUTE);
#endif

    return boom_in_level;
}

static void app_ext_mic_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_EXT_MIC_DEBOUNCE:
        {
            app_stop_timer(&timer_idx_ext_mic_io_debounce);
            app_ext_mic_gpio_detect_switch();
            app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
        }
        break;

    default:
        break;
    }
}

void app_ext_mic_gpio_detect_handle_msg(T_IO_MSG *io_driver_msg_recv)
{
    app_stop_timer(&timer_idx_ext_mic_io_debounce);
    app_start_timer(&timer_idx_ext_mic_io_debounce, "sensor_ld_io_debounce",
                    app_gpio_ext_mic_timer_id, APP_TIMER_EXT_MIC_DEBOUNCE, 0, false,
                    500);
}

void app_ext_mic_gpio_detect_init(void)
{
    app_timer_reg_cb(app_ext_mic_timeout_cb, &app_gpio_ext_mic_timer_id);
}

void app_ext_mic_gpio_detect_enter_dlps_pad_set(void)
{
    app_dlps_pad_wake_up_enable(BOOM_IN_DETECT_PIN);
}

void app_ext_mic_gpio_detect_exit_dlps_pad_set(void)
{
    app_dlps_restore_pad(BOOM_IN_DETECT_PIN);
}

//GPIO interrupt handler function
ISR_TEXT_SECTION void app_ext_mic_gpio_detect_intr_handler(uint32_t param)
{
    T_IO_MSG gpio_msg;
    uint8_t pinmux = BOOM_IN_DETECT_PIN;
    uint8_t gpio_level = hal_gpio_get_input_level(pinmux);
    /* Control of entering DLPS */
    app_dlps_disable(APP_DLPS_ENTER_CHECK_GPIO);

    /* Disable GPIO interrupt */
    hal_gpio_irq_disable(pinmux);

    /* Change GPIO Interrupt Polarity */
    if (gpio_level)
    {
        hal_gpio_irq_change_polarity(pinmux, GPIO_IRQ_ACTIVE_LOW); //Polarity Low
    }
    else
    {
        hal_gpio_irq_change_polarity(pinmux, GPIO_IRQ_ACTIVE_HIGH); //Polarity High
    }

    gpio_msg.type = IO_MSG_TYPE_GPIO;
    gpio_msg.subtype = IO_MSG_GPIO_EXT_MIC_IO_DETECT;

    app_io_msg_send(&gpio_msg);

    /* Enable GPIO interrupt */
    hal_gpio_irq_enable(pinmux);
}
#endif

//initialization of pinmux settings and pad settings
void app_ext_mic_gpio_board_init(void)
{
#if F_APP_EXT_MIC_SWITCH_IC_SUPPORT
    //MIC_SEL pinmux gpio
    //DIO_MUTE_PIN pinmux gpio
#endif
}

void app_ext_mic_gpio_driver_init(void)
{
#if F_APP_EXT_MIC_SWITCH_IC_SUPPORT
    hal_gpio_init_pin(MIC_SEL_PIN, GPIO_TYPE_AON, GPIO_DIR_OUTPUT, GPIO_PULL_DOWN);
    hal_gpio_set_level(MIC_SEL_PIN, GPIO_LEVEL_HIGH);

#ifdef DIO_MUTE_PIN
    hal_gpio_init_pin(DIO_MUTE_PIN, GPIO_TYPE_AON, GPIO_DIR_OUTPUT, GPIO_PULL_DOWN);
    hal_gpio_set_level(DIO_MUTE_PIN, GPIO_LEVEL_LOW);
#endif
#endif

#if F_APP_EXT_MIC_PLUG_IN_GPIO_DETECT
    //init BOOM_IN as an interrupt pin
    /* GPIO Parameter Config */
    {
        hal_gpio_init_pin(BOOM_IN_DETECT_PIN, GPIO_TYPE_AUTO, GPIO_DIR_INPUT, GPIO_PULL_UP);

        hal_gpio_set_up_irq(BOOM_IN_DETECT_PIN, GPIO_IRQ_EDGE, GPIO_IRQ_ACTIVE_HIGH, true); //Polarity Low

        hal_gpio_register_isr_callback(BOOM_IN_DETECT_PIN,
                                       app_ext_mic_gpio_detect_intr_handler, 0);

        //enable int
        hal_gpio_irq_enable(BOOM_IN_DETECT_PIN);
    }
#endif

    app_ext_mic_load_dsp_algo_data();
    audio_mgr_cback_register(app_ext_mic_cback);
    os_queue_init(&vse_queue);
}

#endif
