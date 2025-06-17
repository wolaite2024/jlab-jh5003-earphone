/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "os_queue.h"
#include "os_msg.h"
#include "trace.h"
#include "sys_mgr.h"
#include "audio_mgr.h"
#include "sport_driver.h"
#include "spdif_driver.h"
#include "vadbuf_driver.h"
#include "audio_route_cfg.h"
#include "gateway.h"

/* TODO Remove Start */
#include "app_msg.h"

#define GATEWAY_MSG_MAX_NUM (0x08)

extern void *hEventQueueHandleAu;
/* TODO Remove End */

typedef enum t_gateway_msg_type
{
    GATEWAY_MSG_DATA_IND = 0x00,
} T_GATEWAY_MSG_TYPE;

typedef struct t_gateway_instance
{
    T_AUDIO_ROUTE_GATEWAY_TYPE type;
    T_AUDIO_ROUTE_GATEWAY_IDX  id;
    uint8_t                    direction;
    uint8_t                    tx_ref;
    uint8_t                    rx_ref;
    uint32_t                   tx_sample_rate;
    uint32_t                   rx_sample_rate;
} T_GATEWAY_INSTANCE;

typedef struct t_gateway_sched_msg
{
    uint8_t  type;
    void    *msg;
} T_GATEWAY_SCHED_MSG;

typedef struct t_gateway_session
{
    struct t_gateway_session *next;
    void                     *context;
    T_AUDIO_CATEGORY          category;
    T_VAD_TYPE                vad_type;
    uint16_t                  vad_frame_size;
    uint32_t                  device;
    uint32_t                  tx_sample_rate;
    uint32_t                  rx_sample_rate;
    T_GATEWAY_CBACK           cback;
} T_GATEWAY_SESSION;

typedef struct t_gateway_db
{
    void               *msg_handle;
    T_OS_QUEUE          session_list;
    uint8_t             gateway_num;
    T_GATEWAY_INSTANCE *gateway;
} T_GATEWAY_DB;

static T_GATEWAY_DB gateway_db;

static const float mclk_rate_map[] =
{
    [AUDIO_ROUTE_MCLK_RATE_1P024MHZ]   = 1024.0f,
    [AUDIO_ROUTE_MCLK_RATE_1P4112MHZ]  = 1411.2f,
    [AUDIO_ROUTE_MCLK_RATE_2P048MHZ]   = 2048.0f,
    [AUDIO_ROUTE_MCLK_RATE_2P8224MHZ]  = 2822.4f,
    [AUDIO_ROUTE_MCLK_RATE_3P072MHZ]   = 3072.0f,
    [AUDIO_ROUTE_MCLK_RATE_4P096MHZ]   = 4096.0f,
    [AUDIO_ROUTE_MCLK_RATE_5P6448MHZ]  = 5644.8f,
    [AUDIO_ROUTE_MCLK_RATE_6P144MHZ]   = 6144.0f,
    [AUDIO_ROUTE_MCLK_RATE_8P192MHZ]   = 8192.0f,
    [AUDIO_ROUTE_MCLK_RATE_11P2896MHZ] = 11289.6f,
    [AUDIO_ROUTE_MCLK_RATE_12P288MHZ]  = 12288.0f,
    [AUDIO_ROUTE_MCLK_RATE_16P384MHZ]  = 16384.0f,
    [AUDIO_ROUTE_MCLK_RATE_22P5792MHZ] = 22579.2f,
    [AUDIO_ROUTE_MCLK_RATE_24P576MHZ]  = 24576.0f,
    [AUDIO_ROUTE_MCLK_RATE_32P768MHZ]  = 32768.0f,
};

static void gateway_msg_send(T_GATEWAY_SCHED_MSG *gateway_msg)
{
    unsigned char event = EVENT_GATEWAY_MSG;

    if (hEventQueueHandleAu != NULL)
    {
        if (os_msg_send(gateway_db.msg_handle, gateway_msg, 0) == true)
        {
            (void)os_msg_send(hEventQueueHandleAu, &event, 0);
        }
    }
}

