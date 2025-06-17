#if XM_XIAOAI_FEATURE_SUPPORT
#include "string.h"

#include "xiaoai_mem.h"
#include "audio.h"
#include "app_bt_sniffing.h"
#include "app_cfg.h"

#include "app_xm_xiaoai.h"
#include "app_xiaoai_device.h"
#include "app_xiaoai_record.h"
#include "xm_xiaoai_api.h"

#include "app_main.h"
#include "app_multilink.h"
#include "audio_track.h"
#include "trace.h"
#include "ring_buffer.h"
#if F_APP_LISTENING_MODE_SUPPORT
#include "app_listening_mode.h"
#endif
#include "app_a2dp.h"
#include "app_dsp_cfg.h"
#include "bt_types.h"

#define XIAOAI_NUM_OF_PKT_TO_SEND 2
#define XIAOAI_VOICE_BUFFER_SIZE (6+40*5)

typedef struct
{
    bool is_start;
    uint8_t bd_addr[BD_ADDR_LENGTH];
    T_AUDIO_TRACK_HANDLE handle;
    T_XIAOAI_AUDIO_FORMAT format;
    uint8_t num_voice_buf;
    uint8_t num_start_send;
    uint8_t *p_buf;
    T_RING_BUFFER voice_buf;
} APP_XIAOAI_RECORD;

/*============================================================================*
  *                                        Variables
  *============================================================================*/
static APP_XIAOAI_RECORD xiaoai_record = {.is_start = false, .bd_addr = {0}, .handle = NULL, .format = XIAOAI_OPUS_16KHZ_16KBPS_CBR_0_20MS};

