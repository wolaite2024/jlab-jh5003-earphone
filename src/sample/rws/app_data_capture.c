#if F_APP_DATA_CAPTURE_SUPPORT
#include <string.h>
#include "app_sniff_mode.h"
#include "app_timer.h"
#include "audio_route.h"
#include "app_audio_route.h"
#include "app_cmd.h"
#include "pm.h"
#include "trace.h"
#include "os_mem.h"
#include "stdlib.h"
#include "app_main.h"
#include "audio_probe.h"
#include "app_cfg.h"
#include "app_dsp_cfg.h"
#include "app_dlps.h"
#include "gap_br.h"
#include "bt_bond.h"
#include "app_bond.h"
#include "app_bt_policy_int.h"
#include "audio_track.h"
#include "audio.h"
#include "app_data_capture.h"
#include "gap_br.h"
#include "app_report.h"
#include "os_sched.h"
#include "wdg.h"
#include "app_auto_power_off.h"
#include "sysm.h"
#include "btm.h"
#include "ftl.h"
#if F_APP_HIFI4_SUPPORT
#include "ipc.h"
#include "app_ipc.h"
#endif

#if F_APP_SUPPORT_CAPTURE_DUAL_BUD | F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
#include "bt_hfp.h"
#include "app_roleswap_control.h"
#endif

#define APP_DATA_CAPTURE_POWER_OFF_TIME 600
//for CMD_AUDIO_DSP_CTRL_SEND to capture dsp data
#define VENDOR_SPP_CAPTURE_DSP_LOG      0x01
#define VENDOR_SPP_CAPTURE_DSP_RWA_DATA 0x02
#define H2D_CMD_DSP_DAC_ADC_DATA_TO_MCU 0x1F
#define H2D_SPPCAPTURE_SET              0x0F01
#define CHANGE_MODE_EXIST               0x00
#define CHANGE_MODE_TO_SCO              0x01

#define CAPTURE_MODE_MASK_A2DP          0x01
#define CAPTURE_MODE_MASK_HFP           0x02
#define CAPTURE_MODE_MASK_DSP_APT       0x04
#define CAPTURE_MODE_MASK_DATA_CAPTURE  0x08
#define CAPTURE_MODE_MASK_BUD_SIDE      0x10

static uint8_t app_data_capture_timer_id = 0;
static uint8_t timer_idx_dsp_spp_captrue_check_link = 0;
static uint8_t dsp_capture_data_master_retry = 0;
static uint8_t *dsp_capture_data_cmd_ptr;
static uint8_t dsp_capture_data_cmd_len;
static uint8_t *dsp_capture_data_gain_level;
static uint8_t dsp_capture_data_app_idx;
static uint16_t dsp_capture_data_state = 0;
static uint8_t dsp_capture_data_path = CMD_PATH_SPP;
static uint16_t dsp_capture_data_seq = 0xFFFF;
#if F_APP_HIFI4_SUPPORT
static uint16_t dsp2_capture_data_seq = 0xFFFF;
static T_APP_IPC_HANDLE dsp2_ipc_handle = NULL;
#endif
#if F_APP_SAIYAN_MODE
static uint8_t dsp_capture_data_gain_level_len;
#endif
static uint8_t dsp_capture_data_mode = 0;  // only for SPP2.0  , not for saiyan

T_SAIYAN_MODE data_capture_saiyan;

T_AUDIO_TRACK_HANDLE dsp_capture_data_audio_track_handle;
T_AUDIO_TRACK_HANDLE dsp_capture_data_audio_record_handle;

enum
{
    CAPTRUE_TRACK_STOP       = 0x00,
    CAPTRUE_TRACK_START      = 0x01
};

typedef enum
{
    CAP_REPORT_ST_SUC = 0x0,
    CAP_REPORT_ST_NOT_SUP,
    CAP_REPORT_ST_LOAD_FAIL,
    CAP_REPORT_ST_EXIT_FAIL,
    CAP_REPORT_ST_OTHER_ERR = 0xFF

} T_CAPTURE_REPORT_ST;

typedef struct
{
    uint16_t    data_len;
    uint8_t     *p_data;
} T_PROBE_CB_MAILBOX_DATA;

typedef struct
{
    uint32_t    sample_rate;
    uint8_t     chann_mode;
    uint8_t     block_length;
    uint8_t     subband_num;
    uint8_t     allocation_method;
    uint8_t     bitpool;
} T_CAPTURE_SBC_RECORD;

typedef struct
{
    uint32_t    sample_rate;
    uint8_t     chann_mode;
    uint8_t     block_length;
    uint8_t     subband_num;
    uint8_t     allocation_method;
    uint8_t     bitpool;
    uint8_t     ch0_io_type;
    uint8_t     ch0_mic_sel;
    uint8_t     ch0_mic_type;
    uint8_t     ch1_io_type;
    uint8_t     ch1_mic_sel;
    uint8_t     ch1_mic_type;
} T_CAPTURE_SAIYAN_SBC_RECORD;

enum
{
    LOGIC_FF_MIC              = 0x00,
    LOGIC_FB_MIC              = 0x01,
    LOGIC_VOICE_MIC           = 0x02,
    LOGIC_SECONDARY_VOICE_MIC = 0x03,
    LOGIC_PRIMARY_APT_MIC     = 0x04,
    LOGIC_SECONDARY_APT_MIC   = 0x05,
    LOGIC_INVALID_MIC         = 0xFF,
};

typedef struct
{
    uint32_t    sample_rate;
    uint8_t     chann_mode;
    uint8_t     block_length;
    uint8_t     subband_num;
    uint8_t     allocation_method;
    uint8_t     bitpool;
    uint8_t     logic_mic1;
    uint8_t     logic_mic2;
} T_CAPTURE_USER_MIC_SBC_RECORD;

typedef enum
{
    APP_TIMER_DATA_CAPTRUE_CHECK_LINK,
} T_CAPTURE_TIMER;

typedef enum
{
    APP_DATA_CAPTURE_SAIYAN         = 0x00,
    APP_DATA_CAPTURE_RAW_DATA       = 0x01,
    APP_DATA_CAPTURE_ENTER_SCO_MODE = 0x02,
    APP_DATA_CAPTURE_SCO_MODE_CTL   = 0x03,
    APP_DATA_CAPTURE_USER_MIC       = 0x04,
    APP_DATA_CAPTURE_DSP2_DATA      = 0x05,
    APP_DATA_CAPTURE_ACOUSTICS_MP   = 0x06,
} T_APP_CAPTURE_TYPE;

#if F_APP_SUPPORT_CAPTURE_DUAL_BUD | F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
static uint8_t bud_side_trig_roleswap = 0;
static T_MIC_INDEX app_logic_mic_bitmap = INVALID_MIC;
union
{
    uint8_t cfg;
    struct
    {
        uint8_t left_caputre : 1;
        uint8_t right_caputre : 1;
        uint8_t rsv : 6;
    };
} capture_data_dual_bud;

typedef struct
{
    T_AUDIO_CATEGORY category;
    T_AUDIO_ROUTE_IO_TYPE mic;
} T_APP_AUDIO_CATEGORY_LOGIC_MIC;
#endif

