#if DMA_FEATURE_SUPPORT
#include "app_dma_record.h"
#include "stdint.h"
#include "stdbool.h"
#include "audio_track.h"
#include "trace.h"


struct
{
    bool is_recording;
    AudioFormat au_format;
    T_AUDIO_TRACK_HANDLE handle;
} app_dma_record_mgr = {.is_recording = false, .au_format = AUDIO_FORMAT__OPUS_16KHZ_32KBPS_CBR_0_20MS};


void app_dma_record_start(void)
{
    if (app_dma_record_mgr.is_recording != false)/*g_voice_data.is_voice_start == false8*/
    {
        APP_PRINT_ERROR0("app_dma_record_start: already started");
        return;
    }

    APP_PRINT_INFO0("app_dma_record_start");
    app_dma_record_mgr.is_recording = true;
    audio_track_start(app_dma_record_mgr.handle);
}


void app_dma_record_stop(void)
{
    APP_PRINT_TRACE0("app_ama_record_force_stop_recording");
    app_dma_record_mgr.is_recording = false;
    audio_track_stop(app_dma_record_mgr.handle);
}


static void app_dma_reocrd_gen_format_info(T_AUDIO_FORMAT_INFO *p_format_info, AudioFormat fmt)
{
    APP_PRINT_TRACE1("app_dma_record_gen_format_info: format %d", fmt);

    switch (fmt)
    {
    case AUDIO_FORMAT__OPUS_16KHZ_32KBPS_CBR_0_20MS:
        p_format_info->type = AUDIO_FORMAT_TYPE_OPUS;
        p_format_info->frame_num = 1;
        p_format_info->attr.opus.sample_rate = 16 * 1000;
        p_format_info->attr.opus.chann_num = 1;
        p_format_info->attr.opus.cbr = 1;
        p_format_info->attr.opus.cvbr = 0;
        p_format_info->attr.opus.mode = 3;
        p_format_info->attr.opus.complexity = 0;
        p_format_info->attr.opus.frame_duration = AUDIO_OPUS_FRAME_DURATION_20_MS;
        p_format_info->attr.opus.entropy = 0;
        p_format_info->attr.opus.bitrate = 32 * 1000;
        p_format_info->attr.opus.chann_location = AUDIO_CHANNEL_LOCATION_MONO;
        break;

    case AUDIO_FORMAT__OPUS_16KHZ_16KBPS_CBR_0_20MS:
        p_format_info->type = AUDIO_FORMAT_TYPE_OPUS;
        p_format_info->frame_num = 1;
        p_format_info->attr.opus.sample_rate = 16 * 1000;
        p_format_info->attr.opus.chann_num = 1;
        p_format_info->attr.opus.cbr = 1;
        p_format_info->attr.opus.cvbr = 0;
        p_format_info->attr.opus.mode = 3;
        p_format_info->attr.opus.complexity = 0;
        p_format_info->attr.opus.frame_duration = AUDIO_OPUS_FRAME_DURATION_20_MS;
        p_format_info->attr.opus.entropy = 0;
        p_format_info->attr.opus.bitrate = 16 * 1000;
        p_format_info->attr.opus.chann_location = AUDIO_CHANNEL_LOCATION_MONO;
        break;

    case AUDIO_FORMAT__MSBC:
        p_format_info->type = AUDIO_FORMAT_TYPE_MSBC;
        p_format_info->frame_num = 1;
        p_format_info->attr.msbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_MONO;
        p_format_info->attr.msbc.sample_rate = 16000;
        p_format_info->attr.msbc.bitpool = 26;
        p_format_info->attr.msbc.allocation_method = 0;
        p_format_info->attr.msbc.subband_num = 8;
        p_format_info->attr.msbc.block_length = 15;
        p_format_info->attr.msbc.chann_location = AUDIO_CHANNEL_LOCATION_MONO;
        break;

    default:
        break;
    }

}


static bool app_dma_record_read_cb(T_AUDIO_TRACK_HANDLE  handle,
                                   uint32_t             *timestamp,
                                   uint16_t             *seq_num,
                                   T_AUDIO_STREAM_STATUS *status,
                                   uint8_t              *frame_num,
                                   void                 *buf,
                                   uint16_t              required_len,
                                   uint16_t             *actual_len)
{
    return true;
}


AudioFormat app_dma_record_get_format(void)
{
    return app_dma_record_mgr.au_format;
}


void app_dma_record_init(void)
{
    T_AUDIO_FORMAT_INFO format_info = {(T_AUDIO_FORMAT_TYPE)0, {0}};
    app_dma_record_gen_format_info(&format_info, app_dma_record_mgr.au_format);
    app_dma_record_mgr.handle = audio_track_create(AUDIO_STREAM_TYPE_RECORD,
                                                   AUDIO_STREAM_MODE_NORMAL,
                                                   AUDIO_STREAM_USAGE_LOCAL,
                                                   format_info,
                                                   0,
                                                   10,
                                                   AUDIO_DEVICE_IN_MIC,
                                                   NULL,
                                                   app_dma_record_read_cb);
}

#endif
