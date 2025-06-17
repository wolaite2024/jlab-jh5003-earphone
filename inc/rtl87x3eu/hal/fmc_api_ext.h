/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file    fmc_api_ext.h
* @brief   This file provides FMC EXT API wrapper for sdk customer.
* @author  zola_zhang
* @date    2021-4-29
* @version v1.0
* *************************************************************************************
*/

#ifndef __FMC_API_EXT_H_
#define __FMC_API_EXT_H_

#include <clock_manager.h>
#include "fmc_api.h"

/** @defgroup  87x3e_FMC_EXT    FMC EXT API
    * @brief EXT EXT wrapper
    * @{
    */

/*============================================================================*
 *                                   Types
 *============================================================================*/
/** @defgroup FMC_EXT_TYPE FMC EXT Exported Types
   * @brief
   * @{
   */
typedef enum
{
    FMC_FLASH_NOR_1_BIT_MODE,
    FMC_FLASH_NOR_2_BIT_MODE,
    FMC_FLASH_NOR_4_BIT_MODE,
    FMC_FLASH_NOR_8_BIT_MODE,
    FMC_FLASH_NOR_1_BIT_FAST_MODE,
    FMC_FLASH_NOR_1_BIT_DTR_MODE,
    FMC_FLASH_NOR_2_BIT_DTR_MODE,
    FMC_FLASH_NOR_4_BIT_DTR_MODE,
    FMC_FLASH_NOR_BIT_MODE_MAX,
} FMC_FLASH_NOR_BIT_MODE;

typedef enum
{
    FMC_PSRAM_IDX0,
    FMC_PSRAM_IDX1,
    FMC_PSRAM_IDX2,
    FMC_PSRAM_IDX3
} FMC_PSRAM_IDX_TYPE;

typedef enum
{
    FMC_PSRAM_LPM_STANDBY_MODE,
    FMC_PSRAM_LPM_HALF_SLEEP_MODE,
    FMC_PSRAM_LPM_DEEP_POWER_DOWN_MODE,
} FMC_PSRAM_LPM_TYPE;

typedef enum
{
    FMC_PSRAM_WB_REFRESH_FULL       = 0,
    FMC_PSRAM_WB_REFRESH_BOTTOM_1_2 = 1,
    FMC_PSRAM_WB_REFRESH_BOTTOM_1_4 = 2,
    FMC_PSRAM_WB_REFRESH_BOTTOM_1_8 = 3,
    FMC_PSRAM_WB_REFRESH_NONE       = 4,
    FMC_PSRAM_WB_REFRESH_TOP_1_2    = 5,
    FMC_PSRAM_WB_REFRESH_TOP_1_4    = 6,
    FMC_PSRAM_WB_REFRESH_TOP_1_8    = 7,
} FMC_PSRAM_WB_PARTIAL_ARRAY_REFRESH;

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
        uint32_t ddr_en: 3; // cmd_ddr_en:addr_ddr_en:data_ddr_en
        uint32_t rsvd1: 5;
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
    FMC_SPIC_DATA_BYTE,
    FMC_SPIC_DATA_HALF,
    FMC_SPIC_DATA_WORD,
} FMC_SPIC_CFG_DATA_LEN;

/** End of FMC_EXT_TYPE
    * @}
    */


/*============================================================================*
  *                                Functions
  *============================================================================*/
/** @defgroup FMC_EXT_Functions FMC EXT Exported Functions
    * @brief
    * @{
    */

/**
 * @brief           psram set 4-bit mode
 * @return          true if set successful, otherwise false
 */
bool fmc_psram_set_4bit_mode(void);

/**
 * @brief           psram set sequential transaction
 * @param enable    enable/disable psram sequential transaction
 * @return          true if set successful, otherwise false
 */
bool fmc_psram_set_seq_trans(bool enable);

