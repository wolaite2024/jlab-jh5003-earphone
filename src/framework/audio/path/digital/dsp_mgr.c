/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "trace.h"
#include "wdg.h"
#include "audio_type.h"
#include "audio_mgr.h"
#include "audio_codec.h"
#include "dsp_driver.h"
#include "dsp_ipc.h"
#include "dsp_loader.h"
#include "dsp_mgr.h"

/* TODO Remove Start */
#include "fmc_api.h"
#include "bin_loader_driver.h"
#include "audio_route_cfg.h"
#include "bt_mgr.h"
#include "remote.h"

#define DSP_MGR_SESSION_DECODER_CREATED         (0x01)
#define DSP_MGR_SESSION_DECODER_STARTED         (0x02)
#define DSP_MGR_SESSION_ENCODER_CREATED         (0x04)
#define DSP_MGR_SESSION_ENCODER_STARTED         (0x08)
#define DSP_MGR_SESSION_ALGO_LOAD               (0x10)

#define DSP_MGR_WAIT_POWER_OFF                  (0x01)

#define DSP_GATEWAY_TX_WAIT_CLEAR               (0x01)
#define DSP_GATEWAY_RX_WAIT_CLEAR               (0x02)

#define TIMER_DSP_POWER_DOWN    2000

typedef struct t_dsp_config_header
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
} T_DSP_CONFIG_HEADER;

typedef struct t_dsp_algo_entry
{
    uint8_t  category;
    uint8_t  bulk_num;
    uint16_t total_len;
    uint32_t offset;
} T_DSP_ALGO_ENTRY;

typedef struct t_dsp_algo_block
{
    uint16_t         sync_word;
    uint16_t         reserved;
    uint32_t         block_len;
    uint8_t          version;
    uint8_t          category_num;
    uint16_t         bulk_size;
    T_DSP_ALGO_ENTRY entry[AUDIO_CATEGORY_NUM];
} T_DSP_ALGO_BLOCK;

typedef struct t_coder_instance
{
    uint8_t  coder_id;
    uint8_t  chann_num;
    uint8_t  frame_num;
    uint16_t format_size;
    uint32_t sample_rate;
    uint32_t transport_address;
    union
    {
        T_DIPC_PCM_CODER_FORMAT     pcm;
        T_DIPC_CVSD_CODER_FORMAT    cvsd;
        T_DIPC_MSBC_CODER_FORMAT    msbc;
        T_DIPC_SBC_CODER_FORMAT     sbc;
        T_DIPC_AAC_CODER_FORMAT     aac;
        T_DIPC_OPUS_CODER_FORMAT    opus;
        T_DIPC_MP3_CODER_FORMAT     mp3;
        T_DIPC_LC3_CODER_FORMAT     lc3;
        T_DIPC_LDAC_CODER_FORMAT    ldac;
        T_DIPC_FLAC_CODER_FORMAT    flac;
        T_DIPC_LHDC_CODER_FORMAT    lhdc;
        T_DIPC_G729_CODER_FORMAT    g729;
        T_DIPC_LC3PLUS_CODER_FORMAT lc3plus;
    } format;
} T_CODER_INSTANCE;

typedef struct t_tone_coef
{
    void *coefficient;
    uint16_t size;
} T_TONE_COEF;

typedef enum t_dsp_mgr_session_state
{
    DSP_MGR_SESSION_STATE_DISABLED    = 0x00,
    DSP_MGR_SESSION_STATE_ENABLING    = 0x01,
    DSP_MGR_SESSION_STATE_ENABLED     = 0x02,
    DSP_MGR_SESSION_STATE_SUSPENDING  = 0x03,
    DSP_MGR_SESSION_STATE_SUSPENDED   = 0x04,
    DSP_MGR_SESSION_STATE_DISABLING   = 0x05,
} T_DSP_MGR_SESSION_STATE;

typedef struct t_dsp_mgr_session
{
    struct t_dsp_mgr_session       *p_next;
    void                           *context;
    P_DSP_MGR_SESSION_CBACK         callback;
    T_CODER_INSTANCE               *decoder;
    T_CODER_INSTANCE               *encoder;
    uint32_t                        device;
    T_TONE_COEF                     coef;
    uint8_t                         data_mode;
    T_AUDIO_CATEGORY                category;
    T_DSP_MGR_SESSION_STATE         state;
    uint32_t                        dac_sample_rate;
    uint32_t                        adc_sample_rate;
    uint16_t                        algo_offset;
    uint8_t                         flags;
    uint8_t                         aggressiveness_level;
} T_DSP_MGR_SESSION;

typedef struct t_dsp_gateway_instance
{
    struct t_dsp_gateway_instance  *p_next;
    uint8_t                         type;
    uint8_t                         id;
    uint8_t                         tx_refcnt;
    uint8_t                         rx_refcnt;
    uint8_t                         flag;
} T_DSP_GATEWAY_INSTANCE;

typedef enum t_dsp_mgr_session_event
{
    DSP_MGR_SESSION_EVENT_ENABLE_REQ  = 0x00,
    DSP_MGR_SESSION_EVENT_ENABLE_RSP  = 0x01,
    DSP_MGR_SESSION_EVENT_DISABLE_REQ = 0x02,
    DSP_MGR_SESSION_EVENT_DISABLE_RSP = 0x03,
    DSP_MGR_SESSION_EVENT_SUSPEND_REQ = 0x04,
    DSP_MGR_SESSION_EVENT_SUSPEND_RSP = 0x05,
    DSP_MGR_SESSION_EVENT_RESUME_REQ  = 0x06,
    DSP_MGR_SESSION_EVENT_RESUME_RSP  = 0x07,
} T_DSP_MGR_SESSION_EVENT;

typedef enum t_dsp_mgr_dsp_event
{
    DSP_MGR_EVENT_BOOT              = 0x00,
    DSP_MGR_EVENT_POWER_ON_REQ      = 0x01,
    DSP_MGR_EVENT_POWER_ON_RSP      = 0x02,
    DSP_MGR_EVENT_POWER_DOWN_REQ    = 0x03,
    DSP_MGR_EVENT_POWER_DOWN_RSP    = 0x04,
    DSP_MGR_EVENT_SUSPEND_REQ       = 0x05,
    DSP_MGR_EVENT_SUSPEND_RSP       = 0x06,
    DSP_MGR_EVENT_RESUME_REQ        = 0x07,
    DSP_MGR_EVENT_RESUME_RSP        = 0x08,
    DSP_MGR_EVENT_LOADING_FINISH    = 0x23,
    DSP_MGR_EVENT_START             = 0x52,
    DSP_MGR_EVENT_STOP              = 0x53,
    DSP_MGR_EVENT_STOP_RSP          = 0x54,
} T_DSP_MGR_DSP_EVENT;

typedef struct t_dsp_mgr_activity
{
    struct t_dsp_mgr_activity  *p_next;
    T_DSP_MGR_SESSION          *session;
    T_DSP_MGR_SESSION_EVENT     event;
} T_DSP_MGR_ACTIVITY;

typedef struct t_dsp_mgr_db
{
    T_OS_QUEUE                  session_queue;
    T_OS_QUEUE                  gateway_queue;
    T_OS_QUEUE                  activity_queue;
    T_DSP_MGR_ACTIVITY         *working_activity;
    T_DSP_MGR_SESSION          *suspending_session;
    T_SYS_IPC_HANDLE            dsp_event;
    uint32_t                    algo_block_offset;
    T_DSP_ALGO_BLOCK            algo_block;
    T_DSP_MGR_STATE             state;
    uint8_t                     dsp2_ref;
    uint8_t                     flags;
    bool                        lps_en;
    T_SYS_TIMER_HANDLE          power_off_timer;
} T_DSP_MGR_DB;

typedef bool (*P_DSP_MGR_FILTER_CBACK)(T_AUDIO_ROUTE_ENTRY *entry);
typedef void (*P_DSP_MGR_GATEWAY_CBACK)(T_DSP_MGR_SESSION          *session,
                                        T_AUDIO_ROUTE_GATEWAY_ATTR *attr);
/* TODO Remove End */

static void dsp_mgr_hw_fsm(T_DSP_MGR_DSP_EVENT event);
static void dsp_mgr_session_fsm(T_DSP_MGR_SESSION *session, T_DSP_MGR_SESSION_EVENT event);

static T_DSP_MGR_DB dsp_mgr_db = {0};

static T_DSP_GATEWAY_INSTANCE *dsp_mgr_gateway_instance_get(uint8_t type,
                                                            uint8_t id)
{
    T_DSP_GATEWAY_INSTANCE *gateway;

    gateway = os_queue_peek(&dsp_mgr_db.gateway_queue, 0);
    while (gateway != NULL)
    {
        if ((gateway->type == type) && (gateway->id == id))
        {
            break;
        }

        gateway = gateway->p_next;
    }

    return gateway;
}

static void dsp_mgr_pcm_format_init(T_CODER_INSTANCE *coder_inst,
                                    void             *coder_attr)
{
    T_DIPC_PCM_CODER_FORMAT *pcm_format;
    T_AUDIO_PCM_ATTR *pcm_attr;

    coder_inst->coder_id = DIPC_CODER_ID_PCM;
    coder_inst->format_size = sizeof(T_DIPC_PCM_CODER_FORMAT);
    pcm_format = &(coder_inst->format.pcm);
    pcm_attr = (T_AUDIO_PCM_ATTR *)coder_attr;

    pcm_format->sample_rate = pcm_attr->sample_rate;
    pcm_format->frame_size = audio_codec_frame_size_get(AUDIO_FORMAT_TYPE_PCM, coder_attr);
    pcm_format->chann_num = audio_codec_chann_num_get(AUDIO_FORMAT_TYPE_PCM, coder_attr);
    pcm_format->bit_width = pcm_attr->bit_width;
    pcm_format->chann_location = pcm_attr->chann_location;

    coder_inst->sample_rate = pcm_attr->sample_rate;
    coder_inst->chann_num = pcm_attr->chann_num;
}

static void dsp_mgr_cvsd_format_init(T_CODER_INSTANCE *coder_inst,
                                     void             *coder_attr)
{
    T_DIPC_CVSD_CODER_FORMAT *cvsd_format;
    T_AUDIO_CVSD_ATTR *cvsd_attr;

    coder_inst->coder_id = DIPC_CODER_ID_CVSD;
    coder_inst->format_size = sizeof(T_DIPC_CVSD_CODER_FORMAT);
    cvsd_format = &(coder_inst->format.cvsd);
    cvsd_attr = (T_AUDIO_CVSD_ATTR *)coder_attr;

    cvsd_format->sample_rate = cvsd_attr->sample_rate;
    cvsd_format->frame_size = audio_codec_frame_size_get(AUDIO_FORMAT_TYPE_CVSD, coder_attr);
    cvsd_format->chann_num = audio_codec_chann_num_get(AUDIO_FORMAT_TYPE_CVSD, coder_attr);
    cvsd_format->bit_width = 0x10;
    cvsd_format->chann_location = cvsd_attr->chann_location;

    coder_inst->sample_rate = cvsd_format->sample_rate;
    coder_inst->chann_num = cvsd_attr->chann_num;
}


static void dsp_mgr_msbc_format_init(T_CODER_INSTANCE *coder_inst,
                                     void             *coder_attr)
{
    T_DIPC_MSBC_CODER_FORMAT *msbc_format;
    T_AUDIO_MSBC_ATTR *msbc_attr;

    coder_inst->coder_id = DIPC_CODER_ID_MSBC;
    coder_inst->format_size = sizeof(T_DIPC_MSBC_CODER_FORMAT);
    msbc_format = &(coder_inst->format.msbc);
    msbc_attr = (T_AUDIO_MSBC_ATTR *)coder_attr;

    msbc_format->sample_rate = msbc_attr->sample_rate;
    msbc_format->frame_size = audio_codec_frame_size_get(AUDIO_FORMAT_TYPE_MSBC, coder_attr);
    msbc_format->chann_num = audio_codec_chann_num_get(AUDIO_FORMAT_TYPE_MSBC, coder_attr);
    msbc_format->bit_width = 0x10;
    msbc_format->chann_mode = msbc_attr->chann_mode;
    msbc_format->block_length = msbc_attr->block_length;
    msbc_format->subband_num = msbc_attr->subband_num;
    msbc_format->allocation_method = msbc_attr->allocation_method;
    msbc_format->bitpool = msbc_attr->bitpool;
    msbc_format->chann_location = msbc_attr->chann_location;

    coder_inst->sample_rate = msbc_attr->sample_rate;
    coder_inst->chann_num = msbc_format->chann_num;
}

static void dsp_mgr_sbc_format_init(T_CODER_INSTANCE *coder_inst,
                                    void             *coder_attr)
{
    T_DIPC_SBC_CODER_FORMAT *sbc_format;
    T_AUDIO_SBC_ATTR *sbc_attr;

    coder_inst->coder_id = DIPC_CODER_ID_SBC;
    coder_inst->format_size = sizeof(T_DIPC_SBC_CODER_FORMAT);
    sbc_format = &(coder_inst->format.sbc);
    sbc_attr = (T_AUDIO_SBC_ATTR *)coder_attr;

    sbc_format->sample_rate = sbc_attr->sample_rate;
    sbc_format->frame_size = audio_codec_frame_size_get(AUDIO_FORMAT_TYPE_SBC, coder_attr);
    sbc_format->chann_num = audio_codec_chann_num_get(AUDIO_FORMAT_TYPE_SBC, coder_attr);
    sbc_format->bit_width = 0x10;
    sbc_format->chann_mode = sbc_attr->chann_mode;
    sbc_format->block_length = sbc_attr->block_length;
    sbc_format->subband_num = sbc_attr->subband_num;
    sbc_format->allocation_method = sbc_attr->allocation_method;
    sbc_format->bitpool = sbc_attr->bitpool;
    sbc_format->chann_location = sbc_attr->chann_location;

    coder_inst->sample_rate = sbc_attr->sample_rate;
    coder_inst->chann_num = sbc_format->chann_num;
}

static void dsp_mgr_aac_format_init(T_CODER_INSTANCE *coder_inst,
                                    void             *coder_attr)
{
    T_DIPC_AAC_CODER_FORMAT *aac_format;
    T_AUDIO_AAC_ATTR *aac_attr;

    coder_inst->coder_id = DIPC_CODER_ID_AAC;
    coder_inst->format_size = sizeof(T_DIPC_AAC_CODER_FORMAT);
    aac_attr = (T_AUDIO_AAC_ATTR *)coder_attr;
    aac_format = &(coder_inst->format.aac);

    aac_format->sample_rate = aac_attr->sample_rate;
    aac_format->frame_size = audio_codec_frame_size_get(AUDIO_FORMAT_TYPE_AAC, aac_attr);
    aac_format->chann_num = audio_codec_chann_num_get(AUDIO_FORMAT_TYPE_AAC, aac_attr);
    aac_format->bit_width = 0x10;
    aac_format->transport_format = aac_attr->transport_format;
    aac_format->object_type = aac_attr->object_type;
    aac_format->vbr = aac_attr->vbr;
    aac_format->bitrate = aac_attr->bitrate;
    aac_format->chann_location = aac_attr->chann_location;

    coder_inst->sample_rate = aac_attr->sample_rate;
    coder_inst->chann_num = aac_attr->chann_num;
}

