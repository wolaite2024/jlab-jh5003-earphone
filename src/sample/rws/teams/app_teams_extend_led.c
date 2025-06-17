/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     app_teams_extend_led.c
* @brief    This file provides functions to set some LED working mode.
* @details
* @author   brenda_li
* @date     2016-12-15
* @version  v0.1
*********************************************************************************************************
*/
#if F_APP_TEAMS_FEATURE_SUPPORT
#include "string.h"
#include "rtl876x_pinmux.h"
#include "app_teams_extend_led.h"
#include "app_timer.h"
#include "app_cfg.h"
#include "app_hfp.h"
#include "os_sched.h"
#include "os_mem.h"
#include "os_sync.h"
#include "board.h"
#include "trace.h"
#include "pwm.h"
#include "bt_hfp.h"
#include "app_asp_device.h"
#include "app_teams_audio_policy.h"
#include "app_teams_cmd.h"
#include "app_audio_policy.h"
#include "app_led.h"
#include "app_main.h"
#include "asp_device_link.h"

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
#include "app_single_multilink_customer.h"
#endif
#if F_APP_USB_AUDIO_SUPPORT
#include "app_usb_audio.h"
#endif

#define LED_TEAMS_NUM   2 //MAX: 6 LED
#define LED_OUT_0       P1_6
#define LED_OUT_1       P1_7

static T_LED_TABLE LED_TABLE[7][LED_TEAMS_NUM] =
{
    //LED_MODE_MUTE_ON
    {
        {LED_TYPE_KEEP_ON},
        {LED_TYPE_KEEP_OFF}
    },

    //LED_MODE_MUTE_OFF
    {
        {LED_TYPE_KEEP_OFF},
        {LED_TYPE_BYPASS}
    },

    //LED_MODE_HOOK_SWITCH_ON
    {
        {LED_TYPE_KEEP_OFF},
        {LED_TYPE_KEEP_ON}
    },

    //LED_MODE_HOOK_SWITCH_OFF
    {
        {LED_TYPE_BYPASS},
        {LED_TYPE_KEEP_OFF}
    },

    //LED_MODE_HOOK_SWITCH_FLASH
    {
        {LED_TYPE_KEEP_OFF},
        {LED_TYPE_ON_OFF, 50, 50, 1, 0}
    },

    //LED_MODE_EXTEND_ALL_ON
    {
        {LED_TYPE_KEEP_ON},
        {LED_TYPE_KEEP_ON}
    },

    //LED_MODE_EXTEND_ALL_OFF
    {
        {LED_TYPE_KEEP_OFF},
        {LED_TYPE_KEEP_OFF}
    },
};

typedef enum
{
    APP_TIMER_TEAMS_LED = 0x00,
    APP_TIMER_TEAMS_LED_CH0 = 0x01,
    APP_TIMER_TEAMS_LED_CH1 = 0x02,
    APP_TIMER_TEAMS_POWER_ON_LED = 0x03,
    APP_TIMER_TEAMS_LAUNCH_LED = 0x04,
    APP_TIMER_TEAMS_NOTIFY_LED = 0x05,
    APP_TIMER_TEAMS_BUTTON_PRESS_LED = 0x06,
} T_APP_TEAMS_TIMER;

#define PWM_OUT_COUNT (400000) //time Unit: 10ms(400000/40MHz)
static uint32_t led_breath_timer_count;
static uint8_t  led_breath_blink_count;

static T_PWM_HANDLE ext_led_pwm_handle = NULL;

static T_LED_TABLE led_setting[LED_TEAMS_NUM];
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
static T_TEAMS_LED_MODE teams_active_led_mode = LED_MODE_EXTEND_ALL_OFF;
#endif
static uint32_t led_creat_timer_point[LED_TEAMS_NUM];
static uint32_t led_creat_timer_record[LED_TEAMS_NUM];
static uint8_t led_polarity[LED_TEAMS_NUM];
static bool led_driver_by_sw;
static const char *led_timer_name[LED_TEAMS_NUM] = {"teams_led_ch0", "teams_led_ch1"};
static uint8_t timer_idx_teams_led[LED_TEAMS_NUM];
static uint8_t teams_led_module_timer_id = 0;
static uint8_t teams_led_flash_count = 1;
static T_LED_TABLE led_setting_record[LED_TEAMS_NUM];
static T_LED_TABLE led_count_record[LED_TEAMS_NUM];
static void teams_extend_led_clear_para(T_LED_CHANNEL channel);
static void teams_extend_led_blink_handle(T_LED_CHANNEL led_ch);
static void teams_extend_led_deinit(T_LED_CHANNEL channel);
static void teams_extend_led_breath_timer_control(FunctionalState new_state);
static uint8_t timer_idx_teams_power_on_led = 0;
static uint8_t timer_idx_teams_launch_led = 0;
static uint8_t timer_idx_teams_notify_led = 0;
static uint8_t timer_idx_teams_button_press_led = 0;

