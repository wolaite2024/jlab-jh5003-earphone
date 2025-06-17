/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "os_queue.h"
#include "trace.h"
#include "fmc_api.h"
#include "audio_route_cfg.h"
#include "audio_path.h"
#include "audio_mgr.h"
#include "ringtone.h"
#include "voice_prompt.h"
#include "tts.h"
#include "notification.h"
#include "notification_buffer.h"
#include "patch_header_check.h"
#include "storage.h"
#include "remote.h"
#include "bt_mgr.h"

#define NOTIFICATION_VOLUME_MAX         (15)
#define NOTIFICATION_VOLUME_MIN         (0)
#define NOTIFICATION_VOLUME_DEFAULT     (10)

#define NOTIFICATION_STREAM_STATE_IDLE          0x00
#define NOTIFICATION_STREAM_STATE_STARTED       0x01
#define NOTIFICATION_STREAM_STATE_CONTINUED     0x02
#define NOTIFICATION_STREAM_STATE_STOPPED       0x03

#define NOTIFICATION_QUEUE_NUM      0x08

#define VP_FILE_HDR_LEN         (10)
#define VP_FILE_CFG_OFFSET      (6)
#define VP_FILE_CFG_SIZE        (4)
#define VP_FRAME_HDR_LEN        (2)

#define NOTIFICATION_REMOTE_HANDSHAKE_TIMEOUT   2000000
#define NOTIFICATION_REMOTE_POLL_TIMEOUT        5000000
#define NOTIFICATION_REMOTE_ACK_TIMEOUT         5000000

typedef enum t_notification_remote_msg
{
    NOTIFICATION_REMOTE_START           = 0x0101,
    NOTIFICATION_REMOTE_POLL            = 0x0102,
    NOTIFICATION_REMOTE_ACK             = 0x0103,
    NOTIFICATION_REMOTE_CANCEL          = 0x0104,
    NOTIFICATION_REMOTE_FLUSH           = 0x0105,
    NOTIFICATION_REMOTE_CLK_REQ         = 0x0108,
    NOTIFICATION_REMOTE_SYNC_ACTION     = 0x0109,
    NOTIFICATION_REMOTE_PUSH_RELAY      = 0x010a,
    NOTIFICATION_REMOTE_ACK_ABT         = 0x001b,
    NOTIFICATION_REMOTE_POLL_ABT        = 0x001c,
} T_NOTIFICATION_REMOTE_MSG;

typedef enum t_notification_timer
{
    NOTIFICATION_TIMER_POLL         = 0x01,
    NOTIFICATION_TIMER_ACK          = 0x02,
    NOTIFICATION_TIMER_HANDSHAKE    = 0x03,
} T_NOTIFICATION_TIMER;

typedef struct t_notification_msg_push_relay
{
    uint8_t type;               /* notification type: ringtone, tts, or voice prompt */
    uint8_t index;              /* notification index for ringtone, voice prompt and tts */
    uint8_t language;           /* voice prompt language id */
} T_NOTIFICATION_MSG_PUSH_RELAY;

typedef struct t_notification_msg_relay_start
{
    uint8_t      type;
    uint8_t      index;
    uint8_t      tid;
    uint8_t      language;
} T_NOTIFICATION_MSG_RELAY_START;

typedef struct t_notification_msg_sync_action
{
    uint32_t    sync_clk;
    uint32_t    init_clk;
    uint8_t     clk_ref;
    uint8_t     ref_role;
    uint8_t     tid;
} T_NOTIFICATION_MSG_SYNC_ACTION;

typedef struct t_notification_msg_flush
{
    uint8_t     type;
    uint8_t     relay;
} T_NOTIFICATION_MSG_FLUSH;

typedef struct t_notification_msg_cancel
{
    uint8_t     type;
    uint8_t     index;
    uint8_t     relay;
} T_NOTIFICATION_MSG_CANCEL;

typedef struct t_vp_data_hdr
{
    uint32_t    offset;
    uint32_t    len;
} T_VP_DATA_HDR;

typedef struct t_tone_data_hdr
{
    uint16_t    offset;
    uint16_t    len;
} T_TONE_DATA_HDR;

typedef struct t_tts_instance
{
    T_AUDIO_PATH_HANDLE         handle;
    uint32_t                    length;
} T_TTS_INSTANCE;

typedef struct t_notification_elem
{
    struct t_notification_elem *next;
    uint8_t                     type;     /* notification type: ringtone, tts, or voice prompt */
    uint8_t                     index;    /* notification index for ringtone, voice prompt and tts */
    uint8_t                     language; /* voice prompt language id */
    bool                        relay;    /* relay notification to remote identical device */
    T_TTS_INSTANCE             *instance; /* tts instance */
} T_NOTIFICATION_ELEM;

typedef struct t_notification_db
{
    T_REMOTE_RELAY_HANDLE       relay_handle;
    T_SYS_TIMER_HANDLE          handshake_timer;
    T_SYS_TIMER_HANDLE          poll_timer;
    T_SYS_TIMER_HANDLE          ack_timer;
    T_OS_QUEUE                  idle_queue;
    T_OS_QUEUE                  pending_queue;
    T_OS_QUEUE                  play_queue;
    T_NOTIFICATION_ELEM         elem[NOTIFICATION_QUEUE_NUM];
    T_NOTIFICATION_STATE        state;
    T_RINGTONE_MODE             tone_mode;
    T_VOICE_PROMPT_MODE         vp_mode;
    T_NOTIFICATION_BUFFER_TYPE  vp_buffer_type;
    T_NOTIFICATION_BUFFER_TYPE  tts_buffer_type;
    uint8_t                     sync_action_flag;
    uint8_t                     sync_action_index;
    uint8_t                     sync_action_type;
    uint8_t                     sync_tid;
    uint8_t                     sync_play_index;
    uint8_t                     relay_type;
    bool                        lock;
    uint8_t                     stream_state;
    uint8_t                     tone_idx;
    uint8_t                     vp_idx;
    T_TTS_INSTANCE             *tts_instance;
    T_AUDIO_PATH_HANDLE         tone_path_handle;
    T_AUDIO_PATH_HANDLE         vp_path_handle;
    uint32_t                    vp_cfg[2];
    uint16_t                    frame_num;
    uint32_t                    vp_flash_offset;
    uint32_t                    vp_flash_len;
    uint8_t                     tone_volume_max;
    uint8_t                     tone_volume_min;
    bool                        tone_volume_muted;
    uint8_t                     tone_volume;
    float                       tone_volume_scale;
    uint8_t                     vp_volume_max;
    uint8_t                     vp_volume_min;
    bool                        vp_volume_muted;
    uint8_t                     vp_volume;
    float                       vp_volume_scale;
    uint8_t                     tts_volume_max;
    uint8_t                     tts_volume_min;
    bool                        tts_volume_muted;
    uint8_t                     tts_volume;
    float                       tts_volume_scale;
    bool                        flow_go;
    uint8_t                     vp_seq_num;
    uint8_t                     tts_seq_num;
} T_NOTIFICATION_DB;

static T_NOTIFICATION_DB *notification_db = NULL;

/* TODO Remove Start */
extern bool bt_clk_compare(uint32_t bt_clk_a,
                           uint32_t bt_clk_b);
bool notification_clk_req(void);
void notification_start(uint8_t  clk_ref,
                        uint32_t timestamp);
void notification_timeout_cback(T_SYS_TIMER_HANDLE handle);
static bool audio_path_voice_prompt_cback(T_AUDIO_PATH_HANDLE handle,
                                          T_AUDIO_PATH_EVENT  event,
                                          uint32_t            param);

static bool audio_path_ringtone_cback(T_AUDIO_PATH_HANDLE handle,
                                      T_AUDIO_PATH_EVENT  event,
                                      uint32_t            param);
/* TODO Remove End */

void notification_set_relay_type(uint8_t type)
{
    notification_db->relay_type = type;
}

void notification_set_state(T_NOTIFICATION_STATE state)
{
    notification_db->state = state;
}

T_NOTIFICATION_STATE notification_get_state(void)
{
    return notification_db->state;
}

void notification_lock(void)
{
    notification_db->lock = true;
}

void notification_unlock(void)
{
    notification_db->lock = false;
}

uint8_t notification_mode_get(uint8_t type)
{
    uint8_t mode;

    switch (type)
    {
    case NOTIFICATION_TYPE_NONE:
        mode = 0;
        break;

    case NOTIFICATION_TYPE_RINGTONE:
        mode = (uint8_t)notification_db->tone_mode;
        break;

    case NOTIFICATION_TYPE_VP:
        mode = (uint8_t)notification_db->vp_mode;
        break;

    case NOTIFICATION_TYPE_TTS:
        mode = 0;
        break;

    default:
        mode = 0;
        break;
    }

    return mode;
}

bool notification_mode_set(uint8_t type,
                           uint8_t mode)
{
    bool ret = true;

    switch (type)
    {
    case NOTIFICATION_TYPE_NONE:
        ret = false;
        break;

    case NOTIFICATION_TYPE_RINGTONE:
        notification_db->tone_mode = (T_RINGTONE_MODE)mode;
        break;

    case NOTIFICATION_TYPE_VP:
        notification_db->vp_mode = (T_VOICE_PROMPT_MODE)mode;
        break;

    case NOTIFICATION_TYPE_TTS:
        ret = false;
        break;

    default:
        ret = false;
        break;
    }

    return ret;
}

static T_NOTIFICATION_ELEM *notification_tts_find(T_TTS_HANDLE handle)
{
    T_NOTIFICATION_ELEM *elem;

    elem = (T_NOTIFICATION_ELEM *)notification_db->play_queue.p_first;
    while (elem != NULL)
    {
        if (elem->type == NOTIFICATION_TYPE_TTS &&
            elem->instance == handle)
        {
            break;
        }

        elem = elem->next;
    }

    return elem;
}

static bool notification_tts_data_req(void)
{
    uint32_t               frame_size;
    uint8_t               *frame;
    T_AUDIO_PATH_DATA_HDR *frame_header;

    if (notification_db->stream_state == NOTIFICATION_STREAM_STATE_STARTED)
    {
        if (audio_path_timestamp_set(notification_db->tts_instance->handle,
                                     BT_CLK_NONE,
                                     0xffffffff,
                                     true) == false)
        {
            return false;
        }
    }

    notification_db->stream_state = NOTIFICATION_STREAM_STATE_CONTINUED;

    if (notification_db->flow_go == true)
    {
        frame = notification_buffer_peek(notification_db->tts_buffer_type, &frame_size);
        if (frame != NULL)
        {
            frame_header = calloc(1, sizeof(T_AUDIO_PATH_DATA_HDR) + frame_size - VP_FRAME_HDR_LEN);
            if (frame_header != NULL)
            {
                memcpy(frame_header->payload, frame + VP_FRAME_HDR_LEN, frame_size - VP_FRAME_HDR_LEN);

                frame_header->seq_num = notification_db->tts_seq_num++;
                frame_header->frame_number = 1;
                frame_header->payload_length = frame_size - VP_FRAME_HDR_LEN;
                frame_header->status = 0;
                frame_header->tail = 0xffffffff;
                frame_header->sync_word = 0x3f3f3f3f;

                if (audio_path_data_send(notification_db->tts_instance->handle,
                                         frame_header,
                                         frame_size + sizeof(T_AUDIO_PATH_DATA_HDR) - VP_FRAME_HDR_LEN) == true)
                {
                    free(frame_header);
                    notification_db->flow_go = false;
                    return notification_buffer_flush(notification_db->tts_buffer_type);
                }
                else
                {
                    free(frame_header);
                }
            }
        }
    }

    return false;
}

