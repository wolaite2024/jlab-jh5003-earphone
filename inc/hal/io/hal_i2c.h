/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      hal_i2c.h
* @brief
* @details
* @author
* @date
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _HAL_I2C_
#define _HAL_I2C_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "hal_def.h"

/** @addtogroup HAL_I2C HAL I2C
  * @brief HAL I2C driver module.
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/


/** @defgroup HAL_I2C_Exported_Types HAL I2C Exported Types
  * @{
  */
typedef enum
{
    HAL_I2C_ID_0 = 0,
    HAL_I2C_ID_1 = 1,
#if (CHIP_I2C_NUM > 2)
    HAL_I2C_ID_2 = 2,
#endif
#if (CHIP_I2C_NUM > 3)
    HAL_I2C_ID_3 = 3,
#endif
    HAL_I2C_ID_MAX
} T_I2C_ID;

typedef enum
{
    I2C_STATUS_OK,
    I2C_STATUS_ARB_LOST,
    I2C_STATUS_ABRT_MASTER_DIS,
    I2C_STATUS_ABRT_TXDATA_NOACK,
    I2C_STATUS_ABRT_10ADDR2_NOACK,
    I2C_STATUS_ABRT_10ADDR1_NOACK,
    I2C_STATUS_ABRT_7B_ADDR_NOACK,
    I2C_STATUS_TIMEOUT,
    I2C_STATUS_ERROR_INVALID_PARAMETER = 0xF0,
    I2C_STATUS_ERROR_BUSY,
    I2C_STATUS_ERROR_NOT_INIT,
    I2C_STATUS_ERROR_DMA_NOT_INIT,
} T_I2C_STATUS;

typedef enum
{
    I2C_ADDR_MODE_7_BITS,
    I2C_ADDR_MODE_10_BITS
} T_I2C_ADDR_MODE;

typedef enum
{
    I2C_LOCK_DISABLE,
    I2C_LOCK_TYPE_MUTEX,
    I2C_LOCK_TYPE_SEMAPHORE,
    I2C_LOCK_MAX
} T_I2C_LOCK_TYPE;

typedef struct
{
    uint32_t          i2c_speed;
    uint16_t          target_slave_addr;
    T_I2C_ADDR_MODE   addr_mode;
    T_I2C_LOCK_TYPE   lock_type;
} T_I2C_MASTER_CONFIG;

typedef struct
{
    uint16_t          slave_addr;
    T_I2C_ADDR_MODE   addr_mode;
} T_I2C_SLAVE_CONFIG;

typedef union
{
    struct
    {
        uint32_t I2C_EVENT_RX_UNDER   : 1;
        uint32_t I2C_EVENT_RX_OVER    : 1;
        uint32_t I2C_EVENT_RX_FULL    : 1;
        uint32_t I2C_EVENT_TX_OVER    : 1;
        uint32_t I2C_EVENT_TX_EMPTY   : 1;
        uint32_t I2C_EVENT_RD_REQ     : 1;
        uint32_t I2C_EVENT_TX_ABRT    : 1;
        uint32_t I2C_EVENT_RX_DONE    : 1;
        uint32_t I2C_EVENT_ACTIVITY   : 1;
        uint32_t I2C_EVENT_STOP_DET   : 1;
        uint32_t I2C_EVENT_START_DET  : 1;
        uint32_t I2C_EVENT_GEN_CALL   : 1;
        uint32_t I2C_EVENT_DMA_TX_DONE: 1;
        uint32_t I2C_EVENT_RESERVED   : 19;
    };
    uint32_t event;
} T_HAL_I2C_EVENT;

/**
  * @brief  I2C callback definition.
  */
typedef void (*P_HAL_I2C_CALLBACK)(T_I2C_ID i2d_id, T_HAL_I2C_EVENT event);

/** End of group HAL_I2C_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/



/** @defgroup HAL_I2C_Exported_Functions HAL I2C Exported Functions
  * @{
  */