/**
 * @brief           nor flash try high speed mode with bit mode configuration
 * @param idx       specific nor flash @ref FMC_SPIC_ID
 * @param bit_mode  nor flash bit mode @ref FMC_FLASH_NOR_BIT_MODE
 * @return          true if change to dual/quad mode successful, otherwise false
 */
bool fmc_flash_try_high_speed_mode(FMC_SPIC_ID idx, FMC_FLASH_NOR_BIT_MODE bit_mode);

/**
 * @brief           enter/exit nor flash 4-byte address mode
 * @param idx       the index of nor flash @ref FMC_SPIC_ID
 * @param enable    enable/disable 4-byte address mode
 * @return          true if set successful, otherwise false
 */
bool fmc_flash_set_4_byte_address_mode(FMC_SPIC_ID idx, bool enable);

/**
 * @brief           flash set sequential transaction
 * @param idx       the index of nor flash @ref FMC_SPIC_ID
 * @param enable    enable/disable sequential transaction
 * @return          true if set successful, otherwise false
 */
bool fmc_flash_set_seq_trans(FMC_SPIC_ID idx, bool enable);

/**
 * @brief           flash set auto mode
 * @param idx       the index of nor flash @ref FMC_SPIC_ID
 * @param bit_mode  nor flash bit mode @ref FMC_FLASH_NOR_BIT_MODE
 * @return          true if set successful, otherwise false
 */
bool fmc_flash_set_auto_mode(FMC_SPIC_ID idx, FMC_FLASH_NOR_BIT_MODE bit_mode);

/**
 * @brief           get the rdid of nor flash
 * @param idx       the index of nor flash @ref FMC_SPIC_ID
 * @return          rdid, 0 means flash init error
 */
uint32_t fmc_flash_nor_get_rdid(FMC_SPIC_ID idx);

/**
 * @brief           reinit flash
 * @param idx       the index of nor flash @ref FMC_SPIC_ID
 * @return          true means successful, otherwise false
 */
bool fmc_flash_reinitialize(FMC_SPIC_ID idx);

/**
 * @brief           init nor flash with index
 * \xrefitem        Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 * @param idx       the index of nor flash @ref FMC_SPIC_ID
 * @return          true if init successful, otherwise false
 */
bool fmc_flash_nor_init(FMC_SPIC_ID idx);

/**
 * @brief           flash software reset
 * @param idx       the index of nor flash @ref FMC_SPIC_ID
 * @return          true if software reset successful, otherwise false
 */
bool fmc_flash_sw_reset(FMC_SPIC_ID idx);

/**
 * @brief           switch nor flash clock
 * \xrefitem        Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Added API"
 * @param idx       specific nor flash @ref FMC_FLASH_NOR_IDX_TYPE
 * @param  require_mhz  require nor flash freqency
 * @param  actual_mhz     the freqency of current nor flash
 * @return          true if switch clock successful, otherwise false
 */
bool fmc_flash_nor_clock_switch(FMC_SPIC_ID idx, uint32_t required_mhz,
                                uint32_t *actual_mhz);

/**
 * @brief           set psram enter lower power mode(lpm)
 * @param idx       the index of psram @ref FMC_SPIC_ID
 * @param mode      the LPM type in @ref FMC_PSRAM_LPM_TYPE
 * @return          true if enter lpm successful, otherwise false
 */
bool fmc_psram_enter_lpm(FMC_SPIC_ID idx, FMC_PSRAM_LPM_TYPE mode);

/**
 * @brief           set psram exit low power mode(lpm)
 * @param idx       the index of psram @ref FMC_SPIC_ID
 * @param mode      the LPM type in @ref FMC_PSRAM_LPM_TYPE
 * @return          true if exit lpm successful, otherwise false
 */
bool fmc_psram_exit_lpm(FMC_SPIC_ID idx, FMC_PSRAM_LPM_TYPE mode);

/**
 * @brief           set psram pad config for power off
 * @param idx       the index of psram @ref FMC_SPIC_ID
 * @param enter_lpm enter lpm or not
 * @return          true if set successful, otherwise false
 */
