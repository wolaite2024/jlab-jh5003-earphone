#include <stdio.h>
#include "rtl876x_tim.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_nvic.h"
#include "trace.h"
#include "hw_tim.h"
#include "pwm.h"
#include "section.h"
#include "fmc_platform.h"
#include "vector_table.h"

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
#define HW_TIMER_SUPPORT_GENERAL_IP                                                     (1)
#define hw_timer_get_pwm_func(id)                                                       ((id < 8) ? (timer_pwm0 + id) : ((id < 12) ? (PWM8 + (id - 8)) : (PWM12 + (id - 12))))
#define HW_TIMER_PWM_DEADZONE_MASK                                                      (BIT2 | BIT3)
#define hw_timer_get_pwm_func_p(id)                                                     ((id == 2) ? (PWM2_P) : (PWM2_P + 2))
#define hw_timer_get_pwm_func_n(id)                                                     ((id == 2) ? (PWM2_N) : (PWM2_N + 2))
#define hw_timer_id_valid(id)                                                           ((id < HW_TIMER_ID_MAX) && (id > HW_TIMER_ID_1))
#define hw_general_timer_get_reg_base_from_id(id)                                       (TIM_TypeDef *)((id < 8) ? (TIM0_REG_BASE + 0x14 * id) : (TIM8_REG_BASE + 0x14 * (id - 8)))
#define p_hw_timer_driver_init_pwm_int(id, period_us_high, period_us_low, deadzone)     hw_general_timer_driver_init_pwm_int(id, period_us_high, period_us_low, deadzone)
#define p_pwm_register_timeout_callback(id, cback)                                      hw_general_timer_pwm_register_timeout_callback(id, cback)
#define p_pwm_config(id, config_struct)                                                 hw_general_timer_pwm_config(id, config_struct)
#define p_pwm_change_duty_and_frequency(id, high_count, low_count)                      hw_general_timer_pwm_change_duty_and_frequency(id, high_count, low_count)
extern IRQ_Fun hw_timer_get_isr(T_HW_TIMER_ID id);
extern bool hw_timer_update_isr(VECTORn_Type v_num, IRQ_Fun isr_handler, bool is_xip);
#elif defined(IC_TYPE_RTL87x3EU)
#define HW_TIMER_SUPPORT_RTK_IP                                                         (1)
#define hw_timer_get_pwm_func(id)                                                       (PWM4 + id - 4)
#define HW_TIMER_PWM_DEADZONE_MASK                                                      (BIT6 | BIT7 | BIT8)
#define hw_timer_get_pwm_func_p(id)                                                     (PWM6_P + id - 6)
#define hw_timer_get_pwm_func_n(id)                                                     (PWM6_N + id - 6)
#define hw_timer_id_valid(id)                                                           ((id < HW_TIMER_ID_MAX) && (id > HW_TIMER_ID_3))
#define hw_rtk_timer_get_reg_base_from_id(id)                                           (TIM_TypeDef *)(TIMER1_BASE + 0x50 * id)
#define p_hw_timer_driver_init_pwm_int(id, period_us_high, period_us_low, deadzone)     hw_rtk_timer_driver_init_pwm_int(id, period_us_high, period_us_low, deadzone)
#define p_pwm_register_timeout_callback(id, cback)                                      hw_rtk_timer_pwm_register_timeout_callback(id, cback)
#define p_pwm_config(id, config_struct)                                                 hw_rtk_timer_pwm_config(id, config_struct)
#define p_pwm_change_duty_and_frequency(id, high_count, low_count)                      hw_rtk_timer_pwm_change_duty_and_frequency(id, high_count, low_count)
extern bool hw_timer_update_isr(VECTORn_Type v_num, bool is_xip);
#elif defined(IC_TYPE_RTL87x3EP)
#define HW_TIMER_SUPPORT_GENERAL_IP                                                     (1)
#define hw_timer_get_pwm_func(id)                                                       (TIMER_PWM0 + id)
#define HW_TIMER_PWM_DEADZONE_MASK                                                      (BIT2 | BIT3)
#define hw_timer_get_pwm_func_p(id)                                                     ((id == 2) ? (PWM2_P) : (PWM2_P + 2))
#define hw_timer_get_pwm_func_n(id)                                                     ((id == 2) ? (PWM2_N) : (PWM2_N + 2))
#define hw_timer_id_valid(id)                                                           ((id < HW_TIMER_ID_8) && (id > HW_TIMER_ID_1))
#define hw_general_timer_get_reg_base_from_id(id)                                       (TIM_TypeDef *)(TIM0_REG_BASE + 0x14 * id)
#define p_hw_timer_driver_init_pwm_int(id, period_us_high, period_us_low, deadzone)     hw_general_timer_driver_init_pwm_int(id, period_us_high, period_us_low, deadzone)
#define p_pwm_register_timeout_callback(id, cback)                                      hw_general_timer_pwm_register_timeout_callback(id, cback)
#define p_pwm_config(id, config_struct)                                                 hw_general_timer_pwm_config(id, config_struct)
#define p_pwm_change_duty_and_frequency(id, high_count, low_count)                      hw_general_timer_pwm_change_duty_and_frequency(id, high_count, low_count)
extern void hal_timer_handler(void);
#define hw_timer_get_isr(id)  hal_timer_handler
extern bool hw_timer_update_isr(VECTORn_Type v_num, IRQ_Fun isr_handler, bool is_xip);
#elif defined(IC_TYPE_RTL87x3E)
#define HW_TIMER_SUPPORT_GENERAL_IP                                                     (1)
#define hw_timer_get_pwm_func(id)                                                       (TIMER_PWM0 + id)
#define HW_TIMER_PWM_DEADZONE_MASK                                                      (BIT2 | BIT3)
#define hw_timer_get_pwm_func_p(id)                                                     ((id == 2) ? (PWM2_P) : (PWM2_P + 2))
#define hw_timer_get_pwm_func_n(id)                                                     ((id == 2) ? (PWM2_N) : (PWM2_N + 2))
#define hw_timer_id_valid(id)                                                           ((id < HW_TIMER_ID_MAX) && (id > HW_TIMER_ID_1))
#define hw_general_timer_get_reg_base_from_id(id)                                       (TIM_TypeDef *)(TIM0_REG_BASE + 0x14 * id)
#define p_hw_timer_driver_init_pwm_int(id, period_us_high, period_us_low, deadzone)     hw_general_timer_driver_init_pwm_int(id, period_us_high, period_us_low, deadzone)
#define p_pwm_register_timeout_callback(id, cback)                                      hw_general_timer_pwm_register_timeout_callback(id, cback)
#define p_pwm_config(id, config_struct)                                                 hw_general_timer_pwm_config(id, config_struct)
#define p_pwm_change_duty_and_frequency(id, high_count, low_count)                      hw_general_timer_pwm_change_duty_and_frequency(id, high_count, low_count)
extern void hal_timer_handler(void);
#define hw_timer_get_isr(id)  hal_timer_handler
extern bool hw_timer_update_isr(VECTORn_Type v_num, IRQ_Fun isr_handler, bool is_xip);
#endif

