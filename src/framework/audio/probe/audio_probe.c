/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "trace.h"
#include "audio_probe.h"
#include "audio_mgr.h"
#include "anc_mgr.h"

/* TODO Remove Start */
#include "sys_mgr.h"

#include "dsp_shm.h"
#include "dsp_ipc.h"
#include "dsp_loader.h"
#include "dsp_mgr.h"
#if (CONFIG_SOC_SERIES_RTL8773D == 1)/*https://jira.realtek.com/browse/BBPRO3RD-801*/
#include "errno.h"
#include "os_msg.h"
#include "os_sync.h"
#include "app_msg.h"
#include "dsp_driver.h"
extern void *hEventQueueHandleAu;
#endif
/* TODO Remove End */

static const T_AUDIO_PROBE_EVENT probe_map[] =
{
    PROBE_SCENARIO_STATE,
    PROBE_HA_STATUS_REPORT,
    // HA: this field and DSP_IPC_EVT_SEG_SEND_REQ_DATA will be deleted
    // and the ipc1.0 event D2H_DSP_STATUS_IND(0x02) will be deleted
    PROBE_SEG_SEND_REQ_DATA,
    // HA: as above, DSP_IPC_EVT_SEG_SEND_ERROR will be deleted
    PROBE_SEG_SEND_ERROR,
    PROBE_SYNC_REF_REQUEST,
    PROBE_OVP_TRAINING_REPORT,
};

typedef struct t_audio_probe_cback_item
{
    struct t_audio_probe_cback_item     *p_next;
    P_AUDIO_PROBE_DSP_CABCK              cback;
} T_AUDIO_PROBE_CBACK_ITEM;

typedef struct t_audio_probe_dsp_evt_cback_item
{
    struct t_audio_probe_dsp_evt_cback_item     *p_next;
    P_AUDIO_PROBE_DSP_EVENT_CABCK               cback;
} T_AUDIO_PROBE_DSP_EVT_CBACK_ITEM;

typedef struct t_audio_prob_db
{
    bool register_ipc_handler;
    bool register_adsp_ipc_handler;
    T_SYS_IPC_HANDLE            dsp_event;
    T_OS_QUEUE cback_list;
    P_AUDIO_PROBE_ADSP_CABCK adsp_cback;
    T_OS_QUEUE dsp_event_cback_list;
} T_AODIO_PROBE_DB;

#if (CONFIG_SOC_SERIES_RTL8773D == 1)/*https://jira.realtek.com/browse/BBPRO3RD-801*/
#define ENGINE_Q_MAX_NUM    0x30
typedef enum
{
    ENGINE_EVT_START,
    ENGINE_EVT_STOP,
} T_ENGINE_EVT;

typedef struct t_engine_db
{
    void     *msg_queue;
    void     *sem;
} T_ENGINE_DB;

typedef struct
{
    uint8_t     msg_type;
    uint8_t     msg_sub_type;
    uint16_t    data_len;
    void        *p_data;
} T_ENGINE_MSG;

static T_ENGINE_DB engine_db  = {.msg_queue = NULL, .sem = NULL};
#endif

T_AODIO_PROBE_DB audio_prob_db =
{
    .register_ipc_handler = false,
    .dsp_event            = NULL,
};

static bool audio_probe_dsp_ipc_cback(T_DSP_IPC_EVENT event, uint32_t param)
{
    T_AUDIO_PROBE_CBACK_ITEM *elem;
    uint8_t i;

    switch (event)
    {
    case DSP_IPC_EVT_PROBE:
        {
            i = PROBE_SCENARIO_STATE;
        }
        break;

    case DSP_IPC_EVT_HA_STATUS_INFO:
        {
            i = PROBE_HA_STATUS_REPORT;
        }
        break;

    case DSP_IPC_EVT_SEG_SEND_REQ_DATA:
        {
            i = PROBE_SEG_SEND_REQ_DATA;
        }
        break;

    case DSP_IPC_EVT_SEG_SEND_ERROR:
        {
            i = PROBE_SEG_SEND_ERROR;
        }
        break;

    case DSP_IPC_EVT_PROBE_SYNC_REF_REQUEST:
        {
            i = PROBE_SYNC_REF_REQUEST;
        }
        break;

    case DSP_IPC_EVT_OVP_TRAINING_INFO:
        {
            i = PROBE_OVP_TRAINING_REPORT;
        }
        break;

    default:
        {
            return false;
        }
    }

    elem = os_queue_peek(&audio_prob_db.cback_list, 0);

    while (elem != NULL)
    {
        elem->cback(probe_map[i], (void *)param);
        elem = elem->p_next;
    }
    return true;
}

