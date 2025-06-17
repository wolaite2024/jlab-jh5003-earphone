#ifndef _BASS_SOURCE_MGR_H_
#define _BASS_SOURCE_MGR_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#if LE_AUDIO_BASS_SUPPORT
#include "bass_def.h"
#include "ble_audio_sync.h"
#include "bt_gatt_svc.h"

#define BASS_INVALID_SOURCE_ID 0xff
#define BASS_SUBGROUP_MAX_NUM 4

#define BASS_BIG_INFO_RECEIVE_FLAG         0x01
#define BASS_BIG_BROADCAST_REQUIRED_FLAG   0x02
#define BASS_BIG_BROADCAST_CODE_SET_FLAG   0x04

#define BASS_PAST_SRV_DATA_MATCH_ADV_FLAG        0x01
#define BASS_PAST_SRV_DATA_MATCH_SRC_ADDR_FLAG   0x02

#define BASS_BIG_ENCRYPTION_FLAGS (BASS_BIG_INFO_RECEIVE_FLAG|BASS_BIG_BROADCAST_REQUIRED_FLAG|BASS_BIG_BROADCAST_CODE_SET_FLAG)
#define BASS_BIG_NO_ENCRYPTION_FLAGS (BASS_BIG_INFO_RECEIVE_FLAG)

typedef enum
{
    BASS_PA_IDLE = 0,
    BASS_PA_SYNC_REQ = 1,
    BASS_PA_WAIT_PAST = 2,
    BASS_PA_SYNCED = 3,
    BASS_PA_TERMINATE_REQ = 4,
} T_BASS_PA_SM_STATE;

typedef enum
{
    BASS_PA_CP_NO_SYNC_EVT,
    BASS_PA_CP_SYNC_PAST_EVT,
    BASS_PA_CP_SYNC_NO_PAST_EVT,
    BASS_PA_PAST_TIMEOUT_EVT,
    BASS_PA_STATE_IDLE,
    BASS_PA_STATE_SYNCHRONIZING,
    BASS_PA_STATE_SYNCHRONIZED,
    BASS_PA_STATE_TERMINATING,
} T_BASS_PA_SM_EVENT;

typedef enum
{
    BASS_BIG_IDLE = 0,
    BASS_BIG_SYNC_REQ = 1,
    BASS_BIG_WAIT_SYNC = 2,
    BASS_BIG_SYNCED = 3,
    BASS_BIG_TERMINATE_REQ = 4,
} T_BASS_BIG_SM_STATE;

typedef enum
{
    BASS_BIG_CP_NO_SYNC_EVT,
    BASS_BIG_CP_SYNC_EVT,
    BASS_BIG_PA_SYNC_EVT,
    BASS_BIG_BIG_INFO_EVT,
    BASS_BIG_SET_BROADCODE_EVT,
    BASS_BIG_STATE_IDLE,
    BASS_BIG_STATE_SYNCHRONIZING,
    BASS_BIG_STATE_SYNCHRONIZED,
    BASS_BIG_STATE_TERMINATING,
} T_BASS_BIG_SM_EVENT;

typedef struct
{
    uint8_t  base_data_subgroup_num;
    uint8_t  cp_subgroup_num;
    bool     cp_bis_switch;
    uint32_t base_data_subgroup_bis[BASS_SUBGROUP_MAX_NUM];
    uint32_t cp_big_sync;
    uint32_t cp_subgroup_bis[BASS_SUBGROUP_MAX_NUM];
} T_BASS_BIS_SYNC_INFO;

typedef struct
{
    T_BLE_AUDIO_SYNC_HANDLE handle;
//sm
    T_BASS_PA_SM_STATE pa_sm_state;
    T_BASS_BIG_SM_STATE big_sm_state;
    bool send_notify;
    bool brs_modify;
//PA info
    T_GAP_PA_SYNC_STATE pa_sync_state;
    uint8_t pa_sync_options;
    uint16_t pa_sync_skip;
    uint16_t pa_sync_timeout;
    uint16_t pa_interval;
    uint16_t past_timeout;
    void *p_past_timer_handle;
//BIG info
    T_GAP_BIG_SYNC_RECEIVER_SYNC_STATE big_sync_state;
    uint8_t big_info_flags;
    uint8_t biginfo_bis_num;
    T_BIG_MGR_SYNC_RECEIVER_BIG_CREATE_SYNC_PARAM big_sync_param;
    T_BASS_BIS_SYNC_INFO bis_sync_info;
    T_BASS_CP_DATA  *p_pending_cp_data;

//Broadcast Receive State Field
    T_BASS_BRS_DATA brs_data;
} T_BASS_BRS_DB;

typedef struct
{
    T_SERVER_ID service_id;
    uint8_t     brs_num;
    T_BASS_BRS_DB brs_tbl[];
} T_BASS_CB;

extern T_BASS_CB *p_bass;
void bass_pa_proc_sm_execute(T_BASS_BRS_DB *p_db, T_BASS_PA_SM_EVENT event,
                             uint8_t *p_data, bool send_notify);
void bass_big_proc_sm_execute(T_BASS_BRS_DB *p_db, T_BASS_BIG_SM_EVENT event,
                              uint8_t *p_data, bool send_notify);
uint8_t bass_brs_get_empty_char_num(void);
T_BASS_BRS_DB *bass_brs_find_by_id(uint8_t source_id);
T_BASS_BRS_DB *bass_brs_find_by_handle(T_BLE_AUDIO_SYNC_HANDLE handle);
bool bass_brs_gen_char_data(uint8_t source_id, uint8_t  **pp_data,
                            uint16_t *p_data_len);
bool bass_brs_delete(T_BASS_BRS_DB *p_db, bool send_notify);
T_BASS_BRS_DB *bass_brs_add(uint8_t source_address_type,
                            uint8_t *source_address, uint8_t source_adv_sid, uint8_t *broadcast_id);

bool bass_send_all_notify(T_BASS_BRS_DB *p_db);
#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
