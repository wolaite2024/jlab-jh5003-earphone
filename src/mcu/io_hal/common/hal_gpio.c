/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     hal_gpio.c
* @brief    This file provides all the gpio hal functions.
* @details
* @author
* @date
* @version  v0.1
*********************************************************************************************************
*/
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "os_mem.h"

#include "trace.h"
#include "section.h"
#include "pm.h"
#include "rtl876x.h"
#include "io_dlps.h"
#include "hal_gpio.h"
#include "hal_gpio_int.h"

#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"

#define TOTAL_HAL_PIN_NUM                      TOTAL_PIN_NUM

//#define PRINT_GPIO_LOGS
#ifdef PRINT_GPIO_LOGS
#define GPIO_PRINT_INFO(fmt, ...)                DBG_DIRECT(fmt, __VA_ARGS__)
#else
#define GPIO_PRINT_INFO(...)
#endif

#define IS_GPIO_INVALID(gpio_num)                   (gpio_num == 0xff)
#define IS_FLAG_INVALID(flag_index)                 (flag_index == 0xff)
#define PIN_FLAG_TYPE_BIT                           (0x03)
#define PIN_GET_GPIO_TYPE(flag_index)                ((T_GPIO_TYPE)(hal_gpio_sw_context.pin_flags[flag_index] & PIN_FLAG_TYPE_BIT))

#define PIN_FLAG_FIRST_WAKE_UP_CONFIG_BIT           BIT2
#define PIN_FLAG_FIRST_WAKE_UP_CONFIG(flag_index)    (!(hal_gpio_sw_context.pin_flags[flag_index] & PIN_FLAG_FIRST_WAKE_UP_CONFIG_BIT))
#define PIN_FLAG_WAKE_UP_POLARITY_BIT               BIT3
#define PIN_FLAG_WAKE_UP_POLARITY(flag_index)        ((hal_gpio_sw_context.pin_flags[flag_index] & PIN_FLAG_WAKE_UP_POLARITY_BIT) >> 3)
#define PIN_FLAG_POLARITY_BIT                       BIT4
#define PIN_FLAG_POLARITY(flag_index)                ((hal_gpio_sw_context.pin_flags[flag_index] & PIN_FLAG_POLARITY_BIT) >> 4)
#define PIN_FLAG_NEED_STORE_BIT                     BIT5
#define PIN_FLAG_IS_NEED_STORE(flag_index)           ((hal_gpio_sw_context.pin_flags[flag_index] & PIN_FLAG_NEED_STORE_BIT))
#define PIN_FLAG_OUTPUT_MODE_BIT                    BIT6
#define PIN_FLAG_IS_OUTPUT_MODE(flag_index)         ((hal_gpio_sw_context.pin_flags[flag_index] & PIN_FLAG_OUTPUT_MODE_BIT))
#define PIN_FLAG_OUTPUT_VALUE_BIT                   BIT7
#define PIN_FLAG_OUTPUT_VALUE(flag_index)           ((hal_gpio_sw_context.pin_flags[flag_index] & PIN_FLAG_OUTPUT_VALUE_BIT) >> 7)

typedef struct t_gpio_state
{
    uint8_t *pin_flags;
    uint32_t debounce_time;
} T_GPIO_SW_CONTEXT;

T_GPIO_SW_CONTEXT hal_gpio_sw_context =
{
    .pin_flags = 0,
    .debounce_time = 30,
};

#define IS_HAL_GPIO_MALLOC_FAILED()                  (hal_gpio_sw_context.pin_flags == NULL)

bool hal_gpio_init_flag = false;
bool hal_gpio_mem_init_flag = false;
uint8_t total_pin_num = TOTAL_HAL_PIN_NUM;
const uint8_t *pin_table = NULL;
bool hal_gpio_pin_all_open = true;
#define IS_GPIO_ALL_OPEN()                          (hal_gpio_pin_all_open)