static bool notification_buffer_tts_cback(void                        *owner,
                                          T_NOTIFICATION_BUFFER_EVENT  event,
                                          uint32_t                     param)
{
    bool ret = false;

    AUDIO_PRINT_TRACE3("notification_buffer_tts_cback: owner %p, event 0x%02x, param 0x%08x",
                       owner, event, param);

    switch (event)
    {
    case NOTIFICATION_BUFFER_EVENT_PLAY:
        ret = notification_tts_data_req();
        break;

    case NOTIFICATION_BUFFER_EVENT_LOW:
        {
            T_AUDIO_EVENT_PARAM param;

            param.tts_alerted.handle = (T_TTS_HANDLE)owner;
            ret = audio_mgr_event_post(AUDIO_EVENT_TTS_ALERTED, &param, sizeof(param));
        }
        break;

    case NOTIFICATION_BUFFER_EVENT_EMPTY:
        {
            T_AUDIO_EVENT_PARAM param;

            param.tts_empty.handle = (T_TTS_HANDLE)owner;
            ret = audio_mgr_event_post(AUDIO_EVENT_TTS_EMPTY, &param, sizeof(param));
        }
        break;

    case NOTIFICATION_BUFFER_EVENT_E_PLAY:
        ret = notification_tts_data_req();
        break;

    case NOTIFICATION_BUFFER_EVENT_HIGH:
        {
            T_AUDIO_EVENT_PARAM param;

            param.tts_paused.handle = (T_TTS_HANDLE)owner;
            ret = audio_mgr_event_post(AUDIO_EVENT_TTS_PAUSED, &param, sizeof(param));
        }
        break;

    case NOTIFICATION_BUFFER_EVENT_F_PLAY:
        {
            T_AUDIO_EVENT_PARAM param;

            param.tts_resumed.handle = (T_TTS_HANDLE)owner;
            ret = audio_mgr_event_post(AUDIO_EVENT_TTS_RESUMED, &param, sizeof(param));
        }
        break;

    case NOTIFICATION_BUFFER_EVENT_FULL:
        {
            T_AUDIO_EVENT_PARAM param;

            param.tts_exceeded.handle = (T_TTS_HANDLE)owner;
            ret = audio_mgr_event_post(AUDIO_EVENT_TTS_EXCEEDED, &param, sizeof(param));
        }
        break;

    default:
        break;
    }

    return ret;
}

static bool notification_buffer_vp_cback(void                        *owner,
                                         T_NOTIFICATION_BUFFER_EVENT  event,
                                         uint32_t                     param)
{
    bool ret = true;

    AUDIO_PRINT_TRACE3("notification_buffer_vp_cback: owner %p, event 0x%02x, param 0x%08x",
                       owner, event, param);

    switch (event)
    {
    case NOTIFICATION_BUFFER_EVENT_PREQUEUE:
        {
            if (param == 0xFFFF)
            {
                notification_db->vp_flash_offset = 0;
                notification_db->vp_flash_len    = 0;
            }
            else
            {
                notification_db->vp_flash_offset += param;
                notification_db->vp_flash_len    -= param;
            }
        }
        break;

    case NOTIFICATION_BUFFER_EVENT_PLAY:
        if (notification_clk_req() == false)
        {
            notification_start(BT_CLK_NONE, 0xffffffff);
        }
        else
        {
            if (notification_db->handshake_timer != NULL)
            {
                sys_timer_delete(notification_db->handshake_timer);
            }

            notification_db->handshake_timer = sys_timer_create("notification handshake",
                                                                SYS_TIMER_TYPE_LOW_PRECISION,
                                                                NOTIFICATION_TIMER_HANDSHAKE,
                                                                NOTIFICATION_REMOTE_HANDSHAKE_TIMEOUT,
                                                                false,
                                                                notification_timeout_cback);
            if (notification_db->handshake_timer != NULL)
            {
                sys_timer_start(notification_db->handshake_timer);
            }
        }
        break;

    case NOTIFICATION_BUFFER_EVENT_EMPTY:
        {
            notification_db->vp_flash_offset += param;
            notification_db->vp_flash_len    -= param;

            if (notification_db->vp_flash_len != 0)
            {
                ret = notification_buffer_prequeue(notification_db->vp_buffer_type,
                                                   notification_db->vp_flash_offset,
                                                   notification_db->vp_flash_len);
            }
        }
        break;

    case NOTIFICATION_BUFFER_EVENT_LOW:
        {
            if (notification_db->vp_flash_len != 0)
            {
                ret = notification_buffer_prequeue(notification_db->vp_buffer_type,
                                                   notification_db->vp_flash_offset,
                                                   notification_db->vp_flash_len);
            }
        }
        break;

    default:
        ret = false;
        break;
    }

    return ret;
}

#define VOICE_PROMPT_CODER_AAC          (0)
#define VOICE_PROMPT_CODER_SBC          (1)
#define VOICE_PROMPT_CODER_MP3          (2)
static uint32_t notification_vp_sr_convert(uint32_t sr_idx)
{
    uint32_t sample_rate = 0;

    switch (sr_idx)
    {
    case 0:
        sample_rate = 96000;
        break;
    case 1:
        sample_rate = 88200;
        break;
    case 2:
        sample_rate = 64000;
        break;
    case 3:
        sample_rate = 48000;
        break;
    case 4:
        sample_rate = 44100;
        break;
    case 5:
        sample_rate = 32000;
        break;
    case 6:
        sample_rate = 24000;
        break;
    case 7:
        sample_rate = 22050;
        break;
    case 8:
        sample_rate = 16000;
        break;
    case 9:
        sample_rate = 12000;
        break;
    case 10:
        sample_rate = 11025;
        break;
    case 11:
        sample_rate = 8000;
        break;
    }
    return sample_rate;
}

static void notification_vp_format_init(T_AUDIO_FORMAT_INFO *format_info,
                                        uint32_t             vp_cfg)
{
    uint32_t coder_id = vp_cfg & 0xF;
    uint32_t sr_idx = (vp_cfg >> 8) & 0xF;
    uint32_t chann_num = (vp_cfg >> 12) & 0xF;

    if (coder_id == VOICE_PROMPT_CODER_AAC)
    {
        format_info->type = AUDIO_FORMAT_TYPE_AAC;
        format_info->frame_num = 1;
        format_info->attr.aac.sample_rate = 16000;
        format_info->attr.aac.chann_num = 1;
        format_info->attr.aac.transport_format = AUDIO_AAC_TRANSPORT_FORMAT_MP4FF;
        format_info->attr.aac.object_type = AUDIO_AAC_OBJECT_TYPE_NULL;
        format_info->attr.aac.vbr = 1;
        format_info->attr.aac.bitrate = 0;
        format_info->attr.aac.chann_location = AUDIO_CHANNEL_LOCATION_MONO;
    }
    else if (coder_id == VOICE_PROMPT_CODER_SBC)
    {
        format_info->type = AUDIO_FORMAT_TYPE_SBC;
        format_info->attr.sbc.sample_rate = notification_vp_sr_convert(sr_idx);
        if (chann_num == 1)
        {
            format_info->attr.sbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_MONO;
            format_info->attr.sbc.chann_location = AUDIO_CHANNEL_LOCATION_MONO;
        }
        else
        {
            format_info->attr.sbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_STEREO;
            format_info->attr.sbc.chann_location = AUDIO_CHANNEL_LOCATION_FL | AUDIO_CHANNEL_LOCATION_FR;
        }
    }
    else if (coder_id == VOICE_PROMPT_CODER_MP3)
    {
        format_info->type = AUDIO_FORMAT_TYPE_MP3;
        format_info->attr.mp3.sample_rate = notification_vp_sr_convert(sr_idx);
        if (chann_num == 1)
        {
            format_info->attr.mp3.chann_mode = AUDIO_MP3_CHANNEL_MODE_MONO;
            format_info->attr.mp3.chann_location = AUDIO_CHANNEL_LOCATION_MONO;
        }
        else
        {
            format_info->attr.mp3.chann_mode = AUDIO_MP3_CHANNEL_MODE_STEREO;
            format_info->attr.mp3.chann_location = AUDIO_CHANNEL_LOCATION_FL | AUDIO_CHANNEL_LOCATION_FR;
        }
    }
}

static void notification_start_vp(uint8_t index,
                                  uint8_t language)
{
    uint32_t      table_size;
    uint32_t      hdr_offset;
    T_VP_DATA_HDR vp_hdr;
    uint8_t       cfg_buf[VP_FILE_HDR_LEN];
    T_STORAGE_PARTITION_INFO info;
    T_AUDIO_PATH_PARAM  path_param;
    T_AUDIO_FORMAT_INFO vp_format;
    int32_t       ret = 0;

    table_size = sizeof(T_VP_DATA_HDR) * audio_route_cfg.voice_prompt_index_num;
    hdr_offset = APP_DATA_SYNC_WORD_LEN + (table_size * language) + (sizeof(T_VP_DATA_HDR) * index);

    AUDIO_PRINT_TRACE3("notification_start_vp: index 0x%02x, index num %u, language %u",
                       index, audio_route_cfg.voice_prompt_index_num, language);

    info = storage_partition_get(VP_PARTITION_NAME);
    if (fmc_flash_nor_read(info.address + hdr_offset, &vp_hdr, sizeof(T_VP_DATA_HDR)) == false)
    {
        ret = 1;
        goto fail_load_hdr;
    }

    notification_db->vp_flash_offset = info.address + vp_hdr.offset;
    notification_db->vp_flash_len    = vp_hdr.len;

    if (fmc_flash_nor_read(notification_db->vp_flash_offset,
                           cfg_buf,
                           VP_FILE_HDR_LEN) == false)
    {
        ret = 2;
        goto fail_load_cfg;
    }

    /* big endian */
    notification_db->frame_num = (uint16_t)((cfg_buf[4] << 8) | cfg_buf[5]);
    notification_db->vp_cfg[0] = (uint32_t)((cfg_buf[6] << 24) | (cfg_buf[7] << 16) |
                                            (cfg_buf[8] << 8) | cfg_buf[9]);

    notification_vp_format_init(&vp_format, notification_db->vp_cfg[0]);

    path_param.vp.device       = AUDIO_DEVICE_OUT_SPK;
    path_param.vp.decoder_info = &vp_format;
    path_param.vp.dac_level    = NOTIFICATION_VOLUME_DEFAULT;

    if (audio_path_create(AUDIO_CATEGORY_VP,
                          path_param,
                          audio_path_voice_prompt_cback) == NULL)
    {
        ret = 3;
        goto fail_create_path;
    }

    notification_buffer_config(notification_db->vp_path_handle,
                               notification_buffer_vp_cback,
                               notification_db->vp_flash_len,
                               75,
                               24,
                               24);

    /* calculate the first frame address offset */
    notification_db->vp_flash_offset += VP_FILE_HDR_LEN;
    notification_db->vp_flash_len    -= VP_FILE_HDR_LEN;

    return;

fail_create_path:
fail_load_cfg:
fail_load_hdr:
    AUDIO_PRINT_ERROR4("notification_start_vp: failed %d, index 0x%02x, index num %u, language %u",
                       -ret, index, audio_route_cfg.voice_prompt_index_num, language);
}

