/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     hal_gpio_int.c
* @brief    This file provides all the gpio hal interrupt functions.
* @details
* @author
* @date
* @version  v0.1
*********************************************************************************************************
*/
#include <stdint.h>
#include "rtl876x.h"
#include "rtl876x_gpio.h"
#include "rtl876x_nvic.h"
#include "trace.h"
#include "vector_table.h"
#include "section.h"
#include "hal_gpio.h"
#include "hal_gpio_int.h"
#include "hal_pinmux.h"

#define TOTAL_HAL_GPIO_NUM                          (81)
#define TOTAL_HAL_GPIO_PORT_NUM                     (32)
#define TOTAL_HAL_PIN_NUM                           TOTAL_PIN_NUM

//#define PRINT_GPIO_LOGS
#ifdef PRINT_GPIO_LOGS
#define GPIO_PRINT_INFO(fmt, ...)                   DBG_DIRECT(fmt, __VA_ARGS__)
#else
#define GPIO_PRINT_INFO(...)
#endif

#define IS_GPIO_INVALID(gpio_num)                   ((gpio_num == 0xff) || (gpio_num >= TOTAL_HAL_GPIO_NUM))

typedef struct t_gpio_state
{
    uint32_t context[TOTAL_HAL_GPIO_NUM];
    P_GPIO_CBACK gpio_a0_callback;
    P_GPIO_CBACK gpio_a1_callback;  //other GPIOs use sub irqs for callback
} T_GPIO_HANDLE;

extern VECTORn_Type gpio_get_vector_number(uint8_t p_gpio_num);
extern IRQn_Type gpio_get_irq_number(uint8_t p_gpio_num);
extern T_GPIO_TYPE hal_gpio_get_type(uint8_t pin_index);

static T_GPIO_HANDLE hal_gpio_sw_context;

static GPIO_TypeDef *hal_get_gpio(uint8_t gpio_num)
{
    if (gpio_num < 32)
    {
        return GPIOA;
    }
    else if (gpio_num < 64)
    {
        return GPIOB;
    }
    else
    {
        return GPIOC;
    }
}

static IRQn_Type hal_gpio_get_irq_num(uint8_t p_gpio_num)
{
    if (p_gpio_num < 64)
    {
        return  gpio_get_irq_number(p_gpio_num);
    }
    else if (p_gpio_num < 72)
    {
        return GPIO_C_0to7_IRQn;
    }
    else if (p_gpio_num < 80)
    {
        return GPIO_C_8to15_IRQn;
    }
    else //p_gpio_num = 80
    {
        return GPIO_C_16_IRQn;
    }
}

static void hal_gpio_enable_nvic(uint8_t gpio_num, uint8_t proirity)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = hal_gpio_get_irq_num(gpio_num);
    NVIC_InitStruct.NVIC_IRQChannelPriority = proirity;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

bool hal_gpio_update_pin_table(void)
{
    return true;
}

static bool hal_gpio_register_isr_by_gpio(uint8_t gpio_num, P_GPIO_CBACK callback, uint32_t context)
{
    GPIO_TypeDef *p_gpio = hal_get_gpio(gpio_num);

    if (IS_GPIO_INVALID(gpio_num))
    {
        return false;
    }

    if (gpio_num == GPIO0)
    {
        hal_gpio_sw_context.gpio_a0_callback = callback;
    }
    else if (gpio_num == GPIO1)
    {
        hal_gpio_sw_context.gpio_a1_callback = callback;
    }
    else
    {
        RamVectorTableUpdate(gpio_get_vector_number(gpio_num), (IRQ_Fun)callback);
    }

    hal_gpio_enable_nvic(gpio_num, 4);

    hal_gpio_sw_context.context[gpio_num] = context;

    return true;
}

