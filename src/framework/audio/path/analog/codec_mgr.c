/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "os_queue.h"
#include "os_msg.h"
#include "trace.h"
#include "audio_route_cfg.h"
#include "audio_mgr.h"
#include "codec_mgr.h"
#include "codec_driver.h"
#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
#include "anc_driver.h"
#endif
#include "audio_pad.h"
#include "app_msg.h"
#include "sys_mgr.h"
#include "sys_timer.h"

/* TODO Remove Start */
#include "dsp_driver.h"
/* TODO Remove End */

#define FSM_STATE_NULL                (-1)
#define FSM_EVENT_NULL                (-1)
#define FSM_INVALID_TRANS_TABLE_INDEX (-1)

#define CODEC_MSG_MAX_NUM           (0x10)
#define TIMER_CODEC_POWER_DOWN      1000

#define CODEC_MGR_GATEWAY_CHANN_MASK(chann) (0x01 << (chann))

typedef enum t_codec_mgr_op_event
{
    CODEC_MGR_OP_EVENT_START,
    CODEC_MGR_OP_EVENT_STOP,
} T_CODEC_MGR_OP_EVENT;

typedef enum t_codec_mgr_op_state
{
    CODEC_MGR_OP_STATE_IDLE,
    CODEC_MGR_OP_STATE_BUSY,
} T_CODEC_MGR_OP_STATE;

typedef enum t_codec_mgr_session_event
{
    CODEC_MGR_SESSION_EVENT_ENABLE,
    CODEC_MGR_SESSION_EVENT_ENABLE_COMPLETE,
    CODEC_MGR_SESSION_EVENT_DISABLE,
    CODEC_MGR_SESSION_EVENT_DISABLING,
    CODEC_MGR_SESSION_EVENT_DISABLE_COMPLETE,
} T_CODEC_MGR_SESSION_EVENT;

typedef int8_t (*P_TRANSITION_FUNC)(void *context);
typedef void (*P_ENTER_FUNC)(void *context);

typedef struct
{
    uint8_t     msg_type;
    uint16_t    data_len;
    void       *p_data;
} T_CODEC_SCHED_MSG;

typedef struct t_fsm_transition_table
{
    int8_t enter_state;
    int8_t event;
    P_TRANSITION_FUNC trans_func;
    int8_t next_state;
    P_ENTER_FUNC enter_func;
} T_FSM_TRANSITION_TABLE;

typedef struct t_fsm_handle
{
    int8_t state;
    const char *name;
    const T_FSM_TRANSITION_TABLE *trans_table;
    int8_t *index_table;
    int8_t event_max;
    int8_t state_max;
    void *context;
} T_FSM_HANDLE;

typedef struct t_codec_mgr_session
{
    struct t_codec_mgr_session *p_next;
    T_FSM_HANDLE               *session_fsm;
    T_CODEC_MGR_EVENT_PARAM     event_param;
    void                       *context;
    uint32_t                    device;
    uint32_t                    dac_sample_rate;
    uint32_t                    adc_sample_rate;
    uint8_t                     adc_gain_level;
    uint8_t                     dac_gain_level;
    T_AUDIO_CATEGORY            category;
} T_CODEC_MGR_SESSION;

typedef struct t_codec_mgr_operation
{
    struct t_codec_mgr_operation *p_next;
    T_CODEC_MGR_SESSION *session;
    T_CODEC_MGR_SESSION_EVENT event;
} T_CODEC_MGR_OPERATION;

typedef struct t_codec_mgr_loopback
{
    uint16_t enable      : 1;
    uint16_t dac_ref_cnt : 3;
    uint16_t adc_ref_cnt : 3;
    uint16_t adc_ch      : 3;
    uint16_t adc_loopback: 1;
} T_CODEC_MGR_LOOPBACK;

typedef struct t_adc_status
{
    uint8_t count;
    uint8_t total;
} T_ADC_STATUS;

typedef struct t_codec_mgr_db
{
    void *msg_handle;
    T_OS_QUEUE session_queue;
    T_OS_QUEUE operation_queue;
    T_FSM_HANDLE *operation_fsm;
    T_FSM_HANDLE *codec_fsm;
    T_CODEC_MGR_OPERATION *operation;
    T_CODEC_MGR_EVENT_PARAM event_param;
    uint8_t dac_5m_refcnt[DAC_CHANNEL_MAX];
    uint8_t dac_fs_refcnt[DAC_CHANNEL_MAX];
    uint8_t adc_refcnt[ADC_CHANNEL_MAX];
    uint8_t spk_refcnt[SPK_CHANNEL_MAX];
    uint8_t amic_refcnt[AMIC_CHANNEL_MAX];
    uint8_t dmic_refcnt[DMIC_CHANNEL_MAX];
    uint8_t aux_in_refcnt[AUX_CHANNEL_MAX];
    T_CODEC_MGR_LOOPBACK loopback_chann[AUDIO_ROUTE_DAC_CHANN_NUM];
    bool sidetone_status;
    bool wait_sidetone_off;
    T_ADC_STATUS adc_status[AUDIO_CATEGORY_NUM];
    bool lps_en;
    bool wait_power_off;
    T_SYS_TIMER_HANDLE power_off_timer;
} T_CODEC_MGR_DB;

typedef enum t_codec_mgr_event
{
    CODEC_MGR_EVENT_ENABLE,
    CODEC_MGR_EVENT_ENABLE_COMPLETE,
    CODEC_MGR_EVENT_MUTE,
    CODEC_MGR_EVENT_MUTE_COMPLETE,
    CODEC_MGR_EVENT_DISABLE,
    CODEC_MGR_EVENT_DISABLE_COMPLETE,
} T_CODEC_MGR_EVENT;

typedef void (*P_CODEC_MGR_CBACK)(T_CODEC_MGR_SESSION_STATE  state,
                                  void                      *context);
typedef void (*P_ADC_GAIN_SET_ACTION)(T_CODEC_MGR_SESSION *session,
                                      T_AUDIO_ROUTE_ENTRY *entry);

extern void *hEventQueueHandleAu;
static T_CODEC_MGR_DB *codec_mgr_db = NULL;

static void codec_mgr_session_event_post(T_CODEC_MGR_SESSION_EVENT event);
static void codec_mgr_op_event_post(T_CODEC_MGR_OP_EVENT event);
static void codec_mgr_event_post(T_CODEC_MGR_EVENT event);
static void codec_mgr_session_route(T_CODEC_MGR_SESSION_EVENT event);
static int8_t codec_mgr_session_cfg_set(void *context);
static int8_t codec_mgr_session_cfg_clear(void *context);
static int8_t codec_mgr_session_eq_set(void *context);

/*audio route and codec mic tye define is different*/
const uint8_t codec_mgr_mic_type_table[] =
{
    [AUDIO_ROUTE_MIC_SINGLE_END]   = MIC_TYPE_SINGLE_END_AMIC,
    [AUDIO_ROUTE_MIC_DIFFERENTIAL] = MIC_TYPE_DIFFERENTIAL_AMIC,
    [AUDIO_ROUTE_MIC_FALLING]      = MIC_TYPE_FALLING_DMIC,
    [AUDIO_ROUTE_MIC_RAISING]      = MIC_TYPE_RAISING_DMIC,
};

/* Depend on IC */
static const T_AUDIO_ROUTE_DAC_CHANN loopback_map [AUDIO_ROUTE_ADC_CHANN_NUM] =
{
    [AUDIO_ROUTE_ADC_CHANN0] = AUDIO_ROUTE_DAC_CHANN0,
    [AUDIO_ROUTE_ADC_CHANN1] = AUDIO_ROUTE_DAC_CHANN1,
    [AUDIO_ROUTE_ADC_CHANN2] = AUDIO_ROUTE_DAC_CHANN0,
    [AUDIO_ROUTE_ADC_CHANN3] = AUDIO_ROUTE_DAC_CHANN1,
    [AUDIO_ROUTE_ADC_CHANN4] = AUDIO_ROUTE_DAC_CHANN0,
    [AUDIO_ROUTE_ADC_CHANN5] = AUDIO_ROUTE_DAC_CHANN1,
};

static const T_CODEC_DOWNLINK_MIX
mixpoint_map[AUDIO_ROUTE_DAC_CHANN_NUM][AUDIO_ROUTE_DAC_MIX_POINT_NUM] =
{
    [AUDIO_ROUTE_DAC_CHANN0][AUDIO_ROUTE_DAC_MIX_POINT0]  = CODEC_DOWNLINK_MIX_NONE,
    [AUDIO_ROUTE_DAC_CHANN0][AUDIO_ROUTE_DAC_MIX_POINT2]  = CODEC_DOWNLINK_MIX_L_R,
    [AUDIO_ROUTE_DAC_CHANN0][AUDIO_ROUTE_DAC_MIX_POINT4]  = CODEC_DOWNLINK_MIX_L_2,
    [AUDIO_ROUTE_DAC_CHANN0][AUDIO_ROUTE_DAC_MIX_POINT6]  = CODEC_DOWNLINK_MIX_L_R_2,
    [AUDIO_ROUTE_DAC_CHANN1][AUDIO_ROUTE_DAC_MIX_POINT0]  = CODEC_DOWNLINK_MIX_NONE,
    [AUDIO_ROUTE_DAC_CHANN1][AUDIO_ROUTE_DAC_MIX_POINT1]  = CODEC_DOWNLINK_MIX_R_L,
    [AUDIO_ROUTE_DAC_CHANN1][AUDIO_ROUTE_DAC_MIX_POINT4]  = CODEC_DOWNLINK_MIX_R_2,
    [AUDIO_ROUTE_DAC_CHANN1][AUDIO_ROUTE_DAC_MIX_POINT5]  = CODEC_DOWNLINK_MIX_R_L_2,
};

static void codec_mgr_msg_send(T_CODEC_SCHED_MSG *codec_msg)
{
    unsigned char event = EVENT_CODEC_MSG;

    if (hEventQueueHandleAu == NULL)
    {
        return;
    }

    if (os_msg_send(codec_mgr_db->msg_handle, codec_msg, 0) == true)
    {
        (void)os_msg_send(hEventQueueHandleAu, &event, 0);
    }
}

static void codec_mgr_op_queue_in(T_CODEC_MGR_OPERATION *operation)
{
    os_queue_in(&(codec_mgr_db->operation_queue), operation);
}

static T_CODEC_MGR_OPERATION *codec_mgr_op_queue_peek(void)
{
    return os_queue_peek(&(codec_mgr_db->operation_queue), 0);
}

static T_CODEC_MGR_OPERATION *codec_mgr_op_queue_out(void)
{
    return os_queue_out(&(codec_mgr_db->operation_queue));
}

static T_CODEC_MGR_SESSION *codec_mgr_session_find(T_AUDIO_CATEGORY category)
{
    T_CODEC_MGR_SESSION *session;

    session = os_queue_peek(&(codec_mgr_db->session_queue), 0);
    while (session != NULL)
    {
        if (session->category == category)
        {
            return session;
        }

        session = session->p_next;
    }

    return NULL;
}

void codec_mgr_msg_handler(void)
{
    T_CODEC_SCHED_MSG codec_msg;

    if (os_msg_recv(codec_mgr_db->msg_handle, &codec_msg, 0) == true)
    {
        CODEC_PRINT_TRACE1("codec_mgr_msg_handler: event %d", codec_msg.msg_type);

        switch (codec_msg.msg_type)
        {
        case CODEC_CB_STATE_MSG:
            {
                T_CODEC_STATE state = (T_CODEC_STATE)(*((uint8_t *)codec_msg.p_data));

                switch (state)
                {
                case CODEC_STATE_OFF:
                    {
                        codec_mgr_event_post(CODEC_MGR_EVENT_DISABLE_COMPLETE);
                    }
                    break;

                case CODEC_STATE_MUTE:
                    {
                        codec_mgr_event_post(CODEC_MGR_EVENT_MUTE_COMPLETE);
                    }
                    break;

                case CODEC_STATE_ON:
                    {
                        codec_mgr_event_post(CODEC_MGR_EVENT_ENABLE_COMPLETE);
                    }
                    break;

                default:
                    break;
                }
            }
            break;

        case CODEC_CB_CLEAR_SIDETONE:
            {
                T_CODEC_MGR_SESSION *voice_session;
                T_CODEC_MGR_SESSION *record_session;
                int8_t               state = 0;

                voice_session  = codec_mgr_session_find(AUDIO_CATEGORY_VOICE);
                record_session = codec_mgr_session_find(AUDIO_CATEGORY_RECORD);

                codec_drv_sidetone_config_clear(SIDETONE_CHANNEL_L);
                codec_drv_sidetone_config_clear(SIDETONE_CHANNEL_R);

                if (voice_session != NULL)
                {
                    state = voice_session->session_fsm->state;
                }
                else if (record_session != NULL)
                {
                    state = record_session->session_fsm->state;
                }

                if (state == CODEC_MGR_SESSION_STATE_DISABLING &&
                    codec_mgr_db->wait_sidetone_off)
                {
                    codec_mgr_session_event_post(CODEC_MGR_SESSION_EVENT_DISABLING);
                    codec_mgr_db->wait_sidetone_off = false;
                }
            }
            break;

        default:
            break;
        }
    }
}

