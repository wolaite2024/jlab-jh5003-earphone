#if BISTO_FEATURE_SUPPORT

#include "app_bisto_roleswap.h"
#include "app_bisto_ble.h"
#include "bisto_api.h"
#include "app_cfg.h"
#include "remote.h"
#include "trace.h"

typedef enum
{
    APP_BISTO_SYNC_ALL,
} BISTO_REMOTE_MSG;


static struct
{
    bool roleswap_allow;
    BISTO_TWS *p_tws;
    T_REMOTE_RELAY_HANDLE relay_handle;
} app_bisto_tws = {.p_tws = NULL, .roleswap_allow = false};


typedef struct
{
    APP_BISTO_BLE_REMOTE_INFO app_ble_info;
    BISTO_TWS_INFO            tws_info;
} APP_BISTO_REMOTE_INFO;


bool app_bisto_roleswap_allow(void)
{
    if (app_bisto_tws.roleswap_allow == false)
    {
        app_bisto_tws.p_tws->tws_if.gsound_tws_role_change_request();
        return false;
    }
    else
    {
        return true;
    }
}


void app_bisto_roleswap_request(void)
{

}


void app_bisto_roleswap_send_all_info(void)
{
    APP_BISTO_REMOTE_INFO remote_info = {};

    app_bisto_ble_get_remote_info(&remote_info.app_ble_info);
    bisto_tws_get_info(&remote_info.tws_info);

    remote_async_msg_relay(app_bisto_tws.relay_handle, APP_BISTO_SYNC_ALL,
                           (uint8_t *)&remote_info, sizeof(remote_info), false);
}


void app_bisto_roleswap_start(void)
{
    APP_PRINT_TRACE0("app_bisto_roleswap_start");
    app_bisto_tws.p_tws->tws_if.gsound_tws_role_change_force();
    app_bisto_roleswap_send_all_info();
}


void app_bisto_roleswap_success(T_REMOTE_SESSION_ROLE role)
{
    bool to_master = false;

    if (role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        to_master = true;
    }
    else
    {
        to_master = false;
    }

    APP_PRINT_TRACE1("app_bisto_roleswap_success: switch to_master %d", to_master);

    app_bisto_tws.p_tws->tws_if.gsound_tws_role_change_target_done(to_master);
}


void app_bisto_roleswap_force(void)
{
    app_bisto_tws.p_tws->tws_if.gsound_tws_role_change_force();
}


void app_bisto_roleswap_failed(void)
{
    app_bisto_tws.p_tws->tws_if.gsound_tws_role_change_cancel();
}


static void app_bisto_roleswap_role_change_rsp(bool is_accept)
{
    app_bisto_tws.roleswap_allow = is_accept;
}


static void app_bisto_roleswap_role_change_done(void)
{
    app_bisto_tws.roleswap_allow = false;
}


static bool app_bisto_roleswap_role_is_master(void)
{
    bool ret = false;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        ret = false;
    }
    else
    {
        ret = true;
    }

    APP_PRINT_TRACE1("app_bisto_roleswap_role_is_master: is_master %d ", ret);

    return ret;
}


void app_bisto_roleswap_role_decide(void)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_bisto_ble_adv_start();
    }
    else
    {
        app_bisto_ble_adv_stop();
    }
}


void app_bisto_roleswap_handle_remote_conn_cmpl(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        app_bisto_roleswap_send_all_info();
    }
}


void app_bisto_roleswap_handle_remote_disconn_cmpl(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_bisto_ble_disconnect();
    }
}


void app_bisto_roleswap_relay_cback(uint16_t msg_id, T_REMOTE_RELAY_STATUS status,
                                    APP_BISTO_REMOTE_INFO *p_remote_info, uint16_t len)
{
    if (status != REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        return;
    }

    APP_PRINT_TRACE1("app_bisto_roleswap_relay_cback: msg_id %d", msg_id);

    app_bisto_ble_set_remote_info(&p_remote_info->app_ble_info);
    bisto_tws_set_info(&p_remote_info->tws_info);
}

void app_bisto_roleswap_init(void)
{
    BISTO_TWS_CBS cbs = {};
    bool is_support_tws = true;

    cbs.change_rsp_cb = app_bisto_roleswap_role_change_rsp;
    cbs.change_done_cb = app_bisto_roleswap_role_change_done;
    cbs.is_master_cb = app_bisto_roleswap_role_is_master;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        is_support_tws = false;
    }

    app_bisto_tws.p_tws = bisto_tws_init(&cbs, is_support_tws);

    APP_PRINT_TRACE3("app_bisto_roleswap_init: is_support_tws %d, bud_role %d, p_tws %p",
                     is_support_tws, app_cfg_nv.bud_role, app_bisto_tws.p_tws);

    // app_bt_sniffing_roleswap_cb_reg(app_bisto_roleswap_start);
    app_bisto_tws.relay_handle = remote_relay_register((P_REMOTE_RELAY_CBACK)
                                                       app_bisto_roleswap_relay_cback);
}

#endif
