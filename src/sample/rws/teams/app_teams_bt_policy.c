/*
 * Copyright (c) 2020, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_TEAMS_BT_POLICY
#include "string.h"
#include "os_mem.h"
#include "trace.h"
#include "app_cfg.h"
#include "btm.h"
#include "bt_bond.h"
#include "sysm.h"
#include "app_link_util.h"
#include "app_main.h"
#include "app_multilink.h"
#include "app_bt_policy_int.h"
#include "app_audio_policy.h"
#include "app_relay.h"
#include "app_device.h"
#include "ftl.h"
#include "app_ipc.h"
#include "app_teams_bt_policy.h"
#include "app_bond.h"
#include "bt_sdp.h"
#include "app_teams_rws.h"
#include "app_teams_ext_ftl.h"
#include "app_dsp_cfg.h"

static T_APP_DEVICE_TEMP_COD_INFO cod_data_temp[MULTILINK_SRC_CONNECTED] = {0};
static T_APP_DEVICE_COD app_teams_cod_data[TEAMS_MAX_BOND_INFO_NUM] = {0};
static uint8_t dongle_uuid[16] = {0x40, 0xbf, 0xe9, 0x57, 0x1d, 0x78, 0x49, 0x6a, 0x8b, 0x50, 0x96, 0xd5, 0xe7, 0xc9, 0x64, 0x60};
static bool app_teams_sdp_discov_flag = false;
static T_APP_TEAMS_CMD_SAVE_TYPE g_save_type = APP_TEAMS_COD_SAVE_NONE;

void app_teams_bt_policy_print_cod_info(void)
{
    ENGAGE_PRINT_TRACE8("app_teams_bt_policy_print_cod_info: bd_addr1 %s, device type %d, bd_addr2 %s, device type %d, bd_addr3 %s, device type %d, bd_addr4 %s, device type %d,",
                        TRACE_BDADDR(app_teams_cod_data[0].bd_addr), app_teams_cod_data[0].teams_device_type,
                        TRACE_BDADDR(app_teams_cod_data[1].bd_addr), app_teams_cod_data[1].teams_device_type,
                        TRACE_BDADDR(app_teams_cod_data[2].bd_addr), app_teams_cod_data[2].teams_device_type,
                        TRACE_BDADDR(app_teams_cod_data[3].bd_addr), app_teams_cod_data[3].teams_device_type);
    ENGAGE_PRINT_TRACE8("app_teams_bt_policy_print_cod_info: bd_addr5 %s, device type %d, bd_addr6 %s, device type %d, bd_addr7 %s, device type %d, bd_addr8 %s, device type %d,",
                        TRACE_BDADDR(app_teams_cod_data[4].bd_addr), app_teams_cod_data[4].teams_device_type,
                        TRACE_BDADDR(app_teams_cod_data[5].bd_addr), app_teams_cod_data[5].teams_device_type,
                        TRACE_BDADDR(app_teams_cod_data[6].bd_addr), app_teams_cod_data[6].teams_device_type,
                        TRACE_BDADDR(app_teams_cod_data[7].bd_addr), app_teams_cod_data[7].teams_device_type);
}

bool app_teams_bt_policy_save_cod_info(void)
{
    uint32_t ret = 0;

    if (sizeof(app_teams_cod_data) > APP_TEAMS_BT_COD_INFO_SIZE)
    {
        //save info size > ftl space
        return false;
    }

    app_teams_bt_policy_print_cod_info();

#if (FTL_POOL_ENABLE == 1)
    if (app_teams_ext_save_ftl_data(APP_TEAMS_BT_COD_INFO_ADDR, (uint8_t *)app_teams_cod_data,
                                    sizeof(app_teams_cod_data)) != 0)
#else
    if (ftl_save_to_storage(app_teams_cod_data, APP_TEAMS_BT_COD_INFO_ADDR,
                            sizeof(app_teams_cod_data)) != 0)
#endif
    {
        return false;
    }

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        return app_relay_async_single(APP_MODULE_TYPE_TEAMS_APP_BT_POLICY, TEAMS_APP_RWS_DEV_INFO_MSG);
    }
    return true;
}

bool app_teams_bt_policy_load_cod_info(void)
{
    uint32_t ret = 0;

    if (sizeof(app_teams_cod_data) > APP_TEAMS_BT_COD_INFO_SIZE)
    {
        //save info size > ftl space
        return false;
    }

#if (FTL_POOL_ENABLE == 1)
    if (app_teams_ext_load_ftl_data(APP_TEAMS_BT_COD_INFO_ADDR, (uint8_t *)app_teams_cod_data,
                                    sizeof(app_teams_cod_data)) != 0)
#else
    if (ftl_load_from_storage(app_teams_cod_data, APP_TEAMS_BT_COD_INFO_ADDR,
                              sizeof(app_teams_cod_data)) == 0)
#endif
    {
        app_teams_bt_policy_print_cod_info();
        return true;
    }
    else
    {
        return false;
    }
}

static bool app_teams_bt_policy_add_cod(uint8_t *bd_addr, T_TEAMS_DEVICE_TYPE device_type)
{
    ENGAGE_PRINT_TRACE2("app_teams_bt_policy_add_cod: bd_addr %s, device type %d",
                        TRACE_BDADDR(bd_addr), device_type);

    for (uint8_t i = 0; i < TEAMS_MAX_BOND_INFO_NUM; i++)
    {
        //only used for le secure connect and ltk derive to link key
        if (app_teams_cod_data[i].used && (memcmp(app_teams_cod_data[i].bd_addr, bd_addr, 6) == 0))
        {
            app_teams_cod_data[i].teams_device_type = device_type;
            app_teams_bt_policy_save_cod_info();
            return true;
        }
    }

    for (uint8_t i = 0; i < TEAMS_MAX_BOND_INFO_NUM; i++)
    {
        if (app_teams_cod_data[i].used == false)
        {
            app_teams_cod_data[i].used = true;
            memcpy(app_teams_cod_data[i].bd_addr, bd_addr, 6);
            app_teams_cod_data[i].teams_device_type = device_type;
            memcpy(app_teams_cod_data[i].master_device_name, temp_master_device_name, 40);
            app_teams_cod_data[i].master_device_name_len = temp_master_device_name_len;
            app_teams_bt_policy_save_cod_info();
            return true;
        }
    }
    return false;
}

bool app_teams_bt_policy_del_cod(uint8_t *bd_addr)
{
    ENGAGE_PRINT_TRACE1("app_teams_bt_policy_del_cod: bd_addr %s",
                        TRACE_BDADDR(bd_addr));
    for (uint8_t i = 0; i < TEAMS_MAX_BOND_INFO_NUM; i++)
    {
        if (!memcmp(bd_addr, app_teams_cod_data[i].bd_addr, 6))
        {
            memset(&app_teams_cod_data[i], 0, sizeof(T_APP_DEVICE_COD));
            app_teams_bt_policy_save_cod_info();
            return true;
        }
    }
    return false;
}

void app_teams_bt_policy_clear_cod(void)
{
    ENGAGE_PRINT_TRACE0("app_teams_bt_policy_clear_cod");
    memset(app_teams_cod_data, 0, TEAMS_MAX_BOND_INFO_NUM * sizeof(T_APP_DEVICE_COD));
    app_teams_bt_policy_save_cod_info();
}

void app_teams_bt_policy_handle_factory_reset(void)
{
    APP_PRINT_TRACE3("app_teams_bt_policy_handle_factory_reset: cod_data %p, factory_reset_clear_mode %d, device_state %d",
                     app_teams_cod_data, app_db.factory_reset_clear_mode, app_db.device_state);
    if (app_teams_cod_data != NULL)
    {
        if (app_db.factory_reset_clear_mode == FACTORY_RESET_CLEAR_CFG)
        {
            //do nothing
        }
        else if (app_db.factory_reset_clear_mode == FACTORY_RESET_CLEAR_NORMAL)
        {
            for (uint8_t i = 0; i < TEAMS_MAX_BOND_INFO_NUM; i++)
            {
                if (app_teams_cod_data[i].used &&
                    (app_teams_cod_data[i].teams_device_type != T_TEAMS_DEVICE_DONGLE_TYPE))
                {
                    app_teams_bt_policy_del_cod(app_teams_cod_data[i].bd_addr);
                }
            }
        }
        else if (app_db.factory_reset_clear_mode == FACTORY_RESET_CLEAR_ALL)
        {
            app_teams_bt_policy_clear_cod();
        }
    }
}

static T_APP_DEVICE_TEMP_COD_INFO *app_teams_bt_policy_get_free_cod_temp_info(void)
{
    for (uint8_t i = 0; i < MULTILINK_SRC_CONNECTED; i++)
    {
        if (cod_data_temp[i].used == false)
        {
            APP_PRINT_TRACE1("app_teams_bt_policy_get_free_cod_temp_info: index %d", i);
            return &cod_data_temp[i];
        }
    }
    return NULL;
}

static T_APP_DEVICE_TEMP_COD_INFO *app_teams_bt_policy_get_cod_temp_info_by_addr(uint8_t *bd_addr)
{
    for (uint8_t i = 0; i < MULTILINK_SRC_CONNECTED; i++)
    {
        if ((cod_data_temp[i].used == true) && (!memcmp(cod_data_temp[i].bd_addr, bd_addr, 6)))
        {
            APP_PRINT_TRACE1("app_teams_bt_policy_get_cod_temp_info_by_addr: index %d", i);
            return &cod_data_temp[i];
        }
    }
    return NULL;
}

void app_teams_bt_policy_delete_cod_temp_info_by_addr(uint8_t *bd_addr)
{
    for (uint8_t i = 0; i < MULTILINK_SRC_CONNECTED; i++)
    {
        if ((cod_data_temp[i].used == true) && (!memcmp(cod_data_temp[i].bd_addr, bd_addr, 6)))
        {
            APP_PRINT_TRACE1("app_teams_bt_policy_delete_cod_temp_info_by_addr: index %d", i);
            cod_data_temp[i].used = false;
        }
    }
}

static T_TEAMS_DEVICE_TYPE app_teams_bt_policy_resolving_device_cod(uint32_t cod)
{
    if (cod & TEAMS_DEVICE_COMPUTER_MASK)
    {
        return T_TEAMS_DEVICE_COMPUTER_TYPE;
    }
    else if (cod & TEAMS_DEVICE_PHONE_MASK)
    {
        return T_TEAMS_DEVICE_PHONE_TYPE;
    }

    return T_TEAMS_DEVICE_DEFAULT_TYPE;
}

bool app_teams_bt_policy_check_device_cod_info_exist_by_addr(uint8_t *bd_addr)
{
    for (uint8_t i = 0; i < TEAMS_MAX_BOND_INFO_NUM; i++)
    {
        if (app_teams_cod_data[i].used && (!memcmp(bd_addr, app_teams_cod_data[i].bd_addr, 6)))
        {
            return true;
        }
    }
    return false;
}

bool app_teams_bt_policy_check_device_cod_info_exist_by_type(T_TEAMS_DEVICE_TYPE device_type)
{
    for (uint8_t i = 0; i < TEAMS_MAX_BOND_INFO_NUM; i++)
    {
        if (app_teams_cod_data[i].used && (app_teams_cod_data[i].teams_device_type == device_type))
        {
            return true;
        }
    }
    return false;
}

uint8_t *app_teams_bt_policy_get_dongle_addr(void)
{
    for (uint8_t i = 0; i < TEAMS_MAX_BOND_INFO_NUM; i++)
    {
        if (app_teams_cod_data[i].used &&
            (app_teams_cod_data[i].teams_device_type == T_TEAMS_DEVICE_DONGLE_TYPE))
        {
            return app_teams_cod_data[i].bd_addr;
        }
    }
    return NULL;
}

static T_APP_DEVICE_COD *app_teams_bt_policy_find_cod_info_by_addr(uint8_t *bd_addr)
{
    for (uint8_t i = 0; i < TEAMS_MAX_BOND_INFO_NUM; i++)
    {
        if (app_teams_cod_data[i].used && !memcmp(bd_addr, app_teams_cod_data[i].bd_addr, 6))
        {
            ENGAGE_PRINT_TRACE2("app_teams_bt_policy_find_cod_info_by_addr: index %d, bd_addr %s",
                                i, TRACE_BDADDR(bd_addr));
            return &app_teams_cod_data[i];
        }
    }
    return NULL;
}

bool app_teams_bt_policy_set_cod_type_by_addr(uint8_t *bd_addr, T_TEAMS_DEVICE_TYPE device_type)
{
    T_APP_DEVICE_COD *cod_info = NULL;

    cod_info = app_teams_bt_policy_find_cod_info_by_addr(bd_addr);
    if (cod_info)
    {
        cod_info->teams_device_type = device_type;
        return true;
    }
    return false;
}

T_TEAMS_DEVICE_TYPE app_teams_bt_policy_get_cod_type_by_addr(uint8_t *bd_addr)
{
    T_APP_DEVICE_COD *cod_info = NULL;

    cod_info = app_teams_bt_policy_find_cod_info_by_addr(bd_addr);
    if (cod_info)
    {
        return cod_info->teams_device_type;
    }
    return T_TEAMS_DEVICE_DEFAULT_TYPE;
}

void app_teams_bt_policy_clear_all_cod_info(void)
{
    for (uint8_t i = 0; i < TEAMS_MAX_BOND_INFO_NUM; i++)
    {
        app_teams_cod_data[i].used = false;
    }
}

uint8_t *app_teams_bt_policy_find_least_priority_non_dongle_device(void)
{
    uint8_t bond_num = app_bond_b2s_num_get();
    uint8_t addr_temp[6] = {0};
    T_APP_DEVICE_COD *cod_info = NULL;

    for (uint8_t i = bond_num; i > 0; i--)
    {
        app_bond_b2s_addr_get(bond_num, addr_temp);
        cod_info = app_teams_bt_policy_find_cod_info_by_addr(addr_temp);
        if (cod_info->teams_device_type != T_TEAMS_DEVICE_DONGLE_TYPE)
        {
            return cod_info->bd_addr;
        }
    }
    return NULL;
}

uint8_t *app_teams_bt_policy_find_dongle_device(void)
{
    uint8_t index = 0;
    for (; index < MAX_BR_LINK_NUM; index++)
    {
        if (app_link_check_b2s_link_by_id(index))
        {
            if (T_TEAMS_DEVICE_DONGLE_TYPE == app_teams_bt_policy_get_cod_type_by_addr(
                    app_db.br_link[index].bd_addr))
            {
                return app_db.br_link[index].bd_addr;
            }
        }
    }
    return NULL;
}

bool app_teams_bt_policy_get_link_state_by_device_type(T_TEAMS_DEVICE_TYPE device_type)
{
    uint8_t count = 0;
    T_APP_DEVICE_COD *device_cod_info = NULL;
    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].used)
        {
            device_cod_info = app_teams_bt_policy_find_cod_info_by_addr(app_db.br_link[i].bd_addr);
            if (device_cod_info && device_cod_info->teams_device_type == device_type)
            {
                count++;
                break;
            }
        }
    }

    if (count)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool app_teams_bt_policy_prepare_for_dedicated_enter_pairing_mode(void)
{
    uint8_t index = MAX_BR_LINK_NUM;
    uint8_t dongle_index = 0;
    uint8_t *dongle_bd_addr = NULL;
    T_APP_BR_LINK *dongle_link = NULL;

    dongle_bd_addr = app_teams_bt_policy_find_dongle_device();
    /*if one of remote device is dongle, disconnect the other remote even this device is play music, keep dongle*/
    if (dongle_bd_addr)
    {
        dongle_link = app_link_find_br_link(dongle_bd_addr);
        if (dongle_link)
        {
            dongle_index = dongle_link->id;
            for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if (app_link_check_b2s_link_by_id(i))
                {
                    if (i != dongle_index)
                    {
                        index = i;
                        break;
                    }
                }
            }
        }
    }

    if (index != MAX_BR_LINK_NUM)
    {
        ENGAGE_PRINT_TRACE1("[TEAMS]prepare_for_dedicated_enter_pairing_mode: disconnect device, %s",
                            TRACE_BDADDR(app_db.br_link[index].bd_addr));
        app_bt_policy_disconnect(app_db.br_link[index].bd_addr, ALL_PROFILE_MASK);
        return true;
    }
    return false;
}