static T_GATEWAY_INSTANCE *gateway_instance_get(T_AUDIO_ROUTE_GATEWAY_TYPE type,
                                                T_AUDIO_ROUTE_GATEWAY_IDX  id)
{
    T_GATEWAY_INSTANCE *instance = NULL;
    uint8_t             i;

    for (i = 0; i < gateway_db.gateway_num; i++)
    {
        if ((gateway_db.gateway[i].type == type) && (gateway_db.gateway[i].id == id))
        {
            instance = &gateway_db.gateway[i];
            break;
        }
    }

    return instance;
}

uint32_t gateway_session_sample_rate_get(T_GATEWAY_SESSION_HANDLE   handle,
                                         T_AUDIO_ROUTE_GATEWAY_TYPE type,
                                         T_AUDIO_ROUTE_GATEWAY_IDX  id,
                                         T_AUDIO_ROUTE_GATEWAY_DIR  dir)
{
    T_GATEWAY_SESSION  *session;
    T_GATEWAY_INSTANCE *instance = NULL;
    uint32_t            sample_rate = 0;

    session = (T_GATEWAY_SESSION *)handle;

    if (session != NULL)
    {
        instance = gateway_instance_get(type, id);
        if (instance != NULL)
        {
            if (dir == AUDIO_ROUTE_GATEWAY_DIR_TX)
            {
                sample_rate = instance->tx_sample_rate;
            }
            else
            {
                sample_rate = instance->rx_sample_rate;
            }
        }
    }

    return sample_rate;
}

static T_GATEWAY_SESSION *gateway_session_find(T_AUDIO_CATEGORY category)
{
    T_GATEWAY_SESSION *session;

    session = os_queue_peek(&gateway_db.session_list, 0);
    while (session != NULL)
    {
        if (session->category == category)
        {
            return session;
        }

        session = session->next;
    }

    return NULL;
}

void gateway_vadbuf_cback(T_VADBUF_DRV_EVENT event, void *param)
{
    T_GATEWAY_SESSION *session;

    session = gateway_session_find(AUDIO_CATEGORY_VAD);
    if (session != NULL)
    {
        switch (event)
        {
        case VADBUF_DRV_EVENT_DISABLED:
            {

            }
            break;

        case VADBUF_DRV_EVENT_ENABLED:
            {

            }
            break;

        case VADBUF_DRV_EVENT_SUSPENDED:
            {
                session->cback(GATEWAY_EVENT_SUSPEND, session->context);
            }
            break;

        case VADBUF_DRV_EVENT_RESUMED:
            {
                session->cback(GATEWAY_EVENT_RESUME, session->context);
            }
            break;

        case VADBUF_DRV_EVENT_DATA_IND:
            {
                T_GATEWAY_SCHED_MSG gateway_msg;

                gateway_msg.type = GATEWAY_MSG_DATA_IND;
                gateway_msg.msg  = session;

                gateway_msg_send(&gateway_msg);
            }
            break;
        }
    }
}

static void gateway_msg_handler(void)
{
    T_GATEWAY_SCHED_MSG  gateway_msg;

    if (os_msg_recv(gateway_db.msg_handle, &gateway_msg, 0) == true)
    {
        switch (gateway_msg.type)
        {
        case GATEWAY_MSG_DATA_IND:
            {
                T_GATEWAY_SESSION *session = (T_GATEWAY_SESSION *)gateway_msg.msg;

                session->cback(GATEWAY_EVENT_DATA_IND, session->context);
            }
            break;

        default:
            break;
        }
    }
}

