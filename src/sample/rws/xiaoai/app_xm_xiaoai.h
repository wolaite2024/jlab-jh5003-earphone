
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_XM_XIAOAI_H_
#define _APP_XM_XIAOAI_H_

#include <stdint.h>
#include <stdbool.h>
#include <gap_msg.h>

#ifdef __cplusplus
extern "C"  {
#endif

void app_xm_xiaoai_init(void);
void app_xm_xiaoai_handle_conn_state(uint8_t conn_id, T_GAP_CONN_STATE new_state);


#ifdef __cplusplus
}
#endif

#endif

