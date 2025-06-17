/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "os_msg.h"
#include "os_queue.h"
#include "trace.h"
#include "anc_mgr.h"
#include "anc_driver.h"
#include "anc_loader.h"

/* TODO Remove Start */
#include "codec_driver.h"
#include "dsp_ipc.h"

/* DIPC Little Endian uint8 to stream */
#define DIPC_LE_UINT8_TO_STREAM(s, u8)   {              \
        *s++ = (uint8_t)(u8);                           \
    }

/* DIPC Little Endian uint16 to stream */
#define DIPC_LE_UINT16_TO_STREAM(s, u16) {              \
        *s++ = (uint8_t)((u16) >> 0);                   \
        *s++ = (uint8_t)((u16) >> 8);                   \
    }

/* DIPC Little Endian uint24 to stream */
#define DIPC_LE_UINT24_TO_STREAM(s, u24) {              \
        *s++ = (uint8_t)((u24) >>  0);                  \
        *s++ = (uint8_t)((u24) >>  8);                  \
        *s++ = (uint8_t)((u24) >> 16);                  \
    }

/* DIPC Little Endian uint32 to stream */
#define DIPC_LE_UINT32_TO_STREAM(s, u32) {              \
        *s++ = (uint8_t)((u32) >>  0);                  \
        *s++ = (uint8_t)((u32) >>  8);                  \
        *s++ = (uint8_t)((u32) >> 16);                  \
        *s++ = (uint8_t)((u32) >> 24);                  \
    }

/* DIPC Little Endian stream to uint16 */
#define DIPC_LE_STREAM_TO_UINT16(u16, s) {              \
        u16  = ((uint16_t)(*(s + 0)) << 0) +            \
               ((uint16_t)(*(s + 1)) << 8);             \
        s   += 2;                                       \
    }

void *hANCQueueHandleAu;
/* TODO Remove End */

#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)

#define ANC_MSG_MAX_NUM             (0x10)

typedef enum t_anc_mgr_pending_event
{
    ANC_MGR_PENDING_EVENT_NONE,
    ANC_MGR_PENDING_EVENT_APT_GAIN,
    ANC_MGR_PENDING_EVENT_APT_EQ,
} T_ANC_MGR_PENDING_EVENT;

typedef union t_anc_mgr_pending_record
{
    uint8_t d8[8];
    struct
    {
        float apt_gain_l_record;
        float apt_gain_r_record;
    };
    struct
    {
        float apt_eq_record;
        uint8_t rsvd[4];
    };
} T_ANC_MGR_PENDING_RECORD;

typedef struct t_anc_mgr_db
{
    P_ANC_MGR_CBACK anc_mgr_cback;
    T_ANC_MGR_STATE anc_mgr_state;
    uint8_t anc_mgr_sub_type_record;
    uint8_t anc_mgr_scenario_id_record;
    uint8_t pending_action;
    T_ANC_MGR_PENDING_RECORD pending_record;
} T_ANC_MGR_DB;

typedef struct t_adsp_ipc_db
{
    T_OS_QUEUE cback_list;
} T_ADSP_IPC_DB;

typedef void (*P_ANC_MGR_ADSP_CBACK)(uint16_t event, uint8_t *payload, uint16_t payload_length);

static T_ANC_MGR_DB *anc_mgr_db = NULL;

static T_ADSP_IPC_DB adsp_ipc_db = {0};

typedef struct t_adsp_ipc_cback_elem
{
    struct t_adsp_ipc_cback_elem *next;
    P_ADSP_IPC_CBACK              cback;
} T_ADSP_IPC_CBACK_ELEM;

bool adsp_cmd_send(uint8_t  *cmd_buf,
                   uint32_t  cmd_buf_len,
                   bool      flush)
{
    bool ret;

    ret = anc_drv_adsp_shm_cmd_send(cmd_buf, cmd_buf_len, flush);

    DIPC_PRINT_INFO2("adsp_cmd_send: %b, cmd_buf_len %d", TRACE_BINARY(cmd_buf_len, cmd_buf),
                     cmd_buf_len);

    return ret;
}

