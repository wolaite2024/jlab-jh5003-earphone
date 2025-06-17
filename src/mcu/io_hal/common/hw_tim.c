/*
 * Copyright (c) 2021, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <stdio.h>
#include "rtl876x_tim.h"
#include "rtl876x_nvic.h"
#include "rtl876x_gdma.h"
#include "hw_tim.h"
#include "os_sync.h"
#include "trace.h"
#include "section.h"
#include "fmc_platform.h"
#include "pm.h"
#include "vector_table.h"

#if defined(IC_TYPE_RTL87x3D)
#include "platform_utils.h"
#else
#define count_1bits(x) (uint8_t)__builtin_popcount(x)
#endif

typedef struct _T_HW_TIMER
{
    union
    {
        struct
        {
            uint32_t timer_id:               4;
            uint32_t block_dlps_when_active: 1;
            uint32_t clock_source:           2;
            uint32_t pwm_deadzone_enable:    1;
            uint32_t pwm_pin_index:          8;
            uint32_t pwm_p_pin_index:        8;
            uint32_t pwm_n_pin_index:        8;
        };
        uint32_t flag;
    };

    const char *user_name;
    P_HW_TIMER_CBACK cback;
    void *context;
} T_HW_TIMER;

typedef struct _T_HW_TIMER_TABLE
{
    T_HW_TIMER item[HW_TIMER_ID_MAX];
} T_HW_TIMER_TABLE;

typedef union
{
    struct
    {
        uint32_t support_pwm_deadzone       : 1;
        uint32_t support_dma                : 1;
        uint32_t support_pwm                : 1;
        uint32_t reserved                   : 29;
    };
    uint32_t timer_feature;
} T_HW_TIMER_FEATURE;

#if defined(IC_TYPE_RTL87x3D)
#define GENERAL_TIMER_NEED_SAFE_STOP
#define IC_TIMER_CHANNEL_NUM                                                    (16)
#define HW_TIMER_MASK                                                           (0xFFFC)
#define HW_GENERAL_TIMER_DYNAMIC_MASK                                           (0xF700 | BIT5 | BIT2 | BIT3 | BIT4 | BIT6)
#define HW_TIMER_DYNAMIC_MASK                                                   HW_GENERAL_TIMER_DYNAMIC_MASK
#define HW_TIMER_DMA_SUPPORT_MASK                                               (0xFC)
#define HW_TIMER_PWM_DEADZONE_MASK                                              (BIT2 | BIT3)
#define HW_TIMER_SUPPORT_GENERAL_IP                                             (1)
#define HW_TIMER_DMA_HANDSHAKE                                                  (GDMA_Handshake_TIM1 - 1)
#define hw_general_timer_get_reg_base_from_id(id)                               (TIM_TypeDef *)((id < 8) ? (TIM0_REG_BASE + 0x14 * id) : (TIM8_REG_BASE + 0x14 * (id - 8)))
#define hw_general_timer_get_raw_int_status(id)                                 ((id < 8) ? ((TIMA_CHANNELS->TimersRawIntStatus) & (BIT(id))) : ((TIMB_CHANNELS->TimersRawIntStatus) & (BIT(id - 8))))
#define hw_general_timer_is_one_shot(TIMx)                                      ((TIMx->ControlReg & BIT1) ? false : true)
#define p_timer_driver_init(id, period_us, reload, callback, dma_support)       hw_general_timer_driver_init_int(id, period_us, reload, callback, dma_support)
#define p_hw_timer_start(id, int_en)                                            hw_general_timer_start(id, int_en)
#define p_hw_timer_stop(id, int_en)                                             hw_general_timer_stop(id, int_en)
#define p_hw_timer_restart(id, int_en, period_us)                               hw_general_timer_restart(id, int_en, period_us)
#define p_hw_timer_get_current_count(id, p_current_count)                       hw_general_timer_get_current_count(id, p_current_count)
#define p_hw_timer_get_elapsed_time(id, p_elapsed_time)                         hw_general_timer_get_elapsed_time(id, p_elapsed_time)
#define p_hw_timer_get_period(id, p_period)                                     hw_general_timer_get_period(id, p_period)
#define p_hw_timer_is_active(id, is_active)                                     hw_general_timer_is_active(id, is_active)
#define p_hw_timer_set_auto_reload(id, is_auto_reload)                          hw_general_timer_set_auto_reload(id, is_auto_reload)
#define p_hw_timer_print_debug_info(id)                                         hw_general_timer_print_debug_info(id)
#define p_hw_timer_update_period(id, int_en,one_shot_period_us,period_us)       hw_general_timer_update_period(id, int_en,one_shot_period_us,period_us)
#define p_hw_timer_pm_get_status(active_status, next_wake_item,pwm_en,id)       hw_general_timer_pm_get_status(active_status, next_wake_item,pwm_en,id)
extern IRQ_Fun hw_timer_get_isr(T_HW_TIMER_ID id);
extern bool hw_timer_update_isr(VECTORn_Type v_num, IRQ_Fun isr_handler, bool is_xip);
#elif defined(IC_TYPE_RTL87x3EU)
#define IC_TIMER_CHANNEL_NUM                                                    (9)
#define HW_TIMER_MASK                                                           (0x1FC)
#define HW_RTK_TIMER_DYNAMIC_MASK                                               (BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2)
#define HW_TIMER_DYNAMIC_MASK                                                   HW_RTK_TIMER_DYNAMIC_MASK
#define HW_TIMER_DMA_SUPPORT_MASK                                               (0x1F0)
#define HW_TIMER_PWM_SUPPORT_MASK                                               (0x1F0)
#define HW_TIMER_PWM_DEADZONE_MASK                                              (BIT6 | BIT7 | BIT8)
#define HW_TIMER_SUPPORT_RTK_IP                                                 (1)
#define HW_TIMER_DMA_HANDSHAKE                                                  (GDMA_Handshake_TIM1_CH4 - 4)
#define hw_rtk_timer_is_one_shot(TIMx)                                          ((TIMx->TIMER_MODE_CFG & BIT8) ? true : false)
#define hw_rtk_timer_get_reg_base_from_id(id)                                   (TIM_TypeDef *)(TIMER1_BASE + 0x50 * id)
#define p_timer_driver_init(id, period_us, reload, callback, dma_support)       hw_rtk_timer_driver_init_int(id, period_us, reload, callback, dma_support)
#define p_hw_timer_start(id, int_en)                                            hw_rtk_timer_start(id, int_en)
#define p_hw_timer_stop(id, int_en)                                             hw_rtk_timer_stop(id, int_en)
#define p_hw_timer_restart(id, int_en, period_us)                               hw_rtk_timer_restart(id, int_en, period_us)
#define p_hw_timer_get_current_count(id, p_current_count)                       hw_rtk_timer_get_current_count(id, p_current_count)
#define p_hw_timer_get_elapsed_time(id, p_elapsed_time)                         hw_rtk_timer_get_elapsed_time(id, p_elapsed_time)
#define p_hw_timer_get_period(id, p_period)                                     hw_rtk_timer_get_period(id, p_period)
#define p_hw_timer_is_active(id, is_active)                                     hw_rtk_timer_is_active(id, is_active)
#define p_hw_timer_set_auto_reload(id, is_auto_reload)                          hw_rtk_timer_set_auto_reload(id, is_auto_reload)
#define p_hw_timer_print_debug_info(id)                                         hw_rtk_timer_print_debug_info(id)
#define p_hw_timer_update_period(id, int_en,one_shot_period_us,period_us)       hw_rtk_timer_update_period(id, int_en,one_shot_period_us,period_us)
#define p_hw_timer_pm_get_status(active_status, next_wake_item,pwm_en,id)       hw_rtk_timer_pm_get_status(active_status, next_wake_item,pwm_en,id)
bool hw_timer_update_isr(VECTORn_Type v_num, bool is_xip);
#elif defined(IC_TYPE_RTL87x3EP)
#define IC_TIMER_CHANNEL_NUM                                                    (12)
#define HW_TIMER_MASK                                                           (0xFFC)
#define HW_GENERAL_TIMER_DYNAMIC_MASK                                           (BIT6 | BIT5 | BIT4 | BIT3 | BIT2)
#define HW_ENH_TIMER_DYNAMIC_MASK                                               (0xF00)
#define HW_TIMER_DYNAMIC_MASK                                                   (HW_ENH_TIMER_DYNAMIC_MASK | HW_GENERAL_TIMER_DYNAMIC_MASK)
#define HW_TIMER_DMA_SUPPORT_MASK                                               (0xFC)
#define HW_TIMER_PWM_SUPPORT_MASK                                               (BIT6 | BIT5 | BIT4 | BIT3 | BIT2)
#define HW_TIMER_PWM_DEADZONE_MASK                                              (BIT2 | BIT3)
#define HW_TIMER_SUPPORT_GENERAL_IP                                             (1)
#define HW_TIMER_SUPPORT_ENH_IP                                                 (1)
#define HW_TIMER_DMA_HANDSHAKE                                                  (GDMA_Handshake_TIM1 - 1)
#define hw_general_timer_is_one_shot(TIMx)                                      ((TIMx->ControlReg & BIT1) ? false : true)
#define hw_timer_is_enh_timer(id)                                               (((id) < HW_TIMER_ID_MAX) && ((id) > HW_TIMER_ID_7))
#define hw_general_timer_get_reg_base_from_id(id)                               (TIM_TypeDef *)(TIM0_REG_BASE + 0x14 * id)
#define hw_enh_timer_get_reg_base_from_id(id)                                   (ENH_TIM_TypeDef *)(ENH_TIMER0_BASE + 0x50 * (id - HW_TIMER_ID_8))
#define hw_enh_timer_is_one_shot(ENH_TIMx)                                      ((ENH_TIMx->REG_ENH_TIMER_x_MODE_CFG & BIT8) ? true : false)
#define hw_enh_timer_get_one_shot_enable_statue(id)                             ((ENH_TIM_CTRL->REG_ENH_TIMER_ONESHOT_GO_CTRL & BIT(id)) ? true : false)
#define p_timer_driver_init(id, period_us, reload, callback, dma_support)       hw_timer_is_enh_timer(id)? \
    hw_enh_timer_driver_init_int(id, period_us, reload, callback, dma_support):hw_general_timer_driver_init_int(id, period_us, reload, callback, dma_support)
#define p_hw_timer_start(id, int_en)                                            hw_timer_is_enh_timer(id)? \
    hw_enh_timer_start(id, int_en):hw_general_timer_start(id, int_en)
#define p_hw_timer_stop(id, int_en)                                             hw_timer_is_enh_timer(id)? \
    hw_enh_timer_stop(id, int_en):hw_general_timer_stop(id, int_en)
#define p_hw_timer_restart(id, int_en, period_us)                               hw_timer_is_enh_timer(id)? \
    hw_enh_timer_restart(id, int_en, period_us):hw_general_timer_restart(id, int_en, period_us)
#define p_hw_timer_get_current_count(id, p_current_count)                       hw_timer_is_enh_timer(id)? \
    hw_enh_timer_get_current_count(id, p_current_count):hw_general_timer_get_current_count(id, p_current_count)
#define p_hw_timer_get_elapsed_time(id, p_elapsed_time)                         hw_timer_is_enh_timer(id)? \
    hw_enh_timer_get_elapsed_time((T_HW_TIMER_ID)(id-HW_TIMER_ID_8), p_elapsed_time):hw_general_timer_get_elapsed_time(id, p_elapsed_time)
#define p_hw_timer_get_period(id, p_period)                                     hw_timer_is_enh_timer(id)? \
    hw_enh_timer_get_period(id, p_period):hw_general_timer_get_period(id, p_period)
#define p_hw_timer_is_active(id, is_active)                                     hw_timer_is_enh_timer(id)? \
    hw_enh_timer_is_active((T_HW_TIMER_ID)(id-HW_TIMER_ID_8), is_active):hw_general_timer_is_active(id, is_active)
#define p_hw_timer_set_auto_reload(id, is_auto_reload)                          hw_timer_is_enh_timer(id)? \
    hw_enh_timer_set_auto_reload(id, is_auto_reload):hw_general_timer_set_auto_reload(id, is_auto_reload)
#define p_hw_timer_print_debug_info(id)                                         hw_timer_is_enh_timer(id)? \
    hw_enh_timer_print_debug_info(id):hw_general_timer_print_debug_info(id)
#define p_hw_timer_update_period(id, int_en,one_shot_period_us,period_us)       hw_timer_is_enh_timer(id)? \
    hw_enh_timer_update_period(id, int_en,one_shot_period_us,period_us):hw_general_timer_update_period(id, int_en,one_shot_period_us,period_us)
#define p_hw_timer_pm_get_status(active_status, next_wake_item,pwm_en,id)       hw_timer_is_enh_timer(id)? \
    hw_enh_timer_pm_get_status(active_status, next_wake_item,pwm_en,(T_HW_TIMER_ID)(id-HW_TIMER_ID_8)):hw_general_timer_pm_get_status(active_status, next_wake_item,pwm_en,id)
extern void hal_timer_handler(void);
#define hw_timer_get_isr(id)  hal_timer_handler
extern void hal_enh_timer_handler(void);
#define hw_enh_timer_get_isr(id)  hal_enh_timer_handler
extern bool hw_timer_update_isr(VECTORn_Type v_num, IRQ_Fun isr_handler, bool is_xip);
#elif defined(IC_TYPE_RTL87x3E)
#define GENERAL_TIMER_NEED_SAFE_STOP
#define IC_TIMER_CHANNEL_NUM                                                    (8)
#define HW_TIMER_MASK                                                           (0xFC)
#define HW_GENERAL_TIMER_DYNAMIC_MASK                                           (BIT6 | BIT5 | BIT4 | BIT3 | BIT2)
#define HW_TIMER_DYNAMIC_MASK                                                   HW_GENERAL_TIMER_DYNAMIC_MASK
#define HW_TIMER_DMA_SUPPORT_MASK                                               (0xFC)
#define HW_TIMER_PWM_DEADZONE_MASK                                              (BIT2 | BIT3)
#define HW_TIMER_SUPPORT_GENERAL_IP                                             (1)
#define HW_TIMER_DMA_HANDSHAKE                                                  (GDMA_Handshake_TIM1 - 1)
#define hw_general_timer_get_reg_base_from_id(id)                               (TIM_TypeDef *)(TIM0_REG_BASE + 0x14 * id)
#define hw_general_timer_get_raw_int_status(id)                                 ((TIM_CHANNELS->TimersRawIntStatus) & (BIT(id)))
#define hw_general_timer_is_one_shot(TIMx)                                      ((TIMx->ControlReg & BIT1) ? false : true)
#define p_timer_driver_init(id, period_us, reload, callback, dma_support)       hw_general_timer_driver_init_int(id, period_us, reload, callback, dma_support)
#define p_hw_timer_start(id, int_en)                                            hw_general_timer_start(id, int_en)
#define p_hw_timer_stop(id, int_en)                                             hw_general_timer_stop(id, int_en)
#define p_hw_timer_restart(id, int_en, period_us)                               hw_general_timer_restart(id, int_en, period_us)
#define p_hw_timer_get_current_count(id, p_current_count)                       hw_general_timer_get_current_count(id, p_current_count)
#define p_hw_timer_get_elapsed_time(id, p_elapsed_time)                         hw_general_timer_get_elapsed_time(id, p_elapsed_time)
#define p_hw_timer_get_period(id, p_period)                                     hw_general_timer_get_period(id, p_period)
#define p_hw_timer_is_active(id, is_active)                                     hw_general_timer_is_active(id, is_active)
#define p_hw_timer_set_auto_reload(id, is_auto_reload)                          hw_general_timer_set_auto_reload(id, is_auto_reload)
#define p_hw_timer_print_debug_info(id)                                         hw_general_timer_print_debug_info(id)
#define p_hw_timer_update_period(id, int_en,one_shot_period_us,period_us)       hw_general_timer_update_period(id, int_en,one_shot_period_us,period_us)
#define p_hw_timer_pm_get_status(active_status, next_wake_item,pwm_en,id)       hw_general_timer_pm_get_status(active_status, next_wake_item,pwm_en,id)
extern void hal_timer_handler(void);
#define hw_timer_get_isr(id)  hal_timer_handler
extern bool hw_timer_update_isr(VECTORn_Type v_num, IRQ_Fun isr_handler, bool is_xip);
#endif

#define HW_TIMER_INTTERRUPT_TIME_MIN                  (5)
#define HW_TIMER_ALLOW_ENTER_LPM_THRESHOLD_US         (30000)
#define US_TO_TICK_THRESHOLD                          (0x3FFFFFFF)
#define US_TO_PF_RTC_TICK(us)                         (((us) > US_TO_TICK_THRESHOLD)?(((us)/125) << 2):(((us) << 2)/125))
#define HW_TIMER_TOTAL_NUM                            IC_TIMER_CHANNEL_NUM
#define HW_TIMER_PM_PRINT_INTERVAL                    (5000)

static bool hw_timer_pm_check(uint32_t *p_next_wake_up_time);
extern void hw_timer_rcc_clock_cmd(T_HW_TIMER_ID id);
extern VECTORn_Type hw_timer_get_vectorn_from_id(T_HW_TIMER_ID id);
extern IRQn_Type hw_timer_get_irqn_from_id(T_HW_TIMER_ID id);

static uint16_t g_hw_tim_mask = HW_TIMER_MASK;
static bool     g_hw_timer_init_flag = false;

T_HW_TIMER_TABLE hw_timer_pool =
{
    .item[0]     =  {HW_TIMER_ID_EMPTY,   },
    .item[1]     =  {HW_TIMER_ID_EMPTY,   },
    .item[2]     =  {HW_TIMER_ID_2,       },
    .item[3]     =  {HW_TIMER_ID_3,       },
    .item[4]     =  {HW_TIMER_ID_4,       },
    .item[5]     =  {HW_TIMER_ID_5,       },
    .item[6]     =  {HW_TIMER_ID_6,       },
    .item[7]     =  {HW_TIMER_ID_7,       },
#if (IC_TIMER_CHANNEL_NUM > 8)
    .item[8]     =  {HW_TIMER_ID_8,       },
#endif
#if (IC_TIMER_CHANNEL_NUM >= 12)
    .item[9]     =  {HW_TIMER_ID_9,       },
    .item[10]    =  {HW_TIMER_ID_10,      },
    .item[11]    =  {HW_TIMER_ID_11,      },
#endif
#if (IC_TIMER_CHANNEL_NUM >= 16)
    .item[12]    =  {HW_TIMER_ID_12,      },
    .item[13]    =  {HW_TIMER_ID_13,      },
    .item[14]    =  {HW_TIMER_ID_14,      },
    .item[15]    =  {HW_TIMER_ID_15,      }
#endif
};

T_HW_TIMER_HANDLE hw_timer_pool_alloc(T_HW_TIMER_FEATURE timer_feature)
{
    uint32_t s;
    uint8_t timer_id = 0;
    uint16_t temp_mask = 0;
    T_HW_TIMER_HANDLE handle = NULL;

    if (g_hw_timer_init_flag == false)
    {
        power_check_cb_register(hw_timer_pm_check);
        g_hw_timer_init_flag = true;
    }

    s = os_lock();

    uint16_t curr_mask = g_hw_tim_mask & HW_TIMER_DYNAMIC_MASK;

    if (timer_feature.support_dma)
    {
        temp_mask = curr_mask & HW_TIMER_DMA_SUPPORT_MASK;
    }
    else
    {
        temp_mask = curr_mask & (~HW_TIMER_DMA_SUPPORT_MASK);
        if (!temp_mask)
        {
            temp_mask = curr_mask;
        }
#if defined(HW_TIMER_SUPPORT_RTK_IP) || defined(HW_TIMER_SUPPORT_ENH_IP)
        if (!(temp_mask & HW_TIMER_PWM_SUPPORT_MASK))
        {
            temp_mask |= curr_mask & HW_TIMER_PWM_SUPPORT_MASK;
        }
#else
        if (!(temp_mask & HW_TIMER_PWM_DEADZONE_MASK))
        {
            temp_mask |= curr_mask & HW_TIMER_PWM_DEADZONE_MASK;
        }
#endif
    }

#if defined(HW_TIMER_SUPPORT_RTK_IP) || defined(HW_TIMER_SUPPORT_ENH_IP)
    curr_mask = temp_mask;

    if (timer_feature.support_pwm)
    {
        temp_mask = curr_mask & HW_TIMER_PWM_SUPPORT_MASK;
    }
    else
    {
        temp_mask = curr_mask & (~HW_TIMER_PWM_SUPPORT_MASK);
        if (!temp_mask)
        {
            temp_mask = curr_mask;
        }
        else if (!(temp_mask & HW_TIMER_PWM_DEADZONE_MASK))
        {
            temp_mask |= curr_mask & HW_TIMER_PWM_DEADZONE_MASK;
        }
    }
#endif

    curr_mask = temp_mask;

    if (timer_feature.support_pwm_deadzone)
    {
        temp_mask = curr_mask & HW_TIMER_PWM_DEADZONE_MASK;
    }
    else
    {
        temp_mask = curr_mask & (~HW_TIMER_PWM_DEADZONE_MASK);
        if (!temp_mask)
        {
            temp_mask = curr_mask;
        }
    }

    if (temp_mask)
    {
        timer_id = __builtin_ctz(temp_mask);
    }
    else
    {
        timer_id = HW_TIMER_ID_EMPTY;
    }

    if ((timer_id != HW_TIMER_ID_EMPTY) && (timer_id < HW_TIMER_ID_MAX))
    {
        handle = &hw_timer_pool.item[timer_id];
        g_hw_tim_mask &= ~BIT(timer_id);
    }

    os_unlock(s);

    return handle;
}

bool hw_timer_pool_free(T_HW_TIMER_HANDLE handle)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;
    uint8_t timer_id = p_hw_timer->timer_id;
    bool ret = false;

    uint32_t s = os_lock();
    if (!(g_hw_tim_mask & BIT(timer_id)))
    {
        g_hw_tim_mask |= BIT(timer_id);
        ret = true;
    }
    else
    {
        IO_PRINT_ERROR2("free hw timer error, timer not alloc, id %x, name %s",
                        p_hw_timer->timer_id, TRACE_STRING(p_hw_timer->user_name));
    }
    os_unlock(s);

    return ret;
}

#ifdef HW_TIMER_SUPPORT_GENERAL_IP
typedef struct
{
    uint32_t REG_LoadCount;
    uint32_t REG_ControlReg;
    uint32_t REG_LoadCount2;
    uint32_t REG_CurrentValue;
} T_GENERAL_TIM_RESTORE_REG;

RAM_TEXT_SECTION void hw_general_timer_handler_imp(T_HW_TIMER_ID id)
{
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(id);

    TIM_ClearINT(TIMx);

    if (hw_general_timer_is_one_shot(TIMx))
    {
        TIM_Cmd(TIMx, DISABLE);
    }

    if (hw_timer_pool.item[id].cback)
    {
        hw_timer_pool.item[id].cback(hw_timer_pool.item[id].context);
    }
}

static void hw_general_timer_driver_init_int(T_HW_TIMER_ID id, uint32_t period_us, bool reload,
                                             uint32_t callback, bool dma_support)
{
    uint32_t period_min = 1;

    if (callback)
    {
        period_min = HW_TIMER_INTTERRUPT_TIME_MIN;
    }

    if (period_us < period_min)
    {
        /* Workaround for hw not accept 0 */
        period_us = period_min;
    }

    hw_timer_rcc_clock_cmd(id);

    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(id);

    TIM_TimeBaseInitTypeDef time_param;
    TIM_StructInit(&time_param);

    time_param.TIM_Mode = reload ? TIM_Mode_UserDefine : TIM_Mode_FreeRun;
    time_param.ClockDepend = false;
    time_param.TIM_SOURCE_DIV = TIM_CLOCK_DIVIDER_40;
    time_param.TIM_PWM_En = PWM_DISABLE;
    time_param.TIM_Period = (period_us - 1);
    time_param.PWMDeadZone_En = DEADZONE_DISABLE;
    TIM_TimeBaseInit(TIMx, &time_param);

    if (callback)
    {
        hw_timer_update_isr(hw_timer_get_vectorn_from_id(id), hw_timer_get_isr(id),
                            FMC_IS_SPIC0_ADDR((uint32_t)callback));
        NVIC_InitTypeDef nvic_param;

        nvic_param.NVIC_IRQChannel = hw_timer_get_irqn_from_id(id);
        nvic_param.NVIC_IRQChannelPriority = 3;
        nvic_param.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&nvic_param);

        TIM_ClearINT(TIMx);
        TIM_INTConfig(TIMx, ENABLE);
    }
}