static void notification_start_tts(T_TTS_INSTANCE *instance)
{
    notification_buffer_config(instance,
                               notification_buffer_tts_cback,
                               instance->length,
                               87,
                               25,
                               75);

    if (audio_path_start(instance->handle) == true)
    {
        notification_db->tts_instance = instance;
    }
}

static void notification_start_ringtone(uint8_t index)
{
    T_TONE_DATA_HDR  tone_hdr;
    uint16_t         hdr_offset;
    void            *coeff_buf;
    T_STORAGE_PARTITION_INFO info;

    hdr_offset = APP_DATA_SYNC_WORD_LEN + APP_DATA_TONE_NUM_LEN + (sizeof(T_TONE_DATA_HDR) * index);
    info = storage_partition_get(TONE_PARTITION_NAME);
    fmc_flash_nor_read(info.address + hdr_offset, &tone_hdr, sizeof(T_TONE_DATA_HDR));

    coeff_buf = malloc(tone_hdr.len);
    if (coeff_buf != NULL)
    {
        T_AUDIO_PATH_PARAM  path_param;
        T_RINGTONE_INSTANCE tone_instance;

        notification_db->stream_state = NOTIFICATION_STREAM_STATE_CONTINUED;
        fmc_flash_nor_read(info.address + tone_hdr.offset, coeff_buf, tone_hdr.len);

        tone_instance.coeff               = coeff_buf;
        tone_instance.size                = tone_hdr.len;
        path_param.ringtone.device        = AUDIO_DEVICE_OUT_SPK;
        path_param.ringtone.dac_level     = NOTIFICATION_VOLUME_DEFAULT;
        path_param.ringtone.instance      = &tone_instance;

        if (audio_path_create(AUDIO_CATEGORY_TONE,
                              path_param,
                              audio_path_ringtone_cback) == NULL)
        {
            AUDIO_PRINT_ERROR1("notification_start_ringtone failed, index 0x%02x", index);
        }
        free(coeff_buf);
    }
}

static bool notification_vp_data_req(void)
{
    uint8_t  *p_frame;
    uint32_t  frame_size;
    T_AUDIO_PATH_DATA_HDR *p_frame_header;

    if (notification_db->stream_state == NOTIFICATION_STREAM_STATE_STARTED)
    {
        if (audio_path_timestamp_set(notification_db->vp_path_handle,
                                     notification_db->vp_cfg[0],
                                     notification_db->vp_cfg[1],
                                     true) == false)
        {
            return false;
        }
    }

    notification_db->stream_state = NOTIFICATION_STREAM_STATE_CONTINUED;

    p_frame = notification_buffer_peek(notification_db->vp_buffer_type, &frame_size);
    if (p_frame != NULL)
    {
        p_frame_header = calloc(1, sizeof(T_AUDIO_PATH_DATA_HDR) + frame_size - VP_FRAME_HDR_LEN);
        if (p_frame_header != NULL)
        {
            memcpy(p_frame_header->payload, p_frame + VP_FRAME_HDR_LEN, frame_size - VP_FRAME_HDR_LEN);

            p_frame_header->seq_num = notification_db->vp_seq_num++;
            p_frame_header->frame_number = 1;
            p_frame_header->payload_length = frame_size - VP_FRAME_HDR_LEN;
            p_frame_header->status = 0;
            p_frame_header->tail = 0xffffffff;
            p_frame_header->sync_word = 0x3f3f3f3f;

            if (audio_path_data_send(notification_db->vp_path_handle,
                                     p_frame_header,
                                     frame_size + sizeof(T_AUDIO_PATH_DATA_HDR) - VP_FRAME_HDR_LEN) == true)
            {
                free(p_frame_header);
                return notification_buffer_flush(notification_db->vp_buffer_type);
            }
            else
            {
                free(p_frame_header);
            }
        }
    }

    return false;
}

void notification_path_start(T_NOTIFICATION_ELEM *elem)
{
    if (elem->type == NOTIFICATION_TYPE_RINGTONE)
    {
        notification_set_state(NOTIFICATION_STATE_RINGTONE);
        notification_set_relay_type(NOTIFICATION_TYPE_NONE);
        notification_db->sync_play_index = 0xff;
        notification_db->tone_idx = elem->index;

        notification_start_ringtone(elem->index);
    }
    else if (elem->type == NOTIFICATION_TYPE_VP)
    {
        notification_set_state(NOTIFICATION_STATE_VP);
        notification_set_relay_type(NOTIFICATION_TYPE_NONE);
        notification_db->sync_play_index = 0xff;
        notification_db->vp_idx = elem->index;

        notification_start_vp(elem->index, elem->language);
    }
    else if (elem->type == NOTIFICATION_TYPE_TTS)
    {
        notification_set_state(NOTIFICATION_STATE_TTS);
        notification_set_relay_type(NOTIFICATION_TYPE_NONE);
        notification_db->sync_play_index = 0xff;

        notification_start_tts(elem->instance);
    }
}

void notification_ack_abort(void)
{
    if (notification_db->state == NOTIFICATION_STATE_RELAY)
    {
        notification_set_state(NOTIFICATION_STATE_IDLE);
        notification_set_relay_type(NOTIFICATION_TYPE_NONE);
        notification_db->sync_play_index = 0xff;
    }
}

void notification_poll_abort(void)
{
    if (notification_db->state == NOTIFICATION_STATE_RELAY)
    {
        T_NOTIFICATION_ELEM *elem;

        notification_set_state(NOTIFICATION_STATE_IDLE);
        notification_set_relay_type(NOTIFICATION_TYPE_NONE);
        notification_db->sync_play_index = 0xff;

        elem = os_queue_out(&notification_db->pending_queue);
        while (elem != NULL)
        {
            os_queue_in(&notification_db->play_queue, elem);
            elem = os_queue_out(&notification_db->pending_queue);
        }

        elem = os_queue_peek(&notification_db->play_queue, 0);
        if (elem != NULL)
        {
            if (elem->relay == true)
            {
                AUDIO_PRINT_TRACE4("notification_poll_abort: state %u, type %u, index %u, relay %u",
                                   notification_db->state, elem->type, elem->index, elem->relay);

                elem = os_queue_out(&notification_db->play_queue);
                elem->relay = false;
                notification_path_start(elem);
                os_queue_in(&notification_db->idle_queue, elem);
            }
        }
    }
}

void notification_start(uint8_t  clk_ref,
                        uint32_t timestamp)
{
    if (notification_db->handshake_timer != NULL)
    {
        sys_timer_delete(notification_db->handshake_timer);
        notification_db->handshake_timer = NULL;
    }

    notification_db->sync_action_index = 0xff;
    notification_db->sync_action_flag = 0;

    if (notification_db->state == NOTIFICATION_STATE_VP)
    {
        notification_db->vp_cfg[0] = clk_ref;
        notification_db->vp_cfg[1] = timestamp;
        notification_vp_data_req();
    }
    else if (notification_db->state == NOTIFICATION_STATE_RINGTONE)
    {
        audio_path_timestamp_set(notification_db->tone_path_handle, clk_ref, timestamp, false);
    }
}

void notification_timeout_cback(T_SYS_TIMER_HANDLE handle)
{
    uint32_t timer_id;

    timer_id = sys_timer_id_get(handle);

    AUDIO_PRINT_TRACE2("notification_timeout_cback: handle %p, timer_id 0x%08x",
                       handle, timer_id);

    switch (timer_id)
    {
    case NOTIFICATION_TIMER_POLL:
        {
            notification_poll_abort();
        }
        break;

    case NOTIFICATION_TIMER_ACK:
        {
            notification_ack_abort();
        }
        break;

    case NOTIFICATION_TIMER_HANDSHAKE:
        {
            if (notification_db->sync_action_index != 0xff)
            {
                notification_start(BT_CLK_NONE, 0xffffffff);
            }
        }
        break;

    default:
        break;
    }
}

