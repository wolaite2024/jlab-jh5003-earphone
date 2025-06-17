/**
*****************************************************************************************
*     Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     has_mgr.h
  * @brief    Head file for Hearing Aid Server.
  * @details  This file defines Hearing Aid Server related API.
  * @author
  * @date
  * @version
  * **
  */

#ifndef _HAS_MGR_H_
#define _HAS_MGR_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "has_def.h"

/**
 * \defgroup    LEA_USE_CASE_HAP_Server Hearing Aid Server
 *
 * \brief   Provide status and control of Hearing Aid.
 */

/**
 * \defgroup HAP_Server_Exported_Types Hearing Aid Server Exported Types
 *
 * \ingroup LEA_USE_CASE_HAP_Server
 * @{
 */

/**
 * has_mgr.h
 *
 * \brief  HAS attribute features.
 *
 * \ingroup HAP_Server_Exported_Types
 */
typedef struct
{
    bool is_ha_cp_exist;                  /**< Whether control point is supported.
                                               True: support. False: Not support. */
    bool is_ha_cp_notify;                 /**< Whether control point notify is supported.
                                               True: support. False: Not support. */
    bool is_ha_features_notify_support;   /**< Whether Hearing Aid Features notify is supported.
                                               True: support. False: Not support. */
} T_HAS_ATTR_FEATURE;

/**
 * has_mgr.h
 *
 * \brief  HAS handle control point write request data.
 *
 * The message data for @ref LE_AUDIO_MSG_HAS_CP_IND.
 *
 * \ingroup HAP_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    T_HAS_CP_OP cp_op;
    uint8_t index;
    uint8_t preset_num;
    uint8_t active_preset_idx;
    uint8_t name_len;
    char *p_name;
} T_HAS_CP_IND;

/**
 * has_mgr.h
 *
 * \brief  HAS pending Preset Changed data.
 *
 * The message data for @ref LE_AUDIO_MSG_HAS_PENDING_PRESET_CHANGE.
 *
 * \ingroup HAP_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
} T_HAS_PENDING_PRESET_CHANGE;
/**
 * End of HAP_Server_Exported_Types
 * @}
 */

/**
 * \defgroup HAP_Server_Exported_Functions Hearing Aid Server Exported Functions
 *
 * \ingroup LEA_USE_CASE_HAP_Server
 * @{
 */

/**
 * has_mgr.h
 *
 * \brief  Initialize HAS service.
 *
 * \param[in]  attr_feature           HAS attribute feature: @ref T_HAS_ATTR_FEATURE.
 * \param[in]  hearing_aid_features   Hearing aid features: @ref T_HAS_HA_FEATURES.
 *
 * \return         The result of initializing HAS service.
 * \retval true    Initializing HAS service is successful.
 * \retval false   Initializing HAS service failed.
 *
 * \ingroup HAP_Server_Exported_Functions
 */
bool has_init(T_HAS_ATTR_FEATURE attr_feature, T_HAS_HA_FEATURES hearing_aid_features);

/**
 * has_mgr.h
 *
 * \brief  HAS clear control point flag.
 *
 * This API shall be called when le link is disconnected.
 *
 * \ingroup HAP_Server_Exported_Functions
 */
void has_clear_cp_flag(void);

/**
 * has_mgr.h
 *
 * \brief  Update Hearing Aid Features.
 *
 * \param[in]  hearing_aid_features   Hearing aid features: @ref T_HAS_HA_FEATURES.
 *
 * \return         The result of updating Hearing Aid Features.
 * \retval true    Updating Hearing Aid Features is successful.
 * \retval false   Updating Hearing Aid Features failed.
 *
 * \ingroup HAP_Server_Exported_Functions
 */
bool has_update_ha_features(T_HAS_HA_FEATURES hearing_aid_features);

/**
 * has_mgr.h
 *
 * \brief  Update Active Preset Index.
 *
 * \param[in]  preset_idx   Preset record index.
 *
 * \return         The result of updating Active Preset Index.
 * \retval true    Updating Active Preset Index is successful.
 * \retval false   Updating Active Preset Index failed.
 *
 * \ingroup HAP_Server_Exported_Functions
 */
bool has_update_active_preset_idx(uint8_t preset_idx);

/**
 * has_mgr.h
 *
 * \brief  Send Read Preset Response control point.
 *
 * \param[in]  conn_handle   Connection handle.
 * \param[in]  value_len     Preset record value length.
 * \param[in]  p_value       Pointer to preset record name.
 * \param[in]  is_last       Whether the preset record is the last one.
 * \arg    true  : The preset record is the last one.
 * \arg    false : The preset record is not the last one.
 *
 * \return         The result of sending Read Preset Response control point.
 * \retval true    Sending Read Preset Response control point is successful.
 * \retval false   Sending Read Preset Response control point failed.
 *
 * \ingroup HAP_Server_Exported_Functions
 */
bool has_handle_read_preset_rsp(uint16_t conn_handle, uint8_t value_len,
                                uint8_t *p_value, bool is_last);

/**
 * has_mgr.h
 *
 * \brief  Send Preset Changed control point.
 *
 * \param[in]  change_id     Preset record change id: @ref T_HAS_PRESET_CHANGE_ID.
 * \param[in]  conn_handle   Connection handle.
 * \param[in]  value_len     Preset record value length.
 * \param[in]  p_value       Pointer to Preset Changed data.
 * \param[in]  is_last       Whether the preset record is the last one.
 * \arg    true  : The preset record is the last one.
 * \arg    false : The preset record is not the last one.
 *
 * \return         The result of sending Preset Changed control point.
 * \retval true    Sending Preset Changed control point is successful.
 * \retval false   Sending Preset Changed control point failed.
 *
 * \ingroup HAP_Server_Exported_Functions
 */
bool has_send_preset_change_data(T_HAS_PRESET_CHANGE_ID change_id, uint16_t conn_handle,
                                 uint16_t value_len, uint8_t *p_value, bool is_last);

/**
 * has_mgr.h
 *
 * \brief  Send Preset Changed control point to all the connected client device.
 *
 * \param[in]  change_id     Preset record change id: @ref T_HAS_PRESET_CHANGE_ID.
 * \param[in]  value_len     Preset record value length.
 * \param[in]  p_value       Pointer to Preset Changed data.
 * \param[in]  is_last       Whether the preset record is the last one.
 * \arg    true  : The preset record is the last one.
 * \arg    false : The preset record is not the last one.
 *
 * \return         The result of sending Preset Changed control point.
 * \retval true    Sending Preset Changed control point is successful.
 * \retval false   Sending Preset Changed control point failed.
 *
 * \ingroup HAP_Server_Exported_Functions
 */
bool has_send_preset_change_data_all(T_HAS_PRESET_CHANGE_ID change_id,
                                     uint16_t value_len, uint8_t *p_value, bool is_last);
/**
 * End of HAP_Server_Exported_Functions
 * @}
 */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
