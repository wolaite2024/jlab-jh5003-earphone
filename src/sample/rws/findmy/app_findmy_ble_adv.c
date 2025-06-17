#if F_APP_FINDMY_FEATURE_SUPPORT
#include "fmna_adv_platform.h"
#include "fmna_adv.h"
#include "fmna_battery_platform.h"
#include "fmna_crypto.h"
#include "fmna_gatt_platform.h"
#include "fmna_state_machine.h"
#include "app_ble_rand_addr_mgr.h"
#include "app_adv_stop_cause.h"
#include "app_ble_gap.h"
#include "app_cfg.h"
#include "app_findmy.h"
#include "app_findmy_ble_adv.h"
#include "app_link_util.h"
#include "ble_ext_adv.h"
#include "remote.h"

static fmna_pairing_payload_t m_fmna_pairing_adv_payload       = {0};
#define SERVICE_DATA_LEN               (4)

typedef struct t_le_findmy_adv
{
    uint8_t adv_handle;
    uint8_t conn_id;
    T_BLE_EXT_ADV_MGR_STATE state;
} T_LE_FINDMY_ADV;

static T_LE_FINDMY_ADV le_findmy_adv =
{
    .adv_handle = 0xFF,
    .conn_id = 0xFF,
    .state = BLE_EXT_ADV_MGR_ADV_DISABLED,
};

static uint8_t findmy_pairing_adv_data[29] =
{
    /* Flags */
    0x18,             /* length */
    GAP_ADTYPE_SERVICE_DATA, /* type="Flags" */
    LO_WORD(FINDMY_UUID_SERVICE),
    HI_WORD(FINDMY_UUID_SERVICE),
};

/// Fills Pairing payload according to ADV spec.
static void app_findmy_pairing_adv_service_data_init(void)
{
    uint8_t product_data[PRODUCT_DATA_BLEN] = PRODUCT_DATA_VAL;

    memcpy(m_fmna_pairing_adv_payload.product_data,
           product_data,
           PRODUCT_DATA_BLEN);

    memset(m_fmna_pairing_adv_payload.acc_category, ACCESSORY_CATEGORY, sizeof(uint8_t));
    m_fmna_pairing_adv_payload.battery_state = fmna_battery_platform_get_battery_level();
}

static void app_findmy_adv_callback(uint8_t cb_type, void *p_cb_data)
{
    T_BLE_EXT_ADV_CB_DATA cb_data;

    memcpy(&cb_data, p_cb_data, sizeof(T_BLE_EXT_ADV_CB_DATA));
    switch (cb_type)
    {
    case BLE_EXT_ADV_STATE_CHANGE:
        {
            le_findmy_adv.state = cb_data.p_ble_state_change->state;
            APP_PRINT_TRACE2("app_findmy_adv_callback: BLE_EXT_ADV_STATE_CHANGE, adv_state %d, adv_handle %d",
                             le_findmy_adv.state, cb_data.p_ble_state_change->adv_handle);
            if (le_findmy_adv.state == BLE_EXT_ADV_MGR_ADV_DISABLED)
            {
                APP_PRINT_TRACE2("app_findmy_adv_callback: stop_cause %d, app_cause 0x%02x",
                                 cb_data.p_ble_state_change->stop_cause,
                                 cb_data.p_ble_state_change->app_cause);
            }
        }
        break;

    case BLE_EXT_ADV_SET_CONN_INFO:
        APP_PRINT_TRACE1("app_findmy_adv_callback: BLE_EXT_ADV_SET_CONN_INFO conn_id 0x%x",
                         cb_data.p_ble_conn_info->conn_id);

        le_findmy_adv.conn_id = cb_data.p_ble_conn_info->conn_id;
        break;

    default:
        break;
    }
    return;
}