bool fmc_psram_set_pad_config_for_psram_power_off(FMC_SPIC_ID idx, bool enter_lpm);

/** \xrefitem Added_API_2_12_1_0 "Added Since 2.12.1.0" "Added API"
 * @brief           set wb psram partial refresh to reduce power consumption (W955D8MBYA)
 * @idx             the index to indicate which psram is used
 * @partial         the enum to indicate the part which keeps refreshing
 * @return          true if init successful, otherwise false
 */
bool fmc_psram_wb_set_partial_refresh(FMC_SPIC_ID idx,
                                      FMC_PSRAM_WB_PARTIAL_ARRAY_REFRESH partial);

/**
 * @brief           set wb psram configuration register
 * @param idx       the index of psram @ref FMC_SPIC_ID
 * @return          true if set successful, otherwise false
 */
bool fmc_psram_wb_set_initial_latency(FMC_SPIC_ID idx);

/**
 * @brief           spic2 init
 */
void fmc_spic2_init(void);

/**
 * @brief           spic2 set max retry
 * @param max_retry the max_retry want to be set
 * @return          true if set successful, otherwise false
 */
bool fmc_spic2_set_max_retry(uint32_t max_retry);

/**
 * @brief           spic2 get max retry
 * @return          the max retry of spic2
 */
uint32_t fmc_spic2_get_max_retry(void);

/**
 * @brief           check spic2 whether in busy state
 * @return          true if spic2 is in busy
 */
bool fmc_spic2_in_busy(void);

/**
 * @brief           spic2 lock
 * @param lock_flag used to pass back the lock flag
 * @return          true if lock successful
 */
bool fmc_spic2_lock(uint8_t *lock_flag);

/**
 * @brief           spic2 unlock
 * @param lock_flag the lock flag return in @ref fmc_spic2_lock
 * @return          true if unlock successful
 */
bool fmc_spic2_unlock(uint8_t *lock_flag);

/**
 * @brief           set spic2 device type
 * @param dev       the device type in @ref FMC_SPIC_DEVICE_TYPE
 */
void fmc_spic2_set_device(FMC_SPIC_DEVICE_TYPE dev);

/**
 * @brief           set spic2 SSIENR enable
 * @param enable    enable/disable spic2
 */
void fmc_spic2_enable(bool enable);

/**
 * @brief           spic2 disable interrupt
 */
void fmc_spic2_disable_interrupt(void);

/**
 * @brief           spic2 set SIPOL enable/disable
 * @param sipol     sipol bit
 * @param enable    sipol enable or disable
 */
void fmc_spic2_set_sipol(uint8_t sipol, bool enable);

/**
 * @brief           spic2 set rx mode
 */
void fmc_spic2_set_rx_mode(void);

/**
 * @brief           spic2 set tx mode
 */
void fmc_spic2_set_tx_mode(void);

/**
 * @brief           spic2 set auto mode
 */
void fmc_spic2_set_auto_mode(void);

/**
 * @brief           spic2 set user mode
 */
void fmc_spic2_set_user_mode(void);

/**
 * @brief           spic2 set rx ndf register
 * @param ndf       ndf register bit
 */
void fmc_spic2_set_rxndf(uint32_t ndf);

/**
 * @brief           spic2 set tx ndf register
 * @param ndf       ndf register bit
 */
void fmc_spic2_set_txndf(uint32_t ndf);

/**
 * @brief           spic2 set DR
 * @param data      DR data want to be set
 * @param len       the spic cfg data len in @ref FMC_SPIC_CFG_DATA_LEN
 */
void fmc_spic2_set_dr(uint32_t data, FMC_SPIC_CFG_DATA_LEN len);

/**
 * @brief           spic2 get DR
 * @param len       the spic cfg data len in @ref FMC_SPIC_CFG_DATA_LEN
 */
