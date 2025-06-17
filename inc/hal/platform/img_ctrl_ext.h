/**
*********************************************************************************************************
*               Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     img_ctrl_ext.h
* @brief    This file provides image control related API.
* @details
* @author
* @date
* @version  v1.0
*********************************************************************************************************
*/
#ifndef __IMG_CTRL_EXT_H__
#define __IMG_CTRL_EXT_H__

#include <stddef.h>
#include "patch_header_check.h"

/** @defgroup  HAL_IMG_CTRL_EXT    Image Control Extention
    * @brief Image control extension.
    * @{
    */

/** @defgroup HAL_IMG_CTRL_EXT_EXPORTED_FUNCTIONS Image Control Extention Exported Functions
    * @brief
    * @{
    */
#ifdef __cplusplus
extern "C" {
#endif
/**
    * @brief  Get the image feature information.
    * \xrefitem Added_API_2_12_0_0 "Added Since 2.12.0.0" "Added API"
    * @note   Get the image feature field(16 bytes) from the image header.
    * @param  img_id  Specify the image id to get the image feature.
    * @param  p_feature_buf Specify the buffer to save the image feature.
    * @param  feature_buf_len Specify the buffer length of p_feature_buf.
    * @return The result of getting image feature information.
    * @retval 0     Success.
    */
int get_image_feature_info(IMG_ID img_id, uint8_t *p_feature_buf, uint8_t feature_buf_len);
/**
    * @brief  Get the image release version.
    * \xrefitem Added_API_2_12_0_0 "Added Since 2.12.0.0" "Added API"
    * @note   Get the image release version field(8 bytes) from the image header.
    * @param  img_id  Specify the image id to get the image release version.
    * @return The image release version, data type: unsigned long long.
    */
uint64_t get_image_release_version(IMG_ID img_id);

/**
    * @brief  Print the hal library version with commit id, build time and builder.
    * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Added API"
    */
void hal_ver_print(void);
#ifdef __cplusplus
}
#endif
/** @} */ /* End of group HAL_IMG_CTRL_EXT_EXPORTED_FUNCTIONS */
/** @} */ /* End of group HAL_IMG_CTRL_EXT */
#endif
