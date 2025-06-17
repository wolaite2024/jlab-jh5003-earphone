/**
****************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
****************************************************************************************************
* @file      ftl.h
* @brief     Flash transport layer is used as an abstraction layer for user applications to save read/write
*            parameters in flash.
* @note      FTL is a dedicated block in flash, used for saving data. If the value is only for one-time read,
*            refer to fmc APIs.
* @author    Brenda_li
* @date      2016-12-27
* @version   v1.0
* **************************************************************************************************
*/

#ifndef _FTL_H_
#define _FTL_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "errno.h"

#ifdef  __cplusplus
extern  "C" {
#endif  // __cplusplus


/** @defgroup  HAL_FTL    Flash Transport Layer
    * @brief Simple implementation of file system for flash.
    * @{
    */

/**
 *
 * @section FTL_Usage_Chapter How to use this module
 *
 * - Init FTL. \n
 *   - step1: Init FTL Pool.
 *   - step2: Set FTL cache info if needed.
 *   - Sample code:
 *   @code

        #define FTL_ADDR                        (0x023DE000)
        #define FTL_SIZE                        (0x00015000)  //84K Bytes
        #define ROM_FTL_SIZE                    (0x2000)    //8K Bytes

        void ftl_init_demo(void)
        {
            //set FTL Pool info
            ftl_pool_init(FTL_ADDR, FTL_SIZE, ROM_FTL_SIZE);

            //enable the FTL cache
            uint32_t cache_size = 128;
            ftl_cache_init(cache_size);
        }
 *
 *   @endcode
 *
 *
 * - Default FTL module read and write \n
 *   - step1: Save data to the default FTL module.
 *   - step2: Load data from the default FTL module.
 *   - Sample code:
 *   @code
        void ftl_default_module_demo(void)
        {
            // Save data to the default module
            uint8_t data_buf[4];
            memset(data_buf, 0x5A, 4);
            uint16_t test_offset = 0x1000;

            uint32_t ret = ftl_save_to_storage(data_buf, test_offset, 4);
            if (ret != ESUCCESS)
            {
                //save data error
                return;
            }

            // Load data from the default module.
            uint8_t read_buf[4];
            ret = ftl_load_from_storage(read_buf, test_offset, 4);
            if (ret != ESUCCESS)
            {
                // Load data error
                return;
            }
        }
 *
 *   @endcode
 *
 *
 *  - Ext FTL module access. \n
 *   - Step 1: Init an ext FTL module.
 *   - Step 2: Save data to the ext FTL module.
 *   - Step 3: Load data from the ext FTL module.
 *   - Sample code:
 *   @code

        #define EXT_FTL_NAME "TEST_FTL"
        #define EXT_FTL_LOGIC_SIZE     (0x1000)
        #define EXT_FTL_BLOCK_SIZE     (64)

        void ftl_ext_module_demo(void)
        {
            // Init an ext FTL module
            ftl_init_module(EXT_FTL_NAME, EXT_FTL_LOGIC_SIZE, EXT_FTL_BLOCK_SIZE);

            // Save data
            uint8_t data_buf[EXT_FTL_BLOCK_SIZE];
            memset(data_buf, 0x5A, EXT_FTL_BLOCK_SIZE);
            uint16_t test_offset = 0x800;

            uint32_t ret = ftl_save_to_module(EXT_FTL_NAME, data_buf, test_offset, EXT_FTL_BLOCK_SIZE);
            if (ret != ESUCCESS)
            {
                // Save data error
                return;
            }

            // Load data
            uint8_t read_buf[EXT_FTL_BLOCK_SIZE];
            ret = ftl_load_from_storage(read_buf, test_offset, EXT_FTL_BLOCK_SIZE);
            if (ret != ESUCCESS)
            {
                // Load data error
                return;
            }
        }
 *
 *   @endcode
 */

/*============================================================================*
  *                                   Macros
  *============================================================================*/
/** @defgroup FTL_Exported_Macros Flash Transport Layer Exported Macros
    * @brief
    * @{
    */
#define FTL_PARTITION_NAME "FTL"

#define FTL_POOL_ENABLE 1    /* Indicate that the current version supports FTL POOL, to facilitate compatibility handling for users */


/** End of FTL_Exported_Macros
    * @}
    */
/*============================================================================*
  *                                   Types
  *============================================================================*/


/*============================================================================*
  *                                Functions
  *============================================================================*/
/** @defgroup FTL_Exported_Functions Flash Transport Layer Exported Functions
    * @brief
    * @{
    */
/**
    * @brief    Save specified value to specified FTL offset.
    * @param    pdata  Specify data buffer.
    * @param    offset Specify FTL offset to store.
    * @param    size   Size to store.
    * @return   status, refer to errno.h
    * @retval   0  Successful.
    * @retval   Otherwise Fail.
    * @note     FTL offset is pre-defined and no conflict with ROM.
    */
extern uint32_t(*ftl_save_to_storage)(void *pdata, uint16_t offset, uint16_t size);

/**
    * @brief    Load specified FTL offset parameter to specified buffer.
    * @param    pdata  Specify data buffer.
    * @param    offset Specify FTL offset to load.
    * @param    size   Size to load.
    * @return   status, refer to errno.h
    * @retval   0  Successful.
    * @retval   Otherwise Fail.
    * @note     FTL offset is pre-defined and no conflict with ROM.
    */
extern uint32_t(*ftl_load_from_storage)(void *pdata, uint16_t offset, uint16_t size);

/**
 * @brief FTL pool init
 *
 * @param ftl_pool_startAddr The start address of the FTL pool, must be 4K aligned.
 * @param ftl_pool_size The size of the FTL Pool, must be 4K aligned. And it could not be less than 12K.
 * @param default_module_logicSize The logical size of the default FTL module, must be larger than 0xC00.
 * @return FTL pool init result. Refer to errno.h
 * @retval 0 Init successful.
 * @retval Otherwise Fail.
 */
int32_t ftl_pool_init(uint32_t ftl_pool_startAddr, uint32_t ftl_pool_size,
                      uint16_t default_module_logicSize);

/**
    * @brief    Init an ext FTL module.
    * @param    module_name  Specify FTL module name, and the first 4 bytes must be unique.
    * @param    malloc_size  FTL module logical size.
    * @param    block_len    The minimum access unit for the FTL module must be an integral multiple of 4 and cannot exceed 128.
    * @return   status
    * @retval   0 Successful.
    * @retval   Otherwise Fail.
    * @note     Up to 6 modules can be applied.
    */
int32_t ftl_init_module(char *module_name, uint16_t malloc_size, uint8_t block_len);

/**
    * @brief    Set V1 FTL information for the FTL upgrade.
    * @param    module_name  V1 ext FTL name.
    * @param    u32PageStartAddr  V1 FTL ext start address.
    * @param    pagenum  V1 ext FTL pagenum.
    * @param    block_len  V1 ext FTL value size.
    * @param    offset_min  V1 ext FTL min offset.
    */
void ftl_v1_module_info(char *module_name, uint32_t u32PageStartAddr, uint8_t pagenum,
                        uint32_t block_len, uint16_t offset_min);

/**
    * @brief    Save specified value to FTL module.
    * @param    module_name  Specify FTL module name.
    * @param    pdata  Specify data buffer.
    * @param    offset  FTL offset to store.
    * @param    size   Size to store, needs to be an integer multiple of block_len in ftl_init_module.
    * @return   status
    * @retval   0  Successful.
    * @retval   Otherwise Fail.
    */
int32_t ftl_save_to_module(char *module_name, void *pdata, uint16_t offset, uint16_t size);

/**
    * @brief    Load specified FTL module offset parameter to specified buffer.
    * @param    module_name  Specify FTL module name.
    * @param    pdata  Specify data buffer.
    * @param    offset  Specify FTL offset to load, needs to be an integer multiple of block_len in ftl_init_module.
    * @param    size   Size to load.
    * @return   status
    * @retval   0 Successful.
    * @retval   Otherwise Fail.
    */
int32_t ftl_load_from_module(char *module_name, void *pdata, uint16_t offset, uint16_t size);

/**
    * @brief    Init FTL cache and set FTL cache enable.
    * @param    cache_size  FTL cache size.
    * @return   status
    * @retval   true FTL cache init successfully.
    * @retval   false Fail.
    */
bool ftl_cache_init(uint16_t cache_size);

/** @} */ /* End of group FTL_Exported_Functions */

/** @} */ /* End of group HAL_FTL */

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // _FTL_H_