static void dsp_mgr_opus_format_init(T_CODER_INSTANCE *coder_inst,
                                     void             *coder_attr)
{
    T_DIPC_OPUS_CODER_FORMAT *opus_format;
    T_AUDIO_OPUS_ATTR *opus_attr;

    coder_inst->coder_id = DIPC_CODER_ID_OPUS;
    coder_inst->format_size = sizeof(T_DIPC_OPUS_CODER_FORMAT);
    opus_format = &(coder_inst->format.opus);
    opus_attr = (T_AUDIO_OPUS_ATTR *)coder_attr;

    opus_format->sample_rate = opus_attr->sample_rate;
    opus_format->frame_size = audio_codec_frame_size_get(AUDIO_FORMAT_TYPE_OPUS, coder_attr);
    opus_format->chann_num = audio_codec_chann_num_get(AUDIO_FORMAT_TYPE_OPUS, coder_attr);
    opus_format->bit_width = 0x10;
    opus_format->cbr = opus_attr->cbr;
    opus_format->cvbr = opus_attr->cvbr;
    opus_format->mode = opus_attr->mode;
    opus_format->complexity = opus_attr->complexity;
    opus_format->bitrate = opus_attr->bitrate;
    opus_format->chann_location = opus_attr->chann_location;
    opus_format->entropy = opus_attr->entropy;

    coder_inst->sample_rate = opus_attr->sample_rate;
    coder_inst->chann_num = opus_attr->chann_num;
}

static void dsp_mgr_flac_format_init(T_CODER_INSTANCE *coder_inst,
                                     void             *coder_attr)
{

}

static void dsp_mgr_mp3_format_init(T_CODER_INSTANCE *coder_inst,
                                    void             *coder_attr)
{
    T_DIPC_MP3_CODER_FORMAT *mp3_format;
    T_AUDIO_MP3_ATTR *mp3_attr;

    coder_inst->coder_id = DIPC_CODER_ID_MP3;
    coder_inst->format_size = sizeof(T_DIPC_MP3_CODER_FORMAT);
    mp3_format = &(coder_inst->format.mp3);
    mp3_attr = (T_AUDIO_MP3_ATTR *)coder_attr;

    mp3_format->sample_rate = mp3_attr->sample_rate;
    mp3_format->frame_size = audio_codec_frame_size_get(AUDIO_FORMAT_TYPE_MP3, mp3_attr);
    mp3_format->chann_num = audio_codec_chann_num_get(AUDIO_FORMAT_TYPE_MP3, coder_attr);
    mp3_format->bit_width = 0x10;
    mp3_format->chann_mode = mp3_attr->chann_mode;
    mp3_format->version = mp3_attr->version;
    mp3_format->layer = mp3_attr->layer;
    mp3_format->bitrate = mp3_attr->bitrate;
    mp3_format->chann_location = mp3_attr->chann_location;

    coder_inst->sample_rate = mp3_attr->sample_rate;
    coder_inst->chann_num = mp3_format->chann_num;
}

static void dsp_mgr_lc3_format_init(T_CODER_INSTANCE *coder_inst,
                                    void             *coder_attr)
{
    T_DIPC_LC3_CODER_FORMAT *lc3_format;
    T_AUDIO_LC3_ATTR *lc3_attr;

    coder_inst->coder_id = DIPC_CODER_ID_LC3;
    coder_inst->format_size = sizeof(T_DIPC_LC3_CODER_FORMAT);
    lc3_format = &(coder_inst->format.lc3);
    lc3_attr = (T_AUDIO_LC3_ATTR *)coder_attr;

    lc3_format->sample_rate = lc3_attr->sample_rate;
    lc3_format->frame_size = audio_codec_frame_size_get(AUDIO_FORMAT_TYPE_LC3, coder_attr);
    lc3_format->chann_num = audio_codec_chann_num_get(AUDIO_FORMAT_TYPE_LC3, coder_attr);
    lc3_format->bit_width = lc3_attr->bit_width;
    lc3_format->frame_length = lc3_attr->frame_length;
    lc3_format->chann_location = lc3_attr->chann_location;
    lc3_format->presentation_delay = lc3_attr->presentation_delay;

    coder_inst->sample_rate = lc3_attr->sample_rate;
    coder_inst->chann_num = lc3_format->chann_num;
}

static void dsp_mgr_ldac_format_init(T_CODER_INSTANCE *coder_inst,
                                     void             *coder_attr)
{
    T_DIPC_LDAC_CODER_FORMAT *ldac_format;
    T_AUDIO_LDAC_ATTR *ldac_attr;

    coder_inst->coder_id = DIPC_CODER_ID_LDAC;
    coder_inst->format_size = sizeof(T_DIPC_LDAC_CODER_FORMAT);
    ldac_format = &(coder_inst->format.ldac);
    ldac_attr = (T_AUDIO_LDAC_ATTR *)coder_attr;

    ldac_format->sample_rate = ldac_attr->sample_rate;
    ldac_format->frame_size = audio_codec_frame_size_get(AUDIO_FORMAT_TYPE_LDAC, coder_attr);
    ldac_format->chann_num = audio_codec_chann_num_get(AUDIO_FORMAT_TYPE_LDAC, coder_attr);
    ldac_format->bit_width = 0x18;
    ldac_format->chann_mode = ldac_attr->chann_mode;
    ldac_format->chann_location = ldac_attr->chann_location;

    coder_inst->sample_rate = ldac_attr->sample_rate;
    coder_inst->chann_num = ldac_format->chann_num;
}

static void dsp_mgr_lhdc_format_init(T_CODER_INSTANCE *coder_inst,
                                     void             *coder_attr)
{
    T_DIPC_LHDC_CODER_FORMAT *lhdc_format;
    T_AUDIO_LHDC_ATTR *lhdc_attr;

    coder_inst->coder_id = DIPC_CODER_ID_LHDC;
    coder_inst->format_size = sizeof(T_DIPC_LHDC_CODER_FORMAT);
    lhdc_format = &(coder_inst->format.lhdc);
    lhdc_attr = (T_AUDIO_LHDC_ATTR *)coder_attr;

    lhdc_format->sample_rate = lhdc_attr->sample_rate;
    lhdc_format->frame_size = audio_codec_frame_size_get(AUDIO_FORMAT_TYPE_LHDC, coder_attr);
    lhdc_format->chann_num = audio_codec_chann_num_get(AUDIO_FORMAT_TYPE_LHDC, coder_attr);
    lhdc_format->bit_width = lhdc_attr->bit_width;
    lhdc_format->chann_location = lhdc_attr->chann_location;

    coder_inst->sample_rate = lhdc_format->sample_rate;
    coder_inst->chann_num = lhdc_attr->chann_num;
}

/* static uint8_t dsp_mgr_g729_format_init(T_CODER_INSTANCE *coder_inst,
                                           void             *coder_attr)
{

} */

static void dsp_mgr_lc3plus_format_init(T_CODER_INSTANCE *coder_inst,
                                        void             *coder_attr)
{
    T_DIPC_LC3PLUS_CODER_FORMAT *lc3plus_format;
    T_AUDIO_LC3PLUS_ATTR *lc3plus_attr;

    coder_inst->coder_id = DIPC_CODER_ID_LC3PLUS;
    coder_inst->format_size = sizeof(T_DIPC_LC3PLUS_CODER_FORMAT);
    lc3plus_format = &(coder_inst->format.lc3plus);
    lc3plus_attr = (T_AUDIO_LC3PLUS_ATTR *)coder_attr;

    lc3plus_format->sample_rate = lc3plus_attr->sample_rate;
    lc3plus_format->frame_size = audio_codec_frame_size_get(AUDIO_FORMAT_TYPE_LC3PLUS, coder_attr);
    lc3plus_format->chann_num = audio_codec_chann_num_get(AUDIO_FORMAT_TYPE_LC3PLUS, coder_attr);
    lc3plus_format->bit_width = lc3plus_attr->bit_width;
    lc3plus_format->frame_length = lc3plus_attr->frame_length;
    lc3plus_format->mode = lc3plus_attr->mode;
    lc3plus_format->chann_location = lc3plus_attr->chann_location;
    lc3plus_format->presentation_delay = lc3plus_attr->presentation_delay;

    coder_inst->sample_rate = lc3plus_attr->sample_rate;
    coder_inst->chann_num = lc3plus_format->chann_num;
}

static bool dsp_mgr_format_init(T_AUDIO_FORMAT_TYPE  coder_type,
                                void                *coder_inst,
                                void                *coder_attr)
{
    bool ret = true;

    switch (coder_type)
    {
    case AUDIO_FORMAT_TYPE_PCM:
        {
            dsp_mgr_pcm_format_init(coder_inst, coder_attr);
        }
        break;

    case AUDIO_FORMAT_TYPE_CVSD:
        {
            dsp_mgr_cvsd_format_init(coder_inst, coder_attr);
        }
        break;

    case AUDIO_FORMAT_TYPE_MSBC:
        {
            dsp_mgr_msbc_format_init(coder_inst, coder_attr);
        }
        break;

    case AUDIO_FORMAT_TYPE_SBC:
        {
            dsp_mgr_sbc_format_init(coder_inst, coder_attr);
        }
        break;

    case AUDIO_FORMAT_TYPE_AAC:
        {
            dsp_mgr_aac_format_init(coder_inst, coder_attr);
        }
        break;

    case AUDIO_FORMAT_TYPE_OPUS:
        {
            dsp_mgr_opus_format_init(coder_inst, coder_attr);
        }
        break;

    case AUDIO_FORMAT_TYPE_FLAC:
        {
            dsp_mgr_flac_format_init(coder_inst, coder_attr);
        }
        break;

    case AUDIO_FORMAT_TYPE_MP3:
        {
            dsp_mgr_mp3_format_init(coder_inst, coder_attr);
        }
        break;

    case AUDIO_FORMAT_TYPE_LC3:
        {
            dsp_mgr_lc3_format_init(coder_inst, coder_attr);
        }
        break;

    case AUDIO_FORMAT_TYPE_LDAC:
        {
            dsp_mgr_ldac_format_init(coder_inst, coder_attr);
        }
        break;

    case AUDIO_FORMAT_TYPE_LHDC:
        {
            dsp_mgr_lhdc_format_init(coder_inst, coder_attr);
        }
        break;

    case AUDIO_FORMAT_TYPE_LC3PLUS:
        {
            dsp_mgr_lc3plus_format_init(coder_inst, coder_attr);
        }
        break;

    case AUDIO_FORMAT_TYPE_G729:
    default:
        {
            ret = false;
            break;
        }
    }

    return ret;
}

T_DSP_MGR_STATE dsp_mgr_state_get(void)
{
    return dsp_mgr_db.state;
}

static void dsp_mgr_state_set(T_DSP_MGR_STATE state)
{
    dsp_mgr_db.state = state;
}

static bool dsp_mgr_handle_check(T_DSP_MGR_SESSION_HANDLE handle)
{
    T_DSP_MGR_SESSION *session;

    session = os_queue_peek(&(dsp_mgr_db.session_queue), 0);
    while (session != NULL)
    {
        if (session == (T_DSP_MGR_SESSION *)handle)
        {
            return true;
        }
        session = session->p_next;
    }

    return false;
}

static bool dsp_param_init(void)
{
    T_DSP_CONFIG_HEADER header;
    uint32_t            offset;
    int32_t             ret = 0;

    offset = flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG);
    fmc_flash_nor_read(offset, &header, sizeof(T_DSP_CONFIG_HEADER));
    if (header.sync_word != APP_DATA_SYNC_WORD)
    {
        ret = 1;
        goto fail_load_header;
    }

    offset += header.algo_block_offset;
    dsp_mgr_db.algo_block_offset = offset;
    fmc_flash_nor_read(offset, &dsp_mgr_db.algo_block, sizeof(T_DSP_ALGO_BLOCK));
    if (dsp_mgr_db.algo_block.sync_word != APP_DATA_SYNC_WORD)
    {
        ret = 2;
        goto fail_load_block;
    }

    return true;

fail_load_block:
fail_load_header:
    DIPC_PRINT_ERROR1("dsp_param_init: failed %d", -ret);
    return false;
}

static void dsp_mgr_deployment_traverse(T_DSP_MGR_SESSION       *session,
                                        P_DSP_MGR_GATEWAY_CBACK  cback)
{
    T_AUDIO_ROUTE_PATH path;

    path = audio_route_path_get(session->context,
                                session->category,
                                session->device);

    if (path.entry_num != 0)
    {
        uint8_t i;

        for (i = 0; i < path.entry_num; ++i)
        {
            if (path.entry[i].gateway_attr.idx < DIPC_GATEWAY_MAX)
            {
                cback(session, &path.entry[i].gateway_attr);
            }
        }

        audio_route_path_give(&path);
    }
}

static bool dsp_mgr_is_idle(void)
{
    T_DSP_MGR_SESSION *session;

    session = os_queue_peek(&dsp_mgr_db.session_queue, 0);
    while (session != NULL)
    {
        if (session->state != DSP_MGR_SESSION_STATE_DISABLED)
        {
            return false;
        }
        session = session->p_next;
    }

    return true;
}

void dsp_mgr_decoder_ramp_gain_set(T_DSP_MGR_SESSION_HANDLE handle,
                                   uint16_t                 gain_step_left,
                                   uint16_t                 gain_step_right,
                                   uint16_t                 duration)
{
    dipc_decoder_ramp_gain_set((uint32_t)handle, gain_step_left, gain_step_right, duration);
}

void dsp_rws_set_role(uint8_t session_role,
                      uint8_t role)
{
    dsp_ipc_set_rws(session_role, role);
}

void dsp_remote_init(T_DSP_MGR_SESSION_HANDLE handle,
                     uint8_t                  clk_index,
                     bool                     sync_flag,
                     uint32_t                 timestamp)
{
    T_DSP_MGR_SESSION      *session;
    T_REMOTE_SESSION_STATE  session_state;
    T_REMOTE_SESSION_ROLE   session_role;

    session = (T_DSP_MGR_SESSION *)handle;
    session_role = remote_session_role_get();
    session_state = remote_session_state_get();

    DIPC_PRINT_TRACE2("dsp_remote_init: clk index %d, rws_state %d", clk_index, session_state);
    dsp_ipc_set_rws_init(clk_index, 0, session->category);

    if (session_role == REMOTE_SESSION_ROLE_PRIMARY &&
        session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        if (!sync_flag)
        {
            dsp_rws_set_role(session_role, RWS_ROLE_NONE);
        }
        else
        {
            dsp_rws_set_role(session_role, RWS_ROLE_SRC);
        }
    }
    else if (session_role == REMOTE_SESSION_ROLE_SECONDARY &&
             session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        if (!sync_flag)
        {
            dsp_rws_set_role(session_role, RWS_ROLE_NONE);
        }
        else
        {
            dsp_rws_set_role(session_role, RWS_ROLE_SNK);
        }
    }
    else// single
    {
        dsp_rws_set_role(session_role, RWS_ROLE_NONE);
    }

    dipc_timestamp_set((uint32_t)session, clk_index, timestamp);
}

void dsp_rws_seamless(uint8_t role)
{
    dsp_ipc_set_rws_seamless(role);
}

uint32_t dsp_mailbox_process(T_DSP_SCHED_MSG *p_msg)
{
    T_DSP_MAILBOX_DATA param;

    switch (p_msg->subtype)
    {
    case MAILBOX_D2H_DSP_EXCEPTION:
        {
            DBG_DIRECT("dsp exception msg_sub_type 0x%02X, p_data %p",
                       p_msg->subtype, p_msg->p_data);

            dsp_mgr_state_set(DSP_MGR_STATE_OFF);

            sys_ipc_publish("dsp_mgr", DSP_MGR_EVT_DSP_EXCEPTION, NULL);
            chip_reset(RESET_ALL);
        }
        break;

    case MAILBOX_D2H_SHARE_QUEUE_ADDRESS:
        {
            dsp_mgr_hw_fsm(DSP_MGR_EVENT_BOOT);
        }
        break;

    case MAILBOX_D2H_DSP_ADCDAC_DATA0:
    case MAILBOX_D2H_DSP_ADCDAC_DATA1:
        {
            param.data_len = p_msg->data_len;
            param.p_data = p_msg->p_data;
            sys_ipc_publish("dsp_mgr", DSP_MGR_EVT_MAILBOX_DSP_DATA, &param);
        }
        break;

    default:
        break;
    }

    return 0;
}