RAM_TEXT_SECTION void hw_general_timer_start(T_HW_TIMER_ID timer_id, bool int_en)
{
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(timer_id);

    if (int_en)
    {
        TIM_INTConfig(TIMx, ENABLE);
    }

    TIM_Cmd(TIMx, ENABLE);
}

#ifdef GENERAL_TIMER_NEED_SAFE_STOP

#define HW_DW_TIMER_NEED_SAFE_RESTORE_MASK             (0x05)
#define HW_DW_TIMER_AND_INTERRUPT_BOTH_ENABLE          (0x01)

#if defined(IC_TYPE_RTL87x3D)
RAM_TEXT_SECTION
static void hw_general_timer_set_div(T_HW_TIMER_ID timerid, uint8_t div)
{
    if (timerid < 5)
    {
        SYSBLKCTRL->u_360.REG_PERION_REG_PERI_GTIMER_CLK_SRC0 &= ~(0x7 << (16 + timerid * 3));
        SYSBLKCTRL->u_360.REG_PERION_REG_PERI_GTIMER_CLK_SRC0 |= (div << (16 + timerid * 3));
    }
    else if ((timerid >= 5) && (timerid < 8))
    {
        SYSBLKCTRL->u_35C.REG_PERION_REG_PERI_GTIMER_CLK_SRC1 &= ~(0x7 << ((timerid - 5) * 3));
        SYSBLKCTRL->u_35C.REG_PERION_REG_PERI_GTIMER_CLK_SRC1 |= (div << ((timerid - 5) * 3));
    }
    else if ((timerid >= 8) && (timerid < 16))
    {
        SYSBLKCTRL->u_348.REG_PERION_REG_PERI_GTIMER_CLK_CTRL &= ~((uint32_t)0x7 << (8 +
                                                                                     (timerid - 8) * 3));
        SYSBLKCTRL->u_348.REG_PERION_REG_PERI_GTIMER_CLK_CTRL |= (div << (8 + (timerid - 8) * 3));
    }
}
#elif defined(IC_TYPE_RTL87x3E)
RAM_TEXT_SECTION
static void hw_general_timer_set_div(T_HW_TIMER_ID timerid, uint8_t div)
{
    uint8_t bit_offset = 0;;
    if (timerid < 5)
    {
        bit_offset =  16 + timerid * 3;
        SYSBLKCTRL->u_360.REG_PERI_GTIMER_CLK_SRC0 = (SYSBLKCTRL->u_360.REG_PERI_GTIMER_CLK_SRC0 & \
                                                      ~(0x7 << bit_offset)) | (div << bit_offset);
    }
    else if ((timerid >= 5) && (timerid < 8))
    {
        bit_offset = (timerid - 5) * 3;
        SYSBLKCTRL->u_35C.REG_PERI_GTIMER_CLK_SRC1 = (SYSBLKCTRL->u_35C.REG_PERI_GTIMER_CLK_SRC1 &\
                                                      ~(0x7 << bit_offset)) | (div << bit_offset);
    }
}
#endif

