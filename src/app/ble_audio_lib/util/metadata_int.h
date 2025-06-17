#ifndef _METADATA_INT_H_
#define _METADATA_INT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>

bool metadata_check_format(uint16_t len, uint8_t *p_data, uint8_t *p_resp_code,
                           uint8_t *p_reason, uint16_t *p_audio_contexts);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