bool gateway_init(void)
{
    T_GATEWAY_INSTANCE *instance;
    int32_t             ret = 0;

    if (sport_drv_init() == false)
    {
        ret = 1;
        goto fail_init_sport;
    }

    if (spdif_drv_init() == false)
    {
        ret = 2;
        goto fail_init_spdif;
    }

    if (vadbuf_drv_init(gateway_vadbuf_cback) == false)
    {
        ret = 3;
        goto fail_init_vadbuf;
    }

    gateway_db.gateway_num  = 0;
    gateway_db.gateway_num += audio_route_cfg.sport0_enable;
    gateway_db.gateway_num += audio_route_cfg.sport1_enable;
    gateway_db.gateway_num += audio_route_cfg.sport2_enable;
    gateway_db.gateway_num += audio_route_cfg.sport3_enable;
    gateway_db.gateway_num += audio_route_cfg.spdif0_enable;
    gateway_db.gateway_num += audio_route_cfg.vad0_enable;
    if (gateway_db.gateway_num == 0)
    {
        ret = 4;
        goto fail_invalid_gateway_num;
    }

    gateway_db.gateway = calloc(gateway_db.gateway_num, sizeof(T_GATEWAY_INSTANCE));
    if (gateway_db.gateway == NULL)
    {
        ret = 5;
        goto fail_alloc_gateway_instance;
    }

    instance = gateway_db.gateway;

    if (audio_route_cfg.sport0_enable)
    {
        instance->type           = AUDIO_ROUTE_GATEWAY_SPORT;
        instance->id             = AUDIO_ROUTE_GATEWAY_0;
        instance->tx_ref         = 0;
        instance->tx_sample_rate = 16000;
        instance->rx_ref         = 0;
        instance->rx_sample_rate = 16000;
        instance++;
    }

    if (audio_route_cfg.sport1_enable)
    {
        instance->type           = AUDIO_ROUTE_GATEWAY_SPORT;
        instance->id             = AUDIO_ROUTE_GATEWAY_1;
        instance->tx_ref         = 0;
        instance->tx_sample_rate = 16000;
        instance->rx_ref         = 0;
        instance->rx_sample_rate = 16000;
        instance++;
    }

    if (audio_route_cfg.sport2_enable)
    {
        instance->type           = AUDIO_ROUTE_GATEWAY_SPORT;
        instance->id             = AUDIO_ROUTE_GATEWAY_2;
        instance->tx_ref         = 0;
        instance->tx_sample_rate = 16000;
        instance->rx_ref         = 0;
        instance->rx_sample_rate = 16000;
        instance++;
    }

    if (audio_route_cfg.sport3_enable)
    {
        instance->type           = AUDIO_ROUTE_GATEWAY_SPORT;
        instance->id             = AUDIO_ROUTE_GATEWAY_3;
        instance->tx_ref         = 0;
        instance->tx_sample_rate = 16000;
        instance->rx_ref         = 0;
        instance->rx_sample_rate = 16000;
        instance++;
    }

    if (audio_route_cfg.spdif0_enable)
    {
        instance->type           = AUDIO_ROUTE_GATEWAY_SPDIF;
        instance->id             = AUDIO_ROUTE_GATEWAY_0;
        instance->tx_ref         = 0;
        instance->tx_sample_rate = 16000;
        instance->rx_ref         = 0;
        instance->rx_sample_rate = 16000;
        instance++;
    }

    if (audio_route_cfg.vad0_enable)
    {
        instance->type           = AUDIO_ROUTE_GATEWAY_VAD;
        instance->id             = AUDIO_ROUTE_GATEWAY_0;
        instance->tx_ref         = 0;
        instance->tx_sample_rate = 16000;
        instance->rx_ref         = 0;
        instance->rx_sample_rate = 16000;
    }

    if (os_msg_queue_create(&gateway_db.msg_handle,
                            "GATEWAY_Q",
                            GATEWAY_MSG_MAX_NUM,
                            sizeof(T_GATEWAY_SCHED_MSG)) == false)
    {
        ret = 6;
        goto fail_create_queue;
    }

    sys_mgr_event_register(EVENT_GATEWAY_MSG, gateway_msg_handler);

    os_queue_init(&gateway_db.session_list);

    return true;

fail_create_queue:
    free(gateway_db.gateway);
    gateway_db.gateway = NULL;
fail_alloc_gateway_instance:
fail_invalid_gateway_num:
    vadbuf_drv_deinit();
fail_init_vadbuf:
    spdif_drv_deinit();
fail_init_spdif:
    sport_drv_deinit();
fail_init_sport:
    DIPC_PRINT_ERROR1("gateway_init: failed %d", -ret);
    return false;
}

void gateway_deinit(void)
{
    T_GATEWAY_SESSION *session;

    session = os_queue_out(&gateway_db.session_list);
    while (session != NULL)
    {
        free(session);
        session = os_queue_out(&gateway_db.session_list);
    }

    if (gateway_db.msg_handle != NULL)
    {
        os_msg_queue_delete(gateway_db.msg_handle);
    }

    if (gateway_db.gateway != NULL)
    {
        free(gateway_db.gateway);
    }
}

