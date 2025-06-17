#ifndef _STDLIB_CORECRT_H
#define _STDLIB_CORECRT_H
#include <stdlib.h>
/** @defgroup  HAL_STDLIB_CORECRT    Stdlib Corecrt API
    * @brief Stdlib Corecrt API.
    * @{
    */

/** @defgroup HAL_Stdlib_Corecrt_Exported_Functions Stdlib Corecrt Exported Functions
    * @brief
    * @{
    */
#ifdef __cplusplus
extern "C" {
#endif
/**
    * @brief  memcpy_s with security check mechanism
    * @param  dest destination buffer pointer
    * @param  destsz the size of destination buffer
    * @param  src   source buffer pointer
    * @param  count source data size to memcpy
    * @return The error code.
    * @retval 0      Success.
    * @retval others   Error code.
    */
extern int memcpy_s(void *dest, size_t destsz, const void *src, size_t count);
/**
    * @brief  memset_s with security check mechanism
    * @param  dest destination buffer pointer
    * @param  destsz the size of destination buffer
    * @param  ch   memory set value
    * @param  count data size to memset with ch
    * @return The error code.
    * @retval 0      Success.
    * @retval others   Error code.
    */
int memset_s(void *dest, size_t destsz, int ch, size_t count);
#ifdef __cplusplus
}
#endif
/** @} */ /* End of group HAL_Stdlib_Corecrt_Exported_Functions */
/** @} */ /* End of group HAL_STDLIB_CORECRT */
#endif /* stdlib_corecrt.h */