uint32_t anc_mgr_adsp_mailbox_process(T_ANC_SCHED_MSG *p_msg)
{
    CODEC_PRINT_TRACE1("anc_mgr_adsp_mailbox_process: subtype 0x%X", p_msg->msg_subtype);

    switch (p_msg->msg_subtype)
    {
    case MAILBOX_D2H_ADSP_SHARE_QUEUE_ADDRESS:
        {
        }
        break;

    case MAILBOX_D2H_ADSP_LOG_BUFFER:
    case MAILBOX_D2H_ADSP_ADCDAC_DATA0:
    case MAILBOX_D2H_ADSP_ADCDAC_DATA1:
    case MAILBOX_D2H_ADSP_CHECK:
    case MAILBOX_D2H_ADSP_WATCHDOG_TIMEOUT:
        {
        }
        break;

    case MAILBOX_D2H_ADSP_BOOT_DONE:
        {
        }
        break;

    case MAILBOX_D2H_ADSP_EXCEPTION:
    default:
        break;
    }
    return 0;
}

static bool adsp_ipc_event_parser(uint16_t cmd_id, uint8_t *param)
{
    CODEC_PRINT_TRACE1("adsp_ipc_event_parser: cmd_id 0x%04X", cmd_id);
    switch (cmd_id)
    {
    case D2H_ADSP_PARA_ACK:
        {
            uint32_t target_gain = anc_drv_get_active_adsp_scenario();
            CODEC_PRINT_TRACE1("adsp_ipc_event_parser: D2H_ADSP_PARA_ACK target_gain 0x%x", target_gain);
            if (target_gain & ANC_DRV_GRP_INFO_LOOSEFIT)
            {
                anc_drv_loosefit_enable();
            }
        }
        break;

    default:
        break;
    }

    return true;
}

static void adsp_dipc_event_post(uint32_t event, uint8_t *buf, uint16_t len)
{
    T_ADSP_IPC_CBACK_ELEM *elem;

    if (event != ADSP_IPC_EVT_NONE)
    {
        elem = os_queue_peek(&adsp_ipc_db.cback_list, 0);
        while (elem != NULL)
        {
            elem->cback((T_ADSP_IPC_EVENT)event, buf, len);
            elem = elem->next;
        }
    }
}

static void adsp_dipc_event_parser(uint16_t  event,
                                   uint8_t  *payload,
                                   uint16_t  payload_length)
{
    uint8_t ret;

    ret = 0;
    CODEC_PRINT_TRACE1("adsp_ipc_event_parser: DIPC2.0 event 0x%04x", event);

    switch (event)
    {
    case DIPC3_D2H_INFORMATION_EXCHANGE_COMMAND_BOOT_DONE:
        {
            adsp_dipc_event_post(ADSP_IPC_EVT_BOOT_DONE, payload, payload_length);
        }
        break;

    case DIPC3_D2H_INFORMATION_EXCHANGE_COMMAND_ACK:
        {
            adsp_dipc_event_post(ADSP_IPC_EVT_ACK, payload, payload_length);
        }
        break;

    case DIPC3_D2H_INFORMATION_EXCHANGE_COMMAND_LOG:
        {
            adsp_dipc_event_post(ADSP_IPC_EVT_LOG, payload, payload_length);
        }
        break;

    default :
        {

        }
        break;
    }

    if (ret != DIPC_ERROR_SUCCESS)
    {
        DIPC_PRINT_TRACE2("adsp_ipc_event_parser: event 0x%04x, ret %d", event, ret);
    }
}

bool anc_mgr_adsp_ipc_cback_register(P_ADSP_IPC_CBACK cback)
{
    T_ADSP_IPC_CBACK_ELEM *elem;

    elem = calloc(1, sizeof(T_ADSP_IPC_CBACK_ELEM));

    if (elem != NULL)
    {
        elem->cback = cback;
        os_queue_in(&adsp_ipc_db.cback_list, elem);
        return true;
    }

    return false;
}