static void dsp_mgr_loader_finish(void)
{
    dsp_send_msg(DSP_MSG_PREPARE_READY,
                 0,
                 dsp_mgr_db.working_activity->session,
                 0);
}

static void dsp_mgr_drv_cback(uint32_t  event,
                              void     *msg)
{
    switch (event)
    {
    case DSP_DRV_EVT_DSP_LOAD_PART:
        {
            dsp_loader_next_bin();
        }
        break;

    case DSP_DRV_EVT_DSP_LOAD_FINISH:
        {
            dsp_loader_finish();
        }
        break;

    case DSP_DRV_EVT_CODEC_STATE:
        {
            sys_ipc_publish("dsp_mgr", DSP_MGR_EVT_CODEC_STATE, (void *)msg);
        }
        break;

    case DSP_DRV_EVT_MAILBOX:
        {
            T_DSP_SCHED_MSG *dsp_msg;
            dsp_msg = (T_DSP_SCHED_MSG *)msg;

            dsp_mailbox_process(dsp_msg);
        }
        break;

    case DSP_DRV_EVT_PREPARE_READY:
        {
            T_DSP_SCHED_MSG *dsp_msg;
            T_DSP_MGR_SESSION *session;

            dsp_msg = (T_DSP_SCHED_MSG *)msg;
            session = (T_DSP_MGR_SESSION *)dsp_msg->p_data;

            if (session)
            {
                if (session->callback != NULL)
                {
                    session->callback(session->context, DSP_MGR_EVT_PREPARE_READY, 0);
                }
            }
        }
        break;
    }
}

static void dsp_mgr_msg_post(T_DSP_MGR_SESSION *session,
                             T_DSP_MGR_EVENT    event,
                             uint32_t           param)
{
    if (os_queue_search(&dsp_mgr_db.session_queue, session))
    {
        session->callback(session->context, event, param);
    }
}

static void dsp_mgr_gateway_clear(T_DSP_MGR_SESSION          *session,
                                  T_AUDIO_ROUTE_GATEWAY_ATTR *attr)
{
    T_DSP_GATEWAY_INSTANCE *instance;

    instance = dsp_mgr_gateway_instance_get(attr->type, attr->idx);
    if (instance != NULL)
    {
        if (attr->dir == AUDIO_ROUTE_GATEWAY_DIR_RX)
        {
            if (instance->rx_refcnt > 0)
            {
                instance->rx_refcnt--;
                if (instance->rx_refcnt == 0)
                {
                    if (dipc_gateway_clear(attr->type, attr->idx, DIPC_DIRECTION_RX))
                    {
                        instance->flag |= DSP_GATEWAY_RX_WAIT_CLEAR;
                    }
                }
            }
        }

        if (attr->dir == AUDIO_ROUTE_GATEWAY_DIR_TX)
        {
            if (instance->tx_refcnt > 0)
            {
                instance->tx_refcnt--;
                if (instance->tx_refcnt == 0)
                {
                    if (dipc_gateway_clear(attr->type, attr->idx, DIPC_DIRECTION_TX))
                    {
                        instance->flag |= DSP_GATEWAY_TX_WAIT_CLEAR;
                    }
                }
            }
        }
    }
}

static bool dsp_mgr_gateway_clear_check(T_DSP_MGR_SESSION *session)
{
    dsp_mgr_deployment_traverse(session, dsp_mgr_gateway_clear);

    return false;
}

static void dsp_mgr_fifo_notify_handler(uint32_t param)
{
    T_DIPC_FIFO_NOTIFY       *info;
    T_DSP_MGR_LATENCY_NOTIFY  fifo_notify;

    info = (T_DIPC_FIFO_NOTIFY *)param;

    fifo_notify.packet_number = info->packet_number;
    fifo_notify.decoding_latency = info->decoding_latency;
    fifo_notify.queuing_latency = info->queuing_latency;

    dsp_mgr_msg_post((T_DSP_MGR_SESSION *)info->session_id,
                     DSP_MGR_EVT_FIFO_NOTIFY,
                     (uint32_t)&fifo_notify);
}

static void dsp_mgr_session_start(T_DSP_MGR_SESSION *session)
{
    switch (session->category)
    {
    case AUDIO_CATEGORY_AUDIO:
        {
            dipc_decoder_start((uint32_t)session, DIPC_FADE_ENABLE);
        }
        break;

    case AUDIO_CATEGORY_VOICE:
        {
            dipc_decoder_start((uint32_t)session, DIPC_FADE_ENABLE);
            dipc_encoder_start((uint32_t)session, DIPC_FADE_ENABLE);
        }
        break;

    case AUDIO_CATEGORY_RECORD:
        {
            dipc_encoder_start((uint32_t)session, DIPC_FADE_ENABLE);
        }
        break;

    case AUDIO_CATEGORY_LINE:
        {
            dipc_line_start((uint32_t)session, DIPC_FADE_DISABLE);
        }
        break;

    case AUDIO_CATEGORY_TONE:
        {
            dipc_ringtone_start((uint32_t)session, DIPC_FADE_DISABLE);
        }
        break;

    case AUDIO_CATEGORY_VP:
        {
            dipc_voice_prompt_start((uint32_t)session, DIPC_FADE_ENABLE);
        }
        break;

    case AUDIO_CATEGORY_PIPE:
        {
            dipc_codec_pipe_start((uint32_t)session);
        }
        break;

    case AUDIO_CATEGORY_APT:
        {
            dipc_apt_start((uint32_t)session, DIPC_FADE_DISABLE);
        }
        break;

    case AUDIO_CATEGORY_VAD:
        {
            dipc_vad_start((uint32_t)session);
        }
        break;

    case AUDIO_CATEGORY_KWS:
        {
            dipc_kws_start((uint32_t)session);
        }
        break;

    default:
        break;
    }
}

static void dsp_mgr_algo_load(T_DSP_MGR_SESSION *session)
{
    uint32_t offset;
    uint16_t total_len;

    offset    = dsp_mgr_db.algo_block_offset + dsp_mgr_db.algo_block.entry[session->category].offset;
    total_len = dsp_mgr_db.algo_block.entry[session->category].total_len;
    if (total_len != 0)
    {
        if (session->algo_offset < total_len)
        {
            uint16_t  load_len;
            uint8_t  *algo_buf;

            if (session->algo_offset + dsp_mgr_db.algo_block.bulk_size <= total_len)
            {
                load_len = dsp_mgr_db.algo_block.bulk_size;
            }
            else
            {
                load_len = total_len - session->algo_offset;
            }

            algo_buf = dipc_configuration_alloc(load_len);
            if (algo_buf != NULL)
            {
                uint32_t session_id;

                if (session->category == AUDIO_CATEGORY_RECORD)
                {
                    session_id = (uint32_t)session + 1;
                }
                else
                {
                    session_id = (uint32_t)session;
                }

                fmc_flash_nor_read(offset + session->algo_offset, algo_buf, load_len);
                dipc_configuration_load(DIPC_ENTITY_1, session_id, algo_buf, load_len);
                session->algo_offset += load_len;
                dipc_configuration_free(algo_buf);
            }
        }
        else
        {
            session->flags &= ~DSP_MGR_SESSION_ALGO_LOAD;
            session->algo_offset = 0;
            dsp_mgr_msg_post(session, DSP_MGR_EVT_EFFECT_REQ, 0);
            dsp_mgr_session_start(session);
        }
    }
    else
    {
        session->flags &= ~DSP_MGR_SESSION_ALGO_LOAD;
        session->algo_offset = 0;
        dsp_mgr_msg_post(session, DSP_MGR_EVT_EFFECT_REQ, 0);
        dsp_mgr_session_start(session);
    }
}

static void dsp_mgr_data_queue_init(T_DIPC_DATA_QUEUE_INFO *info)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)info->session_id;

    DIPC_PRINT_TRACE7("dsp_mgr_data_queue_init: status %d, session %p, category %d, src addr 0x%08x, "
                      "src size 0x%08x, snk addr 0x%08x, snk size 0x%08x",
                      info->status, session, session->category, info->src_transport_address,
                      info->src_transport_size, info->snk_transport_address, info->snk_transport_size);

    if (info->status == DIPC_ERROR_SUCCESS)
    {
        if (info->src_transport_size != 0)
        {
            T_CODER_INSTANCE *decoder;

            decoder = session->decoder;
            decoder->transport_address = info->src_transport_address;

            if (session->category == AUDIO_CATEGORY_VOICE)
            {
                session->flags |= DSP_MGR_SESSION_DECODER_CREATED;
            }
        }

        if (info->snk_transport_size != 0)
        {
            T_CODER_INSTANCE *encoder;

            encoder = session->encoder;
            encoder->transport_address = info->snk_transport_address;

            if (session->category == AUDIO_CATEGORY_VOICE)
            {
                session->flags |= DSP_MGR_SESSION_ENCODER_CREATED;
            }
        }
    }
}

static void dsp_mgr_session_signal_refresh_handler(T_DSP_MGR_EVENT event,
                                                   uint32_t        param)
{
    T_DIPC_SIGNAL_REFRESH    *info;
    T_DSP_MGR_SIGNAL_REFRESH  signal_refresh;

    info = (T_DIPC_SIGNAL_REFRESH *)param;

    signal_refresh.direction = info->direction;
    signal_refresh.freq_num = info->freq_num;
    signal_refresh.left_gain_table = info->left_gain_table;
    signal_refresh.right_gain_table = info->right_gain_table;

    dsp_mgr_msg_post((T_DSP_MGR_SESSION *)info->session_id,
                     event,
                     (uint32_t)&signal_refresh);
}

static void dsp_mgr_session_plc_notify_handler(T_DSP_MGR_EVENT event,
                                               uint32_t        param)
{
    T_DIPC_PLC_NOTIFY    *info;
    T_DSP_MGR_PLC_NOTIFY  plc_notify;

    info = (T_DIPC_PLC_NOTIFY *)param;

    plc_notify.total_sample_num = info->total_sample_num;
    plc_notify.plc_sample_num = info->plc_sample_num;

    dsp_mgr_msg_post((T_DSP_MGR_SESSION *)info->session_id,
                     event,
                     (uint32_t)&plc_notify);
}

/* TODO: remove when session-id adapt complete */
T_DSP_MGR_SESSION *dsp_mgr_session_find(T_AUDIO_CATEGORY category)
{
    T_DSP_MGR_SESSION *session;

    session = os_queue_peek(&dsp_mgr_db.session_queue, 0);

    while (session != NULL)
    {
        if (session->state == DSP_MGR_SESSION_STATE_ENABLED)
        {
            if (session->category == category)
            {
                return session;
            }
        }
        session = session->p_next;
    }

    return NULL;
}
/* TODO: end */

static void dsp_mgr_gateway_clear_handler(uint32_t param)
{
    T_DIPC_GATEWAY_STATUS *status;
    T_DSP_GATEWAY_INSTANCE *gateway;

    status = (T_DIPC_GATEWAY_STATUS *)param;
    gateway = dsp_mgr_gateway_instance_get(status->type, status->id);

    if (gateway != NULL)
    {
        if (status->dir == DIPC_DIRECTION_TX)
        {
            gateway->flag &= ~DSP_GATEWAY_TX_WAIT_CLEAR;
        }
        if (status->dir == DIPC_DIRECTION_RX)
        {
            gateway->flag &= ~DSP_GATEWAY_RX_WAIT_CLEAR;
        }

        if ((gateway->tx_refcnt == 0) && (gateway->rx_refcnt == 0) &&
            ((gateway->flag & (DSP_GATEWAY_TX_WAIT_CLEAR | DSP_GATEWAY_RX_WAIT_CLEAR)) == 0))
        {
            os_queue_delete(&dsp_mgr_db.gateway_queue, gateway);
            free(gateway);
        }
    }
}

static void dsp_mgr_session_power_control_handler(T_DIPC_POWER_CONTROL *param)
{
    if (param->mode == DIPC_POWER_CONTROL_MODE_ON)
    {
        dsp_mgr_hw_fsm(DSP_MGR_EVENT_POWER_ON_RSP);
    }
    else if (param->mode == DIPC_POWER_CONTROL_MODE_OFF)
    {
        dsp_mgr_db.flags &= ~DSP_MGR_WAIT_POWER_OFF;

        dsp_mgr_hw_fsm(DSP_MGR_EVENT_POWER_DOWN_RSP);
    }
    else if (param->mode == DIPC_POWER_CONTROL_MODE_SUSPEND)
    {
        dsp_mgr_db.suspending_session = NULL;
        dsp_mgr_hw_fsm(DSP_MGR_EVENT_SUSPEND_RSP);

        dsp_mgr_session_fsm(dsp_mgr_db.working_activity->session, DSP_MGR_SESSION_EVENT_SUSPEND_RSP);
    }
    else if (param->mode == DIPC_POWER_CONTROL_MODE_RESUME)
    {
        dsp_mgr_hw_fsm(DSP_MGR_EVENT_RESUME_RSP);

        dsp_mgr_session_fsm(dsp_mgr_db.working_activity->session, DSP_MGR_SESSION_EVENT_RESUME_RSP);
    }
}

