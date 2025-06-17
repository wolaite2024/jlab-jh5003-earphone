#if F_APP_TEAMS_FEATURE_SUPPORT

#include <stdlib.h>
#include <string.h>
#include <os_mem.h>
#include <trace.h>

#include "app_main.h"
#include "app_cfg.h"
#include "app_teams_cmd.h"
#include "hid_api.h"
#include "app_teams_hid.h"
#include "bt_hid_device.h"
#include "btm.h"
#include "app_main.h"
#include "app_led.h"
#include "fmc_api.h"
#include "sysm.h"
#include "ftl.h"
#include <math.h>
#include "app_ipc.h"
#include "app_teams_ext_ftl.h"

#if F_APP_USB_HID_SUPPORT
#include "app_usb_audio_hid.h"
#endif
#if F_APP_TEAMS_CUSTOMIZED_CMD_SUPPORT
#include "app_teams_customized_cmd.h"
#endif

T_TEAMS_CMD_DATA app_teams_cmd_data;
T_DEVICE_CONSTANT_CONFIGURATION device_constant_configuration;
int16_t teams_dac_gain = 0;
static bool send_vgm_flag = true;
uint8_t *app_command_for_usb = NULL;

#if F_APP_TEAMS_CUSTOMIZED_CMD_SUPPORT
bool app_teams_cmd_get_constant_configuration(void)
{
    uint32_t addr_constant_configuration = flash_partition_addr_get(PARTITION_FLASH_BKP_DATA1);
    uint32_t len = sizeof(T_DEVICE_CONSTANT_CONFIGURATION);
    uint8_t *pBuff = (uint8_t *)(&device_constant_configuration);

    memset(pBuff, 0, len);
    if (!fmc_flash_nor_read(addr_constant_configuration, pBuff,
                            sizeof(T_DEVICE_CONSTANT_CONFIGURATION)))
    {
        return false;
    }

    return true;
}

bool app_teams_cmd_set_constant_configuration(uint16_t len, uint8_t *data)
{
    uint32_t addr_constant_configuration = flash_partition_addr_get(PARTITION_FLASH_BKP_DATA1);
    uint32_t size = sizeof(T_DEVICE_CONSTANT_CONFIGURATION);
    bool ret = false;

    if (len < size)
    {
        return ret;
    }

    if (fmc_flash_nor_erase(addr_constant_configuration, FMC_FLASH_NOR_ERASE_SECTOR))
    {
        if (fmc_flash_nor_write(addr_constant_configuration, data,
                                sizeof(T_DEVICE_CONSTANT_CONFIGURATION)))
        {
            ret = true;
        }
        APP_PRINT_TRACE1("constant_configuration:write %d", ret);
    }

    return ret;
}

bool app_teams_cmd_load_info(void *p_data)
{
    uint32_t ret = 0;

    if (sizeof(app_teams_cmd_data) > APP_TEAMS_CMD_INFO_SIZE)
    {
        //save info size > ftl space
        return false;
    }

#if (FTL_POOL_ENABLE == 1)
    ret = app_teams_ext_load_ftl_data(APP_TEAMS_CMD_INFO_ADDR, p_data, sizeof(app_teams_cmd_data));
#else
    ret = ftl_load_from_storage(p_data, APP_TEAMS_CMD_INFO_ADDR, sizeof(app_teams_cmd_data));
#endif
    if (ret != 0)
    {
        ENGAGE_PRINT_TRACE1("app_teams_cmd_load_info: ret %d", ret);
        return false;
    }

    return true;
}

bool app_teams_cmd_save_info(void *p_data)
{
    uint32_t ret = 0;

    if (sizeof(app_teams_cmd_data) > APP_TEAMS_CMD_INFO_SIZE)
    {
        //save info size > ftl space
        return false;
    }

#if (FTL_POOL_ENABLE == 1)
    ret = app_teams_ext_save_ftl_data(APP_TEAMS_CMD_INFO_ADDR, p_data, sizeof(app_teams_cmd_data));
#else
    ret = ftl_save_to_storage(p_data, APP_TEAMS_CMD_INFO_ADDR, sizeof(app_teams_cmd_data));
#endif
    if (ret != 0)
    {
        APP_PRINT_TRACE1("app_teams_cmd_save_info: ret %d", ret);
        return false;
    }

    return true;
}