/**
 * hal_i2c.h
 *
 * \brief   Initialize a I2C to master mode with the specific structure.
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  i2c_id        The I2C index, please refer to @ref T_I2C_ID.
 *
 * \param[in]  i2c_config    The configuration structure to initialze I2C.
 *
 * \return                   The status of the I2C initialization.
 * \retval  I2C_STATUS_OK             The HAL_I2C is initialized successfully.
 * \retval  I2C_STATUS_ERROR_BUCY     The HAL_I2C is already initialized.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
        T_I2C_MASTER_CONFIG i2c_master_config;
        i2c_master_config.addr_mode = I2C_ADDR_MODE_7_BITS;
        i2c_master_config.i2c_speed = 400000;
        i2c_master_config.lock_type = I2C_LOCK_DISABLE;
        i2c_master_config.target_slave_addr = ADDR;
        hal_i2c_init_master_mode(DEMO_I2C_ID, &i2c_master_config);
 * }
 * \endcode
 *
 * \ingroup  HAL_I2C_Exported_Functions
 */
T_I2C_STATUS hal_i2c_init_master_mode(T_I2C_ID i2c_id, const T_I2C_MASTER_CONFIG *i2c_config);

/**
 * hal_i2c.h
 *
 * \brief   Initialize a I2C to slave mode with the specific structure.
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  i2c_id        The I2C index, please refer to @ref T_I2C_ID.
 *
 * \param[in]  i2c_config    The configuration structure to initialze I2C.
 *
 * \return                   The status of the I2C initialization.
 * \retval  I2C_STATUS_OK        The HAL_I2C is initialized successfully.
 * \retval  I2C_STATUS_ERROR_INVALID_PARAMETER     The HAL_I2C pin was failed to initialized due to invalid pin number.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
        T_I2C_SLAVE_CONFIG i2c_slave_config;
        i2c_slave_config.addr_mode  = I2C_ADDR_MODE_7_BITS;
        i2c_slave_config.slave_addr = ADDR;
        hal_i2c_init_slave_mode(DEMO_I2C_ID, &i2c_slave_config);
 * }
 * \endcode
 *
 * \ingroup  HAL_I2C_Exported_Functions
 */
T_I2C_STATUS hal_i2c_init_slave_mode(T_I2C_ID i2c_id, const T_I2C_SLAVE_CONFIG *i2c_config);

/**
 * hal_i2c.h
 *
 * \brief   Initialize a I2C SCL and SDA pins.
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  i2c_id        The I2C index, please refer to @ref T_I2C_ID.
 *
 * \param[in]  scl_pin       The SCL pin index, Please refer to 'pin_def.h'.
 *
 * \param[in]  sda_pin       The SDA pin index, Please refer to 'pin_def.h'.
 *
 * \return                   The status of the I2C initialization.
 * \retval  I2C_STATUS_OK        The HAL_I2C pin is configured successfully.
 * \retval  I2C_STATUS_ERROR_INVALID_PARAMETER     The HAL_I2C pin was failed to initialized due to invalid parameter.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int board_init(void)
 * {
        hal_i2c_init_pin(DEMO_I2C_ID, PIN_I2C_SCL, PIN_I2C_SDA);
 * }
 * \endcode
 *
 * \ingroup  HAL_I2C_Exported_Functions
 */
T_I2C_STATUS hal_i2c_init_pin(T_I2C_ID i2c_id, uint8_t scl_pin, uint8_t sda_pin);