void app_teams_bt_policy_save_bond_info_after_cod_set(uint8_t *bd_addr)
{
    T_APP_DEVICE_TEMP_COD_INFO *cod_info_temp = NULL;
    T_APP_REMOTE_MSG_PAYLOAD_LINK_KEY_ADDED link_key;
    uint8_t mapping_idx;
    uint8_t reset_volume = 0;

    cod_info_temp = app_teams_bt_policy_get_cod_temp_info_by_addr(bd_addr);
    if (cod_info_temp)
    {
        /*set bond info */
        app_bond_key_set(cod_info_temp->bd_addr, cod_info_temp->link_key, cod_info_temp->key_type);

        app_bt_policy_update_pair_idx_mapping();
        if (app_bond_get_pair_idx_mapping(cod_info_temp->bd_addr, &mapping_idx))
        {
            T_APP_BR_LINK *p_link = app_link_find_br_link(cod_info_temp->bd_addr);
            if (p_link)
            {
                /*
                    Avoid LE CTKD link key update event
                */
                if (!(p_link->connected_profile & A2DP_PROFILE_MASK))
                {
                    app_cfg_nv.audio_gain_level[mapping_idx] = app_dsp_cfg_vol.playback_volume_default;
                    app_cfg_nv.voice_gain_level[mapping_idx] = app_dsp_cfg_vol.voice_out_volume_default;
                    reset_volume = 1;
                }
            }
        }

        /*rws handle*/
        link_key.key_type = cod_info_temp->key_type;
        memcpy(link_key.bd_addr, cod_info_temp->bd_addr, 6);
        memcpy(link_key.link_key, cod_info_temp->link_key, 16);
        link_key.reset_volume = reset_volume;
        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DEVICE, APP_REMOTE_MSG_LINK_RECORD_ADD,
                                            (uint8_t *)&link_key, sizeof(T_APP_REMOTE_MSG_PAYLOAD_LINK_KEY_ADDED));

    }
    else
    {
        APP_PRINT_ERROR1("[TEAMS]app_teams_bt_policy_save_bond_info_after_cod_set: no cod temp info found,bd_addr %s",
                         TRACE_BDADDR(bd_addr));
    }
}

