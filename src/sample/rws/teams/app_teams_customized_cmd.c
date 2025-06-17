#if F_APP_TEAMS_FEATURE_SUPPORT

#if F_APP_TEAMS_CUSTOMIZED_CMD_SUPPORT
#include <stdlib.h>
#include <string.h>
#include <os_mem.h>
#include <trace.h>
#include "stdlib_corecrt.h"
#include "app_audio_policy.h"
#include "gap.h"
#include "app_main.h"
#include "app_cfg.h"
#include "eq_utils.h"
#include "app_eq.h"
#include "app_teams_cmd.h"
#include "app_teams_customized_cmd.h"
#include "app_hfp.h"
#include "app_bond.h"
#include "app_teams_audio_policy.h"
#include "app_teams_bt_policy.h"
#include "app_bt_policy_int.h"
#include "app_bt_policy_api.h"
#include "app_mmi.h"
#include "app_key_gpio.h"
#include "bt_bond.h"
#include "bt_hfp.h"
#include "hid_api.h"
#include "hid_vp_update.h"
#include "app_teams_hid.h"
#include "bt_hid_device.h"
#include "btm.h"
#include "app_main.h"
#include "app_charger.h"
#include "app_device.h"
#include "hal_gpio.h"
#include "version.h"
#include "voice_prompt.h"
#include "app_link_util.h"
#include "app_led.h"
#include "fmc_api.h"
#include "sysm.h"
#include <math.h>
#include "app_teams_bt_policy.h"
#include "gap_le.h"

#define TEAMS_MAX_DEVICE_NAME_LENGTH    40
#define PDU_MAX_LENGTH                  63

static uint8_t app_teams_customized_cmd_hex2bcd(uint8_t val_hex)
{
    uint8_t val_bcd;
    uint8_t temp;
    temp = val_hex % 100;
    val_bcd = val_hex / 100 << 8;
    val_bcd = val_bcd | temp / 10 << 4;
    val_bcd = val_bcd | temp % 10;

    return val_bcd;
}

static void app_teams_customized_cmd_reverse_intcpy(uint8_t *dst, const uint8_t *src, uint16_t n)
{
    for (uint16_t i = 0; i < n; ++i)
    {
        dst[n - 1 - i] = src[i];
    }

}

static uint32_t app_teams_customized_cmd_count(uint32_t i)
{
    uint32_t num = 0;
    while (i != 0)
    {
        i &= (i - 1);
        num++;
    }
    return num;
}

static bool app_teams_customized_cmd_set_led_mode(uint8_t led_status)
{
    bool ret = true;
    switch (led_status)
    {
    case 0:
        {
            app_led_change_mode(LED_MODE_ALL_OFF, true, false);
        }
        break;

    case 0x01:
        {
            app_led_change_mode(LED_MODE_BUD_ROLE_SECONDARY, true, false);//teams led1
        }
        break;

    case 0x02:
        {
            app_led_change_mode(LED_MODE_BUD_FIRST_ENGAGE_CREATION, true, false);//hook and paring led2
        }
        break;

    case 0x04:
        {
            app_led_change_mode(LED_MODE_BUD_ROLE_PRIMARY, true, false);//mute led0
        }
        break;

    default:
        {
            ret = false;
        }
        break;
    }
    return ret;
}

