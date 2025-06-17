#ifndef _CODEC_INT_H_
#define _CODEC_INT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>

bool codec_cap_check(uint8_t len, uint8_t *p_data, T_CODEC_CFG *p_cfg);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
