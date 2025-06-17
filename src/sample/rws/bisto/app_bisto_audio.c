#if BISTO_FEATURE_SUPPORT
#include "app_cfg.h"
#include "app_bisto_audio.h"
#include "app_bisto_msg.h"
#include "app_bisto_bt.h"
#include "btm.h"
#include "audio_track.h"
#include "trace.h"

static struct
{
    BISTO_AUDIO *p_audio;
    T_AUDIO_TRACK_HANDLE handle;
    GSoundTargetAudioInSettings gsound_audio_settings;
} app_bisto_audio = {.p_audio = NULL, .handle = NULL, .gsound_audio_settings = {0}};


static void app_bisto_audio_dump_mic_data(const char *title, uint8_t *record_data_buf,
                                          uint32_t data_len)
{
    const uint32_t bat_num = 8;
    uint32_t times = data_len / bat_num;
    uint32_t residue = data_len % bat_num;
    uint8_t *residue_buf = record_data_buf + times * bat_num;

    APP_PRINT_TRACE3("app_bisto_audio_dump_mic_data: data_len %d, times %d, residue %d", data_len,
                     times, residue);
    APP_PRINT_TRACE2("app_bisto_audio_dump_mic_data: record_data_buf is 0x%08x, residue_buf is 0x%08x\r\n",
                     (uint32_t)record_data_buf,
                     (uint32_t)residue_buf);

    APP_PRINT_TRACE1("@@@@@@@@@@@@@@@@@@@@@%s@@@@@@@@@@@@@@@@@@@@@@@@@@@", TRACE_STRING(title));

    for (int32_t i = 0; i < times; i++)
    {
        APP_PRINT_TRACE8("app_bisto_audio_dump_mic_data: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
                         record_data_buf[i * bat_num], record_data_buf[i * bat_num + 1], record_data_buf[i * bat_num + 2],
                         record_data_buf[i * bat_num + 3],
                         record_data_buf[i * bat_num + 4], record_data_buf[i * bat_num + 5],
                         record_data_buf[i * bat_num + 6],
                         record_data_buf[i * bat_num + 7]);
    }

    switch (residue)
    {
    case 1:
        APP_PRINT_TRACE1("app_bisto_audio_dump_mic_data: 0x%02x\r\n", residue_buf[0]);
        break;
    case 2:
        APP_PRINT_TRACE2("app_bisto_audio_dump_mic_data: 0x%02x, 0x%02x\r\n", residue_buf[0],
                         residue_buf[1]);
        break;
    case 3:
        APP_PRINT_TRACE3("app_bisto_audio_dump_mic_data: 0x%02x, 0x%02x, 0x%02x\r\n", residue_buf[0],
                         residue_buf[1],
                         residue_buf[2]);
        break;
    case 4:
        APP_PRINT_TRACE4("app_bisto_audio_dump_mic_data: 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
                         residue_buf[0],
                         residue_buf[1], residue_buf[2], residue_buf[3]);
        break;
    case 5:
        APP_PRINT_TRACE5("app_bisto_audio_dump_mic_data: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
                         residue_buf[0], residue_buf[1], residue_buf[2], residue_buf[3], residue_buf[4]);
        break;
    case 6:
        APP_PRINT_TRACE6("app_bisto_audio_dump_mic_data: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
                         residue_buf[0], residue_buf[1], residue_buf[2], residue_buf[3], residue_buf[4], residue_buf[5]);
        break;
    case 7:
        APP_PRINT_TRACE7("app_bisto_audio_dump_mic_data: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
                         residue_buf[0], residue_buf[1], residue_buf[2], residue_buf[3], residue_buf[4], residue_buf[5],
                         residue_buf[6]);
        break;

    default:
        break;
    }

    APP_PRINT_TRACE0("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
}


BISTO_ERR app_bisto_audio_ringtone_init(BISTO_AUDIO *p_audio)
{
    return BISTO_OK;
}


BISTO_ERR app_bisto_audio_ringtone_play(BISTO_AUDIO *p_audio, GSoundTargetAudioOutClip clip)
{
    return BISTO_OK;
}


bool app_bisto_audio_async_read(T_AUDIO_TRACK_HANDLE   handle,
                                uint32_t              *timestamp,
                                uint16_t              *seq_num,
                                T_AUDIO_STREAM_STATUS *status,
                                uint8_t               *frame_num,
                                void                  *buf,
                                uint16_t               required_len,
                                uint16_t              *actual_len)
{
    APP_PRINT_TRACE2("app_bisto_audio_async_read: buf 0x%08x, required_len %d", buf, required_len);

    app_bisto_audio_dump_mic_data("app_bisto_audio_async_read", buf, required_len);

    bisto_audio_mic_cap_data_update(app_bisto_audio.p_audio, buf, required_len);

    *actual_len = required_len;

    return true;
}



BISTO_ERR app_bisto_audio_mic_start_pseudo(BISTO_AUDIO *p_audio,
                                           const GSoundTargetAudioInSettings *p_audio_in_settings)
{
    APP_PRINT_TRACE0("aapp_bisto_audio_mic_start_pseudo");

    memcpy(&app_bisto_audio.gsound_audio_settings, p_audio_in_settings, sizeof(*p_audio_in_settings));

    if (app_bisto_audio.handle != NULL)
    {
        APP_PRINT_ERROR0("app_bisto_audio_mic_start_pseudo: mic already start");
        app_bisto_msg_send(IO_BISTO_MIC_STOP_MSG, NULL);
        app_bisto_msg_send(IO_BISTO_MIC_START_MSG, NULL);
    }
    else
    {
        app_bisto_msg_send(IO_BISTO_MIC_START_MSG, NULL);
    }


    return BISTO_OK;
}


void app_bisto_audio_mic_start(void)
{
    T_AUDIO_FORMAT_INFO format = {};

    APP_PRINT_TRACE4("app_bisto_mic_cap_start: enable_sidetone %d, include_sbc_headers %d, raw_samples_required %d, sbc_bitpool %d",
                     app_bisto_audio.gsound_audio_settings.enable_sidetone,
                     app_bisto_audio.gsound_audio_settings.include_sbc_headers,
                     app_bisto_audio.gsound_audio_settings.raw_samples_required,
                     app_bisto_audio.gsound_audio_settings.sbc_bitpool);


    format.type = AUDIO_FORMAT_TYPE_SBC;
    format.frame_num = 1;
    format.attr.sbc.allocation_method = 0;
    format.attr.sbc.bitpool = app_bisto_audio.gsound_audio_settings.sbc_bitpool;
    format.attr.sbc.block_length = 16;
    format.attr.sbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_MONO;
    format.attr.sbc.sample_rate = 16000;
    format.attr.sbc.subband_num = 8;
    format.attr.sbc.chann_location = AUDIO_CHANNEL_LOCATION_MONO;

    app_bisto_audio.handle = audio_track_create(AUDIO_STREAM_TYPE_RECORD,
                                                AUDIO_STREAM_MODE_NORMAL,
                                                AUDIO_STREAM_USAGE_LOCAL,
                                                format,
                                                0,
                                                app_cfg_const.record_volume_default,
                                                AUDIO_DEVICE_IN_MIC,
                                                NULL,
                                                app_bisto_audio_async_read);

    audio_track_start(app_bisto_audio.handle);
}


BISTO_ERR app_bisto_audio_mic_close_pseudo(BISTO_AUDIO *p_audio)
{
    APP_PRINT_TRACE0("app_bisto_audio_mic_close_pseudo");

    if (app_bisto_audio.handle == NULL)
    {
        APP_PRINT_ERROR0("app_bisto_audio_mic_close_pseudo: mic already stop");
        return BISTO_OTHER_ERR;
    }

    app_bisto_msg_send(IO_BISTO_MIC_STOP_MSG, NULL);

    return BISTO_OK;
}


void app_bisto_audio_mic_close(void)
{
    APP_PRINT_TRACE0("app_bisto_mic_cap_close");
    audio_track_stop(app_bisto_audio.handle);
    app_bisto_audio.handle = NULL;
    memset(&app_bisto_audio.gsound_audio_settings, 0, sizeof(app_bisto_audio.gsound_audio_settings));

}


void app_bisto_audio_init(void)
{
    BISTO_AUDIO_CBS cbs;
    cbs.ringtone_init = app_bisto_audio_ringtone_init;
    cbs.ringtone_play = app_bisto_audio_ringtone_play;
    cbs.mic_cap_start = app_bisto_audio_mic_start_pseudo;
    cbs.mic_cap_close = app_bisto_audio_mic_close_pseudo;

    app_bisto_audio.p_audio = bisto_audio_init(&cbs);
}


#endif