extern void io_driver_dlps_register_exit_cb(P_IO_DLPS_CALLBACK func);
extern void io_driver_dlps_register_enter_cb(P_IO_DLPS_CALLBACK func);
extern void io_driver_dlps_register_pre_clock_exit_cb(P_IO_DLPS_CALLBACK func);
extern void hal_gpio_int_deinit(void);
extern bool hal_gpio_update_pin_table(void);
extern uint32_t GPIO_GetDLPSBufferLen(void);
extern void GPIO_DLPSEnter(void *peri_reg, void *p_store_buf);
extern void GPIO_DLPSExitOutputMode(void *peri_reg, void *p_store_buf);
extern void GPIO_DLPSExitInputMode(void *peri_reg, void *p_store_buf);

void *gpio_a_store_buf = NULL;

#ifdef GPIOB
void *gpio_b_store_buf = NULL;
#endif

#ifdef GPIOC
void *gpio_c_store_buf = NULL;
#endif

ISR_TEXT_SECTION
GPIO_TypeDef *get_gpio(uint8_t gpio_num)
{
    if (gpio_num < 32)
    {
        return GPIOA;
    }
#ifdef GPIOB
    else if (gpio_num < 64)
    {
        return GPIOB;
    }
#endif
#ifdef GPIOC
    else if (gpio_num < 96)
    {
        return GPIOC;
    }
#endif
    else
    {
        return NULL;
    }
}

T_GPIO_STATUS hal_gpio_set_pull_value(uint8_t pin_index, T_GPIO_PULL_VALUE pull_value)
{
    Pad_PullEnableValue_Dir(pin_index, 1, (PAD_Pull_Mode)pull_value);
    return GPIO_STATUS_OK;
}

static PAD_Pull_Mode hal_pull_to_pad_pull(T_GPIO_PULL_VALUE pull_value)
{
    switch (pull_value)
    {
    case GPIO_PULL_DOWN:
        return PAD_PULL_DOWN;
    case GPIO_PULL_UP:
        return PAD_PULL_UP;
    case GPIO_PULL_NONE:
        return PAD_PULL_NONE;
    default:
        return PAD_PULL_DOWN;
    }
}

static void hal_gpio_init_flag_mem(void)
{
    if (hal_gpio_mem_init_flag == false)
    {
        hal_gpio_pin_all_open = hal_gpio_update_pin_table();
        hal_gpio_sw_context.pin_flags = (uint8_t *)calloc(total_pin_num, 1);
        if (IS_HAL_GPIO_MALLOC_FAILED())
        {
            IO_PRINT_ERROR0("hal_gpio_init_flag_mem: pin_flags malloc failed");
        }

        gpio_a_store_buf = os_mem_zalloc(OS_MEM_TYPE_DATA, GPIO_GetDLPSBufferLen());
        if (gpio_a_store_buf == NULL)
        {
            IO_PRINT_ERROR0("hal_gpio_init_flag_mem: gpioa dlps buf alloc fail");
        }

#ifdef GPIOB
        gpio_b_store_buf = os_mem_zalloc(OS_MEM_TYPE_DATA, GPIO_GetDLPSBufferLen());
        if (gpio_b_store_buf == NULL)
        {
            IO_PRINT_ERROR0("hal_gpio_init_flag_mem: gpiob dlps buf alloc fail");
        }
#endif

#ifdef GPIOC
        gpio_c_store_buf = os_mem_zalloc(OS_MEM_TYPE_DATA, GPIO_GetDLPSBufferLen());
        if (gpio_c_store_buf == NULL)
        {
            IO_PRINT_ERROR0("hal_gpio_init_flag_mem: gpioc dlps buf alloc fail");
        }
#endif

        hal_gpio_mem_init_flag = true;
    }
}

static void hal_gpio_init_peri(void)
{
    if (hal_gpio_init_flag == false)
    {
        hal_gpio_init();
        hal_gpio_int_init();
    }
}

uint8_t hal_gpio_pin_index_map_flag_index(uint8_t pin_index)
{
    uint8_t index = pin_index;

    if (IS_GPIO_ALL_OPEN())
    {
        return index;
    }

    if (pin_table == NULL)
    {
        return 0xFF;
    }

    for (index = 0; index < total_pin_num; index++)
    {
        if (pin_table[index] == pin_index)
        {
            return index;
        }
    }

    return 0xFF;
}

