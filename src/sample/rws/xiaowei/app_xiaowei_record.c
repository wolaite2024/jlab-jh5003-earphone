#if F_APP_XIAOWEI_FEATURE_SUPPORT
#include "string.h"
#include "stdlib.h"

#include "audio.h"
#include "app_bt_sniffing.h"
#include "app_cfg.h"

#include "app_xiaowei.h"
#include "app_xiaowei_device.h"
#include "app_xiaowei_record.h"
#include "xiaowei_protocol.h"

#include "app_main.h"
#include "app_multilink.h"
#include "audio_track.h"
#include "trace.h"
#include "ring_buffer.h"
#include "gap_le.h"
#include "app_a2dp.h"
#include "app_dsp_cfg.h"

#define XIAOWEI_NUM_OF_PKT_TO_SEND        (2)

#define XIAOWEI_VOICE_BUFFER_SIZE         (6+40*5)

typedef struct
{
    bool is_start;
    uint8_t bd_addr[BD_ADDR_LENGTH];
    T_AUDIO_TRACK_HANDLE handle;
    T_XIAOWEI_AUDIO_FORMAT format;
    uint8_t num_le_voice_buf;//number of ble buffer,each buffer is 40 bytes, the maximum number of num_le_voice_buf is 5
    uint8_t num_voice_buf;//number of spp buffer,each buffer is 40 bytes,the maximum number of num_voice_buf is 5
    uint8_t num_start_send;//used for spp data send,the maximum number of num_start_send is 2
    uint8_t *p_buf;
    T_RING_BUFFER voice_buf;
} APP_XIAOWEI_RECORD;

/*============================================================================*
  *                                        Variables
  *============================================================================*/
static APP_XIAOWEI_RECORD xiaowei_record = {.is_start = false, .bd_addr = {0}, .handle = NULL, .format = XIAOWEI_OPUS_16KHZ_16KBPS_CBR_0_20MS};

void xiaowei_spp_process_voice_data(uint8_t bd_addr[6], uint8_t *data, uint16_t len)
{
    if (ring_buffer_write(&xiaowei_record.voice_buf, data, len))
    {
        xiaowei_record.num_voice_buf++;
    }
    else
    {
        APP_PRINT_ERROR0("xiaowei_spp_process_voice_data:voice_buf full");
    }

    if (xiaowei_record.num_voice_buf == 5)
    {
        xiaowei_record.num_voice_buf = 0;
        xiaowei_record.num_start_send++;
    }

    if (xiaowei_record.num_start_send == XIAOWEI_NUM_OF_PKT_TO_SEND)
    {
        uint8_t *p_data_to_send = calloc(1, XIAOWEI_VOICE_BUFFER_SIZE *
                                         XIAOWEI_NUM_OF_PKT_TO_SEND);
        if (p_data_to_send)
        {
            uint32_t actual_len = ring_buffer_read(&xiaowei_record.voice_buf,
                                                   XIAOWEI_VOICE_BUFFER_SIZE * XIAOWEI_NUM_OF_PKT_TO_SEND, p_data_to_send);
            //APP_PRINT_INFO1("xiaowei_spp_process_voice_data: actual_len %d", actual_len);

            app_xiaowei_255_send_voice_data_request(XIAOWEI_CONNECTION_TYPE_SPP, bd_addr, 0xFF, p_data_to_send,
                                                    (uint16_t)actual_len);
            free(p_data_to_send);
        }

        xiaowei_record.num_start_send = 0;
    }
}

