/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file    fmc_api_ext.h
* @brief   This file provides fmc EXT API wrapper for sdk customers.
* @author  zola_zhang
* @date    2021-4-29
* @version v1.0
* *************************************************************************************
*/

#ifndef __FMC_API_EXT_H_
#define __FMC_API_EXT_H_

#include <stdint.h>
#include <stdbool.h>

/** @defgroup  87x3d_FMC_EXT    FMC EXT API
    * @brief FMC EXT wrapper.
    * @{
    */

/*============================================================================*
 *                                   Types
 *============================================================================*/
/** @defgroup 87x3d_FMC_EXT_TYPE FMC EXT Exported Types
   * @brief
   * @{
   */
typedef enum
{
    FMC_FLASH_NOR_1_BIT_MODE,
    FMC_FLASH_NOR_2_BIT_MODE,
    FMC_FLASH_NOR_4_BIT_MODE,
    FMC_FLASH_NOR_8_BIT_MODE,
} FMC_FLASH_NOR_BIT_MODE;

typedef enum
{
    FMC_PSRAM_IDX0,
    FMC_PSRAM_IDX1,
    FMC_PSRAM_IDX2,
    FMC_PSRAM_IDX3,
    FMC_PSRAM_IDX4
} FMC_PSRAM_IDX_TYPE;

typedef union _FMC_SPIC_ACCESS_INFO
{
    uint32_t d32[3];
    struct
    {
        uint32_t cmd: 16;
        uint32_t cmd_len: 2;
        uint32_t rsvd0: 14;
        uint32_t addr;
        uint32_t addr_len: 3;
        uint32_t cmd_ch: 2;
        uint32_t addr_ch: 2;
        uint32_t data_ch: 2;
        uint32_t dummy_len: 15;
        uint32_t rsvd1: 8;
    };
} FMC_SPIC_ACCESS_INFO;

typedef enum
{
    FMC_SPIC_SING_CH,
    FMC_SPIC_DUAL_CH,
    FMC_SPIC_QUAD_CH,
    FMC_SPIC_OCTAL_CH,
} FMC_SPIC_CFG_CH;

typedef enum
{
    FMC_SPIC_DEVICE_NOR_FLASH,
    FMC_SPIC_DEVICE_NAND_FLASH,
    FMC_SPIC_DEVICE_QSPI_PSRAM,
} FMC_SPIC_DEVICE_TYPE;

typedef enum
{
    FMC_PSRAM_LPM_STANDBY_MODE,
    FMC_PSRAM_LPM_HALF_SLEEP_MODE,
    FMC_PSRAM_LPM_DEEP_POWER_DOWN_MODE,
} FMC_PSRAM_LPM_TYPE;

typedef enum
{
    FMC_SPIC_DATA_BYTE,
    FMC_SPIC_DATA_HALF,
    FMC_SPIC_DATA_WORD,
} FMC_SPIC_CFG_DATA_LEN;

/** End of 87x3d_FMC_EXT_TYPE
    * @}
    */

/*============================================================================*
  *                                Functions
  *============================================================================*/
/** @defgroup 87x3d_FMC_EXT_Functions FMC EXT Exported Functions
    * @brief
    * @{
    */
/**
 * @brief           Init NOR flash with index.
 * @param idx       The index of SPIC.
 * @return          True if init is successful, otherwise false.
 */
bool fmc_flash_nor_init(FMC_SPIC_ID idx);

/**
 * @brief           PSRAM set 4-bit mode.
 * @return          True if set successfully, otherwise false.
 */
bool fmc_psram_set_4bit_mode(void);

/**
 * @brief           PSRAM set sequential transaction.
 * @param enable    Enable or disable psram sequential transaction.
 * @return          True if set successfully, otherwise false.
 */
bool fmc_psram_set_seq_trans(bool enable);

/**
 * @brief           NOR flash try high-speed mode with bit mode configuration.
 * @param idx       The index of SPIC.
 * @param bit_mode  NOR flash bit mode in @ref FMC_FLASH_NOR_BIT_MODE.
 * @return          True if changing to dual/quad mode successfully, otherwise false.
 */
bool fmc_flash_try_high_speed_mode(FMC_SPIC_ID idx, FMC_FLASH_NOR_BIT_MODE bit_mode);

/**
 * @brief           Get the RDID of NOR flash.
 * \xrefitem Added_API_2_12_1_0 "Added Since 2.12.1.0" "Added API"
 * @param idx       The index of SPIC.
 * @return          The RDID of NOR flash.
 */
uint32_t fmc_flash_nor_get_rdid(FMC_SPIC_ID idx);

/**
 * @brief           Reinit NOR flash.
 * \xrefitem Added_API_2_12_1_0 "Added Since 2.12.1.0" "Added API"
 * @param idx       The index of SPIC.
 * @return          True means successful, otherwise false.
 */