static void codec_mgr_amic_set(T_CODEC_AMIC_CHANNEL_SEL mic_sel,
                               T_CODEC_ADC_ANA_GAIN     ana_gain,
                               uint8_t                  mic_type,
                               uint8_t                  mic_class)
{
    T_CODEC_AMIC_CONFIG amic_config;

    codec_drv_config_init(CODEC_CONFIG_SEL_AMIC, (void *)&amic_config);
    amic_config.enable = 1;
    amic_config.ch_sel = mic_sel;
    amic_config.mic_type = mic_type;
    amic_config.mic_class = mic_class;
    amic_config.ana_gain = ana_gain;
    codec_drv_amic_config_set(mic_sel, &amic_config);
}

static void codec_mgr_amic_gain_set(T_CODEC_AMIC_CHANNEL_SEL mic_sel,
                                    T_CODEC_ADC_ANA_GAIN     ana_gain)
{
    T_CODEC_AMIC_CONFIG amic_config;

    codec_drv_config_init(CODEC_CONFIG_SEL_AMIC, (void *)&amic_config);
    amic_config.ch_sel = mic_sel;
    amic_config.ana_gain = ana_gain;
    codec_drv_amic_config_set(mic_sel, &amic_config);
}

static void codec_mgr_dmic_set(T_CODEC_DMIC_CHANNEL_SEL mic_sel,
                               T_CODEC_DMIC_CLK_SEL     clk_sel,
                               uint8_t                  mic_type)
{
    T_CODEC_DMIC_CONFIG dmic_config;

    codec_drv_config_init(CODEC_CONFIG_SEL_DMIC, (void *)&dmic_config);
    dmic_config.enable = 1;
    dmic_config.ch_sel = mic_sel;
    dmic_config.mic_type = mic_type;
    dmic_config.dmic_clk_sel = clk_sel;
    codec_drv_dmic_config_set(mic_sel, &dmic_config);
}

static void codec_mgr_spk_set(T_CODEC_SPK_CHANNEL_SEL spk_sel,
                              uint8_t                 spk_type,
                              uint8_t                 spk_class)
{
    T_CODEC_SPK_CONFIG spk_config;

    codec_drv_config_init(CODEC_CONFIG_SEL_SPK, (void *)&spk_config);
    spk_config.power_en = 1;
    spk_config.spk_type = spk_type;
    spk_config.spk_class = spk_class;
    codec_drv_spk_config_set(spk_sel, &spk_config);
}

static void codec_mgr_aux_set(T_CODEC_AUX_CHANNEL_SEL aux_sel,
                              uint8_t                 aux_class)
{
    T_CODEC_AUX_CONFIG aux_config;

    codec_drv_config_init(CODEC_CONFIG_SEL_AUX, (void *)&aux_config);
    aux_config.enable = 1;
    aux_config.ch_sel = aux_sel;
    aux_config.aux_class = aux_class;
    codec_drv_aux_config_set(aux_sel, &aux_config);
}

static void codec_mgr_aux_in_gain_set(T_CODEC_AUX_CHANNEL_SEL aux_sel,
                                      T_CODEC_ADC_ANA_GAIN    ana_gain)
{
    T_CODEC_AUX_CONFIG aux_config;

    codec_drv_config_init(CODEC_CONFIG_SEL_AUX, (void *)&aux_config);
    aux_config.ch_sel = aux_sel;
    aux_config.ana_gain = ana_gain;
    codec_drv_aux_config_set(aux_sel, &aux_config);
}

static void codec_mgr_adc_set(T_CODEC_ADC_CHANNEL_SEL ch_sel,
                              T_CODEC_I2S_CHANNEL_SEL i2s_sel,
                              T_CODEC_AD_SRC_SEL      mic_src,
                              uint8_t                 src_sel,
                              uint32_t                sample_rate)
{
    T_CODEC_ADC_CONFIG adc_config;

    codec_drv_config_init(CODEC_CONFIG_SEL_ADC, (void *)&adc_config);
    adc_config.ad_src_sel = mic_src;
    adc_config.src_sel = src_sel;
    adc_config.enable = 1;
    adc_config.i2s_sel = i2s_sel;
    adc_config.asrc_en = 0;
    adc_config.sample_rate = sample_rate;
    codec_drv_adc_config_set(ch_sel, &adc_config);
}

static void codec_mgr_adc_gain_set(T_CODEC_ADC_CHANNEL_SEL ch_sel,
                                   T_CODEC_ADC_BOOST_GAIN  boost_gain,
                                   uint16_t                dig_gain)
{
    T_CODEC_ADC_CONFIG adc_config;

    codec_drv_config_init(CODEC_CONFIG_SEL_ADC, (void *)&adc_config);
    adc_config.dig_gain = dig_gain;
    adc_config.boost_gain = boost_gain;
    codec_drv_adc_config_set(ch_sel, &adc_config);
}

static void codec_mgr_i2s_set(T_AUDIO_ROUTE_ENTRY *entry)
{
    T_CODEC_I2S_CONFIG      i2s_config;
    T_CODEC_I2S_CHANNEL_SEL i2s_sel;
    T_CODEC_ADC_CHANNEL_SEL ch_sel;

    i2s_sel = (T_CODEC_I2S_CHANNEL_SEL)(entry->gateway_attr.idx);

    codec_drv_config_init(CODEC_CONFIG_SEL_I2S, (void *)&i2s_config);

    if (entry->gateway_attr.dir == AUDIO_ROUTE_GATEWAY_DIR_TX)
    {
        i2s_config.tx_data_format = (T_SPORT_DATA_FORMAT_SEL)entry->gateway_attr.format;
        i2s_config.tx_data_len = (T_CODEC_I2S_DATA_LEN_SEL)entry->gateway_attr.data_len;
        i2s_config.tx_channel_len = (T_CODEC_I2S_CH_LEN_SEL)entry->gateway_attr.chann_len;

        codec_drv_i2s_config_set(i2s_sel, &i2s_config);
    }
    else
    {
        uint8_t i;

        if (entry->endpoint_type == AUDIO_ROUTE_ENDPOINT_MIC)
        {
            ch_sel = (T_CODEC_ADC_CHANNEL_SEL)entry->endpoint_attr.mic.adc_ch;
        }
        else if (entry->endpoint_type == AUDIO_ROUTE_ENDPOINT_AUX)
        {
            ch_sel = (T_CODEC_ADC_CHANNEL_SEL)entry->endpoint_attr.aux_in.adc_ch;
        }

        i2s_config.rx_data_format = (T_SPORT_DATA_FORMAT_SEL)entry->gateway_attr.format;
        i2s_config.rx_tdm_mode = (T_SPORT_TDM_MODE_SEL)entry->gateway_attr.chann_mode;
        i2s_config.rx_data_len = (T_CODEC_I2S_DATA_LEN_SEL)entry->gateway_attr.data_len;
        i2s_config.rx_channel_len = (T_CODEC_I2S_CH_LEN_SEL)entry->gateway_attr.chann_len;

        for (i = 0; i < AUDIO_ROUTE_GATEWAY_CHANN_NUM; i++)
        {
            if (entry->gateway_attr.channs & CODEC_MGR_GATEWAY_CHANN_MASK(i))
            {
                i2s_config.rx_data_ch_en[i]  = 1;
                i2s_config.rx_data_ch_sel[i] = (T_CODEC_I2S_RX_CH_SEL)ch_sel;

                codec_drv_i2s_config_set(i2s_sel, &i2s_config);
            }
        }
    }

}

static void codec_mgr_dac_set(T_CODEC_DAC_CHANNEL_SEL dac_ch,
                              T_CODEC_I2S_CHANNEL_SEL idx,
                              uint32_t                sample_rate,
                              bool                    is_anc)
{
    T_CODEC_DAC_CONFIG dac_config;
    codec_drv_config_init(CODEC_CONFIG_SEL_DAC, (void *)&dac_config);

    if (is_anc)
    {
        dac_config.anc_mute_en = 0;
    }
    else
    {
        dac_config.music_mute_en = 0;
        dac_config.i2s_sel = idx;
    }

    dac_config.ana_power_en = 1;
    dac_config.dig_power_en = 1;
    dac_config.sample_rate = sample_rate;
    dac_config.asrc_en = 0;

    codec_drv_dac_config_set((T_CODEC_DAC_CHANNEL_SEL)dac_ch, &dac_config);
}

static void codec_mgr_dac_gain_set(T_CODEC_DAC_CHANNEL_SEL dac_ch,
                                   T_CODEC_DAC_ANA_GAIN    ana_gain,
                                   uint16_t                dig_gain)
{
    T_CODEC_DAC_CONFIG dac_config;

    codec_drv_config_init(CODEC_CONFIG_SEL_DAC, (void *)&dac_config);
    dac_config.ana_gain = ana_gain;
    dac_config.dig_gain = dig_gain;
    codec_drv_dac_config_set(dac_ch, &dac_config);
}

static void codec_mgr_dac_clear(T_CODEC_DAC_CHANNEL_SEL dac_ch)
{
    T_CODEC_DAC_CONFIG dac_config;

    codec_drv_config_init(CODEC_CONFIG_SEL_DAC, (void *)&dac_config);
    dac_config.dig_gain = 0;
    codec_drv_dac_config_set(dac_ch, &dac_config);
}

void codec_mgr_loopback_enable(void)
{
    T_CODEC_ADC_CONFIG adc_config;
    T_CODEC_ADC_CHANNEL_SEL ch_sel;
    T_CODEC_MGR_LOOPBACK *loopback;
    uint8_t i;

    for (i = 0; i < AUDIO_ROUTE_DAC_CHANN_NUM; ++i)
    {
        loopback = &(codec_mgr_db->loopback_chann[i]);

        CODEC_PRINT_TRACE6("codec_mgr_loopback_enable: dac_ch %d, enable %d, adc_ref_cnt %d, "
                           "dac_ref_cnt %d, adc_ch %d, adc_loopback %d",
                           i, loopback->enable, loopback->adc_ref_cnt, loopback->dac_ref_cnt,
                           loopback->adc_ch, loopback->adc_loopback);

        if (loopback->adc_loopback == false)
        {
            if (loopback->enable == true)
            {
                ch_sel = (T_CODEC_ADC_CHANNEL_SEL)(loopback->adc_ch);

                codec_drv_config_init(CODEC_CONFIG_SEL_ADC, (void *)&adc_config);

                adc_config.loopback = CODEC_ADC_DECI_SEL_MUSIC;

                codec_drv_adc_config_set(ch_sel, &adc_config);

                loopback->adc_loopback = true;
            }
        }
    }
}