RAM_TEXT_SECTION void hw_general_timer_safe_stop(T_HW_TIMER_ID timerid)
{
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(timerid);

    if ((TIMx->ControlReg & HW_DW_TIMER_NEED_SAFE_RESTORE_MASK) ==
        HW_DW_TIMER_AND_INTERRUPT_BOTH_ENABLE)
    {
        TIM_INTConfig(TIMx, DISABLE);

        if (hw_general_timer_get_raw_int_status(timerid))
        {
            TIM_Cmd(TIMx, DISABLE);
        }
        else
        {
            //switch to 40Mhz
#if defined(IC_TYPE_RTL87x3E)
            SYSBLKCTRL->u_360.BITS_360.r_timer38_div_en = 0;
#endif
            SYSBLKCTRL->u_348.BITS_348.r_timer_cg_en = 0;
            SYSBLKCTRL->u_348.BITS_348.r_timer_div_en = 0;
            hw_general_timer_set_div(timerid, 1);
            if (TIMx->CurrentValue > 10)
            {
                TIM_Cmd(TIMx, DISABLE);
                hw_general_timer_set_div(timerid, 7);
            }
            else
            {
                //switch to 1Mhz
                hw_general_timer_set_div(timerid, 7);
                while (!hw_general_timer_get_raw_int_status(timerid));
                TIM_Cmd(TIMx, DISABLE);
            }
        }
    }
    else
    {
        TIM_Cmd(TIMx, DISABLE);
    }
}
#endif