bool audio_probe_dsp_cback_register(P_AUDIO_PROBE_DSP_CABCK cback)
{
    if (!audio_prob_db.register_ipc_handler &&
        dsp_ipc_cback_register(audio_probe_dsp_ipc_cback) == true)
    {
        audio_prob_db.register_ipc_handler = true;
    }
    if (!audio_prob_db.register_ipc_handler)
    {
        return false;
    }
    T_AUDIO_PROBE_CBACK_ITEM *elem;

    elem = malloc(sizeof(T_AUDIO_PROBE_CBACK_ITEM));
    if (elem != NULL)
    {
        elem->cback = cback;
        os_queue_in(&audio_prob_db.cback_list, elem);
        return true;
    }

    return false;
}

bool audio_probe_dsp_cback_unregister(P_AUDIO_PROBE_DSP_CABCK cback)
{
    bool ret = false;
    T_AUDIO_PROBE_CBACK_ITEM *elem;

    elem = os_queue_peek(&audio_prob_db.cback_list, 0);
    while (elem != NULL)
    {
        if (elem->cback == cback)
        {
            ret = os_queue_delete(&audio_prob_db.cback_list, elem);
            break;
        }

        elem = elem->p_next;
    }
    /*When remove all elems of audio_prob_db.cback_list,
    *should unregister interface of dsp ipc.
    */
    if (audio_prob_db.register_ipc_handler &&
        (audio_prob_db.cback_list.count == 0) &&
        dsp_ipc_cback_unregister(audio_probe_dsp_ipc_cback))
    {
        audio_prob_db.register_ipc_handler = false;
    }
    return ret;
}

bool audio_probe_dsp_send(uint8_t *buf, uint16_t len)
{
    AUDIO_PRINT_TRACE2("audio_probe_dsp_send: buf %p, len 0x%04x",
                       buf, len);

    return dsp_shm_cmd_send(buf, len, true);
}

#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
static bool audio_probe_adsp_ipc_cback(T_ADSP_IPC_EVENT event, uint8_t *buf, uint16_t len)
{
    uint8_t i;

    AUDIO_PRINT_INFO3("audio_probe_adsp_ipc_cback: %x %x %x", event, buf, len);

    switch (event)
    {
    case ADSP_IPC_EVT_BOOT_DONE:
        {
            i = PROBE_ADSP_BOOT_DONE;
        }
        break;

    case ADSP_IPC_EVT_ACK:
        {
            i = PROBE_ADSP_ACK;
        }
        break;

    case ADSP_IPC_EVT_LOG:
        {
            i = PROBE_ADSP_LOG;
        }
        break;

    default:
        {
            return false;
        }
    }

    if (audio_prob_db.adsp_cback)
    {
        audio_prob_db.adsp_cback((T_AUDIO_PROBE_ADSP_EVENT)i, buf, len);
    }

    return true;
}

bool audio_probe_adsp_cback_register(P_AUDIO_PROBE_ADSP_CABCK cback)
{
    if (!audio_prob_db.register_adsp_ipc_handler)
    {
        audio_prob_db.register_adsp_ipc_handler = anc_mgr_adsp_ipc_cback_register(
                                                      audio_probe_adsp_ipc_cback);

        if (audio_prob_db.register_adsp_ipc_handler == false)
        {
            return false;
        }
    }

    audio_prob_db.adsp_cback = cback;

    return true;
}

bool audio_probe_adsp_cback_unregister(P_AUDIO_PROBE_ADSP_CABCK cback)
{
    audio_prob_db.adsp_cback = NULL;

    if (audio_prob_db.register_adsp_ipc_handler)
    {
        if (anc_mgr_adsp_ipc_cback_unregister(audio_probe_adsp_ipc_cback))
        {
            audio_prob_db.register_adsp_ipc_handler = false;

            return true;
        }
    }

    return false;
}

bool audio_probe_adsp_send(uint8_t *buf, uint16_t len)
{
    AUDIO_PRINT_TRACE2("audio_probe_adsp_send: buf %p, len 0x%04x",
                       buf, len);

    return adsp_cmd_send(buf, len, true);
}
#endif

bool audio_probe_dsp_event_cback(uint32_t event, void *msg)
{
    T_AUDIO_PROBE_DSP_EVT_CBACK_ITEM *elem;

    elem = os_queue_peek(&audio_prob_db.dsp_event_cback_list, 0);

    while (elem != NULL)
    {
        elem->cback(event, msg);
        elem = elem->p_next;
    }
    return true;
}

bool audio_probe_dsp_evt_cback_register(P_AUDIO_PROBE_DSP_EVENT_CABCK cback)
{
    if (audio_prob_db.dsp_event == NULL)
    {
        audio_prob_db.dsp_event = dsp_mgr_register_cback(audio_probe_dsp_event_cback);
    }

    T_AUDIO_PROBE_DSP_EVT_CBACK_ITEM *elem;
    elem = malloc(sizeof(T_AUDIO_PROBE_DSP_EVT_CBACK_ITEM));
    if (elem != NULL)
    {
        elem->cback = cback;
        os_queue_in(&audio_prob_db.dsp_event_cback_list, elem);
        return true;
    }
    return false;
}

