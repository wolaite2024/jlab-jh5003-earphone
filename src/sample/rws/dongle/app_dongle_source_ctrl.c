#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
#include "trace.h"
#include "bt_bond.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_dongle_source_ctrl.h"
#include "app_dongle_common.h"
#include "app_dongle_dual_mode.h"
#include "app_bt_policy_api.h"
#include "app_link_util.h"
#include "app_audio_policy.h"
#include "app_dongle_spp.h"
#include "multitopology_ctrl.h"
#include "app_lea_adv.h"
#include "app_dongle_record.h"
#if F_APP_LEA_SUPPORT
#include "ascs_mgr.h"
#include "mcp_client.h"
#include "app_lea_ccp.h"
#include "app_lea_ascs.h"
#include "transmit_svc_dongle.h"
#endif
#include "app_bond.h"
static void source_switch_to_dongle_profile_handle(uint8_t b2s_num, T_APP_BR_LINK *p_dongle_link,
                                                   T_APP_BR_LINK *p_phone_link);
static void source_switch_to_bt_profile_handle(uint8_t b2s_num, T_APP_BR_LINK *p_dongle_link,
                                               T_APP_BR_LINK *p_phone_link);

#define GAMING_DONGLE_COD 0x240100

static void source_switch_to_dongle_profile_handle(uint8_t b2s_num, T_APP_BR_LINK *p_dongle_link,
                                                   T_APP_BR_LINK *p_phone_link)
{
    uint32_t handle_prof = A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK;

    if (b2s_num > 1)
    {
        //connect dongle a2dp/avrcp profile
        if (p_dongle_link != NULL)
        {
            app_bt_policy_default_connect(p_dongle_link->bd_addr, handle_prof, false);
        }

        //disc phone a2dp/avrcp profile
        if ((p_phone_link != NULL) && (p_phone_link->connected_profile & handle_prof))
        {
            app_bt_policy_disconnect(p_phone_link->bd_addr, handle_prof);
        }
    }
    else if (b2s_num == 1)
    {
        if (p_dongle_link != NULL)
        {
            //connect dongle a2dp/avrcp profile
            app_bt_policy_default_connect(p_dongle_link->bd_addr, handle_prof, false);
        }
        else
        {
            //disc phone a2dp/avrcp profile
            if ((p_phone_link != NULL) && (p_phone_link->connected_profile & handle_prof))
            {
                app_bt_policy_disconnect(p_phone_link->bd_addr, handle_prof);
            }
        }
    }
    else
    {
        //do nothing.
    }
}

void linkback_phone(void)
{
    uint32_t bond_flag;
    uint32_t plan_profs;

   uint8_t bond_num = app_bond_b2s_num_get();
        
    for (uint8_t i = 1; i <= bond_num; i++)
      {
        if (app_bond_b2s_addr_get(i, app_db.bt_addr_disconnect))
         {
             bt_bond_flag_get(app_db.bt_addr_disconnect, &bond_flag);
                if ((bond_flag & (BOND_FLAG_HFP | BOND_FLAG_HSP | BOND_FLAG_A2DP)) && !(bond_flag & (BOND_FLAG_DONGLE)))
                {
                    APP_PRINT_TRACE1("linkback_phone(): %d", app_bt_policy_get_state());
                    plan_profs = app_bt_policy_get_profs_by_bond_flag(bond_flag);
                    app_bt_policy_default_connect(app_db.bt_addr_disconnect,  plan_profs, false);
                    break;
                 }
          }
       }
}
  