uint32_t fmc_spic2_get_dr(FMC_SPIC_CFG_DATA_LEN len);

/**
 * @brief           spic2 set cmd length
 * @param len       the cmd length want to be set
 */
void fmc_spic2_set_cmd_len(uint8_t len);

/**
 * @brief           spic2 set user address length
 * @param len       the user address length want to be set
 */
void fmc_spic2_set_user_addr_len(uint8_t len);

/**
 * @brief           spic2 set auto address length
 * @param len       the auto address length want to be set
 */
void fmc_spic2_set_auto_addr_len(uint8_t len);

/**
 * @brief           spic2 set delay length
 * @param delay_len the delay length want to be set
 */
void fmc_spic2_set_delay_len(uint8_t delay_len);

/**
 * @brief           spic2 set user dummy length
 * @param dummy_len the dummy length want to be set
 */
void fmc_spic2_set_user_dummy_len(uint8_t dummy_len);

/**
 * @brief           spic2 set auto dummy length
 * @param dummy_len the dummy length want to be set
 */
void fmc_spic2_set_auto_dummy_len(uint8_t dummy_len);

/**
 * @brief           spic2 set baud
 * @param baud      the baud want to be set
 */
void fmc_spic2_set_baud(uint16_t baud);

/**
 * @brief           spic2 get baud
 * @return          spic2 baud
 */
uint16_t fmc_spic2_get_baud(void);

/**
 * @brief           set spic2 multi ch
 * @param cmd       the cmd ch in @ref FMC_SPIC_CFG_CH
 * @param addr      the address ch in @ref FMC_SPIC_CFG_CH
 * @param data      the data ch in @ref FMC_SPIC_CFG_CH
 */
void fmc_spic2_set_multi_ch(FMC_SPIC_CFG_CH cmd, FMC_SPIC_CFG_CH addr, FMC_SPIC_CFG_CH data);

/**
 * @brief           spic2 set sequential transaction
 * @param enable    enable/disable sequential transaction
 */
void fmc_spic2_set_seq_trans(bool enable);

/**
 * @brief           spic2 clean vaild cmd
 */
void fmc_spic2_clean_valid_cmd(void);

/**
 * @brief           spic2 enable dummy byte
 * @param enable    enable/disable dummy byte
 */
void fmc_spic2_enable_dum_byte(bool enable);

/**
 * @brief           spic2 cmd rx
 * @param info      spic access info
 * @param buf       receive buffer
 * @param buf_len   receive buffer length
 */
bool fmc_spic2_cmd_rx(FMC_SPIC_ACCESS_INFO *info, uint8_t *buf, uint8_t buf_len);

/**
 * @brief           spic2 cmd tx
 * @param info      spic access info
 * @param buf       transport buffer
 * @param buf_len   transport buffer length
 */
bool fmc_spic2_cmd_tx(FMC_SPIC_ACCESS_INFO *info, uint8_t *buf, uint8_t buf_len);

/**
 * @brief           set spic SSIENR enable
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The spic index, please refer to FMC_SPIC_ID.
 * @param enable    enable/disable spic
 */
void fmc_spic_enable(FMC_SPIC_ID spic_id, bool enable);

/**
 * @brief           spic set user mode
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The spic index, please refer to FMC_SPIC_ID.
 */
void fmc_spic_set_user_mode(FMC_SPIC_ID spic_id);

/**
 * @brief           set spic multi ch
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The spic index, please refer to FMC_SPIC_ID.
 * @param cmd       the cmd ch in @ref FMC_SPIC_CFG_CH
 * @param addr      the address ch in @ref FMC_SPIC_CFG_CH
 * @param data      the data ch in @ref FMC_SPIC_CFG_CH
 */
void fmc_spic_set_multi_ch(FMC_SPIC_ID spic_id, FMC_SPIC_CFG_CH cmd, FMC_SPIC_CFG_CH addr,
                           FMC_SPIC_CFG_CH data);

