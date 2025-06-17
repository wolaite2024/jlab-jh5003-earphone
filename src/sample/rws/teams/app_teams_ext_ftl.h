/**
 * @file   app_teams_ext_ftl.h
 * @brief  struct and interface about teams rws for app
 * @author leon
 * @date   2022.5.16
 * @version 1.0
 * @par Copyright (c):
         Realsil Semiconductor Corporation. All rights reserved.
 */


#ifndef _APP_TEAMS_EXT_FTL_H_
#define _APP_TEAMS_EXT_FTL_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if F_APP_TEAMS_FEATURE_SUPPORT

#if (FTL_POOL_ENABLE == 0)
extern uint16_t app_teams_ext_ftl_start_offset;
extern uint16_t app_teams_ext_ftl_max_offset;
#endif

#define APP_TEAMS_BT_COD_INFO_SIZE          512
#define APP_TEAMS_BLE_DEV_BOND_INFO_SIZE    512
#define APP_TEAMS_CMD_INFO_SIZE             64

#if (FTL_POOL_ENABLE == 1)
#define APP_TEAMS_BT_COD_INFO_ADDR          (0)
#else
#define APP_TEAMS_BT_COD_INFO_ADDR          (app_teams_ext_ftl_start_offset)
#endif
#define APP_TEAMS_BLE_DEV_BOND_INFO_ADDR    (APP_TEAMS_BT_COD_INFO_ADDR + APP_TEAMS_BT_COD_INFO_SIZE)
#define APP_TEAMS_CMD_INFO_ADDR             (APP_TEAMS_BLE_DEV_BOND_INFO_ADDR + APP_TEAMS_BLE_DEV_BOND_INFO_SIZE)

bool app_teams_ext_ftl_init(void);
uint32_t app_teams_ext_save_ftl_data(uint32_t offset, uint8_t *data, uint32_t len);
uint32_t app_teams_ext_load_ftl_data(uint32_t offset, uint8_t *data, uint32_t len);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_TEAMS_EXT_FTL_H_ */
