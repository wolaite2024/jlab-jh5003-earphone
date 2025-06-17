/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_XIAOWEI_BLE_ADV_H_
#define _APP_XIAOWEI_BLE_ADV_H_

#include <stdint.h>
#include <stdbool.h>
#include "ble_ext_adv.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_RWS_XIAOWEI App Xiaowei
  * @brief App XIAOWEI
  * @{
  */

/** @defgroup APP_XIAOWEI_ADV App xiaowei adv
  * @brief App xiaowei adv
  * @{
  */

/*============================================================================*
 *                              types
 *============================================================================*/
/** @defgroup APP_XIAOWEI_ADV_Exported_Types APP_XIAOWEI_ADV_Exported_Types
  * @{
  */
typedef struct XIAOWEI_ADV_DATA
{
    uint8_t len1;
    uint8_t type1_flags;
    uint8_t data1;

    uint8_t len2;
    uint8_t type2_service_uuid;
    uint8_t data2_uuid_lo_word;
    uint8_t data2_uuid_hi_word;

    uint8_t len3;
    uint8_t type3_manufacture_data;
    uint8_t company_id_lo_word;
    uint8_t company_id_hi_word;
    uint8_t product_id[4];
    uint8_t bt_mac_address[6];
    uint8_t xiaowei_char_value[8];
} T_XIAOWEI_ADV_DATA;

typedef struct XIAOWEI_SCAN_RSP_DATA
{

} T_XIAOWEI_SCAN_RSP_DATA;
/** End of APP_XIAOWEI_ADV_Exported_Types
  * @}
  */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_XIAOWEI_ADV_Exported_Functions APP_XIAOWEI_ADV_Exported_Functions
  * @{
  */

/**
  * @brief  start ble xiaowei advertising
  * @param  duration_s advertising duration time
  * @return true  Command has been sent successfully.
  * @return false Command was fail to send.
  */
bool le_xiaowei_adv_start(uint16_t timeout_sec);

/**
  * @brief  stop ble xiaowei advertising
  * @param  void
  * @return true  Command has been sent successfully.
  * @return false Command was fail to send.
  */
bool le_xiaowei_adv_stop(void);

bool le_xiaowei_adv_ongoing(void);

/** End of APP_XIAOWEI_ADV_Exported_Functions
  * @}
  */

/** End of APP_XIAOWEI_ADV
  * @}
  */

/** End of APP_RWS_XIAOWEI
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_XM_XIAOWEI_BLE_ADV_H_ */
