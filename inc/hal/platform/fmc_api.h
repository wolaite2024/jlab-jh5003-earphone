/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file    fmc_api.h
* @brief   This file provides fmc API wrapper for SDK customers.
* @author  yuhungweng
* @date    2020-11-26
* @version v1.0
* *************************************************************************************
*/

#ifndef __FMC_API_H_
#define __FMC_API_H_

#include <stdint.h>
#include <stdbool.h>

/** @defgroup  FMC    FMC API
    * @brief FMC API wrapper.
    * @{
    */

/*============================================================================*
  *                                   Types
  *============================================================================*/
/** @defgroup FMC_TYPE FMC Exported Types
   * @brief
   * @{
   */
typedef enum
{
    FMC_SPIC_ID_0,
    FMC_SPIC_ID_1,
    FMC_SPIC_ID_2,
    FMC_SPIC_ID_3,
    FMC_SPIC_ID_4,
    FMC_SPIC_ID_MAX,
} FMC_SPIC_ID;

typedef enum
{
    FMC_FLASH_NOR_ERASE_CHIP   = 1,
    FMC_FLASH_NOR_ERASE_SECTOR = 2,
    FMC_FLASH_NOR_ERASE_BLOCK  = 4,
} FMC_FLASH_NOR_ERASE_MODE;

typedef enum
{
    PARTITION_FLASH_OCCD,
    PARTITION_FLASH_OTA_BANK_0,
    PARTITION_FLASH_OTA_BANK_1,
    PARTITION_FLASH_OTA_TMP,
    PARTITION_FLASH_BKP_DATA1,
    PARTITION_FLASH_BKP_DATA2,
    PARTITION_FLASH_FTL,
    PARTITION_FLASH_HARDFAULT_RECORD,

    PARTITION_FLASH_TOTAL,

} T_FLASH_PARTITION_NAME;

typedef enum _FLASH_IMG_ID
{
    FLASH_IMG_OTA         = 0, /* OTA header */
    FLASH_IMG_SBL         = 1,
    FLASH_IMG_MCUPATCH    = 2,
    FLASH_IMG_MCUAPP      = 3,
    FLASH_IMG_DSPSYSTEM   = 4,
    FLASH_IMG_DSPAPP      = 5,
    FLASH_IMG_MCUCONFIG   = 6,
    FLASH_IMG_DSPCONFIG   = 7,
    FLASH_IMG_ANC         = 8,
    FLASH_IMG_EXT1        = 9,
    FLASH_IMG_EXT2        = 10,
    FLASH_IMG_ADSP        = 11,
    FLASH_IMG_SYSPATCH    = 12,
    FLASH_IMG_STACKPATCH  = 13,
    FLASH_IMG_UPPERSTACK  = 14,
    FLASH_IMG_FRAMEWORK   = 15,
    FLASH_IMG_SYSDATA     = 16,
    FLASH_IMG_VP          = 17,
    FLASH_IMG_TONE        = 18,
    FLASH_IMG_MAX,
} FLASH_IMG_ID;

typedef void (*FMC_FLASH_NOR_ASYNC_CB)(void);

/** End of FMC_TYPE
    * @}
    */

/*============================================================================*
  *                                   Macros
  *============================================================================*/
/** @defgroup FMC_Exported_Macros FMC Exported Macros
    * @brief
    * @{
    */
#define FMC_SEC_SECTION_LEN 0x1000
#define FMC_SEC_BLOCK_LEN   0x10000

#define IS_FLASH_SIZE_LARGER_THAN_2MB()            (fmc_flash_get_flash_size(FMC_SPIC_ID_0) > (1 << 21))
/** End of FMC_Exported_Macros
    * @}
    */

/*============================================================================*
  *                                Functions
  *============================================================================*/
/** @defgroup FMC_Exported_Flash_Functions FMC Exported Flash Functions
    * @brief
    * @{
    */

/**
 * @brief           Task-safe NOR flash read.
 * @param addr      The RAM address mapping of NOR flash going to be read.
 * @param data      Data buffer to be read into.
 * @param len       Read data length.
 * @return          True if read successful, otherwise false.
 */
bool fmc_flash_nor_read(uint32_t addr, void *data, uint32_t len);

/**
 * @brief           Task-safe NOR flash write.
 * @param addr      The RAM address mapping of NOR flash going to be written.
 * @param data      Data buffer to be written into.
 * @param len       Write data length.
 * @return          True if write successful, otherwise false.
 */