static void app_data_capture_send_audio_probe(void);
static void app_data_capture_unregister(void);
static void app_data_capture_register(void);

#if F_APP_SAIYAN_MODE
void app_data_capture_saiyan_mode_ctl(uint8_t start, uint8_t op_code)
{
    APP_PRINT_TRACE3("app_data_capture_saiyan_mode_ctl %d %d %d", data_capture_saiyan.saiyan_enable,
                     start,
                     app_db.device_state);
    if (data_capture_saiyan.saiyan_enable != start)
    {
        data_capture_saiyan.saiyan_enable = start;
        data_capture_saiyan.le_op_code = op_code;
#ifndef CONFIG_SOC_SERIES_RTL8773D
        audio_probe_dsp_test_bin_set((data_capture_saiyan.saiyan_enable) ? true : false);
#endif
        ftl_save_to_storage(&data_capture_saiyan, APP_RW_SAIYAN_ADDR, APP_RW_SAIYAN_SIZE);

        if (start != 0)
        {
            os_delay(100);
            chip_reset(RESET_ALL);
        }
        else
        {
            bt_bond_clear();
            app_cfg_const.bud_role = app_db.saiyan_org_role;
        }
    }
}

void app_data_capture_send_gain(void)
{
    if (data_capture_saiyan.saiyan_enable)
    {
        if (dsp_capture_data_gain_level)
        {
            audio_probe_dsp_send(dsp_capture_data_gain_level, dsp_capture_data_gain_level_len);
        }
    }
}

void app_data_capture_gain_ctl(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                               uint8_t app_idx, uint8_t *ack_pkt)
{
    if (dsp_capture_data_gain_level)
    {
        free(dsp_capture_data_gain_level);
        dsp_capture_data_gain_level = NULL;
    }

    if (cmd_len)
    {
        //APP_PRINT_TRACE1("dsp_capture_data_gain_level free len %d", cmd_len);
        dsp_capture_data_gain_level = malloc(cmd_len);
        memcpy(dsp_capture_data_gain_level, cmd_ptr, cmd_len);
        dsp_capture_data_gain_level_len = cmd_len;
    }
    else
    {
        //APP_PRINT_TRACE0("dsp_capture_data_gain_level free error");
        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
    }
    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
}
#endif

static void app_data_capture_report(uint8_t cmd_path, uint8_t app_idx, uint8_t track_status,
                                    uint8_t mask)
{
    uint8_t buf[2];
    buf[0] = track_status;
    buf[1] = mask;
    app_report_event(cmd_path, EVENT_DATA_CAPTURE_START_STOP, app_idx, buf, sizeof(buf));
}

static void app_data_capture_audio_cback(T_AUDIO_EVENT event_type, void *event_buf,
                                         uint16_t buf_len)
{
    T_AUDIO_EVENT_PARAM *param = event_buf;
    bool handle = true;
    uint16_t seq_num;
    uint8_t send_pkt_num = 5;
    uint8_t track_status = 0;

    if ((param->track_state_changed.handle == dsp_capture_data_audio_track_handle) ||
        (param->track_state_changed.handle == dsp_capture_data_audio_record_handle))
    {
        switch (event_type)
        {
        case AUDIO_EVENT_TRACK_STATE_CHANGED:
            {
                uint8_t p_audio_buf[4] = {0, 0, 0, 0};
                APP_PRINT_INFO2("AUDIO_EVENT_TRACK_STATE_CHANGED handle %p track_state, 0x%x",
                                param->track_state_changed.handle,
                                param->track_state_changed.state);

                if ((param->track_state_changed.state == AUDIO_TRACK_STATE_STARTED) ||
                    (param->track_state_changed.state == AUDIO_TRACK_STATE_RELEASED))
                {
                    if (param->track_state_changed.handle == dsp_capture_data_audio_track_handle)
                    {
                        if (param->track_state_changed.state == AUDIO_TRACK_STATE_STARTED)
                        {
                            for (seq_num = 0; seq_num < send_pkt_num; seq_num++)
                            {
                                uint16_t written_len;
                                audio_track_write(dsp_capture_data_audio_track_handle,
                                                  0,//              timestamp,
                                                  seq_num,
                                                  AUDIO_STREAM_STATUS_CORRECT,
                                                  1,//            frame_num,
                                                  (uint8_t *)p_audio_buf,
                                                  sizeof(p_audio_buf),
                                                  &written_len);
                            }
                            track_status = CAPTRUE_TRACK_START;
                            //dsp_capture_data_state |= DATA_CAPTURE_DATA_START_SCO_MODE;
                        }
                        else if (param->track_state_changed.state == AUDIO_TRACK_STATE_RELEASED)
                        {
                            track_status = CAPTRUE_TRACK_STOP;
                            dsp_capture_data_audio_track_handle = NULL;
                            dsp_capture_data_state &= ~DATA_CAPTURE_DATA_START_SCO_MODE;
                        }
                    }
#if F_APP_SAIYAN_MODE | F_APP_SUPPORT_CAPTURE_DUAL_BUD | F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
                    else if (param->track_state_changed.handle == dsp_capture_data_audio_record_handle)
                    {
                        if (param->track_state_changed.state == AUDIO_TRACK_STATE_STARTED)
                        {
                            // send H2D_SPPCAPTURE_SET cmd
                            track_status = CAPTRUE_TRACK_START;
                        }
                        else if (param->track_state_changed.state == AUDIO_TRACK_STATE_RELEASED)
                        {
                            track_status = CAPTRUE_TRACK_STOP;
                            dsp_capture_data_audio_record_handle = NULL;
                        }
                    }
#endif

                    app_data_capture_send_audio_probe();
                    app_data_capture_report(dsp_capture_data_path, dsp_capture_data_app_idx, track_status, 0);

#if F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
                    if (param->track_state_changed.state == AUDIO_TRACK_STATE_RELEASED)
                    {
                        app_data_capture_unregister();
                    }
#endif
                }
            }
            break;

        default:
            handle = false;
            break;
        }

        if (handle == true)
        {
            APP_PRINT_INFO1("app_data_capture_audio_cback: event_type 0x%04x", event_type);
        }
    }
}

