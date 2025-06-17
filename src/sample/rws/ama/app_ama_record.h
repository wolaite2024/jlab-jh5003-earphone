/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_AMA_VOICE_H_
#define _APP_AMA_VOICE_H_


#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*
  *                           Header Files
  *============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "ama_api.h"



/** @defgroup APP_RWS_AMA App Ama
  * @brief
  * @{
  */

#define BD_ADDR_LENGTH       6


void app_ama_record_init(AudioFormat format);


void app_ama_record_force_stop_recording(void);

/**
    * @brief        This function can stop the voice capture.
    * @return       void
    */
void app_ama_record_stop_recording(uint8_t bd_addr[6]);

/**
    * @brief        This function can start the voice capture.
    * @return       void
    */
void app_ama_record_start_recording(uint8_t bd_addr[6]);


/** @} End of APP_RWS_AMA */

#ifdef __cplusplus
}
#endif

#endif //_VOICE_SPP_H_
