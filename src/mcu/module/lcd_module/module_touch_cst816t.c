/**
*****************************************************************************************
*     Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file    module_touch_cst816t.h
* @brief   This file provides touch pad functions
* @author
* @date    2023-09-15
* @version v1.0
* *************************************************************************************
*/
#include "app_gui.h"
#if (TARGET_TOUCH_DEVICE == TOUCH_DEVICE_CS816T)
#include <stddef.h>
#include "module_touch_cst816t.h"
#include "os_timer.h"
#include "platform_utils.h"
#include "hal_i2c.h"
#include "hal_gpio.h"
#include "hal_gpio_int.h"
#include "trace.h"

#define CST816T_I2C_ID                            HAL_I2C_ID_1
#define TOUCH_SLAVE_ADDRESS                       0x15

typedef struct _t_touch_pin
{
    uint8_t touch_i2c_scl;
    uint8_t touch_i2c_sda;
    uint8_t touch_int;
    uint8_t touch_rst;
} T_TOUCH_PIN;

static void touch_int_handler(uint32_t context);
static void touch_gesture_release_timeout(void *pxTimer);
static bool touch_get_chip_id(uint8_t *p_chip_id);

static void *touch_gesture_release_timer = NULL;
static TOUCH_DATA cur_point;
static void (*touch_indicate)(void *);
static void *touch_context;
static T_TOUCH_PIN touch_pin;

void touch_pin_config(uint8_t touch_i2c_scl, uint8_t touch_i2c_sda, uint8_t touch_int,
                      uint8_t touch_rst)
{
    touch_pin.touch_i2c_scl = touch_i2c_scl;
    touch_pin.touch_i2c_sda = touch_i2c_sda;
    touch_pin.touch_int     = touch_int;
    touch_pin.touch_rst     = touch_rst;
}

static void touch_i2c_init(void)
{
    hal_i2c_init_pin(CST816T_I2C_ID, touch_pin.touch_i2c_scl, touch_pin.touch_i2c_sda);

    T_I2C_MASTER_CONFIG i2c_config;
    i2c_config.addr_mode = I2C_ADDR_MODE_7_BITS;
    i2c_config.i2c_speed = 400000;
    i2c_config.lock_type = I2C_LOCK_DISABLE;
    i2c_config.target_slave_addr = TOUCH_SLAVE_ADDRESS;
    hal_i2c_init_master_mode(CST816T_I2C_ID, &i2c_config);
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

bool touch_driver_init(void)
{
    uint8_t chip_id[4];
    touch_reset_init();
    touch_i2c_init();

    hal_gpio_set_level(touch_pin.touch_rst, GPIO_LEVEL_LOW);
    platform_delay_ms(10);
    hal_gpio_set_level(touch_pin.touch_rst, GPIO_LEVEL_HIGH);
    platform_delay_ms(120);

    if (touch_get_chip_id(chip_id) == false)
    {
        APP_PRINT_ERROR0("touch_driver_init: touch initialization failed!");
        return false;
    }

    touch_interrupt_init();
    if (touch_gesture_release_timer == NULL)
    {
        os_timer_create(&touch_gesture_release_timer, "touch gesture release timer", 1, 20, false,
                        touch_gesture_release_timeout);
    }

    hal_gpio_irq_enable(touch_pin.touch_int);
    return true;
}

bool touch_write(uint8_t reg, uint8_t data)
{
    T_I2C_STATUS res = hal_i2c_master_write_reg(CST816T_I2C_ID, TOUCH_SLAVE_ADDRESS, reg, data);
    if (res != I2C_STATUS_OK)
    {
        APP_PRINT_ERROR1("touch_write: master write fail %d", res);
        return false;
    }
    return true;
}

bool touch_read(uint8_t reg, uint8_t *p_data, uint8_t len)
{
    T_I2C_STATUS res;
    res = hal_i2c_master_write(CST816T_I2C_ID, TOUCH_SLAVE_ADDRESS, &reg, 1);
    if (res != I2C_STATUS_OK)
    {
        APP_PRINT_ERROR1("touch_read: master write fail %d", res);
        return false;
    }
    platform_delay_us(1);

    res = hal_i2c_master_read(CST816T_I2C_ID, TOUCH_SLAVE_ADDRESS, p_data, len);
    if (res != I2C_STATUS_OK)
    {
        APP_PRINT_ERROR1("touch_read: master read fail %d", res);
        return false;
    }

    return true;
}

bool touch_read_key_value(TOUCH_DATA *p_touch_data)
{
    uint8_t point_num;
    touch_read(0x02, &point_num, 1);

    // Only support single point. Normally, point_num can only be 0 or 1.
    if (cur_point.get_point == 0)
    {
        cur_point.get_point = point_num ? 1 : 0;
    }

    if (point_num == 0)
    {
        return false;
    }

    uint8_t buf[4];
    touch_read(0x03, buf, 4);
    cur_point.is_press = true;
    cur_point.x = buf[1] | ((buf[0] & 0xf) << 8);
    cur_point.y = buf[3] | ((buf[2] & 0xf) << 8);
    if (cur_point.x > LCD_WIDTH || cur_point.y > LCD_HIGHT)
    {
        return false;
    }

    // Change origin of coordinate.
    cur_point.x = LCD_WIDTH - cur_point.x;
    cur_point.y = LCD_HIGHT - cur_point.y;
    APP_PRINT_INFO2("touch_read_key_value: x = %d, y = %d", cur_point.x, cur_point.y);

    p_touch_data->x =  cur_point.x;
    p_touch_data->y =  cur_point.y;
    p_touch_data->t = cur_point.t;
    p_touch_data->is_press = cur_point.is_press;

    return true;
}

static bool touch_get_chip_id(uint8_t *p_chip_id)
{
    if (touch_read(0xa7, p_chip_id, 4) == true)
    {
        APP_PRINT_INFO1("touch_get_chip_id: %b", TRACE_BINARY(4, p_chip_id));
        return true;
    }
    return false;
}

static void touch_int_handler(uint32_t context)
{
    cur_point.t++;

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
    cur_point.get_point = 0;
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