bool notification_clk_req(void)
{
    bool ret = false;

    if (notification_db->sync_action_index != 0xff)
    {
        if (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
        {
            if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
            {
                ret = true;
                notification_db->sync_action_flag |= 0xf0;
                if (notification_db->sync_action_flag == 0xff)
                {
                    T_NOTIFICATION_MSG_SYNC_ACTION cmd;
                    T_BT_CLK_REF clk_ref;
                    uint32_t bb_clock_slot;
                    uint16_t bb_clock_us;
                    uint8_t  clk_idx;

                    clk_ref = bt_piconet_clk_get(BT_CLK_NONE, &clk_idx, &bb_clock_slot, &bb_clock_us);
                    cmd.init_clk = bb_clock_slot;
                    if (clk_ref == BT_CLK_NONE)
                    {
                        bb_clock_slot = 0xffffffff;
                    }
                    else
                    {
                        bb_clock_slot += (90 * 2 * 1000 / 625); // ms to bt clk
                        bb_clock_slot &= 0x0fffffff;
                    }
                    cmd.sync_clk = bb_clock_slot;
                    cmd.clk_ref = clk_ref;
                    cmd.tid = notification_db->sync_tid;

                    if (clk_ref == BT_CLK_CTRL)
                    {
                        uint8_t index;
                        bt_piconet_id_get(BT_CLK_CTRL, &index, &cmd.ref_role);
                    }

                    ret = remote_sync_msg_relay(notification_db->relay_handle,
                                                NOTIFICATION_REMOTE_SYNC_ACTION,
                                                &cmd,
                                                sizeof(T_NOTIFICATION_MSG_SYNC_ACTION),
                                                REMOTE_TIMER_HIGH_PRECISION,
                                                70,
                                                false);
                    if (ret == false)
                    {
                        notification_db->sync_action_index = 0xff;
                        notification_db->sync_action_flag = 0;
                    }
                }
            }
            else
            {
                ret = remote_async_msg_relay(notification_db->relay_handle,
                                             NOTIFICATION_REMOTE_CLK_REQ,
                                             &notification_db->sync_tid,
                                             1,
                                             false);
            }
        }
    }

    return ret;
}

void notification_play(void)
{
    T_NOTIFICATION_ELEM *elem;

    if (notification_db->state == NOTIFICATION_STATE_IDLE)
    {
        elem = os_queue_out(&notification_db->pending_queue);
        while (elem != NULL)
        {
            os_queue_in(&notification_db->play_queue, elem);
            elem = os_queue_out(&notification_db->pending_queue);
        }

        if (notification_db->lock == true)
        {
            notification_db->lock = false;

            if (remote_async_msg_relay(notification_db->relay_handle,
                                       NOTIFICATION_REMOTE_ACK,
                                       &notification_db->sync_tid,
                                       1,
                                       false) == true)
            {
                notification_set_state(NOTIFICATION_STATE_RELAY);
                sys_timer_start(notification_db->ack_timer);
            }

            return;
        }

        elem = os_queue_peek(&notification_db->play_queue, 0);
        if (elem == NULL)
        {
            AUDIO_PRINT_INFO0("notification_play: queue empty");
            return;
        }

        if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY &&
            elem->relay == true)
        {
            /*
                sec may have relay type tone after roleswap.
                relay those tones to primary.
            */
            T_NOTIFICATION_MSG_PUSH_RELAY msg;

            msg.index    = elem->index;
            msg.type     = elem->type;
            msg.language = elem->language;

            if (remote_async_msg_relay(notification_db->relay_handle,
                                       NOTIFICATION_REMOTE_PUSH_RELAY,
                                       &msg,
                                       sizeof(T_NOTIFICATION_MSG_PUSH_RELAY),
                                       false) == true)
            {
                notification_set_state(NOTIFICATION_STATE_RELAY);
                sys_timer_start(notification_db->ack_timer);

                elem = os_queue_out(&notification_db->play_queue);
                os_queue_in(&notification_db->idle_queue, elem);
                return;
            }
            else
            {
                elem->relay = false;
            }
        }

        if (elem->relay == true)
        {
            notification_db->sync_tid++;
            if (remote_async_msg_relay(notification_db->relay_handle,
                                       NOTIFICATION_REMOTE_POLL,
                                       &notification_db->sync_tid,
                                       1,
                                       false) == true)
            {
                sys_timer_start(notification_db->poll_timer);
                notification_set_state(NOTIFICATION_STATE_RELAY);
                return;
            }
        }

        AUDIO_PRINT_TRACE4("notification_play: state %u, type %u, index %u, relay %u",
                           notification_db->state, elem->type, elem->index, elem->relay);

        elem = os_queue_out(&notification_db->play_queue);
        notification_db->sync_action_index = 0xff;
        notification_db->sync_action_flag = 0;
        notification_path_start(elem);
        os_queue_in(&notification_db->idle_queue, elem);
    }
    else if (notification_db->state == NOTIFICATION_STATE_RELAY)
    {
        elem = os_queue_out(&notification_db->pending_queue);
        while (elem != NULL)
        {
            os_queue_in(&notification_db->play_queue, elem);
            elem = os_queue_out(&notification_db->pending_queue);
        }

        elem = os_queue_out(&notification_db->play_queue);
        if (elem == NULL)
        {
            notification_set_state(NOTIFICATION_STATE_IDLE);
            notification_play();
            return;
        }

        if (elem->relay == true)
        {
            T_NOTIFICATION_MSG_RELAY_START payload;

            payload.type     = elem->type;
            payload.index    = elem->index;
            payload.language = elem->language;
            payload.tid      = notification_db->sync_tid;

            if (remote_sync_msg_relay(notification_db->relay_handle,
                                      NOTIFICATION_REMOTE_START,
                                      &payload,
                                      sizeof(T_NOTIFICATION_MSG_RELAY_START),
                                      REMOTE_TIMER_HIGH_PRECISION,
                                      70,
                                      false))
            {
                notification_db->sync_play_index = payload.index;
                notification_set_relay_type(elem->type);
                os_queue_in(&notification_db->idle_queue, elem);
                return;
            }
        }

        AUDIO_PRINT_TRACE4("notification_play: state %u, type %u, index %u, relay %u",
                           notification_db->state, elem->type, elem->index, elem->relay);

        notification_path_start(elem);
        os_queue_in(&notification_db->idle_queue, elem);
    }
}

static bool audio_path_voice_prompt_cback(T_AUDIO_PATH_HANDLE handle,
                                          T_AUDIO_PATH_EVENT  event,
                                          uint32_t            param)
{
    AUDIO_PRINT_TRACE3("audio_path_voice_prompt_cback: handle %p, event 0x%02x, param 0x%08x",
                       handle, event, param);

    switch (event)
    {
    case AUDIO_PATH_EVT_CREATE:
        {
            if (notification_db->vp_path_handle == NULL)
            {
                notification_db->vp_path_handle = handle;
                (void)audio_path_start(handle);
            }
        }
        break;

    case AUDIO_PATH_EVT_RELEASE:
        {
            if (handle == notification_db->vp_path_handle)
            {
                notification_db->vp_path_handle = NULL;
                notification_db->state        = NOTIFICATION_STATE_IDLE;
                notification_db->vp_idx       = NOTIFICATION_INVALID_INDEX;
                notification_play();
            }
        }
        break;

    case AUDIO_PATH_EVT_RUNNING:
        {
            T_AUDIO_EVENT_PARAM param;

            param.voice_prompt_started.index = notification_db->vp_idx;
            notification_db->stream_state = NOTIFICATION_STREAM_STATE_STARTED;

            if (notification_db->vp_volume_muted == false)
            {
                audio_path_dac_level_set(handle, notification_db->vp_volume, notification_db->vp_volume_scale);
            }
            else
            {
                audio_path_dac_mute(handle);
            }

            notification_buffer_prequeue(notification_db->vp_buffer_type,
                                         notification_db->vp_flash_offset,
                                         notification_db->vp_flash_len);

            audio_mgr_event_post(AUDIO_EVENT_VOICE_PROMPT_STARTED, &param, sizeof(param));
        }
        break;

    case AUDIO_PATH_EVT_DATA_REQ:
        {
            notification_vp_data_req();
        }
        break;

    case AUDIO_PATH_EVT_IDLE:
        {
            T_AUDIO_EVENT_PARAM param;

            param.voice_prompt_stopped.index = notification_db->vp_idx;
            notification_db->stream_state = NOTIFICATION_STREAM_STATE_IDLE;
            notification_db->vp_seq_num   = 0;
            audio_mgr_event_post(AUDIO_EVENT_VOICE_PROMPT_STOPPED, &param, sizeof(param));

            audio_path_destroy(notification_db->vp_path_handle);
        }
        break;

    case AUDIO_PATH_EVT_DATA_EMPTY:
        {
            notification_stop(NOTIFICATION_TYPE_VP, NULL);
        }
        break;

    default:
        break;
    }

    return true;
}

static bool audio_path_tts_cback(T_AUDIO_PATH_HANDLE handle,
                                 T_AUDIO_PATH_EVENT  event,
                                 uint32_t            param)
{
    AUDIO_PRINT_TRACE3("audio_path_tts_cback: handle %p, event 0x%02x, param 0x%08x",
                       handle, event, param);

    switch (event)
    {
    case AUDIO_PATH_EVT_RUNNING:
        {
            T_AUDIO_EVENT_PARAM param;

            param.tts_started.handle = notification_db->tts_instance;
            notification_db->stream_state = NOTIFICATION_STREAM_STATE_STARTED;

            if (notification_db->tts_volume_muted == false)
            {
                audio_path_dac_level_set(handle, notification_db->tts_volume, notification_db->tts_volume_scale);
            }
            else
            {
                audio_path_dac_mute(handle);
            }

            audio_mgr_event_post(AUDIO_EVENT_TTS_STARTED, &param, sizeof(param));
        }
        break;

    case AUDIO_PATH_EVT_DATA_REQ:
        {
            notification_db->flow_go = true;
            notification_tts_data_req();
        }
        break;

    case AUDIO_PATH_EVT_IDLE:
        {
            T_AUDIO_EVENT_PARAM param;

            param.tts_stopped.handle = notification_db->tts_instance;
            notification_db->flow_go = true;
            notification_db->stream_state = NOTIFICATION_STREAM_STATE_IDLE;
            notification_db->state        = NOTIFICATION_STATE_IDLE;
            notification_db->tts_seq_num  = 0;
            notification_db->tts_instance = NULL;
            audio_mgr_event_post(AUDIO_EVENT_TTS_STOPPED, &param, sizeof(param));

            notification_play();
        }
        break;

    case AUDIO_PATH_EVT_DATA_EMPTY:
        {
            notification_stop(NOTIFICATION_TYPE_TTS, notification_db->tts_instance);
        }
        break;

    default:
        break;
    }

    return true;
}

static bool audio_path_ringtone_cback(T_AUDIO_PATH_HANDLE handle,
                                      T_AUDIO_PATH_EVENT  event,
                                      uint32_t            param)
{
    AUDIO_PRINT_TRACE3("audio_path_ringtone_cback: handle %p, event 0x%02x, param 0x%08x",
                       handle, event, param);

    switch (event)
    {
    case AUDIO_PATH_EVT_CREATE:
        {
            if (notification_db->tone_path_handle == NULL)
            {
                notification_db->tone_path_handle = handle;
                (void)audio_path_start(handle);
            }
        }
        break;

    case AUDIO_PATH_EVT_RELEASE:
        {
            if (handle == notification_db->tone_path_handle)
            {
                notification_db->tone_path_handle = NULL;
                notification_db->state        = NOTIFICATION_STATE_IDLE;
                notification_db->tone_idx     = NOTIFICATION_INVALID_INDEX;
                notification_play();
            }
        }
        break;

    case AUDIO_PATH_EVT_RUNNING:
        {
            T_AUDIO_EVENT_PARAM param;

            param.ringtone_started.index = notification_db->tone_idx;
            notification_db->stream_state = NOTIFICATION_STREAM_STATE_STARTED;

            if (notification_db->tone_volume_muted == false)
            {
                audio_path_dac_level_set(handle, notification_db->tone_volume, notification_db->tone_volume_scale);
            }
            else
            {
                audio_path_dac_mute(handle);
            }

            if (notification_clk_req() == false)
            {
                notification_start(BT_CLK_NONE, 0xffffffff);
            }
            else
            {
                if (notification_db->handshake_timer != NULL)
                {
                    sys_timer_delete(notification_db->handshake_timer);
                }

                notification_db->handshake_timer = sys_timer_create("notification_handshake",
                                                                    SYS_TIMER_TYPE_LOW_PRECISION,
                                                                    NOTIFICATION_TIMER_HANDSHAKE,
                                                                    NOTIFICATION_REMOTE_HANDSHAKE_TIMEOUT,
                                                                    false,
                                                                    notification_timeout_cback);
                if (notification_db->handshake_timer != NULL)
                {
                    sys_timer_start(notification_db->handshake_timer);
                }
            }

            audio_mgr_event_post(AUDIO_EVENT_RINGTONE_STARTED, &param, sizeof(param));
        }
        break;

    case AUDIO_PATH_EVT_DATA_REQ:
        break;

    case AUDIO_PATH_EVT_IDLE:
        {
            T_AUDIO_EVENT_PARAM param;

            param.ringtone_stopped.index = notification_db->tone_idx;
            notification_db->stream_state = NOTIFICATION_STREAM_STATE_IDLE;
            audio_mgr_event_post(AUDIO_EVENT_RINGTONE_STOPPED, &param, sizeof(param));

            audio_path_destroy(notification_db->tone_path_handle);
        }
        break;

    case AUDIO_PATH_EVT_DATA_EMPTY:
        {
            notification_stop(NOTIFICATION_TYPE_RINGTONE, NULL);
        }
        break;

    default:
        break;
    }

    return true;
}