static void sport_param_set(T_AUDIO_ROUTE_GATEWAY_ATTR *attr)
{
    T_GATEWAY_INSTANCE *instance;

    instance = gateway_instance_get(attr->type, attr->idx);
    if (instance != NULL)
    {
        if (attr->dir == AUDIO_ROUTE_GATEWAY_DIR_TX)
        {
            DIPC_PRINT_TRACE6("sport_param_set: id %d, tx_tdm_mode %d, tx_fifo_mode %d, tx_data_length %d, tx_chann_len %d, tx_sample_rate %d",
                              attr->idx, attr->chann_mode, attr->fifo_mode,
                              attr->data_len, attr->chann_len, instance->tx_sample_rate);

            sport_drv_tx_config((T_SPORT_ID)attr->idx,
                                (T_SPORT_TDM_MODE)attr->chann_mode,
                                (T_SPORT_FORMAT_TYPE)attr->format,
                                (T_SPORT_CHANNEL_MODE)attr->fifo_mode,
                                (T_SPORT_CHANNEL_LEN)attr->chann_len,
                                (T_SPORT_DATA_LEN)attr->data_len,
                                instance->tx_sample_rate);

            if (instance->rx_ref == 0)
            {
                if (attr->bridge)
                {
                    instance->rx_sample_rate = instance->tx_sample_rate;

                    sport_drv_rx_config((T_SPORT_ID)attr->idx,
                                        (T_SPORT_TDM_MODE)attr->chann_mode,
                                        (T_SPORT_FORMAT_TYPE)attr->format,
                                        (T_SPORT_CHANNEL_MODE)attr->fifo_mode,
                                        (T_SPORT_CHANNEL_LEN)attr->chann_len,
                                        (T_SPORT_DATA_LEN)attr->data_len,
                                        instance->rx_sample_rate);
                }
            }
        }

        if (attr->dir == AUDIO_ROUTE_GATEWAY_DIR_RX)
        {
            DIPC_PRINT_TRACE6("sport_param_set: id %d, rx_tdm_mode %d, rx_fifo_mode %d, rx_data_length %d, rx_chann_len %d, rx_sample_rate %d",
                              attr->idx, attr->chann_mode, attr->fifo_mode,
                              attr->data_len, attr->chann_len, instance->rx_sample_rate);

            sport_drv_rx_config((T_SPORT_ID)attr->idx,
                                (T_SPORT_TDM_MODE)attr->chann_mode,
                                (T_SPORT_FORMAT_TYPE)attr->format,
                                (T_SPORT_CHANNEL_MODE)attr->fifo_mode,
                                (T_SPORT_CHANNEL_LEN)attr->chann_len,
                                (T_SPORT_DATA_LEN)attr->data_len,
                                instance->rx_sample_rate);

            if (instance->tx_ref == 0)
            {
                if (attr->bridge)
                {
                    instance->tx_sample_rate = instance->rx_sample_rate;
                    sport_drv_tx_config((T_SPORT_ID)attr->idx,
                                        (T_SPORT_TDM_MODE)attr->chann_mode,
                                        (T_SPORT_FORMAT_TYPE)attr->format,
                                        (T_SPORT_CHANNEL_MODE)attr->fifo_mode,
                                        (T_SPORT_CHANNEL_LEN)attr->chann_len,
                                        (T_SPORT_DATA_LEN)attr->data_len,
                                        instance->tx_sample_rate);
                }
            }
        }
    }
}

static void spdif_param_set(T_AUDIO_ROUTE_GATEWAY_ATTR *attr)
{
    T_GATEWAY_INSTANCE *instance;

    instance = gateway_instance_get(attr->type, attr->idx);
    if (instance != NULL)
    {
        if (attr->dir == AUDIO_ROUTE_GATEWAY_DIR_TX)
        {
            DIPC_PRINT_TRACE4("spdif_param_set: id %d, tx_format_type %d, tx_data_length %d, tx_sample_rate %d",
                              attr->idx, attr->format, attr->data_len, instance->tx_sample_rate);

            spdif_drv_tx_config(instance->tx_sample_rate,
                                (T_SPDIF_FORMAT_TYPE)attr->format,
                                (T_SPDIF_DATA_LEN)attr->data_len);

            if (instance->rx_ref == 0)
            {
                if (attr->bridge)
                {
                    instance->rx_sample_rate = instance->tx_sample_rate;

                    spdif_drv_rx_config((T_SPDIF_DATA_LEN)attr->data_len);
                }
            }
        }

        if (attr->dir == AUDIO_ROUTE_GATEWAY_DIR_RX)
        {
            DIPC_PRINT_TRACE2("spdif_param_set: id %d, rx_data_length %d",
                              attr->idx, attr->data_len);

            spdif_drv_rx_config((T_SPDIF_DATA_LEN)attr->data_len);

            if (instance->tx_ref == 0)
            {
                if (attr->bridge)
                {
                    instance->tx_sample_rate = instance->rx_sample_rate;

                    spdif_drv_tx_config(instance->tx_sample_rate,
                                        (T_SPDIF_FORMAT_TYPE)attr->format,
                                        (T_SPDIF_DATA_LEN)attr->data_len);
                }
            }
        }
    }
}

