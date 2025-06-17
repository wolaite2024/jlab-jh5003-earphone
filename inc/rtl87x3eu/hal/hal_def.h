/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      hal_def.h
* @brief
* @details
* @author
* @date
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _HAL_DEFINE_
#define _HAL_DEFINE_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup HAL_87x3e_HAL_DEF HAL DEF
 * @{
 */

#define CHIP_DMA_CHANNEL_NUM                 (16)
#define CHIP_I2C_NUM                         (3)

#ifndef IC_TYPE_RTL87x3EU
#define IC_TYPE_RTL87x3EU
#endif

/** End of group HAL_87x3e_HAL_DEF
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _HAL_DEFINE_ */