uint8_t notification_volume_max_get(uint8_t type)
{
    uint8_t volume;

    switch (type)
    {
    case NOTIFICATION_TYPE_NONE:
        volume = 0;
        break;

    case NOTIFICATION_TYPE_RINGTONE:
        volume = notification_db->tone_volume_max;
        break;

    case NOTIFICATION_TYPE_VP:
        volume = notification_db->vp_volume_max;
        break;

    case NOTIFICATION_TYPE_TTS:
        volume = notification_db->tts_volume_max;
        break;

    default:
        volume = 0;
        break;
    }

    return volume;
}

bool notification_volume_max_set(uint8_t type,
                                 uint8_t volume)
{
    bool ret = true;

    switch (type)
    {
    case NOTIFICATION_TYPE_NONE:
        ret = false;
        break;

    case NOTIFICATION_TYPE_RINGTONE:
        notification_db->tone_volume_max = volume;
        break;

    case NOTIFICATION_TYPE_VP:
        notification_db->vp_volume_max = volume;
        break;

    case NOTIFICATION_TYPE_TTS:
        notification_db->tts_volume_max = volume;
        break;
    }

    return ret;
}

uint8_t notification_volume_min_get(uint8_t type)
{
    uint8_t volume;

    switch (type)
    {
    case NOTIFICATION_TYPE_NONE:
        volume = 0;
        break;

    case NOTIFICATION_TYPE_RINGTONE:
        volume = notification_db->tone_volume_min;
        break;

    case NOTIFICATION_TYPE_VP:
        volume = notification_db->vp_volume_min;
        break;

    case NOTIFICATION_TYPE_TTS:
        volume = notification_db->tts_volume_min;
        break;

    default:
        volume = 0;
        break;
    }

    return volume;
}

bool notification_volume_min_set(uint8_t type,
                                 uint8_t volume)
{
    bool ret = true;

    switch (type)
    {
    case NOTIFICATION_TYPE_NONE:
        ret = false;
        break;

    case NOTIFICATION_TYPE_RINGTONE:
        notification_db->tone_volume_min = volume;
        break;

    case NOTIFICATION_TYPE_VP:
        notification_db->vp_volume_min = volume;
        break;

    case NOTIFICATION_TYPE_TTS:
        notification_db->tts_volume_min = volume;
        break;
    }

    return ret;
}

uint8_t notification_volume_get(uint8_t type)
{
    uint8_t volume;

    switch (type)
    {
    case NOTIFICATION_TYPE_NONE:
        volume = 0;
        break;

    case NOTIFICATION_TYPE_RINGTONE:
        volume = notification_db->tone_volume;
        break;

    case NOTIFICATION_TYPE_VP:
        volume = notification_db->vp_volume;
        break;

    case NOTIFICATION_TYPE_TTS:
        volume = notification_db->tts_volume;
        break;

    default:
        volume = 0;
        break;
    }

    return volume;
}

bool notification_volume_set(uint8_t type,
                             uint8_t volume)
{
    bool ret = true;

    switch (type)
    {
    case NOTIFICATION_TYPE_NONE:
        ret = false;
        break;

    case NOTIFICATION_TYPE_RINGTONE:
        notification_db->tone_volume = volume;

        if (notification_db->tone_volume_muted == false)
        {
            audio_path_dac_level_set(notification_db->tone_path_handle,
                                     notification_db->tone_volume,
                                     notification_db->tone_volume_scale);
        }
        break;

    case NOTIFICATION_TYPE_VP:
        notification_db->vp_volume = volume;

        if (notification_db->vp_volume_muted == false)
        {
            audio_path_dac_level_set(notification_db->vp_path_handle,
                                     notification_db->vp_volume,
                                     notification_db->vp_volume_scale);
        }
        break;

    case NOTIFICATION_TYPE_TTS:
        notification_db->tts_volume = volume;

        if (notification_db->tts_volume_muted == false)
        {
            if (notification_db->tts_instance != NULL)
            {
                audio_path_dac_level_set(notification_db->tts_instance->handle,
                                         notification_db->tts_volume,
                                         notification_db->tts_volume_scale);
            }
        }
        break;
    }

    return ret;
}

bool notification_volume_mute(uint8_t type)
{
    bool ret = true;

    switch (type)
    {
    case NOTIFICATION_TYPE_NONE:
        ret = false;
        break;

    case NOTIFICATION_TYPE_RINGTONE:
        if (notification_db->tone_volume_muted == false)
        {
            notification_db->tone_volume_muted  = true;
            audio_path_dac_mute(notification_db->tone_path_handle);
        }
        else
        {
            ret = false;
        }
        break;

    case NOTIFICATION_TYPE_VP:
        if (notification_db->vp_volume_muted == false)
        {
            notification_db->vp_volume_muted = true;
            audio_path_dac_mute(notification_db->vp_path_handle);
        }
        else
        {
            ret = false;
        }
        break;

    case NOTIFICATION_TYPE_TTS:
        if (notification_db->tts_volume_muted == false)
        {
            notification_db->tts_volume_muted = true;

            if (notification_db->tts_instance != NULL)
            {
                audio_path_dac_mute(notification_db->tts_instance->handle);
            }
        }
        else
        {
            ret = false;
        }
        break;
    }

    return ret;
}

bool notification_volume_unmute(uint8_t type)
{
    bool ret = true;

    switch (type)
    {
    case NOTIFICATION_TYPE_NONE:
        ret = false;
        break;

    case NOTIFICATION_TYPE_RINGTONE:
        if (notification_db->tone_volume_muted == true)
        {
            notification_db->tone_volume_muted = false;
            audio_path_dac_level_set(notification_db->tone_path_handle,
                                     notification_db->tone_volume,
                                     notification_db->tone_volume_scale);
        }
        else
        {
            ret = false;
        }
        break;

    case NOTIFICATION_TYPE_VP:
        if (notification_db->vp_volume_muted == true)
        {
            notification_db->vp_volume_muted = false;
            audio_path_dac_level_set(notification_db->vp_path_handle,
                                     notification_db->vp_volume,
                                     notification_db->vp_volume_scale);
        }
        else
        {
            ret = false;
        }
        break;

    case NOTIFICATION_TYPE_TTS:
        if (notification_db->tts_volume_muted == true)
        {
            notification_db->tts_volume_muted = false;

            if (notification_db->tts_instance != NULL)
            {
                audio_path_dac_level_set(notification_db->tts_instance->handle,
                                         notification_db->tts_volume,
                                         notification_db->tts_volume_scale);
            }
        }
        else
        {
            ret = false;
        }
        break;
    }

    return ret;
}

float notification_volume_balance_get(uint8_t type)
{
    float scale = 0.0f;

    switch (type)
    {
    case NOTIFICATION_TYPE_NONE:
        break;

    case NOTIFICATION_TYPE_RINGTONE:
        scale = notification_db->tone_volume_scale;
        break;

    case NOTIFICATION_TYPE_VP:
        scale = notification_db->vp_volume_scale;
        break;

    case NOTIFICATION_TYPE_TTS:
        scale = notification_db->tts_volume_scale;
        break;
    }

    return scale;
}

bool notification_volume_balance_set(uint8_t type,
                                     float   scale)
{
    bool ret = false;

    switch (type)
    {
    case NOTIFICATION_TYPE_NONE:
        break;

    case NOTIFICATION_TYPE_RINGTONE:
        if (scale >= -1.0f && scale <= 1.0f)
        {
            notification_db->tone_volume_scale = scale;
            audio_path_dac_level_set(notification_db->tone_path_handle,
                                     notification_db->tone_volume,
                                     notification_db->tone_volume_scale);
            ret = true;
        }
        break;

    case NOTIFICATION_TYPE_VP:
        if (scale >= -1.0f && scale <= 1.0f)
        {
            notification_db->vp_volume_scale = scale;
            audio_path_dac_level_set(notification_db->vp_path_handle,
                                     notification_db->vp_volume,
                                     notification_db->vp_volume_scale);
            ret = true;
        }
        break;

    case NOTIFICATION_TYPE_TTS:
        if (scale >= -1.0f && scale <= 1.0f)
        {
            notification_db->tts_volume_scale = scale;

            if (notification_db->tts_instance != NULL)
            {
                audio_path_dac_level_set(notification_db->tts_instance->handle,
                                         notification_db->tts_volume,
                                         notification_db->tts_volume_scale);
            }

            ret = true;
        }
        break;
    }

    return ret;
}