bool fmc_flash_reinitialize(FMC_SPIC_ID idx);

/**
 * @brief           Enter or exit NOR flash 4-byte address mode.
 * @param idx       The index of SPIC.
 * @param enable    Enable or disable 4-byte address mode.
 * @return          True if set successfully, otherwise false.
 */
bool fmc_flash_set_4_byte_address_mode(FMC_SPIC_ID idx, bool enable);

/**
 * @brief           NOR flash set sequential transaction.
 * @param idx       The index of SPIC.
 * @param enable    Enable or disable sequential transaction.
 * @return          True if set successfully, otherwise false.
 */
bool fmc_flash_set_seq_trans(FMC_SPIC_ID idx, bool enable);

/**
 * @brief           NOR flash set auto mode
 * @param idx       The index of SPIC.
 * @param bit_mode  NOR flash bit mode in @ref FMC_FLASH_NOR_BIT_MODE.
 * @return          True if set successfully, otherwise false.
 */
bool fmc_flash_set_auto_mode(FMC_SPIC_ID idx, FMC_FLASH_NOR_BIT_MODE bit_mode);

/**
 * @brief           Switch NOR flash clock.
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 * @param idx       The index of SPIC.
 * @param  require_mhz  Require NOR flash freqency.
 * @param  actual_mhz     The freqency of current NOR flash.
 * @return          True if switch clock successful, otherwise false.
 */
bool fmc_flash_nor_clock_switch(FMC_SPIC_ID idx, uint32_t required_mhz,
                                uint32_t *actual_mhz);

/**
 * @brief           Set PSRAM to enter lower power mode(LPM).
 * @param idx       The index of SPIC.
 * @param mode      The LPM type in @ref FMC_PSRAM_LPM_TYPE.
 * @return          True if entering LPM is successful, otherwise false.
 */
bool fmc_psram_enter_lpm(FMC_SPIC_ID idx, FMC_PSRAM_LPM_TYPE mode);

/**
 * @brief           Set PSRAM to exit low power mode.
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 * @param idx       The index of SPIC.
 * @param mode      The LPM type in @ref FMC_PSRAM_LPM_TYPE.
 * @return          True if exiting LPM is successful, otherwise false.
 */
bool fmc_psram_exit_lpm(FMC_SPIC_ID idx, FMC_PSRAM_LPM_TYPE mode);

/**
 * @brief           Initialize Winbond OPI PSRAM by SPI
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 * @return          True if initialization is successful, otherwise false.
 */
bool fmc_psram_spi_winbond_opi_init(void);

/**
 * @brief           Initialize APMemory QPI psram
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 * @param idx       The index in @ref FMC_PSRAM_IDX_TYPE.
 * @return          True if init successful, otherwise false.
 */
bool fmc_psram_spi_ap_memory_qpi_init(FMC_SPIC_ID idx);

/**
 * @brief           psram clock switch
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 * @param idx        The index of SPIC.
 * @param required_mhz       The required clock(unit: MHz).
 * @param actual_mhz   Return the actual clock(unit: MHz).
 * @return          True if init successful, otherwise false.
 */
bool fmc_psram_spi_clock_switch(FMC_SPIC_ID idx, uint32_t required_mhz,
                                uint32_t *actual_mhz);
/**
 * @brief           SPIC2 init.
 */
void fmc_spic2_init(void);

/**
 * @brief           Set the max retry of SPIC2.
 * @param max_retry The max_retry to be set.
 * @return          True if setting is successful, otherwise false.
 */
bool fmc_spic2_set_max_retry(uint32_t max_retry);

/**
 * @brief           Get the max retry of SPIC2.
 * @return          The max retry of SPIC2.
 */
uint32_t fmc_spic2_get_max_retry(void);

/**
 * @brief           Check if SPIC2 is in a busy state.
 * @return          True if SPIC2 is busy, otherwise false.
 */
bool fmc_spic2_in_busy(void);

/**
 * @brief           SPIC2 lock.
 * @param lock_flag Used to pass back the lock flag.
 * @return          True if lock is successful, otherwise false.
 */
bool fmc_spic2_lock(uint8_t *lock_flag);

/**
 * @brief           SPIC2 unlock.
 * @param lock_flag The lock flag returned in @ref fmc_spic2_lock.
 * @return          True if unlock is successful, otherwise false.
 */
bool fmc_spic2_unlock(uint8_t *lock_flag);

/**
 * @brief           Set SPIC2 device type.
 * @param dev       The device type in @ref FMC_SPIC_DEVICE_TYPE.
 */
void fmc_spic2_set_device(FMC_SPIC_DEVICE_TYPE dev);

