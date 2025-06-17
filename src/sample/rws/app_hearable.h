#if F_APP_HEARABLE_SUPPORT
#ifndef _APP_HEARABLE_H_
#define _APP_HEARABLE_H_

#include <stdbool.h>
#include "app_listening_mode.h"
#include "app_link_util.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define HA_PROG_NAME_SIZE 12 //same as HA_PROG_OBJ_NAME_SIZE

/** @defgroup APP_HEARABLE param report
  * @brief App HEARABLE param report
  * @{
  */
typedef enum
{
    HA_MODE_RHA     = 0x00,
    HA_MODE_ULLRHA  = 0x01,
    HA_MODE_TOTAL   = 0x02,

    HA_MODE_INVALID = 0xFF,
} T_APP_HA_MODE;

typedef enum
{
    HA_STEREO_TYPE_L    = 0x00,
    HA_STEREO_TYPE_R    = 0x01,
    HA_STEREO_TYPE_BOTH = 0x02,
} T_APP_HA_STEREO_TYPE;

typedef struct t_ha_vse_mgr_db
{
    bool is_send;
    uint16_t seq_num;
    uint8_t audio_params_sel_idx;
    uint8_t voice_params_sel_idx;
} T_HA_VSE_MGR_DB;

typedef struct
{
    uint8_t index;
    uint16_t name_len;
    char    p_name[HA_PROG_NAME_SIZE];
} T_HA_HAP_INFO;

void app_ha_listening_delay_start(void);
void app_ha_switch_hearable_prog(void);
void app_ha_adjust_volume_level(bool is_vol_up);
uint8_t app_ha_get_volume_sync(void);
void app_ha_hearing_test_tone_generator(void);
void app_ha_hearing_test_start(void);
bool app_ha_get_is_hearing_test(void);
bool app_ha_get_bud_is_hearing_test(void);
void app_ha_set_is_hearing_test(bool is_hearing_test);
void app_ha_audio_instance_create(T_APP_BR_LINK *p_link);
void app_ha_voice_instance_create(T_APP_BR_LINK *p_link);
void app_ha_audio_instance_release(T_APP_BR_LINK *p_link);
void app_ha_voice_instance_release(T_APP_BR_LINK *p_link);
uint16_t app_ha_get_group_ltv_data_len(uint16_t params_len);
void app_ha_generate_group_ltv_data(T_APP_HA_STEREO_TYPE type, uint8_t *group_ltv_data,
                                    uint8_t prepend_len, uint16_t params_len, uint16_t ha_type);
void app_ha_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                       uint8_t *ack_pkt);
void app_ha_clear_param(void);
void app_ha_init(void);
T_HA_HAP_INFO app_ha_hearing_get_prog_info(uint8_t id);
bool app_ha_hearing_set_prog_id(uint8_t id);
uint8_t app_ha_hearing_get_prog_num(void);
uint8_t app_ha_hearing_get_active_prog_id(void);
T_APP_HA_MODE app_ha_get_ha_mode(void);
void app_ha_wrap_ullrha_cmd(uint8_t *buf, uint16_t len);
uint8_t *app_ha_get_group_ltv_data(void);
void app_ha_generate_ltv_data_from_h2d_cmd(T_APP_HA_STEREO_TYPE stereo_type, uint8_t *buf,
                                           uint16_t buf_len,
                                           uint8_t *prepend_len, uint16_t *ha_type);
void app_ha_push_rha_runtime_effect(T_APP_HA_STEREO_TYPE stereo_type, uint8_t *buf,
                                    uint16_t buf_len);
bool app_ha_send_rha_effect(void);
bool app_ha_send_audio_effect(void);
bool app_ha_send_voice_effect(void);
void app_ha_push_audio_runtime_effect(T_APP_HA_STEREO_TYPE stereo_type, uint8_t *buf,
                                      uint16_t buf_len);
void app_ha_push_voice_runtime_effect(T_APP_HA_STEREO_TYPE stereo_type, uint8_t *buf,
                                      uint16_t buf_len);

/** End of APP_HEARABLE
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_HEARABLE_H_ */
#endif /*F_APP_HEARABLE_SUPPORT */
