/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if XM_XIAOAI_FEATURE_SUPPORT

#include <string.h>
#include "os_mem.h"
#include "os_timer.h"
#include "remote.h"
#include "trace.h"
#include "ble_adv_data.h"
#include "gap_le_types.h"
#include "app_xm_xiaoai_ble_adv.h"
#include "app_cfg.h"
#include "app_ble_gap.h"
#include "app_main.h"
#include "app_xiaoai_device.h"

static void *p_le_xm_xiaoai_adv_handle = NULL;

typedef struct _XM_XIAOAI_ADV_DATA
{
    uint8_t len1;
    uint8_t type1;
    uint8_t data1;

    uint8_t len2;
    uint8_t type2;
    uint8_t company_id_1;
    uint8_t company_id_2;
    uint8_t tlv_len;
    uint8_t tlv_type;
    uint8_t major_id;

    uint8_t bud_side: 1;
    uint8_t minor_id: 2;
    uint8_t reserved1: 1;
    uint8_t bud_l_discov: 1;
    uint8_t bud_r_discov: 1;
    uint8_t bud_l_conn: 1;
    uint8_t bud_r_conn: 1;

    uint8_t reserved2: 1;
    uint8_t case_open: 1;
    uint8_t bt_connected: 1;
    uint8_t bt_paired: 1;
    uint8_t mac_encrypted: 1;
    uint8_t case_out: 1;
    uint8_t tws_connected: 1;
    uint8_t tws_connecting: 1;

    uint8_t left_bat_level: 7;
    uint8_t left_charge_state: 1;

    uint8_t right_bat_level: 7;
    uint8_t right_charge_state: 1;

    uint8_t case_bat_level: 7;
    uint8_t case_charge_state: 1;

    uint8_t lap_phone[3];
    uint8_t bd_addr_r_bud[6];
    uint8_t adv_cnt;
    uint8_t bd_addr_l_bud[6];

} T_XM_XIAOAI_ADV_DATA;

typedef struct _XM_XIAOAI_SCAN_RSP_DATA
{
    uint8_t len1;
    uint8_t type1;
    uint8_t vid[2];
    uint8_t len2;
    uint8_t type;
    uint8_t version;
    uint8_t pid[2];
    uint8_t virtual_address[4];

    uint8_t reserved1: 1;
    uint8_t color: 3;
    uint8_t reserved2: 4;

} T_XM_XIAOAI_SCAN_RSP_DATA;

T_XM_XIAOAI_ADV_DATA xm_xiaoai_adv_data;
T_XM_XIAOAI_SCAN_RSP_DATA xm_xiaoai_scan_rsp_data;

void app_format_adv_data(void)
{
    uint8_t lap_iphone[3] = {0};

    uint8_t bd_addr_r_bud[6] = {0};
    uint8_t bd_addr_l_bud[6] = {0};


    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        bd_addr_r_bud[0] = app_db.factory_addr[4];
        bd_addr_r_bud[1] = app_db.factory_addr[5];
        bd_addr_r_bud[2] = app_db.factory_addr[3];
        bd_addr_r_bud[3] = app_db.factory_addr[0];
        bd_addr_r_bud[4] = app_db.factory_addr[1];
        bd_addr_r_bud[5] = app_db.factory_addr[2];
    }
    else
    {
        bd_addr_r_bud[0] = app_cfg_nv.bud_local_addr[4];
        bd_addr_r_bud[1] = app_cfg_nv.bud_local_addr[5];
        bd_addr_r_bud[2] = app_cfg_nv.bud_local_addr[3];
        bd_addr_r_bud[3] = app_cfg_nv.bud_local_addr[0];
        bd_addr_r_bud[4] = app_cfg_nv.bud_local_addr[1];
        bd_addr_r_bud[5] = app_cfg_nv.bud_local_addr[2];
    }

    xm_xiaoai_adv_data.len1 = 0x02;
    xm_xiaoai_adv_data.type1 = 0x01;
    xm_xiaoai_adv_data.data1 = 0x1A;
    xm_xiaoai_adv_data.len2 = 0x1B;
    xm_xiaoai_adv_data.type2 = 0xFF;
    xm_xiaoai_adv_data.company_id_1 = 0x8F;
    xm_xiaoai_adv_data.company_id_2 = 0x03;
    xm_xiaoai_adv_data.tlv_len = 0x16;
    xm_xiaoai_adv_data.tlv_type = 0x01;
    xm_xiaoai_adv_data.major_id = extend_app_cfg_const.xiaoai_major_id;
    xm_xiaoai_adv_data.bud_r_conn = 1;
    xm_xiaoai_adv_data.bud_l_conn = 0;
    xm_xiaoai_adv_data.bud_r_discov = 1;
    xm_xiaoai_adv_data.bud_l_discov = 0;
    xm_xiaoai_adv_data.reserved1 = 0;
    xm_xiaoai_adv_data.minor_id = extend_app_cfg_const.xiaoai_minor_id >> 1;
    xm_xiaoai_adv_data.bud_side = 0;
    xm_xiaoai_adv_data.tws_connecting = 0;
    xm_xiaoai_adv_data.tws_connected = 0;
    xm_xiaoai_adv_data.case_out = 0;
    xm_xiaoai_adv_data.mac_encrypted = 0;
    xm_xiaoai_adv_data.bt_paired = 0;
    xm_xiaoai_adv_data.bt_connected = 0;
    xm_xiaoai_adv_data.case_open = 1;
    xm_xiaoai_adv_data.reserved2 = 0;
    xm_xiaoai_adv_data.left_charge_state = 0;
    xm_xiaoai_adv_data.left_bat_level = 100;
    xm_xiaoai_adv_data.right_charge_state = 0;
    xm_xiaoai_adv_data.right_bat_level = 100;
    xm_xiaoai_adv_data.case_charge_state = 0;
    xm_xiaoai_adv_data.case_bat_level = 0;

    memcpy(xm_xiaoai_adv_data.lap_phone, lap_iphone, sizeof(lap_iphone));
    memcpy(xm_xiaoai_adv_data.bd_addr_r_bud, bd_addr_r_bud, sizeof(bd_addr_r_bud));
    memcpy(xm_xiaoai_adv_data.bd_addr_l_bud, bd_addr_l_bud, sizeof(bd_addr_l_bud));

    xm_xiaoai_adv_data.adv_cnt = 0;

}

