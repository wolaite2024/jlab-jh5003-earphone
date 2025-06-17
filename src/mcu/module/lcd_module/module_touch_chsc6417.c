/**
*****************************************************************************************
*     Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file    module_touch_chsc6417.h
* @brief   This file provides touch pad functions
* @author
* @date
* @version
* *************************************************************************************
*/
#include "app_gui.h"
#if (TARGET_TOUCH_DEVICE == TOUCH_DEVICE_CHSC6417)
#include <stddef.h>
#include "module_touch_chsc6417.h"
#include "os_timer.h"
#include "platform_utils.h"
#include "hal_gpio.h"
#include "hal_gpio_int.h"
#include "hal_i2c.h"
#include "trace.h"

#define CHSC6417_I2C_ID                           HAL_I2C_ID_1
#define CHSC6417_ADDRESS                          0x2e

typedef struct _t_touch_pin
{
    uint8_t touch_i2c_scl;
    uint8_t touch_i2c_sda;
    uint8_t touch_int;
    uint8_t touch_rst;
} T_TOUCH_PIN;

static void touch_int_handler(uint32_t context);
static void touch_gesture_release_timeout(void *pxTimer);
static bool touch_get_chip_id(void);
bool touch_write(uint8_t *p_data, uint8_t len);
bool touch_read(uint8_t *p_data, uint8_t len);

static void *touch_gesture_release_timer = NULL;
static TOUCH_DATA cur_point;
static void (*touch_indicate)(void *);
static void *touch_context;
static T_TOUCH_PIN touch_pin;
static uint8_t read_buf[3] = {0, 0, 0};

void touch_pin_config(uint8_t touch_i2c_scl, uint8_t touch_i2c_sda, uint8_t touch_int,
                      uint8_t touch_rst)
{
    touch_pin.touch_i2c_scl = touch_i2c_scl;
    touch_pin.touch_i2c_sda = touch_i2c_sda;
    touch_pin.touch_int     = touch_int;
    touch_pin.touch_rst     = touch_rst;
}

static void touch_reset_init(void)
{
    hal_gpio_init();
    hal_gpio_init_pin(touch_pin.touch_rst, GPIO_TYPE_AON, GPIO_DIR_OUTPUT, GPIO_PULL_UP);
    hal_gpio_set_level(touch_pin.touch_rst, GPIO_LEVEL_HIGH);
}

static void touch_interrupt_init(void)
{
    hal_gpio_int_init();
    hal_gpio_init_pin(touch_pin.touch_int, GPIO_TYPE_AUTO, GPIO_DIR_INPUT, GPIO_PULL_UP);
    hal_gpio_set_up_irq(touch_pin.touch_int, GPIO_IRQ_EDGE, GPIO_IRQ_ACTIVE_LOW, false);
    hal_gpio_register_isr_callback(touch_pin.touch_int, touch_int_handler, 0);

    hal_gpio_irq_disable(touch_pin.touch_int);
}

static void touch_i2c_init(void)
{
    hal_i2c_init_pin(CHSC6417_I2C_ID, touch_pin.touch_i2c_scl, touch_pin.touch_i2c_sda);

    T_I2C_MASTER_CONFIG i2c_config;

    i2c_config.addr_mode = I2C_ADDR_MODE_7_BITS;
    i2c_config.i2c_speed = 400000;
    i2c_config.lock_type = I2C_LOCK_DISABLE;
    i2c_config.target_slave_addr = CHSC6417_ADDRESS;
    hal_i2c_init_master_mode(CHSC6417_I2C_ID, &i2c_config);
}

bool touch_driver_init(void)
{
    touch_reset_init();
    touch_i2c_init();

    if (touch_get_chip_id() == false)
    {
        APP_PRINT_ERROR0("touch_driver_init: touch initialization failed!");
        return false;
    }

    hal_gpio_set_level(touch_pin.touch_rst, GPIO_LEVEL_LOW);
    platform_delay_ms(30);
    hal_gpio_set_level(touch_pin.touch_rst, GPIO_LEVEL_HIGH);
    platform_delay_ms(30);

    touch_interrupt_init();
    if (touch_gesture_release_timer == NULL)
    {
        os_timer_create(&touch_gesture_release_timer, "touch gesture release timer", 1, 20, false,
                        touch_gesture_release_timeout);
    }

    hal_gpio_irq_enable(touch_pin.touch_int);
    return true;
}

