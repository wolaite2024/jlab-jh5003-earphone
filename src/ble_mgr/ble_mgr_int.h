#ifndef __BLE_MGR_INT_H
#define __BLE_MGR_INT_H

#include "os_queue.h"

#define BLE_MGR_DEINIT   0
#define BLE_MGR_INIT_DEBUG   0

typedef struct
{
    struct
    {
        bool enable;
    } ble_ext_adv;

    struct
    {
        bool enable;
    } ble_adv_data;

    struct
    {
        bool enable;
    } ble_conn;

    struct
    {
        bool enable;
    } ble_scan;

    T_OS_QUEUE msg_cb_queue;
} T_BLE_MGR_CB;

extern T_BLE_MGR_CB *p_ble_mgr;

#if BLE_MGR_DEINIT
#define BLE_MGR_DEINIT_FLAG 0x01
extern uint8_t ble_mgr_deinit_flow;

void ble_conn_update_deinit(void);
void ble_adv_data_deinit(void);
void ble_ext_adv_mgr_deinit(void);
void ble_scan_deinit(void);

#if CONFIG_REALTEK_BT_GATT_CLIENT_SUPPORT
void gatt_client_deinit(void);
#endif
#if CONFIG_REALTEK_BT_GATT_SVC_SUPPORT
void gatt_svc_deinit(void);
#endif
#if CONFIG_REALTEK_APP_BOND_MGR_SUPPORT
void bt_bond_deinit(void);
#if F_BT_LE_SUPPORT
void bt_le_key_deinit(void);
#endif
#if F_BT_BREDR_SUPPORT
void bt_legacy_key_deinit(void);
#endif
#endif
#endif

#endif

