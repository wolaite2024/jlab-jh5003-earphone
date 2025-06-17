/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_XIAOWEI_RECORD_H_
#define _APP_XIAOWEI_RECORD_H_


#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*
  *                           Header Files
  *============================================================================*/

#include <stdint.h>
#include <stdbool.h>



/** @defgroup APP_RWS_XIAOWEI App Xiaowei
  * @brief
  * @{
  */

#define BD_ADDR_LENGTH       6

typedef enum _T_XIAOWEI_AUDIO_FORMAT
{
    XIAOWEI_OPUS_16KHZ_16KBPS_CBR_0_20MS,
    XIAOWEI_OPUS_8KHZ_32KBPS_CBR_0_10MS,

} T_XIAOWEI_AUDIO_FORMAT;


void app_xiaowei_record_init(uint8_t app_exp_bit_rate);


void app_xiaowei_force_stop_recording(void);

/**
    * @brief        This function can stop the voice capture.
    * @return       void
    */
void app_xiaowei_stop_recording(uint8_t bd_addr[6]);

/**
    * @brief        This function can start the voice capture.
    * @return       void
    */
void app_xiaowei_start_recording(uint8_t bd_addr[6]);

bool app_xiaowei_is_recording(void);


/** @} End of APP_RWS_XIAOWEI */

#ifdef __cplusplus
}
#endif

#endif //_APP_XIAOWEI_RECORD_H_