static bool dsp_mgr_ipc_event_handler(T_DSP_IPC_EVENT event,
                                      uint32_t        param)
{
    if (event != DSP_IPC_EVT_DATA_ACK &&
        event != DSP_IPC_EVT_DATA_IND)
    {
        DIPC_PRINT_INFO2("dsp_mgr_ipc_event_handler: event 0x%04x, param 0x%08x", event, param);
    }

    switch (event)
    {
    case DSP_IPC_EVT_POWER_CONTROL_ACK:
        {
            dsp_mgr_session_power_control_handler((T_DIPC_POWER_CONTROL *)param);
        }
        break;

    case DSP_IPC_EVT_B2BMSG:
        {
            sys_ipc_publish("dsp_mgr", DSP_MGR_EVT_DSP_INTER_MSG, (void *)(param));
        }
        break;

    case DSP_IPC_EVT_DSP_SYNC_V2_SUCC:
        {
            T_DSP_MGR_SESSION *session;

            session = dsp_mgr_session_find(AUDIO_CATEGORY_AUDIO);

            if (session == NULL)
            {
                session = dsp_mgr_session_find(AUDIO_CATEGORY_VOICE);
            }

            if (session != NULL)
            {
                session->callback(session->context, DSP_MGR_EVT_DSP_SYNC_V2_SUCC, 0);
            }
        }
        break;

    case DSP_IPC_EVT_DSP_UNSYNC:
    case DSP_IPC_EVT_BTCLK_EXPIRED:
        {
            T_DSP_MGR_SESSION *session;

            session = dsp_mgr_session_find(AUDIO_CATEGORY_AUDIO);

            if (session == NULL)
            {
                session = dsp_mgr_session_find(AUDIO_CATEGORY_VOICE);
            }

            if (session != NULL)
            {
                session->callback(session->context, DSP_MGR_EVT_DSP_UNSYNC, 0);
            }
        }
        break;

    case DSP_IPC_EVT_DSP_SYNC_UNLOCK:
        {
            T_DSP_MGR_SESSION *session;

            session = dsp_mgr_session_find(AUDIO_CATEGORY_AUDIO);

            if (session == NULL)
            {
                session = dsp_mgr_session_find(AUDIO_CATEGORY_VOICE);
            }

            if (session != NULL)
            {
                session->callback(session->context, DSP_MGR_EVT_DSP_SYNC_UNLOCK, 0);
            }
        }
        break;

    case DSP_IPC_EVT_DSP_SYNC_LOCK:
        {
            T_DSP_MGR_SESSION *session;

            session = dsp_mgr_session_find(AUDIO_CATEGORY_AUDIO);

            if (session == NULL)
            {
                session = dsp_mgr_session_find(AUDIO_CATEGORY_VOICE);
            }

            if (session != NULL)
            {
                session->callback(session->context, DSP_MGR_EVT_DSP_SYNC_LOCK, 0);
            }
        }
        break;

    case DSP_IPC_EVT_SYNC_EMPTY:
        {
            T_DSP_MGR_SESSION *session;

            session = dsp_mgr_session_find(AUDIO_CATEGORY_AUDIO);

            if (session == NULL)
            {
                session = dsp_mgr_session_find(AUDIO_CATEGORY_VOICE);
            }

            if (session != NULL)
            {
                session->callback(session->context, DSP_MGR_EVT_SYNC_EMPTY, 0);
            }
        }
        break;

    case DSP_IPC_EVT_SYNC_LOSE_TIMESTAMP:
        {
            T_DSP_MGR_SESSION *session;

            session = dsp_mgr_session_find(AUDIO_CATEGORY_AUDIO);

            if (session == NULL)
            {
                session = dsp_mgr_session_find(AUDIO_CATEGORY_VOICE);
            }

            if (session != NULL)
            {
                session->callback(session->context, DSP_MGR_EVT_SYNC_LOSE_TIMESTAMP, 0);
            }
        }
        break;

    case DSP_IPC_EVT_JOIN_CLK:
        {
            T_DSP_MGR_SESSION *session;

            session = dsp_mgr_session_find(AUDIO_CATEGORY_AUDIO);

            if (session == NULL)
            {
                session = dsp_mgr_session_find(AUDIO_CATEGORY_VOICE);
            }

            if (session != NULL)
            {
                session->callback(session->context, DSP_MGR_EVT_DSP_JOIN_INFO, param);
            }
        }
        break;

    case DSP_IPC_EVT_GATEWAY_CLEAR:
        {
            dsp_mgr_gateway_clear_handler(param);
        }
        break;

    case DSP_IPC_EVT_CONFIGURATION_LOADED:
        {
            T_DIPC_CONFIGURATION_LOAD *info = (T_DIPC_CONFIGURATION_LOAD *)param;

            if (info->status == DIPC_ERROR_SUCCESS)
            {
                T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)info->session_id;

                if (os_queue_search(&dsp_mgr_db.session_queue, session) &&
                    (session->flags & DSP_MGR_SESSION_ALGO_LOAD))
                {
                    dsp_mgr_algo_load(session);
                }
            }
        }
        break;

    case DSP_IPC_EVT_CODEC_PIPE_PRE_MIXER_ADD:
    case DSP_IPC_EVT_CODEC_PIPE_POST_MIXER_ADD:
        {
            dsp_mgr_msg_post((T_DSP_MGR_SESSION *)param, DSP_MGR_EVT_CODEC_PIPE_MIX, param);
        }
        break;

    case DSP_IPC_EVT_CODEC_PIPE_MIXER_REMOVE:
        {
            dsp_mgr_msg_post((T_DSP_MGR_SESSION *)param, DSP_MGR_EVT_CODEC_PIPE_DEMIX, param);
        }
        break;

    case DSP_IPC_EVT_CODEC_PIPE_CREATED:
    case DSP_IPC_EVT_DECODER_CREATED:
    case DSP_IPC_EVT_ENCODER_CREATED:
    case DSP_IPC_EVT_VP_CREATED:
    case DSP_IPC_EVT_TONE_CREATED:
    case DSP_IPC_EVT_APT_CREATED:
    case DSP_IPC_EVT_LINE_CREATED:
    case DSP_IPC_EVT_VAD_CREATED:
    case DSP_IPC_EVT_KWS_CREATED:
        {
            T_DIPC_DATA_QUEUE_INFO *info = (T_DIPC_DATA_QUEUE_INFO *)param;
            T_DSP_MGR_SESSION      *session = (T_DSP_MGR_SESSION *)info->session_id;

            if (os_queue_search(&dsp_mgr_db.session_queue, session))
            {
                dsp_mgr_data_queue_init(info);
                if (session->category == AUDIO_CATEGORY_VOICE)
                {
                    if ((session->flags & DSP_MGR_SESSION_DECODER_CREATED) &&
                        (session->flags & DSP_MGR_SESSION_ENCODER_CREATED))
                    {
                        session->flags |= DSP_MGR_SESSION_ALGO_LOAD;
                        dsp_mgr_algo_load(session);
                    }
                }
                else
                {
                    session->flags |= DSP_MGR_SESSION_ALGO_LOAD;
                    dsp_mgr_algo_load(session);
                }
            }
        }
        break;

    case DSP_IPC_EVT_VP_STARTED:
    case DSP_IPC_EVT_TONE_STARTED:
    case DSP_IPC_EVT_DECODER_STARTED:
    case DSP_IPC_EVT_APT_STARTED:
    case DSP_IPC_EVT_LINE_STARTED:
    case DSP_IPC_EVT_KWS_STARTED:
        {
            T_DSP_MGR_SESSION *session;

            session = (T_DSP_MGR_SESSION *)param;
            if (session->category == AUDIO_CATEGORY_VOICE)
            {
                session->flags |= DSP_MGR_SESSION_DECODER_STARTED;
            }

            dsp_mgr_session_fsm(session, DSP_MGR_SESSION_EVENT_ENABLE_RSP);
        }
        break;

    case DSP_IPC_EVT_ENCODER_STARTED:
    case DSP_IPC_EVT_VAD_STARTED:
    case DSP_IPC_EVT_CODEC_PIPE_STARTED:
        {
            T_DSP_MGR_SESSION *session;

            session = (T_DSP_MGR_SESSION *)param;
            if (session->category == AUDIO_CATEGORY_VOICE)
            {
                session->flags |= DSP_MGR_SESSION_ENCODER_STARTED;
            }

            dsp_mgr_session_fsm(session, DSP_MGR_SESSION_EVENT_ENABLE_RSP);
        }
        break;

    case DSP_IPC_EVT_VP_STOPPED:
    case DSP_IPC_EVT_TONE_STOPPED:
    case DSP_IPC_EVT_DECODER_STOPPED:
    case DSP_IPC_EVT_APT_STOPPED:
    case DSP_IPC_EVT_LINE_STOPPED:
    case DSP_IPC_EVT_KWS_STOPPED:
        {
            T_DSP_MGR_SESSION *session;

            session = (T_DSP_MGR_SESSION *)param;
            if (session->category == AUDIO_CATEGORY_VOICE)
            {
                session->flags &= ~DSP_MGR_SESSION_DECODER_STARTED;
            }

            dsp_mgr_session_fsm(session, DSP_MGR_SESSION_EVENT_DISABLE_RSP);
        }
        break;

    case DSP_IPC_EVT_ENCODER_STOPPED:
    case DSP_IPC_EVT_VAD_STOPPED:
    case DSP_IPC_EVT_CODEC_PIPE_STOPPED:
        {
            T_DSP_MGR_SESSION *session;

            session = (T_DSP_MGR_SESSION *)param;
            if (session->category == AUDIO_CATEGORY_VOICE)
            {
                session->flags &= ~DSP_MGR_SESSION_ENCODER_STARTED;
            }

            dsp_mgr_session_fsm(session, DSP_MGR_SESSION_EVENT_DISABLE_RSP);
        }
        break;

    case DSP_IPC_EVT_DATA_ACK:
        {
            T_DIPC_DATA_ACK *ack_info;

            ack_info = (T_DIPC_DATA_ACK *)param;

            dsp_mgr_msg_post((T_DSP_MGR_SESSION *)ack_info->session_id,
                             DSP_MGR_EVT_DATA_REQ,
                             ack_info->session_id);
        }
        break;

    case DSP_IPC_EVT_DATA_COMPLETE:
        {
            dsp_mgr_msg_post((T_DSP_MGR_SESSION *)param, DSP_MGR_EVT_DATA_COMPLETE, param);
        }
        break;

    case DSP_IPC_EVT_DATA_IND:
        {
            T_DSP_MGR_SESSION *session;
            T_DIPC_DATA_INDICATE *indicate_info;

            indicate_info = (T_DIPC_DATA_INDICATE *)param;
            session = (T_DSP_MGR_SESSION *)indicate_info->session_id;

            if (os_queue_search(&dsp_mgr_db.session_queue, (void *)indicate_info->session_id))
            {
                session->callback(session->context, DSP_MGR_EVT_DATA_IND, indicate_info->session_id);
            }
            else
            {
                dsp_ipc_data_flush((void *)session->encoder->transport_address);
            }
        }
        break;

    case DSP_IPC_EVT_DATA_EMPTY:
        {
            dsp_mgr_msg_post((T_DSP_MGR_SESSION *)param, DSP_MGR_EVT_DATA_EMPTY, param);
        }
        break;

    case DSP_IPC_EVT_PLC_NOTIFY:
        {
            dsp_mgr_session_plc_notify_handler(DSP_MGR_EVT_PLC_NOTIFY, param);
        }
        break;

    case DSP_IPC_EVT_SIGNAL_REFRESH:
        {
            dsp_mgr_session_signal_refresh_handler(DSP_MGR_EVT_SIGNAL_REFRESH, param);
        }
        break;

    case DSP_IPC_EVT_FIFO_NOTIFY:
        {
            dsp_mgr_fifo_notify_handler(param);
        }
        break;

    case DSP_IPC_EVT_KWS_HIT:
        {
            T_DIPC_KWS_HIT *info;

            info = (T_DIPC_KWS_HIT *)param;

            dsp_mgr_msg_post((T_DSP_MGR_SESSION *)info->session_id,
                             DSP_MGR_EVT_KWS_HIT,
                             (uint32_t)info->keyword_index);
        }
        break;

    case DSP_IPC_EVT_ERROR_REPORT:
        {
            T_DIPC_ERROR_REPORT *error_info;

            error_info = (T_DIPC_ERROR_REPORT *)param;

            if (error_info->error_code == DIPC_ERROR_HARDWARE_FAILURE)
            {
                dsp_mgr_msg_post((T_DSP_MGR_SESSION *)error_info->session_id,
                                 DSP_MGR_EVT_ERROR_REPORT,
                                 error_info->session_id);
            }
        }
        break;

    case DSP_IPC_EVT_VSE_RESPONSE:
        {
            T_DIPC_VSE_RESPONSE    *info;
            T_DSP_MGR_VSE_RESPONSE  vse_response;

            info = (T_DIPC_VSE_RESPONSE *)param;

            vse_response.company_id  = info->company_id;
            vse_response.effect_id   = info->effect_id;
            vse_response.seq_num     = info->seq_num;
            vse_response.payload_len = info->payload_len;
            vse_response.payload     = info->payload;

            dsp_mgr_msg_post((T_DSP_MGR_SESSION *)info->session_id,
                             DSP_MGR_EVT_VSE_RESPONSE,
                             (uint32_t)&vse_response);
        }
        break;

    case DSP_IPC_EVT_VSE_INDICATE:
        {
            T_DIPC_VSE_INDICATE    *info;
            T_DSP_MGR_VSE_INDICATE  vse_indicate;

            info = (T_DIPC_VSE_INDICATE *)param;

            vse_indicate.company_id  = info->company_id;
            vse_indicate.effect_id   = info->effect_id;
            vse_indicate.seq_num     = info->seq_num;
            vse_indicate.payload_len = info->payload_len;
            vse_indicate.payload     = info->payload;

            dsp_mgr_msg_post((T_DSP_MGR_SESSION *)info->session_id,
                             DSP_MGR_EVT_VSE_INDICATE,
                             (uint32_t)&vse_indicate);
        }
        break;

    case DSP_IPC_EVT_VSE_NOTIFY:
        {
            T_DIPC_VSE_NOTIFY    *info;
            T_DSP_MGR_VSE_NOTIFY  vse_notify;

            info = (T_DIPC_VSE_NOTIFY *)param;

            vse_notify.company_id  = info->company_id;
            vse_notify.effect_id   = info->effect_id;
            vse_notify.seq_num     = info->seq_num;
            vse_notify.payload_len = info->payload_len;
            vse_notify.payload     = info->payload;

            dsp_mgr_msg_post((T_DSP_MGR_SESSION *)info->session_id,
                             DSP_MGR_EVT_VSE_NOTIFY,
                             (uint32_t)&vse_notify);
        }
        break;

    default:
        break;
    }

    return true;
}

T_SYS_IPC_HANDLE dsp_mgr_register_cback(P_SYS_IPC_CBACK cback)
{
    if (cback != NULL)
    {
        return sys_ipc_subscribe("dsp_mgr", cback);
    }

    return NULL;
}

void dsp_mgr_unregister_cback(T_SYS_IPC_HANDLE handle)
{
    if (handle != NULL)
    {
        sys_ipc_unsubscribe(handle);
    }
}

static bool dsp_mgr_io_filter_all(T_AUDIO_ROUTE_ENTRY *entry)
{
    return true;
}

static bool dsp_mgr_io_filter_tx(T_AUDIO_ROUTE_ENTRY *entry)
{
    return ((entry->gateway_attr.dir == AUDIO_ROUTE_GATEWAY_DIR_TX) ? true :
            false);
}

static bool dsp_mgr_io_filter_rx(T_AUDIO_ROUTE_ENTRY *entry)
{
    return ((entry->gateway_attr.dir == AUDIO_ROUTE_GATEWAY_DIR_RX) ? true :
            false);
}

void dsp_mgr_loader_cback(T_DSP_LOADER_EVENT event,
                          uint32_t           param)
{
    switch (event)
    {
    case DSP_LOADER_EVENT_FINISH:
        {
            dsp_ipc_init_dsp_sdk(false, (uint8_t)param);
            dsp_mgr_hw_fsm(DSP_MGR_EVENT_LOADING_FINISH);
        }
        break;

    default:
        break;
    }
}

static void dsp_mgr_timer_cback(T_SYS_TIMER_HANDLE handle)
{
    if ((dsp_mgr_is_idle()) && (dsp_mgr_dsp2_ref_get() == 0))
    {
        dsp_mgr_hw_fsm(DSP_MGR_EVENT_POWER_DOWN_REQ);
    }
}