static void vadbuf_param_set(T_AUDIO_ROUTE_ENTRY *entry)
{
    T_GATEWAY_INSTANCE *instance;
    uint8_t             i;

    instance = gateway_instance_get(entry->gateway_attr.type,
                                    entry->gateway_attr.idx);
    if (instance != NULL)
    {
        DIPC_PRINT_TRACE2("vadbuf_param_set: id %d, rx_data_length %d",
                          entry->gateway_attr.idx, entry->gateway_attr.data_len);

        for (i = AUDIO_ROUTE_GATEWAY_CHANN0; i < AUDIO_ROUTE_GATEWAY_CHANN_NUM; i++)
        {
            if (entry->gateway_attr.channs & (1U << i))
            {
                vadbuf_drv_ch_set((T_VADBUF_DRV_CHANNEL_SEL)i,
                                  (T_VADBUF_DRV_ADC_CHANNEL_SEL)entry->endpoint_attr.mic.adc_ch);
            }
        }
    }
}

static void sport_param_init(T_AUDIO_ROUTE_GATEWAY_ATTR *attr)
{
    DIPC_PRINT_TRACE6("sport_param_init: id %d, role %d, bclk_src %d, bridge %d, mclk_enable %d, mclk_rate %d",
                      attr->idx, attr->role, attr->clk_src, attr->bridge,
                      audio_route_cfg.sport_ext_mclk_enable, audio_route_cfg.sport_ext_mclk_rate);

    if (attr->bridge)
    {
        sport_drv_mclk_config((bool)audio_route_cfg.sport_ext_mclk_enable,
                              mclk_rate_map[audio_route_cfg.sport_ext_mclk_rate]);
    }

    sport_drv_general_config((T_SPORT_ID)attr->idx,
                             (T_SPORT_ROLE)attr->role,
                             (T_SPORT_CLK_SRC)attr->clk_src,
                             (T_SPORT_CODEC_SEL)attr->bridge);
}

static void spdif_param_init(T_AUDIO_ROUTE_GATEWAY_ATTR *attr)
{
    DIPC_PRINT_TRACE2("spdif_param_init: id %d, role %d", attr->idx, attr->role);

    spdif_drv_general_config((T_SPDIF_ROLE)attr->role);
}

static void gateway_sport_enable(T_GATEWAY_SESSION          *session,
                                 T_AUDIO_ROUTE_GATEWAY_ATTR *attr)
{
    T_GATEWAY_INSTANCE *sport_instance;

    sport_instance = gateway_instance_get(attr->type, attr->idx);
    if (sport_instance != NULL)
    {
        if ((sport_instance->rx_ref == 0) && (sport_instance->tx_ref == 0))
        {
            sport_param_init(attr);
        }

        if (attr->dir == AUDIO_ROUTE_GATEWAY_DIR_RX)
        {
            if (sport_instance->rx_ref == 0)
            {
                if (attr->sample_rate)
                {
                    sport_instance->rx_sample_rate = attr->sample_rate;
                }
                else
                {
                    sport_instance->rx_sample_rate = session->rx_sample_rate;
                }

                sport_param_set(attr);
            }

            sport_instance->rx_ref++;
        }

        if (attr->dir == AUDIO_ROUTE_GATEWAY_DIR_TX)
        {
            if (sport_instance->tx_ref == 0)
            {
                if (attr->sample_rate)
                {
                    sport_instance->tx_sample_rate = attr->sample_rate;
                }
                else
                {
                    sport_instance->tx_sample_rate = session->tx_sample_rate;
                }

                sport_param_set(attr);
            }

            sport_instance->tx_ref++;
        }

        if ((sport_instance->tx_ref == 1) || (sport_instance->rx_ref == 1))
        {
            sport_drv_enable((T_SPORT_ID)attr->idx);
        }

        DIPC_PRINT_TRACE4("gateway_sport_enable: id %d, direction %d, tx_ref %d, rx_ref %d",
                          attr->idx, attr->dir, sport_instance->tx_ref, sport_instance->rx_ref);
    }
}