bool fmc_flash_nor_write(uint32_t addr, void *data, uint32_t len);

/**
 * @brief           Task-safe NOR flash erase.
 * @param addr      The RAM address mapping of NOR flash going to be erased.
 * @param mode      Erase mode defined as @ref FMC_FLASH_NOR_ERASE_MODE.
 * @return          True if erase successful, otherwise false.
 */
bool fmc_flash_nor_erase(uint32_t addr, FMC_FLASH_NOR_ERASE_MODE mode);

/**
 * @brief           Task-safe NOR flash auto DMA read.
 * @param src       The RAM address mapping of NOR flash going to be read from.
 * @param dst       The RAM address going to be written to.
 * @param len       DMA data length.
 * @param cb        Callback function which is to be executed when DMA finishing @ref FMC_FLASH_NOR_ASYNC_CB.
 * @return          True if triggering auto DMA read successful, otherwise false.
 */
bool fmc_flash_nor_auto_dma_read(uint32_t src, uint32_t dst, uint32_t len,
                                 FMC_FLASH_NOR_ASYNC_CB cb);

/**
 * @brief           Get the address of a flash partition.
 * @param name      A partition name in @ref T_FLASH_PARTITION_NAME.
 * @return          The address of the flash partition.
 */
uint32_t flash_partition_addr_get(T_FLASH_PARTITION_NAME name);

/**
 * @brief           Get the size of a flash partition.
 * @param name      A partition in @ref T_FLASH_PARTITION_NAME.
 * @return          The size of the flash partition.
 */
uint32_t flash_partition_size_get(T_FLASH_PARTITION_NAME name);

/**
 * @brief           Get the payload address of an image.
 * @param id        An image ID in @ref FLASH_IMG_ID.
 * @return          The payload address of the image.
 */
uint32_t flash_cur_bank_img_payload_addr_get(FLASH_IMG_ID id);

/**
 * @brief           Get the header address of the image in the current bank.
 * @param id        An image ID in @ref FLASH_IMG_ID.
 * @return          The header address of the image.
 */
uint32_t flash_cur_bank_img_header_addr_get(FLASH_IMG_ID id);

/**
 * @brief           Task-safe NOR flash set block protection level.
 * @param addr      The RAM address mapping of NOR flash.
 * @param bp_lv     The block protection level to be set.
 * @return          True if setting block protection level is successful, otherwise false.
 */
bool fmc_flash_nor_set_bp_lv(uint32_t addr, uint8_t bp_lv);

/**
 * @brief           Get NOR flash block protect level.
 * @param addr      The RAM address mapping of NOR flash.
 * @param bp_lv     Used to pass back the block protect level.
 * @return          True if getting BP level is successful, otherwise false.
 */
bool fmc_flash_nor_get_bp_lv(uint32_t addr, uint8_t *bp_lv);

/**
 * @brief           Get flash size with idx.
 *\xrefitem         Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
 * @param idx       SPIC channel number.
 * @return          Byte size of current flash.
 */
uint32_t fmc_flash_get_flash_size(FMC_SPIC_ID idx);

/** @} */ /* End of group FMC_Exported_Flash_Functions */



/** @defgroup FMC_Exported_PSRAM_Functions FMC Exported PSRAM Functions
    * @brief
    * @{
    */



/**
 * @brief           Initialize Winbond OPI PSRAM (W955D8MBYA).
 * @param spic_id   SPIC channel number.
 * @return          True if initialization is successful, otherwise false.
 */
bool fmc_psram_winbond_opi_init(FMC_SPIC_ID spic_id);

/**
 * @brief           Initialize APMemory QPI PSRAM (APS1604M-SQR) which only supports quad mode.
 * @param spic_id   SPIC channel number.
 * @return          True if initialization is successful, otherwise false.
 */
bool fmc_psram_ap_memory_qpi_init(FMC_SPIC_ID spic_id);

/**
 * @brief           Initialize APMemory OPI PSRAM (APS6408L-OBx).
 * @param spic_id   SPIC channel number.
 * @return          True if initialization is successful, otherwise false.
 */
bool fmc_psram_ap_memory_opi_init(FMC_SPIC_ID spic_id);

/** @} */ /* End of group FMC_Exported_PSRAM_Functions */

/** @} */ /* End of group FMC */

#endif  // __FMC_API_H__