void app_teams_bt_policy_handle_link_auth_cmpl(uint8_t *bd_addr)
{
    bt_link_tpoll_range_set(bd_addr, 0x6, 0x100);
}

bool app_teams_bt_policy_handle_auth_link_key_info(T_BT_PARAM *param)
{
    T_TEAMS_DEVICE_TYPE cod_type;
    T_APP_DEVICE_TEMP_COD_INFO *cod_temp_info = NULL;
    bool rcv_acl_conn_req_flag = true;
    uint8_t *addr_point = NULL;
    uint8_t teams_bond_num = app_bond_b2s_num_get();
    cod_temp_info = app_teams_bt_policy_get_cod_temp_info_by_addr(param->bd_addr);
    if (cod_temp_info)
    {
        /* resolve cod to device type*/
        cod_type = app_teams_bt_policy_resolving_device_cod(cod_temp_info->cod);
    }
    else
    {
        rcv_acl_conn_req_flag = false;
        cod_type = T_TEAMS_DEVICE_DEFAULT_TYPE;
    }
    APP_PRINT_INFO2("app_teams_bt_policy_handle_auth_link_key_info: new cod type %d, rcv_acl_conn_req_flag %d",
                    cod_type, rcv_acl_conn_req_flag);

    /*if device is phone or computer, storage cod info; if not , continue to check is dongle?*/
    if (cod_type == T_TEAMS_DEVICE_DEFAULT_TYPE && rcv_acl_conn_req_flag)
    {
        /* search dongle sdp service, check this device is dongle? */
        T_GAP_UUID_DATA uuid_data;
        memcpy(uuid_data.uuid_128, dongle_uuid, 16);
        cod_temp_info->key_type = param->key_type;
        memcpy(cod_temp_info->link_key, param->link_key, 16);
        gap_br_start_sdp_discov(param->bd_addr, GAP_UUID128, uuid_data);
        app_teams_sdp_discov_flag = true;
        return true;
    }
    else if (cod_type == T_TEAMS_DEVICE_DEFAULT_TYPE && !rcv_acl_conn_req_flag &&
             app_teams_bt_policy_check_device_cod_info_exist_by_addr(param->bd_addr))
    {
        //only used for le secure connect that has paired with host, not save cod info
        APP_PRINT_TRACE0("app_teams_bt_policy_handle_auth_link_key_info: bond info for paired device, so not svae cod info");
        return false;
    }
    else
    {
        //computer or phone or default type(only happen for le secure connect that not pair with host)
        addr_point = app_teams_bt_policy_find_least_priority_non_dongle_device();
        /* if storage bond info num reach max, delete priority minimum non dongle device*/
        if ((teams_bond_num == TEAMS_MAX_BOND_INFO_NUM) && addr_point)
        {
            bt_bond_delete(addr_point);
            app_teams_bt_policy_del_cod(addr_point);
        }

        /* storage device type in flash*/
        APP_PRINT_TRACE2("app_teams_bt_policy_handle_auth_link_key_info: new device(%s), device type %d",
                         TRACE_BDADDR(param->bd_addr), cod_type);
        app_teams_bt_policy_add_cod(param->bd_addr, cod_type);
        return false;
    }
}