T_GPIO_STATUS hal_gpio_init_pin(uint8_t pin_index, T_GPIO_TYPE type, T_GPIO_DIRECTION direction,
                                T_GPIO_PULL_VALUE pull_value)
{
    hal_gpio_init_flag_mem();

    uint8_t gpio_num = GPIO_GetNum(pin_index);
    uint8_t flag_index = hal_gpio_pin_index_map_flag_index(pin_index);

    if (IS_HAL_GPIO_MALLOC_FAILED())
    {
        return GPIO_STATUS_ERROR;
    }

    if (IS_GPIO_INVALID(gpio_num) || IS_FLAG_INVALID(flag_index))
    {
        return GPIO_STATUS_ERROR_PIN;
    }

    if ((type == GPIO_TYPE_AON) && (direction == GPIO_DIR_INPUT))
    {
        type = GPIO_TYPE_AUTO;
    }

    PAD_Mode aon_pad_mode = PAD_SW_MODE;
    PAD_PWR_Mode pad_power_mode = PAD_IS_PWRON;
    PAD_Pull_Mode aon_pad_pull = hal_pull_to_pad_pull(pull_value);
    PAD_OUTPUT_ENABLE_Mode pad_output_mode = (type == GPIO_TYPE_CORE) ? PAD_OUT_DISABLE :
                                             (PAD_OUTPUT_ENABLE_Mode)direction;
    PAD_OUTPUT_VAL output_value = PAD_OUT_LOW;

    hal_gpio_sw_context.pin_flags[flag_index] = (hal_gpio_sw_context.pin_flags[flag_index] &
                                                 (~(PIN_FLAG_TYPE_BIT | PIN_FLAG_OUTPUT_VALUE_BIT))) | type;

    if ((type == GPIO_TYPE_AUTO) && (direction == GPIO_DIR_OUTPUT))
    {
        hal_gpio_sw_context.pin_flags[flag_index] = hal_gpio_sw_context.pin_flags[flag_index] |
                                                    PIN_FLAG_OUTPUT_MODE_BIT;
    }

    if (type != GPIO_TYPE_AON)
    {
        uint32_t gpio_pin = GPIO_GetPin(pin_index);
        GPIO_TypeDef *GPIOx = get_gpio(gpio_num);
        hal_gpio_init_peri();

        Pinmux_Config(pin_index, DWGPIO);
        GPIOx_ModeSet(GPIOx, gpio_pin, (GPIOMode_TypeDef)direction);
        aon_pad_mode = PAD_PINMUX_MODE;
    }

    Pad_Config(pin_index, aon_pad_mode, pad_power_mode, aon_pad_pull, pad_output_mode, output_value);

    return GPIO_STATUS_OK;
}

T_GPIO_TYPE hal_gpio_get_type(uint8_t flag_index)
{
    return PIN_GET_GPIO_TYPE(flag_index);
}

T_GPIO_STATUS hal_gpio_change_direction(uint8_t pin_index, T_GPIO_DIRECTION direction)
{
    T_GPIO_TYPE type = GPIO_TYPE_AUTO;
    uint32_t gpio_pin = GPIO_GetPin(pin_index);
    uint8_t gpio_num = GPIO_GetNum(pin_index);
    GPIO_TypeDef *GPIOx = get_gpio(gpio_num);
    uint8_t flag_index = hal_gpio_pin_index_map_flag_index(pin_index);

    if (IS_HAL_GPIO_MALLOC_FAILED())
    {
        return GPIO_STATUS_ERROR;
    }

    if (IS_GPIO_INVALID(gpio_num) || IS_FLAG_INVALID(flag_index))
    {
        return GPIO_STATUS_ERROR_PIN;
    }

    type = PIN_GET_GPIO_TYPE(flag_index);

    switch (type)
    {
    case GPIO_TYPE_CORE:
        GPIOx_ModeSet(GPIOx, gpio_pin, (GPIOMode_TypeDef)direction);
        break;

    case GPIO_TYPE_AON:
        Pad_OutputEnableValue(pin_index, direction);
        break;

    case GPIO_TYPE_AUTO:
        Pad_OutputEnableValue(pin_index, direction);
        GPIOx_ModeSet(GPIOx, gpio_pin, (GPIOMode_TypeDef)direction);
        hal_gpio_sw_context.pin_flags[flag_index] = (hal_gpio_sw_context.pin_flags[flag_index] &
                                                     (~PIN_FLAG_OUTPUT_MODE_BIT) | (direction << 6));
        break;

    default:
        break;
    }

    return GPIO_STATUS_OK;
}