bool notification_sync_play(void *p_data)
{
    if (notification_db->state == NOTIFICATION_STATE_RELAY)
    {
        T_NOTIFICATION_MSG_RELAY_START *p_buf;
        T_NOTIFICATION_ELEM             elem;

        p_buf = (T_NOTIFICATION_MSG_RELAY_START *)p_data;

        elem.next     = NULL;
        elem.index    = p_buf->index;
        elem.type     = p_buf->type;
        elem.language = p_buf->language;
        elem.relay    = false;
        elem.instance = notification_db->tts_instance;

        if (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
        {
            if (elem.type == NOTIFICATION_TYPE_VP)
            {
                notification_db->sync_action_type = NOTIFICATION_TYPE_VP;
                notification_db->sync_action_index = elem.index;
            }
            else if (elem.type == NOTIFICATION_TYPE_TTS)
            {
                notification_db->sync_action_type = NOTIFICATION_TYPE_TTS;
                notification_db->sync_action_index = elem.index;
            }
            else
            {
                notification_db->sync_action_type = NOTIFICATION_TYPE_RINGTONE;
                notification_db->sync_action_index = elem.index;
            }
        }
        else
        {
            notification_db->sync_action_index = 0xff;
        }

        AUDIO_PRINT_TRACE4("notification_sync_play: state %u, type %u, index %u, relay %u",
                           notification_db->state, elem.type, elem.index, elem.relay);

        notification_path_start(&elem);
        return true;
    }

    return false;
}

bool notification_flush(uint8_t type,
                        bool    relay)
{
    T_NOTIFICATION_ELEM *curr_elem;
    T_NOTIFICATION_ELEM *next_elem;

    AUDIO_PRINT_TRACE2("notification_flush: type %u, relay %u",
                       type, relay);

    curr_elem = os_queue_peek(&notification_db->play_queue, 0);
    while (curr_elem != NULL)
    {
        if (curr_elem->type == type)
        {
            next_elem = curr_elem->next;
            os_queue_delete(&notification_db->play_queue, curr_elem);
            os_queue_in(&notification_db->idle_queue, curr_elem);
            curr_elem = next_elem;
        }
        else
        {
            curr_elem = curr_elem->next;
        }
    }

    curr_elem = os_queue_peek(&notification_db->pending_queue, 0);
    while (curr_elem != NULL)
    {
        if (curr_elem->type == type)
        {
            next_elem = curr_elem->next;
            os_queue_delete(&notification_db->pending_queue, curr_elem);
            os_queue_in(&notification_db->idle_queue, curr_elem);
            curr_elem = next_elem;
        }
        else
        {
            curr_elem = curr_elem->next;
        }
    }

    if (relay)
    {
        T_NOTIFICATION_MSG_FLUSH payload;

        payload.type  = type;
        payload.relay = relay;

        remote_async_msg_relay(notification_db->relay_handle,
                               NOTIFICATION_REMOTE_FLUSH,
                               &payload,
                               sizeof(T_NOTIFICATION_MSG_FLUSH),
                               false);
    }

    return true;
}

bool notification_cancel(uint8_t type,
                         uint8_t index,
                         bool    relay)
{
    T_NOTIFICATION_ELEM *curr_elem;
    T_NOTIFICATION_ELEM *next_elem;

    AUDIO_PRINT_TRACE3("notification_cancel: type %u, index %u, relay %u",
                       type, index, relay);

    curr_elem = os_queue_peek(&notification_db->play_queue, 0);
    while (curr_elem != NULL)
    {
        if (curr_elem->type == type && curr_elem->index == index)
        {
            next_elem = curr_elem->next;
            os_queue_delete(&notification_db->play_queue, curr_elem);
            os_queue_in(&notification_db->idle_queue, curr_elem);
            curr_elem = next_elem;
        }
        else
        {
            curr_elem = curr_elem->next;
        }
    }

    curr_elem = os_queue_peek(&notification_db->pending_queue, 0);
    while (curr_elem != NULL)
    {
        if (curr_elem->type == type && curr_elem->index == index)
        {
            next_elem = curr_elem->next;
            os_queue_delete(&notification_db->pending_queue, curr_elem);
            os_queue_in(&notification_db->idle_queue, curr_elem);
            curr_elem = next_elem;
        }
        else
        {
            curr_elem = curr_elem->next;
        }
    }

    if (type == NOTIFICATION_TYPE_RINGTONE)
    {
        if (notification_db->state == NOTIFICATION_STATE_RINGTONE)
        {
            if (notification_db->tone_idx == index)
            {
                notification_stop(NOTIFICATION_TYPE_RINGTONE, NULL);
            }
        }
        else if (notification_db->state == NOTIFICATION_STATE_RELAY)
        {
            if (notification_db->relay_type == NOTIFICATION_TYPE_RINGTONE)
            {
                if (notification_db->sync_play_index == index)
                {
                    notification_stop(NOTIFICATION_TYPE_RINGTONE, NULL);
                }
            }
        }
    }
    else if (type == NOTIFICATION_TYPE_VP)
    {
        if (notification_db->state == NOTIFICATION_STATE_VP)
        {
            if (notification_db->vp_idx == index)
            {
                notification_stop(NOTIFICATION_TYPE_VP, NULL);
            }
        }
        else if (notification_db->state == NOTIFICATION_STATE_RELAY)
        {
            if (notification_db->relay_type == NOTIFICATION_TYPE_VP)
            {
                if (notification_db->sync_play_index == index)
                {
                    notification_stop(NOTIFICATION_TYPE_VP, NULL);
                }
            }
        }
    }

    if (relay)
    {
        T_NOTIFICATION_MSG_CANCEL payload;

        payload.index = index;
        payload.type  = type;
        payload.relay = relay;

        remote_async_msg_relay(notification_db->relay_handle,
                               NOTIFICATION_REMOTE_CANCEL,
                               &payload,
                               sizeof(T_NOTIFICATION_MSG_CANCEL),
                               false);
    }

    return true;
}

bool notification_push(uint8_t      type,
                       uint8_t      index,
                       uint8_t      language,
                       T_TTS_HANDLE handle,
                       bool         relay)
{
    T_NOTIFICATION_ELEM *elem;

    elem = os_queue_out(&notification_db->idle_queue);
    if (elem != NULL)
    {
        elem->index    = index;
        elem->type     = type;
        elem->relay    = relay;
        elem->language = language;
        elem->instance = handle;

        os_queue_in(&notification_db->pending_queue, elem);

        AUDIO_PRINT_TRACE3("notification_push: state %u, pending_queue num %u, play_queue num %u",
                           notification_db->state, notification_db->pending_queue.count, notification_db->play_queue.count);

        if (notification_db->state == NOTIFICATION_STATE_IDLE)
        {
            notification_play();
        }
        else
        {
            elem = os_queue_out(&notification_db->pending_queue);
            os_queue_in(&notification_db->play_queue, elem);
        }

        return true;
    }

    return false;
}

bool notification_stop(uint8_t      type,
                       T_TTS_HANDLE handle)
{
    bool ret = false;

    if (type == NOTIFICATION_TYPE_RINGTONE)
    {
        if (notification_db->state == NOTIFICATION_STATE_RINGTONE)
        {
            notification_db->sync_action_type = NOTIFICATION_TYPE_NONE;
            notification_db->sync_action_index = 0xff;
            notification_db->sync_action_flag = 0;
            ret = audio_path_stop(notification_db->tone_path_handle);
        }
        else if (notification_db->state == NOTIFICATION_STATE_RELAY)
        {
            if (notification_db->relay_type == NOTIFICATION_TYPE_RINGTONE)
            {
                uint8_t tid = notification_db->sync_tid;
                notification_set_state(NOTIFICATION_STATE_IDLE);
                notification_set_relay_type(NOTIFICATION_TYPE_NONE);
                notification_db->sync_play_index = 0xff;
                notification_db->sync_tid++;

                if (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
                {
                    if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        ret = remote_async_msg_relay(notification_db->relay_handle,
                                                     NOTIFICATION_REMOTE_POLL_ABT,
                                                     &tid,
                                                     1,
                                                     false);
                        if (notification_get_pending_elem_num() != 0)
                        {
                            notification_play();
                        }
                    }
                }

                ret = true;
            }
        }
    }
    else if (type == NOTIFICATION_TYPE_VP)
    {
        if (notification_db->state == NOTIFICATION_STATE_VP)
        {
            notification_db->sync_action_type = NOTIFICATION_TYPE_NONE;
            notification_db->sync_action_index = 0xff;
            notification_db->sync_action_flag = 0;
            ret = audio_path_stop(notification_db->vp_path_handle);
        }
        else if (notification_db->state == NOTIFICATION_STATE_RELAY)
        {
            if (notification_db->relay_type == NOTIFICATION_TYPE_VP)
            {
                uint8_t tid = notification_db->sync_tid;
                notification_set_state(NOTIFICATION_STATE_IDLE);
                notification_set_relay_type(NOTIFICATION_TYPE_NONE);
                notification_db->sync_play_index = 0xff;
                notification_db->sync_tid++;
                if (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
                {
                    if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        ret = remote_async_msg_relay(notification_db->relay_handle,
                                                     NOTIFICATION_REMOTE_POLL_ABT,
                                                     &tid,
                                                     1,
                                                     false);
                        if (notification_get_pending_elem_num() != 0)
                        {
                            notification_play();
                        }
                    }
                }
                ret = true;
            }
        }
    }
    else if (type == NOTIFICATION_TYPE_TTS)
    {
        T_NOTIFICATION_ELEM *elem;

        elem = notification_tts_find(handle);
        if (elem != NULL)
        {
            if (os_queue_delete(&notification_db->play_queue, elem))
            {
                T_AUDIO_EVENT_PARAM param;

                param.tts_stopped.handle = handle;
                os_queue_in(&notification_db->idle_queue, elem);

                ret = audio_mgr_event_post(AUDIO_EVENT_TTS_STOPPED, &param, sizeof(param));
            }
        }
        else
        {
            if (notification_db->state == NOTIFICATION_STATE_TTS)
            {
                if (notification_db->tts_instance == handle)
                {
                    ret = audio_path_stop(notification_db->tts_instance->handle);
                }
            }
        }
    }

    AUDIO_PRINT_TRACE5("notification_stop: state %d, type %u, handle %p, tid %d, ret %d",
                       notification_db->state, type, handle, notification_db->sync_tid, ret);

    return ret;
}

bool notification_write(uint8_t       type,
                        T_TTS_HANDLE  handle,
                        uint8_t      *buf,
                        uint16_t      len)
{
    if (type == NOTIFICATION_TYPE_TTS)
    {
        if (notification_db->tts_instance == handle)
        {
            return notification_buffer_push(notification_db->tts_buffer_type,
                                            (uint32_t)buf,
                                            len);
        }
    }

    return false;
}

void notification_relay_cback(T_NOTIFICATION_REMOTE_MSG  event,
                              T_REMOTE_RELAY_STATUS      status,
                              void                      *buf,
                              uint16_t                   len)
{
    AUDIO_PRINT_TRACE4("notification_relay_cback: event 0x%04x, status %u, flag 0x%02x, index 0x%02x",
                       event, status, notification_db->sync_action_flag, notification_db->sync_action_index);

    switch (event)
    {
    case NOTIFICATION_REMOTE_ACK:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t tid = *(uint8_t *)buf;
                if (notification_db->sync_tid == tid)
                {
                    sys_timer_stop(notification_db->poll_timer);
                    if (notification_get_pending_elem_num() != 0)
                    {
                        if (notification_get_state() != NOTIFICATION_STATE_RELAY &&
                            notification_get_state() != NOTIFICATION_STATE_IDLE)
                        {
                            notification_set_state(NOTIFICATION_STATE_IDLE);
                            remote_async_msg_relay(notification_db->relay_handle,
                                                   NOTIFICATION_REMOTE_ACK_ABT,
                                                   NULL,
                                                   0,
                                                   false);
                        }
                        else
                        {
                            notification_play();
                        }
                    }
                    else
                    {
                        notification_set_state(NOTIFICATION_STATE_IDLE);
                        notification_set_relay_type(NOTIFICATION_TYPE_NONE);
                        notification_db->sync_play_index = 0xff;
                        remote_async_msg_relay(notification_db->relay_handle,
                                               NOTIFICATION_REMOTE_ACK_ABT,
                                               NULL,
                                               0,
                                               false);
                    }
                }
                else
                {
                    AUDIO_PRINT_WARN2("Notification error tid local %d remote %d",
                                      notification_db->sync_tid, tid);
                }
            }
            else if (status == REMOTE_RELAY_STATUS_ASYNC_SENT_FAILED)
            {
                sys_timer_stop(notification_db->ack_timer);
                notification_set_state(NOTIFICATION_STATE_IDLE);
                notification_set_relay_type(NOTIFICATION_TYPE_NONE);
                notification_db->sync_play_index = 0xff;
                notification_play();
            }
        }
        break;

    case NOTIFICATION_REMOTE_POLL:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                notification_db->sync_tid = *(uint8_t *)buf;
                if (notification_get_state() == NOTIFICATION_STATE_RELAY)
                {
                    notification_unlock();
                    if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
                    {
                        sys_timer_stop(notification_db->poll_timer);
                        if (remote_async_msg_relay(notification_db->relay_handle,
                                                   NOTIFICATION_REMOTE_ACK,
                                                   &notification_db->sync_tid,
                                                   1,
                                                   false))
                        {
                            sys_timer_start(notification_db->ack_timer);
                        }
                    }
                }
                else if (notification_get_state() == NOTIFICATION_STATE_IDLE)
                {
                    notification_unlock();
                    if (remote_async_msg_relay(notification_db->relay_handle,
                                               NOTIFICATION_REMOTE_ACK,
                                               &notification_db->sync_tid,
                                               1,
                                               false))
                    {
                        notification_set_state(NOTIFICATION_STATE_RELAY);
                        sys_timer_start(notification_db->ack_timer);
                    }
                }
                else
                {
                    notification_lock();
                }
            }
            else if (status == REMOTE_RELAY_STATUS_ASYNC_SENT_FAILED)
            {
                sys_timer_stop(notification_db->poll_timer);
                notification_set_state(NOTIFICATION_STATE_IDLE);
                notification_set_relay_type(NOTIFICATION_TYPE_NONE);
                notification_db->sync_play_index = 0xff;
                notification_play();
            }
        }
        break;

    case NOTIFICATION_REMOTE_FLUSH:
        {
            T_NOTIFICATION_MSG_FLUSH *payload = (T_NOTIFICATION_MSG_FLUSH *)buf;

            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                notification_flush(payload->type, false);
            }
        }
        break;

    case NOTIFICATION_REMOTE_CANCEL:
        {
            T_NOTIFICATION_MSG_CANCEL *payload = (T_NOTIFICATION_MSG_CANCEL *)buf;

            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                notification_cancel(payload->type, payload->index, false);
            }
        }
        break;

    case NOTIFICATION_REMOTE_POLL_ABT:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t state = notification_get_state();
                notification_unlock();
                if (state != NOTIFICATION_STATE_IDLE)
                {
                    if (state == NOTIFICATION_STATE_RELAY)
                    {
                        notification_set_state(NOTIFICATION_STATE_IDLE);
                        notification_set_relay_type(NOTIFICATION_TYPE_NONE);
                        notification_db->sync_play_index = 0xff;
                        if (notification_get_pending_elem_num() != 0)
                        {
                            notification_play();
                        }
                    }
                    else if (state == NOTIFICATION_STATE_VP)
                    {
                        uint8_t tid = *(uint8_t *)buf;
                        if (notification_db->sync_tid == tid)
                        {
                            notification_db->sync_action_type = NOTIFICATION_TYPE_NONE;
                            notification_db->sync_action_index = 0xff;
                            notification_db->sync_action_flag = 0;
                            audio_path_stop(notification_db->vp_path_handle);
                        }
                    }
                    else if (state == NOTIFICATION_STATE_RINGTONE)
                    {
                        uint8_t tid = *(uint8_t *)buf;
                        if (notification_db->sync_tid == tid)
                        {
                            notification_db->sync_action_type = NOTIFICATION_TYPE_NONE;
                            notification_db->sync_action_index = 0xff;
                            notification_db->sync_action_flag = 0;
                            audio_path_stop(notification_db->tone_path_handle);
                        }
                    }
                }
            }
        }
        break;

    case NOTIFICATION_REMOTE_ACK_ABT:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                sys_timer_stop(notification_db->ack_timer);
                if (notification_get_state() != NOTIFICATION_STATE_IDLE)
                {
                    if (notification_get_state() == NOTIFICATION_STATE_RELAY)
                    {
                        notification_set_state(NOTIFICATION_STATE_IDLE);
                        notification_set_relay_type(NOTIFICATION_TYPE_NONE);
                        notification_db->sync_play_index = 0xff;
                    }
                    if (notification_get_pending_elem_num() != 0)
                    {
                        notification_play();
                    }
                }
            }
        }
        break;

    case NOTIFICATION_REMOTE_PUSH_RELAY:
        {
            T_NOTIFICATION_MSG_PUSH_RELAY *p_cmd = (T_NOTIFICATION_MSG_PUSH_RELAY *)buf;

            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                notification_push(p_cmd->type, p_cmd->index, p_cmd->language, NULL, true);
            }
        }
        break;

    case NOTIFICATION_REMOTE_START:
        {
            T_NOTIFICATION_MSG_RELAY_START *payload = (T_NOTIFICATION_MSG_RELAY_START *)buf;

            if (status == REMOTE_RELAY_STATUS_SYNC_RCVD)
            {
                notification_db->sync_play_index = payload->index;
                notification_set_relay_type(payload->type);
            }
            else if (status == REMOTE_RELAY_STATUS_SYNC_SENT_OUT)
            {
                notification_db->sync_action_index = payload->index;
            }
            else if (status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                     status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                     status == REMOTE_RELAY_STATUS_SYNC_TOUT)
            {
                sys_timer_stop(notification_db->poll_timer);
                sys_timer_stop(notification_db->ack_timer);

                if (payload->tid == notification_db->sync_tid)
                {
                    notification_sync_play(buf);
                }
                else
                {
                    AUDIO_PRINT_WARN2("Notification error tid local %d remote %d",
                                      notification_db->sync_tid, payload->tid);
                }
            }
        }
        break;

    case NOTIFICATION_REMOTE_CLK_REQ:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t tid = *(uint8_t *)buf;

                if (tid != notification_db->sync_tid)
                {
                    AUDIO_PRINT_WARN2("Notification error tid local %d remote %d",
                                      notification_db->sync_tid, tid);
                    break;
                }
                if (notification_db->sync_action_index != 0xff)
                {
                    if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY &&
                        remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
                    {
                        notification_db->sync_action_flag |= 0x0f;
                        if (notification_db->sync_action_flag == 0xff)
                        {
                            T_NOTIFICATION_MSG_SYNC_ACTION cmd;
                            T_BT_CLK_REF clk_ref;
                            uint32_t bb_clock_slot;
                            uint16_t bb_clock_us;
                            uint8_t  clk_idx;

                            clk_ref = bt_piconet_clk_get(BT_CLK_NONE, &clk_idx, &bb_clock_slot, &bb_clock_us);
                            cmd.init_clk = bb_clock_slot;
                            if (clk_ref == BT_CLK_NONE)
                            {
                                bb_clock_slot = 0xffffffff;
                            }
                            else
                            {
                                bb_clock_slot += (90 * 2 * 1000 / 625); // ms to bt clk
                                bb_clock_slot &= 0x0fffffff;
                            }
                            cmd.sync_clk = bb_clock_slot;
                            cmd.clk_ref = clk_ref;
                            cmd.tid = notification_db->sync_tid;

                            if (clk_ref == BT_CLK_CTRL)
                            {
                                uint8_t index;
                                bt_piconet_id_get(BT_CLK_CTRL, &index, &cmd.ref_role);
                            }

                            if (remote_sync_msg_relay(notification_db->relay_handle,
                                                      NOTIFICATION_REMOTE_SYNC_ACTION,
                                                      &cmd,
                                                      sizeof(T_NOTIFICATION_MSG_SYNC_ACTION),
                                                      REMOTE_TIMER_HIGH_PRECISION,
                                                      70,
                                                      false) == false)

                            {
                                notification_start(clk_idx, 0xffffffff);
                            }
                        }
                    }
                }
            }
        }
        break;

    case NOTIFICATION_REMOTE_SYNC_ACTION:
        {
            if (notification_db->sync_action_index != 0xff)
            {
                T_NOTIFICATION_MSG_SYNC_ACTION *p_msg = (T_NOTIFICATION_MSG_SYNC_ACTION *)buf;
                if (p_msg->tid != notification_db->sync_tid)
                {
                    AUDIO_PRINT_WARN2("Notification error tid local %d remote %d",
                                      notification_db->sync_tid, p_msg->tid);
                    break;
                }
                if (status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED)
                {
                    T_BT_CLK_REF clk_ref;
                    uint32_t bb_clock_slot;
                    uint32_t bb_clock_rcv;
                    uint16_t bb_clock_us;
                    uint8_t  clk_idx;

                    bb_clock_rcv = p_msg->sync_clk;
                    clk_ref = bt_piconet_clk_get(BT_CLK_CTRL, &clk_idx, &bb_clock_slot, &bb_clock_us);

                    if (clk_ref == BT_CLK_CTRL &&
                        bt_clk_compare(bb_clock_rcv, bb_clock_slot + SYNC_PLAY_NOTI_PROTECT_MARGIN * 2 * 1000 / 625) &&
                        bt_clk_compare(bb_clock_slot, p_msg->init_clk))
                    {
                        uint8_t role;
                        uint8_t index;

                        bt_piconet_id_get(BT_CLK_CTRL, &index, &role);
                        if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
                        {
                            if (p_msg->ref_role == role)
                            {
                                AUDIO_PRINT_WARN0("Sec b2b link role has been changed while playing notification");
                                notification_start(clk_idx, 0xffffffff);
                            }
                            else
                            {
                                notification_start(clk_idx, bb_clock_rcv);
                            }
                        }
                        else
                        {
                            if (p_msg->ref_role != role)
                            {
                                AUDIO_PRINT_WARN0("Pri b2b link role has been changed while playing notification");
                                notification_start(clk_idx, 0xffffffff);
                            }
                            else
                            {
                                notification_start(clk_idx, bb_clock_rcv);
                            }
                        }
                    }
                    else
                    {
                        notification_start(clk_idx, 0xffffffff);
                    }
                }
                else if (status == REMOTE_RELAY_STATUS_SYNC_EXPIRED || status == REMOTE_RELAY_STATUS_SYNC_TOUT)
                {
                    T_BT_CLK_REF clk_ref;
                    uint32_t bb_clock_slot;
                    uint32_t bb_clock_rcv;
                    uint16_t bb_clock_us;
                    uint8_t  clk_idx;

                    bb_clock_rcv = p_msg->sync_clk;
                    clk_ref = bt_piconet_clk_get((T_BT_CLK_REF)(p_msg->clk_ref), &clk_idx, &bb_clock_slot,
                                                 &bb_clock_us);

                    if (clk_ref != BT_CLK_NONE &&
                        bt_clk_compare(bb_clock_rcv, bb_clock_slot + SYNC_PLAY_NOTI_PROTECT_MARGIN * 2 * 1000 / 625))
                    {
                        if (clk_ref == BT_CLK_CTRL)
                        {
                            uint8_t role;
                            uint8_t index;

                            bt_piconet_id_get(BT_CLK_CTRL, &index, &role);
                            if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
                            {
                                if (p_msg->ref_role == role)
                                {
                                    AUDIO_PRINT_WARN0("Sec b2b link role has been changed while playing notification");
                                    notification_start(clk_idx, 0xffffffff);
                                }
                                else
                                {
                                    notification_start(clk_idx, bb_clock_rcv);
                                }
                            }
                            else
                            {
                                if (p_msg->ref_role != role)
                                {
                                    AUDIO_PRINT_WARN0("Pri b2b link role has been changed while playing notification");
                                    notification_start(clk_idx, 0xffffffff);
                                }
                                else
                                {
                                    notification_start(clk_idx, bb_clock_rcv);
                                }
                            }
                        }
                        else
                        {
                            notification_start(clk_idx, bb_clock_rcv);
                        }
                    }
                    else
                    {
                        clk_ref = bt_piconet_clk_get(BT_CLK_CTRL, &clk_idx, &bb_clock_slot, &bb_clock_us);

                        if (clk_ref == BT_CLK_CTRL &&
                            bt_clk_compare(bb_clock_rcv, bb_clock_slot + SYNC_PLAY_NOTI_PROTECT_MARGIN * 2 * 1000 / 625) &&
                            bt_clk_compare(bb_clock_slot, p_msg->init_clk))
                        {
                            AUDIO_PRINT_WARN4("VP_clk_err_acceptable: clk ref %u sync clk %u local %u + margin %u", clk_ref,
                                              bb_clock_rcv, bb_clock_slot, SYNC_PLAY_NOTI_PROTECT_MARGIN * 2 * 1000 / 625);
                            notification_start(clk_ref, bb_clock_rcv);
                        }
                        else
                        {
                            notification_start(clk_idx, 0xffffffff);
                        }
                    }
                }
            }
        }
        break;

    default:
        break;
    }
}