/**
 * @brief           Set SPIC2 SSIENR enable.
 * @param enable    Enable or disable SPIC2 SSIENR.
 */
void fmc_spic2_enable(bool enable);

/**
 * @brief           Disable SPIC2 interrupt.
 */
void fmc_spic2_disable_interrupt(void);

/**
 * @brief           Set SPIC2 SIPOL enable or disable.
 * @param sipol     SIPOL bit.
 * @param enable    SIPOL enable or disable.
 */
void fmc_spic2_set_sipol(uint8_t sipol, bool enable);

/**
 * @brief           Set SPIC2 to RX mode.
 */
void fmc_spic2_set_rx_mode(void);

/**
 * @brief           Set SPIC2 to TX mode.
 */
void fmc_spic2_set_tx_mode(void);

/**
 * @brief           Set SPIC2 to auto mode.
 */
void fmc_spic2_set_auto_mode(void);

/**
 * @brief           Set SPIC2 to user mode.
 */
void fmc_spic2_set_user_mode(void);

/**
 * @brief           SPIC2 set RX NDF register.
 * @param ndf       The NDF register bit.
 */
void fmc_spic2_set_rxndf(uint32_t ndf);

/**
 * @brief           SPIC2 set TX NDF register.
 * @param ndf       The NDF register bit.
 */
void fmc_spic2_set_txndf(uint32_t ndf);

/**
 * @brief           SPIC2 set DR.
 * @param data      DR data to be set.
 * @param len       The spic cfg data len in @ref FMC_SPIC_CFG_DATA_LEN.
 */
void fmc_spic2_set_dr(uint32_t data, FMC_SPIC_CFG_DATA_LEN len);

/**
 * @brief           SPIC2 get DR.
 * @param len       The spic cfg data len in @ref FMC_SPIC_CFG_DATA_LEN.
 * @return          The DR of SPIC2.
 */
uint32_t fmc_spic2_get_dr(FMC_SPIC_CFG_DATA_LEN len);

/**
 * @brief           SPIC2 set cmd length.
 * @param len       The cmd length to be set.
 */
void fmc_spic2_set_cmd_len(uint8_t len);

/**
 * @brief           SPIC2 set user address length.
 * @param len       The user address length to be set.
 */
void fmc_spic2_set_user_addr_len(uint8_t len);

/**
 * @brief           SPIC2 set auto address length.
 * @param len       The auto address length to be set.
 */
void fmc_spic2_set_auto_addr_len(uint8_t len);

/**
 * @brief           SPIC2 set delay length.
 * @param delay_len The delay length to be set.
 */
void fmc_spic2_set_delay_len(uint8_t delay_len);

/**
 * @brief           SPIC2 set user dummy length.
 * @param dummy_len The dummy length to be set.
 */
void fmc_spic2_set_user_dummy_len(uint8_t dummy_len);

/**
 * @brief           SPIC2 set auto dummy length.
 * @param dummy_len The dummy length to be set.
 */
void fmc_spic2_set_auto_dummy_len(uint8_t dummy_len);

/**
 * @brief           SPIC2 set baud.
 * @param baud      The baud to be set.
 */
void fmc_spic2_set_baud(uint16_t baud);

/**
 * @brief           SPIC2 get baud.
 * @return          SPIC2 baud.
 */
uint16_t fmc_spic2_get_baud(void);

/**
 * @brief           Set SPIC2 multi ch.
 * @param cmd       The cmd ch in @ref FMC_SPIC_CFG_CH.
 * @param addr      The address ch in @ref FMC_SPIC_CFG_CH.
 * @param data      The data ch in @ref FMC_SPIC_CFG_CH.
 */
void fmc_spic2_set_multi_ch(FMC_SPIC_CFG_CH cmd, FMC_SPIC_CFG_CH addr, FMC_SPIC_CFG_CH data);

/**
 * @brief           SPIC2 set sequential transaction.
 * @param enable    Enable or disable sequential transaction.
 */
void fmc_spic2_set_seq_trans(bool enable);

/**
 * @brief           SPIC2 clean valid cmd.
 */
void fmc_spic2_clean_valid_cmd(void);

/**
 * @brief           SPIC2 enable or disable dummy byte.
 * @param enable    Enable or disable dummy byte.
 */
void fmc_spic2_enable_dum_byte(bool enable);

/**
 * @brief           SPIC2 cmd rx.
 * @param info      SPIC access info.
 * @param buf       Receive buffer.
 * @param buf_len   Receive buffer length.
 * @return          True if successful, otherwise false.
 */
bool fmc_spic2_cmd_rx(FMC_SPIC_ACCESS_INFO *info, uint8_t *buf, uint8_t buf_len);

/**
 * @brief           SPIC2 cmd tx.
 * @param info      SPIC access info.
 * @param buf       Transport buffer.
 * @param buf_len   Transport buffer length.
 */