bool dsp_mgr_init(P_DSP_MGR_CBACK cback)
{
    int32_t ret = 0;

    if (dsp_drv_init() == false)
    {
        ret = 1;
        goto fail_init_drv;
    }

    if (dsp_ipc_init() == false)
    {
        ret = 2;
        goto fail_init_ipc;
    }

    if (dsp_loader_init(dsp_mgr_loader_cback) == false)
    {
        ret = 3;
        goto fail_init_loader;
    }

    dsp_mgr_db.dsp_event = dsp_mgr_register_cback(cback);
    if (dsp_mgr_db.dsp_event == NULL)
    {
        ret = 4;
        goto fail_register_mgr_cback;
    }

    if (dsp_param_init() == false)
    {
        ret = 5;
        goto fail_init_dsp_param;
    }

    if (dsp_drv_register_cback(dsp_mgr_drv_cback) == false)
    {
        ret = 6;
        goto fail_register_drv_cback;
    }

    dsp_mgr_db.power_off_timer = sys_timer_create("dsp_power_off",
                                                  SYS_TIMER_TYPE_LOW_PRECISION,
                                                  0,
                                                  TIMER_DSP_POWER_DOWN * 1000,
                                                  false,
                                                  dsp_mgr_timer_cback);
    if (dsp_mgr_db.power_off_timer == NULL)
    {
        ret = 7;
        goto fail_create_timer;
    }

    os_queue_init(&(dsp_mgr_db.session_queue));
    os_queue_init(&(dsp_mgr_db.gateway_queue));
    os_queue_init(&(dsp_mgr_db.activity_queue));

    dsp_ipc_cback_register(dsp_mgr_ipc_event_handler);

    dsp_mgr_db.lps_en = true;
    dsp_mgr_db.state  = DSP_MGR_STATE_OFF;

    return true;

fail_create_timer:
    dsp_drv_unregister_cback(dsp_mgr_drv_cback);
fail_register_drv_cback:
fail_init_dsp_param:
    dsp_mgr_unregister_cback(dsp_mgr_db.dsp_event);
    dsp_mgr_db.dsp_event = NULL;
fail_register_mgr_cback:
    dsp_loader_deinit();
fail_init_loader:
    dsp_ipc_deinit();
fail_init_ipc:
    dsp_drv_deinit();
fail_init_drv:
    DIPC_PRINT_ERROR1("dsp_mgr_init: failed %d", -ret);
    return false;
}

void dsp_mgr_deinit(void)
{
    T_DSP_MGR_SESSION      *session;
    T_DSP_GATEWAY_INSTANCE *gateway;
    T_DSP_MGR_ACTIVITY     *activity;

    dsp_ipc_cback_unregister(dsp_mgr_ipc_event_handler);
    dsp_drv_unregister_cback(dsp_mgr_drv_cback);

    session = os_queue_out(&dsp_mgr_db.session_queue);
    while (session != NULL)
    {
        free(session);
        session = os_queue_out(&dsp_mgr_db.session_queue);
    }

    gateway = os_queue_out(&dsp_mgr_db.gateway_queue);
    while (gateway != NULL)
    {
        free(gateway);
        gateway = os_queue_out(&dsp_mgr_db.gateway_queue);
    }

    activity = os_queue_out(&dsp_mgr_db.activity_queue);
    while (activity != NULL)
    {
        free(activity);
        activity = os_queue_out(&dsp_mgr_db.activity_queue);
    }

    if (dsp_mgr_db.power_off_timer != NULL)
    {
        sys_timer_delete(dsp_mgr_db.power_off_timer);
    }

    dsp_mgr_unregister_cback(dsp_mgr_db.dsp_event);
    dsp_mgr_db.dsp_event = NULL;
    dsp_loader_deinit();
    dsp_ipc_deinit();
    dsp_drv_deinit();
}

void dsp_mgr_lpm_set(bool enable)
{
    if (enable)
    {
        sys_timer_start(dsp_mgr_db.power_off_timer);
    }
    else
    {
        sys_timer_stop(dsp_mgr_db.power_off_timer);
    }

    dsp_mgr_db.lps_en = enable;
}

bool dsp_mgr_is_stable(T_DSP_MGR_SESSION_HANDLE handle)
{
    T_DSP_MGR_STATE state;
    T_DSP_MGR_SESSION *session;
    bool loader_match = true;

    session = (T_DSP_MGR_SESSION *)handle;
    state = dsp_mgr_state_get();

    if (state == DSP_MGR_STATE_READY)
    {
        if (session != NULL)
        {
            session = (T_DSP_MGR_SESSION *)handle;
            loader_match = dsp_loader_bin_match(session->category);
        }
    }

    DIPC_PRINT_TRACE2("dsp_mgr_is_stable: state %d, loader_match %d", state, loader_match);

    if (state == DSP_MGR_STATE_OFF)
    {
        dsp_mgr_power_on();
        return false;
    }

    if ((state == DSP_MGR_STATE_WAIT_OFF) ||
        (state == DSP_MGR_STATE_WAIT_ON) ||
        (state == DSP_MGR_STATE_WAIT_READY) ||
        (loader_match == false))
    {
        return false;
    }

    return true;
}

static bool dsp_mgr_audio_init(T_DSP_MGR_SESSION   *session,
                               T_AUDIO_FORMAT_INFO *format_info)
{
    session->decoder = calloc(1, sizeof(T_CODER_INSTANCE));
    if (session->decoder == NULL)
    {
        goto fail_decoder_alloc;
    }

    session->decoder->frame_num = format_info->frame_num;

    if (dsp_mgr_format_init(format_info->type,
                            session->decoder,
                            &format_info->attr) == false)
    {
        goto fail_decoder_fmt_init;
    }

    return true;

fail_decoder_fmt_init:
    free(session->decoder);
    session->decoder = NULL;
fail_decoder_alloc:
    return false;
}

static bool dsp_mgr_voice_init(T_DSP_MGR_SESSION   *session,
                               T_AUDIO_FORMAT_INFO *decoder_info,
                               T_AUDIO_FORMAT_INFO *encoder_info)
{
    session->decoder = calloc(1, sizeof(T_CODER_INSTANCE));
    if (session->decoder == NULL)
    {
        goto fail_decoder_alloc;
    }

    session->encoder = calloc(1, sizeof(T_CODER_INSTANCE));
    if (session->encoder == NULL)
    {
        goto fail_encoder_alloc;
    }

    session->decoder->frame_num = decoder_info->frame_num;
    session->encoder->frame_num = encoder_info->frame_num;

    if (dsp_mgr_format_init(decoder_info->type,
                            session->decoder,
                            &decoder_info->attr) == false)
    {
        goto fail_decoder_fmt_init;
    }

    if (dsp_mgr_format_init(encoder_info->type,
                            session->encoder,
                            &encoder_info->attr) == false)
    {
        goto fail_encoder_fmt_init;
    }

    return true;

fail_encoder_fmt_init:
fail_decoder_fmt_init:
    free(session->encoder);
    session->encoder = NULL;
fail_encoder_alloc:
    free(session->decoder);
    session->decoder = NULL;
fail_decoder_alloc:
    return false;
}

static bool dsp_mgr_record_init(T_DSP_MGR_SESSION   *session,
                                T_AUDIO_FORMAT_INFO *format_info)
{
    session->encoder = calloc(1, sizeof(T_CODER_INSTANCE));
    if (session->encoder == NULL)
    {
        goto fail_encoder_alloc;
    }

    session->encoder->frame_num = format_info->frame_num;

    if (dsp_mgr_format_init(format_info->type,
                            session->encoder,
                            &format_info->attr) == false)
    {
        goto fail_encoder_fmt_init;
    }

    return true;

fail_encoder_fmt_init:
    free(session->encoder);
    session->encoder = NULL;
fail_encoder_alloc:
    return false;
}

static bool dsp_mgr_vad_init(T_DSP_MGR_SESSION   *session,
                             T_AUDIO_FORMAT_INFO *format_info)
{
    session->encoder = calloc(1, sizeof(T_CODER_INSTANCE));
    if (session->encoder == NULL)
    {
        goto fail_encoder_alloc;
    }

    session->encoder->frame_num = format_info->frame_num;

    if (dsp_mgr_format_init(format_info->type,
                            session->encoder,
                            &format_info->attr) == false)
    {
        goto fail_encoder_fmt_init;
    }

    return true;

fail_encoder_fmt_init:
    free(session->encoder);
    session->encoder = NULL;
fail_encoder_alloc:
    return false;
}

static bool dsp_mgr_voice_prompt_init(T_DSP_MGR_SESSION   *session,
                                      T_AUDIO_FORMAT_INFO *format_info)
{
    session->decoder = calloc(1, sizeof(T_CODER_INSTANCE));
    if (session->decoder == NULL)
    {
        goto fail_decoder_alloc;
    }

    session->decoder->frame_num = format_info->frame_num;

    if (dsp_mgr_format_init(format_info->type,
                            session->decoder,
                            &format_info->attr) == false)
    {
        goto fail_decoder_fmt_init;
    }

    return true;

fail_decoder_fmt_init:
    free(session->decoder);
    session->decoder = NULL;
fail_decoder_alloc:
    return false;
}

static bool dsp_mgr_pipe_init(T_DSP_MGR_SESSION   *session,
                              T_AUDIO_FORMAT_INFO *decoder_info,
                              T_AUDIO_FORMAT_INFO *encoder_info)
{
    session->decoder = calloc(1, sizeof(T_CODER_INSTANCE));
    if (session->decoder == NULL)
    {
        goto fail_alloc_decoder;
    }

    session->encoder = calloc(1, sizeof(T_CODER_INSTANCE));
    if (session->encoder == NULL)
    {
        goto fail_alloc_encoder;
    }

    session->decoder->frame_num = decoder_info->frame_num;
    session->encoder->frame_num = encoder_info->frame_num;

    if (dsp_mgr_format_init(decoder_info->type,
                            session->decoder,
                            &decoder_info->attr) == false)
    {
        goto fail_decoder_fmt_init;
    }

    if (dsp_mgr_format_init(encoder_info->type,
                            session->encoder,
                            &encoder_info->attr) == false)
    {
        goto fail_encoder_fmt_init;
    }

    return true;

fail_encoder_fmt_init:
fail_decoder_fmt_init:
    free(session->encoder);
    session->encoder = NULL;
fail_alloc_encoder:
    free(session->decoder);
    session->decoder = NULL;
fail_alloc_decoder:
    return false;
}

static bool dsp_mgr_ringtone_init(T_DSP_MGR_SESSION *session,
                                  void              *pconfig)
{
    T_TONE_COEF *coef;

    coef = (T_TONE_COEF *)pconfig;

    session->coef.size = coef->size;

    session->coef.coefficient = malloc(session->coef.size);
    if (session->coef.coefficient == NULL)
    {
        goto fail_coeff_alloc;
    }

    memcpy(session->coef.coefficient, coef->coefficient, session->coef.size);

    return true;

fail_coeff_alloc:
    return false;
}

static bool dsp_mgr_kws_init(T_DSP_MGR_SESSION   *session,
                             T_AUDIO_FORMAT_INFO *format_info)
{
    session->decoder = calloc(1, sizeof(T_CODER_INSTANCE));
    if (session->decoder == NULL)
    {
        goto fail_decoder_alloc;
    }

    session->decoder->frame_num = format_info->frame_num;

    if (dsp_mgr_format_init(format_info->type,
                            session->decoder,
                            &format_info->attr) == false)
    {
        goto fail_decoder_fmt_init;
    }

    return true;

fail_decoder_fmt_init:
    free(session->decoder);
fail_decoder_alloc:
    return false;
}

T_DSP_MGR_SESSION_HANDLE dsp_mgr_session_create(T_AUDIO_CATEGORY          category,
                                                uint8_t                   data_mode,
                                                uint32_t                  dac_sample_rate,
                                                uint32_t                  adc_sample_rate,
                                                T_AUDIO_FORMAT_INFO      *decoder_info,
                                                T_AUDIO_FORMAT_INFO      *encoder_info,
                                                P_DSP_MGR_SESSION_CBACK   callback,
                                                uint32_t                  device,
                                                T_VAD_TYPE                vad_type,
                                                uint8_t                   aggressiveness_level,
                                                void                     *context)
{
    T_DSP_MGR_SESSION *session;
    int32_t            ret = 0;

    session = calloc(1, sizeof(T_DSP_MGR_SESSION));
    if (session == NULL)
    {
        ret = 1;
        goto fail_alloc_item;
    }

    session->context               = context;
    session->callback              = callback;
    session->category              = category;
    session->dac_sample_rate       = dac_sample_rate;
    session->adc_sample_rate       = adc_sample_rate;
    session->state                 = DSP_MGR_SESSION_STATE_DISABLED;
    session->data_mode             = data_mode;
    session->device                = device;
    session->aggressiveness_level  = aggressiveness_level;

    switch (category)
    {
    case AUDIO_CATEGORY_AUDIO:
        {
            if (!dsp_mgr_audio_init(session, decoder_info))
            {
                ret = 2;
                goto fail_init_item;
            }
        }
        break;

    case AUDIO_CATEGORY_VOICE:
        {
            if (!dsp_mgr_voice_init(session, decoder_info, encoder_info))
            {
                ret = 3;
                goto fail_init_item;
            }
        }
        break;

    case AUDIO_CATEGORY_RECORD:
        {
            if (!dsp_mgr_record_init(session, encoder_info))
            {
                ret = 4;
                goto fail_init_item;
            }
        }
        break;

    case AUDIO_CATEGORY_LINE:
        break;

    case AUDIO_CATEGORY_TONE:
        {
            if (!dsp_mgr_ringtone_init(session, decoder_info))
            {
                ret = 5;
                goto fail_init_item;
            }
        }
        break;

    case AUDIO_CATEGORY_VP:
        {
            if (!dsp_mgr_voice_prompt_init(session, decoder_info))
            {
                ret = 6;
                goto fail_init_item;
            }
        }
        break;

    case AUDIO_CATEGORY_APT:
        break;

    case AUDIO_CATEGORY_PIPE:
        {
            if (!dsp_mgr_pipe_init(session, decoder_info, encoder_info))
            {
                ret = 7;
                goto fail_init_item;
            }
        }
        break;

    case AUDIO_CATEGORY_VAD:
        {
            if ((vad_type == VAD_TYPE_SW) ||
                (vad_type == VAD_TYPE_FUSION))
            {
                if (!dsp_mgr_vad_init(session, encoder_info))
                {
                    ret = 8;
                    goto fail_init_item;
                }
            }
            else
            {
                /* TBD */
                /* HW VAD setting */

            }
        }
        break;

    case AUDIO_CATEGORY_KWS:
        {
            if (!dsp_mgr_kws_init(session, decoder_info))
            {
                ret = 8;
                goto fail_init_item;
            }
        }
        break;

    default:
        break;
    }

    os_queue_in(&(dsp_mgr_db.session_queue), session);

    return (T_DSP_MGR_SESSION_HANDLE)session;

fail_init_item:
    free(session);
fail_alloc_item:
    DIPC_PRINT_ERROR1("dsp_mgr_session_create: failed %d", -ret);
    return NULL;
}

bool dsp_mgr_session_destroy(T_DSP_MGR_SESSION_HANDLE handle)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;

    if (session->decoder != NULL)
    {
        free(session->decoder);
    }

    if (session->encoder != NULL)
    {
        free(session->encoder);
    }

    if (session->category == AUDIO_CATEGORY_TONE)
    {
        if (session->coef.coefficient != NULL)
        {
            free(session->coef.coefficient);
        }
    }

    os_queue_delete(&(dsp_mgr_db.session_queue), session);
    free(session);

    return true;
}

bool dsp_mgr_power_on_check(void)
{
    return (dsp_drv_boot_ref_cnt_get() != 0) ? true : false;
}

static bool dsp_mgr_activity_filter(T_DSP_MGR_SESSION       *session,
                                    T_DSP_MGR_SESSION_EVENT  event)
{
    return false;
}

static bool dsp_mgr_suspend_check(T_DSP_MGR_SESSION *initiator_session)
{
    T_DSP_MGR_SESSION *session;

    session = os_queue_peek(&dsp_mgr_db.session_queue, 0);
    while (session != NULL)
    {
        if ((session->state != DSP_MGR_SESSION_STATE_DISABLED) &&
            (session != initiator_session))
        {
            dsp_mgr_db.suspending_session = initiator_session;
            return false;
        }

        session = session->p_next;
    }

    return true;
}