/**
 * hal_i2c.h
 *
 * \brief   Initialize I2C TX DMA mode.
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  i2c_id        The I2C index, please refer to @ref T_I2C_ID.
 *
 * \param[in]  dma_channel_num       The DMA channel number, Please refer to 'hal_dma_internal.h'.
 *
 * \param[in]  cb            The callback when I2C Send DMA done and ISR triggered.
 *
 * \return                   The status of the I2C DMA initialization.
 * \retval  I2C_STATUS_OK        The HAL_I2C DMA is configured successfully.
 * \retval  I2C_STATUS_ERROR_INVALID_PARAMETER     The HAL_I2C DMA was failed to initialized due to invalid parameter.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
  void i2c_dma_callback(T_I2C_ID id, T_HAL_I2C_EVENT p_event)
  {
       APP_PRINT_TRACE2("i2c dma callback id %u, event %x", id, p_event.event);
  }
 *
 * int i2c_dma_init(void)
 * {
 *     hal_i2c_init_pin(DEMO_I2C_ID, PIN_I2C_SCL, PIN_I2C_SDA);
 *
 *     T_I2C_MASTER_CONFIG i2c_master_config;
 *     i2c_master_config.addr_mode = I2C_ADDR_MODE_7_BITS;
 *     i2c_master_config.i2c_speed = 400000;
 *     i2c_master_config.lock_type = I2C_LOCK_DISABLE;
 *     i2c_master_config.target_slave_addr = ADDR;
 *     hal_i2c_init_master_mode(DEMO_I2C_ID, &i2c_master_config);
 *
 *     if (!GDMA_channel_request(&i2c_tx_dma_ch_num, adc_dma_handler, true))
 *     {
 *         return;
 *     }
 *
 *     hal_i2c_enable_tx_dma_mode(DEMO_I2C_ID, i2c_tx_dma_ch_num, i2c_dma_callback);
 * }
 * \endcode
 *
 * \ingroup  HAL_I2C_Exported_Functions
 */
T_I2C_STATUS hal_i2c_enable_tx_dma_mode(T_I2C_ID i2c_id, uint8_t dma_channel_num,
                                        P_HAL_I2C_CALLBACK cb);

/**
 * hal_i2c.h
 *
 * \brief   Initialize I2C RX DMA mode.
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  i2c_id        The I2C index, please refer to @ref T_I2C_ID.
 *
 * \param[in]  dma_channel_num       The DMA channel number, Please refer to 'hal_dma_internal.h'.
 *
 * \param[in]  cb            The callback when I2C Receive DMA done and ISR triggered.
 *
 * \return                   The status of the I2C DMA initialization.
 * \retval  I2C_STATUS_OK        The HAL_I2C DMA is configured successfully.
 * \retval  I2C_STATUS_ERROR_INVALID_PARAMETER     The HAL_I2C DMA was failed to initialized due to invalid parameter.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
  void i2c_dma_callback(T_I2C_ID id, T_HAL_I2C_EVENT p_event)
  {
       APP_PRINT_TRACE2("i2c dma callback id %u, event %x", id, p_event.event);
  }
 *
 * int i2c_dma_init(void)
 * {
       hal_i2c_enable_rx_dma_mode(DEMO_I2C_ID, i2c_rx_dma_ch_num, i2c_dma_callback);
 * }
 * \endcode
 *
 * \ingroup  HAL_I2C_Exported_Functions
 */
T_I2C_STATUS hal_i2c_enable_rx_dma_mode(T_I2C_ID i2c_id, uint8_t dma_channel_num,
                                        P_HAL_I2C_CALLBACK cb);

/**
 * hal_i2c.h
 *
 * \brief   Set I2C clock speed.
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  i2c_id        The I2C index, please refer to @ref T_I2C_ID.
 *
 * \param[in]  clock_speed   The I2C clock speed in HZ to be set.
 *
 * \return                   The status of the I2C DMA initialization.
 * \retval  I2C_STATUS_OK        The HAL_I2C DMA is configured successfully.
 * \retval  I2C_STATUS_ERROR_INVALID_PARAMETER     The HAL_I2C DMA was failed to set speed due to invalid parameter.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int i2c_test(void)
 * {
       hal_i2c_master_set_speed(DEMO_I2C_ID, i2c_rx_dma_ch_num, 400000);
 * }
 * \endcode
 *
 * \ingroup  HAL_I2C_Exported_Functions
 */
T_I2C_STATUS hal_i2c_master_set_speed(T_I2C_ID i2c_id, uint32_t clock_speed);