#define hw_timer_support_deadzone(id)               (BIT(id) & HW_TIMER_PWM_DEADZONE_MASK)

extern void hw_timer_rcc_clock_cmd(T_HW_TIMER_ID id);
extern uint8_t pwm_src_div_mapping(T_PWM_CLOCK_SOURCE source);
extern VECTORn_Type hw_timer_get_vectorn_from_id(T_HW_TIMER_ID id);
extern IRQn_Type hw_timer_get_irqn_from_id(T_HW_TIMER_ID id);
extern T_HW_TIMER_HANDLE hw_timer_pool_alloc(T_HW_TIMER_FEATURE timer_feature);

#if (HW_TIMER_SUPPORT_GENERAL_IP == 1)
void hw_general_timer_driver_init_pwm_int(T_HW_TIMER_ID timer_id, uint32_t period_us_high,
                                          uint32_t period_us_low, bool deadzone)
{
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(timer_id);

    hw_timer_rcc_clock_cmd(timer_id);

    TIM_TimeBaseInitTypeDef time_param;
    TIM_StructInit(&time_param);

    time_param.ClockDepend = false;
    time_param.TIM_SOURCE_DIV = TIM_CLOCK_DIVIDER_40;
    time_param.TIM_PWM_En = PWM_ENABLE;
    time_param.TIM_PWM_High_Count = period_us_high > 1 ? (period_us_high - 1) : 1;
    time_param.TIM_PWM_Low_Count = period_us_low > 1 ? (period_us_low - 1) : 1;
    time_param.PWMDeadZone_En = deadzone ? DEADZONE_ENABLE : DEADZONE_DISABLE;
    time_param.PWM_Deazone_Size = 0;
    TIM_TimeBaseInit(TIMx, &time_param);
}