void app_teams_bt_policy_handle_acl_conn_ind(uint8_t *bd_addr, uint32_t cod)
{
    T_APP_DEVICE_TEMP_COD_INFO *cod_temp_info = NULL;
    T_APP_DEVICE_COD *cod_info = NULL;
    T_TEAMS_DEVICE_TYPE cod_type;
    cod_temp_info = app_teams_bt_policy_get_cod_temp_info_by_addr(bd_addr);
    if (!cod_temp_info)
    {
        cod_temp_info = app_teams_bt_policy_get_free_cod_temp_info();
    }
    if (cod_temp_info)
    {
        APP_PRINT_TRACE2("app_teams_bt_policy_handle_acl_conn_ind: bd addr %s, cod %x",
                         TRACE_BDADDR(bd_addr),
                         cod);
        cod_temp_info->used = true;
        memcpy(cod_temp_info->bd_addr, bd_addr, 6);
        cod_temp_info->cod = cod;
    }
    else
    {
        APP_PRINT_TRACE4("app_teams_bt_handle_acl_conn_ind: temp data full, index 0 %s, cod 0x%x, index 1 %s, cod 0x%x",
                         TRACE_BDADDR(cod_data_temp[0].bd_addr), cod_data_temp[0].cod,
                         TRACE_BDADDR(cod_data_temp[1].bd_addr), cod_data_temp[1].cod);
    }

    cod_info = app_teams_bt_policy_find_cod_info_by_addr(bd_addr);
    cod_type = app_teams_bt_policy_resolving_device_cod(cod);
    if (cod_info && cod_info->teams_device_type == T_TEAMS_DEVICE_DEFAULT_TYPE &&
        cod_type != T_TEAMS_DEVICE_DEFAULT_TYPE)
    {
        cod_info->teams_device_type = cod_type;
        app_teams_bt_policy_save_cod_info();
    }
}