static bool hal_gpio_get_isr_callback_by_gpio(uint8_t gpio_num, P_GPIO_CBACK *callback,
                                              uint32_t *context)
{
    GPIO_TypeDef *p_gpio = hal_get_gpio(gpio_num);

    if (IS_GPIO_INVALID(gpio_num))
    {
        return false;
    }

    IRQ_Fun *Vectors = (IRQ_Fun *)SCB->VTOR;

    if (gpio_num == GPIO0)
    {
        *callback = hal_gpio_sw_context.gpio_a0_callback;
    }
    else if (gpio_num == GPIO1)
    {
        *callback = hal_gpio_sw_context.gpio_a1_callback ;
    }
    else
    {
        *callback = (P_GPIO_CBACK)Vectors[GPIOA2_VECTORn - GPIO2 + gpio_num];
    }

    *context = hal_gpio_sw_context.context[gpio_num];

    return true;
}

static ISR_TEXT_SECTION void imp_gpio_handler(GPIO_TypeDef *GPIOx, uint8_t gpio_num,
                                              uint32_t gpio_pin,
                                              P_GPIO_CBACK callback)
{
    GPIOx_ClearINTPendingBit(GPIOx, gpio_pin);
    if (callback)
    {
        callback(hal_gpio_sw_context.context[gpio_num]);
    }
    else
    {
        IO_PRINT_ERROR1("empty isr callback for gpioa%u", gpio_num);
    }
}

static ISR_TEXT_SECTION void gpio_a_sub_isr(uint32_t real_interrupt_status, int32_t vector_index)
{
    IRQ_Fun *Vectors = (IRQ_Fun *)SCB->VTOR;
    uint8_t idx;

    while (real_interrupt_status)
    {
        idx = 31 - __builtin_clz(real_interrupt_status);
        imp_gpio_handler(GPIOA, idx, BIT(idx), (P_GPIO_CBACK)Vectors[vector_index + idx]);
        real_interrupt_status &= ~BIT(idx);
    }
}

static ISR_TEXT_SECTION void gpio_b_sub_isr(uint32_t real_interrupt_status, int32_t vector_index)
{
    IRQ_Fun *Vectors = (IRQ_Fun *)SCB->VTOR;
    uint8_t idx;

    while (real_interrupt_status)
    {
        idx = 31 - __builtin_clz(real_interrupt_status);
        imp_gpio_handler(GPIOB, (idx + 32), BIT(idx), (P_GPIO_CBACK)Vectors[vector_index + idx]);
        real_interrupt_status &= ~BIT(idx);
    }
}

static ISR_TEXT_SECTION void gpio_c_sub_isr(uint32_t real_interrupt_status, int32_t vector_index)
{
    IRQ_Fun *Vectors = (IRQ_Fun *)SCB->VTOR;
    uint8_t idx;

    while (real_interrupt_status)
    {
        idx = 31 - __builtin_clz(real_interrupt_status);
        imp_gpio_handler(GPIOC, (idx + 64), BIT(idx), (P_GPIO_CBACK)Vectors[vector_index + idx]);
        real_interrupt_status &= ~BIT(idx);
    }
}

#define GPIOA_Handler(mask) gpio_a_sub_isr(GPIOA->INTSTATUS & mask, GPIOA2_VECTORn - 2)
#define GPIOB_Handler(mask) gpio_b_sub_isr(GPIOB->INTSTATUS & mask, GPIOB0_VECTORn)
#define GPIOC_Handler(mask) gpio_c_sub_isr(GPIOC->INTSTATUS & mask, GPIOC0_VECTORn)

ISR_TEXT_SECTION void GPIOA0_Handler(void)
{
    imp_gpio_handler(GPIOA, 0, BIT0, hal_gpio_sw_context.gpio_a0_callback);
}

ISR_TEXT_SECTION void GPIOA1_Handler(void)
{
    imp_gpio_handler(GPIOA, 1, BIT1, hal_gpio_sw_context.gpio_a1_callback);
}

ISR_TEXT_SECTION void GPIO_A_9_Handler(void)
{
    GPIOA_Handler(0x200);
}

ISR_TEXT_SECTION void GPIO_A_16_Handler(void)
{
    GPIOA_Handler(0x10000);
}