/**
 * hal_i2c.h
 *
 * \brief   I2C master write data.
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  i2c_id        The I2C index, please refer to @ref T_I2C_ID.
 *
 * \param[in]  slave_addr    The target slave address to write to.
 *
 * \param[in]  write_buf     The byte data to be writen.
 *
 * \param[in]  write_len     The length for data buffer to be writen through I2C.
 *
 * \return                   The status of the I2C DMA initialization.
 * \retval  I2C_STATUS_OK        The HAL_I2C data is sent successfully.
 * \retval  I2C_STATUS_ARB_LOST:            Master or slave transmitter losed arbitration.
 * \retval  I2C_STATUS_ABRT_MASTER_DIS:     User tried to initiate a Master operation with the Master mode disabled.
 * \retval  I2C_STATUS_ABRT_TXDATA_NOACK:   Master sent data byte(s) following the address, but it did not receive an acknowledge from the remote slave.
 * \retval  I2C_STATUS_ABRT_10ADDR2_NOACK:  Master is in 10-bit address mode and the second address byte of the 10-bit address was not acknowledged by any slave.
 * \retval  I2C_STATUS_ABRT_10ADDR1_NOACK:  Master is in 10-bit address mode and the first 10-bit address byte was not acknowledged by any slave.
 * \retval  I2C_STATUS_ABRT_7B_ADDR_NOACK:  Master is in 7-bit address mode and the address sent was not acknowledged by any slave.
 * \retval  I2C_STATUS_TIMEOUT:                    I2C timeout.
 * \retval  I2C_STATUS_ERROR_INVALID_PARAMETER     The HAL_I2C was failed to send data due to invalid parameter.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int i2c_test(void)
 * {
        status = hal_i2c_master_write(DEMO_I2C_ID, ADDR, I2C_WriteBuf, 6);
 * }
 * \endcode
 *
 * \ingroup  HAL_I2C_Exported_Functions
 */
T_I2C_STATUS hal_i2c_master_write(T_I2C_ID i2c_id, uint16_t slave_addr, uint8_t *write_buf,
                                  uint32_t write_len);

/**
 * hal_i2c.h
 *
 * \brief   I2C master write data.
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  i2c_id        The I2C index, please refer to @ref T_I2C_ID.
 *
 * \param[in]  slave_addr    The target slave address to write to.
 *
 * \param[in]  reg_addr      The device register address to write to.
 *
 * \param[in]  reg_data      The byte data to be writen.
 *
 * \return                   The status of the I2C DMA initialization.
 * \retval  I2C_STATUS_OK        The HAL_I2C data is sent successfully.
 * \retval  I2C_STATUS_ARB_LOST:            Master or slave transmitter losed arbitration.
 * \retval  I2C_STATUS_ABRT_MASTER_DIS:     User tried to initiate a Master operation with the Master mode disabled.
 * \retval  I2C_STATUS_ABRT_TXDATA_NOACK:   Master sent data byte(s) following the address, but it did not receive an acknowledge from the remote slave.
 * \retval  I2C_STATUS_ABRT_10ADDR2_NOACK:  Master is in 10-bit address mode and the second address byte of the 10-bit address was not acknowledged by any slave.
 * \retval  I2C_STATUS_ABRT_10ADDR1_NOACK:  Master is in 10-bit address mode and the first 10-bit address byte was not acknowledged by any slave.
 * \retval  I2C_STATUS_ABRT_7B_ADDR_NOACK:  Master is in 7-bit address mode and the address sent was not acknowledged by any slave.
 * \retval  I2C_STATUS_TIMEOUT:                    I2C timeout.
 * \retval  I2C_STATUS_ERROR_INVALID_PARAMETER     The HAL_I2C was failed to send data due to invalid parameter.
 *
 * <b>Example usage</b>
 * \code{.c}
 * #define DEVICE_SLAVE_ADDR        (0xA0)
 * #define REG_ADDR                 (0x20)
 * int i2c_test(void)
 * {
        status = hal_i2c_master_write_reg(DEMO_I2C_ID, DEVICE_SLAVE_ADDR, REG_ADDR, 0xAA);
 * }
 * \endcode
 *
 * \ingroup  HAL_I2C_Exported_Functions
 */
