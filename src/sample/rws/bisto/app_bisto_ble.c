#if BISTO_FEATURE_SUPPORT

#include "app_bisto_ble.h"
#include "bisto_api.h"
#include "trace.h"
#include "ble_adv_data.h"
#include "ancs_client.h"
#include "ams_client.h"


typedef struct
{
    bool is_adving;
    void *p_adv_handle;
    BISTO_BLE *p_ble;
} APP_BISTO_BLE;


static APP_BISTO_BLE app_bisto_ble = {.is_adving = false, .p_adv_handle = NULL, .p_ble = NULL};



/** @brief  GAP - Advertisement data (max size = 31 bytes, best kept short to conserve power) */
static uint8_t adv_data[26] =
{
    /* Flags */
    0x02,             /* length */
    GAP_ADTYPE_FLAGS, /* type="Flags" */
    GAP_ADTYPE_FLAGS_LIMITED,
    /* Service */
    0x06,             /* length */
    GAP_ADTYPE_SERVICE_DATA,
    LO_WORD(GATT_UUID_BISTO_PROFILE),
    HI_WORD(GATT_UUID_BISTO_PROFILE),
    LO_WORD(DEF_BISTO_DEV_ID),
    HI_WORD(DEF_BISTO_DEV_ID),
    ((uint8_t)((DEF_BISTO_DEV_ID & 0xFF0000) >> 16)),
    /* Local name */
    sizeof(BISTO_LE_NAME) + 1,             /* length */
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    BISTO_LE_NAME[0], BISTO_LE_NAME[1], BISTO_LE_NAME[2], BISTO_LE_NAME[3], BISTO_LE_NAME[4], BISTO_LE_NAME[5],
    BISTO_LE_NAME[6], BISTO_LE_NAME[7], BISTO_LE_NAME[8], BISTO_LE_NAME[9], BISTO_LE_NAME[10], BISTO_LE_NAME[11],
    BISTO_LE_NAME[12], BISTO_LE_NAME[13],
};


bool app_bisto_ble_adv_start(void)
{
    APP_PRINT_TRACE1("app_bisto_ble_adv_start: is_adving %d", app_bisto_ble.is_adving);

    if (app_bisto_ble.is_adving != true)
    {
        if (ble_adv_data_add(&app_bisto_ble.p_adv_handle, sizeof(adv_data), adv_data, 0, NULL))
        {
            app_bisto_ble.is_adving = true;
            APP_PRINT_TRACE0("app_bisto_ble_adv_start: success");
            return true;
        }
        else
        {
            return false;
        }
    }

    return true;
}


bool app_bisto_ble_adv_stop(void)
{
    APP_PRINT_TRACE1("app_bisto ble_adv_stop: is_adving %d", app_bisto_ble.is_adving);

    if (app_bisto_ble.is_adving != false)
    {
        if (ble_adv_data_del(app_bisto_ble.p_adv_handle))
        {
            app_bisto_ble.p_adv_handle = NULL;
            app_bisto_ble.is_adving = false;

            return true;
        }
        else
        {
            APP_PRINT_ERROR0("app_bisto ble_adv_stop: failed");
            return false;
        }
    }

    return true;
}


void app_bisto_ble_set_name(void)
{
    le_set_gap_param(GAP_PARAM_DEVICE_NAME, sizeof(BISTO_LE_NAME), BISTO_LE_NAME);
}


void app_bisto_ble_init(void)
{
    app_bisto_ble.p_ble = bisto_ble_create();
    if (app_bisto_ble.p_ble == NULL)
    {
        APP_PRINT_TRACE0("app_bisto_ble_init: init failed");
    }

    bisto_ble_set_adv_stop_handler(app_bisto_ble.p_ble, app_bisto_ble_adv_stop);

    APP_PRINT_INFO0("app_bisto_ble_init: success");
}


/*This func is used to start bisto gatt client discovery explicitly*/
void app_bisto_ble_gatt_client_start_discovery(uint8_t conn_id)
{
    APP_PRINT_TRACE0("app_bisto_ble_gatt_client_start_discovery");
    ancs_start_discovery(conn_id);
}


void app_bisto_ble_disconnect(void)
{
    bisto_ble_disconnect(app_bisto_ble.p_ble);
}


void app_bisto_ble_set_remote_info(APP_BISTO_BLE_REMOTE_INFO *p_info)
{
    app_bisto_ble.is_adving = p_info->is_adving;

    APP_PRINT_TRACE1("app_bisto_ble_set_remote_info: is_adving %d", app_bisto_ble.is_adving);

    if (app_bisto_ble.is_adving)
    {
        app_bisto_ble_adv_start();
    }
}


void app_bisto_ble_get_remote_info(APP_BISTO_BLE_REMOTE_INFO *p_info)
{
    p_info->is_adving = app_bisto_ble.is_adving;
}

#endif
