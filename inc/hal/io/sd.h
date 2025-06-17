/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     sd.h
* @brief    This file provides application functions for SD card or eMMC card library.
* @details
* @author   qinyuan_hu
* @date     2024-10-15
* @version  v2.0
*********************************************************************************************************
*/

#ifndef __SD_H
#define __SD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "sd_define.h"


/** @addtogroup SD SD
  * @brief SD driver module.
  * @{
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/

/** @defgroup SD_Exported_Constants SD Exported Constants
  * @{
  */

typedef enum
{
    GROUP_0,
    GROUP_1,
    GROUP_MAX,
} T_SDIO_PIN_GROUP;

typedef enum
{
    SD_IF_NONE      = 0x00,
    SD_IF_SD_CARD   = 0x01,
    SD_IF_MMC       = 0x02,
    SD_IF_SDIO_CARD = 0x03,
} T_SD_IF_TYPE;

typedef enum
{
    SD_BUS_WIDTH_1B,
    SD_BUS_WIDTH_4B = 2,
    SD_BUS_WIDTH_MAX
} T_SD_BUS_WIDTH;

typedef enum
{
#ifdef CONFIG_SOC_SERIES_RTL8773D
    /* Support high speed through set pll4 160MHz*/
    SD_BUS_CLK_80M = 0x100,
#else
    /* RTL87x3E support high speed through set cpu clk 80 or 100MHz; RTL87x3EP through pll1 100MHz*/
    SD_BUS_CLK_50M = 0x102,
#endif

    SD_BUS_CLK_40M = 0x101,
#ifdef CONFIG_SOC_SERIES_RTL8773E
    SD_BUS_CLK_25M = 0x103,
#endif
    SD_BUS_CLK_20M = 0x00,
    SD_BUS_CLK_10M = 0x01,
    SD_BUS_CLK_5M = 0x02,
    SD_BUS_CLK_2M5 = 0x04,
    SD_BUS_CLK_1M25 = 0x08,
    SD_BUS_CLK_625K = 0x10,
    SD_BUS_CLK_312K5 = 0x20,
    SD_BUS_CLK_156K25 = 0x40,
    SD_BUS_CLK_78K125 = 0x80,

} T_SD_BUS_CLK_FREQ;

typedef enum
{
    SDH_CLK,
    SDH_CMD,
    SDH_D0,
    SDH_D1,
    SDH_D2,
    SDH_D3,
    SDH_WT_PROT,
    SDH_CD,
} T_SD_PIN_NUM;

/** End of group SD_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Types
 *============================================================================*/


/** @defgroup SD_Exported_Types SD Exported Types
  * @{
  */

typedef void(*RW_DONE_CBACK)(void *);

/**
  * @brief  SD host controller ID definition.
  */
typedef enum
{
    SDHC_ID0,
    SDHC_ID1,
    SDHC_ID_MAX,
} T_SDHC_ID;

/**
  * @brief  SD configure structure definition.
  */
typedef struct
{
    uint8_t     sd_if_type: 2;
    uint8_t     rsv0: 2;
    uint8_t     sd_power_en: 1;
    uint8_t     sd_power_high_active: 1;
    uint8_t     sdh_bus_width: 2;

    uint8_t     sdh_group: 2;
    uint8_t     sd_debug_en: 1;
    uint8_t     sd_mutex_en: 1;
    uint8_t     rsv1: 4;
    uint16_t    sd_bus_clk_sel;
    uint8_t     sd_power_pin;
    RW_DONE_CBACK rw_cback;
    void       *rw_user_data;
} T_SD_CONFIG;

/** End of group SD_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/


/** @defgroup SD_Exported_Functions SD Exported Functions
  * @{
  */