static void gateway_spdif_enable(T_GATEWAY_SESSION          *session,
                                 T_AUDIO_ROUTE_GATEWAY_ATTR *attr)
{
    T_GATEWAY_INSTANCE *spdif_instance;

    spdif_instance = gateway_instance_get(attr->type, attr->idx);
    if (spdif_instance != NULL)
    {
        if ((spdif_instance->rx_ref == 0) && (spdif_instance->tx_ref == 0))
        {
            spdif_param_init(attr);
        }

        if (attr->dir == AUDIO_ROUTE_GATEWAY_DIR_RX)
        {
            if (spdif_instance->rx_ref == 0)
            {
                if (attr->sample_rate)
                {
                    spdif_instance->rx_sample_rate = attr->sample_rate;
                }
                else
                {
                    spdif_instance->rx_sample_rate = session->rx_sample_rate;
                }

                spdif_param_set(attr);
            }

            spdif_instance->rx_ref++;
        }

        if (attr->dir == AUDIO_ROUTE_GATEWAY_DIR_TX)
        {
            if (spdif_instance->tx_ref == 0)
            {
                if (attr->sample_rate)
                {
                    spdif_instance->tx_sample_rate = attr->sample_rate;
                }
                else
                {
                    spdif_instance->tx_sample_rate = session->tx_sample_rate;
                }

                spdif_param_set(attr);
            }

            spdif_instance->tx_ref++;
        }

        if ((spdif_instance->tx_ref == 1) || (spdif_instance->rx_ref) == 1)
        {
            spdif_drv_enable();
        }

        DIPC_PRINT_TRACE4("gateway_spdif_enable: id %d, direction %d, tx_ref %d, rx_ref %d",
                          attr->idx, attr->dir, spdif_instance->tx_ref, spdif_instance->rx_ref);
    }
}

static void gateway_vadbuf_enable(T_GATEWAY_SESSION   *session,
                                  T_AUDIO_ROUTE_ENTRY *entry)
{
    T_GATEWAY_INSTANCE *vadbuf_instance;

    vadbuf_instance = gateway_instance_get(entry->gateway_attr.type,
                                           entry->gateway_attr.idx);
    if (vadbuf_instance != NULL)
    {
        if (vadbuf_instance->rx_ref == 0)
        {
            T_VADBUF_DRV_VAD_TYPE  type;
            T_VADBUF_DRV_DATA_LEN  data_len;
            T_VADBUF_DRV_CHANN_LEN chann_len;

            type = (session->vad_type == VAD_TYPE_HW) ? VADBUF_DRV_HW_VAD : VADBUF_DRV_FUSION_VAD;
            data_len = (entry->gateway_attr.data_len == AUDIO_ROUTE_GATEWAY_DATA_LEN_16BIT) ?
                       VADBUF_DRV_DATA_LEN_16BIT : VADBUF_DRV_DATA_LEN_24BIT;
            chann_len = (entry->gateway_attr.chann_len == AUDIO_ROUTE_GATEWAY_CHANN_LEN_16BIT) ?
                        VADBUF_DRV_CHANN_LEN_16BIT : VADBUF_DRV_CHANN_LEN_32BIT;
            vadbuf_drv_param_set(type,
                                 (T_VADBUF_DRV_CHANNEL_MODE)entry->gateway_attr.chann_mode,
                                 data_len,
                                 chann_len,
                                 session->vad_frame_size);
        }

        if (entry->gateway_attr.dir == AUDIO_ROUTE_GATEWAY_DIR_RX)
        {
            if (vadbuf_instance->rx_ref == 0)
            {
                if (entry->gateway_attr.sample_rate)
                {
                    vadbuf_instance->rx_sample_rate = entry->gateway_attr.sample_rate;
                }
                else
                {
                    vadbuf_instance->rx_sample_rate = session->rx_sample_rate;
                }

                vadbuf_param_set(entry);
            }

            vadbuf_instance->rx_ref++;
        }

        if (vadbuf_instance->rx_ref == 1)
        {
            vadbuf_drv_enable();
        }

        DIPC_PRINT_TRACE3("gateway_vadbuf_enable: id %d, direction %d, rx_ref %d",
                          entry->gateway_attr.idx, entry->gateway_attr.dir, vadbuf_instance->rx_ref);
    }
}