#if 0
static void xiaoai_dump_record_data(const char *title, uint8_t *record_data_buf, uint32_t data_len)
{
    const uint32_t bat_num = 8;
    uint32_t times = data_len / bat_num;
    uint32_t residue = data_len % bat_num;
    uint8_t *residue_buf = record_data_buf + times * bat_num;

    APP_PRINT_TRACE3("ama_dump_record_data: data_len %d, times %d, residue %d", data_len,
                     times, residue);
    APP_PRINT_TRACE2("ama_dump_record_data: record_data_buf is 0x%08x, residue_buf is 0x%08x\r\n",
                     (uint32_t)record_data_buf,
                     (uint32_t)residue_buf);

    APP_PRINT_TRACE1("@@@@@@@@@@@@@@@@@@@@@%s@@@@@@@@@@@@@@@@@@@@@@@@@@@", TRACE_STRING(title));

    for (int32_t i = 0; i < times; i++)
    {
        APP_PRINT_TRACE8("ama_dump_record_data: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
                         record_data_buf[i * bat_num], record_data_buf[i * bat_num + 1], record_data_buf[i * bat_num + 2],
                         record_data_buf[i * bat_num + 3],
                         record_data_buf[i * bat_num + 4], record_data_buf[i * bat_num + 5],
                         record_data_buf[i * bat_num + 6],
                         record_data_buf[i * bat_num + 7]);
    }

    switch (residue)
    {
    case 1:
        APP_PRINT_TRACE1("ama_dump_record_data: 0x%02x\r\n", residue_buf[0]);
        break;
    case 2:
        APP_PRINT_TRACE2("ama_dump_record_data: 0x%02x, 0x%02x\r\n", residue_buf[0], residue_buf[1]);
        break;
    case 3:
        APP_PRINT_TRACE3("ama_dump_record_data: 0x%02x, 0x%02x, 0x%02x\r\n", residue_buf[0], residue_buf[1],
                         residue_buf[2]);
        break;
    case 4:
        APP_PRINT_TRACE4("ama_dump_record_data: 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n", residue_buf[0],
                         residue_buf[1], residue_buf[2], residue_buf[3]);
        break;
    case 5:
        APP_PRINT_TRACE5("ama_dump_record_data: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
                         residue_buf[0], residue_buf[1], residue_buf[2], residue_buf[3], residue_buf[4]);
        break;
    case 6:
        APP_PRINT_TRACE6("ama_dump_record_data: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
                         residue_buf[0], residue_buf[1], residue_buf[2], residue_buf[3], residue_buf[4], residue_buf[5]);
        break;
    case 7:
        APP_PRINT_TRACE7("ama_dump_record_data: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
                         residue_buf[0], residue_buf[1], residue_buf[2], residue_buf[3], residue_buf[4], residue_buf[5],
                         residue_buf[6]);
        break;

    default:
        break;
    }

    APP_PRINT_TRACE0("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
}
#else

static void xiaoai_dump_record_data(const char *title, uint8_t *record_data_buf, uint32_t data_len)
{
}

#endif

bool xiaoai_process_voice_data(uint8_t bd_addr[6], uint8_t *data, uint16_t len)
{
    if (xiaoai_record.num_voice_buf == 0)
    {
        uint8_t pkt_head[4];
        uint8_t *p_pkt_head = pkt_head;

        uint8_t frame_total_len[2];
        uint8_t *p_n_frame_total_len = frame_total_len;

        BE_UINT32_TO_STREAM(p_pkt_head, 0xAAEABDAC);
        LE_UINT16_TO_STREAM(p_n_frame_total_len, 40);

        ring_buffer_write(&xiaoai_record.voice_buf, pkt_head, 4);
        ring_buffer_write(&xiaoai_record.voice_buf, frame_total_len, 2);
    }

    if (ring_buffer_write(&xiaoai_record.voice_buf, data, len))
    {
        xiaoai_record.num_voice_buf++;
    }
    else
    {
        APP_PRINT_WARN0("xiaoai_process_voice_data: xiaoai_record.voice_buf is full, drop pkt");
    }

    if (xiaoai_record.num_voice_buf == 5)
    {
        xiaoai_record.num_voice_buf = 0;
        xiaoai_record.num_start_send++;
    }

    T_XM_CMD_COMM_WAY_TYPE way = app_xiaoai_device_get_active_comm_way();

    if (way == T_XM_CMD_COMMUNICATION_WAY_TYPE_SPP)
    {
        if (xiaoai_record.num_start_send == XIAOAI_NUM_OF_PKT_TO_SEND)
        {
            uint8_t *p_data_to_send = xiaoai_mem_zalloc(XIAOAI_VOICE_BUFFER_SIZE * XIAOAI_NUM_OF_PKT_TO_SEND);
            if (p_data_to_send)
            {
                uint32_t actual_len = ring_buffer_read(&xiaoai_record.voice_buf,
                                                       XIAOAI_VOICE_BUFFER_SIZE * XIAOAI_NUM_OF_PKT_TO_SEND, p_data_to_send);
                APP_PRINT_INFO1("xiaoai_process_voice_data: actual_len %d sent", actual_len);

                xm_cmd_speech_data(way, bd_addr, 0xFF, p_data_to_send,
                                   (uint16_t)actual_len);

                xiaoai_mem_free(p_data_to_send, XIAOAI_VOICE_BUFFER_SIZE * XIAOAI_NUM_OF_PKT_TO_SEND);
            }

            xiaoai_record.num_start_send = 0;
        }
    }
    else if (way == T_XM_CMD_COMMUNICATION_WAY_TYPE_BLE)
    {
        uint8_t *p_data_to_send = xiaoai_mem_zalloc(46);
        if (p_data_to_send)
        {
            uint32_t actual_len = ring_buffer_read(&xiaoai_record.voice_buf, 46, p_data_to_send);
            APP_PRINT_INFO1("xiaoai_process_voice_data: actual_len %d sent", actual_len);

            T_XM_XIAOAI_LINK *p_link = app_xiaoai_find_le_link_by_addr(bd_addr);

            xm_cmd_speech_data(way, bd_addr, p_link->conn_id, p_data_to_send,
                               (uint16_t)actual_len);

            xiaoai_mem_free(p_data_to_send, 46);
        }
    }

    return true;
}

bool app_xiaoai_record_read_cb(T_AUDIO_TRACK_HANDLE   handle,
                               uint32_t              *timestamp,
                               uint16_t              *seq_num,
                               T_AUDIO_STREAM_STATUS *status,
                               uint8_t               *frame_num,
                               void                  *buf,
                               uint16_t               required_len,
                               uint16_t              *actual_len)
{
    APP_PRINT_TRACE1("app_xiaoai_record_read_cb: required_len %d", required_len);

    if (required_len != 40)
    {
        APP_PRINT_ERROR0("app_xiaoai_record_read_cb required_len is incorrect");
    }
    else
    {
        uint8_t app_idx = app_a2dp_get_active_idx();

        xiaoai_dump_record_data("app_xiaoai_record_read_cb", buf, required_len);

        if (false == xiaoai_process_voice_data(app_db.br_link[app_idx].bd_addr, (uint8_t *)buf,
                                               required_len))
        {
            return false;
        }
    }

    *actual_len = required_len;

    return true;
}

static void xiaoai_voice_gen_format_info(T_AUDIO_FORMAT_INFO *p_format_info,
                                         T_XIAOAI_AUDIO_FORMAT audio_format)
{
    switch (audio_format)
    {
    case XIAOAI_OPUS_16KHZ_16KBPS_CBR_0_20MS:
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
        APP_PRINT_ERROR0("gen_format_info: only support XIAOAI_OPUS_16KHZ_16KBPS_CBR_0_20MS");
        break;
    }
}

void app_xiaoai_force_stop_recording(void)
{
    APP_PRINT_TRACE0("app_xiaoai_force_stop_recording");
    xiaoai_record.is_start = false;
    xiaoai_record.num_voice_buf = 0;
    xiaoai_record.num_start_send = 0;
    audio_track_stop(xiaoai_record.handle);
}

/**
    * @brief        This function can stop the voice capture.
    * @return       void
    */
void app_xiaoai_stop_recording(uint8_t bd_addr[6])
{
    if (memcmp(xiaoai_record.bd_addr, bd_addr, sizeof(xiaoai_record.bd_addr)) != 0)
    {
        APP_PRINT_ERROR0("app_xiaoai_stop_recording: bd_addr is not matched!");
        return;
    }

    if (xiaoai_record.is_start != true)
    {
        APP_PRINT_ERROR0("app_xiaoai_stop_recording: already stopped!");
        return;
    }

    APP_PRINT_TRACE0("app_xiaoai_stop_recording");
    xiaoai_record.is_start = false;
    audio_track_stop(xiaoai_record.handle);

#if F_APP_LISTENING_MODE_SUPPORT
    app_listening_special_event_trigger(LISTENING_MODE_SPECIAL_EVENT_XIAOAI_END);
#endif
}

/**
    * @brief        This function can start the voice capture.
    * @return       void
    */
void app_xiaoai_start_recording(uint8_t bd_addr[6])
{
    if (xiaoai_record.is_start != false) /*g_voice_data.is_voice_start == false8*/
    {
        APP_PRINT_ERROR0("app_xiaoai_start_recording: already started");
        return;
    }

#if F_APP_LISTENING_MODE_SUPPORT
    app_listening_special_event_trigger(LISTENING_MODE_SPECIAL_EVENT_XIAOAI);
#endif

    APP_PRINT_INFO0("app_xiaoai_start_recording");
    xiaoai_record.is_start = true;
    memcpy(xiaoai_record.bd_addr, bd_addr, sizeof(xiaoai_record.bd_addr));
    audio_track_start(xiaoai_record.handle);
}

bool app_xiaoai_is_recording(void)
{
    return xiaoai_record.is_start;
}

static void app_xiaoai_record_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_AUDIO_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case AUDIO_EVENT_TRACK_STATE_CHANGED:
        {
            if (param->track_state_changed.handle == xiaoai_record.handle)
            {
                APP_PRINT_INFO1("app_xiaoai_record_cback: track_state_changed.state %d",
                                param->track_state_changed.state);

                switch (param->track_state_changed.state)
                {
                case AUDIO_TRACK_STATE_STARTED:
                    {
                        app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_BLE_ACTIVE_CHANGE);
                    }
                    break;

                case AUDIO_TRACK_STATE_STOPPED:
                    {
                        app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_BLE_ACTIVE_CHANGE);
                    }
                    break;

                default:
                    break;
                }
            }
        }
        break;
    }
}