#if F_APP_HIFI4_SUPPORT
static void app_data_capture_dsp2_audio_cback(uint32_t event, void *msg)
{
    switch (event)
    {
    case IPC_EVT_DSP2_READY:
        {
        }
        break;

    case IPC_EVT_D2H_CMD:
        {
        }
        break;

    case IPC_EVT_DATA_IND:
        {
            //FOR SPP CAPTURE DSP2 DATA
            uint32_t data_length = 0;
            uint8_t *dsp2_buf = NULL;
            uint8_t type = 0;
            uint32_t actual_queue_id = 0;
            uint16_t cur_seq = 0, pre_seq = 0;

            data_length = ipc_dsp2_data_length_peek(actual_queue_id);
            dsp2_buf = malloc(data_length);
            if (dsp2_buf == NULL)
            {
                return;
            }

            ipc_dsp2_data_recv(&type, dsp2_buf, data_length, actual_queue_id);

            cur_seq = (dsp2_buf[2] | dsp2_buf[3] << 8);
            pre_seq = (cur_seq == 0) ? 0xFFFF : cur_seq - 1;

            if (dsp2_capture_data_seq != pre_seq)
            {
                APP_PRINT_INFO3("rcv_capture_dsp2_data, sync_word=%x, curr_seq=%x, pre_seq=%x",
                                (dsp2_buf[0] | (dsp2_buf[1] << 8)), (dsp2_buf[2] | (dsp2_buf[3] << 8)), pre_seq);

            }

            app_report_event(dsp_capture_data_path, EVENT_DATA_CAPTURE_DATA, dsp_capture_data_app_idx, dsp2_buf,
                             data_length);
            free(dsp2_buf);
            dsp2_buf = NULL;
            dsp2_capture_data_seq = cur_seq;

        }
        break;

    default:
        break;
    }
}
#endif

void app_data_capture_dsp_event_cback(uint32_t event, void *msg)
{
    switch (event)
    {
    case AUDIO_PROBE_DSP_EVT_MAILBOX_DSP_DATA:
        {
            //FOR SPP CAPTURE DSP DATA
            T_PROBE_CB_MAILBOX_DATA *p_info = (T_PROBE_CB_MAILBOX_DATA *)msg;
            uint16_t cur_seq = (p_info->p_data[2] | p_info->p_data[3] << 8);
            uint16_t pre_seq = (cur_seq == 0) ? 0xFFFF : cur_seq - 1;

            if (dsp_capture_data_seq != pre_seq)
            {
                APP_PRINT_INFO3("app_data_capture_dsp_event_cback pre_seq %d curr_seq %d drop_seq %d",
                                dsp_capture_data_seq,
                                (p_info->p_data[2] | p_info->p_data[3] << 8),
                                pre_seq);
            }
            //APP_PRINT_TRACE1("rcv_capture_data seq 0x%x", cur_seq);
            //APP_PRINT_INFO1("rcv_capture_data %b",TRACE_BINARY(p_info->data_len, p_info->p_data));
            app_report_event(dsp_capture_data_path, EVENT_DATA_CAPTURE_DATA, dsp_capture_data_app_idx,
                             p_info->p_data, p_info->data_len);
            dsp_capture_data_seq = cur_seq;
        }
        break;

    default:
        break;
    }
}

bool app_data_capture_get_record_handle(void)
{
    return (dsp_capture_data_audio_record_handle != NULL);
}

uint16_t app_data_capture_get_state(void)
{
    return dsp_capture_data_state;
}

bool app_data_capture_executing_check(void)
{
    return (dsp_capture_data_state & (DATA_CAPTURE_DATA_LOG_EXECUTING | DATA_CAPTURE_RAW_DATA_EXECUTING
                                      | DATA_CAPTURE_DATA_SAIYAN_EXECUTING | DATA_CAPTURE_DATA_USER_MIC_EXECUTING |
                                      DATA_CAPTURE_DATA_ACOUSTICS_MP_EXECUTING))
           ? true : false;
}

static void app_data_capture_send_audio_probe(void)
{
    if (dsp_capture_data_cmd_ptr)
    {
        audio_probe_dsp_send(dsp_capture_data_cmd_ptr, dsp_capture_data_cmd_len);
        free(dsp_capture_data_cmd_ptr);
        dsp_capture_data_cmd_ptr = NULL;
    }
}

#if F_APP_HIFI4_SUPPORT
static void app_data_capture_dsp2_send_audio_probe(void)
{
    if (dsp_capture_data_cmd_ptr)
    {
        ipc_dsp2_cmd_send(dsp_capture_data_cmd_ptr, dsp_capture_data_cmd_len, true);
        free(dsp_capture_data_cmd_ptr);
        dsp_capture_data_cmd_ptr = NULL;
    }
    else
    {
        APP_PRINT_ERROR0("app_data_capture_dsp2_send_audio_probe NULL");
    }
}
#endif

void app_data_capture_start_process(T_CAPTURE_HEADER *param, uint8_t cmd_path, uint8_t app_idx)
{
    uint32_t actual_mhz;
    uint16_t pkt_type = GAP_PKT_TYPE_DM1 | GAP_PKT_TYPE_DH1 | \
                        GAP_PKT_TYPE_DM3 | GAP_PKT_TYPE_DH3 | \
                        GAP_PKT_TYPE_DM5 | GAP_PKT_TYPE_DH5 | \
                        GAP_PKT_TYPE_NO_2DH1 | GAP_PKT_TYPE_NO_2DH3 | GAP_PKT_TYPE_NO_2DH5;

    dsp_capture_data_state |= DATA_CAPTURE_DATA_ENTER_START;

    app_dlps_disable(APP_DLPS_ENTER_CHECK_SPP_CAPTURE);
    app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_SPP_CAPTURE);

    if (cmd_path == CMD_PATH_LE)
    {
        app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_DATA_CAPTURE_START);
    }

    app_auto_power_off_disable(AUTO_POWER_OFF_MASK_DATA_CAPTURE);

    // if ((T_BT_ACL_PKT_TYPE)param->bandwith == BT_ACL_PKT_TYPE_3M)
    // {
    //     gap_br_vendor_data_rate_set(0); // force 3M not support yet, 0 : 2M/3M
    // }
    gap_br_cfg_acl_pkt_type(app_db.br_link[app_idx].bd_addr, pkt_type); // set 3M only
    pm_cpu_freq_set(100, &actual_mhz);
    app_db.data_capture_tpoll = param->tpoll;
    app_bt_policy_b2s_tpoll_update(app_db.br_link[app_idx].bd_addr, BP_TPOLL_EVENT_DATA_CAPTURE_START);

    /*if(app_db.br_link[app_idx].acl_link_role == BT_LINK_ROLE_SLAVE)
    {
        dsp_capture_data_state |= DATA_CAPTURE_DATA_SWAP_TO_MASTER;
        bt_link_role_switch(app_db.br_link[app_idx].bd_addr, true);
    }*/

    if (param->flush_tout)
    {
        gap_br_cfg_acl_link_flush_tout(app_db.br_link[app_idx].bd_addr, param->flush_tout);
    }
    app_bt_policy_abandon_engage();
}