ISR_TEXT_SECTION
T_GPIO_STATUS hal_gpio_set_level(uint8_t pin_index, T_GPIO_LEVEL level)
{
    T_GPIO_TYPE type = GPIO_TYPE_AUTO;
    uint8_t gpio_num = GPIO_GetNum(pin_index);
    uint8_t flag_index = hal_gpio_pin_index_map_flag_index(pin_index);

    if (IS_HAL_GPIO_MALLOC_FAILED())
    {
        return GPIO_STATUS_ERROR;
    }

    if (IS_GPIO_INVALID(gpio_num) || IS_FLAG_INVALID(flag_index))
    {
        return GPIO_STATUS_ERROR_PIN;
    }

    type = PIN_GET_GPIO_TYPE(flag_index);

    if (type ==  GPIO_TYPE_AON)
    {
        Pad_OutputControlValue(pin_index, level);
    }
    else
    {
        GPIOx_WriteBit(get_gpio(gpio_num), GPIO_GetPin(pin_index), (BitAction)level);
    }

    return GPIO_STATUS_OK;
}

ISR_TEXT_SECTION
T_GPIO_LEVEL hal_gpio_get_input_level(uint8_t pin_index)
{
    T_GPIO_TYPE type = GPIO_TYPE_AUTO;
    uint8_t gpio_num = GPIO_GetNum(pin_index);
    uint8_t flag_index = hal_gpio_pin_index_map_flag_index(pin_index);

    if (IS_HAL_GPIO_MALLOC_FAILED())
    {
        return GPIO_LEVEL_UNKNOWN;
    }

    if (IS_GPIO_INVALID(gpio_num) || IS_FLAG_INVALID(flag_index))
    {
        return GPIO_LEVEL_UNKNOWN;
    }

    type = PIN_GET_GPIO_TYPE(flag_index);

    if (type == GPIO_TYPE_AON)
    {
        return GPIO_LEVEL_UNKNOWN;
    }

    return (T_GPIO_LEVEL)GPIOx_ReadInputDataBit(get_gpio(gpio_num), GPIO_GetPin(pin_index));
}

static T_GPIO_STATUS gpio_set_up_irq_active(uint8_t pin_index, T_GPIO_IRQ_MODE mode,
                                            T_GPIO_IRQ_POLARITY polarity,
                                            bool debounce_enable)
{
    uint32_t gpio_pin = GPIO_GetPin(pin_index);
    uint32_t gpio_num = GPIO_GetNum(pin_index);
    GPIO_TypeDef *p_gpio = get_gpio(gpio_num);
    uint8_t flag_index = hal_gpio_pin_index_map_flag_index(pin_index);

    if (IS_HAL_GPIO_MALLOC_FAILED())
    {
        return GPIO_STATUS_ERROR;
    }

    if (IS_GPIO_INVALID(gpio_num) || IS_FLAG_INVALID(flag_index))
    {
        return GPIO_STATUS_ERROR_PIN;
    }

    hal_gpio_sw_context.pin_flags[flag_index] = (hal_gpio_sw_context.pin_flags[flag_index] &
                                                 (~PIN_FLAG_POLARITY_BIT)) | (polarity << 4);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_PinBit  = gpio_pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_ITCmd = ENABLE;
    GPIO_InitStruct.GPIO_ITTrigger = (GPIO_IRQ_EDGE == mode) ? GPIO_INT_Trigger_EDGE :
                                     GPIO_INT_Trigger_LEVEL;
    GPIO_InitStruct.GPIO_ITPolarity = (polarity == GPIO_IRQ_ACTIVE_HIGH) ?
                                      GPIO_INT_POLARITY_ACTIVE_HIGH : GPIO_INT_POLARITY_ACTIVE_LOW;
    GPIO_InitStruct.GPIO_ITDebounce = debounce_enable ? GPIO_INT_DEBOUNCE_ENABLE :
                                      GPIO_INT_DEBOUNCE_DISABLE;
    GPIO_InitStruct.GPIO_DebounceTime = hal_gpio_sw_context.debounce_time;

    GPIOx_Init(p_gpio, &GPIO_InitStruct);

    return GPIO_STATUS_OK;
}

