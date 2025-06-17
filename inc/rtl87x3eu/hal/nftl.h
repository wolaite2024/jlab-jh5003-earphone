#ifndef _NFTL_H_
#define _NFTL_H_

#include <stdint.h>
#include <stdbool.h>
#include "errno.h"

#ifdef  __cplusplus
extern  "C" {
#endif  // __cplusplus


/** @defgroup  87x3e_FTL    Flash Transport Layer
    * @brief simple implementation of file system for flash
    * @{
    */

/*============================================================================*
  *                                   Macros
  *============================================================================*/
/** @defgroup 87x3e_FTL_Exported_Macros Flash Transport Layer Exported Macros
    * @brief
    * @{
    */

/** @defgroup 87x3e_FTL_IO_CTL_CODE Flash Transport Layer ioctrl code
 * @{
 */

/**
  * @}
  */


/** End of FTL_Exported_Macros
    * @}
    */
/*============================================================================*
  *                                   Types
  *============================================================================*/

/*============================================================================*
  *                                Functions
  *============================================================================*/
bool nftl_init(uint32_t address, uint32_t size);

bool nftl_module_init(char *module_name, uint32_t logic_size);
uint32_t nftl_module_write(char *module_name, uint32_t offset, void *pdata, uint32_t size);
uint32_t nftl_module_read(char *module_name, uint32_t offset, void *pdata, uint32_t size);

/** @} */ /* End of group 87x3e_FTL_Exported_Functions */

/** @} */ /* End of group 87x3e_FTL */

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // _FTL_H_
