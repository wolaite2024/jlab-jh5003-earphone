/*
 * Copyright (c) 2020, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_TEAMS_FEATURE_SUPPORT
#include "string.h"
#include "os_mem.h"
#include "sysm.h"
#include "trace.h"
#include "remote.h"
#include "gap_conn_le.h"
#include "app_cfg.h"
#include "app_asp_device.h"
#include "app_teams_hid.h"
#include "app_teams_rws.h"
#include "app_single_multilink_customer.h"
#include "teams_util.h"
#include "app_teams_ext_ftl.h"
#if F_APP_TEAMS_BT_POLICY
#include "app_teams_bt_policy.h"
#endif
#if F_APP_TEAMS_BLE_POLICY
#include "app_teams_ble_policy.h"
#endif
#if F_APP_USB_AUDIO_SUPPORT
#include "app_usb_audio_hid.h"
#endif

static void app_teams_util_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    switch (event_type)
    {
    case SYS_EVENT_POWER_ON:
        {
            app_asp_device_handle_device_state(true);
        }
        break;

    case SYS_EVENT_POWER_OFF:
        {
            app_asp_device_handle_device_state(false);
        }
        break;

    default:
        break;
    }
}


void app_teams_util_init(void)
{
    app_asp_device_init();
    app_teams_hid_init();
    app_teams_ext_ftl_init();
#if F_APP_TEAMS_BT_POLICY
    app_teams_bt_policy_init();
#endif
#if F_APP_TEAMS_BLE_POLICY
    app_teams_ble_policy_init();
#endif
#if F_APP_USB_AUDIO_SUPPORT
    teams_util_init(app_hid_interrupt_in, app_asp_device_send_pkt_by_ble);
#else
    teams_util_init(NULL, app_asp_device_send_pkt_by_ble);
#endif
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SINGLE)
    {
        app_teams_rws_init();
    }
    sys_mgr_cback_register(app_teams_util_dm_cback);
}

#endif