void hw_general_timer_pwm_config(T_HW_TIMER_ID timer_id, T_PWM_CONFIG *config_struct)
{
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(timer_id);

    uint32_t pwm_high_count = config_struct->pwm_high_count ? config_struct->pwm_high_count : 1;
    uint32_t pwm_low_count = config_struct->pwm_low_count ? config_struct->pwm_low_count : 1;

    TIM_TimeBaseInitTypeDef time_param;
    TIM_StructInit(&time_param);

    time_param.PWMDeadZone_En = DEADZONE_DISABLE;
    time_param.PWM_Deazone_Size = 0;
    time_param.ClockDepend = false;
    time_param.TIM_SOURCE_DIV = pwm_src_div_mapping(config_struct->clock_source);
    time_param.TIM_PWM_En = PWM_ENABLE;
    time_param.TIM_PWM_High_Count = pwm_high_count - 1;
    time_param.TIM_PWM_Low_Count = pwm_low_count - 1;

    if (config_struct->pwm_deadzone_enable && hw_timer_support_deadzone(timer_id))
    {
        time_param.PWMDeadZone_En = ENABLE;
        time_param.PWM_Deazone_Size = config_struct->pwm_deadzone_size;
        time_param.PWM_Stop_State_N = config_struct->pwm_n_stop_state;
        time_param.PWM_Stop_State_P = config_struct->pwm_p_stop_state;
    }

    TIM_TimeBaseInit(TIMx, &time_param);
}

void hw_general_timer_pwm_register_timeout_callback(T_HW_TIMER_ID timer_id, P_HW_TIMER_CBACK cback)
{
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(timer_id);

    hw_timer_update_isr(hw_timer_get_vectorn_from_id(timer_id), hw_timer_get_isr(timer_id),
                        FMC_IS_SPIC0_ADDR((uint32_t)cback));

    NVIC_InitTypeDef nvic_param;
    nvic_param.NVIC_IRQChannel = hw_timer_get_irqn_from_id(timer_id);
    nvic_param.NVIC_IRQChannelPriority = 3;
    nvic_param.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_param);

    TIM_ClearINT(TIMx);
    TIM_INTConfig(TIMx, ENABLE);
}

RAM_TEXT_SECTION void hw_general_timer_pwm_change_duty_and_frequency(T_HW_TIMER_ID timer_id,
                                                                     uint32_t high_count,
                                                                     uint32_t low_count)
{
    TIM_TypeDef *TIMx = hw_general_timer_get_reg_base_from_id(timer_id);
    TIM_PWMChangeFreqAndDuty(TIMx, high_count, low_count);
}
#endif

#if (HW_TIMER_SUPPORT_RTK_IP == 1)
void hw_rtk_timer_driver_init_pwm_int(T_HW_TIMER_ID timer_id, uint32_t period_us_high,
                                      uint32_t period_us_low, bool deadzone)
{
    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(timer_id);

    hw_timer_rcc_clock_cmd(timer_id);

    TIM_TimeBaseInitTypeDef time_param;

    TIM_StructInit(&time_param);

    time_param.TIM_Mode = TIM_Mode_UserDefine;
    time_param.TIM_ClockSrc = CK_40M_TIMER;
    time_param.TIM_ClockDiv = TIM_CLOCK_DIVIDER_1;
    time_param.TIM_Period = (period_us_high + period_us_low) * 40;
    time_param.PWM_HighCount = period_us_high * 40;
    time_param.PWM_En = ENABLE;
    time_param.PWM_DZ.PWM_DZEn = deadzone ? ENABLE : DISABLE;
    time_param.PWM_DZ.PWM_DZSize = 0;
    TIM_TimeBaseInit(TIMx, &time_param);
}

