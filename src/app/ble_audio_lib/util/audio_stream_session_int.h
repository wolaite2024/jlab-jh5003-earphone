#ifndef _AUDIO_STREAM_SESSION_INT_H_
#define _AUDIO_STREAM_SESSION_INT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "bap_unicast_client.h"
#include "gap_cig_mgr.h"
#include "ascs_def.h"
#include "bap_int.h"
#include "sys_timer.h"

#if LE_AUDIO_ASCS_CLIENT_SUPPORT

#define AUDIO_ACTION_RETRY_MAX_NUM 2
//#define AUDIO_STREAM_SRV_CIS_MAX 2

#define AUDIO_STREAM_SERVER_CFG_QOS 0x01

typedef enum
{
    AUDIO_STREAM_EVENT_START_ACTION,
    AUDIO_STREAM_EVENT_STOP_ACTION,
    AUDIO_STREAM_EVENT_RELEASE_ACTION,
    AUDIO_STREAM_EVENT_ASE_STATE_CHANGE,
    AUDIO_STREAM_EVENT_READ_ASE_STATE,
    AUDIO_STREAM_EVENT_DATA_PATH_ADD,
    AUDIO_STREAM_EVENT_DATA_PATH_REMOVE,
    AUDIO_STREAM_EVENT_CIG_REMOVE,
    AUDIO_STREAM_EVENT_CREATE_CIS,
    AUDIO_STREAM_EVENT_ASE_CP_ERROR,
    AUDIO_STREAM_EVENT_CIS_DISCONN,
    AUDIO_STREAM_EVENT_CIG_SET_ERROR,
} T_AUDIO_STREAM_EVENT;

typedef enum
{
    T_CIG_SETTING_STATE_IDLE,
    T_CIG_SETTING_STATE_STARTING,
    T_CIG_SETTING_STATE_CONFIGURED,
    T_CIG_SETTING_STATE_REMOVING
} T_CIG_SETTING_STATE;

typedef struct
{
    T_BAP_ASE_ENTRY *p_ase_entry;
    uint8_t cis_id;
    uint8_t direction;
    uint8_t channel_num;
    bool    codec_update;
    uint8_t metadata_len;
    T_AUDIO_ASE_CODEC_CFG ase_codec_cfg;
    T_AUDIO_ASE_QOS_CFG   qos_cfg;
    uint8_t *p_metadata;
} T_BAP_ASE_INFO;

typedef struct
{
    uint8_t                cis_id;
    uint8_t                sink_ase_idx;
    uint8_t                source_ase_idx;
    uint8_t                path_direction;
    uint16_t               cis_conn_handle;
    T_CIS_CFG_PARAM        cis_param;
} T_BAP_CIS_PARAM;

typedef struct
{
    uint8_t                cis_num;
    uint8_t                ase_num;
    uint8_t                cfg_flags;
    T_BAP_CIS_PARAM        cis_info[AUDIO_STREAM_SRV_CIS_MAX];
    T_BAP_ASE_INFO         *ase_info;
    T_BLE_AUDIO_DEV_HANDLE dev_handle;
} T_AUDIO_STREAM_SERVER;

typedef struct t_audio_stream_session
{
    struct t_audio_stream_session *p_next;
    T_BLE_AUDIO_GROUP_HANDLE group_handle;
    T_SYS_TIMER_HANDLE       p_session_timer_handle;
    uint32_t                 cis_timeout_ms;
    T_SYS_TIMER_HANDLE       p_cis_disc_timer_handle;
    uint8_t                  cig_id;
    T_CIG_SETTING_STATE      cig_state;
    T_ASE_STATE              prefer_state;
    T_UNICAST_AUDIO_CFG_TYPE cfg_type;
    T_AUDIO_STREAM_STATE     state;
    T_BAP_UNICAST_ACTION     req_action;
    uint8_t                  retry_num;
    uint8_t                  server_num;
    bool                     retry_update_metadata;
    T_AUDIO_STREAM_SERVER   *server;
    T_CIG_CFG_PARAM          cig_param;
    T_AUDIO_SESSION_QOS_CFG  session_qos;
} T_AUDIO_STREAM_SESSION;

extern T_OS_QUEUE audio_session_queue;
bool audio_stream_session_check_int(const char *p_func_name,
                                    T_AUDIO_STREAM_SESSION_HANDLE handle);
#define audio_stream_session_check(handle) audio_stream_session_check_int(__func__, handle)
bool audio_stream_session_release(T_AUDIO_STREAM_SESSION_HANDLE handle);
uint8_t audio_stream_get_free_cis_id(void);
#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
