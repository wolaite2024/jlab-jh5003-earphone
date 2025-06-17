#ifndef _BLE_AUDIO_UAL_H_
#define _BLE_AUDIO_UAL_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ble_link_util.h"

bool ble_audio_ual_resolve_rpa(uint8_t *unresolved_addr, uint8_t *resolved_addr,
                               uint8_t *resolved_addr_type);
bool ble_audio_ual_check_le_bond(uint16_t conn_handle);
void ble_audio_ual_set_pending_cccd(uint16_t cccd_handle);
void ble_audio_ual_set_pending_link_cccd(T_BLE_AUDIO_LINK *p_link, uint16_t cccd_handle);
bool ble_audio_ual_clear_pending_cccd(uint16_t conn_handle, uint16_t cccd_handle);
bool ble_audio_ual_check_addr(uint8_t *bd_addr1, uint8_t bd_type1,
                              uint8_t *bd_addr2, uint8_t bd_type2);
bool ble_audio_ual_get_le_ltk(uint8_t *remote_bd, uint8_t remote_bd_type, uint16_t conn_handle,
                              bool remote, uint8_t *p_key_len, uint8_t *p_ltk);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