T_I2C_STATUS hal_i2c_master_write_reg(T_I2C_ID i2c_id, uint16_t slave_addr, uint8_t reg_addr,
                                      uint8_t reg_data);


/**
 * hal_i2c.h
 *
 * \brief   I2C master write data.
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  i2c_id        The I2C index, please refer to @ref T_I2C_ID.
 *
 * \param[in]  slave_addr    The target slave address to write to.
 *
 * \param[in]  reg_addr      The device register address to write to.
 *
 * \param[in]  reg_data      The halfword data to be writen.
 *
 * \return                   The status of the I2C DMA initialization.
 * \retval  I2C_STATUS_OK        The HAL_I2C data is sent successfully.
 * \retval  I2C_STATUS_ARB_LOST:            Master or slave transmitter losed arbitration.
 * \retval  I2C_STATUS_ABRT_MASTER_DIS:     User tried to initiate a Master operation with the Master mode disabled.
 * \retval  I2C_STATUS_ABRT_TXDATA_NOACK:   Master sent data byte(s) following the address, but it did not receive an acknowledge from the remote slave.
 * \retval  I2C_STATUS_ABRT_10ADDR2_NOACK:  Master is in 10-bit address mode and the second address byte of the 10-bit address was not acknowledged by any slave.
 * \retval  I2C_STATUS_ABRT_10ADDR1_NOACK:  Master is in 10-bit address mode and the first 10-bit address byte was not acknowledged by any slave.
 * \retval  I2C_STATUS_ABRT_7B_ADDR_NOACK:  Master is in 7-bit address mode and the address sent was not acknowledged by any slave.
 * \retval  I2C_STATUS_TIMEOUT:                    I2C timeout.
 * \retval  I2C_STATUS_ERROR_INVALID_PARAMETER     The HAL_I2C was failed to send data due to invalid parameter.
 *
 * <b>Example usage</b>
 * \code{.c}
 * #define DEVICE_SLAVE_ADDR        (0xA0)
 * #define REG_ADDR                 (0x20)
 * int i2c_test(void)
 * {
        status = hal_i2c_master_write_8b_reg_16b(DEMO_I2C_ID, DEVICE_SLAVE_ADDR, REG_ADDR, 0xAABB);
 * }
 * \endcode
 *
 * \ingroup  HAL_I2C_Exported_Functions
 */
T_I2C_STATUS hal_i2c_master_write_8b_reg_16b(T_I2C_ID i2c_id, uint16_t slave_addr, uint8_t reg_addr,
                                             uint16_t reg_data);

/**
 * hal_i2c.h
 *
 * \brief   I2C master write data.
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  i2c_id        The I2C index, please refer to @ref T_I2C_ID.
 *
 * \param[in]  slave_addr    The target slave address to write to.
 *
 * \param[in]  reg_addr      The device register address to write to.
 *
 * \param[in]  write_buf     The byte data to be writen.
 *
 * \param[in]  write_len     The length for data buffer to be writen through I2C.
 *
 * \return                   The status of the I2C DMA initialization.
 * \retval  I2C_STATUS_OK        The HAL_I2C data is sent successfully.
 * \retval  I2C_STATUS_ARB_LOST:            Master or slave transmitter losed arbitration.
 * \retval  I2C_STATUS_ABRT_MASTER_DIS:     User tried to initiate a Master operation with the Master mode disabled.
 * \retval  I2C_STATUS_ABRT_TXDATA_NOACK:   Master sent data byte(s) following the address, but it did not receive an acknowledge from the remote slave.
 * \retval  I2C_STATUS_ABRT_10ADDR2_NOACK:  Master is in 10-bit address mode and the second address byte of the 10-bit address was not acknowledged by any slave.
 * \retval  I2C_STATUS_ABRT_10ADDR1_NOACK:  Master is in 10-bit address mode and the first 10-bit address byte was not acknowledged by any slave.
 * \retval  I2C_STATUS_ABRT_7B_ADDR_NOACK:  Master is in 7-bit address mode and the address sent was not acknowledged by any slave.
 * \retval  I2C_STATUS_TIMEOUT:                    I2C timeout.
 * \retval  I2C_STATUS_ERROR_INVALID_PARAMETER     The HAL_I2C was failed to send data due to invalid parameter.
 *
 * <b>Example usage</b>
 * \code{.c}
 * #define DEVICE_SLAVE_ADDR        (0xA0)
 * #define REG_ADDR                 (0x20)
 * int i2c_test(void)
 * {
        uint8_t reg_datas[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05}
        status = hal_i2c_master_write_regs(DEMO_I2C_ID, DEVICE_SLAVE_ADDR, REG_ADDR, reg_datas, 6);
 * }
 * \endcode
 *
 * \ingroup  HAL_I2C_Exported_Functions
 */