bool fmc_spic2_cmd_tx(FMC_SPIC_ACCESS_INFO *info, uint8_t *buf, uint8_t buf_len);

/**
 * @brief           Set SPIC SSIENR enable.
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The index of SPIC.
 * @param enable    Enable or disable SPIC SSIENR.
 */
void fmc_spic_enable(FMC_SPIC_ID spic_id, bool enable);

/**
 * @brief           SPIC set user mode.
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The index of SPIC.
 */
void fmc_spic_set_user_mode(FMC_SPIC_ID spic_id);

/**
 * @brief           Set SPIC multi ch.
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The index of SPIC.
 * @param cmd       The cmd ch in @ref FMC_SPIC_CFG_CH.
 * @param addr      The address ch in @ref FMC_SPIC_CFG_CH.
 * @param data      The data ch in @ref FMC_SPIC_CFG_CH.
 */
void fmc_spic_set_multi_ch(FMC_SPIC_ID spic_id, FMC_SPIC_CFG_CH cmd, FMC_SPIC_CFG_CH addr,
                           FMC_SPIC_CFG_CH data);

/**
 * @brief           SPIC set tx mode.
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The index of SPIC.
 */
void fmc_spic_set_tx_mode(FMC_SPIC_ID spic_id);

/**
 * @brief           SPIC set cmd length.
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The index of SPIC.
 * @param len       The cmd length to be set.
 */
void fmc_spic_set_cmd_len(FMC_SPIC_ID spic_id, uint8_t len);

/**
 * @brief           SPIC set user address length.
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The index of SPIC.
 * @param len       The user address length to be set.
 */
void fmc_spic_set_user_addr_len(FMC_SPIC_ID spic_id, uint8_t len);

/**
 * @brief           SPIC set tx ndf register.
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The index of SPIC.
 * @param ndf       The ndf register bit.
 */
void fmc_spic_set_txndf(FMC_SPIC_ID spic_id, uint32_t ndf);

/**
 * @brief           SPIC set DR.
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The index of SPIC.
 * @param data      DR data to be set.
 * @param len       The SPIC cfg data len in @ref FMC_SPIC_CFG_DATA_LEN.
 */
void fmc_spic_set_dr(FMC_SPIC_ID spic_id, uint32_t data, FMC_SPIC_CFG_DATA_LEN len);

/**
 * @brief           Check SPIC whether in busy state.
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The index of SPIC.
 * @return          True if SPIC is busy.
 */
bool fmc_spic_in_busy(FMC_SPIC_ID spic_id);

/**
 * @brief           SPIC set sequential transaction.
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The index of SPIC.
 * @param enable    Enable or disable sequential transaction.
 */
void fmc_spic_set_seq_trans(FMC_SPIC_ID spic_id, bool enable);

/**
 * @brief           SPIC init.
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The index of SPIC.
 */
void fmc_spic_init(FMC_SPIC_ID spic_id);

/**
 * @brief           SPIC set baud.
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The index of SPIC.
 * @param baud      The baud to be set.
 */
void fmc_spic_set_baud(FMC_SPIC_ID spic_id, uint16_t baud);

/**
 * @brief           spic set clock
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The index of SPIC.
 * @param  require_mhz  require spic freqency
 * @param  actual_mhz     the freqency of current spic
 * @return              true if switch clock successful, otherwise false
 */
bool fmc_spic_clock_switch(FMC_SPIC_ID spic_id, uint32_t required_mhz, uint32_t *actual_mhz);

/**
 * @brief           SPIC set DMA RX enable.
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The spic index, please refer to FMC_SPIC_ID.
 * @param enable    enable/disable dma rx
 */
void fmc_spic_set_dmac_rx_en(FMC_SPIC_ID spic_id, bool enable);

/**
 * @brief           SPIC set DMA TX enable.
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The index of SPIC.
 * @param enable    Enable or disable DMA TX.
 */
void fmc_spic_set_dmac_tx_en(FMC_SPIC_ID spic_id, bool enable);

/**
 * @brief           SPIC set DMA TX data level.
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id         The index of SPIC.
 * @param tx_data_lv      The DMA TX data level to be set.
 */
void fmc_spic_set_dmac_tx_data_lv(FMC_SPIC_ID spic_id, uint32_t tx_data_lv);

/**
 * @brief           SPIC set RX data level.
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id         The index of SPIC.
 * @param rx_data_lv      The DMA RX data level to be set.
 */
void fmc_spic_set_dmac_rx_data_lv(FMC_SPIC_ID spic_id, uint32_t rx_data_lv);


/** @} */ /* End of group 87x3d_FMC_EXT_Functions */

/** @} */ /* End of group 87x3d_FMC_EXT */

#endif