/**
 * sd.h
 *
 * \brief   Config the SD card type information for SD card.
 *
 * \param[in]  app_sd_cfg    Point to SD card configuration parameters.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * static const T_SD_CONFIG    sd_card_cfg =
 * {
 *     .sd_if_type = SD_IF_SD_CARD,
 *     .sdh_group = GROUP_0,
 *     .sdh_bus_width = SD_BUS_WIDTH_4B,
 *     .sd_bus_clk_sel = SD_BUS_CLK_20M
 * };
 *
 * void sd_init(void)
 * {
 *    sd_config_init(SDHC_ID0, (T_SD_CONFIG *)&sd_card_cfg);
 * }
 *
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
void sd_config_init(uint8_t sdio_id, T_SD_CONFIG *sd_cfg);

/**
 * sd.h
 *
 * \brief   Config the PAD and PINMUX for SD card.
 *
 * \param[in]  None.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_init(void)
 * {
 *    sd_config_init(SDHC_ID0, (T_SD_CONFIG *)&sd_card_cfg);
 *    sd_board_init(SDHC_ID0);
 * }
 *
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
void sd_board_init(uint8_t sdio_id);

/**
 * sd.h
 *
 * \brief   Initial the SD card.
 *
 * \param[in]  None.
 *
 * \return The SD card init status, 0 is SD_OK.
 * \retval 0         The SD card was initialized successfully.
 * \retval 0x1-0xFF  The SD card was failed to initialized, error status can refer to \ref T_SD_STATUS.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_init(void)
 * {
 *    sd_config_init(SDHC_ID0, (T_SD_CONFIG *)&sd_card_cfg);
 *    sd_board_init(SDHC_ID0);
 *    sd_card_init(SDHC_ID0);
 * }
 *
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
T_SD_STATUS sd_card_init(uint8_t sdio_id);

/**
 * sd.h
 *
 * \brief   Suspend the SD host by disable SD CLK, when SD card is not powered off. Use sd_resume can be restored.
 *          At this time, SD card does not need to be powered on and initialized again.
 * \xrefitem Experimental_Added_API_2_14_0_0 " Experimental Added Since 2.14.0.0" "Added API"
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_test(void)
 * {
 *    sd_suspend(SDHC_ID0);
 * }
 *
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
void sd_suspend(uint8_t sdio_id);

/**
 * sd.h
 *
 * \brief   Resume the PAD config and SD host controller power on.
 * \xrefitem Experimental_Added_API_2_14_0_0 " Experimental Added Since 2.14.0.0" "Added API"
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_test(void)
 * {
 *    sd_resume(SDHC_ID0);
 * }
 *
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
void sd_resume(uint8_t sdio_id);

/**
 * sd.h
 *
 * \brief   Initialize the SD PAD, SD host and SD card, when SD card is powered off.
 * \xrefitem Experimental_Added_API_2_14_0_0 " Experimental Added Since 2.14.0.0" "Added API"
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_test(void)
 * {
 *    sd_init(SDHC_ID0);
 * }
 *
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
T_SD_STATUS sd_init(uint8_t sdio_id);

/**
 * sd.h
 *
 * \brief   Print the SD card binary data by TRACE_BINARY.
 *
 * \param[in]  p    Point to test buffer for SD read or write.
 * \param[in]  len  The data length to print.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define OPER_SD_CARD_ADDR       ((uint32_t)0x8000)
 * uint8_t *test_buf = NULL;
 *
 * void sd_test(void)
 * {
 *    test_buf = os_mem_alloc(OS_MEM_TYPE_BUFFER, 512);
 *    sd_read(SDHC_ID0, OPER_SD_CARD_ADDR, (uint32_t)test_buf, 512, 1);
 *    sd_print_binary_data(test_buf, 512);
 * }
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
void sd_print_binary_data(uint8_t *p, uint32_t len);

/**
 * sd.h
 *
 * \brief   Erase SD card from the specified start address to end address.
 *
 * \param[in]  start_addr    The SD card start address to be erased.
 *
 * \param[in]  end_addr     The SD card end address to be erased.
 *
 * \return The SD card erase status, 0 is SD_OK.
 * \retval 0         The SD card was erased successfully.
 * \retval 0x1-0xFF  The SD card was failed to erased, error status can refer to \ref T_SD_STATUS.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_test(void)
 * {
 *     sd_erase(SDHC_ID0, start_addr, end_addr);
 * }
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
T_SD_STATUS sd_erase(uint8_t sdio_id, uint32_t start_addr, uint32_t end_addr);

/**
 * sd.h
 *
 * \brief   Read data from a specified address in SD card.
 *
 * \param[in]  sector    The specified SD card address to read.
 *
 * \param[in]  buf       The buffer of SD card to read data. The buf address must be 4 bytes aligned.
 *
 * \param[in]  blk_size   The block size of SD card to read.
 *
 * \param[in]  blk_num   The block number of SD card to read.
 *
 * \return The SD card read status, 0 is SD_OK.
 * \retval 0         The SD card was read successfully.
 * \retval 0x1-0xFF  The SD card was failed to read, error status can refer to \ref T_SD_STATUS.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_test(void)
 * {
 *     uint32_t sd_status = 0;
 *     sd_status = sd_read(SDHC_ID0, OPER_SD_CARD_ADDR, (uint32_t)test_buf, SINGLE_BLOCK_SIZE, BLOCK_NUM);
 *     if (sd_status != 0)
 *     {
 *        IO_PRINT_ERROR0("sd_read fail");
 *        return ;
 *     }
 * }
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
T_SD_STATUS sd_read(uint8_t sdio_id, uint32_t sector, uint32_t buf,
                    uint16_t blk_size, uint16_t blk_num);

/**
 * sd.h
 *
 * \brief   Write data from a specified address in SD card.
 *
 * \param[in]  sector    The specified SD card address to write.
 *
 * \param[in]  buf       The buffer of SD card to write data. The buf address must be 4 bytes aligned.
 *
 * \param[in]  blk_size   The block size of SD card to write.
 *
 * \param[in]  blk_num   The block number of SD card to write.
 *
 * \return The SD card write status, 0 is SD_OK.
 * \retval 0         The SD card was write successfully.
 * \retval 0x1-0xFF  The SD card was failed to write, error status can refer to \ref T_SD_STATUS.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_test(void)
 * {
 *     uint32_t sd_status = 0;
 *     sd_status = sd_write(SDHC_ID0, OPER_SD_CARD_ADDR, (uint32_t)test_buf, SINGLE_BLOCK_SIZE, BLOCK_NUM);
 *     if (sd_status != 0)
 *     {
 *        IO_PRINT_ERROR0("sd_write fail");
 *        return ;
 *     }
 * }
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
T_SD_STATUS sd_write(uint8_t sdio_id, uint32_t sector, uint32_t buf,
                     uint16_t blk_size, uint16_t blk_num);

/**
 * sd.h
 *
 * \brief   Set block length for standard capacity SD card.
 *
 * \param[in]   block_len   The block length to set.
 *
 * \return The SD card set block length status, 0 is SD_OK.
 * \retval 0         The SD card block length was set successfully.
 * \retval 0x1-0xFF  The SD card block length was failed to set, error status can refer to \ref T_SD_STATUS.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void sd_test(void)
 * {
 *     sd_set_block_len(SDHC_ID0, block_len);
 * }
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
T_SD_STATUS sd_set_block_len(uint8_t sdio_id, uint32_t block_len);

/**
 * sd.h
 *
 * \brief   Start or stop the SD host operation clock.
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in]  NewState   New state of the SD host operation clock, can be set to true or false.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_init(void)
 * {
 *    sd_sdh_clk_cmd(SDHC_ID0, true);
 * }
 *
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
void sd_sdh_clk_cmd(uint8_t sdio_id, bool NewState);

/**
 * sd.h
 *
 * \brief   Check if the SD card is in programming state.
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in]  None.
 *
 * \return The SD card check program status, 0 is SD_OK.
 * \retval 0         The SD card was checked successfully.
 * \retval 0x1-0xFF  The SD card was failed to check, error status can refer to \ref T_SD_STATUS.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_test(void)
 * {
 *     uint32_t sd_status = 0;
 *     sd_status = sd_check_program_status(SDHC_ID0);
 * }
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
T_SD_STATUS sd_check_program_status(uint8_t sdio_id);

/**
 * sd.h
 *
 * \brief   Get the device block size.
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in]  None.
 *
 * \return The block size of device.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void test(void)
 * {
 *     uint32_t block_size = 0;
 *     block_size = sd_get_dev_block_size(SDHC_ID0);
 * }
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
uint32_t sd_get_dev_block_size(uint8_t sdio_id);

/**
 * sd.h
 *
 * \brief   Get the device capacity in bytes.
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in]  None.
 *
 * \return The capacity of device.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void test(void)
 * {
 *     uint64_t capacity = 0;
 *     capacity = sd_get_dev_capacity(SDHC_ID0);
 * }
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
uint64_t sd_get_dev_capacity(uint8_t sdio_id);

/**
 * sd.h
 *
 * \brief   De-initialize the SD host and SD card. Use sd_init can be restored.
 * \xrefitem Experimental_Added_API_2_14_0_0 " Experimental Added Since 2.14.0.0" "Added API"
 *
 * \param[in]  None.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void test(void)
 * {
 *     sd_deinit(SDHC_ID0);
 * }
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
void sd_deinit(uint8_t sdio_id);

#ifdef __cplusplus
}
#endif

#endif /*__SD_H*/

/** @} */ /* End of group SD_Exported_Functions */
/** @} */ /* End of group SD */

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/