void app_data_capture_stop_process(uint8_t cmd_path, uint8_t app_idx)
{
    uint32_t actual_mhz;
    uint32_t plan_profs;
    uint32_t bond_flag;

    dsp_capture_data_state &= ~DATA_CAPTURE_DATA_ENTER_START;

    if (dsp_capture_data_audio_record_handle)
    {
        audio_track_release(dsp_capture_data_audio_record_handle);
    }

    if (dsp_capture_data_state & DATA_CAPTURE_DATA_CHANGE_TO_SCO_MODE)
    {
        if (dsp_capture_data_audio_track_handle != NULL)
        {
            audio_track_release(dsp_capture_data_audio_track_handle);
        }
        bt_bond_flag_get(app_db.br_link[app_idx].bd_addr, &bond_flag);
        if (bond_flag & (BOND_FLAG_HFP | BOND_FLAG_HSP | BOND_FLAG_A2DP))
        {
            plan_profs = app_bt_policy_get_profs_by_bond_flag(bond_flag);
            app_bt_policy_default_connect(app_db.br_link[app_idx].bd_addr, plan_profs, false);
        }
    }

    app_dlps_enable(APP_DLPS_ENTER_CHECK_SPP_CAPTURE);

    if (cmd_path == CMD_PATH_LE)
    {
        app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_DATA_CAPTURE_STOP);
    }

    app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_SPP_CAPTURE);
    bt_acl_pkt_type_set(app_db.br_link[app_idx].bd_addr, BT_ACL_PKT_TYPE_2M);
    pm_cpu_freq_set(APP_BASE_CPU_FREQ, &actual_mhz);

    //bt_link_role_switch(app_db.br_link[app_idx].bd_addr, false);
    app_bt_policy_b2s_tpoll_update(app_db.br_link[app_idx].bd_addr, BP_TPOLL_EVENT_DATA_CAPTURE_STOP);

    if (dsp_capture_data_state & DATA_CAPTURE_DATA_ENTER_TEST_MODE)
    {
        dsp_capture_data_state = 0;
        dsp_capture_data_state |= DATA_CAPTURE_DATA_ENTER_TEST_MODE;
    }
    else
    {
        dsp_capture_data_state = 0;
    }
}

static void app_data_capture_recorder_create(T_CAPTURE_SBC_RECORD *sbc_record)
{
    T_AUDIO_FORMAT_INFO format_info;
    format_info.attr.sbc.sample_rate = sbc_record->sample_rate;

    format_info.type = AUDIO_FORMAT_TYPE_SBC;
    format_info.attr.sbc.chann_mode         = (T_AUDIO_SBC_CHANNEL_MODE)sbc_record->chann_mode;
    format_info.attr.sbc.block_length       = sbc_record->block_length;
    format_info.attr.sbc.subband_num        = sbc_record->subband_num;
    format_info.attr.sbc.allocation_method  = sbc_record->allocation_method;
    format_info.attr.sbc.bitpool            = sbc_record->bitpool;
    if (format_info.attr.sbc.chann_mode == AUDIO_SBC_CHANNEL_MODE_MONO)
    {
        format_info.attr.sbc.chann_location = AUDIO_CHANNEL_LOCATION_MONO;
    }
    else
    {
        format_info.attr.sbc.chann_location = AUDIO_CHANNEL_LOCATION_FL | AUDIO_CHANNEL_LOCATION_FR;
    }

    if (dsp_capture_data_audio_record_handle)
    {
        audio_track_release(dsp_capture_data_audio_record_handle);
    }

    dsp_capture_data_audio_record_handle = audio_track_create(AUDIO_STREAM_TYPE_RECORD,
                                                              AUDIO_STREAM_MODE_NORMAL,
                                                              AUDIO_STREAM_USAGE_LOCAL,
                                                              format_info,
                                                              0,
                                                              1,
                                                              AUDIO_DEVICE_IN_MIC,
                                                              NULL,
                                                              NULL);

    audio_track_latency_set(dsp_capture_data_audio_record_handle, 15, false);
    audio_track_start(dsp_capture_data_audio_record_handle);
}

static void app_data_capture_enter_sco_pre_handle(uint8_t app_idx)
{
    uint32_t plan_profs;
    dsp_capture_data_state |= DATA_CAPTURE_DATA_CHANGE_TO_SCO_MODE;
    plan_profs = (app_db.br_link[app_idx].connected_profile & (~RDTP_PROFILE_MASK) &
                  (~SPP_PROFILE_MASK));
    if (plan_profs)
    {
        app_bt_policy_disconnect(app_db.br_link[app_idx].bd_addr, plan_profs);
    }
    app_start_timer(&timer_idx_dsp_spp_captrue_check_link, "dsp_spp_captrue_check_link",
                    app_data_capture_timer_id, APP_TIMER_DATA_CAPTRUE_CHECK_LINK, app_idx, false,
                    1500);
}