void codec_mgr_loopback_disable(void)
{
    T_CODEC_ADC_CONFIG adc_config;
    T_CODEC_ADC_CHANNEL_SEL ch_sel;
    T_CODEC_MGR_LOOPBACK *loopback;
    uint8_t i;

    for (i = 0; i < AUDIO_ROUTE_DAC_CHANN_NUM; ++i)
    {
        loopback = &(codec_mgr_db->loopback_chann[i]);

        CODEC_PRINT_TRACE6("codec_mgr_loopback_disable: dac_ch %d, enable %d, adc_ref_cnt %d, "
                           "dac_ref_cnt %d, adc_ch %d, adc_loopback %d",
                           i, loopback->enable, loopback->adc_ref_cnt, loopback->dac_ref_cnt,
                           loopback->adc_ch, loopback->adc_loopback);

        if (loopback->adc_loopback == true)
        {
            if (loopback->enable == false)
            {
                ch_sel = (T_CODEC_ADC_CHANNEL_SEL)(loopback->adc_ch);

                codec_drv_config_init(CODEC_CONFIG_SEL_ADC, (void *)&adc_config);

                adc_config.loopback = CODEC_ADC_DECI_SEL_AMIC;

                codec_drv_adc_config_set(ch_sel, &adc_config);

                loopback->adc_loopback = false;
            }
        }
    }
}

bool codec_mgr_handle_check(T_CODEC_MGR_SESSION_HANDLE handle)
{
    T_CODEC_MGR_SESSION *session = os_queue_peek(&(codec_mgr_db->session_queue), 0);
    while (session != NULL)
    {
        if (session == (T_CODEC_MGR_SESSION *)handle)
        {
            return true;
        }
        session = session->p_next;
    }

    return false;
}

static T_CODEC_MGR_OPERATION *codec_mgr_op_create(T_CODEC_MGR_SESSION_HANDLE handle,
                                                  T_CODEC_MGR_SESSION_EVENT  event)
{
    T_CODEC_MGR_OPERATION *operation;
    operation = malloc(sizeof(T_CODEC_MGR_OPERATION));
    if (operation != NULL)
    {
        operation->session = (T_CODEC_MGR_SESSION *)handle;
        operation->event = event;
    }

    return operation;
}

static void codec_mgr_op_destroy(T_CODEC_MGR_OPERATION *operation)
{
    if (operation != NULL)
    {
        free(operation);
    }
}

static int8_t fsm_get_trans_table_index(T_FSM_HANDLE *handle,
                                        int8_t        event)
{
    int8_t enter_state = handle->state;
    if ((event > handle->event_max) ||
        (enter_state > handle->state_max))
    {
        return FSM_INVALID_TRANS_TABLE_INDEX;
    }

    return handle->index_table[enter_state * (handle->event_max + 1) + event];
}

void fsm_run(T_FSM_HANDLE *handle,
             uint32_t      event)
{
    int8_t next_state;
    int8_t index = FSM_INVALID_TRANS_TABLE_INDEX;

    int8_t enter_state = handle->state;
    const T_FSM_TRANSITION_TABLE *trans_table = handle->trans_table;
    if (trans_table == NULL)
    {
        return;
    }

    index = fsm_get_trans_table_index(handle, event);
    if (index != FSM_INVALID_TRANS_TABLE_INDEX)
    {
        if (trans_table[index].trans_func)
        {
            next_state = trans_table[index].trans_func(handle->context);
            if (trans_table[index].next_state != FSM_STATE_NULL)
            {
                handle->state = trans_table[index].next_state;
            }
            else
            {
                handle->state = next_state;
            }
        }
        else
        {
            handle->state = trans_table[index].next_state;
        }

        if (trans_table[index].enter_func)
        {
            trans_table[index].enter_func(handle->context);
        }
    }

    CODEC_PRINT_TRACE4("fsm_run: name %s, enter_state %d, event %d, next_state %d",
                       TRACE_STRING(handle->name), enter_state, event, handle->state);

}

static bool fsm_create_trans_index_table(T_FSM_HANDLE                 *handle,
                                         const T_FSM_TRANSITION_TABLE *trans_table)
{
    uint8_t i = 0;
    uint8_t state_max = 0;
    uint8_t event_max = 0;
    while (trans_table[i].enter_state != FSM_STATE_NULL)
    {
        if (trans_table[i].enter_state > state_max)
        {
            state_max = trans_table[i].enter_state;
        }
        if (trans_table[i].event > event_max)
        {
            event_max = trans_table[i].event;
        }
        i++;
    }
    handle->event_max = event_max;
    handle->state_max = state_max;

    handle->index_table = malloc((state_max + 1) * (event_max + 1));
    if (handle->index_table == NULL)
    {
        return false;
    }
    memset(handle->index_table, FSM_INVALID_TRANS_TABLE_INDEX, (state_max + 1) * (event_max + 1));

    i = 0;
    uint8_t enter_state = 0;
    uint8_t event = 0;
    while (trans_table[i].enter_state != FSM_STATE_NULL)
    {
        enter_state = trans_table[i].enter_state;
        event = trans_table[i].event;
        handle->index_table[enter_state * (handle->event_max + 1) + event] = i;
        i++;
    }

    return true;
}

T_FSM_HANDLE *fsm_init(const T_FSM_TRANSITION_TABLE *trans_table,
                       int8_t                        init_state,
                       const char                   *name,
                       void                         *context)
{
    T_FSM_HANDLE *handle;
    int32_t ret = 0;

    handle = malloc(sizeof(T_FSM_HANDLE));
    if (handle == NULL)
    {
        ret = 1;
        goto fail_alloc_handle;
    }

    handle->name        = name;
    handle->trans_table = trans_table;
    handle->state       = init_state;
    handle->context     = context;

    if (!fsm_create_trans_index_table(handle, trans_table))
    {
        ret = 2;
        goto fail_create_trans_index_table;
    }

    return handle;

fail_create_trans_index_table:
    free(handle);
fail_alloc_handle:
    CODEC_PRINT_TRACE1("fsm_init: failed %d", -ret);
    return NULL;
}

static void fsm_deinit(T_FSM_HANDLE *handle)
{
    if (handle != NULL)
    {
        free(handle->index_table);
        free(handle);
    }
}

static void codec_mgr_op_enter_busy(void *context)
{
    T_CODEC_MGR_SESSION_EVENT event = codec_mgr_db->operation->event;
    codec_mgr_session_event_post(event);
}

static int8_t codec_mgr_op_start_action(void *context)
{
    T_CODEC_MGR_OPERATION *operation = codec_mgr_op_queue_peek();
    codec_mgr_db->operation = operation;

    return FSM_STATE_NULL;
}

static int8_t codec_mgr_op_stop_action(void *context)
{
    T_CODEC_MGR_OPERATION *operation = codec_mgr_op_queue_out();
    codec_mgr_op_destroy(operation);

    return FSM_STATE_NULL;
}

static void codec_mgr_op_enter_idle(void *context)
{
    T_CODEC_MGR_OPERATION *operation = os_queue_peek(&(codec_mgr_db->operation_queue), 0);
    if (operation != NULL)
    {
        codec_mgr_op_event_post(CODEC_MGR_OP_EVENT_START);
    }
}

const static T_FSM_TRANSITION_TABLE operation_fsm_trans_table[] =
{
    {CODEC_MGR_OP_STATE_IDLE, CODEC_MGR_OP_EVENT_START, codec_mgr_op_start_action, CODEC_MGR_OP_STATE_BUSY, codec_mgr_op_enter_busy},
    {CODEC_MGR_OP_STATE_BUSY, CODEC_MGR_OP_EVENT_STOP, codec_mgr_op_stop_action, CODEC_MGR_OP_STATE_IDLE, codec_mgr_op_enter_idle},
    {FSM_STATE_NULL, FSM_EVENT_NULL, NULL, FSM_STATE_NULL, NULL},
};

static void codec_mgr_op_event_post(T_CODEC_MGR_OP_EVENT event)
{
    fsm_run(codec_mgr_db->operation_fsm, event);
}

static T_CODEC_MGR_SESSION_STATE codec_mgr_get_main_session_state(void)
{
    T_CODEC_MGR_SESSION *session = os_queue_peek(&(codec_mgr_db->session_queue), 0);
    T_CODEC_MGR_SESSION_STATE state = CODEC_MGR_SESSION_STATE_DISABLED;
    while (session != NULL)
    {
        if (session->session_fsm->state > state)
        {
            state = (T_CODEC_MGR_SESSION_STATE)session->session_fsm->state;
            if (state == CODEC_MGR_SESSION_STATE_ENABLING)
            {
                break;
            }
        }
        session = session->p_next;
    }

    return state;
}

static void codec_mgr_enter_enabling(void *context)
{
    codec_drv_enable();
}

static void codec_mgr_enter_enabled(void *context)
{
    codec_mgr_session_event_post(CODEC_MGR_SESSION_EVENT_ENABLE_COMPLETE);
}

static void codec_mgr_enter_muting(void *context)
{
    codec_drv_set_mute();
}

static bool codec_mgr_is_idle(void)
{
    T_CODEC_MGR_SESSION *session;

    session = os_queue_peek(&codec_mgr_db->session_queue, 0);
    while (session != NULL)
    {
        if (session->session_fsm->state != CODEC_MGR_SESSION_STATE_DISABLED)
        {
            return false;
        }
        session = session->p_next;
    }

    return true;
}

static void codec_mgr_enter_muted(void *context)
{
    codec_mgr_session_event_post(CODEC_MGR_SESSION_EVENT_DISABLE_COMPLETE);

    sys_timer_stop(codec_mgr_db->power_off_timer);

    if (codec_mgr_db->wait_power_off == true)
    {
        if (codec_mgr_is_idle())
        {
            codec_mgr_event_post(CODEC_MGR_EVENT_DISABLE);
        }
    }
    else
    {
        if (codec_mgr_db->lps_en == true)
        {
            if (codec_mgr_db->codec_fsm->state != CODEC_MGR_STATE_POWER_OFF)
            {
                sys_timer_start(codec_mgr_db->power_off_timer);
            }
        }
    }
}

static void codec_mgr_enter_disabling(void *context)
{
    codec_drv_disable();
}

static void codec_mgr_enter_disabled(void *context)
{
    (codec_mgr_db->event_param).context = NULL;
    (codec_mgr_db->event_param).state = CODEC_MGR_STATE_POWER_OFF;
    codec_mgr_db->wait_power_off = false;
    dsp_send_msg(DSP_MSG_CODEC_STATE, 0, &(codec_mgr_db->event_param), 0);
}

const static T_FSM_TRANSITION_TABLE codec_mgr_fsm_trans_table[] =
{
    {CODEC_MGR_STATE_POWER_OFF, CODEC_MGR_EVENT_ENABLE, NULL, CODEC_MGR_STATE_WAIT_ACTIVE, codec_mgr_enter_enabling},
    {CODEC_MGR_STATE_WAIT_ACTIVE, CODEC_MGR_EVENT_ENABLE_COMPLETE, NULL, CODEC_MGR_STATE_ACTIVED, codec_mgr_enter_enabled},
    {CODEC_MGR_STATE_ACTIVED, CODEC_MGR_EVENT_MUTE, NULL, CODEC_MGR_STATE_WAIT_MUTE, codec_mgr_enter_muting},
    {CODEC_MGR_STATE_ACTIVED, CODEC_MGR_EVENT_ENABLE, NULL, CODEC_MGR_STATE_WAIT_ACTIVE, codec_mgr_enter_enabling},
    {CODEC_MGR_STATE_WAIT_MUTE, CODEC_MGR_EVENT_MUTE_COMPLETE, NULL, CODEC_MGR_STATE_MUTED, codec_mgr_enter_muted},
    {CODEC_MGR_STATE_MUTED, CODEC_MGR_EVENT_DISABLE, NULL, CODEC_MGR_STATE_WAIT_POWER_OFF, codec_mgr_enter_disabling},
    {CODEC_MGR_STATE_MUTED, CODEC_MGR_EVENT_ENABLE, NULL, CODEC_MGR_STATE_WAIT_ACTIVE, codec_mgr_enter_enabling},
    {CODEC_MGR_STATE_WAIT_POWER_OFF, CODEC_MGR_EVENT_DISABLE_COMPLETE, NULL, CODEC_MGR_STATE_POWER_OFF, codec_mgr_enter_disabled},
    {CODEC_MGR_STATE_WAIT_POWER_OFF, CODEC_MGR_EVENT_ENABLE, NULL, CODEC_MGR_STATE_WAIT_ACTIVE, codec_mgr_enter_enabling},
    {FSM_STATE_NULL, FSM_EVENT_NULL, NULL, FSM_STATE_NULL, NULL},
};