RAM_TEXT_SECTION void hw_general_timer_stop(T_HW_TIMER_ID timer_id, bool int_en)
{
#ifdef GENERAL_TIMER_NEED_SAFE_STOP
    hw_general_timer_safe_stop(timer_id);
#else
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(timer_id);

    if (int_en)
    {
        TIM_INTConfig(TIMx, DISABLE);
        TIM_ClearINT(TIMx);
    }

    TIM_Cmd(TIMx, DISABLE);
#endif
}

RAM_TEXT_SECTION void hw_general_timer_restart(T_HW_TIMER_ID timer_id, bool int_en,
                                               uint32_t period_us)
{
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(timer_id);
    uint32_t period_min = 1;

    if (int_en)
    {
        period_min = HW_TIMER_INTTERRUPT_TIME_MIN;
    }

    if (period_us < period_min)
    {
        /* Workaround for hw not accept 0 */
        period_us = period_min;
    }

    if (int_en)
    {
#ifdef GENERAL_TIMER_NEED_SAFE_STOP
        hw_general_timer_safe_stop(timer_id);
#else
        TIM_Cmd(TIMx, DISABLE);
        TIM_INTConfig(TIMx, DISABLE);
        TIM_ClearINT(TIMx);
#endif
        TIM_ChangePeriod(TIMx, period_us - 1);
        TIM_INTConfig(TIMx, ENABLE);
    }
    else
    {
        TIM_Cmd(TIMx, DISABLE);
        TIM_ChangePeriod(TIMx, period_us - 1);
    }

    TIM_Cmd(TIMx, ENABLE);
}

RAM_TEXT_SECTION void hw_general_timer_get_current_count(T_HW_TIMER_ID timer_id, uint32_t *count)
{
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(timer_id);

    *count = TIM_GetCurrentValue(TIMx);
}

void hw_general_timer_get_elapsed_time(T_HW_TIMER_ID timer_id, uint32_t *time)
{
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(timer_id);
    bool state = false;

    state = TIM_GetStatus(TIMx);

    if (state)
    {
        *time = TIM_GetElapsedValue(TIMx);
    }
    else
    {
        *time = 0;
    }
}

void hw_general_timer_get_period(T_HW_TIMER_ID timer_id, uint32_t *period_us)
{
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(timer_id);

    *period_us = TIMx->LoadCount;
}

void hw_general_timer_is_active(T_HW_TIMER_ID timer_id, bool *state)
{
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(timer_id);

    *state = TIM_GetStatus(TIMx);
}

void hw_general_timer_set_auto_reload(T_HW_TIMER_ID timer_id, bool is_auto_reload)
{
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(timer_id);
    if (is_auto_reload)
    {
        TIMx->ControlReg |= BIT1;
    }
    else
    {
        TIMx->ControlReg &= ~BIT1;
    }
}

void hw_general_timer_update_period(T_HW_TIMER_ID timer_id, bool int_en,
                                    uint32_t one_shot_period_us,
                                    uint32_t period_us)
{
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(timer_id);
    uint32_t period_min = 1;

    if (int_en)
    {
        period_min = HW_TIMER_INTTERRUPT_TIME_MIN;
    }

    if (one_shot_period_us < period_min)
    {
        /* Workaround for hw not accept 0 */
        one_shot_period_us = period_min;
    }

    if (int_en)
    {
        TIM_INTConfig(TIMx, DISABLE);
#ifdef GENERAL_TIMER_NEED_SAFE_STOP
        hw_general_timer_safe_stop(timer_id);
#else
        TIM_Cmd(TIMx, DISABLE);
#endif
        TIM_ChangePeriod(TIMx, one_shot_period_us - 1);
        TIM_Cmd(TIMx, ENABLE);
        while (TIMx->CurrentValue > one_shot_period_us - 1);
        TIM_ChangePeriod(TIMx, period_us - 1);
        TIM_INTConfig(TIMx, ENABLE);
    }
    else
    {
        TIM_Cmd(TIMx, DISABLE);
        TIM_ChangePeriod(TIMx, one_shot_period_us - 1);
        TIM_Cmd(TIMx, ENABLE);
        while (TIMx->CurrentValue > one_shot_period_us - 1);
        TIM_ChangePeriod(TIMx, period_us - 1);
    }
}

void hw_general_timer_print_debug_info(T_HW_TIMER_ID timer_id)
{
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(timer_id);
    IO_PRINT_TRACE6("Debug hw timer id %d LoadCount 0x%x, CurrentValue 0x%x, ControlReg 0x%x, EOI 0x%x, TIMER_INTR 0x%x",
                    timer_id, TIMx->LoadCount, TIMx->CurrentValue, TIMx->ControlReg, TIMx->EOI, TIMx->IntStatus);
}

static void hw_general_timer_restore_compensate(T_HW_TIMER_ID id, T_GENERAL_TIM_RESTORE_REG *buf,
                                                uint32_t sleep_time_us)
{
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(id);
    uint32_t load_count_setting = buf->REG_CurrentValue > sleep_time_us ?
                                  (buf->REG_CurrentValue - sleep_time_us) : 5;
    *(volatile uint32_t *)(&TIMER0_LOAD_COUNT2 + id) = load_count_setting;
    TIMx->LoadCount = load_count_setting;
    TIMx->ControlReg = buf->REG_ControlReg;
    while (TIMx->CurrentValue > load_count_setting);
    TIMx->LoadCount = buf->REG_LoadCount;
}

static void hw_general_timer_restore_direct(T_HW_TIMER_ID id, T_GENERAL_TIM_RESTORE_REG *buf)
{
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(id);
    TIMx->LoadCount = buf->REG_LoadCount;
    TIMx->ControlReg = buf->REG_ControlReg;
    *(volatile uint32_t *)(&TIMER0_LOAD_COUNT2 + id) = buf->REG_LoadCount2;
}

void hw_general_timer_pm_exit(T_HW_TIMER_ID id, T_GENERAL_TIM_RESTORE_REG *buf,
                              uint32_t sleep_time_us)
{
    uint16_t used_mask = (~g_hw_tim_mask) & HW_GENERAL_TIMER_DYNAMIC_MASK;
    if (!(used_mask & BIT(id)))
    {
        return;
    }

    //if block DLPS or PWM enable or Timer disable then just restore the registers
    if ((hw_timer_pool.item[id].block_dlps_when_active == 0) && ((buf->REG_ControlReg & BIT3) == 0) && \
        (buf->REG_ControlReg & BIT0))
    {
        hw_general_timer_restore_compensate(id, buf, sleep_time_us);
    }
    else
    {
        hw_general_timer_restore_direct(id, buf);
    }
}

static void hw_general_timer_pm_get_status(bool *hw_timer_active_status,
                                           uint32_t *next_wake_time_item,
                                           uint32_t *pwm_enable_status, uint8_t timer_id)
{
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(timer_id);
    *hw_timer_active_status = TIM_GetStatus(TIMx);
    *pwm_enable_status = TIMx->ControlReg & BIT3;
    *next_wake_time_item = TIM_GetCurrentValue(TIMx);
}
#endif

#if (HW_TIMER_SUPPORT_ENH_IP == 1)
#include "rtl876x_enh_tim.h"

typedef struct
{
    uint32_t REG_EnhTimerMaxCnt[4];
    uint32_t REG_EnhTimerModeCfg[4];
    uint32_t REG_EnhTimerCurCnt[4];
    uint32_t reg_enh_timer_en_ctrl;
    uint32_t reg_enh_timer_intr_en_ctrl;
} T_ENH_TIM_RESTORE_REG;

RAM_TEXT_SECTION void hw_enh_timer_handler_imp(T_HW_TIMER_ID id)
{
    ENH_TIM_TypeDef *ENH_TIMx = hw_enh_timer_get_reg_base_from_id(id);

    ENH_TIM_ClearINT(ENH_TIMx);

    if (hw_timer_pool.item[id].cback)
    {
        hw_timer_pool.item[id].cback(hw_timer_pool.item[id].context);
    }
}