static void dsp_mgr_activity_schedule(void)
{
    T_DSP_MGR_ACTIVITY *activity;

    if (dsp_mgr_db.working_activity == NULL)
    {
        activity = os_queue_out(&(dsp_mgr_db.activity_queue));
        if (activity != NULL)
        {
            dsp_mgr_db.working_activity = activity;
            dsp_mgr_session_fsm(activity->session, activity->event);
        }
    }
    else
    {
        if (dsp_mgr_db.suspending_session != NULL)
        {
            if (dsp_mgr_suspend_check(dsp_mgr_db.suspending_session))
            {
                dsp_mgr_hw_fsm(DSP_MGR_EVENT_SUSPEND_REQ);
            }
        }
    }
}

static bool dsp_mgr_activity_add(T_DSP_MGR_SESSION *session, T_DSP_MGR_SESSION_EVENT event)
{
    T_DSP_MGR_ACTIVITY *activity;

    DIPC_PRINT_TRACE2("dsp_mgr_activity_add: session %p, event %d", session, event);

    if (dsp_mgr_activity_filter(session, event) == false)
    {
        activity = malloc(sizeof(T_DSP_MGR_ACTIVITY));
        if (activity != NULL)
        {
            activity->session = session;
            activity->event = event;
            os_queue_in(&(dsp_mgr_db.activity_queue), activity);

            dsp_mgr_activity_schedule();
            return true;
        }
    }

    return false;
}

static void dsp_mgr_activity_remove(void)
{
    if (dsp_mgr_db.working_activity != NULL)
    {
        free(dsp_mgr_db.working_activity);

        dsp_mgr_db.working_activity = NULL;
    }

    dsp_mgr_activity_schedule();
}

bool dsp_mgr_session_enable(T_DSP_MGR_SESSION_HANDLE handle)
{
    T_DSP_MGR_SESSION *session;

    DIPC_PRINT_INFO1("dsp_mgr_session_enable: handle %p", handle);

    if (dsp_mgr_handle_check(handle))
    {
        session = (T_DSP_MGR_SESSION *)handle;
        return dsp_mgr_activity_add(session, DSP_MGR_SESSION_EVENT_ENABLE_REQ);
    }

    return false;
}

bool dsp_mgr_session_disable(T_DSP_MGR_SESSION_HANDLE handle)
{
    T_DSP_MGR_SESSION *session;

    DIPC_PRINT_INFO1("dsp_mgr_session_disable: handle %p", handle);

    if (dsp_mgr_handle_check(handle))
    {
        session = (T_DSP_MGR_SESSION *)handle;
        return dsp_mgr_activity_add(session, DSP_MGR_SESSION_EVENT_DISABLE_REQ);
    }

    return false;
}

bool dsp_mgr_session_suspend(T_DSP_MGR_SESSION_HANDLE handle)
{
    T_DSP_MGR_SESSION *session;

    DIPC_PRINT_INFO1("dsp_mgr_session_suspend: handle %p", handle);

    session = (T_DSP_MGR_SESSION *)handle;
    if (os_queue_search(&dsp_mgr_db.session_queue, session))
    {
        return dsp_mgr_activity_add(session, DSP_MGR_SESSION_EVENT_SUSPEND_REQ);
    }

    return false;
}

bool dsp_mgr_session_resume(T_DSP_MGR_SESSION_HANDLE handle)
{
    T_DSP_MGR_SESSION *session;

    DIPC_PRINT_INFO1("dsp_mgr_session_resume: handle %p", handle);

    session = (T_DSP_MGR_SESSION *)handle;
    if (os_queue_search(&dsp_mgr_db.session_queue, session))
    {
        return dsp_mgr_activity_add(session, DSP_MGR_SESSION_EVENT_RESUME_REQ);
    }

    return false;
}

static void dsp_mgr_session_stop(T_DSP_MGR_SESSION *session)
{
    switch (session->category)
    {
    case AUDIO_CATEGORY_AUDIO:
        {
            dipc_decoder_stop((uint32_t)session, DIPC_FADE_ENABLE);
        }
        break;

    case AUDIO_CATEGORY_VOICE:
        {
            dipc_decoder_stop((uint32_t)session, DIPC_FADE_ENABLE);
            dipc_encoder_stop((uint32_t)session, DIPC_FADE_DISABLE);
        }
        break;

    case AUDIO_CATEGORY_RECORD:
        {
            dipc_encoder_stop((uint32_t)session, DIPC_FADE_ENABLE);
        }
        break;

    case AUDIO_CATEGORY_VP:
        {
            dipc_voice_prompt_stop((uint32_t)session, DIPC_FADE_ENABLE);
        }
        break;

    case AUDIO_CATEGORY_TONE:
        {
            dipc_ringtone_stop((uint32_t)session, DIPC_FADE_ENABLE);
        }
        break;

    case AUDIO_CATEGORY_APT:
        {
            dipc_apt_stop((uint32_t)session, DIPC_FADE_DISABLE);
        }
        break;

    case AUDIO_CATEGORY_LINE:
        {
            dipc_line_stop((uint32_t)session, DIPC_FADE_ENABLE);
        }
        break;

    case AUDIO_CATEGORY_PIPE:
        {
            dipc_codec_pipe_stop((uint32_t)session);
        }
        break;

    case AUDIO_CATEGORY_VAD:
        {
            dipc_vad_stop((uint32_t)session);
        }
        break;

    case AUDIO_CATEGORY_KWS:
        {
            dipc_kws_stop((uint32_t)session);
        }
        break;

    default:
        break;
    }
}

void dsp_mgr_power_on(void)
{
    dsp_mgr_hw_fsm(DSP_MGR_EVENT_POWER_ON_REQ);
}

void dsp_mgr_power_off(void)
{
    dsp_mgr_db.flags |= DSP_MGR_WAIT_POWER_OFF;
    sys_timer_stop(dsp_mgr_db.power_off_timer);

    if ((dsp_mgr_is_idle()) && (dsp_mgr_dsp2_ref_get() == 0))
    {
        dsp_mgr_hw_fsm(DSP_MGR_EVENT_POWER_DOWN_REQ);
    }
}

static uint16_t dsp_mgr_logic_io_table_take(T_DSP_MGR_SESSION       *session,
                                            uint8_t                **tbl,
                                            P_DSP_MGR_FILTER_CBACK   filter)
{
    T_AUDIO_ROUTE_PATH          path;
    T_DIPC_LOGICAL_IO_TBL_HEAD *tbl_head;
    T_DIPC_LOGICAL_IO_ENTRY    *tbl_entries;
    uint16_t                    tbl_size = 0;
    uint8_t                     tlv_cnt = 0;
    uint8_t                    *p = NULL;
    uint8_t                     i;

    path = audio_route_path_get(session->context,
                                session->category,
                                session->device);

    if (path.entry_num == 0)
    {
        goto func_end;
    }

    tbl_size = sizeof(T_DIPC_LOGICAL_IO_ENTRY) * path.entry_num +
               sizeof(T_DIPC_LOGICAL_IO_TBL_HEAD);

    p = calloc(1, tbl_size);

    if (p == NULL)
    {
        goto func_end;
    }

    tbl_entries = (T_DIPC_LOGICAL_IO_ENTRY *)(p + sizeof(T_DIPC_LOGICAL_IO_TBL_HEAD));

    for (i = 0; i < path.entry_num; ++i)
    {
        if (filter(&path.entry[i]))
        {
            tbl_entries[tlv_cnt].logical_io       = path.entry[i].io_type;
            tbl_entries[tlv_cnt].endpoint         = path.entry[i].endpoint_type;
            tbl_entries[tlv_cnt].gateway_type     = path.entry[i].gateway_attr.type;
            tbl_entries[tlv_cnt].gateway_id       = path.entry[i].gateway_attr.idx;
            tbl_entries[tlv_cnt].gateway_channels = path.entry[i].gateway_attr.channs;
            tbl_entries[tlv_cnt].gateway_polarity = path.entry[i].io_polarity;

            tlv_cnt++;
        }
    }

    tbl_head = (T_DIPC_LOGICAL_IO_TBL_HEAD *)p;

    tbl_head->version     = 0x02;
    tbl_head->entry_count = tlv_cnt;
    tbl_head->reserved    = 0;

    audio_route_path_give(&path);

    *tbl = p;
    tbl_size = sizeof(T_DIPC_LOGICAL_IO_ENTRY) * tlv_cnt + sizeof(T_DIPC_LOGICAL_IO_TBL_HEAD);

    DIPC_PRINT_TRACE1("dsp_mgr_logic_io_table_take: io_tbl %b", TRACE_BINARY(tbl_size, *tbl));

    return tbl_size;

func_end:
    return 0;
}

static void dsp_mgr_logic_io_table_give(uint8_t *tbl)
{
    free(tbl);
}

static void dsp_mgr_session_decoder_create(T_DSP_MGR_SESSION *session)
{
    uint8_t *tbl;
    uint8_t *coder_format;
    uint16_t tbl_size;
    uint16_t format_size;
    uint8_t coder_id;

    if (session->decoder == NULL)
    {
        goto fail_check_session;
    }

    tbl_size = dsp_mgr_logic_io_table_take(session,
                                           &tbl,
                                           dsp_mgr_io_filter_tx);

    if (tbl_size != 0)
    {
        format_size = session->decoder->format_size;

        if (format_size == 0)
        {
            goto func_end;
        }

        coder_format = (uint8_t *)(&(session->decoder->format));
        coder_id = session->decoder->coder_id;

        dipc_decoder_create((uint32_t)session, session->category, session->data_mode,
                            session->decoder->frame_num, coder_id, format_size,
                            coder_format, tbl_size, tbl);

        dsp_mgr_logic_io_table_give(tbl);
        return ;
    }

func_end:
    dsp_mgr_logic_io_table_give(tbl);
fail_check_session:
    return ;
}

static void dsp_mgr_session_encoder_create(T_DSP_MGR_SESSION *session)
{
    uint8_t *tbl;
    uint8_t *coder_format;
    uint16_t tbl_size;
    uint16_t format_size;
    uint8_t coder_id;

    if (session->encoder == NULL)
    {
        goto fail_check_session;
    }

    tbl_size = dsp_mgr_logic_io_table_take(session,
                                           &tbl,
                                           dsp_mgr_io_filter_rx);

    if (tbl_size != 0)
    {
        format_size = session->encoder->format_size;

        if (format_size == 0)
        {
            goto func_end;
        }

        coder_format = (uint8_t *)(&(session->encoder->format));
        coder_id = session->encoder->coder_id;

        dipc_encoder_create((uint32_t)session, session->category, session->data_mode,
                            session->encoder->frame_num, coder_id, format_size,
                            coder_format, tbl_size, tbl);

        dsp_mgr_logic_io_table_give(tbl);
        return ;
    }

func_end:
    dsp_mgr_logic_io_table_give(tbl);
fail_check_session:
    return ;
}

static void dsp_mgr_line_create(T_DSP_MGR_SESSION *session)
{
    uint8_t *tbl;
    uint16_t tbl_size;

    tbl_size = dsp_mgr_logic_io_table_take(session,
                                           &tbl,
                                           dsp_mgr_io_filter_all);

    if (tbl_size == 0)
    {
        goto func_end;
    }

    dipc_line_create((uint32_t)session, tbl_size, tbl);

    dsp_mgr_logic_io_table_give(tbl);

func_end:
    return ;
}

static void dsp_mgr_voice_prompt_create(T_DSP_MGR_SESSION *session)
{
    uint8_t          *table;
    uint16_t          table_size;
    T_CODER_INSTANCE *decoder;

    table_size = dsp_mgr_logic_io_table_take(session,
                                             &table,
                                             dsp_mgr_io_filter_tx);

    if (table_size == 0)
    {
        goto func_end;
    }

    decoder = (T_CODER_INSTANCE *)session->decoder;

    dipc_voice_prompt_create((uint32_t)session,
                             decoder->coder_id,
                             decoder->frame_num,
                             decoder->format_size,
                             (uint8_t *)(&decoder->format),
                             table_size,
                             table);

    dsp_mgr_logic_io_table_give(table);

func_end:
    return ;
}

static void dsp_mgr_ringtone_create(T_DSP_MGR_SESSION *session)
{
    uint8_t *tbl;
    uint16_t tbl_size;

    tbl_size = dsp_mgr_logic_io_table_take(session,
                                           &tbl,
                                           dsp_mgr_io_filter_tx);

    if (tbl_size == 0)
    {
        goto func_end;
    }

    dipc_ringtone_create((uint32_t)session,
                         tbl_size,
                         tbl,
                         session->coef.size,
                         session->coef.coefficient);

    dsp_mgr_logic_io_table_give(tbl);

func_end:
    return ;
}

static void dsp_mgr_vad_create(T_DSP_MGR_SESSION *session)
{
    uint8_t  *table;
    uint16_t  table_size;

    table_size = dsp_mgr_logic_io_table_take(session,
                                             &table,
                                             dsp_mgr_io_filter_all);
    if (table_size != 0)
    {
        T_CODER_INSTANCE *encoder = session->encoder;

        dipc_vad_create((uint32_t)session,
                        session->aggressiveness_level,
                        encoder->coder_id,
                        encoder->format_size,
                        (uint8_t *)(&encoder->format),
                        session->data_mode,
                        encoder->frame_num,
                        table_size,
                        table);

        dsp_mgr_logic_io_table_give(table);
    }
}

static void dsp_mgr_kws_create(T_DSP_MGR_SESSION *session)
{
    T_CODER_INSTANCE *decoder;

    decoder = session->decoder;

    dipc_kws_create((uint32_t)session,
                    decoder->coder_id,
                    decoder->frame_num,
                    decoder->format_size,
                    (uint8_t *)(&decoder->format));
}

static void dsp_mgr_apt_create(T_DSP_MGR_SESSION *session)
{
    uint8_t *tbl;
    uint16_t tbl_size;

    tbl_size = dsp_mgr_logic_io_table_take(session,
                                           &tbl,
                                           dsp_mgr_io_filter_all);

    if (tbl_size == 0)
    {
        goto func_end;
    }

    dipc_apt_create((uint32_t)session, tbl_size, tbl);

    dsp_mgr_logic_io_table_give(tbl);

func_end:
    return ;
}

static void dsp_mgr_gateway_set(T_DSP_MGR_SESSION          *session,
                                T_AUDIO_ROUTE_GATEWAY_ATTR *attr)
{
    uint8_t                 fifo_mode;
    uint32_t                sample_rate;
    T_DSP_GATEWAY_INSTANCE *instance;

    instance = dsp_mgr_gateway_instance_get(attr->type, attr->idx);
    if (instance == NULL)
    {
        instance = calloc(1, sizeof(T_DSP_GATEWAY_INSTANCE));
        if (instance != NULL)
        {
            instance->type = attr->type;
            instance->id   = attr->idx;
            os_queue_in(&(dsp_mgr_db.gateway_queue), instance);
        }
    }

    if (instance != NULL)
    {
        if (attr->dir == AUDIO_ROUTE_GATEWAY_DIR_TX)
        {
            if (instance->tx_refcnt == 0)
            {
                fifo_mode = (attr->fifo_mode == AUDIO_ROUTE_GATEWAY_FIFO_MODE_INTERLACED) ?
                            DIPC_CH_MODE_INTERLACED : DIPC_CH_MODE_NON_INTERLACED;

                if (attr->sample_rate)
                {
                    sample_rate = attr->sample_rate;
                }
                else
                {
                    sample_rate = session->dac_sample_rate;
                }

                dipc_gateway_set(attr->type,
                                 attr->idx,
                                 attr->dir,
                                 attr->role,
                                 attr->bridge,
                                 sample_rate,
                                 fifo_mode,
                                 attr->chann_mode,
                                 attr->format,
                                 attr->chann_len,
                                 attr->data_len);
            }

            instance->tx_refcnt++;
        }

        if (attr->dir == AUDIO_ROUTE_GATEWAY_DIR_RX)
        {
            if (instance->rx_refcnt == 0)
            {
                fifo_mode = (attr->fifo_mode == AUDIO_ROUTE_GATEWAY_FIFO_MODE_INTERLACED) ?
                            DIPC_CH_MODE_INTERLACED : DIPC_CH_MODE_NON_INTERLACED;

                if (attr->sample_rate)
                {
                    sample_rate = attr->sample_rate;
                }
                else
                {
                    sample_rate = session->adc_sample_rate;
                }

                dipc_gateway_set(attr->type,
                                 attr->idx,
                                 attr->dir,
                                 attr->role,
                                 attr->bridge,
                                 sample_rate,
                                 fifo_mode,
                                 attr->chann_mode,
                                 attr->format,
                                 attr->chann_len,
                                 attr->data_len);
            }

            instance->rx_refcnt++;
        }

        DIPC_PRINT_TRACE4("dsp_mgr_gateway_set: type %d, id %d, rx_refcnt %d, tx_refcnt %d",
                          instance->type, instance->id, instance->rx_refcnt, instance->tx_refcnt);
    }
}

