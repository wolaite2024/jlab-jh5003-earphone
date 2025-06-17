/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      hal_def.h
* @brief
* @details
* @author
* @date
* @version   v1.1
* *********************************************************************************************************
*/

#ifndef _HAL_DEF_
#define _HAL_DEF_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup HAL_87x3d_HAL_DEF HAL DEF
 * @brief HAL define.
 * @{
 */

/** @defgroup HAL_DEF_Exported_Constants HAL DEF Exported Constants
  * @{
  */

#define CHIP_DMA_CHANNEL_NUM                 (16)
#define CHIP_I2C_NUM                         (3)

#define GPIOB_SUPPORT
#define GPIOC_SUPPORT

#ifndef IC_TYPE_RTL87x3D
#define IC_TYPE_RTL87x3D
#endif

/** End of group HAL_DEF_Exported_Constants
  * @}
  */

/** End of group HAL_87x3d_HAL_DEF
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _HAL_DEF_ */

