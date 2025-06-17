#ifndef __BLE_SCAN_INT_H
#define __BLE_SCAN_INT_H

#include <stdint.h>
#include "gap_ext_scan.h"

void ble_scan_init(void);

void ble_scan_handle_gap_cb(uint8_t cb_type, T_LE_CB_DATA *cb_data);

void ble_scan_handle_state(uint8_t gap_scan_state, uint16_t cause);

#endif

