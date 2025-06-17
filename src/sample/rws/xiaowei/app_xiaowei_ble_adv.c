/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_XIAOWEI_FEATURE_SUPPORT
#include <string.h>
#include "stdlib.h"
#include "os_timer.h"
#include "remote.h"
#include "trace.h"
#include "ble_adv_data.h"
#include "gap_le_types.h"
#include "app_xiaowei_ble_adv.h"
#include "app_cfg.h"
#include "app_ble_gap.h"
#include "app_main.h"
#include "app_xiaowei_device.h"
#include "xiaowei_ble_service.h"
#include "app_xiaowei.h"

void *p_le_xiaowei_adv_handle = NULL;
T_XIAOWEI_ADV_DATA xiaowei_adv_data;
T_XIAOWEI_SCAN_RSP_DATA xiaowei_scan_rsp_data;

void app_xiaowei_adv_set_fvalue()
{
    memcpy(app_xiaowei_fvalues, xiaowei_fvalue_get_all(),
           sizeof(T_XIAOWEI_FVALUE_STORE) * XIAOWEI_FVALUE_MAX_NUMBER);

    for (int i = XIAOWEI_FVALUE_MAX_NUMBER - 1; i >= 0; i--)
    {
        if (app_xiaowei_fvalues[i].is_used && (app_xiaowei_fvalues[i].os_system == XIAOWEI_OS_TYPE_IOS))
        {
            memcpy(xiaowei_adv_data.xiaowei_char_value, app_xiaowei_fvalues[i].xiaowei_fvalue, 8);
            return;
        }
    }

    memset(xiaowei_adv_data.xiaowei_char_value, 0, 8);
}

void app_xiaowei_format_adv_data(void)
{
    uint8_t bt_mac_address[6] = {0};

    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        memcpy(bt_mac_address, app_db.factory_addr, 6);
    }
    else
    {
        memcpy(bt_mac_address, app_cfg_nv.bud_local_addr, 6);
    }

    xiaowei_adv_data.len1 = 0x02;
    xiaowei_adv_data.type1_flags = GAP_ADTYPE_FLAGS;
    xiaowei_adv_data.data1 = GAP_ADTYPE_FLAGS_GENERAL |
                             GAP_ADTYPE_FLAGS_SIMULTANEOUS_LE_BREDR_CONTROLLER | GAP_ADTYPE_FLAGS_SIMULTANEOUS_LE_BREDR_HOST;

    xiaowei_adv_data.len2 = 0x03;
    xiaowei_adv_data.type2_service_uuid = GAP_ADTYPE_16BIT_COMPLETE;
    xiaowei_adv_data.data2_uuid_lo_word = LO_WORD(GATT_UUID_SERVICE_XIAOWEI);
    xiaowei_adv_data.data2_uuid_hi_word = HI_WORD(GATT_UUID_SERVICE_XIAOWEI);

    xiaowei_adv_data.len3 = 0x15;
    xiaowei_adv_data.type3_manufacture_data = GAP_ADTYPE_MANUFACTURER_SPECIFIC;
    xiaowei_adv_data.company_id_lo_word = 0x5d;
    xiaowei_adv_data.company_id_hi_word = 0x00;

    LE_UINT32_TO_ARRAY(xiaowei_adv_data.product_id, extend_app_cfg_const.xiaowei_product_id);

    memcpy(xiaowei_adv_data.bt_mac_address, bt_mac_address, sizeof(bt_mac_address));

    app_xiaowei_adv_set_fvalue();

    APP_PRINT_TRACE1("app_xiaowei_format_adv_data: %b", TRACE_BINARY(sizeof(xiaowei_adv_data),
                                                                     &xiaowei_adv_data));
}

bool le_xiaowei_adv_start(uint16_t timeout_sec)
{
    if (p_le_xiaowei_adv_handle != NULL)
    {
        APP_PRINT_WARN0("le_xiaowei_adv_start: already started");
        return true;
    }
    uint16_t adv_interval = (extend_app_cfg_const.multi_adv_interval * 8) / 5;
    APP_PRINT_INFO2("le_xiaowei_adv_start: adv timeout %d s, adv_interval %d ms", timeout_sec,
                    adv_interval);

    app_xiaowei_format_adv_data();

    if (ble_adv_data_add(&p_le_xiaowei_adv_handle, sizeof(xiaowei_adv_data),
                         (uint8_t *)&xiaowei_adv_data,
                         sizeof(xiaowei_scan_rsp_data), (uint8_t *)&xiaowei_scan_rsp_data))
    {
        app_xiaowei_device_start_adv_timer(timeout_sec);
        return true;
    }
    else
    {
        return false;
    }
}

bool le_xiaowei_adv_stop(void)
{
    if (p_le_xiaowei_adv_handle == NULL)
    {
        APP_PRINT_WARN0("le_xiaowei_adv_stop: already stopped");
        return true;
    }

    APP_PRINT_INFO0("le_xiaowei_adv_stop");

    if (ble_adv_data_del(p_le_xiaowei_adv_handle))
    {
        p_le_xiaowei_adv_handle = NULL;
        return true;
    }
    else
    {
        return false;
    }
}

bool le_xiaowei_adv_ongoing(void)
{
    if (p_le_xiaowei_adv_handle)
    {
        return true;
    }
    else
    {
        return false;
    }
}
#endif
