#ifndef _BLE_AUDIO_SYNC_INT_H_
#define _BLE_AUDIO_SYNC_INT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#if (LE_AUDIO_BROADCAST_SINK_ROLE | LE_AUDIO_BROADCAST_ASSISTANT_ROLE)
#include "ble_audio_sync.h"
#if LE_AUDIO_BASS_SUPPORT
#define MSG_BLE_AUDIO_SYNC_PAST_SRV_DATA  0xFE
#define MSG_BLE_AUDIO_SYNC_LOCAL_ADD  0xFF

extern uint8_t bass_brs_get_empty_char_num(void);

void ble_audio_sync_register_bass_cb(P_FUN_BLE_AUDIO_SYNC_CB cb_pfn);
#endif
T_BLE_AUDIO_SYNC_HANDLE ble_audio_sync_create_int(P_FUN_BLE_AUDIO_SYNC_CB cb_pfn,
                                                  uint8_t advertiser_address_type,
                                                  uint8_t *advertiser_address, uint8_t adv_sid,
                                                  uint8_t broadcast_id[BROADCAST_ID_LEN],
                                                  T_BLE_AUDIO_ACTION_ROLE role);
bool ble_audio_sync_release_int(T_BLE_AUDIO_SYNC_HANDLE *p_handle, T_BLE_AUDIO_ACTION_ROLE role);

bool ble_audio_pa_sync_establish_int(T_BLE_AUDIO_SYNC_HANDLE handle, uint8_t options,
                                     uint8_t sync_cte_type,
                                     uint16_t skip, uint16_t sync_timeout, T_BLE_AUDIO_ACTION_ROLE role);
bool ble_audio_pa_terminate_int(T_BLE_AUDIO_SYNC_HANDLE handle, T_BLE_AUDIO_ACTION_ROLE role);


#if LE_AUDIO_BROADCAST_SINK_ROLE
bool ble_audio_big_sync_establish_int(T_BLE_AUDIO_SYNC_HANDLE handle,
                                      T_BIG_MGR_SYNC_RECEIVER_BIG_CREATE_SYNC_PARAM *p_sync_param, T_BLE_AUDIO_ACTION_ROLE role);
bool ble_audio_big_terminate_int(T_BLE_AUDIO_SYNC_HANDLE handle, T_BLE_AUDIO_ACTION_ROLE role);
bool ble_audio_big_get_sync_info(T_BLE_AUDIO_SYNC_HANDLE handle,
                                 T_BIG_MGR_SYNC_RECEIVER_BIG_CREATE_SYNC_PARAM *p_sync_param);

#endif

#endif


#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
