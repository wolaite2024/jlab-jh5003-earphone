/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "trace.h"
#include "sys_timer.h"
#include "audio_mgr.h"
#include "media_buffer.h"
#include "jitter_buffer.h"

#define LONGTERM_ASRC_LIMITATION 40
#define AVERAGE_WINDOW 30
#define AVERAGE_CHECK_WINDOW 500

#define NORMAL_ASRC_PID_LIMITATION_PPM 2000
#define NORMAL_PID_ON_LOWER_THRESHOLD_ERROR_MS 25
#define NORMAL_PID_ON_UPPER_THRESHOLD_ERROR_MS 25
#define NORMAL_PID_OFF_LOWER_THRESHOLD_ERROR_MS 5
#define NORMAL_PID_OFF_UPPER_THRESHOLD_ERROR_MS 5
#define NORMAL_PID_KP 0.035
#define NORMAL_PID_KI 0.005
#define NORMAL_PID_KD 0.01
#define NORMAL_TGT_OFFSET 0

#define LOW_LAT_ASRC_PID_LIMITATION_PPM 2000
#define LOW_LAT_PID_ON_LOWER_THRESHOLD_ERROR_MS 11
#define LOW_LAT_PID_ON_UPPER_THRESHOLD_ERROR_MS 11
#define LOW_LAT_PID_OFF_LOWER_THRESHOLD_ERROR_MS 5
#define LOW_LAT_PID_OFF_UPPER_THRESHOLD_ERROR_MS 5
#define LOW_LAT_TGT_OFFSET 0

#define ULTRA_LOW_LAT_ASRC_PID_LIMITATION_PPM 2000
#define ULTRA_LOW_LAT_PID_ON_UPPER_THRESHOLD_ERROR_MS 1
#define ULTRA_LOW_LAT_PID_ON_LOWER_THRESHOLD_ERROR_MS 1
#define ULTRA_LOW_LAT_PID_OFF_UPPER_THRESHOLD_ERROR_MS 1
#define ULTRA_LOW_LAT_PID_OFF_LOWER_THRESHOLD_ERROR_MS 1
#define ULTRA_LOW_LAT_PID_KP 0.08
#define ULTRA_LOW_LAT_PID_KI 0.01
#define ULTRA_LOW_LAT_PID_KD 0.04
#define ULTRA_LOW_LAT_TGT_OFFSET 5

typedef struct t_asrc_pid
{
    float    current_asrc_ppm;
    float    last_excute_asrc_ppm;
    int32_t  error;
    int32_t  total_error;
    int32_t  error_1;
    float    kp, ki, kd;
    float    T;
    int32_t  asrc_limitation_ppm;
    int32_t  pid_on_upper_threshold_error_ms;
    int32_t  pid_on_lower_threshold_error_ms;
    int32_t  pid_off_upper_threshold_error_ms;
    int32_t  pid_off_lower_threshold_error_ms;
    int32_t  offset;
    bool     is_on;
} T_ASRC_PID;

typedef struct t_jitter_buffer_instance
{
    struct t_jitter_buffer_instance  *p_next;
    T_MEDIA_BUFFER_ENTITY            *buffer_ent;
    T_JITTER_BUFFER_EVT_CBACK         cback;
    float                             average_latency;
    uint16_t                          average_count;
    bool                              block_asrc_adjust;
    uint8_t                           id;
    T_ASRC_PID                        asrc_pid;
    void                             *asrc_resume_timer;
    uint32_t                          asrc_restore_clk;
    uint16_t                          asrc_cancel_threshold;
    bool                              asrc_accelerate;
    bool                              deviation_detected;
    uint16_t                          limit_latency;
    uint16_t                          target_latency;
} T_JITTER_BUFFER_INSTANCE;

typedef struct t_jitter_buffer_db
{
    T_OS_QUEUE     handles;
    uint8_t        id;
} T_JITTER_BUFFER_DB;

static T_JITTER_BUFFER_DB *jb_db;

bool jb_init(void)
{
    jb_db = calloc(1, sizeof(T_JITTER_BUFFER_DB));
    if (jb_db != NULL)
    {
        os_queue_init(&jb_db->handles);
        return true;
    }

    return false;
}