void app_xiaoai_record_init(void)
{
    T_AUDIO_FORMAT_INFO format_info = {};
    xiaoai_voice_gen_format_info(&format_info, xiaoai_record.format);
    xiaoai_record.handle = audio_track_create(AUDIO_STREAM_TYPE_RECORD,
                                              AUDIO_STREAM_MODE_NORMAL,
                                              AUDIO_STREAM_USAGE_LOCAL,
                                              format_info,
                                              0,
                                              app_dsp_cfg_vol.record_volume_default,
                                              AUDIO_DEVICE_IN_MIC,
                                              NULL,
                                              app_xiaoai_record_read_cb);

    if (xiaoai_record.handle == NULL)
    {
        APP_PRINT_ERROR0("app_xiaoai_record_init: handle is NULL");
    }
    xiaoai_record.p_buf = xiaoai_mem_zalloc(XIAOAI_VOICE_BUFFER_SIZE * XIAOAI_NUM_OF_PKT_TO_SEND + 1);
    xiaoai_record.num_voice_buf = 0;
    ring_buffer_init(&xiaoai_record.voice_buf, xiaoai_record.p_buf,
                     XIAOAI_VOICE_BUFFER_SIZE * XIAOAI_NUM_OF_PKT_TO_SEND + 1);

    audio_mgr_cback_register(app_xiaoai_record_cback);
}

#endif
