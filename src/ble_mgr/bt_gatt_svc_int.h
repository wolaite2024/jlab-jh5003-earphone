#ifndef __BT_GATT_SVC_INT_H
#define __BT_GATT_SVC_INT_H

#include "bt_gatt_svc.h"
extern uint16_t gatt_svc_mode;

void gatt_svc_handle_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state);
void gatt_svc_handle_gap_common_cb(uint8_t cb_type, void *p_cb_data);

#endif