void jb_deinit(void)
{
    T_JITTER_BUFFER_INSTANCE *jb_instance;

    if (jb_db)
    {
        jb_instance = os_queue_out(&jb_db->handles);
        while (jb_instance != NULL)
        {
            if (jb_instance->asrc_resume_timer != NULL)
            {
                sys_timer_delete(jb_instance->asrc_resume_timer);
                jb_instance->asrc_resume_timer = NULL;
            }

            free(jb_instance);
            jb_instance = os_queue_out(&jb_db->handles);
        }

        free(jb_db);
        jb_db = NULL;
    }
}

bool jitter_buffer_handle_check(T_JITTER_BUFFER_HANDLE handle)
{
    T_JITTER_BUFFER_INSTANCE *jb_instance = handle;

    if (jb_instance)
    {
        return os_queue_search(&jb_db->handles, jb_instance);
    }

    return false;
}

void jitter_buffer_target_latency_update(T_JITTER_BUFFER_HANDLE handle,
                                         uint32_t               latency)
{
    T_JITTER_BUFFER_INSTANCE *jb_instance = handle;

    if (jb_instance)
    {
        uint32_t target_latency;

        target_latency = jb_instance->limit_latency;
        if (latency > target_latency)
        {
            target_latency = latency;
        }

        jb_instance->target_latency = jb_instance->asrc_pid.offset + target_latency;
    }
}