void xiaowei_ble_process_voice_data(uint8_t bd_addr[6], uint8_t *data, uint16_t len)
{
    uint8_t ret = 0;
    uint32_t actual_len = 0;
    uint8_t *p_data_to_send = NULL;
    uint16_t xiaowei_payload_mtu = 0;
    T_XIAOWEI_LINK *p_link = app_xiaowei_find_le_link_by_addr(bd_addr);

    if (ring_buffer_write(&xiaowei_record.voice_buf, data, len))
    {
        xiaowei_record.num_le_voice_buf++;
    }
    else
    {
        ret = 1;
    }

    if (p_link == NULL)
    {
        ret = 2;
        goto err;
    }

    app_xiaowei_device_get_mtu(XIAOWEI_CONNECTION_TYPE_BLE, bd_addr, p_link->conn_id,
                               &xiaowei_payload_mtu);

    if (xiaowei_record.num_le_voice_buf == (xiaowei_payload_mtu / 40))
    {
        //the size of the voice data is a multiple of 40, Must be smaller or equal to mtu size
        p_data_to_send = calloc(1, xiaowei_record.num_le_voice_buf * 40);

        if (p_data_to_send)
        {
            actual_len = ring_buffer_read(&xiaowei_record.voice_buf,
                                          xiaowei_record.num_le_voice_buf * 40, p_data_to_send);

            uint8_t credit = 0;
            le_get_gap_param(GAP_PARAM_LE_REMAIN_CREDITS, &credit);
            if (credit)
            {
                app_xiaowei_255_send_voice_data_request(XIAOWEI_CONNECTION_TYPE_BLE, bd_addr, p_link->conn_id,
                                                        p_data_to_send,
                                                        (uint16_t)actual_len);
            }
            else
            {
                ret = 3;
            }
            free(p_data_to_send);
        }
        xiaowei_record.num_le_voice_buf = 0;
    }

err:
    APP_PRINT_INFO3("xiaowei_ble_process_voice_data: xiaowei_payload_mtu %d,actual_len %d, ret %d ",
                    xiaowei_payload_mtu, actual_len, ret);
};

bool xiaowei_process_voice_data(uint8_t bd_addr[6], uint8_t *data, uint16_t len)
{
    T_XIAOWEI_CONNECTION_TYPE way = app_xiaowei_device_get_active_comm_way();

    if (way == XIAOWEI_CONNECTION_TYPE_SPP)
    {
        xiaowei_spp_process_voice_data(bd_addr, data, len);
    }
    else if (way == XIAOWEI_CONNECTION_TYPE_BLE)
    {
        xiaowei_ble_process_voice_data(bd_addr, data, len);
    }

    return true;
}

bool app_xiaowei_record_read_cb(T_AUDIO_TRACK_HANDLE   handle,
                                uint32_t              *timestamp,
                                uint16_t              *seq_num,
                                T_AUDIO_STREAM_STATUS *status,
                                uint8_t               *frame_num,
                                void                  *buf,
                                uint16_t               required_len,
                                uint16_t              *actual_len)
{
    APP_PRINT_TRACE2("app_xiaowei_record_read_cb: buf 0x%08x, required_len %d", buf, required_len);

    if (required_len != 40)
    {
        //APP_PRINT_ERROR0("app_xiaowei_record_read_cb required_len is incorrect");
    }
    else
    {
        uint8_t app_idx = app_a2dp_get_active_idx();

        if (false == xiaowei_process_voice_data(app_db.br_link[app_idx].bd_addr, (uint8_t *)buf,
                                                required_len))
        {
            //APP_PRINT_ERROR0("app_xiaowei_record_read_cb: read_voice_data failed");
            return false;
        }
    }

    *actual_len = required_len;

    return true;
}

static void xiaowei_voice_gen_format_info(T_AUDIO_FORMAT_INFO *p_format_info,
                                          T_XIAOWEI_AUDIO_FORMAT audio_format)
{
    APP_PRINT_TRACE1("xiaowei_voice_gen_format_info: audio_format %d", audio_format);

    switch (audio_format)
    {
    case XIAOWEI_OPUS_16KHZ_16KBPS_CBR_0_20MS:
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

    case XIAOWEI_OPUS_8KHZ_32KBPS_CBR_0_10MS:
        p_format_info->type = AUDIO_FORMAT_TYPE_OPUS;
        p_format_info->frame_num = 1;
        p_format_info->attr.opus.sample_rate = 16 * 1000;
        p_format_info->attr.opus.chann_num = 1;
        p_format_info->attr.opus.cbr = 1;
        p_format_info->attr.opus.cvbr = 0;
        p_format_info->attr.opus.mode = 3;
        p_format_info->attr.opus.complexity = 3;
        p_format_info->attr.opus.frame_duration = AUDIO_OPUS_FRAME_DURATION_10_MS;
        p_format_info->attr.opus.entropy = 0;
        p_format_info->attr.opus.bitrate = 32 * 1000;
        p_format_info->attr.opus.chann_location = AUDIO_CHANNEL_LOCATION_MONO;
        break;

    default:
        //APP_PRINT_ERROR0("gen_format_info: only support XIAOWEI_OPUS_16KHZ_16KBPS_CBR_0_20MS");
        break;
    }
}

