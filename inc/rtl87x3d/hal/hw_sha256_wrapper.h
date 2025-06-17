/**
*****************************************************************************************
*     Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file    hw_sha256_wrapper.h
* @brief   This file provides hw SHA26 DMA API for flash data.
* @author
* @date
* @version  v1.0
* *************************************************************************************
*/
#ifndef __HW_SHA256_WRAPPER_H_
#define __HW_SHA256_WRAPPER_H_
/** @defgroup  HAL_87x3D_HW_SHA256_WRAPPER    HW SHA256 Wrapper API
    * @brief HW SHA256 wrapper API.
    * @{
    */
/*============================================================================*
  *                                Functions
  *============================================================================*/
/** @defgroup HAL_87x3D_HW_SHA26_WRAPPER_FUNCTIONS HW SHA256 Wrapper Exported Functions
    * @brief
    * @{
    */
/**
 * @brief           Calculate the SHA256 for flash data through HW SHA256 by dma.
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Added API"
 * @param[in] src_flash_addr   The flash address to calculate the SHA256 value.
 * @param[in] len              The flash data length to calculate the SHA256 value.
 * @param[out] result      Pointer to the 32 bytes SHA256 result buffer.
 * @return          The result of calculating SHA256.
 * @retval true     Success.
 * @retval false    Fail.
 */
bool hw_sha256_flash_data_by_dma(uint32_t src_flash_addr,  uint32_t len, uint32_t *result);

/** @} */ /* End of group HAL_87x3D_HW_SHA26_WRAPPER_FUNCTIONS */

/** @} */ /* End of group HAL_87x3D_HW_SHA256_WRAPPER */
#endif