static void codec_mgr_event_post(T_CODEC_MGR_EVENT event)
{
    fsm_run(codec_mgr_db->codec_fsm, event);
}

static void codec_mgr_session_route(T_CODEC_MGR_SESSION_EVENT event)
{
    T_CODEC_MGR_SESSION_STATE state = codec_mgr_get_main_session_state();
    CODEC_PRINT_TRACE2("codec_mgr_session_route: state %d, event %d", state, event);
    switch (state)
    {
    case CODEC_MGR_SESSION_STATE_ENABLING:
        {
            if (event == CODEC_MGR_SESSION_EVENT_ENABLE)
            {
                codec_mgr_event_post(CODEC_MGR_EVENT_ENABLE);
            }
        }
        break;

    case CODEC_MGR_SESSION_STATE_DISABLING:
        {
            if (event == CODEC_MGR_SESSION_EVENT_DISABLE)
            {
                codec_mgr_event_post(CODEC_MGR_EVENT_MUTE);
            }
        }
        break;

    case CODEC_MGR_SESSION_STATE_ENABLED:
        {
            if (event == CODEC_MGR_SESSION_EVENT_DISABLE)
            {
                codec_mgr_session_event_post(CODEC_MGR_SESSION_EVENT_DISABLE_COMPLETE);
            }
        }
        break;

    default:
        break;
    }
}

static void codec_mgr_session_event_post(T_CODEC_MGR_SESSION_EVENT event)
{
    T_CODEC_MGR_SESSION *session = codec_mgr_db->operation->session;
    fsm_run(session->session_fsm, event);
}

static void codec_mgr_session_enter_enabling(void *context)
{
    codec_mgr_session_route(CODEC_MGR_SESSION_EVENT_ENABLE);
}

static void codec_mgr_session_enter_enabled(void *context)
{
    T_CODEC_MGR_SESSION *session = (T_CODEC_MGR_SESSION *)context;
    (session->event_param).context = session->context;
    (session->event_param).state = CODEC_MGR_SESSION_STATE_ENABLED;
    dsp_send_msg(DSP_MSG_CODEC_STATE, 0, &(session->event_param), 0);
    codec_mgr_op_event_post(CODEC_MGR_OP_EVENT_STOP);
}

static void codec_mgr_session_enter_disabling(void *context)
{
    codec_mgr_session_route(CODEC_MGR_SESSION_EVENT_DISABLE);
}

static void codec_mgr_session_enter_disabled(void *context)
{
    T_CODEC_MGR_SESSION *session = (T_CODEC_MGR_SESSION *)context;
    (session->event_param).context = session->context;
    (session->event_param).state = CODEC_MGR_SESSION_STATE_DISABLED;
    dsp_send_msg(DSP_MSG_CODEC_STATE, 0, &(session->event_param), 0);
    codec_mgr_op_event_post(CODEC_MGR_OP_EVENT_STOP);
}


static void codec_mgr_session_nop(void *context)
{
    codec_mgr_op_event_post(CODEC_MGR_OP_EVENT_STOP);
}

static void codec_mgr_session_enter_fading_out(void *context)
{
    T_CODEC_MGR_SESSION *session = (T_CODEC_MGR_SESSION *)context;

    if (codec_mgr_db->sidetone_status)
    {
        if ((session->category == AUDIO_CATEGORY_APT) ||
            (session->category == AUDIO_CATEGORY_VOICE) ||
            (session->category == AUDIO_CATEGORY_RECORD))
        {
            codec_mgr_sidetone_set(0, 0, false);
            codec_mgr_db->wait_sidetone_off = true;
        }
        else
        {
            codec_mgr_session_event_post(CODEC_MGR_SESSION_EVENT_DISABLING);
        }
    }
    else
    {
        codec_mgr_session_event_post(CODEC_MGR_SESSION_EVENT_DISABLING);
    }
}

const static T_FSM_TRANSITION_TABLE session_fsm_trans_table[] =
{
    {CODEC_MGR_SESSION_STATE_DISABLED, CODEC_MGR_SESSION_EVENT_ENABLE, codec_mgr_session_cfg_set, CODEC_MGR_SESSION_STATE_ENABLING, codec_mgr_session_enter_enabling},
    {CODEC_MGR_SESSION_STATE_DISABLED, CODEC_MGR_SESSION_EVENT_DISABLE, codec_mgr_session_cfg_clear, CODEC_MGR_SESSION_STATE_DISABLED, codec_mgr_session_nop},
    {CODEC_MGR_SESSION_STATE_ENABLING, CODEC_MGR_SESSION_EVENT_ENABLE_COMPLETE, codec_mgr_session_eq_set, CODEC_MGR_SESSION_STATE_ENABLED, codec_mgr_session_enter_enabled},
    {CODEC_MGR_SESSION_STATE_ENABLED, CODEC_MGR_SESSION_EVENT_DISABLE, NULL, CODEC_MGR_SESSION_STATE_DISABLING, codec_mgr_session_enter_fading_out},
    {CODEC_MGR_SESSION_STATE_DISABLING, CODEC_MGR_SESSION_EVENT_DISABLING, codec_mgr_session_cfg_clear, CODEC_MGR_SESSION_STATE_DISABLING, codec_mgr_session_enter_disabling},
    {CODEC_MGR_SESSION_STATE_ENABLED, CODEC_MGR_SESSION_EVENT_ENABLE, codec_mgr_session_cfg_set, CODEC_MGR_SESSION_STATE_ENABLING, codec_mgr_session_enter_enabling},
    {CODEC_MGR_SESSION_STATE_DISABLING, CODEC_MGR_SESSION_EVENT_DISABLE_COMPLETE, NULL, CODEC_MGR_SESSION_STATE_DISABLED, codec_mgr_session_enter_disabled},
    {FSM_STATE_NULL, FSM_EVENT_NULL, NULL, FSM_STATE_NULL, NULL},
};

bool codec_mgr_session_control(T_CODEC_MGR_SESSION_HANDLE handle,
                               T_CODEC_MGR_SESSION_EVENT  event)
{
    T_CODEC_MGR_OPERATION *operation;
    int32_t ret = 0;

    CODEC_PRINT_TRACE2("codec_mgr_session_control: handle %p, event %d", handle, event);

    if (!codec_mgr_handle_check(handle))
    {
        ret = 1;
        goto fail_invalid_handle;
    }

    operation = codec_mgr_op_create(handle, event);
    if (operation == NULL)
    {
        ret = 2;
        goto fail_create_operation;
    }

    codec_mgr_op_queue_in(operation);
    codec_mgr_op_event_post(CODEC_MGR_OP_EVENT_START);

    return true;

fail_create_operation:
fail_invalid_handle:
    CODEC_PRINT_ERROR1("codec_mgr_session_control: failed %d", -ret);
    return false;
}

bool codec_mgr_session_enable(T_CODEC_MGR_SESSION_HANDLE handle)
{
    return codec_mgr_session_control(handle, CODEC_MGR_SESSION_EVENT_ENABLE);
}

bool codec_mgr_session_disable(T_CODEC_MGR_SESSION_HANDLE handle)
{
    return codec_mgr_session_control(handle, CODEC_MGR_SESSION_EVENT_DISABLE);
}

T_CODEC_MGR_SESSION_STATE codec_mgr_session_state_get(T_CODEC_MGR_SESSION_HANDLE handle)
{
    T_CODEC_MGR_SESSION_STATE state = CODEC_MGR_SESSION_STATE_DISABLED;
    T_CODEC_MGR_SESSION *session = (T_CODEC_MGR_SESSION *)handle;

    if (codec_mgr_handle_check(handle))
    {
        state = (T_CODEC_MGR_SESSION_STATE)session->session_fsm->state;
    }

    return state;
}

void codec_mgr_power_on(void)
{
    codec_mgr_event_post(CODEC_MGR_EVENT_ENABLE);
}

void codec_mgr_power_off(void)
{
    codec_mgr_db->wait_power_off = true;
    sys_timer_stop(codec_mgr_db->power_off_timer);

    if (codec_mgr_is_idle())
    {
        codec_mgr_event_post(CODEC_MGR_EVENT_DISABLE);
    }
}

static bool codec_mgr_is_audio_spk(T_AUDIO_ROUTE_IO_TYPE io_type)
{
    if ((io_type == AUDIO_ROUTE_IO_AUDIO_PRIMARY_OUT)          ||
        (io_type == AUDIO_ROUTE_IO_AUDIO_SECONDARY_OUT)        ||
        (io_type == AUDIO_ROUTE_IO_VOICE_PRIMARY_OUT)          ||
        (io_type == AUDIO_ROUTE_IO_VOICE_SECONDARY_OUT)        ||
        (io_type == AUDIO_ROUTE_IO_LINE_PRIMARY_OUT)           ||
        (io_type == AUDIO_ROUTE_IO_LINE_SECONDARY_OUT)         ||
        (io_type == AUDIO_ROUTE_IO_RINGTONE_PRIMARY_OUT)       ||
        (io_type == AUDIO_ROUTE_IO_RINGTONE_SECONDARY_OUT)     ||
        (io_type == AUDIO_ROUTE_IO_VOICE_PROMPT_PRIMARY_OUT)   ||
        (io_type == AUDIO_ROUTE_IO_VOICE_PROMPT_SECONDARY_OUT) ||
        (io_type == AUDIO_ROUTE_IO_APT_PRIMARY_OUT)            ||
        (io_type == AUDIO_ROUTE_IO_APT_SECONDARY_OUT))
    {
        return true;
    }

    return false;
}

static bool codec_mgr_is_anc_mic(T_AUDIO_ROUTE_IO_TYPE io_type)
{
    if ((io_type == AUDIO_ROUTE_IO_ANC_FF_LEFT_IN)  ||
        (io_type == AUDIO_ROUTE_IO_ANC_FF_RIGHT_IN) ||
        (io_type == AUDIO_ROUTE_IO_ANC_FB_LEFT_IN)  ||
        (io_type == AUDIO_ROUTE_IO_ANC_FB_RIGHT_IN) ||
        (io_type == AUDIO_ROUTE_IO_LLAPT_LEFT_IN)   ||
        (io_type == AUDIO_ROUTE_IO_LLAPT_RIGHT_IN))
    {
        return true;
    }

    return false;
}

static bool codec_mgr_is_audio_mic(T_AUDIO_ROUTE_IO_TYPE io_type)
{
    if ((io_type == AUDIO_ROUTE_IO_VOICE_PRIMARY_IN)        ||
        (io_type == AUDIO_ROUTE_IO_VOICE_SECONDARY_IN)      ||
        (io_type == AUDIO_ROUTE_IO_VOICE_FUSION_IN)         ||
        (io_type == AUDIO_ROUTE_IO_VOICE_BONE_IN)           ||
        (io_type == AUDIO_ROUTE_IO_VOICE_PRIMARY_REF_IN)    ||
        (io_type == AUDIO_ROUTE_IO_VOICE_SECONDARY_REF_IN)  ||
        (io_type == AUDIO_ROUTE_IO_RECORD_PRIMARY_IN)       ||
        (io_type == AUDIO_ROUTE_IO_RECORD_SECONDARY_IN)     ||
        (io_type == AUDIO_ROUTE_IO_RECORD_FUSION_IN)        ||
        (io_type == AUDIO_ROUTE_IO_RECORD_BONE_IN)          ||
        (io_type == AUDIO_ROUTE_IO_RECORD_PRIMARY_REF_IN)   ||
        (io_type == AUDIO_ROUTE_IO_RECORD_SECONDARY_REF_IN) ||
        (io_type == AUDIO_ROUTE_IO_LINE_LEFT_IN)            ||
        (io_type == AUDIO_ROUTE_IO_LINE_RIGHT_IN)           ||
        (io_type == AUDIO_ROUTE_IO_APT_FRONT_LEFT_IN)       ||
        (io_type == AUDIO_ROUTE_IO_APT_FRONT_RIGHT_IN)      ||
        (io_type == AUDIO_ROUTE_IO_APT_FF_LEFT_IN)          ||
        (io_type == AUDIO_ROUTE_IO_APT_FF_RIGHT_IN)         ||
        (io_type == AUDIO_ROUTE_IO_APT_FB_LEFT_IN)          ||
        (io_type == AUDIO_ROUTE_IO_APT_FB_RIGHT_IN)         ||
        (io_type == AUDIO_ROUTE_IO_APT_BONE_LEFT_IN)        ||
        (io_type == AUDIO_ROUTE_IO_APT_BONE_RIGHT_IN)       ||
        (io_type == AUDIO_ROUTE_IO_APT_PRIMARY_REF_IN)      ||
        (io_type == AUDIO_ROUTE_IO_APT_SECONDARY_REF_IN)    ||
        (io_type == AUDIO_ROUTE_IO_VAD_PRIMARY_IN)          ||
        (io_type == AUDIO_ROUTE_IO_VAD_SECONDARY_IN)        ||
        (io_type == AUDIO_ROUTE_IO_VAD_PRIMARY_REF_IN)      ||
        (io_type == AUDIO_ROUTE_IO_VAD_SECONDARY_REF_IN)    ||
        (io_type == AUDIO_ROUTE_IO_LINE_PRIMARY_REF_IN)     ||
        (io_type == AUDIO_ROUTE_IO_LINE_SECONDARY_REF_IN)   ||
        (io_type == AUDIO_ROUTE_IO_LLAPT_PRIMARY_REF_IN)    ||
        (io_type == AUDIO_ROUTE_IO_LLAPT_SECONDARY_REF_IN)  ||
        (io_type == AUDIO_ROUTE_IO_ANC_PRIMARY_REF_IN)      ||
        (io_type == AUDIO_ROUTE_IO_ANC_SECONDARY_REF_IN))
    {
        return true;
    }

    return false;
}

