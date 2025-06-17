/*******************************************************************************
* Copyright (C) 2022 Realtek Semiconductor Corporation. All Rights Reserved.
*/
#include "app_buck_apw7564.h"
#include "hal_i2c.h"
#include "ext_buck.h"

#define APW7564_I2C_ID                               HAL_I2C_ID_0
#define APW7564_DEVICE_ADDR                          (0x64)

#define APW7564_REG_00                               (0x00)
#define APW7654_REG_01                               (0x01)
#define APW7654_REG_02                               (0x02)

//reg 01
#define APW7564_BITS_VO_SET                          (0x3F)

//reg 02
#define APW7564_BITS_ENABLE                          (0x01)
#define APW7564_BITS_DISCHARGE                       (0x02)
#define APW7564_BITS_TRS                             (0x0C)

#define APW7654_RAMP_SPEED_STEP_20US                 (0x00 << 2)
#define APW7564_RAMP_SPEED_STEP_10US                 (0x01 << 2)
#define APW7564_RAMP_SPEED_STEP_2US                  (0x02 << 2)
#define APW7564_RAMP_SPEED_STEP_1US                  (0x03 << 2)

static bool app_buck_apw7564_write_reg(uint8_t reg_addr, uint8_t data)
{
    if (hal_i2c_master_write_reg(APW7564_I2C_ID, APW7564_DEVICE_ADDR, reg_addr, data) != I2C_STATUS_OK)
    {
        return false;
    }

    return true;
}

static bool app_buck_apw7564_read_reg(uint8_t reg_addr, uint8_t *data, uint8_t len)
{
    if (hal_i2c_master_write_to_read(APW7564_I2C_ID, APW7564_DEVICE_ADDR, &reg_addr, 1, data,
                                     len) != I2C_STATUS_OK)
    {
        return false;
    }

    return true;
}

static bool app_buck_apw7564_update_bits(uint8_t reg_addr, uint8_t bitmask, uint8_t data)
{
    uint8_t reg_value = 0;

    app_buck_apw7564_read_reg(reg_addr, &reg_value, 1);

    reg_value = ((reg_value & (~bitmask)) | (data & bitmask));

    return app_buck_apw7564_write_reg(reg_addr, reg_value);
}

static bool app_buck_apw7564_set_vo(uint32_t volt_h_uv)
{
    if ((volt_h_uv < 5000) || (volt_h_uv > 113000))
    {
        return false;
    }

    uint16_t volt_offset = volt_h_uv - 5000;
    uint8_t cal_data = volt_offset / 100;

    return app_buck_apw7564_update_bits(APW7564_REG_00, APW7564_BITS_VO_SET, cal_data);
}

static bool app_buck_apw7564_vout_enable(bool enable)
{
    uint8_t data = enable ? APW7564_BITS_ENABLE : 0;

    return app_buck_apw7564_update_bits(APW7654_REG_01, APW7564_BITS_ENABLE, data);
}

static bool app_buck_apw7564_output_discharge(bool enable)
{
    uint8_t data = enable ? APW7564_BITS_DISCHARGE : 0;

    return app_buck_apw7564_update_bits(APW7654_REG_01, APW7564_BITS_DISCHARGE, data);
}

static bool app_buck_apw7564_set_voltage_ramp_speed(uint8_t speed)
{
    if ((speed & (~APW7564_BITS_TRS)) != 0)
    {
        return false;
    }

    return app_buck_apw7564_update_bits(APW7654_REG_01, APW7564_BITS_TRS, speed);
}

static bool app_buck_apw7564_hw_init(void)
{
    T_I2C_MASTER_CONFIG i2c_config;

    i2c_config.addr_mode = I2C_ADDR_MODE_7_BITS;
    i2c_config.i2c_speed = 400000;
    i2c_config.lock_type = I2C_LOCK_DISABLE;
    i2c_config.target_slave_addr = APW7564_DEVICE_ADDR;
    hal_i2c_init_master_mode(APW7564_I2C_ID, &i2c_config);

    uint8_t buck_reg_02 = 0;
    if (app_buck_apw7564_read_reg(APW7654_REG_02, &buck_reg_02, 1) == false)
    {
        return false;
    }

    app_buck_apw7564_set_voltage_ramp_speed(APW7654_RAMP_SPEED_STEP_20US);
    app_buck_apw7564_set_vo(9300);

    return true;
}

static bool app_buck_apw7564_enable(void)
{
    app_buck_apw7564_output_discharge(false);
    app_buck_apw7564_vout_enable(true);

    return true;
}

static bool app_buck_apw7564_disable(void)
{
    app_buck_apw7564_output_discharge(true);
    app_buck_apw7564_vout_enable(false);

    return true;
}

bool app_buck_apw7564_init(void)
{
    EXT_BUCK_T buck_ops;

    buck_ops.ext_buck_set_voltage = app_buck_apw7564_set_vo;
    buck_ops.ext_buck_enable = app_buck_apw7564_enable;
    buck_ops.ext_buck_disable = app_buck_apw7564_disable;
    ext_buck_init(&buck_ops);

    return app_buck_apw7564_hw_init();
}

