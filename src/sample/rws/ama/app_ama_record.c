#if AMA_FEATURE_SUPPORT

#include "app_ama.h"
#include "app_ama_record.h"
#include "ama_service_api.h"

#include "app_cfg.h"
#include "app_main.h"
#include "app_a2dp.h"
#include "audio_track.h"
#include "trace.h"
#include "vad.h"


typedef struct
{
    bool is_start;
    uint8_t bd_addr[BD_ADDR_LENGTH];
    T_AUDIO_TRACK_HANDLE handle;
    AudioFormat format;
} APP_AMA_RECORD;


/*============================================================================*
  *                                        Variables
  *============================================================================*/
static APP_AMA_RECORD ama_record = {.is_start = false, .bd_addr = {0}, .handle = NULL, .format = (AudioFormat)0xff};


#if 1
static void app_ama_record_dump_record_data(const char *title, uint8_t *record_data_buf,
                                            uint32_t data_len)
{
    const uint32_t bat_num = 8;
    uint32_t times = data_len / bat_num;
    uint32_t residue = data_len % bat_num;
    uint8_t *residue_buf = record_data_buf + times * bat_num;

    APP_PRINT_TRACE3("app_ama_record_dump_record_data: data_len %d, times %d, residue %d", data_len,
                     times, residue);

    APP_PRINT_TRACE1("@@@@@@@@@@@@@@@@@@@@@%s@@@@@@@@@@@@@@@@@@@@@@@@@@@", TRACE_STRING(title));

    for (int32_t i = 0; i < times; i++)
    {
        APP_PRINT_TRACE8("app_ama_record_dump_record_data: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
                         record_data_buf[i * bat_num], record_data_buf[i * bat_num + 1], record_data_buf[i * bat_num + 2],
                         record_data_buf[i * bat_num + 3],
                         record_data_buf[i * bat_num + 4], record_data_buf[i * bat_num + 5],
                         record_data_buf[i * bat_num + 6],
                         record_data_buf[i * bat_num + 7]);
    }

    switch (residue)
    {
    case 1:
        APP_PRINT_TRACE1("app_ama_record_dump_record_data: 0x%02x\r\n", residue_buf[0]);
        break;
    case 2:
        APP_PRINT_TRACE2("app_ama_record_dump_record_data: 0x%02x, 0x%02x\r\n", residue_buf[0],
                         residue_buf[1]);
        break;
    case 3:
        APP_PRINT_TRACE3("app_ama_record_dump_record_data: 0x%02x, 0x%02x, 0x%02x\r\n", residue_buf[0],
                         residue_buf[1],
                         residue_buf[2]);
        break;
    case 4:
        APP_PRINT_TRACE4("app_ama_record_dump_record_data: 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
                         residue_buf[0],
                         residue_buf[1], residue_buf[2], residue_buf[3]);
        break;
    case 5:
        APP_PRINT_TRACE5("app_ama_record_dump_record_data: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
                         residue_buf[0], residue_buf[1], residue_buf[2], residue_buf[3], residue_buf[4]);
        break;
    case 6:
        APP_PRINT_TRACE6("app_ama_record_dump_record_data: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
                         residue_buf[0], residue_buf[1], residue_buf[2], residue_buf[3], residue_buf[4], residue_buf[5]);
        break;
    case 7:
        APP_PRINT_TRACE7("app_ama_record_dump_record_data: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
                         residue_buf[0], residue_buf[1], residue_buf[2], residue_buf[3], residue_buf[4], residue_buf[5],
                         residue_buf[6]);
        break;

    default:
        break;
    }

    APP_PRINT_TRACE0("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
}
#else

static void app_ama_record_dump_record_data(const char *title, uint8_t *record_data_buf,
                                            uint32_t data_len)
{

}

#endif


bool app_ama_record_read_cb(T_AUDIO_TRACK_HANDLE   handle,
                            uint32_t              *timestamp,
                            uint16_t              *seq_num,
                            T_AUDIO_STREAM_STATUS *status,
                            uint8_t               *frame_num,
                            void                  *buf,
                            uint16_t               required_len,
                            uint16_t              *actual_len)
{
    APP_PRINT_TRACE2("app_ama_record_read_cb: buf 0x%08x, required_len %d", buf, required_len);

    uint8_t app_idx = app_a2dp_get_active_idx();

    app_ama_record_dump_record_data("app_ama_record_read_cb", buf, required_len);

    if (AMA_ERROR == ama_process_voice_data(app_db.br_link[app_idx].bd_addr, (uint8_t *)buf,
                                            required_len))
    {
        return false;
    }

    *actual_len = required_len;

    return true;
}


static void app_ama_record_voice_gen_format_info(T_AUDIO_FORMAT_INFO *p_format_info,
                                                 AudioFormat format_)
{
    APP_PRINT_TRACE1("app_ama_record_voice_gen_format_info: format %d", format_);

    switch (format_)
    {
    case AudioFormat_OPUS_16KHZ_32KBPS_CBR_0_20MS:
        p_format_info->type = AUDIO_FORMAT_TYPE_OPUS;
        p_format_info->frame_num = 1;
        p_format_info->attr.opus.sample_rate = 16 * 1000;
        p_format_info->attr.opus.chann_num = 1;
        p_format_info->attr.opus.cbr = 1;
        p_format_info->attr.opus.cvbr = 0;
        p_format_info->attr.opus.mode = 3;
        p_format_info->attr.opus.complexity = 3;
        p_format_info->attr.opus.frame_duration = AUDIO_OPUS_FRAME_DURATION_20_MS;
        p_format_info->attr.opus.entropy = 0;
        p_format_info->attr.opus.bitrate = 32 * 1000;
        p_format_info->attr.opus.chann_location = AUDIO_CHANNEL_LOCATION_MONO;
        break;
    case AudioFormat_OPUS_16KHZ_16KBPS_CBR_0_20MS:
        p_format_info->type = AUDIO_FORMAT_TYPE_OPUS;
        p_format_info->frame_num = 1;
        p_format_info->attr.opus.sample_rate = 16 * 1000;
        p_format_info->attr.opus.chann_num = 1;
        p_format_info->attr.opus.cbr = 1;
        p_format_info->attr.opus.cvbr = 0;
        p_format_info->attr.opus.mode = 3;
        p_format_info->attr.opus.complexity = 3;
        p_format_info->attr.opus.frame_duration = AUDIO_OPUS_FRAME_DURATION_20_MS;
        p_format_info->attr.opus.entropy = 0;
        p_format_info->attr.opus.bitrate = 16 * 1000;
        p_format_info->attr.opus.chann_location = AUDIO_CHANNEL_LOCATION_MONO;
        break;

    default:
        break;
    }
}


void app_ama_record_force_stop_recording(void)
{
    APP_PRINT_TRACE0("app_ama_record_force_stop_recording");
    ama_record.is_start = false;
    audio_track_stop(ama_record.handle);
}


/**
    * @brief        This function can stop the voice capture.
    * @return       void
    */
void app_ama_record_stop_recording(uint8_t bd_addr[6])
{
    if (memcmp(ama_record.bd_addr, bd_addr, sizeof(ama_record.bd_addr)) != 0)
    {
        APP_PRINT_ERROR0("ama_voice_stop_capture: bd_addr is not matched!");
        return;
    }

    if (ama_record.is_start != true)
    {
        return;
    }

    APP_PRINT_TRACE0("app_ama_record_stop_recording");
    ama_record.is_start = false;
    audio_track_stop(ama_record.handle);

#if APP_AMA_VAD_SUPPORT
    if (app_cfg_const.enable_vad == 1)
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
#endif
}

/**
    * @brief        This function can start the voice capture.
    * @return       void
    */
void app_ama_record_start_recording(uint8_t bd_addr[6])
{
    if (ama_record.is_start != false)/*g_voice_data.is_voice_start == false8*/
    {
        return;
    }

    APP_PRINT_INFO0("app_ama_record_start_recording");
    ama_record.is_start = true;
    memcpy(ama_record.bd_addr, bd_addr, sizeof(ama_record.bd_addr));
    audio_track_start(ama_record.handle);
}



void app_ama_record_init(AudioFormat format)
{
    T_AUDIO_FORMAT_INFO format_info = {};
    ama_record.format = format;

    app_ama_record_voice_gen_format_info(&format_info, ama_record.format);
    ama_record.handle = audio_track_create(AUDIO_STREAM_TYPE_RECORD,
                                           AUDIO_STREAM_MODE_NORMAL,
                                           AUDIO_STREAM_USAGE_LOCAL,
                                           format_info,
                                           0,
                                           10,
                                           AUDIO_DEVICE_IN_MIC,
                                           NULL,
                                           app_ama_record_read_cb);

}


#endif