void app_teams_customized_cmd_handle_data(uint8_t *bd_addr, uint8_t report_id, uint8_t report_type,
                                          T_APP_TEAMS_HID_REPORT_ACTION_TYPE report_action,
                                          T_APP_TEAMS_HID_REPORT_SOURCE report_source,
                                          uint16_t len, uint8_t *data)
{
    uint8_t current_cmd_id = data[0];
    uint8_t pdu[PDU_MAX_LENGTH] = {0};
    uint8_t length = 0;
    uint16_t rsp_len = 63;
    bool ret = false;
    uint8_t *p_buff = NULL;
    uint16_t buff_len = 0;
    pdu[length++] = APP_CONFIG_D2H;
    APP_PRINT_INFO1("app_teams_handle_customized_cmd: current_command_id 0x%x", current_cmd_id);

    switch (current_cmd_id)
    {
    case TEAMS_CMD_GET_DEVICE_CAPABILITY:
        {
            pdu[length++] = TEAMS_CMD_GET_DEVICE_CAPABILITY;
            ret = app_teams_cmd_get_device_capability((const void **)&p_buff, &buff_len);
            if (ret && buff_len == 0x10)
            {
                pdu[length++] = TEAMS_RSP_STATUS_OK;
                pdu[length++] = 0x10;
                memcpy(&pdu[length], p_buff, 16);
                length += 16;
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                pdu[length++] = 0;
            }
        }
        break;

    case TEAMS_CMD_GET_CHARGE_STATUS:
        {
            pdu[length++] = TEAMS_CMD_GET_CHARGE_STATUS;
            pdu[length++] = TEAMS_RSP_STATUS_OK;
            pdu[length++] = 0x01;
            T_APP_CHARGER_STATE app_charger_state;
            app_charger_state = app_charger_get_charge_state();

            if (app_charger_state == APP_CHARGER_STATE_NO_CHARGE)
            {
                pdu[length++] = 0x00;
            }
            else if (app_charger_state == APP_CHARGER_STATE_CHARGING)
            {
                pdu[length++] = 0x01;
            }
            else if (app_charger_state == APP_CHARGER_STATE_CHARGE_FINISH)
            {
                pdu[length++] = 0x02;
            }
        }
        break;

    case TEAMS_CMD_GET_SERIAL_NUMBER:
        {
            pdu[length++] = TEAMS_CMD_GET_SERIAL_NUMBER;
            ret = app_teams_cmd_get_device_serial_number((const void **)&p_buff, &buff_len);
            if (ret && buff_len <= 16)
            {
                pdu[length++] = TEAMS_RSP_STATUS_OK;
                if (p_buff[0] > 15)
                {
                    pdu[length++] = 0x0F;
                    for (uint8_t i = 0; i < 15; i++)
                    {
                        pdu[length++] = 0;
                    }
                }
                else
                {
                    pdu[length++] = p_buff[0];
                    for (uint8_t i = 0; i < p_buff[0]; i++)
                    {
                        pdu[length++] = p_buff[i + 1];
                    }
                }
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                pdu[length++] = 0;
            }
        }
        break;

    case TEAMS_CMD_GET_BT_ADDR:
        {
            pdu[length++] = TEAMS_CMD_GET_BT_ADDR;
            pdu[length++] = TEAMS_RSP_STATUS_OK;
            pdu[length++] = 0x06;
            pdu[length++] = app_cfg_nv.bud_local_addr[5];
            pdu[length++] = app_cfg_nv.bud_local_addr[4];
            pdu[length++] = app_cfg_nv.bud_local_addr[3];
            pdu[length++] = app_cfg_nv.bud_local_addr[2];
            pdu[length++] = app_cfg_nv.bud_local_addr[1];
            pdu[length++] = app_cfg_nv.bud_local_addr[0];
        }
        break;

    case TEAMS_CMD_GET_COLOR_NAME:
        {
            pdu[length++] = TEAMS_CMD_GET_COLOR_NAME;
            ret = app_teams_cmd_get_color_code((const void **)&p_buff, &buff_len);
            if (ret && buff_len == 16)
            {
                pdu[length++] = TEAMS_RSP_STATUS_OK;
                pdu[length++] = 0x10;
                memcpy(&pdu[length], p_buff, 16);
                length += 16;
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                pdu[length++] = 0;
            }
        }
        break;

    case TEAMS_CMD_GET_FIRMWARE_VERSION:
        {
            pdu[length++] = TEAMS_CMD_GET_FIRMWARE_VERSION;
            pdu[length++] = TEAMS_RSP_STATUS_OK;
            pdu[length++] = 0x06;
            pdu[length++] = app_teams_customized_cmd_hex2bcd((uint8_t)VERSION_MAJOR);
            pdu[length++] = app_teams_customized_cmd_hex2bcd((uint8_t)VERSION_REVISION);
            pdu[length++] = (uint8_t)VERSION_BUILDNUM;
            pdu[length++] = (uint8_t)VERSION_REVISION;
            pdu[length++] = (uint8_t)VERSION_MINOR;
            pdu[length++] = (uint8_t)VERSION_MAJOR;
        }
        break;

    case TEAMS_CMD_SET_CURRENT_EQ:
        {
            pdu[length++] = TEAMS_CMD_SET_CURRENT_EQ;
            if (data[1] >= 1)
            {
                if (app_teams_cmd_data.teams_test_mode == TEAMS_MTE_MODE)
                {
                    uint8_t eq_idx = data[2];
                    if (eq_idx <= 16)
                    {
                        if (app_eq_index_set(SPK_SW_EQ, EQ_AUDIO, app_db.spk_eq_mode, eq_idx) == true)
                        {
                            pdu[length++] = TEAMS_RSP_STATUS_OK;
                        }
                        else
                        {
                            pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                        }
                    }
                    else
                    {
                        pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
                    }
                }
                else
                {
                    pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                }
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_GET_CURRENT_EQ:
        {
            pdu[length++] = TEAMS_CMD_GET_CURRENT_EQ;
            pdu[length++] = TEAMS_RSP_STATUS_OK;
            pdu[length++] = 0x01;
            pdu[length++] = app_cfg_nv.eq_idx;
        }
        break;

    case TEAMS_CMD_SET_TONES_STATUS:
        {
            pdu[length++] = TEAMS_CMD_SET_TONES_STATUS;
            if (data[1] >= 8)
            {
                app_teams_cmd_data.teams_tones_status = *((uint32_t *)(&data[2]));
                app_teams_cmd_data.teams_voice_prompt_status = *((uint32_t *)(&data[6]));
                pdu[length++] = TEAMS_RSP_STATUS_OK;
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_GET_TONES_STATUS:
        {
            pdu[length++] = TEAMS_CMD_GET_TONES_STATUS;
            pdu[length++] = TEAMS_RSP_STATUS_OK;
            pdu[length++] = 0x08;
            pdu[length++] = (uint8_t)(app_teams_cmd_data.teams_tones_status & 0x000000FF);
            pdu[length++] = (uint8_t)((app_teams_cmd_data.teams_tones_status & 0x0000FF00) >> 8);
            pdu[length++] = (uint8_t)((app_teams_cmd_data.teams_tones_status & 0x00FF0000) >> 16);
            pdu[length++] = (uint8_t)((app_teams_cmd_data.teams_tones_status & 0xFF000000) >> 24);
            pdu[length++] = (uint8_t)(app_teams_cmd_data.teams_voice_prompt_status & 0x000000FF);
            pdu[length++] = (uint8_t)((app_teams_cmd_data.teams_voice_prompt_status & 0x0000FF00) >> 8);
            pdu[length++] = (uint8_t)((app_teams_cmd_data.teams_voice_prompt_status & 0x00FF0000) >> 16);
            pdu[length++] = (uint8_t)((app_teams_cmd_data.teams_voice_prompt_status & 0xFF000000) >> 24);
        }
        break;

    case TEAMS_CMD_SET_TONES_VOLUME:
        {
            pdu[length++] = TEAMS_CMD_SET_TONES_VOLUME;
            if (data[1] >= 1)
            {
                if (data[2] < 16)
                {
                    if (voice_prompt_volume_set(data[2]))
                    {
                        pdu[length++] = TEAMS_RSP_STATUS_OK;
                    }
                    else
                    {
                        pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                    }
                }
                else
                {
                    pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
                }
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_GET_TONES_VOLUME:
        {
            pdu[length++] = TEAMS_CMD_GET_TONES_VOLUME;
            pdu[length++] = TEAMS_RSP_STATUS_OK;
            pdu[length++] = 0x01;
            pdu[length++] = voice_prompt_volume_get();
        }
        break;

    case TEAMS_CMD_SET_VOLUME_LIMITATION:
        {
            pdu[length++] = TEAMS_CMD_SET_VOLUME_LIMITATION;
            if (data[1] >= 2)
            {
                app_teams_cmd_data.teams_limitation_mode = data[2];
                uint8_t scale = data[3];
                if (data[2] == 0)
                {
                    scale = 100;
                    teams_dac_gain = 0;
                    app_teams_cmd_data.teams_limitation_mode = data[2];
                    app_teams_cmd_data.teams_limitation_mode_max_volume = scale;
                    pdu[length++] = TEAMS_RSP_STATUS_OK;
                }
                else
                {
                    if ((scale <= 100) && (scale > 0))
                    {
                        teams_dac_gain = 20 * pow(2, 7) * log10(scale / 100.00);
                        app_teams_cmd_data.teams_limitation_mode = data[2];
                        app_teams_cmd_data.teams_limitation_mode_max_volume = scale;
                        pdu[length++] = TEAMS_RSP_STATUS_OK;
                    }
                    else
                    {
                        pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
                    }
                }
                app_teams_cmd_save_info(&app_teams_cmd_data);
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_GET_VOLUME_LIMITATION:
        {
            pdu[length++] = TEAMS_CMD_GET_VOLUME_LIMITATION;
            pdu[length++] = TEAMS_RSP_STATUS_OK;
            pdu[length++] = 0x02;
            pdu[length++] = app_teams_cmd_data.teams_limitation_mode;
            pdu[length++] = app_teams_cmd_data.teams_limitation_mode_max_volume;
        }
        break;

    case TEAMS_CMD_SET_PTT_STATUS:
        {
            pdu[length++] = TEAMS_CMD_SET_PTT_STATUS;
            if (data[1] >= 1)
            {
                if (data[2])
                {
                    app_teams_cmd_data.teams_ptt_status = 1;
                }
                else
                {
                    app_teams_cmd_data.teams_ptt_status = 0;
                }
                app_teams_cmd_save_info(&app_teams_cmd_data);
                pdu[length++] = TEAMS_RSP_STATUS_OK;
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_GET_PTT_STATUS:
        {
            pdu[length++] = TEAMS_CMD_GET_PTT_STATUS;
            pdu[length++] = TEAMS_RSP_STATUS_OK;
            pdu[length++] = 0x01;
            pdu[length++] = app_teams_cmd_data.teams_ptt_status;
        }
        break;

    case TEAMS_CMD_GET_REGION_CODE:
        {
            pdu[length++] = TEAMS_CMD_GET_REGION_CODE;
            ret = app_teams_cmd_get_region_code((const void **)&p_buff, &buff_len);
            if (ret && buff_len == 2)
            {
                pdu[length++] = TEAMS_RSP_STATUS_OK;
                pdu[length++] = 0x02;
                memcpy(&pdu[length], p_buff, 2);
                length += 2;
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                pdu[length++] = 0;
            }
        }
        break;

    case TEAMS_CMD_GET_PAIRED_HOST_INFO:
        {
            pdu[length++] = TEAMS_CMD_GET_PAIRED_HOST_INFO;

            uint8_t pair_idx;
            uint8_t host_name_length;
            uint8_t payload_len;
            uint8_t bond_num = app_bond_b2s_num_get();
            uint8_t host_addr[6];
            uint8_t host_name[40];
            T_TEAMS_DEVICE_TYPE host_type = T_TEAMS_DEVICE_DEFAULT_TYPE;
            app_teams_customized_cmd_reverse_intcpy(host_addr, (const uint8_t *)&data[2], 6);

            if (data[1] >= 6)
            {
                if (bt_bond_index_get(host_addr, &pair_idx))
                {
                    app_teams_bt_policy_master_device_name_get_by_addr(host_addr, host_name, &host_name_length);
                    payload_len = host_name_length + 10;
                    pdu[length++] = TEAMS_RSP_STATUS_OK;
                    pdu[length++] = payload_len;
                    pdu[length++] = bond_num;
                    pdu[length++] = pair_idx + 1;
                    if (app_link_find_br_link(host_addr) != NULL)
                    {
                        pdu[length++] = 0x02;
                    }
                    else
                    {
                        pdu[length++] = 0x01;
                    }

#if F_APP_TEAMS_BT_POLICY
                    host_type = app_teams_bt_policy_get_cod_type_by_addr(host_addr);
#endif
                    if (host_type == T_TEAMS_DEVICE_PHONE_TYPE)
                    {
                        pdu[length++] = 0x03;
                    }
                    else if (host_type == T_TEAMS_DEVICE_COMPUTER_TYPE)
                    {
                        pdu[length++] = 0x02;
                    }
                    else if (host_type == T_TEAMS_DEVICE_DONGLE_TYPE)
                    {
                        pdu[length++] = 0x01;
                    }
                    else
                    {
                        pdu[length++] = 0;
                    }
                    memset(&pdu[length], 0, 5);
                    length += 5;

                    pdu[length++] = host_name_length;
                    memcpy_s(&pdu[length], (PDU_MAX_LENGTH - length), host_name, host_name_length);
                    length += host_name_length;
                }
                else
                {
                    pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
                    pdu[length++] = 0;
                }
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
                pdu[length++] = 0;
            }
        }
        break;

    case TEAMS_CMD_REMOVE_PAIRED_HOST_INFO:
        {
            uint8_t host_addr[6];
            pdu[length++] = TEAMS_CMD_REMOVE_PAIRED_HOST_INFO;
            if (data[1] >= 6)
            {
                app_teams_customized_cmd_reverse_intcpy(host_addr, (const uint8_t *)&data[2], 6);
                if (bt_bond_delete(host_addr))
                {
                    pdu[length++] = TEAMS_RSP_STATUS_OK;
                }
                else
                {
                    pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                }
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_GET_BT_CONNECTION_STATE:
        {
            pdu[length++] = TEAMS_CMD_GET_BT_CONNECTION_STATE;
            pdu[length++] = TEAMS_RSP_STATUS_OK;
            pdu[length++] = 0x38;
            uint8_t bond_num = app_bond_b2s_num_get();

            if (bond_num)
            {
                uint8_t host_addr[6];
                for (uint8_t priority = 1; priority <= bond_num; priority++)
                {
                    if (app_bond_b2s_addr_get(priority, host_addr))
                    {
                        app_teams_customized_cmd_reverse_intcpy(&pdu[length], (const uint8_t *)&host_addr[0], 6);
                        length += 6;
                        if (app_link_find_br_link(host_addr) != NULL)
                        {
                            pdu[length++] = 0x02;
                        }
                        else
                        {
                            pdu[length++] = 0x01;
                        }
                    }
                }
            }
            length = 0x3B;
        }
        break;

    case TEAMS_CMD_SET_BT_CONNECTION_STATE:
        {
            pdu[length++] = TEAMS_CMD_SET_BT_CONNECTION_STATE;
            if (data[1] >= 7)
            {
                uint8_t host_addr[6];
                app_teams_customized_cmd_reverse_intcpy(host_addr, (const uint8_t *)&data[2], 6);

                if (data[8] == 0)
                {
                    if (app_link_find_br_link(host_addr) != NULL)
                    {
                        app_bt_policy_disconnect(host_addr, ALL_PROFILE_MASK);
                        pdu[length++] = TEAMS_RSP_STATUS_OK;
                    }
                    else
                    {
                        pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
                    }
                }
                else if (data[8] == 0x01)
                {
                    uint32_t bond_flag;
                    uint32_t plan_profs;

                    bt_bond_flag_get(host_addr, &bond_flag);
                    if (bond_flag & (BOND_FLAG_HFP | BOND_FLAG_HSP | BOND_FLAG_A2DP))
                    {
                        plan_profs = app_bt_policy_get_profs_by_bond_flag(bond_flag);
                        app_bt_policy_default_connect(host_addr, plan_profs, false);
                        pdu[length++] = TEAMS_RSP_STATUS_OK;
                    }
                    else
                    {
                        pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
                    }
                }
                else
                {
                    pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
                }
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_SET_DEVICE_NAME:
        {
            pdu[length++] = TEAMS_CMD_SET_DEVICE_NAME;

            if (data[1] > 0)
            {
                if ((bt_local_name_set(&data[2], data[1])) &&
                    (!le_set_gap_param(GAP_PARAM_DEVICE_NAME, data[1], &data[2])))
                {
                    uint8_t device_name_length = data[1];
                    if (device_name_length >= TEAMS_MAX_DEVICE_NAME_LENGTH)
                    {
                        device_name_length = TEAMS_MAX_DEVICE_NAME_LENGTH - 1;
                    }

                    APP_PRINT_INFO1("app_teams_handle_customized_cmd: device_name_len %d", device_name_length);

                    memset(app_cfg_nv.device_name_legacy, 0, GAP_DEVICE_NAME_LEN);
                    memcpy(app_cfg_nv.device_name_legacy, &data[2], device_name_length);
                    app_cfg_nv.device_name_legacy[device_name_length] = 0;

                    memset(app_cfg_nv.device_name_le, 0, GAP_DEVICE_NAME_LEN);
                    memcpy(app_cfg_nv.device_name_le, &data[2], device_name_length);
                    app_cfg_nv.device_name_le[device_name_length] = 0;

                    app_cfg_store(app_cfg_nv.device_name_legacy, 40);
                    app_cfg_store(app_cfg_nv.device_name_le, 40);
                    pdu[length++] = TEAMS_RSP_STATUS_OK;
                }
                else
                {
                    pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                }

            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_GET_DEVICE_NAME:
        {
            uint8_t device_name_length = strlen((char *)app_cfg_nv.device_name_legacy);

            if (device_name_length >= TEAMS_MAX_DEVICE_NAME_LENGTH)
            {
                device_name_length = TEAMS_MAX_DEVICE_NAME_LENGTH;
                APP_PRINT_ERROR2("TEAMS_NAME: device_name_length cut from %d to %d",
                                 device_name_length, TEAMS_MAX_DEVICE_NAME_LENGTH);
            }

            APP_PRINT_INFO1("device_len %d", device_name_length);
            pdu[length++] = TEAMS_CMD_GET_DEVICE_NAME;
            pdu[length++] = TEAMS_RSP_STATUS_OK;
            pdu[length++] = device_name_length;

            memcpy(&pdu[length], app_cfg_nv.device_name_legacy, device_name_length);
            length += device_name_length;
        }
        break;

    case TEAMS_CMD_GET_LANGUAGE_LIST:
        {
            uint8_t lcid_flash[4] = {0};
            pdu[length++] = TEAMS_CMD_GET_LANGUAGE_LIST;
            pdu[length++] = TEAMS_RSP_STATUS_OK;
            pdu[length++] = 0x0D;
            pdu[length++] = voice_prompt_language_get() + 1;
            for (uint8_t i = 0; i < 3; i++)
            {
#if F_APP_TEAMS_VP_UPDATE_SUPPORT
                if (hid_vp_update_get_lcid(i, lcid_flash))
                {
                    memcpy(&pdu[length], &lcid_flash[0], 4);
                }
                else
#endif
                {
                    memset(&pdu[length], 0, 4);
                }
                length += 4;
                memset(&lcid_flash[0], 0, 4);
            }
        }
        break;

    case TEAMS_CMD_SET_ACTIVE_LANGUAGE:
        {
            pdu[length++] = TEAMS_CMD_SET_ACTIVE_LANGUAGE;
            if (data[1] >= 4)
            {
                if (app_teams_audio_language_set_by_lcid(&data[2]))
                {
                    pdu[length++] = TEAMS_RSP_STATUS_OK;
                }
                else
                {
                    pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                }
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_USER_CONFIGURATION_RESET:
        {
            pdu[length++] = TEAMS_CMD_USER_CONFIGURATION_RESET;
            if (data[1] >= 1)
            {
#if F_APP_TEAMS_BT_POLICY
                app_db.factory_reset_clear_mode == FACTORY_RESET_CLEAR_CFG;
                app_teams_bt_policy_handle_factory_reset();
#endif
                if (app_teams_cmd_user_config_reset())
                {
                    pdu[length++] = TEAMS_RSP_STATUS_OK;
                }
                else
                {
                    pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                }
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_GET_PRODUCT_NAME:
        {
            pdu[length++] = TEAMS_CMD_GET_PRODUCT_NAME;
            pdu[length++] = TEAMS_RSP_STATUS_OK;
            uint8_t device_name_length = strlen((char *)&app_cfg_const.device_name_legacy_default[0]);
            pdu[length++] = device_name_length;
            memcpy(&pdu[length], &app_cfg_const.device_name_legacy_default[0], device_name_length);
            length += device_name_length;
        }
        break;

    case TEAMS_CMD_GET_DEVICE_MODEL_CODE:
        {
            pdu[length++] = TEAMS_CMD_GET_DEVICE_MODEL_CODE;
            ret = app_teams_cmd_get_device_model((const void **)&p_buff, &buff_len);
            if (ret && buff_len == 4)
            {
                pdu[length++] = TEAMS_RSP_STATUS_OK;
                pdu[length++] = 0x04;
                memcpy(&pdu[length], p_buff, 4);
                length += 4;
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                pdu[length++] = 0;
            }
        }
        break;

    case TEAMS_CMD_SET_CUSTOMIZED_EQ:
        {
            pdu[length++] = TEAMS_CMD_SET_CUSTOMIZED_EQ;
            pdu[length++] = TEAMS_RSP_STATUS_DATA_NOT_READY;
            pdu[length++] = 0;
            // bypass to dsp
        }
        break;

    case TEAMS_CMD_GET_CUSTOMIZED_EQ:
        {
            pdu[length++] = TEAMS_CMD_GET_CUSTOMIZED_EQ;
            pdu[length++] = TEAMS_RSP_STATUS_DATA_NOT_READY;
            pdu[length++] = 0;
            // bypass to dsp
        }
        break;

    case TEAMS_CMD_REMOVE_CUSTOMIZED_EQ:
        {
            pdu[length++] = TEAMS_CMD_REMOVE_CUSTOMIZED_EQ;
            pdu[length++] = TEAMS_RSP_STATUS_DATA_NOT_READY;
            pdu[length++] = 0;
            // bypass to dsp
        }
        break;

    case TEAMS_CMD_GET_ACTIVE_EQ_TYPE:
        {
            pdu[length++] = TEAMS_CMD_GET_ACTIVE_EQ_TYPE;
            pdu[length++] = TEAMS_RSP_STATUS_DATA_NOT_READY;
            pdu[length++] = 0;
            // bypass to dsp
        }
        break;

    case TEAMS_CMD_PLAY_TONE:
        {
            pdu[length++] = TEAMS_CMD_PLAY_TONE;
            if (data[1] >= 8)
            {
                if (app_teams_cmd_data.teams_test_mode == TEAMS_MTE_MODE)
                {
                    uint32_t play_tone = *((uint32_t *)(&data[2]));
                    uint32_t play_vp = *((uint32_t *)(&data[6]));
                    uint32_t tone_count1 = app_teams_customized_cmd_count(play_tone);
                    uint32_t vp_count1 = app_teams_customized_cmd_count(play_vp);
                    uint32_t total_count1 = app_teams_customized_cmd_count(play_tone) + app_teams_customized_cmd_count(
                                                play_vp);
                    if (total_count1 == 1)
                    {
                        pdu[length++] = TEAMS_RSP_STATUS_OK;
                        if (tone_count1 == 1)
                        {
                            for (uint8_t i = 2; i < 12; i++)
                            {
                                if (play_tone & BIT(i))
                                {
                                    //play tone
                                    break;
                                }
                            }
                        }
                        else
                        {
                            for (uint8_t i = 0; i < 20; i++)
                            {
                                if (play_vp & BIT(i))
                                {
                                    //play vp
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
                    }
                }
                else
                {
                    pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                }
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_SET_TEST_MODE:
        {
            pdu[length++] = TEAMS_CMD_SET_TEST_MODE;
            if (data[1] >= 4)
            {
                app_teams_cmd_data.teams_test_mode = data[2];
                app_teams_cmd_save_info(&app_teams_cmd_data);
                pdu[length++] = TEAMS_RSP_STATUS_OK;
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_GET_TEST_MODE:
        {
            pdu[length++] = TEAMS_CMD_GET_TEST_MODE;
            pdu[length++] = TEAMS_RSP_STATUS_OK;
            pdu[length++] = 0x04;
            pdu[length++] = app_teams_cmd_data.teams_test_mode;
            pdu[length++] = 0;
            pdu[length++] = 0;
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_GET_COMPONENT_STATUS:
        {
            pdu[length++] = TEAMS_CMD_GET_COMPONENT_STATUS;

            if (data[1] >= 1)
            {
                uint8_t component_id = data[2];
                switch (component_id)
                {
                case T_TEAMS_BUTTON_STATUS:
                    {
                        pdu[length++] = TEAMS_RSP_STATUS_OK;
                        pdu[length++] = 0x02;
                        pdu[length++] = T_TEAMS_BUTTON_STATUS;
                        pdu[length] = 0;
                        if (key_gpio_get_press_state(6))//mute button
                        {
                            pdu[length] |= BIT(0);
                        }
                        if (key_gpio_get_press_state(4))//hook switch button
                        {
                            pdu[length] |= BIT(1);
                        }
                        if (key_gpio_get_press_state(5))//teams button
                        {
                            pdu[length] |= BIT(2);
                        }
                        if (key_gpio_get_press_state(1))//BT button
                        {
                            pdu[length] |= BIT(7);
                        }
                        length += 1;
                    }
                    break;

                case T_TEAMS_BATTERY_INFO:
                    {
                        pdu[length++] = TEAMS_RSP_STATUS_OK;
                        pdu[length++] = 0x05;
                        pdu[length++] = T_TEAMS_BATTERY_INFO;
                        pdu[length++] = app_db.local_batt_level;
                        uint16_t bat_vol = app_charger_get_bat_vol();
                        int16_t bat_curr = app_charger_get_bat_curr();
                        pdu[length++] = (uint8_t)(bat_vol & 0x00FF);
                        pdu[length++] = (uint8_t)((bat_vol & 0xFF00) >> 8);
                        pdu[length++] = app_cfg_const.battery_capacity / bat_curr;
                    }
                    break;

                case T_TEAMS_LED_STATUS:
                    {
                        pdu[length++] = TEAMS_RSP_STATUS_OK;
                        pdu[length++] = 0x08;
                        pdu[length++] = T_TEAMS_LED_STATUS;
                        uint8_t led0_value = hal_gpio_get_input_level(app_cfg_const.led_0_pinmux);
                        uint8_t led1_value = hal_gpio_get_input_level(app_cfg_const.led_1_pinmux);
                        uint8_t led2_value = hal_gpio_get_input_level(app_cfg_const.led_2_pinmux);
                        if (led1_value)
                        {
                            pdu[length] |= BIT(0);
                            pdu[8] = 100;
                        }
                        if (led2_value)
                        {
                            pdu[length] |= BIT(1);
                            pdu[9] = 100;
                        }
                        if (led0_value)
                        {
                            pdu[length] |= BIT(2);
                            pdu[10] = 100;
                        }
                        length = 10;
                    }
                    break;

                case T_TEAMS_MIC_STATUS:
                    {
                        //TODO
                        pdu[length++] = TEAMS_RSP_STATUS_OK;
                        pdu[length++] = 0x02;
                        pdu[length++] = T_TEAMS_MIC_STATUS;
                    }
                    break;

                case T_TEAMS_ENCODER_STATUS:
                    {
                        pdu[length++] = TEAMS_RSP_STATUS_OK;
                        pdu[length++] = 0x04;
                        pdu[length++] = T_TEAMS_ENCODER_STATUS;

                    }
                    break;

                default:
                    {
                        pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
                        pdu[length++] = 0;
                    }
                    break;
                }
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
                pdu[length++] = 0;
            }
        }
        break;

    case TEAMS_CMD_SET_LED_STATUS:
        {
            pdu[length++] = TEAMS_CMD_SET_LED_STATUS;
            if (data[1] >= 2)
            {
                if (app_teams_cmd_data.teams_test_mode == TEAMS_MTE_MODE)
                {
                    if (app_teams_customized_cmd_set_led_mode(data[2]))
                    {
                        pdu[length++] = TEAMS_RSP_STATUS_OK;
                    }
                    else
                    {
                        pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
                    }
                }
                else
                {
                    pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                }
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_DEVICE_REBOOT:
        {
            pdu[length++] = TEAMS_CMD_DEVICE_REBOOT;
            if (app_teams_cmd_data.teams_test_mode == TEAMS_MTE_MODE)
            {
                pdu[length++] = TEAMS_RSP_STATUS_OK;
                app_cfg_store_all();
                app_device_reboot(1000, RESET_ALL);
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_FAIL;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_FACTORY_RESET:
        {
            pdu[length++] = TEAMS_CMD_FACTORY_RESET;
            if (app_teams_cmd_data.teams_test_mode == TEAMS_MTE_MODE)
            {
                pdu[length++] = TEAMS_RSP_STATUS_OK;
                app_mmi_handle_action(MMI_DEV_FACTORY_RESET);
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_FAIL;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_ENABLE_MIC_EQ:
        {
            pdu[length++] = TEAMS_CMD_ENABLE_MIC_EQ;
            if (data[1] >= 1)
            {
                if (app_teams_cmd_data.teams_test_mode == TEAMS_MTE_MODE)
                {
                    pdu[length++] = TEAMS_RSP_STATUS_OK;
                    if (data[2] == 0)
                    {
                        //TO DO
                    }
                    else
                    {

                    }
                }
                else
                {
                    pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                }
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_ENABLE_SPEAKER_EQ:
        {
            pdu[length++] = TEAMS_CMD_ENABLE_SPEAKER_EQ;
            if (data[1] >= 1)
            {
                if (app_teams_cmd_data.teams_test_mode == TEAMS_MTE_MODE)
                {
                    pdu[length++] = TEAMS_RSP_STATUS_OK;
                    if (data[2] == 0)
                    {
                    }
                    else
                    {

                    }
                }
                else
                {
                    pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                }
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_DISABLE_MIC:
        {
            pdu[length++] = TEAMS_CMD_DISABLE_MIC;
            if (data[1] >= 1)
            {
                if (app_teams_cmd_data.teams_test_mode == TEAMS_MTE_MODE)
                {
                    pdu[length++] = TEAMS_RSP_STATUS_OK;
                    app_audio_mic_switch(data[2]);
                }
                else
                {
                    pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                }
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_FW_FULL_RESET:
        {
            pdu[length++] = TEAMS_CMD_FW_FULL_RESET;
            if (app_teams_cmd_data.teams_test_mode == TEAMS_MTE_MODE)
            {
                pdu[length++] = TEAMS_RSP_STATUS_OK;
                app_mmi_handle_action(MMI_DEV_RESET_TO_UNINITIAL_STATE);
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_FAIL;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_SET_BT_MODE:
        {
            pdu[length++] = TEAMS_CMD_SET_BT_MODE;
            if (data[1] >= 1)
            {
                if (app_teams_cmd_data.teams_test_mode == TEAMS_MTE_MODE)
                {
                    if (data[2] == 0)
                    {
                        app_mmi_handle_action(MMI_DEV_EXIT_PAIRING_MODE);
                        pdu[length++] = TEAMS_RSP_STATUS_OK;
                    }
                    else if (data[2] == 0x01)
                    {
                        app_mmi_handle_action(MMI_DEV_FORCE_ENTER_PAIRING_MODE);
                        pdu[length++] = TEAMS_RSP_STATUS_OK;
                    }
                    else
                    {
                        pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
                    }
                }
                else
                {
                    pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                }
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
            }
            pdu[length++] = 0;
        }
        break;

    case TEAMS_CMD_GET_BT_MODE:
        {
            pdu[length++] = TEAMS_CMD_GET_BT_MODE;
            if (app_teams_cmd_data.teams_test_mode == TEAMS_MTE_MODE)
            {
                pdu[length++] = TEAMS_RSP_STATUS_OK;
                pdu[length++] = 0x01;
                if (app_bt_policy_get_state() == BP_STATE_PAIRING_MODE)
                {
                    pdu[length++] = 0x01;
                }
                else
                {
                    pdu[length++] = 0;
                }
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                pdu[length++] = 0;
            }
        }
        break;

    case TEAMS_CMD_GET_LINK_KEY:
        {
            pdu[length++] = TEAMS_CMD_GET_LINK_KEY;
            if (data[1] >= 6)
            {
                if (app_teams_cmd_data.teams_test_mode == TEAMS_MTE_MODE)
                {
                    uint8_t key_type;
                    uint8_t key[16];
                    uint8_t host_addr[6];

                    app_teams_customized_cmd_reverse_intcpy(host_addr, (const uint8_t *)&data[2], 6);
                    if (bt_bond_key_get(host_addr, key, &key_type))
                    {
                        pdu[length++] = TEAMS_RSP_STATUS_OK;
                        pdu[length++] = 0x16;
                        memcpy(&pdu[length], &data[2], 6);
                        length += 6;
                        memcpy(&pdu[length], key, 16);
                        length += 16;
                    }
                    else
                    {
                        pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
                        pdu[length++] = 0;
                    }
                }
                else
                {
                    pdu[length++] = TEAMS_RSP_STATUS_FAIL;
                    pdu[length++] = 0;
                }
            }
            else
            {
                pdu[length++] = TEAMS_RSP_STATUS_INVALID_PARAMETER;
                pdu[length++] = 0;
            }

        }
        break;

    default:
        {
            APP_PRINT_ERROR0("app_teams_cmd_handle_data: err cmd id");
        }
        break;

    }

    app_teams_cmd_send_response_data(bd_addr, APP_CONFIG_D2H, report_type, report_action, report_source,
                                     rsp_len, pdu);
}

#endif

#endif
