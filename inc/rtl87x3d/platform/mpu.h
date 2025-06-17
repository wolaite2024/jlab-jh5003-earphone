/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    mpu.h
  * @brief   configure mpu
  * @date    2017.6.6
  * @version v1.0
  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef _MPU_H_
#define _MPU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"
/** @defgroup 87x3d_MEM_CONFIG Memory Configure
  * @{
  */

/*============================================================================*
 *                               Types
*============================================================================*/
/** @defgroup MEM_CONFIG_Exported_Types Memory Configure Exported Types
  * @{
  */

int mpu_init(const uint32_t *mpu_cfg_rbar, const uint32_t *mpu_cfg_rasr, uint8_t num);
int mpu_set_region(uint32_t mpu_cfg_rbar, uint32_t mpu_cfg_rasr, uint8_t region_num,
                   bool is_set);


/** @} */ /* End of group MEM_TYPES_Exported_Types */

/** @} */ /* End of group 87x3d_MEM_CONFIG */


#ifdef __cplusplus
}
#endif

#endif /* _MEM_TYPES_H_ */
