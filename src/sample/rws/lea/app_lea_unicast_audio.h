#ifndef _APP_LEA_UNICAST_AUDIO_H_
#define _APP_LEA_UNICAST_AUDIO_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "audio_type.h"
#include "bt_direct_msg.h"
#include "mics_def.h"
#include "app_dsp_cfg.h"
#include "app_link_util.h"
#include "app_msg.h"

/** @defgroup APP_LEA_UCA App LE Audio UNICAST_AUDIO
  * @brief this file handle Unicast Audio related process
  * @{
  */
#if F_APP_ERWS_SUPPORT
#define LC3PLUS_2_5MS_PD        50
#define LC3PLUS_5MS_PD          50
#else
#define LC3PLUS_2_5MS_PD        24
#define LC3PLUS_5MS_PD          28
#endif
#define LC3PLUS_HIRES_10MS_PD   100

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_LEA_UCA_Exported_Functions App LE Audio Unicast Audio Functions
    * @{
    */

T_APP_LE_LINK *app_lea_uca_get_stream_link(void);
void app_lea_uca_pwr_on_enter_gaming_mode(void);
void app_lea_uca_init(void);
void app_lea_uca_link_sm(uint16_t conn_handle, uint8_t event, void *p_data);
void app_lea_uca_set_sidetone_instance(T_APP_DSP_CFG_SIDETONE info);
T_AUDIO_EFFECT_INSTANCE *app_lea_uca_p_eq_instance(void);
T_AUDIO_EFFECT_INSTANCE app_lea_uca_get_eq_instance(void);
bool *app_lea_uca_get_eq_abled(void);
bool app_lea_uca_get_diff_path(void);
void app_lea_uca_enable_mic(void);
bool app_lea_uca_sync_cis_streaming_state(bool is_streaming);
void app_lea_uca_handle_iso_data(T_BT_DIRECT_CB_DATA *p_data);
bool app_lea_uca_set_mic_mute(void *p_link_info, uint8_t action, bool is_all_mic_synced,
                              T_MICS_MUTE *mute_state);
void app_lea_uca_set_mic_mute_state(bool mute);
bool app_lea_uca_get_mic_mute_state(void);
void app_lea_uca_set_ready_to_downstream(bool downstream);
bool app_lea_uca_check_enable_mic(void);
uint8_t app_lea_uca_count_num(void);
void app_lea_uca_msg_handle(T_IO_MSG *io_msg);
bool app_lea_uca_is_upstreaming_enabled(void);
void app_lea_uca_set_key_convert(void);
/** @} */ /* End of group APP_LEA_UCA_Exported_Functions */
/** End of APP_LEA_UCA
* @}
*/

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