bool gateway_session_enable(T_GATEWAY_SESSION_HANDLE handle)
{
    T_GATEWAY_SESSION *session;

    session = (T_GATEWAY_SESSION *)handle;
    if (session != NULL)
    {
        T_AUDIO_ROUTE_PATH path;

        path = audio_route_path_get(session->context,
                                    session->category,
                                    session->device);
        if (path.entry_num != 0)
        {
            uint8_t i;

            for (i = 0; i < path.entry_num; i++)
            {
                if (path.entry[i].gateway_attr.idx < AUDIO_ROUTE_GATEWAY_NUM)
                {
                    if (path.entry[i].gateway_attr.type == AUDIO_ROUTE_GATEWAY_SPORT)
                    {
                        gateway_sport_enable(session, &path.entry[i].gateway_attr);
                    }
                    else if (path.entry[i].gateway_attr.type == AUDIO_ROUTE_GATEWAY_SPDIF)
                    {
                        gateway_spdif_enable(session, &path.entry[i].gateway_attr);
                    }
                    else if (path.entry[i].gateway_attr.type == AUDIO_ROUTE_GATEWAY_VAD)
                    {
                        gateway_vadbuf_enable(session, &path.entry[i]);
                    }
                }
            }

            audio_route_path_give(&path);
        }

        return true;
    }

    return false;
}

static void gateway_sport_disable(T_GATEWAY_SESSION          *session,
                                  T_AUDIO_ROUTE_GATEWAY_ATTR *attr)
{
    T_GATEWAY_INSTANCE *sport_instance;

    sport_instance = gateway_instance_get(attr->type, attr->idx);
    if (sport_instance != NULL)
    {
        if (attr->dir == AUDIO_ROUTE_GATEWAY_DIR_RX)
        {
            if (sport_instance->rx_ref)
            {
                sport_instance->rx_ref--;
            }
        }

        if (attr->dir == AUDIO_ROUTE_GATEWAY_DIR_TX)
        {
            if (sport_instance->tx_ref)
            {
                sport_instance->tx_ref--;
            }
        }

        if ((sport_instance->rx_ref == 0) &&
            (sport_instance->tx_ref == 0))
        {
            sport_drv_disable((T_SPORT_ID)attr->idx);
        }

        DIPC_PRINT_TRACE4("gateway_sport_disable: id %d, direction %d, tx_ref %d, rx_ref %d",
                          attr->idx, attr->dir, sport_instance->tx_ref, sport_instance->rx_ref);
    }
}

static void gateway_spdif_disable(T_GATEWAY_SESSION          *session,
                                  T_AUDIO_ROUTE_GATEWAY_ATTR *attr)
{
    T_GATEWAY_INSTANCE *spdif_instance;

    spdif_instance = gateway_instance_get(attr->type, attr->idx);
    if (spdif_instance != NULL)
    {
        if (attr->dir == AUDIO_ROUTE_GATEWAY_DIR_RX)
        {
            if (spdif_instance->rx_ref)
            {
                spdif_instance->rx_ref--;
            }
        }

        if (attr->dir == AUDIO_ROUTE_GATEWAY_DIR_TX)
        {
            if (spdif_instance->tx_ref)
            {
                spdif_instance->tx_ref--;
            }
        }

        if ((spdif_instance->rx_ref == 0) &&
            (spdif_instance->tx_ref == 0))
        {
            spdif_drv_disable();
        }

        DIPC_PRINT_TRACE4("gateway_spdif_disable: id %d, direction %d, tx_ref %d, rx_ref %d",
                          attr->idx, attr->dir, spdif_instance->tx_ref, spdif_instance->rx_ref);
    }
}

