#ifndef _MODULE_GPIO_BUTTON_POLLING__
#define _MODULE_GPIO_BUTTON_POLLING__
#include "rtl876x_gpio.h"
#include "rtl876x_rcc.h"
#include "rtl876x_tim.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x.h"
#include "trace.h"

#define GPIO_BUTTON_APBPeriph                       APBPeriph_GPIOA
#define GPIO_BUTTON_APBPeriph_CLK                   APBPeriph_GPIOA_CLOCK
#define GPIO_BUTTON_GROUP                           GPIOA
#define GPIO_BUTTON_KEY                             P3_0
#define GPIO_BUTTON_HANDLER                         GPIOA23_Handler
#define GPIO_BUTTON_IRQ                             GPIO23_IRQn
#define GPIO_BUTTON_VECTORn                         GPIOA23_VECTORn

typedef enum
{
    GPIO_KEY_INVALID,
    GPIO_KEY_PRESSED,
    GPIO_KEY_RELEASED,
} T_GPIO_KEY_STATE;

typedef struct
{
    uint32_t key_id;
    T_GPIO_KEY_STATE current_state;
    uint32_t press_timestamp;
    uint32_t release_timestamp;
} T_GPIO_KEY;

void gpio_button_init(void);
void gpio_button_enter_dlps(void);
void gpio_button_exit_dlps(void);
T_GPIO_KEY gpio_button_read_key(void);

#endif //_MODULE_GPIO_BUTTON_POLLING__