bool touch_write(uint8_t *p_data, uint8_t len)
{
    T_I2C_STATUS res = hal_i2c_master_write(CHSC6417_I2C_ID, CHSC6417_ADDRESS, p_data, len);
    if (res != I2C_STATUS_OK)
    {
        APP_PRINT_ERROR1("touch_write: master write fail %d", res);
        return false;
    }
    return true;
}

bool touch_read(uint8_t *p_data, uint8_t len)
{
    T_I2C_STATUS res = hal_i2c_master_read(CHSC6417_I2C_ID, CHSC6417_ADDRESS, p_data, len);
    if (res != I2C_STATUS_OK)
    {
        APP_PRINT_ERROR1("touch_read: master read fail error %d", res);
        return false;
    }
    return true;
}

bool touch_read_key_value(TOUCH_DATA *p_touch_data)
{
    if (((read_buf[0] >> 4) & 0x03) == 2)
    {
        cur_point.is_press = true;
    }
    else
    {
        cur_point.is_press = false;
    }

    cur_point.x = read_buf[1] | (((read_buf[0] & 0x40) >> 6) << 8);
    cur_point.y = read_buf[2] | (((read_buf[0] & 0x80) >> 7) << 8);
    if (cur_point.x > LCD_WIDTH || cur_point.y > LCD_HIGHT)
    {
        return false;
    }
#if FB_DIRECTION_ROTATE
    p_touch_data->x =  LCD_WIDTH - cur_point.y;
    p_touch_data->y =  cur_point.x;
#else
    p_touch_data->x =  cur_point.x;
    p_touch_data->y =  cur_point.y;
#endif
    p_touch_data->t = cur_point.t;
    p_touch_data->is_press = cur_point.is_press;

    return true;
}

static bool touch_get_chip_id(void)
{
    uint8_t chip_id[2] = {0, 0};
    uint8_t I2C_WriteBuf[8] = {0x42, 0xbd, 0x28, 0x35, 0xc1, 0x00, 0x35, 0xae};

    hal_gpio_set_level(touch_pin.touch_rst, GPIO_LEVEL_HIGH);
    platform_delay_ms(30);
    if (touch_write(I2C_WriteBuf, 8) == false)
    {
        return false;
    }
    platform_delay_ms(20);
    I2C_WriteBuf[0] = 0x9e;
    I2C_WriteBuf[1] = 0x6a;
    if (touch_write(I2C_WriteBuf, 2) == false)
    {
        return false;
    }
    if (touch_read(chip_id, 2) == false)
    {
        return false;
    }
    APP_PRINT_INFO2("touch_get_chip_id: 0x%x 0x%x", chip_id[0], chip_id[1]);
    return true;
}

static void touch_int_handler(uint32_t context)
{
    cur_point.t++;
    touch_read(read_buf, 3);
    os_timer_restart(&touch_gesture_release_timer, 30);
}

void touch_int_config(bool is_enable)
{
    if (is_enable == true)
    {
        hal_gpio_irq_enable(touch_pin.touch_int);
    }
    else
    {
        hal_gpio_irq_disable(touch_pin.touch_int);
    }
}

void touch_gesture_process_timeout(void)
{
//    APP_PRINT_INFO1("touch gesture release timeout t=%d", cur_point.t);
    cur_point.is_press = false;

    cur_point.t = 0;
    cur_point.x = 0;
    cur_point.y = 0;
}

static void touch_gesture_release_timeout(void *pxTimer)
{
    touch_gesture_process_timeout();
    if (touch_indicate)
    {
        touch_indicate(touch_context);
    }
}

void touch_register_irq_callback(void (*indicate)(void *), void *context)
{
    touch_indicate = indicate;
    touch_context = context;
}
#endif