bool audio_probe_dsp_evt_cback_unregister(P_AUDIO_PROBE_DSP_EVENT_CABCK cback)
{
    bool ret = false;
    T_AUDIO_PROBE_DSP_EVT_CBACK_ITEM *elem;

    elem = os_queue_peek(&audio_prob_db.dsp_event_cback_list, 0);
    while (elem != NULL)
    {
        if (elem->cback == cback)
        {
            ret = os_queue_delete(&audio_prob_db.dsp_event_cback_list, elem);
            break;
        }

        elem = elem->p_next;
    }
    /*When remove all elems of audio_prob_db.dsp_event_cback_list,
    *should unregister interface of dsp evt.
    */
    if ((audio_prob_db.dsp_event != NULL) &&
        (audio_prob_db.dsp_event_cback_list.count == 0))
    {
        dsp_mgr_unregister_cback(audio_prob_db.dsp_event);
        audio_prob_db.dsp_event = NULL;
    }
    return ret;
}

void *audio_probe_media_buffer_malloc(uint16_t buf_size)
{
    return media_buffer_malloc(buf_size);
}

bool audio_probe_media_buffer_free(void *p_buf)
{
    return media_buffer_free(p_buf);
}

void audio_probe_disable_dsp_powerdown(void)
{
    audio_path_lpm_set(false);
}

void audio_probe_dsp_test_bin_set(bool enable)
{
    dsp_loader_set_test_bin(enable);
}

#if (CONFIG_SOC_SERIES_RTL8773D == 1)/*https://jira.realtek.com/browse/BBPRO3RD-801*/
void audio_probe_dsp2_exist_set(bool enable)
{
    // enable is refer to app_cfg_const.ext_buck_support
    dsp_drv_dsp2_exist_set(enable);
}

void engine_msg_handler(void)
{
    T_ENGINE_MSG msg;

    if (os_msg_recv(engine_db.msg_queue, &msg, 0) == true)
    {
        AUDIO_PRINT_INFO1("engine_msg_handler, evt:%d", msg.msg_type);
        switch (msg.msg_type)
        {
        case ENGINE_EVT_START:
            {
                bool *success = msg.p_data;
                if (dsp_mgr_power_on_check())
                {
                    dsp_mgr_dsp2_ref_increment();
                    *success = true;
                }
                else
                {
                    *success = false;
                }
                os_sem_give(engine_db.sem);
            }
            break;

        case ENGINE_EVT_STOP:
            {
                bool *success = msg.p_data;

                dsp_mgr_dsp2_ref_decrement();
                if (dsp_mgr_state_get() == DSP_MGR_STATE_ON)
                {
                    audio_path_power_off();
                }
                *success = true;
                os_sem_give(engine_db.sem);

            }
            break;

        default:
            break;
        }
    }
}

uint32_t engine_start(void)
{
    T_ENGINE_MSG msg = {.msg_type = ENGINE_EVT_START};
    bool success = false;
    uint8_t evt = EVENT_ENGINE_MSG;
    uint32_t instance = 0;

    if (engine_db.msg_queue == NULL)
    {
        if (os_msg_queue_create(&engine_db.msg_queue, "engineQ",
                                ENGINE_Q_MAX_NUM, sizeof(T_ENGINE_MSG)) == false)
        {
            return 0;
        }

        sys_mgr_event_register(EVENT_ENGINE_MSG, engine_msg_handler);
    }

    if (engine_db.sem == NULL)
    {
        if (os_sem_create(&engine_db.sem, "engine semaphore", 0, 1) == false)
        {
            return 0;
        }
    }

    msg.p_data = &success;
    msg.data_len = sizeof(&success);
    if (os_msg_send(engine_db.msg_queue, &msg, 0))
    {
        if (os_msg_send(hEventQueueHandleAu, &evt, 0))
        {
            os_sem_take(engine_db.sem, 2000);
        }
    }

    if (success)
    {
        instance = dsp_mgr_dsp2_ref_get();
    }
    AUDIO_PRINT_INFO2("engine_start:%d-%d", instance, success);
    return instance;
}

int32_t engine_stop(uint32_t instance)
{
    T_ENGINE_MSG msg = {.msg_type = ENGINE_EVT_STOP};
    bool success = false;
    uint8_t evt = EVENT_ENGINE_MSG;

    if (dsp_mgr_dsp2_ref_get() == 0)
    {
        AUDIO_PRINT_ERROR0("engine_stop failed:engine has not started");
        return -EPERM;
    }

    msg.p_data = &success;
    msg.data_len = sizeof(&success);
    if (os_msg_send(engine_db.msg_queue, &msg, 0))
    {
        if (os_msg_send(hEventQueueHandleAu, &evt, 0))
        {
            os_sem_take(engine_db.sem, 2000);
        }
    }

    AUDIO_PRINT_INFO2("engine_stop:%d-%d", dsp_mgr_dsp2_ref_get(), success);
    if (!success)
    {
        return -EPERM;
    }

    return 0;
}
#endif