T_GPIO_STATUS hal_gpio_set_up_irq(uint8_t pin_index, T_GPIO_IRQ_MODE mode,
                                  T_GPIO_IRQ_POLARITY polarity,
                                  bool debounce_enable)
{
    return gpio_set_up_irq_active(pin_index, mode, polarity, debounce_enable);
}

ISR_TEXT_SECTION
T_GPIO_STATUS hal_gpio_irq_change_polarity(uint8_t pin_index, T_GPIO_IRQ_POLARITY polarity)
{
    uint32_t gpio_num = GPIO_GetNum(pin_index);
    uint32_t gpio_pin = GPIO_GetPin(pin_index);
    uint8_t flag_index = hal_gpio_pin_index_map_flag_index(pin_index);

    if (IS_HAL_GPIO_MALLOC_FAILED())
    {
        return GPIO_STATUS_ERROR;
    }

    if (IS_GPIO_INVALID(gpio_num) || IS_FLAG_INVALID(flag_index))
    {
        return GPIO_STATUS_ERROR_PIN;
    }

    hal_gpio_sw_context.pin_flags[flag_index] = (hal_gpio_sw_context.pin_flags[flag_index] &
                                                 (~PIN_FLAG_POLARITY_BIT)) | (polarity << 4);

    GPIOx_IntPolaritySet(get_gpio(gpio_num), gpio_pin, (GPIOIT_PolarityType)polarity);
    return GPIO_STATUS_OK;
}

ISR_TEXT_SECTION
T_GPIO_STATUS hal_gpio_irq_enable(uint8_t pin_index)
{
    uint32_t gpio_pin = GPIO_GetPin(pin_index);
    uint32_t gpio_num = GPIO_GetNum(pin_index);
    GPIO_TypeDef *p_gpio = get_gpio(gpio_num);
    T_GPIO_TYPE type = GPIO_TYPE_AUTO;
    uint8_t flag_index = hal_gpio_pin_index_map_flag_index(pin_index);

    if (IS_HAL_GPIO_MALLOC_FAILED())
    {
        return GPIO_STATUS_ERROR;
    }

    if (IS_GPIO_INVALID(gpio_num) || IS_FLAG_INVALID(flag_index))
    {
        return GPIO_STATUS_ERROR_PIN;
    }

    type = PIN_GET_GPIO_TYPE(flag_index);

    GPIOx_MaskINTConfig(p_gpio, gpio_pin, (FunctionalState)DISABLE);
    GPIOx_INTConfig(p_gpio, gpio_pin, (FunctionalState)ENABLE);
    if (type == GPIO_TYPE_AUTO)
    {
        hal_gpio_sw_context.pin_flags[flag_index] |= PIN_FLAG_NEED_STORE_BIT;
    }
    return GPIO_STATUS_OK;
}

ISR_TEXT_SECTION
T_GPIO_STATUS hal_gpio_irq_disable(uint8_t pin_index)
{
    uint32_t gpio_pin = GPIO_GetPin(pin_index);
    uint32_t gpio_num = GPIO_GetNum(pin_index);
    GPIO_TypeDef *p_gpio = get_gpio(gpio_num);
    T_GPIO_TYPE type = GPIO_TYPE_AUTO;
    uint8_t flag_index = hal_gpio_pin_index_map_flag_index(pin_index);

    if (IS_HAL_GPIO_MALLOC_FAILED())
    {
        return GPIO_STATUS_ERROR;
    }

    if (IS_GPIO_INVALID(gpio_num) || IS_FLAG_INVALID(flag_index))
    {
        return GPIO_STATUS_ERROR_PIN;
    }

    type = PIN_GET_GPIO_TYPE(flag_index);

    GPIOx_MaskINTConfig(p_gpio, gpio_pin, (FunctionalState)ENABLE);
    GPIOx_INTConfig(p_gpio, gpio_pin, (FunctionalState)DISABLE);
    if (type == GPIO_TYPE_AUTO)
    {
        hal_gpio_sw_context.pin_flags[flag_index] &= (~PIN_FLAG_NEED_STORE_BIT);
        hal_gpio_sw_context.pin_flags[flag_index] = hal_gpio_sw_context.pin_flags[flag_index] &
                                                    (~PIN_FLAG_FIRST_WAKE_UP_CONFIG_BIT);
        System_WakeUpPinDisable(pin_index);
        System_WakeUpInterruptDisable(pin_index);
    }
    return GPIO_STATUS_OK;
}