bool notification_init(uint16_t pool_size)
{
    uint8_t i;
    int32_t ret = 0;

    notification_db = calloc(1, sizeof(T_NOTIFICATION_DB));
    if (notification_db == NULL)
    {
        ret = 1;
        goto fail_alloc_db;
    }

    if (pool_size != 0)
    {
        if (notification_buffer_init(pool_size) == false)
        {
            ret = 2;
            goto fail_init_vp_buffer;
        }
    }

    notification_db->relay_handle = remote_relay_register(notification_relay_cback);
    if (notification_db->relay_handle == NULL)
    {
        ret = 3;
        goto fail_register_remote_relay;
    }

    notification_db->poll_timer = sys_timer_create("notification poll",
                                                   SYS_TIMER_TYPE_LOW_PRECISION,
                                                   NOTIFICATION_TIMER_POLL,
                                                   NOTIFICATION_REMOTE_POLL_TIMEOUT,
                                                   false,
                                                   notification_timeout_cback);
    if (notification_db->poll_timer == NULL)
    {
        ret = 4;
        goto fail_init_poll_timer;
    }

    notification_db->ack_timer = sys_timer_create("notification ack",
                                                  SYS_TIMER_TYPE_LOW_PRECISION,
                                                  NOTIFICATION_TIMER_ACK,
                                                  NOTIFICATION_REMOTE_ACK_TIMEOUT,
                                                  false,
                                                  notification_timeout_cback);
    if (notification_db->ack_timer == NULL)
    {
        ret = 5;
        goto fail_init_ack_timer;
    }

    os_queue_init(&notification_db->idle_queue);
    os_queue_init(&notification_db->pending_queue);
    os_queue_init(&notification_db->play_queue);

    for (i = 0; i < NOTIFICATION_QUEUE_NUM; ++i)
    {
        os_queue_in(&notification_db->idle_queue, &notification_db->elem[i]);
    }

    notification_db->tone_mode          = RINGTONE_MODE_AUDIBLE;
    notification_db->vp_mode            = VOICE_PROMPT_MODE_AUDIBLE;

    notification_db->vp_buffer_type     = NOTIFICATION_BUFFER_TYPE_STREAM;
    notification_db->tts_buffer_type    = NOTIFICATION_BUFFER_TYPE_FRAME;

    notification_db->tone_idx           = 0xFF;
    notification_db->vp_idx             = 0xFF;
    notification_db->tts_instance       = NULL;

    notification_db->tone_volume_max    = NOTIFICATION_VOLUME_MAX;
    notification_db->tone_volume_min    = NOTIFICATION_VOLUME_MIN;
    notification_db->tone_volume_muted  = false;
    notification_db->tone_volume        = NOTIFICATION_VOLUME_DEFAULT;
    notification_db->tone_volume_scale  = 0.0f;

    notification_db->vp_volume_max      = NOTIFICATION_VOLUME_MAX;
    notification_db->vp_volume_min      = NOTIFICATION_VOLUME_MIN;
    notification_db->vp_volume_muted    = false;
    notification_db->vp_volume          = NOTIFICATION_VOLUME_DEFAULT;
    notification_db->vp_volume_scale    = 0.0f;
    notification_db->vp_seq_num         = 0;

    notification_db->tts_volume_max     = NOTIFICATION_VOLUME_MAX;
    notification_db->tts_volume_min     = NOTIFICATION_VOLUME_MIN;
    notification_db->tts_volume_muted   = false;
    notification_db->tts_volume         = NOTIFICATION_VOLUME_DEFAULT;
    notification_db->tts_volume_scale   = 0.0f;
    notification_db->tts_seq_num        = 0;

    notification_db->sync_action_index  = 0xFF;
    notification_db->sync_action_type   = 0;
    notification_db->sync_action_flag   = 0;
    notification_db->flow_go            = true;

    return true;

fail_init_ack_timer:
    sys_timer_delete(notification_db->poll_timer);
fail_init_poll_timer:
    remote_relay_unregister(notification_db->relay_handle);
fail_register_remote_relay:
    notification_buffer_deinit();
fail_init_vp_buffer:
    free(notification_db);
    notification_db = NULL;
fail_alloc_db:
    AUDIO_PRINT_ERROR1("notification_init: failed %d", -ret);
    return false;
}

