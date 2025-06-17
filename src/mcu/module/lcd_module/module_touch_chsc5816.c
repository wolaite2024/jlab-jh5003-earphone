/**
*****************************************************************************************
*     Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file    module_touch_chsc5816.h
* @brief   This file provides touch pad functions
* @author
* @date
* @version
* *************************************************************************************
*/
#include "app_gui.h"
#if (TARGET_TOUCH_DEVICE == TOUCH_DEVICE_CHSC5816)
#include <stddef.h>
#include "module_touch_chsc5816.h"
#include "os_timer.h"
#include "hal_i2c.h"
#include "platform_utils.h"
#include "hal_gpio.h"
#include "hal_gpio_int.h"
#include "trace.h"

#define CHSC5816_I2C_ID                           HAL_I2C_ID_1
#define CHSC5816_ADDRESS                          0x2e

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
bool touch_read(uint32_t reg_addr, uint8_t *p_data, uint8_t len);

static void *touch_gesture_release_timer = NULL;
static TOUCH_DATA cur_point;
static void (*touch_indicate)(void *);
static void *touch_context;
static T_TOUCH_PIN touch_pin;
static uint8_t read_buf[8] = {0, 0, 0};

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
    hal_i2c_init_pin(CHSC5816_I2C_ID, touch_pin.touch_i2c_scl, touch_pin.touch_i2c_sda);

    T_I2C_MASTER_CONFIG i2c_config;

    i2c_config.addr_mode = I2C_ADDR_MODE_7_BITS;
    i2c_config.i2c_speed = 400000;
    i2c_config.lock_type = I2C_LOCK_DISABLE;
    i2c_config.target_slave_addr = CHSC5816_ADDRESS;
    hal_i2c_init_master_mode(CHSC5816_I2C_ID, &i2c_config);
}

bool touch_driver_init(void)
{
    touch_reset_init();
    touch_i2c_init();

    hal_gpio_set_level(touch_pin.touch_rst, GPIO_LEVEL_LOW);
    platform_delay_ms(10);
    hal_gpio_set_level(touch_pin.touch_rst, GPIO_LEVEL_HIGH);
    platform_delay_ms(150);

    if (touch_get_chip_id() == false)
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

bool touch_read(uint32_t reg_addr, uint8_t *p_data, uint8_t len)
{
    uint8_t write_buf[4] = {0, 0, 0, 0};
    T_I2C_STATUS status;

    write_buf[0] = (reg_addr & 0xFF000000) >> 24;
    write_buf[1] = (reg_addr & 0xFF0000) >> 16;
    write_buf[2] = (reg_addr & 0xFF00) >> 8;
    write_buf[3] = reg_addr & 0x00FF;

    status = hal_i2c_master_write(CHSC5816_I2C_ID, CHSC5816_ADDRESS, write_buf, 4);
    if (status != I2C_STATUS_OK)
    {
        APP_PRINT_ERROR1("touch_read: master write fail error %d", status);
        return false;
    }
    status = hal_i2c_master_read(CHSC5816_I2C_ID, CHSC5816_ADDRESS, p_data, len);
    if (status != I2C_STATUS_OK)
    {
        APP_PRINT_ERROR1("touch_read: master read fail error %d", status);
        return false;
    }

    return true;
}

bool touch_read_key_value(TOUCH_DATA *p_touch_data)
{
    if (read_buf[1])
    {
        cur_point.is_press = true;
    }
    else
    {
        cur_point.is_press = false;
    }

    cur_point.x = read_buf[2] | (((read_buf[5] & 0x0f)) << 8);
    cur_point.y = read_buf[3] | (((read_buf[5] & 0xf0) >> 4) << 8);
    if (cur_point.x > LCD_WIDTH || cur_point.y > LCD_HIGHT)
    {
        return false;
    }
    p_touch_data->x =  cur_point.x;
    p_touch_data->y =  cur_point.y;
    p_touch_data->t = cur_point.t;
    p_touch_data->is_press = cur_point.is_press;

    return true;
}

static bool touch_get_chip_id(void)
{
    uint8_t chip_id[16] = {0, 0, 0};

    if (touch_read(0x20000014, chip_id, 16) == false)
    {
        return false;
    }
    APP_PRINT_INFO3("touch_get_chip_id: 0x%x 0x%x 0x%x", chip_id[9], chip_id[10], chip_id[11]);
    return true;
}

static void touch_int_handler(uint32_t context)
{
    cur_point.t++;
    touch_read(0x2000002c, read_buf, 8);
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