static void teams_extend_led_set_active_polarity(T_LED_CHANNEL channel,
                                                 T_LED_ACTIVE_POLARITY polarity)
{
    //SleepLed_SetIdleMode((SLEEP_LED_CHANNEL)channel, polarity);

    //FW work around
    switch (channel)
    {
    case LED_CH_0:
        led_polarity[0] = polarity;
        break;
    case LED_CH_1:
        led_polarity[1] = polarity;
        break;
    }
}

static uint8_t teams_extend_led_bit_to_idx(T_LED_CHANNEL channel)
{
    uint8_t num = 0;

    if (channel != 0x01)
    {
        for (;;)
        {
            channel >>= 1;
            num ++;

            if (channel == 1)
            {
                break;
            }
        }
    }

    return num;
}

static uint8_t teams_extend_led_get_pin_num(T_LED_CHANNEL channel)
{
    uint8_t Pin_Num;

    switch (channel)
    {
    case LED_CH_0:
        Pin_Num = LED_OUT_0;
        break;

    case LED_CH_1:
        Pin_Num = LED_OUT_1;
        break;

    default:
        break;
    }

    return Pin_Num;
}

/**
  * @brief  initialization of pinmux settings and pad settings.
  * @param   No parameter.
  * @return  void
  */
static void teams_extend_led_board_led_init(void)
{
    teams_extend_led_set_active_polarity(LED_CH_0, LED_ACTIVE_POLARITY_HIGH);
    Pad_Config(LED_OUT_0,
               PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    teams_extend_led_set_active_polarity(LED_CH_1, LED_ACTIVE_POLARITY_HIGH);
    Pad_Config(LED_OUT_1,
               PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
}

static void teams_extend_led_cmd(uint8_t channel, bool state)
{
    uint32_t s;

    if (led_driver_by_sw)
    {
        //LED should be handled simultaneously
        //Avoid APP be preempted by higher priority task
        s = os_lock();

        if (state)
        {
            //set pad and start timer
            if (LED_CH_0 & channel)
            {
                teams_extend_led_blink_handle(LED_CH_0);
            }

            if (LED_CH_1 & channel)
            {
                teams_extend_led_blink_handle(LED_CH_1);
            }
        }
        else
        {
            //reset pad and stop timer
            if (LED_CH_0 & channel)
            {
                teams_extend_led_deinit(LED_CH_0);
                teams_extend_led_clear_para(LED_CH_0);
                app_stop_timer(&timer_idx_teams_led[0]);
            }

            if (LED_CH_1 & channel)
            {
                teams_extend_led_deinit(LED_CH_1);
                teams_extend_led_clear_para(LED_CH_1);
                app_stop_timer(&timer_idx_teams_led[1]);
            }
        }

        os_unlock(s);
    }
}

static void teams_extend_led_deinit(T_LED_CHANNEL channel)
{
    uint8_t i = teams_extend_led_bit_to_idx(channel);
    uint8_t Pin_Num = teams_extend_led_get_pin_num(channel);

    if (led_driver_by_sw)
    {
        if (led_setting_record[i].type == LED_TYPE_BREATH)
        {
            pwm_pin_config(ext_led_pwm_handle, Pin_Num, PWM_FUNC);
        }
        else
        {
            //blink mode shoud enable pad output
            Pad_Config(Pin_Num, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
                       (PAD_OUTPUT_VAL)led_polarity[i]);
        }
    }
}

static void teams_extend_led_config(T_LED_CHANNEL channel, T_LED_TABLE *table)
{
    uint8_t ch_idx = 0;

    switch (channel)
    {
    case LED_CH_0:
        ch_idx = 0;
        break;

    case LED_CH_1:
        ch_idx = 1;
        break;
    }

    if (led_driver_by_sw)
    {
        led_setting_record[ch_idx].type = table->type;// new set type
        led_setting_record[ch_idx].on_time = table->on_time;
        led_setting_record[ch_idx].off_time = table->off_time;
        led_setting_record[ch_idx].blink_count = 2 * table->blink_count;
        led_setting_record[ch_idx].blink_interval = table->blink_interval;

        led_count_record[ch_idx].blink_count = 2 * table->blink_count;
        teams_extend_led_deinit(channel);
    }
}

static void teams_extend_led_create_timer(uint8_t i, uint32_t time, uint16_t led_ch)
{
    led_creat_timer_point[i] = os_sys_time_get();
    led_creat_timer_record[i] = time;

    switch (led_ch)
    {
    case LED_CH_0:
        led_ch = APP_TIMER_TEAMS_LED_CH0;
        break;

    case LED_CH_1:
        led_ch = APP_TIMER_TEAMS_LED_CH1;
        break;

    default:
        break;
    }

    if (time != 0)
    {
        app_start_timer(&timer_idx_teams_led[i], led_timer_name[i],
                        teams_led_module_timer_id, APP_TIMER_TEAMS_LED, led_ch, false,
                        time);
    }
}

/*
**
  * @brief  enable or disable led breath imer.
    * @param  TIM_TypeDef *index TIM_Type.
    * @param  FunctionalState NewState.
  * @return  void
  */
void teams_extend_led_breath_timer_control(FunctionalState new_state)
{
    if (new_state)
    {
        pwm_start(ext_led_pwm_handle);
    }
    else
    {
        pwm_stop(ext_led_pwm_handle);
    }
}

void teams_extend_led_breath_timer_handler(void *handle)
{
    uint8_t i = 0;
    uint8_t led_num = 0;
    uint32_t high_count = 0;
    uint32_t low_count = 0;

    for (i = 0; i < LED_TEAMS_NUM; i++)
    {
        if ((teams_extend_led_get_pin_num((T_LED_CHANNEL)BIT(i)) != 0xFF) &&
            (led_setting_record[i].type == LED_TYPE_BREATH) &&
            (led_setting_record[i].on_time != 0))
        {
            led_num = i + 1;
            break;
        }
    }

    if (led_num != 0)
    {
        led_breath_timer_count++;

        if (led_breath_timer_count <=
            2 * led_setting_record[led_num - 1].on_time) //set led breath duty during on time
        {
            high_count = PWM_OUT_COUNT * led_breath_timer_count /
                         led_setting_record[led_num - 1].on_time / 2;
            low_count = PWM_OUT_COUNT - PWM_OUT_COUNT * led_breath_timer_count /
                        led_setting_record[led_num - 1].on_time / 2;
        }
        else if ((led_breath_timer_count >
                  2 * led_setting_record[led_num - 1].on_time) && //set led breath duty during off time
                 (led_breath_timer_count <=
                  2 * (led_setting_record[led_num - 1].on_time + led_setting_record[led_num - 1].off_time)))
        {
            high_count = PWM_OUT_COUNT - PWM_OUT_COUNT * (led_breath_timer_count -
                                                          2 * led_setting_record[led_num - 1].on_time) / led_setting_record[led_num - 1].off_time / 2;
            low_count = PWM_OUT_COUNT * (led_breath_timer_count -
                                         2 * led_setting_record[led_num - 1].on_time) / led_setting_record[led_num - 1].off_time / 2;
        }
        else if (led_breath_timer_count >
                 2 * (led_setting_record[led_num - 1].on_time + led_setting_record[led_num - 1].off_time))
        {
            //restart blink when led_breath_blink_count is less than blink_count
            if (led_breath_blink_count < led_setting_record[led_num - 1].blink_count / 2)
            {
                led_breath_timer_count = 0;
            }
            else
            {
                if (led_breath_timer_count <= (2 * (led_setting_record[led_num - 1].on_time +
                                                    led_setting_record[led_num - 1].off_time) +
                                               led_setting_record[led_num - 1].blink_interval *
                                               10)) // disable led breath when blink_interval is not zero
                {
                    high_count = 0;
                    low_count = PWM_OUT_COUNT;
                }
                else
                {
                    led_breath_timer_count = 0;
                    led_breath_blink_count = 0;
                }
            }
        }

        if (led_breath_timer_count == 2 * (led_setting_record[led_num - 1].on_time +
                                           led_setting_record[led_num - 1].off_time))
        {
            led_breath_blink_count++;
        }

        /*up the change duty cnt */
        if (led_breath_timer_count != 0)
        {
            /*change duty */
            pwm_change_duty_and_frequency(ext_led_pwm_handle, high_count, low_count);
        }
    }
}

void teams_extend_led_breath_init(void)
{
    if (ext_led_pwm_handle == NULL)
    {
        ext_led_pwm_handle = pwm_create("ext_led", PWM_OUT_COUNT, PWM_OUT_COUNT, false);
        if (ext_led_pwm_handle == NULL)
        {
            APP_PRINT_ERROR0("Could not create extend led pwm!!!");
            return;
        }
    }
    pwm_register_timeout_callback(ext_led_pwm_handle, teams_extend_led_breath_timer_handler);
}

static void teams_extend_led_blink_handle(T_LED_CHANNEL led_ch)
{
    uint8_t idx = teams_extend_led_bit_to_idx(led_ch);
    uint8_t Pin_Num = teams_extend_led_get_pin_num(led_ch);

    if (Pin_Num == 0xFF)
    {
        return;
    }

    if ((led_setting_record[idx].type == LED_TYPE_KEEP_ON) ||
        (led_setting_record[idx].type == LED_TYPE_ON_OFF))
    {
        if (led_polarity[idx])
        {
            Pad_OutputControlValue(Pin_Num, PAD_OUT_LOW);
        }
        else
        {
            Pad_OutputControlValue(Pin_Num, PAD_OUT_HIGH);
        }
    }
    else
    {
        teams_extend_led_deinit(led_ch);
    }

    if ((led_setting_record[idx].type == LED_TYPE_ON_OFF) ||
        (led_setting_record[idx].type == LED_TYPE_OFF_ON))
    {
        teams_extend_led_create_timer(idx, led_setting_record[idx].on_time * 10, led_ch);
    }
    else if (led_setting_record[idx].type == LED_TYPE_BREATH)
    {
        teams_extend_led_breath_timer_control(ENABLE);
    }
}

static void teams_extend_led_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("teams_extend_led_timeout_cb: timer_evt 0x%02x, param %d", timer_evt, param);
    switch (timer_evt)
    {
    case APP_TIMER_TEAMS_LED:
        {
            uint8_t i = teams_extend_led_bit_to_idx((T_LED_CHANNEL)param);
            uint8_t Pin_Num = teams_extend_led_get_pin_num((T_LED_CHANNEL)param);

            if ((os_sys_time_get() - led_creat_timer_point[i]) >= led_creat_timer_record[i])
            {
                if (led_setting_record[i].blink_count == 0) //restart repeat blink when blink_interval is not zero
                {
                    teams_extend_led_deinit((T_LED_CHANNEL)param);
                    led_setting_record[i].blink_count = led_count_record[i].blink_count;
                    teams_extend_led_cmd((T_LED_CHANNEL)param, true);
                }
                else if (led_setting_record[i].blink_count != 1)
                {
                    led_setting_record[i].blink_count--;
                    if (Pad_GetOutputCtrl(Pin_Num) == PAD_OUT_LOW)
                    {
                        Pad_OutputControlValue(Pin_Num, PAD_OUT_HIGH);
                    }
                    else
                    {
                        Pad_OutputControlValue(Pin_Num, PAD_OUT_LOW);
                    }

                    if (led_setting_record[i].blink_count % 2 == 1) //start off timer when blink_cnt is odd
                    {
                        teams_extend_led_create_timer(i, led_setting_record[i].off_time * 10, param);
                    }
                    else
                    {
                        teams_extend_led_create_timer(i, led_setting_record[i].on_time * 10, param);
                    }
                }
                else
                {
                    if (led_setting_record[i].blink_interval != 0) //blink_interval is not zero
                    {
                        teams_extend_led_deinit((T_LED_CHANNEL)param);
                        led_setting_record[i].blink_count = 0;
                        teams_extend_led_create_timer(i, led_setting_record[i].blink_interval * 100, param);
                    }
                    else
                    {
                        teams_extend_led_deinit((T_LED_CHANNEL)param);
                        led_setting_record[i].blink_count = led_count_record[i].blink_count;
                        teams_extend_led_cmd((T_LED_CHANNEL)param, true);
                    }
                }
            }
        }
        break;

    case APP_TIMER_TEAMS_POWER_ON_LED:
        {
            app_stop_timer(&timer_idx_teams_power_on_led);
            if (!asp_device_api_get_active_link_addr())
            {
                if (teams_led_flash_count < 5)
                {
                    //teams led flash
                    app_led_change_mode(LED_MODE_TEAMS_LAUNCHING, true, false);

                    app_start_timer(&timer_idx_teams_power_on_led, "teams_power_on_led",
                                    teams_led_module_timer_id, APP_TIMER_TEAMS_POWER_ON_LED, 0, false,
                                    2000);
                    teams_led_flash_count += 1;
                }
                else
                {
                    //teams led off
                    app_led_change_mode(LED_MODE_TEAMS_CLOSE, true, false);
                }
            }
        }
        break;

    case APP_TIMER_TEAMS_LAUNCH_LED:
        {
            app_stop_timer(&timer_idx_teams_launch_led);
            //teams led on
            app_led_change_mode(LED_MODE_TEAMS_LAUNCHED, true, false);

            app_teams_set_launch_status(1);
        }
        break;

    case APP_TIMER_TEAMS_NOTIFY_LED:
        {
            app_stop_timer(&timer_idx_teams_notify_led);
            //teams led on
            app_led_change_mode(LED_MODE_TEAMS_LAUNCHED, true, false);
        }
        break;

    case APP_TIMER_TEAMS_BUTTON_PRESS_LED:
        {
            app_stop_timer(&timer_idx_teams_button_press_led);
            //teams led off
            app_led_change_mode(LED_MODE_TEAMS_CLOSE, true, false);
        }
        break;

    default:
        break;
    }
}

static void teams_extend_led_clear_para(T_LED_CHANNEL channel)
{
    uint8_t i = teams_extend_led_bit_to_idx(channel);

    memset(&led_setting_record[i], 0, sizeof(T_LED_TABLE));
}

static void teams_extend_led_load_table(uint8_t i)
{
    memcpy((void *)&led_setting,
           (const void *)&LED_TABLE[i], LED_TEAMS_NUM * sizeof(T_LED_TABLE));
}

static void teams_extend_led_control(uint8_t bit_num)
{
    teams_extend_led_cmd(BIT(bit_num), DISABLE);

    teams_extend_led_config((T_LED_CHANNEL)BIT(bit_num), &led_setting[bit_num]);
}

void teams_extend_led_set_mode(T_TEAMS_LED_MODE mode)
{
    uint8_t channel = 0;

#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
    APP_PRINT_INFO3("teams_extend_led_set_mode: cur_call_status %d, pre_mode 0x%02x, mode 0x%02x",
                    app_teams_multilink_get_voice_status(), teams_active_led_mode, mode);
#endif

    teams_extend_led_load_table(mode);

    switch (mode)
    {
    case LED_MODE_MUTE_OFF:
        {
            teams_extend_led_control(0);
            channel = BIT(0);
        }
        break;

    case LED_MODE_HOOK_SWITCH_OFF:
        {
            teams_extend_led_control(1);
            channel = BIT(1);
        }
        break;

    case LED_MODE_MUTE_ON:
    case LED_MODE_HOOK_SWITCH_ON:
    case LED_MODE_HOOK_SWITCH_FLASH:
    case LED_MODE_EXTEND_ALL_ON:
    case LED_MODE_EXTEND_ALL_OFF:
        {
            teams_extend_led_control(0);
            teams_extend_led_control(1);
            channel = BIT(0) | BIT(1);
        }
        break;

    default:
        break;
    }

    teams_extend_led_cmd(channel, ENABLE);

#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
    teams_active_led_mode = mode;
#endif
}

static void teams_extend_led_set_driver_mode(void)
{
    led_driver_by_sw = true;
    app_timer_reg_cb(teams_extend_led_timeout_cb, &teams_led_module_timer_id);
}

/**
  * @brief  demo code of operation about LED breathe mode.
  * @param   No parameter.
  * @return  void
  */
void teams_extend_led_init(void)
{
    teams_extend_led_set_driver_mode();
    teams_extend_led_board_led_init();
}

#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
void teams_extend_led_check_mode(void)
{
    if ((app_cfg_const.led_support) && (app_teams_cmd_data.teams_test_mode != 0x08))
    {
        T_TEAMS_LED_MODE teams_change_mode = LED_MODE_EXTEND_ALL_OFF;
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
        T_APP_CALL_STATUS cur_call_status = app_teams_multilink_get_voice_status();
#else
        T_APP_CALL_STATUS cur_call_status = app_hfp_get_call_status();
#endif
        if (app_db.device_state == APP_DEVICE_STATE_ON)
        {
            if (app_teams_audio_get_global_mute_status())
            {
                teams_change_mode = LED_MODE_MUTE_ON;
            }
            else
            {
                if (cur_call_status >= APP_CALL_INCOMING)
                {
                    teams_change_mode = LED_MODE_HOOK_SWITCH_ON;
                }
                else
                {
                    teams_change_mode = LED_MODE_EXTEND_ALL_OFF;
                }
            }
        }
        else
        {
            teams_change_mode = LED_MODE_EXTEND_ALL_OFF;
        }

        if (teams_change_mode != teams_active_led_mode)
        {
            teams_extend_led_set_mode(teams_change_mode);
        }
    }
    else
    {
        APP_PRINT_INFO2("teams_extend_led_check_mode: led_support %d, test_mode %d",
                        app_cfg_const.led_support, app_teams_cmd_data.teams_test_mode);
    }

}
#endif

void teams_extend_led_stop_timer(void)
{
    app_stop_timer(&timer_idx_teams_power_on_led);
    app_stop_timer(&timer_idx_teams_launch_led);
    app_stop_timer(&timer_idx_teams_notify_led);
}

void teams_extend_led_control_when_power_on(void)
{
    teams_led_flash_count = 1;
    //teams led flash
    app_led_change_mode(LED_MODE_TEAMS_LAUNCHING, true, false);
    app_start_timer(&timer_idx_teams_power_on_led, "teams_power_on_led",
                    teams_led_module_timer_id, APP_TIMER_TEAMS_POWER_ON_LED, 0, false,
                    2000);
}

void teams_extend_led_control_when_teams_launch(void)
{
    app_stop_timer(&timer_idx_teams_power_on_led);
    app_stop_timer(&timer_idx_teams_notify_led);
    //teams led breath
    app_led_change_mode(LED_MODE_TEAMS_NOTIFY, true, false);

    app_start_timer(&timer_idx_teams_launch_led, "teams_launch_led",
                    teams_led_module_timer_id, APP_TIMER_TEAMS_LAUNCH_LED, 0, false,
                    2000);
}

void teams_extend_led_control_when_notify(void)
{
    //teams led breath
    app_led_change_mode(LED_MODE_TEAMS_NOTIFY, true, false);

    app_start_timer(&timer_idx_teams_notify_led, "teams_notify_led",
                    teams_led_module_timer_id, APP_TIMER_TEAMS_NOTIFY_LED, 0, false,
                    300 * 1000);
}

void teams_extend_led_control_when_clear_notification(void)
{
    app_stop_timer(&timer_idx_teams_notify_led);

    //teams led on
    app_led_change_mode(LED_MODE_TEAMS_LAUNCHED, true, false);
}

void teams_extend_led_tone_control_when_press_teams_button(void)
{
    if (!asp_device_api_get_active_link_addr())
    {
        app_audio_tone_type_play(TONE_ERROR, false, true);//Tone for error

        //teams led flash
        app_led_change_mode(LED_MODE_TEAMS_LAUNCHING, true, false);

        app_start_timer(&timer_idx_teams_button_press_led, "teams_button_press_led",
                        teams_led_module_timer_id, APP_TIMER_TEAMS_BUTTON_PRESS_LED, 0, false,
                        3000);
    }
}

#endif
