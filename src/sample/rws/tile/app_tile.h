/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_BLE_TILE_CLIENT_H_
#define _APP_BLE_TILE_CLIENT_H_

#include <stdint.h>
#include "tile_tmd_module.h"
#include "btm.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_BLE_TILE_CLIENT App Ble TILE Client
  * @brief App Ble TILE Client
  * @{
  */

/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup APP_BLE_TILE_CLIENT_Exported_Macros App Ble Client Macros
   * @{
   */

#define TILE_ADV_INTERVAL_SHORT     0x140       // 200(ms)
#define TILE_ADV_INTERVAL_LONG      0xC80       // 2000(ms)

/** End of APP_BLE_TILE_CLIENT_Exported_Macros
    * @}
    */

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup APP_BLE_TILE_CLIENT_Exported_Types App Ble Client Types
    * @{
    */

/** End of APP_BLE_TILE_CLIENT_Exported_Types
    * @}
    */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_BLE_TILE_CLIENT_Exported_Functions App Ble Client Functions
    * @{
    */
void app_tile_adv_init(void);
void app_tile_update_adv_content(uint8_t tile_mode_input);
void app_tile_stop_adv(void);
void app_tile_handle_role_swap(T_BT_EVENT_PARAM_REMOTE_ROLESWAP_STATUS *p_role_swap_status);
void app_tile_change_adv_interval(uint16_t adv_interval);
void app_tile_shpping_adv_enable(void);
void app_tile_active_adv_enable(void);
void app_tile_disable(void);
void app_tile_set_mode(T_TILE_BUD_TO_SRC_STATE mode);
void app_tile_init(void);

/** @} */ /* End of group APP_BLE_TILE_CLIENT_Exported_Functions */


/** End of APP_BLE_TILE_CLIENT
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_BLE_TILE_CLIENT_H_ */