static void hw_enh_timer_driver_init_int(T_HW_TIMER_ID id, uint32_t period_us, bool reload,
                                         uint32_t callback, bool dma_support)
{
    uint32_t period_min = 1;

    if (callback)
    {
        period_min = HW_TIMER_INTTERRUPT_TIME_MIN;
    }

    if (period_us < period_min)
    {
        /* Workaround for hw not accept 0 */
        period_us = period_min;
    }

    hw_timer_rcc_clock_cmd(id);

    ENH_TIM_TypeDef *ENH_TIMx = hw_enh_timer_get_reg_base_from_id(id);

    ENH_TIM_TimeBaseInitTypeDef enh_time_param;
    ENH_TIM_StructInit(&enh_time_param);
    enh_time_param.TIM_ClockSrcDiv = CLK_DIV_40;
    enh_time_param.TIM_Mode = ENH_TIM_Mode_UserDefine;
    enh_time_param.TIM_OneShotEn = reload ? ENH_TIM_ONE_SHOT_DISABLE : ENH_TIM_ONE_SHOT_ENABLE;
    enh_time_param.TIM_Period = period_us;
    ENH_TIM_TimeBaseInit(ENH_TIMx, &enh_time_param);

    if (callback)
    {
        hw_timer_update_isr(hw_timer_get_vectorn_from_id(id), hw_enh_timer_get_isr(id),
                            FMC_IS_SPIC0_ADDR((uint32_t)callback));

        NVIC_InitTypeDef nvic_param;
        nvic_param.NVIC_IRQChannel = hw_timer_get_irqn_from_id(id);
        nvic_param.NVIC_IRQChannelPriority = 3;
        nvic_param.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&nvic_param);

        ENH_TIM_ClearINT(ENH_TIMx);
        ENH_TIM_INTConfig(ENH_TIMx, ENABLE);
    }
}

RAM_TEXT_SECTION void hw_enh_timer_start(T_HW_TIMER_ID timer_id, bool int_en)
{
    ENH_TIM_TypeDef *ENH_TIMx = hw_enh_timer_get_reg_base_from_id(timer_id);

    if (int_en)
    {
        ENH_TIM_INTConfig(ENH_TIMx, ENABLE);
    }

    ENH_TIM_Cmd(ENH_TIMx, ENABLE);
    if (hw_enh_timer_is_one_shot(ENH_TIMx))
    {
        ENH_TIM_OneShotActivate(ENH_TIMx);
    }
}

RAM_TEXT_SECTION void hw_enh_timer_stop(T_HW_TIMER_ID timer_id, bool int_en)
{
    ENH_TIM_TypeDef *ENH_TIMx = hw_enh_timer_get_reg_base_from_id(timer_id);

    if (int_en)
    {
        ENH_TIM_INTConfig(ENH_TIMx, DISABLE);
        ENH_TIM_ClearINT(ENH_TIMx);
    }

    ENH_TIM_Cmd(ENH_TIMx, DISABLE);
}

RAM_TEXT_SECTION void hw_enh_timer_restart(T_HW_TIMER_ID timer_id, bool int_en, uint32_t period_us)
{
    ENH_TIM_TypeDef *ENH_TIMx = hw_enh_timer_get_reg_base_from_id(timer_id);
    uint32_t period_min = 1;

    if (int_en)
    {
        period_min = HW_TIMER_INTTERRUPT_TIME_MIN;
    }
    if (period_us < period_min)
    {
        /* Workaround for hw not accept 0 */
        period_us = period_min;
    }

    ENH_TIM_Cmd(ENH_TIMx, DISABLE);

    if (int_en)
    {
        ENH_TIM_INTConfig(ENH_TIMx, DISABLE);
        ENH_TIM_ClearINT(ENH_TIMx);
        ENH_TIM_ChangePeriod(ENH_TIMx, period_us);
        ENH_TIM_INTConfig(ENH_TIMx, ENABLE);
    }
    else
    {
        ENH_TIM_ChangePeriod(ENH_TIMx, period_us);
    }

    ENH_TIM_Cmd(ENH_TIMx, ENABLE);
    if (hw_enh_timer_is_one_shot(ENH_TIMx))
    {
        ENH_TIM_OneShotActivate(ENH_TIMx);
    }
}

RAM_TEXT_SECTION void hw_enh_timer_get_current_count(T_HW_TIMER_ID timer_id, uint32_t *count)
{
    ENH_TIM_TypeDef *ENH_TIMx = hw_enh_timer_get_reg_base_from_id(timer_id);

    *count = ENH_TIM_GetCurrentValue(ENH_TIMx);
}

void hw_enh_timer_get_elapsed_time(T_HW_TIMER_ID timer_id, uint32_t *time)
{
    ENH_TIM_TypeDef *ENH_TIMx = hw_enh_timer_get_reg_base_from_id(timer_id + HW_TIMER_ID_8);
    bool state = false;

    if (hw_enh_timer_is_one_shot(ENH_TIMx))
    {
        state = hw_enh_timer_get_one_shot_enable_statue(timer_id);
    }
    else
    {
        state = ENH_TIM_GetEnableStatus(ENH_TIMx);
    }

    if (state)
    {
        *time = ENH_TIMx->REG_ENH_TIMER_x_MAX_CNT - ENH_TIM_GetCurrentValue(ENH_TIMx);
    }
    else
    {
        *time = 0;
    }
}

void hw_enh_timer_get_period(T_HW_TIMER_ID timer_id, uint32_t *period_us)
{
    ENH_TIM_TypeDef *ENH_TIMx = hw_enh_timer_get_reg_base_from_id(timer_id);

    *period_us = ENH_TIMx->REG_ENH_TIMER_x_MAX_CNT;
}

void hw_enh_timer_print_debug_info(T_HW_TIMER_ID timer_id)
{
    ENH_TIM_TypeDef *ENH_TIMx = hw_enh_timer_get_reg_base_from_id(timer_id);
    IO_PRINT_TRACE7("Debug hw timer id %d REG_ENH_TIMER_x_MAX_CNT 0x%x, REG_ENH_TIMER_x_CUR_CNT 0x%x, REG_ENH_TIMER_x_MODE_CFG 0x%x, REG_ENH_TIMER_EN_CTRL 0x%x, REG_ENH_TIMER_INTR_EN_CTRL 0x%x, REG_ENH_TIMER_INTR 0x%x",
                    timer_id, ENH_TIMx->REG_ENH_TIMER_x_MAX_CNT, ENH_TIMx->REG_ENH_TIMER_x_CUR_CNT,
                    ENH_TIMx->REG_ENH_TIMER_x_MODE_CFG, ENH_TIM_CTRL->REG_ENH_TIMER_EN_CTRL,
                    ENH_TIM_CTRL->REG_ENH_TIMER_INTR_EN_CTRL,
                    ENH_TIM_CTRL->REG_ENH_TIMER_INTR);
}

void hw_enh_timer_is_active(T_HW_TIMER_ID timer_id, bool *state)
{
    ENH_TIM_TypeDef *ENH_TIMx = hw_enh_timer_get_reg_base_from_id(timer_id + HW_TIMER_ID_8);

    if (hw_enh_timer_is_one_shot(ENH_TIMx))
    {
        *state = hw_enh_timer_get_one_shot_enable_statue(timer_id);
    }
    else
    {
        *state = ENH_TIM_GetEnableStatus(ENH_TIMx);
    }
}

void hw_enh_timer_set_auto_reload(T_HW_TIMER_ID timer_id, bool is_auto_reload)
{
    ENH_TIM_TypeDef *ENH_TIMx = hw_enh_timer_get_reg_base_from_id(timer_id);
    if (is_auto_reload)
    {
        ENH_TIMx->REG_ENH_TIMER_x_MODE_CFG &= ~ BIT8;
    }
    else
    {
        ENH_TIMx->REG_ENH_TIMER_x_MODE_CFG |= BIT8;
    }
}

void hw_enh_timer_update_period(T_HW_TIMER_ID timer_id, bool int_en, uint32_t one_shot_period_us,
                                uint32_t period_us)
{
    ENH_TIM_TypeDef *ENH_TIMx = hw_enh_timer_get_reg_base_from_id(timer_id);
    uint32_t period_min = 1;

    if (int_en)
    {
        period_min = HW_TIMER_INTTERRUPT_TIME_MIN;
    }
    if (one_shot_period_us < period_min)
    {
        /* Workaround for hw not accept 0 */
        one_shot_period_us = period_min;
    }

    if (int_en)
    {
        ENH_TIM_INTConfig(ENH_TIMx, DISABLE);
        ENH_TIM_Cmd(ENH_TIMx, DISABLE);
        ENH_TIM_ChangePeriod(ENH_TIMx, one_shot_period_us);
        ENH_TIM_Cmd(ENH_TIMx, ENABLE);
        while (ENH_TIMx->REG_ENH_TIMER_x_CUR_CNT > one_shot_period_us);
        ENH_TIM_ChangePeriod(ENH_TIMx, period_us);
        ENH_TIM_INTConfig(ENH_TIMx, ENABLE);
    }
    else
    {
        ENH_TIM_Cmd(ENH_TIMx, DISABLE);
        ENH_TIM_ChangePeriod(ENH_TIMx, one_shot_period_us);
        ENH_TIM_Cmd(ENH_TIMx, ENABLE);
        while (ENH_TIMx->REG_ENH_TIMER_x_CUR_CNT > one_shot_period_us);
        ENH_TIM_ChangePeriod(ENH_TIMx, period_us);
    }
}