static bool codec_mgr_is_mic(T_AUDIO_ROUTE_IO_TYPE io_type)
{
    if ((io_type == AUDIO_ROUTE_IO_VOICE_PRIMARY_IN)        ||
        (io_type == AUDIO_ROUTE_IO_VOICE_SECONDARY_IN)      ||
        (io_type == AUDIO_ROUTE_IO_VOICE_FUSION_IN)         ||
        (io_type == AUDIO_ROUTE_IO_VOICE_BONE_IN)           ||
        (io_type == AUDIO_ROUTE_IO_RECORD_PRIMARY_IN)       ||
        (io_type == AUDIO_ROUTE_IO_RECORD_SECONDARY_IN)     ||
        (io_type == AUDIO_ROUTE_IO_RECORD_FUSION_IN)        ||
        (io_type == AUDIO_ROUTE_IO_RECORD_BONE_IN)          ||
        (io_type == AUDIO_ROUTE_IO_LINE_LEFT_IN)            ||
        (io_type == AUDIO_ROUTE_IO_LINE_RIGHT_IN)           ||
        (io_type == AUDIO_ROUTE_IO_APT_FRONT_LEFT_IN)       ||
        (io_type == AUDIO_ROUTE_IO_APT_FRONT_RIGHT_IN)      ||
        (io_type == AUDIO_ROUTE_IO_APT_FF_LEFT_IN)          ||
        (io_type == AUDIO_ROUTE_IO_APT_FF_RIGHT_IN)         ||
        (io_type == AUDIO_ROUTE_IO_APT_FB_LEFT_IN)          ||
        (io_type == AUDIO_ROUTE_IO_APT_FB_RIGHT_IN)         ||
        (io_type == AUDIO_ROUTE_IO_APT_BONE_LEFT_IN)        ||
        (io_type == AUDIO_ROUTE_IO_APT_BONE_RIGHT_IN)       ||
        (io_type == AUDIO_ROUTE_IO_VAD_PRIMARY_IN)          ||
        (io_type == AUDIO_ROUTE_IO_VAD_SECONDARY_IN))
    {
        return true;
    }

    return false;
}

T_CODEC_MGR_SESSION_HANDLE codec_mgr_session_create(T_AUDIO_CATEGORY  category,
                                                    uint32_t          dac_sample_rate,
                                                    uint32_t          adc_sample_rate,
                                                    uint8_t           dac_gain_level,
                                                    uint8_t           adc_gain_level,
                                                    uint32_t          device,
                                                    void             *context)
{
    T_CODEC_MGR_SESSION *session;
    int32_t              ret = 0;

    if (codec_mgr_db == NULL)
    {
        ret = 1;
        goto fail_invalid_db;
    }

    session = calloc(1, sizeof(T_CODEC_MGR_SESSION));
    if (session == NULL)
    {
        ret = 2;
        goto fail_alloc_session;
    }

    session->session_fsm = fsm_init(session_fsm_trans_table, CODEC_MGR_SESSION_STATE_DISABLED,
                                    "codec_mgr_session_fsm", session);
    if (session->session_fsm == NULL)
    {
        ret = 3;
        goto fail_init_fsm;
    }

    session->context         = context;
    session->device          = device;
    session->category        = category;
    session->adc_gain_level  = adc_gain_level;
    session->dac_gain_level  = dac_gain_level;
    session->adc_sample_rate = adc_sample_rate;
    session->dac_sample_rate = dac_sample_rate;
    os_queue_in(&(codec_mgr_db->session_queue), session);

    return (T_CODEC_MGR_SESSION_HANDLE)session;

fail_init_fsm:
    free(session);
fail_alloc_session:
fail_invalid_db:
    CODEC_PRINT_ERROR1("codec_mgr_session_create: failed %d", -ret);
    return NULL;
}

bool codec_mgr_session_destroy(T_CODEC_MGR_SESSION_HANDLE handle)
{
    T_CODEC_MGR_SESSION *session = (T_CODEC_MGR_SESSION *)handle;

    if (!codec_mgr_handle_check(handle))
    {
        return false;
    }

    os_queue_delete(&(codec_mgr_db->session_queue), session);
    fsm_deinit(session->session_fsm);
    free(session);

    return true;
}

bool codec_mgr_cback(T_CODEC_CB_EVENT event,
                     uint32_t         param)
{
    T_CODEC_SCHED_MSG codec_msg;

    codec_msg.msg_type = (uint8_t)event;
    codec_msg.p_data = (void *)param;
    codec_mgr_msg_send(&codec_msg);

    return true;
}

T_CODEC_DMIC_CLK_SEL codec_mgr_get_codec_dmic_clk(uint8_t mic_sel)
{
    uint8_t dmic_clk = DMIC_CLK_5MHZ;

    if (mic_sel == MIC_SEL_DMIC_1)
    {
        dmic_clk = audio_route_cfg.dmic1_clock;
    }
    else if (mic_sel == MIC_SEL_DMIC_2)
    {
        dmic_clk = audio_route_cfg.dmic2_clock;
    }
    else if (mic_sel == MIC_SEL_DMIC_3)
    {
        dmic_clk = audio_route_cfg.dmic3_clock;
    }
    return (T_CODEC_DMIC_CLK_SEL)dmic_clk;
}

static void codec_mgr_adc_config_set(T_AUDIO_ROUTE_ENTRY *entry,
                                     T_CODEC_MGR_SESSION *session)
{
    T_CODEC_ADC_CHANNEL_SEL ch_sel;
    T_CODEC_ADC_CONFIG ref_adc_config;
    uint8_t mic_type, mic_class;
    uint8_t codec_mic_sel, mic_src;
    T_CODEC_I2S_CHANNEL_SEL i2s_sel;
    T_AUDIO_ROUTE_DAC_CHANN dac_ch;

    dac_ch = AUDIO_ROUTE_DAC_CHANN_NUM;

    /*To be optimized */
    mic_type = codec_mgr_mic_type_table[entry->endpoint_attr.mic.type];
    mic_class = entry->endpoint_attr.mic.class;
    codec_mic_sel = codec_drv_get_mic_ch_sel(entry->endpoint_attr.mic.id, &mic_src);
    ch_sel = (T_CODEC_ADC_CHANNEL_SEL)(entry->endpoint_attr.mic.adc_ch);
    i2s_sel = (T_CODEC_I2S_CHANNEL_SEL)(entry->gateway_attr.idx);

    if (codec_mgr_is_audio_mic(entry->io_type))
    {
        if (entry->endpoint_attr.mic.id == AUDIO_ROUTE_REF_MIC)
        {
            dac_ch = loopback_map[entry->endpoint_attr.mic.adc_ch];

            if (dac_ch < AUDIO_ROUTE_DAC_CHANN_NUM)
            {
                (codec_mgr_db->loopback_chann[dac_ch].adc_ref_cnt) ++;
                codec_mgr_db->loopback_chann[dac_ch].adc_ch = entry->endpoint_attr.mic.adc_ch;
                if ((codec_mgr_db->loopback_chann[dac_ch].dac_ref_cnt > 0) &&
                    (codec_mgr_db->loopback_chann[dac_ch].adc_ref_cnt > 0) &&
                    (codec_mgr_db->loopback_chann[dac_ch].adc_ch < AUDIO_ROUTE_ADC_CHANN_NUM))
                {
                    codec_mgr_db->loopback_chann[dac_ch].enable = true;
                }
            }

            // adc setting
            codec_drv_config_init(CODEC_CONFIG_SEL_ADC, (void *)&ref_adc_config);
            ref_adc_config.enable = 1;
            ref_adc_config.i2s_sel = i2s_sel;
            ref_adc_config.asrc_en = 0;
            ref_adc_config.equalizer_en = 0;
            ref_adc_config.sample_rate = session->adc_sample_rate;
            codec_drv_adc_config_set(ch_sel, &ref_adc_config);
            (codec_mgr_db->adc_refcnt[ch_sel])++;
        }
        else
        {
            // adc setting
            codec_mgr_adc_set(ch_sel,
                              i2s_sel,
                              (T_CODEC_AD_SRC_SEL)mic_src,
                              codec_drv_get_mic_src_sel(entry->endpoint_attr.mic.id, mic_type),
                              session->adc_sample_rate);

            (codec_mgr_db->adc_refcnt[ch_sel])++;

            // mic setting
            if (mic_src == AD_SRC_AMIC)
            {
                codec_mgr_amic_set((T_CODEC_AMIC_CHANNEL_SEL)codec_mic_sel,
                                   (T_CODEC_ADC_ANA_GAIN)entry->endpoint_attr.mic.ana_gain,
                                   mic_type,
                                   mic_class);

                (codec_mgr_db->amic_refcnt[codec_mic_sel])++;
            }
            else if (mic_src == AD_SRC_DMIC)
            {
                codec_mgr_dmic_set((T_CODEC_DMIC_CHANNEL_SEL)codec_mic_sel,
                                   codec_mgr_get_codec_dmic_clk(entry->endpoint_attr.mic.id),
                                   mic_type);

                (codec_mgr_db->dmic_refcnt[codec_mic_sel])++;
            }
        }
    }
    else if (codec_mgr_is_anc_mic(entry->io_type))
    {
        if (mic_src == AD_SRC_AMIC)
        {
            codec_mgr_amic_set((T_CODEC_AMIC_CHANNEL_SEL)codec_mic_sel,
                               (T_CODEC_ADC_ANA_GAIN)entry->endpoint_attr.mic.ana_gain,
                               mic_type,
                               mic_class);

            (codec_mgr_db->amic_refcnt[codec_mic_sel])++;

        }
        else if (mic_src == AD_SRC_DMIC)
        {
            codec_mgr_dmic_set((T_CODEC_DMIC_CHANNEL_SEL)codec_mic_sel,
                               codec_mgr_get_codec_dmic_clk(entry->endpoint_attr.mic.id),
                               mic_type);

            (codec_mgr_db->dmic_refcnt[codec_mic_sel])++;
        }
    }

    codec_mgr_db->adc_status[session->category].count++;
}