uint8_t Dongle_disconect_flag = 0;
static void source_switch_to_bt_br_profile_handle(uint8_t b2s_num, T_APP_BR_LINK *p_dongle_link,
                                                  T_APP_BR_LINK *p_phone_link)
{
    uint32_t handle_prof = A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK|SPP_PROFILE_MASK;

    if (b2s_num > 1)
    {
        bool is_link_phone_profile = (p_phone_link != NULL);

#if F_APP_LEA_SUPPORT
        T_APP_LE_LINK *p_le_phone_link = app_dongle_get_le_audio_phone_link();

        if (p_le_phone_link)
        {
            is_link_phone_profile = false;
        }
#endif

        //connect phone a2dp/avrcp profile
        if (is_link_phone_profile)
        {
            app_bt_policy_default_connect(p_phone_link->bd_addr, handle_prof, false);
        }

        //disc dongle a2dp/avrcp porifle
        if ((p_dongle_link != NULL) && (p_dongle_link->connected_profile & handle_prof))
        {
            app_bt_policy_disconnect(p_dongle_link->bd_addr, handle_prof);
        }
    }
    else if (b2s_num == 1)
    {
        if (p_dongle_link != NULL)
        {
            //disc dongle a2dp/avrcp porifle
            if (p_dongle_link->connected_profile & handle_prof)
            {
                app_bt_policy_disconnect(p_dongle_link->bd_addr, handle_prof);
            }
        }
        else
        {
            //connect phone a2dp/avrcp profile
            if (p_phone_link != NULL)
            {
                app_bt_policy_default_connect(p_phone_link->bd_addr, handle_prof, false);
            }
        }
    }
    else
    {
        //do nothing.
    }
	// Dongle_disconect_flag = 1;
	   bool is_link_dongle_profile1 = (app_dongle_get_connected_dongle_link() != NULL);

	  if (p_phone_link == NULL)
	     {
	       if(is_link_dongle_profile1)
		   {	 
		     os_delay(200);
	         APP_PRINT_TRACE1("app_bt_policy_get_state: %d", is_link_dongle_profile1);
	      }
	   //  if (app_bt_policy_get_state() == BP_STATE_STANDBY)
	        // linkback_load_bond_list(0, 10);
	        linkback_phone();
	  	} 
}

#if F_APP_GAMING_CHAT_MIXING_SUPPORT
static void source_switch_to_bt_dongle_profile_handle(uint8_t b2s_num, T_APP_BR_LINK *p_dongle_link,
                                                      T_APP_BR_LINK *p_phone_link)
{
    uint32_t handle_prof = A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK;

    //connect dongle a2dp/avrcp profile
    if (p_dongle_link != NULL && !(p_dongle_link->connected_profile & handle_prof))
    {
        app_bt_policy_default_connect(p_dongle_link->bd_addr, handle_prof, false);
    }

    //connect phone a2dp/avrcp profile
    if (p_phone_link != NULL && !(p_phone_link->connected_profile & handle_prof))
    {
        app_bt_policy_default_connect(p_phone_link->bd_addr, handle_prof, false);
    }
}
#endif

#if TARGET_LE_AUDIO_GAMING
void app_dongle_update_allowed_source(void)
{
    if (app_cfg_const.enable_24g_bt_audio_source_switch)
    {
        if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_24G)
        {
            headset_status.headset_source = HEADSET_SOURCE_24G;
        }
        else if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_BT)
        {
            headset_status.headset_source = HEADSET_SOURCE_BT;
        }
    }

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        APP_PRINT_TRACE1("update allowed source to dongle: %d", headset_status.headset_source);
        app_dongle_sync_headset_status();
    }
}
#endif