void notification_deinit(void)
{
    T_NOTIFICATION_ELEM *elem;

    if (notification_db != NULL)
    {
        elem = os_queue_out(&notification_db->idle_queue);
        while (elem != NULL)
        {
            elem = os_queue_out(&notification_db->idle_queue);
        }

        elem = os_queue_out(&notification_db->pending_queue);
        while (elem != NULL)
        {
            elem = os_queue_out(&notification_db->pending_queue);
        }

        elem = os_queue_out(&notification_db->play_queue);
        while (elem != NULL)
        {
            elem = os_queue_out(&notification_db->play_queue);
        }

        if (notification_db->tts_instance != NULL)
        {
            audio_path_destroy(notification_db->tts_instance->handle);
        }

        if (notification_db->vp_path_handle != NULL)
        {
            audio_path_destroy(notification_db->vp_path_handle);
        }

        if (notification_db->tone_path_handle != NULL)
        {
            audio_path_destroy(notification_db->tone_path_handle);
        }

        if (notification_db->ack_timer != NULL)
        {
            sys_timer_delete(notification_db->ack_timer);
        }

        if (notification_db->poll_timer != NULL)
        {
            sys_timer_delete(notification_db->poll_timer);
        }

        if (notification_db->handshake_timer != NULL)
        {
            sys_timer_delete(notification_db->handshake_timer);
        }

        if (notification_db->relay_handle != NULL)
        {
            remote_relay_unregister(notification_db->relay_handle);
        }

        notification_buffer_deinit();

        free(notification_db);
        notification_db = NULL;
    }
}

T_TTS_HANDLE notification_tts_alloc(uint32_t length,
                                    uint32_t cfg)
{
    T_TTS_INSTANCE *instance;

    instance = malloc(sizeof(T_TTS_INSTANCE));
    if (instance != NULL)
    {
        T_AUDIO_PATH_PARAM  path_param;
        T_AUDIO_FORMAT_INFO tts_format;

        notification_vp_format_init(&tts_format, cfg);

        instance->length = length;
        path_param.vp.device       = AUDIO_DEVICE_OUT_SPK;
        path_param.vp.dac_level    = NOTIFICATION_VOLUME_DEFAULT;
        path_param.vp.decoder_info = &tts_format;
        instance->handle = audio_path_create(AUDIO_CATEGORY_VP,
                                             path_param,
                                             audio_path_tts_cback);
        if (instance->handle == NULL)
        {
            free(instance);
            instance = NULL;
        }
    }

    return (T_TTS_HANDLE)instance;
}

bool notification_tts_free(T_TTS_HANDLE handle)
{
    T_TTS_INSTANCE *instance;

    instance = (T_TTS_INSTANCE *)handle;
    if (instance != NULL)
    {
        if (notification_db->tts_instance == instance)
        {
            notification_db->tts_instance = NULL;
        }

        audio_path_destroy(instance->handle);
        free(instance);
        return true;
    }

    return false;
}

uint8_t notification_get_pending_elem_num(void)
{
    return (notification_db->pending_queue.count + notification_db->play_queue.count);
}

uint8_t notification_remaining_count_get(uint8_t type)
{

    T_NOTIFICATION_ELEM *elem;
    uint8_t              count = 0;

    elem = os_queue_peek(&notification_db->play_queue, 0);
    while (elem != NULL)
    {
        if (elem->type == type)
        {
            count++;
        }
        elem = elem->next;
    }

    elem = os_queue_peek(&notification_db->pending_queue, 0);
    while (elem != NULL)
    {
        if (elem->type == type)
        {
            count++;
        }
        elem = elem->next;
    }

    if (notification_db->state == NOTIFICATION_STATE_RINGTONE)
    {
        if (type == NOTIFICATION_TYPE_RINGTONE)
        {
            count++;
        }
    }
    else if (notification_db->state == NOTIFICATION_STATE_VP)
    {
        if (type == NOTIFICATION_TYPE_VP)
        {
            count++;
        }
    }
    else if (notification_db->state == NOTIFICATION_STATE_TTS)
    {
        if (type == NOTIFICATION_TYPE_TTS)
        {
            count++;
        }
    }
    else if (notification_db->state == NOTIFICATION_STATE_RELAY)
    {
        if (type == notification_db->relay_type)
        {
            count++;
        }
    }

    return count;
}

void notification_sync_lost(void)
{
    notification_unlock();

    if (notification_get_state() == NOTIFICATION_STATE_RELAY)
    {
        sys_timer_stop(notification_db->poll_timer);
        sys_timer_stop(notification_db->ack_timer);
        notification_set_state(NOTIFICATION_STATE_IDLE);
        notification_set_relay_type(NOTIFICATION_TYPE_NONE);
        notification_db->sync_play_index = 0xff;
        notification_play();
    }
}