T_I2C_STATUS hal_i2c_master_write_regs(T_I2C_ID i2c_id, uint16_t slave_addr, uint8_t reg_addr,
                                       uint8_t *write_buf, uint32_t write_len);

/**
 * hal_i2c.h
 *
 * \brief   I2C master write data.
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * \param[in]  i2c_id        The I2C index, please refer to @ref T_I2C_ID.
 *
 * \param[in]  slave_addr    The target slave address to write to.
 *
 * \param[in]  reg_addr      The device register address to write to.
 *
 * \param[in]  write_buf     The byte data to be writen.
 *
 * \param[in]  write_len     The length for data buffer to be writen through I2C.
 *
 * \return                   The status of the I2C DMA initialization.
 * \retval  I2C_STATUS_OK        The HAL_I2C data is sent successfully.
 * \retval  I2C_STATUS_ARB_LOST:            Master or slave transmitter losed arbitration.
 * \retval  I2C_STATUS_ABRT_MASTER_DIS:     User tried to initiate a Master operation with the Master mode disabled.
 * \retval  I2C_STATUS_ABRT_TXDATA_NOACK:   Master sent data byte(s) following the address, but it did not receive an acknowledge from the remote slave.
 * \retval  I2C_STATUS_ABRT_10ADDR2_NOACK:  Master is in 10-bit address mode and the second address byte of the 10-bit address was not acknowledged by any slave.
 * \retval  I2C_STATUS_ABRT_10ADDR1_NOACK:  Master is in 10-bit address mode and the first 10-bit address byte was not acknowledged by any slave.
 * \retval  I2C_STATUS_ABRT_7B_ADDR_NOACK:  Master is in 7-bit address mode and the address sent was not acknowledged by any slave.
 * \retval  I2C_STATUS_TIMEOUT:                    I2C timeout.
 * \retval  I2C_STATUS_ERROR_INVALID_PARAMETER     The HAL_I2C was failed to send data due to invalid parameter.
 *
 * <b>Example usage</b>
 * \code{.c}
 * #define DEVICE_SLAVE_ADDR        (0xA0)
 * #define REG_ADDR                 (0x1020)
 * int i2c_test(void)
 * {
        uint8_t reg_datas[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05}
        status = hal_i2c_master_write_16b_regs(DEMO_I2C_ID, DEVICE_SLAVE_ADDR, REG_ADDR, reg_datas, 6);
 * }
 * \endcode
 *
 * \ingroup  HAL_I2C_Exported_Functions
 */
T_I2C_STATUS hal_i2c_master_write_16b_regs(T_I2C_ID i2c_id, uint16_t slave_addr, uint16_t reg_addr,
                                           uint8_t *write_buf, uint32_t write_len);

