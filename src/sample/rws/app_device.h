/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DEVICE_H_
#define _APP_DEVICE_H_
#include <stdbool.h>
#include <stdint.h>
#include <rtl876x_wdg.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_DEVICE App Device
  * @brief App Device
  * @{
  */
/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup APP_DEVICE_Exported_Macros App Device Macros
    * @{
    */

/** End of APP_DEVICE_Exported_Macros
    * @}
    */

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup APP_DEVICE_Exported_Types App Device Types
    * @{
    */
#if F_APP_TEAMS_BT_POLICY
extern uint8_t temp_master_device_name[40];
extern uint8_t temp_master_device_name_len;
#endif

#define APP_DEVICE_IPC_TOPIC "DEVICE"

typedef enum
{
    APP_DEVICE_STATE_OFF       = 0x00,
    APP_DEVICE_STATE_ON        = 0x01,
    APP_DEVICE_STATE_OFF_ING   = 0x02,
} T_APP_DEVICE_STATE;

typedef enum
{
    APP_DEVICE_IPC_EVT_STACK_READY          = 0x00,
    APP_DEVICE_IPC_EVT_FACTORY_RESET        = 0x01,
    APP_DEVICE_IPC_EVT_HFP_CALL_STATUS      = 0x02,
    APP_DEVICE_IPC_EVT_LEA_CCP_CALL_STATUS  = 0x03,
    APP_DEVICE_IPC_EVT_LEA_BCA_STREAM       = 0x04,
    APP_DEVICE_IPC_EVT_LEA_CIS_STREAM       = 0x05,
    APP_DEVICE_IPC_EVT_MIC_MUTE_STATUS      = 0x06,
    APP_DEVICE_IPC_EVT_A2DP_PLAY_STATUS     = 0x07,
    APP_DEVICE_IPC_EVT_B2S_LINK_NUM         = 0x08,
    APP_DEVICE_IPC_EVT_BT_POLICY_STATE      = 0x09,
    APP_DEVICE_IPC_EVT_LISTENING_STATE      = 0x0A,
    APP_DEVICE_IPC_EVT_GAMING_MODE          = 0x0B,
    APP_DEVICE_IPC_EVT_DONGLE_DUAL_MODE     = 0x0C,
    APP_DEVICE_IPC_EVT_CHARGING_STATE       = 0x0D,
    APP_DEVICE_IPC_EVT_REMOTE_CONNECTED     = 0x0E,
    APP_DEVICE_IPC_EVT_BT_MODE_STATE        = 0x0F,
} T_APP_DEVICE_IPC_EVT;

typedef enum
{
    FACTORY_RESET_CLEAR_CFG     = 0x01,
    FACTORY_RESET_CLEAR_PHONE   = 0x02,
    FACTORY_RESET_CLEAR_NORMAL  = 0x03,
    FACTORY_RESET_CLEAR_ALL     = 0x04,
} T_FACTORY_RESET_CLEAR_MODE;

typedef enum
{
    APP_TONE_VP_STOP    = 0x00,
    APP_TONE_VP_STARTED = 0x01,
} T_APP_TONE_VP_STATE;

typedef enum
{
    APP_REMOTE_MSG_SYNC_DB                      = 0x00,
    APP_REMOTE_MSG_SYNC_IO_PIN_PULL_HIGH        = 0x01,
    APP_REMOTE_MSG_LINK_RECORD_ADD              = 0x02,
    APP_REMOTE_MSG_LINK_RECORD_DEL              = 0x03,
    APP_REMOTE_MSG_LINK_RECORD_XMIT             = 0x04,
    APP_REMOTE_MSG_LINK_RECORD_PRIORITY_SET     = 0x05,
    APP_REMOTE_MSG_DEVICE_NAME_SYNC             = 0x06,
    APP_REMOTE_MSG_SET_LPS_SYNC                 = 0x07,
    APP_REMOTE_MSG_LE_NAME_SYNC                 = 0x08,
    APP_REMOTE_MSG_POWERING_OFF                 = 0x09,
    APP_REMOTE_MSG_REMOTE_SPK2_PLAY_SYNC        = 0x0a,
    APP_REMOTE_MSG_SPK1_REPLY_SRC_IS_IOS        = 0x0b,
    APP_REMOTE_MSG_SPATIAL_AUDIO                = 0x0c,

    APP_REMOTE_MSG_DEVICE_TOTAL
} T_DEVICE_REMOTE_MSG;

typedef struct
{
    T_APP_TONE_VP_STATE state;
    uint8_t     index;
} T_APP_TONE_VP_STARTED;

typedef struct test_equipment_info
{
    uint8_t oui[3];
    const char *name;
} T_APP_TEST_EQUIPMENT_INFO;

typedef struct
{
    bool remote_is_dongle;
} T_ROLESWAP_APP_DB;


/** End of APP_DEVICE_Exported_Types
    * @}
    */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_DEVICE_Exported_Functions App Device Functions
    * @{
    */
/* @brief  app device module init.
*
* @param  void
* @return none
*/
void app_device_init(void);

/* @brief  app device reboot.
*
* @param  timeout_ms timeout value to do system reboot
* @param  wdg_mode Watch Dog Mode
* @return none
*/
void app_device_reboot(uint32_t timeout_ms, T_WDG_MODE wdg_mode);

void app_device_factory_reset(void);

/* @brief  check device inside/outside box state
*
* @param  void
* @return true/false
*/
bool app_device_is_in_the_box(void);

/* @brief  check device inside/outside ear state
*
* @param  void
* @return true/false
*/
bool app_device_is_in_ear(uint8_t loc);

/* @brief  change device state
*
* @param  state @ref T_APP_DEVICE_STATE
* @return none
*/
void app_device_state_change(T_APP_DEVICE_STATE state);

/* @brief  disconnect_all_link and enter dut mode
*
* @param  none
* @return none
*/
void app_device_enter_dut_mode(void);

/* @brief  get bud physical channel
*
* @param  void
* @return physical channel
*/
uint8_t app_device_get_bud_channel(void);

/* @brief  start up bt policy
*
* @param  at_once_trigger
* @return none
*/
void app_device_bt_policy_startup(bool at_once_trigger);

/** @} */ /* End of group APP_DEVICE_Exported_Functions */


/** End of APP_DEVICE
* @}
*/

/**
    * @brief  unlock vbat disallow power on
    * @param  void
    * @return void
    */
void app_device_unlock_vbat_disallow_power_on(void);

#if F_APP_COMMON_DONGLE_SUPPORT
bool app_device_has_dongle_record(void);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_DEVICE_H_ */
