#ifndef _BLE_LINK_UTIL_H_
#define _BLE_LINK_UTIL_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ble_audio_mgr.h"

T_BLE_AUDIO_LINK *ble_audio_link_find_by_conn_id(uint8_t conn_id);
T_BLE_AUDIO_LINK *ble_audio_link_find_by_conn_handle(uint16_t conn_handle);
T_BLE_AUDIO_LINK *ble_audio_link_alloc_by_conn_id(uint8_t conn_id, uint8_t srv_num);
T_BLE_AUDIO_LINK *ble_audio_link_find_by_addr(uint8_t *bd_addr, uint8_t bd_type);

T_LE_SRV_CFG *ble_srv_find_by_srv_id(uint16_t conn_handle, T_SERVER_ID service_id);
T_LE_SRV_CFG *ble_srv_alloc_by_srv_id(uint16_t conn_handle, T_SERVER_ID service_id);
void ble_srv_set_cccd_flags(T_LE_SRV_CFG *p_srv, uint16_t cccd_bits, uint8_t idx,
                            uint16_t flag_bit, bool pending);
bool ble_audio_link_free(T_BLE_AUDIO_LINK *p_link);
void ble_audio_send_notify_all(uint16_t cid, T_SERVER_ID service_id, uint16_t attrib_index,
                               uint8_t *p_data, uint16_t data_len);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