static void hw_enh_timer_restore_compensate(T_HW_TIMER_ID id, T_ENH_TIM_RESTORE_REG *buf,
                                            uint32_t sleep_time_us)
{
    ENH_TIM_TypeDef *ENH_TIMx = hw_enh_timer_get_reg_base_from_id(id + HW_TIMER_ID_8);
    uint32_t load_count_setting = buf->REG_EnhTimerCurCnt[id] > sleep_time_us ?
                                  (buf->REG_EnhTimerCurCnt[id] - sleep_time_us) : 5;
    ENH_TIMx->REG_ENH_TIMER_x_MODE_CFG = buf->REG_EnhTimerModeCfg[id];
    ENH_TIMx->REG_ENH_TIMER_x_MAX_CNT = load_count_setting;
    ENH_TIM_CTRL->REG_ENH_TIMER_INTR_EN_CTRL |= (buf->reg_enh_timer_intr_en_ctrl & BIT(id));
    ENH_TIM_CTRL->REG_ENH_TIMER_EN_CTRL |= BIT(id);
    if (hw_enh_timer_is_one_shot(ENH_TIMx))
    {
        ENH_TIM_CTRL->REG_ENH_TIMER_ONESHOT_GO_CTRL |= BIT(id);
    }
    while (ENH_TIMx->REG_ENH_TIMER_x_CUR_CNT > load_count_setting);
    ENH_TIMx->REG_ENH_TIMER_x_MAX_CNT = buf->REG_EnhTimerMaxCnt[id];
}

static void hw_enh_timer_restore_direct(T_HW_TIMER_ID id, T_ENH_TIM_RESTORE_REG *buf)
{
    ENH_TIM_TypeDef *ENH_TIMx = hw_enh_timer_get_reg_base_from_id(id + HW_TIMER_ID_8);
    ENH_TIMx->REG_ENH_TIMER_x_MODE_CFG = buf->REG_EnhTimerModeCfg[id];
    ENH_TIMx->REG_ENH_TIMER_x_MAX_CNT = buf->REG_EnhTimerMaxCnt[id];
    ENH_TIM_CTRL->REG_ENH_TIMER_INTR_EN_CTRL |= (buf->reg_enh_timer_intr_en_ctrl & BIT(id));
    ENH_TIM_CTRL->REG_ENH_TIMER_EN_CTRL |= (buf->reg_enh_timer_en_ctrl & BIT(id));
}

void hw_enh_timer_pm_exit(T_HW_TIMER_ID id, T_ENH_TIM_RESTORE_REG *buf, uint32_t sleep_time_us)
{
    uint16_t used_mask = (~g_hw_tim_mask) & HW_ENH_TIMER_DYNAMIC_MASK;
    if (!(used_mask & BIT(id + HW_TIMER_ID_8)))
    {
        return;
    }

    if ((hw_timer_pool.item[id + HW_TIMER_ID_8].block_dlps_when_active == 0) &&
        (buf->reg_enh_timer_en_ctrl & BIT(id)))
    {
        hw_enh_timer_restore_compensate(id, buf, sleep_time_us);
    }
    else
    {
        hw_enh_timer_restore_direct(id, buf);
    }
}

static void hw_enh_timer_pm_get_status(bool *hw_timer_active_status, uint32_t *next_wake_time_item,
                                       uint32_t *pwm_enable_status, uint8_t timer_id)
{
    ENH_TIM_TypeDef *ENH_TIMx = hw_enh_timer_get_reg_base_from_id(timer_id + HW_TIMER_ID_8);
    if (hw_enh_timer_is_one_shot(ENH_TIMx))
    {
        *hw_timer_active_status = hw_enh_timer_get_one_shot_enable_statue(timer_id);
    }
    else
    {
        *hw_timer_active_status = ENH_TIM_GetEnableStatus(ENH_TIMx);
    }
    *pwm_enable_status = 0;
    *next_wake_time_item = ENH_TIM_GetCurrentValue(ENH_TIMx);
}
#endif

#if (HW_TIMER_SUPPORT_RTK_IP == 1)
typedef struct
{
    uint32_t REG_TIMER_CUR_CNT[7];
    uint32_t REG_TIMER_MODE_CFG[7];
    uint32_t REG_TIMER_MAX_CNT[7];
    uint32_t REG_TIMER_CCR[7];
    uint32_t REG_TIMER_CCR_FIFO[7];
    uint32_t REG_TIMER_PWM_CFG[7];
    uint32_t REG_TIMER_PWM_SHIFT_CNT[7];
    uint32_t REG_TIMER_DMA_CFG[7];
    uint32_t REG_TIMER_EN_CTRL;
    uint32_t REG_TIMER_INTR_EN_CTRL;
    uint32_t REG_TIMER_PAUSE_INTR_EN_CTRL;
} T_RTK_TIM_RESTORE_REG;

RAM_TEXT_SECTION void hw_rtk_timer_handler_imp(T_HW_TIMER_ID id)
{
    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(id);

    TIM_ClearInterrupt(TIMx, TIM_INT_TIMEOUT);

    if (hw_timer_pool.item[id].cback)
    {
        hw_timer_pool.item[id].cback(hw_timer_pool.item[id].context);
    }
}

static void hw_rtk_timer_driver_init_int(T_HW_TIMER_ID id, uint32_t period_us, bool reload,
                                         uint32_t callback, bool dma_support)
{
    uint32_t period_min = 1;

    if (callback)
    {
        period_min = HW_TIMER_INTTERRUPT_TIME_MIN;
    }

    if (period_us < period_min)
    {
        /* Workaround for hw not accept 0 */
        period_us = period_min;
    }

    hw_timer_rcc_clock_cmd(id);

    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(id);

    TIM_TimeBaseInitTypeDef time_param;
    TIM_StructInit(&time_param);

    time_param.TIM_Mode = TIM_Mode_UserDefine;
    time_param.TIM_ClockSrc = CK_40M_TIMER;
    time_param.TIM_ClockDiv = TIM_CLOCK_DIVIDER_1;
    time_param.TIM_Period = period_us * 40;
    time_param.TIM_OneShotEn = reload ? DISABLE : ENABLE;
    time_param.PWM_En = dma_support ? ENABLE : DISABLE;
    time_param.PWM_HighCount = dma_support ? (period_us * 20) : 0;
    TIM_TimeBaseInit(TIMx, &time_param);

    if (callback)
    {
        hw_timer_update_isr(hw_timer_get_vectorn_from_id(id), FMC_IS_SPIC0_ADDR((uint32_t)callback));
        NVIC_InitTypeDef nvic_param;

        nvic_param.NVIC_IRQChannel = hw_timer_get_irqn_from_id(id);
        nvic_param.NVIC_IRQChannelPriority = 3;
        nvic_param.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&nvic_param);

        TIM_ClearInterrupt(TIMx, TIM_INT_TIMEOUT);
        TIM_InterruptConfig(TIMx, TIM_INT_TIMEOUT, ENABLE);
    }
}

RAM_TEXT_SECTION void hw_rtk_timer_start(T_HW_TIMER_ID timer_id, bool int_en)
{
    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(timer_id);

    if (int_en)
    {
        TIM_InterruptConfig(TIMx, TIM_INT_TIMEOUT, ENABLE);
    }

    TIM_Cmd(TIMx, ENABLE);
    if (hw_rtk_timer_is_one_shot(TIMx))
    {
        TIM_OneShotEnable(TIMx);
    }
}

RAM_TEXT_SECTION void hw_rtk_timer_stop(T_HW_TIMER_ID timer_id, bool int_en)
{
    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(timer_id);

    if (int_en)
    {
        TIM_InterruptConfig(TIMx, TIM_INT_TIMEOUT, DISABLE);
        TIM_ClearInterrupt(TIMx, TIM_INT_TIMEOUT);
    }

    TIM_Cmd(TIMx, DISABLE);
}

RAM_TEXT_SECTION void hw_rtk_timer_restart(T_HW_TIMER_ID timer_id, bool int_en, uint32_t period_us)
{
    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(timer_id);
    uint32_t period_min = 1;

    if (int_en)
    {
        period_min = HW_TIMER_INTTERRUPT_TIME_MIN;
    }
    if (period_us < period_min)
    {
        /* Workaround for hw not accept 0 */
        period_us = period_min;
    }

    if (int_en)
    {
        TIM_Cmd(TIMx, DISABLE);
        TIM_InterruptConfig(TIMx, TIM_INT_TIMEOUT, DISABLE);
        TIM_ClearInterrupt(TIMx, TIM_INT_TIMEOUT);
        TIM_ChangePeriod(TIMx, period_us * 40);
        TIM_InterruptConfig(TIMx, TIM_INT_TIMEOUT, ENABLE);
    }
    else
    {
        TIM_Cmd(TIMx, DISABLE);
        TIM_ChangePeriod(TIMx, period_us * 40);
    }

    TIM_Cmd(TIMx, ENABLE);
    if (hw_rtk_timer_is_one_shot(TIMx))
    {
        TIM_OneShotEnable(TIMx);
    }
}

RAM_TEXT_SECTION void hw_rtk_timer_get_current_count(T_HW_TIMER_ID timer_id, uint32_t *count)
{
    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(timer_id);
    *count = TIM_GetCurrentValue(TIMx) / 40;
}