void dsp_mgr_session_run(T_DSP_MGR_SESSION_HANDLE handle)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;

    dsp_mgr_deployment_traverse(session, dsp_mgr_gateway_set);

    switch (session->category)
    {
    case AUDIO_CATEGORY_AUDIO:
        {
            if (session->data_mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
            {
                dsp_ipc_set_fifo_len(DSP_FIFO_TX_DIRECTION, true);
                dsp_ipc_set_decode_emtpy(false);
            }
            dsp_mgr_session_decoder_create(session);
        }
        break;

    case AUDIO_CATEGORY_VOICE:
        {
            dsp_mgr_session_decoder_create(session);
            dsp_mgr_session_encoder_create(session);
        }
        break;

    case AUDIO_CATEGORY_RECORD:
        {
            dsp_mgr_session_encoder_create(session);
        }
        break;

    case AUDIO_CATEGORY_LINE:
        {
            dsp_mgr_line_create(session);
        }
        break;

    case AUDIO_CATEGORY_TONE:
        {
            dsp_mgr_ringtone_create(session);
        }
        break;

    case AUDIO_CATEGORY_VP:
        {
            dsp_mgr_voice_prompt_create(session);
        }
        break;

    case AUDIO_CATEGORY_APT:
        {
            dsp_mgr_apt_create(session);
        }
        break;

    case AUDIO_CATEGORY_PIPE:
        {
            dipc_codec_pipe_create((uint32_t)session,
                                   session->data_mode,
                                   session->decoder->coder_id,
                                   session->decoder->frame_num,
                                   session->decoder->format_size,
                                   (uint8_t *)(&(session->decoder->format)),
                                   session->encoder->coder_id,
                                   session->encoder->frame_num,
                                   session->encoder->format_size,
                                   (uint8_t *)(&(session->encoder->format)));
        }
        break;

    case AUDIO_CATEGORY_VAD:
        {
            dsp_mgr_vad_create(session);
        }
        break;

    case AUDIO_CATEGORY_KWS:
        {
            dsp_mgr_kws_create(session);
        }
        break;

    default:
        break;
    }
}

static void dsp_mgr_session_fsm(T_DSP_MGR_SESSION *session, T_DSP_MGR_SESSION_EVENT event)
{
    DIPC_PRINT_INFO2("dsp_mgr_session_fsm: session %p, event %d", session, event);

    switch (session->state)
    {
    case DSP_MGR_SESSION_STATE_DISABLED:
        {
            if (event == DSP_MGR_SESSION_EVENT_ENABLE_REQ)
            {
                session->state = DSP_MGR_SESSION_STATE_ENABLING;
                dsp_mgr_hw_fsm(DSP_MGR_EVENT_START);
            }
        }
        break;

    case DSP_MGR_SESSION_STATE_ENABLING:
        {
            if ((event == DSP_MGR_SESSION_EVENT_ENABLE_RSP) ||
                (event == DSP_MGR_SESSION_EVENT_RESUME_RSP))
            {
                if (session->category == AUDIO_CATEGORY_VOICE)
                {
                    if ((session->flags & DSP_MGR_SESSION_DECODER_STARTED) &&
                        (session->flags & DSP_MGR_SESSION_ENCODER_STARTED))
                    {
                        session->state = DSP_MGR_SESSION_STATE_ENABLED;
                        session->callback(session->context, DSP_MGR_EVT_SESSION_STARTED, 0);
                    }
                }
                else
                {
                    session->state = DSP_MGR_SESSION_STATE_ENABLED;
                    session->callback(session->context, DSP_MGR_EVT_SESSION_STARTED, 0);
                }

                dsp_mgr_activity_remove();
            }
        }
        break;

    case DSP_MGR_SESSION_STATE_ENABLED:
        {
            if (event == DSP_MGR_SESSION_EVENT_DISABLE_REQ)
            {
                session->state = DSP_MGR_SESSION_STATE_DISABLING;
                dsp_mgr_hw_fsm(DSP_MGR_EVENT_STOP);
            }
            else if (event == DSP_MGR_SESSION_EVENT_SUSPEND_REQ)
            {
                session->state = DSP_MGR_SESSION_STATE_SUSPENDING;

                if (dsp_mgr_suspend_check(session))
                {
                    dsp_mgr_hw_fsm(DSP_MGR_EVENT_SUSPEND_REQ);
                }
                else
                {
                    dsp_mgr_session_fsm(session, DSP_MGR_SESSION_EVENT_SUSPEND_RSP);
                }
            }
        }
        break;

    case DSP_MGR_SESSION_STATE_SUSPENDING:
        {
            if (event == DSP_MGR_SESSION_EVENT_SUSPEND_RSP)
            {
                session->state = DSP_MGR_SESSION_STATE_SUSPENDED;
                dsp_mgr_activity_remove();
            }
        }
        break;

    case DSP_MGR_SESSION_STATE_SUSPENDED:
        {
            if (event == DSP_MGR_SESSION_EVENT_DISABLE_REQ)
            {
                session->state = DSP_MGR_SESSION_STATE_DISABLING;
                if (dsp_mgr_db.suspending_session != NULL)
                {
                    dsp_mgr_db.suspending_session = NULL;
                    dsp_mgr_session_fsm(session, DSP_MGR_SESSION_EVENT_RESUME_RSP);
                }
                else
                {
                    dsp_mgr_hw_fsm(DSP_MGR_EVENT_RESUME_REQ);
                }
            }
            else if (event == DSP_MGR_SESSION_EVENT_RESUME_REQ)
            {
                session->state = DSP_MGR_SESSION_STATE_ENABLING;
                if (dsp_mgr_db.suspending_session != NULL)
                {
                    dsp_mgr_db.suspending_session = NULL;
                    dsp_mgr_session_fsm(session, DSP_MGR_SESSION_EVENT_RESUME_RSP);
                }
                else
                {
                    dsp_mgr_hw_fsm(DSP_MGR_EVENT_RESUME_REQ);
                }
            }
        }
        break;

    case DSP_MGR_SESSION_STATE_DISABLING:
        {
            if (event == DSP_MGR_SESSION_EVENT_DISABLE_RSP)
            {
                if (session->category == AUDIO_CATEGORY_VOICE)
                {
                    if (((session->flags & DSP_MGR_SESSION_DECODER_STARTED) == 0) &&
                        ((session->flags & DSP_MGR_SESSION_ENCODER_STARTED) == 0))
                    {
                        session->state = DSP_MGR_SESSION_STATE_DISABLED;
                        session->callback(session->context, DSP_MGR_EVT_SESSION_STOPPED, 0);
                        dsp_mgr_gateway_clear_check(session);
                    }
                }
                else
                {
                    session->state = DSP_MGR_SESSION_STATE_DISABLED;
                    session->callback(session->context, DSP_MGR_EVT_SESSION_STOPPED, 0);
                    dsp_mgr_gateway_clear_check(session);
                }

                dsp_mgr_hw_fsm(DSP_MGR_EVENT_STOP_RSP);
                dsp_mgr_activity_remove();
            }
            else if (event == DSP_MGR_EVENT_RESUME_RSP)
            {
                dsp_mgr_session_stop(session);
            }
        }
        break;
    }
}

static void dsp_mgr_power_off_apply(void)
{
    sys_timer_stop(dsp_mgr_db.power_off_timer);

    if ((dsp_mgr_db.flags & DSP_MGR_WAIT_POWER_OFF) != 0)
    {
        if ((dsp_mgr_is_idle()) && (dsp_mgr_dsp2_ref_get() == 0))
        {
            dsp_mgr_hw_fsm(DSP_MGR_EVENT_POWER_DOWN_REQ);
        }
    }
    else
    {
        if (dsp_mgr_db.lps_en == true)
        {
            if (dsp_mgr_db.state != DSP_MGR_STATE_OFF)
            {
                sys_timer_start(dsp_mgr_db.power_off_timer);
            }
        }
    }
}

bool dsp_mgr_channel_out_reorder(T_DSP_MGR_SESSION_HANDLE handle,
                                 uint8_t                  channel_num,
                                 uint32_t                 channel_array[])
{
    T_DSP_MGR_SESSION *session;

    session = (T_DSP_MGR_SESSION *)handle;
    if (os_queue_search(&dsp_mgr_db.session_queue, session))
    {
        return dipc_channel_out_reorder((uint32_t)session,
                                        channel_num,
                                        channel_array);
    }

    return false;
}

bool dsp_mgr_channel_in_reorder(T_DSP_MGR_SESSION_HANDLE handle,
                                uint8_t                  channel_num,
                                uint32_t                 channel_array[])
{
    T_DSP_MGR_SESSION *session;

    session = (T_DSP_MGR_SESSION *)handle;
    if (os_queue_search(&dsp_mgr_db.session_queue, session))
    {
        return dipc_channel_in_reorder((uint32_t)session,
                                       channel_num,
                                       channel_array);
    }

    return false;
}

static bool dsp_mgr_is_running(void)
{
    T_DSP_MGR_SESSION *session;

    session = os_queue_peek(&dsp_mgr_db.session_queue, 0);
    while (session != NULL)
    {
        if (session->state == DSP_MGR_SESSION_STATE_ENABLED)
        {
            return true;
        }
        session = session->p_next;
    }

    return false;
}

static void dsp_mgr_hw_fsm(T_DSP_MGR_DSP_EVENT event)
{
    T_DSP_MGR_STATE state;

    state = dsp_mgr_state_get();

    DIPC_PRINT_INFO2("dsp_mgr_hw_fsm: state %d, event %d", state, event);

    switch (state)
    {
    case DSP_MGR_STATE_OFF:
        {
            if (event == DSP_MGR_EVENT_POWER_ON_REQ)
            {
                dsp_mgr_state_set(DSP_MGR_STATE_WAIT_ON);
                dsp_drv_boot();
                dsp_loader_run();
            }
            else if (event == DSP_MGR_EVENT_POWER_DOWN_REQ)
            {
                dsp_mgr_db.flags &= (~DSP_MGR_WAIT_POWER_OFF);
                sys_ipc_publish("dsp_mgr", DSP_MGR_EVT_POWER_OFF, NULL);
            }
        }
        break;

    case DSP_MGR_STATE_WAIT_ON:
        {
            if ((event == DSP_MGR_EVENT_POWER_ON_RSP) ||
                (event == DSP_MGR_EVENT_RESUME_RSP))
            {
                dsp_mgr_state_set(DSP_MGR_STATE_ON);
                dsp_ipc_set_log_output_sel(audio_route_cfg.dsp_log_output_select);
                dsp_ipc_set_dsp2_log_output_sel(audio_route_cfg.dsp2_log_output_select);
                dsp_ipc_set_dsp1_uart(audio_route_cfg.dsp_uart_enable);
                dsp_ipc_set_dsp2_uart(audio_route_cfg.dsp_uart_enable);
                sys_ipc_publish("dsp_mgr", DSP_MGR_EVT_POWER_ON, NULL);
            }
            else if (event == DSP_MGR_EVENT_LOADING_FINISH)
            {
                dsp_drv_reset();
            }
            else if (event == DSP_MGR_EVENT_BOOT)
            {
                dsp_drv_power_on();
                dipc_power_control(DIPC_ENTITY_1, DIPC_POWER_CONTROL_MODE_ON);
            }
        }
        break;

    case DSP_MGR_STATE_ON:
        {
            if (event == DSP_MGR_EVENT_START)
            {
                dsp_mgr_state_set(DSP_MGR_STATE_WAIT_READY);

                if (dsp_mgr_db.working_activity != NULL)
                {
                    if (dsp_loader_bin_load(dsp_mgr_db.working_activity->session->category) == true)
                    {
                        break;
                    }
                }
            }
            else if (event == DSP_MGR_EVENT_STOP)
            {
                if (dsp_mgr_db.working_activity != NULL)
                {
                    dsp_mgr_session_stop(dsp_mgr_db.working_activity->session);
                }

                dsp_mgr_power_off_apply();
            }
            else if (event == DSP_MGR_EVENT_POWER_DOWN_REQ)
            {
                dsp_mgr_state_set(DSP_MGR_STATE_WAIT_OFF);
                dsp_ipc_set_log_output_sel(DSP_OUTPUT_LOG_NONE);
                dsp_ipc_set_dsp2_log_output_sel(DSP_OUTPUT_LOG_NONE);
                dipc_power_control(DIPC_ENTITY_1, DIPC_POWER_CONTROL_MODE_OFF);
            }
        }
        break;

    case DSP_MGR_STATE_WAIT_READY:
        {
            if (event == DSP_MGR_EVENT_LOADING_FINISH)
            {
                dsp_mgr_state_set(DSP_MGR_STATE_READY);

                dsp_mgr_loader_finish();
            }
        }
        break;

    case DSP_MGR_STATE_READY:
        {
            if (event == DSP_MGR_EVENT_START)
            {
                if (dsp_mgr_db.working_activity != NULL)
                {
                    if (dsp_loader_bin_match(dsp_mgr_db.working_activity->session->category) == true)
                    {
                        dsp_send_msg(DSP_MSG_PREPARE_READY,
                                     0,
                                     dsp_mgr_db.working_activity->session,
                                     0);
                    }
                }
            }
            else if (event == DSP_MGR_EVENT_STOP)
            {
                if (dsp_mgr_db.working_activity != NULL)
                {
                    dsp_mgr_session_stop(dsp_mgr_db.working_activity->session);
                }
            }
            else if (event == DSP_MGR_EVENT_STOP_RSP)
            {
                if (!dsp_mgr_is_running())
                {
                    dsp_mgr_state_set(DSP_MGR_STATE_ON);
                    dsp_mgr_power_off_apply();
                }
            }
            else if (event == DSP_MGR_EVENT_SUSPEND_REQ)
            {
                dsp_mgr_state_set(DSP_MGR_STATE_WAIT_FROZEN);

                dipc_power_control(DIPC_ENTITY_1, DIPC_POWER_CONTROL_MODE_SUSPEND);
            }
        }
        break;

    case DSP_MGR_STATE_WAIT_FROZEN:
        {
            if (event == DSP_MGR_EVENT_SUSPEND_RSP)
            {
                dsp_mgr_state_set(DSP_MGR_STATE_FROZEN);
            }
        }
        break;

    case DSP_MGR_STATE_FROZEN:
        {
            if ((event == DSP_MGR_EVENT_RESUME_REQ) ||
                (event == DSP_MGR_EVENT_POWER_ON_REQ))
            {
                dsp_mgr_state_set(DSP_MGR_STATE_WAIT_ON);

                dipc_power_control(DIPC_ENTITY_1, DIPC_POWER_CONTROL_MODE_RESUME);
            }
        }
        break;

    case DSP_MGR_STATE_WAIT_OFF:
        {
            if (event == DSP_MGR_EVENT_POWER_DOWN_RSP)
            {
                dsp_mgr_state_set(DSP_MGR_STATE_OFF);

                dsp_drv_shut_down();

                dsp_mgr_db.flags &= (~DSP_MGR_WAIT_POWER_OFF);
                sys_ipc_publish("dsp_mgr", DSP_MGR_EVT_POWER_OFF, NULL);

            }
        }
        break;
    }
}