/**
 * hal_i2c.h
 *
 * \brief   I2C master read data.
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  i2c_id        The I2C index, please refer to @ref T_I2C_ID.
 *
 * \param[in]  slave_addr    The target slave address to write to.
 *
 * \param[in]  read_buf      The buffer to store read data.
 *
 * \param[in]  read_len      The length for data buffer to be read through I2C.
 *
 * \return                   The status of the I2C DMA initialization.
 * \retval  I2C_STATUS_OK        The HAL_I2C data is sent successfully.
 * \retval  I2C_STATUS_ARB_LOST:            Master or slave transmitter losed arbitration.
 * \retval  I2C_STATUS_ABRT_MASTER_DIS:     User tried to initiate a Master operation with the Master mode disabled.
 * \retval  I2C_STATUS_ABRT_TXDATA_NOACK:   Master sent data byte(s) following the address, but it did not receive an acknowledge from the remote slave.
 * \retval  I2C_STATUS_ABRT_10ADDR2_NOACK:  Master is in 10-bit address mode and the second address byte of the 10-bit address was not acknowledged by any slave.
 * \retval  I2C_STATUS_ABRT_10ADDR1_NOACK:  Master is in 10-bit address mode and the first 10-bit address byte was not acknowledged by any slave.
 * \retval  I2C_STATUS_ABRT_7B_ADDR_NOACK:  Master is in 7-bit address mode and the address sent was not acknowledged by any slave.
 * \retval  I2C_STATUS_TIMEOUT:                    I2C timeout.
 * \retval  I2C_STATUS_ERROR_INVALID_PARAMETER     The HAL_I2C was failed to send data due to invalid parameter.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int i2c_test(void)
 * {
        status = hal_i2c_master_read(DEMO_I2C_ID, ADDR, I2C_ReadBuf, 6);
 * }
 * \endcode
 *
 * \ingroup  HAL_I2C_Exported_Functions
 */
T_I2C_STATUS hal_i2c_master_read(T_I2C_ID i2c_id, uint16_t slave_addr, uint8_t *read_buf,
                                 uint32_t read_len);

/**
 * hal_i2c.h
 *
 * \brief   I2C master write data and then repeat read.
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  i2c_id        The I2C index, please refer to @ref T_I2C_ID.
 *
 * \param[in]  slave_addr    The target slave address to write to.
 *
 * \param[in]  write_buf     The byte data to be writen.
 *
 * \param[in]  write_len     The length for data buffer to be writen through I2C.
 *
 * \param[in]  read_buf      The buffer to store read data.
 *
 * \param[in]  read_len      The length for data buffer to be read through I2C.
 *
 * \return                   The status of the I2C DMA initialization.
 * \retval  I2C_STATUS_OK        The HAL_I2C data is sent successfully.
 * \retval  I2C_STATUS_ARB_LOST:            Master or slave transmitter losed arbitration.
 * \retval  I2C_STATUS_ABRT_MASTER_DIS:     User tried to initiate a Master operation with the Master mode disabled.
 * \retval  I2C_STATUS_ABRT_TXDATA_NOACK:   Master sent data byte(s) following the address, but it did not receive an acknowledge from the remote slave.
 * \retval  I2C_STATUS_ABRT_10ADDR2_NOACK:  Master is in 10-bit address mode and the second address byte of the 10-bit address was not acknowledged by any slave.
 * \retval  I2C_STATUS_ABRT_10ADDR1_NOACK:  Master is in 10-bit address mode and the first 10-bit address byte was not acknowledged by any slave.
 * \retval  I2C_STATUS_ABRT_7B_ADDR_NOACK:  Master is in 7-bit address mode and the address sent was not acknowledged by any slave.
 * \retval  I2C_STATUS_TIMEOUT:                    I2C timeout.
 * \retval  I2C_STATUS_ERROR_INVALID_PARAMETER     The HAL_I2C was failed to send data due to invalid parameter.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int i2c_test(void)
 * {
        status = hal_i2c_master_write_to_read(DEMO_I2C_ID, ADDR, I2C_WriteBuf, 1, aI2C_ReadBuf, 6);
 * }
 * \endcode
 *
 * \ingroup  HAL_I2C_Exported_Functions
 */
T_I2C_STATUS hal_i2c_master_write_to_read(T_I2C_ID i2c_id, uint16_t slave_addr,
                                          uint8_t *write_buf, uint32_t write_len,
                                          uint8_t *read_buf, uint32_t read_len);