#if F_APP_LEA_SUPPORT
void app_dongle_source_switch_to_dongle_le_audio_handle(uint8_t le_audio_num,
                                                        T_APP_LE_LINK *p_dongle_link,
                                                        T_APP_LE_LINK *p_phone_link)
{
    T_LEA_ASE_ENTRY *p_ase_entry;
    T_ASE_CHAR_DATA ase_data;

    if (le_audio_num == 2)
    {
        //for lea phone
        if (p_phone_link != NULL)
        {
            p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_CONN, p_phone_link->conn_handle, NULL);

            if (p_ase_entry != NULL)
            {
                //pause mcp
                if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                {
                    T_MCP_CLIENT_WRITE_MEDIA_CP_PARAM param;

                    param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PAUSE;
                    mcp_client_write_media_cp(p_ase_entry->conn_handle, 0, p_phone_link->gmcs, &param, true);

                    //release cig when mcp status notify
                    app_db.release_cis_later = true;

                }
            }
        }

#if TARGET_LE_AUDIO_GAMING
        app_dongle_update_allowed_source();
#endif
    }
    else if (le_audio_num == 1)
    {
        if (p_dongle_link != NULL)
        {
#if TARGET_LE_AUDIO_GAMING
            app_dongle_update_allowed_source();
#endif
            app_lea_adv_start();
        }
        else
        {
#if TARGET_LE_AUDIO_GAMING
            app_dongle_adv_start(false);
#endif
            if (p_phone_link != NULL)
            {
                p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_CONN, p_phone_link->conn_handle, NULL);

                if (p_ase_entry != NULL)
                {
                    //pause mcp
                    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                    {
                        T_MCP_CLIENT_WRITE_MEDIA_CP_PARAM param;

                        param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PAUSE;
                        mcp_client_write_media_cp(p_ase_entry->conn_handle, 0, p_phone_link->gmcs, &param, true);

                        //release cig when mcp status notify
                        app_db.release_cis_later = true;
                    }
                }
            }
        }
    }
    else
    {
#if TARGET_LE_AUDIO_GAMING
        app_dongle_adv_start(false);
#endif
        app_lea_adv_start();
    }

#if TARGET_LE_AUDIO_GAMING
    mtc_set_btmode(MULTI_PRO_BT_CIS);
#endif
}

void app_dongle_source_switch_to_bt_le_audio_handle(uint8_t le_audio_num,
                                                    T_APP_LE_LINK *p_dongle_link,
                                                    T_APP_LE_LINK *p_phone_link)
{
    if (le_audio_num == 2)
    {
#if TARGET_LE_AUDIO_GAMING
        app_dongle_update_allowed_source();
#endif
    }
    else if (le_audio_num == 1)
    {
        if (p_dongle_link != NULL)
        {
#if TARGET_LE_AUDIO_GAMING
            app_dongle_update_allowed_source();
#endif
            app_lea_adv_start();
        }
    }
    else
    {
#if TARGET_LE_AUDIO_GAMING
        app_dongle_adv_start(false);
#endif
        app_lea_adv_start();
    }

#if TARGET_LE_AUDIO_GAMING
    mtc_ase_release();
#endif
    mtc_set_btmode(MULTI_PRO_BT_BREDR);
}

bool app_dongle_is_allowed_le_source(T_APP_LE_LINK *p_link)
{
    bool ret = true;

    if (app_cfg_const.enable_24g_bt_audio_source_switch)
    {
        if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_24G)
        {
            if (p_link->remote_device_type != DEVICE_TYPE_DONGLE)
            {
                if (app_lea_ccp_get_call_status() == APP_CALL_IDLE)
                {
                    ret = false;
                }
            }
        }
        else if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_BT)
        {
            if (p_link->remote_device_type == DEVICE_TYPE_DONGLE)
            {
                ret = false;
            }
        }
    }

    APP_PRINT_TRACE2("app_dongle_is_allowed_le_source: ret %d  call 0x%x", ret,
                     app_lea_ccp_get_call_status());
    return ret;
}
#endif

void app_dongle_switch_allowed_source(void)
{
#if F_APP_GAMING_CHAT_MIXING_SUPPORT
    /* 2.4G -> BT -> 2.4G+BT -> 2.4G -> ... */
    T_ALLOWED_SOURCE next_state[] =
    {
        [ALLOWED_SOURCE_24G]    = ALLOWED_SOURCE_BT,
        [ALLOWED_SOURCE_BT]     = ALLOWED_SOURCE_BT_24G,
        [ALLOWED_SOURCE_BT_24G] = ALLOWED_SOURCE_24G,
    };
#else
    /* 2.4G -> BT -> 2.4G -> ... */
    T_ALLOWED_SOURCE next_state[] =
    {
        [ALLOWED_SOURCE_24G]    = ALLOWED_SOURCE_BT,
        [ALLOWED_SOURCE_BT]     = ALLOWED_SOURCE_24G,
        [ALLOWED_SOURCE_BT_24G] = ALLOWED_SOURCE_24G,   /* should not be BT_24G */
    };
#endif

     //app_dongle_set_allowed_source(next_state[app_cfg_nv.allowed_source]);
    app_dongle_set_allowed_source((T_ALLOWED_SOURCE)app_cfg_nv.allowed_source);
	APP_PRINT_TRACE1("app_dongle_switch_allowed_source: %d", app_cfg_nv.allowed_source);
}