void hw_rtk_timer_pwm_config(T_HW_TIMER_ID timer_id, T_PWM_CONFIG *config_struct)
{
    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(timer_id);

    TIM_TimeBaseInitTypeDef time_param;
    TIM_StructInit(&time_param);

    time_param.PWM_DZ.PWM_DZEn = DISABLE;
    time_param.PWM_DZ.PWM_DZSize = 0;
    time_param.TIM_ClockDiv = pwm_src_div_mapping(config_struct->clock_source);
    time_param.PWM_En = ENABLE;
    time_param.TIM_Period = config_struct->pwm_high_count + config_struct->pwm_low_count;
    time_param.PWM_HighCount = config_struct->pwm_high_count;
    time_param.PWM_Polarity = (PWMPolarity_TypeDef)config_struct->pwm_polarity;
    time_param.PWM_OutputMode = (PWMOutputMode_TypeDef)config_struct->pwm_output_mode;

    if (config_struct->pwm_deadzone_enable && hw_timer_support_deadzone(timer_id))
    {
        time_param.PWM_DZ.PWM_DZEn = ENABLE;
        time_param.PWM_DZ.PWM_DZSize = config_struct->pwm_deadzone_size;
        time_param.PWM_DZ.PWM_DZStopStateN = (PWMDZStopState_TypeDef)config_struct->pwm_n_stop_state;
        time_param.PWM_DZ.PWM_DZStopStateP = (PWMDZStopState_TypeDef)config_struct->pwm_p_stop_state;
    }

    TIM_TimeBaseInit(TIMx, &time_param);
}

void hw_rtk_timer_pwm_register_timeout_callback(T_HW_TIMER_ID timer_id, P_HW_TIMER_CBACK cback)
{
    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(timer_id);

    hw_timer_update_isr(hw_timer_get_vectorn_from_id(timer_id), FMC_IS_SPIC0_ADDR((uint32_t)cback));

    NVIC_InitTypeDef nvic_param;
    nvic_param.NVIC_IRQChannel = hw_timer_get_irqn_from_id(timer_id);
    nvic_param.NVIC_IRQChannelPriority = 3;
    nvic_param.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_param);

    TIM_ClearInterrupt(TIMx, TIM_INT_TIMEOUT);
    TIM_InterruptConfig(TIMx, TIM_INT_TIMEOUT, ENABLE);
}

RAM_TEXT_SECTION void hw_rtk_timer_pwm_change_duty_and_frequency(T_HW_TIMER_ID timer_id,
                                                                 uint32_t high_count,
                                                                 uint32_t low_count)
{
    TIM_TypeDef *TIMx = hw_rtk_timer_get_reg_base_from_id(timer_id);
    TIM_PWMChangeFreqAndDuty(TIMx, high_count + low_count, high_count);
}
#endif

T_PWM_HANDLE pwm_create(const char *name,
                        uint32_t high_period_us, uint32_t low_period_us, bool deadzone_enable)
{
    T_HW_TIMER *p_hw_timer;
    T_HW_TIMER_FEATURE p_feature = {.timer_feature = 0};
    p_feature.support_pwm = 1;
    p_feature.support_pwm_deadzone = deadzone_enable;

    p_hw_timer = hw_timer_pool_alloc(p_feature);

    if (p_hw_timer != NULL)
    {
        p_hw_timer->user_name = name;
        p_hw_timer->cback = NULL;
        p_hw_timer->context = p_hw_timer;
        p_hw_timer->block_dlps_when_active = false;
        p_hw_timer->pwm_deadzone_enable = deadzone_enable;

        p_hw_timer_driver_init_pwm_int((T_HW_TIMER_ID)(p_hw_timer->timer_id), high_period_us,
                                       low_period_us, deadzone_enable);
    }

    return (T_PWM_HANDLE)p_hw_timer;
}