bool anc_mgr_adsp_ipc_cback_unregister(P_ADSP_IPC_CBACK cback)
{
    T_ADSP_IPC_CBACK_ELEM *elem;

    elem = os_queue_peek(&adsp_ipc_db.cback_list, 0);

    while (elem != NULL)
    {
        if (elem->cback == cback)
        {
            return os_queue_delete(&adsp_ipc_db.cback_list, elem);
        }

        elem = elem->next;
    }

    return false;
}

bool anc_mgr_drv_event_handler(T_ANC_DRV_EVENT event, void *msg)
{
    T_ANC_SCHED_MSG *anc_msg;
    anc_msg = (T_ANC_SCHED_MSG *)msg;
    CODEC_PRINT_TRACE2("anc_mgr_drv_event_handler: event %d, state %d", event,
                       anc_mgr_db->anc_mgr_state);

    switch (event)
    {
    case ANC_DRV_EVENT_ADSP_INTR_D2H_CMD:
        {
            uint16_t peek_len;

            peek_len = anc_drv_adsp_shm_evt_len_peek();
            CODEC_PRINT_TRACE1("anc_mgr_drv_event_handler: peek_len 0x%x", peek_len);

            if (peek_len != 0)
            {
                uint8_t *rcv_buffer;
                rcv_buffer = malloc(peek_len);
                if (rcv_buffer != NULL)
                {
                    uint16_t rxlen = anc_drv_adsp_shm_evt_recv(rcv_buffer, peek_len);

                    if (rxlen)
                    {
                        uint16_t event;
                        uint16_t payload_length;
                        uint8_t *p;

                        p = rcv_buffer;

                        DIPC_LE_STREAM_TO_UINT16(event, p);
                        DIPC_LE_STREAM_TO_UINT16(payload_length, p);

                        if (DIPC_EIF_GET(event) == DIPC_EIF_CURRENT)
                        {
                            DIPC_PRINT_INFO2("anc_mgr_drv_event_handler: DIPC2.0 cmd %b, rxlen %d", TRACE_BINARY(rxlen,
                                             rcv_buffer), rxlen);
                            adsp_dipc_event_parser(event, p, payload_length);
                        }
                        else
                        {
                            DIPC_PRINT_INFO2("anc_mgr_drv_event_handler: DIPC1.0 cmd %b, rxlen %d", TRACE_BINARY(rxlen,
                                             rcv_buffer), rxlen);
                            adsp_ipc_event_parser(event, p);
                        }

                        anc_send_msg(ANC_MSG_ADSP_INTR_D2H_CMD, 0, NULL, 0);
                    }
                    free(rcv_buffer);
                }
            }
        }
        break;

    case ANC_DRV_EVENT_ADSP_INTR_MAILBOX:
        {
            anc_mgr_adsp_mailbox_process(anc_msg);
        }
        break;

    case ANC_DRV_EVENT_LOAD_PART:
        {
            anc_loader_next_bin();
        }
        break;

    case ANC_DRV_EVENT_LOAD_FINISH:
        {
            anc_loader_finish();
        }
        break;

    case ANC_DRV_EVENT_FADE_IN_COMPLETE:
        {
        }
        break;

    case ANC_DRV_EVENT_FADE_OUT_COMPLETE:
        {
            anc_mgr_db->anc_mgr_state = ANC_MGR_STATE_DISABLED;

            anc_drv_disable();

            anc_mgr_db->anc_mgr_cback(ANC_MGR_EVENT_DISABLED);
        }
        break;

    case ANC_DRV_EVENT_GAIN_ADJUST_COMPLETE:
    case ANC_DRV_EVENT_EQ_ADJUST_COMPLETE:
        {
            anc_mgr_db->anc_mgr_state = ANC_MGR_STATE_ENABLED;

            if (anc_mgr_db->pending_action == ANC_MGR_PENDING_EVENT_APT_GAIN)
            {
                anc_mgr_db->pending_action = ANC_MGR_PENDING_EVENT_NONE;

                anc_mgr_gain_set(anc_mgr_db->pending_record.apt_gain_l_record,
                                 anc_mgr_db->pending_record.apt_gain_l_record);
            }
            else if (anc_mgr_db->pending_action == ANC_MGR_PENDING_EVENT_APT_EQ)
            {
                anc_mgr_db->pending_action = ANC_MGR_PENDING_EVENT_NONE;

                anc_mgr_eq_set(anc_mgr_db->pending_record.apt_eq_record);
            }
        }
        break;

    default:
        break;
    }

    return true;
}