/**
 * @brief           spic set tx mode
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The spic index, please refer to FMC_SPIC_ID.
 */
void fmc_spic_set_tx_mode(FMC_SPIC_ID spic_id);

/**
 * @brief           spic set cmd length
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The spic index, please refer to FMC_SPIC_ID.
 * @param len       the cmd length want to be set
 */
void fmc_spic_set_cmd_len(FMC_SPIC_ID spic_id, uint8_t len);

/**
 * @brief           spic set user address length
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The spic index, please refer to FMC_SPIC_ID.
 * @param len       the user address length want to be set
 */
void fmc_spic_set_user_addr_len(FMC_SPIC_ID spic_id, uint8_t len);

/**
 * @brief           spic set tx ndf register
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The spic index, please refer to FMC_SPIC_ID.
 * @param ndf       ndf register bit
 */
void fmc_spic_set_txndf(FMC_SPIC_ID spic_id, uint32_t ndf);

/**
 * @brief           spic set DR
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The spic index, please refer to FMC_SPIC_ID.
 * @param data      DR data want to be set
 * @param len       the spic cfg data len in @ref FMC_SPIC_CFG_DATA_LEN
 */
void fmc_spic_set_dr(FMC_SPIC_ID spic_id, uint32_t data, FMC_SPIC_CFG_DATA_LEN len);

/**
 * @brief           check spic whether in busy state
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The spic index, please refer to FMC_SPIC_ID.
 * @return          true if spic2 is in busy
 */
bool fmc_spic_in_busy(FMC_SPIC_ID spic_id);

/**
 * @brief           spic set sequential transaction
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The spic index, please refer to FMC_SPIC_ID.
 * @param enable    enable/disable sequential transaction
 */
void fmc_spic_set_seq_trans(FMC_SPIC_ID spic_id, bool enable);

/**
 * @brief           spic init
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The spic index, please refer to FMC_SPIC_ID.
 */
void fmc_spic_init(FMC_SPIC_ID spic_id);

/**
 * @brief           spic set baud
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The spic index, please refer to FMC_SPIC_ID.
 * @param baud      the baud want to be set
 */
void fmc_spic_set_baud(FMC_SPIC_ID spic_id, uint16_t baud);

/**
 * @brief           spic set dma rx enable
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The spic index, please refer to FMC_SPIC_ID.
 * @param enable    enable/disable dma rx
 */
void fmc_spic_set_dmac_rx_en(FMC_SPIC_ID spic_id, bool enable);

/**
 * @brief           spic set dma tx enable
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id   The spic index, please refer to FMC_SPIC_ID.
 * @param enable    enable/disable dma tx
 */
void fmc_spic_set_dmac_tx_en(FMC_SPIC_ID spic_id, bool enable);

/**
 * @brief           spic set dma tx data level
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id         The spic index, please refer to FMC_SPIC_ID.
 * @param tx_data_lv      the dma tx data level want to be set
 */
void fmc_spic_set_dmac_tx_data_lv(FMC_SPIC_ID spic_id, uint32_t tx_data_lv);

/**
 * @brief           spic set rx data level
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id         The spic index, please refer to FMC_SPIC_ID.
 * @param rx_data_lv      the dma rx data level want to be set
 */
void fmc_spic_set_dmac_rx_data_lv(FMC_SPIC_ID spic_id, uint32_t rx_data_lv);

/**
 * @brief           spic set clock
 *
 * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
 *
 * @param spic_id       The spic index, please refer to FMC_SPIC_ID.
 * @param  require_mhz  require spic freqency
 * @param  actual_mhz     the freqency of current spic
 * @return              true if switch clock successful, otherwise false
 */
bool fmc_spic_clock_switch(FMC_SPIC_ID spic_id, uint32_t required_mhz, uint32_t *actual_mhz);

/** @} */ /* End of group FMC_EXT_Functions */

/** @} */ /* End of group 87x3e_FMC_EXT */

#endif