static uint8_t app_data_capture_a2dp_hfp_mask_check(uint8_t app_idx)
{
    T_AUDIO_TRACK_STATE state;
    uint8_t mask = 0;
    for (uint8_t app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
    {
        if (app_db.br_link[app_idx].a2dp_track_handle)
        {
            audio_track_state_get(app_db.br_link[app_idx].a2dp_track_handle, &state);
            if ((state == AUDIO_TRACK_STATE_CREATED) ||
                (state == AUDIO_TRACK_STATE_STARTED) ||
                (state == AUDIO_TRACK_STATE_RESTARTED))
            {
                mask |= CAPTURE_MODE_MASK_A2DP;
            }
        }
        if (app_db.br_link[app_idx].sco_track_handle)
        {
            audio_track_state_get(app_db.br_link[app_idx].sco_track_handle, &state);
            if ((state == AUDIO_TRACK_STATE_CREATED) ||
                (state == AUDIO_TRACK_STATE_STARTED) ||
                (state == AUDIO_TRACK_STATE_RESTARTED))
            {
                mask |= CAPTURE_MODE_MASK_HFP;
            }
        }
    }

    return mask;
}

void app_audio_route_entry_record_update(T_AUDIO_CATEGORY      category,
                                         uint32_t              device,
                                         T_AUDIO_TRACK_HANDLE  handle,
                                         uint8_t               entry_num,
                                         T_AUDIO_ROUTE_ENTRY   entry[])
{
    T_AUDIO_ROUTE_PATH path;
    uint8_t i, j;

    APP_PRINT_TRACE1("app_audio_route_entry_record_update: category 0x%x", category);

    app_audio_route_path_unbind(category, handle);

    path = audio_route_path_take(category,
                                 device);

    if (path.entry_num == 0)
    {
        return;
    }

    for (i = 0; i < path.entry_num; i++)
    {
        for (j = 0; j < entry_num; j++)
        {
            if (path.entry[i].endpoint_attr.mic.adc_ch == entry[j].endpoint_attr.mic.adc_ch &&
                path.entry[i].endpoint_type == entry[j].endpoint_type)
            {
#if F_APP_SAIYAN_MODE
                path.entry[i].io_type                = entry[j].io_type;
                path.entry[i].endpoint_attr.mic.id   = entry[j].endpoint_attr.mic.id;
                path.entry[i].endpoint_attr.mic.type = entry[j].endpoint_attr.mic.type;
#endif
                break;
            }
            else if (path.entry[i].io_type == entry[j].io_type)
            {
#if F_APP_SUPPORT_CAPTURE_DUAL_BUD | F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
                path.entry[i].endpoint_attr.mic.id     = entry[j].endpoint_attr.mic.id;
                path.entry[i].endpoint_attr.mic.type   = entry[j].endpoint_attr.mic.type;
#endif
#if F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
                path.entry[i].endpoint_attr.mic.adc_ch = entry[j].endpoint_attr.mic.adc_ch;
#endif
                break;
            }
        }
    }

    app_audio_route_path_bind(category, handle, path);
    audio_route_path_give(&path);
}

#if F_APP_SUPPORT_CAPTURE_DUAL_BUD | F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
static void app_data_capture_mic_index_set(T_MIC_INDEX mic_index)
{
    app_logic_mic_bitmap |= mic_index;
}

T_MIC_INDEX app_data_capture_mic_index_get(void)
{
    return app_logic_mic_bitmap;
}

static void app_data_capture_mic_index_clear(T_MIC_INDEX mic_index)
{
    app_logic_mic_bitmap &= ~mic_index;
}

static T_APP_AUDIO_CATEGORY_LOGIC_MIC app_data_capture_get_category_mic(uint8_t logic_mic)
{
    T_APP_AUDIO_CATEGORY_LOGIC_MIC category_mic;
    switch (logic_mic)
    {
    case LOGIC_FF_MIC:
        {
            category_mic.category = AUDIO_CATEGORY_ANC;
            category_mic.mic      = AUDIO_ROUTE_IO_ANC_FF_LEFT_IN;
            app_data_capture_mic_index_set(FF_MIC);
        }
        break;

    case LOGIC_FB_MIC:
        {
            category_mic.category = AUDIO_CATEGORY_ANC;
            category_mic.mic      = AUDIO_ROUTE_IO_ANC_FB_LEFT_IN;
            app_data_capture_mic_index_set(FB_MIC);
        }
        break;

    case LOGIC_VOICE_MIC:
        {
            category_mic.category = AUDIO_CATEGORY_VOICE;
            category_mic.mic      = AUDIO_ROUTE_IO_VOICE_PRIMARY_IN;
            app_data_capture_mic_index_set(VOICE_MIC);
        }
        break;

    case LOGIC_SECONDARY_VOICE_MIC:
        {
            category_mic.category = AUDIO_CATEGORY_VOICE;
            category_mic.mic      = AUDIO_ROUTE_IO_VOICE_SECONDARY_IN;
            app_data_capture_mic_index_set(SECONDARY_VOICE_MIC);
        }
        break;

    case LOGIC_PRIMARY_APT_MIC:
        {
            category_mic.category = AUDIO_CATEGORY_APT;
            category_mic.mic      = AUDIO_ROUTE_IO_APT_FF_LEFT_IN;
            app_data_capture_mic_index_set(PRIMARY_APT_MIC);
        }
        break;

    case LOGIC_SECONDARY_APT_MIC:
        {
            category_mic.category = AUDIO_CATEGORY_APT;
            category_mic.mic      = AUDIO_ROUTE_IO_APT_FB_LEFT_IN;
            app_data_capture_mic_index_set(SECONDARY_APT_MIC);
        }
        break;

    default:
        {
            category_mic.category = AUDIO_CATEGORY_NUM;
            category_mic.mic      = AUDIO_ROUTE_IO_AUDIO_PRIMARY_OUT;
        }
    }
    return category_mic;
}

static void app_data_capture_record_update(T_CAPTURE_USER_MIC_SBC_RECORD *sbc_record,
                                           uint8_t                        record_num)
{
    T_APP_AUDIO_CATEGORY_LOGIC_MIC category_mic;
    T_AUDIO_ROUTE_ENTRY update_entry[2];
    T_AUDIO_ROUTE_IO_TYPE record_io[2] = {AUDIO_ROUTE_IO_RECORD_PRIMARY_IN, AUDIO_ROUTE_IO_RECORD_SECONDARY_IN};
    uint8_t logic_mic[2] = {sbc_record->logic_mic1, sbc_record->logic_mic2};
    uint8_t mic_num = 0;
    uint8_t i;

    memset(&update_entry, 0, sizeof(update_entry));

    for (i = 0; i < record_num; i++)
    {
        if (logic_mic[i] != LOGIC_INVALID_MIC)
        {
            mic_num += 1;
            category_mic = app_data_capture_get_category_mic(logic_mic[i]);

            app_audio_route_entry_get(category_mic.category,
                                      AUDIO_DEVICE_IN_MIC | AUDIO_DEVICE_OUT_SPK,
                                      category_mic.mic,
                                      &update_entry[i]);

            if (update_entry[i].io_type != record_io[i])
            {
                update_entry[i].io_type = record_io[i];
            }
        }
    }

    app_audio_route_entry_record_update(AUDIO_CATEGORY_RECORD,
                                        AUDIO_DEVICE_IN_MIC,
                                        dsp_capture_data_audio_record_handle,
                                        mic_num,
                                        update_entry);
}
#endif

void app_data_capture_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                                 uint8_t app_idx, uint8_t *ack_pkt)
{
    uint8_t data_idx = 0;
    uint8_t *p_data = &cmd_ptr[8];
    uint8_t capture_type = 0;
    uint8_t capture_len = 0;
    bool stop_process = false;
    uint8_t capture_mask = 0;

    T_CAPTURE_HEADER throughput_param;

    APP_PRINT_TRACE3("app_data_capture_cmd_handle capture_mode %d data_state %d %b",
                     dsp_capture_data_mode, dsp_capture_data_state,
                     TRACE_BINARY(cmd_len, cmd_ptr));

    memcpy(&throughput_param, &cmd_ptr[2], sizeof(T_CAPTURE_HEADER));
    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

    dsp_capture_data_app_idx = app_idx;
    dsp_capture_data_path = cmd_path;

    if (app_data_capture_executing_check() == throughput_param.capture_enable)
    {
        app_data_capture_report(dsp_capture_data_path, dsp_capture_data_app_idx,
                                throughput_param.capture_enable, 0);
        APP_PRINT_TRACE2("app_data_capture_cmd_handle check state %d %d",
                         dsp_capture_data_state,
                         throughput_param.capture_enable);
    }

    if (throughput_param.capture_enable)
    {
        capture_mask = app_data_capture_a2dp_hfp_mask_check(app_idx);
    }

    if (throughput_param.type_num)
    {
        for (uint8_t i = 0; i < throughput_param.type_num; i++)
        {
            capture_type = p_data[data_idx++];
            capture_len  = p_data[data_idx++];
            APP_PRINT_TRACE4("app_data_capture_cmd_handle enable %d type %d len %d data_idx %d",
                             throughput_param.capture_enable,
                             capture_type,
                             capture_len,
                             data_idx);

            if (throughput_param.capture_enable)
            {
                /*need check capture_mask when enter fack sco mode*/
                if (capture_type != APP_DATA_CAPTURE_ENTER_SCO_MODE)
                {
                    app_data_capture_start_process(&throughput_param, cmd_path, app_idx);
                }

                switch (capture_type)
                {
#if F_APP_SAIYAN_MODE
                case APP_DATA_CAPTURE_SAIYAN:
                    {
                        T_CAPTURE_SAIYAN_SBC_RECORD saiyan_sbc_record;
                        T_CAPTURE_SBC_RECORD sbc_record;
                        memcpy(&saiyan_sbc_record, &p_data[data_idx], capture_len);
                        memcpy((uint8_t *)&sbc_record, (uint8_t *)&saiyan_sbc_record.sample_rate, sizeof(sbc_record));

                        T_AUDIO_ROUTE_ENTRY update_entry[2];
                        memset(&update_entry, 0, sizeof(update_entry));

                        update_entry[0].endpoint_attr.mic.adc_ch = AUDIO_ROUTE_ADC_CHANN0;
                        update_entry[0].io_type = (T_AUDIO_ROUTE_IO_TYPE)saiyan_sbc_record.ch0_io_type;
                        update_entry[0].endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)saiyan_sbc_record.ch0_mic_sel;
                        update_entry[0].endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)saiyan_sbc_record.ch0_mic_type;
                        update_entry[0].endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;

                        update_entry[1].endpoint_attr.mic.adc_ch = AUDIO_ROUTE_ADC_CHANN1;
                        update_entry[1].io_type = (T_AUDIO_ROUTE_IO_TYPE)saiyan_sbc_record.ch1_io_type;
                        update_entry[1].endpoint_attr.mic.id = (T_AUDIO_ROUTE_MIC_ID)saiyan_sbc_record.ch1_mic_sel;
                        update_entry[1].endpoint_attr.mic.type = (T_AUDIO_ROUTE_MIC_TYPE)saiyan_sbc_record.ch1_mic_type;
                        update_entry[1].endpoint_type = AUDIO_ROUTE_ENDPOINT_MIC;

                        app_data_capture_recorder_create(&sbc_record);

                        app_audio_route_entry_record_update(AUDIO_CATEGORY_RECORD,
                                                            AUDIO_DEVICE_IN_MIC | AUDIO_DEVICE_IN_AUX,
                                                            dsp_capture_data_audio_record_handle,
                                                            2,
                                                            update_entry);

                        dsp_capture_data_state |= DATA_CAPTURE_DATA_SAIYAN_EXECUTING;
                    }
                    break;
#endif

#if F_APP_SUPPORT_CAPTURE_DUAL_BUD
                case APP_DATA_CAPTURE_USER_MIC:
                    {
                        T_CAPTURE_USER_MIC_SBC_RECORD user_mic_sbc_record;
                        T_CAPTURE_SBC_RECORD sbc_record;

                        memcpy((uint8_t *)&user_mic_sbc_record, &p_data[data_idx], capture_len);
                        memcpy((uint8_t *)&sbc_record, (uint8_t *)&user_mic_sbc_record.sample_rate, sizeof(sbc_record));

                        app_data_capture_recorder_create(&sbc_record);

                        app_data_capture_record_update(&user_mic_sbc_record, 1);

                        dsp_capture_data_state |= DATA_CAPTURE_DATA_USER_MIC_EXECUTING;
                    }
                    break;
#endif

#if F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
                case APP_DATA_CAPTURE_ACOUSTICS_MP:
                    {
                        if ((dsp_capture_data_state & DATA_CAPTURE_DATA_ACOUSTICS_MP_EXECUTING) != 0)
                        {
                            return;
                        }

                        T_CAPTURE_USER_MIC_SBC_RECORD user_mic_sbc_record;
                        T_CAPTURE_SBC_RECORD sbc_record;

#ifndef CONFIG_SOC_SERIES_RTL8773D
                        audio_probe_dsp_test_bin_set(true);
#endif
                        app_data_capture_register();
                        memcpy((uint8_t *)&user_mic_sbc_record, &p_data[data_idx], capture_len);
                        memcpy((uint8_t *)&sbc_record, (uint8_t *)&user_mic_sbc_record.sample_rate, sizeof(sbc_record));

                        app_data_capture_recorder_create(&sbc_record);

                        app_data_capture_record_update(&user_mic_sbc_record, 2);

                        dsp_capture_data_state |= DATA_CAPTURE_DATA_ACOUSTICS_MP_EXECUTING;
                    }
                    break;
#endif

                case APP_DATA_CAPTURE_RAW_DATA:
                    {
                        dsp_capture_data_state |= DATA_CAPTURE_RAW_DATA_EXECUTING;
                    }
                    break;

                case APP_DATA_CAPTURE_ENTER_SCO_MODE:
                    {
                        if (capture_mask == 0)
                        {
                            app_data_capture_start_process(&throughput_param, cmd_path, app_idx);
                            app_data_capture_enter_sco_pre_handle(app_idx);
                        }
                        else
                        {
                            app_data_capture_report(dsp_capture_data_path, dsp_capture_data_app_idx, CAPTRUE_TRACK_STOP,
                                                    capture_mask);
                        }
                    }
                    break;

                case APP_DATA_CAPTURE_SCO_MODE_CTL:
                    {
                        dsp_capture_data_state |= DATA_CAPTURE_DATA_START_SCO_MODE;
                    }
                    break;
#if F_APP_HIFI4_SUPPORT
                case APP_DATA_CAPTURE_DSP2_DATA:
                    {
                        dsp_capture_data_state &= ~DATA_CAPTURE_DATA_START_SCO_MODE;
                        dsp_capture_data_state |= DATA_CAPTURE_DATA_DSP2_MODE;
                        dsp2_capture_data_seq = 0xFFFF;
                    }
                    break;
#endif
                }
            }
            else
            {
                if ((capture_type == APP_DATA_CAPTURE_SAIYAN) ||
                    (capture_type == APP_DATA_CAPTURE_RAW_DATA) ||
                    (capture_type == APP_DATA_CAPTURE_ENTER_SCO_MODE) ||
                    (capture_type == APP_DATA_CAPTURE_USER_MIC) ||
                    (capture_type == APP_DATA_CAPTURE_ACOUSTICS_MP))
                {
                    stop_process = true;
                }
            }
            data_idx += capture_len;
        }
    }
    else
    {
        stop_process = true;
    }

    if (capture_type != APP_DATA_CAPTURE_DSP2_DATA)
    {
        dsp_capture_data_seq = 0xFFFF;
    }

    dsp_capture_data_cmd_len = cmd_len - data_idx - sizeof(T_CAPTURE_HEADER) - 2;
    dsp_capture_data_cmd_ptr = malloc(dsp_capture_data_cmd_len);
    memcpy(dsp_capture_data_cmd_ptr, &p_data[data_idx], dsp_capture_data_cmd_len);

    if ((dsp_capture_data_state & DATA_CAPTURE_RAW_DATA_EXECUTING) ||
        (dsp_capture_data_state & DATA_CAPTURE_DATA_START_SCO_MODE) ||
        ((dsp_capture_data_state & DATA_CAPTURE_DATA_SAIYAN_EXECUTING) && (stop_process)) ||
        ((dsp_capture_data_state & DATA_CAPTURE_DATA_USER_MIC_EXECUTING) && (stop_process)) ||
        ((dsp_capture_data_state & DATA_CAPTURE_DATA_ACOUSTICS_MP_EXECUTING) && (stop_process)))
    {
        app_data_capture_send_audio_probe();
    }