T_JITTER_BUFFER_HANDLE jitter_buffer_register(void                      *buffer_ent,
                                              T_JITTER_BUFFER_EVT_CBACK  cback)
{
    T_JITTER_BUFFER_INSTANCE *jb_instance;

    jb_instance = calloc(1, sizeof(T_JITTER_BUFFER_INSTANCE));
    if (jb_instance != NULL)
    {
        T_AUDIO_STREAM_MODE mode;

        jb_instance->buffer_ent = buffer_ent;
        jb_instance->cback = cback;
        jb_instance->id = jb_db->id++;
        jb_instance->average_count = 0;
        jb_instance->average_latency = 0;
        jb_instance->block_asrc_adjust = false;
        jb_instance->limit_latency = 0;

        media_buffer_get_mode(buffer_ent, &mode);
        if (mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
        {
            jb_instance->asrc_pid.T = LATENCY_REPORT_PERIOD_MS / 1000;
            jb_instance->asrc_pid.asrc_limitation_ppm = ULTRA_LOW_LAT_ASRC_PID_LIMITATION_PPM;
            jb_instance->asrc_pid.kp = ULTRA_LOW_LAT_PID_KP;
            jb_instance->asrc_pid.ki = ULTRA_LOW_LAT_PID_KI;
            jb_instance->asrc_pid.kd = ULTRA_LOW_LAT_PID_KD;
            jb_instance->asrc_pid.pid_on_upper_threshold_error_ms =
                ULTRA_LOW_LAT_PID_ON_UPPER_THRESHOLD_ERROR_MS;
            jb_instance->asrc_pid.pid_on_lower_threshold_error_ms =
                ULTRA_LOW_LAT_PID_ON_LOWER_THRESHOLD_ERROR_MS;
            jb_instance->asrc_pid.pid_off_upper_threshold_error_ms =
                ULTRA_LOW_LAT_PID_OFF_UPPER_THRESHOLD_ERROR_MS;
            jb_instance->asrc_pid.pid_off_lower_threshold_error_ms =
                ULTRA_LOW_LAT_PID_OFF_LOWER_THRESHOLD_ERROR_MS;
            jb_instance->asrc_pid.offset = ULTRA_LOW_LAT_TGT_OFFSET;
        }
        else if (mode == AUDIO_STREAM_MODE_LOW_LATENCY)
        {
            jb_instance->asrc_pid.T = LATENCY_REPORT_PERIOD_MS / 1000;
            jb_instance->asrc_pid.asrc_limitation_ppm = LOW_LAT_ASRC_PID_LIMITATION_PPM;
            jb_instance->asrc_pid.kp = NORMAL_PID_KP;
            jb_instance->asrc_pid.ki = NORMAL_PID_KI;
            jb_instance->asrc_pid.kd = NORMAL_PID_KD;

            jb_instance->asrc_pid.pid_on_upper_threshold_error_ms = LOW_LAT_PID_ON_UPPER_THRESHOLD_ERROR_MS;
            jb_instance->asrc_pid.pid_on_lower_threshold_error_ms = LOW_LAT_PID_ON_LOWER_THRESHOLD_ERROR_MS;
            jb_instance->asrc_pid.pid_off_upper_threshold_error_ms = LOW_LAT_PID_OFF_LOWER_THRESHOLD_ERROR_MS;
            jb_instance->asrc_pid.pid_off_lower_threshold_error_ms = LOW_LAT_PID_OFF_LOWER_THRESHOLD_ERROR_MS;
            jb_instance->asrc_pid.offset = LOW_LAT_TGT_OFFSET;
        }
        else
        {
            jb_instance->asrc_pid.T = LATENCY_REPORT_PERIOD_MS / 1000;
            jb_instance->asrc_pid.asrc_limitation_ppm = NORMAL_ASRC_PID_LIMITATION_PPM;
            jb_instance->asrc_pid.kp = NORMAL_PID_KP;
            jb_instance->asrc_pid.ki = NORMAL_PID_KI;
            jb_instance->asrc_pid.kd = NORMAL_PID_KD;
            jb_instance->asrc_pid.pid_on_upper_threshold_error_ms = NORMAL_PID_ON_UPPER_THRESHOLD_ERROR_MS;
            jb_instance->asrc_pid.pid_on_lower_threshold_error_ms = NORMAL_PID_ON_LOWER_THRESHOLD_ERROR_MS;
            jb_instance->asrc_pid.pid_off_upper_threshold_error_ms = NORMAL_PID_OFF_UPPER_THRESHOLD_ERROR_MS;
            jb_instance->asrc_pid.pid_off_lower_threshold_error_ms = NORMAL_PID_OFF_LOWER_THRESHOLD_ERROR_MS;
            jb_instance->asrc_pid.offset = NORMAL_TGT_OFFSET;
        }

        os_queue_in(&jb_db->handles, jb_instance);
    }

    return jb_instance;
}

void jitter_buffer_unregister(T_JITTER_BUFFER_HANDLE handle)
{
    T_JITTER_BUFFER_INSTANCE *jb_instance = handle;

    if (jb_instance)
    {
        if (os_queue_search(&jb_db->handles, handle) == true)
        {
            os_queue_delete(&jb_db->handles, handle);
            free(jb_instance);
        }
    }
}

T_JITTER_BUFFER_HANDLE jitter_buffer_find_handle_by_id(uint8_t id)
{
    T_JITTER_BUFFER_INSTANCE *jb_instance;

    jb_instance = os_queue_peek(&jb_db->handles, 0);
    while (jb_instance)
    {
        if (jb_instance->id != id)
        {
            jb_instance = jb_instance->p_next;
        }
        else
        {
            break;
        }
    }

    return jb_instance;
}

void jitter_buffer_timeout_cback(T_SYS_TIMER_HANDLE handle)
{
    uint32_t timer_id;
    T_JITTER_BUFFER_INSTANCE *jb_instance;

    timer_id = sys_timer_id_get(handle);
    AUDIO_PRINT_TRACE2("jitter_buffer_timeout_cback: timer_id 0x%02X handle %p",
                       timer_id, handle);

    jb_instance = jitter_buffer_find_handle_by_id(timer_id);
    if (jb_instance != NULL)
    {
        if (jb_instance->buffer_ent != NULL)
        {
            sys_timer_delete(jb_instance->asrc_resume_timer);
            jb_instance->asrc_resume_timer = NULL;

            jb_instance->cback(JITTER_BUFFER_EVT_LOW_LAT_ASRC_RESTORE_REQ,
                               &jb_instance->asrc_restore_clk, jb_instance->buffer_ent);
        }
    }
}

void jitter_buffer_asrc_ratio_adjust(T_JITTER_BUFFER_HANDLE handle,
                                     uint32_t               timestamp,
                                     int32_t                ratio_ppm)
{
    T_JITTER_BUFFER_INSTANCE *jb_instance = handle;
    T_JITTER_BUFFER_ASRC_ADJ msg;

    if (jitter_buffer_handle_check(handle) == true)
    {
        msg.asrc_ppm = ratio_ppm;
        msg.timestamp = timestamp;
        AUDIO_PRINT_INFO2("jitter_buffer_asrc_ratio_adjust: clk 0x%x ppm %d",
                          timestamp, ratio_ppm);
        jb_instance->cback(JITTER_BUFFER_EVT_ASRC_ADJ, &msg, jb_instance->buffer_ent);
    }
}

void jitter_buffer_asrc_pid_reset(T_JITTER_BUFFER_HANDLE handle)
{
    T_JITTER_BUFFER_INSTANCE *jb_instance = handle;
    T_ASRC_PID *asrc_pid;
    if (jitter_buffer_handle_check(handle) == true)
    {
        asrc_pid = &jb_instance->asrc_pid;
        //AUDIO_PRINT_TRACE0("jitter_buffer_asrc_pid_reset");
        asrc_pid->is_on = false;
        asrc_pid->error = 0;
        asrc_pid->total_error = 0;
        asrc_pid->error_1 = 0;
        asrc_pid->current_asrc_ppm = 0;
        asrc_pid->last_excute_asrc_ppm = 0;
        jb_instance->average_count = 0;
        jb_instance->average_latency = 0;
        jb_instance->deviation_detected = false;
    }
}

void jitter_buffer_buffer_reset(T_JITTER_BUFFER_HANDLE handle)
{
    T_JITTER_BUFFER_INSTANCE *jb_instance = handle;

    if (jitter_buffer_handle_check(handle) == true)
    {
        if (jb_instance->asrc_resume_timer != NULL)
        {
            sys_timer_delete(jb_instance->asrc_resume_timer);
            jb_instance->asrc_resume_timer = NULL;
            jb_instance->asrc_restore_clk = 0;
        }

        jitter_buffer_asrc_pid_reset(handle);
        jb_instance->block_asrc_adjust = false;
    }
}

void jitter_buffer_asrc_pid_unblock(T_JITTER_BUFFER_HANDLE handle)
{
    T_JITTER_BUFFER_INSTANCE *jb_instance = handle;

    if (jitter_buffer_handle_check(handle) == true)
    {
        AUDIO_PRINT_TRACE0("jitter_buffer_asrc_pid_unblock");
        jb_instance->block_asrc_adjust = false;
    }
}

void jitter_buffer_asrc_pid_block(T_JITTER_BUFFER_HANDLE handle)
{
    T_JITTER_BUFFER_INSTANCE *jb_instance = handle;

    if (jitter_buffer_handle_check(handle) == true)
    {
        AUDIO_PRINT_TRACE0("jitter_buffer_asrc_pid_block");
        if (jb_instance->block_asrc_adjust == false)
        {
            T_JITTER_BUFFER_ASRC_ADJ msg;

            jb_instance->block_asrc_adjust = true;
            msg.asrc_ppm = 0;
            jitter_buffer_asrc_pid_reset(jb_instance);
            jb_instance->cback(JITTER_BUFFER_EVT_ASRC_PID_ADJ_CANCEL, &msg, jb_instance->buffer_ent);
        }
    }
}

bool jitter_buffer_asrc_pid_is_block(T_JITTER_BUFFER_HANDLE handle)
{
    T_JITTER_BUFFER_INSTANCE *jb_instance = handle;

    if (jitter_buffer_handle_check(handle) == true)
    {
        return jb_instance->block_asrc_adjust;
    }

    return false;
}

bool jitter_buffer_asrc_pid_adjust(T_JITTER_BUFFER_HANDLE handle,
                                   uint32_t               latency_report)
{
    T_JITTER_BUFFER_INSTANCE *jb_instance = handle;
    T_MEDIA_BUFFER_ENTITY *buffer_ent;
    T_ASRC_PID *asrc_pid;
    T_JITTER_BUFFER_ASRC_ADJ msg;
    float adjust_kp = 0;
    float adjust_ki = 0;
    float curdev = 0;
    int32_t error;
    int8_t k = 2;
    bool error_in_on_thres = false;
    bool error_in_off_thres = false;
    bool first_adjust = false;

    if (jitter_buffer_handle_check(handle) == false)
    {
        return false;
    }

    buffer_ent = jb_instance->buffer_ent;
    asrc_pid = &jb_instance->asrc_pid;

    error = latency_report - jb_instance->target_latency;
    if (error >= 0)
    {
        if (error > asrc_pid->pid_on_upper_threshold_error_ms)
        {
            error_in_on_thres = false;
        }
        else
        {
            error_in_on_thres = true;
        }
        if (error > asrc_pid->pid_off_upper_threshold_error_ms)
        {
            error_in_off_thres = false;
        }
        else
        {
            error_in_off_thres = true;
        }
    }
    else
    {
        if (-error > asrc_pid->pid_on_lower_threshold_error_ms)
        {
            error_in_on_thres = false;
        }
        else
        {
            error_in_on_thres = true;
        }
        if (-error > asrc_pid->pid_off_lower_threshold_error_ms)
        {
            error_in_off_thres = false;
        }
        else
        {
            error_in_off_thres = true;
        }
    }

    if (asrc_pid->is_on == false)
    {

        if (error_in_on_thres == false)
        {
            if (jb_instance->deviation_detected == true)
            {
                jb_instance->deviation_detected = false;
                jitter_buffer_asrc_pid_reset(handle);
                asrc_pid->is_on = true;
                first_adjust = true;
                buffer_ent->ignore_latency_report = true;
                jb_instance->average_count = 0;
            }
            else
            {
                jb_instance->deviation_detected = true;
                return false;
            }
        }
        else
        {
            jb_instance->average_count++;
            jb_instance->average_latency = jb_instance->average_latency * (AVERAGE_WINDOW - 1) /
                                           AVERAGE_WINDOW +
                                           latency_report / AVERAGE_WINDOW;
            if (jb_instance->average_count > AVERAGE_CHECK_WINDOW)
            {
                int32_t check_error;
                jb_instance->average_count = 0;
                check_error = jb_instance->average_latency - jb_instance->target_latency;
                msg.asrc_ppm = (check_error * 1000000) / (LATENCY_REPORT_PERIOD_MS * AVERAGE_CHECK_WINDOW);
                AUDIO_PRINT_INFO2("jitter_buffer_asrc_pid_func: longterm asrc adjust err %d ppm %d",
                                  check_error, msg.asrc_ppm);
                if (msg.asrc_ppm > LONGTERM_ASRC_LIMITATION)
                {
                    msg.asrc_ppm = LONGTERM_ASRC_LIMITATION;
                }
                else if (msg.asrc_ppm < -LONGTERM_ASRC_LIMITATION)
                {
                    msg.asrc_ppm = -LONGTERM_ASRC_LIMITATION;
                }
                if (jb_instance->block_asrc_adjust == false)
                {
                    jb_instance->cback(JITTER_BUFFER_EVT_ASRC_LONGTERM_ADJ_REQ, &msg, jb_instance->buffer_ent);
                    return true;
                }

            }

            return false;
        }
    }

    asrc_pid->error = latency_report - jb_instance->target_latency;
    asrc_pid->total_error += asrc_pid->error;
    adjust_kp = asrc_pid->kp;
    adjust_ki = asrc_pid->ki;

    if (asrc_pid->pid_on_lower_threshold_error_ms == asrc_pid->pid_off_lower_threshold_error_ms)
    {
        k = 5;
    }

    if (asrc_pid->error > 0)
    {
        if (asrc_pid->error > k * asrc_pid->pid_on_upper_threshold_error_ms)
        {
            adjust_kp = asrc_pid->kp;
            adjust_ki = 0;
            asrc_pid->total_error = 0;
        }
    }
    else
    {
        if (-asrc_pid->error > k * asrc_pid->pid_on_lower_threshold_error_ms)
        {
            adjust_kp = asrc_pid->kp;
            adjust_ki = 0;
            asrc_pid->total_error = 0;
        }
    }

    curdev = asrc_pid->kd * (asrc_pid->error - asrc_pid->error_1) / asrc_pid->T;
    asrc_pid->current_asrc_ppm = (adjust_kp * asrc_pid->error +
                                  adjust_ki * asrc_pid->T * asrc_pid->total_error + curdev) * 1000;

    if (abs((int32_t)asrc_pid->current_asrc_ppm) > asrc_pid->asrc_limitation_ppm)
    {
        if (asrc_pid->current_asrc_ppm > 0)
        {
            asrc_pid->current_asrc_ppm = asrc_pid->asrc_limitation_ppm;
        }
        else
        {
            asrc_pid->current_asrc_ppm = -asrc_pid->asrc_limitation_ppm;
        }
    }

    AUDIO_PRINT_INFO4("jitter_buffer_asrc_pid_adjust: ppm %d error %d latency_report %d tgt latency %d",
                      (int32_t)(asrc_pid->current_asrc_ppm),
                      (int32_t)(asrc_pid->error), latency_report, jb_instance->target_latency);

    asrc_pid->error_1 = asrc_pid->error;
    //return 0x8000000 / 1000000 * asrc_pid->current_asrc_ppm;
    if (error_in_off_thres)
    {
        asrc_pid->is_on = false;
        jitter_buffer_asrc_pid_reset(jb_instance);
        msg.asrc_ppm = 0;
        jb_instance->cback(JITTER_BUFFER_EVT_ASRC_PID_ADJ_CLOSE, &msg, jb_instance->buffer_ent);
        return false;
    }
    else if ((int32_t)asrc_pid->current_asrc_ppm == (int32_t)asrc_pid->last_excute_asrc_ppm)
    {
        AUDIO_PRINT_TRACE0("jitter_buffer_asrc_pid_func: same ppm");
        return false;
    }
    else
    {
        msg.asrc_ppm = asrc_pid->current_asrc_ppm;
        asrc_pid->last_excute_asrc_ppm = asrc_pid->current_asrc_ppm;
        if (first_adjust)
        {
            jb_instance->cback(JITTER_BUFFER_EVT_ASRC_PID_ADJ_OPEN, &msg, jb_instance->buffer_ent);
        }
        else
        {
            jb_instance->cback(JITTER_BUFFER_EVT_ASRC_PID_ADJ, &msg, jb_instance->buffer_ent);
        }

        return true;
    }
}

void jitter_buffer_low_latency_asrc_restore(T_JITTER_BUFFER_HANDLE handle, uint32_t restore_clk)
{
    T_JITTER_BUFFER_INSTANCE *jb_instance = handle;

    if (jitter_buffer_handle_check(handle) == false)
    {
        return;
    }

    if (jb_instance->asrc_resume_timer)
    {
        sys_timer_delete(jb_instance->asrc_resume_timer);
        jb_instance->asrc_resume_timer = NULL;
    }

    jitter_buffer_asrc_ratio_adjust(jb_instance, restore_clk, 0);
    jb_instance->asrc_restore_clk = 0xffffffff;
}

void jitter_buffer_low_latency_adjust_latency(T_JITTER_BUFFER_HANDLE           handle,
                                              T_JITTER_BUFFER_EVT_LOW_LAT_ADJ *params)
{
    T_JITTER_BUFFER_INSTANCE *jb_instance = handle;

    if (jitter_buffer_handle_check(handle) == false)
    {
        return;
    }

    if (params->policy == LATENCY_KEEPER_POLICY_BY_PLC ||
        params->policy == LATENCY_KEEPER_POLICY_BY_BOTH)
    {
        if (params->dec_frame_num != 0)
        {
            jb_instance->cback(JITTER_BUFFER_EVT_LAT_ADJ_PLC_REQ, &params->dec_frame_num,
                               jb_instance->buffer_ent);
        }
    }
    if (params->policy == LATENCY_KEEPER_POLICY_BY_ASRC ||
        params->policy == LATENCY_KEEPER_POLICY_BY_BOTH)
    {
        jb_instance->asrc_accelerate = params->accelerate;
        if (params->accelerate)
        {
            jitter_buffer_asrc_ratio_adjust(jb_instance, params->sync_adj_clk, params->ratio);
        }
        else
        {
            jitter_buffer_asrc_ratio_adjust(jb_instance, params->sync_adj_clk, -params->ratio);
        }

        if (jb_instance->asrc_resume_timer != NULL)
        {
            sys_timer_delete(jb_instance->asrc_resume_timer);
        }

        jb_instance->asrc_resume_timer = sys_timer_create("jb_asrc_restore",
                                                          SYS_TIMER_TYPE_LOW_PRECISION,
                                                          jb_instance->id,
                                                          params->duration * 1000 - 100000,
                                                          false,
                                                          jitter_buffer_timeout_cback);

        if (jb_instance->asrc_resume_timer != NULL)
        {
            sys_timer_start(jb_instance->asrc_resume_timer);
        }

        if (params->sync_adj_clk == 0xffffffff)
        {
            jb_instance->asrc_restore_clk = 0xffffffff;
        }
        else
        {
            jb_instance->asrc_restore_clk = params->restore_clk;
        }
    }
}

void jitter_buffer_low_latency_keeper(T_JITTER_BUFFER_HANDLE handle,
                                      uint32_t               latency_report_ms)
{
    T_MEDIA_BUFFER_ENTITY *buffer_ent;
    T_JITTER_BUFFER_INSTANCE *jb_instance = handle;
    T_JITTER_BUFFER_EVT_LOW_LAT_ADJ_REQ msg;

    if (jitter_buffer_handle_check(handle) == false)
    {
        return;
    }

    buffer_ent = jb_instance->buffer_ent;

    if (buffer_ent == NULL)
    {
        return;
    }

    if (jb_instance->asrc_resume_timer)
    {
        bool cancel_asrc = false;
        if (jb_instance->asrc_accelerate)
        {
            if (latency_report_ms < jb_instance->asrc_cancel_threshold)
            {
                cancel_asrc = true;
            }
        }
        else
        {
            if (latency_report_ms > jb_instance->asrc_cancel_threshold)
            {
                cancel_asrc = true;
            }
        }
        if (cancel_asrc)
        {
            jb_instance->cback(JITTER_BUFFER_EVT_LOW_LAT_ASRC_RESTORE_CANCEL, &msg, jb_instance->buffer_ent);
            AUDIO_PRINT_WARN2("AUDIO_MSG_STREAM_LATENCY_REPORT: latency reach cancel threshold %u in advance, target %u",
                              jb_instance->asrc_cancel_threshold, buffer_ent->latency);

            sys_timer_delete(jb_instance->asrc_resume_timer);
            jb_instance->asrc_resume_timer = NULL;
        }
        return;
    }
    else
    {
        if (buffer_ent->mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
        {
            if (abs((int32_t)latency_report_ms - jb_instance->target_latency) <= 5)
            {
                return;
            }
        }
        else
        {
            if (abs((int32_t)latency_report_ms - jb_instance->target_latency) <= 10)
            {
                return;
            }
        }
    }

    uint16_t diff_time_ms = 0;
    bool accelerate = false;

    if (latency_report_ms > jb_instance->target_latency)
    {
        accelerate = true;
        diff_time_ms = latency_report_ms - jb_instance->target_latency;
    }
    else
    {
        accelerate = false;
        diff_time_ms = jb_instance->target_latency - latency_report_ms;
    }

    if (diff_time_ms)
    {
        if (accelerate)
        {
            jb_instance->asrc_cancel_threshold = jb_instance->target_latency +
                                                 LOW_LATENCY_ASRC_PROTECTION_THRESHOLD / 2;
        }
        else
        {
            jb_instance->asrc_cancel_threshold = jb_instance->target_latency -
                                                 LOW_LATENCY_ASRC_PROTECTION_THRESHOLD / 2;
        }
        msg.diff_ms = diff_time_ms;
        msg.accelerate = accelerate;
        jb_instance->cback(JITTER_BUFFER_EVT_ASRC_ADJ_REQ, &msg, jb_instance->buffer_ent);
    }
}

int32_t jitter_buffer_compensation_get(T_JITTER_BUFFER_HANDLE handle)
{
    T_JITTER_BUFFER_INSTANCE *jb_instance = handle;

    if (jb_instance)
    {
        return jb_instance->asrc_pid.offset;
    }

    return 0;
}

uint16_t jitter_buffer_target_latency_get(T_JITTER_BUFFER_HANDLE handle)
{
    T_JITTER_BUFFER_INSTANCE *jb_instance = handle;

    if (jb_instance)
    {
        return jb_instance->target_latency;
    }

    return 0;
}
