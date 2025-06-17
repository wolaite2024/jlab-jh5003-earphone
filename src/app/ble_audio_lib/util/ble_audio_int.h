#ifndef _BLE_AUDIO_INT_H_
#define _BLE_AUDIO_INT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "base_data_parse.h"
#include "ble_audio_sync.h"
#include "ble_audio_group.h"
#include "broadcast_source_sm.h"
#include "bap_unicast_client.h"
#include "bass_mgr.h"
#include "ascs_mgr.h"

#define LE_AUDIO_DEBUG            1
#define LE_AUDIO_INIT_DEBUG       0

#define LE_AUDIO_MSG_INTERNAL     0x00F0

extern void base_data_print(T_BASE_DATA_MAPPING *p_mapping);

extern bool ble_audio_set_past_recipient_param(uint16_t conn_handle,
                                               T_GAP_PAST_RECIPIENT_PERIODIC_ADV_SYNC_TRANSFER_PARAM *p_param);
extern bool ble_audio_get_bis_sync_state(T_BLE_AUDIO_SYNC_HANDLE handle, uint32_t *p_sync_state);


extern bool set_coordinator_read_chars(uint16_t conn_handle, uint8_t srv_instance_id);
extern T_BLE_AUDIO_GROUP_HANDLE set_coordinator_find_by_sirk(uint8_t *p_sirk);

extern bool codec_cap_parse(uint8_t value_len, uint8_t *p_value, T_CODEC_CAP *p_cap);
extern bool codec_cfg_check_cap(T_CODEC_CAP *p_cap, T_CODEC_CFG *p_cfg);

//not used now
extern T_BROADCAST_SOURCE_HANDLE broadcast_source_find(uint8_t adv_sid,
                                                       uint8_t broadcast_id[BROADCAST_ID_LEN]);
//not used now
extern T_BIG_MGR_BIS_CONN_HANDLE_INFO *broadcast_source_get_bis_info(T_BROADCAST_SOURCE_HANDLE
                                                                     handle,
                                                                     uint8_t *p_bis_num);
//not used now
extern bool broadcast_source_get_bis_conn_handle(T_BROADCAST_SOURCE_HANDLE handle, uint8_t bis_idx,
                                                 uint16_t *p_bis_conn_handle);

extern T_AUDIO_STREAM_SESSION_HANDLE audio_stream_session_find_by_cig_id(uint8_t cig_id);

typedef struct
{
    uint8_t metadata_len;
    uint8_t *p_metadata;
} T_BASS_METADATA_INFO;

extern bool bass_update_metadata(uint8_t source_id, uint8_t num_subgroups,
                                 T_BASS_METADATA_INFO *p_metadata_tbl);

extern bool ascs_action_cfg_codec(uint16_t conn_handle, uint8_t ase_id,
                                  T_ASE_CP_CODEC_CFG_ARRAY_PARAM *p_param, uint8_t *p_codec_data);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