bool anc_mgr_loader_event_handler(T_ANC_LOADER_EVENT event)
{
    CODEC_PRINT_TRACE2("anc_mgr_loader_event_handler: event %d, state %d",
                       event, anc_mgr_db->anc_mgr_state);

    switch (event)
    {
#if (AM_ADSP_SUPPORT == 0)
#else
    case ANC_LOADER_EVENT_ADSP_IMAGE:
        {
            // wait until ADSP image load done
        }
        break;
    case ANC_LOADER_EVENT_ADSP_PARA_COMPLETE:
        {
            uint32_t target_gain = anc_drv_get_active_adsp_scenario();

            anc_drv_turn_on_adsp(1);

            if (target_gain & ANC_DRV_GRP_INFO_ULLRHA)
            {
                anc_drv_cmd_load_adsp_para();
            }
            else
            {
                anc_drv_load_adsp_para(NULL, 0, 1);

                if (target_gain & ANC_DRV_GRP_INFO_LOOSEFIT)
                {
                    anc_drv_loosefit_enable();
                }
            }
        }
        break;
    case ANC_LOADER_EVENT_ADSP_LOOSE_FIT_COMPLETE:
        {
            uint8_t image_type;
            uint32_t target_gain = anc_drv_get_active_adsp_scenario();
            if (target_gain & (ANC_DRV_GRP_INFO_ADAPTIVE_ANC |
                               ANC_DRV_GRP_INFO_ULLRHA))
            {
                image_type = SHM_SCENARIO_ANC_ADSP_PARA;
                anc_loader_bin_load(image_type);
            }
        }
        break;
#endif
    case ANC_LOADER_EVENT_CONFIGURATION_COMPLETE:
        {
            if (anc_mgr_db->anc_mgr_state == ANC_MGR_STATE_UNINITIALIZED)
            {
                if (anc_mgr_db->anc_mgr_scenario_id_record == 0xFF)
                {
                    // for tool mode, there's no need to load pending scenario image
                    anc_mgr_db->anc_mgr_state = ANC_MGR_STATE_ENABLED;
#if (AM_ADSP_SUPPORT == 0)
                    anc_drv_enable();
#else
                    if (anc_drv_get_adsp_para_source() == ANC_DRV_ADSP_PARA_SRC_FROM_TOOL)
                    {
                        // anc_drv_enable has been loaded by tool
                    }
                    else
                    {
                        anc_drv_enable();
                    }
#endif
                    anc_mgr_db->anc_mgr_cback(ANC_MGR_EVENT_ENABLED);
                }
                else
                {
                    anc_mgr_db->anc_mgr_state = ANC_MGR_STATE_ENABLING;

                    // load pending scenario image
                    anc_mgr_load_scenario();
                }
            }
        }
        break;

    case ANC_LOADER_EVENT_SCENARIO_COMPLETE:
        {
#if (AM_ADSP_SUPPORT == 0)
            anc_drv_enable();

            anc_drv_fade_in();
#else
            if (anc_drv_get_adsp_para_source() == ANC_DRV_ADSP_PARA_SRC_FROM_TOOL)
            {
                // anc_drv_enable has been loaded by tool
            }
            else
            {
                anc_drv_enable();
                anc_drv_fade_in();

                uint8_t image_type;
                uint32_t target_gain = anc_drv_get_active_adsp_scenario();

                if (target_gain & ANC_DRV_GRP_INFO_LOOSEFIT)
                {
                    image_type = SHM_SCENARIO_ANC_ADSP_LOOSE_FIT;
                    anc_loader_bin_load(image_type);
                }
                else
                {
                    if (target_gain & (ANC_DRV_GRP_INFO_ADAPTIVE_ANC |
                                       ANC_DRV_GRP_INFO_ULLRHA))
                    {
                        image_type = SHM_SCENARIO_ANC_ADSP_PARA;
                        anc_loader_bin_load(image_type);
                    }
                }
            }
#endif
            anc_mgr_db->anc_mgr_state = ANC_MGR_STATE_ENABLED;

            anc_mgr_db->anc_mgr_cback(ANC_MGR_EVENT_ENABLED);

            // clear record scenario id & sub type
            anc_mgr_db->anc_mgr_sub_type_record = 0;
            anc_mgr_db->anc_mgr_scenario_id_record = 0;
        }
        break;

    default:
        break;
    }

    return true;
}