bool app_findmy_adv_start(uint16_t timeout_sec)
{
    FMNA_LOG_INFO("app_findmy_adv_start: timeout_sec %d", timeout_sec);
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        return false;
    }

    if (le_findmy_adv.state == BLE_EXT_ADV_MGR_ADV_DISABLED)
    {
        if (ble_ext_adv_mgr_enable(le_findmy_adv.adv_handle, timeout_sec) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        APP_PRINT_TRACE0("app_findmy_adv_start: Already started");
        return true;
    }
}

bool app_findmy_adv_stop(uint8_t app_cause)
{
    if (le_findmy_adv.state == BLE_EXT_ADV_MGR_ADV_ENABLED)
    {
        if (ble_ext_adv_mgr_disable(le_findmy_adv.adv_handle, app_cause) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        APP_PRINT_TRACE0("app_findmy_adv_stop: Already stoped");
        return true;
    }
}

uint8_t app_findmy_adv_get_adv_handle(void)
{
    return le_findmy_adv.adv_handle;
}

uint8_t le_findmy_adv_get_conn_id(void)
{
    return le_findmy_adv.conn_id;
}

void app_findmy_enter_pair_mode(void)
{
    APP_PRINT_INFO0("app_findmy_enter_pair_mode");

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        start_pair_adv();
    }
}

void app_findmy_handle_role_swap_success(T_REMOTE_SESSION_ROLE device_role)
{
    APP_PRINT_INFO0("app_findmy_handle_role_swap_success");
    if (device_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        fmna_crypto_init();
        fmna_gatt_platform_roleswap_init();
        fmna_state_machine_init();
    }
    else if (device_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_findmy_adv_stop(APP_STOP_ADV_CAUSE_FINDMY);

        uint8_t conn_id = le_findmy_adv_get_conn_id();

        if (conn_id != 0xFF)
        {
            T_APP_LE_LINK *p_link;

            p_link = app_link_find_le_link_by_conn_id(conn_id);
            if (p_link != NULL)
            {
                app_ble_gap_disconnect(p_link, LE_LOCAL_DISC_CAUSE_SECONDARY);
            }
        }
    }
}

void app_findmy_handle_role_swap_fail(T_REMOTE_SESSION_ROLE device_role)
{
    if (device_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        start_pair_adv();
    }
}

void app_findmy_adv_init(void)
{
    T_LE_EXT_ADV_LEGACY_ADV_PROPERTY adv_event_prop = LE_EXT_ADV_LEGACY_ADV_CONN_SCAN_UNDIRECTED;
    uint16_t adv_interval_min = 0x30;
    uint16_t adv_interval_max = 0x30;
    T_GAP_LOCAL_ADDR_TYPE own_address_type = GAP_LOCAL_ADDR_LE_RANDOM;
    T_GAP_REMOTE_ADDR_TYPE peer_address_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t  peer_address[6] = {0, 0, 0, 0, 0, 0};
    T_GAP_ADV_FILTER_POLICY filter_policy = GAP_ADV_FILTER_ANY;
    uint8_t random_addr[6] = {0};

    gap_get_param(GAP_PARAM_BD_ADDR, random_addr);
    random_addr[5] |= (uint8_t)FMNA_ADV_ADDR_TYPE_MASK;

    app_findmy_pairing_adv_service_data_init();
    memcpy(findmy_pairing_adv_data + SERVICE_DATA_LEN, (uint8_t *)&m_fmna_pairing_adv_payload,
           sizeof(m_fmna_pairing_adv_payload));

    ble_ext_adv_mgr_init_adv_params(&le_findmy_adv.adv_handle, adv_event_prop, adv_interval_min,
                                    adv_interval_max, own_address_type, peer_address_type, peer_address,
                                    filter_policy, 25, (uint8_t *)findmy_pairing_adv_data,
                                    0, NULL,
                                    random_addr);

    APP_PRINT_TRACE2("app_findmy_adv_init: findmy_pairing_adv_data %b,random address %b",
                     TRACE_BINARY(sizeof(findmy_pairing_adv_data), findmy_pairing_adv_data), TRACE_BDADDR(random_addr));

    ble_ext_adv_mgr_register_callback(app_findmy_adv_callback, le_findmy_adv.adv_handle);
}
#endif