void app_teams_bt_policy_handle_sdp_cmpl_or_stop(uint8_t *bd_addr)
{
    uint8_t *addr_point = NULL;
    uint8_t teams_bond_num = app_bond_b2s_num_get();

    if (app_teams_sdp_discov_flag)
    {
        app_teams_sdp_discov_flag = false;

        addr_point = app_teams_bt_policy_find_least_priority_non_dongle_device();
        /* if storage bond info num reach max, delete priority minimum non dongle device*/
        if ((teams_bond_num == TEAMS_MAX_BOND_INFO_NUM) && addr_point)
        {
            bt_bond_delete(addr_point);
            app_teams_bt_policy_del_cod(addr_point);
        }

        /* storage device type in flash*/
        APP_PRINT_TRACE2("app_teams_bt_policy_handle_sdp_cmpl_or_stop: new device(%s), device type %d",
                         TRACE_BDADDR(bd_addr),
                         T_TEAMS_DEVICE_DEFAULT_TYPE);
        app_teams_bt_policy_add_cod(bd_addr, T_TEAMS_DEVICE_DEFAULT_TYPE);

        app_teams_bt_policy_save_bond_info_after_cod_set(bd_addr);
    }
}

void app_teams_bt_policy_handle_sdp_info(uint8_t *bd_addr, T_BT_SDP_ATTR_INFO *sdp_info)
{
    bool dongle_flag = false;
    uint8_t *addr_temp = NULL;
    uint8_t  *p_attr;
    uint8_t  *p_elem;
    uint32_t  elem_len;
    uint8_t   elem_type;
    uint8_t dongle_uuid[16] = {0x40, 0xbf, 0xe9, 0x57, 0x1d, 0x78, 0x49, 0x6a, 0x8b, 0x50, 0x96, 0xd5, 0xe7, 0xc9, 0x64, 0x60};
    uint8_t dongle_cap[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x10, 0, 0, 0, 0x20};

    if (sdp_info->srv_class_uuid_type == GAP_UUID128)
    {
        if (memcmp(&dongle_uuid, &sdp_info->srv_class_uuid_data, 16) == 0)
        {
            app_teams_sdp_discov_flag = false;
            p_attr = bt_sdp_attr_find(sdp_info->p_attr, sdp_info->attr_len, 0x0210);
            if (p_attr)
            {
                p_elem = bt_sdp_elem_decode(p_attr, sdp_info->attr_len, &elem_len, &elem_type);
                if (p_elem && (elem_len == 32))
                {
                    if ((p_elem[31]&dongle_cap[31]) && (p_elem[27]&dongle_cap[27]))
                    {
                        dongle_flag = true;
                    }
                }
                else
                {
                }
            }
            else
            {
            }

            if (dongle_flag)
            {
                app_audio_tone_type_play(TONE_CONNECTED_TO_USB_LINK, false, true);
            }
            else
            {
                app_audio_tone_type_play(TONE_CONNECTED, false, true);
            }

            /*if this remote is dongle and device has dongle bond info, so delete old bond info*/
            if (dongle_flag &&
                app_teams_bt_policy_check_device_cod_info_exist_by_type(T_TEAMS_DEVICE_DONGLE_TYPE))
            {
                addr_temp = app_teams_bt_policy_get_dongle_addr();
            }
            /*reach max bond num, if this remote is dongle and device has no dongle bond info, or remote is non dongle device, so delete least priority non dongle bond info*/
            else if (TEAMS_MAX_BOND_INFO_NUM == app_bond_b2s_num_get())
            {
                addr_temp = app_teams_bt_policy_find_least_priority_non_dongle_device();
            }

            if (addr_temp)
            {
                bt_bond_delete(addr_temp);
                app_teams_bt_policy_del_cod(addr_temp);
            }

            /* storage device type in flash*/
            APP_PRINT_TRACE2("app_teams_bt_policy_handle_sdp_info: new device(%s), device type %d",
                             TRACE_BDADDR(bd_addr),
                             dongle_flag ? T_TEAMS_DEVICE_DONGLE_TYPE : T_TEAMS_DEVICE_DEFAULT_TYPE);
            app_teams_bt_policy_add_cod(bd_addr,
                                        dongle_flag ? T_TEAMS_DEVICE_DONGLE_TYPE : T_TEAMS_DEVICE_DEFAULT_TYPE);

            if (dongle_flag)
            {
#if F_APP_COMMON_DONGLE_SUPPORT
                app_bond_add_dongle_bond_info(bd_addr);
#endif
            }

            app_teams_bt_policy_save_bond_info_after_cod_set(bd_addr);
        }
    }
}