bool anc_mgr_init(P_ANC_MGR_CBACK cback)
{
    int32_t ret = 0;

    anc_mgr_db = malloc(sizeof(T_ANC_MGR_DB));
    if (anc_mgr_db == NULL)
    {
        ret = 1;
        goto fail_alloc_db;
    }

    if (anc_drv_init(anc_mgr_drv_event_handler) == false)
    {
        ret = 2;
        goto fail_init_drv;
    }

    if (anc_loader_init(anc_mgr_loader_event_handler) == false)
    {
        ret = 3;
        goto fail_init_loader;
    }

    if (os_msg_queue_create(&hANCQueueHandleAu,
                            "anc",
                            ANC_MSG_MAX_NUM,
                            sizeof(T_ANC_SCHED_MSG)) == false)
    {
        ret = 5;
        goto fail_create_queue;
    }

    anc_mgr_db->anc_mgr_cback = cback;
    anc_mgr_db->anc_mgr_state = ANC_MGR_STATE_UNINITIALIZED;
    anc_mgr_db->anc_mgr_sub_type_record  = 0;
    anc_mgr_db->anc_mgr_scenario_id_record = 0;
    anc_mgr_db->pending_action = ANC_MGR_PENDING_EVENT_NONE;

    for (uint8_t i = 0; i < sizeof(T_ANC_MGR_PENDING_RECORD); i++)
    {
        anc_mgr_db->pending_record.d8[i] = 0;
    }

    os_queue_init(&adsp_ipc_db.cback_list);

    return true;

fail_create_queue:
    anc_loader_deinit();
fail_init_loader:
    anc_drv_deinit();
fail_init_drv:
    free(anc_mgr_db);
    anc_mgr_db = NULL;
fail_alloc_db:
    CODEC_PRINT_ERROR1("anc_mgr_init: failed %d", -ret);
    return false;
}

void anc_mgr_deinit(void)
{
    if (anc_mgr_db != NULL)
    {
        free(anc_mgr_db);
        anc_mgr_db = NULL;
    }
}

void anc_mgr_enable(void)
{
    CODEC_PRINT_TRACE1("anc_mgr_enable: state %d", anc_mgr_db->anc_mgr_state);

    switch (anc_mgr_db->anc_mgr_state)
    {
    case ANC_MGR_STATE_ENABLED:
        {
            anc_mgr_db->anc_mgr_state = ANC_MGR_STATE_ENABLED;
        }
        break;

    case ANC_MGR_STATE_ENABLING:
        {
            anc_mgr_db->anc_mgr_state = ANC_MGR_STATE_ENABLING;
        }
        break;

    case ANC_MGR_STATE_DISABLED:
        {
            if (anc_mgr_db->anc_mgr_scenario_id_record == 0xFF)
            {
                // for tool mode, there's no need to load scenario image
                anc_mgr_db->anc_mgr_state = ANC_MGR_STATE_ENABLED;

                anc_drv_enable();

                anc_mgr_db->anc_mgr_cback(ANC_MGR_EVENT_ENABLED);
            }
            else
            {
                anc_mgr_db->anc_mgr_state = ANC_MGR_STATE_ENABLING;

                anc_mgr_load_scenario();
            }
        }
        break;

    case ANC_MGR_STATE_DISABLING:
        {
            anc_mgr_db->anc_mgr_state = ANC_MGR_STATE_DISABLING;
        }
        break;

    case ANC_MGR_STATE_UNINITIALIZED:
        {
            // load configuration image first whether in user mode or tool mode
            anc_mgr_load_configuration();
        }
        break;

    default:
        break;
    }
}

