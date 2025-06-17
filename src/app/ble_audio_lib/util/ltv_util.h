#ifndef _LTV_UTIL_H_
#define _LTV_UTIL_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>


#define ltv_util_check_format(len, p_data) ltv_util_check_format_int(__func__, len, p_data);
bool ltv_util_check_format_int(const char *p_func_name, uint16_t len, uint8_t *p_data);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