void hw_rtk_timer_get_elapsed_time(T_HW_TIMER_ID timer_id, uint32_t *time)
{
    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(timer_id);
    bool state = false;

    if (hw_rtk_timer_is_one_shot(TIMx))
    {
        state = TIM_GetOneshotStatus(TIMx);
    }
    else
    {
        state = TIM_GetOperationStatus(TIMx);
    }

    if (state)
    {
        *time = TIM_GetElapsedValue(TIMx) / 40;
    }
    else
    {
        *time = 0;
    }
}

void hw_rtk_timer_get_period(T_HW_TIMER_ID timer_id, uint32_t *period_us)
{
    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(timer_id);

    *period_us = TIM_GetPeriod(TIMx) / 40;
}

void hw_rtk_timer_is_active(T_HW_TIMER_ID timer_id, bool *state)
{
    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(timer_id);

    if (hw_rtk_timer_is_one_shot(TIMx))
    {
        *state = TIM_GetOneshotStatus(TIMx);
    }
    else
    {
        *state = TIM_GetOperationStatus(TIMx);
    }
}

void hw_rtk_timer_print_debug_info(T_HW_TIMER_ID timer_id)
{
    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(timer_id);
    IO_PRINT_TRACE7("Debug hw timer id %d TIMER_MAX_CNT 0x%x, TIMER_CUR_CNT 0x%x, TIMER_MODE_CFG 0x%x, TIMER_EN_CTRL 0x%x, TIMER_INTR_EN_CTRL 0x%x, TIMER_INTR 0x%x",
                    timer_id, TIMx->TIMER_MAX_CNT, TIMx->TIMER_CUR_CNT, TIMx->TIMER_MODE_CFG,
                    TIMER1_SHARE->TIMER_EN_CTRL, TIMER1_SHARE->TIMER_INTR_EN_CTRL, TIMER1_SHARE->TIMER_INTR);
}

void hw_rtk_timer_set_auto_reload(T_HW_TIMER_ID timer_id, bool is_auto_reload)
{
    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(timer_id);
    if (is_auto_reload)
    {
        TIMx->TIMER_MODE_CFG &= ~BIT8;
    }
    else
    {
        TIMx->TIMER_MODE_CFG |= BIT8;
    }
}

void hw_rtk_timer_update_period(T_HW_TIMER_ID timer_id, bool int_en, uint32_t one_shot_period_us,
                                uint32_t period_us)
{
    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(timer_id);
    uint32_t period_min = 1;
    if (int_en)
    {
        period_min = HW_TIMER_INTTERRUPT_TIME_MIN;
    }
    if (one_shot_period_us < period_min)
    {
        /* Workaround for hw not accept 0 */
        one_shot_period_us = period_min;
    }

    one_shot_period_us *= 40;
    period_us *= 40;

    if (int_en)
    {
        TIM_InterruptConfig(TIMx, TIM_INT_TIMEOUT, DISABLE);
        TIM_Cmd(TIMx, DISABLE);
        TIM_ChangePeriod(TIMx, one_shot_period_us);
        TIM_Cmd(TIMx, ENABLE);
        while (TIMx->TIMER_CUR_CNT > one_shot_period_us);
        TIM_ChangePeriod(TIMx, period_us);
        TIM_InterruptConfig(TIMx, TIM_INT_TIMEOUT, ENABLE);
    }
    else
    {
        TIM_Cmd(TIMx, DISABLE);
        TIM_ChangePeriod(TIMx, one_shot_period_us);
        TIM_Cmd(TIMx, ENABLE);
        while (TIMx->TIMER_CUR_CNT > one_shot_period_us);
        TIM_ChangePeriod(TIMx, period_us);
    }
}

static void hw_rtk_timer_restore_compensate(T_HW_TIMER_ID id, T_RTK_TIM_RESTORE_REG *buf,
                                            uint32_t sleep_time_us)
{
    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(id);
    T_HW_TIMER_ID timer_id_offet = id - HW_TIMER_ID_2;
    uint32_t load_count_setting = buf->REG_TIMER_CUR_CNT[timer_id_offet] > (sleep_time_us * 40) ?
                                  (buf->REG_TIMER_CUR_CNT[timer_id_offet] - (sleep_time_us * 40)) : 5;
    TIMx->TIMER_MODE_CFG = buf->REG_TIMER_MODE_CFG[timer_id_offet];
    TIMx->TIMER_MAX_CNT = load_count_setting;
    if (id >= HW_TIMER_ID_4)
    {
        TIMx->TIMER_PWM_CFG = buf->REG_TIMER_PWM_CFG[timer_id_offet];
        TIMx->TIMER_CCR = buf->REG_TIMER_CCR[timer_id_offet];
    }
    if (id >= HW_TIMER_ID_6)
    {
        TIMx->TIMER_CCR_FIFO = buf->REG_TIMER_CCR_FIFO[timer_id_offet];
        TIMx->TIMER_PWM_SHIFT_CNT = buf->REG_TIMER_PWM_SHIFT_CNT[timer_id_offet];
        TIMx->TIMER_DMA_CFG = buf->REG_TIMER_DMA_CFG[timer_id_offet];
    }

    TIMER1_SHARE->TIMER_PAUSE_INTR_EN_CTRL |= (buf->REG_TIMER_PAUSE_INTR_EN_CTRL & BIT(id));
    TIMER1_SHARE->TIMER_INTR_EN_CTRL |= (buf->REG_TIMER_INTR_EN_CTRL & BIT(id));
    TIMER1_SHARE->TIMER_EN_CTRL |= (buf->REG_TIMER_EN_CTRL & BIT(id));
    if (hw_rtk_timer_is_one_shot(TIMx))
    {
        TIMER1_SHARE->TIMER_ONESHOT_GO_CTRL |= BIT(id);
    }
    while (TIMx->TIMER_CUR_CNT > load_count_setting);
    TIMx->TIMER_MAX_CNT = buf->REG_TIMER_MAX_CNT[timer_id_offet];
}

static void hw_rtk_timer_restore_direct(T_HW_TIMER_ID id, T_RTK_TIM_RESTORE_REG *buf)
{
    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(id);
    T_HW_TIMER_ID timer_id_offet = id - HW_TIMER_ID_2;
    TIMx->TIMER_MODE_CFG = buf->REG_TIMER_MODE_CFG[timer_id_offet];
    TIMx->TIMER_MAX_CNT = buf->REG_TIMER_MAX_CNT[timer_id_offet];
    if (id >= HW_TIMER_ID_4)
    {
        TIMx->TIMER_PWM_CFG = buf->REG_TIMER_PWM_CFG[timer_id_offet];
        TIMx->TIMER_CCR = buf->REG_TIMER_CCR[timer_id_offet];
    }
    if (id >= HW_TIMER_ID_6)
    {
        TIMx->TIMER_CCR_FIFO = buf->REG_TIMER_CCR_FIFO[timer_id_offet];
        TIMx->TIMER_PWM_SHIFT_CNT = buf->REG_TIMER_PWM_SHIFT_CNT[timer_id_offet];
        TIMx->TIMER_DMA_CFG = buf->REG_TIMER_DMA_CFG[timer_id_offet];
    }

    TIMER1_SHARE->TIMER_PAUSE_INTR_EN_CTRL |= (buf->REG_TIMER_PAUSE_INTR_EN_CTRL & BIT(id));
    TIMER1_SHARE->TIMER_INTR_EN_CTRL |= (buf->REG_TIMER_INTR_EN_CTRL & BIT(id));
    TIMER1_SHARE->TIMER_EN_CTRL |= (buf->REG_TIMER_EN_CTRL & BIT(id));
}

void hw_rtk_timer_pm_exit(T_HW_TIMER_ID id, T_RTK_TIM_RESTORE_REG *buf, uint32_t sleep_time_us)
{
    uint16_t used_mask = (~g_hw_tim_mask) & HW_RTK_TIMER_DYNAMIC_MASK;
    if (!(used_mask & BIT(id)))
    {
        return;
    }

    //if block DLPS or PWM enable or Timer disable then just restore the registers
    if ((hw_timer_pool.item[id].block_dlps_when_active == 0) &&
        ((buf->REG_TIMER_PWM_CFG[id] & BIT0) == 0) && \
        (buf->REG_TIMER_EN_CTRL & BIT(id)))
    {
        hw_rtk_timer_restore_compensate(id, buf, sleep_time_us);
    }
    else
    {
        hw_rtk_timer_restore_direct(id, buf);
    }
}

static void hw_rtk_timer_pm_get_status(bool *hw_timer_active_status, uint32_t *next_wake_time_item,
                                       uint32_t *pwm_enable_status, uint8_t timer_id)
{
    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(timer_id);
    if (hw_rtk_timer_is_one_shot(TIMx))
    {
        *hw_timer_active_status = TIM_GetOneshotStatus(TIMx);
    }
    else
    {
        *hw_timer_active_status = TIM_GetOperationStatus(TIMx);
    }
    *pwm_enable_status = TIMx->TIMER_PWM_CFG & BIT0;
    *next_wake_time_item = TIM_GetCurrentValue(TIMx) / 40;
}
#endif