T_GPIO_STATUS hal_gpio_set_debounce_time(uint8_t ms)
{
    if (!ms)
    {
        ms = 1;
    }
    hal_gpio_sw_context.debounce_time = ms;

    return GPIO_STATUS_OK;
}

static void hal_gpio_pm_store_config_wake_up(uint8_t pin_index, uint8_t flag_index)
{
    T_GPIO_TYPE type = PIN_GET_GPIO_TYPE(flag_index);

    if (type != GPIO_TYPE_AUTO)
    {
        return;
    }

    T_GPIO_IRQ_POLARITY polarity = (T_GPIO_IRQ_POLARITY)(PIN_FLAG_POLARITY(flag_index));

    if (PIN_FLAG_FIRST_WAKE_UP_CONFIG(flag_index) ||
        (PIN_FLAG_WAKE_UP_POLARITY(flag_index) != polarity))
    {
        if (polarity == GPIO_IRQ_ACTIVE_HIGH)
        {
            System_WakeUpPinEnable(pin_index, PAD_WAKEUP_POL_HIGH);
        }
        else
        {
            System_WakeUpPinEnable(pin_index, PAD_WAKEUP_POL_LOW);
        }
        System_WakeUpInterruptEnable(pin_index);
        hal_gpio_sw_context.pin_flags[flag_index] = (hal_gpio_sw_context.pin_flags[flag_index] &
                                                     (~PIN_FLAG_WAKE_UP_POLARITY_BIT)) | (polarity << 3) | PIN_FLAG_FIRST_WAKE_UP_CONFIG_BIT;
    }
}

static void hal_gpio_pm_store_config_output_mode(uint8_t pin_index, uint8_t flag_index)
{
    T_GPIO_TYPE type = PIN_GET_GPIO_TYPE(flag_index);

    if (type != GPIO_TYPE_AUTO)
    {
        return;
    }

    uint8_t gpio_num = GPIO_GetNum(pin_index);
    uint32_t gpio_pin = GPIO_GetPin(pin_index);
    GPIO_TypeDef *GPIOx = get_gpio(gpio_num);

    uint8_t level  = GPIOx_ReadOutputDataBit(GPIOx, gpio_pin);

    if (level != PIN_FLAG_OUTPUT_VALUE(flag_index))
    {
        Pad_OutputControlValue(pin_index, level);
        hal_gpio_sw_context.pin_flags[flag_index] = (hal_gpio_sw_context.pin_flags[flag_index] &
                                                     (~PIN_FLAG_OUTPUT_VALUE_BIT)) | (level << 7);
    }
}

static void hal_gpio_pm_store(void)
{
    uint8_t flag_index = 0;
    uint8_t pin_index = 0;

    if (!IS_HAL_GPIO_MALLOC_FAILED())
    {
        if (IS_GPIO_ALL_OPEN())
        {
            for (; pin_index < total_pin_num; pin_index++)
            {
                if (PIN_FLAG_IS_OUTPUT_MODE(pin_index))
                {
                    hal_gpio_pm_store_config_output_mode(pin_index, pin_index);
                }

                if (PIN_FLAG_IS_NEED_STORE(pin_index))
                {
                    hal_gpio_pm_store_config_wake_up(pin_index, pin_index);
                }
            }
        }
        else
        {
            for (; flag_index < total_pin_num; flag_index++)
            {
                pin_index = pin_table[flag_index];

                if (PIN_FLAG_IS_OUTPUT_MODE(flag_index))
                {
                    hal_gpio_pm_store_config_output_mode(pin_index, flag_index);
                }

                if (PIN_FLAG_IS_NEED_STORE(flag_index))
                {
                    hal_gpio_pm_store_config_wake_up(pin_index, flag_index);
                }
            }
        }
    }

    if (gpio_a_store_buf != NULL)
    {
        GPIO_DLPSEnter((void *)GPIOA, gpio_a_store_buf);
    }

#ifdef GPIOB
    if (gpio_b_store_buf != NULL)
    {
        GPIO_DLPSEnter((void *)GPIOB, gpio_b_store_buf);
    }
#endif

#ifdef GPIOC
    if (gpio_c_store_buf != NULL)
    {
        GPIO_DLPSEnter((void *)GPIOC, gpio_c_store_buf);
    }
#endif

}