/**
 * hal_i2c.h
 *
 * \brief   I2C master send general call to all slave device.
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  i2c_id        The I2C index, please refer to @ref T_I2C_ID.
 *
 * \param[in]  data          The byte data to be send through general call.
 *
 * \return                   The status of the I2C DMA initialization.
 * \retval  I2C_STATUS_OK        The HAL_I2C data is sent successfully.
 * \retval  I2C_STATUS_ARB_LOST:            Master or slave transmitter losed arbitration.
 * \retval  I2C_STATUS_ABRT_MASTER_DIS:     User tried to initiate a Master operation with the Master mode disabled.
 * \retval  I2C_STATUS_ABRT_TXDATA_NOACK:   Master sent data byte(s) following the address, but it did not receive an acknowledge from the remote slave.
 * \retval  I2C_STATUS_ABRT_10ADDR2_NOACK:  Master is in 10-bit address mode and the second address byte of the 10-bit address was not acknowledged by any slave.
 * \retval  I2C_STATUS_ABRT_10ADDR1_NOACK:  Master is in 10-bit address mode and the first 10-bit address byte was not acknowledged by any slave.
 * \retval  I2C_STATUS_ABRT_7B_ADDR_NOACK:  Master is in 7-bit address mode and the address sent was not acknowledged by any slave.
 * \retval  I2C_STATUS_TIMEOUT:                    I2C timeout.
 * \retval  I2C_STATUS_ERROR_INVALID_PARAMETER     The HAL_I2C was failed to send data due to invalid parameter.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int i2c_test(void)
 * {
        status = hal_i2c_master_general_call(DEMO_I2C_ID, ADDR, 0x22);
 * }
 * \endcode
 *
 * \ingroup  HAL_I2C_Exported_Functions
 */
T_I2C_STATUS hal_i2c_master_general_call(T_I2C_ID i2c_id, uint8_t data);


/**
 * hal_i2c.h
 *
 * \brief   I2C master write data.
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  i2c_id        The I2C index, please refer to @ref T_I2C_ID.
 *
 * \param[in]  slave_addr    The target slave address to write to.
 *
 * \param[in]  write_buf     The byte data to be writen.
 *
 * \param[in]  write_len     The length for data buffer to be writen through I2C.
 *
 * \return                   The status of the I2C DMA initialization.
 * \retval  I2C_STATUS_OK        The HAL_I2C data is sent successfully.
 * \retval  I2C_STATUS_ARB_LOST:            Master or slave transmitter losed arbitration.
 * \retval  I2C_STATUS_ABRT_MASTER_DIS:     User tried to initiate a Master operation with the Master mode disabled.
 * \retval  I2C_STATUS_ABRT_TXDATA_NOACK:   Master sent data byte(s) following the address, but it did not receive an acknowledge from the remote slave.
 * \retval  I2C_STATUS_ABRT_10ADDR2_NOACK:  Master is in 10-bit address mode and the second address byte of the 10-bit address was not acknowledged by any slave.
 * \retval  I2C_STATUS_ABRT_10ADDR1_NOACK:  Master is in 10-bit address mode and the first 10-bit address byte was not acknowledged by any slave.
 * \retval  I2C_STATUS_ABRT_7B_ADDR_NOACK:  Master is in 7-bit address mode and the address sent was not acknowledged by any slave.
 * \retval  I2C_STATUS_TIMEOUT:                    I2C timeout.
 * \retval  I2C_STATUS_ERROR_INVALID_PARAMETER     The HAL_I2C was failed to send data due to invalid parameter.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int i2c_test(void)
 * {
        status = hal_i2c_master_write_by_dma(DEMO_I2C_ID, ADDR, I2C_WriteBuf, 6);
 * }
 * \endcode
 *
 * \ingroup  HAL_I2C_Exported_Functions
 */
T_I2C_STATUS hal_i2c_master_write_by_dma(T_I2C_ID i2c_id, uint16_t slave_addr,
                                         uint16_t *write_buf, uint32_t write_len);

#ifdef __cplusplus
}
#endif

#endif /* _HAL_I2C_ */

/** @} */ /* End of group HAL_I2C_Exported_Functions */
/** @} */ /* End of group HAL_I2C */

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/