void app_teams_bt_policy_set_eir_data(void)
{
    uint8_t app_teams_eir[11] = {0};
    gap_br_set_did_eir(RTK_COMPANY_ID, VENDOR_ID_SOURCE, PRODUCT_ID, PRODUCT_VERSION);
    app_teams_eir[0] = 2;
    app_teams_eir[1] = 0x0a;
    app_teams_eir[2] = 0x07;/*tx power level*/
    app_teams_eir[3] = 7;
    app_teams_eir[4] = 0xff;
    app_teams_eir[5] = 0x06;
    app_teams_eir[6] = 0x00;
    app_teams_eir[7] = 0x31;
    app_teams_eir[8] = 0x39;
    app_teams_eir[9] = 0x31;
    app_teams_eir[10] = 0x39; /*manufacture specific data*/
    gap_br_set_ext_eir(app_teams_eir, sizeof(app_teams_eir));
}

static void app_teams_bt_policy_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    switch (event_type)
    {
    case SYS_EVENT_POWER_ON:
        {
            app_teams_bt_policy_set_eir_data();
        }
        break;

    default:
        break;
    }
}

bool app_teams_bt_policy_master_device_name_get_by_addr(uint8_t *bd_addr,
                                                        uint8_t *master_device_name,
                                                        uint8_t *master_device_name_len)
{
    T_APP_DEVICE_COD *cod_info = NULL;

    cod_info = app_teams_bt_policy_find_cod_info_by_addr(bd_addr);
    if (cod_info)
    {
        memcpy(master_device_name, cod_info->master_device_name, cod_info->master_device_name_len);
        *master_device_name_len = cod_info->master_device_name_len;
        return true;
    }

    return false;
}