void anc_mgr_disable(void)
{
    CODEC_PRINT_TRACE1("anc_mgr_disable: state %d", anc_mgr_db->anc_mgr_state);

    switch (anc_mgr_db->anc_mgr_state)
    {
    case ANC_MGR_STATE_ENABLED:
    case ANC_MGR_STATE_ENABLING:
        {
            anc_mgr_db->anc_mgr_state = ANC_MGR_STATE_DISABLING;

            // disable ANC after fade out complete
            // call anc_drv_set_gain during ANC fade in/out will not receive corresponding event
            anc_drv_fade_out();
        }
        break;

    case ANC_MGR_STATE_DISABLED:
        {
            anc_mgr_db->anc_mgr_state = ANC_MGR_STATE_DISABLED;
        }
        break;

    case ANC_MGR_STATE_DISABLING:
        {
            anc_mgr_db->anc_mgr_state = ANC_MGR_STATE_DISABLING;
        }
        break;

    case ANC_MGR_STATE_UNINITIALIZED:
    default:
        break;
    }
}

void anc_mgr_gain_set(float l_gain,
                      float r_gain)
{
    switch (anc_mgr_db->anc_mgr_state)
    {
    case ANC_MGR_STATE_ENABLED:
        {
            anc_mgr_db->anc_mgr_state = ANC_MGR_STATE_ENABLING;

            anc_drv_set_gain(l_gain, r_gain);
        }
        break;

    case ANC_MGR_STATE_ENABLING:
        {
            anc_mgr_db->pending_action = ANC_MGR_PENDING_EVENT_APT_GAIN;
            anc_mgr_db->pending_record.apt_gain_l_record = l_gain;
            anc_mgr_db->pending_record.apt_gain_r_record = r_gain;
        }
        break;

    case ANC_MGR_STATE_DISABLED:
    case ANC_MGR_STATE_DISABLING:
        break;

    default:
        break;
    }
}

void anc_mgr_load_configuration(void)
{
    anc_loader_bin_load(SHM_SCENARIO_ANC_0_CFG);
}

void anc_mgr_load_scenario(void)
{
    uint8_t image_type, en_code_fmt;
    uint8_t sub_type, scenario_id;

    en_code_fmt = 0;

#if (AM_ADSP_SUPPORT==1)
    anc_drv_enable_adaptive_anc(0, 0);
    anc_drv_turn_on_adsp(0);
#endif

    sub_type = anc_mgr_db->anc_mgr_sub_type_record;
    scenario_id = anc_mgr_db->anc_mgr_scenario_id_record;

    anc_drv_get_info(&en_code_fmt, ANC_INFO_EN_CODE_FMT);

    if (en_code_fmt == ANC_DRV_EN_CODE_FMT_ANC_ONLY ||
        en_code_fmt == ANC_DRV_EN_CODE_FMT_OLD_ANC)
    {
        // ANC only or old ANC image
        image_type = SHM_SCENARIO_ANC_1_COEF + scenario_id;
        anc_loader_bin_load(image_type);
    }
    else if ((sub_type == ANC_IMAGE_SUB_TYPE_ANC_COEF) ||
             (sub_type == ANC_IMAGE_SUB_TYPE_APT_COEF) ||
             (sub_type == ANC_IMAGE_SUB_TYPE_ANC_APT_COEF))
    {
        anc_loader_set_img_load_param(sub_type, scenario_id);
        image_type = SHM_SCENARIO_ANC_APT_COEF;
        anc_loader_bin_load(image_type);
    }

    CODEC_PRINT_TRACE4("anc_mgr_load_scenario: state %d, scenario_id %d, sub_type: %d, en_code_fmt %d",
                       anc_mgr_db->anc_mgr_state, scenario_id, sub_type, en_code_fmt);
}

