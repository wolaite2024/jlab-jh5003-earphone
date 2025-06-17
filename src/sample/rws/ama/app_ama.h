/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_AMA_H_
#define _APP_AMA_H_

#include "remote.h"
#include "ama_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_RWS_AMA App Ama
  * @brief App Ama
  * @{
  */

#define APP_AMA_VAD_SUPPORT     0


/**
    * @brief  ama module init
    * @param  void
    * @return void
    */
void app_ama_init(void);


void app_ama_va_start(void);


void app_ama_reset(uint8_t bd_addr[6]);


bool app_ama_bond_exist(uint8_t *bd_addr);

/** End of APP_RWS_AMA
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_AMA_H_ */