#if F_APP_HIFI4_SUPPORT
    else if (dsp_capture_data_state & DATA_CAPTURE_DATA_DSP2_MODE)
    {
        app_data_capture_dsp2_send_audio_probe();
    }
#endif

    if (stop_process)
    {
#if F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
        app_data_capture_mic_index_clear(app_logic_mic_bitmap);
#endif
        app_data_capture_stop_process(cmd_path, app_idx);
    }
}

static void app_data_capture_bt_event_cback(T_BT_EVENT event_type, void *event_buf,
                                            uint16_t buf_len)
{
    bool handle = true;
    T_BT_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BT_EVENT_SPP_DISCONN_CMPL:
    case BT_EVENT_ACL_CONN_DISCONN:
        {
            if ((dsp_capture_data_state & DATA_CAPTURE_DATA_ENTER_TEST_MODE) != 0)
            {
                if (dsp_capture_data_mode)
                {
                    if ((event_type == BT_EVENT_ACL_CONN_DISCONN) &&
                        (app_link_check_b2s_link(param->acl_conn_disconn.bd_addr)))
                    {
                        app_data_capture_stop_process(dsp_capture_data_path, 0);
                        dsp_capture_data_mode = 0;
                        audio_probe_dsp_test_bin_set(false);
                        app_data_capture_unregister();
                    }
                }
#if F_APP_SUPPORT_CAPTURE_DUAL_BUD | F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
#else
                if (event_type == BT_EVENT_SPP_DISCONN_CMPL)
                {
                    app_bt_policy_disconnect_all_link();
                }
#endif
            }
        }
        break;

#if F_APP_SUPPORT_CAPTURE_DUAL_BUD | F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
    case BT_EVENT_REMOTE_ROLESWAP_STATUS:
        {
            T_BT_ROLESWAP_STATUS roleswap_status = param->remote_roleswap_status.status;

            if (roleswap_status == BT_ROLESWAP_STATUS_SUCCESS)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    bud_side_trig_roleswap = 0;
                }
                else
                {
                    if (bud_side_trig_roleswap != 0)
                    {
                        app_report_rws_bud_info();
                        bud_side_trig_roleswap = 0;
                    }
                }
            }
        }
        break;

    case BT_EVENT_SCO_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link = app_link_find_br_link(param->sco_conn_ind.bd_addr);
            if (p_link != NULL)
            {
                if (app_data_capture_get_state() != 0)
                {
                    bt_hfp_audio_disconnect_req(param->sco_conn_ind.bd_addr);
                }
            }
        }
        break;