void anc_mgr_eq_set(float strength)
{
    switch (anc_mgr_db->anc_mgr_state)
    {
    case ANC_MGR_STATE_ENABLED:
        {
            anc_mgr_db->anc_mgr_state = ANC_MGR_STATE_ENABLING;

            anc_drv_eq_set(strength);
        }
        break;

    case ANC_MGR_STATE_ENABLING:
        {
            anc_mgr_db->pending_action = ANC_MGR_PENDING_EVENT_APT_EQ;
            anc_mgr_db->pending_record.apt_eq_record = strength;
        }
        break;

    case ANC_MGR_STATE_DISABLED:
    case ANC_MGR_STATE_DISABLING:
        break;

    default:
        break;
    }
}

void anc_mgr_tool_enable(void)
{
    anc_drv_enable();
}

void anc_mgr_tool_disable(void)
{
    anc_drv_disable();
}

void anc_mgr_tool_set_feature_map(uint32_t feature_map)
{
    anc_drv_tool_set_feature_map((T_ANC_DRV_FEATURE_MAP)feature_map);
}

uint32_t anc_mgr_tool_get_feature_map(void)
{
    return anc_drv_tool_get_feature_map();
}

uint8_t anc_mgr_tool_set_para(void *anc_cmd_ptr)
{
    return anc_drv_tool_set_para((T_ANC_CMD_PKT *)anc_cmd_ptr);
}

uint32_t anc_mgr_tool_read_reg(uint32_t reg_addr)
{
    return anc_drv_tool_reg_read(reg_addr);
}

bool anc_mgr_tool_response_measure_enable(uint8_t   enable,
                                          uint8_t   ch_sel,
                                          uint32_t *tx_freq,
                                          uint8_t   freq_num,
                                          uint8_t   amp_ratio)
{
    T_CODEC_SPK_CONFIG spk_config;
    T_CODEC_DAC_CONFIG dac_config;

    if (enable)
    {
        // spk setting for response measure
        codec_drv_config_init(CODEC_CONFIG_SEL_SPK, (void *)&spk_config);

        spk_config.power_en = (ch_sel == SPK_CHANNEL_L) ? 1 : 0;
        codec_drv_spk_config_set(SPK_CHANNEL_L, &spk_config);
        spk_config.power_en = (ch_sel == DAC_CHANNEL_R) ? 1 : 0;
        codec_drv_spk_config_set(SPK_CHANNEL_R, &spk_config);

        // dac setting for response measure
        codec_drv_config_init(CODEC_CONFIG_SEL_DAC, (void *)&dac_config);
        dac_config.anc_mute_en = 1;
        dac_config.sample_rate = 48000;
        dac_config.dig_gain = 0xAF;
        dac_config.i2s_sel = I2S_CHANNEL_0;
        dac_config.downlink_mix = CODEC_DOWNLINK_MIX_NONE;
        dac_config.music_mute_en = (ch_sel == DAC_CHANNEL_L) ? 0 : 1;
        dac_config.ana_power_en = (ch_sel == DAC_CHANNEL_L) ? 1 : 0;
        dac_config.dig_power_en = (ch_sel == DAC_CHANNEL_L) ? 1 : 0;
        codec_drv_dac_config_set(DAC_CHANNEL_L, &dac_config);
        dac_config.music_mute_en = (ch_sel == DAC_CHANNEL_R) ? 0 : 1;
        dac_config.ana_power_en = (ch_sel == DAC_CHANNEL_R) ? 1 : 0;
        dac_config.dig_power_en = (ch_sel == DAC_CHANNEL_R) ? 1 : 0;
        codec_drv_dac_config_set(DAC_CHANNEL_R, &dac_config);
    }

    return anc_drv_response_measure_enable(enable, tx_freq, freq_num, amp_ratio);
}

bool anc_mgr_tool_config_data_log(uint8_t  src0_sel,
                                  uint8_t  src1_sel,
                                  uint16_t log_len)
{
    anc_drv_config_data_log((T_ANC_DRV_LOG_SRC_SEL)src0_sel,
                            (T_ANC_DRV_LOG_SRC_SEL)src1_sel,
                            log_len);
    return true;
}

bool anc_mgr_tool_load_data_log(void)
{
    anc_drv_load_data_log();

    return true;
}

void anc_mgr_convert_data_log_addr(uint32_t *log_dest_addr)
{
    anc_drv_convert_data_log_addr(log_dest_addr);
}

