/**
*********************************************************************************************************
*               Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     cache.h
* @brief    This file provides cache API for sdk customer.
* @details
* @author   huan_yang
* @date
* @version  v1.0
*********************************************************************************************************
*/
/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __CACHE_H_
#define __CACHE_H_

/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdint.h>

/** @defgroup  HAL_CACHE    Cache
    * @brief Cache API.
    * @{
    */
/*============================================================================*
 *                              Variables
*============================================================================*/


/*============================================================================*
 *                              Functions
*============================================================================*/
/** @defgroup HAL_CACHE_Exported_Functions Cache Exported Functions
    * @brief
    * @{
    */
#ifdef __cplusplus
extern "C" {
#endif
/**
    * @brief Enable cache.
    * @return void
    */
void cache_enable(void);

/**
    * @brief Disable cache.
    * @return void
    */
void cache_disable(void);

/**
    * @brief Disable cache.
    * @warning This API is supported in RTL87x3E and RTL87x3G.
    *          It is NOT supported in RTL87x3D.
    * @return Refer to errno.h.
    */
int cache_flush_by_addr(uint32_t *addr, uint32_t length);

/**
    * @brief Cache init.
    * @param do_cache_flush Do cache flush or not.
    * @warning The param is supported in RTL87x3D.
    *          It is NOT supported in RTL87x3E and RTL87x3G.
    * @return void
    */
void cache_hit_init(bool do_cache_flush);

/**
 * @brief Print cache hit rate *100.
 *
 * @return void
*/
void cache_hit_print(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
/** @} */ /* End of group HAL_CACHE_Exported_Functions */
/** @} */ /* End of group HAL_CACHE */
#endif /* __CACHE_H_ */
