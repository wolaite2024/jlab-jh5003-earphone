/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_XIAOAI_VOICE_H_
#define _APP_XIAOAI_VOICE_H_


#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*
  *                           Header Files
  *============================================================================*/

#include <stdint.h>
#include <stdbool.h>



/** @defgroup APP_RWS_XIAOAI App Xiaoai
  * @brief
  * @{
  */

#define BD_ADDR_LENGTH       6

typedef enum _T_XIAOAI_AUDIO_FORMAT
{
    XIAOAI_OPUS_16KHZ_16KBPS_CBR_0_20MS,

} T_XIAOAI_AUDIO_FORMAT;


void app_xiaoai_record_init(void);


void app_xiaoai_force_stop_recording(void);

/**
    * @brief        This function can stop the voice capture.
    * @return       void
    */
void app_xiaoai_stop_recording(uint8_t bd_addr[6]);

/**
    * @brief        This function can start the voice capture.
    * @return       void
    */
void app_xiaoai_start_recording(uint8_t bd_addr[6]);

bool app_xiaoai_is_recording(void);


/** @} End of APP_RWS_XIAOAI */

#ifdef __cplusplus
}
#endif

#endif //_APP_XIAOAI_VOICE_H_