uint32_t anc_mgr_tool_get_data_log_length(void)
{
    return anc_drv_get_data_log_length();
}

uint8_t anc_mgr_tool_check_resp_meas_mode(void)
{
    return anc_drv_check_resp_meas_mode();
}

void anc_mgr_tool_set_resp_meas_mode(uint8_t resp_meas_mode)
{
    anc_drv_set_resp_meas_mode(resp_meas_mode);
}

void anc_mgr_tool_set_gain_mismatch(uint8_t  gain_src,
                                    uint32_t l_gain,
                                    uint32_t r_gain)
{
    anc_drv_set_gain_mismatch(gain_src, l_gain, r_gain);
}

bool anc_mgr_tool_read_gain_mismatch(uint8_t   gain_src,
                                     uint8_t   read_flash,
                                     uint32_t *l_gain,
                                     uint32_t *r_gain)
{
    return anc_drv_read_gain_mismatch(gain_src, read_flash, l_gain, r_gain);
}

bool anc_mgr_tool_read_mp_ext_data(uint32_t *mp_ext_data)
{
    return anc_drv_read_mp_ext_data((T_ANC_DRV_MP_EXT_DATA *)mp_ext_data);
}

bool anc_mgr_tool_burn_gain_mismatch(uint32_t mp_ext_data)
{
    return anc_drv_burn_gain_mismatch((T_ANC_DRV_MP_EXT_DATA)mp_ext_data);
}

uint8_t anc_mgr_tool_get_scenario_info(uint8_t *scenario_mode,
                                       uint8_t  sub_type,
                                       uint8_t *scenario_apt_brightness,
                                       uint8_t *scenario_apt_ullrha,
                                       uint8_t *scenario_apt_type)
{
    return anc_drv_get_scenario_info(scenario_mode,
                                     sub_type,
                                     scenario_apt_brightness,
                                     scenario_apt_ullrha,
                                     scenario_apt_type);
}

void anc_mgr_tool_set_llapt_gain_mismatch(uint32_t l_gain,
                                          uint32_t r_gain)
{
    anc_drv_set_llapt_gain_mismatch(l_gain, r_gain);
}

bool anc_mgr_tool_read_llapt_gain_mismatch(uint8_t   read_flash,
                                           uint32_t *l_gain,
                                           uint32_t *r_gain)
{
    return anc_drv_read_llapt_gain_mismatch(read_flash, l_gain, r_gain);
}

bool anc_mgr_tool_read_llapt_ext_data(uint32_t *llapt_ext_data)
{
    return anc_drv_read_llapt_ext_data((T_ANC_DRV_MP_EXT_DATA *)llapt_ext_data);
}

bool anc_mgr_tool_burn_llapt_gain_mismatch(uint32_t llapt_ext_data)
{
    return anc_drv_burn_llapt_gain_mismatch((T_ANC_DRV_MP_EXT_DATA)llapt_ext_data);
}

void anc_mgr_tool_limiter_wns_switch(void)
{
    anc_drv_limiter_wns_switch();
}

void anc_mgr_load_cfg_set(uint8_t sub_type,
                          uint8_t scenario_id)
{
    CODEC_PRINT_TRACE2("anc_mgr_load_cfg_set: pending scenario_id: %d, sub_type: %d",
                       scenario_id, sub_type);

    anc_mgr_db->anc_mgr_sub_type_record = sub_type;
    anc_mgr_db->anc_mgr_scenario_id_record = scenario_id;
}

#else
bool anc_mgr_init(P_ANC_MGR_CBACK cback)
{
    return true;
}

void anc_mgr_deinit(void)
{
    return;
}

void anc_mgr_enable(void)
{
    return;
}

void anc_mgr_disable(void)
{
    return;
}

void anc_mgr_eq_set(float strength)
{
    return;
}


void anc_mgr_gain_set(float l_gain,
                      float r_gain)
{
    return;
}

void anc_mgr_load_cfg_set(uint8_t sub_type,
                          uint8_t scenario_id)
{
    return;
}
#endif