static void gateway_vadbuf_disable(T_GATEWAY_SESSION          *session,
                                   T_AUDIO_ROUTE_GATEWAY_ATTR *attr)
{
    T_GATEWAY_INSTANCE *vad_instance;

    vad_instance = gateway_instance_get(attr->type, attr->idx);
    if (vad_instance != NULL)
    {
        if (attr->dir == AUDIO_ROUTE_GATEWAY_DIR_RX)
        {
            if (vad_instance->rx_ref)
            {
                vad_instance->rx_ref--;
            }
        }

        if (vad_instance->rx_ref == 0)
        {
            vadbuf_drv_disable();
        }

        DIPC_PRINT_TRACE3("gateway_vadbuf_disable: id %d, direction %d, rx_ref %d",
                          attr->idx, attr->dir, vad_instance->rx_ref);
    }
}

bool gateway_session_disable(T_GATEWAY_SESSION_HANDLE handle)
{
    T_GATEWAY_SESSION *session;

    session = (T_GATEWAY_SESSION *)handle;
    if (session != NULL)
    {
        T_AUDIO_ROUTE_PATH path;

        path = audio_route_path_get(session->context,
                                    session->category,
                                    session->device);
        if (path.entry_num != 0)
        {
            uint8_t i;

            for (i = 0; i < path.entry_num; i++)
            {
                if (path.entry[i].gateway_attr.idx < AUDIO_ROUTE_GATEWAY_NUM)
                {
                    if (path.entry[i].gateway_attr.type == AUDIO_ROUTE_GATEWAY_SPORT)
                    {
                        gateway_sport_disable(session, &path.entry[i].gateway_attr);
                    }
                    else if (path.entry[i].gateway_attr.type == AUDIO_ROUTE_GATEWAY_SPDIF)
                    {
                        gateway_spdif_disable(session, &path.entry[i].gateway_attr);
                    }
                    else if (path.entry[i].gateway_attr.type == AUDIO_ROUTE_GATEWAY_VAD)
                    {
                        gateway_vadbuf_disable(session, &path.entry[i].gateway_attr);
                    }
                }
            }

            audio_route_path_give(&path);
        }

        return true;
    }

    return false;
}

T_GATEWAY_SESSION_HANDLE gateway_session_create(T_AUDIO_CATEGORY  category,
                                                uint32_t          tx_sample_rate,
                                                uint32_t          rx_sample_rate,
                                                uint32_t          device,
                                                T_VAD_TYPE        vad_type,
                                                uint16_t          vad_frame_size,
                                                T_GATEWAY_CBACK   cback,
                                                void             *context)
{
    T_GATEWAY_SESSION *session;

    session = calloc(1, sizeof(T_GATEWAY_SESSION));
    if (session != NULL)
    {
        session->context        = context;
        session->category       = category;
        session->device         = device;
        session->tx_sample_rate = tx_sample_rate;
        session->rx_sample_rate = rx_sample_rate;
        session->cback          = cback;
        session->vad_type       = vad_type;
        session->vad_frame_size = vad_frame_size;

        os_queue_in(&gateway_db.session_list, session);
    }

    return (T_GATEWAY_SESSION_HANDLE)session;
}

bool gateway_session_destroy(T_GATEWAY_SESSION_HANDLE handle)
{
    T_GATEWAY_SESSION *session;

    session = (T_GATEWAY_SESSION *)handle;

    if (session != NULL)
    {
        os_queue_delete(&gateway_db.session_list, session);

        free(session);
        return true;
    }

    return false;
}

uint16_t gateway_session_data_len_peek(T_GATEWAY_SESSION_HANDLE handle)
{
    T_GATEWAY_SESSION *session;

    session = (T_GATEWAY_SESSION *)handle;
    if (os_queue_search(&gateway_db.session_list, session))
    {
        return vadbuf_drv_data_len_peek();
    }

    return 0;
}

uint16_t gateway_session_data_recv(T_GATEWAY_SESSION_HANDLE  handle,
                                   uint8_t                  *buffer,
                                   uint16_t                  length)
{
    T_GATEWAY_SESSION *session;

    session = (T_GATEWAY_SESSION *)handle;
    if (os_queue_search(&gateway_db.session_list, session))
    {
        return vadbuf_drv_data_recv(buffer, length);
    }

    return 0;
}