void app_teams_bt_policy_build_relay_info(T_APP_TEAMS_RELAY_BT_POLICY_INFO *relay_info)
{
    memcpy((uint8_t *)relay_info->cod_info, (uint8_t *)app_teams_cod_data, sizeof(app_teams_cod_data));
}

void app_teams_bt_policy_handle_relay_info(T_APP_TEAMS_RELAY_BT_POLICY_INFO *relay_info,
                                           uint16_t data_size)
{
    if (data_size != sizeof(T_APP_TEAMS_RELAY_BT_POLICY_INFO))
    {
        return;
    }

    memcpy((uint8_t *)app_teams_cod_data, (uint8_t *)relay_info->cod_info, sizeof(app_teams_cod_data));
    app_teams_bt_policy_save_cod_info();
}

static void app_teams_bt_policy_factory_reset_cback(uint32_t event, void *msg)
{
    switch (event)
    {
    case APP_DEVICE_IPC_EVT_FACTORY_RESET:
        {
            app_teams_bt_policy_handle_factory_reset();
        }
        break;

    default:
        break;
    }
}

void app_teams_bt_policy_init(void)
{
    app_teams_bt_policy_load_cod_info();
    sys_mgr_cback_register(app_teams_bt_policy_dm_cback);

    app_ipc_subscribe(APP_DEVICE_IPC_TOPIC, app_teams_bt_policy_factory_reset_cback);
}
#endif