void app_dongle_set_allowed_source(T_ALLOWED_SOURCE allowed_source)
{
    uint8_t cause = 0;

#if F_APP_DONGLE_MULTI_PAIRING
    if (app_db.cmd_dsp_config_enable_multilink)
    {
        cause = 1;
        goto exit;
    }
#endif

    app_cfg_nv.allowed_source = allowed_source;

#if F_APP_DONGLE_MULTI_PAIRING
    if (app_cfg_const.enable_dongle_multi_pairing)
    {
        if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_24G)
        {
            app_cfg_nv.is_bt_pairing = 0;
        }
        else
        {
            app_cfg_nv.is_bt_pairing = 1;
        }
    }
#endif

    if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_24G)
    {
        app_dongle_source_ctrl_evt_handler(EVENT_SOURCE_SWITCH_TO_DONGLE);

        if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
        {
            app_audio_tone_type_play(TONE_SWITCH_SOURCE_TO_DONGLE, true, true);
        }
    }
    else if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_BT)
    {
        app_dongle_source_ctrl_evt_handler(EVENT_SOURCE_SWITCH_TO_BT);

        if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
        {
            app_audio_tone_type_play(TONE_SWITCH_SOURCE_TO_BT, true, true);
        }
    }

#if F_APP_GAMING_CHAT_MIXING_SUPPORT
    else if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_BT_24G)
    {
        app_dongle_source_ctrl_evt_handler(EVENT_SOURCE_SWITCH_TO_BT_DONGLE);
        app_audio_tone_type_play(TONE_SWITCH_SOURCE_TO_BT_DONGLE, true, true);
    }
#endif

exit:
    APP_PRINT_TRACE2("app_dongle_set_allowed_source: %d, cause %d", app_cfg_nv.allowed_source, cause);
}

void app_dongle_source_ctrl_evt_handler(T_SOURCE_CTRL_EVENT event)
{
    uint8_t b2s_num = app_link_get_b2s_link_num();
    uint8_t le_audio_num = 0;
    T_APP_BR_LINK *p_br_dongle_link = app_dongle_get_connected_dongle_link();
    T_APP_BR_LINK *p_br_phone_link = app_dongle_get_connected_phone_link();

#if F_APP_LEA_SUPPORT
    T_APP_LE_LINK *p_le_dongle_link = app_dongle_get_le_audio_dongle_link();
    T_APP_LE_LINK *p_le_phone_link = app_dongle_get_le_audio_phone_link();
    le_audio_num = app_link_get_lea_link_num();
#endif

    APP_PRINT_TRACE3("app_dongle_source_ctrl_evt_handler: event 0x%02x, br_b2s_num %d, lea_num %d",
                     event, b2s_num, le_audio_num);

    switch (event)
    {
    case EVENT_SOURCE_SWITCH_TO_BT:
        {
#if F_APP_LEA_SUPPORT
            app_dongle_source_switch_to_bt_le_audio_handle(le_audio_num, p_le_dongle_link, p_le_phone_link);
#endif

            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                source_switch_to_bt_br_profile_handle(b2s_num, p_br_dongle_link, p_br_phone_link);
            }

            //For legacy gaming dongle exist
            if (p_br_dongle_link != NULL)
            {
                // request dongle to exit gaming to lower down current
                app_db.ignore_bau_first_gaming_cmd = APP_IGNORE_DONGLE_SPP_GAMING_CMD_SRC_SWITCH;
                app_db.ignore_bau_first_gaming_cmd_handled = false;
                app_dongle_gaming_mode_request(false);

                app_audio_update_dongle_flag(false);

                if (app_db.restore_gaming_mode && !app_db.gaming_mode)
                {
                    app_db.restore_gaming_mode = false;
                    app_db.disallow_play_gaming_mode_vp = true;
                    app_mmi_switch_gaming_mode();
                }

                if (app_db.dongle_is_enable_mic)
                {
                    app_db.restore_dongle_recording = true;
                    app_dongle_update_is_mic_enable(false);
                    app_dongle_stop_recording(app_cfg_nv.dongle_addr);
                }
            }
        }
        break;

    case EVENT_SOURCE_SWITCH_TO_DONGLE:
        {
#if F_APP_LEA_SUPPORT
            app_dongle_source_switch_to_dongle_le_audio_handle(le_audio_num, p_le_dongle_link, p_le_phone_link);
#endif

            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                source_switch_to_dongle_profile_handle(b2s_num, p_br_dongle_link, p_br_phone_link);
            }

            //For legacy gaming dongle exist
            if (p_br_dongle_link != NULL)
            {
                app_audio_update_dongle_flag(true);

                if (app_db.gaming_mode)
                {
                    app_db.restore_gaming_mode = true;
                    app_db.disallow_play_gaming_mode_vp = true;
                    app_db.ignore_bau_first_gaming_cmd_handled = false;
                    app_mmi_switch_gaming_mode();
                }

                if (app_db.restore_dongle_recording)
                {
                    app_dongle_update_is_mic_enable(true);
                    app_dongle_start_recording(app_cfg_nv.dongle_addr);
                }
            }
        }
        break;

