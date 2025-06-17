#ifndef __BT_GATT_CLIENT_INT_H
#define __BT_GATT_CLIENT_INT_H

#include "gap_msg.h"
#include "bt_gatt_client.h"

extern uint16_t gattc_dis_mode;

#define F_BT_GATT_CLIENT_EXT_API           1
#define BT_GATT_CLIENT_USE_NORMAL_API      1

void gatt_client_handle_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state);
void gatt_client_handle_mtu_info(uint8_t conn_id, uint16_t mtu_size);
void gatt_client_handle_gap_common_cb(uint8_t cb_type, void *p_cb_data);

#endif