uint32_t app_teams_cmd_info_reset(void)
{
    app_teams_cmd_data.teams_limitation_mode = 0;
    app_teams_cmd_data.teams_limitation_mode_max_volume = 100;
    app_teams_cmd_data.teams_ptt_status = 1;
    app_teams_cmd_data.teams_test_mode = 0;

    app_teams_cmd_data.teams_tones_status = 0xFFFFFFFF;
    app_teams_cmd_data.teams_voice_prompt_status = 0xFFFFFFFF;

#if (FTL_POOL_ENABLE == 1)
    return app_teams_ext_save_ftl_data(APP_TEAMS_CMD_INFO_ADDR, p_data, sizeof(app_teams_cmd_data));
#else
    return ftl_save_to_storage(p_data, APP_TEAMS_CMD_INFO_ADDR, sizeof(app_teams_cmd_data));
#endif
}

static void app_teams_cmd_factory_reset_cback(uint32_t event, void *msg)
{
    switch (event)
    {
    case APP_DEVICE_IPC_EVT_FACTORY_RESET:
        {
            app_teams_cmd_info_reset();
        }
        break;

    default:
        break;
    }
}

void app_teams_cmd_info_init(void)
{
    app_ipc_subscribe(APP_DEVICE_IPC_TOPIC, app_teams_cmd_factory_reset_cback);

    app_teams_cmd_load_info(&app_teams_cmd_data);
}

bool app_teams_cmd_user_config_reset(void)
{
    bool ret = true;

    app_cfg_reset();

    if (app_cfg_store_all() != 0)
    {
        ret = false;
        APP_PRINT_ERROR0("app_teams_cmd_user_config_reset: save nv cfg data error");
    }

    if (app_teams_cmd_info_reset() != 0)
    {
        ret = false;
    }

    return ret;
}
#endif

void app_teams_cmd_send_response_data(uint8_t *bd_addr, uint8_t report_id, uint8_t report_type,
                                      T_APP_TEAMS_HID_REPORT_ACTION_TYPE report_action,
                                      T_APP_TEAMS_HID_REPORT_SOURCE report_source,
                                      uint16_t len, uint8_t *data)
{
    T_APP_TEAMS_HID_DATA hid_msg;
    uint8_t rsp_status = data[2];
    if (report_source == APP_TEAMS_HID_REPORT_SOURCE_USB)
    {
        app_command_for_usb = os_mem_zalloc(OS_MEM_TYPE_DATA, APP_COMMAND_MAX_FOR_USB);
        memset(app_command_for_usb, 0, APP_COMMAND_MAX_FOR_USB);
        memcpy(app_command_for_usb, data,
               len <= APP_COMMAND_MAX_FOR_USB ? len : APP_COMMAND_MAX_FOR_USB);
#if F_APP_USB_HID_SUPPORT
        if (rsp_status != TEAMS_RSP_STATUS_DATA_NOT_READY)
        {
            app_hid_interrupt_in(app_command_for_usb, APP_COMMAND_MAX_FOR_USB);
        }
#endif
    }
    else
    {
        if (report_source == APP_TEAMS_HID_REPORT_SOURCE_SPP)
        {
            memcpy(hid_msg.teams_spp_data.bd_addr, bd_addr, 6);
            hid_msg.teams_spp_data.data_len = len - 1;
            hid_msg.teams_spp_data.p_data = &data[1];
        }
        else if (report_source == APP_TEAMS_HID_REPORT_SOURCE_CLASSIC_HID)
        {
            teams_rsp_cache = os_mem_zalloc(OS_MEM_TYPE_DATA, teams_rsp_len);
            memcpy(teams_rsp_cache, &data[1], teams_rsp_len);
            memcpy(hid_msg.teams_classic_hid_data.bd_addr, bd_addr, 6);
            hid_msg.teams_classic_hid_data.report_id = APP_CONFIG_D2H;
            hid_msg.teams_classic_hid_data.report_type = report_type;
            hid_msg.teams_classic_hid_data.report_action = report_action;
            hid_msg.teams_classic_hid_data.data_len = len - 1;
            hid_msg.teams_classic_hid_data.p_data = &data[1];
        }
        else
        {
            return ;
        }
        if (rsp_status != TEAMS_RSP_STATUS_DATA_NOT_READY)
        {
            app_teams_hid_send_config_report(APP_CONFIG_D2H, report_source, &hid_msg);
        }
    }
}

uint8_t *app_teams_cmd_get_usb_response(void)
{
    return app_command_for_usb;
}

#if F_APP_TEAMS_CUSTOMIZED_CMD_SUPPORT
bool app_teams_cmd_get_device_capability(const void **p_buff, uint16_t *buff_len)
{
    bool ret = false;

    if (!buff_len || !p_buff)
    {
        return ret;
    }

    ret = app_teams_cmd_get_constant_configuration();
    if (ret)
    {
        *p_buff = (uint8_t *)(&device_constant_configuration.device_capability);
        *buff_len = sizeof(device_constant_configuration.device_capability);
    }
    APP_PRINT_TRACE3(" *buff_len=%d, *p_buff=0x%lx, ret=%d", *buff_len, *p_buff, ret);
    return ret;
}