void app_xiaowei_force_stop_recording(void)
{
    APP_PRINT_TRACE0("app_xiaowei_force_stop_recording");
    xiaowei_record.is_start = false;
    xiaowei_record.num_voice_buf = 0;
    xiaowei_record.num_start_send = 0;
    audio_track_stop(xiaowei_record.handle);
}

/**
    * @brief        This function can stop the voice capture.
    * @return       void
    */
void app_xiaowei_stop_recording(uint8_t bd_addr[6])
{
    uint8_t ret = 0;

    if (memcmp(xiaowei_record.bd_addr, bd_addr, sizeof(xiaowei_record.bd_addr)) != 0)
    {
        ret = 1;
        goto err;
    }

    if (xiaowei_record.is_start != true)
    {
        ret = 2;
        goto err;
    }

    xiaowei_record.is_start = false;
    ring_buffer_clear(&xiaowei_record.voice_buf);
    xiaowei_record.num_le_voice_buf = 0;
    xiaowei_record.num_start_send = 0;
    xiaowei_record.num_voice_buf = 0;
    audio_track_stop(xiaowei_record.handle);

err:
    APP_PRINT_TRACE1("app_xiaowei_stop_recording: ret %d", ret);
}

/**
    * @brief        This function can start the voice capture.
    * @return       void
    */
void app_xiaowei_start_recording(uint8_t bd_addr[6])
{
    if (xiaowei_record.is_start != false) /*g_voice_data.is_voice_start == false*/
    {
        //APP_PRINT_ERROR0("app_xiaowei_start_recording: already started");
        return;
    }

    APP_PRINT_INFO0("app_xiaowei_start_recording");
    xiaowei_record.is_start = true;
    memcpy(xiaowei_record.bd_addr, bd_addr, sizeof(xiaowei_record.bd_addr));
    audio_track_start(xiaowei_record.handle);
}

bool app_xiaowei_is_recording(void)
{
    return xiaowei_record.is_start;
}

static void app_xiaowei_record_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_AUDIO_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case AUDIO_EVENT_TRACK_STATE_CHANGED:
        {
            if (param->track_state_changed.handle == xiaowei_record.handle)
            {
                APP_PRINT_INFO1("app_xiaowei_record_cback: track_state_changed.state %d",
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

    default:
        break;
    }
}

void app_xiaowei_record_init(uint8_t app_exp_bit_rate)
{
    uint8_t ret = 0;
    T_AUDIO_FORMAT_INFO format_info = {};
    if (app_exp_bit_rate == 32)
    {
        xiaowei_record.format = XIAOWEI_OPUS_8KHZ_32KBPS_CBR_0_10MS;
    }
    else if (app_exp_bit_rate == 16)
    {
        xiaowei_record.format = XIAOWEI_OPUS_16KHZ_16KBPS_CBR_0_20MS;
    }

    xiaowei_voice_gen_format_info(&format_info, xiaowei_record.format);
    xiaowei_record.handle = audio_track_create(AUDIO_STREAM_TYPE_RECORD,
                                               AUDIO_STREAM_MODE_NORMAL,
                                               AUDIO_STREAM_USAGE_LOCAL,
                                               format_info,
                                               0,
                                               app_dsp_cfg_vol.record_volume_default,
                                               AUDIO_DEVICE_IN_MIC,
                                               NULL,
                                               app_xiaowei_record_read_cb);

    if (xiaowei_record.handle == NULL)
    {
        ret = 1;
    }
    xiaowei_record.p_buf = calloc(1,
                                  XIAOWEI_VOICE_BUFFER_SIZE * XIAOWEI_NUM_OF_PKT_TO_SEND + 1);
    xiaowei_record.num_voice_buf = 0;
    ring_buffer_init(&xiaowei_record.voice_buf, xiaowei_record.p_buf,
                     XIAOWEI_VOICE_BUFFER_SIZE * XIAOWEI_NUM_OF_PKT_TO_SEND + 1);

    audio_mgr_cback_register(app_xiaowei_record_cback);
    APP_PRINT_INFO2("app_xiaowei_record_init:app_exp_bit_rate %d Kbps, ret %d", app_exp_bit_rate, ret);
}

#endif