void app_format_scan_rsp_data(void)
{
    uint8_t virtual_address[4] = {0};

    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        virtual_address[0] = app_db.factory_addr[3];
        virtual_address[1] = app_db.factory_addr[2];
        virtual_address[2] = app_db.factory_addr[1];
        virtual_address[3] = app_db.factory_addr[0];
    }
    else
    {
        virtual_address[0] = app_cfg_nv.bud_local_addr[3];
        virtual_address[1] = app_cfg_nv.bud_local_addr[2];
        virtual_address[2] = app_cfg_nv.bud_local_addr[1];
        virtual_address[3] = app_cfg_nv.bud_local_addr[0];
    }

    xm_xiaoai_scan_rsp_data.len1 = 0x0D;
    xm_xiaoai_scan_rsp_data.type1 = 0xFF;


    BE_UINT16_TO_ARRAY(xm_xiaoai_scan_rsp_data.vid, extend_app_cfg_const.xiaoai_vid);
    xm_xiaoai_scan_rsp_data.len2 = 8;
    xm_xiaoai_scan_rsp_data.type = 0x03;
    xm_xiaoai_scan_rsp_data.version = 0x02;
    BE_UINT16_TO_ARRAY(xm_xiaoai_scan_rsp_data.pid,  extend_app_cfg_const.xiaoai_pid);

    memcpy(xm_xiaoai_scan_rsp_data.virtual_address, virtual_address, sizeof(virtual_address));
    xm_xiaoai_scan_rsp_data.reserved1 = 0;
    xm_xiaoai_scan_rsp_data.color = 0;
    xm_xiaoai_scan_rsp_data.reserved2 = 0;
}

uint8_t app_xm_xiaoai_get_headset_state(void)
{
    uint8_t adv_head_state;
    adv_head_state = *((uint8_t *)&xm_xiaoai_adv_data + 11); //get byte 11
    return adv_head_state;
}

void app_xm_xiaoai_get_battery_state(uint8_t *p_data)
{
    memcpy(p_data, (uint8_t *)&xm_xiaoai_adv_data + 12, 3);
}

uint8_t app_xm_xiaoai_get_adv_count(void)
{
    return xm_xiaoai_adv_data.adv_cnt;
}

bool le_xm_xiaoai_adv_start(uint16_t timeout_sec)
{
    if (p_le_xm_xiaoai_adv_handle != NULL)
    {
        APP_PRINT_WARN0("le_xm_xiaoai_adv_start: already started");
        return true;
    }

    APP_PRINT_INFO0("le_xm_xiaoai_adv_start");

    app_format_adv_data();
    app_format_scan_rsp_data();

    if (ble_adv_data_add(&p_le_xm_xiaoai_adv_handle, sizeof(xm_xiaoai_adv_data),
                         (uint8_t *)&xm_xiaoai_adv_data,
                         sizeof(xm_xiaoai_scan_rsp_data), (uint8_t *)&xm_xiaoai_scan_rsp_data))
    {
        app_xiaoai_device_start_adv_timer(timeout_sec);
        return true;
    }
    else
    {
        return false;
    }
}

bool le_xm_xiaoai_adv_stop(void)
{
    if (p_le_xm_xiaoai_adv_handle == NULL)
    {
        APP_PRINT_WARN0("le_xm_xiaoai_adv_stop: already stopped");
        return true;
    }

    APP_PRINT_INFO0("le_xm_xiaoai_adv_stop");

    if (ble_adv_data_del(p_le_xm_xiaoai_adv_handle))
    {
        p_le_xm_xiaoai_adv_handle = NULL;
        return true;
    }
    else
    {
        return false;
    }
}

bool le_xm_xiaoai_adv_ongoing(void)
{
    if (p_le_xm_xiaoai_adv_handle)
    {
        return true;
    }
    else
    {
        return false;
    }
}
#endif