bool dsp_mgr_data_send(T_DSP_MGR_SESSION_HANDLE  handle,
                       uint8_t                  *buffer,
                       uint16_t                  length)
{
    T_DSP_MGR_SESSION *session;

    session = (T_DSP_MGR_SESSION *)handle;

    return dsp_ipc_data_send((void *)session->decoder->transport_address, buffer, length, true);
}

uint16_t dsp_mgr_data_len_peek(T_DSP_MGR_SESSION_HANDLE handle)
{
    T_DSP_MGR_SESSION *session;

    session = (T_DSP_MGR_SESSION *)handle;

    return dsp_ipc_data_len_peek((void *)session->encoder->transport_address);
}

uint16_t dsp_mgr_data_recv(T_DSP_MGR_SESSION_HANDLE  handle,
                           uint8_t                  *buffer,
                           uint16_t                  length)
{
    T_DSP_MGR_SESSION *session;

    session = (T_DSP_MGR_SESSION *)handle;

    return dsp_ipc_data_recv((void *)session->encoder->transport_address, buffer, length);
}

uint8_t dsp_mgr_dsp2_ref_get(void)
{
    return dsp_mgr_db.dsp2_ref;
}

void dsp_mgr_dsp2_ref_increment(void)
{
    dsp_mgr_db.dsp2_ref++;
}

void dsp_mgr_dsp2_ref_decrement(void)
{
    dsp_mgr_db.dsp2_ref--;
}

void dsp_mgr_signal_proc_start(T_DSP_MGR_SESSION_HANDLE handle,
                               uint32_t                 timestamp,
                               uint8_t                  clk_ref,
                               bool                     sync_flag)
{
    dsp_remote_init(handle, clk_ref, sync_flag, timestamp);
}

void dsp_mgr_decoder_fifo_report(T_DSP_MGR_SESSION_HANDLE handle,
                                 uint16_t                 interval)
{
    T_DSP_MGR_SESSION *session;

    session = (T_DSP_MGR_SESSION *)handle;

    if (os_queue_search(&dsp_mgr_db.session_queue, session))
    {
        dipc_decoder_fifo_report((uint32_t)session, interval);
    }
}

bool dsp_mgr_decoder_effect_control(T_DSP_MGR_SESSION_HANDLE handle,
                                    uint8_t                  action)
{
    T_DSP_MGR_SESSION *session;
    bool ret = false;

    session = (T_DSP_MGR_SESSION *)handle;

    if (os_queue_search(&dsp_mgr_db.session_queue, session))
    {
        ret = dipc_decoder_effect_control((uint32_t)session, action);
    }

    return ret;
}

bool dsp_mgr_encoder_effect_control(T_DSP_MGR_SESSION_HANDLE handle,
                                    uint8_t                  action)
{
    T_DSP_MGR_SESSION *session;
    bool ret = false;

    session = (T_DSP_MGR_SESSION *)handle;

    if (os_queue_search(&dsp_mgr_db.session_queue, session))
    {
        ret = dipc_encoder_effect_control((uint32_t)session, action);
    }

    return ret;
}

bool dsp_mgr_asrc_set(T_DSP_MGR_SESSION_HANDLE handle,
                      uint8_t                  toggle,
                      int32_t                  ratio,
                      uint32_t                 timestamp)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool               ret = false;

    if (os_queue_search(&dsp_mgr_db.session_queue, session))
    {
        switch (session->category)
        {
        case AUDIO_CATEGORY_AUDIO:
        case AUDIO_CATEGORY_VOICE:
            ret = dipc_decoder_asrc_set((uint32_t)session, ratio, timestamp);
            break;

        case AUDIO_CATEGORY_PIPE:
            ret = dipc_codec_pipe_asrc_set((uint32_t)session, toggle, ratio);
            break;

        default:
            break;
        }
    }

    return ret;
}

bool dsp_mgr_decoder_plc_set(T_DSP_MGR_SESSION_HANDLE handle,
                             bool                     enable,
                             uint16_t                 interval,
                             uint32_t                 threshold)
{
    T_DSP_MGR_SESSION *session;

    session = (T_DSP_MGR_SESSION *)handle;
    if (os_queue_search(&dsp_mgr_db.session_queue, session))
    {
        return dipc_decoder_plc_set((uint32_t)handle, enable, interval, threshold);
    }

    return false;
}

bool dsp_mgr_decoder_gain_set(T_DSP_MGR_SESSION_HANDLE handle,
                              uint16_t                 gain_step_left,
                              uint16_t                 gain_step_right)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool ret = false;

    switch (session->category)
    {
    case AUDIO_CATEGORY_AUDIO:
    case AUDIO_CATEGORY_VOICE:
        ret = dipc_decoder_gain_set((uint32_t)handle, gain_step_left, gain_step_right);
        break;

    case AUDIO_CATEGORY_LINE:
        ret = dipc_line_dac_gain_set((uint32_t)handle, gain_step_left, gain_step_right);
        break;

    case AUDIO_CATEGORY_TONE:
        ret = dipc_ringtone_gain_set((uint32_t)handle, gain_step_left, gain_step_right);
        break;

    case AUDIO_CATEGORY_VP:
        ret = dipc_voice_prompt_gain_set((uint32_t)handle, gain_step_left, gain_step_right);
        break;

    case AUDIO_CATEGORY_APT:
        ret = dipc_apt_dac_gain_set((uint32_t)handle, gain_step_left, gain_step_right);
        break;

    case AUDIO_CATEGORY_PIPE:
        ret = dipc_codec_pipe_gain_set((uint32_t)handle, gain_step_left, gain_step_right);
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_encoder_gain_set(T_DSP_MGR_SESSION_HANDLE handle,
                              uint16_t                 gain_step_left,
                              uint16_t                 gain_step_right)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool ret = false;

    switch (session->category)
    {
    case AUDIO_CATEGORY_VOICE:
    case AUDIO_CATEGORY_RECORD:
        ret = dipc_encoder_gain_set((uint32_t)handle, gain_step_left, gain_step_right);
        break;

    case AUDIO_CATEGORY_LINE:
        ret = dipc_line_adc_gain_set((uint32_t)handle, gain_step_left, gain_step_right);
        break;

    case AUDIO_CATEGORY_APT:
        ret = dipc_apt_adc_gain_set((uint32_t)handle, gain_step_left, gain_step_right);
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_signal_monitoring_set(T_DSP_MGR_SESSION_HANDLE handle,
                                   uint8_t                  direction,
                                   bool                     enable,
                                   uint16_t                 refresh_interval,
                                   uint8_t                  freq_num,
                                   uint32_t                 freq_table[])
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    uint32_t           session_id = (uint32_t)session;

    if ((session->category == AUDIO_CATEGORY_RECORD) ||
        (session->category == AUDIO_CATEGORY_VOICE))
    {
        session_id += 1;
    }

    if (enable == true)
    {
        return dipc_signal_monitoring_start(session_id,
                                            direction,
                                            refresh_interval,
                                            freq_num,
                                            freq_table);
    }
    else
    {
        return dipc_signal_monitoring_stop(session_id, direction);
    }
}

bool dsp_mgr_kws_set(T_DSP_MGR_SESSION_HANDLE handle)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool               ret = false;

    switch (session->category)
    {
    case AUDIO_CATEGORY_VOICE:
    case AUDIO_CATEGORY_RECORD:
    case AUDIO_CATEGORY_VAD:
        ret = dipc_kws_set((uint32_t)session);
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_kws_clear(T_DSP_MGR_SESSION_HANDLE handle)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool               ret = false;

    switch (session->category)
    {
    case AUDIO_CATEGORY_VOICE:
    case AUDIO_CATEGORY_RECORD:
    case AUDIO_CATEGORY_VAD:
        ret = dipc_kws_clear((uint32_t)session);
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_vad_filter(T_DSP_MGR_SESSION_HANDLE handle,
                        uint8_t                  level)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool               ret = false;

    switch (session->category)
    {
    case AUDIO_CATEGORY_VOICE:
    case AUDIO_CATEGORY_RECORD:
    case AUDIO_CATEGORY_VAD:
        ret = dipc_vad_filter((uint32_t)session, level);
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_eq_out_set(T_DSP_MGR_SESSION_HANDLE  handle,
                        uint8_t                  *info_buf,
                        uint16_t                  info_len)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool               ret = false;

    switch (session->category)
    {
    case AUDIO_CATEGORY_AUDIO:
    case AUDIO_CATEGORY_VOICE:
    case AUDIO_CATEGORY_APT:
    case AUDIO_CATEGORY_LINE:
        ret = dipc_eq_set((uint32_t)session, info_buf, info_len);
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_eq_in_set(T_DSP_MGR_SESSION_HANDLE  handle,
                       uint8_t                  *info_buf,
                       uint16_t                  info_len)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool               ret = false;
    uint32_t           session_id = (uint32_t)session;

    switch (session->category)
    {
    case AUDIO_CATEGORY_VOICE:
    case AUDIO_CATEGORY_RECORD:
        {
            session_id += 1;
            ret = dipc_eq_set(session_id, info_buf, info_len);
        }
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_eq_out_clear(T_DSP_MGR_SESSION_HANDLE handle)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool               ret = false;

    switch (session->category)
    {
    case AUDIO_CATEGORY_AUDIO:
    case AUDIO_CATEGORY_VOICE:
    case AUDIO_CATEGORY_APT:
    case AUDIO_CATEGORY_LINE:
        ret = dipc_eq_clear((uint32_t)session);
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_eq_in_clear(T_DSP_MGR_SESSION_HANDLE handle)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool               ret = false;
    uint32_t           session_id = (uint32_t)session;

    switch (session->category)
    {
    case AUDIO_CATEGORY_VOICE:
    case AUDIO_CATEGORY_RECORD:
        {
            session_id += 1;
            ret = dipc_eq_clear(session_id);
        }
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_nrec_set(T_DSP_MGR_SESSION_HANDLE handle,
                      uint8_t                  mode,
                      uint8_t                  level)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool               ret = false;

    switch (session->category)
    {
    case AUDIO_CATEGORY_VOICE:
    case AUDIO_CATEGORY_RECORD:
    case AUDIO_CATEGORY_APT:
        ret = dipc_nrec_set((uint32_t)session, mode, level);
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_nrec_clear(T_DSP_MGR_SESSION_HANDLE handle)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool               ret = false;

    switch (session->category)
    {
    case AUDIO_CATEGORY_VOICE:
    case AUDIO_CATEGORY_RECORD:
    case AUDIO_CATEGORY_APT:
        ret = dipc_nrec_clear((uint32_t)session);
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_sidetone_set(T_DSP_MGR_SESSION_HANDLE handle,
                          uint16_t                 gain_step)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool               ret = false;

    switch (session->category)
    {
    case AUDIO_CATEGORY_VOICE:
        ret = dipc_voice_sidetone_set((uint32_t)session, gain_step);
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_sidetone_clear(T_DSP_MGR_SESSION_HANDLE handle)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool               ret = false;

    switch (session->category)
    {
    case AUDIO_CATEGORY_VOICE:
        ret = dipc_voice_sidetone_clear((uint32_t)session);
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_ovp_set(T_DSP_MGR_SESSION_HANDLE handle,
                     uint8_t                  level)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool               ret = false;

    switch (session->category)
    {
    case AUDIO_CATEGORY_APT:
        ret = dipc_apt_ovp_set((uint32_t)session, level);
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_ovp_clear(T_DSP_MGR_SESSION_HANDLE handle)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool               ret = false;

    switch (session->category)
    {
    case AUDIO_CATEGORY_APT:
        ret = dipc_apt_ovp_clear((uint32_t)session);
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_beamforming_set(T_DSP_MGR_SESSION_HANDLE handle,
                             uint8_t                  direction)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool               ret = false;

    switch (session->category)
    {
    case AUDIO_CATEGORY_VOICE:
    case AUDIO_CATEGORY_RECORD:
    case AUDIO_CATEGORY_APT:
        ret = dipc_apt_beamforming_set((uint32_t)session, direction);
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_beamforming_clear(T_DSP_MGR_SESSION_HANDLE handle)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool               ret = false;

    switch (session->category)
    {
    case AUDIO_CATEGORY_VOICE:
    case AUDIO_CATEGORY_RECORD:
    case AUDIO_CATEGORY_APT:
        ret = dipc_apt_beamforming_clear((uint32_t)session);
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_wdrc_set(T_DSP_MGR_SESSION_HANDLE  handle,
                      uint8_t                  *info_buf,
                      uint16_t                  info_len)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool               ret = false;

    switch (session->category)
    {
    case AUDIO_CATEGORY_AUDIO:
    case AUDIO_CATEGORY_VOICE:
    case AUDIO_CATEGORY_APT:
        ret = dipc_wdrc_set((uint32_t)session, info_buf, info_len);
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_wdrc_clear(T_DSP_MGR_SESSION_HANDLE handle)
{
    T_DSP_MGR_SESSION *session = (T_DSP_MGR_SESSION *)handle;
    bool               ret = false;

    switch (session->category)
    {
    case AUDIO_CATEGORY_AUDIO:
    case AUDIO_CATEGORY_VOICE:
    case AUDIO_CATEGORY_APT:
        ret = dipc_wdrc_clear((uint32_t)session);
        break;

    default:
        break;
    }

    return ret;
}

bool dsp_mgr_vse_req(T_DSP_MGR_SESSION_HANDLE  handle,
                     uint16_t                  company_id,
                     uint16_t                  effect_id,
                     uint16_t                  seq_num,
                     uint8_t                  *info_buf,
                     uint16_t                  info_len)
{
    T_DSP_MGR_SESSION *session;
    uint32_t           session_id;

    session = (T_DSP_MGR_SESSION *)handle;
    if (session->category == AUDIO_CATEGORY_RECORD)
    {
        session_id = (uint32_t)session + 1;
    }
    else
    {
        session_id = (uint32_t)session;
    }

    return dipc_vse_req(session_id,
                        company_id,
                        effect_id,
                        seq_num,
                        info_buf,
                        info_len);
}

bool dsp_mgr_vse_cfm(T_DSP_MGR_SESSION_HANDLE  handle,
                     uint16_t                  company_id,
                     uint16_t                  effect_id,
                     uint16_t                  seq_num,
                     uint8_t                  *info_buf,
                     uint16_t                  info_len)
{
    T_DSP_MGR_SESSION *session;
    uint32_t           session_id;

    session = (T_DSP_MGR_SESSION *)handle;
    if (session->category == AUDIO_CATEGORY_RECORD)
    {
        session_id = (uint32_t)session + 1;
    }
    else
    {
        session_id = (uint32_t)session;
    }

    return dipc_vse_cfm(session_id,
                        company_id,
                        effect_id,
                        seq_num,
                        info_buf,
                        info_len);
}
