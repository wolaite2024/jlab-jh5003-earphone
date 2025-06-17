#ifndef _BLE_AUDIO_DM_H_
#define _BLE_AUDIO_DM_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "gap_msg.h"
#include "ble_audio.h"

typedef enum
{
    BLE_DM_EVENT_UNKNOWN                                     = 0x0000,
    BLE_DM_EVENT_CONN_STATE                                  = 0x0001,
    BLE_DM_EVENT_MTU_INFO                                    = 0x0002,
    BLE_DM_EVENT_AUTHEN_INFO                                 = 0x0003,
    BLE_DM_EVENT_BOND_MODIFY                                 = 0x0004,
} T_BLE_DM_EVENT;

typedef struct
{
    uint8_t conn_id;
    T_GAP_CONN_STATE state;
    uint16_t disc_cause;
} T_BLE_DM_EVENT_PARAM_CONN_STATE;

typedef struct
{
    uint8_t conn_id;
    uint16_t mtu_size;
} T_BLE_DM_EVENT_PARAM_MTU_INFO;

typedef struct
{
    uint8_t conn_id;
    uint8_t new_state;
    uint16_t cause;
} T_BLE_DM_EVENT_PARAM_AUTHEN_INFO;

typedef union
{
    T_BLE_DM_EVENT_PARAM_CONN_STATE  conn_state;
    T_BLE_DM_EVENT_PARAM_MTU_INFO    mtu_info;
    T_BLE_DM_EVENT_PARAM_AUTHEN_INFO authen_info;
    T_BT_BOND_MODIFY                 bond_modify;
} T_BLE_DM_EVENT_PARAM;

typedef void (*P_BLE_DM_CBACK)(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len);

bool ble_dm_cback_register(P_BLE_DM_CBACK cback);
bool ble_dm_cback_unregister(P_BLE_DM_CBACK cback);
bool ble_dm_event_post(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