#if F_APP_GAMING_CHAT_MIXING_SUPPORT
    case EVENT_SOURCE_SWITCH_TO_BT_DONGLE:
        {
            bool need_restore_dongle_a2dp = false;
            uint32_t handle_prof = A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK;

            if (p_br_dongle_link || p_br_phone_link)
            {
                if (!(p_br_dongle_link->connected_profile & handle_prof))
                {
                    need_restore_dongle_a2dp = true;
                }

                source_switch_to_bt_dongle_profile_handle(b2s_num, p_br_dongle_link, p_br_phone_link);

                if (need_restore_dongle_a2dp)
                {
                    app_audio_update_dongle_flag(true);

                    if (app_db.gaming_mode)
                    {
                        app_db.restore_gaming_mode = true;
                        app_db.disallow_play_gaming_mode_vp = true;
                        app_db.ignore_bau_first_gaming_cmd_handled = false;
                        app_mmi_switch_gaming_mode();
                    }

                    if (app_db.restore_dongle_recording && app_link_get_sco_conn_num == 0)
                    {
                        app_dongle_update_is_mic_enable(true);
                        app_dongle_start_recording(app_cfg_nv.dongle_addr);
                        app_db.restore_dongle_recording = false;
                    }
                }
            }
#if F_APP_LEA_SUPPORT
            else if (p_le_dongle_link)
            {
                /* the case that only connect with dongle */
                app_dongle_source_switch_to_dongle_le_audio_handle(le_audio_num, p_le_dongle_link, p_le_phone_link);
            }
#endif
        }
        break;
#endif

    default:
        break;
    }

}
static void app_dongle_source_ctrl_bt_cback(T_BT_EVENT event_type, void *event_buf,
                                            uint16_t buf_len)
{
    bool handle = true;
    T_BT_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BT_EVENT_HFP_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link = app_link_find_br_link(param->hfp_conn_cmpl.bd_addr);

            if (p_link && !app_link_check_dongle_link(p_link->bd_addr))
            {
                uint32_t plan_profs = (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK);

                if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_24G && (p_link->connected_profile & plan_profs))
                {
                    app_bt_policy_disconnect(p_link->bd_addr, plan_profs);
                }
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle)
   {
        APP_PRINT_TRACE1("app_dongle_source_ctrl_bt_cback: event 0x%04x", event_type);
    }
}

void app_dongle_source_ctrl_init(void)
{
  bt_mgr_cback_register(app_dongle_source_ctrl_bt_cback);
}
#endif