static ISR_TEXT_SECTION void GPIO_A_2_7_Handler(void)
{
    GPIOA_Handler(0xFC);
}

static ISR_TEXT_SECTION void GPIO_A_8_15_Handler(void)
{
    GPIOA_Handler(0xFF00);
}

static ISR_TEXT_SECTION void GPIO_A_17_23_Handler(void)
{
    GPIOA_Handler(0xFE0000);
}

static ISR_TEXT_SECTION void GPIO_A_24_31_Handler(void)
{
    GPIOA_Handler(0xFF000000);
}

static ISR_TEXT_SECTION void GPIO_B_0_7_Handler(void)
{
    GPIOB_Handler(0xFF);
}

static ISR_TEXT_SECTION void GPIO_B_8_15_Handler(void)
{
    GPIOB_Handler(0xFF00);
}

static ISR_TEXT_SECTION void GPIO_B_16_23_Handler(void)
{
    GPIOB_Handler(0xFF0000);
}

static ISR_TEXT_SECTION void GPIO_B_24_31_Handler(void)
{
    GPIOB_Handler(0xFF000000);
}

static ISR_TEXT_SECTION void GPIO_C_0_7_Handler(void)
{
    GPIOC_Handler(0xFF);
}

static ISR_TEXT_SECTION void GPIO_C_8_15_Handler(void)
{
    GPIOC_Handler(0xFF00);
}

static ISR_TEXT_SECTION void GPIO_C_16_Handler(void)
{
    GPIOC_Handler(0x10000);
}

bool hal_gpio_register_isr_callback(uint8_t pin_index, P_GPIO_CBACK callback, uint32_t context)
{
    if (hal_gpio_get_type(pin_index) == GPIO_TYPE_AUTO)
    {
        hal_pad_register_pin_wake_callback(pin_index, (P_PIN_WAKEUP_CALLBACK)callback, context);
    }

    return hal_gpio_register_isr_by_gpio(GPIO_GetNum(pin_index), callback, context);
}

bool hal_gpio_get_isr_callback(uint8_t pin_index, P_GPIO_CBACK *callback, uint32_t *context)
{
    return hal_gpio_get_isr_callback_by_gpio(GPIO_GetNum(pin_index), callback, context);
}

void hal_gpio_int_deinit(void)
{

}

void hal_gpio_int_init(void)
{
    RamVectorTableUpdate(GPIO_A0_VECTORn,         GPIOA0_Handler);
    RamVectorTableUpdate(GPIO_A1_VECTORn,         GPIOA1_Handler);
    RamVectorTableUpdate(GPIO_A_9_VECTORn,        GPIO_A_9_Handler);
    RamVectorTableUpdate(GPIO_A_16_VECTORn,       GPIO_A_16_Handler);
    RamVectorTableUpdate(GPIO_A_2_7_VECTORn,      GPIO_A_2_7_Handler);
    RamVectorTableUpdate(GPIO_A_8_10to15_VECTORn, GPIO_A_8_15_Handler);
    RamVectorTableUpdate(GPIO_A_17to23_VECTORn,   GPIO_A_17_23_Handler);
    RamVectorTableUpdate(GPIO_A_24to31_VECTORn,   GPIO_A_24_31_Handler);
    RamVectorTableUpdate(GPIO_B_0to7_VECTORn,     GPIO_B_0_7_Handler);
    RamVectorTableUpdate(GPIO_B_8to15_VECTORn,    GPIO_B_8_15_Handler);
    RamVectorTableUpdate(GPIO_B_16to23_VECTORn,   GPIO_B_16_23_Handler);
    RamVectorTableUpdate(GPIO_B_24to31_VECTORn,   GPIO_B_24_31_Handler);
    RamVectorTableUpdate(GPIO_C_0to7_VECTORn,     GPIO_C_0_7_Handler);
    RamVectorTableUpdate(GPIO_C_8to15_VECTORn,    GPIO_C_8_15_Handler);
    RamVectorTableUpdate(GPIO_C_16_VECTORn,       GPIO_C_16_Handler);
}