#endif

    case BT_EVENT_ACL_ROLE_MASTER:
        {
            if ((dsp_capture_data_state & DATA_CAPTURE_DATA_ENTER_TEST_MODE) != 0)
            {
                if (dsp_capture_data_state & DATA_CAPTURE_DATA_SWAP_TO_MASTER)
                {
                    app_data_capture_send_audio_probe();
                    dsp_capture_data_state |= DATA_CAPTURE_DATA_START_MASK;
                    dsp_capture_data_state &= ~DATA_CAPTURE_DATA_SWAP_TO_MASTER;
                }
            }
        }
        break;

    case BT_EVENT_ACL_ROLE_SWITCH_FAIL:
        {
            if ((dsp_capture_data_state & DATA_CAPTURE_DATA_ENTER_TEST_MODE) != 0)
            {
                if (dsp_capture_data_state & DATA_CAPTURE_DATA_SWAP_TO_MASTER)
                {
                    if (dsp_capture_data_master_retry < 3)
                    {
                        bt_link_role_switch(param->acl_role_switch_fail.bd_addr, true);
                        dsp_capture_data_master_retry++;
                    }
                    else
                    {
                        ///if ((dsp_capture_data_state & DATA_CAPTURE_DATA_SAIYAN_EXECUTING) == 0)
                        {
                            app_data_capture_send_audio_probe();
                        }
                        dsp_capture_data_state |= DATA_CAPTURE_DATA_START_MASK;
                        dsp_capture_data_state &= ~DATA_CAPTURE_DATA_SWAP_TO_MASTER;
                        dsp_capture_data_master_retry = 0;
                    }
                }
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_data_capture_bt_event_cback: event_type 0x%04x", event_type);
    }
}

static void app_data_capture_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_data_capture_timeout_cb %d %d", timer_evt, param);
    switch (timer_evt)
    {
    case APP_TIMER_DATA_CAPTRUE_CHECK_LINK:
        {
            app_stop_timer(&timer_idx_dsp_spp_captrue_check_link);

            if ((app_db.br_link[param].connected_profile & (~RDTP_PROFILE_MASK) & (~SPP_PROFILE_MASK)))
            {
                app_start_timer(&timer_idx_dsp_spp_captrue_check_link, "dsp_spp_captrue_check_link",
                                app_data_capture_timer_id, APP_TIMER_DATA_CAPTRUE_CHECK_LINK, param, false,
                                1500);
            }
            else
            {
                T_AUDIO_FORMAT_INFO format_info;

                format_info.type = AUDIO_FORMAT_TYPE_MSBC;
                format_info.frame_num = 1;
                format_info.attr.msbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_MONO;
                format_info.attr.msbc.sample_rate = 16000;
                format_info.attr.msbc.bitpool = 26;
                format_info.attr.msbc.allocation_method = 0;
                format_info.attr.msbc.subband_num = 8;
                format_info.attr.msbc.block_length = 15;
                format_info.attr.msbc.chann_location = AUDIO_CHANNEL_LOCATION_MONO;

                if (dsp_capture_data_audio_track_handle)
                {
                    audio_track_release(dsp_capture_data_audio_track_handle);
                }

                dsp_capture_data_audio_track_handle = audio_track_create(AUDIO_STREAM_TYPE_VOICE,
                                                                         AUDIO_STREAM_MODE_NORMAL,
                                                                         AUDIO_STREAM_USAGE_LOCAL,
                                                                         format_info,
                                                                         app_dsp_cfg_vol.voice_out_volume_default,
                                                                         app_dsp_cfg_vol.voice_volume_in_default,
                                                                         AUDIO_DEVICE_OUT_SPK | AUDIO_DEVICE_IN_MIC,
                                                                         NULL,
                                                                         NULL);

                audio_track_latency_set(dsp_capture_data_audio_track_handle, 15, false);
                audio_track_start(dsp_capture_data_audio_track_handle);
            }
        }
        break;

    default:
        break;
    }
}

static void app_data_capture_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    switch (event_type)
    {
    case SYS_EVENT_POWER_ON:
        {
#if F_APP_SAIYAN_MODE
            if (data_capture_saiyan.saiyan_enable)
            {
                app_auto_power_off_enable(AUTO_POWER_OFF_MASK_DATA_CAPTURE, APP_DATA_CAPTURE_POWER_OFF_TIME);
#ifndef CONFIG_SOC_SERIES_RTL8773D
                audio_probe_dsp_test_bin_set(true);
#endif
            }
#endif
        }
        break;
    }
    APP_PRINT_TRACE1("app_data_capture_dm_cback %d", event_type);
}