bool app_teams_cmd_get_device_model(const void **p_buff, uint16_t *buff_len)
{
    bool ret = false;

    if (!buff_len || !p_buff)
    {
        return ret;
    }
    ret = app_teams_cmd_get_constant_configuration();
    if (ret)
    {
        *p_buff = (uint8_t *)(&device_constant_configuration.device_model);
        *buff_len = sizeof(device_constant_configuration.device_model);
    }
    APP_PRINT_TRACE3(" *buff_len=%d, *p_buff=0x%lx, ret=%d", *buff_len, *p_buff, ret);
    return ret;
}

bool app_teams_cmd_get_device_serial_number(const void **p_buff, uint16_t *buff_len)
{
    bool ret = false;

    if (!buff_len || !p_buff)
    {
        return ret;
    }
    ret = app_teams_cmd_get_constant_configuration();
    if (ret)
    {
        *p_buff = (uint8_t *)(&device_constant_configuration.serial_number);
        *buff_len = sizeof(device_constant_configuration.serial_number);
    }
    APP_PRINT_TRACE3(" *buff_len=%d, *p_buff=0x%lx, ret=%d", *buff_len, *p_buff, ret);
    return ret;
}

bool app_teams_cmd_get_color_code(const void **p_buff, uint16_t *buff_len)
{
    bool ret = false;

    if (!buff_len || !p_buff)
    {
        return ret;
    }
    ret = app_teams_cmd_get_constant_configuration();
    if (ret)
    {
        *p_buff = (uint8_t *)(&device_constant_configuration.color_code);
        *buff_len = sizeof(device_constant_configuration.color_code);
    }
    APP_PRINT_TRACE3(" *buff_len=%d, *p_buff=0x%lx, ret=%d", *buff_len, *p_buff, ret);
    return ret;
}

bool app_teams_cmd_get_region_code(const void **p_buff, uint16_t *buff_len)
{
    bool ret = false;

    if (!buff_len || !p_buff)
    {
        return ret;
    }
    ret = app_teams_cmd_get_constant_configuration();
    if (ret)
    {
        *p_buff = (uint8_t *)(&device_constant_configuration.region_code);
        *buff_len = sizeof(device_constant_configuration.region_code);
    }
    APP_PRINT_TRACE3(" *buff_len=%d, *p_buff=0x%lx, ret=%d", *buff_len, *p_buff, ret);
    return ret;
}
#endif

bool app_teams_cmd_get_send_vgm_flag(void)
{
    return send_vgm_flag;
}

void app_teams_cmd_set_send_vgm_flag(bool status)
{
    send_vgm_flag = status;
}

void app_teams_cmd_handle_common_data(uint8_t *bd_addr, uint8_t report_id, uint8_t report_type,
                                      T_APP_TEAMS_HID_REPORT_ACTION_TYPE report_action,
                                      T_APP_TEAMS_HID_REPORT_SOURCE report_source,
                                      uint16_t len, uint8_t *data)
{
    uint8_t current_cmd_id = data[0];
    uint8_t pdu[64] = {0};
    uint8_t length = 0;
    pdu[length++] = APP_CONFIG_D2H;
    APP_PRINT_INFO1("app_teams_handle_common: current_command_id 0x%x", current_cmd_id);

    switch (current_cmd_id)
    {
    case TEAMS_CMD_DEMO:
        {
            pdu[length++] = TEAMS_CMD_DEMO;
            pdu[length++] = TEAMS_RSP_STATUS_OK;
            pdu[length++] = 0x10;
        }
        break;

    default:
        {
            APP_PRINT_ERROR0("app_teams_cmd_handle_data: err common cmd id");
        }
        break;

    }
    app_teams_cmd_send_response_data(bd_addr, APP_CONFIG_D2H, report_type, report_action, report_source,
                                     length, pdu);
}

void app_teams_cmd_handle_data(uint8_t *bd_addr, uint8_t report_id, uint8_t report_type,
                               T_APP_TEAMS_HID_REPORT_ACTION_TYPE report_action,
                               T_APP_TEAMS_HID_REPORT_SOURCE report_source,
                               uint16_t len, uint8_t *data)
{
#if F_APP_TEAMS_CUSTOMIZED_CMD_SUPPORT
    app_teams_customized_cmd_handle_data(bd_addr, report_id, report_type, report_action, report_source,
                                         len,
                                         data);
#else
    app_teams_cmd_handle_common_data(bd_addr, report_id, report_type, report_action, report_source, len,
                                     data);
#endif
}

#endif
