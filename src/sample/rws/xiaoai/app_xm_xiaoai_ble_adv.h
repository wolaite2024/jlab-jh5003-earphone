/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_XM_XIAOAI_BLE_ADV_H_
#define _APP_XM_XIAOAI_BLE_ADV_H_

#include <stdint.h>
#include <stdbool.h>
#include "ble_ext_adv.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_BLE_TTS_OTA App BLE TTS OTA
  * @brief App BLE Device
  * @{
  */

/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup APP_BLE_TTS_OTA_Exported_Macros App Ble TTS OTA Macros
   * @{
   */


#define APP_STOP_AMA_ADV_CAUSE_SPP_CONN  0x51
#define APP_STOP_AMA_ADV_CAUSE_DEV_DISC  0x52
/** End of APP_BLE_TTS_OTA_Exported_Macros
    * @}
    */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_BLE_TTS_OTA_Exported_Functions App Ble TTS OTA Functions
    * @{
    */

uint8_t app_xm_xiaoai_get_headset_state(void);

void app_xm_xiaoai_get_battery_state(uint8_t *p_data);

uint8_t app_xm_xiaoai_get_adv_count(void);

/**
    * @brief  start ble xiaomi xiaoai advertising
    * @param  duration_ms advertising duration time
    * @return true  Command has been sent successfully.
    * @return false Command was fail to send.
    */
bool le_xm_xiaoai_adv_start(uint16_t timeout_sec);

/**
    * @brief  stop ble xiaomi xiaoai advertising
    * @param  app_cause cause
    * @return true  Command has been sent successfully.
    * @return false Command was fail to send.
    */
bool le_xm_xiaoai_adv_stop(void);

bool le_xm_xiaoai_adv_ongoing(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_XM_XIAOAI_BLE_ADV_H_ */