static void codec_mgr_mic_enable(T_AUDIO_ROUTE_ENTRY *entry,
                                 T_CODEC_MGR_SESSION *session)
{
    if (codec_mgr_is_audio_mic(entry->io_type))
    {
        codec_mgr_adc_config_set(entry, session);
        codec_mgr_i2s_set(entry);
    }
    else if (codec_mgr_is_anc_mic(entry->io_type))
    {
#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
        T_ANC_DRV_MIC_SEL mic_ch;
        uint8_t mic_class;
        uint8_t mic_type;

        /*To be optimized */
        mic_type = codec_mgr_mic_type_table[entry->endpoint_attr.mic.type];
        mic_class = entry->endpoint_attr.mic.class;
        mic_ch = (T_ANC_DRV_MIC_SEL)entry->endpoint_attr.mic.adc_ch;

        codec_mgr_adc_config_set(entry, session);

        anc_drv_mic_src_setting(mic_ch,
                                entry->endpoint_attr.mic.id,
                                mic_type,
                                mic_class);
#endif
    }
}

static void codec_mgr_mic_disable(T_CODEC_MGR_SESSION *session,
                                  T_AUDIO_ROUTE_ENTRY *entry)
{
    T_CODEC_ADC_CHANNEL_SEL ch_sel;
    uint8_t mic_sel, mic_src;

    ch_sel = (T_CODEC_ADC_CHANNEL_SEL)(entry->endpoint_attr.mic.adc_ch);
    mic_sel = codec_drv_get_mic_ch_sel(entry->endpoint_attr.mic.id, &mic_src);

    if (codec_mgr_db->adc_status[session->category].count > 0)
    {
        if (codec_mgr_is_audio_mic(entry->io_type))
        {
            T_AUDIO_ROUTE_DAC_CHANN dac_ch;

            if (entry->endpoint_attr.mic.id == AUDIO_ROUTE_REF_MIC)
            {
                dac_ch = loopback_map[entry->endpoint_attr.mic.adc_ch];
                codec_mgr_db->loopback_chann[dac_ch].adc_ch = AUDIO_ROUTE_ADC_CHANN_NUM;

                if ((codec_mgr_db->loopback_chann[dac_ch].adc_ref_cnt) != 0)
                {
                    (codec_mgr_db->loopback_chann[dac_ch].adc_ref_cnt)--;
                }

                if ((codec_mgr_db->loopback_chann[dac_ch].adc_ref_cnt) == 0)
                {
                    codec_mgr_db->loopback_chann[dac_ch].enable = false;
                }
            }
            else
            {
                if (mic_src == AD_SRC_AMIC)
                {
                    if (codec_mgr_db->amic_refcnt[mic_sel] != 0)
                    {
                        (codec_mgr_db->amic_refcnt[mic_sel])--;
                    }

                    if (codec_mgr_db->amic_refcnt[mic_sel] == 0)
                    {
                        codec_drv_amic_config_clear((T_CODEC_AMIC_CHANNEL_SEL)mic_sel);
                    }
                }
                else if (mic_src == AD_SRC_DMIC)
                {
                    if (codec_mgr_db->dmic_refcnt[mic_sel] != 0)
                    {
                        (codec_mgr_db->dmic_refcnt[mic_sel])--;
                    }

                    if (codec_mgr_db->dmic_refcnt[mic_sel] == 0)
                    {
                        codec_drv_dmic_config_clear((T_CODEC_DMIC_CHANNEL_SEL)mic_sel);
                    }
                }
            }

            if (codec_mgr_db->adc_refcnt[ch_sel] != 0)
            {
                (codec_mgr_db->adc_refcnt[ch_sel])--;
            }

            if (codec_mgr_db->adc_refcnt[ch_sel] == 0)
            {
                codec_drv_adc_config_clear(ch_sel);
                codec_drv_eq_data_clear(CODEC_EQ_CONFIG_PATH_ADC, ch_sel);
            }
        }
        else if (codec_mgr_is_anc_mic(entry->io_type))
        {
#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
            T_ANC_DRV_MIC_SEL mic_ch;

            if (mic_src == AD_SRC_AMIC)
            {
                if (codec_mgr_db->amic_refcnt[mic_sel] != 0)
                {
                    (codec_mgr_db->amic_refcnt[mic_sel])--;
                }

                if (codec_mgr_db->amic_refcnt[mic_sel] == 0)
                {
                    codec_drv_amic_config_clear((T_CODEC_AMIC_CHANNEL_SEL)mic_sel);
                }
            }
            else if (mic_src == AD_SRC_DMIC)
            {
                if (codec_mgr_db->dmic_refcnt[mic_sel] != 0)
                {
                    (codec_mgr_db->dmic_refcnt[mic_sel])--;
                }

                if (codec_mgr_db->dmic_refcnt[mic_sel] == 0)
                {
                    codec_drv_dmic_config_clear((T_CODEC_DMIC_CHANNEL_SEL)mic_sel);
                }
            }

            mic_ch  = (T_ANC_DRV_MIC_SEL)entry->endpoint_attr.mic.adc_ch;
            anc_drv_mic_src_clear(mic_ch);
#endif
        }

        codec_mgr_db->adc_status[session->category].count--;
    }
}

static void codec_mgr_mix_enable(void)
{
    T_CODEC_DAC_CONFIG dac_config;
    uint8_t mixpoint;

    if (audio_route_cfg.dac_0_mix_point != 0)
    {
        mixpoint = audio_route_cfg.dac_0_mix_point;
        codec_drv_config_init(CODEC_CONFIG_SEL_DAC, (void *)&dac_config);
        dac_config.ana_power_en = 1;
        dac_config.dig_power_en = 1;
        dac_config.downlink_mix = mixpoint_map[DAC_CHANNEL_L][mixpoint];
        dac_config.music_mute_en = 0;

        if (codec_mgr_db->dac_fs_refcnt[DAC_CHANNEL_L] == 0)
        {
            dac_config.dig_gain = 0;
        }

        (codec_mgr_db->dac_5m_refcnt[DAC_CHANNEL_L])++;
        codec_drv_dac_config_set(DAC_CHANNEL_L, &dac_config);
    }

    if (audio_route_cfg.dac_1_mix_point != 0)
    {
        mixpoint = audio_route_cfg.dac_1_mix_point;
        codec_drv_config_init(CODEC_CONFIG_SEL_DAC, (void *)&dac_config);
        dac_config.ana_power_en = 1;
        dac_config.dig_power_en = 1;
        dac_config.downlink_mix = mixpoint_map[DAC_CHANNEL_R][mixpoint];
        dac_config.music_mute_en = 0;

        if (codec_mgr_db->dac_fs_refcnt[DAC_CHANNEL_R] == 0)
        {
            dac_config.dig_gain = 0;
        }

        (codec_mgr_db->dac_5m_refcnt[DAC_CHANNEL_R])++;
        codec_drv_dac_config_set(DAC_CHANNEL_R, &dac_config);
    }
}

static void codec_mgr_spk_enable(T_AUDIO_ROUTE_ENTRY *entry,
                                 T_CODEC_MGR_SESSION *session)
{
    uint32_t spk_sample_rate = 0;
    T_AUDIO_ROUTE_DAC_CHANN dac_ch;
    T_AUDIO_ROUTE_SPK_ID spk_id;

    dac_ch = entry->endpoint_attr.spk.dac_ch;
    spk_id = entry->endpoint_attr.spk.id;

    if (spk_id == AUDIO_ROUTE_REF_SPK)
    {
        (codec_mgr_db->loopback_chann[dac_ch].dac_ref_cnt) ++;

        if ((codec_mgr_db->loopback_chann[dac_ch].adc_ref_cnt > 0) &&
            (codec_mgr_db->loopback_chann[dac_ch].dac_ref_cnt > 0) &&
            (codec_mgr_db->loopback_chann[dac_ch].adc_ch < AUDIO_ROUTE_ADC_CHANN_NUM))
        {
            codec_mgr_db->loopback_chann[dac_ch].enable = true;
        }
    }
    else
    {
        // spk setting
        (codec_mgr_db->spk_refcnt[spk_id])++;
        codec_mgr_spk_set((T_CODEC_SPK_CHANNEL_SEL)spk_id,
                          entry->endpoint_attr.spk.type,
                          entry->endpoint_attr.spk.class);

        // dac setting
        spk_sample_rate = session->dac_sample_rate;
        if (codec_mgr_db->dac_fs_refcnt[dac_ch] != 0)
        {
            spk_sample_rate = 0xFFFFFFFF;
        }

        if ((session->category == AUDIO_CATEGORY_ANC) ||
            (session->category == AUDIO_CATEGORY_LLAPT))
        {
            codec_mgr_db->dac_5m_refcnt[dac_ch]++;
            codec_mgr_dac_set((T_CODEC_DAC_CHANNEL_SEL)dac_ch,
                              I2S_CHANNEL_MAX,
                              spk_sample_rate,
                              true);
        }
        else
        {
            codec_mgr_db->dac_5m_refcnt[dac_ch]++;
            codec_mgr_db->dac_fs_refcnt[dac_ch]++;
            codec_mgr_dac_set((T_CODEC_DAC_CHANNEL_SEL)dac_ch,
                              (T_CODEC_I2S_CHANNEL_SEL)entry->gateway_attr.idx,
                              spk_sample_rate,
                              false);
        }
    }

    // i2s setting
    codec_mgr_i2s_set(entry);
}

static void codec_mgr_mix_disable(void)
{
    if (audio_route_cfg.dac_0_mix_point != 0)
    {
        codec_mgr_db->dac_5m_refcnt[DAC_CHANNEL_L]--;

        if ((codec_mgr_db->dac_5m_refcnt[DAC_CHANNEL_L] == 0) &&
            (codec_mgr_db->dac_fs_refcnt[DAC_CHANNEL_L] == 0))
        {
            codec_drv_dac_config_clear((T_CODEC_DAC_CHANNEL_SEL)DAC_CHANNEL_L);
            codec_drv_eq_data_clear(CODEC_EQ_CONFIG_PATH_DAC, DAC_CHANNEL_L);
        }
    }

    if (audio_route_cfg.dac_1_mix_point != 0)
    {
        codec_mgr_db->dac_5m_refcnt[DAC_CHANNEL_R]--;

        if ((codec_mgr_db->dac_5m_refcnt[DAC_CHANNEL_R] == 0) &&
            (codec_mgr_db->dac_fs_refcnt[DAC_CHANNEL_R] == 0))
        {
            codec_drv_dac_config_clear((T_CODEC_DAC_CHANNEL_SEL)DAC_CHANNEL_R);
            codec_drv_eq_data_clear(CODEC_EQ_CONFIG_PATH_DAC, DAC_CHANNEL_R);
        }
    }
}

static void codec_mgr_spk_disable(T_CODEC_MGR_SESSION *session,
                                  T_AUDIO_ROUTE_ENTRY *entry)
{
    T_AUDIO_ROUTE_DAC_CHANN dac_ch;
    T_AUDIO_ROUTE_SPK_ID spk_id;

    dac_ch = entry->endpoint_attr.spk.dac_ch;
    spk_id = entry->endpoint_attr.spk.id;

    if (spk_id != AUDIO_ROUTE_REF_SPK)
    {
        if (codec_mgr_db->spk_refcnt[spk_id] != 0)
        {
            (codec_mgr_db->spk_refcnt[spk_id])--;
        }

        if (codec_mgr_db->dac_5m_refcnt[dac_ch] != 0)
        {
            (codec_mgr_db->dac_5m_refcnt[dac_ch])--;
        }

        if ((session->category == AUDIO_CATEGORY_ANC) ||
            (session->category == AUDIO_CATEGORY_LLAPT))
        {
            T_CODEC_DAC_CONFIG dac_config;
            codec_drv_config_init(CODEC_CONFIG_SEL_DAC, (void *)&dac_config);
            dac_config.anc_mute_en = 1;
            codec_drv_dac_config_set((T_CODEC_DAC_CHANNEL_SEL)dac_ch, &dac_config);
        }
        else if (codec_mgr_db->dac_fs_refcnt[dac_ch] != 0)
        {
            (codec_mgr_db->dac_fs_refcnt[dac_ch])--;
        }

        if ((codec_mgr_db->dac_5m_refcnt[dac_ch] == 0) &&
            (codec_mgr_db->dac_fs_refcnt[dac_ch] == 0))
        {
            codec_drv_dac_config_clear((T_CODEC_DAC_CHANNEL_SEL)dac_ch);
            codec_drv_eq_data_clear(CODEC_EQ_CONFIG_PATH_DAC, dac_ch);
        }
        else if ((codec_mgr_db->dac_fs_refcnt[dac_ch] == 0))
        {
            codec_mgr_dac_clear((T_CODEC_DAC_CHANNEL_SEL)dac_ch);
        }

        if (codec_mgr_db->spk_refcnt[spk_id] == 0)
        {
            codec_drv_spk_config_clear((T_CODEC_SPK_CHANNEL_SEL)spk_id);
        }

        CODEC_PRINT_TRACE5("codec_mgr_spk_disable: spk_id %d, spk_refcnt %d, dac_ch %d, "
                           "dac_5m_refcnt %d, dac_fs_refcnt %d",
                           spk_id, codec_mgr_db->spk_refcnt[spk_id], dac_ch,
                           codec_mgr_db->dac_5m_refcnt[dac_ch], codec_mgr_db->dac_fs_refcnt[dac_ch]);
    }
    else
    {
        if ((codec_mgr_db->loopback_chann[dac_ch].dac_ref_cnt) != 0)
        {
            (codec_mgr_db->loopback_chann[dac_ch].dac_ref_cnt)--;
        }

        if (codec_mgr_db->loopback_chann[dac_ch].dac_ref_cnt == 0)
        {
            codec_mgr_db->loopback_chann[dac_ch].enable = 0;
        }
    }
}