T_HW_TIMER_HANDLE hw_timer_create(const char *name, uint32_t period_us, bool reload,
                                  P_HW_TIMER_CBACK cback)
{
    T_HW_TIMER *p_hw_timer;
    T_HW_TIMER_FEATURE p_feature = {.timer_feature = 0};

    p_hw_timer = hw_timer_pool_alloc(p_feature);

    if (p_hw_timer != NULL)
    {
        p_hw_timer->user_name = name;
        p_hw_timer->cback = cback;
        p_hw_timer->context = p_hw_timer;
        p_hw_timer->block_dlps_when_active = false;

        p_timer_driver_init((T_HW_TIMER_ID)(p_hw_timer->timer_id), period_us, reload,
                            (uint32_t)cback, p_feature.support_dma);
    }

    return (T_HW_TIMER_HANDLE)p_hw_timer;
}

T_HW_TIMER_HANDLE hw_timer_create_dma_mode(const char *name, uint32_t period_us, bool reload,
                                           P_HW_TIMER_CBACK cback)
{
    T_HW_TIMER *p_hw_timer;
    T_HW_TIMER_FEATURE p_feature = {.timer_feature = 0};
    p_feature.support_dma = 1;

    p_hw_timer = hw_timer_pool_alloc(p_feature);

    if (p_hw_timer != NULL)
    {
        p_hw_timer->user_name = name;
        p_hw_timer->cback = cback;
        p_hw_timer->context = p_hw_timer;
        p_hw_timer->block_dlps_when_active = false;

        p_timer_driver_init((T_HW_TIMER_ID)(p_hw_timer->timer_id), period_us, reload,
                            (uint32_t)cback, p_feature.support_dma);
    }

    return (T_HW_TIMER_HANDLE)p_hw_timer;
}

bool hw_timer_delete(T_HW_TIMER_HANDLE handle)
{
    bool ret = false;

    if (handle != NULL)
    {
        hw_timer_stop(handle);
        ret = hw_timer_pool_free(handle);
    }

    return ret;
}

uint8_t hw_timer_get_dma_handshake(T_HW_TIMER_HANDLE handle)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        return p_hw_timer->timer_id + HW_TIMER_DMA_HANDSHAKE;
    }

    return 0;
}

const char *hw_timer_get_name(T_HW_TIMER_HANDLE handle)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        return p_hw_timer->user_name;
    }

    return NULL;
}

T_HW_TIMER_ID hw_timer_get_id(T_HW_TIMER_HANDLE handle)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        return (T_HW_TIMER_ID)(p_hw_timer->timer_id);
    }

    return HW_TIMER_ID_EMPTY;
}

RAM_TEXT_SECTION bool hw_timer_start(T_HW_TIMER_HANDLE handle)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        bool int_en = (p_hw_timer->cback) ? true : false;
        T_HW_TIMER_ID timer_id = (T_HW_TIMER_ID)(p_hw_timer->timer_id);

        p_hw_timer_start(timer_id, int_en);

        return true;
    }

    return false;
}

RAM_TEXT_SECTION bool hw_timer_stop(T_HW_TIMER_HANDLE handle)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        bool int_en = (p_hw_timer->cback) ? true : false;
        T_HW_TIMER_ID timer_id = (T_HW_TIMER_ID)(p_hw_timer->timer_id);

        p_hw_timer_stop(timer_id, int_en);

        return true;
    }

    return false;
}

RAM_TEXT_SECTION bool hw_timer_restart(T_HW_TIMER_HANDLE handle, uint32_t period_us)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        bool int_en = (p_hw_timer->cback) ? true : false;
        T_HW_TIMER_ID timer_id = (T_HW_TIMER_ID)(p_hw_timer->timer_id);

        p_hw_timer_restart(timer_id, int_en, period_us);

        return true;
    }

    return false;
}

RAM_TEXT_SECTION bool hw_timer_get_current_count(T_HW_TIMER_HANDLE handle,
                                                 uint32_t *count)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        T_HW_TIMER_ID timer_id = (T_HW_TIMER_ID)(p_hw_timer->timer_id);


        p_hw_timer_get_current_count(timer_id, count);
        return true;
    }

    *count = 0;
    return false;
}

bool hw_timer_set_auto_reload(T_HW_TIMER_HANDLE handle, bool is_auto_reload)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;
    if (p_hw_timer != NULL)
    {
        T_HW_TIMER_ID timer_id = (T_HW_TIMER_ID)(p_hw_timer->timer_id);

        p_hw_timer_set_auto_reload(timer_id, is_auto_reload);
        return true;
    }
    return false;
}

bool hw_timer_get_elapsed_time(T_HW_TIMER_HANDLE handle, uint32_t *time)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        T_HW_TIMER_ID timer_id = (T_HW_TIMER_ID)(p_hw_timer->timer_id);

        p_hw_timer_get_elapsed_time(timer_id, time);
        return true;
    }

    *time = 0;
    return false;
}

bool hw_timer_get_period(T_HW_TIMER_HANDLE handle, uint32_t *period_us)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        T_HW_TIMER_ID timer_id = (T_HW_TIMER_ID)(p_hw_timer->timer_id);
        p_hw_timer_get_period(timer_id, period_us);
        return true;
    }

    *period_us = 0;
    return false;
}

bool hw_timer_is_active(T_HW_TIMER_HANDLE handle, bool *state)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        T_HW_TIMER_ID timer_id = (T_HW_TIMER_ID)(p_hw_timer->timer_id);

        p_hw_timer_is_active(timer_id, state);

        return true;
    }

    *state = false;
    return false;
}

bool hw_timer_update_period(T_HW_TIMER_HANDLE handle, uint32_t one_shot_period_us,
                            uint32_t period_us)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        bool int_en = (p_hw_timer->cback) ? true : false;
        T_HW_TIMER_ID timer_id = (T_HW_TIMER_ID)(p_hw_timer->timer_id);

        p_hw_timer_update_period(timer_id, int_en, one_shot_period_us, period_us);

        return true;
    }

    return false;
}

uint8_t hw_timer_get_free_timer_number(void)
{
    return count_1bits(g_hw_tim_mask & HW_TIMER_DYNAMIC_MASK);
}

void hw_timer_print_all_users(void)
{
    uint8_t i = 0;
    uint16_t used_mask = (~g_hw_tim_mask) & HW_TIMER_DYNAMIC_MASK;

    for (i = 0; i < HW_TIMER_TOTAL_NUM; i++)
    {
        if (used_mask & BIT(i))
        {
            IO_PRINT_INFO2("dump hw timer user id %d: %s",
                           i, TRACE_STRING(hw_timer_pool.item[i].user_name));
        }
    }
}

void hw_timer_print_debug_info(T_HW_TIMER_HANDLE handle)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;
    T_HW_TIMER_ID timer_id = (T_HW_TIMER_ID)(p_hw_timer->timer_id);

    p_hw_timer_print_debug_info(timer_id);
}

bool hw_timer_lpm_set(T_HW_TIMER_HANDLE handle, bool enable)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;
    bool ret = false;

    if (p_hw_timer)
    {
        p_hw_timer->block_dlps_when_active = !enable;
        ret = true;
    }

    return ret;
}

static bool hw_timer_pm_check(uint32_t *p_next_wake_up_time)
{
    uint16_t used_mask = (~g_hw_tim_mask) & HW_TIMER_DYNAMIC_MASK;
    bool ret = true;
    uint32_t next_wake_time = 0xFFFFFFFF;

    char *hw_timer_pm_check_fail_reason = "null";
    static uint16_t hw_timer_print_count = 0;
    uint8_t i;
    bool hw_timer_active_status = false;
    uint32_t next_wake_time_item = 0;
    uint32_t pwm_enable_status = 0;

    for (i = 0; i < HW_TIMER_TOTAL_NUM; i++)
    {
        if (used_mask & BIT(i))
        {
            p_hw_timer_pm_get_status(&hw_timer_active_status, &next_wake_time_item,
                                     &pwm_enable_status, i);

            if (hw_timer_active_status)
            {
                if (pwm_enable_status)
                {
                    hw_timer_pm_check_fail_reason = "pwm";
                    ret = false;
                    break;
                }
                else if (hw_timer_pool.item[i].block_dlps_when_active)
                {
                    hw_timer_pm_check_fail_reason = "lpm_false_timer";
                    ret = false;
                    break;
                }
                else
                {
                    if (next_wake_time_item < HW_TIMER_ALLOW_ENTER_LPM_THRESHOLD_US)
                    {
                        hw_timer_pm_check_fail_reason = "next_time";
                        ret = false;
                        break;
                    }
                    else if (next_wake_time > next_wake_time_item)
                    {
                        next_wake_time = next_wake_time_item;
                    }
                }
            }
        }
    }

    if (ret == true)
    {
        if (next_wake_time != 0xFFFFFFFF)
        {
            *p_next_wake_up_time = US_TO_PF_RTC_TICK(next_wake_time);
            hw_timer_print_count = 0;
        }
    }
    else
    {
        hw_timer_print_count++;
        if (hw_timer_print_count > HW_TIMER_PM_PRINT_INTERVAL)
        {
            hw_timer_print_count = 0;
            if (hw_timer_pool.item[i].user_name == NULL)
            {
                hw_timer_pool.item[i].user_name = "null";
            }
            IO_PRINT_ERROR3("hw_timer_pm_check fail, id %u, name %s, reason %s", \
                            i, TRACE_STRING(hw_timer_pool.item[i].user_name), TRACE_STRING(hw_timer_pm_check_fail_reason));
        }
    }

    return ret;
}