static void hal_gpio_pm_restore_output(void)
{
    if (gpio_a_store_buf != NULL)
    {
        GPIO_DLPSExitOutputMode((void *)GPIOA, gpio_a_store_buf);
    }

#ifdef GPIOB
    if (gpio_b_store_buf != NULL)
    {
        GPIO_DLPSExitOutputMode((void *)GPIOB, gpio_b_store_buf);
    }
#endif

#ifdef GPIOC
    if (gpio_c_store_buf != NULL)
    {
        GPIO_DLPSExitOutputMode((void *)GPIOC, gpio_c_store_buf);
    }
#endif
}

static void hal_gpio_pm_restore_input(void)
{
    if (gpio_a_store_buf != NULL)
    {
        GPIO_DLPSExitInputMode((void *)GPIOA, gpio_a_store_buf);
    }

#ifdef GPIOB
    if (gpio_b_store_buf != NULL)
    {
        GPIO_DLPSExitInputMode((void *)GPIOB, gpio_b_store_buf);
    }
#endif

#ifdef GPIOC
    if (gpio_c_store_buf != NULL)
    {
        GPIO_DLPSExitInputMode((void *)GPIOC, gpio_c_store_buf);
    }
#endif
}

static void hal_gpio_pm_regsiter(void)
{
    io_driver_dlps_register_enter_cb(hal_gpio_pm_store);
    io_driver_dlps_register_pre_clock_exit_cb(hal_gpio_pm_restore_output);
    io_driver_dlps_register_exit_cb(hal_gpio_pm_restore_input);
}


void hal_gpio_init(void)
{
    if (hal_gpio_init_flag)
    {
        return;
    }

    hal_gpio_init_flag = true;

    hal_gpio_init_flag_mem();

    RCC_PeriphClockCmd(APBPeriph_GPIOA, APBPeriph_GPIOA_CLOCK, ENABLE);

#ifdef GPIOB
    RCC_PeriphClockCmd(APBPeriph_GPIOB, APBPeriph_GPIOB_CLOCK, ENABLE);
#endif
#ifdef GPIOC
    RCC_PeriphClockCmd(APBPeriph_GPIOC, APBPeriph_GPIOC_CLOCK, ENABLE);
#endif

    hal_gpio_pm_regsiter();
}

void hal_gpio_deinit(void)
{
    hal_gpio_init_flag = false;

    RCC_PeriphClockCmd(APBPeriph_GPIOA, APBPeriph_GPIOA_CLOCK, DISABLE);
    if (gpio_a_store_buf != NULL)
    {
        os_mem_free(gpio_a_store_buf);
        gpio_a_store_buf = NULL;
    }
#ifdef GPIOB
    RCC_PeriphClockCmd(APBPeriph_GPIOB, APBPeriph_GPIOB_CLOCK, DISABLE);
    if (gpio_b_store_buf != NULL)
    {
        os_mem_free(gpio_b_store_buf);
        gpio_b_store_buf = NULL;
    }
#endif
#ifdef GPIOC
    RCC_PeriphClockCmd(APBPeriph_GPIOC, APBPeriph_GPIOC_CLOCK, DISABLE);
    if (gpio_c_store_buf != NULL)
    {
        os_mem_free(gpio_c_store_buf);
        gpio_c_store_buf = NULL;
    }
#endif
    if (hal_gpio_sw_context.pin_flags != NULL)
    {
        free(hal_gpio_sw_context.pin_flags);
        hal_gpio_sw_context.pin_flags = NULL;
    }

    hal_gpio_mem_init_flag = false;

    hal_gpio_int_deinit();
}