static void codec_mgr_aux_in_enable(T_AUDIO_ROUTE_ENTRY *entry,
                                    T_CODEC_MGR_SESSION *session)
{
    T_CODEC_ADC_CHANNEL_SEL ch_sel;
    T_CODEC_I2S_CHANNEL_SEL i2s_sel;
    uint8_t                 aux_sel;
    uint8_t                 aux_class;

    aux_sel = (T_CODEC_AUX_CHANNEL_SEL)(entry->endpoint_attr.aux_in.id);
    aux_class = entry->endpoint_attr.aux_in.class;
    codec_mgr_aux_set((T_CODEC_AUX_CHANNEL_SEL)aux_sel, aux_class);
    (codec_mgr_db->aux_in_refcnt[aux_sel])++;

    ch_sel = (T_CODEC_ADC_CHANNEL_SEL)(entry->endpoint_attr.aux_in.adc_ch);
    i2s_sel = (T_CODEC_I2S_CHANNEL_SEL)(entry->gateway_attr.idx);
    codec_mgr_adc_set(ch_sel,
                      i2s_sel,
                      AD_SRC_AUX,
                      aux_sel,
                      session->adc_sample_rate);
    (codec_mgr_db->adc_refcnt[ch_sel])++;

    codec_mgr_i2s_set(entry);
}

static void codec_mgr_aux_in_disable(T_AUDIO_ROUTE_ENTRY *entry)
{
    T_CODEC_ADC_CHANNEL_SEL ch_sel;
    T_CODEC_AUX_CHANNEL_SEL aux_sel;

    ch_sel = (T_CODEC_ADC_CHANNEL_SEL)(entry->endpoint_attr.aux_in.adc_ch);
    aux_sel = (T_CODEC_AUX_CHANNEL_SEL)(entry->endpoint_attr.aux_in.id);

    if (codec_mgr_db->adc_refcnt[ch_sel] != 0)
    {
        (codec_mgr_db->adc_refcnt[ch_sel])--;
    }

    if (codec_mgr_db->adc_refcnt[ch_sel] == 0)
    {
        codec_drv_adc_config_clear(ch_sel);
    }

    if (codec_mgr_db->aux_in_refcnt[aux_sel] != 0)
    {
        (codec_mgr_db->aux_in_refcnt[aux_sel])--;
    }

    if (codec_mgr_db->aux_in_refcnt[aux_sel] == 0)
    {
        codec_drv_aux_config_clear(aux_sel);
    }
}

static void codec_mgr_adc_gain_mute_action(T_CODEC_MGR_SESSION *session,
                                           T_AUDIO_ROUTE_ENTRY *entry)
{
    T_CODEC_ADC_CHANNEL_SEL ch_sel;

    ch_sel = (T_CODEC_ADC_CHANNEL_SEL)(entry->endpoint_attr.mic.adc_ch);

    CODEC_PRINT_TRACE2("codec_mgr_adc_gain_mute_action: ch_sel %d, endpoint_type %d",
                       ch_sel, entry->endpoint_type);

    if ((entry->endpoint_type == AUDIO_ROUTE_ENDPOINT_MIC) ||
        (entry->endpoint_type == AUDIO_ROUTE_ENDPOINT_AUX))
    {
        if (codec_mgr_is_audio_mic(entry->io_type))
        {
            T_AUDIO_ROUTE_DAC_CHANN dac_ch;
            T_CODEC_MGR_LOOPBACK *loopback;

            dac_ch = loopback_map[entry->endpoint_attr.mic.adc_ch];

            loopback = &(codec_mgr_db->loopback_chann[dac_ch]);

            if ((loopback->adc_loopback == true) && (loopback->adc_ch == ch_sel))
            {
                loopback->adc_loopback = false;
            }

            if (codec_mgr_db->sidetone_status == false)
            {
                codec_mgr_mic_disable(session, entry);
            }
        }
    }
}

static void codec_mgr_adc_gain_set_action(T_CODEC_MGR_SESSION *session,
                                          T_AUDIO_ROUTE_ENTRY *entry)
{
    T_CODEC_ADC_CHANNEL_SEL ch_sel;

    if (entry->endpoint_type == AUDIO_ROUTE_ENDPOINT_MIC)
    {
        if (entry->endpoint_attr.mic.id != AUDIO_ROUTE_EXT_MIC)
        {
        uint8_t mic_sel, mic_src;

        mic_sel = codec_drv_get_mic_ch_sel(entry->endpoint_attr.mic.id, &mic_src);
        ch_sel = (T_CODEC_ADC_CHANNEL_SEL)(entry->endpoint_attr.mic.adc_ch);

        if (codec_mgr_is_audio_mic(entry->io_type))
        {
            if (codec_mgr_db->adc_status[session->category].count !=
                codec_mgr_db->adc_status[session->category].total)
            {
                codec_mgr_adc_config_set(entry, session);
                codec_mgr_loopback_enable();
            }

            if (codec_mgr_is_mic(entry->io_type))
            {
                if (mic_src == AD_SRC_AMIC)
                {
                    codec_mgr_amic_gain_set((T_CODEC_AMIC_CHANNEL_SEL)mic_sel,
                                            (T_CODEC_ADC_ANA_GAIN)entry->endpoint_attr.mic.ana_gain);
                }
            }

            codec_mgr_adc_gain_set(ch_sel,
                                   (T_CODEC_ADC_BOOST_GAIN)entry->endpoint_attr.mic.dig_boost_gain,
                                   entry->endpoint_attr.mic.dig_gain);
        }
        else if (codec_mgr_is_anc_mic(entry->io_type))
        {
            if (mic_src == AD_SRC_AMIC)
            {
                codec_mgr_amic_gain_set((T_CODEC_AMIC_CHANNEL_SEL)mic_sel,
                                        (T_CODEC_ADC_ANA_GAIN)entry->endpoint_attr.mic.ana_gain);
                }
            }
        }
    }
    else if (entry->endpoint_type == AUDIO_ROUTE_ENDPOINT_AUX)
    {
        T_CODEC_AUX_CHANNEL_SEL aux_sel;

        ch_sel = (T_CODEC_ADC_CHANNEL_SEL)(entry->endpoint_attr.aux_in.adc_ch);
        if (codec_mgr_db->adc_status[session->category].count !=
            codec_mgr_db->adc_status[session->category].total)
        {
            codec_mgr_adc_config_set(entry, session);
            codec_mgr_loopback_enable();
        }

        aux_sel = (T_CODEC_AUX_CHANNEL_SEL)(entry->endpoint_attr.aux_in.id);
        codec_mgr_aux_in_gain_set(aux_sel, (T_CODEC_ADC_ANA_GAIN)entry->endpoint_attr.aux_in.ana_gain);

        codec_mgr_adc_gain_set(ch_sel,
                               (T_CODEC_ADC_BOOST_GAIN)entry->endpoint_attr.aux_in.dig_boost_gain,
                               entry->endpoint_attr.aux_in.dig_gain);
    }
}

static void codec_mgr_adc_gain_config(T_CODEC_MGR_SESSION_HANDLE handle,
                                      P_ADC_GAIN_SET_ACTION      action)
{
    T_CODEC_MGR_SESSION *session = (T_CODEC_MGR_SESSION *)handle;
    T_AUDIO_ROUTE_PATH   path;

    path = audio_route_path_get(session->context, session->category, session->device);
    if (path.entry != NULL)
    {
        for (uint8_t i = 0; i < path.entry_num; i++)
        {
            action(session, &path.entry[i]);
        }

        audio_route_path_give(&path);
    }
}

void codec_mgr_session_adc_gain_mute(T_CODEC_MGR_SESSION_HANDLE handle)
{
    codec_mgr_adc_gain_config(handle, codec_mgr_adc_gain_mute_action);
}

void codec_mgr_session_adc_gain_set(T_CODEC_MGR_SESSION_HANDLE handle)
{
    codec_mgr_adc_gain_config(handle, codec_mgr_adc_gain_set_action);
}

void codec_mgr_session_dac_gain_set(T_CODEC_MGR_SESSION_HANDLE handle)
{
    T_CODEC_MGR_SESSION     *session;
    T_AUDIO_ROUTE_PATH       path;
    T_AUDIO_ROUTE_DAC_CHANN  dac_ch;


    session = (T_CODEC_MGR_SESSION *)handle;
    path = audio_route_path_get(session->context, session->category, session->device);
    if (path.entry != NULL)
    {
        for (uint8_t i = 0; i < path.entry_num; i++)
        {
            if (codec_mgr_is_audio_spk(path.entry[i].io_type))
            {
                // dac setting
                dac_ch = path.entry[i].endpoint_attr.spk.dac_ch;
                codec_mgr_dac_gain_set((T_CODEC_DAC_CHANNEL_SEL)dac_ch,
                                       (T_CODEC_DAC_ANA_GAIN)path.entry[i].endpoint_attr.spk.ana_gain,
                                       path.entry[i].endpoint_attr.spk.dig_gain);
            }
        }

        audio_route_path_give(&path);
    }
}

void codec_mgr_path_enable(T_CODEC_MGR_SESSION *session)
{
    T_AUDIO_ROUTE_PATH path;

    path = audio_route_path_get(session->context, session->category, session->device);
    if (path.entry != NULL)
    {
        uint8_t i;

        for (i = 0; i < path.entry_num; i++)
        {
            if (path.entry[i].endpoint_type == AUDIO_ROUTE_ENDPOINT_MIC)
            {
                if (path.entry[i].gateway_attr.sample_rate)
                {
                    session->adc_sample_rate = path.entry[i].gateway_attr.sample_rate;
                }

                if (path.entry[i].endpoint_attr.mic.id != AUDIO_ROUTE_EXT_MIC)
                {
                    codec_mgr_mic_enable(&path.entry[i], session);
                    codec_mgr_db->adc_status[session->category].total++;
                }
            }
            else if (path.entry[i].endpoint_type == AUDIO_ROUTE_ENDPOINT_SPK)
            {
                if (path.entry[i].gateway_attr.sample_rate)
                {
                    session->dac_sample_rate = path.entry[i].gateway_attr.sample_rate;
                }

                if (path.entry[i].endpoint_attr.spk.id != AUDIO_ROUTE_EXT_SPK)
                {
                    codec_mgr_spk_enable(&path.entry[i], session);
                }
            }
            else if (path.entry[i].endpoint_type == AUDIO_ROUTE_ENDPOINT_AUX)
            {
                if (path.entry[i].gateway_attr.sample_rate)
                {
                    session->adc_sample_rate = path.entry[i].gateway_attr.sample_rate;
                }

                codec_mgr_aux_in_enable(&path.entry[i], session);
            }
        }

        audio_route_path_give(&path);
    }

    CODEC_PRINT_TRACE4("codec_mgr_path_enable: category 0x%02x, path_num %d, adc_sample_rate %d, dac_sample_rate %d",
                       session->category, path.entry_num, session->adc_sample_rate, session->dac_sample_rate);

    codec_mgr_mix_enable();
    codec_mgr_loopback_enable();
}