static void app_data_capture_register(void)
{
    APP_PRINT_TRACE0("app_data_capture_register");
    dsp_capture_data_state |= DATA_CAPTURE_DATA_ENTER_TEST_MODE;
    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DATA_CAPTURE,
                                        ROLESWAP_EVENT_DATA_CAPTURE_DATA_STATE, (uint8_t *)&dsp_capture_data_state,
                                        sizeof(dsp_capture_data_state));
    audio_mgr_cback_register(app_data_capture_audio_cback);
    audio_probe_dsp_evt_cback_register(app_data_capture_dsp_event_cback);
    sys_mgr_cback_register(app_data_capture_dm_cback);
#if F_APP_HIFI4_SUPPORT
    dsp2_ipc_handle = app_ipc_subscribe(DSP2_IPC_TOPIC, app_data_capture_dsp2_audio_cback);
#endif
}

static void app_data_capture_unregister(void)
{
    APP_PRINT_TRACE0("app_data_capture_unregister");
    dsp_capture_data_state &= ~DATA_CAPTURE_DATA_ENTER_TEST_MODE;
    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DATA_CAPTURE,
                                        ROLESWAP_EVENT_DATA_CAPTURE_DATA_STATE, (uint8_t *)&dsp_capture_data_state,
                                        sizeof(dsp_capture_data_state));
    audio_mgr_cback_unregister(app_data_capture_audio_cback);
    audio_probe_dsp_evt_cback_unregister(app_data_capture_dsp_event_cback);
    sys_mgr_cback_unregister(app_data_capture_dm_cback);
#if F_APP_HIFI4_SUPPORT
    app_ipc_unsubscribe(dsp2_ipc_handle);
#endif
}

#if F_APP_SUPPORT_CAPTURE_DUAL_BUD | F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
bool app_data_capture_need_roleswap(void)
{
    return (bud_side_trig_roleswap != 0) ? true : false;
}
#endif

void app_data_capture_mode_ctl(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                               uint8_t app_idx, uint8_t *ack_pkt)
{
    uint8_t dsp_data_capture_scenario = cmd_ptr[0];
    uint8_t report_data[2] = {0};
    uint8_t report_mask = 0;

#if F_APP_SUPPORT_CAPTURE_DUAL_BUD | F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
    capture_data_dual_bud.cfg = 0;
#endif

    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

    if (dsp_data_capture_scenario != 0)
    {
        if (dsp_capture_data_state & DATA_CAPTURE_DATA_ENTER_TEST_MODE)
        {
            dsp_capture_data_mode = dsp_data_capture_scenario;
            report_data[0] = CAP_REPORT_ST_SUC;
            report_data[1] = 0;
            goto capture_mode_report;
        }
    }

    report_mask = app_data_capture_a2dp_hfp_mask_check(app_idx);

#if F_APP_SUPPORT_CAPTURE_DUAL_BUD | F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
    if (cmd_len >= 2)
    {
        capture_data_dual_bud.cfg = cmd_ptr[1];
        if (((capture_data_dual_bud.left_caputre != 0) &&
             (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT)) ||
            ((capture_data_dual_bud.right_caputre != 0) && (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT)))
        {
            report_mask |= CAPTURE_MODE_MASK_BUD_SIDE;
            bud_side_trig_roleswap = 1;
        }
    }
#endif

#if F_APP_APT_SUPPORT
    if (app_apt_is_normal_apt_on_state(app_db.current_listening_state) ||
        app_listening_is_anc_apt_on_state(app_db.current_listening_state))
    {
        report_mask |= CAPTURE_MODE_MASK_DSP_APT;
    }
#endif

    if (app_data_capture_executing_check())
    {
        report_mask |= CAPTURE_MODE_MASK_DATA_CAPTURE;
    }

    report_data[1] = report_mask;

    if (report_mask == 0)
    {
        dsp_capture_data_mode = dsp_data_capture_scenario;
#ifndef CONFIG_SOC_SERIES_RTL8773D
        audio_probe_dsp_test_bin_set(dsp_capture_data_mode);
#endif
        report_data[0] = CAP_REPORT_ST_SUC;
    }
    else
    {
        report_data[0] = (dsp_capture_data_mode) ? CAP_REPORT_ST_EXIT_FAIL : CAP_REPORT_ST_LOAD_FAIL;
    }

    if ((dsp_capture_data_mode != 0) && (dsp_data_capture_scenario != 0))
    {
        app_data_capture_register();
    }
    else
    {
        app_data_capture_unregister();
    }

capture_mode_report:
    APP_PRINT_TRACE5("app_data_capture_mode_ctl %d %d 0x%04x %d 0x%02x",
                     dsp_data_capture_scenario, dsp_capture_data_mode, dsp_capture_data_state,
                     report_data[0],
                     report_data[1]);

    app_report_event(cmd_path, EVENT_DATA_CAPTURE_ENTER_EXIT, app_idx, report_data, 2);

#if F_APP_SUPPORT_CAPTURE_DUAL_BUD | F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
    if (bud_side_trig_roleswap != 0)
    {
        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DATA_CAPTURE,
                                            ROLESWAP_EVENT_DATA_CAPTURE_REPORT_BUD_SIDE, &bud_side_trig_roleswap, 1);
        app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_DATA_CAPTURE);
    }
#endif
}

#if F_APP_SUPPORT_CAPTURE_DUAL_BUD | F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
static void app_data_capture_roleswap_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                                  T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE2("app_data_capture_roleswap_parse_cback: msg_type 0x%04x, status %d", msg_type,
                     status);
    uint8_t para = *(uint8_t *)buf;

    switch (msg_type)
    {
    case ROLESWAP_EVENT_DATA_CAPTURE_REPORT_BUD_SIDE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                bud_side_trig_roleswap = para;
            }
        }
        break;

    case ROLESWAP_EVENT_DATA_CAPTURE_DATA_STATE:
        {
            uint8_t *p_info = (uint8_t *)buf;
            if ((status == REMOTE_RELAY_STATUS_ASYNC_RCVD) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY))
            {
                dsp_capture_data_state = (p_info[1] << 8) | p_info[0];
            }
        }
        break;

    default:
        break;
    }
}
#endif

void app_data_capture_init(void)
{
#if F_APP_SAIYAN_MODE
    ftl_load_from_storage(&data_capture_saiyan, APP_RW_SAIYAN_ADDR, APP_RW_SAIYAN_SIZE);

    if (data_capture_saiyan.saiyan_enable)
    {
        app_db.saiyan_org_role = app_cfg_const.bud_role;
        app_cfg_const.bud_role = REMOTE_SESSION_ROLE_PRIMARY;
        app_cfg_nv.bud_role = REMOTE_SESSION_ROLE_PRIMARY;
        app_data_capture_register();
    }
#endif
#if F_APP_SUPPORT_CAPTURE_DUAL_BUD | F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
    app_relay_cback_register(NULL, app_data_capture_roleswap_parse_cback,
                             APP_MODULE_TYPE_DATA_CAPTURE, ROLESWAP_EVENT_DATA_CAPTURE_MAX);
#endif
    bt_mgr_cback_register(app_data_capture_bt_event_cback);
    app_timer_reg_cb(app_data_capture_timeout_cb, &app_data_capture_timer_id);
}

#endif