static bool pwm_deinit(T_PWM_HANDLE handle)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        if (p_hw_timer->pwm_deadzone_enable == false)
        {
            Pinmux_Deinit(p_hw_timer->pwm_pin_index);
            Pad_Config(p_hw_timer->pwm_pin_index,
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
            return true;
        }
        else if (p_hw_timer->pwm_deadzone_enable == true)
        {
            Pinmux_Deinit(p_hw_timer->pwm_p_pin_index);
            Pinmux_Deinit(p_hw_timer->pwm_n_pin_index);
            Pad_Config(p_hw_timer->pwm_p_pin_index,
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
            Pad_Config(p_hw_timer->pwm_n_pin_index,
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
            return true;
        }
    }

    return false;
}

bool pwm_delete(T_PWM_HANDLE handle)
{
    return (hw_timer_delete(handle) && pwm_deinit(handle));
}

RAM_TEXT_SECTION bool pwm_start(T_PWM_HANDLE handle)
{
    return hw_timer_start(handle);
}

RAM_TEXT_SECTION bool pwm_stop(T_PWM_HANDLE handle)
{
    return hw_timer_stop(handle);
}

bool pwm_register_timeout_callback(T_PWM_HANDLE handle, P_HW_TIMER_CBACK cback)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        T_HW_TIMER_ID timer_id = (T_HW_TIMER_ID)(p_hw_timer->timer_id);
        p_hw_timer->cback = cback;
        p_pwm_register_timeout_callback(timer_id, cback);

        return true;
    }

    return false;
}

bool pwm_is_active(T_PWM_HANDLE handle, bool *state)
{
    return hw_timer_is_active(handle, state);
}

bool pwm_config(T_PWM_HANDLE handle, T_PWM_CONFIG *config_struct)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        T_HW_TIMER_ID timer_id = (T_HW_TIMER_ID)(p_hw_timer->timer_id);
        p_pwm_config(timer_id, config_struct);

        return true;
    }

    return false;
}

RAM_TEXT_SECTION
bool pwm_pin_config(T_PWM_HANDLE handle, uint8_t pin_index, T_PWM_PIN_FUNC func)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        T_HW_TIMER_ID timer_id = (T_HW_TIMER_ID)p_hw_timer->timer_id;

        if (hw_timer_id_valid(timer_id) == false)
        {
            return false;
        }

        if (func == PWM_FUNC)
        {
            Pinmux_Config(pin_index, hw_timer_get_pwm_func(timer_id));
            Pad_Config(pin_index,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
            p_hw_timer->pwm_pin_index = pin_index;
        }
        else if ((func == PWM_FUNC_P) || (func == PWM_FUNC_N))
        {
            if (!hw_timer_support_deadzone(timer_id))
            {
                IO_PRINT_ERROR1("pwm %d not support deadzone", timer_id);
                return false;
            }

            Pad_Config(pin_index,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);

            if (func == PWM_FUNC_P)
            {
                Pinmux_Config(pin_index, hw_timer_get_pwm_func_p(timer_id));
                p_hw_timer->pwm_p_pin_index = pin_index;
            }
            else
            {
                Pinmux_Config(pin_index, hw_timer_get_pwm_func_n(timer_id));
                p_hw_timer->pwm_n_pin_index = pin_index;
            }
        }
        else
        {
            Pinmux_Deinit(pin_index);
            Pad_Config(pin_index,
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
        }

        return true;
    }

    return false;
}

RAM_TEXT_SECTION bool pwm_change_duty_and_frequency(T_PWM_HANDLE handle, uint32_t high_count,
                                                    uint32_t low_count)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        T_HW_TIMER_ID timer_id = (T_HW_TIMER_ID)(p_hw_timer->timer_id);
        p_pwm_change_duty_and_frequency(timer_id, high_count, low_count);
        return true;
    }

    return false;
}