void codec_mgr_path_disable(T_CODEC_MGR_SESSION *session)
{
    T_AUDIO_ROUTE_PATH path;

    path = audio_route_path_get(session->context, session->category, session->device);
    if (path.entry != NULL)
    {
        for (uint8_t i = 0; i < path.entry_num; i++)
        {
            if (path.entry[i].endpoint_type == AUDIO_ROUTE_ENDPOINT_MIC)
            {
                if (path.entry[i].endpoint_attr.mic.id != AUDIO_ROUTE_EXT_MIC)
                {
                    codec_mgr_mic_disable(session, &path.entry[i]);
                }
            }
            else if (path.entry[i].endpoint_type == AUDIO_ROUTE_ENDPOINT_SPK)
            {
                if (path.entry[i].endpoint_attr.spk.id != AUDIO_ROUTE_EXT_SPK)
                {
                    codec_mgr_spk_disable(session, &path.entry[i]);
                }
            }
            else if (path.entry[i].endpoint_type == AUDIO_ROUTE_ENDPOINT_AUX)
            {
                codec_mgr_aux_in_disable(&path.entry[i]);
            }
        }

        audio_route_path_give(&path);
    }

    codec_mgr_mix_disable();
    codec_mgr_loopback_disable();

    codec_mgr_db->adc_status[session->category].total = 0;
}

void codec_mgr_bias_setting(void)
{
    T_CODEC_BIAS_CONFIG bias_config;

    codec_drv_config_init(CODEC_CONFIG_SEL_BIAS, &bias_config);
    bias_config.gpio_mode = audio_pad_check_bias_gpio_mode(audio_route_cfg.avcc_drv_voltage_sel,
                                                           audio_route_cfg.micbias_voltage_sel);
    bias_config.micbias_voltage = (T_CODEC_MICBIAS_VOLT)audio_route_cfg.micbias_voltage_sel;
    bias_config.avcc_drv_voltage = (T_CODEC_AVCC_DRV_VOLT)audio_route_cfg.avcc_drv_voltage_sel;
    codec_drv_bias_config_set(&bias_config);
}

static void codec_mgr_timer_cback(T_SYS_TIMER_HANDLE handle)
{
    if (codec_mgr_is_idle())
    {
        codec_mgr_event_post(CODEC_MGR_EVENT_DISABLE);
    }
}

void codec_mgr_lpm_set(bool enable)
{
    if (enable)
    {
        sys_timer_start(codec_mgr_db->power_off_timer);
    }
    else
    {
        sys_timer_stop(codec_mgr_db->power_off_timer);
    }

    codec_mgr_db->lps_en = enable;
}

bool codec_mgr_init(void)
{
    int32_t ret = 0;

    codec_mgr_db = calloc(1, sizeof(T_CODEC_MGR_DB));
    if (codec_mgr_db == NULL)
    {
        ret = 1;
        goto fail_alloc_codec_mgr_db;
    }

    codec_mgr_db->operation_fsm = fsm_init(operation_fsm_trans_table, CODEC_MGR_OP_STATE_IDLE,
                                           "codec_mgr_operation_fsm", NULL);
    if (codec_mgr_db->operation_fsm == NULL)
    {
        ret = 2;
        goto fail_init_codec_mgr_operation_fsm;
    }

    codec_mgr_db->codec_fsm = fsm_init(codec_mgr_fsm_trans_table, CODEC_MGR_STATE_POWER_OFF,
                                       "codec_mgr_fsm", NULL);
    if (codec_mgr_db->codec_fsm == NULL)
    {
        ret = 3;
        goto fail_init_codec_mgr_fsm;
    }

    if (os_msg_queue_create(&codec_mgr_db->msg_handle,
                            "CODEC_Q",
                            CODEC_MSG_MAX_NUM,
                            sizeof(T_CODEC_SCHED_MSG)) == false)
    {
        ret = 4;
        goto fail_create_queue;
    }

    if (codec_drv_init(codec_mgr_cback) == false)
    {
        ret = 5;
        goto fail_init_codec;
    }

    codec_mgr_db->power_off_timer = sys_timer_create("codec_power_off",
                                                     SYS_TIMER_TYPE_LOW_PRECISION,
                                                     0,
                                                     TIMER_CODEC_POWER_DOWN * 1000,
                                                     false,
                                                     codec_mgr_timer_cback);
    if (codec_mgr_db->power_off_timer == NULL)
    {
        ret = 6;
        goto fail_create_timer;
    }

    sys_mgr_event_register(EVENT_CODEC_MSG, codec_mgr_msg_handler);

    codec_mgr_db->lps_en = true;
    os_queue_init(&(codec_mgr_db->session_queue));
    os_queue_init(&(codec_mgr_db->operation_queue));

    codec_mgr_db->loopback_chann[AUDIO_ROUTE_DAC_CHANN0].adc_ch = AUDIO_ROUTE_ADC_CHANN_NUM;
    codec_mgr_db->loopback_chann[AUDIO_ROUTE_DAC_CHANN1].adc_ch = AUDIO_ROUTE_ADC_CHANN_NUM;
    codec_mgr_db->loopback_chann[AUDIO_ROUTE_DAC_CHANN2].adc_ch = AUDIO_ROUTE_ADC_CHANN_NUM;
    codec_mgr_db->loopback_chann[AUDIO_ROUTE_DAC_CHANN3].adc_ch = AUDIO_ROUTE_ADC_CHANN_NUM;

    return true;

fail_create_timer:
    codec_drv_deinit();
fail_init_codec:
    os_msg_queue_delete(codec_mgr_db->msg_handle);
fail_create_queue:
    fsm_deinit(codec_mgr_db->codec_fsm);
fail_init_codec_mgr_fsm:
    fsm_deinit(codec_mgr_db->operation_fsm);
fail_init_codec_mgr_operation_fsm:
    free(codec_mgr_db);
    codec_mgr_db = NULL;
fail_alloc_codec_mgr_db:
    CODEC_PRINT_ERROR1("codec_mgr_init: failed %d", -ret);
    return false;
}

void codec_mgr_deinit(void)
{
    if (codec_mgr_db != NULL)
    {
        T_CODEC_MGR_SESSION *session;

        session = os_queue_out(&(codec_mgr_db->session_queue));
        while (session != NULL)
        {
            fsm_deinit(session->session_fsm);
            free(session);
            session = os_queue_out(&(codec_mgr_db->session_queue));
        }

        if (codec_mgr_db->msg_handle != NULL)
        {
            os_msg_queue_delete(codec_mgr_db->msg_handle);
        }

        if (codec_mgr_db->power_off_timer != NULL)
        {
            sys_timer_delete(codec_mgr_db->power_off_timer);
        }

        fsm_deinit(codec_mgr_db->codec_fsm);
        fsm_deinit(codec_mgr_db->operation_fsm);
        free(codec_mgr_db);
        codec_mgr_db = NULL;
    }
}

bool codec_mgr_sidetone_set(int16_t gain,
                            uint8_t level,
                            uint8_t enable)
{
    uint8_t boost_gain, dig_gain;
    T_CODEC_SIDETONE_CONFIG sidetone_config;

    codec_drv_config_init(CODEC_CONFIG_SEL_SIDETONE, (void *)&sidetone_config);
    codec_drv_sidetone_gain_convert(CODEC_SIDETONE_TYPE_5M, gain, &boost_gain, &dig_gain);

    CODEC_PRINT_TRACE5("codec_mgr_sidetone_set: gain %d, level %d, "
                       "boost_gain: 0x%02x, dig_gain: 0x%02x, enable 0x%x", gain,
                       level, boost_gain, dig_gain, enable);

    if (enable)
    {
        codec_mgr_db->sidetone_status = true;

        sidetone_config.enable = 1;
        sidetone_config.dig_gain.val = dig_gain;
        sidetone_config.boost_gain = (T_CODEC_SIDETONE_BOOST_GAIN)boost_gain;
        sidetone_config.src = (T_CODEC_ADC_CHANNEL_SEL)audio_route_cfg.voice_primary_in_mic_adc_ch;
        sidetone_config.type = CODEC_SIDETONE_TYPE_5M;
        sidetone_config.eq_en = 0;
        sidetone_config.hpf_en = 1;
        sidetone_config.hpf_fc_sel = level;
        sidetone_config.input = CODEC_SIDETONE_INPUT_L;
        codec_drv_sidetone_config_set(SIDETONE_CHANNEL_L, &sidetone_config);
        codec_drv_sidetone_config_set(SIDETONE_CHANNEL_R, &sidetone_config);

        codec_drv_sidetone_enable();
    }
    else
    {
        codec_mgr_db->sidetone_status = false;

        sidetone_config.enable = 0;
        codec_drv_sidetone_config_set(SIDETONE_CHANNEL_L, &sidetone_config);
        codec_drv_sidetone_config_set(SIDETONE_CHANNEL_R, &sidetone_config);

        codec_drv_sidetone_disable();
    }

    return true;
}

static int8_t codec_mgr_session_cfg_set(void *context)
{
    T_CODEC_MGR_SESSION *session = (T_CODEC_MGR_SESSION *)context;

    codec_mgr_bias_setting();
    codec_mgr_path_enable(session);
    codec_mgr_session_adc_gain_set(session);
    codec_mgr_session_dac_gain_set(session);
    return CODEC_MGR_SESSION_STATE_ENABLING;
}

static int8_t codec_mgr_session_eq_set(void *context)
{
    T_CODEC_MGR_SESSION *session = (T_CODEC_MGR_SESSION *)context;
    T_AUDIO_ROUTE_PATH   path;

    path = audio_route_path_get(session->context, session->category, session->device);

    CODEC_PRINT_TRACE2("codec_mgr_session_eq_set: category %d, path_num %d",
                       session->category, path.entry_num);

    if (path.entry != NULL)
    {
        uint8_t i;
        uint8_t chan;

        for (i = 0; i < path.entry_num; i++)
        {
            if (path.entry[i].endpoint_type == AUDIO_ROUTE_ENDPOINT_MIC)
            {
                if (path.entry[i].endpoint_attr.mic.id != AUDIO_ROUTE_EXT_MIC)
                {
                    chan = path.entry[i].endpoint_attr.mic.adc_ch;

                    if (codec_mgr_is_audio_mic(path.entry[i].io_type) &&
                        (codec_mgr_db->adc_refcnt[chan] == 0))
                    {
                        audio_route_hw_eq_set(AUDIO_ROUTE_HW_EQ_MIC, chan, session->adc_sample_rate);
                    }
                }
            }
            else if (path.entry[i].endpoint_type == AUDIO_ROUTE_ENDPOINT_SPK)
            {
                chan = path.entry[i].endpoint_attr.spk.dac_ch;

                if (path.entry[i].endpoint_attr.spk.id != AUDIO_ROUTE_EXT_SPK)
                {
                    if (codec_mgr_db->dac_fs_refcnt[chan] == 0)
                    {
                        audio_route_hw_eq_set(AUDIO_ROUTE_HW_EQ_SPK, chan, session->dac_sample_rate);
                    }
                }
            }
            else if (path.entry[i].endpoint_type == AUDIO_ROUTE_ENDPOINT_AUX)
            {
                chan = path.entry[i].endpoint_attr.mic.adc_ch;

                if (codec_mgr_db->adc_refcnt[chan] == 0)
                {
                    audio_route_hw_eq_set(AUDIO_ROUTE_HW_EQ_MIC, chan, session->adc_sample_rate);
                }
            }
        }

        audio_route_path_give(&path);
    }

    return CODEC_MGR_SESSION_STATE_ENABLED;
}

bool codec_mgr_hw_eq_apply(T_AUDIO_ROUTE_HW_EQ_TYPE  type,
                           uint8_t                   chann,
                           uint8_t                  *buf,
                           uint16_t                  len)
{
    return codec_drv_eq_data_set(type, chann, buf, len);
}

static int8_t codec_mgr_session_cfg_clear(void *context)
{
    codec_mgr_path_disable((T_CODEC_MGR_SESSION *)context);

    return CODEC_MGR_SESSION_STATE_DISABLING;
}
